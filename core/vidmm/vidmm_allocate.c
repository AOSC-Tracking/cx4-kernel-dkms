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
#include "vidmmi.h"
#include "vidsch.h"
#include "vidschi.h"
#include "vidsch_submit.h"


#define SWAP_OUT_PAGES_SIZE (32*1024*1024)

#define MAX_UPDATE_VM_DMA_SIZE (8 * 4096) //(MAX_UPDATE_PTE_COUNT * 4 + 1024)

void vidmmi_release_video_memory(vidmm_mgr_t *mm_mgr, vidmm_allocation_t *allocation)
{
    vidmm_segment_t *segment = &mm_mgr->segment[allocation->segment_id];
    int i = 0;

    if(allocation->fb_pages_mem)
    {
        zx_mutex_lock(segment->lock);
        for(i=0; i<allocation->fb_pages_mem->range_node_num; i++){
            struct range_node *range = allocation->fb_pages_mem->range_node_list[i];

            range->release(range);
        }

        segment->used_size -= allocation->size;

        zx_mutex_unlock(segment->lock);
        zx_free(allocation->fb_pages_mem->range_node_list);
        zx_free(allocation->fb_pages_mem);

        allocation->fb_pages_mem = NULL;
    }

}

static void vidmmi_cleanup_allocation_vma(vidmm_allocation_t *allocation)
{
    zx_cpu_vm_area_t *vma = NULL;

    zx_spin_lock(allocation->lock);
    vma = allocation->krnl_cpu_vma;
    if(vma != NULL)
    {
        vma->ref_cnt--;
        allocation->krnl_cpu_vma = NULL;
    }
    if(vma && (vma->ref_cnt != 0)) vma = NULL;

    zx_spin_unlock(allocation->lock);

    if(vma)
    {
        zx_assert(vma->ref_cnt == 0, "vma->ref_cnt=%d", vma->ref_cnt);

        switch(vma->flags.mem_type)
        {
        case ZX_SYSTEM_IO:
            zx_unmap_io_memory(vma);
            break;
        case ZX_SYSTEM_RAM:
            zx_unmap_pages_memory(vma);
            break;
        default:
            break;
        }
    }
}

/* caller should make sure the allocation is idle */
static void vidmm_do_destroy_allocation(vidmm_allocation_t *allocation)
{
    adapter_t *adapter = allocation->adapter;
    vidmm_mgr_t *mm_mgr = adapter->mm_mgr;

    if (allocation->flag.cpu_map)
        vidmm_unlock_allocation(allocation);

    vidmm_remove_allocation_from_segment_resident_list(adapter, allocation);

    vidmmi_cleanup_allocation_vma(allocation);

    vidmm_vm_release_allocation(allocation);

    vidmmi_release_video_memory(mm_mgr, allocation);

    vidmmi_release_system_memory(adapter, allocation);

    if(mm_mgr->chip_func->free_bl_slot && allocation->bl_range)
    {
        mm_mgr->chip_func->free_bl_slot(adapter, allocation);
        allocation->slot_index = 0;
    }

    if(allocation->file_storage)
    {
        zx_release_file_storage(allocation->file_storage);

        allocation->file_storage = NULL;
    }

    zx_destroy_spinlock(allocation->lock);

    zx_destroy_mutex(allocation->va_lock);

    zx_destroy_mutex(allocation->paging_lock);

    zx_free(allocation);
}


void* vidmm_lock_segment_memory(adapter_t *adapter, vidmm_segment_memory_t *segment_memory)
{
    vidmm_mgr_t         *mm_mgr     = adapter->mm_mgr;
    zx_map_argu_t       map        = {0};
    zx_cpu_vm_area_t   *vma        = NULL;

    vma = segment_memory->krnl_cpu_vma;

    if(vma)
    {
        return vma->virt_addr;
    }

    map.size             = segment_memory->size;
    if(segment_memory->flag.local == 0)
    {
        map.flags.mem_type   = ZX_SYSTEM_RAM;
        map.memory           = segment_memory->pages_mem;
        map.offset           = 0;

        if(segment_memory->flag.snooping_enabled)
        {
            map.flags.cache_type = ZX_MEM_WRITE_BACK;
        }
        else
        {
            map.flags.cache_type = ZX_MEM_WRITE_COMBINED;
        }

        vma = zx_map_pages_memory(&map);
    }
    else
    {
        vidmm_mgr_t              *mm_mgr     = adapter->mm_mgr;
        vidmm_segment_t          *fb_heap    = mm_mgr->segment;
        unsigned int             segment_id = segment_memory->segment_id;

//todo: force use write_combinded
#if 0
        if(segment_memory->flag.snooping_enabled)
        {
            map.flags.cache_type = ZX_MEM_WRITE_BACK;
        }
        else
#endif
        {
            map.flags.cache_type = ZX_MEM_WRITE_COMBINED;
        }

        map.flags.mem_type   = ZX_SYSTEM_IO;
        map.node_num = segment_memory->fb_pages_mem->range_node_num;
        map.phys_addr = adapter->vidmm_bus_addr + segment_memory->fb_pages_mem->range_node_list[0]->start;

        if (segment_memory->fb_pages_mem->range_node_list[0]->start + map.size > adapter->visible_local_memory_size)
        {
            return NULL;
        }
        vma = zx_map_io_memory(&map);

        //zx_info("vma:%p, start: 0x%x, phys_addr_start: 0x%x\n", vma->virt_addr, fb_heap[segment_id].start_addr, fb_heap[segment_id].phys_addr_start);
    }

    /* put need flush check in the last, maybe future we also map io as wb */
    if((!segment_memory->flag.snooping_enabled) && (vma->flags.cache_type == ZX_MEM_WRITE_BACK))
    {
        vma->need_flush_cache = TRUE;
    }

    segment_memory->krnl_cpu_vma = vma;

    return vma->virt_addr;
}

