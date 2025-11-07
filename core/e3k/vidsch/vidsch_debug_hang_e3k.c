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
#include "vidsch_debug_hang_e3k.h"
#include "vidsch_engine_e3k.h"
#include "mm_e3k.h"
#include "context.h"
#include "global.h"
#include "chip_include_e3k.h"


#define PRINT_AND_DUMP_FILE(__printer, __buf, __buf_size, __need_dump_file) \
do { \
    zx_printf(__printer, __buf); \
    if(__need_dump_file) \
    { \
        util_write_to_file(__buf, __buf_size, "", HW_HANG_LOG_FILE); \
    } \
} while(0) \


extern void enginei_mmio_invalidate_tlb_e3k (adapter_t * adapter);


int vidschi_get_dump_data_gpu_offset_e3k(adapter_t *adapter, unsigned int type, unsigned int index)
{
    engine_share_e3k_t *share    = adapter->private_data;
    vidmm_segment_memory_t *ring_buffer_for_hang    = share->ring_buffer_for_hang;
    unsigned int   buffer_gpu_offset           = 0;

    index = index % MAX_HANG_DUMP_DATA_POOL_NUMBER;

    switch (type)
    {
    case HANG_DUMP_RING_BUFFER_OFFSET:
        buffer_gpu_offset = ring_buffer_for_hang->gpu_va + index * HangDump_RingBufferBlockSize;
        break;
    case HANG_DUMP_DH_COMMON_INFO_OFFSET:
        buffer_gpu_offset = ring_buffer_for_hang->gpu_va + MAX_HANG_DUMP_DATA_POOL_NUMBER * HangDump_RingBufferBlockSize;
        break;
    default:
        break;
    }
    return buffer_gpu_offset;
}

unsigned char *vidschi_get_dump_data_cpu_virt_addr_e3k(adapter_t *adapter, unsigned int type, unsigned int index)
{
    engine_share_e3k_t *share    = adapter->private_data;
    vidmm_segment_memory_t *ring_buffer_for_hang    = share->ring_buffer_for_hang;
    unsigned char  *virt_addr                  = NULL;

    index = index % MAX_HANG_DUMP_DATA_POOL_NUMBER;

    switch (type)
    {
    case HANG_DUMP_RING_BUFFER_OFFSET:
        virt_addr = ring_buffer_for_hang->krnl_cpu_vma->virt_addr + index * HangDump_RingBufferBlockSize;
        break;
    case HANG_DUMP_DH_COMMON_INFO_OFFSET:
        virt_addr = ring_buffer_for_hang->krnl_cpu_vma->virt_addr + MAX_HANG_DUMP_DATA_POOL_NUMBER * HangDump_RingBufferBlockSize;
        break;
    default:
        break;
    }

    return virt_addr;
}

#define DEBUG_BUS_VERSION   "0.0.3"

void vidsch_dump_debugbus_label(adapter_t *adapter, struct os_printer *p)
{
    unsigned char  debug_bus_label[256];

    zx_memset(debug_bus_label, 0, sizeof(debug_bus_label));
    zx_vsprintf(debug_bus_label, "OS: linux\n");
    PRINT_AND_DUMP_FILE(p, debug_bus_label, zx_strlen(debug_bus_label), adapter->ctl_flags.dump_hang_info_to_file);

    zx_memset(debug_bus_label, 0, sizeof(debug_bus_label));
    zx_vsprintf(debug_bus_label, "Debugbus version:"DEBUG_BUS_VERSION"\n");
    PRINT_AND_DUMP_FILE(p, debug_bus_label, zx_strlen(debug_bus_label), adapter->ctl_flags.dump_hang_info_to_file);

    zx_memset(debug_bus_label, 0, sizeof(debug_bus_label));
    if(adapter->chip_id == CHIP_CHX004)
    {
        zx_vsprintf(debug_bus_label, "Project: CHX004\n");
    }
    else if (adapter->chip_id == CHIP_CNE001)
    {
        zx_vsprintf(debug_bus_label, "Project: CNE001\n");
    }

    PRINT_AND_DUMP_FILE(p, debug_bus_label, zx_strlen(debug_bus_label), adapter->ctl_flags.dump_hang_info_to_file);
}


void vidsch_display_debugbus_info_e3k(adapter_t *adapter, struct os_printer *p, int video)
{
    unsigned char  sr1a = 0;
    unsigned char  crc8;//Debug bus 0 select
    unsigned char  crc9;
    unsigned char  crca;//Debug bus 1 select
    unsigned char  crcb;
    unsigned char  crcc;//Debug bus 2 select
    unsigned char  crcd;
    unsigned char  crce;
    unsigned char  crcf;
    unsigned char  statu0 = 0;//debug bus status
    unsigned char  statu1 = 0;
    unsigned char  statu2 = 0;
    unsigned char  statu3 = 0;
    unsigned int   i,j,k;
    unsigned char  debug_bus_buffer[256];
    struct os_file *file;

    vidsch_dump_debugbus_label(adapter, p);

    zx_memset(debug_bus_buffer, 0, sizeof(debug_bus_buffer));
    zx_vsprintf(debug_bus_buffer, "~~~~~~~~~~~~~~debug bus info~~~~~~~~~~~~~~\n");
    PRINT_AND_DUMP_FILE(p, debug_bus_buffer, zx_strlen(debug_bus_buffer), adapter->ctl_flags.dump_hang_info_to_file);

    //enable read debug bus from diu
    sr1a = zx_read8(adapter->mmio + 0x861a);
    zx_write8(adapter->mmio + 0x861a, sr1a | 0x10);

    if(video)
    {
        unsigned char * video_mmio = NULL;
        for(k = 0; k <= 0x1ff; k++)
        {
            video_mmio = adapter->mmio + 0x4C000 + k*4;
            i = zx_read32(video_mmio);

            zx_memset(debug_bus_buffer, 0, sizeof(debug_bus_buffer));
            zx_vsprintf(debug_bus_buffer,"%08x: %08x\n", video_mmio, i);
            PRINT_AND_DUMP_FILE(p, debug_bus_buffer, zx_strlen(debug_bus_buffer), adapter->ctl_flags.dump_hang_info_to_file);
        }

        for(k = 0; k <= 0x1ff; k++)
        {
            video_mmio = adapter->mmio + 0x4A000 + k*4;
            i = zx_read32(video_mmio);

            zx_memset(debug_bus_buffer, 0, sizeof(debug_bus_buffer));
            zx_vsprintf(debug_bus_buffer,"%08x: %08x\n", video_mmio, i);
            PRINT_AND_DUMP_FILE(p, debug_bus_buffer, zx_strlen(debug_bus_buffer), adapter->ctl_flags.dump_hang_info_to_file);
        }

        if(adapter->chip_id == CHIP_CNE001)
        {
            for(k = 0; k <= 0x1ff; k++)
            {
                video_mmio = adapter->mmio + 0x4D000 + k*4;
                i = zx_read32(video_mmio);

                zx_memset(debug_bus_buffer, 0, sizeof(debug_bus_buffer));
                zx_vsprintf(debug_bus_buffer,"%08x: %08x\n", video_mmio, i);
                PRINT_AND_DUMP_FILE(p, debug_bus_buffer, zx_strlen(debug_bus_buffer), adapter->ctl_flags.dump_hang_info_to_file);
            }
        }
    }

    if(adapter->chip_id == CHIP_CHX004)
    {
        for(i=0; i<sizeof(debug_bus_info_CHX004)/sizeof(debug_bus_info); i++)
        {
            if(debug_bus_info_CHX004[i].scope == CHIP1_SCOPE_DEBUG_BUS)
            {
                zx_vsprintf(debug_bus_buffer, "-----------------%s----------------\n", debug_bus_info_CHX004[i].group_name);
                zx_printf(p, debug_bus_buffer);

                if(adapter->ctl_flags.dump_hang_info_to_file)
                {
                    util_write_to_file(debug_bus_buffer, zx_strlen(debug_bus_buffer) , "", HW_HANG_LOG_FILE);
                }

                for(j=debug_bus_info_CHX004[i].group_start_offset; j<=debug_bus_info_CHX004[i].group_end_offset; j++)
                {
                    /*
                    if((!zx_strcmp(debug_bus_info_CHX004[i].group_name, "BIU group")))
                    {
                        crc8 = ((j & 0x3800)>>11) << 3;//should be configed to crc8[5:3]
                        crcc = (j & 0x0700)>>8;
                        crcf = j & 0x00ff;
                        zx_write8(adapter->mmio + 0x88C8, crc8);
                        zx_write8(adapter->mmio + 0x88CC, crcc);
                        zx_write8(adapter->mmio + 0x88CF, crcf);

                        statu0 = zx_read8(adapter->mmio + 0x88C0);
                        statu1 = zx_read8(adapter->mmio + 0x88C1);
                        statu2 = zx_read8(adapter->mmio + 0x88C2);
                        statu3 = zx_read8(adapter->mmio + 0x88C3);

                        zx_vsprintf(debug_bus_buffer,"%08x: %08x\n", j, (statu0<<24 | statu1<<16 | statu2<<8 | statu3));
                        zx_printf(p, debug_bus_buffer);

                        if(adapter->ctl_flags.dump_hang_info_to_file)
                        {
                            util_write_to_file(debug_bus_buffer, zx_strlen(debug_bus_buffer), "", HW_HANG_LOG_FILE);
                        }
                    }
                    else
                    */
                    {
                        crc8 = (j & 0x3f00)>>8;
                        crc9 = j & 0x00ff;
                        zx_write8(adapter->mmio + 0x88C8, crc8);
                        zx_write8(adapter->mmio + 0x88C9, crc9);

                        statu0 = zx_read8(adapter->mmio + 0x88C0);
                        statu1 = zx_read8(adapter->mmio + 0x88C1);
                        statu2 = zx_read8(adapter->mmio + 0x88C2);
                        statu3 = zx_read8(adapter->mmio + 0x88C3);

                        zx_vsprintf(debug_bus_buffer,"%08x: %08x\n", j, (statu0<<24 | statu1<<16 | statu2<<8 | statu3));
                        zx_printf(p, debug_bus_buffer);
                        if(adapter->ctl_flags.dump_hang_info_to_file)
                        {
                            util_write_to_file(debug_bus_buffer, zx_strlen(debug_bus_buffer), "", HW_HANG_LOG_FILE);
                        }
                    }
                }
            }
            else if(debug_bus_info_CHX004[i].scope == CHIP2_SCOPE_DEBUG_BUS)
            {
                zx_vsprintf(debug_bus_buffer, "-----------------%s----------------\n", debug_bus_info_CHX004[i].group_name);
                zx_printf(p, debug_bus_buffer);

                if(adapter->ctl_flags.dump_hang_info_to_file)
                {
                    util_write_to_file(debug_bus_buffer, zx_strlen(debug_bus_buffer) , "", HW_HANG_LOG_FILE);
                }

                for(j=debug_bus_info_CHX004[i].group_start_offset; j<=debug_bus_info_CHX004[i].group_end_offset; j++)
                {
                    crca = (j & 0x3f00)>>8;
                    crcb = j & 0x00ff;
                    zx_write8(adapter->mmio+ 0x88CA, crca);
                    zx_write8(adapter->mmio+ 0x88CB, crcb);

                    statu0 = zx_read8(adapter->mmio + 0x88C4);
                    statu1 = zx_read8(adapter->mmio + 0x88C5);
                    statu2 = zx_read8(adapter->mmio + 0x88C6);
                    statu3 = zx_read8(adapter->mmio + 0x88C7);

                    zx_vsprintf(debug_bus_buffer,"%08x: %08x\n", j, (statu0<<24 | statu1<<16 | statu2<<8 | statu3));
                    zx_printf(p, debug_bus_buffer);

                    if(adapter->ctl_flags.dump_hang_info_to_file)
                    {
                        util_write_to_file(debug_bus_buffer, zx_strlen(debug_bus_buffer), "", HW_HANG_LOG_FILE);
                    }
                }
            }
            else if(debug_bus_info_CHX004[i].scope == CHIP3_SCOPE_DEBUG_BUS)
            {
                zx_vsprintf(debug_bus_buffer, "-----------------%s----------------\n", debug_bus_info_CHX004[i].group_name);
                zx_printf(p, debug_bus_buffer);

                if(adapter->ctl_flags.dump_hang_info_to_file)
                {
                    util_write_to_file(debug_bus_buffer, zx_strlen(debug_bus_buffer) , "", HW_HANG_LOG_FILE);
                }

                for(j=debug_bus_info_CHX004[i].group_start_offset; j<=debug_bus_info_CHX004[i].group_end_offset; j++)
                {
                    crcc = (j & 0x3f00)>>8;
                    crcd = j & 0x00ff;
                    zx_write8(adapter->mmio+ 0x88CC, crcc);
                    zx_write8(adapter->mmio+ 0x88CD, crcd);
                    statu0 = zx_read8(adapter->mmio + 0x88FC);
                    statu1 = zx_read8(adapter->mmio + 0x88FD);
                    statu2 = zx_read8(adapter->mmio + 0x88FE);
                    statu3 = zx_read8(adapter->mmio + 0x88FF);

                    zx_vsprintf(debug_bus_buffer,"%08x: %08x\n", j, (statu0<<24 | statu1<<16 | statu2<<8 | statu3));
                    zx_printf(p, debug_bus_buffer);

                    if(adapter->ctl_flags.dump_hang_info_to_file)
                    {
                        util_write_to_file(debug_bus_buffer, zx_strlen(debug_bus_buffer), "", HW_HANG_LOG_FILE);
                    }
                }
            }
            else if(debug_bus_info_CHX004[i].scope == CHIP4_SCOPE_DEBUG_BUS)
            {
                zx_vsprintf(debug_bus_buffer, "-----------------%s----------------\n", debug_bus_info_CHX004[i].group_name);
                zx_printf(p, debug_bus_buffer);

                if(adapter->ctl_flags.dump_hang_info_to_file)
                {
                    util_write_to_file(debug_bus_buffer, zx_strlen(debug_bus_buffer) , "", HW_HANG_LOG_FILE);
                }

                for(j=debug_bus_info_CHX004[i].group_start_offset; j<=debug_bus_info_CHX004[i].group_end_offset; j++)
                {
                    crce = (j & 0x3f00)>>8;
                    crcf = j & 0x00ff;
                    zx_write8(adapter->mmio+ 0x88CE, crce);
                    zx_write8(adapter->mmio+ 0x88CF, crcf);
                    statu0 = zx_read8(adapter->mmio + 0x88F6);
                    statu1 = zx_read8(adapter->mmio + 0x88F7);
                    statu2 = zx_read8(adapter->mmio + 0x88F8);
                    statu3 = zx_read8(adapter->mmio + 0x88F9);

                    zx_vsprintf(debug_bus_buffer,"%08x: %08x\n", j, (statu0<<24 | statu1<<16 | statu2<<8 | statu3));
                    zx_printf(p, debug_bus_buffer);

                    if(adapter->ctl_flags.dump_hang_info_to_file)
                    {
                        util_write_to_file(debug_bus_buffer, zx_strlen(debug_bus_buffer), "", HW_HANG_LOG_FILE);
                    }
                }
            }
        }
    }
    else if (adapter->chip_id == CHIP_CNE001)
    {
        for(i=0; i<sizeof(debug_bus_info_CNE001)/sizeof(debug_bus_info); i++)
        {
            if(debug_bus_info_CNE001[i].scope == CHIP1_SCOPE_DEBUG_BUS)
            {
                zx_memset(debug_bus_buffer, 0, sizeof(debug_bus_buffer));
                zx_vsprintf(debug_bus_buffer, "-----------------%s----------------\n", debug_bus_info_CNE001[i].group_name);
                PRINT_AND_DUMP_FILE(p, debug_bus_buffer, zx_strlen(debug_bus_buffer), adapter->ctl_flags.dump_hang_info_to_file);

                for(j=debug_bus_info_CNE001[i].group_start_offset; j<=debug_bus_info_CNE001[i].group_end_offset; j++)
                {
                    crc8 = (j & 0xff00)>>8;
                    crc9 = j & 0x00ff;
                    zx_write8(adapter->mmio + 0x88C8, crc8);
                    zx_write8(adapter->mmio + 0x88C9, crc9);

                    statu0 = zx_read8(adapter->mmio + 0x88C0);
                    statu1 = zx_read8(adapter->mmio + 0x88C1);
                    statu2 = zx_read8(adapter->mmio + 0x88C2);
                    statu3 = zx_read8(adapter->mmio + 0x88C3);

                    zx_memset(debug_bus_buffer, 0, sizeof(debug_bus_buffer));
                    zx_vsprintf(debug_bus_buffer,"%08x: %08x\n", j, (statu0<<24 | statu1<<16 | statu2<<8 | statu3));
                    PRINT_AND_DUMP_FILE(p, debug_bus_buffer, zx_strlen(debug_bus_buffer), adapter->ctl_flags.dump_hang_info_to_file);
                }
            }
            else if(debug_bus_info_CNE001[i].scope == CHIP2_SCOPE_DEBUG_BUS)
            {
                zx_memset(debug_bus_buffer, 0, sizeof(debug_bus_buffer));
                zx_vsprintf(debug_bus_buffer, "-----------------%s----------------\n", debug_bus_info_CNE001[i].group_name);
                PRINT_AND_DUMP_FILE(p, debug_bus_buffer, zx_strlen(debug_bus_buffer), adapter->ctl_flags.dump_hang_info_to_file);

                for(j=debug_bus_info_CNE001[i].group_start_offset; j<=debug_bus_info_CNE001[i].group_end_offset; j++)
                {
                    crca = (j & 0xff00)>>8;
                    crcb = j & 0x00ff;
                    zx_write8(adapter->mmio+ 0x88CA, crca);
                    zx_write8(adapter->mmio+ 0x88CB, crcb);

                    statu0 = zx_read8(adapter->mmio + 0x88C4);
                    statu1 = zx_read8(adapter->mmio + 0x88C5);
                    statu2 = zx_read8(adapter->mmio + 0x88C6);
                    statu3 = zx_read8(adapter->mmio + 0x88C7);

                    zx_memset(debug_bus_buffer, 0, sizeof(debug_bus_buffer));
                    zx_vsprintf(debug_bus_buffer,"%08x: %08x\n", j, (statu0<<24 | statu1<<16 | statu2<<8 | statu3));
                    PRINT_AND_DUMP_FILE(p, debug_bus_buffer, zx_strlen(debug_bus_buffer), adapter->ctl_flags.dump_hang_info_to_file);
                }
            }
        }
    }

    zx_memset(debug_bus_buffer, 0, sizeof(debug_bus_buffer));
    zx_vsprintf(debug_bus_buffer, "~~~~~~~~~~~~~~end of debug bus info~~~~~~~~~~~~~~");
    PRINT_AND_DUMP_FILE(p, debug_bus_buffer, zx_strlen(debug_bus_buffer), adapter->ctl_flags.dump_hang_info_to_file);

    {
        unsigned long dump_cc = 0;
        unsigned int dump_i = 0;
        zx_memset(debug_bus_buffer, 0, sizeof(debug_bus_buffer));
        zx_vsprintf(debug_bus_buffer, "~~~~~~~~~mmio VPP  info~~~~~~~~~~~~~~\n");
        PRINT_AND_DUMP_FILE(p, debug_bus_buffer, zx_strlen(debug_bus_buffer), adapter->ctl_flags.dump_hang_info_to_file);

        for (dump_i = 0; dump_i < 0xFFF; )
        {
            dump_cc = zx_read32(adapter->mmio +  0x4b000 + dump_i);

            zx_memset(debug_bus_buffer, 0, sizeof(debug_bus_buffer));
            zx_vsprintf(debug_bus_buffer,"%08x: %08x\n",  (adapter->mmio + 0x4b000 + dump_i), dump_cc);
            PRINT_AND_DUMP_FILE(p, debug_bus_buffer, zx_strlen(debug_bus_buffer), adapter->ctl_flags.dump_hang_info_to_file);
            dump_i = dump_i + 4;
        }
        if(adapter->chip_id == CHIP_CNE001)
        {
            for (dump_i = 0; dump_i < 0xFFF; )
            {
                dump_cc = zx_read32(adapter->mmio +  0x4f000 + dump_i);

                zx_memset(debug_bus_buffer, 0, sizeof(debug_bus_buffer));
                zx_vsprintf(debug_bus_buffer,"%08x: %08x\n",  (adapter->mmio + 0x4f000 + dump_i), dump_cc);
                PRINT_AND_DUMP_FILE(p, debug_bus_buffer, zx_strlen(debug_bus_buffer), adapter->ctl_flags.dump_hang_info_to_file);
                dump_i = dump_i + 4;
            }
        }

        zx_memset(debug_bus_buffer, 0, sizeof(debug_bus_buffer));
        zx_vsprintf(debug_bus_buffer, "~~~~~~~~~end of mmio VPP info~~~~~~~~~~~~~~\n");
        PRINT_AND_DUMP_FILE(p, debug_bus_buffer, zx_strlen(debug_bus_buffer), adapter->ctl_flags.dump_hang_info_to_file);
    }
}

