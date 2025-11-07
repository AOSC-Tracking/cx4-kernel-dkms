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
//    Spec Last Modified Time: 14:22 2019-10-18
#ifndef _IU_REGISTER_H
#define _IU_REGISTER_H


#ifndef        IU_BLOCKBASE_INF
    #define    IU_BLOCKBASE_INF
    #define    BLOCK_IU_VERSION 1
    #define    BLOCK_IU_TIMESTAMP "14:22 2019-10-18"
    #define    IU_BLOCK                                           0x7 // match with BlockID.h
    #define    IU_REG_START                                       0x0 // match with BlockID.h
    #define    IU_REG_END_CHX004                                  0x48 // match with BlockID.h
    #define    IU_REG_LIMIT_CHX004                                0x48 // match with BlockID.h
    #define    IU_REG_END_Elt3k                                   0x40 // match with BlockID.h
    #define    IU_REG_LIMIT_Elt3k                                 0x40 // match with BlockID.h
#endif

// Register Offset definition
#define        Reg_Iu_Ctrl_Offset                                         0x0
#define        Reg_Iu_Ctrl_Ex_Offset                                      0x1
#define        Reg_Iu_Rt_Size_Offset                                      0x2
#define        Reg_Iu_Mapping_Offset                                      0x3
#define        Reg_Iu_Intp_Mode_Offset                                    0xd
#define        Reg_Iu_Clip_Distance_Offset                                0x12
#define        Reg_Iu_Crf_Baseaddr_Offset                                 0x14
#define        Reg_Iu_Pxpy_Crf_Baseaddr_Offset                            0x1e
#define        Reg_Iu_Pxpy_Lowhigh_Offset                                 0x28
#define        Reg_Iu_Output_Format_Offset                                0x2a
#define        Reg_Iu_Viewportzvalue_Offset                               0x2c
#define        Reg_Iu_Builtin_Attr_Packing_Offset                         0x3c
#define        Reg_Iu_Reserved_Dw_Offset_Elt3k                            0x3d
#define        Reg_Programmable_Msaa_Pattern_Offset_CHX004                0x3d
#define        Reg_Iu_Ctrl_Ex2_Offset_CHX004                              0x41
#define        Reg_Iu_Reserved_Dw_Offset_CHX004                           0x42

// Block constant definition
typedef enum
{
    IU_CTRL_INSERT_POS_DISABLED              = 0, // 0: no position in PS inputs
    IU_CTRL_INSERT_POS_ENABLED               = 1, // 1: insert position following normal varying attributes
} IU_CTRL_INSERT_POS;

typedef enum
{
    IU_CTRL_INSERT_FACE_DISABLED             = 0, // 0: no face ID in PS inputs
    IU_CTRL_INSERT_FACE_ENABLED              = 1, // 1: insert face ID following normal varying attributes
                                                  // and position if exists
} IU_CTRL_INSERT_FACE;

typedef enum
{
    IU_CTRL_INSERT_ARRAY_ID_DISABLE          = 0, // 0:no array_id in PS inputs
    IU_CTRL_INSERT_ARRAY_ID_ENABLED          = 1, // 1:insert array_id following normal varying attributes
} IU_CTRL_INSERT_ARRAY_ID;

typedef enum
{
    IU_CTRL_INSERT_RASTER_MASK_DISABLED      = 0, // 0: no gl_SampleID in PS inputs
    IU_CTRL_INSERT_RASTER_MASK_ENABLED       = 1, // 1: insert sample index following "pos &| face &| array
                                                  // index"
} IU_CTRL_INSERT_RASTER_MASK;

typedef enum
{
    IU_CTRL_INSERT_SAMPLE_ID_DISABLED        = 0, // 0: no gl_SampleID in PS inputs
    IU_CTRL_INSERT_SAMPLE_ID_ENABLED         = 1, // 1: insert sample index following "pos &| face &|
                                                  // array_id &| raster_mask"
} IU_CTRL_INSERT_SAMPLE_ID;

typedef enum
{
    IU_CTRL_INSERT_SAMPLE_OFFSET_DISABLED    = 0, // 0: no gl_SamplePosition in PS inputs
    IU_CTRL_INSERT_SAMPLE_OFFSET_ENABLED     = 1, // 1: insert sample index following "pos &| face &|
                                                  // array_id &| raster_mask &| sample_id"
} IU_CTRL_INSERT_SAMPLE_OFFSET;

typedef enum
{
    IU_CTRL_INSERT_CONSERVATIVE_INNER_MASK_DISABLED= 0, // 0: no convservative inner mask
    IU_CTRL_INSERT_CONSERVATIVE_INNER_MASK_ENABLED= 1, // 1: PS need innder coverage mask
} IU_CTRL_INSERT_CONSERVATIVE_INNER_MASK;

typedef enum
{
    IU_CTRL_QUAD_2X2_MODE_EN_4X1             = 0, // 0: default 4x1 quad layout
    IU_CTRL_QUAD_2X2_MODE_EN_2X2             = 1, // 1: only when Raster Resolution = PS Resolution = 1X, may
                                                  // have 4x1 quad layout
} IU_CTRL_QUAD_2X2_MODE_EN;

typedef enum
{
    IU_CTRL_RAST_RULE_TOP_LEFT               = 0, // 0x00: top left
    IU_CTRL_RAST_RULE_BOTTOM_LEFT            = 1, // 0x01: bottom left
    IU_CTRL_RAST_RULE_TOP_RIGHT              = 2, // 0x10: top right
    IU_CTRL_RAST_RULE_BOTTOM_RIGHT           = 3, // 0x11: bottom right
} IU_CTRL_RAST_RULE;

typedef enum
{
    IU_CTRL_SIMD_MODE_SIMD32                 = 0, // 0x00: simd32 mode, IU can pack 32 lanes (8 valid quads)
                                                  // to one thread at most
    IU_CTRL_SIMD_MODE_SIMD64                 = 1, // 0x01: simd64 mode, IU can pack 64 lanes (16 valid quads)
                                                  // to one threadd at most
} IU_CTRL_SIMD_MODE;

typedef enum
{
    IU_CTRL_EX_EU_BLENDING_EN_DISABLED       = 0, // 0: EU blending off
    IU_CTRL_EX_EU_BLENDING_EN_ENABLED        = 1, // 1: EU blending on
} IU_CTRL_EX_EU_BLENDING_EN;

typedef enum
{
    IU_CTRL_EX_Y_INVERT_EN_DISABLED          = 0, // 0: no y invert
    IU_CTRL_EX_Y_INVERT_EN_ENABLED           = 1, // 1: y invert
} IU_CTRL_EX_Y_INVERT_EN;

typedef enum
{
    IU_CTRL_EX_PRE_ROTATE_EN_DISABLED        = 0, // 0: EU blending off
    IU_CTRL_EX_PRE_ROTATE_EN_ENABLED         = 1, // 1: EU blending on
} IU_CTRL_EX_PRE_ROTATE_EN;

typedef enum
{
    IU_CTRL_EX_PRE_ROTATE_TYPE_CCW_0         = 0, // 0: Counter clockwise 0 degree
    IU_CTRL_EX_PRE_ROTATE_TYPE_CCW_90        = 1, // 1: Counter clockwise 90 degree
    IU_CTRL_EX_PRE_ROTATE_TYPE_CCW_90_H      = 2, // 2: Counter clockwise 90 degree plus H
    IU_CTRL_EX_PRE_ROTATE_TYPE_CCW_90_V      = 3, // 3: Counter clockwise 90 degree plus V
    IU_CTRL_EX_PRE_ROTATE_TYPE_CCW_180       = 4, // 4: Counter clockwise 180 degree
    IU_CTRL_EX_PRE_ROTATE_TYPE_CCW_270       = 5, // 5: Counter clockwise 270 degree
} IU_CTRL_EX_PRE_ROTATE_TYPE;

