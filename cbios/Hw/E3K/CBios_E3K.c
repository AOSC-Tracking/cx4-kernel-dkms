/*
* Copyright 2019-2020 Shanghai Zhaoxin Semiconductor Co., Ltd. All Rights Reserved.
*
* Permission is hereby granted, free of charge, to any person obtaining a
* copy of this software and associated documentation files (the "Software"),
* to deal in the Software without restriction, including without limitation
* the rights to use, copy, modify, merge, publish, distribute, sub license,
* and/or sell copies of the Software, and to permit persons to whom the
* Software is furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice (including the
* next paragraph) shall be included in all copies or substantial portions
* of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT. IN NO EVENT SHALL
* THE AUTHOR(S) OR COPYRIGHT HOLDER(S) BE LIABLE FOR ANY CLAIM, DAMAGES OR
* OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
* ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
* DEALINGS IN THE SOFTWARE.
*/

/*****************************************************************************
** DESCRIPTION:
** chip functions implementation.
**
** NOTE:
** The chip functions SHOULD NOT be called by other modules directly.
** Please call the corresponding function defined in CBiosChipFunc.h.
******************************************************************************/

#include "CBios_E3K.h"
#include "../Register/BIU_SBI_registers.h"
#include "../Register/pmu_registers.h"
#include "../HwBlock/CBiosDIU_HDTV.h"

static    CBIOS_U32  PSStartAddrIndex[CBIOS_IGACOUNTS] = {0x81dc, 0x33988, 0x34088, 0x34788};
static    CBIOS_U32  PSCompFifoIndex[CBIOS_IGACOUNTS] = {0x81A4, 0x33910, 0x34010, 0x34710};
static    CBIOS_U32  PSBaseOffsetIndex[CBIOS_IGACOUNTS] = {0x33700, 0x3394C, 0x3404C, 0x3474C};
static    CBIOS_U32  PSRightBaseIndex[CBIOS_IGACOUNTS] = {0x81E0, 0x3398C, 0x3408C, 0x3478C};
static    CBIOS_U32  PSVsyncOffIndex[CBIOS_IGACOUNTS] = {0x81C4, 0x33908, 0x34008, 0x34708};
static    CBIOS_U32  PSStrideIndex[CBIOS_IGACOUNTS] = {0x81C8, 0x3390C, 0x3400C, 0x3470C};
static    CBIOS_U32  PSShadowIndex[CBIOS_IGACOUNTS] = {0x81FC, 0x33924, 0x34024, 0x34724};
//static    CBIOS_U32  PSWinWIndex[CBIOS_IGACOUNTS] = {0x33290, 0x3392C, 0x3402C, 0x3472C};
//static    CBIOS_U32  PSWinHIndex[CBIOS_IGACOUNTS] = {0x33294, 0x33930, 0x34030, 0x34730};
static    CBIOS_U32  PSDstWinPosIndex[CBIOS_IGACOUNTS] = {0x81B8, 0x33A18, 0x34118, 0x0};
static    CBIOS_U32  PSDstWinSizeIndex[CBIOS_IGACOUNTS] = {0x8208, 0x33A1C, 0x3411C, 0x0};
static    CBIOS_U32  PSScalRatWIndex[CBIOS_IGACOUNTS] = {0x33318, 0x33990, 0x34090, 0x34790};
static    CBIOS_U32  PSScalRatHIndex[CBIOS_IGACOUNTS] = {0x3331C, 0x33994, 0x34094, 0x34794};
static    CBIOS_U32  PSScalSrcIndex[CBIOS_IGACOUNTS] = {0x33320, 0x33998, 0x34098, 0x34798};

static    CBIOS_U32  PSCscIndex[CBIOS_IGACOUNTS] = {0x33644, 0x33944, 0x34044, 0x34744};
static    CBIOS_U32  PSCscCoef1[CBIOS_IGACOUNTS] = {0x33634, 0x33934, 0x34034, 0x34734};
static    CBIOS_U32  PSCscCoef2[CBIOS_IGACOUNTS] = {0x33638, 0x33938, 0x34038, 0x34738};
static    CBIOS_U32  PSCscCoef3[CBIOS_IGACOUNTS] = {0x3363c, 0x3393c, 0x3403c, 0x3473c};
static    CBIOS_U32  PSCscCoef4[CBIOS_IGACOUNTS] = {0x33640, 0x33940, 0x34040, 0x34740};
static    CBIOS_U32  PSBLIndex[CBIOS_IGACOUNTS] = {0x33810, 0x33E6C, 0x34574, 0x34c74};

static    CBIOS_U32  SSStartAddrIndex[CBIOS_IGACOUNTS] = {0x81D0, 0x339D4, 0x340D4, 0x347D4};
static    CBIOS_U32  SSCompFifoIndex[CBIOS_IGACOUNTS] = {0x8198, 0x339C8, 0x340C8, 0x347C8};
static    CBIOS_U32  SSBaseOffsetIndex[CBIOS_IGACOUNTS] = {0x33708, 0x33A14, 0x34114, 0x34814};
static    CBIOS_U32  SSEnableIndex[CBIOS_IGACOUNTS] = {0x33834, 0x33E70, 0x34578, 0x34C78};
static    CBIOS_U32  SSRightBaseIndex[CBIOS_IGACOUNTS] = {0x81D4, 0x339D8, 0x340D8, 0x347D8};
static    CBIOS_U32  SSStrideIndex[CBIOS_IGACOUNTS] = {0x81D8, 0x339DC, 0x340DC, 0x347DC};
static    CBIOS_U32  SSFmtSrcWIndex[CBIOS_IGACOUNTS] = {0x8190, 0x339C0, 0x340C0, 0x347C0};
static    CBIOS_U32  SSDstWinIndex[CBIOS_IGACOUNTS] = {0x819C, 0x339CC, 0x340CC, 0x347CC};
static    CBIOS_U32  SSSrcWinHIndex[CBIOS_IGACOUNTS] = {0x81A8, 0x339D0, 0x340D0, 0x347D0};
static    CBIOS_U32  SSDstWinPosIndex[CBIOS_IGACOUNTS] = {0x81F8, 0x339E8, 0x340E8, 0x347E8};
static    CBIOS_U32  SSScalRatioHIndex[CBIOS_IGACOUNTS] = {0x332E0, 0x339F4, 0x340F4, 0x347F4};
static    CBIOS_U32  SSScalRatioVIndex[CBIOS_IGACOUNTS] = {0x332E4, 0x339F8, 0x340F8, 0x347F8};
static    CBIOS_U32  SSCscIndex[CBIOS_IGACOUNTS] = {0x3362C, 0x33A10, 0x34110, 0x34810};
static    CBIOS_U32  SSCscCoef1[CBIOS_IGACOUNTS] = {0x3361c, 0x33a00, 0x34100, 0x34800};
static    CBIOS_U32  SSCscCoef2[CBIOS_IGACOUNTS] = {0x33620, 0x33a04, 0x34104, 0x34804};
static    CBIOS_U32  SSCscCoef3[CBIOS_IGACOUNTS] = {0x33624, 0x33a08, 0x34108, 0x34808};
static    CBIOS_U32  SSCscCoef4[CBIOS_IGACOUNTS] = {0x33628, 0x33a0c, 0x3410c, 0x3480c};
static    CBIOS_U32  SSBLIndex[CBIOS_IGACOUNTS] = {0x33854, 0x33a24, 0x34124, 0x34824};

static    CBIOS_U32  TQSStartAddrIndex[2][CBIOS_IGACOUNTS] = {{0x8430, 0x33A40, 0x34140, 0x34840},
{0x33404, 0x33AF4, 0x341F4, 0x348F4}};
static    CBIOS_U32  TQSCompFifoIndex[2][CBIOS_IGACOUNTS] = {{0x8414, 0x33A3C, 0x3413C, 0x3483C},
{0x33400, 0x33AF0, 0x341F0, 0x348F0}};
static    CBIOS_U32  TQSBaseOffsetIndex[2][CBIOS_IGACOUNTS] = {{0x8498, 0x33A78, 0x34178, 0x34878},
{0x33410, 0x33B00, 0x34200, 0x34900}};
static    CBIOS_U32  TQSEnableIndex[2][CBIOS_IGACOUNTS] = {{0x8458, 0x33A5C, 0x3415C, 0x3485C},
{0x3384C, 0x33B8C, 0x3428C, 0x3498C}};
static    CBIOS_U32  TQSRightBaseIndex[2][CBIOS_IGACOUNTS] = {{0x844C, 0x33A54, 0x34154, 0x34854},
{0x33408, 0x33AF8, 0x341F8, 0x348F8}};
static    CBIOS_U32  TQSStrideIndex[2][CBIOS_IGACOUNTS] = {{0x8438, 0x33A44, 0x34144, 0x34844},
{0x3340C, 0x33AFC, 0x341FC, 0x348FC}};
static    CBIOS_U32  TQSFormatIndex[2][CBIOS_IGACOUNTS] = {{0x33600, 0x33ABC, 0x341BC, 0x348BC},
{0x33480, 0x33B64, 0x34264, 0x34964}};
static    CBIOS_U32  TQSDstWinPosIndex[2][CBIOS_IGACOUNTS] = {{0x8444, 0x33A4C, 0x3414C, 0x3484C},
{0x33418, 0x33B08, 0x34208, 0x34908}};
static    CBIOS_U32  TQSDstWinIndex[2][CBIOS_IGACOUNTS] = {{0x8450, 0x33A58, 0x34158, 0x34858},
{0x3341C, 0x33B0C, 0x3420C, 0x3490C}};
static    CBIOS_U32  TQSSrcWinIndex[2][CBIOS_IGACOUNTS] = {{0x8448, 0x33A50, 0x34150, 0x34850},
{0x33414, 0x33B04, 0x34204, 0x34904}};
static    CBIOS_U32  TQSScalRatioHIndex[2][CBIOS_IGACOUNTS] = {{0x84A4, 0x33A84, 0x34184, 0x34884},
{0x33420, 0x33B10, 0x34210, 0x34910}};
static    CBIOS_U32  TQSScalRatioVIndex[2][CBIOS_IGACOUNTS] = {{0x84A8, 0x33A88, 0x34188, 0x34888},
{0x33424, 0x33B14, 0x34214, 0x34914}};
static    CBIOS_U32  TQSCscIndex[2][CBIOS_IGACOUNTS] = {{0x33614, 0x33AD0, 0x341D0, 0x348D0},
{0x33494, 0x33B78, 0x34278, 0x34978}};
static    CBIOS_U32  TQSCscCoef1[2][CBIOS_IGACOUNTS] = {{0x33604, 0x33ac0, 0x341c0, 0x348c0},
{0x33484, 0x33b68, 0x34268, 0x34968}};
static    CBIOS_U32  TQSCscCoef2[2][CBIOS_IGACOUNTS] = {{0x33608, 0x33ac4, 0x341c4, 0x348c4},
{0x33488, 0x33b6c, 0x3426c, 0x3496c}};
static    CBIOS_U32  TQSCscCoef3[2][CBIOS_IGACOUNTS] = {{0x3360c, 0x33ac8, 0x341c8, 0x348c8},
{0x3348c, 0x33b70, 0x34270, 0x34970}};
static    CBIOS_U32  TQSCscCoef4[2][CBIOS_IGACOUNTS] = {{0x33610, 0x33acc, 0x341cc, 0x348cc},
{0x33490, 0x33b74, 0x34274, 0x34974}};
static    CBIOS_U32  TQSBLIndex[2][CBIOS_IGACOUNTS] = {{0x33858, 0x33ad4, 0x341d4, 0x348d4},
{0x33860, 0x33b84, 0x34284, 0x34984}};

static  CBIOS_U32 StreamThresholdIndex[CBIOS_IGACOUNTS][STREAM_NUM_E3K] = 
{   //PS and SS share the same register
    {0x8454, 0x8454, 0x3346c, 0x33428},
    {0x33928,0x33928,0x33a9c, 0x33b18},
    {0x34028,0x34028,0x3419c, 0x34218},
    {0x34728,0x34728,0x3489c, 0x34918},
};



static    CBIOS_U32  BackgndColorIndex[CBIOS_IGACOUNTS] = {0x33670, 0x339A4, 0x340A4, 0x347A4};
static    CBIOS_U32  OneShotTrigIndex[CBIOS_IGACOUNTS] = {0x8200, 0x33958, 0x34058, 0x34758};
static    CBIOS_U32  VsyncOffBackdoorIndex[CBIOS_IGACOUNTS] = {0x8260, 0x33960, 0x3405C, 0};

CBIOS_U32  OvlKeyIndex[CBIOS_IGACOUNTS][OVL_NUM_E3K] = { 
                         {0x33840, 0x8194, 0x8410, 0x332BC},
                         {0x33E80, 0x339C4, 0x33A38, 0x33AE8},
                         {0x34590, 0x340C4, 0x34138, 0x341E8},
                         {0x34C90, 0x347C4, 0x34838, 0x348E8},
};

static    CBIOS_U32  OvlPlaneAlphaIndex[CBIOS_IGACOUNTS][OVL_NUM_E3K] = { 
                         {0x33844, 0x332A0, 0x332A4, 0x332C0},
                         {0x33E84, 0x339EC, 0x33A98, 0x33AEC},
                         {0x34594, 0x340EC, 0x34198, 0x341EC},
                         {0x34C94, 0x347EC, 0x34898, 0x348EC},
};

static CBIOS_U32 KeyHighIndex[CBIOS_IGACOUNTS][STREAM_NUM_E3K] =
{
    {0x8188, 0x332E8, 0x843C, 0x33498,},
    {0x3397C, 0x33A20, 0x33A48, 0x33B7C,},
    {0x3407C, 0x34120, 0x34148, 0x3427C,},
    {0x3477C, 0x34820, 0x34848, 0x3497C},
};

static CBIOS_U32 KeyLowIndex[CBIOS_IGACOUNTS][STREAM_NUM_E3K] =
{
    {0x81AC, 0x8184, 0x8400, 0x3342C,},
    {0x33980, 0x339BC, 0x33A34, 0x33B1C,},
    {0x34080, 0x340BC, 0x34134, 0x3421C,},
    {0x34780, 0x347BC, 0x34834, 0x3491C},
};

static CBIOS_U32 CursorCscIndex[CBIOS_IGACOUNTS] = {0x81e4, 0x33bb0, 0x342b0, 0x349b0};
static CBIOS_U32 CursorControl1Index[CBIOS_IGACOUNTS] = {0x33718, 0x33bb4, 0x342b4, 0x349b4};
static CBIOS_U32 CursorControl2Index[CBIOS_IGACOUNTS] = {0x3371c, 0x33bb8, 0x342b8, 0x349b8};
static CBIOS_U32 CursorBaseAddrIndex[CBIOS_IGACOUNTS] = {0x33720, 0x33bbc, 0x342bc, 0x349bc};
//static CBIOS_U32 CursorRightFrameBaseAddrIndex[CBIOS_IGACOUNTS] = {0x33724, 0x33bc0, 0x342c0, 0x349c0};
static CBIOS_U32 CursorEndPixelIndex[CBIOS_IGACOUNTS] = {0x33728, 0x33bc4, 0x342c4, 0x349c4};

static CBIOS_U16 LUTBitWidthIndex[CBIOS_IGACOUNTS] = {CR_E4, CR_E5, CR_E6, CR_B_CD};


/******** The following tables are for HDTV encoders ********/
/*
static CBREGISTER_IDX TriLevelSyncWidth_INDEX[] = {  
    {SR_B_71,0x3F}, 
    {MAPMASK_EXIT},
};

static CBREGISTER_IDX BlankLevel_INDEX[] = {  
    {SR_B_72,0xFF},
    {SR_B_73,0x03},     
    {MAPMASK_EXIT},
};

static CBREGISTER_IDX SyncDelayReg_INDEX[] = {  
    {SR_B_7E,0xff},
    {SR_B_7D,0x07},
    {MAPMASK_EXIT},
};

static CBREGISTER_IDX HSyncToHActive_INDEX[] = {  
    {SR_B_8A,0xFF}, 
    {SR_B_89,0x07},    
    {MAPMASK_EXIT},
};

static CBREGISTER_IDX HSyncWidth_INDEX[] = {
    {SR_B_A3, 0xFF},
    {MAPMASK_EXIT},
};

static CBREGISTER_IDX HSyncDelay_INDEX[] = {  
    {SR_B_AA,0x07},
    {MAPMASK_EXIT},
};

static CBREGISTER_IDX BroadPulseReg_INDEX[] = {  
    {SR_B_D0,0x7F},
    {SR_B_D1,0xFF},    
    {MAPMASK_EXIT},
};

static CBREGISTER_IDX HalfSyncReg_INDEX[] = {  
    {SR_B_D2,0xFF},
    {SR_B_D3,0x07},    
    {MAPMASK_EXIT},
};

static CBREGISTER_IDX HDTVHDEReg_INDEX[] = {  
    {SR_B_E4,0xFF},
    {SR_B_E5,0x07},    
    {MAPMASK_EXIT},
};
*/
static CBREGISTER_IDX HSS2Reg_INDEX[] = {
    {SR_64,0xff},   //SR64[7:0]
    {SR_66,0x08},   //SR66[3]
    {SR_67,0x08},   //SR67[3]
    {SR_2E,0x40},   //SR2E[6]
    {MAPMASK_EXIT},
};

static CBREGISTER_IDX HT2Reg_INDEX[] = {
    {SR_60,0xff},   //SR60[7:0]
    {SR_66,0x01},   //SR66[0]
    {SR_67,0x01},   //SR67[0]
    {SR_2E,0x01},   //SR2E[0]
    {MAPMASK_EXIT},
};

static CBREGISTER_IDX HSE2Reg_INDEX[] = {
    {SR_65,0x3f},   //SR65[5:0]
    {MAPMASK_EXIT},
};

static CBREGISTER_IDX HDE2Reg_INDEX[] = {
    {SR_61,0xff},   //SR61[7:0]
    {SR_66,0x02},   //SR66[1]
    {SR_67,0x02},   //SR67[1]
    {SR_2E,0x02},   //SR2E[1]
    {MAPMASK_EXIT},
};

static CBREGISTER_IDX HBS2Reg_INDEX[] = {
    {SR_62,0xff},   //SR62[7:0]
    {SR_66,0x04},   //SR66[2]
    {SR_67,0x04},   //SR67[2]
    {SR_2E,0x04},   //SR2E[2]
    {MAPMASK_EXIT},
};

static CBREGISTER_IDX HBE2Reg_INDEX[] = {
    {SR_63,0x7f},   //SR63[6:0]
    {SR_2E,0x38},   //SR2E[5:3]
    {MAPMASK_EXIT},
};

static CBREGISTER_IDX VT2Reg_INDEX[] = {
    {SR_68,0xff},   //SR68[7:0]
    {SR_6E,0x0f},   //SR6E[3:0]
    {SR_2F,0X01},   //SR2F[0]
    {MAPMASK_EXIT},
};

static CBREGISTER_IDX VDE2Reg_INDEX[] = {
    {SR_69,0xff},   //SR69[7:0]
    {SR_6E,0xf0},   //SR6E[7:4]
    {SR_2F,0X02},   //SR2F[1]
    {MAPMASK_EXIT},
};

static CBREGISTER_IDX VBS2Reg_INDEX[] = {
    {SR_6A,0xFF},   //SR6A[7:0]
    {SR_6F,0x0f},   //SR6F[3:0]
    {SR_2F,0X04},   //SR2F[2]
    {MAPMASK_EXIT},
};

static CBREGISTER_IDX VBE2Reg_INDEX[] = {
    {SR_6B,0xff},   //SR6B[7:0]
    {SR_2F,0X08},   //SR2F[3]
    {MAPMASK_EXIT},
};

static CBREGISTER_IDX VSS2Reg_INDEX[] = {
    {SR_6C,0xff},   //SR6C[7:0]
    {SR_6F,0xf0},   //SR6F[7:4]
    {SR_2F,0X10},   //SR2F[4]
    {MAPMASK_EXIT},
};

static CBREGISTER_IDX VSE2Reg_INDEX[] = {
    {SR_6D,0x0f},   //SR6D[3:0]
    {SR_2F,0x60},   //SR2F[6:5]
    {MAPMASK_EXIT},
};

static CBREGISTER_IDX HorSyncStartReg_INDEX[] = {
    {    CR_04,0xFF     }, //CR04[7:0]
    {    CR_5D,0x10     }, //CR5D[4]
    {    CR_5F, 0x08    }, //CR5F[3]
    {    CR_74, 0x40    }, //CR74[6]
    {    MAPMASK_EXIT},
};

static CBREGISTER_IDX HorTotalReg_INDEX[] = {
    {    CR_00, 0xFF    }, //CR00[7:0]
    {    CR_5D, 0x01    }, //CR5D[0]
    {    CR_5F, 0x01    }, //CR5F[0]
    {    CR_74, 0x01    }, //CR74[0]
    {    MAPMASK_EXIT},
};

static CBREGISTER_IDX HorSyncEndReg_INDEX[] = {
    {    CR_05, 0x1F    }, //CR05[4:0]
    {    CR_5D, 0x20    }, //CR5D[5]
    {    MAPMASK_EXIT},
};

static CBREGISTER_IDX HorDisEndReg_INDEX[] = {
    {    CR_01, 0xFF    }, //CR01[7:0]
    {    CR_5D, 0x02    }, //CR5D[1]
    {    CR_5F, 0x02    }, //CR5F[1]
    {    CR_74, 0x02    }, //CR74[1]
    {    MAPMASK_EXIT},
};

static CBREGISTER_IDX HorBStartReg_INDEX[] = {
    {    CR_02, 0xFF    }, //CR02[7:0]
    {    CR_5D, 0x04    }, //CR5D[2]
    {    CR_5F, 0x04    }, //CR5F[2]
    {    CR_74, 0x04    }, //CR74[2]
    {    MAPMASK_EXIT},
};

