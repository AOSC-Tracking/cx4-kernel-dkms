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
** CBios display manager functions. 
** Mainly take charge of set mode, active devices and stream update.
**
** NOTE:
** The hw dependent function or structure SHOULD NOT be added to this file.
******************************************************************************/


#include "CBiosDisplayManager.h"
#include "CBiosChipShare.h"

static CBIOS_STATUS cbValidateSettingModeParam(PCBIOS_VOID pvcbe, PCBIOS_DISP_MODE_PARAMS pModeParams, PCBiosSettingModeParams pSettingModeParams)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    CBIOS_STATUS csRet = CBIOS_OK;
    //CBIOS_U32 DispDev;
    CBIOS_U32 IGAIndex = pSettingModeParams->IGAIndex;
    CBiosSettingModeParams SetModePara;
    CBIOS_SCALER_STATUS ScalerStatusToUse = DISABLE_SCALER;
    CBIOS_BOOL  bIsEnable3DVideo;
    CBIOS_U16   HDMI3DStructure;
    
    //DispDev = pcbe->DispMgr.ActiveDevices[IGAIndex];    
    
    cb_memcpy(&(SetModePara.SourcModeParams), &(pSettingModeParams->SourcModeParams), sizeof(CBiosSourceModeParams));
    cb_memcpy(&(SetModePara.DestModeParams), &(pSettingModeParams->DestModeParams), sizeof(CBiosDestModeParams));
    cb_memcpy(&(SetModePara.ScalerSizeParams), &(pSettingModeParams->ScalerSizeParams), sizeof(CBiosScalerSizeParams));


    if ((SetModePara.SourcModeParams.XRes == 0) ||(SetModePara.SourcModeParams.YRes == 0) ||
        (SetModePara.DestModeParams.XRes == 0) ||(SetModePara.DestModeParams.YRes == 0) ||
        (SetModePara.ScalerSizeParams.XRes == 0) ||(SetModePara.ScalerSizeParams.YRes == 0) ||
        (SetModePara.DestModeParams.RefreshRate == 0))
    {
        csRet = CBIOS_ER_INVALID_PARAMETER;
        cbDebugPrint((MAKE_LEVEL(GENERIC, ERROR), "cbValidateSettingModeParam: Mode para is ZERO!\n"));
    }
    else if ((SetModePara.DestModeParams.XRes < SetModePara.ScalerSizeParams.XRes)
        || (SetModePara.DestModeParams.YRes < SetModePara.ScalerSizeParams.YRes))
    {
        //dest mode should always bigger than or equal to scaler size, otherwise we will see panning
        csRet = CBIOS_ER_INVALID_PARAMETER;
        cbDebugPrint((MAKE_LEVEL(GENERIC, ERROR), "cbValidateSettingModeParam: Dest mode is smaller than scaler size!\n"));
    }
    else if ((SetModePara.ScalerSizeParams.XRes == SetModePara.SourcModeParams.XRes)
        && (SetModePara.ScalerSizeParams.YRes == SetModePara.SourcModeParams.YRes))
    {
        //source mode equal to dest mode, do not need scaler
        csRet = CBIOS_OK;
        ScalerStatusToUse = DISABLE_SCALER;
    }
    else if (((SetModePara.ScalerSizeParams.XRes <= SetModePara.SourcModeParams.XRes) && 
           (SetModePara.ScalerSizeParams.YRes < SetModePara.SourcModeParams.YRes))
           || ((SetModePara.ScalerSizeParams.XRes < SetModePara.SourcModeParams.XRes) &&
           (SetModePara.ScalerSizeParams.YRes <= SetModePara.SourcModeParams.YRes)))
    {
        csRet = CBIOS_ER_INVALID_PARAMETER;
        cbDebugPrint((MAKE_LEVEL(GENERIC, ERROR), "cbValidateSettingModeParam: Can't support downscaling!\n"));
    }
    else if (((SetModePara.ScalerSizeParams.XRes >= SetModePara.SourcModeParams.XRes)&&
             (SetModePara.ScalerSizeParams.YRes > SetModePara.SourcModeParams.YRes))
            || ((SetModePara.ScalerSizeParams.XRes > SetModePara.SourcModeParams.XRes)&&
             (SetModePara.ScalerSizeParams.YRes >= SetModePara.SourcModeParams.YRes)))
    {
        if (((SetModePara.ScalerSizeParams.XRes > SetModePara.SourcModeParams.XRes) &&
             (SetModePara.SourcModeParams.XRes > pcbe->ChipLimits.ulMaxPUHorSrc ||
              SetModePara.ScalerSizeParams.XRes > pcbe->ChipLimits.ulMaxPUHorDst))
            || ((SetModePara.ScalerSizeParams.YRes > SetModePara.SourcModeParams.YRes) &&
             (SetModePara.SourcModeParams.YRes > pcbe->ChipLimits.ulMaxPUVerSrc ||
              SetModePara.ScalerSizeParams.YRes > pcbe->ChipLimits.ulMaxPUVerDst))
            || !pcbe->ChipCaps.IsSupportUpScaling)
        {
            csRet = CBIOS_ER_HARDWARE_LIMITATION;
            cbDebugPrint((MAKE_LEVEL(GENERIC, ERROR), "cbValidateSettingModeParam: Scaler size exceeds hardware limit!\n"));
        }
        else
        {
            //upscaling
            csRet = CBIOS_OK;
            ScalerStatusToUse = ENABLE_UPSCALER;
        }
    }
    else
    {
        csRet = CBIOS_ER_HARDWARE_LIMITATION;
        cbDebugPrint((MAKE_LEVEL(GENERIC, ERROR), "cbValidateSettingModeParam: Invalid scaler size!\n"));
    }
    
    // Validate if user specified timing is OK.
    // Inorder to compatible with old driver, need to check struct size to avoid overflow to write.
    if (pSettingModeParams->SpecifyTiming.Flag)
    {
        cb_memcpy(&pcbe->SpecifyDestTimingSrc[IGAIndex], &pSettingModeParams->SpecifyTiming, sizeof(CBiosSpecifyDstTimingSrc));
    }    
    else
    {
        cb_memset(&pcbe->SpecifyDestTimingSrc[IGAIndex], 0, sizeof(CBiosSpecifyDstTimingSrc));
    }

    // Validate 3D video
    if (csRet == CBIOS_OK)
    {
        bIsEnable3DVideo = pSettingModeParams->Is3DVideoMode;
        HDMI3DStructure = pSettingModeParams->Video3DStruct;

        if (bIsEnable3DVideo)
        {
            if ((!pcbe->ChipCaps.IsSupport3DVideo) && (HDMI3DStructure != 0))
            {
                csRet = CBIOS_ER_INVALID_PARAMETER;
                cbDebugPrint((MAKE_LEVEL(GENERIC, ERROR), "cbValidateSettingModeParam: Invalid 3D video parameter!\n"));
            }
            else if (ScalerStatusToUse != DISABLE_SCALER)
            {
                csRet = CBIOS_ER_HARDWARE_LIMITATION;
                cbDebugPrint((MAKE_LEVEL(GENERIC, ERROR), "cbValidateSettingModeParam: HW limitation, can't support PU/PD when enable 3D video!\n"));
            }
            else
            {
                csRet = CBIOS_OK;
            }
        }
        else
        {
            csRet = CBIOS_OK;
        }
    }

    if (csRet == CBIOS_OK)
    {
        pModeParams->ScalerStatusToUse = ScalerStatusToUse;
        pModeParams->IsEnable3DVideo = bIsEnable3DVideo;
        pModeParams->IsSingleBuffer = pSettingModeParams->IsSingleBuffer;
        pModeParams->Video3DStruct = pSettingModeParams->Video3DStruct;
    }

    return csRet;
}

