/*
 * Copyright (c) 2018 Shanghai Zhaoxin Semiconductor Co., Ltd.
 * All Rights Reserved.
 * This is UNPUBLISHED PROPRIETARY SOURCE CODE of Shanghai Zhaoxin Semiconductor Co., Ltd.;
 * the contents of this file may not be disclosed to third parties, copied or
 * duplicated in any form, in whole or in part, without the prior written
 * permission of Shanghai Zhaoxin Semiconductor Co., Ltd.
 *
 * The copyright of the source code is protected by the copyright laws of the People's
 * Republic of China and the related laws promulgated by the People's Republic of China
 * and the international covenant(s) ratified by the People's Republic of China.
 *
 */

#include "zx_adapter.h"
#include "global.h"
#include "context.h"
#include "vidsch.h"
#include "vidmm.h"
#include "vidmmi.h"

/**
 * @brief      create gpu device
 *
 * @param      adapter  the lead physical adapter in the linked chain
 * @param      filp     linux file pointer
 *
 * @return     NULL if param is invalid,
 *             gpu device struct if create successfully
 */
gpu_device_t* cm_create_device(adapter_t *adapter, void *filp)
{
    gpu_device_t *device;
    int i;

    device = zx_calloc(sizeof(gpu_device_t));

    if(device == NULL)
    {
        zx_error("create gpu device: alloc memory failed!\n");
        return NULL;
    }

    device->adapter = adapter;
    device->filp    = filp;
    device->pid     = zx_get_current_pid();
    device->tid     = zx_get_current_tid();
    device->priority= 0;
    device->lock    = zx_create_mutex();
    device->uuid    = zx_atomic64_inc(adapter->device_uuid);

    device->pid = 0;
    device->tid = 0;
    zx_strcpy(device->pname, "Unknown");

    list_init_head(&device->context_list);
    list_init_head(&device->hwctx_list);
    list_init_head(&device->bl_slot_list);
    list_init_head(&device->sync_obj_list);

    cm_device_resource_ref_num_inc(device);

    //The global reserved device with the filp NULL.
    if(filp != NULL)
    {
        if(adapter->chip_id == CHIP_CNE001) {
            device->vm = adapter->private_vma;
        }
        else
        {
            device->vm        = zx_calloc(sizeof(zx_vma_space_t));

            if(device->vm == NULL)
            {
                zx_error("create gpu device: alloc vm memory failed!\n");
                return NULL;
            }

            //todo: need post done the vma space when initialize the global device?
            vidmm_vm_init_vma_space(device->vm, adapter);
        }
    }
    else
    {
        //for the global reserved device, use the global reserved vma.
        zx_vma_space_t *reserved_vma = adapter->reserved_vma;
        device->vm           = reserved_vma;
    }

    /* add device to adapter device list */
    zx_mutex_lock(adapter->device_list_lock);
    list_add_tail(&device->list_node, &adapter->device_list);
    zx_mutex_unlock(adapter->device_list_lock);

    return device;
}

/**
 * @brief      destroy gpu device
 *
 * @param      adapter  the lead physical adapter in the linked chain
 * @param      device   The gpu device that needs to be destroyed
 */
void cm_destroy_device(adapter_t *adapter, gpu_device_t *device)
{
    gpu_context_t   *context = NULL, *context_next;
    hw_ctxbuf_t     *hwctx = NULL,   *hwctx_next;
    bl_slot_t *bl_slot, *bl_slot_next;

    int i;

    /* put destory_context first, since destroy context default need wait chip idle
     * so no need call wait chip idle here.
     */
    /* destroy exist context */
    list_for_each_entry_safe(context, context_next, &device->context_list, list_node)
    {
        cm_destroy_context(device, context);
    }

    /* destroy exist hwctx */
    list_for_each_entry_safe(hwctx, hwctx_next, &device->hwctx_list, list_node)
    {
        cm_remove_hwctx_buffer(device, NULL, hwctx->index);
    }

    /* destroy exsit cil3 allocate bl slot buffer */
    list_for_each_entry_safe(bl_slot, bl_slot_next, &device->bl_slot_list, list_node)
    {
        zx_mutex_lock(adapter->mm_mgr->mgr_lock);
        bl_slot->bl_node->release(bl_slot->bl_node);
        zx_mutex_unlock(adapter->mm_mgr->mgr_lock);
        list_del(&bl_slot->list_node);
        zx_free(bl_slot);
    }

    /* fini vma space */
    if(adapter->chip_id != CHIP_CNE001 && device->vm && (device->vm != adapter->reserved_vma))
    {
        vidmm_vm_fini_vma_space(device->vm);
        zx_free(device->vm);
    }

    /* remove device from adapter device list */
    zx_mutex_lock(adapter->device_list_lock);

    list_del(&device->list_node);

    zx_mutex_unlock(adapter->device_list_lock);

    cm_device_resource_ref_num_dec(device);

}

