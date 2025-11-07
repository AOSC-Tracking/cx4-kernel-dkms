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
#include "vidsch_render.h"
#include "vidsch_paging.h"
#include "vidsch_vm_update.h"
#include "vidmm.h"
#include "vidsch_trace.h"

void vidschi_dump_task(task_desc_t *task, int idx, int dump_detail)
{
    switch (task->type)
    {
    case task_type_render:
        vidschi_dump_dma_task((task_dma_t*)task, idx, dump_detail);
        break;

    case task_type_paging:
        vidschi_dump_paging_task((task_paging_t*)task, idx, dump_detail);
        break;

    case task_type_vm_update:
        vidschi_dump_vm_update_task((task_vm_update_t*)task, idx, dump_detail);
        break;

    case task_type_page_table_set:
        vidschi_dump_page_table_set_task((task_page_table_set_t *)task, idx, dump_detail);
        break;

    default:
        zx_error("Unknown Task type: %d.\n", task->type);
        break;
    }
}

static void vidschi_do_release_task(vidsch_mgr_t *sch_mgr, task_desc_t *desc)
{
    gpu_context_t *context = desc->context;

    vidschi_trace_task_release(sch_mgr, desc);

    zx_destroy_atomic(desc->ref_cnt);
    desc->ref_cnt = NULL;

    desc->release(sch_mgr, desc);
    if(context)
    {
        zx_atomic_dec(context->ref_cnt);
    }
    zx_atomic64_inc(sch_mgr->handled_tasks_num);
}

void vidschi_dump_allocation(int index, void *alloc)
{
    vidmm_allocation_t *allocation = (vidmm_allocation_t *) alloc;

    zx_info("allo[%03d]: %p, size: %6dk, temp_unpagable status: %6d.\n",
        index, allocation,
        util_align(allocation->size, allocation->aligned_page_size) >> 10,
        allocation->status.temp_unpagable);
}

task_desc_t *vidschi_uncompleted_task_exceed_wait_time(vidsch_mgr_t *sch_mgr, unsigned long long max_wait_time)
{
    adapter_t   *adapter = sch_mgr->adapter;
    task_desc_t *task    = NULL;
    int         exceed   = FALSE;

    vidsch_release_completed_tasks(sch_mgr, NULL);

    zx_mutex_lock(sch_mgr->task_list_lock);

    if(!list_empty(&sch_mgr->submitted_task_list))
    {
        unsigned long long curr_time  = 0ll;

        task = list_entry(sch_mgr->submitted_task_list.next, task_desc_t, list_item);

        zx_get_nsecs(&curr_time);

        /* NOTE: all timestamp is unsigned value, so if x - y < 0, since the type it minus value still a
         * unsigned which will be big vale, So not use if(x - y > delta) but if(x + delta > y) to avoid overflow issue
         */
#ifdef ZX_VMI_MODE
        if(sch_mgr->returned_fence_id < task->fence_id)
#else
        if((task->timestamp + max_wait_time) < curr_time && sch_mgr->returned_fence_id < task->fence_id)
#endif
        {
            if (sch_mgr->chip_func->update_fence_id(sch_mgr) >= task->fence_id)
            {
                zx_info("recovery from fence value update timeout.\n");
            }
            else
            {
                zx_info("hang detect: engine[%d], hang task used_time: %dms, submit_time: %dms, curr_time: %dms.\n",
                    sch_mgr->engine_index,
                    (unsigned int)zx_do_div(curr_time - task->timestamp, 1000000),
                    (unsigned int)zx_do_div(task->timestamp, 1000000),
                    (unsigned int)zx_do_div(curr_time, 1000000));

                zx_info("hang task info: type: %d, dma_size: %d, fence id: %lld, engine returned fence id: %lld, fence buffer actually id: %lld.\n",
                        task->type, task->cmd_size, task->fence_id, sch_mgr->returned_fence_id,
                        sch_mgr->chip_func->update_fence_id(sch_mgr));

                exceed = TRUE;

                vidsch_task_inc_reference(task);
            }
        }
    }

    zx_mutex_unlock(sch_mgr->task_list_lock);

    return exceed ? task : NULL;
}

void vidsch_task_init_desc(vidsch_mgr_t *sch_mgr, gpu_context_t *context, task_desc_t *desc, task_type_t type)
{
    zx_atomic_inc(context->ref_cnt);
    desc->context = context;
    desc->uuid = zx_atomic64_inc(sch_mgr->adapter->task_uuid);
    desc->type = type;
    desc->vm_id = -1;
    desc->cmd_size = 0;
    desc->ref_cnt = zx_create_atomic(1);
    list_init_head(&desc->list_item);
    desc->need_send_irq = 1;

    zx_atomic64_inc(sch_mgr->pending_tasks_num);
    vidschi_trace_task_create(sch_mgr, desc);
}

void vidsch_task_inc_reference(task_desc_t *desc)
{
    zx_atomic_inc(desc->ref_cnt);
}

