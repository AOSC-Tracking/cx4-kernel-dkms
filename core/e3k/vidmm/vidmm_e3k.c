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
#include "mm_e3k.h"
#include "chip_include_e3k.h"
#include "vidmmi_e3k.h"

/*
VA Space Layout:
+---------------------+ ---GPU_VA_SHARED_SPACE_OFFSET_ELT3K
|  local linear       |  ^
|---------------------|Shared Space
|                     |  v
+---------------------
+----GPU_VA_SHARED_SPACE_SIZE_ELT3K
|                     |
|  Private Space      |
|                     |
|                     |
+---------------------+

*/

void vidmm_query_chip_vm_info_e3k(adapter_t *adapter, vidmm_chip_vm_info_t *vm_info)
{
    vidmm_mgr_t         *mm_mgr       = adapter->mm_mgr;
    zx_vm_zone_desc_t   *desc         = NULL;
    int                  i, zone_cnt  = 0;

    adapter->ctl_flags.share_space_cover_local = adapter->hw_caps.system_only ? 0 : 1;

    if (adapter->chip_id == CHIP_CNE001)
        adapter->private_size_of_cne001 = GPU_VA_PRIVATE_VMA_SPACE_SIZE * 4;

    adapter->shared_gpu_va_size     =  adapter->total_local_memory_size;
    if(adapter->hw_caps.share_pcie_enable)
    {
        adapter->shared_gpu_va_size     +=  GPU_VA_SHARED_SPACE_PCIE_SIZE_ELT3K;
    }
    // share size should aligned up to 4G
    adapter->shared_gpu_va_size = (adapter->shared_gpu_va_size + 0x100000000ULL - 1) & ~(0x100000000ULL - 1);

    adapter->shared_gpu_va_offset   = GPU_VA_SHARED_SPACE_OFFSET_ELT3K;

    vm_info->pt_levels_num       = ZX_PT_LEVEL_MAX;
    //vm_info->pt_segment_id       = adapter->hw_caps.local_only ? SEGMENT_ID_LOCAL_CPU_VISIABLE_ELT3K : SEGMENT_ID_LOCAL_CPU_UNVISIABLE_ELT3K;
    vm_info->pt_segment_id       = SEGMENT_ID_LOCAL_ELT3K;
    vm_info->share_pt_segment_id = SEGMENT_ID_LOCAL_ELT3K;

    for(i = 0; i < ZX_PAGE_TYPE_MAX; i++)
    {
        vm_info->pt_bits_array[i][ZX_PT_LEVEL_ROOT] = 8;
        vm_info->pt_bits_array[i][ZX_PT_LEVEL_1]    = 10;
        vm_info->pt_bits_array[i][ZX_PT_LEVEL_2]    = ((i == 0)?10:6);

        vm_info->pt_bits_mask[i][ZX_PT_LEVEL_ROOT] =((i == 0)? 0xFF00000:0xFF0000);
        vm_info->pt_bits_mask[i][ZX_PT_LEVEL_1]    = ((i == 0)?0xFFC00:0xFFC0);
        vm_info->pt_bits_mask[i][ZX_PT_LEVEL_2]    = ((i == 0)?0x3FF:0x3F);
    }

    desc                               = &vm_info->desc[VM_ZONE_ID_SHARE_SPACE_LOCAL];
    desc->zone_id                      = VM_ZONE_ID_SHARE_SPACE_LOCAL;
    desc->reserved_vm_start            = 0;
    desc->reserved_vm_size             = 0;
    desc->gpu_vm_start                 = adapter->shared_gpu_va_offset;
    desc->gpu_vm_size                  = adapter->total_local_memory_size;
    desc->phys_addr_start              = desc->gpu_vm_start;
    desc->flags.share = 1;

    zx_info("config vm zone [%d]: va range [0x%llx - 0x%llx) size 0x%llx(%dMB), reserve [0x%llx - 0x%llx] size 0x%llx, shared %d\n", desc->zone_id,
            desc->gpu_vm_start, desc->gpu_vm_start + desc->gpu_vm_size, desc->gpu_vm_size, desc->gpu_vm_size >> 20,
            desc->reserved_vm_start, desc->reserved_vm_start + desc->reserved_vm_size, desc->reserved_vm_size,
            desc->flags.share);

    zone_cnt++;

    if(adapter->hw_caps.share_pcie_enable)
    {
        desc                               = &vm_info->desc[VM_ZONE_ID_SHARE_SPACE_PCIE];
        desc->zone_id                      = VM_ZONE_ID_SHARE_SPACE_PCIE;
        desc->reserved_vm_start            = 0;
        desc->reserved_vm_size             = 0;
        desc->gpu_vm_start                 = adapter->total_local_memory_size;
        desc->gpu_vm_size                  = adapter->shared_gpu_va_size - adapter->total_local_memory_size;
        desc->phys_addr_start              = desc->gpu_vm_start;
        desc->flags.share = 1;

        zx_info("config vm zone [%d]: va range [0x%llx - 0x%llx) size 0x%llx(%dMB), reserve [0x%llx - 0x%llx] size 0x%llx, shared %d\n", desc->zone_id,
                desc->gpu_vm_start, desc->gpu_vm_start + desc->gpu_vm_size, desc->gpu_vm_size, desc->gpu_vm_size >> 20,
                desc->reserved_vm_start, desc->reserved_vm_start + desc->reserved_vm_size, desc->reserved_vm_size,
                desc->flags.share);

        zone_cnt++;
    }

    desc                               = &vm_info->desc[VM_ZONE_ID_PRIVATE_SPACE];
    desc->zone_id                      = VM_ZONE_ID_PRIVATE_SPACE;
    desc->reserved_vm_start            = vm_info->desc[zone_cnt -1].gpu_vm_start + vm_info->desc[zone_cnt -1].gpu_vm_size;
    desc->reserved_vm_start            = (desc->reserved_vm_start + 0x100000000ULL - 1) & ~(0x100000000ULL - 1);
    desc->reserved_vm_size             = 0;
    desc->gpu_vm_start                 = desc->reserved_vm_start + desc->reserved_vm_size;
    if (adapter->chip_id == CHIP_CNE001)
        desc->gpu_vm_size              = adapter->private_size_of_cne001;
    else
        desc->gpu_vm_size              = GPU_VA_SPACE_TOTAL_SIZE - desc->gpu_vm_start;
    desc->phys_addr_start              = 0;//should not use this value for private va.

    zx_info("config vm zone [%d]: va range [0x%llx - 0x%llx) size 0x%llx(%dMB), reserve [0x%llx - 0x%llx] size 0x%llx, shared %d\n", desc->zone_id,
            desc->gpu_vm_start, desc->gpu_vm_start + desc->gpu_vm_size, desc->gpu_vm_size, desc->gpu_vm_size >> 20,
            desc->reserved_vm_start, desc->reserved_vm_start + desc->reserved_vm_size, desc->reserved_vm_size,
            desc->flags.share);

    vm_info->zone_cnt = VM_ZONE_ID_MAX;
}

static void vidmm_init_segments_e3k(adapter_t *adapter)
{
    int i, priority = 0;
    vidmm_mgr_t *mm_mgr = adapter->mm_mgr;
    vidmm_segment_t *segment = NULL;

    if (adapter->chip_id == CHIP_CNE001)
    {
        mm_mgr->paging_segment_id = SEGMENT_ID_PCIE_SNOOPABLE_ELT3K ;
    }
    else
    {
        mm_mgr->paging_segment_id = adapter->hw_caps.snoop_only ? SEGMENT_ID_PCIE_SNOOPABLE_ELT3K : SEGMENT_ID_PCIE_UNSNOOPABLE_ELT3K;
    }

    mm_mgr->segment_cnt     = SEGMENT_ID_MAX_ELT3K;
    mm_mgr->segment         = zx_calloc(mm_mgr->segment_cnt * sizeof(vidmm_segment_t));

    // invalid segment
    segment = mm_mgr->segment + SEGMENT_ID_INVALID_ELT3K;
    segment->segment_id                 = SEGMENT_ID_INVALID_ELT3K;
    segment->flags.invalid              = 1;

    // LOCAL
    segment                             = mm_mgr->segment + SEGMENT_ID_LOCAL_ELT3K;
    segment->segment_id                 = SEGMENT_ID_LOCAL_ELT3K;
    segment->flags.local                = (adapter->hw_caps.system_only) ? 0 : 1;
    segment->flags.support_manual_flush = 1;
    segment->flags.support_snoop        = 0;
    segment->flags.support_page_64kb    = 0;
    segment->flags.mtrr                 = 1;
    segment->property                   = ZX_SEGMENT_PROPERTY_LOCAL;
    segment->size                       = adapter->total_local_memory_size;
    segment->page_size                  = GPU_PAGE_SIZE;
    segment->start_addr                 = 0;
    segment->phys_addr_start            = 0;

    // PCIE_UNSNOOPABLE
    segment                             = mm_mgr->segment + SEGMENT_ID_PCIE_UNSNOOPABLE_ELT3K;
    segment->segment_id                 = SEGMENT_ID_PCIE_UNSNOOPABLE_ELT3K;
    segment->flags.local                = 0;
    segment->flags.support_manual_flush = 0;
    segment->flags.support_snoop        = 0;
    segment->flags.support_page_64kb    = 0;
    segment->flags.mtrr                 = 1;
    segment->flags.require_system_pages = 1;
    segment->property                   = ZX_SEGMENT_PROPERTY_PCIE;
    segment->size                       = (adapter->hw_caps.local_only || adapter->hw_caps.snoop_only) ?  0 : GPU_VA_SPACE_TOTAL_SIZE;
    segment->page_size                  = adapter->os_page_size;

    // PCIE_SNOOPABLE
    segment                             = mm_mgr->segment + SEGMENT_ID_PCIE_SNOOPABLE_ELT3K;
    segment->segment_id                 = SEGMENT_ID_PCIE_SNOOPABLE_ELT3K;
    segment->flags.local                = 0;
    segment->flags.support_manual_flush = 0;
    segment->flags.support_snoop        = 1;
    segment->flags.support_page_64kb    = 0;
    segment->flags.mtrr                 = 1;
    segment->flags.require_system_pages = 1;
    segment->property                   = ZX_SEGMENT_PROPERTY_PCIE | ZX_SEGMENT_PROPERTY_SNOOPABLE;
    segment->size                       = (adapter->hw_caps.local_only) ?  0 : GPU_VA_SPACE_TOTAL_SIZE;
    segment->page_size                  = adapter->os_page_size;

    for (i = 0; i < mm_mgr->segment_cnt; i++)
    {
        segment = mm_mgr->segment + i;
        segment->lock         = zx_create_mutex();
        segment->secure_lock  = zx_create_mutex();
        for(priority = 0; priority < PALL; priority++)
        {
            list_init_head(&segment->pagable_resident_list[priority]);
        }
        list_init_head(&segment->unpagable_resident_list);

        if (segment->flags.local && !segment->flags.invalid)
        {
#define ZX_SHADOW_VBIOS_SIZE    0x20000
            segment->mem_ra = zx_create_range_allocator(segment->start_addr, segment->size, segment->page_size);
            adapter->reserv_uaval = segment->mem_ra->reserve(segment->mem_ra,
                    adapter->visible_local_memory_size - ZX_SHADOW_VBIOS_SIZE, ZX_SHADOW_VBIOS_SIZE);
        }
    }
}

