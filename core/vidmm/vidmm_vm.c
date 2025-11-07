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

//the max update pte count is dependent by the blit command, with max count 2^16.
#define MAX_UPDATE_PTE_COUNT  65536

//the max update vm dma size is dependent by the MAX_UPDATE_PTE_COUNT and extra size for
// the blit command size.
int vidmm_vm_alloc_pd(zx_vma_space_t *vma_space, vidmm_vm_pt_t *parent, unsigned long long saddr, unsigned long long eaddr, unsigned int level, int update_private_vma);
static void vidmmi_vm_fini_zone(zx_vma_space_t *vma);
/*
* the pt_levels_num include the page directory and the leaf page table.
* the layout of pd and pt:
* [root_pd][level1_pd][level2_pd]...[leaf_pd][pt]
*/
static inline unsigned int vidmmi_vm_pt_level_num(adapter_t *adapter)
{
    vidmm_mgr_t          *mm_mgr    = adapter->mm_mgr;
    vidmm_chip_vm_info_t *chip_info = (vidmm_chip_vm_info_t *)mm_mgr->vm_info;

    return chip_info->pt_levels_num;
}

static inline unsigned vidmmi_vm_leaf_pd_level(adapter_t *adapter)
{
    return (vidmmi_vm_pt_level_num(adapter) - 2);
}

static inline unsigned int vidmmi_vm_pt_bits_count(adapter_t *adapter, unsigned int level)
{
    vidmm_mgr_t          *mm_mgr    = adapter->mm_mgr;
    vidmm_chip_vm_info_t *chip_info = (vidmm_chip_vm_info_t *)mm_mgr->vm_info;

    return chip_info->pt_bits_array[0][level];
}

static inline unsigned int vidmmi_vm_pt_total_bits(adapter_t *adapter)
{
    vidmm_mgr_t          *mm_mgr    = adapter->mm_mgr;
    vidmm_chip_vm_info_t *chip_info = (vidmm_chip_vm_info_t *)mm_mgr->vm_info;

    unsigned int level, total_bits = 0;
    unsigned int level_num = vidmmi_vm_pt_level_num(adapter);
    for(level = 0; level < level_num; level ++)
    {
        total_bits += vidmmi_vm_pt_bits_count(adapter, level);
    }

    return total_bits;
}

static unsigned long long vidmmi_vm_pt_addr_mask(adapter_t *adapter, unsigned int level)
{
    vidmm_mgr_t          *mm_mgr    = adapter->mm_mgr;
    vidmm_chip_vm_info_t *chip_info = (vidmm_chip_vm_info_t *)mm_mgr->vm_info;

    return chip_info->pt_bits_mask[0][level];
}

//return the pd bits upper one pd's level.
static inline unsigned int vidmmi_vm_accum_pd_bits_count(adapter_t *adapter, unsigned int level)
{
    int i = 0;
    unsigned int pd_bits = 0;
    unsigned int level_num = vidmmi_vm_pt_level_num(adapter);
    for(i = level + 1; i < level_num; i ++)
    {
        pd_bits += vidmmi_vm_pt_bits_count(adapter, i);
    }
    return pd_bits;
}

static inline unsigned int vidmmi_vm_pt_num_entries(adapter_t *adapter, unsigned int level)
{
    return (1 << vidmmi_vm_pt_bits_count(adapter, level));
}

static inline unsigned int vidmmi_vm_pt_buffer_size(adapter_t *adapter, unsigned int level)
{
    return vidmmi_vm_pt_num_entries(adapter, level) * GPU_PTE_SIZE;
}

//todo: refine the set/clear bit logic.
static inline void vidmmi_vm_pt_update_bitmap(zx_vma_space_t *vma_space, vidmm_vm_pt_t *entry, unsigned int s_idx, unsigned int e_idx, unsigned int value)
{
    unsigned int   i      = 0;
    unsigned long *bitmap = entry->update_bitmap;
    unsigned int   bit = 0;

    if(bitmap == NULL) return;

    if(value == 1)
    {
        vm_trace("%s: set bit @ %x~%x\n", __func__, s_idx, e_idx);
        for(i = s_idx; i <= e_idx; i ++)
        {
            zx_set_bit(i, bitmap);
        }
    }

    if(value == 0)
    {
        vm_trace("%s: clear bit @ %x~%x\n", __func__, s_idx, e_idx);

        for(i = s_idx; i <= e_idx; i ++)
        {
            zx_clear_bit(i, bitmap);
        }
    }

    //for debug
    bit = zx_find_first_bit(bitmap, 1024);//use the max 1024
    vm_trace("bit: %x\n", bit);

}

static inline int vidmmi_vm_pt_check_bitmap(zx_vma_space_t *vma_space, vidmm_vm_pt_t *entry, unsigned int s_idx, unsigned int e_idx)
{
    unsigned int   i      = 0;
    unsigned int   exist  = 0;
    unsigned long *bitmap = entry->update_bitmap;

    zx_assert(bitmap != NULL, "");

    for(i = s_idx; i <= e_idx; i ++)
    {
        if(zx_test_bit(i, bitmap))
        {
            exist = 1;
        }
        else
        {
            vm_trace("pt bitmap check failed, need new one @ %x.\n", i);
            exist &= 0;
            break;
        }
    }

    return exist;
}


void vidmmi_vm_get_node_info(zx_vm_node_t *node, unsigned long long *offset, unsigned long long *size)
{
    if(offset != NULL)
    {
        *offset = node->range->start;
    }
    if(size != NULL)
    {
        *size = node->range->size;
    }
}

/**
* vidmm_vm_init_zone: initialize one vm zone of one vma space.
* @ vma_space
* @ adapter
* @ zone
* @ id: the index in zone array return by the vidmm.
*         the last item of this array is the shared vm zone.
*/
int vidmmi_vm_init_zone(zx_vma_space_t *vma_space, unsigned int id)
{
    vidmm_mgr_t           *mm_mgr          = vma_space->adapter->mm_mgr;
    vidmm_chip_vm_info_t  *chip_info       = (vidmm_chip_vm_info_t *)mm_mgr->vm_info;
    zx_vm_zone_desc_t     *zone_desc       = &chip_info->desc[id];
    int                    result          = S_OK;
    zx_vm_zone_t          *zone             = NULL;

    if(zone_desc->gpu_vm_size == 0)
    {
        return result;
    }

    zone = zx_calloc(sizeof(*zone));
    zone->zone_id                      = zone_desc->zone_id;
    zone->gpu_vm_start                 = zone_desc->gpu_vm_start;
    zone->gpu_vm_size                  = zone_desc->gpu_vm_size;
    zone->reserved_vm_size             = zone_desc->reserved_vm_size;
    zone->flags.share                  = zone_desc->flags.share;
    zone->vma_space                    = vma_space;
    zone->lock = zx_create_mutex();

    if (zone->zone_id != VM_ZONE_ID_SHARE_SPACE_LOCAL)
    {
        zone->ra = zx_create_range_allocator(zone->gpu_vm_start, zone->gpu_vm_size, GPU_PAGE_SIZE);
    }

    vma_space->zones[id] = zone;

    vm_trace("%s: zone[%d]: range: [0x%llx -> 0x%llx], size: 0x%llx, reserved: 0x%llx.\n",  __func__, \
              zone->zone_id, zone->gpu_vm_start, zone->gpu_vm_start + zone->gpu_vm_size - 1,  \
              zone->gpu_vm_size, zone->reserved_vm_size);

    return result;
}