typedef enum
{
    IU_CTRL_EX_RAST_RESOLUTION_MSAA_1X       = 0, // 0: MSAA1X
    IU_CTRL_EX_RAST_RESOLUTION_MSAA_2X       = 1, // 1: MSAA2X
    IU_CTRL_EX_RAST_RESOLUTION_MSAA_4X       = 2, // 2: MSAA4X
    IU_CTRL_EX_RAST_RESOLUTION_MSAA_8X       = 3, // 3: MSAA8X
    IU_CTRL_EX_RAST_RESOLUTION_MSAA_16X      = 4, // 4: MSAA16X
} IU_CTRL_EX_RAST_RESOLUTION;

typedef enum
{
    IU_CTRL_EX_API_VERSION_OGL               = 0, // 0: OGL
    IU_CTRL_EX_API_VERSION_DX9               = 1, // 1: DX9
    IU_CTRL_EX_API_VERSION_DX10              = 2, // 2: DX10
    IU_CTRL_EX_API_VERSION_DX11              = 3, // 3: DX11
    IU_CTRL_EX_API_VERSION_DX12              = 4, // 4: DX12
} IU_CTRL_EX_API_VERSION;

typedef enum
{
    IU_CTRL_EX_MSAA_EN_MSAA_OFF              = 0, // 0: msaa off
    IU_CTRL_EX_MSAA_EN_MSAA_DEFAULT          = 1, // 1: msaa with default sample location
    IU_CTRL_EX_MSAA_EN_MSAA_PROGRAMMABLE     = 2, // 2: msaa with programmable sample locations
    IU_CTRL_EX_MSAA_EN_MSAA_CENTER_SAMPLE    = 3, // 3: msaa with center sampling, when quality is
                                                  // -2(0xfffffffe)
} IU_CTRL_EX_MSAA_EN;

typedef enum
{
    IU_CTRL_EX_MSAA_GRANULARITY_PER_PIXEL    = 0, // 0: per-pixel specified sample location, each pixel has
                                                  // the same sample location
    IU_CTRL_EX_MSAA_GRANULARITY_PER_QUAD     = 1, // 1: per-quad specified sample locations, each quads has
                                                  // the same sample locations
} IU_CTRL_EX_MSAA_GRANULARITY;

typedef enum
{
    IU_CTRL_EX_PS_SAMPLE_FREQUENCY_EN_DISABLED= 0, // 0: per-pixel specified sample location, each pixel has
                                                   // the same sample location
    IU_CTRL_EX_PS_SAMPLE_FREQUENCY_EN_ENABLED= 1, // 1: per-quad specified sample locations, each quads has
                                                  // the same sample locations
} IU_CTRL_EX_PS_SAMPLE_FREQUENCY_EN;

typedef enum
{
    IU_INTP_MODE_ATTR0_MODE_CENTER           = 0, // 000:LINEAR or LINEAR_NOPERSPECTIVE
    IU_INTP_MODE_ATTR0_MODE_SAMPLE           = 1, // 001:LINEAR_SAMPLE or LINEAR_NOPERSPECTIVE_SAMPLE
    IU_INTP_MODE_ATTR0_MODE_CENTROID         = 2, // 010:LINEAR_CENTROID or LINEAR_NOPERSPECTIVE_CENTROID
    IU_INTP_MODE_ATTR0_MODE_CONSTANT_NORMAL  = 3, // 011:CONSTANT-NORMAL, normal float point constant which
                                                  // will go into ALU for format conversion
    IU_INTP_MODE_ATTR0_MODE_CONSTANT_BYPASS  = 4, // 100:CONSTANT-BYPASS, bypass integer constant which need
                                                  // to be bypassed out of ALU, just cut lsb 24bit if whose
                                                  // output format is pp mode.
    IU_INTP_MODE_ATTR0_MODE_EU_INTP_PXPY     = 5, // 101: current attrbiute is shader mode, bypass Px & Py to
                                                  // EU
    IU_INTP_MODE_ATTR0_MODE_EU_INTP_PXINVPY  = 6, // 110: current attribute is shader mode, bypass Px & Inv
                                                  // Py to EU
} IU_INTP_MODE_ATTR0_MODE;

typedef enum
{
    IU_CLIP_DISTANCE_ATTR0_CLIP_DISTANCE_EN_DISABLED= 0, // 0x0: not clip distance
    IU_CLIP_DISTANCE_ATTR0_CLIP_DISTANCE_EN_ENABLED= 1, // 0x1:clip distance
} IU_CLIP_DISTANCE_ATTR0_CLIP_DISTANCE_EN;

typedef enum
{
    IU_PXPY_LOWHIGH_ATTR0_LOWHIGH_LOW_RG     = 0, // 0x0: write Px/Py to low G|R channel
    IU_PXPY_LOWHIGH_ATTR0_LOWHIGH_HIGH_BA    = 1, // 0x1: write Px/Py to high A|B channel
} IU_PXPY_LOWHIGH_ATTR0_LOWHIGH;

typedef enum
{
    IU_OUTPUT_FORMAT_ATTR0_FMT_FP            = 0, // 0x0:fp32 or 32bit integer
    IU_OUTPUT_FORMAT_ATTR0_FMT_HP            = 1, // 0x1:fp16 or cut 32bit integer to 16bit from LSB
} IU_OUTPUT_FORMAT_ATTR0_FMT;

typedef enum
{
    IU_CTRL_EX2_RT_RESOLUTION_1X             = 0, // MSAA1X
    IU_CTRL_EX2_RT_RESOLUTION_2X             = 1, // MSAA2X
    IU_CTRL_EX2_RT_RESOLUTION_4X             = 2, // MSAA4X
    IU_CTRL_EX2_RT_RESOLUTION_8X             = 3, // MSAA8X
    IU_CTRL_EX2_RT_RESOLUTION_16X            = 4, // MSAA16X
} IU_CTRL_EX2_RT_RESOLUTION;