static CBREGISTER_IDX HorBEndReg_INDEX[] = {
    {    CR_03, 0x1F    }, //CR03[4:0]
    {    CR_05, 0x80    }, //CR05[7]
    {    CR_5D, 0x08    }, //CR5D[3]
    {    CR_74, 0x38    }, //CR74[5:3]
    {    MAPMASK_EXIT},
};

static CBREGISTER_IDX VerTotalReg_INDEX[] = {
    {    CR_06, 0xFF    }, //CR06[7:0]
    {    CR_07, 0x21    }, //CR07[0], CR07[5]
    {    CR_63, 0xC0    }, //CR63[7:6]
    {    CR_79, 0x04    }, //CR79[2]
    {    MAPMASK_EXIT},
};

static CBREGISTER_IDX VerDisEndReg_INDEX[] = {
    {    CR_12, 0xFF    }, //CR12[7:0]
    {    CR_07, 0x42    }, //CR07[1], CR07[6]
    {    CR_5E, 0x03    }, //CR5E[1:0]
    {    CR_79, 0x08    }, //CR79[3]
    {    MAPMASK_EXIT},
};
static CBREGISTER_IDX VerBStartReg_INDEX[] = {
    {    CR_15, 0xFF    }, //CR15[7:0]
    {    CR_07, 0x08    }, //CR07[3]
    {    CR_09, 0x20    }, //CR09[5]
    {    CR_5E, 0x0C    }, //CR5E[3:2]
    {    CR_79, 0x10    }, //CR79[4]
    {    MAPMASK_EXIT},
};

static CBREGISTER_IDX VerBEndReg_INDEX[] = {
    {    CR_16, 0xFF    }, //CR16[7:0]
    {    CR_79, 0x20    }, //CR79[5]
    {    MAPMASK_EXIT},
};

static CBREGISTER_IDX VerSyncStartReg_INDEX[] = {
    {    CR_10, 0xFF    }, //CR10[7:0]
    {    CR_07, 0x84    }, //CR07[2], CR07[7]
    {    CR_5E, 0x30    }, //CR5E[5:4]
    {    CR_79, 0x01    }, //CR79[0]
    {    MAPMASK_EXIT},
};

static CBREGISTER_IDX VerSyncEndReg_INDEX[] = {
    {    CR_11, 0x0F    }, //CR11[3:0]
    {    CR_79, 0x42    }, //CR79[1], CR79[6]
    {    MAPMASK_EXIT},
};

extern CBIOS_U32 HDTV_REG_LB[4];

CBIOS_VOID cbInitChipAttribute_E3K(PCBIOS_EXTENSION_COMMON pcbe)
{
    PCBIOS_CHIP_FUNC_TABLE pFuncTbl = &(pcbe->ChipFuncTbl);

    //init chip dependent functions
    pFuncTbl->pfncbGetModeInfoFromReg = (PFN_cbGetModeInfoFromReg)cbGetModeInfoFromReg_E3K;
    pFuncTbl->pfncbInitVCP = (PFN_cbInitVCP)cbInitVCP_E3K;
    pFuncTbl->pfncbDoHDTVFuncSetting = (PFN_cbDoHDTVFuncSetting)cbDoHDTVFuncSetting_E3K;
    pFuncTbl->pfncbLoadSSC = CBIOS_NULL;
    pFuncTbl->pfncbEnableSpreadSpectrum = CBIOS_NULL;
    pFuncTbl->pfncbDisableSpreadSpectrum = CBIOS_NULL;
    pFuncTbl->pfncbSetGamma = (PFN_cbSetGamma)cbSetGamma_E3K;
    pFuncTbl->pfncbCECEnableDisable = (PFN_cbCECEnableDisable)cbCECEnableDisable_E3K;
    pFuncTbl->pfncbGetDispAddr = (PFN_cbGetDispAddr)cbGetDispAddr_E3K;

    pFuncTbl->pfncbSetHwCursor = (PFN_cbSetHwCursor)cbSetHwCursor_E3K;
    pFuncTbl->pfncbSetCRTimingReg = (PFN_cbSetTimingReg)cbSetCRTimingReg_E3K;
    pFuncTbl->pfncbSetSRTimingReg = (PFN_cbSetTimingReg)cbSetSRTimingReg_E3K;
    pFuncTbl->pfncbDoCSCAdjust = (PFN_cbDoCSCAdjust)cbDoCSCAdjust_E3K;
    pFuncTbl->pfncbAdjustStreamCSC = (PFN_cbAdjustStreamCSC)cbAdjustStreamCSC_E3K;

    pFuncTbl->pfncbUpdateFrame = (PFN_cbUpdateFrame)cbUpdateFrame_E3K;
    pFuncTbl->pfncbDisableStream = (PFN_cbDisableStream)cbDisableStream_E3K;

    if(pcbe->ChipID >= CHIPID_CHX004)
    {
        pFuncTbl->pfncbUpdateShadowInfo =(PFN_cbUpdateShadowInfo)cbUpdateShadowInfo_CX4;
    }
}

CBIOS_VOID cbSetSRTimingReg_E3K(PCBIOS_EXTENSION_COMMON pcbe,
                                PCBIOS_TIMING_ATTRIB pTiming,
                                CBIOS_U32 IGAIndex,
                                PCBIOS_TIMING_FLAGS pFlags)
{
    CBIOS_U32   ulBlankingTime = 0,ulSyncWidth= 0, ulBackPorchWidth = 0, ulNewBackPorchWidth = 0;
    CBIOS_TIMING_REG_E3K   TimingReg;
    CBIOS_U16   tmp_HorSyncStart = 0, tmp_HorSyncEnd = 0, HsyncDelay = 0, VsyncDelay = 0;
    
    cb_memset(&TimingReg, 0, sizeof(CBIOS_TIMING_REG_E3K));

    //If BackPorchWidth less than 32 and can't be divisible by 8,some monitors
    //will display skew to left, so slightly adjust timing parameters to patch it
    ulBackPorchWidth = pTiming->HorTotal - pTiming->HorSyncEnd;

    if((ulBackPorchWidth < 32) && (ulBackPorchWidth %8 != 0))
    {
        tmp_HorSyncStart = pTiming->HorSyncStart;
        tmp_HorSyncEnd = pTiming->HorSyncEnd;
        while(tmp_HorSyncStart - pTiming->HorDisEnd > 16)//adjust HsyncStart,but FrontPortchWidth need greater than 16
        {
            tmp_HorSyncStart--;
            tmp_HorSyncEnd--;
            ulNewBackPorchWidth = pTiming->HorTotal - tmp_HorSyncEnd;
            HsyncDelay = pTiming->HorSyncStart - tmp_HorSyncStart;
            if((ulNewBackPorchWidth % 8 == 0) && (HsyncDelay > 2))//loop util new backPorchWidth can be divisible by 8 and HsyncDelay > 2
            {
                pTiming->HorSyncStart = tmp_HorSyncStart;
                pTiming->HorSyncEnd = tmp_HorSyncEnd;
                pTiming->VerSyncStart--;
                pTiming->VerSyncEnd--;
                HsyncDelay -= 2;
                VsyncDelay = 1;
                pFlags->IsUseHbackPatch = 1;
                break;
            }
        }
    }

    //================================================================//
    //************Start Setting Horizontal Timing Registers***********//
    //================================================================//

    //(1) Adjust (H Bottom/Right Border + H Front Porch) period: CR72[2:0] = (Hor sync start position in pixel)%8
    TimingReg.HSSRemainder = pTiming->HorSyncStart % 8;
    cbBiosMMIOWriteReg(pcbe, CR_72, TimingReg.HSSRemainder, 0xF8, IGAIndex);

    //(2) Adjust HSYNC period: CR72[5:3] = (the number of pixel in Hor sync time)%8
    ulSyncWidth = pTiming->HorSyncEnd - pTiming->HorSyncStart;
    TimingReg.HSyncRemainder = ulSyncWidth % 8;
    ulSyncWidth = cbRound(ulSyncWidth, CELLGRAN, ROUND_DOWN);
    cbBiosMMIOWriteReg(pcbe, CR_72, (TimingReg.HSyncRemainder) << 3, 0xC7, IGAIndex);

    //(3) Adjust (H Back Porch + H Top/Left Border) period: CR73[2:0] = (the number of pixel in (Hor back porch + H top/left border))%8
    ulBackPorchWidth = pTiming->HorTotal - pTiming->HorSyncEnd;
    TimingReg.HBackPorchRemainder = ulBackPorchWidth % 8;
    cbBiosMMIOWriteReg(pcbe, CR_73, TimingReg.HBackPorchRemainder, 0xF8, IGAIndex);
    
    //(4) Start Horizontal Sync Position = HSync Start(in pixel)/8
    TimingReg.HorSyncStart = (CBIOS_U16)cbRound(pTiming->HorSyncStart, CELLGRAN, ROUND_DOWN);
    cbMapMaskWrite(pcbe, (CBIOS_U32)TimingReg.HorSyncStart, HSS2Reg_INDEX, IGAIndex);
    
    //(5) Htotal = (number of character clocks in one scan line) - 5 - (CR72[2:0]+CR72[5:3]+CR73[2:0])/8
    TimingReg.HorTotal = (CBIOS_U16)cbRound(pTiming->HorTotal, CELLGRAN, ROUND_DOWN) - 5;
    TimingReg.HorTotal -= (CBIOS_U16)cbRound(TimingReg.HSSRemainder + TimingReg.HSyncRemainder + 
                                             TimingReg.HBackPorchRemainder, CELLGRAN, ROUND_DOWN);
    cbMapMaskWrite(pcbe, (CBIOS_U32)TimingReg.HorTotal, HT2Reg_INDEX, IGAIndex);

    //(6) End Horizontal Sync Position = {Start Horizontal Sync Position + Hsync time(in pixel)/8}[5:0]
    TimingReg.HorSyncEnd = TimingReg.HorSyncStart + (CBIOS_U16)ulSyncWidth;
    if (TimingReg.HorSyncEnd > TimingReg.HorTotal + 3)
    {
        TimingReg.HorSyncEnd = TimingReg.HorTotal + 3;
    }
    TimingReg.HorSyncEnd &= 0x1F;
    if(ulSyncWidth > 32)
    {
        TimingReg.HorSyncEnd |= 0x20;//sr65[5]set when Hsynwidth>32char
    }
    else
    {
        TimingReg.HorSyncEnd &= ~0x20;//sr65[5]set when Hsynwidth>32char
    }    
    cbMapMaskWrite(pcbe, (CBIOS_U32)TimingReg.HorSyncEnd, HSE2Reg_INDEX, IGAIndex);


    //(7) Adjust HDE:CR58[2:0] = (the number of pixel for 1 line of active display)%8 - 1
    TimingReg.HDERemainder = pTiming->HorDisEnd % 8;
    if (TimingReg.HDERemainder != 0)
    {
        TimingReg.HDERemainder = (TimingReg.HDERemainder - 1) & 0x07;
        cbBiosMMIOWriteReg(pcbe, CR_58, TimingReg.HDERemainder, 0xF8, IGAIndex);
        //enable HDE adjust
        cbBiosMMIOWriteReg(pcbe, CR_58, 0x08, 0xF7, IGAIndex);
    }
    else
    {
        //disable HDE adjust
        cbBiosMMIOWriteReg(pcbe, CR_58, 0x00, 0xF0, IGAIndex);
    }
    

    //(8) Horizontal Display End = (the number of pixel for 1 line of active display)/8 - 1, rounded up to the nearest integer.
    TimingReg.HorDisEnd = (CBIOS_U16)cbRound(pTiming->HorDisEnd, CELLGRAN, ROUND_UP) - 1;
    cbMapMaskWrite(pcbe, (CBIOS_U32)TimingReg.HorDisEnd, HDE2Reg_INDEX, IGAIndex);

    //(9) Horizontal Blank Start
    TimingReg.HorBStart = (CBIOS_U16)cbRound(pTiming->HorBStart, CELLGRAN, ROUND_UP);
    cbMapMaskWrite(pcbe, (CBIOS_U32)TimingReg.HorBStart, HBS2Reg_INDEX, IGAIndex);

    //(10) Horizontal Blank End
    ulBlankingTime = cbRound(pTiming->HorBEnd - pTiming->HorBStart, CELLGRAN, ROUND_DOWN);
    TimingReg.HorBEnd = TimingReg.HorBStart + ulBlankingTime;
    if (TimingReg.HorBEnd > TimingReg.HorTotal + 3)
    {
        TimingReg.HorBEnd = TimingReg.HorTotal + 3;
    }
      
    cbMapMaskWrite(pcbe, (CBIOS_U32)TimingReg.HorBEnd, HBE2Reg_INDEX, IGAIndex);
            
    //================================================================//
    //************Start Setting Vertical Timing Registers*************//
    //================================================================//
    TimingReg.VerTotal = pTiming->VerTotal - 2;//VerTotalTime-2
    cbMapMaskWrite(pcbe,(CBIOS_U32) TimingReg.VerTotal,VT2Reg_INDEX, IGAIndex);

    TimingReg.VerDisEnd = pTiming->VerDisEnd - 1;//HorAddrTime-1
    cbMapMaskWrite(pcbe, (CBIOS_U32)TimingReg.VerDisEnd, VDE2Reg_INDEX, IGAIndex);

    TimingReg.VerBStart = pTiming->VerBStart - 1;//VerBlankStart
    //According to spec, both CR15 and SR6A need -1, move the -1 patch to cbConvertVesaTimingToInternalTiming_dst
    //Add this patch for HDMI to pass golden file check. Later SE will help measure the output timing for HDMI
    // to see whether this patch is still needed.
    cbMapMaskWrite(pcbe, (CBIOS_U32)TimingReg.VerBStart, VBS2Reg_INDEX, IGAIndex);

    ulBlankingTime = pTiming->VerBEnd - pTiming->VerBStart;
    TimingReg.VerBEnd = TimingReg.VerBStart + ulBlankingTime;//(VerBlankStart+VerBlankTime) and 00FFh
    if (TimingReg.VerBEnd > TimingReg.VerTotal)
    {
        TimingReg.VerBEnd = TimingReg.VerTotal;
    }

    cbMapMaskWrite(pcbe, (CBIOS_U32)TimingReg.VerBEnd, VBE2Reg_INDEX, IGAIndex);

    //According to spec, SR6C[7-0] = [scan line counter value at which the vertical sync pulse (FLM) becomes active] -1
    //CR10 needn't -1
    TimingReg.VerSyncStart = pTiming->VerSyncStart - 1;//VerSyncStart
    cbMapMaskWrite(pcbe, (CBIOS_U32)TimingReg.VerSyncStart, VSS2Reg_INDEX, IGAIndex);

    //SR6D = SR6C+VerSyncTime
    ulSyncWidth = pTiming->VerSyncEnd - pTiming->VerSyncStart;
    TimingReg.VerSyncEnd = TimingReg.VerSyncStart + ulSyncWidth; //(VerSyncStart+VerSyncTime) and 000Fh
    if (TimingReg.VerSyncEnd > TimingReg.VerTotal)
    {
        TimingReg.VerSyncEnd = TimingReg.VerTotal;
    }
    TimingReg.VerSyncEnd &= 0x3F;
    cbMapMaskWrite(pcbe, (CBIOS_U32)TimingReg.VerSyncEnd, VSE2Reg_INDEX, IGAIndex);
    if(pFlags->IsUseHbackPatch)
    {
        cbBiosMMIOWriteReg(pcbe, SR_57, (CBIOS_U8)(HsyncDelay), 0x0, IGAIndex);//Set HsyncDelay to Upscaler HSYNC Delay
        cbBiosMMIOWriteReg(pcbe, SR_58, (CBIOS_U8)(VsyncDelay), 0xC0, IGAIndex);//Set VsyncDelay to Upscaler VSYNC Delay
    }
    else
    {
        cbBiosMMIOWriteReg(pcbe, SR_57, 0, 0x0, IGAIndex);//Set 0 to Upscaler HSYNC Delay
        cbBiosMMIOWriteReg(pcbe, SR_58, 0, 0xC0, IGAIndex);//Set 0 to Upscaler VSYNC Delay
    }

}


CBIOS_VOID cbSetCRTimingReg_E3K(PCBIOS_EXTENSION_COMMON pcbe,
                                PCBIOS_TIMING_ATTRIB pTiming,
                                CBIOS_U32 IGAIndex,
                                PCBIOS_TIMING_FLAGS pFlags)
{
    CBIOS_U32   ulBlankingTime = 0,ulSyncWidth= 0;
    CBIOS_TIMING_REG_E3K  TimingReg;

    //patch for unlocking CR: CR is locked  for some reason, so unlock CR here temporarily
    cbBiosMMIOWriteReg(pcbe, CR_11, 0, 0x7f, IGAIndex);

    cb_memset(&TimingReg, 0, sizeof(CBIOS_TIMING_REG_E3K));
    
    //================================================================//
    //************Start Setting Horizontal Timing Registers***********//
    //================================================================//

    ulSyncWidth = cbRound(pTiming->HorSyncEnd - pTiming->HorSyncStart, CELLGRAN, ROUND_DOWN);
    TimingReg.HorSyncStart = (CBIOS_U16)cbRound(pTiming->HorSyncStart, CELLGRAN, ROUND_UP);
   


    //=========================Set HorSyncStart: CR04, CR5D, CR5F===================//	
    cbMapMaskWrite(pcbe, (CBIOS_U32)TimingReg.HorSyncStart, HorSyncStartReg_INDEX, IGAIndex);
    //===================================================================//	


    TimingReg.HorTotal = (CBIOS_U16)cbRound(pTiming->HorTotal, CELLGRAN, ROUND_UP) - 5;	


    //========================Set HorTotal: CR00, CR5D, CR5F=====================//	
    cbMapMaskWrite(pcbe, (CBIOS_U32)TimingReg.HorTotal, HorTotalReg_INDEX, IGAIndex);
    //==================================================================//


    TimingReg.HorSyncEnd = TimingReg.HorSyncStart + (CBIOS_U16)ulSyncWidth;
    if (TimingReg.HorSyncEnd > TimingReg.HorTotal + 3)
    {
        TimingReg.HorSyncEnd = TimingReg.HorTotal + 3;
    }
    TimingReg.HorSyncEnd &= 0x1F;
    if(ulSyncWidth > 32)
    {
        TimingReg.HorSyncEnd |= 0x20;//sr65[5]set when Hsynwidth>32char
    }
    else
    {
        TimingReg.HorSyncEnd &= ~0x20;//sr65[5]set when Hsynwidth>32char
    }    


    //=============================Set HorSyncEnd: CR05, CR5D======================//	
    cbMapMaskWrite(pcbe, (CBIOS_U32)TimingReg.HorSyncEnd, HorSyncEndReg_INDEX, IGAIndex);
    //======================================================================//




    TimingReg.HorDisEnd = (CBIOS_U16)cbRound(pTiming->HorDisEnd, CELLGRAN, ROUND_UP) - 1;

    //=======================Set HorDisEnd: CR01, CR5D, CR5F=======================//	
    cbMapMaskWrite(pcbe, (CBIOS_U32)TimingReg.HorDisEnd, HorDisEndReg_INDEX, IGAIndex);
    //====================================================================//


    TimingReg.HorBStart = (CBIOS_U16)cbRound(pTiming->HorBStart, CELLGRAN, ROUND_UP);

    //=====================Set HorBStart: CR02, CR5D, CR5F========================//	
    cbMapMaskWrite(pcbe, (CBIOS_U32)TimingReg.HorBStart, HorBStartReg_INDEX, IGAIndex);
    //===================================================================//


    ulBlankingTime = cbRound(pTiming->HorBEnd - pTiming->HorBStart, CELLGRAN, ROUND_DOWN);
    TimingReg.HorBEnd = TimingReg.HorBStart + (CBIOS_U16)ulBlankingTime;


    if (TimingReg.HorBEnd > TimingReg.HorTotal + 3)
    {
        TimingReg.HorBEnd = TimingReg.HorTotal + 3;
    }
    TimingReg.HorBEnd &= 0x3F;
    if(ulBlankingTime > 64)
    {
        TimingReg.HorBEnd |= 0x40;
    }
    else
    {
        TimingReg.HorBEnd &= ~0x40;
    }  

    //==================Set HorBEnd: CR03, CR05, CR5D=========================//	
    cbMapMaskWrite(pcbe, (CBIOS_U32)TimingReg.HorBEnd, HorBEndReg_INDEX, IGAIndex);
    //================================================================//



    //================================================================//
    //************Start Setting Vertical Timing Registers*************//
    //================================================================//
    TimingReg.VerTotal = pTiming->VerTotal - 2;//VerTotalTime-2


    //======================Set VerTotal: CR06, CR07, CR63======================//	
    cbMapMaskWrite(pcbe, (CBIOS_U32) TimingReg.VerTotal, VerTotalReg_INDEX, IGAIndex);
    //================================================================//


    TimingReg.VerDisEnd = pTiming->VerDisEnd - 1;//HorAddrTime-1
    
    //====================Set VerDisEnd: CR12, CR07, CR5E==========================//
    cbMapMaskWrite(pcbe, (CBIOS_U32)TimingReg.VerDisEnd, VerDisEndReg_INDEX, IGAIndex);
    //====================================================================//



    TimingReg.VerBStart = pTiming->VerBStart - 1;//VerBlankStart
    //According to spec, both CR15 and SR6A need -1, move the -1 patch to cbConvertVesaTimingToInternalTiming_dst
    //Add this patch for HDMI to pass golden file check. Later SE will help measure the output timing for HDMI
    // to see whether this patch is still needed.
    cbMapMaskWrite(pcbe, (CBIOS_U32)TimingReg.VerBStart, VerBStartReg_INDEX, IGAIndex);

    ulBlankingTime = pTiming->HorBEnd - pTiming->HorBStart;
    TimingReg.VerBEnd = TimingReg.VerBStart + ulBlankingTime;//(VerBlankStart+VerBlankTime) and 00FFh
    if (TimingReg.VerBEnd > TimingReg.VerTotal)
    {
        TimingReg.VerBEnd = TimingReg.VerTotal;
    }
    TimingReg.VerBEnd &= 0xFF;
    cbMapMaskWrite(pcbe, (CBIOS_U32)TimingReg.VerBEnd, VerBEndReg_INDEX, IGAIndex);

    //According to spec, SR6C[7-0] = [scan line counter value at which the vertical sync pulse (FLM) becomes active] -1
    //CR10 needn't -1
    TimingReg.VerSyncStart = pTiming->VerSyncStart;//VerSyncStart
    cbMapMaskWrite(pcbe, (CBIOS_U32)TimingReg.VerSyncStart, VerSyncStartReg_INDEX, IGAIndex);

    //SR6D = SR6C+VerSyncTime
    ulSyncWidth = pTiming->VerSyncEnd - pTiming->VerSyncStart;
    TimingReg.VerSyncEnd = TimingReg.VerSyncStart + ulSyncWidth; //(VerSyncStart+VerSyncTime) and 000Fh
    if (TimingReg.VerSyncEnd > TimingReg.VerTotal)
    {
        TimingReg.VerSyncEnd = TimingReg.VerTotal;
    }
    TimingReg.VerSyncEnd &= 0x3F;
    cbMapMaskWrite(pcbe, (CBIOS_U32)TimingReg.VerSyncEnd, VerSyncEndReg_INDEX, IGAIndex);

}

