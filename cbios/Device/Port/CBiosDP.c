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
** DP port interface function implementation.
**
** NOTE:
** 
******************************************************************************/

#include "CBiosChipShare.h"
#include "CBiosDP.h"
#include "../../Hw/HwBlock/CBiosDIU_DP.h"
#include "../../Hw/HwBlock/CBiosDIU_HDCP.h"
#include "../../Hw/HwBlock/CBiosDIU_HDMI.h"
#include "../../Hw/CBiosHwShare.h"

PCBIOS_VOID cbDPPort_GetDPMonitorContext(PCBIOS_VOID pvcbe, PCBIOS_DEVICE_COMMON pDevCommon)
{
    PCBIOS_DP_CONTEXT  pDpContext = container_of(pDevCommon, PCBIOS_DP_CONTEXT, Common);

    if ((pDevCommon == CBIOS_NULL) || !(pDpContext->Common.DeviceType & ALL_DP_TYPES))
    {
        cbDebugPrint((MAKE_LEVEL(DP, ERROR), "%s: the 2nd param is invalid!\n", FUNCTION_NAME));
        return CBIOS_NULL;
    }

    if (pDpContext->Common.SupportMonitorType & (CBIOS_MONITOR_TYPE_DP | CBIOS_MONITOR_TYPE_PANEL))
    {
        return &(pDpContext->DPMonitorContext);
    }
    else
    {
        cbDebugPrint((MAKE_LEVEL(DP, ERROR), "%s: wrong support monitor type 0x%x\n", FUNCTION_NAME, pDpContext->Common.SupportMonitorType));
        return CBIOS_NULL;
    }
}

PCBIOS_VOID cbDPPort_GetHDMIMonitorContext(PCBIOS_VOID pvcbe, PCBIOS_DEVICE_COMMON pDevCommon)
{
    PCBIOS_DP_CONTEXT  pDpContext = container_of(pDevCommon, PCBIOS_DP_CONTEXT, Common);

    if ((pDevCommon == CBIOS_NULL) || !(pDpContext->Common.DeviceType & ALL_DP_TYPES))
    {
        cbDebugPrint((MAKE_LEVEL(DP, ERROR), "%s: the 2nd param is invalid!\n", FUNCTION_NAME));
        return CBIOS_NULL;
    }

    if ((pDpContext->Common.CurrentMonitorType == CBIOS_MONITOR_TYPE_HDMI) || (pDpContext->Common.CurrentMonitorType == CBIOS_MONITOR_TYPE_DVI))
    {
        return &(pDpContext->HDMIMonitorContext);
    }
    else
    {
        cbDebugPrint((MAKE_LEVEL(DP, ERROR), "%s: wrong monitor type 0x%x\n", FUNCTION_NAME, pDpContext->Common.CurrentMonitorType));
        return CBIOS_NULL;
    }
}

CBIOS_BOOL cbDPPort_IsDeviceInDpMode(PCBIOS_VOID pvcbe, PCBIOS_DEVICE_COMMON pDevCommon)
{
    PCBIOS_DP_CONTEXT pDpContext = container_of(pDevCommon, PCBIOS_DP_CONTEXT, Common);
    CBIOS_BOOL isDpMode = CBIOS_FALSE;

    if ((pDevCommon == CBIOS_NULL) || !(pDpContext->Common.DeviceType & ALL_DP_TYPES))
    {
        return CBIOS_FALSE;
    }

    isDpMode = !pDpContext->DPPortParams.bDualMode;

    return isDpMode;
}

CBIOS_STATUS cbDPPort_Isr(PCBIOS_VOID pvcbe, PCBIOS_DEVICE_COMMON pDevCommon, PCBIOS_DP_ISR_PARA pDPIsrPara)
{
    PCBIOS_DP_CONTEXT pDpContext = container_of(pDevCommon, PCBIOS_DP_CONTEXT, Common);

    if ((pDevCommon == CBIOS_NULL) || !(pDpContext->Common.DeviceType & ALL_DP_TYPES))
    {
        cbDebugPrint((MAKE_LEVEL(DP, ERROR), "%s: the 2nd param is invalid!\n", FUNCTION_NAME));
        return CBIOS_ER_INVALID_PARAMETER;
    }

    return cbDPMonitor_Isr(pvcbe, &pDpContext->DPMonitorContext, pDPIsrPara);
}

CBIOS_BOOL cbDPPort_WorkThreadMainFunc(PCBIOS_VOID pvcbe, PCBIOS_DEVICE_COMMON pDevCommon, PCBIOS_DP_WORKTHREAD_PARA pDPWorkThreadPara)
{
    PCBIOS_DP_CONTEXT pDpContext = container_of(pDevCommon, PCBIOS_DP_CONTEXT, Common);

    if ((pDevCommon == CBIOS_NULL) || !(pDpContext->Common.DeviceType & ALL_DP_TYPES))
    {
        cbDebugPrint((MAKE_LEVEL(DP, ERROR), "%s: the 2nd param is invalid!\n", FUNCTION_NAME));
        return CBIOS_FALSE;
    }

    return cbDPMonitor_WorkThreadMainFunc(pvcbe, &pDpContext->DPMonitorContext, pDPWorkThreadPara);
}

CBIOS_STATUS cbDPPort_SetNotifications(PCBIOS_VOID pvcbe, PCBIOS_DEVICE_COMMON pDevCommon, PCBIOS_DP_NOTIFICATIONS pDPNotifications)
{
    PCBIOS_DP_CONTEXT pDpContext = container_of(pDevCommon, PCBIOS_DP_CONTEXT, Common);

    if ((pDevCommon == CBIOS_NULL) || !(pDpContext->Common.DeviceType & ALL_DP_TYPES))
    {
        cbDebugPrint((MAKE_LEVEL(DP, ERROR), "%s: the 2nd param is invalid!\n", FUNCTION_NAME));
        return CBIOS_ER_INVALID_PARAMETER;
    }

    return cbDPMonitor_SetNotifications(pvcbe, &pDpContext->DPMonitorContext, pDPNotifications);
}

