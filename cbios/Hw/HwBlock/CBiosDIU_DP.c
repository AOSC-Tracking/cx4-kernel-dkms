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
** DP hw block interface function implementation.
**
** NOTE:
**
******************************************************************************/

#include "CBiosDIU_DP.h"
#include "CBiosChipShare.h"
#include "../CBiosHwShare.h"

#define MEASURE_LT_TIME_GPIO    0

CBIOS_U32 DP_REG_MISC1[DP_MODU_NUM] = {0x8210,  0x33CA8,  0x343A8,  0x34AA8};
CBIOS_U32 DP_REG_LINK[DP_MODU_NUM] = {0x8214,  0x33CAC,  0x343AC,  0x34AAC};
CBIOS_U32 DP_REG_GEN_CTRL[DP_MODU_NUM] = {0x8218,  0x33CB0,  0x343B0,  0x34AB0};
CBIOS_U32 DP_REG_EXT_PACKET[DP_MODU_NUM] = {0x821C,  0x33CB4,  0x343B4,  0x34AB4};
CBIOS_U32 DP_REG_ENABLE[DP_MODU_NUM] = {0x8240,  0x33CB8,  0x343B8,  0x34AB8};
CBIOS_U32 DP_REG_HWIDTH_TU[DP_MODU_NUM] = {0x8244,  0x33CBC,  0x343BC,  0x34ABC};
CBIOS_U32 DP_REG_HLINE_DUR[DP_MODU_NUM] = {0x8248,  0x33CC0,  0x343C0,  0x34AC0};
CBIOS_U32 DP_REG_MISC0[DP_MODU_NUM] = {0x824C,  0x33CC4,  0x343C4,  0x34AC4};
CBIOS_U32 DP_REG_HV_TOTAL[DP_MODU_NUM] = {0x8250,  0x33CC8,  0x343C8,  0x34AC8};
CBIOS_U32 DP_REG_HV_START[DP_MODU_NUM] = {0x8254,  0x33CCC,  0x343CC,  0x34ACC};
CBIOS_U32 DP_REG_HV_SYNC[DP_MODU_NUM] = {0x8258,  0x33CD0,  0x343D0,  0x34AD0};
CBIOS_U32 DP_REG_HV_ACTIVE[DP_MODU_NUM] = {0x825C,  0x33CD4,  0x343D4,  0x34AD4};
CBIOS_U32 DP_REG_EPHY_CTRL[DP_MODU_NUM] = {0x82CC,  0x33CD8,  0x343D8,  0x34AD8};
CBIOS_U32 DP_REG_AUX_WRITE0[DP_MODU_NUM] = {0x8310,  0x33CDC,  0x343DC,  0x34ADC};
CBIOS_U32 DP_REG_AUX_WRITE1[DP_MODU_NUM] = {0x8314,  0x33CE0,  0x343E0,  0x34AE0};
CBIOS_U32 DP_REG_AUX_WRITE2[DP_MODU_NUM] = {0x8318,  0x33CE4,  0x343E4,  0x34AE4};
CBIOS_U32 DP_REG_AUX_WRITE3[DP_MODU_NUM] = {0x831C,  0x33CE8,  0x343E8,  0x34AE8};
CBIOS_U32 DP_REG_AUX_READ0[DP_MODU_NUM] = {0x8320,  0x33CEC,  0x343EC,  0x34AEC};
CBIOS_U32 DP_REG_AUX_READ1[DP_MODU_NUM] = {0x8324,  0x33CF0,  0x343F0,  0x34AF0};
CBIOS_U32 DP_REG_AUX_READ2[DP_MODU_NUM] = {0x8328,  0x33CF4,  0x343F4,  0x34AF4};
CBIOS_U32 DP_REG_AUX_READ3[DP_MODU_NUM] = {0x832C,  0x33CF8,  0x343F8,  0x34AF8};
CBIOS_U32 DP_REG_AUX_TIMER[DP_MODU_NUM] = {0x8330,  0x33CFC,  0x343FC,  0x34AFC};
CBIOS_U32 DP_REG_AUX_CMD[DP_MODU_NUM] = {0x8334,  0x33D00,  0x34400,  0x34B00};
CBIOS_U32 DP_REG_MUTE[DP_MODU_NUM] = {0x8338,  0x33D04,  0x34404,  0x34B04};
CBIOS_U32 DP_REG_MAUD[DP_MODU_NUM] = {0x833C,  0x33D08,  0x34408,  0x34B08};
CBIOS_U32 DP_REG_EPHY_MPLL[DP_MODU_NUM] = {0x8340,  0x33D0C,  0x3440C,  0x34B0C};
CBIOS_U32 DP_REG_EPHY_TX[DP_MODU_NUM] = {0x8344,  0x33D10,  0x34410,  0x34B10};
CBIOS_U32 DP_REG_EPHY_MISC[DP_MODU_NUM] = {0x8348,  0x33D14,  0x34414,  0x34B14};
CBIOS_U32 DP_REG_SWING[DP_MODU_NUM] = {0x8368,  0x33D28,  0x34428,  0x34B28};
CBIOS_U32 DP_REG_EPHY_STATUS[DP_MODU_NUM] = {0x836C,  0x33D2C,  0x3442C,  0x34B2C};
CBIOS_U32 DP_REG_LINK_CTRL[DP_MODU_NUM] = {0x334C8, 0x33D3C,  0x3443C,  0x34B3C};
CBIOS_U32 DP_REG_CTRL2[DP_MODU_NUM] = {0x334CC, 0x33D40,  0x34440,  0x34B40};
CBIOS_U32 DP_REG_CTRL[DP_MODU_NUM] = {0x334DC, 0x33D50,  0x34450,  0x34B50};
CBIOS_U32 DP_REG_EPHY_SETTING1[DP_MODU_NUM] = {0x334E0, 0x33D54,  0x34454,  0x34B54};
CBIOS_U32 DP_REG_EPHY_SETTING2[DP_MODU_NUM] = {0x334E4, 0x33D58,  0x34458,  0x34B58};
CBIOS_U32 DP_REG_EPHY_SETTING3[DP_MODU_NUM] = {0x33800, 0x33D5C, 0x3445C, 0x34B5C};
CBIOS_U32 DP_REG_FREE_SYNC[DP_MODU_NUM] = {0x33668, 0x339A0, 0x340A0, 0x347A0};
CBIOS_U32 DP_REG_FREE_SYNC_TRIGER[DP_MODU_NUM] = {0x8200, 0x33958, 0x34058, 0x34758};
CBIOS_U32  EDP_REG_PSR_CONTROL1[DP_MODU_NUM] = {0x334BC, 0x33D30, 0x34430, 0x34B30};
CBIOS_U32  EDP_REG_PSR_CONTROL2[DP_MODU_NUM] = {0x334C4, 0x33D38, 0x34438, 0x34B38};

CBIOS_U32 DP_REG_LTTPR_CFG0[DP_USB_MODU_NUM]   = {0x33554, 0x33EF0};
CBIOS_U32 DP_REG_LTTPR_CFG1[DP_USB_MODU_NUM]   = {0x33558, 0x33EF4};
CBIOS_U32 DP_REG_EPHY_MUX_HPD[DP_USB_MODU_NUM] = {0x33570, 0x33F00};
CBIOS_U32 DP_REG_UD_EPHY_RTN[DP_USB_MODU_NUM]  = {0x3356C, 0x33EFC};
CBIOS_U32 DP_REG_UD_EPHY_PLL[DP_USB_MODU_NUM]  = {0x33574, 0x33F04};

CBIOS_BOOL cbDIU_EDP_IsEDPSupported(PCBIOS_EXTENSION_COMMON pcbe, CBIOS_MODULE_INDEX DPModuleIndex)
{
    CBIOS_BOOL bRet = CBIOS_FALSE;

    if (pcbe->SysBiosInfo.Version >= 0x11)
    {
        bRet = (((DPModuleIndex == CBIOS_MODULE_INDEX1) && (pcbe->SysBiosInfo.Dp1PortConnType == CBIOS_EDP_CONN))
                 || ((DPModuleIndex == CBIOS_MODULE_INDEX2) && (pcbe->SysBiosInfo.Dp2PortConnType == CBIOS_EDP_CONN))
                 || ((DPModuleIndex == CBIOS_MODULE_INDEX3) && (pcbe->SysBiosInfo.Dp3PortConnType == CBIOS_EDP_CONN)));
    }
    else
    {
        bRet = (((DPModuleIndex == CBIOS_MODULE_INDEX1) && (pcbe->FeatureSwitch.IsEDP1Enabled))
                 || ((DPModuleIndex == CBIOS_MODULE_INDEX2) && (pcbe->FeatureSwitch.IsEDP2Enabled))
                 || ((DPModuleIndex == CBIOS_MODULE_INDEX3) && (pcbe->FeatureSwitch.IsEDP3Enabled)));
    }

    return bRet;
}

CBIOS_VOID cbDIU_EDP_ControlVDDSignal(PCBIOS_VOID pvcbe, PCBIOS_DP_MONITOR_CONTEXT pDPMonitorContext, CBIOS_MODULE_INDEX DPModuleIndex, CBIOS_BOOL status)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    REG_SR3C_Pair RegSR3CValue, RegSR3CMask;
    REG_SR35_Pair RegSR35Value, RegSR35Mask;
    CBIOS_BOOL bInvertSig = pcbe->SysBiosInfo.bInvertPanelSignal;
#ifndef GOP_BUILD
    CBIOS_U64 CurrentTime, Duration;
#endif

    if(status) //ON
    {
    #ifndef GOP_BUILD
        CurrentTime = cb_QuerySystemTime();
        Duration = (CurrentTime - pDPMonitorContext->TimeofLastVDDOff) / 1000;

        if(Duration < 500)
        {
            cbDelayMilliSeconds(500 - (CBIOS_U32)Duration);
        }
    #endif

        if(((DPModuleIndex == CBIOS_MODULE_INDEX1) && (pDPMonitorContext->GpioForEDP1Power== 0x0f))
            || ((DPModuleIndex == CBIOS_MODULE_INDEX2) && (pDPMonitorContext->GpioForEDP2Power== 0x0f)))  //invalid
        {
            RegSR3CValue.Value = 0;
            RegSR3CValue.ENVDD = 0;
            RegSR3CMask.Value = 0xFF;
            RegSR3CMask.ENVDD = 0;
            cbMMIOWriteReg(pcbe, SR_3C, RegSR3CValue.Value, RegSR3CMask.Value);
            cbDelayMilliSeconds(2);
        }
        else if(((DPModuleIndex == CBIOS_MODULE_INDEX1) && (pDPMonitorContext->GpioForEDP1Power== 0))
            || ((DPModuleIndex == CBIOS_MODULE_INDEX2) && (pDPMonitorContext->GpioForEDP2Power== 0))
            || (DPModuleIndex == CBIOS_MODULE_INDEX3))
        {
            RegSR35Value.Value = 0;
            RegSR35Value.GPIO0_Select = 0x1;
            RegSR35Value.GPIO0_Data = bInvertSig ? 0 : 1;
            RegSR35Mask.Value = 0xFF;
            RegSR35Mask.GPIO0_Select = 0;
            RegSR35Mask.GPIO0_Data = 0;
            cbMMIOWriteReg(pcbe, SR_35, RegSR35Value.Value, RegSR35Mask.Value);
        }
        else
        {
            //TODO: GPIO control power sequence
            cbDebugPrint((MAKE_LEVEL(DP, WARNING), "%s: invalid power gpio index!\n", FUNCTION_NAME));
        }
    }
    else    //OFF
    {
        if(((DPModuleIndex == CBIOS_MODULE_INDEX1) && (pDPMonitorContext->GpioForEDP1Power== 0x0f))
            || ((DPModuleIndex == CBIOS_MODULE_INDEX2) && (pDPMonitorContext->GpioForEDP2Power== 0x0f)))  //invalid
        {
            //cbDelayMilliSeconds(16);
            RegSR3CValue.Value = 0;
            RegSR3CValue.ENVDD = 1;
            RegSR3CMask.Value = 0xFF;
            RegSR3CMask.ENVDD = 0;
            cbMMIOWriteReg(pcbe, SR_3C, RegSR3CValue.Value, RegSR3CMask.Value);
            cbDelayMilliSeconds(500);
        }
        else if(((DPModuleIndex == CBIOS_MODULE_INDEX1) && (pDPMonitorContext->GpioForEDP1Power== 0))
            || ((DPModuleIndex == CBIOS_MODULE_INDEX2) && (pDPMonitorContext->GpioForEDP2Power== 0))
            || (DPModuleIndex == CBIOS_MODULE_INDEX3))
        {
            RegSR35Value.Value = 0;
            RegSR35Value.GPIO0_Select = 0x1;
            RegSR35Value.GPIO0_Data = bInvertSig ? 1 : 0;
            RegSR35Mask.Value = 0xFF;
            RegSR35Mask.GPIO0_Select = 0;
            RegSR35Mask.GPIO0_Data = 0;
            cbMMIOWriteReg(pcbe, SR_35, RegSR35Value.Value, RegSR35Mask.Value);
        #ifndef GOP_BUILD
            pDPMonitorContext->TimeofLastVDDOff = cb_QuerySystemTime();
        #else
            cbDelayMilliSeconds(500);
        #endif
        }
        else
        {
            //TODO: GPIO control power sequence
            cbDebugPrint((MAKE_LEVEL(DP, WARNING), "%s: invalid power gpio index!\n", FUNCTION_NAME));
        }
    }
}

CBIOS_VOID cbDIU_EDP_ControlVEESignal(PCBIOS_VOID pvcbe, PCBIOS_DP_MONITOR_CONTEXT pDPMonitorContext, CBIOS_MODULE_INDEX DPModuleIndex, CBIOS_BOOL status)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    REG_SR3C_Pair   RegSR3CValue, RegSR3CMask;
    REG_SR35_Pair   RegSR35Value, RegSR35Mask;
    CBIOS_BOOL bInvertSig = pcbe->SysBiosInfo.bInvertPanelSignal;

    if(status) //ON
    {
        if(((DPModuleIndex == CBIOS_MODULE_INDEX1) && (pDPMonitorContext->GpioForEDP1BackLight== 0x0f))
            || ((DPModuleIndex == CBIOS_MODULE_INDEX2) && (pDPMonitorContext->GpioForEDP2BackLight== 0x0f)))  //invalid
        {
            //cbDelayMilliSeconds(256);
            RegSR3CValue.Value = 0;
            RegSR3CValue.ENVEE = 0;
            RegSR3CMask.Value = 0xFF;
            RegSR3CMask.ENVEE = 0;
            cbMMIOWriteReg(pcbe,SR_3C, RegSR3CValue.Value, RegSR3CMask.Value);
        }
        else if(((DPModuleIndex == CBIOS_MODULE_INDEX1) && (pDPMonitorContext->GpioForEDP1BackLight== 1))
            || ((DPModuleIndex == CBIOS_MODULE_INDEX2) && (pDPMonitorContext->GpioForEDP2BackLight== 1))
            || (DPModuleIndex == CBIOS_MODULE_INDEX3))
        {
            RegSR35Value.Value = 0;
            RegSR35Value.GPIO0_Select = 0x1;
            RegSR35Value.GPIO0_Data = bInvertSig ? 0 : 1;
            RegSR35Mask.Value = 0xFF;
            RegSR35Mask.GPIO0_Select = 0;
            RegSR35Mask.GPIO0_Data = 0;
            cbMMIOWriteReg(pcbe, SR_B_35, RegSR35Value.Value, RegSR35Mask.Value);
        }
        else
        {
            //TODO: GPIO control power sequence
            cbDebugPrint((MAKE_LEVEL(DP, WARNING), "%s: invalid backlignt gpio index!\n", FUNCTION_NAME));
        }
    }
    else    //OFF
    {
        if(((DPModuleIndex == CBIOS_MODULE_INDEX1) && (pDPMonitorContext->GpioForEDP1BackLight== 0x0f))
            || ((DPModuleIndex == CBIOS_MODULE_INDEX2) && (pDPMonitorContext->GpioForEDP2BackLight== 0x0f)))  //invalid
        {
            RegSR3CValue.Value = 0;
            RegSR3CValue.ENVEE = 1;
            RegSR3CMask.Value = 0xFF;
            RegSR3CMask.ENVEE = 0;
            cbMMIOWriteReg(pcbe,SR_3C, RegSR3CValue.Value, RegSR3CMask.Value);
            //cbDelayMilliSeconds(256);
        }
        else if(((DPModuleIndex == CBIOS_MODULE_INDEX1) && (pDPMonitorContext->GpioForEDP1BackLight== 1))
            || ((DPModuleIndex == CBIOS_MODULE_INDEX2) && (pDPMonitorContext->GpioForEDP2BackLight== 1))
            || (DPModuleIndex == CBIOS_MODULE_INDEX3))
        {
            RegSR35Value.Value = 0;
            RegSR35Value.GPIO0_Select = 0x1;
            RegSR35Value.GPIO0_Data = bInvertSig ? 1 : 0;
            RegSR35Mask.Value = 0xFF;
            RegSR35Mask.GPIO0_Select = 0;
            RegSR35Mask.GPIO0_Data = 0;
            cbMMIOWriteReg(pcbe, SR_B_35, RegSR35Value.Value, RegSR35Mask.Value);
        }
        else
        {
            //TODO: GPIO control power sequence
            cbDebugPrint((MAKE_LEVEL(DP, WARNING), "%s: invalid backlignt gpio index!\n", FUNCTION_NAME));
        }
    }
}

CBIOS_BOOL cbDIU_EDP_WaitforSinkHPDSignal(PCBIOS_VOID pvcbe, CBIOS_MODULE_INDEX DPModuleIndex, CBIOS_U16 timeout)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    CBIOS_U16               counter = 0;
    REG_MM8330              DPAuxTimerRegValue;

    if(DPModuleIndex >= DP_MODU_NUM)
    {
        cbDebugPrint((MAKE_LEVEL(DP, ERROR), "Invalid module index in %s\n", FUNCTION_NAME));
        return  CBIOS_FALSE;
    }

    while (counter <= timeout)
    {
        //Per Sean, if mmio 8330[31:30] shows hot plugin, means a rising edge of hpd signal
        //After VDD on, HPD signal will be a rising edge, and vdd off will be falling edge
        // EDP cannot be Tunnel or Alt-mode, just read MM8330
        DPAuxTimerRegValue.Value = cb_ReadU32(pcbe->pAdapterContext, DP_REG_AUX_TIMER[DPModuleIndex]);
        if (DPAuxTimerRegValue.HPD_Status == CBIOS_HPD_STATUS_IN)
        {
            return CBIOS_TRUE;
        }
        cbDelayMilliSeconds(1);  //query HPD status every 1ms
        counter += 1;
    }

    return CBIOS_FALSE;
}

CBIOS_BOOL cbDIU_EDP_EDPAuxPowerSeqCtrl(PCBIOS_EXTENSION_COMMON pcbe, PCBIOS_DP_MONITOR_CONTEXT pDPMonitorContext, CBIOS_MODULE_INDEX DPModuleIndex, CBIOS_BOOL bOn)
{
    if (bOn)
    {
        // eDP power sequence on for aux channel operation
        // 1. enable vdd
        cbDIU_EDP_ControlVDDSignal(pcbe, pDPMonitorContext, DPModuleIndex, CBIOS_TRUE);
        // delay 100ms here to resolve Lenovo hpd issue as the hpd signal is not stable
        cbDelayMilliSeconds(100);
        // 2. wait for HDP from sink, timeout is 200ms
        if (!cbDIU_EDP_WaitforSinkHPDSignal(pcbe, DPModuleIndex, 400))
        {
            cbDebugPrint((MAKE_LEVEL(DP, ERROR), "%s: can't wait HPD high from sink\n", FUNCTION_NAME));
            return CBIOS_FALSE;
        }
        pDPMonitorContext->VddStatus = 1;
    }
    else
    {
        // eDP power sequence off for aux channel operation
        // disable vdd
        cbDIU_EDP_ControlVDDSignal(pcbe, pDPMonitorContext, DPModuleIndex, CBIOS_FALSE);
        pDPMonitorContext->VddStatus = 0;
    }
    return CBIOS_TRUE;
}

CBIOS_VOID cbDIU_DP_SetFreeSync(PCBIOS_VOID pvcbe, CBIOS_MODULE_INDEX DPModuleIndex, CBIOS_U32 FreeSyncInsertLineNum)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    REG_MM33668_CX4 DPFreeSyncRegValue, DPFreeSyncRegMask;

    if (DPModuleIndex >= DP_MODU_NUM)
    {
        cbDebugPrint((MAKE_LEVEL(DP, ERROR), "Invalid module index in %s\n", FUNCTION_NAME));
        return;
    }

    DPFreeSyncRegValue.Value = 0;
    DPFreeSyncRegValue.Dynamic_Sync_Enable = (FreeSyncInsertLineNum == 0)? 0: 1;
    DPFreeSyncRegValue.Select_Mode = 1;
    DPFreeSyncRegValue.Max_Line_Number = FreeSyncInsertLineNum;
    DPFreeSyncRegMask.Value = 0xFFFFFFFF;
    DPFreeSyncRegMask.Dynamic_Sync_Enable = 0;
    DPFreeSyncRegMask.Select_Mode = 0;
    DPFreeSyncRegMask.Max_Line_Number = 0;
    cbMMIOWriteReg32(pcbe, DP_REG_FREE_SYNC[DPModuleIndex], DPFreeSyncRegValue.Value, DPFreeSyncRegMask.Value);

    //MM33668[0] need trigger to take effect.
    cbMMIOWriteReg32(pcbe, DP_REG_FREE_SYNC_TRIGER[DPModuleIndex], 0x01, 0x0);
}

CBIOS_VOID cbDIU_DP_DPModeEnable(PCBIOS_VOID pvcbe, CBIOS_MODULE_INDEX DPModuleIndex, CBIOS_BOOL bEnable)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    REG_MM8240 DPEnableRegValue, DPEnableRegMask;

    if(DPModuleIndex >= DP_MODU_NUM)
    {
        cbDebugPrint((MAKE_LEVEL(DP, ERROR), "Invalid module index in %s\n", FUNCTION_NAME));
        return;
    }

    cbTraceEnter(DP);

    DPEnableRegValue.Value = 0;
    DPEnableRegValue.DP_Enable = (bEnable)? 1 : 0;
    DPEnableRegMask.Value = 0xFFFFFFFF;
    DPEnableRegMask.DP_Enable = 0;

    cbMMIOWriteReg32(pcbe, DP_REG_ENABLE[DPModuleIndex], DPEnableRegValue.Value, DPEnableRegMask.Value);
    
    cbTraceExit(DP);
}

CBIOS_VOID cbDIU_DP_SetInterruptMode(PCBIOS_VOID pvcbe, CBIOS_MODULE_INDEX DPModuleIndex, CBIOS_BOOL isDPMode)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    REG_MM82CC DPEphyCtrlRegValue, DPEphyCtrlRegMask;

    if(DPModuleIndex >= DP_MODU_NUM)
    {
        cbDebugPrint((MAKE_LEVEL(DP, ERROR), "Invalid module index in %s\n", FUNCTION_NAME));
        return;
    }

    DPEphyCtrlRegValue.Value = 0;
    DPEphyCtrlRegValue.int_mode = (isDPMode)? 0 : 1;
    DPEphyCtrlRegMask.Value = 0xFFFFFFFF;
    DPEphyCtrlRegMask.int_mode = 0;

    cbMMIOWriteReg32(pcbe, DP_REG_EPHY_CTRL[DPModuleIndex], DPEphyCtrlRegValue.Value, DPEphyCtrlRegMask.Value);
}

CBIOS_VOID cbDIU_DP_SetHVSync(PCBIOS_VOID pvcbe, CBIOS_MODULE_INDEX DPModuleIndex, CBIOS_U8 HVPolarity)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    REG_MM8258 DPPolarityRegValue, DPPolarityRegMask;

    if(DPModuleIndex >= DP_MODU_NUM)
    {
        cbDebugPrint((MAKE_LEVEL(DP, ERROR), "Invalid module index in %s\n", FUNCTION_NAME));
        return;
    }

    DPPolarityRegValue.Value = 0;
    if (HVPolarity & HorNEGATIVE)
    {
        DPPolarityRegValue.HSYNC_Polarity = 1;
    }
    if (HVPolarity & VerNEGATIVE)
    {
        DPPolarityRegValue.VSYNC_Polarity = 1;
    }

    DPPolarityRegMask.Value = 0xFFFFFFFF;
    DPPolarityRegMask.HSYNC_Polarity = 0;
    DPPolarityRegMask.VSYNC_Polarity = 0;
    cbMMIOWriteReg32(pcbe, DP_REG_HV_SYNC[DPModuleIndex], DPPolarityRegValue.Value, DPPolarityRegMask.Value);
}

CBIOS_VOID cbDIU_DP_SetMaudNaud(PCBIOS_VOID pvcbe, CBIOS_MODULE_INDEX DPModuleIndex, CBIOS_U32 LinkSpeed, CBIOS_U32 StreamFormat)
{
    CBIOS_U64 Naud = 0, Maud = 0;
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    REG_MM8240 DPEnableRegValue, DPEnableRegMask;
    REG_MM8338 DPMuteRegValue, DPMuteRegMask;
    REG_MM833C DPMaudRegValue, DPMaudRegMask;
    REG_MM8218 DPCodecSelRegValue, DPCodecSelRegMask;

    if(DPModuleIndex >= DP_MODU_NUM)
    {
        cbDebugPrint((MAKE_LEVEL(DP, ERROR), "Invalid module index in %s\n", FUNCTION_NAME));
        return;
    }

    // 1. Audio Enable
    DPMuteRegValue.Value = 0;
    DPMuteRegValue.Audio_Output_Enable = 1;
    DPMuteRegMask.Value = 0xFFFFFFFF;
    DPMuteRegMask.Audio_Output_Enable = 0;
    cbMMIOWriteReg32(pcbe, DP_REG_MUTE[DPModuleIndex], DPMuteRegValue.Value, DPMuteRegMask.Value);

    DPEnableRegValue.Value = 0;
    DPEnableRegValue.Enable_Audio = 1;
    DPEnableRegMask.Value = 0xFFFFFFFF;
    DPEnableRegMask.Enable_Audio = 0;
    cbMMIOWriteReg32(pcbe, DP_REG_ENABLE[DPModuleIndex], DPEnableRegValue.Value, DPEnableRegMask.Value);

    DPCodecSelRegValue.Value = 0;
    DPCodecSelRegMask.Value = 0xFFFFFFFF;
    DPCodecSelRegMask.Audio_Strm_Select = 0;
    if(DPModuleIndex == CBIOS_MODULE_INDEX1)
    {
        DPCodecSelRegValue.Audio_Strm_Select = 0;
    }
    else if(DPModuleIndex == CBIOS_MODULE_INDEX2)
    {

        DPCodecSelRegValue.Audio_Strm_Select = 1;       
        if(pcbe->ChipID == CHIPID_CNE001)
        {
            DPCodecSelRegValue.Audio_Strm_Select = 0;
        }
        
    }
    else if(DPModuleIndex == CBIOS_MODULE_INDEX3)
    {
        if(pcbe->ChipID == CHIPID_CNE001)
        {
            DPCodecSelRegValue.Audio_Strm_Select = 0;
        }
    }
    cbMMIOWriteReg32(pcbe, DP_REG_GEN_CTRL[DPModuleIndex], DPCodecSelRegValue.Value, DPCodecSelRegMask.Value);

    // 2. Calculate Naud Maud
    Naud = 0x8000; // set Naud = 32768
#if 1 // DP SW MAUD
    Maud = cb_do_div((Naud * 512 * StreamFormat), (LinkSpeed * 100));

    DPMaudRegValue.Value = 0;
    DPMaudRegValue.MAUD = (CBIOS_U32)Maud;
    DPMaudRegMask.Value = 0xFFFFFFFF;
    DPMaudRegMask.MAUD = 0;
    cbMMIOWriteReg32(pcbe, DP_REG_MAUD[DPModuleIndex], DPMaudRegValue.Value, DPMaudRegMask.Value);
#endif
    DPMuteRegValue.Value = 0;
    DPMuteRegValue.NAUD = (CBIOS_U32)Naud;
#if 1 //DP SW MAUD
    DPMuteRegValue.Generate_MAUD = 0; // use SW MAUD
    DPMuteRegValue.Generated_MAUD_Mode = 0; // use HW MAUD mode 0
#else
    DPMuteRegValue.Generate_MAUD = 1; // use HW MAUD
    DPMuteRegValue.Generated_MAUD_Mode = 1; // use HW MAUD
#endif
    DPMuteRegMask.Value = 0xFFFFFFFF;
    DPMuteRegMask.NAUD = 0;
    DPMuteRegMask.Generate_MAUD = 0;
    DPMuteRegMask.Generated_MAUD_Mode = 0;
    cbMMIOWriteReg32(pcbe, DP_REG_MUTE[DPModuleIndex], DPMuteRegValue.Value, DPMuteRegMask.Value);
}

CBIOS_VOID cbDIU_DP_ResetLinkTraining(PCBIOS_VOID pvcbe, CBIOS_MODULE_INDEX DPModuleIndex)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    REG_MM8214    DPLinkRegValue, DPLinkRegMask;

    cbTraceEnter(DP);

    if(DPModuleIndex >= DP_MODU_NUM)
    {
        cbDebugPrint((MAKE_LEVEL(DP, ERROR), "Invalid module index in %s\n", FUNCTION_NAME));
        return;
    }

    // reset Link Training
    DPLinkRegValue.Value = 0;
    DPLinkRegValue.SW_Set_Link_Train_Fail = 1;
    DPLinkRegMask.Value = 0xFFFFFFFF;
    DPLinkRegMask.SW_Set_Link_Train_Fail = 0;
    cbMMIOWriteReg32(pcbe, DP_REG_LINK[DPModuleIndex], DPLinkRegValue.Value, DPLinkRegMask.Value);

    DPLinkRegValue.Value = 0;
    DPLinkRegValue.SW_Set_Link_Train_Fail = 0;
    DPLinkRegMask.Value = 0xFFFFFFFF;
    DPLinkRegMask.SW_Set_Link_Train_Fail = 0;
    cbMMIOWriteReg32(pcbe, DP_REG_LINK[DPModuleIndex], DPLinkRegValue.Value, DPLinkRegMask.Value);

    DPLinkRegValue.Value = 0;
    DPLinkRegValue.SW_Link_Train_State = 0;
    DPLinkRegValue.SW_Set_Link_Train_Fail = 0;
    DPLinkRegMask.Value = 0xFFFFFFFF;
    DPLinkRegMask.SW_Link_Train_State = 0;
    DPLinkRegMask.SW_Set_Link_Train_Fail = 0;
    cbMMIOWriteReg32(pcbe, DP_REG_LINK[DPModuleIndex], DPLinkRegValue.Value, DPLinkRegMask.Value);

    cbTraceExit(DP);
}

CBIOS_VOID cbDIU_DP_VideoAudioOnOff(PCBIOS_VOID pvcbe, CBIOS_MODULE_INDEX DPModuleIndex, CBIOS_BOOL bOn)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    REG_MM8240 DPEnableRegValue, DPEnableRegMask;
    REG_MM8338 DPMuteRegValue, DPMuteRegMask;

    cbTraceEnter(DP);

    if(DPModuleIndex >= DP_MODU_NUM)
    {
        cbDebugPrint((MAKE_LEVEL(DP, ERROR), "Invalid module index in %s\n", FUNCTION_NAME));
        return;
    }

    if(bOn)
    {
        DPMuteRegValue.Value = 0;
        DPMuteRegValue.Audio_Output_Enable = 1;
        DPMuteRegValue.Mute = 0;
        DPMuteRegValue.Mute_Mode = 1;
        DPMuteRegMask.Value = 0xFFFFFFFF;
        DPMuteRegMask.Audio_Output_Enable = 0;
        DPMuteRegMask.Mute = 0;
        DPMuteRegMask.Mute_Mode = 0;
        cbMMIOWriteReg32(pcbe, DP_REG_MUTE[DPModuleIndex], DPMuteRegValue.Value, DPMuteRegMask.Value);

        DPEnableRegValue.Value = 0;
        DPEnableRegValue.Enable_Audio = 1;
        DPEnableRegValue.Video_Enable = 1;
        DPEnableRegMask.Value = 0xFFFFFFFF;
        DPEnableRegMask.Video_Enable = 0;
        DPEnableRegMask.Enable_Audio = 0;
        cbMMIOWriteReg32(pcbe, DP_REG_ENABLE[DPModuleIndex], DPEnableRegValue.Value, DPEnableRegMask.Value);
    }
    else
    {
        //disable video and audio
        DPMuteRegValue.Value = 0;
        DPMuteRegValue.Audio_Output_Enable = 0;
        DPMuteRegValue.Mute = 1;
        DPMuteRegValue.Mute_Mode = 1;
        DPMuteRegMask.Value = 0xFFFFFFFF;
        DPMuteRegMask.Audio_Output_Enable = 0;
        DPMuteRegMask.Mute = 0;
        DPMuteRegMask.Mute_Mode =0;
        cbMMIOWriteReg32(pcbe, DP_REG_MUTE[DPModuleIndex], DPMuteRegValue.Value, DPMuteRegMask.Value);

        DPEnableRegValue.Value = 0;
        DPEnableRegValue.Enable_Audio = 0;
        DPEnableRegValue.Video_Enable = 0;
        DPEnableRegMask.Value = 0xFFFFFFFF;
        DPEnableRegMask.Enable_Audio = 0;
        DPEnableRegMask.Video_Enable = 0;
        cbWaitNonFullVBlank(pcbe, DPModuleIndex);
        cbMMIOWriteReg32(pcbe, DP_REG_ENABLE[DPModuleIndex], DPEnableRegValue.Value, DPEnableRegMask.Value);
    }
}