void vidmm_unlock_segment_memory(adapter_t *adapter, vidmm_segment_memory_t *segment_memory)
{
    zx_cpu_vm_area_t *vma = NULL;

    vma = segment_memory->krnl_cpu_vma;
    segment_memory->krnl_cpu_vma = NULL;

    if(vma == NULL)
    {
        return;
    }

    switch(vma->flags.mem_type)
    {
    case ZX_SYSTEM_IO:
        zx_unmap_io_memory(vma);
        break;
    case ZX_SYSTEM_RAM:
        zx_unmap_pages_memory(vma);
        break;
    default:
        zx_assert(0, "mem_type:%d", vma->flags.mem_type);
    }
}

int vidmmi_init_allocation(adapter_t *adapter, vidmm_allocation_t *allocation)
{
    allocation->adapter             = adapter;

    list_init_head(&allocation->gpu_vm_list);

    // init node.
    list_init_head(&allocation->list_item);

    allocation->resident_age      = 0;
    allocation->lock              = zx_create_spinlock();
    allocation->va_lock           = zx_create_mutex();
    allocation->paging_lock       = zx_create_mutex();

    return 0;
}

/*
* vidmm_create_allocation: create new allocation object and initialize the related description information.
*/
void* vidmm_create_allocation(adapter_t *adapter, krnl_create_allocation_info_t *create)
{
    vidmm_mgr_t  *mm_mgr  = adapter->mm_mgr;
    vidmm_describe_allocation_t   desc_info       = {0};
    vidmm_allocation_t           *allocation      = NULL;
    zx_create_allocation_info_t  *allocation_info = create->info;

    zx_assert(allocation_info != NULL, "allocation info is NULL when create allocation");

    allocation = zx_calloc(sizeof(vidmm_allocation_t));
    zx_assert(allocation != NULL, "out of memory");

    vidmmi_init_allocation(adapter, allocation);
    allocation->priority          = allocation_info->priority;

    allocation->flag.global       = allocation_info->flags.share;
    allocation->flag.cache_type   = allocation_info->flags.cache_type;
    allocation->flag.pool_type    = allocation_info->flags.pool_type;
    allocation->flag.cpu_visible  = allocation_info->flags.cpu_visible;
    allocation->flag.unpagable    = allocation_info->flags.unpagable;
    allocation->flag.secured      = allocation_info->flags.secured;
    allocation->flag.tiled        = allocation_info->flags.tiled;
    allocation->flag.force_clear  = allocation_info->flags.force_clear;
    allocation->flag.bVideoInternal  = allocation_info->flags.bVideoInternal;
    allocation->flag.continuous = allocation_info->flags.continuous;
    allocation->compress_format   = allocation_info->range_type;
    allocation->segment_id        = 0;
    allocation->need_remap        = 0;
    allocation->get_system_pages = create->get_system_pages;
    allocation->release_system_pages = create->release_system_pages;

    desc_info.allocation    = allocation;
    allocation->size        = allocation_info->size;
    allocation->alignment   = allocation_info->alignment;

    //call the chip dependent function to fill the allocation description information.
    mm_mgr->chip_func->describe_allocation(adapter, &desc_info);

    allocation->parent = create->parent;
    allocation->debug_gid = create->debug_gid;

    if (adapter->hw_caps.snoop_only && allocation->compress_format)
    {
        vidmm_prepare_and_check_compress(adapter, allocation);
    }

    return allocation;
}

