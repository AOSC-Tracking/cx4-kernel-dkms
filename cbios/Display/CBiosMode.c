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
** CBios mode related functions. Generate mode list from EDID data. Search timing from EDID and 
** timing table.
**
** NOTE:
** The hw dependent function or structure SHOULD NOT be added to this file.
******************************************************************************/

#include "CBiosChipShare.h"

#define MODE_COLOR_DEPTH_CAPS (CBIOS_COLORDEPTH8 | CBIOS_COLORDEPTH16 | CBIOS_COLORDEPTH32XRGB | \
                              CBIOS_COLORDEPTH32ARGB | CBIOS_COLORDEPTH32ABGR | CBIOS_COLORDEPTH2101010ARGB | \
                              CBIOS_COLORDEPTH2101010ABGR | CBIOS_COLORDEPTH16161616ABGRF)

#define MODE_INFO_EXT_SIZE    sizeof(CBiosModeInfoExt)

CBIOS_U8 KortekMonitorID[MONITORIDLENGTH] = 
{
    0x2E, 0x8C, 0x19, 0x00
};

static CBiosModeInfoExt AdapterModeList[] = 
{
{
    MODE_INFO_EXT_SIZE, 
    640, 
    480, 
    6000, 
    MODE_COLOR_DEPTH_CAPS,
    800,
    525,
    251750,
    {0},
},
{
    MODE_INFO_EXT_SIZE, 
    800, 
    600, 
    6000, 
    MODE_COLOR_DEPTH_CAPS,
    1056,
    628,
    400000,
    {0},
},
{
    MODE_INFO_EXT_SIZE, 
    1024, 
    768, 
    6000, 
    MODE_COLOR_DEPTH_CAPS,
    1344,
    806,
    650000,
    {0},
},
{
    MODE_INFO_EXT_SIZE, 
    1280, 
    720, 
    6000, 
    MODE_COLOR_DEPTH_CAPS,
    1650,
    750,
    742500,
    {0},
},
{
    MODE_INFO_EXT_SIZE, 
    1280, 
    800, 
    6000, 
    MODE_COLOR_DEPTH_CAPS,
    1680,
    831,
    835000,
    {0},
},
{
    MODE_INFO_EXT_SIZE, 
    1280, 
    1024, 
    6000, 
    MODE_COLOR_DEPTH_CAPS,
    1688,
    1066,
    1080000,
    {0},
},
{
    MODE_INFO_EXT_SIZE, 
    1440, 
    900, 
    6000, 
    MODE_COLOR_DEPTH_CAPS,
    1904,
    934,
    1065000,
    {0},
},
{
    MODE_INFO_EXT_SIZE, 
    1680, 
    1050, 
    6000, 
    MODE_COLOR_DEPTH_CAPS,
    2240,
    1089,
    1462500,
    {0},
},
{
    MODE_INFO_EXT_SIZE, 
    1920, 
    1080, 
    6000, 
    MODE_COLOR_DEPTH_CAPS,
    2200,
    1125,
    1485000,
    {0},
},
};

        
static CBiosModeInfoExt DP_DefaultModeList[] = 
{
{
    MODE_INFO_EXT_SIZE, 
    640, 
    480, 
    6000, 
    MODE_COLOR_DEPTH_CAPS,
    800,
    525,
    251750,
    {0},
},
{
    MODE_INFO_EXT_SIZE, 
    1280, 
    720, 
    5000, 
    MODE_COLOR_DEPTH_CAPS,
    1980,
    750,
    742500,
    {0},
},
{
    MODE_INFO_EXT_SIZE, 
    1280, 
    720, 
    5994, 
    MODE_COLOR_DEPTH_CAPS,
    1650,
    750,
    742500,
    {0},
},
{
    MODE_INFO_EXT_SIZE, 
    1280, 
    720, 
    6000, 
    MODE_COLOR_DEPTH_CAPS,
    1650,
    750,
    742500,
    {0},
},
{
    MODE_INFO_EXT_SIZE, 
    1920, 
    1080, 
    5994, 
    MODE_COLOR_DEPTH_CAPS,
    2200,
    1125,
    1485000,
    {0},
},
{
    MODE_INFO_EXT_SIZE, 
    1920, 
    1080, 
    6000, 
    MODE_COLOR_DEPTH_CAPS,
    2200,
    1125,
    1485000,
    {1},
},
};