static void vidmm_fini_segments_e3k(adapter_t *adapter)
{
    int i;
    vidmm_mgr_t *mm_mgr = adapter->mm_mgr;

    if (adapter->reserv_uaval)
    {
        struct range_node *node = adapter->reserv_uaval;
        node->release(node);

        adapter->reserv_uaval = NULL;
    }

    if (mm_mgr->segment)
    {
        for (i = 0; i < mm_mgr->segment_cnt; i++)
        {
            vidmm_segment_t *segment = mm_mgr->segment + i;
            zx_mutex_lock(segment->lock);
            if (segment->mem_ra)
                segment->mem_ra->release(segment->mem_ra);
            zx_mutex_unlock(segment->lock);
            if (segment->lock)
                zx_destroy_mutex(segment->lock);
            if (segment->secure_lock)
                zx_destroy_mutex(segment->secure_lock);
        }

        zx_free(mm_mgr->segment);
        mm_mgr->segment = NULL;
    }
}

int vidmmi_get_segment_id_by_property(adapter_t* adapter, unsigned int property)
{
    vidmm_mgr_t     *mm_mgr  = adapter->mm_mgr;
    vidmm_segment_t *segment = NULL;
    int i;

    for(i = 0; i < SEGMENT_ID_MAX_ELT3K; i ++)
    {
        segment = &mm_mgr->segment[i];//skip the invalid segment

        if(!segment->flags.invalid && (segment->property & property) == property)
            return segment->segment_id;
    }

    zx_error("%s: invalidate segment property: 0x%x\n", __func__, property);
    return 0;
}

/*
* vidmm_init_shared_pt_e3k: init the page table for the shared vma space.
* the shared page table resident in local memory with static. the buffer which uses this
* page table only needs to fill the level3's pte.
* the local memory for shared space is directly mapping accessed, so the page table for shared local memory
* is ignored.
*/
// 1) When a address is in [Legacy_Offset, Legacy_Offset+Legacy_Size], it's belong to share context.
// While Hw has a bug, it thinks the boundary is [Legacy_Offset, LocalVideoSize], so caused share context
// VA translate has problem.
// Here I just work-around it like this, first program internal share context's ptbs for not only pcie but also local
// then program the legacy_offset/legacy_size with some fake values to make sure all VA request not in this range.
int vidmm_init_shared_pt_e3k(adapter_t *adapter)
{
    vidmm_mgr_t            *mm_mgr     = adapter->mm_mgr;
    vidmm_segment_memory_t *pt_buffer  = NULL;

    PTE_L1_t                pte_level1 = {0};
    PTE_L2_t                pte_level2 = {0};
    PTE_L3_t                pte_level3 = {0};
    Reg_Ttbr                reg_ttbr   = {0};
    unsigned int            reg_offset = 0;

    unsigned char     *pt_buffer_cpu_va = NULL;

    unsigned int total_ptbs_size = 0;
    unsigned int level, idx, id  = 0;
    int          result          = S_OK;

    unsigned int level3_entries_local = 0;
    unsigned int level3_entries_pcie = 0;
    unsigned int level2_entries = 0;
    unsigned int level1_entries = 0;
    unsigned int level2_pte_local_idx = 0;

    unsigned int level3_ptbs  = 0;
    unsigned int level2_ptbs  = 0;
    unsigned int level1_ptbs  = 1;

    unsigned long long  level1_ptb_offset, level2_ptb_offset, level3_ptb_offset;
    unsigned long long  level1_gpu_pa_offset, level2_gpu_pa_offset, level3_gpu_pa_offset;
    unsigned char      *level1_cpu_va_offset, *level2_cpu_va_offset, *level3_cpu_va_offset;

    if(!adapter->ctl_flags.share_space_cover_local)
    {
        zx_info("shared_gpu_va_size: %dM, total_local_size: %dM. the whole va shared space belongs to local domain. no need to the pt.\n",
            adapter->shared_gpu_va_size >> 20, adapter->total_local_memory_size >> 20);
        return result;
    }

    zx_assert(adapter->shared_gpu_va_size >= adapter->total_local_memory_size, "share mem should cover total local mem");

    zx_info("shared pt. total:%llx, share:%llx\n", adapter->total_local_memory_size, adapter->shared_gpu_va_size);

    level3_entries_local = (adapter->total_local_memory_size >> GPU_PAGE_SHIFT);
    level3_entries_pcie  = ((adapter->shared_gpu_va_size - adapter->total_local_memory_size) >> GPU_PAGE_SHIFT);
    level2_entries = (level3_entries_local + level3_entries_pcie) / 1024 + (((level3_entries_local + level3_entries_pcie)%1024)?1:0);
    level1_entries = level2_entries/1024 + ((level2_entries % 1024 )? 1 : 0);

    level3_ptbs  = level2_entries;
    level2_ptbs  = level1_entries;

    level2_ptb_offset = level1_ptbs * GPU_PAGE_SIZE; //one page is enough for 256 TTBR.
    level3_ptb_offset = level2_ptb_offset + level2_ptbs * GPU_PAGE_SIZE;

    total_ptbs_size = (level2_ptbs + level3_ptbs + level1_ptbs) * GPU_PAGE_SIZE;

    //the layout of pt_buffer as below:
    //|<---level 1---->|<-----level 2------>|<------level 3----->|
    pt_buffer = vidmm_create_segment_memory(adapter, mm_mgr->vm_info->share_pt_segment_id, total_ptbs_size, 1, 1);
    if(pt_buffer == NULL)
    {
        zx_error("create segment memory failed\n");
        result = E_FAIL;
        return result;
    }

    // used for hang dump
    if (pt_buffer->fb_pages_mem)
    {
        // tag it as a ptes page, hang dump will use this info
        pt_buffer->fb_pages_mem->range_node_list[0]->tag = 7;
    }

    pt_buffer_cpu_va = (unsigned char*)vidmm_lock_segment_memory(adapter, pt_buffer);
    if(pt_buffer_cpu_va == NULL)
    {
        zx_error("lock segment memory failed\n");
        result = E_FAIL;
        return result;
    }
    zx_memset(pt_buffer_cpu_va, 0, pt_buffer->size);

    zx_info("shared pt. pt_buffer_cpu_va: %p, l1_entries: %d, l2_entries: %d, l3_entries_pcie: %d, level3_entries_local:%d\n", pt_buffer_cpu_va, level1_entries, level2_entries, level3_entries_pcie, level3_entries_local);
    zx_info("shared pt. l1_ptbs: %d, l2_ptbs: %d, l3_ptbs: %d\n", level1_ptbs, level2_ptbs, level3_ptbs);

    level1_gpu_pa_offset = vidmm_get_segment_memory_pa_cont(pt_buffer, 0);
    level2_gpu_pa_offset = level1_gpu_pa_offset + level2_ptb_offset;
    level3_gpu_pa_offset = level1_gpu_pa_offset + level3_ptb_offset;

    level1_cpu_va_offset = pt_buffer_cpu_va;
    level2_cpu_va_offset = level1_cpu_va_offset + level2_ptb_offset;
    level3_cpu_va_offset = level1_cpu_va_offset + level3_ptb_offset;

    zx_info("level1_cpu_va_offset: %p, level2_cpu_va_offset: %p, level3_cpu_va_offset: %p\n", level1_cpu_va_offset, level2_cpu_va_offset, level3_cpu_va_offset);
    zx_info("level1_gpu_pa_offset: 0x%llx, level2_gpu_pa_offset: 0x%llx, level3_gpu_pa_offset: 0x%llx\n", level1_gpu_pa_offset, level2_gpu_pa_offset, level3_gpu_pa_offset);

    mm_mgr->vm_info->share_level3_cpu_va_offset   = (unsigned int*)level3_cpu_va_offset;
    mm_mgr->vm_info->share_level3_gpu_va_offset   = level3_gpu_pa_offset;
    mm_mgr->vm_info->share_level1_cpu_va_offset   = (unsigned int*)level1_cpu_va_offset;

    adapter->share_pcie_level3_gpu_va = mm_mgr->vm_info->share_level3_gpu_va_offset + level3_entries_local * 4;

    //shared local part of  level3.
    pte_level3.pte_4k.Valid   = 1;
    pte_level3.pte_4k.Segment = 1;
    pte_level3.pte_4k.NS = 1;
    for(idx = 0; idx < level3_entries_local; idx ++)
    {
        pte_level3.pte_4k.Addr = idx;
        *((unsigned int *)level3_cpu_va_offset + idx) = pte_level3.uint;
    }

    //shared  part level 2
    pte_level2._4k_d.Valid   = 1;
    pte_level2._4k_d.Segment = 1;
    pte_level2._4k_d.NStable = 1;
    for(idx = 0; idx < level2_entries; idx ++)
    {
        pte_level2._4k_d.Addr = (unsigned long long)((level3_gpu_pa_offset + idx * GPU_PAGE_SIZE) >> 12);
        *((unsigned int *)level2_cpu_va_offset + idx) = pte_level2.uint;
    }

    reg_ttbr.reg.Valid       = 1;
    reg_ttbr.reg.Segment     = 1;
    reg_ttbr.reg.Ns          = 1;

    //todo: get the 16  from the chip func
    for(id = 0; id < 16; id ++)
    {
        for(idx = 0; idx < level1_entries; idx ++)
        {
            //memory backup
            reg_ttbr.reg.Addr = (unsigned long long)((level2_gpu_pa_offset + idx * GPU_PAGE_SIZE) >> 12);
            *((unsigned int *)level1_cpu_va_offset + idx) = reg_ttbr.uint;

            //write ttbr through mmio
            reg_offset = MMIO_MMU_START_ADDRESS + Reg_Mmu_Ttbr_Offset * 4 + id * 256 * 4 + 4 * idx;
            zx_write32(adapter->mmio + reg_offset, reg_ttbr.uint);
        }
    }

    adapter->gart_dirty.dirty = TRUE;

    /* flush write_combine buffer */
    zx_flush_wc();

    mm_mgr->share_pt_buffer = pt_buffer;
    return result;
}