void vidsch_dump_engine_info_e3k(vidsch_mgr_t *sch_mgr)
{
    adapter_t        *adapter                 = sch_mgr->adapter;
    engine_e3k_t     *engine                  = sch_mgr->private_data;
    unsigned int     engine_index             = sch_mgr->engine_index;
    unsigned int     RegRbOffset              = EngineRbOffset(adapter->chip_id, engine_index);
    unsigned int     engine_status_offset     = MMIO_CSP_START_ADDRESS + Reg_Block_Busy_Bits_Top_Offset*4;
    unsigned int     cur_rb_head_offset       = MMIO_CSP_START_ADDRESS + (RegRbOffset + 2)*4;
    unsigned int     cur_rb_tail_offset       = MMIO_CSP_START_ADDRESS + (RegRbOffset + 3)*4;
    unsigned int     cur_rb_cmd_offset        = MMIO_CSP_START_ADDRESS + (Reg_Cur_3d_Rbuf_Cmd_Offset + engine_index)*4;
    unsigned int     cur_l1_dma_cmd_offset    = MMIO_CSP_START_ADDRESS + Reg_Cur_L1_Dma_Cmd_Offset*4;
    unsigned int     cur_l2_dma_cmd_offset    = MMIO_CSP_START_ADDRESS + Reg_Cur_L2_Dma_Cmd_Offset*4;
    unsigned int     cur_rb_cmd               = zx_read32(adapter->mmio + cur_rb_cmd_offset);
    unsigned int     cur_rb_head              = zx_read32(adapter->mmio + cur_rb_head_offset);
    unsigned int     cur_rb_tail              = zx_read32(adapter->mmio + cur_rb_tail_offset);
    unsigned int     cur_l1_dma_cmd           = zx_read32(adapter->mmio + cur_l1_dma_cmd_offset);
    unsigned int     cur_l2_dma_cmd           = zx_read32(adapter->mmio + cur_l2_dma_cmd_offset);
    unsigned long long fence_in_mem           = *(volatile unsigned long long *)engine->fence_buffer_cpu_va;
    EngineSatus_e3k  *engine_status           = &engine->dumped_engine_status;
    unsigned int      *status = (unsigned int *)engine_status;
    unsigned int      *video_engine_status = (unsigned int *)&engine_status->ES_VCP_VPP;
    int i;

    for( i = 0; i <7; i++)
    {
        *(status + i) = zx_read32(adapter->mmio + MMIO_CSP_START_ADDRESS + (Reg_Block_Busy_Bits_Top_Offset + i)*4);
    }
    *video_engine_status = zx_read32(adapter->mmio + MMIO_CSP_START_ADDRESS + Reg_Vcp_Vpp_Block_Busy_Bits_Offset*4);

    zx_info("engine:%d, last send fence:0x%llx, returned fence:0x%llx, fence in mem:0x%llx\n", engine_index,
        sch_mgr->last_send_fence_id, sch_mgr->returned_fence_id, fence_in_mem);
    zx_info("chip layer last_send_fence_id:0x%llx, last_process_id:%d\n",
        engine->last_send_fence_id, engine->last_process_id);
    zx_info("current rb  cmd(0x%x): 0x%08x, rb head(0x%x): 0x%08x, rb tail(0x%x): 0x%08x\n",
        cur_rb_cmd_offset, cur_rb_cmd, cur_rb_head_offset, cur_rb_head, cur_rb_tail_offset, cur_rb_tail);
    zx_info("current L1 cmd(%x):%x, L2 cmd(%x):%x\n", cur_l1_dma_cmd_offset, cur_l1_dma_cmd, cur_l2_dma_cmd_offset, cur_l2_dma_cmd);

    zx_info("<<----^^^^^^^^^-----[dump hang]Current HW Hang Status-----^^^^^^^^^^^--->>\n");
    zx_info("ChipSliceMask: %08x\n", adapter->hw_caps.chip_slice_mask);
    zx_info("Engine Status: %08x,%08x,%08x,%08x,%08x,%08x,%08x,VCP0 = %x,VCP1 = %x,VPP = %x.\n",
        engine_status->Top.uint, engine_status->Gpc0_0.uint, engine_status->Gpc0_1.uint,
        engine_status->Gpc1_0.uint, engine_status->Gpc1_1.uint, engine_status->Gpc2_0.uint, engine_status->Gpc2_1.uint,
        ((*video_engine_status)&0x1), ((*video_engine_status)&0x2), ((*video_engine_status)&0x4));
}

