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

static void vidschi_release_paging_task(vidsch_mgr_t *sch_mgr, task_desc_t *desc)
{
    task_paging_t *paging_task  = (task_paging_t*)desc;
    adapter_t     *adapter      = sch_mgr->adapter;

    vidschi_release_dma_node(sch_mgr, paging_task->dma);

    vidmm_release_temp_paging_allocation(adapter, paging_task);

    zx_free(paging_task);
}

task_paging_t * vidsch_allocate_paging_task(gpu_context_t *context, vidsch_allocate_paging_task_t *arg)
{
    adapter_t      *adapter     = context->device->adapter;
    vidsch_mgr_t   *sch_mgr     = adapter->sch_mgr[context->rb_index];
    task_paging_t  *paging_task = NULL;
    task_desc_t    *desc        = NULL;
    char           *memory      = NULL;
    unsigned int   offset       = 0;

    unsigned int total_size = util_align(sizeof(task_paging_t), 16) +
                              util_align(sizeof(vidmm_paging_allocation_t) * arg->allocation_num, 16);

    memory = zx_calloc(total_size);

    if(memory == NULL)
    {
        zx_error("vidsch_allocate_paging_task not enough memeory\n");
        return NULL;
    }

    paging_task = (task_paging_t *)(memory + offset);
    offset     += util_align(sizeof(task_paging_t), 16);

    if(arg->allocation_num)
    {
        paging_task->paging_allocation_list      = (vidmm_paging_allocation_t *)(memory + offset);
        paging_task->paging_allocation_list_size = arg->allocation_num;
        offset += util_align(sizeof(vidmm_paging_allocation_t) * arg->allocation_num, 16);
    }

    paging_task->dma = vidschi_allocate_dma_node(sch_mgr, arg->dma_size);

    desc = &paging_task->desc;

    vidsch_task_init_desc(sch_mgr, context, desc, task_type_paging);
    desc->cmd_size    = arg->dma_size;
    desc->dma_type  = PAGING_DMA;

    desc->release = vidschi_release_paging_task;

    return paging_task;
}

int vidschi_handle_paging_task(struct gpu_context *context, task_paging_t *paging_task)
{
    adapter_t      *adapter     = context->device->adapter;

    vidsch_submit_paging_task(adapter, paging_task);

    return S_OK;
}


void vidschi_dump_paging_task(task_paging_t *paging, int idx, int dump_detail)
{
    vidmm_allocation_t        *allocation        = NULL;
    vidmm_paging_allocation_t *paging_allocation = NULL;

    int i = 0;

    zx_info("[^^^^  engine %d, Dump Paging Task] Paging Type: %d, AllocationNum: %d, FenceId: %lld.\n",
         idx, paging->flag, paging->paging_allocation_list_size, paging->desc.fence_id);

    if(!dump_detail)  return;

    for(i = 0; i < paging->paging_allocation_list_size; i++)
    {
        paging_allocation = &paging->paging_allocation_list[i];
        allocation        = paging_allocation->allocation;

        if(allocation == NULL) continue;

        vidschi_dump_allocation(i, allocation);
    }

    zx_msleep(20);

    if(paging->dma != NULL)
    {
        util_dump_memory(paging->dma->cpu_virtual_address, paging->dma->command_length, "Dma");

        zx_msleep(10);
    }
}
