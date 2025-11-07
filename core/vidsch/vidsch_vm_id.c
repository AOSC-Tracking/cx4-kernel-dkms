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
#include "vidsch_workerthread.h"

static void vidschi_release_page_table_set_task(vidsch_mgr_t *sch_mgr, task_desc_t *desc)
{
    task_page_table_set_t *page_table_set_task  = (task_page_table_set_t*)desc;

    zx_free(page_table_set_task);
}

task_page_table_set_t *vidsch_allocate_page_table_set_task(adapter_t *adapter, gpu_device_t *device, int engine_idx)
{
    gpu_context_t  *reserved_context   = adapter->reserved_context;
    int            reserved_rb_index   = reserved_context->rb_index;
    vidsch_mgr_t   *sch_mgr            = adapter->sch_mgr[reserved_rb_index];
    zx_vma_space_t     *vma_space      = device->vm;
    task_page_table_set_t *page_table_set_task = NULL;
    task_desc_t *desc = NULL;

    page_table_set_task = zx_calloc(sizeof(task_page_table_set_t));
    if(page_table_set_task == NULL)
    {
        zx_error("vidsch_allocate_page_table_set_task not enough memeory\n");
        return NULL;
    }

    page_table_set_task->target_device     = device;
    page_table_set_task->target_engine_idx = engine_idx;

    desc = &page_table_set_task->desc;
    vidsch_task_init_desc(sch_mgr, reserved_context, desc, task_type_page_table_set);

    desc->release = vidschi_release_page_table_set_task;
    desc->vm_id   = 0;

    return page_table_set_task;
}

void vidschi_dump_page_table_set_task(task_page_table_set_t *page_table_set_task, int idx, int dump_detail)
{
    gpu_device_t       *target_device      = page_table_set_task->target_device;
    int                target_engine_index = page_table_set_task->target_engine_idx;
    zx_vm_id_obj_t     *vm_id_obj          = target_device->vm_id;
    int                vm_id               = vm_id_obj->vm_id;

    zx_info("[^^^^  engine %d, Dump Page Table Set Task]: FenceId: %lld.\n",
         idx, page_table_set_task->desc.fence_id);

    if(!dump_detail)  return;

    zx_info("[target info] Device:%p. Process pname:%s. Engine Index: %d. Vm Id: %d. Is set root page table:%d\n",
            target_device, target_device->pname, target_engine_index, vm_id, page_table_set_task->is_set_root_page_table);
}

/**
 * @brief      init vm id manager,
 *             reserve some ids for specific process, see vidsch_reserve_vm_id,
 *             and add other ids to lru list, which will dynamic allocate
 *             for normal use, see vidsch_choose_vm_id
 *
 * @param      adapter  The physical adapter
 */
void vidschi_init_vm_id_mgr(adapter_t *adapter)
{
    vidsch_global_t    *sch_global = adapter->sch_global;
    vidsch_vm_id_mgr_t *vm_id_mgr  = &sch_global->vm_id_mgr;
    zx_vm_id_obj_t    *vm_id_array = NULL;

    vidsch_query_vm_id_t info = {0};
    int i;

    vidsch_query_vm_id(adapter, &info);

    vm_id_mgr->num_ids          = info.total_vm_id_num;
    vm_id_mgr->reserved_num_ids = info.reserved_vm_id_num;

    vm_id_mgr->lock = zx_create_mutex();
    list_init_head(&vm_id_mgr->vm_id_list);

    vm_id_mgr->vm_id_array =
    vm_id_array = zx_calloc(sizeof(zx_vm_id_obj_t) * vm_id_mgr->num_ids);
    if(vm_id_array == NULL)
    {
        zx_error("vidschi_init_vm_id_mgr not enough memory\n");
        return;
    }

    for(i = 0; i < vm_id_mgr->reserved_num_ids; i++)
    {
        vm_id_array[i].vm_id         = i;
    }

    for(i = vm_id_mgr->reserved_num_ids; i < vm_id_mgr->num_ids; i++)
    {
        vm_id_array[i].vm_id = i;
        list_add_tail(&vm_id_array[i].vm_id_node, &vm_id_mgr->vm_id_list);
    }

    if (adapter->chip_id == CHIP_CNE001)
    {
        zx_vma_space_t *vma_space  = adapter->private_vma;
        unsigned int private_vm_id = adapter->private_vm_id;
        zx_vm_id_obj_t *vm_id_obj  = &vm_id_array[private_vm_id];

        vm_id_obj->pd_age = vma_space->root_pd_age;
    }
}

