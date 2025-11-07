//*****************************************************************************
//  Copyright (c) 2018 Shanghai Zhaoxin Semiconductor Co., Ltd.
//  All Rights Reserved.
//
//  This is UNPUBLISHED PROPRIETARY SOURCE CODE of Shanghai Zhaoxin Semiconductor Co., Ltd.;
//  the contents of this file may not be disclosed to third parties, copied or
//  duplicated in any form, in whole or in part, without the prior written
//  permission of Shanghai Zhaoxin Semiconductor Co., Ltd.
//
//  The copyright of the source code is protected by the copyright laws of the People's
//  Republic of China and the related laws promulgated by the People's Republic of China
//  and the international covenant(s) ratified by the People's Republic of China.
//*****************************************************************************

#include "zx_drm_syncobj.h"

#include <linux/anon_inodes.h>
#include <linux/file.h>
#include <linux/fs.h>
#include <linux/sync_file.h>
#include <linux/uaccess.h>

#include <drm/drm.h>
#if DRM_VERSION_CODE >= KERNEL_VERSION(4,10,0)
#include <drm/drm_drv.h>
#endif

#if DRM_VERSION_CODE >= KERNEL_VERSION(4,12,0)
#include <drm/drm_file.h>
#else
#include <drm/drmP.h>
#endif

#include <drm/drm_gem.h>
#if DRM_VERSION_CODE >= KERNEL_VERSION(4,10,0)
#include <drm/drm_print.h>
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,11,0)
#include <linux/sched/signal.h>
#else
#include <linux/sched.h>
#endif

#if DRM_VERSION_CODE >= KERNEL_VERSION(4,15,0)
#include <drm/drm_syncobj.h>
#endif

static DEFINE_SPINLOCK(zx_dma_fence_stub_lock);
static dma_fence_t zx_dma_fence_stub;

struct syncobj_wait_entry {
    struct list_head node;
    struct task_struct *task;
    dma_fence_t *fence;
    dma_fence_cb_t fence_cb;
    u64    point;
};

static void syncobj_wait_syncobj_func(struct zx_drm_syncobj *syncobj,
                      struct syncobj_wait_entry *wait);

static const char *zx_dma_fence_stub_get_name(dma_fence_t *fence)
{
    return "stub";
}

static bool zx_dma_fence_stub_enable_signaling(dma_fence_t *fence)
{
    return true;
}

static const dma_fence_ops_t zx_dma_fence_stub_ops = {
    .get_driver_name = zx_dma_fence_stub_get_name,
    .get_timeline_name = zx_dma_fence_stub_get_name,
    .wait = dma_fence_default_wait,
    .enable_signaling = zx_dma_fence_stub_enable_signaling,
};

/**
* zx_dma_fence_get_stub - return a signaled fence
*
* Return a stub fence which is already signaled.
*/
dma_fence_t *zx_dma_fence_get_stub(void)
{
    spin_lock(&zx_dma_fence_stub_lock);
    if(!zx_dma_fence_stub.ops){
        dma_fence_init(&zx_dma_fence_stub, &zx_dma_fence_stub_ops, &zx_dma_fence_stub_lock, 0, 0);
        dma_fence_signal_locked(&zx_dma_fence_stub);
    }
    spin_unlock(&zx_dma_fence_stub_lock);

    return dma_fence_get(&zx_dma_fence_stub);
}

/**
 * zx_drm_syncobj_find - lookup and reference a sync object.
 * @file_private: drm file private pointer
 * @handle: sync object handle to lookup.
 *
 * Returns a reference to the syncobj pointed to by handle or NULL. The
 * reference must be released by calling drm_syncobj_put().
 */
void *zx_drm_syncobj_find(void *drm_file,
                     unsigned int handle)
{
    struct drm_file *drmFile = (struct drm_file*)drm_file;
#if DRM_VERSION_CODE >= KERNEL_VERSION(4,15,0)
    return drm_syncobj_find(drmFile, handle);
#else
    zx_file_t *priv = (zx_file_t*)drmFile->driver_priv;
    struct zx_drm_syncobj *syncobj;

    spin_lock(&priv->syncobj_table_lock);

    /* Check if we currently have a reference on the object */
    syncobj = idr_find(&priv->syncobj_idr, handle);
    if (syncobj)
        zx_drm_syncobj_get(syncobj);

    spin_unlock(&priv->syncobj_table_lock);

    return syncobj;
#endif
}

