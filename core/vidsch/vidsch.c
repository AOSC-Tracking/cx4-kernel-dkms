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
#include "vidsch.h"
#include "vidschi.h"
#include "vidsch_workerthread.h"
#include "vidmm.h"
#include "vidsch_trace.h"
#include "perfevent.h"

extern vidschedule_chip_func_t vidschedule_chip_func;

static void vidsch_init_page_table_set_task_queue(struct gpu_context *context)
{
    adapter_t    *adapter  = context->device->adapter;
    vidsch_mgr_t *sch_mgr  = adapter->sch_mgr[context->rb_index];
    vidsch_task_queue_t *q = NULL;

    if (!sch_mgr || !sch_mgr->page_table_set_pool)
        return;

    list_for_each_entry(q, &sch_mgr->page_table_set_pool->queue_list, link)
    {
        if (q->priority == context->priority)
        {
            context->page_table_set_task_queue = vidsch_task_queue_get(q);
            break;
        }
    }
}

void vidsch_init_task_queue(struct gpu_context *context)
{
    adapter_t    *adapter  = context->device->adapter;
    vidsch_mgr_t *sch_mgr  = adapter->sch_mgr[context->rb_index];
    vidsch_task_queue_t *q = NULL;

    if (!sch_mgr)
        return;

    if (sch_mgr->engine_schedule)
    {
        list_for_each_entry(q, &sch_mgr->task_queue_list->queue_list, link)
        {
            if (q->priority == context->priority)
            {
                context->task_queue = vidsch_task_queue_get(q);
                break;
            }
        }
    }
    else
    {
        vidsch_task_queue_t *tq = vidsch_task_queue_alloc(32, context->priority);
        vidsch_task_queue_pool_add(sch_mgr->task_queue_list, tq);
        context->task_queue = tq;
    }
}

void vidsch_deinit_task_queue(struct gpu_context *context)
{
    adapter_t    *adapter  = context->device->adapter;
    vidsch_mgr_t *sch_mgr  = adapter->sch_mgr[context->rb_index];

    if (context->task_queue)
    {
        if (!sch_mgr->engine_schedule)
        {
            if (context->task_queue->pool)
            {
                vidsch_task_queue_pool_remove(context->task_queue->pool, context->task_queue);
            }
        }
        vidsch_task_queue_release(context->task_queue);
        context->task_queue = NULL;
    }

    if (context->page_table_set_task_queue)
    {
        vidsch_task_queue_release(context->page_table_set_task_queue);
        context->page_table_set_task_queue = NULL;
    }
}

static vidsch_fence_buffer_t *vidschi_create_fence_buffer(adapter_t *adapter, unsigned int segment_id, int buffer_size)
{
    gpu_device_t             *device          = adapter->reserved_device;
    vidsch_fence_buffer_t    *fence_buf       = zx_calloc(sizeof(vidsch_fence_buffer_t));
    vidmm_segment_memory_t   *reserved_memory = NULL;
    unsigned short           total_num        = 0;
    int                      ret              = S_OK;

    reserved_memory = vidmm_create_segment_memory(adapter, segment_id, buffer_size, 1, 1);
    if(reserved_memory == NULL) return NULL;

    ret = vidmm_map_segment_memory(adapter, reserved_memory);
    if(ret != S_OK)
    {
        if(reserved_memory->gpu_vm != NULL)
        {
            vidmm_unmap_segment_memory(reserved_memory);
        }

        vidmm_destroy_segment_memory(adapter, reserved_memory);
        zx_assert(0, "map fence segment memory failed, with err:0x%x", ret);
    }

    vidmm_lock_segment_memory(adapter, reserved_memory);

    /* per fence use a 64-bit slot */
    total_num = reserved_memory->size / sizeof(unsigned long long);

    zx_debug("create fence buffer total num %d \n", total_num);
    zx_debug("fence buffer segment_id:%d, gpu_virt_addr:%llx, cpu_va: %p\n", segment_id, reserved_memory->gpu_va, reserved_memory->krnl_cpu_vma->virt_addr);

    fence_buf->fence_reserved_memory = reserved_memory;
    fence_buf->bitmap_lock           = zx_create_spinlock();
    fence_buf->bitmap_size           = total_num >> 3;
    fence_buf->bitmap                = zx_calloc(fence_buf->bitmap_size);
    fence_buf->total_num             =
    fence_buf->left_num              = total_num;
    fence_buf->free_start            = 0;

    return fence_buf;
}

static void vidschi_destroy_fence_buffer(adapter_t *adapter, vidsch_fence_buffer_t *fence_buf)
{
    vidmm_segment_memory_t *reserved_memory = fence_buf->fence_reserved_memory;

    vidmm_destroy_segment_memory(adapter, reserved_memory);

    zx_destroy_spinlock(fence_buf->bitmap_lock);

    zx_free(fence_buf->bitmap);

    zx_free(fence_buf);
}

int vidsch_request_fence_space(adapter_t *adapter, unsigned long long **virt_addr, unsigned long long **virt_addr_user, void *filp, unsigned long long *gpu_addr)
{
    vidsch_fence_buffer_t   *fence_buf       = adapter->fence_buf;
    vidmm_segment_memory_t  *reserved_memory = fence_buf->fence_reserved_memory;
    int                      num             = 0;
    int                      status          = E_FAIL;

    if(fence_buf->left_num == 0)
    {
        zx_error("fence_buf no enough space.\n");
    }

    zx_spin_lock(fence_buf->bitmap_lock);

    num = zx_find_next_zero_bit(fence_buf->bitmap, fence_buf->total_num, fence_buf->free_start);

    if(num < fence_buf->total_num)
    {
        zx_set_bit(num, fence_buf->bitmap);

        fence_buf->left_num--;
        fence_buf->free_start = (num + 1) % fence_buf->total_num;

        *gpu_addr   = reserved_memory->gpu_va + (num << 3);
        *virt_addr  = (unsigned long long*)reserved_memory->krnl_cpu_vma->virt_addr + num;

        if((virt_addr_user != NULL) && (filp != NULL))
        {
            /* NOT support map umd va */
            *virt_addr_user = 0;
        }

        **virt_addr = 0ll;

        status = S_OK;
    }

    zx_spin_unlock(fence_buf->bitmap_lock);

    return status;
}