typedef enum
{
    IU_CTRL_EX2_ROV_EN_DISABLED              = 0, // 0: disabled
    IU_CTRL_EX2_ROV_EN_ENABLED               = 1, // 1: enabled
} IU_CTRL_EX2_ROV_EN;


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////            IU Block (Block ID = 0x7) Register Definitions                      ///////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define Reg_IU_CTRL_Ps_Attr_Num_BitField_MaxValue ((1u << 6) - 1)
#define Reg_IU_CTRL_Ab_Attr_Num_BitField_MaxValue ((1u << 6) - 1)
#define Reg_IU_CTRL_Insert_Pos_BitField_MaxValue ((1u << 1) - 1)
#define Reg_IU_CTRL_Insert_Face_BitField_MaxValue ((1u << 1) - 1)
#define Reg_IU_CTRL_Insert_Array_Id_BitField_MaxValue ((1u << 1) - 1)
#define Reg_IU_CTRL_Insert_Raster_Mask_BitField_MaxValue ((1u << 1) - 1)
#define Reg_IU_CTRL_Insert_Sample_Id_BitField_MaxValue ((1u << 1) - 1)
#define Reg_IU_CTRL_Insert_Sample_Offset_BitField_MaxValue ((1u << 1) - 1)
#define Reg_IU_CTRL_Insert_Conservative_Inner_Mask_BitField_MaxValue ((1u << 1) - 1)
#define Reg_IU_CTRL_Pack_Z_BitField_MaxValue ((1u << 1) - 1)
#define Reg_IU_CTRL_Pack_W_BitField_MaxValue ((1u << 1) - 1)
#define Reg_IU_CTRL_Quad_2x2_Mode_En_BitField_MaxValue ((1u << 1) - 1)
#define Reg_IU_CTRL_Rast_Rule_BitField_MaxValue ((1u << 2) - 1)
#define Reg_IU_CTRL_Rule_For_Msaa_Line_BitField_MaxValue ((1u << 2) - 1)
#define Reg_IU_CTRL_Rule_For_A_Line_BitField_MaxValue ((1u << 2) - 1)
#define Reg_IU_CTRL_Simd_Mode_BitField_MaxValue ((1u << 1) - 1)
#define Reg_IU_CTRL_Pack_Face_BitField_MaxValue ((1u << 1) - 1)
#define Reg_IU_CTRL_Pack_Sample_Id_BitField_MaxValue ((1u << 1) - 1)
#define Reg_IU_CTRL_Post_Depth_Coverage_En_BitField_MaxValue ((1u << 1) - 1)

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Ps_Attr_Num               : 6;  // Number of normal PS input attributes (number of entries
                                                     // in IU mapping table), 1~48. (1 based). Position,
                                                     // face,array_id,etc, not included.
        unsigned int Ab_Attr_Num               : 6;  // Number of total attributes in the Setup Attribute
                                                     // Buffer, 1~48. (1 based).This is used for AB buffer release
                                                     // purpose.
        unsigned int Insert_Pos                : 1;  // Insert position
        unsigned int Insert_Face               : 1;  // Insert face ID
        unsigned int Insert_Array_Id           : 1;  // Whether insert array_id
        unsigned int Insert_Raster_Mask        : 1;  // Insert raster mask, 4bit sample mask per pixel when
                                                     // msaa+pixel frequency,otherwise 1bit pixel or sample mask
                                                     // per 1pixel or sample
        unsigned int Insert_Sample_Id          : 1;  // Insert sample index, sample frequency only
        unsigned int Insert_Sample_Offset      : 1;  // Insert sample offset, it's sample offset inside a pixel,
                                                     // a pixel's center's offset is 0.5 decimal fraction, 0x4 in
                                                     // .3 hexadecimal
        unsigned int Insert_Conservative_Inner_Mask: 1;  // did ps need inner coverage mask in conservative raster
                                                         // mode
        unsigned int Pack_Z                    : 1;  // pack z in the last attr 3rd component
        unsigned int Pack_W                    : 1;  // pack w in the last attr 4th component
        unsigned int Quad_2x2_Mode_En          : 1;  // Quad layout
        unsigned int Rast_Rule                 : 2;  // Current Rasterization Rule
        unsigned int Rule_For_Msaa_Line        : 2;  // In dx, we will set it into TOP_LEFT. In OGL, we can use
                                                     // the one driver specified.
        unsigned int Rule_For_A_Line           : 2;  // If the line is xMajor, then the TOP or BOTTOM means
                                                     // whether the top line or bottom line will be drawn. So is
                                                     // the LEFT/RIGHT means for yMajor.
        unsigned int Simd_Mode                 : 1;  // PS supports SIMD32 and SIMD64 mode
        unsigned int Pack_Face                 : 1;  // pack face_id enable
        unsigned int Pack_Sample_Id            : 1;  // pack sample_id enable
        unsigned int Post_Depth_Coverage_En    : 1;  // used to tell IU how to choose mask to do insert rast
                                                     // mask. 0: use mask before early Z. 1: mask after early Z
    } reg;
} Reg_Iu_Ctrl;

#define Reg_IU_CTRL_EX_Eu_Blending_En_BitField_MaxValue ((1u << 1) - 1)
#define Reg_IU_CTRL_EX_Y_Invert_En_BitField_MaxValue ((1u << 1) - 1)
#define Reg_IU_CTRL_EX_Pre_Rotate_En_BitField_MaxValue ((1u << 1) - 1)
#define Reg_IU_CTRL_EX_Pre_Rotate_Type_BitField_MaxValue ((1u << 3) - 1)
#define Reg_IU_CTRL_EX_Rast_Resolution_BitField_MaxValue ((1u << 3) - 1)
#define Reg_IU_CTRL_EX_Api_Version_BitField_MaxValue ((1u << 3) - 1)
#define Reg_IU_CTRL_EX_Pixel_Center_Integer_En_BitField_MaxValue ((1u << 1) - 1)
#define Reg_IU_CTRL_EX_Fragcoord_Invert_En_BitField_MaxValue ((1u << 1) - 1)
#define Reg_IU_CTRL_EX_Msaa_Center_Sample_En_BitField_MaxValue ((1u << 1) - 1)
#define Reg_IU_CTRL_EX_Mrt_En_BitField_MaxValue ((1u << 1) - 1)
#define Reg_IU_CTRL_EX_First_Valid_Sample_BitField_MaxValue ((1u << 5) - 1)
#define Reg_IU_CTRL_EX_Insert_Shaing_Rate_BitField_MaxValue ((1u << 1) - 1)
#define Reg_IU_CTRL_EX_Force_Kickoff_Tile_Num_BitField_MaxValue ((1u << 5) - 1)
#define Reg_IU_CTRL_EX_Msaa_En_BitField_MaxValue ((1u << 2) - 1)
#define Reg_IU_CTRL_EX_Msaa_Granularity_BitField_MaxValue ((1u << 1) - 1)
#define Reg_IU_CTRL_EX_Ps_Sample_Frequency_En_BitField_MaxValue ((1u << 1) - 1)
#define Reg_IU_CTRL_EX_Reserved_BitField_MaxValue ((1u << 1) - 1)

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Eu_Blending_En : 1;  // EU do blending
        unsigned int Y_Invert_En : 1;  // PS input position y invert adjustment
        unsigned int Pre_Rotate_En : 1;  // Pre-rotate, IU will convert inserted position based on
                                                         // pre rotate formula
        unsigned int Pre_Rotate_Type : 3;  // Pre-rotate type
        unsigned int Rast_Resolution : 3;  // Raster resolution
        unsigned int Api_Version : 3;  // API selection, used for API specific handling, raster
                                                         // rule, etc.
        unsigned int Pixel_Center_Integer_En : 1;  // Wheter need insert position as pixel upper-left corner
                                                         // integer, only set on under OGL mode
        unsigned int Fragcoord_Invert_En : 1;  // Wheter need insert position as original upper left, only
                                                         // set on under OGL mode
        unsigned int Msaa_Center_Sample_En : 1;  // MSAA center sample when quality is -2( useless, refer to
                                                         // msaa_en field)
        unsigned int Mrt_En : 1;  // Used to EUB + MRT case, when mrt_en, IU need per-tile
                                                         // force kickoff thread
        unsigned int First_Valid_Sample : 5;  // For centroid process, total 5 bits. Bit4 stands for
                                                         // whether pixel center interpolate or not (1 : for pixel
                                                         // center; 0 for nomral. Bit4 can only set when msaa_mask is
                                                         // full mask, e.g. 16X/8X/4X/2X are 0xffff /0xff /0xf /0x3
                                                         // respectively.) Bit3 ~ Bit0 stands for the first valid
                                                         // sample in hw pattern. ( For 16X, can be 0 ~ 15)
        unsigned int Insert_Shaing_Rate : 1;  // Wheter PS need shading rate system value
        unsigned int Force_Kickoff_Tile_Num : 5;  // Force kickoff tile number can be 8 (simd32) or 16
                                                         // (simd64)
        unsigned int Msaa_En : 2;  // MSAA mode
        unsigned int Msaa_Granularity : 1;  // MSAA Granularity
        unsigned int Ps_Sample_Frequency_En : 1;  // Whether ps run in sample frequency, for iu pack thread
                                                         // under variable rate shading
        unsigned int Reserved : 1;  // Reserved
    } reg_CHX004;
    struct
    {
        unsigned int Eu_Blending_En : 1;   // EU do blending
        unsigned int Y_Invert_En : 1;   // PS input position y invert adjustment
        unsigned int Pre_Rotate_En : 1;   // Pre-rotate, IU will convert inserted position based on
                                                          // pre rotate formula
        unsigned int Pre_Rotate_Type : 3;   // Pre-rotate type
        unsigned int Rast_Resolution : 3;   // Raster resolution
        unsigned int Api_Version : 3;   // API selection, used for API specific handling, raster
                                                          // rule, etc.
        unsigned int Pixel_Center_Integer_En : 1;   // Wheter need insert position as pixel upper-left corner
                                                          // integer, only set on under OGL mode
        unsigned int Fragcoord_Invert_En : 1;   // Wheter need insert position as original upper left, only
                                                          // set on under OGL mode
        unsigned int Msaa_Center_Sample_En : 1;   // MSAA center sample when quality is -2
        unsigned int Mrt_En : 1;   // Used to EUB + MRT case, when mrt_en, IU need per-tile force
                                                          // kickoff thread
        unsigned int First_Valid_Sample : 5;   // For centroid process, total 5 bits. Bit4 stands for
                                                          // whether pixel center interpolate or not (1 : for pixel
                                                          // center; 0 for nomral.  Bit4 can only set when msaa_mask is
                                                          // full mask, e.g. 16X/8X/4X/2X are 0xffff /0xff /0xf /0x3
                                                          // respectively.)
                                                          //  Bit3 ~ Bit0 stands for the first valid sample in hw pattern.
                                                          // ( For 16X, can be 0 ~ 15)
        unsigned int Reserved : 11;  // Reserved
    } reg;
} Reg_Iu_Ctrl_Ex;