/**
 * @brief      increate device resource reference number
 *
 * @param      device  The device
 */
void cm_device_resource_ref_num_inc(gpu_device_t *device)
{
    cm_device_lock(device);

    device->resource_ref_num++;

    cm_device_unlock(device);
}

/**
 * @brief      decrease device resource reference number
 *
 * @param      device  The device
 */
void cm_device_resource_ref_num_dec(gpu_device_t *device)
{
    int free_device = FALSE;

    cm_device_lock(device);

    device->resource_ref_num--;

    if(device->resource_ref_num == 0)
    {
        free_device = TRUE;
    }

    cm_device_unlock(device);

    if(free_device)
    {
        vidsch_power_mask_clear(device);
        zx_destroy_mutex(device->lock);
        zx_free(device);
    }
}

/**
 * @brief      destroy all the devices which bind to the adapter
 *
 * @param      adapter  The adapter
 */
void cm_destroy_remained_device(adapter_t *adapter)
{
    gpu_device_t    *device      = NULL;
    gpu_device_t    *device_next = NULL;

    //destroy exist device when deinit adapter
    list_for_each_entry_safe(device, device_next, &adapter->device_list, list_node)
    {
        cm_destroy_device(adapter, device);
    }
}

/**
 * @brief      get the ringbuffer index for the specific engine index
 *
 * @param      adapter       The adapter
 * @param[in]  node_ordinal  The engine index
 *
 * @return     the ring buffer index
 */
static int cm_get_hw_ringbuffer_index(adapter_t *adapter, unsigned int node_ordinal)
{
    int  i;
    for(i = 0; i < adapter->active_engine_count; i++)
    {
        if(adapter->engine_index_table[i].node_ordinal == node_ordinal)
        {
            break;
        }
    }

    zx_assert(i < adapter->active_engine_count, "i:%d, active_engine_count:%d", i, adapter->active_engine_count);
    return i;
}

/**
 * @brief      transform umd priority to context priority
 *
 * @param[in]  priority  The priority
 *
 * @return     CONTEXT_PRIORITY
 */
static unsigned int cm_get_context_priority(unsigned int priority)
{
    switch(priority)
    {
        case 0:
            return ZX_CONTEXT_PRIO_LOW;
        case 1:
            return ZX_CONTEXT_PRIO_MEDIA;
        case 2:
            return ZX_CONTEXT_PRIO_HIGH;
        case 3:
            return ZX_CONTEXT_PRIO_MAX;
        default:
            return ZX_CONTEXT_PRIO_LOW;
    }
}

/**
 * @brief      transform umd type to context type
 *
 * @param[in]  type  The type
 *
 * @return     CM_CONTEXT_TYPE
 */
static unsigned int cm_get_context_type(unsigned int type)
{
    switch(type)
    {
        case 0:
            return CONTEXT_NORMAL_RENDER;
        case 1:
            return CONTEXT_PAGING_QUEUE;
        case 2:
            return CONTEXT_COMPANION;
        default:
            return CONTEXT_NORMAL_RENDER;
    }
}

static unsigned int cm_get_client_type(unsigned int client)
{
    switch(client)
    {
        case 0:
            return CLIENT_TYPE_UNKNOWN;
        case 1:
            return CLIENT_TYPE_OPENGL;
        case 2:
            return CLIENT_TYPE_OPENGLES;
        case 3:
            return CLIENT_TYPE_VULKAN;
        case 4:
            return CLIENT_TYPE_OPENCL;
        default:
            return CLIENT_TYPE_UNKNOWN;
    }
}

/**
 * @brief      create gpu context
 *
 * @param      device          The device which the context bind to
 * @param      create_context  The create context
 *
 * @return     NULL if param is invalid
 */
gpu_context_t* cm_create_context(gpu_device_t *device, create_context_t *create_context)
{
    adapter_t           *adapter    = device->adapter;
    gpu_context_t       *context    = zx_calloc(sizeof(gpu_context_t));

    if(context == NULL)
    {
        zx_error("create gpu context fail: out of memory!\n");
        return NULL;
    }

    context->device              = device;
    context->rb_index            = cm_get_hw_ringbuffer_index(adapter, create_context->node_ordinal);
    context->priority            = cm_get_context_priority(create_context->priority);
    context->tid                 = create_context->tid;

    context->flag.type           = cm_get_context_type(create_context->type);
    context->flag.client         = cm_get_client_type(create_context->client_type);
    context->flag.null_render    = create_context->null_render;

    context->ref_cnt             = zx_create_atomic(0);

    context->uuid                = zx_atomic64_inc(adapter->context_uuid);
    if(create_context->power_dvfs_indicador)
    {
        vidsch_power_mask_acquire(adapter, device);
    }

    //TODO: after sync
    //context->timeline = zx_create_sync_timeline();

    vidsch_init_task_queue(context);

    cm_device_lock(device);
    list_add(&context->list_node, &device->context_list);
    cm_device_unlock(device);

    //cm_reset_command_header(context);
    return context;
}

