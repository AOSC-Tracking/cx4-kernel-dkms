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
#include "zx_def.h"
#include "zx_ioctl.h"
#include "os_interface.h"
#include "kernel_interface.h"
#include "zx_debugfs.h"
#include "zx_gem.h"
#include "zx_gem_priv.h"
#include "zx_disp.h"
#include "zx_drm_syncobj.h"
#include "zx_fence.h"
#if LINUX_VERSION_CODE < KERNEL_VERSION(4,10,0)
#include "zx_dma_fence_array.h"
#else
#include <linux/dma-fence-array.h>
#endif

#define LOCAL_ALLOC(SIZE)  ({   \
        void *__p = pcurrent;   \
        pcurrent += (SIZE);     \
        zx_assert(pcurrent <= payload + payload_size, "bug!!!"); \
        __p; \
    })

struct zx_context{
    void *ctx;
    int engine_index;
    unsigned int client_type;
    dma_fence_t *last_queued_dma_fence;
};

int zx_ioctl_wait_chip_idle(struct drm_device *dev, void *data,struct drm_file *filp)
{
    zx_file_t *priv = filp->driver_priv;
    zx_card_t  *zx = priv->card;

    zx_core_interface->wait_chip_idle(zx->adapter);

    return 0;
}

//#ifdef ZX_HW_NULL
//create alloaction route for hw_null path
int zx_create_allocation_hw_null(zx_file_t *priv, zx_create_allocation_t *create)
{
    zx_error("%s: not implement.\n", __func__);

#if 0
    zx_card_t *zx  = priv->card;
    unsigned int i, size = 0;
    zx_create_allocation_info_t *alloc_info = NULL;
    zx_ptr64_t __user alloc_info_user = create->alloc_info;
    int result = S_OK;

    size = sizeof(zx_create_allocation_info_t) * create->num_allocations;
    alloc_info = zx_malloc(size);
    zx_assert(alloc_info != NULL, "");

    zx_copy_from_user(alloc_info, ptr64_to_ptr(create->alloc_info), size);
    create->alloc_info = ptr_to_ptr64(alloc_info);

    result = zx_core_interface->create_allocation(zx->adapter, create, NULL);
    if(result != S_OK)
    {
        if(alloc_info)
        {
            zx_free(alloc_info);
        }
        return E_FAIL;
    }

    if(create->device)
    {
        unsigned long long  *allocation_list = NULL;
        unsigned long long   allocation_list_stacks[8];

        if(create->num_allocations > 8)
        {
            allocation_list = zx_malloc(sizeof(unsigned long long) * create->num_allocations);
        }
        else
        {
            allocation_list = &allocation_list_stacks[0];
        }

        for(i = 0; i < create->num_allocations; i ++)
        {
            allocation_list[i] = zx_get_from_gem_handle(priv->parent_file, alloc_info[i].allocation);
        }

        zx_core_interface->bind_allocation(create->device, allocation_list, create->num_allocations);
    }

    create->alloc_info = alloc_info_user;
    zx_copy_to_user(ptr64_to_ptr(create->alloc_info), alloc_info, size);

    zx_free(alloc_info);
#endif
    return S_OK;
}
//#endif

int zx_ioctl_gem_create_allocation(struct drm_device *dev, void *data,struct drm_file *filp)
{
    int                    ret  = -1;
    zx_file_t              *priv = filp->driver_priv;
    zx_create_allocation_t *create = (zx_create_allocation_t *)data;

#ifdef ZX_HW_NULL
    ret = zx_create_allocation_hw_null(priv, create);
#else
    ret = zx_drm_gem_create_object_ioctl(priv->parent_file, create);
#endif

    return ret;
}

int zx_ioctl_wait_allocation_idle(struct drm_device *dev, void *data,struct drm_file *filp)
{
    zx_file_t                 *priv = filp->driver_priv;
    zx_card_t                 *zx  = priv->card;

    zx_wait_allocation_idle_t  *wait_allocation = (zx_wait_allocation_idle_t *)data;

    return zx_gem_wait_allocation_idle(priv, wait_allocation);
}

int zx_ioctl_set_gem_dbg_info(struct drm_device *dev, void *data,struct drm_file *filp)
{
    zx_file_t                 *priv = filp->driver_priv;
    zx_set_gem_dbg_info_t     *set = (zx_set_gem_dbg_info_t *)data;

    zx_gem_set_dbg_info_ioctl(priv->parent_file, set);

    return 0;
}

int zx_ioctl_gem_open(struct drm_device *dev, void *data,struct drm_file *filp)
{
    int                 ret = 0;
    zx_file_t           *priv = filp->driver_priv;
    zx_gem_open_t       *open = (zx_gem_open_t *)data;

    ret = zx_gem_open_ioctl(priv->parent_file, open);

    return ret;
}

int zx_ioctl_query_info(struct drm_device *dev, void *data,struct drm_file *filp)
{
    zx_file_t *priv = filp->driver_priv;
    zx_card_t  *zx  = priv->card;
    zx_query_info_t *info = (zx_query_info_t *)data;
    int ret = 0;

    switch(info->type)
    {
    case ZX_QUERY_ALLOCATION_RESIDENCY:
        {
            zx_query_info_t kinfo = {0, };
            unsigned int    *klist = NULL; // allocation handle list
            void            **apriv_list = NULL;   // allocation priv list
            unsigned int    *status_list = NULL;   // output status list

            kinfo.type = ZX_QUERY_ALLOCATION_RESIDENCY;
            kinfo.argu = info->argu; // num_allocations;
            klist = (unsigned int*)zx_malloc(info->argu * (sizeof(unsigned int) + sizeof(unsigned int) + sizeof(void*)));
            apriv_list = (void**)(klist + info->argu);
            kinfo.in_buf = ptr_to_ptr64(apriv_list);
            kinfo.buf = ptr_to_ptr64(apriv_list + info->argu);

            zx_copy_from_user(klist, ptr64_to_ptr(info->in_buf), info->argu * sizeof(unsigned int));
            zx_get_gem_priv_list(priv, apriv_list, klist, info->argu);

            ret = zx_core_interface->query_info(zx->adapter, info);
            if (ret == 0)
                zx_copy_to_user(ptr64_to_ptr(info->buf), ptr64_to_ptr(kinfo.buf), sizeof(unsigned int) * info->argu);
            zx_free(klist);
            goto done;
        }
        break;
    case ZX_QUERY_ALLOCATION_INFO:
        {
            zx_allocation_info_t ainfo = {0, };

            ret = zx_drm_gem_query_alloc_info(priv, info->argu, &ainfo);
            if (ret == 0)
                zx_copy_to_user(ptr64_to_ptr(info->buf), &ainfo, sizeof(ainfo));
            goto done;
        }
        break;
    case ZX_QUERY_ENGINE_CLOCK:
        {
            if (DISP_OK == disp_cbios_get_clock(zx->disp_info, ZX_QUERY_ENGINE_CLOCK, &info->value))
            {
                info->value /= 1000;//cbios clock is KHZ, change to MHZ
                goto done;
            }
            else
            {
                return -1;
            }
        }
    case ZX_QUERY_VIDEO_SEQ_INDEX:
        info->in_buf = ptr_to_ptr64(priv->gpu_device);
        break;
    default:
        break;
    }
    ret = zx_core_interface->query_info(zx->adapter, info);
done:
    return ret;
}

