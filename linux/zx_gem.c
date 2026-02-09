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

#include "zx.h"
#include "zx_debugfs.h"
#include "zx_gem.h"
#include "zx_gem_priv.h"
#include "zx_fence.h"
#include "zx_driver.h"
#include "zxgfx_trace.h"
#include "os_interface.h"
#include "zx_disp.h"
#include "zx_cbios.h"

#if DRM_VERSION_CODE >= KERNEL_VERSION(5, 16, 0)
#if DRM_VERSION_CODE < KERNEL_VERSION(6, 13, 0)
MODULE_IMPORT_NS(DMA_BUF);
#else
MODULE_IMPORT_NS("DMA_BUF");
#endif
#endif

extern struct os_pages_memory* zx_allocate_pages_memory_struct(int page_cnt, struct sg_table *st);
extern void zx_pages_memory_extract_st(struct os_pages_memory *memory);
extern unsigned char zx_validate_page_cache(struct os_pages_memory *memory, int start_page, int end_page, unsigned char request_cache_type);

extern struct mutex g_pages_memory_lock;
extern struct list_head g_pages_memory_list;

int zx_gem_debugfs_add_object(struct drm_zx_gem_object *obj);
void zx_gem_debugfs_remove_object(struct drm_zx_gem_object *obj);

#if DRM_VERSION_CODE < KERNEL_VERSION(5,18,0)
#define DMABUF_PREFIX(x) dma_buf_##x
#else
#define DMABUF_PREFIX(x) iosys_##x
#endif

#if DRM_VERSION_CODE < KERNEL_VERSION(5,11,0)
#define DMA_BUF_MAP_CLEAR() do {} while(0)
#else
#define DMA_BUF_MAP_CLEAR() do { DMABUF_PREFIX(map_clear)(map); } while(0)
#endif

#define ZX_DEFAULT_PREFAULT_NUM 16
#define NUM_PAGES(x) (((x) + PAGE_SIZE-1) / PAGE_SIZE)

static int zx_gem_object_pin_pages(struct drm_zx_gem_object *obj)
{
    int err = 0;

    mutex_lock(&obj->mm.lock);
    if (++obj->mm.pages_pin_count == 1)
    {
        zx_assert(!obj->mm.pages, "");
        err = obj->ops->get_pages(obj);
        if (err)
            --obj->mm.pages_pin_count;
    }
    mutex_unlock(&obj->mm.lock);
    return err;
}

static void zx_gem_object_unpin_pages(struct drm_zx_gem_object *obj)
{
    struct sg_table *pages;

    mutex_lock(&obj->mm.lock);
    zx_assert(obj->mm.pages_pin_count > 0, "");
    if (--obj->mm.pages_pin_count == 0)
    {
        pages = obj->mm.pages;
        obj->mm.pages = NULL;
        zx_assert(pages != NULL, "");
        if (!IS_ERR(pages))
            obj->ops->put_pages(obj, pages);
    }
    mutex_unlock(&obj->mm.lock);
}

void zx_gem_free_object(struct drm_gem_object *gem_obj)
{
    struct drm_zx_gem_object *obj = to_zx_bo(gem_obj);
    zx_card_t *zx = obj->base.dev->dev_private;

    drm_gem_object_release(&obj->base);

    if (obj->ops->release)
    {
        obj->ops->release(obj);
    }

    reservation_object_fini(&obj->__builtin_resv);

    zx_assert(!obj->mm.pages_pin_count, "");
    zx_assert(!obj->mm.pages, "");

    if(obj->get_pages_work.func)
        flush_work(&obj->get_pages_work);

    if(obj->map_argu){
        if(obj->map_argu->phys_addrs){
            zx_free(obj->map_argu->phys_addrs);
            zx_free(obj->map_argu->node_sizes);
        }
        zx_free(obj->map_argu);
    }

    zx_free(obj);
}

void zx_drm_gem_dump(struct drm_zx_gem_object *bo)
{
    zx_info("bo(%d):%p\n", bo->gid, bo);
    zx_info("           size:%d\n", bo->info.size);
    zx_info("          tiled:%d\n", bo->info.tiled);
    zx_info("          share:%d\n", bo->info.share);
    zx_info("      pool_type:%d\n", bo->info.pool_type);
    zx_info("        secured:%d\n", bo->info.secured);
    zx_info("       priority:%d\n", bo->info.priority);
    zx_info("     cache_type:%d\n", bo->info.cache_type);
    zx_info("      unpagable:%d\n", bo->info.unpagable);
    zx_info("     segment_id:%d\n", bo->info.segment_id);
    zx_info("   cpu_phy_addr:%llx\n", bo->info.cpu_phy_addr);
    zx_info("compress_format:%d\n", bo->info.compress_format);
}

int zx_drm_gem_query_info(zx_card_t *zx, struct drm_zx_gem_object *obj, zx_allocation_info_t *info)
{
    zx_query_info_t query = {0, };

    info->gid = obj->gid;
    query.type = ZX_QUERY_ALLOCATION_INFO;
    query.buf = ptr_to_ptr64(info);
    query.in_buf = ptr_to_ptr64(obj->priv);

    return zx_core_interface->query_info(zx->adapter, &query);
}

int zx_drm_gem_query_alloc_info(zx_file_t *priv, unsigned int handle, zx_allocation_info_t *info)
{
    int ret = -ENOENT;
    zx_card_t *zx = priv->card;
    struct drm_file *file = priv->parent_file;
    struct drm_device *dev = file->minor->dev;
    struct drm_zx_gem_object *obj;
    zx_query_info_t query = {0, };

    obj = zx_drm_gem_object_lookup(dev, file, handle);
    if (obj)
    {
        ret = zx_drm_gem_query_info(zx, obj, info);
        zx_gem_object_put(obj);
    }

    return ret;
}

static struct sg_table *zx_gem_map_dma_buf(struct dma_buf_attachment *attachment, enum dma_data_direction dir)
{
    struct sg_table *st;
    struct scatterlist *src, *dst;
    struct drm_zx_gem_object *obj = dmabuf_to_zx_bo(attachment->dmabuf);
    zx_card_t *zx = obj->base.dev->dev_private;
    int ret, i;

    //1, prepare the allocation and mark it to unpagable.
    if ((ret = zx_core_interface->prepare_and_mark_unpagable(zx->adapter, obj->priv)) != S_OK)
    {
        goto err;
    }

    //2, get pages, format in sg_table, here need to transfer the os_pages_memory to sg_table
    ret = zx_gem_object_pin_pages(obj);
    if (ret)
        goto err;
    //3, allocate a new sg table, sourced from the pages get from the allocation in step2.
    st = kmalloc(sizeof(struct sg_table), GFP_KERNEL);
    if (st == NULL)
    {
        ret = -ENOMEM;
        goto err_unpin_pages;
    }

    ret = sg_alloc_table(st, obj->mm.pages->nents, GFP_KERNEL);
    if (ret)
        goto err_free;

    src = obj->mm.pages->sgl;
    dst = st->sgl;

    for (i = 0; i < obj->mm.pages->nents; i++)
    {
        sg_set_page(dst, sg_page(src), src->length, 0);
        dst = sg_next(dst);
        src = sg_next(src);
    }

    st->nents = dma_map_sg(attachment->dev, st->sgl, st->orig_nents, dir);
    if (!st->nents)
    {
        ret = -ENOMEM;
        goto err_free_sg;
    }
    return st;

err_free_sg:
    sg_free_table(st);
err_free:
    kfree(st);
err_unpin_pages:
    zx_gem_object_unpin_pages(obj);
err:
    return NULL;
}

//unmap the dmabuf, which is just unbind the sg table, that fetched from the map_dma_buf.
// this operation has no-effect on the src sg table in dmabuf.
static void zx_gem_unmap_dma_buf(struct dma_buf_attachment *attachment, struct sg_table *sg, enum dma_data_direction dir)
{
    struct drm_zx_gem_object *obj = dmabuf_to_zx_bo(attachment->dmabuf);
    zx_card_t *zx = obj->base.dev->dev_private;

    dma_unmap_sg(attachment->dev, sg->sgl, sg->nents, dir);
    sg_free_table(sg);
    kfree(sg);

    zx_gem_object_unpin_pages(obj);
    zx_core_interface->mark_pagable(zx->adapter, obj->priv);
}

zx_cpu_vm_area_t *zx_gem_object_vmap(struct drm_zx_gem_object *obj)
{
    zx_card_t *zx = obj->base.dev->dev_private;
    zx_cpu_vm_area_t *vma = NULL;
    zx_map_argu_t map_argu = {0, };
    int ret;

    if ((ret = zx_core_interface->prepare_and_mark_unpagable(zx->adapter, obj->priv)) != S_OK)
    {
        goto exit;
    }

    mutex_lock(&obj->mm.lock);
    if (obj->krnl_vma)
    {
        vma = obj->krnl_vma;
        obj->krnl_vma->ref_cnt++;
        goto unlock;
    }

    map_argu.flags.read_only = false;
    zx_core_interface->get_map_allocation_info(zx->adapter, obj->priv, &map_argu);

    if(map_argu.node_num > 1){
        map_argu.phys_addrs = zx_calloc(sizeof(unsigned long long) * map_argu.node_num);
        map_argu.node_sizes = zx_calloc(sizeof(unsigned long long) * map_argu.node_num);
    }
    zx_core_interface->get_map_allocation_info(zx->adapter, obj->priv, &map_argu);

    if (map_argu.flags.mem_type == ZX_SYSTEM_IO)
    {
        vma = zx_map_io_memory(&map_argu);
    }
    else if (map_argu.flags.mem_type == ZX_SYSTEM_RAM)
    {
        vma = zx_map_pages_memory(&map_argu);
    }
    else
    {
        zx_assert(0, "");
    }

#if DRM_VERSION_CODE >= KERNEL_VERSION(5,11,0)
    obj->vmap_mem_type = map_argu.flags.mem_type;
#endif
    obj->krnl_vma = vma;

    if(map_argu.phys_addrs){
        zx_free(map_argu.phys_addrs);
        zx_free(map_argu.node_sizes);
    }

unlock:
    mutex_unlock(&obj->mm.lock);
exit:
    return vma;
}
//map the dmabuf in kernel mode.
#if DRM_VERSION_CODE >= KERNEL_VERSION(5,11,0)
static int zx_gem_dmabuf_vmap(struct dma_buf *dma_buf, struct DMABUF_PREFIX(map) *map)
#else
static void *zx_gem_dmabuf_vmap(struct dma_buf *dma_buf)
#endif
{
#if DRM_VERSION_CODE >= KERNEL_VERSION(5,11,0)
    struct drm_gem_object *obj = dma_buf->priv;
    int ret;

    if(!obj->funcs->vmap)
        return -EOPNOTSUPP;

    ret = obj->funcs->vmap(obj, map);
    if(ret)
        return ret;
    else if(DMABUF_PREFIX(map_is_null)(map))
        return -ENOMEM;

    return 0;
#else
    struct drm_zx_gem_object *obj= dmabuf_to_zx_bo(dma_buf);
    zx_cpu_vm_area_t *vma = NULL;

    /*
        1. For export to other vendors, dma_buf should be located in pcie, vmap should be ok
        2. For export to zx self, should not go this map path
    */
    if(obj->info.pool_type != ZX_POOL_NONLOCALVIDMEM)
        zx_assert(0, "%s local memory should not be vmapped by this path", __func__);

    vma = zx_gem_object_vmap(obj);

    return vma->virt_addr;
#endif
}

