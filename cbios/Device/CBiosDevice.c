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
** CBios general device port functions. Call different port or monitor interface.
**
** NOTE:
** 
******************************************************************************/

#include "CBiosChipShare.h"
#include "../Hw/HwBlock/CBiosDIU_DP.h"
#include "../Hw/HwBlock/CBiosDIU_HDAC.h"
#include "../Hw/HwBlock/CBiosDIU_HDCP.h"

static CBIOS_U32 DevPriorityTbl[]=
{
    CBIOS_TYPE_DP1,
    CBIOS_TYPE_DP2,
    CBIOS_TYPE_DP3,
    CBIOS_TYPE_DP4,
    CBIOS_TYPE_DVO,
    CBIOS_TYPE_CRT,
};

CBIOS_VOID cbDevDeviceHwInit(PCBIOS_VOID pvcbe, PCBIOS_DEVICE_COMMON pDevCommon)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;

    if (pDevCommon->pfncbDeviceHwInit)
    {
        pDevCommon->pfncbDeviceHwInit(pcbe, pDevCommon);
    }
    else
    {
        cbDebugPrint((MAKE_LEVEL(GENERIC, DEBUG), "%s: current device: 0x%x has no hw init func.\n", FUNCTION_NAME, pDevCommon->DeviceType));
    }
}

CBIOS_VOID cbInitDeviceArray(PCBIOS_VOID pvcbe, PVCP_INFO pVCP)
{
    PCBIOS_EXTENSION_COMMON  pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    CBIOS_U32                TmpDev = pcbe->DeviceMgr.SupportDevices;
    CBIOS_ACTIVE_TYPE        CurDevice = CBIOS_TYPE_NONE;
    PCBIOS_DEVICE_COMMON    *ppDeviceCommon = CBIOS_NULL;

    while (TmpDev)
    {
        CurDevice = GET_LAST_BIT(TmpDev);
        ppDeviceCommon = &(cbGetDeviceCommon(&pcbe->DeviceMgr, CurDevice));

        switch (CurDevice)
        {
        case CBIOS_TYPE_CRT:
            *ppDeviceCommon = cbCRTPort_Init(pcbe, pVCP, CBIOS_TYPE_CRT);
            break;
        case CBIOS_TYPE_DVO:
            *ppDeviceCommon = cbDVOPort_Init(pcbe, pVCP, CBIOS_TYPE_DVO);
            break;
        case CBIOS_TYPE_DP1:
        case CBIOS_TYPE_DP2:
        case CBIOS_TYPE_DP3:
        case CBIOS_TYPE_DP4:
            *ppDeviceCommon = cbDPPort_Init(pcbe, pVCP, CurDevice);
            break;
        default:
            break;
        }

        TmpDev &= (~CurDevice);
    }
}

CBIOS_VOID cbDeInitDeviceArray(PCBIOS_VOID pvcbe)
{
    PCBIOS_EXTENSION_COMMON  pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    CBIOS_ACTIVE_TYPE        CurDevice = CBIOS_TYPE_NONE;
    CBIOS_U32                i = 0;

    for (i = 0; i < CBIOS_MAX_DISPLAY_DEVICES_NUM; i++)
    {
        if (pcbe->DeviceMgr.pDeviceArray[i] != CBIOS_NULL)
        {
            if(pcbe->DeviceMgr.pDeviceArray[i]->EdidStruct.pDeviceTimingList)
            {
                cb_FreePool(pcbe->DeviceMgr.pDeviceArray[i]->EdidStruct.pDeviceTimingList);
                pcbe->DeviceMgr.pDeviceArray[i]->EdidStruct.pDeviceTimingList = CBIOS_NULL;
            }
            if(pcbe->DeviceMgr.pDeviceArray[i]->pDeviceModeList)
            {
                cb_FreePool(pcbe->DeviceMgr.pDeviceArray[i]->pDeviceModeList);
                pcbe->DeviceMgr.pDeviceArray[i]->pDeviceModeList = CBIOS_NULL;
            }
            cbClearEdidData(&pcbe->DeviceMgr.pDeviceArray[i]->EdidData);
            cbClearEdidData(&pcbe->DeviceMgr.pDeviceArray[i]->FakeEdid);
            
            CurDevice = pcbe->DeviceMgr.pDeviceArray[i]->DeviceType;
            switch (CurDevice)
            {
            case CBIOS_TYPE_CRT:
                cbCRTPort_DeInit(pcbe, pcbe->DeviceMgr.pDeviceArray[i]);
                break;
            case CBIOS_TYPE_DVO:
                cbDVOPort_DeInit(pcbe, pcbe->DeviceMgr.pDeviceArray[i]);
                break;
            case CBIOS_TYPE_DP1:
            case CBIOS_TYPE_DP2:
            case CBIOS_TYPE_DP3:
            case CBIOS_TYPE_DP4:
                cbDPPort_DeInit(pcbe, pcbe->DeviceMgr.pDeviceArray[i]);
                break;
            default:
                break;
            }

            pcbe->DeviceMgr.pDeviceArray[i] = CBIOS_NULL;
        }
    }
}

CBIOS_U32 cbDevGetPrimaryDevice(CBIOS_U32 Device)
{
    CBIOS_U8 NumOfBit1;
    CBIOS_U32 i;

    NumOfBit1 = cbGetBitsNum(Device);
    if(NumOfBit1 == 1) //only one device
    {
        return Device;
    }
    else if(NumOfBit1 >= 2) //two devices
    {
        for(i = 0; i < sizeofarray(DevPriorityTbl); i++)
        {
            if(Device & DevPriorityTbl[i])
            {
                return DevPriorityTbl[i];
            }
        }
    }

    return 0;
}

CBIOS_VOID cbDevUpdateDeviceModeInfo(PCBIOS_VOID pvcbe, PCBIOS_DEVICE_COMMON pDevCommon, PCBIOS_DISP_MODE_PARAMS pModeParams)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;

    if (pDevCommon->pfncbUpdateDeviceModeInfo)
    {
        pDevCommon->pfncbUpdateDeviceModeInfo(pcbe, pDevCommon, pModeParams);
    }
}

CBIOS_VOID cbDevSetModeToDevice(PCBIOS_VOID pvcbe, PCBIOS_DEVICE_COMMON pDevCommon, PCBIOS_DISP_MODE_PARAMS pModeParams)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;

    if (pDevCommon->pfncbDeviceSetMode)
    {
        pDevCommon->pfncbDeviceSetMode(pcbe, pDevCommon, pModeParams);
    }
}

