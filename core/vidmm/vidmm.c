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
#include "context.h"

#if 1
void vidmmi_test_video_memory(adapter_t *adapter)
{
       unsigned int  offset;
       volatile unsigned int *frame_buffer;
       unsigned int save_data;
       unsigned int dummy_data = 0xFEFECDEF;
       unsigned int  result     = S_OK;
       unsigned long long  scan_scope = adapter->visible_local_memory_size;//0x400000;//adapter->physical_bus_base_length[1];//cpu visiable
       vidmm_mgr_t   *mm_mgr    = adapter->mm_mgr;
       unsigned int  scan_line  = 0x400000;//remapio will assert in vmalloc if size is larger than vmalloc total size. add to avoid this. or can add vmalloc='scan_scope' in boot manu
       unsigned int  scan_loop  = 0;
       unsigned int  map_size   = scan_line;
       zx_map_argu_t    map    = {0};
       zx_cpu_vm_area_t     *vma   = NULL;
       unsigned int count = 0;

       zx_info("==================test local memory begin==========================\n");
       zx_info("total size = 0x%llx, vidmm_bus_addr=%x \n", scan_scope, adapter->vidmm_bus_addr);

       map.node_num        = 1;
       //test local video memory
       for(scan_loop=0; scan_loop<scan_scope; scan_loop+=scan_line)
       {
           map_size = (scan_scope-scan_loop) < scan_line ?  scan_scope-scan_loop : scan_line;
           {
               map.flags.cache_type= ZX_MEM_UNCACHED;
               map.flags.mem_type  = ZX_SYSTEM_IO;
               map.phys_addr       = adapter->vidmm_bus_addr+scan_loop;
               map.size            = map_size;
               map.node_num        = 1;
               vma = zx_map_io_memory(&map);

               zx_assert(vma!=NULL, "");

               frame_buffer = vma->virt_addr;
           }

           for(offset=0; offset<map_size/4; offset++)
           {
               dummy_data = offset;
               save_data = frame_buffer[offset];
               frame_buffer[offset] = dummy_data;
               dummy_data = frame_buffer[offset];
               frame_buffer[offset] = save_data;

               if(dummy_data != offset)
               {
                   zx_info("test video momory failed: PA=%x, value=%x, expected:%x\n",(adapter->vidmm_bus_addr + scan_scope+ offset*4),dummy_data,offset);
                   if(count++ >500) break;
               }
           }

           {
               zx_unmap_io_memory(vma);
               vma = NULL;
           }
           zx_info("passed %xth 4M ~~~ %xth 4M.\n", scan_loop/scan_line, scan_loop/scan_line+1);
       }

       zx_info("==================test local memory end==========================\n");
}

#endif

#define SYSTEM_RESERVED_PAGES_SIZE  (256 * 1024 * 1024)
#define EMERENCY_PAGES_SIZE         ( 64 * 1024 * 1024)


