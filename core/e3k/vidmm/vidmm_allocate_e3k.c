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

#define add_prefer_segment(segment_id) do { \
        int segment_bit = 1 << (segment_id);                                            \
        if(!(segment_mask & segment_bit))                                         \
        {                                                                         \
            segments[num++] = (segment_id);                                         \
            segment_mask   |= segment_bit;                                        \
        }                                                                         \
    }while(0)

static void vidmm_set_segment_id_e3k(adapter_t *adapter, vidmm_allocation_t *allocation)
{
    vidmm_mgr_t *mm_mgr = adapter->mm_mgr;
    int segments[5]     = {0, };
    int segment_mask    = 0;
    int num             = 0;
    int i;
    BOOL bSnoopOnly = adapter->hw_caps.snoop_only ? TRUE : FALSE;

    if (adapter->hw_caps.local_only)
    {
        allocation->flag.pool_type = ZX_POOL_LOCALVIDMEM;
    }

    switch(allocation->flag.pool_type)
    {
    case ZX_POOL_LOCALVIDMEM:
        add_prefer_segment(SEGMENT_ID_LOCAL_ELT3K);
        break;
    case ZX_POOL_NONLOCALVIDMEM:
        switch (allocation->flag.cache_type)
        {
        case ZX_CACHE_CACHABLE:
            add_prefer_segment(SEGMENT_ID_PCIE_SNOOPABLE_ELT3K);
            break;
        case ZX_CACHE_UNCACHABLE:
            if (bSnoopOnly)
                add_prefer_segment(SEGMENT_ID_PCIE_SNOOPABLE_ELT3K);
            else
                add_prefer_segment(SEGMENT_ID_PCIE_UNSNOOPABLE_ELT3K);
            break;
        default:
            if (!bSnoopOnly)
                add_prefer_segment(SEGMENT_ID_PCIE_UNSNOOPABLE_ELT3K);
            add_prefer_segment(SEGMENT_ID_PCIE_SNOOPABLE_ELT3K);
            break;
        }
        break;
    case ZX_POOL_UNKNOWN:
    case ZX_POOL_VIDEOMEMORY:
        switch (allocation->flag.cache_type)
        {
        case ZX_CACHE_CACHABLE:
            add_prefer_segment(SEGMENT_ID_PCIE_SNOOPABLE_ELT3K);
            break;
        case ZX_CACHE_UNCACHABLE:
            add_prefer_segment(SEGMENT_ID_LOCAL_ELT3K);
            if (!bSnoopOnly)
                add_prefer_segment(SEGMENT_ID_PCIE_UNSNOOPABLE_ELT3K);
            break;
        default:
            add_prefer_segment(SEGMENT_ID_LOCAL_ELT3K);
            if (!bSnoopOnly)
                add_prefer_segment(SEGMENT_ID_PCIE_UNSNOOPABLE_ELT3K);
            else
                add_prefer_segment(SEGMENT_ID_PCIE_SNOOPABLE_ELT3K);
            break;
        }
        break;
    }

    for (i = 0; i < ARRAY_SIZE(allocation->preferred_segment); i++)
    {
        allocation->preferred_segment[i].value = segments[i];
    }
}

int vidmm_describe_allocation_e3k(adapter_t *adapter, vidmm_describe_allocation_t *desc_info)
{
    vidmm_allocation_t *allocation = desc_info->allocation;

    vidmm_set_segment_id_e3k(adapter, allocation);

    if(allocation->compress_format)
    {
        allocation->slot_index = vidmm_alloc_bl_slot_e3k(adapter, allocation);
    }

    return S_OK;
}