CBIOS_STATUS cbDevGetModeTiming(PCBIOS_VOID pvcbe, PCBIOS_DEVICE_COMMON pDevCommon, PCBIOS_GET_MODE_TIMING_PARAM pGetModeTiming)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    PCBIOS_TIMING_ATTRIB      pTiming = CBIOS_NULL;
    CBIOS_STATUS              Status = CBIOS_ER_INTERNAL;
    CBIOS_MODE_TARGET_PARA  TargetMode = {0};

    if (CBIOS_NULL == pGetModeTiming)
    {
        cbDebugPrint((MAKE_LEVEL(GENERIC, ERROR), "%s: pGetModeTiming is CBIOS_NULL!\n", FUNCTION_NAME));
        Status =  CBIOS_ER_NULLPOINTER;
        goto END;
    }

    pTiming = pGetModeTiming->pTiming;

    if(CBIOS_NULL == pGetModeTiming->pMode || CBIOS_NULL == pGetModeTiming->pTiming)
    {
        cbDebugPrint((MAKE_LEVEL(GENERIC, ERROR), "%s: pGetModeTiming->pMode or pGetModeTiming->pTiming is CBIOS_NULL!\n", FUNCTION_NAME));
        Status =  CBIOS_ER_INVALID_PARAMETER;
        goto END;
    }

    cb_memset(pTiming, 0, sizeof(CBIOS_TIMING_ATTRIB));

    TargetMode.XRes = pGetModeTiming->pMode->XRes;
    TargetMode.YRes = pGetModeTiming->pMode->YRes;
    TargetMode.XTotal = pGetModeTiming->pMode->XTotal;
    TargetMode.YTotal = pGetModeTiming->pMode->YTotal;
    TargetMode.RefRate = pGetModeTiming->pMode->RefreshRate;
    TargetMode.PixelClock = pGetModeTiming->pMode->PixelClock;
    TargetMode.bInterlace = pGetModeTiming->pMode->isInterlaceMode;

    // 2. Get detail timing
    cbMode_GetHVTiming(pcbe, &TargetMode, pGetModeTiming->DeviceId, pTiming);

    Status = CBIOS_OK;

END:
    return Status;
}

CBIOS_STATUS cbDevGetModeFromReg(PCBIOS_VOID pvcbe, PCBIOS_DEVICE_COMMON pDevCommon, PCBIOS_GETTING_MODE_PARAMS pModeParams)
{
    PCBIOS_EXTENSION_COMMON  pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    CBIOS_STATUS             csRet = CBIOS_OK;
    CBIOS_U32                IGAIndex = IGA1;
    CBIOS_TIMING_ATTRIB      timing_reg = {0};
    CBIOS_TIMING_FLAGS       flags = {0};

#ifdef CHECK_CHIPENABLE
    if (!cbHWIsChipEnable(pcbe))
    {
        csRet = CBIOS_ER_CHIPDISABLE;
        return csRet;
    }
#endif

    cbGetModeInfoFromReg(pcbe, pModeParams->Device, &timing_reg, &flags, IGAIndex, TIMING_REG_TYPE_CR);

    pModeParams->IGAIndex = IGAIndex;
    pModeParams->DestModeParams.XRes = timing_reg.XRes;
    pModeParams->DestModeParams.YRes = timing_reg.YRes;
    pModeParams->DestModeParams.RefreshRate = timing_reg.RefreshRate;
    pModeParams->DestModeParams.InterlaceFlag = (flags.IsInterlace == 0) ? 0 : 1;

    cb_memcpy(&(pModeParams->DetailedTiming), &timing_reg, sizeof(CBIOS_TIMING_ATTRIB));
    return csRet;
}

CBIOS_BOOL cbDevDeviceDetect(PCBIOS_VOID pvcbe, PCBIOS_DEVICE_COMMON pDevCommon, PCBIOS_DETECT_FLAG pDetectFlag)
{
    PCBIOS_EXTENSION_COMMON pcbe   = (PCBIOS_EXTENSION_COMMON)pvcbe;
    CBIOS_BOOL  bConnected = CBIOS_FALSE;

    if (!pDevCommon->pfncbDeviceDetect)
    {
        cbDebugPrint((MAKE_LEVEL(GENERIC, WARNING), "%s: current device: 0x%x has no detect func.\n", FUNCTION_NAME, pDevCommon->DeviceType));
    }
    else if(!pDetectFlag)
    {
        cbDebugPrint((MAKE_LEVEL(GENERIC, WARNING), "%s: Invalid detect flag.\n", FUNCTION_NAME));
    }
    else
    {
        bConnected = pDevCommon->pfncbDeviceDetect(pcbe, pDevCommon, pDetectFlag);
    }

    return bConnected;
}

CBIOS_STATUS cbDevGetEdidFromBuffer(PCBIOS_VOID pvcbe, PCBIOS_DEVICE_COMMON pDevCommon, PCBIOS_PARAM_GET_EDID pCBParamGetEdid)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    CBIOS_STATUS bRetStatus = CBIOS_ER_INTERNAL;
    CBIOS_ACTIVE_TYPE Device = pCBParamGetEdid->DeviceId;
    CBIOS_U32 EdidLen = 0;

    if(cbEDIDModule_IsEDIDValid(&pDevCommon->EdidData))
    {
        if (pCBParamGetEdid->EdidBuffer && pCBParamGetEdid->EdidBufferLen)
        {
            EdidLen = cb_min(pCBParamGetEdid->EdidBufferLen, pDevCommon->EdidData.BufferSize);
            if (pCBParamGetEdid->EdidBufferLen > EdidLen)
            {
                cb_memset(pCBParamGetEdid->EdidBuffer, 0, pCBParamGetEdid->EdidBufferLen);
            }
            cb_memcpy(pCBParamGetEdid->EdidBuffer, pDevCommon->EdidData.Buffer, EdidLen);
            pCBParamGetEdid->EdidLen = EdidLen;
            bRetStatus = CBIOS_OK;
        }
        else
        {
            pCBParamGetEdid->EdidLen = pDevCommon->EdidData.BufferSize;
            bRetStatus = CBIOS_OK;
        }
    }
    else
    {
        bRetStatus = CBIOS_ER_EDID_INVALID;
        pCBParamGetEdid->EdidLen = 0;
        cbDebugPrint((MAKE_LEVEL(GENERIC, INFO), "%s: No valid EDID for Device 0x%x!\n", FUNCTION_NAME, Device));
    }

    return bRetStatus;
}

CBIOS_STATUS cbDevSetEdid(PCBIOS_VOID pvcbe, PCBIOS_DEVICE_COMMON pDevCommon, PCBIOS_PARAM_SET_EDID pCBParamSetEdid)
{
    CBIOS_STATUS            status  = CBIOS_OK;

    // When the EdidBuffer == NULL, we unlock the Edid buffer in CBIOS
    if((pCBParamSetEdid->EdidData.Buffer == CBIOS_NULL) || (pCBParamSetEdid->EdidData.BufferSize == 0))
    {
        cbClearEdidData(&pDevCommon->FakeEdid);
        pDevCommon->isFakeEdid = CBIOS_FALSE;
        return CBIOS_OK;
    }

    if(!cbEDIDModule_IsEDIDValid(&pCBParamSetEdid->EdidData))
    {
        cbDebugPrint((MAKE_LEVEL(GENERIC, ERROR), "%s: EDID is invalid\n", FUNCTION_NAME));
        status = CBIOS_ER_INVALID_PARAMETER;
    }
    else
    {
        if (cbCopyBuffer2EdidData(&pDevCommon->FakeEdid, pCBParamSetEdid->EdidData.Buffer, pCBParamSetEdid->EdidData.BufferSize))
        {
            pDevCommon->isFakeEdid = CBIOS_TRUE;
        }
        else
        {
            status = CBIOS_ER_NO_ENOUGH_MEM;
        }
    }

    return status;
}