void vidsch_dump_hang_info_e3k(vidsch_mgr_t *sch_mgr)
{
    adapter_t        *adapter                 = sch_mgr->adapter;
    engine_e3k_t     *engine                  = sch_mgr->private_data;
    unsigned int     engine_index             = sch_mgr->engine_index;
    unsigned int     RegRbOffset              = EngineRbOffset(adapter->chip_id, engine_index);
    unsigned int     engine_status_offset     = MMIO_CSP_START_ADDRESS + Reg_Block_Busy_Bits_Top_Offset*4;
    unsigned int     cur_rb_head_offset       = MMIO_CSP_START_ADDRESS + (RegRbOffset + 2)*4;
    unsigned int     cur_rb_tail_offset       = MMIO_CSP_START_ADDRESS + (RegRbOffset + 3)*4;
    unsigned int     cur_rb_cmd_offset        = MMIO_CSP_START_ADDRESS + (Reg_Cur_3d_Rbuf_Cmd_Offset + engine_index)*4;
    unsigned int     cur_l1_dma_cmd_offset    = MMIO_CSP_START_ADDRESS + Reg_Cur_L1_Dma_Cmd_Offset*4;
    unsigned int     cur_l2_dma_cmd_offset    = MMIO_CSP_START_ADDRESS + Reg_Cur_L2_Dma_Cmd_Offset*4;
    unsigned int     cur_rb_cmd               = zx_read32(adapter->mmio + cur_rb_cmd_offset);
    unsigned int     cur_rb_head              = zx_read32(adapter->mmio + cur_rb_head_offset);
    unsigned int     cur_rb_tail              = zx_read32(adapter->mmio + cur_rb_tail_offset);
    unsigned int     cur_l1_dma_cmd           = zx_read32(adapter->mmio + cur_l1_dma_cmd_offset);
    unsigned int     cur_l2_dma_cmd           = zx_read32(adapter->mmio + cur_l2_dma_cmd_offset);
    unsigned long long fence_in_mem           = *(volatile unsigned long long *)engine->fence_buffer_cpu_va;
    EngineSatus_e3k    *engine_status         = &engine->dumped_engine_status;
    unsigned int       *status                = (unsigned int *)engine_status;
    unsigned int       *video_engine_status   = &engine_status->ES_VCP_VPP.uint;
    unsigned int       busy_status_offset     = Reg_Vcp_Vpp_Block_Busy_Bits_Offset;
    int i;
    unsigned char  status_buffer[256];

    if(adapter->chip_id == CHIP_CNE001)
    {
        busy_status_offset = Reg_Vcp_Vpp_Block_Busy_Bits_Offset_CNE001;
    }

    if(fence_in_mem == sch_mgr->last_send_fence_id)
    {
        zx_info("%s: fake hang.", __func__);
        return;
    }

    for( i = 0; i <7; i++)
    {
        *(status + i) = zx_read32(adapter->mmio + MMIO_CSP_START_ADDRESS + (Reg_Block_Busy_Bits_Top_Offset + i)*4);
    }
    *video_engine_status = zx_read32(adapter->mmio + MMIO_CSP_START_ADDRESS + busy_status_offset*4);

    zx_memset(status_buffer, 0, sizeof(status_buffer));
    zx_vsprintf(status_buffer, "engine:%d, last send fence:%lld, returned fence:%lld, fence in mem:%lld\n", engine_index,
        sch_mgr->last_send_fence_id, sch_mgr->returned_fence_id, fence_in_mem);
    PRINT_AND_DUMP_FILE(NULL, status_buffer, zx_strlen(status_buffer), adapter->ctl_flags.dump_hang_info_to_file);

    zx_memset(status_buffer, 0, sizeof(status_buffer));
    zx_vsprintf(status_buffer, "current rb  cmd(0x%x): 0x%08x, rb head(0x%x): 0x%08x, rb tail(0x%x): 0x%08x\n",
        cur_rb_cmd_offset, cur_rb_cmd, cur_rb_head_offset, cur_rb_head, cur_rb_tail_offset, cur_rb_tail);
    PRINT_AND_DUMP_FILE(NULL, status_buffer, zx_strlen(status_buffer), adapter->ctl_flags.dump_hang_info_to_file);

    zx_memset(status_buffer, 0, sizeof(status_buffer));
    zx_vsprintf(status_buffer, "current L1 cmd(%x):%x, L2 cmd(%x):%x\n", cur_l1_dma_cmd_offset, cur_l1_dma_cmd, cur_l2_dma_cmd_offset, cur_l2_dma_cmd);
    PRINT_AND_DUMP_FILE(NULL, status_buffer, zx_strlen(status_buffer), adapter->ctl_flags.dump_hang_info_to_file);

    /* dump to file */
    if(0)
    {
        util_dump_memory_to_file(&engine_status_offset,          4, "Engine status offset: ",           HW_HANG_LOG_FILE);
        util_dump_memory_to_file(&engine_status,                28, "Engine status: ",                  HW_HANG_LOG_FILE);
        util_dump_memory_to_file(&engine_index,                  4, "Engine index: ",                   HW_HANG_LOG_FILE);
        util_dump_memory_to_file(&fence_in_mem,                  4, "current returned fence: ",         HW_HANG_LOG_FILE);
        util_dump_memory_to_file(&sch_mgr->last_send_fence_id,   4, "last send fence: ",                HW_HANG_LOG_FILE);
        util_dump_memory_to_file(&cur_rb_cmd_offset,             4, "current rb cmd offset: ",          HW_HANG_LOG_FILE);
        util_dump_memory_to_file(&cur_rb_cmd,                    4, "current rb cmd: ",                 HW_HANG_LOG_FILE);
        util_dump_memory_to_file(&cur_rb_head_offset,            4, "current rb head offset: ",         HW_HANG_LOG_FILE);
        util_dump_memory_to_file(&cur_rb_head,                   4, "current rb head: ",                HW_HANG_LOG_FILE);
        util_dump_memory_to_file(&cur_rb_tail_offset,            4, "current rb tail offset: ",         HW_HANG_LOG_FILE);
        util_dump_memory_to_file(&cur_rb_tail,                   4, "current rb tail: ",                HW_HANG_LOG_FILE);
        util_dump_memory_to_file(&cur_l1_dma_cmd_offset,         4, "current l1 dma cmd offset: ",      HW_HANG_LOG_FILE);
        util_dump_memory_to_file(&cur_l2_dma_cmd_offset,         4, "current l2 dma cmd offset: ",      HW_HANG_LOG_FILE);
        util_dump_memory_to_file(&cur_l1_dma_cmd,                4, "current l1 dma cmd: ",             HW_HANG_LOG_FILE);
        util_dump_memory_to_file(&cur_l2_dma_cmd,                4, "current l2 dma cmd: ",             HW_HANG_LOG_FILE);
        util_dump_memory_to_file(engine->ring_buf_virt_addr, RING_BUFFER_SIZE_E3K, "RING buffer", HW_HANG_LOG_FILE);
    }

    zx_memset(status_buffer, 0, sizeof(status_buffer));
    zx_vsprintf(status_buffer, "<<----^^^^^^^^^-----[dump hang]Current HW Hang Status-----^^^^^^^^^^^--->>\n");
    PRINT_AND_DUMP_FILE(NULL, status_buffer, zx_strlen(status_buffer), adapter->ctl_flags.dump_hang_info_to_file);

    zx_memset(status_buffer, 0, sizeof(status_buffer));
    zx_vsprintf(status_buffer, "ChipSliceMask: %08x\n", adapter->hw_caps.chip_slice_mask);
    PRINT_AND_DUMP_FILE(NULL, status_buffer, zx_strlen(status_buffer), adapter->ctl_flags.dump_hang_info_to_file);

    if (adapter->pm_caps.pwm_mode)
    {
        zx_memset(status_buffer, 0, sizeof(status_buffer));
        if (adapter->chip_id == CHIP_CHX004)
        {
            zx_vsprintf(status_buffer, "Power Status: %08x\n",
                zx_read32(adapter->mmio + MMIO_CSP_START_ADDRESS + Reg_Pwr_Mgr_Status1_Offset * 4));
        }
        else if (adapter->chip_id == CHIP_CNE001)
        {
            zx_vsprintf(status_buffer, "Power Status0: %08x\n",
                zx_read32(adapter->mmio + MMIO_CSP_START_ADDRESS + Reg_Pwr_Mgr_Status0_Offset_Cne001 * 4));
            zx_vsprintf(status_buffer+zx_strlen(status_buffer), "Power Status1: %08x\n",
                zx_read32(adapter->mmio + MMIO_CSP_START_ADDRESS + Reg_Pwr_Mgr_Status1_Offset_Cne001 * 4));
            zx_vsprintf(status_buffer+zx_strlen(status_buffer), "Power Status2: %08x\n",
                zx_read32(adapter->mmio + MMIO_CSP_START_ADDRESS + Reg_Pwr_Mgr_Status2_Offset * 4));
        }
        PRINT_AND_DUMP_FILE(NULL, status_buffer, zx_strlen(status_buffer), adapter->ctl_flags.dump_hang_info_to_file);
    }

    zx_memset(status_buffer, 0, sizeof(status_buffer));
    if(adapter->chip_id == CHIP_CNE001)
    {
        zx_vsprintf(status_buffer, "Engine Hang and Status: %08x,%08x,%08x,%08x,%08x,%08x,%08x,VCP0 = %x,VCP1 = %x,VCP2 = %x,VPP0 = %x,VPP1 = %x.\n",
                    engine_status->Top.uint, engine_status->Gpc0_0.uint, engine_status->Gpc0_1.uint,
                    engine_status->Gpc1_0.uint, engine_status->Gpc1_1.uint, engine_status->Gpc2_0.uint, engine_status->Gpc2_1.uint,
                    engine_status->ES_VCP_VPP.reg.Vcp0_Busy, engine_status->ES_VCP_VPP.reg.Vcp1_Busy, engine_status->ES_VCP_VPP.reg.Vcp2_Busy,
                    engine_status->ES_VCP_VPP.reg.Vpp0_Busy, engine_status->ES_VCP_VPP.reg.Vpp1_Busy);
    }else if(adapter->chip_id == CHIP_CHX004){
        zx_vsprintf(status_buffer, "Engine Hang and Status: %08x,%08x,%08x,%08x,%08x,%08x,%08x,VCP0 = %x,VCP1 = %x,VPP = %x.\n",
                    engine_status->Top.uint, engine_status->Gpc0_0.uint, engine_status->Gpc0_1.uint,
                    engine_status->Gpc1_0.uint, engine_status->Gpc1_1.uint, engine_status->Gpc2_0.uint, engine_status->Gpc2_1.uint,
                    engine_status->ES_VCP_VPP.reg.Vcp0_Busy, engine_status->ES_VCP_VPP.reg.Vcp1_Busy, engine_status->ES_VCP_VPP.reg.Vcp2_Busy);
    }
    PRINT_AND_DUMP_FILE(NULL, status_buffer, zx_strlen(status_buffer), adapter->ctl_flags.dump_hang_info_to_file);

    /*dump debug bus*/
    if(adapter->display_debugbus_flag)
    {
        vidsch_display_debugbus_info_e3k(adapter, NULL, (engine_index >= RB_INDEX_VCP0));
    }
}

#define MIN(a,b)  ((a<b) ? a : b)

typedef enum dump_data_type{
    HANG_DUMP_NONE,
    HANG_DUMP_RING_BUFFER,
    HANG_DUMP_COMMON_INFO,

    HANG_DUMP_FENCE_ADDR,
    HANG_DUMP_FLUSH_FIFO_BUFFER,
    HANG_DUMP_BL_BUFFER,
} dump_data_type;

static unsigned long long _get_dump_data_gpu_va(dup_hang_ctx_t *dup_hang_ctx, int type, int index)
{
    unsigned long long offset = 0;

    switch(type)
    {
        case HANG_DUMP_RING_BUFFER:
            offset = dup_hang_ctx->ring_buffer_offset + index * dup_hang_ctx->single_ring_buffer_size_in_byte;
            break;

        case HANG_DUMP_COMMON_INFO:
            offset = dup_hang_ctx->ring_buffer_offset + MAX_HANG_DUMP_DATA_POOL_NUMBER * dup_hang_ctx->single_ring_buffer_size_in_byte;
            break;

        case HANG_DUMP_FENCE_ADDR:
            {
                unsigned long long common_info_offset = dup_hang_ctx->ring_buffer_offset + MAX_HANG_DUMP_DATA_POOL_NUMBER * dup_hang_ctx->single_ring_buffer_size_in_byte;
                dh_common_info_e3k *common_info = (dh_common_info_e3k*)(dup_hang_ctx->local_visible_memory_cpu_va + common_info_offset);

                offset = common_info->rb_info[index].fence_gpu_va;
            }
            break;

        case HANG_DUMP_FLUSH_FIFO_BUFFER:
            {
                unsigned long long common_info_offset = dup_hang_ctx->ring_buffer_offset + MAX_HANG_DUMP_DATA_POOL_NUMBER * dup_hang_ctx->single_ring_buffer_size_in_byte;

                offset =  common_info_offset + OFFSETOF(dh_common_info_e3k, flush_fifo[0])/sizeof(DWORD);
            }
            break;
        default:
            zx_assert(0, "invalid type");
    }

    return offset;
}