static unsigned long long vidmmi_map_shared_buffer_local_e3k(vidmm_mgr_t *mm_mgr, struct fb_pages_memory *fb_pages, unsigned long long offset, unsigned long long size, unsigned long long gpu_va)
{
    int i;
    unsigned long long mask = 0;
    unsigned int range_index = 0;
    unsigned int range_offset = 0;
    unsigned long long range_size;
    unsigned long long pte_pa;
    unsigned long long skip_size = 0;
    unsigned int pte_start = 0;
    PTE_L3_t pte_level3  = {0};
    unsigned long long curr_va = gpu_va;

    pte_start  = (gpu_va >> GPU_PAGE_SHIFT);
    pte_level3.pte_4k.Valid   = 1;
    pte_level3.pte_4k.Segment = 1;
    pte_level3.pte_4k.NS      = 1;
    pte_level3.pte_4k.Snoop   = 0;

    while(range_index < fb_pages->range_node_num &&
          skip_size + fb_pages->range_node_list[range_index]->size <= offset)
    {
        skip_size += fb_pages->range_node_list[range_index]->size;
        ++range_index;
    }

    range_offset = offset - skip_size;
    while(size > 0)
    {
        pte_pa = fb_pages->range_node_list[range_index]->start + range_offset;
        #define __MIN__(x,y) ((x) < (y) ? (x) : (y))
        range_size = __MIN__(fb_pages->range_node_list[range_index]->size - range_offset, size);
        #undef __MIN__

        for (i = 0; i < (range_size >> GPU_PAGE_SHIFT); i++)
        {
            pte_level3.pte_4k.Addr = ((pte_pa + i * GPU_PAGE_SIZE) >> 12);
            *(((unsigned int *)mm_mgr->vm_info->share_level3_cpu_va_offset) + pte_start) = pte_level3.uint;
            ++pte_start;
            mask |= (gpu_va ^ curr_va);
            curr_va += GPU_PAGE_SIZE;
        }

        range_offset = 0;
        size -= range_size;
        ++range_index;
    }

    return mask;
}

struct fill_gart_arg
{
    adapter_t *adapter;
    unsigned long long gpu_va;
    unsigned int *pte_base;
    PTE_L3_t pte_level3;
    unsigned int pte_start;

    int start;
    int end;
    unsigned int os_page_shift;
    unsigned long long curr_va;
    unsigned long long mask;
};
extern unsigned long long vidsch_get_pte_value(adapter_t *adapter, int snoop, unsigned long long phy_addr);
static int vidmmi_fill_gart_pcie_e3k(void *arg, int page_start, int page_cnt, unsigned long long dma_addr)
{
    unsigned int i;
    unsigned int end= 0;
    unsigned int this_offset = 0;
    struct fill_gart_arg *fill = arg;
#define __GPU_PAGE__(cpu_page)  ((cpu_page) << (fill->os_page_shift - GPU_PAGE_SHIFT))
    int gpu_start = __GPU_PAGE__(page_start);
    int gpu_end = __GPU_PAGE__(page_start + page_cnt);
#undef __GPU_PAGE__

    if (gpu_end <= fill->start)
    {
        // skip
        return 0;
    }

    if (gpu_start >= fill->end)
    {
        // break the loop
        return -1;
    }

#define __MAX__(x,y) ((x) > (y) ? (x) : (y))
#define __MIN__(x,y) ((x) < (y) ? (x) : (y))
#define __DMA_ADDR__(gpu_page)  ((dma_addr) + ((gpu_page - gpu_start) << GPU_PAGE_SHIFT))
    for (i = __MAX__(gpu_start, fill->start); i < __MIN__(gpu_end, fill->end); i++)
    {
        fill->pte_level3.pte_4k.Addr =  vidsch_get_pte_value(fill->adapter, fill->pte_level3.pte_4k.Snoop, (__DMA_ADDR__(i)))  >> 12;
        *(fill->pte_base + fill->pte_start) = fill->pte_level3.uint;
        fill->mask |= (fill->gpu_va ^ fill->curr_va);
        fill->curr_va += GPU_PAGE_SIZE;
        fill->pte_start++;
    }
#undef __DMA_ADDR__
#undef __MIN__
#undef __MAX__

    return 0;
}

static unsigned long long vidmmi_map_shared_buffer_pcie_e3k(vidmm_mgr_t *mm_mgr, struct os_pages_memory *pages_mem, int snoop, unsigned long long offset, unsigned long long size, unsigned long long gpu_va)
{
    struct fill_gart_arg fill = {0, };

    fill.pte_level3.pte_4k.Valid   = 1;
    fill.pte_level3.pte_4k.Segment = 0;
    fill.pte_level3.pte_4k.NS      = 1;
    fill.pte_level3.pte_4k.Snoop   = snoop;
    fill.start = (offset >> GPU_PAGE_SHIFT);
    fill.end = ((offset + size) >> GPU_PAGE_SHIFT);
    fill.gpu_va = gpu_va;
    fill.pte_start = (gpu_va >> GPU_PAGE_SHIFT);
    fill.curr_va = gpu_va;
    fill.pte_base = mm_mgr->vm_info->share_level3_cpu_va_offset;
    fill.os_page_shift = mm_mgr->adapter->os_page_shift;
    fill.adapter = mm_mgr->adapter;
    zx_pages_memory_for_each_continues(pages_mem, &fill, vidmmi_fill_gart_pcie_e3k);

    return fill.mask;
}

#if 0
static unsigned long long vidmmi_map_shared_buffer_pcie2_e3k(vidmm_mgr_t *mm_mgr, struct os_pages_memory *pages_mem, int snoop, unsigned long long offset, unsigned long long size, unsigned long long gpu_va)
{
    unsigned int idx, j;
    unsigned long long mask = 0;
    unsigned long long pte_pa;
    PTE_L3_t pte_level3  = {0};
    unsigned int os_page_size = mm_mgr->adapter->os_page_size;
    unsigned int start = (offset >> mm_mgr->adapter->os_page_shift);
    unsigned int end = ((offset + size) >> mm_mgr->adapter->os_page_shift);
    unsigned int pte_start  = gpu_va / GPU_PAGE_SIZE;

    pte_level3.pte_4k.Valid   = 1;
    pte_level3.pte_4k.Segment = 0;
    pte_level3.pte_4k.NS      = 1;
    pte_level3.pte_4k.Snoop   = snoop;

    for (idx = start; idx < end; idx++)
    {
        pte_pa = zx_get_page_phys_address(pages_mem, idx, NULL);

        for (j = 0; j < os_page_size / GPU_PAGE_SIZE; j++)
        {
            pte_level3.pte_4k.Addr = ((pte_pa + j * GPU_PAGE_SIZE)>> 12);
            *(((unsigned int *)mm_mgr->vm_info->share_level3_cpu_va_offset) + pte_start) = pte_level3.uint;
            ++pte_start;
            mask |= gpu_va ^(gpu_va + (idx - start) * os_page_size + j * GPU_PAGE_SIZE);
        }
    }

    return mask;
}
#endif

