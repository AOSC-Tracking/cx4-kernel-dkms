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
#include "vidmm.h"
#include "vidsch.h"
#include "vidmmi.h"
#include "vidschi.h"
#include "vidsch_submit.h"

void vidmm_add_allocation_to_segment_resident_list(adapter_t *adapter, vidmm_allocation_t *allocation)
{
    vidmm_mgr_t      *mm_mgr  = adapter->mm_mgr;
    vidmm_segment_t  *segment = &mm_mgr->segment[allocation->segment_id];

    zx_mutex_lock(segment->lock);

    if (allocation->flag.unpagable)
    {
        list_add_tail(&allocation->list_item, &segment->unpagable_resident_list);
    }
    else
    {
        list_add_tail(&allocation->list_item, &segment->pagable_resident_list[allocation->priority]);
    }
    zx_mutex_unlock(segment->lock);
}

void vidmm_remove_allocation_from_segment_resident_list(adapter_t *adapter, vidmm_allocation_t *allocation)
{
    vidmm_mgr_t      *mm_mgr  = adapter->mm_mgr;
    vidmm_segment_t  *segment = &mm_mgr->segment[allocation->segment_id];

    zx_mutex_lock(segment->lock);

    list_del(&allocation->list_item);

    zx_mutex_unlock(segment->lock);
}

static inline void vidmm_add_allocation_to_cache_list(adapter_t *adapter, vidmm_allocation_t *allocation)
{
    zx_assert(allocation->segment_id == 0, "");

    vidmm_add_allocation_to_segment_resident_list(adapter, allocation);
}

static inline void vidmm_remove_allocation_from_cache_list(adapter_t *adapter, vidmm_allocation_t *allocation)
{
    zx_assert(allocation->segment_id == 0, "");

    vidmm_remove_allocation_from_segment_resident_list(adapter, allocation);
}

int vidmmi_copy_data(vidmm_mgr_t *mm_mgr, task_paging_t *paging_task, vidmm_paging_allocation_t *paging_allocation)
{
    int status = S_OK;
    vidmm_allocation_t *src_allocation = paging_allocation->temp_allocation_src;
    vidmm_allocation_t *dst_allocation = paging_allocation->temp_allocation_dst;
    vidmm_private_build_paging_buffer_arg_t build_paging_buffer = {0, };
    unsigned long long src_addr = paging_allocation->src_gpu_va;
    unsigned long long dst_addr = paging_allocation->dst_gpu_va;

    build_paging_buffer.operation              = BUILDING_PAGING_OPERATION_TRANSFER;
    build_paging_buffer.dma_buffer             = paging_task->dma->cpu_virtual_address;
    build_paging_buffer.dma_size               = paging_task->dma->size;
    build_paging_buffer.multi_pass_offset      = 0;
    build_paging_buffer.transfer.transfer_offset    = 0;
    build_paging_buffer.transfer.transfer_size = dst_allocation->size;
    build_paging_buffer.transfer.src.phy_addr  = src_addr;
    build_paging_buffer.transfer.dst.phy_addr  = dst_addr;
    build_paging_buffer.allocation             = dst_allocation;

    build_paging_buffer.transfer.src.compress_format  = src_allocation->compress_format;
    build_paging_buffer.transfer.dst.compress_format  = dst_allocation->compress_format;

    status = mm_mgr->chip_func->build_paging_buffer(mm_mgr->adapter, &build_paging_buffer);

    if (status == S_OK)
    {
        vidsch_update_dma_command_length(paging_task->dma, (char*)build_paging_buffer.dma_buffer - (char*)paging_task->dma->cpu_virtual_address);
    }
    else
    {
        zx_info("Paging DMA size not enough: size: %d, current_size: %d.\n",
            paging_task->dma->size, util_get_ptr_span(build_paging_buffer.dma_buffer, paging_task->dma->cpu_virtual_address));
    }

    return status;
}