int vidmm_fill_allocation(gpu_context_t *context, vidmm_allocation_t *allocation, unsigned int fill_pattern, unsigned int auto_clear)
{
    gpu_device_t                            *device             = context->device;
    adapter_t                               *adapter            = device->adapter;
    vidmm_mgr_t                             *mm_mgr             = adapter->mm_mgr;
    vidmm_private_build_paging_buffer_arg_t build_paging_buffer = {0};
    vidsch_allocate_paging_task_t           allocate_paging_task= {0,};
    task_paging_t                           *paging_task        = NULL;
    vidmm_map_desc_args_t                   map_args            = {0, };
    int result = S_OK;

    zx_allocation_trace_event(allocation->adapter->index, allocation->parent, ALLOCATION_EVENT_FILL_BEGIN, fill_pattern);

    if (adapter->hw_caps.share_pcie_enable || allocation->pages_mem)
    {
        map_args.size = allocation->size;
        vidmm_vm_map_gpu_virtual_address(device, allocation, &map_args);
        vidmm_allocation_update_mapping(device, context, allocation, NULL);
        allocate_paging_task.dma_size       = 4096;
    }
    else if (allocation->fb_pages_mem)
    {
        allocate_paging_task.dma_size = 4096 * allocation->fb_pages_mem->range_node_num;
    }
    allocate_paging_task.allocation_num = 1;
    paging_task = vidsch_allocate_paging_task(context, &allocate_paging_task);

    paging_task->flag.paging_fill              = 1;
    paging_task->paging_allocation_list[0].allocation = allocation;
    paging_task->paging_allocation_num         = 1;
    build_paging_buffer.allocation             = allocation;
    build_paging_buffer.operation              = BUILDING_PAGING_OPERATION_FILL;
    build_paging_buffer.multi_pass_offset      = 0;
    build_paging_buffer.fill.fill_pattern      = fill_pattern;
    build_paging_buffer.dma_buffer             = paging_task->dma->cpu_virtual_address;
    build_paging_buffer.dma_size               = paging_task->dma->size;

    if (adapter->hw_caps.share_pcie_enable || allocation->pages_mem)
    {
        build_paging_buffer.fill.fill_size         = allocation->size;
        build_paging_buffer.fill.gpu_virt_addr     = map_args.out_va;
        result = mm_mgr->chip_func->build_paging_buffer(adapter, &build_paging_buffer);
        zx_assert(result == S_OK, "");
    }
    else if (allocation->fb_pages_mem)
    {
        int i;
        for (i = 0; i < allocation->fb_pages_mem->range_node_num; i++)
        {
            build_paging_buffer.fill.fill_size         = allocation->fb_pages_mem->range_node_list[i]->size;
            build_paging_buffer.fill.gpu_virt_addr     = allocation->fb_pages_mem->range_node_list[i]->start;
            result = mm_mgr->chip_func->build_paging_buffer(adapter, &build_paging_buffer);
            zx_assert(result == S_OK, "");
        }
    }

    paging_task->dma->command_length = paging_task->dma->size - build_paging_buffer.dma_size;

    //zx_info("^^^ fill_allocation command_length=%d, pages_mem:%p, fb_pages_mem:%p, range_node_cnt:%d\n",
    //        paging_task->dma->command_length,allocation->pages_mem, allocation->fb_pages_mem,
    //        allocation->fb_pages_mem ? allocation->fb_pages_mem->range_node_num : 0);

    vidsch_submit_paging_task(adapter, paging_task);

    //force wait fence back.
    vidsch_wait_fence_back(adapter, adapter->paging_engine_index, paging_task->desc.fence_id, TRUE);

    vidsch_task_dec_reference(&paging_task->desc);

    if (adapter->hw_caps.share_pcie_enable || allocation->pages_mem)
    {
        vidmm_vm_free_gpu_virtual_address(device->vm, map_args.out_va, allocation->size);
    }

    zx_allocation_trace_event(allocation->adapter->index, allocation->parent, ALLOCATION_EVENT_FILL_END, 0);

    return S_OK;
}

#ifdef ZX_VMI_MODE
#define MAX_ONCE_ALLOC_FB_SIZE (1024 * 1024)// max alloc 1MB continuous fb memory
#else
#define MAX_ONCE_ALLOC_FB_SIZE (8 * 1024 * 1024)// max alloc 8MB continuous fb memory
#endif
#define PRE_ALLOC_RANGE_NODE_LIST_SIZE 32 // 8k*8k*4/8M