CBIOS_STATUS cbDevQueryMonitorAttribute(PCBIOS_VOID pvcbe, PCBIOS_DEVICE_COMMON pDevCommon, PCBiosMonitorAttribute pMonitorAttribute)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;

    if (!cbEDIDModule_IsEDIDValid(&pDevCommon->EdidData))
    {
        cbDebugPrint((MAKE_LEVEL(GENERIC, DEBUG), "%s: EDID is invalid\n", FUNCTION_NAME));
        cb_memset(pMonitorAttribute, 0, sizeof(CBiosMonitorAttribute));
        pMonitorAttribute->MonitorType = pDevCommon->CurrentMonitorType;
    }
    else 
    {
        //get monitor chromaticity x,y coordinates and default white point
        cb_memcpy(&pMonitorAttribute->Chromaticity, &pDevCommon->EdidStruct.Attribute.Chromaticity, sizeof(CBIOS_MONITOR_CHROMATICITY));
        /*
        ** 1. get basic monitor attributes
        */
        pMonitorAttribute->MonitorType = pDevCommon->CurrentMonitorType;
        pMonitorAttribute->bSupportCEA861 = pDevCommon->EdidStruct.Attribute.IsCEA861Monitor;
        pMonitorAttribute->bSupportRGB = pDevCommon->EdidStruct.Attribute.IsCEA861RGB;
        pMonitorAttribute->bSupportYCbCr422 = pDevCommon->EdidStruct.Attribute.IsCEA861YCbCr422;
        pMonitorAttribute->bSupportYCbCr444 = pDevCommon->EdidStruct.Attribute.IsCEA861YCbCr444;
        pMonitorAttribute->bSupportHDAudio = pDevCommon->EdidStruct.Attribute.IsCEA861Audio;
        pMonitorAttribute->bSupportBLCtrl = CBIOS_FALSE;
        pMonitorAttribute->bSupportPsr = CBIOS_FALSE;
        pMonitorAttribute->SupportBPC.IsSupport8BPC = CBIOS_TRUE;
        
        // get monitor screen image size
        pMonitorAttribute->MonitorHorSize = pDevCommon->EdidStruct.Attribute.MonitorHorSize;
        pMonitorAttribute->MonitorVerSize = pDevCommon->EdidStruct.Attribute.MonitorVerSize;

        cb_memcpy(pMonitorAttribute->MonitorID, pDevCommon->EdidStruct.Attribute.MonitorID, 8);

        /*
        ** 2. get special monitor attributes
        */
        if (pDevCommon->pfncbQueryMonitorAttribute)
        {
            pDevCommon->pfncbQueryMonitorAttribute(pcbe, pDevCommon, pMonitorAttribute);
        }
    }
    return CBIOS_OK;
}

CBIOS_STATUS cbDevQueryMonitor3DCapability(PCBIOS_VOID pvcbe, PCBIOS_DEVICE_COMMON pDevCommon, PCBIOS_MONITOR_3D_CAPABILITY_PARA p3DCapability)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    CBIOS_U32 Status = CBIOS_OK;
    
    if (!cbEDIDModule_IsEDIDValid(&pDevCommon->EdidData))
    {
        cbDebugPrint((MAKE_LEVEL(GENERIC, INFO), "%s: EDID is invalid\n", FUNCTION_NAME));
        cb_memset(p3DCapability, 0, sizeof(CBIOS_MONITOR_3D_CAPABILITY_PARA));
    }
    else
    {
        Status = cbEDIDModule_GetMonitor3DCaps(&(pDevCommon->EdidStruct), p3DCapability, pcbe->ChipCaps.IsSupport3DVideo);
    }

    return Status;
}

CBIOS_STATUS cbGetDeviceName(PCBIOS_VOID pvcbe,   PCBIOS_GET_DEVICE_NAME  pGetName)
{
    CBIOS_U32  Status = CBIOS_OK;
    CBIOS_ACTIVE_TYPE  DeviceType = (CBIOS_ACTIVE_TYPE)pGetName->DeviceId;
    
    switch(DeviceType)
    {
    case  CBIOS_TYPE_CRT:
        pGetName->pDeviceName = "CRT1";
        break;
    case CBIOS_TYPE_TV:
        pGetName->pDeviceName = "TV";
        break;
    case CBIOS_TYPE_HDTV:
        pGetName->pDeviceName = "HDTV";
        break;
    case CBIOS_TYPE_DP1:
        pGetName->pDeviceName = "DP1";
        break;
    case CBIOS_TYPE_DP2:
        pGetName->pDeviceName = "DP2";
        break;
    case CBIOS_TYPE_DP3:
        pGetName->pDeviceName = "DP3";
        break;
    case CBIOS_TYPE_DP4:
        pGetName->pDeviceName = "DP4";
        break;
    case  CBIOS_TYPE_DVO:
        pGetName->pDeviceName = "DVO";
        break;
    default:
        cbDebugPrint((MAKE_LEVEL(GENERIC, WARNING), "Invalid device type 0x%x in query name.\n", DeviceType));
        pGetName->pDeviceName = "NONE";
        break;
    }

    return  Status;
}

CBIOS_STATUS cbDevGetDeviceModeList(PCBIOS_VOID pvcbe, PCBIOS_DEVICE_COMMON pDevCommon, PCBiosModeInfoExt pModeList, CBIOS_U32 *pBufferSize)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    CBIOS_U32 ulModeNum = 0;

    if(!pModeList || *pBufferSize < sizeof(CBiosModeInfoExt))
    {
        *pBufferSize = 0;
        return CBIOS_ER_INVALID_PARAMETER;
    }

    if(pDevCommon->ValidModeNum && pDevCommon->pDeviceModeList)
    {
        ulModeNum = cb_min(pDevCommon->ValidModeNum, *pBufferSize/sizeof(CBiosModeInfoExt));
        cb_memcpy(pModeList, pDevCommon->pDeviceModeList, ulModeNum * sizeof(CBiosModeInfoExt));
    }
    else
    {
        ulModeNum = cbMode_GetDefaultModeList(pcbe, CBIOS_NULL, pDevCommon->DeviceType);
        if(ulModeNum <= *pBufferSize/sizeof(CBiosModeInfoExt))
        {
            cbMode_GetDefaultModeList(pcbe, pModeList, pDevCommon->DeviceType);
        }
        else
        {
            ulModeNum = 0;
        }
    }

    *pBufferSize = ulModeNum * sizeof(CBiosModeInfoExt);

    return CBIOS_OK;
}

CBIOS_STATUS cbDevGetDeviceModeListBufferSize(PCBIOS_VOID pvcbe, PCBIOS_DEVICE_COMMON pDevCommon, CBIOS_U32 *pBufferSize)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    CBIOS_U32 ulNumOfMode = 0;
    
    ulNumOfMode = pDevCommon->ValidModeNum;

    if(ulNumOfMode == 0)
    {
        ulNumOfMode = cbMode_GetDefaultModeList(pcbe, CBIOS_NULL, pDevCommon->DeviceType);
    }

    *pBufferSize = ulNumOfMode * sizeof(CBiosModeInfoExt);

    return CBIOS_OK;
}

