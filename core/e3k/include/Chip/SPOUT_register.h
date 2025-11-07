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
//    Spec Last Modified Time: 2018/6/11 22:15:55
#ifndef _SPOUT_REGISTER_H
#define _SPOUT_REGISTER_H


#ifndef        SPOUT_BLOCKBASE_INF
#define    SPOUT_BLOCKBASE_INF
#define    BLOCK_SPOUT_VERSION 1
#define    BLOCK_SPOUT_TIMESTAMP "2018/6/11 22:15:55"
#define    SPOUT_BLOCK                                                0x16 // match with BlockID.h
#define    SPOUT_REG_START                                            0x0 // match with BlockID.h
#define    SPOUT_REG_END                                              0x20 // match with BlockID.h
#define    SPOUT_REG_LIMIT                                            0x20 // match with BlockID.h
#endif

// Register offset definition
#define        Reg_Spout_Cfg_Offset                                       0x0
#define        Reg_Spout_Idx_Ch_Offset                                    0x1
#define        Reg_Pos_Read_Loc_Offset                                    0x2
#define        Reg_Attr_Read_Loc0_Offset                                  0x3
#define        Reg_Attr_Read_Loc1_Offset                                  0x4
#define        Reg_Pos_Read_Id_Offset                                     0x5
#define        Reg_Attr_Read_Id_Offset                                    0x6
#define        Reg_Pos_Def_Value_Offset                                   0x12
#define        Reg_Attr_Def_Value_Offset                                  0x13
#define        Reg_Spout_Reserved_Offset                                  0x1F