static CBiosModeInfoExt Device_DefaultModeList[] = 
{
{
    MODE_INFO_EXT_SIZE, 
    640, 
    480, 
    6000, 
    MODE_COLOR_DEPTH_CAPS,
    800,
    525,
    251750,
    {0},
},
{
    MODE_INFO_EXT_SIZE, 
    800, 
    600, 
    6000, 
    MODE_COLOR_DEPTH_CAPS,
    1056,
    628,
    400000,
    {0},
},
{
    MODE_INFO_EXT_SIZE, 
    1024, 
    768, 
    6000, 
    MODE_COLOR_DEPTH_CAPS,
    1344,
    806,
    650000,
    {0},
},
{
    MODE_INFO_EXT_SIZE, 
    1280, 
    1024, 
    6000, 
    MODE_COLOR_DEPTH_CAPS,
    1688,
    1066,
    1080000,
    {0},
},
{
    MODE_INFO_EXT_SIZE, 
    1920, 
    1080, 
    6000, 
    MODE_COLOR_DEPTH_CAPS,
    2200,
    1125,
    1485000,
    {1},
}
};

/***************************************************************
Function:    cbMode_CompareMode

Description: Compare two mode XRes, YRes, Refresh Rate and Interlace/Progress

Input:       pMode1, pMode2: two modes need to compare

Output:      

Return:      Positive: if Mode1 >  Mode2
             Zero    : if Mode1 == Mode2
             Negative: if Mode1 <  Mode2
***************************************************************/
static CBIOS_S32 cbMode_CompareMode(PCBIOS_DEVICE_TIMING_INFO pMode1, PCBIOS_DEVICE_TIMING_INFO pMode2)
{
    CBIOS_S32 retValue;
    CBIOS_TIMING_ATTRIB *pTiming1 = &pMode1->FullTiming;
    CBIOS_TIMING_ATTRIB *pTiming2 = &pMode2->FullTiming;
    CBIOS_U8 Priority1 = 0, Priority2 = 0;

    //Mode is reported to OS that require partial or full timing and only shows x/y/vrefresh(p or i) to user. vrefresh is derived from clock,
    //we sort mode with priority x->y->refresh->prefer_flag->interlace->clock, and ignore diff in total/blank/sync of timing
    Priority1 |= (pTiming1->XRes > pTiming2->XRes)? (1 << 7) : 0;
    Priority2 |= (pTiming2->XRes > pTiming1->XRes)? (1 << 7) : 0;

    Priority1 |= (pTiming1->YRes > pTiming2->YRes)? (1 << 6) : 0;
    Priority2 |= (pTiming2->YRes > pTiming1->YRes)? (1 << 6) : 0;

    Priority1 |= (pTiming1->RefreshRate > pTiming2->RefreshRate)? (1 << 5) : 0;
    Priority2 |= (pTiming2->RefreshRate > pTiming1->RefreshRate)? (1 << 5) : 0;

    if(pMode1->IsPreferMode && !pMode2->IsPreferMode)
    {
        Priority1 |= 1 << 4;
    }
    else if(pMode2->IsPreferMode && !pMode1->IsPreferMode)
    {
        Priority2 |= 1 << 4;
    }

    if(!pMode1->IsInterLaced && pMode2->IsInterLaced)
    {
        Priority1 |= 1 << 3;
    }
    else if(!pMode2->IsInterLaced && pMode1->IsInterLaced)
    {
        Priority2 |= 1 << 3;
    }

    Priority1 |= (pTiming1->PixelClock > pTiming2->PixelClock)? (1 << 2) : 0;
    Priority2 |= (pTiming2->PixelClock > pTiming1->PixelClock)? (1 << 2) : 0;

    if((Priority1 | (1 << 4)) == (Priority2 | (1 << 4)) || Priority1 == Priority2)
    {
        //if only prefer_flag is different, we think they are the same mode, just report the prefer one is ok
        retValue = 0;
    }
    else if(Priority1 > Priority2)
    {
        retValue = 1;
    }
    else
    {
        retValue = -1;
    }

    return retValue;
}