void vidsch_task_dec_reference(task_desc_t *desc)
{
    gpu_context_t   *context    = desc->context;
    adapter_t       *adapter    = context->device->adapter;
    vidsch_mgr_t    *sch_mgr    = adapter->sch_mgr[context->rb_index];

    if (zx_atomic_dec(desc->ref_cnt) == 0)
    {
        vidschi_do_release_task(sch_mgr, desc);
    }
}

void vidsch_release_completed_tasks(vidsch_mgr_t *sch_mgr, unsigned long long *uncompleted_dma)
{
    task_desc_t *task = NULL, *next;

try_next:
    zx_mutex_lock(sch_mgr->task_list_lock);

    if(!list_empty(&sch_mgr->submitted_task_list))
    {
        task = list_entry(sch_mgr->submitted_task_list.next, task_desc_t, list_item);

        if(!vidsch_is_fence_back(sch_mgr->adapter, sch_mgr->engine_index, task->fence_id))
        {
            if(task->timestamp < sch_mgr->returned_timestamp)
            {
                task->timestamp = sch_mgr->returned_timestamp;
            }

            if(uncompleted_dma)
                *uncompleted_dma = task->fence_id;

            task = NULL;
        }
        else
        {
            list_del(&task->list_item);
        }
    }
    zx_mutex_unlock(sch_mgr->task_list_lock);

    if(task)
    {
        vidsch_task_dec_reference(task);

        task = NULL;
        goto try_next;
    }
}

void vidschi_dump_general_task_info(task_desc_t *task)
{
    gpu_context_t       *context    = task->context;
    gpu_device_t        *device     = context->device;

    zx_info("Device: %p, Context: %p, Engine: %d.\n",
        device, context, context->rb_index);
    zx_info("Submitted By Process PID: %d-%s, TID: %d.\n",
        device->pid, device->pname, context->tid);
}

void vidschi_set_uncompleted_task_dropped(vidsch_mgr_t *sch_mgr, unsigned long long dropped_task)
{
    task_desc_t *task = NULL;

    zx_mutex_lock(sch_mgr->task_list_lock);

    list_for_each_entry(task, &sch_mgr->submitted_task_list, list_item)
    {
        if(task->fence_id <= dropped_task)
        {
            task->reset_dropped = TRUE;

            zx_info("task: %p, cmd_size: %d, fence_id: %lld, type: %d, dropped.\n",
                task, task->cmd_size, task->fence_id, task->type);

            if(task->type == task_type_render)
            {
                task_dma_t    *render  = (task_dma_t*)task;
                gpu_context_t *context = task->context;

                zx_info("task: %p, context: %p, tid: %d.\n",
                    render, context, context->tid);
            }
        }
    }

    zx_mutex_unlock(sch_mgr->task_list_lock);
}

dma_node_t *vidschi_allocate_dma_node(vidsch_mgr_t *sch_mgr, unsigned int dma_size)
{
    vidmm_segment_memory_t  *dma_reserved_memory = sch_mgr->dma_reserved_memory;
    dma_node_t              *dma                 = zx_calloc(sizeof(dma_node_t));
    int                     sleep_cnt            = 0;

    while(dma->range == NULL && sleep_cnt++ < 500)
    {
        zx_mutex_lock(sch_mgr->dma_lock);
        dma->range = sch_mgr->dma_ra->alloc(sch_mgr->dma_ra, dma_size, 0, 0, -1, 0);
        zx_mutex_unlock(sch_mgr->dma_lock);

        if(dma->range == NULL)
        {
            if (sch_mgr->last_send_fence_id > sch_mgr->returned_fence_id)
            {
                vidsch_wait_fence_back(sch_mgr->adapter, sch_mgr->engine_index, sch_mgr->returned_fence_id + 1, 1);
                sleep_cnt = 0;
            }
            else
            {
                zx_msleep(20);
                if(sch_mgr->worker_thread)
                {
                    // notify workthread to recycle the completed task
                    util_wakeup_event_thread(sch_mgr->worker_thread);
                }
            }
        }
    }
    if (!dma->range)
    {
        zx_error("allocate_dma_node(size=%d) failed.", dma_size);
        return NULL;
    }

    dma->gpu_virtual_address  = dma->range->start;
    dma->cpu_virtual_address  = dma_reserved_memory->krnl_cpu_vma->virt_addr + dma->range->start - dma_reserved_memory->gpu_va;
    dma->size                 = dma->range->size;
    dma->command_length       = 0;

    return dma;
}

void vidschi_release_dma_node(vidsch_mgr_t *sch_mgr, dma_node_t *dma)
{
    //zx_info("release dma:%p gvirt_addr: %x, size: %lld, request_size: %d.\n",
    //    dma, dma->gpu_virtual_address, dma->size, dma->command_length);
    zx_mutex_lock(sch_mgr->dma_lock);
    dma->range->release(dma->range);
    zx_mutex_unlock(sch_mgr->dma_lock);

    zx_free(dma);
}

void vidsch_update_dma_command_length(dma_node_t *dma, unsigned int command_length)
{
     dma->command_length = command_length;
}

