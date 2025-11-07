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
//    Spec Last Modified Time: 15:13 2019-11-22
#ifndef _WLS_REGISTER_H
#define _WLS_REGISTER_H


#ifndef        WLS_FE_BLOCKBASE_INF
    #define    WLS_FE_BLOCKBASE_INF
    #define    BLOCK_WLS_FE_VERSION 1
    #define    BLOCK_WLS_FE_TIMESTAMP "15:13 2019-11-22"
    #define    WLS_FE_BLOCK                                           0x8 // match with BlockID.h
    #define    WLS_FE_REG_START                                       0x0 // match with BlockID.h
    #define    WLS_FE_REG_END                                         0x418 // match with BlockID.h
    #define    WLS_FE_REG_LIMIT                                       0x418 // match with BlockID.h
#endif

#ifndef        WLS_BE_BLOCKBASE_INF
    #define    WLS_BE_BLOCKBASE_INF
    #define    BLOCK_WLS_BE_VERSION 1
    #define    BLOCK_WLS_BE_TIMESTAMP "09:54 2019-09-05"
    #define    WLS_BE_BLOCK                                           0x13// match with BlockID.h
    #define    WLS_BE_REG_START                                       0x0 // match with BlockID.h
    #define    WLS_BE_REG_END                                         0x418 // match with BlockID.h
    #define    WLS_BE_REG_LIMIT                                       0x418 // match with BlockID.h
#endif

// Register Offset definition
#define        Reg_Ffc_Ubuf_Golbalconfig_Offset                           0x0
#define        Reg_Ffc_Ubuf_3dconfig_Offset                               0x1
#define        Reg_Descriptor_Heap_Base_Offset                            0x2
#define        Reg_Wls_Reserved0_Offset                                   0xa
#define        Reg_Wls_Reserved1_Offset                                   0xb
#define        Reg_Wls_Reserved2_Offset                                   0xc
#define        Reg_Wls_Reserved3_Offset                                   0xd
#define        Reg_Wls_Reserved4_Offset                                   0xe
#define        Reg_Wls_Reserved5_Offset                                   0xf
#define        Reg_Ffc_Ubuf_Csconfig_Offset                               0x10
#define        Reg_Wls_Reserved6_Offset                                   0x11
#define        Reg_Uav_Group_Offset                                       0x18

// Block constant definition
typedef enum
{
    FFC_UBUF_3DCONFIG_D3D_U_SHARP_CACHED_DISABLED= 0,
    FFC_UBUF_3DCONFIG_D3D_U_SHARP_CACHED_ENABLED= 1,
} FFC_UBUF_3DCONFIG_D3D_U_SHARP_CACHED;

typedef enum
{
    FFC_UBUF_CSCONFIG_CSL_U_SHARP_CACHED_DISABLED= 0,
    FFC_UBUF_CSCONFIG_CSL_U_SHARP_CACHED_ENABLED= 1,
} FFC_UBUF_CSCONFIG_CSL_U_SHARP_CACHED;

typedef enum
{
    FFC_UBUF_CSCONFIG_CSH_U_SHARP_CACHED_DISABLED= 0,
    FFC_UBUF_CSCONFIG_CSH_U_SHARP_CACHED_ENABLED= 1,
} FFC_UBUF_CSCONFIG_CSH_U_SHARP_CACHED;

typedef enum
{
    UAV_CTL_U_SHARP_EN_DISABLE               = 0,
    UAV_CTL_U_SHARP_EN_ENABLE                = 1,
} UAV_CTL_U_SHARP_EN;