CBIOS_U32 cbDevGetHDAFormatList(PCBIOS_VOID pvcbe, PCBIOS_DEVICE_COMMON pDevCommon, PCBIOS_HDMI_AUDIO_INFO pHDAFormatList)
{
    CBIOS_U32 HDAFormatNum = 0;

    if ((pDevCommon->EdidStruct.Attribute.IsCEA861Audio) || (pDevCommon->EdidStruct.HDAudioFormatNum != 0))
    {
        HDAFormatNum = pDevCommon->EdidStruct.HDAudioFormatNum;
        if (pHDAFormatList != CBIOS_NULL)
        {
            cb_memcpy(pHDAFormatList, pDevCommon->EdidStruct.HDAudioFormat, HDAFormatNum * sizeof(CBIOS_HDMI_AUDIO_INFO));
        }
    }
    else
    {
        cbDebugPrint((MAKE_LEVEL(GENERIC, WARNING), "%s: Device:0x%x doesn't support HD audio!\n", FUNCTION_NAME, pDevCommon->DeviceType));
        HDAFormatNum = 0;
    }

    return HDAFormatNum;
}

CBIOS_STATUS cbDevSetDisplayDevicePowerState(PCBIOS_VOID pvcbe, PCBIOS_DEVICE_COMMON pDevCommon, CBIOS_BOOL bPowerOn, CBIOS_U32 Flags)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;

    if (pcbe->bNeedBgaPatch && bPowerOn)
    {
        cbDebugPrint((MAKE_LEVEL(GENERIC, INFO), "%s: Need BGA Patch, skip device power on.\n", FUNCTION_NAME));
        return CBIOS_OK;
    }

    if (pDevCommon->pfncbDeviceOnOff)
    {
        pDevCommon->pfncbDeviceOnOff(pcbe, pDevCommon, bPowerOn, Flags);
    }
    else
    {
        cbDebugPrint((MAKE_LEVEL(GENERIC, WARNING), "%s: current device: 0x%x has no func to turn on/off.\n", FUNCTION_NAME, pDevCommon->DeviceType));
    }

    return CBIOS_OK;
}

CBIOS_STATUS cbDevGetDisplayDevicePowerState(PCBIOS_VOID pvcbe, PCBIOS_DEVICE_COMMON pDevCommon, PCBIOS_BOOL pIsPowerOn)
{
    if(pDevCommon->PowerState == CBIOS_PM_ON)
    {
        *pIsPowerOn = CBIOS_TRUE;
    }
    else
    {
        *pIsPowerOn = CBIOS_FALSE;
    }

    return CBIOS_OK;
}

CBIOS_STATUS cbDevDSISendWriteCmd(PCBIOS_VOID pvcbe, PCBIOS_DEVICE_COMMON pDevCommon, PCBIOS_DSI_WRITE_PARA_INTERNAL pDSIWriteParams)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    return cbDSI_SendWriteCmd(pcbe, pDSIWriteParams);
}

CBIOS_STATUS cbDevDSISendReadCmd(PCBIOS_VOID pvcbe, PCBIOS_DEVICE_COMMON pDevCommon, PCBIOS_DSI_READ_PARA_INTERNAL pDSIReadParams)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    return cbDSI_SendReadCmd(pcbe, pDSIReadParams);
}

CBIOS_STATUS cbDevDSIDisplayUpdate(PCBIOS_VOID pvcbe, PCBIOS_DEVICE_COMMON pDevCommon, PCBIOS_DSI_UPDATE_PARA pDSIUpdatePara)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    return cbDSI_DisplayUpdate(pcbe, pDSIUpdatePara);
}

CBIOS_STATUS cbDevDSIPanelSetCabc(PCBIOS_VOID pvcbe, PCBIOS_DEVICE_COMMON pDevCommon, CBIOS_U32 CabcValue)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    PCBIOS_DSI_PANEL_DESC pDSIPanelDesc = &(pDevCommon->DeviceParas.DSIDevice.DSIPanelDesc);
    
    return cbDSIPanel_SetCabc(pcbe, CabcValue, pDSIPanelDesc);
}

CBIOS_STATUS cbDevDSIPanelSetBacklight(PCBIOS_VOID pvcbe, PCBIOS_DEVICE_COMMON pDevCommon, CBIOS_U32 BacklightValue)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    PCBIOS_DSI_PANEL_DESC pDSIPanelDesc = &(pDevCommon->DeviceParas.DSIDevice.DSIPanelDesc);

    return cbDSIPanel_SetBacklight(pcbe, BacklightValue, pDSIPanelDesc);
}

CBIOS_STATUS cbDevDSIPanelGetBacklight(PCBIOS_VOID pvcbe, PCBIOS_DEVICE_COMMON pDevCommon, CBIOS_U32 *pBacklightValue)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    PCBIOS_DSI_PANEL_DESC pDSIPanelDesc = &(pDevCommon->DeviceParas.DSIDevice.DSIPanelDesc);

    return cbDSIPanel_GetBacklight(pcbe, pBacklightValue, pDSIPanelDesc);
}

CBIOS_STATUS cbDevSetHDACodecPara(PCBIOS_VOID pvcbe, PCBIOS_DEVICE_COMMON pDevCommon, PCBIOS_HDAC_PARA pCbiosHDACPara)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;

    cbDIU_HDAC_SetHDACodecPara(pcbe, pCbiosHDACPara);

    return CBIOS_OK;
}

CBIOS_STATUS cbDevSetHDACConnectStatus(PCBIOS_VOID pvcbe, PCBIOS_DEVICE_COMMON pDevCommon, PCBIOS_HDAC_PARA pCbiosHDACPara)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;

    cbDIU_HDAC_SetConnectStatus(pcbe, pCbiosHDACPara);

    return CBIOS_OK;
}


CBIOS_STATUS cbDevContentProtectionOnOff(PCBIOS_VOID pvcbe, PCBIOS_DEVICE_COMMON pDevCommon, CBIOS_U8 IGAIndex, PCBiosContentProtectionOnOffParams pContentProtectionOnOffParams)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    CBIOS_STATUS Status = CBIOS_ER_INVALID_PARAMETER;
    CBIOS_MODULE_INDEX HDCPModuleIndex = CBIOS_MODULE_INDEX_INVALID;

    HDCPModuleIndex = cbGetModuleIndex(pcbe, pContentProtectionOnOffParams->DevicesId, CBIOS_MODULE_TYPE_HDCP);
    if (HDCPModuleIndex != CBIOS_MODULE_INDEX_INVALID)
    {
        cbHDCP_OnOff(pcbe, pContentProtectionOnOffParams->DevicesId, IGAIndex, pContentProtectionOnOffParams->bHdcpStatus);
        Status = CBIOS_OK;
    }
    else
    {
        cbDebugPrint((MAKE_LEVEL(GENERIC, ERROR), "%s: invalid HDCP module index!\n", FUNCTION_NAME));
        Status = CBIOS_ER_INVALID_PARAMETER;
    }
    return Status;
}