#define RESERVE_FOR_PT_BUFFER 0x1000000 //reserve 16M local for page table buffer
static fb_pages_memory_t* vidmmi_allocate_fb_page_memory(vidmm_mgr_t *mm_mgr, vidmm_segment_t *fb_segment, unsigned long long size, unsigned int alignment, int direction, int continuous, int cpu_visible, int need_reserve)
{
    adapter_t                   *adapter         = mm_mgr->adapter;
    fb_pages_memory_t           *fb_pages_memory = NULL;
    struct range_node           *range      = NULL;
    unsigned long long reserved_size = adapter->ctl_flags.page_table_pcie_enable ? 0 : RESERVE_FOR_PT_BUFFER;

    fb_pages_memory = zx_calloc(sizeof(fb_pages_memory_t));
    zx_assert(fb_segment->flags.local, "");

#define TRY_ALLOC(SIZE) ({ \
    struct range_node *__range = NULL; \
    if (!cpu_visible && (adapter->total_local_memory_size > adapter->visible_local_memory_size)) \
            __range = fb_segment->mem_ra->alloc(fb_segment->mem_ra, (SIZE), alignment, adapter->visible_local_memory_size, adapter->total_local_memory_size, direction ? RANGE_CREATE_FROM_TOP : 0); \
    if (!__range) \
            __range = fb_segment->mem_ra->alloc(fb_segment->mem_ra, (SIZE), alignment, 0, cpu_visible ? adapter->visible_local_memory_size : adapter->total_local_memory_size, direction ? RANGE_CREATE_FROM_TOP : 0); \
    __range; \
    })

    if(continuous)
    {
        zx_mutex_lock(fb_segment->lock);
        if (!need_reserve || (size + reserved_size <= fb_segment->mem_ra->free_size))
        {
            range = TRY_ALLOC(size);
        }
        else
        {
            range = NULL;
        }
        zx_mutex_unlock(fb_segment->lock);
        if(!range)
        {
            goto failed;
        }

        fb_pages_memory->range_node_num  = 1;
        fb_pages_memory->range_node_list = zx_calloc(sizeof(struct range_node*) * fb_pages_memory->range_node_num);
        fb_pages_memory->range_node_list[0] = range;
        fb_pages_memory->size   = range->size;
    }
    else
    {
        unsigned long long total_size = 0;
        unsigned long long remain_pages = size >> 12;
        unsigned long long alloc_pages = remain_pages;
        struct range_node  **range_node_list = NULL;
        int range_node_index = 0, range_node_list_size = 0;

        zx_assert((size & 4095) == 0, "alignment mismatch");
        if (alignment < 4096)
            alignment = 4096;

        range_node_list_size = PRE_ALLOC_RANGE_NODE_LIST_SIZE;
        range_node_list = zx_calloc(sizeof(struct range_node*) * range_node_list_size);
        range_node_index = 0;

        alloc_pages = MAX_ONCE_ALLOC_FB_SIZE >> 12;
        while(remain_pages)
        {
#define __MIN__(a,b)    ((a) < (b) ? (a) : (b))
            alloc_pages = __MIN__(alloc_pages, __MIN__(remain_pages, MAX_ONCE_ALLOC_FB_SIZE >> 12));
#undef __MIN__

            zx_mutex_lock(fb_segment->lock);
            if (!need_reserve || ((alloc_pages << 12) + reserved_size <= fb_segment->mem_ra->free_size))
            {
                range = TRY_ALLOC(alloc_pages << 12);
            }
            else
            {
                range = NULL;
            }
            zx_mutex_unlock(fb_segment->lock);

            if (range)
            {
                if (range_node_index >= range_node_list_size)
                {
                    int new_list_size = range_node_list_size + PRE_ALLOC_RANGE_NODE_LIST_SIZE;
                    struct range_node **new_node_list = zx_calloc(sizeof(*new_node_list) * new_list_size);

                    zx_memcpy(new_node_list, range_node_list, range_node_list_size * sizeof(range_node_list[0]));
                    range_node_list_size = new_list_size;
                    zx_free(range_node_list);
                    range_node_list = new_node_list;
                }

                range_node_list[range_node_index++] = range;
                remain_pages -= alloc_pages;
                total_size += range->size;
            }
            else
            {
                if (alloc_pages > 1)
                {
                    alloc_pages = (alloc_pages + 1) >> 1;
                }
                else
                {
                    // failed
                    zx_mutex_lock(fb_segment->lock);
                    while(range_node_index--)
                    {
                        range_node_list[range_node_index]->release(range_node_list[range_node_index]);
                    }
                    zx_mutex_unlock(fb_segment->lock);
                    zx_free(range_node_list);
                    range_node_list = NULL;
                    goto failed;
                }
            }
        }
        fb_pages_memory->range_node_list = range_node_list;
        fb_pages_memory->range_node_num = range_node_index;
        fb_pages_memory->size = total_size;
    }

    return fb_pages_memory;

failed:
    zx_free(fb_pages_memory);
    return NULL;
}

static void *vidmmi_allocate_physical_memory(vidmm_mgr_t *mm_mgr, unsigned int segment_id,
                                             vidmm_allocation_t *allocation, unsigned int direction,
                                             unsigned int must_succeed)
{
    void *pages              = NULL;
    adapter_t       *adapter = mm_mgr->adapter;
    vidmm_segment_t *segment = &mm_mgr->segment[segment_id];
    alloc_pages_flags_t flags = {0};

    if(segment->flags.local)
    {
        pages = vidmmi_allocate_fb_page_memory(mm_mgr, segment, allocation->size, allocation->alignment, \
                                                    direction,
                                                    allocation->flag.global || allocation->flag.continuous,
                                                    allocation->flag.cpu_visible, 1);
        if (!pages)
        {
            return NULL;
        }

        allocation->fb_pages_mem = pages;
        allocation->aligned_page_size = GPU_PAGE_SIZE;

        zx_mutex_lock(segment->lock);
        segment->used_size += allocation->size;
        zx_mutex_unlock(segment->lock);
    }
    else
    {
        /* For unsnoop mem, need do flush firstly */
        flags.dma32        = !adapter->hw_caps.address_mode_64bit;
        flags.need_flush   = !segment->flags.support_snoop;
        flags.fixed_page   = TRUE;
        flags.need_dma_map = segment->flags.support_snoop;

        if(allocation->get_system_pages)
            pages = allocation->get_system_pages(allocation->parent, adapter->os_device.pdev, allocation->size, &flags);

        if(!pages)
        {
            return NULL;
        }

        allocation->pages_mem = pages;
        allocation->aligned_page_size = adapter->os_page_size;
    }

    if(allocation->fb_pages_mem || allocation->pages_mem)
    {
        allocation->segment_id = segment_id;
    }

    return pages;
}