void zx_gem_object_vunmap(struct drm_zx_gem_object *obj)
{
    zx_card_t *zx = obj->base.dev->dev_private;

    mutex_lock(&obj->mm.lock);
    if (obj->krnl_vma && --obj->krnl_vma->ref_cnt == 0)
    {
        switch(obj->krnl_vma->flags.mem_type)
        {
        case ZX_SYSTEM_IO:
            zx_unmap_io_memory(obj->krnl_vma);
            break;
        case ZX_SYSTEM_RAM:
            zx_unmap_pages_memory(obj->krnl_vma);
            break;
        default:
            zx_assert(0, "");
            break;
        }
        obj->krnl_vma = NULL;
    }
    mutex_unlock(&obj->mm.lock);

    zx_core_interface->mark_pagable(zx->adapter, obj->priv);
}

#if DRM_VERSION_CODE >= KERNEL_VERSION(5,11,0)
static void zx_gem_dmabuf_vunmap(struct dma_buf *dma_buf, struct DMABUF_PREFIX(map) *map)
#else
static void zx_gem_dmabuf_vunmap(struct dma_buf *dma_buf, void *vaddr)
#endif
{
#if DRM_VERSION_CODE >= KERNEL_VERSION(5,11,0)
    struct drm_gem_object *obj = dma_buf->priv;

    if(!obj->funcs->vunmap)
    {
        zx_error("drm gem object funcs vunmap not register.\n");
        DMA_BUF_MAP_CLEAR();
    }
    else
    {
        obj->funcs->vunmap(obj, map);
    }
#else
    struct drm_zx_gem_object *obj = dmabuf_to_zx_bo(dma_buf);

    zx_gem_object_vunmap(obj);
#endif
}

static void *zx_gem_dmabuf_kmap_atomic(struct dma_buf *dma_buf, unsigned long page_num)
{
    zx_info("kmap_atomic: not support yet.\n");
    return NULL;
}

static void zx_gem_dmabuf_kunmap_atomic(struct dma_buf *dma_buf, unsigned long page_num, void *addr)
{
    zx_info("kunmap_atomic: not support yet.\n");
}

static void *zx_gem_dmabuf_kmap(struct dma_buf *dma_buf, unsigned long page_num)
{
    zx_info("kmap: not support yet.\n");
    return NULL;
}

static void zx_gem_dmabuf_kunmap(struct dma_buf *dma_buf, unsigned long page_num, void *addr)
{
    zx_info("kunmap: not support yet.\n");
}

//map the dmabuf to user mode
static int zx_gem_dmabuf_mmap(struct dma_buf *dma_buf, struct vm_area_struct *vma)
{
    struct drm_zx_gem_object *obj = dmabuf_to_zx_bo(dma_buf);
    zx_card_t *zx = obj->base.dev->dev_private;
    zx_map_argu_t map_argu = {0, };
    int ret = 0;

    zx_info("zx_gem_dmabuf_mmap(%p)\n", dma_buf);

    mutex_lock(&obj->mm.lock);

    map_argu.flags.read_only = false;
    zx_core_interface->get_map_allocation_info(zx->adapter, obj->priv, &map_argu);
    if(map_argu.node_num > 1){
        map_argu.phys_addrs = zx_calloc(sizeof(unsigned long long) * map_argu.node_num);
        map_argu.node_sizes = zx_calloc(sizeof(unsigned long long) * map_argu.node_num);
    }
    zx_core_interface->get_map_allocation_info(zx->adapter, obj->priv, &map_argu);

    switch(map_argu.flags.mem_type)
    {
    case ZX_SYSTEM_IO:
        ret = zx_map_system_io(vma, &map_argu);
        break;
    case ZX_SYSTEM_RAM:
       {
            int start_page = _ALIGN_DOWN(map_argu.offset, PAGE_SIZE)/PAGE_SIZE;
            int end_page = start_page + ALIGN(map_argu.size, PAGE_SIZE) / PAGE_SIZE;
            map_argu.flags.cache_type = zx_validate_page_cache(map_argu.memory, start_page, end_page, map_argu.flags.cache_type);
            ret = zx_map_system_ram(vma, &map_argu);
            break;
        }
    default:
        zx_assert(0, "");
        break;
    }

    if(map_argu.phys_addrs){
        zx_free(map_argu.phys_addrs);
        zx_free(map_argu.node_sizes);
    }
    mutex_unlock(&obj->mm.lock);
    return ret;
}


#if DRM_VERSION_CODE < KERNEL_VERSION(4,6,0)
static int zx_gem_begin_cpu_access(struct dma_buf *dma_buf, size_t s1, size_t s2, enum dma_data_direction direction)
#else
static int zx_gem_begin_cpu_access(struct dma_buf *dma_buf, enum dma_data_direction direction)
#endif
{
    struct drm_zx_gem_object *obj= dmabuf_to_zx_bo(dma_buf);
    int write = (direction == DMA_BIDIRECTIONAL || direction == DMA_TO_DEVICE);

    zx_gem_object_begin_cpu_access(obj, 2 * HZ, write);

    return 0;
}

#if DRM_VERSION_CODE < KERNEL_VERSION(4,6,0)
static void zx_gem_end_cpu_access(struct dma_buf *dma_buf, size_t s1, size_t s2, enum dma_data_direction direction)
#else
static int zx_gem_end_cpu_access(struct dma_buf *dma_buf, enum dma_data_direction direction)
#endif
{
    struct drm_zx_gem_object *obj = dmabuf_to_zx_bo(dma_buf);
    int write = (direction == DMA_BIDIRECTIONAL || direction == DMA_TO_DEVICE);

    zx_gem_object_end_cpu_access(obj, write);
#if DRM_VERSION_CODE >= KERNEL_VERSION(4,6,0)
    return 0;
#endif
}

int zx_gem_wait_allocation_idle(zx_file_t *priv, zx_wait_allocation_idle_t *wait)
{
    long ret;
    struct drm_gem_object *gem;
    struct drm_file *drm_file = priv->parent_file;

    spin_lock(&drm_file->table_lock);
    gem = idr_find(&drm_file->object_idr, wait->allocation);
    spin_unlock(&drm_file->table_lock);

    if (!gem)
        return -ENODEV;
    ret = zx_gem_object_fence_await(to_zx_bo(gem), 0, msecs_to_jiffies(wait->timeout), wait->write);

    if (ret > 0)
        return 0;       // already signaled
    else if (ret == 0)
        return -ETIME;  // timeout
    else
        return ret;     // error
}

//memory layout transfer: sg_table --> os_pages_memory
static struct os_pages_memory* zx_allocate_pages_memory_from_sg(struct sg_table *st, int size)
{
    int page_cnt = ALIGN(size, PAGE_SIZE) / PAGE_SIZE;
    struct os_pages_memory *memory = NULL;

    memory = zx_allocate_pages_memory_struct(page_cnt, st);
    if (!memory)
        return NULL;

    memory->shared              = TRUE;
    memory->size                = ALIGN(size, PAGE_SIZE);
    memory->need_flush          = TRUE;
    memory->need_zero           = FALSE;
    memory->fixed_page          = TRUE;
    memory->page_size           = PAGE_SIZE;
    memory->dma32               = FALSE;
    memory->st                  = st;
    memory->has_dma_map         = TRUE;
    zx_pages_memory_extract_st(memory);

    zx_info("%s memory=%p size=%d st=%p.\n", __func__, memory, size, memory->st);
    return memory;
}

//get the private pages and orginized these pages to sg_table.
//todo: do we replace the os_pages_memory with the sg_table? which can reduce the transfer from sg_table to os_pages_memory
static int zx_gem_object_get_pages_generic(struct drm_zx_gem_object *obj)
{
    struct os_pages_memory *memory = NULL;
    zx_card_t *zx = obj->base.dev->dev_private;

    memory = obj->pages_mem;
    zx_assert(memory != NULL, "");
    obj->mm.pages = memory->st;

    return 0;
}

static void zx_gem_object_put_pages_generic(struct drm_zx_gem_object *obj, struct sg_table *pages)
{

}

static void zx_drm_gem_alloc_gid(zx_card_t *zx, struct drm_zx_gem_object *obj)
{
    spin_lock(&zx->gem_gid_lock);
    obj->gid = idr_alloc_cyclic(&zx->gem_gid_table, obj, 0x5000000, 0, GFP_NOWAIT);
    spin_unlock(&zx->gem_gid_lock);
}

static void zx_drm_gem_release_gid(zx_card_t *zx, struct drm_zx_gem_object *obj)
{
    spin_lock(&zx->gem_gid_lock);
    idr_remove(&zx->gem_gid_table, obj->gid);
    spin_unlock(&zx->gem_gid_lock);

    obj->gid = 0;
}

static void zx_gem_object_release_generic(struct drm_zx_gem_object *obj)
{
    zx_destroy_flag_t flag = {0, };
    zx_card_t *zx = obj->base.dev->dev_private;

    zx_gem_object_fence_await(obj, 0, 2 * HZ, 1);

    zx_gem_debugfs_remove_object(obj);

    zx_core_interface->destroy_allocation(zx->adapter, obj->priv, flag);

    obj->priv = NULL;

    trace_zxgfx_drm_gem_release_object(zx->index, obj);

    zx_drm_gem_release_gid(zx, obj);
}


static const struct drm_zx_gem_object_ops zx_gem_object_generic_ops =
{
    .get_pages = zx_gem_object_get_pages_generic,
    .put_pages = zx_gem_object_put_pages_generic,
    .release = zx_gem_object_release_generic,
};

