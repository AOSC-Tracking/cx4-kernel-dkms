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
#include "vidsch_dfs_e3k.h"
#include "vidsch.h"
#include "vidschi.h"
#include "chip_include_e3k.h"
#include "vidsch_engine_e3k.h"
#include "register_e3k.h"

void vidschi_init_dvfs_setting_004(adapter_t *adapter)
{
    vidsch_mgr_t *sch_mgr = NULL;

    Reg_Reg_Dvfs_Cfg_En         reg_dvfs_cfg_en;
    Reg_Reg_Dvfs_Cfg_Misc0      reg_dvfs_cfg_misc0;

    Reg_Reg_3d_Dvfs_Checksize   reg_3d_dvfs_checksize;
    Reg_Reg_3d_Dvfs_H_Th        reg_3d_dvfs_h_threshold;
    Reg_Reg_3d_Dvfs_L_Th        reg_3d_dvfs_l_threshold;
    Reg_Reg_3d_Dvfs_H_Th_Mem    reg_3d_dvfs_h_th_mem;

    Reg_Reg_Vcp0_Dvfs_Checksize reg_vcp0_dvfs_checksize = {.uint = 0x2000};
    Reg_Reg_Vcp0_Dvfs_H_Th      reg_vcp0_dvfs_h_th      = {.uint = 0x1500};
    Reg_Reg_Vcp0_Dvfs_L_Th      reg_vcp0_dvfs_l_th      = {.uint = 0x0500};
    Reg_Reg_Vcp0_Dvfs_H_Th_Mem  reg_vcp0_dvfs_h_th_mem  = {.uint = 0x1800};

    Reg_Reg_Vcp1_Dvfs_Checksize reg_vcp1_dvfs_checksize = {.uint = 0x2000};
    Reg_Reg_Vcp1_Dvfs_H_Th      reg_vcp1_dvfs_h_th      = {.uint = 0x1500};
    Reg_Reg_Vcp1_Dvfs_L_Th      reg_vcp1_dvfs_l_th      = {.uint = 0x0500};
    Reg_Reg_Vcp1_Dvfs_H_Th_Mem  reg_vcp1_dvfs_h_th_mem  = {.uint = 0x1800};

    Reg_Reg_Vpp_Dvfs_Checksize reg_vpp_dvfs_checksize = {.uint = 0x2000};
    Reg_Reg_Vpp_Dvfs_H_Th      reg_vpp_dvfs_h_th      = {.uint = 0x1500};
    Reg_Reg_Vpp_Dvfs_L_Th      reg_vpp_dvfs_l_th      = {.uint = 0x0500};
    Reg_Reg_Vpp_Dvfs_H_Th_Mem  reg_vpp_dvfs_h_th_mem  = {.uint = 0x1800};

    if (adapter->chip_id == CHIP_CNE001)
    {
        reg_3d_dvfs_checksize.uint   = 0x4000;
        reg_3d_dvfs_h_threshold.uint = 0x2000;
        reg_3d_dvfs_l_threshold.uint = 0x0500;
        reg_3d_dvfs_h_th_mem.uint    = 0x3000;
    }
    else
    {
        reg_3d_dvfs_checksize.uint   = 0x2000;
        reg_3d_dvfs_h_threshold.uint = 0x1500;
        reg_3d_dvfs_l_threshold.uint = 0x0500;
        reg_3d_dvfs_h_th_mem.uint    = 0x1800;
    }

    //Before ECLK change, CRAB_C must set to 0, otherwise snoop request will make BIU hang
    zx_write8(adapter->mmio + 0x8AAB, 0);

    reg_dvfs_cfg_en.uint = 0;
    reg_dvfs_cfg_en.reg.Dvfs_3d_Auto_En   = adapter->pm_caps.gfx_dvfs_auto;
    reg_dvfs_cfg_en.reg.Dvfs_Vcp0_Auto_En = adapter->pm_caps.vcp_dvfs_auto;
    reg_dvfs_cfg_en.reg.Dvfs_Vcp1_Auto_En = adapter->pm_caps.vcp_dvfs_auto;
    reg_dvfs_cfg_en.reg.Dvfs_Vpp_Auto_En  = adapter->pm_caps.vpp_dvfs_auto;

    if(adapter->pm_caps.gfx_mem_dvfs || adapter->pm_caps.vcp_mem_dvfs || adapter->pm_caps.vpp_mem_dvfs)
    {
        reg_dvfs_cfg_en.reg.Reg_Memory_Dvfs_En = 1;

        reg_dvfs_cfg_misc0.reg.Dvfs_3d_M2m_Dvfs_En   = adapter->pm_caps.gfx_mem_dvfs;
        reg_dvfs_cfg_misc0.reg.Dvfs_Vcp0_M2m_Dvfs_En = adapter->pm_caps.vcp_mem_dvfs;
        reg_dvfs_cfg_misc0.reg.Dvfs_Vcp1_M2m_Dvfs_En = adapter->pm_caps.vcp_mem_dvfs;
        reg_dvfs_cfg_misc0.reg.Dvfs_Vpp_M2m_Dvfs_En  = adapter->pm_caps.vpp_mem_dvfs;
        /*******************miu to engine ratio***************
        *000:engine<16/16 miu
        *001:engine<15/16 miu
        *010:engine<14/16 miu
        *011:engine<13/16 miu
        *100:engine<12/16 miu
        *101:engine<11/16 miu
        *110:engine<10/16 miu
        *111:engine< 9/16 miu
        ******************************************************/
        reg_dvfs_cfg_misc0.reg.Dvfs_3d_M2e_Ratio     = 0x7;
        reg_dvfs_cfg_misc0.reg.Dvfs_Vcp0_M2e_Ratio   = 0x7;
        reg_dvfs_cfg_misc0.reg.Dvfs_Vcp1_M2e_Ratio   = 0x7;
        reg_dvfs_cfg_misc0.reg.Dvfs_Vpp_M2e_Ratio    = 0x7;
    }

    reg_dvfs_cfg_misc0.reg.Dvfs_3d_Output_Mode   = adapter->pm_caps.gfx_output_mode;
    reg_dvfs_cfg_misc0.reg.Dvfs_Vcp0_Output_Mode = adapter->pm_caps.vcp_output_mode;
    reg_dvfs_cfg_misc0.reg.Dvfs_Vcp1_Output_Mode = adapter->pm_caps.vcp_output_mode;
    reg_dvfs_cfg_misc0.reg.Dvfs_Vpp_Output_Mode  = adapter->pm_caps.vpp_output_mode;

    //3d_cnt_mode:
    //0--Max(EU,FFU,TU)
    //1--Max(EU,FFU,TU,TSG,L2)
    //2--OR(EU,FFU,TU)
    //3--OR(EU,FFU,TU,TSG,L2)
    reg_dvfs_cfg_misc0.reg.Dvfs_3d_Cnt_Mode = 3;

    //3d_cnt_mux: 0--ALU  1--Workload
    reg_dvfs_cfg_misc0.reg.Dvfs_3d_Cnt_Mux    = 0;

    zx_write32(adapter->mmio + MMIO_CSP_START_ADDRESS + Reg_Reg_3d_Dvfs_Checksize_Offset*4, reg_3d_dvfs_checksize.uint);
    zx_write32(adapter->mmio + MMIO_CSP_START_ADDRESS + Reg_Reg_3d_Dvfs_H_Th_Offset*4, reg_3d_dvfs_h_threshold.uint);
    zx_write32(adapter->mmio + MMIO_CSP_START_ADDRESS + Reg_Reg_3d_Dvfs_L_Th_Offset*4, reg_3d_dvfs_l_threshold.uint);
    zx_write32(adapter->mmio + MMIO_CSP_START_ADDRESS + Reg_Reg_3d_Dvfs_H_Th_Mem_Offset*4, reg_3d_dvfs_h_th_mem.uint);

    zx_write32(adapter->mmio + MMIO_CSP_START_ADDRESS + Reg_Reg_Vcp0_Dvfs_Checksize_Offset*4, reg_vcp0_dvfs_checksize.uint);
    zx_write32(adapter->mmio + MMIO_CSP_START_ADDRESS + Reg_Reg_Vcp0_Dvfs_H_Th_Offset*4, reg_vcp0_dvfs_h_th.uint);
    zx_write32(adapter->mmio + MMIO_CSP_START_ADDRESS + Reg_Reg_Vcp0_Dvfs_L_Th_Offset*4, reg_vcp0_dvfs_l_th.uint);
    zx_write32(adapter->mmio + MMIO_CSP_START_ADDRESS + Reg_Reg_Vcp0_Dvfs_H_Th_Mem_Offset*4, reg_vcp0_dvfs_h_th_mem.uint);

    zx_write32(adapter->mmio + MMIO_CSP_START_ADDRESS + Reg_Reg_Vcp1_Dvfs_Checksize_Offset*4, reg_vcp1_dvfs_checksize.uint);
    zx_write32(adapter->mmio + MMIO_CSP_START_ADDRESS + Reg_Reg_Vcp1_Dvfs_H_Th_Offset*4, reg_vcp1_dvfs_h_th.uint);
    zx_write32(adapter->mmio + MMIO_CSP_START_ADDRESS + Reg_Reg_Vcp1_Dvfs_L_Th_Offset*4, reg_vcp1_dvfs_l_th.uint);
    zx_write32(adapter->mmio + MMIO_CSP_START_ADDRESS + Reg_Reg_Vcp1_Dvfs_H_Th_Mem_Offset*4, reg_vcp1_dvfs_h_th_mem.uint);

    zx_write32(adapter->mmio + MMIO_CSP_START_ADDRESS + Reg_Reg_Vpp_Dvfs_Checksize_Offset*4, reg_vpp_dvfs_checksize.uint);
    zx_write32(adapter->mmio + MMIO_CSP_START_ADDRESS + Reg_Reg_Vpp_Dvfs_H_Th_Offset*4, reg_vpp_dvfs_h_th.uint);
    zx_write32(adapter->mmio + MMIO_CSP_START_ADDRESS + Reg_Reg_Vpp_Dvfs_L_Th_Offset*4, reg_vpp_dvfs_l_th.uint);
    zx_write32(adapter->mmio + MMIO_CSP_START_ADDRESS + Reg_Reg_Vpp_Dvfs_H_Th_Mem_Offset*4, reg_vpp_dvfs_h_th_mem.uint);

    zx_write32(adapter->mmio + MMIO_CSP_START_ADDRESS + Reg_Reg_Dvfs_Cfg_Misc0_Offset * 4, reg_dvfs_cfg_misc0.uint);
    zx_write32(adapter->mmio + MMIO_CSP_START_ADDRESS + Reg_Reg_Dvfs_Cfg_En_Offset * 4, reg_dvfs_cfg_en.uint);

    if(adapter->chip_id == CHIP_CNE001 && !adapter->hw_caps.dvfs_enable)
        vidschi_adjust_gfx_voltage_cne001(adapter);

    zx_info("init dvfs done.\n");
}