/*
* vidmm_init: initialize the vidmm module.
* @adapter: the physical adapter of the vidmm.
*/
int vidmm_init(adapter_t *adapter, int reserved_vmem)
{
    vidmm_mgr_t                  *mm_mgr         = NULL;

    int            result      = S_OK;
    int            id          = 0;
    int            priority    = 0;
    int            max_size    = 0;
    mem_info_t     mem         = {0};

    mm_mgr = zx_calloc(sizeof(vidmm_mgr_t));
    if(NULL == mm_mgr)
    {
        zx_error("vidmm_init failed.\n");
        result = E_OUTOFMEMORY;
        goto exit_out_of_memory;
    }

    adapter->mm_mgr       = mm_mgr;
    mm_mgr->adapter       = adapter;
    mm_mgr->reserved_vmem = reserved_vmem;
    mm_mgr->mgr_lock      = zx_create_mutex();
    mm_mgr->list_lock     = zx_create_mutex();

    list_init_head(&mm_mgr->defer_destroy_list);

    /* calc max use pages size by real system memory info to avoid use much system memory */

    zx_get_mem_info(&mem);

    if(mem.totalram > 2 * SYSTEM_RESERVED_PAGES_SIZE/adapter->os_page_size)
    {
        mm_mgr->max_pages_num   = mem.totalram - ((SYSTEM_RESERVED_PAGES_SIZE - EMERENCY_PAGES_SIZE)/adapter->os_page_size);
        mm_mgr->emergency_pages = EMERENCY_PAGES_SIZE/adapter->os_page_size;
    }
    else
    {
        mm_mgr->max_pages_num   = mem.totalram >> 1;
        mm_mgr->emergency_pages = mem.totalram >> 2;
    }

#ifdef ANDROID
    {
        mm_mgr->max_pages_num   = mem.totalram;
        mm_mgr->emergency_pages = mem.totalram;
    }
#endif

    zx_info("GPU can used system memory size: %dk. set GPU max used size as: %dk.\n",
         mem.totalram * adapter->os_page_size>> 10,  mm_mgr->max_pages_num * adapter->os_page_size>> 10);

    mm_mgr->chip_func = &vidmm_chip_func;

    /*
     init vm info
    */
    mm_mgr->vm_info = zx_calloc(sizeof(vidmm_chip_vm_info_t));
    if(NULL == mm_mgr->vm_info)
    {
        zx_error("mm_mgr->vm_info init failed.\n");
        result = E_OUTOFMEMORY;
        goto exit_out_of_memory;
    }

    mm_mgr->chip_func->query_chip_vm_info(adapter, mm_mgr->vm_info);

    vidmmi_dump_vm_info(mm_mgr->vm_info);

    mm_mgr->chip_func->init_segments(adapter);

    //init the paging mgr
    mm_mgr->paging_mgr = zx_calloc(sizeof(vidmm_paging_mgr_t));
    if(NULL == mm_mgr->paging_mgr)
    {
        zx_error("vidmm_mgr-->paging_mgr init failed.\n");
        result = E_OUTOFMEMORY;
        goto exit_out_of_memory;
    }

    list_init_head(&mm_mgr->paging_mgr->system_allocation_list);

    if(adapter->ctl_flags.swap_enable)
    {
        list_init_head(&mm_mgr->paging_mgr->swapout_allocation_list);
    }

    //init the kmd reserved vma. the vma can be used for the kmd reserved buffer, like the paging, dma, page table, fence, context, etc.
    adapter->reserved_vma = zx_calloc(sizeof(zx_vma_space_t));
    if(NULL == adapter->reserved_vma)
    {
        zx_error("adapter-->reserved_vma init failed.\n");
        result = E_OUTOFMEMORY;
        goto exit_out_of_memory;
    }

    result = vidmm_vm_init_vma_space(adapter->reserved_vma, adapter);
    if(result != S_OK)
    {
        zx_error("first init vma space for mm_mgr-->reserved_vma failed, err code: %x\n", result);
        goto init_vma_fail;
    }

    //patch for cne001. init the private vma for ap. the vma can be used for all ap device.
    if(adapter->chip_id == CHIP_CNE001) {
        adapter->private_vma = zx_calloc(sizeof(zx_vma_space_t));
        if(NULL == adapter->private_vma)
        {
            zx_error("adapter-->private_vma init failed.\n");
            result = E_OUTOFMEMORY;
            goto init_private_vma_fail;
        }

        adapter->private_vm_id = 15;
        result = vidmm_vm_init_vma_space(adapter->private_vma, adapter);
        if(result != S_OK)
        {
            zx_error("first init vma space for mm_mgr-->private_vma failed, err code: %x\n", result);
            goto init_private_vma_fail;
        }
    }

#if DEBUG_TEST
    vidmmi_test_video_memory(adapter);
#endif

    if(mm_mgr->chip_func->init_bl_heap)
    {
        //force reserve BL buffer from local start, hw limitation.
        mm_mgr->bl_reserved_memory = vidmm_create_segment_memory(adapter, 1, BL_BUFFER_SIZE, 0, 1);

        result = vidmm_map_segment_memory(adapter, mm_mgr->bl_reserved_memory);
        if(result != S_OK)
        {
            if(mm_mgr->bl_reserved_memory->gpu_vm != NULL)
            {
                vidmm_unmap_segment_memory(mm_mgr->bl_reserved_memory);
            }
            vidmm_destroy_segment_memory(adapter, mm_mgr->bl_reserved_memory);
            goto init_vma_fail;
        }

        vidmm_lock_segment_memory(adapter, mm_mgr->bl_reserved_memory);

        mm_mgr->chip_func->init_bl_heap(adapter);
    }

    return result;

init_private_vma_fail:
    if (adapter->reserved_vma)
    {
        vidmm_vm_fini_vma_space(adapter->reserved_vma);
        zx_free(adapter->reserved_vma);
    }

exit_out_of_memory:
init_vma_fail:
    if(mm_mgr && mm_mgr->vm_info) zx_free(mm_mgr->vm_info);
    if(mm_mgr && mm_mgr->segment) zx_free(mm_mgr->segment);
    if(mm_mgr && mm_mgr->paging_mgr) zx_free(mm_mgr->paging_mgr);
    if(mm_mgr) zx_free(mm_mgr);

    return result;
}