static CBIOS_BOOL cbMode_AddMode(PCBIOS_DEVICE_COMMON pDevCommon, PCBiosModeInfoExt pMode)
{
    CBIOS_BOOL bAdded = CBIOS_FALSE;
    PCBIOS_EDID_STRUCTURE_DATA pEDIDStruct = &pDevCommon->EdidStruct;
    CBIOS_U32  InitSize = pEDIDStruct->TimingNum;
    CBIOS_U32  Index = 0;

    //to check whether pDeviceMode is already added
    for(Index = 0; Index < pDevCommon->ValidModeNum; Index++)
    {
        if(pDevCommon->pDeviceModeList[Index].XRes == pMode->XRes &&
           pDevCommon->pDeviceModeList[Index].YRes == pMode->YRes &&
           pDevCommon->pDeviceModeList[Index].PixelClock == pMode->PixelClock &&
           pDevCommon->pDeviceModeList[Index].XTotal == pMode->XTotal &&
           pDevCommon->pDeviceModeList[Index].YTotal == pMode->YTotal &&
           pDevCommon->pDeviceModeList[Index].isInterlaceMode == pMode->isInterlaceMode)
        {
            return CBIOS_TRUE;
        }
    }

    if(!pDevCommon->pDeviceModeList)
    {
        pDevCommon->ValidModeNum = 0;
        pDevCommon->pDeviceModeList = cb_AllocatePagedPool(InitSize * sizeof(CBiosModeInfoExt));
        pDevCommon->ModeListArraySize = (pDevCommon->pDeviceModeList)? InitSize : 0;
    }

    if(pDevCommon->ValidModeNum >= pDevCommon->ModeListArraySize && pDevCommon->pDeviceModeList)
    {
        CBIOS_U32 NewSize = pDevCommon->ModeListArraySize + InitSize/2;
        PCBiosModeInfoExt  pTempTable = cb_AllocatePagedPool(NewSize * sizeof(CBiosModeInfoExt));
        if(pTempTable)
        {
            cb_memcpy(pTempTable, pDevCommon->pDeviceModeList, pDevCommon->ModeListArraySize * sizeof(CBiosModeInfoExt));
            cb_FreePool(pDevCommon->pDeviceModeList);
            pDevCommon->pDeviceModeList = pTempTable;
            pDevCommon->ModeListArraySize = NewSize;
        }
    }

    if(pDevCommon->pDeviceModeList && pDevCommon->ValidModeNum < pDevCommon->ModeListArraySize)
    {
        cb_memcpy(&pDevCommon->pDeviceModeList[pDevCommon->ValidModeNum], pMode, sizeof(CBiosModeInfoExt));
        pDevCommon->ValidModeNum += 1;
        bAdded = CBIOS_TRUE;
    }

    return bAdded;
}

/*********
per CEA861-F spec
if A Video Timing with a vertical frequency that is an integer multiple of 6.00 Hz 
(i.e., 24.00, 30.00, 60.00, 120.00 or 240.00 Hz) is considered to be the same as a 
Video Timing with the equivalent detailed timing information but where the vertical 
frequency is adjusted by a factor of 1000/1001

here Add 5994(6000) for 720X480,  add 5994(6000) for 1280X720, add 5994(6000)  and 2997(3000) for 1920X1080 mode
*********/
static CBIOS_BOOL cbMode_AddDeviceMode(PCBIOS_DEVICE_COMMON pDevCommon, PCBiosModeInfoExt pDeviceMode, PCBIOS_DEVICE_TIMING_INFO pTiming)
{
    CBIOS_BOOL bHasEqvlMode = CBIOS_FALSE, bRet = CBIOS_FALSE;
    CBIOS_U32  Refresh = 0, PixelClock = 0;
    CBiosModeInfoExt EquivalentMode;
    CBIOS_BOOL bCEA861Device = CBIOS_FALSE;

    if(pDevCommon->EdidStruct.Attribute.IsCEA861Monitor || pDevCommon->EdidStruct.Attribute.IsCEA861HDMI)
    {
        bCEA861Device = CBIOS_TRUE;
    }

    //look for equivalent mode
    if(bCEA861Device && pTiming->FullTiming.FormatVIC)
    {
        bHasEqvlMode = cbEdidModule_IsEquivalentModeExist(pTiming->FullTiming.FormatVIC, &Refresh, &PixelClock);
    }

    if(!bHasEqvlMode)
    {
        return cbMode_AddMode(pDevCommon, pDeviceMode);
    }

    cb_memcpy(&EquivalentMode, pDeviceMode, sizeof(CBiosModeInfoExt));
    EquivalentMode.RefreshRate = Refresh;
    EquivalentMode.PixelClock = PixelClock;
    EquivalentMode.isPreferredMode = 0;
    //Add prefer/bigger one first
    if(pDeviceMode->isPreferredMode || (!pDeviceMode->isPreferredMode && pDeviceMode->RefreshRate > Refresh))
    {
        bRet = cbMode_AddMode(pDevCommon, pDeviceMode);
        cbMode_AddMode(pDevCommon, &EquivalentMode);
    }
    else
    {
        cbMode_AddMode(pDevCommon, &EquivalentMode);
        bRet = cbMode_AddMode(pDevCommon, pDeviceMode);
    }

    return bRet;
}