// create and map the temp allocation to the paging vma space.
static vidmm_allocation_t * vidmmi_create_temp_paging_allocation(vidmm_mgr_t *mm_mgr, gpu_context_t *context, vidmm_allocation_t *allocation, int paging_allocation_type, unsigned long long *gpu_va)
{
    adapter_t                   *adapter              = mm_mgr->adapter;
    gpu_device_t                *device               = context->device;
    unsigned long long           paging_fence_value   = 0;
    vidmm_allocation_t          *temp_allocation      = NULL;
    vidmm_map_desc_args_t        map_arg              = {0,};
    int status = S_OK;

    temp_allocation = zx_calloc(sizeof(vidmm_allocation_t));
    vidmmi_init_allocation(adapter, temp_allocation);

    temp_allocation->size       = allocation->size;
    temp_allocation->alignment  = allocation->alignment;
    temp_allocation->flag.unpagable = 1;
    temp_allocation->flag.global = allocation->flag.global;

    switch(paging_allocation_type)
    {
    case PAGING_IN_SRC:
        temp_allocation->segment_id = mm_mgr->paging_segment_id;
        temp_allocation->pages_mem = allocation->pages_mem;
        temp_allocation->aligned_page_size = adapter->os_page_size;
        temp_allocation->release_system_pages =  allocation->release_system_pages;
        temp_allocation->get_system_pages = allocation->get_system_pages;
        break;
    case PAGING_IN_DST:
        if (allocation->fb_pages_mem)
        {
            // for restore case
            temp_allocation->segment_id = allocation->segment_id;
            temp_allocation->fb_pages_mem = allocation->fb_pages_mem;
            temp_allocation->aligned_page_size = allocation->aligned_page_size;
            temp_allocation->release_system_pages =  allocation->release_system_pages;
            temp_allocation->get_system_pages = allocation->get_system_pages;
        }
        else
        {
            zx_memcpy(temp_allocation->preferred_segment, allocation->preferred_segment, sizeof(allocation->preferred_segment));
            status = vidmmi_allocate_fb_memory_try(mm_mgr, temp_allocation);
            if (status == S_OK)
            {
                temp_allocation->aligned_page_size = allocation->aligned_page_size;
            }
            else
            {
                vidmm_segment_t *segment = mm_mgr->segment + temp_allocation->preferred_segment[0].segment_id;
                int paging_result, try_times = 0;
try_again:
                paging_result = vidmmi_segment_unresident_allocations(mm_mgr, segment);

                status = vidmmi_allocate_fb_memory_try(mm_mgr, temp_allocation);

                if (status != 0)
                {
                    if (paging_result == S_OK)
                    {
                        goto try_again;
                    }
                    else if (try_times < 100)
                    {
                        zx_msleep(100);

                        try_times++;

                        goto try_again;
                    }
                }

                if (status == S_OK)
                {
                    temp_allocation->aligned_page_size = allocation->aligned_page_size;
                }
                else
                {
                    vidmm_destroy_allocation(temp_allocation, NULL);
                    temp_allocation = NULL;
                }
            }
        }
        break;
    case PAGING_OUT_SRC:
        temp_allocation->segment_id = allocation->segment_id;
        temp_allocation->fb_pages_mem = allocation->fb_pages_mem;
        temp_allocation->aligned_page_size = allocation->aligned_page_size;
        break;
    case PAGING_OUT_DST:
        if (allocation->pages_mem)
        {
            temp_allocation->segment_id = mm_mgr->paging_segment_id;
            temp_allocation->pages_mem = allocation->pages_mem;
            temp_allocation->aligned_page_size = adapter->os_page_size;
            temp_allocation->release_system_pages = allocation->release_system_pages;
            temp_allocation->get_system_pages = allocation->get_system_pages;
        }
        else
        {
            unsigned int page_num = ((allocation->size + adapter->os_page_size - 1) >> adapter->os_page_shift);
            alloc_pages_flags_t flags = {0};
            temp_allocation->preferred_segment[0].segment_id = mm_mgr->paging_segment_id;
            flags.dma32        = !adapter->hw_caps.address_mode_64bit;
            flags.need_flush   = 1;
            flags.fixed_page   = TRUE;
            flags.need_dma_map = 0;
            temp_allocation->pages_mem = allocation->get_system_pages(allocation->parent, adapter->os_device.pdev, temp_allocation->size, &flags);
            temp_allocation->release_system_pages = allocation->release_system_pages;
            if (temp_allocation->pages_mem)
            {
                zx_mutex_lock(mm_mgr->mgr_lock);
                mm_mgr->used_pages_num += page_num;
                zx_mutex_unlock(mm_mgr->mgr_lock);

                temp_allocation->aligned_page_size = adapter->os_page_size;
                temp_allocation->release_system_pages =  allocation->release_system_pages;
                temp_allocation->get_system_pages = allocation->get_system_pages;
                temp_allocation->segment_id = mm_mgr->paging_segment_id;
            }
            else
            {
                zx_info("allocate system pages failed. mm_mgr already used size: %dK, request size: %dK.\n", mm_mgr->used_pages_num*(adapter->os_page_size>>10), page_num*(adapter->os_page_size>>10));
                vidmm_destroy_allocation(temp_allocation, NULL);
                temp_allocation = NULL;
            }
        }
        break;
    }

    // map src allocation to device vma space
    if (temp_allocation)
    {
        ++temp_allocation->resident_age;
        if (gpu_va)
        {
            map_arg.size = temp_allocation->size;
            vidmm_vm_map_gpu_virtual_address(device, temp_allocation, &map_arg);
            vidmm_allocation_update_mapping(device, context, temp_allocation, NULL);
            *gpu_va = map_arg.out_va;
        }
    }

    return temp_allocation;
}

/* release complete temp paging allocation in paging task */
void vidmm_release_temp_paging_allocation(adapter_t *adapter, void *ptask)
{
    vidmm_mgr_t        *mm_mgr              = adapter->mm_mgr;
    vidmm_allocation_t *temp_allocation_src = NULL;
    vidmm_allocation_t *temp_allocation_dst = NULL;
    task_paging_t      *paging_task         = ptask;
    int i;

    for(i = 0; i < paging_task->paging_allocation_num; i++)
    {
        vidmm_paging_allocation_t *paging_allocation = paging_task->paging_allocation_list + i;
        vidmm_allocation_t *allocation = paging_allocation->allocation;
        temp_allocation_src = paging_allocation->temp_allocation_src;
        temp_allocation_dst = paging_allocation->temp_allocation_dst;

        if (!allocation)
            continue;

        if (paging_task->flag.paging_out || paging_task->flag.paging_in ||paging_task->flag.save_restore)
        {
            if (temp_allocation_src)
                vidmm_destroy_allocation(temp_allocation_src, NULL);

            if (temp_allocation_dst)
                vidmm_destroy_allocation(temp_allocation_dst, NULL);
        }
        else if (paging_task->flag.paging_fill)
        {

        }
    }
}

