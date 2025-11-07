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
//    Spec Last Modified Time: 2019/8/21 8:54:48
#ifndef _TASFE_REGISTERS_H
#define _TASFE_REGISTERS_H


#ifndef        TASFE_BLOCKBASE_INF
    #define    TASFE_BLOCKBASE_INF
    #define    BLOCK_TASFE_VERSION 1
    #define    BLOCK_TASFE_TIMESTAMP "2019/8/21 8:54:48"
    #define    TASFE_BLOCK                                                0x4 // match with BlockID.h
    #define    TASFE_REG_START                                            0x0 // match with BlockID.h
    #define    TASFE_REG_END_CHX004                                       0xE8 // match with BlockID.h
    #define    TASFE_REG_LIMIT_CHX004                                     0xE8 // match with BlockID.h
    #define    TASFE_REG_END_Elt3k                                        0xC0 // match with BlockID.h
    #define    TASFE_REG_LIMIT_Elt3k                                      0xC0 // match with BlockID.h
#endif

// Register offset definition
#define        Reg_Tasfe_Ctrl_Offset                                      0x0
#define        Reg_Tasfe_Ctrl_Misc_Offset                                 0x1
#define        Reg_Pre_Setup_Ctrl_Offset                                  0x2
#define        Reg_Gb_Exp_Offset                                          0x3
#define        Reg_Scissor_X_Offset                                       0xB
#define        Reg_Scissor_Y_Offset                                       0x1B
#define        Reg_Viewport_Xscale_Offset                                 0x2B
#define        Reg_Viewport_Xoffset_Offset                                0x3B
#define        Reg_Viewport_Yscale_Offset                                 0x4B
#define        Reg_Viewport_Yoffset_Offset                                0x5B
#define        Reg_Viewport_Zscale_Offset                                 0x6B
#define        Reg_Viewport_Zoffset_Offset                                0x7B
#define        Reg_Wscaling_Xcoeff_Offset                                 0x8B
#define        Reg_Wscaling_Ycoeff_Offset                                 0x9B
#define        Reg_Viewport_Swizzle_Offset                                0xAB
#define        Reg_Point_Size_Offset                                      0xB3
#define        Reg_Line_Width_Offset                                      0xB4
#define        Reg_Z_Bias_Offset                                          0xB5
#define        Reg_Z_Scale_Factor_Offset                                  0xB6
#define        Reg_Z_Bias_Clamp_Offset                                    0xB7
#define        Reg_Attr_Const_Persp_Offset                                0xB8
#define        Reg_Tasfe_Reserved_Offset                                  0xBB
#define        Reg_Viewport_X_Offset                                      0xBB
#define        Reg_Viewport_Y_Offset                                      0xCB
#define        Reg_Rt_Max_Offset                                          0xDB
#define        Reg_Ymin_Offset_Offset                                     0xDC
#define        Reg_Ymax_Offset_Offset                                     0xE2