static void zx_drm_syncobj_fence_add_wait(struct zx_drm_syncobj *syncobj,
                       struct syncobj_wait_entry *wait)
{
    dma_fence_t *fence;

    if (wait->fence)
        return;

    spin_lock(&syncobj->lock);
    /* We've already tried once to get a fence and failed.  Now that we
     * have the lock, try one more time just to be sure we don't add a
     * callback when a fence has already been set.
     */
    fence = dma_fence_get(rcu_dereference_protected(syncobj->fence, 1));
    if (!fence) {
        wait->fence = zx_dma_fence_get_stub();
    } else {
        wait->fence = fence;
    }
    spin_unlock(&syncobj->lock);
}

static void zx_drm_syncobj_remove_wait(struct zx_drm_syncobj *syncobj,
                    struct syncobj_wait_entry *wait)
{
    if (!wait->node.next)
        return;

    spin_lock(&syncobj->lock);
    list_del_init(&wait->node);
    spin_unlock(&syncobj->lock);
}

/**
 * zx_drm_syncobj_replace_fence - replace fence in a sync object.
 * @syncobj: Sync object to replace fence in
 * @fence: fence to install in sync file.
 *
 * This replaces the fence on a sync object.
 */
void zx_drm_syncobj_replace_fence(void *drm_syncobj,
                   void *dma_fence)
{
    dma_fence_t *fence = (dma_fence_t *)dma_fence;
#if DRM_VERSION_CODE >= KERNEL_VERSION(4,15,0)
    struct drm_syncobj *syncobj = (struct drm_syncobj *)drm_syncobj;
#if DRM_VERSION_CODE >= KERNEL_VERSION(4,20,0) && DRM_VERSION_CODE < KERNEL_VERSION(5,0,0)
    // linux 4.20 introduced a new replace_fence interface in commit 9a09a42369a4a37a959c051d8e1a1f948c1529a4,
    // and reverted in linux 5.0.
    drm_syncobj_replace_fence(syncobj, 0, fence);
#else
    drm_syncobj_replace_fence(syncobj, fence);
#endif
#else
    struct zx_drm_syncobj *syncobj = (struct zx_drm_syncobj *)drm_syncobj;
    dma_fence_t *old_fence;
    struct syncobj_wait_entry *cur = NULL, *tmp;

    if (fence)
        dma_fence_get(fence);

    spin_lock(&syncobj->lock);

    old_fence = rcu_dereference_protected(syncobj->fence,
                          lockdep_is_held(&syncobj->lock));
    rcu_assign_pointer(syncobj->fence, fence);

    if (fence != old_fence) {
        list_for_each_entry_safe(cur, tmp, &syncobj->cb_list, node) {
            syncobj_wait_syncobj_func(syncobj, cur);
        }
    }

    spin_unlock(&syncobj->lock);

    dma_fence_put(old_fence);
#endif

}

/**
 * zx_drm_syncobj_assign_null_handle - assign a stub fence to the sync object
 * @syncobj: sync object to assign the fence on
 *
 * Assign a already signaled stub fence to the sync object.
 */
static void zx_drm_syncobj_assign_null_handle(struct zx_drm_syncobj *syncobj)
{
    dma_fence_t *fence = zx_dma_fence_get_stub();

    zx_drm_syncobj_replace_fence(syncobj, fence);
    dma_fence_put(fence);
}

/* 5s default for wait submission */
#define DRM_SYNCOBJ_WAIT_FOR_SUBMIT_TIMEOUT 5000000000ULL

/**
 * zx_drm_syncobj_free - free a sync object.
 * @kref: kref to free.
 *
 * Only to be called from kref_put in drm_syncobj_put.
 */