static CBIOS_VOID cb3DVideoTimingAdjust(PCBIOS_EXTENSION_COMMON pcbe, CBIOS_3D_STRUCTURE Video3DStruct, PCBIOS_TIMING_ATTRIB pTiming)
{
    switch (Video3DStruct)
    {
    case FRAME_PACKING:
    case L_DEPTH:
        pTiming->PixelClock *= 2;
        break;
    case LINE_ALTERNATIVE:
        pTiming->PixelClock *= 2;
        pTiming->VerTotal *= 2;
        pTiming->VerDisEnd *= 2;
        pTiming->VerBStart *= 2;
        pTiming->VerBEnd*= 2;
        pTiming->VerSyncStart *= 2;
        pTiming->VerSyncEnd *= 2;
        break;
    case SIDE_BY_SIDE_FULL:
        pTiming->PixelClock *= 2;
        pTiming->HorDisEnd *= 2;
        pTiming->HorBStart *= 2;
        pTiming->HorBEnd *= 2;
        pTiming->HorSyncStart *= 2;
        pTiming->HorSyncEnd *= 2;
        break;
    case SIDE_BY_SIDE_HALF:
    case TOP_AND_BOTTOM:
    default:
        break;
    }
}

static CBIOS_STATUS cbUpdateIGAModeInfo(PCBIOS_VOID pvcbe, PCBIOS_DISP_MODE_PARAMS pModeParams, PCBiosSettingModeParams pSettingModeParams)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    CBIOS_U32 IGAIndex = pSettingModeParams->IGAIndex;
    CBIOS_TIMING_ATTRIB Timing = {0};
    CBIOS_U32 Device = 0;
    PCBIOS_DEVICE_COMMON pDevCommon  = CBIOS_NULL;
    PCBIOS_MONITOR_MISC_ATTRIB pMonitorAttr = CBIOS_NULL;

    Device = pcbe->DispMgr.ActiveDevices[IGAIndex];
    pDevCommon = cbGetDeviceCommon(&pcbe->DeviceMgr, Device);
    pMonitorAttr = &(pDevCommon->EdidStruct.Attribute);

    //check bpc
    if ((pcbe->ChipCaps.IsSupportDeepColor) && (pMonitorAttr->IsCEA861HDMI))
    {
        pModeParams->BitPerComponent = pSettingModeParams->BitPerComponent;
    }
    else
    {
        //do not support deep color
        pModeParams->BitPerComponent = 8;
    }

    pModeParams->IGAIndex = pSettingModeParams->IGAIndex;
    
    pModeParams->SrcModePara.XRes = pSettingModeParams->SourcModeParams.XRes;
    pModeParams->SrcModePara.YRes = pSettingModeParams->SourcModeParams.YRes;

    pModeParams->ScalerPara.XRes = pSettingModeParams->ScalerSizeParams.XRes;
    pModeParams->ScalerPara.YRes = pSettingModeParams->ScalerSizeParams.YRes;

    pModeParams->TargetModePara.XRes = pSettingModeParams->DestModeParams.XRes;
    pModeParams->TargetModePara.YRes = pSettingModeParams->DestModeParams.YRes;
    pModeParams->TargetModePara.XTotal = pSettingModeParams->DestModeParams.XTotal;
    pModeParams->TargetModePara.YTotal = pSettingModeParams->DestModeParams.YTotal;
    pModeParams->TargetModePara.PixelClock = pSettingModeParams->DestModeParams.PixelClock;
    pModeParams->TargetModePara.RefRate = pSettingModeParams->DestModeParams.RefreshRate;
    if(pModeParams->TargetModePara.RefRate <= 1000)
    {
        pModeParams->TargetModePara.RefRate = 6000;
    }
    pModeParams->TargetModePara.bInterlace = pSettingModeParams->DestModeParams.InterlaceFlag;

    pModeParams->IGAOutColorSpace = CSC_FMT_RGB;//hardcode IGA output color space to RGB
    pModeParams->TargetModePara.DevInColorSpace = pModeParams->IGAOutColorSpace;

    cbMode_GetHVTiming(pcbe, &pModeParams->TargetModePara, Device, &Timing);
    if (pModeParams->IsEnable3DVideo)
    {
        cb3DVideoTimingAdjust(pcbe, pModeParams->Video3DStruct, &Timing);
    }

    pModeParams->VICCode = (CBIOS_U8)Timing.FormatVIC;
    pModeParams->TargetModePara.OutputSignal = cbMode_GetTimingSignal(pcbe, Device, 
                                                pSettingModeParams->DestModeParams.OutputSignal, &Timing);
    if (!pSettingModeParams->SkipDeviceMode)
    {
        cbDebugPrint((MAKE_LEVEL(GENERIC, INFO),  "ModeVIC = %d, Signal = %d!\n", Timing.FormatVIC, pModeParams->TargetModePara.OutputSignal));
    }

    //check pixel repetition
    pModeParams->PixelRepitition = 1;
    if(((pModeParams->TargetModePara.bInterlace) && ((Timing.YRes == 576) || (Timing.YRes == 480))) 
       || (!(pModeParams->TargetModePara.bInterlace) && ((Timing.YRes == 288) || (Timing.YRes == 240))))
    {
        pModeParams->PixelRepitition = 2;
    }

    pModeParams->PLLClock = Timing.PixelClock;
    
    cb_memcpy(&(pModeParams->TargetTiming), &Timing, sizeof(CBIOS_TIMING_ATTRIB));

    cb_memcpy(&pModeParams->ColorimetryCaps, &pMonitorAttr->ExtDataBlock[COLORIMETRY_DATA_BLOCK_TAG].ColorimetryData,
                sizeof(CBIOS_COLORIMETRY_DATA));
    cb_memcpy(&pModeParams->VideoCapability, &pMonitorAttr->ExtDataBlock[VIDEO_CAPABILITY_DATA_BLOCK_TAG].VideoCapabilityData,
                sizeof(CBIOS_VIDEO_CAPABILITY_DATA));

    return CBIOS_OK;
}


