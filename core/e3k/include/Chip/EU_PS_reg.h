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
//    Spec Last Modified Time: 2018/11/26 19:05:36
#ifndef _EU_PS_REGISTER_H
#define _EU_PS_REGISTER_H


#ifndef        EU_PS_BLOCKBASE_INF
    #define    EU_PS_BLOCKBASE_INF
    #define    BLOCK_EU_PS_VERSION 1
    #define    BLOCK_EU_PS_TIMESTAMP "2018/11/26 19:05:36"
    #define    EU_PS_BLOCK                                                0x9 // match with BlockID.h
    #define    EU_PS_REG_START                                            0x0 // match with BlockID.h
    #define    EU_PS_REG_END                                              0x900 // match with BlockID.h
    #define    EU_PS_REG_LIMIT                                            0x900 // match with BlockID.h
#endif

// Register offset definition
#define        Reg_Ps_Cfg_Offset                                          0x0
#define        Reg_Ps_Ctrl_Offset                                         0x1
#define        Reg_Ps_Out_Cfg_Offset                                      0x2
#define        Reg_Ps_Doutbuf_Cfg_Offset                                  0x3
#define        Reg_Ps_Pm_Cfg_Offset                                       0x4
#define        Reg_Ps_Pm_Id_Offset                                        0x5
#define        Reg_Ps_Pm_Range_Offset                                     0x6
#define        Reg_Ps_Out_Mapping_Offset                                  0x7
#define        Reg_Ps_Out_Rtfmt_Offset                                    0x8
#define        Reg_Ps_Out_Fmt_Offset                                      0x8
#define        Reg_Ps_U_Enable_Offset                                     0x9
#define        Reg_Ps_U_Fmt_Offset                                        0xB
#define        Reg_Ps_U_Layout_Offset                                     0x13
#define        Reg_Ps_U_Cfg_Offset                                        0x15
#define        Reg_Ps_Instr0_Offset                                       0x16
#define        Reg_Ps_Instr1_Offset                                       0x17
#define        Reg_Ps_Instr_Range_Offset                                  0x18
#define        Reg_Ps_Cb_Cfg_Offset                                       0x19
#define        Reg_Ps_Rev_8aligned_Offset                                 0x1A
#define        Reg_Ps_Rev_Cb_Offset                                       0x20
#define        Reg_Ps_Cb_Data_Offset                                      0x100