int zx_ioctl_create_device(struct drm_device *dev, void *data,struct drm_file *filp)
{
    zx_file_t             *priv = filp->driver_priv;
    zx_card_t             *zx  = priv->card;
    zx_create_device_t    *create_device = (zx_create_device_t *)data;
    char pname[16] = {0, };

    create_device->device = priv->gpu_device_handle;

    if (priv->debug) {
        priv->debug->user_pid = zx_get_current_pid();
    }

    zx_get_current_pname(pname, sizeof(pname));
    zx_core_interface->update_device_info(zx->adapter,
                                          priv->gpu_device,
                                          zx_get_current_pid(),
                                          zx_get_current_tid(),
                                          pname);

    return 0;
}

int zx_ioctl_begin_perf_event(struct drm_device *dev, void *data,struct drm_file *filp)
{
    zx_file_t   *priv = filp->driver_priv;
    zx_card_t   *zx = priv->card;
    zx_begin_perf_event_t *begin_perf_event = (zx_begin_perf_event_t *)data;
    int          ret = 0;

    ret = zx_core_interface->begin_perf_event(zx->adapter, begin_perf_event);

    return ret;
}

int zx_ioctl_begin_miu_dump_perf_event(struct drm_device *dev, void *data,struct drm_file *filp)
{
    zx_file_t *priv = filp->driver_priv;
    zx_card_t *zx = priv->card;
    zx_begin_miu_dump_perf_event_t *begin_miu_dump = (zx_begin_miu_dump_perf_event_t *)data;
    int ret = 0;

    ret = zx_core_interface->begin_miu_dump_perf_event(zx->adapter, begin_miu_dump);

    return ret;
}

int zx_ioctl_end_perf_event(struct drm_device *dev, void *data,struct drm_file *filp)
{
    zx_file_t   *priv = filp->driver_priv;
    zx_card_t   *zx = priv->card;
    zx_end_perf_event_t *end_perf_event = (zx_end_perf_event_t *)data;
    int          ret = 0;

    ret = zx_core_interface->end_perf_event(zx->adapter, end_perf_event);

    return ret;
}

int zx_ioctl_end_miu_dump_perf_event(struct drm_device *dev, void *data,struct drm_file *filp)
{
    zx_file_t *priv = filp->driver_priv;
    zx_card_t *zx = priv->card;
    zx_end_miu_dump_perf_event_t *end_miu_dump = (zx_end_miu_dump_perf_event_t*)data;
    int ret = 0;

    ret = zx_core_interface->end_miu_dump_perf_event(zx->adapter, end_miu_dump);

    return ret;
}


static int zx_ioctl_set_miu_reg_list_perf_event(struct drm_device *dev, void *data,struct drm_file *filp)
{
    zx_file_t *priv = filp->driver_priv;
    zx_card_t *zx = priv->card;
    zx_miu_reg_list_perf_event_t *miu_reg_list = (zx_miu_reg_list_perf_event_t *)data;
    int ret = 0;

    ret = zx_core_interface->set_miu_reg_list_perf_event(zx->adapter, miu_reg_list);

    return ret;
}

static int zx_ioctl_get_miu_dump_perf_event(struct drm_device *dev, void *data,struct drm_file *filp)
{
    zx_file_t *priv = filp->driver_priv;
    zx_card_t *zx = priv->card;
    zx_get_miu_dump_perf_event_t *get_miu_dump = (zx_get_miu_dump_perf_event_t *)data;
    int ret = 0;

    ret = zx_core_interface->get_miu_dump_perf_event(zx->adapter, get_miu_dump);

    return ret;
}

static int zx_ioctl_direct_get_miu_dump_perf_event(struct drm_device *dev, void *data,struct drm_file *filp)
{
    zx_file_t *priv = filp->driver_priv;
    zx_card_t *zx = priv->card;
    zx_direct_get_miu_dump_perf_event_t *direct_get_dump = (zx_direct_get_miu_dump_perf_event_t *)data;
    int ret = 0;

    ret = zx_core_interface->direct_get_miu_dump_perf_event(zx->adapter, direct_get_dump);

    return ret;
}

static int zx_ioctl_get_perf_event(struct drm_device *dev, void *data,struct drm_file *filp)
{
    zx_file_t   *priv = filp->driver_priv;
    zx_card_t   *zx = priv->card;
    zx_get_perf_event_t *get_perf_event = (zx_get_perf_event_t *)data;
    int          ret = 0;

    ret = zx_core_interface->get_perf_event(zx->adapter, get_perf_event);

    return ret;
}

int zx_ioctl_send_perf_event(struct drm_device *dev, void *data,struct drm_file *filp)
{
    zx_file_t *priv = filp->driver_priv;
    zx_card_t   *zx = priv->card;
    zx_perf_event_header_t *perf_event_head = (zx_perf_event_header_t *)data;
    zx_perf_event_t perf_event;
    int          ret = 0;

    if(perf_event_head->size > sizeof(zx_perf_event_t))
    {
        return -1;
    }

    if (perf_event_head->size >= sizeof(zx_perf_event_header_t))
    {
        zx_memcpy((void *)&perf_event, data, perf_event_head->size);
        ret = zx_core_interface->send_perf_event(zx->adapter, &perf_event);
    }

    return ret;
}


int zx_ioctl_get_perf_status(struct drm_device *dev, void *data,struct drm_file *filp)
{
    zx_file_t   *priv = filp->driver_priv;
    zx_card_t   *zx = priv->card;
    zx_perf_status_t *perf_status = (zx_perf_status_t *)data;
    int ret = 0;

    ret = zx_core_interface->get_perf_status(zx->adapter, perf_status);

    return ret;
}


int zx_ioctl_create_context(struct drm_device *dev, void *data,struct drm_file *filp)
{
    int                 ret = 0;
    zx_file_t           *priv = filp->driver_priv;
    zx_card_t           *zx  = priv->card;
    zx_create_context_t  *create_context = (zx_create_context_t *)data;
    struct zx_context *ctx = zx_calloc(sizeof(struct zx_context));

    ctx->ctx = zx_core_interface->create_context(zx->adapter, priv->gpu_device, create_context);
    if (!ctx->ctx){
        zx_free(ctx);
        return -1;
    }
    ctx->engine_index = create_context->engine_index;
    ctx->client_type = create_context->client_type;

    spin_lock(&priv->ctx_table_lock);
    ret = idr_alloc_cyclic(&priv->ctx_idr, (void*)ctx, 0x3000000, 0, GFP_NOWAIT);
    spin_unlock(&priv->ctx_table_lock);

    if (ret < 0)
        return -1;

    create_context->context = ret;

    return 0;
}