int vidsch_release_fence_space(adapter_t *adapter, unsigned long long gpu_addr)
{
    vidsch_fence_buffer_t   *fence_buf       = adapter->fence_buf;
    vidmm_segment_memory_t  *reserved_memory = fence_buf->fence_reserved_memory;

    int num = (gpu_addr - reserved_memory->gpu_va) >> 3;

    zx_spin_lock(fence_buf->bitmap_lock);

    fence_buf->left_num++;

    zx_clear_bit(num, fence_buf->bitmap);

    zx_spin_unlock(fence_buf->bitmap_lock);

    return S_OK;

}

static void vidschi_init_vidsch_global(adapter_t *adapter)
{
    vidsch_global_t  *sch_global = NULL;
    vidsch_query_private_t  query_data = {0};

    sch_global = zx_calloc(sizeof(vidsch_global_t));

    if(sch_global == NULL)
    {
        zx_error("calloc vidsch_global_t fail\n");
        return;
    }

    sch_global->adapter = adapter;
    adapter->sch_global = sch_global;
    sch_global->dvfs_indicator = zx_calloc(MAX_POWER_MASK_NUM >> 3);
    sch_global->chip_func = &vidschedule_chip_func;

    sch_global->rw_lock = zx_create_rwsema();

    sch_global->power_status_lock = zx_create_spinlock();

    vidschi_init_vm_id_mgr(adapter);
}

static void vidschi_fini_vidsch_global(adapter_t *adapter)
{
    vidsch_global_t  *sch_global = adapter->sch_global;

    vidschi_fini_vm_id_mgr(adapter);

    vidschi_deinit_daemon_thread(adapter);

    zx_destroy_rwsema(sch_global->rw_lock);
    zx_destroy_spinlock(sch_global->power_status_lock);
    zx_free(sch_global->dvfs_indicator);
    zx_free(sch_global);
}