CBIOS_STATUS cbDevGetHDCPStatus(PCBIOS_VOID pvcbe, PCBIOS_DEVICE_COMMON pDevCommon, PCBIOS_HDCP_STATUS_PARA pCBiosHdcpStatusParams)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    CBIOS_STATUS Status = CBIOS_ER_INVALID_PARAMETER;
    CBIOS_MODULE_INDEX HDCPModuleIndex = CBIOS_MODULE_INDEX_INVALID;

    HDCPModuleIndex = cbGetModuleIndex(pcbe, pCBiosHdcpStatusParams->DevicesId, CBIOS_MODULE_TYPE_HDCP);
    if (HDCPModuleIndex != CBIOS_MODULE_INDEX_INVALID)
    {
        pCBiosHdcpStatusParams->HdcpStatus = cbHDCP_GetStatus(pcbe, pCBiosHdcpStatusParams->DevicesId);
        cbHDCP_GetHDCPBKsv(pcbe, pCBiosHdcpStatusParams->DevicesId, pCBiosHdcpStatusParams->BKsv, &pCBiosHdcpStatusParams->bRepeater);
        Status = CBIOS_OK;
    }
    else
    {
        cbDebugPrint((MAKE_LEVEL(GENERIC, ERROR), "%s: device:0x%x doesn't support HDCP!\n", FUNCTION_NAME, pCBiosHdcpStatusParams->DevicesId));
        Status = CBIOS_ER_INVALID_PARAMETER;
    }
    return Status;
}

CBIOS_STATUS cbDevHDCPWorkThread(PCBIOS_VOID pvcbe, PCBIOS_DEVICE_COMMON pDevCommon, CBIOS_U8 IGAIndex, PCBIOS_HDCP_WORK_PARA pCBiosHdcpWorkParams)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    CBIOS_STATUS Status = CBIOS_ER_INVALID_PARAMETER;
    CBIOS_MODULE_INDEX HDCPModuleIndex = CBIOS_MODULE_INDEX_INVALID;

    HDCPModuleIndex = cbGetModuleIndex(pcbe, pCBiosHdcpWorkParams->DevicesId, CBIOS_MODULE_TYPE_HDCP);
    if (HDCPModuleIndex != CBIOS_MODULE_INDEX_INVALID)
    {
        cbHDCP_WorkThread(pcbe, pCBiosHdcpWorkParams->DevicesId, IGAIndex);
        Status = CBIOS_OK;
    }
    else
    {
        cbDebugPrint((MAKE_LEVEL(GENERIC, ERROR), "%s: device:0x%x doesn't support HDCP!\n", FUNCTION_NAME, pCBiosHdcpWorkParams->DevicesId));
        Status = CBIOS_ER_INVALID_PARAMETER;
    }
    return Status;
}

CBIOS_STATUS cbDevHDCPIsr(PCBIOS_VOID pvcbe, PCBIOS_DEVICE_COMMON pDevCommon, PCBIOS_HDCP_ISR_PARA pHdcpIsrParam)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    CBIOS_STATUS Status = CBIOS_ER_INVALID_PARAMETER;
    CBIOS_MODULE_INDEX HDCPModuleIndex = CBIOS_MODULE_INDEX_INVALID;

    HDCPModuleIndex = cbGetModuleIndex(pcbe, pHdcpIsrParam->DevicesId, CBIOS_MODULE_TYPE_HDCP);
    if (HDCPModuleIndex != CBIOS_MODULE_INDEX_INVALID)
    {
        cbHDCP_Isr(pcbe, pHdcpIsrParam->DevicesId);
        Status = CBIOS_OK;
    }
    else
    {
        cbDebugPrint((MAKE_LEVEL(GENERIC, ERROR), "%s: device:0x%x doesn't support HDCP!\n", FUNCTION_NAME, pHdcpIsrParam->DevicesId));
        Status = CBIOS_ER_INVALID_PARAMETER;
    }
    return Status;
}

CBIOS_BOOL cbDevHdmiSCDCWorkThreadMainFunc(PCBIOS_VOID pvcbe, PCBIOS_DEVICE_COMMON pDevCommon)
{
    CBIOS_BOOL bRet = CBIOS_FALSE;
    
    bRet = cbHDMIMonitor_SCDC_Handler(pvcbe, pDevCommon);
    
    return bRet;
}

CBIOS_STATUS cbDevDPIsr(PCBIOS_VOID pvcbe, PCBIOS_DEVICE_COMMON pDevCommon, PCBIOS_DP_ISR_PARA pDPIsrPara)
{
    return cbDPPort_Isr(pvcbe, pDevCommon, pDPIsrPara);
}

CBIOS_BOOL cbDevDPWorkThreadMainFunc(PCBIOS_VOID pvcbe, PCBIOS_DEVICE_COMMON pDevCommon, PCBIOS_DP_WORKTHREAD_PARA pDPWorkThreadPara)
{
    return cbDPPort_WorkThreadMainFunc(pvcbe, pDevCommon, pDPWorkThreadPara);
}

CBIOS_STATUS cbDevDPSetNotifications(PCBIOS_VOID pvcbe, PCBIOS_DEVICE_COMMON pDevCommon, PCBIOS_DP_NOTIFICATIONS pDPNotifications)
{
    return cbDPPort_SetNotifications(pvcbe, pDevCommon, pDPNotifications);
}

CBIOS_STATUS  cbDevGetDPIntType(PCBIOS_VOID pvcbe, PCBIOS_DEVICE_COMMON pDevCommon, PCBIOS_DP_INT_PARA pDpIntPara)
{
    return  cbDPPort_GetInt(pvcbe, pDevCommon, pDpIntPara);
}

CBIOS_STATUS  cbDevDPHandleIrq(PCBIOS_VOID pvcbe, PCBIOS_DEVICE_COMMON pDevCommon, PCBIOS_DP_HANDLE_IRQ_PARA pDPHandleIrqPara)
{
    return  cbDPPort_HandleIrq(pvcbe, pDevCommon, pDPHandleIrqPara);
}

CBIOS_STATUS cbDevDPGetCustomizedTiming(PCBIOS_VOID pvcbe, PCBIOS_DEVICE_COMMON pDevCommon, PCBIOS_DP_CUSTOMIZED_TIMING pDPCustomizedTiming)
{
    return cbDPPort_GetCustomizedTiming(pvcbe, pDevCommon, pDPCustomizedTiming);
}

