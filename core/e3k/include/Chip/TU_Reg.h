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
//    Spec Last Modified Time: 2019/10/9 16:01:06
#ifndef _TUFE_REGISTER_H
#define _TUFE_REGISTER_H


#ifndef    TU_BLOCKBASE_INF
#define    TU_BLOCKBASE_INF
#define    BLOCK_TU_VERSION 1
#define    BLOCK_TU_TIMESTAMP "7/10/2018 2:50:05 PM"
#define    TUFE_BLOCK                                               0xA // match with BlockID.h
#define    TUBE_BLOCK                                               0x14
#define    TU_REG_START                                             0x0 // match with BlockID.h
#define    TU_REG_END_CHX004                                        0x700//0x4B3 // match with BlockID.h
#define    TU_REG_LIMIT_CHX004                                      0x700// match with BlockID.h
#define    TU_REG_END                                               0x580//0x4B3 // match with BlockID.h
#define    TU_REG_LIMIT                                             0x580// match with BlockID.h
#endif

// Register offset definition
#define        Reg_Tu_Tssharp_Ctrl_Offset                                 0x0
#define        Reg_Tu_Tssharp_3d_Ctrl_Offset                              0x1
#define        Reg_Tu_Tssharp_Heap_Base0_Offset                           0x2
#define        Reg_Tu_Tssharp_Heap_Base1_Offset                           0x3
#define        Reg_Tu_Tssharp_Heap_Base2_Offset                           0x4
#define        Reg_Tu_Tssharp_Heap_Base3_Offset                           0x5
#define        Reg_Tu_Tssharp_Heap_Base4_Offset                           0x6
#define        Reg_Tu_Tssharp_Heap_Base5_Offset                           0x7
#define        Reg_Tu_Tssharp_Heap_Base6_Offset                           0x8
#define        Reg_Tu_Tssharp_Heap_Base7_Offset                           0x9
#define        Reg_Tu_Reserved1_Offset_Elt3k                              0xA
#define        Reg_Tu_Tssharp_Heap_Base8_Offset_CHX004                    0xA
#define        Reg_Tu_Tssharp_Heap_Base9_Offset_CHX004                    0xB
#define        Reg_Tu_Tssharp_Heap_Base10_Offset_CHX004                   0xC
#define        Reg_Tu_Tssharp_Heap_Base11_Offset_CHX004                   0xD
#define        Reg_Tu_Tssharp_Heap_Base12_Offset_CHX004                   0xE
#define        Reg_Tu_Tssharp_Heap_Base13_Offset_CHX004                   0xF
#define        Reg_Tu_Control_Fe_Offset                                   0x10
#define        Reg_Tu_Si_Ctl_Offset                                       0x11
#define        Reg_Tu_Vs_Tsharp_Ctrl_Offset                               0x12
#define        Reg_Tu_Vs_Ssharp_Ctrl_Offset                               0x13
#define        Reg_Tu_Hs_Tsharp_Ctrl_Offset                               0x14
#define        Reg_Tu_Hs_Ssharp_Ctrl_Offset                               0x15
#define        Reg_Tu_Ds_Tsharp_Ctrl_Offset                               0x16
#define        Reg_Tu_Ds_Ssharp_Ctrl_Offset                               0x17
#define        Reg_Tu_Gs_Tsharp_Ctrl_Offset                               0x18
#define        Reg_Tu_Gs_Ssharp_Ctrl_Offset                               0x19
#define        Reg_Tu_Reserved2_Offset_Elt3k                              0x1A
#define        Reg_Tu_Tssharp_Heap_Base14_Offset_CHX004                   0x1A
#define        Reg_Tu_Tssharp_Heap_Base15_Offset_CHX004                   0x1B
#define        Reg_Tu_Reserved2_Offset_CHX004                             0x1C
#define        Reg_Tu_Vb_Offset                                           0x20
#define        Reg_Tu_Ve_Offset                                           0xA0
#define        Reg_Tu_Instance_Offset                                     0xC0
#define        Reg_Tu_Control_Be_Offset                                   0xE0
#define        Reg_Tu_Ps_Tsharp_Ctrl_Offset                               0xE1
#define        Reg_Tu_Ps_Ssharp_Ctrl_Offset                               0xE2
#define        Reg_Tu_Reserved3_Offset                                    0xE3
#define        Reg_Tu_Tssharp_Cs_Ctrl_Offset                              0xE8
#define        Reg_Tu_Csl_Tsharp_Ctrl_Offset                              0xE9
#define        Reg_Tu_Csl_Ssharp_Ctrl_Offset                              0xEA
#define        Reg_Tu_Csh_Tsharp_Ctrl_Offset                              0xEB
#define        Reg_Tu_Csh_Ssharp_Ctrl_Offset                              0xEC
#define        Reg_Tu_Reserved4_Offset                                    0xED
#define        Reg_Tu_Ts_Sharp_Offset                                     0x100
//#define        Reg_Tu_T_Sharp_Offset                                      0x100
//#define        Reg_Tu_S_Sharp_Offset                                      0x580