static void vidschi_init_vidsch_mgr(adapter_t *adapter)
{
    vidmm_segment_memory_t *reserved_memory = NULL;
    vidsch_query_private_t  query_data      = {0};

    int ret = S_OK;
    int i;

    zx_memset(&query_data, 0, sizeof(vidsch_query_private_t));

    vidsch_query_chip(adapter, &query_data);

    adapter->active_engine_count = query_data.engine_count;

    adapter->fence_buf = vidschi_create_fence_buffer(adapter, query_data.fence_buffer_segment_id, 16*1024);
    if(adapter->fence_buf == NULL)
    {
        zx_error("create adapter fence buffer failed.\n");
        return;
    }

    for (i = 0; i < query_data.engine_count; i++)
    {
        vidsch_mgr_t *sch_mgr = NULL;

        if(query_data.engine_ctrl[i] & ENGINE_CTRL_DISABLE)
        {
            continue;
        }

        if(query_data.engine_caps[i] & ENGINE_CAPS_PAGING)
        {
            adapter->paging_engine_index = i;
        }

        sch_mgr = zx_calloc(sizeof(vidsch_mgr_t));

        zx_assert(sch_mgr != NULL, "");

        adapter->sch_mgr[i] = sch_mgr;

        sch_mgr->adapter         = adapter;
        sch_mgr->engine_caps     = query_data.engine_caps[i];
        sch_mgr->engine_ctrl     = query_data.engine_ctrl[i];
        sch_mgr->engine_index    = i;
        sch_mgr->chip_func       = query_data.engine_func[i];
        sch_mgr->init_submit     = 1;
        sch_mgr->hw_queue_size   = query_data.engine_hw_queue_size[i];
        sch_mgr->engine_lock     = zx_create_mutex();
        sch_mgr->handled_tasks_num = zx_create_atomic64(0);
        sch_mgr->pending_tasks_num = zx_create_atomic64(0);

        sch_mgr->last_send_fence_id = 0;
        sch_mgr->returned_fence_id  = 0;
        sch_mgr->fence_event        = zx_create_event(0);
        sch_mgr->fence_lock         = zx_create_spinlock();
        sch_mgr->power_status_lock  = zx_create_spinlock();

        sch_mgr->task_list_lock          = zx_create_mutex();
        sch_mgr->dma_lock          = zx_create_mutex();
        list_init_head(&sch_mgr->submitted_task_list);

        sch_mgr->engine_schedule = TRUE;
        sch_mgr->monitor_count   = 0;

        if (IS_VIDEO_ENGINE(adapter->chip_id, i))
        {
            sch_mgr->prepare_event = zx_create_event(0);
        }

        zx_debug("engine_schedule[%d]: %d\n", i, sch_mgr->engine_schedule);
        sch_mgr->task_queue_list = vidsch_task_queue_pool_alloc(ZX_CONTEXT_PRIO_MAX);

        if (i == 0)
        {
            vidsch_task_queue_t *page_table_set_list = vidsch_task_queue_alloc(32, ZX_CONTEXT_PRIO_LOW);
            sch_mgr->page_table_set_pool = vidsch_task_queue_pool_alloc(ZX_CONTEXT_PRIO_LOW);

            vidsch_task_queue_pool_add(sch_mgr->page_table_set_pool, page_table_set_list);
            vidsch_task_queue_release(page_table_set_list);
        }

        if(sch_mgr->engine_schedule)
        {
            int k;

            for (k = 0; k <= ZX_CONTEXT_PRIO_MAX; k++)
            {
                vidsch_task_queue_t *q = vidsch_task_queue_alloc(32, k);
                vidsch_task_queue_pool_add(sch_mgr->task_queue_list, q);
                vidsch_task_queue_release(q);
            }
        }

        /* reserve memory for dma buffer */
        if(query_data.dma_buffer_size[i])
        {
            reserved_memory = vidmm_create_segment_memory(adapter, query_data.dma_segment[i], query_data.dma_buffer_size[i], 1, 1);

            ret = vidmm_map_segment_memory(adapter, reserved_memory);
            if(ret != S_OK)
            {
                if(reserved_memory->gpu_vm != NULL)
                {
                    vidmm_unmap_segment_memory(reserved_memory);
                }

                vidmm_destroy_segment_memory(adapter, reserved_memory);
                zx_assert(0, "map dma segment memory failed, with err:0x%x", ret);
            }

            sch_mgr->dma_reserved_memory = reserved_memory;

            vidmm_lock_segment_memory(adapter, reserved_memory);
            zx_memset(reserved_memory->krnl_cpu_vma->virt_addr, 0, query_data.dma_buffer_size[i]);

            sch_mgr->dma_ra = zx_create_range_allocator(reserved_memory->gpu_va, query_data.dma_buffer_size[i], reserved_memory->aligned_page_size);
            zx_debug("engine[%d] reserved dma memory size: %d, segment_id:%d  gpu_virt_addr:%llx, cpu_virt_addr: %p\n", i, reserved_memory->size, sch_mgr->dma_reserved_memory->segment_id, reserved_memory->gpu_va, reserved_memory->krnl_cpu_vma->virt_addr);
        }

        /* reserved memory for chip private initialization use */
        if(query_data.local_memory_size[i])
        {
            /* allocate local reserved memory */
            reserved_memory = vidmm_create_segment_memory(adapter, query_data.local_segment_id, query_data.local_memory_size[i], 1, 1);

            ret = vidmm_map_segment_memory(adapter, reserved_memory);
            if(ret != S_OK)
            {
                if(reserved_memory->gpu_vm != NULL)
                {
                    vidmm_unmap_segment_memory(reserved_memory);
                }
                vidmm_destroy_segment_memory(adapter, reserved_memory);
                zx_assert(0, "map local segment memory failed, with err:0x%x", ret);
            }

            vidmm_lock_segment_memory(adapter, reserved_memory);

            zx_memset(reserved_memory->krnl_cpu_vma->virt_addr, 0, query_data.local_memory_size[i]);

            sch_mgr->local_reserved_memory = reserved_memory;
            zx_debug("engine[%d] reserved local memory size: %d, segment_id:%d  gpu_virt_addr:%llx, cpu_virt_addr: %p\n", i, reserved_memory->size, sch_mgr->local_reserved_memory->segment_id, reserved_memory->gpu_va, reserved_memory->krnl_cpu_vma->virt_addr);
        }

        if(query_data.pcie_memory_size[i])
        {
            /* allocate pcie reserved memory */
            reserved_memory = vidmm_create_segment_memory(adapter, query_data.pcie_segment_id, query_data.pcie_memory_size[i], 1, 1);

            ret = vidmm_map_segment_memory(adapter, reserved_memory);
            if(ret != S_OK)
            {
                if(reserved_memory->gpu_vm != NULL)
                {
                    vidmm_unmap_segment_memory(reserved_memory);
                }

                vidmm_destroy_segment_memory(adapter, reserved_memory);
                zx_error(0, "map pcie segment memory failed, with err:0x%x", ret);
            }

            vidmm_lock_segment_memory(adapter, reserved_memory);

            zx_memset(reserved_memory->krnl_cpu_vma->virt_addr, 0, query_data.pcie_memory_size[i]);

            sch_mgr->pcie_reserved_memory = reserved_memory;

            zx_debug("engine[%d] reserved pcie memory size: %d, segment_id:%d,  gpu_virt_addr:%llx, cpu_virt_addr: %p\n", i, reserved_memory->size, sch_mgr->pcie_reserved_memory->segment_id, reserved_memory->gpu_va, reserved_memory->krnl_cpu_vma->virt_addr);
        }
        else if (adapter->hw_caps.local_only)
        {
            sch_mgr->pcie_reserved_memory = sch_mgr->local_reserved_memory;
        }

        ret = sch_mgr->chip_func->initialize(adapter, sch_mgr);

        if (adapter->ctl_flags.worker_thread_enable && (sch_mgr->engine_ctrl & ENGINE_CTRL_THREAD_ENABLE))
        {
            util_thread_info_t info = {0};
            char thread_name[32];

            zx_memset(&info, 0, sizeof(util_thread_info_t));
            zx_vsprintf(thread_name, "zx_kthread_%d", i);

            info.msec         = 0;
            info.priority     = 99;
            info.thread_name  = thread_name;
            info.private_data = sch_mgr;

            sch_mgr->worker_thread      = util_create_event_thread(vidsch_worker_thread_event_handler, &info);
        }

        sch_mgr->engine_dvfs_power_on = TRUE; //default is on
    }

    vidschi_init_daemon_thread(adapter);

    zx_debug("leave %s.\n", __func__);
}

