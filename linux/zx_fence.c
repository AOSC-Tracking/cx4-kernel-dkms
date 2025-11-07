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

#include "zx_fence.h"
#include "zx_gem.h"
#include "zx_gem_priv.h"
#include "zx_drm_syncobj.h"
#include "zxgfx_trace.h"
#include "zx_disp.h"
#include "zx_cbios.h"
#if LINUX_VERSION_CODE < KERNEL_VERSION(4,10,0)
#include "zx_dma_fence_array.h"
#else
#include <linux/dma-fence-array.h>
#endif

static const char *engine_name[MAX_ENGINE_COUNT] = {
    "ring0",
    "ring1",
    "ring2",
    "ring3",
    "ring4",
    "ring5",
    "ring6",
    "ring7",
    "ring8",
    "ring9",
    "ring10",
    "ring11"
};

static bool zx_dma_fence_signaled(dma_fence_t *base)
{
    struct zx_dma_fence *fence = to_zx_fence(base);
    unsigned int engine_index = zx_engine_by_fence(fence->driver, &fence->base);

    trace_zxgfx_dma_fence_signaled(fence, engine_index);
    return zx_core_interface->is_fence_back(fence->driver->adapter, zx_engine_by_fence(fence->driver, base), fence->value);
}

static bool zx_dma_fence_enable_signaling(dma_fence_t *base)
{
    bool ret = true;
    struct zx_dma_fence *fence = to_zx_fence(base);
    struct zx_dma_fence_driver *driver = fence->driver;
    struct zx_dma_fence_context *context = zx_context_by_fence(driver, base);
    zx_card_t *zx_card = driver->zx_card;

    trace_zxgfx_dma_fence_enable_signaling(base);

    assert_spin_locked(&context->lock);
    if (zx_dma_fence_signaled(base))
        return false;

    spin_lock(&driver->lock);
    if (zx_dma_fence_signaled(base))
    {
        ret = false;
        goto unlock;
    }
    dma_fence_get(base);
    list_add_tail(&fence->link, &driver->fence_list);
    zx_get_nsecs(&fence->enqueue_time);
    atomic_inc(&driver->fence_count);


    if (fence->value == 0xffffffffffffffffULL)
    {
        fence->signal_mode = ZX_SEND_INTERRUPT;
    }
    else
    {
        atomic_inc(&driver->poll_count);
        fence->signal_mode = ZX_POLL_SIGNAL;
        if (unlikely(trace_zxgfx_fence_back_enabled()))
        {
            goto unlock;
        }
        schedule_work(&zx_card->signal_poll_work);
    }

unlock:
    spin_unlock(&driver->lock);

    return ret;
}

signed long zx_dma_fence_wait(dma_fence_t *base, bool intr, signed long timeout)
{
    trace_zxgfx_dma_fence_wait(base, intr, timeout);

    return dma_fence_default_wait(base, intr, timeout);
}

static void zx_dma_fence_free(struct rcu_head *rcu)
{
    dma_fence_t *base = container_of(rcu, dma_fence_t, rcu);
    struct zx_dma_fence *fence = to_zx_fence(base);
    unsigned int engine_index = zx_engine_by_fence(fence->driver, &fence->base);

    trace_zxgfx_dma_fence_release(fence, engine_index);

    atomic_dec(&fence->driver->fence_alloc_count);
    kmem_cache_free(fence->driver->fence_slab, fence);
}

static void zx_dma_fence_release(dma_fence_t *base)
{
    call_rcu(&base->rcu, zx_dma_fence_free);
}

static const char *zx_dma_fence_get_driver_name(dma_fence_t *fence)
{
    return "zx";
}


static const char *zx_dma_fence_get_timeline_name(dma_fence_t *fence_)
{
    struct zx_dma_fence *fence = to_zx_fence(fence_);

    if (zx_dma_fence_signaled(fence_))
        return "signaled";
    else if (fence->value == fence->initialize_value)
        return "swq";
    else
        return engine_name[zx_engine_by_fence(fence->driver, fence_)];
}

static dma_fence_ops_t zx_dma_fence_ops = {
    .get_driver_name    = zx_dma_fence_get_driver_name,
    .enable_signaling   = zx_dma_fence_enable_signaling,
    .signaled           = zx_dma_fence_signaled,
    .wait               = zx_dma_fence_wait,
    .release            = zx_dma_fence_release,
    .get_timeline_name  = zx_dma_fence_get_timeline_name,
};