int zx_ioctl_destroy_context(struct drm_device *dev, void *data,struct drm_file *filp)
{
    zx_file_t       *priv = filp->driver_priv;
    zx_card_t       *zx = priv->card;
    zx_destroy_context_t *destroy = (zx_destroy_context_t *)data;
    struct zx_context *ctx = NULL;
    void            *device = NULL;
    int ret = 0;

    spin_lock(&priv->ctx_table_lock);
    ctx = (struct zx_context*)idr_find(&priv->ctx_idr, destroy->context);
    if (ctx)
        idr_remove(&priv->ctx_idr, destroy->context);
    spin_unlock(&priv->ctx_table_lock);

    if (!ctx)
        return -1;

    ret = zx_core_interface->destroy_context(zx->adapter, priv->gpu_device, ctx->ctx);

    if(ctx->last_queued_dma_fence)
        dma_fence_put(ctx->last_queued_dma_fence);

    zx_free(ctx);

    return ret;
}

extern void zx_invalid_resident_list_cache(zx_file_t *priv, struct zx_resident_list_cache *cache);
extern void zx_update_resident_list_cache(zx_file_t *priv, struct zx_resident_list_cache *cache);


static void free_batch(batch_parser_t *b)
{
    int i;
    zx_file_t *priv = b->os_priv;
    struct drm_zx_gem_object **bos = b->bos;

    if (bos)
    {
        for (i = 0; i < b->bos_cnt; i++)
        {
            if (bos[i])
            {
                zx_gem_object_put(bos[i]);
            }
            bos[i] = NULL;
        }
    }

    if (b->resident_list)
    {
        zx_mutex_lock(priv->resident_lock);
        for (i = 0; i < ARRAY_SIZE(priv->resident_list_cache); i++)
        {
            struct zx_resident_list_cache *cache = priv->resident_list_cache + i;

            if (cache->in_use && cache->resident_list == b->resident_list)
            {
                b->resident_list = NULL;
                if (--cache->in_use == 0 && cache->dirty_count < priv->resident_dirty_count)
                {
                    // need invalid cache list
                    zx_invalid_resident_list_cache(priv, cache);
                }
                break;
            }
        }
        zx_mutex_unlock(priv->resident_lock);
    }

    if (b->resident_list)
    {
        zx_free(b->resident_list);
    }

    zx_free(b);
}

#define OBJ_DEFAULT_COUNTS 8
static batch_parser_t * alloc_batch_from_render(zx_file_t *priv, zx_render_t * r, int bos_cnt)
{
    batch_parser_t *b = NULL;
    unsigned char *payload = NULL;
    unsigned int payload_size = 0;
    unsigned char *pcurrent = NULL;

    payload_size = sizeof(batch_parser_t) +
                   r->dma_cnt * (
                                sizeof(unsigned long long) +    // cmd va
                                sizeof(unsigned int)) +    // cmd length
                   r->primary_cnt * (sizeof(unsigned int) +  // primary handle list
                                     sizeof(unsigned int)) +  // write_op_list
                   r->wait_fence_cnt * sizeof(unsigned int) +  // wait fence cnt
                   r->signal_fence_cnt * sizeof(unsigned int) + // signal fence_cnt
                   r->primary_cnt * sizeof(void*) +              // primary priv list
                   bos_cnt * sizeof(void*)
               ;
    payload = zx_calloc(payload_size);
    pcurrent = payload;

    /*now setup the batch payload */
    b = LOCAL_ALLOC(sizeof(batch_parser_t));
    b->dma_cnt = r->dma_cnt;
    b->primary_cnt = r->primary_cnt;
    b->uflag = r->uflag;
    b->wait_fence_cnt = r->wait_fence_cnt;
    b->signal_fence_cnt = r->signal_fence_cnt;
    b->ctx_buf_va = r->ctx_buf_va;
    b->os_priv = priv;

    if(r->dma_cnt > 0) {
        b->dma_gpu_va = LOCAL_ALLOC(r->dma_cnt * sizeof(unsigned long long));
        zx_copy_from_user(b->dma_gpu_va, ptr64_to_ptr(r->dma_gpu_va), r->dma_cnt * sizeof(unsigned long long));

        b->dma_length = LOCAL_ALLOC(r->dma_cnt * sizeof(unsigned int));
        zx_copy_from_user(b->dma_length, ptr64_to_ptr(r->dma_length), r->dma_cnt * sizeof(unsigned int));
    }

    /*primary list*/
    if (r->primary_cnt > 0) {
        unsigned int *primary_handle_list;

        primary_handle_list = LOCAL_ALLOC(r->primary_cnt * sizeof(unsigned int));
        zx_copy_from_user(primary_handle_list, ptr64_to_ptr(r->primary_list), r->primary_cnt * sizeof(unsigned int));

        b->primary_list = LOCAL_ALLOC(r->primary_cnt * sizeof(void*));
        zx_get_gem_priv_list(priv, b->primary_list, primary_handle_list, r->primary_cnt);

        b->primary_write_op_list = LOCAL_ALLOC(r->primary_cnt * sizeof(unsigned int));
        zx_copy_from_user(b->primary_write_op_list, ptr64_to_ptr(r->primary_write_op_list), r->primary_cnt * sizeof(unsigned int));
    }

    //wait fence
    if (r->wait_fence_cnt > 0) {
        b->wait_fence = LOCAL_ALLOC(r->wait_fence_cnt * sizeof(unsigned int));
        zx_copy_from_user(b->wait_fence, ptr64_to_ptr(r->wait_fence), r->wait_fence_cnt * sizeof(unsigned int));
    }

    //signal fence
    if (r->signal_fence_cnt > 0) {
        b->signal_fence = LOCAL_ALLOC(r->signal_fence_cnt * sizeof(unsigned int));
        zx_copy_from_user(b->signal_fence, ptr64_to_ptr(r->signal_fence), r->signal_fence_cnt * sizeof(unsigned int));
    }

    if (bos_cnt > 0)
        b->bos = LOCAL_ALLOC(bos_cnt * sizeof(void*));

    b->release = free_batch;
    return b;
}