#if DRM_VERSION_CODE < KERNEL_VERSION(5, 11, 0)
void *zx_gem_prime_vmap(struct drm_gem_object *gem_obj)
{

    struct drm_zx_gem_object *zx_gem_obj= to_zx_bo(gem_obj);

    zx_cpu_vm_area_t *vma = NULL;

    vma = (zx_cpu_vm_area_t *)zx_gem_object_vmap(zx_gem_obj);

    if (!vma)
    {
        return ERR_PTR(-ENOMEM);
    }

    return vma->virt_addr;
}

void zx_gem_prime_vunmap(struct drm_gem_object *gem_obj, void *vaddr)
{

    struct drm_zx_gem_object *zx_gem_obj= to_zx_bo(gem_obj);

    zx_gem_object_vunmap(zx_gem_obj);
}
#endif


#if DRM_VERSION_CODE >= KERNEL_VERSION(5,11,0)
static int zx_gem_object_vmap_wrapper(struct drm_gem_object *gem, struct DMABUF_PREFIX(map) *map)
{
    struct drm_zx_gem_object *obj= to_zx_bo(gem);
    zx_cpu_vm_area_t *vma = NULL;

    vma = zx_gem_object_vmap(obj);

    if (!vma)
        return -ENOMEM;

    if (obj->vmap_mem_type == ZX_SYSTEM_IO)
    {
        DMABUF_PREFIX(map_set_vaddr_iomem)(map, (void __iomem *)vma->virt_addr);
    }
    else if (obj->vmap_mem_type == ZX_SYSTEM_RAM)
    {
        DMABUF_PREFIX(map_set_vaddr)(map, vma->virt_addr);
    }
    else
    {
        return -ENODEV;
    }

    return 0;
}

static void zx_gem_object_vunmap_wrapper(struct drm_gem_object *gem, struct DMABUF_PREFIX(map) *map)
{
    struct drm_zx_gem_object *obj= to_zx_bo(gem);

    zx_gem_object_vunmap(obj);

    DMA_BUF_MAP_CLEAR();
}
#endif

#if DRM_VERSION_CODE >= KERNEL_VERSION(5,11,0)
static const struct drm_gem_object_funcs zx_gem_object_funcs =
{
    //.close         = ,
    .open          = zx_gem_open,
    .free          = zx_gem_free_object,
    //mmp is still in drm_driver
    //.mmp
    .vm_ops        = &zx_gem_vm_ops,
    //.pin           = ,
    //.unpin         = ,
    .vmap          = zx_gem_object_vmap_wrapper,
    .vunmap        = zx_gem_object_vunmap_wrapper,
    .export        = zx_gem_prime_export,
    //.get_set_table = ,
};
#endif


static int zx_drm_gem_object_mmap_immediate(struct vm_area_struct* vma, zx_map_argu_t *map_argu)
{
    int ret = 0;

    switch(map_argu->flags.mem_type)
    {
    case ZX_SYSTEM_IO:
        vma->vm_pgoff = map_argu->phys_addr >> PAGE_SHIFT;
#if defined (__aarch64__) || defined (__mips64__)
         map_argu->flags.cache_type = ZX_MEM_UNCACHED;
#endif
        ret = zx_map_system_io(vma, map_argu);
        break;
    case ZX_SYSTEM_RAM:
        {
            int start_page = _ALIGN_DOWN(map_argu->offset, PAGE_SIZE)/PAGE_SIZE;
            int end_page = start_page + ALIGN(map_argu->size, PAGE_SIZE) / PAGE_SIZE;
            map_argu->flags.cache_type = zx_validate_page_cache(map_argu->memory, start_page, end_page, map_argu->flags.cache_type);
            ret = zx_map_system_ram(vma, map_argu);
            break;
        }
    default:
        break;
    }

    return ret;
}


static int zx_drm_gem_object_mmap_delay(struct vm_area_struct* vma, zx_map_argu_t *map_argu)
{
    int err = 0;
    unsigned int  cache_type;

    switch(map_argu->flags.mem_type)
    {
    case ZX_SYSTEM_IO:
        cache_type = map_argu->flags.cache_type;
#if LINUX_VERSION_CODE >= 0x020612
        vma->vm_page_prot = vm_get_page_prot(vma->vm_flags);
#endif
        vma->vm_page_prot = os_get_pgprot_val(&cache_type, vma->vm_page_prot, 1);
        map_argu->flags.cache_type = cache_type;

        break;
    case ZX_SYSTEM_RAM:
        // actually map 0 pages here, just force update cache_type
        map_argu->flags.cache_type = zx_validate_page_cache(map_argu->memory, 0, 0, map_argu->flags.cache_type);
        cache_type = map_argu->flags.cache_type;

#if LINUX_VERSION_CODE >= 0x020612
        vma->vm_page_prot = vm_get_page_prot(vma->vm_flags);
#endif
        vma->vm_page_prot = os_get_pgprot_val(&cache_type, vma->vm_page_prot, 0);

        break;
    default:
        err = -EINVAL;
        zx_assert(0, "");
        break;
    }

    return err;
}

static int zx_drm_gem_object_mmap(struct file *filp, struct drm_zx_gem_object *obj, struct vm_area_struct *vma)
{
    struct drm_file *file = filp->private_data;
    zx_file_t     *priv = file->driver_priv;
    zx_map_argu_t *map_argu = NULL;
    unsigned int  cache_type;
    int ret = -EINVAL;

    mutex_lock(&obj->mm.lock);

    if (!obj->info.cpu_visible)
        goto unlock;

    if(!obj->map_argu){
        map_argu = zx_calloc(sizeof(zx_map_argu_t));
        map_argu->flags.read_only = false;
    }else{
        if(obj->map_argu->phys_addrs){
            zx_free(obj->map_argu->phys_addrs);
            zx_free(obj->map_argu->node_sizes);
        }
        map_argu = obj->map_argu;
        zx_memset(map_argu, 0, sizeof(zx_map_argu_t));
        map_argu->flags.read_only = false;
    }
    if (zx_core_interface->get_map_allocation_info(priv->card->adapter, obj->priv, map_argu))
        goto unlock;
    if(map_argu->node_num > 1){
        map_argu->phys_addrs = zx_calloc(sizeof(unsigned long long) * map_argu->node_num);
        map_argu->node_sizes = zx_calloc(sizeof(unsigned long long) * map_argu->node_num);
    }
    if (zx_core_interface->get_map_allocation_info(priv->card->adapter, obj->priv, map_argu))
        goto unlock;

#if DRM_VERSION_CODE < KERNEL_VERSION(6,3,0)
    vma->vm_flags |= (VM_IO | VM_PFNMAP | VM_DONTEXPAND | VM_DONTDUMP);
#else
    vm_flags_set(vma, (VM_IO | VM_PFNMAP | VM_DONTEXPAND | VM_DONTDUMP));
#endif

    if (obj->delay_map)
    {
        ret = zx_drm_gem_object_mmap_delay(vma, map_argu);
    }
    else
    {
        ret = zx_drm_gem_object_mmap_immediate(vma, map_argu);
    }

    obj->map_argu = map_argu;
unlock:
    mutex_unlock(&obj->mm.lock);
    return ret;
}

static void zx_drm_gem_object_vm_prepare(struct drm_zx_gem_object *obj)
{
    zx_card_t *zx = obj->base.dev->dev_private;

    zx_core_interface->prepare_and_mark_unpagable(zx->adapter, obj->priv);
}

static void zx_drm_gem_object_vm_release(struct drm_zx_gem_object *obj)
{
    zx_card_t *zx = obj->base.dev->dev_private;

    zx_core_interface->mark_pagable(zx->adapter, obj->priv);
}

static void zx_gem_object_init(struct drm_zx_gem_object *obj, const struct drm_zx_gem_object_ops *ops)
{
    mutex_init(&obj->mm.lock);
    obj->ops = ops;

    reservation_object_init(&obj->__builtin_resv);
    bo_resv(obj) = &obj->__builtin_resv;

#if DRM_VERSION_CODE >= KERNEL_VERSION(5,11,0)
    obj->base.funcs = &zx_gem_object_funcs;
#endif
}

static int zx_get_pages_from_userptr(struct device *dev, unsigned long userptr, unsigned int size, struct os_pages_memory **mem)
{
    struct os_pages_memory *memory = NULL;
    struct page **pages = NULL;
    int page_num  = PAGE_ALIGN(size)/PAGE_SIZE;
    int index = 0;
    int pinned = 0;
    int result = 0;

    pages = vzalloc(page_num * sizeof(struct page*));

#if DRM_VERSION_CODE < KERNEL_VERSION(5,8,0)
    down_read(&current->mm->mmap_sem);
#else
    down_read(&current->mm->mmap_lock);
#endif

    while (pinned < page_num)
    {
// For CentOS old kernel + new drm backport, we will include drm_backport.h globally,
// DRM_BACKPORT_H_ is defined in drm_backport.h, and get_user_pages() is also redefined
// in drm_backport.h to be used by backported new drm drivers, so use DRM_VERSION_CODE
// to check which get_user_pages() to call.
// If the DRM_BACKPORT_H_ is not defined, should always use LINUX_VERSION_CODE even if
// the drm is backported, since the get_user_pages() is defined by kernel, not by drm.

#ifdef DRM_BACKPORT_H_
#define GET_USER_PAGES_VERSION_CODE DRM_VERSION_CODE
#else
#define GET_USER_PAGES_VERSION_CODE LINUX_VERSION_CODE
#endif

#if GET_USER_PAGES_VERSION_CODE >= KERNEL_VERSION(6, 5, 0)
        result = get_user_pages(userptr + (pinned * PAGE_SIZE),
                                page_num - pinned,
                                FOLL_WRITE,
                                &pages[pinned]);
#elif GET_USER_PAGES_VERSION_CODE >= KERNEL_VERSION(4, 9, 0)
        result = get_user_pages(userptr + (pinned * PAGE_SIZE),
                                page_num - pinned,
                                FOLL_WRITE,
                                &pages[pinned],
                                NULL);
#elif GET_USER_PAGES_VERSION_CODE >= KERNEL_VERSION(4, 6, 0)
        result = get_user_pages(userptr + (pinned * PAGE_SIZE),
                                page_num - pinned,
                                FOLL_WRITE,
                                0,
                                &pages[pinned],
                                NULL);
#elif GET_USER_PAGES_VERSION_CODE >= KERNEL_VERSION(4, 4, 168) && GET_USER_PAGES_VERSION_CODE < KERNEL_VERSION(4, 5, 0)
        result = get_user_pages(current,
                                current->mm,
                                userptr + (pinned * PAGE_SIZE),
                                page_num - pinned,
                                FOLL_WRITE,
                                &pages[pinned],
                                NULL);
#else
        result = get_user_pages(current,
                                current->mm,
                                userptr + (pinned * PAGE_SIZE),
                                page_num - pinned,
                                1,
                                0,
                                &pages[pinned],
                                NULL);
#endif
        if (result < 0)
        {
            goto release_pages;
        }
        pinned += result;
    }
#if DRM_VERSION_CODE < KERNEL_VERSION(5,8,0)
    up_read(&current->mm->mmap_sem);
#else
    up_read(&current->mm->mmap_lock);
#endif

    memory = zx_allocate_pages_memory_struct(page_num, NULL);

    memory->size        = PAGE_ALIGN(size); // supose size==PAGE_ALIGN(size)
    memory->page_size   = PAGE_SIZE;
    memory->need_flush  = TRUE;
    memory->fixed_page  = TRUE;
    memory->need_zero   = FALSE;
    memory->noswap      = FALSE;
    memory->dma32       = FALSE;
    memory->shared      = FALSE;
    memory->userptr     = TRUE;
    sg_alloc_table_from_pages(memory->st, pages, page_num, 0, PAGE_ALIGN(size), GFP_KERNEL);
    zx_pages_memory_extract_st(memory);
    memory->st->nents = dma_map_sg(dev, memory->st->sgl, memory->st->orig_nents, DMA_BIDIRECTIONAL);

    if (memory->st->nents)
    {
        memory->has_dma_map = TRUE;
    }
    else
    {
        zx_assert(0, "dma_map_sg failed");
    }

    *mem = memory;

    vfree(pages);


    mutex_lock(&g_pages_memory_lock);
    list_add_tail(&memory->link, &g_pages_memory_list);
    mutex_unlock(&g_pages_memory_lock);
    return S_OK;


release_pages:
    for(index = 0; index < pinned; index++)
    {
        put_page(pages[index]);
    }
#if DRM_VERSION_CODE < KERNEL_VERSION(5,8,0)
    up_read(&current->mm->mmap_sem);
#else
    up_read(&current->mm->mmap_lock);
#endif
    vfree(pages);

    return E_FAIL;
}