void *zx_dma_fence_create(void *driver_, unsigned int engine_index, unsigned long long initialize_value)
{
    unsigned int seq;
    struct zx_dma_fence *fence;
    struct zx_dma_fence_driver *driver = driver_;
    struct zx_dma_fence_context *context = zx_context_by_engine(driver, engine_index);

    fence = kmem_cache_alloc(driver->fence_slab, GFP_KERNEL);
    if (fence == NULL)
        return NULL;
    trace_zxgfx_dma_fence_create(fence, engine_index);

    seq = ++context->sync_seq;
    fence->initialize_value =
    fence->value            = initialize_value;
    fence->driver = driver;
    fence->signal_mode = 0;
    zx_get_nsecs(&fence->create_time);
    INIT_LIST_HEAD(&fence->link);
    dma_fence_init(&fence->base, &zx_dma_fence_ops, &context->lock, context->id, seq);

    atomic_inc(&driver->fence_alloc_count);

    return fence;
}

//add the reservation fence
void zx_dma_fence_attach_bo(struct drm_zx_gem_object *bo, void *fence_, int readonly)
{
    struct zx_dma_fence *fence = fence_;

    reservation_object_lock(bo_resv(bo), NULL);
    if (!readonly)
    {
#if  DRM_VERSION_CODE >= KERNEL_VERSION(5,19,0)
        if (reservation_object_reserve_shared(bo_resv(bo),1) == 0)
#endif
        {
            reservation_object_add_excl_fence(bo_resv(bo), &fence->base);
        }
    }
#if DRM_VERSION_CODE < KERNEL_VERSION(5,0,0)
    else if (reservation_object_reserve_shared(bo_resv(bo)) == 0)
#else
    else if (reservation_object_reserve_shared(bo_resv(bo),1) == 0)
#endif
    {
        reservation_object_add_shared_fence(bo_resv(bo), &fence->base);
    }
    reservation_object_unlock(bo_resv(bo));
}

void zx_dma_fence_update_value(void *fence_, unsigned long long value)
{
    struct zx_dma_fence *fence = fence_;
    struct zx_dma_fence_driver *driver = fence->driver;
    zx_card_t *zx_card = driver->zx_card;

    spin_lock(&driver->lock);
    fence->value = value;
    spin_unlock(&driver->lock);
}
int zx_dma_fence_need_send_irq(void *fence_, unsigned long long value)
{
    struct zx_dma_fence *fence = fence_;
    if (unlikely(trace_zxgfx_fence_back_enabled()))
    {
        return 1;
    }
    else
    {
        return (fence->signal_mode == ZX_SEND_INTERRUPT) ? 1:0;
    }
}


void zx_dma_fence_release_cb(void *fence_)
{
    struct zx_dma_fence *fence = fence_;

    dma_fence_put(&fence->base);
}

void zx_dma_fence_notify_event(void *pdev)
{
    struct zx_dma_fence_driver *driver = NULL;
    struct pci_dev *pci_dev = (struct pci_dev*)pdev;
    struct device *dev = &pci_dev->dev;
    struct drm_device *drm_dev = _to_drm_device(pci_dev);
    zx_card_t *zx = (zx_card_t*)drm_dev->dev_private;

    if(zx){
        driver = zx->fence_drv;
        zx_thread_wake_up(driver->event);
    }
}

void zx_dma_fence_array_callback(dma_fence_t *fence, dma_fence_cb_t *cb)
{
    struct zx_dma_fence_cb *fence_cb = NULL;

    if (cb)
    {
        fence_cb = container_of(cb, struct zx_dma_fence_cb, base);
        fence_cb->call_back(fence_cb->adapter, fence_cb->engine_index);

        zx_free(fence_cb);
    }
 }

int zx_dma_fence_is_signaled(void *dma_fence)
{
    dma_fence_t *fence = (dma_fence_t*)dma_fence;
    int ret = 0;

    if(!fence)
        ret = 1;
    else if(dma_fence_is_signaled(fence))
        ret = 1;

    return ret;
}

void zx_get_bo_wait_fences(struct drm_zx_gem_object *bo, struct zx_dma_fence_context *context, unsigned int write, struct zx_bo_wait_fences *bo_wait_fences)
{
    int ret;
    int i, fence_count = 0,num_fences = 0;
    dma_fence_t **fences = NULL;

    ret = reservation_get_fences(bo_resv(bo), write, &fence_count, &fences);
    zx_assert(ret == 0, "ret:%d", ret);

    bo_wait_fences->fences = fences; // reuse fences space, avoid alloc new array
    for(i = 0; i < fence_count; i++) {
        if(fences[i]->context != context->id) {
            bo_wait_fences->fences[num_fences++] = fences[i];
        }
        else
            dma_fence_put(fences[i]);
    }

    if(fences && !num_fences) {
        kfree(fences);
        bo_wait_fences->fences = NULL;
    }

    bo_wait_fences->num_fences = num_fences;
}