static void vidschi_fini_vidsch_mgr(adapter_t *adapter)
{
    vidmm_segment_memory_t *reserved_memory = NULL;
    int i;

    for(i = 0; i < adapter->active_engine_count; i++)
    {
        vidsch_mgr_t *sch_mgr = adapter->sch_mgr[i];

        if(sch_mgr == NULL)
        {
            continue;
        }

        //wait worker thread idle and kill it
        if(sch_mgr->worker_thread)
        {
            vidsch_wait_worker_thread_idle(sch_mgr);
        }

        if(sch_mgr->worker_thread)
        {
            util_destroy_event_thread(sch_mgr->worker_thread);
        }

        vidsch_task_queue_pool_release(sch_mgr->task_queue_list);

        if (i == 0)
        {
            vidsch_task_queue_pool_release(sch_mgr->page_table_set_pool);
        }

        if(sch_mgr->private_data)
        {
            sch_mgr->chip_func->destroy(sch_mgr);
        }

        /* free reserved video and pcie memory */
        if(sch_mgr->pcie_reserved_memory)
        {
            reserved_memory = sch_mgr->pcie_reserved_memory;
            zx_debug("destroy pcie memory\n");
            vidmm_destroy_segment_memory(adapter, reserved_memory);

            sch_mgr->pcie_reserved_memory = NULL;
        }

        if(sch_mgr->local_reserved_memory)
        {
            reserved_memory = sch_mgr->local_reserved_memory;
            zx_debug("destroy local memory\n");
            vidmm_destroy_segment_memory(adapter, reserved_memory);

            sch_mgr->local_reserved_memory = NULL;
        }

        if(sch_mgr->dma_reserved_memory)
        {
            reserved_memory = sch_mgr->dma_reserved_memory;
            zx_debug("destroy dma memory\n");
            vidmm_destroy_segment_memory(adapter, reserved_memory);

            sch_mgr->dma_reserved_memory = NULL;
        }

        if(sch_mgr->dma_ra)
        {
            zx_mutex_lock(sch_mgr->dma_lock);
            sch_mgr->dma_ra->release(sch_mgr->dma_ra);
            zx_mutex_unlock(sch_mgr->dma_lock);
            sch_mgr->dma_ra = NULL;
        }

        if (IS_VIDEO_ENGINE(adapter->chip_id, i))
        {
            zx_destroy_event(sch_mgr->prepare_event);
        }

        zx_destroy_mutex(sch_mgr->dma_lock);
        zx_destroy_mutex(sch_mgr->task_list_lock);
        zx_destroy_event(sch_mgr->fence_event);
        zx_destroy_spinlock(sch_mgr->fence_lock);
        zx_destroy_spinlock(sch_mgr->power_status_lock);
        zx_destroy_atomic64(sch_mgr->pending_tasks_num);
        zx_destroy_atomic64(sch_mgr->handled_tasks_num);
        zx_destroy_mutex(sch_mgr->engine_lock);

        zx_free(sch_mgr);
    }

    if(adapter->fence_buf)
    {
        vidschi_destroy_fence_buffer(adapter, adapter->fence_buf);
    }
}

int vidsch_create_reserve_resource(adapter_t *adapter)
{
    create_context_t  create = {0};
    gpu_device_t *device;
    gpu_context_t *context;

    device = cm_create_device(adapter, NULL);
    if(!device)
    {
        zx_error("%s: create reserved device faild.\n", __func__);
        return E_FAIL;
    }

    create.type            = CONTEXT_NORMAL_RENDER;
    create.node_ordinal    = RESERVED_ENGINE_INDEX;
    create.client_type     = CLIENT_TYPE_VULKAN;

    context = cm_create_context(device, &create);
    if (!context)
    {
        zx_error("%s: create reserved context faild.\n", __func__);
        return E_FAIL;
    }

    adapter->reserved_device = device;
    adapter->reserved_context = context;

    vidsch_init_page_table_set_task_queue(adapter->reserved_context);

    return S_OK;
}

int vidsch_init(adapter_t *adapter)
{
    vidschi_init_vidsch_global(adapter);

    vidschi_init_vidsch_mgr(adapter);

    return 0;
}

void vidsch_destroy(adapter_t *adapter)
{
    vidschi_fini_vidsch_global(adapter);

    vidschi_fini_vidsch_mgr(adapter);
}

/**
 * @brief      update fence id
 *
 * @param      sch_mgr        The sch manager
 * @param      curr_returned  The curr returned
 *
 * @return     { description_of_the_return_value }
 */
int vidschi_update_fence_id(vidsch_mgr_t *sch_mgr, unsigned long long *curr_returned)
{
    unsigned long long new_fence, old_fence;

    int  update_new = FALSE;

    //if(sch_mgr->private_data == NULL) return update_new;

    zx_spin_lock_bh(sch_mgr->fence_lock);

    new_fence = sch_mgr->chip_func->update_fence_id(sch_mgr);

    old_fence = sch_mgr->returned_fence_id;

    // zx_debug("%s. new_fence: %llx, old_fence: %llx, engine: %d\n", __func__, new_fence, old_fence, sch_mgr->engine_index);

    /* check if new_returned valid, if not skipped it */
    if((new_fence >  sch_mgr->returned_fence_id) &&
       (old_fence <= sch_mgr->last_send_fence_id))
    {
        sch_mgr->returned_fence_id   = new_fence;
        sch_mgr->uncomplete_task_num = sch_mgr->last_send_fence_id - sch_mgr->returned_fence_id;

        update_new = TRUE;
    }
    else
    {
        new_fence = sch_mgr->returned_fence_id;
    }

    zx_spin_unlock_bh(sch_mgr->fence_lock);

    if(new_fence > old_fence)
    {
        zx_get_nsecs(&sch_mgr->returned_timestamp);
    }

    *curr_returned = new_fence;

    return update_new;
}

void vidschi_notify_fence_interrupt(adapter_t *adapter, unsigned int engine_mask)
{
    vidsch_global_t    *sch_global       = adapter->sch_global;
    vidsch_mgr_t       *sch_mgr          = NULL;
    unsigned int       engine            = 0;
    unsigned long long returned_fence_id = 0ll;
    unsigned long long timestamp = 0;

    int new_fence = FALSE;

    for(engine = 0; engine < adapter->active_engine_count; engine++)
    {
        if((1 << engine) & engine_mask)
        {
            sch_mgr = adapter->sch_mgr[engine];

            if(sch_mgr == NULL) continue;

            new_fence = vidschi_update_fence_id(sch_mgr, &returned_fence_id);

            if(new_fence)
            {
                if (timestamp == 0)
                    zx_get_nsecs(&timestamp);
                vidschi_trace_fence_back(sch_mgr, timestamp, returned_fence_id);
            }

            zx_wake_up_event(sch_mgr->fence_event);

            if(sch_mgr->worker_thread && !vidschi_is_worker_thread_idle(sch_mgr))
            {
                //zx_begin_section_trace_event("util_wakeup_event_thread");
                //zx_counter_trace_event("arg_engine", engine);
                //zx_counter_trace_event("arg_pending_tasks", zx_atomic64_read(sch_mgr->pending_tasks_num));
                //zx_counter_trace_event("arg_handled_tasks", zx_atomic64_read(sch_mgr->handled_tasks_num));
                util_wakeup_event_thread(sch_mgr->worker_thread);
                //zx_end_section_trace_event(0);
            }
            if(adapter->ctl_flags.hwq_event_enable && new_fence)
            {
                if(timestamp==0)
                {
                   zx_get_nsecs(&timestamp);
                }
                hwq_process_complete_event(adapter, timestamp,returned_fence_id & 0xffffffff,engine);
            }
        }
    }

    vidschi_notify_dma_fence(adapter);
}