CBIOS_BOOL cbDevPsrOp0_1(PCBIOS_VOID pvcbe, CBIOS_U32 Device)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    CBIOS_MODULE_INDEX DPModuleIndex = cbGetModuleIndex(pcbe, Device, CBIOS_MODULE_TYPE_DP);

    return cbDIU_EDP_PsrOp0_1(pcbe, DPModuleIndex);
}
CBIOS_STATUS cbDevPsrOp1_0(PCBIOS_VOID pvcbe, CBIOS_U32 Device)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    CBIOS_MODULE_INDEX DPModuleIndex = cbGetModuleIndex(pcbe, Device, CBIOS_MODULE_TYPE_DP);

    cbDIU_EDP_PsrOp1_0(pcbe, DPModuleIndex);
    return CBIOS_OK;
}
CBIOS_STATUS cbDevPsrOp1_2(PCBIOS_VOID pvcbe, CBIOS_U32 Device)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    CBIOS_MODULE_INDEX DPModuleIndex = cbGetModuleIndex(pcbe, Device, CBIOS_MODULE_TYPE_DP);

    cbDIU_EDP_PsrOp1_2(pcbe, DPModuleIndex);
    return CBIOS_OK;
}
CBIOS_STATUS cbDevPsrOp2_0(PCBIOS_VOID pvcbe, CBIOS_U32 Device)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    CBIOS_MODULE_INDEX DPModuleIndex = cbGetModuleIndex(pcbe, Device, CBIOS_MODULE_TYPE_DP);

    cbDIU_EDP_PsrOp2_0(pcbe, DPModuleIndex);
    return CBIOS_OK;
}
CBIOS_BOOL cbDevPsrOp2_3(PCBIOS_VOID pvcbe, CBIOS_U32 Device)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    CBIOS_MODULE_INDEX DPModuleIndex = cbGetModuleIndex(pcbe, Device, CBIOS_MODULE_TYPE_DP);

    return cbDIU_EDP_PsrOp2_3(pcbe, DPModuleIndex);;
}
CBIOS_STATUS cbDevPsrOp2_5(PCBIOS_VOID pvcbe, CBIOS_U32 Device)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    CBIOS_MODULE_INDEX DPModuleIndex = cbGetModuleIndex(pcbe, Device, CBIOS_MODULE_TYPE_DP);

    cbDIU_EDP_PsrOp2_5(pcbe, DPModuleIndex);
    return CBIOS_OK;
}
CBIOS_STATUS cbDevPsrOp3_0(PCBIOS_VOID pvcbe, CBIOS_U32 Device)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    CBIOS_MODULE_INDEX DPModuleIndex = cbGetModuleIndex(pcbe, Device, CBIOS_MODULE_TYPE_DP);

    cbDIU_EDP_PsrOp3_0(pcbe, DPModuleIndex);
    return CBIOS_OK;
}
CBIOS_STATUS cbDevPsrOp3_1(PCBIOS_VOID pvcbe, CBIOS_U32 Device)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    CBIOS_MODULE_INDEX DPModuleIndex = cbGetModuleIndex(pcbe, Device, CBIOS_MODULE_TYPE_DP);

    cbDIU_EDP_PsrOp3_1(pcbe, DPModuleIndex);
    return CBIOS_OK;
}
CBIOS_STATUS cbDevPsrOp3_5(PCBIOS_VOID pvcbe, CBIOS_U32 Device)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    CBIOS_MODULE_INDEX DPModuleIndex = cbGetModuleIndex(pcbe, Device, CBIOS_MODULE_TYPE_DP);

    cbDIU_EDP_PsrOp3_5(pcbe, DPModuleIndex);
    return CBIOS_OK;
}
CBIOS_STATUS cbDevPsrOp5_0(PCBIOS_VOID pvcbe, CBIOS_U32 Device)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    CBIOS_MODULE_INDEX DPModuleIndex = cbGetModuleIndex(pcbe, Device, CBIOS_MODULE_TYPE_DP);

    cbDIU_EDP_PsrOp5_0(pcbe, DPModuleIndex);
    return CBIOS_OK;
}
CBIOS_BOOL cbDevPsrOp5_1(PCBIOS_VOID pvcbe, CBIOS_U32 Device)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    CBIOS_MODULE_INDEX DPModuleIndex = cbGetModuleIndex(pcbe, Device, CBIOS_MODULE_TYPE_DP);

    return cbDIU_EDP_PsrOp5_1(pcbe, DPModuleIndex);
}
CBIOS_STATUS cbDevPsrOp3_3_1(PCBIOS_VOID pvcbe, CBIOS_U32 Device)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    CBIOS_MODULE_INDEX DPModuleIndex = cbGetModuleIndex(pcbe, Device, CBIOS_MODULE_TYPE_DP);

    cbDIU_EDP_PsrOp3_3_1(pcbe, DPModuleIndex);
    return CBIOS_OK;
}
CBIOS_STATUS cbDevPsrOp3_1_3_2(PCBIOS_VOID pvcbe, CBIOS_U32 Device)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    CBIOS_MODULE_INDEX DPModuleIndex = cbGetModuleIndex(pcbe, Device, CBIOS_MODULE_TYPE_DP);

    cbDIU_EDP_PsrOp3_1_3_2(pcbe, DPModuleIndex);
    return CBIOS_OK;
}
CBIOS_STATUS cbDevPsrOp3_2_3_3(PCBIOS_VOID pvcbe, CBIOS_U32 Device)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    CBIOS_MODULE_INDEX DPModuleIndex = cbGetModuleIndex(pcbe, Device, CBIOS_MODULE_TYPE_DP);

    cbDIU_EDP_PsrOp3_2_3_3(pcbe, DPModuleIndex);
    return CBIOS_OK;
}
CBIOS_STATUS cbDevPsrOp3_3_3_2(PCBIOS_VOID pvcbe, CBIOS_U32 Device)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    CBIOS_MODULE_INDEX DPModuleIndex = cbGetModuleIndex(pcbe, Device, CBIOS_MODULE_TYPE_DP);

    cbDIU_EDP_PsrOp3_3_3_2(pcbe, DPModuleIndex);
    return CBIOS_OK;
}
CBIOS_STATUS cbDevPsrOp3_2_3_1(PCBIOS_VOID pvcbe, CBIOS_U32 Device)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    CBIOS_MODULE_INDEX DPModuleIndex = cbGetModuleIndex(pcbe, Device, CBIOS_MODULE_TYPE_DP);

    cbDIU_EDP_PsrOp3_2_3_1(pcbe, DPModuleIndex);
    return CBIOS_OK;
}
CBIOS_STATUS cbDevPsrOp3_1_3(PCBIOS_VOID pvcbe, CBIOS_U32 Device)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    CBIOS_MODULE_INDEX DPModuleIndex = cbGetModuleIndex(pcbe, Device, CBIOS_MODULE_TYPE_DP);

    cbDIU_EDP_PsrOp3_1_3(pcbe, DPModuleIndex);
    return CBIOS_OK;
}

CBIOS_STATUS cbDevAux_DataAccess(PCBIOS_VOID pvcbe, PCBIOS_PARAM_AUX_DATA pCBParamAuxData)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    CBIOS_STATUS Status = CBIOS_ER_INVALID_PARAMETER;
    CBIOS_BOOL   bRet = CBIOS_FALSE;
    CBIOS_MODULE_INDEX DPModuleIndex = cbGetModuleIndex(pcbe, pCBParamAuxData->DeviceId, CBIOS_MODULE_TYPE_DP);
    CBIOS_PARAM_AUX_DATA TempAuxData = {0};
    CBIOS_U8 SegNum = 0;
    CBIOS_U8  TestData[1] = {0};