/**
* vidmm_vm_init_vma_space: inialize the vma space instance. the owner can be one given device(process) or KMD internal use.
*
* @vma_space: the request vma space to be initialized. the memory space of the vma_space should be
*             allocated before calling this interface.
* @adapter: the physical adapter of the vma space instance bind to. for LDA case, the adapter is a adapter list,
*           and the variable 'adapter' refers to the head adapter.
* @adapter_num: specify the number of the adapter. if this value bigger than 1, indicates the LDA case, and the para @adapter
*               is the list of adapter.
*/
int vidmm_vm_init_vma_space(zx_vma_space_t *vma_space, adapter_t *adapter)
{
    int result = S_OK;
    int i    = 0;

    //use the head adapter's mm_mgr to do the common initialize for the vma space.
    vidmm_mgr_t          *mm_mgr         = adapter->mm_mgr;
    vidmm_chip_vm_info_t *vm_info        = (vidmm_chip_vm_info_t *)mm_mgr->vm_info;
    zx_vm_zone_t         *zone           = NULL;
    int                   zone_id        = 0;
    unsigned int          pt_seg_id      = mm_mgr->vm_info->share_pt_segment_id;
    unsigned int          local_seg_id   = 1;
    unsigned int          unsnoop_seg_id = 2;
    char *pt_buffer_cpu_va = NULL;

    vm_trace("enter %s, vma_space: %p, adapter->vma_space: %p\n", __func__, vma_space, adapter->reserved_vma);

    if(NULL == vma_space)
    {
        zx_error(" vma space is NULL. confirm the memory of this object be allocated before.\n");
        result = E_INVALIDARG;
        goto exit;
    }

    //For LDA case, each adapter has the same vma zone.
    vma_space->zone_cnt = vm_info->zone_cnt;
    vma_space->root_pd_age = 1;
    vma_space->lock = zx_create_mutex();
    vma_space->adapter = adapter;

    for (i = 0; i < MAX_ENGINE_COUNT; i++)
    {
        vma_space->utlb_dirtys[i].dirty = TRUE;
        vma_space->dtlb_dirtys[i].dirty = TRUE;
    }

    //the shared vma space
    if(adapter->reserved_vma == vma_space)
    {
        mm_mgr->chip_func->init_shared_pt(adapter);
        vma_space->adapter = adapter;

        vidmmi_vm_init_zone(vma_space, VM_ZONE_ID_SHARE_SPACE_LOCAL);
        vidmmi_vm_init_zone(vma_space, VM_ZONE_ID_SHARE_SPACE_PCIE);
    }
    else
    {
        vidmmi_vm_init_zone(vma_space, VM_ZONE_ID_PRIVATE_SPACE);
    }

    //init the page table for the vma space, each adapter owns independent page table state.
    if(adapter->reserved_vma != vma_space)
    {
        zx_vm_pt_info_t *pt_info  = &vma_space->pt_info;
        vidmm_vm_pt_t   *root     = &pt_info->root;
        unsigned int     size     = 0;

        root->update_bitmap = zx_calloc(vidmmi_vm_pt_num_entries(adapter, ZX_PT_LEVEL_ROOT) >> 3);
        zx_assert(root->update_bitmap, "");

        size = vidmmi_vm_pt_buffer_size(adapter, ZX_PT_LEVEL_ROOT);
        if (adapter->ctl_flags.page_table_local_enable)
        {
            root->buffer = vidmm_create_segment_memory(adapter, local_seg_id, size, 1, 1);
            if (!root->buffer)
                zx_warning("allocate root page table buffer from local failed, size %d Byte\n", size);
        }

        if (!root->buffer && adapter->ctl_flags.page_table_pcie_enable) // not local_only
        {
            root->buffer = vidmm_create_segment_memory(adapter, unsnoop_seg_id, size, 1, 1);
        }

        if(NULL == root->buffer)
        {
            zx_error("allocate root page table buffer failed, with size %d Byte\n", size);

            vidmmi_vm_fini_zone(vma_space);
            zx_free(vma_space);
            if(root->update_bitmap) zx_free(root->update_bitmap);

            result = E_OUTOFMEMORY;
            goto exit;
        }

        vm_trace("finish create root buffer: %p, memory: %p, size: %d\n", root->buffer, root->buffer->fb_pages_mem, root->buffer->size);

        result = vidmm_map_segment_memory(adapter, root->buffer);
        if(result != S_OK)
        {
            zx_error("map segment memory failed, with err:0x%x\n", result);
            if(root->buffer->gpu_vm != NULL)
            {
                vidmm_unmap_segment_memory(root->buffer);
            }
            vidmm_destroy_segment_memory(adapter, root->buffer);
            goto exit;
        }

        root->addr = vidmm_get_segment_memory_gpuva_cont(root->buffer, 0);


        if (adapter->chip_id == CHIP_CNE001)
        {
            result = vidmm_vm_alloc_pd(vma_space, root, adapter->shared_gpu_va_size >> GPU_PAGE_SHIFT, (adapter->shared_gpu_va_size + adapter->private_size_of_cne001 - 1) >> GPU_PAGE_SHIFT, ZX_PT_LEVEL_ROOT, 1);
            if(result != S_OK)
            {
                goto exit;
            }

            /* use mmio to invalidate_tlb in private_vma */
            mm_mgr->chip_func->init_private_vma_space(adapter);
        }
    }
exit:
    return result;
}

static zx_vm_node_t* vidmmi_vm_allocate_node(struct range_allocator *ra, unsigned long long prefer_start_addr, unsigned long long size, unsigned long long alignment, unsigned int force_start)
{
    zx_vm_node_t *vm_node = zx_calloc(sizeof(zx_vm_node_t));

    if (prefer_start_addr)
    {
        vm_node->range = ra->reserve(ra, prefer_start_addr, size);
        if (!vm_node->range && force_start)
            goto done;
    }

    if (!vm_node->range)
    {
        vm_node->range = ra->alloc(ra, size, alignment, 0, -1, 0);
    }

    if(vm_node->range)
    {
        vm_node->range->private = vm_node;
        vm_trace("%s vm_node:%p, size:%lld.\n", __func__, vm_node, size);
    }

done:
    if (!vm_node->range)
    {
        zx_free(vm_node);
        vm_node = NULL;
    }
    return vm_node;
}

static void vidmmi_vm_free_node(zx_vm_node_t *vm_node)
{
    if (vm_node->range)
    {
        vm_node->range->release(vm_node->range);
        vm_node->range = NULL;
    }

    zx_free(vm_node);
}

/*
* vidmmi_vm_get_pt: return the entry of page table memory of the specified start address.
* the pt's memory is allocated by the leaf pd.
*/
static inline vidmm_vm_pt_t* vidmmi_vm_get_pt_entry(zx_vma_space_t *vma_space, vidmm_vm_pt_t *root, unsigned long long start_addr, int *pt_index)
{
    adapter_t         *adapter       = vma_space->adapter;
    vidmm_vm_pt_t     *entry         = root;
    unsigned int       pt_total_bits = vidmmi_vm_pt_total_bits(adapter);
    unsigned int       level         = 0;
    unsigned int       idx           = 0;
    unsigned long long addr_partion  = 0;

    //only root and level1 has entry->entries
    while(entry->entries)
    {
        if(entry->buffer)
        {
            vm_trace(" entry. buffer: %p, size: %d\n", entry->buffer, entry->buffer->size);
        }
        else
        {
            vm_trace(" entry buffer is null\n");
            return NULL;
        }

        addr_partion = start_addr & vidmmi_vm_pt_addr_mask(adapter, level);
        idx          = addr_partion >> vidmmi_vm_accum_pd_bits_count(adapter, level);

        vm_trace("%s. addr_partion: 0x%x, idx: 0x%x\n", __func__, addr_partion, idx);

        entry = &entry->entries[idx];
        if(!entry)
        {
            vm_trace("entry is null\n");
            return NULL;
        }

        level ++;
    }

    //*entry         = root;
    // entry must be on level 2
    if(entry == NULL ||entry == root ||level != 2)//not find in the leaf pd.
    {
        vm_trace("not found the entry...\n");
        return NULL;
    }

    *pt_index = idx;

    vm_trace("%s: found entry. with addr: 0x%x, @idx: 0x%x, entry: %p\n", __func__, start_addr, idx, entry);

    return entry;
}

/*
* vidmmi_vm_release_pt: release the pt memory.
* called by vidmm_vm_unmap.
*
* @vma_space: the vma space.
* @start_addr: the start va address, should be PAGE_SIZE shift before.
* @end_addr:the end va address, should be PAGE_SIZE shift before.
*/
int vidmmi_vm_release_pt(zx_vma_space_t *vma_space, unsigned long long start_addr, unsigned long long end_addr)
{
    adapter_t            *adapter      = vma_space->adapter;
    zx_vm_pt_info_t      *pt_info      = &vma_space->pt_info;
    vidmm_vm_pt_t        *root         = &pt_info->root;
    vidmm_vm_pt_t        *entry        = NULL;
    unsigned int          level        = vidmmi_vm_leaf_pd_level(adapter);

    unsigned int          pt_index     = 0;
    int                   result       = S_OK;

    vm_trace("%s, Release pt of VA range[%llx,%llx]\n",__func__, start_addr, end_addr);

    //walk over the address range
    while(start_addr < end_addr)
    {
        unsigned long long       s_addr   = (start_addr >> GPU_PAGE_SHIFT);
        unsigned long long       e_addr   = (end_addr >> GPU_PAGE_SHIFT);
        unsigned int expect_pt_entry_num  = vidmmi_vm_pt_num_entries(adapter, level + 1);
        unsigned int expect_addr_mask     = expect_pt_entry_num - 1;
        unsigned int expect_pte_offset    = s_addr & expect_addr_mask;
        unsigned int expect_nptes         = 0;

        //intra pt case
        if((s_addr & ~expect_addr_mask) == (e_addr & ~expect_addr_mask))
        {
            expect_nptes = e_addr - s_addr + 1;
        }
        else//inter pt case
        {
            expect_nptes = expect_pt_entry_num - (s_addr & expect_addr_mask);
        }
        vm_trace("%s, Release pt of shifted VA range[%llx,%llx], expect_nptes:%d\n",__func__, s_addr, e_addr, expect_nptes);

        entry = vidmmi_vm_get_pt_entry(vma_space, root, s_addr, &pt_index);

        if(entry && entry->buffer)
        {
            vidmm_vm_pt_t      *parent             = entry->parent;

            unsigned int       pt_entry_num        = vidmmi_vm_pt_num_entries(adapter, level + 1);
            unsigned int       parent_pt_entry_num = vidmmi_vm_pt_num_entries(adapter, level);
            unsigned long long parent_addr_partion = (s_addr >> vidmmi_vm_accum_pd_bits_count(adapter, level));

            unsigned int       nptes             = 0;
            unsigned long long addr_mask         = pt_entry_num - 1;
            unsigned long long parent_addr_mask  = parent_pt_entry_num - 1;
            unsigned long long cur_pte_offset    = 0;
            unsigned long long parent_pte_offset = (parent_addr_partion & parent_addr_mask);

            unsigned long long tmp_saddr = s_addr, tmp_eaddr = e_addr;

            cur_pte_offset = tmp_saddr & addr_mask;

            if((tmp_saddr & ~addr_mask) == (tmp_eaddr & ~addr_mask))
            {
                nptes = tmp_eaddr - tmp_saddr + 1;
            }
            else//inter pt case
            {
                nptes = pt_entry_num - (tmp_saddr & addr_mask);
            }

            vidmmi_vm_pt_update_bitmap(vma_space, entry, cur_pte_offset, cur_pte_offset + nptes -1, 0);

            //still has some ptes exist.
            if(zx_find_first_bit(entry->update_bitmap, pt_entry_num) < pt_entry_num)
            {
                goto entry_end;
            }

            vm_trace("%s, destroy entry %p, buffer:%p\n", __func__, entry, entry->buffer);

            if (adapter->chip_id == CHIP_CNE001)
                goto entry_end;

            vidmm_destroy_segment_memory(adapter, entry->buffer);
            entry->buffer = NULL;
            entry->addr = 0;

            zx_free(entry->update_bitmap);
            entry->update_bitmap = NULL;

            zx_clear_bit(parent_pte_offset, parent->update_bitmap);
        }
        else
        {
            vm_trace("%s,not found entry when release pt, shifted va range[%llx,%llx]\n", __func__, s_addr, e_addr);
        }

entry_end:
        s_addr += expect_nptes;
        start_addr = (s_addr << GPU_PAGE_SHIFT);
    }

    return result;

}