int zx_ioctl_render(struct drm_device *dev, void *data,struct drm_file *filp)
{
    zx_file_t   *priv = filp->driver_priv;
    zx_card_t   *zx    = priv->card;
    struct zx_context *ctx = NULL;
    void *drm_syncobj = NULL;
    int i = 0;
    struct drm_file *file = priv->parent_file;
    struct zx_dma_fence_context *context = NULL;
#if LINUX_VERSION_CODE < KERNEL_VERSION(4,10,0)
    struct zx_dma_fence_array *zx_dma_fence_array = NULL;
#else
    struct dma_fence_array *dma_fence_array = NULL;
#endif
    int total_num_wait_fence = 0, accum_fences = 0;
    dma_fence_t **wait_fences = NULL;
    struct zx_dma_fence_cb *fence_cb = NULL;
    unsigned int *primary_handle_list = NULL;
    struct drm_zx_gem_object **bos = NULL;
    int bos_cnt = 0;
    struct zx_bo_wait_fences *bo_wait_fences_list = NULL;
    zx_render_t  *render      = (zx_render_t *)data;
    int           ret         = 0;
    batch_parser_t * b = NULL;

    zx_begin_section_trace_event("zx_ioctl_render");
    zx_counter_trace_event("arg_dma_cnt", render->dma_cnt);

    spin_lock(&priv->ctx_table_lock);
    ctx = (struct zx_context*)idr_find(&priv->ctx_idr, render->context);
    spin_unlock(&priv->ctx_table_lock);

    if (!ctx)
    {
        ret = -ENOENT;
        goto done;
    }

    context = zx_context_by_engine(zx->fence_drv, ctx->engine_index);
    bos_cnt = render->primary_cnt;
    if (ctx->client_type == CLIENT_TYPE_VULKAN)
    {
        bos_cnt += priv->resident_cnt;
    }
    b = alloc_batch_from_render(priv, render, bos_cnt);
    b->bos_cnt = b->primary_cnt;
    bos = b->bos;

    if(b->primary_cnt)
    {
        primary_handle_list = zx_calloc(b->primary_cnt * sizeof(unsigned int));
        bo_wait_fences_list = zx_calloc(b->primary_cnt * sizeof(struct zx_bo_wait_fences));

        zx_copy_from_user(primary_handle_list, ptr64_to_ptr(render->primary_list), b->primary_cnt * sizeof(unsigned int));

        for(i=0; i<b->primary_cnt; i++)
        {
            // hold a ref_count
            bos[i] = zx_drm_gem_object_lookup(file->minor->dev, file, primary_handle_list[i]);
            if(bos[i] == 0)
            {
                zx_error("%s can't find gem:%d\n", __func__, primary_handle_list[i]);
            }
        }

        for(i=0; i<b->primary_cnt; i++)
        {
            zx_get_bo_wait_fences(bos[i], context, b->primary_write_op_list[i], &bo_wait_fences_list[i]);
            total_num_wait_fence += bo_wait_fences_list[i].num_fences;
        }
    }

    if (ctx->client_type == CLIENT_TYPE_VULKAN)
    {
        zx_mutex_lock(priv->resident_lock);
        if (priv->resident_cnt > 0)
        {
            int i;
            struct zx_resident_node *rnode = NULL;
            struct zx_resident_list_cache *cache;
            b->resident_cnt = priv->resident_cnt;

            // first choose a un dirty slot
            for (i = 0; i < ARRAY_SIZE(priv->resident_list_cache); i++)
            {
                cache = priv->resident_list_cache + i;
                if (cache->dirty_count == priv->resident_dirty_count)
                {
                    b->resident_list = cache->resident_list;
                    ++cache->in_use;
                    ++priv->debug->resident_list_cache_hit;
                    break;
                }
            }

            if (!b->resident_list)
            {
                // find a unused slot in cache
                for (i = 0; i < ARRAY_SIZE(priv->resident_list_cache); i++)
                {
                    cache = priv->resident_list_cache + i;
                    if (!cache->in_use)
                    {
                        zx_update_resident_list_cache(priv, cache);
                        cache->in_use = 1;
                        b->resident_list = cache->resident_list;
                        ++priv->debug->resident_list_cache_miss;
                        break;
                    }
                }
            }
            if (!b->resident_list)
            {
                i = 0;
                b->resident_list = zx_malloc(b->resident_cnt * sizeof(void*));
                list_for_each_entry(rnode, &priv->resident_list, link)
                {
                    bos[b->bos_cnt++] = zx_gem_object_get(rnode->obj);
                    b->resident_list[i++] = rnode->obj->priv;
                }
                ++priv->debug->resident_list_cache_miss;
            }
        }
        zx_mutex_unlock(priv->resident_lock);
    }
    total_num_wait_fence += b->wait_fence_cnt;
    if(total_num_wait_fence > 0)
    {
        wait_fences = kcalloc(total_num_wait_fence, sizeof(dma_fence_t*), GFP_KERNEL);
    }

    for(i=0; i<b->primary_cnt; i++)
    {
        if(bo_wait_fences_list[i].num_fences > 0)
        {
            zx_memcpy((wait_fences + accum_fences), bo_wait_fences_list[i].fences, sizeof(dma_fence_t*)*bo_wait_fences_list[i].num_fences);
            accum_fences += bo_wait_fences_list[i].num_fences;

            kfree(bo_wait_fences_list[i].fences);
            bo_wait_fences_list[i].num_fences = 0;
        }
    }

    for(i=0; i<b->wait_fence_cnt; i++)
    {
        drm_syncobj = zx_drm_syncobj_find(file, b->wait_fence[i]);
        wait_fences[accum_fences++] = (dma_fence_t*)zx_drm_syncobj_fence_get(drm_syncobj);
        if (!wait_fences[accum_fences - 1])
        {
            --accum_fences;
            --total_num_wait_fence;
        }
        zx_drm_syncobj_put_hook(drm_syncobj);
        drm_syncobj = NULL;
    }

    if(accum_fences!=total_num_wait_fence)
    {
        zx_error("accum_fences not equal total_num_wait_fence\n");
    }

    if(total_num_wait_fence > 0)
    {
#if LINUX_VERSION_CODE < KERNEL_VERSION(4,10,0)
        zx_dma_fence_array = zx_dma_fence_array_create(total_num_wait_fence, wait_fences, context->id, ++context->sync_seq, false);
        b->wait_dma_fence = &zx_dma_fence_array->base;
#else
        dma_fence_array = dma_fence_array_create(total_num_wait_fence, wait_fences, context->id, ++context->sync_seq, false);
        b->wait_dma_fence = &dma_fence_array->base;
#endif
        fence_cb = zx_calloc(sizeof(*fence_cb));
        if (!fence_cb)
        {
            zx_error("fence cb calloc fail\n");
            return -1;
        }
        fence_cb->call_back = zx_core_interface->wakeup_worker_thread;
        fence_cb->adapter = zx->adapter;
        fence_cb->engine_index = ctx->engine_index;

        if(dma_fence_add_callback((dma_fence_t*)b->wait_dma_fence, &fence_cb->base, zx_dma_fence_array_callback))
        {
            if(fence_cb)
                zx_free(fence_cb);

            dma_fence_put((dma_fence_t*)b->wait_dma_fence);
            b->wait_dma_fence = NULL;
        }
    }

    b->dma_fence = zx_dma_fence_create(zx->fence_drv, ctx->engine_index, 0xffffffffffffffffULL);

    if(b->dma_fence)
    {
        for(i=0; i<b->primary_cnt; i++)
        {
            zx_dma_fence_attach_bo(bos[i], b->dma_fence, b->primary_write_op_list[i] ? 0 : 1);

            if (b->primary_write_op_list[i])
            {
                zx_check_touch_primary(zx->fence_drv, bos[i]);
            }
        }

        if(render->signal_fence_cnt)
        {
            for(i=0; i<render->signal_fence_cnt; i++)
            {
                drm_syncobj = zx_drm_syncobj_find((void*)priv->parent_file, b->signal_fence[i]);
                if(drm_syncobj)
                {
                    zx_drm_syncobj_replace_fence(drm_syncobj, b->dma_fence);
                    zx_drm_syncobj_put_hook(drm_syncobj);
                    drm_syncobj = NULL;
                }
            }
        }

        if(ctx->last_queued_dma_fence)
            dma_fence_put(ctx->last_queued_dma_fence);

        ctx->last_queued_dma_fence = (dma_fence_t*)b->dma_fence;
        dma_fence_get(ctx->last_queued_dma_fence);
    }

    ret = zx_core_interface->render(zx->adapter, ctx->ctx, b);

    // should not access b->xxx after render
    if (primary_handle_list)
        zx_free(primary_handle_list);
    if (bo_wait_fences_list)
        zx_free(bo_wait_fences_list);
done:
    zx_end_section_trace_event(ret);
    return ret;
}