#ifdef CHECK_CHIPENABLE
    if (!cbHWIsChipEnable(pcbe))
        return CBIOS_ER_CHIPDISABLE;
#endif

    if(DPModuleIndex == CBIOS_MODULE_INDEX_INVALID)
    {
        cbDebugPrint((MAKE_LEVEL(DP, ERROR), "%s: invalid DP module index!\n", FUNCTION_NAME));
        return Status;
    }

    //read a byte to test whether aux can work normally
    if(cbDIU_Aux_Dpcd_Read(pcbe, DPModuleIndex, 0, TestData, 1) == CBIOS_FALSE)
    {
        return Status;
    }

    if(pCBParamAuxData->NativeAux)
    {
        if(pCBParamAuxData->Write)
        {
            bRet = cbDIU_Aux_Dpcd_Write(pcbe, DPModuleIndex, pCBParamAuxData->Address, pCBParamAuxData->Buffer, pCBParamAuxData->BytesRequested);
        }
        else
        {
            bRet = cbDIU_Aux_Dpcd_Read(pcbe, DPModuleIndex, pCBParamAuxData->Address, pCBParamAuxData->Buffer, pCBParamAuxData->BytesRequested);
        }

        Status = bRet ? CBIOS_OK : CBIOS_ER_INTERNAL;
    }
    else
    {
        if(pCBParamAuxData->Write)
        {
            Status = cbDIU_Aux_I2c_Access(pcbe, DPModuleIndex, CBIOS_AUX_REQUEST_I2C_WRITE, pCBParamAuxData);
        }
        else
        {
            if(pCBParamAuxData->EDDCMode)
            {
                SegNum = pCBParamAuxData->SegNum;
                TempAuxData.EDDCMode = CBIOS_TRUE;
                TempAuxData.BytesRequested = 1;
                TempAuxData.Address = 0x60;
                TempAuxData.Buffer = &SegNum;
                TempAuxData.Offset = 0;
                Status = cbDIU_Aux_I2c_Access(pcbe, DPModuleIndex, CBIOS_AUX_REQUEST_I2C_WRITE, &TempAuxData);
                pCBParamAuxData->EDDCMode = 0;//after write segNum, should set EDDCMode flag to 0 for edid read
                if(Status != CBIOS_OK)
                {
                    cbDebugPrint((MAKE_LEVEL(DP, ERROR),"%s: Aux write EDDC SegNum failed!\n",FUNCTION_NAME));
                    return CBIOS_ER_INTERNAL;
                }
            }

            Status = cbDIU_Aux_I2c_Access(pcbe, DPModuleIndex, CBIOS_AUX_REQUEST_I2C_READ, pCBParamAuxData);
        }
    }

    return Status;
}

CBIOS_STATUS cbDevI2C_ReadData(PCBIOS_VOID pvcbe, PCBIOS_PARAM_I2C_DATA pCBParamI2CData)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    CBIOS_STATUS Status = CBIOS_ER_INVALID_PARAMETER;
    PCBIOS_DEVICE_COMMON pDevCommon = cbGetDeviceCommon(&pcbe->DeviceMgr, pCBParamI2CData->DeviceId);
    CBIOS_MODULE_I2C_PARAMS I2CParams;

    if(pDevCommon == CBIOS_NULL)
    {
        return Status;
    }

#ifdef CHECK_CHIPENABLE
    if (!cbHWIsChipEnable(pcbe))
        return CBIOS_ER_CHIPDISABLE;
#endif

    cb_memset(&I2CParams, 0, sizeof(CBIOS_MODULE_I2C_PARAMS));

    I2CParams.I2CBusNum = (CBIOS_U8)pDevCommon->I2CBus;
    I2CParams.I2CDelay = pDevCommon->I2CDelay;
    I2CParams.SlaveAddress = pCBParamI2CData->SlaveAddress;
    I2CParams.OffSet = pCBParamI2CData->OffSet;
    I2CParams.BufferLen = pCBParamI2CData->BufferLen;
    I2CParams.Buffer = pCBParamI2CData->Buffer;
    
    Status = cbI2C_ReadData(pcbe, &I2CParams);

    return Status;

}

CBIOS_STATUS cbDevI2C_WriteData(PCBIOS_VOID pvcbe, PCBIOS_PARAM_I2C_DATA pCBParamI2CData)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    CBIOS_STATUS Status = CBIOS_ER_INVALID_PARAMETER;
    PCBIOS_DEVICE_COMMON pDevCommon = cbGetDeviceCommon(&pcbe->DeviceMgr, pCBParamI2CData->DeviceId);
    CBIOS_MODULE_I2C_PARAMS I2CParams;

    if(pDevCommon == CBIOS_NULL)
    {
        return Status;
    }

#ifdef CHECK_CHIPENABLE
    if (!cbHWIsChipEnable(pcbe))
        return CBIOS_ER_CHIPDISABLE;
#endif

    cb_memset(&I2CParams, 0, sizeof(CBIOS_MODULE_I2C_PARAMS));

    I2CParams.I2CBusNum = (CBIOS_U8)pDevCommon->I2CBus;
    I2CParams.I2CDelay = pDevCommon->I2CDelay;
    I2CParams.SlaveAddress = pCBParamI2CData->SlaveAddress;
    I2CParams.OffSet = pCBParamI2CData->OffSet;
    I2CParams.BufferLen = pCBParamI2CData->BufferLen;
    I2CParams.Buffer = pCBParamI2CData->Buffer;

    Status = cbI2C_WriteData(pcbe, &I2CParams);

    return Status;
}

CBIOS_STATUS cbDevEDPSetBacklight(PCBIOS_VOID pvcbe, PCBIOS_DEVICE_COMMON pDevCommon, PCBIOS_BACKLIGHT_PARA pBacklight_para)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    PCBIOS_DP_CONTEXT     pDpContext = CBIOS_NULL;
    PCBIOS_EDP_PANEL_DESC   pEDPPanelDesc = CBIOS_NULL;
    CBIOS_STATUS Status = CBIOS_ER_INVALID_PARAMETER;
    CBIOS_MODULE_INDEX DPModuleIndex = CBIOS_MODULE_INDEX_INVALID;

    DPModuleIndex = cbGetModuleIndex(pcbe, pBacklight_para->DeviceId, CBIOS_MODULE_TYPE_DP);
    if (DPModuleIndex == CBIOS_MODULE_INDEX_INVALID)
    {
        cbDebugPrint((MAKE_LEVEL(GENERIC, ERROR), "%s: invalid DP module index!\n", FUNCTION_NAME));
        return CBIOS_ER_INVALID_PARAMETER;
    }

    if(pDevCommon == CBIOS_NULL)
    {
        return CBIOS_ER_NULLPOINTER;
    }

    pDpContext = container_of(pDevCommon, PCBIOS_DP_CONTEXT, Common);
    pEDPPanelDesc = &(pDevCommon->DeviceParas.EDPPanelDevice.EDPPanelDesc);

    if(pDpContext)
    {
        Status = cbEDPPanel_SetBacklight(pcbe, DPModuleIndex, pBacklight_para->BacklightValue, pEDPPanelDesc);
        if(Status == CBIOS_OK)
        {
            pDpContext->DPMonitorContext.bBlEverSetByOS = CBIOS_TRUE;
            pDpContext->DPMonitorContext.BrightnessSetByOS = pBacklight_para->BacklightValue;

        }
    }
    else
    {
        Status = CBIOS_ER_NULLPOINTER;
    }

    return Status;
}