struct os_pages_memory* zx_gem_get_pages_from_user_ptr(void *gemObj, void *pdev, unsigned long long size, alloc_pages_flags_t *flags)
{
    struct drm_zx_gem_object *obj = (struct drm_zx_gem_object*)gemObj;

    if(obj){
        if(!obj->pages_mem)
            wait_for_completion(&obj->get_pages_completion);

        return obj->pages_mem;
    }else
        return NULL;
}

void zx_gem_release_pages_from_user_ptr(void *pdev, struct os_pages_memory *pages_mem)
{
    zx_free_pages_memory(pdev, pages_mem);
}

struct os_pages_memory* zx_gem_alloc_pages(void *gemObj, void *pdev, unsigned long long size, alloc_pages_flags_t *flags)
{
    struct drm_zx_gem_object *obj = (struct drm_zx_gem_object*)gemObj;

    if(obj){
        if(1){
            obj->pages_mem = zx_allocate_pages_memory(pdev, size, PAGE_SIZE, *flags);
        }else{
            if(!obj->pages_mem)
                wait_for_completion(&obj->get_pages_completion);
        }

        return obj->pages_mem;
    }else
        return NULL;
}

void zx_gem_release_pages(void *pdev, struct os_pages_memory *pages_mem)
{
    zx_free_pages_memory(pdev, pages_mem);
}

struct os_pages_memory* zx_gem_get_pages_from_import_pages(void *gemObj, void *pdev, unsigned long long size, alloc_pages_flags_t *flags)
{
    struct drm_zx_gem_object *obj = (struct drm_zx_gem_object*)gemObj;

    if(obj){
        return obj->pages_mem;
    }else{
        return NULL;
    }
}

void zx_gem_release_pages_from_import_pages(void *pdev, struct os_pages_memory *pages_mem)
{
    zx_free_pages_memory(pdev, pages_mem);
}

void zx_get_pages_work_func(struct work_struct *work)
{
    struct drm_zx_gem_object *obj = container_of(work, struct drm_zx_gem_object, get_pages_work);
    if(obj->pages_args.user_ptr){
        zx_get_pages_from_userptr(obj->base.dev->dev, obj->pages_args.user_ptr, obj->pages_args.user_buf_size, &obj->pages_mem);

        complete(&obj->get_pages_completion);
    }else{
        zx_assert(0, "TODO");
        //obj->pages_mem = zx_allocate_pages_memory(obj->base.dev->pdev, obj->info.size, PAGE_SIZE, obj->info.alloc_pages_flags);

        complete(&obj->get_pages_completion);
    }
}


struct drm_zx_gem_object* zx_drm_gem_create_object(zx_card_t *zx, zx_create_allocation_info_t *create)
{
    struct drm_zx_gem_object *obj = NULL;
    int ret = 0;
    krnl_create_allocation_info_t krnl_create = {0, };

    obj = zx_calloc(sizeof(*obj));

    zx_drm_gem_alloc_gid(zx, obj);

    krnl_create.info = create;
    krnl_create.parent = obj;
    krnl_create.debug_gid = obj->gid;
    if(create->user_ptr)
    {
        obj->pages_args.user_ptr = create->user_ptr;
        obj->pages_args.user_buf_size = create->user_buf_size;
        krnl_create.get_system_pages = zx_gem_get_pages_from_user_ptr;
        krnl_create.release_system_pages = zx_gem_release_pages_from_user_ptr;
    }
    else
    {
        krnl_create.get_system_pages = zx_gem_alloc_pages;
        krnl_create.release_system_pages = zx_gem_release_pages;
    }

    obj->priv = zx_core_interface->create_allocation(zx->adapter, &krnl_create);
    if (!obj->priv)
    {
        zx_free(obj);
        return NULL;
    }

    zx_drm_gem_query_info(zx, obj, &obj->info);
#ifndef ZX_HW_NULL
    drm_gem_private_object_init(_to_drm_device(zx->pdev), &obj->base, create->size);
#else
    drm_gem_private_object_init(NULL, &obj->base, create->size);
#endif
    zx_gem_object_init(obj, &zx_gem_object_generic_ops);

    obj->debug.parent_gem = obj;
    obj->debug.root       = zx_debugfs_get_allocation_root(zx->debugfs_dev);
    obj->debug.is_dma_buf_import = false;

    //if(create->user_ptr || obj->info.has_pages){
    if(create->user_ptr) {
        INIT_WORK(&obj->get_pages_work, zx_get_pages_work_func);
        init_completion(&obj->get_pages_completion);
        schedule_work(&obj->get_pages_work);
    }

    zx_gem_debugfs_add_object(obj);
    trace_zxgfx_drm_gem_create_object(zx->index, obj);

    return obj;
}

int zx_drm_gem_create_object_ioctl(struct drm_file *file, zx_create_allocation_t *create)
{
    int ret = 0, idx;
    unsigned int alloc_nr = create->num_allocations;
    struct drm_zx_gem_object *obj;
    zx_file_t *priv = file->driver_priv;
    zx_card_t *zx  = priv->card;
    zx_create_allocation_info_t *create_info;

    unsigned int gem_handle = 0;

    if(alloc_nr == 0)
        return -EINVAL;

    create_info = zx_calloc(alloc_nr * sizeof(*create_info));
    if(!create_info)
        return -ENOMEM;

    zx_copy_from_user(create_info, ptr64_to_ptr(create->alloc_info), \
                            alloc_nr * sizeof(*create_info));

    for (idx = 0; idx < alloc_nr; ++idx)
    {
        obj = zx_drm_gem_create_object(zx, &create_info[idx]);
        if (!obj)
            return -ENOMEM;

        obj->debug.device = priv->gpu_device_handle;

        ret = drm_gem_handle_create(file, &obj->base, &gem_handle);
        if(ret)
            return ret;

        create_info[idx].handle = gem_handle;

        zx_gem_object_put(obj);
    }

    zx_copy_to_user(ptr64_to_ptr(create->alloc_info), create_info, sizeof(*create_info) * alloc_nr);

    zx_free(create_info);

    return 0;
}

// DMABUF
static int zx_gem_object_get_pages_dmabuf(struct drm_zx_gem_object *obj)
{
    struct sg_table *pages;
    pages = dma_buf_map_attachment(obj->base.import_attach, DMA_BIDIRECTIONAL);
    if (IS_ERR(pages))
        return PTR_ERR(pages);

    obj->mm.pages = pages;
    return 0;
}

static void zx_gem_object_put_pages_dmabuf(struct drm_zx_gem_object *obj, struct sg_table *pages)
{
    dma_buf_unmap_attachment(obj->base.import_attach, pages, DMA_BIDIRECTIONAL);
}

static void zx_gem_object_release_dmabuf(struct drm_zx_gem_object *obj)
{
    zx_destroy_flag_t flag = {0, };
    zx_card_t *zx = obj->base.dev->dev_private;
    struct dma_buf_attachment *attach;
    struct dma_buf *dma_buf;

    if (obj->priv)
    {
        zx_gem_object_fence_await(obj, 0, 2 * HZ, 1);

        zx_core_interface->destroy_allocation(zx->adapter, obj->priv, flag);
        obj->priv = 0;

        zx_gem_debugfs_remove_object(obj);

        zx_gem_object_unpin_pages(obj);
    }

    attach = obj->base.import_attach;
    dma_buf = attach->dmabuf;
    dma_buf_detach(dma_buf, attach);
    dma_buf_put(dma_buf);

    trace_zxgfx_drm_gem_release_object(zx->index, obj);

    zx_drm_gem_release_gid(zx, obj);
}

static const struct drm_zx_gem_object_ops zx_gem_object_dmabuf_ops =
{
    .get_pages = zx_gem_object_get_pages_dmabuf,
    .put_pages = zx_gem_object_put_pages_dmabuf,
    .release = zx_gem_object_release_dmabuf,
};

#if DRM_VERSION_CODE < KERNEL_VERSION(6,6,0)
//import the prime_fd to handle. call the drm_gem_prime_fd_to_handle, which calles the drm_driver->gem_prime_import
int zx_gem_prime_fd_to_handle(struct drm_device *dev, struct drm_file *file_priv, int prime_fd, uint32_t *handle)
{
    int ret;
    struct drm_zx_driver *driver = to_drm_zx_driver(dev->driver);

    mutex_lock(&driver->lock);
    zx_assert(!driver->file_priv, "");

    driver->file_priv = file_priv;
    ret = drm_gem_prime_fd_to_handle(dev, file_priv, prime_fd, handle);
    driver->file_priv = NULL;
    mutex_unlock(&driver->lock);

    return ret;
}
#endif