#ifdef GFX_ONLY_FPGA
#define CM_DESTROY_TIMEOUT 20000
#else
#define CM_DESTROY_TIMEOUT 9000
#endif

/**
 * @brief      wait context idle, if wait timeout, drop tasks
 *
 * @param      context  The context
 */
void cm_wait_context_idle(gpu_context_t *context)
{
    adapter_t * adapter = context->device->adapter;
    int times = 0, idle = FALSE;

#ifdef ZX_HW_NULL
    return;
#endif

    while(times++ < CM_DESTROY_TIMEOUT)
    {
        if(zx_atomic_read(context->ref_cnt) != 0)
        {
            zx_msleep(1);
        }
        //TODO: after vidsch, paging queue need to wait all of the vm operation be finished
        //else if(context->paging_fence != NULL && ){}
        else
        {
            idle = TRUE;
            break;
        }
    }

    /* if not idle, seems something wrong, maybe sync wait a never returned value, set task dropped skip wait */
    if(!idle)
    {
        if (adapter->ctl_flags.hang_dump == 2)
        {
            while(TRUE)
            {
                zx_msleep(1000);
            }
        }
        context->device->task_dropped = TRUE;

        for(times = 0; times < 1000; times++)
        {
            /* notify a INT_FENCE to let vidsch reschedule the task. since we
             * set task_dropped here then, schedule will dropped the task not submitted
             */
            //TODO: after vidsch
            //TODO: for LDA, is it enough to notify one adapter?
            //vidsch_notify_interrupt(adapter, INT_FENCE);

            zx_msleep(1);

            if(zx_atomic_read(context->ref_cnt) == 0)
            {
                idle = TRUE;
                break;
            }
        }

        if(!idle)
        {
            //TODO:after vidsch dispmgr
            //vidsch_dump(adapter);
            //dispmgr_dump(adapter);
        }
        zx_assert(idle,  "");
    }
}

void cm_destroy_context(gpu_device_t *device, gpu_context_t *context)
{
    adapter_t    *adapter = device->adapter;
    unsigned int  idx     = 0;

    cm_wait_context_idle(context);

    //TODO:after sync
    //zx_destroy_sync_timeline(context->timeline);

    vidsch_deinit_task_queue(context);

    cm_device_lock(device);

    list_del(&context->list_node);

    cm_device_unlock(device);

    context->device = NULL;

    zx_destroy_atomic(context->ref_cnt);

    zx_free(context);
}

static unsigned int cmi_acquire_physical_context_index(adapter_t  *adapter)
{
    unsigned char* context_mask = adapter->context_buffer_inuse;
    unsigned int   mask_num     = 0xFFFF/8;
    unsigned int   i,j;
    unsigned int   physical_context_index = 0xFFFF;//no phys context index, no using deferred context

    for( i=0; i<mask_num; i++ )
    {
        if( context_mask[i] == 0xFF ) continue;

        for( j=0; j<8; j++ )
        {
            if( context_mask[i] & (1<<j) ) continue;
            physical_context_index = i*8+j;
            context_mask[i] |= (1<<j);
            break;
        }

        if( physical_context_index ) break;
    }

    return physical_context_index;
}

inline static void cmi_release_physical_context_index(adapter_t *adapter, unsigned int physical_context_index)
{
    if( physical_context_index && physical_context_index < 0xFFFF )
    {
        unsigned int i = physical_context_index/8;
        unsigned int j = physical_context_index%8;
        adapter->context_buffer_inuse[i] &= ~(1<<j);
    }
}

int cm_add_hwctx_buffer(gpu_device_t *device, gpu_context_t *context, unsigned int hw_buf_index)
{
    adapter_t   *adapter       = device->adapter;
    hw_ctxbuf_t *hw_ctx_buffer = zx_calloc(sizeof(hw_ctxbuf_t));

    if(hw_ctx_buffer == NULL)
    {
        return E_OUTOFMEMORY;
    }

    hw_ctx_buffer->index      = hw_buf_index;
    hw_ctx_buffer->phys_index = cmi_acquire_physical_context_index(adapter);

    cm_device_lock(device);

    list_add(&hw_ctx_buffer->list_node, &device->hwctx_list);

    cm_device_unlock(device);

    return S_OK;
}