static void* _get_dump_data_cpu_va(dup_hang_ctx_t *dup_hang_ctx, int type, int index)
{
    adapter_t          *adapter = dup_hang_ctx->adapter;
    void               *virt_addr = 0;
    unsigned long long  gpu_va = 0;

    switch(type)
    {
        case HANG_DUMP_RING_BUFFER:
        case HANG_DUMP_COMMON_INFO:
        case HANG_DUMP_FLUSH_FIFO_BUFFER:
        case HANG_DUMP_BL_BUFFER:
            gpu_va = _get_dump_data_gpu_va(dup_hang_ctx, type, index);
            virt_addr = dup_hang_ctx->local_visible_memory_cpu_va + gpu_va;
            break;
        case HANG_DUMP_FENCE_ADDR:
            zx_assert(0, "should not get fence cpu addr like this.\n");
            break;
        default:
            zx_assert(0, "invalid type");
            break;
    }

    return virt_addr;
}

void vidschi_copy_mem_e3k(adapter_t *adapter, unsigned long long dst_phys_addr,void *dst_virt_addr,unsigned long long src_phys_addr, void *src_virt_addr,unsigned int total_size)
{
    unsigned int       RbIndex  = RB_INDEX_GFXL;
    vidsch_mgr_t       *sch_mgr = adapter->sch_mgr[RbIndex];
    engine_share_e3k_t *share   = adapter->private_data;
    engine_e3k_t       *engine  = sch_mgr->private_data;

    Hw_Surf_Format              Format  = HSF_R8G8B8A8_UNORM;
    DWORD                       Width   = MAX_WIDTH_2DBLT;
    DWORD                       Height  = 0;
    DWORD                       Length  = total_size;
    unsigned long long          oldFence= 0;
    DWORD                       i       = 0;
    DWORD                       StartX  = 0;
    DWORD                       CmdLength;
    DWORD                       AddrOffset = 0;
    static int                  bHwCopyEn  = TRUE;
    DWORD                       dwRealRBSize  = 0;
    DWORD                       dwAlignRBSize = 0;
    unsigned long long          Count   = 0;
    unsigned long long          DstAddr = dst_phys_addr;
    unsigned long long          SrcAddr = src_phys_addr;


    Cmd_Blk_Cmd_Csp_Indicator           pwmTrigger_on = {0};
    Cmd_Blk_Cmd_Csp_Indicator           pwmTrigger_off = {0};

    Cmd_Blk_Cmd_Csp_Indicator_Dword1    trigger_on_Dw = {0};
    Cmd_Blk_Cmd_Csp_Indicator_Dword1    trigger_off_Dw = {0};

    Csp_Opcodes_cmd                     cmd        = {0};
    BITBLT_REGSETTING_E3K       CopyCmd   = {0};
    BITBLT_REGSETTING_E3K*      p2DBltCmd = &CopyCmd;
    DWORD*                      pRB;
    DWORD*                      pRB0;
    int                         j;

    zx_assert(total_size % 4 == 0, "total_size:%d", total_size);

    // SrcAddr and DstAddr should be aligned to 2kbit.
    zx_assert((SrcAddr & ~0xFFll, NULL) == 0, "SrcAddr:%llx", SrcAddr);
    zx_assert((DstAddr & ~0xFFll, NULL) == 0, "DstAddr:%llx", DstAddr);

    zx_memcpy(p2DBltCmd, &share->_2dblt_cmd_e3k, sizeof(BITBLT_REGSETTING_E3K));

    if(!adapter->hw_caps.local_only)
    {
     //   enginei_invalidate_gart_e3k(engine);
    }

    if (dst_virt_addr && src_virt_addr)
    {
    //    bHwCopyEn = 0;
    }
    // HW copy
    if (bHwCopyEn)
    {
        pwmTrigger_on.Major_Opcode         = CSP_OPCODE_Blk_Cmd_Csp_Indicator;
        pwmTrigger_on.Block_Id             = CSP_GLOBAL_BLOCK;
        pwmTrigger_on.Type                 = BLOCK_COMMAND_CSP_TYPE_INDICATOR;
        pwmTrigger_on.Info                 = BLK_CMD_CSP_INDICATOR_INFO_3D_MODE;
        pwmTrigger_on.Dwc                  = 1;
        trigger_on_Dw.Slice_Mask           = adapter->hw_caps.chip_slice_mask;

        pwmTrigger_off.Major_Opcode         = CSP_OPCODE_Blk_Cmd_Csp_Indicator;
        pwmTrigger_off.Block_Id             = CSP_GLOBAL_BLOCK;
        pwmTrigger_off.Type                 = BLOCK_COMMAND_CSP_TYPE_INDICATOR;
        pwmTrigger_off.Info                 = BLK_CMD_CSP_INDICATOR_INFO_OFF_MODE;
        pwmTrigger_off.Dwc                  = 1;
        trigger_off_Dw.Slice_Mask           = adapter->hw_caps.chip_slice_mask;

        cmd.cmd_Tbr_Indicator.Skip_En   = 1;
        cmd.cmd_Tbr_Indicator.Block_Id  = TASBE_BLOCK;
        cmd.cmd_Tbr_Indicator.Major_Opcode = 0xF;

        // save fence need more check
        oldFence = *(unsigned long long*)(engine->fence_buffer_cpu_va + DH_FENCE_OFFSET);
        *(engine->fence_buffer_cpu_va + DH_FENCE_OFFSET) = 0;

        while(Length)
        {
            Width = MAX_WIDTH_2DBLT;
            Height = Length/(Width << 2);
            Height = Height > MAX_HEIGHT_2DBLT ? MAX_HEIGHT_2DBLT : Height;

            if(Length < (MAX_WIDTH_2DBLT << 2))
            {
                Width  = Length >> 2;
                Height = 1;
            }

            StartX     = AddrOffset & 0xFF;
            AddrOffset = AddrOffset - StartX;

            p2DBltCmd->SrcAddr.reg.Base_Addr = (SrcAddr + AddrOffset) >> 8;
            p2DBltCmd->DstAddr.reg.Base_Addr = (DstAddr + AddrOffset) >> 8;

            p2DBltCmd->SrcFormat.reg.Format         = Format;
            p2DBltCmd->SrcFormat.reg.Bl_Slot_Idx    = 0;
            p2DBltCmd->SrcSize.reg.Width            = Width;
            p2DBltCmd->SrcSize.reg.Height           = Height;
            p2DBltCmd->SrcDepth.reg.Depth_Or_Arraysize = 1;
            p2DBltCmd->SrcDepth.reg.Range_Type      = 0;
            if (adapter->chip_id == CHIP_CHX004)
            {
                p2DBltCmd->SrcViewCtrl.reg_CHX004.Arraysize    = 1;
                p2DBltCmd->SrcViewCtrl.reg_CHX004.First_Array_Idx = 0;
            }
            else
            {
                p2DBltCmd->SrcViewCtrl.reg.Arraysize    = 1;
                p2DBltCmd->SrcViewCtrl.reg.First_Array_Idx = 0;
            }

            p2DBltCmd->DstFormat    = p2DBltCmd->SrcFormat;
            p2DBltCmd->DstSize      = p2DBltCmd->SrcSize;
            p2DBltCmd->DstDepth     = p2DBltCmd->SrcDepth;
            p2DBltCmd->DstViewCtrl  = p2DBltCmd->SrcViewCtrl;

            p2DBltCmd->SrcMisc.reg.Is_Tiling        = 0;
            p2DBltCmd->SrcMisc.reg.Rt_Enable        = 1;
            p2DBltCmd->SrcMisc.reg.Resource_Type    = RT_MISC_RESOURCE_TYPE_2D_TEXTURE;

            p2DBltCmd->DstMisc.reg.Is_Tiling        = 0;
            p2DBltCmd->DstMisc.reg.Rt_Enable        = 1;
            p2DBltCmd->DstMisc.reg.Rt_Write_Mask    = 0xF;
            p2DBltCmd->DstMisc.reg.Resource_Type    = RT_MISC_RESOURCE_TYPE_2D_TEXTURE;

            p2DBltCmd->Dzs_Ctrl.reg.Src_Read_Alloc_En = 0;

            p2DBltCmd->RectX.Xmin = StartX >> 2;//in pixel
            p2DBltCmd->RectX.Xmax = Width - 1;
            p2DBltCmd->RectY.Ymin = 0;
            p2DBltCmd->RectY.Ymax = Height - 1;
            p2DBltCmd->SrcDxDy.Dx = 0;
            p2DBltCmd->SrcDxDy.Dy = 0;

            p2DBltCmd->D_Flush.Type                  = BLOCK_COMMAND_TEMPLATE_TYPE_FLUSH;
            p2DBltCmd->D_Flush.Target                = BLOCK_COMMAND_FLUSH_TARGET_D_C;
            p2DBltCmd->D_Flush.Block_Id              = FF_BLOCK;
            p2DBltCmd->D_Flush.Major_Opcode          = CSP_OPCODE_Block_Command_Flush;

            p2DBltCmd->D_Invalidate.Type             = BLOCK_COMMAND_TEMPLATE_TYPE_INVALIDATE_CACHE;
            p2DBltCmd->D_Invalidate.Target           = BLOCK_COMMAND_FLUSH_TARGET_D_C;
            p2DBltCmd->D_Invalidate.Block_Id         = FF_BLOCK;
            p2DBltCmd->D_Invalidate.Major_Opcode     = CSP_OPCODE_Block_Command_Flush;

            p2DBltCmd->FenceCmd.uint = SEND_INTERNAL_WRITEFENCE_E3K(FENCE_ROUTE_ID_D_FENCE,_RBT_3DBE, HWM_SYNC_KMD_SLOT);
            p2DBltCmd->FenceValue = SEND_INTERNAL_FENCE_VALUE_E3K(FENCE_VALUE_CACHE_DMA_DFENCE);
            p2DBltCmd->WaitCmd.uint = SEND_INTERNAL_WAIT_E3K(HWM_SYNC_KMD_SLOT, WAIT_METHOD_EQUAL, FENCE_VALUE_CACHE_DMA_DFENCE);
            p2DBltCmd->WaitMainCmd.uint = SEND_INTERNAL_WAIT_MAIN_E3K(HWM_SYNC_KMD_SLOT, WAIT_METHOD_EQUAL, FENCE_VALUE_CACHE_DMA_DFENCE);

            CmdLength = sizeof(BITBLT_REGSETTING_E3K)/sizeof(DWORD);
            CmdLength = util_align(CmdLength + 5 + 6 + 8 + POWER_SKIP_COMMAND_SIZE,16);

            pRB = enginei_get_ring_buffer_space_e3k(engine, CmdLength);
            pRB0 = pRB;

            zx_memset(pRB, 0, CmdLength << 2);

            *pRB++ = *(DWORD*)&pwmTrigger_on;
            *pRB++ = *(DWORD*)&trigger_on_Dw;
            pRB += 8;  // skip 8dw for hw block sync

            zx_memcpy(pRB, p2DBltCmd, sizeof(BITBLT_REGSETTING_E3K));
            pRB =pRB + sizeof(BITBLT_REGSETTING_E3K)/sizeof(DWORD);

            *pRB++ = SEND_EXTERNAL_FENCE_E3K(FENCE_IRQ_INTERRUPT_CPU);
            *pRB++ = (engine->fence_buffer_gpu_va + DH_FENCE_OFFSET * sizeof(unsigned long long)) & 0xFFFFFFFF;
            *pRB++ = ((engine->fence_buffer_gpu_va + DH_FENCE_OFFSET * sizeof(unsigned long long)) >> 32 )& 0xFF;

            *pRB++ = (DH_FENCE_VALUE + i)&0xFFFFFFFF;
            *pRB++ = (((unsigned long long)(DH_FENCE_VALUE + i)) >>32) &0xFFFFFFFF;

            for (j = 0; j < POWER_SKIP_COMMAND_SIZE; j++)
            {
                *pRB++  = 0;
            }

            *pRB++ = *(DWORD*)&pwmTrigger_off;
            *pRB++ = *(DWORD*)&trigger_off_Dw;

            //to ensure cmd write in mem
            util_crc32((unsigned char*)pRB0, CmdLength << 2);

            enginei_release_ring_buffer_space_e3k(engine, CmdLength << 2);

            while((*(unsigned long long*)(engine->fence_buffer_cpu_va + DH_FENCE_OFFSET) + 2)<= (DH_FENCE_VALUE + i))//max kick 2 dma to hw
            {
                if(++Count < 0x10)
                {
                     zx_msleep(1000);
                }
                else
                {
                    zx_info("%s, i:%d, fence not back, expect:%llx, fence in mem:%llx, hw queue dma num:%d\n",__func__, i, (DH_FENCE_VALUE + i), *(unsigned long long*)(engine->fence_buffer_cpu_va + DH_FENCE_OFFSET), 2);
                    Count = 0;
                }
            }

            i++;

            AddrOffset +=((Width * Height) << 2);
            Length -=((Width * Height) << 2) + StartX;
        }

        Count = 0;

        while(*(unsigned long long*)(engine->fence_buffer_cpu_va + DH_FENCE_OFFSET) != (DH_FENCE_VALUE + i -1))
        {
            if(++Count < 1000)
            {
                 zx_msleep(10);
            }
            else
            {
                zx_info("%s, i:%d, fence not back, expect:%llx, fence in mem:%llx, out of loop\n",__func__, i, (DH_FENCE_VALUE + i), *(unsigned long long*)(engine->fence_buffer_cpu_va + DH_FENCE_OFFSET));
                Count = 0;
            }
        }

        // restore fence
        *(unsigned long long*)(engine->fence_buffer_cpu_va + DH_FENCE_OFFSET) = oldFence;
    }
    else if(src_virt_addr !=NULL && dst_virt_addr != NULL)
    {
        zx_memcpy(dst_virt_addr, src_virt_addr, total_size);
    }
    else
    {
        zx_assert(0, "hw copy not enable, VA is null");
    }
}