int vidmmi_paging_out(vidmm_mgr_t *mm_mgr, task_paging_t *paging_task, vidmm_paging_allocation_t *paging_allocation)
{
    gpu_context_t       *context                = paging_task->desc.context;
    gpu_device_t        *device                 = context->device;
    vidmm_allocation_t  *allocation             = paging_allocation->allocation;
    int                 result                  = S_OK;
    vidmm_segment_t     *segment                = mm_mgr->segment + allocation->segment_id;

    if (!segment->flags.local)
    {
        zx_info("pcie allocation no need paging...\n");

        return S_OK;
    }

    if (allocation->flag.global && !paging_task->flag.save_restore)
    {
        zx_warning("local + Share allocation only support save_resotre...\n");
        return E_UNEXPECTED;
    }

    if (allocation->fb_pages_mem == NULL)
    {
        zx_error("no fb_pages_mem, how to paging out???\n");
        return E_UNEXPECTED;
    }

    if (allocation->pages_mem != NULL)
    {
        zx_error("already has pages_mem, how to paging out???\n");
        return E_UNEXPECTED;
    }

    paging_allocation->temp_allocation_src = vidmmi_create_temp_paging_allocation(mm_mgr, context, allocation, PAGING_OUT_SRC, &paging_allocation->src_gpu_va);
    if (!paging_allocation->temp_allocation_src)
    {
        result = E_OUTOFMEMORY;
        goto exit;
    }

    paging_allocation->temp_allocation_dst = vidmmi_create_temp_paging_allocation(mm_mgr, context, allocation, PAGING_OUT_DST, &paging_allocation->dst_gpu_va);
    if (!paging_allocation->temp_allocation_dst)
    {
        result = E_OUTOFMEMORY;
        goto exit;
    }

    result = vidmmi_copy_data(mm_mgr,
                              paging_task,
                              paging_allocation);
exit:
    return result;
}

int vidmmi_paging_in(vidmm_mgr_t *mm_mgr, task_paging_t *paging_task, vidmm_paging_allocation_t *paging_allocation)
{
    int                 result      = S_OK;
    gpu_context_t       *context    = paging_task->desc.context;
    gpu_device_t        *device     = context->device;
    vidmm_allocation_t  *allocation = paging_allocation->allocation;
    adapter_t *adapter = mm_mgr->adapter;
    unsigned int page_num = 0;
    alloc_pages_flags_t flags = {0};

    if (!allocation->pages_mem)
    {
        if (allocation->file_storage == NULL)
        {
            zx_error("no pages_mem and no file_storage...\n");
            return E_UNEXPECTED;
        }

        page_num = ((allocation->size + adapter->os_page_size - 1) >> adapter->os_page_shift);
        flags.dma32        = !adapter->hw_caps.address_mode_64bit;
        flags.need_flush   = 1;
        flags.fixed_page   = TRUE;
        flags.need_dma_map = 0;
        allocation->pages_mem = allocation->get_system_pages(allocation->parent, adapter->os_device.pdev, allocation->size, &flags);
        if (allocation->pages_mem)
        {
            zx_mutex_lock(mm_mgr->mgr_lock);
            mm_mgr->used_pages_num += page_num;
            zx_mutex_unlock(mm_mgr->mgr_lock);

            if(zx_pages_memory_swapin(allocation->pages_mem, allocation->file_storage))
            {
                vidmmi_release_system_memory(mm_mgr->adapter, allocation);
                return E_OUTOFMEMORY;
            }
            else
            {
                ++allocation->resident_age;
                allocation->file_storage = NULL;
            }
        }
        else
            zx_info("allocate system pages failed. mm_mgr already used size: %dK, request size: %dK.\n", mm_mgr->used_pages_num*(adapter->os_page_size>>10), page_num*(adapter->os_page_size>>10));
    }

    zx_assert(allocation->pages_mem != NULL, "");

    if (allocation->flag.global && (!paging_task->flag.save_restore || !allocation->fb_pages_mem))
    {
        zx_error("share memory only support restore...\n");
        return E_UNEXPECTED;
    }

    if (allocation->fb_pages_mem && !paging_task->flag.save_restore)
    {
        zx_error("already has fb_pages_mem, but not save_restore, why???\n");
        return E_UNEXPECTED;
    }


    paging_allocation->temp_allocation_src = vidmmi_create_temp_paging_allocation(mm_mgr, context, allocation, PAGING_IN_SRC, &paging_allocation->src_gpu_va);
    if (!paging_allocation->temp_allocation_src)
    {
        result = E_OUTOFMEMORY;
        goto exit;
    }

    paging_allocation->temp_allocation_dst = vidmmi_create_temp_paging_allocation(mm_mgr, context, allocation, PAGING_IN_DST, &paging_allocation->dst_gpu_va);
    if (!paging_allocation->temp_allocation_dst)
    {
        result = E_OUTOFMEMORY;
        goto exit;
    }

    result = vidmmi_copy_data(mm_mgr,
                              paging_task,
                              paging_allocation);
exit:
    return result;
}

