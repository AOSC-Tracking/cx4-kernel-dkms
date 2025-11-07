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
#include "vidsch.h"
#include "vidschi.h"
#include "vidmm.h"
#include "ring_buffer.h"
#include "vidsch_engine_e3k.h"
#include "mm_e3k.h"
#include "Chip/registerDef.h"
#include "chip_include_e3k.h"
#include "bit_op.h"
#include "vidsch_debug_hang_e3k.h"
#include "vidsch_dfs_e3k.h"

vidsch_chip_func_t engine_gfx_low_chip_func_e3k;
vidsch_chip_func_t engine_gfx_high_chip_func_e3k;
vidsch_chip_func_t engine_cs_high_chip_func_e3k;
vidsch_chip_func_t engine_cs_low0_chip_func_e3k;
vidsch_chip_func_t engine_cs_low1_chip_func_e3k;
vidsch_chip_func_t engine_cs_low2_chip_func_e3k;
vidsch_chip_func_t engine_cs_low3_chip_func_e3k;
vidsch_chip_func_t engine_vcp_low0_chip_func_e3k;
vidsch_chip_func_t engine_vcp_low1_chip_func_e3k;
vidsch_chip_func_t engine_vcp_low2_chip_func_e3k;
vidsch_chip_func_t engine_vpp_chip_func_e3k;
vidsch_chip_func_t engine_vpp0_chip_func_e3k;
vidsch_chip_func_t engine_vpp1_chip_func_e3k;

extern void vidschi_init_blt_cmd_e3k(engine_share_e3k_t *share, adapter_t *adapter);
extern void vidsch_dump_hang_e3k(adapter_t *adapter);

void enginei_mmio_invalidate_tlb_e3k (adapter_t * adapter)
{
    Reg_Pt_Inv_Trig reg_Pt_Inv_Trig                 = {0};
    Reg_Pt_Inv_Addr reg_Pt_Inv_Addr                 = {0};
    Reg_Pt_Inv_Mask reg_Pt_Inv_Mask                 = {0};
    Cmd_Block_Command_Mxu InvTriCmd                 = {0};
    Cmd_Block_Command_Mxu_Ptc_Dword0   InvCmdDword0 = {0};
    Cmd_Block_Command_Mxu_Ptc_Dword1   InvCmdDword1 = {0};
    Cmd_Block_Command_Mxu_Ptc_Dword2   InvCmdDword2 = {0};
    Cmd_Block_Command_Mxu_Ptc_Dword3   InvCmdDword3 = {0};
    DWORD               DtlbShift    = 0x10; //Per HW, one Dtlb Cachelline is 512bit, one cache line size maps to 16*4KB virtual address.
    DWORD               UtlbShift    = 0x1a; //Per HW, one Utlb Cacheline is 512bit, one cache line size maps to 16*4MB virtual address.
    reg_Pt_Inv_Trig.reg.Target       = PT_INV_TRIG_TARGET_DTLB;

    zx_mutex_lock(adapter->gart_table_lock);

    if(adapter->gart_dirty.dirty == FALSE)
    {
        zx_mutex_unlock(adapter->gart_table_lock);
        return;
    }

    reg_Pt_Inv_Addr.reg.Address = (adapter->gart_dirty.dirty_addr>>DtlbShift)<<4;
    reg_Pt_Inv_Mask.reg.Mask    = ((~adapter->gart_dirty.dirty_mask )>>DtlbShift)<<4;

    reg_Pt_Inv_Trig.reg.Proc_Id = 0;
    reg_Pt_Inv_Addr.reg.Proc_Id = 0;
    reg_Pt_Inv_Mask.reg.Proc_Id = 0;
    zx_write32(adapter->mmio + MMIO_MXU_START_ADDRESS + Reg_Pt_Inv_Addr_Offset*4 , reg_Pt_Inv_Addr.uint);
    zx_write32(adapter->mmio + MMIO_MXU_START_ADDRESS + Reg_Pt_Inv_Mask_Offset*4,  reg_Pt_Inv_Mask.uint);
    zx_write32(adapter->mmio + MMIO_MXU_START_ADDRESS + Reg_Pt_Inv_Trig_Offset*4,  reg_Pt_Inv_Trig.uint);

    adapter->gart_dirty.dirty_mask = 0;
    adapter->gart_dirty.dirty_addr = 0;
    adapter->gart_dirty.dirty = FALSE;

    zx_mutex_unlock(adapter->gart_table_lock);
}

/* CHX004:
PMU_PART_SW_RESET_set
    [29]    MXU1_ADB_RSTB
    [28]    MXU0_ADB_RSTB
    [27]    REG_RSTB_tmp
    [26]    IGA1_RSTB_tmp
    [25]    IGA2_RSTB_tmp
    [24]    IGA3_RSTB_tmp
    [23]    HOT_WIRE_RESETB_tmp
    [22]    PMP_RESETB
    [21]    BIU_RESETB
    [20]    DIU_RESETB
    [19]    MXU1_RESETB
    [18]    CSP_MXU0_RESETB
    [17]    L2_RESETB_tmp
    [16]    POOL_RESETB_tmp
    [15]    SGTBE_RESETB_tmp
    [14]    SPTFE_RESETB_tmp
    [13]    EUC0_RESETB_tmp
    [12]    FFU0_RESETB_tmp
    [11]    TU0_RESETB_tmp
    [10]    PEA0_RESETB_tmp
    [9]    PEB0_RESETB_tmp
    [8]    EUC1_RESETB_tmp
    [7]    FFU1_RESETB_tmp
    [6]    TU1_RESETB_tmp
    [5]    PEA1_RESETB_tmp
    [4]    PEB1_RESETB_tmp
    [3]    S3VD0_RESETB_tmp
    [2]    S3VD1_RESETB_tmp
    [1]    VPP_RESETB_tmp
    [0]    GFX_TOP_RESETB
*/

void vidschi_reset_adapter_e3k(adapter_t *adapter)
{
    if(adapter->chip_id >= CHIP_CHX004)
    {
        unsigned int read_value = 0;
        Reg_HWReset_004  dwResetHW = {0};

        zx_disp_wait_idle(adapter->disp_info);

#ifndef ZX_VMI_MODE
        read_value = zx_read32(adapter->mmio + 0x6882c); // PMU_SW_RST_AUTO_RLS
        zx_write32(adapter->mmio + 0x6882c, (read_value & 0xfffffffe));//PMU_SW_RST_AUTO_RLS

        dwResetHW.uint = 0x300C00FF | (adapter->chip_id == CHIP_CNE001 ? 0xc0000000 : 0);
        zx_write32(adapter->mmio + 0x68808, dwResetHW.uint);//PMU_PART_SW_RESET_ADDR
        if (adapter->chip_id == CHIP_CNE001)
        {
            zx_write32(adapter->mmio + 0x6880c, 0x3);//PMU_PART_SW_RESET_ADDR1
        }

        zx_udelay(20);

        zx_write32(adapter->mmio + 0x68830, dwResetHW.uint);//PMU_PART_SW_RESET_RLS_ADDR
        if (adapter->chip_id == CHIP_CNE001)
        {
            zx_write32(adapter->mmio + 0x68834, 0x3);//PMU_PART_SW_RESET_RLS_ADDR1
        }

        while(zx_read32(adapter->mmio + 0x68808));

        zx_udelay(20);

        if (adapter->pm_caps.pwm_mode)
        {
            Reg_Pwr_Mgr_Status1 reg_status1;
            int delay_times = 2000 * 100; // 2000 * 100 * 1 us = 200ms

            if(adapter->chip_id == CHIP_CHX004)
            {
                reg_status1.uint = zx_read32(adapter->mmio + MMIO_CSP_START_ADDRESS + Reg_Pwr_Mgr_Status1_Offset * 4);
                while(reg_status1.reg.Csp_Pwm_Central_Pwron_Be == 0 ||
                  reg_status1.reg.Csp_Pwm_Slc0_Pwron_Be == 0 ||
                  reg_status1.reg.Csp_Pwm_Slc1_Pwron_Be == 0 ||
                  reg_status1.reg.Csp_Pwm_S3vd0_Pwron_Be == 0 ||
                  reg_status1.reg.Csp_Pwm_S3vd1_Pwron_Be == 0 ||
                  reg_status1.reg.Csp_Pwm_Vpp_Pwron_Be == 0)
                {
                    delay_times--;
                    if (!delay_times)
                    {
                        zx_warning("cannot power on Be !!! reg_status1=0x%x\n", reg_status1);
                    break;
                    }
                    zx_udelay(1);

                    reg_status1.uint = zx_read32(adapter->mmio + MMIO_CSP_START_ADDRESS + Reg_Pwr_Mgr_Status1_Offset * 4);
                }
            }
            else if(adapter->chip_id == CHIP_CNE001)
            {
                Reg_Pwr_Mgr_Status2 reg_status2;
                reg_status1.uint = zx_read32(adapter->mmio + MMIO_CSP_START_ADDRESS + Reg_Pwr_Mgr_Status1_Offset_Cne001 * 4);
                reg_status2.uint = zx_read32(adapter->mmio + MMIO_CSP_START_ADDRESS + Reg_Pwr_Mgr_Status2_Offset * 4);
                while(reg_status1.reg_cne001.Csp_Pwm_Central_Pwron_Be == 0 ||
                  reg_status1.reg_cne001.Csp_Pwm_Slc0_Pwron_Be == 0 ||
                  reg_status1.reg_cne001.Csp_Pwm_Slc1_Pwron_Be == 0 ||
                  reg_status2.reg_cne001.Csp_Pwm_Vcp_Pwron_Be == 0 ||
                  reg_status2.reg_cne001.Csp_Pwm_Vdp0_Pwron_Be == 0 ||
                  reg_status2.reg_cne001.Csp_Pwm_Vdp1_Pwron_Be == 0 ||
                  reg_status2.reg_cne001.Csp_Pwm_Vpp0_Pwron_Be == 0 ||
                  reg_status2.reg_cne001.Csp_Pwm_Vpp1_Pwron_Be == 0)
                {
                    delay_times--;
                    if (!delay_times)
                    {
                        zx_warning("cannot power on Be !!! reg_status1=0x%x\n", reg_status1);
                    break;
                    }
                    zx_udelay(1);

                    reg_status1.uint = zx_read32(adapter->mmio + MMIO_CSP_START_ADDRESS + Reg_Pwr_Mgr_Status1_Offset_Cne001 * 4);
                    reg_status2.uint = zx_read32(adapter->mmio + MMIO_CSP_START_ADDRESS + Reg_Pwr_Mgr_Status2_Offset * 4);
                }
            }

            // reset again
            read_value = zx_read32(adapter->mmio + 0x6882c); // PMU_SW_RST_AUTO_RLS
            zx_write32(adapter->mmio + 0x6882c, (read_value & 0xfffffffe));//PMU_SW_RST_AUTO_RLS

            dwResetHW.uint = 0x300C00FF | (adapter->chip_id == CHIP_CNE001 ? 0xc0000000 : 0);
            zx_write32(adapter->mmio + 0x68808, dwResetHW.uint);//PMU_PART_SW_RESET_ADDR
            if (adapter->chip_id == CHIP_CNE001)
            {
                zx_write32(adapter->mmio + 0x6880c, 0x3);//PMU_PART_SW_RESET_ADDR1
            }

            zx_udelay(20);

            zx_write32(adapter->mmio + 0x68830, dwResetHW.uint);//PMU_PART_SW_RESET_RLS_ADDR
            if (adapter->chip_id == CHIP_CNE001)
            {
                zx_write32(adapter->mmio + 0x68834, 0x3);//PMU_PART_SW_RESET_RLS_ADDR1
            }

            while(zx_read32(adapter->mmio + 0x68808));
        }
#endif
    }

    {
        EngineSatus_e3k engine_status = {0};
        unsigned int *status = (unsigned int *)&engine_status;
        int i;

        for( i = 0; i <7; i++)
        {
            *(status + i) = zx_read32(adapter->mmio + MMIO_CSP_START_ADDRESS + (Reg_Block_Busy_Bits_Top_Offset + i)*4);
        }

        zx_info("after reset......\n");
        zx_info("current engine Status: %08x,%08x,%08x,%08x,%08x,%08x,%08x\n",
            engine_status.Top.uint,engine_status.Gpc0_0.uint,engine_status.Gpc0_1.uint,
            engine_status.Gpc1_0.uint,engine_status.Gpc1_1.uint,engine_status.Gpc2_0.uint,engine_status.Gpc2_1.uint);
        zx_info("done......\n");
    }
}