static void vidmmi_vm_release_sparse_node(zx_sparse_node_t *node, void *priv)
{
    // remove the sparse space from vm space
    node->range->release(node->range);

    // release sub allocator
    node->ra->release(node->ra);

    zx_free(node);
}

void vidmm_vm_release_segment_memory(vidmm_segment_memory_t *segment_memory)
{
    zx_vm_node_t *node = segment_memory->gpu_vm;

    if(node)
    {
        zx_vm_zone_t *zone = node->zone;

        zx_mutex_lock(zone->vma_space->lock);
        vidmmi_vm_release_pt(zone->vma_space, node->range->start, node->range->start + node->range->size - 1);
        zx_mutex_unlock(zone->vma_space->lock);

        vidmmi_vm_free_node(node);
    }

    segment_memory->gpu_va = 0;
    segment_memory->gpu_vm = NULL;
}

static int vidmmi_vm_release_vm_node(struct range_node *range, void *priv)
{
    zx_vm_node_t *vm_node = range->private;

    switch(vm_node->mapped_entity_type)
    {
    case ZX_VM_NODE_TYPE_ALLOCATION:
        {
            vidmm_allocation_t *allocation = vm_node->mapped_entity;

            zx_mutex_lock(allocation->va_lock);
            list_del(&vm_node->mapped_list_item);
            zx_mutex_unlock(allocation->va_lock);

            zx_mutex_lock(vm_node->zone->vma_space->lock);
            vidmmi_vm_release_pt(vm_node->zone->vma_space, range->start, range->start + range->size - 1);
            zx_mutex_unlock(vm_node->zone->vma_space->lock);

            vidmmi_vm_free_node(vm_node);
        }
        break;
    case ZX_VM_NODE_TYPE_SEGMENET_MEMORY:
        {
            vidmm_vm_release_segment_memory(vm_node->mapped_entity);
        }
        break;
    default:
        break;
    }

    return 1;
}

static int vidmmi_vm_cleanup_node(struct range_node *range, void *priv)
{
    if ((long)range->private & 1)
    {
        // it's sparse node
        zx_sparse_node_t *sparse_node = (void*)((long)range->private & ~0x1);

        // release all vm_node related to this sparse_node
        sparse_node->ra->for_each_node(sparse_node->ra,
                range->start, range->start + range->size, NULL, vidmmi_vm_release_vm_node);

        // release the sparse node itself
        vidmmi_vm_release_sparse_node(sparse_node, priv);
    }
    else
    {
        // it's vm_node
        vidmmi_vm_release_vm_node(range, NULL);
    }

    return 1;
}

static void vidmmi_vm_fini_zone(zx_vma_space_t *vma)
{
    int i;
    zx_vm_zone_t *zone;

    for(i = 0; i < VM_ZONE_ID_MAX; i ++)
    {
        zone = vma->zones[i];
        if (zone)
        {
            zx_mutex_lock(zone->lock);
            if (zone->ra)
            {
                zone->ra->for_each_node(zone->ra, 0, -1, NULL, vidmmi_vm_cleanup_node);
                zone->ra->release(zone->ra);
                zone->ra = NULL;
            }
            zx_mutex_unlock(zone->lock);
            zx_destroy_mutex(zone->lock);
            zx_free(zone);
        }
        vma->zones[i] = NULL;
    }
}

void vidmm_vm_release_pd(adapter_t *adapter, vidmm_vm_pt_t *root)
{
    unsigned int root_pt_entry_num  = vidmmi_vm_pt_num_entries(adapter, ZX_PT_LEVEL_ROOT);

    if(!root) return;

    if(root && root->buffer)
    {
        unsigned int cur_root_pt_idx =  zx_find_first_bit(root->update_bitmap, root_pt_entry_num);
        while ( cur_root_pt_idx < root_pt_entry_num)
        {
            vidmm_vm_pt_t *entry = & root->entries[cur_root_pt_idx];

            if (adapter->chip_id == CHIP_CNE001)
            {
                unsigned int leaf_pt_entry_num = vidmmi_vm_pt_num_entries(adapter, ZX_PT_LEVEL_1);
                unsigned int cur_leaf_pt_idx = zx_find_first_bit(entry->update_bitmap, leaf_pt_entry_num);

                while (cur_leaf_pt_idx < leaf_pt_entry_num)
                {
                    vidmm_vm_pt_t *child_entry = &entry->entries[cur_leaf_pt_idx];
                    if(child_entry && child_entry->buffer)
                    {
                        vidmm_destroy_segment_memory(adapter, child_entry->buffer);
                        zx_free(child_entry->update_bitmap);

                        child_entry->update_bitmap = NULL;
                        child_entry->buffer = NULL;
                    }

                    cur_leaf_pt_idx = zx_find_next_bit(entry->update_bitmap, leaf_pt_entry_num, cur_leaf_pt_idx+1);
                }
            }

            if(entry && entry->buffer)
            {
                vidmm_destroy_segment_memory(adapter, entry->buffer);
                zx_free(entry->update_bitmap);

                entry->update_bitmap = NULL;
                entry->buffer = NULL;
            }
            if(entry->entries)
            {
                zx_free(entry->entries);
                entry->entries = NULL;
            }
            cur_root_pt_idx = zx_find_next_bit(root->update_bitmap, root_pt_entry_num, cur_root_pt_idx+1);
        }

        vidmm_destroy_segment_memory(adapter, root->buffer);
        zx_free(root->update_bitmap);
        if(root->entries)
            zx_free(root->entries);

        root->update_bitmap = NULL;
        root->buffer = NULL;
        root->entries = NULL;
    }
}

/**
* vidmm_vm_fini_vma_space: finialize the vma space instance. the owner can be one given device(process) or KMD internal use.
*
* @vma_space: the request vma space to be finialized.
* @adapter: the physical adapter of the vma space instance bind to. for LDA case, the adapter is a adapter list,
                  and the variable 'adapter' refers to the head adapter.
* @adapter_num: specify the number of the adapter. if this value bigger than 1, indicates the LDA case, and the para @adapter
*                        is the list of adapter.
*/
void vidmm_vm_fini_vma_space(zx_vma_space_t *vma_space)
{
    adapter_t *adapter = vma_space->adapter;
    zx_vm_pt_info_t *pt_info = &vma_space->pt_info;

    vidmmi_vm_fini_zone(vma_space);

    if(pt_info)
    {
        zx_mutex_lock(vma_space->lock);

        vidmm_vm_release_pd(adapter, &pt_info->root);

        zx_mutex_unlock(vma_space->lock);
    }

    if(adapter->reserved_vma == vma_space)
    {
        if (adapter->mm_mgr->share_pt_buffer)
        {
            vidmm_destroy_segment_memory(adapter, adapter->mm_mgr->share_pt_buffer);
            adapter->mm_mgr->share_pt_buffer = NULL;
        }
    }

    zx_destroy_mutex(vma_space->lock);
}


zx_vm_node_t *vidmmi_vm_prepare_allocate(gpu_device_t *device, vidmm_allocation_t *allocation, unsigned long long start_addr, unsigned long long size)
{
    zx_vm_node_t *find_node = NULL;
    zx_vm_node_t *node = NULL, *node_next;
    unsigned long long node_offset, node_size;

    zx_mutex_lock(allocation->va_lock);

    //the allocation's exist gpu_vm_node's  start address already with the 'start_addr'.
    list_for_each_entry_safe(node, node_next, &allocation->gpu_vm_list, mapped_list_item)
    {
        vidmmi_vm_get_node_info(node, &node_offset, &node_size);

        if((start_addr >= node_offset) && (size <= node_size))
        {
           find_node = node;
           break;
        }
    }

    zx_mutex_unlock(allocation->va_lock);
    //todo: the start va belong to another allocation?
    //

    return find_node;
}