void vidmm_destroy(adapter_t *adapter)
{
    vidmm_mgr_t     *mm_mgr  = adapter->mm_mgr;
    int              id      = 0;

    if(mm_mgr->chip_func->fini_bl_heap)
    {
        if(mm_mgr->bl_reserved_memory->gpu_vm != NULL)
        {
            vidmm_unmap_segment_memory(mm_mgr->bl_reserved_memory);
        }
        vidmm_destroy_segment_memory(adapter, mm_mgr->bl_reserved_memory);

        mm_mgr->chip_func->fini_bl_heap(adapter);
    }

    if(adapter->private_vma)
    {
        vidmm_vm_fini_vma_space(adapter->private_vma);
        zx_free(adapter->private_vma);
    }

    if(adapter->reserved_vma)
    {
        vidmm_vm_fini_vma_space(adapter->reserved_vma);
        zx_free(adapter->reserved_vma);
    }

    mm_mgr->chip_func->fini_segments(adapter);

#if defined(ZX_HW_NULL)
    if(adapter->mmio_mem)
    {
        zx_unmap_pages_memory(adapter->mmio_vma);
        adapter->mmio_vma = NULL;

        zx_free_pages_memory(adapter->os_device.pdev, adapter->mmio_mem);
        adapter->mmio_mem = NULL;
    }

#endif

    zx_destroy_mutex(mm_mgr->list_lock);
    zx_destroy_mutex(mm_mgr->mgr_lock);

    if(mm_mgr->vm_info) zx_free(mm_mgr->vm_info);
    if(mm_mgr->segment) zx_free(mm_mgr->segment);
    if(mm_mgr->paging_mgr) zx_free(mm_mgr->paging_mgr);

    if(adapter->mm_mgr)
    {
        zx_free(adapter->mm_mgr);
    }
}


void vidmm_save(adapter_t *adapter)
{
    vidmm_mgr_t *mm_mgr = adapter->mm_mgr;

    if(mm_mgr->chip_func->save)
        mm_mgr->chip_func->save(adapter);
}