CBIOS_STATUS  cbDPPort_GetInt(PCBIOS_VOID pvcbe, PCBIOS_DEVICE_COMMON pDevCommon, PCBIOS_DP_INT_PARA  pDPIntPara)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    PCBIOS_DP_CONTEXT pDpContext = container_of(pDevCommon, PCBIOS_DP_CONTEXT, Common);
    PCBIOS_DP_MONITOR_CONTEXT    pDPMonitorContext = &(pDpContext->DPMonitorContext);
    CBIOS_CONNECT_STATUS ConnectStatus = {0};
    CBIOS_U8  HPDStatus = 0;
    CBIOS_MODULE_INDEX DPModuleIndex = CBIOS_MODULE_INDEX_INVALID; 

    if ((pDevCommon == CBIOS_NULL) || !(pDevCommon->DeviceType & ALL_DP_TYPES))
    {
        cbDebugPrint((MAKE_LEVEL(DP, ERROR), "%s: the 2nd param is invalid!\n", FUNCTION_NAME));
        return CBIOS_ER_INVALID_PARAMETER;
    }

    DPModuleIndex = cbGetModuleIndex(pcbe, pDevCommon->DeviceType, CBIOS_MODULE_TYPE_DP);

    if (DPModuleIndex >= DP_MODU_NUM)
    {
        return CBIOS_ER_INTERNAL;
    }

    pDPIntPara->IntType = CBIOS_DP_INT_STATUS_NO_INT;
    cbDIU_DP_GetConnectStatus(pcbe, DPModuleIndex, &ConnectStatus);
    HPDStatus = ConnectStatus.HpdStatus;

    if (pDevCommon->bConnected && HPDStatus != CBIOS_HPD_STATUS_OUT && ConnectStatus.DpUsbMode != pDPMonitorContext->CurrentDpUsbMode)
    {
        pDPIntPara->IntType = CBIOS_DP_INT_STATUS_DP_OUT;
    }
    else if (HPDStatus == CBIOS_HPD_STATUS_IN)
    {
        pDPIntPara->IntType = CBIOS_DP_INT_STATUS_IN;
    }
    else if (HPDStatus == CBIOS_HPD_STATUS_OUT)
    {
        if(cbDPPort_IsDeviceInDpMode(pcbe, pDevCommon))
        {
            pDPIntPara->IntType = CBIOS_DP_INT_STATUS_DP_OUT;
        }
        else
        {
            pDPIntPara->IntType = CBIOS_DP_INT_STATUS_HDMI_OUT;
        }
    }
    else if(HPDStatus == CBIOS_HPD_STATUS_IRQ)
    {
        pDPIntPara->IntType = CBIOS_DP_INT_IRQ;
    }

    //for eDP plug out INT, nothing need to do by driver, so report NO_INT
    if(cbDIU_EDP_IsEDPSupported(pcbe, DPModuleIndex) &&
        pDPIntPara->IntType == CBIOS_DP_INT_STATUS_DP_OUT)
    {
        pDPIntPara->IntType = CBIOS_DP_INT_STATUS_NO_INT;
    }

    return  CBIOS_OK;
}

CBIOS_STATUS  cbDPPort_HandleIrq(PCBIOS_VOID pvcbe, PCBIOS_DEVICE_COMMON pDevCommon, PCBIOS_DP_HANDLE_IRQ_PARA pDPHandleIrqPara)
{
    PCBIOS_DP_CONTEXT pDpContext = container_of(pDevCommon, PCBIOS_DP_CONTEXT, Common);

    if ((pDevCommon == CBIOS_NULL) || !(pDpContext->Common.DeviceType & ALL_DP_TYPES))
    {
        cbDebugPrint((MAKE_LEVEL(DP, ERROR), "%s: the 2nd param is invalid!\n", FUNCTION_NAME));
        return CBIOS_ER_INVALID_PARAMETER;
    }

    return  cbDPMonitor_HandleIrq(pvcbe, &pDpContext->DPMonitorContext, pDPHandleIrqPara);
}

CBIOS_STATUS cbDPPort_GetCustomizedTiming(PCBIOS_VOID pvcbe, PCBIOS_DEVICE_COMMON pDevCommon, PCBIOS_DP_CUSTOMIZED_TIMING pDPCustomizedTiming)
{
    PCBIOS_DP_CONTEXT pDpContext = container_of(pDevCommon, PCBIOS_DP_CONTEXT, Common);

    if ((pDevCommon == CBIOS_NULL) || !(pDpContext->Common.DeviceType & ALL_DP_TYPES))
    {
        cbDebugPrint((MAKE_LEVEL(DP, ERROR), "%s: the 2nd param is invalid!\n", FUNCTION_NAME));
        return CBIOS_ER_INVALID_PARAMETER;
    }

    return cbDPMonitor_GetCustomizedTiming(pvcbe, &pDpContext->DPMonitorContext, pDPCustomizedTiming);
}

static CBIOS_VOID cbDPPort_HwInit(PCBIOS_EXTENSION_COMMON pcbe, PCBIOS_DEVICE_COMMON pDevCommon)
{
    CBIOS_MODULE_INDEX DPModuleIndex = CBIOS_MODULE_INDEX_INVALID;
    PCBIOS_DP_MONITOR_CONTEXT pDPMonitorContext = CBIOS_NULL;

    if ((pDevCommon == CBIOS_NULL) || (!(pDevCommon->DeviceType & ALL_DP_TYPES)))
    {
        cbDebugPrint((MAKE_LEVEL(DP, ERROR), "%s: the 2nd param is invalid!\n", FUNCTION_NAME));
        return;
    }

    DPModuleIndex = cbGetModuleIndex(pcbe, pDevCommon->DeviceType, CBIOS_MODULE_TYPE_DP);

    //warning!!! if DP/HDMI is on, can't change ephy, or maybe exist garbage
    if (!(cbDIU_DP_IsOn(pcbe, DPModuleIndex) || cbDIU_HDMI_IsOn(pcbe, DPModuleIndex)))
    {
        cbPHY_DP_InitEPHY(pcbe, DPModuleIndex);
        cbPHY_DP_DeInitEPHY(pcbe, DPModuleIndex);
        cbPHY_DP_AuxPowerOnOff(pcbe, DPModuleIndex, CBIOS_FALSE);
    }
    else
    {
        cbDebugPrint((MAKE_LEVEL(DP, DEBUG), "%s: EPHY has been initialized for 0x%x!\n", FUNCTION_NAME, pDevCommon->DeviceType));
    }

    if (pcbe->ChipID == CHIPID_CNE001 && DPModuleIndex < DP_USB_MODU_NUM)
    {
        // Enable USB Apply DP_PWR, for Tunneling HPD detect
        cbMMIOWriteReg32(pcbe, DP_REG_LTTPR_CFG1[DPModuleIndex], 0x00000008, 0xfffffff7);

        // Enable Alt-mode HPD detecter power
        cbMMIOWriteReg32(pcbe, DP_REG_EPHY_MUX_HPD[DPModuleIndex], 0x00010000, 0xfffeffff);

        pDPMonitorContext = cbGetDPMonitorContext(pcbe, pDevCommon);
        cbDPMonitor_InitUsbPara(pcbe, pDPMonitorContext);
    }
}

