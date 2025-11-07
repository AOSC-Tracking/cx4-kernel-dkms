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
#include "mm_e3k.h"
#include "vidsch.h"
#include "vidschi.h"
#include "ring_buffer.h"
#include "vidsch_engine_e3k.h"
#include "vidsch_debug_hang_e3k.h"
#include "vidsch_dfs_e3k.h"

extern vidsch_chip_func_t engine_gfx_low_chip_func_e3k;
extern vidsch_chip_func_t engine_gfx_high_chip_func_e3k;
extern vidsch_chip_func_t engine_cs_high_chip_func_e3k;
extern vidsch_chip_func_t engine_cs_low0_chip_func_e3k;
extern vidsch_chip_func_t engine_cs_low1_chip_func_e3k;
extern vidsch_chip_func_t engine_cs_low2_chip_func_e3k;
extern vidsch_chip_func_t engine_cs_low3_chip_func_e3k;
extern vidsch_chip_func_t engine_vcp_low0_chip_func_e3k;
extern vidsch_chip_func_t engine_vcp_low1_chip_func_e3k;
extern vidsch_chip_func_t engine_vcp_low2_chip_func_e3k;
extern vidsch_chip_func_t engine_vpp_chip_func_e3k;
extern vidsch_chip_func_t engine_vpp0_chip_func_e3k;
extern vidsch_chip_func_t engine_vpp1_chip_func_e3k;

static int vidmmi_get_dummyfence_addr(void *arg, int pg_start, int pg_cnt, unsigned long long dma_addr)
{
    *(unsigned long long*)arg = dma_addr;

    return 1;
}

static int vidschi_init_engine_share_e3k(adapter_t *adapter)
{
    engine_share_e3k_t  *share = zx_calloc(sizeof(engine_share_e3k_t));

    share->lock = zx_create_spinlock();

    adapter->hang_dump_lock = zx_create_mutex();

    adapter->private_data = share;

    {
       alloc_pages_flags_t alloc_flags = {0};
       alloc_flags.dma32 = TRUE;
       alloc_flags.need_zero = TRUE;
       alloc_flags.fixed_page = TRUE;
       alloc_flags.need_dma_map = 1;
       share->dummyfence[0] = zx_allocate_pages_memory(adapter->os_device.pdev, adapter->os_page_size, adapter->os_page_size, alloc_flags);
       zx_pages_memory_for_each_continues(share->dummyfence[0], &share->dummyfence_phy[0], vidmmi_get_dummyfence_addr);

       alloc_flags.need_dma_map = 0;
       share->dummyfence[1] = zx_allocate_pages_memory(adapter->os_device.pdev, adapter->os_page_size, adapter->os_page_size, alloc_flags);
       zx_pages_memory_for_each_continues(share->dummyfence[1], &share->dummyfence_phy[1], vidmmi_get_dummyfence_addr);

       zx_info("dummy_fence_snoop:      phy_addr=0x%llx\n", share->dummyfence_phy[0]);
       zx_info("dummy_fence_nonsnoop:   phy_addr=0x%llx\n", share->dummyfence_phy[1]);
    }
    return S_OK;
}



//*****************************************************************************
//
// vidschi_init_lookup_table_e3k
//
// Init ring buffer lookup table, this table map from node_ordinal and
// engine_affinity to specific engine index
//
//*****************************************************************************

