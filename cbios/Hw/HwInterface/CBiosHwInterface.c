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
** CBios hw layer interface function implementation.
**
** NOTE:
** The sw layer CAN call the hw interface defined in this file to do some hw related operation. 
******************************************************************************/

#include "CBiosChipShare.h"
#include "CBiosHwInterface.h"
#include "../CBiosHwShare.h"
#include "../HwInit/CBiosInitHw.h"
#include "../HwBlock/CBiosIGA_Timing.h"
#include "../HwBlock/CBiosScaler.h"
#include "../E3K/CBios_E3K.h"
#include "../E3K/CBiosVCP_E3K.h"
#include "../HwBlock/CBiosDIU_VIP.h"
#include "../HwBlock/CBiosDIU_HDMI.h"
#include "../HwBlock/CBiosDIU_HDTV.h"

static    CBIOS_U32  CECMiscReg1Tab[4]       = {0x33148,0x33e34,0x34538,0x34c38};
static    CBIOS_U32  CECInitiatorCmdTab[4]   = {0x3314c,0x33e38,0x3453c,0x34c3c};
static    CBIOS_U32  CECMiscReg2Tab[4]       = {0x33150,0x33e3c,0x34540,0x34c40};
static    CBIOS_U32  CECFollowerCmdTab[4]    = {0x33154,0x33e40,0x34544,0x34c44};

CBIOS_STATUS cbHWInit(PCBIOS_VOID pvcbe)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    CBIOS_STATUS            status = CBIOS_OK;
    CBIOS_U32 wait_us = 100, timeout = 10000, count = 0;

    while (pcbe->ChipID == CHIPID_E3K)
    {
        if (cb_ReadU32(pcbe->pAdapterContext, 0xd370) == 0xdeadbeaf)
        {
            break;
        }

        cb_DelayMicroSeconds(wait_us);
        count++;
        if (count > timeout)
        {
            cbDebugPrint((MAKE_LEVEL(GENERIC, ERROR), "%s: wait memory init time out\n", FUNCTION_NAME));
            break;
        }    
    }

    if(cbHWIsChipPost(pcbe))
    {
        cbUnlockSR(pcbe);
        cbUnlockCR(pcbe);
    }
    else
    {
        status = cbPost(pcbe);
    }

    if(status == CBIOS_OK)
    {
        status = cbInitChip(pcbe);
    }

    if(status != CBIOS_OK)
    {
        cbDebugPrint((MAKE_LEVEL(GENERIC, ERROR),"cbHWInit: post failed!\n"));
    }

    return status;
}


CBIOS_VOID cbHWInitChipAttribute(PCBIOS_VOID pvcbe, CBIOS_U32 ChipID)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;

    if(ChipID == CHIPID_E3K || ChipID == CHIPID_CHX004 || ChipID == CHIPID_CNE001)
    {
        cbInitChipAttribute_E3K(pcbe);
    }
}

CBIOS_BOOL cbHWIsChipPost(PCBIOS_VOID pvcbe)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    CBIOS_U8  byTemp = 0;
    CBIOS_U8  byResult = 0;
    byTemp = cbMMIOReadReg(pcbe, CR_52);
    if(0xA == byTemp)
    {
        byResult = 1;
    }
    return byResult;
}

CBIOS_STATUS cbHWSetChipPostFlag(PCBIOS_VOID pvcbe)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    CBIOS_STATUS  status = CBIOS_OK;
    cbMMIOWriteReg(pcbe, CR_52, 0x0A, 0x0);
    return status;
}


CBIOS_BOOL cbHWIsChipEnable(PCBIOS_VOID pvcbe)
{
    CBIOS_U8 byTemp1 = 0, byTemp2 = 0;
    CBIOS_U8 byResult = 0;
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    
    byTemp1 = cb_ReadU8(pcbe->pAdapterContext, 0x850C) & 0x2;
    byTemp2 = cb_ReadU8(pcbe->pAdapterContext, 0x83C3) & 0x01;
    byResult = (byTemp1 >> 1) | byTemp2;
    if(!byResult)
    {
        cbDebugPrint((MAKE_LEVEL(GENERIC, INFO),"Chip is not enable!\n"));
    }
    return byResult;
}

CBIOS_STATUS cbHWSetMmioEndianMode(PCBIOS_VOID pAdapterContext)
{
    CBIOS_U32 crf0_c = cb_ReadU32(pAdapterContext, 0x8af0);

#ifdef __BIG_ENDIAN__
    //here has a littel side effect when POST D3 as BE mode under BE system.
    //If D3 has been POST BE mode and now POST it again,because D3 BE is enabled,
    //fetched crf0_c[DW] will be byte swapped by BIU. so crf2_c[7] will be set 1 wrongly instead crf1_c[7].
    //but currently crf2_c[7] is reserved, so we can ignore it.
    crf0_c |= 0x00800000;//enable d3 big endian mode
#else
    crf0_c &= ~0x00008000; //enable d3 little endian mode
#endif

    cb_WriteU32(pAdapterContext, 0x8af0, crf0_c);//endian selection

    return CBIOS_OK;
}


CBIOS_STATUS cbHWUnload(PCBIOS_VOID pvcbe)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    CBIOS_U8        byRomType = 0;
    CBIOS_U8        byTemp = 0;

#ifdef CHECK_CHIPENABLE
    if (!cbHWIsChipEnable(pcbe))
        return CBIOS_ER_CHIPDISABLE;
#endif

    if(!pcbe->bMAMMPrimaryAdapter)
    {
        REG_CR37    RegCR37Mask;
        //Harrison:
        //VBios & CBios will invalid the Rom type after Post and save the
        //correct Rom type into CRC8, so for Secondary adapter we need
        //restore the correct Rom type for CR37 or the Vista's StartDevice()
        //can not read RomImage again

        //CRC8 low 3bits is the rom type saved by CBios init code
        byRomType = cbMMIOReadReg(pcbe, CR_C8) & 0x07;
        byTemp    = cbMMIOReadReg(pcbe, CR_37) & 0xF8;
        RegCR37Mask.Value = 0xFF;
        RegCR37Mask.reserved = 0;
        cbMMIOWriteReg(pcbe,CR_37,byTemp|byRomType, RegCR37Mask.Value);
    }

    cbDeInitDeviceArray(pcbe);
    cbDispMgrDeInit(pcbe);
    return CBIOS_OK;
}

CBIOS_STATUS cbHWSetIgaScreenOnOffState(PCBIOS_VOID pvcbe, CBIOS_BOOL status, CBIOS_U8 IGAIndex)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    REG_CR71_Pair    RegCR71Value;
    REG_CR71_Pair    RegCR71Mask;
    REG_SR01    RegSR01Value;
    REG_SR01    RegSR01Mask;
    REG_CRFC_B RegCRB_FCValue;
    REG_CRFC_B RegCRB_FCMask;
    REG_SR0B RegSR0BValue;
    REG_SR0B RegSR0BMask;

#ifdef CHECK_CHIPENABLE
    if (!cbHWIsChipEnable(pcbe))
        return CBIOS_ER_CHIPDISABLE;
