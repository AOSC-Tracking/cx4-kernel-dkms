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
//    Spec Last Modified Time: 2019/8/26 14:01:51
#ifndef _CSP_GLOBAL_REGISTERS_H
#define _CSP_GLOBAL_REGISTERS_H


#ifndef        CSP_GLOBAL_BLOCKBASE_INF
#define    CSP_GLOBAL_BLOCKBASE_INF
#define    BLOCK_CSP_GLOBAL_VERSION 1
#define    BLOCK_CSP_GLOBAL_TIMESTAMP "2019/8/26 14:01:51"
#define    CSP_GLOBAL_BLOCK                                           0x0 // match with BlockID.h
#define    CSP_GLOBAL_REG_START                                       0x0 // match with BlockID.h
#define    CSP_GLOBAL_REG_END_CHX004                                  0x380 // match with BlockID.h
#define    CSP_GLOBAL_REG_LIMIT_CHX004                                0x380 // match with BlockID.h
#define    CSP_GLOBAL_REG_END_Elt3k                                   0x3C0 // match with BlockID.h
#define    CSP_GLOBAL_REG_LIMIT_Elt3k                                 0x3C0 // match with BlockID.h
#endif

// Register offset definition
#define        Reg_Block_Busy_Bits_Top_Offset                             0x0
#define        Reg_Block_Busy_Bits_Gpc0_0_Offset                          0x1
#define        Reg_Block_Busy_Bits_Gpc0_1_Offset                          0x2
#define        Reg_Block_Busy_Bits_Gpc1_0_Offset                          0x3
#define        Reg_Block_Busy_Bits_Gpc1_1_Offset                          0x4
#define        Reg_Block_Busy_Bits_Gpc2_0_Offset                          0x5
#define        Reg_Block_Busy_Bits_Gpc2_1_Offset                          0x6
#define        Reg_Ring_Buf_Offset                                        0x7
#define        Reg_Preempt_Cmd_Process_Offset                             0x23
#define        Reg_Cpu_Set_Offset                                         0x24
#define        Reg_Fence_Mask_Offset                                      0x25
#define        Reg_Trans_Threshold_Cnt_Offset_CHX004                      0x26
#define        Reg_Reserved_Dw_0_Offset_Elt3k                             0x26
#define        Reg_Pfb_Partition_3d_Cfg_Offset                            0x27
#define        Reg_Pfb_Partition_Cs_Cfg_Offset                            0x28
#define        Reg_Csp_Ms_Total_Gpu_Timestamp_Offset_Elt3k                0x29

#define        Reg_Csp_Ref_Total_Gpu_Timestamp_Offset                     Reg_Csp_Ms_Total_Gpu_Timestamp_Offset_Elt3k
#define        Reg_Csp_Ms_Total_Busy_Time_Offset                          0x2B
#define        Reg_Csp_Ms_Query_Occlusion_Offset                          0x2D
#define        Reg_Csp_Ms_Total_Gpu_Timestamp_Offset_CHX004               0x2F

//only for cne001
#define        Reg_Csp_Ms_Total_Gpu_Timestamp_Offset_CNE001                      0x29
#define        Reg_Csp_Ref_Total_Gpu_Timestamp_Offset_CNE001                     0x2d
#define        Reg_Csp_Ms_Query_Occlusion_Offset_CNE001                          0x2f

#define        Reg_Ia_Vertices_Cnt_Offset_Elt3k                           0x2F
#define        Reg_Ia_Primitives_Cnt_Offset_Elt3k                         0x31

#define        Reg_Ia_Ogl_Cut_Flag_32_Offset                              0x33
#define        Reg_Ia_Vb0_Offset_Offset                                   0x34
#define        Reg_Ia_Vb0_Stride_Offset                                   0x35
#define        Reg_Ia_Bufferfilledsize_Offset                             0x36
#define        Reg_Ia_Batch_Cfg_Offset                                    0x37
#define        Reg_Predicate_Status_Offset                                0x38
#define        Reg_Csp_Misc_Control_Offset                                0x39

#define        Reg_Index_Buf_Fmt_Enum_Offset_CHX004                       0x3A
#define        Reg_Ia_Input_Vertices_Cnt_Offset_CHX004                    0x3B
#define        Reg_Ia_Output_Vertices_Cnt_Offset_CHX004                   0x3D
#define        Reg_Ia_Primitives_Cnt_Offset_CHX004                        0x3F
#define        Reg_Csp_Skip_2_Offset_CHX004                               0x41

#define        Reg_Tbr_Render_Mode_Ctrl_Offset_Elt3k                      0x3A
#define        Reg_Tbr_Frametimestamp_Lth_Offset_Elt3k                    0x3B
#define        Reg_Tbr_Frametimestamp_Hth_Offset_Elt3k                    0x3C
#define        Reg_Tbr_Traffic_Th_Offset_Elt3k                            0x3D
#define        Reg_Tbr_Event_Count_Ref_Offset_Elt3k                       0x3E
#define        Reg_Descriptor_Base_Addr_Offset_Elt3k                      0x3F

#define        Reg_Csp_Fence_Counter_Offset                               0x47
#define        Reg_Ila_Counters_Offset                                    0x57
#define        Reg_Gpc_Signature_Offset                                   0x7F
#define        Reg_Sto_Signature_Offset                                   0xE5
#define        Reg_Csp_Slice_Status_Offset                                0xED
#define        Reg_Cur_L1_Dma_Cmd_Offset                                  0xEE
#define        Reg_Cur_L2_Dma_Cmd_Offset                                  0xEF
#define        Reg_Cur_3d_Rbuf_Cmd_Offset                                 0xF0
#define        Reg_Cur_3d_Hrbuf_Cmd_Offset                                0xF1
#define        Reg_Cur_Csh_Rbuf_Cmd_Offset                                0xF2
#define        Reg_Cur_Cs0_Rbuf_Cmd_Offset                                0xF3
#define        Reg_Cur_Cs1_Rbuf_Cmd_Offset                                0xF4
#define        Reg_Cur_Cs2_Rbuf_Cmd_Offset                                0xF5
#define        Reg_Cur_Cs3_Rbuf_Cmd_Offset                                0xF6
#define        Reg_U_Fence_Num_Ctrl_Offset                                0xF7
#define        Reg_Gpu_Idle_Threshold_Offset                              0xF8 //cne001
#define        Reg_Csp_Skip_3_Offset                                      0xF9

#define        Reg_C3d_Eng_Workload_Offset_Elt3k                          0xF7
#define        Reg_C3d_Eng_Alu_Workload_Offset_Elt3k                      0xF8

#define        Reg_C3d_Mem_Workload_Offset_Elt3k                          0xF9
#define        Reg_Vpp_Workload_Offset_Elt3k                              0xFA
#define        Reg_Vcp0_Workload_Offset_Elt3k                             0xFB
#define        Reg_Vcp1_Workload_Offset_Elt3k                             0xFC
#define        Reg_Csp_Skip0_Offset_Elt3k                                 0xFD

#define        Reg_Reg_Dvfs_Cfg_En_Offset                                 0x100
#define        Reg_Reg_Dvfs_Cfg_Misc0_Offset                              0x101
#define        Reg_Reg_3d_Dvfs_Checksize_Offset                           0x102
#define        Reg_Reg_3d_Dvfs_H_Th_Offset                                0x103
#define        Reg_Reg_3d_Dvfs_L_Th_Offset                                0x104
#define        Reg_Reg_3d_Dvfs_H_Th_Mem_Offset                            0x105
#define        Reg_Reg_3d_Alu_Workload_Offset                             0x106
#define        Reg_Reg_3d_Busy_Workload_Offset                            0x107
#define        Reg_Reg_Vcp0_Dvfs_Checksize_Offset                         0x108
#define        Reg_Reg_Vcp0_Dvfs_H_Th_Offset                              0x109
#define        Reg_Reg_Vcp0_Dvfs_L_Th_Offset                              0x10A
#define        Reg_Reg_Vcp0_Dvfs_H_Th_Mem_Offset                          0x10B
#define        Reg_Reg_Vcp0_Busy_Cycle_Offset                             0x10C
#define        Reg_Reg_Vcp1_Dvfs_Checksize_Offset                         0x10D
#define        Reg_Reg_Vcp1_Dvfs_H_Th_Offset                              0x10E
#define        Reg_Reg_Vcp1_Dvfs_L_Th_Offset                              0x10F
#define        Reg_Reg_Vcp1_Dvfs_H_Th_Mem_Offset                          0x110
#define        Reg_Reg_Vcp1_Busy_Cycle_Offset                             0x111
#define        Reg_Reg_Vpp_Dvfs_Checksize_Offset                          0x112
#define        Reg_Reg_Vpp_Dvfs_H_Th_Offset                               0x113
#define        Reg_Reg_Vpp_Dvfs_L_Th_Offset                               0x114
#define        Reg_Reg_Vpp_Dvfs_H_Th_Mem_Offset                           0x115
#define        Reg_Reg_Vpp_Busy_Cycle_Offset                              0x116
#define        Reg_Reg_Dvfs_Pcu_Cfg_En_Offset                             0x117
#define        Reg_Reg_3d_Mem_Busy_Cycle_Offset                           0x118
#define        Reg_Reg_Vcp0_Mem_Busy_Cycle_Offset                         0x119
#define        Reg_Reg_Vcp1_Mem_Busy_Cycle_Offset                         0x11A
#define        Reg_Reg_Vpp_Mem_Busy_Cycle_Offset                          0x11B
#define        Reg_Pwr_Mgr_Cfg_En_Offset                                  0x11C
#define        Reg_Pwr_Mgr_Pg_Pd_Misc0_Offset                             0x11D
#define        Reg_Pwr_Mgr_Pg_Pd_Misc1_Offset                             0x11E
#define        Reg_Pwr_Mgr_Pg_Pd_Misc2_Offset                             0x11F
#define        Reg_Pwr_Mgr_Pg_Pu_Misc0_Offset                             0x120
#define        Reg_Pwr_Mgr_Pg_Pu_Misc1_Offset                             0x121
#define        Reg_Pwr_Mgr_Pg_Pu_Misc2_Offset                             0x122
#define        Reg_Pwr_Mgr_Wait_3d_Cnt0_Offset                            0x123
#define        Reg_Pwr_Mgr_Wait_S3vd_Cnt1_Offset                          0x124
#define        Reg_Pwr_Mgr_Wait_Vpp_Cnt2_Offset                           0x125
#define        Reg_Pwr_Mgr_Wait_Mxu0_Cnt3_Offset                          0x126
#define        Reg_Pwr_Mgr_Sw_Cfg_En_Offset                               0x127
#define        Reg_Pwr_Mgr_Sw_Cfg0_Offset                                 0x128
#define        Reg_Pwr_Mgr_Status0_Offset                                 0x129
#define        Reg_Pwr_Mgr_Status1_Offset                                 0x12A
#define        Reg_Csp_Skip_4_Offset                                      0x12B

// For Cne001
#define        Reg_Pwr_Mgr_Sw_Cfg1_Offset                                 0x129
#define        Reg_Pwr_Mgr_Status0_Offset_Cne001                          0x12A
#define        Reg_Pwr_Mgr_Status1_Offset_Cne001                          0x12B
#define        Reg_Pwr_Mgr_Status2_Offset                                 0x12C
#define        Reg_Csp_Skip_4_Offset_Cne001                               0x12D

#define        Reg_Pwr_Ts_Cfg_En_Offset                                   0x140
#define        Reg_Pwr_Ts_Cfg_Misc0_Offset                                0x141
#define        Reg_Pwr_Ts_Cfg_Misc1_Offset                                0x142
#define        Reg_Pwr_Ts_Cfg_Misc2_Offset                                0x143
#define        Reg_Pwr_Ts_Cfg_Misc3_Offset                                0x144
#define        Reg_Pwr_Ts_Cfg_Misc4_Offset                                0x145
#define        Reg_Pwr_Ts_Cfg_Misc5_Offset                                0x146
#define        Reg_Pwr_Ts_Cfg_Misc6_Offset                                0x147
#define        Reg_Pwr_Ts_Cfg_Misc7_Offset                                0x148
#define        Reg_Csp_Skip_5_Offset                                      0x149
// For Cne001
#define        Reg_Csp_Skip_5_Offset_Cne001                               0x142

#define        Reg_Vcp_Ring_Buf_Offset                                    0x180
#define        Reg_Vpp_Ring_Buf_Offset                                    0x188
#define        Reg_Vpp_Ring_Buf_Offset_Cne001                             0x190
#define        Reg_Vcp_Vpp_Block_Busy_Bits_Offset                         0x18C
#define        Reg_Vcp_Vpp_Block_Busy_Bits_Offset_CNE001                  0x198
#define        Reg_Csp_Skip1_Offset                                       0x18D
#define        Reg_Cmodel_Dma_Preemption_Begin_Offset                     0x1FC
#define        Reg_Cmodel_Dma_Preemption_Cnt_Offset                       0x1FD
#define        Reg_Cmodel_Dma_Preemption_Tail_Offset                      0x1FE
#define        Reg_Cmodel_Hl_Switch_Offset                                0x1FF
#define        Reg_Eu_Dbg_Reg_Offset                                      0x200

#ifndef CHOOSE_REG_OFFSET_BY_CHIP
#define CHOOSE_REG_OFFSET_BY_CHIP(bElt3k,offset)  ((bElt3k) ? offset##_Elt3k : offset##_CHX004)
#endif

//#define Reg_Csp_Ms_Total_Gpu_Timestamp_Offset(bElt3k)                   CHOOSE_REG_OFFSET_BY_CHIP(bElt3k,Reg_Csp_Ms_Total_Gpu_Timestamp_Offset)//get total time stamp value
#define Reg_Csp_Ms_Total_Gpu_Timestamp_Offset                           Reg_Csp_Ms_Total_Gpu_Timestamp_Offset_CHX004

