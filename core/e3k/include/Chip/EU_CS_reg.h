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
#ifndef _EU_CS_REG_H
#define _EU_CS_REG_H


#ifndef        EU_CS_BLOCKBASE_INF
    #define    EU_CS_BLOCKBASE_INF
    #define    BLOCK_EU_CS_VERSION 1
    #define    BLOCK_EU_CS_TIMESTAMP "2018/11/26 19:05:36"
    #define    EU_CS_BLOCK                                                0xE // match with BlockID.h
    #define    EU_CS_REG_START                                            0x0 // match with BlockID.h
    #define    EU_CS_REG_END                                              0x900 // match with BlockID.h
    #define    EU_CS_REG_LIMIT                                            0x900 // match with BlockID.h
#endif

// Register offset definition
//#define        Reg_Eu_Full_Glb_Offset                                     0x0
#define        Reg_Eu_Cs_Glb_Offset                                       0x1
#define        Reg_Cs_Cfg_Offset                                          0x2
#define        Reg_Cs_Sm_Cfg_Offset                                       0x3
#define        Reg_Cs_Pm_Cfg_Offset                                       0x4
#define        Reg_Cs_Pm_Id_Offset                                        0x5
#define        Reg_Cs_Pm_Range_Offset                                     0x6
#define        Reg_Cs_U_Enable_Offset                                     0x7
#define        Reg_Cs_U_Fmt_Offset                                        0x9
#define        Reg_Cs_U_Layout_Offset                                     0x11
#define        Reg_Cs_U_Cfg_Offset                                        0x13
#define        Reg_Cs_Instr0_Offset                                       0x14
#define        Reg_Cs_Instr1_Offset                                       0x15
#define        Reg_Cs_Instr_Range_Offset                                  0x16
#define        Reg_Cs_Cb_Cfg_Offset                                       0x17
#define        Reg_Cs_Rev_8aligned_Offset                                 0x18
#define        Reg_Cs_Rev_Cb_Offset                                       0x20
#define        Reg_Cs_Cb_Data_Offset                                      0x100

// Block constant definition
//typedef enum
//{
//    EU_FULL_GLB_SHADER_DBG_EN_DISABLED       = 0,   // 0: Disable, normal mode
//    EU_FULL_GLB_SHADER_DBG_EN_ENABLED        = 1,   // 1: Enable, the EU enables breakpoint handling based on
                                                    // MMIO shader debugging register settings
//} EU_FULL_GLB_SHADER_DBG_EN;
//typedef enum
//{
//    EU_FULL_GLB_THD_SLOT_SIZE_8THD_SLOT      = 0,   // 0: Each set can allocate 8 thread slot. Default.
//    EU_FULL_GLB_THD_SLOT_SIZE_4THD_SLOT      = 1,   // 1: Each set can allocate 4 thread slot.
//    EU_FULL_GLB_THD_SLOT_SIZE_2THD_SLOT      = 2,   // 2: Each set can allocate 2 thread slot.
//    EU_FULL_GLB_THD_SLOT_SIZE_RESERVED       = 3,   // Reserved
//} EU_FULL_GLB_THD_SLOT_SIZE;
//typedef enum
//{
//    EU_FULL_GLB_UAV_CONTINUE_CHK_DISABLED    = 0,   // 0: disabled, LS doesn't do continuous check for CRF input
                                                    // address
//    EU_FULL_GLB_UAV_CONTINUE_CHK_ENABLED     = 1,   // 1: enabled, UAV check CRF input addresses for continuous
                                                    // address, (default is enabled)
//} EU_FULL_GLB_UAV_CONTINUE_CHK;
//typedef enum
//{
//    EU_FULL_GLB_UAV_BUF_SIZE_0LINES          = 0,   // 0lines for UAV, UAV_En should be 0.
//    EU_FULL_GLB_UAV_BUF_SIZE_16LINES         = 1,
//    EU_FULL_GLB_UAV_BUF_SIZE_32LINES         = 2,
//    EU_FULL_GLB_UAV_BUF_SIZE_64LINES         = 3,
//    EU_FULL_GLB_UAV_BUF_SIZE_128LINES        = 4,
//} EU_FULL_GLB_UAV_BUF_SIZE;
//typedef enum
//{
//    EU_FULL_GLB_IC_TO_L2_DISABLED            = 0,   // 0: disabled, IC miss request will go to MXU directly.
//    EU_FULL_GLB_IC_TO_L2_ENABLED             = 1,   // 1: enabled, IC miss request will go to L2 firstly, and instr
                                                    // may be cached in L2 (defalut value)