void zx_drm_syncobj_free(struct kref *kref)
{
    struct zx_drm_syncobj *syncobj = container_of(kref,
                           struct zx_drm_syncobj,
                           refcount);
    zx_drm_syncobj_replace_fence(syncobj, NULL);
    kfree(syncobj);
}

/**
 * zx_drm_syncobj_create - create a new syncobj
 * @out_syncobj: returned syncobj
 * @flags: DRM_SYNCOBJ_* flags
 * @fence: if non-NULL, the syncobj will represent this fence
 *
 * This is the first function to create a sync object. After creating, drivers
 * probably want to make it available to userspace, either through
 * zx_drm_syncobj_get_handle() or zx_drm_syncobj_get_fd().
 *
 * Returns 0 on success or a negative error value on failure.
 */
int zx_drm_syncobj_create(struct zx_drm_syncobj **out_syncobj, uint32_t flags,
               dma_fence_t *fence)
{
    struct zx_drm_syncobj *syncobj;

    syncobj = kzalloc(sizeof(struct zx_drm_syncobj), GFP_KERNEL);
    if (!syncobj)
        return -ENOMEM;

    kref_init(&syncobj->refcount);
    INIT_LIST_HEAD(&syncobj->cb_list);
    spin_lock_init(&syncobj->lock);

    if (flags & ZX_DRM_SYNCOBJ_CREATE_SIGNALED)
        zx_drm_syncobj_assign_null_handle(syncobj);

    if (fence)
        zx_drm_syncobj_replace_fence(syncobj, fence);

    *out_syncobj = syncobj;
    return 0;
}

/**
 * zx_drm_syncobj_get_handle - get a handle from a syncobj
 * @file_private: drm file private pointer
 * @syncobj: Sync object to export
 * @handle: out parameter with the new handle
 *
 * Exports a sync object created with zx_drm_syncobj_create() as a handle on
 * @file_private to userspace.
 *
 * Returns 0 on success or a negative error value on failure.
 */
int zx_drm_syncobj_get_handle(zx_file_t *priv,
               struct zx_drm_syncobj *syncobj, u32 *handle)
{
    int ret;

    /* take a reference to put in the idr */
    zx_drm_syncobj_get(syncobj);

    idr_preload(GFP_KERNEL);
    spin_lock(&priv->syncobj_table_lock);
    ret = idr_alloc_cyclic(&priv->syncobj_idr, syncobj, 1, 0, GFP_NOWAIT);
    spin_unlock(&priv->syncobj_table_lock);

    idr_preload_end();

    if (ret < 0) {
        zx_drm_syncobj_put(syncobj);
        return ret;
    }

    *handle = ret;
    return 0;
}

static int zx_drm_syncobj_create_as_handle(zx_file_t *priv,
                    u32 *handle, uint32_t flags)
{
    int ret;
    struct zx_drm_syncobj *syncobj;

    ret = zx_drm_syncobj_create(&syncobj, flags, NULL);
    if (ret)
        return ret;

    ret = zx_drm_syncobj_get_handle(priv, syncobj, handle);

    zx_drm_syncobj_put(syncobj);

    return ret;
}

static int zx_drm_syncobj_destroy(zx_file_t *priv,
                   u32 handle)
{
    struct zx_drm_syncobj *syncobj;

    spin_lock(&priv->syncobj_table_lock);
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,11,0)
    syncobj = idr_remove(&priv->syncobj_idr, handle);
#else
    syncobj = idr_find(&priv->syncobj_idr, handle);
    idr_remove(&priv->syncobj_idr, handle);
#endif
    spin_unlock(&priv->syncobj_table_lock);

    if (!syncobj)
        return -EINVAL;

    zx_drm_syncobj_put(syncobj);
    return 0;
}

int
zx_drm_syncobj_release_handle(int id, void *ptr, void *data)
{
    struct zx_drm_syncobj *syncobj = ptr;

    zx_drm_syncobj_put(syncobj);
    return 0;
}