// Block constant definition
typedef enum
{
    CSP_MISC_CONTROL_PROVOKEMODE_FIRST_VTX = 0,   // when flat rendering, use color of first vtx
    CSP_MISC_CONTROL_PROVOKEMODE_LAST_VTX = 1,   // when flat rendering, use color of last vtx
} CSP_MISC_CONTROL_PROVOKEMODE;
typedef enum
{
    CMODEL_HL_SWITCH_CMD_TYPE_NO_SWITCH = 0,   // not switch
    CMODEL_HL_SWITCH_CMD_TYPE_DIP = 1,
    CMODEL_HL_SWITCH_CMD_TYPE_GP = 2,
    CMODEL_HL_SWITCH_CMD_TYPE_FAST_CLEAR = 3,
    CMODEL_HL_SWITCH_CMD_TYPE_BIT_BLT = 4,
    CMODEL_HL_SWITCH_CMD_TYPE_IMAGE_TRANSFER = 5,
    CMODEL_HL_SWITCH_CMD_TYPE_INDICATOR_OFF = 6,
} CMODEL_HL_SWITCH_CMD_TYPE;
typedef enum
{
    EU_DBG_CFG_EXEC_EN_DISABLED = 0,   // 0: Disable, THC cleans this flag when it reaches one of the
                                       // breakpoints, and waits until the debugger sets this flag
                                       // to resume execution
                                       EU_DBG_CFG_EXEC_EN_ENABLED = 1,   // 1: Enable, the debugger sets this flag to notify the EU to
                                                                         // start/resume execution
} EU_DBG_CFG_EXEC_EN;
typedef enum
{
    EU_DBG_CFG_TH_MODE_SINGLE = 0,   // 0: Single, the EU executes one thread only, specified by
                                     // Dbg_Th_Id
                                     EU_DBG_CFG_TH_MODE_ALL = 1,   // 1: All, the EU executes all available threads
} EU_DBG_CFG_TH_MODE;
typedef enum
{
    EU_DBG_CFG_INT_EN_DISABLED = 0,   // 0: Disable, the EU will NOT call the interrupt routine when
                                      // it hits the breakpoints
                                      EU_DBG_CFG_INT_EN_ENABLED = 1,   // 1: Enable, the EU will call the interrupt routine when it
                                                                       // hits the breakpoints. Only Hit_Th_Id thread need run
                                                                       // interrupt shader.
} EU_DBG_CFG_INT_EN;
typedef enum
{
    EU_DBG_CFG_RES_EN_DISABLED = 0,   // 0: Disable, the EU will NOT call the resume routine when it
                                      // resumes execution.
                                      EU_DBG_CFG_RES_EN_ENABLED = 1,   // 1: Enable, the EU will call the resume routine when it
                                                                       // resumes execution. only Hit_Th_Id thread need run resume
                                                                       // shader
} EU_DBG_CFG_RES_EN;
typedef enum
{
    EU_DBG_BP_STAT_BP_HIT_DISABLED = 0,   // 0: Disable, the EU hasn't hit the breakpoints. Driver need
                                          // clear this flag before shader continues to run.
                                          EU_DBG_BP_STAT_BP_HIT_ENABLED = 1,   // 1: Enable, when the EU hits one of the breakpoints, it will
                                                                               // suspend all threads, and then set this flag. The hit PC and
                                                                               // threadID are also recorded.
} EU_DBG_BP_STAT_BP_HIT;
typedef enum
{
    EU_DBG_BP_PC_BP_VALID_DISABLED = 0,   // 0: Disable, the current breakpoint slot is disabled
    EU_DBG_BP_PC_BP_VALID_ENABLED = 1,   // 1: Enable, the current breakpoint slot is enabled
} EU_DBG_BP_PC_BP_VALID;
typedef enum
{
    EU_DBG_BP_PC_SHADER_VS = 0,
    EU_DBG_BP_PC_SHADER_HS = 1,
    EU_DBG_BP_PC_SHADER_DS = 2,
    EU_DBG_BP_PC_SHADER_GS = 3,
    EU_DBG_BP_PC_SHADER_PS = 4,
    EU_DBG_BP_PC_SHADER_CS = 5,
} EU_DBG_BP_PC_SHADER;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////            CSP_GLOBAL Block (Block ID = 0) Register Definitions                      ///////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define Reg_BLOCK_BUSY_BITS_TOP_Csp_Busy_BitField_MaxValue ((1u << 1) - 1)
#define Reg_BLOCK_BUSY_BITS_TOP_Mxua_Busy_BitField_MaxValue ((1u << 1) - 1)
#define Reg_BLOCK_BUSY_BITS_TOP_Mxub_Busy_BitField_MaxValue ((1u << 1) - 1)
#define Reg_BLOCK_BUSY_BITS_TOP_L2_Busy_BitField_MaxValue ((1u << 1) - 1)
#define Reg_BLOCK_BUSY_BITS_TOP_Gpcpfe_Busy_BitField_MaxValue ((1u << 1) - 1)
#define Reg_BLOCK_BUSY_BITS_TOP_Gpcpbe_Busy_BitField_MaxValue ((1u << 1) - 1)
#define Reg_BLOCK_BUSY_BITS_TOP_Sg_Busy_BitField_MaxValue ((1u << 3) - 1)
#define Reg_BLOCK_BUSY_BITS_TOP_Tasfe_Busy_BitField_MaxValue ((1u << 3) - 1)
#define Reg_BLOCK_BUSY_BITS_TOP_Tasbe_Busy_BitField_MaxValue ((1u << 3) - 1)
#define Reg_BLOCK_BUSY_BITS_TOP_Hub_Busy_BitField_MaxValue ((1u << 3) - 1)
#define Reg_BLOCK_BUSY_BITS_TOP_Reserved_BitField_MaxValue ((1u << 14) - 1)

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Csp_Busy : 1;
        unsigned int Mxua_Busy : 1;
        unsigned int Mxub_Busy : 1;
        unsigned int L2_Busy : 1;
        unsigned int Gpcpfe_Busy : 1;
        unsigned int Gpcpbe_Busy : 1;
        unsigned int Sg_Busy : 3;
        unsigned int Tasfe_Busy : 3;
        unsigned int Tasbe_Busy : 3;
        unsigned int Hub_Busy : 3;
        unsigned int Reserved : 13;
        unsigned int EngineHang : 1;
    } reg;
} Reg_Block_Busy_Bits_Top;

#define Reg_BLOCK_BUSY_BITS_GPC0_0_Tgz_Busy_BitField_MaxValue ((1u << 4) - 1)
#define Reg_BLOCK_BUSY_BITS_GPC0_0_Iu_Busy_BitField_MaxValue ((1u << 4) - 1)
#define Reg_BLOCK_BUSY_BITS_GPC0_0_Wbu_Busy_BitField_MaxValue ((1u << 4) - 1)
#define Reg_BLOCK_BUSY_BITS_GPC0_0_Wls_Busy_BitField_MaxValue ((1u << 4) - 1)
#define Reg_BLOCK_BUSY_BITS_GPC0_0_Ffc_Busy_BitField_MaxValue ((1u << 4) - 1)
#define Reg_BLOCK_BUSY_BITS_GPC0_0_Tu_Busy_BitField_MaxValue ((1u << 4) - 1)
#define Reg_BLOCK_BUSY_BITS_GPC0_0_Reserved_BitField_MaxValue ((1u << 8) - 1)

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Tgz_Busy : 4;
        unsigned int Iu_Busy : 4;
        unsigned int Wbu_Busy : 4;
        unsigned int Wls_Busy : 4;
        unsigned int Ffc_Busy : 4;
        unsigned int Tu_Busy : 4;
        unsigned int Reserved : 8;
    } reg;
} Reg_Block_Busy_Bits_Gpc0_0;

#define Reg_BLOCK_BUSY_BITS_GPC0_1_Eu_Constructor_Busy_BitField_MaxValue ((1u << 4) - 1)
#define Reg_BLOCK_BUSY_BITS_GPC0_1_Euvs_Busy_BitField_MaxValue ((1u << 4) - 1)
#define Reg_BLOCK_BUSY_BITS_GPC0_1_Euhs_Busy_BitField_MaxValue ((1u << 4) - 1)
#define Reg_BLOCK_BUSY_BITS_GPC0_1_Eufe_Busy_BitField_MaxValue ((1u << 4) - 1)
#define Reg_BLOCK_BUSY_BITS_GPC0_1_Euds_Busy_BitField_MaxValue ((1u << 4) - 1)
#define Reg_BLOCK_BUSY_BITS_GPC0_1_Eugs_Busy_BitField_MaxValue ((1u << 4) - 1)
#define Reg_BLOCK_BUSY_BITS_GPC0_1_Eups_Busy_BitField_MaxValue ((1u << 4) - 1)
#define Reg_BLOCK_BUSY_BITS_GPC0_1_Eucs_Busy_BitField_MaxValue ((1u << 4) - 1)

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Eu_Constructor_Busy : 4;   // 3DFE,PSC,CSL,CSH. Each ctx in 4 slices.
        unsigned int Euvs_Busy : 4;
        unsigned int Euhs_Busy : 4;
        unsigned int Eufe_Busy : 4;
        unsigned int Euds_Busy : 4;
        unsigned int Eugs_Busy : 4;
        unsigned int Eups_Busy : 4;
        unsigned int Eucs_Busy : 4;
    } reg;
} Reg_Block_Busy_Bits_Gpc0_1;

#define Reg_BLOCK_BUSY_BITS_GPC1_0_Tgz_Busy_BitField_MaxValue ((1u << 4) - 1)
#define Reg_BLOCK_BUSY_BITS_GPC1_0_Iu_Busy_BitField_MaxValue ((1u << 4) - 1)
#define Reg_BLOCK_BUSY_BITS_GPC1_0_Wbu_Busy_BitField_MaxValue ((1u << 4) - 1)
#define Reg_BLOCK_BUSY_BITS_GPC1_0_Wls_Busy_BitField_MaxValue ((1u << 4) - 1)
#define Reg_BLOCK_BUSY_BITS_GPC1_0_Ffc_Busy_BitField_MaxValue ((1u << 4) - 1)
#define Reg_BLOCK_BUSY_BITS_GPC1_0_Tu_Busy_BitField_MaxValue ((1u << 4) - 1)
#define Reg_BLOCK_BUSY_BITS_GPC1_0_Reserved_BitField_MaxValue ((1u << 8) - 1)

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Tgz_Busy : 4;
        unsigned int Iu_Busy : 4;
        unsigned int Wbu_Busy : 4;
        unsigned int Wls_Busy : 4;
        unsigned int Ffc_Busy : 4;
        unsigned int Tu_Busy : 4;
        unsigned int Reserved : 8;
    } reg;
} Reg_Block_Busy_Bits_Gpc1_0;

#define Reg_BLOCK_BUSY_BITS_GPC1_1_Eu_Constructor_Busy_BitField_MaxValue ((1u << 4) - 1)
#define Reg_BLOCK_BUSY_BITS_GPC1_1_Euvs_Busy_BitField_MaxValue ((1u << 4) - 1)
#define Reg_BLOCK_BUSY_BITS_GPC1_1_Euhs_Busy_BitField_MaxValue ((1u << 4) - 1)
#define Reg_BLOCK_BUSY_BITS_GPC1_1_Eufe_Busy_BitField_MaxValue ((1u << 4) - 1)
#define Reg_BLOCK_BUSY_BITS_GPC1_1_Euds_Busy_BitField_MaxValue ((1u << 4) - 1)
#define Reg_BLOCK_BUSY_BITS_GPC1_1_Eugs_Busy_BitField_MaxValue ((1u << 4) - 1)
#define Reg_BLOCK_BUSY_BITS_GPC1_1_Eups_Busy_BitField_MaxValue ((1u << 4) - 1)
#define Reg_BLOCK_BUSY_BITS_GPC1_1_Eucs_Busy_BitField_MaxValue ((1u << 4) - 1)

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Eu_Constructor_Busy : 4;   // 3DFE,PSC,CSL,CSH. Each ctx in 4 slices.
        unsigned int Euvs_Busy : 4;
        unsigned int Euhs_Busy : 4;
        unsigned int Eufe_Busy : 4;
        unsigned int Euds_Busy : 4;
        unsigned int Eugs_Busy : 4;
        unsigned int Eups_Busy : 4;
        unsigned int Eucs_Busy : 4;
    } reg;
} Reg_Block_Busy_Bits_Gpc1_1;

#define Reg_BLOCK_BUSY_BITS_GPC2_0_Tgz_Busy_BitField_MaxValue ((1u << 4) - 1)
#define Reg_BLOCK_BUSY_BITS_GPC2_0_Iu_Busy_BitField_MaxValue ((1u << 4) - 1)
#define Reg_BLOCK_BUSY_BITS_GPC2_0_Wbu_Busy_BitField_MaxValue ((1u << 4) - 1)
#define Reg_BLOCK_BUSY_BITS_GPC2_0_Wls_Busy_BitField_MaxValue ((1u << 4) - 1)
#define Reg_BLOCK_BUSY_BITS_GPC2_0_Ffc_Busy_BitField_MaxValue ((1u << 4) - 1)
#define Reg_BLOCK_BUSY_BITS_GPC2_0_Tu_Busy_BitField_MaxValue ((1u << 4) - 1)
#define Reg_BLOCK_BUSY_BITS_GPC2_0_Reserved_BitField_MaxValue ((1u << 8) - 1)

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Tgz_Busy : 4;
        unsigned int Iu_Busy : 4;
        unsigned int Wbu_Busy : 4;
        unsigned int Wls_Busy : 4;
        unsigned int Ffc_Busy : 4;
        unsigned int Tu_Busy : 4;
        unsigned int Reserved : 8;
    } reg;
} Reg_Block_Busy_Bits_Gpc2_0;

#define Reg_BLOCK_BUSY_BITS_GPC2_1_Eu_Constructor_Busy_BitField_MaxValue ((1u << 4) - 1)
#define Reg_BLOCK_BUSY_BITS_GPC2_1_Euvs_Busy_BitField_MaxValue ((1u << 4) - 1)
#define Reg_BLOCK_BUSY_BITS_GPC2_1_Euhs_Busy_BitField_MaxValue ((1u << 4) - 1)
#define Reg_BLOCK_BUSY_BITS_GPC2_1_Eufe_Busy_BitField_MaxValue ((1u << 4) - 1)
#define Reg_BLOCK_BUSY_BITS_GPC2_1_Euds_Busy_BitField_MaxValue ((1u << 4) - 1)
#define Reg_BLOCK_BUSY_BITS_GPC2_1_Eugs_Busy_BitField_MaxValue ((1u << 4) - 1)
#define Reg_BLOCK_BUSY_BITS_GPC2_1_Eups_Busy_BitField_MaxValue ((1u << 4) - 1)
#define Reg_BLOCK_BUSY_BITS_GPC2_1_Eucs_Busy_BitField_MaxValue ((1u << 4) - 1)

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Eu_Constructor_Busy : 4;   // 3DFE,PSC,CSL,CSH. Each ctx in 4 slices.
        unsigned int Euvs_Busy : 4;
        unsigned int Euhs_Busy : 4;
        unsigned int Eufe_Busy : 4;
        unsigned int Euds_Busy : 4;
        unsigned int Eugs_Busy : 4;
        unsigned int Eups_Busy : 4;
        unsigned int Eucs_Busy : 4;
    } reg;
} Reg_Block_Busy_Bits_Gpc2_1;

#define Reg_RUN_LIST_CTX_ADDR1_Addr_BitField_MaxValue ((1u << 28) - 1)
#define Reg_RUN_LIST_CTX_ADDR1_L2_Cachable_BitField_MaxValue ((1u << 1) - 1)
#define Reg_RUN_LIST_CTX_ADDR1_Reserved_BitField_MaxValue ((1u << 2) - 1)
#define Reg_RUN_LIST_CTX_ADDR1_Kickoff_BitField_MaxValue ((1u << 1) - 1)

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Addr : 28;  // For 4KB alignedRun List Context Base Address, need 40-12
                                 // bits.
        unsigned int L2_Cachable : 1;   // Whether need to check the data is in L2 or not, used by MXU.
                                        // For 3D and CS RB0, since they're driver generated, the cmd
                                        // can't be in L2, so this bit should always be 0. For CS RB1~3,
                                        // they're for kkk, and the cmd can be in L2. Driver can set this
                                        // bit into 1 for CS RB1~3 if necessary.
        unsigned int Reserved : 2;
        unsigned int Kickoff : 1;   // whether the RB is kick off
    } reg;
} Reg_Run_List_Ctx_Addr1;

#define Reg_RING_BUF_SIZE_Rb_Size_BitField_MaxValue ((1u << 32) - 1)

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Rb_Size : 32;  // RB size
    } reg;
} Reg_Ring_Buf_Size;

#define Reg_RING_BUF_HEAD_Rb_Head_BitField_MaxValue ((1u << 32) - 1)

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Rb_Head : 32;  // RB Current Head Pointer, byte offset
    } reg;
} Reg_Ring_Buf_Head;

#define Reg_RING_BUF_TAIL_Rb_Tail_BitField_MaxValue ((1u << 32) - 1)

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Rb_Tail : 32;  // RB Current Tail Pointer, byte offset
    } reg;
} Reg_Ring_Buf_Tail;

typedef struct _Group_Ring_Buf
{
    Reg_Run_List_Ctx_Addr1           reg_Run_List_Ctx_Addr1;
    Reg_Ring_Buf_Size                reg_Ring_Buf_Size;
    Reg_Ring_Buf_Head                reg_Ring_Buf_Head;
    Reg_Ring_Buf_Tail                reg_Ring_Buf_Tail;
} Reg_Ring_Buf_Group;

#define Reg_PREEMPT_CMD_PROCESS_Preempt_BitField_MaxValue ((1u << 1) - 1)
#define Reg_PREEMPT_CMD_PROCESS_Reserved_BitField_MaxValue ((1u << 31) - 1)

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Preempt : 1;   // when 1 and met with ext_fence with swap_fence=1, then do
                                    // preempt
        unsigned int Reserved : 31;
    } reg;
} Reg_Preempt_Cmd_Process;