// All timing value is progressive, even though request is interlace mode.
CBIOS_BOOL cbMode_GetHVTiming(PCBIOS_VOID pvcbe,
                              PCBIOS_VOID  pMode,
                              CBIOS_ACTIVE_TYPE Device,
                              PCBIOS_TIMING_ATTRIB pTiming)

{
    PCBIOS_EXTENSION_COMMON     pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    PCBIOS_DEVICE_COMMON        pDevCommon  = cbGetDeviceCommon(&pcbe->DeviceMgr, Device);
    PCBIOS_EDID_STRUCTURE_DATA  pEdidStruct = &pDevCommon->EdidStruct;
    PCBIOS_DEVICE_TIMING_INFO  pTimingList = pEdidStruct->pDeviceTimingList;
    PCBIOS_MODE_TARGET_PARA pTargetMode = (PCBIOS_MODE_TARGET_PARA)pMode;
    CBIOS_U32  i = 0, MatchIndex = pEdidStruct->TimingNum;
    CBIOS_BOOL bFind = CBIOS_FALSE, bCEA861Device = CBIOS_FALSE, bNeedFix = CBIOS_FALSE;
    CBIOS_U32 FixClock = 0;
    CBIOS_U16 FixRefRate = 0;

    if(pEdidStruct->Attribute.IsCEA861Monitor || pEdidStruct->Attribute.IsCEA861HDMI)
    {
        bCEA861Device = CBIOS_TRUE;
    }

    //when pixel clock of pMode is not 0, use active/total/clock to get accurate timing,
    //otherwise use legacy x/y/refresh to get/calc rough timing
    if(pEdidStruct->pDeviceTimingList && pTargetMode->PixelClock)
    {
        for(i = 0; i < pEdidStruct->TimingNum; i++)
        {
            if(pTimingList[i].FullTiming.XRes == pTargetMode->XRes &&
                pTimingList[i].FullTiming.YRes == pTargetMode->YRes &&
                pTimingList[i].FullTiming.HorTotal == pTargetMode->XTotal &&
                pTimingList[i].FullTiming.VerTotal == pTargetMode->YTotal &&
                !!pTimingList[i].IsInterLaced == !!pTargetMode->bInterlace)
            {
                CBIOS_U16 FmtVIC = pTimingList[i].FullTiming.FormatVIC;

                //clock unit in linux is khz, but cbios use 100hz, drm_mode.clock = cb_mode.clock/10, this may bring deviation
                //so compare clock after /10
                if(pTimingList[i].FullTiming.PixelClock/10 == pTargetMode->PixelClock/10)
                {
                    MatchIndex = i;
                    bNeedFix = CBIOS_FALSE;
                    break;
                }
                else if(FmtVIC && bCEA861Device && (MatchIndex == pEdidStruct->TimingNum) &&
                          cbEdidModule_IsRefreshInCEARange((CBIOS_U16)pTargetMode->RefRate, FmtVIC, &FixRefRate, &FixClock))
                {
                    MatchIndex = i;
                    bNeedFix = CBIOS_TRUE;
                }
            }          
        }
    }

    if(MatchIndex < pEdidStruct->TimingNum)
    {
        cb_memcpy(pTiming, &pTimingList[MatchIndex].FullTiming, sizeof(CBIOS_TIMING_ATTRIB));
        bFind = CBIOS_TRUE;
        if(bNeedFix)
        {
            pTiming->RefreshRate = FixRefRate;
            pTiming->PixelClock = FixClock;
        }
    }

    if(!bFind)
    {
        bFind = cbEDIDModule_GetXYRTiming(pEdidStruct, 
                                          (CBIOS_U16)pTargetMode->XRes,
                                          (CBIOS_U16)pTargetMode->YRes,
                                          (CBIOS_U16)pTargetMode->RefRate,
                                          pTiming);

        if(bFind && !bCEA861Device)
        {
            pTiming->FormatVIC = 0;
        }
    }

    return bFind;
}