//} EU_FULL_GLB_IC_TO_L2;
typedef enum
{
    CS_CFG_AND_V_MASK_DISABLED               = 0,   // 0: Disable
    CS_CFG_AND_V_MASK_ENABLED                = 1,   // 1: Enable, HW ands the sample predicate with the P7 quad
                                                    // based mask, also LS, branch
} CS_CFG_AND_V_MASK;
typedef enum
{
    CS_CFG_RD_MODE_NEAR                      = 0,   // 00: Round toward nearest even
    CS_CFG_RD_MODE_ZERO                      = 1,   // 01: Round to Zero
    CS_CFG_RD_MODE_POS                       = 2,   // 10: Round to Positive Infinity
    CS_CFG_RD_MODE_NEG                       = 3,   // 11: Round to Negative infinity
} CS_CFG_RD_MODE;
typedef enum
{
    CS_CFG_FP_MODE_STD_IEEE                  = 0,   // 00: Standard IEEE-745
    CS_CFG_FP_MODE_NSTD_IEEE                 = 1,   // 01: Non-Standard IEEE-745
    CS_CFG_FP_MODE_MS_ALT                    = 2,   // 10: Microsoft Alt float point
    CS_CFG_FP_MODE_RESERVED                  = 3,   // 11: Reserved
} CS_CFG_FP_MODE;
typedef enum
{
    CS_CFG_DENORM_EN_DISABLED                = 0,   // 0: Disable, denormal number will be flush to 0 for floating
    CS_CFG_DENORM_EN_ENABLED                 = 1,   // 1: Enable, denormal number is kept for floating
} CS_CFG_DENORM_EN;
typedef enum
{
    CS_CFG_THREAD_MODE_SIMD32                = 0,   // 0: only use FP/DP instr
    CS_CFG_THREAD_MODE_SIMD64                = 1,   // 1: use HP/FP/DP/Mix instr
} CS_CFG_THREAD_MODE;
typedef enum
{
    CS_CFG_PATTERN_MODE_FLAT                 = 0,   // Flat mode, normal mode. all 3D are flat mode.
    CS_CFG_PATTERN_MODE_LINEAR               = 1,   // 32x1x1 or 64x1x1 1D-linear pattern
    CS_CFG_PATTERN_MODE_TILE                 = 2,   // 8x4x1 or 8x8x1 2D-tile pattern
    CS_CFG_PATTERN_MODE_RESERVED             = 3,   // Reserved
} CS_CFG_PATTERN_MODE;
typedef enum
{
    CS_CFG_U_SHARP_CACHED_DISABLED           = 0,   // 0: disabled, register mode. U# are recorded in registers.
                                                    // LS need read fmt to convert data.
    CS_CFG_U_SHARP_CACHED_ENABLED            = 1,   // 1: enabled, U# is cache mode, LS need read fmt from WLS U#
                                                    // cache and then do data conversion.
} CS_CFG_U_SHARP_CACHED;
typedef enum
{
    CS_SM_CFG_SM_EN_OFF                      = 0,   // 0: OFF, no SM used in CS threads
    CS_SM_CFG_SM_EN_ON                       = 1,   // 1: ON, CS uses SM. The CS cannot run with other 3D-FS or other
                                                    // CS task with SM.
} CS_SM_CFG_SM_EN;
typedef enum
{
    CS_PM_CFG_PM_EN_DISABLED                 = 0,   // 0: Disable. Should no PM instr in CS.
    CS_PM_CFG_PM_EN_ENABLED                  = 1,   // 1: Enable, CS use PM.
} CS_PM_CFG_PM_EN;
typedef enum
{
    CS_PM_CFG_TH_PM_SIZE_8KDW                = 0,   // 8K dword, can hold 256# for Simd32, or 128# for Simd64
                                                    // threads
    CS_PM_CFG_TH_PM_SIZE_16KDW               = 1,   // 512# for Simd32; 256# for Simd64
    CS_PM_CFG_TH_PM_SIZE_32KDW               = 2,   // 1024# for Simd32; 512# for Simd64
    CS_PM_CFG_TH_PM_SIZE_64KDW               = 3,   // 2048# for Simd32; 1024# for Simd64
    CS_PM_CFG_TH_PM_SIZE_128KDW              = 4,   // 4096# for Simd32; 2048# for Simd64
    CS_PM_CFG_TH_PM_SIZE_256KDW              = 5,   // 8192# for Simd32; 4096# for Simd64
    CS_PM_CFG_TH_PM_SIZE_512KDW              = 6,   // 16384# for simd32 thread; 8192 for simd64 thread.
    CS_PM_CFG_TH_PM_SIZE_1024KDW             = 7,   // 32k# for simd32 thread; 16k# form simd64 thread.
    CS_PM_CFG_TH_PM_SIZE_1040KDW             = 8,   // 16640# for simd64 thread
} CS_PM_CFG_TH_PM_SIZE;
typedef enum
{
    CS_U_ENABLE_U0_EN_DISABLE                = 0,   // 0: U# disable, discard store and return 0 values for load
    CS_U_ENABLE_U0_EN_ENABLE                 = 1,   // 1: U# enable, the following U# fmt and layout are valid
} CS_U_ENABLE_U0_EN;
typedef enum
{
    CS_U_FMT_U0_DATA_FMT_FP32                = 0,   // 000: Floating-point 32-bits
    CS_U_FMT_U0_DATA_FMT_FP16                = 1,   // 001: Floating-point 16-bit
    CS_U_FMT_U0_DATA_FMT_SINT32              = 2,   // 010: Signed Integer 32-bit
    CS_U_FMT_U0_DATA_FMT_SINT16              = 3,   // 011: Signed Integer 16-bit
    CS_U_FMT_U0_DATA_FMT_UINT32              = 4,   // 100: Unsigned integer 32-bit
    CS_U_FMT_U0_DATA_FMT_UINT16              = 5,   // 101: Unsigned integer 16-bit
    CS_U_FMT_U0_DATA_FMT_UNORM24             = 6,   // 110: Unorm 24-bit
    CS_U_FMT_U0_DATA_FMT_UNORM16             = 7,   // 111: Unorm 16-bit
    CS_U_FMT_U0_DATA_FMT_UNORM10             = 8,   // 1000: Unorm 10-bit
    CS_U_FMT_U0_DATA_FMT_UNORM8              = 9,   // 1001: Unorm 8-bit
    CS_U_FMT_U0_DATA_FMT_SINT8               = 10,  // 1010: Signed Integer 8-bit
    CS_U_FMT_U0_DATA_FMT_UINT8               = 11,  // 1011: Unsigned integer 8-bit
    CS_U_FMT_U0_DATA_FMT_SNORM8              = 12,  // 1100: Snorm 8-bit
    CS_U_FMT_U0_DATA_FMT_SNORM16             = 13,  // 1100: Snorm 16-bit
    CS_U_FMT_U0_DATA_FMT_UINT64              = 14,  // 1101: unsigned int-64bits, only for reduction
    CS_U_FMT_U0_DATA_FMT_RESERVED            = 15,  // 1111: Reserved
} CS_U_FMT_U0_DATA_FMT;
typedef enum
{
    CS_U_LAYOUT_U0_LAYOUT_VERTICAL           = 0,   // 0: Untyped vertical buffer, should be 32bits
    CS_U_LAYOUT_U0_LAYOUT_HORIZONTAL         = 1,   // 1: Untyped/Typed horizontal buffer.
} CS_U_LAYOUT_U0_LAYOUT;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////            EU_CS Block (Block ID = 14) Register Definitions                          ///////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//typedef union
//{
//    unsigned int uint;
//    struct
//    {
//        unsigned int Shader_Dbg_En             : 1;   // CS/3D shader debugging mode enable
//        unsigned int Thd_Slot_Size             : 2;   // Allocate thread slot size in one set, to let driver use less
                                                      // PM size