#endif

    cbTraceEnter(GENERIC);

    RegCR71Value.Value = cbBiosMMIOReadReg(pcbe, CR_71, IGAIndex);
    if((RegCR71Value.Screen_Off_Control_Select == 1) &&
        ((RegCR71Value.Screen_Off == 0 && status) || (RegCR71Value.Screen_Off == 1 && !status)))
    {
        return CBIOS_OK;
    }
    
    if (status) //Screen On
    {
        RegCR71Value.Value = 0;
        RegCR71Value.Screen_Off_Control_Select = 1;
        RegCR71Value.Screen_Off = 0;
        RegCR71Mask.Value = 0xFF;
        RegCR71Mask.Screen_Off_Control_Select = 0;
        RegCR71Mask.Screen_Off = 0;
        cbBiosMMIOWriteReg(pcbe, CR_71, RegCR71Value.Value, RegCR71Mask.Value, IGAIndex);
        
        if(pcbe->ChipID >= CHIPID_CHX004)
        {
            if(IGAIndex == IGA1)
            {
                cbMMIOWriteReg32(pcbe, 0x8180, 0x0, ~0x1);
                cb_WriteU32(pcbe->pAdapterContext, 0x8200, 0x80000000);
            }
            else if(IGAIndex == IGA2)
            {
                cbMMIOWriteReg32(pcbe, 0x33900, 0x0, ~0x1);
                cb_WriteU32(pcbe->pAdapterContext, 0x33958, 0x80000000);
            }
            else if(IGAIndex == IGA3)
            {
                cbMMIOWriteReg32(pcbe, 0x34000, 0x0, ~0x1);
                cb_WriteU32(pcbe->pAdapterContext, 0x34058, 0x80000000);
            }
            cbWaitVSync(pcbe, IGAIndex);
        }
        
    }
    else        //Screen Off
    {
        cbWaitNonFullVBlank(pcbe, IGAIndex);
        RegCR71Value.Value = 0;
        RegCR71Value.Screen_Off_Control_Select = 1;
        RegCR71Value.Screen_Off = 1;
        RegCR71Mask.Value = 0xFF;
        RegCR71Mask.Screen_Off_Control_Select = 0;
        RegCR71Mask.Screen_Off = 0;
        cbBiosMMIOWriteReg(pcbe, CR_71, RegCR71Value.Value, RegCR71Mask.Value, IGAIndex);
        
        if(pcbe->ChipID >= CHIPID_CHX004)
        {
            if(IGAIndex == IGA1)
            {
                cbMMIOWriteReg32(pcbe, 0x8180, 0x1, ~0x1);
                cb_WriteU32(pcbe->pAdapterContext, 0x8200, 0x80000000);
            }
            else if(IGAIndex == IGA2)
            {
                cbMMIOWriteReg32(pcbe, 0x33900, 0x1, ~0x1);
                cb_WriteU32(pcbe->pAdapterContext, 0x33958, 0x80000000);
            }
            else if(IGAIndex == IGA3)
            {
                cbMMIOWriteReg32(pcbe, 0x34000, 0x1, ~0x1);
                cb_WriteU32(pcbe->pAdapterContext, 0x34058, 0x80000000);
            }
            cbWaitVSync(pcbe, IGAIndex);
        }
    }

    cbTraceExit(GENERIC);

    return CBIOS_OK;
}

CBIOS_STATUS cbHWSetIgaOnOffState(PCBIOS_VOID pvcbe, CBIOS_BOOL status, CBIOS_U8 IGAIndex)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    REG_CRFC_B RegCRB_FCValue;
    REG_CRFC_B RegCRB_FCMask;
    REG_SR0B RegSR0BValue;
    REG_SR0B RegSR0BMask;

    cbTraceEnter(GENERIC);

    if(status)
    {
        if (IGAIndex == IGA1)
        {
            RegCRB_FCValue.Value = 0;
            RegCRB_FCValue.PLL_DCLK1_POWER_DOWN = 0;
            RegCRB_FCMask.Value = 0xFF;
            RegCRB_FCMask.PLL_DCLK1_POWER_DOWN = 0;
            cbMMIOWriteReg(pcbe,CR_B_FC, RegCRB_FCValue.Value, RegCRB_FCMask.Value);
        }
        else if(IGAIndex == IGA2)
        {
            RegSR0BValue.Value = 0;
            RegSR0BValue.DCLK2_Power_Down = 0;
            RegSR0BMask.Value = 0xFF;
            RegSR0BMask.DCLK2_Power_Down = 0;
            cbMMIOWriteReg(pcbe,SR_0B, RegSR0BValue.Value, RegSR0BMask.Value);
        }
        else if(IGAIndex == IGA3)
        {
            RegCRB_FCValue.Value = 0;
            RegCRB_FCValue.PLL_DCLK3_Power_Down = 0;
            RegCRB_FCMask.Value = 0xFF;
            RegCRB_FCMask.PLL_DCLK3_Power_Down = 0;
            cbMMIOWriteReg(pcbe,CR_B_FC, RegCRB_FCValue.Value, RegCRB_FCMask.Value);
        }
    }
    else
    {
        if (IGAIndex == IGA1)
        {
            RegCRB_FCValue.Value = 0;
            RegCRB_FCValue.PLL_DCLK1_POWER_DOWN = 1;
            RegCRB_FCMask.Value = 0xFF;
            RegCRB_FCMask.PLL_DCLK1_POWER_DOWN = 0;
            cbMMIOWriteReg(pcbe,CR_B_FC, RegCRB_FCValue.Value, RegCRB_FCMask.Value);
        }
        else if(IGAIndex == IGA2)
        {
            RegSR0BValue.Value = 0;
            RegSR0BValue.DCLK2_Power_Down = 1;
            RegSR0BMask.Value = 0xFF;
            RegSR0BMask.DCLK2_Power_Down = 0;
            cbMMIOWriteReg(pcbe,SR_0B, RegSR0BValue.Value, RegSR0BMask.Value);
        }
        else if(IGAIndex == IGA3)
        {
            RegCRB_FCValue.Value = 0;
            RegCRB_FCValue.PLL_DCLK3_Power_Down = 1;
            RegCRB_FCMask.Value = 0xFF;
            RegCRB_FCMask.PLL_DCLK3_Power_Down = 0;
            cbMMIOWriteReg(pcbe,CR_B_FC, RegCRB_FCValue.Value, RegCRB_FCMask.Value);
        }
    }

    if(pcbe->ChipID >= CHIPID_CHX004)
    {
        cbSetDFSConfig(pcbe);
    }

    cbTraceExit(GENERIC);

    return CBIOS_OK;
}