static CBIOS_STATUS cbDispMgrPrepareToSetMode(PCBIOS_EXTENSION_COMMON pcbe, PCBIOS_DEVICE_MANAGER pDevMgr, 
                                              PCBIOS_DISPLAY_MANAGER pDispMgr, PCBiosSettingModeParams pSettingModeParams)
{
    CBIOS_U32 IGAIndex = pSettingModeParams->IGAIndex;
    CBIOS_ACTIVE_TYPE Device = pDispMgr->ActiveDevices[IGAIndex];
    PCBIOS_DEVICE_COMMON pDevCommon = cbGetDeviceCommon(pDevMgr, Device);
    PCBIOS_DISP_MODE_PARAMS pModeParams = pDispMgr->pModeParams[IGAIndex];
    CBIOS_STATUS csRet = CBIOS_OK;

    if(pSettingModeParams->SkipIgaMode == 0)
    {
        cbDebugPrint((MAKE_LEVEL(GENERIC, INFO),  "SourceMode: %d x %d!\n", pSettingModeParams->SourcModeParams.XRes, pSettingModeParams->SourcModeParams.YRes)); 
        cbDebugPrint((MAKE_LEVEL(GENERIC, INFO),  "DestMode:   %d x %d @ %d!\n", pSettingModeParams->DestModeParams.XRes, pSettingModeParams->DestModeParams.YRes, 
                             pSettingModeParams->DestModeParams.RefreshRate)); 
        cbDebugPrint((MAKE_LEVEL(GENERIC, INFO), "ScalerMode: %d x %d!\n", pSettingModeParams->ScalerSizeParams.XRes, pSettingModeParams->ScalerSizeParams.YRes));
        cbDebugPrint((MAKE_LEVEL(GENERIC, INFO),  "Interlace=%d, Bpc=%d!\n", pSettingModeParams->DestModeParams.InterlaceFlag,
                             pSettingModeParams->BitPerComponent));
    }
    
    // Validate the input parameter.
    csRet = cbValidateSettingModeParam(pcbe, pModeParams, pSettingModeParams);
    if (csRet != CBIOS_OK)
    {
        cbDebugPrint((MAKE_LEVEL(GENERIC, ERROR), "%s: mode parameters are invalid!\n", FUNCTION_NAME));
        return csRet;
    }

    cbPathMgrSelectDIUPath(pcbe, pDevCommon->DeviceType, pModeParams->IGAIndex, &pSettingModeParams->DestModeParams);

    cbUpdateIGAModeInfo(pcbe, pModeParams, pSettingModeParams);
    cbDevUpdateDeviceModeInfo(pcbe, pDevCommon, pModeParams);

    return csRet;
}