int vidsch_notify_interrupt(adapter_t *adapter, unsigned int interrupt_event)
{
    vidsch_global_t  *sch_global = adapter->sch_global;

    if(sch_global->disable_schedule)
    {
       return S_OK;
    }

    vidschi_notify_fence_interrupt(adapter, ALL_ENGINE_MASK);

    //do cg and dfs when fence back.
    if(!adapter->hw_caps.video_only &&
        !adapter->hw_caps.gfx_only )
    {
        /*
        if(adapter->hw_caps.dvfs_enable)
            vidsch_try_dvfs_tuning(adapter);
        */

        vidschi_try_power_tuning(adapter);
    }

    return S_OK;
}

int vidsch_notify_power_tuning(adapter_t *adapter, unsigned int interrupt_event)
{
    vidsch_global_t  *sch_global = adapter->sch_global;

    if(sch_global->disable_schedule)
    {
       return S_OK;
    }

    if(!adapter->hw_caps.video_only &&
        !adapter->hw_caps.gfx_only )
    {
        vidschi_try_power_tuning(adapter);
    }

    return S_OK;
}

int vidsch_notify_page_fault(adapter_t *adapter)
{
    vidsch_global_t  *sch_global  = adapter->sch_global;
    vidsch_vm_id_mgr_t *vm_id_mgr = &sch_global->vm_id_mgr;
    gpu_device_t    *device       = NULL;
    gpu_device_t    *device_next  = NULL;
    zx_vm_id_obj_t *vm_id_obj     = NULL;
    int vm_id                     = 0;
    unsigned long long pfVa       = 0;
    zx_vm_zone_t *zone            = NULL;
    struct range_node *range      = NULL;
    zx_vm_node_t *vm_node         = NULL;
    zx_vma_space_t *vma_space     = NULL;

    zx_info("******************PAGE FAULT******************\n");

    pfVa = sch_global->chip_func->notify_page_fault(adapter, &vm_id);

    if (adapter->chip_id == CHIP_CHX004)
    {
        vm_id_obj = &vm_id_mgr->vm_id_array[vm_id];

        list_for_each_entry_safe(device, device_next, &adapter->device_list, list_node)
        {
            if (device->uuid == vm_id_obj->device_uuid)
            {
                vma_space = device->vm;
                zx_info("Page fault device:%p, PID: %d-%s\n", device, device->pid, device->pname);

                break;
            }
        }
    }
    else if (adapter->chip_id == CHIP_CNE001)
    {
        if (vm_id == 0)
            vma_space = adapter->reserved_vma;
        else if (vm_id == 15)
            vma_space = adapter->private_vma;
    }

    if (vma_space == NULL)
    {
        zx_info("Page fault: can't find any vma_space releated to this address\n");
        return E_FAIL;
    }
    else if (vma_space == adapter->reserved_vma && pfVa >= adapter->shared_gpu_va_size)
    {
        zx_info("Page fault: reserved_vma doesn't contain private space\n");
        return E_FAIL;
    }

    zone = vidmm_vm_get_zone_from_range(vma_space, pfVa, 1);
    if (!zone)
    {
        zx_info("Page fault: can't find any zone contain the address\n");
    }
    else if(zone->zone_id == VM_ZONE_ID_SHARE_SPACE_LOCAL)
    {
        zx_info("Page fault: this address is in VM_ZONE_ID_SHARE_SPACE_LOCAL\n");
    }
    else
    {
        zx_mutex_lock(zone->lock);

        range = zone->ra->lookup(zone->ra, pfVa, 1);
        if (range == NULL)
        {
            zx_info("Page fault , can't find any range which include this address\n");
        }
        else
        {
            zx_info("Page fault start_addr = %llx, end_addr = %llx\n", range->start, range->start + range->size);

            vm_node = range->private;
            if (vm_node == NULL)
            {
                zx_info("Page fault: can't find releated vm_node which include  this range\n");
            }
            else
            {
                if (vm_node->mapped_entity == NULL)
                {
                    zx_info("Page fault: the releated vm_node->mapped_entity is NULL\n");
                }
                else if (vm_node->mapped_entity_type == ZX_VM_NODE_TYPE_SEGMENET_MEMORY)
                {
                    zx_info("Page fault: this vm_node's type is segment_memory\n");

                    zx_info("Page fault: dump page fault address's vm_info:\n");
                    vidmm_vm_dump_address_info(adapter, vma_space, vm_node, range->start, pfVa);
                }
                else if (vm_node->mapped_entity_type == ZX_VM_NODE_TYPE_ALLOCATION)
                {
                    vidmm_allocation_t *allocation = vm_node->mapped_entity;

                    zx_info("Page fault: this vm_node's type is allocation: segment_id = %d, debug_gid = %x\n",
                            allocation->segment_id, allocation->debug_gid);

                    zx_info("Page fault: dump page fault address's vm_info:\n");
                    vidmm_vm_dump_address_info(adapter, vma_space, vm_node, range->start, pfVa);
                }
            }
        }

        zx_mutex_unlock(zone->lock);
    }

    return S_OK;
}


unsigned long long vidsch_get_current_returned_fence(adapter_t *adapter, int engine_index)
{
    vidsch_mgr_t *vidsch = adapter->sch_mgr[engine_index];

    return vidsch->returned_fence_id;
}