CBIOS_VOID cbGetCRTimingReg_E3K(PCBIOS_EXTENSION_COMMON pcbe,
                                PCBIOS_TIMING_ATTRIB pTiming,
                                CBIOS_U32 IGAIndex,
                                PCBIOS_TIMING_FLAGS pFlags)
{
    CBIOS_U32 temp = 0;
    CBIOS_U8 ClockType = CBIOS_DCLK1TYPE;
    CBIOS_U32 CurrentFreq = 0, CurrPLLClock = 0;
    CBIOS_U32   remainder = 0;

    cb_memset(pTiming, 0, sizeof(CBIOS_TIMING_ATTRIB));
    cb_memset(pFlags, 0, sizeof(CBIOS_TIMING_FLAGS));

    if(IGAIndex == IGA1)
    {
        ClockType = CBIOS_DCLK1TYPE;
    }
    else if(IGAIndex == IGA2)
    {
        ClockType = CBIOS_DCLK2TYPE;
    }
    else if(IGAIndex == IGA3)
    {
        ClockType = CBIOS_DCLK3TYPE;
    }
    else if(IGAIndex == IGA4)
    {
        ClockType = CBIOS_DCLK4TYPE;
    }
    cbGetProgClock(pcbe, &CurrentFreq, ClockType);
    CurrPLLClock = CurrentFreq;

    //================================================================//
    //************Start Getting Horizontal Timing Registers***********//
    //================================================================//
    pTiming->HorTotal = (CBIOS_U16)cbMapMaskRead(pcbe, HorTotalReg_INDEX, IGAIndex); //Horizontal Total
    pTiming->HorTotal += 5;
    pTiming->HorTotal *= 8;

    pTiming->HorDisEnd = (CBIOS_U16)cbMapMaskRead(pcbe, HorDisEndReg_INDEX, IGAIndex); //Horizontal Display End
    pTiming->HorDisEnd++;
    pTiming->HorDisEnd *= 8;

    pTiming->HorBStart = (CBIOS_U16)cbMapMaskRead(pcbe, HorBStartReg_INDEX, IGAIndex); //Horizontal Blank Start
    pTiming->HorBStart *= 8;

    temp = cbMapMaskRead(pcbe, HorBEndReg_INDEX, IGAIndex); //Horizontal Blank End
    pTiming->HorBEnd =  (temp & 0x3F) | ((pTiming->HorBStart/8) & ~0x3F);
    if (temp & BIT6)
    {
        pTiming->HorBEnd += 0x40;
    }
    if (pTiming->HorBEnd <= (pTiming->HorBStart/8))
    {
        pTiming->HorBEnd += 0x40;
    }
    if (pTiming->HorBEnd >= (pTiming->HorTotal/8) - 2)
    {
        pTiming->HorBEnd += 2;
    }
    pTiming->HorBEnd *= 8;

    pTiming->HorSyncStart = (CBIOS_U16)cbMapMaskRead(pcbe, HorSyncStartReg_INDEX, IGAIndex); //Horizontal Sync Start
    pTiming->HorSyncStart *= 8;

    temp = cbMapMaskRead(pcbe, HorSyncEndReg_INDEX, IGAIndex); //Horizontal Sync End
    pTiming->HorSyncEnd =  (temp & 0x1F) | ((pTiming->HorSyncStart/8) & ~0x1F);
    if (temp & BIT5)
    {
        pTiming->HorSyncEnd += 0x20;
    }
    if (pTiming->HorSyncEnd <= (pTiming->HorSyncStart/8))
    {
        pTiming->HorSyncEnd += 0x20;
    }
    pTiming->HorSyncEnd *= 8;

    //================================================================//
    //************Start Getting Vertical Timing Registers*************//
    //================================================================//
    pTiming->VerTotal = (CBIOS_U16)cbMapMaskRead(pcbe, VerTotalReg_INDEX, IGAIndex); //Vertical Total
    pTiming->VerTotal +=2;

    pTiming->VerDisEnd = (CBIOS_U16)cbMapMaskRead(pcbe, VerDisEndReg_INDEX, IGAIndex); //Vertical Display End
    pTiming->VerDisEnd++;

    pTiming->VerBStart = (CBIOS_U16)cbMapMaskRead(pcbe, VerBStartReg_INDEX, IGAIndex); //Vertical Blank Start
    pTiming->VerBStart++;

    temp = cbMapMaskRead(pcbe, VerBEndReg_INDEX, IGAIndex); //Vertical Blank End
    temp++;

    pTiming->VerBEnd = (temp & 0xFF) | (pTiming->VerBStart & ~(0xFF));
    if (pTiming->VerBEnd <= pTiming->VerBStart )
    {
        pTiming->VerBEnd += 0x100;
    }
    if(pTiming->VerBEnd >= pTiming->VerTotal - 2)
    {
        pTiming->VerBEnd += 1;
    }

    pTiming->VerSyncStart = (CBIOS_U16)cbMapMaskRead(pcbe, VerSyncStartReg_INDEX, IGAIndex); //Vertical Sync Start

    temp = cbMapMaskRead(pcbe, VerSyncEndReg_INDEX, IGAIndex); //Vertical Sync End
    pTiming->VerSyncEnd = (temp & 0x0F) | (pTiming->VerSyncStart & ~(0x0F));
    if (pTiming->VerSyncEnd <= pTiming->VerSyncStart)
    {
        pTiming->VerSyncEnd += 0x10;
    }

    //calculate the result value
    pTiming->XRes = pTiming->HorDisEnd;
    pTiming->YRes = pTiming->VerDisEnd;

    temp = (CBIOS_U32)pTiming->HorTotal * (CBIOS_U32)pTiming->VerTotal;
    pTiming->RefreshRate = CurrPLLClock * 100 / temp;
    remainder = (CurrPLLClock * 100) % temp;
    pTiming->RefreshRate = pTiming->RefreshRate * 100 + remainder * 100 / temp;
}

CBIOS_VOID cbGetSRTimingReg_E3K(PCBIOS_EXTENSION_COMMON pcbe,
                                PCBIOS_TIMING_ATTRIB pTiming,
                                CBIOS_U32 IGAIndex,
                                PCBIOS_TIMING_FLAGS pFlags)
{
    CBIOS_U32 temp = 0;
    CBIOS_U8 ClockType = CBIOS_DCLK1TYPE;
    CBIOS_U32 CurrentFreq = 0, CurrPLLClock = 0;
    CBIOS_U32   remainder = 0;
    CBIOS_U32 HSSRemainder = 0;
    CBIOS_U32 HSyncRemainder = 0;
    CBIOS_U32 HBackPorchRemainder = 0;
    CBIOS_U32 HDisEndRemainder = 0;
    CBIOS_U32 HorTotalReg = 0;

    cb_memset(pTiming, 0, sizeof(CBIOS_TIMING_ATTRIB));
    cb_memset(pFlags, 0, sizeof(CBIOS_TIMING_FLAGS));

    if(IGAIndex == IGA1)
    {
        ClockType = CBIOS_DCLK1TYPE;
    }
    else if(IGAIndex == IGA2)
    {
        ClockType = CBIOS_DCLK2TYPE;
    }
    else if(IGAIndex == IGA3)
    {
        ClockType = CBIOS_DCLK3TYPE;
    }
    else if(IGAIndex == IGA4)
    {
        ClockType = CBIOS_DCLK4TYPE;
    }

    cbGetProgClock(pcbe, &CurrentFreq, ClockType);
    CurrPLLClock = CurrentFreq;

    //================================================================//
    //************Start Getting Horizontal Timing Registers***********//
    //================================================================//
    HSSRemainder = cbBiosMMIOReadReg(pcbe, CR_72, IGAIndex) & 0x07;
    temp = cbBiosMMIOReadReg(pcbe, CR_72, IGAIndex);
    HSyncRemainder = (temp >> 3) & 0x07;
    HBackPorchRemainder = cbBiosMMIOReadReg(pcbe, CR_73, IGAIndex) & 0x07;

    temp = cbBiosMMIOReadReg(pcbe, CR_58, IGAIndex);
    if(temp & BIT3)
    {
        HDisEndRemainder = (temp & 0x07) + 1;
    }

    pTiming->HorSyncStart = (CBIOS_U16)cbMapMaskRead(pcbe, HSS2Reg_INDEX, IGAIndex); //Horizontal Sync Start
    pTiming->HorSyncStart *= 8;
    pTiming->HorSyncStart += (CBIOS_U16)HSSRemainder;

    pTiming->HorTotal = (CBIOS_U16)cbMapMaskRead(pcbe, HT2Reg_INDEX, IGAIndex); //Horizontal Total
    HorTotalReg = pTiming->HorTotal;
    pTiming->HorTotal = (pTiming->HorTotal + 5) * 8 + (CBIOS_U16)(HSSRemainder + HSyncRemainder + HBackPorchRemainder);


    temp = cbMapMaskRead(pcbe, HSE2Reg_INDEX, IGAIndex); //Horizontal Sync End 
    pTiming->HorSyncEnd =  (temp & 0x1F) | ((pTiming->HorSyncStart/8) & ~(0x1F));
    if (temp & BIT5)
    {
        pTiming->HorSyncEnd += 0x20;
    }
    if (pTiming->HorSyncEnd <= (pTiming->HorSyncStart/8))
    {
        pTiming->HorSyncEnd += 0x20;
    }
    pTiming->HorSyncEnd *= 8;
    pTiming->HorSyncEnd += (CBIOS_U16)(HSyncRemainder + HSSRemainder);

    pTiming->HorDisEnd = (CBIOS_U16)cbMapMaskRead(pcbe, HDE2Reg_INDEX, IGAIndex); //Horizontal Display End
    //rounded up to the nearest integer 
    if(HDisEndRemainder)
    {
        pTiming->HorDisEnd = pTiming->HorDisEnd  * 8;
    }
    else
    {
        pTiming->HorDisEnd = (pTiming->HorDisEnd + 1) * 8;
    }
    pTiming->HorDisEnd += (CBIOS_U16)HDisEndRemainder;
    
    pTiming->HorBStart = (CBIOS_U16)cbMapMaskRead(pcbe, HBS2Reg_INDEX, IGAIndex); //Horizontal Blank Start
    pTiming->HorBStart *= 8;

    pTiming->HorBEnd = (CBIOS_U16)cbMapMaskRead(pcbe, HBE2Reg_INDEX, IGAIndex); //Horizontal Blank End
    if (pTiming->HorBEnd >= HorTotalReg + 3)
    {
        pTiming->HorBEnd += 0x02;
        pTiming->HorBEnd *= 8;
        pTiming->HorBEnd += (CBIOS_U16)(HSSRemainder + HSyncRemainder + HBackPorchRemainder);
    }
    else
    {
        pTiming->HorBEnd *= 8;
    }

    //================================================================//
    //************Start Getting Vertical Timing Registers*************//
    //================================================================//
    pTiming->VerTotal = (CBIOS_U16)cbMapMaskRead(pcbe, VT2Reg_INDEX, IGAIndex); //Vertical Total
    pTiming->VerTotal +=2;

    pTiming->VerDisEnd = (CBIOS_U16)cbMapMaskRead(pcbe, VDE2Reg_INDEX, IGAIndex); //Vertical Display End
    pTiming->VerDisEnd++;

    pTiming->VerBStart = (CBIOS_U16)cbMapMaskRead(pcbe, VBS2Reg_INDEX, IGAIndex); //Vertical Blank Start
    pTiming->VerBStart++;

    temp = cbMapMaskRead(pcbe, VBE2Reg_INDEX, IGAIndex); //Vertical Blank End
    temp++;
    pTiming->VerBEnd = (temp & 0xFF) | (pTiming->VerBStart & ~(0xFF));
    if (pTiming->VerBEnd <= pTiming->VerBStart )
    {
        pTiming->VerBEnd += 0x100;
    }
    if(pTiming->VerBEnd >= pTiming->VerTotal - 2)
    {
        pTiming->VerBEnd += 1;
    }

    pTiming->VerSyncStart = (CBIOS_U16)cbMapMaskRead(pcbe, VSS2Reg_INDEX, IGAIndex); //Vertical Sync Start
    pTiming->VerSyncStart++;

    temp = cbMapMaskRead(pcbe, VSE2Reg_INDEX, IGAIndex); //Vertical Sync End
    temp++;
    pTiming->VerSyncEnd = (temp & 0x0F) | (pTiming->VerSyncStart & ~(0x0F));
    if (pTiming->VerSyncEnd <= pTiming->VerSyncStart)
    {
        pTiming->VerSyncEnd += 0x10;
    }

    //calculate the result value
    pTiming->XRes = pTiming->HorDisEnd;
    pTiming->YRes = pTiming->VerDisEnd;

    temp = (CBIOS_U32)pTiming->HorTotal * (CBIOS_U32)pTiming->VerTotal;
    pTiming->RefreshRate = CurrPLLClock * 100 / temp;
    remainder = (CurrPLLClock * 100) % temp;
    pTiming->RefreshRate = pTiming->RefreshRate * 100 + remainder * 100 / temp;
}


CBIOS_VOID cbGetModeInfoFromReg_E3K(PCBIOS_EXTENSION_COMMON pcbe,
                                CBIOS_ACTIVE_TYPE ulDevice,
                                PCBIOS_TIMING_ATTRIB pTiming,
                                PCBIOS_TIMING_FLAGS pFlags,
                                CBIOS_U32  IGAIndex,
                                CBIOS_TIMING_REG_TYPE TimingRegType)
{
    REG_SR70_B    RegSR70_BValue;
    REG_MM33694   RegMM33694;

    if(TIMING_REG_TYPE_CR == TimingRegType)
    {
        cbGetCRTimingReg_E3K(pcbe, pTiming, IGAIndex, pFlags);
    }
    else if(TIMING_REG_TYPE_SR == TimingRegType)
    {
        cbGetSRTimingReg_E3K(pcbe, pTiming, IGAIndex, pFlags);
    }
    else
    {
        cbGetSRTimingReg_E3K(pcbe, pTiming, IGAIndex, pFlags);
    }

    // judge if timing mode is interlaced or not
    switch (ulDevice)
    {
    case CBIOS_TYPE_CRT:
    case CBIOS_TYPE_DVO:
        pFlags->IsInterlace = 0;
        break;

    case CBIOS_TYPE_HDTV:
        RegSR70_BValue.Value = 0;
        RegSR70_BValue.Value = cbMMIOReadReg(pcbe, SR_B_70);
        if (RegSR70_BValue.Progressive_Mode_Enable == 0)
        {
            pFlags->IsInterlace = 1;
        }
        else
        {
            pFlags->IsInterlace = 0;
        }
        break;
    case CBIOS_TYPE_DP1:
    case CBIOS_TYPE_DP2:
    case CBIOS_TYPE_DP3:
    case CBIOS_TYPE_DP4:
        RegMM33694.Value = cb_ReadU32(pcbe->pAdapterContext, HDTV_REG_LB[IGAIndex]);
        if (RegMM33694.LB1_BYPASS == 0) // HDTV source
        {
            RegSR70_BValue.Value = 0;
            RegSR70_BValue.Value = cbMMIOReadReg(pcbe, SR_B_70);
            if (0)//RegSR70_BValue.Progressive_Mode_Enable == 0) //can't find this define in e3k register spec, just comment interlace mode
            {
                pFlags->IsInterlace = 1;
            }
            else
            {
                pFlags->IsInterlace = 0;
            }
        }
        else
        {
            pFlags->IsInterlace = 0;
        }
        break;
    case CBIOS_TYPE_TV:
        pFlags->IsInterlace = 1;
        break;

    default:
        cbDebugPrint((MAKE_LEVEL(GENERIC, WARNING), "unknown device type!\n"));
        return; 
    }
}

CBIOS_STATUS  cbGetDispAddr_E3K(PCBIOS_EXTENSION_COMMON pcbe, PCBIOS_GET_DISP_ADDR  pGetDispAddr)
{
    CBIOS_STREAM_TP         StreamType = pGetDispAddr->StreamType;
    CBIOS_U32               IGAIndex = pGetDispAddr->IGAIndex;
    CBIOS_U64               AddrRegValue = 0;
    REG_MM81DC_E3K          PSStartAddrValue = {0};
    REG_MM81D0_E3K          SSStartAddrValue = {0};

    if(StreamType == CBIOS_STREAM_PS)
    {
        PSStartAddrValue.Value = cb_ReadU32(pcbe->pAdapterContext, PSStartAddrIndex[IGAIndex]);
        AddrRegValue = PSStartAddrValue.PS1_Start_Address;
    }
    else if(StreamType == CBIOS_STREAM_SS)
    {
        SSStartAddrValue.Value = cb_ReadU32(pcbe->pAdapterContext, SSStartAddrIndex[IGAIndex]);
        AddrRegValue = SSStartAddrValue.SS_Start_Address0;
    }
    else if(StreamType == CBIOS_STREAM_TS)
    {
        SSStartAddrValue.Value = cb_ReadU32(pcbe->pAdapterContext, TQSStartAddrIndex[0][IGAIndex]);
        AddrRegValue = SSStartAddrValue.SS_Start_Address0;
    }
    else
    {
        SSStartAddrValue.Value = cb_ReadU32(pcbe->pAdapterContext, TQSStartAddrIndex[1][IGAIndex]);
        AddrRegValue = SSStartAddrValue.SS_Start_Address0;
    }

    pGetDispAddr->DispAddr = AddrRegValue << 5;


    return  CBIOS_OK;
}

CBIOS_BOOL  cbUpdateOverlay_E3K(PCBIOS_EXTENSION_COMMON pcbe, 
                                     CBIOS_U32  IGAIndex,
                                     CBIOS_U32  OvlIndex,
                                     CBIOS_STREAM_TP  StreamType,
                                     PCBIOS_OVERLAY_INFO  pOverlayInfo,
                                     PCBIOS_FLIP_MODE  pFlipMode)
{
    REG_MM33670_E3K  BackgndValue = {0};
    REG_MM33840_E3K  OvlKeyValue = {0};
    REG_MM33844_E3K  OvlPlaneAlphaValue = {0};
    CBIOS_U32  KeyMode = 0;
    CBIOS_U8  Kp = 0, Ks = 0;

    if(OvlIndex == CBIOS_OVERLAY0)
    {
        BackgndValue.Backgnd_Color_Value = 0; // black background
        BackgndValue.Backgnd_Color_Ycbcr = 0;
    }

    if(pFlipMode->FlipType == CBIOS_PLANE_FLIP_WITH_DISABLE)
    {
        KeyMode = 0;
        Kp = 8;
        Ks = 0;
    }
    else
    {
        KeyMode = cbGetHwColorKey(pOverlayInfo, &Ks, &Kp);

        //invert alpha
        if((pOverlayInfo->KeyMode == CBIOS_CONSTANT_ALPHA && pOverlayInfo->ConstantAlphaBlending.bInvertAlpha) ||
            (pOverlayInfo->KeyMode == CBIOS_ALPHA_BLENDING && pOverlayInfo->AlphaBlending.bInvertAlpha))
        {
            OvlKeyValue.Invert_Alpha_Or_Ka = 1;
        }

        //pre-multi
        if(pOverlayInfo->KeyMode == CBIOS_ALPHA_BLENDING && pOverlayInfo->AlphaBlending.bPremulBlend)
        {
            OvlPlaneAlphaValue.Ovl0_Alpha_Blend_Mode = 1;
        }

        //plane alpha
        if(pOverlayInfo->KeyMode == CBIOS_ALPHA_BLENDING)
        {
            OvlPlaneAlphaValue.Ovl0_Plane_Alpha =
                pOverlayInfo->AlphaBlending.bUsePlaneAlpha ? pOverlayInfo->AlphaBlending.PlaneValue : 0xff;
        }
        if(pOverlayInfo->KeyMode == CBIOS_CONSTANT_ALPHA)
        {
            OvlPlaneAlphaValue.Ovl0_Plane_Alpha =
                pOverlayInfo->ConstantAlphaBlending.bUsePlaneAlpha ? pOverlayInfo->ConstantAlphaBlending.PlaneValue : 0xff;
        }

        //mix alpha
        if(pOverlayInfo->KeyMode == CBIOS_ALPHA_BLENDING)
        {
            OvlKeyValue.Alpha_Select = pOverlayInfo->AlphaBlending.AlphaMixType;
        }

        //color key select
        if(pOverlayInfo->KeyMode == CBIOS_COLOR_KEY)
        {
            OvlKeyValue.Color_Key_Sel = pOverlayInfo->ColorKey.ColorKeyType;
        }
    }

    OvlKeyValue.Ka_3to0_Or_Ks = Ks;
    OvlKeyValue.Ka_7to4_Or_Kp = Kp;
    OvlKeyValue.Key_Mode = KeyMode;
    OvlKeyValue.Ovl0_Input_Stream = StreamType;

    if(OvlIndex == CBIOS_OVERLAY0)
    {
        cbMMIOWriteReg32(pcbe, BackgndColorIndex[IGAIndex], BackgndValue.Value, 0); 
    }

    cbMMIOWriteReg32(pcbe, OvlKeyIndex[IGAIndex][OvlIndex], OvlKeyValue.Value, 0);
    cbMMIOWriteReg32(pcbe, OvlPlaneAlphaIndex[IGAIndex][OvlIndex], OvlPlaneAlphaValue.Value, 0);
        
    return  CBIOS_TRUE;
}