static zx_vm_node_t* vidmm_vm_allocate_allocation(zx_vma_space_t *vma_space, gpu_device_t *device, vidmm_allocation_t *allocation, vidmm_vm_create_desc_args_t *arg)
{
    zx_vm_node_t    *vm_node         = NULL, *first = NULL;
    zx_vm_zone_t    *zone            = NULL;
    adapter_t        *adapter         = vma_space->adapter;
    unsigned int     alignment        = 0;
    unsigned int      result          = S_OK;
    int               zone_id         = 0;
    vidmm_segment_t *segment = allocation->adapter->mm_mgr->segment + allocation->segment_id;

    if (vma_space == adapter->reserved_vma)
    {
        zx_assert(!segment->flags.invalid, "share alloc should resident when create");

        zone_id = VM_ZONE_ID_SHARE_SPACE_PCIE;
        alignment = util_max(adapter->os_page_size, allocation->alignment);
    }
    else
    {
        zone_id = VM_ZONE_ID_PRIVATE_SPACE;

        if (segment->flags.invalid)
        {
            alignment = util_max(util_max(adapter->os_page_size, GPU_PAGE_SIZE), allocation->alignment);
        }
        else if (segment->flags.local)
        {
            alignment = util_max(GPU_PAGE_SIZE, allocation->alignment);
        }
        else
        {
            alignment = util_max(adapter->os_page_size, allocation->alignment);
        }
    }

    zone = vma_space->zones[zone_id];
    zx_mutex_lock(zone->lock);

    if(arg->size > zone->gpu_vm_size)
    {
        zx_error("%s: the request size (%lld) exceed the zone's max size (%lld)\n", __func__, arg->size, zone->gpu_vm_size);
        goto exit;
    }

    vm_node = vidmmi_vm_allocate_node(zone->ra, arg->prefer_start_addr, arg->size, alignment, arg->force_start);
    if(!vm_node)
    {
        zx_error("allocate vm node for allocation failed.");
        goto exit;
    }
    vm_trace("%s: gpu_va: %llx, allocation:%p, size: %llx-%llx\n", __func__, vm_node->range->start, allocation, arg->size, vm_node->range->size);

    vm_node->zone               = zone;
    vm_node->mapped_entity      = allocation;
    vm_node->mapped_entity_type = ZX_VM_NODE_TYPE_ALLOCATION;
    vm_node->offset             = arg->offset;
    vm_node->age                = allocation->resident_age - 1;

    zx_mutex_lock(allocation->va_lock);
    list_add_tail(&vm_node->mapped_list_item, &allocation->gpu_vm_list);
    vm_trace("%s node:%p, node->zone:%p, allocation:%p, device:%p.\n",
            __func__, vm_node, vm_node->zone, allocation, device);
    zx_mutex_unlock(allocation->va_lock);

exit:
    zx_mutex_unlock(zone->lock);
    return vm_node;
}

zx_vm_zone_t *vidmm_vm_get_zone_from_range(zx_vma_space_t *vma_space, unsigned long long base, unsigned long long size)
{
    adapter_t *adapter = vma_space->adapter;
    zx_vma_space_t *share_space = adapter->reserved_vma;

    if (base >= adapter->shared_gpu_va_offset && base + size <= adapter->shared_gpu_va_offset + adapter->shared_gpu_va_size)
    {
        if (base <= adapter->shared_gpu_va_offset + adapter->total_local_memory_size)
        {
            // share local
            return share_space->zones[VM_ZONE_ID_SHARE_SPACE_LOCAL];
        }
        else
        {
            // share pcie
            return share_space->zones[VM_ZONE_ID_SHARE_SPACE_PCIE];
        }
    }
    else
    {
        zx_vm_zone_t *zone = vma_space->zones[VM_ZONE_ID_PRIVATE_SPACE];
        if (base >= zone->gpu_vm_start && base + size <= zone->gpu_vm_start + zone->gpu_vm_size)
        {
            // private
            return zone;
        }
    }

    return NULL;
}

zx_vm_node_t* vidmm_vm_allocate_sparse_vm_node(zx_vma_space_t *vma_space, vidmm_allocation_t *allocation, unsigned long long base, unsigned long long size, unsigned long long offset)
{
    int result = S_OK;
    zx_vm_zone_t *zone = vidmm_vm_get_zone_from_range(vma_space, base, size);
    struct range_allocator *ra = NULL;
    struct range_node *range = NULL;
    zx_vm_node_t *vm_node = NULL;

    if (!zone)
    {
        zx_error("can't find any zone contain the range [%llx,%llx]\n", base, size);
        return NULL;
    }

    if (zone->zone_id == VM_ZONE_ID_SHARE_SPACE_LOCAL)
    {
        return NULL;
    }

    ra = zone->ra;
    zx_mutex_lock(zone->lock);
    range = ra->lookup(ra, base, size);

    if ((long)range->private & 1)
    {
        // it is an sparse node
        zx_sparse_node_t *sparse_node = (void*)((long)range->private & ~0x1);

        vm_node = vidmmi_vm_allocate_node(sparse_node->ra, base, size, 4096, 1);
        if (!vm_node)
        {
            goto done;
        }
        vm_node->zone               = zone;
        vm_node->mapped_entity      = allocation;
        vm_node->mapped_entity_type = ZX_VM_NODE_TYPE_ALLOCATION;
        vm_node->offset             = offset;
        vm_node->age                = allocation->resident_age - 1;

        zx_mutex_lock(allocation->va_lock);
        list_add_tail(&vm_node->mapped_list_item, &allocation->gpu_vm_list);
        vm_trace("%s node:%p, node->zone:%p, allocation:%p.\n",
                __func__, vm_node, vm_node->zone, allocation);
        zx_mutex_unlock(allocation->va_lock);
    }
    else
    {
        zx_error("it's not an sparse range!!!!!!\n");
    }
done:
    zx_mutex_unlock(zone->lock);
    return vm_node;
}

int vidmm_vm_allocate_segment_memory(adapter_t *adapter, vidmm_segment_memory_t *segment_memory)
{
    zx_vma_space_t          *vma_space     = adapter->reserved_vma;
    unsigned int            result         = S_OK;
    zx_vm_node_t          *vm_node        = NULL;
    zx_vm_zone_t          *zone           = NULL;

    unsigned int            alignment      = segment_memory->aligned_page_size;
    int                     zone_id        = 0;
    unsigned long long      size           = segment_memory->size;


    //for the shared local memory, the gpu va is directly mapping with pa, with the offset as
    //legacy_offset in mmu_mode.
    if(segment_memory->flag.local)
    {
        if(segment_memory->fb_pages_mem != NULL)
        {
            vm_trace("%s. shared direct mapping on local\n", __func__);
            zx_assert(segment_memory->fb_pages_mem->range_node_num == 1, "");
            segment_memory->gpu_va = adapter->shared_gpu_va_offset + segment_memory->fb_pages_mem->range_node_list[0]->start;
        }
        else
        {
            zx_error("%s. directly mapping error, forget create the segment memory first?\n");
            result = E_FAIL;
        }
        goto exit;
    }

    zx_assert(adapter->hw_caps.share_pcie_enable, "share pcie not enable");

    zone_id = VM_ZONE_ID_SHARE_SPACE_PCIE ;

    zone = vma_space->zones[zone_id];

    zx_mutex_lock(zone->lock);
    vm_node = vidmmi_vm_allocate_node(zone->ra, 0, size, alignment, 0);
    if(NULL == vm_node)
    {
        zx_error("allocate vm node for segment memory failed, with size: %lld\n", size);
        result = E_FAIL;
        zx_mutex_unlock(zone->lock);
        goto exit;
    }

    vm_node->zone               = zone;
    vm_node->mapped_entity      = segment_memory;
    vm_node->mapped_entity_type = ZX_VM_NODE_TYPE_SEGMENET_MEMORY;

    segment_memory->gpu_vm = vm_node;
    vidmmi_vm_get_node_info(vm_node, &segment_memory->gpu_va, NULL);
    zx_mutex_unlock(zone->lock);

exit:
    return result;

}

void vidmm_vm_release_allocation(vidmm_allocation_t *allocation)
{
    unsigned int     zone_id   = 0;
    zx_vm_zone_t   *zone      = NULL;
    zx_vma_space_t *vma_space = NULL;
    zx_vm_node_t *node = NULL, *node_next = NULL;

retry:
    zx_mutex_lock(allocation->va_lock);

    list_for_each_entry_safe(node, node_next, &allocation->gpu_vm_list, mapped_list_item)
    {
        zone = node->zone;

        if (ZX_LOCKED == zx_mutex_trylock(zone->lock))
        {
            vm_trace("%s alloc:%p, node:%p, zone:%p.\n", __func__, allocation, node, zone);

            vidmmi_vm_release_vm_node(node->range, NULL);

            zx_mutex_unlock(zone->lock);
        }
        else
        {
            // avoid deadlock since general case was lock zone->lock first then allocation->va_lock
            zx_mutex_unlock(allocation->va_lock);

            zx_msleep(2);

            goto retry;
        }
    }

    zx_mutex_unlock(allocation->va_lock);
}


unsigned long long vidmm_vm_get_pde_addr(zx_vma_space_t *vma_space, vidmm_segment_memory_t *segment_memory, unsigned int page_idx)
{
    unsigned long long pde_addr = 0;

    if(vma_space->flag.cpu_update_pte)
    {
        pde_addr = ptr_to_ptr64(vidmm_get_segment_memory_cpu_va(segment_memory));
    }
    else
    {
        pde_addr = vidmm_get_segment_memory_gpuva_cont(segment_memory, page_idx);
    }

    zx_assert(pde_addr != 0, "");

    return pde_addr;
}