#define Reg_IU_RT_SIZE_Rt_Height_BitField_MaxValue ((1u << 15) - 1)
#define Reg_IU_RT_SIZE_Rt_Width_BitField_MaxValue ((1u << 15) - 1)
#define Reg_IU_RT_SIZE_Reserved_BitField_MaxValue ((1u << 2) - 1)

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Rt_Height                 : 15;  // Render target height for y invert adjustment in OES
        unsigned int Rt_Width                  : 15;  // Render target width for pre-rotate adjustment
        unsigned int Reserved                  : 2;  // Reserved
    } reg;
} Reg_Iu_Rt_Size;

#define Reg_IU_MAPPING_Attr0_Id_BitField_MaxValue ((1u << 6) - 1)
#define Reg_IU_MAPPING_Attr1_Id_BitField_MaxValue ((1u << 6) - 1)
#define Reg_IU_MAPPING_Attr2_Id_BitField_MaxValue ((1u << 6) - 1)
#define Reg_IU_MAPPING_Attr3_Id_BitField_MaxValue ((1u << 6) - 1)
#define Reg_IU_MAPPING_Attr4_Id_BitField_MaxValue ((1u << 6) - 1)
#define Reg_IU_MAPPING_Reserved_BitField_MaxValue ((1u << 2) - 1)

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Attr0_Id                  : 6;  // Attribute ID in AB (0~47)
        unsigned int Attr1_Id                  : 6;  // Attribute ID in AB (0~47)
        unsigned int Attr2_Id                  : 6;  // Attribute ID in AB (0~47)
        unsigned int Attr3_Id                  : 6;  // Attribute ID in AB (0~47)
        unsigned int Attr4_Id                  : 6;  // Attribute ID in AB (0~47)
        unsigned int Reserved                  : 2;  // Attribute ID in AB (0~47)
    } reg;
} Reg_Iu_Mapping;

#define Reg_IU_INTP_MODE_Attr0_Mode_BitField_MaxValue ((1u << 3) - 1)
#define Reg_IU_INTP_MODE_Attr1_Mode_BitField_MaxValue ((1u << 3) - 1)
#define Reg_IU_INTP_MODE_Attr2_Mode_BitField_MaxValue ((1u << 3) - 1)
#define Reg_IU_INTP_MODE_Attr3_Mode_BitField_MaxValue ((1u << 3) - 1)
#define Reg_IU_INTP_MODE_Attr4_Mode_BitField_MaxValue ((1u << 3) - 1)
#define Reg_IU_INTP_MODE_Attr5_Mode_BitField_MaxValue ((1u << 3) - 1)
#define Reg_IU_INTP_MODE_Attr6_Mode_BitField_MaxValue ((1u << 3) - 1)
#define Reg_IU_INTP_MODE_Attr7_Mode_BitField_MaxValue ((1u << 3) - 1)
#define Reg_IU_INTP_MODE_Attr8_Mode_BitField_MaxValue ((1u << 3) - 1)
#define Reg_IU_INTP_MODE_Attr9_Mode_BitField_MaxValue ((1u << 3) - 1)
#define Reg_IU_INTP_MODE_Reserved_BitField_MaxValue ((1u << 2) - 1)

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Attr0_Mode                : 3;  // Attribute's Interpolation Mode
        unsigned int Attr1_Mode                : 3;  // Attribute's Interpolation Mode
        unsigned int Attr2_Mode                : 3;  // Attribute's Interpolation Mode
        unsigned int Attr3_Mode                : 3;  // Attribute's Interpolation Mode
        unsigned int Attr4_Mode                : 3;  // Attribute's Interpolation Mode
        unsigned int Attr5_Mode                : 3;  // Attribute's Interpolation Mode
        unsigned int Attr6_Mode                : 3;  // Attribute's Interpolation Mode
        unsigned int Attr7_Mode                : 3;  // Attribute's Interpolation Mode
        unsigned int Attr8_Mode                : 3;  // Attribute's Interpolation Mode
        unsigned int Attr9_Mode                : 3;  // Attribute's Interpolation Mode
        unsigned int Reserved                  : 2;  // Rreserved
    } reg;
} Reg_Iu_Intp_Mode;