void vidmm_restore(adapter_t *adapter)
{
    vidmm_mgr_t                 *mm_mgr = adapter->mm_mgr;

    if(mm_mgr->chip_func->restore)
        mm_mgr->chip_func->restore(adapter);

    if (adapter->chip_id == CHIP_CNE001)
    {
        zx_vma_space_t  *private_vma = adapter->private_vma;
        zx_vm_pt_info_t *pt_info     = &private_vma->pt_info;
        vidmm_vm_pt_t *root = &pt_info->root;

        vidmm_vm_alloc_pd(private_vma, root, adapter->shared_gpu_va_size >> GPU_PAGE_SHIFT, (adapter->shared_gpu_va_size + adapter->private_size_of_cne001 - 1) >> GPU_PAGE_SHIFT, ZX_PT_LEVEL_ROOT, 1);

        /* use mmio to invalidate_tlb in private_vma */
        mm_mgr->chip_func->init_private_vma_space(adapter);
    }
}

struct range_allocator* vidmm_get_burst_length_allocator(adapter_t *adapter)
{
    return adapter->mm_mgr->bl_ra;
}

vidmm_segment_t *vidmm_get_segment_by_id(adapter_t *adapter, unsigned int segment_id)
{
    vidmm_mgr_t     *vidmm = adapter->mm_mgr;
    vidmm_segment_t *segment = NULL;

    if((SEGMENT_ID_INVALID == segment_id) || (segment_id >= vidmm->segment_cnt))
    {
        zx_assert(0, "invalid segment_id-%d\n", segment_id);
        return segment;
    }

    segment = &vidmm->segment[segment_id];
    return segment;
}

vidmm_segment_t *vidmm_get_segment_by_property(adapter_t *adapter, unsigned int property)
{
    vidmm_segment_t *segment = NULL;
    unsigned int     seg_id = 0;

    seg_id = vidmmi_get_segment_id_by_property(adapter, property);
    if(!seg_id)
    {
        zx_error("invalid segment property: %x\n", property);
    }
    segment = vidmm_get_segment_by_id(adapter, seg_id);

    return segment;
}

int vidmm_get_segment_count(adapter_t *adapter)
{
    return adapter->mm_mgr->segment_cnt;
}

int vidmm_query_info(adapter_t *adapter, zx_query_info_t *info)
{
    vidmm_mgr_t *mm_mgr  = adapter->mm_mgr;
    int ret = 0;

    switch (info->type)
    {
    case ZX_QUERY_TOTAL_VRAM_SIZE:
        info->value = adapter->total_local_memory_size;
        break;


    case ZX_QUERY_CPU_VISIBLE_VRAM_SIZE:
        info->value = adapter->visible_local_memory_size;
        break;

    case ZX_QUERY_ALLOCATION_RESIDENCY:
    {
        int                     i;
        unsigned int            num_allocations = info->argu;
        vidmm_allocation_t      **alloc_list = ptr64_to_ptr(info->in_buf);
        unsigned int            *resi_status = ptr64_to_ptr(info->buf);

        for(i = 0; i < num_allocations; i++)
        {
            vidmm_allocation_t *allocation = alloc_list[i];

            if(!allocation) continue;

            if(allocation->fb_pages_mem)
                    resi_status[i] = 0;//local
            else if(allocation->pages_mem)
                    resi_status[i] = 1;//non-local
            else
                    resi_status[i] = 2;//non-resident;
        }
        break;
    }
    case ZX_QUERY_ALLOCATION_INFO:
    {
        vidmm_allocation_t    *allocation = ptr64_to_ptr(info->in_buf);
        zx_allocation_info_t  *open_info  = ptr64_to_ptr(info->buf);

        vidmm_fill_allocation_info(adapter, allocation, open_info);
        break;
    }
    case ZX_QUERY_SEGMENT_FREE_SIZE:
    {
        vidmm_segment_t               *segment = NULL;
        unsigned int idx = info->argu;

        segment = &mm_mgr->segment[idx];
        info->value = (segment->size - segment->used_size) >> 10;
        break;
    }
    default:
        ret = -1;
        break;
    }

    return ret;
}