CBIOS_VOID cbSetDFSConfig(PCBIOS_VOID pvcbe)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    CBIOS_MODULE_INDEX eDPModuleIndex = CBIOS_MODULE_INDEX_INVALID;
    REG_CRFC_B RegCRB_FCValue;
    REG_SR0B RegSR0BValue;
    CBIOS_U8 Dclk1Off = 1;
    CBIOS_U8 Dclk2Off = 1;
    CBIOS_U8 Dclk3Off = 1;

    if (pcbe->SysBiosInfo.Version >= 0x11)
    {
        if(pcbe->SysBiosInfo.Dp1PortConnType == CBIOS_EDP_CONN)
        {
            eDPModuleIndex = CBIOS_MODULE_INDEX1;
        }
        else if(pcbe->SysBiosInfo.Dp2PortConnType == CBIOS_EDP_CONN)
        {
            eDPModuleIndex = CBIOS_MODULE_INDEX2;
        }
        else if(pcbe->SysBiosInfo.Dp3PortConnType == CBIOS_EDP_CONN)
        {
            eDPModuleIndex = CBIOS_MODULE_INDEX3;
        }
    }
    else
    {
        if(pcbe->FeatureSwitch.IsEDP1Enabled)
        {
            eDPModuleIndex = CBIOS_MODULE_INDEX1;
        }
        else if(pcbe->FeatureSwitch.IsEDP2Enabled)
        {
            eDPModuleIndex = CBIOS_MODULE_INDEX2;
        }
        else if(pcbe->FeatureSwitch.IsEDP3Enabled)
        {
            eDPModuleIndex = CBIOS_MODULE_INDEX3;
        }
    }

    RegCRB_FCValue.Value = cb_ReadU8(pcbe->pAdapterContext, 0x89fc);
    RegSR0BValue.Value = cb_ReadU8(pcbe->pAdapterContext, 0x860b);

    Dclk1Off = RegCRB_FCValue.PLL_DCLK1_POWER_DOWN;
    Dclk2Off = RegSR0BValue.DCLK2_Power_Down;
    Dclk3Off = RegCRB_FCValue.PLL_DCLK3_Power_Down;

    if(pcbe->ChipID == CHIPID_CHX004)
    {
        if((!Dclk1Off) && Dclk2Off && Dclk3Off) //gate on VBLANK and VBLANK select IGA1
        {
            cbMMIOWriteReg32(pcbe, 0x81b0, 0x40000, ~0x4C000);
        }
        else if(Dclk1Off && (!Dclk2Off) && Dclk3Off) //gate on VBLANK and VBLANK select IGA2
        {
            cbMMIOWriteReg32(pcbe, 0x81b0, 0x44000, ~0x4C000);
        }
        else if(Dclk1Off && Dclk2Off && (!Dclk3Off)) //gate on VBLANK and VBLANK select IGA3
        {
            cbMMIOWriteReg32(pcbe, 0x81b0, 0x48000, ~0x4C000);
        }
        else //gate off VBLANK
        {
            cbMMIOWriteReg32(pcbe, 0x81b0, 0x0, ~0x40000);
        }

        if((eDPModuleIndex == CBIOS_MODULE_INDEX1) && Dclk2Off && Dclk3Off) //when edp connect DP1, gate on UMARDY4DFS, UMARDY4DFS source select DP1
        {
            cbMMIOWriteReg32(pcbe, 0x81b0, 0x80000, ~0x90000);
        }
        else if((eDPModuleIndex == CBIOS_MODULE_INDEX2) && Dclk1Off && Dclk3Off) //when edp connect DP2, gate on UMARDY4DFS, UMARDY4DFS source select DP2
        {
            cbMMIOWriteReg32(pcbe, 0x81b0, 0x90000, ~0x90000);
        }
        else //gate off UMARDY4DFS
        {
            cbMMIOWriteReg32(pcbe, 0x81b0, 0x0, ~0x80000);
        }
    }
    else if(pcbe->ChipID == CHIPID_CNE001)
    {
        if((eDPModuleIndex == CBIOS_MODULE_INDEX1) && Dclk2Off && Dclk3Off) //when edp connect DP1, gate on UMARDY4DFS, UMARDY4DFS source select DP1
        {
            cbMMIOWriteReg32(pcbe, 0x81b0, 0x80000, ~0x98000);
        }
        else if((eDPModuleIndex == CBIOS_MODULE_INDEX2) && Dclk1Off && Dclk3Off) //when edp connect DP2, gate on UMARDY4DFS, UMARDY4DFS source select DP2
        {
            cbMMIOWriteReg32(pcbe, 0x81b0, 0x88000, ~0x98000);
        }
        else if((eDPModuleIndex == CBIOS_MODULE_INDEX3) && Dclk1Off && Dclk2Off) //when edp connect DP3, gate on UMARDY4DFS, UMARDY4DFS source select DP3
        {
            cbMMIOWriteReg32(pcbe, 0x81b0, 0x90000, ~0x98000);
        }
        else //gate off UMARDY4DFS
        {
            cbMMIOWriteReg32(pcbe, 0x81b0, 0x0, ~0x80000);
        }
    }

    //gate off the DIU_IDLE signal immediately when any clock on, or may underflow
    if(Dclk1Off && Dclk2Off && Dclk3Off)
    {
        cbMMIOWriteReg32(pcbe, 0x81b0, 0x20000, ~0x20000);  //gate on DIU_IDLE
    }
    else
    {
        cbMMIOWriteReg32(pcbe, 0x81b0, 0x0, ~0x20000);  //gate off DIU_IDLE
    }
}

CBIOS_STATUS cbHWResetBlock(PCBIOS_VOID pvcbe, CBIOS_HW_BLOCK HWBlock)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    CBIOS_GAMMA_PARA        GammaParam = {0};
    CBIOS_U32               i = 0;
    CBIOS_U32  PSVsyncOffIndex[CBIOS_IGACOUNTS] = {0x81C4, 0x33908, 0x34008, 0x34708};
    CBIOS_U32  PSStrideIndex[CBIOS_IGACOUNTS] = {0x81C8, 0x3390C, 0x3400C, 0x3470C};
    CBIOS_U32  PSShadowIndex[CBIOS_IGACOUNTS] = {0x81FC, 0x33924, 0x34024, 0x34724};
    CBIOS_U32  OneShotTrigIndex[CBIOS_IGACOUNTS] = {0x8200, 0x33958, 0x34058, 0x34758};
    CBIOS_U32  PSDstWinSizeIndex[CBIOS_IGACOUNTS] = {0x8208, 0x33A1C, 0x3411C, 0x0};
    CBIOS_U32  StreamControlIndex[CBIOS_IGACOUNTS] = {0x8180, 0x33900, 0x34000, 0x0};
    CBIOS_U32  PSStartAddrIndex[CBIOS_IGACOUNTS] = {0x81dc, 0x33988, 0x34088, 0x34788};

#ifdef CHECK_CHIPENABLE
    if (!cbHWIsChipEnable(pcbe))
        return CBIOS_ER_CHIPDISABLE;
#endif

    if(HWBlock == CBIOS_HW_IGA)
    {
        for(i = 0; i < pcbe->DispMgr.IgaCount; i++)
        {
            cbBiosMMIOWriteReg(pcbe, CR_67, 0x00, (CBIOS_U8)~0x0E, i);
            cbMMIOWriteReg32(pcbe, PSVsyncOffIndex[i], 0, ~0x10000000);
            cbMMIOWriteReg32(pcbe, PSStrideIndex[i], 0, 0);
            cbMMIOWriteReg32(pcbe, PSShadowIndex[i], 0, ~0x0003FF81);
            
            //disable gamma
            GammaParam.IGAIndex = i;
            GammaParam.Flags.bDisableGamma = 1;
            cbSetGamma(pcbe, &GammaParam);

            //fix the issue about displaying incorrectly when disable the driver
            if(pcbe->ChipID >= CHIPID_CHX004)
            {
                 //PS use CR register,when using CR register,we must disable the window mode,otherwise it will display incorrectly
                cbBiosMMIOWriteReg(pcbe, CR_64, 0x00, (CBIOS_U8)~0x40, i); 
                cbMMIOWriteReg32(pcbe, PSDstWinSizeIndex[i], 0, ~0x80000000);
                cbMMIOWriteReg32(pcbe, StreamControlIndex[i], 0, ~0x1);
                //Even the PS use CR registers after disable driver,but if the surface is located at pcie memory before disable driver,
                //it will display incorrectly,so clear the startaddress register
                cbMMIOWriteReg32(pcbe, PSStartAddrIndex[i], 0, 0);                
                cbMMIOWriteReg32(pcbe, OneShotTrigIndex[i], 1, ~0x1);

            }
            
        }

        // Clear the CRBD[7] to let BIOS control the screen on/off
        cbMMIOWriteReg(pcbe, CR_BD, 0x00, (CBIOS_U8)~0x80);
 
    }
    
    return CBIOS_OK;
}