static  CBIOS_U32  cbDispMgrGetIgaCnt(PCBIOS_EXTENSION_COMMON pcbe)
{
    CBIOS_U32  Count = 0;
    
    switch(pcbe->ChipID)
    {
        case  CHIPID_E3K:
            Count = 4;
            break;
        case  CHIPID_CHX004:
        case  CHIPID_CNE001:
            Count = 3;
            break;
        default:
            Count = 2;
            break;
    }

    return  Count;
}

CBIOS_STATUS cbDispMgrInit(PCBIOS_VOID pvcbe)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    PCBIOS_DISPLAY_MANAGER pDispMgr = &(pcbe->DispMgr);
    CBIOS_U32 i = 0, j = 0;

    pDispMgr->IgaCount = cbDispMgrGetIgaCnt(pcbe);

    for (i = IGA1; i < pDispMgr->IgaCount; i++)
    {
        pDispMgr->ActiveDevices[i] = CBIOS_TYPE_NONE;
        pDispMgr->pModeParams[i] = cb_AllocateNonpagedPool(sizeof(CBIOS_DISP_MODE_PARAMS));
        if(pDispMgr->pModeParams[i] == CBIOS_NULL)
        {
            cbDebugPrint((MAKE_LEVEL(GENERIC, ERROR),"%s: pModeParams for IGA%d allocate error!\n", FUNCTION_NAME, i));
            goto Fail;
        }

        pDispMgr->pModeParams[i]->IGAIndex = i;

        pDispMgr->IgaUpScale = (pcbe->ChipCaps.IsSupportUpScaling)? 1 : 0;

        pDispMgr->IgaDownScale = 0;

        if(pcbe->ChipID == CHIPID_E3K)
        {
            pDispMgr->StreamCount[i] = STREAM_NUM_E3K;             
        }
        else if (pcbe->ChipID == CHIPID_CHX004)
        {
            pDispMgr->StreamCount[i] = (i < (pDispMgr->IgaCount-1))? STREAM_NUM_CHX004 : (STREAM_NUM_CHX004-1);
        }
        else if (pcbe->ChipID == CHIPID_CNE001)
        {
            pDispMgr->StreamCount[i] = STREAM_NUM_CNE001;
        }
        
        for(j = 0; j < pDispMgr->StreamCount[i]; j++)
        {
            pDispMgr->DownScaleStreamMask[i] |= 0;
            if(pcbe->ChipID == CHIPID_E3K)
            {
                pDispMgr->UpScaleStreamMask[i] |= (j == CBIOS_STREAM_PS)? 0 : (1 << j);
            }
            else
            {
                pDispMgr->UpScaleStreamMask[i] |= (1 << j);
            }
        }
    }

    return CBIOS_OK;