int  enginei_init_hardware_context_e3k(engine_gfx_e3k_t *gfx_engine)
{
    engine_e3k_t   *engine   = &gfx_engine->common;
    engine_share_e3k_t  *share  = engine->share;
    adapter_t      *adapter  = engine->sch_mgr->adapter;
    unsigned int*   pRB      = NULL, *pRB0;
    unsigned int    RbSize   = util_align(64 + (sizeof(CONTEXT_RESTORE_DMA_E3K) >> 2)+ POWER_SKIP_COMMAND_SIZE, 16);//rb size aligned with 16 dword.
    unsigned int dwSliceMask = adapter->hw_caps.chip_slice_mask;
    unsigned int GpcBitMask = 0, ShiftNum = 1;
    int i;

    RB_PREDEFINE_DMA *pRBDMA = (RB_PREDEFINE_DMA*) share->begin_end_buffer_cpu_va;
    Reg_Eu_Full_Glb                     reg_Eu_Full_Glb           = {0};
    Reg_Tu_Tssharp_Ctrl                 reg_Tu_Tssharp_Ctrl       = {0};
    Reg_Ffc_Ubuf_Golbalconfig           reg_Ffc_Ubuf_Golbalconfig = {0};
    Reg_Fence_Mask                      reg_Fence_Mask            = {0};
    BOOL                                b3dSigZero = FALSE;

    //L2 performance
    Reg_L2_Performance                  reg_L2_Performance        = {0};

    //pwm trigger
    Cmd_Blk_Cmd_Csp_Indicator           pwmTrigger      = {0};
    Cmd_Blk_Cmd_Csp_Indicator_Dword1    trigger_Dw      = {0};
    Cmd_Blk_Cmd_Csp_Indicator           pwmTriggerOff   = {0};
    Cmd_Blk_Cmd_Csp_Indicator_Dword1    triggerOff_Dw   = {0};

    Reg_U_Fence_Num_Ctrl                reg_U_Fence_Num_Ctrl = { 0 };
    reg_U_Fence_Num_Ctrl.uint = 0x40;

    while (dwSliceMask)
    {
        if (dwSliceMask & 0xf)
        {
            GpcBitMask |= ShiftNum;
        }
        dwSliceMask >>= 4;
        ShiftNum <<= 1;
    }

    enginei_mmio_invalidate_tlb_e3k(adapter);

    pwmTrigger.Major_Opcode         = CSP_OPCODE_Blk_Cmd_Csp_Indicator;
    pwmTrigger.Block_Id             = CSP_GLOBAL_BLOCK;
    pwmTrigger.Type                 = BLOCK_COMMAND_CSP_TYPE_INDICATOR;
    pwmTrigger.Info                 = BLK_CMD_CSP_INDICATOR_INFO_3D_MODE;
    pwmTrigger.Dwc                  = 1;
    trigger_Dw.Slice_Mask           = adapter->hw_caps.chip_slice_mask;

    pwmTriggerOff.Major_Opcode      = CSP_OPCODE_Blk_Cmd_Csp_Indicator;
    pwmTriggerOff.Block_Id          = CSP_GLOBAL_BLOCK;
    pwmTriggerOff.Type              = BLOCK_COMMAND_CSP_TYPE_INDICATOR;
    pwmTriggerOff.Info              = BLK_CMD_CSP_INDICATOR_INFO_OFF_MODE;
    pwmTriggerOff.Dwc               = 1;

    reg_Eu_Full_Glb.uint                        = 0;
    reg_Eu_Full_Glb.reg.Uav_En                  = 1;
    reg_Eu_Full_Glb.reg.Uav_Continue_Chk        = 1;
    if (adapter->chip_id >= CHIP_CHX004)
    {
        reg_Eu_Full_Glb.reg.Uav_Buf_Size            = EU_FULL_GLB_UAV_BUF_SIZE_32LINES;
        reg_Eu_Full_Glb.reg.U_3d_Base               = 6;//U_SHARP_3D_SLOT_START / 8;
        reg_Eu_Full_Glb.reg.Cb_3d_Base              = 0;//CB_3D_SLOT_START >> 4;
    }
    else
    {
        reg_Eu_Full_Glb.reg.Uav_Buf_Size            = EU_FULL_GLB_UAV_BUF_SIZE_128LINES;
        reg_Eu_Full_Glb.reg.U_3d_Base               = 6;//U_SHARP_3D_SLOT_START / 8;
        reg_Eu_Full_Glb.reg.Cb_3d_Base              = 8;//CB_3D_SLOT_START >> 4;
    }

    reg_Tu_Tssharp_Ctrl.uint                    = 0;
    reg_Ffc_Ubuf_Golbalconfig.uint              = 0;
    reg_Ffc_Ubuf_Golbalconfig.reg.U_3d_Base     = 6;//U_SHARP_3D_SLOT_START / 8;
    reg_Fence_Mask.reg.Mask                     = 0xFFFFFFFF;//init this reg as 0xffffffff per Wenni

    // initialize context buffer
    if (adapter->chip_id >= CHIP_CHX004)
    {
        CONTEXT_BUFFER_CHX004 *pContext = (CONTEXT_BUFFER_CHX004*)gfx_engine->context_buffer_cpu_va;
        zx_memset(pContext, 0, sizeof(CONTEXT_BUFFER_CHX004));

        pContext->CSPRegs.reg_Csp_Misc_Control.reg.Dump_3d_Signature_Zero = b3dSigZero;
        pContext->HWContextbuffer.shadow_buf_3D.fe_be_regs.ff_regs.reg_Ff_Glb_Ctrl.reg.Ffc_Config_Mode = FF_GLB_CTRL_FFC_CONFIG_MODE_ZS_D_U;
        //3d global
        //although 3d global regs will also be set in PSO, we still set an initial value here
        //because we may have some 2d command which will cause shadow buffer flush when initializing KMD
        //We need these 3d global regs set before shadow buffer flush, otherwise the HW will get crashed.
        pContext->HWContextbuffer.shadow_buf_3D.fe_be_regs.eufs_regs.reg_Eu_3d_Glb.reg.Cb_Ps_Base = reg_Eu_Full_Glb.reg.Cb_3d_Base; // umd will set it again, here is just for HW init check.
        pContext->HWContextbuffer.shadow_buf_3D.fe_be_regs.eufs_regs.reg_Eu_3d_Glb.reg.U_Ps_Base = reg_Eu_Full_Glb.reg.U_3d_Base; // umd will set it again, here is just for HW init check.
        pContext->HWContextbuffer.shadow_buf_3D.fe_be_regs.wls_regs.reg_Ffc_Ubuf_3dconfig.reg.U_Ps_Base = reg_Eu_Full_Glb.reg.U_3d_Base; // umd will set it again, here is just for HW init check.
        pContext->HWContextbuffer.shadow_buf_3D.fe_be_regs.tu_regs.reg_Tu_Tssharp_3d_Ctrl.reg.Base_Address_Be = reg_Tu_Tssharp_Ctrl.reg.Base_Address_3d;  // umd will set it again, here is just for HW init check.
        pContext->HWContextbuffer.shadow_buf_3D.fe_be_regs.ff_regs.reg_Ffc_Config.reg.En_Decompressed_Data_Reorder = 0x1;
        pContext->HWContextbuffer.shadow_buf_3D.fe_be_regs.ff_regs.reg_Ffc_Config.reg.Slot_Swz_Enable = 0x1;
    }
    else
    {
        CONTEXT_BUFFER *pContext = (CONTEXT_BUFFER*)gfx_engine->context_buffer_cpu_va;
        zx_memset(pContext, 0, sizeof(CONTEXT_BUFFER));
        //3d global
        //although 3d global regs will also be set in PSO, we still set an initial value here
        //because we may have some 2d command which will cause shadow buffer flush when initializing KMD
        //We need these 3d global regs set before shadow buffer flush, otherwise the HW will get crashed.
        pContext->CSPRegs.reg_Csp_Misc_Control.reg.Dump_3d_Signature_Zero = b3dSigZero;
        pContext->HWContextbuffer.shadow_buf_3D.fe_be_regs.ff_regs.reg_Ff_Glb_Ctrl.reg.Ffc_Config_Mode = FF_GLB_CTRL_FFC_CONFIG_MODE_ZS_D_U;
        pContext->HWContextbuffer.shadow_buf_3D.fe_be_regs.eufs_regs.reg_Eu_3d_Glb.reg.Cb_Ps_Base = reg_Eu_Full_Glb.reg.Cb_3d_Base; // umd will set it again, here is just for HW init check.
        pContext->HWContextbuffer.shadow_buf_3D.fe_be_regs.eufs_regs.reg_Eu_3d_Glb.reg.U_Ps_Base = reg_Eu_Full_Glb.reg.U_3d_Base; // umd will set it again, here is just for HW init check.
        pContext->HWContextbuffer.shadow_buf_3D.fe_be_regs.wls_regs.reg_Ffc_Ubuf_3dconfig.reg.U_Ps_Base = reg_Eu_Full_Glb.reg.U_3d_Base; // umd will set it again, here is just for HW init check.
        pContext->HWContextbuffer.shadow_buf_3D.fe_be_regs.tu_regs.reg_Tu_Tssharp_3d_Ctrl.reg.Base_Address_Be = reg_Tu_Tssharp_Ctrl.reg.Base_Address_3d;  // umd will set it again, here is just for HW init check.
        pContext->HWContextbuffer.shadow_buf_3D.fe_be_regs.ff_regs.reg_Ffc_Config.reg.En_Decompressed_Data_Reorder = 0x1;
        pContext->HWContextbuffer.shadow_buf_3D.fe_be_regs.ff_regs.reg_Ffc_Config.reg.Slot_Swz_Enable = 0x1;
    }

    // TODO: initialize context buffer for vulkan
    if (adapter->chip_id >= CHIP_CHX004)
    {
        CONTEXT_BUFFER_CHX004 *pContext = (CONTEXT_BUFFER_CHX004*)gfx_engine->context_buffer_cpu_va_for_vk;
        zx_memset(pContext, 0, sizeof(CONTEXT_BUFFER_CHX004));
    }
    else
    {
        CONTEXT_BUFFER *pContext = (CONTEXT_BUFFER*)gfx_engine->context_buffer_cpu_va_for_vk;
        zx_memset(pContext, 0, sizeof(CONTEXT_BUFFER));
    }

    //init hw global config
    if(!gfx_engine->is_high_engine)
    {
        CONTEXT_RESTORE_DMA_E3K *pRestoreDMA = NULL;

        pRB0 =
        pRB = enginei_get_ring_buffer_space_e3k(engine, RbSize);
        zx_memset(pRB, 0, (RbSize << 2));
        *pRB++ = *(unsigned int*)&pwmTrigger;
        *pRB++ = *(unsigned int*)&trigger_Dw;
        // skip 8dw for hw block sync
        pRB += 8;
        *pRB++ = SET_REGISTER_FD_E3K(EU_FS_BLOCK,Reg_Eu_Full_Glb_Offset,1);
        *pRB++ = reg_Eu_Full_Glb.uint;
        *pRB++ = SET_REGISTER_FD_E3K(TUFE_BLOCK,Reg_Tu_Tssharp_Ctrl_Offset,1);
        *pRB++ = reg_Tu_Tssharp_Ctrl.uint;
        *pRB++ = SET_REGISTER_FD_E3K(WLS_FE_BLOCK,Reg_Ffc_Ubuf_Golbalconfig_Offset,1);
        *pRB++ = reg_Ffc_Ubuf_Golbalconfig.uint;
        *pRB++ = SET_REGISTER_FD_E3K(CSP_GLOBAL_BLOCK, Reg_Fence_Mask_Offset, 1);
        *pRB++ = reg_Fence_Mask.uint;
        if (adapter->chip_id >= CHIP_CHX004)
        {
            *pRB++ = SET_REGISTER_FD_E3K(CSP_GLOBAL_BLOCK, Reg_U_Fence_Num_Ctrl_Offset, 1);
            *pRB++ = reg_U_Fence_Num_Ctrl.uint;
        }
        *pRB++ = SET_REGISTER_FD_E3K(L2_BLOCK, Reg_L2_Performance_Offset, 1);
        *pRB++ = reg_L2_Performance.uint;
        if (adapter->chip_id >= CHIP_CHX004)
        {
            *pRB++ = SET_REGISTER_FD_E3K(CSP_GLOBAL_BLOCK, Reg_Fence_Mask_Offset, 1);
            *pRB++ = reg_Fence_Mask.uint;
        }
        pRestoreDMA  = (CONTEXT_RESTORE_DMA_E3K*)pRB;

        zx_memcpy(pRestoreDMA, &(pRBDMA->RestoreDMA3D), sizeof(CONTEXT_RESTORE_DMA_E3K));

        pRestoreDMA->RestoreContext.Address_Mode = SET_REGISTER_ADDRESS_MODE_ADDRESS;
        pRestoreDMA->ContextBufferOffset_L = share->context_buf_gpu_va;
        pRestoreDMA->ContextBufferOffset_H = share->context_buf_gpu_va >> 32;

        //Csp will initinal its registers by itself, no need driver to do it.
        pRestoreDMA->RestoreCsp.uint = 0;
        pRestoreDMA->CSPOffset_H = 0;
        pRestoreDMA->CSPOffset_L = 0;

        pRestoreDMA->RestoreEuFullGlb.uint = 0;
        pRestoreDMA->EuFullGlb = 0;

        pRestoreDMA->RestoreTsSharpCtrl.uint = 0;
        pRestoreDMA->TsSharpCtrl = 0;

        pRestoreDMA->RestoreFFCUbufConfig.uint = 0;
        pRestoreDMA->FFCUbufConfig = 0;

        if (adapter->chip_id >= CHIP_CHX004)
        {
            pRestoreDMA->RestoreGpcpFe.uint = SET_REGISTER_ADDR_E3K(GPCPFE_BLOCK, Reg_Gpcpfe_Iidcnt_Offset, SET_REGISTER_ADDRESS_MODE_ADDRESS);
            pRestoreDMA->GpcpFeOffset_H = SET_REGISTER_ADDR_HIGH_AND_REGCNT_E3K(share->context_buf_gpu_va >> 32, sizeof(GPCPFE_CONTEXTBUFFER_REGS) >> 2, 0);
            pRestoreDMA->GpcpFeOffset_L = SET_REGISTER_ADDR_LOW_E3K(share->context_buf_gpu_va + (unsigned long)(&(((CONTEXT_BUFFER_CHX004*)0)->GpcpFeRegs)));

            pRestoreDMA->RestoreGpcpBe.uint = SET_REGISTER_ADDR_E3K(GPCPBE_BLOCK, Reg_Sto_Cfg_Offset, SET_REGISTER_ADDRESS_MODE_ADDRESS);
            pRestoreDMA->GpcpBeOffset_H = SET_REGISTER_ADDR_HIGH_AND_REGCNT_E3K(share->context_buf_gpu_va >> 32, sizeof(Gpcpbe_regs) >> 2, 0);
            pRestoreDMA->GpcpBeOffset_L = SET_REGISTER_ADDR_LOW_E3K(share->context_buf_gpu_va + (unsigned long)(&(((CONTEXT_BUFFER_CHX004*)0)->GpcpBeRegs)));

            pRestoreDMA->RestoreGpcTop[0].RestoreGpcTopCmd.uint = SET_REGISTER_ADDR_E3K(24, 0, SET_REGISTER_ADDRESS_MODE_ADDRESS);
            pRestoreDMA->RestoreGpcTop[0].GpcTopOffset_L = SET_REGISTER_ADDR_LOW_E3K(share->context_buf_gpu_va
                                                            + (unsigned long)(&(((CONTEXT_BUFFER_CHX004*)0)->GpcTopRegs[0])));
            pRestoreDMA->RestoreGpcTop[0].GpcTopOffset_H = SET_REGISTER_ADDR_HIGH_AND_REGCNT_E3K(0x0, 40, 0);
        }
        else
        {
            pRestoreDMA->RestoreGpcpFe.uint = SET_REGISTER_ADDR_E3K(GPCPFE_BLOCK, Reg_Gpcpfe_Iidcnt_Offset, SET_REGISTER_ADDRESS_MODE_ADDRESS);
            pRestoreDMA->GpcpFeOffset_H     = SET_REGISTER_ADDR_HIGH_AND_REGCNT_E3K(share->context_buf_gpu_va >>32, sizeof(GPCPFE_CONTEXTBUFFER_REGS) >> 2, 0);;
            pRestoreDMA->GpcpFeOffset_L     = SET_REGISTER_ADDR_LOW_E3K(share->context_buf_gpu_va + (unsigned long)(&(((CONTEXT_BUFFER*)0)->GpcpFeRegs)));

            pRestoreDMA->RestoreGpcpBe.uint = SET_REGISTER_ADDR_E3K(GPCPBE_BLOCK, Reg_Sto_Cfg_Offset, SET_REGISTER_ADDRESS_MODE_ADDRESS);
            pRestoreDMA->GpcpBeOffset_H     = SET_REGISTER_ADDR_HIGH_AND_REGCNT_E3K(share->context_buf_gpu_va >> 32, sizeof(Gpcpbe_regs) >> 2, 0);;
            pRestoreDMA->GpcpBeOffset_L     = SET_REGISTER_ADDR_LOW_E3K(share->context_buf_gpu_va + (unsigned long)(&(((CONTEXT_BUFFER*)0)->GpcpBeRegs)));

            while (GpcBitMask)
            {
                unsigned int gpcIndex = 0;
                _BitScanForward(&gpcIndex, GpcBitMask);
                _bittestandreset(&GpcBitMask, gpcIndex);
                pRestoreDMA->RestoreGpcTop[gpcIndex].RestoreGpcTopCmd.uint = SET_REGISTER_ADDR_E3K(24, 0 + gpcIndex * 40, SET_REGISTER_ADDRESS_MODE_ADDRESS);
                pRestoreDMA->RestoreGpcTop[gpcIndex].GpcTopOffset_L = SET_REGISTER_ADDR_LOW_E3K(share->context_buf_gpu_va
                                                                    + (unsigned long)(&(((CONTEXT_BUFFER*)0)->GpcTopRegs[gpcIndex])));
                pRestoreDMA->RestoreGpcTop[gpcIndex].GpcTopOffset_H = SET_REGISTER_ADDR_HIGH_AND_REGCNT_E3K(share->context_buf_gpu_va >> 32, 40, 0);
            }
        }


        pRB += (sizeof(CONTEXT_RESTORE_DMA_E3K)>> 2);

        for (i = 0; i < POWER_SKIP_COMMAND_SIZE; i++)
        {
            *pRB++  = 0;
        }
        *pRB++ = *(unsigned int*)&pwmTriggerOff;
        *pRB++ = *(unsigned int*)&triggerOff_Dw;

        enginei_release_ring_buffer_space_e3k(engine, RbSize);
    }

    return 0;
}