// assume allocation->paging_lock hold
int vidmm_resident_one_allocation(adapter_t *adapter, vidmm_allocation_t *allocation)
{
    int ret = S_OK;
    int segment_id = 0, i = 0;
    vidmm_segment_preference_t *preferred_segment = allocation->preferred_segment;
    task_paging_t *paging_task   = NULL;
    vidmm_paging_allocation_t *paging_allocation = NULL;
    vidsch_allocate_paging_task_t allocate_paging_task = {0,};

    if (allocation->flag.global)
    {
        zx_warning("share allocation can't paging...\n");
        return E_UNEXPECTED;
    }

    zx_allocation_trace_event(allocation->adapter->index, allocation->parent, ALLOCATION_EVENT_RESIDENT_BEGIN, allocation->segment_id);

    for (i = 0; i < ARRAY_SIZE(allocation->preferred_segment); i++)
    {
        segment_id = preferred_segment[i].segment_id;

        if (segment_id != SEGMENT_ID_INVALID && segment_id == adapter->mm_mgr->paging_segment_id)
        {
            // no need pagin
            vidmm_remove_allocation_from_cache_list(adapter, allocation);

            allocation->segment_id = adapter->mm_mgr->paging_segment_id;

            vidmm_add_allocation_to_segment_resident_list(adapter, allocation);

            ++allocation->resident_age;

            if (allocation->flag.global || allocation->flag.continuous)
            {
                allocation->need_remap = 1;
            }

            goto done;
        }
    }

    allocate_paging_task.dma_size       = 1024;
    allocate_paging_task.allocation_num = 1;
    paging_task = vidsch_allocate_paging_task(adapter->reserved_context, &allocate_paging_task);
    paging_task->paging_allocation_num  = 1;
    paging_task->must_success           = TRUE;
    paging_task->flag.paging_in         = 1;

    paging_allocation                   = &paging_task->paging_allocation_list[0];
    paging_allocation->allocation       = allocation;

    ret = vidmmi_paging_in(adapter->mm_mgr, paging_task, paging_allocation);

    if (ret == S_OK)
    {
        vidsch_submit_paging_task(adapter, paging_task);

        vidsch_wait_fence_back(adapter, adapter->paging_engine_index, paging_task->desc.fence_id, TRUE);

        if (0 /* DEBUG */)
        {
            zx_map_argu_t       src_map         = {0};
            zx_cpu_vm_area_t    *src_vma        = NULL;
            unsigned int        src_first_uint  = 0;

            zx_map_argu_t       dst_map         = {0};
            zx_cpu_vm_area_t    *dst_vma        = NULL;
            unsigned int        dst_first_uint  = 0;

            dst_map.size             = allocation->size;
            dst_map.flags.mem_type   = ZX_SYSTEM_IO;
            dst_map.node_num         = 1;
            dst_map.phys_addr        = adapter->vidmm_bus_addr + paging_allocation->temp_allocation_dst->fb_pages_mem->range_node_list[0]->start;
            dst_map.flags.cache_type = ZX_MEM_WRITE_COMBINED;
            dst_vma = zx_map_io_memory(&dst_map);
            dst_first_uint = *(unsigned int*)dst_vma->virt_addr;
            zx_unmap_io_memory(dst_vma);


            src_map.size             = allocation->size;
            src_map.flags.mem_type   = ZX_SYSTEM_RAM;
            src_map.memory           = allocation->pages_mem;
            src_map.flags.cache_type = ZX_MEM_WRITE_COMBINED;
            src_vma = zx_map_pages_memory(&src_map);
            src_first_uint = *(unsigned int*)src_vma->virt_addr;
            zx_unmap_pages_memory(src_vma);

            zx_info("paging: resident_one(%p pages_mem:%p -> fb_pages_mem:%p, %x -> %x)\n",
                    allocation, allocation->pages_mem,
                    paging_allocation->temp_allocation_dst->fb_pages_mem,
                    src_first_uint, dst_first_uint);
        }

        allocation->pages_mem = NULL;

        allocation->fb_pages_mem = paging_allocation->temp_allocation_dst->fb_pages_mem;

        paging_allocation->temp_allocation_dst->fb_pages_mem = NULL;

        vidmm_remove_allocation_from_cache_list(adapter, allocation);

        allocation->segment_id = paging_allocation->temp_allocation_dst->segment_id;

        vidmm_add_allocation_to_segment_resident_list(adapter, allocation);

        ++allocation->resident_age;
    }
    else if (paging_allocation->temp_allocation_src)
    {
        paging_allocation->temp_allocation_src->pages_mem = NULL;
    }

    vidsch_task_dec_reference(&paging_task->desc);
done:
    zx_allocation_trace_event(allocation->adapter->index, allocation->parent, ALLOCATION_EVENT_RESIDENT_END, allocation->segment_id);
    return ret;
}