static void vidmmi_dump_allocation(struct os_printer *p, vidmm_allocation_t *allocation)
{
    if(!allocation)
        return;

    zx_printf(p, "allocation: %p, size: %lld, status: %8x, shared %d\n",
        allocation,
        allocation->size,
        allocation->status.temp_unpagable,
        allocation->flag.global);
}

static void vidmmi_dump_allocations(struct os_printer *p, vidmm_mgr_t *mm_mgr, struct list_head *allocation_list)
{
    vidmm_allocation_t *allocation = NULL;

    if(!allocation_list)
        return;

    list_for_each_entry(allocation, allocation_list, list_item)
    {
        vidmmi_dump_allocation(p, allocation);
    }
}

void vidmm_dump_heap(struct os_printer *p, adapter_t *adapter, int id)
{
    vidmm_mgr_t        *mm_mgr     = adapter->mm_mgr;
    vidmm_segment_t    *segment    = NULL;

    if (id >= 0 && id < mm_mgr->segment_cnt)
    {
        segment = mm_mgr->segment + id;

        zx_printf(p, "Total segment size:  %llu\n", segment->size);
        zx_printf(p, "KMD used segment memory:  %llu\n", segment->used_segment_memory_size);

        if (segment->mem_ra)
        {
            zx_mutex_lock(segment->lock);
            segment->mem_ra->dump(p, segment->mem_ra, "");
            zx_mutex_unlock(segment->lock);
        }
    }
}

void vidmm_dump_memtrack(struct os_printer *p, adapter_t *adapter, int pid)
{
    gpu_device_t     *device       = NULL;
    gpu_device_t     *device_next  = NULL;


    unsigned long long total = 0;

    UNUSED(pid);

    zx_printf(p, "Name                               PID              \n");
    zx_printf(p, "====================================================\n");

    zx_mutex_lock(adapter->device_list_lock);

    list_for_each_entry_safe(device, device_next, &adapter->device_list, list_node)
    {
        zx_printf(p, "  %-32s %-16d\n", device->pname, device->pid);
    }

    zx_mutex_unlock(adapter->device_list_lock);

    zx_printf(p, "====================================================\n");

}

void vidmm_dump_resource(struct os_printer *p, adapter_t *adapter)
{
    vidmm_mgr_t        *mm_mgr     = adapter->mm_mgr;
    vidmm_segment_t    *segment    = NULL;


    unsigned int idx, priority;

    for(idx = 0; idx < mm_mgr->segment_cnt; idx++)
    {
        segment = &mm_mgr->segment[idx];

        zx_printf(p, "\n\n");

        zx_mutex_lock(segment->lock);

        zx_printf(p, "**** segment[%d] size: %dK, used:%lldk, allocation_num:%d\n", idx,
             segment->size >> 10, segment->used_size >> 10, segment->used_num);

        if(segment->flags.local)
        {
            segment->mem_ra->dump(p, segment->mem_ra, "  ");
        }

        for(priority = 0; priority < PALL; priority++)
        {
            zx_printf(p, "----pagable priority: %d.\n", priority);

            vidmmi_dump_allocations(p, mm_mgr, &segment->pagable_resident_list[priority]);
        }
        zx_printf(p, "----unpagable.\n", priority);

        vidmmi_dump_allocations(p, mm_mgr, &segment->unpagable_resident_list);

        zx_mutex_unlock(segment->lock);

        zx_msleep(10);
    }
}

void vidmm_set_allocation_priority(krnl_set_allocation_priority_t *prio)
{
    int i;
    vidmm_allocation_t *allocation = NULL;

    for(i = 0; i < prio->num_allocations; i ++)
    {
        allocation = prio->allocation_list[i];

        if(allocation == NULL) continue;

        allocation->priority = prio->priority_list[i];
    }

}