CBIOS_STATUS cbHWDumpReg(PCBIOS_VOID pvcbe, PCBIOS_PARAM_DUMP_REG pCBParamDumpReg)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    CBIOS_S32     i;
    CBIOS_UCHAR   ch;

    cbUnlockSR(pcbe);
    cbUnlockCR(pcbe);

    cb_WriteU16(pcbe->pAdapterContext, CB_CRT_ADDR_REG, 0xA039);
    cb_WriteU8(pcbe->pAdapterContext, CB_CRT_ADDR_REG, 0x35);
    cb_WriteU8(pcbe->pAdapterContext, CB_CRT_DATA_REG,(CBIOS_U8)(cb_ReadU8(pcbe->pAdapterContext, CB_CRT_DATA_REG)&0x0F));
    cb_WriteU8(pcbe->pAdapterContext, CB_CRT_ADDR_REG, 0x11);
    cb_WriteU8(pcbe->pAdapterContext, CB_CRT_DATA_REG,(CBIOS_U8)(cb_ReadU8(pcbe->pAdapterContext, CB_CRT_DATA_REG)&0x7F));

    cbDebugPrint((MAKE_LEVEL(GENERIC, DEBUG), "\nSR registers:"));
    cbDebugPrint((MAKE_LEVEL(GENERIC, DEBUG), "\n      00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F"));
    cb_WriteU16(pcbe->pAdapterContext, CB_SEQ_ADDR_REG, 0x4026);
    cb_WriteU8(pcbe->pAdapterContext, CB_SEQ_ADDR_REG, 0x2A);
    cb_WriteU8(pcbe->pAdapterContext, CB_SEQ_DATA_REG, cb_ReadU8(pcbe->pAdapterContext, CB_SEQ_DATA_REG)&~0x08);
    for (i=0; i<SEQREGSNUM; i++)
    {
        if ((i & 0x0f)==0)
            cbDebugPrint((MAKE_LEVEL(GENERIC, DEBUG), "\n%04X ", i&0xf0));
            
        cb_WriteU8(pcbe->pAdapterContext, CB_SEQ_ADDR_REG, (CBIOS_U8)i);
        ch = cb_ReadU8(pcbe->pAdapterContext, CB_SEQ_DATA_REG);
        cbDebugPrint((MAKE_LEVEL(GENERIC, DEBUG), " %02X", ch));
    }

    cbDebugPrint((MAKE_LEVEL(GENERIC, DEBUG), "\nCR registers:"));
    cbDebugPrint((MAKE_LEVEL(GENERIC, DEBUG), "\n      00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F"));
    for (i=0; i<CRTCREGSNUM; i++)
    {
        if ((i & 0x0f)==0)
            cbDebugPrint((MAKE_LEVEL(GENERIC, DEBUG), "\n%04X ", i&0xf0));
        cb_WriteU8(pcbe->pAdapterContext, CB_CRT_ADDR_REG, (CBIOS_U8)i);
        ch = cb_ReadU8(pcbe->pAdapterContext, CB_CRT_DATA_REG);
        cbDebugPrint((MAKE_LEVEL(GENERIC, DEBUG), " %02X", ch));
    }

    cbDebugPrint((MAKE_LEVEL(GENERIC, DEBUG), "\nCR_B registers:"));
    cbDebugPrint((MAKE_LEVEL(GENERIC, DEBUG), "\n      00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F"));    
    //Switch to CR_B
    cb_WriteU8(pcbe->pAdapterContext, CB_SEQ_ADDR_REG, 0x2A);
    cb_WriteU8(pcbe->pAdapterContext, CB_SEQ_DATA_REG,(CBIOS_U8)(cb_ReadU8(pcbe->pAdapterContext, CB_SEQ_DATA_REG)|0x08));
    for (i=0xc0; i<CRTCREGSNUM; i++)
    {
        if ((i & 0x0f)==0)
            cbDebugPrint((MAKE_LEVEL(GENERIC, DEBUG), "\n%04X ", i&0xf0));
//            cbDebugPrint((DBG_LEVEL_ERROR_MSG, "\n%2x ", (i&0xf0)>>4));
        cb_WriteU8(pcbe->pAdapterContext, CB_CRT_ADDR_REG, (CBIOS_U8)i);
        ch = cb_ReadU8(pcbe->pAdapterContext, CB_CRT_DATA_REG);
        cbDebugPrint((MAKE_LEVEL(GENERIC, DEBUG), " %02X", ch));
//        cbDebugPrint((DBG_LEVEL_ERROR_MSG, " %2x", ch));
    }
    //Switch back
    cb_WriteU8(pcbe->pAdapterContext, CB_SEQ_ADDR_REG, 0x2A);
    cb_WriteU8(pcbe->pAdapterContext, CB_SEQ_DATA_REG,(CBIOS_U8)(cb_ReadU8(pcbe->pAdapterContext, CB_SEQ_DATA_REG)&~0x08));

    cbDebugPrint((MAKE_LEVEL(GENERIC, DEBUG), "\nCR_C registers:"));
    cbDebugPrint((MAKE_LEVEL(GENERIC, DEBUG), "\n      00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F"));
    //Switch to CR_C
    cb_WriteU8(pcbe->pAdapterContext, CB_SEQ_ADDR_REG, 0x2A);
    cb_WriteU8(pcbe->pAdapterContext, CB_SEQ_DATA_REG,(CBIOS_U8)(cb_ReadU8(pcbe->pAdapterContext, CB_SEQ_DATA_REG)|0x04));
    for (i=0x0; i<CRTCREGSNUM; i++)
    {
        if ((i & 0x0f)==0)
            cbDebugPrint((MAKE_LEVEL(GENERIC, DEBUG), "\n%04X ", i&0xf0));
//            cbDebugPrint((DBG_LEVEL_ERROR_MSG, "\n%2x ", (i&0xf0)>>4));
        cb_WriteU8(pcbe->pAdapterContext, CB_CRT_ADDR_REG, (CBIOS_U8)i);
        ch = cb_ReadU8(pcbe->pAdapterContext, CB_CRT_DATA_REG);
        cbDebugPrint((MAKE_LEVEL(GENERIC, DEBUG), " %02X", ch));
//        cbDebugPrint((DBG_LEVEL_ERROR_MSG, " %2x", ch));
    }
    //Switch back
    cb_WriteU8(pcbe->pAdapterContext, CB_SEQ_ADDR_REG, 0x2A);
    cb_WriteU8(pcbe->pAdapterContext, CB_SEQ_DATA_REG,(CBIOS_U8)(cb_ReadU8(pcbe->pAdapterContext, CB_SEQ_DATA_REG)&~0x04));

    cbDebugPrint((MAKE_LEVEL(GENERIC, DEBUG), "\nCR_D registers:"));
    cbDebugPrint((MAKE_LEVEL(GENERIC, DEBUG), "\n      00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F"));
    //Switch to CR_D
    cb_WriteU8(pcbe->pAdapterContext, CB_SEQ_ADDR_REG, 0x2A);
    cb_WriteU8(pcbe->pAdapterContext, CB_SEQ_DATA_REG,(CBIOS_U8)(cb_ReadU8(pcbe->pAdapterContext, CB_SEQ_DATA_REG)|0x46));
    for (i=0x0; i<CRTCREGSNUM; i++)
    {
        if ((i & 0x0f)==0)
            cbDebugPrint((MAKE_LEVEL(GENERIC, DEBUG), "\n%04X ", i&0xf0));
        //cbDebugPrint((DBG_LEVEL_ERROR_MSG, "\n%2x ", (i&0xf0)>>4));
        cb_WriteU8(pcbe->pAdapterContext, CB_CRT_ADDR_REG, (CBIOS_U8)i);
        ch = cb_ReadU8(pcbe->pAdapterContext, CB_CRT_DATA_REG);
        cbDebugPrint((MAKE_LEVEL(GENERIC, DEBUG), " %02X", ch));
        //cbDebugPrint((DBG_LEVEL_ERROR_MSG, " %2x", ch));
    }
    //Switch back
    cb_WriteU8(pcbe->pAdapterContext, CB_SEQ_ADDR_REG, 0x2A);
    cb_WriteU8(pcbe->pAdapterContext, CB_SEQ_DATA_REG,(CBIOS_U8)(cb_ReadU8(pcbe->pAdapterContext, CB_SEQ_DATA_REG)&~0x46));

    //Switch to Paired SR, CR
    cb_WriteU16(pcbe->pAdapterContext, CB_SEQ_ADDR_REG, 0x4E26);
    cbDebugPrint((MAKE_LEVEL(GENERIC, DEBUG), "\nPaired SR registers:"));
    for (i=0; i<SEQREGSNUM; i++)
    {
        if ((i & 0x0f)==0)
            cbDebugPrint((MAKE_LEVEL(GENERIC, DEBUG), "\n%04X ", i&0xf0));
//            cbDebugPrint((DBG_LEVEL_ERROR_MSG, "\n%2x ", (i&0xf0)>>4));
        cb_WriteU8(pcbe->pAdapterContext, CB_SEQ_ADDR_REG, (CBIOS_U8)i);
        ch = cb_ReadU8(pcbe->pAdapterContext, CB_SEQ_DATA_REG);
        cbDebugPrint((MAKE_LEVEL(GENERIC, DEBUG), " %02X", ch));
//        cbDebugPrint((DBG_LEVEL_ERROR_MSG, " %2x", ch));
    }

    cbDebugPrint((MAKE_LEVEL(GENERIC, DEBUG), "\nPaired CR registers:"));
    for (i=0; i<CRTCREGSNUM; i++)
    {
        if ((i & 0x0f)==0)
            cbDebugPrint((MAKE_LEVEL(GENERIC, DEBUG), "\n%04X ", i&0xf0));
//            cbDebugPrint((DBG_LEVEL_ERROR_MSG, "\n%2x ", (i&0xf0)>>4));
        cb_WriteU8(pcbe->pAdapterContext, CB_CRT_ADDR_REG, (CBIOS_U8)i);
        ch = cb_ReadU8(pcbe->pAdapterContext, CB_CRT_DATA_REG);
        cbDebugPrint((MAKE_LEVEL(GENERIC, DEBUG), " %02X", ch));
//        cbDebugPrint((DBG_LEVEL_ERROR_MSG, " %2x", ch));
    }
    //Switch back
    cb_WriteU16(pcbe->pAdapterContext, CB_SEQ_ADDR_REG, 0x4026);
    return CBIOS_OK;

}