/*
 *  flip mode is set by the corresponding stream
 *  so flip mode is no need in the parameters' list
 */
CBIOS_BOOL  cbUpdateOVLKey_E3K(PCBIOS_EXTENSION_COMMON pcbe, 
                                     CBIOS_U32  IGAIndex,
                                     CBIOS_OVL_KEY_INFO *pInfo)
{
    REG_MM8188_E3K  PSKeyHighValue = {0};
    REG_MM81AC_E3K  PSKeyLowValue = {0};

    if  (CBIOS_OVL_KEY_TYPE_ALPHA == pInfo->Type)
    {
        PSKeyLowValue.PS_B_Cb_Low_Or_Alpha_Key = pInfo->AlphaKey;
        PSKeyHighValue.Value = 0;
    }
    else if (CBIOS_OVL_KEY_TYPE_COLOR == pInfo->Type)
    {
        PSKeyLowValue.Value = pInfo->ColorKey;
        PSKeyHighValue.Value = 0;
    }
    else if (CBIOS_OVL_KEY_TYPE_CHROME == pInfo->Type)
    {
        PSKeyLowValue.Value = pInfo->ChromeKey.LowColor;
        PSKeyHighValue.Value = pInfo->ChromeKey.UpColor;
    }
    else
    {
        return CBIOS_FALSE;
    }

    cbMMIOWriteReg32(pcbe, KeyHighIndex[IGAIndex][pInfo->Index], PSKeyHighValue.Value, 0);
    cbMMIOWriteReg32(pcbe, KeyLowIndex[IGAIndex][pInfo->Index], PSKeyLowValue.Value, 0);

    return CBIOS_TRUE;
}


CBIOS_BOOL cbUpdatePrimaryStream_E3K(PCBIOS_EXTENSION_COMMON pcbe, 
                                     CBIOS_U32 IGAIndex,
                                     PCBIOS_STREAM_PARA pStreamPara,
                                     PCBIOS_FLIP_MODE  pFlipMode)
{
    PCBIOS_SURFACE_ATTRIB    pSurfaceAttrib = CBIOS_NULL; 
    PCBIOS_DISP_MODE_PARAMS  pModeParams = pcbe->DispMgr.pModeParams[IGAIndex];
    CBIOS_U64                ullStartAddress = 0;
    CBIOS_U32                dwBitCnt = 0, dwFetchCount = 0, DacColorMode = 0;
    CBIOS_U32               bWindowMode = CBIOS_FALSE;
    PCBIOS_WINDOW_PARA   pSrcWin = CBIOS_NULL, pDstWin = CBIOS_NULL;
    CBIOS_STREAM_ATTRIBUTE   StreamAttr = {0};
    REG_MM81A4_E3K  PSCompFifoValue = {0};
    REG_MM81DC_E3K  PSStartAddrValue = {0};
    REG_MM33700_E3K  PSBaseOffsetValue = {0};
    REG_MM81C4_E3K  PSVsyncOff = {0};
    REG_MM81E0_E3K  PSRightBaseValue = {0};
    REG_MM81C8_E3K  PSStrideValue = {0};
    REG_MM81FC_E3K  PSShadowValue = {0};
    REG_MM81B8_E3K  PSDstWindowPosValue = {0};
    REG_MM8208_E3K  PSDstWindowSizeValue = {0};
    REG_MM33320_E3K PSSrcWindowSizeValue = {0};
    REG_MM33318_E3K PSSCLRatioHValue = {0};
    REG_MM3331C_E3K PSSCLRatioVValue = {0};
    REG_MM33644_E3K  PSCscValue = {0};
    REG_MM33810_E3K PS1BLIIndexValue = {0};
    REG_MM33E6C_E3K PS2BLIIndexValue = {0};
    REG_MM8260_CX4  VsyncOffBackdoor = {0};
    REG_MM33318_CNE001 PSSCLRatioHValue_cne001 = {0};
    REG_MM3331C_CNE001 PSSCLRatioVValue_cne001 = {0};
    

    //use mmio
    cbBiosMMIOWriteReg(pcbe, CR_67, 0x08, ~0x08, IGAIndex);
    cbBiosMMIOWriteReg(pcbe, CR_64, 0x40, ~0x40, IGAIndex);

    if(pFlipMode->FlipType == CBIOS_PLANE_FLIP_WITH_DISABLE)
    {
        ullStartAddress = STREAM_DISABLE_FAKE_ADDR;
    }
    else
    {
        pSurfaceAttrib = &pStreamPara->SurfaceAttrib;
        pSrcWin = &pStreamPara->SrcWindow;
        pDstWin = &pStreamPara->DispWindow;
        
        StreamAttr.pSurfaceAttr = pSurfaceAttrib;
        StreamAttr.pSrcWinPara  = pSrcWin;
        StreamAttr.bVsyncOff = (pFlipMode->FlipImme)? 1 : 0;
        cbGetStreamAttribute(pcbe, &StreamAttr);
        ullStartAddress = pSurfaceAttrib->StartAddr;
    }

    PSStartAddrValue.PS1_Start_Address = (CBIOS_U32)(ullStartAddress >> 5);

    if(pFlipMode->FlipType == CBIOS_PLANE_FLIP_WITH_DISABLE)
    {
        PSShadowValue.PS_Disable = 1;
        PSBaseOffsetValue.Base_Offset = 0;
        cbMMIOWriteReg32(pcbe, PSBaseOffsetIndex[IGAIndex], PSBaseOffsetValue.Value, 0);
        cbMMIOWriteReg32(pcbe, PSStartAddrIndex[IGAIndex], PSStartAddrValue.Value, 0);
        cbMMIOWriteReg32(pcbe, PSShadowIndex[IGAIndex], PSShadowValue.Value, 0);
        return  CBIOS_TRUE;
    }

    PSBaseOffsetValue.Base_Offset = StreamAttr.dwBaseOffset >> 5;
    PSStrideValue.PS_Pixel_Offset = StreamAttr.PixelOffset;
    PSStrideValue.PS_Pixel_Offset_Bit4 = StreamAttr.PixelOffset >> 4;
    PSStrideValue.PS_Stride = StreamAttr.dwStride;

    if(pSurfaceAttrib->SurfaceFmt & (CBIOS_FMT_A2B10G10R10 | CBIOS_FMT_X2B10G10R10 |
                                     CBIOS_FMT_A8B8G8R8 | CBIOS_FMT_X8B8G8R8))
    {
        PSStrideValue.PS_Abgr_En = 1; //DX10 ABGR format enabled for PS1
    }
    if(pSurfaceAttrib->SurfaceFmt & (CBIOS_FMT_YCRYCB422_16BIT | CBIOS_FMT_YCRYCB422_32BIT))
    {
        PSStrideValue.PS_Uyvy422 = 1;
    }
    if(pSurfaceAttrib->SurfaceFmt & (CBIOS_FMT_YCBCR8888_32BIT | CBIOS_FMT_YCBCR2101010_32BIT))
    {
        PSStrideValue.PS_Ycbcr_Mode = 1;
    }

    if(pSurfaceAttrib->bCompress)
    {
        PSCompFifoValue.PS_Compress_Type = pSurfaceAttrib->Range_Type;
        if (0 == IGAIndex)
        {
            PS1BLIIndexValue.PS1_BL_IDX = pSurfaceAttrib->BLIndex;
        }
        else
        {
            PS2BLIIndexValue.PS2_BL_IDX = pSurfaceAttrib->BLIndex;
        }
        PSShadowValue.PS_Compress_Enable = 1;
    }
    PSCompFifoValue.PS_Fifo_Depth = 0; //256
    if(pSrcWin->WinSize == pDstWin->WinSize)
    {
        PSCompFifoValue.PS_Share_Scl_Fifo = 1;
    }

    if(pcbe->ChipID >= CHIPID_CHX004)
    {
        bWindowMode = CBIOS_TRUE;       //default set ps window mode to enable for chx004
    }
    PSDstWindowPosValue.PS_Window_X_Position = (pDstWin->Position & 0xFFFF) + 1;
    PSDstWindowPosValue.PS_Window_Y_Position = ((pDstWin->Position >> 16) & 0xFFFF) + 1;
    PSDstWindowSizeValue.PS_Window_Width = pDstWin->WinSize & 0xFFFF;
    PSDstWindowSizeValue.PS_Window_Height = (pDstWin->WinSize >> 16) & 0xFFFF;
    PSDstWindowSizeValue.PS_Window_Enable = bWindowMode ? 1 : 0;

    PSSrcWindowSizeValue.PS_Scl_Src_Width = pSrcWin->WinSize & 0xFFFF;
    PSSrcWindowSizeValue.PS_Scl_Src_Height = (pSrcWin->WinSize >> 16) & 0xFFFF;

    if(pcbe->ChipID == CHIPID_CHX004)
    {
        if((pSrcWin->WinSize & 0xFFFF) == (pDstWin->WinSize & 0xFFFF))
        {
            PSSCLRatioHValue.PS_Scl_Width_Ratio = 0x100000;
        }
        else
        {
            PSSCLRatioHValue.PS_Scl_Width_Ratio = ((1048576 * (pSrcWin->WinSize & 0xFFFF)) /(pDstWin->WinSize & 0xFFFF)) & 0x1FFFFF;
        }
        if((pSrcWin->WinSize & 0xFFFF0000) == (pDstWin->WinSize & 0xFFFF0000))
        {
            PSSCLRatioVValue.PS_Scl_Height_Ratio = 0x100000;
        }
        else
        {
            PSSCLRatioVValue.PS_Scl_Height_Ratio = ((1048576 * ((pSrcWin->WinSize >> 16) & 0xFFFF)) /((pDstWin->WinSize >> 16) & 0xFFFF)) & 0x1FFFFF;
        }
    }
    else if(pcbe->ChipID == CHIPID_CNE001)
    {
        if((pSrcWin->WinSize & 0xFFFF) == (pDstWin->WinSize & 0xFFFF))
        {
            PSSCLRatioHValue_cne001.PS_Scl_Width_Ratio = 0x1000000;
        }
        else
        {
            PSSCLRatioHValue_cne001.PS_Scl_Width_Ratio = ((16777216 * (CBIOS_U64)(pSrcWin->WinSize & 0xFFFF)) /(CBIOS_U64)(pDstWin->WinSize & 0xFFFF)) & 0x1FFFFFF;
        }
        if((pSrcWin->WinSize & 0xFFFF0000) == (pDstWin->WinSize & 0xFFFF0000))
        {
            PSSCLRatioVValue_cne001.PS_Scl_Height_Ratio = 0x1000000;
        }
        else
        {
            PSSCLRatioVValue_cne001.PS_Scl_Height_Ratio = ((16777216 * (CBIOS_U64)((pSrcWin->WinSize >> 16) & 0xFFFF)) /(CBIOS_U64)((pDstWin->WinSize >> 16) & 0xFFFF)) & 0x1FFFFFF;
        }
    }

    DacColorMode = cbGetHwDacMode(pcbe, pSurfaceAttrib->SurfaceFmt);

    if(pSurfaceAttrib->SurfaceFmt == CBIOS_FMT_P8)
    {
        dwBitCnt = 8;
    }
    else if(pSurfaceAttrib->SurfaceFmt & CBIOS_STREAM_16BPP_FMTS)
    {
        dwBitCnt = 16;
    }
    else
    {
        dwBitCnt = 32;
    }

    //if MM8208[31] = 1, PS Window Mode enable, dwFetchCount = (pStreamPara->SrcWindow.WinSize & 0xFFFF) * dwBitCnt
    //else MM8208[31] = 0, PS Window Mode disable, dwFetchCount = (pModeParams->SrcModePara.XRes & 0xFFFF) * dwBitCnt
    if(bWindowMode)
    {
        dwFetchCount = (pStreamPara->SrcWindow.WinSize & 0xFFFF) * dwBitCnt;
    }
    else
    {
        dwFetchCount = (pModeParams->SrcModePara.XRes & 0xFFFF) * dwBitCnt;
    }
    if(pSurfaceAttrib->b3DMode && pModeParams->Video3DStruct == SIDE_BY_SIDE_HALF)
    {
        dwFetchCount /= 2;
    }
    dwFetchCount = (dwFetchCount + 255) / 256;

    PSShadowValue.DAC_Color_Mode = DacColorMode;
    PSShadowValue.PS_L1_10to0 = dwFetchCount & 0x7ff;
    PSShadowValue.PS_L1_Bit11 = (dwFetchCount >> 11) & 0x1;
    PSShadowValue.Enable_PS_L1 = 1;
    PSShadowValue.PS_Read_Length = 2; 
    PSShadowValue.PS_Disable = 0;
    if(pSurfaceAttrib->b3DMode)
    {
        PSShadowValue.PS_3D_Video_Mode = cbGetHw3DVideoMode(pSurfaceAttrib->Surface3DPara.Video3DStruct);
    }

    if(pSurfaceAttrib->SurfaceFmt &  CBIOS_STREAM_FMT_YUV_SPACE)
    {
        if(pModeParams->TargetModePara.YRes >= 720)
        {
            PSCscValue.PS_Data_In_Fmt = CSC_FMT_YCBCR709;
        }
        else
        {
            PSCscValue.PS_Data_In_Fmt = CSC_FMT_YCBCR601;
        }
    }
    else
    {
        PSCscValue.PS_Data_In_Fmt = CSC_FMT_RGB;
    }
    PSCscValue.PS_Data_Out_Fmt = CSC_FMT_RGB;

    if(pSurfaceAttrib->b3DMode)
    {
        PSRightBaseValue.PS_Right_Frame_Base = ((pSurfaceAttrib->Surface3DPara.RightFrameOffset & ~0xf) >> 4);
    }

    if(pFlipMode->FlipImme)
    {
        PSVsyncOff.VsyncOff = 1;
        VsyncOffBackdoor.ps_vsync_off_backdoor = 1;
    }

    cbMMIOWriteReg32(pcbe, VsyncOffBackdoorIndex[IGAIndex], VsyncOffBackdoor.Value, 0);
    cbMMIOWriteReg32(pcbe, PSVsyncOffIndex[IGAIndex], PSVsyncOff.Value, 0);
    
    cbMMIOWriteReg32(pcbe, PSCompFifoIndex[IGAIndex], PSCompFifoValue.Value, 0);
    cbMMIOWriteReg32(pcbe, PSStartAddrIndex[IGAIndex], PSStartAddrValue.Value, 0);
    cbMMIOWriteReg32(pcbe, PSBaseOffsetIndex[IGAIndex], PSBaseOffsetValue.Value, 0);
    cbMMIOWriteReg32(pcbe, PSRightBaseIndex[IGAIndex], PSRightBaseValue.Value, 0);
    cbMMIOWriteReg32(pcbe, PSStrideIndex[IGAIndex], PSStrideValue.Value, 0);
    if(bWindowMode)
    {
        cbMMIOWriteReg32(pcbe, PSDstWinPosIndex[IGAIndex], PSDstWindowPosValue.Value, 0);
        cbMMIOWriteReg32(pcbe, PSDstWinSizeIndex[IGAIndex], PSDstWindowSizeValue.Value, 0);
        cbMMIOWriteReg32(pcbe, PSScalSrcIndex[IGAIndex], PSSrcWindowSizeValue.Value, 0);
        if(pcbe->ChipID == CHIPID_CHX004)
        {
            cbMMIOWriteReg32(pcbe, PSScalRatWIndex[IGAIndex], PSSCLRatioHValue.Value, 0);
            cbMMIOWriteReg32(pcbe, PSScalRatHIndex[IGAIndex], PSSCLRatioVValue.Value, 0);
        }
        else if(pcbe->ChipID == CHIPID_CNE001)
        {
            cbMMIOWriteReg32(pcbe, PSScalRatWIndex[IGAIndex], PSSCLRatioHValue_cne001.Value, 0);
            cbMMIOWriteReg32(pcbe, PSScalRatHIndex[IGAIndex], PSSCLRatioVValue_cne001.Value, 0);
        }
    }

    cbMMIOWriteReg32(pcbe, PSShadowIndex[IGAIndex], PSShadowValue.Value, 0);

    cbMMIOWriteReg32(pcbe, PSCscIndex[IGAIndex], PSCscValue.Value, 0);

    if (0 == IGAIndex)
    {
        cbMMIOWriteReg32(pcbe, PSBLIndex[IGAIndex], PS1BLIIndexValue.Value, 0);
    }
    else
    {
        cbMMIOWriteReg32(pcbe, PSBLIndex[IGAIndex], PS2BLIIndexValue.Value, 0);
    }

    //config stream threhold
    if(pcbe->ChipID == CHIPID_CNE001)
    {
        if((pModeParams->SrcModePara.XRes <= 2560) && (pModeParams->SrcModePara.YRes <= 1600) && (PSCompFifoValue.PS_Share_Scl_Fifo == 1))
        {
           // 0x8454 [0-7]:PS low threshold,[8-15]:PS high threshold,[16-23]:SS low threshold, [24-31]:SS high threshold
            cbMMIOWriteReg32(pcbe, StreamThresholdIndex[IGAIndex][CBIOS_STREAM_PS],0x0, 0xFFFF0000);  
        }
        else
        {
            cbMMIOWriteReg32(pcbe, StreamThresholdIndex[IGAIndex][CBIOS_STREAM_PS],0xa0a0, 0xFFFF0000);

        }
    }
    return CBIOS_TRUE;
}