Fail:
    
    for (i = IGA1; i < pDispMgr->IgaCount; i++)
    {
        if(pDispMgr->pModeParams[i]) 
        {
            cb_FreePool(pDispMgr->pModeParams[i]);
            pDispMgr->pModeParams[i] = CBIOS_NULL;
        }
    }

    return   CBIOS_ER_NO_ENOUGH_MEM;
}


CBIOS_STATUS cbDispMgrGetActiveDevice(PCBIOS_VOID pvcbe, PCBIOS_DISPLAY_MANAGER pDispMgr, PCIP_ACTIVE_DEVICES pActiveDevices)
{
    CBIOS_STATUS  status = CBIOS_ER_INVALID_PARAMETER;
    CBIOS_U32     i = 0;
    
    if(pDispMgr)
    {
        for (i = IGA1; i < CBIOS_IGACOUNTS; i++)
        {
            pActiveDevices->DeviceId[i]  = pDispMgr->ActiveDevices[i];
            status = CBIOS_OK;
        }
    }

    return  status;
}


CBIOS_STATUS cbDispMgrSetActiveDevice(PCBIOS_VOID pvcbe, PCBIOS_DISPLAY_MANAGER pDispMgr, PCIP_ACTIVE_DEVICES pActiveDevices)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    CBIOS_STATUS Status = CBIOS_OK;
    CBIOS_U32    i = 0;

    cbTraceEnter(GENERIC);

    for (i = IGA1; i < CBIOS_IGACOUNTS; i++)
    {
        pDispMgr->ActiveDevices[i] = pActiveDevices->DeviceId[i];
    }

    // If Both IGA device equal zero, we should not update register value.
    // Or else, VBIOS will have no device to light.
    if((pDispMgr->ActiveDevices[IGA1] != 0) || (pDispMgr->ActiveDevices[IGA2] != 0) || \
       (pDispMgr->ActiveDevices[IGA3] != 0) || (pDispMgr->ActiveDevices[IGA4] != 0))
    {
        // Update the CR6b and CR6C register for compatible with VBIOS, and for Dos full screen.
        cbHwUpdateActDeviceToReg(pcbe, pDispMgr);
    }

    cbTraceExit(GENERIC);

    return Status;
}