/* check if fence is back in target engine */
int vidsch_is_fence_back(adapter_t *adapter, unsigned char engine_index, unsigned long long fence_id)
{
    vidsch_mgr_t  *sch_mgr = adapter->sch_mgr[engine_index];
    unsigned long long returned_fence_id = 0ll;
    vidschi_update_fence_id(sch_mgr, &returned_fence_id);

    if(sch_mgr->returned_fence_id >= fence_id)
        return 1;

    return 0;
}

void vidsch_update_engine_idle_status(adapter_t *adapter, unsigned int engine_mask)
{
    vidsch_mgr_t  *sch_mgr  = NULL;
    vidsch_global_t *sch_global = adapter->sch_global;
    unsigned long long timestamp  = 0ll;
    unsigned long long returned_fence_id = 0ll;

    unsigned int i = 0;
    unsigned int task_list_empty;
    unsigned int work_thread_idle;
    unsigned int fence_back;

    sch_global->busy_engine_mask = 0;

    for(i = 0; i < adapter->active_engine_count; i++)
    {
        if((engine_mask & (1<<i)) == 0) continue;

        sch_mgr = adapter->sch_mgr[i];
        if(sch_mgr == NULL) continue;

        work_thread_idle = vidschi_is_worker_thread_idle(sch_mgr);

        vidschi_update_fence_id(sch_mgr, &returned_fence_id);

        zx_spin_lock_bh(sch_mgr->fence_lock);
        fence_back = sch_mgr->returned_fence_id >= sch_mgr->last_send_fence_id ? 1:0;
        zx_spin_unlock_bh(sch_mgr->fence_lock);

        zx_get_nsecs(&timestamp);
        timestamp = zx_do_div(timestamp, 1000000);

        //task_list_empty &&
        if(work_thread_idle && fence_back)
        {
            sch_mgr->last_idle = timestamp;
            sch_mgr->completely_idle = 1;
            sch_mgr->idle_elapse = sch_mgr->last_idle - sch_mgr->last_busy;
            sch_global->busy_engine_mask &= ~(1 << i);
        }
        else
        {
            sch_mgr->last_busy = timestamp;
            sch_mgr->completely_idle = 0;
            sch_mgr->idle_elapse = 0;
            sch_global->busy_engine_mask |= 1 << i;
        }
    }
}

int vidsch_is_fence_back_condition(void *argu)
{
    vidsch_wait_fence_back_arg_t *wait_fence = argu;

    return vidsch_is_fence_back(wait_fence->adapter, wait_fence->engine, wait_fence->fence_id);
}

/** vidsch_wait_fence_interrupt implemented by vidsch_wait_fence_back.
 *  So if use vidsch_wait_fence_interrupt, before wait check, we need
 *  log current fence id, if check failed, we can directly wait
 *  fence_id + 1 back.
 **/
int vidsch_wait_fence_back(adapter_t *adapter, unsigned char engine_index, unsigned long long fence_id, unsigned int force_wait)
{
    vidsch_mgr_t                 *sch_mgr        = adapter->sch_mgr[engine_index];
    vidsch_wait_fence_back_arg_t  wait_fence      = {0};
    condition_func_t              condition       = vidsch_is_fence_back_condition;
    zx_event_status_t            e_status        = ZX_EVENT_UNKNOWN;
    int                           timeout_base    = /*adapter->ctl_flags.run_on_qt ? 2000*100 :*/ 2000*10;
    int                           timeout         = adapter->hw_caps.fence_interrupt_enable ? timeout_base:2;
    int timeout_cnt = 0, max_timeout     = 40*timeout_base /timeout;

    zx_begin_section_trace_event("wait_fence_back");
    zx_counter_trace_event("arg_engine_index", engine_index);
    zx_counter_trace_event("arg_fence_id", fence_id);

    wait_fence.adapter  = adapter;
    wait_fence.engine   = engine_index;
    wait_fence.fence_id = fence_id;

#ifdef ZX_HW_NULL
    e_status = ZX_EVENT_SIGNAL;
#else
    do
    {
        e_status = zx_wait_event_thread_safe(sch_mgr->fence_event, condition, &wait_fence, timeout);

        /* general timeout should not happen, but only HW hang, or interrupt disabled(we only update fence id on ISR)
         * So if interrutp disabled, we need a place to update fence too, we add here.
         */

        /*
         * Notice: pls do not add any printk to here, consider this: when system reboot, interrupt already disabled,
         * we need use timeout to update the fence, if fb enabled and printk add here, it will lead a dead loop like
         * this, we loop to wait hw idle, but we add some draw on this loop, this lead this loop never end.
         */
        if(force_wait && e_status == ZX_EVENT_TIMEOUT)
        {
            timeout_cnt++;

            if (timeout_cnt > max_timeout)
            {
                zx_info("FENCE not back. engine index %x, wait fence id %lld, last send: %lld, last_return: %lld\n",
                    engine_index, fence_id, sch_mgr->last_send_fence_id, sch_mgr->returned_fence_id);

                zx_dump_stack();

                max_timeout = 0;
            }

            /* timeout happen, force notify a interrupt to update fence id */
            vidsch_notify_interrupt(adapter, 0);
        }
    }while(force_wait && e_status != ZX_EVENT_BACK);

#endif

    zx_end_section_trace_event(e_status);

    return e_status;
}

void vidsch_wait_engine_idle(adapter_t *adapter, int idx)
{
    vidsch_mgr_t *sch_mgr = adapter->sch_mgr[idx];

    if(!sch_mgr)
        return;

    if(sch_mgr->worker_thread)
    {
        vidsch_wait_worker_thread_idle(sch_mgr);
    }

    vidsch_wait_fence_back(sch_mgr->adapter, idx, sch_mgr->last_send_fence_id, TRUE);
}