typedef enum
{
    UAV_CTL_RESOURCE_TYPE_1D_BUFFER          = 0,
    UAV_CTL_RESOURCE_TYPE_1D_TEXTURE         = 1,
    UAV_CTL_RESOURCE_TYPE_2D_TEXTURE         = 2,
    UAV_CTL_RESOURCE_TYPE_3D_TEXTURE         = 3,
    UAV_CTL_RESOURCE_TYPE_1D_TEXTURE_ARRAY   = 5,
    UAV_CTL_RESOURCE_TYPE_2D_TEXTURE_ARRAY   = 6,
    UAV_CTL_RESOURCE_TYPE_STRUCTURED_BUFFER  = 9,
    UAV_CTL_RESOURCE_TYPE_UNTYPED            = 11,
} UAV_CTL_RESOURCE_TYPE;

typedef enum
{
    UAV_CTL_UAV_LAYOUT_HORZ_LINEAR           = 0,
    UAV_CTL_UAV_LAYOUT_VERT_TILED            = 1,
} UAV_CTL_UAV_LAYOUT;

typedef enum
{
    UAV_CTL_OUT_OF_BOUND_MODE_DEFAULT        = 0,
    UAV_CTL_OUT_OF_BOUND_MODE_CLAMP          = 1,
    UAV_CTL_OUT_OF_BOUND_MODE_WRAP           = 2,
} UAV_CTL_OUT_OF_BOUND_MODE;

typedef enum
{
    UAV_CTL_MIPMAP_EN_DISABLE                = 0,
    UAV_CTL_MIPMAP_EN_ENABLE                 = 1,
} UAV_CTL_MIPMAP_EN;

typedef enum
{
    UAV_CTL_L1CACHABLE_DISABLE               = 0,
    UAV_CTL_L1CACHABLE_ENABLE                = 1,
} UAV_CTL_L1CACHABLE;

typedef enum
{
    UAV_CTL_L2CACHABLE_DISABLE               = 0,
    UAV_CTL_L2CACHABLE_ENABLE                = 1,
} UAV_CTL_L2CACHABLE;

typedef enum
{
    UAV_CTL_KKK_INFO_NORMAL_UAV              = 0,
    UAV_CTL_KKK_INFO_KKK_UAV                 = 1,
} UAV_CTL_KKK_INFO;


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////            WLS_FE Block (Block ID = 0x8) Register Definitions                      ///////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define Reg_FFC_UBUF_GOLBALCONFIG_U_3d_Base_BitField_MaxValue ((1u << 5) - 1)
#define Reg_FFC_UBUF_GOLBALCONFIG_Reserved_BitField_MaxValue ((1u << 27) - 1)

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int U_3d_Base                 : 5;
        unsigned int Reserved                  : 27;
    } reg;
} Reg_Ffc_Ubuf_Golbalconfig;

#define Reg_FFC_UBUF_3DCONFIG_U_Ps_Base_BitField_MaxValue ((1u << 5) - 1)
#define Reg_FFC_UBUF_3DCONFIG_D3d_U_Sharp_Cached_BitField_MaxValue ((1u << 1) - 1)
#define Reg_FFC_UBUF_3DCONFIG_Reserved_BitField_MaxValue ((1u << 26) - 1)

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int U_Ps_Base                 : 5;
        unsigned int D3d_U_Sharp_Cached        : 1;
        unsigned int Reserved                  : 26;
    } reg;
} Reg_Ffc_Ubuf_3dconfig;

#define Reg_DESCRIPTOR_HEAP_BASE_Heap_Addr_BitField_MaxValue ((1u << 32) - 1)

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Heap_Addr                 : 32;
    } reg;
} Reg_Descriptor_Heap_Base;

#define Reg_WLS_RESERVED0_Reserved_BitField_MaxValue ((1u << 32) - 1)

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Reserved                  : 32;
    } reg;
} Reg_Wls_Reserved0;

#define Reg_WLS_RESERVED1_Reserved_BitField_MaxValue ((1u << 32) - 1)

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Reserved                  : 32;
    } reg;
} Reg_Wls_Reserved1;

#define Reg_WLS_RESERVED2_Reserved_BitField_MaxValue ((1u << 32) - 1)

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Reserved                  : 32;
    } reg;
} Reg_Wls_Reserved2;

