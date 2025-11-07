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
#include "vidmm.h"
#include "vidmmi.h"
#include "vidsch.h"
#include "vidschi.h"
#include "chip_include_e3k.h"
#include "ring_buffer.h"
#include "vidsch_engine_e3k.h"
#include "mm_e3k.h"
#include "context.h"
#include "global.h"
#include "chip_include_e3k.h"

extern void vidmmi_test_video_memory(adapter_t *adapter);
extern gpu_device_t* cm_create_device(adapter_t *adapter, void *filp);
gpu_context_t* cm_create_context(gpu_device_t *device, create_context_t *create_context);
extern int vidmmi_fast_clear_e3k(adapter_t *adapter, vidmm_private_build_paging_buffer_arg_t *data);
extern int vidmm_lock_allocation(gpu_device_t *device, vidmm_allocation_t *allocation);
extern int vidmm_unlock_allocation(vidmm_allocation_t *allocation);
extern int vidmm_fill_allocation(gpu_context_t *context, vidmm_allocation_t *allocation, unsigned int fill_pattern, unsigned int auto_clear);

static struct os_pages_memory* vidsch_test_alloc_pages(void *parent, void *pdev, unsigned long long size, alloc_pages_flags_t *flags)
{
    return zx_allocate_pages_memory(pdev, size, 4096, *flags);
}

static void vidsch_test_release_pages(void *pdev, struct os_pages_memory *pages_mem)
{
    zx_free_pages_memory(pdev, pages_mem);
}

static vidmm_allocation_t *vidsch_test_create_allocation(adapter_t *adapter, unsigned int size, int segment_id, int share)
{
    vidmm_mgr_t *mm_mgr  = adapter->mm_mgr;
    vidmm_segment_t *segment = mm_mgr->segment + segment_id;
    vidmm_allocation_t *allocation = NULL;
    krnl_create_allocation_info_t create = {0, };
    zx_create_allocation_info_t info = {0, };

    if (segment->flags.invalid)
        return NULL;

    create.info = &info;
    info.flags.share = share;
    info.flags.cpu_visible = 1;
    info.flags.unpagable = 1;
    info.flags.cache_type = segment->property & ZX_SEGMENT_PROPERTY_SNOOPABLE ? ZX_CACHE_CACHABLE : ZX_CACHE_UNCACHABLE;
    info.flags.pool_type = segment->property & ZX_SEGMENT_PROPERTY_LOCAL ? ZX_POOL_LOCALVIDMEM : ZX_POOL_NONLOCALVIDMEM;
    info.flags.force_clear = 0;
    info.flags.continuous = 0;
    info.size = size;
    info.alignment = 8192;
    create.get_system_pages = vidsch_test_alloc_pages;
    create.release_system_pages = vidsch_test_release_pages;

    return vidmm_create_allocation(adapter, &create);
}

static void vidsch_test_fill_allocation(adapter_t *adapter, gpu_context_t *context, int segment_id)
{
#define FILL_PATTERN1   0xaa55aa55
#define FILL_PATTERN2   0x12345678
    vidmm_allocation_t *allocation = NULL;

    zx_info("^^^test_fill_allocation: seg_id=%d, share=%d\n", segment_id, context == adapter->reserved_context);

    allocation = vidsch_test_create_allocation(adapter, 4096*4, segment_id, context == adapter->reserved_context);
    if (!allocation)
    {
        zx_info("create allocation failed.\n");
        return;
    }
    vidmm_lock_allocation(context->device, allocation);

    vidmm_fill_allocation(context, allocation, FILL_PATTERN1, 0);
    if (*(unsigned int*)allocation->krnl_cpu_vma->virt_addr != FILL_PATTERN1)
    {
        zx_info("fill pattern1 failed, expected:%x, actually:%x\n",
                FILL_PATTERN1, *(unsigned int*)allocation->krnl_cpu_vma->virt_addr);
    }

    vidmm_fill_allocation(context, allocation, FILL_PATTERN2, 0);
    if (*(unsigned int*)allocation->krnl_cpu_vma->virt_addr != FILL_PATTERN2)
    {
        zx_info("fill pattern2 failed, expected:%x, actually:%x\n",
                FILL_PATTERN2, *(unsigned int*)allocation->krnl_cpu_vma->virt_addr);
    }

    vidmm_destroy_allocation(allocation, NULL);
    zx_info("^^^ %s done\n", __func__);
}


void vidsch_test(void *data)
{
    adapter_t *adapter = data;
    gpu_device_t *device = NULL;
    gpu_context_t *context = NULL;
    create_context_t create = {0, };

    device = cm_create_device(adapter, (void*)1);
    if (!device)
    {
        zx_info("create priv device failed.\n");
    }
    create.node_ordinal = RESERVED_ENGINE_INDEX;
    create.priority = 0;
    create.client_type = CLIENT_TYPE_OPENGL;
    create.type = CONTEXT_NORMAL_RENDER;
    context = cm_create_context(device, &create);
    if (!context)
    {
        zx_info("create priv context failed.\n");
    }

    // test share space
    vidsch_test_fill_allocation(adapter, adapter->reserved_context, SEGMENT_ID_LOCAL_ELT3K);
    vidsch_test_fill_allocation(adapter, adapter->reserved_context, SEGMENT_ID_PCIE_UNSNOOPABLE_ELT3K);
    vidsch_test_fill_allocation(adapter, adapter->reserved_context, SEGMENT_ID_PCIE_SNOOPABLE_ELT3K);

    // test priv space
    if (context)
    {
        vidsch_test_fill_allocation(adapter, context, SEGMENT_ID_LOCAL_ELT3K);
        vidsch_test_fill_allocation(adapter, context, SEGMENT_ID_PCIE_UNSNOOPABLE_ELT3K);
        vidsch_test_fill_allocation(adapter, context, SEGMENT_ID_PCIE_SNOOPABLE_ELT3K);
    }
}