CBIOS_U32  cbMode_GetTimingSignal(PCBIOS_VOID pvcbe, CBIOS_ACTIVE_TYPE Device, CBIOS_U32 ReqSignal, PCBIOS_TIMING_ATTRIB pTiming)
{
    PCBIOS_EXTENSION_COMMON     pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    PCBIOS_DEVICE_COMMON        pDevCommon  = cbGetDeviceCommon(&pcbe->DeviceMgr, Device);
    PCBIOS_EDID_STRUCTURE_DATA  pEdidStruct = &pDevCommon->EdidStruct;
    CBIOS_U32  OutSignal = CBIOS_RGBOUTPUT, i = 0;

    if (ReqSignal)
    {
        OutSignal = ReqSignal;
    }
    else
    {
        OutSignal = CBIOS_RGBOUTPUT;
    }

    if(!pTiming || !pEdidStruct->pDeviceTimingList)
    {
        return OutSignal;
    }

    for(i = 0; i < pEdidStruct->TimingNum; i++)
    {
        if(pTiming->FormatVIC)
        {
            if (pEdidStruct->pDeviceTimingList[i].FullTiming.FormatVIC == pTiming->FormatVIC)
            {
                break;
            }
        }
        else if (cb_memcmp(pTiming, &pEdidStruct->pDeviceTimingList[i].FullTiming, sizeof(CBIOS_TIMING_ATTRIB)) == 0)
        {
            break;
        }
    }

    // for y420 only mode or force y420 patch
    if(i < pEdidStruct->TimingNum)
    {
        if(pEdidStruct->pDeviceTimingList[i].IsSupportYCbCr420 &&
            !pEdidStruct->pDeviceTimingList[i].IsSupportOtherFormats)
        {
            OutSignal = CBIOS_YCBCR420OUTPUT;
        }
        else if((pcbe->SVID == 0x206E && pcbe->SSID == 0x1028) &&
            (pTiming->XRes >= 3840 && pTiming->YRes >= 2160 && pTiming->RefreshRate >= 5900))
        {
            OutSignal = CBIOS_YCBCR420OUTPUT;
        }
        else if (OutSignal == CBIOS_YCBCR420OUTPUT && !pEdidStruct->pDeviceTimingList[i].IsSupportYCbCr420)
        {
            OutSignal = CBIOS_RGBOUTPUT;
        }
    }
    else if (OutSignal == CBIOS_YCBCR420OUTPUT)
    {
        OutSignal = CBIOS_RGBOUTPUT;
    }

    // check format support
    if((OutSignal == CBIOS_YCBCR422OUTPUT && !pEdidStruct->Attribute.IsCEA861YCbCr422) ||
       (OutSignal == CBIOS_YCBCR444OUTPUT && !pEdidStruct->Attribute.IsCEA861YCbCr444))
    {
        OutSignal = CBIOS_RGBOUTPUT;
    }
    
    if (OutSignal == CBIOS_RGBOUTPUT && !pEdidStruct->Attribute.IsCEA861RGB)
    {
        cbDebugPrint((MAKE_LEVEL(GENERIC, WARNING), "Current monitor may not support RGB!\n"));
    }

    return OutSignal;
}

CBIOS_U32 cbMode_GetDefaultModeList(PCBIOS_VOID pvcbe, PCBiosModeInfoExt pModeList, CBIOS_ACTIVE_TYPE  Device)
{   
    CBIOS_U32          ulModeNum = 0;
    CBIOS_U32          i = 0;

    switch(Device)
    {
    case CBIOS_TYPE_DP1:
    case CBIOS_TYPE_DP2:
    case CBIOS_TYPE_DP3:
    case CBIOS_TYPE_DP4:
        ulModeNum = sizeofarray(DP_DefaultModeList);
        if (pModeList != CBIOS_NULL)
        {
            cb_memcpy(pModeList, DP_DefaultModeList, ulModeNum * sizeof(CBiosModeInfoExt));
        }
        break;
        
    case CBIOS_TYPE_DVO:
    case CBIOS_TYPE_CRT:
    default:
        ulModeNum = sizeofarray(Device_DefaultModeList);
        if (pModeList != CBIOS_NULL)
        {
            cb_memcpy(pModeList, Device_DefaultModeList, ulModeNum * sizeof(CBiosModeInfoExt));
        }
        break;
    }

    cbDebugPrint((MAKE_LEVEL(GENERIC, DEBUG), "cbMode_GetDefaultModeList: Total mode num is %d!\n", ulModeNum));
    
    return ulModeNum;
}