// Block constant definition
typedef enum
{
    PS_CFG_PS_ON_OFF                         = 0,   // 0: Pixel Shader Off
    PS_CFG_PS_ON_ON                          = 1,   // 1: Pixel Shader On
} PS_CFG_PS_ON;
typedef enum
{
    PS_CTRL_AND_V_MASK_DISABLED              = 0,   // 0: Disable
    PS_CTRL_AND_V_MASK_ENABLED               = 1,   // 1: Enable, HW ands the sample predicate with the P7 quad
                                                    // based mask
} PS_CTRL_AND_V_MASK;
typedef enum
{
    PS_CTRL_RD_MODE_NEAR                     = 0,   // 00: Round toward nearest even
    PS_CTRL_RD_MODE_ZERO                     = 1,   // 01: Round to Zero
    PS_CTRL_RD_MODE_POS                      = 2,   // 10: Round to Positive Infinity
    PS_CTRL_RD_MODE_NEG                      = 3,   // 11: Round to Negative infinity
} PS_CTRL_RD_MODE;
typedef enum
{
    PS_CTRL_FP_MODE_STD_IEEE                 = 0,   // 00: Standard IEEE-745
    PS_CTRL_FP_MODE_NSTD_IEEE                = 1,   // 01: Non-Standard IEEE-745
    PS_CTRL_FP_MODE_MS_ALT                   = 2,   // 10: Microsoft Alt float point
    PS_CTRL_FP_MODE_RESERVED                 = 3,   // 11: Reserved
} PS_CTRL_FP_MODE;
typedef enum
{
    PS_CTRL_DENORM_EN_DISABLED               = 0,   // 0: Disable, denormal number will be flush to 0 for floating
    PS_CTRL_DENORM_EN_ENABLED                = 1,   // 1: Enable, denormal number is kept for floating
} PS_CTRL_DENORM_EN;
typedef enum
{
    PS_CTRL_THREAD_MODE_SIMD32               = 0,   // 0: only use FP/DP instr
    PS_CTRL_THREAD_MODE_SIMD64               = 1,   // 1: use HP/FP/DP/Mix instr
} PS_CTRL_THREAD_MODE;
typedef enum
{
    PS_CTRL_CUST_BLEND_EN_DISABLED           = 0,   // 0: Custom blending feature is disabled. The BLD bit check
                                                    // is useless
    PS_CTRL_CUST_BLEND_EN_ENABLED            = 1,   // 1: Custom blending feature is enabled. THC need check each
                                                    // thread blending flag.
} PS_CTRL_CUST_BLEND_EN;
typedef enum
{
    PS_CTRL_ROV_EN_DISABLED                  = 0,   // 0: ROV is disabled. The ROV bit check is useless
    PS_CTRL_ROV_EN_ENABLED                   = 1,   // 1: ROV is enabled. THC need check each thread rov flag.
} PS_CTRL_ROV_EN;
typedef enum
{
    PS_OUT_CFG_CLR_OUT_EN_OFF                = 0,   // 0: PS doesn't emit color. maybe zl3 or uav
    PS_OUT_CFG_CLR_OUT_EN_ON                 = 1,   // 1: PS emits color, normal cases
} PS_OUT_CFG_CLR_OUT_EN;
typedef enum
{
    PS_OUT_CFG_Z_OUT_EN_OFF                  = 0,   // 0: PS doesn't emit Z value.
    PS_OUT_CFG_Z_OUT_EN_ON                   = 1,   // 1: PS emits Z value.
} PS_OUT_CFG_Z_OUT_EN;
typedef enum
{
    PS_OUT_CFG_ALPHA_OUT_EN_OFF              = 0,   // 0: if PS emits Alpha value, DOUT will discard it
    PS_OUT_CFG_ALPHA_OUT_EN_ON               = 1,   // 1: PS emits Alpha value.
} PS_OUT_CFG_ALPHA_OUT_EN;
typedef enum
{
    PS_OUT_CFG_STENCIL_OUT_EN_OFF            = 0,   // 0: PS doesn't emit Stencil value.
    PS_OUT_CFG_STENCIL_OUT_EN_ON             = 1,   // 1: PS emits Stencil value.
} PS_OUT_CFG_STENCIL_OUT_EN;
typedef enum
{
    PS_OUT_CFG_MASK_OUT_EN_OFF               = 0,   // 0: PS doesn't emit Mask value.
    PS_OUT_CFG_MASK_OUT_EN_ON                = 1,   // 1: PS emits Mask value.
} PS_OUT_CFG_MASK_OUT_EN;
typedef enum
{
    PS_OUT_CFG_DUAL_SRC_EN_OFF               = 0,   // 0: dual source blending is off in the WBU
    PS_OUT_CFG_DUAL_SRC_EN_ON                = 1,   // 1: dual source blending is on in the WBU
} PS_OUT_CFG_DUAL_SRC_EN;
typedef enum
{
    PS_OUT_CFG_Z_DATA_FMT_FP32               = 0,   // 000: Floating-point 32-bits, FP32.
    PS_OUT_CFG_Z_DATA_FMT_UNORM24            = 6,   // 110: Unorm 24-bit
    PS_OUT_CFG_Z_DATA_FMT_UNORM16            = 7,   // 111: Unorm 16-bit
} PS_OUT_CFG_Z_DATA_FMT;
typedef enum
{
    PS_OUT_CFG_DUAL_SRC_FMT_FP16             = 1,   // 001: Floating-point 16-bit, 4P4C, one quad per entry
    PS_OUT_CFG_DUAL_SRC_FMT_UNORM10          = 8,   // 1000: Unorm 10-bit, 4P4C, one quad per entry
    PS_OUT_CFG_DUAL_SRC_FMT_UNORM8           = 9,   // 1001: Unorm 8-bit, 8P4C, two quads per entry
    PS_OUT_CFG_DUAL_SRC_FMT_SNORM8           = 12,  // 1100: Snorm 8-bit, two quads per entry
} PS_OUT_CFG_DUAL_SRC_FMT;
typedef enum
{
    PS_OUT_CFG_MASK_OUT_MODE_BIT1            = 0,   // 1bit per lane. Maybe kill pixel or sample.
    PS_OUT_CFG_MASK_OUT_MODE_BIT2            = 1,   // 2bits per lane. Maybe shader need output oMask in MSAA-2X.
                                                    // one entry for one PS thread
    PS_OUT_CFG_MASK_OUT_MODE_BIT4            = 2,   // 4bits per lane. Maybe shader need output oMask in MSAA-4X.
                                                    // one entry for one PS thread
    PS_OUT_CFG_MASK_OUT_MODE_BIT8            = 3,   // 8bits per lane. Maybe shader need output oMask in MSAA-8X.
                                                    // 8x64 = 512bits in one entry or 256bits for simd32
    PS_OUT_CFG_MASK_OUT_MODE_BIT16           = 4,   // 16bits per lane. Maybe shader need output oMask in
                                                    // MSAA-16X. 16x64 = 2x512bits in two entries; or one entry
                                                    // for simd32
} PS_OUT_CFG_MASK_OUT_MODE;
typedef enum
{
    PS_DOUTBUF_CFG_ZL3_SIZE_0LINES           = 0,   // 0lines for ZL3 disabled
    PS_DOUTBUF_CFG_ZL3_SIZE_16LINES          = 4,   // If any Z/Alpha/Stencil/Mask_Out is 1, DOUT need allocate
                                                    // ZL3 buffer based on it.
    PS_DOUTBUF_CFG_ZL3_SIZE_32LINES          = 5,
    PS_DOUTBUF_CFG_ZL3_SIZE_64LINES          = 6,
    PS_DOUTBUF_CFG_ZL3_SIZE_128LINES         = 7,
} PS_DOUTBUF_CFG_ZL3_SIZE;
typedef enum
{
    PS_DOUTBUF_CFG_CLR_SIZE_0LINES           = 0,   // 0lines for Color disabled, Clr_Out_En should be 0.
    PS_DOUTBUF_CFG_CLR_SIZE_32LINES          = 5,   // If Clr_Out_En is 1, DOUT need allocate color buffer based
                                                    // on it.
    PS_DOUTBUF_CFG_CLR_SIZE_64LINES          = 6,
    PS_DOUTBUF_CFG_CLR_SIZE_128LINES         = 7,
} PS_DOUTBUF_CFG_CLR_SIZE;
typedef enum
{
    PS_PM_CFG_PM_EN_DISABLED                 = 0,   // 0: Disable. Should no PM instr in PS.
    PS_PM_CFG_PM_EN_ENABLED                  = 1,   // 1: Enable, PS use PM.
} PS_PM_CFG_PM_EN;
typedef enum
{
    PS_PM_CFG_TH_PM_SIZE_8KDW                = 0,   // 8K dword, can hold 256# for Simd32, or 128# for Simd64
                                                    // threads
    PS_PM_CFG_TH_PM_SIZE_16KDW               = 1,   // 512# for Simd32; 256# for Simd64
    PS_PM_CFG_TH_PM_SIZE_32KDW               = 2,   // 1024# for Simd32; 512# for Simd64
    PS_PM_CFG_TH_PM_SIZE_64KDW               = 3,   // 2048# for Simd32; 1024# for Simd64
    PS_PM_CFG_TH_PM_SIZE_128KDW              = 4,   // 4096# for Simd32; 2048# for Simd64
    PS_PM_CFG_TH_PM_SIZE_256KDW              = 5,   // 8192# for Simd32; 4096# for Simd64
    PS_PM_CFG_TH_PM_SIZE_512KDW              = 6,   // 16384# for simd32 thread; 8192 for simd64 thread.
    PS_PM_CFG_TH_PM_SIZE_1024KDW             = 7,   // 32k# for simd32 thread; 16k# form simd64 thread.
    PS_PM_CFG_TH_PM_SIZE_1040KDW             = 8,   // 16640# for simd64 thread
} PS_PM_CFG_TH_PM_SIZE;
typedef enum
{
    PS_OUT_MAPPING_O0_EN_DISABLED            = 0,   // 0: Disabled
    PS_OUT_MAPPING_O0_EN_ENABLED             = 1,   // 1: Enabled
} PS_OUT_MAPPING_O0_EN;
typedef enum
{
    PS_OUT_MAPPING_O1_EN_DISABLED            = 0,   // 0: Disabled
    PS_OUT_MAPPING_O1_EN_ENABLED             = 1,   // 1: Enabled
} PS_OUT_MAPPING_O1_EN;
typedef enum
{
    PS_OUT_MAPPING_O2_EN_DISABLED            = 0,   // 0: Disabled
    PS_OUT_MAPPING_O2_EN_ENABLED             = 1,   // 1: Enabled
} PS_OUT_MAPPING_O2_EN;
typedef enum
{
    PS_OUT_MAPPING_O3_EN_DISABLED            = 0,   // 0: Disabled
    PS_OUT_MAPPING_O3_EN_ENABLED             = 1,   // 1: Enabled
} PS_OUT_MAPPING_O3_EN;
typedef enum
{
    PS_OUT_MAPPING_O4_EN_DISABLED            = 0,   // 0: Disabled
    PS_OUT_MAPPING_O4_EN_ENABLED             = 1,   // 1: Enabled
} PS_OUT_MAPPING_O4_EN;
typedef enum
{
    PS_OUT_MAPPING_O5_EN_DISABLED            = 0,   // 0: Disabled
    PS_OUT_MAPPING_O5_EN_ENABLED             = 1,   // 1: Enabled
} PS_OUT_MAPPING_O5_EN;
typedef enum
{
    PS_OUT_MAPPING_O6_EN_DISABLED            = 0,   // 0: Disabled
    PS_OUT_MAPPING_O6_EN_ENABLED             = 1,   // 1: Enabled
} PS_OUT_MAPPING_O6_EN;
typedef enum
{
    PS_OUT_MAPPING_O7_EN_DISABLED            = 0,   // 0: Disabled
    PS_OUT_MAPPING_O7_EN_ENABLED             = 1,   // 1: Enabled
} PS_OUT_MAPPING_O7_EN;
typedef enum
{
    PS_OUT_FMT_O0_DATA_FMT_FP32 = 0,   // 000: Floating-point 32-bits
    PS_OUT_FMT_O0_DATA_FMT_FP16 = 1,   // 001: Floating-point 16-bit
    PS_OUT_FMT_O0_DATA_FMT_SINT32 = 2,   // 010: Signed Integer 32-bit
    PS_OUT_FMT_O0_DATA_FMT_SINT16 = 3,   // 011: Signed Integer 16-bit
    PS_OUT_FMT_O0_DATA_FMT_UINT32 = 4,   // 100: Unsigned integer 32-bit
    PS_OUT_FMT_O0_DATA_FMT_UINT16 = 5,   // 101: Unsigned integer 16-bit
    PS_OUT_FMT_O0_DATA_FMT_UNORM24 = 6,   // 110: Unorm 24-bit
    PS_OUT_FMT_O0_DATA_FMT_UNORM16 = 7,   // 111: Unorm 16-bit
    PS_OUT_FMT_O0_DATA_FMT_UNORM10 = 8,   // 1000: Unorm 10-bit
    PS_OUT_FMT_O0_DATA_FMT_UNORM8 = 9,   // 1001: Unorm 8-bit
    PS_OUT_FMT_O0_DATA_FMT_SINT8 = 10,  // 1010: Signed Integer 8-bit
    PS_OUT_FMT_O0_DATA_FMT_UINT8 = 11,  // 1011: Unsigned integer 8-bit
    PS_OUT_FMT_O0_DATA_FMT_SNORM8 = 12,  // 1100: Snorm 8-bit
    PS_OUT_FMT_O0_DATA_FMT_SNORM16 = 13,  // 1100: Snorm 16-bit
    PS_OUT_FMT_O0_DATA_FMT_UINT64 = 14,  // 1101: unsigned int-64bits, only for reduction
    PS_OUT_FMT_O0_DATA_FMT_RESERVED = 15,  // 1111: Reserved
} PS_OUT_FMT_O0_DATA_FMT;
typedef enum
{
    PS_OUT_RTFMT_RT0_DATA_FMT_FP32           = 0, // 000: Floating-point 32-bits
    PS_OUT_RTFMT_RT0_DATA_FMT_FP16           = 1, // 001: Floating-point 16-bit
    PS_OUT_RTFMT_RT0_DATA_FMT_SINT32         = 2, // 010: Signed Integer 32-bit
    PS_OUT_RTFMT_RT0_DATA_FMT_SINT16         = 3, // 011: Signed Integer 16-bit
    PS_OUT_RTFMT_RT0_DATA_FMT_UINT32         = 4, // 100: Unsigned integer 32-bit
    PS_OUT_RTFMT_RT0_DATA_FMT_UINT16         = 5, // 101: Unsigned integer 16-bit
    PS_OUT_RTFMT_RT0_DATA_FMT_UNORM24        = 6, // 110: Unorm 24-bit
    PS_OUT_RTFMT_RT0_DATA_FMT_UNORM16        = 7, // 111: Unorm 16-bit
    PS_OUT_RTFMT_RT0_DATA_FMT_UNORM10        = 8, // 1000: Unorm 10-bit
    PS_OUT_RTFMT_RT0_DATA_FMT_UNORM8         = 9, // 1001: Unorm 8-bit
    PS_OUT_RTFMT_RT0_DATA_FMT_SINT8          = 10, // 1010: Signed Integer 8-bit
    PS_OUT_RTFMT_RT0_DATA_FMT_UINT8          = 11, // 1011: Unsigned integer 8-bit
    PS_OUT_RTFMT_RT0_DATA_FMT_SNORM8         = 12, // 1100: Snorm 8-bit
    PS_OUT_RTFMT_RT0_DATA_FMT_SNORM16        = 13, // 1100: Snorm 16-bit
    PS_OUT_RTFMT_RT0_DATA_FMT_UINT64         = 14, // 1101: unsigned int-64bits, only for reduction
    PS_OUT_RTFMT_RT0_DATA_FMT_RESERVED       = 15, // 1111: Reserved
} PS_OUT_RTFMT_RT0_DATA_FMT;

