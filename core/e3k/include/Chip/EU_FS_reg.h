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
#ifndef _EU_FS_REG_H
#define _EU_FS_REG_H


#ifndef        EU_FS_BLOCKBASE_INF
    #define    EU_FS_BLOCKBASE_INF
    #define    BLOCK_EU_FS_VERSION 1
    #define    BLOCK_EU_FS_TIMESTAMP "2018/11/26 19:05:36"
    #define    EU_FS_BLOCK                                                0x3 // match with BlockID.h
    #define    EU_FS_REG_START                                            0x0 // match with BlockID.h
    #define    EU_FS_REG_END                                              0x900 // match with BlockID.h
    #define    EU_FS_REG_LIMIT_CHX004                                     0xA80 // match with BlockID.h
    #define    EU_FS_REG_LIMIT_Elt3k                                      0xAC0 // match with BlockID.h
#endif

// Register offset definition
#define        Reg_Eu_Full_Glb_Offset                                     0x0
#define        Reg_Eu_3d_Glb_Offset                                       0x1
#define        Reg_Eu_Fe_Glb_Offset                                       0x2
#define        Reg_Mv_Cfg_Offset                                          0x3
#define        Reg_Vc_Size_Cfg0_Offset                                    0x4
#define        Reg_Vc_Size_Cfg1_Offset                                    0x5
#define        Reg_Vc_Base_Cfg0_Offset                                    0x6
#define        Reg_Vc_Base_Cfg1_Offset                                    0x7
#define        Reg_Fs_Cfg_Offset                                          0x8
#define        Reg_Max_Threads_Cfg_Offset                                 0x9
#define        Reg_Antilock_Cfg_Offset                                    0xA
#define        Reg_Vs_Ctrl_Offset                                         0xB
#define        Reg_Hs_Ctrl_Offset                                         0xC
#define        Reg_Hs_In_Cfg_Offset                                       0xD
#define        Reg_Hs_Out_Cfg_Offset                                      0xE
#define        Reg_Ds_Ctrl_Offset                                         0xF
#define        Reg_Ds_In_Cfg_Offset                                       0x10
#define        Reg_Gs_Ctrl_Offset                                         0x11
#define        Reg_Gs_In_Cfg_Offset                                       0x12
#define        Reg_Gs_Out_Cfg_Offset                                      0x13
#define        Reg_Ts_Ctrl_Offset                                         0x14
#define        Reg_Ts_Input_Mapping0_Offset                               0x15
#define        Reg_Ts_Input_Mapping1_Offset                               0x16
#define        Reg_Ts_Factors_Const_Offset                                0x17
#define        Reg_Fs_Cs_Ctrl_Offset                                      0x1D
#define        Reg_Fs_Cs_Sm_Cfg_Offset                                    0x1E
#define        Reg_Fs_Out_Loc0_Offset                                     0x1F
#define        Reg_Fs_Out_Loc1_Offset                                     0x20
#define        Reg_Fs_Out_Mapping_Offset                                  0x21
#define        Reg_Fs_Out_Comp_Offset                                     0x2D
#define        Reg_Fs_Out_Mask_Offset                                     0x30
#define        Reg_Fs_Pm_Cfg_Offset                                       0x36
#define        Reg_Fs_Pm_Id_Offset                                        0x37
#define        Reg_Vgs_Pm_Range_Offset                                    0x38
#define        Reg_Hds_Pm_Range_Offset                                    0x39
#define        Reg_Fs_U_Enable_Offset                                     0x3A
#define        Reg_Fs_U_Fmt_Offset                                        0x3E
#define        Reg_Fs_U_Layout_Offset                                     0x4E
#define        Reg_Vcs_U_Cfg_Offset                                       0x52
#define        Reg_Hs_U_Cfg_Offset                                        0x53
#define        Reg_Ds_U_Cfg_Offset                                        0x54
#define        Reg_Gs_U_Cfg_Offset                                        0x55
#define        Reg_Vcs_Instr0_Offset                                      0x56
#define        Reg_Vcs_Instr1_Offset                                      0x57
#define        Reg_Vcs_Instr_Range_Offset                                 0x58
#define        Reg_Hs_Instr0_Offset                                       0x59
#define        Reg_Hs_Instr1_Offset                                       0x5A
#define        Reg_Hs_Instr_Range_Offset                                  0x5B
#define        Reg_Ds_Instr0_Offset                                       0x5C
#define        Reg_Ds_Instr1_Offset                                       0x5D
#define        Reg_Ds_Instr_Range_Offset                                  0x5E
#define        Reg_Gs_Instr0_Offset                                       0x5F
#define        Reg_Gs_Instr1_Offset                                       0x60
#define        Reg_Gs_Instr_Range_Offset                                  0x61
#define        Reg_Vcs_Cb_Cfg_Offset                                      0x62
#define        Reg_Hs_Cb_Cfg_Offset                                       0x63
#define        Reg_Ds_Cb_Cfg_Offset                                       0x64
#define        Reg_Gs_Cb_Cfg_Offset                                       0x65
#define        Reg_Fs_Cb_Cfg_Offset                                       0x66
#define        Reg_Fs_Rev_8aligned_Offset                                 0x67
#define        Reg_Fs_Rev_Cb_Offset                                       0x68
#define        Reg_Fs_Cb_Data_Offset                                      0x100
#define        Reg_Sh_Dbg_Offset                                          0x900
#define        Reg_Dbg_Cfg_Offset                                         0x900
#define        Reg_Dbg_Bp_Stat_Offset                                     0x920
#define        Reg_Dbg_Bp_Pc_Offset                                       0x940
#define        Reg_Dbg_Int_Instr_Offset                                   0xA40
#define        Reg_Dbg_Res_Instr_Offset                                   0xA60
#define        Reg_Dbg_Time_Stamp_Offset                                  0xA80