static void zx_fence_dump(struct zx_dma_fence *fence)
{
    zx_info("  DmaFence %p context:%lld value:%lld flags:0x%x\n", fence, fence->base.context, fence->value, fence->base.flags);
}

void zx_check_touch_primary(struct zx_dma_fence_driver* driver, struct drm_zx_gem_object *bo)
{
   int i;
   int touch_primary = 0;
   zx_card_t *zx_card = driver->zx_card;
   disp_info_t* disp_info = (disp_info_t*)zx_card->disp_info;

   if(!disp_info->psr_data->enabled)
       return;

   for (i = 0; i < MAX_CORE_CRTCS; i++)
   {
       if (bo && bo->priv == zx_card->primary_allocation[i])
       {
           spin_lock(&driver->lock);
           if (driver->primary_bo[i])
           {
               zx_gem_object_put(driver->primary_bo[i]);
           }
           driver->primary_bo[i] = zx_gem_object_get(bo);
           spin_unlock(&driver->lock);
           //zx_info("try draw to bo %llx while it's used for flip in crtc %d\n", bo, i);

           psr_acquire_display(disp_info->psr_data, PSR_ONSCREENDRAW_REF, 0);
       }
   }
}

static void zx_touch_primary_done(struct zx_dma_fence_driver *driver)
{
   int i,j, fence_count = 0;
   struct drm_zx_gem_object *bo = NULL;
   dma_fence_t **fences = NULL;
   int ret = 0;
   zx_card_t *zx_card = driver->zx_card;
   disp_info_t* disp_info = (disp_info_t*)zx_card->disp_info;

   if(!disp_info->psr_data->enabled)
       return;

   for (i = 0; i < MAX_CORE_CRTCS; i++)
   {
        bo = driver->primary_bo[i];
        if (bo && bo_resv(bo))
        {
           int touch_primary_done = 1;
           ret = reservation_get_fences(bo_resv(bo), 1, &fence_count, &fences);
           zx_assert(ret == 0, "");

           for (j = 0; j < fence_count; j++)
           {
               if (!zx_dma_fence_signaled(fences[j]))
               {
                   touch_primary_done = 0;
               }
               dma_fence_put(fences[j]);
           }

           if (touch_primary_done)
           {
               spin_lock(&driver->lock);
               zx_gem_object_put(driver->primary_bo[i]);
               driver->primary_bo[i] = NULL;
               spin_unlock(&driver->lock);
               //zx_info("draw to bo %llx in crtc %d has done\n", bo, i);

               psr_release_display(disp_info->psr_data, PSR_ONSCREENDRAW_REF);
           }

           if (fences)
           {
              kfree(fences);
              fences = NULL;
              fence_count = 0;
           }
        }
   }
}

static int zx_dma_fence_event_thread(void *data)
{
    int ret = 0;
    struct list_head process_list;
    struct zx_dma_fence *fence, *tmp;
    struct zx_dma_fence_driver *driver = data;
    int try_freeze_num = 0;

    zx_set_freezable();
    INIT_LIST_HEAD(&process_list);

    do {
        ret = zx_thread_wait(driver->event, driver->timeout_msec);
        zx_touch_primary_done(driver);

try_again1:
        spin_lock(&driver->lock);
        list_splice_init(&driver->fence_list, &process_list);
        spin_unlock(&driver->lock);

try_again2:
        list_for_each_entry_safe(fence, tmp, &process_list, link)
        {
            if (zx_dma_fence_signaled(&fence->base))
            {
                if (fence->signal_mode == ZX_POLL_SIGNAL)
                {
                    atomic_dec(&driver->poll_count);
                }
                list_del(&fence->link);
                atomic_dec(&driver->fence_count);
                dma_fence_signal(&fence->base);
                dma_fence_put(&fence->base);
            }
        }

        if (driver->can_freeze)
        {
            if(zx_freezing())
            {
                if (!list_empty(&process_list))
                {
                    zx_msleep(10); //relase CPU wait 10ms, and try_again

                    if (++try_freeze_num > 100)
                    {
                        zx_info("dma fence event thread! try freezing count %d \n", try_freeze_num);
                        try_freeze_num = 0;
                    }
                    goto try_again2;
                }
                else if(!list_empty(&driver->fence_list))
                {
                    goto try_again1;
                }
                zx_info("sleep fence thread! freezing\n");
            }

            spin_lock(&driver->lock);
            list_splice_init(&process_list, &driver->fence_list);
            spin_unlock(&driver->lock);
            if(zx_try_to_freeze())
                zx_info("sleep dma fence thread!\n");
        }
    } while(!zx_thread_should_stop());

    return 0;
}