#define Reg_IU_CLIP_DISTANCE_Attr0_Clip_Distance_En_BitField_MaxValue ((1u << 1) - 1)
#define Reg_IU_CLIP_DISTANCE_Attr1_Clip_Distance_En_BitField_MaxValue ((1u << 1) - 1)
#define Reg_IU_CLIP_DISTANCE_Attr2_Clip_Distance_En_BitField_MaxValue ((1u << 1) - 1)
#define Reg_IU_CLIP_DISTANCE_Attr3_Clip_Distance_En_BitField_MaxValue ((1u << 1) - 1)
#define Reg_IU_CLIP_DISTANCE_Attr4_Clip_Distance_En_BitField_MaxValue ((1u << 1) - 1)
#define Reg_IU_CLIP_DISTANCE_Attr5_Clip_Distance_En_BitField_MaxValue ((1u << 1) - 1)
#define Reg_IU_CLIP_DISTANCE_Attr6_Clip_Distance_En_BitField_MaxValue ((1u << 1) - 1)
#define Reg_IU_CLIP_DISTANCE_Attr7_Clip_Distance_En_BitField_MaxValue ((1u << 1) - 1)
#define Reg_IU_CLIP_DISTANCE_Attr8_Clip_Distance_En_BitField_MaxValue ((1u << 1) - 1)
#define Reg_IU_CLIP_DISTANCE_Attr9_Clip_Distance_En_BitField_MaxValue ((1u << 1) - 1)
#define Reg_IU_CLIP_DISTANCE_Attr10_Clip_Distance_En_BitField_MaxValue ((1u << 1) - 1)
#define Reg_IU_CLIP_DISTANCE_Attr11_Clip_Distance_En_BitField_MaxValue ((1u << 1) - 1)
#define Reg_IU_CLIP_DISTANCE_Attr12_Clip_Distance_En_BitField_MaxValue ((1u << 1) - 1)
#define Reg_IU_CLIP_DISTANCE_Attr13_Clip_Distance_En_BitField_MaxValue ((1u << 1) - 1)
#define Reg_IU_CLIP_DISTANCE_Attr14_Clip_Distance_En_BitField_MaxValue ((1u << 1) - 1)
#define Reg_IU_CLIP_DISTANCE_Attr15_Clip_Distance_En_BitField_MaxValue ((1u << 1) - 1)
#define Reg_IU_CLIP_DISTANCE_Attr16_Clip_Distance_En_BitField_MaxValue ((1u << 1) - 1)
#define Reg_IU_CLIP_DISTANCE_Attr17_Clip_Distance_En_BitField_MaxValue ((1u << 1) - 1)
#define Reg_IU_CLIP_DISTANCE_Attr18_Clip_Distance_En_BitField_MaxValue ((1u << 1) - 1)
#define Reg_IU_CLIP_DISTANCE_Attr19_Clip_Distance_En_BitField_MaxValue ((1u << 1) - 1)
#define Reg_IU_CLIP_DISTANCE_Attr20_Clip_Distance_En_BitField_MaxValue ((1u << 1) - 1)
#define Reg_IU_CLIP_DISTANCE_Attr21_Clip_Distance_En_BitField_MaxValue ((1u << 1) - 1)
#define Reg_IU_CLIP_DISTANCE_Attr22_Clip_Distance_En_BitField_MaxValue ((1u << 1) - 1)
#define Reg_IU_CLIP_DISTANCE_Attr23_Clip_Distance_En_BitField_MaxValue ((1u << 1) - 1)
#define Reg_IU_CLIP_DISTANCE_Attr24_Clip_Distance_En_BitField_MaxValue ((1u << 1) - 1)
#define Reg_IU_CLIP_DISTANCE_Attr25_Clip_Distance_En_BitField_MaxValue ((1u << 1) - 1)
#define Reg_IU_CLIP_DISTANCE_Attr26_Clip_Distance_En_BitField_MaxValue ((1u << 1) - 1)
#define Reg_IU_CLIP_DISTANCE_Attr27_Clip_Distance_En_BitField_MaxValue ((1u << 1) - 1)
#define Reg_IU_CLIP_DISTANCE_Attr28_Clip_Distance_En_BitField_MaxValue ((1u << 1) - 1)
#define Reg_IU_CLIP_DISTANCE_Attr29_Clip_Distance_En_BitField_MaxValue ((1u << 1) - 1)
#define Reg_IU_CLIP_DISTANCE_Attr30_Clip_Distance_En_BitField_MaxValue ((1u << 1) - 1)
#define Reg_IU_CLIP_DISTANCE_Attr31_Clip_Distance_En_BitField_MaxValue ((1u << 1) - 1)

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Attr0_Clip_Distance_En    : 1;  // Attribute0's Clip Distance enable
        unsigned int Attr1_Clip_Distance_En    : 1;  // Attribute1's Clip Distance enable
        unsigned int Attr2_Clip_Distance_En    : 1;  // Attribute2's Clip Distance enable
        unsigned int Attr3_Clip_Distance_En    : 1;  // Attribute3's Clip Distance enable
        unsigned int Attr4_Clip_Distance_En    : 1;  // Attribute4's Clip Distance enable
        unsigned int Attr5_Clip_Distance_En    : 1;  // Attribute5's Clip Distance enable
        unsigned int Attr6_Clip_Distance_En    : 1;  // Attribute6's Clip Distance enable
        unsigned int Attr7_Clip_Distance_En    : 1;  // Attribute7's Clip Distance enable
        unsigned int Attr8_Clip_Distance_En    : 1;  // Attribute8's Clip Distance enable
        unsigned int Attr9_Clip_Distance_En    : 1;  // Attribute9's Clip Distance enable
        unsigned int Attr10_Clip_Distance_En   : 1;  // Attribute10's Clip Distance enable
        unsigned int Attr11_Clip_Distance_En   : 1;  // Attribute11's Clip Distance enable
        unsigned int Attr12_Clip_Distance_En   : 1;  // Attribute12's Clip Distance enable
        unsigned int Attr13_Clip_Distance_En   : 1;  // Attribute13's Clip Distance enable
        unsigned int Attr14_Clip_Distance_En   : 1;  // Attribute14's Clip Distance enable
        unsigned int Attr15_Clip_Distance_En   : 1;  // Attribute15's Clip Distance enable
        unsigned int Attr16_Clip_Distance_En   : 1;  // Attribute16's Clip Distance enable
        unsigned int Attr17_Clip_Distance_En   : 1;  // Attribute17's Clip Distance enable
        unsigned int Attr18_Clip_Distance_En   : 1;  // Attribute18's Clip Distance enable
        unsigned int Attr19_Clip_Distance_En   : 1;  // Attribute19's Clip Distance enable
        unsigned int Attr20_Clip_Distance_En   : 1;  // Attribute20's Clip Distance enable
        unsigned int Attr21_Clip_Distance_En   : 1;  // Attribute21's Clip Distance enable
        unsigned int Attr22_Clip_Distance_En   : 1;  // Attribute22's Clip Distance enable
        unsigned int Attr23_Clip_Distance_En   : 1;  // Attribute23's Clip Distance enable
        unsigned int Attr24_Clip_Distance_En   : 1;  // Attribute24's Clip Distance enable
        unsigned int Attr25_Clip_Distance_En   : 1;  // Attribute25's Clip Distance enable
        unsigned int Attr26_Clip_Distance_En   : 1;  // Attribute26's Clip Distance enable
        unsigned int Attr27_Clip_Distance_En   : 1;  // Attribute27's Clip Distance enable
        unsigned int Attr28_Clip_Distance_En   : 1;  // Attribute28's Clip Distance enable
        unsigned int Attr29_Clip_Distance_En   : 1;  // Attribute29's Clip Distance enable
        unsigned int Attr30_Clip_Distance_En   : 1;  // Attribute30's Clip Distance enable
        unsigned int Attr31_Clip_Distance_En   : 1;  // Attribute31's Clip Distance enable
    } reg;
} Reg_Iu_Clip_Distance;

#define Reg_IU_CRF_BASEADDR_Attr0_Crf_Addr_BitField_MaxValue ((1u << 6) - 1)
#define Reg_IU_CRF_BASEADDR_Attr1_Crf_Addr_BitField_MaxValue ((1u << 6) - 1)
#define Reg_IU_CRF_BASEADDR_Attr2_Crf_Addr_BitField_MaxValue ((1u << 6) - 1)
#define Reg_IU_CRF_BASEADDR_Attr3_Crf_Addr_BitField_MaxValue ((1u << 6) - 1)
#define Reg_IU_CRF_BASEADDR_Attr4_Crf_Addr_BitField_MaxValue ((1u << 6) - 1)
#define Reg_IU_CRF_BASEADDR_Reserved_BitField_MaxValue ((1u << 2) - 1)

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Attr0_Crf_Addr            : 6;  // Attribute0 CRF Base address in AB (0~47)
        unsigned int Attr1_Crf_Addr            : 6;  // Attribute1 CRF Base address in AB (0~47)
        unsigned int Attr2_Crf_Addr            : 6;  // Attribute2 CRF Base address in AB (0~47)
        unsigned int Attr3_Crf_Addr            : 6;  // Attribute3 CRF Base address in AB (0~47)
        unsigned int Attr4_Crf_Addr            : 6;  // Attribute4 CRF Base address in AB (0~47)
        unsigned int Reserved                  : 2;  // Reserved
    } reg;
} Reg_Iu_Crf_Baseaddr;