int zx_ioctl_add_hw_ctx_buf(struct drm_device *dev, void *data,struct drm_file *filp)
{
    zx_file_t        *priv = filp->driver_priv;
    zx_card_t        *zx = priv->card;
    zx_add_hw_ctx_buf_t *add = (zx_add_hw_ctx_buf_t *)data;
    struct zx_context *ctx = NULL;

    spin_lock(&priv->ctx_table_lock);
    ctx = (struct zx_context*)idr_find(&priv->ctx_idr, add->context);
    spin_unlock(&priv->ctx_table_lock);
    if (ctx == NULL)
    {
        zx_error("add hw ctx buf failed\n");
        return -EINVAL;
    }

    return zx_core_interface->add_hw_ctx_buf(zx->adapter, priv->gpu_device, ctx->ctx, add->hw_ctx_buf_index);
}


int zx_ioctl_rm_hw_ctx_buf(struct drm_device *dev, void *data,struct drm_file *filp)
{
    zx_file_t        *priv = filp->driver_priv;
    zx_card_t        *zx = priv->card;
    zx_rm_hw_ctx_buf_t *rm = (zx_rm_hw_ctx_buf_t *)data;
    struct zx_context *ctx = NULL;

    spin_lock(&priv->ctx_table_lock);
    ctx = (struct zx_context*)idr_find(&priv->ctx_idr, rm->context);
    spin_unlock(&priv->ctx_table_lock);
    if (ctx == NULL)
    {
        zx_error("rm hw ctx buf failed\n");

        return -EINVAL;
    }

    return zx_core_interface->rm_hw_ctx_buf(zx->adapter, priv->gpu_device, ctx->ctx, rm->hw_ctx_buf_index);
}

int zx_ioctl_gem_begin_cpu_access(struct drm_device *dev, void *data,struct drm_file *filp)
{
    zx_file_t                     *priv = filp->driver_priv;
    zx_drm_gem_begin_cpu_access_t *begin = (zx_drm_gem_begin_cpu_access_t *)data;

    return zx_gem_object_begin_cpu_access_ioctl(priv->parent_file, begin);
}

int zx_ioctl_gem_end_cpu_access(struct drm_device *dev, void *data,struct drm_file *filp)
{
    zx_file_t                   *priv = filp->driver_priv;
    zx_drm_gem_end_cpu_access_t *end = (zx_drm_gem_end_cpu_access_t *)data;

    zx_gem_object_end_cpu_access_ioctl(priv->parent_file, end);

    return 0;
}

int zx_ioctl_kms_get_pipe_from_crtc(struct drm_device *dev, void *data,struct drm_file *filp)
{
    zx_file_t                   *priv = filp->driver_priv;
    zx_kms_get_pipe_from_crtc_t *get = (zx_kms_get_pipe_from_crtc_t *)data;

#ifndef ZX_HW_NULL
    if (disp_get_pipe_from_crtc(priv, get))
    {
        return -1;
    }
#endif
    return 0;
}

extern struct dma_fence* zx_dma_fence_get_stub(void);
int zx_ioctl_signal_sync_object(struct drm_device *dev, void *data,struct drm_file *filp)
{
    zx_file_t   *priv       = filp->driver_priv;
    zx_card_t   *zx         = priv->card;
    int          i, result  = 0;

    zx_signal_sync_object_t  *signal = (zx_signal_sync_object_t *)data;
    unsigned int       _statck_sync_obj_list[OBJ_DEFAULT_COUNTS]= {0,};
    unsigned int       _stack_context_list[OBJ_DEFAULT_COUNTS]  = {0,};
    unsigned int       *k_sync_object_list                      = _statck_sync_obj_list;
    unsigned int       *k_context_list                          = _stack_context_list;
    unsigned int       __user  *p1, *p3 = NULL;
    void *drm_syncobj = NULL;
    struct zx_context *ctx = NULL;

    p1 = ptr64_to_ptr(signal->context_list);
    p3 = ptr64_to_ptr(signal->sync_object_list);

    if(signal->num_context > OBJ_DEFAULT_COUNTS)
    {
        k_context_list = zx_malloc(sizeof(unsigned int) * signal->num_context);
        if(k_context_list == NULL)
        {
            zx_error("signal syncobject alloc context failed\n");
            result= -ENOMEM;
            goto alloc_context_fail;
        }

    }

    if(signal->sync_object_cnt > OBJ_DEFAULT_COUNTS)
    {
        k_sync_object_list = zx_malloc(sizeof(unsigned int) * signal->sync_object_cnt);
        if(k_sync_object_list == NULL)
        {
            zx_error("signal syncobject alloc sync failed\n");
            result= -ENOMEM;
            goto alloc_sync_fail;
        }
    }

    zx_copy_from_user(k_context_list, p1, sizeof(unsigned int) * signal->num_context);
    zx_copy_from_user(k_sync_object_list, p3, sizeof(unsigned int) * signal->sync_object_cnt);

    spin_lock(&priv->ctx_table_lock);
    ctx = (struct zx_context*)idr_find(&priv->ctx_idr, k_context_list[0]);
    spin_unlock(&priv->ctx_table_lock);
    if(ctx == NULL)
    {
        zx_error("signal syncobject ctx buf failed\n");
        result = -EINVAL;
        goto ctx_err;
    }

    for(i=0; i<signal->sync_object_cnt; i++){
        drm_syncobj = zx_drm_syncobj_find(priv->parent_file, k_sync_object_list[i]);
        if(ctx->last_queued_dma_fence)
            zx_drm_syncobj_replace_fence(drm_syncobj, ctx->last_queued_dma_fence);
        else
            zx_drm_syncobj_replace_fence(drm_syncobj, zx_dma_fence_get_stub());
        zx_drm_syncobj_put_hook(drm_syncobj);
    }

ctx_err:
    if(k_sync_object_list != _statck_sync_obj_list)
    {
        zx_free(k_sync_object_list);
    }

alloc_sync_fail:
    if (k_context_list != _stack_context_list)
    {
        zx_free(k_context_list);
    }

alloc_context_fail:
    return result;

}

