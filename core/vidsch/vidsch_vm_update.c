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

static void vidschi_release_vm_update_task(vidsch_mgr_t *sch_mgr, task_desc_t *desc)
{
    int i;
    adapter_t           *adapter = sch_mgr->adapter;
    task_vm_update_t    *vm_task = (task_vm_update_t*)desc;

    for (i = 0; i < vm_task->desc.dma_cnt; i++)
    {
        if (vm_task->dma_list[i])
        {
            vidschi_release_dma_node(sch_mgr, vm_task->dma_list[i]);

            vm_task->dma_list[i] = NULL;
        }
    }

    if (vm_task->dma_list != &vm_task->reserved[0])
    {
        zx_free(vm_task->dma_list);
    }
    zx_free(vm_task);
}

task_vm_update_t*  vidsch_allocate_vm_update_task(struct gpu_context *context, vidsch_allocate_vm_update_task_t *arg)
{
    adapter_t         *adapter = context->device->adapter;
    vidsch_mgr_t      *sch_mgr = adapter->sch_mgr[context->rb_index];
    task_vm_update_t  *vm_task = NULL;
    task_desc_t       *desc    = NULL;

    vm_task = (task_vm_update_t *)zx_calloc(sizeof(task_vm_update_t));
    vm_task->dma_size = arg->dma_size;
    vm_task->dma_list = &vm_task->reserved[0];
    vm_task->dma_list_cnt = sizeof(vm_task->reserved) / sizeof(vm_task->reserved[0]);
    vm_task->dma_list[0] = vidschi_allocate_dma_node(sch_mgr, arg->dma_size);

    desc = &vm_task->desc;
    vidsch_task_init_desc(sch_mgr, context, desc, task_type_vm_update);
    desc->dma_cnt       = 1;
    desc->dma_type = VM_UPDATE_DMA;
    desc->cmd_size      = 0;
    desc->release       = vidschi_release_vm_update_task;

    return vm_task;
}

void* vidsch_vm_update_task_request_space(task_vm_update_t *vm_task, unsigned int size)
{
    gpu_context_t *context = vm_task->desc.context;
    adapter_t *adapter = context->device->adapter;
    vidsch_mgr_t *sch_mgr = adapter->sch_mgr[context->rb_index];
    dma_node_t *dma = vm_task->dma_list[vm_task->desc.dma_cnt - 1];

    if (dma->size - dma->command_length < size)
    {
        // request a new dma node
        if (vm_task->desc.dma_cnt == vm_task->dma_list_cnt)
        {
            // alloc a new dma_node_t array
            dma_node_t **dma_node_array = zx_calloc(vm_task->dma_list_cnt * 2 * sizeof(dma_node_t*));
            zx_memcpy(dma_node_array, vm_task->dma_list, vm_task->dma_list_cnt * sizeof(dma_node_t*));

            if (vm_task->dma_list != &vm_task->reserved[0])
            {
                zx_free(vm_task->dma_list);
            }

            vm_task->dma_list = dma_node_array;
            vm_task->dma_list_cnt *= 2;
        }
        ++vm_task->desc.dma_cnt;
        if (size < vm_task->dma_list[0]->size)
        {
            size = vm_task->dma_list[0]->size;
        }
        else
        {
            size = (size + adapter->os_page_size - 1) & ~(adapter->os_page_size - 1);
        }
        dma = vidschi_allocate_dma_node(sch_mgr, size);
        vm_task->dma_list[vm_task->desc.dma_cnt - 1] = dma;
        vm_task->dma_size += size;
    }

    return dma->cpu_virtual_address + dma->command_length;
}

void vidsch_vm_update_task_release_space(task_vm_update_t *vm_task, void *cmd_buf)
{
    dma_node_t *dma = vm_task->dma_list[vm_task->desc.dma_cnt - 1];

    zx_assert(dma != NULL, "invalid dma");
    zx_assert((unsigned char*)cmd_buf >= dma->cpu_virtual_address &&
              (unsigned char*)cmd_buf < dma->cpu_virtual_address + dma->size,
              "dma override");

    vm_task->desc.cmd_size += ((unsigned char*)cmd_buf - dma->cpu_virtual_address) - dma->command_length;

    dma->command_length = (unsigned char*)cmd_buf - dma->cpu_virtual_address;
}


void vidschi_dump_vm_update_task(task_vm_update_t *vm_task, int idx, int dump_detail)
{
    int i;
    zx_info("[^^^^  engine %d, Dump VM Update Task]: FenceId: %lld.\n",
         idx, vm_task->desc.fence_id);

    if(!dump_detail)  return;

    zx_msleep(20);

    for (i = 0; i < vm_task->desc.dma_cnt; i++)
    {
        if(vm_task->dma_list[i] != NULL)
        {
            util_dump_memory(vm_task->dma_list[i]->cpu_virtual_address, vm_task->dma_list[i]->command_length, "Dma");
        }
    }
    zx_msleep(10);
}