/*
* vidmm_vm_alloc_pd: allocate the page directory for a given va range, include set up the page table translation for the
*                    the pd buffer.
* @ vma_space: the vma space of this pd represent to.
* @ parent: the parent page directory.
* @ start_addr: the start va address in current page directory.
* @ end_addr: the end va address in current page directory.
* @ level: the level of the page directory.
* @ update_private_vma: determine whether update private_vma
*/
int vidmm_vm_alloc_pd(zx_vma_space_t *vma_space, vidmm_vm_pt_t *parent, unsigned long long saddr, unsigned long long eaddr, unsigned int level, int update_private_vma)
{
    unsigned int result        = S_OK;
    unsigned int cur_pte_idx   = 0;
    unsigned int from, to = 0;
    unsigned int skip_cur_level = 0;
    void  *parent_buffer_cpu_va = 0;

    adapter_t   *adapter = vma_space->adapter;
    vidmm_mgr_t *mm_mgr  = adapter->mm_mgr;
    unsigned int          local_seg_id   = 1;
    unsigned int          unsnoop_seg_id = 2;

    unsigned int pd_addr_shift = vidmmi_vm_accum_pd_bits_count(adapter, level);

    vm_trace("%s, shift va range[%llx,%llx] for level:%d\n", __func__, saddr, eaddr, level);

    if (update_private_vma)
    {
        vidmm_lock_segment_memory(adapter, parent->buffer);
        parent_buffer_cpu_va = parent->buffer->krnl_cpu_vma->virt_addr;
    }

    if(NULL == parent->entries)
    {
        int num_entry = vidmmi_vm_pt_num_entries(adapter, level);

        parent->entries = zx_calloc(num_entry * sizeof(vidmm_vm_pt_t));
        if(NULL == parent->entries)
        {
            zx_error("allocate level: %d page directory entry failed.\n", level);
            result = E_OUTOFMEMORY;
            goto exit;
        }
    }

    from = saddr >> pd_addr_shift;
    to   = eaddr >> pd_addr_shift;

    vm_trace("%s(%d): s_idx: %x, e_idx: %x\n", __func__, level, from, to);

    //va range validate check
    if(from >= vidmmi_vm_pt_num_entries(adapter,level)||
         to >= vidmmi_vm_pt_num_entries(adapter,level))
    {
        zx_error("invalidate vm range[%llx, %llx] in page directory level: %d, with pd_addr_shift: %d\n", saddr, eaddr, level, pd_addr_shift);
        result = E_INVALIDARG;
        goto exit;
    }

    if(!vidmmi_vm_pt_check_bitmap(vma_space, parent, from, to))
    {
        //set pt bitmap firstly.
        vm_trace("update new pte bit:[0x%x, 0x%x]\n", from, to);
        vidmmi_vm_pt_update_bitmap(vma_space, parent, from, to, 1);
        //TODO: need reset the bitmap if the alloc_pd of this pte index failed.
    }
    else
    {
        vm_trace("skip exist pte index range:[0x%x, 0x%x]\n", from, to);
        skip_cur_level = 1;
    }

    saddr = saddr & ((1 << pd_addr_shift) - 1);
    eaddr = eaddr & ((1 << pd_addr_shift) - 1);

    //make the level value goto next level
    level ++;

    //walk over the sub page directory space
    for(cur_pte_idx = from; cur_pte_idx <= to; cur_pte_idx ++)
    {
        vidmm_vm_pt_t *entry = &parent->entries[cur_pte_idx];

        vm_trace("level: %d, entry[%x]->buffer: %p, update_bitmap: %p\n", level, cur_pte_idx, entry->buffer, entry->update_bitmap);

        if(NULL == entry->buffer)
        {
            unsigned int size = vidmmi_vm_pt_buffer_size(adapter, level);

            entry->age = adapter->resume_age;
            if (adapter->ctl_flags.page_table_local_enable)
            {
                entry->buffer = vidmm_create_segment_memory(adapter, local_seg_id, size, 1, 1);
                if (!entry->buffer)
                {
                    zx_warning("allocate level %d page table buffer from local failed, siz %d Byte\n", level, size);
                }
            }

            if (!entry->buffer && adapter->ctl_flags.page_table_pcie_enable)
            {
                entry->buffer = vidmm_create_segment_memory(adapter, unsnoop_seg_id, size, 1, 1);
            }

            if(NULL == entry->buffer)
            {
                zx_error("allocate level %d page table buffer failed, with size %d Byte\n", level, size);
                result = E_OUTOFMEMORY;
                goto entry_end;
            }
            if (entry->buffer->fb_pages_mem)
            {
                entry->buffer->fb_pages_mem->range_node_list[0]->tag = 0x5;

                if (level == (vidmmi_vm_pt_level_num(adapter) - 1))
                {
                    // mark it as a level2 ptes, hang dump will used this info
                    entry->buffer->fb_pages_mem->range_node_list[0]->tag = 0x7;
                }
            }
            vm_trace("finish create entry[0x%x] buffer: %p. size: %d\n", cur_pte_idx, entry->buffer, entry->buffer->size);

            result = vidmm_map_segment_memory(adapter, entry->buffer);
            if(result != S_OK)
            {
                zx_error("map segment memory failed, with err:0x%x\n", result);
                if(entry->buffer->gpu_vm != NULL)
                {
                    vidmm_unmap_segment_memory(entry->buffer);
                }
                vidmm_destroy_segment_memory(adapter, entry->buffer);
                goto entry_end;
            }

            {
                // zero the segment memory
                void *virt = vidmm_lock_segment_memory(adapter, entry->buffer);
                if (virt)
                {
                    zx_memset(virt, 0, size);

                    vidmm_unlock_segment_memory(adapter, entry->buffer);
                }
            }

            if(vma_space->flag.cpu_update_pte)
            {
                if(vidmm_lock_segment_memory(adapter, entry->buffer) == NULL)
                {
                    zx_error("lock segment memory failed.\n");
                    if(entry->buffer->gpu_vm != NULL)
                    {
                        vidmm_unmap_segment_memory(entry->buffer);
                    }
                    vidmm_destroy_segment_memory(adapter, entry->buffer);

                    result = E_FAIL;
                    goto entry_end;
                }
            }

            if(NULL == entry->update_bitmap)
            {
                entry->update_bitmap = zx_calloc(vidmmi_vm_pt_num_entries(adapter, level) >> 3);
                if(NULL == entry->update_bitmap)
                {
                    zx_error("allocate level %d page table update_bitmap failed\n", level);

                    vidmm_destroy_segment_memory(adapter, entry->buffer);

                    result = E_OUTOFMEMORY;
                    goto entry_end;
                }
            }

            entry->parent     = parent;
        }

        if (update_private_vma)
        {
            adapter->mm_mgr->chip_func->update_private_vma_space(adapter, parent_buffer_cpu_va, entry, cur_pte_idx, level);
        }

entry_end:
        if(result != S_OK)
            goto exit;

        if(skip_cur_level) goto next_level;


next_level:
        if(level < (vidmmi_vm_pt_level_num(adapter) - 1))//level < 2
        {
            //one buffer may cover 2(or more, not likely) ttbrs.
            unsigned long long sub_saddr = (cur_pte_idx == from) ? saddr : 0;
            unsigned long long sub_eaddr = (cur_pte_idx == to) ? eaddr : ((1 << pd_addr_shift) - 1);

            result = vidmm_vm_alloc_pd(vma_space, entry, sub_saddr, sub_eaddr, level, update_private_vma);
            if(result != S_OK)
            {
                zx_error("allocate level %d page directory failed, err code: %x\n", level, result);
                return result;
            }
        }
    }

exit:
    if (update_private_vma)
        vidmm_unlock_segment_memory(adapter, parent->buffer);

    return result;
}

/*
* vidmmi_vm_update_level: update one specific page directory level with DMA mode.
*
* note: the pd->entry has independent buffer, so update pd only one pe per one time.
*       we DO NOT release the page directory buffer for a vma address in one vma space, except
        that the vma space has been released.
*/
int vidmmi_vm_update_level(zx_vma_space_t *vma_space, unsigned long long start_va, vidmm_vm_pt_t *parent, unsigned int level, task_vm_update_t *vm_task, pfn_update_pte func, int *root_pd_dirty, int engine_index)
{
    int result = S_OK;
    unsigned long long pd_addr  = 0;
    unsigned int       pd_idx;
    unsigned char      *dma_buf;
    adapter_t *adapter = vma_space->adapter;

    vidmm_vm_update_ptes_arg_t update_pte_arg = {0,};

    vidmm_mgr_t  *mm_mgr    = adapter->mm_mgr;
    unsigned int entry_num  = vidmmi_vm_pt_num_entries(adapter, level);
    unsigned int page_idx   = 0;

    if(NULL == parent->entries)
    {
        //level 3 no need entries for the last page.
        return result;
    }

    //get the parent page directory address
    pd_addr = vidmm_vm_get_pde_addr(vma_space, parent->buffer, page_idx);

    pd_idx = zx_find_first_bit(parent->update_bitmap, entry_num);

    vm_trace("%s. level:%d, pd_idx: 0x%x, entry_num: %d\n", __func__, level, pd_idx, entry_num);

    //walk over the update pd entry from the update_bitmap
    while(pd_idx < entry_num)
    {
        unsigned long long pde_offset = 0, pt = 0;
        unsigned long long gpu_va = 0ULL;

        vidmm_vm_pt_t *entry = &parent->entries[pd_idx];

        if(NULL == entry->buffer)
        {
            goto find_next;
        }

        gpu_va = start_va + ((unsigned long long)pd_idx << (vidmmi_vm_accum_pd_bits_count(adapter, level) + GPU_PAGE_SHIFT));

        pt = vidmm_vm_get_pde_addr(vma_space, entry->buffer, page_idx);

        if(entry->addr == pt && entry->age >= adapter->resume_age)
        {
            goto next_level;
        }

        pde_offset = pd_idx;//get the pde address

        dma_buf = vidsch_vm_update_task_request_space(vm_task, 4096 * 2);

        update_pte_arg.buf_start_va    = gpu_va;
        update_pte_arg.dst_pte_page_va = pd_addr;
        update_pte_arg.npte            = 1;
        update_pte_arg.dma_buffer      = dma_buf;
        update_pte_arg.entity          = entry->buffer;
        update_pte_arg.pte_offset      = pde_offset;
        update_pte_arg.entity_type     = ZX_VM_NODE_TYPE_SEGMENET_MEMORY;
        update_pte_arg.pt_level        = level;

        func(vma_space, engine_index, &update_pte_arg);

        vidsch_vm_update_task_release_space(vm_task, update_pte_arg.dma_buffer);

        if ( level == ZX_PT_LEVEL_ROOT && !(*root_pd_dirty) )
        {
            *root_pd_dirty = TRUE;
        }

        entry->addr = pt;
        entry->age = adapter->resume_age;
next_level:
        //recursive to next level's pd.
        result = vidmmi_vm_update_level(vma_space, gpu_va, entry, level + 1, vm_task, adapter->mm_mgr->chip_func->update_ptes, root_pd_dirty, engine_index);
        if(result != S_OK)
        {
            zx_error("vm update pd level: %d, failed. err code:%x\n", level, result);
            return result;
        }
find_next:
        pd_idx = zx_find_next_bit(parent->update_bitmap, entry_num, pd_idx + 1);//pd_idx+1 to exclude the previously one.
    }

    vm_trace("%s. finish. level: %d, result: 0x%x\n", __func__, level, result);
    return result;
}