/***************************************************************************************/
/*******************************  Link Training Interfaces  ****************************/
// Reset Aux, PISO, enable Scramble
static CBIOS_VOID cbDIU_DP_LinkTrainingPreset(PCBIOS_EXTENSION_COMMON pcbe, CBIOS_MODULE_INDEX DPModuleIndex, PCBIOS_LINK_TRAINING_PARAMS pLinkTrainingParams, CBIOS_U32 LaneNum)
{
    REG_MM8218 DPGenCtrlRegValue, DPGenCtrlRegMask;
    REG_MM8240 DPEnableRegValue, DPEnableRegMask;
    REG_MM8344 DPEphyTxRegValue, DPEphyTxRegMask;

    cbTraceEnter(DP);

    if(DPModuleIndex >= DP_MODU_NUM)
    {
        cbDebugPrint((MAKE_LEVEL(DP, ERROR), "Invalid module index in %s\n", FUNCTION_NAME));
        return;
    }

    // Reset Aux
    cbDIU_DP_ResetAUX(pcbe, DPModuleIndex);

    if(LaneNum == 2)
    {
        if (pLinkTrainingParams->AltReversePlug) // alt-mode reverse plug
        {
            // Reset EPHY PISO in case of speed change
            DPEphyTxRegValue.Value = 0;
            DPEphyTxRegValue.TX_Power_Control_Lane2 = 1;
            DPEphyTxRegValue.TX_Power_Control_Lane3 = 1;
            DPEphyTxRegMask.Value = 0xFFFFFFFF;
            DPEphyTxRegMask.TX_Power_Control_Lane2 = 0;
            DPEphyTxRegMask.TX_Power_Control_Lane3 = 0;
            cbMMIOWriteReg32(pcbe, DP_REG_EPHY_TX[DPModuleIndex], DPEphyTxRegValue.Value, DPEphyTxRegMask.Value);

            cb_DelayMicroSeconds(1);

            DPEphyTxRegValue.Value = 0;
            DPEphyTxRegValue.TX_Power_Control_Lane2 = 0;
            DPEphyTxRegValue.TX_Power_Control_Lane3 = 0;
            DPEphyTxRegMask.Value = 0xFFFFFFFF;
            DPEphyTxRegMask.TX_Power_Control_Lane2 = 0;
            DPEphyTxRegMask.TX_Power_Control_Lane3 = 0;
            cbMMIOWriteReg32(pcbe, DP_REG_EPHY_TX[DPModuleIndex], DPEphyTxRegValue.Value, DPEphyTxRegMask.Value);
        }
        else
        {
            // Reset EPHY PISO in case of speed change
            DPEphyTxRegValue.Value = 0;
            DPEphyTxRegValue.TX_Power_Control_Lane0 = 1;
            DPEphyTxRegValue.TX_Power_Control_Lane1 = 1;
            DPEphyTxRegMask.Value = 0xFFFFFFFF;
            DPEphyTxRegMask.TX_Power_Control_Lane0 = 0;
            DPEphyTxRegMask.TX_Power_Control_Lane1 = 0;
            cbMMIOWriteReg32(pcbe, DP_REG_EPHY_TX[DPModuleIndex], DPEphyTxRegValue.Value, DPEphyTxRegMask.Value);

            cb_DelayMicroSeconds(1);

            DPEphyTxRegValue.Value = 0;
            DPEphyTxRegValue.TX_Power_Control_Lane0 = 0;
            DPEphyTxRegValue.TX_Power_Control_Lane1 = 0;
            DPEphyTxRegMask.Value = 0xFFFFFFFF;
            DPEphyTxRegMask.TX_Power_Control_Lane0 = 0;
            DPEphyTxRegMask.TX_Power_Control_Lane1 = 0;
            cbMMIOWriteReg32(pcbe, DP_REG_EPHY_TX[DPModuleIndex], DPEphyTxRegValue.Value, DPEphyTxRegMask.Value);
        }
    }
    else if(LaneNum == 1)
    {
        if (pLinkTrainingParams->AltReversePlug) // alt-mode reverse plug
        {
            // Reset EPHY PISO in case of speed change
            DPEphyTxRegValue.Value = 0;
            DPEphyTxRegValue.TX_Power_Control_Lane3 = 1;
            DPEphyTxRegMask.Value = 0xFFFFFFFF;
            DPEphyTxRegMask.TX_Power_Control_Lane3 = 0;
            cbMMIOWriteReg32(pcbe, DP_REG_EPHY_TX[DPModuleIndex], DPEphyTxRegValue.Value, DPEphyTxRegMask.Value);

            cb_DelayMicroSeconds(1);

            DPEphyTxRegValue.Value = 0;
            DPEphyTxRegValue.TX_Power_Control_Lane3 = 0;
            DPEphyTxRegMask.Value = 0xFFFFFFFF;
            DPEphyTxRegMask.TX_Power_Control_Lane3 = 0;
            cbMMIOWriteReg32(pcbe, DP_REG_EPHY_TX[DPModuleIndex], DPEphyTxRegValue.Value, DPEphyTxRegMask.Value);
        }
        else
        {
            // Reset EPHY PISO in case of speed change
            DPEphyTxRegValue.Value = 0;
            DPEphyTxRegValue.TX_Power_Control_Lane0 = 1;
            DPEphyTxRegMask.Value = 0xFFFFFFFF;
            DPEphyTxRegMask.TX_Power_Control_Lane0 = 0;
            cbMMIOWriteReg32(pcbe, DP_REG_EPHY_TX[DPModuleIndex], DPEphyTxRegValue.Value, DPEphyTxRegMask.Value);

            cb_DelayMicroSeconds(1);

            DPEphyTxRegValue.Value = 0;
            DPEphyTxRegValue.TX_Power_Control_Lane0 = 0;
            DPEphyTxRegMask.Value = 0xFFFFFFFF;
            DPEphyTxRegMask.TX_Power_Control_Lane0 = 0;
            cbMMIOWriteReg32(pcbe, DP_REG_EPHY_TX[DPModuleIndex], DPEphyTxRegValue.Value, DPEphyTxRegMask.Value);
        }
    }
    else
    {
        // Reset EPHY PISO in case of speed change
        DPEphyTxRegValue.Value = 0;
        DPEphyTxRegValue.TX_Power_Control_Lane0 = 1;
        DPEphyTxRegValue.TX_Power_Control_Lane1 = 1;
        DPEphyTxRegValue.TX_Power_Control_Lane2 = 1;
        DPEphyTxRegValue.TX_Power_Control_Lane3 = 1;
        DPEphyTxRegMask.Value = 0xFFFFFFFF;
        DPEphyTxRegMask.TX_Power_Control_Lane0 = 0;
        DPEphyTxRegMask.TX_Power_Control_Lane1 = 0;
        DPEphyTxRegMask.TX_Power_Control_Lane2 = 0;
        DPEphyTxRegMask.TX_Power_Control_Lane3 = 0;
        cbMMIOWriteReg32(pcbe, DP_REG_EPHY_TX[DPModuleIndex], DPEphyTxRegValue.Value, DPEphyTxRegMask.Value);

        cb_DelayMicroSeconds(1);

        DPEphyTxRegValue.Value = 0;
        DPEphyTxRegValue.TX_Power_Control_Lane0 = 0;
        DPEphyTxRegValue.TX_Power_Control_Lane1 = 0;
        DPEphyTxRegValue.TX_Power_Control_Lane2 = 0;
        DPEphyTxRegValue.TX_Power_Control_Lane3 = 0;
        DPEphyTxRegMask.Value = 0xFFFFFFFF;
        DPEphyTxRegMask.TX_Power_Control_Lane0 = 0;
        DPEphyTxRegMask.TX_Power_Control_Lane1 = 0;
        DPEphyTxRegMask.TX_Power_Control_Lane2 = 0;
        DPEphyTxRegMask.TX_Power_Control_Lane3 = 0;
        cbMMIOWriteReg32(pcbe, DP_REG_EPHY_TX[DPModuleIndex], DPEphyTxRegValue.Value, DPEphyTxRegMask.Value);
    }

    // enable scramble
    DPGenCtrlRegValue.Value = 0;
    DPGenCtrlRegValue.Scramble_enable = 1;
    DPGenCtrlRegValue.Switch_Idle_mode_to_video = 0;
    DPGenCtrlRegValue.Idle_Pattern_Counter = 0;
    DPGenCtrlRegValue.AUX_Length = 0;
    DPGenCtrlRegValue.HW_Link_Train_Fail = 0;
    DPGenCtrlRegMask.Value = 0xFFFFFFFF;
    DPGenCtrlRegMask.Scramble_enable = 0;
    DPGenCtrlRegMask.Switch_Idle_mode_to_video = 0;
    DPGenCtrlRegMask.Idle_Pattern_Counter = 0;
    DPGenCtrlRegMask.AUX_Length = 0;
    DPGenCtrlRegMask.HW_Link_Train_Fail = 0;
    cbMMIOWriteReg32(pcbe, DP_REG_GEN_CTRL[DPModuleIndex], DPGenCtrlRegValue.Value, DPGenCtrlRegMask.Value);

    DPEnableRegValue.Value = 0;
    DPEnableRegValue.DP_Enable = 1;
    DPEnableRegValue.Field_Invert = 0;
    DPEnableRegValue.Enhanced_Framing_Mode = pLinkTrainingParams->bEnhancedMode ? 1 : 0;
    DPEnableRegValue.InfoFrame_FIFO_1_Ready = 0;
    DPEnableRegValue.INFOFRAME_FIFO_2_READY = 0;
    DPEnableRegValue.InfoFrame_FIFO_Select = 0;
    DPEnableRegValue.InfoFrame_FIFO_1_Start_Address = 0;
    DPEnableRegValue.InfoFrame_FIFO_2_Start_Address = 0;
    DPEnableRegValue.InfoFrame_FIFO_1_Length = 0;
    DPEnableRegValue.InfoFrame_FIFO_2_Length = 0;
    DPEnableRegValue.Ext_Packet_Enable = 0;
    DPEnableRegValue.header_of_audio_info_frame_is_from_HDAudio_codec = 0;
    DPEnableRegValue.Main_Link_Status = 0;

    DPEnableRegMask.Value = 0xFFFFFFFF;
    DPEnableRegMask.DP_Enable = 0;
    DPEnableRegMask.Field_Invert = 0;
    DPEnableRegMask.Enhanced_Framing_Mode = 0;
    DPEnableRegMask.InfoFrame_FIFO_1_Ready = 0;
    DPEnableRegMask.INFOFRAME_FIFO_2_READY = 0;
    DPEnableRegMask.InfoFrame_FIFO_Select = 0;
    DPEnableRegMask.InfoFrame_FIFO_1_Start_Address = 0;
    DPEnableRegMask.InfoFrame_FIFO_2_Start_Address = 0;
    DPEnableRegMask.InfoFrame_FIFO_1_Length = 0;
    DPEnableRegMask.InfoFrame_FIFO_2_Length = 0;
    DPEnableRegMask.Ext_Packet_Enable = 0;
    DPEnableRegMask.header_of_audio_info_frame_is_from_HDAudio_codec = 0;
    DPEnableRegMask.Main_Link_Status = 0;

    cbMMIOWriteReg32(pcbe, DP_REG_ENABLE[DPModuleIndex], DPEnableRegValue.Value, DPEnableRegMask.Value);

    cbTraceExit(DP);
}

static CBIOS_VOID cbDIU_DP_SetDPVersion(PCBIOS_EXTENSION_COMMON pcbe, CBIOS_MODULE_INDEX DPModuleIndex, CBIOS_U32 DPVersion)
{
    REG_MM334CC         DPCtrl2RegValue, DPCtrl2RegMask;

    if(DPModuleIndex >= DP_MODU_NUM)
    {
        cbDebugPrint((MAKE_LEVEL(DP, ERROR), "Invalid module index in %s\n", FUNCTION_NAME));
        return;
    }

    DPCtrl2RegValue.Value = 0;
    DPCtrl2RegValue.DP_VERSION_VALUE = DPVersion;
    DPCtrl2RegMask.Value = 0xFFFFFFFF;
    DPCtrl2RegMask.DP_VERSION_VALUE = 0;
    cbMMIOWriteReg32(pcbe, DP_REG_CTRL2[DPModuleIndex], DPCtrl2RegValue.Value, DPCtrl2RegMask.Value);
}