CBIOS_STATUS cbDispMgrGetMode(PCBIOS_VOID pvcbe, PCBIOS_DISPLAY_MANAGER pDispMgr, PCBiosSettingModeParams pModeParams)
{
    CBIOS_U32 IGAIndex = pModeParams->IGAIndex;
    PCBIOS_DISP_MODE_PARAMS pDispModeParams = pDispMgr->pModeParams[IGAIndex];
    //source mode
    pModeParams->SourcModeParams.XRes = pDispModeParams->SrcModePara.XRes;
    pModeParams->SourcModeParams.YRes = pDispModeParams->SrcModePara.YRes;
    //target mode
    pModeParams->DestModeParams.XRes = pDispModeParams->TargetModePara.XRes;
    pModeParams->DestModeParams.YRes = pDispModeParams->TargetModePara.YRes;
    pModeParams->DestModeParams.RefreshRate = pDispModeParams->TargetModePara.RefRate;
    pModeParams->DestModeParams.OutputSignal = pDispModeParams->TargetModePara.OutputSignal;
    pModeParams->DestModeParams.InterlaceFlag = pDispModeParams->TargetModePara.bInterlace;
    pModeParams->DestModeParams.XTotal = pDispModeParams->TargetModePara.XTotal;
    pModeParams->DestModeParams.YTotal = pDispModeParams->TargetModePara.YTotal;
    pModeParams->DestModeParams.PixelClock = pDispModeParams->TargetModePara.PixelClock;
    //scaler size
    pModeParams->ScalerSizeParams.XRes = pDispModeParams->ScalerPara.XRes;
    pModeParams->ScalerSizeParams.YRes = pDispModeParams->ScalerPara.YRes;

    return  CBIOS_OK;
}

CBIOS_STATUS cbDispMgrGetHwCounter(PCBIOS_VOID pvcbe, PCBIOS_GET_HW_COUNTER  pGetCounter)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    return  cbHwGetCounter(pcbe, pGetCounter);
}

