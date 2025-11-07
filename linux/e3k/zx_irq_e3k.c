//*****************************************************************************
//  Copyright (c) 2018 Shanghai Zhaoxin Semiconductor Co., Ltd.
//  All Rights Reserved.
//
//  This is UNPUBLISHED PROPRIETARY SOURCE CODE of Shanghai Zhaoxin Semiconductor Co., Ltd.;
//  the contents of this file may not be disclosed to third parties, copied or
//  duplicated in any form, in whole or in part, without the prior written
//  permission of Shanghai Zhaoxin Semiconductor Co., Ltd.
//
//  The copyright of the source code is protected by the copyright laws of the People's
//  Republic of China and the related laws promulgated by the People's Republic of China
//  and the international covenant(s) ratified by the People's Republic of China.
//*****************************************************************************
#include "zx_irq_e3k.h"

static void zx_translate_interrupt_bits(disp_info_t* disp_info, int sw2hw, intr_info_t* info, unsigned long long* masks)
{
    adapter_info_t*  adapter = disp_info->adp_info;

    if(!info || !masks)
    {
        return;
    }

    if(sw2hw)
    {
        info->biu_intr_bits = 0;
        info->csp_intr_bits = 0;
        info->pmu_intr_bits = 0;

        info->biu_intr_bits |= (*masks & INT_VSYNC1)? VSYNC1_INT : 0;
        info->biu_intr_bits |= (*masks & INT_VSYNC2)? VSYNC2_INT : 0;
        info->biu_intr_bits |= (*masks & INT_DP1)? DP1_INT : 0;
        info->biu_intr_bits |= (*masks & INT_HDCODEC)? HDCODEC_INT : 0;

        info->biu_intr_bits |= (*masks & INT_VSYNC3)? VSYNC3_INT_CHX004 : 0;
        info->biu_intr_bits |= (*masks & INT_DP2)? DP2_INT_CHX004 : 0;
        info->biu_intr_bits |= (*masks & INT_PAGE_FAULT) ? PAGE_FAULT_INT_CHX004 : 0;
        info->csp_intr_bits |= (*masks & INT_FENCE)? ENGINE_FENCE_INT_CHX004 : 0;
        info->csp_intr_bits |= (*masks & INT_FE_HANG_VD0)? FE_HANG_VD0_INT : 0;
        info->csp_intr_bits |= (*masks & INT_BE_HANG_VD0)? BE_HANG_VD0_INT : 0;
        info->csp_intr_bits |= (*masks & INT_FE_ERROR_VD0)? FE_ERROR_VD0_INT : 0;
        info->csp_intr_bits |= (*masks & INT_BE_ERROR_VD0)? BE_ERROR_VD0_INT : 0;
        info->csp_intr_bits |= (*masks & INT_FE_HANG_VD1)? FE_HANG_VD1_INT : 0;
        info->csp_intr_bits |= (*masks & INT_BE_HANG_VD1)? BE_HANG_VD1_INT : 0;
        info->csp_intr_bits |= (*masks & INT_FE_ERROR_VD1)? FE_ERROR_VD1_INT : 0;
        info->csp_intr_bits |= (*masks & INT_BE_ERROR_VD1)? BE_ERROR_VD1_INT : 0;
        if(adapter->chip_id == CHIP_CNE001)
        {
            info->csp_intr_bits |= (*masks & INT_FE_HANG_VD2)  ? FE_HANG_VD2_INT  : 0;
            info->csp_intr_bits |= (*masks & INT_BE_HANG_VD2)  ? BE_HANG_VD2_INT  : 0;
            info->csp_intr_bits |= (*masks & INT_FE_ERROR_VD2) ? FE_ERROR_VD2_INT : 0;
            info->csp_intr_bits |= (*masks & INT_BE_ERROR_VD2) ? BE_ERROR_VD2_INT : 0;
            info->csp_intr_bits |= (*masks & INT_DP3)? DP3_INT_CNE001 : 0;

            info->biu_intr_bits |= (*masks & INT_PMU_CPU)? PMU_CPU_INT : 0;
            info->pmu_intr_bits |= (*masks & INT_C3D_REFUCE)? C3D_PERF_REDUCE_INT : 0;
        }
    }
    else
    {
        *masks = 0;
        *masks |= (info->biu_intr_bits & VSYNC1_INT)? INT_VSYNC1 : 0;
        *masks |= (info->biu_intr_bits & VSYNC2_INT)? INT_VSYNC2 : 0;
        *masks |= (info->biu_intr_bits & DP1_INT)? INT_DP1 : 0;
        *masks |= (info->biu_intr_bits & HDCODEC_INT)? INT_HDCODEC : 0;

        *masks |= (info->biu_intr_bits & VSYNC3_INT_CHX004)? INT_VSYNC3 : 0;
        *masks |= (info->biu_intr_bits & DP2_INT_CHX004)? INT_DP2 : 0;
        *masks |= (info->biu_intr_bits & PAGE_FAULT_INT_CHX004) ? INT_PAGE_FAULT : 0;
        *masks |= (info->csp_intr_bits & ENGINE_FENCE_INT_CHX004)? INT_FENCE : 0;
        *masks |= (info->csp_intr_bits & FE_HANG_VD0_INT)? INT_FE_HANG_VD0 : 0;
        *masks |= (info->csp_intr_bits & BE_HANG_VD0_INT)? INT_BE_HANG_VD0 : 0;
        *masks |= (info->csp_intr_bits & FE_ERROR_VD0_INT)? INT_FE_ERROR_VD0 : 0;
        *masks |= (info->csp_intr_bits & BE_ERROR_VD0_INT)? INT_BE_ERROR_VD0 : 0;
        *masks |= (info->csp_intr_bits & FE_HANG_VD1_INT)? INT_FE_HANG_VD1 : 0;
        *masks |= (info->csp_intr_bits & BE_HANG_VD1_INT)? INT_BE_HANG_VD1 : 0;
        *masks |= (info->csp_intr_bits & FE_ERROR_VD1_INT)? INT_FE_ERROR_VD1 : 0;
        *masks |= (info->csp_intr_bits & BE_ERROR_VD1_INT)? INT_BE_ERROR_VD1 : 0;
        if(adapter->chip_id == CHIP_CNE001)
        {
            *masks |= (info->csp_intr_bits & DP3_INT_CNE001) ? INT_DP3 : 0;

            *masks |= (info->biu_intr_bits & PMU_CPU_INT)? INT_PMU_CPU : 0;
            *masks |= (info->pmu_intr_bits & C3D_PERF_REDUCE_INT)? INT_C3D_REFUCE : 0;

            *masks |= (info->csp_intr_bits & FE_HANG_VD2_INT)   ? INT_FE_HANG_VD2   : 0;
            *masks |= (info->csp_intr_bits & BE_HANG_VD2_INT)   ? INT_BE_HANG_VD2   : 0;
            *masks |= (info->csp_intr_bits & FE_ERROR_VD2_INT)  ? INT_FE_ERROR_VD2  : 0;
            *masks |= (info->csp_intr_bits & BE_ERROR_VD2_INT)  ? INT_BE_ERROR_VD2  : 0;
        }
    }
}

