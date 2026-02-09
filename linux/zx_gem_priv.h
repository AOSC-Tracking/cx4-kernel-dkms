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

#ifndef __ZX_GEM_PRIV_H__
#define __ZX_GEM_PRIV_H__
#include <drm/drm_vma_manager.h>
#include <drm/drm_gem.h>

#if DRM_VERSION_CODE >= KERNEL_VERSION(5,4,0)
#include <linux/dma-resv.h>
#define reservation_object  dma_resv
#else
#include <linux/reservation.h>
#endif

#include <linux/dma-buf.h>
#include "zx_types.h"
#include "zx_driver.h"
#include "kernel_import.h"
#include "os_interface.h"
#include "zx_gem_debug.h"

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4, 5, 0) || \
     DRM_VERSION_CODE >= KERNEL_VERSION(4, 5, 0)) && DRM_VERSION_CODE < KERNEL_VERSION(6, 17, 0)
#include <linux/pfn_t.h>
#else
typedef struct {
    u64 val;
} pfn_t;
#endif


#define to_zx_bo(gem) container_of(gem, struct drm_zx_gem_object, base)
#define dmabuf_to_zx_bo(dmabuf) to_zx_bo((struct drm_gem_object*)((dmabuf)->priv))

#if LINUX_VERSION_CODE < KERNEL_VERSION(5,4,0)
#define bo_resv(bo)  ((bo)->resv)
#else
#define bo_resv(bo)  ((bo)->base.resv)
#endif

static inline struct drm_zx_gem_object *zx_gem_get_object(zx_file_t *file, unsigned int handle)
{
    struct drm_gem_object *gem;
    struct drm_file *drm_file = file->parent_file;

    spin_lock(&drm_file->table_lock);
    gem = idr_find(&drm_file->object_idr, handle);
    spin_unlock(&drm_file->table_lock);

    if (!gem)
        return NULL;

    return to_zx_bo(gem);
}

struct drm_zx_driver
{
    struct mutex lock;
    struct drm_file *file_priv;
    struct drm_driver base;
};
#define to_drm_zx_driver(drm_driver) container_of((drm_driver), struct drm_zx_driver, base)

struct drm_zx_gem_object* zx_drm_gem_create_object(zx_card_t *zx, zx_create_allocation_info_t *create);

extern int zx_gem_wait_allocation_idle(zx_file_t *priv, zx_wait_allocation_idle_t *wait);

extern int zx_drm_gem_create_object_ioctl(struct drm_file *file, zx_create_allocation_t *create);

extern int zx_gem_dumb_map_offset(struct drm_file *file, struct drm_device *dev, uint32_t handle, uint64_t *offset);
extern int zx_gem_mmap_gtt_ioctl(struct drm_file *file, zx_drm_gem_map_t *args);
extern int zx_drm_gem_mmap(struct file *filp, struct vm_area_struct *vma);

extern void zx_gem_free_object(struct drm_gem_object *gem_obj);
extern int zx_gem_open(struct drm_gem_object *gem_obj, struct drm_file *file);
#if DRM_VERSION_CODE < KERNEL_VERSION(6,6,0)
extern int zx_gem_prime_fd_to_handle(struct drm_device *dev, struct drm_file *file_priv, int prime_fd, uint32_t *handle);
#endif
extern struct drm_gem_object *zx_gem_prime_import(struct drm_device *dev, struct dma_buf *dma_buf);

#if DRM_VERSION_CODE >= KERNEL_VERSION(5,4,0)
extern struct dma_buf *zx_gem_prime_export(struct drm_gem_object *gem_obj, int flags);
#else
extern struct dma_buf *zx_gem_prime_export(struct drm_device *dev, struct drm_gem_object *gem_obj, int flags);
#endif

#if DRM_VERSION_CODE < KERNEL_VERSION(5, 11, 0)
extern void *zx_gem_prime_vmap(struct drm_gem_object *gem_obj);
extern void zx_gem_prime_vunmap(struct drm_gem_object *gem_obj, void *vaddr);
#endif

extern void zx_drm_gem_dump(struct drm_zx_gem_object *bo);

extern int zx_drm_gem_query_info(zx_card_t *zx, struct drm_zx_gem_object *obj, zx_allocation_info_t *info);

extern int zx_gem_dumb_create(struct drm_file *file, struct drm_device *dev, struct drm_mode_create_dumb *args);

extern signed long zx_gem_object_begin_cpu_access(struct drm_zx_gem_object *allocation, long timeout, int write);
extern void zx_gem_object_end_cpu_access(struct drm_zx_gem_object *allocation, int write);
extern int zx_gem_object_begin_cpu_access_ioctl(struct drm_file *file, zx_drm_gem_begin_cpu_access_t *args);
extern void zx_gem_object_end_cpu_access_ioctl(struct drm_file *file, zx_drm_gem_end_cpu_access_t *args);

extern int zx_gem_set_dbg_info_ioctl(struct drm_file *file, zx_set_gem_dbg_info_t *args);

extern int zx_drm_gem_query_alloc_info(zx_file_t *priv, unsigned int handle, zx_allocation_info_t *info);
extern int zx_get_gem_priv_list(zx_file_t *priv, void **gem_priv_list, unsigned int *handle_list, int cnt);

extern int zx_gem_make_resident_locked(struct drm_file *file, unsigned int handle);
extern int zx_gem_evict_locked(struct drm_file *file, unsigned int handle);
extern void zx_gem_cleanup_resident(struct drm_file *file);

extern zx_cpu_vm_area_t *zx_gem_object_vmap(struct drm_zx_gem_object *obj);
extern void zx_gem_object_vunmap(struct drm_zx_gem_object *obj);

extern int zx_gem_open_ioctl(struct drm_file *file, zx_gem_open_t *open);

static inline struct drm_zx_gem_object *
zx_drm_gem_object_lookup(struct drm_device *dev, struct drm_file *filp,
                      u32 handle)
{
    struct drm_gem_object *obj;
#if DRM_VERSION_CODE < KERNEL_VERSION(4,7,0)
    obj = drm_gem_object_lookup(dev, filp, handle);
#else
    obj = drm_gem_object_lookup(filp, handle);
#endif

    if (!obj)
        return NULL;
    return to_zx_bo(obj);
}

extern const struct vm_operations_struct zx_gem_vm_ops;

#if LINUX_VERSION_CODE < KERNEL_VERSION(4, 17, 0)
typedef int vm_fault_t;

static inline vm_fault_t vmf_insert_mixed(struct vm_area_struct *vma,
                unsigned long addr,
#if DRM_VERSION_CODE >= KERNEL_VERSION(4, 5, 0) && DRM_VERSION_CODE < KERNEL_VERSION(6, 17, 0)
                pfn_t pfn)
#else
                unsigned long pfn)
#endif
{
    int err = vm_insert_mixed(vma, addr, pfn);

    if (err == -ENOMEM)
        return VM_FAULT_OOM;
    if (err < 0 && err != -EBUSY)
        return VM_FAULT_SIGBUS;

    return VM_FAULT_NOPAGE;
}

static inline vm_fault_t vmf_insert_pfn(struct vm_area_struct *vma,
                unsigned long addr, unsigned long pfn)
{
    int err = vm_insert_pfn(vma, addr, pfn);

    if (err == -ENOMEM)
        return VM_FAULT_OOM;
    if (err < 0 && err != -EBUSY)
        return VM_FAULT_SIGBUS;

    return VM_FAULT_NOPAGE;
}

#endif
#endif