int
zx_drm_syncobj_create_ioctl(struct drm_device *dev, void *data,struct drm_file *filp)
{
    zx_file_t                    *priv   = filp->driver_priv;
    struct zx_drm_syncobj_create *create = (struct zx_drm_syncobj_create *)data;

    /* no valid flags yet */
    if (create->flags & ~ZX_DRM_SYNCOBJ_CREATE_SIGNALED)
        return -EINVAL;

    zx_drm_syncobj_create_as_handle(priv, &create->handle, create->flags);

    return 0;
}

int
zx_drm_syncobj_destroy_ioctl(struct drm_device *dev, void *data,struct drm_file *filp)
{
    zx_file_t                     *priv = filp->driver_priv;
    struct zx_drm_syncobj_destroy *destroy = (struct zx_drm_syncobj_destroy *)data;

    /* make sure padding is empty */
    if (destroy->pad)
        return -EINVAL;

    return zx_drm_syncobj_destroy(priv, destroy->handle);
}

static void syncobj_wait_fence_func(dma_fence_t *fence,
                    dma_fence_cb_t *cb)
{
    struct syncobj_wait_entry *wait =
        container_of(cb, struct syncobj_wait_entry, fence_cb);

    wake_up_process(wait->task);
}

static void syncobj_wait_syncobj_func(struct zx_drm_syncobj *syncobj,
                      struct syncobj_wait_entry *wait)
{
    dma_fence_t *fence;

    /* This happens inside the syncobj lock */
    fence = rcu_dereference_protected(syncobj->fence,
                      lockdep_is_held(&syncobj->lock));
    dma_fence_get(fence);
    if (!fence) {
        wait->fence = zx_dma_fence_get_stub();
    } else {
        wait->fence = fence;
    }

    wake_up_process(wait->task);
    list_del_init(&wait->node);
}

static signed long zx_drm_syncobj_array_wait_timeout(struct zx_drm_syncobj **syncobjs,
                          void __user *user_points,
                          uint32_t count,
                          uint32_t flags,
                          signed long timeout,
                          uint32_t *idx)
{
    struct syncobj_wait_entry *entries;
    dma_fence_t *fence;
    uint64_t *points;
    uint32_t signaled_count, i;

    points = kmalloc_array(count, sizeof(*points), GFP_KERNEL);
    if (points == NULL)
        return -ENOMEM;

    if (!user_points) {
        memset(points, 0, count * sizeof(uint64_t));

    } else if (zx_copy_from_user(points, user_points,
                  sizeof(uint64_t) * count)) {
        timeout = -EFAULT;
        goto err_free_points;
    }

    entries = kcalloc(count, sizeof(*entries), GFP_KERNEL);
    if (!entries) {
        timeout = -ENOMEM;
        goto err_free_points;
    }
    /* Walk the list of sync objects and initialize entries.  We do
     * this up-front so that we can properly return -EINVAL if there is
     * a syncobj with a missing fence and then never have the chance of
     * returning -EINVAL again.
     */
    signaled_count = 0;
    for (i = 0; i < count; ++i) {
        dma_fence_t *fence;

        entries[i].task = current;
        entries[i].point = points[i];
        fence = zx_drm_syncobj_fence_get(syncobjs[i]);
        /*
        if (!fence || dma_fence_chain_find_seqno(&fence, points[i])) {
            dma_fence_put(fence);
            if (flags & DRM_SYNCOBJ_WAIT_FLAGS_WAIT_FOR_SUBMIT) {
                continue;
            } else {
                timeout = -EINVAL;
                goto cleanup_entries;
            }
        }
        */

        if (fence)
            entries[i].fence = fence;
        else
            entries[i].fence = zx_dma_fence_get_stub();

        if ((flags & ZX_DRM_SYNCOBJ_WAIT_FLAGS_WAIT_AVAILABLE) ||
            dma_fence_is_signaled(entries[i].fence)) {
            if (signaled_count == 0 && idx)
                *idx = i;
            signaled_count++;
        }
    }

    if (signaled_count == count ||
        (signaled_count > 0 &&
         !(flags & ZX_DRM_SYNCOBJ_WAIT_FLAGS_WAIT_ALL)))
        goto cleanup_entries;

    /* There's a very annoying laxness in the dma_fence API here, in
     * that backends are not required to automatically report when a
     * fence is signaled prior to fence->ops->enable_signaling() being
     * called.  So here if we fail to match signaled_count, we need to
     * fallthough and try a 0 timeout wait!
     */

    if (flags & ZX_DRM_SYNCOBJ_WAIT_FLAGS_WAIT_FOR_SUBMIT) {
        for (i = 0; i < count; ++i)
            zx_drm_syncobj_fence_add_wait(syncobjs[i], &entries[i]);
    }

    do {
        set_current_state(TASK_INTERRUPTIBLE);

        signaled_count = 0;
        for (i = 0; i < count; ++i) {
            fence = entries[i].fence;
            if (!fence)
                continue;

            if ((flags & ZX_DRM_SYNCOBJ_WAIT_FLAGS_WAIT_AVAILABLE) ||
                dma_fence_is_signaled(fence) ||
                (!entries[i].fence_cb.func &&
                 dma_fence_add_callback(fence,
                            &entries[i].fence_cb,
                            syncobj_wait_fence_func))) {
                /* The fence has been signaled */
                if (flags & ZX_DRM_SYNCOBJ_WAIT_FLAGS_WAIT_ALL) {
                    signaled_count++;
                } else {
                    if (idx)
                        *idx = i;
                    goto done_waiting;
                }
            }
        }

        if (signaled_count == count)
            goto done_waiting;

        if (timeout == 0) {
            timeout = -ETIME;
            goto done_waiting;
        }

        if (signal_pending(current)) {
            timeout = -ERESTARTSYS;
            goto done_waiting;
        }

        timeout = schedule_timeout(timeout);
    } while (1);

done_waiting:
    __set_current_state(TASK_RUNNING);

cleanup_entries:
    for (i = 0; i < count; ++i) {
        zx_drm_syncobj_remove_wait(syncobjs[i], &entries[i]);
        if (entries[i].fence_cb.func)
            dma_fence_remove_callback(entries[i].fence,
                          &entries[i].fence_cb);
        dma_fence_put(entries[i].fence);
    }
    kfree(entries);

err_free_points:
    kfree(points);

    return timeout;
}