/**
 * @brief      fini vm id mgr
 *
 * @param      adapter  The adapter
 */
void vidschi_fini_vm_id_mgr(adapter_t *adapter)
{
    vidsch_global_t    *sch_global = adapter->sch_global;
    vidsch_vm_id_mgr_t *vm_id_mgr  = &sch_global->vm_id_mgr;
    zx_vm_id_obj_t    *vm_id_obj  = NULL;
    int i, j;

    for(i = 0; i < vm_id_mgr->num_ids; i++)
    {
        vm_id_obj = &vm_id_mgr->vm_id_array[i];

        for(j = 0; j < MAX_ENGINE_COUNT; j++)
        {
            if(zx_test_bit(j, &vm_id_obj->bitmap))
            {
                vidsch_wait_fence_back(adapter, j, vm_id_obj->last_flush[j], TRUE);

                zx_clear_bit(j, &vm_id_obj->bitmap);
            }
        }
    }

    zx_free(vm_id_mgr->vm_id_array);

    vm_id_mgr->vm_id_array = NULL;

    zx_destroy_mutex(vm_id_mgr->lock);
}

/**
 * @brief      add sync object to vm id,
 *             this function should be protected by vidsch_vm_id_mgr_t->lock
 *
 * @param      vm_id_obj   The virtual memory identifier object
 * @param      sync_obj    The synchronize object
 * @param[in]  engine_idx  The engine index
 */
void vidschi_vm_id_add_sync(adapter_t *adapter, zx_vm_id_obj_t *vm_id_obj, unsigned long long fence_id, unsigned int engine_idx)
{
    vm_id_obj->last_flush[engine_idx] = fence_id;

    zx_set_bit(engine_idx, &vm_id_obj->bitmap);
}

/**
 * @brief      check if vm id idle
 *
 * @param      adapter    The adapter
 * @param      vm_id_obj  The virtual memory identifier object
 *
 * @return     TRUE if idle
 */
static int vidschi_is_vm_id_idle(adapter_t *adapter, zx_vm_id_obj_t *vm_id_obj, unsigned int engine_mask)
{
    //note: this function must be in vidsch_vm_id_mgr_t->lock
    int idle = TRUE;
    unsigned int engine_index, bitmap;

    bitmap  = vm_id_obj->bitmap & engine_mask;

    while(bitmap)
    {
        engine_index = util_get_msb_position(bitmap);

        if(vidsch_is_fence_back(adapter, engine_index, vm_id_obj->last_flush[engine_index]))
        {
            zx_clear_bit(engine_index, &bitmap);
            zx_clear_bit(engine_index, &vm_id_obj->bitmap);
        }
        else
        {
            idle = FALSE;
            break;
        }
    }
    return idle;
}

static void vidschi_wait_vm_id_idle(adapter_t *adapter, zx_vm_id_obj_t *vm_id_obj, unsigned int engine_mask)
{
    unsigned int engine_index, bitmap;

    bitmap  = vm_id_obj->bitmap & engine_mask;
    while(bitmap)
    {
        engine_index = util_get_msb_position(bitmap);

        vidsch_wait_fence_back(adapter, engine_index, vm_id_obj->last_flush[engine_index], TRUE);

        zx_clear_bit(engine_index, &bitmap);

        zx_clear_bit(engine_index, &vm_id_obj->bitmap);
    }
}

static zx_vm_id_obj_t* vidschi_choose_vm_id_004(adapter_t *adapter, vidsch_vm_id_mgr_t *vm_id_mgr, struct gpu_device *device)
{
    zx_vm_id_obj_t    *vm_id_obj   = NULL, *temp = NULL;
    zx_vma_space_t     *vma_space  = device->vm;

    if (device == adapter->reserved_device)
    {
        vm_id_obj = &vm_id_mgr->vm_id_array[0];
    }
    else if (device->vm_id && device->vm_id->device_uuid == device->uuid)
    {
        vm_id_obj = device->vm_id;
    }

    while (!vm_id_obj)
    {
        list_for_each_entry(temp, &vm_id_mgr->vm_id_list, vm_id_node)
        {
            if(vidschi_is_vm_id_idle(adapter, temp, -1))
            {
                vm_id_obj = temp;
                break;
            }
        }

        if (!vm_id_obj)
        {
            zx_warning("not found a avalable vm id, retry\n");

            vidschi_wait_vm_id_idle(adapter, list_first_entry(&vm_id_mgr->vm_id_list, zx_vm_id_obj_t, vm_id_node), -1);
        }
    }

    if (vm_id_obj != &vm_id_mgr->vm_id_array[0])
    {
        // move to tail
        list_del(&vm_id_obj->vm_id_node);
        list_add_tail(&vm_id_obj->vm_id_node, &vm_id_mgr->vm_id_list);
    }

    if (device->vm_id != vm_id_obj || vm_id_obj->device_uuid != device->uuid)
    {
        device->vm_id = vm_id_obj;
        vm_id_obj->device_uuid = device->uuid;
        vm_id_obj->pd_age = 0;
    }

    return vm_id_obj;
}