CBIOS_VOID cbDIU_DP_LT_SetEphy(PCBIOS_EXTENSION_COMMON pcbe, CBIOS_MODULE_INDEX DPModuleIndex, PCBIOS_LINK_TRAINING_PARAMS pLinkTrainingParams, CBIOS_U32 BitRate, CBIOS_BOOL ChangeSWswing)
{
    REG_MM8368     DPSwingRegValue, DPSwingRegMask;
    REG_MM8368_UD  UDEphyRegValue, UDEphyRegMask;
    CBIOS_BOOL bEDP = CBIOS_FALSE;
    CBIOS_BOOL bTMA = CBIOS_FALSE;
    CBIOS_U32               Device = CBIOS_TYPE_NONE;
    PCBIOS_DEVICE_COMMON    pDevCommon = CBIOS_NULL;

    Device = cbGetDeviceType(pcbe, DPModuleIndex, CBIOS_MODULE_TYPE_DP);

    if(Device == CBIOS_TYPE_NONE)
    {
        cbDebugPrint((MAKE_LEVEL(DP, DEBUG), "%s: invalid DP module index, LT failed! \n", FUNCTION_NAME));
        return;
    }

    pDevCommon = cbGetDeviceCommon(&pcbe->DeviceMgr, Device);

    if (!cbStrnCmp(pDevCommon->EdidStruct.Attribute.MonitorID, (CBIOS_UCHAR*)"TMA07E7", 7))
    {
        bTMA = CBIOS_TRUE;
    }

    bEDP = cbDIU_EDP_IsEDPSupported(pcbe, DPModuleIndex);

    if (pLinkTrainingParams->DpUsbMode == CBIOS_DP_ALTMODE)
    {
        UDEphyRegValue.Value = 0;
        UDEphyRegMask.Value = 0xFFFFFFFF;
        UDEphyRegMask.Vol_Swing_Control = 0;
        UDEphyRegMask.Preshoot_Set = 0;
        UDEphyRegMask.DeEmphasis_Set = 0;
        UDEphyRegMask.Index_and_Level = 0;
        UDEphyRegMask.enable_SW_swing_pp = 0;
        UDEphyRegMask.VER1P2 = 0;
        if (BitRate >= CBIOS_DP_LINK_SPEED_5400Mbps) // 5.4G
        {
            UDEphyRegValue.Vol_Swing_Control = 0x3;
            UDEphyRegValue.Preshoot_Set = 0;
            UDEphyRegValue.DeEmphasis_Set = 0x7;
            UDEphyRegValue.enable_SW_swing_pp = 1;
            UDEphyRegValue.VER1P2 = 1;

            UDEphyRegValue.Index_and_Level = 0x1;
            cbMMIOWriteReg32(pcbe, DP_REG_SWING[DPModuleIndex], UDEphyRegValue.Value, UDEphyRegMask.Value);

            UDEphyRegValue.Index_and_Level = 0x5;
            cbMMIOWriteReg32(pcbe, DP_REG_SWING[DPModuleIndex], UDEphyRegValue.Value, UDEphyRegMask.Value);

            UDEphyRegValue.Index_and_Level = 0x9;
            cbMMIOWriteReg32(pcbe, DP_REG_SWING[DPModuleIndex], UDEphyRegValue.Value, UDEphyRegMask.Value);

            UDEphyRegValue.Index_and_Level = 0x11;
            cbMMIOWriteReg32(pcbe, DP_REG_SWING[DPModuleIndex], UDEphyRegValue.Value, UDEphyRegMask.Value);

            UDEphyRegValue.Index_and_Level = 0x15;
            cbMMIOWriteReg32(pcbe, DP_REG_SWING[DPModuleIndex], UDEphyRegValue.Value, UDEphyRegMask.Value);

            UDEphyRegValue.Index_and_Level = 0x19;
            cbMMIOWriteReg32(pcbe, DP_REG_SWING[DPModuleIndex], UDEphyRegValue.Value, UDEphyRegMask.Value);

            UDEphyRegValue.Index_and_Level = 0x1d;
            cbMMIOWriteReg32(pcbe, DP_REG_SWING[DPModuleIndex], UDEphyRegValue.Value, UDEphyRegMask.Value);

            UDEphyRegValue.Index_and_Level = 0x21;
            cbMMIOWriteReg32(pcbe, DP_REG_SWING[DPModuleIndex], UDEphyRegValue.Value, UDEphyRegMask.Value);

            UDEphyRegValue.Index_and_Level = 0x25;
            cbMMIOWriteReg32(pcbe, DP_REG_SWING[DPModuleIndex], UDEphyRegValue.Value, UDEphyRegMask.Value);
        }
        else // 1.62G, 2.7G
        {
            UDEphyRegValue.Vol_Swing_Control = 0x3;
            UDEphyRegValue.Preshoot_Set = 0;
            UDEphyRegValue.DeEmphasis_Set = 0x7;
            UDEphyRegValue.enable_SW_swing_pp = 1;
            UDEphyRegValue.VER1P2 = 0;

            UDEphyRegValue.Index_and_Level = 0x1;
            cbMMIOWriteReg32(pcbe, DP_REG_SWING[DPModuleIndex], UDEphyRegValue.Value, UDEphyRegMask.Value);

            UDEphyRegValue.Index_and_Level = 0x5;
            cbMMIOWriteReg32(pcbe, DP_REG_SWING[DPModuleIndex], UDEphyRegValue.Value, UDEphyRegMask.Value);

            UDEphyRegValue.Index_and_Level = 0x9;
            cbMMIOWriteReg32(pcbe, DP_REG_SWING[DPModuleIndex], UDEphyRegValue.Value, UDEphyRegMask.Value);

            UDEphyRegValue.Index_and_Level = 0x11;
            cbMMIOWriteReg32(pcbe, DP_REG_SWING[DPModuleIndex], UDEphyRegValue.Value, UDEphyRegMask.Value);

            UDEphyRegValue.Index_and_Level = 0x15;
            cbMMIOWriteReg32(pcbe, DP_REG_SWING[DPModuleIndex], UDEphyRegValue.Value, UDEphyRegMask.Value);

            UDEphyRegValue.Index_and_Level = 0x19;
            cbMMIOWriteReg32(pcbe, DP_REG_SWING[DPModuleIndex], UDEphyRegValue.Value, UDEphyRegMask.Value);

            UDEphyRegValue.Index_and_Level = 0x1d;
            cbMMIOWriteReg32(pcbe, DP_REG_SWING[DPModuleIndex], UDEphyRegValue.Value, UDEphyRegMask.Value);

            UDEphyRegValue.Index_and_Level = 0x21;
            cbMMIOWriteReg32(pcbe, DP_REG_SWING[DPModuleIndex], UDEphyRegValue.Value, UDEphyRegMask.Value);

            UDEphyRegValue.Index_and_Level = 0x25;
            cbMMIOWriteReg32(pcbe, DP_REG_SWING[DPModuleIndex], UDEphyRegValue.Value, UDEphyRegMask.Value);
        }
    }
    else if (pcbe->ChipID == CHIPID_CNE001)  // standard DP or tunneling, CHX005
    {
        if (pcbe->SysBiosInfo.bLgaChip) // LGA
        {
            DPSwingRegMask.Value = 0xFFFFFFFF;
            DPSwingRegMask.enable_SW_swing_pp = 0;
            DPSwingRegMask.SW_swing_SW_PP_SW_post_cursor_load_index = 0;
            DPSwingRegMask.DP1_SW_swing = 0;
            DPSwingRegMask.DP1_SW_pp = 0;
            DPSwingRegMask.DP1_SW_post_cursor = 0;
            if (BitRate >= CBIOS_DP_LINK_SPEED_5400Mbps)
            {
                DPSwingRegValue.Value = 0;
                DPSwingRegValue.enable_SW_swing_pp = 1;
                DPSwingRegValue.SW_swing_SW_PP_SW_post_cursor_load_index = 1;
                DPSwingRegValue.DP1_SW_swing = ChangeSWswing ? 0x01 : 0x09;
                DPSwingRegValue.DP1_SW_pp = 0;
                DPSwingRegValue.DP1_SW_post_cursor = 0;
                cbMMIOWriteReg32(pcbe, DP_REG_SWING[DPModuleIndex], DPSwingRegValue.Value, DPSwingRegMask.Value);

                DPSwingRegValue.Value = 0;
                DPSwingRegValue.enable_SW_swing_pp = 1;
                DPSwingRegValue.SW_swing_SW_PP_SW_post_cursor_load_index = 5;
                DPSwingRegValue.DP1_SW_swing = 0x16;
                DPSwingRegValue.DP1_SW_pp = 0x6;
                DPSwingRegValue.DP1_SW_post_cursor = 0;
                cbMMIOWriteReg32(pcbe, DP_REG_SWING[DPModuleIndex], DPSwingRegValue.Value, DPSwingRegMask.Value);

                DPSwingRegValue.Value = 0;
                DPSwingRegValue.enable_SW_swing_pp = 1;
                DPSwingRegValue.SW_swing_SW_PP_SW_post_cursor_load_index = 9;
                DPSwingRegValue.DP1_SW_swing = 0x31;
                DPSwingRegValue.DP1_SW_pp = 0x18;
                DPSwingRegValue.DP1_SW_post_cursor = 0;
                cbMMIOWriteReg32(pcbe, DP_REG_SWING[DPModuleIndex], DPSwingRegValue.Value, DPSwingRegMask.Value);

                DPSwingRegValue.Value = 0;
                DPSwingRegValue.enable_SW_swing_pp = 1;
                DPSwingRegValue.SW_swing_SW_PP_SW_post_cursor_load_index = 0x11;
                DPSwingRegValue.DP1_SW_swing = 0x10;
                DPSwingRegValue.DP1_SW_pp = 0;
                DPSwingRegValue.DP1_SW_post_cursor = 0;
                cbMMIOWriteReg32(pcbe, DP_REG_SWING[DPModuleIndex], DPSwingRegValue.Value, DPSwingRegMask.Value);

                DPSwingRegValue.Value = 0;
                DPSwingRegValue.enable_SW_swing_pp = 1;
                DPSwingRegValue.SW_swing_SW_PP_SW_post_cursor_load_index = 0x15;
                DPSwingRegValue.DP1_SW_swing = 0x1D;
                DPSwingRegValue.DP1_SW_pp = 0x09;
                DPSwingRegValue.DP1_SW_post_cursor = 0;
                cbMMIOWriteReg32(pcbe, DP_REG_SWING[DPModuleIndex], DPSwingRegValue.Value, DPSwingRegMask.Value);

                DPSwingRegValue.Value = 0;
                DPSwingRegValue.enable_SW_swing_pp = 1;
                DPSwingRegValue.SW_swing_SW_PP_SW_post_cursor_load_index = 0x19;
                DPSwingRegValue.DP1_SW_swing = 0x38;
                DPSwingRegValue.DP1_SW_pp = 0x19;
                DPSwingRegValue.DP1_SW_post_cursor = 0;
                cbMMIOWriteReg32(pcbe, DP_REG_SWING[DPModuleIndex], DPSwingRegValue.Value, DPSwingRegMask.Value);

                DPSwingRegValue.Value = 0;
                DPSwingRegValue.enable_SW_swing_pp = 1;
                DPSwingRegValue.SW_swing_SW_PP_SW_post_cursor_load_index = 0x1D;
                DPSwingRegValue.DP1_SW_swing = 0x16;
                DPSwingRegValue.DP1_SW_pp = 0;
                DPSwingRegValue.DP1_SW_post_cursor = 0;
                cbMMIOWriteReg32(pcbe, DP_REG_SWING[DPModuleIndex], DPSwingRegValue.Value, DPSwingRegMask.Value);

                DPSwingRegValue.Value = 0;
                DPSwingRegValue.enable_SW_swing_pp = 1;
                DPSwingRegValue.SW_swing_SW_PP_SW_post_cursor_load_index = 0x21;
                DPSwingRegValue.DP1_SW_swing = 0x3E;
                DPSwingRegValue.DP1_SW_pp = 0xE;
                DPSwingRegValue.DP1_SW_post_cursor = 0;
                cbMMIOWriteReg32(pcbe, DP_REG_SWING[DPModuleIndex], DPSwingRegValue.Value, DPSwingRegMask.Value);

                DPSwingRegValue.Value = 0;
                DPSwingRegValue.enable_SW_swing_pp = 1;
                DPSwingRegValue.SW_swing_SW_PP_SW_post_cursor_load_index = 0x25;
                DPSwingRegValue.DP1_SW_swing = 0x2C;
                DPSwingRegValue.DP1_SW_pp = 0;
                DPSwingRegValue.DP1_SW_post_cursor = 0;
                cbMMIOWriteReg32(pcbe, DP_REG_SWING[DPModuleIndex], DPSwingRegValue.Value, DPSwingRegMask.Value);
            }
            else
            {
                DPSwingRegValue.Value = 0;
                DPSwingRegValue.enable_SW_swing_pp = 1;
                DPSwingRegValue.SW_swing_SW_PP_SW_post_cursor_load_index = 1;
                DPSwingRegValue.DP1_SW_swing = ChangeSWswing ? 0x01 : 0x09;
                DPSwingRegValue.DP1_SW_pp = 0;
                DPSwingRegValue.DP1_SW_post_cursor = 0;
                cbMMIOWriteReg32(pcbe, DP_REG_SWING[DPModuleIndex], DPSwingRegValue.Value, DPSwingRegMask.Value);

                DPSwingRegValue.Value = 0;
                DPSwingRegValue.enable_SW_swing_pp = 1;
                DPSwingRegValue.SW_swing_SW_PP_SW_post_cursor_load_index = 5;
                DPSwingRegValue.DP1_SW_swing = 0x13;
                DPSwingRegValue.DP1_SW_pp = 0x04;
                DPSwingRegValue.DP1_SW_post_cursor = 0;
                cbMMIOWriteReg32(pcbe, DP_REG_SWING[DPModuleIndex], DPSwingRegValue.Value, DPSwingRegMask.Value);

                DPSwingRegValue.Value = 0;
                DPSwingRegValue.enable_SW_swing_pp = 1;
                DPSwingRegValue.SW_swing_SW_PP_SW_post_cursor_load_index = 9;
                DPSwingRegValue.DP1_SW_swing = 0x16;
                DPSwingRegValue.DP1_SW_pp = 0x08;
                DPSwingRegValue.DP1_SW_post_cursor = 0;
                cbMMIOWriteReg32(pcbe, DP_REG_SWING[DPModuleIndex], DPSwingRegValue.Value, DPSwingRegMask.Value);

                DPSwingRegValue.Value = 0;
                DPSwingRegValue.enable_SW_swing_pp = 1;
                DPSwingRegValue.SW_swing_SW_PP_SW_post_cursor_load_index = 0x11;
                DPSwingRegValue.DP1_SW_swing = 0x0E;
                DPSwingRegValue.DP1_SW_pp = 0;
                DPSwingRegValue.DP1_SW_post_cursor = 0;
                cbMMIOWriteReg32(pcbe, DP_REG_SWING[DPModuleIndex], DPSwingRegValue.Value, DPSwingRegMask.Value);

                DPSwingRegValue.Value = 0;
                DPSwingRegValue.enable_SW_swing_pp = 1;
                DPSwingRegValue.SW_swing_SW_PP_SW_post_cursor_load_index = 0x15;
                DPSwingRegValue.DP1_SW_swing = 0x26;
                DPSwingRegValue.DP1_SW_pp = 0x05;
                DPSwingRegValue.DP1_SW_post_cursor = 0;
                cbMMIOWriteReg32(pcbe, DP_REG_SWING[DPModuleIndex], DPSwingRegValue.Value, DPSwingRegMask.Value);

                DPSwingRegValue.Value = 0;
                DPSwingRegValue.enable_SW_swing_pp = 1;
                DPSwingRegValue.SW_swing_SW_PP_SW_post_cursor_load_index = 0x19;
                DPSwingRegValue.DP1_SW_swing = 0x26;
                DPSwingRegValue.DP1_SW_pp = 0x9;
                DPSwingRegValue.DP1_SW_post_cursor = 0;
                cbMMIOWriteReg32(pcbe, DP_REG_SWING[DPModuleIndex], DPSwingRegValue.Value, DPSwingRegMask.Value);

                DPSwingRegValue.Value = 0;
                DPSwingRegValue.enable_SW_swing_pp = 1;
                DPSwingRegValue.SW_swing_SW_PP_SW_post_cursor_load_index = 0x1D;
                DPSwingRegValue.DP1_SW_swing = 0x16;
                DPSwingRegValue.DP1_SW_pp = 1;
                DPSwingRegValue.DP1_SW_post_cursor = 0;
                cbMMIOWriteReg32(pcbe, DP_REG_SWING[DPModuleIndex], DPSwingRegValue.Value, DPSwingRegMask.Value);

                DPSwingRegValue.Value = 0;
                DPSwingRegValue.enable_SW_swing_pp = 1;
                DPSwingRegValue.SW_swing_SW_PP_SW_post_cursor_load_index = 0x21;
                DPSwingRegValue.DP1_SW_swing = 0x1A;
                DPSwingRegValue.DP1_SW_pp = 0x6;
                DPSwingRegValue.DP1_SW_post_cursor = 0;
                cbMMIOWriteReg32(pcbe, DP_REG_SWING[DPModuleIndex], DPSwingRegValue.Value, DPSwingRegMask.Value);

                DPSwingRegValue.Value = 0;
                DPSwingRegValue.enable_SW_swing_pp = 1;
                DPSwingRegValue.SW_swing_SW_PP_SW_post_cursor_load_index = 0x25;
                DPSwingRegValue.DP1_SW_swing = 0x29;
                DPSwingRegValue.DP1_SW_pp = 0;
                DPSwingRegValue.DP1_SW_post_cursor = 0;
                cbMMIOWriteReg32(pcbe, DP_REG_SWING[DPModuleIndex], DPSwingRegValue.Value, DPSwingRegMask.Value);
                cbMMIOWriteReg32(pcbe, DP_REG_SWING[DPModuleIndex], 0x2687F8, 0xFFC00000);
            }
        }
        else // BGA
        {
            DPSwingRegMask.Value = 0xFFFFFFFF;
            DPSwingRegMask.enable_SW_swing_pp = 0;
            DPSwingRegMask.SW_swing_SW_PP_SW_post_cursor_load_index = 0;
            DPSwingRegMask.DP1_SW_swing = 0;
            DPSwingRegMask.DP1_SW_pp = 0;
            DPSwingRegMask.DP1_SW_post_cursor = 0;
            if (BitRate >= CBIOS_DP_LINK_SPEED_5400Mbps)
            {
                DPSwingRegValue.Value = 0;
                DPSwingRegValue.enable_SW_swing_pp = 1;
                DPSwingRegValue.SW_swing_SW_PP_SW_post_cursor_load_index = 1;
                if(bEDP && bTMA)
                {
                    DPSwingRegValue.DP1_SW_swing = 0x3F;
                    DPSwingRegValue.DP1_SW_pp = 0x03;
                    DPSwingRegValue.DP1_SW_post_cursor = 0;
                }
                else
                {
                    DPSwingRegValue.DP1_SW_swing = ChangeSWswing ? 0x01 : 0x09;
                    DPSwingRegValue.DP1_SW_pp = 0;
                    DPSwingRegValue.DP1_SW_post_cursor = 0;
                }
                cbMMIOWriteReg32(pcbe, DP_REG_SWING[DPModuleIndex], DPSwingRegValue.Value, DPSwingRegMask.Value);

                DPSwingRegValue.Value = 0;
                DPSwingRegValue.enable_SW_swing_pp = 1;
                DPSwingRegValue.SW_swing_SW_PP_SW_post_cursor_load_index = 5;
                if(bEDP && bTMA)
                {
                    DPSwingRegValue.DP1_SW_swing = 0x3F;
                    DPSwingRegValue.DP1_SW_pp = 0x03;
                    DPSwingRegValue.DP1_SW_post_cursor = 0;
                }
                else
                {
                    DPSwingRegValue.DP1_SW_swing = 0x16;
                    DPSwingRegValue.DP1_SW_pp = 0x6;
                    DPSwingRegValue.DP1_SW_post_cursor = 0;
                }
                cbMMIOWriteReg32(pcbe, DP_REG_SWING[DPModuleIndex], DPSwingRegValue.Value, DPSwingRegMask.Value);

                DPSwingRegValue.Value = 0;
                DPSwingRegValue.enable_SW_swing_pp = 1;
                DPSwingRegValue.SW_swing_SW_PP_SW_post_cursor_load_index = 9;
                if(bEDP && bTMA)
                {
                    DPSwingRegValue.DP1_SW_swing = 0x3F;
                    DPSwingRegValue.DP1_SW_pp = 0x03;
                    DPSwingRegValue.DP1_SW_post_cursor = 0;
                }
                else
                {
                    DPSwingRegValue.DP1_SW_swing = 0x31;
                    DPSwingRegValue.DP1_SW_pp = 0x18;
                    DPSwingRegValue.DP1_SW_post_cursor = 0;
                }
                cbMMIOWriteReg32(pcbe, DP_REG_SWING[DPModuleIndex], DPSwingRegValue.Value, DPSwingRegMask.Value);

                DPSwingRegValue.Value = 0;
                DPSwingRegValue.enable_SW_swing_pp = 1;
                DPSwingRegValue.SW_swing_SW_PP_SW_post_cursor_load_index = 0x11;
                if(bEDP && bTMA)
                {
                    DPSwingRegValue.DP1_SW_swing = 0x3F;
                    DPSwingRegValue.DP1_SW_pp = 0x03;
                    DPSwingRegValue.DP1_SW_post_cursor = 0;
                }
                else
                {
                    DPSwingRegValue.DP1_SW_swing = 0x10;
                    DPSwingRegValue.DP1_SW_pp = 0;
                    DPSwingRegValue.DP1_SW_post_cursor = 0;
                }
                cbMMIOWriteReg32(pcbe, DP_REG_SWING[DPModuleIndex], DPSwingRegValue.Value, DPSwingRegMask.Value);

                DPSwingRegValue.Value = 0;
                DPSwingRegValue.enable_SW_swing_pp = 1;
                DPSwingRegValue.SW_swing_SW_PP_SW_post_cursor_load_index = 0x15;
                if(bEDP && bTMA)
                {
                    DPSwingRegValue.DP1_SW_swing = 0x3F;
                    DPSwingRegValue.DP1_SW_pp = 0x03;
                    DPSwingRegValue.DP1_SW_post_cursor = 0;
                }
                else
                {
                    DPSwingRegValue.DP1_SW_swing = 0x1D;
                    DPSwingRegValue.DP1_SW_pp = 0x09;
                    DPSwingRegValue.DP1_SW_post_cursor = 0;
                }
                cbMMIOWriteReg32(pcbe, DP_REG_SWING[DPModuleIndex], DPSwingRegValue.Value, DPSwingRegMask.Value);

                DPSwingRegValue.Value = 0;
                DPSwingRegValue.enable_SW_swing_pp = 1;
                DPSwingRegValue.SW_swing_SW_PP_SW_post_cursor_load_index = 0x19;
                if(bEDP && bTMA)
                {
                    DPSwingRegValue.DP1_SW_swing = 0x3F;
                    DPSwingRegValue.DP1_SW_pp = 0x03;
                    DPSwingRegValue.DP1_SW_post_cursor = 0;
                }
                else
                {
                    DPSwingRegValue.DP1_SW_swing = 0x38;
                    DPSwingRegValue.DP1_SW_pp = 0x19;
                    DPSwingRegValue.DP1_SW_post_cursor = 0;
                }
                cbMMIOWriteReg32(pcbe, DP_REG_SWING[DPModuleIndex], DPSwingRegValue.Value, DPSwingRegMask.Value);

                DPSwingRegValue.Value = 0;
                DPSwingRegValue.enable_SW_swing_pp = 1;
                DPSwingRegValue.SW_swing_SW_PP_SW_post_cursor_load_index = 0x1D;
                if(bEDP && bTMA)
                {
                    DPSwingRegValue.DP1_SW_swing = 0x3F;
                    DPSwingRegValue.DP1_SW_pp = 0x03;
                    DPSwingRegValue.DP1_SW_post_cursor = 0;
                }
                else
                {
                    DPSwingRegValue.DP1_SW_swing = 0x16;
                    DPSwingRegValue.DP1_SW_pp = 0;
                    DPSwingRegValue.DP1_SW_post_cursor = 0;
                }
                cbMMIOWriteReg32(pcbe, DP_REG_SWING[DPModuleIndex], DPSwingRegValue.Value, DPSwingRegMask.Value);

                DPSwingRegValue.Value = 0;
                DPSwingRegValue.enable_SW_swing_pp = 1;
                DPSwingRegValue.SW_swing_SW_PP_SW_post_cursor_load_index = 0x21;
                if(bEDP && bTMA)
                {
                    DPSwingRegValue.DP1_SW_swing = 0x3F;
                    DPSwingRegValue.DP1_SW_pp = 0x03;
                    DPSwingRegValue.DP1_SW_post_cursor = 0;
                }
                else
                {
                    DPSwingRegValue.DP1_SW_swing = 0x39;
                    DPSwingRegValue.DP1_SW_pp = 0xC;
                    DPSwingRegValue.DP1_SW_post_cursor = 0;
                }
                cbMMIOWriteReg32(pcbe, DP_REG_SWING[DPModuleIndex], DPSwingRegValue.Value, DPSwingRegMask.Value);

                DPSwingRegValue.Value = 0;
                DPSwingRegValue.enable_SW_swing_pp = 1;
                DPSwingRegValue.SW_swing_SW_PP_SW_post_cursor_load_index = 0x25;
                if(bEDP && bTMA)
                {
                    DPSwingRegValue.DP1_SW_swing = 0x3F;
                    DPSwingRegValue.DP1_SW_pp = 0x03;
                    DPSwingRegValue.DP1_SW_post_cursor = 0;
                }
                else
                {
                    DPSwingRegValue.DP1_SW_swing = 0x29;
                    DPSwingRegValue.DP1_SW_pp = 0;
                    DPSwingRegValue.DP1_SW_post_cursor = 0;
                }
                cbMMIOWriteReg32(pcbe, DP_REG_SWING[DPModuleIndex], DPSwingRegValue.Value, DPSwingRegMask.Value);
            }
            else
            {
                DPSwingRegValue.Value = 0;
                DPSwingRegValue.enable_SW_swing_pp = 1;
                DPSwingRegValue.SW_swing_SW_PP_SW_post_cursor_load_index = 1;
                if(bEDP && bTMA)
                {
                    DPSwingRegValue.DP1_SW_swing = 0x16;
                    DPSwingRegValue.DP1_SW_pp = 1;
                    DPSwingRegValue.DP1_SW_post_cursor = 0;
                }
                else
                {
                    DPSwingRegValue.DP1_SW_swing = ChangeSWswing ? 0x01 : 0x09;
                    DPSwingRegValue.DP1_SW_pp = 0;
                    DPSwingRegValue.DP1_SW_post_cursor = 0;
                }
                cbMMIOWriteReg32(pcbe, DP_REG_SWING[DPModuleIndex], DPSwingRegValue.Value, DPSwingRegMask.Value);

                DPSwingRegValue.Value = 0;
                DPSwingRegValue.enable_SW_swing_pp = 1;
                DPSwingRegValue.SW_swing_SW_PP_SW_post_cursor_load_index = 5;
                if(bEDP && bTMA)
                {
                    DPSwingRegValue.DP1_SW_swing = 0x16;
                    DPSwingRegValue.DP1_SW_pp = 1;
                    DPSwingRegValue.DP1_SW_post_cursor = 0;
                }
                else
                {
                    DPSwingRegValue.DP1_SW_swing = 0x13;
                    DPSwingRegValue.DP1_SW_pp = 0x04;
                    DPSwingRegValue.DP1_SW_post_cursor = 0;
                }
                cbMMIOWriteReg32(pcbe, DP_REG_SWING[DPModuleIndex], DPSwingRegValue.Value, DPSwingRegMask.Value);

                DPSwingRegValue.Value = 0;
                DPSwingRegValue.enable_SW_swing_pp = 1;
                DPSwingRegValue.SW_swing_SW_PP_SW_post_cursor_load_index = 9;
                if(bEDP && bTMA)
                {
                    DPSwingRegValue.DP1_SW_swing = 0x16;
                    DPSwingRegValue.DP1_SW_pp = 1;
                    DPSwingRegValue.DP1_SW_post_cursor = 0;
                }
                else
                {
                    DPSwingRegValue.DP1_SW_swing = 0x16;
                    DPSwingRegValue.DP1_SW_pp = 0x08;
                    DPSwingRegValue.DP1_SW_post_cursor = 0;
                }
                cbMMIOWriteReg32(pcbe, DP_REG_SWING[DPModuleIndex], DPSwingRegValue.Value, DPSwingRegMask.Value);

                DPSwingRegValue.Value = 0;
                DPSwingRegValue.enable_SW_swing_pp = 1;
                DPSwingRegValue.SW_swing_SW_PP_SW_post_cursor_load_index = 0x11;
                if(bEDP && bTMA)
                {
                    DPSwingRegValue.DP1_SW_swing = 0x16;
                    DPSwingRegValue.DP1_SW_pp = 1;
                    DPSwingRegValue.DP1_SW_post_cursor = 0;
                }
                else
                {
                    DPSwingRegValue.DP1_SW_swing = 0x0E;
                    DPSwingRegValue.DP1_SW_pp = 0;
                    DPSwingRegValue.DP1_SW_post_cursor = 0;
                }
                cbMMIOWriteReg32(pcbe, DP_REG_SWING[DPModuleIndex], DPSwingRegValue.Value, DPSwingRegMask.Value);

                DPSwingRegValue.Value = 0;
                DPSwingRegValue.enable_SW_swing_pp = 1;
                DPSwingRegValue.SW_swing_SW_PP_SW_post_cursor_load_index = 0x15;
                if(bEDP && bTMA)
                {
                    DPSwingRegValue.DP1_SW_swing = 0x16;
                    DPSwingRegValue.DP1_SW_pp = 1;
                    DPSwingRegValue.DP1_SW_post_cursor = 0;
                }
                else
                {
                    DPSwingRegValue.DP1_SW_swing = 0x26;
                    DPSwingRegValue.DP1_SW_pp = 0x05;
                    DPSwingRegValue.DP1_SW_post_cursor = 0;
                }
                cbMMIOWriteReg32(pcbe, DP_REG_SWING[DPModuleIndex], DPSwingRegValue.Value, DPSwingRegMask.Value);

                DPSwingRegValue.Value = 0;
                DPSwingRegValue.enable_SW_swing_pp = 1;
                DPSwingRegValue.SW_swing_SW_PP_SW_post_cursor_load_index = 0x19;
                if(bEDP && bTMA)
                {
                    DPSwingRegValue.DP1_SW_swing = 0x16;
                    DPSwingRegValue.DP1_SW_pp = 1;
                    DPSwingRegValue.DP1_SW_post_cursor = 0;
                }
                else
                {
                    DPSwingRegValue.DP1_SW_swing = 0x24;
                    DPSwingRegValue.DP1_SW_pp = 0x9;
                    DPSwingRegValue.DP1_SW_post_cursor = 0;
                }
                cbMMIOWriteReg32(pcbe, DP_REG_SWING[DPModuleIndex], DPSwingRegValue.Value, DPSwingRegMask.Value);

                DPSwingRegValue.Value = 0;
                DPSwingRegValue.enable_SW_swing_pp = 1;
                DPSwingRegValue.SW_swing_SW_PP_SW_post_cursor_load_index = 0x1D;
                DPSwingRegValue.DP1_SW_swing = 0x16;
                DPSwingRegValue.DP1_SW_pp = 1;
                DPSwingRegValue.DP1_SW_post_cursor = 0;
                cbMMIOWriteReg32(pcbe, DP_REG_SWING[DPModuleIndex], DPSwingRegValue.Value, DPSwingRegMask.Value);

                DPSwingRegValue.Value = 0;
                DPSwingRegValue.enable_SW_swing_pp = 1;
                DPSwingRegValue.SW_swing_SW_PP_SW_post_cursor_load_index = 0x21;
                DPSwingRegValue.DP1_SW_swing = 0x1A;
                DPSwingRegValue.DP1_SW_pp = 0x6;
                DPSwingRegValue.DP1_SW_post_cursor = 0;
                cbMMIOWriteReg32(pcbe, DP_REG_SWING[DPModuleIndex], DPSwingRegValue.Value, DPSwingRegMask.Value);

                DPSwingRegValue.Value = 0;
                DPSwingRegValue.enable_SW_swing_pp = 1;
                DPSwingRegValue.SW_swing_SW_PP_SW_post_cursor_load_index = 0x25;
                DPSwingRegValue.DP1_SW_swing = 0x29;
                DPSwingRegValue.DP1_SW_pp = 0;
                DPSwingRegValue.DP1_SW_post_cursor = 0;
                cbMMIOWriteReg32(pcbe, DP_REG_SWING[DPModuleIndex], DPSwingRegValue.Value, DPSwingRegMask.Value);
                cbMMIOWriteReg32(pcbe, DP_REG_SWING[DPModuleIndex], 0x2687F8, 0xFFC00000);
            }
        }
    }
    else  // standard DP or tunneling
    {
        DPSwingRegMask.Value = 0xFFFFFFFF;
        DPSwingRegMask.enable_SW_swing_pp = 0;
        DPSwingRegMask.SW_swing_SW_PP_SW_post_cursor_load_index = 0;
        DPSwingRegMask.DP1_SW_swing = 0;
        DPSwingRegMask.DP1_SW_pp = 0;
        DPSwingRegMask.DP1_SW_post_cursor = 0;
        if (BitRate >= CBIOS_DP_LINK_SPEED_5400Mbps)
        {
            DPSwingRegValue.Value = 0;
            DPSwingRegValue.enable_SW_swing_pp = 1;
            DPSwingRegValue.SW_swing_SW_PP_SW_post_cursor_load_index = 1;
            DPSwingRegValue.DP1_SW_swing = ChangeSWswing ? 0x01 : 0x09;
            DPSwingRegValue.DP1_SW_pp = 0;
            DPSwingRegValue.DP1_SW_post_cursor = 0;
            cbMMIOWriteReg32(pcbe, DP_REG_SWING[DPModuleIndex], DPSwingRegValue.Value, DPSwingRegMask.Value);

            DPSwingRegValue.Value = 0;
            DPSwingRegValue.enable_SW_swing_pp = 1;
            DPSwingRegValue.SW_swing_SW_PP_SW_post_cursor_load_index = 5;
            DPSwingRegValue.DP1_SW_swing = 0x16; // CHX004: 0x16
            DPSwingRegValue.DP1_SW_pp = 0x06; // CHX004: 0x06
            DPSwingRegValue.DP1_SW_post_cursor = 0;
            cbMMIOWriteReg32(pcbe, DP_REG_SWING[DPModuleIndex], DPSwingRegValue.Value, DPSwingRegMask.Value);

            DPSwingRegValue.Value = 0;
            DPSwingRegValue.enable_SW_swing_pp = 1;
            DPSwingRegValue.SW_swing_SW_PP_SW_post_cursor_load_index = 9;
            DPSwingRegValue.DP1_SW_swing = 0x31; // CHX004: 0x31
            DPSwingRegValue.DP1_SW_pp = 0x18; //CHX004: 0x18
            DPSwingRegValue.DP1_SW_post_cursor = 0;
            cbMMIOWriteReg32(pcbe, DP_REG_SWING[DPModuleIndex], DPSwingRegValue.Value, DPSwingRegMask.Value);

            DPSwingRegValue.Value = 0;
            DPSwingRegValue.enable_SW_swing_pp = 1;
            DPSwingRegValue.SW_swing_SW_PP_SW_post_cursor_load_index = 0x11;
            DPSwingRegValue.DP1_SW_swing = 0x10; // CHX004: 0x10
            DPSwingRegValue.DP1_SW_pp = 0;
            DPSwingRegValue.DP1_SW_post_cursor = 0;
            cbMMIOWriteReg32(pcbe, DP_REG_SWING[DPModuleIndex], DPSwingRegValue.Value, DPSwingRegMask.Value);

            DPSwingRegValue.Value = 0;
            DPSwingRegValue.enable_SW_swing_pp = 1;
            DPSwingRegValue.SW_swing_SW_PP_SW_post_cursor_load_index = 0x15;
            DPSwingRegValue.DP1_SW_swing = 0x1D; // CHX004: 0x1D
            DPSwingRegValue.DP1_SW_pp = 0x09; // CHX004: 0x09
            DPSwingRegValue.DP1_SW_post_cursor = 0;
            cbMMIOWriteReg32(pcbe, DP_REG_SWING[DPModuleIndex], DPSwingRegValue.Value, DPSwingRegMask.Value);

            DPSwingRegValue.Value = 0;
            DPSwingRegValue.enable_SW_swing_pp = 1;
            DPSwingRegValue.SW_swing_SW_PP_SW_post_cursor_load_index = 0x19;
            DPSwingRegValue.DP1_SW_swing = 0x34; // CHX004: 0x34
            DPSwingRegValue.DP1_SW_pp = 0x19; // CHX004: 0x19
            DPSwingRegValue.DP1_SW_post_cursor = 0;
            cbMMIOWriteReg32(pcbe, DP_REG_SWING[DPModuleIndex], DPSwingRegValue.Value, DPSwingRegMask.Value);

            DPSwingRegValue.Value = 0;
            DPSwingRegValue.enable_SW_swing_pp = 1;
            DPSwingRegValue.SW_swing_SW_PP_SW_post_cursor_load_index = 0x1D;
            DPSwingRegValue.DP1_SW_swing = 0x16; // CHX004: 0x16
            DPSwingRegValue.DP1_SW_pp = 0;
            DPSwingRegValue.DP1_SW_post_cursor = 0;
            cbMMIOWriteReg32(pcbe, DP_REG_SWING[DPModuleIndex], DPSwingRegValue.Value, DPSwingRegMask.Value);

            DPSwingRegValue.Value = 0;
            DPSwingRegValue.enable_SW_swing_pp = 1;
            DPSwingRegValue.SW_swing_SW_PP_SW_post_cursor_load_index = 0x21;
            DPSwingRegValue.DP1_SW_swing = 0x3B; // CHX004: 0x3B
            DPSwingRegValue.DP1_SW_pp = 0x0C; // CHX004: 0x0C
            DPSwingRegValue.DP1_SW_post_cursor = 0;
            cbMMIOWriteReg32(pcbe, DP_REG_SWING[DPModuleIndex], DPSwingRegValue.Value, DPSwingRegMask.Value);

            DPSwingRegValue.Value = 0;
            DPSwingRegValue.enable_SW_swing_pp = 1;
            DPSwingRegValue.SW_swing_SW_PP_SW_post_cursor_load_index = 0x25;
            DPSwingRegValue.DP1_SW_swing = 0x2C; //0x2C
            DPSwingRegValue.DP1_SW_pp = 0;
            DPSwingRegValue.DP1_SW_post_cursor = 0;
            cbMMIOWriteReg32(pcbe, DP_REG_SWING[DPModuleIndex], DPSwingRegValue.Value, DPSwingRegMask.Value);
        }
        else
        {
            DPSwingRegValue.Value = 0;
            DPSwingRegValue.enable_SW_swing_pp = 1;
            DPSwingRegValue.SW_swing_SW_PP_SW_post_cursor_load_index = 1;
            DPSwingRegValue.DP1_SW_swing = ChangeSWswing ? 0x01 : 0x09;
            DPSwingRegValue.DP1_SW_pp = 0;
            DPSwingRegValue.DP1_SW_post_cursor = 0;
            cbMMIOWriteReg32(pcbe, DP_REG_SWING[DPModuleIndex], DPSwingRegValue.Value, DPSwingRegMask.Value);

            DPSwingRegValue.Value = 0;
            DPSwingRegValue.enable_SW_swing_pp = 1;
            DPSwingRegValue.SW_swing_SW_PP_SW_post_cursor_load_index = 5;
            DPSwingRegValue.DP1_SW_swing = 0x13; // CHX004: 0x13
            DPSwingRegValue.DP1_SW_pp = 0x04; // CHX004: 0x04
            DPSwingRegValue.DP1_SW_post_cursor = 0;
            cbMMIOWriteReg32(pcbe, DP_REG_SWING[DPModuleIndex], DPSwingRegValue.Value, DPSwingRegMask.Value);

            DPSwingRegValue.Value = 0;
            DPSwingRegValue.enable_SW_swing_pp = 1;
            DPSwingRegValue.SW_swing_SW_PP_SW_post_cursor_load_index = 9;
            DPSwingRegValue.DP1_SW_swing = 0x16; // CHX004: 0x16
            DPSwingRegValue.DP1_SW_pp = 0x08; // CHX004: 0x08
            DPSwingRegValue.DP1_SW_post_cursor = 0;
            cbMMIOWriteReg32(pcbe, DP_REG_SWING[DPModuleIndex], DPSwingRegValue.Value, DPSwingRegMask.Value);

            DPSwingRegValue.Value = 0;
            DPSwingRegValue.enable_SW_swing_pp = 1;
            DPSwingRegValue.SW_swing_SW_PP_SW_post_cursor_load_index = 0x11;
            DPSwingRegValue.DP1_SW_swing = 0xE; // CHX004: 0x0E
            DPSwingRegValue.DP1_SW_pp = 0;
            DPSwingRegValue.DP1_SW_post_cursor = 0;
            cbMMIOWriteReg32(pcbe, DP_REG_SWING[DPModuleIndex], DPSwingRegValue.Value, DPSwingRegMask.Value);

            DPSwingRegValue.Value = 0;
            DPSwingRegValue.enable_SW_swing_pp = 1;
            DPSwingRegValue.SW_swing_SW_PP_SW_post_cursor_load_index = 0x15;
            DPSwingRegValue.DP1_SW_swing = 0x26; // CHX004: 0x26
            DPSwingRegValue.DP1_SW_pp = 0x05; // CHX004: 0x05
            DPSwingRegValue.DP1_SW_post_cursor = 0;
            cbMMIOWriteReg32(pcbe, DP_REG_SWING[DPModuleIndex], DPSwingRegValue.Value, DPSwingRegMask.Value);

            DPSwingRegValue.Value = 0;
            DPSwingRegValue.enable_SW_swing_pp = 1;
            DPSwingRegValue.SW_swing_SW_PP_SW_post_cursor_load_index = 0x19;
            DPSwingRegValue.DP1_SW_swing = 0x26; // CHX004: 0x26
            DPSwingRegValue.DP1_SW_pp = 0x09;
            DPSwingRegValue.DP1_SW_post_cursor = 0;
            cbMMIOWriteReg32(pcbe, DP_REG_SWING[DPModuleIndex], DPSwingRegValue.Value, DPSwingRegMask.Value);

            DPSwingRegValue.Value = 0;
            DPSwingRegValue.enable_SW_swing_pp = 1;
            DPSwingRegValue.SW_swing_SW_PP_SW_post_cursor_load_index = 0x1D;
            DPSwingRegValue.DP1_SW_swing = 0x16; // CHX004: 0x16
            DPSwingRegValue.DP1_SW_pp = 0x01;
            DPSwingRegValue.DP1_SW_post_cursor = 0;
            cbMMIOWriteReg32(pcbe, DP_REG_SWING[DPModuleIndex], DPSwingRegValue.Value, DPSwingRegMask.Value);

            DPSwingRegValue.Value = 0;
            DPSwingRegValue.enable_SW_swing_pp = 1;
            DPSwingRegValue.SW_swing_SW_PP_SW_post_cursor_load_index = 0x21;
            DPSwingRegValue.DP1_SW_swing = 0x18; // CHX004: 0x18
            DPSwingRegValue.DP1_SW_pp = 0x06; // CHX004: 0x06
            DPSwingRegValue.DP1_SW_post_cursor = 0;
            cbMMIOWriteReg32(pcbe, DP_REG_SWING[DPModuleIndex], DPSwingRegValue.Value, DPSwingRegMask.Value);

            DPSwingRegValue.Value = 0;
            DPSwingRegValue.enable_SW_swing_pp = 1;
            DPSwingRegValue.SW_swing_SW_PP_SW_post_cursor_load_index = 0x25;
            DPSwingRegValue.DP1_SW_swing = 0x29; // CHX004: 0x29
            DPSwingRegValue.DP1_SW_pp = 0;
            DPSwingRegValue.DP1_SW_post_cursor = 0;
            cbMMIOWriteReg32(pcbe, DP_REG_SWING[DPModuleIndex], DPSwingRegValue.Value, DPSwingRegMask.Value);
            cbMMIOWriteReg32(pcbe, DP_REG_SWING[DPModuleIndex], 0x2687F8, 0xFFC00000);
        }
    }
}

CBIOS_BOOL cbDIU_DP_LT_SetTunnelPara(PCBIOS_EXTENSION_COMMON pcbe, CBIOS_MODULE_INDEX DPModuleIndex, PCBIOS_LINK_TRAINING_PARAMS pLinkTrainingParams)
{
    CBIOS_BOOL  bStatus = CBIOS_TRUE;
    REG_MM33558 LttprConfig1, LttprConfig1Mask;
    CBIOS_U8    Data[1] = {0};

    LttprConfig1.Value = 0;
    LttprConfig1.Tunnel_DP_EN = CBIOS_TRUE;
    LttprConfig1.Force_1st_AV_BS = CBIOS_TRUE;
    LttprConfig1Mask.Value = 0xFFFFFFFF;
    LttprConfig1Mask.Tunnel_DP_EN = 0;
    LttprConfig1Mask.Force_1st_AV_BS = 0;

    if (!pLinkTrainingParams->LttprCount)  // Non-LTTPR
    {
        cbDebugPrint((MAKE_LEVEL(DP, INFO), "%s: NON-LTTPR mode!\n", FUNCTION_NAME));
        LttprConfig1.LTTPR_Connected       = CBIOS_FALSE;
        LttprConfig1.LT_DPCD205_NOT_RD     = CBIOS_TRUE;
        LttprConfig1.EN_RDINTVAL_COMPS     = CBIOS_TRUE;
        LttprConfig1.COMPS_TIME_SEL        = 0x1;

        LttprConfig1Mask.LTTPR_Connected   = 0;
        LttprConfig1Mask.EN_RDINTVAL_COMPS = 0;
        LttprConfig1Mask.LT_DPCD205_NOT_RD = 0;
        LttprConfig1Mask.COMPS_TIME_SEL    = 0;
    }
    else if (!pLinkTrainingParams->LttprNonTransparent) // LTTPR Transparent
    {
        cbDebugPrint((MAKE_LEVEL(DP, INFO), "%s: LTTPR Transperent mode!\n", FUNCTION_NAME));
        LttprConfig1.LTTPR_Connected       = CBIOS_TRUE;
        LttprConfig1.Non_Transparent       = CBIOS_FALSE;
        LttprConfig1.LT_DPCD205_NOT_RD     = CBIOS_TRUE;
        LttprConfig1.EN_RDINTVAL_COMPS     = CBIOS_TRUE;
        LttprConfig1.COMPS_TIME_SEL        = 0x1;

        LttprConfig1Mask.LTTPR_Connected   = 0;
        LttprConfig1Mask.Non_Transparent   = 0;
        LttprConfig1Mask.LT_DPCD205_NOT_RD = 0;
        LttprConfig1Mask.EN_RDINTVAL_COMPS = 0;
        LttprConfig1Mask.COMPS_TIME_SEL    = 0;
    }
    else if (pLinkTrainingParams->LttprNonTransparent) // LTTPR Non-Transparent
    {
        cbDebugPrint((MAKE_LEVEL(DP, INFO), "%s: LTTPR Non-Transperent mode!\n", FUNCTION_NAME));
        LttprConfig1.LTTPR_Connected       = CBIOS_TRUE;
        LttprConfig1.Non_Transparent       = CBIOS_TRUE;
        LttprConfig1.LTTPR_Count           = pLinkTrainingParams->LttprCount - 1;
        LttprConfig1.LT_DPCD205_NOT_RD     = CBIOS_FALSE;
        LttprConfig1.EN_RDINTVAL_COMPS     = CBIOS_FALSE;
        LttprConfig1.Force_TPS4_DIS_Scramble = CBIOS_TRUE;

        LttprConfig1Mask.LTTPR_Connected   = 0;
        LttprConfig1Mask.Non_Transparent   = 0;
        LttprConfig1Mask.LTTPR_Count       = 0;
        LttprConfig1Mask.LT_DPCD205_NOT_RD = 0;
        LttprConfig1Mask.EN_RDINTVAL_COMPS = 0;
        LttprConfig1Mask.Force_TPS4_DIS_Scramble = 0;
    }
    cbMMIOWriteReg32(pcbe, DP_REG_LTTPR_CFG1[DPModuleIndex], LttprConfig1.Value, LttprConfig1Mask.Value);

    if (pLinkTrainingParams->LttprCount)
    {
        Data[0] = pLinkTrainingParams->LttprNonTransparent ? 0xAA : 0x55;
        if(!cbDIU_Aux_Dpcd_Write(pcbe, DPModuleIndex, 0xF0003, Data, 1))
        {
            cbDebugPrint((MAKE_LEVEL(DP, ERROR), "%s: Aux set PHY_REPEATER_MODE Error before LT!\n", FUNCTION_NAME));
            bStatus = CBIOS_FALSE;
        }
    }

    return bStatus;
}