typedef enum
{
    PS_U_ENABLE_U0_EN_DISABLE                = 0,   // 0: U# disable, discard store and return 0 values for load
    PS_U_ENABLE_U0_EN_ENABLE                 = 1,   // 1: U# enable, the following U# fmt and layout are valid
} PS_U_ENABLE_U0_EN;
typedef enum
{
    PS_U_FMT_U0_DATA_FMT_FP32                = 0,   // 000: Floating-point 32-bits
    PS_U_FMT_U0_DATA_FMT_FP16                = 1,   // 001: Floating-point 16-bit
    PS_U_FMT_U0_DATA_FMT_SINT32              = 2,   // 010: Signed Integer 32-bit
    PS_U_FMT_U0_DATA_FMT_SINT16              = 3,   // 011: Signed Integer 16-bit
    PS_U_FMT_U0_DATA_FMT_UINT32              = 4,   // 100: Unsigned integer 32-bit
    PS_U_FMT_U0_DATA_FMT_UINT16              = 5,   // 101: Unsigned integer 16-bit
    PS_U_FMT_U0_DATA_FMT_UNORM24             = 6,   // 110: Unorm 24-bit
    PS_U_FMT_U0_DATA_FMT_UNORM16             = 7,   // 111: Unorm 16-bit
    PS_U_FMT_U0_DATA_FMT_UNORM10             = 8,   // 1000: Unorm 10-bit
    PS_U_FMT_U0_DATA_FMT_UNORM8              = 9,   // 1001: Unorm 8-bit
    PS_U_FMT_U0_DATA_FMT_SINT8               = 10,  // 1010: Signed Integer 8-bit
    PS_U_FMT_U0_DATA_FMT_UINT8               = 11,  // 1011: Unsigned integer 8-bit
    PS_U_FMT_U0_DATA_FMT_SNORM8              = 12,  // 1100: Snorm 8-bit
    PS_U_FMT_U0_DATA_FMT_SNORM16             = 13,  // 1100: Snorm 16-bit
    PS_U_FMT_U0_DATA_FMT_UINT64              = 14,  // 1101: unsigned int-64bits, only for reduction
    PS_U_FMT_U0_DATA_FMT_RESERVED            = 15,  // 1111: Reserved
} PS_U_FMT_U0_DATA_FMT;
typedef enum
{
    PS_U_LAYOUT_U0_LAYOUT_VERTICAL           = 0,   // 0: Untyped vertical buffer, should be 32bits
    PS_U_LAYOUT_U0_LAYOUT_HORIZONTAL         = 1,   // 1: Untyped/Typed horizontal buffer.
} PS_U_LAYOUT_U0_LAYOUT;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////            EU_PS Block (Block ID = 9) Register Definitions                           ///////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define Reg_PS_CFG_Ps_On_BitField_MaxValue ((1u << 1) - 1)
#define Reg_PS_CFG_Max_Threads_BitField_MaxValue ((1u << 8) - 1)
#define Reg_PS_CFG_In_Size_BitField_MaxValue ((1u << 6) - 1)
#define Reg_PS_CFG_Reserved1_BitField_MaxValue ((1u << 17) - 1)

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Ps_On                     : 1;   // Pixel Shader On
        unsigned int Max_Threads               : 8;   // Maximum number of total PS threads kickoff in one slice.
                                                      // Value 0~64 for 0~64 threads in current slices. 0 is illegal
                                                      // if PS is ON.
        unsigned int In_Size                   : 6;   // PS Varying Attribute Input Size. Value is 1~32
                                                      // 4-component elements.  Zero In_Size is illegal.
        unsigned int Reserved1                 : 17;  // Reserved
    } reg;
} Reg_Ps_Cfg;