#define Reg_IU_PXPY_CRF_BASEADDR_Attr0_Pxpy_Crf_Addr_BitField_MaxValue ((1u << 6) - 1)
#define Reg_IU_PXPY_CRF_BASEADDR_Attr1_Pxpy_Crf_Addr_BitField_MaxValue ((1u << 6) - 1)
#define Reg_IU_PXPY_CRF_BASEADDR_Attr2_Pxpy_Crf_Addr_BitField_MaxValue ((1u << 6) - 1)
#define Reg_IU_PXPY_CRF_BASEADDR_Attr3_Pxpy_Crf_Addr_BitField_MaxValue ((1u << 6) - 1)
#define Reg_IU_PXPY_CRF_BASEADDR_Attr4_Pxpy_Crf_Addr_BitField_MaxValue ((1u << 6) - 1)
#define Reg_IU_PXPY_CRF_BASEADDR_Reserved_BitField_MaxValue ((1u << 2) - 1)

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Attr0_Pxpy_Crf_Addr       : 6;  // Attribute0 PxPy CRF Base address in AB (0~47)
        unsigned int Attr1_Pxpy_Crf_Addr       : 6;  // Attribute1 PxPy CRF Base address in AB (0~47)
        unsigned int Attr2_Pxpy_Crf_Addr       : 6;  // Attribute2 PxPy CRF Base address in AB (0~47)
        unsigned int Attr3_Pxpy_Crf_Addr       : 6;  // Attribute3 PxPy CRF Base address in AB (0~47)
        unsigned int Attr4_Pxpy_Crf_Addr       : 6;  // Attribute4 PxPy CRF Base address in AB (0~47)
        unsigned int Reserved                  : 2;  // Reserved
    } reg;
} Reg_Iu_Pxpy_Crf_Baseaddr;

#define Reg_IU_PXPY_LOWHIGH_Attr0_Lowhigh_BitField_MaxValue ((1u << 1) - 1)
#define Reg_IU_PXPY_LOWHIGH_Attr1_Lowhigh_BitField_MaxValue ((1u << 1) - 1)
#define Reg_IU_PXPY_LOWHIGH_Attr2_Lowhigh_BitField_MaxValue ((1u << 1) - 1)
#define Reg_IU_PXPY_LOWHIGH_Attr3_Lowhigh_BitField_MaxValue ((1u << 1) - 1)
#define Reg_IU_PXPY_LOWHIGH_Attr4_Lowhigh_BitField_MaxValue ((1u << 1) - 1)
#define Reg_IU_PXPY_LOWHIGH_Attr5_Lowhigh_BitField_MaxValue ((1u << 1) - 1)
#define Reg_IU_PXPY_LOWHIGH_Attr6_Lowhigh_BitField_MaxValue ((1u << 1) - 1)
#define Reg_IU_PXPY_LOWHIGH_Attr7_Lowhigh_BitField_MaxValue ((1u << 1) - 1)
#define Reg_IU_PXPY_LOWHIGH_Attr8_Lowhigh_BitField_MaxValue ((1u << 1) - 1)
#define Reg_IU_PXPY_LOWHIGH_Attr9_Lowhigh_BitField_MaxValue ((1u << 1) - 1)
#define Reg_IU_PXPY_LOWHIGH_Attr10_Lowhigh_BitField_MaxValue ((1u << 1) - 1)
#define Reg_IU_PXPY_LOWHIGH_Attr11_Lowhigh_BitField_MaxValue ((1u << 1) - 1)
#define Reg_IU_PXPY_LOWHIGH_Attr12_Lowhigh_BitField_MaxValue ((1u << 1) - 1)
#define Reg_IU_PXPY_LOWHIGH_Attr13_Lowhigh_BitField_MaxValue ((1u << 1) - 1)
#define Reg_IU_PXPY_LOWHIGH_Attr14_Lowhigh_BitField_MaxValue ((1u << 1) - 1)
#define Reg_IU_PXPY_LOWHIGH_Attr15_Lowhigh_BitField_MaxValue ((1u << 1) - 1)
#define Reg_IU_PXPY_LOWHIGH_Attr16_Lowhigh_BitField_MaxValue ((1u << 1) - 1)
#define Reg_IU_PXPY_LOWHIGH_Attr17_Lowhigh_BitField_MaxValue ((1u << 1) - 1)
#define Reg_IU_PXPY_LOWHIGH_Attr18_Lowhigh_BitField_MaxValue ((1u << 1) - 1)
#define Reg_IU_PXPY_LOWHIGH_Attr19_Lowhigh_BitField_MaxValue ((1u << 1) - 1)
#define Reg_IU_PXPY_LOWHIGH_Attr20_Lowhigh_BitField_MaxValue ((1u << 1) - 1)
#define Reg_IU_PXPY_LOWHIGH_Attr21_Lowhigh_BitField_MaxValue ((1u << 1) - 1)
#define Reg_IU_PXPY_LOWHIGH_Attr22_Lowhigh_BitField_MaxValue ((1u << 1) - 1)
#define Reg_IU_PXPY_LOWHIGH_Attr23_Lowhigh_BitField_MaxValue ((1u << 1) - 1)
#define Reg_IU_PXPY_LOWHIGH_Attr24_Lowhigh_BitField_MaxValue ((1u << 1) - 1)
#define Reg_IU_PXPY_LOWHIGH_Attr25_Lowhigh_BitField_MaxValue ((1u << 1) - 1)
#define Reg_IU_PXPY_LOWHIGH_Attr26_Lowhigh_BitField_MaxValue ((1u << 1) - 1)
#define Reg_IU_PXPY_LOWHIGH_Attr27_Lowhigh_BitField_MaxValue ((1u << 1) - 1)
#define Reg_IU_PXPY_LOWHIGH_Attr28_Lowhigh_BitField_MaxValue ((1u << 1) - 1)
#define Reg_IU_PXPY_LOWHIGH_Attr29_Lowhigh_BitField_MaxValue ((1u << 1) - 1)
#define Reg_IU_PXPY_LOWHIGH_Attr30_Lowhigh_BitField_MaxValue ((1u << 1) - 1)
#define Reg_IU_PXPY_LOWHIGH_Attr31_Lowhigh_BitField_MaxValue ((1u << 1) - 1)

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Attr0_Lowhigh             : 1;  // Attribute0 Px/Py Low/High in AB (0~47)
        unsigned int Attr1_Lowhigh             : 1;  // Attribute1 Px/Py Low/High in AB (0~47)
        unsigned int Attr2_Lowhigh             : 1;  // Attribute2 Px/Py Low/High in AB (0~47)
        unsigned int Attr3_Lowhigh             : 1;  // Attribute3 Px/Py Low/High in AB (0~47)
        unsigned int Attr4_Lowhigh             : 1;  // Attribute4 Px/Py Low/High in AB (0~47)
        unsigned int Attr5_Lowhigh             : 1;  // Attribute5 Px/Py Low/High in AB (0~47)
        unsigned int Attr6_Lowhigh             : 1;  // Attribute6 Px/Py Low/High in AB (0~47)
        unsigned int Attr7_Lowhigh             : 1;  // Attribute7 Px/Py Low/High in AB (0~47)
        unsigned int Attr8_Lowhigh             : 1;  // Attribute8 Px/Py Low/High in AB (0~47)
        unsigned int Attr9_Lowhigh             : 1;  // Attribute9 Px/Py Low/High in AB (0~47)
        unsigned int Attr10_Lowhigh            : 1;  // Attribute10 Px/Py Low/High in AB (0~47)
        unsigned int Attr11_Lowhigh            : 1;  // Attribute11 Px/Py Low/High in AB (0~47)
        unsigned int Attr12_Lowhigh            : 1;  // Attribute12 Px/Py Low/High in AB (0~47)
        unsigned int Attr13_Lowhigh            : 1;  // Attribute13 Px/Py Low/High in AB (0~47)
        unsigned int Attr14_Lowhigh            : 1;  // Attribute14 Px/Py Low/High in AB (0~47)
        unsigned int Attr15_Lowhigh            : 1;  // Attribute15 Px/Py Low/High in AB (0~47)
        unsigned int Attr16_Lowhigh            : 1;  // Attribute16 Px/Py Low/High in AB (0~47)
        unsigned int Attr17_Lowhigh            : 1;  // Attribute17 Px/Py Low/High in AB (0~47)
        unsigned int Attr18_Lowhigh            : 1;  // Attribute18 Px/Py Low/High in AB (0~47)
        unsigned int Attr19_Lowhigh            : 1;  // Attribute19 Px/Py Low/High in AB (0~47)
        unsigned int Attr20_Lowhigh            : 1;  // Attribute20 Px/Py Low/High in AB (0~47)
        unsigned int Attr21_Lowhigh            : 1;  // Attribute21 Px/Py Low/High in AB (0~47)
        unsigned int Attr22_Lowhigh            : 1;  // Attribute22 Px/Py Low/High in AB (0~47)
        unsigned int Attr23_Lowhigh            : 1;  // Attribute23 Px/Py Low/High in AB (0~47)
        unsigned int Attr24_Lowhigh            : 1;  // Attribute24 Px/Py Low/High in AB (0~47)
        unsigned int Attr25_Lowhigh            : 1;  // Attribute25 Px/Py Low/High in AB (0~47)
        unsigned int Attr26_Lowhigh            : 1;  // Attribute26 Px/Py Low/High in AB (0~47)
        unsigned int Attr27_Lowhigh            : 1;  // Attribute27 Px/Py Low/High in AB (0~47)
        unsigned int Attr28_Lowhigh            : 1;  // Attribute28 Px/Py Low/High in AB (0~47)
        unsigned int Attr29_Lowhigh            : 1;  // Attribute29 Px/Py Low/High in AB (0~47)
        unsigned int Attr30_Lowhigh            : 1;  // Attribute30 Px/Py Low/High in AB (0~47)
        unsigned int Attr31_Lowhigh            : 1;  // Attribute31 Px/Py Low/High in AB (0~47)
    } reg;
} Reg_Iu_Pxpy_Lowhigh;