CBIOS_BOOL cbDIU_DP_LinkTrainingHw(PCBIOS_VOID pvcbe, CBIOS_MODULE_INDEX DPModuleIndex, PCBIOS_LINK_TRAINING_PARAMS pLinkTrainingParams)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    CBIOS_U32               Device = CBIOS_TYPE_NONE;
    PCBIOS_DEVICE_COMMON    pDevCommon = CBIOS_NULL;
    PCBIOS_DP_CONTEXT       pDpContext = CBIOS_NULL;
    CBIOS_BOOL  bStatus = CBIOS_FALSE;
    CBIOS_U32   i = 0;
    CBIOS_U32   bEQFallBack = 0;
    CBIOS_U32   BitRate = 0;
    CBIOS_U32   ulLaneNum, MaxLaneCount = 0;
    CBIOS_U32   BitRateIndex = 0, MaxBitRateIndex = 0;
    CBIOS_U8    Data[2] = {0};
    REG_MM8214  DPLinkRegValue, DPLinkRegMask;
    REG_MM8218  DPGenCtrlRegValue;
    REG_MM8240  DPEnableRegValue;
    REG_MM82CC  DPEphyCtrlRegValue, DPEphyCtrlRegMask;
    REG_MM8330  DPAuxTimerRegValue;
    REG_MM8338  DPMuteRegValue, DPMuteRegMask;
    REG_MM8368  DPSwingRegValue, DPSwingRegMask;
    REG_MM334C8 DPLinkCtrlRegValue, DPLinkCtrlRegMask;
    REG_MM334DC DPCtrlRegValue, DPCtrlRegMask;
    REG_MM33554 LttprConfig0, LttprConfig0Mask;
    REG_MM33570 DPEphyMuxHpd, DPEphyMuxHpdMask;
    DPCD_REG_00100 DPCD_00100;
    DPCD_REG_00101 DPCD_00101;
    DPCD_REG_0010A DPCD_0010A;
    DPCD_REG_00102 DPCD_00102;
    CBIOS_BOOL  ChangeSWswing = CBIOS_FALSE;
    CBIOS_BOOL  UseReducedLTPara = CBIOS_TRUE;

    //For DP test 4.3.1.4, if use SW to try 1.62 when 2.7 fail, the ulMaxRetryCount should set to 1.
    CBIOS_U32   ulRetryCount = 0, ulMaxRetryCount = 3;

    if(DPModuleIndex >= DP_MODU_NUM)
    {
        cbDebugPrint((MAKE_LEVEL(DP, ERROR), "Invalid module index in %s\n", FUNCTION_NAME));
        return CBIOS_FALSE;
    }

    cbTraceEnter(DP);

    Device = cbGetDeviceType(pcbe, DPModuleIndex, CBIOS_MODULE_TYPE_DP);

    if(Device == CBIOS_TYPE_NONE)
    {
        cbDebugPrint((MAKE_LEVEL(DP, DEBUG), "%s: invalid DP module index, LT failed! \n", FUNCTION_NAME));
        return bStatus;
    }

    pDevCommon = cbGetDeviceCommon(&pcbe->DeviceMgr, Device);
    pDpContext = container_of(pDevCommon, PCBIOS_DP_CONTEXT, Common);

    if(pDpContext->DPPortParams.bRunCTS)
    {
        ulMaxRetryCount = 1;//For DP test 4.3.1.4, if use SW to try 1.62 when 2.7 fail, the ulMaxRetryCount should set to 1.
    }

    MaxLaneCount = pLinkTrainingParams->MaxLaneCount;
    switch (pLinkTrainingParams->MaxLinkSpeed)
    {
    case CBIOS_DP_LINK_SPEED_8100Mbps:  MaxBitRateIndex = 4; break;
    case CBIOS_DP_LINK_SPEED_5400Mbps:  MaxBitRateIndex = 3; break;
    case CBIOS_DP_LINK_SPEED_2700Mbps:  MaxBitRateIndex = 2; break;
    default:  MaxBitRateIndex = 1; break;
    }
    if (pLinkTrainingParams->DpUsbMode == CBIOS_DP_TUNNEL)
    {
        if(!cbDIU_DP_LT_SetTunnelPara(pcbe, DPModuleIndex, pLinkTrainingParams))
        {
            goto EndOfLinkTraining;
        }
    }

    // set DP version according to sink
    cbDIU_DP_SetDPVersion(pcbe, DPModuleIndex, pLinkTrainingParams->DpSinkVersion);

    BitRateIndex = MaxBitRateIndex;
    ulLaneNum = MaxLaneCount;
    while(ulLaneNum > 0 && BitRateIndex > 0)
    {
        switch (BitRateIndex)
        {
        case 4:  BitRate = CBIOS_DP_LINK_SPEED_8100Mbps; break;
        case 3:  BitRate = CBIOS_DP_LINK_SPEED_5400Mbps; break;
        case 2:  BitRate = CBIOS_DP_LINK_SPEED_2700Mbps; break;
        default:  BitRate = CBIOS_DP_LINK_SPEED_1620Mbps; break;
        }

        for (ulRetryCount = 0; ulRetryCount < ulMaxRetryCount; ulRetryCount++)
        {
            //----------------- HW link training core:----------------------------------------
            // reset link training, MM8338[31] used for HW linktraining only.
            DPMuteRegValue.Value = 0;
            DPMuteRegValue.Link_Training_SW_Reset = 1;
            DPMuteRegMask.Value = 0xFFFFFFFF;
            DPMuteRegMask.Link_Training_SW_Reset = 0;
            cbMMIOWriteReg32(pcbe, DP_REG_MUTE[DPModuleIndex], DPMuteRegValue.Value, DPMuteRegMask.Value);

            cb_DelayMicroSeconds(1000); // wait HW Link Training Module Reset to be done especially on FPGA.

            DPMuteRegValue.Link_Training_SW_Reset = 0;
            cbMMIOWriteReg32(pcbe, DP_REG_MUTE[DPModuleIndex], DPMuteRegValue.Value, DPMuteRegMask.Value);

            if ((pLinkTrainingParams->DpUsbMode == CBIOS_DP_TUNNEL) && (pLinkTrainingParams->LttprNonTransparent) && (DPModuleIndex < DP_USB_MODU_NUM))
            {
                LttprConfig0.Value = 0;
                LttprConfig0Mask.Value = 0;
                LttprConfig0Mask.Reserved_1 = 0x3;
                for (i=0; i < pLinkTrainingParams->LttprCount; i++)
                {
                    LttprConfig0.LTTPR_Idx        = i;
                    LttprConfig0.Aux_RD_Interval  = 0x4; // hard code, pLinkTrainingParams->LttprAuxRDInterval[i];
                    LttprConfig0.Use_TPS3         = CBIOS_TRUE;
                    LttprConfig0.Use_TPS4         = CBIOS_FALSE;
                    LttprConfig0.Scramble_Disable = CBIOS_TRUE;
                    LttprConfig0.Max_Vol_Level    = (pLinkTrainingParams->LttprVolPELevel3[i] & 0x1) ? 3 : 2;
                    LttprConfig0.Max_PE_Level     = (pLinkTrainingParams->LttprVolPELevel3[i] & 0x2) ? 3 : 2;
                    cbMMIOWriteReg32(pcbe, DP_REG_LTTPR_CFG0[DPModuleIndex], LttprConfig0.Value, LttprConfig0Mask.Value);
                    cb_DelayMicroSeconds(1000);
                }
            }

            if ((pLinkTrainingParams->DpUsbMode == CBIOS_DP_ALTMODE) && (DPModuleIndex < DP_USB_MODU_NUM))
            {
                DPEphyMuxHpd.Value = 0;
                switch (ulLaneNum)
                {
                case 1: DPEphyMuxHpd.DP_Lane_Active = 0x1; break;
                case 2: DPEphyMuxHpd.DP_Lane_Active = 0x3; break;
                case 4: DPEphyMuxHpd.DP_Lane_Active = 0xF; break;
                default: DPEphyMuxHpd.DP_Lane_Active = 0x3; break;
                }
                DPEphyMuxHpdMask.Value = 0xFFFFFFFF;
                DPEphyMuxHpdMask.DP_Lane_Active = 0;
                cbMMIOWriteReg32(pcbe, DP_REG_EPHY_MUX_HPD[DPModuleIndex], DPEphyMuxHpd.Value, DPEphyMuxHpdMask.Value);
            }

            // Set MM8214
            DPLinkRegValue.Value = 0;
            DPLinkRegValue.Num_of_Lanes = ulLaneNum;
            DPLinkRegValue.HW_Link_Training_Done = 0;
            DPLinkRegValue.Max_V_swing = 3;
            DPLinkRegValue.Max_Pre_emphasis = 2;
            DPLinkRegValue.SW_Link_Train_Enable = 0;
            DPLinkRegValue.SW_Lane0_Swing = 0;
            DPLinkRegValue.SW_Lane1_Swing = 0;
            DPLinkRegValue.SW_Lane2_Swing = 0;
            DPLinkRegValue.SW_Lane3_Swing = 0;
            DPLinkRegMask.Value = 0xFFFFFFFF;
            DPLinkRegMask.Num_of_Lanes = 0;
            DPLinkRegMask.HW_Link_Training_Done = 0;
            DPLinkRegMask.Max_V_swing = 0;
            DPLinkRegMask.Max_Pre_emphasis = 0;
            DPLinkRegMask.SW_Link_Train_Enable = 0;
            DPLinkRegMask.SW_Lane0_Swing = 0;
            DPLinkRegMask.SW_Lane1_Swing = 0;
            DPLinkRegMask.SW_Lane2_Swing = 0;
            DPLinkRegMask.SW_Lane3_Swing = 0;

            DPLinkCtrlRegValue.Value = 0;
            DPLinkCtrlRegMask.Value = 0xFFFFFFFF;

            DPLinkCtrlRegValue.DPCD102_B5 = 1;
            DPLinkCtrlRegMask.DPCD102_B5 = 0;

            if (pLinkTrainingParams->bEnableTPS3)
            {
                // to support 5.4Gbps Link Rate, need to send Training Pattern 3
                DPLinkCtrlRegValue.EQ_USE_TP3 = 1;
                DPLinkCtrlRegMask.EQ_USE_TP3 = 0;
            }
            else
            {
                DPLinkCtrlRegValue.EQ_USE_TP3 = 0;
                DPLinkCtrlRegMask.EQ_USE_TP3 = 0;
            }

            // choose HW Link Training Speed, 2'b 00/01/10/11
            DPLinkRegValue.Start_Link_Rate_0 = (BitRateIndex-1) & 0x1;
            DPLinkRegMask.Start_Link_Rate_0 = 0;
            DPLinkCtrlRegValue.Start_LINK_RATE_1 = ((BitRateIndex-1) & 0x2) >> 1;
            DPLinkCtrlRegMask.Start_LINK_RATE_1 = 0;
            cbMMIOWriteReg32(pcbe, DP_REG_LINK[DPModuleIndex], DPLinkRegValue.Value, DPLinkRegMask.Value);
            cbMMIOWriteReg32(pcbe, DP_REG_LINK_CTRL[DPModuleIndex], DPLinkCtrlRegValue.Value, DPLinkCtrlRegMask.Value);
            if (pLinkTrainingParams->DpUsbMode == CBIOS_DP_ALTMODE)
            {
                cbPHY_DP_UDPHY_OnOff(pcbe, DPModuleIndex, CBIOS_TRUE);
            }

            cbDebugPrint((MAKE_LEVEL(DP, DEBUG), "%s: { %dMbps by %dLane(s) }: initiated ......\n", FUNCTION_NAME, (BitRate / 1000), ulLaneNum));
            // Reset Aux, PISO, enable Scramble
            cbDIU_DP_LinkTrainingPreset(pcbe, DPModuleIndex, pLinkTrainingParams, ulLaneNum);

            DPCtrlRegValue.Value = 0;
            DPCtrlRegMask.Value = 0xFFFFFFFF;
            DPCtrlRegMask.Hw_Set_Link_Para = 0;
            if (pLinkTrainingParams->DpUsbMode == CBIOS_DP_TUNNEL)
            {
                DPCtrlRegValue.Hw_Set_Link_Para = CBIOS_TRUE;
                cbMMIOWriteReg32(pcbe, DP_REG_CTRL[DPModuleIndex], DPCtrlRegValue.Value, DPCtrlRegMask.Value);
            }
            else
            {
                DPCtrlRegValue.Hw_Set_Link_Para = CBIOS_FALSE;
                cbMMIOWriteReg32(pcbe, DP_REG_CTRL[DPModuleIndex], DPCtrlRegValue.Value, DPCtrlRegMask.Value);

                // SW Set DPCD
                // According linklayer compliance test spec,
                // Before link training pattern set, must update LINK_BW_SET and LANE_COUNT_SET fields of DPCD.
                DPCD_00100.Value = 0;
                switch (BitRate)
                {
                case CBIOS_DP_LINK_SPEED_8100Mbps:  DPCD_00100.LINK_BW_SET = CBIOS_DPCD_LINK_RATE_8100Mbps; break;
                case CBIOS_DP_LINK_SPEED_5400Mbps:  DPCD_00100.LINK_BW_SET = CBIOS_DPCD_LINK_RATE_5400Mbps; break;
                case CBIOS_DP_LINK_SPEED_2700Mbps:  DPCD_00100.LINK_BW_SET = CBIOS_DPCD_LINK_RATE_2700Mbps; break;
                default:  DPCD_00100.LINK_BW_SET = CBIOS_DPCD_LINK_RATE_1620Mbps; break;
                }

                DPCD_00101.Value = 0;
                DPCD_00101.LANE_COUNT_SET = (CBIOS_U8)ulLaneNum;
                //enhanced mode bit.
                //if eDP alternate framing is enabled, DPCD 101h enhanced frame mode should not be set
                if ((pLinkTrainingParams->bEnhancedMode) && (pLinkTrainingParams->CPMethod != CBIOS_EDP_CP_AF))
                {
                    DPCD_00101.ENHANCED_FRAME_EN = 1;
                }

                Data[0] = DPCD_00100.Value;
                Data[1] = DPCD_00101.Value;

                // patch for Lenovo P1: aux maybe not ready, so write 4 more times
                i = 0;
                while (!cbDIU_Aux_Dpcd_Write(pcbe, DPModuleIndex, 0x100, Data, 2))
                {
                    i += 1;
                    if (pLinkTrainingParams->DpUsbMode != CBIOS_DP_ALTMODE || i >= 5)
                    {
                        cbDebugPrint((MAKE_LEVEL(DP, ERROR), "%s: Aux Error before LT!\n", FUNCTION_NAME));
                        goto EndOfLinkTraining;
                    }
                    cbDelayMilliSeconds(10);
                }
            }
            //write eDP CP DPCD
            if (pLinkTrainingParams->CPMethod == CBIOS_EDP_CP_ASSR)
            {
                //enable ASSR for source
                DPEphyCtrlRegValue.Value = 0;
                DPEphyCtrlRegValue.EDP_ASSR = 1;
                DPEphyCtrlRegMask.Value = 0xFFFFFFFF;
                DPEphyCtrlRegMask.EDP_ASSR = 0;
                cbMMIOWriteReg32(pcbe, DP_REG_EPHY_CTRL[DPModuleIndex], DPEphyCtrlRegValue.Value, DPEphyCtrlRegMask.Value);

                DPCD_0010A.Value = 0;
                DPCD_0010A.ALTERNATE_SCRAMBER_RESET_ENABLE = 1;
                Data[0] = DPCD_0010A.Value;
                if(!cbDIU_Aux_Dpcd_Write(pcbe, DPModuleIndex, 0x10A, Data, 1))
                {
                    cbDebugPrint((MAKE_LEVEL(DP, ERROR), "%s: Aux Error before LT!\n", FUNCTION_NAME));
                    goto EndOfLinkTraining;
                }
            }
            else if (pLinkTrainingParams->CPMethod == CBIOS_EDP_CP_AF)
            {
                DPCD_0010A.Value = 0;
                DPCD_0010A.FRAMING_CHANGE_ENABLE = 1;
                Data[0] = DPCD_0010A.Value;
                if(!cbDIU_Aux_Dpcd_Write(pcbe, DPModuleIndex, 0x10A, Data, 1))
                {
                    cbDebugPrint((MAKE_LEVEL(DP, ERROR), "%s: Aux Error before LT!\n", FUNCTION_NAME));
                    goto EndOfLinkTraining;
                }
            }
            else
            {
                //disable ASSR for source
                DPEphyCtrlRegValue.Value = 0;
                DPEphyCtrlRegValue.EDP_ASSR = 0;
                DPEphyCtrlRegMask.Value = 0xFFFFFFFF;
                DPEphyCtrlRegMask.EDP_ASSR = 0;
                cbMMIOWriteReg32(pcbe, DP_REG_EPHY_CTRL[DPModuleIndex], DPEphyCtrlRegValue.Value, DPEphyCtrlRegMask.Value);

                //disable eDP CP
                DPCD_0010A.Value = 0;
                Data[0] = DPCD_0010A.Value;
                cbDIU_Aux_Dpcd_Write(pcbe, DPModuleIndex, 0x10A, Data, 1);
                //here we don't check aux reply since for some monitors,
                //DPCD 0x10A is reserved and will not send back reply
            }

#if MEASURE_LT_TIME_GPIO
            // GPIO2 outputs 1 for measuring purpose
            cbMMIOWriteReg(pcbe,CR_B_DA,0x30,0x8F);
#endif

            // Increase TX amplitude by 60%.
            DPEphyCtrlRegValue.Value = 0;
            DPEphyCtrlRegValue.DIAJ_L0 = 2;
            DPEphyCtrlRegValue.DIAJ_L1 = 2;
            DPEphyCtrlRegValue.DIAJ_L2 = 2;
            DPEphyCtrlRegValue.DIAJ_L3 = 2;
            DPEphyCtrlRegMask.Value = 0xFF000FFF;
            cbMMIOWriteReg32(pcbe, DP_REG_EPHY_CTRL[DPModuleIndex], DPEphyCtrlRegValue.Value, DPEphyCtrlRegMask.Value);

            cbDIU_DP_LT_SetEphy(pcbe, DPModuleIndex, pLinkTrainingParams, BitRate, ChangeSWswing);

            if (pLinkTrainingParams->DpSinkVersion >= 0x12)
            {
                if (pcbe->ChipID == CHIPID_E3K || pcbe->ChipID == CHIPID_CHX004 || pcbe->ChipID == CHIPID_CNE001)
                {
                    DPSwingRegValue.Value = 0;
                    DPSwingRegValue.VER1P2 = 1;
                    DPSwingRegValue.RD_INTVAL = pLinkTrainingParams->TrainingAuxRdInterval;
                    DPSwingRegMask.Value = 0xFFFFFFFF;
                    DPSwingRegMask.VER1P2 = 0;
                    DPSwingRegMask.RD_INTVAL = 0;
                    cbMMIOWriteReg32(pcbe, DP_REG_SWING[DPModuleIndex], DPSwingRegValue.Value, DPSwingRegMask.Value);
                }
            }

            cb_AcquireLock(pDevCommon->pBusLock, CBIOS_OS_MUTEX_LOCK);

            cbDIU_DP_HWUseAuxChannel(pcbe, DPModuleIndex);
            // enable HW link training
            DPLinkRegValue.Value = 0;
            DPLinkRegValue.Start_Link_Training = 1;
            DPLinkRegMask.Value = 0xFFFFFFFF;
            DPLinkRegMask.Start_Link_Training = 0;
            cbMMIOWriteReg32(pcbe, DP_REG_LINK[DPModuleIndex], DPLinkRegValue.Value, DPLinkRegMask.Value);

            // Spec says LT should be done within 10ms.
            // Here we wait 30ms.
            for(i = 0; i <= 600; i++)
            {
                cb_DelayMicroSeconds(250);

                DPLinkRegValue.Value = cb_ReadU32(pcbe->pAdapterContext, DP_REG_LINK[DPModuleIndex]);
                if (DPLinkRegValue.HW_Link_Training_Done)
                {
                    DPEnableRegValue.Value = cb_ReadU32(pcbe->pAdapterContext, DP_REG_ENABLE[DPModuleIndex]);
                    if (DPEnableRegValue.Main_Link_Status == 3)
                    {
                        DPLinkRegValue.Value = 0;
                        DPLinkRegValue.Start_Link_Training = 0;
                        DPLinkRegValue.HW_Link_Training_Done = 0;
                        DPLinkRegMask.Value = 0xFFFFFFFF;
                        DPLinkRegMask.Start_Link_Training = 0;
                        DPLinkRegMask.HW_Link_Training_Done = 0;
                        cbMMIOWriteReg32(pcbe, DP_REG_LINK[DPModuleIndex], DPLinkRegValue.Value, DPLinkRegMask.Value);
                        bStatus = CBIOS_TRUE;
                        break;
                    }
                }
                if(i == 600)
                {
                    //clear last link training fail status
                    DPLinkRegValue.Value = 0;
                    DPLinkRegValue.Start_Link_Training = 0;
                    DPLinkRegValue.HW_Link_Training_Done = 0;
                    DPLinkRegMask.Value = 0xFFFFFFFF;
                    DPLinkRegMask.Start_Link_Training = 0;
                    DPLinkRegMask.HW_Link_Training_Done = 0;
                    cbMMIOWriteReg32(pcbe, DP_REG_LINK[DPModuleIndex], DPLinkRegValue.Value, DPLinkRegMask.Value);
                }
            }

            cb_ReleaseLock(pDevCommon->pBusLock, CBIOS_OS_MUTEX_LOCK, 0);

            if (bStatus)
            {
                cbDebugPrint((MAKE_LEVEL(DP, DEBUG), "%s: { %dMbps by %dLane(s) }: Succeed!\n", FUNCTION_NAME, (BitRate / 1000), ulLaneNum));
                goto LinkTrainingDone;
            }
            else
            {
                cbDebugPrint((MAKE_LEVEL(DP, WARNING), "%s: { %dMbps by %dLane(s) }: Fail!\n", FUNCTION_NAME, (BitRate / 1000), ulLaneNum));

                //clear link training pattern
                DPCD_00102.Value = 0;
                DPCD_00102.TRAINING_PATTERN_SELECT = 0;
                DPCD_00102.SCRAMBLING_DISABLE = 0;
                Data[0] = DPCD_00102.Value;
                if(!cbDIU_Aux_Dpcd_Write(pcbe, DPModuleIndex, 0x102, Data, 1))
                {
                    cbDebugPrint((MAKE_LEVEL(DP, ERROR), "%s: Aux Error before LT!\n", FUNCTION_NAME));
                    goto EndOfLinkTraining;
                }

            }

        }//end of retry loop

        if(cbDIU_Aux_Dpcd_Read(pcbe, DPModuleIndex, 0x202, Data, 2))
        {
            if(BitRateIndex == MaxBitRateIndex && ulLaneNum == MaxLaneCount)
            {
                if((MaxLaneCount == 4 && ((Data[1]<<8)|Data[0]) == 0x1177) ||
                    (MaxLaneCount == 2 && Data[0] == 0x17))
                {
                    bEQFallBack = 1;
                }
                if((MaxLaneCount == 4 && ((Data[1]<<8)|Data[0]) == 0) ||
                    (MaxLaneCount == 2 && Data[0] == 0) ||
                    (MaxLaneCount == 1 && (Data[0]&0xF) == 0))//CR_DONE,CHANNEL_EQ_DONE,SYMBOL_LOCKED are all 0
                {
                    if(cbDIU_Aux_Dpcd_Read(pcbe, DPModuleIndex, 0x206, Data, 2))
                    {
                        if((MaxLaneCount == 4 && ((Data[1]<<8)|Data[0]) == 0) ||
                            (MaxLaneCount == 2 && Data[0] == 0) ||
                            (MaxLaneCount == 1 && (Data[0]&0xF) == 0))//VOLTAGE_SWING_LANE,PRE-EMPHASIS_LANE are all 0
                        {
                            //For LT of MaxLaneCount and MaxBitRateIndex, if XXX_done all 0,swing and pre_emphasis level all 0.
                            //In this case, we can set sw_swing to 1 for load_index 1, then HW LT will fail at level 0 and retry LT at level 1.
                            //it can enhance stability of LT
                            //When testing DP CTS, ChangeSWswing should not be set to true, otherwise some test items such as 400.3.1.4 will fail.
                            if(!ChangeSWswing)
                            {
                                ChangeSWswing = CBIOS_TRUE;
                                UseReducedLTPara = CBIOS_FALSE;
                                cbDebugPrint((MAKE_LEVEL(DP, WARNING),"set sw_swing to 1 for load_index 1"));
                            }
                        }
                    }
                }
            }
        }

        if(UseReducedLTPara)
        {
            ChangeSWswing = CBIOS_FALSE;
            if(bEQFallBack)
            {
                ulLaneNum = ulLaneNum >> 1;
                if(ulLaneNum == 0 && BitRateIndex > 0)
                {
                    ulLaneNum = MaxLaneCount;
                    BitRateIndex --;
                    if(BitRateIndex == 0)
                    {
                        ulLaneNum = 0;
                    }
                }
            }
            else
            {
                BitRateIndex --;
                if(BitRateIndex == 0 && ulLaneNum > 0)
                {
                    BitRateIndex = MaxBitRateIndex;
                    ulLaneNum = ulLaneNum >> 1;
                    if(ulLaneNum == 0)
                    {
                        BitRateIndex = 0;
                    }
                }
            }
        }
        UseReducedLTPara = CBIOS_TRUE;
    }//end of lane count loop

LinkTrainingDone:
    //link training succeeded, update current link speed and lane count
    if (bStatus)
    {
        pLinkTrainingParams->CurrLinkSpeed = BitRate;
        pLinkTrainingParams->CurrLaneCount = ulLaneNum;
    }

    cbDebugPrint((MAKE_LEVEL(DP, DEBUG), "%s: Polling Loop Count ==: %d, equivalent time (i*50us) == %dus!\n", FUNCTION_NAME, i, (i * 50)));

#if MEASURE_LT_TIME_GPIO
    // GPIO2 outputs 0 for measuring purpose
    cbMMIOWriteReg(pcbe,CR_B_DA,0x20,0x8F);
#endif
EndOfLinkTraining:
    if (!bStatus)
    {
        DPGenCtrlRegValue.Value = cb_ReadU32(pcbe->pAdapterContext, DP_REG_GEN_CTRL[DPModuleIndex]);
        DPAuxTimerRegValue.Value = cb_ReadU32(pcbe->pAdapterContext, DP_REG_AUX_TIMER[DPModuleIndex]);
        cbDebugPrint((MAKE_LEVEL(DP, ERROR), "%s: LT fail: mm8218==0x%08x, mm8330==0x%08x\n", FUNCTION_NAME, DPGenCtrlRegValue.Value, DPAuxTimerRegValue.Value));

        if (DPGenCtrlRegValue.HW_Link_Train_Fail)
        {
            cbDebugPrint((MAKE_LEVEL(DP, ERROR), "%s: LT failed due to Aux error!\n", FUNCTION_NAME));

            // Clear TRAINING_PATTERN_SET byte
            cbDebugPrint((MAKE_LEVEL(DP, DEBUG), "%s: write DPCD_102 0x00 to abort LT and recover Aux\n", FUNCTION_NAME));

            DPCD_00102.Value = 0;
            DPCD_00102.TRAINING_PATTERN_SELECT = 0;
            DPCD_00102.SCRAMBLING_DISABLE = 0;
            Data[0] = DPCD_00102.Value;
            if(!cbDIU_Aux_Dpcd_Write(pcbe, DPModuleIndex, 0x102, Data, 1))
            {
                cbDebugPrint((MAKE_LEVEL(DP, ERROR), "%s: Write DPCD error to abort LT!\n", FUNCTION_NAME));
                bStatus = CBIOS_FALSE;
            }
        }
    }

    cbTraceExit(DP);

    return bStatus;
}

