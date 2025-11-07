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
#include "vidsch_submit.h"
#include "vidmm.h"
#include "vidsch_trace.h"

static void vidschi_set_task_submitted(vidsch_mgr_t *sch_mgr, task_desc_t *task, int fake_submit, int fake_dma)
{
    /* update submit timestamp */
    zx_get_nsecs(&task->timestamp);

    if(fake_submit)
        task->fake_submitted = 1;

    vidschi_trace_task_submit(sch_mgr, task, fake_dma);

    zx_mutex_lock(sch_mgr->task_list_lock);
    if (task->list_item.next != &task->list_item)
    {
        zx_error("??? task already in submitted list, maybe a bug!!!!!!\n");
        list_del(&task->list_item);

        if(!fake_submit)
            list_add_tail(&task->list_item, &sch_mgr->submitted_task_list);
    }
    else
    {
        if(!fake_submit)
        {
            list_add_tail(&task->list_item, &sch_mgr->submitted_task_list);
            vidsch_task_inc_reference(task);
        }
        else
        {
            task = NULL;
        }
    }
    zx_mutex_unlock(sch_mgr->task_list_lock);

    if(fake_submit)
    {
        if(task)
            vidsch_task_dec_reference(task);
    }
    else
        task->submitted = TRUE;
}

/* fake submit: use to update device info when submit failed */
static void vidschi_fake_submit_dma(vidsch_mgr_t *sch_mgr, task_dma_t *dma_task, int discard)
{
    gpu_context_t      *context    = dma_task->desc.context;

    int i;

    zx_get_nsecs(&dma_task->desc.timestamp);

    vidschi_update_and_release_dma_fence(&dma_task->desc);

    vidschi_set_task_submitted(sch_mgr, &dma_task->desc, 1, 1);

    vidsch_notify_interrupt(sch_mgr->adapter, 0);
}

#ifdef ZX_HW_NULL
static void vidschi_fake_submit_dma_with_fence(vidsch_mgr_t *sch_mgr, task_dma_t *dma_task)
{
    unsigned long long completed_fence_id = dma_task->desc.fence_id;

    zx_debug("fake submit dma task with fence_id: %lld\n", completed_fence_id);
    if(sch_mgr->chip_func->set_fence_id)
    {
        sch_mgr->chip_func->set_fence_id(sch_mgr, completed_fence_id);
    }
    vidschi_fake_submit_dma(sch_mgr, dma_task, FALSE);
}
#endif

static int vidschi_submit_dma(vidsch_mgr_t *sch_mgr, task_dma_t *dma_task)
{
    adapter_t                    *adapter       = sch_mgr->adapter;
    vidsch_global_t              *sch_global    = adapter->sch_global;
    gpu_context_t                *context       = dma_task->desc.context;
    unsigned long long            fence_id;
    int i, ret = S_OK;

    zx_down_read(sch_global->rw_lock);

    if(adapter->ctl_flags.hang_dump)
    {
        zx_mutex_lock(adapter->hang_dump_lock);
    }

    zx_mutex_lock(sch_mgr->engine_lock);

    //zx_mutex_lock(adapter->hw_reset_lock);

    fence_id = vidschi_inc_send_fence_id(sch_mgr, FALSE);

    dma_task->desc.fence_id   = fence_id;

    vidsch_choose_vm_id(context->device, &dma_task->desc);

    vidschi_update_and_release_dma_fence(&dma_task->desc);

    ret = sch_mgr->chip_func->submit(context, sch_mgr, dma_task);

    //zx_mutex_unlock(adapter->hw_reset_lock);
    zx_mutex_unlock(sch_mgr->engine_lock);

    vidschi_set_task_submitted(sch_mgr, &dma_task->desc, 0, 0);

    if(adapter->ctl_flags.hang_dump)
    {
        zx_mutex_unlock(adapter->hang_dump_lock);
    }
    zx_up_read(sch_global->rw_lock);

    return ret;
}