// Block constant definition
typedef enum
{
    TU_CONTROL_FE_SECTOR_MODE_512B           = 0,   // 512b(default)
    TU_CONTROL_FE_SECTOR_MODE_1K             = 1,   // 1k
    TU_CONTROL_FE_SECTOR_MODE_2K             = 2,   // 2k
    TU_CONTROL_FE_SECTOR_MODE_RESERVED       = 3,   // Reserved
} TU_CONTROL_FE_SECTOR_MODE;
typedef enum
{
    TU_CONTROL_FE_API_MODE_FE_DX             = 0,
    TU_CONTROL_FE_API_MODE_FE_DX9            = 1,
    TU_CONTROL_FE_API_MODE_FE_VULKAN         = 2,
    TU_CONTROL_FE_API_MODE_FE_OGL            = 3,
    TU_CONTROL_FE_API_MODE_FE_OES            = 4,
    TU_CONTROL_FE_API_MODE_FE_OCL            = 5,
} TU_CONTROL_FE_API_MODE_FE;
typedef enum
{
    TU_SI_CTL_API_MODE_DX                    = 0,
    TU_SI_CTL_API_MODE_DX9                   = 1,
    TU_SI_CTL_API_MODE_VULKAN                = 2,
    TU_SI_CTL_API_MODE_OGL                   = 3,
    TU_SI_CTL_API_MODE_OES                   = 4,
    TU_SI_CTL_API_MODE_OCL                   = 5,
} TU_SI_CTL_API_MODE;
typedef enum
{
    TU_CONTROL_BE_SECTOR_MODE_512B           = 0,   // 512b(default)
    TU_CONTROL_BE_SECTOR_MODE_1K             = 1,   // 1k
    TU_CONTROL_BE_SECTOR_MODE_2K             = 2,   // 2k
    TU_CONTROL_BE_SECTOR_MODE_RESERVED       = 3,   // Reserved
} TU_CONTROL_BE_SECTOR_MODE;
typedef enum
{
    TU_CONTROL_BE_API_MODE_BE_DX             = 0,
    TU_CONTROL_BE_API_MODE_BE_DX9            = 1,
    TU_CONTROL_BE_API_MODE_BE_VULKAN         = 2,
    TU_CONTROL_BE_API_MODE_BE_OGL            = 3,
    TU_CONTROL_BE_API_MODE_BE_OES            = 4,
    TU_CONTROL_BE_API_MODE_BE_OCL            = 5,
} TU_CONTROL_BE_API_MODE_BE;
typedef enum
{
    TU_T_SHARP_REG2_MSAA_MODE_MSAA_1X        = 0,   // MSAA OFF
    TU_T_SHARP_REG2_MSAA_MODE_MSAA_2X        = 1,   // 2x
    TU_T_SHARP_REG2_MSAA_MODE_MSAA_4X        = 2,   // 4x
    TU_T_SHARP_REG2_MSAA_MODE_MSAA_8X        = 3,   // 8x
    TU_T_SHARP_REG2_MSAA_MODE_MSAA_16X       = 4,   // 16x
} TU_T_SHARP_REG2_MSAA_MODE;
typedef enum
{
    TU_T_SHARP_REG2_RES_TYPE_1D_BUFFER       = 0,   // 0000: 1D Buffer
    TU_T_SHARP_REG2_RES_TYPE_1D_TEXTURE      = 1,   // 0001: Texture1D
    TU_T_SHARP_REG2_RES_TYPE_2D_TEXTURE      = 2,   // 0010: Texture2D
    TU_T_SHARP_REG2_RES_TYPE_3D_TEXTURE      = 3,   // 0011: Texture3D
    TU_T_SHARP_REG2_RES_TYPE_CUBE_TEXTURE    = 4,   // 0100: TextureCube
    TU_T_SHARP_REG2_RES_TYPE_1D_TEXTURE_ARRAY= 5,   // 0101: Texture1DArray
    TU_T_SHARP_REG2_RES_TYPE_2D_TEXTURE_ARRAY= 6,   // 0110: Texture2DArray
    TU_T_SHARP_REG2_RES_TYPE_CUBE_TEXTURE_ARRAY= 7, // 0111: TextureCubeArray
    TU_T_SHARP_REG2_RES_TYPE_RAW_BUFFER      = 8,   // 1000:  raw buffer.
    TU_T_SHARP_REG2_RES_TYPE_STRUCTURED_BUFFER= 9,  // 1001:  structured buffer
    TU_T_SHARP_REG2_RES_TYPE_VERTEX_BUFFER   = 10,  // 1010:  vertex buffer
} TU_T_SHARP_REG2_RES_TYPE;
typedef enum
{
    TU_T_SHARP_REG5_COMPONENT_MAPPING_R_COMPONENT_0= 0,
                                                    // the value is got from r channel
    TU_T_SHARP_REG5_COMPONENT_MAPPING_R_COMPONENT_1= 1,
                                                    // the value is got from g channel
    TU_T_SHARP_REG5_COMPONENT_MAPPING_R_COMPONENT_2= 2,
                                                    // the value is got from b channel
    TU_T_SHARP_REG5_COMPONENT_MAPPING_R_COMPONENT_3= 3,
                                                    // the value is got from a channel
    TU_T_SHARP_REG5_COMPONENT_MAPPING_R_VALUE_0= 4, // the value is 0
    TU_T_SHARP_REG5_COMPONENT_MAPPING_R_VALUE_1= 5, // the value is 1
} TU_T_SHARP_REG5_COMPONENT_MAPPING_R;
typedef enum
{
    TU_T_SHARP_REG5_COMPONENT_MAPPING_G_COMPONENT_0= 0,
                                                    // the value is got from r channel
    TU_T_SHARP_REG5_COMPONENT_MAPPING_G_COMPONENT_1= 1,
                                                    // the value is got from g channel
    TU_T_SHARP_REG5_COMPONENT_MAPPING_G_COMPONENT_2= 2,
                                                    // the value is got from b channel
    TU_T_SHARP_REG5_COMPONENT_MAPPING_G_COMPONENT_3= 3,
                                                    // the value is got from a channel
    TU_T_SHARP_REG5_COMPONENT_MAPPING_G_VALUE_0= 4, // the value is 0
    TU_T_SHARP_REG5_COMPONENT_MAPPING_G_VALUE_1= 5, // the value is 1
} TU_T_SHARP_REG5_COMPONENT_MAPPING_G;
typedef enum
{
    TU_T_SHARP_REG5_COMPONENT_MAPPING_B_COMPONENT_0= 0,
                                                    // the value is got from r channel
    TU_T_SHARP_REG5_COMPONENT_MAPPING_B_COMPONENT_1= 1,
                                                    // the value is got from g channel
    TU_T_SHARP_REG5_COMPONENT_MAPPING_B_COMPONENT_2= 2,
                                                    // the value is got from b channel
    TU_T_SHARP_REG5_COMPONENT_MAPPING_B_COMPONENT_3= 3,
                                                    // the value is got from a channel
    TU_T_SHARP_REG5_COMPONENT_MAPPING_B_VALUE_0= 4, // the value is 0
    TU_T_SHARP_REG5_COMPONENT_MAPPING_B_VALUE_1= 5, // the value is 1
} TU_T_SHARP_REG5_COMPONENT_MAPPING_B;
typedef enum
{
    TU_T_SHARP_REG5_COMPONENT_MAPPING_A_COMPONENT_0= 0,
                                                    // the value is got from r channel
    TU_T_SHARP_REG5_COMPONENT_MAPPING_A_COMPONENT_1= 1,
                                                    // the value is got from g channel
    TU_T_SHARP_REG5_COMPONENT_MAPPING_A_COMPONENT_2= 2,
                                                    // the value is got from b channel
    TU_T_SHARP_REG5_COMPONENT_MAPPING_A_COMPONENT_3= 3,
                                                    // the value is got from a channel
    TU_T_SHARP_REG5_COMPONENT_MAPPING_A_VALUE_0= 4, // the value is 0
    TU_T_SHARP_REG5_COMPONENT_MAPPING_A_VALUE_1= 5, // the value is 1
} TU_T_SHARP_REG5_COMPONENT_MAPPING_A;
typedef enum
{
    TU_S_SHARP_REG0_COMPARE_FUNC_NEVER       = 0,   // 000: NEVER
    TU_S_SHARP_REG0_COMPARE_FUNC_LESS        = 1,   // 001: LESS
    TU_S_SHARP_REG0_COMPARE_FUNC_EQUAL       = 2,   // 010: EQUAL
    TU_S_SHARP_REG0_COMPARE_FUNC_LESSEQUAL   = 3,   // 011: LESSEQUAL
    TU_S_SHARP_REG0_COMPARE_FUNC_GREATER     = 4,   // 100: GREATER
    TU_S_SHARP_REG0_COMPARE_FUNC_NOTEQUAL    = 5,   // 101: NOTEQUAL
    TU_S_SHARP_REG0_COMPARE_FUNC_GREATEREQUAL= 6,   // 110: GREATEREQUAL
    TU_S_SHARP_REG0_COMPARE_FUNC_ALWAYS      = 7,   // 111: ALWAYS
} TU_S_SHARP_REG0_COMPARE_FUNC;
typedef enum
{
    TU_S_SHARP_REG0_LOD_BRILINEAR_THRESHOLD_BRI_0= 0,
                                                    // 00: Brilinear Threshold 0
    TU_S_SHARP_REG0_LOD_BRILINEAR_THRESHOLD_BRI_DOT_0625= 1,
                                                    // 01: Threshold 0.0625
    TU_S_SHARP_REG0_LOD_BRILINEAR_THRESHOLD_BRI_DOT_125= 2,
                                                    // 10:Threshold 0.125
    TU_S_SHARP_REG0_LOD_BRILINEAR_THRESHOLD_BRI_DOT_1875= 3,
                                                    // 11: Threshold 0.1875
} TU_S_SHARP_REG0_LOD_BRILINEAR_THRESHOLD;
typedef enum
{
    TU_S_SHARP_REG0_DEPTH_MODE_NORMAL        = 0,   // 000: Normal
    TU_S_SHARP_REG0_DEPTH_MODE_AMODE         = 1,   // 001: AMODE: tex format set as R_
    TU_S_SHARP_REG0_DEPTH_MODE_IMODE         = 2,   // 010: IMODE: tex format set as R_
    TU_S_SHARP_REG0_DEPTH_MODE_LMODE         = 3,   // 011: LMODE: tex format set as R_
    TU_S_SHARP_REG0_DEPTH_MODE_LAMODE        = 4,   // not used anymore
    TU_S_SHARP_REG0_DEPTH_MODE_RAMODE        = 5,   // 101: RAMODE: tex format set as RG_
} TU_S_SHARP_REG0_DEPTH_MODE;
typedef enum
{
    TU_S_SHARP_REG0_FILTER_REDU_TYPE_STANDARD= 0,   // normal filter mode
    TU_S_SHARP_REG0_FILTER_REDU_TYPE_COMPARISON= 1, // compare mode
    TU_S_SHARP_REG0_FILTER_REDU_TYPE_MIN     = 2,   // min mode, filter output is the per channel min value of the
                                                    // fetched texels
    TU_S_SHARP_REG0_FILTER_REDU_TYPE_MAX     = 3,   // max mode, filter output is the per channel max value of the
                                                    // fetched texels
} TU_S_SHARP_REG0_FILTER_REDU_TYPE;
typedef enum
{
    TU_S_SHARP_REG1_ADDR_U_WRAP              = 0,   // 000: WRAP. Tiles the texture at every integer junction. No
                                                    // mirroring is performed.
    TU_S_SHARP_REG1_ADDR_U_MIRROR            = 1,   // 001: MIRROR. Similar to WRAP, except that the texture is
                                                    // flipped at every integer junction.
    TU_S_SHARP_REG1_ADDR_U_CLAMP             = 2,   // 010: CLAMP. Texture coordinates outside the range [0.0,
                                                    // 1.0] are set to the texture color at 0.0 or 1.0,
                                                    // respectively.
    TU_S_SHARP_REG1_ADDR_U_BORDER            = 3,   // 011: BORDER.
    TU_S_SHARP_REG1_ADDR_U_MIRROR_ONCE       = 4,   // 100: MIRROR ONCE.
    TU_S_SHARP_REG1_ADDR_U_HALF_BORDER       = 5,   // 101: half BORDER.
    TU_S_SHARP_REG1_ADDR_U_CUBE_WRAP         = 6,   // 110: Seamless cube enable
} TU_S_SHARP_REG1_ADDR_U;
typedef enum
{
    TU_S_SHARP_REG1_CHROMA_FILTER_POINT     = 0,
    TU_S_SHARP_REG1_CHROMA_FILTER_LINEAR     = 1,
} TU_S_SHARP_REG1_CHROMA_FILTER;
typedef enum
{
    TU_S_SHARP_REG1_XCHROMA_OFST_COSITED_EVEN= 0,
    TU_S_SHARP_REG1_XCHROMA_OFST_MIDPOINT    = 1,
} TU_S_SHARP_REG1_XCHROMA_OFST;
typedef enum
{
    TU_S_SHARP_REG1_YCHROMA_OFST_COSITED_EVEN= 0,
    TU_S_SHARP_REG1_YCHROMA_OFST_MIDPOINT    = 1,
} TU_S_SHARP_REG1_YCHROMA_OFST;
typedef enum
{
    TU_S_SHARP_REG3_YUV_SELECT_CHANNEL_Y     = 0,   // load the Y channel for NV12 or YV12
    TU_S_SHARP_REG3_YUV_SELECT_CHANNEL_U     = 1,   // load the U channel for YV12
                                                    //  load the UV channels for NV12(UV is mingled in NV12 format)
    TU_S_SHARP_REG3_YUV_SELECT_CHANNEL_V     = 2,   // load the V channel for YV12
    TU_S_SHARP_REG3_YUV_SELECT_CHANNEL_ALL   = 3,   // load YUV channels for NV12 or YV12
} TU_S_SHARP_REG3_YUV_SELECT;
typedef enum
{
    TU_S_SHARP_REG3_MAG_FILTER_POINT         = 0,   // 00: Point sample
    TU_S_SHARP_REG3_MAG_FILTER_LINEAR        = 1,   // 01: Bilinear
    TU_S_SHARP_REG3_MAG_FILTER_ANISOPOINT    = 2,   // 10: Anisotropic point
    TU_S_SHARP_REG3_MAG_FILTER_ANISO         = 3,   // 11: Anisotropic
} TU_S_SHARP_REG3_MAG_FILTER;
typedef enum
{
    TU_S_SHARP_REG3_MIN_FILTER_POINT         = 0,   // 00: Point sample
    TU_S_SHARP_REG3_MIN_FILTER_LINEAR        = 1,   // 01: Bilinear
    TU_S_SHARP_REG3_MIN_FILTER_ANISOPOINT    = 2,   // 10: Anisotropic point
    TU_S_SHARP_REG3_MIN_FILTER_ANISO         = 3,   // 11: Anisotropic
} TU_S_SHARP_REG3_MIN_FILTER;
typedef enum
{
    TU_S_SHARP_REG3_MIP_FILTER_NONE          = 0,   // 00: No Mip Map Filter
    TU_S_SHARP_REG3_MIP_FILTER_POINT         = 1,   // 01: Mip Map Filter is point sampled
    TU_S_SHARP_REG3_MIP_FILTER_LINEAR        = 2,   // 10: Mip Map Filter is two-level linearly sampled
    TU_S_SHARP_REG3_MIP_FILTER_RESERVED      = 3,   // 11: Reserved
} TU_S_SHARP_REG3_MIP_FILTER;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////            TUFE Block (Block ID = 10) Register Definitions                           ///////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Base_Address_3d           : 8;   // total ts# space is 192*192bit, the 3D ts# is behind CSH, the
                                                      // range is 0-191.
        unsigned int Reserved                  : 24;
    } reg;
} Reg_Tu_Tssharp_Ctrl;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Base_Address_Be           : 8;   // total ts# space is 192*192bit, the 3D ts# is behind CSH,
                                                      // this base is 3D_BE range, used to divide 3D FE and BE.
        unsigned int Is_3d_Cachemode           : 1;   // 1 means 3d ts# is cachemode, 0 means buffer mode
        unsigned int Reserved                  : 23;
    } reg;
} Reg_Tu_Tssharp_3d_Ctrl;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Base_Address              : 32;
    } reg;
} Reg_Tu_Tssharp_Heap_Base0;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Base_Address              : 32;
    } reg;
} Reg_Tu_Tssharp_Heap_Base1;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Base_Address              : 32;
    } reg;
} Reg_Tu_Tssharp_Heap_Base2;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Base_Address              : 32;
    } reg;
} Reg_Tu_Tssharp_Heap_Base3;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Base_Address              : 32;
    } reg;
} Reg_Tu_Tssharp_Heap_Base4;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Base_Address              : 32;
    } reg;
} Reg_Tu_Tssharp_Heap_Base5;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Base_Address              : 32;
    } reg;
} Reg_Tu_Tssharp_Heap_Base6;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Base_Address              : 32;
    } reg;
} Reg_Tu_Tssharp_Heap_Base7;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Reserved                  : 32;  // reserved
    } reg;
} Reg_Tu_Reserved1_Elt3k;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Base_Address              : 32;
    } reg;
} Reg_Tu_Tssharp_Heap_Base8;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Base_Address              : 32;
    } reg;
} Reg_Tu_Tssharp_Heap_Base9;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Base_Address              : 32;
    } reg;
} Reg_Tu_Tssharp_Heap_Base10;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Base_Address              : 32;
    } reg;
} Reg_Tu_Tssharp_Heap_Base11;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Base_Address              : 32;
    } reg;
} Reg_Tu_Tssharp_Heap_Base12;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Base_Address              : 32;
    } reg;
} Reg_Tu_Tssharp_Heap_Base13;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Sector_Mode               : 2;   // sector size
        unsigned int Vb_Sector_Mode            : 2;   // VB sector size
        unsigned int Reserved_0                : 1;   // Not used.
        unsigned int Enable_Vb_Bank_Swizzle    : 1;   // vb bank swizzle to reduce bank conflict for loading vb.
        unsigned int Enable_3dtex_Qswizzle     : 1;   // set and bank swizzle for 3d texture to reduce set/bank
                                                      // conflict
        unsigned int Api_Mode_Fe               : 3;   // api_mode
        unsigned int Reserved_1                : 22;  // Reserved
    } reg;
} Reg_Tu_Control_Fe;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Element_Num               : 7;   // element number does not include the vid
        unsigned int Vid_En                    : 1;   // vid is always inserted in the 1st place, R0
        unsigned int Iid_En                    : 1;   // iid is always inserted in the 2nd place, R1
        unsigned int Api_Mode                  : 3;
        unsigned int Did_En                    : 1;   // draw_id enable, it is always inserted in the 3rd place, R2
        unsigned int Base_Vertex_En            : 1;   // gl_base_vertex enable, it is always inserted in the 4th
                                                      // place, R3
        unsigned int Base_Instance_En          : 1;   // gl_instance_enable, it is always inserted in the 5th
                                                      // place, R4
        unsigned int Reserved                  : 17;
    } reg;
} Reg_Tu_Si_Ctl;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Base_Address              : 8;   // 6dw aligned base address, total has 192*6dw, so it is 8bits
        unsigned int Count                     : 8;   // total t# count, each t# has 6dw
        unsigned int Reserved                  : 16;
    } reg;
} Reg_Tu_Vs_Tsharp_Ctrl;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Base_Address              : 8;   // 6dw aligned base address, total has 192*6dw, so it is 8bits
        unsigned int Count                     : 8;   // total s# count, each s# has 6dw
        unsigned int Reserved                  : 16;
    } reg;
} Reg_Tu_Vs_Ssharp_Ctrl;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Base_Address              : 8;   // 6dw aligned base address, total has 192*6dw, so it is 8bits
        unsigned int Count                     : 8;   // total t# count, each t# has 6dw
        unsigned int Reserved                  : 16;
    } reg;
} Reg_Tu_Hs_Tsharp_Ctrl;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Base_Address              : 8;   // 6dw aligned base address, total has 192*6dw, so it is 8bits
        unsigned int Count                     : 8;   // total s# count, each s# has 6dw
        unsigned int Reserved                  : 16;
    } reg;
} Reg_Tu_Hs_Ssharp_Ctrl;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Base_Address              : 8;   // 6dw aligned base address, total has 192*6dw, so it is 8bits
        unsigned int Count                     : 8;   // total t# count, each t# has 6dw
        unsigned int Reserved                  : 16;
    } reg;
} Reg_Tu_Ds_Tsharp_Ctrl;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Base_Address              : 8;   // 6dw aligned base address, total has 192*6dw, so it is 8bits
        unsigned int Count                     : 8;   // total s# count, each s# has 6dw
        unsigned int Reserved                  : 16;
    } reg;
} Reg_Tu_Ds_Ssharp_Ctrl;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Base_Address              : 8;   // 6dw aligned base address, total has 192*6dw, so it is 8bits
        unsigned int Count                     : 8;   // total t# count, each t# has 6dw
        unsigned int Reserved                  : 16;
    } reg;
} Reg_Tu_Gs_Tsharp_Ctrl;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Base_Address              : 8;   // 6dw aligned base address, total has 192*6dw, so it is 8bits
        unsigned int Count                     : 8;   // total s# count, each s# has 6dw
        unsigned int Reserved                  : 16;
    } reg;
} Reg_Tu_Gs_Ssharp_Ctrl;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Base_Address              : 32;
    } reg;
} Reg_Tu_Tssharp_Heap_Base14;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Base_Address              : 32;
    } reg;
} Reg_Tu_Tssharp_Heap_Base15;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Reserved                  : 32;  // reserved
    } reg;
} Reg_Tu_Reserved2;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Resource_Address          : 32;  // VA is 40bit, so 2kbit base is 32bit
    } reg;
} Reg_Tu_Vb0;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Vb_Size_Inbyte            : 28;  // max vertex buffer size, vulkan requires 2^27 bytes
        unsigned int Reserved                  : 4;
    } reg;
} Reg_Tu_Vb1;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Vb_Start_Offset           : 32;
    } reg;
} Reg_Tu_Vb2;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Vb_Stride_Inbyte          : 12;  // vb stride is 12bits in total
    } reg;
} Reg_Tu_Vb3;