/*
vidmm_map_shared_buffer_e3k:
set up the page table translation for the shared segment memory.
the page table for the shared segment memory already be filled with level1 and level2.
so here we only fill the level3's page table.
*/
static int vidmm_map_shared_buffer_e3k(adapter_t *adapter, ZX_VM_NODE_TYPE entity_type, void *buffer, unsigned long long off, unsigned long long size, unsigned long long gpu_va)
{
    vidmm_mgr_t      *mm_mgr     = adapter->mm_mgr;
    vidmm_segment_t  *segment    = NULL;
    struct os_pages_memory  *pages_mem      = NULL;
    struct fb_pages_memory  *fb_pages_mem  = NULL;
    unsigned long long      pte_pa         = 0;
    vidmm_segment_memory_t  *segment_memory = NULL;
    vidmm_allocation_t      *allocation     = NULL;
    unsigned long long      mask           = 0;

    if(!adapter->ctl_flags.share_space_cover_local)
    {
        zx_debug("skip map share_space.\n");
        return S_OK;
    }

    zx_assert(((entity_type == ZX_VM_NODE_TYPE_ALLOCATION) || (entity_type == ZX_VM_NODE_TYPE_SEGMENET_MEMORY)), "");

    if (gpu_va >= adapter->shared_gpu_va_offset && gpu_va < adapter->shared_gpu_va_offset + adapter->total_local_memory_size)
    {
        // direct map, go legacy path
        return S_OK;
    }

    zx_assert(gpu_va >= adapter->total_local_memory_size &&
        gpu_va <= adapter->shared_gpu_va_size, "map wrong gpu va into share space");

    if(entity_type == ZX_VM_NODE_TYPE_ALLOCATION)
    {
        allocation = (vidmm_allocation_t *)buffer;
        segment    = &mm_mgr->segment[allocation->segment_id];

        if (segment->flags.local)
        {
            fb_pages_mem = allocation->fb_pages_mem;
            mask = vidmmi_map_shared_buffer_local_e3k(mm_mgr, fb_pages_mem, off, size, gpu_va);
        }
        else
        {
            pages_mem = allocation->pages_mem;
            mask = vidmmi_map_shared_buffer_pcie_e3k(mm_mgr, pages_mem, segment->flags.support_snoop, off, size, gpu_va);
        }
    }
    else if(entity_type == ZX_VM_NODE_TYPE_SEGMENET_MEMORY)
    {
        segment_memory = (vidmm_segment_memory_t *)buffer;
        segment        = &mm_mgr->segment[segment_memory->segment_id];

        if(segment_memory->flag.local == 0)
        {
            pages_mem  = segment_memory->pages_mem;
            mask = vidmmi_map_shared_buffer_pcie_e3k(mm_mgr, pages_mem, segment->flags.support_snoop, off, size, gpu_va);
        }
    }

    zx_mutex_lock(adapter->gart_table_lock);

    if(adapter->gart_dirty.dirty == TRUE)
    {
         adapter->gart_dirty.dirty_mask =
            (adapter->gart_dirty.dirty_mask | mask |
            (adapter->gart_dirty.dirty_addr ^ ( gpu_va & ~mask)));
         adapter->gart_dirty.dirty_addr =
            adapter->gart_dirty.dirty_addr & ~adapter->gart_dirty.dirty_mask;
    }
    else
    {
        adapter->gart_dirty.dirty_mask = mask;
        adapter->gart_dirty.dirty_addr = gpu_va & ~mask;
    }

    adapter->gart_dirty.dirty = TRUE;

    /* flush write_combine buffer */
    zx_flush_wc();

    zx_mutex_unlock(adapter->gart_table_lock);
    return S_OK;
}