CBIOS_STATUS cbHWReadReg(PCBIOS_VOID pvcbe, CBIOS_U8 type, CBIOS_U8 index, PCBIOS_U8 result)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;

    *result = cbMMIOReadReg(pcbe, (((CBIOS_U16)type<<8)|index));
    return CBIOS_OK;
}

CBIOS_STATUS cbHWWriteReg(PCBIOS_VOID pvcbe, CBIOS_U8 type, CBIOS_U8 index, CBIOS_U8 value, CBIOS_U8 mask)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;

    cbMMIOWriteReg(pcbe, (((CBIOS_U16)type<<8)|index), value, mask);
    return CBIOS_OK;
}

CBIOS_STATUS cbHWGetClockByType(PCBIOS_VOID pvcbe, PCBios_GetClock_Params pCBiosGetClockParams)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    cbGetProgClock(pcbe, pCBiosGetClockParams->ClockFreq, pCBiosGetClockParams->ClockType);

    // change ClockFreq unit to KHz
    *pCBiosGetClockParams->ClockFreq /= 10;

    return CBIOS_OK;
}


//in func cbHWSetClockByType we can set ICLK and VCLK now.
CBIOS_STATUS cbHWSetClockByType(PCBIOS_VOID pvcbe, PCBios_SetClock_Params pCBiosSetClockParams)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    CBIOS_STATUS status = CBIOS_OK;
    switch(pCBiosSetClockParams->ClockType)
    {
    case CBIOS_ICLKTYPE:
        cbProgClock(pcbe, pCBiosSetClockParams->ClockFreq, pCBiosSetClockParams->ClockType, IGA1);
        break;
    case CBIOS_ECLKTYPE:
        cbProgClock(pcbe, pCBiosSetClockParams->ClockFreq, pCBiosSetClockParams->ClockType, IGA1);
        break;

    case CBIOS_CPUFRQTYPE:
        cbProgClock(pcbe, pCBiosSetClockParams->ClockFreq, pCBiosSetClockParams->ClockType, IGA1);
        break;
        
    case CBIOS_VCLKTYPE:
        cbProgClock(pcbe, pCBiosSetClockParams->ClockFreq, pCBiosSetClockParams->ClockType, IGA1);
        break;

    case CBIOS_DCLK1TYPE:
        cbProgramDclk(pcbe, IGA1, pCBiosSetClockParams->ClockFreq);
        break;

    case CBIOS_DCLK2TYPE:
        cbProgramDclk(pcbe, IGA2, pCBiosSetClockParams->ClockFreq);
        break;
        
    case CBIOS_DCLK3TYPE:
        cbProgramDclk(pcbe, IGA3, pCBiosSetClockParams->ClockFreq);
        break;

    case CBIOS_DCLK4TYPE:
        cbProgramDclk(pcbe, IGA4, pCBiosSetClockParams->ClockFreq);
        break;
        
    default:
        cbDebugPrint((MAKE_LEVEL(GENERIC, ERROR),"cbHWSetClockByType: Your ClockType error.\n"));
        status = CBIOS_ER_INVALID_PARAMETER;
        break;
    }
    return status;
}

static CBREGISTER_IDX PCIDeviceRevTable[] = {
    {   CR_C_82, 0xff},  //CR82_C
    {   CR_C_83, 0xff},  //CR83_C
    {   MAPMASK_EXIT},
};

static CBREGISTER_IDX PCIDeviceIDTable[] = {
    {   CR_C_81, 0xFF},    //CR81_C
    {   CR_C_80, 0xFF},   //CR80_C
    {   MAPMASK_EXIT},
};

CBIOS_STATUS  cbHwSyncDataWithVbios(PCBIOS_VOID  pvcbe, PCBIOS_VBIOS_DATA_PARAM  pDataPara)
{
    CBIOS_U32   i;
    CBIOS_U32    ActiveDevices = 0;
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    PCBIOS_DEVICE_COMMON    pDevCommon = CBIOS_NULL;
    CBIOS_GET_DEV_COMB      GetDevComb = {0};
    CBIOS_DEVICE_COMB       DeviceComb = {0};
    CBIOS_MODULE_INDEX  ModuleIndex;
    PCBIOS_DP_CONTEXT pDpContext = CBIOS_NULL;

#ifdef CHECK_CHIPENABLE
    if (!cbHWIsChipEnable(pcbe))
        return CBIOS_ER_CHIPDISABLE;
#endif

    if(pDataPara->SyncToVbios)
    {
        return CBIOS_OK;
    }
    
    ActiveDevices = cbHwGetActDeviceFromReg(pcbe);
    ActiveDevices &= (~CBIOS_TYPE_DUOVIEW);

    if(!ActiveDevices)
    {
        return CBIOS_OK;
    }
    
    //update active device from hibernation using CR6B,CR6C    
    GetDevComb.Size = sizeof(CBIOS_GET_DEV_COMB);
    GetDevComb.pDeviceComb = &DeviceComb;
    DeviceComb.Devices = ActiveDevices;
    cbPathMgrGetDevComb(pcbe, &GetDevComb);
    pcbe->DispMgr.ActiveDevices[IGA1] = DeviceComb.Iga1Dev;
    pcbe->DispMgr.ActiveDevices[IGA2] = DeviceComb.Iga2Dev;
    pcbe->DispMgr.ActiveDevices[IGA3] = DeviceComb.Iga3Dev;
    pcbe->DispMgr.ActiveDevices[IGA4] = DeviceComb.Iga4Dev;
    
    for (i = 0; i < CBIOS_IGACOUNTS; i++)
    {
        if (!pcbe->DispMgr.ActiveDevices[i])
        {
            continue;
        }
        
        pDevCommon = cbGetDeviceCommon(&pcbe->DeviceMgr, pcbe->DispMgr.ActiveDevices[i]);
        if (!pDevCommon)
        {
            continue;
        }
        
        pDevCommon->PowerState = CBIOS_PM_ON;
        if(pDevCommon->PortConnType == CBIOS_EDP_CONN)
        {
            pDpContext = container_of(pDevCommon, PCBIOS_DP_CONTEXT, Common);
            if(pDpContext)
            {
                pDpContext->DPMonitorContext.VddStatus = 1;
            }
        }
        //to generate path info from vbios
        pDevCommon->DispSource.ModuleList.IGAModule.Index = i;
        if(pDevCommon->DeviceType & ALL_DP_TYPES)
        {
            ModuleIndex = cbGetModuleIndex(pcbe, pDevCommon->DeviceType, CBIOS_MODULE_TYPE_DP);
            if(cbDIU_HDMI_IsOn(pcbe, ModuleIndex))
            {
                pDevCommon->DispSource.ModuleList.HDMIModule.Index = ModuleIndex;
            }
            else
            {
                pDevCommon->DispSource.ModuleList.HDMIModule.Index = CBIOS_MODULE_INDEX_INVALID;
            }

            if(cbDIU_HDTV_IsOn(pcbe, ModuleIndex))
            {
                pDevCommon->DispSource.ModuleList.HDTVModule.Index = ModuleIndex;
            }
            else
            {
                pDevCommon->DispSource.ModuleList.HDTVModule.Index = CBIOS_MODULE_INDEX_INVALID;
            }
        }
        cbPathMgrGeneratePath(pcbe, pDevCommon->DeviceType);
    }   
    
    return CBIOS_OK;
}