//        unsigned int Uav_En                    : 1;   // There are LS UAV opeartion in 3D/CS shader. If true, DOUT
                                                      // need allocate UAV buffer.
//        unsigned int Uav_Continue_Chk          : 1;   // UAV LS continuous check function, default is Enabled
//        unsigned int Uav_Buf_Size              : 4;   // UAV buffer size by bit-width specification, shared with
                                                      // 3D-PS, 0 ~ 192 buffer lines in total,  LS may use 128 lines
                                                      // buffer in max for all 3D/CS LS requests. (global reg,
                                                      // should be sync by all process)
//        unsigned int U_3d_Base                 : 5;   // (0~16), 3D U# base, total 128U# per slice, served for one 3D
                                                      // and two CS processes. 8U# in one group, (0~8*Base) for
                                                      // CS(L/H), (8*Base~127) for 3D. From low to high, the order
                                                      // is CS_L, CS_H, 3D_FS, 3D_PS
//        unsigned int Cb_3d_Base                : 6;   // (0~32) 3D CB base, total 8Kbytes (512x 128bits) per PE,
                                                      // served for one 3D and two CS processes. 2kbits (16E) in one
                                                      // group, (0~16*Base) for CS(L/H), (16*Base ~ 511) for 3D.
                                                      // From low to high, the order is CS_L, CS_H, 3D_FS, 3D_PS