/*
* vidmm_destroy_segment_memory:
* 1, release the vm node for the segment memory
* 2, free page memory for the segment memory
* 3, unmap the vm node to the page memory
*/
void vidmm_destroy_segment_memory(adapter_t *adapter, vidmm_segment_memory_t *segment_memory)
{
    vidmm_mgr_t       *mm_mgr     = adapter->mm_mgr;
    vidmm_segment_t   *segment    = &mm_mgr->segment[segment_memory->segment_id];
    gpu_device_t* gpu_device = (gpu_device_t*)adapter->reserved_device;
    int i = 0;

    if(segment_memory->gpu_va != 0)
    {
        //unmap segment memory
        vidmm_unmap_segment_memory(segment_memory);
    }

    if (segment_memory->krnl_cpu_vma)
    {
        vidmm_unlock_segment_memory(adapter, segment_memory);
    }

    if(segment_memory->flag.local == 0)
    {
        zx_assert(segment_memory->pages_mem != NULL, "");
        zx_free_pages_memory(adapter->os_device.pdev, segment_memory->pages_mem);
    }
    else
    {
        fb_pages_memory_t  *fb_pages_mem  = segment_memory->fb_pages_mem;
        zx_assert(fb_pages_mem != NULL, "");

        if(fb_pages_mem->range_node_num){
            zx_mutex_lock(segment->lock);
            for(i=0; i<fb_pages_mem->range_node_num; i++){
                fb_pages_mem->range_node_list[i]->release(fb_pages_mem->range_node_list[i]);
            }
            zx_mutex_unlock(segment->lock);
            zx_free(fb_pages_mem->range_node_list);
            fb_pages_mem->range_node_list = NULL;
            fb_pages_mem->range_node_num  = 0;
        }
        zx_free(fb_pages_mem);
    }

    zx_mutex_lock(segment->lock);
    segment->used_size -= segment_memory->size;
    segment->used_segment_memory_size -= segment_memory->size;
    zx_mutex_unlock(segment->lock);

    zx_free(segment_memory);
}


/*
* vidmm_create_segment_memory:
* 1, allocate the vm node for the segment memory
* 2, allocate page memory for the segment memory
* 3, map the vm node to the page memory
*
* @adapter: the physical adapter of this segment resident in.
* @segment_id: indicates the physical memory resident heap.
* @size: the size of the segment memory.
* @direction: the direction when allocate the vm node in heap. suit for heap algorithm.
* @shared: indicates the segment memory be shared.
*/
vidmm_segment_memory_t *vidmm_create_segment_memory(adapter_t *adapter, int segment_id, unsigned long long size, int direction, int continuous)
{
    vidmm_mgr_t                 *mm_mgr          = adapter->mm_mgr;
    vidmm_segment_t             *segment         = &mm_mgr->segment[segment_id];
    vidmm_segment_memory_t      *segment_memory  = NULL;
    int res = S_OK;

    segment_memory = zx_calloc(sizeof(vidmm_segment_memory_t));
    segment_memory->segment_id = segment_id;

    /* allocate system pages  */
    if(segment->flags.local == 0)
    {
        struct os_pages_memory *pages_mem   = NULL;
        alloc_pages_flags_t     alloc_flags = {0};

        alloc_flags.dma32      = !adapter->hw_caps.address_mode_64bit;
        alloc_flags.need_flush = !segment->flags.support_snoop;
        alloc_flags.fixed_page = TRUE;
        alloc_flags.need_dma_map = segment->flags.support_snoop;

        pages_mem = zx_allocate_pages_memory(adapter->os_device.pdev, size, adapter->os_page_size, alloc_flags);
        if(pages_mem == NULL)
        {
            zx_error("%s(): allocate pages failed with size: %d\n", __func__, size);
            //add error info: allocate pa failed @system
            goto allocate_fail;
        }

        segment_memory->pages_mem = pages_mem;
        segment_memory->size      = util_align( size,  adapter->os_page_size);
        segment_memory->aligned_page_size = adapter->os_page_size;
    }
    else//allocate fb physical memory
    {
        int try_times = 10, paging_result = 0;
        fb_pages_memory_t *fb_page_memory  = NULL;
retry:
        fb_page_memory = vidmmi_allocate_fb_page_memory(mm_mgr,segment, size, GPU_PAGE_SIZE, direction, continuous, 1, 0);
        if(fb_page_memory == NULL)
        {
            if (try_times == 0)
            {
                goto allocate_fail;
            }

            // try paging out some allocation
            paging_result = vidmmi_segment_unresident_allocations(mm_mgr, mm_mgr->segment + 1);
            if (paging_result == S_OK)
            {
                --try_times;
                goto retry;
            }
        }
        else
        {
            segment_memory->fb_pages_mem = fb_page_memory;
            segment_memory->size         = fb_page_memory->size;
            segment_memory->aligned_page_size    = GPU_PAGE_SIZE;//segment for local all use 4K page
            segment_memory->flag.local   = 1;
        }
    }
    segment_memory->flag.snooping_enabled = segment->flags.support_snoop ? 1 : 0;

    zx_mutex_lock(segment->lock);
    segment->used_size += segment_memory->size;
    segment->used_segment_memory_size += segment_memory->size;
    zx_mutex_unlock(segment->lock);

    return segment_memory;

allocate_fail:
    zx_free(segment_memory);
    return NULL;
}