typedef struct _Group_Tu_Vb
{
    Reg_Tu_Vb0                       reg_Tu_Vb0;
    Reg_Tu_Vb1                       reg_Tu_Vb1;
    Reg_Tu_Vb2                       reg_Tu_Vb2;
    Reg_Tu_Vb3                       reg_Tu_Vb3;
} Reg_Tu_Vb_Group;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Res_Format                : 9;   // Texture Format.
        unsigned int Valid                     : 1;   // register is valid.
        unsigned int Element_Offset            : 11;  // the start offset of this element. Vulkan requires 2047 for
                                                      // it.
        unsigned int Vb_Id                     : 5;
        unsigned int Instance_En               : 1;
        unsigned int Reserved                  : 5;
    } reg;
} Reg_Tu_Ve;

typedef struct _Group_Tu_Ve
{
    Reg_Tu_Ve                        reg_Tu_Ve;
} Reg_Tu_Ve_Group;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Step_Rate                 : 26;  // StepRate is how many instances to draw before stepping one
                                                      // unit forward in a vertex buffer containing instance Data.
        unsigned int Reserved                  : 6;
    } reg;
} Reg_Tu_Instance_Ctl;

typedef struct _Group_Tu_Instance
{
    Reg_Tu_Instance_Ctl              reg_Tu_Instance_Ctl;
} Reg_Tu_Instance_Group;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Sector_Mode               : 2;   // sector size
        unsigned int Reserved_0                : 4;   // Not used.
        unsigned int Enable_3dtex_Qswizzle     : 1;   // set and bank swizzle for 3d texture to reduce set/bank
                                                      // conflict
        unsigned int Api_Mode_Be               : 3;   // api_mode
        unsigned int Reserved_1                : 22;  // Reserved
    } reg;
} Reg_Tu_Control_Be;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Base_Address              : 8;   // 6dw aligned base address, total has 192*6dw, so it is 8bits
        unsigned int Count                     : 8;   // total t# count, each t# has 6dw
        unsigned int Reserved                  : 16;
    } reg;
} Reg_Tu_Ps_Tsharp_Ctrl;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Base_Address              : 8;   // 6dw aligned base address, total has 192*6dw, so it is 8bits
        unsigned int Count                     : 8;   // total s# count, each s# has 6dw
        unsigned int Reserved                  : 16;
    } reg;
} Reg_Tu_Ps_Ssharp_Ctrl;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Reserved                  : 32;  // reserved
    } reg;
} Reg_Tu_Reserved3;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Base_Address_Csh          : 8;   // total ts# space is 192*192bit, the CS ts# is at front, this
                                                      // base is CSH range, used to divide CS LOW and HIGH.
        unsigned int Is_Csl_Cachemode          : 1;   // 1 means csl cachemode, 0 means buffer mode
        unsigned int Is_Csh_Cachemode          : 1;   // 1 means csh cachemode, 0 means buffer mode
        unsigned int Reserved                  : 22;
    } reg;
} Reg_Tu_Tssharp_Cs_Ctrl;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Base_Address              : 8;   // 6dw aligned base address, total has 192*6dw, so it is 8bits
        unsigned int Count                     : 8;   // total t# count, each t# has 6dw
        unsigned int Reserved                  : 16;
    } reg;
} Reg_Tu_Csl_Tsharp_Ctrl;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Base_Address              : 8;   // 6dw aligned base address, total has 192*6dw, so it is 8bits
        unsigned int Count                     : 8;   // total s# count, each s# has 6dw
        unsigned int Reserved                  : 16;
    } reg;
} Reg_Tu_Csl_Ssharp_Ctrl;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Base_Address              : 8;   // 6dw aligned base address, total has 192*6dw, so it is 8bits
        unsigned int Count                     : 8;   // total t# count, each t# has 6dw
        unsigned int Reserved                  : 16;
    } reg;
} Reg_Tu_Csh_Tsharp_Ctrl;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Base_Address              : 8;   // 6dw aligned base address, total has 192*6dw, so it is 8bits
        unsigned int Count                     : 8;   // total s# count, each s# has 6dw
        unsigned int Reserved                  : 16;
    } reg;
} Reg_Tu_Csh_Ssharp_Ctrl;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Reserved                  : 32;  // reserved
    } reg;
} Reg_Tu_Reserved4_Ctl;