static CBIOS_VOID cbDPPort_CalculateInsertLineNum(PCBIOS_EXTENSION_COMMON pcbe, PCBIOS_DEVICE_COMMON pDevCommon, PCBIOS_DISP_MODE_PARAMS pModeParams)
{
    PCBIOS_DP_CONTEXT  pDpContext = container_of(pDevCommon, PCBIOS_DP_CONTEXT, Common);
    PCBIOS_MONITOR_MISC_ATTRIB pMonitorAttrib = &(pDevCommon->EdidStruct.Attribute);
    CBIOS_U32 VTotal = 0;
    CBIOS_U32 CurRefRate = 0;
    CBIOS_U32 MinRefRate = 0, MaxRefRate = 0;

    if ((pDevCommon == CBIOS_NULL) || !(pDpContext->Common.DeviceType & ALL_DP_TYPES))
    {
        cbDebugPrint((MAKE_LEVEL(DP, ERROR), "%s: the 2nd param is invalid!\n", FUNCTION_NAME));
        return ;
    }

    if (!(pMonitorAttrib->AMDVSDBData.IsFreeSyncSupported))
    {
        pDpContext->DPPortParams.InsertLineNum = 0;
        return;
    }

    CurRefRate = pModeParams->TargetTiming.RefreshRate / 100;
    MinRefRate = pMonitorAttrib->AMDVSDBData.MinSupportedRefRate;
    MaxRefRate = pMonitorAttrib->AMDVSDBData.MaxSupportedRefRate;
    VTotal = pModeParams->TargetTiming.VerTotal;

    if ((CurRefRate > MinRefRate) && (CurRefRate <= MaxRefRate))
    {
        pDpContext->DPPortParams.InsertLineNum = (VTotal * (CurRefRate - MinRefRate))/MinRefRate;
    }
    else
    {
        pDpContext->DPPortParams.InsertLineNum = 0;
    }
}

//check whether device is plugged out through interrupt status
static CBIOS_BOOL cbDPPort_IsDevicePlugOut(PCBIOS_EXTENSION_COMMON pcbe, PCBIOS_DP_CONTEXT  pDpContext, PCBIOS_DETECT_FLAG pDetectFlag)
{
    CBIOS_MODULE_INDEX DPModuleIndex = CBIOS_MODULE_INDEX_INVALID;
    PCBIOS_DEVICE_COMMON pDevCommon = CBIOS_NULL;
    PCBIOS_HDCP_CONTEXT   pHdcpCtx = CBIOS_NULL;
    CBIOS_CONNECT_STATUS  ConnectStatus = {0};
    CBIOS_BOOL  bPluggedOut = CBIOS_FALSE;

    if(!pcbe || !pDpContext || !pDetectFlag)
    {
        return CBIOS_FALSE;
    }

    pDpContext->DPMonitorContext.DpUsbMode = CBIOS_DP_STANDARD;
    pDevCommon = &pDpContext->Common;
    DPModuleIndex = cbGetModuleIndex(pcbe, pDevCommon->DeviceType, CBIOS_MODULE_TYPE_DP);

    //can only hardcode EDID of HDMI
    if (pDetectFlag->HardcodeDetect)
    {
        pDpContext->DPPortParams.bDualMode = CBIOS_TRUE;
    }
    else
    {
        pDpContext->DPPortParams.bDualMode = cbDualModeDetect(pcbe, pDevCommon);
    }

    //In DualMode(HDMI), must do real detection through i2c, as some monitor will trigger hpd interrupt even when it's connected and can always be detected
    //but for HDCP CTS, when sink trigger hpd out interrupt, driver must report status change to OS
    if(pcbe->DriverFlags.bRunHDCPCTS || pDpContext->DPPortParams.bDualMode == CBIOS_FALSE)
    {
        if (cbDIU_EDP_IsEDPSupported(pcbe, DPModuleIndex) && (pDevCommon->PowerState != CBIOS_PM_ON))
        {
            //for EDP, it's always connected, there is no plug out case
            bPluggedOut = CBIOS_FALSE;
        }
        else
        {
            bPluggedOut = CBIOS_FALSE;
            cbDIU_DP_GetConnectStatus(pcbe, DPModuleIndex, &ConnectStatus);
            if(ConnectStatus.DpUsbMode == CBIOS_DP_STANDARD)
            {
                if(ConnectStatus.HpdStatus == CBIOS_HPD_STATUS_OUT || 
                    ConnectStatus.HpdStatus == CBIOS_HPD_STATUS_INIT)
                {
                    bPluggedOut = CBIOS_TRUE;
                }
            }
            else if(ConnectStatus.HpdStatus == CBIOS_HPD_STATUS_OUT)
            {
                bPluggedOut = CBIOS_TRUE;
            }
            pDpContext->DPMonitorContext.DpUsbMode = ConnectStatus.DpUsbMode;
        }
    }

    if(bPluggedOut)
    {
        pHdcpCtx = (PCBIOS_HDCP_CONTEXT)pDevCommon->pHDCPContext;
        cbClearEdidRelatedData(pcbe, pDevCommon);
        cb_memset(pHdcpCtx->BKsv, 0, sizeof(pHdcpCtx->BKsv));
    }

    return bPluggedOut;
}