static int vidmm_allocate_physical_memory(vidmm_mgr_t *mm_mgr, vidmm_allocation_t *allocation, unsigned int must_succeed)
{
    vidmm_segment_preference_t *preferred_segment = allocation->preferred_segment;
    void  *phys_mem = NULL;
    unsigned int segment_id;
    unsigned int direction;
    int i;

    zx_allocation_trace_event(allocation->adapter->index, allocation->parent, ALLOCATION_EVENT_ALLOC_BEGIN, allocation->segment_id);

    for (i = 0; !phys_mem && i < ARRAY_SIZE(allocation->preferred_segment); i++)
    {
        segment_id = preferred_segment[i].segment_id;
        direction  = preferred_segment[i].direction;

        if (SEGMENT_ID_INVALID != segment_id)
            phys_mem = vidmmi_allocate_physical_memory(mm_mgr, segment_id, allocation, \
                                                                direction, must_succeed);
    }

    zx_allocation_trace_event(allocation->adapter->index, allocation->parent, ALLOCATION_EVENT_ALLOC_END, allocation->segment_id);

    if(!phys_mem)
        return E_OUTOFMEMORY;

    return S_OK;
}

/*
 * vidmmi_allocate_fb_memroy_try: try muti segment_id specified on prefreered_segment.
 */
int vidmmi_allocate_fb_memory_try(vidmm_mgr_t *mm_mgr, vidmm_allocation_t *allocation)
{
    fb_pages_memory_t *fb_pages_memory = NULL;
    int                segment_id      = 0;
    int                direction       = 0;
    int                result          = E_FAIL;
    int                i               = 0;
    unsigned int       continuous = allocation->flag.global || allocation->flag.continuous;
    vidmm_segment_preference_t *preferred_segment = allocation->preferred_segment;

    for (i = 0; !fb_pages_memory && i < ARRAY_SIZE(allocation->preferred_segment); i++)
    {
        segment_id = preferred_segment[i].segment_id;
        direction  = preferred_segment[i].direction;

        if(SEGMENT_ID_INVALID != segment_id && mm_mgr->segment[segment_id].flags.local)
        {
            fb_pages_memory = vidmmi_allocate_fb_page_memory(mm_mgr, mm_mgr->segment + segment_id, allocation->size, allocation->alignment, \
                    direction, continuous, allocation->flag.cpu_visible, 0);
        }
    }

    if(fb_pages_memory != NULL)
    {
        allocation->segment_id   = segment_id;
        allocation->fb_pages_mem = fb_pages_memory;
        allocation->aligned_page_size = GPU_PAGE_SIZE;
        result = S_OK;
    }

    return result;
}

unsigned long long vidmm_get_segment_memory_gpuva_cont(vidmm_segment_memory_t *segment_memory, unsigned int page_index)
{
    unsigned long long offset = page_index * GPU_PAGE_SIZE;

    return segment_memory->gpu_va + offset;
}


/*
vidmm_get_segment_memory_pa_cont: return physical memory offset of the segment_memory with continue memory.
*/

unsigned long long vidmm_get_segment_memory_pa_cont(vidmm_segment_memory_t *segment_memory, unsigned int page_index)
{
    struct os_pages_memory *pages_mem      = NULL;
    fb_pages_memory_t      *fb_pages_mem   = NULL;
    unsigned long long                pte_pa         = 0;

    if(segment_memory->flag.local == 0)
    {
        pages_mem = segment_memory->pages_mem;
        zx_assert(pages_mem != NULL, "");

        pte_pa = zx_get_page_phys_address(pages_mem, page_index, NULL);
    }
    else
    {
        unsigned long long offset = page_index * segment_memory->aligned_page_size;
        fb_pages_mem = segment_memory->fb_pages_mem;

        pte_pa = fb_pages_mem->range_node_list[0]->start + offset;
    }

    return pte_pa;
}


void *vidmm_get_segment_memory_cpu_va(vidmm_segment_memory_t *segment_memory)
{
    zx_assert(segment_memory != NULL, "");
    return segment_memory->krnl_cpu_vma->virt_addr;
}

void vidmmi_release_system_memory(adapter_t *adapter, vidmm_allocation_t *allocation)
{
    vidmm_mgr_t *mm_mgr    = adapter->mm_mgr;

    if(allocation->pages_mem)
    {
        unsigned int page_num = ((allocation->size + adapter->os_page_size - 1) >> adapter->os_page_shift);

        allocation->release_system_pages(adapter->os_device.pdev, allocation->pages_mem);

        zx_mutex_lock(mm_mgr->mgr_lock);
        mm_mgr->used_pages_num -= page_num;
        zx_mutex_unlock(mm_mgr->mgr_lock);

        allocation->pages_mem = NULL;
    }
}

/*
* vidmm_map_segment_memory: map segment memory with a vm node.
* 1, allocate vm node for the segment memory with full-size
* 2, va->pa mapping for the segment memory
* 3, wait the update vm operation completed (optional).
*
* @vma_space: the vma space where the segment memory map to.
* @segment_memory: the mapped segment memory.
*
*/
int vidmm_map_segment_memory(adapter_t *adapter, vidmm_segment_memory_t *segment_memory)
{
    int             result  = S_OK;

    if(segment_memory->gpu_va == 0)
    {
        result = vidmm_vm_allocate_segment_memory(adapter, segment_memory);
        if(result != S_OK)
        {
            //add error info: allocate va failed
            zx_error("%s: vm segment memory allocate failed result:%x.\n", __func__, result);
            goto exit;
        }
    }
    result = adapter->mm_mgr->chip_func->map_shared_buffer(adapter, ZX_VM_NODE_TYPE_SEGMENET_MEMORY, segment_memory, 0, segment_memory->size, segment_memory->gpu_va);
exit:
    return result;
}