#define Reg_IU_OUTPUT_FORMAT_Attr0_Fmt_BitField_MaxValue ((1u << 1) - 1)
#define Reg_IU_OUTPUT_FORMAT_Attr1_Fmt_BitField_MaxValue ((1u << 1) - 1)
#define Reg_IU_OUTPUT_FORMAT_Attr2_Fmt_BitField_MaxValue ((1u << 1) - 1)
#define Reg_IU_OUTPUT_FORMAT_Attr3_Fmt_BitField_MaxValue ((1u << 1) - 1)
#define Reg_IU_OUTPUT_FORMAT_Attr4_Fmt_BitField_MaxValue ((1u << 1) - 1)
#define Reg_IU_OUTPUT_FORMAT_Attr5_Fmt_BitField_MaxValue ((1u << 1) - 1)
#define Reg_IU_OUTPUT_FORMAT_Attr6_Fmt_BitField_MaxValue ((1u << 1) - 1)
#define Reg_IU_OUTPUT_FORMAT_Attr7_Fmt_BitField_MaxValue ((1u << 1) - 1)
#define Reg_IU_OUTPUT_FORMAT_Attr8_Fmt_BitField_MaxValue ((1u << 1) - 1)
#define Reg_IU_OUTPUT_FORMAT_Attr9_Fmt_BitField_MaxValue ((1u << 1) - 1)
#define Reg_IU_OUTPUT_FORMAT_Attr10_Fmt_BitField_MaxValue ((1u << 1) - 1)
#define Reg_IU_OUTPUT_FORMAT_Attr11_Fmt_BitField_MaxValue ((1u << 1) - 1)
#define Reg_IU_OUTPUT_FORMAT_Attr12_Fmt_BitField_MaxValue ((1u << 1) - 1)
#define Reg_IU_OUTPUT_FORMAT_Attr13_Fmt_BitField_MaxValue ((1u << 1) - 1)
#define Reg_IU_OUTPUT_FORMAT_Attr14_Fmt_BitField_MaxValue ((1u << 1) - 1)
#define Reg_IU_OUTPUT_FORMAT_Attr15_Fmt_BitField_MaxValue ((1u << 1) - 1)
#define Reg_IU_OUTPUT_FORMAT_Attr16_Fmt_BitField_MaxValue ((1u << 1) - 1)
#define Reg_IU_OUTPUT_FORMAT_Attr17_Fmt_BitField_MaxValue ((1u << 1) - 1)
#define Reg_IU_OUTPUT_FORMAT_Attr18_Fmt_BitField_MaxValue ((1u << 1) - 1)
#define Reg_IU_OUTPUT_FORMAT_Attr19_Fmt_BitField_MaxValue ((1u << 1) - 1)
#define Reg_IU_OUTPUT_FORMAT_Attr20_Fmt_BitField_MaxValue ((1u << 1) - 1)
#define Reg_IU_OUTPUT_FORMAT_Attr21_Fmt_BitField_MaxValue ((1u << 1) - 1)
#define Reg_IU_OUTPUT_FORMAT_Attr22_Fmt_BitField_MaxValue ((1u << 1) - 1)
#define Reg_IU_OUTPUT_FORMAT_Attr23_Fmt_BitField_MaxValue ((1u << 1) - 1)
#define Reg_IU_OUTPUT_FORMAT_Attr24_Fmt_BitField_MaxValue ((1u << 1) - 1)
#define Reg_IU_OUTPUT_FORMAT_Attr25_Fmt_BitField_MaxValue ((1u << 1) - 1)
#define Reg_IU_OUTPUT_FORMAT_Attr26_Fmt_BitField_MaxValue ((1u << 1) - 1)
#define Reg_IU_OUTPUT_FORMAT_Attr27_Fmt_BitField_MaxValue ((1u << 1) - 1)
#define Reg_IU_OUTPUT_FORMAT_Attr28_Fmt_BitField_MaxValue ((1u << 1) - 1)
#define Reg_IU_OUTPUT_FORMAT_Attr29_Fmt_BitField_MaxValue ((1u << 1) - 1)
#define Reg_IU_OUTPUT_FORMAT_Attr30_Fmt_BitField_MaxValue ((1u << 1) - 1)
#define Reg_IU_OUTPUT_FORMAT_Attr31_Fmt_BitField_MaxValue ((1u << 1) - 1)

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Attr0_Fmt                 : 1;  // attribute0's data format
        unsigned int Attr1_Fmt                 : 1;  // attribute1's data format
        unsigned int Attr2_Fmt                 : 1;  // attribute2's data format
        unsigned int Attr3_Fmt                 : 1;  // attribute3's data format
        unsigned int Attr4_Fmt                 : 1;  // attribute4's data format
        unsigned int Attr5_Fmt                 : 1;  // attribute5's data format
        unsigned int Attr6_Fmt                 : 1;  // attribute6's data format
        unsigned int Attr7_Fmt                 : 1;  // attribute7's data format
        unsigned int Attr8_Fmt                 : 1;  // attribute8's data format
        unsigned int Attr9_Fmt                 : 1;  // attribute9's data format
        unsigned int Attr10_Fmt                : 1;  // attribute10's data format
        unsigned int Attr11_Fmt                : 1;  // attribute11's data format
        unsigned int Attr12_Fmt                : 1;  // attribute12's data format
        unsigned int Attr13_Fmt                : 1;  // attribute13's data format
        unsigned int Attr14_Fmt                : 1;  // attribute14's data format
        unsigned int Attr15_Fmt                : 1;  // attribute15's data format
        unsigned int Attr16_Fmt                : 1;  // attribute16's data format
        unsigned int Attr17_Fmt                : 1;  // attribute17's data format
        unsigned int Attr18_Fmt                : 1;  // attribute18's data format
        unsigned int Attr19_Fmt                : 1;  // attribute19's data format
        unsigned int Attr20_Fmt                : 1;  // attribute20's data format
        unsigned int Attr21_Fmt                : 1;  // attribute21's data format
        unsigned int Attr22_Fmt                : 1;  // attribute22's data format
        unsigned int Attr23_Fmt                : 1;  // attribute23's data format
        unsigned int Attr24_Fmt                : 1;  // attribute24's data format
        unsigned int Attr25_Fmt                : 1;  // attribute25's data format
        unsigned int Attr26_Fmt                : 1;  // attribute26's data format
        unsigned int Attr27_Fmt                : 1;  // attribute27's data format
        unsigned int Attr28_Fmt                : 1;  // attribute28's data format
        unsigned int Attr29_Fmt                : 1;  // attribute29's data format
        unsigned int Attr30_Fmt                : 1;  // attribute30's data format
        unsigned int Attr31_Fmt                : 1;  // attribute31's data format
    } reg;
} Reg_Iu_Output_Format;