static CBIOS_BOOL cbDPPort_DeviceDetect(PCBIOS_EXTENSION_COMMON pcbe, PCBIOS_DEVICE_COMMON pDevCommon, PCBIOS_DETECT_FLAG pDetectFlag)
{
    CBIOS_BOOL         bConnected = CBIOS_FALSE;
    PCBIOS_DP_CONTEXT  pDpContext = container_of(pDevCommon, PCBIOS_DP_CONTEXT, Common);
    CBIOS_MODULE_INDEX DPModuleIndex = CBIOS_MODULE_INDEX_INVALID;
    DP_EPHY_MODE  Mode;

    if ((pDevCommon == CBIOS_NULL) || !(pDpContext->Common.DeviceType & ALL_DP_TYPES))
    {
        cbDebugPrint((MAKE_LEVEL(DP, ERROR), "%s: the 2nd param is invalid!\n", FUNCTION_NAME));
        return CBIOS_FALSE;
    }

    DPModuleIndex = cbGetModuleIndex(pcbe, pDevCommon->DeviceType, CBIOS_MODULE_TYPE_DP);
    if(CBIOS_MODULE_INDEX_INVALID == DPModuleIndex)
    {
        cbDebugPrint((MAKE_LEVEL(DP, ERROR), "Invalid DP index in detect device 0x%x!\n", pDevCommon->DeviceType));
        return CBIOS_FALSE;
    }

    if(cbDPPort_IsDevicePlugOut(pcbe, pDpContext, pDetectFlag))
    {
        /*cbDebugPrint((MAKE_LEVEL(DP, INFO), "Non monitor is plugged in on port 0x%x, FullDetect:%d!\n", 
                                 pDevCommon->DeviceType, pDetectFlag->FullDetect));*/
        return CBIOS_FALSE;
    }

    Mode = cbPHY_DP_GetEphyMode(pcbe, DPModuleIndex);

    if ((!bConnected) && (pDpContext->DPPortParams.bDualMode)
        && (pDpContext->Common.SupportMonitorType & (CBIOS_MONITOR_TYPE_HDMI | CBIOS_MONITOR_TYPE_DVI)))
    {
        bConnected = cbHDMIMonitor_Detect(pcbe, &pDpContext->HDMIMonitorContext, pDetectFlag);
        if (bConnected)
        {
            cbPHY_DP_SelectEphyMode(pcbe, DPModuleIndex, DP_EPHY_TMDS_MODE);
            pDpContext->DPPortParams.DPEphyMode = DP_EPHY_TMDS_MODE;
            if(pcbe->DriverFlags.bRunHDCPCTS)
            {
                cbDIU_DP_SetInterruptMode(pcbe, DPModuleIndex, CBIOS_TRUE);
            }
            else
            {
                cbDIU_DP_SetInterruptMode(pcbe, DPModuleIndex, CBIOS_FALSE);
            }
        }
    }

#if DP_MONITOR_SUPPORT
    if ((!bConnected) && (!pDpContext->DPPortParams.bDualMode)
        && (pDpContext->Common.SupportMonitorType & (CBIOS_MONITOR_TYPE_DP | CBIOS_MONITOR_TYPE_PANEL)))
    {
        cbPHY_DP_SelectEphyMode(pcbe, DPModuleIndex, DP_EPHY_DP_MODE);

        bConnected = cbDPMonitor_Detect(pcbe, &pDpContext->DPMonitorContext, pDetectFlag);
        if (bConnected)
        {
            pDpContext->DPPortParams.DPEphyMode = DP_EPHY_DP_MODE;
            cbDIU_DP_SetInterruptMode(pcbe, DPModuleIndex, CBIOS_TRUE);
        }
    }
#endif

    if (bConnected)
    {
        /*
         *If the previous monitor supports freesync and several lines are inserted,
         *but the newly monitor does not support freesync, then clear inerted lines.
         */
        if ((pDpContext->DPPortParams.InsertLineNum != 0)
            &&!(pDevCommon->EdidStruct.Attribute.AMDVSDBData.IsFreeSyncSupported))
        {
            cbDIU_DP_SetFreeSync(pcbe, DPModuleIndex, 0);
        }
        if (pDpContext->DPMonitorContext.DpUsbMode != pDpContext->DPMonitorContext.CurrentDpUsbMode)
        {
            pDetectFlag->Changed = 1;
        }
    }

    if (!bConnected)
    {
        cbPHY_DP_SelectEphyMode(pcbe, DPModuleIndex, Mode);
        /*cbDebugPrint((MAKE_LEVEL(DP, INFO), "Non monitor is detected on port 0x%x, FullDetect:%d!\n", 
                                pDevCommon->DeviceType, pDetectFlag->FullDetect));*/
    }

    return bConnected;
}