// Get the mode counts from new CBIOS adapter mode timing table.
// This mode list does not include the interlace mode.
CBIOS_VOID cbMode_GetAdapterModeNum(PCBIOS_VOID pvcbe, CBIOS_U32* AdapterModeNum)
{
    CBIOS_U32 ulModeCount = sizeofarray(AdapterModeList);
    
    *AdapterModeNum = ulModeCount;
}

CBIOS_STATUS cbMode_FillAdapterModeList(PCBIOS_VOID pvcbe, PCBiosModeInfoExt pModeList, CBIOS_U32 *pBufferSize)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    CBIOS_U32  InputNum = *pBufferSize/sizeof(CBiosModeInfoExt);
    CBIOS_U32  CopyNum = (InputNum < sizeofarray(AdapterModeList))? InputNum : sizeofarray(AdapterModeList);
    
    if (pModeList == CBIOS_NULL)
    {
        cbDebugPrint((MAKE_LEVEL(GENERIC, ERROR),"cbMode_FillAdapterModeList: pModeList is null.\n"));
        return CBIOS_ER_NULLPOINTER;
    }

    cb_memset(pModeList, 0, *pBufferSize);

    cb_memcpy(pModeList, AdapterModeList, CopyNum * sizeof(CBiosModeInfoExt));

    *pBufferSize = CopyNum * sizeof(CBiosModeInfoExt);
    
    return CBIOS_OK;
}


CBIOS_VOID cbMode_GetFilterPara(PCBIOS_VOID pvcbe, CBIOS_ACTIVE_TYPE Device, PCBIOS_MODE_FILTER_PARA pFilter)
{
    PCBIOS_EXTENSION_COMMON    pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    CBIOS_MONITOR_TYPE         MonitorType = CBIOS_MONITOR_TYPE_NONE;
    PCBIOS_DEVICE_COMMON       pDevCommon = cbGetDeviceCommon(&pcbe->DeviceMgr, Device);
    PCBIOS_VOID                pDPMonitorContext = CBIOS_NULL;
    
    
    MonitorType = pDevCommon->CurrentMonitorType;

    switch(MonitorType)
    {
    case CBIOS_MONITOR_TYPE_DVI:
        pFilter->MaxDclk = 1650000;        
        break;
    case CBIOS_MONITOR_TYPE_HDMI:
        pFilter->MaxDclk = pcbe->ChipLimits.ulMaxHDMIClock;
        break;
    case CBIOS_MONITOR_TYPE_MHL:
        pFilter->MaxDclk = pcbe->ChipLimits.ulMaxMHLClock;
        break;
    case CBIOS_MONITOR_TYPE_DP:
    case CBIOS_MONITOR_TYPE_PANEL:
        pDPMonitorContext = cbGetDPMonitorContext(pcbe, pDevCommon);
        pFilter->MaxDclk = cbDPMonitor_GetMaxSupportedDclk(pcbe, pDPMonitorContext);
        break;
    case CBIOS_MONITOR_TYPE_CRT:
        pFilter->MaxDclk = 4000000;
        break;
    default:                                       
        pFilter->MaxDclk = 3300000;              
        break;
    }

    if(pcbe->SysBiosInfo.Version >= 0x11)
    {
        if(((Device == CBIOS_TYPE_DP1) && (pcbe->SysBiosInfo.Dp1PortConnType == CBIOS_DP_CONN))
            || ((Device == CBIOS_TYPE_DP2) && (pcbe->SysBiosInfo.Dp2PortConnType == CBIOS_DP_CONN))
            || ((Device == CBIOS_TYPE_DP3) && (pcbe->SysBiosInfo.Dp3PortConnType == CBIOS_DP_CONN)))
        {
            if(MonitorType == CBIOS_MONITOR_TYPE_HDMI)
            {
                pFilter->MaxDclk = 3400000;
            }
        }
    }

    if(pFilter->MaxDclk > pcbe->ChipLimits.ulMaxIGAClock)
    {
        pFilter->MaxDclk = pcbe->ChipLimits.ulMaxIGAClock;
    }

    if((pcbe->SVID == 0x17AA) && (pcbe->SSID == 0x350B) && (Device == CBIOS_TYPE_DP1))
    {
        pFilter->MaxDclk = 7000000;
    }

    pFilter->bFilterInterlace = CBIOS_TRUE;
}