void vidsch_wait_chip_idle(adapter_t *adapter, unsigned int engine_mask)
{
    vidsch_global_t  *sch_global = adapter->sch_global;
    int i = 0;

    /*
    if(sch_global->chip_func->disable_dvfs)
    {
        sch_global->chip_func->disable_dvfs(adapter);
    }*/

    if(adapter->hw_caps.video_only)
    {
        engine_mask &= (1<<2);
    }

    for(i = 0; i < adapter->active_engine_count; i ++)
    {
         if((engine_mask & (1 << i)) && (adapter->sch_mgr[i] != NULL))
         {
               vidsch_wait_engine_idle(adapter, i);
         }
    }
}

/**
 * @brief      increase last_send_fence_id
 *
 * @param      sch_mgr          The sch manager
 * @param[in]  dec_prepare_num  The decrement prepare number
 *
 * @return     fence id
 */
unsigned long long vidschi_inc_send_fence_id(vidsch_mgr_t *sch_mgr, int dec_prepare_num)
{
    unsigned long long fence_id;
    int can_prepare = FALSE;

    zx_spin_lock_bh(sch_mgr->fence_lock);

    sch_mgr->last_send_fence_id++;

    sch_mgr->uncomplete_task_num = sch_mgr->last_send_fence_id - sch_mgr->returned_fence_id;

    fence_id = sch_mgr->last_send_fence_id;

    zx_spin_unlock_bh(sch_mgr->fence_lock);

    return fence_id;
}

int vidsch_query_hw_hang(adapter_t *adapter, zx_query_info_t *info)
{

    vidsch_global_t *schedule = adapter->sch_global;

    info->value = schedule->hw_hang;

    schedule->hw_hang = 0;

    //zx_info("%s() hw_hang-0x%x\n", __func__, info->value);

    return 0;
}

void vidsch_get_timestamp(adapter_t *adapter, unsigned long long *time_stamp)
{
    vidsch_mgr_t *sch_mgr = adapter->sch_mgr[0];
    return sch_mgr->chip_func->get_timestamp(adapter, time_stamp);
}

void vidsch_get_set_reg(adapter_t *adapter, zx_query_info_t *info)
{
    vidsch_global_t  *sch_global = adapter->sch_global;
    if(sch_global->chip_func->get_set_reg)
    {
        return sch_global->chip_func->get_set_reg(adapter, info);
    }
}


int vidsch_query_info(adapter_t *adapter, zx_query_info_t *info)
{
    int  ret = 0;

    switch(info->type)
    {
        case ZX_QUERY_HW_HANG:
            ret = vidsch_query_hw_hang(adapter, info);
            break;
        case ZX_QUERY_GET_VIDEO_BRIDGE_BUFFER:
            info->value64 =  adapter->sch_mgr[info->argu + RB_INDEX_VIDEO_START]->local_reserved_memory->gpu_va;
            break;
        case ZX_QUERY_GPU_TIME_STAMP:
            vidsch_get_timestamp(adapter, &info->value64);
            break;
        case ZX_QUERY_REGISTER_U32:
        case ZX_SET_REGISTER_U32:
            vidsch_get_set_reg(adapter, info);
            break;

        default:
            zx_assert(0, "invalidate type: %d", info->type);
    }

    return ret;
}

int vidsch_save(adapter_t *adapter)
{
    vidsch_mgr_t *vidsch = NULL;
    int i = 0;
    int result = 0;

    vidsch_wait_chip_idle(adapter, ALL_ENGINE_MASK);

    for(i = 0; i < adapter->active_engine_count; i++)
    {
        vidsch = adapter->sch_mgr[i];

        if(!vidsch || !vidsch->chip_func ||!vidsch->chip_func->save)  continue;

        result = vidsch->chip_func->save(vidsch);
        if (result)
        {
             return result;
        }
    }
    return result;
}

void vidsch_restore(adapter_t *adapter)
{
    vidsch_mgr_t *vidsch = NULL;
    int i = 0;

    for(i = 0; i < adapter->active_engine_count; i++)
    {
        vidsch = adapter->sch_mgr[i];

        if(!vidsch || !vidsch->chip_func ||!vidsch->chip_func->restore)  continue;

        vidsch->chip_func->restore(vidsch, TRUE);

        vidsch->engine_dvfs_power_on = TRUE;
        vidsch->init_submit = TRUE;
    }
}

void vidsch_restore_dvfs(adapter_t *adapter)
{
    vidsch_mgr_t *vidsch = NULL;
    int i = 0;

    for(i = 0; i < adapter->active_engine_count; i++)
    {
        vidsch = adapter->sch_mgr[i];

        if(!vidsch || !vidsch->chip_func ||!vidsch->chip_func->restore_dvfs)  continue;

        vidsch->chip_func->restore_dvfs(vidsch);
    }
}