CBIOS_BOOL cbUpdateSecondStream_E3K(PCBIOS_EXTENSION_COMMON pcbe, 
                                    CBIOS_U32 IGAIndex, 
                                    PCBIOS_STREAM_PARA pStreamPara,   
                                    PCBIOS_FLIP_MODE pFlipMode)
{
    PCBIOS_SURFACE_ATTRIB     pSurfaceAttrib = CBIOS_NULL;
    PCBIOS_DISP_MODE_PARAMS   pModeParams = pcbe->DispMgr.pModeParams[IGAIndex];
    PCBIOS_WINDOW_PARA   pSrcWin = CBIOS_NULL, pDstWin = CBIOS_NULL;
    CBIOS_U64                ullStartAddress = 0;
    CBIOS_STREAM_ATTRIBUTE    StreamAttr = {0};
    REG_MM8198_E3K    SSCompFifoValue = {0};
    REG_MM81D0_E3K    SSStartAddrValue = {0};
    REG_MM33708_E3K    SSBaseOffsetValue = {0};
    REG_MM33834_E3K    SSEnableValue = {0};
    REG_MM81D4_E3K    SSRightBaseValue = {0};
    REG_MM81D8_E3K    SSStrideValue = {0};
    REG_MM8190_E3K    SSFmtSrcWValue = {0};
    REG_MM819C_E3K    SSDstWinValue = {0};
    REG_MM81A8_E3K    SSSrcWinHValue = {0};
    REG_MM81F8_E3K    SSDstWinPosValue = {0};
    REG_MM332E0_E3K    SSScalRatioHValue = {0};
    REG_MM332E4_E3K    SSScalRatioVValue = {0};
    REG_MM3362C_E3K    SSCscValue = {0};
    REG_MM33854_E3K    SSBLIndexValue = {0};
    REG_MM8260_CX4    VsyncOffBackdoor = {0};
    REG_MM332E0_CNE001    SSScalRatioHValue_cne001 = {0};
    REG_MM332E4_CNE001    SSScalRatioVValue_cne001 = {0};

    if(pFlipMode->FlipType == CBIOS_PLANE_FLIP_WITH_DISABLE)
    {
        ullStartAddress = STREAM_DISABLE_FAKE_ADDR;
    }
    else
    {
        pSurfaceAttrib = &pStreamPara->SurfaceAttrib;
        pSrcWin = &pStreamPara->SrcWindow;
        pDstWin = &pStreamPara->DispWindow;
        
        StreamAttr.pSurfaceAttr = pSurfaceAttrib;
        StreamAttr.pSrcWinPara  = pSrcWin;
        StreamAttr.bVsyncOff = (pFlipMode->FlipImme)? 1 : 0;
        cbGetStreamAttribute(pcbe, &StreamAttr);
        ullStartAddress = pSurfaceAttrib->StartAddr;
    }

    SSStartAddrValue.SS_Start_Address0 = (CBIOS_U32)(ullStartAddress >> 5);
    
    SSEnableValue.SS_Use_Mmio_En = 1;
    if(pFlipMode->FlipType == CBIOS_PLANE_FLIP_WITH_DISABLE)
    {
        SSEnableValue.SS_Enable = 0;
        SSBaseOffsetValue.SS_Base_Offset = 0;
        cbMMIOWriteReg32(pcbe, SSBaseOffsetIndex[IGAIndex], SSBaseOffsetValue.Value, 0);
        cbMMIOWriteReg32(pcbe, SSStartAddrIndex[IGAIndex], SSStartAddrValue.Value, 0);
        cbMMIOWriteReg32(pcbe, SSEnableIndex[IGAIndex], SSEnableValue.Value, 0);
        return  CBIOS_TRUE;
    }

    SSEnableValue.SS_Enable = 1;

    SSBaseOffsetValue.SS_Base_Offset = StreamAttr.dwBaseOffset >> 5;
    SSStrideValue.SS_Stride = StreamAttr.dwStride;
    SSStrideValue.SS_Pixel_Offset = StreamAttr.PixelOffset;
    SSStrideValue.SS_Read_Length = 3; //256 bits
    if(pSurfaceAttrib->SurfaceFmt & (CBIOS_FMT_A2B10G10R10 | CBIOS_FMT_X2B10G10R10 |
                                     CBIOS_FMT_A8B8G8R8 | CBIOS_FMT_X8B8G8R8))
    {
        SSStrideValue.SS_Abgr_En = 1; //DX10 ABGR format enabled for PS1
    }

    if(pSurfaceAttrib->bCompress)
    {
        SSCompFifoValue.SS_Comp_Mode_Enable = 1;
        SSCompFifoValue.SS_Compress_Type = pSurfaceAttrib->Range_Type;
        SSBLIndexValue.SS1_burst_index = pSurfaceAttrib->BLIndex;
    }
    SSCompFifoValue.SS_Fifo_Depth_Control = 0; //256
    if(pSrcWin->WinSize == pDstWin->WinSize)
    {
        SSCompFifoValue.SS_Share_Scaler_Fifo = 1;
    }

    if(pSurfaceAttrib->SurfaceFmt & (CBIOS_FMT_YCRYCB422_16BIT | CBIOS_FMT_YCRYCB422_32BIT))
    {
        SSFmtSrcWValue.SS_Uyvy422 = 1;
    }
    if(pSurfaceAttrib->SurfaceFmt & (CBIOS_FMT_YCBCR8888_32BIT | CBIOS_FMT_YCBCR2101010_32BIT))
    {
        SSFmtSrcWValue.SS_YCbCr_Mode = 1;
    }
    SSFmtSrcWValue.SS_Input_Format = cbGetHwDacMode(pcbe, pSurfaceAttrib->SurfaceFmt);
    SSFmtSrcWValue.SS_YCbCr_Mode = 0; //?
    SSFmtSrcWValue.SS_Src_Line_Width = pSrcWin->WinSize & 0xFFFF;

    SSSrcWinHValue.SS_Line_Height = (pSrcWin->WinSize >> 16) & 0xFFFF;

    SSDstWinValue.SS_Dest_Width = pDstWin->WinSize & 0xFFFF;
    SSDstWinValue.SS_Dest_Height = (pDstWin->WinSize >> 16) & 0xFFFF;

    SSDstWinPosValue.SS_X_Start = (pDstWin->Position & 0xFFFF) + 1;
    SSDstWinPosValue.SS_Y_Start = ((pDstWin->Position >> 16) & 0xFFFF) + 1;

    if(pcbe->ChipID == CHIPID_CHX004)
    {
        if((pSrcWin->WinSize & 0xFFFF) == (pDstWin->WinSize & 0xFFFF))
        {
            SSScalRatioHValue.SS_Hacc = 0x100000;
        }
        else
        {
            SSScalRatioHValue.SS_Hacc = ((1048576 * (pSrcWin->WinSize & 0xFFFF)) /(pDstWin->WinSize & 0xFFFF)) & 0x1FFFFF;
        }
        if((pSrcWin->WinSize & 0xFFFF0000) == (pDstWin->WinSize & 0xFFFF0000))
        {
            SSScalRatioVValue.SS_Vacc = 0x100000;
        }
        else
        {
            SSScalRatioVValue.SS_Vacc = ((1048576 * ((pSrcWin->WinSize >> 16) & 0xFFFF)) /((pDstWin->WinSize >> 16) & 0xFFFF)) & 0x1FFFFF;
        }
    }
    else if(pcbe->ChipID == CHIPID_CNE001)
    {
        if((pSrcWin->WinSize & 0xFFFF) == (pDstWin->WinSize & 0xFFFF))
        {
            SSScalRatioHValue_cne001.SS_Hacc = 0x1000000;
        }
        else
        {
            SSScalRatioHValue_cne001.SS_Hacc = ((16777216 * (CBIOS_U64)(pSrcWin->WinSize & 0xFFFF)) /(CBIOS_U64)(pDstWin->WinSize & 0xFFFF)) & 0x1FFFFFF;
        }
        if((pSrcWin->WinSize & 0xFFFF0000) == (pDstWin->WinSize & 0xFFFF0000))
        {
            SSScalRatioVValue_cne001.SS_Vacc = 0x1000000;
        }
        else
        {
            SSScalRatioVValue_cne001.SS_Vacc = ((16777216 * (CBIOS_U64)((pSrcWin->WinSize >> 16) & 0xFFFF)) /(CBIOS_U64)((pDstWin->WinSize >> 16) & 0xFFFF)) & 0x1FFFFFF;
        }
    }

    if(pSurfaceAttrib->SurfaceFmt &  CBIOS_STREAM_FMT_YUV_SPACE)
    {
        if(pModeParams->TargetModePara.YRes >= 720)
        {
            SSCscValue.SS_Data_In_Format = CSC_FMT_YCBCR709;
        }
        else
        {
            SSCscValue.SS_Data_In_Format = CSC_FMT_YCBCR601;
        }
    }
    else
    {
        SSCscValue.SS_Data_In_Format = CSC_FMT_RGB;
    }
    SSCscValue.SS_Data_Out_Format = CSC_FMT_RGB;

    if(pSurfaceAttrib->b3DMode)
    {
        SSRightBaseValue.SS_Roffset = pSurfaceAttrib->Surface3DPara.RightFrameOffset;
    }

    if(pFlipMode->FlipImme)
    {
        SSRightBaseValue.SS_Vsync_Off_Flip = 1;
        VsyncOffBackdoor.ss_vsync_off_backdoor = 1;
    }

    cbMMIOWriteReg32(pcbe, VsyncOffBackdoorIndex[IGAIndex], VsyncOffBackdoor.Value, 0);
    cbMMIOWriteReg32(pcbe, SSRightBaseIndex[IGAIndex], SSRightBaseValue.Value, 0);

    cbMMIOWriteReg32(pcbe, SSStartAddrIndex[IGAIndex], SSStartAddrValue.Value, 0);
    cbMMIOWriteReg32(pcbe, SSBaseOffsetIndex[IGAIndex], SSBaseOffsetValue.Value, 0);
    cbMMIOWriteReg32(pcbe, SSStrideIndex[IGAIndex], SSStrideValue.Value, 0);
    cbMMIOWriteReg32(pcbe, SSCompFifoIndex[IGAIndex], SSCompFifoValue.Value, 0);
    cbMMIOWriteReg32(pcbe, SSFmtSrcWIndex[IGAIndex], SSFmtSrcWValue.Value, 0);
    
    cbMMIOWriteReg32(pcbe, SSDstWinIndex[IGAIndex], SSDstWinValue.Value, 0);
    cbMMIOWriteReg32(pcbe, SSDstWinPosIndex[IGAIndex], SSDstWinPosValue.Value, 0);
    cbMMIOWriteReg32(pcbe, SSSrcWinHIndex[IGAIndex], SSSrcWinHValue.Value, 0);
    if(pcbe->ChipID == CHIPID_CHX004)
    {
        cbMMIOWriteReg32(pcbe, SSScalRatioHIndex[IGAIndex], SSScalRatioHValue.Value, 0);
        cbMMIOWriteReg32(pcbe, SSScalRatioVIndex[IGAIndex], SSScalRatioVValue.Value, 0);
    }
    else if(pcbe->ChipID == CHIPID_CNE001)
    {
        cbMMIOWriteReg32(pcbe, SSScalRatioHIndex[IGAIndex], SSScalRatioHValue_cne001.Value, 0);
        cbMMIOWriteReg32(pcbe, SSScalRatioVIndex[IGAIndex], SSScalRatioVValue_cne001.Value, 0);
    }
    cbMMIOWriteReg32(pcbe, SSCscIndex[IGAIndex], SSCscValue.Value, 0);
    cbMMIOWriteReg32(pcbe, SSEnableIndex[IGAIndex], SSEnableValue.Value, 0);
    cbMMIOWriteReg32(pcbe, SSBLIndex[IGAIndex], SSBLIndexValue.Value, 0);

    //config stream threshold
    if(pcbe->ChipID == CHIPID_CNE001)
    {
        if((pModeParams->SrcModePara.XRes <= 2560) && (pModeParams->SrcModePara.YRes <= 1600) && (SSCompFifoValue.SS_Share_Scaler_Fifo == 1))
        {
           // 0x8454 [0-7]:PS low threshold,[8-15]:PS high threshold,[16-23]:SS low threshold, [24-31]:SS high threshold
           // SS use the same register as PS 
            cbMMIOWriteReg32(pcbe, StreamThresholdIndex[IGAIndex][CBIOS_STREAM_SS],0x0, 0x0000FFFF);  
        }
        else
        {
            cbMMIOWriteReg32(pcbe, StreamThresholdIndex[IGAIndex][CBIOS_STREAM_SS],0xa0a00000, 0x0000FFFF); 
         
        }
    }
    
    return  CBIOS_TRUE;
}

CBIOS_BOOL cbUpdateThdFourStream_E3K(PCBIOS_EXTENSION_COMMON pcbe, 
                                    CBIOS_U32 IGAIndex, 
                                    CBIOS_STREAM_TP  StreamIndex,
                                    PCBIOS_STREAM_PARA pStreamPara,   
                                    PCBIOS_FLIP_MODE pFlipMode)
{
    PCBIOS_SURFACE_ATTRIB     pSurfaceAttrib = CBIOS_NULL;
    PCBIOS_DISP_MODE_PARAMS   pModeParams = pcbe->DispMgr.pModeParams[IGAIndex];
    PCBIOS_WINDOW_PARA   pSrcWin = CBIOS_NULL, pDstWin = CBIOS_NULL;
    CBIOS_U64                 ullStartAddress = 0;
    CBIOS_U32            dwIndex = StreamIndex - CBIOS_STREAM_TS;
    CBIOS_STREAM_ATTRIBUTE    StreamAttr = {0};
    REG_MM8430_E3K  TQSStartAddrValue = {0};
    REG_MM8414_E3K  TQSCompFifoValue = {0};
    REG_MM8498_E3K  TQSBaseOffsetValue = {0};
    REG_MM8458_E3K  TQSEnableValue = {0};
    REG_MM844C_E3K  TQSRightBaseValue = {0};
    REG_MM8438_E3K  TQSStrideValue = {0};
    REG_MM33600_E3K  TQSFormatValue = {0};
    REG_MM8444_E3K  TQSDstWinPosValue = {0};
    REG_MM8450_E3K  TQSDstWinValue = {0};
    REG_MM8448_E3K  TQSSrcWinValue = {0};
    REG_MM84A4_E3K  TQSScalRatioHValue = {0};
    REG_MM84A8_E3K  TQSScalRatioVValue = {0};
    REG_MM33614_E3K  TQSCscValue = {0};
    REG_MM33858_E3K TQSBLIndexValue = {0};
    REG_MM8260_CX4  VsyncOffBackdoor = {0};
    REG_MM84A4_CNE001  TQSScalRatioHValue_cne001 = {0};
    REG_MM84A8_CNE001  TQSScalRatioVValue_cne001 = {0};

    if(StreamIndex != CBIOS_STREAM_TS && StreamIndex != CBIOS_STREAM_FS)
    {
        return  CBIOS_FALSE;
    }

    //TS and QS can only use mmio

    if(pFlipMode->FlipType == CBIOS_PLANE_FLIP_WITH_DISABLE)
    {
        ullStartAddress = STREAM_DISABLE_FAKE_ADDR;
    }
    else
    {
        pSurfaceAttrib = &pStreamPara->SurfaceAttrib;
        pSrcWin = &pStreamPara->SrcWindow;
        pDstWin = &pStreamPara->DispWindow;
        
        StreamAttr.pSurfaceAttr = pSurfaceAttrib;
        StreamAttr.pSrcWinPara  = pSrcWin;
        StreamAttr.bVsyncOff = (pFlipMode->FlipImme)? 1 : 0;
        cbGetStreamAttribute(pcbe, &StreamAttr);
        ullStartAddress = pSurfaceAttrib->StartAddr;
    }

    TQSStartAddrValue.TS_FB_Start_Address_0 = (CBIOS_U32)(ullStartAddress >> 5);
    
    if(pFlipMode->FlipType == CBIOS_PLANE_FLIP_WITH_DISABLE)
    {
        TQSEnableValue.TS_En = 0;
        TQSBaseOffsetValue.TS_Win1_Base_Offset = 0;
        cbMMIOWriteReg32(pcbe, TQSBaseOffsetIndex[dwIndex][IGAIndex], TQSBaseOffsetValue.Value, 0);
        cbMMIOWriteReg32(pcbe, TQSStartAddrIndex[dwIndex][IGAIndex], TQSStartAddrValue.Value, 0);
        cbMMIOWriteReg32(pcbe, TQSEnableIndex[dwIndex][IGAIndex], TQSEnableValue.Value, 0);
        return  CBIOS_TRUE;
    }

    TQSEnableValue.TS_En = 1;

    TQSBaseOffsetValue.TS_Win1_Base_Offset = StreamAttr.dwBaseOffset >> 5;
    TQSStrideValue.TS_Stride = StreamAttr.dwStride;
    TQSStrideValue.TS_Start_Address_Byte_Offset = StreamAttr.PixelOffset;
    TQSStrideValue.TS_Read_Length = 3; //256 bits
    if(pSurfaceAttrib->SurfaceFmt & (CBIOS_FMT_A2B10G10R10 | CBIOS_FMT_X2B10G10R10 |
                                     CBIOS_FMT_A8B8G8R8 | CBIOS_FMT_X8B8G8R8))
    {
        TQSStrideValue.TS_Abgr_En = 1; //DX10 ABGR format enabled for PS1
    }

    if(pSurfaceAttrib->bCompress)
    {
        TQSCompFifoValue.TS_Comp_Enable = 1;
        TQSCompFifoValue.TS_Comp_Type = pSurfaceAttrib->Range_Type;
        TQSBLIndexValue.TS_win1_burst_index = pSurfaceAttrib->BLIndex; 
    }
    TQSCompFifoValue.TS_Fifo_Depth = 0; //256
    if(pSrcWin->WinSize == pDstWin->WinSize)
    {
        TQSCompFifoValue.TS_Share_Scaler_Fifo = 1;
    }

    if(pSurfaceAttrib->SurfaceFmt & (CBIOS_FMT_YCRYCB422_16BIT | CBIOS_FMT_YCRYCB422_32BIT))
    {
        TQSFormatValue.TS_YCbCr_Order = 1;
    }
    if(pSurfaceAttrib->SurfaceFmt & (CBIOS_FMT_YCBCR8888_32BIT | CBIOS_FMT_YCBCR2101010_32BIT))
    {
        TQSFormatValue.TS_444_YCbCr_Order = 1;
    }
    TQSFormatValue.TS_Win1_Format = cbGetHwDacMode(pcbe, pSurfaceAttrib->SurfaceFmt);

    TQSSrcWinValue.TS_Win1_Src_W = pSrcWin->WinSize & 0xFFFF;

    TQSSrcWinValue.TS_Win1_Src_H = (pSrcWin->WinSize >> 16) & 0xFFFF;

    TQSDstWinValue.TS_Win1_Dst_W = pDstWin->WinSize & 0xFFFF;
    TQSDstWinValue.TS_Win1_Dst_H = (pDstWin->WinSize >> 16) & 0xFFFF;

    TQSDstWinPosValue.TS_X_Start = (pDstWin->Position & 0xFFFF) + 1;
    TQSDstWinPosValue.TS_Y_Start = ((pDstWin->Position >> 16) & 0xFFFF) + 1;

    if(pcbe->ChipID == CHIPID_CHX004)
    {
        if((pSrcWin->WinSize & 0xFFFF) == (pDstWin->WinSize & 0xFFFF))
        {
            TQSScalRatioHValue.TS_Win1_Hacc = 0x100000;
        }
        else
        {
            TQSScalRatioHValue.TS_Win1_Hacc = ((1048576 * (pSrcWin->WinSize & 0xFFFF)) /(pDstWin->WinSize & 0xFFFF)) & 0x1FFFFF;
        }
        if((pSrcWin->WinSize & 0xFFFF0000) == (pDstWin->WinSize & 0xFFFF0000))
        {
            TQSScalRatioVValue.TS_Win1_Vacc = 0x100000;
        }
        else
        {
            TQSScalRatioVValue.TS_Win1_Vacc = ((1048576 * ((pSrcWin->WinSize >> 16) & 0xFFFF)) /((pDstWin->WinSize >> 16) & 0xFFFF)) & 0x1FFFFF;
        }
    }
    else if(pcbe->ChipID == CHIPID_CNE001)
    {
        if((pSrcWin->WinSize & 0xFFFF) == (pDstWin->WinSize & 0xFFFF))
        {
            TQSScalRatioHValue_cne001.TS_Win1_Hacc = 0x1000000;
        }
        else
        {
            TQSScalRatioHValue_cne001.TS_Win1_Hacc = ((16777216 * (CBIOS_U64)(pSrcWin->WinSize & 0xFFFF)) /(CBIOS_U64)(pDstWin->WinSize & 0xFFFF)) & 0x1FFFFFF;
        }
        if((pSrcWin->WinSize & 0xFFFF0000) == (pDstWin->WinSize & 0xFFFF0000))
        {
            TQSScalRatioVValue_cne001.TS_Win1_Vacc = 0x1000000;
        }
        else
        {
            TQSScalRatioVValue_cne001.TS_Win1_Vacc = ((16777216 * (CBIOS_U64)((pSrcWin->WinSize >> 16) & 0xFFFF)) /(CBIOS_U64)((pDstWin->WinSize >> 16) & 0xFFFF)) & 0x1FFFFFF;
        }
    }

    if(pSurfaceAttrib->SurfaceFmt &  CBIOS_STREAM_FMT_YUV_SPACE)
    {
        if(pModeParams->TargetModePara.YRes >= 720)
        {
            TQSCscValue.TS_In_Format = CSC_FMT_YCBCR709;
        }
        else
        {
            TQSCscValue.TS_In_Format = CSC_FMT_YCBCR601;
        }
    }
    else
    {
        TQSCscValue.TS_In_Format = CSC_FMT_RGB;
    }
    TQSCscValue.TS_Out_Format = CSC_FMT_RGB;

    if(pSurfaceAttrib->b3DMode)
    {
        TQSRightBaseValue.TS_Win1_Roffset = pSurfaceAttrib->Surface3DPara.RightFrameOffset;
    }

    if(pFlipMode->FlipImme)
    {
        TQSRightBaseValue.TS_Win1_Vsync_Off_Flip = 1;
        VsyncOffBackdoor.ts_vsync_off_backdoor = 1;
    }

    cbMMIOWriteReg32(pcbe, VsyncOffBackdoorIndex[IGAIndex], VsyncOffBackdoor.Value, 0);
    cbMMIOWriteReg32(pcbe, TQSRightBaseIndex[dwIndex][IGAIndex], TQSRightBaseValue.Value, 0);

    cbMMIOWriteReg32(pcbe, TQSStartAddrIndex[dwIndex][IGAIndex], TQSStartAddrValue.Value, 0);
    cbMMIOWriteReg32(pcbe, TQSBaseOffsetIndex[dwIndex][IGAIndex], TQSBaseOffsetValue.Value, 0);
    cbMMIOWriteReg32(pcbe, TQSStrideIndex[dwIndex][IGAIndex], TQSStrideValue.Value, 0);
    cbMMIOWriteReg32(pcbe, TQSCompFifoIndex[dwIndex][IGAIndex], TQSCompFifoValue.Value, 0);
    cbMMIOWriteReg32(pcbe, TQSFormatIndex[dwIndex][IGAIndex], TQSFormatValue.Value, 0);
    
    cbMMIOWriteReg32(pcbe, TQSDstWinIndex[dwIndex][IGAIndex], TQSDstWinValue.Value, 0);
    cbMMIOWriteReg32(pcbe, TQSDstWinPosIndex[dwIndex][IGAIndex], TQSDstWinPosValue.Value, 0);
    cbMMIOWriteReg32(pcbe, TQSSrcWinIndex[dwIndex][IGAIndex], TQSSrcWinValue.Value, 0);
    if(pcbe->ChipID == CHIPID_CHX004)
    {
        cbMMIOWriteReg32(pcbe, TQSScalRatioHIndex[dwIndex][IGAIndex], TQSScalRatioHValue.Value, 0);
        cbMMIOWriteReg32(pcbe, TQSScalRatioVIndex[dwIndex][IGAIndex], TQSScalRatioVValue.Value, 0);
    }
    else if(pcbe->ChipID == CHIPID_CNE001)
    {
        cbMMIOWriteReg32(pcbe, TQSScalRatioHIndex[dwIndex][IGAIndex], TQSScalRatioHValue_cne001.Value, 0);
        cbMMIOWriteReg32(pcbe, TQSScalRatioVIndex[dwIndex][IGAIndex], TQSScalRatioVValue_cne001.Value, 0);
    }
    cbMMIOWriteReg32(pcbe, TQSCscIndex[dwIndex][IGAIndex], TQSCscValue.Value, 0);
    cbMMIOWriteReg32(pcbe, TQSEnableIndex[dwIndex][IGAIndex], TQSEnableValue.Value, 0);
    cbMMIOWriteReg32(pcbe, TQSBLIndex[dwIndex][IGAIndex], TQSBLIndexValue.Value, 0);

    //config stream threshold
    //0x3346C[0-7]: TS win1 low threshold,[8-15]: TS win1 high threshold,[16-23]:TS win2 low threshold, [24-31]: TS win2 high threshold
    //0x33428[0-7]: QS win1 low threshold,[8-15]: QS win1 high threshold,[16-23]:QS win2 low threshold, [24-31]: QS win2 high threshold
    if(pcbe->ChipID == CHIPID_CNE001)
    {
        if((pModeParams->SrcModePara.XRes <= 2560) && (pModeParams->SrcModePara.YRes <= 1600) && (TQSCompFifoValue.TS_Share_Scaler_Fifo == 1))
        {
          
            cbMMIOWriteReg32(pcbe, StreamThresholdIndex[IGAIndex][StreamIndex], 0x0, 0x0);  
        }
        else
        {
            cbMMIOWriteReg32(pcbe, StreamThresholdIndex[IGAIndex][StreamIndex], 0xa0a0a0a0,0x0); 
         
        }
    }
    
    return  CBIOS_TRUE;
}