int vidschi_prepare_and_submit_dma(vidsch_mgr_t *sch_mgr, task_dma_t *dma_task)
{
    hw_ctxbuf_t     *hw_ctx  = dma_task->hw_ctx_info;
    int             result   = S_OK;
    int             discard  = FALSE;

    if(hw_ctx && hw_ctx->is_invalid)
    {
        vidschi_fake_submit_dma(sch_mgr, dma_task, TRUE);

        return E_FAIL;
    }

    if(dma_task->flag.null_rendering)
    {
        vidschi_fake_submit_dma(sch_mgr, dma_task, TRUE);

        return S_OK;
    }

    if(dma_task->desc.dma_cnt == 0)
    {
        vidschi_fake_submit_dma(sch_mgr, dma_task, TRUE);

        return S_OK;
    }

    vidsch_prepare_dma_task(dma_task);

    if(dma_task->flag.has_resident)
    {
        vidschi_submit_dma(sch_mgr, dma_task);
    }
    else
    {
        /* if prepare task failed by pages not enough, discard the task */
        discard = TRUE;
    }

    if(discard)
    {
        /* submit fail, mark hw_ctx invalid and discard this dma_task */
        gpu_context_t *context = dma_task->desc.context;

        zx_info("submit failed, set hwctx invalid.\n");

        context->device->task_dropped  = TRUE;

        if(hw_ctx)
        {
            hw_ctx->is_invalid = TRUE;
        }

        vidschi_fake_submit_dma(sch_mgr, dma_task, TRUE);
    }

#if defined(ZX_HW_NULL)
    vidschi_fake_submit_dma_with_fence(sch_mgr, dma_task);
#endif

#if defined(ZX_VMI_MODE)
    {
        unsigned long long uncompleted_dma = 0;

        do {
            vidschi_notify_fence_interrupt(sch_mgr->adapter, -1);
            vidsch_release_completed_tasks(sch_mgr, &uncompleted_dma);
        } while(uncompleted_dma > 0);
    }
#endif

    return result;
}

void vidsch_submit_paging_task(adapter_t *adapter, task_paging_t *paging_task)
{
    vidsch_global_t              *sch_global = adapter->sch_global;
    gpu_context_t                *context    = paging_task->desc.context;
    vidsch_mgr_t                 *sch_mgr    = adapter->sch_mgr[context->rb_index];
    task_dma_t task_dma;
    unsigned long long fence_id;

    if(paging_task->dma->command_length == 0)
    {
        vidsch_notify_interrupt(adapter, 0); //render_count update, wait up waiter

        vidschi_set_task_submitted(sch_mgr, &paging_task->desc, 1, 1);

        return;
    }

    zx_memset(&task_dma, 0, sizeof(task_dma_t));
    task_dma.desc.type                = task_type_paging;
    task_dma.dma_type                 = PAGING_DMA;
    task_dma.flag.need_hwctx_switch   = FALSE;
    task_dma.dma_buffer_list          = paging_task->dma;
    task_dma.desc.dma_cnt             = 1;
    task_dma.desc.context             = context;
    task_dma.desc.uuid                = paging_task->desc.uuid;

    zx_down_read(sch_global->rw_lock);

    if(adapter->ctl_flags.hang_dump)
    {
        zx_mutex_lock(adapter->hang_dump_lock);
    }
    /* lock engine */
    zx_mutex_lock(sch_mgr->engine_lock);

    fence_id = vidschi_inc_send_fence_id(sch_mgr, FALSE);

    paging_task->desc.fence_id = fence_id;
    paging_task->desc.cmd_size = paging_task->dma->command_length;

    vidsch_choose_vm_id(context->device, &paging_task->desc);

    task_dma.desc.fence_id     = fence_id;
    task_dma.desc.vm_id        = paging_task->desc.vm_id;

    /* call chip submit dma buffer */
    sch_mgr->chip_func->submit(paging_task->desc.context, sch_mgr, &task_dma);

    /* unlock engine */
    zx_mutex_unlock(sch_mgr->engine_lock);

    /* set dma node submitted*/
    vidschi_set_task_submitted(sch_mgr, &paging_task->desc, 0, 0);

    if(adapter->ctl_flags.hang_dump)
    {
      zx_mutex_unlock(adapter->hang_dump_lock);
    }
    zx_up_read(sch_global->rw_lock);
}