CBIOS_STATUS cbHWGetVBiosInfo(PCBIOS_VOID pvcbe, PCBIOS_VBINFO_PARAM  pVbiosInfo)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    CBIOS_U32 size = 0;

    if(CBIOS_NULL == pVbiosInfo)
    {
        cbDebugPrint((MAKE_LEVEL(GENERIC, ERROR),("cbHWGetVBiosInfo: pVbiosInfo is CBIOS_NULL!\n")));
        return CBIOS_ER_NULLPOINTER;
    }
   
    // to see if driver is older or newer than cbios version, get min size
    size = (pVbiosInfo->Size < sizeof(CBIOS_VBINFO_PARAM)) ? 
            pVbiosInfo->Size : sizeof(CBIOS_VBINFO_PARAM);

    // clear ELD buffer with min size
    cb_memset(pVbiosInfo, 0, size);
    
    pVbiosInfo->Size = size;

    pVbiosInfo->BiosVersion = pcbe->BiosVersion;

    pVbiosInfo->FBSize = 1 << (pcbe->SysBiosInfo.FBSize & 0xF); 
        

    //Get Revision ID / Device ID
    pVbiosInfo->RevisionID = cbMapMaskRead(pcbe,PCIDeviceRevTable, CBIOS_NOIGAENCODERINDEX) & 0xFFFF;
    pVbiosInfo->DeviceID = cbMapMaskRead(pcbe,PCIDeviceIDTable, CBIOS_NOIGAENCODERINDEX) & 0xFFFF;

    pVbiosInfo->SupportDev = pcbe->DeviceMgr.SupportDevices;
    pVbiosInfo->PortSplitSupport = pcbe->PortSplitSupport & pVbiosInfo->SupportDev;
    pVbiosInfo->HPDDevicesMask = pcbe->HPDDeviceMasks;
    pVbiosInfo->BootDevInCMOS = pcbe->SysBiosInfo.BootUpDev;	
    pVbiosInfo->PollingDevMask = 0;
    if (pcbe->DeviceMgr.SupportDevices & CBIOS_TYPE_CRT)
    {
        pVbiosInfo->PollingDevMask |= CBIOS_TYPE_CRT;
    }

    pVbiosInfo->bNoHDCPSupport = CBIOS_FALSE;

    if (pVbiosInfo->Size >= SIZEOF_STRUCT_TILL_MEMBER(pVbiosInfo, TotalMemSize))
    {
        if (pcbe->ChipID >= CHIPID_CHX004)
        {
            pVbiosInfo->FBSize = 1 << (pcbe->SysBiosInfo.FBSize & 0xF);
            pVbiosInfo->AvalMemSize = pVbiosInfo->FBSize;
            pVbiosInfo->TotalMemSize = pVbiosInfo->FBSize;
            pVbiosInfo->LowTopAddress = (CBIOS_U32)((pcbe->SysBiosInfo.LowTopAddress)<<16);
            pVbiosInfo->SnoopOnly = pcbe->SysBiosInfo.SnoopOnly;
            pVbiosInfo->bTAEnable = pcbe->SysBiosInfo.bTAEnable;
            pVbiosInfo->bBLGfxMode = pcbe->SysBiosInfo.bBLGfxMode;
            pVbiosInfo->AddressCHA = pcbe->SysBiosInfo.AddressCHA;
            pVbiosInfo->AddressCHB = pcbe->SysBiosInfo.AddressCHB;
        }
        else
        {
            cbHwGetMemInfo(pcbe, pVbiosInfo);
            pVbiosInfo->FBSize = pVbiosInfo->AvalMemSize;
        }
    }
    
    if(pcbe->ChipID == CHIPID_E3K || pcbe->ChipID == CHIPID_CHX004 || pcbe->ChipID == CHIPID_CNE001)
    {
        cb_memcpy(pVbiosInfo->PMPVer,pcbe->PMPVer,sizeof(pcbe->PMPVer));    
    }

    return CBIOS_OK;
}

CBIOS_STATUS cbHwI2CCheckStatus(PCBIOS_VOID pvcbe, CBIOS_U8 I2CBusNum)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    PCBIOS_VOID pAdapter = pcbe->pAdapterContext;
    CBIOS_U32   HdcpReg = 0;
    CBIOS_U8  HDCP_Port1 = 0,HDCP_Port2 = 0;
    CBIOS_U8  Temp = 0, Temp1 = 0;
    CBIOS_STATUS  bRet = CBIOS_OK;

    Temp = cbMMIOReadReg(pcbe, CR_AA); //HDCP1
    Temp1 = cbMMIOReadReg(pcbe, CR_B_C6);//HDCP2
    HDCP_Port1 = (((Temp >> 5) & 0x01) << 2) | (Temp >> 6);
    HDCP_Port2 = (((Temp1 >> 5) & 0x01) << 2) | (Temp >> 6);

    if((HDCP_Port1 == I2CBusNum) && (cb_ReadU32(pAdapter, HDCPCTL2_DEST) & HDCP_I2C_ENABLE_DEST))
    {
        HdcpReg = HDCPCTL2_DEST;
    }
    else if((HDCP_Port2 == I2CBusNum) && (cb_ReadU32(pAdapter, HDCP2_CTL2_DEST) & HDCP_I2C_ENABLE_DEST))
    {
        HdcpReg = HDCP2_CTL2_DEST;
    }

    bRet = (HdcpReg != 0) ? CBIOS_ER_HDCP_USING_I2C : CBIOS_OK;

    return bRet;
}

CBIOS_BOOL cbHWDumpInfo(PCBIOS_VOID pvcbe, CBIOS_DUMP_TYPE DumpType)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    
    if (DumpType & CBIOS_DUMP_VCP_INFO)
    {
        cbDebugPrint((MAKE_LEVEL(GENERIC, DEBUG),"can't dump VCP info!\n"));
        DumpType &= (~CBIOS_DUMP_VCP_INFO); 
    }
    if (DumpType & CBIOS_DUMP_MODE_INFO)
    {
        cbDumpModeInfo(pcbe);
        DumpType &= (~CBIOS_DUMP_MODE_INFO); 
    }
    if (DumpType & CBIOS_DUMP_CLOCK_INFO)
    {
        cbDumpClockInfo(pcbe);
        DumpType &= (~CBIOS_DUMP_CLOCK_INFO); 
    }

    cbDumpRegisters(pcbe, DumpType);

    return CBIOS_TRUE;
}


CBIOS_STATUS cbHWSetWriteback(PCBIOS_VOID pvcbe, PCBIOS_WB_PARA pWBPara)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;

    return cbSetWriteback(pcbe, pWBPara);
}