// Block constant definition
typedef enum
{
    EU_FULL_GLB_SHADER_DBG_EN_DISABLED       = 0,   // 0: Disable, normal mode
    EU_FULL_GLB_SHADER_DBG_EN_ENABLED        = 1,   // 1: Enable, the EU enables breakpoint handling based on
                                                    // MMIO shader debugging register settings
} EU_FULL_GLB_SHADER_DBG_EN;
typedef enum
{
    EU_FULL_GLB_THD_SLOT_SIZE_8THD_SLOT      = 0,   // 0: Each set can allocate 8 thread slot. Default.
    EU_FULL_GLB_THD_SLOT_SIZE_4THD_SLOT      = 1,   // 1: Each set can allocate 4 thread slot.
    EU_FULL_GLB_THD_SLOT_SIZE_2THD_SLOT      = 2,   // 2: Each set can allocate 2 thread slot.
    EU_FULL_GLB_THD_SLOT_SIZE_RESERVED       = 3,   // Reserved
} EU_FULL_GLB_THD_SLOT_SIZE;
typedef enum
{
    EU_FULL_GLB_UAV_CONTINUE_CHK_DISABLED    = 0,   // 0: disabled, LS doesn't do continuous check for CRF input
                                                    // address
    EU_FULL_GLB_UAV_CONTINUE_CHK_ENABLED     = 1,   // 1: enabled, UAV check CRF input addresses for continuous
                                                    // address, (default is enabled)
} EU_FULL_GLB_UAV_CONTINUE_CHK;
typedef enum
{
    EU_FULL_GLB_UAV_BUF_SIZE_0LINES          = 0,   // 0lines for UAV, UAV_En should be 0.
    EU_FULL_GLB_UAV_BUF_SIZE_16LINES         = 4,
    EU_FULL_GLB_UAV_BUF_SIZE_32LINES         = 5,
    EU_FULL_GLB_UAV_BUF_SIZE_64LINES         = 6,
    EU_FULL_GLB_UAV_BUF_SIZE_128LINES        = 7,
} EU_FULL_GLB_UAV_BUF_SIZE;
typedef enum
{
    EU_FULL_GLB_IC_TO_L2_DISABLED            = 0,   // 0: disabled, IC miss request will go to MXU directly.
    EU_FULL_GLB_IC_TO_L2_ENABLED             = 1,   // 1: enabled, IC miss request will go to L2 firstly, and instr
                                                    // may be cached in L2 (defalut value)
} EU_FULL_GLB_IC_TO_L2;
typedef enum
{
    EU_3D_GLB_U_SHARP_CACHED_DISABLED        = 0,   // 0: disabled, register mode. U# are recorded in registers.
                                                    // LS need read fmt to convert data.
    EU_3D_GLB_U_SHARP_CACHED_ENABLED         = 1,   // 1: enabled, U# is cache mode, LS need read fmt from WLS U#
                                                    // cache and then do data conversion.
} EU_3D_GLB_U_SHARP_CACHED;
typedef enum
{
    EU_FE_GLB_CS_ON_OFF                      = 0,   // 0: Disable: The 3D pipe is running 3D task
    EU_FE_GLB_CS_ON_ON                       = 1,   // 1: Enable, The 3D pipe is running CS task
} EU_FE_GLB_CS_ON;
typedef enum
{
    EU_FE_GLB_VS_ON_OFF                      = 0,   // 0: Disable: All 3D stages are OFF, or run CS task
    EU_FE_GLB_VS_ON_ON                       = 1,   // 1: Enable 3D draw, so VS is ON
} EU_FE_GLB_VS_ON;
typedef enum
{
    EU_FE_GLB_TESS_ON_OFF                    = 0,   // 0: Tessellator is off
    EU_FE_GLB_TESS_ON_ON                     = 1,   // 1: HS, DS and TS are all on
} EU_FE_GLB_TESS_ON;
typedef enum
{
    EU_FE_GLB_GS_ON_OFF                      = 0,   // 0: GS is off
    EU_FE_GLB_GS_ON_ON                       = 1,   // 1: GS is on
} EU_FE_GLB_GS_ON;
typedef enum
{
    EU_FE_GLB_FS_LAST_VS                     = 0,   // 0: VS, both Tessellation and GS are OFF
    EU_FE_GLB_FS_LAST_HS                     = 1,   // 1: useless
    EU_FE_GLB_FS_LAST_DS                     = 2,   // 2: DS, Tessellation is ON, GS OFF
    EU_FE_GLB_FS_LAST_GS                     = 3,   // 3: GS, GS is ON
    EU_FE_GLB_FS_LAST_PS                     = 4,   // 4: useless
    EU_FE_GLB_FS_LAST_CS                     = 5,   // 5: useless
} EU_FE_GLB_FS_LAST;
typedef enum
{
    EU_FE_GLB_OUT_RAST_EN_DISABLE            = 0,   // 0: Disable
    EU_FE_GLB_OUT_RAST_EN_ENABLE             = 1,   // 1: Out to TAS Enable, do rast
} EU_FE_GLB_OUT_RAST_EN;
typedef enum
{
    EU_FE_GLB_STO_EN_DISABLED                = 0,   // 0: Disable, default mode
    EU_FE_GLB_STO_EN_ENABLED                 = 1,   // 1: Enable, VS/DS/GS may stream-out, and TAS need assemble
                                                    // the primitives and let VC to output vertices to memory.
} EU_FE_GLB_STO_EN;
typedef enum
{
    EU_FE_GLB_OGL_EN_DX                      = 0,   // 0: MS DirectX
    EU_FE_GLB_OGL_EN_OGL                     = 1,   // 1: OpenGL/OpenES
} EU_FE_GLB_OGL_EN;
typedef enum
{
    EU_FE_GLB_TAS_AB_SIZE_CFG_HOLD_8VTX      = 0,   // 1: AB size of one vtx > 24; can hold 8vtx
    EU_FE_GLB_TAS_AB_SIZE_CFG_HOLD_16VTX     = 1,   // 0: AB size of one vertex <= 24; it can hold 16vtx
} EU_FE_GLB_TAS_AB_SIZE_CFG;
typedef enum
{
    MV_CFG_MV_EN_DISABLE                     = 0,   // 0: Disable, Mv_Num should be 1
    MV_CFG_MV_EN_ENABLE                      = 1,   // 1: Enable, the following MV-related reg are valid.
} MV_CFG_MV_EN;
typedef enum
{
    FS_CFG_AND_V_MASK_DISABLED               = 0,   // 0: Disable
    FS_CFG_AND_V_MASK_ENABLED                = 1,   // 1: Enable, HW ands the sample predicate with the P7 quad
                                                    // based mask, also LS, branch
} FS_CFG_AND_V_MASK;
typedef enum
{
    FS_CFG_RD_MODE_NEAR                      = 0,   // 00: Round toward nearest even
    FS_CFG_RD_MODE_ZERO                      = 1,   // 01: Round to Zero
    FS_CFG_RD_MODE_POS                       = 2,   // 10: Round to Positive Infinity
    FS_CFG_RD_MODE_NEG                       = 3,   // 11: Round to Negative infinity
} FS_CFG_RD_MODE;
typedef enum
{
    FS_CFG_FP_MODE_STD_IEEE                  = 0,   // 00: Standard IEEE-745
    FS_CFG_FP_MODE_NSTD_IEEE                 = 1,   // 01: Non-Standard IEEE-745
    FS_CFG_FP_MODE_MS_ALT                    = 2,   // 10: Microsoft Alt float point
    FS_CFG_FP_MODE_RESERVED                  = 3,   // 11: Reserved
} FS_CFG_FP_MODE;
typedef enum
{
    FS_CFG_DENORM_EN_DISABLED                = 0,   // 0: Disable, denormal number will be flush to 0 for floating
    FS_CFG_DENORM_EN_ENABLED                 = 1,   // 1: Enable, denormal number is kept for floating
} FS_CFG_DENORM_EN;
typedef enum
{
    ANTILOCK_CFG_ANTILOCK_TO_EN_DISABLED     = 0,   // 0: no timeout force kickoff in those constructors
    ANTILOCK_CFG_ANTILOCK_TO_EN_ENABLED      = 1,   // 1: force kickoff threads based on antilock_timeout count
                                                    // set in VS/HS/DS/GS constructors accordingly
} ANTILOCK_CFG_ANTILOCK_TO_EN;
typedef enum
{
    ANTILOCK_CFG_ACCUR_KICKOFF_EN_DISABLED   = 0,   // 0: Disable, no auto deadlock detection in EU-FS
                                                    // constructors
    ANTILOCK_CFG_ACCUR_KICKOFF_EN_ENABLED    = 1,   // 1: Enable, HW checks pending tasks in previous stages and
                                                    // does force kickoff when necessary
} ANTILOCK_CFG_ACCUR_KICKOFF_EN;
typedef enum
{
    VS_CTRL_VID_EN_VID_NONE                  = 0,   // 0: no VID input to VS
    VS_CTRL_VID_EN_VID_AV                    = 1,   // 1: has VID input to VS
} VS_CTRL_VID_EN;
typedef enum
{
    VS_CTRL_IID_EN_IID_NONE                  = 0,   // 0: no IID input to VS
    VS_CTRL_IID_EN_IID_AV                    = 1,   // 1: has IID input to VS
} VS_CTRL_IID_EN;
typedef enum
{
    VS_CTRL_HITTEST_DISABLE_ENABLED          = 0,   // 0: Default. enable TagRAM.Tagram size is 4 lines, each
                                                    // line holds 8 vertices
    VS_CTRL_HITTEST_DISABLE_DISABLED         = 1,   // 1: Disable Targram.
    VS_CTRL_HITTEST_DISABLE_RESERVED         = 2,   // Reserved
} VS_CTRL_HITTEST_DISABLE;
typedef enum
{
    VS_CTRL_SIMD_NUM_1_LANE                  = 0,   // 0: 1 lane per thread
    VS_CTRL_SIMD_NUM_2_LANE                  = 1,   // 1: 2 lanes per thread
    VS_CTRL_SIMD_NUM_4_LANE                  = 2,   // 2: 4 lanes per thread
    VS_CTRL_SIMD_NUM_8_LANE                  = 3,   // 3: 8 lanes per thread
    VS_CTRL_SIMD_NUM_16_LANE                 = 4,   // 4: 16 lanes per thread
    VS_CTRL_SIMD_NUM_32_LANE                 = 5,   // 5: 32 lanes per thread
} VS_CTRL_SIMD_NUM;
typedef enum
{
    HS_CTRL_PID_EN_PID_NONE                  = 0,   // 0: no PID input to HS
    HS_CTRL_PID_EN_PID_AV                    = 1,   // 1: has PID input to HS
} HS_CTRL_PID_EN;
typedef enum
{
    HS_CTRL_SIMD_NUM_1_LANE                  = 0,   // 0: 1 lane per thread
    HS_CTRL_SIMD_NUM_2_LANE                  = 1,   // 1: 2 lanes per thread
    HS_CTRL_SIMD_NUM_4_LANE                  = 2,   // 2: 4 lanes per thread
    HS_CTRL_SIMD_NUM_8_LANE                  = 3,   // 3: 8 lanes per thread
    HS_CTRL_SIMD_NUM_16_LANE                 = 4,   // 4: 16 lanes per thread
    HS_CTRL_SIMD_NUM_32_LANE                 = 5,   // 5: 32 lanes per thread
} HS_CTRL_SIMD_NUM;
typedef enum
{
    HS_CTRL_CPPC_OUT_MODE_BOTH_ON            = 0,   // 0: default, HS thread need output CPs and PCs.
    HS_CTRL_CPPC_OUT_MODE_CP_OFF_PC_ON       = 1,   // 1: HS thread only outputs PC, and CPs are bypassed from VS.
                                                    // Often used in DX11
    HS_CTRL_CPPC_OUT_MODE_BOTH_OFF           = 2,   // 2: HS thread is disabled, CPs are bypassed from VS, and PCs
                                                    // are defined in Tess regs. OpenGL mode.
    HS_CTRL_CPPC_OUT_MODE_RESERVED           = 3,   // Reserved
} HS_CTRL_CPPC_OUT_MODE;
typedef enum
{
    DS_CTRL_DOM_EN_DOM_NONE                  = 0,   // 0: no Domain input to DS
    DS_CTRL_DOM_EN_DOM_AV                    = 1,   // 1: has Domain input to DS
} DS_CTRL_DOM_EN;
typedef enum
{
    DS_CTRL_PID_EN_PID_NONE                  = 0,   // 0: no PID input to DS
    DS_CTRL_PID_EN_PID_AV                    = 1,   // 1: has PID input to DS
} DS_CTRL_PID_EN;
typedef enum
{
    DS_CTRL_HITTEST_DIS_ENABLED              = 0,   // 0: Default. Enable Tagram, whose size is 4 lines, each line
                                                    // holds 8 vertices
    DS_CTRL_HITTEST_DIS_DISABLED             = 1,   // 1: Disable Targram.
    DS_CTRL_HITTEST_DIS_RESERVED             = 2,   // Reserved
} DS_CTRL_HITTEST_DIS;
typedef enum
{
    DS_CTRL_SIMD_NUM_1_LANE                  = 0,   // 0: 1 lane per thread
    DS_CTRL_SIMD_NUM_2_LANE                  = 1,   // 1: 2 lanes per thread
    DS_CTRL_SIMD_NUM_4_LANE                  = 2,   // 2: 4 lanes per thread
    DS_CTRL_SIMD_NUM_8_LANE                  = 3,   // 3: 8 lanes per thread
    DS_CTRL_SIMD_NUM_16_LANE                 = 4,   // 4: 16 lanes per thread
    DS_CTRL_SIMD_NUM_32_LANE                 = 5,   // 5: 32 lanes per thread
} DS_CTRL_SIMD_NUM;
typedef enum
{
    GS_CTRL_PID_EN_PID_NONE                  = 0,   // 0: no PID input to GS
    GS_CTRL_PID_EN_PID_AV                    = 1,   // 1: has PID input to GS
} GS_CTRL_PID_EN;
typedef enum
{
    GS_CTRL_INST_EN_INST_NONE                = 0,   // 0: no Instance input to GS
    GS_CTRL_INST_EN_INST_AV                  = 1,   // 1: has Instance input to GS
} GS_CTRL_INST_EN;
typedef enum
{
    GS_CTRL_SIMD_NUM_1_LANE                  = 0,   // 0: 1 lane per thread
    GS_CTRL_SIMD_NUM_2_LANE                  = 1,   // 1: 2 lanes per thread
    GS_CTRL_SIMD_NUM_4_LANE                  = 2,   // 2: 4 lanes per thread
    GS_CTRL_SIMD_NUM_8_LANE                  = 3,   // 3: 8 lanes per thread
    GS_CTRL_SIMD_NUM_16_LANE                 = 4,   // 4: 16 lanes per thread
    GS_CTRL_SIMD_NUM_32_LANE                 = 5,   // 5: 32 lanes per thread
} GS_CTRL_SIMD_NUM;
typedef enum
{
    GS_OUT_CFG_OUT_PRIM_TYPE_POINTLIST       = 0,   // 000: Point List
    GS_OUT_CFG_OUT_PRIM_TYPE_LINESTRIP       = 1,   // 001: Line Strip
    GS_OUT_CFG_OUT_PRIM_TYPE_TRIANGLESTRIP   = 2,   // 010: Triangle Strip
    GS_OUT_CFG_OUT_PRIM_TYPE_LINESTRIP_ADJ   = 3,   // 011: Line Strip Adjacency
    GS_OUT_CFG_OUT_PRIM_TYPE_TRIANGLESTRIP_ADJ= 4,  // 100: Triangle Strip Adjacency
    GS_OUT_CFG_OUT_PRIM_TYPE_TRIANGLELIST_ADJ= 5,   // 101: Triangle List Adjacency
    GS_OUT_CFG_OUT_PRIM_TYPE_PATCHLIST       = 6,   // 110: Patch list
    GS_OUT_CFG_OUT_PRIM_TYPE_AA_RECT         = 7,   // 111: Unsupported in GS input, Dx12
} GS_OUT_CFG_OUT_PRIM_TYPE;
typedef enum
{
    TS_CTRL_DOMAIN_TRIANGLE                  = 0,   // 00: triangle domain
    TS_CTRL_DOMAIN_QUAD                      = 1,   // 01: quad domain
    TS_CTRL_DOMAIN_ISOLINE                   = 2,   // 10: isoline domain
    TS_CTRL_DOMAIN_RESERVED                  = 3,   // 11: Reserved
} TS_CTRL_DOMAIN;
typedef enum
{
    TS_CTRL_PARTITIONING_INTEGER             = 0,   // 00: integer
    TS_CTRL_PARTITIONING_POW2                = 1,   // 01: pow of 2
    TS_CTRL_PARTITIONING_FRAC_ODD            = 2,   // 10: fractional odd
    TS_CTRL_PARTITIONING_FRAC_EVEN           = 3,   // 11: fractional even
} TS_CTRL_PARTITIONING;
typedef enum
{
    TS_CTRL_TF_REDU_MODE_REV_MIN             = 0,   // 00: min
    TS_CTRL_TF_REDU_MODE_REV_MAX             = 1,   // 01: max
    TS_CTRL_TF_REDU_MODE_REV_AVG             = 2,   // 10: average
    TS_CTRL_TF_REDU_MODE_REV_RESERVED        = 3,   // 11: Reserved
} TS_CTRL_TF_REDU_MODE_REV;
typedef enum
{
    TS_CTRL_DOMAIN_ORIGIN_UPPER_LEFT = 0,   // 0: upper-left origin, DX11/Vulkan
    TS_CTRL_DOMAIN_ORIGIN_LOWER_LEFT = 1,   // 1: lower-left origin, Vulkan/OGL
} TS_CTRL_DOMAIN_ORIGIN;
typedef enum
{
    TS_CTRL_TOPOLOGY_POINT                   = 0,   // 00: point
    TS_CTRL_TOPOLOGY_LINE                    = 1,   // 01: line
    TS_CTRL_TOPOLOGY_TRIANGLE_CW             = 2,   // 10: triangle CW
    TS_CTRL_TOPOLOGY_TRIANGLE_CCW            = 3,   // 11: triangle CCW
} TS_CTRL_TOPOLOGY;
typedef enum
{
    TS_INPUT_MAPPING0_TF0_EN_DISABLED        = 0,   // 0: Disabled
    TS_INPUT_MAPPING0_TF0_EN_ENABLED         = 1,   // 1: Enabled
} TS_INPUT_MAPPING0_TF0_EN;
typedef enum
{
    TS_INPUT_MAPPING0_TF1_EN_DISABLED        = 0,   // 0: Disabled
    TS_INPUT_MAPPING0_TF1_EN_ENABLED         = 1,   // 1: Enabled
} TS_INPUT_MAPPING0_TF1_EN;
typedef enum
{
    TS_INPUT_MAPPING0_TF2_EN_DISABLED        = 0,   // 0: Disabled
    TS_INPUT_MAPPING0_TF2_EN_ENABLED         = 1,   // 1: Enabled
} TS_INPUT_MAPPING0_TF2_EN;
typedef enum
{
    TS_INPUT_MAPPING1_TF3_EN_DISABLED        = 0,   // 0: Disabled
    TS_INPUT_MAPPING1_TF3_EN_ENABLED         = 1,   // 1: Enabled
} TS_INPUT_MAPPING1_TF3_EN;
typedef enum
{
    TS_INPUT_MAPPING1_TF4_EN_DISABLED        = 0,   // 0: Disabled
    TS_INPUT_MAPPING1_TF4_EN_ENABLED         = 1,   // 1: Enabled
} TS_INPUT_MAPPING1_TF4_EN;
typedef enum
{
    TS_INPUT_MAPPING1_TF5_EN_DISABLED        = 0,   // 0: Disabled
    TS_INPUT_MAPPING1_TF5_EN_ENABLED         = 1,   // 1: Enabled
} TS_INPUT_MAPPING1_TF5_EN;
typedef enum
{
    FS_CS_CTRL_THREAD_MODE_SIMD32            = 0,   // 0: only use FP/DP instr
    FS_CS_CTRL_THREAD_MODE_SIMD64            = 1,   // 1: use HP/FP/DP/Mix instr
} FS_CS_CTRL_THREAD_MODE;
typedef enum
{
    FS_CS_CTRL_PATTERN_MODE_FLAT             = 0,   // Flat mode, normal mode. all 3D are flat mode.
    FS_CS_CTRL_PATTERN_MODE_LINEAR           = 1,   // 32x1x1 or 64x1x1 1D-linear pattern
    FS_CS_CTRL_PATTERN_MODE_TILE             = 2,   // 8x4x1 or 8x8x1 2D-tile pattern
    FS_CS_CTRL_PATTERN_MODE_RESERVED         = 3,   // Reserved
} FS_CS_CTRL_PATTERN_MODE;
typedef enum
{
    FS_CS_SM_CFG_SM_EN_OFF                   = 0,   // 0: OFF, no SM used in CS threads
    FS_CS_SM_CFG_SM_EN_ON                    = 1,   // 1: ON, CS uses SM. The CS cannot run with other 3D-FS or other
                                                    // CS task with SM.
} FS_CS_SM_CFG_SM_EN;
typedef enum
{
    FS_OUT_LOC0_OUT_LOC_SEPARATED            = 0,   // 0: Default, output data are at separated space, for Mv. If
                                                    // Mv_En is OFF, the location is always at separated space.
    FS_OUT_LOC0_OUT_LOC_SHARED               = 1,   // 1: Mv_En is ON, the output data are at shared space, used by
                                                    // all vertices of multi-views.
} FS_OUT_LOC0_OUT_LOC;
typedef enum
{
    FS_OUT_LOC1_OUT_LOC_SEPARATED            = 0,   // 0: Default, output data are at separated space, for Mv. If
                                                    // Mv_En is OFF, the location is always at separated space.
    FS_OUT_LOC1_OUT_LOC_SHARED               = 1,   // 1: Mv_En is ON, the output data are at shared space, used by
                                                    // all vertices of multi-views.
} FS_OUT_LOC1_OUT_LOC;
typedef enum
{
    FS_PM_CFG_PM_EN_DISABLED                 = 0,   // 0: Disable. no PM space, for PM instr, treat them as
                                                    // out-of-range.
    FS_PM_CFG_PM_EN_ENABLED                  = 1,   // 1: Enable, FS/CS use PM.
} FS_PM_CFG_PM_EN;
typedef enum
{
    FS_PM_CFG_TH_PM_SIZE_8KDW                = 0,   // 8K dword, can hold 256# for Simd32, or 128# for Simd64
                                                    // threads
    FS_PM_CFG_TH_PM_SIZE_16KDW               = 1,   // 512# for Simd32; 256# for Simd64
    FS_PM_CFG_TH_PM_SIZE_32KDW               = 2,   // 1024# for Simd32; 512# for Simd64
    FS_PM_CFG_TH_PM_SIZE_64KDW               = 3,   // 2048# for Simd32; 1024# for Simd64
    FS_PM_CFG_TH_PM_SIZE_128KDW              = 4,   // 4096# for Simd32; 2048# for Simd64
    FS_PM_CFG_TH_PM_SIZE_256KDW              = 5,   // 8192# for Simd32; 4096# for Simd64
    FS_PM_CFG_TH_PM_SIZE_512KDW              = 6,   // 16384# for simd32 thread; 8192 for simd64 thread.
    FS_PM_CFG_TH_PM_SIZE_1024KDW             = 7,   // 32k# for simd32 thread; 16k# form simd64 thread.
    FS_PM_CFG_TH_PM_SIZE_1040KDW             = 8,   // 16640# for simd64 thread
} FS_PM_CFG_TH_PM_SIZE;
typedef enum
{
    FS_U_ENABLE_U0_EN_DISABLE                = 0,   // 0: U# disable, discard store and return 0 values for load
    FS_U_ENABLE_U0_EN_ENABLE                 = 1,   // 1: U# enable, the following U# fmt and layout are valid
} FS_U_ENABLE_U0_EN;
typedef enum
{
    FS_U_FMT_U0_DATA_FMT_FP32                = 0,   // 000: Floating-point 32-bits
    FS_U_FMT_U0_DATA_FMT_FP16                = 1,   // 001: Floating-point 16-bit
    FS_U_FMT_U0_DATA_FMT_SINT32              = 2,   // 010: Signed Integer 32-bit
    FS_U_FMT_U0_DATA_FMT_SINT16              = 3,   // 011: Signed Integer 16-bit
    FS_U_FMT_U0_DATA_FMT_UINT32              = 4,   // 100: Unsigned integer 32-bit
    FS_U_FMT_U0_DATA_FMT_UINT16              = 5,   // 101: Unsigned integer 16-bit
    FS_U_FMT_U0_DATA_FMT_UNORM24             = 6,   // 110: Unorm 24-bit
    FS_U_FMT_U0_DATA_FMT_UNORM16             = 7,   // 111: Unorm 16-bit
    FS_U_FMT_U0_DATA_FMT_UNORM10             = 8,   // 1000: Unorm 10-bit
    FS_U_FMT_U0_DATA_FMT_UNORM8              = 9,   // 1001: Unorm 8-bit
    FS_U_FMT_U0_DATA_FMT_SINT8               = 10,  // 1010: Signed Integer 8-bit
    FS_U_FMT_U0_DATA_FMT_UINT8               = 11,  // 1011: Unsigned integer 8-bit
    FS_U_FMT_U0_DATA_FMT_SNORM8              = 12,  // 1100: Snorm 8-bit
    FS_U_FMT_U0_DATA_FMT_SNORM16             = 13,  // 1100: Snorm 16-bit
    FS_U_FMT_U0_DATA_FMT_UINT64              = 14,  // 1101: unsigned int-64bits, only for reduction
    FS_U_FMT_U0_DATA_FMT_RESERVED            = 15,  // 1111: Reserved
} FS_U_FMT_U0_DATA_FMT;
typedef enum
{
    FS_U_LAYOUT_U0_LAYOUT_VERTICAL           = 0,   // 0: Untyped vertical buffer, should be 32bits
    FS_U_LAYOUT_U0_LAYOUT_HORIZONTAL         = 1,   // 1: Untyped/Typed horizontal buffer.
} FS_U_LAYOUT_U0_LAYOUT;
typedef enum
{
    DBG_CFG_EXEC_EN_DISABLED                 = 0,   // 0: Disable, THC cleans this flag when it reaches one of the
                                                    // breakpoints, and waits until the debugger sets this flag
                                                    // to resume execution
    DBG_CFG_EXEC_EN_ENABLED                  = 1,   // 1: Enable, the debugger sets this flag to notify the EU to
                                                    // start/resume execution
} DBG_CFG_EXEC_EN;
typedef enum
{
    DBG_CFG_TH_MODE_SINGLE                   = 0,   // 0: Single, the EU executes one thread only, specified by
                                                    // Dbg_Th_Id
    DBG_CFG_TH_MODE_ALL                      = 1,   // 1: All, the EU executes all available threads
} DBG_CFG_TH_MODE;
typedef enum
{
    DBG_CFG_INT_EN_DISABLED                  = 0,   // 0: Disable, the EU will NOT call the interrupt routine when
                                                    // it hits the breakpoints
    DBG_CFG_INT_EN_ENABLED                   = 1,   // 1: Enable, the EU will call the interrupt routine when it
                                                    // hits the breakpoints. Only Hit_Th_Id thread need run
                                                    // interrupt shader.
} DBG_CFG_INT_EN;
typedef enum
{
    DBG_CFG_RES_EN_DISABLED                  = 0,   // 0: Disable, the EU will NOT call the resume routine when it
                                                    // resumes execution.
    DBG_CFG_RES_EN_ENABLED                   = 1,   // 1: Enable, the EU will call the resume routine when it
                                                    // resumes execution. only Hit_Th_Id thread need run resume
                                                    // shader
} DBG_CFG_RES_EN;
typedef enum
{
    DBG_BP_STAT_BP_HIT_DISABLED              = 0,   // 0: Disable, the EU hasn't hit the breakpoints. Driver need
                                                    // clear this flag before shader continues to run.
    DBG_BP_STAT_BP_HIT_ENABLED               = 1,   // 1: Enable, when the EU hits one of the breakpoints, it will
                                                    // suspend all threads, and then set this flag. The hit PC and
                                                    // threadID are also recorded.
} DBG_BP_STAT_BP_HIT;
typedef enum
{
    DBG_BP_PC_BP_VALID_DISABLED              = 0,   // 0: Disable, the current breakpoint slot is disabled
    DBG_BP_PC_BP_VALID_ENABLED               = 1,   // 1: Enable, the current breakpoint slot is enabled
} DBG_BP_PC_BP_VALID;
typedef enum
{
    DBG_BP_PC_SHADER_VS                      = 0,
    DBG_BP_PC_SHADER_HS                      = 1,
    DBG_BP_PC_SHADER_DS                      = 2,
    DBG_BP_PC_SHADER_GS                      = 3,
    DBG_BP_PC_SHADER_PS                      = 4,
    DBG_BP_PC_SHADER_CS                      = 5,
} DBG_BP_PC_SHADER;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////            EU_FS Block (Block ID = 3) Register Definitions                           ///////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Shader_Dbg_En             : 1;   // CS/3D shader debugging mode enable
        unsigned int Thd_Slot_Size             : 2;   // Allocate thread slot size in one set, to let driver use less
                                                      // PM size
        unsigned int Uav_En                    : 1;   // There are LS UAV opeartion in 3D/CS shader. If true, DOUT
                                                      // need allocate UAV buffer.
        unsigned int Uav_Continue_Chk          : 1;   // UAV LS continuous check function, default is Enabled
        unsigned int Uav_Buf_Size              : 4;   // UAV buffer size by bit-width specification, shared with
                                                      // 3D-PS, 0 ~ 192 buffer lines in total,  LS may use 128 lines
                                                      // buffer in max for all 3D/CS LS requests. (global reg,
                                                      // should be sync by all process)
        unsigned int U_3d_Base                 : 5;   // (0~16), 3D U# base, total 128U# per slice, served for one 3D
                                                      // and two CS processes. 8U# in one group, (0~8*Base) for
                                                      // CS(L/H), (8*Base~127) for 3D. From low to high, the order
                                                      // is CS_L, CS_H, 3D_FS, 3D_PS
        unsigned int Cb_3d_Base                : 6;   // (0~32) 3D CB base, total 8Kbytes (512x 128bits) per PE,
                                                      // served for one 3D and two CS processes. 2kbits (16E) in one
                                                      // group, (0~16*Base) for CS(L/H), (16*Base ~ 511) for 3D.
                                                      // From low to high, the order is CS_L, CS_H, 3D_FS, 3D_PS
        unsigned int Ic_To_L2                  : 1;   // IC miss request will go to L2 firstly, and instr may be
                                                      // cached in L2.
        unsigned int Reserved                  : 11;  // Reserved
    } reg;
} Reg_Eu_Full_Glb;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int U_Ps_Base                 : 5;   // (0~16), PS U# base, total 128U# per slice, served for one 3D
                                                      // and two CS processes. 8U# in one group,
        unsigned int Cb_Ps_Base                : 6;   // (0~16) PS CB base, total 8Kbytes (512x 128bits) per PE,
                                                      // served for one 3D and two CS processes. 2kbits (16E-dw) in
                                                      // one group.
        unsigned int U_Sharp_Cached            : 1;   // UAV U# cached or register mode for 3D.
        unsigned int Reserved                  : 20;  // Reserved
    } reg;
} Reg_Eu_3d_Glb;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Cs_On                     : 1;   // 3D-CS Enable
        unsigned int Vs_On                     : 1;   // 3D draw Enable
        unsigned int Tess_On                   : 1;   // If Tessellation is on, HS, DS and TS will be on alltogether
        unsigned int Gs_On                     : 1;   // Geometry Shader
        unsigned int Fs_Last                   : 3;   // Which shader type is the last shader stage  (TASASM need
                                                      // read it)
        unsigned int Out_Rast_En               : 1;   // Enable outputs to Tas, used for 3D rendering
        unsigned int Sto_En                    : 1;   // Stream Out enable
        unsigned int Rast_Sel                  : 2;   // Rast Stream Select, the stream # bound to the
                                                      // Rasterization pipe (maybe sto0~3)
        unsigned int Ogl_En                    : 1;   // API selection, used for API specific handling, strip
                                                      // winding direction, leading vertex, etc
        unsigned int Tas_Ab_Size_Cfg           : 1;   // TASASM use it to do attribute buffer hit/miss test for TAS
                                                      // AB buffer. 8 or 16vtx based on each vtx AB size.
        unsigned int Reserved                  : 19;  // Reserved
    } reg;
} Reg_Eu_Fe_Glb;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Mv_En                     : 1;   // Multiview Enabled or not
        unsigned int Mv_Mask                   : 8;   // If Mv_En is enabled, 1~8 Multi-view cases. 0 value is
                                                      // illegal.
        unsigned int Mv_Shared_Size            : 6;   // MV VS/DS output shared attr, used by all MV vertices. Value
                                                      // is 0~32 4-component elements.
        unsigned int Reserved                  : 17;  // Reserved
    } reg;
} Reg_Mv_Cfg;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Vs_Vc_Size                : 16;  // (0~1024), number of VC lines allocated for VS output
                                                      // elements
        unsigned int Hs_Vc_Size                : 16;  // (0~1024), number of VC lines allocated for HS output
                                                      // elements, including HS control points and patch consts
    } reg;
} Reg_Vc_Size_Cfg0;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Ds_Vc_Size                : 16;  // (0~1024), number of VC lines allocated for VS output
                                                      // elements
        unsigned int Gs_Vc_Size                : 16;  // (0~1024), number of VC lines allocated for VS output
                                                      // elements
    } reg;
} Reg_Vc_Size_Cfg1;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Vs_Vc_Base                : 16;  // (0~1024), VC base line for output elements of VS, 4lines
                                                      // aligned
        unsigned int Hs_Vc_Base                : 16;  // (0~1024), VC base line for output elements of HS,  4lines
                                                      // aligned
    } reg;
} Reg_Vc_Base_Cfg0;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Ds_Vc_Base                : 16;  // (0~1024), VC base line for output elements of DS, 4lines
                                                      // aligned
        unsigned int Gs_Vc_Base                : 16;  // (0~1024), VC base line for output elements of GS,  4lines
                                                      // aligned
    } reg;
} Reg_Vc_Base_Cfg1;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int And_V_Mask                : 1;   // And sample predicate with P7 based mask in FS
        unsigned int Rd_Mode                   : 2;   // EU floating point rounding mode
        unsigned int Fp_Mode                   : 2;   // EU floating point operation mode
        unsigned int Denorm_En                 : 1;   // Whether support denomal computation
        unsigned int Reserved                  : 26;  // Reserved
    } reg;
} Reg_Fs_Cfg;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Vcs_Max_Threads           : 8;   // Maximum number of total VS/3D_CS threads kickoff in the
                                                      // EU. If VS, Value 0~32 for 0~32 threads. If CS, it means 0~64.
                                                      // Default value is 0x40 for 64 threads.
        unsigned int Hs_Max_Threads            : 8;   // Maximum number (0~16) of total HS threads kickoff.
                                                      // Default value 0x10 as 16 threads.
        unsigned int Ds_Max_Threads            : 8;   // Maximum number (0~64) of total DS threads kickoff.
                                                      // Default value 0x40 as 64 threads.
        unsigned int Gs_Max_Threads            : 8;   // Maximum number (0~16) of total GS threads kickoff.
                                                      // Default value 0x10 as 16 threads.
    } reg;
} Reg_Max_Threads_Cfg;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Vs_Antilock_Timeout       : 4;   // VS anti-lock force kickoff timeout count
                                                      //  bit[3]: 1=16384 cycles
                                                      //  bit[2]: 1=8192 cycles
                                                      //  bit[1:0]: 0~3=1 to 4 x 1024 cycles
                                                      //  bit[3:0] when set to 1111, it has special meaning 512
                                                      // cycles
        unsigned int Hs_Antilock_Timeout       : 4;   // HS anti-lock force kickoff timeout count
                                                      //  bit[3]: 1=16384 cycles
                                                      //  bit[2]: 1=8192 cycles
                                                      //  bit[1:0]: 0~3=1 to 4 x 1024 cycles
                                                      //  bit[3:0] when set to 1111, it has special meaning 512
                                                      // cycles
        unsigned int Ds_Antilock_Timeout       : 4;   // DS anti-lock force kickoff timeout count
                                                      //  bit[3]: 1=16384 cycles
                                                      //  bit[2]: 1=8192 cycles
                                                      //  bit[1:0]: 0~3=1 to 4 x 1024 cycles
                                                      //  bit[3:0] when set to 1111, it has special meaning 512
                                                      // cycles
        unsigned int Gs_Antilock_Timeout       : 4;   // GS anti-lock force kickoff timeout count
                                                      //  bit[3]: 1=16384 cycles
                                                      //  bit[2]: 1=8192 cycles
                                                      //  bit[1:0]: 0~3=1 to 4 x 1024 cycles
                                                      //  bit[3:0] when set to 1111, it has special meaning 512
                                                      // cycles
        unsigned int Antilock_To_En            : 1;   // VS/HS/DS/GS anti-lock timeout enable. To match dump with
                                                      // HW, the bit should be 0
        unsigned int Accur_Kickoff_En          : 1;   // EU-FS accurate force kickoff. To match dump with HW, the
                                                      // bit should be 1
        unsigned int Reserved                  : 14;  // Reserved
    } reg;
} Reg_Antilock_Cfg;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Crf_Size                  : 6;   // Total Common Register File (CRF) space. Value is 1~52
                                                      // 4-component elements, including temp, input, output
                                                      // registers.
        unsigned int In_Size                   : 6;   // VS Input Size. Value is 1~48 4-component elements. Zero
                                                      // In_Size is illegal.
        unsigned int Out_Size                  : 6;   // VS output size for one vertex. Value is 1~48/96
                                                      // 4-component elements, zero size is illegal.
        unsigned int Vid_En                    : 1;   // VertexID available as VS inputs, when it's available the
                                                      // hardware needs to store it in the thread CRF address
                                                      // R(In_Size << 2).
        unsigned int Iid_En                    : 1;   // InstanceID available as VS inputs, when it's available
                                                      // the hardware needs to store it in the thread CRF address
                                                      // R(In_Size << 2 + 1).
        unsigned int Hittest_Disable           : 2;   // The active hit/miss test lines in Tagram. To avoid
                                                      // dead-lock or bug-issue.
        unsigned int Simd_Num                  : 3;   // Vertex Shader Lane Number, the actual number of lanes
        unsigned int Reserved                  : 7;   // Reserved
    } reg;
} Reg_Vs_Ctrl;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Crf_Size                  : 6;   // Total Common Register File (CRF) space. Value is 1~52
                                                      // 4-component elements, including temp, input, output
                                                      // registers.
        unsigned int Pid_En                    : 1;   // System Generated Values (for HS, PID) available as HS
                                                      // inputs, when it's available the hardware needs to store it
                                                      // into SGV buffer for HS threads to read.
        unsigned int Simd_Num                  : 3;   // Hull Shader Lane Number, the actual number of lanes.
        unsigned int Cppc_Out_Mode             : 2;   // HS thread outputs CP & PC mode.
        unsigned int Reserved                  : 20;  // Reserved
    } reg;
} Reg_Hs_Ctrl;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Input_Cp_Num              : 6;   // Number of input control points received by HS Control
                                                      // Point Phase. Value is 1~32 4-component vectors.
        unsigned int Input_Cp_Size             : 6;   // Vertex size of each input control point received by HS
                                                      // Control Point Phase. Value is 1~48 4-component vectors.
        unsigned int Reserved                  : 20;  // Reserved
    } reg;
} Reg_Hs_In_Cfg;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Output_Cp_Num             : 6;   // Number of output control points generated by HS Control
                                                      // Point Phase. Value is 0~32 4-component vectors.
        unsigned int Output_Cp_Size            : 6;   // Vertex size of each output control point generated by HS
                                                      // Control Point Phase. Value is 0~48 4-component vectors.
        unsigned int Output_Pc_Size            : 6;   // Total size of HS Patch Constant Fork and Join Phase. Value
                                                      // is 1~40 4-component vectors.
        unsigned int Out_Patch_Size            : 11;  // HS output patch size allocated in VC for each lane.
                                                      // Folding2 mode (each line is 8E for two lanes data). maximun
                                                      // to 266 VC lines per lane.
        unsigned int Reserved                  : 3;   // Reserved
    } reg;
} Reg_Hs_Out_Cfg;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Crf_Size                  : 6;   // Total Common Register File (CRF) space. Value is 1~52
                                                      // 4-component elements, including temp, input, output
                                                      // registers.
        unsigned int Out_Size                  : 6;   // DS output size  for one vertex. Value is 1~48/96
                                                      // 4-component vectors, zero size is illegal.
        unsigned int Dom_En                    : 1;   // Domain Location SGV available as DS inputs, when it's
                                                      // available the hardware needs to store it into SGV buffer
                                                      // for DS threads to read
        unsigned int Pid_En                    : 1;   // Primitive ID (patch based) available as DS inputs When
                                                      // it's available DSTC needs to store it into SGV buffer for DS
                                                      // threads to read.
        unsigned int Hittest_Dis               : 2;   // The active hit/miss test lines in Tagram. To avoid
                                                      // dead-lock or bug-issue.
        unsigned int Simd_Num                  : 3;   // Domain Shader Lane Number, the actual number of lanes .
        unsigned int Reserved                  : 13;  // Reserved
    } reg;
} Reg_Ds_Ctrl;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Input_Cp_Num              : 6;   // Number of input control points generated by HS Control
                                                      // Point Phase. Value is 1~32 4-component vectors.
        unsigned int Input_Cp_Size             : 6;   // Vertex size of each input control point generated by HS
                                                      // Control Point Phase. Value is 1~48 4-component vectors.
        unsigned int Input_Pc_Size             : 6;   // Total size of HS Patch Constant Fork and Join Phase. Value
                                                      // is 1~40 4-component vectors.
        unsigned int Reserved                  : 14;  // Reserved
    } reg;
} Reg_Ds_In_Cfg;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Crf_Size                  : 6;   // Total Common Register File (CRF) space. Value is 1~52
                                                      // 4-component elements, including temp, input, output
                                                      // registers.
        unsigned int Pid_En                    : 1;   // Primitive ID (patch based) available as GS inputs,  when
                                                      // it's available GSTC needs to store it into SGV buffer for GS
                                                      // threads to read.
        unsigned int Inst_En                   : 1;   // Instance ID available as GS inputs,  when it's available
                                                      // GSTC needs to store it into SGV buffer for GS threads to
                                                      // read.
        unsigned int Instance_Cnt              : 6;   // GS instance count 0 ~ 32, when it's set to 0, GS instancing is
                                                      // disabled.  When GS instancing is enabled, the EU packs
                                                      // instances across lanes and stores the GS instance ID in the
                                                      // SGV buffer if sgv_en is on.
        unsigned int Simd_Num                  : 3;   // Geometry Shader Lane Number, the actual number of lanes.
        unsigned int Reserved                  : 15;  // Reserved
    } reg;
} Reg_Gs_Ctrl;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int In_Size                   : 6;   // GS input element size of one vertex. Value is 0~48
                                                      // 4-component vectors.
        unsigned int In_Vtx_Num                : 6;   // GS input vertex number, maybe 1(point), 2(line),
                                                      // 3(triangle), 4(line_adj), 6(tri_adj), or 1~32(patch)
        unsigned int Reserved                  : 20;  // Reserved
    } reg;
} Reg_Gs_In_Cfg;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Out_Size                  : 6;   // GS output size for one vertex.. Value is 1~48 4-component
                                                      // element.
        unsigned int Out_Vtx_Num               : 11;  // GS max output vertex number, maximum to 1024V per lane. >
                                                      // 512v, only simd1, at least 1GS thread; <= 512v, at least 2GS
                                                      // threads.
        unsigned int Out_Prim_Size             : 9;   // GS output primitive size allocated in VC for each lane.
                                                      // maximun to 256 VC lines per lane. (GS outputs 1024C in
                                                      // total, one line is 8E for two lanes, folding2)
        unsigned int Out_Prim_Type             : 4;   // GS Output Primitive Type, only support point list/line
                                                      // strip/triangle strip
        unsigned int Reserved                  : 2;   // Reserved
    } reg;
} Reg_Gs_Out_Cfg;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Vtx_Split_Cnt             : 6;   // Vertex split threshold count (0~63).
        unsigned int Domain                    : 2;   // Tessellation Domain
        unsigned int Partitioning              : 2;   // Tessellation Partitioning
        unsigned int Tf_Redu_Mode_Rev          : 2;   // Tessellation Factor Reduction.  (Unused now)
        unsigned int Domain_Origin             : 1;   //  Domain origin of tessellation coordinate
        unsigned int Topology                  : 2;   // Topology
        unsigned int Reserved                  : 17;  // Reserved
    } reg;
} Reg_Ts_Ctrl;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Tf0_Addr                  : 6;   // HS Patch Constant Phase output element # (O#) for TS input
                                                      // TessFactor0
        unsigned int Tf0_Comp_Addr             : 2;   // HS output element component selection for TS input
                                                      // TessFactor0
        unsigned int Tf0_En                    : 1;   // TS input TessFactor0 enable
        unsigned int Tf1_Addr                  : 6;   // HS Patch Constant Phase output element # (O#) for TS input
                                                      // TessFactor1
        unsigned int Tf1_Comp_Addr             : 2;   // HS output element component selection for TS input
                                                      // TessFactor1
        unsigned int Tf1_En                    : 1;   // TS input TessFactor1 enable
        unsigned int Tf2_Addr                  : 6;   // HS Patch Constant Phase output element # (O#) for TS input
                                                      // TessFactor2
        unsigned int Tf2_Comp_Addr             : 2;   // HS output element component selection for TS input
                                                      // TessFactor2
        unsigned int Tf2_En                    : 1;   // TS input TessFactor2 enable
        unsigned int Reserved                  : 5;   // reserved
    } reg;
} Reg_Ts_Input_Mapping0;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Tf3_Addr                  : 6;   // HS Patch Constant Phase output element # (O#) for TS input
                                                      // TessFactor3
        unsigned int Tf3_Comp_Addr             : 2;   // HS output element component selection for TS input
                                                      // TessFactor3
        unsigned int Tf3_En                    : 1;   // TS input TessFactor3 enable
        unsigned int Tf4_Addr                  : 6;   // HS Patch Constant Phase output element # (O#) for TS input
                                                      // TessFactor4
        unsigned int Tf4_Comp_Addr             : 2;   // HS output element component selection for TS input
                                                      // TessFactor4
        unsigned int Tf4_En                    : 1;   // TS input TessFactor4 enable
        unsigned int Tf5_Addr                  : 6;   // HS Patch Constant Phase output element # (O#) for TS input
                                                      // TessFactor5
        unsigned int Tf5_Comp_Addr             : 2;   // HS output element component selection for TS input
                                                      // TessFactor5
        unsigned int Tf5_En                    : 1;   // TS input TessFactor5 enable
        unsigned int Reserved                  : 5;   // reserved
    } reg;
} Reg_Ts_Input_Mapping1;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Factor                    : 32;  // Tessellator Factors defined when HS thread is OFF (0x2
                                                      // CPPC_BOTH_OFF) in CPPC_Out_Mode register.
    } reg;
} Reg_Ts_Factors_Const;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Crf_Size                  : 6;   // Total FP Common Register File (FP-CRF) space. Value is
                                                      // 1~52 4-component elements, including temp, input,
                                                      // output registers.
        unsigned int Reserved0                 : 6;   // And_V_Mask, RD_Mode, FP_Mode, Denorm_En are defined in
                                                      // FP_Cfg
        unsigned int Thread_Mode               : 1;   // CS initial execution mode, this affects how the CSTC
                                                      // allocate thread IDs into the CRF and thread size
        unsigned int Pattern_Mode              : 3;   // CS kick-off thread with pattern mode.
        unsigned int Th_Num_In_Group           : 6;   // Total number (1~32) of EU hardware threads for each Thread
                                                      // Group. in one PE.  16 threads for SIMD64 or 32 threads for
                                                      // SIMD32
        unsigned int Reserved                  : 10;  // Reserved
    } reg;
} Reg_Fs_Cs_Ctrl;

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
} Reg_Fs_Cs_Sm_Cfg;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Out_Loc                   : 32;  // FS output  location, may be at spearated(0x0) or
                                                      // shared(0x1), 48E in total. Shared space only be used if
                                                      // Mv_En is ON.
    } reg;
} Reg_Fs_Out_Loc0;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Out_Loc                   : 16;  // FS output  location, may be at separated(0x0) or
                                                      // shared(0x1), 48E in total. Shared space only be used if
                                                      // Mv_En is ON.
        unsigned int Reserved                  : 16;  // Reserved
    } reg;
} Reg_Fs_Out_Loc1;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int O0_Addr                   : 6;   // FS output O0 address in VC. 6bits per O# addr, 4 O# in one
                                                      // dword. 12dwords to specify 48elements in total. If MV is
                                                      // ON, the out addr may be at shared location.
        unsigned int O1_Addr                   : 6;   // FS output O1 address in VC
        unsigned int O2_Addr                   : 6;   // FS output O2 address in VC
        unsigned int O3_Addr                   : 6;   // FS output O3 address in VC
        unsigned int Reserved                  : 8;   // Reserved
    } reg;
} Reg_Fs_Out_Mapping;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int O0_Comp_Id                : 2;   // FS output O0 component index, 2bits comp id per elements.
                                                      // One dword includes 16 comp_id, 3dowrds in total to specify
                                                      // 48elements.
        unsigned int O1_Comp_Id                : 2;   // FS output O1 component index
        unsigned int O2_Comp_Id                : 2;   // FS output O2 component index
        unsigned int O3_Comp_Id                : 2;   // FS output O3 component index
        unsigned int O4_Comp_Id                : 2;   // FS output O4 component index
        unsigned int O5_Comp_Id                : 2;   // FS output O5 component index
        unsigned int O6_Comp_Id                : 2;   // FS output O6 component index
        unsigned int O7_Comp_Id                : 2;   // FS output O7 component index
        unsigned int O8_Comp_Id                : 2;   // FS output O8 component index
        unsigned int O9_Comp_Id                : 2;   // FS output O9 component index
        unsigned int O10_Comp_Id               : 2;   // FS output O10 component index
        unsigned int O11_Comp_Id               : 2;   // FS output O11 component index
        unsigned int O12_Comp_Id               : 2;   // FS output O12 component index
        unsigned int O13_Comp_Id               : 2;   // FS output O13 component index
        unsigned int O14_Comp_Id               : 2;   // FS output O14 component index
        unsigned int O15_Comp_Id               : 2;   // FS output O15 component index
    } reg;
} Reg_Fs_Out_Comp;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int O0_Mask                   : 4;   // FS output O0 mask. One element requires 4bits to specify
                                                      // 4comp, and 6dwords to special 48E in total.
        unsigned int O1_Mask                   : 4;   // FS output O1 mask
        unsigned int O2_Mask                   : 4;   // FS output O2 mask
        unsigned int O3_Mask                   : 4;   // FS output O3 mask
        unsigned int O4_Mask                   : 4;   // FS output O4 mask
        unsigned int O5_Mask                   : 4;   // FS output O5 mask
        unsigned int O6_Mask                   : 4;   // FS output O6 mask
        unsigned int O7_Mask                   : 4;   // FS output O7 mask
    } reg;
} Reg_Fs_Out_Mask;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Pm_En                     : 1;   // Wheter FS/CS uses PM.
        unsigned int Th_Pm_Size                : 4;   // Size of thread external storage in memory/L2 including
                                                      // call stacks, CRF spill, x# buffers, etc. IF FS and PS use the
                                                      // same UAV buffer, it should use the same PM_Size
        unsigned int Reserved                  : 27;  // Reserved
    } reg;
} Reg_Fs_Pm_Cfg;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Pm_U_Id                   : 32;  // FS private memory U_sharp id, to load/store private
                                                      // memory.  LS unit will convert PM into Untyped LD/ST. In reg
                                                      // mode, it should < 64, or else regard it as out-of-range; in
                                                      // cache mode, it is the offset in cache.
    } reg;
} Reg_Fs_Pm_Id;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Vcs_Idx_Ele_Range         : 14;  // Range of VS/CS thread external storage used by OCL Local
                                                      // Memory or x#s in memory. Note the compiler used external
                                                      // thread storage i.e. call stacks, etc. is not included, it
                                                      // should be placed in the upper memory. Element based
        unsigned int Gs_Idx_Ele_Range          : 14;  // Range of GS thread external storage used by private
                                                      // memory, or CRF spill. Element based
        unsigned int Reserved                  : 4;   // Reserved
    } reg;
} Reg_Vgs_Pm_Range;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Hs_Idx_Ele_Range          : 14;  // Range of HS thread external storage used by private
                                                      // memory, or CRF spill. Element based
        unsigned int Ds_Idx_Ele_Range          : 14;  // Range of DS thread external storage used by private
                                                      // memory, or CRF spill. Element based
        unsigned int Reserved                  : 4;   // Reserved
    } reg;
} Reg_Hds_Pm_Range;

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
} Reg_Fs_U_Enable;

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
} Reg_Fs_U_Fmt;

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
} Reg_Fs_U_Layout;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Vcs_U_Base                : 8;   // Base address of VS/CS U# (0~127). only used in Reg_U# mode.
                                                      // 128U# intotal. Relative Address of U_3D_Base
        unsigned int Vcs_U_Range               : 8;   // Address range of  U# (0~64). Actual U# defined by driver to
                                                      // do U# indexing.
        unsigned int Reserved                  : 16;  // Reserved
    } reg;
} Reg_Vcs_U_Cfg;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Hs_U_Base                 : 8;   // Base address of HS U# (0~127). only used in Reg_U# mode.
                                                      // 128U# intotal. Relative Address of U_3D_Base
        unsigned int Hs_U_Range                : 8;   // Address range of  U# (0~64). Actual U# defined by driver to
                                                      // do U# indexing.
        unsigned int Reserved                  : 16;  // Reserved
    } reg;
} Reg_Hs_U_Cfg;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Ds_U_Base                 : 8;   // Base address of DS U# (0~127). only used in Reg_U# mode.
                                                      // 128U# intotal. Relative Address of U_3D_Base
        unsigned int Ds_U_Range                : 8;   // Address range of  U# (0~64). Actual U# defined by driver to
                                                      // do U# indexing.
        unsigned int Reserved                  : 16;  // Reserved
    } reg;
} Reg_Ds_U_Cfg;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Gs_U_Base                 : 8;   // Base address of GS U# (0~127). only used in Reg_U# mode.
                                                      // 128U# intotal. Relative Address of U_3D_Base
        unsigned int Gs_U_Range                : 8;   // Address range of  U# (0~64). Actual U# defined by driver to
                                                      // do U# indexing.
        unsigned int Reserved                  : 16;  // Reserved
    } reg;
} Reg_Gs_U_Cfg;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Address_Low               : 32;  //  Vertex Shader Instruction Base Address low32bits. Total
                                                      // 40bits
    } reg;
} Reg_Vcs_Instr0;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Address_High              : 8;   //  Vertex/Compute Shader Instruction Base Address
                                                      // high8bits. Total 40bits
        unsigned int Reserved                  : 24;  // Reserved
    } reg;
} Reg_Vcs_Instr1;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Ins_Range                 : 16;  // Instruction range of VS/CS shader codes (0 ~ (16K-1)).
                                                      // Each instr occupies 4Dw (128bits)
        unsigned int Reserved                  : 16;  // Reserved
    } reg;
} Reg_Vcs_Instr_Range;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Address_Low               : 32;  //  Hull Shader Instruction Base Address low32bits. In total
                                                      // 40bits
    } reg;
} Reg_Hs_Instr0;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Address_High              : 8;   //  Hull Shader Instruction Base Address high8bits. In total
                                                      // 40bits
        unsigned int Reserved                  : 24;  // Reserved
    } reg;
} Reg_Hs_Instr1;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Ins_Range                 : 16;  // Instruction range of HS shader codes (0 ~ (16K-1)). Each
                                                      // instr occupies 4Dw (128bits)
        unsigned int Reserved                  : 16;  // Reserved
    } reg;
} Reg_Hs_Instr_Range;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Address_Low               : 32;  //  Domain Shader Instruction Base Address low32bits. In
                                                      // total 40bits
    } reg;
} Reg_Ds_Instr0;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Address_High              : 8;   //  Domain Shader Instruction Base Address high8bits. In
                                                      // total 40bits
        unsigned int Reserved                  : 24;  // Reserved
    } reg;
} Reg_Ds_Instr1;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Ins_Range                 : 16;  // Instruction range of DS shader codes (0 ~ (16K-1)). Each
                                                      // instr occupies 4Dw (128bits)
        unsigned int Reserved                  : 16;  // Reserved
    } reg;
} Reg_Ds_Instr_Range;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Address_Low               : 32;  //  Geometry Shader Instruction Base Address low32bits. In
                                                      // total 40bits
    } reg;
} Reg_Gs_Instr0;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Address_High              : 8;   //  Geometry Shader Instruction Base Address high8bits. In
                                                      // total 40bits
        unsigned int Reserved                  : 24;  // Reserved
    } reg;
} Reg_Gs_Instr1;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Ins_Range                 : 16;  // Instruction range of GS shader codes (0 ~ (16K-1)). Each
                                                      // instr occupies 4Dw (128bits)
        unsigned int Reserved                  : 16;  // Reserved
    } reg;
} Reg_Gs_Instr_Range;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Vcs_Cb_Base               : 9;   // Base address of VS/CS const data in CB (0 ~ 511), 4Dwrods
                                                      // element aligned. Absolute address
        unsigned int Vcs_Cb_Range              : 9;   // Address range of VS/CS const (0 ~ 256E), 4DWords element
                                                      // aligned.
        unsigned int Reserved                  : 14;  // Reserved
    } reg;
} Reg_Vcs_Cb_Cfg;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Hs_Cb_Base                : 9;   // Base address of HS const data in CB (0 ~ 511), 4Dwords
                                                      // element aligned. Absolute address
        unsigned int Hs_Cb_Range               : 9;   // Address range of HS const (0 ~ 256E), 4DWords element
                                                      // aligned.
        unsigned int Reserved                  : 14;  // Reserved
    } reg;
} Reg_Hs_Cb_Cfg;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Ds_Cb_Base                : 9;   // Base address of DS const data in CB (0 ~ 511), 4Dwords
                                                      // element aligned. Absolute address
        unsigned int Ds_Cb_Range               : 9;   // Address range of DS const (0 ~ 256E), 4DWords element
                                                      // aligned.
        unsigned int Reserved                  : 14;  // Reserved
    } reg;
} Reg_Ds_Cb_Cfg;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Gs_Cb_Base                : 9;   // Base address of GS const data in CB (0 ~ 511), 4Dwords
                                                      // element aligned. Absolute address
        unsigned int Gs_Cb_Range               : 9;   // Address range of GS const (0 ~ 256E), 4DWords element
                                                      // aligned.
        unsigned int Reserved                  : 14;  // Reserved
    } reg;
} Reg_Gs_Cb_Cfg;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Fs_Cb_Range               : 9;   // Address range of FS const (0 ~ 256E), 4DWords element
                                                      // aligned. <= FS_CB_Size, defined in EU_CS_Glb.
        unsigned int Reserved                  : 23;  // Reserved
    } reg;
} Reg_Fs_Cb_Cfg;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Reserved                  : 32;  // Reserved
    } reg;
} Reg_Fs_Rev_8aligned;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Reserved                  : 32;  // Reserved
    } reg;
} Reg_Fs_Rev_Cb;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Const                     : 32;  // Reserved
    } reg;
} Reg_Fs_Cb_Data;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Exec_En                   : 1;   // Debug Thread execution enable per PE
        unsigned int Th_Mode                   : 1;   // Thread debugging execution mode
        unsigned int Int_En                    : 1;   // Interrupt routine enable
        unsigned int Res_En                    : 1;   // Resume routine enable
        unsigned int Dbg_Th_Id                 : 5;   // 5bits Hardware thread ID in PE, that be executed,
                                                      // set(2)+slot(3). It is enabled when Th_Mode is single.
                                                      // Resume shader need use it to run.
        unsigned int Reserved                  : 23;  // Reserved
    } reg;
} Reg_Dbg_Cfg;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Bp_Hit                    : 1;   // Thread hits breakpoints for 32 THCs in E3k, if BP happens,
                                                      // the THC need set its Bp_Hit as 1.
        unsigned int Bp_Id                     : 3;   // Breakpoint ID that gets hit
        unsigned int Hit_Th_Id                 : 5;   // 5bits Hardware thread ID in PE that hits breakpoint,
                                                      // set(2)+slot(3)
        unsigned int Reserved                  : 23;  // Reserved
    } reg;
} Reg_Dbg_Bp_Stat;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Bp_Valid                  : 1;   // Breakpoint slot valid or not. 8 BP slot in total.
        unsigned int Shader                    : 3;   // Which shader type has BP offset
        unsigned int Offset                    : 16;  // Breakpoint PC offset, instruction (dword, 128bits)
                                                      // based, The EU hardware needs to calculate the PC address
                                                      // from this offset and base PC address.
        unsigned int Reserved                  : 12;  // Reserved
    } reg;
} Reg_Dbg_Bp_Pc;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Offset                    : 16;  // Instruction range of Debug Int shader codes (0 ~ (16K-1)).
                                                      // Each instr occupies 4Dw (128bits)
        unsigned int Reserved                  : 16;  // Reserved
    } reg;
} Reg_Dbg_Int_Instr;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Offset                    : 16;  //  Debug Resume Shader Instruction offset in main shader.
                                                      // 128its-instr aligned.
        unsigned int Reserved                  : 16;  // Reserved
    } reg;
} Reg_Dbg_Res_Instr;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Value                     : 32;  // CSP 64bits time stamp, CSP write it into MMIO register and
                                                      // then THC write them into SRFs by MOVIMM instr
    } reg;
} Reg_Dbg_Time_Stamp;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Reserved                  : 32;  // Reserved
    } reg;
} Reg_Dbg_Reversed;