void vidschi_init_lookup_table_e3k(adapter_t  *adapter, vidsch_query_private_t *query)
{
    int i, j;
    unsigned int hw_ctx_buffer_size = CONTEXT_BUFFER_SIZE_CHX004;

    query->engine_count = RB_NUM;

    adapter->paging_engine_index = RB_INDEX_GFXL;

    if(adapter->hw_caps.local_only)
    {
        query->pcie_segment_id         = SEGMENT_ID_LOCAL_ELT3K;
        query->local_segment_id        = SEGMENT_ID_LOCAL_ELT3K;
        query->fence_buffer_segment_id = SEGMENT_ID_LOCAL_ELT3K;
    }
    else
    {
        query->local_segment_id        = SEGMENT_ID_LOCAL_ELT3K;

        if (adapter->hw_caps.snoop_only)
        {
            query->fence_buffer_segment_id = SEGMENT_ID_PCIE_SNOOPABLE_ELT3K;
            query->pcie_segment_id         = SEGMENT_ID_PCIE_SNOOPABLE_ELT3K;
        }
        else
        {
#if ENABLE_UNSNOOPABLE_WROKAROUND
            query->fence_buffer_segment_id = SEGMENT_ID_PCIE_SNOOPABLE_ELT3K;
            query->pcie_segment_id         = SEGMENT_ID_PCIE_SNOOPABLE_ELT3K;
#else
            query->fence_buffer_segment_id = SEGMENT_ID_PCIE_UNSNOOPABLE_ELT3K;
            query->pcie_segment_id         = SEGMENT_ID_PCIE_UNSNOOPABLE_ELT3K;
#endif
        }

        if(!adapter->ctl_flags.share_space_cover_local)
        {
            query->fence_buffer_segment_id = SEGMENT_ID_LOCAL_ELT3K;
        }
    }

    for ( i = 0; i < query->engine_count; i++)
    {
        int total_size = 0;

        switch(i)
        {
        case RB_INDEX_GFXL:
            adapter->engine_index_table[i].node_ordinal    = 0;
            adapter->engine_index_table[i].engine_affinity = 1;

            query->engine_caps[i]      = ENGINE_CAPS_PAGING | ENGINE_CAPS_PRESENT_ALL |
                                                    ENGINE_CAPS_2D_GRAPHICS | ENGINE_CAPS_3D_GRAPHICS;
            query->engine_ctrl[i]      = ENGINE_CTRL_THREAD_ENABLE;

            query->dma_buffer_size[i]  = 20*1024*1024;
            query->pcie_memory_size[i] = RING_BUFFER_SIZE_E3K;
            query->local_memory_size[i]= ((hw_ctx_buffer_size * (HW_CONTEXT_COUNT_E3K+1))
                                                                + BEGIN_END_BUF_SIZE_E3K);

            total_size += E3K_3DBLT_VB_SIZE;

            total_size += _3DBLT_DATA_RESERVE_SIZE;

            query->local_memory_size[i] += total_size;

            if(adapter->hw_caps.local_only || !adapter->ctl_flags.share_space_cover_local)
            {
                query->pcie_memory_size[i]   = 0;
                query->local_memory_size[i] += RING_BUFFER_SIZE_E3K;
            }

            query->engine_func[i]        = &engine_gfx_low_chip_func_e3k;
            query->engine_hw_queue_size[i] = E3K_ENGINE_HW_QUEUE_SIZE;
            break;

        case RB_INDEX_GFXH:
            adapter->engine_index_table[i].node_ordinal    = 1;
            adapter->engine_index_table[i].engine_affinity = 1;

            query->engine_caps[i]      = ENGINE_CAPS_PRESENT_ALL | ENGINE_CAPS_2D_GRAPHICS | ENGINE_CAPS_3D_GRAPHICS;
            query->engine_ctrl[i]      = ENGINE_CTRL_DISABLE;
            query->dma_buffer_size[i]  = 1*1024*1024;
            query->pcie_memory_size[i] = RING_BUFFER_SIZE_E3K;
            query->local_memory_size[i]= (hw_ctx_buffer_size * (HW_CONTEXT_COUNT_E3K+1) );

            if(adapter->hw_caps.local_only || !adapter->ctl_flags.share_space_cover_local)
            {
                query->pcie_memory_size[i]   = 0;
                query->local_memory_size[i] += RING_BUFFER_SIZE_E3K;
            }

            query->engine_func[i]      = &engine_gfx_high_chip_func_e3k;
            query->engine_hw_queue_size[i] = E3K_ENGINE_HW_QUEUE_SIZE;
            break;

        case RB_INDEX_CSH:
            adapter->engine_index_table[i].node_ordinal    = 2;
            adapter->engine_index_table[i].engine_affinity = 1;

            query->engine_caps[i]      = ENGINE_CAPS_CS;
            query->engine_ctrl[i]      = ENGINE_CTRL_DISABLE;
            query->dma_buffer_size[i]  = 1*1024*1024;
            query->pcie_memory_size[i] = RING_BUFFER_SIZE_E3K;
            query->local_memory_size[i]= 0;

            if(adapter->hw_caps.local_only || !adapter->ctl_flags.share_space_cover_local)
            {
                query->pcie_memory_size[i]   = 0;
                query->local_memory_size[i] += RING_BUFFER_SIZE_E3K;
            }

            query->engine_func[i]      = &engine_cs_high_chip_func_e3k;
            query->engine_hw_queue_size[i] = E3K_ENGINE_HW_QUEUE_SIZE;
            break;

        case RB_INDEX_CSL0:
            adapter->engine_index_table[i].node_ordinal    = 3;
            adapter->engine_index_table[i].engine_affinity = 1;

            query->engine_caps[i]      = ENGINE_CAPS_CS;
            query->engine_ctrl[i]      = ENGINE_CTRL_DISABLE;
            query->dma_buffer_size[i]  = 1*1024*1024;
            query->pcie_memory_size[i] = RING_BUFFER_SIZE_E3K;
            query->local_memory_size[i]= 0;

            if(adapter->hw_caps.local_only || !adapter->ctl_flags.share_space_cover_local)
            {
                query->pcie_memory_size[i]   = 0;
                query->local_memory_size[i] += RING_BUFFER_SIZE_E3K;
            }

            query->engine_func[i]      = &engine_cs_low0_chip_func_e3k;
            query->engine_hw_queue_size[i] = E3K_ENGINE_HW_QUEUE_SIZE;
            break;

        case RB_INDEX_CSL1:
            // Paging RB visible in KMD and KMD runtime
            adapter->engine_index_table[i].node_ordinal    = 4;
            adapter->engine_index_table[i].engine_affinity = 1;

            query->engine_caps[i]      =  ENGINE_CAPS_CS;
            query->engine_ctrl[i]      =  ENGINE_CTRL_DISABLE;
            query->dma_buffer_size[i]  = 1*1024*1024;
            query->pcie_memory_size[i] = RING_BUFFER_SIZE_E3K;

            if(adapter->hw_caps.local_only || !adapter->ctl_flags.share_space_cover_local)
            {
                query->pcie_memory_size[i]   = 0;
                query->local_memory_size[i] += RING_BUFFER_SIZE_E3K;
            }

            query->engine_func[i]      =  &engine_cs_low1_chip_func_e3k;
            query->engine_hw_queue_size[i] = E3K_ENGINE_HW_QUEUE_SIZE;
            break;

        case RB_INDEX_CSL2:
            adapter->engine_index_table[i].node_ordinal    = 5;
            adapter->engine_index_table[i].engine_affinity = 1;

            query->engine_caps[i]      =  ENGINE_CAPS_CS;
            query->engine_ctrl[i]      =  ENGINE_CTRL_DISABLE;
            query->dma_buffer_size[i]  = 1*1024*1024;
            query->pcie_memory_size[i] = RING_BUFFER_SIZE_E3K;
            query->local_memory_size[i]= 0;

            if(adapter->hw_caps.local_only || !adapter->ctl_flags.share_space_cover_local)
            {
                query->pcie_memory_size[i]   = 0;
                query->local_memory_size[i] += RING_BUFFER_SIZE_E3K;
            }

            query->engine_func[i]      = &engine_cs_low2_chip_func_e3k;
            query->engine_hw_queue_size[i] = E3K_ENGINE_HW_QUEUE_SIZE;
            break;

        case RB_INDEX_CSL3:
            adapter->engine_index_table[i].node_ordinal    = 6;
            adapter->engine_index_table[i].engine_affinity = 1;

            query->engine_caps[i]      = ENGINE_CAPS_CS ;
            query->engine_ctrl[i]      = ENGINE_CTRL_DISABLE;
            query->dma_buffer_size[i]  = 1*1024*1024;
            query->pcie_memory_size[i] = RING_BUFFER_SIZE_E3K;
            query->local_memory_size[i]= 0;

            if(adapter->hw_caps.local_only || !adapter->ctl_flags.share_space_cover_local)
            {
                query->pcie_memory_size[i]   = 0;
                query->local_memory_size[i] += RING_BUFFER_SIZE_E3K;
            }

            query->engine_func[i]      = &engine_cs_low3_chip_func_e3k;
            query->engine_hw_queue_size[i] = E3K_ENGINE_HW_QUEUE_SIZE;
            break;

        case RB_INDEX_VCP0:
            adapter->engine_index_table[i].node_ordinal    = 7;
            adapter->engine_index_table[i].engine_affinity = 1;

            query->engine_caps[i]      = ENGINE_CAPS_VIDEO_DECODE | ENGINE_CAPS_VIDEO_ENCODE | ENGINE_CAPS_NO_HWCTX;
            query->engine_ctrl[i]      = ENGINE_CTRL_THREAD_ENABLE ;
            //query->engine_ctrl[i]      = ENGINE_CTRL_DISABLE;
            query->dma_buffer_size[i]  = 1*1024*1024;
            query->pcie_memory_size[i] = RING_BUFFER_SIZE_E3K;
            query->local_memory_size[i]= VIDEO_BRIDGE_BUF_SIZE;

            if(adapter->hw_caps.local_only || !adapter->ctl_flags.share_space_cover_local)
            {
                query->pcie_memory_size[i]   = 0;
                query->local_memory_size[i] += RING_BUFFER_SIZE_E3K;
            }

            query->engine_func[i]      = &engine_vcp_low0_chip_func_e3k;
            query->engine_hw_queue_size[i] = E3K_ENGINE_HW_QUEUE_SIZE;
            break;
        case RB_INDEX_VCP1:
            adapter->engine_index_table[i].node_ordinal    = 8;
            adapter->engine_index_table[i].engine_affinity = 1;

            query->engine_caps[i]      = ENGINE_CAPS_VIDEO_DECODE | ENGINE_CAPS_VIDEO_ENCODE | ENGINE_CAPS_NO_HWCTX;
            query->engine_ctrl[i]      = ENGINE_CTRL_THREAD_ENABLE;
            //query->engine_ctrl[i]      = ENGINE_CTRL_DISABLE;
            query->dma_buffer_size[i]  = 1*1024*1024;
            query->pcie_memory_size[i] = RING_BUFFER_SIZE_E3K;
            query->local_memory_size[i]= VIDEO_BRIDGE_BUF_SIZE;

            if(adapter->hw_caps.local_only || !adapter->ctl_flags.share_space_cover_local)
            {
                query->pcie_memory_size[i]   = 0;
                query->local_memory_size[i] += RING_BUFFER_SIZE_E3K;
            }

            query->engine_func[i]      = &engine_vcp_low1_chip_func_e3k;
            query->engine_hw_queue_size[i] = E3K_ENGINE_HW_QUEUE_SIZE;
            break;
        case RB_INDEX_VCP2:
        //case RB_INDEX_VPP:
            if(adapter->chip_id == CHIP_CHX004){
                adapter->engine_index_table[i].node_ordinal    = 9;
                adapter->engine_index_table[i].engine_affinity = 1;

                query->engine_caps[i]      = ENGINE_CAPS_VIDEO_VPP | ENGINE_CAPS_NO_HWCTX;
                query->engine_ctrl[i]      = ENGINE_CTRL_THREAD_ENABLE;
                //query->engine_ctrl[i]      = ENGINE_CTRL_DISABLE;
                query->dma_buffer_size[i]  = 1*1024*1024;
                query->pcie_memory_size[i] = RING_BUFFER_SIZE_E3K;
                query->local_memory_size[i]= 0;

                if(adapter->hw_caps.local_only || !adapter->ctl_flags.share_space_cover_local)
                {
                    query->pcie_memory_size[i]   = 0;
                    query->local_memory_size[i] += RING_BUFFER_SIZE_E3K;
                }

                query->engine_func[i]      = &engine_vpp_chip_func_e3k;
                query->engine_hw_queue_size[i] = E3K_ENGINE_HW_QUEUE_SIZE;
            }
            if(adapter->chip_id == CHIP_CNE001){
                adapter->engine_index_table[i].node_ordinal    = 9;
                adapter->engine_index_table[i].engine_affinity = 1;

                query->engine_caps[i]      = ENGINE_CAPS_VIDEO_DECODE | ENGINE_CAPS_VIDEO_ENCODE | ENGINE_CAPS_NO_HWCTX;
                query->engine_ctrl[i]      = ENGINE_CTRL_THREAD_ENABLE;
                //query->engine_ctrl[i]      = ENGINE_CTRL_DISABLE;
                query->dma_buffer_size[i]  = 1*1024*1024;
                query->pcie_memory_size[i] = RING_BUFFER_SIZE_E3K;
                query->local_memory_size[i]= VIDEO_BRIDGE_BUF_SIZE;

                if(adapter->hw_caps.local_only || !adapter->ctl_flags.share_space_cover_local)
                {
                    query->pcie_memory_size[i]   = 0;
                    query->local_memory_size[i] += RING_BUFFER_SIZE_E3K;
                }

                query->engine_func[i]      = &engine_vcp_low2_chip_func_e3k;
                query->engine_hw_queue_size[i] = E3K_ENGINE_HW_QUEUE_SIZE;
            }
            break;
        case RB_INDEX_VPP0:
            adapter->engine_index_table[i].node_ordinal    = 10;
            adapter->engine_index_table[i].engine_affinity = 1;

            query->engine_caps[i]      = ENGINE_CAPS_VIDEO_VPP | ENGINE_CAPS_NO_HWCTX;
            query->engine_ctrl[i]      = ENGINE_CTRL_THREAD_ENABLE;
            //query->engine_ctrl[i]      = ENGINE_CTRL_DISABLE;
            query->dma_buffer_size[i]  = 1*1024*1024;
            query->pcie_memory_size[i] = RING_BUFFER_SIZE_E3K;
            query->local_memory_size[i]= 0;

            if(adapter->hw_caps.local_only || !adapter->ctl_flags.share_space_cover_local)
            {
                query->pcie_memory_size[i]   = 0;
                query->local_memory_size[i] += RING_BUFFER_SIZE_E3K;
            }

            query->engine_func[i]      = &engine_vpp0_chip_func_e3k;
            query->engine_hw_queue_size[i] = E3K_ENGINE_HW_QUEUE_SIZE;
            break;
        case RB_INDEX_VPP1:
            adapter->engine_index_table[i].node_ordinal    = 11;
            adapter->engine_index_table[i].engine_affinity = 1;

            query->engine_caps[i]      = ENGINE_CAPS_VIDEO_VPP | ENGINE_CAPS_NO_HWCTX;
            query->engine_ctrl[i]      = ENGINE_CTRL_THREAD_ENABLE;
            //query->engine_ctrl[i]      = ENGINE_CTRL_DISABLE;
            query->dma_buffer_size[i]  = 1*1024*1024;
            query->pcie_memory_size[i] = RING_BUFFER_SIZE_E3K;
            query->local_memory_size[i]= 0;

            if(adapter->hw_caps.local_only || !adapter->ctl_flags.share_space_cover_local)
            {
                query->pcie_memory_size[i]   = 0;
                query->local_memory_size[i] += RING_BUFFER_SIZE_E3K;
            }

            query->engine_func[i]      = &engine_vpp1_chip_func_e3k;
            query->engine_hw_queue_size[i] = E3K_ENGINE_HW_QUEUE_SIZE;
            break;
        default:
            zx_assert(0, "");
            break;
        }

        if(adapter->hw_caps.local_only || !adapter->ctl_flags.share_space_cover_local)
        {
            query->dma_segment[i]      = SEGMENT_ID_LOCAL_ELT3K;
        }
        else
        {
            query->dma_segment[i]      = SEGMENT_ID_PCIE_SNOOPABLE_ELT3K;

            if(! adapter->hw_caps.snoop_only)
            {
#if !ENABLE_UNSNOOPABLE_WROKAROUND
                query->dma_segment[i]      = SEGMENT_ID_PCIE_UNSNOOPABLE_ELT3K;
#endif
            }
        }
    }

    if(adapter->ctl_flags.hang_dump)
    {
        engine_share_e3k_t      *share     = adapter->private_data;
        unsigned int            segment_id        = SEGMENT_ID_LOCAL_ELT3K;
        vidmm_segment_t         *segment          = vidmm_get_segment_by_id(adapter, segment_id);

        /* reserve ring buffer for hang */
        share->ring_buffer_for_hang   = vidmm_create_segment_memory(adapter, segment_id, zx_align_up(MAX_HANG_DUMP_DATA_POOL_NUMBER * HangDump_RingBufferBlockSize + sizeof(dh_common_info_e3k), 4096), 1, 1);
        vidmm_map_segment_memory(adapter, share->ring_buffer_for_hang);
        vidmm_lock_segment_memory(adapter, share->ring_buffer_for_hang);

        /* reserve pcie buffer for copy cpu invisble segment out*/
        segment_id = SEGMENT_ID_PCIE_SNOOPABLE_ELT3K;
        share->transfer_buffer_for_hang = vidmm_create_segment_memory(adapter, segment_id, E3K_TRANSFERBUFFER_FOR_HANG_SIZE, 0, 1);
        vidmm_map_segment_memory(adapter, share->transfer_buffer_for_hang);
        vidmm_lock_segment_memory(adapter, share->transfer_buffer_for_hang);
    }
}