int vidmm_update_ptes_e3k(zx_vma_space_t* vma_space, int engine_index, vidmm_vm_update_ptes_arg_t *update_pte_arg)
{
    adapter_t *adapter = vma_space->adapter;
    _2d_bltimm_cmd_elt3k_t     data_follow_cmd;
    vidmm_vm_pte_elt3k_t       pte_elt3k={ 0 };

    unsigned int i, j, s_idx, page_shift, left = 0, right = 0;

    unsigned int  *dst                  = NULL;
    unsigned int   pte_count            = update_pte_arg->npte;
    unsigned int   pt_level             = update_pte_arg->pt_level;

    vidmm_allocation_t     *allocation     = NULL;
    vidmm_segment_memory_t *segment_memory = NULL;

    struct os_pages_memory *pages_mem      = NULL;
    fb_pages_memory_t      *fb_pages_mem   = NULL;
    unsigned long long dtlb_mask     = 0;
    unsigned long long utlb_mask     = 0;

    //per pte field value
    unsigned int       pte_security  = 0;
    unsigned int       pte_segment   = 0;
    unsigned int       pte_snoop     = 0;
    unsigned long long pte_pa        = 0;
    unsigned int      start_page_idx  = 0;
    int current_page = 0;
    struct range_node **current_node = NULL;
    int bPatchForDependencyTLB = (adapter->chip_id >= CHIP_CHX004);

    UNUSED(vma_space);

    //zx_debug("%s. pte_count: %d, update_mode: %d\n", __func__, pte_count, vma_space->flag.cpu_update_pte);

    if(!vma_space->flag.cpu_update_pte)
    {
        dst = (unsigned int *)update_pte_arg->dma_buffer;

        //init and fill the 2d blt cmd
        //todo: the below cmd can be saved as backup in adapter global.
        zx_memset(&data_follow_cmd, 0, sizeof(_2d_bltimm_cmd_elt3k_t));

        data_follow_cmd.SetDstRTAddrCtrl = SET_REGISTER_FD_E3K(FF_BLOCK,
            Reg_Rt_Addr_Ctrl_Offset + FF_RT_ADDR_OFFSET + FF_RT_DST * FF_RT_ADDR_CTRL_REG_SIZE,
            FF_RT_ADDR_CTRL_REG_SIZE);
        data_follow_cmd.SetDstRTCtrl = SET_REGISTER_FD_E3K(FF_BLOCK,
            Reg_Rt_Ctrl_Offset + FF_RT_FMT_OFFSET + FF_RT_DST * FF_RT_CTRL_REG_SIZE,
            FF_RT_CTRL_REG_SIZE);
        data_follow_cmd.SetSrcRTAddrCtrl = SET_REGISTER_FD_E3K(FF_BLOCK,
            Reg_Rt_Addr_Ctrl_Offset + FF_RT_ADDR_OFFSET + FF_RT_SRC * FF_RT_ADDR_CTRL_REG_SIZE,
            FF_RT_ADDR_CTRL_REG_SIZE);
        data_follow_cmd.SetSrcRTCtrl = SET_REGISTER_FD_E3K(FF_BLOCK,
            Reg_Rt_Ctrl_Offset + FF_RT_FMT_OFFSET + FF_RT_SRC * FF_RT_CTRL_REG_SIZE,
            FF_RT_CTRL_REG_SIZE);
        data_follow_cmd.Set_Rast_Ctrl = SET_REGISTER_FD_E3K(FF_BLOCK, Reg_Rast_Ctrl_Offset, 1);
        data_follow_cmd.Set_Tasfe_Ctrl = SET_REGISTER_FD_E3K(TASFE_BLOCK, Reg_Tasfe_Ctrl_Offset, 1);
        data_follow_cmd.Set_Resolution_Ctrl = SET_REGISTER_FD_E3K(FF_BLOCK, Reg_Resolution_Ctrl_Offset, 1);
        data_follow_cmd.Set_Dzs_Ctrl = SET_REGISTER_FD_E3K(FF_BLOCK, Reg_Dzs_Ctrl_Offset, 1);
        data_follow_cmd.Set_Tasbe_Ctrl = SET_REGISTER_FD_E3K(FF_BLOCK, Reg_Tasbe_Ctrl_Offset, 1);
        data_follow_cmd.Set_Tasfe_Ctrl_Misc = SET_REGISTER_FD_E3K(TASFE_BLOCK, Reg_Tasfe_Ctrl_Misc_Offset, 1);
        data_follow_cmd.Set_Ff_Glb_Ctrl = SET_REGISTER_FD_E3K(FF_BLOCK, Reg_Ff_Glb_Ctrl_Offset, 1);

        data_follow_cmd.SrcFormat.reg.Format = HSF_R8G8B8A8_UNORM;
        data_follow_cmd.DstFormat.reg.Format = HSF_R8G8B8A8_UNORM;
        data_follow_cmd.DstSize.reg.Width = 16384;//set to max size
        data_follow_cmd.DstSize.reg.Height = 1;
        data_follow_cmd.DstMisc.reg.Resource_Type = RT_MISC_RESOURCE_TYPE_1D_BUFFER;
        data_follow_cmd.DstMisc.reg.Is_Tiling = 0;
        data_follow_cmd.DstMisc.reg.Resolve_Write_En = 0;
        data_follow_cmd.DstMisc.reg.Rt_Enable = 1;
        data_follow_cmd.DstMisc.reg.Rt_Write_Mask = 0xF;
        data_follow_cmd.DstMisc.reg.D_Read_En = 0;
        data_follow_cmd.DstMisc.reg.L1cachable = 0;
        data_follow_cmd.DstMisc.reg.L2cachable = 0;
        data_follow_cmd.DstMisc.reg.Mipmap_En = 0;
        data_follow_cmd.DstDepth.reg.Range_Type = CP_OFF;
        data_follow_cmd.DstDepth.reg.Depth_Or_Arraysize = 1;
        data_follow_cmd.DstDepth.reg.Lod = 0;

        data_follow_cmd.DstViewCtrl.reg_CHX004.Arraysize = 1;
        data_follow_cmd.DstViewCtrl.reg_CHX004.First_Array_Idx = 0;
        data_follow_cmd.Dzs_Ctrl.reg.Src_Bits_Mode = DZS_CTRL_SRC_BITS_MODE_NORMAL;
        data_follow_cmd.reg_Tasbe_Ctrl.reg.Msaa_State = 0;
        data_follow_cmd.reg_Tasfe_Ctrl.reg.Msaa_State = 0;
        data_follow_cmd.reg_Resolution_Ctrl.reg.Msaa_Mode = 0;
        data_follow_cmd.reg_Resolution_Ctrl.reg.Dst_Resolution_Mode = 0;
        data_follow_cmd.Rast_Ctrl.reg.Msaa_En = 0;
        data_follow_cmd.Rast_Ctrl.reg.Check_Board = RAST_CTRL_CHECK_BOARD_256X32;
        data_follow_cmd.Ff_Glb_Ctrl.reg.Tile_Size = 0;
        data_follow_cmd.reg_Tasfe_Ctrl_Misc.reg.Tile_Size = 0;

        data_follow_cmd.Ff_Glb_Ctrl.reg.Ffc_Config_Mode = FF_GLB_CTRL_FFC_CONFIG_MODE_D_ONLY;
        data_follow_cmd.DstAddr.reg.Base_Addr = (unsigned int)(update_pte_arg->dst_pte_page_va >> 8);

        /*
           layout of the data follow cmd:
           [data_follow_cmd]
           [COPY_IMM_COMMAND]
           [left-right-range]
           [0]
           [pte data...]
          */
        left  = update_pte_arg->pte_offset;
        right = left + update_pte_arg->npte - 1;

        zx_memcpy(dst, &data_follow_cmd, sizeof(_2d_bltimm_cmd_elt3k_t));
        dst += sizeof(_2d_bltimm_cmd_elt3k_t) >> 2;
        *dst++ = (unsigned int)SEND_COPY_IMM_COMMAND_E3K((sizeof(vidmm_vm_pte_elt3k_t) * pte_count >> 2) + 2, FALSE);
        *dst++ = left | (right << 16);
        *dst++ = 0;
    }
    else
    {
        dst = (unsigned int*)ptr64_to_ptr(update_pte_arg->dst_pte_page_va + update_pte_arg->pte_offset);
    }

    zx_assert(((update_pte_arg->entity_type == ZX_VM_NODE_TYPE_ALLOCATION)||(update_pte_arg->entity_type == ZX_VM_NODE_TYPE_SEGMENET_MEMORY)), "");

    if(update_pte_arg->entity_type == ZX_VM_NODE_TYPE_ALLOCATION)
    {
        allocation = update_pte_arg->entity;
        zx_assert(allocation != NULL, "");
    }

    if(update_pte_arg->entity_type == ZX_VM_NODE_TYPE_SEGMENET_MEMORY)
    {
        segment_memory = update_pte_arg->entity;
        zx_assert(segment_memory != NULL, "");
    }

    pte_elt3k.value = 0;

    if(pt_level == ZX_PT_LEVEL_ROOT) //root pd
    {
        unsigned long long mask = 0;

        //pd is formed in system memory of segment memory
        pte_security = !segment_memory->flag.security;
        pte_snoop    = segment_memory->flag.snooping_enabled;
        pte_segment  = (segment_memory->flag.local) ? 1 : 0;
        //zx_info("snoop:%d, seg:%d\n", pte_snoop, pte_segment);
        for(i = 0; i < pte_count; i ++)
        {
            if(pte_segment == 0)
            {
                pte_pa = vidsch_get_pte_value(adapter, pte_snoop, zx_get_page_phys_address(segment_memory->pages_mem, i, NULL));
            }
            else
            {
                pte_pa = segment_memory->fb_pages_mem->range_node_list[0]->start + i * GPU_PAGE_SIZE;//per 4K one pte
            }

            pte_elt3k.pte_level1.pte.Valid   = 1;
            pte_elt3k.pte_level1.pte.NStable = pte_security;
            pte_elt3k.pte_level1.pte.Snoop   = pte_snoop;
            pte_elt3k.pte_level1.pte.Segment = pte_segment;
            pte_elt3k.pte_level1.pte.Addr    = pte_pa >> 12;//todo: check 32bits pa
            *dst++ = pte_elt3k.value;

            mask |= update_pte_arg->buf_start_va ^ (update_pte_arg->buf_start_va + i * 4 * 1024 * 1024 * 1024ULL);
        }
    }
    else if(pt_level == ZX_PT_LEVEL_1)//leaf pd
    {
        unsigned long long mask = 0;

        //pd is formed in system memory of segment memory
        pte_security = !segment_memory->flag.security;
        pte_snoop    = segment_memory->flag.snooping_enabled;
        pte_segment  = (segment_memory->flag.local) ? 1 : 0;

        for(i = 0; i < pte_count; i ++)
        {
            if(pte_segment == 0)
            {
                pte_pa = vidsch_get_pte_value(adapter, pte_snoop, zx_get_page_phys_address(segment_memory->pages_mem, i, NULL));
            }
            else
            {
                pte_pa = segment_memory->fb_pages_mem->range_node_list[0]->start + i * GPU_PAGE_SIZE;//per 4K one pte
            }

            //need care about the pa should not boyond 39bits, since Large_Page occupy one bit
            pte_elt3k.pte_level2._4k_d.Valid     = 1;
            pte_elt3k.pte_level2._4k_d.NStable   = pte_security;
            pte_elt3k.pte_level2._4k_d.Snoop     = pte_snoop;
            pte_elt3k.pte_level2._4k_d.Segment   = pte_segment;
            pte_elt3k.pte_level2._4k_d.LargePage = 0;
            pte_elt3k.pte_level2._4k_d.Addr      = pte_pa >> 12;

            *dst++ = pte_elt3k.value;

            mask |= update_pte_arg->buf_start_va ^ (update_pte_arg->buf_start_va + i * 4 * 1024 * 1024ULL);
        }
        if (vma_space->utlb_dirtys[engine_index].dirty)
        {
            vma_space->utlb_dirtys[engine_index].dirty_mask =
               (vma_space->utlb_dirtys[engine_index].dirty_mask | mask |
               (vma_space->utlb_dirtys[engine_index].dirty_addr ^ (update_pte_arg->buf_start_va & ~mask)));
            vma_space->utlb_dirtys[engine_index].dirty_addr =
               vma_space->utlb_dirtys[engine_index].dirty_addr & ~vma_space->utlb_dirtys[engine_index].dirty_mask;
        }
        else
        {
            vma_space->utlb_dirtys[engine_index].dirty_mask = mask;
            vma_space->utlb_dirtys[engine_index].dirty_addr = update_pte_arg->buf_start_va & ~mask;
            vma_space->utlb_dirtys[engine_index].dirty = TRUE;
        }
    }
    else if(pt_level == ZX_PT_LEVEL_2)//pt
    {
        unsigned long long mask = 0;
        if(allocation)
        {
            vidmm_segment_t *segment = allocation->adapter->mm_mgr->segment + allocation->segment_id;

            pte_security = !allocation->flag.secured;
            pte_snoop    = segment->flags.support_snoop;
            pte_segment  = segment->flags.local ? 1 : 0;

            if(pte_segment == 0)
            {
                pages_mem = allocation->pages_mem;
            }
            else
            {
                fb_pages_mem = allocation->fb_pages_mem;
            }
        }
        else//segment memory
        {
            pte_security = !segment_memory->flag.security;
            pte_snoop    = segment_memory->flag.snooping_enabled;
            pte_segment  = (segment_memory->flag.local) ? 1 : 0;

            if(pte_segment == 0)
            {
                pages_mem = segment_memory->pages_mem;
            }
            else
            {
                fb_pages_mem = segment_memory->fb_pages_mem;
            }
        }

        if(pte_segment)
        {
            zx_assert(fb_pages_mem != NULL,  "");
            page_shift = GPU_PAGE_SHIFT;
        }
        else
        {
            zx_assert(pages_mem != NULL,  "");
            page_shift = update_pte_arg->adapter->os_page_shift;
        }

        start_page_idx =  ((update_pte_arg->cur_buf_va_on_fill - update_pte_arg->buf_start_va) >> page_shift);
        start_page_idx += (update_pte_arg->entity_offset >> page_shift);

        if (allocation && fb_pages_mem)
        {
            int total_skip_pages = 0;

            current_page = 0;
            current_node = &fb_pages_mem->range_node_list[0];
            while(total_skip_pages < start_page_idx)
            {
                #define __MIN__(x,y) ((x) < (y) ? (x) : (y))
                int this_skip = __MIN__((*current_node)->size >> page_shift, start_page_idx - total_skip_pages);
                #undef __MIN__

                current_page += this_skip;
                total_skip_pages += this_skip;
                if (current_page == ((*current_node)->size >> page_shift))
                {
                    current_node++;
                    current_page = 0;
                }
            }
        }

        zx_assert((page_shift >= GPU_PAGE_SHIFT),  "page size is not order of 4K");
        pte_count /= (1 << (page_shift - GPU_PAGE_SHIFT));

        for(i = 0; i < pte_count; i ++)
        {
            if(pte_segment == 0)
            {
                pte_pa = zx_get_page_phys_address(pages_mem, i + start_page_idx, NULL);
                //zx_debug("%s, index:%d, pa:%llx\n", __func__, i, pte_pa);
            }
            else
            {
                if(allocation) {
                    pte_pa = (*current_node)->start + (current_page << page_shift);

                    if (++current_page == ((*current_node)->size >> page_shift))
                    {
                        current_node++;
                        current_page = 0;
                    }
                }else{
                    pte_pa = fb_pages_mem->range_node_list[0]->start + ((i + start_page_idx) << page_shift);
                }

            }

            pte_elt3k.pte_level3.pte_4k.Valid     = 1;
            pte_elt3k.pte_level3.pte_4k.NS        = pte_security;
            pte_elt3k.pte_level3.pte_4k.Snoop     = pte_snoop;
            pte_elt3k.pte_level3.pte_4k.Segment   = pte_segment;

            for(j = 0; j <  1 << (page_shift - GPU_PAGE_SHIFT); j++)
            {
               pte_elt3k.pte_level3.pte_4k.Addr  = vidsch_get_pte_value(adapter, pte_snoop, (pte_pa + j * 4096))>> 12;

               *dst++ = pte_elt3k.value;
               mask |= update_pte_arg->cur_buf_va_on_fill ^ (update_pte_arg->cur_buf_va_on_fill + (i << page_shift) + j * GPU_PAGE_SIZE);
            }
        }

        if (vma_space->dtlb_dirtys[engine_index].dirty == TRUE)
        {
            vma_space->dtlb_dirtys[engine_index].dirty_mask =
                (vma_space->dtlb_dirtys[engine_index].dirty_mask | mask |
                (vma_space->dtlb_dirtys[engine_index].dirty_addr ^ (update_pte_arg->cur_buf_va_on_fill & ~mask)));
            vma_space->dtlb_dirtys[engine_index].dirty_addr =
                vma_space->dtlb_dirtys[engine_index].dirty_addr & ~vma_space->dtlb_dirtys[engine_index].dirty_mask;

        }
        else
        {
            vma_space->dtlb_dirtys[engine_index].dirty_mask = mask;
            vma_space->dtlb_dirtys[engine_index].dirty_addr = update_pte_arg->cur_buf_va_on_fill & ~mask;
        }
        vma_space->dtlb_dirtys[engine_index].dirty = TRUE;
    }
    else
    {
        zx_error("invalidate page table index: %d\n", pt_level);
    }

    if (0 && !vma_space->flag.cpu_update_pte)
    {
        // move follow command to invalidate_tlb ?
        *dst++ = SEND_FFCACHE_FLUSH_COMMAND_E3K();
        *dst++ = SEND_FFCACHE_INVALIDATE_COMMAND_E3K();
        *dst++ = SEND_L2_FLUSH_COMMAND_E3K(BLOCK_COMMAND_L2_USAGE_ALL, 0);
        *dst++ = SEND_L2_INVALIDATE_COMMAND_E3K(BLOCK_COMMAND_L2_USAGE_ALL);

        *dst++ = SEND_INTERNAL_WRITEFENCE_E3K(FENCE_ROUTE_ID_D_FENCE, _RBT_3DBE, HWM_SYNC_KMD_SLOT);
        *dst++ = SEND_INTERNAL_FENCE_VALUE_E3K(FENCE_VALUE_CACHE_DMA_DFENCE);
        *dst++ = SEND_INTERNAL_WAIT_E3K(HWM_SYNC_KMD_SLOT, WAIT_METHOD_EQUAL, FENCE_VALUE_CACHE_DMA_DFENCE);
        *dst++ = SEND_INTERNAL_WAIT_MAIN_E3K(HWM_SYNC_KMD_SLOT, WAIT_METHOD_EQUAL, FENCE_VALUE_CACHE_DMA_DFENCE);

        *dst++ = SEND_INTERNAL_WRITEFENCE_E3K(FENCE_ROUTE_ID_D_FENCE, _RBT_3DBE, HWM_SYNC_KMD_SLOT);
        *dst++ = SEND_INTERNAL_FENCE_VALUE_E3K(FENCE_VALUE_RESET);
        *dst++ = SEND_INTERNAL_WAIT_E3K(HWM_SYNC_KMD_SLOT, WAIT_METHOD_EQUAL, FENCE_VALUE_RESET);
        *dst++ = SEND_INTERNAL_WAIT_MAIN_E3K(HWM_SYNC_KMD_SLOT, WAIT_METHOD_EQUAL, FENCE_VALUE_RESET);
    }

    if (!vma_space->flag.cpu_update_pte && bPatchForDependencyTLB)
    {
        unsigned long long pageTableAddr = update_pte_arg->dst_pte_page_va + right * 4;

        *dst++ = SET_REGISTER_ADDR_E3K(FF_BLOCK, Reg_Reserved_1_Offset, SET_REGISTER_ADDRESS_MODE_ADDRESS);
        *dst++ = SET_REGISTER_ADDR_LOW_E3K((unsigned int)(pageTableAddr & 0xffffffff));
        *dst++ = SET_REGISTER_ADDR_HIGH_AND_REGCNT_E3K((unsigned int)((pageTableAddr >> 32) & 0xffffffff), 1, 0);

        *dst++ = SEND_INTERNAL_WRITEFENCE_E3K(FENCE_ROUTE_ID_CSP_FENCE, _RBT_3DBE, HWM_SYNC_KMD_SLOT);
        *dst++ = SEND_INTERNAL_FENCE_VALUE_E3K(FENCE_VALUE_SET_MXU_REG_DFENCE);
        *dst++ = SEND_INTERNAL_WAIT_E3K(HWM_SYNC_KMD_SLOT, WAIT_METHOD_EQUAL, FENCE_VALUE_SET_MXU_REG_DFENCE);
        *dst++ = SEND_INTERNAL_WAIT_MAIN_E3K(HWM_SYNC_KMD_SLOT, WAIT_METHOD_EQUAL, FENCE_VALUE_SET_MXU_REG_DFENCE);
        *dst++ = SEND_INTERNAL_WRITEFENCE_E3K(FENCE_ROUTE_ID_CSP_FENCE, _RBT_3DBE, HWM_SYNC_KMD_SLOT);
        *dst++ = SEND_INTERNAL_FENCE_VALUE_E3K(FENCE_VALUE_RESET);
        *dst++ = SEND_INTERNAL_WAIT_E3K(HWM_SYNC_KMD_SLOT, WAIT_METHOD_EQUAL, FENCE_VALUE_RESET);
        *dst++ = SEND_INTERNAL_WAIT_MAIN_E3K(HWM_SYNC_KMD_SLOT, WAIT_METHOD_EQUAL, FENCE_VALUE_RESET);
    }

    if(!vma_space->flag.cpu_update_pte)
    {
        update_pte_arg->dma_buffer = (void *)dst;
    }

    return S_OK;
}