typedef struct _Group_Tu_Reserved4
{
    Reg_Tu_Reserved4_Ctl             reg_Tu_Reserved4_Ctl;
} Reg_Tu_Reserved4_Group;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Res_Address               : 32;  // 2kbits aligned base address
    } reg;
} Reg_Tu_T_Sharp_Reg0;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Res_Width                 : 15;  // max width is16384
        unsigned int Res_Height                : 15;  // max height is 16384
        unsigned int Is_Compression            : 1;   // tc_mdec, whether is compression
        unsigned int Is_Data_Matrix            : 1;   // tc_mdec, matrix or weight
    } reg;
} Reg_Tu_T_Sharp_Reg1;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Res_Depth                 : 12;  // max 3D depth = 2k, max array size = 2k
                                                      //  3D texture and array resource need to fill this field.
        unsigned int Res_Format                : 9;
        unsigned int Msaa_Mode                 : 3;
        unsigned int Res_Type                  : 4;   // Texture Construction Type
        unsigned int Reserved                  : 4;
    } reg;
} Reg_Tu_T_Sharp_Reg2;

typedef union
{
    unsigned int uint;     // for raw/structured/normal buffer, it is reused as 32bit buffer start offset, aligned to bpt
    struct
    {
        unsigned int Is_Mipmap                 : 1;   // MipMap More than One
        unsigned int Is_Linear_Texture         : 1;   // Linear Texture Flag.
        unsigned int View_Lod_Start            : 4;   // include
        unsigned int View_Lod_End              : 4;   // include
        unsigned int View_Array_Slice_Start    : 11;  // include
        unsigned int View_Array_Slice_End      : 11;  // include
    } reg;
} Reg_Tu_T_Sharp_Reg3;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Bl_Start                  : 18;  // bl slot index, start addr to store compression burst
                                                      // length
        unsigned int Lod_Clamp                 : 12;  // per resource lod clamp value as 4.8 format
                                                      //  if resource type == structure buffer, then this field is
                                                      // reused as RES_STRIDE, which means sizeof(structure) in
                                                      // structure buffer
        unsigned int Reserved                  : 1;   // control whether TFU will do degamma for srgb texture, oes
                                                      // has extension to control this, move degamma enable to S#
        unsigned int Is_Bdr_Added              : 1;   // border added flag
    } reg;
} Reg_Tu_T_Sharp_Reg4;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Component_Mapping_R       : 3;   // R channel swizzle for output
        unsigned int Component_Mapping_G       : 3;   // G channel swizzle for output
        unsigned int Component_Mapping_B       : 3;   // B channel swizzle for output
        unsigned int Component_Mapping_A       : 3;   // A channel swizzle for output
        unsigned int Range_Type                : 6;   // compression format
        unsigned int Is_Sparse                 : 1;   // is sparse texture or not, 0: normal, 1: sparse
        unsigned int Valid                     : 1;   // register is valid. Null texture will use this feature
        unsigned int Reserved                  : 12;
    } reg;
} Reg_Tu_T_Sharp_Reg5;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Reserved : 32;  // slot align to 8dw
    } reg;
} Reg_Tu_T_Sharp_Reg6;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Reserved : 32;  // slot align to 8dw
    } reg;
} Reg_Tu_T_Sharp_Reg7;