CBIOS_STATUS cbDoCSCAdjust_E3K(PCBIOS_EXTENSION_COMMON pcbe, CBIOS_ACTIVE_TYPE Device, PCBIOS_CSC_ADJUST_PARA pCSCAdjustPara)
{
    PCBIOS_CSC_ADJUST_PARA  pAdjustPara = pCSCAdjustPara;
    CBIOS_S64               CSCm[3][3] = {0};
    CSC_INDEX               CSCIndex = 0;
    REG_MM33698_E3K         CSCCoef1RegValue = {0}, CSCCoef1RegMask = {0xffffffff};
    REG_MM3369C_E3K         CSCCoef2RegValue = {0}, CSCCoef2RegMask = {0xffffffff};
    REG_MM336A0_E3K         CSCCoef3RegValue = {0}, CSCCoef3RegMask = {0xffffffff};
    REG_MM336A4_E3K         CSCCoef4RegValue = {0}, CSCCoef4RegMask = {0xffffffff};
    REG_MM336A8_E3K         CSCCoef5RegValue = {0}, CSCCoef5RegMask = {0xffffffff};
    REG_MM33694_E3K         LBCscFormatRegValue = {0}, LBCscFormatRegMask = {0xffffffff};
    REG_MM33868_E3K         DACCscFormatRegValue = {0}, DACCscFormatRegMask = {0xffffffff};

    if(CBIOS_TYPE_DP1 == Device)
    {
        CSCIndex = CSC_INDEX_DP1;
    }
    else if(CBIOS_TYPE_DP2 == Device)
    {
        CSCIndex = CSC_INDEX_DP2;
    }
    else if(CBIOS_TYPE_DP3 == Device)
    {
        CSCIndex = CSC_INDEX_DP3;
    }
    else if(CBIOS_TYPE_DP4 == Device)
    {
        CSCIndex = CSC_INDEX_DP4;
    }
    else
    {
        CSCIndex = CSC_INDEX_CRT;
    }

    LBCscFormatRegMask.Value = 0xffffffff;
    DACCscFormatRegMask.Value = 0xffffffff;

    if (CSC_INDEX_CRT == CSCIndex)
    {
        DACCscFormatRegValue.Dac_Csc_In_Format = pAdjustPara->InputFormat;
        DACCscFormatRegMask.Dac_Csc_In_Format = 0;
        DACCscFormatRegValue.Dac_Csc_Out_Format = pAdjustPara->OutputFormat;
        DACCscFormatRegMask.Dac_Csc_Out_Format = 0;
    }
    else
    {
        LBCscFormatRegValue.LB1_CSC_IN_FMT = pAdjustPara->InputFormat;
        LBCscFormatRegMask.LB1_CSC_IN_FMT = 0;
        LBCscFormatRegValue.LB1_CSC_OUT_FMT = pAdjustPara->OutputFormat;
        LBCscFormatRegMask.LB1_CSC_OUT_FMT = 0;
    }

    if(!pAdjustPara->Flag.bProgrammable)
    {
        if (CSC_INDEX_CRT == CSCIndex)
        {
            DACCscFormatRegValue.Dac_Csc_Program = 0;
            DACCscFormatRegMask.Dac_Csc_Program = 0;
        }
        else
        {
            LBCscFormatRegValue.LB1_PROGRAMMBLE = 0;
            LBCscFormatRegMask.LB1_PROGRAMMBLE = 0;
        }
    }
    else
    {
        if (CSC_INDEX_CRT == CSCIndex)
        {
            DACCscFormatRegValue.Dac_Csc_Program = 1;
            DACCscFormatRegMask.Dac_Csc_Program = 0;
        }
        else
        {
            LBCscFormatRegValue.LB1_PROGRAMMBLE = 1;
            LBCscFormatRegMask.LB1_PROGRAMMBLE = 0;
        }

        if(pAdjustPara->Bright > 255 || pAdjustPara->Bright < -255)
        {
            cbDebugPrint((MAKE_LEVEL(GENERIC, ERROR), "CSC bright value should be [-255,255]!\n"));
        }
        else
        {
            if(pAdjustPara->Bright >= 0)
            {
                CSCCoef5RegValue.LB1_BRIGHT = pAdjustPara->Bright;
            }
            else
            {
                CSCCoef5RegValue.LB1_BRIGHT = (CBIOS_U32)(1 << CSC_BRIGHT_MODULE) + pAdjustPara->Bright;  //get brightness 2's complement
            }
            CSCCoef5RegMask.LB1_BRIGHT = 0;
        }

        if(cbGetCscCoefMatrix(pAdjustPara,
                              pCSCAdjustPara->InputFormat,
                              pCSCAdjustPara->OutputFormat,
                              CSCm))
        {
            CSCCoef1RegValue.LB1_COEF_F1 = cbTran_CSCm_to_coef_fx(CSCm[0][0]);
            CSCCoef1RegValue.LB1_COEF_F2 = cbTran_CSCm_to_coef_fx(CSCm[0][1]);
            CSCCoef2RegValue.LB1_COEF_F3 = cbTran_CSCm_to_coef_fx(CSCm[0][2]);
            CSCCoef2RegValue.LB1_COEF_F4 = cbTran_CSCm_to_coef_fx(CSCm[1][0]);
            CSCCoef3RegValue.LB1_COEF_F5 = cbTran_CSCm_to_coef_fx(CSCm[1][1]);
            CSCCoef3RegValue.LB1_COEF_F6 = cbTran_CSCm_to_coef_fx(CSCm[1][2]);
            CSCCoef4RegValue.LB1_COEF_F7 = cbTran_CSCm_to_coef_fx(CSCm[2][0]);
            CSCCoef4RegValue.LB1_COEF_F8 = cbTran_CSCm_to_coef_fx(CSCm[2][1]);
            CSCCoef5RegValue.LB1_COEF_F9 = cbTran_CSCm_to_coef_fx(CSCm[2][2]);

            CSCCoef1RegMask.Value = 0;
            CSCCoef2RegMask.Value = 0;
            CSCCoef3RegMask.Value = 0;
            CSCCoef4RegMask.Value = 0;
            CSCCoef5RegMask.LB1_COEF_F9 = 0;
        }
        else // illegal para,use fixed coef do csc
        {
            if (CSC_INDEX_CRT == CSCIndex)
            {
                DACCscFormatRegValue.Dac_Csc_Program = 0;
                DACCscFormatRegMask.Dac_Csc_Program = 0;
            }
            else
            {
                LBCscFormatRegValue.LB1_PROGRAMMBLE = 0;
                LBCscFormatRegMask.LB1_PROGRAMMBLE = 0;
            }
        }

        if(CSC_INDEX_DP1 == CSCIndex)
        {
            cbMMIOWriteReg32(pcbe, 0x33698, CSCCoef1RegValue.Value, CSCCoef1RegMask.Value);
            cbMMIOWriteReg32(pcbe, 0x3369c, CSCCoef2RegValue.Value, CSCCoef2RegMask.Value);
            cbMMIOWriteReg32(pcbe, 0x336a0, CSCCoef3RegValue.Value, CSCCoef3RegMask.Value);
            cbMMIOWriteReg32(pcbe, 0x336a4, CSCCoef4RegValue.Value, CSCCoef4RegMask.Value);
            cbMMIOWriteReg32(pcbe, 0x336a8, CSCCoef5RegValue.Value, CSCCoef5RegMask.Value);
        }
        else if(CSC_INDEX_DP2 == CSCIndex)
        {
            cbMMIOWriteReg32(pcbe, 0x33c38, CSCCoef1RegValue.Value, CSCCoef1RegMask.Value);
            cbMMIOWriteReg32(pcbe, 0x33c3c, CSCCoef2RegValue.Value, CSCCoef2RegMask.Value);
            cbMMIOWriteReg32(pcbe, 0x33c40, CSCCoef3RegValue.Value, CSCCoef3RegMask.Value);
            cbMMIOWriteReg32(pcbe, 0x33c44, CSCCoef4RegValue.Value, CSCCoef4RegMask.Value);
            cbMMIOWriteReg32(pcbe, 0x33c48, CSCCoef5RegValue.Value, CSCCoef5RegMask.Value);
        }
        else if(CSC_INDEX_DP3 == CSCIndex)
        {
            cbMMIOWriteReg32(pcbe, 0x34338, CSCCoef1RegValue.Value, CSCCoef1RegMask.Value);
            cbMMIOWriteReg32(pcbe, 0x3433c, CSCCoef2RegValue.Value, CSCCoef2RegMask.Value);
            cbMMIOWriteReg32(pcbe, 0x34340, CSCCoef3RegValue.Value, CSCCoef3RegMask.Value);
            cbMMIOWriteReg32(pcbe, 0x34344, CSCCoef4RegValue.Value, CSCCoef4RegMask.Value);
            cbMMIOWriteReg32(pcbe, 0x34348, CSCCoef5RegValue.Value, CSCCoef5RegMask.Value);
        }
        else if(CSC_INDEX_DP4 == CSCIndex)
        {
            cbMMIOWriteReg32(pcbe, 0x34a38, CSCCoef1RegValue.Value, CSCCoef1RegMask.Value);
            cbMMIOWriteReg32(pcbe, 0x34a3c, CSCCoef2RegValue.Value, CSCCoef2RegMask.Value);
            cbMMIOWriteReg32(pcbe, 0x34a40, CSCCoef3RegValue.Value, CSCCoef3RegMask.Value);
            cbMMIOWriteReg32(pcbe, 0x34a44, CSCCoef4RegValue.Value, CSCCoef4RegMask.Value);
            cbMMIOWriteReg32(pcbe, 0x34a48, CSCCoef5RegValue.Value, CSCCoef5RegMask.Value);
        }
        else
        {
            cbMMIOWriteReg32(pcbe, 0x3386c, CSCCoef1RegValue.Value, CSCCoef1RegMask.Value);
            cbMMIOWriteReg32(pcbe, 0x33870, CSCCoef2RegValue.Value, CSCCoef2RegMask.Value);
            cbMMIOWriteReg32(pcbe, 0x33874, CSCCoef3RegValue.Value, CSCCoef3RegMask.Value);
            cbMMIOWriteReg32(pcbe, 0x33878, CSCCoef4RegValue.Value, CSCCoef4RegMask.Value);
            cbMMIOWriteReg32(pcbe, 0x3387c, CSCCoef5RegValue.Value, CSCCoef5RegMask.Value);
        }
     }

    if(CSC_INDEX_DP1 == CSCIndex)
    {
        cbMMIOWriteReg32(pcbe, 0x33694, LBCscFormatRegValue.Value, LBCscFormatRegMask.Value);
    }
    else if(CSC_INDEX_DP2 == CSCIndex)
    {
        cbMMIOWriteReg32(pcbe, 0x33c34, LBCscFormatRegValue.Value, LBCscFormatRegMask.Value);
    }
    else if(CSC_INDEX_DP3 == CSCIndex)
    {
        cbMMIOWriteReg32(pcbe, 0x34334, LBCscFormatRegValue.Value, LBCscFormatRegMask.Value);
    }
    else if(CSC_INDEX_DP4 == CSCIndex)
    {
        cbMMIOWriteReg32(pcbe, 0x34a34, LBCscFormatRegValue.Value, LBCscFormatRegMask.Value);
    }
    else
    {
        cbMMIOWriteReg32(pcbe, 0x33868, DACCscFormatRegValue.Value, DACCscFormatRegMask.Value);
    }

    return CBIOS_OK;
}

CBIOS_STATUS cbAdjustStreamCSC_E3K(PCBIOS_EXTENSION_COMMON pcbe, PCBIOS_STREAM_CSC_PARA pStreamCSCPara)
{
    PCBIOS_STREAM_CSC_PARA  pAdjustPara = pStreamCSCPara;
    CBIOS_S64               CSCm[3][3] = {0};
    CBIOS_U32               IGAIndex = pAdjustPara->IGAIndex;
    CBIOS_U32               StreamType = pAdjustPara->StreamType;
    REG_MM33634_E3K         CSCCoef1RegValue = {0}, CSCCoef1RegMask = {0xffffffff};
    REG_MM33638_E3K         CSCCoef2RegValue = {0}, CSCCoef2RegMask = {0xffffffff};
    REG_MM3363C_E3K         CSCCoef3RegValue = {0}, CSCCoef3RegMask = {0xffffffff};
    REG_MM33640_E3K         CSCCoef4RegValue = {0}, CSCCoef4RegMask = {0xffffffff};
    REG_MM33644_E3K         CscFormatRegValue = {0}, CscFormatRegMask = {0xffffffff};
    REG_MM8200_E3K          OneShotTrigValue = {0};
    CBIOS_U32               CSCCoef1RegIndex = 0;
    CBIOS_U32               CSCCoef2RegIndex = 0;
    CBIOS_U32               CSCCoef3RegIndex = 0;
    CBIOS_U32               CSCCoef4RegIndex = 0;
    CBIOS_U32               CscFormatRegIndex = 0;

    if (CBIOS_STREAM_PS == StreamType)
    {
        CSCCoef1RegIndex = PSCscCoef1[IGAIndex];
        CSCCoef2RegIndex = PSCscCoef2[IGAIndex];
        CSCCoef3RegIndex = PSCscCoef3[IGAIndex];
        CSCCoef4RegIndex = PSCscCoef4[IGAIndex];
        CscFormatRegIndex = PSCscIndex[IGAIndex];
        OneShotTrigValue.PS_Enable_Work = 1;
    }
    else if (CBIOS_STREAM_SS == StreamType)
    {
        CSCCoef1RegIndex = SSCscCoef1[IGAIndex];
        CSCCoef2RegIndex = SSCscCoef2[IGAIndex];
        CSCCoef3RegIndex = SSCscCoef3[IGAIndex];
        CSCCoef4RegIndex = SSCscCoef4[IGAIndex];
        CscFormatRegIndex = SSCscIndex[IGAIndex];
        OneShotTrigValue.SS_Enable_Work = 1;
    }
    else if (CBIOS_STREAM_TS == StreamType)
    {
        CSCCoef1RegIndex = TQSCscCoef1[0][IGAIndex];
        CSCCoef2RegIndex = TQSCscCoef2[0][IGAIndex];
        CSCCoef3RegIndex = TQSCscCoef3[0][IGAIndex];
        CSCCoef4RegIndex = TQSCscCoef4[0][IGAIndex];
        CscFormatRegIndex = TQSCscIndex[0][IGAIndex];
        OneShotTrigValue.TS_Enable_Work = 1;
    }
    else if (CBIOS_STREAM_FS == StreamType)
    {
        CSCCoef1RegIndex = TQSCscCoef1[1][IGAIndex];
        CSCCoef2RegIndex = TQSCscCoef2[1][IGAIndex];
        CSCCoef3RegIndex = TQSCscCoef3[1][IGAIndex];
        CSCCoef4RegIndex = TQSCscCoef4[1][IGAIndex];
        CscFormatRegIndex = TQSCscIndex[1][IGAIndex];
        OneShotTrigValue.QS_Enable_Work = 1;
    }

    CscFormatRegValue.PS_Data_In_Fmt = pAdjustPara->InputFormat;
    CscFormatRegMask.PS_Data_In_Fmt = 0;
    CscFormatRegValue.PS_Data_Out_Fmt = pAdjustPara->OutputFormat;
    CscFormatRegMask.PS_Data_Out_Fmt = 0;

    if(!pAdjustPara->Flag.bProgrammable)
    {
        CscFormatRegValue.PS_Program = 0;
        CscFormatRegMask.PS_Program = 0;
    }
    else
    {

        CscFormatRegValue.PS_Program = 1;
        CscFormatRegMask.PS_Program = 0;

        if(pAdjustPara->Bright > 255 || pAdjustPara->Bright < -255)
        {
            cbDebugPrint((MAKE_LEVEL(GENERIC, ERROR), "CSC bright value should be [-255,255]!\n"));
        }
        else
        {
            if(pAdjustPara->Bright >= 0)
            {
                CscFormatRegValue.PS_Bright = pAdjustPara->Bright;
            }
            else
            {
                CscFormatRegValue.PS_Bright = (CBIOS_U32)(1 << CSC_BRIGHT_MODULE) + pAdjustPara->Bright;  //get brightness 2's complement
            }
            CscFormatRegMask.PS_Bright = 0;
        }

        if(cbGetCscCoefMatrix((PCBIOS_CSC_ADJUST_PARA)pAdjustPara,
                              pAdjustPara->InputFormat,
                              pAdjustPara->OutputFormat,
                              CSCm))
        {
            CSCCoef1RegValue.PS_F1 = cbTran_CSCm_to_coef_fx(CSCm[0][0]);
            CSCCoef1RegValue.PS_F2 = cbTran_CSCm_to_coef_fx(CSCm[0][1]);
            CSCCoef2RegValue.PS_F3 = cbTran_CSCm_to_coef_fx(CSCm[0][2]);
            CSCCoef2RegValue.PS_F4 = cbTran_CSCm_to_coef_fx(CSCm[1][0]);
            CSCCoef3RegValue.PS_F5 = cbTran_CSCm_to_coef_fx(CSCm[1][1]);
            CSCCoef3RegValue.PS_F6 = cbTran_CSCm_to_coef_fx(CSCm[1][2]);
            CSCCoef4RegValue.PS_F7 = cbTran_CSCm_to_coef_fx(CSCm[2][0]);
            CSCCoef4RegValue.PS_F8 = cbTran_CSCm_to_coef_fx(CSCm[2][1]);
            CscFormatRegValue.PS_F9 = cbTran_CSCm_to_coef_fx(CSCm[2][2]);

            CSCCoef1RegMask.Value = 0;
            CSCCoef2RegMask.Value = 0;
            CSCCoef3RegMask.Value = 0;
            CSCCoef4RegMask.Value = 0;
            CscFormatRegMask.PS_F9 = 0;
        }
        else // illegal para,use fixed coef do csc
        {
            CscFormatRegValue.PS_Program = 0;
            CscFormatRegMask.PS_Program = 0;
        }

        cbMMIOWriteReg32(pcbe, CSCCoef1RegIndex, CSCCoef1RegValue.Value, CSCCoef1RegMask.Value);
        cbMMIOWriteReg32(pcbe, CSCCoef2RegIndex, CSCCoef2RegValue.Value, CSCCoef2RegMask.Value);
        cbMMIOWriteReg32(pcbe, CSCCoef3RegIndex, CSCCoef3RegValue.Value, CSCCoef3RegMask.Value);
        cbMMIOWriteReg32(pcbe, CSCCoef4RegIndex, CSCCoef4RegValue.Value, CSCCoef4RegMask.Value);
     }

    cbMMIOWriteReg32(pcbe, CscFormatRegIndex, CscFormatRegValue.Value, CscFormatRegMask.Value);
    cbMMIOWriteReg32(pcbe, OneShotTrigIndex[IGAIndex], OneShotTrigValue.Value, 0);

    return CBIOS_OK;
}

CBIOS_U32 cbTriggerPlanes_E3K(PCBIOS_EXTENSION_COMMON pcbe,
                             CBIOS_U32  IGAIndex,
                             PCBIOS_FLIP_MODE  pFlipMode)
{
    PCBIOS_FLIP_MODE_EX     pFlipModeEx = (PCBIOS_FLIP_MODE_EX)pFlipMode;
    REG_MM8200_E3K          OneShotTrigValue = {0};
    REG_MM8200_E3K          TrigStreamValue = {0};
    CBIOS_U32               i = 0;

    for(i = 0; i < OVL_NUM_E3K; i++)
    {
        if(pFlipModeEx->TrigOvlMask & (1 << i))
        {
            if(i == CBIOS_OVERLAY0)
            {
                OneShotTrigValue.Ovl0_Enable_Work = 1;
            }
            else if(i == CBIOS_OVERLAY1)
            {
                OneShotTrigValue.Ovl1_Enable_Work = 1;
            }
            else if(i == CBIOS_OVERLAY2)
            {
                OneShotTrigValue.Ovl2_Enable_Work = 1;
            }
            else if(i == CBIOS_OVERLAY3)
            {
                OneShotTrigValue.Ovl3_Enable_Work = 1;
            }
        }
    }

    for(i = 0; i < STREAM_NUM_E3K; i++)
    {
        if(pFlipModeEx->TrigStreamMask & (1 << i))
        {
            if(i == CBIOS_STREAM_PS)
            {
                OneShotTrigValue.PS_Enable_Work = 1;
                TrigStreamValue.PS_Enable_Work = 1;
            }
            else if(i == CBIOS_STREAM_SS)
            {
                OneShotTrigValue.SS_Enable_Work = 1;
                TrigStreamValue.SS_Enable_Work = 1;
            }
            else if(i == CBIOS_STREAM_TS)
            {
                OneShotTrigValue.TS_Enable_Work = 1;
                TrigStreamValue.TS_Enable_Work = 1;
            }
            else if(i == CBIOS_STREAM_FS)
            {
                OneShotTrigValue.QS_Enable_Work = 1;
            }
        }
    }

    cbMMIOWriteReg32(pcbe, OneShotTrigIndex[IGAIndex], OneShotTrigValue.Value, 0);
    
    return TrigStreamValue.Value;
}