// assume allocation->paging_lock hold
int vidmmi_unresident_one_allocation(adapter_t *adapter, vidmm_allocation_t *allocation)
{
    int ret = S_OK;
    task_paging_t *paging_task   = NULL;
    vidmm_paging_allocation_t *paging_allocation = NULL;
    vidsch_allocate_paging_task_t allocate_paging_task = {0,};
    vidmm_segment_t *segment = adapter->mm_mgr->segment + allocation->segment_id;

    if (allocation->status.temp_unpagable > 0)
        return E_PAGEOUT_ALLOCATION_BUSY;

    if (segment->flags.invalid || !segment->flags.local || allocation->flag.global)
    {
        zx_warning("only local + private allocation can paging...\n");
        return E_UNEXPECTED;
    }

    zx_allocation_trace_event(allocation->adapter->index, allocation->parent, ALLOCATION_EVENT_EVICT_BEGIN, allocation->segment_id);

    allocate_paging_task.dma_size       = 1024;
    allocate_paging_task.allocation_num = 1;
    paging_task = vidsch_allocate_paging_task(adapter->reserved_context, &allocate_paging_task);
    paging_task->paging_allocation_num  = 1;
    paging_task->must_success           = TRUE;
    paging_task->flag.paging_out        = 1;

    paging_allocation                   = &paging_task->paging_allocation_list[0];
    paging_allocation->allocation       = allocation;

    ret = vidmmi_paging_out(adapter->mm_mgr, paging_task, paging_allocation);
    if (ret == S_OK)
    {
        vidsch_submit_paging_task(adapter, paging_task);

        vidsch_wait_fence_back(adapter, adapter->paging_engine_index, paging_task->desc.fence_id, TRUE);

        if (0 /* DEBUG */)
        {
            zx_map_argu_t       src_map         = {0};
            zx_cpu_vm_area_t    *src_vma        = NULL;
            unsigned int        src_first_uint  = 0;

            zx_map_argu_t       dst_map         = {0};
            zx_cpu_vm_area_t    *dst_vma        = NULL;
            unsigned int        dst_first_uint  = 0;

            src_map.size             = allocation->size;
            src_map.flags.mem_type   = ZX_SYSTEM_IO;
            src_map.node_num         = 1;
            src_map.phys_addr        = adapter->vidmm_bus_addr + allocation->fb_pages_mem->range_node_list[0]->start;
            src_map.flags.cache_type = ZX_MEM_WRITE_COMBINED;
            src_vma = zx_map_io_memory(&src_map);
            src_first_uint = *(unsigned int*)src_vma->virt_addr;
            zx_unmap_io_memory(src_vma);

            dst_map.size             = allocation->size;
            dst_map.flags.mem_type   = ZX_SYSTEM_RAM;
            dst_map.memory           = paging_allocation->temp_allocation_dst->pages_mem;
            dst_map.flags.cache_type = ZX_MEM_WRITE_COMBINED;
            dst_vma = zx_map_pages_memory(&dst_map);
            dst_first_uint = *(unsigned int*)dst_vma->virt_addr;
            zx_unmap_pages_memory(dst_vma);

            zx_info("paging: unresident_one(%p fb_pages_mem:%p -> pages_mem:%p, %x -> %x)\n",
                    allocation, allocation->fb_pages_mem,
                    paging_allocation->temp_allocation_dst->pages_mem,
                    src_first_uint, dst_first_uint);
        }

        allocation->fb_pages_mem = NULL;

        allocation->pages_mem = paging_allocation->temp_allocation_dst->pages_mem;

        paging_allocation->temp_allocation_dst->pages_mem = NULL;
    }
    else if (paging_allocation->temp_allocation_src)
    {
        paging_allocation->temp_allocation_src->fb_pages_mem = NULL;
    }

    /*Correspond to inc refs in allocate paging task*/
    vidsch_task_dec_reference(&paging_task->desc);

    zx_allocation_trace_event(allocation->adapter->index, allocation->parent, ALLOCATION_EVENT_EVICT_END, allocation->segment_id);

    return ret;
}

// this interface just for debug test...
int vidmm_unresident_one_allocation(adapter_t *adapter, vidmm_allocation_t *allocation)
{
    int ret;

    vidmm_remove_allocation_from_segment_resident_list(adapter, allocation);

    ret = vidmmi_unresident_one_allocation(adapter, allocation);

    if (ret == 0)
    {
        allocation->segment_id = 0;
        vidmm_add_allocation_to_cache_list(adapter, allocation);
    }
    else
    {
        vidmm_add_allocation_to_segment_resident_list(adapter, allocation);
    }

    return ret;
}

#define VIDMM_MAX_PAGING_OUT_MEMORY_SIZE     4*1024*1024 //4M
int vidmmi_segment_unresident_allocations(vidmm_mgr_t *mm_mgr, vidmm_segment_t *segment)
{
    int ret = 0;
    int priority = 0;
    int unresident_size = 0;
    adapter_t *adapter = mm_mgr->adapter;
    vidmm_allocation_t *allocation = NULL, *next;

    if (!(segment->property & ZX_SEGMENT_PROPERTY_LOCAL))
    {
        zx_error("only local segment support paging out...\n");
        return E_UNEXPECTED;
    }

    zx_mutex_lock(segment->lock);
    for (priority = 0; priority < PALL; priority++)
    {
        struct list_head paging_list;

        list_init_head(&paging_list);

        list_splice_init(&segment->pagable_resident_list[priority], &paging_list);

        zx_mutex_unlock(segment->lock);

        list_for_each_entry_safe(allocation, next, &paging_list, list_item)
        {
            zx_mutex_lock(allocation->paging_lock);

            if (!allocation->status.temp_unpagable && !allocation->flag.global)
            {
                vidmm_remove_allocation_from_segment_resident_list(adapter, allocation);

                ret = vidmmi_unresident_one_allocation(adapter, allocation);

                if (ret == 0)
                {
                    unresident_size += allocation->size;
                    allocation->segment_id = 0;
                    vidmm_add_allocation_to_cache_list(adapter, allocation);
                }
                else
                {
                    vidmm_add_allocation_to_segment_resident_list(adapter, allocation);
                }

                if (ret != 0 || unresident_size >= VIDMM_MAX_PAGING_OUT_MEMORY_SIZE)
                {
                    zx_mutex_unlock(allocation->paging_lock);
                    goto done;
                }
            }

            zx_mutex_unlock(allocation->paging_lock);
        }

done:
        zx_mutex_lock(segment->lock);
        list_splice_init(&paging_list, &segment->pagable_resident_list[priority]);
    }
    zx_mutex_unlock(segment->lock);

    return ret;
}

int vidmmi_get_swapable_pages_num(vidmm_mgr_t *mm_mgr)
{
    adapter_t       *adapter = mm_mgr->adapter;

    return (mm_mgr->paging_mgr->system_allocation_size) >> adapter->os_page_shift;
}

/* swapout system cache to shmem
 * return the real swap out size.
 */
#define MAX_SWAP_NUM 32