/*
* vidmm_vm_update_pd: update the page directories with given va range. make sure all of the
                      directories are valid for the ptes.
* @ context: the context of the update pd operation with update_vm task. the context can be NULL for
*                 the internal update operation called by the kmd.
* @ vma_space: the vma space of this va range.
*/


int vidmm_vm_update_pd(zx_vma_space_t *vma_space, int engine_index, task_vm_update_t *update_vm_task, int *root_pd_dirty)
{
    adapter_t        *adapter        = vma_space->adapter;
    zx_vm_pt_info_t *pt_info        = &vma_space->pt_info;
    vidmm_vm_pt_t    *root           = &pt_info->root;
    vidmm_mgr_t      *mm_mgr         = adapter->mm_mgr;
    vidmm_vm_pt_t    *parent         = NULL;
    unsigned int      level          = 0;

    unsigned int                     level_num          = vidmmi_vm_pt_level_num(adapter);
    unsigned char                   *dma_buf            = NULL;
    int                              result             = S_OK;
    unsigned long long               tlb_addr           = 0;
    unsigned long long               tlb_mask           = 0;

    //2, update page directory per level
    parent = root;//start pd level
    level  = ZX_PT_LEVEL_ROOT;

    result = vidmmi_vm_update_level(vma_space, 0ULL, parent, level, update_vm_task, adapter->mm_mgr->chip_func->update_ptes, root_pd_dirty, engine_index);
    if(result != S_OK)
    {
        zx_error("update pd level failed, err code: %x\n", result);
        goto done;
    }

done:
    return result;
}

/*
* vidmm_vm_update_ptes: update the page table entry.
*
* @ context: the context of the update pd operation with update_vm task. the context can be NULL for
*                 the internal update operation called by the kmd.
* @ vma_space: the vma space of this va range.
* @ start_addr: start va address, the value already be shifted out by the PAGE shift bits.
* @ end_addr: end va address, the value already be shifted out by the PAGE shift bits.
* @ fence_value: the out fence value of the update vm operation.
*/

int vidmm_vm_update_ptes(zx_vma_space_t *vma_space, int engine_index, task_vm_update_t *update_vm_task, unsigned long long entity_offset, unsigned long long start_addr, unsigned long long end_addr, void *entity, ZX_VM_NODE_TYPE entity_type, pfn_update_pte func)
{
    adapter_t               *adapter       = vma_space->adapter;
    zx_vm_pt_info_t         *pt_info       = &vma_space->pt_info;
    vidmm_vm_pt_t           *root          = &pt_info->root;
    vidmm_mgr_t             *mm_mgr        = adapter->mm_mgr;

    unsigned int             pt_level  = vidmmi_vm_pt_level_num(adapter) - 1;
    unsigned int             pt_entry_num_parent  = vidmmi_vm_pt_num_entries(adapter, pt_level -1);
    unsigned long long       addr_mask_parent     = pt_entry_num_parent - 1;

    vidmm_vm_pt_t           *pt_entry     = NULL;
    vidmm_segment_memory_t  *pt           = NULL;
    unsigned int             pt_index     = 0;

    void                             *dma_buf           = NULL;
    vidmm_vm_update_ptes_arg_t       update_pte_arg     = {0,};

    unsigned long long pte_dst_va     = 0;
    unsigned int       command_length = 0;
    unsigned long long vm_va = start_addr;

    int result = S_OK;

    vm_trace("%s, Org va range[%llx,%llx]\n", __func__, start_addr, end_addr);

    while(start_addr < end_addr)
    {
        unsigned long long       s_addr   = (start_addr >> GPU_PAGE_SHIFT);
        unsigned long long       e_addr   = (  end_addr >> GPU_PAGE_SHIFT);
        unsigned int expect_pt_entry_num  = vidmmi_vm_pt_num_entries(adapter, pt_level);
        unsigned int expect_addr_mask     = expect_pt_entry_num - 1;
        unsigned int expect_pte_offset    = s_addr & expect_addr_mask;
        unsigned int expect_nptes         = 0;

        //intra pt case
        if((s_addr & ~expect_addr_mask) == (e_addr & ~expect_addr_mask))
        {
            expect_nptes = e_addr - s_addr + 1;
        }
        else//inter pt case
        {
            expect_nptes = expect_pt_entry_num - (s_addr & expect_addr_mask);
        }
        vm_trace("%s, Partial shift va:[%llx-%llx], expect_nptes:%d\n", __func__, s_addr, e_addr, expect_nptes);

        pt_entry = vidmmi_vm_get_pt_entry(vma_space, root, s_addr, &pt_index);

        if(pt_entry && pt_entry->buffer)
        {
            vidmm_vm_pt_t       *parent                 = pt_entry->parent;
            unsigned int         real_pt_entry_num      = vidmmi_vm_pt_num_entries(adapter, pt_level);
            unsigned int         first_set              = (zx_find_first_bit(pt_entry->update_bitmap, real_pt_entry_num) >= real_pt_entry_num);
            unsigned long long   real_addr_mask         = 0;
            unsigned long long   real_pte_offset        = 0;
            unsigned int         real_nptes             = 0;
            unsigned long long   tmp_saddr              = s_addr, tmp_eaddr = e_addr;

            pte_dst_va  = vidmm_vm_get_pde_addr(vma_space, pt_entry->buffer, 0);

            real_pt_entry_num = vidmmi_vm_pt_num_entries(adapter, pt_level);
            real_addr_mask = real_pt_entry_num -1;
            real_pte_offset = (tmp_saddr & real_addr_mask);

            //intra pt case
            if((tmp_saddr & ~real_addr_mask) == (tmp_eaddr & ~real_addr_mask))
            {
                real_nptes = tmp_eaddr - tmp_saddr + 1;
            }
            else//inter pt case
            {
                real_nptes = real_pt_entry_num - (tmp_saddr & real_addr_mask);
            }

            dma_buf = vidsch_vm_update_task_request_space(update_vm_task, 4096 * 2);

            update_pte_arg.npte          = real_nptes;
            update_pte_arg.dma_buffer    = dma_buf;
            update_pte_arg.entity        = entity;
            update_pte_arg.entity_type   = entity_type;
            update_pte_arg.pte_offset    = real_pte_offset;
            update_pte_arg.pt_level      = pt_level;
            update_pte_arg.dst_pte_page_va    = pte_dst_va;
            update_pte_arg.cur_buf_va_on_fill = (tmp_saddr << 12);
            update_pte_arg.buf_start_va= vm_va;
            update_pte_arg.adapter= adapter;
            update_pte_arg.entity_offset = entity_offset;

            func(vma_space, engine_index, &update_pte_arg);

            vidsch_vm_update_task_release_space(update_vm_task, update_pte_arg.dma_buffer);
            //update update_bitmap
            vidmmi_vm_pt_update_bitmap(vma_space, pt_entry, update_pte_arg.pte_offset, update_pte_arg.pte_offset + update_pte_arg.npte - 1, 1);
        }
        else
        {
            zx_error("get the page table failed, at shifted start va: %llx\n", s_addr);
            result = E_FAIL;
        }

        if(result != S_OK) goto exit;

        s_addr     += expect_nptes;
        start_addr  = (s_addr << GPU_PAGE_SHIFT);
    }
exit:
    return result;
}