signed long zx_gem_object_begin_cpu_access(struct drm_zx_gem_object *obj, long timeout, int write)
{
    zx_card_t *zx = obj->base.dev->dev_private;

    trace_zxgfx_gem_object_begin_cpu_access(zx->index, obj, timeout, write);

    return zx_gem_object_fence_await(obj, 0, timeout, write);
}

int zx_gem_object_begin_cpu_access_ioctl(struct drm_file *file, zx_drm_gem_begin_cpu_access_t *args)
{
    int ret = 0;
    struct drm_device *dev = file->minor->dev;
    struct drm_zx_gem_object *obj;

    obj = zx_drm_gem_object_lookup(dev, file, args->handle);

    if (!obj)
        return -ENOENT;

    ret = zx_gem_object_begin_cpu_access(obj, 2 * HZ, !args->readonly);

    zx_gem_object_put(obj);
    return 0;
}

void zx_gem_object_end_cpu_access(struct drm_zx_gem_object *obj, int write)
{
    zx_card_t *zx = obj->base.dev->dev_private;
    zx_map_argu_t map_argu = {0, };

    map_argu.flags.read_only = false;
    zx_core_interface->get_map_allocation_info(zx->adapter, obj->priv, &map_argu);
    if(map_argu.node_num > 1){
        map_argu.phys_addrs = zx_calloc(sizeof(unsigned long long) * map_argu.node_num);
        map_argu.node_sizes = zx_calloc(sizeof(unsigned long long) * map_argu.node_num);
    }
    zx_core_interface->get_map_allocation_info(zx->adapter, obj->priv, &map_argu);

    if (map_argu.flags.mem_type == ZX_SYSTEM_RAM &&
        map_argu.flags.cache_type == ZX_MEM_WRITE_BACK &&
        (obj->info.cache_type != ZX_CACHE_CACHABLE) &&
        map_argu.memory)
    {
        zx_flush_cache(NULL, map_argu.memory, map_argu.offset, map_argu.size);
    }

    if (map_argu.flags.cache_type == ZX_MEM_WRITE_COMBINED)
    {
        zx_wmb();
    }

    if(map_argu.phys_addrs){
        zx_free(map_argu.phys_addrs);
        zx_free(map_argu.node_sizes);
    }

    trace_zxgfx_gem_object_end_cpu_access(zx->index, obj);
}

void zx_gem_object_end_cpu_access_ioctl(struct drm_file *file, zx_drm_gem_end_cpu_access_t *args)
{
    struct drm_device *dev = file->minor->dev;
    struct drm_zx_gem_object *obj;

    obj = zx_drm_gem_object_lookup(dev, file, args->handle);

    if (!obj)
        return;

    zx_gem_object_end_cpu_access(obj, 1);
    zx_gem_object_put(obj);
}

const struct dma_buf_ops CONCAT(zx_dmabuf_ops,DRIVER_NAME) =
{
    .map_dma_buf    = zx_gem_map_dma_buf,
    .unmap_dma_buf  = zx_gem_unmap_dma_buf,
    .release        = drm_gem_dmabuf_release,
#if DRM_VERSION_CODE < KERNEL_VERSION(5,6,0)
#if DRM_VERSION_CODE > KERNEL_VERSION(4,11,0)
    .map           = zx_gem_dmabuf_kmap,
    .unmap         = zx_gem_dmabuf_kunmap,
#if DRM_VERSION_CODE < KERNEL_VERSION(4,19,0)
    .map_atomic    = zx_gem_dmabuf_kmap_atomic,
    .unmap_atomic  = zx_gem_dmabuf_kunmap_atomic,
#endif
#else
    .kmap            = zx_gem_dmabuf_kmap,
    .kmap_atomic     = zx_gem_dmabuf_kmap_atomic,
    .kunmap          = zx_gem_dmabuf_kunmap,
    .kunmap_atomic   = zx_gem_dmabuf_kunmap_atomic,
#endif
#endif
    .mmap           = zx_gem_dmabuf_mmap,
    .vmap           = zx_gem_dmabuf_vmap,
    .vunmap         = zx_gem_dmabuf_vunmap,
    .begin_cpu_access   = zx_gem_begin_cpu_access,
    .end_cpu_access     = zx_gem_end_cpu_access,
};

static int zx_gem_dmabuf_open(struct drm_gem_object *gem_obj, struct drm_file *file)
{
    struct dma_buf_attachment *attach = gem_obj->import_attach;
    struct dma_buf *dma_buf = attach->dmabuf;
    struct drm_zx_gem_object *obj = to_zx_bo(gem_obj);
    zx_file_t *priv = file->driver_priv;
    zx_card_t *zx = priv->card;
    struct os_pages_memory *pages = NULL;
    zx_create_allocation_info_t alloc_info = {0, };
    krnl_create_allocation_info_t krnl_create = {0, };
    int ret;

    zx_assert(priv->gpu_device, "");

    if (obj->priv)
        return 0;

    ret = zx_gem_object_pin_pages(obj);
    if (ret)
        return ret;

    pages = zx_allocate_pages_memory_from_sg(obj->mm.pages, dma_buf->size);
    if (!pages)
        goto fail_unpin;

    mutex_lock(&g_pages_memory_lock);
    list_add_tail(&pages->link, &g_pages_memory_list);
    mutex_unlock(&g_pages_memory_lock);

    obj->pages_mem = pages;
    krnl_create.parent = obj;
    krnl_create.debug_gid = obj->gid;
    krnl_create.info = &alloc_info;
    krnl_create.get_system_pages = zx_gem_get_pages_from_import_pages;
    krnl_create.release_system_pages = zx_gem_release_pages_from_import_pages;

    alloc_info.size = dma_buf->size;
    alloc_info.alignment = 4096;
    alloc_info.flags.pool_type   = ZX_POOL_NONLOCALVIDMEM;
    alloc_info.flags.cache_type  = ZX_CACHE_CACHABLE;
    alloc_info.flags.cpu_visible = 1;
    alloc_info.flags.share       = 1;
    alloc_info.priority          = 0;

    //1, allocate allocation obj
    obj->priv = zx_core_interface->create_allocation(priv->card->adapter, &krnl_create);
    if (!obj->priv)
    {
        ret = -ENOMEM;
        goto fail_unpin;
    }

    zx_drm_gem_query_info(zx, obj, &obj->info);

    obj->debug.parent_gem = obj;
    obj->debug.root       = zx_debugfs_get_allocation_root(zx->debugfs_dev);
    obj->debug.is_dma_buf_import = true;
    zx_gem_debugfs_add_object(obj);

    return ret;

fail_unpin:
    if (pages)
        zx_free_pages_memory(zx->pdev, pages);

    zx_gem_object_unpin_pages(obj);
    return ret;
}

int zx_gem_open(struct drm_gem_object *gem_obj, struct drm_file *file)
{
    if (gem_obj->import_attach)
        return zx_gem_dmabuf_open(gem_obj, file);

    return 0;
}

//import the dmabuf to the private drm_gem_object, which is wrapped in a private gem object.
struct drm_gem_object *zx_gem_prime_import(struct drm_device *dev, struct dma_buf *dma_buf)
{
    struct drm_zx_driver *driver = to_drm_zx_driver(dev->driver);
    zx_card_t *zx = dev->dev_private;
    struct drm_zx_gem_object *obj = NULL;
    struct dma_buf_attachment *attach;

     //a fastpath for ourself driver. the gem_object is fetched directly.
    if (dma_buf->ops == &CONCAT(zx_dmabuf_ops, DRIVER_NAME))
    {
        obj = dmabuf_to_zx_bo(dma_buf);

        if (obj->base.dev == dev)
        {
            trace_zxgfx_gem_prime_import(zx->index, dma_buf, obj);
            return &(zx_gem_object_get(obj)->base);
        }
    }

    //for the case of the dma_buf is derived from the foreign vendor, we needs to create a new private allocation:
    //1, allocate a new drm_zx_gem_allocation struct;
    //2, get the dma_buffer_attachment from the dma_buf, then pin pages from the attachment, get the sg_table.
    //3, create the page_memory struct from the sg table
    //4, create the private allocation from the page_memory
    attach = dma_buf_attach(dma_buf, dev->dev);
    if (IS_ERR(attach))
        return ERR_CAST(attach);

    get_dma_buf(dma_buf);
    obj = zx_calloc(sizeof(*obj));
    if (obj == NULL)
        goto fail_detach;

    drm_gem_private_object_init(dev, &obj->base, dma_buf->size);
    zx_gem_object_init(obj, &zx_gem_object_dmabuf_ops);

    obj->base.import_attach = attach;
    bo_resv(obj) = dma_buf->resv;

    zx_drm_gem_alloc_gid(zx, obj);

    trace_zxgfx_drm_gem_create_object(zx->index, obj);
    trace_zxgfx_gem_prime_import(zx->index, dma_buf, obj);

    return &obj->base;

fail_detach:
    dma_buf_detach(dma_buf, attach);
    dma_buf_put(dma_buf);

    return NULL;
}

#if DRM_VERSION_CODE >= KERNEL_VERSION(5,4,0)
struct dma_buf *zx_gem_prime_export(struct drm_gem_object *gem_obj, int flags)
#else
struct dma_buf *zx_gem_prime_export(struct drm_device *dev, struct drm_gem_object *gem_obj, int flags)
#endif
{
    struct dma_buf *dma_buf;
    struct drm_zx_gem_object *obj;
    struct drm_device  *dev1;
    zx_card_t *zx;
    DEFINE_DMA_BUF_EXPORT_INFO(exp_info);

#if DRM_VERSION_CODE >= KERNEL_VERSION(5,4,0)
    dev1 = gem_obj->dev;
#else
    dev1 = dev;
#endif
    zx = dev1->dev_private;
    obj = to_zx_bo(gem_obj);

    exp_info.ops = &CONCAT(zx_dmabuf_ops,DRIVER_NAME);
    exp_info.size = obj->base.size;
    exp_info.flags = flags;
    exp_info.priv = gem_obj;
    exp_info.resv = bo_resv(obj);

#if DRM_VERSION_CODE < KERNEL_VERSION(4,9,0)
    dma_buf = dma_buf_export(&exp_info);
#else
    dma_buf = drm_gem_dmabuf_export(dev1, &exp_info);
#endif

    trace_zxgfx_gem_prime_export(zx->index, dma_buf, obj);

    return dma_buf;
}