unsigned int vidmm_alloc_bl_slot_e3k(adapter_t *adapter, vidmm_allocation_t *allocation)
{
    vidmm_mgr_t *mm_mgr  = adapter->mm_mgr;
    unsigned int bl_size = allocation->size >> 9;// 1:512 compress rate
    struct range_allocator *ra = mm_mgr->bl_ra;

    zx_mutex_lock(mm_mgr->mgr_lock);
    allocation->bl_range = ra->alloc(ra, bl_size, BL_SLICE_ALIGNMENT, 0, -1, 1);
    zx_mutex_unlock(mm_mgr->mgr_lock);

    if(!allocation->bl_range)
    {
         zx_assert(0, "bl allocate failed, alloc:%p, bl size:0x%x\n", allocation, bl_size);
         return FAIL;
    }
    else
    {
        return zx_do_div(allocation->bl_range->start - ra->start, BL_SLICE_ALIGNMENT); //slice index
    }
}

int vidmm_free_bl_slot_e3k(adapter_t *adapter, vidmm_allocation_t *allocation)
{
    vidmm_mgr_t *mm_mgr  = adapter->mm_mgr;

    zx_mutex_lock(mm_mgr->mgr_lock);
    allocation->bl_range->release(allocation->bl_range);
    zx_mutex_unlock(mm_mgr->mgr_lock);

    allocation->bl_range = NULL;

    return S_OK;
}

static void vidmm_init_bl_heap_e3k(adapter_t *adapter)
{
    vidmm_mgr_t     *mm_mgr  = adapter->mm_mgr;
    unsigned long long heap_start, heap_size;

    heap_start = mm_mgr->bl_reserved_memory->gpu_va;
    heap_size  = mm_mgr->bl_reserved_memory->size;

    zx_assert(heap_start == 0,"BL start not from 0, hw limitation should be 0 start");//only for gray

    mm_mgr->bl_ra = zx_create_range_allocator(heap_start, heap_size, BL_SLICE_ALIGNMENT);

    zx_info("BL buffer heap init [0x%llx - 0x%llx], size 0x%llx\n", heap_start, heap_start + heap_size, heap_size);

    adapter->RangeBufferGpuVa = mm_mgr->bl_reserved_memory->gpu_va;
    //TODO: need destroy
    //dig reserve vm from bl start
    if(mm_mgr->reserved_vmem)
    {
        zx_mutex_lock(mm_mgr->mgr_lock);
        mm_mgr->bl_reserve_node =  mm_mgr->bl_ra->alloc(mm_mgr->bl_ra, mm_mgr->reserved_vmem, BL_SLICE_ALIGNMENT, 0, -1, 0);
        zx_mutex_unlock(mm_mgr->mgr_lock);
    }
}

