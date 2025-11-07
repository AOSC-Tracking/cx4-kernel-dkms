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
#ifndef __CHIP_INCLUDE_E3K_H__
#define __CHIP_INCLUDE_E3K_H__

#undef __inline
#undef _inline
#undef __int64

#define __inline static __inline__
#define _inline  static __inline__
#define  __int64 long long

#if !defined(XFree86Server)

/* why define ULONG/DWORD as unsigned int in linux ?
 * long is 32-bit type in window, and window use ULONG/DWORD as a 32-bit value,
 * but in linux its size equal to OS ptr size. different between 32/64-bit OS,
 * So we can not define ULONG to unsigned long in linux, but define it as unsigned int for compatible
 */
typedef unsigned int   ULONG;
typedef unsigned int   DWORD;
typedef int            BOOL;
typedef unsigned char  BYTE;
typedef unsigned int   UINT;
typedef unsigned short WORD;
typedef unsigned int   U32;
typedef void           VOID;
typedef float          FLOAT;
typedef unsigned long long UINT64;

#endif

#include "./Chip/surface_format.h"
#include "./Chip/registerDef.h"
#include "./Chip/registercommands.h"
#include "stm_context_e3k.h"


#define MAX_IGAS_E3K  MAX_CORE_CRTCS

typedef enum
{
    HWM_SYNC_VS_SLOT  = 0,
    HWM_SYNC_STO_SLOT = 1,
    HWM_SYNC_Z_SLOT   = 2,
    HWM_SYNC_PS_SLOT  = 3,
    HWM_SYNC_WBU_SLOT = 4,
    HWM_SYNC_MXU_SLOT = 5,
    HWM_SYNC_L2_SLOT  = 6,

    HWM_SYNC_SECONDARY_CMDBUF_STARTSLOT = HWM_SYNC_L2_SLOT + 1,
    HWM_SYNC_VS_SLOT_SECONDARY = 7,
    HWM_SYNC_STO_SLOT_SECONDARY = 8,
    HWM_SYNC_Z_SLOT_SECONDARY = 9,
    HWM_SYNC_PS_SLOT_SECONDARY = 10,
    HWM_SYNC_WBU_SLOT_SECONDARY = 11,
    HWM_SYNC_MXU_SLOT_SECONDARY = 12,
    HWM_SYNC_L2_SLOT_SECONDARY = 13,

    HWM_SYNC_MAX_SLOT = HWM_SYNC_L2_SLOT_SECONDARY + 1,
    HWM_SYNC_CONTEXT_LAST = HWM_SYNC_MAX_SLOT,
    HWM_SYNC_UMD_FLUSH_SLOT = 14,
    HWM_SYNC_DRAIN_3DPIPE_SLOT = 15,

    // Above are slots used by 3D UMD
    HWM_SYNC_KMD_SLOT = 16,

    //Slot used by video
    HWM_SYNC_VCP0_FE_SLOT = 17,
    HWM_SYNC_VCP0_BE_SLOT = 18,
    HWM_SYNC_VCP1_FE_SLOT = 19,
    HWM_SYNC_VCP1_BE_SLOT = 20,
    HWM_SYNC_VCP2_FE_SLOT = 21,
    HWM_SYNC_VCP2_BE_SLOT = 22,
    HWM_SYNC_VCP3_FE_SLOT = 23,
    HWM_SYNC_VCP3_BE_SLOT = 24,
    HWM_SYNC_VPP0_SLOT    = 25,
    HWM_SYNC_VPP1_SLOT    = 26,
    HWM_SYNC_VCP_BANDWIDTH = 27,

    //Special for Executed indirect
    HWM_SYNC_INDIRECT_L2_SLOT = 28,
    HWM_SYNC_INDIRECT_L2_SLOT_SECONDARY = 29,

} HWM_SYNC_SLOT_E3K;