static unsigned long long zx_get_default_intr_e3k(disp_info_t* disp_info)
{
    unsigned long long intr = DEF_INTR;
    adapter_info_t *adapter = disp_info->adp_info;
    zx_card_t      *zx_card = disp_info->zx_card;
    krnl_adapter_init_info_t *info  = &zx_card->a_info;

    //default disable dvfs interrupt since it's too much
    //if(info->zx_dvfs_mode == 0)
    {
        intr &= ~INT_DVFS;
    }
    if (adapter->chip_id != CHIP_CNE001)
    {
        intr &= ~INT_PMU_CPU;
    }

    return intr;
}

static unsigned long long zx_get_intr_enable_mask_e3k(disp_info_t* disp_info)
{
    adapter_info_t*  adapter = disp_info->adp_info;
    intr_info_t intr_info = {0};
    unsigned long long masks = 0;

    intr_info.biu_intr_bits = zx_read32(adapter->mmio + INTR_EN_REG);
    intr_info.csp_intr_bits = zx_read32(adapter->mmio + ADV_INTR_EN_REG);

    if(intr_info.biu_intr_bits & PMU_CPU_INT)
    {
        intr_info.pmu_intr_bits = zx_read32(adapter->mmio + PMU_CPU_INT_BASE + PMU_INTR_EN_REG);
        zx_write32(adapter->mmio + PMU_CPU_INT_BASE + PMU_INTR_IRQ_CTL_REG, intr_info.pmu_intr_bits);
    }

    zx_translate_interrupt_bits(disp_info, 0, &intr_info, &masks);

    return masks;
}

