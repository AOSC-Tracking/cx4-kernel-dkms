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

#ifndef _H_ZX_GEM_OBJECT_H
#define _H_ZX_GEM_OBJECT_H

#if DRM_VERSION_CODE >= KERNEL_VERSION(5,5,0)
#include <drm/drm_drv.h>
#else
#include <drm/drmP.h>
#endif

#include <drm/drm_vma_manager.h>
#include <drm/drm_gem.h>
#if DRM_VERSION_CODE >= KERNEL_VERSION(5,4,0)
#include <linux/dma-resv.h>
#define reservation_object  dma_resv
#else
#include <linux/reservation.h>
#endif
#include <linux/dma-buf.h>
#include <linux/completion.h>
#include "zx_types.h"
#include "zx_driver.h"
#include "kernel_import.h"
#include "os_interface.h"
#include "zx_gem_debug.h"
#include "zx_version.h"

struct drm_zx_gem_object;

struct drm_zx_gem_object_ops
{
    int (*get_pages)(struct drm_zx_gem_object *);
    void (*put_pages)(struct drm_zx_gem_object *, struct sg_table *);
    void (*release)(struct drm_zx_gem_object *);
};

struct get_system_pages_args{
    zx_ptr64_t      user_ptr;
    unsigned int    user_buf_size;
};

struct drm_zx_gem_object
{
    struct drm_gem_object base;
    const struct drm_zx_gem_object_ops *ops;

    void   *priv;   // vidmm_allocation_t
    struct
    {
        struct mutex lock;
        int pages_pin_count;
        struct sg_table *pages;
    } mm;

    zx_allocation_info_t info;
    zx_map_argu_t *map_argu;
    zx_cpu_vm_area_t *krnl_vma;

#if LINUX_VERSION_CODE < KERNEL_VERSION(5,4,0)
    struct reservation_object *resv;
#endif
    struct reservation_object __builtin_resv;

    unsigned int prefault_num;
    unsigned int delay_map;
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 11, 0)
    unsigned int vmap_mem_type;
#endif

    //debugfs related things
    unsigned int gid;
    zx_gem_debug_info_t  debug;

    struct work_struct get_pages_work;
    struct completion get_pages_completion;
    struct get_system_pages_args pages_args;
    struct os_pages_memory *pages_mem;

    unsigned int dumb_mapped;
};

void zx_get_pages_work_func(struct work_struct *work);

static inline struct drm_zx_gem_object* zx_gem_object_get(struct drm_zx_gem_object* obj)
{
#if DRM_VERSION_CODE < KERNEL_VERSION(4,12,0)
    drm_gem_object_reference(&obj->base);
#else
    drm_gem_object_get(&obj->base);
#endif
    return obj;
}

static inline void zx_gem_object_put(struct drm_zx_gem_object* obj)
{
#if DRM_VERSION_CODE < KERNEL_VERSION(4,12,0)
    drm_gem_object_unreference_unlocked(&obj->base);
#elif DRM_VERSION_CODE < KERNEL_VERSION(5,9,0)
     drm_gem_object_put_unlocked(&obj->base);
#else
    drm_gem_object_put(&obj->base);
#endif
}

#undef __CONCAT
#undef CONCAT
#define __CONCAT(x,y) x##y
#define CONCAT(x,y)     __CONCAT(x,y)

#endif