static CBIOS_BOOL  cbMode_NeedFilterMode(PCBIOS_EXTENSION_COMMON    pcbe,
                                PCBIOS_DEVICE_TIMING_INFO pMode,
                                CBIOS_ACTIVE_TYPE  Device,
                                CBIOS_MONITOR_TYPE MonitorType,
                                PCBIOS_MODE_FILTER_PARA pFilter)
{
    CBIOS_BOOL  bFilterThisMode = CBIOS_FALSE;
    CBIOS_BOOL bSnoopOnly = CBIOS_FALSE;
    CBIOS_U32  FbSize = 0;
    CBIOS_U16 MaxX = 0, MaxY = 0, MaxRR = 0;
    
    // check whether to filter the mode
    if ((pFilter->bFilterInterlace) && (pMode->IsInterLaced))
    {
        bFilterThisMode = CBIOS_TRUE;
    }
    else if (pMode->FullTiming.PixelClock > pFilter->MaxDclk)
    {
        bFilterThisMode = CBIOS_TRUE;
        cbDebugPrint((MAKE_LEVEL(GENERIC, DEBUG), "----Mode pixel clk(%d) greater than the max DClk(%d), filter this mode-----\n",
            pMode->FullTiming.PixelClock, pFilter->MaxDclk));
    }
    else if(pcbe->ChipID >= CHIPID_CHX004)
    {
        FbSize = 1 << (pcbe->SysBiosInfo.FBSize & 0x0F);
        bSnoopOnly = pcbe->SysBiosInfo.SnoopOnly;

        MaxX = pMode->FullTiming.XRes;
        MaxY = pMode->FullTiming.YRes;
        MaxRR = pMode->FullTiming.RefreshRate;

        if(FbSize <= 64)//Fb<= 64M, max mode is 1080p
        { 
            if(MonitorType != CBIOS_MONITOR_TYPE_PANEL)
            {
                MaxX = 1920;
                MaxY = 1080;
                MaxRR = 6000;
            }
        }
        else
        {
            if(!bSnoopOnly) //Fb > 64M, not snoop only, max mode is 4K for HDMI/DP, 2K for CRT
            {
                if(MonitorType == CBIOS_MONITOR_TYPE_CRT)
                {
                    MaxX = 2048;//2K
                    MaxY = 1536;
                    MaxRR = 8500;// 85Hz
                }
                else if(MonitorType == CBIOS_MONITOR_TYPE_DVI)
                {
                    MaxX = 1920;
                    MaxY = 1200;
                    MaxRR = 6000;
                }
                else
                {
                    MaxX = 4096;
                    MaxY = 2160;
                    MaxRR = 6000;
                }
                if((pcbe->SVID == 0x8888) && (pcbe->SSID == 0x00ED)) //set max mode 3840x2160@30 for Yidao notebook type-c port
                {
                    if((Device == CBIOS_TYPE_DP2) && (MonitorType == CBIOS_MONITOR_TYPE_DP))//Yidao type-c port is binded with DP2
                    {
                        MaxX = 3840;
                        MaxY = 2160;
                        MaxRR = 3000;
                    }
                }
            }
            else //Fb > 64M, snoop only, max mode is 1080p
            {
                //Snoop only:  Do not filter mode for edp
                if(MonitorType != CBIOS_MONITOR_TYPE_PANEL)
                {
                    if(FbSize >= 2*1024)
                    {
                        MaxX = 4096;
                        MaxY = 2160;
                        MaxRR = 6000;
                    }                   
                    else
                    {
                        MaxX = 1920;
                        MaxY = 1080;
                        MaxRR = 6000;
                    }
               }
            }
        }

        if(MaxX && MaxY && MaxRR)
        {
            if((pMode->FullTiming.XRes > MaxX) || (pMode->FullTiming.YRes > MaxY))
            {
                bFilterThisMode = CBIOS_TRUE;
                cbDebugPrint((MAKE_LEVEL(GENERIC, DEBUG),"Filter mode %d x %d @ %d as it is greater than config max mode.\n",
                              pMode->FullTiming.XRes, pMode->FullTiming.YRes, pMode->FullTiming.RefreshRate));
            }
            else if((pMode->FullTiming.XRes == MaxX) && (pMode->FullTiming.YRes == MaxY)
                     && (pMode->FullTiming.RefreshRate > MaxRR))
            {
                cbDebugPrint((MAKE_LEVEL(GENERIC, DEBUG),"Filter mode %d x %d @ %d as it is greater than config max mode.\n",
                              pMode->FullTiming.XRes, pMode->FullTiming.YRes, pMode->FullTiming.RefreshRate));
                bFilterThisMode = CBIOS_TRUE;
            }
        }
    }

    return bFilterThisMode;
}