// Block constant definition
typedef enum
{
    SPOUT_CFG_OUT_RAST_EN_DISABLE            = 0,   // 0: Disable
    SPOUT_CFG_OUT_RAST_EN_ENABLE             = 1,   // 1: Out to TAS Enable, do Rast
} SPOUT_CFG_OUT_RAST_EN;
typedef enum
{
    SPOUT_CFG_STO_EN_DISABLED                = 0,   // 0: Disable, default mode
    SPOUT_CFG_STO_EN_ENABLED                 = 1,   // 1: Enable, VS/DS/GS may stream-out, and TAS need assembe
    // the primitives and let VC to output vertices to memory.
} SPOUT_CFG_STO_EN;
typedef enum
{
    SPOUT_CFG_RTARRAY_IDX_OUT_EN_DISABLE     = 0,   // 0: Disable, need use MvIdx to TASFE interface
    SPOUT_CFG_RTARRAY_IDX_OUT_EN_ENABLE      = 1,   // 1: Enable. need read from VC, whose channel is set in
    // RTArray_Idx_Ch register
} SPOUT_CFG_RTARRAY_IDX_OUT_EN;
typedef enum
{
    SPOUT_CFG_VIEWPORT_MASK_OUT_EN_DISABLE   = 0,   // 0: Disable, SP need set 0 to TASFE interface
    SPOUT_CFG_VIEWPORT_MASK_OUT_EN_ENABLE    = 1,   // 1: Enable. need read from VC, whose channel is set in
    // Viewport_Mask_Ch register
} SPOUT_CFG_VIEWPORT_MASK_OUT_EN;
typedef enum
{
    SPOUT_CFG_CULLCLIP_FLAG_OUT_EN_DISABLE   = 0,   // 0: Disable, SP need set 0 to TASFE interface
    SPOUT_CFG_CULLCLIP_FLAG_OUT_EN_ENABLE    = 1,   // 1: Enable. need read from VC, whose channel is set in
    // CullClip_Flag_Ch register
} SPOUT_CFG_CULLCLIP_FLAG_OUT_EN;
typedef enum
{
    SPOUT_CFG_PRIMID_APPEND_EN_DISABLE       = 0,   // 0: Disable, do nothing
    SPOUT_CFG_PRIMID_APPEND_EN_ENABLE        = 1,   // 1: Enable. append Pid after all attributes are outputs
} SPOUT_CFG_PRIMID_APPEND_EN;
typedef enum
{
    POS_READ_LOC_POS_LOC_SEPARATED           = 0,   // 0: Default,  all position values are at separated space. If
    // Mv_En is OFF, the location is always at separated space.
    POS_READ_LOC_POS_LOC_SHARED              = 1,   // 1: Mv_En is ON, some positin data may be at shared space,
    // used by all vertices of multi-views.
} POS_READ_LOC_POS_LOC;
typedef enum
{
    ATTR_READ_LOC0_ATTR_LOC_SEPARATED        = 0,   // 0: Default,  all attribute values are at separated space.
    // If Mv_En is OFF, the location is always at separated space.
    ATTR_READ_LOC0_ATTR_LOC_SHARED           = 1,   // 1: Mv_En is ON, some attribute data may be at shared space,
    // used by all vertices of multi-views.
} ATTR_READ_LOC0_ATTR_LOC;
typedef enum
{
    ATTR_READ_LOC1_ATTR_LOC_SEPARATED        = 0,   // 0: Default,  all attribute values are at separated space.
    // If Mv_En is OFF, the location is always at separated space.
    ATTR_READ_LOC1_ATTR_LOC_SHARED           = 1,   // 1: Mv_En is ON, some attribute data may be at shared space,
    // used by all vertices of multi-views.
} ATTR_READ_LOC1_ATTR_LOC;
typedef enum
{
    POS_DEF_VALUE_P0_X_DEF_DISABLED          = 0,   // 0: Disable. no default value, it should be read from VC
    POS_DEF_VALUE_P0_X_DEF_ZERO              = 1,   // 1: The component value is set 0
    POS_DEF_VALUE_P0_X_DEF_FP_ONE            = 2,   // 2: The component value is set 1.0f
    POS_DEF_VALUE_P0_X_DEF_RESERVED          = 3,   // 3: Reserved
} POS_DEF_VALUE_P0_X_DEF;
typedef enum
{
    ATTR_DEF_VALUE_A0_X_DEF_DISABLED         = 0,   // 0: Disable. no default value, it should be read from VC
    ATTR_DEF_VALUE_A0_X_DEF_ZERO             = 1,   // 1: The component value is set 0
    ATTR_DEF_VALUE_A0_X_DEF_FP_ONE           = 2,   // 2: The component value is set 1.0f
    ATTR_DEF_VALUE_A0_X_DEF_RESERVED         = 3,   // 3: Reserved
} ATTR_DEF_VALUE_A0_X_DEF;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////            SPOUT Block (Block ID = 22) Register Definitions                          ///////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Out_Rast_En               : 1;   // Enable outputs to Tas, used for 3D rendering
        unsigned int Sto_En                    : 1;   // Stream Out enable
        unsigned int Out_Size                  : 6;   // VS/DS/GS output attr size, note that as for VS/DS MV cases,
        // that's separate attr size; Value is 0~48 elements.
        unsigned int Shared_Size               : 6;   // MV VS/DS output shared attr, used by all MV vertices. Value
        // is 0~48 4-component elements.
        unsigned int Vtx_Pos_Size              : 2;   // Vertex output Position element size, may be 1~2, which are
        // outputted to TAS-POS. They are pos, Pt_Size(.x),
        // edge_flag(.y) wscale0/1/2/3 in order.
        unsigned int Vtx_Attr_Size             : 6;   // VS outputs Attribute size, may be 0~48, which are
        // outputted to TAS-ATTR. Its offset are from Vtx_Pos_Size.
        // 0 means no attributes.
        unsigned int Rtarray_Idx_Out_En        : 1;   // 11bits RT array index value control bit
        unsigned int Viewport_Mask_Out_En      : 1;   // 16bits viewport mask (idx) control bit
        unsigned int Cullclip_Flag_Out_En      : 1;   // 16bits cull/clip distance sign flags control bit. 8
        // culld/clip2. 2bits (NAN/<0) per clipd
        unsigned int Vtx_Idx_Id                : 6;   // Vertex index element position, maybe at O0~ O48.
        // rt_array_idx/ vp_mask/cullclip_flag should be in one
        // element to read them with position in one cycle.
        unsigned int Primid_Append_En          : 1;   // Whether append PrimID value after all attributes are
        // output to TAS. For VS + PS, needn't use GS to do Pid, use it to
        // append PrimID into TAS AB.
    } reg;
} Reg_Spout_Cfg;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Rtarray_Idx_Ch            : 2;   // RT array index channel in Vtx_Idx_Id
        unsigned int Viewport_Mask_Ch          : 2;   // view port mask channel in Vtx_Idx_Id
        unsigned int Cullclip_Flag_Ch          : 2;   // Cull/clip distance sign flags channel in Vtx_Idx_Id
        unsigned int Reserved                  : 26;  // Reserved
    } reg;
} Reg_Spout_Idx_Ch;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Pos_Loc                   : 2;   // Read Position location, may be at separated(0x0) or
        // shared(0x1), 2E in total. If Mv is ON, may be in shared
        // space.
        unsigned int Reserved                  : 30;  // Reserved
    } reg;
} Reg_Pos_Read_Loc;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Attr_Loc                  : 32;  // Read Attribute location, may be at separated(0x0) or
        // shared(0x1), 48E in total. If Mv is ON, may be in shared
        // space.
    } reg;
} Reg_Attr_Read_Loc0;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Attr_Loc                  : 16;  // Read Attribute location, may be at separated(0x0) or
        // shared(0x1), 48E in total. If Mv is ON, may be in shared
        // space.
        unsigned int Reserved                  : 16;  // Reserved
    } reg;
} Reg_Attr_Read_Loc1;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int P0_Addr                   : 6;   // Position data may includes 6elements in total, maybe Pos,
        // PtSize, Wscale0~3. If MV is ON, the data may be at shared
        // location.
        unsigned int P1_Addr                   : 6;   // PTSize(.x), Edge_Flag(.y) read data position.
        unsigned int Reserved                  : 20;  // Reserved
    } reg;
} Reg_Pos_Read_Id;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int A0_Addr                   : 6;   // Attributes data may be in separated or shared space. 48E in
        // total.
        unsigned int A1_Addr                   : 6;   // Attribute read data position.
        unsigned int A2_Addr                   : 6;   // Attribute read data position.
        unsigned int A3_Addr                   : 6;   // Attribute read data position.
        unsigned int Reserved                  : 8;   // Reserved
    } reg;
} Reg_Attr_Read_Id;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int P0_X_Def                  : 2;   // Pos#.x defalut value, may be VC/0/1.0f
        unsigned int P0_Y_Def                  : 2;   // Pos#.y defalut value, may be VC/0/1.0f
        unsigned int P0_Z_Def                  : 2;   // Pos#.z defalut value, may be VC/0/1.0f
        unsigned int P0_W_Def                  : 2;   // Pos#.w defalut value, may be VC/0/1.0f
        unsigned int P1_X_Def                  : 2;   // Pos#.x defalut value, may be VC/0/1.0f
        unsigned int P1_Y_Def                  : 2;   // Pos#.y defalut value, may be VC/0/1.0f
        unsigned int P1_Z_Def                  : 2;   // Pos#.z defalut value, may be VC/0/1.0f
        unsigned int P1_W_Def                  : 2;   // Pos#.w defalut value, may be VC/0/1.0f
        unsigned int Reserved                  : 16;  // Reserved
    } reg;
} Reg_Pos_Def_Value;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int A0_X_Def                  : 2;   // Attr#.x defalut value, may be VC/0/1.0f
        unsigned int A0_Y_Def                  : 2;   // Attr#.y defalut value, may be VC/0/1.0f
        unsigned int A0_Z_Def                  : 2;   // Attr#.z defalut value, may be VC/0/1.0f
        unsigned int A0_W_Def                  : 2;   // Attr#.w defalut value, may be VC/0/1.0f
        unsigned int A1_X_Def                  : 2;   // Attr#.x defalut value, may be VC/0/1.0f
        unsigned int A1_Y_Def                  : 2;   // Attr#.y defalut value, may be VC/0/1.0f
        unsigned int A1_Z_Def                  : 2;   // Attr#.z defalut value, may be VC/0/1.0f
        unsigned int A1_W_Def                  : 2;   // Attr#.w defalut value, may be VC/0/1.0f
        unsigned int A2_X_Def                  : 2;   // Attr#.x defalut value, may be VC/0/1.0f
        unsigned int A2_Y_Def                  : 2;   // Attr#.y defalut value, may be VC/0/1.0f
        unsigned int A2_Z_Def                  : 2;   // Attr#.z defalut value, may be VC/0/1.0f
        unsigned int A2_W_Def                  : 2;   // Attr#.w defalut value, may be VC/0/1.0f
        unsigned int A3_X_Def                  : 2;   // Attr#.x defalut value, may be VC/0/1.0f
        unsigned int A3_Y_Def                  : 2;   // Attr#.y defalut value, may be VC/0/1.0f
        unsigned int A3_Z_Def                  : 2;   // Attr#.z defalut value, may be VC/0/1.0f
        unsigned int A3_W_Def                  : 2;   // Attr#.w defalut value, may be VC/0/1.0f
    } reg;
} Reg_Attr_Def_Value;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Reserved                  : 32;  // RESERVED
    } reg;
} Reg_Spout_Reserved;

typedef struct _Spout_regs
{
    Reg_Spout_Cfg                    reg_Spout_Cfg;
    Reg_Spout_Idx_Ch                 reg_Spout_Idx_Ch;
    Reg_Pos_Read_Loc                 reg_Pos_Read_Loc;
    Reg_Attr_Read_Loc0               reg_Attr_Read_Loc0;
    Reg_Attr_Read_Loc1               reg_Attr_Read_Loc1;
    Reg_Pos_Read_Id                  reg_Pos_Read_Id;
    Reg_Attr_Read_Id                 reg_Attr_Read_Id[12];
    Reg_Pos_Def_Value                reg_Pos_Def_Value;
    Reg_Attr_Def_Value               reg_Attr_Def_Value[12];
    Reg_Spout_Reserved               reg_Spout_Reserved;
} Spout_regs;

#endif