int zx_ioctl_gem_map_gtt(struct drm_device *dev, void *data,struct drm_file *filp)
{
    int               ret = -1;
    zx_file_t        *priv = filp->driver_priv;
    zx_drm_gem_map_t *map = (zx_drm_gem_map_t *)data;

#ifdef ZX_HW_NULL
    return 0;
#endif

    ret = zx_gem_mmap_gtt_ioctl(priv->parent_file, map);

    return ret;
}


int zx_ioctl_set_allocation_priority(struct drm_device *dev, void *data,struct drm_file *filp)
{
    int                 ret     = -1;
    zx_file_t           *priv = filp->driver_priv;
    zx_card_t           *zx      = priv->card;
    unsigned int        *k_allocation_list = NULL;
    zx_set_allocation_priority_t *prio = (zx_set_allocation_priority_t *)data;
    krnl_set_allocation_priority_t *kprio;
    char                *payload, *pcurrent;
    int                 payload_size;

    // allocation handle list + allocation priv list + priority list
    payload_size = sizeof(*kprio) + prio->num_allocations * (sizeof(unsigned int) + sizeof(void*) + sizeof(unsigned int));
    payload = pcurrent = zx_malloc(sizeof(*kprio) + payload_size);

    kprio = LOCAL_ALLOC(sizeof(*kprio));
    kprio->device = priv->gpu_device;
    kprio->num_allocations = prio->num_allocations;
    kprio->allocation_list = LOCAL_ALLOC(sizeof(void*) * prio->num_allocations);
    kprio->priority_list = LOCAL_ALLOC(sizeof(unsigned int) * prio->num_allocations);
    zx_copy_from_user(kprio->priority_list, ptr64_to_ptr(prio->priority_list), prio->num_allocations * sizeof(unsigned int));

    k_allocation_list = LOCAL_ALLOC(sizeof(unsigned int) * prio->num_allocations);
    zx_copy_from_user(k_allocation_list, ptr64_to_ptr(prio->allocation_list), prio->num_allocations * sizeof(unsigned int));
    zx_get_gem_priv_list(priv, kprio->allocation_list, k_allocation_list, prio->num_allocations);

    ret = zx_core_interface->set_allocation_priority(zx->adapter, kprio);
    zx_free(payload);

    return ret;
}

int zx_ioctl_make_resident(struct drm_device *dev, void *data,struct drm_file *filp)
{
    int                 i, ret        = -1;
    zx_file_t           *priv          = filp->driver_priv;
    zx_make_resident_t  *make_resident = (zx_make_resident_t *)data;
    unsigned int        *allocation_list = NULL;
    struct drm_file     *file = priv->parent_file;

    allocation_list = zx_malloc(sizeof(unsigned int) * make_resident->num_allocations);
    zx_copy_from_user(allocation_list, ptr64_to_ptr(make_resident->allocation_list),
                        make_resident->num_allocations * sizeof(unsigned int));

    zx_mutex_lock(priv->resident_lock);
    for (i = 0; i < make_resident->num_allocations; i++)
    {
        ret = zx_gem_make_resident_locked(file, allocation_list[i]);
        if (ret != 0)
        {
            zx_error("resident 0x%x failed with %d.\n", allocation_list[i], ret);
        }
    }
    zx_mutex_unlock(priv->resident_lock);
    zx_free(allocation_list);

    return 0;
}

int zx_ioctl_evict(struct drm_device *dev, void *data,struct drm_file *filp)
{
    int                 i, ret  = -1;
    zx_file_t           *priv = filp->driver_priv;
    struct drm_file     *file = priv->parent_file;
    zx_evict_t          *evict   = (zx_evict_t *)data;
    unsigned int        *allocation_list = NULL;

    allocation_list = zx_malloc(sizeof(unsigned int) * evict->num_allocations);
    zx_copy_from_user(allocation_list, ptr64_to_ptr(evict->allocation_list),
                        evict->num_allocations * sizeof(unsigned int));

    zx_mutex_lock(priv->resident_lock);
    for (i = 0; i < evict->num_allocations; i++)
    {
        ret = zx_gem_evict_locked(file, allocation_list[i]);
        if (ret != 0)
        {
            zx_error("evict 0x%x failed with %d.\n", allocation_list[i], ret);
        }
    }
    zx_mutex_unlock(priv->resident_lock);

    zx_free(allocation_list);
    return ret;
}


int zx_ioctl_map_gpu_virtual_address(struct drm_device *dev, void *data,struct drm_file *filp)
{
    int                           ret  = -1;
    zx_file_t                    *priv = filp->driver_priv;
    zx_card_t                    *zx   = priv->card;
    zx_map_gpu_virtual_address_t  *map  = (zx_map_gpu_virtual_address_t *)data;
    krnl_map_gpu_va_t            kmap  = {0};

    kmap.device         = priv->gpu_device;
    kmap.size           = map->size;
    kmap.base_addr      = map->base_addr;
    kmap.min_addr       = map->min_addr;
    kmap.max_addr       = map->max_addr;
    kmap.offset         = map->offset;
    kmap.driver_protection = map->driver_protection;
    kmap.protection_flag= map->protection_flag;
    zx_get_gem_priv_list(priv, &kmap.allocation, &map->allocation, 1);

    ret = zx_core_interface->map_gpu_virtual_address(zx->adapter, &kmap);
    if(ret == 0)
    {
        map->gpu_va = kmap.gpu_va;
    }

    return ret;
}

int zx_ioctl_free_gpu_virtual_address(struct drm_device *dev, void *data,struct drm_file *filp)
{
    zx_file_t                    *priv = filp->driver_priv;
    zx_card_t                    *zx  = priv->card;
    zx_free_gpu_virtual_address_t *free = (zx_free_gpu_virtual_address_t *)data;
    krnl_free_gpu_va_t          kfree = {0, };

    kfree.device = priv->gpu_device;
    kfree.base = free->base;
    kfree.size = free->size;

    return zx_core_interface->free_gpu_virtual_address(zx->adapter, &kfree);
}

int zx_ioctl_reserve_gpu_virtual_address(struct drm_device *dev, void *data,struct drm_file *filp)
{
    int                               ret     = -1;
    zx_file_t                       *priv   = filp->driver_priv;
    zx_card_t                       *zx     = priv->card;
    zx_reserve_gpu_virtual_address_t *reserve = (zx_reserve_gpu_virtual_address_t *)data;
    krnl_reserve_gpu_va_t           kreserve = {0, };

    kreserve.device = priv->gpu_device;
    kreserve.base = reserve->base;
    kreserve.min = reserve->min;
    kreserve.max = reserve->max;
    kreserve.size = reserve->size;
    kreserve.driver_protection = reserve->driver_protection;
    ret = zx_core_interface->reserve_gpu_virtual_address(zx->adapter, &kreserve);
    if (ret == 0)
    {
        reserve->gpu_va = kreserve.gpu_va;
    }

    return ret;
}