static void vidmm_fini_bl_heap_e3k(adapter_t *adapter)
{
    vidmm_mgr_t     *mm_mgr  = adapter->mm_mgr;

    zx_mutex_lock(mm_mgr->mgr_lock);
    if(mm_mgr->bl_reserve_node)
    {
        mm_mgr->bl_reserve_node->release(mm_mgr->bl_reserve_node);

        mm_mgr->bl_reserve_node = NULL;
    }

    if(mm_mgr->bl_ra)
    {
        mm_mgr->bl_ra->release(mm_mgr->bl_ra);
        mm_mgr->bl_ra = NULL;
    }
    zx_mutex_unlock(mm_mgr->mgr_lock);
}

void vidmm_save_e3k(adapter_t *adapter)
{
    vidmm_mgr_t *mm_mgr = adapter->mm_mgr;

    //back share gart info.
    if(mm_mgr->share_pt_buffer && !mm_mgr->share_pt_backup)
    {
        mm_mgr->share_pt_backup = zx_malloc(mm_mgr->share_pt_buffer->size);

        if (mm_mgr->share_pt_backup)
        {
            zx_memcpy(mm_mgr->share_pt_backup, mm_mgr->share_pt_buffer->krnl_cpu_vma->virt_addr, mm_mgr->share_pt_buffer->size);
        }
        else
        {
            zx_error("out of memory when malloc share_pt_backup! size=0x%x\n", mm_mgr->share_pt_buffer->size);
        }
    }

    //TODO: save private gart table.

    //back up bl buffer
    if(mm_mgr->bl_reserved_memory && !mm_mgr->bl_buf_backup)
    {
        mm_mgr->bl_buf_backup = zx_malloc(mm_mgr->bl_reserved_memory->size);

        if (mm_mgr->bl_buf_backup)
        {
            zx_memcpy(mm_mgr->bl_buf_backup, mm_mgr->bl_reserved_memory->krnl_cpu_vma->virt_addr, mm_mgr->bl_reserved_memory->size);
        }
        else
        {
            zx_error("out of memory when malloc bl_buf_backup! size=0x%x\n", mm_mgr->bl_reserved_memory->size);
        }
    }
}

void vidmm_restore_e3k(adapter_t *adapter)
{
    vidmm_mgr_t *mm_mgr = adapter->mm_mgr;

    //restore share gart info.
    if(mm_mgr->share_pt_buffer && mm_mgr->share_pt_backup)
    {
        unsigned int level1_entries = (adapter->shared_gpu_va_size >> 32) + ((adapter->shared_gpu_va_size&0xFFFFFFFF)?1:0);
        unsigned int reg_offset = 0;
        unsigned int *level1_cpu_va_offset = mm_mgr->vm_info->share_level1_cpu_va_offset;
        int id, idx;

        zx_memcpy(mm_mgr->share_pt_buffer->krnl_cpu_vma->virt_addr, mm_mgr->share_pt_backup, mm_mgr->share_pt_buffer->size);

        zx_free(mm_mgr->share_pt_backup);
        mm_mgr->share_pt_backup = NULL;

        //todo: get the 16  from the chip func
        for(id = 0; id < 16; id ++)
        {
            for(idx = 0; idx < level1_entries; idx ++)
            {
                //write ttbr through mmio
                reg_offset = MMIO_MMU_START_ADDRESS + Reg_Mmu_Ttbr_Offset * 4 + id * 256 * 4 + 4 * idx;
                zx_write32(adapter->mmio + reg_offset, *(level1_cpu_va_offset + idx));
            }
        }

        adapter->gart_dirty.dirty = TRUE;

        /* flush write_combine buffer */
        zx_flush_wc();
    }

    {
        int id;
        Reg_Pt_Inv_Trig reg_Pt_Inv_Trig                 = {0};
        Reg_Pt_Inv_Addr reg_Pt_Inv_Addr                 = {0};
        Reg_Pt_Inv_Mask reg_Pt_Inv_Mask                 = {0};

        reg_Pt_Inv_Trig.reg.Target       = PT_INV_TRIG_TARGET_DTLB;
        for (id = 0; id < 16; id++)
        {
            reg_Pt_Inv_Trig.reg.Proc_Id = id;
            reg_Pt_Inv_Addr.reg.Proc_Id = id;
            reg_Pt_Inv_Mask.reg.Proc_Id = id;
            zx_write32(adapter->mmio + MMIO_MXU_START_ADDRESS + Reg_Pt_Inv_Addr_Offset*4 , reg_Pt_Inv_Addr.uint);
            zx_write32(adapter->mmio + MMIO_MXU_START_ADDRESS + Reg_Pt_Inv_Mask_Offset*4,  reg_Pt_Inv_Mask.uint);
            zx_write32(adapter->mmio + MMIO_MXU_START_ADDRESS + Reg_Pt_Inv_Trig_Offset*4,  reg_Pt_Inv_Trig.uint);
        }


        reg_Pt_Inv_Trig.reg.Target       = PT_INV_TRIG_TARGET_UTLB;
        for (id = 0; id < 16; id++)
        {
            reg_Pt_Inv_Trig.reg.Proc_Id = id;
            reg_Pt_Inv_Addr.reg.Proc_Id = id;
            reg_Pt_Inv_Mask.reg.Proc_Id = id;
            zx_write32(adapter->mmio + MMIO_MXU_START_ADDRESS + Reg_Pt_Inv_Addr_Offset*4 , reg_Pt_Inv_Addr.uint);
            zx_write32(adapter->mmio + MMIO_MXU_START_ADDRESS + Reg_Pt_Inv_Mask_Offset*4,  reg_Pt_Inv_Mask.uint);
            zx_write32(adapter->mmio + MMIO_MXU_START_ADDRESS + Reg_Pt_Inv_Trig_Offset*4,  reg_Pt_Inv_Trig.uint);
        }
    }

    //TODO: restore private gart table.

    //restore bl buffer
    if(mm_mgr->bl_reserved_memory && mm_mgr->bl_buf_backup)
    {
        zx_memcpy( mm_mgr->bl_reserved_memory->krnl_cpu_vma->virt_addr, mm_mgr->bl_buf_backup, mm_mgr->bl_reserved_memory->size);
        zx_free(mm_mgr->bl_buf_backup);
        mm_mgr->bl_buf_backup = NULL;
    }
}

static void vidmm_dump_ptes_e3k(struct os_printer *p, unsigned int *virt, unsigned int large_page, int level, int index)
{
    vidmm_vm_pte_elt3k_t pte = {0, };

    if (level == ZX_PT_LEVEL_ROOT)
    {
        pte.value = virt[index];
        if (pte.pte_level1.pte.Valid)
        {
            zx_printf(p, "  %d:%d Snoop:%d Segment:%d Addr:0x%x\n",
                    level, index, pte.pte_level1.pte.Snoop,
                    pte.pte_level1.pte.Segment, pte.pte_level1.pte.Addr);
        }
    }
    else if (level == ZX_PT_LEVEL_1)
    {
        pte.value = virt[index];
        if (large_page)
        {
            if (pte.pte_level2._64k_d.Valid)
            {
                zx_printf(p, "  %d:%d Snoop:%d Segment:%d LargePage:%d Addr:0x%x\n",
                        level, index, pte.pte_level2._64k_d.Snoop,
                        pte.pte_level2._64k_d.Segment, pte.pte_level2._64k_d.LargePage,
                        pte.pte_level2._64k_d.Addr);
            }
        }
        else
        {
            if (pte.pte_level2._4k_d.Valid)
            {
                zx_printf(p, "  %d:%d Snoop:%d Segment:%d LargePage:%d Addr:0x%x\n",
                        level, index, pte.pte_level2._4k_d.Snoop,
                        pte.pte_level2._4k_d.Segment, pte.pte_level2._4k_d.LargePage,
                        pte.pte_level2._4k_d.Addr);
            }
        }
    }
    else if (level == ZX_PT_LEVEL_2)
    {
        pte.value = virt[index];
        if (large_page)
        {
            if (pte.pte_level3.pte_64k.Valid)
            {
                zx_printf(p, "  %d:%d Snoop:%d Segment:%d Addr:0x%x\n",
                        level, index, pte.pte_level3.pte_64k.Snoop,
                        pte.pte_level3.pte_64k.Segment,
                        pte.pte_level3.pte_64k.Addr);
            }
        }
        else
        {
            if (pte.pte_level2._4k_d.Valid)
            {
                zx_printf(p, "  %d:%d Snoop:%d Segment:%d Addr:0x%x\n",
                        level, index, pte.pte_level3.pte_4k.Snoop,
                        pte.pte_level3.pte_4k.Segment,
                        pte.pte_level3.pte_4k.Addr);
            }
        }
    }
}