// Block constant definition
typedef enum
{
    TASFE_CTRL_AS_EN_DISABLED                = 0,   // 0: attribute setup disabled
    TASFE_CTRL_AS_EN_ENABLED                 = 1,   // 1: attribute setup enabled
} TASFE_CTRL_AS_EN;
typedef enum
{
    TASFE_CTRL_ZERO_DET_MODE_FORCE_REJECT    = 0,   // 0: force reject triangle
    TASFE_CTRL_ZERO_DET_MODE_BACK_FACE       = 1,   // 1: set backfacing
} TASFE_CTRL_ZERO_DET_MODE;
typedef enum
{
    TASFE_CTRL_CULL_EN_DISABLED              = 0,   // 0: disabled
    TASFE_CTRL_CULL_EN_ENABLED               = 1,   // 1: enabled
} TASFE_CTRL_CULL_EN;
typedef enum
{
    TASFE_CTRL_CULL_MODE_CULL_BACK           = 0,   // 0: cull backface
    TASFE_CTRL_CULL_MODE_CULL_FRONT          = 1,   // 1: cull frontface
} TASFE_CTRL_CULL_MODE;
typedef enum
{
    TASFE_CTRL_WINDING_FRONT_CW              = 0,   // 0: front cw
    TASFE_CTRL_WINDING_FRONT_CCW             = 1,   // 1: front ccw
} TASFE_CTRL_WINDING;
typedef enum
{
    TASFE_CTRL_ZCLIP_EN_DISABLED             = 0,   // 0: disabled
    TASFE_CTRL_ZCLIP_EN_ENABLED              = 1,   // 1: enabled
} TASFE_CTRL_ZCLIP_EN;
typedef enum
{
    TASFE_CTRL_ZOFFSET_FILL_EN_DISABLED      = 0,   // 0: disabled
    TASFE_CTRL_ZOFFSET_FILL_EN_ENABLED       = 1,   // 1: enabled
} TASFE_CTRL_ZOFFSET_FILL_EN;
typedef enum
{
    TASFE_CTRL_ZOFFSET_LINE_EN_DISABLED      = 0,   // 0: disabled
    TASFE_CTRL_ZOFFSET_LINE_EN_ENABLED       = 1,   // 1: enabled
} TASFE_CTRL_ZOFFSET_LINE_EN;
typedef enum
{
    TASFE_CTRL_ZOFFSET_POINT_EN_DISABLED     = 0,   // 0: disabled
    TASFE_CTRL_ZOFFSET_POINT_EN_ENABLED      = 1,   // 1: enabled
} TASFE_CTRL_ZOFFSET_POINT_EN;
typedef enum
{
    TASFE_CTRL_STRICT_LINE_EN_DISABLED       = 0,   // 0: disabled
    TASFE_CTRL_STRICT_LINE_EN_ENABLED        = 1,   // 1: enabled
} TASFE_CTRL_STRICT_LINE_EN;
typedef enum
{
    TASFE_CTRL_NON_PERSPECTIVE_EN_DISABLED   = 0,   // 0: disabled
    TASFE_CTRL_NON_PERSPECTIVE_EN_ENABLED    = 1,   // 1: enabled
} TASFE_CTRL_NON_PERSPECTIVE_EN;
typedef enum
{
    TASFE_CTRL_PTTRI_EN_DISABLE              = 0,   // 0: point triangle disabled
    TASFE_CTRL_PTTRI_EN_ENABLE               = 1,   // 1: point triangle enabled
} TASFE_CTRL_PTTRI_EN;
typedef enum
{
    TASFE_CTRL_POINT_SIZE_EN_DISABLED        = 0,   // 0: disabled, use point_size register for point setup.
                                                    // Point sprite must be disabled.
    TASFE_CTRL_POINT_SIZE_EN_ENABLED         = 1,   // 1: enabled, the point size is in VS output.
} TASFE_CTRL_POINT_SIZE_EN;
typedef enum
{
    TASFE_CTRL_LAST_PIXEL_DISABLED           = 0,   // 0: draw last pixel disabled.
    TASFE_CTRL_LAST_PIXEL_ENABLED            = 1,   // 1: draw last pixel enabled.
} TASFE_CTRL_LAST_PIXEL;
typedef enum
{
    TASFE_CTRL_LEADING_VTX_VTX0              = 0,   // for DX9, 0 means vertex 0, for D3D list and strip
                                                    //  for DX10/OGL, 0 mean FIRST_VTX.
    TASFE_CTRL_LEADING_VTX_VTX1              = 1,   // for DX9, 1 means vertex 1, for D3D fan.
                                                    //  For DX10/OGL, 1 means LAST_VTX
} TASFE_CTRL_LEADING_VTX;
typedef enum
{
    TASFE_CTRL_Z32F_EN_DISABLED              = 0,   // 0: z24 or z16
    TASFE_CTRL_Z32F_EN_ENABLED               = 1,   // 1: z32f
} TASFE_CTRL_Z32F_EN;
typedef enum
{
    TASFE_CTRL_Z_SETUP_MODE_LINE_BASED       = 0,   // 0: line-based
    TASFE_CTRL_Z_SETUP_MODE_TRIANGLE_BASED   = 1,   // 1: triangle-based
} TASFE_CTRL_Z_SETUP_MODE;
typedef enum
{
    TASFE_CTRL_RHW_SETUP_MODE_LINE_BASED     = 0,   // 0: line-based
    TASFE_CTRL_RHW_SETUP_MODE_TRIANGLE_BASED= 1,    // 1: triangle-based
} TASFE_CTRL_RHW_SETUP_MODE;
typedef enum
{
    TASFE_CTRL_FRONT_POLYGON_MODE_SOLID      = 0,   // 0: solid triangle
    TASFE_CTRL_FRONT_POLYGON_MODE_WIREFRAME  = 1,   // 1: wireframe triangle
    TASFE_CTRL_FRONT_POLYGON_MODE_POINT      = 2,   // 2:point triangle
} TASFE_CTRL_FRONT_POLYGON_MODE;
typedef enum
{
    TASFE_CTRL_BACK_POLYGON_MODE_SOLID       = 0,   // 0: solid triangle
    TASFE_CTRL_BACK_POLYGON_MODE_WIREFRAME   = 1,   // 1: wireframe triangle
    TASFE_CTRL_BACK_POLYGON_MODE_POINT       = 2,   // 2:point triangle
} TASFE_CTRL_BACK_POLYGON_MODE;
typedef enum
{
    TASFE_CTRL_DEPTH_MODE_ZERO_TO_ONE        = 0,   // 0: zero to one
    TASFE_CTRL_DEPTH_MODE_NEGATIVE_ONE_TO_ONE= 1,   // 1: negative one to one
} TASFE_CTRL_DEPTH_MODE;
typedef enum
{
    TASFE_CTRL_FORCE_Z_GBC_FALSE             = 0,   // 0: False
    TASFE_CTRL_FORCE_Z_GBC_TRUE              = 1,   // 1: True
} TASFE_CTRL_FORCE_Z_GBC;
typedef enum
{
    TASFE_CTRL_API_VERSION_OGL               = 0,   // 000: OGL
    TASFE_CTRL_API_VERSION_DX9               = 1,   // 001: DX9 or lower
    TASFE_CTRL_API_VERSION_DX10              = 2,   // 010: DX10.0 and Dx10.1
    TASFE_CTRL_API_VERSION_DX11              = 3,   // 011: DX11
    TASFE_CTRL_API_VERSION_DX12              = 4,   // 100: DX12
} TASFE_CTRL_API_VERSION;
typedef enum
{
    TASFE_CTRL_MSAA_STATE_MSAA_1X            = 0,   // 0: 1 samples per pixel
    TASFE_CTRL_MSAA_STATE_MSAA_2X            = 1,   // 1: 2 samples per pixel
    TASFE_CTRL_MSAA_STATE_MSAA_4X            = 2,   // 2: 4 samples per pixel
    TASFE_CTRL_MSAA_STATE_MSAA_8X            = 3,   // 3: 8 samples per pixel
    TASFE_CTRL_MSAA_STATE_MSAA_16X           = 4,   // 4: 16 samples per pixel
} TASFE_CTRL_MSAA_STATE;
typedef enum
{
    TASFE_CTRL_AA_EN_FALSE                   = 0,   // 0: False
    TASFE_CTRL_AA_EN_TRUE                    = 1,   // 1: True
} TASFE_CTRL_AA_EN;
typedef enum
{
    TASFE_CTRL_RASTER_MODE_NORMAL            = 0,   // 0:normal
    TASFE_CTRL_RASTER_MODE_CONSERVATIVE      = 1,   // 1:conservative
} TASFE_CTRL_RASTER_MODE;
typedef enum
{
    TASFE_CTRL_MISC_VIEWPORT_IDX_EN_DISABLED= 0,    // 0: Disable
    TASFE_CTRL_MISC_VIEWPORT_IDX_EN_ENABLED  = 1,   // 1: Enable
} TASFE_CTRL_MISC_VIEWPORT_IDX_EN;
typedef enum
{
    TASFE_CTRL_MISC_STO_EN_DISABLED          = 0,   // 0: Disable
    TASFE_CTRL_MISC_STO_EN_ENABLED           = 1,   // 1: Enable
} TASFE_CTRL_MISC_STO_EN;
typedef enum
{
    TASFE_CTRL_MISC_POINT_SPRITE_EN_DISABLED= 0,    // 0: disabled
    TASFE_CTRL_MISC_POINT_SPRITE_EN_ENABLED  = 1,   // 1: enabled, use point size in VS output to setup point
                                                    // sprite UV
} TASFE_CTRL_MISC_POINT_SPRITE_EN;
typedef enum
{
    TASFE_CTRL_MISC_TILE_SIZE_256X128        = 0,   // 32bpp, 1RT, 4slice, MSAA1x
    TASFE_CTRL_MISC_TILE_SIZE_128X128        = 1,
    TASFE_CTRL_MISC_TILE_SIZE_128X64         = 2,
    TASFE_CTRL_MISC_TILE_SIZE_64X64          = 3,
    TASFE_CTRL_MISC_TILE_SIZE_64X32          = 4,
    TASFE_CTRL_MISC_TILE_SIZE_32X32          = 5,
    TASFE_CTRL_MISC_TILE_SIZE_32X16          = 6,
    TASFE_CTRL_MISC_TILE_SIZE_16X16          = 7,
    TASFE_CTRL_MISC_TILE_SIZE_16X8           = 8,
    TASFE_CTRL_MISC_TILE_SIZE_8X8            = 9,   // 128bpp, 8rt, 4slice, MSAA16x
} TASFE_CTRL_MISC_TILE_SIZE;
typedef enum
{
    TASFE_CTRL_MISC_SPECIAL_ROTATE_FALSE     = 0,   // 0: False
    TASFE_CTRL_MISC_SPECIAL_ROTATE_TRUE      = 1,   // 1: True
} TASFE_CTRL_MISC_SPECIAL_ROTATE;
typedef enum
{
    TASFE_CTRL_MISC_PRIMID_APPEND_EN_FALSE   = 0,   // 0: False
    TASFE_CTRL_MISC_PRIMID_APPEND_EN_TRUE    = 1,   // 1: True, append Pid after all other attributes
} TASFE_CTRL_MISC_PRIMID_APPEND_EN;
typedef enum
{
    TASFE_CTRL_MISC_RULE_FOR_TRIANGLE_TOP_LEFT= 0,  // 00: When the edge is top or left, it will be drawn.
    TASFE_CTRL_MISC_RULE_FOR_TRIANGLE_TOP_RIGHT= 1, // 01: When the edge is top or right, it will be drawn.
    TASFE_CTRL_MISC_RULE_FOR_TRIANGLE_BOTTOM_LEFT= 2,
                                                    // 10: When the edge is bottom or left, it will be drawn.
    TASFE_CTRL_MISC_RULE_FOR_TRIANGLE_BOTTON_RIGHT= 3,
                                                    // 11: When the edge is bottom or right, it will be drawn.
} TASFE_CTRL_MISC_RULE_FOR_TRIANGLE;
typedef enum
{
    TASFE_CTRL_MISC_QUAD_LINE_EN_FALSE       = 0,   // 0: False
    TASFE_CTRL_MISC_QUAD_LINE_EN_TRUE        = 1,   // 1: True, setup qudrilateral line
} TASFE_CTRL_MISC_QUAD_LINE_EN;
typedef enum
{
    PRE_SETUP_CTRL_PRIMID_PACK_EN_FALSE      = 0,   // 0: False
    PRE_SETUP_CTRL_PRIMID_PACK_EN_TRUE       = 1,   // 1: True
} PRE_SETUP_CTRL_PRIMID_PACK_EN;
typedef enum
{
    PRE_SETUP_CTRL_PTSPRITE_PERSP_FALSE      = 0,   // 0: False
    PRE_SETUP_CTRL_PTSPRITE_PERSP_TRUE       = 1,   // 1: True, need multiply rhw for ps/px/py
} PRE_SETUP_CTRL_PTSPRITE_PERSP;
typedef enum
{
    PRE_SETUP_CTRL_EDGE_FLAG_EN_FALSE        = 0,   // 0: False
    PRE_SETUP_CTRL_EDGE_FLAG_EN_TRUE         = 1,   // 1: True
} PRE_SETUP_CTRL_EDGE_FLAG_EN;
typedef enum
{
    PRE_SETUP_CTRL_VIEWPORT_MASK_EN_FALSE    = 0,   // 0: False
    PRE_SETUP_CTRL_VIEWPORT_MASK_EN_TRUE     = 1,   // 1: True
} PRE_SETUP_CTRL_VIEWPORT_MASK_EN;
typedef enum
{
    PRE_SETUP_CTRL_VIEWPORT_SWIZZLE_EN_FALSE= 0,    // 0: False
    PRE_SETUP_CTRL_VIEWPORT_SWIZZLE_EN_TRUE  = 1,   // 1: True
} PRE_SETUP_CTRL_VIEWPORT_SWIZZLE_EN;
typedef enum
{
    PRE_SETUP_CTRL_VIEWPORT_RELATIVE_FALSE   = 0,   // 0: False
    PRE_SETUP_CTRL_VIEWPORT_RELATIVE_TRUE    = 1,   // 1: True
} PRE_SETUP_CTRL_VIEWPORT_RELATIVE;
typedef enum
{
    PRE_SETUP_CTRL_W_SCALING_EN_FALSE        = 0,   // 0: False
    PRE_SETUP_CTRL_W_SCALING_EN_TRUE         = 1,   // 1: True
} PRE_SETUP_CTRL_W_SCALING_EN;
typedef enum
{
    PRE_SETUP_CTRL_TRANSFORM_EN_FALSE        = 0,   // 0: False
    PRE_SETUP_CTRL_TRANSFORM_EN_TRUE         = 1,   // 1: True
} PRE_SETUP_CTRL_TRANSFORM_EN;
typedef enum
{
    PRE_SETUP_CTRL_DO_CLIPPING_FOR_POINT_FALSE= 0,  // 0: False
    PRE_SETUP_CTRL_DO_CLIPPING_FOR_POINT_TRUE= 1,   // 1: True
} PRE_SETUP_CTRL_DO_CLIPPING_FOR_POINT;
typedef enum
{
    PRE_SETUP_CTRL_PTSPRITE_INVERT_POSITIVE_U_POSITIVE_V= 0,
                                                    // 0: U slope=(Ux,0,0.5) V slope=(0,Vy,0.5)
    PRE_SETUP_CTRL_PTSPRITE_INVERT_POSITIVE_U_NEGATIVE_V= 1,
                                                    // 1: U slope=(Ux,0,0.5) V slope=(0,-Vy,0.5)
    PRE_SETUP_CTRL_PTSPRITE_INVERT_NEGATIVE_U_POSITIVE_V= 2,
                                                    // 2: U slope=(-Ux,0,0.5) V slope=(0,Vy,0.5)
    PRE_SETUP_CTRL_PTSPRITE_INVERT_NEGATIVE_U_NEGATIVE_V= 3,
                                                    // 3: U slope=(-Ux,0,0.5) V slope=(0,-Vy,0.5)
    PRE_SETUP_CTRL_PTSPRITE_INVERT_POSITIVE_V_POSITIVE_U= 4,
                                                    // 4: U slope=(0,Vy,0.5) V slope=(Ux,0,0.5)
    PRE_SETUP_CTRL_PTSPRITE_INVERT_POSITIVE_V_NEGATIVE_U= 5,
                                                    // 5: U slope=(0,Vy,0.5) V slope=(-Ux,0,0.5)
    PRE_SETUP_CTRL_PTSPRITE_INVERT_NEGATIVE_V_POSITIVE_U= 6,
                                                    // 6: U slope=(0,-Vy,0.5) V slope=(Ux,0,0.5)
    PRE_SETUP_CTRL_PTSPRITE_INVERT_NEGATIVE_V_NEGATIVE_U= 7,
                                                    // 7: U slope=(0,-Vy,0.5) V slope=(-Ux,0,0.5)
} PRE_SETUP_CTRL_PTSPRITE_INVERT;
typedef enum
{
    PRE_SETUP_CTRL_SKIP_FRUSTUM_REJ_FALSE    = 0,   // 0: False
    PRE_SETUP_CTRL_SKIP_FRUSTUM_REJ_TRUE     = 1,   // 1: True
} PRE_SETUP_CTRL_SKIP_FRUSTUM_REJ;
typedef enum
{
    PRE_SETUP_CTRL_SKIP_SMALLTRI_REJ_FALSE   = 0,   // 0: False, perform small triangle reject
    PRE_SETUP_CTRL_SKIP_SMALLTRI_REJ_TRUE    = 1,   // 1: True, skip small triangle reject
} PRE_SETUP_CTRL_SKIP_SMALLTRI_REJ;
typedef enum
{
    VIEWPORT_SWIZZLE_X_SWIZZLE0__POSITIVE_X  = 0,   // x'=x
    VIEWPORT_SWIZZLE_X_SWIZZLE0_NEGATIVE_X   = 1,   // x'=-x
    VIEWPORT_SWIZZLE_X_SWIZZLE0__POSITIVE_Y  = 2,   // x'=y
    VIEWPORT_SWIZZLE_X_SWIZZLE0_NEGATIVE_Y   = 3,   // x'=-y
    VIEWPORT_SWIZZLE_X_SWIZZLE0__POSITIVE_Z  = 4,   // x'=z
    VIEWPORT_SWIZZLE_X_SWIZZLE0_NEGATIVE_Z   = 5,   // x'=-z
    VIEWPORT_SWIZZLE_X_SWIZZLE0__POSITIVE_W  = 6,   // x'=w
    VIEWPORT_SWIZZLE_X_SWIZZLE0_NEGATIVE_W   = 7,   // x'=-w
} VIEWPORT_SWIZZLE_X_SWIZZLE0;
typedef enum
{
    VIEWPORT_SWIZZLE_Y_SWIZZLE0__POSITIVE_X  = 0,   // y'=x
    VIEWPORT_SWIZZLE_Y_SWIZZLE0_NEGATIVE_X   = 1,   // y'=-x
    VIEWPORT_SWIZZLE_Y_SWIZZLE0__POSITIVE_Y  = 2,   // y'=y
    VIEWPORT_SWIZZLE_Y_SWIZZLE0_NEGATIVE_Y   = 3,   // y'=-y
    VIEWPORT_SWIZZLE_Y_SWIZZLE0__POSITIVE_Z  = 4,   // y'=z
    VIEWPORT_SWIZZLE_Y_SWIZZLE0_NEGATIVE_Z   = 5,   // y'=-z
    VIEWPORT_SWIZZLE_Y_SWIZZLE0__POSITIVE_W  = 6,   // y'=w
    VIEWPORT_SWIZZLE_Y_SWIZZLE0_NEGATIVE_W   = 7,   // y'=-w
} VIEWPORT_SWIZZLE_Y_SWIZZLE0;
typedef enum
{
    VIEWPORT_SWIZZLE_Z_SWIZZLE0__POSITIVE_X  = 0,   // z'=x
    VIEWPORT_SWIZZLE_Z_SWIZZLE0_NEGATIVE_X   = 1,   // z'=-x
    VIEWPORT_SWIZZLE_Z_SWIZZLE0__POSITIVE_Y  = 2,   // z'=y
    VIEWPORT_SWIZZLE_Z_SWIZZLE0_NEGATIVE_Y   = 3,   // z'=-y
    VIEWPORT_SWIZZLE_Z_SWIZZLE0__POSITIVE_Z  = 4,   // z'=z
    VIEWPORT_SWIZZLE_Z_SWIZZLE0_NEGATIVE_Z   = 5,   // z'=-z
    VIEWPORT_SWIZZLE_Z_SWIZZLE0__POSITIVE_W  = 6,   // z'=w
    VIEWPORT_SWIZZLE_Z_SWIZZLE0_NEGATIVE_W   = 7,   // z'=-w
} VIEWPORT_SWIZZLE_Z_SWIZZLE0;
typedef enum
{
    VIEWPORT_SWIZZLE_W_SWIZZLE0__POSITIVE_X  = 0,   // w'=x
    VIEWPORT_SWIZZLE_W_SWIZZLE0_NEGATIVE_X   = 1,   // w'=-x
    VIEWPORT_SWIZZLE_W_SWIZZLE0__POSITIVE_Y  = 2,   // w'=y
    VIEWPORT_SWIZZLE_W_SWIZZLE0_NEGATIVE_Y   = 3,   // w'=-y
    VIEWPORT_SWIZZLE_W_SWIZZLE0__POSITIVE_Z  = 4,   // w'=z
    VIEWPORT_SWIZZLE_W_SWIZZLE0_NEGATIVE_Z   = 5,   // w'=-z
    VIEWPORT_SWIZZLE_W_SWIZZLE0__POSITIVE_W  = 6,   // w'=w
    VIEWPORT_SWIZZLE_W_SWIZZLE0_NEGATIVE_W   = 7,   // w'=-w
} VIEWPORT_SWIZZLE_W_SWIZZLE0;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////            TASFE Block (Block ID = 4) Register Definitions                           ///////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int As_En                     : 1;   // Attribute setup enable
        unsigned int Zero_Det_Mode             : 1;   // Zero Determinate Mode
        unsigned int Cull_En                   : 1;   // Culling enable
        unsigned int Cull_Mode                 : 1;   // Culling mode
        unsigned int Winding                   : 1;   // Winding direction
        unsigned int Zclip_En                  : 1;   // Z clipping enable, used for z trivial rejection
        unsigned int Zoffset_Fill_En           : 1;   // Polygon offset or depth bias enable for fill mode
        unsigned int Zoffset_Line_En           : 1;   // Polygon offset or depth bias enable for line mode
        unsigned int Zoffset_Point_En          : 1;   // Polygon offset or depth bias enable for point mode
        unsigned int Strict_Line_En            : 1;   // Strict line enable
        unsigned int Non_Perspective_En        : 1;   // whether there's any non-perspective attribute
        unsigned int Pttri_En                  : 1;   // point triangle enabled
        unsigned int Point_Size_En             : 1;   // Point size enable
        unsigned int Last_Pixel                : 1;   // Line draw last pixel on/off
        unsigned int Leading_Vtx               : 1;   // leading vertex selection
        unsigned int Z32f_En                   : 1;   // used for z32f depth bias calculation;
        unsigned int Z_Setup_Mode              : 1;   // z setup mode for wireframe
        unsigned int Rhw_Setup_Mode            : 1;   // rhw setup mode for wireframe
        unsigned int Front_Polygon_Mode        : 2;   // fill mode of front face
        unsigned int Back_Polygon_Mode         : 2;   // fill mode of back face
        unsigned int Depth_Mode                : 1;   // Clip control depth mode
        unsigned int Force_Z_Gbc               : 1;   // force to perform guard band clipping for z
        unsigned int Api_Version               : 3;   // API Version,DX9's coordinate system is different from
                                                      // DX10+ and OGL;DX's strip winding direction is different
                                                      // from OGL
        unsigned int Msaa_State                : 3;   // used to adjust bounding box, if msaa enabled, don't care
                                                      // whether it's normal msaa or user-defined mass pattern
        unsigned int Aa_En                     : 1;   // AA mode enable
        unsigned int Raster_Mode               : 1;   // Conervative raster enable or not
    } reg;
} Reg_Tasfe_Ctrl;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Attr_Num                  : 6;   // attribute number, 0-based
        unsigned int Viewport_Idx_En           : 1;   // input viewport index enable
        unsigned int Sto_En                    : 1;   // STO enable
        unsigned int Point_Sprite_En           : 1;   // Point sprite enable
        unsigned int Tile_Size                 : 4;   // tile size such that 1 cache line holds data of one tile
        unsigned int Special_Rotate            : 1;   // 90/270 degree rotate enable
        unsigned int Primid_Append_En          : 1;   // If set enabled, it's TASFE's job to insert the primitive id
                                                      // (SP should send it) after all other attributes (if
                                                      // primid_pack_en=0) or into some attribute's some channel
                                                      // (if primid_pack_en=1), the attribute/channel index is
                                                      // indicated by primid_pack_attr and primid_pack_channel
        unsigned int Pos_End_Ptr               : 10;  // position end pointer of PTB in GPCP
        unsigned int Rule_For_Triangle         : 2;   // Triangle Edge Rule
        unsigned int Rule_For_A_Line           : 2;
        unsigned int Rule_For_Point            : 2;
        unsigned int Quad_Line_En              : 1;
    } reg;
} Reg_Tasfe_Ctrl_Misc;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Primid_Pack_En            : 1;   // primitive id pack enabled
        unsigned int Primid_Pack_Attr          : 6;   // attribute index into which the primitive id is packed,
                                                      // ranges from 0 to 47
        unsigned int Primid_Pack_Channel       : 2;   // component index into which the primitive id is packed,
                                                      // ranges from 0 to 3
        unsigned int Ptsprite_Persp            : 1;   // Whether point sprite is perspective
        unsigned int Edge_Flag_En              : 1;   // Whether edge flag is enabled
        unsigned int Reserved                  : 5;   // cull distance mask in all 8 component
        unsigned int Viewport_Mask_En          : 1;   // viewport mask enable
        unsigned int Viewport_Swizzle_En       : 1;   // viewport swizzle enable
        unsigned int Viewport_Relative         : 1;   // viewport relative enable
        unsigned int W_Scaling_En              : 1;   // w scaling enable
        unsigned int Transform_En              : 1;   // transform enable
        unsigned int Do_Clipping_For_Point     : 1;   // Do Clipping for Point
        unsigned int Ptsprite_Invert           : 3;   // point sprite invert mode
        unsigned int Skip_Frustum_Rej          : 1;   // Skip frustum reject for X and Y
        unsigned int Gb_Size                   : 5;   // guard band size
        unsigned int Skip_Smalltri_Rej         : 1;   // skip reject small triangle
    } reg;
} Reg_Pre_Setup_Ctrl;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int X_Gb_Exp0                 : 8;   // exponent[(2^19-x_offset)/x_scale]-127, x_scale and
                                                      // x_offset is the paramenter of 1st viewport
        unsigned int Y_Gb_Exp0                 : 8;   // exponent[(2^19-y_offset)/y_scale]-127,x_scale and
                                                      // x_offset is the paramenter of 1st viewport
        unsigned int X_Gb_Exp1                 : 8;   // exponent[(2^19-x_offset)/x_scale]-127, x_scale and
                                                      // x_offset is the paramenter of end viewport
        unsigned int Y_Gb_Exp1                 : 8;   // exponent[(2^19-y_offset)/y_scale]-127,x_scale and
                                                      // x_offset is the paramenter of 2nd viewport
    } reg;
} Reg_Gb_Exp;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int X_Min                     : 16;  // X minimum  (Fixed point, 16.0, only 14.0 is used)
        unsigned int X_Max                     : 16;  // X maximum (Fixed point, 16.0, only 14.0 is used)
    } reg;
} Reg_Scissor_X;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Y_Min                     : 16;  // Y minimum (Fixed point, only 16.0, only 14.0 is used)
        unsigned int Y_Max                     : 16;  // Y maximum (Fixed point, only 15.0, only 14.0 is used)
    } reg;
} Reg_Scissor_Y;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int X_Scale                   : 32;  // X scale  (Float point)
    } reg;
} Reg_Viewport_Xscale;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int X_Offset                  : 32;  // X offset (Float point)
    } reg;
} Reg_Viewport_Xoffset;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Y_Scale                   : 32;  // Y scale  (Float point)
    } reg;
} Reg_Viewport_Yscale;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Y_Offset                  : 32;  // Y offset  (Float point)
    } reg;
} Reg_Viewport_Yoffset;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Z_Scale                   : 32;  // Z scale  (Float point)
    } reg;
} Reg_Viewport_Zscale;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Z_Offset                  : 32;  // Z offset  (Float point)
    } reg;
} Reg_Viewport_Zoffset;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int X_Coeff                   : 32;  // X coefficient (Float point)
    } reg;
} Reg_Wscaling_Xcoeff;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Y_Coeff                   : 32;  // Y coefficient (Float point)
    } reg;
} Reg_Wscaling_Ycoeff;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int X_Swizzle0                : 3;   // 1
        unsigned int Y_Swizzle0                : 3;
        unsigned int Z_Swizzle0                : 3;
        unsigned int W_Swizzle0                : 3;
        unsigned int X_Swizzle1                : 3;   // 1
        unsigned int Y_Swizzle1                : 3;
        unsigned int Z_Swizzle1                : 3;
        unsigned int W_Swizzle1                : 3;
        unsigned int Reserved                  : 8;   // Reserved
    } reg;
} Reg_Viewport_Swizzle;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Point_Size                : 27;  //  (Ndot8 format) 19.8 fixed point. This controls the half
                                                      // line width inside RU.
        unsigned int Reserved                  : 5;   // Reserved
    } reg;
} Reg_Point_Size;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Line_Width                : 27;  // 19.8 fixed point. This control the line width inside RU.
        unsigned int Reserved                  : 5;   // Reserved
    } reg;
} Reg_Line_Width;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Zbias                     : 32;  // fp32
    } reg;
} Reg_Z_Bias;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Factor                    : 32;  // fp32
    } reg;
} Reg_Z_Scale_Factor;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Zbias_Clamp               : 32;  // s23e8,since TAS don't support fp28, change this register
                                                      // to standard fp32
    } reg;
} Reg_Z_Bias_Clamp;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Attr0_Const_Persp         : 2;   // Attribute 0 const intp flag, 0=const, 1=linear
                                                      // non-perspective, 2= perspective
        unsigned int Attr1_Const_Persp         : 2;   // Attribute 1 const intp flag, 0=const, 1=linear
                                                      // non-perspective, 2= perspective
        unsigned int Attr2_Const_Persp         : 2;   // Attribute 2 const intp flag, 0=const, 1=linear
                                                      // non-perspective, 3= perspective
        unsigned int Attr3_Const_Persp         : 2;   // Attribute 3 const intp flag, 0=const, 1=linear
                                                      // non-perspective, 3= perspective
        unsigned int Attr4_Const_Persp         : 2;   // Attribute 4 const intp flag, 0=const, 1=linear
                                                      // non-perspective, 3= perspective
        unsigned int Attr5_Const_Persp         : 2;   // Attribute 5 const intp flag, 0=const, 1=linear
                                                      // non-perspective, 4= perspective
        unsigned int Attr6_Const_Persp         : 2;   // Attribute 6 const intp flag, 0=const, 1=linear
                                                      // non-perspective, 5= perspective
        unsigned int Attr7_Const_Persp         : 2;   // Attribute 7 const intp flag, 0=const, 1=linear
                                                      // non-perspective, 6= perspective
        unsigned int Attr8_Const_Persp         : 2;   // Attribute 8 const intp flag, 0=const, 1=linear
                                                      // non-perspective, 7= perspective
        unsigned int Attr9_Const_Persp         : 2;   // Attribute 9 const intp flag, 0=const, 1=linear
                                                      // non-perspective, 8= perspective
        unsigned int Attr10_Const_Persp        : 2;   // Attribute 10 const intp flag, 0=const, 1=linear
                                                      // non-perspective, 9= perspective
        unsigned int Attr11_Const_Persp        : 2;   // Attribute 11 const intp flag, 0=const, 1=linear
                                                      // non-perspective, 10= perspective
        unsigned int Attr12_Const_Persp        : 2;   // Attribute 12 const intp flag, 0=const, 1=linear
                                                      // non-perspective, 11= perspective
        unsigned int Attr13_Const_Persp        : 2;   // Attribute 13 const intp flag, 0=const, 1=linear
                                                      // non-perspective, 12= perspective
        unsigned int Attr14_Const_Persp        : 2;   // Attribute 14 const intp flag, 0=const, 1=linear
                                                      // non-perspective, 13= perspective
        unsigned int Attr15_Const_Persp        : 2;   // Attribute 15 const intp flag, 0=const, 1=linear
                                                      // non-perspective, 14= perspective
    } reg;
} Reg_Attr_Const_Persp;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Reserved                  : 32;  // reserved
    } reg;
} Reg_Tasfe_Reserved;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int X_Min                     : 16;  // X minimum  (Fixed point, 16.0, only 14.0 is used)
        unsigned int X_Max                     : 16;  // X maximum (Fixed point, 16.0, only 14.0 is used)
    } reg;
} Reg_Viewport_X;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Y_Min                     : 16;  // Y minimum  (Fixed point, 16.0, only 14.0 is used)
        unsigned int Y_Max                     : 16;  // Y maximum (Fixed point, 16.0, only 14.0 is used)
    } reg;
} Reg_Viewport_Y;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int X_Max                     : 16;  // X maximum, only 14.0 is used
        unsigned int Y_Max                     : 16;  // Y maximum, only 14.0 is used
    } reg;
} Reg_Rt_Max;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Y0_Offset                 : 5;
        unsigned int Y1_Offset                 : 5;
        unsigned int Y2_Offset                 : 5;
        unsigned int Y3_Offset                 : 5;
        unsigned int Y4_Offset                 : 5;
        unsigned int Y5_Offset                 : 5;
        unsigned int Reserved                  : 2;   // Reserved
    } reg;
} Reg_Ymin_Offset;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Y0_Offset                 : 5;
        unsigned int Y1_Offset                 : 5;
        unsigned int Y2_Offset                 : 5;
        unsigned int Y3_Offset                 : 5;
        unsigned int Y4_Offset                 : 5;
        unsigned int Y5_Offset                 : 5;
        unsigned int Reserved                  : 2;   // Reserved
    } reg;
} Reg_Ymax_Offset;