#define Reg_PS_CTRL_Crf_Size_BitField_MaxValue ((1u << 6) - 1)
#define Reg_PS_CTRL_And_V_Mask_BitField_MaxValue ((1u << 1) - 1)
#define Reg_PS_CTRL_Rd_Mode_BitField_MaxValue ((1u << 2) - 1)
#define Reg_PS_CTRL_Fp_Mode_BitField_MaxValue ((1u << 2) - 1)
#define Reg_PS_CTRL_Denorm_En_BitField_MaxValue ((1u << 1) - 1)
#define Reg_PS_CTRL_Thread_Mode_BitField_MaxValue ((1u << 1) - 1)
#define Reg_PS_CTRL_Cust_Blend_En_BitField_MaxValue ((1u << 1) - 1)
#define Reg_PS_CTRL_Rov_En_BitField_MaxValue ((1u << 1) - 1)
#define Reg_PS_CTRL_Cust_Crf_Addr_BitField_MaxValue ((1u << 6) - 1)
#define Reg_PS_CTRL_Reserved_BitField_MaxValue ((1u << 11) - 1)

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Crf_Size                  : 6;   // Total FP Common Register File (FP-CRF) space. Value is
                                                      // 1~52 4-component elements, including temp, input,
                                                      // output registers.
        unsigned int And_V_Mask                : 1;   // And sample predicate with P7 quad based mask in PS
        unsigned int Rd_Mode                   : 2;   // EU floating point rounding mode
        unsigned int Fp_Mode                   : 2;   // EU floating point operation mode
        unsigned int Denorm_En                 : 1;   // Whether support denomal computation
        unsigned int Thread_Mode               : 1;   // PS initial thread mode, this affects how the PSTC allocate
                                                      // thread IDs into the CRF and thread size
        unsigned int Cust_Blend_En             : 1;   // Custom Blending enable. WBU notifies the PS thread that
                                                      // order dependency is resolved.
        unsigned int Rov_En                    : 1;   // DX12. PS ROV enable (Raster Ordered Views). WBU notifies
                                                      // the PS thread that order dependency is resolved.
        unsigned int Cust_Crf_Addr             : 6;   // The start CRF address that records the color from WBU side,
                                                      // and is valid when Cust_Blend_En is 1. The CRF address is FP
                                                      // mode.
        unsigned int Reserved                  : 11;  // Reserved
    } reg;
} Reg_Ps_Ctrl;

#define Reg_PS_OUT_CFG_Clr_Out_En_BitField_MaxValue ((1u << 1) - 1)
#define Reg_PS_OUT_CFG_Z_Out_En_BitField_MaxValue ((1u << 1) - 1)
#define Reg_PS_OUT_CFG_Alpha_Out_En_BitField_MaxValue ((1u << 1) - 1)
#define Reg_PS_OUT_CFG_Stencil_Out_En_BitField_MaxValue ((1u << 1) - 1)
#define Reg_PS_OUT_CFG_Mask_Out_En_BitField_MaxValue ((1u << 1) - 1)
#define Reg_PS_OUT_CFG_Dual_Src_En_BitField_MaxValue ((1u << 1) - 1)
#define Reg_PS_OUT_CFG_Clr_Out_Num_BitField_MaxValue ((1u << 4) - 1)
#define Reg_PS_OUT_CFG_Z_Data_Fmt_BitField_MaxValue ((1u << 4) - 1)
#define Reg_PS_OUT_CFG_Dual_Src_Fmt_BitField_MaxValue ((1u << 4) - 1)
#define Reg_PS_OUT_CFG_Mask_Out_Mode_BitField_MaxValue ((1u << 3) - 1)
#define Reg_PS_OUT_CFG_Reserved_BitField_MaxValue ((1u << 11) - 1)

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Clr_Out_En                : 1;   // Color output to WBU enable.
        unsigned int Z_Out_En                  : 1;   // oDepth output to ZL3 enable. DOUT need convert EUDP data
                                                      // based on Z format.
        unsigned int Alpha_Out_En              : 1;   // Alpha output to ZL3 enable.  DOUT need convert EUDP data to
                                                      // UNORM8
        unsigned int Stencil_Out_En            : 1;   // Stencil output to ZL3 enable. DOUT need truncate EUDP data
                                                      // into lsb-8bits
        unsigned int Mask_Out_En               : 1;   // Pixel Mask output to ZL3 enable. DOUT need trucate EUDP
                                                      // data into lsb-1/2/4/8/16 bits.
        unsigned int Dual_Src_En               : 1;   // Dual source blending enable. only support 1RT-dual src,
                                                      // maybe unorm8/snorm8/unorm10/fp16 format. For other
                                                      // format dual src blending, compiler need patch it by EU
                                                      // blending.
        unsigned int Clr_Out_Num               : 4;   // Color output (to WBU) number, multiple RT caes, and may be
                                                      // 1~8 RTs, whose format may be different.
        unsigned int Z_Data_Fmt                : 4;   // EU to WBU PS output format for Z
        unsigned int Dual_Src_Fmt              : 4;   // EU to WBU dual source format, always 8P4C, two quads per
                                                      // entry
        unsigned int Mask_Out_Mode             : 3;   // Emit Mask mode, based on MSAA Multi-sampling
                                                      // AntiAliasing mode
        unsigned int Reserved                  : 11;  // Reserved
    } reg;
} Reg_Ps_Out_Cfg;

#define Reg_PS_DOUTBUF_CFG_Zl3_Size_BitField_MaxValue ((1u << 4) - 1)
#define Reg_PS_DOUTBUF_CFG_Clr_Size_BitField_MaxValue ((1u << 4) - 1)
#define Reg_PS_DOUTBUF_CFG_Reserved_BitField_MaxValue ((1u << 24) - 1)

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Zl3_Size                  : 4;   // ZL3 bit-width buffer size for PS EMIT, including Mask,
                                                      // Alpha, Z and Stencil
        unsigned int Clr_Size                  : 4;   // Color bit-width buffer size for PS EMITCLR, maybe 0 ~ 8 RTs.
        unsigned int Reserved                  : 24;  // Reserved
    } reg;
} Reg_Ps_Doutbuf_Cfg;