#define Reg_CPU_SET_Cpu_Set_BitField_MaxValue ((1u << 1) - 1)
#define Reg_CPU_SET_Reserved_BitField_MaxValue ((1u << 31) - 1)

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Cpu_Set : 1;   // updated by cpu, through mmio write. For vulkan event sync
        unsigned int Reserved : 31;
    } reg;
} Reg_Cpu_Set;

#define Reg_FENCE_MASK_Mask_BitField_MaxValue ((1u << 32) - 1)

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Mask : 32;  // tell which fence need to be restore when do restore cmd,
                                 // totally 32 fence dw
    } reg;
} Reg_Fence_Mask;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Counter : 20;
        unsigned int Reesrved : 12;
    } reg;
} Reg_Trans_Threshold_Cnt;

#define Reg_PFB_PARTITION_3D_CFG_Lrb_Size_BitField_MaxValue ((1u << 4) - 1)
#define Reg_PFB_PARTITION_3D_CFG_L1dma_Size_BitField_MaxValue ((1u << 4) - 1)
#define Reg_PFB_PARTITION_3D_CFG_L2dma_Size_BitField_MaxValue ((1u << 4) - 1)
#define Reg_PFB_PARTITION_3D_CFG_L1buf_Size_BitField_MaxValue ((1u << 4) - 1)
#define Reg_PFB_PARTITION_3D_CFG_Lib_Size_BitField_MaxValue ((1u << 4) - 1)
#define Reg_PFB_PARTITION_3D_CFG_Hrb_Size_BitField_MaxValue ((1u << 4) - 1)
#define Reg_PFB_PARTITION_3D_CFG_Hl1buf_Size_BitField_MaxValue ((1u << 4) - 1)
#define Reg_PFB_PARTITION_3D_CFG_Hib_Size_BitField_MaxValue ((1u << 4) - 1)

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Reesrved                  : 32;
    } reg;
} Reg_Reserved_Dw_0;

#define Reg_PFB_PARTITION_CS_CFG_Csh_Rb_Size_BitField_MaxValue ((1u << 4) - 1)
#define Reg_PFB_PARTITION_CS_CFG_Csl0_Rb_Size_BitField_MaxValue ((1u << 4) - 1)
#define Reg_PFB_PARTITION_CS_CFG_Csl1_Rb_Size_BitField_MaxValue ((1u << 4) - 1)
#define Reg_PFB_PARTITION_CS_CFG_Csl2_Rb_Size_BitField_MaxValue ((1u << 4) - 1)
#define Reg_PFB_PARTITION_CS_CFG_Csl3_Rb_Size_BitField_MaxValue ((1u << 4) - 1)
#define Reg_PFB_PARTITION_CS_CFG_L1dma_Size_BitField_MaxValue ((1u << 4) - 1)
#define Reg_PFB_PARTITION_CS_CFG_L2dma_Size_BitField_MaxValue ((1u << 4) - 1)
#define Reg_PFB_PARTITION_CS_CFG_Csl_L1buf_Size_BitField_MaxValue ((1u << 4) - 1)

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Lrb_Size : 4;   // aligned to 8X256bit space. There will be LRB_size*
                                     // 8*256bit space in PFB for LowRB ,
        unsigned int L1dma_Size : 4;   // 1st level DMA prefecth buffer
        unsigned int L2dma_Size : 4;   // 2nd level DMA prefecth buffer
        unsigned int L1buf_Size : 4;   // L1Buf of LowRB
        unsigned int Lib_Size : 4;   // index buffer of LowRB
        unsigned int Hrb_Size : 4;   // High RB
        unsigned int Hl1buf_Size : 4;   // L1Buf of HighRB
        unsigned int Hib_Size : 4;   // index buffer of HighRB
    } reg;
} Reg_Pfb_Partition_3d_Cfg;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Csh_Rb_Size : 4;
        unsigned int Csl0_Rb_Size : 4;
        unsigned int Csl1_Rb_Size : 4;
        unsigned int Csl2_Rb_Size : 4;
        unsigned int Csl3_Rb_Size : 4;
        unsigned int L1dma_Size : 4;   // 1st level DMA prefecth buffer
        unsigned int L2dma_Size : 4;   // 2nd level DMA prefecth buffer
        unsigned int Csl_L1buf_Size : 4;
    } reg;
} Reg_Pfb_Partition_Cs_Cfg;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int     uint;
    } reg;
} Reg_Csp_Ref_Total_Gpu_Timestamp;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int     uint;
    } reg;
} Reg_Csp_Ms_Total_Busy_Time;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int     uint;
    } reg;
} Reg_Csp_Ms_Query_Occlusion;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int     uint;
    } reg;
} Reg_Csp_Ms_Total_Gpu_Timestamp;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int     uint;
    } reg;
} Reg_Ia_Vertices_Cnt;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int     uint;
    } reg;
} Reg_Ia_Primitives_Cnt;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int     uint;
    } reg;
} Reg_Csp_Skip_1;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Ufence_Total_Num : 8;   // The total number of UAV FE/BE fence that each FFC can hold in
                                             // local fifo.
        unsigned int Reserved : 24;
    } reg;
} Reg_U_Fence_Num_Ctrl;

#define Reg_IA_OGL_CUT_FLAG_32_Ogl_Cut_Flag_32_BitField_MaxValue ((1u << 32) - 1)

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Ogl_Cut_Flag_32 : 32;  // Cut Flag for IB(only for triangle list/strip,others will
                                            // be patched by sw)
    } reg;
} Reg_Ia_Ogl_Cut_Flag_32;

#define Reg_IA_VB0_OFFSET_Offset_BitField_MaxValue ((1u << 32) - 1)

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Offset : 32;
    } reg;
} Reg_Ia_Vb0_Offset;

#define Reg_IA_VB0_STRIDE_Stride_BitField_MaxValue ((1u << 32) - 1)

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Stride : 32;
    } reg;
} Reg_Ia_Vb0_Stride;

#define Reg_IA_BUFFERFILLEDSIZE_Bufferfilledsize_BitField_MaxValue ((1u << 32) - 1)

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Bufferfilledsize : 32;  // it's the byte size
    } reg;
} Reg_Ia_Bufferfilledsize;

#define Reg_IA_BATCH_CFG_Batchprimnum_BitField_MaxValue ((1u << 32) - 1)

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Batchprimnum : 32;  // how many primitives per batch, CSP will append batch end
                                         // token once meet this boundary, GPCP will separate
                                         // different batch to different GPC
    } reg;
} Reg_Ia_Batch_Cfg;

#define Reg_PREDICATE_STATUS_Predicate_En_BitField_MaxValue ((1u << 1) - 1)
#define Reg_PREDICATE_STATUS_Reserved_BitField_MaxValue ((1u << 31) - 1)

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Predicate_En : 1;   // set by the set_object cmd
        unsigned int Reserved : 31;
    } reg;
} Reg_Predicate_Status;

#define Reg_CSP_MISC_CONTROL_Triangle_Cut_Index_Enable_BitField_MaxValue ((1u << 1) - 1)
#define Reg_CSP_MISC_CONTROL_Gs_On_BitField_MaxValue ((1u << 1) - 1)
#define Reg_CSP_MISC_CONTROL_Ila_Mode_BitField_MaxValue ((1u << 4) - 1)
#define Reg_CSP_MISC_CONTROL_Fe_Cnt_Disable_BitField_MaxValue ((1u << 1) - 1)
#define Reg_CSP_MISC_CONTROL_Ila_Gpc_Sel_BitField_MaxValue ((1u << 2) - 1)
#define Reg_CSP_MISC_CONTROL_Leading_Vtx_BitField_MaxValue ((1u << 1) - 1)
#define Reg_CSP_MISC_CONTROL_Dump_3d_Signature_Zero_BitField_MaxValue ((1u << 1) - 1)
#define Reg_CSP_MISC_CONTROL_Flat_Quad_BitField_MaxValue ((1u << 1) - 1)
#define Reg_CSP_MISC_CONTROL_Reserved1_BitField_MaxValue ((1u << 12) - 1)
#define Reg_CSP_MISC_CONTROL_Reserved2_BitField_MaxValue ((1u << 4) - 1)
#define Reg_CSP_MISC_CONTROL_Gpc_Ila_Mode_BitField_MaxValue ((1u << 4) - 1)
#define Reg_CSP_MISC_CONTROL_Cmu_Ila_Mode_BitField_MaxValue ((1u << 4) - 1)

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Triangle_Cut_Index_Enable : 1;   // Whether enable the cut index for triangle strip/list or
                                                      // not.
        unsigned int Gs_On : 1;   // Whether GS on for adj primitives.
        unsigned int Ila_Mode : 4;   // ILA mode selector
        unsigned int Fe_Cnt_Disable : 1;   // Fe cnt disable or not, default to 0
        unsigned int Ila_Gpc_Sel : 2;   // ILA dump slice select to indicate which GPC the current
                                        // group of ILA counter corresponding to.
        unsigned int Provokemode : 1;   // OGL provoke mode for flat rendering. For CSP, only used for
                                        // quad split.
        unsigned int Dump_3d_Signature_Zero : 1;   // When it is 1, dump SG/D/Z signature with 0. Else, dump as
                                                   // their correct value. This is used for FPGA validation, to
                                                   // avoid change other environment part.
        unsigned int Flat_Quad : 1;   // Whether the rendering mode for current quad list/strip is
                                      // flat or not.
        unsigned int Reserved1 : 20;
    } reg;
    struct
    {
        unsigned int Triangle_Cut_Index_Enable : 1;  // Whether enable the cut index for triangle strip/list or
                                                     // not.
        unsigned int Gs_On : 1;  // Whether GS on for adj primitives.
        unsigned int Reserved2 : 4;
        unsigned int Fe_Cnt_Disable : 1;  // Fe cnt disable or not, default to 0
        unsigned int Ila_Gpc_Sel : 2;  // ILA dump slice select to indicate which GPC the current
                                                     // group of ILA counter corresponding to.
        unsigned int Leading_Vtx : 1;  // Used to decide how to choose the leading vtx when flat
                                                     // shading. For DX/Vulkan, only has first leading vtx mode;
                                                     // for OGL, there are first and last leading mode. For CSP,
                                                     // only cares the "leading_vtx" when
                                                     // P_Type=TriangleFan/Quadlist/Quadstrip/Polygon. For other
                                                     // P_Type, not care which vtx would be choosed by raster as
                                                     // leading one.
        unsigned int Dump_3d_Signature_Zero : 1;  // When it is 1, dump SG/D/Z signature with 0. Else, dump
                                                     // as their correct value. This is used for FPGA validation,
                                                     // to avoid change other environment part.
        unsigned int Flat_Quad : 1;  // Whether the rendering mode for current quad list/strip
                                                     // is flat or not.
        unsigned int Reserved1 : 12;
        unsigned int Gpc_Ila_Mode : 4;  // ILA mode for each GPC
        unsigned int Cmu_Ila_Mode : 4;  // ILA mode for CMU part
    } reg_CNE001;
} Reg_Csp_Misc_Control;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Render_Mode_Switch_En     : 1;
        unsigned int Render_Mode               : 1;   // 0: IMR. 1: TBR. Driver set this bit to tell HW current render
                                                      // mode.
        unsigned int Render_Irq_Mode           : 1;   // 0: continuous mode. TBR/IMR event match counter increase
                                                      // when meet condition and reset to 0 when any frame not meet
                                                      // the condition. 1: ratio mode. TBR/IMR event match counter
                                                      // increase in a certain window and check the accumulated
                                                      // value in window boundary.BCI send IRQ when achieve REF
        unsigned int Event_Counter_Reset       : 1;   // Driver can set this bit when change render mode to reset
                                                      // event counter and frame counter, clear by HW
        unsigned int Frame_Begin               : 1;   // Driver set this bit when frame begin, cleared by HW
        unsigned int Frame_End                 : 1;   // Driver set this bit when frame end, clear by HW
        unsigned int Reserved1                 : 10;
        unsigned int Frame_Check_Window        : 16;  // Driver config. Used as frame window when Render_irq_mode
                                                      // is set as ratio mode.
    } reg;
} Reg_Tbr_Render_Mode_Ctrl;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Frametimestamp_Low_Threshold : 32;
                                                      // this counter is a reference value, when FrameTimeStamp <
                                                      // LTH,  means we have enough performance margin for tile
                                                      // rendering. Used when current status is IMR,
    } reg;
} Reg_Tbr_Frametimestamp_Lth;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Frametimestamp_High_Threshold : 32;
                                                      // This is a reference value, when FrameTimeStamp > HTH,?
                                                      // means we don't have enough performance margin for tile
                                                      // rendering, need switch to IMR. Used when current status is
                                                      // TBR
    } reg;
} Reg_Tbr_Frametimestamp_Hth;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Traffic_Threshold         : 32;  // this counter could be set as a reference value, when
                                                      // TrafficCounter >TRAFFIC_TH, that means we don't have
                                                      // enough performance margin for tile rendering, need to
                                                      // switch to IMR
    } reg;
} Reg_Tbr_Traffic_Th;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Tbr_Irq_Ref               : 16;  // HW send TBR IRQ request , only when tbr_event_match_cnt is
                                                      // larger than this reference value
        unsigned int Imr_Irq_Ref               : 16;  // HW send TBR IRQ request , only when imr_event_match_cnt is
                                                      // larger than this reference value
    } reg;
} Reg_Tbr_Event_Count_Ref;

#define Reg_INDEX_BUF_FMT_ENUM_Enum_Ib8_BitField_MaxValue ((1u << 10) - 1)
#define Reg_INDEX_BUF_FMT_ENUM_Enum_Ib16_BitField_MaxValue ((1u << 10) - 1)
#define Reg_INDEX_BUF_FMT_ENUM_Enum_Ib32_BitField_MaxValue ((1u << 10) - 1)
#define Reg_INDEX_BUF_FMT_ENUM_Reserved_BitField_MaxValue ((1u << 2) - 1)

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Enum_Ib8 : 10;  // enum value in DX for R8_UINT
        unsigned int Enum_Ib16 : 10;  // enum value in DX for R16_UINT
        unsigned int Enum_Ib32 : 10;  // enum value in DX for R32_UINT
        unsigned int Reserved : 2;
    } reg;
} Reg_Index_Buf_Fmt_Enum;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int     uint;
    } reg;
} Reg_Ia_Input_Vertices_Cnt;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int     uint;
    } reg;
} Reg_Ia_Output_Vertices_Cnt;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int     uint;
    } reg;
} Reg_Base_Addr;

typedef struct _Group_Descriptor_Base_Addr
{
    Reg_Base_Addr                    reg_Base_Addr;
} Reg_Descriptor_Base_Addr_Group;

#define Reg_FENCE_COUNTER_Counter0_BitField_MaxValue ((1u << 16) - 1)
#define Reg_FENCE_COUNTER_Counter1_BitField_MaxValue ((1u << 16) - 1)

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Counter0 : 16;
        unsigned int Counter1 : 16;
    } reg;
} Reg_Fence_Counter;

