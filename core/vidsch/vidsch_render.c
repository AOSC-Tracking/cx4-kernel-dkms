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
#include "context.h"
#include "vidsch.h"
#include "vidschi.h"
#include "vidsch_workerthread.h"
#include "vidsch_submit.h"
#include "vidmm.h"

void vidsch_dma_sync_object_trigger(void *arg)
{
    vidsch_mgr_t *sch_mgr = arg;

    util_wakeup_event_thread(sch_mgr->worker_thread);
}

//when task submit
void vidschi_update_and_release_dma_fence(task_desc_t *task)
{
    if (task->dma_fence)
    {
        zx_dma_fence_update_value(task->dma_fence, task->fence_id);

        task->need_send_irq = zx_dma_fence_need_send_irq(task->dma_fence,task->fence_id);

        zx_dma_fence_release_cb(task->dma_fence);

        task->dma_fence = NULL;
    }
}

//when fence int happen
void vidschi_notify_dma_fence(adapter_t *adapter)
{
    zx_dma_fence_notify_event(adapter->os_device.pdev);
}

static void vidschi_release_dma_task(vidsch_mgr_t *vidsch_mgr, task_desc_t *desc)
{
    int i;
    task_dma_t *dma_task = (task_dma_t*)desc;
    vidsch_batch_parser_t *parser = dma_task->private_data;
    struct gpu_context *context =desc->context;

    if (dma_task->flag.has_resident)
    {
        for(i = 0; i < dma_task->primary_cnt; i++)
        {
            vidmm_mark_pagable(dma_task->primary_allocation_list[i]);

            dma_task->primary_allocation_list[i] = NULL;
        }

        for(i = 0; i < dma_task->resident_cnt; i++)
        {
            vidmm_mark_pagable(dma_task->resident_allocation_list[i]);
        }
    }

    if (parser && parser->release)
    {
        parser->release(parser);
    }

    zx_free(dma_task);
}


/**
 * @brief      allocate dma task struct, still need to fill some members manually
 *
 * @param      context  The context
 * @param      arg      The argument
 *
 * @return     { NULL if fail }
 */
task_dma_t * vidsch_allocate_dma_task(struct gpu_context *context, vidsch_allocate_dma_task_t *arg)
{
    vidsch_mgr_t *sch_mgr              = NULL;
    task_dma_t   *dma_task             = NULL;
    task_desc_t  *desc                 = NULL;
    unsigned int dma_size              = arg->dma_cnt * sizeof(dma_node_t);
    unsigned int primary_size          = arg->primary_cnt * sizeof(vidmm_allocation_t*);
    unsigned int offset                = 0;
    char        *memory                = NULL;
    adapter_t   *adapter               = NULL;

    unsigned int total_size = util_align(sizeof(task_dma_t), 16) +
                              util_align(dma_size, 16) +
                              util_align(primary_size, 16);

    if(context == NULL)
    {
        zx_error("vidsch_allocate_dma_task, invalid context\n");
        return NULL;
    }

    memory = zx_calloc(total_size);

    if(memory == NULL)
    {
        zx_error("func vidsch_allocate_dma_task allocated memory failed, size:%x.\n", total_size);
        return NULL;
    }

    sch_mgr = context->device->adapter->sch_mgr[context->rb_index];
    adapter = sch_mgr->adapter;

    dma_task = (task_dma_t *)(memory + offset);

    offset += util_align(sizeof(task_dma_t), 16);

    if(arg->dma_cnt)
    {
        dma_task->dma_buffer_list      = (dma_node_t *)(memory + offset);
        offset += util_align(dma_size, 16);
    }

    if(arg->primary_cnt)
    {
        dma_task->primary_cnt             = arg->primary_cnt;
        dma_task->primary_allocation_list = (vidmm_allocation_t **)(memory + offset);
        offset += util_align(primary_size, 16);
    }

    zx_assert(offset == total_size, "offset:%d, total_size:%d", offset, total_size);

    dma_task->dma_type               = arg->dma_type;
    dma_task->hw_ctx_info            = NULL;
    dma_task->flag.need_hwctx_switch = (arg->dma_type == DRAW_3D_DMA) ? TRUE : FALSE;//todo: need confirm for vulkan.

    /* if hw engine no hw ctx, then no need hwctx swicth */
    if(sch_mgr->engine_caps & ENGINE_CAPS_NO_HWCTX)
    {
        dma_task->flag.need_hwctx_switch = FALSE;
    }

    if(context->flag.client == CLIENT_TYPE_VULKAN)
    {
        dma_task->flag.need_hwctx_switch = FALSE;
    }

    desc = &dma_task->desc;
    vidsch_task_init_desc(sch_mgr, context, desc, task_type_render);
    desc->dma_cnt     = arg->dma_cnt;
    desc->dma_type  = dma_task->dma_type;

    dma_task->desc.release = vidschi_release_dma_task;

    return dma_task;
}