CBIOS_BOOL cbDIU_DP_LT_With_DPCD115(PCBIOS_VOID pvcbe, CBIOS_MODULE_INDEX DPModuleIndex, PCBIOS_LINK_TRAINING_PARAMS pLinkTrainingParams)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    CBIOS_U32               Device = CBIOS_TYPE_NONE;
    PCBIOS_DEVICE_COMMON    pDevCommon = CBIOS_NULL;
    PCBIOS_DP_CONTEXT       pDpContext = CBIOS_NULL;
    CBIOS_BOOL  bStatus = CBIOS_FALSE;
    CBIOS_U32   i = 0;
    CBIOS_U32   bEQFallBack = 0;
    CBIOS_U32   BitRate = 0;
    CBIOS_U32   ulLaneNum, MaxLaneCount = 0;
    CBIOS_U32   BitRateIndex = 0, MaxBitRateIndex = 0, LinkRateEntry = 0;
    CBIOS_U8    Data[2] = {0};
    REG_MM8214  DPLinkRegValue, DPLinkRegMask;
    REG_MM8218  DPGenCtrlRegValue;
    REG_MM8240  DPEnableRegValue;
    REG_MM82CC  DPEphyCtrlRegValue, DPEphyCtrlRegMask;
    REG_MM8330  DPAuxTimerRegValue;
    REG_MM8338  DPMuteRegValue, DPMuteRegMask;
    REG_MM8368  DPSwingRegValue, DPSwingRegMask;
    REG_MM334C8 DPLinkCtrlRegValue, DPLinkCtrlRegMask;
    REG_MM334DC DPCtrlRegValue, DPCtrlRegMask;
    DPCD_REG_00100 DPCD_00100;
    DPCD_REG_00101 DPCD_00101;
    DPCD_REG_0010A DPCD_0010A;
    DPCD_REG_00102 DPCD_00102;
    CBIOS_BOOL  ChangeSWswing = CBIOS_FALSE;
    CBIOS_BOOL  UseReducedLTPara = CBIOS_TRUE;
    DPCD_REG_00115 DPCD_00115 = {0};
    //For DP test 4.3.1.4, if use SW to try 1.62 when 2.7 fail, the ulMaxRetryCount should set to 1.
    CBIOS_U32   ulRetryCount = 0, ulMaxRetryCount = 3;
    if(DPModuleIndex >= DP_MODU_NUM)
    {
        cbDebugPrint((MAKE_LEVEL(DP, ERROR), "Invalid module index in %s\n", FUNCTION_NAME));
        return CBIOS_FALSE;
    }
    cbTraceEnter(DP);
    Device = cbGetDeviceType(pcbe, DPModuleIndex, CBIOS_MODULE_TYPE_DP);
    if(Device == CBIOS_TYPE_NONE)
    {
        cbDebugPrint((MAKE_LEVEL(DP, DEBUG), "%s: invalid DP module index, LT failed! \n", FUNCTION_NAME));
        return bStatus;
    }
    pDevCommon = cbGetDeviceCommon(&pcbe->DeviceMgr, Device);
    pDpContext = container_of(pDevCommon, PCBIOS_DP_CONTEXT, Common);
    if(pDpContext->DPPortParams.bRunCTS)
    {
        ulMaxRetryCount = 1;//For DP test 4.3.1.4, if use SW to try 1.62 when 2.7 fail, the ulMaxRetryCount should set to 1.
    }
    MaxLaneCount = pLinkTrainingParams->MaxLaneCount;
    MaxBitRateIndex = pDpContext->DPMonitorContext.LinkRateHwIndex;
    LinkRateEntry = pDpContext->DPMonitorContext.SupportedLinkRateNum - 1;
    // set DP version according to sink
    cbDIU_DP_SetDPVersion(pcbe, DPModuleIndex, pLinkTrainingParams->DpSinkVersion);
    BitRateIndex = MaxBitRateIndex;
    ulLaneNum = MaxLaneCount;
    while(ulLaneNum > 0 && BitRateIndex > 0)
    {
        BitRate = pDpContext->DPMonitorContext.SupportedLinkRates[LinkRateEntry];
        for (ulRetryCount = 0; ulRetryCount < ulMaxRetryCount; ulRetryCount++)
        {
            // reset link training, MM8338[31] used for HW linktraining only.
            DPMuteRegValue.Value = 0;
            DPMuteRegValue.Link_Training_SW_Reset = 1;
            DPMuteRegMask.Value = 0xFFFFFFFF;
            DPMuteRegMask.Link_Training_SW_Reset = 0;
            cbMMIOWriteReg32(pcbe, DP_REG_MUTE[DPModuleIndex], DPMuteRegValue.Value, DPMuteRegMask.Value);
            cb_DelayMicroSeconds(1000); // wait HW Link Training Module Reset to be done especially on FPGA.
            DPMuteRegValue.Link_Training_SW_Reset = 0;
            cbMMIOWriteReg32(pcbe, DP_REG_MUTE[DPModuleIndex], DPMuteRegValue.Value, DPMuteRegMask.Value);
            // Set MM8214
            DPLinkRegValue.Value = 0;
            DPLinkRegValue.Num_of_Lanes = ulLaneNum;
            DPLinkRegValue.HW_Link_Training_Done = 0;
            DPLinkRegValue.Max_V_swing = 3;
            DPLinkRegValue.Max_Pre_emphasis = 2;
            DPLinkRegValue.SW_Link_Train_Enable = 0;
            DPLinkRegValue.SW_Lane0_Swing = 0;
            DPLinkRegValue.SW_Lane1_Swing = 0;
            DPLinkRegValue.SW_Lane2_Swing = 0;
            DPLinkRegValue.SW_Lane3_Swing = 0;
            DPLinkRegMask.Value = 0xFFFFFFFF;
            DPLinkRegMask.Num_of_Lanes = 0;
            DPLinkRegMask.HW_Link_Training_Done = 0;
            DPLinkRegMask.Max_V_swing = 0;
            DPLinkRegMask.Max_Pre_emphasis = 0;
            DPLinkRegMask.SW_Link_Train_Enable = 0;
            DPLinkRegMask.SW_Lane0_Swing = 0;
            DPLinkRegMask.SW_Lane1_Swing = 0;
            DPLinkRegMask.SW_Lane2_Swing = 0;
            DPLinkRegMask.SW_Lane3_Swing = 0;
            DPLinkCtrlRegValue.Value = 0;
            DPLinkCtrlRegMask.Value = 0xFFFFFFFF;
            DPLinkCtrlRegValue.DPCD102_B5 = 1;
            DPLinkCtrlRegMask.DPCD102_B5 = 0;
            if (pLinkTrainingParams->bEnableTPS3)
            {
                // to support 5.4Gbps Link Rate, need to send Training Pattern 3
                DPLinkCtrlRegValue.EQ_USE_TP3 = 1;
                DPLinkCtrlRegMask.EQ_USE_TP3 = 0;
            }
            else
            {
                DPLinkCtrlRegValue.EQ_USE_TP3 = 0;
                DPLinkCtrlRegMask.EQ_USE_TP3 = 0;
            }
            // choose HW Link Training Speed, 2'b 00/01/10/11
            DPLinkRegValue.Start_Link_Rate_0 = (BitRateIndex-1) & 0x1;
            DPLinkRegMask.Start_Link_Rate_0 = 0;
            DPLinkCtrlRegValue.Start_LINK_RATE_1 = ((BitRateIndex-1) & 0x2) >> 1;
            DPLinkCtrlRegMask.Start_LINK_RATE_1 = 0;
            cbMMIOWriteReg32(pcbe, DP_REG_LINK[DPModuleIndex], DPLinkRegValue.Value, DPLinkRegMask.Value);
            cbMMIOWriteReg32(pcbe, DP_REG_LINK_CTRL[DPModuleIndex], DPLinkCtrlRegValue.Value, DPLinkCtrlRegMask.Value);
            cbDebugPrint((MAKE_LEVEL(DP, DEBUG), "%s: { %dMbps by %dLane(s) }: initiated ......\n", FUNCTION_NAME, (BitRate / 1000), ulLaneNum));
            // Reset Aux, PISO, enable Scramble
            cbDIU_DP_LinkTrainingPreset(pcbe, DPModuleIndex, pLinkTrainingParams, ulLaneNum);
            DPCtrlRegValue.Value = 0;
            DPCtrlRegMask.Value = 0xFFFFFFFF;
            DPCtrlRegMask.Hw_Set_Link_Para = 0;
            DPCtrlRegValue.Hw_Set_Link_Para = 0;
            DPCtrlRegMask.Link_Rate_use_DPCP115 = 0;
            DPCtrlRegValue.Link_Rate_use_DPCP115 = 1;
            switch (BitRateIndex)
            {
            case 3:
                DPCtrlRegValue.Link_Rate_value_3 = LinkRateEntry;
                DPCtrlRegMask.Link_Rate_value_3 = 0;
                break;
            case 2:
                DPCtrlRegValue.Link_Rate_value_2 = LinkRateEntry;
                DPCtrlRegMask.Link_Rate_value_2 = 0;
                break;
            case 1:
                DPCtrlRegValue.Link_Rate_value_1 = LinkRateEntry;
                DPCtrlRegMask.Link_Rate_value_1 = 0;
                break;
            default:
                break;
            }
            cbMMIOWriteReg32(pcbe, DP_REG_CTRL[DPModuleIndex], DPCtrlRegValue.Value, DPCtrlRegMask.Value);
             // SW Set DPCD
             // According linklayer compliance test spec,
             // Before link training pattern set, must update LINK_BW_SET and LANE_COUNT_SET fields of DPCD.
            DPCD_00100.Value = 0;
            DPCD_00100.LINK_BW_SET = 0;//use DPCD 115 and can't write DPCD_00100.LINK_BW_SET
            DPCD_00101.Value = 0;
            DPCD_00101.LANE_COUNT_SET = (CBIOS_U8)ulLaneNum;
            //enhanced mode bit.
            //if eDP alternate framing is enabled, DPCD 101h enhanced frame mode should not be set
            if ((pLinkTrainingParams->bEnhancedMode) && (pLinkTrainingParams->CPMethod != CBIOS_EDP_CP_AF))
            {
                DPCD_00101.ENHANCED_FRAME_EN = 1;
            }
            Data[0] = DPCD_00100.Value;
            Data[1] = DPCD_00101.Value;
            if(!cbDIU_Aux_Dpcd_Write(pcbe, DPModuleIndex, 0x100, Data, 2))
            {
                cbDebugPrint((MAKE_LEVEL(DP, ERROR), "%s: Aux Error before LT!\n", FUNCTION_NAME));
                goto EndOfLinkTraining;
            }
            DPCD_00115.Value = 0;
            DPCD_00115.Link_Rate_Set = (CBIOS_U8)LinkRateEntry;
            Data[0] = DPCD_00115.Value;
            if(!cbDIU_Aux_Dpcd_Write(pcbe, DPModuleIndex, 0x115, Data, 1))
            {
                cbDebugPrint((MAKE_LEVEL(DP, ERROR),"%s: Aux Write DPCD 115 Fail \n", FUNCTION_NAME));
            }
            //write eDP CP DPCD
            if (pLinkTrainingParams->CPMethod == CBIOS_EDP_CP_ASSR)
            {
                //enable ASSR for source
                DPEphyCtrlRegValue.Value = 0;
                DPEphyCtrlRegValue.EDP_ASSR = 1;
                DPEphyCtrlRegMask.Value = 0xFFFFFFFF;
                DPEphyCtrlRegMask.EDP_ASSR = 0;
                cbMMIOWriteReg32(pcbe, DP_REG_EPHY_CTRL[DPModuleIndex], DPEphyCtrlRegValue.Value, DPEphyCtrlRegMask.Value);
                DPCD_0010A.Value = 0;
                DPCD_0010A.ALTERNATE_SCRAMBER_RESET_ENABLE = 1;
                Data[0] = DPCD_0010A.Value;
                if(!cbDIU_Aux_Dpcd_Write(pcbe, DPModuleIndex, 0x10A, Data, 1))
                {
                    cbDebugPrint((MAKE_LEVEL(DP, ERROR), "%s: Aux Error before LT!\n", FUNCTION_NAME));
                    goto EndOfLinkTraining;
                }
            }
            else if (pLinkTrainingParams->CPMethod == CBIOS_EDP_CP_AF)
            {
                DPCD_0010A.Value = 0;
                DPCD_0010A.FRAMING_CHANGE_ENABLE = 1;
                Data[0] = DPCD_0010A.Value;
                if(!cbDIU_Aux_Dpcd_Write(pcbe, DPModuleIndex, 0x10A, Data, 1))
                {
                    cbDebugPrint((MAKE_LEVEL(DP, ERROR), "%s: Aux Error before LT!\n", FUNCTION_NAME));
                    goto EndOfLinkTraining;
                }
            }
            else
            {
                //disable ASSR for source
                DPEphyCtrlRegValue.Value = 0;
                DPEphyCtrlRegValue.EDP_ASSR = 0;
                DPEphyCtrlRegMask.Value = 0xFFFFFFFF;
                DPEphyCtrlRegMask.EDP_ASSR = 0;
                cbMMIOWriteReg32(pcbe, DP_REG_EPHY_CTRL[DPModuleIndex], DPEphyCtrlRegValue.Value, DPEphyCtrlRegMask.Value);
                //disable eDP CP
                DPCD_0010A.Value = 0;
                Data[0] = DPCD_0010A.Value;
                cbDIU_Aux_Dpcd_Write(pcbe, DPModuleIndex, 0x10A, Data, 1);
                //here we don't check aux reply since for some monitors,
                //DPCD 0x10A is reserved and will not send back reply
            }
#if MEASURE_LT_TIME_GPIO
            // GPIO2 outputs 1 for measuring purpose
            cbMMIOWriteReg(pcbe,CR_B_DA,0x30,0x8F);
#endif
            // Increase TX amplitude by 60%.
            DPEphyCtrlRegValue.Value = 0;
            DPEphyCtrlRegValue.DIAJ_L0 = 2;
            DPEphyCtrlRegValue.DIAJ_L1 = 2;
            DPEphyCtrlRegValue.DIAJ_L2 = 2;
            DPEphyCtrlRegValue.DIAJ_L3 = 2;
            DPEphyCtrlRegMask.Value = 0xFF000FFF;
            cbMMIOWriteReg32(pcbe, DP_REG_EPHY_CTRL[DPModuleIndex], DPEphyCtrlRegValue.Value, DPEphyCtrlRegMask.Value);
            cbDIU_DP_LT_SetEphy(pcbe, DPModuleIndex, pLinkTrainingParams, BitRate, ChangeSWswing);
            if (pLinkTrainingParams->DpSinkVersion >= 0x12)
            {
                if (pcbe->ChipID == CHIPID_E3K || pcbe->ChipID == CHIPID_CHX004 || pcbe->ChipID == CHIPID_CNE001)
                {
                    DPSwingRegValue.Value = 0;
                    DPSwingRegValue.VER1P2 = 1;
                    DPSwingRegValue.RD_INTVAL = pLinkTrainingParams->TrainingAuxRdInterval;
                    DPSwingRegMask.Value = 0xFFFFFFFF;
                    DPSwingRegMask.VER1P2 = 0;
                    DPSwingRegMask.RD_INTVAL = 0;
                    cbMMIOWriteReg32(pcbe, DP_REG_SWING[DPModuleIndex], DPSwingRegValue.Value, DPSwingRegMask.Value);
                }
            }
            cb_AcquireLock(pDevCommon->pBusLock, CBIOS_OS_MUTEX_LOCK);
            cbDIU_DP_HWUseAuxChannel(pcbe, DPModuleIndex);
            // enable HW link training
            DPLinkRegValue.Value = 0;
            DPLinkRegValue.Start_Link_Training = 1;
            DPLinkRegMask.Value = 0xFFFFFFFF;
            DPLinkRegMask.Start_Link_Training = 0;
            cbMMIOWriteReg32(pcbe, DP_REG_LINK[DPModuleIndex], DPLinkRegValue.Value, DPLinkRegMask.Value);
            // Spec says LT should be done within 10ms.
            // Here we wait 30ms.
            for(i = 0; i <= 600; i++)
            {
                cb_DelayMicroSeconds(250);
                DPLinkRegValue.Value = cb_ReadU32(pcbe->pAdapterContext, DP_REG_LINK[DPModuleIndex]);
                if (DPLinkRegValue.HW_Link_Training_Done)
                {
                    DPEnableRegValue.Value = cb_ReadU32(pcbe->pAdapterContext, DP_REG_ENABLE[DPModuleIndex]);
                    if (DPEnableRegValue.Main_Link_Status == 3)
                    {
                        DPLinkRegValue.Value = 0;
                        DPLinkRegValue.Start_Link_Training = 0;
                        DPLinkRegValue.HW_Link_Training_Done = 0;
                        DPLinkRegMask.Value = 0xFFFFFFFF;
                        DPLinkRegMask.Start_Link_Training = 0;
                        DPLinkRegMask.HW_Link_Training_Done = 0;
                        cbMMIOWriteReg32(pcbe, DP_REG_LINK[DPModuleIndex], DPLinkRegValue.Value, DPLinkRegMask.Value);
                        bStatus = CBIOS_TRUE;
                        break;
                    }
                }
                if(i == 600)
                {
                    //clear last link training fail status
                    DPLinkRegValue.Value = 0;
                    DPLinkRegValue.Start_Link_Training = 0;
                    DPLinkRegValue.HW_Link_Training_Done = 0;
                    DPLinkRegMask.Value = 0xFFFFFFFF;
                    DPLinkRegMask.Start_Link_Training = 0;
                    DPLinkRegMask.HW_Link_Training_Done = 0;
                    cbMMIOWriteReg32(pcbe, DP_REG_LINK[DPModuleIndex], DPLinkRegValue.Value, DPLinkRegMask.Value);
                }
            }
            cb_ReleaseLock(pDevCommon->pBusLock, CBIOS_OS_MUTEX_LOCK, 0);
            if (bStatus)
            {
                cbDebugPrint((MAKE_LEVEL(DP, DEBUG), "%s: { %dMbps by %dLane(s) }: Succeed!\n", FUNCTION_NAME, (BitRate / 1000), ulLaneNum));
                goto LinkTrainingDone;
            }
            else
            {
                cbDebugPrint((MAKE_LEVEL(DP, WARNING), "%s: { %dMbps by %dLane(s) }: Fail!\n", FUNCTION_NAME, (BitRate / 1000), ulLaneNum));
                //clear link training pattern
                DPCD_00102.Value = 0;
                DPCD_00102.TRAINING_PATTERN_SELECT = 0;
                DPCD_00102.SCRAMBLING_DISABLE = 0;
                Data[0] = DPCD_00102.Value;
                if(!cbDIU_Aux_Dpcd_Write(pcbe, DPModuleIndex, 0x102, Data, 1))
                {
                    cbDebugPrint((MAKE_LEVEL(DP, ERROR), "%s: Aux Error before LT!\n", FUNCTION_NAME));
                    goto EndOfLinkTraining;
                }
            }
        }//end of retry loop
        if(cbDIU_Aux_Dpcd_Read(pcbe, DPModuleIndex, 0x202, Data, 2))
        {
            if(BitRateIndex == MaxBitRateIndex && ulLaneNum == MaxLaneCount)
            {
                if((MaxLaneCount == 4 && ((Data[1]<<8)|Data[0]) == 0x1177) ||
                    (MaxLaneCount == 2 && Data[0] == 0x17))
                {
                     bEQFallBack = 1;
                }
                if((MaxLaneCount == 4 && ((Data[1]<<8)|Data[0]) == 0) ||
                    (MaxLaneCount == 2 && Data[0] == 0) ||
                    (MaxLaneCount == 1 && (Data[0]&0xF) == 0))//CR_DONE,CHANNEL_EQ_DONE,SYMBOL_LOCKED are all 0
                {
                    if(cbDIU_Aux_Dpcd_Read(pcbe, DPModuleIndex, 0x206, Data, 2))
                    {
                        if((MaxLaneCount == 4 && ((Data[1]<<8)|Data[0]) == 0) ||
                            (MaxLaneCount == 2 && Data[0] == 0) ||
                            (MaxLaneCount == 1 && (Data[0]&0xF) == 0))//VOLTAGE_SWING_LANE,PRE-EMPHASIS_LANE are all 0
                        {
                            //For LT of MaxLaneCount and MaxBitRateIndex, if XXX_done all 0,swing and pre_emphasis level all 0.
                            //In this case, we can set sw_swing to 1 for load_index 1, then HW LT will fail at level 0 and retry LT at level 1.
                            //it can enhance stability of LT
                            //When testing DP CTS, ChangeSWswing should not be set to true, otherwise some test items such as 400.3.1.4 will fail.
                            if(!ChangeSWswing)
                            {
                                ChangeSWswing = CBIOS_TRUE;
                                UseReducedLTPara = CBIOS_FALSE;
                                cbDebugPrint((MAKE_LEVEL(DP, WARNING),"set sw_swing to 1 for load_index 1"));
                            }
                        }
                    }
                }
            }
        }
        if(UseReducedLTPara)
        {
            ChangeSWswing = CBIOS_FALSE;
            if(bEQFallBack)
            {
                ulLaneNum = ulLaneNum >> 1;
                if(ulLaneNum == 0 && BitRateIndex > 0)
                {
                    ulLaneNum = MaxLaneCount;
                    BitRateIndex --;
                    LinkRateEntry--;
                    if(BitRateIndex == 0)
                    {
                        ulLaneNum = 0;
                        LinkRateEntry = 0;
                    }
                }
            }
            else
            {
                BitRateIndex --;
                LinkRateEntry--;
                if(BitRateIndex == 0 && ulLaneNum > 0)
                {
                    BitRateIndex = MaxBitRateIndex;
                    LinkRateEntry = pDpContext->DPMonitorContext.SupportedLinkRateNum - 1;
                    ulLaneNum = ulLaneNum >> 1;
                    if(ulLaneNum == 0)
                    {
                        BitRateIndex = 0;
                        LinkRateEntry= 0;
                    }
                }
            }
        }
        UseReducedLTPara = CBIOS_TRUE;
    }//end of lane count loop
LinkTrainingDone:
    //link training succeeded, update current link speed and lane count
    if (bStatus)
    {
        pLinkTrainingParams->CurrLinkSpeed = BitRate;
        pLinkTrainingParams->CurrLaneCount = ulLaneNum;
    }
    cbDebugPrint((MAKE_LEVEL(DP, DEBUG), "%s: Polling Loop Count ==: %d, equivalent time (i*50us) == %dus!\n", FUNCTION_NAME, i, (i * 50)));
#if MEASURE_LT_TIME_GPIO
    // GPIO2 outputs 0 for measuring purpose
    cbMMIOWriteReg(pcbe,CR_B_DA,0x20,0x8F);
#endif
EndOfLinkTraining:
    if (!bStatus)
    {
        DPGenCtrlRegValue.Value = cb_ReadU32(pcbe->pAdapterContext, DP_REG_GEN_CTRL[DPModuleIndex]);
        DPAuxTimerRegValue.Value = cb_ReadU32(pcbe->pAdapterContext, DP_REG_AUX_TIMER[DPModuleIndex]);
        cbDebugPrint((MAKE_LEVEL(DP, ERROR), "%s: LT fail: mm8218==0x%08x, mm8330==0x%08x\n", FUNCTION_NAME, DPGenCtrlRegValue.Value, DPAuxTimerRegValue.Value));
        if (DPGenCtrlRegValue.HW_Link_Train_Fail)
        {
            cbDebugPrint((MAKE_LEVEL(DP, ERROR), "%s: LT failed due to Aux error!\n", FUNCTION_NAME));
            // Clear TRAINING_PATTERN_SET byte
            cbDebugPrint((MAKE_LEVEL(DP, DEBUG), "%s: write DPCD_102 0x00 to abort LT and recover Aux\n", FUNCTION_NAME));
            DPCD_00102.Value = 0;
            DPCD_00102.TRAINING_PATTERN_SELECT = 0;
            DPCD_00102.SCRAMBLING_DISABLE = 0;
            Data[0] = DPCD_00102.Value;
            if(!cbDIU_Aux_Dpcd_Write(pcbe, DPModuleIndex, 0x102, Data, 1))
            {
                cbDebugPrint((MAKE_LEVEL(DP, ERROR), "%s: Write DPCD error to abort LT!\n", FUNCTION_NAME));
                bStatus = CBIOS_FALSE;
            }
        }
    }
    cbTraceExit(DP);
    return bStatus;
}

CBIOS_BOOL cbDIU_DP_SetUpMainLink(PCBIOS_VOID pvcbe, CBIOS_MODULE_INDEX DPModuleIndex, PCBIOS_MAIN_LINK_PARAMS pMainLinkParams)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    CBIOS_BOOL              bStatus = CBIOS_FALSE;
    PCBIOS_TIMING_ATTRIB    pTiming = pMainLinkParams->pTiming;
    CBIOS_U64               ulTURatio, ulTemp1, ulTemp2, ulTemp3;
    REG_MM8210              DPMisc1RegValue, DPMisc1RegMask;
    REG_MM8218              DPGenCtrlRegValue, DPGenCtrlRegMask;
    REG_MM821C              DPExtPacketRegValue, DPExtPacketRegMask;
    REG_MM8240              DPEnableRegValue, DPEnableRegMask;
    REG_MM8244              DPHWidthTURegValue, DPHWidthTURegMask;
    REG_MM8248              DPHLineDurRegValue, DPHLineDurRegMask;
    REG_MM824C              DPMisc0RegValue, DPMisc0RegMask;
    REG_MM8250              DPHVTotalRegValue, DPHVTotalRegMask;
    REG_MM8254              DPHVStartRegValue, DPHVStartRegMask;
    REG_MM8258              DPHVSyncRegValue, DPHVSyncRegMask;
    REG_MM825C              DPHVActiveRegValue, DPHVActiveRegMask;
    REG_MM334DC             DPCtrlRegValue;
    REG_SR3A_Pair           RegSR3AValue;
    REG_SR3A_Pair           RegSR3AMask;
    CBIOS_U8                Data[2] = {0};
    CBIOS_U32               TempBitRate = 0;
    CBIOS_U32               Device = CBIOS_TYPE_NONE;
    PCBIOS_DEVICE_COMMON    pDevCommon = CBIOS_NULL;
    PCBIOS_DP_CONTEXT       pDpContext = CBIOS_NULL;
    CBIOS_BOOL              bUseLinkRateSet = 0;

    cbTraceEnter(DP);

    if (DPModuleIndex >= DP_MODU_NUM)
    {
        cbDebugPrint((MAKE_LEVEL(DP, ERROR), "%s: invalid DP module index!\n", FUNCTION_NAME));
        return bStatus;
    }
    Device = cbGetDeviceType(pcbe, DPModuleIndex, CBIOS_MODULE_TYPE_DP);

    if(Device == CBIOS_TYPE_NONE)
    {
        cbDebugPrint((MAKE_LEVEL(DP, ERROR), "%s: can't get device from DPModuleIndex %d\n", FUNCTION_NAME,DPModuleIndex));
        return bStatus;
    }
    pDevCommon = cbGetDeviceCommon(&pcbe->DeviceMgr, Device);
    pDpContext = container_of(pDevCommon, PCBIOS_DP_CONTEXT, Common);
    bUseLinkRateSet = pDpContext->DPMonitorContext.bUseLinkRateSet;

    DPEnableRegValue.Value = 0;
    DPEnableRegValue.Field_Invert = 0;
    DPEnableRegMask.Value = 0xFFFFFFFF;
    DPEnableRegMask.Field_Invert = 0;
    cbMMIOWriteReg32(pcbe, DP_REG_ENABLE[DPModuleIndex], DPEnableRegValue.Value, DPEnableRegMask.Value);

    if ((pcbe->ChipID == CHIPID_E3K || pcbe->ChipID == CHIPID_CHX004 || pcbe->ChipID == CHIPID_CNE001) && !bUseLinkRateSet)
    {
        #if 1
        DPCtrlRegValue.Value = cb_ReadU32(pcbe->pAdapterContext, DP_REG_CTRL[DPModuleIndex]);
        switch (DPCtrlRegValue.LINK_bit_rate_status_1_0)
        {
        case 3:  TempBitRate = CBIOS_DP_LINK_SPEED_8100Mbps; break;
        case 2:  TempBitRate = CBIOS_DP_LINK_SPEED_5400Mbps; break;
        case 1:  TempBitRate = CBIOS_DP_LINK_SPEED_2700Mbps; break;
        case 0:  TempBitRate = CBIOS_DP_LINK_SPEED_1620Mbps; break;
        default:  TempBitRate = CBIOS_DP_LINK_SPEED_1620Mbps; break;
        }
        if (pMainLinkParams->LinkedSpeed != TempBitRate)
        {
            cbDebugPrint((MAKE_LEVEL(DP, WARNING), "%s: why current link speed:%d is not sync with REG:0x%x?\n",
                            FUNCTION_NAME, pMainLinkParams->LinkedSpeed, DP_REG_CTRL[DPModuleIndex]));
            pMainLinkParams->LinkedSpeed = TempBitRate;
        }
        #endif

        if(cbDIU_Aux_Dpcd_Read(pcbe, DPModuleIndex, 0x100, Data, 1))
        {
            DPCD_REG_00100 DPCD_0100;
            DPCD_0100.Value = Data[0];

            switch (DPCD_0100.LINK_BW_SET)
            {
            case CBIOS_DPCD_LINK_RATE_8100Mbps:  TempBitRate = CBIOS_DP_LINK_SPEED_8100Mbps; break;
            case CBIOS_DPCD_LINK_RATE_5400Mbps:  TempBitRate = CBIOS_DP_LINK_SPEED_5400Mbps; break;
            case CBIOS_DPCD_LINK_RATE_2700Mbps:  TempBitRate = CBIOS_DP_LINK_SPEED_2700Mbps; break;
            case CBIOS_DPCD_LINK_RATE_1620Mbps:  TempBitRate = CBIOS_DP_LINK_SPEED_1620Mbps; break;
            default:  TempBitRate = CBIOS_DP_LINK_SPEED_1620Mbps; break;
            }
            if (pMainLinkParams->LinkedSpeed != TempBitRate)
            {
                cbDebugPrint((MAKE_LEVEL(DP, WARNING), "%s: why current link speed:%d is not sync with DPCD100?\n",
                        FUNCTION_NAME, pMainLinkParams->LinkedSpeed));
                pMainLinkParams->LinkedSpeed = TempBitRate;
            }
        }
    }
    else if(!bUseLinkRateSet)
    {
        if (cb_ReadU32(pcbe->pAdapterContext, DP_REG_EPHY_STATUS[DPModuleIndex]) & 0x00010000)
        {
            if (pMainLinkParams->LinkedSpeed != CBIOS_DP_LINK_SPEED_2700Mbps)
            {
                cbDebugPrint((MAKE_LEVEL(DP, WARNING), "%s: why current link speed:%d is not sync with REG:0x%x?\n",
                              FUNCTION_NAME, pMainLinkParams->LinkedSpeed, DP_REG_EPHY_STATUS[DPModuleIndex]));
                pMainLinkParams->LinkedSpeed = CBIOS_DP_LINK_SPEED_2700Mbps;
            }
        }
        else
        {
            if (pMainLinkParams->LinkedSpeed != CBIOS_DP_LINK_SPEED_1620Mbps)
            {
                cbDebugPrint((MAKE_LEVEL(DP, WARNING), "%s: why current link speed:%d is not sync with REG:0x%x?\n",
                              FUNCTION_NAME, pMainLinkParams->LinkedSpeed, DP_REG_EPHY_STATUS[DPModuleIndex]));
                pMainLinkParams->LinkedSpeed = CBIOS_DP_LINK_SPEED_1620Mbps;
            }
        }
    }
    // TU: Transfer Unit(used to carry main video stream data during its horizontal active period).
    //     TU has 32 to 64 symbols per lane. Currently we default to 48.
    // MM8244, H width and TU size/ratio, MM821C[30]:Bit-12 of HDisp
    // TU ratio = (Dclk * bpp) * 32768 / (Ls_clk * Lane# * 8)
    //  bpp: bit per pixel
    //  bpc: bit per channel
    if (pMainLinkParams->ColorFormat == 1) // YUV422 format
    {
        ulTemp1 = pTiming->PixelClock*2*pMainLinkParams->bpc;
    }
    else
    {
        ulTemp1 = pTiming->PixelClock*3*pMainLinkParams->bpc;
    }

    ulTemp2 = pMainLinkParams->LinkedLaneNumber * pMainLinkParams->LinkedSpeed * 8;

    if (ulTemp2 == 0) // Prevent being divided by zero
    {
        cbDebugPrint((MAKE_LEVEL(DP, ERROR), "%s: fata error -- LaneNumber or LinkSpeed is ZERO!!!\n", FUNCTION_NAME));
        bStatus = CBIOS_FALSE;
        goto Exit;
    }
    ulTURatio = cb_do_div((ulTemp1<<15), ulTemp2);

    DPHWidthTURegValue.Value = 0;
    if (pMainLinkParams->LinkedLaneNumber == 0) // Prevent being divided by zero
    {
        cbDebugPrint((MAKE_LEVEL(DP, ERROR), "%s: fata error -- LaneNumber is ZERO!!!\n", FUNCTION_NAME));
        bStatus = CBIOS_FALSE;
        goto Exit;
    }

    DPHWidthTURegValue.Horiz_Width = cbRound((CBIOS_U32)pTiming->HorDisEnd, pMainLinkParams->LinkedLaneNumber, ROUND_UP) - 1;
    DPHWidthTURegValue.TU_Size = (pMainLinkParams->TUSize - 1);
    DPHWidthTURegValue.TU_Ratio = (CBIOS_U32)ulTURatio;
    DPHWidthTURegMask.Value = 0;
    cbMMIOWriteReg32(pcbe, DP_REG_HWIDTH_TU[DPModuleIndex], DPHWidthTURegValue.Value, DPHWidthTURegMask.Value);

    DPExtPacketRegValue.Value = 0;
    DPExtPacketRegValue.Horizontal_Width_bit12to11 = ((pTiming->HorDisEnd / pMainLinkParams->LinkedLaneNumber - 1) >> 11);
    DPExtPacketRegMask.Value = 0xFFFFFFFF;
    DPExtPacketRegMask.Horizontal_Width_bit12to11 = 0;
    cbMMIOWriteReg32(pcbe, DP_REG_EXT_PACKET[DPModuleIndex], DPExtPacketRegValue.Value, DPExtPacketRegMask.Value);

    // MM8248, H line duration: H * Ls_clk / Dclk
    // ulTemp2 = pDevCommon->DeviceParas.DPDevice.LinkSpeed / pTiming->DCLK;
    // ulTemp3 = ((ulTemp1 * ulTemp2) & 0x00000FFF ) << 15;
    ulTemp1 = pTiming->HorTotal - pTiming->HorDisEnd;

    if (pTiming->PixelClock == 0) // Prevent being divided by zero
    {
        cbDebugPrint((MAKE_LEVEL(DP, ERROR), "%s: fata error -- PixelClock is ZERO!!!\n", FUNCTION_NAME));
        bStatus = CBIOS_FALSE;
        goto Exit;
    }

    /*previous setting -35, will lead to audio abnormal with some specific timing(ex. 800x600@75), change it to 64*/
    ulTemp3 = (cb_do_div(ulTemp1 * pMainLinkParams->LinkedSpeed, pTiming->PixelClock) & 0x00000FFF) - 64;

    ulTemp2 = pTiming->HorTotal;
    ulTemp1 = cb_do_div(ulTemp2 * pMainLinkParams->LinkedSpeed, pTiming->PixelClock)  - 64;

    DPHLineDurRegValue.Value = 0;
    DPHLineDurRegValue.Horiz_Line_Duration = (CBIOS_U32)ulTemp1;
    DPHLineDurRegValue.HBLANK_Duration = (CBIOS_U32)ulTemp3;
    DPHLineDurRegMask.Value = 0xFFFFFFFF;
    DPHLineDurRegMask.Horiz_Line_Duration = 0;
    DPHLineDurRegMask.HBLANK_Duration = 0;
    cbMMIOWriteReg32(pcbe, DP_REG_HLINE_DUR[DPModuleIndex], DPHLineDurRegValue.Value, DPHLineDurRegMask.Value);

    DPEnableRegValue.Value = 0;
    DPEnableRegValue.Generate_MVID = pMainLinkParams->AsyncMode ? 1 : 0;
    DPEnableRegMask.Value = 0xFFFFFFFF;
    DPEnableRegMask.Generate_MVID = 0;
    cbMMIOWriteReg32(pcbe, DP_REG_ENABLE[DPModuleIndex], DPEnableRegValue.Value, DPEnableRegMask.Value);

    // M/N = Dclk / Ls_clk, but N must be 32768 to work with Parade DP
    // So, N = 32768, M = Dclk * 32768 / Ls_clk
    // Mvid and Nvid are not needed if in asynchronous mode, set it anyway
    if (pMainLinkParams->LinkedSpeed == 0) // Prevent being divided by zero
    {
        cbDebugPrint((MAKE_LEVEL(DP, ERROR), "%s: fata error -- LinkSpeed is ZERO!!!\n", FUNCTION_NAME));
        bStatus = CBIOS_FALSE;
        goto Exit;
    }
    ulTemp1 = cb_do_div(32768 * ((CBIOS_U64)(pTiming->PixelClock)), pMainLinkParams->LinkedSpeed); // N = 32768

    DPMisc0RegValue.Value = 0;
    DPMisc0RegValue.MVID = (CBIOS_U32)ulTemp1;

    if (pMainLinkParams->bpc == 6)
        DPMisc0RegValue.MISC0_Bit_depth = 0;
    else if (pMainLinkParams->bpc == 8)
        DPMisc0RegValue.MISC0_Bit_depth = 1;
    else if (pMainLinkParams->bpc == 10)
        DPMisc0RegValue.MISC0_Bit_depth = 2;
    else if (pMainLinkParams->bpc == 12)
        DPMisc0RegValue.MISC0_Bit_depth = 3;
    else if (pMainLinkParams->bpc == 16)
        DPMisc0RegValue.MISC0_Bit_depth = 4;

    DPMisc0RegValue.MISC0_Sync_Clk = pMainLinkParams->AsyncMode ? 0 : 1;
    DPMisc0RegValue.MISC0_Component_Format = pMainLinkParams->ColorFormat;
    
    // bit[27] "VESA/CEA range"
    if (pMainLinkParams->DynamicRange == 1)
    {
        DPMisc0RegValue.MISC0_Dynamic_Range = 1;
    }

    // bit[28] "YCbCr colorimetry" ITU-R BT601-5/ITU-R BT709-5
    if (pMainLinkParams->YCbCrCoefficients == 1)
    {
        DPMisc0RegValue.MISC0_YCbCr_Colorimetry = 1;
    }

    DPMisc0RegMask.Value = 0;
    cbMMIOWriteReg32(pcbe, DP_REG_MISC0[DPModuleIndex], DPMisc0RegValue.Value, DPMisc0RegMask.Value);

    // MM8250, DP HTotal attribute data
    DPHVTotalRegValue.H_Total = pTiming->HorTotal;
    DPHVTotalRegValue.V_Total = pTiming->VerTotal;
    DPHVTotalRegMask.Value = 0;
    cbMMIOWriteReg32(pcbe, DP_REG_HV_TOTAL[DPModuleIndex], DPHVTotalRegValue.Value, DPHVTotalRegMask.Value);

    // MM8254, DP H/V start attribute data (from leading edge of Sync)
    DPHVStartRegValue.H_Start = (pTiming->HorTotal - pTiming->HorSyncStart);
    DPHVStartRegValue.V_Start = (pTiming->VerTotal - pTiming->VerSyncStart);
    DPHVStartRegMask.Value = 0;
    cbMMIOWriteReg32(pcbe, DP_REG_HV_START[DPModuleIndex], DPHVStartRegValue.Value, DPHVStartRegMask.Value);

    // MM8258, DP H/V Sync Polarity, Width attribute data
    DPHVSyncRegValue.HSYNC_Width = (pTiming->HorSyncEnd - pTiming->HorSyncStart);
    DPHVSyncRegValue.VSYNC_Width = (pTiming->VerSyncEnd - pTiming->VerSyncStart);

    RegSR3AValue.Value = 0;
    RegSR3AValue.DP_PHY_Test_Mode_Select = 0;
    RegSR3AValue.FP_DP_HYSNC_POL = 0;
    RegSR3AValue.FP_DP_VYSNC_POL = 0;
    RegSR3AMask.Value = 0x38;
    cbMMIOWriteReg(pcbe, SR_3A, RegSR3AValue.Value, RegSR3AMask.Value);

    DPHVSyncRegValue.HSYNC_Polarity = 0;
    DPHVSyncRegValue.VSYNC_Polarity = 0;

    if (pTiming->HVPolarity & HorNEGATIVE)
    {
        RegSR3AValue.Value = 0;
        RegSR3AValue.DP_PHY_Test_Mode_Select = 0;
        RegSR3AValue.FP_DP_HYSNC_POL = 0;
        RegSR3AValue.FP_DP_VYSNC_POL = 1;
        RegSR3AMask.Value = 0x38;
        cbMMIOWriteReg(pcbe, SR_3A, RegSR3AValue.Value, RegSR3AMask.Value);
        DPHVSyncRegValue.HSYNC_Polarity = 1;
    }
    if (pTiming->HVPolarity & VerNEGATIVE)
    {
        RegSR3AValue.Value = 0;
        RegSR3AValue.DP_PHY_Test_Mode_Select = 0;
        RegSR3AValue.FP_DP_HYSNC_POL = 1;
        RegSR3AValue.FP_DP_VYSNC_POL = 0;
        RegSR3AMask.Value = 0x38;
        cbMMIOWriteReg(pcbe, SR_3A, RegSR3AValue.Value, RegSR3AMask.Value);
        DPHVSyncRegValue.VSYNC_Polarity = 1;
    }
    DPHVSyncRegMask.Value = 0;
    cbMMIOWriteReg32(pcbe, DP_REG_HV_SYNC[DPModuleIndex], DPHVSyncRegValue.Value, DPHVSyncRegMask.Value);

    // MM825C, DP H/V display attribute data
    DPHVActiveRegValue.Acitve_Width = pTiming->HorDisEnd;
    DPHVActiveRegValue.Active_Height = pTiming->VerDisEnd;
    DPHVActiveRegMask.Value = 0;
    cbMMIOWriteReg32(pcbe, DP_REG_HV_ACTIVE[DPModuleIndex], DPHVActiveRegValue.Value, DPHVActiveRegMask.Value);

    // MM8210, DP Nvid attribute data, N=32768
    DPMisc1RegValue.Value = 0;
    DPMisc1RegValue.NVID = 0x008000;
    DPMisc1RegValue.MISC1_Even = 0;
    DPMisc1RegValue.MISC1_Stereo_Video = 0;
    DPMisc1RegValue.MISC1_Reserved = 0;
    DPMisc1RegMask.Value = 0xFFFFFFFF;
    DPMisc1RegMask.NVID = 0;
    DPMisc1RegMask.MISC1_Even = 0;
    DPMisc1RegMask.MISC1_Stereo_Video = 0;
    DPMisc1RegMask.MISC1_Reserved = 0;
    cbMMIOWriteReg32(pcbe, DP_REG_MISC1[DPModuleIndex], DPMisc1RegValue.Value, DPMisc1RegMask.Value);

    // MM8218, DP input and general control
    DPGenCtrlRegValue.Value = 0;
    DPGenCtrlRegValue.DELAY = 0x20;
    DPGenCtrlRegMask.Value = 0xFFFFFFFF;
    DPGenCtrlRegMask.DELAY = 0;
    cbMMIOWriteReg32(pcbe, DP_REG_GEN_CTRL[DPModuleIndex], DPGenCtrlRegValue.Value, DPGenCtrlRegMask.Value);

    bStatus = CBIOS_TRUE;

    cbTraceExit(DP);