typedef struct _Group_Sh_Dbg
{
    Reg_Dbg_Cfg                      reg_Dbg_Cfg;
    Reg_Dbg_Bp_Stat                  reg_Dbg_Bp_Stat;
    Reg_Dbg_Bp_Pc                    reg_Dbg_Bp_Pc[8];
    Reg_Dbg_Int_Instr                reg_Dbg_Int_Instr;
    Reg_Dbg_Res_Instr                reg_Dbg_Res_Instr;
    Reg_Dbg_Time_Stamp               reg_Dbg_Time_Stamp[2];
    Reg_Dbg_Reversed                 reg_Dbg_Reversed[2];
} Reg_Sh_Dbg_Group;

typedef struct _Eu_Fs_regs_CHX004
{
    Reg_Eu_Full_Glb                  reg_Eu_Full_Glb;
    Reg_Eu_3d_Glb                    reg_Eu_3d_Glb;
    Reg_Eu_Fe_Glb                    reg_Eu_Fe_Glb;
    Reg_Mv_Cfg                       reg_Mv_Cfg;
    Reg_Vc_Size_Cfg0                 reg_Vc_Size_Cfg0;
    Reg_Vc_Size_Cfg1                 reg_Vc_Size_Cfg1;
    Reg_Vc_Base_Cfg0                 reg_Vc_Base_Cfg0;
    Reg_Vc_Base_Cfg1                 reg_Vc_Base_Cfg1;
    Reg_Fs_Cfg                       reg_Fs_Cfg;
    Reg_Max_Threads_Cfg              reg_Max_Threads_Cfg;
    Reg_Antilock_Cfg                 reg_Antilock_Cfg;
    Reg_Vs_Ctrl                      reg_Vs_Ctrl;
    Reg_Hs_Ctrl                      reg_Hs_Ctrl;
    Reg_Hs_In_Cfg                    reg_Hs_In_Cfg;
    Reg_Hs_Out_Cfg                   reg_Hs_Out_Cfg;
    Reg_Ds_Ctrl                      reg_Ds_Ctrl;
    Reg_Ds_In_Cfg                    reg_Ds_In_Cfg;
    Reg_Gs_Ctrl                      reg_Gs_Ctrl;
    Reg_Gs_In_Cfg                    reg_Gs_In_Cfg;
    Reg_Gs_Out_Cfg                   reg_Gs_Out_Cfg;
    Reg_Ts_Ctrl                      reg_Ts_Ctrl;
    Reg_Ts_Input_Mapping0            reg_Ts_Input_Mapping0;
    Reg_Ts_Input_Mapping1            reg_Ts_Input_Mapping1;
    Reg_Ts_Factors_Const             reg_Ts_Factors_Const[6];
    Reg_Fs_Cs_Ctrl                   reg_Fs_Cs_Ctrl;
    Reg_Fs_Cs_Sm_Cfg                 reg_Fs_Cs_Sm_Cfg;
    Reg_Fs_Out_Loc0                  reg_Fs_Out_Loc0;
    Reg_Fs_Out_Loc1                  reg_Fs_Out_Loc1;
    Reg_Fs_Out_Mapping               reg_Fs_Out_Mapping[12];
    Reg_Fs_Out_Comp                  reg_Fs_Out_Comp[3];
    Reg_Fs_Out_Mask                  reg_Fs_Out_Mask[6];
    Reg_Fs_Pm_Cfg                    reg_Fs_Pm_Cfg;
    Reg_Fs_Pm_Id                     reg_Fs_Pm_Id;
    Reg_Vgs_Pm_Range                 reg_Vgs_Pm_Range;
    Reg_Hds_Pm_Range                 reg_Hds_Pm_Range;
    Reg_Fs_U_Enable                  reg_Fs_U_Enable[4];
    Reg_Fs_U_Fmt                     reg_Fs_U_Fmt[16];
    Reg_Fs_U_Layout                  reg_Fs_U_Layout[4];
    Reg_Vcs_U_Cfg                    reg_Vcs_U_Cfg;
    Reg_Hs_U_Cfg                     reg_Hs_U_Cfg;
    Reg_Ds_U_Cfg                     reg_Ds_U_Cfg;
    Reg_Gs_U_Cfg                     reg_Gs_U_Cfg;
    Reg_Vcs_Instr0                   reg_Vcs_Instr0;
    Reg_Vcs_Instr1                   reg_Vcs_Instr1;
    Reg_Vcs_Instr_Range              reg_Vcs_Instr_Range;
    Reg_Hs_Instr0                    reg_Hs_Instr0;
    Reg_Hs_Instr1                    reg_Hs_Instr1;
    Reg_Hs_Instr_Range               reg_Hs_Instr_Range;
    Reg_Ds_Instr0                    reg_Ds_Instr0;
    Reg_Ds_Instr1                    reg_Ds_Instr1;
    Reg_Ds_Instr_Range               reg_Ds_Instr_Range;
    Reg_Gs_Instr0                    reg_Gs_Instr0;
    Reg_Gs_Instr1                    reg_Gs_Instr1;
    Reg_Gs_Instr_Range               reg_Gs_Instr_Range;
    Reg_Vcs_Cb_Cfg                   reg_Vcs_Cb_Cfg;
    Reg_Hs_Cb_Cfg                    reg_Hs_Cb_Cfg;
    Reg_Ds_Cb_Cfg                    reg_Ds_Cb_Cfg;
    Reg_Gs_Cb_Cfg                    reg_Gs_Cb_Cfg;
    Reg_Fs_Cb_Cfg                    reg_Fs_Cb_Cfg;
    Reg_Fs_Rev_8aligned              reg_Fs_Rev_8aligned;
    Reg_Fs_Rev_Cb                    reg_Fs_Rev_Cb[152];
    Reg_Fs_Cb_Data                   reg_Fs_Cb_Data[2048];
    Reg_Sh_Dbg_Group                 reg_Sh_Dbg[24];
} Eu_Fs_regs_CHX004;
typedef struct _Eu_Fs_regs_Elt3k
{
    Reg_Eu_Full_Glb                  reg_Eu_Full_Glb;
    Reg_Eu_3d_Glb                    reg_Eu_3d_Glb;
    Reg_Eu_Fe_Glb                    reg_Eu_Fe_Glb;
    Reg_Mv_Cfg                       reg_Mv_Cfg;
    Reg_Vc_Size_Cfg0                 reg_Vc_Size_Cfg0;
    Reg_Vc_Size_Cfg1                 reg_Vc_Size_Cfg1;
    Reg_Vc_Base_Cfg0                 reg_Vc_Base_Cfg0;
    Reg_Vc_Base_Cfg1                 reg_Vc_Base_Cfg1;
    Reg_Fs_Cfg                       reg_Fs_Cfg;
    Reg_Max_Threads_Cfg              reg_Max_Threads_Cfg;
    Reg_Antilock_Cfg                 reg_Antilock_Cfg;
    Reg_Vs_Ctrl                      reg_Vs_Ctrl;
    Reg_Hs_Ctrl                      reg_Hs_Ctrl;
    Reg_Hs_In_Cfg                    reg_Hs_In_Cfg;
    Reg_Hs_Out_Cfg                   reg_Hs_Out_Cfg;
    Reg_Ds_Ctrl                      reg_Ds_Ctrl;
    Reg_Ds_In_Cfg                    reg_Ds_In_Cfg;
    Reg_Gs_Ctrl                      reg_Gs_Ctrl;
    Reg_Gs_In_Cfg                    reg_Gs_In_Cfg;
    Reg_Gs_Out_Cfg                   reg_Gs_Out_Cfg;
    Reg_Ts_Ctrl                      reg_Ts_Ctrl;
    Reg_Ts_Input_Mapping0            reg_Ts_Input_Mapping0;
    Reg_Ts_Input_Mapping1            reg_Ts_Input_Mapping1;
    Reg_Ts_Factors_Const             reg_Ts_Factors_Const[6];
    Reg_Fs_Cs_Ctrl                   reg_Fs_Cs_Ctrl;
    Reg_Fs_Cs_Sm_Cfg                 reg_Fs_Cs_Sm_Cfg;
    Reg_Fs_Out_Loc0                  reg_Fs_Out_Loc0;
    Reg_Fs_Out_Loc1                  reg_Fs_Out_Loc1;
    Reg_Fs_Out_Mapping               reg_Fs_Out_Mapping[12];
    Reg_Fs_Out_Comp                  reg_Fs_Out_Comp[3];
    Reg_Fs_Out_Mask                  reg_Fs_Out_Mask[6];
    Reg_Fs_Pm_Cfg                    reg_Fs_Pm_Cfg;
    Reg_Fs_Pm_Id                     reg_Fs_Pm_Id;
    Reg_Vgs_Pm_Range                 reg_Vgs_Pm_Range;
    Reg_Hds_Pm_Range                 reg_Hds_Pm_Range;
    Reg_Fs_U_Enable                  reg_Fs_U_Enable[4];
    Reg_Fs_U_Fmt                     reg_Fs_U_Fmt[16];
    Reg_Fs_U_Layout                  reg_Fs_U_Layout[4];
    Reg_Vcs_U_Cfg                    reg_Vcs_U_Cfg;
    Reg_Hs_U_Cfg                     reg_Hs_U_Cfg;
    Reg_Ds_U_Cfg                     reg_Ds_U_Cfg;
    Reg_Gs_U_Cfg                     reg_Gs_U_Cfg;
    Reg_Vcs_Instr0                   reg_Vcs_Instr0;
    Reg_Vcs_Instr1                   reg_Vcs_Instr1;
    Reg_Vcs_Instr_Range              reg_Vcs_Instr_Range;
    Reg_Hs_Instr0                    reg_Hs_Instr0;
    Reg_Hs_Instr1                    reg_Hs_Instr1;
    Reg_Hs_Instr_Range               reg_Hs_Instr_Range;
    Reg_Ds_Instr0                    reg_Ds_Instr0;
    Reg_Ds_Instr1                    reg_Ds_Instr1;
    Reg_Ds_Instr_Range               reg_Ds_Instr_Range;
    Reg_Gs_Instr0                    reg_Gs_Instr0;
    Reg_Gs_Instr1                    reg_Gs_Instr1;
    Reg_Gs_Instr_Range               reg_Gs_Instr_Range;
    Reg_Vcs_Cb_Cfg                   reg_Vcs_Cb_Cfg;
    Reg_Hs_Cb_Cfg                    reg_Hs_Cb_Cfg;
    Reg_Ds_Cb_Cfg                    reg_Ds_Cb_Cfg;
    Reg_Gs_Cb_Cfg                    reg_Gs_Cb_Cfg;
    Reg_Fs_Cb_Cfg                    reg_Fs_Cb_Cfg;
    Reg_Fs_Rev_8aligned              reg_Fs_Rev_8aligned;
    Reg_Fs_Rev_Cb                    reg_Fs_Rev_Cb[152];
    Reg_Fs_Cb_Data                   reg_Fs_Cb_Data[2048];
    Reg_Dbg_Cfg                      reg_Dbg_Cfg[32];
    Reg_Dbg_Bp_Stat                  reg_Dbg_Bp_Stat[32];
    Reg_Dbg_Bp_Pc                    reg_Dbg_Bp_Pc[256];
    Reg_Dbg_Int_Instr                reg_Dbg_Int_Instr[32];
    Reg_Dbg_Res_Instr                reg_Dbg_Res_Instr[32];
    Reg_Dbg_Time_Stamp               reg_Dbg_Time_Stamp[64];
} Eu_Fs_regs_Elt3k;

#endif