#define Reg_PS_PM_CFG_Pm_En_BitField_MaxValue ((1u << 1) - 1)
#define Reg_PS_PM_CFG_Th_Pm_Size_BitField_MaxValue ((1u << 4) - 1)
#define Reg_PS_PM_CFG_Reserved_BitField_MaxValue ((1u << 27) - 1)

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Pm_En                     : 1;   // Wheter PS uses PM.
        unsigned int Th_Pm_Size                : 4;   // Size of thread external storage in memory/L2 including
                                                      // call stacks, CRF spill, x# buffers, etc. IF FS and PS use the
                                                      // same UAV buffer, it should use the same PM_Size.
        unsigned int Reserved                  : 27;  // Reserved
    } reg;
} Reg_Ps_Pm_Cfg;

#define Reg_PS_PM_ID_Pm_U_Id_BitField_MaxValue ((1u << 32) - 1)

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Pm_U_Id                   : 32;  // PS private memory U_sharp id, to load/store private
                                                      // memory.  LS unit will convert PM into Untyped LD/ST. In reg
                                                      // mode, it should < 64, or else regard it as out-of-range; in
                                                      // cache mode, it is the offset in cache.
    } reg;
} Reg_Ps_Pm_Id;

#define Reg_PS_PM_RANGE_Ps_Idx_Ele_Range_BitField_MaxValue ((1u << 14) - 1)
#define Reg_PS_PM_RANGE_Reserved_BitField_MaxValue ((1u << 18) - 1)

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Ps_Idx_Ele_Range          : 14;  // Range of PS thread external storage used by private
                                                      // memory, or CRF spill. Element based
        unsigned int Reserved                  : 18;  // Reserved
    } reg;
} Reg_Ps_Pm_Range;

#define Reg_PS_OUT_MAPPING_O0_En_BitField_MaxValue ((1u << 1) - 1)
#define Reg_PS_OUT_MAPPING_O0_Addr_BitField_MaxValue ((1u << 3) - 1)
#define Reg_PS_OUT_MAPPING_O1_En_BitField_MaxValue ((1u << 1) - 1)
#define Reg_PS_OUT_MAPPING_O1_Addr_BitField_MaxValue ((1u << 3) - 1)
#define Reg_PS_OUT_MAPPING_O2_En_BitField_MaxValue ((1u << 1) - 1)
#define Reg_PS_OUT_MAPPING_O2_Addr_BitField_MaxValue ((1u << 3) - 1)
#define Reg_PS_OUT_MAPPING_O3_En_BitField_MaxValue ((1u << 1) - 1)
#define Reg_PS_OUT_MAPPING_O3_Addr_BitField_MaxValue ((1u << 3) - 1)
#define Reg_PS_OUT_MAPPING_O4_En_BitField_MaxValue ((1u << 1) - 1)
#define Reg_PS_OUT_MAPPING_O4_Addr_BitField_MaxValue ((1u << 3) - 1)
#define Reg_PS_OUT_MAPPING_O5_En_BitField_MaxValue ((1u << 1) - 1)
#define Reg_PS_OUT_MAPPING_O5_Addr_BitField_MaxValue ((1u << 3) - 1)
#define Reg_PS_OUT_MAPPING_O6_En_BitField_MaxValue ((1u << 1) - 1)
#define Reg_PS_OUT_MAPPING_O6_Addr_BitField_MaxValue ((1u << 3) - 1)
#define Reg_PS_OUT_MAPPING_O7_En_BitField_MaxValue ((1u << 1) - 1)
#define Reg_PS_OUT_MAPPING_O7_Addr_BitField_MaxValue ((1u << 3) - 1)

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int O0_En                     : 1;   // RT0 OSharp is enabled/disabled.It set true if shader
                                                     // outputs O0 color. DOUT need output color when (O#_En &
                                                     // RT#_En)
        unsigned int O0_Addr                   : 3;  // EMIT O0 is mapped into address of DOUT
        unsigned int O1_En                     : 1;  // EMIT O1 is enabled/disabled.
        unsigned int O1_Addr                   : 3;  // EMIT O1 is mapped into address of DOUT
        unsigned int O2_En                     : 1;  // EMIT O2 is enabled/disabled.
        unsigned int O2_Addr                   : 3;  // EMIT O2 is mapped into address of DOUT
        unsigned int O3_En                     : 1;  // EMIT O3 is enabled/disabled.
        unsigned int O3_Addr                   : 3;  // EMIT O3 is mapped into address of DOUT
        unsigned int O4_En                     : 1;  // RT4 O4 is enabled/disabled.
        unsigned int O4_Addr                   : 3;  // EMIT O4 is mapped into address of DOUT
        unsigned int O5_En                     : 1;  // EMIT O5 is enabled/disabled.
        unsigned int O5_Addr                   : 3;  // EMIT O5 is mapped into address of DOUT
        unsigned int O6_En                     : 1;  // EMIT O6 is enabled/disabled.
        unsigned int O6_Addr                   : 3;  // EMIT O6 is mapped into address of DOUT
        unsigned int O7_En                     : 1;  // EMIT O7 is enabled/disabled.
        unsigned int O7_Addr                   : 3;  // EMIT O7 is mapped into address of DOUT
    } reg;
} Reg_Ps_Out_Mapping;

#define Reg_PS_OUT_RTFMT_Rt0_Data_Fmt_BitField_MaxValue ((1u << 4) - 1)
#define Reg_PS_OUT_RTFMT_Rt1_Data_Fmt_BitField_MaxValue ((1u << 4) - 1)
#define Reg_PS_OUT_RTFMT_Rt2_Data_Fmt_BitField_MaxValue ((1u << 4) - 1)
#define Reg_PS_OUT_RTFMT_Rt3_Data_Fmt_BitField_MaxValue ((1u << 4) - 1)
#define Reg_PS_OUT_RTFMT_Rt4_Data_Fmt_BitField_MaxValue ((1u << 4) - 1)
#define Reg_PS_OUT_RTFMT_Rt5_Data_Fmt_BitField_MaxValue ((1u << 4) - 1)
#define Reg_PS_OUT_RTFMT_Rt6_Data_Fmt_BitField_MaxValue ((1u << 4) - 1)
#define Reg_PS_OUT_RTFMT_Rt7_Data_Fmt_BitField_MaxValue ((1u << 4) - 1)

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int O0_Data_Fmt : 4;   // EU to WBU PS output format for O0
        unsigned int O1_Data_Fmt : 4;   // EU to WBU PS output format for O1
        unsigned int O2_Data_Fmt : 4;   // EU to WBU PS output format for O2
        unsigned int O3_Data_Fmt : 4;   // EU to WBU PS output format for O3
        unsigned int O4_Data_Fmt : 4;   // EU to WBU PS output format for O4
        unsigned int O5_Data_Fmt : 4;   // EU to WBU PS output format for O5
        unsigned int O6_Data_Fmt : 4;   // EU to WBU PS output format for O6
        unsigned int O7_Data_Fmt : 4;   // EU to WBU PS output format for O7
    } reg;
} Reg_Ps_Out_Fmt;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Rt0_Data_Fmt              : 4;  // RT0 color format
        unsigned int Rt1_Data_Fmt              : 4;  // RT1 color format
        unsigned int Rt2_Data_Fmt              : 4;  // RT2 color format
        unsigned int Rt3_Data_Fmt              : 4;  // RT3 color format
        unsigned int Rt4_Data_Fmt              : 4;  // RT4 color format
        unsigned int Rt5_Data_Fmt              : 4;  // RT5 color format
        unsigned int Rt6_Data_Fmt              : 4;  // RT6 color format
        unsigned int Rt7_Data_Fmt              : 4;  // RT7 color format
    } reg;
} Reg_Ps_Out_Rtfmt;

