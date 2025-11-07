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
#ifndef _ZX_IRQ_E3K_
#define _ZX_IRQ_E3K_

#include "../zx_irq.h"


#define INTR_EN_REG                 0x8508
#define ADV_INTR_EN_REG             0x854c
#define INTR_SHADOW_REG             0x8574
#define ADV_SHADOW_REG              0x8578

#define ENABLE_PMU_CPU_INTR     1

#define PMU_CPU_INT_BASE            0x60000
#define PMU_INTR_EN_REG             0x8858
#define PMU_INTR_STATUS_REG         0x887c
#define PMU_INTR_IRQ_CTL_REG        0x8870
#define PMU_AUTO_REG_CNT_EN         0x80e4
#define PMU_C3D_AUTO_REG_CNT_MAX    0x80ec

#define PMU_C3D_REDUCE_CTRL_CNT     0x7ff

typedef enum _BIU_INTR_BIT
{
    VSYNC1_INT  = 0x1,
    VSYNC2_INT  = 0x4,
    VSYNC3_INT  = 0x8,
    VSYNC4_INT  = 0x10,
    DP2_INT_CHX004 = 0x10,
    DP1_INT     = 0x80,
    DP2_INT     = 0x100,
    DP3_INT     = 0x10000,
    DP4_INT     = 0x20000,
    VIP1_INT    = 0x1000000,
    HDCODEC_INT = 0x4000000,
    VIP2_INT    = 0x8000000,
    VIP3_INT    = 0x10000000,
    VIP4_INT    = 0x40000000,
    VSYNC3_INT_CHX004 = 0x40000000,
    PAGE_FAULT_INT_CHX004 = 0x10000000,

    PMU_CPU_INT     = 0x10000,
}BIU_INTR_BIT;

typedef enum _CSP_INTR_BIT
{
    FE_HANG_VD0_INT  = 0x1,
    BE_HANG_VD0_INT  = 0x2,
    FE_ERROR_VD0_INT = 0x4,
    BE_ERROR_VD0_INT = 0x8,
    FE_HANG_VD1_INT  = 0x10,
    BE_HANG_VD1_INT  = 0x20,
    FE_ERROR_VD1_INT = 0x40,
    BE_ERROR_VD1_INT = 0x80,
    FE_HANG_VD2_INT  = 0x100,
    BE_HANG_VD2_INT  = 0x200,
    FE_ERROR_VD2_INT = 0x400,
    BE_ERROR_VD2_INT = 0x800,
    ENGINE_FENCE_INT_CHX004 = 0x10000,
    DP3_INT_CNE001   = 0x100000,
    ENGINE_FENCE_INT = 0x8000000,
}CSP_INTR_BIT;

typedef enum PMU_CPU_INTR_BIT
{
    C3D_PERF_REDUCE_INT  = 0x4000,
}CPU_INTR_BIT;

typedef struct _intr_info
{
    unsigned int biu_intr_bits;
    unsigned int csp_intr_bits;
    unsigned int pmu_intr_bits;
}intr_info_t;

#endif