int enginei_common_share_init_e3k(engine_e3k_t *engine)
{
    engine_share_e3k_t *share = engine->share;

    vidschi_init_blt_cmd_e3k(share, engine->sch_mgr->adapter);

    return S_OK;
}

int enginei_common_init_e3k(vidsch_mgr_t *sch_mgr, engine_e3k_t *engine, unsigned int *current_local_offset, unsigned int *current_pcie_offset)
{
    adapter_t               *adapter                  = sch_mgr->adapter;
    engine_share_e3k_t      *share                    = adapter->private_data;
    unsigned int            *virt_buffer              = NULL;
    vidmm_segment_memory_t  *pcie_reserved_memory     = sch_mgr->pcie_reserved_memory;
    unsigned long long      pcie_reserved_gpu_va      = pcie_reserved_memory->gpu_va;
    unsigned char           *pcie_reserved_cpu_va     = pcie_reserved_memory->krnl_cpu_vma->virt_addr;
    unsigned long long      local_reserved_gpu_va     = 0;
    unsigned char           *local_reserved_cpu_va    = NULL;

    int result = S_OK;

    sch_mgr->private_data = engine;
    engine->sch_mgr   = sch_mgr;

    engine->ring_buf_phys_addr    = pcie_reserved_gpu_va + *current_pcie_offset;
    engine->ring_buf_virt_addr    = (unsigned int *)(pcie_reserved_cpu_va + *current_pcie_offset);

    *current_pcie_offset += RING_BUFFER_SIZE_E3K;

    zx_memset(engine->ring_buf_virt_addr, 0, RING_BUFFER_SIZE_E3K);

    /* request fence buffer */
    result = vidsch_request_fence_space(sch_mgr->adapter, &engine->fence_buffer_cpu_va, NULL, NULL, &engine->fence_buffer_gpu_va);
    zx_assert(result == S_OK, "result:%d", result);

    zx_debug("engine[%d]. fence buffer gpu_va: %llx, rb gpu va:%llx, rb cpu va:%p\n",
                sch_mgr->engine_index, engine->fence_buffer_gpu_va, engine->ring_buf_phys_addr, engine->ring_buf_virt_addr);

    result = vidsch_request_fence_space(sch_mgr->adapter, &engine->fence_buffer_cpu_va_fake, NULL, NULL, &engine->fence_buffer_gpu_va_fake);
    zx_assert(result == S_OK, "result:%d", result);

    zx_debug("engine[%d]. fence buffer gpu_va_fake: %llx, fence_buffer cpu va:%llx\n",
                sch_mgr->engine_index, engine->fence_buffer_gpu_va, engine->fence_buffer_gpu_va_fake);

    sch_mgr->fence_buffer_cpu_va = engine->fence_buffer_cpu_va;

    enginei_enable_ring_buffer_e3k(engine);

    if(share != NULL)
    {
       share->ref_cnt++;
       engine->share = share;
    }

    return result;
}