#define Reg_PS_U_ENABLE_U0_En_BitField_MaxValue ((1u << 1) - 1)
#define Reg_PS_U_ENABLE_U1_En_BitField_MaxValue ((1u << 1) - 1)
#define Reg_PS_U_ENABLE_U2_En_BitField_MaxValue ((1u << 1) - 1)
#define Reg_PS_U_ENABLE_U3_En_BitField_MaxValue ((1u << 1) - 1)
#define Reg_PS_U_ENABLE_U4_En_BitField_MaxValue ((1u << 1) - 1)
#define Reg_PS_U_ENABLE_U5_En_BitField_MaxValue ((1u << 1) - 1)
#define Reg_PS_U_ENABLE_U6_En_BitField_MaxValue ((1u << 1) - 1)
#define Reg_PS_U_ENABLE_U7_En_BitField_MaxValue ((1u << 1) - 1)
#define Reg_PS_U_ENABLE_U8_En_BitField_MaxValue ((1u << 1) - 1)
#define Reg_PS_U_ENABLE_U9_En_BitField_MaxValue ((1u << 1) - 1)
#define Reg_PS_U_ENABLE_U10_En_BitField_MaxValue ((1u << 1) - 1)
#define Reg_PS_U_ENABLE_U11_En_BitField_MaxValue ((1u << 1) - 1)
#define Reg_PS_U_ENABLE_U12_En_BitField_MaxValue ((1u << 1) - 1)
#define Reg_PS_U_ENABLE_U13_En_BitField_MaxValue ((1u << 1) - 1)
#define Reg_PS_U_ENABLE_U14_En_BitField_MaxValue ((1u << 1) - 1)
#define Reg_PS_U_ENABLE_U15_En_BitField_MaxValue ((1u << 1) - 1)
#define Reg_PS_U_ENABLE_U16_En_BitField_MaxValue ((1u << 1) - 1)
#define Reg_PS_U_ENABLE_U17_En_BitField_MaxValue ((1u << 1) - 1)
#define Reg_PS_U_ENABLE_U18_En_BitField_MaxValue ((1u << 1) - 1)
#define Reg_PS_U_ENABLE_U19_En_BitField_MaxValue ((1u << 1) - 1)
#define Reg_PS_U_ENABLE_U20_En_BitField_MaxValue ((1u << 1) - 1)
#define Reg_PS_U_ENABLE_U21_En_BitField_MaxValue ((1u << 1) - 1)
#define Reg_PS_U_ENABLE_U22_En_BitField_MaxValue ((1u << 1) - 1)
#define Reg_PS_U_ENABLE_U23_En_BitField_MaxValue ((1u << 1) - 1)
#define Reg_PS_U_ENABLE_U24_En_BitField_MaxValue ((1u << 1) - 1)
#define Reg_PS_U_ENABLE_U25_En_BitField_MaxValue ((1u << 1) - 1)
#define Reg_PS_U_ENABLE_U26_En_BitField_MaxValue ((1u << 1) - 1)
#define Reg_PS_U_ENABLE_U27_En_BitField_MaxValue ((1u << 1) - 1)
#define Reg_PS_U_ENABLE_U28_En_BitField_MaxValue ((1u << 1) - 1)
#define Reg_PS_U_ENABLE_U29_En_BitField_MaxValue ((1u << 1) - 1)
#define Reg_PS_U_ENABLE_U30_En_BitField_MaxValue ((1u << 1) - 1)
#define Reg_PS_U_ENABLE_U31_En_BitField_MaxValue ((1u << 1) - 1)


typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int U0_En                     : 1;   // U# enabled
        unsigned int U1_En                     : 1;   // U# enabled
        unsigned int U2_En                     : 1;   // U# enabled
        unsigned int U3_En                     : 1;   // U# enabled
        unsigned int U4_En                     : 1;   // U# enabled
        unsigned int U5_En                     : 1;   // U# enabled
        unsigned int U6_En                     : 1;   // U# enabled
        unsigned int U7_En                     : 1;   // U# enabled
        unsigned int U8_En                     : 1;   // U# enabled
        unsigned int U9_En                     : 1;   // U# enabled
        unsigned int U10_En                    : 1;   // U# enabled
        unsigned int U11_En                    : 1;   // U# enabled
        unsigned int U12_En                    : 1;   // U# enabled
        unsigned int U13_En                    : 1;   // U# enabled
        unsigned int U14_En                    : 1;   // U# enabled
        unsigned int U15_En                    : 1;   // U# enabled
        unsigned int U16_En                    : 1;   // U# enabled
        unsigned int U17_En                    : 1;   // U# enabled
        unsigned int U18_En                    : 1;   // U# enabled
        unsigned int U19_En                    : 1;   // U# enabled
        unsigned int U20_En                    : 1;   // U# enabled
        unsigned int U21_En                    : 1;   // U# enabled
        unsigned int U22_En                    : 1;   // U# enabled
        unsigned int U23_En                    : 1;   // U# enabled
        unsigned int U24_En                    : 1;   // U# enabled
        unsigned int U25_En                    : 1;   // U# enabled
        unsigned int U26_En                    : 1;   // U# enabled
        unsigned int U27_En                    : 1;   // U# enabled
        unsigned int U28_En                    : 1;   // U# enabled
        unsigned int U29_En                    : 1;   // U# enabled
        unsigned int U30_En                    : 1;   // U# enabled
        unsigned int U31_En                    : 1;   // U# enabled
    } reg;
} Reg_Ps_U_Enable;

#define Reg_PS_U_FMT_U0_Data_Fmt_BitField_MaxValue ((1u << 4) - 1)
#define Reg_PS_U_FMT_U1_Data_Fmt_BitField_MaxValue ((1u << 4) - 1)
#define Reg_PS_U_FMT_U2_Data_Fmt_BitField_MaxValue ((1u << 4) - 1)
#define Reg_PS_U_FMT_U3_Data_Fmt_BitField_MaxValue ((1u << 4) - 1)
#define Reg_PS_U_FMT_U4_Data_Fmt_BitField_MaxValue ((1u << 4) - 1)
#define Reg_PS_U_FMT_U5_Data_Fmt_BitField_MaxValue ((1u << 4) - 1)
#define Reg_PS_U_FMT_U6_Data_Fmt_BitField_MaxValue ((1u << 4) - 1)
#define Reg_PS_U_FMT_U7_Data_Fmt_BitField_MaxValue ((1u << 4) - 1)

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int U0_Data_Fmt               : 4;   // EU to WLS UAV format for U[8*id + 0]. LS need convert data
                                                      // format based on U# format.
        unsigned int U1_Data_Fmt               : 4;   // EU to WBU UAV format for U[8*id + 1]
        unsigned int U2_Data_Fmt               : 4;   // EU to WBU UAV format for U[8*id + 2]
        unsigned int U3_Data_Fmt               : 4;   // EU to WBU UAV format for U[8*id + 3]
        unsigned int U4_Data_Fmt               : 4;   // EU to WBU UAV format for U[8*id + 4]
        unsigned int U5_Data_Fmt               : 4;   // EU to WBU UAV format for U[8*id + 5]
        unsigned int U6_Data_Fmt               : 4;   // EU to WBU UAV format for U[8*id + 6]
        unsigned int U7_Data_Fmt               : 4;   // EU to WBU UAV format for U[8*id + 7]
    } reg;
} Reg_Ps_U_Fmt;