CBIOS_STATUS cbHWCECTransmitMessage(PCBIOS_VOID pvcbe, PCBIOS_CEC_MESSAGE pCECMessage, CBIOS_CEC_INDEX CECIndex)
{
    CBIOS_U32       CECMiscReg1Index = 0, CECMiscReg2Index = 0, CECInitiatorCmdRegIndex = 0;
    CEC_MISC_REG1   CECMiscReg1;
    CEC_MISC_REG2   CECMiscReg2;
    CBIOS_STATUS    Status = CBIOS_OK;
    CBIOS_U32       CMDData = 0;
    CBIOS_U8        i = 0;
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;

    if (pCECMessage == CBIOS_NULL)
    {
        Status = CBIOS_ER_NULLPOINTER;
        cbDebugPrint((MAKE_LEVEL(HDMI, ERROR), "cbHWCECTransmitMessage: pCECMessage is NULL!"));
    }
    else if (!pcbe->ChipCaps.IsSupportCEC)
    {
        Status = CBIOS_ER_HARDWARE_LIMITATION;
        cbDebugPrint((MAKE_LEVEL(HDMI, ERROR), "cbHWCECTransmitMessage: Can't support CEC!"));
    }
    else if (CECIndex >= CBIOS_CEC_INDEX_COUNT)
    {
        Status = CBIOS_ER_INVALID_PARAMETER;
        cbDebugPrint((MAKE_LEVEL(HDMI, ERROR), "cbHWCECTransmitMessage: invalid CEC index!"));
    }
    else if (!pcbe->CECPara[CECIndex].CECEnable)
    {
        Status = CBIOS_ER_INVALID_PARAMETER;
        cbDebugPrint((MAKE_LEVEL(HDMI, ERROR), "cbHWCECTransmitMessage: CEC module is not enabled!"));
    }
    else
    {
        CECMiscReg1Index = CECMiscReg1Tab[CECIndex];
        CECInitiatorCmdRegIndex = CECInitiatorCmdTab[CECIndex];
        CECMiscReg2Index = CECMiscReg2Tab[CECIndex];

        //load command
        //write consequently, 4 bytes 1 time.
        //Byte0 | Byte1 | Byte2 | Byte3
        if (pCECMessage->CmdLen != 0)
        {
            for (i = 0; i < pCECMessage->CmdLen / 4; i++)
            {
                CMDData = ((CBIOS_U32)pCECMessage->Command[i * 4] << 24) + ((CBIOS_U32)pCECMessage->Command[i * 4 + 1] << 16)
                    + ((CBIOS_U32)pCECMessage->Command[i * 4 + 2] << 8) + (CBIOS_U32)pCECMessage->Command[i * 4 + 3];

                //MM3314c/MM33190 has 4 dwords. Every read/write operation will increase register index by 1
                //so remember that DO NOT add any other r/w operations here!!!
                cb_WriteU32(pcbe->pAdapterContext, CECInitiatorCmdRegIndex, CMDData);
            }
            //get remained bytes
            if ((pCECMessage->CmdLen % 4) != 0)
            {
                CMDData = 0;
                for (i = 0; i < pCECMessage->CmdLen % 4; i++)
                {
                    CMDData |= (CBIOS_U32)pCECMessage->Command[i] << ((3 - i) * 8);
                }
                //MM3314c/MM33190 has 4 dwords. Every read/write operation will increase register index by 1
                //so remember that DO NOT add any other r/w operations here!!!
                cb_WriteU32(pcbe->pAdapterContext, CECInitiatorCmdRegIndex, CMDData);

            }

        }

        //clear Follower_Received_Ready to receive next command
        //if we have received a message but not handled before transmit,
        //Follower_Received_Ready will be set, then we may miss the reply message
        cb_memset(&CECMiscReg2, 0, sizeof(CECMiscReg2));
        CECMiscReg2.FolReceiveReady = 0;
        cbMMIOWriteReg32(pcbe, CECMiscReg2Index, CECMiscReg2.CECMiscReg2Value, ~BIT13);

        cb_memset(&CECMiscReg1, 0, sizeof(CECMiscReg1));

        //logical address
        CECMiscReg1.DeviceAddr = pCECMessage->SourceAddr;

        //config retry times
        CECMiscReg1.IniRetryCnt = pCECMessage->RetryCnt;

        //set initiator destination address
        CECMiscReg1.IniDestAddr = pCECMessage->DestAddr;

        //set initiator command length
        CECMiscReg1.IniCmdLen = pCECMessage->CmdLen;

        //set initiator command type
        if (pCECMessage->bBroadcast)
        {
            CECMiscReg1.IniBroadcast = 1;
        }
        else
        {
            CECMiscReg1.IniBroadcast = 0;
        }


        //start transmission
        CECMiscReg1.IniCmdAvailable = 1;
        cbMMIOWriteReg32(pcbe, CECMiscReg1Index, CECMiscReg1.CECMiscReg1Value, 0xFFF60000);


        //wait for transmit done
        for (i = 0; i < 100; i++)
        {
            
            CECMiscReg2.CECMiscReg2Value = cb_ReadU32(pcbe->pAdapterContext, CECMiscReg2Index);
            if (CECMiscReg2.IniTransFinish && CECMiscReg2.IniTransSucceed)
            {
                break;
            }
            else
            {
                cb_DelayMicroSeconds(4000);
                //we'd better use sleep instead of delay here to avoid kernel soft lock error
                //cbSleepMilliSeconds(10000); 
            }
        }
        if (CECMiscReg2.IniTransFinish && CECMiscReg2.IniTransSucceed)
        {
            Status = CBIOS_OK;
        }
        else
        {
            cbDebugPrint((MAKE_LEVEL(HDMI, ERROR), "cbHWCECTransmitMessage: Message transmission fail!\n"));
            Status = CBIOS_ER_INTERNAL;
        }

        //clear transmit finish and succeed bits
        CECMiscReg2.IniTransFinish = 0;
        CECMiscReg2.IniTransSucceed = 0;
        cbMMIOWriteReg32(pcbe, CECMiscReg2Index, CECMiscReg2.CECMiscReg2Value, 0xFFFFFFFC);

    }



    return Status;

}


CBIOS_STATUS cbHWCECReceiveMessage(PCBIOS_VOID pvcbe, PCBIOS_CEC_MESSAGE pCECMessage, CBIOS_CEC_INDEX CECIndex)
{
    CBIOS_U32       CECMiscReg2Index = 0, CECFollowerCmdRegIndex = 0;
    CEC_MISC_REG2   CECMiscReg2;
    CBIOS_STATUS    Status = CBIOS_OK;
    CBIOS_U32       CMDData = 0;
    CBIOS_U8        i = 0;
    CBIOS_U8        *pTmpCommand = CBIOS_NULL;
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;

    if (pCECMessage == CBIOS_NULL)
    {
        Status = CBIOS_ER_NULLPOINTER;
        cbDebugPrint((MAKE_LEVEL(HDMI, ERROR), "cbHWCECReceiveMessage: pCECMessage is NULL!"));
    }
    else if (!pcbe->ChipCaps.IsSupportCEC)
    {
        Status = CBIOS_ER_HARDWARE_LIMITATION;
        cbDebugPrint((MAKE_LEVEL(HDMI, ERROR), "cbHWCECReceiveMessage: Can't support CEC!"));
    }
    else if (CECIndex >= CBIOS_CEC_INDEX_COUNT)
    {
        Status = CBIOS_ER_INVALID_PARAMETER;
        cbDebugPrint((MAKE_LEVEL(HDMI, ERROR), "cbHWCECReceiveMessage: invalid CEC index!"));
    }
    else if (!pcbe->CECPara[CECIndex].CECEnable)
    {
        Status = CBIOS_ER_INVALID_PARAMETER;
        cbDebugPrint((MAKE_LEVEL(HDMI, ERROR), "cbHWCECReceiveMessage: CEC module is not enabled!"));
    }
    else
    {

        //CECMiscReg1Index = CECMiscReg1Tab[CECIndex];
        CECMiscReg2Index = CECMiscReg2Tab[CECIndex];
        CECFollowerCmdRegIndex = CECFollowerCmdTab[CECIndex];

        //check whether a transaction has been successfully received.
        CECMiscReg2.CECMiscReg2Value = cb_ReadU32(pcbe->pAdapterContext, CECMiscReg2Index);
        if (CECMiscReg2.FolReceiveReady)
        {
            //The source address received by CEC Follower.
            pCECMessage->SourceAddr = (CBIOS_U8)CECMiscReg2.FolSrcAddr;

            //transaction type 
            pCECMessage->bBroadcast = CECMiscReg2.FolBroadcast == 1 ? CBIOS_TRUE : CBIOS_FALSE;

            //Follower received command length
            pCECMessage->CmdLen = CECMiscReg2.FolCmdLen;

            //get command data
            //write consequently, 4 bytes 1 time.
            //Byte0 | Byte1 | Byte2 | Byte3
            cb_memset(pCECMessage->Command, 0, sizeof(pCECMessage->Command));
            pTmpCommand = pCECMessage->Command;

            if (pCECMessage->CmdLen != 0)
            {
                for (i = 0; i < pCECMessage->CmdLen / 4; i++)
                {
                    CMDData = cb_ReadU32(pcbe->pAdapterContext, CECFollowerCmdRegIndex);
                    *(pTmpCommand++) = (CBIOS_U8)((CMDData >> 24) & 0x000000FF);
                    *(pTmpCommand++) = (CBIOS_U8)((CMDData >> 16) & 0x000000FF);
                    *(pTmpCommand++) = (CBIOS_U8)((CMDData >> 8) & 0x000000FF);
                    *(pTmpCommand++) = (CBIOS_U8)(CMDData & 0x000000FF);

                }
                //get remained bytes
                if ((pCECMessage->CmdLen % 4) != 0)
                {
                    CMDData = cb_ReadU32(pcbe->pAdapterContext, CECFollowerCmdRegIndex);
                    for (i = 0; i < pCECMessage->CmdLen % 4; i++)
                    {
                        *(pTmpCommand++) = (CBIOS_U8)((CMDData >> ((3 - i) * 8)) & 0x000000FF);
                    }
                }
            }

            Status = CBIOS_OK;

        }
        else
        {
            cbDebugPrint((MAKE_LEVEL(HDMI, ERROR), "cbHWCECReceiveMessage: Message receive fail!\n"));
            Status = CBIOS_ER_INTERNAL;
        }

        //clear received command
        CECMiscReg2.FolReceiveReady = 0;
        cbMMIOWriteReg32(pcbe, CECMiscReg2Index, CECMiscReg2.CECMiscReg2Value, ~BIT13);


    }

    return Status;

}