/*
 *  Returns <0 if unexpected error, 0 if timeout, or the remaining timeout in jiffies;
 *      if timeout is zero the value one is returned if the fence is already signaled
 *      for consistency with other call
 */
long zx_gem_object_fence_await(struct drm_zx_gem_object *obj, int exclude_self, long timeout, int write)
{
    long ret;
    dma_fence_t **fences = NULL;
    unsigned int fence_count = 0, i;

    ret = (long)reservation_get_fences(bo_resv(obj), write, &fence_count, &fences);
    if (ret)
        return (long)ret;

    /* for case timeout=0, if no fences or all fences signaled, should return 1 */
    ret = timeout ? timeout : 1;
    for (i = 0; i < fence_count; i++)
    {
        if (exclude_self && fences[i]->ops == &zx_dma_fence_ops)
            continue;

        ret = dma_fence_wait_timeout(fences[i], 0, timeout);
        if (ret <= 0)   // <0: error;   ==0: timeout
            break;

        // if wait_timeout==0 then returned timeout==1 means signled, keep wait_timeout=0 for next wait
        if (timeout != 0)
            timeout = ret;
    }

    for (i = 0; i < fence_count; i++)
    {
        dma_fence_put(fences[i]);
    }

    if (fences)
        kfree(fences);

    return ret;
}

int zx_dma_fence_driver_init(void *adapter, struct zx_dma_fence_driver *driver)
{
    int i;
    int engine_count = MAX_ENGINE_COUNT;

    driver->adapter = adapter;
    driver->base_context_id = dma_fence_context_alloc(engine_count);
    driver->context = zx_calloc(sizeof(struct zx_dma_fence_context) * engine_count);
    spin_lock_init(&driver->lock);
    driver->fence_slab = kmem_cache_create(
        "zx_dma_fence", sizeof(struct zx_dma_fence), 0, SLAB_HWCACHE_ALIGN, NULL);
    zx_assert(driver->fence_slab != NULL, "");
    for (i = 0; i < engine_count; i++)
    {
        driver->context[i].id = driver->base_context_id + i;
        driver->context[i].sync_seq = 0;
        spin_lock_init(&driver->context[i].lock);
    }

    atomic_set(&driver->fence_count, 0);
    atomic_set(&driver->poll_count, 0);
    driver->timeout_msec = -1;
    driver->can_freeze = TRUE;
    driver->event = zx_create_event(0);
    atomic_set(&driver->fence_alloc_count, 0);
    INIT_LIST_HEAD(&driver->fence_list);
    driver->os_thread = zx_create_thread(zx_dma_fence_event_thread, driver, "dma_fenced");

    return 0;
}

void zx_dma_fence_driver_fini(struct zx_dma_fence_driver *driver)
{
    zx_destroy_thread(driver->os_thread);

    zx_destroy_event(driver->event);
    driver->event = NULL;

    if(driver->context)
    {
        zx_free(driver->context);
        driver->context = NULL;
    }

    rcu_barrier();
    kmem_cache_destroy(driver->fence_slab);
    driver->fence_slab = NULL;
}

void zx_dma_track_fences_dump(zx_card_t *card)
{
    struct zx_dma_fence_driver *driver = card->fence_drv;
    struct zx_dma_fence *fence;
    unsigned long long now;


    spin_lock(&driver->lock);
    zx_get_nsecs(&now);
    zx_info("now:%lld, fence_alloc_count: %d, fence_count: %d\n", now, atomic_read(&driver->fence_alloc_count), atomic_read(&driver->fence_count));
    list_for_each_entry(fence, &driver->fence_list, link)
    {
        zx_info(" fence:%p value:%lld context:%d seqno:%d create:%lld enqueue:%lld.\n",
                fence, fence->value, fence->base.context, fence->base.seqno, fence->create_time, fence->enqueue_time);
    }
    spin_unlock(&driver->lock);
}

void zx_dma_track_fences_clear(zx_card_t *card)
{
}