/*
* vidmm_vm_update_mapping: update one vm node page table mapping, include update the pd and pt
*
* caller should hold allocation->va_lock;
* @vma_space: the vma space of the residency operation
* @context: the paging context to do the residency operation.
* @vm_node: the mapping vm node for va.
* @allocation: the mapping allocation for pa.
* @paging_fence_value: the returned paging fence value for paging context.
*
*/
int vidmm_vm_update_mapping(zx_vma_space_t *vma_space, gpu_context_t *context, zx_vm_node_t *vm_node, vidmm_allocation_t *allocation, void* update_task)
{
    unsigned long long start_addr  = 0;
    unsigned long long end_addr    = 0;
    unsigned long long node_offset = 0;
    unsigned long long node_size   = 0;

    vidmm_vm_pt_t *root     = &vma_space->pt_info.root;
    adapter_t     *adapter  = vma_space->adapter;

    task_vm_update_t *update_vm_task = (task_vm_update_t *) update_task;
    int              engine_index    = context->rb_index;

    int            result   = S_OK;
    int            root_pd_dirty = 0;

    zx_assert(!allocation->flag.global, "");

    vidmmi_vm_get_node_info(vm_node, &node_offset, &node_size);

    start_addr = node_offset;
    end_addr   = start_addr + node_size -1; //addr is 0 start

    //1, alloc page directory. this function also can be considered as validate pd if not access related pd entry firstly.
    zx_mutex_lock(vma_space->lock);

    if (adapter->chip_id == CHIP_CNE001)
        goto update_ptes;

    result = vidmm_vm_alloc_pd(vma_space, root, (start_addr >>GPU_PAGE_SHIFT), (end_addr >> GPU_PAGE_SHIFT), ZX_PT_LEVEL_ROOT, 0);
    if(result != S_OK)
    {
        goto exit;
    }

    //2, update page directory and page tables.
    result = vidmm_vm_update_pd(vma_space, engine_index, update_vm_task, &root_pd_dirty);
    if(result != S_OK)
    {
        goto exit;
    }

update_ptes:
    result = vidmm_vm_update_ptes(vma_space, engine_index, update_vm_task, vm_node->offset, start_addr, end_addr, allocation, ZX_VM_NODE_TYPE_ALLOCATION, adapter->mm_mgr->chip_func->update_ptes);
    if(result != S_OK)
    {
        goto exit;
    }

exit:
    if (root_pd_dirty)
    {
        vma_space->root_pd_age++;
    }

    zx_mutex_unlock(vma_space->lock);

    return result;
}

void vidmm_vm_clear_engine_dirty_range(zx_vma_space_t *vma_space, int engine_index)
{
    zx_mutex_lock(vma_space->lock);

    if (vma_space->utlb_dirtys[engine_index].dirty)
    {
        vma_space->utlb_dirtys[engine_index].dirty      = 0;
        vma_space->utlb_dirtys[engine_index].dirty_mask = 0;
        vma_space->utlb_dirtys[engine_index].dirty_addr = 0;
    }

    if (vma_space->dtlb_dirtys[engine_index].dirty)
    {
        vma_space->dtlb_dirtys[engine_index].dirty      = 0;
        vma_space->dtlb_dirtys[engine_index].dirty_mask = 0;
        vma_space->dtlb_dirtys[engine_index].dirty_addr = 0;
    }

    zx_mutex_unlock(vma_space->lock);
}

int vidmm_vm_map_gpu_virtual_address(gpu_device_t *device, vidmm_allocation_t *allocation, vidmm_map_desc_args_t *arg)
{
    int result = 0;
    adapter_t       *adapter       = device->adapter;
    zx_vma_space_t  *vma_space     = allocation->flag.global ? adapter->reserved_vma : device->vm;
    vidmm_segment_t *segment       = allocation->adapter->mm_mgr->segment + allocation->segment_id;
    zx_vm_node_t    *new_node = NULL;
    vidmm_vm_create_desc_args_t create_desc_arg = {0};

    if (vma_space == adapter->reserved_vma)
    {
        zx_assert(!arg->is_sparse, "share space not support sparse");

        if (segment->flags.invalid)
        {
            zx_mutex_lock(allocation->paging_lock);
            result = vidmm_make_resident_locked(allocation);
            zx_mutex_unlock(allocation->paging_lock);

            if (result != 0)
                goto done;

            segment = allocation->adapter->mm_mgr->segment + allocation->segment_id;
        }

        if (allocation->pages_mem)
            goto alloc_vm_node;

        zx_assert(allocation->fb_pages_mem != NULL, "");
        if (allocation->fb_pages_mem->range_node_num > 1)
            goto alloc_vm_node;

        arg->out_va = adapter->shared_gpu_va_offset + allocation->fb_pages_mem->range_node_list[0]->start + arg->offset;
    }
    else
    {
alloc_vm_node:
        create_desc_arg.prefer_start_addr = arg->base_addr;
        create_desc_arg.force_start       = arg->force_base;
        create_desc_arg.size              = arg->size;
        create_desc_arg.offset            = arg->offset;

        if(arg->is_sparse)
            new_node = vidmm_vm_allocate_sparse_vm_node(vma_space, allocation, arg->base_addr, arg->size, arg->offset);
        else
            new_node = vidmm_vm_allocate_allocation(vma_space, device, allocation, &create_desc_arg);

        if (!new_node)
        {
            result = E_FAIL;
            goto done;
        }

        new_node->device = device;
        vidmmi_vm_get_node_info(new_node, &arg->out_va, NULL);
    }
done:
    return result;
}

void vidmm_vm_unmap_gpu_virtual_address(zx_vma_space_t *vma_space, unsigned long long base, unsigned long long size)
{
    int i;
    zx_vm_zone_t *zone  = NULL;
    struct range_node *range = NULL;

    zone = vidmm_vm_get_zone_from_range(vma_space, base, size);
    if (!zone)
    {
        zx_error("can't find any zone contain the range [%llx,%llx]\n", base, size);
        return;
    }

    if (zone->zone_id == VM_ZONE_ID_SHARE_SPACE_LOCAL)
    {
        return;
    }

    zx_mutex_lock(zone->lock);

    range = zone->ra->lookup(zone->ra, base, size);
    if (!range)
    {
        zx_info("no such range [%llx,%llx]...\n", base, size);
        goto done;
    }
    if ((long)range->private & 1)
    {
        // it is an sparse node
        zx_sparse_node_t *sparse_node = (void*)((long)range->private & ~0x1);

        // free all the vm node attached to this sparse node
        sparse_node->ra->for_each_node(sparse_node->ra,
                base, base + size, NULL, vidmmi_vm_release_vm_node);

        // just unmap it, not free it
    }
    else
    {
        // it is an vm node
        // the behavior the same as free_gpu_virtual_ddress
        vidmmi_vm_release_vm_node(range, NULL);
    }

done:
    zx_mutex_unlock(zone->lock);
}

void vidmm_vm_free_gpu_virtual_address(zx_vma_space_t *vma_space, unsigned long long base, unsigned long long size)
{
    int i;
    zx_vm_zone_t *zone  = NULL;
    struct range_node *range = NULL;

    zone = vidmm_vm_get_zone_from_range(vma_space, base, size);
    if (!zone)
    {
        zx_error("can't find any zone contain the range [%llx,%llx]\n", base, size);
        return;
    }

    if (zone->zone_id == VM_ZONE_ID_SHARE_SPACE_LOCAL)
    {
        return;
    }

    zx_mutex_lock(zone->lock);

    range = zone->ra->lookup(zone->ra, base, size);
    if (!range)
    {
        zx_info("no such range [%llx,%llx]...\n", base, size);
        goto done;
    }
    if ((long)range->private & 1)
    {
        // it is an sparse node
        zx_sparse_node_t *sparse_node = (void*)((long)range->private & ~0x1);

        if (base != sparse_node->range->start && size != sparse_node->range->size)
        {
            zx_warning("free_vm_space with unmatched va range, request[%llx,%llx], sparse[%llx,%llx]!!!!!!\n",
                    base, base + size,
                    sparse_node->range->start, sparse_node->range->start + sparse_node->range->size
                    );
            zx_warning("will free all vm_node related with this sparse!!!!!!.\n");
        }

        // free all the vm node attached to this sparse node
        sparse_node->ra->for_each_node(sparse_node->ra,
                sparse_node->range->start,
                sparse_node->range->start + sparse_node->range->size,
                NULL,
                vidmmi_vm_release_vm_node);

        // release the sparse node itself
        vidmmi_vm_release_sparse_node(sparse_node, NULL);
    }
    else
    {
        // it is an vm node
        vidmmi_vm_release_vm_node(range, NULL);
    }

done:
    zx_mutex_unlock(zone->lock);
}

int vidmm_vm_reserve_gpu_virtual_address(gpu_device_t *device, vidmm_vm_reserve_gpu_va_arg_t *reserve, unsigned long long *gpu_va)
{
    adapter_t *adapter = device->adapter;
    zx_sparse_node_t *sparse_node = NULL;
    zx_vm_zone_t *zone = NULL;
    unsigned int result = S_OK;

    if (device == adapter->reserved_device)
    {
        zx_assert(adapter->hw_caps.share_pcie_enable, "share pcie not enable");
        zone = ((zx_vma_space_t*)adapter->reserved_vma)->zones[VM_ZONE_ID_SHARE_SPACE_PCIE];
    }
    else
    {
        zone = device->vm->zones[VM_ZONE_ID_PRIVATE_SPACE];
    }

    sparse_node = zx_calloc(sizeof(*sparse_node));

    zx_mutex_lock(zone->lock);
    if (reserve->base != 0)
    {
        sparse_node->range = zone->ra->reserve(zone->ra, reserve->base, reserve->size);
    }
    else
    {
        sparse_node->range = zone->ra->alloc(zone->ra, reserve->size, reserve->alignment, reserve->min, reserve->max, 0);
    }

    if (sparse_node->range)
    {
        sparse_node->range->private = (void*)((unsigned long)sparse_node | 1);
        sparse_node->ra = zx_create_range_allocator(sparse_node->range->start, sparse_node->range->size, 4096);
    }
    else
    {
        zx_free(sparse_node);
        result = E_OUTOFMEMORY;
    }
    zx_mutex_unlock(zone->lock);

    if (result == S_OK)
        *gpu_va = sparse_node->range->start;

    return result;
}