static CBIOS_VOID cbDPPort_OnOff(PCBIOS_EXTENSION_COMMON pcbe, PCBIOS_DEVICE_COMMON pDevCommon, CBIOS_BOOL bOn)
{
    PCBIOS_DP_CONTEXT pDpContext = container_of(pDevCommon, PCBIOS_DP_CONTEXT, Common);
    CBIOS_MODULE_INDEX DPModuleIndex = CBIOS_MODULE_INDEX_INVALID;
    CBIOS_MODULE_INDEX HDMIModuleIndex = CBIOS_MODULE_INDEX_INVALID;
    CBIOS_DISPLAY_SOURCE *pSource = CBIOS_NULL;
    CBIOS_BOOL bDPMode = 0;
    PCBIOS_DP_MONITOR_CONTEXT pDPMonitorContext = &pDpContext->DPMonitorContext;

    if ((pDevCommon == CBIOS_NULL) || !(pDpContext->Common.DeviceType & ALL_DP_TYPES))
    {
        cbDebugPrint((MAKE_LEVEL(DP, ERROR), "%s: the 2nd param is invalid!\n", FUNCTION_NAME));
        return ;
    }

    DPModuleIndex = cbGetModuleIndex(pcbe, pDevCommon->DeviceType, CBIOS_MODULE_TYPE_DP);
    if(CBIOS_MODULE_INDEX_INVALID == DPModuleIndex)
    {
        cbDebugPrint((MAKE_LEVEL(DP, ERROR), "Invalid DP index in onoff device 0x%x!\n", pDevCommon->DeviceType));
        return;
    }

    HDMIModuleIndex = cbGetModuleIndex(pcbe, pDevCommon->DeviceType, CBIOS_MODULE_TYPE_HDMI);
    bDPMode = (HDMIModuleIndex == CBIOS_MODULE_INDEX_INVALID) ? CBIOS_TRUE : CBIOS_FALSE;
    pSource = &(pDevCommon->DispSource);

    if (pSource->bIsSrcChanged)
    {
        cbPHY_DP_SelectPhySource(pcbe, pDpContext->DPPortParams.DPEphyMode, pSource, pDpContext->Common.CurrentMonitorType);
        cbPathMgrSelectDIUSource(pcbe, pSource);

        // source select done, clear flag
        pSource->bIsSrcChanged = CBIOS_FALSE;
    }

    if (bOn)
    {
        cbPathMgrModuleOnOff(pcbe, pSource->ModulePath, CBIOS_TRUE);
        cbDIU_DP_DPModeEnable(pcbe, DPModuleIndex, bDPMode);
    }
    else
    {
        cbDIU_DP_SetFreeSync(pcbe, DPModuleIndex, 0);
    }

    if (bDPMode)
    {
        if(bOn)
        {
            if ((pDpContext->Common.CurrentMonitorType == CBIOS_MONITOR_TYPE_DP) || (pDpContext->Common.CurrentMonitorType == CBIOS_MONITOR_TYPE_PANEL))
            {
            #if DP_MONITOR_SUPPORT
                if (pDPMonitorContext->CurrentDpUsbMode != CBIOS_DP_ALTMODE)
                {
                    cbPHY_DP_InitEPHY(pcbe, DPModuleIndex);
                }
                cbDPMonitor_OnOff(pcbe, &pDpContext->DPMonitorContext, bOn);
            #endif
            }
        }
        else
        {
        #if DP_MONITOR_SUPPORT
            cbDPMonitor_OnOff(pcbe, &pDpContext->DPMonitorContext, bOn);
        #endif
        }
    }
    else
    {
        if (bOn)
        {
            if ((pDpContext->Common.CurrentMonitorType == CBIOS_MONITOR_TYPE_HDMI) || (pDpContext->Common.CurrentMonitorType == CBIOS_MONITOR_TYPE_DVI))
            {
                cbHDMIMonitor_OnOff(pcbe, &pDpContext->HDMIMonitorContext, bOn);
                //cbPHY_DP_InitEPHY(pcbe, DPModuleIndex);
                cbPHY_DP_DualModeOnOff(pcbe, DPModuleIndex, pDpContext->HDMIMonitorContext.HDMIClock, bOn);
            }
        }
        else
        {
            // Wait vblank before turning off device to avoid flashing white
            cbWaitNonFullVBlank(pcbe, DPModuleIndex);
            cbHDMIMonitor_OnOff(pcbe, &pDpContext->HDMIMonitorContext, bOn);
            cbPHY_DP_DualModeOnOff(pcbe, DPModuleIndex, pDpContext->HDMIMonitorContext.HDMIClock, bOn);
        }
    }

    if (!bOn)
    {
        cbPathMgrModuleOnOff(pcbe, pSource->ModulePath, CBIOS_FALSE);
    }
    else
    {
        cbDIU_DP_SetFreeSync(pcbe, DPModuleIndex, pDpContext->DPPortParams.InsertLineNum);
    }

    pDevCommon->PowerState = bOn ? CBIOS_PM_ON : CBIOS_PM_OFF;

    cbDebugPrint((MAKE_LEVEL(DP, INFO),"%s: status = %s.\n", FUNCTION_NAME, (bOn)? "On" : "Off"));
}

static CBIOS_VOID cbDPPort_QueryMonitorAttribute(PCBIOS_EXTENSION_COMMON pcbe, PCBIOS_DEVICE_COMMON pDevCommon, PCBiosMonitorAttribute pMonitorAttribute)
{
    PCBIOS_DP_CONTEXT pDpContext = container_of(pDevCommon, PCBIOS_DP_CONTEXT, Common);

    cbTraceEnter(DP);

    if ((pDevCommon == CBIOS_NULL) || !(pDpContext->Common.DeviceType & ALL_DP_TYPES))
    {
        cbDebugPrint((MAKE_LEVEL(DP, ERROR), "%s: the 2nd param is invalid!\n", FUNCTION_NAME));
        return;
    }

    if (pDpContext->Common.CurrentMonitorType & (CBIOS_MONITOR_TYPE_HDMI | CBIOS_MONITOR_TYPE_DVI))
    {
        cbHDMIMonitor_QueryAttribute(pcbe, &pDpContext->HDMIMonitorContext, pMonitorAttribute);
    }
    else if (pDpContext->Common.CurrentMonitorType & (CBIOS_MONITOR_TYPE_DP | CBIOS_MONITOR_TYPE_PANEL))
    {
        cbDPMonitor_QueryAttribute(pcbe, &pDpContext->DPMonitorContext, pMonitorAttribute);
    }

    cbTraceExit(DP);
}

CBIOS_VOID cbDPPort_WriteFIFO(PCBIOS_VOID pvcbe, CBIOS_ACTIVE_TYPE DeviceType, CBIOS_U8 FIFOIndex, CBIOS_U8 *pDataBuff, CBIOS_U32 BuffLen)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    PCBIOS_DEVICE_COMMON    pDevCommon  = cbGetDeviceCommon(&pcbe->DeviceMgr, DeviceType);
    CBIOS_MONITOR_TYPE      MonitorType = pDevCommon->CurrentMonitorType;
    CBIOS_BOOL              bDPDevice = CBIOS_FALSE;
    CBIOS_U8                SR47Value = cbMMIOReadReg(pcbe, SR_47);
    CBIOS_U32               i = 0;

    if ((MonitorType == CBIOS_MONITOR_TYPE_DP) || (MonitorType == CBIOS_MONITOR_TYPE_PANEL))
    {
        bDPDevice = CBIOS_TRUE;
    }

    if (DeviceType == CBIOS_TYPE_DP1)
    {
        //select LUT
        if (bDPDevice)
        {
            cbMMIOWriteReg(pcbe, SR_47, 0x06, 0xF0);
        }
        else
        {
            cbMMIOWriteReg(pcbe, SR_47, 0x04, 0xF0);
        }
    }
    else if (DeviceType == CBIOS_TYPE_DP2)
    {
        //select LUT
        if (bDPDevice)
        {
            cbMMIOWriteReg(pcbe, SR_47, 0x07, 0xF0);
        }
        else
        {
            cbMMIOWriteReg(pcbe, SR_47, 0x05, 0xF0);
        }
    }
    else if (DeviceType == CBIOS_TYPE_DP3)
    {
        //select LUT
        if (bDPDevice)
        {
            cbMMIOWriteReg(pcbe, SR_47, 0x0E, 0xF0);
        }
        else
        {
            cbMMIOWriteReg(pcbe, SR_47, 0x0C, 0xF0);
        }
    }
    else if (DeviceType == CBIOS_TYPE_DP4)
    {
        //select LUT
        if (bDPDevice)
        {
            cbMMIOWriteReg(pcbe, SR_47, 0x0F, 0xF0);
        }
        else
        {
            cbMMIOWriteReg(pcbe, SR_47, 0x0D, 0xF0);
        }
    }
    else
    {
        cbDebugPrint((MAKE_LEVEL(HDMI, ERROR), "%s: invalid HDMI/DP device!\n", FUNCTION_NAME));
        return;
    }

    cb_WriteU8(pcbe->pAdapterContext, 0x83C8, FIFOIndex);

    for (i = BuffLen; i > 0; i--)
    {
        cb_WriteU8(pcbe->pAdapterContext, 0x83C9, pDataBuff[i - 1]);
    }

    //restore SR47
    cbMMIOWriteReg(pcbe, SR_47, SR47Value, 0x00);
}