CBIOS_BOOL cbUpdateOnePlane_E3K(PCBIOS_EXTENSION_COMMON pcbe,  
                             PCBIOS_PLANE_PARA pPlanePara, 
                             CBIOS_U32 IGAIndex)
{
    CBIOS_BOOL    bStatus = CBIOS_TRUE;
    CBIOS_U32     i, RegValue, TrigStream;
    

    if(pPlanePara->pInputStream)
    {
        if(pPlanePara->StreamType == CBIOS_STREAM_PS)
        {
            bStatus = cbUpdatePrimaryStream_E3K(pcbe, IGAIndex, pPlanePara->pInputStream, &pPlanePara->FlipMode);
        }
        else if(pPlanePara->StreamType == CBIOS_STREAM_SS)
        {
            bStatus = cbUpdateSecondStream_E3K(pcbe, IGAIndex, pPlanePara->pInputStream, &pPlanePara->FlipMode);
        }
        else if((pPlanePara->StreamType == CBIOS_STREAM_TS) || (pPlanePara->StreamType == CBIOS_STREAM_FS))
        {
            bStatus = cbUpdateThdFourStream_E3K(pcbe, IGAIndex, pPlanePara->StreamType,
                                                                  pPlanePara->pInputStream, &pPlanePara->FlipMode);
        }
    }
    
    if(pPlanePara->pOverlayInfo || (pPlanePara->FlipMode.FlipType == CBIOS_PLANE_FLIP_WITH_DISABLE))
    {
        cbUpdateOverlay_E3K(pcbe, IGAIndex, pPlanePara->PlaneIndex, pPlanePara->StreamType, 
                                                   pPlanePara->pOverlayInfo, &pPlanePara->FlipMode);
    }

    if (pPlanePara->pOVLKeyInfo) 
    {
        cbUpdateOVLKey_E3K(pcbe, IGAIndex, pPlanePara->pOVLKeyInfo);
    }

    TrigStream = cbTriggerPlanes_E3K(pcbe, IGAIndex, &pPlanePara->FlipMode);

    if (pPlanePara->FlipMode.FlipImme)
    {
        for (i = 0; i < 300; i++)
        {
            RegValue = cb_ReadU32(pcbe->pAdapterContext, OneShotTrigIndex[IGAIndex]);
            
            if ((RegValue & TrigStream) == 0)
            {
                break;
            }

            cb_DelayMicroSeconds(20);
        }
    }
    return  bStatus;
}

CBIOS_STATUS cbUpdatePlanePerTrig_E3K(PCBIOS_EXTENSION_COMMON pcbe,  
                                    PCBIOS_UPDATE_FRAME_PARA   pUpdateFramePara)
{
    CBIOS_STATUS   Status = CBIOS_OK;
    CBIOS_U32    i = 0, TrigIndex = 0xFFFFFFFF, TrigOvlMask = 0, TrigStreamMask = 0;
    PCBIOS_PLANE_PARA   pPlane = CBIOS_NULL, *ppPlane = CBIOS_NULL;
    PCBIOS_FLIP_MODE_EX   pFlipModeEx = CBIOS_NULL;

    ppPlane = pUpdateFramePara->pPlanePara;

    //stream update/disable/enable require timing&vsync to trigger, enable DCLK before touch stream
    if (pUpdateFramePara->IGAIndex == IGA1)
    {
        cbMMIOWriteReg(pcbe,CR_B_FC, 0x00, 0xFE);
    }
    else if(pUpdateFramePara->IGAIndex == IGA2)
    {
        cbMMIOWriteReg(pcbe,SR_0B, 0x00, 0xFE);
    }
    else if(pUpdateFramePara->IGAIndex == IGA3)
    {
        cbMMIOWriteReg(pcbe,CR_B_FC, 0x00, 0xFD);
    }
    
    if(pUpdateFramePara->TrigMode.OneShotTrig)
    {
        for(i = 0; i < PLANE_NUM_E3K; i++)
        {
            pPlane = ppPlane[i];

            if(!pPlane)
            {
                continue;
            }

            TrigOvlMask |= (1 << pPlane->PlaneIndex);
            TrigStreamMask |= (1 << pPlane->StreamType);
            if (pPlane->pOVLKeyInfo)
            {
                TrigStreamMask |= (1 << pPlane->pOVLKeyInfo->Index);
            }

            pFlipModeEx = (PCBIOS_FLIP_MODE_EX)(&pPlane->FlipMode);
            
            if(pPlane->PlaneIndex != pUpdateFramePara->TrigMode.TrigPlane)
            {  
                pFlipModeEx->TrigOvlMask = 0;
                pFlipModeEx->TrigStreamMask = 0;
                if(cbUpdateOnePlane_E3K(pcbe, pPlane, pUpdateFramePara->IGAIndex) != CBIOS_TRUE)
                {
                    Status = CBIOS_ER_INVALID_PARAMETER;
                    break;
                }
            }
            else if(pPlane->PlaneIndex == pUpdateFramePara->TrigMode.TrigPlane)
            {
                TrigIndex = i;
            }
        }
        if(Status == CBIOS_OK && TrigIndex != 0xFFFFFFFF)
        {
            pPlane = ppPlane[TrigIndex];
            pFlipModeEx = (PCBIOS_FLIP_MODE_EX)(&pPlane->FlipMode);
            pFlipModeEx->TrigOvlMask = TrigOvlMask;
            pFlipModeEx->TrigStreamMask = TrigStreamMask;
            if(cbUpdateOnePlane_E3K(pcbe, pPlane, pUpdateFramePara->IGAIndex) != CBIOS_TRUE)
            {
                Status = CBIOS_ER_INVALID_PARAMETER;
            }
        }
    }
    else
    {
         for(i = 0; i < PLANE_NUM_E3K; i++)
        {
            pPlane = ppPlane[i];
            if(pPlane)
            {
                pFlipModeEx = (PCBIOS_FLIP_MODE_EX)(&pPlane->FlipMode);
                pFlipModeEx->TrigOvlMask = 1 << pPlane->PlaneIndex;
                pFlipModeEx->TrigStreamMask = 1 << pPlane->StreamType;
                if (pPlane->pOVLKeyInfo)
                {
                    pFlipModeEx->TrigStreamMask |= (1 << pPlane->pOVLKeyInfo->Index);
                }
                if(cbUpdateOnePlane_E3K(pcbe, pPlane, pUpdateFramePara->IGAIndex) != CBIOS_TRUE)
                {
                    Status = CBIOS_ER_INVALID_PARAMETER;
                    break;
                }
            }
        }
    }
    return  Status;
}

CBIOS_STATUS   cbValidatePlanePara_E3K(PCBIOS_EXTENSION_COMMON pcbe,  PCBIOS_UPDATE_FRAME_PARA   pUpdateFrame)
{
    CBIOS_U32  i = 0, iga = 0;
    CBIOS_BOOL  bPlaneValid = CBIOS_FALSE;
    CBIOS_BOOL  bTrigPlaneValid = CBIOS_FALSE;
    PCBIOS_PLANE_PARA  pPlane = CBIOS_NULL;
    CBIOS_STATUS   Status = CBIOS_OK;

    if (pUpdateFrame == CBIOS_NULL)
    {
        Status = CBIOS_ER_INVALID_PARAMETER;
        goto  DONE;
    }

    iga = pUpdateFrame->IGAIndex;

    for (i = 0; i < PLANE_NUM_E3K; i++)
    {
        if(pUpdateFrame->pPlanePara[i] != CBIOS_NULL)
        {
            bPlaneValid = CBIOS_TRUE;
            break;
        }
    }

    if (!bPlaneValid)
    {
        Status = CBIOS_ER_INVALID_PARAMETER;
        goto  DONE;
    }

    for (i = 0; i < PLANE_NUM_E3K; i++)
    {
        if(pUpdateFrame->pPlanePara[i])
        {
            pPlane = pUpdateFrame->pPlanePara[i];
            if(pPlane->PlaneIndex == pUpdateFrame->TrigMode.TrigPlane)
            {
                bTrigPlaneValid = CBIOS_TRUE;
            }
            if(pPlane->FlipMode.FlipType == CBIOS_PLANE_INVALID_FLIP)
            {
                Status = CBIOS_ER_INVALID_PARAMETER;
                goto  DONE;
            }
            if(pPlane->FlipMode.FlipImme && pUpdateFrame->TrigMode.OneShotTrig)
            {
                Status = CBIOS_ER_INVALID_PARAMETER;
                goto  DONE;
            }
            if((pPlane->pInputStream == CBIOS_NULL) && (pPlane->pOverlayInfo == CBIOS_NULL)
                && (pPlane->FlipMode.FlipType != CBIOS_PLANE_FLIP_WITH_DISABLE))
            {
                Status = CBIOS_ER_INVALID_PARAMETER;
                goto  DONE;
            }
            if(pPlane->FlipMode.FlipType == CBIOS_PLANE_INVALID_FLIP)
            {
                Status = CBIOS_ER_INVALID_PARAMETER;
                goto  DONE;
            }
            if(pPlane->pInputStream)
            {
                CBIOS_BOOL  bSupportUpScale = pcbe->DispMgr.UpScaleStreamMask[iga] & (1 << i);
                CBIOS_BOOL  bSupportDownScale = pcbe->DispMgr.DownScaleStreamMask[iga] & (1 << i);
                if(!bSupportUpScale && !bSupportDownScale &&
                    (pPlane->pInputStream->SrcWindow.WinSize!= pPlane->pInputStream->DispWindow.WinSize))
                {
                    Status = CBIOS_ER_INVALID_PARAMETER;
                    goto  DONE;
                }
            }
        }
    }

    if (pUpdateFrame->TrigMode.OneShotTrig && !bTrigPlaneValid)
    {
        Status = CBIOS_ER_INVALID_PARAMETER;
    }

DONE:
    return Status;
}

CBIOS_STATUS cbUpdateFrame_E3K(PCBIOS_EXTENSION_COMMON pcbe, PCBIOS_UPDATE_FRAME_PARA   pUpdateFrame)
{
    CBIOS_STATUS         Status = CBIOS_OK;

    if(cbValidatePlanePara_E3K(pcbe, pUpdateFrame) != CBIOS_OK)
    {
        return  CBIOS_ER_INVALID_PARAMETER;
    }

    Status = cbUpdatePlanePerTrig_E3K(pcbe, pUpdateFrame);

    return  Status;
}

CBIOS_STATUS cbSetGamma_E3K(PCBIOS_EXTENSION_COMMON pcbe, PCBIOS_GAMMA_PARA pGammaParam)
{
    if((!pGammaParam) || (!pGammaParam->Flags.bDisableGamma && !pGammaParam->pGammaTable))
    {
        return CBIOS_ER_INVALID_PARAMETER;
    }

    // always use IO path
    pGammaParam->Flags.bUseIO = 1;

    if(1 == pGammaParam->Flags.bDisableGamma)
    {
       cbDisableGamma_E3K(pcbe, pGammaParam->IGAIndex);
    }
    else
    {
        cbWaitVBlank(pcbe, (CBIOS_U8)pGammaParam->IGAIndex);
        if(pGammaParam->Flags.bConfigGamma)
        {
            cbDoGammaConfig_E3K(pcbe, pGammaParam);
        }

        if(pGammaParam->Flags.bSetLUT)
        {
            cbSetLUT_E3K(pcbe, pGammaParam);
        }

        if(pGammaParam->Flags.bGetLUT)
        {
            cbGetLUT_E3K(pcbe, pGammaParam);
        }
    }

    return CBIOS_OK;
}

CBIOS_VOID cbDisableGamma_E3K(PCBIOS_EXTENSION_COMMON pcbe, CBIOS_U32 IGAIndex)
{
    REG_SR47 SR47Value = {0}, SR47Mask = {0xff};

    SR47Value.SP_Gamma_Enable = 0;
    SR47Mask.SP_Gamma_Enable = 0;
    SR47Value.SP_LUT_Interpolation_Enable = 0;
    SR47Mask.SP_LUT_Interpolation_Enable = 0;

    cbBiosMMIOWriteReg(pcbe, SR_47, SR47Value.Value, SR47Mask.Value, IGAIndex);
}

CBIOS_VOID cbDoGammaConfig_E3K(PCBIOS_EXTENSION_COMMON pcbe, PCBIOS_GAMMA_PARA pGammaParam)
{
    REG_SR47    SR47Value = {0}, SR47Mask = {0xff};
    REG_CRE2    CRE2Value = {0}, CRE2Mask = {0xff};
    REG_CRE4    CRE4Value = {0}, CRE4Mask = {0xff};

    SR47Value.SP_Gamma_Enable = 1;
    SR47Mask.SP_Gamma_Enable = 0;

    if (pGammaParam->Flags.bInterpolation)
    {
        SR47Value.SP_LUT_Interpolation_Enable = 1;
        SR47Mask.SP_LUT_Interpolation_Enable = 0;
    }
    else
    {
        SR47Value.SP_LUT_Interpolation_Enable = 0;
        SR47Mask.SP_LUT_Interpolation_Enable = 0;
    }

    if (!pGammaParam->Flags.bUseIO)
    {
        //use mmio to R/W LUT
        CRE2Value.LUT_MMIO_EN = 1;
        CRE2Mask.LUT_MMIO_EN = 0;
    }
    else
    {
        CRE2Value.LUT_MMIO_EN = 0;
        CRE2Mask.LUT_MMIO_EN = 0;

        CRE4Value.LUT1_BIT_width = 2;
        CRE4Mask.LUT1_BIT_width = 0;
        cbMMIOWriteReg(pcbe, LUTBitWidthIndex[pGammaParam->IGAIndex], CRE4Value.Value, CRE4Mask.Value);
    }

    cbBiosMMIOWriteReg(pcbe, SR_47, SR47Value.Value, SR47Mask.Value, pGammaParam->IGAIndex);
    cbMMIOWriteReg(pcbe, CR_E2, CRE2Value.Value, CRE2Mask.Value);
}

CBIOS_VOID cbGetLUT_E3K(PCBIOS_EXTENSION_COMMON pcbe, PCBIOS_GAMMA_PARA pGammaParam)
{
    PCBIOS_LUT      pGammaTable = pGammaParam->pGammaTable;
    REG_MM332AC_E3K MM332ACValue = {0}, MM332ACMask = {0xff};
    CBIOS_U32       i;

    if(IGA1 == pGammaParam->IGAIndex)
    {
        cbMMIOWriteReg(pcbe, SR_47, 0x01, 0xF0);        //LUT1 read and write
    }
    else if(IGA2 == pGammaParam->IGAIndex)
    {
        cbMMIOWriteReg(pcbe, SR_47, 0x02, 0xF0);        //LUT2 read and write
    }
    else if(IGA3 == pGammaParam->IGAIndex)
    {
        cbMMIOWriteReg(pcbe, SR_47, 0x09, 0xF0);        //LUT3 read and write
    }
    else
    {
        cbMMIOWriteReg(pcbe, SR_47, 0x0a, 0xF0);        //LUT4 read and write
    }

    if (!pGammaParam->Flags.bUseIO)
    {
        MM332ACValue.LUT_SSC_HDMI_INFO_DP_INFO_Read_current_index = (CBIOS_UCHAR)(pGammaParam->FisrtEntryIndex);
        MM332ACMask.LUT_SSC_HDMI_INFO_DP_INFO_Read_current_index = 0;
        cbMMIOWriteReg32(pcbe, 0x332ac, MM332ACValue.Value, MM332ACMask.Value);

        //read LUT
        for (i = 0; i < pGammaParam->EntryNum; i++)
        {
            pGammaTable[i].RgbLong = cb_ReadU32(pcbe->pAdapterContext, 0x81a0);
        }
    }
    else
    {
        cb_WriteU8(pcbe->pAdapterContext, 0x83c7, (CBIOS_UCHAR)pGammaParam->FisrtEntryIndex); // index auto-increments

        for ( i = 0; i < pGammaParam->EntryNum; i++)
        {
            CBIOS_U8    Byte1, Byte2, Byte3, Byte4;
            CBIOS_U16   R_10, G_10, B_10;

            Byte1 = cb_ReadU8(pcbe->pAdapterContext, 0x83c9);
            Byte2 = cb_ReadU8(pcbe->pAdapterContext, 0x83c9);
            Byte3 = cb_ReadU8(pcbe->pAdapterContext, 0x83c9);
            Byte4 = cb_ReadU8(pcbe->pAdapterContext, 0x83c9);

            R_10 = ((Byte2 >> 4) & 0x0f) | ((Byte1 & 0x3f) << 4);
            G_10 = ((Byte3 >> 2) & 0x3f) | ((Byte2 & 0x0f) << 6);
            B_10 = Byte4 | ((Byte3 & 0x03) << 8);

            pGammaTable[i].RgbLong = (R_10 << 20) | (G_10 << 10) | B_10;
        }
    }

    //restore SR_47
    cbMMIOWriteReg(pcbe, SR_47, 0x00, 0xF0);
}

CBIOS_VOID cbSetLUT_E3K(PCBIOS_EXTENSION_COMMON pcbe, PCBIOS_GAMMA_PARA pGammaParam)
{
    REG_CR33_Pair   CR33Value = {0}, CR33Mask = {0xff};
    REG_MM332AC_E3K MM332ACValue = {0}, MM332ACMask = {0xffffffff};
    PCBIOS_LUT      pGammaTable = pGammaParam->pGammaTable;
    CBIOS_U32       FirstEntry = pGammaParam->FisrtEntryIndex;
    CBIOS_U32       i = 0;

    if(IGA1 == pGammaParam->IGAIndex)
    {
        cbMMIOWriteReg(pcbe, SR_47, 0x01, 0xF0);        //LUT1 read and write
    }
    else if(IGA2 == pGammaParam->IGAIndex)
    {
        cbMMIOWriteReg(pcbe, SR_47, 0x02, 0xF0);        //LUT2 read and write
    }
    else if(IGA3 == pGammaParam->IGAIndex)
    {
        cbMMIOWriteReg(pcbe, SR_47, 0x09, 0xF0);        //LUT3 read and write
    }
    else
    {
        cbMMIOWriteReg(pcbe, SR_47, 0x0a, 0xF0);        //LUT4 read and write
    }

    //Clear CR33 bit4, enable DAC write.
    CR33Value.Lock_DAC_Writes = 0;
    CR33Mask.Lock_DAC_Writes = 0;
    cbBiosMMIOWriteReg(pcbe, CR_33, CR33Value.Value, CR33Mask.Value, pGammaParam->IGAIndex);

    if (!pGammaParam->Flags.bUseIO)
    {
        // set index
        MM332ACValue.LUT_SSC_HDMI_INFO_DP_INFO_write_current_index = (CBIOS_UCHAR)FirstEntry;
        MM332ACMask.LUT_SSC_HDMI_INFO_DP_INFO_write_current_index = 0;
        cbMMIOWriteReg32(pcbe, 0x332ac, MM332ACValue.Value, MM332ACMask.Value);

        //write lut
        for ( i = 0; i < pGammaParam->EntryNum; i++)
        {
            cb_WriteU32(pcbe->pAdapterContext, 0x81a0, pGammaTable[i].RgbLong);
        }
    }
    else
    {
        cb_WriteU8(pcbe->pAdapterContext, 0x83c8, (CBIOS_UCHAR)FirstEntry); // index auto-increments

        for ( i = 0; i < pGammaParam->EntryNum; i++)
        {
            CBIOS_U8    Byte1, Byte2, Byte3, Byte4;
            CBIOS_U16   R_10, G_10, B_10;

            B_10 = (CBIOS_U16)(pGammaTable[i].RgbLong & 0x000003FF);
            G_10 = (CBIOS_U16)((pGammaTable[i].RgbLong>>10) & 0x000003FF);
            R_10 = (CBIOS_U16)((pGammaTable[i].RgbLong>>20) & 0x000003FF);

            Byte1 = (CBIOS_U8)((R_10 & 0x03F0) >>4) ;
            Byte2 = (CBIOS_U8)(((R_10 & 0x000F)<<4) |((G_10 & 0x03C0)>>6));
            Byte3 = (CBIOS_U8)(((G_10 & 0x003F)<<2) |((B_10 & 0x0300)>>8));
            Byte4 = (CBIOS_U8)(B_10 & 0x00FF);

            cb_WriteU8(pcbe->pAdapterContext, 0x83c9, Byte1);                      //3C9h
            cb_WriteU8(pcbe->pAdapterContext, 0x83c9, Byte2);                      //3C9h
            cb_WriteU8(pcbe->pAdapterContext, 0x83c9, Byte3);                      //3C9h
            cb_WriteU8(pcbe->pAdapterContext, 0x83c9, Byte4);                      //3C9h
        }
    }

    //restore SR_47
    cbMMIOWriteReg(pcbe, SR_47, 0x00, 0xF0);
}

CBIOS_STATUS cbCECEnableDisable_E3K(PCBIOS_VOID pvcbe, PCBIOS_CEC_ENABLE_DISABLE_PARA pCECEnableDisablePara)
{
    PCBIOS_EXTENSION_COMMON pcbe = pvcbe;
    CBIOS_U32   CECMiscReg = 0;
    CBIOS_STATUS Status = CBIOS_OK;
    CBIOS_CEC_INDEX CECIndex = CBIOS_CEC_INDEX1;

    if (pCECEnableDisablePara == CBIOS_NULL)
    {
        Status = CBIOS_ER_NULLPOINTER;
        cbDebugPrint((MAKE_LEVEL(HDMI, ERROR), "CBiosCECEnableDisable_E3K: pCECEnableDisablePara is NULL!"));
    }
    else if (!pcbe->ChipCaps.IsSupportCEC)
    {
        Status = CBIOS_ER_HARDWARE_LIMITATION;
        cbDebugPrint((MAKE_LEVEL(HDMI, ERROR), "CBiosCECEnableDisable_E3K: Can't support CEC!"));
    }
    else if (pCECEnableDisablePara->CECIndex >= CBIOS_CEC_INDEX_COUNT)
    {
        Status = CBIOS_ER_INVALID_PARAMETER;
        cbDebugPrint((MAKE_LEVEL(HDMI, ERROR), "CBiosCECEnableDisable_E3K: invalid CEC index!"));
    }
    else
    {
        CECIndex = pCECEnableDisablePara->CECIndex;
        if (CECIndex == CBIOS_CEC_INDEX1)
        {
            CECMiscReg = 0x33148;
        }
        else if (CECIndex == CBIOS_CEC_INDEX2)
        {
            CECMiscReg = 0x33e34;
        }
        else if (CECIndex == CBIOS_CEC_INDEX3)
        {
            CECMiscReg = 0x34538;
        }
        else if (CECIndex == CBIOS_CEC_INDEX4)
        {
            CECMiscReg = 0X34c38;
        }


        if (pCECEnableDisablePara->bEnableCEC)
        {
            cbMMIOWriteReg32(pcbe, CECMiscReg, BIT18, ~BIT18);
            pcbe->CECPara[CECIndex].CECEnable = CBIOS_TRUE;
            //allocate logical address
            cbCECAllocateLogicalAddr(pcbe, CECIndex);
        }
        else
        {
            cbMMIOWriteReg32(pcbe, CECMiscReg, 0x00000000, ~BIT18);
            pcbe->CECPara[CECIndex].CECEnable = CBIOS_FALSE;
        }

        Status = CBIOS_OK;

    }

    return Status;

}