#define WRITE_DUMP_FILE_CHECK(request_size, returned_size) do { \
    if((returned_size) != (request_size)) \
    { \
        zx_error("write error @%d. request: %d, actual: %d\n", __LINE__, (request_size), (returned_size)); \
    }\
    else \
    { \
        zx_info("write file ok, len: 0x%x\n", (returned_size)); \
    } } while(0)


static void _fill_and_write_file_header(adapter_t *adapter, struct os_file *file, unsigned long *ppos)
{
    int len;
    HangDumpFileHeader file_header_info = {0, };
    engine_share_e3k_t *share           = adapter->private_data;

    file_header_info.nHeaderVersion = 0x0002;
    file_header_info.nChipID = adapter->chip_id;
    file_header_info.nDeviceID = adapter->bus_config.device_id;
    file_header_info.nSliceMask = adapter->hw_caps.chip_slice_mask;
    file_header_info.nHangDumpFileHeaderSizeInByte = sizeof(HangDumpFileHeader);
    file_header_info.nAdapterMemorySizeInByte = adapter->Real_vram_size;
    file_header_info.nPCIEMemorySizeInByte = 0; // no used for va
    file_header_info.nRecordNums = 6;

    zx_info("%s() verion-0x%x, chip_id-0x%x, slice_mask-0x%x, header_size-%d, local_size-0x%llx, pcie_size-0x%llx\n", __func__,
              file_header_info.nHeaderVersion,
              file_header_info.nChipID,
              file_header_info.nSliceMask,
              file_header_info.nHangDumpFileHeaderSizeInByte,
              file_header_info.nAdapterMemorySizeInByte,
              file_header_info.nPCIEMemorySizeInByte);

    file_header_info.nTtbrBufferOffset = 0;

    file_header_info.lowTopAddress = 0; //adapter->low_top_address;

    file_header_info.nDmaOffset = 0; //unused, share->dma_buffer_for_hang->gpu_va;
    file_header_info.nSingleDmaSizeInByte = 0; // unused, HangDump_SingleDmaBufferSize;

    file_header_info.nRingBufferOffset = share->ring_buffer_for_hang->gpu_va;
    file_header_info.nSingleRingBufferSizeInByte = HangDump_RingBufferBlockSize;

    file_header_info.nContextOffset = 0; // unused, share->context_buffer_for_hang->gpu_va;
    file_header_info.nSingleContextSizeInByte = 0; // unused HangDump_ContextBufferBlockSize;

    file_header_info.nTransferBufferOffsetInFBFile = 0; //share->transfer_buffer_for_hang->gpu_va;

    file_header_info.dummyPageEntry = 0; //adapter->dummy_page_addr;//dummyPageEntry.uint;

    file_header_info.nBlBufferOffset = 0; // unused, share->bl_buffer->gpu_va;

    len = zx_file_write(file, &file_header_info, sizeof(HangDumpFileHeader));
    WRITE_DUMP_FILE_CHECK(sizeof(HangDumpFileHeader), len);
    *ppos += sizeof(HangDumpFileHeader);
}

struct _write_local_param
{
    adapter_t *adapter;
    struct os_file *file;
    unsigned int type;
    unsigned int is_ptes;
    unsigned long *ppos;
    unsigned long long start;
    unsigned long long size;
};

static int _write_local_flush(struct _write_local_param *param)
{
    int len;
    adapter_t *adapter = param->adapter;
    engine_share_e3k_t *share = adapter->private_data;
    struct os_file *file = param->file;
    unsigned int blit_size = 0;
    HangDumpMemoryRangeHeader range_header = {0, };
    unsigned long long  rest_size;
    unsigned long long  src;

    if (param->size == 0)
        return 1;

    // write range header
    range_header.type = param->type;
    if (param->is_ptes)
        range_header.type |= MEMORY_TYPE_PTES;
    range_header.phyAddr = param->start;
    range_header.size = param->size;

    len = zx_file_write(file, &range_header, sizeof(range_header));
    WRITE_DUMP_FILE_CHECK(sizeof(range_header), len);
    zx_info("&&&&&& write %s (phy:0x%llx, size:0x%x, type:0x%x, pos:0x%lx).\n",
        (param->type == MEMORY_TYPE_LOCAL_VISIBLE) ? "cpu visible" : "cpu invisible",
        range_header.phyAddr, param->size, range_header.type, *param->ppos);
    *param->ppos += sizeof(range_header);

    rest_size = param->size;
    src = param->start;
    while(rest_size)
    {
        blit_size = rest_size > E3K_TRANSFERBUFFER_FOR_HANG_SIZE ? E3K_TRANSFERBUFFER_FOR_HANG_SIZE : rest_size;

#ifdef ZX_VMI_MODE
        if (1)
#else
        if (adapter->chip_id >= CHIP_CHX004 && param->type == MEMORY_TYPE_LOCAL_VISIBLE)
#endif
        {
            // go cpu path
            zx_map_argu_t map = {0, };
            zx_cpu_vm_area_t *vma = NULL;

            map.flags.cache_type= ZX_MEM_UNCACHED;
            map.flags.mem_type  = ZX_SYSTEM_IO;
            map.phys_addr       = adapter->vidmm_bus_addr + src;
            map.size            = blit_size;
            map.node_num        = 1;
            vma = zx_map_io_memory(&map);

            if (!vma || !vma->virt_addr)
            {
                zx_info("@@@ map_io_memory failed, goto hw_blit.\n");
                goto hw_blit;
            }

            len = zx_file_write(file, vma->virt_addr, blit_size);
            zx_unmap_io_memory(vma);
        }
        else
        {
hw_blit:
            vidschi_copy_mem_e3k(adapter,
                                 share->transfer_buffer_for_hang->gpu_va,
                                 NULL,
                                 src,
                                 NULL,
                                 blit_size);
            len = zx_file_write(file, share->transfer_buffer_for_hang->krnl_cpu_vma->virt_addr, blit_size);
        }

        WRITE_DUMP_FILE_CHECK(blit_size, len);
        *param->ppos += blit_size;

        src += blit_size;
        rest_size -= blit_size;
    }

    param->start = 0;
    param->size = 0;
    param->is_ptes = 0;

    return 1;
}

static int _write_range_node_to_file(struct range_node *range, void *priv)
{
    struct _write_local_param *param = priv;
    adapter_t *adapter = param->adapter;
    unsigned int type = MEMORY_TYPE_INVALID;

    if (range->start + range->size < adapter->visible_local_memory_size)
    {
        type = MEMORY_TYPE_LOCAL_VISIBLE;
    }
    else if (range->start >= adapter->visible_local_memory_size)
    {
        type = MEMORY_TYPE_LOCAL_INVISIBLE;
    }
retry:
    if (param->size == 0)
    {
        if (type == MEMORY_TYPE_INVALID)
        {
            // this means the range cross cpu_visible & cpu_invisible, split it into two node
            param->start = range->start;
            param->size = adapter->visible_local_memory_size - range->start;
            param->type = MEMORY_TYPE_LOCAL_VISIBLE;
            param->is_ptes = (range->tag == 0x7) ? 1 : 0;
            _write_local_flush(param);

            param->start = adapter->visible_local_memory_size;
            param->size = range->start + range->size - adapter->visible_local_memory_size;
            param->type = MEMORY_TYPE_LOCAL_INVISIBLE;
            param->is_ptes = (range->tag == 0x7) ? 1 : 0;
        }
        else
        {
            param->start = range->start;
            param->size = range->size;
            param->type = type;
            param->is_ptes = (range->tag == 0x7) ? 1 : 0;
        }
    }
    else if ((param->start + param->size != range->start) ||
            (param->is_ptes != !!(range->tag == 0x7)) ||
            (param->type != type))
    {
        _write_local_flush(param);
        goto retry;
    }
    else
    {
        param->size += range->size;
    }

    return 1;
}

static void _write_local_to_file(adapter_t *adapter, struct os_file *file, unsigned long *ppos)
{
    int                 id;
    vidmm_mgr_t         *mm_mgr = adapter->mm_mgr;
    vidmm_segment_t     *segment = NULL;
    struct _write_local_param param = {0, };

    param.file      = file;
    param.adapter   = adapter;
    param.ppos      = ppos;

    for (id = 0; id < mm_mgr->segment_cnt; id++)
    {
        segment = mm_mgr->segment + id;

        if (segment->mem_ra)
        {
            zx_mutex_lock(segment->lock);

            segment->mem_ra->for_each_node(segment->mem_ra,
                    0, -1, &param, _write_range_node_to_file);

            _write_local_flush(&param);

            zx_mutex_unlock(segment->lock);
        }
    }
}

struct _write_pcie_param
{
    adapter_t *adapter;
    struct os_file *file;
    zx_cpu_vm_area_t *vma;
    unsigned long *ppos;
};

static int _write_continues_page_to_file(void *arg, int page_start, int page_cnt, unsigned long long dma_addr)
{
    int len;
    struct _write_pcie_param *param = arg;
    struct os_file *file = param->file;
    HangDumpMemoryRangeHeader range_header = {0, };

    // write range header
    range_header.type = MEMORY_TYPE_PCIE;
    range_header.phyAddr = dma_addr;
    range_header.size = page_cnt << 12;
    len = zx_file_write(file, &range_header, sizeof(range_header));
    zx_assert(len == sizeof(range_header), "");
    zx_info("&&&&&& write pcie (phy:0x%llx, size:0x%x, type:0x%x, pos:0x%lx).\n",
           range_header.phyAddr, range_header.size, range_header.type, *param->ppos);
    *param->ppos += sizeof(range_header);

    len = zx_file_write(file, (char*)param->vma->virt_addr + (page_start << 12), page_cnt << 12);
    WRITE_DUMP_FILE_CHECK(page_cnt << 12, len);
    *param->ppos += (page_cnt << 12);
    return 0;
}

static int _write_pages_memory_to_file(struct os_pages_memory *memory, unsigned int size, void *priv)
{
    struct _write_pcie_param *param = priv;
    zx_map_argu_t map = {0, };

    map.size = size;
    map.flags.mem_type = ZX_SYSTEM_RAM;
    map.memory = memory;
    map.offset = 0;
    map.flags.cache_type = ZX_MEM_WRITE_BACK;
    param->vma = zx_map_pages_memory(&map);
    zx_pages_memory_for_each_continues(memory, param, _write_continues_page_to_file);
    zx_unmap_pages_memory(param->vma);
    param->vma = NULL;

    return 0;
}

static void _write_pcie_to_file(adapter_t *adapter, struct os_file *file, unsigned long *ppos)
{
    struct _write_pcie_param param = {0, };

    param.adapter = adapter;
    param.file = file;
    param.ppos = ppos;
    zx_for_each_pages_memory(_write_pages_memory_to_file, &param);
}

static void _write_memory_to_file(adapter_t *adapter, struct os_file *file, unsigned long *ppos)
{
    int len;
    HangDumpMemoryRangeHeader range_header = {0, };

    _write_local_to_file(adapter, file, ppos);
    _write_pcie_to_file(adapter, file, ppos);

    // end
    range_header.type = MEMORY_TYPE_INVALID;
    len =zx_file_write(file, &range_header, sizeof(range_header));
    WRITE_DUMP_FILE_CHECK(sizeof(range_header), len);
    zx_info("memory End, pos = 0x%lx\n", *ppos);
    *ppos += sizeof(range_header);
}

void vidsch_dump_hang_e3k(adapter_t *adapter)
{
    struct os_file *file;
    unsigned long pos = 0;

    if(adapter->ctl_flags.hang_dump != 2)
    {
        zx_error("vidsch_dump_hang_e3k skip this function, status error...\n");
        return;
    }

    if (debug_mode_e3k.internal_dump_hw)
    {
        zx_error("vidsch_dump_hang_e3k skip this function, status error...\n");
        return;
    }
    debug_mode_e3k.internal_dump_hw = 1;

    file = zx_file_open("/var/fb.FB", OS_RDWR | OS_CREAT | OS_APPEND | OS_LARGEFILE, 0666);
    if (!file)
    {
        zx_info("create file /var/fb.FB fail!\n");
        return;
    }

    _fill_and_write_file_header(adapter, file, &pos);
    _write_memory_to_file(adapter, file, &pos);
    zx_file_close(file);

    zx_info("[DUMP HANG] ................................End dump ................................\n");
#ifndef ZX_VMI_MODE
    while(1)
    {
        zx_info("[DUMP HANG] Dump image saved in /var/fb.FB, you can copy it out and reboot you machine!!!\n");

        zx_msleep(50000);
    }
#endif
}