CBIOS_VOID cbDPPort_ReadFIFO(PCBIOS_VOID pvcbe, CBIOS_ACTIVE_TYPE DeviceType, CBIOS_U8 FIFOIndex, CBIOS_U8 *pDataBuff, CBIOS_U32 BuffLen)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    PCBIOS_DEVICE_COMMON    pDevCommon  = cbGetDeviceCommon(&pcbe->DeviceMgr, DeviceType);
    CBIOS_MONITOR_TYPE      MonitorType = pDevCommon->CurrentMonitorType;
    CBIOS_BOOL              bDPDevice = CBIOS_FALSE;
    CBIOS_U8                SR47Value = cbMMIOReadReg(pcbe, SR_47);
    CBIOS_U32               i = 0;

    if ((MonitorType == CBIOS_MONITOR_TYPE_DP) || (MonitorType == CBIOS_MONITOR_TYPE_PANEL))
    {
        bDPDevice = CBIOS_TRUE;
    }

    if (DeviceType == CBIOS_TYPE_DP1)
    {
        //select LUT
        if (bDPDevice)
        {
            cbMMIOWriteReg(pcbe, SR_47, 0x06, 0xF0);
        }
        else
        {
            cbMMIOWriteReg(pcbe, SR_47, 0x04, 0xF0);
        }
    }
    else if (DeviceType == CBIOS_TYPE_DP2)
    {
        //select LUT
        if (bDPDevice)
        {
            cbMMIOWriteReg(pcbe, SR_47, 0x07, 0xF0);
        }
        else
        {
            cbMMIOWriteReg(pcbe, SR_47, 0x05, 0xF0);
        }
    }
    else if (DeviceType == CBIOS_TYPE_DP3)
    {
        //select LUT
        if (bDPDevice)
        {
            cbMMIOWriteReg(pcbe, SR_47, 0x0E, 0xF0);
        }
        else
        {
            cbMMIOWriteReg(pcbe, SR_47, 0x0C, 0xF0);
        }
    }
    else if (DeviceType == CBIOS_TYPE_DP4)
    {
        //select LUT
        if (bDPDevice)
        {
            cbMMIOWriteReg(pcbe, SR_47, 0x0F, 0xF0);
        }
        else
        {
            cbMMIOWriteReg(pcbe, SR_47, 0x0D, 0xF0);
        }
    }
    else
    {
        cbDebugPrint((MAKE_LEVEL(HDMI, ERROR), "%s: invalid HDMI/DP device!\n", FUNCTION_NAME));
        return;
    }

    cb_WriteU8(pcbe->pAdapterContext, 0x83C7, FIFOIndex);

    for (i = BuffLen; i > 0; i--)
    {
        pDataBuff[i - 1] = cb_ReadU8(pcbe->pAdapterContext, 0x83C9);
    }

    //restore SR47
    cbMMIOWriteReg(pcbe, SR_47, SR47Value, 0x00);
}

static CBIOS_VOID cbDPPort_UpdateModeInfo(PCBIOS_EXTENSION_COMMON pcbe, PCBIOS_DEVICE_COMMON pDevCommon, PCBIOS_DISP_MODE_PARAMS pModeParams)
{
    PCBIOS_DP_CONTEXT pDpContext = container_of(pDevCommon, PCBIOS_DP_CONTEXT, Common);
    CBIOS_U32 PixelRepitition = pModeParams->PixelRepitition;
    CBIOS_U32 DiuPLLClock = 0;
    CBIOS_MODULE_INDEX HDTVModuleIndex = CBIOS_MODULE_INDEX_INVALID;

    if ((pDevCommon == CBIOS_NULL) || !(pDpContext->Common.DeviceType & ALL_DP_TYPES))
    {
        cbDebugPrint((MAKE_LEVEL(DP, ERROR), "%s: the 2nd param is invalid!\n", FUNCTION_NAME));
        return ;
    }

    HDTVModuleIndex = pDevCommon->DispSource.ModuleList.HDTVModule.Index;
    if (pDpContext->Common.CurrentMonitorType & (CBIOS_MONITOR_TYPE_DP | CBIOS_MONITOR_TYPE_PANEL))
    {
        if ((HDTVModuleIndex != CBIOS_MODULE_INDEX_INVALID) && (pModeParams->TargetModePara.bInterlace))
        {
            pModeParams->TargetTiming.PixelClock *= 2;
        }
        DiuPLLClock = pModeParams->TargetTiming.PixelClock;
        pModeParams->TargetTiming.PLLClock = DiuPLLClock;

        cbDPMonitor_UpdateModeInfo(pcbe, &pDpContext->DPMonitorContext, pModeParams);
    }

    if (pDpContext->Common.CurrentMonitorType & (CBIOS_MONITOR_TYPE_HDMI | CBIOS_MONITOR_TYPE_DVI))
    {
        // patch to 1080i don't bypass HDTV module
        if ((HDTVModuleIndex != CBIOS_MODULE_INDEX_INVALID) && (pModeParams->TargetModePara.bInterlace))
        {
            if((pModeParams->TargetModePara.XRes == 1920) && (pModeParams->TargetModePara.YRes == 1080))
            {
                pModeParams->TargetTiming.PixelClock *= 2;
            }
        }
        // determine DIU PLL clock
        DiuPLLClock = pModeParams->TargetTiming.PixelClock;

        if (pModeParams->BitPerComponent == 8) // 24bit
        {
            if (PixelRepitition == 1)
            {
                DiuPLLClock *= 1;
            }
            else if (PixelRepitition == 2)
            {
                DiuPLLClock *= 2;
            }
            else if (PixelRepitition == 4)
            {
                DiuPLLClock *= 4;
            }
            pDpContext->HDMIMonitorContext.HDMIClock = DiuPLLClock;
        }
        else if (pModeParams->BitPerComponent == 10) // 30bit
        {
            if (PixelRepitition == 1)
            {
                DiuPLLClock = (DiuPLLClock * 5);
            }
            else if (PixelRepitition == 2)
            {
                DiuPLLClock *= 5;
            }
            else if (PixelRepitition == 4)
            {
                DiuPLLClock *= 10;
            }
            pDpContext->HDMIMonitorContext.HDMIClock = DiuPLLClock/4;
        }
        else if (pModeParams->BitPerComponent == 12) // 36bit
        {
            if (PixelRepitition == 1)
            {
                if (CBIOS_YCBCR422OUTPUT == pModeParams->TargetModePara.OutputSignal)
                {
                    DiuPLLClock *= 1;
                    pDpContext->HDMIMonitorContext.HDMIClock = DiuPLLClock;
                }
                else
                {
                    DiuPLLClock *= 3;
                    pDpContext->HDMIMonitorContext.HDMIClock = DiuPLLClock/2;
                }
            }
            else if (PixelRepitition == 2)
            {
                DiuPLLClock *= 3;
                pDpContext->HDMIMonitorContext.HDMIClock = DiuPLLClock;
            }
            else if (PixelRepitition == 4)
            {
                DiuPLLClock *= 6;
                pDpContext->HDMIMonitorContext.HDMIClock = DiuPLLClock;
            }
        }

        if (HDTVModuleIndex != CBIOS_MODULE_INDEX_INVALID && (pModeParams->TargetModePara.bInterlace))
        {
            pDpContext->HDMIMonitorContext.HDMIClock /= 2;
        }

        if (CBIOS_YCBCR420OUTPUT == pModeParams->TargetModePara.OutputSignal)
        {
             pDpContext->HDMIMonitorContext.HDMIClock /= 2;
        }
        pModeParams->TargetTiming.PLLClock = DiuPLLClock;

        cbHDMIMonitor_UpdateModeInfo(pcbe, &pDpContext->HDMIMonitorContext, pModeParams);

    }

    pDpContext->DPMonitorContext.CurrentDpUsbMode = pDpContext->DPMonitorContext.DpUsbMode;
    cbDPPort_CalculateInsertLineNum(pcbe, pDevCommon, pModeParams);
}