typedef struct _Group_Tu_T_Sharp
{
    Reg_Tu_T_Sharp_Reg0              reg_Tu_T_Sharp_Reg0;
    Reg_Tu_T_Sharp_Reg1              reg_Tu_T_Sharp_Reg1;
    Reg_Tu_T_Sharp_Reg2              reg_Tu_T_Sharp_Reg2;
    Reg_Tu_T_Sharp_Reg3              reg_Tu_T_Sharp_Reg3;
    Reg_Tu_T_Sharp_Reg4              reg_Tu_T_Sharp_Reg4;
    Reg_Tu_T_Sharp_Reg5              reg_Tu_T_Sharp_Reg5;
    Reg_Tu_T_Sharp_Reg6              reg_Tu_T_Sharp_Reg6;
    Reg_Tu_T_Sharp_Reg7              reg_Tu_T_Sharp_Reg7;
} Reg_Tu_T_Sharp_Group;

typedef struct _Group_Tu_T_Sharp_E3K
{
    Reg_Tu_T_Sharp_Reg0              reg_Tu_T_Sharp_Reg0;
    Reg_Tu_T_Sharp_Reg1              reg_Tu_T_Sharp_Reg1;
    Reg_Tu_T_Sharp_Reg2              reg_Tu_T_Sharp_Reg2;
    Reg_Tu_T_Sharp_Reg3              reg_Tu_T_Sharp_Reg3;
    Reg_Tu_T_Sharp_Reg4              reg_Tu_T_Sharp_Reg4;
    Reg_Tu_T_Sharp_Reg5              reg_Tu_T_Sharp_Reg5;
} Reg_Tu_T_Sharp_Group_E3K;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Sample_C_Fail_Value       : 8;   // Used for OGL shadow map. In the format as UNORM8 in range of
                                                      // [0, 1].
        unsigned int Compare_Func              : 3;   // Comparision Function to use if any comparison flag is
                                                      // specified in filter. Compare function for shadow map.
        unsigned int Reserved_0                : 3;   // reserved, used to be API MODE in E2K5;
        unsigned int Lod_Brilinear_Threshold   : 2;   // to reduce the workload from trilinear filter
        unsigned int Uv_Brilinear_Threshold    : 2;   // The same threshold enumeration as LOD brilinear
        unsigned int Q_Brilinear_Threshold     : 2;   // The same threshold enumeration as LOD brilinear
        unsigned int Depth_Mode                : 3;   // Depth Mode. Used for OGL depth texture.
        unsigned int Max_Aniso_Ratio           : 4;   // The actual max aniso ratio = this field + 1
        unsigned int Is_Unnormalized_Coord     : 1;   // coordinates is unnormalized or not.
        unsigned int Filter_Redu_Type          : 2;   // filter mode reduction type
        unsigned int Degamma_En                : 1;
        unsigned int Reserved_1                : 1;
    } reg;
} Reg_Tu_S_Sharp_Reg0;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Mip_Bias : 16;  // normal case is MIP BIAS: s4.8,  13bit;
                                     // case is buffer start , 16 bit
        unsigned int Addr_U : 3;   // Sampler Texture Address Mode AddressU
        unsigned int Addr_V : 3;   // Sampler Texture Address Mode AddressV
        unsigned int Addr_R : 3;   // Sampler Texture Address Mode AddressR
        unsigned int Is_Non_Seamless_Cube : 1;   // Is non-seamless cube or seamless cube
        unsigned int Need_Chroma : 1;   // need_chroma_recstr=1, tu will do ycrcb convertion
        unsigned int Chroma_Filter : 1;   // VK_FILTER_NEAREST, VK_FILTER_LINEAR
        unsigned int Xchroma_Ofst : 1;
        unsigned int Ychroma_Ofst : 1;
        unsigned int Reserved : 2;   // originally is bypass tag, should be no use now, right?
    } reg_CHX004;
    struct
    {
        unsigned int Mip_Bias                  : 16;  // normal case is MIP BIAS: s4.8,  13bit;
                                                      // case is buffer start , 16 bit
        unsigned int Addr_U                    : 3;   // Sampler Texture Address Mode AddressU
        unsigned int Addr_V                    : 3;   // Sampler Texture Address Mode AddressV
        unsigned int Addr_R                    : 3;   // Sampler Texture Address Mode AddressR
        unsigned int Is_Non_Seamless_Cube      : 1;   // Is non-seamless cube or seamless cube
        unsigned int Reserved                  : 6;   // originally is bypass tag, should be no use now, right?
    } reg;
} Reg_Tu_S_Sharp_Reg1;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Bc_Address                : 32;  // border color surface base address, aligned to 2kbits
    } reg;
} Reg_Tu_S_Sharp_Reg2;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Min_Lod                   : 12;  // 4.8 format
        unsigned int Max_Lod                   : 12;  // 4.8 format
        unsigned int Yuv_Select                : 2;   // select the channel to load for YUV planar formats(NV12,
                                                      // YV12)
        unsigned int Mag_Filter                : 2;   // Texture Magnification Filter
        unsigned int Min_Filter                : 2;   // Texture Minification Filter
        unsigned int Mip_Filter                : 2;   // Sampler Mip Map Filter
    } reg;
} Reg_Tu_S_Sharp_Reg3;