int vidmmi_do_swap_out(vidmm_mgr_t *mm_mgr, int need_pages)
{
    return 0;
#if 0
    adapter_t       *adapter = mm_mgr->adapter;
    vidmm_segment_t *segment = &mm_mgr->segment[SEGMENT_ID_INVALID];
    vidmm_allocation_t *allocation, *next;
    int i, real_swap_pages = 0, try_times = 20, swap_pages, swap_num, priority;

try_again:
    zx_mutex_lock(segment->lock);
    for (priority = 0; priority < PALL; priority++)
    {
        struct list_head paging_list;
        struct list_head swap_list;

        list_init_head(&paging_list);
        list_init_head(&swap_list);

        list_splice_init(&segment->pagable_resident_list[priority], &paging_list);

        zx_mutex_unlock(segment->lock);
        list_for_each_entry_safe(allocation, next, &paging_list, list_item)
        {
            zx_mutex_lock(allocation->paging_lock);
            if (allocaiton->pages_mem)
            {
                zx_info("swapout: allocation:%p size:%dk.\n", allocation, allocation->size / 1024);

                allocation->file_storage = zx_pages_memory_swapout(allocation->pages_mem);
                if (allocation->file_storage != NULL)
                {
                    vidmmi_release_system_memory(adapter, allocation);

                    real_swap_pages += (allocation->size / ZX_PAGE_SIZE);

                    list_del(&allocation->list_item);
                    list_add_tail(&allocation->list_item, &swap_list);
                }
            }
            zx_mutex_unlock(allocation->paging_lock);

            if (real_swap_pages >= need_pages)
            {
                break;
            }
        }

        zx_mutex_lock(segment->lock);
        list_splice_init_tail(&paging_list, &segment->pagable_resident_list[priority]);
        list_splice_init_tail(&swap_list, &segment->pagable_resident_list[priority]);
    }
    zx_mutex_unlock(segment->lock);

    if (real_swap_pages < need_pages)
    {
        if (--try_times > 0)
        {
            zx_msleep(20);
            goto try_again;
        }
    }

    return real_swap_pages;
#endif
}

int vidmmi_prepare_allocation_by_cpu(adapter_t *adapter, vidmm_allocation_t *allocation,unsigned int save)
{
    vidmm_allocation_t     *temp_allocation     = NULL;
    vidmm_mgr_t            *mm_mgr              = adapter->mm_mgr;
    gpu_context_t          *context             = adapter->reserved_context;

    zx_map_argu_t           src_map             = {0};
    zx_cpu_vm_area_t       *src_vma             = NULL;

    zx_map_argu_t           dst_map             = {0};
    zx_cpu_vm_area_t       *dst_vma             = NULL;
    unsigned int            ret                 = S_OK;
    int i;
    unsigned long long     node_size;

    if (save)
    {
        temp_allocation = vidmmi_create_temp_paging_allocation(mm_mgr,context,allocation,PAGING_OUT_DST,NULL);
        if (! temp_allocation)
        {
            return E_OUTOFMEMORY;
        }
        src_map.size                = allocation->size;
        src_map.flags.mem_type      = ZX_SYSTEM_IO;
        src_map.node_num            = allocation->fb_pages_mem->range_node_num;
        if (src_map.node_num == 1)
        {
            src_map.phys_addr           = adapter->vidmm_bus_addr + allocation->fb_pages_mem->range_node_list[0]->start;
        }
        else
        {
            src_map.phys_addrs = zx_calloc(sizeof(unsigned long long) * src_map.node_num);
            src_map.node_sizes = zx_calloc(sizeof(unsigned long long) * src_map.node_num);
            for (i=0;i<src_map.node_num;i++)
            {
                src_map.node_sizes[i] = allocation->fb_pages_mem->range_node_list[i]->size;
                src_map.phys_addrs[i] = adapter->vidmm_bus_addr + allocation->fb_pages_mem->range_node_list[i]->start;
            }
        }
        src_map.flags.cache_type    = ZX_MEM_WRITE_COMBINED;
        src_vma                     = zx_map_io_memory(&src_map);

        dst_map.size                = allocation->size;
        dst_map.flags.mem_type      = ZX_SYSTEM_RAM;
        dst_map.memory              = temp_allocation->pages_mem;
        dst_map.flags.cache_type    = ZX_MEM_WRITE_COMBINED;
        dst_vma                     = zx_map_pages_memory(&dst_map);

        if (src_map.node_num == 1)
        {
            zx_memcpy(dst_vma->virt_addr, src_vma->virt_addr, allocation->size);
        }
        else
        {
            node_size = 0;
            for (i=0;i<src_map.node_num;i++)
            {
                zx_memcpy(((char*)dst_vma->virt_addr+node_size), src_vma->virt_addrs[i], src_vma->node_sizes[i]);
                node_size += src_vma->node_sizes[i];
            }
        }
        zx_unmap_io_memory(src_vma);
        zx_unmap_pages_memory(dst_vma);

        /*
        zx_info("paging: resident_one(alloc:%p pages_mem:%p -> fb_pages_mem:%p, %x -> %x)\n",
                allocation, allocation->pages_mem,
                temp_allocation_dst->fb_pages_mem,
                src_first_uint, dst_first_uint);
        */
        allocation->pages_mem           = temp_allocation->pages_mem;
        temp_allocation->pages_mem      = NULL;

        if (temp_allocation)
        {
            vidmm_destroy_allocation(temp_allocation, NULL);
            temp_allocation     = NULL;
        }
        if (src_map.node_num > 1)
        {
            zx_free(src_map.phys_addrs);
            zx_free(src_map.node_sizes);
        }
    }
    else
    {
        temp_allocation = vidmmi_create_temp_paging_allocation(mm_mgr,context,allocation,PAGING_IN_DST,NULL);
        if (! temp_allocation)
        {
            return E_OUTOFMEMORY;
        }

        dst_map.size                    = allocation->size;
        dst_map.flags.mem_type          = ZX_SYSTEM_IO;
        dst_map.node_num = temp_allocation->fb_pages_mem->range_node_num;
        if (dst_map.node_num == 1)
        {
            dst_map.phys_addr           = adapter->vidmm_bus_addr + temp_allocation->fb_pages_mem->range_node_list[0]->start;
        }
        else
        {
            dst_map.phys_addrs = zx_calloc(sizeof(unsigned long long) * dst_map.node_num);
            dst_map.node_sizes = zx_calloc(sizeof(unsigned long long) * dst_map.node_num);
            for (i=0;i<dst_map.node_num;i++)
            {
                dst_map.node_sizes[i] = allocation->fb_pages_mem->range_node_list[i]->size;
                dst_map.phys_addrs[i] = adapter->vidmm_bus_addr + temp_allocation->fb_pages_mem->range_node_list[i]->start;
            }
        }
        dst_map.flags.cache_type        = ZX_MEM_WRITE_COMBINED;
        dst_vma                         = zx_map_io_memory(&dst_map);

        src_map.size                    = allocation->size;
        src_map.flags.mem_type          = ZX_SYSTEM_RAM;
        src_map.memory                  = allocation->pages_mem;
        src_map.flags.cache_type        = ZX_MEM_WRITE_COMBINED;
        src_vma                         = zx_map_pages_memory(&src_map);

        /*
        zx_info("paging: resident_one(alloc:%p pages_mem:%p -> fb_pages_mem:%p, %x -> %x)\n",
                    allocation, allocation->pages_mem,
                    temp_allocation_dst->fb_pages_mem,
                    src_first_uint, dst_first_uint);
        */
        if (dst_map.node_num == 1)
        {
            zx_memcpy(dst_vma->virt_addr, src_vma->virt_addr, allocation->size);
        }
        else
        {
            node_size = 0;
            for (i=0;i<dst_map.node_num;i++)
            {
                zx_memcpy(dst_vma->virt_addrs[i], ((char*)src_vma->virt_addr+node_size), dst_vma->node_sizes[i]);
                node_size += dst_vma->node_sizes[i];
            }
        }
        zx_unmap_io_memory(dst_vma);
        zx_unmap_pages_memory(src_vma);

        temp_allocation->pages_mem      = allocation->pages_mem;

        allocation->pages_mem           = NULL;
        temp_allocation->fb_pages_mem   = NULL;

        if (temp_allocation)
        {
            vidmm_destroy_allocation(temp_allocation, NULL);
            temp_allocation     = NULL;
        }
        if (dst_map.node_num > 1)
        {
            zx_free(dst_map.phys_addrs);
            zx_free(dst_map.node_sizes);
        }
    }
    return ret ;
}