CBIOS_STATUS cbDispMgrGetModeFromReg(PCBIOS_VOID pvcbe, PCBIOS_DISPLAY_MANAGER pDispMgr, PCBiosSettingModeParams pModeParams)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    CBIOS_U32 IGAIndex = pModeParams->IGAIndex;
    CBIOS_ACTIVE_TYPE ActiveDevice = pDispMgr->ActiveDevices[IGAIndex];
    CBIOS_TIMING_ATTRIB SourceTimingAttr = {0};
    CBIOS_TIMING_ATTRIB TargetTimingAttr = {0};
    CBIOS_TIMING_FLAGS  Flags = {0};

    cbGetModeInfoFromReg(pcbe, ActiveDevice, &SourceTimingAttr, &Flags, IGAIndex, TIMING_REG_TYPE_CR);
    cbGetModeInfoFromReg(pcbe, ActiveDevice, &TargetTimingAttr, &Flags, IGAIndex, TIMING_REG_TYPE_SR);

    //source mode
    pModeParams->SourcModeParams.XRes = SourceTimingAttr.XRes;
    pModeParams->SourcModeParams.YRes = SourceTimingAttr.YRes;
    //target mode
    pModeParams->DestModeParams.XRes = TargetTimingAttr.XRes;
    pModeParams->DestModeParams.YRes = TargetTimingAttr.YRes;
    pModeParams->DestModeParams.RefreshRate = TargetTimingAttr.RefreshRate;
    pModeParams->DestModeParams.InterlaceFlag = Flags.IsInterlace;
    pModeParams->DestModeParams.XTotal = TargetTimingAttr.HorTotal;
    pModeParams->DestModeParams.YTotal = TargetTimingAttr.VerTotal;
    pModeParams->DestModeParams.PixelClock = TargetTimingAttr.PixelClock;
    //scaler size
    pModeParams->ScalerSizeParams.XRes = TargetTimingAttr.XRes;
    pModeParams->ScalerSizeParams.YRes = TargetTimingAttr.YRes;

    return  CBIOS_OK;
}

CBIOS_STATUS cbDispMgrSetMode(PCBIOS_VOID pvcbe, PCBIOS_DEVICE_MANAGER pDevMgr, PCBIOS_DISPLAY_MANAGER pDispMgr, PCBiosSettingModeParams pSettingModeParams)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    CBIOS_STATUS csRet = CBIOS_OK;
    CBIOS_U32 IGAIndex = pSettingModeParams->IGAIndex;
    CBIOS_ACTIVE_TYPE Device = pDispMgr->ActiveDevices[IGAIndex];
    PCBIOS_DEVICE_COMMON pDevCommon  = cbGetDeviceCommon(pDevMgr, Device);
    PCBIOS_DISP_MODE_PARAMS pModeParams = pDispMgr->pModeParams[IGAIndex];

#ifdef CHECK_CHIPENABLE
    if (!cbHWIsChipEnable(pcbe))
        return CBIOS_ER_CHIPDISABLE;
#endif

    cbTraceEnter(GENERIC);

    if(!Device || !pDevCommon)
    {
        return  CBIOS_ER_OPERATION_ON_NONE_DEVICE;
    }

    csRet = cbDispMgrPrepareToSetMode(pcbe, pDevMgr, pDispMgr, pSettingModeParams);
    if (csRet != CBIOS_OK)
    {
        return csRet;
    }

    if(!pSettingModeParams->SkipIgaMode)
    {
        cbHwSetModeToIGA(pcbe, pModeParams);
    }

    if(!pSettingModeParams->SkipDeviceMode)
    {
        cbDevSetModeToDevice(pcbe, pDevCommon, pModeParams);
    }

    cbTraceExit(GENERIC);
    return csRet;
}