typedef struct _Group_Tu_S_Sharp
{
    Reg_Tu_S_Sharp_Reg0              reg_Tu_S_Sharp_Reg0;
    Reg_Tu_S_Sharp_Reg1              reg_Tu_S_Sharp_Reg1;
    Reg_Tu_S_Sharp_Reg2              reg_Tu_S_Sharp_Reg2;
    Reg_Tu_S_Sharp_Reg3              reg_Tu_S_Sharp_Reg3;
} Reg_Tu_S_Sharp_Group;

typedef union
{
    Reg_Tu_T_Sharp_Group             reg_tsharp;
    Reg_Tu_S_Sharp_Group             reg_ssharp;
} Reg_Tu_Ts_Sharp_Group;

typedef union
{
    Reg_Tu_T_Sharp_Group_E3K             reg_tsharp;
    Reg_Tu_S_Sharp_Group                 reg_ssharp;
} Reg_Tu_Ts_Sharp_Group_E3K;

typedef struct _Tu_regs
{
    Reg_Tu_Tssharp_Ctrl              reg_Tu_Tssharp_Ctrl;
    Reg_Tu_Tssharp_3d_Ctrl           reg_Tu_Tssharp_3d_Ctrl;
    Reg_Tu_Tssharp_Heap_Base0        reg_Tu_Tssharp_Heap_Base0;
    Reg_Tu_Tssharp_Heap_Base1        reg_Tu_Tssharp_Heap_Base1;
    Reg_Tu_Tssharp_Heap_Base2        reg_Tu_Tssharp_Heap_Base2;
    Reg_Tu_Tssharp_Heap_Base3        reg_Tu_Tssharp_Heap_Base3;
    Reg_Tu_Tssharp_Heap_Base4        reg_Tu_Tssharp_Heap_Base4;
    Reg_Tu_Tssharp_Heap_Base5        reg_Tu_Tssharp_Heap_Base5;
    Reg_Tu_Tssharp_Heap_Base6        reg_Tu_Tssharp_Heap_Base6;
    Reg_Tu_Tssharp_Heap_Base7        reg_Tu_Tssharp_Heap_Base7;
    Reg_Tu_Tssharp_Heap_Base8        reg_Tu_Tssharp_Heap_Base8;
    Reg_Tu_Tssharp_Heap_Base9        reg_Tu_Tssharp_Heap_Base9;
    Reg_Tu_Tssharp_Heap_Base10       reg_Tu_Tssharp_Heap_Base10;
    Reg_Tu_Tssharp_Heap_Base11       reg_Tu_Tssharp_Heap_Base11;
    Reg_Tu_Tssharp_Heap_Base12       reg_Tu_Tssharp_Heap_Base12;
    Reg_Tu_Tssharp_Heap_Base13       reg_Tu_Tssharp_Heap_Base13;
    Reg_Tu_Control_Fe                reg_Tu_Control_Fe;
    Reg_Tu_Si_Ctl                    reg_Tu_Si_Ctl;
    Reg_Tu_Vs_Tsharp_Ctrl            reg_Tu_Vs_Tsharp_Ctrl;
    Reg_Tu_Vs_Ssharp_Ctrl            reg_Tu_Vs_Ssharp_Ctrl;
    Reg_Tu_Hs_Tsharp_Ctrl            reg_Tu_Hs_Tsharp_Ctrl;
    Reg_Tu_Hs_Ssharp_Ctrl            reg_Tu_Hs_Ssharp_Ctrl;
    Reg_Tu_Ds_Tsharp_Ctrl            reg_Tu_Ds_Tsharp_Ctrl;
    Reg_Tu_Ds_Ssharp_Ctrl            reg_Tu_Ds_Ssharp_Ctrl;
    Reg_Tu_Gs_Tsharp_Ctrl            reg_Tu_Gs_Tsharp_Ctrl;
    Reg_Tu_Gs_Ssharp_Ctrl            reg_Tu_Gs_Ssharp_Ctrl;
    Reg_Tu_Tssharp_Heap_Base14       reg_Tu_Tssharp_Heap_Base14;
    Reg_Tu_Tssharp_Heap_Base15       reg_Tu_Tssharp_Heap_Base15;
    Reg_Tu_Reserved2                 reg_Tu_Reserved2[4];
    Reg_Tu_Vb_Group                  reg_Tu_Vb[32];
    Reg_Tu_Ve_Group                  reg_Tu_Ve[32];
    Reg_Tu_Instance_Group            reg_Tu_Instance[32];
    Reg_Tu_Control_Be                reg_Tu_Control_Be;
    Reg_Tu_Ps_Tsharp_Ctrl            reg_Tu_Ps_Tsharp_Ctrl;
    Reg_Tu_Ps_Ssharp_Ctrl            reg_Tu_Ps_Ssharp_Ctrl;
    Reg_Tu_Reserved3                 reg_Tu_Reserved3[5];
    Reg_Tu_Tssharp_Cs_Ctrl           reg_Tu_Tssharp_Cs_Ctrl;
    Reg_Tu_Csl_Tsharp_Ctrl           reg_Tu_Csl_Tsharp_Ctrl;
    Reg_Tu_Csl_Ssharp_Ctrl           reg_Tu_Csl_Ssharp_Ctrl;
    Reg_Tu_Csh_Tsharp_Ctrl           reg_Tu_Csh_Tsharp_Ctrl;
    Reg_Tu_Csh_Ssharp_Ctrl           reg_Tu_Csh_Ssharp_Ctrl;
    Reg_Tu_Reserved4_Group           reg_Tu_Reserved4[19];
    Reg_Tu_Ts_Sharp_Group            reg_Tu_Ts_Sharp[192];
} Tu_regs;