static void zx_set_intr_enable_mask_e3k(disp_info_t* disp_info, unsigned long long masks)
{
    adapter_info_t*  adapter = disp_info->adp_info;
    intr_info_t intr_info = {0};
    unsigned char mm8aa0;
    unsigned int reg_value = 0;

    zx_translate_interrupt_bits(disp_info, 1, &intr_info, &masks);

    zx_write32(adapter->mmio + INTR_EN_REG, intr_info.biu_intr_bits);

    zx_write32(adapter->mmio + ADV_INTR_EN_REG, intr_info.csp_intr_bits);

    if(intr_info.biu_intr_bits & PMU_CPU_INT)
    {
        zx_write32(adapter->mmio + PMU_CPU_INT_BASE + PMU_C3D_AUTO_REG_CNT_MAX, PMU_C3D_REDUCE_CTRL_CNT);
        zx_write32(adapter->mmio + PMU_CPU_INT_BASE + PMU_AUTO_REG_CNT_EN, 1);
        zx_write32(adapter->mmio + PMU_CPU_INT_BASE+ PMU_INTR_EN_REG, intr_info.pmu_intr_bits);
        if (adapter->chip_id == CHIP_CNE001)
        {
            //In fact, it's more reasonable to write this in vidschi_init_hw_setting, but it will cause
            //system can't enter and print "Rebooting in 30 seconds", need hw check later, temp write it here
            reg_value = zx_read32(adapter->mmio + PMU_CPU_INT_BASE + 0x8154);
            if(!(reg_value & 0x1))
            {
                reg_value |= 0x1;//enable PMU SVID request timeout logic
                zx_write32(adapter->mmio + PMU_CPU_INT_BASE + 0x8154,reg_value);
            }
        }
    }

    mm8aa0 = zx_read8(adapter->mmio + 0x8AA0);
    if(!masks && (mm8aa0 & 0x01))
    {
        //disable chip interrupt case
        zx_write8(adapter->mmio + 0x8AA0, mm8aa0 & 0xFE);    //disable adapter interrupt
        zx_write32(adapter->mmio + INTR_SHADOW_REG, 0x00);   //Clear all interrupts with any write to MM8574
    }
    else if(masks && !(mm8aa0 & 0x01))
    {
        //enable chip interrupt case
        mm8aa0 = zx_read8(adapter->mmio + 0x8AA0);
        zx_write8(adapter->mmio + 0x8AA0, mm8aa0 | 0x01);
        zx_write32(adapter->mmio + INTR_SHADOW_REG, 0x00);   //Clear all interrupts with any write to MM8574
    }
}

static void zx_enable_msi_e3k(disp_info_t* disp_info)
{
    unsigned int        tmp;
    adapter_info_t*  adapter = disp_info->adp_info;
    int  msi_reg1, msi_reg2;

    msi_reg1 = 0x80C4;
    msi_reg2 = 0x80C0;

    //when MSI is turned on from registry,the MSI address should have be written here by OS

    tmp = zx_read32(adapter->mmio + msi_reg1);

    if (tmp != 0)
    {
        tmp = zx_read32(adapter->mmio + msi_reg2);
        if ((tmp & 0x10000) == 0)
        {
            // Before turn on MSI we need clear all pending interrupts, clear all BIU interrupts
            zx_write32(adapter->mmio + INTR_SHADOW_REG, 0);

            zx_write32(adapter->mmio + msi_reg2, tmp | 0x10000);
        }
    }
}

static void zx_disable_msi_e3k(disp_info_t* disp_info)
{
    unsigned int        temp;
    adapter_info_t*  adapter = disp_info->adp_info;
    int msi_reg;

    msi_reg = 0x80C0;

    temp = zx_read32(adapter->mmio + msi_reg);
    temp &= ~0x10000;

    zx_write32(adapter->mmio + msi_reg, temp);
}

static unsigned long long zx_get_interrupt_mask_e3k(disp_info_t* disp_info)
{
    adapter_info_t*  adapter = disp_info->adp_info;
    intr_info_t intr_info = {0};
    unsigned long long intr_masks = 0;

    zx_write32(adapter->mmio + INTR_SHADOW_REG, 0);
    zx_mb();

    intr_info.biu_intr_bits = zx_read32(adapter->mmio + INTR_SHADOW_REG);
    intr_info.csp_intr_bits = zx_read32(adapter->mmio + ADV_SHADOW_REG);

    if(intr_info.biu_intr_bits & PMU_CPU_INT)
    {
        intr_info.pmu_intr_bits = zx_read32(adapter->mmio + PMU_CPU_INT_BASE + PMU_INTR_STATUS_REG);
        zx_write32(adapter->mmio + PMU_CPU_INT_BASE+ PMU_INTR_IRQ_CTL_REG, intr_info.pmu_intr_bits);
    }

    zx_translate_interrupt_bits(disp_info, 0, &intr_info, &intr_masks);

    return intr_masks;
}

static void zx_detect_pmu_intr_conflict_recovery_e3k(disp_info_t* disp_info,unsigned long long intr_masks)
{
    adapter_info_t*  adapter = disp_info->adp_info;
    intr_info_t intr_info = {0};

    zx_translate_interrupt_bits(disp_info, 1, &intr_info, &intr_masks);

    if(intr_info.biu_intr_bits & PMU_CPU_INT)
    {
        intr_info.pmu_intr_bits = zx_read32(adapter->mmio + PMU_CPU_INT_BASE + PMU_INTR_STATUS_REG);
        zx_write32(adapter->mmio + PMU_CPU_INT_BASE+ PMU_INTR_IRQ_CTL_REG, intr_info.pmu_intr_bits);
    }
}

irq_chip_funcs_t irq_chip_funcs =
{
    .get_default_intr = zx_get_default_intr_e3k,
    .get_intr_enable_mask = zx_get_intr_enable_mask_e3k,
    .set_intr_enable_mask = zx_set_intr_enable_mask_e3k,
    .enable_msi = zx_enable_msi_e3k,
    .disable_msi = zx_disable_msi_e3k,
    .get_interrupt_mask = zx_get_interrupt_mask_e3k,

#if ENABLE_PMU_CPU_INTR
    .detect_pmu_intr_conflict_recovery = zx_detect_pmu_intr_conflict_recovery_e3k,
#endif
};