void vidsch_disable_dvfs_004(adapter_t *adapter)
{
    zx_write32(adapter->mmio + MMIO_CSP_START_ADDRESS + Reg_Reg_Dvfs_Cfg_En_Offset * 4, 0);
    zx_info("disable dvfs done.\n");
}

void vidsch_dfs_deinit_e3k(adapter_t *adapter)
{
    unsigned int reg_value = 0;

    //enable the bit of all reg protection. disable cpu R/W of dfs mmio.
    //reg_value = zx_read32(adapter->mmio +   );
    //reg_value |= (1 << 8);
    //zx_write32(adapter->mmio +   , reg_value);
}

void vidsch_dfs_tuning_e3k(adapter_t *adapter)
{
}
int vidsch_power_clock_on_off_004(vidsch_mgr_t *sch_mgr, unsigned int off)
{
    adapter_t * adapter = sch_mgr->adapter;
    vidsch_global_t* sch_global = adapter->sch_global;
    unsigned long flags = 0;
    int delay_times = 2000 * 100; // 2000 * 100 * 1 us = 200ms
    int i;

    Reg_Pwr_Mgr_Sw_Cfg0 reg_sw_cfg0;
    Reg_Pwr_Mgr_Status1 reg_status1;

    reg_status1.uint = 0;
    flags = zx_spin_lock_irqsave(sch_global->power_status_lock);
    //power_trace("[CG] used_gpc_mask_bit:%x\n", used_gpc_mask_bit);

    reg_sw_cfg0.uint = zx_read32(adapter->mmio + MMIO_CSP_START_ADDRESS + Reg_Pwr_Mgr_Sw_Cfg0_Offset * 4);
#define WRITE_SW_CFG_REG_GFX(type, val) \
        do \
        {  \
           reg_sw_cfg0.uint = zx_read32(adapter->mmio + MMIO_CSP_START_ADDRESS + Reg_Pwr_Mgr_Sw_Cfg0_Offset * 4); \
           reg_sw_cfg0.reg.Sw_Ctl_3d_Centroy_##type = val; \
           reg_sw_cfg0.reg.Sw_Ctl_3d_Slc0_##type = val;    \
           reg_sw_cfg0.reg.Sw_Ctl_3d_Slc1_##type = val;    \
           zx_write32(adapter->mmio + MMIO_CSP_START_ADDRESS + Reg_Pwr_Mgr_Sw_Cfg0_Offset * 4, reg_sw_cfg0.uint); \
           zx_udelay(1); \
        } while(0)

#define WRITE_SW_CFG0_REG_VIDEO(type, val)                                        \
    do {                                                                          \
         reg_sw_cfg0.uint = zx_read32(adapter->mmio + MMIO_CSP_START_ADDRESS + Reg_Pwr_Mgr_Sw_Cfg0_Offset * 4); \
         reg_sw_cfg0.reg.Sw_Ctl_##type = val;                                     \
         zx_write32(adapter->mmio + MMIO_CSP_START_ADDRESS + Reg_Pwr_Mgr_Sw_Cfg0_Offset * 4, reg_sw_cfg0.uint); \
         zx_udelay(1);                                                            \
    } while(0)

#define WRITE_SW_CFG0_REG_VIDEO_POWER_OFF(video_reg)                                            \
        do {                                                                                    \
            WRITE_SW_CFG0_REG_VIDEO(video_reg##_Clken, 0);                                      \
            WRITE_SW_CFG0_REG_VIDEO(video_reg##_Isoon, 1);                                      \
            WRITE_SW_CFG0_REG_VIDEO(video_reg##_Pwron, 0);                                      \
                                                                                                \
            reg_status1.uint = zx_read32(adapter->mmio + MMIO_CSP_START_ADDRESS + Reg_Pwr_Mgr_Status1_Offset * 4);       \
                                                                                                \
            while(reg_status1.reg.Csp_Pwm_##video_reg##_Pwron_Be != 0)                          \
            {                                                                                   \
                  delay_times--;                                                                \
                  if (!delay_times)                                                             \
                  {                                                                             \
                      zx_info("cannot power off video!!!\n");                                   \
                      break;                                                                    \
                  }                                                                             \
                  zx_udelay(1);                                                                 \
                  reg_status1.uint = zx_read32(adapter->mmio + MMIO_CSP_START_ADDRESS + Reg_Pwr_Mgr_Status1_Offset * 4); \
            }                                                                                   \
        }while (0)


#define WRITE_CFG0_REG_VIDEO_POWER_ON(video_reg) \
    do { \
        WRITE_SW_CFG0_REG_VIDEO(video_reg##_Pwron, 1);                                      \
        reg_status1.uint = zx_read32(adapter->mmio + MMIO_CSP_START_ADDRESS + Reg_Pwr_Mgr_Status1_Offset * 4);       \
        while(reg_status1.reg.Csp_Pwm_##video_reg##_Pwron_Be == 0)                          \
        {                                                                                   \
              delay_times--;                                                                \
              if (!delay_times)                                                             \
              {                                                                             \
                  zx_info("cannot power on video!!!\n");                                    \
                  break;                                                                    \
              }                                                                             \
              zx_udelay(1);                                                                 \
              reg_status1.uint = zx_read32(adapter->mmio + MMIO_CSP_START_ADDRESS + Reg_Pwr_Mgr_Status1_Offset * 4); \
         }                                                                                  \
         WRITE_SW_CFG0_REG_VIDEO(video_reg##_Rston, 0);                                     \
         WRITE_SW_CFG0_REG_VIDEO(video_reg##_Clken, 1);                                     \
         WRITE_SW_CFG0_REG_VIDEO(video_reg##_Rston, 1);                                     \
         WRITE_SW_CFG0_REG_VIDEO(video_reg##_Isoon, 0);                                     \
    }while (0)

    if(off)
    {
        switch(sch_mgr->engine_index)
        {
            case RB_INDEX_GFXL:
            case RB_INDEX_GFXH:
                //write_reg_e3k(adapter->mmio, CR_C, 0x01, 0, 0x8F);
                if (adapter->pm_caps.gfx_pg_manual)
                {
                    WRITE_SW_CFG_REG_GFX(Clken, 0);
                    WRITE_SW_CFG_REG_GFX(Isoon, 1);
                    WRITE_SW_CFG_REG_GFX(Pwron, 0);

                    reg_status1.uint = zx_read32(adapter->mmio + MMIO_CSP_START_ADDRESS + Reg_Pwr_Mgr_Status1_Offset * 4);

                    while(reg_status1.reg.Csp_Pwm_Central_Pwron_Be != 0 ||
                          reg_status1.reg.Csp_Pwm_Slc0_Pwron_Be != 0 ||
                          reg_status1.reg.Csp_Pwm_Slc1_Pwron_Be != 0)
                    {
                          delay_times--;
                          if (!delay_times)
                          {
                              zx_info("cannot power off gfx!!!\n");
                              for (i = Reg_Pwr_Mgr_Cfg_En_Offset; i <= Reg_Pwr_Mgr_Status1_Offset; i++)
                              {
                                   zx_info("reg %x value: %x\n", i, zx_read32(adapter->mmio + MMIO_CSP_START_ADDRESS + i * 4));
                              }
                              break;
                          }
                          zx_udelay(1);
                          reg_status1.uint = zx_read32(adapter->mmio + MMIO_CSP_START_ADDRESS + Reg_Pwr_Mgr_Status1_Offset * 4);
                    }
                }
                break;
            case RB_INDEX_VCP0:
                //write_reg_e3k(adapter->mmio, CR_C, 0x01, 0, 0xF7);
                if (adapter->pm_caps.vcp0_pg_manual)
                {
                    WRITE_SW_CFG0_REG_VIDEO_POWER_OFF(S3vd0);
                }
                break;
            case RB_INDEX_VCP1:
                //write_reg_e3k(adapter->mmio, CR_C, 0x01, 0, 0xFB);
                if (adapter->pm_caps.vcp1_pg_manual)
                {
                    WRITE_SW_CFG0_REG_VIDEO_POWER_OFF(S3vd1);
                }
                break;
            case RB_INDEX_VPP:
                //write_reg_e3k(adapter->mmio, CR_C, 0x01, 0, 0xFD);
                if(adapter->chip_id == CHIP_CHX004) {
                    if (adapter->pm_caps.vpp0_pg_manual ||adapter->pm_caps.vpp1_pg_manual) {
                        WRITE_SW_CFG0_REG_VIDEO_POWER_OFF(Vpp);
                    }
                }
                // TODO: for CNE001, need add VPP0/VPP1, VCP2 later. by vandy
                break;
            default:
                  break;
        }
        power_trace("[CG] engine %d do cg power off,  CR01:%x\n", sch_mgr->engine_index, read_reg_e3k(adapter->mmio, CR_C, 0x01));
    }
    else
    {
        switch(sch_mgr->engine_index)
        {
            case RB_INDEX_GFXL:
            case RB_INDEX_GFXH:
                //write_reg_e3k(adapter->mmio, CR_C, 0x01, (0x70 & used_gpc_mask_bit), 0x8F);
                if (adapter->pm_caps.gfx_pg_manual)
                {
                    reg_sw_cfg0.reg.Sw_Ctl_3d_Centroy_Pwron = 1;
                    reg_sw_cfg0.reg.Sw_Ctl_3d_Slc0_Pwron = 1;
                    reg_sw_cfg0.reg.Sw_Ctl_3d_Slc1_Pwron = 1;

                    zx_write32(adapter->mmio + MMIO_CSP_START_ADDRESS + Reg_Pwr_Mgr_Sw_Cfg0_Offset * 4, reg_sw_cfg0.uint);
                    reg_status1.uint = zx_read32(adapter->mmio + MMIO_CSP_START_ADDRESS + Reg_Pwr_Mgr_Status1_Offset * 4);

                    while(reg_status1.reg.Csp_Pwm_Central_Pwron_Be == 0 ||
                          reg_status1.reg.Csp_Pwm_Slc0_Pwron_Be == 0 ||
                          reg_status1.reg.Csp_Pwm_Slc1_Pwron_Be == 0)
                    {
                          delay_times--;
                          if (!delay_times)
                          {
                              zx_info("cannot power on gfx!!!\n");
                              for (i = Reg_Pwr_Mgr_Cfg_En_Offset; i <= Reg_Pwr_Mgr_Status1_Offset; i++)
                              {
                                   zx_info("reg %x value: %x\n", i, zx_read32(adapter->mmio + MMIO_CSP_START_ADDRESS + i * 4));
                              }
                              break;
                          }
                          zx_udelay(1);
                          reg_status1.uint = zx_read32(adapter->mmio + MMIO_CSP_START_ADDRESS + Reg_Pwr_Mgr_Status1_Offset * 4);

                    }

                    WRITE_SW_CFG_REG_GFX(Rston, 0);
                    WRITE_SW_CFG_REG_GFX(Clken, 1);
                    WRITE_SW_CFG_REG_GFX(Rston, 1);
                    WRITE_SW_CFG_REG_GFX(Isoon, 0);

                    zx_udelay(1);
                }
                break;
            case RB_INDEX_VCP0:
                //write_reg_e3k(adapter->mmio, CR_C, 0x01, 8, 0xF7);
                if (adapter->pm_caps.vcp0_pg_manual)
                {
                    WRITE_CFG0_REG_VIDEO_POWER_ON(S3vd0);
                }
                break;
            case RB_INDEX_VCP1:
                //write_reg_e3k(adapter->mmio, CR_C, 0x01, 4, 0xFB);
                if (adapter->pm_caps.vcp1_pg_manual)
                {
                    WRITE_CFG0_REG_VIDEO_POWER_ON(S3vd1);
                }
                break;
            case RB_INDEX_VPP:
                //write_reg_e3k(adapter->mmio, CR_C, 0x01, 2, 0xFD);
                if(adapter->chip_id == CHIP_CHX004) {
                    if (adapter->pm_caps.vpp0_pg_manual ||adapter->pm_caps.vpp1_pg_manual) {
                        WRITE_CFG0_REG_VIDEO_POWER_ON(Vpp);
                    }
                }
                // TODO: for CNE001, need add VPP0/VPP1, VCP2 later. by vandy
                break;
            default:
                  break;
        }
        power_trace("[CG] engine %d do cg power on,  CR01:%x\n", sch_mgr->engine_index, read_reg_e3k(adapter->mmio, CR_C, 0x01));
    }

    zx_spin_unlock_irqrestore(sch_global->power_status_lock, flags);

    return S_OK;
}

int vidsch_power_clock_on_off_cne001(vidsch_mgr_t *sch_mgr, unsigned int off)
{
    adapter_t * adapter = sch_mgr->adapter;
    vidsch_global_t* sch_global = adapter->sch_global;
    unsigned long flags = 0;
    int delay_times = 2000 * 100; // 2000 * 100 * 1 us = 200ms
    int ret = S_OK;
    int i;

    Reg_Pwr_Mgr_Sw_Cfg0 reg_sw_cfg0;
    Reg_Pwr_Mgr_Sw_Cfg1 reg_sw_cfg1;
    Reg_Pwr_Mgr_Sw_Cfg_En reg_sw_cfg_en;
    Reg_Pwr_Mgr_Status0 reg_status0;
    Reg_Pwr_Mgr_Status1 reg_status1;
    Reg_Pwr_Mgr_Status2 reg_status2;

    reg_status1.uint = 0;
    flags = zx_spin_lock_irqsave(sch_global->power_status_lock);
    //power_trace("[CG] used_gpc_mask_bit:%x\n", used_gpc_mask_bit);

    reg_sw_cfg0.uint = zx_read32(adapter->mmio + MMIO_CSP_START_ADDRESS + Reg_Pwr_Mgr_Sw_Cfg0_Offset * 4);
    reg_sw_cfg1.uint = zx_read32(adapter->mmio + MMIO_CSP_START_ADDRESS + Reg_Pwr_Mgr_Sw_Cfg1_Offset * 4);

#define WRITE_SW_CFG_REG_GFX_CNE001(num, reg, type, val, offset) \
        do \
        {  \
           reg_sw_cfg##num.uint = zx_read32(adapter->mmio + MMIO_CSP_START_ADDRESS + offset * 4); \
           reg_sw_cfg##num.reg.Sw_Ctl_3d_Centroy_##type = val; \
           reg_sw_cfg##num.reg.Sw_Ctl_3d_Slc0_##type = val;    \
           reg_sw_cfg##num.reg.Sw_Ctl_3d_Slc1_##type = val;    \
           zx_write32(adapter->mmio + MMIO_CSP_START_ADDRESS + offset * 4, reg_sw_cfg##num.uint); \
           zx_udelay(1); \
        } while(0)

#define CHECK_POWER_OFF_STATUS_GFX_CNE001(reg, offset,ret) \
        do \
        {  \
                    reg_status1.uint = zx_read32(adapter->mmio + MMIO_CSP_START_ADDRESS + offset * 4); \
                    while(reg_status1.reg.Csp_Pwm_Central_Pwron_Be != 0 ||                      \
                          reg_status1.reg.Csp_Pwm_Slc0_Pwron_Be != 0 ||                         \
                          reg_status1.reg.Csp_Pwm_Slc1_Pwron_Be != 0)                           \
                    {                                                                           \
                          delay_times--;                                                        \
                          if (!delay_times)                                                     \
                          {                                                                     \
                              zx_info("cannot power off gfx!!!\n");                             \
                              for (i = Reg_Pwr_Mgr_Cfg_En_Offset; i <= offset; i++) \
                              {                                                                 \
                                   zx_info("reg %x value: %x\n", i, zx_read32(adapter->mmio + MMIO_CSP_START_ADDRESS + i * 4)); \
                              }                                                                 \
                              ret = E_FAIL;                                                     \
                              break;                                                            \
                          }                                                                     \
                          zx_udelay(1);                                                         \
                          reg_status1.uint = zx_read32(adapter->mmio + MMIO_CSP_START_ADDRESS + offset * 4); \
                    } \
        } while(0)

#define CHECK_POWER_ON_STATUS_GFX_CNE001(reg, offset,ret) \
        do \
        {  \
                    reg_status1.uint = zx_read32(adapter->mmio + MMIO_CSP_START_ADDRESS + offset * 4); \
                    while(reg_status1.reg.Csp_Pwm_Central_Pwron_Be == 0 ||                      \
                          reg_status1.reg.Csp_Pwm_Slc0_Pwron_Be == 0 ||                         \
                          reg_status1.reg.Csp_Pwm_Slc1_Pwron_Be == 0)                           \
                    {                                                                           \
                          delay_times--;                                                        \
                          if (!delay_times)                                                     \
                          {                                                                     \
                              zx_info("cannot power on gfx!!!\n");                              \
                              for (i = Reg_Pwr_Mgr_Cfg_En_Offset; i <= offset; i++) \
                              {                                                                 \
                                   zx_info("reg %x value: %x\n", i, zx_read32(adapter->mmio + MMIO_CSP_START_ADDRESS + i * 4)); \
                              }                                                                 \
                              ret = E_FAIL;                                                     \
                              break;                                                            \
                          }                                                                     \
                          zx_udelay(1);                                                         \
                          reg_status1.uint = zx_read32(adapter->mmio + MMIO_CSP_START_ADDRESS + offset * 4); \
                    }                                                                           \
        } while(0)

#define WRITE_SW_CFG_REG_VIDEO_CNE001(num, reg, engine, type, val, offset)                      \
    do {                                                                                        \
         reg_sw_cfg##num.uint = zx_read32(adapter->mmio + MMIO_CSP_START_ADDRESS + offset * 4); \
         reg_sw_cfg##num.reg.Sw_Ctl_##engine##_##type = val;                                    \
         zx_write32(adapter->mmio + MMIO_CSP_START_ADDRESS + offset * 4, reg_sw_cfg##num.uint); \
         zx_udelay(1);                                                                          \
    } while(0)

#define CHECK_POWER_OFF_STATUS_VIDEO_CNE001(num, reg, engine, offset,ret)                       \
        do {                                                                                    \
            reg_status##num.uint = zx_read32(adapter->mmio + MMIO_CSP_START_ADDRESS + offset * 4); \
            while(reg_status##num.reg.Csp_Pwm_##engine##_Pwron_Be != 0)                         \
            {                                                                                   \
                  delay_times--;                                                                \
                  if (!delay_times)                                                             \
                  {                                                                             \
                      zx_info("cannot power off video!!!\n");                                   \
                      ret = E_FAIL;                                                             \
                      break;                                                                    \
                  }                                                                             \
                  zx_udelay(1);                                                                 \
                  reg_status##num.uint = zx_read32(adapter->mmio + MMIO_CSP_START_ADDRESS + offset * 4); \
            }                                                                                   \
        }while (0)


#define CHECK_POWER_ON_STATUS_VIDEO_CNE001(num, reg, engine, offset,ret) \
    do { \
        reg_status##num.uint = zx_read32(adapter->mmio + MMIO_CSP_START_ADDRESS + offset * 4); \
        while(reg_status##num.reg.Csp_Pwm_##engine##_Pwron_Be == 0)                          \
        {                                                                                   \
              delay_times--;                                                                \
              if (!delay_times)                                                             \
              {                                                                             \
                  zx_info("cannot power on video!!!\n");                                    \
                  ret = E_FAIL;                                                             \
                  break;                                                                    \
              }                                                                             \
              zx_udelay(1);                                                                 \
              reg_status##num.uint = zx_read32(adapter->mmio + MMIO_CSP_START_ADDRESS + offset * 4); \
         }                                                                                  \
    }while (0)

#define CHECK_CG_STATE_GFX_CNE001(offset,ret) \
    do { \
        reg_status0.uint = zx_read32(adapter->mmio + MMIO_CSP_START_ADDRESS + offset * 4); \
        while((reg_status0.uint & 0x3FFF) != 0)                                            \
        {                                                                                   \
              delay_times--;                                                                \
              if (!delay_times)                                                             \
              {                                                                             \
                  zx_info("cannot enter gfx cg state!!!\n");                                \
                  ret = E_FAIL;                                                             \
                  break;                                                                    \
              }                                                                             \
              zx_udelay(1);                                                                 \
              reg_status0.uint = zx_read32(adapter->mmio + MMIO_CSP_START_ADDRESS + offset * 4); \
         }                                                                                  \
    }while (0)
#define WRITE_SW_CFG_EN_GFX_CNE001(val, offset) \
        do \
        {  \
           reg_sw_cfg_en.uint = zx_read32(adapter->mmio + MMIO_CSP_START_ADDRESS + offset * 4); \
           reg_sw_cfg_en.reg.Sw_Ctl_3d_En = val;                                                \
           zx_write32(adapter->mmio + MMIO_CSP_START_ADDRESS + offset * 4, reg_sw_cfg_en.uint); \
           zx_udelay(1); \
        } while(0)

#define CHECK_CG_STATE_VIDEO_CNE001(offset,mask) \
    do \
    {  \
        reg_status2.uint = zx_read32(adapter->mmio + MMIO_CSP_START_ADDRESS + offset * 4);  \
        while((reg_status2.uint & mask) != 0)                                                 \
        {                                                                                   \
            delay_times--;                                                                \
            if (!delay_times)                                                             \
            {                                                                             \
                ret = E_FAIL;                                                             \
                zx_info("cannot enter video cg state!!!\n");                              \
                break;                                                                    \
            }                                                                             \
            zx_udelay(1);                                                                 \
            reg_status2.uint = zx_read32(adapter->mmio + MMIO_CSP_START_ADDRESS + offset * 4); \
        }                                                                                   \
    }while (0)
#define WRITE_SW_CFG_EN_VIDEO_CNE001(val, offset, engine) \
        do \
        {  \
           reg_sw_cfg_en.uint = zx_read32(adapter->mmio + MMIO_CSP_START_ADDRESS + offset * 4); \
           reg_sw_cfg_en.reg_cne001.Sw_Ctl_##engine##_En = val;                                 \
           zx_write32(adapter->mmio + MMIO_CSP_START_ADDRESS + offset * 4, reg_sw_cfg_en.uint); \
           zx_udelay(1); \
        } while(0)
#define CHECK_POWER_FAILURE_RETURN_VALUE(ret)                                               \
        if (ret != S_OK)                                                                    \
        {                                                                                   \
            break;                                                                          \
        }                                                                                   \

    if(off)
    {
        switch(sch_mgr->engine_index)
        {
            case RB_INDEX_GFXL:
            case RB_INDEX_GFXH:
                //write_reg_e3k(adapter->mmio, CR_C, 0x01, 0, 0x8F);
                if (adapter->pm_caps.gfx_cg_auto && adapter->pm_caps.gfx_pg_manual)
                {
                    CHECK_CG_STATE_GFX_CNE001(Reg_Pwr_Mgr_Status0_Offset_Cne001,ret);
                    CHECK_POWER_FAILURE_RETURN_VALUE(ret);
                    WRITE_SW_CFG_REG_GFX_CNE001(0, reg_cne001, Clken, 0, Reg_Pwr_Mgr_Sw_Cfg0_Offset);
                    WRITE_SW_CFG_REG_GFX_CNE001(0, reg_cne001, Isoon, 1, Reg_Pwr_Mgr_Sw_Cfg0_Offset);
                    WRITE_SW_CFG_REG_GFX_CNE001(0, reg_cne001, Rston, 0, Reg_Pwr_Mgr_Sw_Cfg0_Offset);
                    WRITE_SW_CFG_EN_GFX_CNE001(1, Reg_Pwr_Mgr_Sw_Cfg_En_Offset);
                    WRITE_SW_CFG_REG_GFX_CNE001(1, reg_cne001, Pwron, 0, Reg_Pwr_Mgr_Sw_Cfg1_Offset);
                    CHECK_POWER_OFF_STATUS_GFX_CNE001(reg_cne001, Reg_Pwr_Mgr_Status1_Offset_Cne001,ret);
                    CHECK_POWER_FAILURE_RETURN_VALUE(ret);
                }
                else if (adapter->pm_caps.gfx_cg_manual && adapter->pm_caps.gfx_pg_manual)
                {
                    WRITE_SW_CFG_REG_GFX_CNE001(0, reg_cne001, Clken, 0, Reg_Pwr_Mgr_Sw_Cfg0_Offset);
                    WRITE_SW_CFG_REG_GFX_CNE001(0, reg_cne001, Isoon, 1, Reg_Pwr_Mgr_Sw_Cfg0_Offset);
                    WRITE_SW_CFG_REG_GFX_CNE001(1, reg_cne001, Pwron, 0, Reg_Pwr_Mgr_Sw_Cfg1_Offset);
                    CHECK_POWER_OFF_STATUS_GFX_CNE001(reg_cne001, Reg_Pwr_Mgr_Status1_Offset_Cne001, ret);
                    CHECK_POWER_FAILURE_RETURN_VALUE(ret);
                }
                else if (adapter->pm_caps.gfx_cg_manual && !adapter->pm_caps.gfx_pg_manual)
                {
                    WRITE_SW_CFG_REG_GFX_CNE001(0, reg_cne001, Clken, 0, Reg_Pwr_Mgr_Sw_Cfg0_Offset);
                }
                break;
            case RB_INDEX_VCP0:
                //write_reg_e3k(adapter->mmio, CR_C, 0x01, 0, 0xF7);
                if (adapter->pm_caps.vcp0_cg_auto && adapter->pm_caps.vcp0_pg_manual)
                {
                    CHECK_CG_STATE_VIDEO_CNE001(Reg_Pwr_Mgr_Status2_Offset, 0x1);
                    CHECK_POWER_FAILURE_RETURN_VALUE(ret);
                    WRITE_SW_CFG_REG_VIDEO_CNE001(0, reg_cne001, Vcp, Clken, 0, Reg_Pwr_Mgr_Sw_Cfg0_Offset);
                    WRITE_SW_CFG_REG_VIDEO_CNE001(0, reg_cne001, Vcp, Isoon, 1, Reg_Pwr_Mgr_Sw_Cfg0_Offset);
                    WRITE_SW_CFG_REG_VIDEO_CNE001(0, reg_cne001, Vcp, Rston, 0, Reg_Pwr_Mgr_Sw_Cfg0_Offset);
                    WRITE_SW_CFG_EN_VIDEO_CNE001(1, Reg_Pwr_Mgr_Sw_Cfg_En_Offset, Vcp);
                    WRITE_SW_CFG_REG_VIDEO_CNE001(1, reg_cne001, Vcp, Pwron, 0, Reg_Pwr_Mgr_Sw_Cfg1_Offset);
                    CHECK_POWER_OFF_STATUS_VIDEO_CNE001(2, reg_cne001, Vcp, Reg_Pwr_Mgr_Status2_Offset,ret);
                    CHECK_POWER_FAILURE_RETURN_VALUE(ret);
                }
                else if (adapter->pm_caps.vcp0_cg_manual && adapter->pm_caps.vcp0_pg_manual)
                {
                    WRITE_SW_CFG_REG_VIDEO_CNE001(0, reg_cne001, Vcp, Clken, 0, Reg_Pwr_Mgr_Sw_Cfg0_Offset);
                    WRITE_SW_CFG_REG_VIDEO_CNE001(0, reg_cne001, Vcp, Isoon, 1, Reg_Pwr_Mgr_Sw_Cfg0_Offset);
                    WRITE_SW_CFG_REG_VIDEO_CNE001(1, reg_cne001, Vcp, Pwron, 0, Reg_Pwr_Mgr_Sw_Cfg1_Offset);
                    CHECK_POWER_OFF_STATUS_VIDEO_CNE001(2, reg_cne001, Vcp, Reg_Pwr_Mgr_Status2_Offset, ret);
                    CHECK_POWER_FAILURE_RETURN_VALUE(ret);
                }
                else if (adapter->pm_caps.vcp0_cg_manual && !adapter->pm_caps.vcp0_pg_manual)
                {
                    WRITE_SW_CFG_REG_VIDEO_CNE001(0, reg_cne001, Vcp, Clken, 0, Reg_Pwr_Mgr_Sw_Cfg0_Offset);
                }
                break;
            case RB_INDEX_VCP1:
                //write_reg_e3k(adapter->mmio, CR_C, 0x01, 0, 0xFB);
                if (adapter->pm_caps.vcp1_cg_auto && adapter->pm_caps.vcp1_pg_manual)
                {
                    CHECK_CG_STATE_VIDEO_CNE001(Reg_Pwr_Mgr_Status2_Offset, 0x2);
                    CHECK_POWER_FAILURE_RETURN_VALUE(ret);
                    WRITE_SW_CFG_REG_VIDEO_CNE001(0, reg_cne001, Vdp0, Clken, 0, Reg_Pwr_Mgr_Sw_Cfg0_Offset);
                    WRITE_SW_CFG_REG_VIDEO_CNE001(0, reg_cne001, Vdp0, Isoon, 1, Reg_Pwr_Mgr_Sw_Cfg0_Offset);
                    WRITE_SW_CFG_REG_VIDEO_CNE001(0, reg_cne001, Vdp0, Rston, 0, Reg_Pwr_Mgr_Sw_Cfg0_Offset);
                    WRITE_SW_CFG_EN_VIDEO_CNE001(1, Reg_Pwr_Mgr_Sw_Cfg_En_Offset, Vdp0);
                    WRITE_SW_CFG_REG_VIDEO_CNE001(1, reg_cne001, Vdp0, Pwron, 0, Reg_Pwr_Mgr_Sw_Cfg1_Offset);
                    CHECK_POWER_OFF_STATUS_VIDEO_CNE001(2, reg_cne001, Vdp0, Reg_Pwr_Mgr_Status2_Offset,ret);
                    CHECK_POWER_FAILURE_RETURN_VALUE(ret);
                }
                else if (adapter->pm_caps.vcp1_cg_manual && adapter->pm_caps.vcp1_pg_manual)
                {
                    WRITE_SW_CFG_REG_VIDEO_CNE001(0, reg_cne001, Vdp0, Clken, 0, Reg_Pwr_Mgr_Sw_Cfg0_Offset);
                    WRITE_SW_CFG_REG_VIDEO_CNE001(0, reg_cne001, Vdp0, Isoon, 1, Reg_Pwr_Mgr_Sw_Cfg0_Offset);
                    WRITE_SW_CFG_REG_VIDEO_CNE001(1, reg_cne001, Vdp0, Pwron, 0, Reg_Pwr_Mgr_Sw_Cfg1_Offset);
                    CHECK_POWER_OFF_STATUS_VIDEO_CNE001(2, reg_cne001, Vdp0, Reg_Pwr_Mgr_Status2_Offset, ret);
                    CHECK_POWER_FAILURE_RETURN_VALUE(ret);
                }
                else if (adapter->pm_caps.vcp1_cg_manual && !adapter->pm_caps.vcp1_pg_manual)
                {
                    WRITE_SW_CFG_REG_VIDEO_CNE001(0, reg_cne001, Vdp0, Clken, 0, Reg_Pwr_Mgr_Sw_Cfg0_Offset);
                }
                break;
            case RB_INDEX_VCP2:
                //write_reg_e3k(adapter->mmio, CR_C, 0x01, 0, 0xFB);
                if (adapter->pm_caps.vcp2_cg_auto && adapter->pm_caps.vcp2_pg_manual)
                {
                    CHECK_CG_STATE_VIDEO_CNE001(Reg_Pwr_Mgr_Status2_Offset, 0x4);
                    CHECK_POWER_FAILURE_RETURN_VALUE(ret);
                    WRITE_SW_CFG_REG_VIDEO_CNE001(0, reg_cne001, Vdp1, Clken, 0, Reg_Pwr_Mgr_Sw_Cfg0_Offset);
                    WRITE_SW_CFG_REG_VIDEO_CNE001(0, reg_cne001, Vdp1, Isoon, 1, Reg_Pwr_Mgr_Sw_Cfg0_Offset);
                    WRITE_SW_CFG_REG_VIDEO_CNE001(0, reg_cne001, Vdp1, Rston, 0, Reg_Pwr_Mgr_Sw_Cfg0_Offset);
                    WRITE_SW_CFG_EN_VIDEO_CNE001(1, Reg_Pwr_Mgr_Sw_Cfg_En_Offset, Vdp1);
                    WRITE_SW_CFG_REG_VIDEO_CNE001(1, reg_cne001, Vdp1, Pwron, 0, Reg_Pwr_Mgr_Sw_Cfg1_Offset);
                    CHECK_POWER_OFF_STATUS_VIDEO_CNE001(2, reg_cne001, Vdp1, Reg_Pwr_Mgr_Status2_Offset,ret);
                    CHECK_POWER_FAILURE_RETURN_VALUE(ret);
                }
                else if (adapter->pm_caps.vcp2_cg_manual && adapter->pm_caps.vcp2_pg_manual)
                {
                    WRITE_SW_CFG_REG_VIDEO_CNE001(0, reg_cne001, Vdp1, Clken, 0, Reg_Pwr_Mgr_Sw_Cfg0_Offset);
                    WRITE_SW_CFG_REG_VIDEO_CNE001(0, reg_cne001, Vdp1, Isoon, 1, Reg_Pwr_Mgr_Sw_Cfg0_Offset);
                    WRITE_SW_CFG_REG_VIDEO_CNE001(1, reg_cne001, Vdp1, Pwron, 0, Reg_Pwr_Mgr_Sw_Cfg1_Offset);
                    CHECK_POWER_OFF_STATUS_VIDEO_CNE001(2, reg_cne001, Vdp1, Reg_Pwr_Mgr_Status2_Offset, ret);
                    CHECK_POWER_FAILURE_RETURN_VALUE(ret);
                }
                else if (adapter->pm_caps.vcp2_cg_manual && !adapter->pm_caps.vcp2_pg_manual)
                {
                    WRITE_SW_CFG_REG_VIDEO_CNE001(0, reg_cne001, Vdp1, Clken, 0, Reg_Pwr_Mgr_Sw_Cfg0_Offset);
                }
                break;
            case RB_INDEX_VPP0:
                //write_reg_e3k(adapter->mmio, CR_C, 0x01, 0, 0xFD);
                if (adapter->pm_caps.vpp0_cg_auto && adapter->pm_caps.vpp0_pg_manual)
                {
                    CHECK_CG_STATE_VIDEO_CNE001(Reg_Pwr_Mgr_Status2_Offset, 0x10);
                    CHECK_POWER_FAILURE_RETURN_VALUE(ret);
                    WRITE_SW_CFG_REG_VIDEO_CNE001(0, reg_cne001, Vpp0, Clken, 0, Reg_Pwr_Mgr_Sw_Cfg0_Offset);
                    WRITE_SW_CFG_REG_VIDEO_CNE001(0, reg_cne001, Vpp0, Isoon, 1, Reg_Pwr_Mgr_Sw_Cfg0_Offset);
                    WRITE_SW_CFG_REG_VIDEO_CNE001(0, reg_cne001, Vpp0, Rston, 0, Reg_Pwr_Mgr_Sw_Cfg0_Offset);
                    WRITE_SW_CFG_EN_VIDEO_CNE001(1, Reg_Pwr_Mgr_Sw_Cfg_En_Offset, Vpp0);
                    WRITE_SW_CFG_REG_VIDEO_CNE001(1, reg_cne001, Vpp0, Pwron, 0, Reg_Pwr_Mgr_Sw_Cfg1_Offset);
                    CHECK_POWER_OFF_STATUS_VIDEO_CNE001(2, reg_cne001, Vpp0, Reg_Pwr_Mgr_Status2_Offset,ret);
                    CHECK_POWER_FAILURE_RETURN_VALUE(ret);
                }
                else if (adapter->pm_caps.vpp0_cg_manual && adapter->pm_caps.vpp0_pg_manual)
                {
                    WRITE_SW_CFG_REG_VIDEO_CNE001(0, reg_cne001, Vpp0, Clken, 0, Reg_Pwr_Mgr_Sw_Cfg0_Offset);
                    WRITE_SW_CFG_REG_VIDEO_CNE001(0, reg_cne001, Vpp0, Isoon, 1, Reg_Pwr_Mgr_Sw_Cfg0_Offset);
                    WRITE_SW_CFG_REG_VIDEO_CNE001(1, reg_cne001, Vpp0, Pwron, 0, Reg_Pwr_Mgr_Sw_Cfg1_Offset);
                    CHECK_POWER_OFF_STATUS_VIDEO_CNE001(2, reg_cne001, Vpp0, Reg_Pwr_Mgr_Status2_Offset, ret);
                    CHECK_POWER_FAILURE_RETURN_VALUE(ret);
                }
                else if (adapter->pm_caps.vpp0_cg_manual && !adapter->pm_caps.vpp0_pg_manual)
                {
                    WRITE_SW_CFG_REG_VIDEO_CNE001(0, reg_cne001, Vpp0, Clken, 0, Reg_Pwr_Mgr_Sw_Cfg0_Offset);
                }
                break;
            case RB_INDEX_VPP1:
                //write_reg_e3k(adapter->mmio, CR_C, 0x01, 0, 0xFD);
                if (adapter->pm_caps.vpp1_cg_auto && adapter->pm_caps.vpp1_pg_manual)
                {
                    CHECK_CG_STATE_VIDEO_CNE001(Reg_Pwr_Mgr_Status2_Offset, 0x20);
                    CHECK_POWER_FAILURE_RETURN_VALUE(ret);
                    WRITE_SW_CFG_REG_VIDEO_CNE001(0, reg_cne001, Vpp1, Clken, 0, Reg_Pwr_Mgr_Sw_Cfg0_Offset);
                    WRITE_SW_CFG_REG_VIDEO_CNE001(0, reg_cne001, Vpp1, Isoon, 1, Reg_Pwr_Mgr_Sw_Cfg0_Offset);
                    WRITE_SW_CFG_REG_VIDEO_CNE001(0, reg_cne001, Vpp1, Rston, 0, Reg_Pwr_Mgr_Sw_Cfg0_Offset);
                    WRITE_SW_CFG_EN_VIDEO_CNE001(1, Reg_Pwr_Mgr_Sw_Cfg_En_Offset, Vpp1);
                    WRITE_SW_CFG_REG_VIDEO_CNE001(1, reg_cne001, Vpp1, Pwron, 0, Reg_Pwr_Mgr_Sw_Cfg1_Offset);
                    CHECK_POWER_OFF_STATUS_VIDEO_CNE001(2, reg_cne001, Vpp1, Reg_Pwr_Mgr_Status2_Offset,ret);
                    CHECK_POWER_FAILURE_RETURN_VALUE(ret);
                }
                else if (adapter->pm_caps.vpp1_cg_manual && adapter->pm_caps.vpp1_pg_manual)
                {
                    WRITE_SW_CFG_REG_VIDEO_CNE001(0, reg_cne001, Vpp1, Clken, 0, Reg_Pwr_Mgr_Sw_Cfg0_Offset);
                    WRITE_SW_CFG_REG_VIDEO_CNE001(0, reg_cne001, Vpp1, Isoon, 1, Reg_Pwr_Mgr_Sw_Cfg0_Offset);
                    WRITE_SW_CFG_REG_VIDEO_CNE001(1, reg_cne001, Vpp1, Pwron, 0, Reg_Pwr_Mgr_Sw_Cfg1_Offset);
                    CHECK_POWER_OFF_STATUS_VIDEO_CNE001(2, reg_cne001, Vpp1, Reg_Pwr_Mgr_Status2_Offset, ret);
                    CHECK_POWER_FAILURE_RETURN_VALUE(ret);
                }
                else if (adapter->pm_caps.vpp1_cg_manual && !adapter->pm_caps.vpp1_pg_manual)
                {
                    WRITE_SW_CFG_REG_VIDEO_CNE001(0, reg_cne001, Vpp1, Clken, 0, Reg_Pwr_Mgr_Sw_Cfg0_Offset);
                }
                break;
            default:
                  break;
        }

        power_trace("[CG] engine %d do cg power off,  CR01:%x\n", sch_mgr->engine_index, read_reg_e3k(adapter->mmio, CR_C, 0x01));
    }
    else
    {
        switch(sch_mgr->engine_index)
        {
            case RB_INDEX_GFXL:
            case RB_INDEX_GFXH:
                //write_reg_e3k(adapter->mmio, CR_C, 0x01, (0x70 & used_gpc_mask_bit), 0x8F);
                if (adapter->pm_caps.gfx_cg_auto && adapter->pm_caps.gfx_pg_manual)
                {
                    WRITE_SW_CFG_REG_GFX_CNE001(1, reg_cne001, Pwron, 1, Reg_Pwr_Mgr_Sw_Cfg1_Offset);
                    CHECK_POWER_ON_STATUS_GFX_CNE001(reg_cne001, Reg_Pwr_Mgr_Status1_Offset_Cne001,ret);
                    CHECK_POWER_FAILURE_RETURN_VALUE(ret);
                    WRITE_SW_CFG_REG_GFX_CNE001(0, reg_cne001, Isoon, 0, Reg_Pwr_Mgr_Sw_Cfg0_Offset);
                    WRITE_SW_CFG_REG_GFX_CNE001(0, reg_cne001, Rston, 1, Reg_Pwr_Mgr_Sw_Cfg0_Offset);
                    WRITE_SW_CFG_EN_GFX_CNE001(0, Reg_Pwr_Mgr_Sw_Cfg_En_Offset);
                }
                else if (adapter->pm_caps.gfx_cg_manual && adapter->pm_caps.gfx_pg_manual)
                {
                    WRITE_SW_CFG_REG_GFX_CNE001(1, reg_cne001, Pwron, 1, Reg_Pwr_Mgr_Sw_Cfg1_Offset);
                    CHECK_POWER_ON_STATUS_GFX_CNE001(reg_cne001, Reg_Pwr_Mgr_Status1_Offset_Cne001,ret);
                    CHECK_POWER_FAILURE_RETURN_VALUE(ret);
                    WRITE_SW_CFG_REG_GFX_CNE001(0, reg_cne001, Rston, 0, Reg_Pwr_Mgr_Sw_Cfg0_Offset);
                    WRITE_SW_CFG_REG_GFX_CNE001(0, reg_cne001, Clken, 1, Reg_Pwr_Mgr_Sw_Cfg0_Offset);
                    WRITE_SW_CFG_REG_GFX_CNE001(0, reg_cne001, Rston, 1, Reg_Pwr_Mgr_Sw_Cfg0_Offset);
                    WRITE_SW_CFG_REG_GFX_CNE001(0, reg_cne001, Isoon, 0, Reg_Pwr_Mgr_Sw_Cfg0_Offset);
                }
                else if (adapter->pm_caps.gfx_cg_manual && !adapter->pm_caps.gfx_pg_manual)
                {
                    WRITE_SW_CFG_REG_GFX_CNE001(0, reg_cne001, Clken, 1, Reg_Pwr_Mgr_Sw_Cfg0_Offset);
                }
                break;
            case RB_INDEX_VCP0:
                //write_reg_e3k(adapter->mmio, CR_C, 0x01, 8, 0xF7);
                if (adapter->pm_caps.vcp0_cg_auto && adapter->pm_caps.vcp0_pg_manual)
                {
                    WRITE_SW_CFG_REG_VIDEO_CNE001(1, reg_cne001, Vcp, Pwron, 1, Reg_Pwr_Mgr_Sw_Cfg1_Offset);
                    CHECK_POWER_ON_STATUS_VIDEO_CNE001(2, reg_cne001, Vcp, Reg_Pwr_Mgr_Status2_Offset,ret);
                    CHECK_POWER_FAILURE_RETURN_VALUE(ret);
                    WRITE_SW_CFG_REG_VIDEO_CNE001(0, reg_cne001, Vcp, Isoon, 0, Reg_Pwr_Mgr_Sw_Cfg0_Offset);
                    WRITE_SW_CFG_REG_VIDEO_CNE001(0, reg_cne001, Vcp, Rston, 1, Reg_Pwr_Mgr_Sw_Cfg0_Offset);
                    WRITE_SW_CFG_EN_VIDEO_CNE001(0,Reg_Pwr_Mgr_Sw_Cfg_En_Offset,Vcp);
                }
                else if (adapter->pm_caps.vcp0_cg_manual && adapter->pm_caps.vcp0_pg_manual)
                {
                    WRITE_SW_CFG_REG_VIDEO_CNE001(1, reg_cne001, Vcp, Pwron, 1, Reg_Pwr_Mgr_Sw_Cfg1_Offset);
                    CHECK_POWER_ON_STATUS_VIDEO_CNE001(2, reg_cne001, Vcp, Reg_Pwr_Mgr_Status2_Offset,ret);
                    CHECK_POWER_FAILURE_RETURN_VALUE(ret);
                    WRITE_SW_CFG_REG_VIDEO_CNE001(0, reg_cne001, Vcp, Rston, 0, Reg_Pwr_Mgr_Sw_Cfg0_Offset);
                    WRITE_SW_CFG_REG_VIDEO_CNE001(0, reg_cne001, Vcp, Clken, 1, Reg_Pwr_Mgr_Sw_Cfg0_Offset);
                    WRITE_SW_CFG_REG_VIDEO_CNE001(0, reg_cne001, Vcp, Rston, 1, Reg_Pwr_Mgr_Sw_Cfg0_Offset);
                    WRITE_SW_CFG_REG_VIDEO_CNE001(0, reg_cne001, Vcp, Isoon, 0, Reg_Pwr_Mgr_Sw_Cfg0_Offset);
                }
                else if (adapter->pm_caps.vcp0_cg_manual && !adapter->pm_caps.vcp0_pg_manual)
                {
                    WRITE_SW_CFG_REG_VIDEO_CNE001(0, reg_cne001, Vcp, Clken, 1, Reg_Pwr_Mgr_Sw_Cfg0_Offset);
                }
                break;
            case RB_INDEX_VCP1:
                //write_reg_e3k(adapter->mmio, CR_C, 0x01, 4, 0xFB);
                if (adapter->pm_caps.vcp1_cg_auto && adapter->pm_caps.vcp1_pg_manual)
                {
                    WRITE_SW_CFG_REG_VIDEO_CNE001(1, reg_cne001, Vdp0, Pwron, 1, Reg_Pwr_Mgr_Sw_Cfg1_Offset);
                    CHECK_POWER_ON_STATUS_VIDEO_CNE001(2, reg_cne001, Vdp0, Reg_Pwr_Mgr_Status2_Offset,ret);
                    CHECK_POWER_FAILURE_RETURN_VALUE(ret);
                    WRITE_SW_CFG_REG_VIDEO_CNE001(0, reg_cne001, Vdp0, Isoon, 0, Reg_Pwr_Mgr_Sw_Cfg0_Offset);
                    WRITE_SW_CFG_REG_VIDEO_CNE001(0, reg_cne001, Vdp0, Rston, 1, Reg_Pwr_Mgr_Sw_Cfg0_Offset);
                    WRITE_SW_CFG_EN_VIDEO_CNE001(0,Reg_Pwr_Mgr_Sw_Cfg_En_Offset,Vdp0);
                }
                else if (adapter->pm_caps.vcp1_cg_manual && adapter->pm_caps.vcp1_pg_manual)
                {
                    WRITE_SW_CFG_REG_VIDEO_CNE001(1, reg_cne001, Vdp0, Pwron, 1, Reg_Pwr_Mgr_Sw_Cfg1_Offset);
                    CHECK_POWER_ON_STATUS_VIDEO_CNE001(2, reg_cne001, Vdp0, Reg_Pwr_Mgr_Status2_Offset,ret);
                    CHECK_POWER_FAILURE_RETURN_VALUE(ret);
                    WRITE_SW_CFG_REG_VIDEO_CNE001(0, reg_cne001, Vdp0, Rston, 0, Reg_Pwr_Mgr_Sw_Cfg0_Offset);
                    WRITE_SW_CFG_REG_VIDEO_CNE001(0, reg_cne001, Vdp0, Clken, 1, Reg_Pwr_Mgr_Sw_Cfg0_Offset);
                    WRITE_SW_CFG_REG_VIDEO_CNE001(0, reg_cne001, Vdp0, Rston, 1, Reg_Pwr_Mgr_Sw_Cfg0_Offset);
                    WRITE_SW_CFG_REG_VIDEO_CNE001(0, reg_cne001, Vdp0, Isoon, 0, Reg_Pwr_Mgr_Sw_Cfg0_Offset);
                }
                else if (adapter->pm_caps.vcp1_cg_manual && !adapter->pm_caps.vcp1_pg_manual)
                {
                    WRITE_SW_CFG_REG_VIDEO_CNE001(0, reg_cne001, Vdp0, Clken, 1, Reg_Pwr_Mgr_Sw_Cfg0_Offset);
                }
                break;
            case RB_INDEX_VCP2:
                //write_reg_e3k(adapter->mmio, CR_C, 0x01, 4, 0xFB);
                if (adapter->pm_caps.vcp2_cg_auto && adapter->pm_caps.vcp2_pg_manual)
                {
                    WRITE_SW_CFG_REG_VIDEO_CNE001(1, reg_cne001, Vdp1, Pwron, 1, Reg_Pwr_Mgr_Sw_Cfg1_Offset);
                    CHECK_POWER_ON_STATUS_VIDEO_CNE001(2, reg_cne001, Vdp1, Reg_Pwr_Mgr_Status2_Offset,ret);
                    CHECK_POWER_FAILURE_RETURN_VALUE(ret);
                    WRITE_SW_CFG_REG_VIDEO_CNE001(0, reg_cne001, Vdp1, Isoon, 0, Reg_Pwr_Mgr_Sw_Cfg0_Offset);
                    WRITE_SW_CFG_REG_VIDEO_CNE001(0, reg_cne001, Vdp1, Rston, 1, Reg_Pwr_Mgr_Sw_Cfg0_Offset);
                    WRITE_SW_CFG_EN_VIDEO_CNE001(0,Reg_Pwr_Mgr_Sw_Cfg_En_Offset,Vdp1);
                }
                else if (adapter->pm_caps.vcp2_cg_manual && adapter->pm_caps.vcp2_pg_manual)
                {
                    WRITE_SW_CFG_REG_VIDEO_CNE001(1, reg_cne001, Vdp1, Pwron, 1, Reg_Pwr_Mgr_Sw_Cfg1_Offset);
                    CHECK_POWER_ON_STATUS_VIDEO_CNE001(2, reg_cne001, Vdp1, Reg_Pwr_Mgr_Status2_Offset,ret);
                    CHECK_POWER_FAILURE_RETURN_VALUE(ret);
                    WRITE_SW_CFG_REG_VIDEO_CNE001(0, reg_cne001, Vdp1, Rston, 0, Reg_Pwr_Mgr_Sw_Cfg0_Offset);
                    WRITE_SW_CFG_REG_VIDEO_CNE001(0, reg_cne001, Vdp1, Clken, 1, Reg_Pwr_Mgr_Sw_Cfg0_Offset);
                    WRITE_SW_CFG_REG_VIDEO_CNE001(0, reg_cne001, Vdp1, Rston, 1, Reg_Pwr_Mgr_Sw_Cfg0_Offset);
                    WRITE_SW_CFG_REG_VIDEO_CNE001(0, reg_cne001, Vdp1, Isoon, 0, Reg_Pwr_Mgr_Sw_Cfg0_Offset);
                }
                else if (adapter->pm_caps.vcp2_cg_manual && !adapter->pm_caps.vcp2_pg_manual)
                {
                    WRITE_SW_CFG_REG_VIDEO_CNE001(0, reg_cne001, Vdp1, Clken, 1, Reg_Pwr_Mgr_Sw_Cfg0_Offset);
                }
                break;
            case RB_INDEX_VPP0:
                //write_reg_e3k(adapter->mmio, CR_C, 0x01, 2, 0xFD);
                if (adapter->pm_caps.vpp0_cg_auto && adapter->pm_caps.vpp0_pg_manual)
                {
                    WRITE_SW_CFG_REG_VIDEO_CNE001(1, reg_cne001, Vpp0, Pwron, 1, Reg_Pwr_Mgr_Sw_Cfg1_Offset);
                    CHECK_POWER_ON_STATUS_VIDEO_CNE001(2, reg_cne001, Vpp0, Reg_Pwr_Mgr_Status2_Offset,ret);
                    CHECK_POWER_FAILURE_RETURN_VALUE(ret);
                    WRITE_SW_CFG_REG_VIDEO_CNE001(0, reg_cne001, Vpp0, Isoon, 0, Reg_Pwr_Mgr_Sw_Cfg0_Offset);
                    WRITE_SW_CFG_REG_VIDEO_CNE001(0, reg_cne001, Vpp0, Rston, 1, Reg_Pwr_Mgr_Sw_Cfg0_Offset);
                    WRITE_SW_CFG_EN_VIDEO_CNE001(0,Reg_Pwr_Mgr_Sw_Cfg_En_Offset,Vpp0);
                }
                else if (adapter->pm_caps.vpp0_cg_manual && adapter->pm_caps.vpp0_pg_manual)
                {
                    WRITE_SW_CFG_REG_VIDEO_CNE001(1, reg_cne001, Vpp0, Pwron, 1, Reg_Pwr_Mgr_Sw_Cfg1_Offset);
                    CHECK_POWER_ON_STATUS_VIDEO_CNE001(2, reg_cne001, Vpp0, Reg_Pwr_Mgr_Status2_Offset,ret);
                    CHECK_POWER_FAILURE_RETURN_VALUE(ret);
                    WRITE_SW_CFG_REG_VIDEO_CNE001(0, reg_cne001, Vpp0, Rston, 0, Reg_Pwr_Mgr_Sw_Cfg0_Offset);
                    WRITE_SW_CFG_REG_VIDEO_CNE001(0, reg_cne001, Vpp0, Clken, 1, Reg_Pwr_Mgr_Sw_Cfg0_Offset);
                    WRITE_SW_CFG_REG_VIDEO_CNE001(0, reg_cne001, Vpp0, Rston, 1, Reg_Pwr_Mgr_Sw_Cfg0_Offset);
                    WRITE_SW_CFG_REG_VIDEO_CNE001(0, reg_cne001, Vpp0, Isoon, 0, Reg_Pwr_Mgr_Sw_Cfg0_Offset);
                }
                else if (adapter->pm_caps.vpp0_cg_manual && !adapter->pm_caps.vpp0_pg_manual)
                {
                    WRITE_SW_CFG_REG_VIDEO_CNE001(0, reg_cne001, Vpp0, Clken, 1, Reg_Pwr_Mgr_Sw_Cfg0_Offset);
                }
                break;
            case RB_INDEX_VPP1:
                //write_reg_e3k(adapter->mmio, CR_C, 0x01, 2, 0xFD);
                if (adapter->pm_caps.vpp1_cg_auto && adapter->pm_caps.vpp1_pg_manual)
                {
                    WRITE_SW_CFG_REG_VIDEO_CNE001(1, reg_cne001, Vpp1, Pwron, 1, Reg_Pwr_Mgr_Sw_Cfg1_Offset);
                    CHECK_POWER_ON_STATUS_VIDEO_CNE001(2, reg_cne001, Vpp1, Reg_Pwr_Mgr_Status2_Offset,ret);
                    CHECK_POWER_FAILURE_RETURN_VALUE(ret);
                    WRITE_SW_CFG_REG_VIDEO_CNE001(0, reg_cne001, Vpp1, Isoon, 0, Reg_Pwr_Mgr_Sw_Cfg0_Offset);
                    WRITE_SW_CFG_REG_VIDEO_CNE001(0, reg_cne001, Vpp1, Rston, 1, Reg_Pwr_Mgr_Sw_Cfg0_Offset);
                    WRITE_SW_CFG_EN_VIDEO_CNE001(0,Reg_Pwr_Mgr_Sw_Cfg_En_Offset,Vpp1);
                }
                else if (adapter->pm_caps.vpp1_cg_manual && adapter->pm_caps.vpp1_pg_manual)
                {
                    WRITE_SW_CFG_REG_VIDEO_CNE001(1, reg_cne001, Vpp1, Pwron, 1, Reg_Pwr_Mgr_Sw_Cfg1_Offset);
                    CHECK_POWER_ON_STATUS_VIDEO_CNE001(2, reg_cne001, Vpp1, Reg_Pwr_Mgr_Status2_Offset,ret);
                    CHECK_POWER_FAILURE_RETURN_VALUE(ret);
                    WRITE_SW_CFG_REG_VIDEO_CNE001(0, reg_cne001, Vpp1, Rston, 0, Reg_Pwr_Mgr_Sw_Cfg0_Offset);
                    WRITE_SW_CFG_REG_VIDEO_CNE001(0, reg_cne001, Vpp1, Clken, 1, Reg_Pwr_Mgr_Sw_Cfg0_Offset);
                    WRITE_SW_CFG_REG_VIDEO_CNE001(0, reg_cne001, Vpp1, Rston, 1, Reg_Pwr_Mgr_Sw_Cfg0_Offset);
                    WRITE_SW_CFG_REG_VIDEO_CNE001(0, reg_cne001, Vpp1, Isoon, 0, Reg_Pwr_Mgr_Sw_Cfg0_Offset);
                }
                else if (adapter->pm_caps.vpp1_cg_manual && !adapter->pm_caps.vpp1_pg_manual)
                {
                    WRITE_SW_CFG_REG_VIDEO_CNE001(0, reg_cne001, Vpp1, Clken, 1, Reg_Pwr_Mgr_Sw_Cfg0_Offset);
                }
                break;
            default:
                  break;
        }

        power_trace("[CG] engine %d do cg power on,  CR01:%x\n", sch_mgr->engine_index, read_reg_e3k(adapter->mmio, CR_C, 0x01));
    }

    zx_spin_unlock_irqrestore(sch_global->power_status_lock, flags);

    return ret;
}

int vidsch_power_clock_on_off_e3k(vidsch_mgr_t *sch_mgr, unsigned int off)
{
    adapter_t * adapter = sch_mgr->adapter;
    int ret = S_OK;
    if(adapter->chip_id == CHIP_CHX004)
    {
        ret = vidsch_power_clock_on_off_004(sch_mgr, off);
    }
    else if(adapter->chip_id == CHIP_CNE001)
    {
        ret = vidsch_power_clock_on_off_cne001(sch_mgr, off);
    }
    return ret;
}

void vidsch_power_tuning_e3k(adapter_t *adapter)
{
    vidsch_global_t* sch_global = adapter->sch_global;
    vidsch_mgr_t  *sch_mgr = NULL;
    int engine_index;

    unsigned int engine_pwm_auto_mode [] = {
    adapter->pm_caps.gfx_pg_auto, //RB_INDEX_GFXL
    adapter->pm_caps.gfx_pg_auto, //RB_INDEX_GFXH
    1, //RB_INDEX_CSH
    1, //RB_INDEX_CSL0
    1, //RB_INDEX_CSL1
    1, //RB_INDEX_CSL2
    1, //RB_INDEX_CSL3
    adapter->pm_caps.vcp0_pg_auto, //RB_INDEX_VCP0
    adapter->pm_caps.vcp1_pg_auto, //RB_INDEX_VCP1
    adapter->pm_caps.vcp2_pg_auto, //RB_INDEX_VCP2
    adapter->pm_caps.vpp0_pg_auto, //RB_INDEX_VPP0
    adapter->pm_caps.vpp1_pg_auto, //RB_INDEX_VPP1
    };

    if(adapter->chip_id == CHIP_CNE001)
    {
        engine_pwm_auto_mode[RB_INDEX_VCP1] = adapter->pm_caps.vcp1_pg_auto;
        engine_pwm_auto_mode[RB_INDEX_VCP2] = adapter->pm_caps.vcp2_pg_auto;
    }

    if (!adapter->pwm_level.EnableClockGating || !adapter->pm_caps.pwm_manual)
    {
        return;
    }

    for (engine_index = adapter->active_engine_count-1; engine_index >= 0; engine_index--)
    {
        int ret = S_OK;
        unsigned long flags=0;
        sch_mgr = adapter->sch_mgr[engine_index];

        if (sch_mgr == NULL)  continue;

        if (engine_pwm_auto_mode[sch_mgr->engine_index]) continue;

        vidsch_update_engine_idle_status(adapter, (1 << engine_index));
        if (sch_mgr->completely_idle)
        {
            flags = zx_spin_lock_irqsave(sch_mgr->power_status_lock);
            if (sch_mgr->chip_func->power_clock &&
                sch_mgr->engine_dvfs_power_on &&
                vidsch_is_fence_back(sch_mgr->adapter, sch_mgr->engine_index, sch_mgr->last_send_fence_id))
            {
                ret = sch_mgr->chip_func->power_clock(sch_mgr, TRUE);
                if (S_OK == ret)
                {
                    sch_mgr->engine_dvfs_power_on = FALSE;
                }
            }
            zx_spin_unlock_irqrestore(sch_mgr->power_status_lock, flags);
        }
    }
}

void vidschi_adjust_gfx_voltage_cne001(adapter_t *adapter)
{
#define MMIO_C3D_AVS_EN 0x68910
#define MMIO_C3D_AVS_CMD_FIFO 0x68120
#define MMIO_C3D_DVFS_REG_CTRL 0x680d4
#define MMIO_C3D_DVFS_CFG 0x680c0
    unsigned char* pRegAddr = NULL;
    unsigned int value = 0;
    unsigned int avs_cmd = 0x10802183;
    // enable avs command
    pRegAddr = adapter->mmio + MMIO_C3D_AVS_EN;
    zx_write32(pRegAddr, 0x1);

    // write AVS cmd.
    // CMD_INT_EN       [31]
    // CMD_INT_DEST     [30]
    // CMD_OP_CODE      [29:24]  AVS cmd op code :0x10
    // CMD_GRP_EN       [23]     suggest setting the value to 1
    // Reserved         [22:16]
    // Voltage message  [15:0]    //please refer to the DVFS TABLE in //hw/CNE001/design/PMU/CNE001_PMU_register_spec.xlsx for details.
    pRegAddr = adapter->mmio + MMIO_C3D_AVS_CMD_FIFO;
    zx_write32(pRegAddr, avs_cmd);
    zx_udelay(1000);

    pRegAddr = adapter->mmio + MMIO_C3D_DVFS_REG_CTRL;
    value = zx_read32(pRegAddr);
    zx_info("MMIO_C3D_DVFS_REG_CTRL(0x%x): 0x%x", MMIO_C3D_DVFS_REG_CTRL, value);

    if (adapter->hw_caps.zx_set_high_eclk)
    {
        // read the dvfs table index range
        pRegAddr = adapter->mmio + MMIO_C3D_DVFS_CFG;
        value = zx_read32(pRegAddr);

        // force set the dvfs table index to the max index that means eclk 600M
        value = (value & 0xfffffff) >> 24;
        zx_write32(adapter->mmio + MMIO_C3D_DVFS_REG_CTRL, value << 28);
    }
}