Exit:
    return bStatus;
}

CBIOS_VOID cbDIU_DP_SendInfoFrame(PCBIOS_VOID pvcbe, CBIOS_MODULE_INDEX DPModuleIndex, CBIOS_U32 Length)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    CBIOS_U32  StartAddress = 0;
    REG_MM8240 DPEnableInfoFrameRegValue, DPEnableInfoFrameRegMask;
    REG_MM8338 DPMuteRegValue, DPMuteRegMask;

    if (DPModuleIndex >= DP_MODU_NUM)
    {
        cbDebugPrint((MAKE_LEVEL(DP, ERROR), "%s: invalid DP module index!\n", FUNCTION_NAME));
        return;
    }

    // InfoFrame from FIFO
    DPMuteRegValue.Value = 0;
    DPMuteRegValue.Audio_InfoFrame = 0;
    DPMuteRegMask.Value = 0xFFFFFFFF;
    DPMuteRegMask.Audio_InfoFrame = 0;
    cbMMIOWriteReg32(pcbe, DP_REG_MUTE[DPModuleIndex], DPMuteRegValue.Value, DPMuteRegMask.Value);

    DPEnableInfoFrameRegValue.Value = 0;
    DPEnableInfoFrameRegValue.InfoFrame_FIFO_Select = 0; // select FIFO 1
    DPEnableInfoFrameRegValue.InfoFrame_FIFO_1_Ready = 1;
    DPEnableInfoFrameRegValue.InfoFrame_FIFO_1_Start_Address = StartAddress;
    DPEnableInfoFrameRegValue.InfoFrame_FIFO_1_Length = Length;
    DPEnableInfoFrameRegMask.Value = 0xFFFFFFFF;
    DPEnableInfoFrameRegMask.InfoFrame_FIFO_Select = 0;
    DPEnableInfoFrameRegMask.InfoFrame_FIFO_1_Ready = 0;
    DPEnableInfoFrameRegMask.InfoFrame_FIFO_1_Start_Address = 0;
    DPEnableInfoFrameRegMask.InfoFrame_FIFO_1_Length = 0;
    cbMMIOWriteReg32(pcbe, DP_REG_ENABLE[DPModuleIndex], DPEnableInfoFrameRegValue.Value, DPEnableInfoFrameRegMask.Value);
}

/***************************************************************************************/
/*******************************  Aux Channel Interfaces  ******************************/
CBIOS_VOID cbDIU_DP_ResetAUX(PCBIOS_VOID pvcbe, CBIOS_MODULE_INDEX DPModuleIndex)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    REG_MM8214    DPLinkRegValue, DPLinkRegMask;
    REG_MM8338    DPMuteRegValue, DPMuteRegMask;

    if (DPModuleIndex >= DP_MODU_NUM)
    {
        cbDebugPrint((MAKE_LEVEL(DP, ERROR), "%s: invalid DP module index!\n", FUNCTION_NAME));
        return;
    }

    DPMuteRegValue.Value = 0;
    DPMuteRegMask.Value = 0xFFFFFFFF;
    DPMuteRegMask.AUX_SW_Reset = 0;
    
    DPMuteRegValue.AUX_SW_Reset = 1;
    cbMMIOWriteReg32(pcbe, DP_REG_MUTE[DPModuleIndex], DPMuteRegValue.Value, DPMuteRegMask.Value);

    cb_DelayMicroSeconds(1000); // wait AUX Reset to be done

    DPMuteRegValue.AUX_SW_Reset = 0;
    cbMMIOWriteReg32(pcbe, DP_REG_MUTE[DPModuleIndex], DPMuteRegValue.Value, DPMuteRegMask.Value);

    DPLinkRegValue.Value = 0;
    DPLinkRegMask.Value = 0xFFFFFFFF;
    DPLinkRegMask.SW_Hpd_assert = 0;
    
    DPLinkRegValue.SW_Hpd_assert = 1;
    cbMMIOWriteReg32(pcbe, DP_REG_LINK[DPModuleIndex], DPLinkRegValue.Value, DPLinkRegMask.Value);

    DPLinkRegValue.SW_Hpd_assert = 0;
    cbMMIOWriteReg32(pcbe, DP_REG_LINK[DPModuleIndex], DPLinkRegValue.Value, DPLinkRegMask.Value);

    cb_DelayMicroSeconds(200);
}

static CBIOS_VOID cbDIU_DP_ClearAuxReadBuffer(PCBIOS_EXTENSION_COMMON pcbe, CBIOS_MODULE_INDEX DPModuleIndex)
{
    REG_MM8320    DPAuxRead0RegValue;
    REG_MM8324    DPAuxRead1RegValue;
    REG_MM8328    DPAuxRead2RegValue;
    REG_MM832C    DPAuxRead3RegValue;

    if (DPModuleIndex >= DP_MODU_NUM)
    {
        cbDebugPrint((MAKE_LEVEL(DP, ERROR), "%s: invalid DP module index!\n", FUNCTION_NAME));
        return;
    }

    DPAuxRead0RegValue.Value = 0;
    DPAuxRead0RegValue.AUX_Read_Bytes_3_0 = 0;
    cb_WriteU32(pcbe->pAdapterContext, DP_REG_AUX_READ0[DPModuleIndex], DPAuxRead0RegValue.Value);
    DPAuxRead1RegValue.Value = 0;
    DPAuxRead1RegValue.AUX_Read_Bytes_7_4 = 0;
    cb_WriteU32(pcbe->pAdapterContext, DP_REG_AUX_READ1[DPModuleIndex], DPAuxRead1RegValue.Value);
    DPAuxRead2RegValue.Value = 0;
    DPAuxRead2RegValue.AUX_Read_Bytes_11_8 = 0;
    cb_WriteU32(pcbe->pAdapterContext, DP_REG_AUX_READ2[DPModuleIndex], DPAuxRead2RegValue.Value);
    DPAuxRead3RegValue.Value = 0;
    DPAuxRead3RegValue.AUX_Read_Bytes_15_12 = 0;
    cb_WriteU32(pcbe->pAdapterContext, DP_REG_AUX_READ3[DPModuleIndex], DPAuxRead3RegValue.Value);
}

static CBIOS_VOID cbDIU_DP_ClearAuxWriteBuffer(PCBIOS_EXTENSION_COMMON pcbe, CBIOS_MODULE_INDEX DPModuleIndex)
{
    REG_MM8310    DPAuxWrite0RegValue;
    REG_MM8314    DPAuxWrite1RegValue;
    REG_MM8318    DPAuxWrite2RegValue;
    REG_MM831C    DPAuxWrite3RegValue;

    if (DPModuleIndex >= DP_MODU_NUM)
    {
        cbDebugPrint((MAKE_LEVEL(DP, ERROR), "%s: invalid DP module index!\n", FUNCTION_NAME));
        return;
    }

    DPAuxWrite0RegValue.Value = 0;
    DPAuxWrite0RegValue.AUX_Write_Bytes_3_0 = 0;
    cb_WriteU32(pcbe->pAdapterContext, DP_REG_AUX_WRITE0[DPModuleIndex], DPAuxWrite0RegValue.Value);
    DPAuxWrite1RegValue.Value = 0;
    DPAuxWrite1RegValue.AUX_Write_Bytes_7_4 = 0;
    cb_WriteU32(pcbe->pAdapterContext, DP_REG_AUX_WRITE1[DPModuleIndex], DPAuxWrite1RegValue.Value);
    DPAuxWrite2RegValue.Value = 0;
    DPAuxWrite2RegValue.AUX_Write_Bytes_11_8 = 0;
    cb_WriteU32(pcbe->pAdapterContext, DP_REG_AUX_WRITE2[DPModuleIndex], DPAuxWrite2RegValue.Value);
    DPAuxWrite3RegValue.Value = 0;
    DPAuxWrite3RegValue.AUX_Write_Bytes_15_12 = 0;
    cb_WriteU32(pcbe->pAdapterContext, DP_REG_AUX_WRITE3[DPModuleIndex], DPAuxWrite3RegValue.Value);
}

static CBIOS_VOID cbDIU_DP_SWAuxRequest(PCBIOS_EXTENSION_COMMON pcbe, CBIOS_MODULE_INDEX DPModuleIndex)
{
    REG_MM8330    DPAuxTimerRegValue, DPAuxTimerRegMask;

    if (DPModuleIndex >= DP_MODU_NUM)
    {
        cbDebugPrint((MAKE_LEVEL(DP, ERROR), "%s: invalid DP module index!\n", FUNCTION_NAME));
        return;
    }

    DPAuxTimerRegValue.Value = 0;
    DPAuxTimerRegValue.SW_AUX = 1;
    DPAuxTimerRegValue.AUX_Request = 1;
    DPAuxTimerRegValue.AUX_DRDY = 0;
    DPAuxTimerRegValue.AUX_Timeout = 0;
    DPAuxTimerRegMask.Value = 0xFFFFFFFF;
    DPAuxTimerRegMask.SW_AUX = 0;
    DPAuxTimerRegMask.AUX_Request = 0;
    DPAuxTimerRegMask.AUX_DRDY = 0;
    DPAuxTimerRegMask.AUX_Timeout = 0;
    cbMMIOWriteReg32(pcbe, DP_REG_AUX_TIMER[DPModuleIndex], DPAuxTimerRegValue.Value, DPAuxTimerRegMask.Value);
}

CBIOS_VOID cbDIU_DP_HWUseAuxChannel(PCBIOS_VOID pvcbe, CBIOS_MODULE_INDEX DPModuleIndex)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    REG_MM8330    DPAuxTimerRegValue, DPAuxTimerRegMask;

    if (DPModuleIndex >= DP_MODU_NUM)
    {
        cbDebugPrint((MAKE_LEVEL(DP, ERROR), "%s: invalid DP module index!\n", FUNCTION_NAME));
        return;
    } 

    DPAuxTimerRegValue.Value = 0;
    DPAuxTimerRegValue.SW_AUX = 0;
    DPAuxTimerRegValue.AUX_Request = 0;
    DPAuxTimerRegValue.AUX_DRDY = 0;
    DPAuxTimerRegValue.AUX_Timeout = 0;
    
    DPAuxTimerRegMask.Value = 0xFFFFFFFF;
    DPAuxTimerRegMask.SW_AUX = 0;
    DPAuxTimerRegMask.AUX_Request = 0;
    DPAuxTimerRegMask.AUX_DRDY = 0;
    DPAuxTimerRegMask.AUX_Timeout = 0;
    
    cbMMIOWriteReg32(pcbe, DP_REG_AUX_TIMER[DPModuleIndex], DPAuxTimerRegValue.Value, DPAuxTimerRegMask.Value);
}

// IsNative = 1: native aux read/write
// IsNative = 0: I2C over aux read/write
static CBIOS_BOOL cbDIU_DP_CheckAuxReplyStatus(PCBIOS_EXTENSION_COMMON pcbe, CBIOS_MODULE_INDEX DPModuleIndex, PCBIOS_DP_MONITOR_CONTEXT pDPMonitorContext, CBIOS_BOOL IsNative)
{
    // 20090326_BC
    // 4.2.1.2 will not send AUX STOP so we won't get AUX ready usually.
    // But sometimes, after ~600us, HW would set AUX ready with timeout bit.
    // For the EDID read intermittent error cited below we cannot rely on timeout bit checking.
    // To workaround this problem, reduce counter from 13 to 11.
    // So resend the request after 550us instead of 650us.
    CBIOS_U32     i, j, counter;
    CBIOS_BOOL    bStatus = CBIOS_FALSE;
    REG_MM8330    DPAuxTimerRegValue;

    if (DPModuleIndex >= DP_MODU_NUM)
    {
        cbDebugPrint((MAKE_LEVEL(DP, ERROR), "%s: invalid DP module index!\n", FUNCTION_NAME));
        return  CBIOS_FALSE;
    }

    //increase the waiting time in case EDID can't be read correctly
    //now SW will resend the request after 10ms
    counter = IsNative ? 10 : 100;

    // Tunneling aux timeout > 3600us
    if (pDPMonitorContext->DpUsbMode == CBIOS_DP_TUNNEL)
    {
        counter = 50;
    }

    for(i = 0; i < 10; i++)
    {
        for (j = 1; j < counter; j++)
        {
            cb_DelayMicroSeconds(100);
            // Need to check timeout (bit 3) as well
            // But when reading EDID sometime, timeout got set but the correct data did arrive,
            // making us throwing away good data and end up with bad checksum.
            // Going back to old method (no timeout check).
            DPAuxTimerRegValue.Value = cb_ReadU32(pcbe->pAdapterContext, DP_REG_AUX_TIMER[DPModuleIndex]) & 0x3C000007;

            // [29:26]: Reply AUX command status
            // [3:0]: AUX command status
            if (DPAuxTimerRegValue.SW_AUX && DPAuxTimerRegValue.AUX_DRDY)
            {
                if ((DPAuxTimerRegValue.AUX_Command == CBIOS_AUX_REPLY_ACK) ||
                    (DPAuxTimerRegValue.AUX_Command == CBIOS_AUX_REPLY_NACK))
                {
                    if (DPAuxTimerRegValue.AUX_Command == CBIOS_AUX_REPLY_NACK)
                    {
                        cbDebugPrint((MAKE_LEVEL(DP, DEBUG), "%s: Sink send NACK reply, just abort current aux transaction\n", FUNCTION_NAME));
                    }

                    bStatus = CBIOS_TRUE;
                    break;
                }
            }
        }

        if (bStatus)
        {
            break;
        }
        else
        {

#if DP_RESET_AUX_WHEN_DEFER
            cbDIU_DP_ResetAUX(pcbe, DPModuleIndex);
#endif
            cbDebugPrint((MAKE_LEVEL(DP, DEBUG), "%s: Failed! i=%d, j=%d, mm%x=0x%08x\n", FUNCTION_NAME, 
                          i, j, DP_REG_AUX_TIMER[DPModuleIndex], DPAuxTimerRegValue.Value));

            // I2C over AUX channel is much slower than native AUX channel transaction
            // I2C R/W over AUX channel defer case
            if (DPAuxTimerRegValue.AUX_Command == CBIOS_AUX_REPLY_DEFER)
            {
                cbDelayMilliSeconds(1);
            }

            // Can satisfy 4.2.1.1 &4.2.1.2 compliance test cause 600 us has passed
            cbDIU_DP_SWAuxRequest(pcbe, DPModuleIndex);
        }
    }

    return bStatus;
}

// Read all HPD status of standard-DP/tunnel/alt-mode, and decide which mode to work
CBIOS_VOID cbDIU_DP_GetConnectStatus(PCBIOS_EXTENSION_COMMON pcbe, CBIOS_MODULE_INDEX DPModuleIndex, PCBIOS_CONNECT_STATUS pConnectStatus)
{
    REG_MM33570 DPEphyMuxHpd;
    REG_MM8330  DPAuxTimerReg;

    if (pcbe->ChipID != CHIPID_CNE001 || DPModuleIndex >= DP_USB_MODU_NUM)
    {
        DPAuxTimerReg.Value = cb_ReadU32(pcbe->pAdapterContext, DP_REG_AUX_TIMER[DPModuleIndex]);

        pConnectStatus->HpdStatus = (CBIOS_U8)DPAuxTimerReg.HPD_Status;
        pConnectStatus->DpUsbMode = CBIOS_DP_STANDARD;
    }
    else
    {
        DPAuxTimerReg.Value = cb_ReadU32(pcbe->pAdapterContext, DP_REG_AUX_TIMER[DPModuleIndex]);
        DPEphyMuxHpd.Value  = cb_ReadU32(pcbe->pAdapterContext, DP_REG_EPHY_MUX_HPD[DPModuleIndex]);

        // Priority HDMI/DP > Tunnel/Alt-mode
        if ((DPAuxTimerReg.HPD_Status == CBIOS_HPD_STATUS_IN) || (DPAuxTimerReg.HPD_Status == CBIOS_HPD_STATUS_IRQ))
        {
            pConnectStatus->HpdStatus = (CBIOS_U8)DPAuxTimerReg.HPD_Status;
            pConnectStatus->DpUsbMode = CBIOS_DP_STANDARD;
        }
        else if ((DPEphyMuxHpd.Tunnel_HPD_Status == CBIOS_HPD_STATUS_IN) || (DPEphyMuxHpd.Tunnel_HPD_Status == CBIOS_HPD_STATUS_IRQ))
        {
            pConnectStatus->HpdStatus = (CBIOS_U8)DPEphyMuxHpd.Tunnel_HPD_Status;
            pConnectStatus->DpUsbMode = CBIOS_DP_TUNNEL;
        }
        else if ((DPEphyMuxHpd.AltMode_HPD_Status == CBIOS_HPD_STATUS_IN) || (DPEphyMuxHpd.AltMode_HPD_Status == CBIOS_HPD_STATUS_IRQ))
        {
            pConnectStatus->HpdStatus = (CBIOS_U8)DPEphyMuxHpd.AltMode_HPD_Status;
            pConnectStatus->DpUsbMode = CBIOS_DP_ALTMODE;
        }
        else  // all plug out
        {
            pConnectStatus->HpdStatus = CBIOS_HPD_STATUS_OUT;
            pConnectStatus->DpUsbMode = CBIOS_DP_STANDARD;
        }
    }
}

CBIOS_U8 cbDIU_DP_GetHpdStatus(PCBIOS_EXTENSION_COMMON pcbe, CBIOS_MODULE_INDEX DPModuleIndex, CBIOS_DP_USB_MODE DpUsbMode)
{
    CBIOS_U8  hpdStatus = 0;
    REG_MM33570  DPEphyMuxHpd;
    REG_MM8330  DPAuxTimerRegValue;

    if (DpUsbMode == CBIOS_DP_TUNNEL)
    {
        DPEphyMuxHpd.Value = cb_ReadU32(pcbe->pAdapterContext, DP_REG_EPHY_MUX_HPD[DPModuleIndex]);
        hpdStatus = (CBIOS_U8)DPEphyMuxHpd.Tunnel_HPD_Status;
    }
    else if (DpUsbMode == CBIOS_DP_ALTMODE)
    {
        DPEphyMuxHpd.Value = cb_ReadU32(pcbe->pAdapterContext, DP_REG_EPHY_MUX_HPD[DPModuleIndex]);
        hpdStatus = (CBIOS_U8)DPEphyMuxHpd.AltMode_HPD_Status;
    }
    else
    {
        DPAuxTimerRegValue.Value = cb_ReadU32(pcbe->pAdapterContext, DP_REG_AUX_TIMER[DPModuleIndex]);
        hpdStatus = (CBIOS_U8)DPAuxTimerRegValue.HPD_Status;
    }

    return hpdStatus;
}

static CBIOS_BOOL cbDIU_DP_WaitAuxReady(PCBIOS_EXTENSION_COMMON pcbe, CBIOS_MODULE_INDEX DPModuleIndex, PCBIOS_DP_MONITOR_CONTEXT pDPMonitorContext)
{
    CBIOS_U32     i = 0;
    REG_MM8330    DPAuxTimerRegValue;
    CBIOS_U8      hpdStatus;

    if (DPModuleIndex >= DP_MODU_NUM)
    {
        cbDebugPrint((MAKE_LEVEL(DP, ERROR), "%s: invalid DP module index!\n", FUNCTION_NAME));
        return  CBIOS_FALSE;
    }

    hpdStatus = cbDIU_DP_GetHpdStatus(pcbe, DPModuleIndex, pDPMonitorContext->DpUsbMode);

    if (hpdStatus == CBIOS_HPD_STATUS_OUT || hpdStatus == CBIOS_HPD_STATUS_INIT)  // unplug
    {
        cbDebugPrint((MAKE_LEVEL(DP, DEBUG), "%s: Aux can't work when DP is unplugged, hpdStatus==0x%x!!!\n", FUNCTION_NAME, hpdStatus));
        return CBIOS_FALSE;
    }

    DPAuxTimerRegValue.Value = cb_ReadU32(pcbe->pAdapterContext, DP_REG_AUX_TIMER[DPModuleIndex]);
    while ((DPAuxTimerRegValue.AUX_Request) || // Data not sent yet
           (((DPAuxTimerRegValue.Value & 0xE) != 0x0) && (!(DPAuxTimerRegValue.Value& 0xC)))) // If not the first aux cmd, data should be ready or time out
    {
        cbDebugPrint((MAKE_LEVEL(DP, WARNING), "%s: Aux channel is busy, mm%x==%08x, wait 100us!\n", 
            FUNCTION_NAME, DP_REG_AUX_TIMER[DPModuleIndex], DPAuxTimerRegValue.Value));
        i++;
        if (i > 15) // 1.5ms time out
        {
            cbDebugPrint((MAKE_LEVEL(DP, ERROR), "%s: Aux channel is busy for 15ms, time out!\n", FUNCTION_NAME));
            return CBIOS_FALSE;
        }
        cb_DelayMicroSeconds(100);
        DPAuxTimerRegValue.Value = cb_ReadU32(pcbe->pAdapterContext, DP_REG_AUX_TIMER[DPModuleIndex]);
    }

    return CBIOS_TRUE;
}

static CBIOS_BOOL cbDIU_DP_Aux_Cmd(PCBIOS_EXTENSION_COMMON pcbe, CBIOS_U32 addr, CBIOS_U32 numData, CBIOS_U8 cmd, CBIOS_MODULE_INDEX DPModuleIndex)
{
    PCBIOS_DEVICE_COMMON  pDevCommon = CBIOS_NULL;
    PCBIOS_DP_CONTEXT  pDpContext = CBIOS_NULL;
    CBIOS_U32     Device = CBIOS_TYPE_NONE;
    CBIOS_BOOL    bStatus = CBIOS_FALSE;
    REG_MM8334    DPAuxCmdRegValue;
    CBIOS_BOOL    IsNative;

    Device = cbGetDeviceType(pcbe, DPModuleIndex, CBIOS_MODULE_TYPE_DP);
    if (Device == CBIOS_TYPE_NONE)
    {
        cbDebugPrint((MAKE_LEVEL(DP, WARNING), "%s: invalid DP module index! \n", FUNCTION_NAME));
        return bStatus;
    }
    pDevCommon = cbGetDeviceCommon(&pcbe->DeviceMgr, Device);
    pDpContext = container_of(pDevCommon, PCBIOS_DP_CONTEXT, Common);

    if (!cbDIU_DP_WaitAuxReady(pcbe, DPModuleIndex, &(pDpContext->DPMonitorContext)))
    {
        return CBIOS_FALSE;
    }

    if ((cmd & CBIOS_AUX_REQUEST_NATIVE_READ) || (cmd & CBIOS_AUX_REQUEST_NATIVE_WRITE))
    {
        IsNative = CBIOS_TRUE;
    }
    else
    {
        IsNative = CBIOS_FALSE;
    }

    DPAuxCmdRegValue.Value = 0;
    DPAuxCmdRegValue.SW_AUX_Addr = addr;
    DPAuxCmdRegValue.SW_AUX_Length = numData;
    DPAuxCmdRegValue.SW_AUX_CMD = cmd;
    cb_WriteU32(pcbe->pAdapterContext, DP_REG_AUX_CMD[DPModuleIndex], DPAuxCmdRegValue.Value);

    cbDIU_DP_SWAuxRequest(pcbe, DPModuleIndex);
    bStatus = cbDIU_DP_CheckAuxReplyStatus(pcbe, DPModuleIndex, &(pDpContext->DPMonitorContext), IsNative);

    return bStatus;

}

static CBIOS_VOID cbDIU_DP_SetAuxWriteBuffer(PCBIOS_EXTENSION_COMMON pcbe, CBIOS_MODULE_INDEX DPModuleIndex, PAUX_CONTROL pAUX)
{
    CBIOS_U32     i = 0;
    REG_MM8310    DPAuxWrite0RegValue;
    REG_MM8314    DPAuxWrite1RegValue;
    REG_MM8318    DPAuxWrite2RegValue;
    REG_MM831C    DPAuxWrite3RegValue;
    CBIOS_U8*     buffer = pAUX->Buffer;
    CBIOS_U32     Data[4] = {0};

    //  first clear write buffer
    cbDIU_DP_ClearAuxWriteBuffer(pcbe, DPModuleIndex);

    cb_memcpy((PCBIOS_U8)(Data), buffer, pAUX->Length);

    // Clear Data[i] over pAUX->Lenth
    for(i = pAUX->Length; i < 0x10; i++)
        *((PCBIOS_U8)(Data) + i) = 0x00;

    DPAuxWrite0RegValue.Value = Data[0];
    DPAuxWrite1RegValue.Value = Data[1];
    DPAuxWrite2RegValue.Value = Data[2];
    DPAuxWrite3RegValue.Value = Data[3];

    cb_WriteU32(pcbe->pAdapterContext, DP_REG_AUX_WRITE0[DPModuleIndex], DPAuxWrite0RegValue.Value);
    cb_WriteU32(pcbe->pAdapterContext, DP_REG_AUX_WRITE1[DPModuleIndex], DPAuxWrite1RegValue.Value);
    cb_WriteU32(pcbe->pAdapterContext, DP_REG_AUX_WRITE2[DPModuleIndex], DPAuxWrite2RegValue.Value);
    cb_WriteU32(pcbe->pAdapterContext, DP_REG_AUX_WRITE3[DPModuleIndex], DPAuxWrite3RegValue.Value);
}

static CBIOS_VOID cbDIU_DP_GetAuxReadBuffer(PCBIOS_EXTENSION_COMMON pcbe, CBIOS_MODULE_INDEX DPModuleIndex, PAUX_CONTROL pAUX)
{
    CBIOS_U32   i = 0;
    CBIOS_U32   Data[4] = {0};
    CBIOS_U8*   buffer = pAUX->Buffer;

    if (DPModuleIndex >= DP_MODU_NUM)
    {
        cbDebugPrint((MAKE_LEVEL(DP, ERROR), "%s: invalid DP module index!\n", FUNCTION_NAME));
        return;
    } 

    Data[0] = cb_ReadU32(pcbe->pAdapterContext, DP_REG_AUX_READ0[DPModuleIndex]);
    Data[1] = cb_ReadU32(pcbe->pAdapterContext, DP_REG_AUX_READ1[DPModuleIndex]);
    Data[2] = cb_ReadU32(pcbe->pAdapterContext, DP_REG_AUX_READ2[DPModuleIndex]);
    Data[3] = cb_ReadU32(pcbe->pAdapterContext, DP_REG_AUX_READ3[DPModuleIndex]);

    // Clear Data[i] over pAUX->Lenth
    for(i = pAUX->Length; i < 0x10; i++)
    {
        *((PCBIOS_U8)(Data) + i) = 0x00;
    }
    cb_memcpy(buffer, (PCBIOS_U8)(Data), pAUX->Length);

}

static CBIOS_BOOL cbDIU_DP_AuxReplyStatus(PCBIOS_EXTENSION_COMMON pcbe, CBIOS_MODULE_INDEX DPModuleIndex)
{
    CBIOS_BOOL    bStatus = CBIOS_FALSE;
    REG_MM8330    DPAuxTimerRegValue;

    if (DPModuleIndex >= DP_MODU_NUM)
    {
        cbDebugPrint((MAKE_LEVEL(DP, ERROR), "%s: invalid DP module index!\n", FUNCTION_NAME));
        return  CBIOS_FALSE;
    }

    DPAuxTimerRegValue.Value = cb_ReadU32(pcbe->pAdapterContext, DP_REG_AUX_TIMER[DPModuleIndex]);

    if(DPAuxTimerRegValue.AUX_DRDY)
        bStatus = CBIOS_TRUE;

    return bStatus;
}

static CBIOS_BOOL  cbDIU_AUX_Write_Data(PCBIOS_VOID pvcbe, CBIOS_MODULE_INDEX DPModuleIndex, PAUX_CONTROL pAUX)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    CBIOS_BOOL    bStatus = CBIOS_TRUE;
    CBIOS_U32     Address = 0;

    if((pAUX->Function & ~CBIOS_AUX_REQUEST_I2C_MOT) == CBIOS_AUX_REQUEST_I2C_WRITE)
    {
        Address = (CBIOS_U32)pAUX->SlaveAddress;
        Address = Address >> 1;
    }
    else
    {
        Address = pAUX->Offset;
    }

    if(pAUX->Length != 0)
    {
        cbDIU_DP_SetAuxWriteBuffer(pcbe, DPModuleIndex, pAUX);//set data
    }

    bStatus = cbDIU_DP_Aux_Cmd(pcbe, Address, pAUX->Length, pAUX->Function, DPModuleIndex);// AUX cmd

    return bStatus;

}

