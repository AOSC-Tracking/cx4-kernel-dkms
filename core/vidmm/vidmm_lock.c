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


int vidmm_get_map_allocation_info(adapter_t *adapter, vidmm_allocation_t *allocation, zx_map_argu_t *argu)
{
    vidmm_mgr_t     *mm_mgr  = adapter->mm_mgr;
    vidmm_segment_t *segment = &mm_mgr->segment[allocation->segment_id];
    struct range_node *range_node = NULL;
    int i = 0, j = 0;

    if (segment->flags.invalid)
    {
        zx_warning("allocation 0x%x not resident!\n", allocation->debug_gid);
        return -1;
    }

    argu->size = allocation->size;
    if (!segment->flags.local)
    {
        argu->memory = allocation->pages_mem;
        argu->flags.mem_type = ZX_SYSTEM_RAM;
        argu->offset = 0;

        if (segment->flags.support_snoop)
        {
            argu->flags.cache_type = ZX_MEM_WRITE_BACK;
        }
        else
        {
            argu->flags.cache_type = ZX_MEM_WRITE_COMBINED;
        }
    }
    else
    {
        argu->flags.cache_type = ZX_MEM_WRITE_COMBINED;

        argu->flags.mem_type   = ZX_SYSTEM_IO;

        if(!argu->node_num)
            argu->node_num = allocation->fb_pages_mem->range_node_num;
        else{
            if(argu->node_num == 1){
                argu->phys_addr = adapter->vidmm_bus_addr + allocation->fb_pages_mem->range_node_list[0]->start;
                if(argu->phys_addr >= (adapter->vidmm_bus_addr + adapter->visible_local_memory_size))
                    zx_assert(0, "vidmm_get_map_allocation_info map invisable allocation!!!");
            }else{
                for(j=0; j<argu->node_num; j++){
                    range_node = allocation->fb_pages_mem->range_node_list[j];
                    argu->phys_addrs[i] = adapter->vidmm_bus_addr + range_node->start;
                    if(argu->phys_addrs[i] >= (adapter->vidmm_bus_addr + adapter->visible_local_memory_size))
                        zx_assert(0, "vidmm_get_map_allocation_info map invisable allocation!!!");
                    argu->node_sizes[i++] = range_node->size;
                }
                zx_assert(i==allocation->fb_pages_mem->range_node_num, "");
            }
        }
    }

    return 0;
}

int vidmm_unlock_allocation(vidmm_allocation_t *allocation)
{
    zx_cpu_vm_area_t *vma;

    zx_spin_lock(allocation->lock);
    vma = allocation->krnl_cpu_vma;
    if (vma && --vma->ref_cnt == 0)
    {
        allocation->krnl_cpu_vma = NULL;
    }
    else
    {
        vma = NULL;
    }
    zx_spin_unlock(allocation->lock);

    if (vma)
    {
        switch(vma->flags.mem_type)
        {
        case ZX_SYSTEM_IO:
            zx_unmap_io_memory(vma);
            break;
        case ZX_SYSTEM_RAM:
            zx_unmap_pages_memory(vma);
            break;
        }
    }

    vidmm_mark_pagable(allocation);

    return 0;
}

int vidmm_lock_allocation(gpu_device_t *device, vidmm_allocation_t *allocation)
{
    zx_cpu_vm_area_t *vma;
    adapter_t *adapter = device->adapter;
    int ret = S_OK;

    if ((ret = vidmm_prepare_and_mark_unpagable(allocation)) != S_OK)
    {
        goto exit;
    }
    zx_spin_lock(allocation->lock);
    vma = allocation->krnl_cpu_vma;
    if (vma)
        ++vma->ref_cnt;
    zx_spin_unlock(allocation->lock);

    if (!vma)
    {
        zx_map_argu_t argu = {0, };
        if ((ret = vidmm_get_map_allocation_info(adapter, allocation, &argu)) != S_OK)
        {
            goto exit;
        }
        if(argu.node_num > 1){
            argu.phys_addrs = zx_calloc(sizeof(unsigned long long) * argu.node_num);
            argu.node_sizes = zx_calloc(sizeof(unsigned long long) * argu.node_num);
        }
        if ((ret = vidmm_get_map_allocation_info(adapter, allocation, &argu)) != S_OK)
        {
            if(argu.phys_addrs){
                zx_free(argu.phys_addrs);
                zx_free(argu.node_sizes);
            }
            goto exit;
        }

        switch(argu.flags.mem_type)
        {
        case ZX_SYSTEM_RAM:
            vma = zx_map_pages_memory(&argu);
            break;
        case ZX_SYSTEM_IO:
            vma = zx_map_io_memory(&argu);
            break;
        }

        if(argu.phys_addrs){
            zx_free(argu.phys_addrs);
            zx_free(argu.node_sizes);
        }

        zx_spin_lock(allocation->lock);
        allocation->krnl_cpu_vma = vma;
        if (vma)
        {
            vma->ref_cnt = 1;
        }
        zx_spin_unlock(allocation->lock);
    }

exit:
    return ret;
}