CBIOS_STATUS cbDevEDPGetBacklight(PCBIOS_VOID pvcbe, PCBIOS_DEVICE_COMMON pDevCommon, PCBIOS_BACKLIGHT_PARA pBacklight_para)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    PCBIOS_EDP_PANEL_DESC   pEDPPanelDesc = CBIOS_NULL;
    CBIOS_MODULE_INDEX DPModuleIndex = CBIOS_MODULE_INDEX_INVALID;

    DPModuleIndex = cbGetModuleIndex(pcbe, pBacklight_para->DeviceId, CBIOS_MODULE_TYPE_DP);
    if (DPModuleIndex == CBIOS_MODULE_INDEX_INVALID)
    {
        cbDebugPrint((MAKE_LEVEL(GENERIC, ERROR), "%s: invalid DP module index!\n", FUNCTION_NAME));
        return CBIOS_ER_INVALID_PARAMETER;
    }

    if(pDevCommon == CBIOS_NULL)
    {
        return CBIOS_ER_NULLPOINTER;
    }

    pEDPPanelDesc = &(pDevCommon->DeviceParas.EDPPanelDevice.EDPPanelDesc);

    return cbEDPPanel_GetBacklight(pcbe, DPModuleIndex, &pBacklight_para->BacklightValue, pEDPPanelDesc);
}

CBIOS_U8 cbDevHDACGetCAValue(PCBIOS_VOID pvcbe, CBIOS_ACTIVE_TYPE DeviceType)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    CBIOS_U8 CA_Value = 0;
    PCBIOS_DEVICE_COMMON pDevCommon = cbGetDeviceCommon(&pcbe->DeviceMgr, DeviceType);
    PCBIOS_MONITOR_MISC_ATTRIB pMonitorAttrib = &(pDevCommon->EdidStruct.Attribute);

    //pMonitorAttrib->SpeakerAllocationData
    //  bit |  0  |  1  |  2  |  3  |  4  |  5  |  6
    //-------------------------------------------------
    //      | FLR | LFE | FC  | RLR | RC  | FLRC| RLRC

    if(pMonitorAttrib->IsCEA861Monitor)
    {
        // refer to CEA-861D table 20
        if(pMonitorAttrib->SpeakerAllocationData & 0x02)    // Lower Frequency Effect
        {
            CA_Value |= BIT0;
        }

        if(pMonitorAttrib->SpeakerAllocationData & 0x04)     // Front Center
        {
            CA_Value |= BIT1;
        }

        if((pMonitorAttrib->SpeakerAllocationData & 0x60) == 0)
        {
            if(pMonitorAttrib->SpeakerAllocationData & 0x10)     // Rear Center
            {
                CA_Value |= BIT2;
            }

            if(pMonitorAttrib->SpeakerAllocationData & 0x08)    // Rear Left/Rear Right
            {
                CA_Value |= BIT3;
            }
        }

        if((pMonitorAttrib->SpeakerAllocationData & 0x40) && (pMonitorAttrib->SpeakerAllocationData & 0x08))
        {
            CA_Value |= BIT4;
        }

        if(pMonitorAttrib->SpeakerAllocationData & 0x20)   // Front Left Center/Front Right Center
        {
            CA_Value |= BIT4;
            if(pMonitorAttrib->SpeakerAllocationData & 0x10)
            {
                CA_Value |= BIT3;
            }

            if(pMonitorAttrib->SpeakerAllocationData & 0x08)
            {
                CA_Value |= (BIT2 | BIT3);
            }
        }
    }

    return CA_Value;
}

CBIOS_BOOL cbDevNeedBgaPatch(PCBIOS_VOID pvcbe, CBIOS_U32 SupportDevices)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    PCBIOS_DEVICE_COMMON    pDevCommon = CBIOS_NULL;
    PCBIOS_DP_CONTEXT       pDpContext = CBIOS_NULL;
    CBIOS_MODULE_INDEX      DPModuleIndex = CBIOS_MODULE_INDEX_INVALID;
    CBIOS_DETECT_FLAG       DetectFlag;
    DP_EPHY_MODE            EphyMode;
    CBIOS_U8  Buffer[3] = {0};
    CBIOS_U8  DPCD00070 = 0;
    CBIOS_U32 DevList[] = {CBIOS_TYPE_DP1, CBIOS_TYPE_DP2, CBIOS_TYPE_DP3};
    CBIOS_U32 HImageSize = 0, VImageSize = 0, i;
    CBIOS_BOOL bRet = CBIOS_FALSE;

    for(i = 0; i < sizeofarray(DevList); i++)
    {
        if(SupportDevices & DevList[i])
        {
            pDevCommon = cbGetDeviceCommon(&pcbe->DeviceMgr, DevList[i]);
            pDpContext = container_of(pDevCommon, PCBIOS_DP_CONTEXT, Common);
            DPModuleIndex = cbGetModuleIndex(pcbe, DevList[i], CBIOS_MODULE_TYPE_DP);
            DetectFlag.Value = 0;
            
            EphyMode = cbPHY_DP_GetEphyMode(pcbe, DPModuleIndex);
            cbPHY_DP_SelectEphyMode(pcbe, DPModuleIndex, DP_EPHY_DP_MODE);

            if (cbDPMonitor_Detect(pcbe, &pDpContext->DPMonitorContext, &DetectFlag))
            {
                cbI2cOverAux_ReadEDID(pcbe, pDevCommon, Buffer, sizeof(Buffer), 0x42, 0);
                
                HImageSize = (((CBIOS_U16)Buffer[2] & 0xF0) << 4) + Buffer[0];
                VImageSize = (((CBIOS_U16)Buffer[2] & 0x0F) << 8) + Buffer[1];
                cbDebugPrint((MAKE_LEVEL(GENERIC, DEBUG), "%s: get image size %d x %d mm.\n", FUNCTION_NAME, HImageSize, VImageSize));

                if ((HImageSize == 0) || (VImageSize == 0))
                {
                    cbDIU_Aux_Dpcd_Read(pcbe, DPModuleIndex, 0x70, &DPCD00070, 1);

                    if (DPCD00070 & 0x03)
                    {
                        cbDebugPrint((MAKE_LEVEL(GENERIC, DEBUG), "%s: read DPCD 00070 = %d, need patch.\n", FUNCTION_NAME, DPCD00070));
                        bRet = CBIOS_TRUE;
                    }
                }
                else if ((HImageSize <= 312) || (VImageSize <= 195))
                {
                    cbDebugPrint((MAKE_LEVEL(GENERIC, DEBUG), "%s: panel size is small, need patch.\n", FUNCTION_NAME));
                    bRet = CBIOS_TRUE;
                }
            }
            else
            {
                cbPHY_DP_SelectEphyMode(pcbe, DPModuleIndex, EphyMode);
            }
        }
    }

    return bRet;
}