int zx_gem_dumb_map_offset(struct drm_file *file, struct drm_device *dev, uint32_t handle, uint64_t *offset)
{
    zx_file_t *priv = file->driver_priv;
    zx_card_t *zx = priv->card;
    struct drm_zx_gem_object *obj;
    int ret, i, acquire_psr = 0;
    disp_info_t *disp_info = (disp_info_t *)zx->disp_info;
    psr_data_t *psr_data = disp_info->psr_data;

    obj = zx_drm_gem_object_lookup(dev, file, handle);
    if (!obj)
    {
        ret = -ENOENT;
        goto unlock;
    }

    ret = drm_gem_create_mmap_offset(&obj->base);
    if (ret)
        goto out;
    *offset = drm_vma_node_offset_addr(&obj->base.vma_node);

    obj->dumb_mapped = 1;

    for(i = 0; i < MAX_CORE_CRTCS; i++)
    {
        if(obj->priv == zx->primary_allocation[i])
        {
            acquire_psr = 1;
            break;
        }
    }

    if(acquire_psr)
    {
        psr_acquire_display(psr_data, PSR_FB_REF, 0);     //cpu mapped fb, use FB_REF
    }

out:
    zx_gem_object_put(obj);
unlock:
    return ret;
}

int zx_gem_mmap_gtt_ioctl(struct drm_file *file, zx_drm_gem_map_t *args)
{
    int ret;
    struct drm_zx_gem_object *obj;

    obj = zx_drm_gem_object_lookup(file->minor->dev, file, args->gem_handle);
    if (!obj)
    {
        zx_error("zx_gem_mmap_gtt_ioctl lookup failed \n");
        ret = -ENOENT;
        goto unlock;
    }

    obj->delay_map = args->delay_map;
    obj->prefault_num = args->prefault_num;

    ret = drm_gem_create_mmap_offset(&obj->base);
    if (ret)
        goto out;
    args->offset = drm_vma_node_offset_addr(&obj->base.vma_node);

out:
    zx_gem_object_put(obj);
unlock:
    return ret;
}

int zx_drm_gem_mmap(struct file *filp, struct vm_area_struct *vma)
{
    struct drm_zx_gem_object *obj;
    struct drm_file *file = filp->private_data;
    zx_file_t     *priv = file->driver_priv;
    int             ret = 0;

    ret = drm_gem_mmap(filp, vma);
    if (ret < 0)
    {
        zx_error("failed to mmap, ret = %d.\n", ret);
        return ret;
    }

    obj = to_zx_bo(vma->vm_private_data);

#if DRM_VERSION_CODE < KERNEL_VERSION(6,3,0)
    vma->vm_flags &= ~(VM_IO | VM_PFNMAP | VM_DONTEXPAND | VM_DONTDUMP);
#else
    vm_flags_clear(vma, (VM_IO | VM_PFNMAP | VM_DONTEXPAND | VM_DONTDUMP));
#endif

    vma->vm_page_prot = pgprot_writecombine(vm_get_page_prot(vma->vm_flags));

    zx_drm_gem_object_vm_prepare(obj);
    return zx_drm_gem_object_mmap(filp, obj, vma);
}

void zx_invalid_resident_list_cache(zx_file_t *priv, struct zx_resident_list_cache *cache)
{
    int i;

    zx_assert(!cache->in_use, "still in_use.\n");

    if (cache->gem_list)
    {
        for (i = 0; i < cache->resident_count; i++)
        {
            zx_gem_object_put(cache->gem_list[i]);
        }
        cache->resident_count = 0;
    }
}

void zx_update_resident_list_cache(zx_file_t *priv, struct zx_resident_list_cache *cache)
{
    int i = 0;
    struct zx_resident_node *rnode = NULL;

    zx_invalid_resident_list_cache(priv, cache);

    if (cache->resident_caps < priv->resident_cnt)
    {
        if (cache->resident_list)
        {
            zx_free(cache->resident_list);
        }
        if (cache->gem_list)
        {
            zx_free(cache->gem_list);
        }

        cache->resident_caps = ((priv->resident_cnt + 31) & ~31) * 2;
        cache->resident_list = zx_malloc(sizeof(void*) * cache->resident_caps);
        cache->gem_list = zx_malloc(sizeof(void*) * cache->resident_caps);
    }

    i = 0;
    list_for_each_entry(rnode, &priv->resident_list, link)
    {
        ((void**)cache->resident_list)[i] = rnode->obj->priv;
        cache->gem_list[i] = zx_gem_object_get(rnode->obj);
        ++i;
    }

    cache->resident_count = priv->resident_cnt;
    cache->dirty_count = priv->resident_dirty_count;
}


/* assume resident_lock locked */
int zx_gem_make_resident_locked(struct drm_file *file, unsigned int handle)
{
    int i;
    zx_file_t *priv = file->driver_priv;
    struct drm_device *dev = file->minor->dev;
    struct drm_zx_gem_object *obj;
    struct zx_resident_node *resident = NULL;

    obj = zx_drm_gem_object_lookup(dev, file, handle);
    if (!obj)
        return -ENOENT;

    list_for_each_entry(resident, &priv->resident_list, link)
    {
        if (resident->obj == obj)
        {
            ++resident->resident_cnt;
            zx_gem_object_put(obj);
            goto done;
        }
    }
    resident = zx_calloc(sizeof(*resident));
    resident->resident_cnt = 1;
    resident->obj = obj;
    ++priv->resident_cnt;
    ++priv->resident_dirty_count;
    list_add_tail(&resident->link, &priv->resident_list);

    priv->debug->resident_cnt = priv->resident_cnt;

    // try update cache
    for (i = 0; i < ARRAY_SIZE(priv->resident_list_cache); i++)
    {
        struct zx_resident_list_cache *cache = priv->resident_list_cache + i;
        if (cache->dirty_count == priv->resident_dirty_count - 1 &&
            cache->resident_caps >= priv->resident_cnt)
        {
            // only append is safe
            ((void**)cache->resident_list)[cache->resident_count] = obj->priv;
            cache->gem_list[cache->resident_count] = zx_gem_object_get(obj);
            cache->dirty_count = priv->resident_dirty_count;
            ++cache->resident_count;
        }
    }
done:
    return 0;
}

int zx_gem_evict_locked(struct drm_file *file, unsigned int handle)
{
    int i;
    zx_file_t *priv = file->driver_priv;
    struct drm_device *dev = file->minor->dev;
    struct drm_zx_gem_object *obj;
    struct zx_resident_node *resident = NULL;

    obj = zx_drm_gem_object_lookup(dev, file, handle);
    if (!obj)
        return -ENOENT;

    list_for_each_entry(resident, &priv->resident_list, link)
    {
        if (resident->obj == obj)
        {
            if (--resident->resident_cnt == 0)
            {
                list_del(&resident->link);
                zx_gem_object_put(resident->obj);
                zx_free(resident);
                --priv->resident_cnt;
                ++priv->resident_dirty_count;
                priv->debug->resident_cnt = priv->resident_cnt;

                // try invalid cache
                for (i = 0; i < ARRAY_SIZE(priv->resident_list_cache); i++)
                {
                    struct zx_resident_list_cache *cache = priv->resident_list_cache + i;
                    if (!cache->in_use)
                    {
                        zx_invalid_resident_list_cache(priv, cache);
                    }
                }
            }
            break;
        }
    }
    zx_gem_object_put(obj);
    return 0;
}

void zx_gem_cleanup_resident(struct drm_file *file)
{
    int i;
    zx_file_t *priv = file->driver_priv;
    struct zx_resident_node *resident = NULL, *tmp;

    list_for_each_entry_safe(resident, tmp, &priv->resident_list, link)
    {
        list_del(&resident->link);
        zx_gem_object_put(resident->obj);
        zx_free(resident);
    }
    priv->resident_cnt = 0;

    for (i = 0; i < ARRAY_SIZE(priv->resident_list_cache); i++)
    {
        struct zx_resident_list_cache *cache = priv->resident_list_cache + i;

        zx_assert(!cache->in_use, "resident_list_cache still in_use");

        zx_invalid_resident_list_cache(priv, cache);

        if (cache->resident_list)
        {
            zx_free(cache->resident_list);
            cache->resident_list = NULL;
        }

        if (cache->gem_list)
        {
            zx_free(cache->gem_list);
            cache->gem_list = NULL;
        }
    }
}


int zx_gem_dumb_create(struct drm_file *file, struct drm_device *dev, struct drm_mode_create_dumb *args)
{
    int ret = 0;
    unsigned int pitch;
    struct drm_zx_gem_object *obj = NULL;
    zx_file_t *priv = file->driver_priv;
    zx_card_t *zx  = priv->card;
    zx_create_allocation_info_t alloc_info = {0, };
    zx_query_info_t info = {0,};
    zx_resident_flag_t flags = {0, };
    krnl_map_gpu_va_t map_gpu = {0};

    pitch                           = (args->width * args->bpp/8 + 255) & ~255;
    alloc_info.size                 = (pitch * args->height + 4095) & ~4095;//todo: refine the size
    alloc_info.alignment            = 4096;
    alloc_info.flags.pool_type      = ZX_POOL_VIDEOMEMORY;
    alloc_info.flags.cache_type     = ZX_CACHE_UNCACHABLE;
    alloc_info.flags.unpagable      = 1;
    alloc_info.flags.share          = 1;
    alloc_info.flags.cpu_visible    = 1;
    alloc_info.flags.force_clear    = 1;

    obj = zx_drm_gem_create_object(zx, &alloc_info);
    if (!obj)
        return -ENOMEM;

    map_gpu.allocation = obj->priv;
    map_gpu.size       = alloc_info.size;
    ret = zx_core_interface->map_gpu_virtual_address(zx->adapter, &map_gpu);
    if(ret != S_OK)
    {
        zx_error("%s: faild to map gpu va for fb allocation.\n", __func__);
        return ret;
    }

    zx_drm_gem_query_info(zx, obj, &obj->info);

    ret = drm_gem_handle_create(file, &obj->base, &args->handle);
    zx_assert(ret == 0, "");

    args->pitch = pitch;//todo: get the pitch in kmd
    args->size  = alloc_info.size;

    {
        zx_gem_dbg_info_t *dinfo = &obj->debug.dbginfo;

        dinfo->width = args->width;
        dinfo->height = args->height;
        dinfo->pitch = pitch;
        dinfo->bit_cnt = args->bpp;
        if (args->bpp == 32)
        {
            dinfo->hw_format = 89; // HSF_B8G8R8A8_UNORM
        }
        else if (args->bpp == 16)
        {
            dinfo->hw_format = 153; // HSF_B5G6R5_UNORM
        }
    }
    zx_gem_object_put(obj);

    return ret;
}