void vidsch_dump(struct os_printer *p, adapter_t *adapter)
{
    vidsch_global_t *sch_global = adapter->sch_global;
    vidsch_mgr_t *sch_mgr;
    task_desc_t *task;
    vidsch_task_queue_pool_t *pool;
    vidsch_task_queue_t *q;
    int idx = 0, engine = 0;

    zx_printf(p, "=====engine fence status=====\n");
    for(engine = 0; engine < adapter->active_engine_count; engine++)
    {
        idx = 0;
        sch_mgr = adapter->sch_mgr[engine];

        if(!sch_mgr) continue;

        zx_printf(p, "*engine[%d]: returned fence: %llx, last send: %llx, in mem: %llx\n", engine, sch_mgr->returned_fence_id, sch_mgr->last_send_fence_id, *sch_mgr->fence_buffer_cpu_va);
    }
    zx_printf(p, "\n");

    zx_printf(p, "=====submited tasks fence not back=====\n");
    for(engine = 0; engine < adapter->active_engine_count; engine++)
    {
        idx = 0;
        sch_mgr = adapter->sch_mgr[engine];

        if(!sch_mgr) continue;

        zx_printf(p, "**engine[%d]:\n", engine);

        zx_mutex_lock(sch_mgr->task_list_lock);

        list_for_each_entry(task, &sch_mgr->submitted_task_list, list_item)
        {
            zx_printf(p, "  [%d] type: %d, fence_id: %lld, ref: %d\n",
                    idx++, task->type, task->fence_id, zx_atomic_read(task->ref_cnt));
        }

        zx_mutex_unlock(sch_mgr->task_list_lock);
    }
    zx_printf(p, "\n");

    for(engine = 0; engine < adapter->active_engine_count; engine++)
    {
        sch_mgr = adapter->sch_mgr[engine];
        if (!sch_mgr) continue;

        zx_get_nsecs(&sch_mgr->current);
        zx_printf(p, "***engine[%d] dump task pool and queue, current time: %lld\n", engine, sch_mgr->current);

        if(engine == 0)
        {
            zx_printf(p, "=====page_table_set_pool=====\n");
            pool = sch_mgr->page_table_set_pool;
            zx_mutex_lock(pool->lock);
            list_for_each_entry(q, &pool->queue_list, link)
            {
                zx_printf(p, "***** queue id: %d, priority: %d, task_num: %d, last_touch: %lld *****\n",
                        q->id, q->priority, q->task_num, q->last_active);
                /*
                zx_spin_lock(q->lock);
                list_for_each_entry(task, &q->task_list, schedule_node)
                {
                    zx_printf(p, "  desc: %p, ref: %d, type: %d, enqueue: %lld, last update submit timestamp: %lld\n",
                            task, zx_atomic_read(task->ref_cnt), task->type, task->enqueue_timestamp, task->timestamp);
                }
                zx_spin_unlock(q->lock);
                */
            }

            for (idx = 0; idx <= pool->max_priority; idx++)
            {
                list_for_each_entry(q, pool->active_list + idx, active_link)
                {
                    zx_printf(p, "***** activeq id: %d, priority: %d,  task_num: %d *****\n", q->id, q->priority, q->task_num);
                    zx_printf(p, "***** dump sw task queue:\n");
                    zx_spin_lock(q->lock);
                    list_for_each_entry(task, &q->task_list, schedule_node)
                    {
                        zx_printf(p, "  desc: %p, ref: %d, type: %d, enqueue: %lld, last update submit timestamp: %lld\n",
                                task, zx_atomic_read(task->ref_cnt), task->type, task->enqueue_timestamp, task->timestamp);
                    }
                    zx_spin_unlock(q->lock);
                }
            }
            zx_mutex_unlock(pool->lock);
        }

        zx_printf(p, "=====task_queue_pool=====\n");
        pool = sch_mgr->task_queue_list;
        zx_mutex_lock(pool->lock);
        list_for_each_entry(q, &pool->queue_list, link)
        {
            zx_printf(p, "***** queue id: %d, priority: %d, task_num: %d, last_touch: %lld *****\n",
                        q->id, q->priority, q->task_num, q->last_active);
            /*
            zx_spin_lock(q->lock);
            list_for_each_entry(task, &q->task_list, schedule_node)
            {
                zx_printf(p, "  desc %p, ref %d, type %d, enqueue:%lld timestamp:%lld\n",
                        task, zx_atomic_read(task->ref_cnt), task->type, task->enqueue_timestamp, task->timestamp);
            }
            zx_spin_unlock(q->lock);
            */
        }

        for (idx = 0; idx <= pool->max_priority; idx++)
        {
            list_for_each_entry(q, pool->active_list + idx, active_link)
            {
                zx_printf(p, "***** activeq id: %d, priority: %d,  task_num: %d *****\n", q->id, q->priority, q->task_num);
                zx_printf(p, "***** dump sw task queue:\n");
                zx_spin_lock(q->lock);
                list_for_each_entry(task, &q->task_list, schedule_node)
                {
                    zx_printf(p, "  desc: %p, ref: %d, type: %d, enqueue: %lld, last update submit timestamp: %lld\n",
                            task, zx_atomic_read(task->ref_cnt), task->type, task->enqueue_timestamp, task->timestamp);
                }
                zx_spin_unlock(q->lock);
            }
        }
        zx_mutex_unlock(pool->lock);
    }
}

int vidsch_cil2_misc(gpu_device_t *device, krnl_cil2_misc_t *misc)
{
    adapter_t       *adapter = device->adapter;
    vidsch_mgr_t    *sch_mgr = adapter->sch_mgr[0];

    if (misc->context)
    {
        sch_mgr = adapter->sch_mgr[misc->context->rb_index];
    }

    return sch_mgr->chip_func->cil2_misc(sch_mgr, misc);
}

int vidsch_dump_debugbus(struct os_printer *p, adapter_t *adapter)
{
    vidsch_global_t* sch_global = adapter->sch_global;

    if (sch_global->chip_func->dump_debugbus)
    {
        sch_global->chip_func->dump_debugbus(p, adapter);
    }
    return 0;
}

void vidsch_wakeup_worker_thread(adapter_t *adapter, int engine_index)
{
    vidsch_mgr_t *sch_mgr = adapter->sch_mgr[engine_index];

    if(sch_mgr)
        util_wakeup_event_thread(sch_mgr->worker_thread);
}

void vidsch_power_mask_acquire(adapter_t *adapter, gpu_device_t *device)
{
    vidsch_global_t* sch_global = adapter->sch_global;

    cm_device_lock(device);
    if(!device->power_dvfs_indicador)
    {
        device->power_dvfs_indicador = zx_find_next_zero_bit(sch_global->dvfs_indicator,MAX_POWER_MASK_NUM, 1);
        if(device->power_dvfs_indicador != MAX_POWER_MASK_NUM)
        {
            zx_set_bit(device->power_dvfs_indicador,sch_global->dvfs_indicator);
        }
        else
        {
            zx_warning("power_dvfs_indicador recahed max :%d\n",device->power_dvfs_indicador);
        }
    }
    cm_device_unlock(device);
}
void vidsch_power_mask_clear(gpu_device_t *device)
{
    adapter_t    *adapter = device->adapter;
    vidsch_global_t* sch_global = adapter->sch_global;
    if(device->power_dvfs_indicador)
    {
        zx_clear_bit(device->power_dvfs_indicador,sch_global->dvfs_indicator);
    }
}