typedef struct _Tu_regs_E3k
{
    Reg_Tu_Tssharp_Ctrl              reg_Tu_Tssharp_Ctrl;
    Reg_Tu_Tssharp_3d_Ctrl           reg_Tu_Tssharp_3d_Ctrl;
    Reg_Tu_Tssharp_Heap_Base0        reg_Tu_Tssharp_Heap_Base0;
    Reg_Tu_Tssharp_Heap_Base1        reg_Tu_Tssharp_Heap_Base1;
    Reg_Tu_Tssharp_Heap_Base2        reg_Tu_Tssharp_Heap_Base2;
    Reg_Tu_Tssharp_Heap_Base3        reg_Tu_Tssharp_Heap_Base3;
    Reg_Tu_Tssharp_Heap_Base4        reg_Tu_Tssharp_Heap_Base4;
    Reg_Tu_Tssharp_Heap_Base5        reg_Tu_Tssharp_Heap_Base5;
    Reg_Tu_Tssharp_Heap_Base6        reg_Tu_Tssharp_Heap_Base6;
    Reg_Tu_Tssharp_Heap_Base7        reg_Tu_Tssharp_Heap_Base7;
    Reg_Tu_Reserved1_Elt3k           reg_Tu_Reserved1[6];
    Reg_Tu_Control_Fe                reg_Tu_Control_Fe;
    Reg_Tu_Si_Ctl                    reg_Tu_Si_Ctl;
    Reg_Tu_Vs_Tsharp_Ctrl            reg_Tu_Vs_Tsharp_Ctrl;
    Reg_Tu_Vs_Ssharp_Ctrl            reg_Tu_Vs_Ssharp_Ctrl;
    Reg_Tu_Hs_Tsharp_Ctrl            reg_Tu_Hs_Tsharp_Ctrl;
    Reg_Tu_Hs_Ssharp_Ctrl            reg_Tu_Hs_Ssharp_Ctrl;
    Reg_Tu_Ds_Tsharp_Ctrl            reg_Tu_Ds_Tsharp_Ctrl;
    Reg_Tu_Ds_Ssharp_Ctrl            reg_Tu_Ds_Ssharp_Ctrl;
    Reg_Tu_Gs_Tsharp_Ctrl            reg_Tu_Gs_Tsharp_Ctrl;
    Reg_Tu_Gs_Ssharp_Ctrl            reg_Tu_Gs_Ssharp_Ctrl;
    Reg_Tu_Reserved2                 reg_Tu_Reserved2[6];
    Reg_Tu_Vb_Group                  reg_Tu_Vb[32];
    Reg_Tu_Ve_Group                  reg_Tu_Ve[32];
    Reg_Tu_Instance_Group            reg_Tu_Instance[32];
    Reg_Tu_Control_Be                reg_Tu_Control_Be;
    Reg_Tu_Ps_Tsharp_Ctrl            reg_Tu_Ps_Tsharp_Ctrl;
    Reg_Tu_Ps_Ssharp_Ctrl            reg_Tu_Ps_Ssharp_Ctrl;
    Reg_Tu_Reserved3                 reg_Tu_Reserved3[5];
    Reg_Tu_Tssharp_Cs_Ctrl           reg_Tu_Tssharp_Cs_Ctrl;
    Reg_Tu_Csl_Tsharp_Ctrl           reg_Tu_Csl_Tsharp_Ctrl;
    Reg_Tu_Csl_Ssharp_Ctrl           reg_Tu_Csl_Ssharp_Ctrl;
    Reg_Tu_Csh_Tsharp_Ctrl           reg_Tu_Csh_Tsharp_Ctrl;
    Reg_Tu_Csh_Ssharp_Ctrl           reg_Tu_Csh_Ssharp_Ctrl;
    Reg_Tu_Reserved4_Group           reg_Tu_Reserved4[19];
    Reg_Tu_Ts_Sharp_Group            reg_Tu_Ts_Sharp[192];
} Tu_regs_E3k;

#endif