int zx_ioctl_update_gpu_virtual_address(struct drm_device *dev, void *data,struct drm_file *filp)
{
    int                               ret     = -1;
    zx_file_t                       *priv   = filp->driver_priv;
    zx_card_t                       *zx     = priv->card;
    zx_update_gpu_virtual_address_t  *update  = (zx_update_gpu_virtual_address_t *)data;
    zx_update_sparse_mapping_info_t *ops = NULL;
    krnl_update_gpu_va_t            *kupdate;
    int                             i;
    struct zx_context *ctx = NULL;

    spin_lock(&priv->ctx_table_lock);
    ctx = (struct zx_context*)idr_find(&priv->ctx_idr, update->context);
    spin_unlock(&priv->ctx_table_lock);

    kupdate = zx_calloc(sizeof(*kupdate) +
            update->num_operation * sizeof(krnl_update_sparse_mapping_info_t) +
            update->num_operation * sizeof(zx_update_sparse_mapping_info_t));

    kupdate->context = ctx->ctx;
    kupdate->fence_obj = update->fence_obj;
    kupdate->num_operation = update->num_operation;
    kupdate->fence_value = update->fence_value;
    kupdate->flags = update->flags;
    kupdate->ops = (void*)(kupdate + 1);
    ops = (void*)(kupdate->ops + update->num_operation);
    if (zx_copy_from_user(ops, ptr64_to_ptr(update->ops),
                update->num_operation * sizeof(zx_update_sparse_mapping_info_t)))
    {
        ret = -1;
        goto done;
    }

    for (i = 0; i < update->num_operation; i++)
    {
        zx_update_sparse_mapping_info_t     *update_sparse = ops + i;
        unsigned int                        ops_type = update_sparse->op_type;

        kupdate->ops[i].op_type = ops_type;
#define COPY_FIELD(field)   kupdate->ops[i].field = update_sparse->field
        switch(ops_type)
        {
        case RESERVED_RESOURCE_MAP:
            COPY_FIELD(map.tile_gpu_va);
            COPY_FIELD(map.tile_size);
            COPY_FIELD(map.allocation_offset);
            COPY_FIELD(map.allocation_size);
            zx_get_gem_priv_list(priv, &kupdate->ops[i].map.allocation, &update_sparse->map.allocation, 1);
            break;
        case RESERVED_RESOURCE_UNMAP:
            COPY_FIELD(unmap.tile_gpu_va);
            COPY_FIELD(unmap.tile_size);
            break;
        case RESERVED_RESOURCE_COPY:
            COPY_FIELD(copy.src_tile_gpu_va);
            COPY_FIELD(copy.dst_tile_gpu_va);
            COPY_FIELD(copy.tile_size);
            break;
        default:
            zx_warning("unsupport.\n");
            break;
        }
    }
    ret = zx_core_interface->update_gpu_virtual_address(zx->adapter, kupdate);
done:
    zx_free(kupdate);
    return ret;
}


int zx_ioctl_cil2_misc(struct drm_device *dev, void *data,struct drm_file *filp)
{
    int               ret = -1;
    zx_file_t         *priv   = filp->driver_priv;
    zx_card_t        *zx = priv->card;
    zx_cil2_misc_t  *misc = (zx_cil2_misc_t *)data;
    krnl_cil2_misc_t kmisc = {0, };
    struct zx_context *ctx = NULL;
    struct drm_file   *file = priv->parent_file;
    struct drm_zx_gem_object *obj  = NULL;
    void *allocation = NULL;

    if (misc->handle)
    {
        obj = zx_drm_gem_object_lookup(file->minor->dev, file, misc->handle);
        if(obj)
            allocation = obj->priv;
    }

    spin_lock(&priv->ctx_table_lock);
    ctx = (struct zx_context*)idr_find(&priv->ctx_idr, misc->context);
    spin_unlock(&priv->ctx_table_lock);

    kmisc.device  = priv->gpu_device;
    kmisc.context = ctx ? ctx->ctx : NULL;
    kmisc.zx_misc = misc;

    ret = zx_core_interface->cil2_misc(zx->adapter, &kmisc, allocation);

    if (obj)
    {
        zx_gem_object_put(obj);
    }
    return ret;
}

int zx_ioctl_test(struct drm_device *dev, void *data,struct drm_file *filp)
{
    int               ret = -1;
    zx_file_t        *priv   = filp->driver_priv;
    zx_card_t        *zx = priv->card;
    zx_test_t       *test = (zx_test_t *)data;
    krnl_test_t     ktest = {0, };

    ktest.type = test->type;
    ktest.device = priv->gpu_device;
    ktest.v_long = test->v_long;
    switch(test->type)
    {
    case ZX_TEST_PAGING_IN:
    case ZX_TEST_PAGING_OUT:
        zx_get_gem_priv_list(priv, &ktest.allocation, &test->v_uint, 1);
        break;
    default:
        break;
    }

    ret = zx_core_interface->test(zx->adapter, &ktest);
    return ret;
}