#define Reg_PS_U_LAYOUT_U0_Layout_BitField_MaxValue ((1u << 1) - 1)
#define Reg_PS_U_LAYOUT_U1_Layout_BitField_MaxValue ((1u << 1) - 1)
#define Reg_PS_U_LAYOUT_U2_Layout_BitField_MaxValue ((1u << 1) - 1)
#define Reg_PS_U_LAYOUT_U3_Layout_BitField_MaxValue ((1u << 1) - 1)
#define Reg_PS_U_LAYOUT_U4_Layout_BitField_MaxValue ((1u << 1) - 1)
#define Reg_PS_U_LAYOUT_U5_Layout_BitField_MaxValue ((1u << 1) - 1)
#define Reg_PS_U_LAYOUT_U6_Layout_BitField_MaxValue ((1u << 1) - 1)
#define Reg_PS_U_LAYOUT_U7_Layout_BitField_MaxValue ((1u << 1) - 1)
#define Reg_PS_U_LAYOUT_U8_Layout_BitField_MaxValue ((1u << 1) - 1)
#define Reg_PS_U_LAYOUT_U9_Layout_BitField_MaxValue ((1u << 1) - 1)
#define Reg_PS_U_LAYOUT_U10_Layout_BitField_MaxValue ((1u << 1) - 1)
#define Reg_PS_U_LAYOUT_U11_Layout_BitField_MaxValue ((1u << 1) - 1)
#define Reg_PS_U_LAYOUT_U12_Layout_BitField_MaxValue ((1u << 1) - 1)
#define Reg_PS_U_LAYOUT_U13_Layout_BitField_MaxValue ((1u << 1) - 1)
#define Reg_PS_U_LAYOUT_U14_Layout_BitField_MaxValue ((1u << 1) - 1)
#define Reg_PS_U_LAYOUT_U15_Layout_BitField_MaxValue ((1u << 1) - 1)
#define Reg_PS_U_LAYOUT_U16_Layout_BitField_MaxValue ((1u << 1) - 1)
#define Reg_PS_U_LAYOUT_U17_Layout_BitField_MaxValue ((1u << 1) - 1)
#define Reg_PS_U_LAYOUT_U18_Layout_BitField_MaxValue ((1u << 1) - 1)
#define Reg_PS_U_LAYOUT_U19_Layout_BitField_MaxValue ((1u << 1) - 1)
#define Reg_PS_U_LAYOUT_U20_Layout_BitField_MaxValue ((1u << 1) - 1)
#define Reg_PS_U_LAYOUT_U21_Layout_BitField_MaxValue ((1u << 1) - 1)
#define Reg_PS_U_LAYOUT_U22_Layout_BitField_MaxValue ((1u << 1) - 1)
#define Reg_PS_U_LAYOUT_U23_Layout_BitField_MaxValue ((1u << 1) - 1)
#define Reg_PS_U_LAYOUT_U24_Layout_BitField_MaxValue ((1u << 1) - 1)
#define Reg_PS_U_LAYOUT_U25_Layout_BitField_MaxValue ((1u << 1) - 1)
#define Reg_PS_U_LAYOUT_U26_Layout_BitField_MaxValue ((1u << 1) - 1)
#define Reg_PS_U_LAYOUT_U27_Layout_BitField_MaxValue ((1u << 1) - 1)
#define Reg_PS_U_LAYOUT_U28_Layout_BitField_MaxValue ((1u << 1) - 1)
#define Reg_PS_U_LAYOUT_U29_Layout_BitField_MaxValue ((1u << 1) - 1)
#define Reg_PS_U_LAYOUT_U30_Layout_BitField_MaxValue ((1u << 1) - 1)
#define Reg_PS_U_LAYOUT_U31_Layout_BitField_MaxValue ((1u << 1) - 1)

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int U0_Layout                 : 1;   // U# data layout, maybe vertical/horizontal.
        unsigned int U1_Layout                 : 1;   // U# data layout, maybe vertical/horizontal.
        unsigned int U2_Layout                 : 1;   // U# data layout, maybe vertical/horizontal.
        unsigned int U3_Layout                 : 1;   // U# data layout, maybe vertical/horizontal.
        unsigned int U4_Layout                 : 1;   // U# data layout, maybe vertical/horizontal.
        unsigned int U5_Layout                 : 1;   // U# data layout, maybe vertical/horizontal.
        unsigned int U6_Layout                 : 1;   // U# data layout, maybe vertical/horizontal.
        unsigned int U7_Layout                 : 1;   // U# data layout, maybe vertical/horizontal.
        unsigned int U8_Layout                 : 1;   // U# data layout, maybe vertical/horizontal.
        unsigned int U9_Layout                 : 1;   // U# data layout, maybe vertical/horizontal.
        unsigned int U10_Layout                : 1;   // U# data layout, maybe vertical/horizontal.
        unsigned int U11_Layout                : 1;   // U# data layout, maybe vertical/horizontal.
        unsigned int U12_Layout                : 1;   // U# data layout, maybe vertical/horizontal.
        unsigned int U13_Layout                : 1;   // U# data layout, maybe vertical/horizontal.
        unsigned int U14_Layout                : 1;   // U# data layout, maybe vertical/horizontal.
        unsigned int U15_Layout                : 1;   // U# data layout, maybe vertical/horizontal.
        unsigned int U16_Layout                : 1;   // U# data layout, maybe vertical/horizontal.
        unsigned int U17_Layout                : 1;   // U# data layout, maybe vertical/horizontal.
        unsigned int U18_Layout                : 1;   // U# data layout, maybe vertical/horizontal.
        unsigned int U19_Layout                : 1;   // U# data layout, maybe vertical/horizontal.
        unsigned int U20_Layout                : 1;   // U# data layout, maybe vertical/horizontal.
        unsigned int U21_Layout                : 1;   // U# data layout, maybe vertical/horizontal.
        unsigned int U22_Layout                : 1;   // U# data layout, maybe vertical/horizontal.
        unsigned int U23_Layout                : 1;   // U# data layout, maybe vertical/horizontal.
        unsigned int U24_Layout                : 1;   // U# data layout, maybe vertical/horizontal.
        unsigned int U25_Layout                : 1;   // U# data layout, maybe vertical/horizontal.
        unsigned int U26_Layout                : 1;   // U# data layout, maybe vertical/horizontal.
        unsigned int U27_Layout                : 1;   // U# data layout, maybe vertical/horizontal.
        unsigned int U28_Layout                : 1;   // U# data layout, maybe vertical/horizontal.
        unsigned int U29_Layout                : 1;   // U# data layout, maybe vertical/horizontal.
        unsigned int U30_Layout                : 1;   // U# data layout, maybe vertical/horizontal.
        unsigned int U31_Layout                : 1;   // U# data layout, maybe vertical/horizontal.
    } reg;
} Reg_Ps_U_Layout;