static void _load_file_header(dup_hang_ctx_t *dup_hang_ctx)
{
    struct os_file    *file = dup_hang_ctx->file;
    adapter_t         *adapter = dup_hang_ctx->adapter;
    HangDumpFileHeader file_header_info = {0};
    unsigned long long len = 0;

    len = zx_file_read(file, &file_header_info, sizeof(file_header_info), NULL);

    zx_assert(len == sizeof(file_header_info), "");
    zx_assert(file_header_info.nHeaderVersion == 0x0002, "head version is wrong");
    zx_assert(file_header_info.nChipID == adapter->chip_id, "chip id is wrong");
    zx_assert(file_header_info.nSliceMask == adapter->hw_caps.chip_slice_mask, "slice_mask is wrong");
    zx_assert(file_header_info.nHangDumpFileHeaderSizeInByte == sizeof(HangDumpFileHeader), "hang dump file header is wrong");
    zx_assert(file_header_info.nAdapterMemorySizeInByte == adapter->Real_vram_size, "local memory size is wrong");
//    zx_assert(file_header_info.nPCIEMemorySizeInByte == adapter->gart_ram_size, "pcie memory size is wrong");

    dup_hang_ctx->chip_id = file_header_info.nChipID;
    dup_hang_ctx->slice_mask = file_header_info.nSliceMask;
    dup_hang_ctx->local_memory_size = file_header_info.nAdapterMemorySizeInByte;
    // pcie_memory_size was dynamic calc when load pcie memory
//    dup_hang_ctx->pcie_memory_size = file_header_info.nPCIEMemorySizeInByte;
    dup_hang_ctx->record_number = file_header_info.nRecordNums;
    dup_hang_ctx->ring_buffer_offset = file_header_info.nRingBufferOffset;
    dup_hang_ctx->single_ring_buffer_size_in_byte = file_header_info.nSingleRingBufferSizeInByte;
    dup_hang_ctx->dummy_page_entry = file_header_info.dummyPageEntry;

    INIT_LIST_HEAD(&dup_hang_ctx->addr_map_list);
    INIT_LIST_HEAD(&dup_hang_ctx->ptes_list);
}

static void _load_local_visible_from_file(dup_hang_ctx_t *dup_hang_ctx, HangDumpMemoryRangeHeader *header, unsigned long long *ppos)
{
    zx_map_argu_t       map = {0, };
    zx_cpu_vm_area_t    *vma = NULL;
    unsigned long long  total_size = header->size;
    unsigned int        blit_size = 0;
    unsigned int        len = 0;
    void                *addr = NULL;

    zx_info("load local visible %llx - %llx is_ptes=%d.\n", header->phyAddr,
            header->phyAddr + header->size, header->type & MEMORY_TYPE_PTES ? 1 : 0);

    map.flags.cache_type= ZX_MEM_UNCACHED;
    map.flags.mem_type  = ZX_SYSTEM_IO;
    map.phys_addr       = dup_hang_ctx->adapter->vidmm_bus_addr + header->phyAddr;
    map.size            = total_size;
    map.node_num        = 1;
    vma = zx_map_io_memory(&map);

    addr = vma->virt_addr;
    while(total_size > 0)
    {
        blit_size = total_size > E3K_TRANSFERBUFFER_FOR_HANG_SIZE ? E3K_TRANSFERBUFFER_FOR_HANG_SIZE : total_size;
        len = zx_file_read(dup_hang_ctx->file, addr, blit_size, ppos);
        zx_assert(len == blit_size, "");

        addr = (char*)addr + len;
        total_size -= len;
    }

    if (header->type & MEMORY_TYPE_PTES)
    {
        ptes_memory_t *ptes = zx_calloc(sizeof(*ptes));
        ptes->virt_addr = vma->virt_addr;
        ptes->size = map.size;
        ptes->vma = vma;
        list_add_tail(&ptes->link, &dup_hang_ctx->ptes_list);
    }
    else
    {
        zx_unmap_io_memory(vma);
    }
}

static void _load_local_unvisible_from_file(dup_hang_ctx_t *dup_hang_ctx, HangDumpMemoryRangeHeader *header, unsigned long long *ppos)
{
    adapter_t           *adapter = dup_hang_ctx->adapter;
    engine_share_e3k_t *share = adapter->private_data;
    unsigned long long  total_size = header->size;
    unsigned int        blit_size = 0;
    unsigned int        len = 0;
    void                *addr = NULL;
    unsigned long long  dst_gpu_va = header->phyAddr;

    zx_info("load local unvisible %llx - %llx.\n", header->phyAddr, header->phyAddr + header->size);
    while(total_size)
    {
        blit_size = total_size >  E3K_TRANSFERBUFFER_FOR_HANG_SIZE ? E3K_TRANSFERBUFFER_FOR_HANG_SIZE : total_size;

        len = zx_file_read(dup_hang_ctx->file, share->transfer_buffer_for_hang->krnl_cpu_vma->virt_addr, blit_size, ppos);

        zx_assert(blit_size == len, "");

        vidschi_copy_mem_e3k(adapter,
                            dst_gpu_va,
                            NULL,
                            share->transfer_buffer_for_hang->gpu_va,
                            NULL,
                            blit_size);

        dst_gpu_va += blit_size;

        total_size -= blit_size;
    }
}

struct _addrmap_param
{
    dup_hang_ctx_t *ctx;
    struct os_pages_memory *memory;
    unsigned long long phyAddr;
    zx_cpu_vm_area_t *vma;
};

static int _add_addr_map(void *arg, int page_start, int page_cnt, unsigned long long dma_addr)
{
    struct _addrmap_param *param = arg;
    dup_hang_ctx_t *dup_hang_ctx = param->ctx;
    addr_map_entry_t *addrmap = NULL;

    addrmap = zx_calloc(sizeof(*addrmap));
    addrmap->phyAddr = param->phyAddr + (page_start << 12);
    addrmap->size = page_cnt << 12;
    addrmap->target = dma_addr;
    addrmap->memory = param->memory;
    addrmap->offset = (page_start << 12);
    addrmap->vma = param->vma;
    list_add_tail(&addrmap->link, &dup_hang_ctx->addr_map_list);

    return 0;
}

static void _load_pcie_from_file(dup_hang_ctx_t *dup_hang_ctx, HangDumpMemoryRangeHeader *header, unsigned long long *ppos)
{
    adapter_t *adapter = dup_hang_ctx->adapter;
    unsigned int blit_size, len;
    unsigned long long size = header->size;
    alloc_pages_flags_t alloc_flags = {0};
    zx_map_argu_t map = {0};
    zx_cpu_vm_area_t  *vma = NULL;
    struct os_pages_memory *memory = NULL;
    unsigned long long phyAddr = header->phyAddr;
    struct _addrmap_param param = {0, };

    zx_info("load pcie %llx - %llx.\n", header->phyAddr, header->phyAddr + header->size);

    while(size > 0)
    {
        blit_size = size > E3K_TRANSFERBUFFER_FOR_HANG_SIZE ? E3K_TRANSFERBUFFER_FOR_HANG_SIZE : size;

        alloc_flags.dma32 = FALSE;
        alloc_flags.need_zero = TRUE;
        alloc_flags.fixed_page = TRUE;
        alloc_flags.need_dma_map = FALSE;
        alloc_flags.need_flush  = FALSE;

        memory = zx_allocate_pages_memory(adapter->os_device.pdev, blit_size, 1 << 12, alloc_flags);
        zx_assert(memory != NULL, "fail to allocate pages");

        map.flags.cache_type = ZX_MEM_WRITE_COMBINED;
        map.flags.mem_type   = ZX_SYSTEM_RAM;
        map.size             = blit_size;
        map.memory           = memory;
        vma = zx_map_pages_memory(&map);
        zx_assert(vma && vma->virt_addr , "fail to map");

        len = zx_file_read(dup_hang_ctx->file, vma->virt_addr, blit_size, ppos);
        zx_assert(len==blit_size, "");

        param.ctx = dup_hang_ctx;
        param.phyAddr = phyAddr;
        param.memory = memory;
        param.vma = vma;
        zx_pages_memory_for_each_continues(memory, &param, _add_addr_map);

        size -= blit_size;
        phyAddr += blit_size;
        dup_hang_ctx->pcie_memory_size += blit_size;
        //zx_unmap_pages_memory(vma);
    }
}

static unsigned long long _translate_pcie_addr(dup_hang_ctx_t *dup_hang_ctx, unsigned long long addr)
{
    static addr_map_entry_t *cached = NULL;
    addr_map_entry_t *map = cached;

    if (map && addr >= map->phyAddr && addr < map->phyAddr + map->size)
    {
        goto done;
    }

    list_for_each_entry(map, &dup_hang_ctx->addr_map_list, link)
    {
        if (addr >= map->phyAddr && addr < map->phyAddr + map->size)
        {
            cached = map;
            goto done;
        }
    }

    return 0;
done:
    return map ? map->target + (addr - map->phyAddr) : 0;
}

static void _load_memory_from_file(dup_hang_ctx_t *dup_hang_ctx)
{
    HangDumpMemoryRangeHeader header = {0, };
    unsigned int        len   = 0;
    unsigned long long pos = sizeof(HangDumpFileHeader);
    zx_map_argu_t       map = {0, };
    zx_cpu_vm_area_t    *vma = NULL;

    map.flags.cache_type= ZX_MEM_UNCACHED;
    map.flags.mem_type  = ZX_SYSTEM_IO;
    map.phys_addr       = dup_hang_ctx->adapter->vidmm_bus_addr;
    map.size            = dup_hang_ctx->adapter->visible_local_memory_size;
    map.node_num        = 1;
    vma = zx_map_io_memory(&map);
    dup_hang_ctx->local_visible_memory_cpu_va = vma->virt_addr;

    // should load cpu invisible local first, because it use gpu 2dblit
    len = zx_file_read(dup_hang_ctx->file, &header, sizeof(HangDumpMemoryRangeHeader), &pos);
    zx_assert(len == sizeof(HangDumpMemoryRangeHeader), "");
    while(header.type != MEMORY_TYPE_INVALID)
    {
        switch(header.type & (~MEMORY_TYPE_PTES))
        {
        case MEMORY_TYPE_LOCAL_VISIBLE:
        case MEMORY_TYPE_PCIE:
            pos += header.size;
            break;
        case MEMORY_TYPE_LOCAL_INVISIBLE:
#ifdef ZX_VMI_MODE
            _load_local_visible_from_file(dup_hang_ctx, &header, &pos);
#else
            _load_local_unvisible_from_file(dup_hang_ctx, &header, &pos);
#endif
            break;
        default:
            zx_assert(0, "Invalid memory type.");
            break;
        }
        len = zx_file_read(dup_hang_ctx->file, &header, sizeof(HangDumpMemoryRangeHeader), &pos);
        zx_assert(len == sizeof(HangDumpMemoryRangeHeader), "");
    }

    pos = sizeof(HangDumpFileHeader);
    len = zx_file_read(dup_hang_ctx->file, &header, sizeof(HangDumpMemoryRangeHeader), &pos);
    zx_assert(len == sizeof(HangDumpMemoryRangeHeader), "");
    while(header.type != MEMORY_TYPE_INVALID)
    {
        switch(header.type & (~MEMORY_TYPE_PTES))
        {
        case MEMORY_TYPE_LOCAL_VISIBLE:
            _load_local_visible_from_file(dup_hang_ctx, &header, &pos);
            break;
        case MEMORY_TYPE_PCIE:
            _load_pcie_from_file(dup_hang_ctx, &header, &pos);
            break;
        case MEMORY_TYPE_LOCAL_INVISIBLE:
            pos += header.size;
            break;
        default:
            zx_assert(0, "Invalid memory type %x.", header.type);
            break;
        }

        len = zx_file_read(dup_hang_ctx->file, &header, sizeof(HangDumpMemoryRangeHeader), &pos);
        zx_assert(len == sizeof(HangDumpMemoryRangeHeader), "");
    }
}

static void* _get_local_cpu_va(dup_hang_ctx_t *dup_hang_ctx, unsigned long long gpuva)
{
    return (char*)dup_hang_ctx->local_visible_memory_cpu_va + gpuva;
}

static void _rebuild_page_table(dup_hang_ctx_t *dup_hang_ctx)
{
    adapter_t *adapter = dup_hang_ctx->adapter;
    ptes_memory_t *ptes = NULL;
    int num_entry;
    int i;

    list_for_each_entry(ptes, &dup_hang_ctx->ptes_list, link)
    {
        num_entry = ptes->size / sizeof(int);
        for (i = 0; i < num_entry; i++)
        {
            PTE_L3_t *level3 = (PTE_L3_t*)ptes->virt_addr + i;
            if (level3->pte_4k.Valid && level3->pte_4k.Segment == 0)
            {
                unsigned int old = level3->pte_4k.Addr;

                level3->pte_4k.Addr = _translate_pcie_addr(dup_hang_ctx, (unsigned long)old << 12) >> 12;
                //zx_info("patch pte %x -> %x.\n", old, level3->pte_4k.Addr);
            }
        }
    }
}