typedef struct _Group_Csp_Fence_Counter
{
    Reg_Fence_Counter                reg_Fence_Counter;
} Reg_Csp_Fence_Counter_Group;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int     uint;
    } reg;
} Reg_Ila_Counters;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int     uint;
    } reg;
} Reg_Sg_Sig_Low;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int     uint;
    } reg;
} Reg_Sg_Sig_High;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int     uint;
    } reg;
} Reg_D_Sig_Low_S0;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int     uint;
    } reg;
} Reg_D_Sig_High_S0;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int     uint;
    } reg;
} Reg_D_Sig_Low_S1;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int     uint;
    } reg;
} Reg_D_Sig_High_S1;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int     uint;
    } reg;
} Reg_D_Sig_Low_S2;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int     uint;
    } reg;
} Reg_D_Sig_High_S2;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int     uint;
    } reg;
} Reg_D_Sig_Low_S3;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int     uint;
    } reg;
} Reg_D_Sig_High_S3;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int     uint;
    } reg;
} Reg_Z_Sig_Low_S0;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int     uint;
    } reg;
} Reg_Z_Sig_High_S0;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int     uint;
    } reg;
} Reg_Z_Sig_Low_S1;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int     uint;
    } reg;
} Reg_Z_Sig_High_S1;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int     uint;
    } reg;
} Reg_Z_Sig_Low_S2;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int     uint;
    } reg;
} Reg_Z_Sig_High_S2;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int     uint;
    } reg;
} Reg_Z_Sig_Low_S3;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int     uint;
    } reg;
} Reg_Z_Sig_High_S3;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int     uint;
    } reg;
} Reg_U_Sig_Low_S0;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int     uint;
    } reg;
} Reg_U_Sig_High_S0;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int     uint;
    } reg;
} Reg_U_Sig_Low_S1;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int     uint;
    } reg;
} Reg_U_Sig_High_S1;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int     uint;
    } reg;
} Reg_U_Sig_Low_S2;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int     uint;
    } reg;
} Reg_U_Sig_High_S2;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int     uint;
    } reg;
} Reg_U_Sig_Low_S3;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int     uint;
    } reg;
} Reg_U_Sig_High_S3;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int     uint;
    } reg;
} Reg_Eu_Sig_Low_S0;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int     uint;
    } reg;
} Reg_Eu_Sig_High_S0;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int     uint;
    } reg;
} Reg_Eu_Sig_Low_S1;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int     uint;
    } reg;
} Reg_Eu_Sig_High_S1;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int     uint;
    } reg;
} Reg_Eu_Sig_Low_S2;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int     uint;
    } reg;
} Reg_Eu_Sig_High_S2;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int     uint;
    } reg;
} Reg_Eu_Sig_Low_S3;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int     uint;
    } reg;
} Reg_Eu_Sig_High_S3;

typedef struct _Group_Gpc_Signature
{
    Reg_Sg_Sig_Low                   reg_Sg_Sig_Low;
    Reg_Sg_Sig_High                  reg_Sg_Sig_High;
    Reg_D_Sig_Low_S0                 reg_D_Sig_Low_S0;
    Reg_D_Sig_High_S0                reg_D_Sig_High_S0;
    Reg_D_Sig_Low_S1                 reg_D_Sig_Low_S1;
    Reg_D_Sig_High_S1                reg_D_Sig_High_S1;
    Reg_D_Sig_Low_S2                 reg_D_Sig_Low_S2;
    Reg_D_Sig_High_S2                reg_D_Sig_High_S2;
    Reg_D_Sig_Low_S3                 reg_D_Sig_Low_S3;
    Reg_D_Sig_High_S3                reg_D_Sig_High_S3;
    Reg_Z_Sig_Low_S0                 reg_Z_Sig_Low_S0;
    Reg_Z_Sig_High_S0                reg_Z_Sig_High_S0;
    Reg_Z_Sig_Low_S1                 reg_Z_Sig_Low_S1;
    Reg_Z_Sig_High_S1                reg_Z_Sig_High_S1;
    Reg_Z_Sig_Low_S2                 reg_Z_Sig_Low_S2;
    Reg_Z_Sig_High_S2                reg_Z_Sig_High_S2;
    Reg_Z_Sig_Low_S3                 reg_Z_Sig_Low_S3;
    Reg_Z_Sig_High_S3                reg_Z_Sig_High_S3;
    Reg_U_Sig_Low_S0                 reg_U_Sig_Low_S0;
    Reg_U_Sig_High_S0                reg_U_Sig_High_S0;
    Reg_U_Sig_Low_S1                 reg_U_Sig_Low_S1;
    Reg_U_Sig_High_S1                reg_U_Sig_High_S1;
    Reg_U_Sig_Low_S2                 reg_U_Sig_Low_S2;
    Reg_U_Sig_High_S2                reg_U_Sig_High_S2;
    Reg_U_Sig_Low_S3                 reg_U_Sig_Low_S3;
    Reg_U_Sig_High_S3                reg_U_Sig_High_S3;
    Reg_Eu_Sig_Low_S0                reg_Eu_Sig_Low_S0;
    Reg_Eu_Sig_High_S0               reg_Eu_Sig_High_S0;
    Reg_Eu_Sig_Low_S1                reg_Eu_Sig_Low_S1;
    Reg_Eu_Sig_High_S1               reg_Eu_Sig_High_S1;
    Reg_Eu_Sig_Low_S2                reg_Eu_Sig_Low_S2;
    Reg_Eu_Sig_High_S2               reg_Eu_Sig_High_S2;
    Reg_Eu_Sig_Low_S3                reg_Eu_Sig_Low_S3;
    Reg_Eu_Sig_High_S3               reg_Eu_Sig_High_S3;
} Reg_Gpc_Signature_Group;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int     uint;
    } reg;
} Reg_Sto_Sig_Low;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int     uint;
    } reg;
} Reg_Sto_Sig_High;