int enginei_init_pwm_reg_004(engine_e3k_t *engine)
{
    adapter_t            *adapter = engine->sch_mgr->adapter;
    unsigned int        rb_size_uint = 29;
    unsigned int        align_rb_size_uint = util_align(rb_size_uint + POWER_SKIP_COMMAND_SIZE, 16);
    int i;
    unsigned int *pRB = enginei_get_ring_buffer_space_e3k(engine, align_rb_size_uint);

    Reg_Pwr_Mgr_Cfg_En pwr_mgr_cfg_en = {.uint = 0xe0001fff};

    Reg_Pwr_Mgr_Sw_Cfg_En sw_cfg_en = {.uint = 0x0};
    Reg_Pwr_Mgr_Sw_Cfg0 pwr_mgr_sw_cfg0 = {.uint = 0x0};

    Cmd_Blk_Cmd_Csp_Indicator       pwm_trigger_on = {0,};
    Cmd_Blk_Cmd_Csp_Indicator_Dword1 trigger_on_dw = {0,};
    Cmd_Blk_Cmd_Csp_Indicator       pwm_trigger_off = {0, };
    Cmd_Blk_Cmd_Csp_Indicator_Dword1 trigger_off_dw = {0,};

    zx_memset(pRB, 0, (align_rb_size_uint << 2));

    pwm_trigger_on.Major_Opcode = CSP_OPCODE_Blk_Cmd_Csp_Indicator;
    pwm_trigger_on.Block_Id = CSP_GLOBAL_BLOCK;
    pwm_trigger_on.Type = BLOCK_COMMAND_CSP_TYPE_INDICATOR;
    pwm_trigger_on.Info = BLK_CMD_CSP_INDICATOR_INFO_3D_MODE;
    pwm_trigger_on.Dwc = 1;

    pwm_trigger_off.Major_Opcode = CSP_OPCODE_Blk_Cmd_Csp_Indicator;
    pwm_trigger_off.Block_Id = CSP_GLOBAL_BLOCK;
    pwm_trigger_off.Type = BLOCK_COMMAND_CSP_TYPE_INDICATOR;
    pwm_trigger_off.Info = BLK_CMD_CSP_INDICATOR_INFO_OFF_MODE;
    pwm_trigger_off.Dwc = 1;

    trigger_on_dw.Slice_Mask = adapter->hw_caps.chip_slice_mask;
    trigger_off_dw.Slice_Mask = adapter->hw_caps.chip_slice_mask;

    // set auto mode register
#define SET_CFG_EN_REG(type1, type2) \
    do { \
        pwr_mgr_cfg_en.reg.En_Pwrgate_##type1 = adapter->pm_caps.type2##_pg_auto; \
        pwr_mgr_cfg_en.reg.En_Clkgate_##type1 = adapter->pm_caps.type2##_cg_auto; \
    }while(0)

    SET_CFG_EN_REG(3d_Central, gfx);
    SET_CFG_EN_REG(3d_Slc0, gfx);
    SET_CFG_EN_REG(3d_Slc1, gfx);

    SET_CFG_EN_REG(S3vd0, vcp0);
    SET_CFG_EN_REG(S3vd1, vcp1);

    SET_CFG_EN_REG(Vpp, vpp0);

    // set manual mode register
#define SET_SW_CFG0_REG(type, val) \
    do { \
        pwr_mgr_sw_cfg0.reg.Sw_Ctl_##type = val; \
    }while(0)
#define SET_SW_CFG0_3D_REG(type, val) \
    do { \
        SET_SW_CFG0_REG(3d_Centroy_##type, val); \
        SET_SW_CFG0_REG(3d_Slc0_##type, val); \
        SET_SW_CFG0_REG(3d_Slc1_##type, val); \
    } while (0)

#define SET_SW_CFG0_VIDEO_REG(type, val) \
    do { \
        SET_SW_CFG0_REG(Vpp_##type, val); \
        SET_SW_CFG0_REG(S3vd0_##type, val); \
        SET_SW_CFG0_REG(S3vd1_##type, val); \
    } while(0)

    SET_SW_CFG0_3D_REG(Clken, 1);
    SET_SW_CFG0_3D_REG(Isoon, 0);
    SET_SW_CFG0_3D_REG(Rston, 1);
    SET_SW_CFG0_3D_REG(Pwron, 1);

    SET_SW_CFG0_VIDEO_REG(Clken, 1);
    SET_SW_CFG0_VIDEO_REG(Isoon, 0);
    SET_SW_CFG0_VIDEO_REG(Rston, 1);
    SET_SW_CFG0_VIDEO_REG(Pwron, 1);

    if (adapter->pm_caps.pwm_manual)
    {
        pwr_mgr_sw_cfg0.reg.Sw_Ctl_Mxu0_Clken = 1;
    }

    if (adapter->pm_caps.gfx_cg_manual || adapter->pm_caps.gfx_pg_manual)
    {
        sw_cfg_en.reg.Sw_Ctl_3d_En = 1;
    }

    if (adapter->pm_caps.vcp0_cg_manual || adapter->pm_caps.vcp0_pg_manual)
    {
        sw_cfg_en.reg.Sw_Ctl_S3vd0_En = 1;
    }
    if (adapter->pm_caps.vcp1_cg_manual || adapter->pm_caps.vcp1_pg_manual)
    {
        sw_cfg_en.reg.Sw_Ctl_S3vd1_En = 1;
    }

    if (adapter->pm_caps.vpp0_cg_manual || adapter->pm_caps.vpp0_pg_manual || adapter->pm_caps.vpp1_cg_manual || adapter->pm_caps.vpp1_pg_manual)
    {
        sw_cfg_en.reg.Sw_Ctl_Vpp_En = 1;
    }

    if (adapter->pm_caps.pwm_manual)
    {
        sw_cfg_en.reg.Sw_Ctl_Mxu0_En = 1;
    }
    zx_memset(pRB, 0, align_rb_size_uint << 2);

    *pRB++ = *(unsigned int*)&pwm_trigger_on;
    *pRB++ = *(unsigned int*)&trigger_on_dw;

    *pRB++ = SEND_INTERNAL_WRITEFENCE_E3K(FENCE_ROUTE_ID_D_FENCE, _RBT_3DFE, HWM_SYNC_KMD_SLOT);
    *pRB++ = SEND_INTERNAL_FENCE_VALUE_E3K(FENCE_VALUE_RESET);
    *pRB++ = SEND_INTERNAL_WAIT_E3K(HWM_SYNC_KMD_SLOT, WAIT_METHOD_EQUAL, FENCE_VALUE_RESET);
    *pRB++ = SEND_INTERNAL_WAIT_MAIN_E3K(HWM_SYNC_KMD_SLOT, WAIT_METHOD_EQUAL, FENCE_VALUE_RESET);
    *pRB++ = SEND_INTERNAL_WRITEFENCE_E3K(FENCE_ROUTE_ID_D_FENCE, _RBT_3DFE, HWM_SYNC_KMD_SLOT);
    *pRB++ = SEND_INTERNAL_FENCE_VALUE_E3K(FENCE_VALUE_RANGESET);
    *pRB++ = SEND_INTERNAL_WAIT_E3K(HWM_SYNC_KMD_SLOT, WAIT_METHOD_EQUAL, FENCE_VALUE_RANGESET);
    *pRB++ = SEND_INTERNAL_WAIT_MAIN_E3K(HWM_SYNC_KMD_SLOT, WAIT_METHOD_EQUAL, FENCE_VALUE_RANGESET);

    *pRB++ = SET_REGISTER_FD_E3K(CSP_GLOBAL_BLOCK, Reg_Pwr_Mgr_Cfg_En_Offset, 1);
    *pRB++ = 0;         //Reg_Pwr_Mgr_Cfg_En_Offset

    *pRB++ = SET_REGISTER_FD_E3K(CSP_GLOBAL_BLOCK, Reg_Pwr_Mgr_Pg_Pd_Misc0_Offset, 12);
    *pRB++ = 0x001f1f1f;         //Reg_Pwr_Mgr_Pg_Pd_Misc0_Offset
    *pRB++ = 0x03020698;         //Reg_Pwr_Mgr_Pg_Pd_Misc1_Offset
    *pRB++ = 0x00000302;         //Reg_Pwr_Mgr_Pg_Pd_Misc2_Offset
    *pRB++ = 0x03020698;         //Reg_Pwr_Mgr_Pg_Pu_Misc0_Offset
    *pRB++ = 0x00000302;         //Reg_Pwr_Mgr_Pg_Pu_Misc1_Offset
    *pRB++ = 0x20401010;         //Reg_Pwr_Mgr_Pg_Pu_Misc2_Offset
    *pRB++ = 0x02000020;         //Reg_Pwr_Mgr_Wait_3d_Cnt0_Offset
    *pRB++ = 0x02000020;         //Reg_Pwr_Mgr_Wait_S3vd_Cnt1_Offset
    *pRB++ = 0x02000020;         //Reg_Pwr_Mgr_Wait_Vpp_Cnt2_Offset
    *pRB++ = 0x00000020;         //Reg_Pwr_Mgr_Wait_Mxu0_Cnt3_Offset
    *pRB++ = sw_cfg_en.uint;     //Reg_Pwr_Mgr_Sw_Cfg_En_Offset
    *pRB++ = pwr_mgr_sw_cfg0.uint;         //Reg_Pwr_Mgr_Sw_Cfg0_Offset

    *pRB++ = SET_REGISTER_FD_E3K(CSP_GLOBAL_BLOCK, Reg_Pwr_Mgr_Cfg_En_Offset, 1);
    *pRB++ = pwr_mgr_cfg_en.uint;         //Reg_Pwr_Mgr_Cfg_En_Offset

    for (i = 0; i < POWER_SKIP_COMMAND_SIZE; i++)
    {
        *pRB++  = 0;
    }

    *pRB++ = *(unsigned int*)&pwm_trigger_off;
    *pRB++ = *(unsigned int*)&trigger_off_dw;

    enginei_release_ring_buffer_space_e3k(engine, align_rb_size_uint);
    return S_OK;
}

int enginei_init_pwm_reg_cne001(engine_e3k_t *engine)
{
    adapter_t            *adapter = engine->sch_mgr->adapter;
    unsigned int        rb_size_uint = 30;
    unsigned int        align_rb_size_uint = util_align(rb_size_uint + POWER_SKIP_COMMAND_SIZE, 16);
    int i;

    unsigned int *pRB = enginei_get_ring_buffer_space_e3k(engine, align_rb_size_uint);

    Reg_Pwr_Mgr_Cfg_En pwr_mgr_cfg_en = {.uint = 0xa007ffff};
    Reg_Pwr_Mgr_Sw_Cfg_En sw_cfg_en = {.uint = 0x0};
    Reg_Pwr_Mgr_Sw_Cfg0 pwr_mgr_sw_cfg0 = {.uint = 0x0};
    Reg_Pwr_Mgr_Sw_Cfg1 pwr_mgr_sw_cfg1 = {.uint = 0x0};
    Cmd_Blk_Cmd_Csp_Indicator       pwm_trigger_on = {0,};
    Cmd_Blk_Cmd_Csp_Indicator_Dword1 trigger_on_dw = {0,};
    Cmd_Blk_Cmd_Csp_Indicator       pwm_trigger_off = {0,};
    Cmd_Blk_Cmd_Csp_Indicator_Dword1 trigger_off_dw = {0,};

    zx_memset(pRB, 0, (align_rb_size_uint << 2));

    pwm_trigger_on.Major_Opcode = CSP_OPCODE_Blk_Cmd_Csp_Indicator;
    pwm_trigger_on.Block_Id = CSP_GLOBAL_BLOCK;
    pwm_trigger_on.Type = BLOCK_COMMAND_CSP_TYPE_INDICATOR;
    pwm_trigger_on.Info = BLK_CMD_CSP_INDICATOR_INFO_3D_MODE;
    pwm_trigger_on.Dwc = 1;

    pwm_trigger_off.Major_Opcode = CSP_OPCODE_Blk_Cmd_Csp_Indicator;
    pwm_trigger_off.Block_Id = CSP_GLOBAL_BLOCK;
    pwm_trigger_off.Type = BLOCK_COMMAND_CSP_TYPE_INDICATOR;
    pwm_trigger_off.Info = BLK_CMD_CSP_INDICATOR_INFO_OFF_MODE;
    pwm_trigger_off.Dwc = 1;

    trigger_on_dw.Slice_Mask = adapter->hw_caps.chip_slice_mask;
    trigger_off_dw.Slice_Mask = adapter->hw_caps.chip_slice_mask;
// set auto mode register
#define SET_CFG_EN_REG_CNE001(type1, type2) \
    do { \
        pwr_mgr_cfg_en.reg_cne001.En_Pwrgate_##type1 = adapter->pm_caps.type2##_pg_auto; \
        pwr_mgr_cfg_en.reg_cne001.En_Clkgate_##type1 = adapter->pm_caps.type2##_cg_auto; \
    }while(0)

    SET_CFG_EN_REG_CNE001(3d_Central, gfx);
    SET_CFG_EN_REG_CNE001(3d_Slc0, gfx);
    SET_CFG_EN_REG_CNE001(3d_Slc1, gfx);

    SET_CFG_EN_REG_CNE001(Vcp, vcp0);
    SET_CFG_EN_REG_CNE001(Vdp0, vcp1);
    SET_CFG_EN_REG_CNE001(Vdp1, vcp2);
    /*SET_CFG_EN_REG_CNE001(Vdp2, vcp);*/

    SET_CFG_EN_REG_CNE001(Vpp0, vpp0);
    SET_CFG_EN_REG_CNE001(Vpp1, vpp1);
// set manual mode register
#define SET_SW_CFG0_REG_CNE001(type, val) \
    do { \
        pwr_mgr_sw_cfg0.reg_cne001.Sw_Ctl_##type = val; \
    }while(0)
#define SET_SW_CFG1_REG_CNE001(type, val) \
    do { \
        pwr_mgr_sw_cfg1.reg_cne001.Sw_Ctl_##type = val; \
    }while(0)
#define SET_SW_CFG0_3D_REG_CNE001(type, val) \
    do { \
        SET_SW_CFG0_REG_CNE001(3d_Centroy_##type, val); \
        SET_SW_CFG0_REG_CNE001(3d_Slc0_##type, val); \
        SET_SW_CFG0_REG_CNE001(3d_Slc1_##type, val); \
    } while (0)
#define SET_SW_CFG1_3D_REG_CNE001(type, val) \
    do { \
        SET_SW_CFG1_REG_CNE001(3d_Centroy_##type, val); \
        SET_SW_CFG1_REG_CNE001(3d_Slc0_##type, val); \
        SET_SW_CFG1_REG_CNE001(3d_Slc1_##type, val); \
    } while (0)

#define SET_SW_CFG0_VIDEO_REG_CNE001(type, val) \
    do { \
        SET_SW_CFG0_REG_CNE001(Vcp_##type, val); \
        SET_SW_CFG0_REG_CNE001(Vdp0_##type, val); \
        SET_SW_CFG0_REG_CNE001(Vdp1_##type, val); \
        /*SET_SW_CFG0_REG_CNE001(Vdp2_##type, val);*/ \
        SET_SW_CFG0_REG_CNE001(Vpp0_##type, val); \
        SET_SW_CFG0_REG_CNE001(Vpp1_##type, val); \
    } while(0)
#define SET_SW_CFG1_VIDEO_REG_CNE001(type, val) \
    do { \
        SET_SW_CFG1_REG_CNE001(Vcp_##type, val); \
        SET_SW_CFG1_REG_CNE001(Vdp0_##type, val); \
        SET_SW_CFG1_REG_CNE001(Vdp1_##type, val); \
        /*SET_SW_CFG1_REG_CNE001(Vdp2_##type, val);*/ \
        SET_SW_CFG1_REG_CNE001(Vpp0_##type, val); \
        SET_SW_CFG1_REG_CNE001(Vpp1_##type, val); \
    } while(0)

    SET_SW_CFG0_3D_REG_CNE001(Clken, 1);
    SET_SW_CFG0_3D_REG_CNE001(Isoon, 0);
    SET_SW_CFG0_3D_REG_CNE001(Rston, 1);
    SET_SW_CFG1_3D_REG_CNE001(Pwron, 1);

    SET_SW_CFG0_VIDEO_REG_CNE001(Clken, 1);
    SET_SW_CFG0_VIDEO_REG_CNE001(Isoon, 0);
    SET_SW_CFG0_VIDEO_REG_CNE001(Rston, 1);
    SET_SW_CFG1_VIDEO_REG_CNE001(Pwron, 1);

    if (adapter->pm_caps.gfx_cg_manual)
    {
        sw_cfg_en.reg_cne001.Sw_Ctl_3d_En = 1;
    }

    if (adapter->pm_caps.vcp0_cg_manual || adapter->pm_caps.vcp1_cg_manual || adapter->pm_caps.vcp2_cg_manual)
    {
        sw_cfg_en.reg_cne001.Sw_Ctl_Vcp_En = 1;
    }

    if (adapter->pm_caps.vcp0_cg_manual)
    {
        sw_cfg_en.reg_cne001.Sw_Ctl_Vdp0_En = 1;
    }
    if (adapter->pm_caps.vcp1_cg_manual)
    {
        sw_cfg_en.reg_cne001.Sw_Ctl_Vdp1_En = 1;
    }
    if (adapter->pm_caps.vcp2_cg_manual)
    {
        //sw_cfg_en.reg_cne001.Sw_Ctl_Vdp2_En = 1;
    }

    if (adapter->pm_caps.vpp0_cg_manual)
    {
        sw_cfg_en.reg_cne001.Sw_Ctl_Vpp0_En = 1;
    }

    if (adapter->pm_caps.vpp1_cg_manual)
    {
        sw_cfg_en.reg_cne001.Sw_Ctl_Vpp1_En = 1;
    }

    pwr_mgr_cfg_en.reg_cne001.Be_Mode = adapter->pm_caps.be_mode & 0x1;

    *pRB++ = *(unsigned int*)&pwm_trigger_on;
    *pRB++ = *(unsigned int*)&trigger_on_dw;

    *pRB++ = SEND_INTERNAL_WRITEFENCE_E3K(FENCE_ROUTE_ID_D_FENCE, _RBT_3DFE, HWM_SYNC_KMD_SLOT);
    *pRB++ = SEND_INTERNAL_FENCE_VALUE_E3K(FENCE_VALUE_RESET);
    *pRB++ = SEND_INTERNAL_WAIT_E3K(HWM_SYNC_KMD_SLOT, WAIT_METHOD_EQUAL, FENCE_VALUE_RESET);
    *pRB++ = SEND_INTERNAL_WAIT_MAIN_E3K(HWM_SYNC_KMD_SLOT, WAIT_METHOD_EQUAL, FENCE_VALUE_RESET);
    *pRB++ = SEND_INTERNAL_WRITEFENCE_E3K(FENCE_ROUTE_ID_D_FENCE, _RBT_3DFE, HWM_SYNC_KMD_SLOT);
    *pRB++ = SEND_INTERNAL_FENCE_VALUE_E3K(FENCE_VALUE_RANGESET);
    *pRB++ = SEND_INTERNAL_WAIT_E3K(HWM_SYNC_KMD_SLOT, WAIT_METHOD_EQUAL, FENCE_VALUE_RANGESET);
    *pRB++ = SEND_INTERNAL_WAIT_MAIN_E3K(HWM_SYNC_KMD_SLOT, WAIT_METHOD_EQUAL, FENCE_VALUE_RANGESET);

    *pRB++ = SET_REGISTER_FD_E3K(CSP_GLOBAL_BLOCK, Reg_Pwr_Mgr_Cfg_En_Offset, 1);
    *pRB++ = 0;         //Reg_Pwr_Mgr_Cfg_En_Offset

    *pRB++ = SET_REGISTER_FD_E3K(CSP_GLOBAL_BLOCK, Reg_Pwr_Mgr_Pg_Pd_Misc0_Offset, 13);
    *pRB++ = 0x001f1f1f;         //Reg_Pwr_Mgr_Pg_Pd_Misc0_Offset
    *pRB++ = 0x03020698;         //Reg_Pwr_Mgr_Pg_Pd_Misc1_Offset
    *pRB++ = 0x00000302;         //Reg_Pwr_Mgr_Pg_Pd_Misc2_Offset
    *pRB++ = 0x03020698;         //Reg_Pwr_Mgr_Pg_Pu_Misc0_Offset
    *pRB++ = 0x00000302;         //Reg_Pwr_Mgr_Pg_Pu_Misc1_Offset
    *pRB++ = 0x20401010;         //Reg_Pwr_Mgr_Pg_Pu_Misc2_Offset
    *pRB++ = 0x02000020;         //Reg_Pwr_Mgr_Wait_3d_Cnt0_Offset
    *pRB++ = 0x02000020;         //Reg_Pwr_Mgr_Wait_S3vd_Cnt1_Offset
    *pRB++ = 0x02000020;         //Reg_Pwr_Mgr_Wait_Vpp_Cnt2_Offset
    *pRB++ = 0x00000020;         //Reg_Pwr_Mgr_Wait_Mxu0_Cnt3_Offset
    *pRB++ = sw_cfg_en.uint;     //Reg_Pwr_Mgr_Sw_Cfg_En_Offset
    *pRB++ = pwr_mgr_sw_cfg0.uint;         //Reg_Pwr_Mgr_Sw_Cfg0_Offset
    *pRB++ = pwr_mgr_sw_cfg1.uint;         //Reg_Pwr_Mgr_Sw_Cfg1_Offset

    *pRB++ = SET_REGISTER_FD_E3K(CSP_GLOBAL_BLOCK, Reg_Pwr_Mgr_Cfg_En_Offset, 1);
    *pRB++ = pwr_mgr_cfg_en.uint;         //Reg_Pwr_Mgr_Cfg_En_Offset

    for (i = 0; i < POWER_SKIP_COMMAND_SIZE; i++)
    {
        *pRB++  = 0;
    }

    *pRB++ = *(unsigned int*)&pwm_trigger_off;
    *pRB++ = *(unsigned int*)&trigger_off_dw;


    enginei_release_ring_buffer_space_e3k(engine, align_rb_size_uint);
    return S_OK;
}

int enginei_init_pwm_reg_e3k(engine_e3k_t *engine)
{
    adapter_t            *adapter = engine->sch_mgr->adapter;

    if (adapter->chip_id == CHIP_CHX004)
    {
        enginei_init_pwm_reg_004(engine);
    }
    else if (adapter->chip_id == CHIP_CNE001)
    {
        enginei_init_pwm_reg_cne001(engine);
    }

    return S_OK;
}

static int enginei_restore_3dblt_data_e3k(engine_gfx_e3k_t *engine)
{
    engine_share_e3k_t *share = engine->common.share;

    if (!share->_3dblt_data_shadow)
        return S_OK;

    zx_memcpy(share->_3dblt_data_cpu_va, share->_3dblt_data_shadow, share->_3dblt_data_size);

    return S_OK;
}

int engine_gfx_low_init_e3k(adapter_t *adapter, vidsch_mgr_t *sch_mgr)
{
    vidmm_segment_memory_t   *pcie_reserved_memory     = sch_mgr->pcie_reserved_memory;
    vidmm_segment_memory_t   *local_reserved_memory    = sch_mgr->local_reserved_memory;
    unsigned long long        pcie_reserved_gpu_va     = 0;
    unsigned char            *pcie_reserved_cpu_va     = NULL;
    unsigned long long        local_reserved_gpu_va    = 0;
    unsigned char            *local_reserved_cpu_va    = NULL;
    unsigned int             hw_ctx_buffer_size = CONTEXT_BUFFER_SIZE_CHX004;

    unsigned int              current_local_offset     = 0;
    unsigned int              current_pcie_offset      = 0;
    unsigned int              data_size                = 0;

    unsigned long long        local_gpu_va_tmp  = 0;
    unsigned long long        pcie_gpu_va_tmp   = 0;

    engine_share_e3k_t       *share = NULL;
    int result = S_OK;

    engine_gfx_e3k_t *engine = zx_calloc(sizeof(engine_gfx_e3k_t));
    if(engine == NULL)
    {
        result = E_OUTOFMEMORY;
        return result;
    }

    pcie_reserved_gpu_va       = pcie_reserved_memory->gpu_va;
    pcie_reserved_cpu_va       = pcie_reserved_memory->krnl_cpu_vma->virt_addr;

    local_reserved_gpu_va       = local_reserved_memory->gpu_va;
    local_reserved_cpu_va       = local_reserved_memory->krnl_cpu_vma->virt_addr;

    //zx_info("reserve pcie va:%llx, local va:%llx\n", pcie_reserved_gpu_va, local_reserved_gpu_va);
    enginei_common_init_e3k(sch_mgr, &engine->common, &current_local_offset, adapter->hw_caps.local_only ? &current_local_offset : &current_pcie_offset);

    share = engine->common.share;

    local_gpu_va_tmp = local_reserved_gpu_va + current_local_offset;
//    pcie_gpu_va_tmp  = pcie_reserved_gpu_va + current_pcie_offset;

    current_local_offset = util_align(local_gpu_va_tmp,CONTEXT_BUFFER_ALIGN) -local_reserved_gpu_va;

    share->context_buf_gpu_va       =
    engine->context_buffer_gpu_va   = local_reserved_gpu_va + current_local_offset;
    engine->context_buffer_cpu_va   = local_reserved_cpu_va + current_local_offset;

    current_local_offset += hw_ctx_buffer_size;//only reserve one ctx buffer for low

    local_gpu_va_tmp = local_reserved_gpu_va + current_local_offset;
    current_local_offset = util_align(local_gpu_va_tmp,CONTEXT_BUFFER_ALIGN) -local_reserved_gpu_va;

    share->context_buf_gpu_va_for_vk       =
    engine->context_buffer_gpu_va_for_vk   = local_reserved_gpu_va + current_local_offset;
    engine->context_buffer_cpu_va_for_vk   = local_reserved_cpu_va + current_local_offset;
    current_local_offset += hw_ctx_buffer_size;

    local_gpu_va_tmp = local_reserved_gpu_va + current_local_offset;
    current_local_offset = util_align(local_gpu_va_tmp,0x1000) -local_reserved_gpu_va;

    share->begin_end_buffer_size   = BEGIN_END_BUF_SIZE_E3K;
    share->begin_end_buffer_gpu_va  = local_reserved_gpu_va + current_local_offset;
    share->begin_end_buffer_cpu_va  = local_reserved_cpu_va + current_local_offset;
    current_local_offset   += BEGIN_END_BUF_SIZE_E3K;

    share->flush_fifo_buffer = vidmm_create_segment_memory(adapter, adapter->hw_caps.local_only ?
        SEGMENT_ID_LOCAL_ELT3K : (adapter->hw_caps.snoop_only ? SEGMENT_ID_PCIE_SNOOPABLE_ELT3K :
        SEGMENT_ID_PCIE_UNSNOOPABLE_ELT3K), 0x1000, 1, 1);

    vidmm_lock_segment_memory(adapter, share->flush_fifo_buffer);

    enginei_init_gfx_begin_end_commands_e3k(engine);

    enginei_init_gfx_submit_rb_cmd_e3k(engine);

    /* 3d blt Initialize */
    local_gpu_va_tmp = local_reserved_gpu_va + current_local_offset;
    current_local_offset = util_align(local_gpu_va_tmp, 0x100) -local_reserved_gpu_va;

    share->_3dblt_data_lock = zx_create_mutex();
    share->_3dblt_data_size = 0;
    share->_3dblt_data_shadow = NULL;

    share->_3dblt_data_gpu_va = local_reserved_gpu_va + current_local_offset;
    share->_3dblt_data_cpu_va = local_reserved_cpu_va + current_local_offset;

    current_local_offset += _3DBLT_DATA_RESERVE_SIZE;

    engine->is_high_engine = 0;

    if(!adapter->hw_caps.video_only)
    {
        enginei_init_hardware_context_e3k(engine);
    }

    enginei_common_share_init_e3k(&engine->common);

    if (adapter->pwm_level.EnableClockGating)
    {
        enginei_init_pwm_reg_e3k(&engine->common);
    }

    zx_assert((current_local_offset <= local_reserved_memory->size) &&
        (current_pcie_offset <= pcie_reserved_memory->size), "size of reserved local or pcie not enough");

    return result;
}

int engine_gfx_high_init_e3k(adapter_t *adapter, vidsch_mgr_t *sch_mgr)
{
    vidmm_segment_memory_t  *local_reserved_memory = sch_mgr->local_reserved_memory;
    vidmm_segment_memory_t  *pcie_reserved_memory  = sch_mgr->pcie_reserved_memory;
    engine_gfx_e3k_t       *engine                = zx_calloc(sizeof(engine_gfx_e3k_t));
    unsigned int     current_pcie_offset   = 0;
    unsigned int     current_local_offset    = 0;
    unsigned long long        local_reserved_gpu_va    = 0;
    unsigned char            *local_reserved_cpu_va    = NULL;
    unsigned int     hw_ctx_buffer_size = CONTEXT_BUFFER_SIZE_CHX004;

    local_reserved_gpu_va       = local_reserved_memory->gpu_va;
    local_reserved_cpu_va       = local_reserved_memory->krnl_cpu_vma->virt_addr;

    if(engine == NULL)
    {
        zx_error("%s, out of mem.\n", __func__);
        zx_assert(0, "engine_gfx_high_init");
        return S_OK;
    }

    current_local_offset = util_align(local_reserved_gpu_va, CONTEXT_BUFFER_ALIGN) -local_reserved_gpu_va;

    engine->context_buffer_gpu_va   = local_reserved_gpu_va + current_local_offset;
    engine->context_buffer_cpu_va   = local_reserved_cpu_va + current_local_offset;

    current_local_offset += hw_ctx_buffer_size;

    enginei_common_init_e3k(sch_mgr, &engine->common, &current_local_offset, &current_pcie_offset);

    engine->is_high_engine = 1;

    if (!adapter->hw_caps.video_only)
    {
        enginei_init_hardware_context_e3k(engine);
    }

    zx_assert((current_local_offset <= local_reserved_memory->size) &&
        (current_pcie_offset <= pcie_reserved_memory->size), "size of reserved local or pcie not enough");

    return S_OK;
}

int engine_cs_init_e3k(adapter_t *adapter, vidsch_mgr_t *sch_mgr)
{
    vidmm_segment_memory_t  *pcie_reserved_memory  = sch_mgr->pcie_reserved_memory;
    engine_cs_e3k_t       *engine                = zx_calloc(sizeof(engine_cs_e3k_t));
    unsigned int     current_pcie_offset   = 0;
    unsigned int     current_local_offset    = 0;

    if(engine == NULL)
    {
        zx_error("%s, out of mem.\n", __func__);
        zx_assert(0, "engine_cs_init");
    }

    enginei_common_init_e3k(sch_mgr, engine, &current_local_offset, &current_pcie_offset);

    return S_OK;
}

int engine_vcp_init_e3k(adapter_t *adapter, vidsch_mgr_t *sch_mgr)
{
    vidmm_segment_memory_t  *pcie_reserved_memory  = sch_mgr->pcie_reserved_memory;
    engine_vcp_e3k_t       *engine                = zx_calloc(sizeof(engine_vcp_e3k_t));
    unsigned int     current_pcie_offset   = 0;
    unsigned int     current_local_offset    = 0;
    if(adapter->hw_caps.local_only || !adapter->ctl_flags.share_space_cover_local){
        current_pcie_offset += VIDEO_BRIDGE_BUF_SIZE;
    }
    if(engine == NULL)
    {
        zx_error("%s, out of mem.\n", __func__);
        zx_assert(0, "engine_vcp_init");
    }

    enginei_common_init_e3k(sch_mgr, engine, &current_local_offset, &current_pcie_offset);

    return S_OK;
}

int engine_vpp_init_e3k(adapter_t *adapter, vidsch_mgr_t *sch_mgr)
{
    vidmm_segment_memory_t  *pcie_reserved_memory  = sch_mgr->pcie_reserved_memory;
    engine_vpp_e3k_t       *engine                = zx_calloc(sizeof(engine_vpp_e3k_t));
    unsigned int     current_pcie_offset   = 0;
    unsigned int     current_local_offset    = 0;

    if(engine == NULL)
    {
        zx_error("%s, out of mem.\n", __func__);
        zx_assert(0, "engine_vpp_init");
    }

    enginei_common_init_e3k(sch_mgr, engine, &current_local_offset, &current_pcie_offset);

    return S_OK;
}


int engine_save_e3k(vidsch_mgr_t *vidsch)
{
    engine_gfx_e3k_t    *engine  = vidsch->private_data;
    engine_share_e3k_t  *share   = engine->common.share;

    return S_OK;
}

static int engine_gfx_low_save_e3k(vidsch_mgr_t *vidsch)
{
    adapter_t *adapter = vidsch->adapter;
    engine_gfx_e3k_t    *engine  = vidsch->private_data;
    engine_share_e3k_t  *share   = engine->common.share;

    if (share->ring_buffer_for_hang)
    {
        share->ring_buffer_for_hang_backup = zx_malloc(share->ring_buffer_for_hang->size);
        zx_memcpy(share->ring_buffer_for_hang_backup,
                share->ring_buffer_for_hang->krnl_cpu_vma->virt_addr,
                share->ring_buffer_for_hang->size);
    }

    return 0;
}

void engine_restore_e3k(vidsch_mgr_t *vidsch, unsigned int pm)
{
    engine_e3k_t *engine = vidsch->private_data;

    enginei_enable_ring_buffer_e3k(engine);
}


void engine_gfx_low_restore_e3k(vidsch_mgr_t *sch_mgr, unsigned int pm)
{
    adapter_t *adapter = sch_mgr->adapter;
    engine_gfx_e3k_t    *engine  = sch_mgr->private_data;
    engine_share_e3k_t  *share   = engine->common.share;
    unsigned int size;

    vidschi_reset_adapter_e3k(adapter);

    vidschi_init_hw_settings_e3k(sch_mgr->adapter);

    enginei_enable_ring_buffer_e3k(&engine->common);

    enginei_init_gfx_begin_end_commands_e3k(engine);

    enginei_restore_3dblt_data_e3k(engine);

    if (!adapter->hw_caps.video_only)
    {
        enginei_init_hardware_context_e3k(engine);
    }

    if (adapter->pwm_level.EnableClockGating)
    {
        enginei_init_pwm_reg_e3k(&engine->common);
        zx_udelay(1000*20);
    }

    if (share->ring_buffer_for_hang_backup)
    {
        zx_memcpy(share->ring_buffer_for_hang->krnl_cpu_vma->virt_addr,
                share->ring_buffer_for_hang_backup,
                share->ring_buffer_for_hang->size);

        zx_free(share->ring_buffer_for_hang_backup);
        share->ring_buffer_for_hang_backup = NULL;
    }
}

void engine_gfx_low_restore_dvfs_e3k(vidsch_mgr_t *sch_mgr)
{
    adapter_t *adapter = sch_mgr->adapter;
    engine_gfx_e3k_t    *engine  = sch_mgr->private_data;
    engine_share_e3k_t  *share   = engine->common.share;

    vidschi_init_dvfs_setting_004(sch_mgr->adapter);
    zx_msleep(20);
}

void engine_gfx_high_restore_e3k(vidsch_mgr_t *sch_mgr, unsigned int pm)
{
    engine_gfx_e3k_t  *engine = sch_mgr->private_data;
    adapter_t *adapter = sch_mgr->adapter;

    enginei_enable_ring_buffer_e3k(&engine->common);

    if (!adapter->hw_caps.video_only)
    {
        enginei_init_hardware_context_e3k(engine);
    }
}

int engine_destroy_e3k(vidsch_mgr_t *sch_mgr)
{
    adapter_t           *adapter      = sch_mgr->adapter;
    engine_e3k_t        *engine       = sch_mgr->private_data;
    int                 result        = S_OK;

    sch_mgr->private_data = NULL;

    if(adapter->private_data)
    {
        engine_share_e3k_t *share = adapter->private_data;

        if (share->dummyfence[0])
        {
            zx_free_pages_memory(adapter->os_device.pdev, share->dummyfence[0]);
            share->dummyfence[0] = NULL;
            share->dummyfence_phy[0] = 0;
        }

        if (share->dummyfence[1])
        {
            zx_free_pages_memory(adapter->os_device.pdev, share->dummyfence[1]);
            share->dummyfence[1] = NULL;
            share->dummyfence_phy[1] = 0;
        }
    }

    if(engine->share != NULL)
    {
        engine_share_e3k_t *share = engine->share;
        share->ref_cnt--;

        if(share->ref_cnt == 0)
        {
            if (share->flush_fifo_buffer)
            {
                vidmm_destroy_segment_memory(adapter, share->flush_fifo_buffer);
            }

            if (share->_3dblt_data_lock)
            {
                zx_destroy_mutex(share->_3dblt_data_lock);
            }

            zx_destroy_spinlock(share->lock);
            share->lock = NULL;
            zx_free(share);
            engine->share = NULL;
        }
    }

    if(adapter->hang_dump_lock)
    {
       zx_destroy_mutex(adapter->hang_dump_lock);
       adapter->hang_dump_lock = NULL;
    }

    zx_free(engine);
    sch_mgr->private_data = NULL;

    return result;
}

unsigned long long engine_update_fence_id_e3k(vidsch_mgr_t *sch_mgr)
{
    engine_e3k_t   *engine = sch_mgr->private_data;
    volatile unsigned long long ret = *(volatile unsigned long long*)(engine->fence_buffer_cpu_va);

    return ret;
}

int engine_cil2_misc_e3k(vidsch_mgr_t *sch_mgr, krnl_cil2_misc_t *misc)
{
    int                 hRet = S_OK;
    engine_e3k_t        *engine = sch_mgr->private_data;
    engine_share_e3k_t *share = engine->share;
    zx_cil2_misc_t      *zx_misc = misc->zx_misc;

    switch(zx_misc->op_code)
    {
    case ZX_MISC_GET_3DBLT_CODE:
        {
            zx_mutex_lock(share->_3dblt_data_lock);
            zx_misc->get_3dblt_code.size = share->_3dblt_data_size;
            zx_misc->get_3dblt_code.capacity = _3DBLT_DATA_RESERVE_SIZE;
            zx_misc->get_3dblt_code.base = share->_3dblt_data_gpu_va;
            zx_mutex_unlock(share->_3dblt_data_lock);
            //zx_info("%lld:GET_3DBLT_CODE: size=%d base=%d\n",
            //        zx_get_current_pid(), zx_misc->get_3dblt_code.size, zx_misc->get_3dblt_code.base);
            break;
        }
    case ZX_MISC_SET_3DBLT_CODE:
        {
            //zx_info("%lld:SET_3DBLT_CODE: oldsize=%d, va=%p, size=%d\n",
            //        zx_get_current_pid(), share->_3dblt_data_size, share->_3dblt_data_cpu_va, zx_misc->set_3dblt_code.size);

            zx_mutex_lock(share->_3dblt_data_lock);
            if (share->_3dblt_data_size == 0)
            {
                share->_3dblt_data_size = zx_misc->set_3dblt_code.size;
                share->_3dblt_data_shadow = zx_malloc(share->_3dblt_data_size);
                zx_copy_from_user(share->_3dblt_data_shadow, ptr64_to_ptr(zx_misc->set_3dblt_code.data), share->_3dblt_data_size);

                zx_memcpy(share->_3dblt_data_cpu_va, share->_3dblt_data_shadow, share->_3dblt_data_size);

            }
            zx_mutex_unlock(share->_3dblt_data_lock);
        }
        break;
    case ZX_MISC_VIDEO_FENCE_GET:
        {
            if (zx_misc->video_fence_get.index < 8)
            {
                zx_spin_lock(share->lock);
                zx_misc->video_fence_get.fence = share->internal_fence_value[zx_misc->video_fence_get.index]++;
                zx_spin_unlock(share->lock);
            }
            else
            {
                hRet = E_FAIL;
                zx_error("VIDEO_FENCE_GET wrong index value: %d.\n", zx_misc->video_fence_get.index);
            }
        }
        break;
    case ZX_MISC_VIDEO_FENCE_CLEAR:
        {
            if (zx_misc->video_fence_clear.index < 8)
            {
                zx_spin_lock(share->lock);
                share->internal_fence_value[zx_misc->video_fence_clear.index] = 0;
                zx_spin_unlock(share->lock);
            }
            else
            {
                hRet = E_FAIL;
                zx_error("VIDEO_FENCE_CLEAR wrong index value: %d.\n", zx_misc->video_fence_clear.index);
            }
        }
        break;
    case ZX_MISC_MAP_GPUVA:
        zx_error("no implements yet. \n");
        break;
    default:
        hRet = E_INVALIDARG;
    }

    return hRet;
}

void vidsch_dump_debugbus_e3k(struct os_printer *p, adapter_t *adapter)
{
    unsigned int     engine_status_offset     = MMIO_CSP_START_ADDRESS + Reg_Block_Busy_Bits_Top_Offset*4;
    EngineSatus_e3k  engine_status;
    unsigned int     *status = (unsigned int *)&engine_status;
    int i;

    for( i = 0; i <7; i++)
    {
        *(status + i) = zx_read32(adapter->mmio + MMIO_CSP_START_ADDRESS + (Reg_Block_Busy_Bits_Top_Offset + i)*4);
    }

    zx_printf(p, "ChipSliceMask: %08x\n", adapter->hw_caps.chip_slice_mask);
    zx_printf(p, "Engine Hang and Status: %08x,%08x,%08x,%08x,%08x,%08x,%08x.\n",
        engine_status.Top.uint, engine_status.Gpc0_0.uint, engine_status.Gpc0_1.uint,
        engine_status.Gpc1_0.uint, engine_status.Gpc1_1.uint, engine_status.Gpc2_0.uint, engine_status.Gpc2_1.uint);

    if (adapter->pm_caps.pwm_mode)
    {
        if (adapter->chip_id == CHIP_CHX004)
        {
            zx_printf(p, "Power Status: %08x\n",
                zx_read32(adapter->mmio + MMIO_CSP_START_ADDRESS + Reg_Pwr_Mgr_Status1_Offset * 4));
        }
        else if (adapter->chip_id == CHIP_CNE001)
        {
            zx_printf(p, "Power Status1: %08x\n",
                zx_read32(adapter->mmio + MMIO_CSP_START_ADDRESS + Reg_Pwr_Mgr_Status1_Offset_Cne001 * 4));
            zx_printf(p, "Power Status2: %08x\n",
                zx_read32(adapter->mmio + MMIO_CSP_START_ADDRESS + Reg_Pwr_Mgr_Status2_Offset * 4));
        }
    }
    vidsch_display_debugbus_info_e3k(adapter, p, FALSE);
}

int engine_reset_hw_e3k(vidsch_mgr_t *sch_mgr)
{
    adapter_t           *adapter = sch_mgr->adapter;
    EngineSatus_e3k     engine_status = {0};
    engine_e3k_t        *engine;
    unsigned int        *status =(unsigned int*) &engine_status;
    unsigned int     last_returned_fence_id, idx;
    unsigned int        index   = 0;
    unsigned int        fence_back_engine_count = 0;
    int result = S_OK;

    for( idx = 0; idx < 7; idx++)
    {
        *(status + idx) = zx_read32(adapter->mmio + MMIO_CSP_START_ADDRESS + (Reg_Block_Busy_Bits_Top_Offset + idx)*4);
    }
    zx_info("<<----^^^^^^^^^-----[Before reset hw] Current HW status -----^^^^^^^^^^^--->>\n");
    zx_info("current submit engine is 0x%x, Engine Hang and Status: %08x,%08x,%08x,%08x,%08x,%08x,%08x\n",
        sch_mgr->engine_index, engine_status.Top.uint,engine_status.Gpc0_0.uint,engine_status.Gpc0_1.uint,
        engine_status.Gpc1_0.uint,engine_status.Gpc1_1.uint,engine_status.Gpc2_0.uint,engine_status.Gpc2_1.uint);

    if(adapter->ctl_flags.hang_dump)
    {
        vidsch_pre_save_misc_e3k(adapter, sch_mgr, SAVE_AFTER_POSTHANG);//save engine status.
    }

    for(index=0; index< adapter->active_engine_count; index++)
    {
        unsigned int     last_returned_fence_id;
        vidsch_mgr_t     *tmp_sch_mgr = adapter->sch_mgr[index];

        if(tmp_sch_mgr == NULL)
        {
            fence_back_engine_count++;
            continue;
        }

        engine = tmp_sch_mgr->private_data;
        last_returned_fence_id = *(volatile unsigned int *)engine->fence_buffer_cpu_va;

        if(last_returned_fence_id == (unsigned int)tmp_sch_mgr->last_send_fence_id)
        {
            //this engine is running normally
            fence_back_engine_count++;
        }
    }

    /* return fake engine hang */
    if(engine_status.Top.uint == 0 && fence_back_engine_count ==  adapter->active_engine_count)
    {
        zx_error("fake hang detected\n");
        return E_FAIL;
    }

    /* update fence id skip the dead dma */
    for(idx = 0; idx < sizeof(adapter->sch_mgr)/sizeof(adapter->sch_mgr[0]); idx++)
    {
        vidsch_mgr_t *tmp_sch_mgr = adapter->sch_mgr[idx];

        if(tmp_sch_mgr)
        {
            engine_e3k_t    *engine_tmp  = tmp_sch_mgr->private_data;

            *(volatile unsigned long long*)engine_tmp->fence_buffer_cpu_va = tmp_sch_mgr->last_send_fence_id & 0xFFFFFFFF;
#ifndef ZX_VMI_MODE
            tmp_sch_mgr->chip_func->restore(tmp_sch_mgr, 0);
#endif
            tmp_sch_mgr->init_submit = TRUE;
        }
    }

    return S_OK;
}

#ifdef ZX_HW_NULL
void engine_set_fence_id_e3k(vidsch_mgr_t *sch_mgr, unsigned long long fence_id)
{
    engine_e3k_t   *engine = sch_mgr->private_data;
    *(engine->fence_buffer_cpu_va) = fence_id;
    zx_info("fake set fence id: %lld, at engine %d\n", fence_id, sch_mgr->engine_index);
}
#endif

void vidsch_get_timestamp_e3k(adapter_t *adapter, unsigned long long *time_stamp)
{
    if (adapter->chip_id == CHIP_CHX004)
    {
        *time_stamp = zx_read64(adapter->mmio + MMIO_CSP_START_ADDRESS  + Reg_Csp_Ref_Total_Gpu_Timestamp_Offset * 4);
    }
    else
    {
        *time_stamp = zx_read64(adapter->mmio + MMIO_CSP_START_ADDRESS  + Reg_Csp_Ref_Total_Gpu_Timestamp_Offset_CNE001 * 4);
    }
}

void vidsch_get_set_reg_e3k(adapter_t *adapter, zx_query_info_t *info)
{
    unsigned long long time_stamp_hi, time_stamp_lo;
    unsigned int *buf;
    int i=0;

    switch(info->type)
    {
        case ZX_QUERY_GPU_TIME_STAMP:
            info->value64 = zx_read64(adapter->mmio + MMIO_CSP_START_ADDRESS  + (adapter->chip_id == CHIP_CHX004 ?
                Reg_Csp_Ref_Total_Gpu_Timestamp_Offset : Reg_Csp_Ref_Total_Gpu_Timestamp_Offset_CNE001)* 4);
            break;

        case ZX_QUERY_REGISTER_U32:
            buf=zx_calloc(info->buf_len*sizeof(unsigned int));
            if(buf)
            {
                for(i=0;i<info->buf_len;i++)
                {
                    *(buf+i)=zx_read32(adapter->mmio+info->argu+i*sizeof(unsigned int));
                }
            }
            zx_copy_to_user(ptr64_to_ptr(info->buf), buf, info->buf_len*sizeof(unsigned int));
            zx_free(buf);
            break;

        case ZX_SET_REGISTER_U32:
            zx_write32(adapter->mmio+info->argu, info->value);
            break;
        default:
            break;
    }
}

unsigned long long vidsch_notify_page_fault_e3k(adapter_t *adapter, int *vm_id)
{
    Reg_Mxu_Page_Fault_Status0 pfStatus0;
    Reg_Mxu_Page_Fault_Status1 pfStatus1;
    unsigned long long pfVa = 0;

    pfStatus0.uint = zx_read32(adapter->mmio + MMIO_MXU_START_ADDRESS + Reg_Mxu_Page_Fault_Status0_Offset * 4);
    pfStatus1.uint = zx_read32(adapter->mmio + MMIO_MXU_START_ADDRESS + Reg_Mxu_Page_Fault_Status1_Offset * 4);
    pfVa           = pfStatus0.reg.Va;
    pfVa           = (pfVa << 32) | pfStatus1.uint;
    *vm_id         = pfStatus0.reg.Proc_Id;

    zx_info("Page fault proc_id:%d, fault_level:%d, Va:0x%llx\n", pfStatus0.reg.Proc_Id, pfStatus0.reg.Fault_Level, pfVa);

    return pfVa;
}

vidsch_chip_func_t engine_gfx_low_chip_func_e3k =
{
    .initialize          = engine_gfx_low_init_e3k,
    .destroy             = engine_destroy_e3k,
    .save                = engine_gfx_low_save_e3k,
    .restore             = engine_gfx_low_restore_e3k,
    .restore_dvfs          = engine_gfx_low_restore_dvfs_e3k,
    .render              = engine_render_e3k,
    .submit              = engine_submit_command_e3k,
    .update_fence_id     = engine_update_fence_id_e3k,
    .set_root_page_table = engine_set_root_page_table_e3k,
    .invalidate_tlb     = engine_invalidate_tlb_command_e3k,
#ifdef ZX_HW_NULL
    .set_fence_id        = engine_set_fence_id_e3k,
#endif
    .cil2_misc          = engine_cil2_misc_e3k,
    .get_timestamp      = vidsch_get_timestamp_e3k,

    .power_clock        = vidsch_power_clock_on_off_e3k,
};

vidsch_chip_func_t engine_gfx_high_chip_func_e3k =
{
    .initialize         = engine_gfx_high_init_e3k,
    .destroy            = engine_destroy_e3k,
    //.save                = engine_save_e3k,
    .restore            = engine_gfx_high_restore_e3k,
    .update_fence_id    = engine_update_fence_id_e3k,

    .power_clock        = vidsch_power_clock_on_off_e3k,
};

vidsch_chip_func_t engine_cs_high_chip_func_e3k =
{
    .initialize         = engine_cs_init_e3k,
    .destroy            = engine_destroy_e3k,
//    .save             = engine_save_e3k,
    .restore            = engine_restore_e3k,
    .update_fence_id    = engine_update_fence_id_e3k,
};

vidsch_chip_func_t engine_cs_low0_chip_func_e3k =
{
    .initialize         = engine_cs_init_e3k,
    .destroy            = engine_destroy_e3k,
//    .save             = engine_save_e3k,
    .restore            = engine_restore_e3k,
    .update_fence_id    = engine_update_fence_id_e3k,
};

vidsch_chip_func_t engine_cs_low1_chip_func_e3k =
{
    .initialize         = engine_cs_init_e3k,
    .destroy            = engine_destroy_e3k,
//    .save             = engine_save_e3k,
    .restore            = engine_restore_e3k,
    .update_fence_id    = engine_update_fence_id_e3k,
};

vidsch_chip_func_t engine_cs_low2_chip_func_e3k =
{
    .initialize         = engine_cs_init_e3k,
    .destroy            = engine_destroy_e3k,
//    .save             = engine_save_e3k,
    .restore            = engine_restore_e3k,
    .update_fence_id    = engine_update_fence_id_e3k,
};

vidsch_chip_func_t engine_cs_low3_chip_func_e3k =
{
    .initialize         = engine_cs_init_e3k,
    .destroy            = engine_destroy_e3k,
//    .save             = engine_save_e3k,
    .restore            = engine_restore_e3k,
    .update_fence_id    = engine_update_fence_id_e3k,
};

vidsch_chip_func_t engine_vcp_low0_chip_func_e3k =
{
    .initialize         = engine_vcp_init_e3k,
    .destroy            = engine_destroy_e3k,
//    .save             = engine_save_e3k,
    .restore            = engine_restore_e3k,
    .render              = engine_render_e3k,
    .submit              = engine_submit_command_e3k,
    .update_fence_id     = engine_update_fence_id_e3k,
#ifdef ZX_HW_NULL
    .set_fence_id        = engine_set_fence_id_e3k,
#endif

    .power_clock         = vidsch_power_clock_on_off_e3k,
};

vidsch_chip_func_t engine_vcp_low1_chip_func_e3k =
{
    .initialize         = engine_vcp_init_e3k,
    .destroy            = engine_destroy_e3k,
//    .save             = engine_save_e3k,
    .restore            = engine_restore_e3k,
    .render              = engine_render_e3k,
    .submit              = engine_submit_command_e3k,
    .update_fence_id     = engine_update_fence_id_e3k,
#ifdef ZX_HW_NULL
    .set_fence_id        = engine_set_fence_id_e3k,
#endif
    .power_clock        = vidsch_power_clock_on_off_e3k,

};

vidsch_chip_func_t engine_vcp_low2_chip_func_e3k =
{
        .initialize         = engine_vcp_init_e3k,
        .destroy            = engine_destroy_e3k,
//    .save             = engine_save_e3k,
        .restore            = engine_restore_e3k,
        .render              = engine_render_e3k,
        .submit              = engine_submit_command_e3k,
        .update_fence_id     = engine_update_fence_id_e3k,
#ifdef ZX_HW_NULL
        .set_fence_id        = engine_set_fence_id_e3k,
#endif
        .power_clock        = vidsch_power_clock_on_off_e3k,

};

vidsch_chip_func_t engine_vpp_chip_func_e3k =
{
    .initialize         = engine_vpp_init_e3k,
    .destroy            = engine_destroy_e3k,
//    .save             = engine_save_e3k,
    .restore            = engine_restore_e3k,
    .render              = engine_render_e3k,
    .submit              = engine_submit_command_e3k,
    .update_fence_id     = engine_update_fence_id_e3k,
#ifdef ZX_HW_NULL
    .set_fence_id        = engine_set_fence_id_e3k,
#endif
    .power_clock         = vidsch_power_clock_on_off_e3k,
};

vidsch_chip_func_t engine_vpp0_chip_func_e3k =
{
        .initialize         = engine_vpp_init_e3k,
        .destroy            = engine_destroy_e3k,
//    .save             = engine_save_e3k,
        .restore            = engine_restore_e3k,
        .render              = engine_render_e3k,
        .submit              = engine_submit_command_e3k,
        .update_fence_id     = engine_update_fence_id_e3k,
#ifdef ZX_HW_NULL
        .set_fence_id        = engine_set_fence_id_e3k,
#endif
        .power_clock         = vidsch_power_clock_on_off_e3k,
};

vidsch_chip_func_t engine_vpp1_chip_func_e3k =
{
        .initialize         = engine_vpp_init_e3k,
        .destroy            = engine_destroy_e3k,
//    .save             = engine_save_e3k,
        .restore            = engine_restore_e3k,
        .render              = engine_render_e3k,
        .submit              = engine_submit_command_e3k,
        .update_fence_id     = engine_update_fence_id_e3k,
#ifdef ZX_HW_NULL
        .set_fence_id        = engine_set_fence_id_e3k,
#endif
        .power_clock         = vidsch_power_clock_on_off_e3k,
};


vidschedule_chip_func_t vidschedule_chip_func =
{
    .reset_hw          = engine_reset_hw_e3k,
    .dump_hang_info    = vidsch_dump_hang_info_e3k,
    .dump_hang         = vidsch_dump_hang_e3k,
    .dump_debugbus     = vidsch_dump_debugbus_e3k,

    .power_tuning      = vidsch_power_tuning_e3k,
    .disable_dvfs          = vidsch_disable_dvfs_004,
    .get_set_reg           = vidsch_get_set_reg_e3k,
    .notify_page_fault     = vidsch_notify_page_fault_e3k,
};