typedef struct _Tasfe_regs_CHX004
{
    Reg_Tasfe_Ctrl                   reg_Tasfe_Ctrl;
    Reg_Tasfe_Ctrl_Misc              reg_Tasfe_Ctrl_Misc;
    Reg_Pre_Setup_Ctrl               reg_Pre_Setup_Ctrl;
    Reg_Gb_Exp                       reg_Gb_Exp[8];
    Reg_Scissor_X                    reg_Scissor_X[16];
    Reg_Scissor_Y                    reg_Scissor_Y[16];
    Reg_Viewport_Xscale              reg_Viewport_Xscale[16];
    Reg_Viewport_Xoffset             reg_Viewport_Xoffset[16];
    Reg_Viewport_Yscale              reg_Viewport_Yscale[16];
    Reg_Viewport_Yoffset             reg_Viewport_Yoffset[16];
    Reg_Viewport_Zscale              reg_Viewport_Zscale[16];
    Reg_Viewport_Zoffset             reg_Viewport_Zoffset[16];
    Reg_Wscaling_Xcoeff              reg_Wscaling_Xcoeff[16];
    Reg_Wscaling_Ycoeff              reg_Wscaling_Ycoeff[16];
    Reg_Viewport_Swizzle             reg_Viewport_Swizzle[8];
    Reg_Point_Size                   reg_Point_Size;
    Reg_Line_Width                   reg_Line_Width;
    Reg_Z_Bias                       reg_Z_Bias;
    Reg_Z_Scale_Factor               reg_Z_Scale_Factor;
    Reg_Z_Bias_Clamp                 reg_Z_Bias_Clamp;
    Reg_Attr_Const_Persp             reg_Attr_Const_Persp[3];
    Reg_Viewport_X                   reg_Viewport_X[16];
    Reg_Viewport_Y                   reg_Viewport_Y[16];
    Reg_Rt_Max                       reg_Rt_Max;
    Reg_Ymin_Offset                  reg_Ymin_Offset[6];
    Reg_Ymax_Offset                  reg_Ymax_Offset[6];
} Tasfe_regs_CHX004;