vm_fault_t zx_gem_io_insert(struct vm_area_struct *vma, unsigned long address, zx_map_argu_t *map, unsigned int offset, unsigned int prefault_num)
{
    vm_fault_t retval = VM_FAULT_NOPAGE;
    unsigned long pfn = 0;
    unsigned int i = 0;

//    zx_begin_section_trace_event(__func__);
//    zx_counter_trace_event("arg_prefault_num", prefault_num);
//    zx_counter_trace_event("arg_node_num", map->node_num);
//    zx_counter_trace_event("arg_offset", offset);
//    zx_counter_trace_event("arg_size", map->size);
//    zx_counter_trace_event("arg_address", address);
//    zx_counter_trace_event("arg_vm_end", vma->vm_end);

    if (map->node_num == 1)
    {
        for (i = 0; i < prefault_num && offset < map->size; i++)
        {
            pfn = (map->phys_addr + offset) >> PAGE_SHIFT;
            retval = vmf_insert_pfn(vma, address, pfn);
            if (unlikely(retval & VM_FAULT_ERROR))
            {
                goto done;
            }

            offset += PAGE_SIZE;
            address += PAGE_SIZE;
        }
    }
    else
    {
        unsigned int node_idx = 0;
        unsigned long long current_offset = 0;
        unsigned long long skip_offset = 0;

        // skip pre pages
        for (node_idx = 0;
             node_idx < map->node_num && skip_offset + map->node_sizes[node_idx] <= offset;
             node_idx++)
        {
            skip_offset += map->node_sizes[node_idx];
        }
        current_offset = offset - skip_offset;

        for (i = 0; i < prefault_num && offset < map->size; i++)
        {
            if (current_offset >= map->node_sizes[node_idx])
            {
                node_idx++;
                current_offset = 0;
            }
            pfn = (map->phys_addrs[node_idx] + current_offset) / PAGE_SIZE;

            retval = vmf_insert_pfn(vma, address, pfn);
            if (unlikely(retval & VM_FAULT_ERROR))
            {
                goto done;
            }

            offset += PAGE_SIZE;
            address += PAGE_SIZE;
            current_offset += PAGE_SIZE;
        }
    }
done:
//    zx_end_section_trace_event(retval);
    return retval;
}

static vm_fault_t zx_gem_ram_insert(struct vm_area_struct *vma, unsigned long address, zx_map_argu_t *map, unsigned int offset, unsigned int prefault_num)
{
    unsigned long pfn;
    vm_fault_t retval = VM_FAULT_NOPAGE;
    int i, start_page, end_page;

    start_page = _ALIGN_DOWN(offset, PAGE_SIZE) / PAGE_SIZE;
    end_page = min((int)(start_page+prefault_num), (int)(map->memory->size / PAGE_SIZE));
    map->flags.cache_type = zx_validate_page_cache(map->memory, start_page, end_page, map->flags.cache_type);

    for (i = start_page; i < end_page; i++)
    {
        pfn  = page_to_pfn(map->memory->pages[i]);
        retval = vmf_insert_pfn(vma, address, pfn);
        if (unlikely(retval & VM_FAULT_ERROR))
        {
            return retval;
        }
        address += PAGE_SIZE;
    }
    return retval;
}


#if LINUX_VERSION_CODE < KERNEL_VERSION(4,11,0)
static vm_fault_t zx_gem_fault(struct vm_area_struct *vma, struct vm_fault *vmf)
#else
static vm_fault_t zx_gem_fault(struct vm_fault *vmf)
#endif
{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,11,0)
    struct vm_area_struct *vma = vmf->vma;
#endif
    struct drm_zx_gem_object *obj;
    unsigned long offset;
    uint64_t vma_node_offset;
    vm_fault_t ret = VM_FAULT_NOPAGE;
    unsigned long address;
    unsigned int prefault_num = 0;

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4,10,0)
    address = vmf->address;
#else
    address = (unsigned long)vmf->virtual_address;
#endif
    obj = to_zx_bo(vma->vm_private_data);

    vma_node_offset = drm_vma_node_offset_addr(&(obj->base.vma_node));

    if (obj->prefault_num == 0)
    {
        prefault_num = (unsigned int)ZX_DEFAULT_PREFAULT_NUM;
    }
    else
    {
        prefault_num = obj->prefault_num;
    }
    offset = ((vma->vm_pgoff << PAGE_SHIFT) - vma_node_offset) + (address - vma->vm_start);
    prefault_num = min((int)prefault_num, (int)((vma->vm_end - address) >> PAGE_SHIFT));

    switch(obj->map_argu->flags.mem_type)
    {
    case ZX_SYSTEM_IO:
        ret = zx_gem_io_insert(vma, address, obj->map_argu, offset, prefault_num);
        break;
    case ZX_SYSTEM_RAM:
        ret = zx_gem_ram_insert(vma, address, obj->map_argu, offset, prefault_num);
        break;
    default:
        zx_assert(0, "");
        break;
    }

    return ret;
}

static void zx_gem_vm_open(struct vm_area_struct *vma)
{
    struct drm_zx_gem_object *obj = to_zx_bo(vma->vm_private_data);

    zx_drm_gem_object_vm_prepare(obj);
    drm_gem_vm_open(vma);
}

static void zx_gem_vm_close(struct vm_area_struct *vma)
{
    struct drm_zx_gem_object *obj = to_zx_bo(vma->vm_private_data);

    zx_drm_gem_object_vm_release(obj);
    drm_gem_vm_close(vma);
}

static int zx_gem_vm_access(struct vm_area_struct *vma, unsigned long addr, void *buf, int len, int write)
{
    zx_cpu_vm_area_t *kvma = NULL;
    struct drm_zx_gem_object *obj;
    unsigned long offset;
    int rest, this_size;
    char *p;

    obj = to_zx_bo(vma->vm_private_data);
    offset = addr - vma->vm_start + ((vma->vm_pgoff - drm_vma_node_start(&obj->base.vma_node)) << PAGE_SHIFT);
    //zx_info("zx_gem_vm_access(addr:%p, len:%d, write:%d, offset:%llx)\n", (void*)addr, len, write, offset);

    if (len < 1 || (offset + len) > obj->base.size)
    {
        zx_info("access overflow (size:%x, vm_start:%llx, vm_pgoff:%llx, node_offset:%llx)\n",
            obj->base.size, vma->vm_start, vma->vm_pgoff, drm_vma_node_start(&obj->base.vma_node));
        return -EIO;
    }

    kvma = zx_gem_object_vmap(obj);
    if (!kvma)
    {
        zx_info("vmap failed.\n");
        return -EIO;
    }

    rest = len;
    p = (char*)buf;
    if (kvma->node_num == 0)
        goto done;

#define __MIN(a,b)  ((a) > (b) ? (b) : (a))
    if (kvma->node_num == 1)
    {
        this_size = __MIN(rest, obj->base.size - offset);
        if (write)
            zx_memcpy(kvma->virt_addr + offset, p, this_size);
        else
            zx_memcpy(p, kvma->virt_addr + offset, this_size);
        rest -= this_size;
        p += this_size;
    }
    else
    {
        int node_idx = 0;
        long obj_offset = 0;
        long this_offset = 0;

        // skip nodes before offset
        while(node_idx < kvma->node_num &&
              obj_offset + kvma->node_sizes[node_idx] < offset)
        {
            obj_offset += kvma->node_sizes[node_idx];
            ++node_idx;
        }

        this_offset = offset - obj_offset;
        while(rest > 0 && node_idx < kvma->node_num)
        {
            this_size = __MIN(rest, kvma->node_sizes[node_idx] - this_offset);
            if (write)
                zx_memcpy(kvma->virt_addrs[node_idx] + this_offset, p, this_size);
            else
                zx_memcpy(p, kvma->virt_addrs[node_idx] + this_offset, this_size);
            p += this_size;
            rest -= this_size;
            this_offset = 0;
            ++node_idx;
        }
    }

done:
    zx_gem_object_vunmap(obj);
    return len - rest;
}

const struct vm_operations_struct zx_gem_vm_ops = {
    .fault  = zx_gem_fault,
    .open   = zx_gem_vm_open,
    .close  = zx_gem_vm_close,
    .access = zx_gem_vm_access,
};

// priv_list: out, the gem_obj->priv list
// handle_list: in, the gem_obj handle list
int zx_get_gem_priv_list(zx_file_t *priv, void **gem_priv_list, unsigned int *handle_list, int cnt)
{
    int i;
    int valid_cnt = 0;
    struct drm_gem_object *gem;
    struct drm_file *file = priv->parent_file;

    spin_lock(&file->table_lock);
    for (i = 0; i < cnt; i++)
    {
        gem = idr_find(&file->object_idr, handle_list[i]);
        if (gem)
        {
            ++valid_cnt;
            gem_priv_list[i] = to_zx_bo(gem)->priv;
        }
        else
        {
            gem_priv_list[i] = NULL;
        }
    }
    spin_unlock(&file->table_lock);

    return valid_cnt;
}

static int zx_gem_debugfs_show_info(struct seq_file *s, void *unused)
{
    struct zx_gem_debug_info *dnode = (struct zx_gem_debug_info *)(s->private);
    struct drm_zx_gem_object *obj = (struct drm_zx_gem_object *) dnode->parent_gem;
    zx_card_t *zx = obj->base.dev->dev_private;
    adapter_info_t* adapter_info = &zx->adapter_info;
    zx_allocation_info_t *info = &obj->info;
    void *gpu_device = NULL;
    zx_process_info_t proc_info = {0};
    zx_query_info_t query = {0};

    spin_lock(&zx->dev_table_lock);
    gpu_device = idr_find(&zx->dev_idr, dnode->device);
    spin_unlock(&zx->dev_table_lock);

    if(gpu_device){
        query.type = ZX_QUERY_PROCESS_INFO;
        query.buf = ptr_to_ptr64(&proc_info);
        query.in_buf = ptr_to_ptr64(gpu_device);
        zx_core_interface->query_info(zx->adapter, &query);
    }

    zx_drm_gem_query_info(zx, obj, &obj->info);

    /*print the info*/
    seq_printf(s, "handle=0x%x\n", obj->gid);
    seq_printf(s, "device=%d\n", dnode->device);
    seq_printf(s, "width=%d\n", dnode->dbginfo.width);
    seq_printf(s, "height=%d\n", dnode->dbginfo.height);
    seq_printf(s, "pitch=%d\n", dnode->dbginfo.pitch);
    seq_printf(s, "bit_cnt=%d\n", dnode->dbginfo.bit_cnt);
    seq_printf(s, "secured=%d\n", info->secured);
    seq_printf(s, "size=%d\n", info->size);
    seq_printf(s, "tile=%d\n", info->tiled);
    seq_printf(s, "cpu_visible=%d\n", info->cpu_visible);
    seq_printf(s, "cache_type=%d\n", info->cache_type);
    seq_printf(s, "pool_type=%d\n", info->pool_type);
    seq_printf(s, "at_type=%d\n", dnode->dbginfo.at_type);
    seq_printf(s, "segmentid=%d\n", info->segment_id);
    seq_printf(s, "hw_format=%d\n", dnode->dbginfo.hw_format);
    seq_printf(s, "compress_format=%d\n", info->compress_format);
    seq_printf(s, "bl_slot_index=%d\n", info->bl_slot_index);
    seq_printf(s, "gpu_virt_addr=%lld\n", 0ll);
    seq_printf(s, "imported=%d\n", dnode->is_dma_buf_import);
    seq_printf(s, "fb_offset=%lld\n", info->cpu_phy_addr ? info->cpu_phy_addr-adapter_info->fb_bus_addr : 0);
    seq_printf(s, "share=%d\n", info->share);
    seq_printf(s, "local_continuous=%d\n", info->local_continuous);

    if(gpu_device){
        seq_printf(s, "create_proc=%s\n", proc_info.pname);
        seq_printf(s, "create_pid=%d\n", proc_info.pid);
    }

    return 0;
}