#define Reg_PS_U_CFG_Ps_U_Range_BitField_MaxValue ((1u << 8) - 1)
#define Reg_PS_U_CFG_Rt0_En_BitField_MaxValue ((1u << 1) - 1)
#define Reg_PS_U_CFG_Rt1_En_BitField_MaxValue ((1u << 1) - 1)
#define Reg_PS_U_CFG_Rt2_En_BitField_MaxValue ((1u << 1) - 1)
#define Reg_PS_U_CFG_Rt3_En_BitField_MaxValue ((1u << 1) - 1)
#define Reg_PS_U_CFG_Rt4_En_BitField_MaxValue ((1u << 1) - 1)
#define Reg_PS_U_CFG_Rt5_En_BitField_MaxValue ((1u << 1) - 1)
#define Reg_PS_U_CFG_Rt6_En_BitField_MaxValue ((1u << 1) - 1)
#define Reg_PS_U_CFG_Rt7_En_BitField_MaxValue ((1u << 1) - 1)
#define Reg_PS_U_CFG_Reserved_BitField_MaxValue ((1u << 16) - 1)

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Ps_U_Range : 8;  // Address range of U# (0~64). Actual U# defined by driver
                                      // to do U# indexing.
        unsigned int Rt0_En : 1;  // RT0 is enabled/disabled. DOUT need check (O0_En &
                                  // Rt0_En) to output color
        unsigned int Rt1_En : 1;  // RT1 is enabled/disabled.
        unsigned int Rt2_En : 1;  // RT2 is enabled/disabled.
        unsigned int Rt3_En : 1;  // RT3 is enabled/disabled.
        unsigned int Rt4_En : 1;  // RT4 is enabled/disabled.
        unsigned int Rt5_En : 1;  // RT5 is enabled/disabled.
        unsigned int Rt6_En : 1;  // RT6 is enabled/disabled.
        unsigned int Rt7_En : 1;  // RT7 is enabled/disabled.
        unsigned int Reserved : 16;  // Reserved
    } reg_CHX004;
    struct
    {
        unsigned int Ps_U_Range : 8;   // Address range of  U# (0~64). Actual U# defined by driver to
                                       // do U# indexing.
        unsigned int Reserved : 24;  // Reserved
    } reg;
} Reg_Ps_U_Cfg;


#define Reg_PS_INSTR0_Address_Low_BitField_MaxValue ((1u << 32) - 1)

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Address_Low               : 32;  // Pixel Shader Instruction Base Address lowest32bits.
                                                      // total 40bits
    } reg;
} Reg_Ps_Instr0;

#define Reg_PS_INSTR1_Address_High_BitField_MaxValue ((1u << 8) - 1)
#define Reg_PS_INSTR1_Reserved_BitField_MaxValue ((1u << 24) - 1)

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Address_High              : 8;   // Pixel Shader Instruction Base Address high8bits.
                                                      // Intotal 40bits
        unsigned int Reserved                  : 24;  // Reserved
    } reg;
} Reg_Ps_Instr1;

#define Reg_PS_INSTR_RANGE_Ins_Range_BitField_MaxValue ((1u << 16) - 1)
#define Reg_PS_INSTR_RANGE_Reserved_BitField_MaxValue ((1u << 16) - 1)

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Ins_Range                 : 16;  // Instruction range of PS shader codes (0 ~ (16K-1)). Each
                                                      // instr occupies 4Dw (128bits)
        unsigned int Reserved                  : 16;  // Reserved
    } reg;
} Reg_Ps_Instr_Range;

#define Reg_PS_CB_CFG_Ps_Cb_Range_BitField_MaxValue ((1u << 9) - 1)
#define Reg_PS_CB_CFG_Reserved_BitField_MaxValue ((1u << 23) - 1)

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Ps_Cb_Range               : 9;   // Address range of PS const (0 ~ 256E), 4DWords element
                                                      // aligned.
        unsigned int Reserved                  : 23;  // Reserved
    } reg;
} Reg_Ps_Cb_Cfg;

#define Reg_PS_REV_8ALIGNED_Reserved_BitField_MaxValue ((1u << 32) - 1)

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Reserved                  : 32;  // Reserved
    } reg;
} Reg_Ps_Rev_8aligned;

#define Reg_PS_REV_CB_Reserved_BitField_MaxValue ((1u << 32) - 1)

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Reserved                  : 32;  // Reserved
    } reg;
} Reg_Ps_Rev_Cb;

#define Reg_PS_CB_DATA_Const_BitField_MaxValue ((1u << 32) - 1)

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Const                     : 32;  // Reserved
    } reg;
} Reg_Ps_Cb_Data;

typedef struct _Eu_Ps_regs
{
    Reg_Ps_Cfg                       reg_Ps_Cfg;
    Reg_Ps_Ctrl                      reg_Ps_Ctrl;
    Reg_Ps_Out_Cfg                   reg_Ps_Out_Cfg;
    Reg_Ps_Doutbuf_Cfg               reg_Ps_Doutbuf_Cfg;
    Reg_Ps_Pm_Cfg                    reg_Ps_Pm_Cfg;
    Reg_Ps_Pm_Id                     reg_Ps_Pm_Id;
    Reg_Ps_Pm_Range                  reg_Ps_Pm_Range;
    Reg_Ps_Out_Mapping               reg_Ps_Out_Mapping;
    union
    {
        Reg_Ps_Out_Rtfmt                 reg_Ps_Out_Rtfmt;
        Reg_Ps_Out_Fmt                   reg_Ps_Out_Fmt;
    };
    Reg_Ps_U_Enable                  reg_Ps_U_Enable[2];
    Reg_Ps_U_Fmt                     reg_Ps_U_Fmt[8];
    Reg_Ps_U_Layout                  reg_Ps_U_Layout[2];
    Reg_Ps_U_Cfg                     reg_Ps_U_Cfg;
    Reg_Ps_Instr0                    reg_Ps_Instr0;
    Reg_Ps_Instr1                    reg_Ps_Instr1;
    Reg_Ps_Instr_Range               reg_Ps_Instr_Range;
    Reg_Ps_Cb_Cfg                    reg_Ps_Cb_Cfg;
    Reg_Ps_Rev_8aligned              reg_Ps_Rev_8aligned[6];
    Reg_Ps_Rev_Cb                    reg_Ps_Rev_Cb[224];
    Reg_Ps_Cb_Data                   reg_Ps_Cb_Data[2048];
} Eu_Ps_regs;

#endif