typedef struct _Tasfe_regs_Elt3k
{
    Reg_Tasfe_Ctrl                   reg_Tasfe_Ctrl;
    Reg_Tasfe_Ctrl_Misc              reg_Tasfe_Ctrl_Misc;
    Reg_Pre_Setup_Ctrl               reg_Pre_Setup_Ctrl;
    Reg_Gb_Exp                       reg_Gb_Exp[8];
    Reg_Scissor_X                    reg_Scissor_X[16];
    Reg_Scissor_Y                    reg_Scissor_Y[16];
    Reg_Viewport_Xscale              reg_Viewport_Xscale[16];
    Reg_Viewport_Xoffset             reg_Viewport_Xoffset[16];
    Reg_Viewport_Yscale              reg_Viewport_Yscale[16];
    Reg_Viewport_Yoffset             reg_Viewport_Yoffset[16];
    Reg_Viewport_Zscale              reg_Viewport_Zscale[16];
    Reg_Viewport_Zoffset             reg_Viewport_Zoffset[16];
    Reg_Wscaling_Xcoeff              reg_Wscaling_Xcoeff[16];
    Reg_Wscaling_Ycoeff              reg_Wscaling_Ycoeff[16];
    Reg_Viewport_Swizzle             reg_Viewport_Swizzle[8];
    Reg_Point_Size                   reg_Point_Size;
    Reg_Line_Width                   reg_Line_Width;
    Reg_Z_Bias                       reg_Z_Bias;
    Reg_Z_Scale_Factor               reg_Z_Scale_Factor;
    Reg_Z_Bias_Clamp                 reg_Z_Bias_Clamp;
    Reg_Attr_Const_Persp             reg_Attr_Const_Persp[3];
    Reg_Tasfe_Reserved               reg_Tasfe_Reserved[5];
} Tasfe_regs_E3K;

#endif