static CBIOS_BOOL cbDIU_AUX_Read_Data(PCBIOS_VOID pvcbe, CBIOS_MODULE_INDEX DPModuleIndex, PAUX_CONTROL pAUX)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    CBIOS_BOOL    bStatus = CBIOS_TRUE;
    CBIOS_U32     Address = 0;

    if ((pAUX->Function & ~CBIOS_AUX_REQUEST_I2C_MOT) == CBIOS_AUX_REQUEST_I2C_READ)
    {
        Address = (CBIOS_U32)pAUX->SlaveAddress;
        Address = Address >> 1;
    }
    else
    {
        Address = pAUX->Offset;
    }

    cbDIU_DP_ClearAuxReadBuffer(pcbe, DPModuleIndex);

    bStatus = cbDIU_DP_Aux_Cmd(pcbe, Address, pAUX->Length, pAUX->Function, DPModuleIndex);// AUX cmd

    if (pAUX->Length != 0)
    {
        cbDIU_DP_GetAuxReadBuffer(pcbe, DPModuleIndex, pAUX);//get data
    }

    return bStatus;

}

CBIOS_BOOL cbDIU_DP_Auxtransfer(PCBIOS_VOID pvcbe, CBIOS_MODULE_INDEX DPModuleIndex, PAUX_CONTROL pAUX)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    CBIOS_BOOL  bStatus = CBIOS_FALSE;
    REG_MM82CC  DPEphyCtrlRegValue;

    if (DPModuleIndex >= DP_MODU_NUM)
    {
        cbDebugPrint((MAKE_LEVEL(DP, ERROR), "%s: invalid DP module index!\n", FUNCTION_NAME));
        return CBIOS_FALSE;
    }

    DPEphyCtrlRegValue.Value = cb_ReadU32(pcbe->pAdapterContext, DP_REG_EPHY_CTRL[DPModuleIndex]);
    if(!DPEphyCtrlRegValue.check_sync_cnt)//check 82cc Check_Sync_Cnt bit
    {
        DPEphyCtrlRegValue.check_sync_cnt = 1;//HW don't check sync counter for aux receiver
        cb_WriteU32(pcbe->pAdapterContext, DP_REG_EPHY_CTRL[DPModuleIndex], DPEphyCtrlRegValue.Value);
    }

    if(!cbPHY_DP_IsAuxPowerOn(pcbe, DPModuleIndex))
    {
        cbPHY_DP_AuxPowerOnOff(pcbe, DPModuleIndex, CBIOS_TRUE);
    }

    cbDIU_DP_ResetAUX(pcbe, (CBIOS_U8)DPModuleIndex);

    if(pAUX->Function == CBIOS_AUX_REQUEST_INNER_WRITE)
    {
        // Communication within the chip, defined by HW
        bStatus = cbDIU_AUX_Write_Data(pcbe, DPModuleIndex, pAUX);
    }
    else
    {
        switch (pAUX->Function & ~CBIOS_AUX_REQUEST_I2C_MOT)
        {
        case CBIOS_AUX_REQUEST_I2C_READ:
        case CBIOS_AUX_REQUEST_NATIVE_READ:
            bStatus = cbDIU_AUX_Read_Data(pcbe, DPModuleIndex, pAUX);
            break;
        case CBIOS_AUX_REQUEST_I2C_WRITE:
        case CBIOS_AUX_REQUEST_NATIVE_WRITE:
            bStatus = cbDIU_AUX_Write_Data(pcbe, DPModuleIndex, pAUX);
            break;
        default:
            cbDebugPrint((MAKE_LEVEL(DP, ERROR), "%s: invalid aux function!\n", FUNCTION_NAME));
            break;
        }
    }

    // return AUX CH control to HW
    cbDIU_DP_HWUseAuxChannel(pcbe, DPModuleIndex);

    return bStatus;

}

CBIOS_BOOL cbDIU_DP_IsOn(PCBIOS_VOID pvcbe, CBIOS_MODULE_INDEX DPModuleIndex)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    REG_MM8240     DPEnableRegValue;
    CBIOS_BOOL     status = CBIOS_FALSE;

    if (DPModuleIndex == CBIOS_MODULE_INDEX_INVALID)
    {
        cbDebugPrint((MAKE_LEVEL(DP, ERROR), "%s: invalid DP module index!\n", FUNCTION_NAME));
        return status;
    }

    DPEnableRegValue.Value = cb_ReadU32(pcbe->pAdapterContext, DP_REG_ENABLE[DPModuleIndex]);

    if(DPEnableRegValue.DP_Enable)
    {
        status = CBIOS_TRUE;
    }
    else
    {
        status = CBIOS_FALSE;
    }

    return status;
}

CBIOS_BOOL cbDIU_EDP_PsrOp0_1(PCBIOS_VOID pvcbe, CBIOS_MODULE_INDEX DPModuleIndex)
{
    PCBIOS_EXTENSION_COMMON pcbe          = (PCBIOS_EXTENSION_COMMON)pvcbe;
    REG_MM8368_E3K              DPSwingRegValue, DPSwingRegMask;
    REG_MM334BC_E3K              EDPPsrCtl1RegValue, EDPPsrCtl1RegMask;
    REG_MM334C4_E3K              EDPPsrCtl2RegValue, EDPPsrCtl2RegMask;
    CBIOS_U8 Data[2] = {0};
    DPCD_REG_00170 DPCD_00170 = {0};
    DPCD_REG_00600 DPCD_00600 = {0};
    DPCD_REG_00071 DPCD_00071 = {0};
    CBIOS_U32   PsrSetupTime = 330;
    CBIOS_BOOL bStatus = CBIOS_TRUE;

    if (DPModuleIndex >= DP_MODU_NUM)
    {
        cbDebugPrint((MAKE_LEVEL(DP, ERROR), "%s: invalid DP module index!\n", FUNCTION_NAME));
        return  CBIOS_FALSE;
    }

    cbDelayMilliSeconds(1);

    //enable sink psr function
    DPCD_00170.Value = 0;
    DPCD_00170.ENABLE_PSR_IN_SINK = 0;//need set other caps before enable sink psr function
    DPCD_00170.MAINLINK_ACTIVE = 0;
    DPCD_00170.CRC_VERIFICATION_ACTIVE = 0;
    DPCD_00170.FRAME_CAPTURE_INDICATION = 0;
    Data[0] = DPCD_00170.Value;
    if(!cbDIU_Aux_Dpcd_Write(pcbe, DPModuleIndex, DP_PSR_EN_CFG, Data, 1))
    {
        bStatus = CBIOS_FALSE;
        cbDebugPrint((MAKE_LEVEL(DP, DEBUG), "%s: Write DPCD 170h fail!\n", FUNCTION_NAME));
    }
    else
    {
        cbDebugPrint((MAKE_LEVEL(DP, DEBUG), "%s: Write DPCD 170h success!\n", FUNCTION_NAME));
    }

    DPCD_00170.Value = 0;
    DPCD_00170.ENABLE_PSR_IN_SINK = 1;  //enable sink psr function
    DPCD_00170.MAINLINK_ACTIVE = 0;
    DPCD_00170.CRC_VERIFICATION_ACTIVE = 0;
    DPCD_00170.FRAME_CAPTURE_INDICATION = 0;
    Data[0] = DPCD_00170.Value;

    if(!cbDIU_Aux_Dpcd_Write(pcbe, DPModuleIndex, DP_PSR_EN_CFG, Data, 1))
    {
        bStatus = CBIOS_FALSE;
        cbDebugPrint((MAKE_LEVEL(DP, DEBUG), "%s: Write DPCD 170h fail!\n", FUNCTION_NAME));
    }
    else
    {
        cbDebugPrint((MAKE_LEVEL(DP, DEBUG), "%s: Write DPCD 170h success!\n", FUNCTION_NAME));
    }

    DPCD_00600.SET_POWER = 1; // normal operation mode
    Data[0] = DPCD_00600.Value;

    if(!cbDIU_Aux_Dpcd_Write(pcbe, DPModuleIndex, DP_SET_POWER, Data, 1))
    {
        bStatus = CBIOS_FALSE;
        cbDebugPrint((MAKE_LEVEL(DP, DEBUG), "%s: Write DPCD 600h fail!\n", FUNCTION_NAME));
    }

    //enable source psr function
    DPSwingRegValue.Value = 0;
    //HW auto off main link sequence have some issue, but only samsung oled panel meet the issue,
    //patch for samsung oled panel first. If other panel also fail, I think we should set PSR_ML_AUTOOFF
    //to 0, and use sw to power off the maik link.
    if((pcbe->SVID == 0x17AA) && (pcbe->SSID == 0x350B))
    {
        DPSwingRegValue.PSR_ML_AUTOOFF= 0;
    }
    else
    {
        DPSwingRegValue.PSR_ML_AUTOOFF= 1;
    }
    DPSwingRegValue.PSR_FORCE_BS = 0;
    DPSwingRegMask.Value = 0xFFFFFFFF;
    DPSwingRegMask.PSR_ML_AUTOOFF = 0;
    DPSwingRegMask.PSR_FORCE_BS = 0;
    cbMMIOWriteReg32(pcbe, DP_REG_SWING[DPModuleIndex], DPSwingRegValue.Value, DPSwingRegMask.Value);

    EDPPsrCtl2RegValue.Value = 0;
    EDPPsrCtl2RegValue.VSC_HEAD = 0x82;
    EDPPsrCtl2RegValue.VSC_DB0 = 0;
    EDPPsrCtl2RegValue.VSC_COUNT = 0xa;
    EDPPsrCtl2RegMask.Value = 0xFFFFFFFF;
    EDPPsrCtl2RegMask.VSC_HEAD = 0;
    EDPPsrCtl2RegMask.VSC_DB0 = 0;
    EDPPsrCtl2RegMask.VSC_COUNT = 0;
    cbMMIOWriteReg32(pcbe, EDP_REG_PSR_CONTROL2[DPModuleIndex], EDPPsrCtl2RegValue.Value, EDPPsrCtl2RegMask.Value);

    EDPPsrCtl1RegValue.Value = 0;
    EDPPsrCtl1RegValue.PSR_EN = 1;
    EDPPsrCtl1RegValue.CRC_ENREG = 1;
    EDPPsrCtl1RegValue.PSR_ENTER_MODE = 0;//relate to psr setup time
    EDPPsrCtl1RegValue.PSR_CRC_MODE = 0;
    EDPPsrCtl1RegValue.PSR_UPDATE_MODE = 0;
    EDPPsrCtl1RegValue.SEL_SW_CRC = 0;
    EDPPsrCtl1RegValue.PSR_MAINLINK_ON = 0;
    EDPPsrCtl1RegMask.Value = 0xFFFFFFFF;
    EDPPsrCtl1RegMask.PSR_EN = 0;
    EDPPsrCtl1RegMask.CRC_ENREG = 0;
    EDPPsrCtl1RegMask.PSR_ENTER_MODE = 0;
    EDPPsrCtl1RegMask.PSR_CRC_MODE = 0;
    EDPPsrCtl1RegMask.PSR_UPDATE_MODE = 0;
    EDPPsrCtl1RegMask.SEL_SW_CRC = 0;
    EDPPsrCtl1RegMask.PSR_MAINLINK_ON = 0;
    cbMMIOWriteReg32(pcbe, EDP_REG_PSR_CONTROL1[DPModuleIndex], EDPPsrCtl1RegValue.Value, EDPPsrCtl1RegMask.Value);

    return bStatus;
}

CBIOS_VOID cbDIU_EDP_PsrOp1_0(PCBIOS_VOID pvcbe, CBIOS_MODULE_INDEX DPModuleIndex)
{
    PCBIOS_EXTENSION_COMMON pcbe          = (PCBIOS_EXTENSION_COMMON)pvcbe;
    REG_MM8368_E3K              DPSwingRegValue, DPSwingRegMask;
    REG_MM334BC_E3K              EDPPsrCtl1RegValue, EDPPsrCtl1RegMask;
    REG_MM334C4_E3K              EDPPsrCtl2RegValue, EDPPsrCtl2RegMask;
    CBIOS_U8   Data[2] = {0};
    DPCD_REG_00170 DPCD_00170 = {0};
    CBIOS_U32 Count = 0;
    CBIOS_BOOL bStatus = CBIOS_FALSE;

    if (DPModuleIndex >= DP_MODU_NUM)
    {
        cbDebugPrint((MAKE_LEVEL(DP, ERROR), "%s: invalid DP module index!\n", FUNCTION_NAME));
        return;
    }

    while(Count < 400)
    {
        bStatus = cbDIU_Psr_Wait_Inactive(pcbe, DPModuleIndex);

        if(bStatus == CBIOS_TRUE)
        {
            break;
        }

        cbDelayMilliSeconds(1);
        Count++;
    }

    //disable source psr function
    DPSwingRegValue.Value = 0;
    DPSwingRegValue.PSR_ML_AUTOOFF= 0;
    DPSwingRegValue.PSR_FORCE_BS = 0;
    DPSwingRegMask.Value = 0xFFFFFFFF;
    DPSwingRegMask.PSR_ML_AUTOOFF = 0;
    DPSwingRegMask.PSR_FORCE_BS = 0;
    cbMMIOWriteReg32(pcbe, DP_REG_SWING[DPModuleIndex], DPSwingRegValue.Value, DPSwingRegMask.Value);

    EDPPsrCtl2RegValue.Value = 0;
    EDPPsrCtl2RegValue.VSC_HEAD = 0x82;
    EDPPsrCtl2RegValue.VSC_DB0 = 0;
    EDPPsrCtl2RegValue.VSC_COUNT = 0xa;
    EDPPsrCtl2RegMask.Value = 0xFFFFFFFF;
    EDPPsrCtl2RegMask.VSC_HEAD = 0;
    EDPPsrCtl2RegMask.VSC_DB0 = 0;
    EDPPsrCtl2RegMask.VSC_COUNT = 0;
    cbMMIOWriteReg32(pcbe, EDP_REG_PSR_CONTROL2[DPModuleIndex], EDPPsrCtl2RegValue.Value, EDPPsrCtl2RegMask.Value);

    EDPPsrCtl1RegValue.Value = 0;
    EDPPsrCtl1RegValue.PSR_EN = 0;
    EDPPsrCtl1RegValue.CRC_ENREG = 0;
    EDPPsrCtl1RegValue.PSR_ENTER_MODE = 0;
    EDPPsrCtl1RegValue.PSR_CRC_MODE = 0;
    EDPPsrCtl1RegValue.PSR_UPDATE_MODE = 0;
    EDPPsrCtl1RegValue.SEL_SW_CRC = 0;
    EDPPsrCtl1RegValue.PSR_MAINLINK_ON = 0;
    EDPPsrCtl1RegMask.Value = 0xFFFFFFFF;
    EDPPsrCtl1RegMask.PSR_EN = 0;
    EDPPsrCtl1RegMask.CRC_ENREG = 0;
    EDPPsrCtl1RegMask.PSR_ENTER_MODE = 0;
    EDPPsrCtl1RegMask.PSR_CRC_MODE = 0;
    EDPPsrCtl1RegMask.PSR_UPDATE_MODE = 0;
    EDPPsrCtl1RegMask.SEL_SW_CRC = 0;
    EDPPsrCtl1RegMask.PSR_MAINLINK_ON = 0;
    cbMMIOWriteReg32(pcbe, EDP_REG_PSR_CONTROL1[DPModuleIndex], EDPPsrCtl1RegValue.Value, EDPPsrCtl1RegMask.Value);

    //disable sink psr function
    DPCD_00170.Value = 0;
    DPCD_00170.ENABLE_PSR_IN_SINK = 0;
    DPCD_00170.CRC_VERIFICATION_ACTIVE = 0;
    DPCD_00170.FRAME_CAPTURE_INDICATION = 0;
    DPCD_00170.MAINLINK_ACTIVE = 0;
    Data[0] = DPCD_00170.Value;

    if(!cbDIU_Aux_Dpcd_Write(pcbe, DPModuleIndex, DP_PSR_EN_CFG, Data, 1))
    {
        cbDebugPrint((MAKE_LEVEL(DP, DEBUG), "%s: Write DPCD 170h fail!\n", FUNCTION_NAME));
    }
    else
    {
        cbDebugPrint((MAKE_LEVEL(DP, DEBUG), "%s: Write DPCD 170h success!\n", FUNCTION_NAME));
    }
}

CBIOS_VOID cbDIU_EDP_PsrOp1_2(PCBIOS_VOID pvcbe, CBIOS_MODULE_INDEX DPModuleIndex)
{
    PCBIOS_EXTENSION_COMMON pcbe          = (PCBIOS_EXTENSION_COMMON)pvcbe;
    REG_MM8340_E3K     DPEphyMpllRegValue, DPEphyMpllRegMask;
    REG_MM8344_E3K     DPEphyTxRegValue, DPEphyTxRegMask;
    REG_MM8348_E3K     DPEphyMiscRegValue, DPEphyMiscRegMask;
    REG_MM8368_E3K     DPSwingRegValue, DPSwingRegMask;
    REG_MM334BC_E3K   EDPPsrCtl1RegValue, EDPPsrCtl1RegMask;
    CBIOS_U8  Data[2] = {0};
    DPCD_REG_00600 DPCD_00600 = {0};
    DPCD_REG_02008 DPCD_02008 = {0};

    if (DPModuleIndex >= DP_MODU_NUM)
    {
        cbDebugPrint((MAKE_LEVEL(DP, ERROR), "%s: invalid DP module index!\n", FUNCTION_NAME));
        return;
    }

    DPCD_00600.SET_POWER = 1; // normal operation mode
    Data[0] = DPCD_00600.Value;

    if(!cbDIU_Aux_Dpcd_Write(pcbe, DPModuleIndex, DP_SET_POWER, Data, 1))
    {
        cbDebugPrint((MAKE_LEVEL(DP, DEBUG), "%s: Write DPCD 600h fail!\n", FUNCTION_NAME));
    }

    EDPPsrCtl1RegValue.Value = 0;
    EDPPsrCtl1RegValue.PSR_MAINLINK_ON = 0;
    EDPPsrCtl1RegMask.Value = 0xFFFFFFFF;
    EDPPsrCtl1RegMask.PSR_MAINLINK_ON = 0;
    cbMMIOWriteReg32(pcbe, EDP_REG_PSR_CONTROL1[DPModuleIndex], EDPPsrCtl1RegValue.Value, EDPPsrCtl1RegMask.Value);

    DPSwingRegValue.Value = 0;
    DPSwingRegValue.PSR_EXIT = 0;
    DPSwingRegValue.PSR_ENTER = 1;
    DPSwingRegMask.Value = 0xFFFFFFFF;
    DPSwingRegMask.PSR_EXIT = 0;
    DPSwingRegMask.PSR_ENTER = 0;
    cbMMIOWriteReg32(pcbe, DP_REG_SWING[DPModuleIndex], DPSwingRegValue.Value, DPSwingRegMask.Value);
}
CBIOS_VOID cbDIU_EDP_PsrOp2_0(PCBIOS_VOID pvcbe, CBIOS_MODULE_INDEX DPModuleIndex)
{
    PCBIOS_EXTENSION_COMMON pcbe          = (PCBIOS_EXTENSION_COMMON)pvcbe;
    if (DPModuleIndex >= DP_MODU_NUM)
    {
        cbDebugPrint((MAKE_LEVEL(DP, ERROR), "%s: invalid DP module index!\n", FUNCTION_NAME));
        return;
    }

    cbDIU_EDP_PsrOp2_5(pcbe, DPModuleIndex);
    cbDIU_EDP_PsrOp5_0(pcbe, DPModuleIndex);
}
CBIOS_BOOL cbDIU_EDP_PsrOp2_3(PCBIOS_VOID pvcbe, CBIOS_MODULE_INDEX DPModuleIndex)
{
    PCBIOS_EXTENSION_COMMON pcbe          = (PCBIOS_EXTENSION_COMMON)pvcbe;
    CBIOS_BOOL bStatus = CBIOS_FALSE;
    CBIOS_U8 Data[2] = {0};
    DPCD_REG_02008 DPCD_02008 = {0};
    REG_MM8240_E3K     DPEnableRegValue, DPEnableRegMask;

    if (DPModuleIndex >= DP_MODU_NUM)
    {
        cbDebugPrint((MAKE_LEVEL(DP, ERROR), "%s: invalid DP module index!\n", FUNCTION_NAME));
        return  CBIOS_FALSE;
    }

    cbDelayMilliSeconds(1);

    DPCD_02008.Value = 0;

    if(!cbDIU_Aux_Dpcd_Read(pcbe, DPModuleIndex, DP_PSR_STATUS, Data, 1))
    {
        cbDebugPrint((MAKE_LEVEL(DP, DEBUG), "%s: Read DPCD 2008h fail!\n", FUNCTION_NAME));
    }
    else
    {
        if(Data[0] == DP_PSR_SINK_ACTIVE_RFB)
        {
            cbDebugPrint((MAKE_LEVEL(DP, DEBUG), "DP_PSR_SINK_ACTIVE_RFB(enter psr)\n"));

            if((pcbe->SVID == 0x17AA) && (pcbe->SSID == 0x350B))
            {
                DPEnableRegValue.Value = 0;
                DPEnableRegValue.Video_Enable = 0;  //idle pattern
                DPEnableRegMask.Value = 0xFFFFFFFF;
                DPEnableRegMask.Video_Enable = 0;
                cbMMIOWriteReg32(pcbe, DP_REG_ENABLE[DPModuleIndex], DPEnableRegValue.Value, DPEnableRegMask.Value);
            }

            bStatus = CBIOS_TRUE;
        }
    }

    return bStatus;
}
CBIOS_VOID cbDIU_EDP_PsrOp2_5(PCBIOS_VOID pvcbe, CBIOS_MODULE_INDEX DPModuleIndex)
{
    PCBIOS_EXTENSION_COMMON pcbe          = (PCBIOS_EXTENSION_COMMON)pvcbe;
    REG_MM8368_E3K     DPSwingRegValue, DPSwingRegMask;

    if (DPModuleIndex >= DP_MODU_NUM)
    {
        cbDebugPrint((MAKE_LEVEL(DP, ERROR), "%s: invalid DP module index!\n", FUNCTION_NAME));
        return;
    }

    DPSwingRegValue.Value = 0;
    DPSwingRegValue.PSR_EXIT = 1;
    DPSwingRegValue.PSR_ENTER = 0;
    DPSwingRegMask.Value = 0xFFFFFFFF;
    DPSwingRegMask.PSR_EXIT = 0;
    DPSwingRegMask.PSR_ENTER = 0;
    cbMMIOWriteReg32(pcbe, DP_REG_SWING[DPModuleIndex], DPSwingRegValue.Value, DPSwingRegMask.Value);
}
CBIOS_VOID cbDIU_EDP_PsrOp3_0(PCBIOS_VOID pvcbe, CBIOS_MODULE_INDEX DPModuleIndex)
{
    PCBIOS_EXTENSION_COMMON pcbe          = (PCBIOS_EXTENSION_COMMON)pvcbe;

    if (DPModuleIndex >= DP_MODU_NUM)
    {
        cbDebugPrint((MAKE_LEVEL(DP, ERROR), "%s: invalid DP module index!\n", FUNCTION_NAME));
        return;
    }

    cbDIU_EDP_PsrOp3_5(pcbe, DPModuleIndex);
    cbDIU_EDP_PsrOp5_0(pcbe, DPModuleIndex);
}
CBIOS_VOID cbDIU_EDP_PsrOp3_1(PCBIOS_VOID pvcbe, CBIOS_MODULE_INDEX DPModuleIndex)
{
    PCBIOS_EXTENSION_COMMON pcbe          = (PCBIOS_EXTENSION_COMMON)pvcbe;
    //sleep do nothing now
    if (DPModuleIndex >= DP_MODU_NUM)
    {
        cbDebugPrint((MAKE_LEVEL(DP, ERROR), "%s: invalid DP module index!\n", FUNCTION_NAME));
        return;
    }
}
CBIOS_VOID cbDIU_EDP_PsrOp3_5(PCBIOS_VOID pvcbe, CBIOS_MODULE_INDEX DPModuleIndex)
{
    PCBIOS_EXTENSION_COMMON pcbe          = (PCBIOS_EXTENSION_COMMON)pvcbe;
    CBIOS_U8       Data[2] = {0};
    DPCD_REG_00600 DPCD_00600 = {0};
    REG_MM8218_E3K     DPGenCtrlRegValue, DPGenCtrlRegMask;
    REG_MM8240_E3K     DPEnableRegValue, DPEnableRegMask;
    REG_MM8340_E3K     DPEphyMpllRegValue, DPEphyMpllRegMask;
    REG_MM8344_E3K     DPEphyTxRegValue, DPEphyTxRegMask;
    REG_MM8348_E3K     DPEphyMiscRegValue, DPEphyMiscRegMask;
    REG_MM8368_E3K     DPSwingRegValue, DPSwingRegMask;
    REG_MM334BC_E3K   EDPPsrCtl1RegValue, EDPPsrCtl1RegMask;
    CBIOS_U32               Device = CBIOS_TYPE_NONE;
    PCBIOS_DEVICE_COMMON    pDevCommon = CBIOS_NULL;
    PCBIOS_DP_MONITOR_CONTEXT   pDPMonitorContext = CBIOS_NULL;
    int cnt = 50;

    if (DPModuleIndex >= DP_MODU_NUM)
    {
        cbDebugPrint((MAKE_LEVEL(DP, ERROR), "%s: invalid DP module index!\n", FUNCTION_NAME));
        return;
    }

    Device = cbGetDeviceType(pcbe, DPModuleIndex, CBIOS_MODULE_TYPE_DP);

    if(Device == CBIOS_TYPE_NONE)
    {
        return;
    }

    pDevCommon = cbGetDeviceCommon(&pcbe->DeviceMgr, Device);
    pDPMonitorContext = cbGetDPMonitorContext(pcbe, pDevCommon);

    DPCD_00600.SET_POWER = 1; // normal operation mode
    Data[0] = DPCD_00600.Value;

    if(!cbDIU_Aux_Dpcd_Write(pcbe, DPModuleIndex, DP_SET_POWER, Data, 1))
    {
        cbDebugPrint((MAKE_LEVEL(DP, DEBUG), "%s: Write DPCD 600h fail!\n", FUNCTION_NAME));
    }

    DPSwingRegValue.Value = 0;
    DPSwingRegValue.PSR_FORCE_BS = 1;
    DPSwingRegMask.Value = 0xFFFFFFFF;
    DPSwingRegMask.PSR_FORCE_BS = 0;
    cbMMIOWriteReg32(pcbe, DP_REG_SWING[DPModuleIndex], DPSwingRegValue.Value, DPSwingRegMask.Value);

    DPEnableRegValue.Value = 0;
    DPEnableRegValue.Video_Enable = 0;  //idle pattern
    DPEnableRegMask.Value = 0xFFFFFFFF;
    DPEnableRegMask.Video_Enable = 0;
    cbMMIOWriteReg32(pcbe, DP_REG_ENABLE[DPModuleIndex], DPEnableRegValue.Value, DPEnableRegMask.Value);

    if(!pDPMonitorContext->bNoNeedLT)
    {
        cbDIU_DP_ResetLinkTraining(pcbe, DPModuleIndex);
    }

    EDPPsrCtl1RegValue.Value = 0;
    EDPPsrCtl1RegValue.PSR_MAINLINK_ON = 1;
    EDPPsrCtl1RegMask.Value = 0xFFFFFFFF;
    EDPPsrCtl1RegMask.PSR_MAINLINK_ON = 0;
    cbMMIOWriteReg32(pcbe, EDP_REG_PSR_CONTROL1[DPModuleIndex], EDPPsrCtl1RegValue.Value, EDPPsrCtl1RegMask.Value);

    if(!pDPMonitorContext->bNoNeedLT)
    {
        cbDPMonitor_LinkTrainingHw(pcbe, pDPMonitorContext, CBIOS_TRUE);
    }

    //at least 5 idle patterns
    do{
        DPGenCtrlRegValue.Value = cb_ReadU32(pcbe->pAdapterContext, DP_REG_GEN_CTRL[DPModuleIndex]);
        if(DPGenCtrlRegValue.Idle_Pattern_Counter > 5)
        {
            break;
        }
        cb_DelayMicroSeconds(100);
        cnt--;
    }while(cnt > 0);


    if ((!cbStrnCmp(pDevCommon->EdidStruct.Attribute.MonitorID, (CBIOS_UCHAR*)"CSO1444", 7)) &&
        (!cbStrnCmp(pDevCommon->EdidStruct.Attribute.Alphanumeric, (CBIOS_UCHAR*)"SNE007ZS2-2", 11)))
    {
        cb_DelayMicroSeconds(2000);
    }

    DPEnableRegValue.Value = 0;
    DPEnableRegValue.Video_Enable = 1;// valid video
    DPEnableRegMask.Value = 0xFFFFFFFF;
    DPEnableRegMask.Video_Enable = 0;
    cbMMIOWriteReg32(pcbe, DP_REG_ENABLE[DPModuleIndex], DPEnableRegValue.Value, DPEnableRegMask.Value);

    DPSwingRegValue.Value = 0;
    DPSwingRegValue.PSR_EXIT = 1;
    DPSwingRegValue.PSR_ENTER = 0;
    DPSwingRegMask.Value = 0xFFFFFFFF;
    DPSwingRegMask.PSR_EXIT = 0;
    DPSwingRegMask.PSR_ENTER = 0;
    cbMMIOWriteReg32(pcbe, DP_REG_SWING[DPModuleIndex], DPSwingRegValue.Value, DPSwingRegMask.Value);
}