typedef struct _Group_Sto_Signature
{
    Reg_Sto_Sig_Low                  reg_Sto_Sig_Low;
    Reg_Sto_Sig_High                 reg_Sto_Sig_High;
} Reg_Sto_Signature_Group;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Slice_Mask : 16;  // valid mask for each 4 slices in the 2 GPC.
        unsigned int Reserved1 : 16;
    } reg;
} Reg_Csp_Slice_Status;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int     uint;
    } reg;
} Reg_Cur_L1_Dma_Cmd;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int     uint;
    } reg;
} Reg_Cur_L2_Dma_Cmd;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int     uint;
    } reg;
} Reg_Cur_3d_Rbuf_Cmd;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int     uint;
    } reg;
} Reg_Cur_3d_Hrbuf_Cmd;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int     uint;
    } reg;
} Reg_Cur_Csh_Rbuf_Cmd;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int     uint;
    } reg;
} Reg_Cur_Cs0_Rbuf_Cmd;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int     uint;
    } reg;
} Reg_Cur_Cs1_Rbuf_Cmd;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int     uint;
    } reg;
} Reg_Cur_Cs2_Rbuf_Cmd;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int     uint;
    } reg;
} Reg_Cur_Cs3_Rbuf_Cmd;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int     uint;
    } reg;
} Reg_Csp_Skip_3;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Dvfs_3d_Auto_En : 1;   // CFG if 3D AUTO DVFS is enabled
        unsigned int Dvfs_Vcp0_Auto_En : 1;   // CFG if VCP0 AUTO DVFS is enabled
        unsigned int Dvfs_Vcp1_Auto_En : 1;   // CFG if VCP1 AUTO DVFS is enabled
        unsigned int Dvfs_Vpp_Auto_En : 1;   // CFG if VPP AUTO DVFS is enabled
        unsigned int Reg_Memory_Dvfs_En : 1;   // Whether recognize MIU busy as the condition that judge
                                               // decreasing ECLK
        unsigned int Reserved : 27;  // reserved bit
    } reg;
} Reg_Reg_Dvfs_Cfg_En;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Dvfs_3d_Cnt_Mode : 2;   // different DVFS workload cnt or busy cnt
        unsigned int Dvfs_3d_Cnt_Mux : 1;   // select which mode,can be used.
        unsigned int Dvfs_3d_Output_Mode : 2;   // DVFS output mode
        unsigned int Dvfs_3d_M2m_Dvfs_En : 1;   // 3D DVFS memory mode enable
        unsigned int Dvfs_3d_M2e_Ratio : 3;   // 3D DVFS memory mode ratio
        unsigned int Dvfs_Vcp0_M2m_Dvfs_En : 1;   // VCP0 DVFS memory mode enable
        unsigned int Dvfs_Vcp0_M2e_Ratio : 3;   // VCP0 DVFS memory mode threshold
        unsigned int Dvfs_Vcp0_Output_Mode : 2;   // VCP0 DVFS memory mode enable
        unsigned int Dvfs_Vcp1_M2m_Dvfs_En : 1;   // VCP1 DVFS memory mode enable
        unsigned int Dvfs_Vcp1_M2e_Ratio : 3;   // VCP1 DVFS memory mode threshold
        unsigned int Dvfs_Vcp1_Output_Mode : 2;   // VCP0 DVFS memory mode enable
        unsigned int Dvfs_Vpp_M2m_Dvfs_En : 1;   // VPP DVFS memory mode enable
        unsigned int Dvfs_Vpp_M2e_Ratio : 3;   // VPP DVFS memory mode threshold
        unsigned int Dvfs_Vpp_Output_Mode : 2;   // VCP0 DVFS memory mode enable
        unsigned int Reserved : 5;   // reserved bit
    } reg;
} Reg_Reg_Dvfs_Cfg_Misc0;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Check_Size_3d : 32;
    } reg;
} Reg_Reg_3d_Dvfs_Checksize;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int High_Workload_Threshold_3d : 32;
    } reg;
} Reg_Reg_3d_Dvfs_H_Th;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Low_Workload_Threshold_3d : 32;
    } reg;
} Reg_Reg_3d_Dvfs_L_Th;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Mem_Busy_Threshold_3d : 32;
    } reg;
} Reg_Reg_3d_Dvfs_H_Th_Mem;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Alu_Workload_3d : 32;
    } reg;
} Reg_Reg_3d_Alu_Workload;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Busy_Workload_3d : 32;
    } reg;
} Reg_Reg_3d_Busy_Workload;
typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Check_Size_Vcp0 : 32;
    } reg;
} Reg_Reg_Vcp0_Dvfs_Checksize;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int High_Workload_Threshold_Vcp0 : 32;
    } reg;
} Reg_Reg_Vcp0_Dvfs_H_Th;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Low_Workload_Threshold_Vcp0 : 32;
    } reg;
} Reg_Reg_Vcp0_Dvfs_L_Th;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Mem_Busy_Threshold_Vcp0 : 32;
    } reg;
} Reg_Reg_Vcp0_Dvfs_H_Th_Mem;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Vcp0_Busy_Cnt : 32;
    } reg;
} Reg_Reg_Vcp0_Busy_Cycle;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Check_Size_Vcp1 : 32;
    } reg;
} Reg_Reg_Vcp1_Dvfs_Checksize;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int High_Workload_Threshold_Vcp1 : 32;
    } reg;
} Reg_Reg_Vcp1_Dvfs_H_Th;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Low_Workload_Threshold_Vcp1 : 32;
    } reg;
} Reg_Reg_Vcp1_Dvfs_L_Th;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Mem_Busy_Threshold_Vcp1 : 32;
    } reg;
} Reg_Reg_Vcp1_Dvfs_H_Th_Mem;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Vcp1_Busy_Cnt : 32;
    } reg;
} Reg_Reg_Vcp1_Busy_Cycle;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Check_Size_Vpp : 32;
    } reg;
} Reg_Reg_Vpp_Dvfs_Checksize;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int High_Workload_Threshold_Vpp : 32;
    } reg;
} Reg_Reg_Vpp_Dvfs_H_Th;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Low_Workload_Threshold_Vpp : 32;
    } reg;
} Reg_Reg_Vpp_Dvfs_L_Th;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Mem_Busy_Threshold_Vpp : 32;
    } reg;
} Reg_Reg_Vpp_Dvfs_H_Th_Mem;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Vpp_Busy_Cnt : 32;
    } reg;
} Reg_Reg_Vpp_Busy_Cycle;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Reg_Pcu_3d_Dvfs_En : 1;
        unsigned int Reg_Force_3d_Inc_Dfvs : 1;   // SW force to increase DVFS, trigger to increase once. The
                                                  // bit is cleared once the force done. Can be read by SW to check
                                                  // if force increase done.
        unsigned int Reg_Force_3d_Dec_Dfvs : 1;   // SW force to decrease DVFS, trigger to decrease once. The
                                                  // bit is cleared once the force done. Can be read by SW to check
                                                  // if force decrease done.
        unsigned int Reg_Pcu_Vcp0_Dvfs_En : 1;
        unsigned int Reg_Force_Vcp0_Inc_Dfvs : 1;   // SW force to increase DVFS, trigger to increase once. The
                                                    // bit is cleared once the force done. Can be read by SW to check
                                                    // if force increase done.
        unsigned int Reg_Force_Vcp0_Dec_Dfvs : 1;   // SW force to decrease DVFS, trigger to decrease once. The
                                                    // bit is cleared once the force done. Can be read by SW to check
                                                    // if force decrease done.
        unsigned int Reg_Pcu_Vcp1_Dvfs_En : 1;
        unsigned int Reg_Force_Vcp1_Inc_Dfvs : 1;   // SW force to increase DVFS, trigger to increase once. The
                                                    // bit is cleared once the force done. Can be read by SW to check
                                                    // if force increase done.
        unsigned int Reg_Force_Vcp1_Dec_Dfvs : 1;   // SW force to decrease DVFS, trigger to decrease once. The
                                                    // bit is cleared once the force done. Can be read by SW to check
                                                    // if force decrease done.
        unsigned int Reg_Pcu_Vpp_Dvfs_En : 1;
        unsigned int Reg_Force_Vpp_Inc_Dfvs : 1;   // SW force to increase DVFS, trigger to increase once. The
                                                   // bit is cleared once the force done. Can be read by SW to check
                                                   // if force increase done.
        unsigned int Reg_Force_Vpp_Dec_Dfvs : 1;   // SW force to decrease DVFS, trigger to decrease once. The
                                                   // bit is cleared once the force done. Can be read by SW to check
                                                   // if force decrease done.
        unsigned int Reserved : 20;  // reserved bit
    } reg;
} Reg_Reg_Dvfs_Pcu_Cfg_En;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Memory_Busy_Cnt : 32;
    } reg;
} Reg_Reg_3d_Mem_Busy_Cycle;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Memory_Busy_Cnt : 32;
    } reg;
} Reg_Reg_Vcp0_Mem_Busy_Cycle;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Memory_Busy_Cnt : 32;
    } reg;
} Reg_Reg_Vcp1_Mem_Busy_Cycle;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Memory_Busy_Cnt : 32;
    } reg;
} Reg_Reg_Vpp_Mem_Busy_Cycle;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int En_Clkgate_3d_Central : 1;   // config enable clcok gating of 3D
        unsigned int En_Clkgate_3d_Slc0 : 1;   // config enable clcok gating of Slice0
        unsigned int En_Clkgate_3d_Slc1 : 1;   // config enable clcok gating of Slice1
        unsigned int En_Clkgate_S3vd0 : 1;   // config enable clcok gating of S3VD0
        unsigned int En_Clkgate_S3vd1 : 1;   // config enable clcok gating of S3VD1
        unsigned int En_Clkgate_Vpp : 1;   // config enable clcok gating of VPP
        unsigned int En_Clkgate_Mxu0 : 1;   // config enable clcok gating of MXU0
        unsigned int En_Pwrgate_3d_Central : 1;   // config enable clcok gating of MXU0
        unsigned int En_Pwrgate_3d_Slc0 : 1;   // config enable clcok gating of Slice0
        unsigned int En_Pwrgate_3d_Slc1 : 1;   // config enable clcok gating of Slice1
        unsigned int En_Pwrgate_S3vd0 : 1;   // config enable clcok gating of S3VD0
        unsigned int En_Pwrgate_S3vd1 : 1;   // config enable clcok gating of S3VD1
        unsigned int En_Pwrgate_Vpp : 1;   // config enable clcok gating of VPP
        unsigned int Reserved : 16;  // reserved bit
        unsigned int Fast_Boot_En : 1;   // When task comes, enter power up process immediately, even
                                         // if power down not finished.
        unsigned int Be_Mode : 1;   // Need power gating signal feed back to make sure if power
                                    // gating done.
        unsigned int Grout_Reset_Enable : 1;   // Need reset every partition when any partition reset
    } reg;
    struct
    {
        unsigned int En_Clkgate_3d_Central : 1;  // config enable clcok gating of 3D
        unsigned int En_Clkgate_3d_Slc0 : 1;  // config enable clcok gating of Slice0
        unsigned int En_Clkgate_3d_Slc1 : 1;  // config enable clcok gating of Slice1
        unsigned int En_Clkgate_Vcp : 1;  // config enable clcok gating of VCP
        unsigned int En_Clkgate_Vdp0 : 1;  // config enable clcok gating of VDP0
        unsigned int En_Clkgate_Vdp1 : 1;  // config enable clcok gating of VDP1
        unsigned int En_Clkgate_Vdp2 : 1;  // config enable clcok gating of VDP2
        unsigned int En_Clkgate_Vpp0 : 1;  // config enable clcok gating of VPP0
        unsigned int En_Clkgate_Vpp1 : 1;  // config enable clcok gating of VPP1
        unsigned int En_Clkgate_Mxu0 : 1;  // config enable clcok gating of MXU0
        unsigned int En_Pwrgate_3d_Central : 1;  // config enable clcok gating of MXU0
        unsigned int En_Pwrgate_3d_Slc0 : 1;  // config enable clcok gating of Slice0
        unsigned int En_Pwrgate_3d_Slc1 : 1;  // config enable clcok gating of Slice1
        unsigned int En_Pwrgate_Vcp : 1;  // config enable clcok gating of VCP
        unsigned int En_Pwrgate_Vdp0 : 1;  // config enable clcok gating of VDP0
        unsigned int En_Pwrgate_Vdp1 : 1;  // config enable clcok gating of VDP1
        unsigned int En_Pwrgate_Vdp2 : 1;  // config enable clcok gating of VDP2
        unsigned int En_Pwrgate_Vpp0 : 1;  // config enable clcok gating of VPP0
        unsigned int En_Pwrgate_Vpp1 : 1;  // config enable clcok gating of VPP1
        unsigned int Reserved : 10;  // reserved bit
        unsigned int Fast_Boot_En : 1;  // When task comes, enter power up process immediately,
                                                        // even if power down not finished.
        unsigned int Be_Mode : 1;  // Need power gating signal feed back to make sure if power
                                                        // gating done.
        unsigned int Grout_Reset_Enable : 1;  // Need reset every partition when any partition reset
    } reg_cne001;
} Reg_Pwr_Mgr_Cfg_En;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Cnt_Pd_Isoon_Pwroff_3d : 8;   // Cycles for isolation on to power down in 3D
        unsigned int Cnt_Pd_Isoon_Pwroff_S3vd : 8;    // Cycles for isolation on to power down in s3vd
        unsigned int Cnt_Pd_Isoon_Pwroff_Vpp : 8;   // Cycles for isolation on to power down in vpp
        unsigned int Reserved : 8;   // reserved bit
    } reg;
} Reg_Pwr_Mgr_Pg_Pd_Misc0;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Cnt_Pd_Complete_3d : 16;  // cycle for 3D power dow finish << 4
        unsigned int Cnt_Pd_Complete_S3vd : 16;  // cycle for s3vd power dow finish << 4
    } reg;
    struct
    {
        unsigned int Cnt_Pd_Complete_Vpp : 16;  // cycle for s3vd power down finish << 4
        unsigned int Cnt_Neg_Pulse_Width : 16;  // The width constrain of the output negative pulse for
                                                // 3D/VCDP/VPP power down process when fast boot happens in
                                                // BE mode (For SI consideration) Default: 'd16 Recommended:
                                                // It's an experiment value.
    } reg_cne001;
} Reg_Pwr_Mgr_Pg_Pd_Misc1;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Cnt_Pd_Complete_Vpp : 16;  // cycle for s3vd power dow finish << 4
        unsigned int Reserved : 16;  // reserved bit
    } reg;
} Reg_Pwr_Mgr_Pg_Pd_Misc2;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Cnt_Wakeup_Rston_3d : 16;  // Cycles for 3D  power up finished, before reset on, will be
                                                // used in by CNT_PD_COMPLETE << 4
        unsigned int Cnt_Wakeup_Rston_S3vd : 16;  // Cycles for s3vd  power up finished, before reset on, will be
                                                  // used in by CNT_PD_COMPLETE << 4
    } reg;
} Reg_Pwr_Mgr_Pg_Pu_Misc0;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Cnt_Wakeup_Rston_Vpp : 16;  // Cycles for s3vd  power up finished, before reset on, will be
                                                 // used in by CNT_PD_COMPLETE << 4
        unsigned int Reserved : 16;  // reserved bit
    } reg;
} Reg_Pwr_Mgr_Pg_Pu_Misc1;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Cnt_Rston_Clkon : 8;   // Cycles for reset on to clock on.
                                            //  Default : 4
                                            //  Recommended : 0~31
        unsigned int Cnt_Clkon_Rstoff : 8;   // Cycles for clock on to reset off. Maximum cycles will be
                                             // 256.
                                             //  Default : 4
                                             //  Recommended : 0~31
        unsigned int Cnt_Rstoff_Isooff : 8;   // Cycles for reset off to isolation off. Maximum cycles will
                                              // be 256.
                                              //  Default : 4
                                              //  Recommended : 0~31
        unsigned int Cnt_Isooff_Ready : 8;   // Cycles for isolation off to ready. Maximum cycles will be
                                             // 256.
                                             //  Default : 4
                                             //  Recommended : 0~31
    } reg;
} Reg_Pwr_Mgr_Pg_Pu_Misc2;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Cg_Wait_Cnt : 16;
        unsigned int Pg_Wait_Cnt : 16;
    } reg;
} Reg_Pwr_Mgr_Wait_3d_Cnt0;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Cg_Wait_Cnt : 16;
        unsigned int Pg_Wait_Cnt : 16;
    } reg;
} Reg_Pwr_Mgr_Wait_S3vd_Cnt1;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Wait_Cnt : 16;
        unsigned int Pg_Wait_Cnt : 16;
    } reg;
} Reg_Pwr_Mgr_Wait_Vpp_Cnt2;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Wait_Cnt : 16;
        unsigned int Reserved : 16;  // reserved bit
    } reg;
} Reg_Pwr_Mgr_Wait_Mxu0_Cnt3;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Sw_Ctl_3d_En : 1;   // sw config enable clcok gating of central
        unsigned int Sw_Ctl_S3vd0_En : 1;   // sw config enable clcok gating of slice0
        unsigned int Sw_Ctl_S3vd1_En : 1;   // sw config enable clcok gating of slice1
        unsigned int Sw_Ctl_Vpp_En : 1;   // sw config enable clcok gating of s3vd0
        unsigned int Sw_Ctl_Mxu0_En : 1;   // sw config enable clcok gating of mxu0
        unsigned int Sw_Boot_3d : 1;   // sw boot 3D engine
        unsigned int Sw_Boot_S3vd0 : 1;   // sw boot S3VD0 engine
        unsigned int Sw_Boot_S3vd1 : 1;   // sw boot S3VD1 engine
        unsigned int Sw_Boot_Vpp : 1;   // sw boot VPP engine
        unsigned int Sw_Boot_Mxu0 : 1;   // sw boot VPP engine
        unsigned int Reserved : 22;  // reserved bit
    } reg;
    struct
    {
        unsigned int Sw_Ctl_3d_En : 1;   // sw config enable clcok gating of central
        unsigned int Sw_Ctl_Vcp_En : 1;   // sw config enable clcok gating of vcp
        unsigned int Sw_Ctl_Vdp0_En : 1;   // sw config enable clcok gating of vdp0
        unsigned int Sw_Ctl_Vdp1_En : 1;   // sw config enable clcok gating of vdp1
        unsigned int Sw_Ctl_Vdp2_En : 1;   // sw config enable clcok gating of vdp2
        unsigned int Sw_Ctl_Vpp0_En : 1;   // sw config enable clcok gating of vpp0
        unsigned int Sw_Ctl_Vpp1_En : 1;   // sw config enable clcok gating of vpp1
        unsigned int Sw_Ctl_Mxu0_En : 1;   // sw config enable clcok gating of mxu0
        unsigned int Sw_Boot_3d : 1;   // sw boot 3D engine
        unsigned int Sw_Boot_Vcp : 1;   // sw boot Vcp engine
        unsigned int Sw_Boot_Vdp0 : 1;   // sw boot Vdp0 engine
        unsigned int Sw_Boot_Vdp1 : 1;   // sw boot Vdp1 engine
        unsigned int Sw_Boot_Vdp2 : 1;   // sw boot Vdp2 engine
        unsigned int Sw_Boot_Vpp0 : 1;   // sw boot VPP0 engine
        unsigned int Sw_Boot_Vpp1 : 1;   // sw boot VPP1 engine
        unsigned int Sw_Boot_Mxu0 : 1;   // sw boot mxu0 engine
        unsigned int Reserved : 16;  // reserved bit
    } reg_cne001;
} Reg_Pwr_Mgr_Sw_Cfg_En;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Sw_Ctl_3d_Centroy_Clken : 1;   // SW enable 3D clock
        unsigned int Sw_Ctl_3d_Slc0_Clken : 1;   // SW enable slice0 clock
        unsigned int Sw_Ctl_3d_Slc1_Clken : 1;   // SW enable slice1 clock
        unsigned int Sw_Ctl_S3vd0_Clken : 1;   // SW enable s3vd0 clock
        unsigned int Sw_Ctl_S3vd1_Clken : 1;   // SW enable s3vd1 clock
        unsigned int Sw_Ctl_Vpp_Clken : 1;   // SW enable vpp clock
        unsigned int Sw_Ctl_Mxu0_Clken : 1;   // SW enable vpp clock
        unsigned int Sw_Ctl_3d_Centroy_Isoon : 1;   // SW enable 3D isolation
        unsigned int Sw_Ctl_3d_Slc0_Isoon : 1;   // SW enable slice0 isolation
        unsigned int Sw_Ctl_3d_Slc1_Isoon : 1;   // SW enable slice1 isolation
        unsigned int Sw_Ctl_S3vd0_Isoon : 1;   // SW enable s3vd0 isolation
        unsigned int Sw_Ctl_S3vd1_Isoon : 1;   // SW enable s3vd1 isolation
        unsigned int Sw_Ctl_Vpp_Isoon : 1;   // SW enable vpp isolation
        unsigned int Sw_Ctl_3d_Centroy_Rston : 1;   // SW enable 3D reset
        unsigned int Sw_Ctl_3d_Slc0_Rston : 1;   // SW enable slice0 reset
        unsigned int Sw_Ctl_3d_Slc1_Rston : 1;   // SW enable slice1 reset
        unsigned int Sw_Ctl_S3vd0_Rston : 1;   // SW enable s3vd0 reset
        unsigned int Sw_Ctl_S3vd1_Rston : 1;   // SW enable s3vd1 reset
        unsigned int Sw_Ctl_Vpp_Rston : 1;   // SW enable vpp reset
        unsigned int Sw_Ctl_3d_Centroy_Pwron : 1;   // SW enable 3D power
        unsigned int Sw_Ctl_3d_Slc0_Pwron : 1;   // SW enable slice0 power
        unsigned int Sw_Ctl_3d_Slc1_Pwron : 1;   // SW enable slice1 power
        unsigned int Sw_Ctl_S3vd0_Pwron : 1;   // SW enable s3vd0 power
        unsigned int Sw_Ctl_S3vd1_Pwron : 1;   // SW enable s3vd1 power
        unsigned int Sw_Ctl_Vpp_Pwron : 1;   // SW enable vpp power
        unsigned int Reserved : 7;   // reserved bit
    } reg;
    struct
    {
        unsigned int Sw_Ctl_3d_Centroy_Clken : 1;   // SW enable 3D clock
        unsigned int Sw_Ctl_3d_Slc0_Clken : 1;   // SW enable slice0 clock
        unsigned int Sw_Ctl_3d_Slc1_Clken : 1;   // SW enable slice1 clock
        unsigned int Sw_Ctl_Vcp_Clken : 1;   // SW enable vcp clock
        unsigned int Sw_Ctl_Vdp0_Clken : 1;   // SW enable vdp0 clock
        unsigned int Sw_Ctl_Vdp1_Clken : 1;   // SW enable vdp1 clock
        unsigned int Sw_Ctl_Vdp2_Clken : 1;   // SW enable vdp2 clock
        unsigned int Sw_Ctl_Vpp0_Clken : 1;   // SW enable vpp0 clock
        unsigned int Sw_Ctl_Vpp1_Clken : 1;   // SW enable vpp1 clock
        unsigned int Sw_Ctl_Mxu0_Clken : 1;   // SW enable vpp clock
        unsigned int Sw_Ctl_3d_Centroy_Isoon : 1;   // SW enable 3D isolation
        unsigned int Sw_Ctl_3d_Slc0_Isoon : 1;   // SW enable slice0 isolation
        unsigned int Sw_Ctl_3d_Slc1_Isoon : 1;   // SW enable slice1 isolation
        unsigned int Sw_Ctl_Vcp_Isoon : 1;   // SW enable vcp isolation
        unsigned int Sw_Ctl_Vdp0_Isoon : 1;   // SW enable vdp0 isolation
        unsigned int Sw_Ctl_Vdp1_Isoon : 1;   // SW enable vdp1 isolation
        unsigned int Sw_Ctl_Vdp2_Isoon : 1;   // SW enable vdp2 isolation
        unsigned int Sw_Ctl_Vpp0_Isoon : 1;   // SW enable vpp0 isolation
        unsigned int Sw_Ctl_Vpp1_Isoon : 1;   // SW enable vpp1 isolation
        unsigned int Sw_Ctl_3d_Centroy_Rston : 1;   // SW enable 3D reset
        unsigned int Sw_Ctl_3d_Slc0_Rston : 1;   // SW enable slice0 reset
        unsigned int Sw_Ctl_3d_Slc1_Rston : 1;   // SW enable slice1 reset
        unsigned int Sw_Ctl_Vcp_Rston : 1;   // SW enable vcp reset
        unsigned int Sw_Ctl_Vdp0_Rston : 1;   // SW enable vdp0 reset
        unsigned int Sw_Ctl_Vdp1_Rston : 1;   // SW enable vdp1 reset
        unsigned int Sw_Ctl_Vdp2_Rston : 1;   // SW enable vdp2 reset
        unsigned int Sw_Ctl_Vpp0_Rston : 1;   // SW enable vpp0 reset
        unsigned int Sw_Ctl_Vpp1_Rston : 1;   // SW enable vpp1 reset
        unsigned int Reserved : 4;   // reserved bit
    } reg_cne001;
} Reg_Pwr_Mgr_Sw_Cfg0;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Sw_Ctl_3d_Centroy_Pwron : 1;   // SW enable 3D power
        unsigned int Sw_Ctl_3d_Slc0_Pwron : 1;   // SW enable slice0 power
        unsigned int Sw_Ctl_3d_Slc1_Pwron : 1;   // SW enable slice1 power
        unsigned int Sw_Ctl_Vcp_Pwron : 1;   // SW enable vcp power
        unsigned int Sw_Ctl_Vdp0_Pwron : 1;   // SW enable vdp0 power
        unsigned int Sw_Ctl_Vdp1_Pwron : 1;   // SW enable vdp1 power
        unsigned int Sw_Ctl_Vdp2_Pwron : 1;   // SW enable vdp2 power
        unsigned int Sw_Ctl_Vpp0_Pwron : 1;   // SW enable vpp0 power
        unsigned int Sw_Ctl_Vpp1_Pwron : 1;   // SW enable vpp1 power
        unsigned int Reserved : 23;   // reserved bit
    } reg_cne001;
} Reg_Pwr_Mgr_Sw_Cfg1;
typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Csp_Pwm_L2_Clken : 1;   // L2 clock status
        unsigned int Csp_Pwm_Pool_Clken : 1;   // pool clock status
        unsigned int Csp_Pwm_Sgtbe_Clken : 1;   // sgtbe clock status
        unsigned int Csp_Pwm_Sptfe_Clken : 1;   // sptfe clock status
        unsigned int Csp_Pwm_Euc0_Clken : 1;   // slice0 euc clock status
        unsigned int Csp_Pwm_Ffu0_Clken : 1;   // slice0 ffu clock status
        unsigned int Csp_Pwm_Tu0_Clken : 1;   // slice0 tu clock status
        unsigned int Csp_Pwm_Pea0_Clken : 1;   // slice0 pe0 clock status
        unsigned int Csp_Pwm_Peb0_Clken : 1;   // slice0 pe1 clock status
        unsigned int Csp_Pwm_Euc1_Clken : 1;   // slice1 euc clock status
        unsigned int Csp_Pwm_Ffu1_Clken : 1;   // slice1 ffu clock status
        unsigned int Csp_Pwm_Tu1_Clken : 1;   // slice1 tu clock status
        unsigned int Csp_Pwm_Pea1_Clken : 1;   // slice1 pe0 clock status
        unsigned int Csp_Pwm_Peb1_Clken : 1;   // slice1 pe1 clock status
        unsigned int Csp_Pwm_S3vd0_Clken : 1;   // vcp0 clock status
        unsigned int Csp_Pwm_S3vd1_Clken : 1;   // vcp1 clock status
        unsigned int Csp_Pwm_Vpp_Clken : 1;   // vpp clock status
        unsigned int Csp_Pwm_Mxu0_Clken : 1;   // mxu0 clock status
        unsigned int Csp_Pwm_L2_Isoon : 1;   // l2 isolation status
        unsigned int Csp_Pwm_Pool_Isoon : 1;   // pool isolation status
        unsigned int Csp_Pwm_Sgtbe_Isoon : 1;   // sgtbe isolation status
        unsigned int Csp_Pwm_Sptfe_Isoon : 1;   // sptfe isolation status
        unsigned int Csp_Pwm_Euc0_Isoon : 1;   // slice0 euc isolation status
        unsigned int Csp_Pwm_Ffu0_Isoon : 1;   // slice0 ffu isolation status
        unsigned int Csp_Pwm_Tu0_Isoon : 1;   // slice0 tu isolation status
        unsigned int Csp_Pwm_Pea0_Isoon : 1;   // slice0 pe0 isolation status
        unsigned int Csp_Pwm_Peb0_Isoon : 1;   // slice0 pe1 isolation status
        unsigned int Csp_Pwm_Euc1_Isoon : 1;   // slice1 euc isolation status
        unsigned int Csp_Pwm_Ffu1_Isoon : 1;   // slice1 ffu isolation status
        unsigned int Csp_Pwm_Tu1_Isoon : 1;   // slice1 tu isolation status
        unsigned int Csp_Pwm_Pea1_Isoon : 1;   // slice1 pe0 isolation status
        unsigned int Csp_Pwm_Peb1_Isoon : 1;   // slice1 pe1 isolation status
    } reg;
    struct
    {
        unsigned int Csp_Pwm_L2_Clken : 1;   // L2 clock status
        unsigned int Csp_Pwm_Pool_Clken : 1;   // pool clock status
        unsigned int Csp_Pwm_Sgtbe_Clken : 1;   // sgtbe clock status
        unsigned int Csp_Pwm_Sptfe_Clken : 1;   // sptfe clock status
        unsigned int Csp_Pwm_Euc0_Clken : 1;   // slice0 euc clock status
        unsigned int Csp_Pwm_Ffu0_Clken : 1;   // slice0 ffu clock status
        unsigned int Csp_Pwm_Tu0_Clken : 1;   // slice0 tu clock status
        unsigned int Csp_Pwm_Pea0_Clken : 1;   // slice0 pe0 clock status
        unsigned int Csp_Pwm_Peb0_Clken : 1;   // slice0 pe1 clock status
        unsigned int Csp_Pwm_Euc1_Clken : 1;   // slice1 euc clock status
        unsigned int Csp_Pwm_Ffu1_Clken : 1;   // slice1 ffu clock status
        unsigned int Csp_Pwm_Tu1_Clken : 1;   // slice1 tu clock status
        unsigned int Csp_Pwm_Pea1_Clken : 1;   // slice1 pe0 clock status
        unsigned int Csp_Pwm_Peb1_Clken : 1;   // slice1 pe1 clock status
        unsigned int Csp_Pwm_Mxu0_Clken : 1;   // mxu0 clock status
        unsigned int Csp_Pwm_L2_Isoon : 1;   // l2 isolation status
        unsigned int Csp_Pwm_Pool_Isoon : 1;   // pool isolation status
        unsigned int Csp_Pwm_Sgtbe_Isoon : 1;   // sgtbe isolation status
        unsigned int Csp_Pwm_Sptfe_Isoon : 1;   // sptfe isolation status
        unsigned int Csp_Pwm_Euc0_Isoon : 1;   // slice0 euc isolation status
        unsigned int Csp_Pwm_Ffu0_Isoon : 1;   // slice0 ffu isolation status
        unsigned int Csp_Pwm_Tu0_Isoon : 1;   // slice0 tu isolation status
        unsigned int Csp_Pwm_Pea0_Isoon : 1;   // slice0 pe0 isolation status
        unsigned int Csp_Pwm_Peb0_Isoon : 1;   // slice0 pe1 isolation status
        unsigned int Csp_Pwm_Euc1_Isoon : 1;   // slice1 euc isolation status
        unsigned int Csp_Pwm_Ffu1_Isoon : 1;   // slice1 ffu isolation status
        unsigned int Csp_Pwm_Tu1_Isoon : 1;   // slice1 tu isolation status
        unsigned int Csp_Pwm_Pea1_Isoon : 1;   // slice1 pe0 isolation status
        unsigned int Csp_Pwm_Peb1_Isoon : 1;   // slice1 pe1 isolation status
        unsigned int Csp_Pwm_L2_Rston : 1;   // l2 reset status
        unsigned int Csp_Pwm_Pool_Rston : 1;   // pool reset status
        unsigned int Csp_Pwm_Sgtbe_Rston : 1;   // sgtbe reset status
    } reg_cne001;
} Reg_Pwr_Mgr_Status0;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Csp_Pwm_S3vd0_Isoon : 1;   // vcp0 isolation status    0
        unsigned int Csp_Pwm_S3vd1_Isoon : 1;   // vcp1 isolation status
        unsigned int Csp_Pwm_Vpp_Isoon : 1;   // vpp isolation status
        unsigned int Csp_Pwm_L2_Rston : 1;   // l2 reset status
        unsigned int Csp_Pwm_Pool_Rston : 1;   // pool reset status         4
        unsigned int Csp_Pwm_Sgtbe_Rston : 1;   // sgtbe reset status
        unsigned int Csp_Pwm_Sptfe_Rston : 1;   // sptfe reset status
        unsigned int Csp_Pwm_Euc0_Rston : 1;   // slice0 euc reset status
        unsigned int Csp_Pwm_Ffu0_Rston : 1;   // slice0 ffu reset status   8
        unsigned int Csp_Pwm_Tu0_Rston : 1;   // slice0 tu reset status
        unsigned int Csp_Pwm_Pea0_Rston : 1;   // slice0 pe0 reset status
        unsigned int Csp_Pwm_Peb0_Rston : 1;   // slice0 pe1 reset status
        unsigned int Csp_Pwm_Euc1_Rston : 1;   // slice1 euc reset status   12
        unsigned int Csp_Pwm_Ffu1_Rston : 1;   // slice1 ffu reset status
        unsigned int Csp_Pwm_Tu1_Rston : 1;   // slice1 tu reset status
        unsigned int Csp_Pwm_Pea1_Rston : 1;   // slice1 pe0 reset status
        unsigned int Csp_Pwm_Peb1_Rston : 1;   // slice1 pe1 reset status   16
        unsigned int Csp_Pwm_S3vd0_Rston : 1;   // vcp0 reset status
        unsigned int Csp_Pwm_S3vd1_Rston : 1;   // vcp1 reset status
        unsigned int Csp_Pwm_Vpp_Rston : 1;   // vpp isolation status
        unsigned int Csp_Pwm_Central_Pwron : 1;   // central partition power status 20
        unsigned int Csp_Pwm_Slc0_Pwron : 1;   // slice0 power status
        unsigned int Csp_Pwm_Slc1_Pwron : 1;   // slice1 power status
        unsigned int Csp_Pwm_S3vd0_Pwron : 1;   // vcp0 power status
        unsigned int Csp_Pwm_S3vd1_Pwron : 1;   // vcp1 pwoer status        24
        unsigned int Csp_Pwm_Vpp_Pwron : 1;   // vpp power status
        unsigned int Csp_Pwm_Central_Pwron_Be : 1;    // central partition power status
        unsigned int Csp_Pwm_Slc0_Pwron_Be : 1;   // slice0 power status
        unsigned int Csp_Pwm_Slc1_Pwron_Be : 1;   // slice1 power status    28
        unsigned int Csp_Pwm_S3vd0_Pwron_Be : 1;   // vcp0 power status
        unsigned int Csp_Pwm_S3vd1_Pwron_Be : 1;   // vcp1 pwoer status
        unsigned int Csp_Pwm_Vpp_Pwron_Be : 1;   // vpp power status
    } reg;
    struct
    {
        unsigned int Csp_Pwm_Sptfe_Rston : 1;   // sptfe reset status       0
        unsigned int Csp_Pwm_Euc0_Rston : 1;   // slice0 euc reset status
        unsigned int Csp_Pwm_Ffu0_Rston : 1;   // slice0 ffu reset status
        unsigned int Csp_Pwm_Tu0_Rston : 1;   // slice0 tu reset status
        unsigned int Csp_Pwm_Pea0_Rston : 1;   // slice0 pe0 reset status   4
        unsigned int Csp_Pwm_Peb0_Rston : 1;   // slice0 pe1 reset status
        unsigned int Csp_Pwm_Euc1_Rston : 1;   // slice1 euc reset status
        unsigned int Csp_Pwm_Ffu1_Rston : 1;   // slice1 ffu reset status
        unsigned int Csp_Pwm_Tu1_Rston : 1;   // slice1 tu reset status     8
        unsigned int Csp_Pwm_Pea1_Rston : 1;   // slice1 pe0 reset status
        unsigned int Csp_Pwm_Peb1_Rston : 1;   // slice1 pe1 reset status
        unsigned int Csp_Pwm_Central_Pwron : 1;   // central partition power status
        unsigned int Csp_Pwm_Slc0_Pwron : 1;   // slice0 power status       12
        unsigned int Csp_Pwm_Slc1_Pwron : 1;   // slice1 power status
        unsigned int Csp_Pwm_Central_Pwron_Be : 1;    // central partition power status
        unsigned int Csp_Pwm_Slc0_Pwron_Be : 1;   // slice0 power status
        unsigned int Csp_Pwm_Slc1_Pwron_Be : 1;   // slice1 power status    16
        unsigned int Reserved : 15;  // reserved bit
    } reg_cne001;
} Reg_Pwr_Mgr_Status1;
typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Csp_Pwm_Vcp_Clken : 1;   // vcp clock status       0
        unsigned int Csp_Pwm_Vdp0_Clken : 1;   // vdp0 clock status
        unsigned int Csp_Pwm_Vdp1_Clken : 1;   // vdp1 clock status
        unsigned int Csp_Pwm_Vdp2_Clken : 1;   // vdp2 clock status
        unsigned int Csp_Pwm_Vpp0_Clken : 1;   // vpp0 clock status     4
        unsigned int Csp_Pwm_Vpp1_Clken : 1;   // vpp1 clock status
        unsigned int Csp_Pwm_Vcp_Isoon : 1;   // vcp isolation status
        unsigned int Csp_Pwm_Vdp0_Isoon : 1;   // vdp0 isolation status
        unsigned int Csp_Pwm_Vdp1_Isoon : 1;   // vdp1 isolation status 8
        unsigned int Csp_Pwm_Vdp2_Isoon : 1;   // vdp2 isolation status
        unsigned int Csp_Pwm_Vpp0_Isoon : 1;   // vpp0 isolation status
        unsigned int Csp_Pwm_Vpp1_Isoon : 1;   // vpp1 isolation status
        unsigned int Csp_Pwm_Vcp_Pwron : 1;   // vcp power status       12
        unsigned int Csp_Pwm_Vdp0_Pwron : 1;   // vdp0 power status
        unsigned int Csp_Pwm_Vdp1_Pwron : 1;   // vdp1 power status
        unsigned int Csp_Pwm_Vdp2_Pwron : 1;   // vdp2 power status
        unsigned int Csp_Pwm_Vpp0_Pwron : 1;   // vpp0 power status     16
        unsigned int Csp_Pwm_Vpp1_Pwron : 1;   // vpp1 power status
        unsigned int Csp_Pwm_Vcp_Pwron_Be : 1;   // vcp power be status
        unsigned int Csp_Pwm_Vdp0_Pwron_Be : 1;   // vdp0 power be status
        unsigned int Csp_Pwm_Vdp1_Pwron_Be : 1;   // vdp1 power be status   20
        unsigned int Csp_Pwm_Vdp2_Pwron_Be : 1;   // vdp2 power be status
        unsigned int Csp_Pwm_Vpp0_Pwron_Be : 1;   // vpp0 power be status
        unsigned int Csp_Pwm_Vpp1_Pwron_Be : 1;   // vpp1 power be status
        unsigned int Csp_Pwm_Vcp_Rston : 1;   // vcp reset status           24
        unsigned int Csp_Pwm_Vdp0_Rston : 1;   // vdp0 reset status
        unsigned int Csp_Pwm_Vdp1_Rston : 1;   // vdp1 reset status
        unsigned int Csp_Pwm_Vdp2_Rston : 1;   // vdp2 reset status
        unsigned int Csp_Pwm_Vpp0_Rston : 1;   // vpp0 reset status         28
        unsigned int Csp_Pwm_Vpp1_Rston : 1;   // vpp1 reset status
        unsigned int Reserved : 2;  // reserved bit
    } reg_cne001;
} Reg_Pwr_Mgr_Status2;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int     uint;
    } reg;
} Reg_Csp_Skip_4;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Ts_En : 1;   // config enable clcok gating of 3D
        unsigned int Ts_Mode_Sel : 1;   // 0 is current mode,1 : voltage mode
        unsigned int Reg_Ts_H_Th : 10;  // when tempature is higher than this value,send a alarm
        unsigned int Reg_Ts_D2_Sel : 8;   // select which D2_X is used to replace D2
        unsigned int Tm_Selx : 6;   // Mux Select Signal from TM_XV<8:0> vVSUS Voltage or 3
                                    // tmsenses 'Voltage Mode' and 'Current Mode' Sensor vbe
                                    // Volt
        unsigned int Tm_Clkout_Edge_Sel : 1;   // tm_clkout Edge Select : 0 : tm_clkout is posedge;1 :
                                               // tm_clkout is negedge
        unsigned int Tm_Bg_Pdb : 1;   // Bandgap Enable
        unsigned int Tm_Clksel : 3;   // Select Internal Osc Clock Signal for Sigma Delta
                                      // Convertor0: 000       clk<0> 1MHz;001       clk<1> 2MHz;010       clk<2>
                                      // 4MHz;100       clk<4> 16MHz;101       clk<5> 32MHz; 110  0; 111  0.
                                      // Default value = 0
        unsigned int Rtm_Step3_Once : 1;   // Do Step 3 Once after Power on,When set to 1, controller will
                                           // only run step 3 after power on.
    } reg;
} Reg_Pwr_Ts_Cfg_En;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Ts_Level0 : 10;  // tempature : 0~ ts _level0 value
        unsigned int Ts_Level1 : 10;  // tempature : ts_level0~ ts _level1 value
        unsigned int Ts_Level2 : 10;  // tempature : ts_level1~ ts _level2 value
        unsigned int Reserved : 2;   // reserved bit
    } reg;
} Reg_Pwr_Ts_Cfg_Misc0;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Ts_Level3 : 10;  // tempature : ts_level2~ ts _level3 value
        unsigned int Ts_Level4 : 10;  // tempature : ts_level3~ ts _level4 value
        unsigned int Reserved : 12;  // reserved bit
    } reg;
} Reg_Pwr_Ts_Cfg_Misc1;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Ts_C0 : 20;  // calculate tempature coefficient
        unsigned int Reserved : 12;
    } reg;
} Reg_Pwr_Ts_Cfg_Misc2;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Ts_C1 : 20;  // calculate tempature coefficient
        unsigned int Reserved : 12;
    } reg;
} Reg_Pwr_Ts_Cfg_Misc3;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Ts_C2 : 20;  // calculate tempature coefficient
        unsigned int Reserved : 12;
    } reg;
} Reg_Pwr_Ts_Cfg_Misc4;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Ts_C3 : 20;  // calculate tempature coefficient
        unsigned int Reserved : 12;
    } reg;
} Reg_Pwr_Ts_Cfg_Misc5;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Ts_C4 : 20;  // calculate tempature coefficient
        unsigned int Reserved : 12;
    } reg;
} Reg_Pwr_Ts_Cfg_Misc6;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Tm_Clkout_Cnt : 20;  // TM_CLKOUT Counter
                                          //  It's the counter used to count the TM_CLKOUT. The detail
                                          // usage can refer description in TM_DDR specification.
                                          //  Default value = 20'h7D0 (decimal: 2000)
        unsigned int Tm_Gl : 7;   // Sigma Delta Convertor Internal Integrator Cap Value
                                  // Select
        unsigned int Reserved : 5;
    } reg;
} Reg_Pwr_Ts_Cfg_Misc7;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int     uint;
    } reg;
} Reg_Csp_Skip_5;
typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int     uint;
    } reg;
} Reg_Csp_Skip_6;


typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int     uint;
    } reg;
} Reg_C3d_Eng_Workload;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int     uint;
    } reg;
} Reg_C3d_Eng_Alu_Workload;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int     uint;
    } reg;
} Reg_C3d_Mem_Workload;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int     uint;
    } reg;
} Reg_Vpp_Workload;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int     uint;
    } reg;
} Reg_Vcp0_Workload;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int     uint;
    } reg;
} Reg_Vcp1_Workload;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int     uint;
    } reg;
} Reg_Csp_Skip_2;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int     uint;
    } reg;
} Reg_Csp_Skip0;

#define Reg_VCP_RUN_LIST_CTX_ADDR_Addr_BitField_MaxValue ((1u << 28) - 1)
#define Reg_VCP_RUN_LIST_CTX_ADDR_Reserved_BitField_MaxValue ((1u << 3) - 1)
#define Reg_VCP_RUN_LIST_CTX_ADDR_Kickoff_BitField_MaxValue ((1u << 1) - 1)

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Addr : 28;  // For 4KB alignedRun List Context Base Address, need 40-12
                                 // bits.
        unsigned int Reserved : 3;
        unsigned int Kickoff : 1;   // whether the RB is kick off
    } reg;
} Reg_Vcp_Run_List_Ctx_Addr;