/**
 * zx_drm_timeout_abs_to_jiffies - calculate jiffies timeout from absolute value
 *
 * @timeout_nsec: timeout nsec component in ns, 0 for poll
 *
 * Calculate the timeout in jiffies from an absolute time in sec/nsec.
 */
signed long zx_drm_timeout_abs_to_jiffies(int64_t timeout_nsec)
{
    ktime_t abs_timeout, now;
    u64 timeout_ns, timeout_jiffies64;

    /* make 0 timeout means poll - absolute 0 doesn't seem valid */
    if (timeout_nsec == 0)
        return 0;

    abs_timeout = ns_to_ktime(timeout_nsec);
    now = ktime_get();

    if (!ktime_after(abs_timeout, now))
        return 0;

    timeout_ns = ktime_to_ns(ktime_sub(abs_timeout, now));

    timeout_jiffies64 = nsecs_to_jiffies64(timeout_ns);
    /*  clamp timeout to avoid infinite timeout */
    if (timeout_jiffies64 >= MAX_SCHEDULE_TIMEOUT - 1)
        return MAX_SCHEDULE_TIMEOUT - 1;

    return timeout_jiffies64 + 1;
}

static int zx_drm_syncobj_array_wait(zx_file_t *priv,
                  struct zx_drm_syncobj_wait *wait,
                  struct zx_drm_syncobj_timeline_wait *timeline_wait,
                  struct zx_drm_syncobj **syncobjs, bool timeline)
{
    signed long timeout = 0;
    uint32_t first = ~0;