static CBIOS_VOID cbDPPort_SetMode(PCBIOS_EXTENSION_COMMON pcbe, PCBIOS_DEVICE_COMMON pDevCommon, PCBIOS_DISP_MODE_PARAMS pModeParams)
{
    PCBIOS_DP_CONTEXT  pDpContext  = container_of(pDevCommon, PCBIOS_DP_CONTEXT, Common);
    CBIOS_MONITOR_TYPE MonitorType = CBIOS_MONITOR_TYPE_NONE;
    CBIOS_MODULE_INDEX HDTVModuleIndex = CBIOS_MODULE_INDEX_INVALID;

    if ((pDevCommon == CBIOS_NULL) || !(pDpContext->Common.DeviceType & ALL_DP_TYPES))
    {
        cbDebugPrint((MAKE_LEVEL(DP, ERROR), "%s: the 2nd param is invalid!\n", FUNCTION_NAME));
        return ;
    }

    MonitorType = pDevCommon->CurrentMonitorType;
    HDTVModuleIndex = pDevCommon->DispSource.ModuleList.HDTVModule.Index;

    if (HDTVModuleIndex != CBIOS_MODULE_INDEX_INVALID)
    {
        // Enable HDTV block function.
        cbDoHDTVFuncSetting(pcbe, pModeParams, pModeParams->IGAIndex, pDevCommon->DeviceType);
    }

    if ((MonitorType == CBIOS_MONITOR_TYPE_HDMI) || (MonitorType == CBIOS_MONITOR_TYPE_DVI))
    {
        cbHDMIMonitor_SetMode(pcbe, &pDpContext->HDMIMonitorContext, pModeParams);
    }
    else if ((MonitorType == CBIOS_MONITOR_TYPE_DP) || (MonitorType == CBIOS_MONITOR_TYPE_PANEL))
    {
        cbDPMonitor_SetMode(pcbe, &pDpContext->DPMonitorContext, pModeParams);
    }
}