int cm_remove_hwctx_buffer(gpu_device_t *device, gpu_context_t *context, unsigned int hwctx_index)
{
    hw_ctxbuf_t *hw_ctx  = NULL;
    int          status  = E_FAIL;

    cm_device_lock(device);

    list_for_each_entry(hw_ctx, &device->hwctx_list, list_node)
    {
        if(hw_ctx->index == hwctx_index)
        {
            list_del(&hw_ctx->list_node);

            status = S_OK;

            break;
        }
    }

    cm_device_unlock(device);

    if(status == S_OK)
    {
        cmi_release_physical_context_index(device->adapter, hw_ctx->phys_index);

        zx_free(hw_ctx);
    }

    return status;
}

hw_ctxbuf_t *cm_get_hwctx_buffer(gpu_device_t *device, gpu_context_t *context, unsigned int hwctx_index)
{
    hw_ctxbuf_t *hw_ctx      = NULL;
    hw_ctxbuf_t *match_hwctx = NULL;

    if(hwctx_index == 0)
    {
        return &context->hw_ctx;
    }

    cm_device_lock(device);

    list_for_each_entry(hw_ctx, &device->hwctx_list, list_node)
    {
        if(hw_ctx->index == hwctx_index)
        {
            match_hwctx = hw_ctx;
            break;
        }
    }

    cm_device_unlock(device);

    return match_hwctx;
}

int cm_save(adapter_t *adapter, int need_save_memory)
{
    vidmm_mgr_t                   *mm_mgr = adapter->mm_mgr;
    vidmm_segment_t               *segment = NULL;
    unsigned int                  idx, priority;
    int i = 0;
    int result = S_OK;

    /* not add lock to protect device list here, since save only used in suspend,
     * no multi-thread issue here
     */

    for(i = 0; i < adapter->active_engine_count; i++)
    {
        vidsch_wait_engine_idle(adapter, i);
    }

    for (idx = 0; idx < mm_mgr->segment_cnt; idx++)
    {
        vidmm_allocation_t *allocation;
        segment = &mm_mgr->segment[idx];

        list_for_each_entry(allocation, &segment->unpagable_resident_list, list_item)
        {
            allocation->flag.need_save_when_acpi = need_save_memory;
            result = vidmm_save_allocation(adapter, allocation);
            if (result)
            {
                 return result;
            }
        }

        for (priority = PDISCARD; priority < PALL; priority++)
        {
            list_for_each_entry(allocation, &segment->pagable_resident_list[priority], list_item)
            {
                allocation->flag.need_save_when_acpi = need_save_memory;
                result = vidmm_save_allocation(adapter, allocation);
                if (result)
                {
                     return result;
                }
            }
        }
    }
    return result;
}

int cm_restore(adapter_t *adapter)
{
    vidmm_mgr_t                   *mm_mgr = adapter->mm_mgr;
    vidmm_segment_t               *segment = NULL;
    unsigned int                  idx, priority;
    int result = S_OK;

    /* not add lock to protect device list here, since save only used in resume,
     * no multi-thread issue here
     */
    for (idx = 0; idx < mm_mgr->segment_cnt; idx++)
    {
        vidmm_allocation_t *allocation;
        segment = &mm_mgr->segment[idx];

        list_for_each_entry(allocation, &segment->unpagable_resident_list, list_item)
        {
            result = vidmm_restore_allocation(adapter, allocation);
            if (result)
            {
                 return result;
            }
        }

        for (priority = PDISCARD; priority < PALL; priority++)
        {
            list_for_each_entry(allocation, &segment->pagable_resident_list[priority], list_item)
            {
                result = vidmm_restore_allocation(adapter, allocation);
                if (result)
                {
                     return result;
                }
            }
        }
    }
    return result;
}

void cm_dump_resource(adapter_t *adapter)
{
    gpu_device_t  *device, *device_next;
    gpu_context_t *context, *context_next;

    zx_info("-----------current device info & context info.------------------\n");
    list_for_each_entry_safe(device, device_next, &adapter->device_list, list_node)
    {
        zx_info("^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n");
        zx_info("**device:  %p, pid: %ld, create tid: %ld, create allocation num: %4d.\n",
            device, device->pid, device->tid, device->resource_ref_num - 1);

        list_for_each_entry_safe(context, context_next, &device->context_list, list_node)
        {
            zx_info("context: %p, on engine: %d, tid: %ld, device: %p", context, context->rb_index, context->tid, device);
        }
        zx_info("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n");
    }
}