//        unsigned int Ic_To_L2                  : 1;   // IC miss request will go to L2 firstly, and instr may be
                                                      // cached in L2.
//        unsigned int Reserved                  : 11;  // Reserved
//    } reg;
//} Reg_Eu_Full_Glb;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int U_Csh_Base                : 5;   // (0~16), CS-High U# base, total 128U# per slice, served for
                                                      // one 3D and two CS processes. 8U# in one group.
        unsigned int Cb_Csh_Base               : 6;   // (0~32) CS-High CB base, total 8Kbytes (512x 128bits) per
                                                      // PE, served for one 3D and two CS processes. 2kbits (16E-dw)
                                                      // in one group.
        unsigned int Max_Threads               : 8;   // Maximum number of total CS threads kickoff in one slice.
                                                      // Value 0~64 threads. Default value is 0x40 for 64 threads.
        unsigned int Reserved                  : 13;  // Reserved
    } reg;
} Reg_Eu_Cs_Glb;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Crf_Size                  : 6;   // Total FP Common Register File (FP-CRF) space. Value is
                                                      // 1~52 4-component elements, including temp, input,
                                                      // output registers.
        unsigned int And_V_Mask                : 1;   // And sample predicate with P7 quad based mask in CS
        unsigned int Rd_Mode                   : 2;   // EU floating point rounding mode
        unsigned int Fp_Mode                   : 2;   // EU floating point operation mode
        unsigned int Denorm_En                 : 1;   // Whether support denomal computation
        unsigned int Thread_Mode               : 1;   // CS initial execution mode, this affects how the CSTC
                                                      // allocate thread IDs into the CRF and thread size
        unsigned int Pattern_Mode              : 3;   // CS kick-off thread with pattern mode.
        unsigned int Th_Num_In_Group           : 6;   // Total number (1~32) of EU hardware threads for each Thread
                                                      // Group. in one PE.  16 threads for SIMD64 or 32 threads for
                                                      // SIMD32
        unsigned int U_Sharp_Cached            : 1;   // UAV U# cached or register mode.
        unsigned int Reserved                  : 9;   // Reserved
    } reg;
} Reg_Cs_Cfg;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Sm_En                     : 1;   // Whether CS uses SM. If so, it cannot be runn with 3D-FS or
                                                      // other CS task with SM.
        unsigned int Group_Sm_Size             : 16;  // Shared Memory size (0~1024 lines) for each Thread Group.
                                                      // Each line has 16dwords.
        unsigned int Reserved                  : 15;  // Reserved
    } reg;
} Reg_Cs_Sm_Cfg;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Pm_En                     : 1;   // Wheter CS uses PM.
        unsigned int Th_Pm_Size                : 4;   // Size of thread external storage in memory/L2 including
                                                      // call stacks, CRF spill, x# buffers, etc. IF CS and 3D use the
                                                      // same UAV buffer, it should use the same PM_Size
        unsigned int Reserved                  : 27;  // Reserved
    } reg;
} Reg_Cs_Pm_Cfg;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Pm_U_Id                   : 32;  // CS private memory U_sharp id, to load/store private
                                                      // memory.  LS unit will convert PM into Untyped LD/ST. In reg
                                                      // mode, it should < 64, or else regard it as out-of-range; in
                                                      // cache mode, it is the offset in cache.
    } reg;
} Reg_Cs_Pm_Id;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Cs_Idx_Ele_Range          : 14;  // Range of CS thread external storage used by OCL Local
                                                      // Memory or CS x#s in memory. Note the compiler used external
                                                      // thread storage i.e. call stacks, etc. is not included, it
                                                      // should be placed in the upper memory. Element based
        unsigned int Reserved                  : 18;  // Reserved
    } reg;
} Reg_Cs_Pm_Range;

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
} Reg_Cs_U_Enable;

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
} Reg_Cs_U_Fmt;

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
} Reg_Cs_U_Layout;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Cs_U_Range                : 8;   // Address range of  U# (0~64), should less or equal to
                                                      // CS_U_Size , defined in EU_CS_Glb. For U# indexing check.
        unsigned int Reserved                  : 24;  // Reserved
    } reg;
} Reg_Cs_U_Cfg;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Address_Low               : 32;  //  Compute Shader Instruction Base Address low32bits.
                                                      // Total 40bits
    } reg;
} Reg_Cs_Instr0;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Address_High              : 8;   //  Compute Shader Instruction Base Address high8bits.
                                                      // Total 40bits
        unsigned int Reserved                  : 24;  // Reserved
    } reg;
} Reg_Cs_Instr1;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Ins_Range                 : 16;  // Instruction range of CS shader codes (0 ~ (16K-1)). Each
                                                      // instr occupies 4Dw (128bits)
        unsigned int Reserved                  : 16;  // Reserved
    } reg;
} Reg_Cs_Instr_Range;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Cs_Cb_Range               : 9;   // Address range of CS const (0 ~ 256E), 4DWords element
                                                      // aligned. <= CS_CB_Size, defined in EU_CS_Glb.
        unsigned int Reserved                  : 23;  // Reserved
    } reg;
} Reg_Cs_Cb_Cfg;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Reserved                  : 32;  // Reserved
    } reg;
} Reg_Cs_Rev_8aligned;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Reserved                  : 32;  // Reserved
    } reg;
} Reg_Cs_Rev_Cb;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Const                     : 32;  // Reserved
    } reg;
} Reg_Cs_Cb_Data;