static CBIOS_STATUS cbUpdateCursorColor_E3K(PCBIOS_EXTENSION_COMMON pcbe, CBIOS_U32 IGAIndex, CBIOS_U32 Bg, CBIOS_U32 Fg)
{
    CBIOS_STATUS    bStatus = CBIOS_OK;

    REG_CR4A_Pair   Cr4a    = {0};
    REG_CR4B_Pair   Cr4b    = {0};
    CBIOS_U32       MmioBase = 0;

    if(IGA1 == IGAIndex)
    {
        MmioBase = MMIO_OFFSET_CR_GROUP_A;
    }
    else if(IGA2 == IGAIndex)
    {
        MmioBase = MMIO_OFFSET_CR_GROUP_B;
    }
    else if(IGA3 == IGAIndex)
    {
        MmioBase = MMIO_OFFSET_CR_GROUP_T;
    }
    else
    {
        MmioBase = MMIO_OFFSET_CR_GROUP_F;
    }

    // foreground color
    cbBiosMMIOReadReg(pcbe, CR_45, IGAIndex);//reset cr4a to '0' by reading cr45.
    Cr4a.HW_Cursor_Color_Foreground_Stack = Fg & 0xff;
    cb_WriteU8(pcbe->pAdapterContext, MmioBase + 0x4a, Cr4a.Value);
    Cr4a.HW_Cursor_Color_Foreground_Stack = (Fg >> 8)& 0xff;
    cb_WriteU8(pcbe->pAdapterContext, MmioBase + 0x4a, Cr4a.Value);
    Cr4a.HW_Cursor_Color_Foreground_Stack = (Fg >> 16)& 0xff;
    cb_WriteU8(pcbe->pAdapterContext, MmioBase + 0x4a, Cr4a.Value);
    Cr4a.HW_Cursor_Color_Foreground_Stack = (Fg >> 24)& 0xff;
    cb_WriteU8(pcbe->pAdapterContext, MmioBase + 0x4a, Cr4a.Value);
    // background color
    cbBiosMMIOReadReg(pcbe, CR_45, IGAIndex);//reset cr4a to '0' by reading cr45.
    Cr4b.HW_Cursor_Color_Background_Stack = Bg & 0xff;
    cb_WriteU8(pcbe->pAdapterContext, MmioBase + 0x4b, Cr4b.Value);
    Cr4b.HW_Cursor_Color_Background_Stack = (Bg >> 8) & 0xff;
    cb_WriteU8(pcbe->pAdapterContext, MmioBase + 0x4b, Cr4b.Value);
    Cr4b.HW_Cursor_Color_Background_Stack = (Bg >> 16) & 0xff;
    cb_WriteU8(pcbe->pAdapterContext, MmioBase + 0x4b, Cr4b.Value);
    Cr4b.HW_Cursor_Color_Background_Stack = (Bg >> 24) & 0xff;
    cb_WriteU8(pcbe->pAdapterContext, MmioBase + 0x4b, Cr4b.Value);

    return bStatus;
}

CBIOS_STATUS cbSetHwCursor_E3K(PCBIOS_EXTENSION_COMMON pcbe, PCBIOS_CURSOR_PARA pSetCursor)
{
    CBIOS_STATUS    bStatus = CBIOS_OK;
    CBIOS_U32       IGAIndex = pSetCursor->IGAIndex;
    PCBIOS_DISP_MODE_PARAMS  pModeParams = pcbe->DispMgr.pModeParams[IGAIndex];
    CBIOS_U32       CursorBaseAddr = 0;
    REG_MM33718_E3K CursorCtrl1RegValue = {0};
    REG_MM3371C_E3K CursorCtrl2RegValue = {0};
    REG_MM33720_E3K CursorBaseAddrRegValue = {0};
    REG_MM33728_E3K CursorEndPixelRegValue = {0};
    REG_MM81E4_E3K  CursorCscRegValue = {0};
    CBIOS_U32       CursorSize = 0;
    CBIOS_U8        StartX = 0;
    CBIOS_U8        StartY = 0;

    CursorCtrl1RegValue.Value = 0;

    // enable MMIO setting
    CursorCtrl1RegValue.Cursor_1_mmio_reg_en = 1;

    //check position
    if(!pSetCursor->bDisable)
    {
        if(pSetCursor->Position.PositionX < 0)
        {
            StartX = -pSetCursor->Position.PositionX;
            pSetCursor->Position.PositionX = 0;
        }
        if(pSetCursor->Position.PositionY < 0)
        {
            StartY = -pSetCursor->Position.PositionY;
            pSetCursor->Position.PositionY = 0;
        }
        if(pSetCursor->Position.CursorSize == CBIOS_CURSOR_SIZE_64x64)
        {
            CursorSize = 64;
        }
        else
        {
            CursorSize = 128;
        }
        if(StartX >= CursorSize || StartY >= CursorSize)
        {
            pSetCursor->bDisable = 1;
        }
    }

    if(pSetCursor->bDisable)
    {
        CursorCtrl1RegValue.Cursor_1_Enable = 0;
        CursorBaseAddrRegValue.Cursor_1_Enable_Work_Registers = 1;
        cbMMIOWriteReg32(pcbe, CursorControl1Index[IGAIndex], CursorCtrl1RegValue.Value, 0);
        cbMMIOWriteReg32(pcbe, CursorBaseAddrIndex[IGAIndex], CursorBaseAddrRegValue.Value, 0);
        return CBIOS_OK;
    }

    //enable cursor
    CursorCtrl1RegValue.Cursor_1_Enable = 1;
    
    //position
    CursorCtrl2RegValue.Cursor_1_X_Origin = pSetCursor->Position.PositionX;
    CursorCtrl2RegValue.Cursor_1_Y_Origin = pSetCursor->Position.PositionY;

    //start pixel
    CursorCtrl1RegValue.Cursor_1_Display_Start_X = StartX;
    CursorCtrl1RegValue.Cursor_1_Display_Start_Y = StartY;
    CursorEndPixelRegValue.Cursor_1_X_end = CursorSize;
    CursorEndPixelRegValue.Cursor_1_Y_end = CursorSize;

    //size
    CursorCtrl1RegValue.Cursor_1_Size = pSetCursor->Position.CursorSize;

    //type
    CursorCtrl1RegValue.Cursor_1_Type = pSetCursor->CursorAttrib.Type;
    if(pSetCursor->CursorAttrib.Type == CBIOS_MONO_CURSOR)
    {
        cbUpdateCursorColor_E3K(pcbe, pSetCursor->IGAIndex, pSetCursor->CursorAttrib.Color.BackGround, pSetCursor->CursorAttrib.Color.ForeGround);
    }

    //address
    CursorBaseAddr = (CBIOS_U32)(pSetCursor->CursorAttrib.CurAddr >> 5);
    CursorBaseAddrRegValue.Cursor_1_Base_Address = CursorBaseAddr;

    //rotation
    if (pSetCursor->bhMirror)
    {
        CursorCtrl1RegValue.Cursor_1_X_Rotation = 1;
    }
    if (pSetCursor->bvMirror)
    {
        CursorCtrl1RegValue.Cursor_1_Y_Rotation = 1;
    }

    //format
    CursorCscRegValue.Cursor1_Csc_output_Fmt = pModeParams->IGAOutColorSpace;

    CursorBaseAddrRegValue.Cursor_1_Enable_Work_Registers = 1;

    cbMMIOWriteReg32(pcbe, CursorCscIndex[IGAIndex], CursorCscRegValue.Value, 0);
    cbMMIOWriteReg32(pcbe, CursorControl1Index[IGAIndex], CursorCtrl1RegValue.Value, 0);
    cbMMIOWriteReg32(pcbe, CursorControl2Index[IGAIndex], CursorCtrl2RegValue.Value, 0);
    cbMMIOWriteReg32(pcbe, CursorEndPixelIndex[IGAIndex], CursorEndPixelRegValue.Value, 0);
    cbMMIOWriteReg32(pcbe, CursorBaseAddrIndex[IGAIndex], CursorBaseAddrRegValue.Value, 0);

    return  bStatus;
}

/*****************************************************************************************/
//
//Function:cbGetSysBiosInfo
//Discription:
//      This function will init pcbe scratch pad and related HW registers from system bios info
//Return:
//      TRUE --- Success, init success, in system bios info valid case
//      FALSE--- Fail, system bios info invalid, init fail
/*****************************************************************************************/
CBIOS_BOOL cbGetSysBiosInfo(PCBIOS_EXTENSION_COMMON pcbe)
{
    CBIOS_BOOL status = CBIOS_TRUE;    

    if(pcbe->SysBiosInfo.bSysBiosInfoValid == CBIOS_TRUE)
    {
       pcbe->SysBiosInfo.BootUpDev = cbConvertVBiosDevBit2CBiosDevBit(pcbe->SysBiosInfo.BootUpDev);
   
       cbDebugPrint((MAKE_LEVEL(GENERIC, INFO), "\nCBIOS:cbGetSysBiosInfo:pcbe->SysBiosInfo.BootUpDev = 0x%x!\n", pcbe->SysBiosInfo.BootUpDev));      
    }
    else
    {
        status = CBIOS_FALSE;
    }
    return status;
}

CBIOS_VOID cbDisableStream_E3K(PCBIOS_EXTENSION_COMMON pcbe, CBIOS_U32 IGAIndex)
{
    REG_MM81FC_E3K     PSShadow = {0};
    REG_MM81C4_E3K     PSVsyncOff = {0};
    REG_MM33834_E3K    SSEnable = {0};
    REG_MM81D4_E3K     SSRightBase = {0};
    REG_MM8458_E3K     TQSEnable = {0};
    REG_MM844C_E3K     TQSRightBase = {0};
    REG_MM8200_E3K     OneShotTrig = {0};
    
    //use mmio
    cbBiosMMIOWriteReg(pcbe, CR_67, 0x08, ~0x08, IGAIndex);
    cbBiosMMIOWriteReg(pcbe, CR_64, 0x40, ~0x40, IGAIndex);

    PSShadow.PS_Disable = 1;
    PSVsyncOff.VsyncOff = 1;
    SSEnable.SS_Enable = 0;
    SSRightBase.SS_Vsync_Off_Flip = 1;
    TQSEnable.TS_En = 0;
    TQSRightBase.TS_Win1_Vsync_Off_Flip = 1;
    
    // vsync off
    cbMMIOWriteReg32(pcbe, PSVsyncOffIndex[IGAIndex], PSVsyncOff.Value, ~(PSVsyncOff.Value));
    cbMMIOWriteReg32(pcbe, SSRightBaseIndex[IGAIndex], SSRightBase.Value, ~(SSRightBase.Value));
    //cbMMIOWriteReg32(pcbe, TQSRightBaseIndex[0][IGAIndex], TQSRightBase.Value, ~(TQSRightBase.Value));
    //cbMMIOWriteReg32(pcbe, TQSRightBaseIndex[1][IGAIndex], TQSRightBase.Value, ~(TQSRightBase.Value));

    // disable stream
    cbMMIOWriteReg32(pcbe, PSShadowIndex[IGAIndex], PSShadow.Value, ~(PSShadow.Value));
    cbMMIOWriteReg32(pcbe, SSEnableIndex[IGAIndex], SSEnable.Value, ~(SSEnable.Value));
    //cbMMIOWriteReg32(pcbe, TQSEnableIndex[0][IGAIndex], TQSEnable.Value, ~(TQSEnable.Value));
    //cbMMIOWriteReg32(pcbe, TQSEnableIndex[1][IGAIndex], TQSEnable.Value, ~(TQSEnable.Value));

    if(pcbe->DispMgr.StreamCount[IGAIndex] >= CBIOS_STREAM_TS+1)
    {
        cbMMIOWriteReg32(pcbe, TQSRightBaseIndex[0][IGAIndex], TQSRightBase.Value, ~(TQSRightBase.Value));
        cbMMIOWriteReg32(pcbe, TQSEnableIndex[0][IGAIndex], TQSEnable.Value, ~(TQSEnable.Value));
    }
    if(pcbe->DispMgr.StreamCount[IGAIndex] >= CBIOS_STREAM_FS+1)
    {
        cbMMIOWriteReg32(pcbe, TQSRightBaseIndex[1][IGAIndex], TQSRightBase.Value, ~(TQSRightBase.Value));
        cbMMIOWriteReg32(pcbe, TQSEnableIndex[1][IGAIndex], TQSEnable.Value, ~(TQSEnable.Value));
    }

    OneShotTrig.Iga_Enable_Work = 1;
    cbMMIOWriteReg32(pcbe, OneShotTrigIndex[IGAIndex], OneShotTrig.Value, ~(OneShotTrig.Value));
    
}


CBIOS_BOOL cbUpdateShadowInfo_CX4(PCBIOS_EXTENSION_COMMON pcbe, PCBIOS_PARAM_SHADOWINFO pShadowInfo)
{
    cbDebugPrint((MAKE_LEVEL(GENERIC, DEBUG),"cbUpdateShadowInfo_CX4: %x, %x\n", pShadowInfo->SysShadowAddr, pShadowInfo->SysShadowLength));
    if (!pShadowInfo)
    {
        cbDebugPrint((MAKE_LEVEL(GENERIC, DEBUG),"cbUpdateShadowInfo_CX4: Null pointer!\n" ));
        ASSERT(CBIOS_FALSE);
        return CBIOS_FALSE;
    }
    //clear pcbe sysbiosInfo_chx
    cb_memset(&(pcbe->SysBiosInfo),0,sizeof(Shadow_Info));
    pcbe->SysBiosInfo.bSysBiosInfoValid = CBIOS_FALSE;
    //Get sys shadow info
    if(pShadowInfo->SysShadowAddr != CBIOS_NULL && pShadowInfo->SysShadowLength > sizeof(SYSBIOSInfoHeader_CX4))
    {
        PSYSBIOSInfo_CX4 pSysInfo = (PSYSBIOSInfo_CX4)(((CBIOS_U8*)pShadowInfo->SysShadowAddr));
        CBIOS_U32 dwSysInfoLen = pSysInfo->Header.Length;

        if((pShadowInfo->SysShadowLength >= dwSysInfoLen) && (cbGetCheckSum((CBIOS_U8*)pShadowInfo->SysShadowAddr, dwSysInfoLen)==0))
        {
            if(dwSysInfoLen > sizeof(SYSBIOSInfo_CX4))
            {
                dwSysInfoLen = sizeof(SYSBIOSInfo_CX4);
            }    
            if(dwSysInfoLen > sizeof(SYSBIOSInfoHeader_CX4))
            {
                pcbe->SysBiosInfo.bSysBiosInfoValid = CBIOS_TRUE;
                pcbe->SysBiosInfo.Version = pSysInfo->Header.Version;
                pcbe->SysBiosInfo.FBSize = pSysInfo->FBSize;
                pcbe->SysBiosInfo.DRAMDataRateIdx = pSysInfo->DRAMDataRateIdx;
                pcbe->SysBiosInfo.DRAMMode = pSysInfo->DRAMMode;
                pcbe->SysBiosInfo.SSCCtrl = pSysInfo->SSCCtrl;
                pcbe->SysBiosInfo.ChipCapability = pSysInfo->ChipCapability;
                pcbe->SysBiosInfo.LowTopAddress = pSysInfo->LowTopAddress;
                pcbe->SysBiosInfo.BootUpDev = pSysInfo->BootUpDev;
                pcbe->SysBiosInfo.DetalBootUpDev = pSysInfo->DetalBootUpDev;
                pcbe->SysBiosInfo.SnoopOnly = pSysInfo->SnoopOnly;
                pcbe->SysBiosInfo.bTAEnable = pSysInfo->bTAEnable;
                pcbe->SysBiosInfo.bInvertPanelSignal = pSysInfo->bInvertPanelSignal;
                pcbe->SysBiosInfo.bBLGfxMode = pSysInfo->bBLGfxMode;
                pcbe->SysBiosInfo.bLgaChip = pSysInfo->bLgaChip;
                pcbe->SysBiosInfo.bBLGfxPwm = pSysInfo->Header.Version >= 0x12 ? pSysInfo->bBLGfxPwm : 0;
                pcbe->SysBiosInfo.bInvertPwmBL  = pSysInfo->Header.Version >= 0x12 ? pSysInfo->bInvertPwmBL : 0;
                pcbe->SysBiosInfo.bBgaPatchChip  = pSysInfo->bBgaPatchChip;
                pcbe->SysBiosInfo.ECLK = pSysInfo->ECLK;
                pcbe->SysBiosInfo.VCLK = pSysInfo->VCLK;
                pcbe->SysBiosInfo.ICLK = pSysInfo->ICLK;
                pcbe->SysBiosInfo.AddressCHA = pSysInfo->AddressCHA;
                pcbe->SysBiosInfo.AddressCHB = pSysInfo->AddressCHB;

                if(pSysInfo->Header.Version >= 0x11)
                {
                    pcbe->SysBiosInfo.VgaPortConnType = (CBIOS_U8)pSysInfo->VgaPortConnType;
                    pcbe->SysBiosInfo.Dp1PortConnType = (CBIOS_U8)pSysInfo->Dp1PortConnType;
                    pcbe->SysBiosInfo.Dp2PortConnType = (CBIOS_U8)pSysInfo->Dp2PortConnType;
                    pcbe->SysBiosInfo.Dp3PortConnType = (CBIOS_U8)pSysInfo->Dp3PortConnType;

                    if(pcbe->SysBiosInfo.VgaPortConnType >= CBIOS_INVALID_CONN)
                    {
                        pcbe->SysBiosInfo.VgaPortConnType = CBIOS_VGA_CONN;
                    }
                    if(pcbe->SysBiosInfo.Dp1PortConnType >= CBIOS_INVALID_CONN)
                    {
                        pcbe->SysBiosInfo.Dp1PortConnType = CBIOS_DP_CONN;
                    }
                    if(pcbe->SysBiosInfo.Dp2PortConnType >= CBIOS_INVALID_CONN)
                    {
                        pcbe->SysBiosInfo.Dp2PortConnType = CBIOS_DP_CONN;
                    }
                    if(pcbe->SysBiosInfo.Dp3PortConnType >= CBIOS_INVALID_CONN)
                    {
                        pcbe->SysBiosInfo.Dp3PortConnType = CBIOS_DP_CONN;
                    }
                    if(pcbe->SysBiosInfo.VgaPortConnType == CBIOS_NON_CONN
                        && pcbe->SysBiosInfo.Dp1PortConnType == CBIOS_NON_CONN
                        && pcbe->SysBiosInfo.Dp2PortConnType == CBIOS_NON_CONN
                        && pcbe->SysBiosInfo.Dp3PortConnType == CBIOS_NON_CONN)
                    {
                        pcbe->SysBiosInfo.VgaPortConnType = CBIOS_VGA_CONN;
                        pcbe->SysBiosInfo.Dp1PortConnType = CBIOS_DP_CONN;
                        pcbe->SysBiosInfo.Dp2PortConnType = CBIOS_DP_CONN;
                        pcbe->SysBiosInfo.Dp3PortConnType = CBIOS_DP_CONN;
                    }
                }
                else
                {
                    pcbe->SysBiosInfo.VgaPortConnType = CBIOS_VGA_CONN;
                    pcbe->SysBiosInfo.Dp1PortConnType = CBIOS_DP_CONN;
                    pcbe->SysBiosInfo.Dp2PortConnType = CBIOS_DP_CONN;
                    pcbe->SysBiosInfo.Dp3PortConnType = CBIOS_DP_CONN;
                }
                
                cbGetSysBiosInfo(pcbe);
                cbDebugPrint((MAKE_LEVEL(GENERIC, INFO),  "ShadowInfo: bBLGfxMode = %d, bBLGfxPwm = %d, bInvertPwmBL = %d, bInvertPanelSignal = %d, bLgaChip = %d, bBgaPatchChip = %d\n", 
                    pcbe->SysBiosInfo.bBLGfxMode, pcbe->SysBiosInfo.bBLGfxPwm, pcbe->SysBiosInfo.bInvertPwmBL, pcbe->SysBiosInfo.bInvertPanelSignal, pcbe->SysBiosInfo.bLgaChip, pcbe->SysBiosInfo.bBgaPatchChip));
                cbDebugPrint((MAKE_LEVEL(GENERIC, INFO), "ShadowInfo: FBSize = %dM DRAMMode = %d bSnoopOnly = %d\n", (1 << (pcbe->SysBiosInfo.FBSize & 0x0F)), (pcbe->SysBiosInfo.DRAMMode & BIT0),pcbe->SysBiosInfo.SnoopOnly));
                cbDebugPrint((MAKE_LEVEL(GENERIC, INFO), "Ver:0x%x, VgaPort:%d, Dp1Port:%d, Dp2Port:%d, Dp3Port:%d.\n", pSysInfo->Header.Version, pcbe->SysBiosInfo.VgaPortConnType, pcbe->SysBiosInfo.Dp1PortConnType, 
                    pcbe->SysBiosInfo.Dp2PortConnType, pcbe->SysBiosInfo.Dp3PortConnType));
                cbDebugPrint((MAKE_LEVEL(GENERIC, INFO), "ShadowInfo: AddressCHA = 0x%x, AddressCHB = 0x%x\n", pcbe->SysBiosInfo.AddressCHA, pcbe->SysBiosInfo.AddressCHB));
                return CBIOS_TRUE;
            }
        }
    }

    return CBIOS_FALSE;
}