static void vidschi_release_vm_prepare_task(vidsch_mgr_t *sch_mgr, task_desc_t *desc)
{
    task_vm_prepare_t *vm_prepare_task  = (task_vm_prepare_t*)desc;

    zx_free(vm_prepare_task);
}

static task_vm_prepare_t *vidsch_allocate_vm_prepare_task(vidsch_mgr_t *target_sch_mgr, void *target_task)
{
    task_vm_prepare_t *vm_prepare_task = NULL;
    adapter_t   *adapter = target_sch_mgr->adapter;
    task_desc_t *desc = NULL;
    gpu_context_t  *reserved_context   = adapter->reserved_context;
    int            reserved_rb_index   = reserved_context->rb_index;
    vidsch_mgr_t   *sch_mgr            = adapter->sch_mgr[reserved_rb_index];
    vm_prepare_task = zx_calloc(sizeof(task_vm_prepare_t));
    if(vm_prepare_task == NULL)
    {
        zx_error("vidsch_allocate_vm_prepare_task not enough memeory\n");
        return NULL;
    }

    vm_prepare_task->target_task_desc     = target_task;
    vm_prepare_task->target_sch_mgr       = target_sch_mgr;

    desc = &vm_prepare_task->desc;
    vidsch_task_init_desc(sch_mgr, reserved_context, desc, task_type_vm_prepare);

    desc->release = vidschi_release_vm_prepare_task;
    desc->vm_id   = 0;

    return vm_prepare_task;
}

static int vidschi_is_prepare_finished(void *argu)
{
    task_desc_t * task_desc = argu;

    return task_desc->has_prepared == 1;
}

int vidsch_wait_prepare_event(vidsch_mgr_t *sch_mgr, task_desc_t *task_desc, unsigned int force_wait, task_type_t wait_task_type)
{
    condition_func_t condition = vidschi_is_prepare_finished;
    zx_event_status_t e_status = ZX_EVENT_UNKNOWN;
    int             timeout    = 100; // 100ms
    int           timeout_cnt = 0;

    do
    {
        e_status = zx_wait_event_thread_safe(sch_mgr->prepare_event, condition, task_desc, timeout);

        if (e_status == ZX_EVENT_TIMEOUT)
        {
            timeout_cnt++;

            /* use wait_task_type to distinguish between vm_prepare_task and page_table_set_task */
            if (wait_task_type == task_type_vm_prepare)
                zx_debug("sch_mgr->engine_index %d is waitting for vm_prepare_task more than %d x 100 ms\n", sch_mgr->engine_index, timeout_cnt);
            else if (wait_task_type == task_type_page_table_set)
                zx_debug("sch_mgr->engine_index %d is waitting for page_table_set_task more than %d x 100 ms\n", sch_mgr->engine_index, timeout_cnt);
        }

    }while(force_wait && e_status != ZX_EVENT_BACK);

    return e_status;
}

/* prepare dma, paging in all allocations in the device which the task context bind to
 * in video engine, releated work(prepare dma and update alloation) dispatch 3d engine
 * if allocation_list_update_mapping return S_OK in primary_allocation_list and resident_allocation_list
 *  set dma_task->flag.has_resident = 1
 */
void vidsch_prepare_dma_task(task_dma_t *dma_task)
{
    int       result                   = 0;
    task_vm_prepare_t *vm_prepare_task = NULL;
    gpu_context_t  *context            = dma_task->desc.context;
    gpu_device_t   *device             = context->device;
    adapter_t *adapter                 = device->adapter;
    gpu_context_t  *reserved_context   = adapter->reserved_context;

    if (IS_VIDEO_ENGINE(adapter->chip_id, context->rb_index))
    {
        dma_task->desc.has_prepared = 0;
        vm_prepare_task = vidsch_allocate_vm_prepare_task(adapter->sch_mgr[context->rb_index], dma_task);

        vidschi_add_task_to_page_table_set_queue(adapter->sch_mgr[reserved_context->rb_index], &vm_prepare_task->desc);

        vidsch_wait_prepare_event(adapter->sch_mgr[context->rb_index], &dma_task->desc, 1, vm_prepare_task->desc.type);

        vidsch_task_dec_reference(&vm_prepare_task->desc);
    }
    else
    {
        result = vidmm_allocation_list_update_mapping(device, context, dma_task->primary_cnt, dma_task->primary_allocation_list);

        if (result == 0)
            result = vidmm_allocation_list_update_mapping(device, context, dma_task->resident_cnt, dma_task->resident_allocation_list);

        if (result == 0)
            dma_task->flag.has_resident = 1;
    }
}