//*****************************************************************************
//
//  vidschi_init_hw_settings_e3k
//
// function to init HW MXU register, so that PCIe Ring Buffer can
//  work properly.
//
//*****************************************************************************
void vidschi_init_hw_settings_e3k(adapter_t *adapter)
{
    Reg_Mmu_Mode                    reg_Mmu_Mode               = {0};
    Reg_Mmu_Mode1                   reg_Mmu_Mode1              = {0};
    Reg_Bl_Size                     reg_Bl_Size                = {0};
    Reg_Bl_Size1                    reg_Bl_Size1               = {0};
    Reg_Mxu_Dec_Ctrl                reg_Mxu_Dec_Ctrl           = {0};
    Reg_Mxu_Dec_Ctrl1               reg_Mxu_Dec_Ctrl1          = {0};
    Reg_Mxu_Channel_Control         reg_Mxu_Channel_Control    = {0};
    Reg_Mxu_Channel_Control1        reg_Mxu_Channel_Control1   = {0};
    Reg_Ttbr                        reg_Ttbr                   = {0};
    unsigned int                    dwAdapterMemorySize        = 0;
    unsigned char *                 pRegAddr                   = NULL;
    Reg_Rb0_Fl2                     KKKRb0                     = {0};
    Reg_Rb1_Fl2                     KKKRb1                     = {0};
    Reg_Rb2_Fl2                     KKKRb2                     = {0};
    Reg_Argument_Fl2                ArgumentBuffer             = {0};
    Reg_Mxu_Dummy_Fence_Addr_Gmi0   reg_Mxu_Dummy_Fence_Addr_Gmi0 = { 0 };
    Reg_Mxu_Dummy_Fence_Addr_Gmi1   reg_Mxu_Dummy_Fence_Addr_Gmi1 = { 0 };
    Reg_Mxu_Dummy_Fence_Addr_Bium   reg_Mxu_Dummy_Fence_Addr_Bium = { 0 };
    engine_share_e3k_t              *share = adapter->private_data;
    unsigned long long              GranuleOfFB = (adapter->chip_id >= CHIP_CHX004) ? 24 : 28;  //CHX004 : 16M ; Elite3000 : 256M

    struct range_allocator          *bl_ra;
    unsigned int value;

    // Due to HW issue, give fake legacy_offset/size to make sure all VA go to its own va space,
    // include share context space.
    reg_Mmu_Mode.reg.Vmen          = 1; //use VA mode
    reg_Mmu_Mode.reg.Video_Size    = (adapter->total_local_memory_size >> GranuleOfFB) - 1;
    reg_Mmu_Mode.reg.Legacy_Size   = adapter->shared_gpu_va_size >> 32;
    reg_Mmu_Mode.reg.Legacy_Offset = adapter->shared_gpu_va_offset >> 32;

    if (adapter->chip_id >= CHIP_CHX004)
    {
        --reg_Mmu_Mode.reg.Legacy_Size;
    }

    if (!adapter->hw_caps.legacy_offset_enable)
    {
        reg_Mmu_Mode.reg.Legacy_Size   = 0xFF;
        reg_Mmu_Mode.reg.Legacy_Offset = 0xFF;
    }

    pRegAddr = adapter->mmio + MMIO_MXU_START_ADDRESS + Reg_Mmu_Mode_Offset*4;
    zx_write32(pRegAddr, reg_Mmu_Mode.uint);

    zx_info("reg_mmu: 0x%x, video_size: %d, legace_size: %d, legacy_offset: %d\n", zx_read32(pRegAddr), reg_Mmu_Mode.reg.Video_Size, reg_Mmu_Mode.reg.Legacy_Size, reg_Mmu_Mode.reg.Legacy_Offset);

    if (adapter->chip_id >= CHIP_CHX004)
    {
        reg_Mmu_Mode1.uint = reg_Mmu_Mode.uint;
        pRegAddr = adapter->mmio + MMIO_MXU_START_ADDRESS + Reg_Mmu_Mode1_Offset*4;
        zx_write32(pRegAddr, reg_Mmu_Mode1.uint);

        pRegAddr = adapter->mmio + 0x818c;  // DIU_SYS_VID_BD
        zx_write32(pRegAddr, reg_Mmu_Mode.reg.Video_Size);

        /* share pt with DIU */
        zx_write32((adapter->mmio + 0x81E8), adapter->share_pcie_level3_gpu_va >> 5);

    }

    reg_Mxu_Dec_Ctrl.reg.Ddec_Antilock  = 1;
    pRegAddr = adapter->mmio + MMIO_MXU_START_ADDRESS + Reg_Mxu_Dec_Ctrl_Offset*4;
    zx_write32(pRegAddr, reg_Mxu_Dec_Ctrl.uint);

    if (adapter->chip_id >= CHIP_CHX004)
    {
        reg_Mxu_Dec_Ctrl1.reg.Ddec_Antilock  = 1;
        pRegAddr = adapter->mmio + MMIO_MXU_START_ADDRESS + Reg_Mxu_Dec_Ctrl1_Offset*4;
        zx_write32(pRegAddr, reg_Mxu_Dec_Ctrl1.uint);
    }
    zx_info("reg_Mxu_Dec_Ctrl.uint 0x%x readvalue: 0x%x \n", reg_Mxu_Dec_Ctrl.uint, zx_read32(pRegAddr));

    //BL related
    bl_ra = vidmm_get_burst_length_allocator(adapter);
    reg_Bl_Size.reg.Buffer_Offset = bl_ra->start >> 16;//64k align
    reg_Bl_Size.reg.Buffer_Size = BL_SIZE_BUFFER_SIZE_32MB;
    reg_Bl_Size.reg.Invalid_Bl_To_Rf_En = 1;//1: enable MXU invalid BL detect, if met invalid BL, would hang.
    pRegAddr = adapter->mmio + MMIO_MXU_START_ADDRESS + Reg_Bl_Size_Offset*4;
    zx_write32(pRegAddr, reg_Bl_Size.uint);

    if (adapter->chip_id >= CHIP_CHX004)
    {
        pRegAddr = adapter->mmio + MMIO_MXU_START_ADDRESS + Reg_Bl_Size1_Offset*4;
        reg_Bl_Size1.reg.Buffer_Offset = bl_ra->start >> 16;//64k align
        reg_Bl_Size1.reg.Buffer_Size1 = BL_SIZE_BUFFER_SIZE_32MB;
        reg_Bl_Size1.reg.Invalid_Bl_To_Rf_En = 1;
        zx_write32(pRegAddr, reg_Bl_Size1.uint);
    }


    pRegAddr = adapter->mmio + MMIO_MXU_START_ADDRESS + Reg_Diu_Reserve_Ctrl_Offset*4;
    if (adapter->hw_caps.miu_channel_num == 1)
    {
        reg_Mxu_Channel_Control.reg.Miu_Channel0_Disable = 0;
        reg_Mxu_Channel_Control.reg.Miu_Channel1_Disable = 1;
        reg_Mxu_Channel_Control.reg.Miu_Channel2_Disable = 1;

        zx_write32(pRegAddr, 0x1200003);
    }
    else if (adapter->hw_caps.miu_channel_num == 2)
    {
        reg_Mxu_Channel_Control.reg.Miu_Channel0_Disable = 0;
        reg_Mxu_Channel_Control.reg.Miu_Channel1_Disable = 0;
        reg_Mxu_Channel_Control.reg.Miu_Channel2_Disable = 1;

        zx_write32(pRegAddr, 0x1300003);
    }
    else if (adapter->hw_caps.miu_channel_num == 3)
    {
        reg_Mxu_Channel_Control.reg.Miu_Channel0_Disable = 0;
        reg_Mxu_Channel_Control.reg.Miu_Channel1_Disable = 0;
        reg_Mxu_Channel_Control.reg.Miu_Channel2_Disable = 0;

        zx_write32(pRegAddr, 0x1360003);
    }
    else
    {
        zx_assert(0,"bad miu channel num");
    }

    pRegAddr = adapter->mmio + MMIO_MXU_START_ADDRESS + Reg_Pending_Buf_Ctrl_0_Offset * 4;
    zx_write32(pRegAddr, 0x4);

    reg_Mxu_Channel_Control.reg.Ch_Size = adapter->hw_caps.miu_channel_size;
    pRegAddr =adapter->mmio + MMIO_MXU_START_ADDRESS + Reg_Mxu_Channel_Control_Offset*4;
    zx_write32(pRegAddr, reg_Mxu_Channel_Control.uint);
    //zx_info("reg_Mxu_Channel_Control.uint 0x%x readvalue: 0x%x \n", reg_Mxu_Channel_Control.uint, zx_read32(pRegAddr));

    if (adapter->chip_id >= CHIP_CHX004)
    {
        reg_Mxu_Channel_Control1.reg.Miu_Channel0_Disable = reg_Mxu_Channel_Control.reg.Miu_Channel0_Disable;
        reg_Mxu_Channel_Control1.reg.Miu_Channel1_Disable = reg_Mxu_Channel_Control.reg.Miu_Channel1_Disable;
        reg_Mxu_Channel_Control1.reg.Miu_Channel2_Disable = reg_Mxu_Channel_Control.reg.Miu_Channel2_Disable;
        reg_Mxu_Channel_Control1.reg.Ch_Size1 = reg_Mxu_Channel_Control.reg.Ch_Size;
        pRegAddr = adapter->mmio + MMIO_MXU_START_ADDRESS + Reg_Mxu_Channel_Control1_Offset*4;
        zx_write32(pRegAddr, reg_Mxu_Channel_Control1.uint);
    }

    if(adapter->chip_id >= CHIP_CHX004)
    {
        reg_Mxu_Dummy_Fence_Addr_Gmi0.reg.Address = (share->dummyfence_phy[1] >> 6);
        pRegAddr = adapter->mmio + MMIO_MXU_START_ADDRESS + Reg_Mxu_Dummy_Fence_Addr_Gmi0_Offset * 4;
        zx_write32(pRegAddr, reg_Mxu_Dummy_Fence_Addr_Gmi0.uint);
        zx_debug("reg_Mxu_Dummy_Fence_Addr_Gmi0.reg.Address: %x\n", reg_Mxu_Dummy_Fence_Addr_Gmi0.reg.Address);

        reg_Mxu_Dummy_Fence_Addr_Gmi1.reg.Address = (share->dummyfence_phy[1] >> 6) + 4;
        pRegAddr = adapter->mmio + MMIO_MXU_START_ADDRESS + Reg_Mxu_Dummy_Fence_Addr_Gmi1_Offset * 4;
        zx_write32(pRegAddr, reg_Mxu_Dummy_Fence_Addr_Gmi1.uint);
        zx_debug("reg_Mxu_Dummy_Fence_Addr_Gmi1.reg.Address: %x\n", reg_Mxu_Dummy_Fence_Addr_Gmi1.reg.Address);

        reg_Mxu_Dummy_Fence_Addr_Bium.reg.Address = (share->dummyfence_phy[0] >> 6);
        pRegAddr = adapter->mmio + MMIO_MXU_START_ADDRESS + Reg_Mxu_Dummy_Fence_Addr_Bium_Offset * 4;
        zx_write32(pRegAddr, reg_Mxu_Dummy_Fence_Addr_Bium.uint);
        zx_debug("reg_Mxu_Dummy_Fence_Addr_Bium.reg.Address: %x\n", reg_Mxu_Dummy_Fence_Addr_Bium.reg.Address);
    }

    KKKRb0.reg.Size             = RING_BUFFER_SIZE_E3K >> 16;
    KKKRb1.reg.Size             = RING_BUFFER_SIZE_E3K >> 16;
    KKKRb2.reg.Size             = RING_BUFFER_SIZE_E3K >> 16;
    ArgumentBuffer.reg.Size     = KKK_ARGUMENT_BUFFER_SIZE >> 16;

    pRegAddr = adapter->mmio + MMIO_MXU_START_ADDRESS + Reg_Rb0_Fl2_Offset*4;
    zx_write32(pRegAddr, KKKRb0.uint);
    pRegAddr = adapter->mmio + MMIO_MXU_START_ADDRESS + Reg_Rb1_Fl2_Offset*4;
    zx_write32(pRegAddr, KKKRb1.uint);
    pRegAddr = adapter->mmio + MMIO_MXU_START_ADDRESS + Reg_Rb2_Fl2_Offset*4;
    zx_write32(pRegAddr, KKKRb2.uint);
    pRegAddr = adapter->mmio + MMIO_MXU_START_ADDRESS + Reg_Argument_Fl2_Offset*4;
    zx_write32(pRegAddr, ArgumentBuffer.uint);
}

int vidsch_query_chip(adapter_t *adapter, vidsch_query_private_t *info)
{
    vidschi_init_engine_share_e3k(adapter);

    vidschi_init_lookup_table_e3k(adapter, info);

    vidschi_reset_adapter_e3k(adapter);

    vidschi_init_hw_settings_e3k(adapter);

    vidschi_init_dvfs_setting_004(adapter);

    return 0;
}

int vidsch_query_vm_id(adapter_t *adapter, vidsch_query_vm_id_t *info)
{
    info->total_vm_id_num    = 16;
    info->reserved_vm_id_num = 1;//reserve vm id 0 for share space(kmd) use.
    return 0;
}