static zx_vm_id_obj_t* vidschi_choose_vm_id_cne001(adapter_t *adapter, vidsch_vm_id_mgr_t *vm_id_mgr, struct gpu_device *device)
{
    zx_vm_id_obj_t    *vm_id_obj   = NULL;
    zx_vma_space_t     *vma_space  = device->vm;
    unsigned int    private_vm_id  = adapter->private_vm_id;

    if (device == adapter->reserved_device)
    {
        vm_id_obj = &vm_id_mgr->vm_id_array[0];
    }
    else if (vma_space == adapter->private_vma)
    {
        vm_id_obj =&vm_id_mgr->vm_id_array[private_vm_id];
    }
    else
    {
        zx_assert(0, "invalid in choose_vm_Id");
    }

    device->vm_id = vm_id_obj;

    return vm_id_obj;
}

void vidsch_choose_vm_id(struct gpu_device *device, task_desc_t *task_desc)
{
    zx_vma_space_t     *vma_space  = device->vm;
    gpu_context_t      *context    = task_desc->context;
    adapter_t          *adapter    = device->adapter;
    vidsch_mgr_t       *sch_mgr    = adapter->sch_mgr[context->rb_index];
    vidsch_global_t    *sch_global = adapter->sch_global;
    vidsch_vm_id_mgr_t *vm_id_mgr  = &sch_global->vm_id_mgr;
    zx_vm_id_obj_t    *vm_id_obj   = NULL;
    unsigned int       engine_idx  = context->rb_index;

    zx_mutex_lock(vm_id_mgr->lock);

    if (adapter->chip_id == CHIP_CHX004)
    {
        vm_id_obj = vidschi_choose_vm_id_004(adapter, vm_id_mgr, device);
    }
    else if (adapter->chip_id == CHIP_CNE001)
    {
        vm_id_obj = device->vm_id ? device->vm_id : vidschi_choose_vm_id_cne001(adapter, vm_id_mgr, device);
    }

    task_desc->vm_id = vm_id_obj->vm_id;
    /* For paging, vm update and page table set task, this is no use, just for vidschi_fini_vm_id_mgr wait */
    vidschi_vm_id_add_sync(adapter, vm_id_obj, task_desc->fence_id, engine_idx);

    zx_mutex_unlock(vm_id_mgr->lock);

    if (device != adapter->reserved_device)
    {
        task_page_table_set_t *page_table_set_task = NULL;
        unsigned int  utlb_flag = vma_space->utlb_dirtys[engine_idx].dirty;
        unsigned int  dtlb_flag = vma_space->dtlb_dirtys[engine_idx].dirty;

        if (vm_id_obj->pd_age >= vma_space->root_pd_age && !dtlb_flag && !utlb_flag)
        {
            return;
        }

        page_table_set_task = vidsch_allocate_page_table_set_task(adapter, device, engine_idx);
        page_table_set_task->target_task_desc = task_desc;

        if (IS_VIDEO_ENGINE(adapter->chip_id, context->rb_index))
        {
            gpu_context_t  *reserved_context   = adapter->reserved_context;

            task_desc->has_prepared = 0;

            vidschi_add_task_to_page_table_set_queue(adapter->sch_mgr[reserved_context->rb_index], &page_table_set_task->desc);

            vidsch_wait_prepare_event(sch_mgr, task_desc, 1, page_table_set_task->desc.type);

            vidsch_wait_fence_back(adapter, reserved_context->rb_index, page_table_set_task->desc.fence_id, TRUE);

            vidsch_task_dec_reference(&page_table_set_task->desc);
        }
        else
        {
            vidsch_submit_page_table_set_task(sch_mgr, page_table_set_task);

            vidsch_task_dec_reference(&page_table_set_task->desc);
        }
    }

    /* due to video ap having multiple engine in vma_space , it is necessary that
        vm_id_obj->pd_age = vma_space->root_pd_age after page_table_task_set's fence back */
    if (vm_id_obj->pd_age < vma_space->root_pd_age)
    {
        vm_id_obj->pd_age = vma_space->root_pd_age;
    }
}