CBIOS_U32 cbMode_MakeDeviceModeList(PCBIOS_VOID pvcbe, CBIOS_ACTIVE_TYPE  Device)
{
    PCBIOS_EXTENSION_COMMON    pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    PCBIOS_DEVICE_COMMON       pDevCommon = cbGetDeviceCommon(&pcbe->DeviceMgr, Device);
    PCBIOS_EDID_STRUCTURE_DATA pEdidStruct = &(pDevCommon->EdidStruct);
    CBIOS_MODE_FILTER_PARA Filter = {0}; 
    PCBIOS_DEVICE_TIMING_INFO pTiming = CBIOS_NULL;
    CBiosModeInfoExt  ModeToInsert = {0};
    CBIOS_BOOL  bHasPrefer = CBIOS_FALSE;
    CBIOS_U32  Index = 0;

    if(pDevCommon->pDeviceModeList)
    {
        cb_FreePool(pDevCommon->pDeviceModeList);
        pDevCommon->pDeviceModeList = CBIOS_NULL;
    }
    pDevCommon->ValidModeNum = 0;
    pDevCommon->ModeListArraySize = 0;
    
    if(!pEdidStruct->TimingNum)
    {
        return 0;
    }

    cbMode_GetFilterPara(pcbe, pDevCommon->DeviceType, &Filter);

    for(Index = 0; Index < pEdidStruct->TimingNum; Index++)
    {
        pEdidStruct->pDeviceTimingList[Index].Merged = 0;
    }

    while(CBIOS_TRUE)
    {
        pTiming = CBIOS_NULL;

        for(Index = 0; Index < pEdidStruct->TimingNum; Index++)
        {
            if(pEdidStruct->pDeviceTimingList[Index].Merged)
            {
                continue;
            }

            if(!pTiming)
            {
                pTiming = &pEdidStruct->pDeviceTimingList[Index];
            }
            else
            {
                CBIOS_S32 Ret = cbMode_CompareMode(&pEdidStruct->pDeviceTimingList[Index], pTiming);
                if(Ret > 0)
                {
                    pTiming = &pEdidStruct->pDeviceTimingList[Index];
                }
                else if(Ret == 0)
                {
                    if(pEdidStruct->pDeviceTimingList[Index].IsPreferMode)
                    {
                        pTiming->Merged = 1;
                        pTiming = &pEdidStruct->pDeviceTimingList[Index];
                    }
                    else
                    {
                        pEdidStruct->pDeviceTimingList[Index].Merged = 1;
                    }
                }
            }
        }

        if(!pTiming)
        {
            break;
        }

        pTiming->Merged = 1;
        
        if (!cbMode_NeedFilterMode(pcbe, pTiming, Device, 
                             pDevCommon->CurrentMonitorType, &Filter))
        {
            cb_memset(&ModeToInsert, 0, sizeof(CBiosModeInfoExt));
            ModeToInsert.XRes = pTiming->FullTiming.XRes;
            ModeToInsert.YRes = pTiming->FullTiming.YRes;
            ModeToInsert.RefreshRate = pTiming->FullTiming.RefreshRate;
            ModeToInsert.isInterlaceMode = pTiming->IsInterLaced;
            ModeToInsert.XTotal = pTiming->FullTiming.HorTotal;
            ModeToInsert.YTotal = pTiming->FullTiming.VerTotal;
            ModeToInsert.PixelClock = pTiming->FullTiming.PixelClock;
            ModeToInsert.ColorDepthCaps = DEVICE_MODE_DEPTH_CAPS;
            ModeToInsert.isPreferredMode = (pTiming->IsPreferMode && !bHasPrefer)? 1 : 0;  //one prefer is enough
            if(ModeToInsert.isPreferredMode)
            {
                bHasPrefer = CBIOS_TRUE;
            }

            cbMode_AddDeviceMode(pDevCommon, &ModeToInsert, pTiming);
        }
    }

    if(!bHasPrefer && pDevCommon->pDeviceModeList)
    {
        pDevCommon->pDeviceModeList[0].isPreferredMode = 1; //make sure there is a prefer mode, if none, set the 1st mode as prefer
    }
    
    return pDevCommon->ValidModeNum;
}   

