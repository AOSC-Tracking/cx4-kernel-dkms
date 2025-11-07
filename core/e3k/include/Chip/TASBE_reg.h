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
//    Spec Last Modified Time: 10/13/2017 10:19:33 AM
#ifndef _TASBE_REG_H
#define _TASBE_REG_H


#ifndef        TASBE_BLOCKBASE_INF
#define    TASBE_BLOCKBASE_INF
#define    BLOCK_TASBE_VERSION 1
#define    BLOCK_TASBE_TIMESTAMP "10/13/2017 10:19:33 AM"
#define    TASBE_BLOCK                                                0x5 // match with BlockID.h
#define    TASBE_REG_START                                            0x0 // match with BlockID.h
#define    TASBE_REG_END                                              0x8 // match with BlockID.h
#define    TASBE_REG_LIMIT                                            0x8 // match with BlockID.h
#endif

// Register offset definition
//#define        Reg_Tasbe_Ctrl_Offset                                      0x0
#define        Reg_Tasbe_Reserved_Offset                                  0x1

// Block constant definition
typedef enum
{
    TASBE_CTRL_TBR_EN_DISABLE                = 0,   // 0: TBR disabled
    TASBE_CTRL_TBR_EN_ENABLE                 = 1,   // 1: TBR enabled
} TASBE_CTRL_TBR_EN;
typedef enum
{
    TASBE_CTRL_TBR_MODE_PER_DRAW             = 0,   // 0: per-draw mode
    TASBE_CTRL_TBR_MODE_PER_FRAME            = 1,   // 1: per-frame mode
} TASBE_CTRL_TBR_MODE;
typedef enum
{
    TASBE_CTRL_TILE_SIZE_256X128             = 0,   // 32bpp, 1RT, 4slice, MSAA1x
    TASBE_CTRL_TILE_SIZE_128X128             = 1,
    TASBE_CTRL_TILE_SIZE_128X64              = 2,
    TASBE_CTRL_TILE_SIZE_64X64               = 3,
    TASBE_CTRL_TILE_SIZE_64X32               = 4,
    TASBE_CTRL_TILE_SIZE_32X32               = 5,
    TASBE_CTRL_TILE_SIZE_32X16               = 6,
    TASBE_CTRL_TILE_SIZE_16X16               = 7,
    TASBE_CTRL_TILE_SIZE_16X8                = 8,
    TASBE_CTRL_TILE_SIZE_8X8                 = 9,   // 128bpp, 8rt, 4slice, MSAA16x
} TASBE_CTRL_TILE_SIZE;
//typedef enum
//{
//    TASBE_CTRL_MSAA_STATE_MSAA_OFF           = 0,   // 0: msaa off
//    TASBE_CTRL_MSAA_STATE_MSAA_1X            = 1,   // 1: 1 samples per pixel
//    TASBE_CTRL_MSAA_STATE_MSAA_2X            = 2,   // 2: 2 samples per pixel
//    TASBE_CTRL_MSAA_STATE_MSAA_4X            = 3,   // 3: 4 samples per pixel
//    TASBE_CTRL_MSAA_STATE_MSAA_8X            = 4,   // 4: 8 samples per pixel
//    TASBE_CTRL_MSAA_STATE_MSAA_16X           = 5,   // 5: 16 samples per pixel
//} TASBE_CTRL_MSAA_STATE;
//typedef enum
//{
//    TASBE_CTRL_AA_EN_FALSE                   = 0,   // 0: False
//    TASBE_CTRL_AA_EN_TRUE                    = 1,   // 1: True
//} TASBE_CTRL_AA_EN;
//typedef enum
//{
//    TASBE_CTRL_RASTER_MODE_NORMAL            = 0,   // 0:normal
//    TASBE_CTRL_RASTER_MODE_CONSERVATIVE      = 1,   // 1:conservative
//} TASBE_CTRL_RASTER_MODE;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////            TASBE Block (Block ID = 5) Register Definitions                           ///////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Tbr_En                    : 1;   // TBR enabled
        unsigned int Tbr_Mode                  : 1;   // TBR mode
        unsigned int Tile_Size                 : 4;   // tile size such that 1 cache line holds data of one tile
        unsigned int Attr_Num                  : 6;   // attribute number
        unsigned int Msaa_State                : 3;   // used to adjust bounding box, if msaa enabled, don't care
        // whether it's normal msaa or user-defined mass pattern
        unsigned int Aa_En                     : 1;   // AA mode enable
        unsigned int Raster_Mode               : 1;   // Conervative raster enable or not
        unsigned int Reserved                  : 15;  // Reserved
    } reg;
} reg_Tasbe_Ctrl;


typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Reserved                  : 32;  // reserved
    } reg;
} Reg_Tasbe_Reserved;

typedef struct _Tasbe_regs
{
    reg_Tasbe_Ctrl                   reg_Tasbe_Ctrl;
    Reg_Tasbe_Reserved               reg_Tasbe_Reserved[7];
} Tasbe_regs;

#endif