    if (!timeline) {
        timeout = zx_drm_timeout_abs_to_jiffies(wait->timeout_nsec);
        timeout = zx_drm_syncobj_array_wait_timeout(syncobjs,
                             NULL,
                             wait->count_handles,
                             wait->flags,
                             timeout, &first);
        if (timeout < 0)
            return timeout;
        wait->first_signaled = first;
    } else if(timeline_wait) {
        timeout = zx_drm_timeout_abs_to_jiffies(timeline_wait->timeout_nsec);
        timeout = zx_drm_syncobj_array_wait_timeout(syncobjs,
                             u64_to_user_ptr(timeline_wait->points),
                             timeline_wait->count_handles,
                             timeline_wait->flags,
                             timeout, &first);
        if (timeout < 0)
            return timeout;
        timeline_wait->first_signaled = first;
    }
    return 0;
}

static int zx_drm_syncobj_array_find(zx_file_t *priv,
                  void __user *user_handles,
                  uint32_t count_handles,
                  struct zx_drm_syncobj ***syncobjs_out)
{
    uint32_t i, *handles;
    struct zx_drm_syncobj **syncobjs;
    int ret;

    handles = kmalloc_array(count_handles, sizeof(*handles), GFP_KERNEL);
    if (handles == NULL)
        return -ENOMEM;

    if (zx_copy_from_user(handles, user_handles,
               sizeof(uint32_t) * count_handles)) {
        ret = -EFAULT;
        goto err_free_handles;
    }

    syncobjs = kmalloc_array(count_handles, sizeof(*syncobjs), GFP_KERNEL);
    if (syncobjs == NULL) {
        ret = -ENOMEM;
        goto err_free_handles;
    }

    for (i = 0; i < count_handles; i++) {
        syncobjs[i] = zx_drm_syncobj_find(priv->parent_file, handles[i]);
        if (!syncobjs[i]) {
            ret = -ENOENT;
            goto err_put_syncobjs;
        }
    }

    kfree(handles);
    *syncobjs_out = syncobjs;
    return 0;

err_put_syncobjs:
    while (i-- > 0)
        zx_drm_syncobj_put(syncobjs[i]);
    kfree(syncobjs);
err_free_handles:
    kfree(handles);

    return ret;
}

static void zx_drm_syncobj_array_free(struct zx_drm_syncobj **syncobjs,
                   uint32_t count)
{
    uint32_t i;

    for (i = 0; i < count; i++)
        zx_drm_syncobj_put(syncobjs[i]);
    kfree(syncobjs);
}

int
zx_drm_syncobj_wait_ioctl(struct drm_device *dev, void *data,struct drm_file *filp)
{
    zx_file_t                  *priv = filp->driver_priv;
    struct zx_drm_syncobj_wait *wait = (struct zx_drm_syncobj_wait *)data;
    struct zx_drm_syncobj **syncobjs;
    int ret = 0;

    if (wait->flags & ~(ZX_DRM_SYNCOBJ_WAIT_FLAGS_WAIT_ALL |
                ZX_DRM_SYNCOBJ_WAIT_FLAGS_WAIT_FOR_SUBMIT))
        return -EINVAL;

    if (wait->count_handles == 0)
        return -EINVAL;

    ret = zx_drm_syncobj_array_find(priv,
                     u64_to_user_ptr(wait->handles),
                     wait->count_handles,
                     &syncobjs);
    if (ret < 0)
        return ret;

    ret = zx_drm_syncobj_array_wait(priv,
                     wait, NULL, syncobjs, false);

    zx_drm_syncobj_array_free(syncobjs, wait->count_handles);

    return ret;
}

int
zx_drm_syncobj_reset_ioctl(struct drm_device *dev, void *data,struct drm_file *filp)
{
    zx_file_t                   *priv = filp->driver_priv;
    struct zx_drm_syncobj_array *array = (struct zx_drm_syncobj_array *)data;
    struct zx_drm_syncobj **syncobjs;
    uint32_t i;
    int ret;

    if (array->pad != 0)
        return -EINVAL;

    if (array->count_handles == 0)
        return -EINVAL;

    ret = zx_drm_syncobj_array_find(priv,
                     u64_to_user_ptr(array->handles),
                     array->count_handles,
                     &syncobjs);
    if (ret < 0)
        return ret;

    for (i = 0; i < array->count_handles; i++)
        zx_drm_syncobj_replace_fence(syncobjs[i], NULL);

    zx_drm_syncobj_array_free(syncobjs, array->count_handles);

    return 0;
}