int zx_gem_open_ioctl(struct drm_file *file, zx_gem_open_t *open)
{
    int ret = 0;
    zx_file_t *priv = file->driver_priv;
    zx_card_t *zx  = priv->card;
    unsigned int gem_handle = 0;
    struct drm_device *dev = file->minor->dev;
    struct drm_zx_gem_object *obj;

    spin_lock(&zx->gem_gid_lock);
    obj = idr_find(&zx->gem_gid_table, open->gid);
    if (obj)
        zx_gem_object_get(obj);
    spin_unlock(&zx->gem_gid_lock);

    if (!obj)
        return -ENOENT;

    ret = drm_gem_handle_create(file, &obj->base, &gem_handle);
    if (ret != 0)
        return ret;

    open->handle = gem_handle;

    zx_gem_object_put(obj);

    return 0;
}

int zx_gem_set_dbg_info_ioctl(struct drm_file *file, zx_set_gem_dbg_info_t *args)
{
    int ret = 0;
    zx_file_t *priv = file->driver_priv;
    zx_card_t *zx  = priv->card;
    struct drm_device *dev = file->minor->dev;
    struct drm_zx_gem_object *obj;

    obj = zx_drm_gem_object_lookup(dev, file, args->handle);
    if (!obj)
        return -ENOENT;

    zx_memcpy(&obj->debug.dbginfo, &args->info, sizeof(zx_gem_dbg_info_t));

    trace_zxgfx_drm_gem_set_metadata(zx->index, obj);

    zx_gem_object_put(obj);

    return 0;
}

static int zx_gem_debugfs_info_open(struct inode *inode, struct file *file)
{
    return single_open(file, zx_gem_debugfs_show_info, inode->i_private);
}

static const struct file_operations debugfs_gem_info_fops = {
    .open       = zx_gem_debugfs_info_open,
    .read       = seq_read,
    .llseek     = seq_lseek,
    .release    = single_release,
};

static ssize_t zx_gem_debugfs_data_read(struct file *f, char __user *buf,
                     size_t size, loff_t *pos)
{
  //map & read
    zx_gem_debug_info_t *dbg = file_inode(f)->i_private;
    struct drm_zx_gem_object *obj = dbg->parent_gem;
    void *data = NULL;
    ssize_t result = 0;
    int data_off = 0;
    int copy_size = 0;
    int a_size = 0;
    zx_cpu_vm_area_t *vma = NULL;
    int i = 0, accum_size = 0, start_node_index = 0, start_node_offset = 0, start_node_remain_size = 0;

    vma = zx_gem_object_vmap(obj);
    if (vma) {
        zx_gem_object_begin_cpu_access(obj, 2 * HZ, 0);

        a_size = obj->krnl_vma->size;
        data_off = *pos;
        copy_size = min(size, (size_t)(a_size - data_off));

        if(vma->node_num == 1){
            if(copy_to_user(buf, vma->virt_addr+data_off, copy_size)){
                result = -EFAULT;
            }else{
                result += copy_size;
                *pos += copy_size;
            }
        }else{
            for(i=0; i<vma->node_num; i++){
                accum_size += vma->node_sizes[i];
                if(data_off < accum_size){
                    start_node_index = i;
                    start_node_offset = vma->node_sizes[start_node_index] - (accum_size - data_off);
                    break;
                }
            }

            start_node_remain_size = vma->node_sizes[start_node_index] - start_node_offset;
            if(copy_size <= start_node_remain_size){
                if (copy_to_user(buf,  (vma->virt_addrs[start_node_index] + start_node_offset), copy_size)){
                    result = -EFAULT;
                } else {
                    result += copy_size;
                    *pos += copy_size;
                }
            }else{
                if (copy_to_user(buf,  (vma->virt_addrs[start_node_index] + start_node_offset), start_node_remain_size)){
                    result = -EFAULT;
                } else {
                    result += start_node_remain_size;
                    *pos += start_node_remain_size;
                }
                for(i=start_node_index+1; i<vma->node_num; i++){
                    if((copy_size - result) <= vma->node_sizes[i]){
                        if (copy_to_user(buf+result,  vma->virt_addrs[i], (copy_size - result))){
                            result = -EFAULT;
                        } else {
                            result += (copy_size - result);
                            *pos += (copy_size - result);
                        }

                        break;
                    }else{
                        if (copy_to_user(buf+result,  vma->virt_addrs[i], vma->node_sizes[i])){
                            result = -EFAULT;
                        } else {
                            result += vma->node_sizes[i];
                            *pos += vma->node_sizes[i];
                        }
                    }
                }
            }
        }

        zx_gem_object_end_cpu_access(obj,0);
    }
    else {
        result = -EINVAL;
    }
    zx_gem_object_vunmap(obj);

    return result;
}

static const struct file_operations debugfs_gem_data_fops = {
    .read       = zx_gem_debugfs_data_read,
    .llseek     = default_llseek,
};

static ssize_t zx_gem_debugfs_control_write(struct file *f, const char __user *buf,
                                         size_t size, loff_t *pos)
{
    zx_gem_debug_info_t *dbg = file_inode(f)->i_private;
    struct drm_zx_gem_object *obj = dbg->parent_gem;
    zx_card_t *zx = obj->base.dev->dev_private;
    char c = 0;
    ssize_t ret = 0;

    ret = simple_write_to_buffer(&c, 1, pos, buf, size);
    if (ret > 0) {
        if (c != '0' && !dbg->mark_unpagable) {
            zx_core_interface->prepare_and_mark_unpagable(zx->adapter, obj->priv);
            dbg->mark_unpagable = true;
        } else if (c == '0' && dbg->mark_unpagable) {
            zx_core_interface->mark_pagable(zx->adapter, obj->priv);
            dbg->mark_unpagable = false;
        }
        ret = size;
    }
    return ret;
}

static ssize_t zx_gem_debugfs_control_read(struct file *f, char __user *buf,
                     size_t size, loff_t *pos)
{
    zx_gem_debug_info_t *dbg = file_inode(f)->i_private;
    struct drm_zx_gem_object *obj = dbg->parent_gem;
    char c = dbg->mark_unpagable ? '1' : '0';

    return simple_read_from_buffer(buf, size, pos, &c, 1);
}

static const struct file_operations debugfs_gem_control_fops = {
    .write      = zx_gem_debugfs_control_write,
    .read       = zx_gem_debugfs_control_read,
    .llseek     = default_llseek,
};

static int zx_gem_debugfs_vm_info_show(struct seq_file *s, void *unused)
{
    struct zx_gem_debug_info *dnode   = (struct zx_gem_debug_info *)(s->private);
    struct drm_zx_gem_object *obj     = (struct drm_zx_gem_object *) dnode->parent_gem;
    zx_card_t                *zx      = obj->base.dev->dev_private;
    struct os_seq_file       seq_file = {0};
    struct os_printer        seq_p    = zx_seq_file_printer(&seq_file);

    seq_file.seq_file = s;

    zx_core_interface->debugfs_dump(&seq_p, zx->adapter, DEBUGFS_NODE_VM_NODE_INFO, obj->priv);

    return 0;
}

static int zx_gem_debugfs_vm_info_open(struct inode *inode, struct file *file)
{
    return single_open(file, zx_gem_debugfs_vm_info_show, inode->i_private);
}

static const struct file_operations debugfs_gem_vm_fops = {
    .open       = zx_gem_debugfs_vm_info_open,
    .read       = seq_read,
    .llseek     = seq_lseek,
    .release    = single_release,
};

int zx_gem_debugfs_add_object(struct drm_zx_gem_object *obj)
{
    zx_gem_debug_info_t *dbg = &obj->debug;
    int result = 0;

    dbg->is_cpu_accessable = true; //s houdle false for cpu invisable

    zx_vsprintf(dbg->name,"%08x", obj->gid);

    dbg->self_dir = debugfs_create_dir(dbg->name, dbg->root);
    if (dbg->self_dir) {
        dbg->alloc_info = debugfs_create_file("info", 0444, dbg->self_dir, dbg, &debugfs_gem_info_fops);
        dbg->data = debugfs_create_file("data", 0444, dbg->self_dir, dbg, &debugfs_gem_data_fops);
        dbg->control = debugfs_create_file("control", 0444, dbg->self_dir, dbg, &debugfs_gem_control_fops);
        dbg->vm_info = debugfs_create_file("vm_info", 0444, dbg->self_dir, dbg, &debugfs_gem_vm_fops);
    }else {
        zx_error("create allocation %s dir failed\n", dbg->name);
        result = -1;
    }
    return result;
}

void zx_gem_debugfs_remove_object(struct drm_zx_gem_object *obj)
{
    zx_gem_debug_info_t *dbg = &obj->debug;
    if (dbg->self_dir) {

        debugfs_remove(dbg->alloc_info);
        debugfs_remove(dbg->data);
        debugfs_remove(dbg->control);
        debugfs_remove(dbg->vm_info);
        debugfs_remove(dbg->self_dir);

        dbg->alloc_info = NULL;
        dbg->data = NULL;
        dbg->self_dir = NULL;
        dbg->control = NULL;
        dbg->vm_info = NULL;
    }
}