void vidmm_unmap_segment_memory(vidmm_segment_memory_t *segment_memory)
{
    zx_vm_node_t *node = segment_memory->gpu_vm;
    zx_vm_zone_t *zone = NULL;

    if (node)
    {
        zone = node->zone;
        zx_mutex_lock(zone->lock);
        vidmm_vm_release_segment_memory(segment_memory);
        zx_mutex_unlock(zone->lock);
    }
}

// assume allocation->paging_lock hold
int vidmm_make_resident_locked(vidmm_allocation_t *allocation)
{
    int             ret = 0;
    adapter_t       *adapter    = allocation->adapter;
    vidmm_mgr_t     *mm_mgr     = adapter->mm_mgr;
    vidmm_segment_t *segment    = NULL;

    if (allocation->segment_id > 0)
    {
        return 0;
    }

    if (allocation->file_storage || allocation->pages_mem)
    {
        ret = vidmm_resident_one_allocation(adapter, allocation);
    }
    else if (allocation->fb_pages_mem)
    {
        zx_warning("allocation %p has fb_pages_mem, but segment_id = 0, maybe a bug.\n", allocation);
    }
    else
    {
        int segment_id;
        int try_times = 0, paging_result = 0;
retry:
        ret = vidmm_allocate_physical_memory(adapter->mm_mgr, allocation, 1);
        if (ret == S_OK)
        {
            vidmm_add_allocation_to_segment_resident_list(adapter, allocation);

            ++allocation->resident_age;

            if (allocation->flag.force_clear)
            {
                vidmm_fill_allocation(adapter->reserved_context, allocation, 0, 0);

                allocation->flag.force_clear = 0;
            }
        }
        else if (!adapter->hw_caps.local_only && ret == E_OUTOFMEMORY && try_times < 100)
        {
            int i;
            vidmm_segment_preference_t *preferred_segment = allocation->preferred_segment;
            for (i = 0; i < ARRAY_SIZE(allocation->preferred_segment); i++)
            {
                segment_id = preferred_segment[i].segment_id;
                segment = &mm_mgr->segment[segment_id];
                if (segment_id && segment->flags.local)
                {
                    paging_result = vidmmi_segment_unresident_allocations(mm_mgr, segment);
                    if (paging_result == S_OK)
                    {
                        ++try_times;
                        goto retry;
                    }
                }
            }
        }

        if (ret != S_OK)
        {
            int i;
            int segment_count = ARRAY_SIZE(allocation->preferred_segment);

            zx_error("%s: faild to allocate segment memory, pool_type:%d cache_type:%d cpu_visible:%d.\n",
                    __func__, allocation->flag.pool_type, allocation->flag.cache_type, allocation->flag.cpu_visible);
            zx_error("preferred_segment count: %d, dump preferred_segment info:\n", segment_count);
            for (i = 0; i < segment_count; i++)
            {
                segment_id = allocation->preferred_segment[i].segment_id;
                zx_error("    segment_id[%d]: %d\n", i, segment_id);
            }
        }
    }
    return ret;
}

static int vidmmi_allocation_update_mapping(gpu_device_t *device, gpu_context_t *context, vidmm_allocation_t *allocation, task_vm_update_t **update_vm_task)
{
    int                 result = 0;
    zx_vm_node_t        *vm_node = NULL;
    adapter_t           *adapter = device->adapter;
    zx_vma_space_t      *vma_space = device->vm;

    zx_mutex_lock(allocation->va_lock);

    list_for_each_entry(vm_node, &allocation->gpu_vm_list, mapped_list_item)
    {
        if (vm_node->device != device)
        {
            // ignore this node
            continue;
        }

        if (vm_node->age >= allocation->resident_age)
        {
            // already mapping
            continue;
        }

        if (vma_space == adapter->reserved_vma)
        {
            zx_assert(vm_node->zone->vma_space == adapter->reserved_vma, "");

            // share space can't contain private node
            zx_assert(vm_node->zone->zone_id != VM_ZONE_ID_PRIVATE_SPACE, "");
        }

        if (vm_node->zone->vma_space == adapter->reserved_vma)
        {
            unsigned long long gpu_va, size;

            vidmmi_vm_get_node_info(vm_node, &gpu_va, &size);

            adapter->mm_mgr->chip_func->map_shared_buffer(adapter, ZX_VM_NODE_TYPE_ALLOCATION, allocation, vm_node->offset, size, gpu_va);
        }
        else
        {
            zx_assert(vm_node->zone->vma_space == vma_space, "");
            zx_assert(update_vm_task != NULL, "");

            if (!(*update_vm_task))
            {
                vidsch_allocate_vm_update_task_t update_vm_arg = {0, };
                update_vm_arg.dma_size          = MAX_UPDATE_VM_DMA_SIZE;
                *update_vm_task = vidsch_allocate_vm_update_task(adapter->reserved_context, &update_vm_arg);
                if (!(*update_vm_task))
                {
                    result = -1;
                    goto done;
                }
            }

            result = vidmm_vm_update_mapping(vma_space, context, vm_node, allocation, *update_vm_task);

            if (IS_VIDEO_ENGINE(adapter->chip_id, context->rb_index))
            {
            //    zx_info("vm_update_mapping for engine%d, range[%llx - %llx], fence_value:%lld\n",
            //            context_update->rb_index, vm_node->range->start,
            //            vm_node->range->start + vm_node->range->size, fence_value);

                vm_node->range->tag = allocation->resident_age;
            }
            if (result != 0)
            {
                zx_info("vidmm_vm_update_mapping(device:%p) failed with %d.\n", device, result);
                goto done;
            }
        }
        vm_node->age = allocation->resident_age;
    }

done:
    zx_mutex_unlock(allocation->va_lock);
    return result;
}

