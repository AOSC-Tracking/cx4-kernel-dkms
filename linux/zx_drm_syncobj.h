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

#ifndef __ZX_DRM_SYNCOBJ_H__
#define __ZX_DRM_SYNCOBJ_H__

#include "zx.h"
#include "zx_driver.h"

#include "zx_fence.h"

/**
 * struct zx_drm_syncobj - sync object.
 *
 * This structure defines a generic sync object which wraps a &dma_fence.
 */
typedef struct zx_drm_syncobj {
    /**
     * @refcount: Reference count of this object.
     */
    struct kref refcount;
    /**
     * @fence:
     * NULL or a pointer to the fence bound to this object.
     *
     * This field should not be used directly. Use drm_syncobj_fence_get()
     * and drm_syncobj_replace_fence() instead.
     */
    dma_fence_t __rcu *fence;
    /**
     * @cb_list: List of callbacks to call when the &fence gets replaced.
     */
    struct list_head cb_list;
    /**
     * @lock: Protects &cb_list and write-locks &fence.
     */
    spinlock_t lock;
    /**
     * @file: A file backing for this syncobj.
     */
    struct file *file;
} zx_drm_syncobj_t;

void zx_drm_syncobj_free(struct kref *kref);

/**
 * zx_drm_syncobj_get - acquire a syncobj reference
 * @obj: sync object
 *
 * This acquires an additional reference to @obj. It is illegal to call this
 * without already holding a reference. No locks required.
 */
static inline void
zx_drm_syncobj_get(struct zx_drm_syncobj *obj)
{
    kref_get(&obj->refcount);
}

/**
 * zx_drm_syncobj_put - release a reference to a sync object.
 * @obj: sync object.
 */
static inline void
zx_drm_syncobj_put(struct zx_drm_syncobj *obj)
{
    kref_put(&obj->refcount, zx_drm_syncobj_free);
}

extern void *zx_drm_syncobj_find(void *drm_file,
                     unsigned int handle);
extern void zx_drm_syncobj_replace_fence(void *drm_syncobj,
                   void *dma_fence);
extern void *zx_drm_syncobj_fence_get(void *drm_syncobj);
extern int zx_drm_syncobj_is_signaled(void *drm_syncobj);
extern void *zx_drm_syncobj_wrap_dma_fence(void *dma_fence);
extern void zx_drm_syncobj_put_hook(void *drm_syncobj);
void zx_drm_syncobj_free(struct kref *kref);
int zx_drm_syncobj_create(struct zx_drm_syncobj **out_syncobj, unsigned int flags,
               dma_fence_t *fence);
int zx_drm_syncobj_get_handle(zx_file_t *priv,
               struct zx_drm_syncobj *syncobj, unsigned int *handle);
int zx_drm_syncobj_get_fd(struct zx_drm_syncobj *syncobj, int *p_fd);

#endif