#define Reg_WLS_RESERVED3_Reserved_BitField_MaxValue ((1u << 32) - 1)

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Reserved                  : 32;
    } reg;
} Reg_Wls_Reserved3;

#define Reg_WLS_RESERVED4_Reserved_BitField_MaxValue ((1u << 32) - 1)

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Reserved                  : 32;
    } reg;
} Reg_Wls_Reserved4;

#define Reg_WLS_RESERVED5_Reserved_BitField_MaxValue ((1u << 32) - 1)

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Reserved                  : 32;
    } reg;
} Reg_Wls_Reserved5;

#define Reg_FFC_UBUF_CSCONFIG_U_Csh_Base_BitField_MaxValue ((1u << 5) - 1)
#define Reg_FFC_UBUF_CSCONFIG_Csl_U_Sharp_Cached_BitField_MaxValue ((1u << 1) - 1)
#define Reg_FFC_UBUF_CSCONFIG_Csh_U_Sharp_Cached_BitField_MaxValue ((1u << 1) - 1)
#define Reg_FFC_UBUF_CSCONFIG_Reserved_BitField_MaxValue ((1u << 25) - 1)

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int U_Csh_Base                : 5;
        unsigned int Csl_U_Sharp_Cached        : 1;
        unsigned int Csh_U_Sharp_Cached        : 1;
        unsigned int Reserved                  : 25;
    } reg;
} Reg_Ffc_Ubuf_Csconfig;

#define Reg_WLS_RESERVED6_Reserved_BitField_MaxValue ((1u << 32) - 1)

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Reserved                  : 32;
    } reg;
} Reg_Wls_Reserved6;

#define Reg_UAV_BASE_Uav_Base_BitField_MaxValue ((1u << 32) - 1)

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Uav_Base                  : 32;
    } reg;
} Reg_Uav_Base;

#define Reg_RESOURCE_SIZE_Width_BitField_MaxValue ((1u << 15) - 1)
#define Reg_RESOURCE_SIZE_Height_BitField_MaxValue ((1u << 15) - 1)
#define Reg_RESOURCE_SIZE_Reserved_BitField_MaxValue ((1u << 2) - 1)

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Width                     : 15;
        unsigned int Height                    : 15;
        unsigned int Reserved                  : 2;
    } reg;
} Reg_Resource_Size;

#define Reg_RESOURCE_STRIDE_Bytestride_Or_Depth_BitField_MaxValue ((1u << 32) - 1)

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Bytestride_Or_Depth       : 32;
    } reg;
} Reg_Resource_Stride;

#define Reg_VIEW_OFFSET_First_Element_BitField_MaxValue ((1u << 32) - 1)

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int First_Element             : 32;
    } reg;
} Reg_View_Offset;

#define Reg_VIEW_SIZE_Num_Element_BitField_MaxValue ((1u << 32) - 1)

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Num_Element               : 32;
    } reg;
} Reg_View_Size;

#define Reg_UAV_CTL_U_Sharp_En_BitField_MaxValue ((1u << 1) - 1)
#define Reg_UAV_CTL_Resource_Type_BitField_MaxValue ((1u << 4) - 1)
#define Reg_UAV_CTL_Uav_Layout_BitField_MaxValue ((1u << 1) - 1)
#define Reg_UAV_CTL_Out_Of_Bound_Mode_BitField_MaxValue ((1u << 2) - 1)
#define Reg_UAV_CTL_Format_BitField_MaxValue ((1u << 9) - 1)
#define Reg_UAV_CTL_Mipmap_En_BitField_MaxValue ((1u << 1) - 1)
#define Reg_UAV_CTL_Lod_BitField_MaxValue ((1u << 4) - 1)
#define Reg_UAV_CTL_L1cachable_BitField_MaxValue ((1u << 1) - 1)
#define Reg_UAV_CTL_L2cachable_BitField_MaxValue ((1u << 1) - 1)
#define Reg_UAV_CTL_Kkk_Info_BitField_MaxValue ((1u << 2) - 1)
#define Reg_UAV_CTL_Range_Type_BitField_MaxValue ((1u << 6) - 1)

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int U_Sharp_En                : 1;
        unsigned int Resource_Type             : 4;
        unsigned int Uav_Layout                : 1;
        unsigned int Out_Of_Bound_Mode         : 2;
        unsigned int Format                    : 9;
        unsigned int Mipmap_En                 : 1;
        unsigned int Lod                       : 4;
        unsigned int L1cachable                : 1;
        unsigned int L2cachable                : 1;
        unsigned int Kkk_Info                  : 2;
        unsigned int Range_Type                : 6;
    } reg;
} Reg_Uav_Ctl;