CBIOS_VOID cbDIU_EDP_PsrOp5_0(PCBIOS_VOID pvcbe, CBIOS_MODULE_INDEX DPModuleIndex)
{
    PCBIOS_EXTENSION_COMMON pcbe          = (PCBIOS_EXTENSION_COMMON)pvcbe;

    if (DPModuleIndex >= DP_MODU_NUM)
    {
        cbDebugPrint((MAKE_LEVEL(DP, ERROR), "%s: invalid DP module index!\n", FUNCTION_NAME));
        return;
    }

    cbDIU_EDP_PsrOp1_0(pcbe, DPModuleIndex);
}
CBIOS_BOOL cbDIU_EDP_PsrOp5_1(PCBIOS_VOID pvcbe, CBIOS_MODULE_INDEX DPModuleIndex)
{
    PCBIOS_EXTENSION_COMMON pcbe          = (PCBIOS_EXTENSION_COMMON)pvcbe;
    CBIOS_U8 Data[1] = {0};
    DPCD_REG_02008 DPCD_02008={0};
    CBIOS_BOOL bStatus = CBIOS_FALSE;

    if (DPModuleIndex >= DP_MODU_NUM)
    {
        cbDebugPrint((MAKE_LEVEL(DP, ERROR), "%s: invalid DP module index!\n", FUNCTION_NAME));
        return  CBIOS_FALSE;
    }

    cb_DelayMicroSeconds(500);

    DPCD_02008.Value = 0;

    if(!cbDIU_Aux_Dpcd_Read(pcbe, DPModuleIndex, DP_PSR_STATUS, Data, 1))
    {
        cbDebugPrint((MAKE_LEVEL(DP, DEBUG), "%s: Read DPCD 2008h fail!\n", FUNCTION_NAME));
    }
    else
    {
        if(Data[0] == DP_PSR_SINK_ACTIVE_RESYNC)
        {
            cbDebugPrint((MAKE_LEVEL(DP, DEBUG), "DP_PSR_SINK_ACTIVE_RESYNC(exit psr)\n"));
            bStatus = CBIOS_TRUE;
        }
        else if(Data[0] == DP_PSR_SINK_INACTIVE)
        {
            cbDebugPrint((MAKE_LEVEL(DP, DEBUG), "DP_PSR_SINK_INACTIVE(exit psr)\n"));
            bStatus = CBIOS_TRUE;
        }
    }

    return bStatus;
}
//Add to disable PHY/IGA/DCLK
CBIOS_VOID cbDIU_EDP_PsrOp3_3_1(PCBIOS_VOID pvcbe, CBIOS_MODULE_INDEX DPModuleIndex)
{
    PCBIOS_EXTENSION_COMMON pcbe          = (PCBIOS_EXTENSION_COMMON)pvcbe;

    if (DPModuleIndex >= DP_MODU_NUM)
    {
        cbDebugPrint((MAKE_LEVEL(DP, ERROR), "%s: invalid DP module index!\n", FUNCTION_NAME));
        return;
    }

    cbPHY_DP_DeInitEPHY(pcbe, DPModuleIndex);
}
CBIOS_VOID cbDIU_EDP_PsrOp3_1_3_2(PCBIOS_VOID pvcbe, CBIOS_MODULE_INDEX DPModuleIndex)
{
    PCBIOS_EXTENSION_COMMON pcbe          = (PCBIOS_EXTENSION_COMMON)pvcbe;

    if (DPModuleIndex >= DP_MODU_NUM)
    {
        cbDebugPrint((MAKE_LEVEL(DP, ERROR), "%s: invalid DP module index!\n", FUNCTION_NAME));
        return;
    }

    if(DPModuleIndex == CBIOS_MODULE_INDEX1)
    {
        cbMMIOWriteReg32(pcbe, 0x8180, 0x1, ~0x1);
    }
    else if(DPModuleIndex == CBIOS_MODULE_INDEX2)
    {
        cbMMIOWriteReg32(pcbe, 0x33900, 0x1, ~0x1);
    }
    else if(DPModuleIndex == CBIOS_MODULE_INDEX3)
    {
        cbMMIOWriteReg32(pcbe, 0x34000, 0x1, ~0x1);
    }
    cbWaitVSync(pcbe, DPModuleIndex);
}
CBIOS_VOID cbDIU_EDP_PsrOp3_2_3_3(PCBIOS_VOID pvcbe, CBIOS_MODULE_INDEX DPModuleIndex)
{
    PCBIOS_EXTENSION_COMMON pcbe          = (PCBIOS_EXTENSION_COMMON)pvcbe;

    if (DPModuleIndex >= DP_MODU_NUM)
    {
        cbDebugPrint((MAKE_LEVEL(DP, ERROR), "%s: invalid DP module index!\n", FUNCTION_NAME));
        return;
    }

    cbHWSetIgaOnOffState(pcbe, CBIOS_FALSE, DPModuleIndex);
}
CBIOS_VOID cbDIU_EDP_PsrOp3_3_3_2(PCBIOS_VOID pvcbe, CBIOS_MODULE_INDEX DPModuleIndex)
{
    PCBIOS_EXTENSION_COMMON pcbe          = (PCBIOS_EXTENSION_COMMON)pvcbe;

    if (DPModuleIndex >= DP_MODU_NUM)
    {
        cbDebugPrint((MAKE_LEVEL(DP, ERROR), "%s: invalid DP module index!\n", FUNCTION_NAME));
        return;
    }

    cbHWSetIgaOnOffState(pcbe, CBIOS_TRUE, DPModuleIndex);
}
CBIOS_VOID cbDIU_EDP_PsrOp3_2_3_1(PCBIOS_VOID pvcbe, CBIOS_MODULE_INDEX DPModuleIndex)
{
    PCBIOS_EXTENSION_COMMON pcbe          = (PCBIOS_EXTENSION_COMMON)pvcbe;

    if (DPModuleIndex >= DP_MODU_NUM)
    {
        cbDebugPrint((MAKE_LEVEL(DP, ERROR), "%s: invalid DP module index!\n", FUNCTION_NAME));
        return;
    }

    if(DPModuleIndex == CBIOS_MODULE_INDEX1)
    {
        cbMMIOWriteReg32(pcbe, 0x8180, 0x0, ~0x1);
    }
    else if(DPModuleIndex == CBIOS_MODULE_INDEX2)
    {
        cbMMIOWriteReg32(pcbe, 0x33900, 0x0, ~0x1);
    }
    else if(DPModuleIndex == CBIOS_MODULE_INDEX3)
    {
        cbMMIOWriteReg32(pcbe, 0x34000, 0x0, ~0x1);
    }
}
CBIOS_VOID cbDIU_EDP_PsrOp3_1_3(PCBIOS_VOID pvcbe, CBIOS_MODULE_INDEX DPModuleIndex)
{
    PCBIOS_EXTENSION_COMMON pcbe          = (PCBIOS_EXTENSION_COMMON)pvcbe;
    CBIOS_U32               Device = CBIOS_TYPE_NONE;
    PCBIOS_DEVICE_COMMON    pDevCommon = CBIOS_NULL;
    PCBIOS_DP_CONTEXT       pDpContext = CBIOS_NULL;

    if (DPModuleIndex >= DP_MODU_NUM)
    {
        cbDebugPrint((MAKE_LEVEL(DP, ERROR), "%s: invalid DP module index!\n", FUNCTION_NAME));
        return;
    }

    Device = cbGetDeviceType(pcbe, DPModuleIndex, CBIOS_MODULE_TYPE_DP);

    if(Device == CBIOS_TYPE_NONE)
    {
        return;
    }

    pDevCommon = cbGetDeviceCommon(&pcbe->DeviceMgr, Device);
    pDpContext = container_of(pDevCommon, PCBIOS_DP_CONTEXT, Common);

    cbPHY_DP_InitEPHY(pcbe, DPModuleIndex);
    cbPHY_DP_TxDriverInit(pcbe, DPModuleIndex, CBIOS_FALSE, pDpContext->DPMonitorContext.LinkPassLaneNum);
}

CBIOS_BOOL cbDIU_Psr_Wait_Inactive(PCBIOS_VOID pvcbe, CBIOS_MODULE_INDEX DPModuleIndex)
{
    PCBIOS_EXTENSION_COMMON pcbe          = (PCBIOS_EXTENSION_COMMON)pvcbe;
    CBIOS_U8  Data[1] = {0};
    DPCD_REG_02008 DPCD_02008={0};
    CBIOS_BOOL bStatus = CBIOS_FALSE;

    if (DPModuleIndex >= DP_MODU_NUM)
    {
        cbDebugPrint((MAKE_LEVEL(DP, ERROR), "%s: invalid DP module index!\n", FUNCTION_NAME));
        return  CBIOS_FALSE;
    }

    DPCD_02008.Value = 0;

    if(!cbDIU_Aux_Dpcd_Read(pcbe, DPModuleIndex, DP_PSR_STATUS, Data, 1))
    {
        cbDebugPrint((MAKE_LEVEL(DP, DEBUG), "%s: Read DPCD 2008h fail!\n", FUNCTION_NAME));
    }
    else
    {
        if(Data[0] == DP_PSR_SINK_INACTIVE)
        {
            cbDebugPrint((MAKE_LEVEL(DP, DEBUG), "DP_PSR_SINK_INACTIVE(exit psr)\n"));
            bStatus = CBIOS_TRUE;
        }
    }

    return bStatus;
}

CBIOS_STATUS cbDIU_Aux_I2c_Access(PCBIOS_VOID pvcbe, CBIOS_MODULE_INDEX DPModuleIndex, CBIOS_U8 Request, PCBIOS_PARAM_AUX_DATA pCBParamAuxData)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    CBIOS_STATUS Status = CBIOS_ER_INVALID_PARAMETER;
    CBIOS_U32    TransferLen = pCBParamAuxData->BytesRequested;
    CBIOS_U8*    Buffer = pCBParamAuxData->Buffer;
    CBIOS_U8     Offset = pCBParamAuxData->Offset;
    CBIOS_U32    TransferSize, i;
    CBIOS_VOID   *pBusLock = CBIOS_NULL;
    AUX_CONTROL AUX = {0};
    CBIOS_BOOL  bMerge = CBIOS_FALSE;

    pBusLock = cbGetDeviceBusLock(pvcbe, DPModuleIndex, CBIOS_MODULE_TYPE_DP);
    cb_AcquireLock(pBusLock , CBIOS_OS_MUTEX_LOCK);

    AUX.SlaveAddress = (CBIOS_U8)pCBParamAuxData->Address;

    if(pCBParamAuxData->I2cStop && TransferLen <= AUX_MAX_PAYLOAD_BYTES)
    {
        bMerge = CBIOS_TRUE;
    }

    if(!bMerge && (!pCBParamAuxData->IsDDCCI || Request == CBIOS_AUX_REQUEST_I2C_WRITE))
    {
        //AUX write, 0 byte, address transaction only
        AUX.Function = CBIOS_AUX_REQUEST_I2C_WRITE | CBIOS_AUX_REQUEST_I2C_MOT;
        AUX.Buffer = CBIOS_NULL;
        AUX.Length = 0;
        AUX.Offset = 0;
        if(!cbDIU_DP_Auxtransfer(pcbe, DPModuleIndex, &AUX))
        {
            goto ExitI2cAccess;
        }
    }
    //AUX write, 1 byte, set offset
    if(!bMerge && (!pCBParamAuxData->IsDDCCI) && (!pCBParamAuxData->EDDCMode))
    {
        AUX.Buffer = &Offset;
        AUX.Length = 1;
        if(!cbDIU_DP_Auxtransfer(pcbe, DPModuleIndex, &AUX))
        {
            goto ExitI2cAccess;
        }
    }
    //AUX I2C write/read
    if(!bMerge)
    {
        AUX.Function = Request | CBIOS_AUX_REQUEST_I2C_MOT;
    }
    else
    {
        AUX.Function = Request;
    }
    
    for(i = 0; i < TransferLen; i += TransferSize)
    {
        TransferSize = cb_min(AUX_MAX_PAYLOAD_BYTES, TransferLen - i);
        AUX.Buffer = Buffer + i;
        AUX.Length = TransferSize;
        if(!cbDIU_DP_Auxtransfer(pcbe, DPModuleIndex, &AUX))
        {
            goto ExitI2cAccess;
        }
    }

    //AUX stop,AUX write/read, 0 byte, without MOT => I2C stop
    if((pCBParamAuxData->Address == 0x60) && (Request == CBIOS_AUX_REQUEST_I2C_WRITE))//write segment to 0x60, no need to stop
    {
        Status = CBIOS_OK;
        goto ExitI2cAccess;
    }
    else if(!bMerge)
    {
        AUX.Function = Request;
        AUX.Buffer = CBIOS_NULL;
        AUX.Length = 0;
        AUX.Offset = 0;
        if(!cbDIU_DP_Auxtransfer(pcbe, DPModuleIndex, &AUX))
        {
            goto ExitI2cAccess;
        }
    }

    Status = CBIOS_OK;

ExitI2cAccess:
    if(Status == CBIOS_ER_INVALID_PARAMETER)
    {
        cbDebugPrint((MAKE_LEVEL(GENERIC, WARNING), "%s: Aux access fail  for DP index:%d!\n", FUNCTION_NAME, DPModuleIndex));
    }

    cb_ReleaseLock(pBusLock, CBIOS_OS_MUTEX_LOCK, 0);

    return Status;

}

CBIOS_BOOL cbDIU_Aux_Dpcd_Access(PCBIOS_VOID pvcbe, CBIOS_MODULE_INDEX DPModuleIndex, CBIOS_U8 Request, CBIOS_U32 Offset, CBIOS_UCHAR* Buffer, CBIOS_U32 Length)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    CBIOS_BOOL Status = CBIOS_FALSE;
    CBIOS_VOID  *pBusLock = CBIOS_NULL;
    AUX_CONTROL AUX = {0};

    pBusLock = cbGetDeviceBusLock(pvcbe, DPModuleIndex, CBIOS_MODULE_TYPE_DP);
    cb_AcquireLock(pBusLock , CBIOS_OS_MUTEX_LOCK);

    AUX.Function = Request;
    AUX.Offset = Offset;
    AUX.Buffer = Buffer;
    AUX.Length = Length;

    Status = cbDIU_DP_Auxtransfer(pcbe, DPModuleIndex, &AUX);

    cb_ReleaseLock(pBusLock, CBIOS_OS_MUTEX_LOCK, 0);

    return Status;
}

CBIOS_BOOL cbDIU_Aux_Dpcd_Read(PCBIOS_VOID pvcbe, CBIOS_MODULE_INDEX DPModuleIndex, CBIOS_U32 Offset, CBIOS_UCHAR* Buffer, CBIOS_U32 Length)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    CBIOS_BOOL  bRet = CBIOS_FALSE;

    bRet = cbDIU_Aux_Dpcd_Access(pcbe, DPModuleIndex, CBIOS_AUX_REQUEST_NATIVE_READ, Offset, Buffer, Length);

    return bRet;
}

CBIOS_BOOL cbDIU_Aux_Dpcd_Write(PCBIOS_VOID pvcbe, CBIOS_MODULE_INDEX DPModuleIndex, CBIOS_U32 Offset, CBIOS_UCHAR* Buffer, CBIOS_U32 Length)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    CBIOS_BOOL  bRet = CBIOS_FALSE;

    bRet = cbDIU_Aux_Dpcd_Access(pcbe, DPModuleIndex, CBIOS_AUX_REQUEST_NATIVE_WRITE, Offset, Buffer, Length);

    if (!bRet)
    {
        cbPHY_DP_AuxPowerOnOff(pcbe, DPModuleIndex, CBIOS_FALSE);
    }
    return bRet;
}

CBIOS_BOOL cbDIU_Aux_Inner_Write(PCBIOS_VOID pvcbe, CBIOS_MODULE_INDEX DPModuleIndex, CBIOS_U32 Offset, CBIOS_UCHAR* Buffer, CBIOS_U32 Length)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    CBIOS_BOOL  bRet = CBIOS_FALSE;

    bRet = cbDIU_Aux_Dpcd_Access(pcbe, DPModuleIndex, CBIOS_AUX_REQUEST_INNER_WRITE, Offset, Buffer, Length);
    return bRet;
}

CBIOS_STATUS cbDIU_EDP_SetBacklight(PCBIOS_VOID pvcbe, CBIOS_MODULE_INDEX DPModuleIndex, CBIOS_U32 BacklightValue)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    CBIOS_STATUS Status = CBIOS_OK;
    CBIOS_U32               Device = CBIOS_TYPE_NONE;
    PCBIOS_DEVICE_COMMON    pDevCommon = CBIOS_NULL;
    PCBIOS_DP_CONTEXT       pDpContext = CBIOS_NULL;

    if (DPModuleIndex >= DP_MODU_NUM)
    {
        cbDebugPrint((MAKE_LEVEL(DP, ERROR), "%s: invalid DP module index!\n", FUNCTION_NAME));
        return CBIOS_ER_INTERNAL;
    }

    Device = cbGetDeviceType(pcbe, DPModuleIndex, CBIOS_MODULE_TYPE_DP);

    if(Device == CBIOS_TYPE_NONE)
    {
        return CBIOS_ER_INTERNAL;
    }

    pDevCommon = cbGetDeviceCommon(&pcbe->DeviceMgr, Device);
    pDpContext = container_of(pDevCommon, PCBIOS_DP_CONTEXT, Common);

    if (!pDpContext->DPMonitorContext.bSupportAuxBl || pcbe->SysBiosInfo.bBLGfxPwm)
    {
        Status = cbDIU_EDP_GPIOSetBacklight(pcbe, BacklightValue, pcbe->SysBiosInfo.bInvertPwmBL ? CBIOS_TRUE : CBIOS_FALSE);
    }
    else
    {
        if(!pDpContext->DPMonitorContext.bAuxBlEnabled)
        {
            if(cbDIU_EDP_AUXEnableBacklight(pcbe, DPModuleIndex))
            {
                pDpContext->DPMonitorContext.bAuxBlEnabled = CBIOS_TRUE;
            }
            else
            {
                Status = CBIOS_ER_INTERNAL;
            }
        }

        if(pDpContext->DPMonitorContext.bAuxBlEnabled)
        {
            Status = cbDIU_EDP_AUXSetBacklight(pcbe, &pDpContext->DPMonitorContext, DPModuleIndex, BacklightValue);
        }
    }

    return Status;
}

CBIOS_STATUS cbDIU_EDP_GetBacklight(PCBIOS_VOID pvcbe, CBIOS_MODULE_INDEX DPModuleIndex, CBIOS_U32 *pBacklightValue)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    CBIOS_STATUS  Status = CBIOS_OK;
    CBIOS_U32               Device = CBIOS_TYPE_NONE;
    PCBIOS_DEVICE_COMMON    pDevCommon = CBIOS_NULL;
    PCBIOS_DP_CONTEXT       pDpContext = CBIOS_NULL;

    if (DPModuleIndex >= DP_MODU_NUM)
    {
        cbDebugPrint((MAKE_LEVEL(DP, ERROR), "%s: invalid DP module index!\n", FUNCTION_NAME));
        return CBIOS_ER_INTERNAL;
    }

    Device = cbGetDeviceType(pcbe, DPModuleIndex, CBIOS_MODULE_TYPE_DP);

    if(Device == CBIOS_TYPE_NONE)
    {
        return CBIOS_ER_INTERNAL;
    }

    pDevCommon = cbGetDeviceCommon(&pcbe->DeviceMgr, Device);
    pDpContext = container_of(pDevCommon, PCBIOS_DP_CONTEXT, Common);

    if (!pDpContext->DPMonitorContext.bSupportAuxBl || pcbe->SysBiosInfo.bBLGfxPwm)
    {
        Status = cbDIU_EDP_GPIOGetBacklight(pcbe, pBacklightValue, pcbe->SysBiosInfo.bInvertPwmBL ? CBIOS_TRUE : CBIOS_FALSE);
    }
    else
    {
        Status = cbDIU_EDP_AUXGetBacklight(pcbe, &pDpContext->DPMonitorContext, DPModuleIndex, pBacklightValue);
    }

    return Status;
}

CBIOS_STATUS cbDIU_EDP_GPIOSetBacklight(PCBIOS_VOID pvcbe, CBIOS_U32 BacklightValue, CBIOS_BOOL bInvertValue)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    CBIOS_STATUS Status = CBIOS_OK;
    CBIOS_U32 BlValue = 0, pwm_frequency_counter = 0, max_backlight_value = 0, min_backlight_value = 0;
    REG_MM333D8 RegMM333D8_Value, RegMM333D8_Mask;
    REG_MM333DC RegMM333DC_Value, RegMM333DC_Mask;

    BlValue = BacklightValue;
    pwm_frequency_counter = 0xFFFF;
    max_backlight_value = 0xFF;

    if(BlValue > max_backlight_value)
    {
        BlValue = max_backlight_value;
    }

    if(BlValue < min_backlight_value)
    {
        BlValue = min_backlight_value;
    }

    if (bInvertValue)
    {
        BlValue = max_backlight_value - BlValue + min_backlight_value;
    }

    /**
    REG_PWM_CTRL = 2'b11, high duration = (backlight_value/2    )*(1/27MHz), duty cycle = (backlight_value/2    )/(pwm_frequency_counter + 1)
    REG_PWM_CTRL = 2'b10, high duration = (backlight_value        )*(1/27MHz), duty cycle = (backlight_value       )/(pwm_frequency_counter + 1)
    REG_PWM_CTRL = 2'b01, high duration = (backlight_value*2    )*(1/27MHz), duty cycle= (backlight_value*2     )/(pwm_frequency_counter + 1)
    REG_PWM_CTRL = 2'b00, high duration = (backlight_value*4+1)*(1/27MHz), duty ctcle = (backlight_value*4+1)/(pwm_frequency_counter + 1)
    **/
    RegMM333D8_Value.backlight_value = BlValue * (pwm_frequency_counter / max_backlight_value);
    RegMM333D8_Value.PWM_frequency_counter = pwm_frequency_counter - 1;
    RegMM333D8_Mask.Value = 0xFFFFFFFF;
    RegMM333D8_Mask.backlight_value = 0;
    RegMM333D8_Mask.PWM_frequency_counter = 0;

    cbMMIOWriteReg32(pcbe, 0x333D8, RegMM333D8_Value.Value, RegMM333D8_Mask.Value);

    RegMM333DC_Value.REG_PWM_CTRL = 0x2;
    RegMM333DC_Value.PWM_frequency_counter = 0;
    RegMM333DC_Mask.Value = 0xFFFFFFFF;
    RegMM333DC_Mask.REG_PWM_CTRL = 0;
    RegMM333DC_Mask.PWM_frequency_counter = 0;

    cbMMIOWriteReg32(pcbe, 0x333DC, RegMM333DC_Value.Value, RegMM333DC_Mask.Value);

    RegMM333DC_Value.Value = cb_ReadU32(pcbe->pAdapterContext, 0x333DC);
    if((!RegMM333DC_Value.REG_PWM_EN) || (!RegMM333DC_Value.DIU_DIO_PWM_oen))
    {
        RegMM333DC_Value.REG_PWM_EN = 1;
        RegMM333DC_Value.DIU_DIO_PWM_oen = 1;
        RegMM333DC_Mask.Value = 0xFFFFFFFF;
        RegMM333DC_Mask.REG_PWM_EN = 0;
        RegMM333DC_Mask.DIU_DIO_PWM_oen = 0;

        cbMMIOWriteReg32(pcbe, 0x333DC, RegMM333DC_Value.Value, RegMM333DC_Mask.Value);
    }

    return Status;
}

CBIOS_STATUS cbDIU_EDP_GPIOGetBacklight(PCBIOS_VOID pvcbe, CBIOS_U32 *pBacklightValue, CBIOS_BOOL bInvertValue)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    CBIOS_STATUS Status = CBIOS_OK;
    REG_MM333D8 RegMM333D8_Value, RegMM333D8_Mask;
    CBIOS_U32 BlValue = 0, pwm_frequency_counter = 0, max_backlight_value = 0, min_backlight_value = 0;

    RegMM333D8_Value.Value = cb_ReadU32(pcbe->pAdapterContext, 0x333D8);

    max_backlight_value =  0xFF;
    BlValue = RegMM333D8_Value.backlight_value;
    pwm_frequency_counter = (RegMM333D8_Value.PWM_frequency_counter == 0) ? 0xFFFF : RegMM333D8_Value.PWM_frequency_counter + 1;

    *pBacklightValue = (BlValue * max_backlight_value) / pwm_frequency_counter;

    if (bInvertValue)
    {
        *pBacklightValue = max_backlight_value - *pBacklightValue + min_backlight_value;
    }

    return Status;
}

CBIOS_STATUS cbDIU_EDP_AUXSetBacklight(PCBIOS_VOID pvcbe, PCBIOS_DP_MONITOR_CONTEXT pDPMonitorContext, CBIOS_MODULE_INDEX DPModuleIndex, CBIOS_U32 BacklightValue)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    CBIOS_STATUS         Status = CBIOS_OK;
    CBIOS_U8            Data[2] = {0};
    DPCD_REG_00722      DPCD_00722 = {0};
    DPCD_REG_00723      DPCD_00723 = {0};
    CBIOS_U8 BacklightMode,DpcdValue, NewDpcdValue;
    CBIOS_U16  BacklightMask = 0xFFFF;
    CBIOS_U8    BitCount;

    BitCount = cbDIU_EDP_AUXGetBacklightBitCount(pcbe, pDPMonitorContext);
    BacklightMask = (1 << BitCount) - 1;
    if((pcbe->SVID == 0x17AA) && (pcbe->SSID == 0x350B) && (BacklightValue != 0xFF))//map the backlight to backlight*0.8 except max backlight
    {
        BacklightValue = cbRound(BacklightValue * BacklightMask* 0xCC, 0xFF * 0xFF, ROUND_DOWN);
    }
    else
    {
        BacklightValue = cbRound(BacklightValue * BacklightMask, 0xFF, ROUND_DOWN);
    }

    if(pDPMonitorContext->EDPBlBitAligment == 1)
    {
        if(pDPMonitorContext->EDPBlByteCount)
        {
            BacklightValue = (BacklightValue << (16 - BitCount)) | (BacklightValue >> BitCount);
        }
        else
        {
            BacklightValue = BacklightValue & 0xFF;
            BacklightValue = (BacklightValue << (8 - BitCount)) | (BacklightValue >> BitCount);
        }
    }

    //set aux backlight
    if(pDPMonitorContext->EDPBlByteCount)
    {
        DPCD_00722.Value = (BacklightValue & 0xFF00) >> 8;
        DPCD_00723.Value = BacklightValue & 0xFF;
    }
    else
    {
        DPCD_00722.Value = BacklightValue & 0xFF;
        DPCD_00723.Value = 0x00;
    }

    Data[0] = DPCD_00722.Value;
    Data[1] = DPCD_00723.Value;

    if(!cbDIU_Aux_Dpcd_Write(pcbe, DPModuleIndex, DP_EDP_BACKLIGHT_BRIGHTNESS_MSB, Data, 2))
    {
        cbDebugPrint((MAKE_LEVEL(DP, DEBUG), "%s: Failed to wite dpcd 722h\n", FUNCTION_NAME));
        Status = CBIOS_ER_INTERNAL;
    }

    return Status;
}

CBIOS_STATUS cbDIU_EDP_AUXGetBacklight(PCBIOS_VOID pvcbe, PCBIOS_DP_MONITOR_CONTEXT pDPMonitorContext, CBIOS_MODULE_INDEX DPModuleIndex, CBIOS_U32 *pBacklightValue)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    CBIOS_STATUS        Status = CBIOS_OK;
    CBIOS_U8    Data[2]= {0};
    DPCD_REG_00722      DPCD_00722 = {0};
    DPCD_REG_00723      DPCD_00723 = {0};
    CBIOS_U16  BacklightMask = 0xFFFF;
    CBIOS_U8    BitCount;

    if(cbDIU_Aux_Dpcd_Read(pcbe, DPModuleIndex, DP_EDP_BACKLIGHT_BRIGHTNESS_MSB, Data, 2))
    {
        DPCD_00722.Value = Data[0];
        DPCD_00723.Value = Data[1];

        if(pDPMonitorContext->EDPBlByteCount)
        {
            *pBacklightValue = DPCD_00722.Value << 8 | DPCD_00723.Value;
        }
        else
        {
            *pBacklightValue = DPCD_00722.Value;
        }
    }
    else
    {
        Status = CBIOS_ER_INTERNAL;
    }

    BitCount = cbDIU_EDP_AUXGetBacklightBitCount(pcbe, pDPMonitorContext);
    BacklightMask = (1 << BitCount) - 1;

    if(pDPMonitorContext->EDPBlBitAligment == 1)
    {
        if(pDPMonitorContext->EDPBlByteCount)
        {
            *pBacklightValue = (*pBacklightValue >> (16 - BitCount)) | (*pBacklightValue << BitCount);
        }
        else
        {
            *pBacklightValue = (*pBacklightValue >> (8 - BitCount)) | (*pBacklightValue << BitCount);
        }
    }

    if((pcbe->SVID == 0x17AA) && (pcbe->SSID == 0x350B)
        && (cbRound(0xFF * (*pBacklightValue), BacklightMask, ROUND_UP) != 0xFF))//map the backlight to backlight/0.8 except max backlight
    {
        *pBacklightValue = cbRound(0xFF * 0xFF * (*pBacklightValue), BacklightMask * 0xCC, ROUND_UP);
    }
    else
    {
        *pBacklightValue = cbRound(0xFF * (*pBacklightValue), BacklightMask, ROUND_UP);
    }

    return Status;
}

CBIOS_BOOL cbDIU_EDP_AUXEnableBacklight(PCBIOS_VOID pvcbe, CBIOS_MODULE_INDEX DPModuleIndex)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    CBIOS_BOOL            bStatus = CBIOS_TRUE;
    CBIOS_U8    Data[1] = {0};
    DPCD_REG_00720      DPCD_00720 = {0};
    CBIOS_U8 BacklightMode,DpcdValue, NewDpcdValue;

    //set backlight mode
    if(cbDIU_Aux_Dpcd_Read(pcbe, DPModuleIndex, DP_EDP_BACKLIGHT_MODE_SET_REGISTER, Data, 1))
    {
        DpcdValue = Data[0];
        NewDpcdValue = DpcdValue;
        BacklightMode = DpcdValue & DP_EDP_BACKLIGHT_CONTROL_MODE_MASK;

        switch(BacklightMode)
        {
            case DP_EDP_BACKLIGHT_CONTROL_MODE_PWM:
            case DP_EDP_BACKLIGHT_CONTROL_MODE_PRESET:
            case DP_EDP_BACKLIGHT_CONTROL_MODE_PRODUCT:
                NewDpcdValue &= ~DP_EDP_BACKLIGHT_CONTROL_MODE_MASK;
                NewDpcdValue |= DP_EDP_BACKLIGHT_CONTROL_MODE_DPCD;
                break;
            case DP_EDP_BACKLIGHT_CONTROL_MODE_DPCD:
            default:
                break;
        }
    }
    else
    {
        return CBIOS_FALSE;
    }

    if(DpcdValue != NewDpcdValue)
    {
        Data[0] = NewDpcdValue;
        if(!cbDIU_Aux_Dpcd_Write(pcbe, DPModuleIndex, DP_EDP_BACKLIGHT_MODE_SET_REGISTER, Data, 1))
        {
            cbDebugPrint((MAKE_LEVEL(DP, DEBUG), "%s: Failed to wite dpcd 721h\n", FUNCTION_NAME));
        }
    }

    //enable aux set backlight
    if(cbDIU_Aux_Dpcd_Read(pcbe, DPModuleIndex, DP_EDP_DISPLAY_CONTROL_REGISTER, Data, 1))
    {
        DPCD_00720.Value = Data[0];
        DPCD_00720.Value |= DP_EDP_BACKLIGHT_ENABLE;

        Data[0] = DPCD_00720.Value;
        if(!cbDIU_Aux_Dpcd_Write(pcbe, DPModuleIndex, DP_EDP_DISPLAY_CONTROL_REGISTER, Data, 1))
        {
            cbDebugPrint((MAKE_LEVEL(DP, DEBUG), "%s: Failed to wite dpcd 720h\n", FUNCTION_NAME));
            bStatus = CBIOS_FALSE;
        }
    }
    else
    {
        bStatus = CBIOS_FALSE;
    }

    return bStatus;
}

CBIOS_BOOL cbDIU_EDP_AUXDisableBacklight(PCBIOS_VOID pvcbe, CBIOS_MODULE_INDEX DPModuleIndex)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    CBIOS_BOOL            bStatus = CBIOS_TRUE;
    CBIOS_U8    Data[1] = {0};
    DPCD_REG_00720      DPCD_00720 = {0};
    CBIOS_U8 BacklightMode,DpcdValue, NewDpcdValue;

    //disable aux set backlight
    if(cbDIU_Aux_Dpcd_Read(pcbe, DPModuleIndex, DP_EDP_DISPLAY_CONTROL_REGISTER, Data, 1))
    {
        DPCD_00720.Value = Data[0];
        DPCD_00720.Value &= ~DP_EDP_BACKLIGHT_ENABLE;

        Data[0] = DPCD_00720.Value;
        if(!cbDIU_Aux_Dpcd_Write(pcbe, DPModuleIndex, DP_EDP_DISPLAY_CONTROL_REGISTER, Data, 1))
        {
            cbDebugPrint((MAKE_LEVEL(DP, DEBUG), "%s: Failed to wite dpcd 720h\n", FUNCTION_NAME));
        }
    }
    else
    {
        bStatus = CBIOS_FALSE;
    }

    return bStatus;
}

CBIOS_BOOL cbDIU_EDP_AUXSetPwmFreq(PCBIOS_VOID pvcbe, CBIOS_MODULE_INDEX DPModuleIndex)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    return CBIOS_TRUE;
}

CBIOS_U8 cbDIU_EDP_AUXGetBacklightBitCount(PCBIOS_VOID pvcbe, PCBIOS_DP_MONITOR_CONTEXT pDPMonitorContext)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    CBIOS_U8 BitCount;

    if(pDPMonitorContext->EDPBlByteCount)
    {
        BitCount = 16;
    }
    else
    {
        BitCount = 8;
    }

    if(pDPMonitorContext->EDPBlPwmBitCountMax < pDPMonitorContext->EDPBlPwmBitCountMin
        || pDPMonitorContext->EDPBlPwmBitCountMax == 0
        || pDPMonitorContext->EDPBlPwmBitCountMin == 0)
    {
        return BitCount;
    }

    if(pDPMonitorContext->EDPBlPwmBitCount > pDPMonitorContext->EDPBlPwmBitCountMax)
    {
        pDPMonitorContext->EDPBlPwmBitCount = pDPMonitorContext->EDPBlPwmBitCountMax;
    }

    if(pDPMonitorContext->EDPBlPwmBitCount < pDPMonitorContext->EDPBlPwmBitCountMin)
    {
        pDPMonitorContext->EDPBlPwmBitCount = pDPMonitorContext->EDPBlPwmBitCountMin;
    }

    BitCount = pDPMonitorContext->EDPBlPwmBitCount;

    if(pDPMonitorContext->EDPBlByteCount)
    {
        if(BitCount > 16)
        {
            BitCount = 16;
        }
    }
    else
    {
        if(BitCount > 8)
        {
            BitCount = 8;
        }
    }

    return BitCount;
}