CBIOS_VOID cbHwUpdateActDeviceToReg(PCBIOS_VOID pvcbe, PCBIOS_DISPLAY_MANAGER pDispMgr)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    cbIGA_UpdateActiveDeviceToReg(pcbe, pDispMgr);
}

CBIOS_U32 cbHwGetActDeviceFromReg(PCBIOS_VOID pvcbe)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    return cbIGA_GetActiveDeviceFromReg(pcbe);
}

CBIOS_VOID cbHwSetModeToIGA(PCBIOS_VOID pvcbe, PCBIOS_DISP_MODE_PARAMS pModeParams)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;

    cbDisableStream(pcbe, pModeParams->IGAIndex);

    cbIGA_HW_SetMode(pcbe, pModeParams);
}

#define MIN_MODE_X  640
#define MIN_MODE_Y  480

CBIOS_STATUS cbHwGetCounter(PCBIOS_VOID  pvcbe, PCBIOS_GET_HW_COUNTER  pGetCounter)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    PCBIOS_DISPLAY_MANAGER   pDispMgr = &pcbe->DispMgr;
    PCBIOS_DISP_MODE_PARAMS  pModeParams = CBIOS_NULL; 
    CBIOS_U32   FrameCntIndex = 0, FrameCntValue = 0;
    CBIOS_U32   PixelLineCntIndex = 0, PixelLineCntValue = 0;
    CBIOS_BOOL  bModeValid = CBIOS_FALSE;
    CBIOS_U8    byCR34Value = 0;

    if(pGetCounter->IgaIndex >= pDispMgr->IgaCount)
    {
        return  CBIOS_ER_INVALID_PARAMETER;
    }

    pModeParams = pDispMgr->pModeParams[pGetCounter->IgaIndex];

    if( pModeParams && pModeParams ->TargetTiming.XRes >= MIN_MODE_X  && pModeParams->TargetTiming.YRes >= MIN_MODE_Y)
    {
        bModeValid = CBIOS_TRUE;
    }

    if(pGetCounter->IgaIndex == IGA1)
    {
        FrameCntIndex = 0x8220;
        PixelLineCntIndex = 0x8224;
    }
    else if(pGetCounter->IgaIndex == IGA2)
    {
        FrameCntIndex = 0x8230;
        PixelLineCntIndex = 0x8234;
    }
    else if(pGetCounter->IgaIndex == IGA3)
    {
        FrameCntIndex = 0x34014;
        PixelLineCntIndex = 0x34018;
    }
    else
    {
        FrameCntIndex = 0x34714;
        PixelLineCntIndex = 0x34718;
    }

    FrameCntValue = cb_ReadU32(pcbe->pAdapterContext, FrameCntIndex);
    PixelLineCntValue = cb_ReadU32(pcbe->pAdapterContext, PixelLineCntIndex);
    byCR34Value = cbBiosMMIOReadReg(pcbe, CR_34, pGetCounter->IgaIndex);

    if(pGetCounter->bGetFrameCnt)
    {
        pGetCounter->Value[CBIOS_COUNTER_FRAME] = FrameCntValue & 0xFFFF;
    }
    
    if(pGetCounter->bGetLineCnt)
    {
        pGetCounter->Value[CBIOS_COUNTER_LINE] = PixelLineCntValue & 0xFFFF;
        if(bModeValid)
        {
            pGetCounter->Value[CBIOS_COUNTER_LINE] +=  pModeParams->TargetTiming.VerSyncStart;
            if(pGetCounter->Value[CBIOS_COUNTER_LINE] >=  pModeParams->TargetTiming.VerTotal)
            {
                pGetCounter->Value[CBIOS_COUNTER_LINE] -=  pModeParams->TargetTiming.VerTotal;
            }
        }
    }
    
    if(pGetCounter->bGetPixelCnt)
    {
        pGetCounter->Value[CBIOS_COUNTER_PIXEL] = (PixelLineCntValue >> 16) & 0xFFFF;
        if(bModeValid)
        {
            pGetCounter->Value[CBIOS_COUNTER_PIXEL] +=  pModeParams->TargetTiming.HorSyncStart;
            if(pGetCounter->Value[CBIOS_COUNTER_PIXEL] >=  pModeParams->TargetTiming.HorTotal)
            {
                pGetCounter->Value[CBIOS_COUNTER_PIXEL] -=  pModeParams->TargetTiming.HorTotal;
            }
        }
    }

    pGetCounter->bInVblank = (byCR34Value & VBLANK_ACTIVE_CR34)? 1 : 0;

    return  CBIOS_OK;
}


CBIOS_STATUS cbHwGetMemInfo(PCBIOS_VOID pvcbe, PCBIOS_VBINFO_PARAM pVbiosInfo)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    CBIOS_U32    MM49024RegValue = 0, MMD374RegValue = 0;
    CBIOS_U32    ChannelSize, ChannelNum;
    CBIOS_STATUS status = CBIOS_OK;

    MM49024RegValue = cb_ReadU32(pcbe->pAdapterContext, 0x49024);
    MMD374RegValue  = cb_ReadU32(pcbe->pAdapterContext, 0xD374);

    if ((MMD374RegValue & 0xFF) == 0)
    {
        ChannelNum = 3 - cbGetBitsNum(MM49024RegValue & 0x07);
        ChannelSize = 4096;    // hardcode 4G for old PMP
    }
    else
    {
        ChannelNum = (MMD374RegValue & 0xF0) >> 4;
        ChannelSize = (MMD374RegValue & 0x0F) * 1024;
    }
        
    switch(ChannelNum)
    {
    case 1:
    case 2:
        pVbiosInfo->AvalMemSize = ChannelNum * ChannelSize;
        pVbiosInfo->TotalMemSize = ChannelNum * ChannelSize;
        break;
    case 3:
        pVbiosInfo->TotalMemSize = ChannelNum * ChannelSize;
        if(pVbiosInfo->TotalMemSize == (3*1024))
        {
            pVbiosInfo->AvalMemSize = pVbiosInfo->TotalMemSize - 24;
        }
        else if(pVbiosInfo->TotalMemSize == (6*1024))
        {
            pVbiosInfo->AvalMemSize = pVbiosInfo->TotalMemSize - 72;
        }
        else if(pVbiosInfo->TotalMemSize == (12*1024))
        {
            pVbiosInfo->AvalMemSize = pVbiosInfo->TotalMemSize - 144;
        }
        else if(pVbiosInfo->TotalMemSize == (24*1024))
        {
            pVbiosInfo->AvalMemSize = pVbiosInfo->TotalMemSize - 288;
        }
        else
        {
            cbDebugPrint((MAKE_LEVEL(GENERIC, ERROR), "Total mem size %d is error!\n", pVbiosInfo->TotalMemSize));
            status = CBIOS_ER_INVALID_PARAMETER;
        }
        break;
    default:
        cbDebugPrint((MAKE_LEVEL(GENERIC, ERROR), "Mem channel number %d is error!\n", ChannelNum));
        status = CBIOS_ER_INVALID_PARAMETER;
        break;
    }
    pVbiosInfo->MemChNum = ChannelNum;

    return status;
}

CBIOS_STATUS cbHWVIPCtl(PCBIOS_VOID pvcbe, PCBIOS_VIP_CTRL_DATA pCbiosVIPCtlData)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;

    return cbVIPCtl(pcbe, pCbiosVIPCtlData);
}