int vidmmi_prepare_allocation_by_gpu(adapter_t *adapter, vidmm_allocation_t *allocation,unsigned int save)
{
    task_paging_t                   *paging_task            = NULL;
    vidmm_paging_allocation_t       *paging_allocation      = NULL;
    vidsch_allocate_paging_task_t    allocate_paging_task   = {0,};
    unsigned int ret = S_OK;

    allocate_paging_task.dma_size       = 1024;
    allocate_paging_task.allocation_num = 1;
    paging_task = vidsch_allocate_paging_task(adapter->reserved_context, &allocate_paging_task);
    paging_task->paging_allocation_num  = 1;
    paging_task->must_success           = TRUE;
    paging_task->flag.save_restore      = 1;

    paging_allocation                   = &paging_task->paging_allocation_list[0];
    paging_allocation->allocation       = allocation;

    if (save)
    {
        ret = vidmmi_paging_out(adapter->mm_mgr, paging_task, paging_allocation);
        if (ret == S_OK)
        {
            vidsch_submit_paging_task(adapter, paging_task);
            vidsch_wait_fence_back(adapter, adapter->paging_engine_index, paging_task->desc.fence_id, TRUE);

            // for save-restore, keep allocation->fb_pages_mem,  clear temp src's fb_pages_mem,
            // keep  new allocated allocation->pages_mem, clear temp dst's pages_mem
            paging_allocation->temp_allocation_src->fb_pages_mem    = NULL;

            allocation->pages_mem                                   = paging_allocation->temp_allocation_dst->pages_mem;
            paging_allocation->temp_allocation_dst->pages_mem       = NULL;
        }
        else if (paging_allocation->temp_allocation_src)
        {
            paging_allocation->temp_allocation_src->fb_pages_mem    = NULL;
        }

        /*Correspond to inc refs in allocate paging task*/
        vidsch_task_dec_reference(&paging_task->desc);
    }
    else // restore
    {
        ret = vidmmi_paging_in(adapter->mm_mgr, paging_task, paging_allocation);
        if (ret == S_OK)
        {
            vidsch_submit_paging_task(adapter, paging_task);
            vidsch_wait_fence_back(adapter, adapter->paging_engine_index, paging_task->desc.fence_id, TRUE);

            allocation->pages_mem                                   = NULL;

            paging_allocation->temp_allocation_dst->fb_pages_mem    = NULL;
        }
        else if (paging_allocation->temp_allocation_src)
        {
            paging_allocation->temp_allocation_src->pages_mem       = NULL;
        }

        vidsch_task_dec_reference(&paging_task->desc);
    }
    return ret;
}