int vidmm_vm_update_gpu_virtual_address(gpu_context_t *comp_context, vidmm_vm_update_gpu_va_t *update)
{
    gpu_device_t    *device     = comp_context->device;
    zx_vma_space_t  *vm         = device->vm;

    unsigned long long paging_fence_value = 0;//used for map operation
    unsigned int       num_ops            = update->num_operation;
    int                map_ops            = FALSE;
    int                result             = S_OK;
    int                i                  = 0;

    //loop the update operations
    for(i = 0; i < num_ops; i ++)
    {
        krnl_update_sparse_mapping_info_t *ops = &update->ops[i];
        unsigned int                      ops_type = ops->op_type;
        switch (ops_type)
        {
            case RESERVED_RESOURCE_MAP:
            {
                vidmm_allocation_t   *allocation  = NULL;
                vidmm_map_desc_args_t map_arg= {0,};

                map_ops = TRUE;

                allocation = ops->map.allocation;
                zx_assert(allocation != NULL, "");

                if (allocation->flag.global)
                {
                    result = E_INVALIDARG;
                    zx_error("can't map a shared allocation to sparse memory...\n");
                    goto exit;
                }
                map_arg.base_addr  = ops->map.tile_gpu_va;
                map_arg.size       = ops->map.tile_size;//use allocation_size?
                map_arg.offset     = ops->map.allocation_offset;
                map_arg.force_base = 1;
                map_arg.is_sparse  = 1;

                result = vidmm_vm_map_gpu_virtual_address(device, allocation, &map_arg);
                if(result != S_OK)
                {
                    goto exit;
                }
            }
            break;

            case RESERVED_RESOURCE_UNMAP:
            {
                vidmm_vm_unmap_gpu_virtual_address(vm, ops->unmap.tile_gpu_va, ops->unmap.tile_size);
            }
            break;

            case RESERVED_RESOURCE_COPY:
            {
                vm_trace("unsupport sparse resource copy operation...\n");
            }
            break;

            case RESERVED_RESOURCE_MAP_PROTECT:
            {
                vm_trace("not supprt sparse resource map_protect yet....\n");
            }
            break;

            default:
            {
                vm_trace("unsupport sparse resource update operation type: %d\n", ops_type);
            }
            break;
        }
    }

exit:
    return result;

}

static int vidmm_vm_node_visitor(struct range_node *range, void *priv)
{
    struct os_printer *p = priv;

    if ((long)range->private & 1)
    {
        // it's sparse node
        zx_sparse_node_t *sparse_node = (void*)((long)range->private & ~0x1);
        sparse_node->ra->dump(p, sparse_node->ra, "  ");
    }

    return 1;
}

void vidmm_vm_dump_zone(struct os_printer *p, zx_vm_zone_t *zone)
{
    if (zone->gpu_vm_size > 0 && zone->ra)
    {
        zx_mutex_lock(zone->lock);
        zx_printf(p, "Zone[%d] heap status:\n", zone->zone_id);
        zone->ra->dump(p, zone->ra, "  ");

        zx_printf(p, "Zone[%d] sparse status:\n", zone->zone_id);
        zone->ra->for_each_node(zone->ra, 0ULL, -1ULL, p, vidmm_vm_node_visitor);
        zx_mutex_unlock(zone->lock);
    }
}
void vidmm_vm_dump_vm_node_info(struct os_printer *p, vidmm_allocation_t *allocation)
{
    gpu_device_t       *device   = NULL;
    zx_vm_node_t       *vm_node  = NULL;
    unsigned long long gpu_va, size;

    zx_mutex_lock(allocation->va_lock);

    list_for_each_entry(vm_node, &allocation->gpu_vm_list, mapped_list_item)
    {
        zx_printf(p, "=====vm node info=====\n");
        zx_printf(p, "vm_node = 0x%x\n", vm_node);

        device = vm_node->device;

        zx_printf(p, "device_pname = %s\n", device->pname);
        zx_printf(p, "device_pid = %lu\n", device->pid);

        vidmmi_vm_get_node_info(vm_node, &gpu_va, &size);

        zx_printf(p, "gpu_va = 0x%llx\n", gpu_va);
        zx_printf(p, "size = 0x%llx\n", size);
        zx_printf(p, "======================\n");
    }

    zx_mutex_unlock(allocation->va_lock);
}

static void vidmm_vm_dump_pt(struct os_printer *p, adapter_t *adapter, vidmm_vm_pt_t *entry, int level, int index)
{
    int i, j, already_has_locked;
    unsigned int pt_entry_num = vidmmi_vm_pt_num_entries(adapter, level);
    unsigned int large_page = 0;
    unsigned long *bitmap;
    unsigned int *virt;
    int num_entry;

    // dump this entry's info
    if (entry->buffer)
    {
        zx_printf(p, "%d:%d: entry:%p, parent:%p, addr:%llx, buffer:%p\n", level, index, entry, entry->parent, entry->addr, entry->buffer);

        already_has_locked = entry->buffer->krnl_cpu_vma ? 1 : 0;
        virt = (unsigned int*)vidmm_lock_segment_memory(adapter, entry->buffer);

        bitmap = entry->update_bitmap;
        num_entry = vidmmi_vm_pt_num_entries(adapter, level);
        j = zx_find_first_bit(bitmap, num_entry);

        while(j < num_entry)
        {
            adapter->mm_mgr->chip_func->dump_ptes(p, virt, large_page, level, j);

            j = zx_find_next_bit(bitmap, num_entry, j + 1);
        }

        if (!already_has_locked)
            vidmm_unlock_segment_memory(adapter, entry->buffer);
    }

    // dump sub entries if have
    if (entry->entries)
    {
        for (i = 0; i < pt_entry_num; i++)
        {
            vidmm_vm_dump_pt(p, adapter, entry->entries + i, level + 1, i);
        }
    }
}

void vidmm_vm_dump_vma_space(struct os_printer *p, adapter_t *adapter, zx_vma_space_t *vma_space)
{
    zx_vm_pt_info_t *pt_info = &vma_space->pt_info;
    vidmm_vm_pt_t *root = &pt_info->root;

    zx_mutex_lock(vma_space->lock);

    vidmm_vm_dump_pt(p, adapter, root, ZX_PT_LEVEL_ROOT, 0);

    zx_mutex_unlock(vma_space->lock);
}

extern unsigned long long vidsch_get_pte_value(adapter_t *adapter, int snoop, unsigned long long phy_addr);
void vidmm_vm_dump_address_info(adapter_t *adapter, zx_vma_space_t *vma_space, zx_vm_node_t *vm_node, unsigned long long start_addr, unsigned long long address)
{
    unsigned int           level           = ZX_PT_LEVEL_ROOT;
    unsigned int           idx             = 0;
    unsigned long long     addr_partion    = 0;
    vidmm_vm_pt_t          *entry          = &vma_space->pt_info.root;
    unsigned int           pte_segment, pte_snoop;
    unsigned long long     pte_pa;

    unsigned int           *virt;
    int       already_has_locked;

    if (vma_space != adapter->reserved_vma)
    {
        while(level < ZX_PT_LEVEL_MAX)
        {
            if (level > ZX_PT_LEVEL_ROOT)
            {
                entry = &entry->entries[idx];
            }

            if(!entry->buffer)
            {
                zx_info(" address:0x%llx in level:%d's entry buffer is null\n", address, level);

                return;
            }

            pte_segment   = (entry->buffer->flag.local) ? 1 : 0;
            pte_snoop     = entry->buffer->flag.snooping_enabled;
            if(pte_segment == 0)
            {
                pte_pa = vidsch_get_pte_value(adapter, pte_snoop, zx_get_page_phys_address(entry->buffer->pages_mem, 0, NULL));
            }
            else
            {
                pte_pa = entry->buffer->fb_pages_mem->range_node_list[0]->start;//per 4K one pte
            }

            zx_info("dump_pte: level = %d, idx = %d, pte_segment = %d, pte_snoop = %d, entry->buffer's pte_pa: 0x%llx\n", level, idx, pte_segment, pte_snoop, pte_pa >> 12);

            already_has_locked = entry->buffer->krnl_cpu_vma ? 1 : 0;
            virt = vidmm_lock_segment_memory(adapter, entry->buffer);

            addr_partion        = (address >> GPU_PAGE_SHIFT)& vidmmi_vm_pt_addr_mask(adapter, level);
            idx                 = addr_partion >> vidmmi_vm_accum_pd_bits_count(adapter, level);

            adapter->mm_mgr->chip_func->dump_ptes(NULL, virt, 0, level, idx);

            if (!already_has_locked)
                vidmm_unlock_segment_memory(adapter, entry->buffer);

            level ++;
        }
    }

    adapter->mm_mgr->chip_func->dump_mapped_entity(adapter, vm_node->mapped_entity, vm_node->mapped_entity_type, address, start_addr);
}