int vidmm_allocation_update_mapping(gpu_device_t *device, gpu_context_t *context, vidmm_allocation_t *allocation, unsigned long long *gpu_va)
{
    int                 result = 0;
    adapter_t           *adapter = device->adapter;

    if (context == adapter->reserved_context)
    {
        if (allocation->need_remap && gpu_va)
        {
            vidmm_map_desc_args_t map_args = {0, };

            map_args.size = allocation->size;
            vidmm_vm_map_gpu_virtual_address(device, allocation, &map_args);

            *gpu_va = map_args.out_va;
            allocation->need_remap = 0;
        }

        result = vidmmi_allocation_update_mapping(device, context, allocation, NULL);
    }
    else
    {
        task_vm_update_t *update_vm_task = NULL;

        result = vidmmi_allocation_update_mapping(device, context, allocation, &update_vm_task);
        if (result == 0 && update_vm_task)
        {
            vidsch_submit_vm_update_task(adapter, update_vm_task);
        }

        if (update_vm_task)
        {
            vidsch_task_dec_reference(&update_vm_task->desc);
        }
    }

    return result;
}

int vidmm_allocation_list_update_mapping(gpu_device_t *device, gpu_context_t *context, unsigned int list_count, struct _vidmm_allocation  **alloction_list)
{
    int i;
    int                 result            = 0;
    adapter_t           *adapter          = device->adapter;
    gpu_context_t       *reserved_context = adapter->reserved_context;
    int                 reserved_rb_index = reserved_context->rb_index;
    task_vm_update_t    *update_vm_task   = NULL;

    for (i = 0; i < list_count; i++)
    {
        vidmm_allocation_t *allocation = alloction_list[i];

        if (!allocation)
            continue;

        result = vidmm_prepare_and_mark_unpagable(allocation);
        if (result != 0)
            goto exit;

        result = vidmmi_allocation_update_mapping(device, context, allocation, &update_vm_task);
        if (result != 0)
            goto exit;

        if (update_vm_task && update_vm_task->dma_size > 1024 * 4096 && i != list_count - 1)
        {
            vidsch_submit_vm_update_task(adapter, update_vm_task);

            vidsch_task_dec_reference(&update_vm_task->desc);

            update_vm_task = NULL;
        }
    }

    if (update_vm_task)
    {
        vidsch_submit_vm_update_task(adapter, update_vm_task);
    }

exit:
    if (update_vm_task)
    {
        vidsch_task_dec_reference(&update_vm_task->desc);
    }

    return result;
}

void vidmm_destroy_allocation(vidmm_allocation_t *allocation, zx_destroy_flag_t *flags)
{
    vidmm_do_destroy_allocation(allocation);
}

void vidmm_update_heap_slot(adapter_t *adapter, krnl_cil2_misc_t *misc)
{
    unsigned int bl_size = misc->zx_misc->update_bl_slot.size >> 9;
    unsigned long long node_start = 0;
    struct range_node *bl_node = NULL;
    bl_slot_t *bl_slot = NULL;

    //allocate bl slot
    if(!misc->zx_misc->update_bl_slot.delFlag)
    {
        zx_mutex_lock(adapter->mm_mgr->mgr_lock);
        bl_node = adapter->mm_mgr->bl_ra->alloc(adapter->mm_mgr->bl_ra, bl_size, BL_SLICE_ALIGNMENT, 0, -1, 0);
        zx_mutex_unlock(adapter->mm_mgr->mgr_lock);
        misc->zx_misc->update_bl_slot.bl_slot_index = zx_do_div(bl_node->start - adapter->mm_mgr->bl_ra->start, BL_SLICE_ALIGNMENT); //slice index

        bl_slot = zx_calloc(sizeof(bl_slot_t));
        if(bl_slot)
        {
            bl_slot->bl_node = bl_node;
            list_add_tail(&bl_slot->list_node, &misc->device->bl_slot_list);
            bl_node->private = (void*)bl_slot;
        }
    }
    else
    {
        //delete bl slot
        node_start = (misc->zx_misc->update_bl_slot.bl_slot_index*BL_SLICE_ALIGNMENT) + adapter->mm_mgr->bl_ra->start;
        zx_mutex_lock(adapter->mm_mgr->mgr_lock);
        bl_node = adapter->mm_mgr->bl_ra->lookup(adapter->mm_mgr->bl_ra, node_start, bl_size);

        if(!bl_node || !bl_node->private)
        {
            zx_warning("zx_drm_mm_ra_lookup failed!!! or bl_node has no bl_slot!!!\n");
            zx_mutex_unlock(adapter->mm_mgr->mgr_lock);
            return;
        }

        bl_slot = (bl_slot_t*)bl_node->private;
        list_del(&bl_slot->list_node);
        zx_free(bl_slot);
        bl_node->release(bl_node);
        zx_mutex_unlock(adapter->mm_mgr->mgr_lock);
    }
}