#define Reg_VCP_RING_BUF_SIZE_Rb_Size_BitField_MaxValue ((1u << 32) - 1)

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Rb_Size : 32;  // RB size
    } reg;
} Reg_Vcp_Ring_Buf_Size;

#define Reg_VCP_RING_BUF_HEAD_Rb_Head_BitField_MaxValue ((1u << 32) - 1)

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Rb_Head : 32;  // RB Current Head Pointer, DW offset
    } reg;
} Reg_Vcp_Ring_Buf_Head;

#define Reg_VCP_RING_BUF_TAIL_Rb_Tail_BitField_MaxValue ((1u << 32) - 1)

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Rb_Tail : 32;  // RB Current Tail Pointer, DW offset
    } reg;
} Reg_Vcp_Ring_Buf_Tail;

typedef struct _Group_Vcp_Ring_Buf
{
    Reg_Vcp_Run_List_Ctx_Addr        reg_Vcp_Run_List_Ctx_Addr;
    Reg_Vcp_Ring_Buf_Size            reg_Vcp_Ring_Buf_Size;
    Reg_Vcp_Ring_Buf_Head            reg_Vcp_Ring_Buf_Head;
    Reg_Vcp_Ring_Buf_Tail            reg_Vcp_Ring_Buf_Tail;
} Reg_Vcp_Ring_Buf_Group;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int     uint;
    } reg;
} Reg_Csp_Reserved_Vcp3;

#define Reg_VPP_RUN_LIST_CTX_ADDR_Addr_BitField_MaxValue ((1u << 28) - 1)
#define Reg_VPP_RUN_LIST_CTX_ADDR_Reserved_BitField_MaxValue ((1u << 3) - 1)
#define Reg_VPP_RUN_LIST_CTX_ADDR_Kickoff_BitField_MaxValue ((1u << 1) - 1)

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Addr : 28;  // For 4KB alignedRun List Context Base Address, need 40-12
                                 // bits.
        unsigned int Reserved : 3;
        unsigned int Kickoff : 1;   // whether the RB is kick off
    } reg;
} Reg_Vpp_Run_List_Ctx_Addr;

#define Reg_VPP_RING_BUF_SIZE_Rb_Size_BitField_MaxValue ((1u << 32) - 1)

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Rb_Size : 32;  // RB size
    } reg;
} Reg_Vpp_Ring_Buf_Size;

#define Reg_VPP_RING_BUF_HEAD_Rb_Head_BitField_MaxValue ((1u << 32) - 1)

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Rb_Head : 32;  // RB Current Head Pointer, DW offset
    } reg;
} Reg_Vpp_Ring_Buf_Head;

#define Reg_VPP_RING_BUF_TAIL_Rb_Tail_BitField_MaxValue ((1u << 32) - 1)

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Rb_Tail : 32;  // RB Current Tail Pointer, DW offset
    } reg;
} Reg_Vpp_Ring_Buf_Tail;

typedef struct _Group_Vpp_Ring_Buf
{
    Reg_Vpp_Run_List_Ctx_Addr        reg_Vpp_Run_List_Ctx_Addr;
    Reg_Vpp_Ring_Buf_Size            reg_Vpp_Ring_Buf_Size;
    Reg_Vpp_Ring_Buf_Head            reg_Vpp_Ring_Buf_Head;
    Reg_Vpp_Ring_Buf_Tail            reg_Vpp_Ring_Buf_Tail;
} Reg_Vpp_Ring_Buf_Group;

#define Reg_VCP_VPP_BLOCK_BUSY_BITS_Vcp0_Busy_BitField_MaxValue ((1u << 1) - 1)
#define Reg_VCP_VPP_BLOCK_BUSY_BITS_Vcp1_Busy_BitField_MaxValue ((1u << 1) - 1)
#define Reg_VCP_VPP_BLOCK_BUSY_BITS_Vpp_Busy_BitField_MaxValue ((1u << 1) - 1)
#define Reg_VCP_VPP_BLOCK_BUSY_BITS_Reserved_BitField_MaxValue ((1u << 29) - 1)

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Vcp0_Busy : 1;
        unsigned int Vcp1_Busy : 1;
        unsigned int Vcp2_Busy : 1;
        unsigned int Vcp3_Busy : 1;
        unsigned int Vpp0_Busy : 1;
        unsigned int Vpp1_Busy : 1;
        unsigned int Reserved : 26;
    } reg;
} Reg_Vcp_Vpp_Block_Busy_Bits;

#define Reg_CMODEL_DMA_PREEMPTION_BEGIN_Begin_BitField_MaxValue ((1u << 1) - 1)

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Begin : 1;
        unsigned int Reserved : 31;
    } reg;
} Reg_Cmodel_Dma_Preemption_Begin;

#define Reg_CMODEL_DMA_PREEMPTION_CNT_Dma_Preempt_En_BitField_MaxValue ((1u << 1) - 1)
#define Reg_CMODEL_DMA_PREEMPTION_CNT_Indicator_Off_Cnt_BitField_MaxValue ((1u << 31) - 1)

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Dma_Preempt_En : 1;  // whether test DMA preemption or not
        unsigned int Indicator_Off_Cnt : 31;  // to indicate HW begin to do DMA preeemption at which
                                              // indicator off cmd
    } reg;
} Reg_Cmodel_Dma_Preemption_Cnt;

#define Reg_CMODEL_DMA_PREEMPTION_TAIL_Tailpoint_Skipto_BitField_MaxValue ((1u << 32) - 1)

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Tailpoint_Skipto : 32;  // to indicate where to skip to in HW side.
    } reg;
} Reg_Cmodel_Dma_Preemption_Tail;

#define Reg_CMODEL_HL_SWITCH_Cmd_Type_BitField_MaxValue ((1u << 3) - 1)
#define Reg_CMODEL_HL_SWITCH_Cmd_Cnt_BitField_MaxValue ((1u << 5) - 1)
#define Reg_CMODEL_HL_SWITCH_Draw_Cnt_BitField_MaxValue ((1u << 5) - 1)
#define Reg_CMODEL_HL_SWITCH_Reserved_BitField_MaxValue ((1u << 19) - 1)

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Cmd_Type : 3;
        unsigned int Cmd_Cnt : 5;
        unsigned int Draw_Cnt : 5;
        unsigned int Reserved : 19;
    } reg;
} Reg_Cmodel_Hl_Switch;

#define Reg_EU_DBG_CFG_Exec_En_BitField_MaxValue ((1u << 1) - 1)
#define Reg_EU_DBG_CFG_Th_Mode_BitField_MaxValue ((1u << 1) - 1)
#define Reg_EU_DBG_CFG_Int_En_BitField_MaxValue ((1u << 1) - 1)
#define Reg_EU_DBG_CFG_Res_En_BitField_MaxValue ((1u << 1) - 1)
#define Reg_EU_DBG_CFG_Dbg_Th_Id_BitField_MaxValue ((1u << 5) - 1)
#define Reg_EU_DBG_CFG_Reserved_BitField_MaxValue ((1u << 23) - 1)

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Exec_En : 1;   // Debug Thread execution enable per PE
        unsigned int Th_Mode : 1;   // Thread debugging execution mode
        unsigned int Int_En : 1;   // Interrupt routine enable
        unsigned int Res_En : 1;   // Resume routine enable
        unsigned int Dbg_Th_Id : 5;   // 5bits Hardware thread ID in PE, that be executed,
                                      // set(2)+slot(3). It is enabled when Th_Mode is single.
                                      // Resume shader need use it to run.
        unsigned int Reserved : 23;  // Reserved
    } reg;
} Reg_Eu_Dbg_Cfg;

#define Reg_EU_DBG_BP_STAT_Bp_Hit_BitField_MaxValue ((1u << 1) - 1)
#define Reg_EU_DBG_BP_STAT_Bp_Id_BitField_MaxValue ((1u << 3) - 1)
#define Reg_EU_DBG_BP_STAT_Hit_Th_Id_BitField_MaxValue ((1u << 5) - 1)
#define Reg_EU_DBG_BP_STAT_Reserved_BitField_MaxValue ((1u << 23) - 1)

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Bp_Hit : 1;   // Thread hits breakpoints for 32 THCs in E3k, if BP happens,
                                   // the THC need set its Bp_Hit as 1.
        unsigned int Bp_Id : 3;   // Breakpoint ID that gets hit
        unsigned int Hit_Th_Id : 5;   // 5bits Hardware thread ID in PE that hits breakpoint,
                                      // set(2)+slot(3)
        unsigned int Reserved : 23;  // Reserved
    } reg;
} Reg_Eu_Dbg_Bp_Stat;

#define Reg_EU_DBG_BP_PC_Bp_Valid_BitField_MaxValue ((1u << 1) - 1)
#define Reg_EU_DBG_BP_PC_Shader_BitField_MaxValue ((1u << 3) - 1)
#define Reg_EU_DBG_BP_PC_Offset_BitField_MaxValue ((1u << 16) - 1)
#define Reg_EU_DBG_BP_PC_Reserved_BitField_MaxValue ((1u << 12) - 1)

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Bp_Valid : 1;   // Breakpoint slot valid or not. 8 BP slot in total.
        unsigned int Shader : 3;   // Which shader type has BP offset
        unsigned int Offset : 16;  // Breakpoint PC offset, instruction (dword, 128bits)
                                   // based, The EU hardware needs to calculate the PC address
                                   // from this offset and base PC address.
        unsigned int Reserved : 12;  // Reserved
    } reg;
} Reg_Eu_Dbg_Bp_Pc;

#define Reg_EU_DBG_INT_INSTR_Offset_BitField_MaxValue ((1u << 16) - 1)
#define Reg_EU_DBG_INT_INSTR_Reserved_BitField_MaxValue ((1u << 16) - 1)

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Offset : 16;  // Instruction range of Debug Int shader codes (0 ~ (16K-1)).
                                   // Each instr occupies 4Dw (128bits)
        unsigned int Reserved : 16;  // Reserved
    } reg;
} Reg_Eu_Dbg_Int_Instr;

#define Reg_EU_DBG_RES_INSTR_Offset_BitField_MaxValue ((1u << 16) - 1)
#define Reg_EU_DBG_RES_INSTR_Reserved_BitField_MaxValue ((1u << 16) - 1)

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Offset : 16;  //  Debug Resume Shader Instruction offset in main shader.
                                   // 128its-instr aligned.
        unsigned int Reserved : 16;  // Reserved
    } reg;
} Reg_Eu_Dbg_Res_Instr;

#define Reg_EU_DBG_TIME_STAMP_Value_BitField_MaxValue ((1u << 32) - 1)

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Value : 32;  // CSP 64bits time stamp, CSP write it into MMIO register and
                                  // then THC write them into SRFs by MOVIMM instr
    } reg;
} Reg_Eu_Dbg_Time_Stamp;

#define Reg_EU_DBG_REVERSED_Reserved_BitField_MaxValue ((1u << 32) - 1)

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Reserved : 32;  // Reserved
    } reg;
} Reg_Eu_Dbg_Reversed;

typedef struct _Group_Eu_Dbg_Reg
{
    Reg_Eu_Dbg_Cfg                   reg_Eu_Dbg_Cfg;
    Reg_Eu_Dbg_Bp_Stat               reg_Eu_Dbg_Bp_Stat;
    Reg_Eu_Dbg_Bp_Pc                 reg_Eu_Dbg_Bp_Pc[8];
    Reg_Eu_Dbg_Int_Instr             reg_Eu_Dbg_Int_Instr;
    Reg_Eu_Dbg_Res_Instr             reg_Eu_Dbg_Res_Instr;
    Reg_Eu_Dbg_Time_Stamp            reg_Eu_Dbg_Time_Stamp[2];
    Reg_Eu_Dbg_Reversed              reg_Eu_Dbg_Reversed[2];
} Reg_Eu_Dbg_Reg_Group;