#define Reg_IU_VIEWPORTZVALUE_Viewportzmin_BitField_MaxValue ((1u << 32) - 1)

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Viewportzmin              : 32;  // viewport z value
    } reg;
} Reg_Iu_Viewportzvalue;

#define Reg_IU_BUILTIN_ATTR_PACKING_Faceid_Dst_Crfaddr_BitField_MaxValue ((1u << 9) - 1)
#define Reg_IU_BUILTIN_ATTR_PACKING_Sampleid_Dst_Crfaddr_BitField_MaxValue ((1u << 9) - 1)
#define Reg_IU_BUILTIN_ATTR_PACKING_Reserved_BitField_MaxValue ((1u << 14) - 1)

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Faceid_Dst_Crfaddr        : 9;  // Bit8 : packed to fp or hp attribute, 0: fp, 1: hp Bit7 ~
                                                     // Bit2 : the packed normal attribute crf's addr Bit1 ~ Bit0
                                                     // : the packed channel mask.(0 :R, 1: G, 2:B, 3:A
        unsigned int Sampleid_Dst_Crfaddr      : 9;  // Bit8 : packed to fp or hp attribute, 0: fp, 1: hp Bit7 ~
                                                     // Bit2 : the packed normal attribute crf's addr Bit1 ~ Bit0
                                                     // : the packed channel mask.(0 :R, 1: G, 2:B, 3:A
        unsigned int Reserved                  : 14;  // Reserved
    } reg;
} Reg_Iu_Builtin_Attr_Packing;

#define Reg_PROGRAMMABLE_MSAA_PATTERN_XY_S0_Xy_BitField_MaxValue ((1u << 8) - 1)
#define Reg_PROGRAMMABLE_MSAA_PATTERN_XY_S1_Xy_BitField_MaxValue ((1u << 8) - 1)
#define Reg_PROGRAMMABLE_MSAA_PATTERN_XY_S2_Xy_BitField_MaxValue ((1u << 8) - 1)
#define Reg_PROGRAMMABLE_MSAA_PATTERN_XY_S3_Xy_BitField_MaxValue ((1u << 8) - 1)

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int S0_Xy                     : 8;
        unsigned int S1_Xy                     : 8;
        unsigned int S2_Xy                     : 8;
        unsigned int S3_Xy                     : 8;
    } reg;
} Reg_Programmable_Msaa_Pattern_Xy;



typedef struct _Group_Programmable_Msaa_Pattern
{
    Reg_Programmable_Msaa_Pattern_Xy reg_Programmable_Msaa_Pattern_Xy;
} Reg_Programmable_Msaa_Pattern_Group;

#define Reg_IU_CTRL_EX2_Tir_En_BitField_MaxValue ((1u << 1) - 1)
#define Reg_IU_CTRL_EX2_Rt_Resolution_BitField_MaxValue ((1u << 3) - 1)
#define Reg_IU_CTRL_EX2_Rov_En_BitField_MaxValue ((1u << 1) - 1)
#define Reg_IU_CTRL_EX2_Multi_Osharp_BitField_MaxValue ((1u << 1) - 1)
#define Reg_IU_CTRL_EX2_Reserved_BitField_MaxValue ((1u << 26) - 1)

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Tir_En                    : 1;  // Whether TIR is enabled
        unsigned int Rt_Resolution             : 3;  // RenderTarget Resolution
        unsigned int Rov_En                    : 1;  // ROV enable
        unsigned int Multi_Osharp : 1;  // if current is multiple o sharp, set on this filed
        unsigned int Reserved : 26;  // Reserved
    } reg;
} Reg_Iu_Ctrl_Ex2;

#define Reg_IU_RESERVED_DW_Reserved_BitField_MaxValue ((1u << 32) - 1)

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Reserved                  : 32;  // reserved
    } reg;
} Reg_Iu_Reserved_Dw;

typedef struct _Iu_regs_CHX004
{
    Reg_Iu_Ctrl                      reg_Iu_Ctrl;
    Reg_Iu_Ctrl_Ex                   reg_Iu_Ctrl_Ex;
    Reg_Iu_Rt_Size                   reg_Iu_Rt_Size;
    Reg_Iu_Mapping                   reg_Iu_Mapping[10];
    Reg_Iu_Intp_Mode                 reg_Iu_Intp_Mode[5];
    Reg_Iu_Clip_Distance             reg_Iu_Clip_Distance[2];
    Reg_Iu_Crf_Baseaddr              reg_Iu_Crf_Baseaddr[10];
    Reg_Iu_Pxpy_Crf_Baseaddr         reg_Iu_Pxpy_Crf_Baseaddr[10];
    Reg_Iu_Pxpy_Lowhigh              reg_Iu_Pxpy_Lowhigh[2];
    Reg_Iu_Output_Format             reg_Iu_Output_Format[2];
    Reg_Iu_Viewportzvalue            reg_Iu_Viewportzvalue[16];
    Reg_Iu_Builtin_Attr_Packing      reg_Iu_Builtin_Attr_Packing;
    Reg_Programmable_Msaa_Pattern_Group
                                     reg_Programmable_Msaa_Pattern[4];
    Reg_Iu_Ctrl_Ex2                  reg_Iu_Ctrl_Ex2;
    Reg_Iu_Reserved_Dw               reg_Iu_Reserved_Dw[6];
} Iu_regs_CHX004;

typedef struct _Iu_regs
{
    Reg_Iu_Ctrl                      reg_Iu_Ctrl;
    Reg_Iu_Ctrl_Ex                   reg_Iu_Ctrl_Ex;
    Reg_Iu_Rt_Size                   reg_Iu_Rt_Size;
    Reg_Iu_Mapping                   reg_Iu_Mapping[10];
    Reg_Iu_Intp_Mode                 reg_Iu_Intp_Mode[5];
    Reg_Iu_Clip_Distance             reg_Iu_Clip_Distance[2];
    Reg_Iu_Crf_Baseaddr              reg_Iu_Crf_Baseaddr[10];
    Reg_Iu_Pxpy_Crf_Baseaddr         reg_Iu_Pxpy_Crf_Baseaddr[10];
    Reg_Iu_Pxpy_Lowhigh              reg_Iu_Pxpy_Lowhigh[2];
    Reg_Iu_Output_Format             reg_Iu_Output_Format[2];
    Reg_Iu_Viewportzvalue            reg_Iu_Viewportzvalue[16];
    Reg_Iu_Builtin_Attr_Packing      reg_Iu_Builtin_Attr_Packing;
    Reg_Iu_Reserved_Dw               reg_Iu_Reserved_Dw[3];
} Iu_regs;

#endif