static void* _get_cpu_va(dup_hang_ctx_t *dup_hang_ctx, PTE_L3_t *pt3)
{
    void *cpuva = NULL;
    unsigned long long cpupa = 0;

    if (pt3->pte_4k.Valid)
    {
        cpupa = ((unsigned long long)pt3->pte_4k.Addr << 12);
        if (pt3->pte_4k.Segment)
        {
            // local
            cpuva = (char*)_get_local_cpu_va(dup_hang_ctx, cpupa);
        }
        else
        {
            // pcie
            addr_map_entry_t *addrmap = NULL;
            list_for_each_entry(addrmap, &dup_hang_ctx->addr_map_list, link)
            {
                if (cpupa >= addrmap->target && cpupa < addrmap->target + addrmap->size)
                {
                    zx_cpu_vm_area_t *vma = addrmap->vma;
                    cpuva = (char*)vma->virt_addr + addrmap->offset + (cpupa - addrmap->target);
                }
            }
        }
    }
    return cpuva;
}

static void* _get_cpuva_from_gpuva(dup_hang_ctx_t *dup_hang_ctx, int pool_index, int proc_id, unsigned long long va)
{
    adapter_t *adapter = dup_hang_ctx->adapter;
    dh_common_info_e3k *dh_common_info = _get_dump_data_cpu_va(dup_hang_ctx, HANG_DUMP_COMMON_INFO, 0);
    dh_rb_info_e3k *rb_info = dh_common_info->rb_info + dup_hang_ctx->hang_index;
    unsigned int *share_ttbr = _get_local_cpu_va(dup_hang_ctx, dh_common_info->share_ttbr_gpu_va);
    unsigned int *ttbr = _get_local_cpu_va(dup_hang_ctx, rb_info->last_ttbr_gpu_va);
    int idx1, idx2, idx3;
    PTE_L1_t *pt1;
    PTE_L2_t *pt2;
    PTE_L3_t *pt3;
    static PTE_L3_t *cached_pt3 = NULL;
    static void* cached_pfn = 0;
    unsigned int *cpuva = NULL;
    unsigned long long cpupa = 0;

    if (adapter->hw_caps.legacy_offset_enable && va < adapter->total_local_memory_size)
    {
        cpuva = _get_local_cpu_va(dup_hang_ctx, va);
        goto done;
    }

    if (va < adapter->shared_gpu_va_size)
    {
        ttbr = share_ttbr;
    }

    idx1 = (va >> 32) & 0xff;
    idx2 = (va >> 22) & 0x3ff;
    idx3 = (va >> 12) & 0x3ff;

    pt1 = (PTE_L1_t*)(ttbr + idx1);
    pt2 = (PTE_L2_t*)_get_local_cpu_va(dup_hang_ctx, (unsigned long long)pt1->pte.Addr << 12) + idx2;
    pt3 = (PTE_L3_t*)_get_local_cpu_va(dup_hang_ctx, (unsigned long long)pt2->_4k_d.Addr << 12) + idx3;

    if (cached_pt3 != pt3)
    {
        cached_pt3 = pt3;
        cached_pfn = _get_cpu_va(dup_hang_ctx, pt3);
    }

    cpuva = (unsigned int*)((char*)cached_pfn + ((unsigned int)va & 0xfff));
done:
    return cpuva;
}

static void _read_buffer_from_gpuva(dup_hang_ctx_t *dup_hang_ctx, int pool_index, int proc_id, unsigned long long va, unsigned int *buffer, int dw_cnt)
{
    int i = 0;
    for (i = 0; i < dw_cnt; i++)
    {
        buffer[i] = *(unsigned int*)_get_cpuva_from_gpuva(dup_hang_ctx, pool_index, proc_id, va);
        va += 4;
    }
}

static void _restore_ttbr(dup_hang_ctx_t *dup_hang_ctx)
{
    int id, idx;
    adapter_t *adapter = dup_hang_ctx->adapter;
    dh_common_info_e3k *dh_common_info = _get_dump_data_cpu_va(dup_hang_ctx, HANG_DUMP_COMMON_INFO, 0);
    dh_rb_info_e3k *rb_info = dh_common_info->rb_info + dup_hang_ctx->hang_index;
    unsigned int *share_ttbr = _get_local_cpu_va(dup_hang_ctx, dh_common_info->share_ttbr_gpu_va);
    unsigned int *ttbr = _get_local_cpu_va(dup_hang_ctx, rb_info->last_ttbr_gpu_va);

    // share
    for (idx = 0; idx < (adapter->shared_gpu_va_size >> 32); idx++)
    {
        zx_write32(adapter->mmio + MMIO_MMU_START_ADDRESS + Reg_Mmu_Ttbr_Offset * 4 + 4 * idx, share_ttbr[idx]);
    }

    // private
    for (idx = (adapter->shared_gpu_va_size >> 32); idx < 256; idx++)
    {
        zx_write32(adapter->mmio + MMIO_MMU_START_ADDRESS + Reg_Mmu_Ttbr_Offset * 4 + rb_info->last_process_id * 256 * 4 + 4 * idx, ttbr[idx]);
    }

    adapter->gart_dirty.dirty = 1;
    adapter->gart_dirty.dirty_addr = 0;
    adapter->gart_dirty.dirty_mask = -1ULL;

    enginei_mmio_invalidate_tlb_e3k(adapter);
}

static void _load_data_from_file(dup_hang_ctx_t *dup_hang_ctx)
{
    struct os_file *file;
    file = zx_file_open(dup_hang_ctx->image, OS_RDWR | OS_LARGEFILE, 0666);

    if(!file)
    {
        zx_error("open file: open file %s fail.\n", dup_hang_ctx);
        return;
    }

    dup_hang_ctx->file = file;
    _load_file_header(dup_hang_ctx);
    _load_memory_from_file(dup_hang_ctx);
    _rebuild_page_table(dup_hang_ctx);
}

static void _get_submit_info(dup_hang_ctx_t *dup_hang_ctx, int pool_index)
{
    submit_info_t  *submit_info = &(dup_hang_ctx->submit_info);
    void           *ring_buffer_block = NULL;
    dh_rb_info_e3k *rb_info = NULL;
    dh_common_info_e3k *dh_common_info = _get_dump_data_cpu_va(dup_hang_ctx, HANG_DUMP_COMMON_INFO, 0);

    submit_info->ring_buffer_gpu_va = _get_dump_data_gpu_va(dup_hang_ctx, HANG_DUMP_RING_BUFFER, pool_index);
    ring_buffer_block = _get_dump_data_cpu_va(dup_hang_ctx, HANG_DUMP_RING_BUFFER, pool_index);
    rb_info = dh_common_info->rb_info + pool_index;
    submit_info->ring_buffer_size = rb_info->last_rb_size;
    submit_info->process_id = rb_info->last_process_id;
    submit_info->ring_buffer_cpu_va = ring_buffer_block;
    submit_info->fence_buffer_gpu_va = _get_dump_data_gpu_va(dup_hang_ctx, HANG_DUMP_FENCE_ADDR, pool_index);
    submit_info->fence_buffer_cpu_va = _get_cpuva_from_gpuva(dup_hang_ctx, pool_index, submit_info->process_id, submit_info->fence_buffer_gpu_va);
    submit_info->fence_value = rb_info->fence_value;

    zx_memcpy(&submit_info->status, &dh_common_info->hang_status, sizeof(submit_info->status));
    submit_info->rb_index = rb_info->last_rb_index;
}

static void _set_fence_id(dup_hang_ctx_t *dup_hang_ctx, unsigned long long fence_id)
{
    submit_info_t *submit_info = &(dup_hang_ctx->submit_info);
    void          *fence_cpu_va = submit_info->fence_buffer_cpu_va;

    *(unsigned int *)fence_cpu_va = fence_id & 0xffffffff;
    *(((unsigned int *)fence_cpu_va) + 1) = (fence_id >> 32) & 0xffffffff;
}

static unsigned long long _get_fence_id(dup_hang_ctx_t *dup_hang_ctx)
{
    submit_info_t      *submit_info = &(dup_hang_ctx->submit_info);
    void               *fence_cpu_va = submit_info->fence_buffer_cpu_va;
    unsigned long long  fence_id = 0;

    fence_id = *(unsigned long long *)fence_cpu_va;

    return fence_id;
}


static void _enable_new_ring_buffer(dup_hang_ctx_t *dup_hang_ctx)
{
    adapter_t                  *adapter                     = dup_hang_ctx->adapter;
    submit_info_t              *submit_info                 = &(dup_hang_ctx->submit_info);
    Reg_Run_List_Ctx_Addr1      reg_Run_List_Ctx_Addr1      = { 0 };
    Reg_Ring_Buf_Size           reg_Ring_Buf_Size           = { 0 };
    Reg_Ring_Buf_Head           reg_Ring_Buf_Head           = { 0 };
    Reg_Ring_Buf_Tail           reg_Ring_Buf_Tail           = { 0 };
    unsigned long long          ring_buffer_gpu_va          = submit_info->ring_buffer_gpu_va;
    int                         RbIndex                     = submit_info->rb_index;
    unsigned int                RegRbOffset = EngineRbOffset(adapter->chip_id, RbIndex);

    zx_assert((ring_buffer_gpu_va & 0xfff) == 0, "");
    reg_Ring_Buf_Tail.reg.Rb_Tail =
    reg_Ring_Buf_Head.reg.Rb_Head = 0;
    reg_Ring_Buf_Size.reg.Rb_Size = RING_BUFFER_SIZE_E3K;

    reg_Run_List_Ctx_Addr1.reg.Addr = (ring_buffer_gpu_va >> 12) & 0xFFFFFFF;
    reg_Run_List_Ctx_Addr1.reg.Kickoff = 0;

    if  ((RbIndex == RB_INDEX_CSL1) || (RbIndex ==RB_INDEX_CSL2) || (RbIndex == RB_INDEX_CSL3))
    {
        reg_Run_List_Ctx_Addr1.reg.L2_Cachable = 1;
    }

    zx_write32(adapter->mmio + RegRbOffset     * 4 + MMIO_CSP_START_ADDRESS, reg_Run_List_Ctx_Addr1.uint );
    zx_write32(adapter->mmio + (RegRbOffset+1) * 4 + MMIO_CSP_START_ADDRESS, reg_Ring_Buf_Size.uint );
    zx_write32(adapter->mmio + (RegRbOffset+2) * 4 + MMIO_CSP_START_ADDRESS, reg_Ring_Buf_Head.uint );
    zx_write32(adapter->mmio + (RegRbOffset+3) * 4 + MMIO_CSP_START_ADDRESS, reg_Ring_Buf_Tail.uint );

    reg_Run_List_Ctx_Addr1.reg.Kickoff = 1;

    zx_write32( adapter->mmio + RegRbOffset * 4 + MMIO_CSP_START_ADDRESS, reg_Run_List_Ctx_Addr1.uint );
}

static void _kick_off_cmd(dup_hang_ctx_t *dup_hang_ctx)
{
    adapter_t *adapter = dup_hang_ctx->adapter;
    submit_info_t *submit_info = &(dup_hang_ctx->submit_info);
    unsigned int RbIndex = submit_info->rb_index;
    unsigned int RegRbOffset = EngineRbOffset(adapter->chip_id, RbIndex);
    unsigned int tail = submit_info->ring_buffer_size;
    unsigned int offset = 0;

    zx_mb();
    zx_write32(adapter->mmio +  (RegRbOffset+3) * 4 + MMIO_CSP_START_ADDRESS, tail);
    zx_mb();

    do{
        offset = zx_read32(adapter->mmio +  (RegRbOffset+3) * 4 + MMIO_CSP_START_ADDRESS);
        if(offset != tail)
        {
            zx_error("idx:%d, kick off:%x, read:%x\n", RbIndex, tail, offset);
        }
    }while(tail != offset);
}