/* NOTE: save/restore do not use lock since save/restore used by pm func,
 *       when here all app-thread/zx-thread all freezing
 */

int vidmm_save_allocation(adapter_t *adapter, vidmm_allocation_t *allocation)
{
    int ret = S_OK;
    task_paging_t *paging_task   = NULL;
    vidmm_paging_allocation_t *paging_allocation = NULL;
    vidsch_allocate_paging_task_t allocate_paging_task = {0,};

    /* only need save allocation which locate in local(VRAM on chip)
     * if pages_mem != NULL means allocation currently locate in system, gart or system cache
     */
    if(allocation->pages_mem != NULL)
    {
        return ret;
    }
    if (! allocation->flag.need_save_when_acpi)
    {
        return S_OK;
    }
    zx_allocation_trace_event(allocation->adapter->index, allocation->parent, ALLOCATION_EVENT_SAVE_BEGIN, allocation->segment_id);

    if (adapter->ctl_flags.force_cpu_cm_save_restore && adapter->visible_local_memory_size > allocation->fb_pages_mem->range_node_list[0]->start)
    {
        ret = vidmmi_prepare_allocation_by_cpu(adapter,allocation,1);
    }
    else
    {
        ret = vidmmi_prepare_allocation_by_gpu(adapter,allocation,1);
    }

    zx_allocation_trace_event(allocation->adapter->index, allocation->parent, ALLOCATION_EVENT_SAVE_END, allocation->segment_id);

    return ret;
}

int vidmm_restore_allocation(adapter_t *adapter, vidmm_allocation_t *allocation)
{
    int ret = S_OK;
    task_paging_t *paging_task   = NULL;
    vidmm_paging_allocation_t *paging_allocation = NULL;
    vidsch_allocate_paging_task_t allocate_paging_task = {0,};
    vidmm_segment_t *segment = adapter->mm_mgr->segment + allocation->segment_id;

    // for all allocation should ++resident_age, since need rebuild ptes after resume even if it's pcie buffer
    ++allocation->resident_age;

    if (segment->flags.invalid || !segment->flags.local)
    {
        return ret;
    }

    if(! allocation->flag.need_save_when_acpi)
    {
        allocation->flag.need_save_when_acpi = 1;
        return S_OK;
    }

    zx_allocation_trace_event(allocation->adapter->index, allocation->parent, ALLOCATION_EVENT_RESTORE_BEGIN, allocation->segment_id);

    if (adapter->ctl_flags.force_cpu_cm_save_restore && adapter->visible_local_memory_size > allocation->fb_pages_mem->range_node_list[0]->start)
    {
        ret = vidmmi_prepare_allocation_by_cpu(adapter,allocation,0);
    }
    else
    {
        ret = vidmmi_prepare_allocation_by_gpu(adapter,allocation,0);
    }

    zx_allocation_trace_event(allocation->adapter->index, allocation->parent, ALLOCATION_EVENT_RESTORE_END, allocation->segment_id);
    return ret;
}

void vidmm_fill_allocation_info(adapter_t *adapter, vidmm_allocation_t *allocation, zx_allocation_info_t *info)
{
    vidmm_mgr_t *mm_mgr  = adapter->mm_mgr;
    vidmm_segment_t *segment = NULL;

    if(!allocation)
    {
        zx_error("%s: invalid allocation. %p\n", __func__, allocation);
        return;
    }

    segment = mm_mgr->segment + allocation->segment_id;

    info->size          = allocation->size;
    info->unpagable     = allocation->flag.unpagable;
    info->secured       = allocation->flag.secured;
    info->segment_id    = allocation->segment_id;
    info->pool_type     = allocation->flag.pool_type;
    info->cache_type    = allocation->flag.cache_type;
    info->tiled         = allocation->flag.tiled;
    info->share         = allocation->flag.global;
    info->cpu_visible   = allocation->flag.cpu_visible;
    info->priority      = allocation->priority;
    info->bl_slot_index = allocation->slot_index;
    info->compress_format = allocation->compress_format;
    info->alignment     = allocation->alignment;
    if(segment->flags.local)
        info->local_continuous = (allocation->fb_pages_mem->range_node_num == 1);

    if(allocation->fb_pages_mem)
    {
        info->cpu_phy_addr = allocation->fb_pages_mem->range_node_list[0]->start + adapter->vidmm_bus_addr;
    }
    else
    {
        info->cpu_phy_addr = 0;
#ifdef ZX_VMI_MODE
        if (allocation->pages_mem)
        {
            zx_map_argu_t map = {0, };
            zx_cpu_vm_area_t *vma   = NULL;
            map.size = allocation->size;
            map.flags.mem_type = ZX_SYSTEM_RAM;
            map.memory = allocation->pages_mem;
            map.flags.cache_type = ZX_MEM_WRITE_COMBINED;
            vma = zx_map_pages_memory(&map);
            info->cpu_phy_addr = ptr_to_ptr64(vma->virt_addr);
            zx_unmap_pages_memory(vma);
        }
#endif
    }
}

int vidmm_prepare_and_mark_unpagable(vidmm_allocation_t *allocation)
{
    int ret = 0;

    zx_mutex_lock(allocation->paging_lock);
    ++allocation->status.temp_unpagable;
    ret = vidmm_make_resident_locked(allocation);
    zx_mutex_unlock(allocation->paging_lock);

    return ret;
}

void vidmm_mark_pagable(vidmm_allocation_t *allocation)
{
    zx_mutex_lock(allocation->paging_lock);
    --allocation->status.temp_unpagable;
    zx_mutex_unlock(allocation->paging_lock);
}