vidmm_allocation_t *vidmm_fetch_allocation_by_address(gpu_device_t *device,
            vidmm_allocation_t **alloc_list, unsigned int alloc_list_cnt,
            unsigned long long base, unsigned long long size,unsigned long long *offset, zx_vm_node_t **pnode)
{
    adapter_t *adapter = device->adapter;
    vidmm_allocation_t *found = NULL;
    struct range_node *range = NULL;
    zx_vm_zone_t *zone = vidmm_vm_get_zone_from_range(device->vm, base, size);
    int i;

    if (!zone)
    {
        zx_error("no such zone found...\n");
        return NULL;
    }

    if (zone->zone_id == VM_ZONE_ID_SHARE_SPACE_LOCAL)
    {
        // it's in share space, direct map
        for (i = 0; i < alloc_list_cnt; i++)
        {
            vidmm_allocation_t *alloc = alloc_list[i];
            vidmm_segment_t *segment = alloc->adapter->mm_mgr->segment + alloc->segment_id;
            unsigned long long gpu_va = adapter->shared_gpu_va_offset + alloc->fb_pages_mem->range_node_list[0]->start;

            // for shared alloc, always resident when create
            if (segment->flags.local)
            {
                // the allocation maybe not shared, since private allocation also can map to share space
                if (base >= gpu_va && (base + size) <= gpu_va + alloc->size)
                {
                    found = alloc;
                    if (offset)
                        *offset = base - gpu_va;
                    break;
                }
            }
        }

        return found;
    }

    zx_mutex_lock(zone->lock);
    range = zone->ra->lookup(zone->ra, base, size);
    if (!range)
    {
        zx_error("no such range found...\n");
        goto done;
    }

retry:
    if ((long)range->private & 1)
    {
        // it is an sparse node
        zx_sparse_node_t *sparse_node = (void*)((long)range->private & ~0x1);

        range = sparse_node->ra->lookup(sparse_node->ra, base, size);

        goto retry;
    }
    else
    {
        // it is an vm node
        zx_vm_node_t *vm_node = range->private;

        if (vm_node->mapped_entity_type != ZX_VM_NODE_TYPE_ALLOCATION)
        {
            zx_error("it's not an allocation...\n");
            goto done;
        }

        found = vm_node->mapped_entity;
        if (offset)
            *offset = base - vm_node->range->start;
        if (pnode)
            *pnode = vm_node;
    }

done:
    zx_mutex_unlock(zone->lock);
    return found;
}

void vidmmi_dump_vm_info(vidmm_chip_vm_info_t  *vm_info)
{
    zx_vm_zone_desc_t *zone_desc = NULL;

    int vm_zone_cnt = vm_info->zone_cnt;
    int i           = 0;

    zx_info("vm_info dump. vm_zone_cnt: %d, pt_levels_num: %2d-[%2d, %2d, %2d]-[%2d, %2d, %2d], pt_segment_id: %d\n",
        vm_zone_cnt, vm_info->pt_levels_num,
        vm_info->pt_bits_array[0][ZX_PT_LEVEL_ROOT], vm_info->pt_bits_array[0][ZX_PT_LEVEL_1], vm_info->pt_bits_array[0][ZX_PT_LEVEL_2],
        vm_info->pt_bits_array[1][ZX_PT_LEVEL_ROOT], vm_info->pt_bits_array[1][ZX_PT_LEVEL_1], vm_info->pt_bits_array[1][ZX_PT_LEVEL_2],
        vm_info->pt_segment_id);

    for(i = 0; i < VM_ZONE_ID_MAX; i ++)
    {
        zone_desc = &vm_info->desc[i];
        if(zone_desc->gpu_vm_size > 0)
        {
            zx_info("zone-%d. start: 0x%llx, size: %d MB, reserve_start: 0x%x, reserve_size: %d MB\n",
                    zone_desc->zone_id, zone_desc->gpu_vm_start,
                    zone_desc->gpu_vm_size >> 20, zone_desc->reserved_vm_start, zone_desc->reserved_vm_size >> 20);
        }
    }
}