struct drm_ioctl_desc zx_ioctls[] =
{
#define ZX_IOCTL_DEF_DRV(ioctl, _func, _flags)     \
    [ZX_IOCTL_NR(ioctl) - DRM_COMMAND_BASE] = {    \
        .cmd = ioctl,                              \
        .func = _func,                             \
        .flags = _flags,                           \
        .name = #ioctl                             \
    }
    ZX_IOCTL_DEF_DRV(ZX_IOCTL_BEGIN_PERF_EVENT, zx_ioctl_begin_perf_event, DRM_AUTH|DRM_RENDER_ALLOW),
    ZX_IOCTL_DEF_DRV(ZX_IOCTL_END_PERF_EVENT, zx_ioctl_end_perf_event, DRM_AUTH|DRM_RENDER_ALLOW),
    ZX_IOCTL_DEF_DRV(ZX_IOCTL_GET_PERF_EVENT, zx_ioctl_get_perf_event, DRM_AUTH|DRM_RENDER_ALLOW),
    ZX_IOCTL_DEF_DRV(ZX_IOCTL_SEND_PERF_EVENT, zx_ioctl_send_perf_event, DRM_AUTH|DRM_RENDER_ALLOW),
    ZX_IOCTL_DEF_DRV(ZX_IOCTL_GET_PERF_STATUS, zx_ioctl_get_perf_status, DRM_AUTH|DRM_RENDER_ALLOW),
    ZX_IOCTL_DEF_DRV(ZX_IOCTL_BEGIN_MIU_DUMP_PERF_EVENT, zx_ioctl_begin_miu_dump_perf_event, DRM_AUTH|DRM_RENDER_ALLOW),
    ZX_IOCTL_DEF_DRV(ZX_IOCTL_END_MIU_DUMP_PERF_EVENT, zx_ioctl_end_miu_dump_perf_event, DRM_AUTH|DRM_RENDER_ALLOW),
    ZX_IOCTL_DEF_DRV(ZX_IOCTL_SET_MIU_REG_LIST_PERF_EVENT, zx_ioctl_set_miu_reg_list_perf_event, DRM_AUTH|DRM_RENDER_ALLOW),
    ZX_IOCTL_DEF_DRV(ZX_IOCTL_GET_MIU_DUMP_PERF_EVENT, zx_ioctl_get_miu_dump_perf_event, DRM_AUTH|DRM_RENDER_ALLOW),
    ZX_IOCTL_DEF_DRV(ZX_IOCTL_DIRECT_GET_MIU_DUMP_PERF_EVENT, zx_ioctl_direct_get_miu_dump_perf_event, DRM_AUTH|DRM_RENDER_ALLOW),
    ZX_IOCTL_DEF_DRV(ZX_IOCTL_WAIT_CHIP_IDLE, zx_ioctl_wait_chip_idle, DRM_AUTH|DRM_RENDER_ALLOW),
    ZX_IOCTL_DEF_DRV(ZX_IOCTL_QUERY_INFO, zx_ioctl_query_info, DRM_AUTH|DRM_RENDER_ALLOW),
    ZX_IOCTL_DEF_DRV(ZX_IOCTL_CREATE_DEVICE, zx_ioctl_create_device, DRM_AUTH|DRM_RENDER_ALLOW),
    ZX_IOCTL_DEF_DRV(ZX_IOCTL_RENDER, zx_ioctl_render, DRM_AUTH|DRM_RENDER_ALLOW),
    ZX_IOCTL_DEF_DRV(ZX_IOCTL_SIGNAL_SYNC_OBJECT, zx_ioctl_signal_sync_object, DRM_AUTH|DRM_RENDER_ALLOW),
    ZX_IOCTL_DEF_DRV(ZX_IOCTL_SET_ALLOCATION_PRIORITY, zx_ioctl_set_allocation_priority, DRM_AUTH|DRM_RENDER_ALLOW),
    ZX_IOCTL_DEF_DRV(ZX_IOCTL_MAKE_RESIDENT, zx_ioctl_make_resident, DRM_AUTH|DRM_RENDER_ALLOW),
    ZX_IOCTL_DEF_DRV(ZX_IOCTL_EVICT, zx_ioctl_evict, DRM_AUTH|DRM_RENDER_ALLOW),
    ZX_IOCTL_DEF_DRV(ZX_IOCTL_MAP_GPU_VIRTUAL_ADDRESS, zx_ioctl_map_gpu_virtual_address, DRM_AUTH|DRM_RENDER_ALLOW),
    ZX_IOCTL_DEF_DRV(ZX_IOCTL_FREE_GPU_VIRTUAL_ADDRESS, zx_ioctl_free_gpu_virtual_address, DRM_AUTH|DRM_RENDER_ALLOW),
    ZX_IOCTL_DEF_DRV(ZX_IOCTL_RESERVE_GPU_VIRTUAL_ADDRESS, zx_ioctl_reserve_gpu_virtual_address, DRM_AUTH|DRM_RENDER_ALLOW),
    ZX_IOCTL_DEF_DRV(ZX_IOCTL_UPDATE_GPU_VIRTUAL_ADDRESS, zx_ioctl_update_gpu_virtual_address, DRM_AUTH|DRM_RENDER_ALLOW),
    ZX_IOCTL_DEF_DRV(ZX_IOCTL_ADD_HW_CTX_BUF, zx_ioctl_add_hw_ctx_buf, DRM_AUTH|DRM_RENDER_ALLOW),
    ZX_IOCTL_DEF_DRV(ZX_IOCTL_RM_HW_CTX_BUF, zx_ioctl_rm_hw_ctx_buf, DRM_AUTH|DRM_RENDER_ALLOW),
    ZX_IOCTL_DEF_DRV(ZX_IOCTL_DRM_GEM_CREATE_ALLOCATION, zx_ioctl_gem_create_allocation, DRM_AUTH|DRM_RENDER_ALLOW),
    ZX_IOCTL_DEF_DRV(ZX_IOCTL_CREATE_CONTEXT, zx_ioctl_create_context, DRM_AUTH|DRM_RENDER_ALLOW),
    ZX_IOCTL_DEF_DRV(ZX_IOCTL_DESTROY_CONTEXT, zx_ioctl_destroy_context, DRM_AUTH|DRM_RENDER_ALLOW),
    ZX_IOCTL_DEF_DRV(ZX_IOCTL_DRM_GEM_MAP_GTT, zx_ioctl_gem_map_gtt, DRM_AUTH|DRM_RENDER_ALLOW),
    ZX_IOCTL_DEF_DRV(ZX_IOCTL_DRM_BEGIN_CPU_ACCESS, zx_ioctl_gem_begin_cpu_access, DRM_AUTH|DRM_RENDER_ALLOW),
    ZX_IOCTL_DEF_DRV(ZX_IOCTL_DRM_END_CPU_ACCESS, zx_ioctl_gem_end_cpu_access, DRM_AUTH|DRM_RENDER_ALLOW),
    ZX_IOCTL_DEF_DRV(ZX_IOCTL_KMS_GET_PIPE_FROM_CRTC, zx_ioctl_kms_get_pipe_from_crtc, DRM_AUTH|DRM_RENDER_ALLOW),
    ZX_IOCTL_DEF_DRV(ZX_IOCTL_WAIT_ALLOCATION_IDLE, zx_ioctl_wait_allocation_idle, DRM_AUTH|DRM_RENDER_ALLOW),
    ZX_IOCTL_DEF_DRV(ZX_IOCTL_SET_GEM_DBG_INFO, zx_ioctl_set_gem_dbg_info, DRM_AUTH|DRM_RENDER_ALLOW),
    ZX_IOCTL_DEF_DRV(ZX_IOCTL_GEM_OPEN, zx_ioctl_gem_open, DRM_AUTH|DRM_RENDER_ALLOW),
    ZX_IOCTL_DEF_DRV(ZX_IOCTL_CIL2_MISC, zx_ioctl_cil2_misc, DRM_AUTH|DRM_RENDER_ALLOW),
    ZX_IOCTL_DEF_DRV(ZX_IOCTL_TEST, zx_ioctl_test, DRM_AUTH|DRM_RENDER_ALLOW),
    ZX_IOCTL_DEF_DRV(ZX_DRM_IOCTL_SYNCOBJ_CREATE, zx_drm_syncobj_create_ioctl, DRM_AUTH|DRM_RENDER_ALLOW),
    ZX_IOCTL_DEF_DRV(ZX_DRM_IOCTL_SYNCOBJ_DESTROY, zx_drm_syncobj_destroy_ioctl, DRM_AUTH|DRM_RENDER_ALLOW),
    ZX_IOCTL_DEF_DRV(ZX_DRM_IOCTL_SYNCOBJ_WAIT, zx_drm_syncobj_wait_ioctl, DRM_AUTH|DRM_RENDER_ALLOW),
    ZX_IOCTL_DEF_DRV(ZX_DRM_IOCTL_SYNCOBJ_RESET, zx_drm_syncobj_reset_ioctl, DRM_AUTH|DRM_RENDER_ALLOW),
    ZX_IOCTL_DEF_DRV(ZX_DRM_IOCTL_SYNCOBJ_GET_STATUS, zx_drm_syncobj_get_status_ioctl, DRM_AUTH|DRM_RENDER_ALLOW),
};