/**
 * @brief      check if can prepre to submit dma task
 *
 * @param      sch_mgr   The sch manager
 * @param      dma_task  The dma task
 *
 * @return     { description_of_the_return_value }
 */
int vidschi_can_prepare_dma_task(vidsch_mgr_t *sch_mgr, task_dma_t *task)
{
    int can = 1;
    adapter_t *adapter = sch_mgr->adapter;

    zx_spin_lock_bh(sch_mgr->fence_lock);

    sch_mgr->returned_fence_id = sch_mgr->chip_func->update_fence_id(sch_mgr);
    sch_mgr->uncomplete_task_num = sch_mgr->last_send_fence_id - sch_mgr->returned_fence_id;
    if (sch_mgr->uncomplete_task_num < sch_mgr->hw_queue_size)
    {
        if(task->wait_dma_fence){
            can = zx_dma_fence_is_signaled(task->wait_dma_fence);
            if(can){
                zx_dma_fence_release_cb(task->wait_dma_fence);//correspond to batch_parser_t.wait_dma_fence init value dma_fence_get
                task->wait_dma_fence = NULL;
            }
        }
    }
    else
    {
        can = 0;
    }

    zx_spin_unlock_bh(sch_mgr->fence_lock);
    // zx_debug("%s. sch_mgr->last_send_fence_id: %lld, sch_mgr->returned_fence_id: %lld\n", __func__, sch_mgr->last_send_fence_id, sch_mgr->returned_fence_id);
    // zx_debug("%s. sch_mgr->uncomplete_task_num: %d,  sch_mgr->prepare_task_num: %d, hw_queue_size: %d\n", __func__, sch_mgr->uncomplete_task_num, sch_mgr->prepare_task_num, sch_mgr->hw_queue_size);
    return can;
}

int vidschi_handle_dma_task(struct gpu_context *context, task_dma_t *dma_task)
{
    adapter_t    *adapter  = context->device->adapter;
    vidsch_mgr_t *sch_mgr  = adapter->sch_mgr[context->rb_index];
    int ret = S_OK;

    /* if task have hwctx, need check if hwctx invalid, since previous task may submit by worker thread,
     * and thread may encounter problem, like paging failed, which lead task dropped. since hwctx switch
     * task may depend on previous task, so we set hwctx invalid if drop task. and skip all follow task based on this hwctx
     */
    if((dma_task->hw_ctx_info != NULL) && dma_task->hw_ctx_info->is_invalid)
    {
        ret =  E_FAIL;
    }

#ifdef ZX_VMI_MODE
    vidschi_prepare_and_submit_dma(sch_mgr, dma_task);
#else
    vidschi_add_task_to_pending_queue(sch_mgr, &dma_task->desc);
#endif

    zx_wmb();

    return ret;
}


void vidschi_dump_dma_task(task_dma_t *dma_task, int idx, int dump_detail)
{
    vidmm_allocation_t *allocation;
    task_desc_t *desc = &dma_task->desc;
    dma_node_t  *dma  = NULL;
    void *cpu_addr;
    int i = 0;
    void *drm_sync_obj = NULL;

    zx_info("[^^^^  engine %d, Dump Render Task(detail: %d)]\n", idx, dump_detail);

    vidschi_dump_general_task_info(desc);

    zx_info("DMAType: %d, FenceID: %lld, dma_cnt:%d. client: %d.\n",
            dma_task->dma_type, desc->fence_id, desc->dma_cnt, desc->context->flag.client);

    if(!dump_detail) return;

    zx_msleep(20);

    for(i = 0; i < dma_task->primary_cnt; i++)
    {
        allocation = dma_task->primary_allocation_list[i];

        if(!allocation) continue;

        zx_info("allocation size 0x%llx, segment id %d\n", allocation->size, allocation->segment_id);

    }


    for(i = 0; i < desc->dma_cnt; i++)
    {
        dma = &dma_task->dma_buffer_list[i];

        if(dma->cpu_virtual_address)
        {
            cpu_addr = dma->cpu_virtual_address;

            zx_info("dma_task:%p, dma index:%d, dma_node:%p, dma cpu_va 0x%llx\n", dma_task, i, dma, cpu_addr);

            util_dump_memory(cpu_addr, dma->command_length, "Dma");
        }
    }
}