#define Reg_BL_SLOT_IDX_Bl_Slot_Idx_BitField_MaxValue ((1u << 18) - 1)
#define Reg_BL_SLOT_IDX_Reserved_BitField_MaxValue ((1u << 14) - 1)

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Bl_Slot_Idx               : 18;
        unsigned int Reserved                  : 14;
    } reg;
} Reg_Bl_Slot_Idx;

#define Reg_WLS_CONFIG_Continuous_Optimize_BitField_MaxValue ((1u << 1) - 1)
#define Reg_WLS_CONFIG_Swizzle_Rb_BitField_MaxValue ((1u << 1) - 1)
#define Reg_WLS_CONFIG_Swizzle_A_BitField_MaxValue ((1u << 1) - 1)
#define Reg_WLS_CONFIG_Honor_A_BitField_MaxValue ((1u << 1) - 1)
#define Reg_WLS_CONFIG_Is_Sparse_BitField_MaxValue ((1u << 1) - 1)
#define Reg_WLS_CONFIG_Reserved_BitField_MaxValue ((1u << 27) - 1)

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Continuous_Optimize       : 1;
        unsigned int Swizzle_Rb                : 1;
        unsigned int Swizzle_A                 : 1;
        unsigned int Honor_A                   : 1;
        unsigned int Is_Sparse                 : 1;
        unsigned int Reserved                  : 27;
    } reg;
} Reg_Wls_Config;



typedef struct _Group_Uav_Group
{
    Reg_Uav_Base                     reg_Uav_Base;
    Reg_Resource_Size                reg_Resource_Size;
    Reg_Resource_Stride              reg_Resource_Stride;
    Reg_View_Offset                  reg_View_Offset;
    Reg_View_Size                    reg_View_Size;
    Reg_Uav_Ctl                      reg_Uav_Ctl;
    Reg_Bl_Slot_Idx                  reg_Bl_Slot_Idx;
    Reg_Wls_Config                   reg_Wls_Config;
} Reg_Uav_Group_Group;

typedef struct _Wls_Fe_regs
{
    Reg_Ffc_Ubuf_Golbalconfig        reg_Ffc_Ubuf_Golbalconfig;
    Reg_Ffc_Ubuf_3dconfig            reg_Ffc_Ubuf_3dconfig;
    Reg_Descriptor_Heap_Base         reg_Descriptor_Heap_Base[8];
    Reg_Wls_Reserved0                reg_Wls_Reserved0;
    Reg_Wls_Reserved1                reg_Wls_Reserved1;
    Reg_Wls_Reserved2                reg_Wls_Reserved2;
    Reg_Wls_Reserved3                reg_Wls_Reserved3;
    Reg_Wls_Reserved4                reg_Wls_Reserved4;
    Reg_Wls_Reserved5                reg_Wls_Reserved5;
    Reg_Ffc_Ubuf_Csconfig            reg_Ffc_Ubuf_Csconfig;
    Reg_Wls_Reserved6                reg_Wls_Reserved6[7];
    Reg_Uav_Group_Group              reg_Uav_Group[128];
} Wls_Fe_regs;

#endif