CBIOS_STATUS cbDispMgrGetDispResource(PCBIOS_VOID pvcbe, PCBIOS_GET_DISP_RESOURCE pGetDispRes)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    PCBIOS_DISPLAY_MANAGER  pDispMgr = &pcbe->DispMgr;
    CBIOS_STATUS Status = CBIOS_OK;
    CBIOS_U32  i = 0;

    pGetDispRes->CrtcNum = pDispMgr->IgaCount;

    for(i = 0; i < pGetDispRes->CrtcNum; i++)
    {
        pGetDispRes->PlaneNum[i] = pDispMgr->StreamCount[i];
    }

    return Status;
}

CBIOS_STATUS cbDispMgrGetDisplayCaps(PCBIOS_VOID pvcbe, PCBIOS_DISPLAY_CAPS pDispCaps)
{
    CBIOS_STATUS Status = CBIOS_OK;
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    PCBIOS_DISPLAY_MANAGER  pDispMgr = &pcbe->DispMgr;
    CBIOS_U32  i = 0;

    pDispCaps->SuppCrtcUpScale = pDispMgr->IgaUpScale;
    pDispCaps->SuppCrtcDownScale = pDispMgr->IgaDownScale;

    if(pDispCaps->pUpScalePlaneMask)
    {
        for(i = 0; i < pDispMgr->IgaCount; i++)
        {
            pDispCaps->pUpScalePlaneMask[i] = pDispMgr->UpScaleStreamMask[i];
        }
    }

    if(pDispCaps->pDownScalePlaneMask)
    {
        for(i = 0; i < pDispMgr->IgaCount; i++)
        {
            pDispCaps->pDownScalePlaneMask[i] = pDispMgr->DownScaleStreamMask[i];
        }
    }

    pDispCaps->ulMaxPUHorSrc = pcbe->ChipLimits.ulMaxPUHorSrc;
    pDispCaps->ulMaxPUVerSrc = pcbe->ChipLimits.ulMaxPUVerSrc;

    return Status;
}

CBIOS_STATUS cbDispMgrUpdateFrame(PCBIOS_VOID pvcbe, PCBIOS_UPDATE_FRAME_PARA pUpdateFrame)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;

    return cbUpdateFrame(pcbe, pUpdateFrame);
}

CBIOS_STATUS cbDispMgrDoCSCAdjust(PCBIOS_VOID pvcbe, PCBIOS_DISPLAY_MANAGER pDispMgr, PCBIOS_CSC_ADJUST_PARA pCSCAdjustPara)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    CBIOS_U32 IGAIndex = pCSCAdjustPara->IGAIndex;
    CBIOS_ACTIVE_TYPE Device = pDispMgr->ActiveDevices[IGAIndex];

    return cbDoCSCAdjust(pcbe, Device, pCSCAdjustPara);
}

CBIOS_STATUS cbDispMgrAdjustStreamCSC(PCBIOS_VOID pvcbe, PCBIOS_STREAM_CSC_PARA pCSCAdjustPara)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;

    return cbAdjustStreamCSC(pcbe, pCSCAdjustPara);
}

CBIOS_STATUS  cbDispMgrGetDispAddr(PCBIOS_VOID  pvcbe,  PCBIOS_GET_DISP_ADDR  pGetDispAddr)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;

    return cbGetDispAddr(pcbe, pGetDispAddr);    
}

CBIOS_STATUS cbDispMgrSetHwCursor(PCBIOS_VOID pvcbe, PCBIOS_CURSOR_PARA pSetCursor)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;

    return cbSetHwCursor(pcbe, pSetCursor);
}

CBIOS_STATUS cbDispMgrDeInit(PCBIOS_VOID pvcbe)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    PCBIOS_DISPLAY_MANAGER pDispMgr = &(pcbe->DispMgr);
    CBIOS_U32 i = 0;

    for (i = IGA1; i < pDispMgr->IgaCount; i++)
    {
        if(pDispMgr->pModeParams[i])
        {
            cb_FreePool(pDispMgr->pModeParams[i]);
            pDispMgr->pModeParams[i] = CBIOS_NULL;
        }
    }

    return CBIOS_OK;
}