void vidsch_submit_vm_update_task(adapter_t *adapter, task_vm_update_t *vm_task)
{
    int                          i;
    vidsch_global_t              *sch_global = adapter->sch_global;
    gpu_context_t                *context    = vm_task->desc.context;
    vidsch_mgr_t                 *sch_mgr    = adapter->sch_mgr[context->rb_index];
    gpu_device_t                 *device     = context->device;
    task_dma_t task_dma;
    unsigned long long fence_id;

    if(vm_task->desc.dma_cnt == 1 && vm_task->dma_list[0]->command_length == 0)
    {
        /* cmd size == 0, mean paging task is for paging gart memory to system pages,
         * since gart segment memory data already in system pages, so only update allocation info
         */
        vidsch_notify_interrupt(adapter, 0); //render_count update, wait up waiter

        vidschi_set_task_submitted(sch_mgr, &vm_task->desc, 1, 1);

        return;
    }

    zx_memset(&task_dma, 0, sizeof(task_dma_t));
    task_dma.desc.type                = task_type_render;
    task_dma.desc.context             = vm_task->desc.context;
    task_dma.desc.uuid                = vm_task->desc.uuid;
    task_dma.dma_type                 = VM_UPDATE_DMA;
    task_dma.flag.need_hwctx_switch   = FALSE;
    task_dma.desc.need_send_irq       = 1;
    //task_dma.submit_start_offset      = 0;
    //task_dma.submit_end_offset        = ;

    zx_down_read(sch_global->rw_lock);

    if(adapter->ctl_flags.hang_dump)
    {
        zx_mutex_lock(adapter->hang_dump_lock);
    }
    /* lock engine */
    zx_mutex_lock(sch_mgr->engine_lock);

    fence_id = vidschi_inc_send_fence_id(sch_mgr, FALSE);

    vm_task->desc.fence_id = fence_id;
    vidsch_choose_vm_id(context->device, &vm_task->desc);

    task_dma.desc.fence_id = fence_id;
    task_dma.desc.vm_id = vm_task->desc.vm_id;

    if (vm_task->desc.dma_cnt == 1)
    {
        task_dma.dma_buffer_list          = vm_task->dma_list[0];
        task_dma.desc.dma_cnt             = 1;
    }
    else
    {
        task_dma.desc.dma_cnt             = vm_task->desc.dma_cnt;
        task_dma.dma_buffer_list          = zx_malloc(vm_task->desc.dma_cnt * sizeof(dma_node_t));
        for (i = 0; i < task_dma.desc.dma_cnt; i++)
        {
            zx_memcpy(task_dma.dma_buffer_list + i, vm_task->dma_list[i], sizeof(dma_node_t));
        }
    }
    //zx_debug("-----submit vm task:%p, fence_id: %lld, returned_fence: %lld\n", vm_task, fence_id, sch_mgr->returned_fence_id);

    /* call chip submit dma buffer */
    sch_mgr->chip_func->submit(vm_task->desc.context, sch_mgr, &task_dma);

    /* unlock engine */
    zx_mutex_unlock(sch_mgr->engine_lock);

    vidschi_set_task_submitted(sch_mgr, &vm_task->desc, 0, 0);

    if (task_dma.dma_buffer_list != vm_task->dma_list[0])
    {
        zx_free(task_dma.dma_buffer_list);
    }

    if(adapter->ctl_flags.hang_dump)
    {
      zx_mutex_unlock(adapter->hang_dump_lock);
    }
    zx_up_read(sch_global->rw_lock);

#if defined(ZX_VMI_MODE)
    {
        unsigned long long uncompleted_dma = 0;

        do {
            vidschi_notify_fence_interrupt(adapter, -1);
            vidsch_release_completed_tasks(sch_mgr, &uncompleted_dma);
        } while(uncompleted_dma > 0);
    }
#endif
}

