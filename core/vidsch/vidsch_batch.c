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
#include "kernel_interface.h"
#include "context.h"
#include "vidsch.h"
#include "vidschi.h"
#include "vidsch_render.h"
#include "vidmm.h"
#include "vidsch_workerthread.h"


inline vidmm_allocation_t *vidschi_locate_dma_buffer(task_dma_t *dma_task, unsigned long long gpu_va, unsigned long long size, unsigned long long *offset)
{
    gpu_device_t *device = dma_task->desc.context->device;

    vidmm_allocation_t *allocation = vidmm_fetch_allocation_by_address(device,
            dma_task->primary_allocation_list, dma_task->primary_cnt, gpu_va, size, offset, NULL);

    if (!allocation)
    {
        allocation = vidmm_fetch_allocation_by_address(device,
                dma_task->resident_allocation_list, dma_task->resident_cnt, gpu_va, size, offset, NULL);
    }

    if (allocation && !allocation->flag.cpu_map)
    {
        allocation->flag.cpu_map = 1;

        vidmm_lock_allocation(device, allocation);
    }

    return allocation;
}

/*
return the dma type of the dma batch. we suppose the cmd list transfer from umd owns the same cmd type.
*/

static DMA_TYPE vidschi_get_dma_type(vidsch_batch_parser_t *parser)
{
    DMA_TYPE       dma_type;

    if (parser->submit_flag._3dblt_cmd)
    {
        dma_type = DRAW_3D_BLT;
    }
    else if(parser->submit_flag.contain_lpdp_cmd)
    {
        dma_type = LPDP_DMA;
    }
    else if (parser->submit_flag._2dcmd)
    {
        dma_type = DRAW_2D_DMA;
    }
    else if (parser->submit_flag.clcs_only)
    {
        dma_type = OCL_DMA;
        zx_info("send OCL DMA\n");
    }
    else
    {
        dma_type = DRAW_3D_DMA;
    }

    return dma_type;
}

/**
 * @brief      bind dma as the owner of signal task,
 *             need_sginal flag indicates if need dma task signal the fence
 *
 * @param      context  The gpu context
 * @param      parser   The parser
 */
static int vidschi_parse_dma_and_signal_task(struct gpu_context *context,  vidsch_batch_parser_t *parser)
{
    gpu_device_t               *device   = context->device;
    adapter_t                  *adapter  = device->adapter;
    vidsch_mgr_t               *sch_mgr  = adapter->sch_mgr[context->rb_index];
    task_dma_t                 *dma_task = NULL;
    vidsch_allocate_dma_task_t arg       = {0};
    unsigned int                total_cmd_size = 0;
    int i, ret = S_OK;

    //TODO: handle the cmd_header per dma
    arg.dma_type        = parser->dma_cnt ? vidschi_get_dma_type(parser) : DRAW_3D_DMA;
    arg.dma_cnt         = parser->dma_cnt;
    arg.primary_cnt     = parser->primary_cnt;

    dma_task = vidsch_allocate_dma_task(context, &arg);

    if(dma_task == NULL)
    {
        zx_error("parse dma task fail\n");
        return E_OUTOFMEMORY;
    }

    dma_task->private_data = parser;

    dma_task->resident_cnt             = parser->resident_cnt;
    dma_task->resident_allocation_list = (void*)parser->resident_list;

    for(i = 0; i < parser->primary_cnt; i++)
    {
        vidmm_allocation_t *allocation = parser->primary_list[i];
        zx_assert(allocation != NULL, "");

        dma_task->primary_allocation_list[i] = allocation;

        zx_counter_trace_event(parser->primary_write_op_list[i] ? "var_write_allocation" : "var_read_allocation", allocation->debug_gid);
    }

    for(i = 0; i < parser->dma_cnt; i++)
    {
        dma_task->dma_buffer_list[i].gpu_virtual_address = parser->dma_gpu_va[i];
        dma_task->dma_buffer_list[i].command_length      = parser->dma_length[i];

        total_cmd_size += parser->dma_length[i];
    }

    if (IS_VIDEO_ENGINE(adapter->chip_id, context->rb_index))
    {
        for(i = 0; i < parser->dma_cnt; i++)
        {
            unsigned long long offset = 0;
            vidmm_allocation_t *dma_alloc = vidschi_locate_dma_buffer(dma_task,
                    parser->dma_gpu_va[i], parser->dma_length[i], &offset);

            dma_task->dma_buffer_list[i].cpu_virtual_address = (char*)dma_alloc->krnl_cpu_vma->virt_addr + offset;
        }
    }

    dma_task->desc.cmd_size = total_cmd_size;

    dma_task->flag.null_rendering = parser->submit_flag.null_rendering;
    dma_task->flag.dump_rb = parser->submit_flag.dump_rb;
    dma_task->flag.dump_dma = parser->submit_flag.dump_dma;
    dma_task->flag.ignore_dma = parser->submit_flag.ignore_dma;
    dma_task->flag.initialize_context = parser->submit_flag.initialize_context;

    if(dma_task->flag.need_hwctx_switch)
    {
        if (parser->ctx_buf_va)
        {
            dma_task->ctx_buf_va = parser->ctx_buf_va;
        }
        else
        {
            //zx_error("*** need_ctx_switch but no ctx_buf_va set.\n");
            dma_task->flag.need_hwctx_switch = 0;
        }
    }

    if(sch_mgr->chip_func->render)
    {
        sch_mgr->chip_func->render(context, dma_task);
    }

    dma_task->desc.dma_fence = parser->dma_fence;
    dma_task->wait_dma_fence = parser->wait_dma_fence;

    parser->dma_fence = NULL;
    parser->wait_dma_fence = NULL;

    ret = vidschi_handle_dma_task(context, dma_task);

    /*Correspond to inc refs in allocate dma task*/
    vidsch_task_dec_reference(&dma_task->desc);

    return ret;
}

/**
 * @brief      a task package may include 3 types tasks: wait task, render task and signal task.
 *             this function will generate related tasks one by one to the same context.
 *             first need do wait tasks, then do render task with all dma,
 *             and if has signal task, render task is the signal owner of the signal task
 *
 * @param      context  gpu context
 * @param      parser   batch parser
 *
 * @return     0 if suceess
 */
int  vidsch_batch_parser(struct gpu_context *context, vidsch_batch_parser_t *parser)
{
    int ret = S_OK;
    int i;

    ret = vidschi_parse_dma_and_signal_task(context, parser);

    if(ret != S_OK)
    {
        zx_error("parse dma task fail, ret:%d\n", ret);
        return ret;
    }

    return ret;
}