static void _dump_dma_info(dup_hang_ctx_t *dup_hang_ctx)
{
    submit_info_t *submit_info = &(dup_hang_ctx->submit_info);
    int RbIndex = submit_info->rb_index;
    unsigned long long ring_buffer_gpu_va = submit_info->ring_buffer_gpu_va;
    unsigned int rb_size = submit_info->ring_buffer_size;
    unsigned int *rb_buffer = NULL;
    submit_rb_cmd_part1_e3k_t *part1;
    submit_rb_cmd_part2_e3k_t *part2;
    int vmid = 0;

    if (RbIndex != 0)
        return;

    rb_buffer = zx_calloc(rb_size);
    _read_buffer_from_gpuva(dup_hang_ctx, dup_hang_ctx->hang_index, 0, ring_buffer_gpu_va, rb_buffer, rb_size >> 2);
    util_dump_memory(rb_buffer, rb_size, "ring_buffer");

    part1 = (submit_rb_cmd_part1_e3k_t*)rb_buffer;
    part2 = (submit_rb_cmd_part2_e3k_t*)(part1 + 1);

    vmid = part1->pwm_trigger_on.Process_Id;
    zx_assert(vmid == submit_info->process_id, "");

    while(part2->dma_cmd.Major_Opcode == CSP_OPCODE_Dma &&
          part2->dma_cmd.Mode == DMA_MODE_NORMAL &&
          (char*)part2 + 1 < (char*)rb_buffer + rb_size)
    {
        int dw_dma = part2->dma_cmd.Dw_Num;
        unsigned long long dma_va = part2->dma_cmd_addr_l + ((unsigned long long)part2->dma_cmd_addr_h << 32);
        unsigned int *dma = zx_calloc(dw_dma * 4);

        _read_buffer_from_gpuva(dup_hang_ctx, dup_hang_ctx->hang_index, vmid, dma_va, dma, dw_dma);

        util_dump_memory(dma, dw_dma << 2, "dma");
        zx_free(dma);

        part2++;
    }
}

static void _check_hang(dup_hang_ctx_t *dup_hang_ctx, int pool_index, unsigned long long fence_value)
{
    adapter_t *adapter = dup_hang_ctx->adapter;
    submit_info_t *submit_info = &(dup_hang_ctx->submit_info);
    EngineSatus_e3k status_after = {0};
    EngineSatus_e3k *status_before = &submit_info->status;
    int             i = 0;

    unsigned int            time_out = 0;
    unsigned long long      start_time = 0, current_time, delta_time;
    long                    temp_sec, temp_usec;

    zx_getsecs(&temp_sec, &temp_usec);
    start_time = temp_sec;

    zx_info("%s() start_time-%lld, send_fence-0x%llx, read_fence-0x%llx\n", __func__, start_time, fence_value, _get_fence_id(dup_hang_ctx));

    while(fence_value > _get_fence_id(dup_hang_ctx))
    {

        zx_getsecs(&temp_sec, &temp_usec);
        current_time = temp_sec;
        delta_time   = current_time - start_time;

        /* wait 60s, if fence still not back seems something wrong */
        if(delta_time > DUPLICATE_TIME_OUT)
        {
            unsigned int *status = (unsigned int *)&status_after;

            zx_info("%s() fail, current_time-%lld, delta_time-%lld, send_fence-0x%llx, read_fence-0x%llx\n", __func__, current_time, delta_time, fence_value, _get_fence_id(dup_hang_ctx));

            //get status
            for(i=0; i<7; i++)
            {
                *(status + i) = zx_read32(adapter->mmio + MMIO_CSP_START_ADDRESS + (Reg_Block_Busy_Bits_Top_Offset + i)*4);
            }
            zx_info("[Duplicate] engine 0x%x, Duplicated Hang Status:Top=%x,Gpc0_0=%x,Gpc0_1=%x,Gpc1_0=%x, Gpc1_1=%x,Gpc2_0=%x,Gpc2_1=%x\n",
                    submit_info->rb_index,
                    status_after.Top.uint,
                    status_after.Gpc0_0.uint,
                    status_after.Gpc0_1.uint,
                    status_after.Gpc1_0.uint,
                    status_after.Gpc1_1.uint,
                    status_after.Gpc2_0.uint,
                    status_after.Gpc2_1.uint);

            //vidsch_display_debugbus_info_e3k(adapter, NULL, FALSE);

            if((status_before->Top.uint & 0xFFFFFFF9) == (status_after.Top.uint & 0xFFFFFFF9) && //exclude mxua/b, diu will use it, even 3d hang.
                (status_before->Gpc0_0.uint == status_after.Gpc0_0.uint) &&
                (status_before->Gpc0_1.uint == status_after.Gpc0_1.uint) &&
                (status_before->Gpc1_0.uint == status_after.Gpc1_0.uint) &&
                (status_before->Gpc1_1.uint == status_after.Gpc1_1.uint) &&
                (status_before->Gpc2_0.uint == status_after.Gpc2_0.uint) &&
                (status_before->Gpc2_1.uint == status_after.Gpc2_1.uint))
            {
                zx_info("[Duplicate] <<-------------------:) Good job!, Status is same.------------------------>>.\n");
                zx_assert(0, "[Duplicate] <<-------------------:) Congratulation!! ------------------------>>\n");
            }
            else
            {
                zx_info("[Duplicate]<<------------------:( Sorry, Status is different.-------------------------->>.\n");

                zx_info("[Duplicate]engine 0x%x, Org Status: Top=%x,Gpc0_0=%x,Gpc0_1=%x,Gpc1_0=%x, Gpc1_1=%x,Gpc2_0=%x,Gpc2_1=%x\n",
                        submit_info->rb_index,
                        status_before->Top.uint,
                        status_before->Gpc0_0.uint,
                        status_before->Gpc0_1.uint,
                        status_before->Gpc1_0.uint,
                        status_before->Gpc1_1.uint,
                        status_before->Gpc2_0.uint,
                        status_before->Gpc2_1.uint);
                zx_assert(0, "[Duplicate] <<-------------------:( Do more check !! ------------------------>>\n");
            }
        }
    }

    zx_info("%s() pass, send_fence-0x%llx, read_fence-0x%llx\n", __func__, fence_value, _get_fence_id(dup_hang_ctx));

    return;
}
static void _duplicate_hang(dup_hang_ctx_t *dup_hang_ctx)
{
    adapter_t *adapter = dup_hang_ctx->adapter;
    int record_number = dup_hang_ctx->record_number;
    int pool_index = 0;
    unsigned long long fence_value = 0;
    dh_common_info_e3k *dh_common_info = _get_dump_data_cpu_va(dup_hang_ctx, HANG_DUMP_COMMON_INFO, 0);
    submit_info_t *submit_info = &(dup_hang_ctx->submit_info);
    unsigned long long hang_fence_id = -1Ull;
    int hang_fence_index = -1;

    // search the hang dma index
    for (pool_index = 0; pool_index < MAX_HANG_DUMP_DATA_POOL_NUMBER; pool_index++)
    {
        if (dh_common_info->rb_info[pool_index].fence_value > dh_common_info->last_return_fence_id)
        {
            if (dh_common_info->rb_info[pool_index].fence_value < hang_fence_id)
            {
                hang_fence_id = dh_common_info->rb_info[pool_index].fence_value;
                hang_fence_index = pool_index;
            }
        }
    }
    zx_info("last_return_fence_id = %lld, hang_fence_id = %lld, hang_fence_index = %d\n",
            dh_common_info->last_return_fence_id, hang_fence_id, hang_fence_index);

    if (hang_fence_id != dh_common_info->last_return_fence_id + 1)
        zx_error("hang_fence_id != last_return_fence_id + 1, maybe error!!!\n");

    pool_index =
    dup_hang_ctx->hang_index = hang_fence_index;
    do
    {
        zx_info("[Duplicate] pool_index=%d, count=%d\n",
                    pool_index, dh_common_info->hang_dump_counter);

        _restore_ttbr(dup_hang_ctx);

        zx_memset(submit_info, 0, sizeof(submit_info_t));

        //0. get info about submit
        _get_submit_info(dup_hang_ctx, pool_index);

        _dump_dma_info(dup_hang_ctx);

        fence_value = submit_info->fence_value;

        _set_fence_id(dup_hang_ctx, fence_value -1);

        switch(submit_info->rb_index)
        {
            case RB_INDEX_GFXL:
            case RB_INDEX_CSL0:
            case RB_INDEX_CSL1:
            case RB_INDEX_CSL2:
            case RB_INDEX_CSL3:
            {

                //1. setup ringbuffer
                _enable_new_ring_buffer(dup_hang_ctx);

                //3. kick off cmd
                _kick_off_cmd(dup_hang_ctx);

                break;
            }

            case RB_INDEX_GFXH:
            default:
                break;
        }

        //3. check hang
        _check_hang(dup_hang_ctx, pool_index, fence_value);
    }
    while (0);
}

void vidsch_duplicate_hang_e3k(adapter_t * adapter, const char *image)
{
    dup_hang_ctx_t dup_hang_ctx = {0};

    zx_info("^^^^^^^^^^^^^^^^^^^^^^^^^^^^vidsch_duplicate_hang_e3k^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^\n");

    //block any other submit:
    debug_mode_e3k.internal_block_submit = 1;

    zx_memset(&dup_hang_ctx, 0, sizeof(dup_hang_ctx));

    if (!image)
    {
        image = "/var/fb.FB";
    }

    dup_hang_ctx.image = image;

    dup_hang_ctx.adapter = adapter;

    //1. restore image to memory
    _load_data_from_file(&dup_hang_ctx);

    //2. duplicate hang
    _duplicate_hang(&dup_hang_ctx);

    debug_mode_e3k.internal_block_submit = 0;
}



unsigned int vidsch_pre_save_misc_e3k(adapter_t *adapter, vidsch_mgr_t *sch_mgr, unsigned int what_to_save)
{
    engine_share_e3k_t *share           = adapter->private_data;
    engine_e3k_t *engine                = sch_mgr->private_data;
    dh_common_info_e3k     *dh_common_info;
    unsigned long long     buffer_phys_addr;
    unsigned char*         buffer_virt_addr;
    int id, idx;

    dh_common_info = (dh_common_info_e3k*)(vidschi_get_dump_data_cpu_virt_addr_e3k(adapter, HANG_DUMP_DH_COMMON_INFO_OFFSET, 0));

    if(what_to_save == SAVE_BEFORE_POSTHANG) //Save Resouce,DMA,CONTEXT
    {
        dh_common_info->current_hang_dump_index = (dh_common_info->current_hang_dump_index + 1) % MAX_HANG_DUMP_DATA_POOL_NUMBER;
        dh_common_info->hang_dump_counter++;
        dh_common_info->share_ttbr_gpu_va = adapter->mm_mgr->share_pt_buffer->fb_pages_mem->range_node_list[0]->start;
    }
    else if(what_to_save == SAVE_AFTER_POSTHANG)
    {
        dh_common_info->last_return_fence_id = *(unsigned long long*)engine->fence_buffer_cpu_va;
        dh_common_info->hang_status = engine->dumped_engine_status;

        zx_info("vidsch_save_misc_e3k hang fence id:%d\n", dh_common_info->last_return_fence_id);

        zx_info("vidsch_save_misc_e3k save status: %08x,%08x,%08x,%08x,%08x,%08x,%08x\n",
            dh_common_info->hang_status.Top.uint,dh_common_info->hang_status.Gpc0_0.uint,dh_common_info->hang_status.Gpc0_1.uint,
            dh_common_info->hang_status.Gpc1_0.uint,dh_common_info->hang_status.Gpc1_1.uint,dh_common_info->hang_status.Gpc2_0.uint,
            dh_common_info->hang_status.Gpc2_1.uint);
    }
    else if(what_to_save == SAVE_RING_BUFFER)
    {
        dh_rb_info_e3k  *dh_rb_info;
        unsigned int    offset;
        unsigned int    *tmp;
        unsigned char   *ring_buffer  = NULL;

        ring_buffer = vidschi_get_dump_data_cpu_virt_addr_e3k(adapter,HANG_DUMP_RING_BUFFER_OFFSET, dh_common_info->current_hang_dump_index);
        dh_rb_info = dh_common_info->rb_info + dh_common_info->current_hang_dump_index;
        dh_rb_info->last_rb_index = sch_mgr->engine_index;
        dh_rb_info->fence_gpu_va = engine->fence_buffer_gpu_va;
        dh_rb_info->fence_value = engine->last_send_fence_id;

        switch(sch_mgr->engine_index)
        {
            case RB_INDEX_GFXL:
            case RB_INDEX_CSL0:
            case RB_INDEX_CSL1:
            case RB_INDEX_CSL2:
            case RB_INDEX_CSL3:
                dh_rb_info->last_rb_size  = engine->last_ring_buffer_size;
                dh_rb_info->last_process_id = engine->last_process_id;
                dh_rb_info->last_ttbr_gpu_va = engine->last_ttbr_gpu_va;
                zx_assert(dh_rb_info->last_rb_size <= HangDump_RingBufferBlockSize, "");
                // save ring buffer
                zx_memcpy((void*)ring_buffer, engine->last_ring_buffer, engine->last_ring_buffer_size);
                break;
            //cmd followed mode
            case RB_INDEX_GFXH:
            case RB_INDEX_VCP0:
            case RB_INDEX_VCP1:
            case RB_INDEX_VCP2:
//            case RB_INDEX_VPP:
            case RB_INDEX_VPP0:
            case RB_INDEX_VPP1:
            default:
                zx_assert(0, "Not yet add support engine hang..., engine:%d\n", sch_mgr->engine_index);
        }
    }
    return dh_common_info->current_hang_dump_index;
}

void vidsch_trigger_duplicate_hang_e3k(adapter_t *adapter, const char *image)
{
    vidsch_duplicate_hang_e3k(adapter, image);
}