typedef struct _Eu_Cs_regs
{
    Reg_Eu_Full_Glb                  reg_Eu_Full_Glb;
    Reg_Eu_Cs_Glb                    reg_Eu_Cs_Glb;
    Reg_Cs_Cfg                       reg_Cs_Cfg;
    Reg_Cs_Sm_Cfg                    reg_Cs_Sm_Cfg;
    Reg_Cs_Pm_Cfg                    reg_Cs_Pm_Cfg;
    Reg_Cs_Pm_Id                     reg_Cs_Pm_Id;
    Reg_Cs_Pm_Range                  reg_Cs_Pm_Range;
    Reg_Cs_U_Enable                  reg_Cs_U_Enable[2];
    Reg_Cs_U_Fmt                     reg_Cs_U_Fmt[8];
    Reg_Cs_U_Layout                  reg_Cs_U_Layout[2];
    Reg_Cs_U_Cfg                     reg_Cs_U_Cfg;
    Reg_Cs_Instr0                    reg_Cs_Instr0;
    Reg_Cs_Instr1                    reg_Cs_Instr1;
    Reg_Cs_Instr_Range               reg_Cs_Instr_Range;
    Reg_Cs_Cb_Cfg                    reg_Cs_Cb_Cfg;
    Reg_Cs_Rev_8aligned              reg_Cs_Rev_8aligned[8];
    Reg_Cs_Rev_Cb                    reg_Cs_Rev_Cb[224];
    Reg_Cs_Cb_Data                   reg_Cs_Cb_Data[2048];
} Eu_Cs_regs;

#endif