typedef struct _Csp_Global_regs_CHX004
{
    Reg_Block_Busy_Bits_Top          reg_Block_Busy_Bits_Top;
    Reg_Block_Busy_Bits_Gpc0_0       reg_Block_Busy_Bits_Gpc0_0;
    Reg_Block_Busy_Bits_Gpc0_1       reg_Block_Busy_Bits_Gpc0_1;
    Reg_Block_Busy_Bits_Gpc1_0       reg_Block_Busy_Bits_Gpc1_0;
    Reg_Block_Busy_Bits_Gpc1_1       reg_Block_Busy_Bits_Gpc1_1;
    Reg_Block_Busy_Bits_Gpc2_0       reg_Block_Busy_Bits_Gpc2_0;
    Reg_Block_Busy_Bits_Gpc2_1       reg_Block_Busy_Bits_Gpc2_1;
    Reg_Ring_Buf_Group               reg_Ring_Buf[7];
    Reg_Preempt_Cmd_Process          reg_Preempt_Cmd_Process;
    Reg_Cpu_Set                      reg_Cpu_Set;
    Reg_Fence_Mask                   reg_Fence_Mask;
    Reg_Trans_Threshold_Cnt          reg_Trans_Threshold_Cnt;
    Reg_Pfb_Partition_3d_Cfg         reg_Pfb_Partition_3d_Cfg;
    Reg_Pfb_Partition_Cs_Cfg         reg_Pfb_Partition_Cs_Cfg;
    Reg_Csp_Ref_Total_Gpu_Timestamp  reg_Csp_Ref_Total_Gpu_Timestamp[2];
    Reg_Csp_Ms_Total_Busy_Time       reg_Csp_Ms_Total_Busy_Time[2];
    Reg_Csp_Ms_Query_Occlusion       reg_Csp_Ms_Query_Occlusion[2];
    Reg_Csp_Ms_Total_Gpu_Timestamp   reg_Csp_Ms_Total_Gpu_Timestamp[2];
    Reg_Csp_Skip_1                   reg_Csp_Skip_1[2];
    Reg_Ia_Ogl_Cut_Flag_32           reg_Ia_Ogl_Cut_Flag_32;
    Reg_Ia_Vb0_Offset                reg_Ia_Vb0_Offset;
    Reg_Ia_Vb0_Stride                reg_Ia_Vb0_Stride;
    Reg_Ia_Bufferfilledsize          reg_Ia_Bufferfilledsize;
    Reg_Ia_Batch_Cfg                 reg_Ia_Batch_Cfg;
    Reg_Predicate_Status             reg_Predicate_Status;
    Reg_Csp_Misc_Control             reg_Csp_Misc_Control;
    Reg_Index_Buf_Fmt_Enum           reg_Index_Buf_Fmt_Enum;
    Reg_Ia_Input_Vertices_Cnt        reg_Ia_Input_Vertices_Cnt[2];
    Reg_Ia_Output_Vertices_Cnt       reg_Ia_Output_Vertices_Cnt[2];
    Reg_Ia_Primitives_Cnt            reg_Ia_Primitives_Cnt[2];
    Reg_Csp_Skip_2                   reg_Csp_Skip_2[6];
    Reg_Csp_Fence_Counter_Group      reg_Csp_Fence_Counter[16];
    Reg_Ila_Counters                 reg_Ila_Counters[40];
    Reg_Gpc_Signature_Group          reg_Gpc_Signature[3];
    Reg_Sto_Signature_Group          reg_Sto_Signature[4];
    Reg_Csp_Slice_Status             reg_Csp_Slice_Status;
    Reg_Cur_L1_Dma_Cmd               reg_Cur_L1_Dma_Cmd;
    Reg_Cur_L2_Dma_Cmd               reg_Cur_L2_Dma_Cmd;
    Reg_Cur_3d_Rbuf_Cmd              reg_Cur_3d_Rbuf_Cmd;
    Reg_Cur_3d_Hrbuf_Cmd             reg_Cur_3d_Hrbuf_Cmd;
    Reg_Cur_Csh_Rbuf_Cmd             reg_Cur_Csh_Rbuf_Cmd;
    Reg_Cur_Cs0_Rbuf_Cmd             reg_Cur_Cs0_Rbuf_Cmd;
    Reg_Cur_Cs1_Rbuf_Cmd             reg_Cur_Cs1_Rbuf_Cmd;
    Reg_Cur_Cs2_Rbuf_Cmd             reg_Cur_Cs2_Rbuf_Cmd;
    Reg_Cur_Cs3_Rbuf_Cmd             reg_Cur_Cs3_Rbuf_Cmd;
    Reg_U_Fence_Num_Ctrl             reg_U_Fence_Num_Ctrl;
    Reg_Csp_Skip_3                   reg_Csp_Skip_3[8];
    Reg_Reg_Dvfs_Cfg_En              reg_Reg_Dvfs_Cfg_En;
    Reg_Reg_Dvfs_Cfg_Misc0           reg_Reg_Dvfs_Cfg_Misc0;
    Reg_Reg_3d_Dvfs_Checksize        reg_Reg_3d_Dvfs_Checksize;
    Reg_Reg_3d_Dvfs_H_Th             reg_Reg_3d_Dvfs_H_Th;
    Reg_Reg_3d_Dvfs_L_Th             reg_Reg_3d_Dvfs_L_Th;
    Reg_Reg_3d_Dvfs_H_Th_Mem         reg_Reg_3d_Dvfs_H_Th_Mem;
    Reg_Reg_3d_Alu_Workload          reg_Reg_3d_Alu_Workload;
    Reg_Reg_3d_Busy_Workload         reg_Reg_3d_Busy_Workload;
    Reg_Reg_Vcp0_Dvfs_Checksize      reg_Reg_Vcp0_Dvfs_Checksize;
    Reg_Reg_Vcp0_Dvfs_H_Th           reg_Reg_Vcp0_Dvfs_H_Th;
    Reg_Reg_Vcp0_Dvfs_L_Th           reg_Reg_Vcp0_Dvfs_L_Th;
    Reg_Reg_Vcp0_Dvfs_H_Th_Mem       reg_Reg_Vcp0_Dvfs_H_Th_Mem;
    Reg_Reg_Vcp0_Busy_Cycle          reg_Reg_Vcp0_Busy_Cycle;
    Reg_Reg_Vcp1_Dvfs_Checksize      reg_Reg_Vcp1_Dvfs_Checksize;
    Reg_Reg_Vcp1_Dvfs_H_Th           reg_Reg_Vcp1_Dvfs_H_Th;
    Reg_Reg_Vcp1_Dvfs_L_Th           reg_Reg_Vcp1_Dvfs_L_Th;
    Reg_Reg_Vcp1_Dvfs_H_Th_Mem       reg_Reg_Vcp1_Dvfs_H_Th_Mem;
    Reg_Reg_Vcp1_Busy_Cycle          reg_Reg_Vcp1_Busy_Cycle;
    Reg_Reg_Vpp_Dvfs_Checksize       reg_Reg_Vpp_Dvfs_Checksize;
    Reg_Reg_Vpp_Dvfs_H_Th            reg_Reg_Vpp_Dvfs_H_Th;
    Reg_Reg_Vpp_Dvfs_L_Th            reg_Reg_Vpp_Dvfs_L_Th;
    Reg_Reg_Vpp_Dvfs_H_Th_Mem        reg_Reg_Vpp_Dvfs_H_Th_Mem;
    Reg_Reg_Vpp_Busy_Cycle           reg_Reg_Vpp_Busy_Cycle;
    Reg_Reg_Dvfs_Pcu_Cfg_En          reg_Reg_Dvfs_Pcu_Cfg_En;
    Reg_Reg_3d_Mem_Busy_Cycle        reg_Reg_3d_Mem_Busy_Cycle;
    Reg_Reg_Vcp0_Mem_Busy_Cycle      reg_Reg_Vcp0_Mem_Busy_Cycle;
    Reg_Reg_Vcp1_Mem_Busy_Cycle      reg_Reg_Vcp1_Mem_Busy_Cycle;
    Reg_Reg_Vpp_Mem_Busy_Cycle       reg_Reg_Vpp_Mem_Busy_Cycle;
    Reg_Pwr_Mgr_Cfg_En               reg_Pwr_Mgr_Cfg_En;
    Reg_Pwr_Mgr_Pg_Pd_Misc0          reg_Pwr_Mgr_Pg_Pd_Misc0;
    Reg_Pwr_Mgr_Pg_Pd_Misc1          reg_Pwr_Mgr_Pg_Pd_Misc1;
    Reg_Pwr_Mgr_Pg_Pd_Misc2          reg_Pwr_Mgr_Pg_Pd_Misc2;
    Reg_Pwr_Mgr_Pg_Pu_Misc0          reg_Pwr_Mgr_Pg_Pu_Misc0;
    Reg_Pwr_Mgr_Pg_Pu_Misc1          reg_Pwr_Mgr_Pg_Pu_Misc1;
    Reg_Pwr_Mgr_Pg_Pu_Misc2          reg_Pwr_Mgr_Pg_Pu_Misc2;
    Reg_Pwr_Mgr_Wait_3d_Cnt0         reg_Pwr_Mgr_Wait_3d_Cnt0;
    Reg_Pwr_Mgr_Wait_S3vd_Cnt1       reg_Pwr_Mgr_Wait_S3vd_Cnt1;
    Reg_Pwr_Mgr_Wait_Vpp_Cnt2        reg_Pwr_Mgr_Wait_Vpp_Cnt2;
    Reg_Pwr_Mgr_Wait_Mxu0_Cnt3       reg_Pwr_Mgr_Wait_Mxu0_Cnt3;
    Reg_Pwr_Mgr_Sw_Cfg_En            reg_Pwr_Mgr_Sw_Cfg_En;
    Reg_Pwr_Mgr_Sw_Cfg0              reg_Pwr_Mgr_Sw_Cfg0;
    Reg_Pwr_Mgr_Status0              reg_Pwr_Mgr_Status0;
    Reg_Pwr_Mgr_Status1              reg_Pwr_Mgr_Status1;
    Reg_Csp_Skip_4                   reg_Csp_Skip_4[21];
    Reg_Pwr_Ts_Cfg_En                reg_Pwr_Ts_Cfg_En;
    Reg_Pwr_Ts_Cfg_Misc0             reg_Pwr_Ts_Cfg_Misc0;
    Reg_Pwr_Ts_Cfg_Misc1             reg_Pwr_Ts_Cfg_Misc1;
    Reg_Pwr_Ts_Cfg_Misc2             reg_Pwr_Ts_Cfg_Misc2;
    Reg_Pwr_Ts_Cfg_Misc3             reg_Pwr_Ts_Cfg_Misc3;
    Reg_Pwr_Ts_Cfg_Misc4             reg_Pwr_Ts_Cfg_Misc4;
    Reg_Pwr_Ts_Cfg_Misc5             reg_Pwr_Ts_Cfg_Misc5;
    Reg_Pwr_Ts_Cfg_Misc6             reg_Pwr_Ts_Cfg_Misc6;
    Reg_Pwr_Ts_Cfg_Misc7             reg_Pwr_Ts_Cfg_Misc7;
    Reg_Csp_Skip_5                   reg_Csp_Skip_5[55];
    Reg_Vcp_Ring_Buf_Group           reg_Vcp_Ring_Buf[2];
    Reg_Vpp_Ring_Buf_Group           reg_Vpp_Ring_Buf;
    Reg_Vcp_Vpp_Block_Busy_Bits      reg_Vcp_Vpp_Block_Busy_Bits;
    Reg_Csp_Skip_6                   reg_Csp_Skip_6[111];
    Reg_Cmodel_Dma_Preemption_Begin  reg_Cmodel_Dma_Preemption_Begin;
    Reg_Cmodel_Dma_Preemption_Cnt    reg_Cmodel_Dma_Preemption_Cnt;
    Reg_Cmodel_Dma_Preemption_Tail   reg_Cmodel_Dma_Preemption_Tail;
    Reg_Cmodel_Hl_Switch             reg_Cmodel_Hl_Switch;
    Reg_Eu_Dbg_Reg_Group             reg_Eu_Dbg_Reg[24];
} Csp_Global_regs_CHX004;

typedef struct _Csp_Global_regs_Elt3k
{
    Reg_Block_Busy_Bits_Top          reg_Block_Busy_Bits_Top;
    Reg_Block_Busy_Bits_Gpc0_0       reg_Block_Busy_Bits_Gpc0_0;
    Reg_Block_Busy_Bits_Gpc0_1       reg_Block_Busy_Bits_Gpc0_1;
    Reg_Block_Busy_Bits_Gpc1_0       reg_Block_Busy_Bits_Gpc1_0;
    Reg_Block_Busy_Bits_Gpc1_1       reg_Block_Busy_Bits_Gpc1_1;
    Reg_Block_Busy_Bits_Gpc2_0       reg_Block_Busy_Bits_Gpc2_0;
    Reg_Block_Busy_Bits_Gpc2_1       reg_Block_Busy_Bits_Gpc2_1;
    Reg_Ring_Buf_Group               reg_Ring_Buf[7];
    Reg_Preempt_Cmd_Process          reg_Preempt_Cmd_Process;
    Reg_Cpu_Set                      reg_Cpu_Set;
    Reg_Fence_Mask                   reg_Fence_Mask;
    Reg_Reserved_Dw_0                reg_Reserved_Dw_0;
    Reg_Pfb_Partition_3d_Cfg         reg_Pfb_Partition_3d_Cfg;
    Reg_Pfb_Partition_Cs_Cfg         reg_Pfb_Partition_Cs_Cfg;
    Reg_Csp_Ms_Total_Gpu_Timestamp   reg_Csp_Ms_Total_Gpu_Timestamp[2];
    Reg_Csp_Ms_Total_Busy_Time       reg_Csp_Ms_Total_Busy_Time[2];
    Reg_Csp_Ms_Query_Occlusion       reg_Csp_Ms_Query_Occlusion[2];
    Reg_Ia_Vertices_Cnt              reg_Ia_Vertices_Cnt[2];
    Reg_Ia_Primitives_Cnt            reg_Ia_Primitives_Cnt[2];
    Reg_Ia_Ogl_Cut_Flag_32           reg_Ia_Ogl_Cut_Flag_32;
    Reg_Ia_Vb0_Offset                reg_Ia_Vb0_Offset;
    Reg_Ia_Vb0_Stride                reg_Ia_Vb0_Stride;
    Reg_Ia_Bufferfilledsize          reg_Ia_Bufferfilledsize;
    Reg_Ia_Batch_Cfg                 reg_Ia_Batch_Cfg;
    Reg_Predicate_Status             reg_Predicate_Status;
    Reg_Csp_Misc_Control             reg_Csp_Misc_Control;
    Reg_Tbr_Render_Mode_Ctrl         reg_Tbr_Render_Mode_Ctrl;
    Reg_Tbr_Frametimestamp_Lth       reg_Tbr_Frametimestamp_Lth;
    Reg_Tbr_Frametimestamp_Hth       reg_Tbr_Frametimestamp_Hth;
    Reg_Tbr_Traffic_Th               reg_Tbr_Traffic_Th;
    Reg_Tbr_Event_Count_Ref          reg_Tbr_Event_Count_Ref;
    Reg_Descriptor_Base_Addr_Group   reg_Descriptor_Base_Addr[8];
    Reg_Csp_Fence_Counter_Group      reg_Csp_Fence_Counter[16];
    Reg_Ila_Counters                 reg_Ila_Counters[40];
    Reg_Gpc_Signature_Group          reg_Gpc_Signature[3];
    Reg_Sto_Signature_Group          reg_Sto_Signature[4];
    Reg_Csp_Slice_Status             reg_Csp_Slice_Status;
    Reg_Cur_L1_Dma_Cmd               reg_Cur_L1_Dma_Cmd;
    Reg_Cur_L2_Dma_Cmd               reg_Cur_L2_Dma_Cmd;
    Reg_Cur_3d_Rbuf_Cmd              reg_Cur_3d_Rbuf_Cmd;
    Reg_Cur_3d_Hrbuf_Cmd             reg_Cur_3d_Hrbuf_Cmd;
    Reg_Cur_Csh_Rbuf_Cmd             reg_Cur_Csh_Rbuf_Cmd;
    Reg_Cur_Cs0_Rbuf_Cmd             reg_Cur_Cs0_Rbuf_Cmd;
    Reg_Cur_Cs1_Rbuf_Cmd             reg_Cur_Cs1_Rbuf_Cmd;
    Reg_Cur_Cs2_Rbuf_Cmd             reg_Cur_Cs2_Rbuf_Cmd;
    Reg_Cur_Cs3_Rbuf_Cmd             reg_Cur_Cs3_Rbuf_Cmd;
    Reg_C3d_Eng_Workload             reg_C3d_Eng_Workload;
    Reg_C3d_Eng_Alu_Workload         reg_C3d_Eng_Alu_Workload;
    Reg_C3d_Mem_Workload             reg_C3d_Mem_Workload;
    Reg_Vpp_Workload                 reg_Vpp_Workload;
    Reg_Vcp0_Workload                reg_Vcp0_Workload;
    Reg_Vcp1_Workload                reg_Vcp1_Workload;
    Reg_Csp_Skip0                    reg_Csp_Skip0[131];
    Reg_Vcp_Ring_Buf_Group           reg_Vcp_Ring_Buf[2];
    Reg_Vpp_Ring_Buf_Group           reg_Vpp_Ring_Buf;
    Reg_Vcp_Vpp_Block_Busy_Bits      reg_Vcp_Vpp_Block_Busy_Bits;
    Reg_Csp_Skip_1                    reg_Csp_Skip1[114];
    Reg_Cmodel_Hl_Switch             reg_Cmodel_Hl_Switch;
    Reg_Eu_Dbg_Reg_Group             reg_Eu_Dbg_Reg[24];
} Csp_Global_regs_Elt3k;

#endif