void vidsch_submit_page_table_set_task(vidsch_mgr_t *sch_mgr, task_page_table_set_t *page_table_set_task)
{
    adapter_t               *adapter             = sch_mgr->adapter;
    vidsch_global_t         *sch_global          = adapter->sch_global;
    gpu_context_t           *context             = page_table_set_task->desc.context;
    int                     engine_index         = context->rb_index;
    int                     target_engine_index  = page_table_set_task->target_engine_idx;
    task_desc_t             *target_task_desc    = page_table_set_task->target_task_desc;
    gpu_device_t            *target_device       = page_table_set_task->target_device;
    zx_vma_space_t          *target_vma          = target_device->vm;
    zx_vm_id_obj_t          *target_vm_id_obj    = target_device->vm_id;
    vidsch_mgr_t               *target_sch_mgr   = adapter->sch_mgr[target_engine_index];
    unsigned long long      fence_id;

    zx_down_read(sch_global->rw_lock);

    zx_mutex_lock(sch_mgr->engine_lock);

    fence_id = vidschi_inc_send_fence_id(sch_mgr, FALSE);

    vidsch_choose_vm_id(context->device, &page_table_set_task->desc);

    if (target_engine_index == engine_index)
    {
        /* For 3d engine 0,  page table set task jump queue before the target dma task, so need exchange the fence_id */
        page_table_set_task->desc.fence_id = target_task_desc->fence_id;
        target_task_desc->fence_id = fence_id;

        /* due to exchanging two task's fence, vm_id_obj->last_fence needs to be reset */
        vidschi_vm_id_add_sync(adapter, target_vm_id_obj, target_task_desc->fence_id, target_engine_index);
    }
    else
    {
        page_table_set_task->desc.fence_id = fence_id;
    }

    if (target_vm_id_obj->pd_age < target_vma->root_pd_age)
    {
        page_table_set_task->is_set_root_page_table = 1;

        sch_mgr->chip_func->set_root_page_table(target_sch_mgr, target_device, target_vm_id_obj->vm_id, page_table_set_task);
    }
    else
    {
        sch_mgr->chip_func->invalidate_tlb(target_sch_mgr, target_device, target_vm_id_obj->vm_id, page_table_set_task);
    }

    zx_mutex_unlock(sch_mgr->engine_lock);

    vidschi_set_task_submitted(sch_mgr, &page_table_set_task->desc, 0, 1);

    zx_up_read(sch_global->rw_lock);

    vidmm_vm_clear_engine_dirty_range(target_vma, target_engine_index);

    if (IS_VIDEO_ENGINE(adapter->chip_id, target_engine_index))
    {
        target_task_desc->has_prepared = 1;
        zx_wake_up_event(target_sch_mgr->prepare_event);
    }

    return;
}

void vidsch_submit_vm_prepare_task(vidsch_mgr_t *sch_mgr, task_vm_prepare_t *vm_preapre_task)
{
    adapter_t      *adapter          = sch_mgr->adapter;
    task_desc_t    *target_task_desc = vm_preapre_task->target_task_desc;
    vidsch_mgr_t   *target_sch_mgr   = vm_preapre_task->target_sch_mgr;
    task_dma_t     *target_task_dma  = (task_dma_t *) target_task_desc;
    gpu_context_t  *target_context   = target_task_desc->context;
    gpu_device_t   *target_device    = target_context->device;
    int            result            = 0;

    result = vidmm_allocation_list_update_mapping(target_device, target_context, target_task_dma->primary_cnt, target_task_dma->primary_allocation_list);

    if (result == 0)
        result = vidmm_allocation_list_update_mapping(target_device, target_context, target_task_dma->resident_cnt, target_task_dma->resident_allocation_list);

    if (result == 0)
        target_task_dma->flag.has_resident = 1;

    target_task_desc->has_prepared = 1;

    vidschi_set_task_submitted(sch_mgr, &vm_preapre_task->desc, 0, 1);

    zx_wake_up_event(target_sch_mgr->prepare_event);
}