#define FENCE_VALUE_RESET                   0x000000FF
#define FENCE_VALUE_BEGIN_DMA               0x00000010
#define FENCE_VALUE_END_DMA_CSPFENCE        0x00000021
#define FENCE_VALUE_END_DMA_STOFENCE        0x00000022
#define FENCE_VALUE_CACHE_DMA_DFENCE        0x00000031
#define FENCE_VALUE_CACHE_DMA_ZFENCE        0x00000032
#define FENCE_VALUE_CACHE_DMA_UAVFEFENCE    0x00000033
#define FENCE_VALUE_CACHE_DMA_UAVBEFENCE    0x00000034
#define FENCE_VALUE_CACHE_DMA_PRECSPFENCE   0x00000035
#define FENCE_VALUE_CACHE_DMA_POSTCSPFENCE  0x00000036
#define FENCE_VALUE_CACHE_DMA_STOFENCE      0x00000037
#define FENCE_VALUE_CACHE_DMA_L2C_FENCE     0x00000038

#define FENCE_VALUE_SET_MXU_REG_DFENCE      0x00000039
#define FENCE_VALUE_HANG_DUMP_DFENCE        0x00000040

#define FENCE_VALUE_FAST_CLEAR_POST_D_FENCE 0x00000058

#define FENCE_VALUE_INIT                    0x00000006
#define FENCE_VALUE_DUMMY_RB                0x00000007
#define FENCE_VALUE_RANGESET                0x00000003

#define FENCE_VALUE_FFC_D_FECNE (0x00000050)
#define FENCE_VALUE_2D_BLT_DST  (0x00000060)
#define FENCE_VALUE_3D_BLT_DST  (0x00000070)
#define FENCE_VALUE_3D_BLT_SRC  (0x00000071)



#define REG_FS_OUT_MAX_ATTRIBUTES_NUM           48

#define REG_FS_OUT_MAPPING_ATTRIBUTES_BITS      6
#define REG_FS_OUT_MAPPING_ATTRIBUTE_NUM        4
#define REG_FS_OUT_MAPPING_REGS_NUM             12

#define REG_FS_OUT_MASK_ATTRIBUTES_BITS         4
#define REG_FS_OUT_MASK_ATTRIBUTES_NUM          8
#define REG_FS_OUT_MASK_REGS_NUM                6

#define REG_FS_OUT_COMP_ATTRIBUTES_BITS         2
#define REG_FS_OUT_COMP_ATTRIBUTES_NUM          16
#define REG_FS_OUT_COMP_REGS_NUM                3


typedef enum ELITE_SHADER_STAGE
{
    ELITE_VS_STAGE  = 1,
    ELITE_HS_STAGE ,
    ELITE_DS_STAGE ,
    ELITE_GS_STAGE ,
    ELITE_PS_STAGE ,
}ELITE_SHADER_STAGE;


#define MAX_T_SHARP_NUMBER  128
#define MAX_S_SHARP_NUMBER  16
#define MAX_TS_SHARP_NUMBER 32
#define MAX_U_SHARP_NUMBER  32

#define HWM_SYNC_2DBLT_DST      0x2180000A
#define HWM_SYNC_EU_FE          0x31b10002
#define HWM_SYNC_EU_BE          0x31b20006
#define HWM_SYNC_D              0x31b30008
#define HWM_SYNC_Z              0x31b4000C
#define HWM_SYNC_STO                   0x01110007

#define HWM_SYNC_USAGETYPE_MASK 0x000000FF
#define HwmUsageType(Usage)     (Usage&HWM_SYNC_USAGETYPE_MASK)

#define BCI_MAX_COORD_E3K                  0x3fff

#define HWM_SYNC_MAX_TYPE       0x10  // must be consistent with the following definitions
#define HWM_SYNC_MAX_READTYPE   0x10  // must be consistent with the following definitions

#define HWM_SYNC_READ           0x00000000
#define HWM_SYNC_WRITE          0x01000000

#define HWM_SYNC_MODE_MASK      0xF0000000
#define HWM_SYNC_ACCESS_MASK    0x0F000000
#define HWM_SYNC_USAGEID_MASK   0x00FF0000

#define HWM_SYNC_INVALIDATE_L2_CACHE   0x011A000E
#define HWM_SYNC_FFC_D_FLUSH           0x01160008
#define HWM_SYNC_QUERY_DUMP     0x218e000D
#define HWM_SYNC_AUTOCLEAR      0x218c000D

#endif