/**
 * zx_drm_syncobj_fence_get - get a reference to a fence in a sync object
 * @syncobj: sync object.
 *
 * This acquires additional reference to &drm_syncobj.fence contained in @obj,
 * if not NULL. It is illegal to call this without already holding a reference.
 * No locks required.
 *
 * Returns:
 * Either the fence of @obj or NULL if there's none.
 */
void *
zx_drm_syncobj_fence_get(void *drm_syncobj)
{
#if DRM_VERSION_CODE >= KERNEL_VERSION(4,15,0)
    struct drm_syncobj *syncobj = (struct drm_syncobj*)drm_syncobj;
    return drm_syncobj_fence_get(syncobj);
#else
    struct zx_drm_syncobj *syncobj = (struct zx_drm_syncobj *)drm_syncobj;
    dma_fence_t *fence;

    rcu_read_lock();
    fence = dma_fence_get_rcu_safe(&syncobj->fence);
    rcu_read_unlock();

    return fence;
#endif
}

int zx_drm_syncobj_get_status_ioctl(struct drm_device *dev, void *data,struct drm_file *filp)
{
    zx_file_t               *priv = filp->driver_priv;
    zx_drm_syncobj_status_t *sync_status = (zx_drm_syncobj_status_t *)data;
    zx_drm_syncobj_t *syncobj = NULL;
    dma_fence_t *fence = NULL;

    syncobj = (zx_drm_syncobj_t*)zx_drm_syncobj_find(priv->parent_file, sync_status->handle);

    fence = (dma_fence_t*)zx_drm_syncobj_fence_get((void*)syncobj);

    if(fence && dma_fence_is_signaled(fence))
        sync_status->value = 1;

    if(fence)
        dma_fence_put(fence);
    zx_drm_syncobj_put(syncobj);

    return 0;
}

int zx_drm_syncobj_is_signaled(void *drm_syncobj)
{
    int ret = 0;
    dma_fence_t *fence = NULL;
#if DRM_VERSION_CODE >= KERNEL_VERSION(4,15,0)
    struct drm_syncobj *syncobj = (struct drm_syncobj*)drm_syncobj;
    spin_lock(&syncobj->lock);
    fence = drm_syncobj_fence_get(syncobj);
#else
    struct zx_drm_syncobj *syncobj = (struct zx_drm_syncobj *)drm_syncobj;

    spin_lock(&syncobj->lock);
    rcu_read_lock();
    fence = dma_fence_get_rcu_safe(&syncobj->fence);
    rcu_read_unlock();
#endif

    if(!fence){
        ret = 1;
    }else if(dma_fence_is_signaled(fence)){
        ret = 1;
    }

    spin_unlock(&syncobj->lock);
    dma_fence_put(fence);

    return ret;
}

void* zx_drm_syncobj_wrap_dma_fence(void *dma_fence)
{
    int ret = 0;
    dma_fence_t *fence = (dma_fence_t*)dma_fence;
#if DRM_VERSION_CODE >= KERNEL_VERSION(4,15,0)
    struct drm_syncobj *syncobj = NULL;
    ret = drm_syncobj_create(&syncobj, 0, fence);
    if(ret)
        return NULL;
    else
        return syncobj;
#else
    struct zx_drm_syncobj *syncobj = NULL;
    ret = zx_drm_syncobj_create(&syncobj, 0, fence);
    if(ret)
        return NULL;
    else
        return syncobj;
#endif
}

void zx_drm_syncobj_put_hook(void *drm_syncobj)
{
#if DRM_VERSION_CODE >= KERNEL_VERSION(4,15,0)
    struct drm_syncobj *syncobj = (struct drm_syncobj*)drm_syncobj;
    drm_syncobj_put(syncobj);
#else
    struct zx_drm_syncobj *syncobj = (struct zx_drm_syncobj*)drm_syncobj;
    zx_drm_syncobj_put(syncobj);
#endif
}