PCBIOS_DEVICE_COMMON cbDPPort_Init(PCBIOS_VOID pvcbe, PVCP_INFO pVCP, CBIOS_ACTIVE_TYPE DeviceType)
{
    PCBIOS_EXTENSION_COMMON pcbe          = (PCBIOS_EXTENSION_COMMON)pvcbe;
    PCBIOS_DP_CONTEXT    pDpContext = CBIOS_NULL;
    PCBIOS_DEVICE_COMMON pDeviceCommon = CBIOS_NULL;
    CBIOS_MODULE_INDEX   DPIndex;

    if(!(DeviceType & ALL_DP_TYPES))
    {
        cbDebugPrint((MAKE_LEVEL(GENERIC, ERROR),"%s: unsupported device type!!!\n", FUNCTION_NAME));
        return CBIOS_NULL;
    }

    pDpContext = cb_AllocateNonpagedPool(sizeof(CBIOS_DP_CONTEXT));
    if(pDpContext == CBIOS_NULL)
    {
        cbDebugPrint((MAKE_LEVEL(GENERIC, ERROR),"%s: pDpContext allocate error!!!\n", FUNCTION_NAME));
        return CBIOS_NULL;
    }

    pDeviceCommon = &pDpContext->Common;

    pDeviceCommon->DeviceType = DeviceType;
    pDeviceCommon->SupportMonitorType = cbGetSupportMonitorType(pcbe, DeviceType);
    pDeviceCommon->CurrentMonitorType = CBIOS_MONITOR_TYPE_NONE;
    pDeviceCommon->PowerState = CBIOS_PM_INVALID;
    cbHDCP_Init(pvcbe, &pDeviceCommon->pHDCPContext);

    cbInitialModuleList(&pDeviceCommon->DispSource.ModuleList);

    pDeviceCommon->pfncbDeviceHwInit = (PFN_cbDeviceHwInit)cbDPPort_HwInit;
    pDeviceCommon->pfncbUpdateDeviceModeInfo = (PFN_cbUpdateDeviceModeInfo)cbDPPort_UpdateModeInfo;
    pDeviceCommon->pfncbQueryMonitorAttribute = (PFN_cbQueryMonitorAttribute)cbDPPort_QueryMonitorAttribute;
    pDeviceCommon->pfncbDeviceDetect = (PFN_cbDeviceDetect)cbDPPort_DeviceDetect;
    pDeviceCommon->pfncbDeviceOnOff = (PFN_cbDeviceOnOff)cbDPPort_OnOff;
    pDeviceCommon->pfncbDeviceSetMode = (PFN_cbDeviceSetMode)cbDPPort_SetMode;

    pDeviceCommon->pBusLock = cb_CreateLock(CBIOS_OS_MUTEX_LOCK);

    if (DeviceType == CBIOS_TYPE_DP1)
    {
        pDeviceCommon->I2CBus = pVCP->DP1DualModeCharByte & I2CBUSMASK;    
        pDeviceCommon->DispSource.ModuleList.DPModule.Index = CBIOS_MODULE_INDEX1;
        pDeviceCommon->DispSource.ModuleList.HDMIModule.Index = CBIOS_MODULE_INDEX1;
        pDeviceCommon->DispSource.ModuleList.HDCPModule.Index = CBIOS_MODULE_INDEX1;
        pDeviceCommon->DispSource.ModuleList.HDACModule.Index = CBIOS_MODULE_INDEX1;
        pDeviceCommon->PortConnType = pcbe->SysBiosInfo.Dp1PortConnType;
    }
    else if (DeviceType == CBIOS_TYPE_DP2)
    {
        pDeviceCommon->I2CBus = pVCP->DP2DualModeCharByte & I2CBUSMASK;
        pDeviceCommon->DispSource.ModuleList.DPModule.Index = CBIOS_MODULE_INDEX2;
        pDeviceCommon->DispSource.ModuleList.HDMIModule.Index = CBIOS_MODULE_INDEX2;
        pDeviceCommon->DispSource.ModuleList.HDCPModule.Index = CBIOS_MODULE_INDEX2;
        pDeviceCommon->DispSource.ModuleList.HDACModule.Index = CBIOS_MODULE_INDEX2;
        pDeviceCommon->PortConnType = pcbe->SysBiosInfo.Dp2PortConnType;
    }
    else if (DeviceType == CBIOS_TYPE_DP3)
    {
        pDeviceCommon->I2CBus = pVCP->DP3DualModeCharByte & I2CBUSMASK;
        pDeviceCommon->DispSource.ModuleList.DPModule.Index = CBIOS_MODULE_INDEX3;
        pDeviceCommon->DispSource.ModuleList.HDMIModule.Index = CBIOS_MODULE_INDEX3;
        pDeviceCommon->DispSource.ModuleList.HDCPModule.Index = CBIOS_MODULE_INDEX3;
        pDeviceCommon->DispSource.ModuleList.HDACModule.Index = CBIOS_MODULE_INDEX3;
        pDeviceCommon->PortConnType = pcbe->SysBiosInfo.Dp3PortConnType;
    }
    else if (DeviceType == CBIOS_TYPE_DP4)
    {
        pDeviceCommon->I2CBus = pVCP->DP4DualModeCharByte & I2CBUSMASK;
        pDeviceCommon->DispSource.ModuleList.DPModule.Index = CBIOS_MODULE_INDEX4;
        pDeviceCommon->DispSource.ModuleList.HDMIModule.Index = CBIOS_MODULE_INDEX4;
        pDeviceCommon->DispSource.ModuleList.HDCPModule.Index = CBIOS_MODULE_INDEX4;
        pDeviceCommon->DispSource.ModuleList.HDACModule.Index = CBIOS_MODULE_INDEX_INVALID;
        pDeviceCommon->PortConnType = CBIOS_NON_CONN;
    }

    pDpContext->HDMIMonitorContext.pDevCommon = pDeviceCommon;
    pDpContext->DPMonitorContext.pDevCommon = pDeviceCommon;
    pDpContext->DPMonitorContext.GpioForEDP1Power = pVCP->GpioForEDP1Power;
    pDpContext->DPMonitorContext.GpioForEDP2Power = pVCP->GpioForEDP2Power;
    pDpContext->DPMonitorContext.GpioForEDP1BackLight = pVCP->GpioForEDP1BackLight;
    pDpContext->DPMonitorContext.GpioForEDP2BackLight = pVCP->GpioForEDP2BackLight;
    DPIndex = pDeviceCommon->DispSource.ModuleList.DPModule.Index;
    cb_memcpy(&pDeviceCommon->MaxResConfig, &pVCP->DPMaxResConfig[DPIndex], sizeof(CBIOS_MAX_RES_CONFIG));

    pDpContext->DPMonitorContext.SourceMaxLaneCount = 4;
    pDpContext->DPMonitorContext.SourceMaxLinkSpeed = CBIOS_DP_LINK_SPEED_8100Mbps;
    pDpContext->DPMonitorContext.bSourceSupportTPS3 = CBIOS_TRUE;
    pDpContext->DPMonitorContext.SupportedLinkRateNum = 0;
    pDpContext->DPMonitorContext.LinkRateHwIndex = 0;
    pDpContext->DPMonitorContext.bUseLinkRateSet = 0;
    pDpContext->DPMonitorContext.DpUsbMode = CBIOS_DP_STANDARD;
    pDpContext->DPMonitorContext.CurrentDpUsbMode = CBIOS_DP_STANDARD;
    cb_memset(pDpContext->DPMonitorContext.SupportedLinkRates, 0, DP_MAX_SUPPORTED_RATES_NUM * sizeof(CBIOS_U32));

    pDpContext->DPPortParams.bRunCTS = CBIOS_FALSE;

    return &pDpContext->Common;
}

CBIOS_VOID cbDPPort_DeInit(PCBIOS_VOID pvcbe, PCBIOS_DEVICE_COMMON pDevCommon)
{
    PCBIOS_DP_CONTEXT pDpContext = container_of(pDevCommon, PCBIOS_DP_CONTEXT, Common);

    if ((pDevCommon == CBIOS_NULL) || !(pDevCommon->DeviceType & ALL_DP_TYPES))
    {
        cbDebugPrint((MAKE_LEVEL(DP, ERROR), "%s: the 2nd param is invalid!\n", FUNCTION_NAME));
        return;
    }

    cbHDCP_DeInit(pvcbe, pDevCommon->DeviceType);

    cb_DestroyLock(pDevCommon->pBusLock, CBIOS_OS_MUTEX_LOCK);

    cb_FreePool(pDpContext);
    pDpContext = CBIOS_NULL;
}