static void vidmm_dump_mapped_entity_e3k(adapter_t *adapter, void * mapped_entity, ZX_VM_NODE_TYPE mapped_entity_type, unsigned long long address, unsigned long long start_addr)
{
    unsigned int           pte_snoop, pte_segment, page_idx, page_offset, Addr;
    struct os_pages_memory *pages_mem;
    fb_pages_memory_t      *fb_pages_mem;
    unsigned long long     pte_pa;
    unsigned long long     offset_size = address - start_addr;
    unsigned long long     gpu_offset_size = offset_size & ~0xfff;

    if (address <  adapter->shared_gpu_va_size)
    {
        vidmm_mgr_t *mm_mgr     = adapter->mm_mgr;
        PTE_L3_t pte_level3     = {0};
        unsigned int pte_offset = (address >> GPU_PAGE_SHIFT);

        pte_level3.uint = *(((unsigned int *)mm_mgr->vm_info->share_level3_cpu_va_offset) + pte_offset);

        zx_info("(level3 in gart_table) Snoop:%d Segment:%d Addr:0x%x\n",
                pte_level3.pte_4k.Snoop, pte_level3.pte_4k.Segment, pte_level3.pte_4k.Addr);
    }

    if (mapped_entity_type == ZX_VM_NODE_TYPE_SEGMENET_MEMORY)
    {
        vidmm_segment_memory_t *segment_memory = mapped_entity;

        pte_snoop   = segment_memory->flag.snooping_enabled;
        pte_segment = (segment_memory->flag.local) ? 1 : 0;

        if(pte_segment == 0)
        {
            pages_mem   = segment_memory->pages_mem;
            if (!pages_mem)
            {
                zx_info("(mapped_entity's info) segment_memory->pages_mem is NULL\n");
                return;
            }

            page_idx    = (gpu_offset_size >> adapter->os_page_shift);
            page_offset = gpu_offset_size & (adapter->os_page_size - 1);

            pte_pa = zx_get_page_phys_address(pages_mem, page_idx, NULL);
            Addr = vidsch_get_pte_value(adapter, pte_snoop, pte_pa + page_offset)>> 12;
        }
        else
        {
            fb_pages_mem = segment_memory->fb_pages_mem;
            if (!fb_pages_mem)
            {
                zx_info("(mapped_entity's info) segment_memory->fb_pages_mem is NULL\n");
                return;
            }

            pte_pa = fb_pages_mem->range_node_list[0]->start +  gpu_offset_size;
            Addr   = pte_pa >> 12;
        }

        zx_info("(mapped_entity's info) mapped_entity_type: %d, Snoop:%d Segment:%d: Addr: 0x%x\n",
                mapped_entity_type, pte_snoop, pte_segment, Addr);
    }
    else if (mapped_entity_type == ZX_VM_NODE_TYPE_ALLOCATION)
    {
        struct range_node  **current_node;
        vidmm_allocation_t *allocation = mapped_entity;
        vidmm_segment_t    *segment    = allocation->adapter->mm_mgr->segment + allocation->segment_id;
        pte_snoop    = segment->flags.support_snoop;
        pte_segment  = segment->flags.local ? 1 : 0;

        if(pte_segment == 0)
        {
            pages_mem   = allocation->pages_mem;
            if (!pages_mem)
            {
                zx_info("(mapped_entity's info) allocation->pages_mem is NULL\n");
                return;
            }

            page_idx    = (gpu_offset_size >> adapter->os_page_shift);
            page_offset = gpu_offset_size & (adapter->os_page_size -1);

            pte_pa = zx_get_page_phys_address(pages_mem, page_idx, NULL);
            Addr = vidsch_get_pte_value(adapter, pte_snoop, pte_pa + page_offset)>> 12;
        }
        else
        {
            fb_pages_mem = allocation->fb_pages_mem;
            if (!fb_pages_mem)
            {
                zx_info("(mapped_entity's info) allocation->fb_pages_mem is NULL\n");
                return;
            }

            current_node = &fb_pages_mem->range_node_list[0];

            while((*current_node)->size < gpu_offset_size)
            {
                gpu_offset_size -= (*current_node)->size;
                current_node++;

                if ((*current_node) == NULL)
                {
                    zx_info("(mapped_entity's info) *current_node is NULL\n");
                    return;
                }
            }

            pte_pa = (*current_node)->start +  gpu_offset_size;
            Addr   = pte_pa >> 12;
        }

        zx_info("(mapped_entity's info) mapped_entity_type: %d, Snoop:%d Segment:%d: Addr: 0x%x\n",
                mapped_entity_type, pte_snoop, pte_segment, Addr);
    }
}

void vidmm_init_private_vma_space_e3k(adapter_t *adapter)
{
    unsigned int private_vm_id = adapter->private_vm_id;

    Reg_Pt_Inv_Trig reg_Pt_Inv_Trig                 = {0};
    Reg_Pt_Inv_Addr reg_Pt_Inv_Addr                 = {0};
    Reg_Pt_Inv_Mask reg_Pt_Inv_Mask                 = {0};

    reg_Pt_Inv_Trig.reg.Proc_Id = private_vm_id;
    reg_Pt_Inv_Addr.reg.Proc_Id = private_vm_id;
    reg_Pt_Inv_Mask.reg.Proc_Id = private_vm_id;

    reg_Pt_Inv_Trig.reg.Target       = PT_INV_TRIG_TARGET_UTLB;
    zx_write32(adapter->mmio + MMIO_MXU_START_ADDRESS + Reg_Pt_Inv_Addr_Offset*4 , reg_Pt_Inv_Addr.uint);
    zx_write32(adapter->mmio + MMIO_MXU_START_ADDRESS + Reg_Pt_Inv_Mask_Offset*4,  reg_Pt_Inv_Mask.uint);
    zx_write32(adapter->mmio + MMIO_MXU_START_ADDRESS + Reg_Pt_Inv_Trig_Offset*4,  reg_Pt_Inv_Trig.uint);
}

void vidmm_update_private_vma_space_e3k(adapter_t *adapter, void *parent_buffer_cpu_va, vidmm_vm_pt_t *entry, unsigned int cur_pte_idx, unsigned int level)
{
    unsigned int private_vm_id = adapter->private_vm_id;
    unsigned long long pt = vidmm_get_segment_memory_gpuva_cont(entry->buffer, 0);
    vidmm_segment_memory_t *segment_memory = entry->buffer;
    vidmm_vm_pte_elt3k_t       pte_elt3k={ 0 };
    unsigned int       pte_security  = !segment_memory->flag.security;
    unsigned int       pte_segment   = (segment_memory->flag.local) ? 1 : 0;
    unsigned int       pte_snoop     = segment_memory->flag.snooping_enabled;
    unsigned long long pte_pa        = 0;
    unsigned int          reg_offset = 0;

    if(pte_segment == 0)
    {
        pte_pa = vidsch_get_pte_value(adapter, pte_snoop, zx_get_page_phys_address(segment_memory->pages_mem, 0, NULL));
    }
    else
    {
        pte_pa = segment_memory->fb_pages_mem->range_node_list[0]->start;//per 4K one pte
    }

    if (level == ZX_PT_LEVEL_1)
    {
        pte_elt3k.pte_level1.pte.Valid   = 1;
        pte_elt3k.pte_level1.pte.NStable = pte_security;
        pte_elt3k.pte_level1.pte.Snoop   = pte_snoop;
        pte_elt3k.pte_level1.pte.Segment = pte_segment;
        pte_elt3k.pte_level1.pte.Addr    = pte_pa >> 12;//todo: check 32bits pa

        *((unsigned int *)parent_buffer_cpu_va + cur_pte_idx) = pte_elt3k.value;

        //write ttbr through mmio
        reg_offset = MMIO_MMU_START_ADDRESS + Reg_Mmu_Ttbr_Offset * 4 + private_vm_id * 256 * 4 + 4 * cur_pte_idx;
        zx_write32(adapter->mmio + reg_offset, pte_elt3k.value);
    }
    else if (level == ZX_PT_LEVEL_2)
    {
        pte_elt3k.pte_level2._4k_d.Valid     = 1;
        pte_elt3k.pte_level2._4k_d.NStable   = pte_security;
        pte_elt3k.pte_level2._4k_d.Snoop     = pte_snoop;
        pte_elt3k.pte_level2._4k_d.Segment   = pte_segment;
        pte_elt3k.pte_level2._4k_d.LargePage = 0;
        pte_elt3k.pte_level2._4k_d.Addr      = pte_pa >> 12;

        *((unsigned int *)parent_buffer_cpu_va + cur_pte_idx) = pte_elt3k.value;
    }
    else
    {
        zx_assert(0, "invalid level");
    }

    entry->addr = pt;
    entry->age = adapter->resume_age;
}

vidmm_chip_func_t   vidmm_chip_func =
{
    .describe_allocation = vidmm_describe_allocation_e3k,
    .query_chip_vm_info = vidmm_query_chip_vm_info_e3k,
    .update_ptes = vidmm_update_ptes_e3k,
    .dump_ptes = vidmm_dump_ptes_e3k,
    .dump_mapped_entity = vidmm_dump_mapped_entity_e3k,
    .init_shared_pt = vidmm_init_shared_pt_e3k,
    .map_shared_buffer = vidmm_map_shared_buffer_e3k,
    .alloc_bl_slot = vidmm_alloc_bl_slot_e3k,
    .free_bl_slot = vidmm_free_bl_slot_e3k,
    .init_bl_heap = vidmm_init_bl_heap_e3k,
    .fini_bl_heap = vidmm_fini_bl_heap_e3k,
    .build_paging_buffer = vidmm_build_page_buffer_e3k,
    .save = vidmm_save_e3k,
    .restore = vidmm_restore_e3k,
    .init_segments = vidmm_init_segments_e3k,
    .fini_segments = vidmm_fini_segments_e3k,
    .init_private_vma_space = vidmm_init_private_vma_space_e3k, //patch for cne001
    .update_private_vma_space = vidmm_update_private_vma_space_e3k, //patch for cne001
};



