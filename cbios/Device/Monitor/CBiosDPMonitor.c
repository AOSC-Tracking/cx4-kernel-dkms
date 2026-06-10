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
** DP monitor interface function implementation.
**
** NOTE:
** DP monitor ONLY characters like link training, DPCD, Aux read/write, SHOULD add to this file.
******************************************************************************/

#include "CBiosChipShare.h"
#include "../../Hw/HwBlock/CBiosDIU_DP.h"
#include "../../Hw/CBiosHwShare.h"
#include "../../Hw/HwBlock/CBiosDIU_HDCP.h"
#include "../../Hw/HwBlock/CBiosDIU_HDAC.h"

#if DP_MONITOR_SUPPORT

static CBIOS_U8 DPFailSafeModeEdid[] =
{
    //For DP EDID corruption issue, set fail-safe mode: 640x480@60Hz, bpc=6
    0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x04, 0x43, 0x14, 0x81, 0x89, 0xDA, 0x07, 0x00, 
    0x0A, 0x12, 0x01, 0x04, 0x95, 0x41, 0x29, 0x78, 0xE2, 0x8F, 0x95, 0xAD, 0x4F, 0x32, 0xB2, 0x25,
    0x0F, 0x50, 0x54, 0x20, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0xD6, 0x09, 0x80, 0xA0, 0x20, 0xE0, 0x2D, 0x10, 0x10, 0x60,
    0xA2, 0x00, 0x8A, 0x9A, 0x21, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0xFF, 0x00, 0x44, 0x50, 0x31, 
    0x39, 0x30, 0x35, 0x32, 0x38, 0x38, 0x37, 0x45, 0x54, 0x0A, 0x00, 0x00, 0x00, 0xFC, 0x00, 0x41,
    0x42, 0x43, 0x20, 0x33, 0x30, 0x32, 0x30, 0x30, 0x38, 0x57, 0x53, 0x0A, 0x00, 0x00, 0x00, 0xFD, 
    0x00, 0x31, 0x56, 0x1D, 0x71, 0x1C, 0x00, 0x0A, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x01, 0xF5,
    0x70, 0x10, 0x18, 0x03, 0x00, 0x01, 0x00, 0x0C, 0x8A, 0x02, 0x9A, 0x01, 0x00, 0x0A, 0x40, 0x06, 
    0x18, 0x78, 0x3C, 0x75, 0x0D, 0x00, 0x06, 0x88, 0x20, 0x20, 0x40, 0x20, 0x20, 0xB5, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x90,
};


//bUseLastPassed: 0, link speed/lane number is calculated from cbDPMonitor_DetermineLinkTrainingPara, used in full LT;
// 1, Use last successed link speed/lane number to do fast LT, used in restore link status such as exit PSR
CBIOS_BOOL cbDPMonitor_LinkTrainingHw(PCBIOS_VOID pvcbe, PCBIOS_DP_MONITOR_CONTEXT pDPMonitorContext, CBIOS_BOOL  bUseLastPassed)
{
    PCBIOS_EXTENSION_COMMON pcbe          = (PCBIOS_EXTENSION_COMMON)pvcbe;
    CBIOS_LINK_TRAINING_PARAMS LinkTrainingParams = {0};
    CBIOS_BOOL  bStatus = CBIOS_FALSE;
    CBIOS_EDP_CP_METHOD CPMethod = CBIOS_EDP_CP_DISABLE;
    CBIOS_MODULE_INDEX DPModuleIndex = cbGetModuleIndex(pcbe, pDPMonitorContext->pDevCommon->DeviceType, CBIOS_MODULE_TYPE_DP);

    cbTraceEnter(DP);

    LinkTrainingParams.bEDP = CBIOS_FALSE;
    if(bUseLastPassed && pDPMonitorContext->LinkPassSpeed && pDPMonitorContext->LinkPassLaneNum)
    {
        LinkTrainingParams.MaxLaneCount = pDPMonitorContext->LinkPassLaneNum;
        LinkTrainingParams.MaxLinkSpeed = pDPMonitorContext->LinkPassSpeed;
    }
    else
    {
        LinkTrainingParams.MaxLaneCount = pDPMonitorContext->LaneNumberToUse;
        LinkTrainingParams.MaxLinkSpeed = pDPMonitorContext->LinkSpeedToUse;
    }
    LinkTrainingParams.DpSinkVersion = pDPMonitorContext->DpSinkVersion;
    LinkTrainingParams.TrainingAuxRdInterval = pDPMonitorContext->TrainingAuxRdInterval;
    LinkTrainingParams.bEnableTPS3 = pDPMonitorContext->bEnableTPS3;

    LinkTrainingParams.DpUsbMode = pDPMonitorContext->CurrentDpUsbMode;
    LinkTrainingParams.AltReversePlug = pDPMonitorContext->AltReversePlug;
    if (pDPMonitorContext->CurrentDpUsbMode == CBIOS_DP_TUNNEL)
    {
        LinkTrainingParams.LttprCount = pDPMonitorContext->LttprCount;
        LinkTrainingParams.LttprNonTransparent = pDPMonitorContext->LttprNonTransparent;

        // cne001 usb cannot return combined value among LTTPR(s) and sink, need hardcode
        LinkTrainingParams.TrainingAuxRdInterval = 0x4;

        if (pDPMonitorContext->LttprCount && pDPMonitorContext->LttprCount <= DP_MAX_LTTPR_CNT)
        {
            cb_memcpy(LinkTrainingParams.LttprAuxRDInterval, pDPMonitorContext->LttprAuxRDInterval, pDPMonitorContext->LttprCount);
            cb_memcpy(LinkTrainingParams.LttprVolPELevel3, pDPMonitorContext->LttprVolPELevel3, pDPMonitorContext->LttprCount);
        }
    }

    //check CP method
    if (cbDIU_EDP_IsEDPSupported(pcbe, DPModuleIndex))
    {
        LinkTrainingParams.bEDP = CBIOS_TRUE;

        if (pcbe->CbiosFlags & DISABLE_EDP_CONTENT_PROTECTION)
        {
            CPMethod = CBIOS_EDP_CP_DISABLE;
        }
        else if (pDPMonitorContext->bSupportASSR && pDPMonitorContext->bSupportAF)
        {
            if (pcbe->CbiosFlags & DEFAULT_USE_EDP_CP_METHOD_3A_ASSR)
            {
                CPMethod = CBIOS_EDP_CP_ASSR;
            }
            else if (pcbe->CbiosFlags & DEFAULT_USE_EDP_CP_METHOD_3B_AF)
            {
                CPMethod = CBIOS_EDP_CP_AF;
            }
        }
        else if (pDPMonitorContext->bSupportASSR)
        {
            CPMethod = CBIOS_EDP_CP_ASSR;
        }
        else if (pDPMonitorContext->bSupportAF)
        {
            CPMethod = CBIOS_EDP_CP_AF;
        }
        else 
        {
            CPMethod = CBIOS_EDP_CP_DISABLE;
        }
    }
    else
    {
        LinkTrainingParams.bEDP = CBIOS_FALSE;
        CPMethod = CBIOS_EDP_CP_DISABLE;
    }

    LinkTrainingParams.CPMethod = CPMethod;
    LinkTrainingParams.bEnhancedMode = pDPMonitorContext->EnhancedMode;

    if(pDPMonitorContext->bUseLinkRateSet)
    {
        bStatus = cbDIU_DP_LT_With_DPCD115(pcbe, DPModuleIndex, &LinkTrainingParams);
    }
    else
    {
        bStatus = cbDIU_DP_LinkTrainingHw(pcbe, DPModuleIndex, &LinkTrainingParams);
    }
    if (bStatus)
    {
        pDPMonitorContext->LinkPassSpeed = LinkTrainingParams.CurrLinkSpeed;
        pDPMonitorContext->LinkPassLaneNum = LinkTrainingParams.CurrLaneCount;
    }

    cbTraceExit(DP);

    return bStatus;
}

static CBIOS_BOOL cbDPMonitor_GetAutoTestDpcdData(PCBIOS_EXTENSION_COMMON pcbe, PCBIOS_DP_MONITOR_CONTEXT pDPMonitorContext)
{
    CBIOS_MODULE_INDEX DPModuleIndex = cbGetModuleIndex(pcbe, pDPMonitorContext->pDevCommon->DeviceType, CBIOS_MODULE_TYPE_DP);
    CBIOS_U8  Data[2] = {0};
    DPCD_REG_00219  DPCD_00219;
    DPCD_REG_00220  DPCD_00220;
    CBIOS_U32  LaneNum = 0, LinkSpeed = 0;

    if(!cbDIU_Aux_Dpcd_Read(pcbe, DPModuleIndex, 0x219, Data, 1))
    {
        cbDebugPrint((MAKE_LEVEL(DP, ERROR), "%s: Can not get the auto test DPCD link rate data!\n", FUNCTION_NAME));
        return CBIOS_FALSE;
    }

    DPCD_00219.Value = Data[0];
    if (DPCD_00219.TEST_LINK_RATE == CBIOS_DPCD_LINK_RATE_5400Mbps)
    {
        LinkSpeed = CBIOS_DP_LINK_SPEED_5400Mbps;
    }
    else if (DPCD_00219.TEST_LINK_RATE == CBIOS_DPCD_LINK_RATE_2700Mbps)
    {
        LinkSpeed = CBIOS_DP_LINK_SPEED_2700Mbps;
    }
    else
    {
        LinkSpeed = CBIOS_DP_LINK_SPEED_1620Mbps;
    }

    if(!cbDIU_Aux_Dpcd_Read(pcbe, DPModuleIndex, 0x220, Data, 1))
    {
        cbDebugPrint((MAKE_LEVEL(DP, ERROR), "%s: Can not get the auto test DPCD lane count data!\n", FUNCTION_NAME));
        return CBIOS_FALSE;
    }
    
    DPCD_00220.Value = Data[0];
    if ((DPCD_00220.TEST_LANE_COUNT == 0x01) || (DPCD_00220.TEST_LANE_COUNT == 0x02) || (DPCD_00220.TEST_LANE_COUNT == 0x04))
    {
        LaneNum = DPCD_00220.TEST_LANE_COUNT;
    }
    else
    {
        cbDebugPrint((MAKE_LEVEL(DP, ERROR), "%s: Invalid lane count:%d!\n", FUNCTION_NAME, DPCD_00220.TEST_LANE_COUNT));
        return CBIOS_FALSE;
    }

    pDPMonitorContext->LaneNumberToUse = LaneNum;
    pDPMonitorContext->LinkSpeedToUse = LinkSpeed;

    return CBIOS_TRUE;
}

static CBIOS_BOOL cbDPMonitor_GetSinkCapsFromSpecificPlace(PCBIOS_EXTENSION_COMMON pcbe, PCBIOS_DP_MONITOR_CONTEXT pDPMonitorContext, CBIOS_U8 IGAIndex)
{
    PCBiosCustmizedDestTiming pUserTiming = CBIOS_NULL;   

    if(pcbe->SpecifyDestTimingSrc[IGAIndex].Flag == 1)
    {
        pUserTiming = &pcbe->SpecifyDestTimingSrc[IGAIndex].UserCustDestTiming;
    }
    else if(pcbe->SpecifyDestTimingSrc[IGAIndex].Flag == 2)
    {
        pUserTiming = &pDPMonitorContext->TestDpcdDataTiming;
    }
    else
    {  
        return CBIOS_FALSE;
    }

    if ((pUserTiming->LinkRate == CBIOS_DPCD_LINK_RATE_5400Mbps) ||
        (pUserTiming->LinkRate == CBIOS_DPCD_LINK_RATE_2700Mbps) ||
        (pUserTiming->LinkRate == CBIOS_DPCD_LINK_RATE_1620Mbps))
    {
        pDPMonitorContext->LaneNumberToUse = pUserTiming->LaneCount;
        if(pUserTiming->LinkRate == CBIOS_DPCD_LINK_RATE_5400Mbps)
        {
            pDPMonitorContext->LinkSpeedToUse = CBIOS_DP_LINK_SPEED_5400Mbps;
        }
        else if(pUserTiming->LinkRate == CBIOS_DPCD_LINK_RATE_2700Mbps)
        {
            pDPMonitorContext->LinkSpeedToUse = CBIOS_DP_LINK_SPEED_2700Mbps;
        }
        else
        {
            pDPMonitorContext->LinkSpeedToUse = CBIOS_DP_LINK_SPEED_1620Mbps;
        }

        if(pUserTiming->DClk)
        {
            pDPMonitorContext->bpc = pUserTiming->BitDepthPerComponet;
            pDPMonitorContext->AsyncMode = (pUserTiming->ClockSynAsyn == 0) ? 1 : 0;
            pDPMonitorContext->ColorFormat = pUserTiming->ColorFormat;
            pDPMonitorContext->DynamicRange = pUserTiming->DynamicRange;
            pDPMonitorContext->YCbCrCoefficients = pUserTiming->YCbCrCoefficients;
            pDPMonitorContext->EnhancedMode = pUserTiming->EnhancedFrameMode;
        }

        cbDebugPrint((MAKE_LEVEL(DP, DEBUG), "%s: LinkRate = %d, LaneCount = %d, bpc = %d, Async = %d\n", FUNCTION_NAME,
            pDPMonitorContext->LinkSpeedToUse, pDPMonitorContext->LaneNumberToUse, pDPMonitorContext->bpc, pDPMonitorContext->AsyncMode));
        cbDebugPrint((MAKE_LEVEL(DP, DEBUG), "%s: ColorFormat = %d, DynamicRange = %d, YCbCrCoefficients = %d, EnhancedMode = %d\n", FUNCTION_NAME,
            pDPMonitorContext->ColorFormat,pDPMonitorContext->DynamicRange, pDPMonitorContext->YCbCrCoefficients, pDPMonitorContext->EnhancedMode));

        /*In order to pass dp cts on unigraf DPR-120, use max link rate and max lane count,
          because dpcd timing perhaps don't update, but these two params update everytime.
        */
        if(pcbe->SpecifyDestTimingSrc[IGAIndex].Flag == 2)
        {
            pDPMonitorContext->LinkSpeedToUse = pDPMonitorContext->SinkMaxLinkSpeed;
            pDPMonitorContext->LaneNumberToUse = pDPMonitorContext->SinkMaxLaneCount;
        }

        return CBIOS_TRUE;
    }
    else
    {
        return CBIOS_FALSE;
    }
}

//For some eDP panel, lower link rate such as 5.4G can meet requirement of its dclk, but its max sink link rate is set to 8.1G
//We can't support 8.1G for native DP by now, so LT will fail on such panel. To use max 5.4G in this case
#define EDP_PREFER_MAX_LINK_RATE  CBIOS_DP_LINK_SPEED_5400Mbps

static CBIOS_VOID cbDPMonitor_DetermineLinkTrainingPara(PCBIOS_EXTENSION_COMMON pcbe, PCBIOS_DP_MONITOR_CONTEXT pDPMonitorContext, CBIOS_BOOL isAutoTest)
{
    CBIOS_U32 MaxSupportClock_5400Mbps = 0, MaxSupportClock_2700Mbps = 0, MaxSupportClock_1620Mbps = 0, MaxClockForCurrentLinkSpeed = 0;
    CBIOS_U8  IGAIndex = 0;
    CBIOS_MODULE_INDEX DPModuleIndex = cbGetModuleIndex(pcbe, pDPMonitorContext->pDevCommon->DeviceType, CBIOS_MODULE_TYPE_DP);
    PCBIOS_DP_CONTEXT  pDpContext = container_of(pDPMonitorContext->pDevCommon, PCBIOS_DP_CONTEXT, Common);
    PCBIOS_EDPPanel_PARAMS    pEDPPanelDevice =  &(pDPMonitorContext->pDevCommon->DeviceParas.EDPPanelDevice);
    CBIOS_U32  LinkRateEntry = 0, LinkSpeed = 0, LaneNum = 0;
    CBIOS_BOOL  bGotLinkPara = CBIOS_FALSE;

    IGAIndex = DPModuleIndex;

    //for dp cts, if set registry for dp cts, Flag = 2, but some test of DPR120's dpcd timing is invalid.

    if(pDpContext->DPPortParams.bRunCTS)
    {
        pcbe->SpecifyDestTimingSrc[IGAIndex].Flag = 2;
    }

    if (isAutoTest)
    {
        bGotLinkPara = cbDPMonitor_GetAutoTestDpcdData(pcbe, pDPMonitorContext);
    }
    else if (pcbe->SpecifyDestTimingSrc[IGAIndex].Flag & 0x03)
    {
        bGotLinkPara = cbDPMonitor_GetSinkCapsFromSpecificPlace(pcbe, pDPMonitorContext, IGAIndex);
    }

    if(!bGotLinkPara)
    {
        if(pDPMonitorContext->bpc > DP_Default_bpc)
        {
            pDPMonitorContext->bpc = DP_Default_bpc;
        }

        LaneNum = pDPMonitorContext->SinkMaxLaneCount;

        if ((pDPMonitorContext->CurrentDpUsbMode == CBIOS_DP_ALTMODE) && (LaneNum > pDPMonitorContext->AltMaxLaneCount))
        {
            LaneNum = pDPMonitorContext->AltMaxLaneCount;
        }

        if ((pDPMonitorContext->CurrentDpUsbMode == CBIOS_DP_TUNNEL) && (pDPMonitorContext->LttprCount) &&
            (LaneNum > pDPMonitorContext->LttprMaxLaneCount))
        {
            LaneNum = pDPMonitorContext->LttprMaxLaneCount;
        }

        // choose Link Speed according to current mode's pixel clock
        MaxSupportClock_5400Mbps = (CBIOS_DP_LINK_SPEED_5400Mbps / (pDPMonitorContext->bpc * 3)) * LaneNum * 8;
        MaxSupportClock_2700Mbps = (CBIOS_DP_LINK_SPEED_2700Mbps / (pDPMonitorContext->bpc * 3)) * LaneNum * 8;
        MaxSupportClock_1620Mbps = (CBIOS_DP_LINK_SPEED_1620Mbps / (pDPMonitorContext->bpc * 3)) * LaneNum * 8;
        
        if (pDPMonitorContext->TargetTiming.PixelClock < MaxSupportClock_1620Mbps)
        {
            LinkSpeed = CBIOS_DP_LINK_SPEED_1620Mbps;
        }
        else if (pDPMonitorContext->TargetTiming.PixelClock < MaxSupportClock_2700Mbps)
        {
            LinkSpeed = CBIOS_DP_LINK_SPEED_2700Mbps;
        }
        else if (pDPMonitorContext->TargetTiming.PixelClock < MaxSupportClock_5400Mbps)
        {
            LinkSpeed = CBIOS_DP_LINK_SPEED_5400Mbps;
        }
        else
        {
            LinkSpeed = CBIOS_DP_LINK_SPEED_8100Mbps;
        }

        if (LinkSpeed > pDPMonitorContext->SinkMaxLinkSpeed)
        {
            LinkSpeed = pDPMonitorContext->SinkMaxLinkSpeed;
        }

        if ((pDPMonitorContext->CurrentDpUsbMode == CBIOS_DP_TUNNEL) && (pDPMonitorContext->LttprCount) &&
            (LinkSpeed > pDPMonitorContext->LttprMaxLinkRate))
        {
            LinkSpeed = pDPMonitorContext->LttprMaxLinkRate;
        }

        if(cbDIU_EDP_IsEDPSupported(pcbe, DPModuleIndex))
        {
            if(pEDPPanelDevice &&pEDPPanelDevice->EDPPanelDesc.EDPCaps.isHardcodeLinkPara)
            {
                LaneNum = pEDPPanelDevice->EDPPanelDesc.EDPCaps.LaneNum;
                LinkSpeed = pEDPPanelDevice->EDPPanelDesc.EDPCaps.LinkSpeed;
            }
            else if(pDPMonitorContext->SinkMaxLinkSpeed <= EDP_PREFER_MAX_LINK_RATE)
            {
                LinkSpeed = pDPMonitorContext->SinkMaxLinkSpeed;     
            }
            
            if(pDPMonitorContext->bUseLinkRateSet)
            {
                LinkRateEntry = pDPMonitorContext->SupportedLinkRateNum - 1;
                LinkSpeed = pDPMonitorContext->SupportedLinkRates[LinkRateEntry];//select max supported link_rate
            }
        }

        /* To avoid exceeding max clock when lighten 10bpc or higher, use default bpc*/
        MaxClockForCurrentLinkSpeed = (LinkSpeed / (pDPMonitorContext->bpc * 3)) * LaneNum * 8;
        if(pDPMonitorContext->TargetTiming.PixelClock > MaxClockForCurrentLinkSpeed
            && pDPMonitorContext->bpc > DP_Default_bpc)
        {
            cbDebugPrint((MAKE_LEVEL(DP, DEBUG), "%s: LinkSpeed(%d) can't support PixelClock(%d) with %d bit, use default bpc\n",
                            FUNCTION_NAME, LinkSpeed, pDPMonitorContext->TargetTiming.PixelClock, pDPMonitorContext->bpc));

            pDPMonitorContext->bpc = DP_Default_bpc;
        }

        pDPMonitorContext->LinkSpeedToUse = LinkSpeed;
        pDPMonitorContext->LaneNumberToUse = LaneNum;
    }
    
    // determine final link training params according to Source's caps
    if (pDPMonitorContext->LaneNumberToUse > pDPMonitorContext->SourceMaxLaneCount)
    {
        cbDebugPrint((MAKE_LEVEL(DP, WARNING), "%s: source only supports lane count = %d\n",
            FUNCTION_NAME, pDPMonitorContext->SourceMaxLaneCount));
        pDPMonitorContext->LaneNumberToUse = pDPMonitorContext->SourceMaxLaneCount;
    }

    if (pDPMonitorContext->LinkSpeedToUse > pDPMonitorContext->SourceMaxLinkSpeed)
    {
        cbDebugPrint((MAKE_LEVEL(DP, WARNING), "%s: source only supports link speed = %d\n",
            FUNCTION_NAME, pDPMonitorContext->SourceMaxLinkSpeed));
        pDPMonitorContext->LinkSpeedToUse = pDPMonitorContext->SourceMaxLinkSpeed;
    }

    if (pDPMonitorContext->bSourceSupportTPS3)
    {
        pDPMonitorContext->bEnableTPS3 = pDPMonitorContext->bSupportTPS3;
    }
    else
    {
        if (pDPMonitorContext->bSupportTPS3)
        {
            cbDebugPrint((MAKE_LEVEL(DP, WARNING), "%s: source doesn't support TPS3\n", FUNCTION_NAME));
        }
        pDPMonitorContext->bEnableTPS3 = 0;
    }
}

CBIOS_BOOL cbDPMonitor_SetUpMainLink(PCBIOS_VOID pvcbe, PCBIOS_DP_MONITOR_CONTEXT pDPMonitorContext)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    CBIOS_BOOL              bStatus = CBIOS_FALSE;
    PCBIOS_TIMING_ATTRIB    pTiming = &pDPMonitorContext->TargetTiming;
    CBIOS_MODULE_INDEX      DPModuleIndex = cbGetModuleIndex(pcbe, pDPMonitorContext->pDevCommon->DeviceType, CBIOS_MODULE_TYPE_DP);
    CBIOS_MODULE_INDEX      HDTVModuleIndex = cbGetModuleIndex(pcbe, pDPMonitorContext->pDevCommon->DeviceType, CBIOS_MODULE_TYPE_HDTV);
    CBIOS_MAIN_LINK_PARAMS  MainLinkParams = {0};

    cbTraceEnter(DP);

    pTiming->XRes *= pDPMonitorContext->PixelRepetition;
    pTiming->HorTotal *= pDPMonitorContext->PixelRepetition;
    pTiming->HorDisEnd *= pDPMonitorContext->PixelRepetition;
    pTiming->HorBStart *= pDPMonitorContext->PixelRepetition;
    pTiming->HorBEnd *= pDPMonitorContext->PixelRepetition;
    pTiming->HorSyncStart *= pDPMonitorContext->PixelRepetition;
    pTiming->HorSyncEnd *= pDPMonitorContext->PixelRepetition;

    if(pDPMonitorContext->bInterlace)
    {
        // HDTV module is used to transfer P -> i, so pixel clock need to be divided by 2
        if (HDTVModuleIndex != CBIOS_MODULE_INDEX_INVALID)
        {
            pTiming->PixelClock /= 2;
        }
        pTiming->YRes /= 2;
        pTiming->VerTotal /= 2;
        pTiming->VerDisEnd /= 2;
        pTiming->VerBStart /= 2;
        pTiming->VerBEnd /= 2;
        pTiming->VerSyncStart /= 2;
        pTiming->VerSyncEnd /= 2;
    }

    MainLinkParams.pTiming = pTiming;
    MainLinkParams.LinkedLaneNumber = pDPMonitorContext->LinkPassLaneNum;
    MainLinkParams.LinkedSpeed = pDPMonitorContext->LinkPassSpeed;
    MainLinkParams.bpc = pDPMonitorContext->bpc;
    MainLinkParams.TUSize = pDPMonitorContext->TUSize;
    MainLinkParams.AsyncMode = pDPMonitorContext->AsyncMode;
    MainLinkParams.ColorFormat = pDPMonitorContext->ColorFormat;
    MainLinkParams.DynamicRange = pDPMonitorContext->DynamicRange;
    MainLinkParams.YCbCrCoefficients = pDPMonitorContext->YCbCrCoefficients;

    bStatus = cbDIU_DP_SetUpMainLink(pcbe, DPModuleIndex, &MainLinkParams);

    cbTraceExit(DP);

    return bStatus;
}

static CBIOS_VOID cbDPMonitor_SetAudioInfoFrame(PCBIOS_EXTENSION_COMMON pcbe, PCBIOS_DP_MONITOR_CONTEXT pDPMonitorContext, PCBIOS_U8 FIFOIndex)
{

    CBIOS_ACTIVE_TYPE DeviceType = pDPMonitorContext->pDevCommon->DeviceType;
    CBIOS_MODULE_INDEX HDACModuleIndex = CBIOS_MODULE_INDEX_INVALID;
    CBIOS_U8 ucAAIData[32], i = 0, checksum = 0;
    CBIOS_U32 NumofChannels = 0;

    cb_memset(ucAAIData, 0, sizeof(ucAAIData));

    HDACModuleIndex = cbGetModuleIndex(pcbe, DeviceType, CBIOS_MODULE_TYPE_HDAC);
    if (HDACModuleIndex == CBIOS_MODULE_INDEX_INVALID)
    {
        cbDebugPrint((MAKE_LEVEL(DP, ERROR), "%s: invalid HDAC module index!\n", FUNCTION_NAME));
        return;
    }

    NumofChannels = cbDIU_HDAC_GetChannelNums(pcbe, HDACModuleIndex);

    ucAAIData[0] = 0;       // SDP ID (Must same with Audio Stream)
    ucAAIData[1] = 0x84;	// Inforframe Type = 0x84
    ucAAIData[2] = 0x1B;	// Length = 0x1B
    ucAAIData[3] = cb_min(pDPMonitorContext->DpSinkVersion, 0x12) << 2;	// infoframe SDP Version number(DP 1.3 and newer) or DP Version(DP 1.2)

    // CC2 CC1 CC0  |   Audio Channel Count
    // ---------------------------------------
    //  0   0   0   |   Refer to Stream Header
    //  0   0   1   |   2ch
    //  0   1   0   |   3ch
    //  0   1   1   |   4ch
    //  1   0   0   |   5ch
    //  1   0   1   |   6ch
    //  1   1   0   |   7ch
    //  1   1   1   |   8ch
    ucAAIData[4] |= NumofChannels;

    // CT3 CT2 CT1 CT0  |   Audio Coding Type
    // -------------------------------------------
    //  0   0   0   0   |   Refer to Stream Header
    //  0   0   0   1   |   IEC 60958 PCM[30,31]
    //  0   0   1   0   |   AC-3
    //  0   0   1   1   |   MPEG1 (Layer 1 & 2)
    ucAAIData[5] |= 0x00;

    // SS1 SS0  |   Sample Size
    // --------------------------
    //  0   0   |   Refer to Stream Header
    //  0   1   |   16 bit
    //  1   0   |   20 bit
    //  1   1   |   24 bit
    ucAAIData[6] |= 0x00;

    // SF2 SF1 SF0  |   Sampling Frequency
    // ---------------------------------------
    //  0   0   0   |   Refer to Stream Header
    //  0   0   1   |   32 kHz
    //  0   1   0   |   44.1 kHz(CD)
    //  0   1   1   |   48 kHz
    //  1   0   0   |   88.2 kHz
    //  1   0   1   |   96 kHz
    //  1   1   0   |   176.4 kHz
    //  1   1   1   |   192 kHz
    ucAAIData[6] |= 0x00;

    ucAAIData[7] = 0x00;

    // CA
    if(NumofChannels == 1)
    {
        ucAAIData[8] = 0;
    }
    else
    {
        ucAAIData[8] = cbDevHDACGetCAValue(pcbe, DeviceType);
    }

    ucAAIData[9] = 0x00;

    //all others are 0
    cbDPPort_WriteFIFO(pcbe, DeviceType, *FIFOIndex, ucAAIData, 32);
    (*FIFOIndex)++;
}

static CBIOS_VOID cbDPMonitor_SetInfoFrame(PCBIOS_VOID pvcbe, PCBIOS_DP_MONITOR_CONTEXT pDPMonitorContext)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    PCBIOS_DEVICE_COMMON    pDevCommon = pDPMonitorContext->pDevCommon;
    CBIOS_MODULE_INDEX      DPModuleIndex = cbGetModuleIndex(pcbe, pDevCommon->DeviceType, CBIOS_MODULE_TYPE_DP);
    CBIOS_U8                FIFOIndex = 0;

    // Audio Inforframe SDP
    cbDPMonitor_SetAudioInfoFrame(pcbe, pDPMonitorContext, &FIFOIndex);

    // Send
    cbDIU_DP_SendInfoFrame(pcbe, DPModuleIndex, FIFOIndex);
}

CBIOS_BOOL cbDPMonitor_LinkTraining(PCBIOS_VOID pvcbe, PCBIOS_DP_MONITOR_CONTEXT pDPMonitorContext, CBIOS_BOOL isAutoTest)
{
    CBIOS_BOOL    bStatus = CBIOS_FALSE;
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    CBIOS_MODULE_INDEX      DPModuleIndex = cbGetModuleIndex(pvcbe, pDPMonitorContext->pDevCommon->DeviceType, CBIOS_MODULE_TYPE_DP);

    cbTraceEnter(DP);

    if (pDPMonitorContext->DpAuxWorkingStatus & AUX_WORKING_STATUS_LINKTRAINING)
    {
        cbDebugPrint((MAKE_LEVEL(DP, ERROR), "%s: this routine re-rentered!!!\n", FUNCTION_NAME));
        return CBIOS_FALSE;
    }
    else if (pDPMonitorContext->DpAuxWorkingStatus & AUX_WORKING_STATUS_DATA_READING)
    {
        cbDebugPrint((MAKE_LEVEL(DP, WARNING), "%s:AUX CHANNEL IS BUSY READING EDID!!!\n", FUNCTION_NAME));
    }

    //Get the specified link training parameters
    cbDPMonitor_DetermineLinkTrainingPara(pcbe, pDPMonitorContext, isAutoTest);

    pDPMonitorContext->DpAuxWorkingStatus |= AUX_WORKING_STATUS_LINKTRAINING;

    if (pDPMonitorContext->CurrentDpUsbMode != CBIOS_DP_ALTMODE)
    {
        cbPHY_DP_InitEPHY(pcbe, DPModuleIndex);
    }

    cbPHY_DP_TxDriverInit(pcbe, DPModuleIndex, pDPMonitorContext->AltReversePlug, pDPMonitorContext->LaneNumberToUse);

    bStatus = cbDPMonitor_LinkTrainingHw(pcbe, pDPMonitorContext, CBIOS_FALSE);

    if (bStatus)
    {
        pDPMonitorContext->LT_Status = 1;
    }
    pDPMonitorContext->DpAuxWorkingStatus &= ~AUX_WORKING_STATUS_LINKTRAINING;

    cbTraceExit(DP);
    return bStatus;
}

CBIOS_BOOL cbDPMonitor_GetTrainingData(PCBIOS_VOID pvcbe,
                                       CBIOS_MODULE_INDEX DPModuleIndex,
                                       CBIOS_U32 ulLaneNum,
                                       CBIOS_U32 *DPCD202_205,
                                       CBIOS_U32 RequestVoltage[4],
                                       CBIOS_U32 RequestPreEmph[4])
{
    CBIOS_U32 i, DPCD206_207 = 0;
    CBIOS_U8  Data[6] = {0};
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;

    if(!cbDIU_Aux_Dpcd_Read(pcbe, DPModuleIndex, 0x202, Data, 6))
    {
        cbDebugPrint((MAKE_LEVEL(DP, ERROR), "%s: failed!\n", FUNCTION_NAME));
        return CBIOS_FALSE;
    }

    *DPCD202_205 = (Data[3]<<24) | (Data[2]<<16) | (Data[1]<<8) | Data[0];
    DPCD206_207 = (Data[5]<<8) | Data[4];
    cbDebugPrint((MAKE_LEVEL(DP, DEBUG), "%s: DPCD_207_202: 0x%04x%08x!\n", FUNCTION_NAME, DPCD206_207, *DPCD202_205));

    for (i = 0; i < ulLaneNum; i++)
    {
        RequestVoltage[i] = (DPCD206_207 >> (i * 4)) & 0x00000003;
        RequestPreEmph[i] = (DPCD206_207 >> (2 + i * 4)) & 0x00000003;

        //Verify ulTemp not indicate illegal voltage swing / pre-emphasis combinations
        while ((RequestVoltage[i] + RequestPreEmph[i]) > 3)
        {
            RequestPreEmph[i]--;
            cbDebugPrint((MAKE_LEVEL(DP, DEBUG), "%s: RequestVoltage+PreEmph >= 3, lower RequestPreEmph!\n", FUNCTION_NAME));
        }
    }

    return CBIOS_TRUE;
}

static CBIOS_BOOL cbDPMonitor_GetDPCDVersion(PCBIOS_EXTENSION_COMMON pcbe, PCBIOS_DP_MONITOR_CONTEXT pDPMonitorContext)
{    
    CBIOS_MODULE_INDEX  DPModuleIndex = cbGetModuleIndex(pcbe, pDPMonitorContext->pDevCommon->DeviceType, CBIOS_MODULE_TYPE_DP);
    CBIOS_BOOL  bStatus = CBIOS_FALSE;
    CBIOS_U8  Data[1] = {0};

    if(cbDIU_Aux_Dpcd_Read(pcbe, DPModuleIndex, 0, Data, 1) == CBIOS_TRUE)
    {
        pDPMonitorContext->DpSinkVersion = Data[0];
        bStatus = CBIOS_TRUE;
    }

    return bStatus;
}

static CBIOS_VOID cbDPMonitor_SetIgnoreMSATimingPara(PCBIOS_EXTENSION_COMMON pcbe, CBIOS_MODULE_INDEX DPModuleIndex, CBIOS_BOOL bIgnoreMSA)
{
    CBIOS_U8    Data[2] = {0};
    DPCD_REG_00007  DPCD00007;
    DPCD_REG_00107  DPCD00107;

    DPCD00007.Value = 0;
    if(cbDIU_Aux_Dpcd_Read(pcbe, DPModuleIndex, 0x7, Data, 1) != CBIOS_TRUE)
    {
        cbDebugPrint((MAKE_LEVEL(DP, ERROR), "%s: Read DPCD007 MSA_TIMING_PAR_IGNORED failed!\n", FUNCTION_NAME));
        return;
    }
    else
    {
        DPCD00007.Value = Data[0];
    }

    if (DPCD00007.MSA_TIMING_PAR_IGNORED)
    {
        DPCD00107.Value = 0;
        if (cbDIU_Aux_Dpcd_Read(pcbe, DPModuleIndex, 0x107, Data, 1) != CBIOS_TRUE)
        {
            cbDebugPrint((MAKE_LEVEL(DP, ERROR), "%s: Read DPCD107 MSA_TIMING_PAR_IGNORE_EN failed!\n", FUNCTION_NAME));
            return;
        }
        else
        {
            DPCD00107.Value = Data[0];
        }

        DPCD00107.MSA_TIMING_PAR_IGNORE_EN = (bIgnoreMSA) ? 1 : 0;
        Data[0] = DPCD00107.Value;
        cbDIU_Aux_Dpcd_Write(pcbe, DPModuleIndex, 0x107, Data, 1);
    }
}

static CBIOS_VOID cbDPMonitor_SetSinkPowerState(PCBIOS_EXTENSION_COMMON pcbe, CBIOS_MODULE_INDEX DPModuleIndex, CBIOS_BOOL bPowerOn)
{
    CBIOS_U8       Data[2] = {0};
    DPCD_REG_00600 DPCD00600;

    cbTraceEnter(DP);

    DPCD00600.Value = 0;
    if (bPowerOn)
    {
        DPCD00600.SET_POWER = 1; // normal operation mode
    }
    else
    {
        DPCD00600.SET_POWER = 2; // power down mode
    }

    Data[0] = DPCD00600.Value;

    if(bPowerOn)
    {
        // Source must try at least 3 times if no reply/response from Sink
        // Not all Sink device implemented this feature, so can't exit here
        if(!cbDIU_Aux_Dpcd_Write(pcbe, DPModuleIndex, 0x600, Data, 1))
        {
            cbDelayMilliSeconds(10);
            if(!cbDIU_Aux_Dpcd_Write(pcbe, DPModuleIndex, 0x600, Data, 1))
            {
                cbDelayMilliSeconds(20);
                if(!cbDIU_Aux_Dpcd_Write(pcbe, DPModuleIndex, 0x600, Data, 1))
                {
                    // 20090331_BC For DP compliance test 4.4.3
                    // QuantumData 882 won't ACK this power on request even after 3 attempts
                    // But if we started link training too early, the test failed 
                    // The test checks by monitoring the link training registers for certain period 
                    // so put in some delay here.  Do not reduce this number (40ms).
                    // Note that this would not affect normal operation since the Sink would usually
                    // respond to the very first power on request already
                    cbDelayMilliSeconds(40);
                }
            }
        }
    }
    else
    {
        if(!cbDIU_Aux_Dpcd_Write(pcbe, DPModuleIndex, 0x600, Data, 1))
        {
            cbDebugPrint((MAKE_LEVEL(DP, ERROR), "%s: Write DPCD power state D3 error!\n", FUNCTION_NAME));
        }
    }
    cbTraceExit(DP);
}

static CBIOS_BOOL cbDPMonitor_GetSinkCaps(PCBIOS_EXTENSION_COMMON pcbe, PCBIOS_DP_MONITOR_CONTEXT pDPMonitorContext)
{
    CBIOS_MODULE_INDEX  DPModuleIndex = cbGetModuleIndex(pcbe, pDPMonitorContext->pDevCommon->DeviceType, CBIOS_MODULE_TYPE_DP);
    CBIOS_BOOL          bStatus = CBIOS_FALSE;
    CBIOS_U8            Data[16];
    DPCD_REG_00001      DPCD_00001 = {0};
    DPCD_REG_00002      DPCD_00002 = {0};
    DPCD_REG_00005      DPCD_00005 = {0};
    DPCD_REG_00007      DPCD_00007 = {0};
    DPCD_REG_0000D      DPCD_0000D = {0};
    DPCD_REG_0000E      DPCD_0000E = {0};
    DPCD_REG_00200      DPCD_00200 = {0};
    DPCD_REG_00070      DPCD_00070 = {0};
    DPCD_REG_00701      DPCD_00701 = {0};
    DPCD_REG_00702      DPCD_00702 = {0};
    DPCD_REG_00703      DPCD_00703 = {0};
    DPCD_REG_00724      DPCD_00724 = {0};
    DPCD_REG_00725      DPCD_00725 = {0};
    DPCD_REG_00726      DPCD_00726 = {0};
    DPCD_REG_00071      DPCD_00071 = {0};
    CBIOS_U32           SinkCount = 0;
    CBIOS_U32           PsrSetupTime = 330;
    CBIOS_U32           LinkRate = 0, MaxLinkRate = 0;
    CBIOS_U32           SupportedLinkRateNum = 0, i = 0;

    cbTraceEnter(DP);

    // Read DP monitor Caps
    // In order to meet CTS item: 4.2.2.2, read 16 bytes although some of them are needless here.
    if(cbDIU_Aux_Dpcd_Read(pcbe, DPModuleIndex, 0, Data, 16))
    {
        DPCD_00001.Value = Data[1];
        if (DPCD_00001.MAX_LINK_RATE >= CBIOS_DPCD_LINK_RATE_8100Mbps)
        {
            pDPMonitorContext->SinkMaxLinkSpeed = CBIOS_DP_LINK_SPEED_8100Mbps;

            cbDebugPrint((MAKE_LEVEL(DP, DEBUG), "%s: link speed DP Sink max supports is 8.1Gpbs\n", FUNCTION_NAME));
        }
        else if (DPCD_00001.MAX_LINK_RATE == CBIOS_DPCD_LINK_RATE_5400Mbps)
        {
            pDPMonitorContext->SinkMaxLinkSpeed = CBIOS_DP_LINK_SPEED_5400Mbps;

            cbDebugPrint((MAKE_LEVEL(DP, DEBUG), "%s: link speed DP Sink max supports is 5.4Gpbs\n", FUNCTION_NAME));
        }
        else if (DPCD_00001.MAX_LINK_RATE == CBIOS_DPCD_LINK_RATE_2700Mbps)
        {
            pDPMonitorContext->SinkMaxLinkSpeed = CBIOS_DP_LINK_SPEED_2700Mbps;
            cbDebugPrint((MAKE_LEVEL(DP, DEBUG), "%s: link speed DP Sink max supports is 2.7Gpbs\n", FUNCTION_NAME));
        }
        else
        {
            pDPMonitorContext->SinkMaxLinkSpeed = CBIOS_DP_LINK_SPEED_1620Mbps;
            cbDebugPrint((MAKE_LEVEL(DP, DEBUG), "%s: link speed DP Sink max supports is 1.62Gpbs\n", FUNCTION_NAME));
        }

        // Check if anyone want to over write link speed in Registry.
        if (pcbe->CbiosFlags & HARDCODE_DP1_MAX_LINKSPEED_1620)
        {
            pDPMonitorContext->SinkMaxLinkSpeed = CBIOS_DP_LINK_SPEED_1620Mbps;
            cbDebugPrint((MAKE_LEVEL(DP, DEBUG), "%s: link speed is hardcoded to 1.62Gpbs\n", FUNCTION_NAME));
        }
        else if (pcbe->CbiosFlags & HARDCODE_DP1_MAX_LINKSPEED_2700)
        {
            pDPMonitorContext->SinkMaxLinkSpeed = CBIOS_DP_LINK_SPEED_2700Mbps;
            cbDebugPrint((MAKE_LEVEL(DP, DEBUG), "%s: link speed is hardcoded to 2.7Gbps\n", FUNCTION_NAME));
        }
        else if (pcbe->CbiosFlags & HARDCODE_DP1_MAX_LINKSPEED_5400)
        {
            pDPMonitorContext->SinkMaxLinkSpeed = CBIOS_DP_LINK_SPEED_5400Mbps;
            cbDebugPrint((MAKE_LEVEL(DP, DEBUG), "%s: link speed is hardcoded to 5.4Gbps\n", FUNCTION_NAME));
        }

        DPCD_00002.Value = Data[2];
        if ((DPCD_00002.MAX_LANE_COUNT == 0x01) || (DPCD_00002.MAX_LANE_COUNT == 0x02) || (DPCD_00002.MAX_LANE_COUNT == 0x04))
        {
            pDPMonitorContext->SinkMaxLaneCount= DPCD_00002.MAX_LANE_COUNT;
            cbDebugPrint((MAKE_LEVEL(DP, DEBUG), "%s: lane count DP Sink max supports is %d\n", FUNCTION_NAME, DPCD_00002.MAX_LANE_COUNT));
        }
        else
        {
            pDPMonitorContext->SinkMaxLaneCount = 4;
        }

        // DP 1.2
        if (pDPMonitorContext->DpSinkVersion >= 0x12)
        {
            pDPMonitorContext->bSupportTPS3 = DPCD_00002.TPS3_SUPPORTED;
            cbDebugPrint((MAKE_LEVEL(DP, DEBUG), "%s: bSupportTPS3 = %d\n",
                FUNCTION_NAME, pDPMonitorContext->bSupportTPS3));
        }
        else
        {
            pDPMonitorContext->bSupportTPS3 = CBIOS_FALSE;
        }

        // DP 1.1
        if (pDPMonitorContext->DpSinkVersion >= 0x11)
        {
            pDPMonitorContext->bSupportEnhanceMode = DPCD_00002.ENHANCED_FRAME_CAP;
            cbDebugPrint((MAKE_LEVEL(DP, DEBUG), "%s: bSupportEnhanceMode = %d\n",
                FUNCTION_NAME, pDPMonitorContext->bSupportEnhanceMode));
        }
        else
        {
            pDPMonitorContext->bSupportEnhanceMode = CBIOS_FALSE;
        }

        // Check if anyone want to over write lane count in Registry.
        if (pcbe->CbiosFlags & HARDCODE_DP1_LANECOUNT_1)
        {
            pDPMonitorContext->SinkMaxLaneCount = 1;
            cbDebugPrint((MAKE_LEVEL(DP, DEBUG), "%s: lane count is hardcoded to 1!\n", FUNCTION_NAME));
        }
        else if (pcbe->CbiosFlags & HARDCODE_DP1_LANECOUNT_2)
        {
            pDPMonitorContext->SinkMaxLaneCount = 2;
            cbDebugPrint((MAKE_LEVEL(DP, DEBUG), "%s: lane count is hardcoded to 2!\n", FUNCTION_NAME));
        }
        else if (pcbe->CbiosFlags & HARDCODE_DP1_LANECOUNT_4)
        {
            pDPMonitorContext->SinkMaxLaneCount = 4;
            cbDebugPrint((MAKE_LEVEL(DP, DEBUG), "%s: lane count is hardcoded to 4!\n", FUNCTION_NAME));
        }

        // check eDP content protection
        DPCD_0000D.Value = Data[13];
        //it's BIT0 listed in DP 1.2 draft and eDP 1.2 draft. But bit4 in eDP 1.3. Need confirm
        if (DPCD_0000D.ALTERNATE_SCRAMBLER_RESET_CAPABLE)
        {
            pDPMonitorContext->bSupportASSR = CBIOS_TRUE;
        }
        else
        {
            pDPMonitorContext->bSupportASSR = CBIOS_FALSE;
        }

        if (DPCD_0000D.FRAMING_CHANGE_CAPABLE)
        {
            pDPMonitorContext->bSupportAF = CBIOS_TRUE;
        }
        else
        {
            pDPMonitorContext->bSupportAF = CBIOS_FALSE;
        }

        // DP 1.2
        if (pDPMonitorContext->DpSinkVersion >= 0x12)
        {
            DPCD_0000E.Value = Data[14];
            pDPMonitorContext->TrainingAuxRdInterval = DPCD_0000E.TRAINING_AUX_RD_INTERVAL;
            cbDebugPrint((MAKE_LEVEL(DP, DEBUG), "%s: TrainingAuxRdInterval = %d\n", FUNCTION_NAME, pDPMonitorContext->TrainingAuxRdInterval));
        }

        // CTS: 4.2.2.7
        DPCD_00005.Value = Data[5];
        DPCD_00007.Value = Data[7];

        if (DPCD_00005.DWN_STRM_PORT_PRESENT)
        {
            cbDebugPrint((MAKE_LEVEL(DP, DEBUG), "%s: downstream port count = %d\n", FUNCTION_NAME, DPCD_00007.DWN_STRM_PORT_COUNT));

            if(cbDIU_Aux_Dpcd_Read(pcbe, DPModuleIndex, 0x200, Data,1))
            {
                DPCD_00200.Value = Data[0];
                SinkCount = (DPCD_00200.SINK_COUNT_6 << 6) + DPCD_00200.SINK_COUNT_5_0;
                cbDebugPrint((MAKE_LEVEL(DP, DEBUG), "%s: SinkCount = %d\n", FUNCTION_NAME, SinkCount));
            }
            else
            {
                cbDebugPrint((MAKE_LEVEL(DP, ERROR), "%s: cannot get DPCD(offset = 0x%x, len = %d)\n",
                    FUNCTION_NAME, 0x200, 1));
            }
        }
        bStatus = CBIOS_TRUE;
    }
    else
    {
        cbDebugPrint((MAKE_LEVEL(DP, ERROR), "%s: cannot get sink caps through aux channel!\n", FUNCTION_NAME));
    }

    pDPMonitorContext->bSupportPsr = CBIOS_FALSE;
    pDPMonitorContext->bNoNeedLT = CBIOS_FALSE;
    if(cbDIU_EDP_IsEDPSupported(pcbe, DPModuleIndex))
    {
        if(cbDIU_Aux_Dpcd_Read(pcbe, DPModuleIndex, DP_EDP_SUPPORT, Data, 1))
        {
            DPCD_00070.Value = Data[0];
            if(DPCD_00070.Value & 0x3)
            {
                if(cbDIU_Aux_Dpcd_Read(pcbe, DPModuleIndex, DP_EDP_GENERAL_CAP_1, Data, 1))
                {
                    DPCD_00701.Value = Data[0];
                }
                if(DPCD_00701.SET_POWER_CAPABLE)
                {
                    pDPMonitorContext->bSupportPsr = CBIOS_TRUE;
                }
            }
        }
    }

    //pDPMonitorContext->bSupportPsr = CBIOS_FALSE;

    if(pDPMonitorContext->bSupportPsr)
    {
        DPCD_00071.Value = 0;
        if(cbDIU_Aux_Dpcd_Read(pcbe, DPModuleIndex, DP_PSR_CAPS, Data, 1))
        {
            DPCD_00071.Value = Data[0];
            pDPMonitorContext->bNoNeedLT = DPCD_00071.NO_TRAIN_ON_EXIT;
            if(DPCD_00071.PSR_SETUP_TIME < 7)
            {
                PsrSetupTime = 55 * (6 - DPCD_00071.PSR_SETUP_TIME);
            }
            cbDebugPrint((MAKE_LEVEL(DP, DEBUG), "PsrSetupTime = %d\n", PsrSetupTime));
            cbDebugPrint((MAKE_LEVEL(DP, DEBUG), "pDPMonitorContext->bNoNeedLT = %d\n", pDPMonitorContext->bNoNeedLT));
        }
    }

    pDPMonitorContext->bSupportAuxBl = CBIOS_FALSE;
    if(cbDIU_EDP_IsEDPSupported(pcbe, DPModuleIndex))
    {
        if(cbDIU_Aux_Dpcd_Read(pcbe, DPModuleIndex, DP_EDP_GENERAL_CAP_1, Data, 3))
        {
            DPCD_00701.Value = Data[0];
            DPCD_00702.Value = Data[1];
            DPCD_00703.Value = Data[2];
            if(DPCD_00701.TCON_BACKLIGHT_ADJUSTMENT_CAPABLE &&
               DPCD_00702.BACKLIGHT_BRIGHTNESS_AUX_SET_CAPABLE)
            {
                pDPMonitorContext->bSupportAuxBl = CBIOS_TRUE;
                pDPMonitorContext->EDPBlByteCount = DPCD_00702.BACKLIGHT_BRIGHTNESS_BYTE_COUNT;
                pDPMonitorContext->EDPBlBitAligment = DPCD_00703.BACKLIGHT_BRIGHTNESS_BIT_ALIGMENT;
            }
        }
    }

    //get aux backlight caps
    if(pDPMonitorContext->bSupportAuxBl)
    {
        if(cbDIU_Aux_Dpcd_Read(pcbe, DPModuleIndex, DP_EDP_PWMGEN_BIT_COUNT, Data, 3))
        {
            DPCD_00724.Value = Data[0];
            DPCD_00725.Value = Data[1];
            DPCD_00726.Value = Data[2];

            pDPMonitorContext->EDPBlPwmBitCount = DPCD_00724.Value & DP_EDP_PWMGEN_BIT_COUNT_MASK;
            pDPMonitorContext->EDPBlPwmBitCountMin = DPCD_00725.Value & DP_EDP_PWMGEN_BIT_COUNT_MASK;
            pDPMonitorContext->EDPBlPwmBitCountMax = DPCD_00726.Value & DP_EDP_PWMGEN_BIT_COUNT_MASK;
        }
    }

    if(pDPMonitorContext->DpSinkVersion >= 0x14 && cbDIU_EDP_IsEDPSupported(pcbe, DPModuleIndex))
    {
        if(cbDIU_Aux_Dpcd_Read(pcbe, DPModuleIndex, DP_SUPPORTED_LINK_RATES, Data, 16))
        {
            for(i = 0; i < DP_MAX_SUPPORTED_RATES_NUM; i++)
            {
                LinkRate =  (Data[2 * i + 1] << 8) | Data[2 * i];
                if(LinkRate != 0)
                {
                    //Each valid entry indicates that the receiver supports a nominal per-lane link rate
                    //on the Main-Link equal to (16-bit link rate capability value) * 200kHz.
                    //Valid entries must be unique, and arranged in order of increasing value by increasing DPCD address.
                    pDPMonitorContext->SupportedLinkRates[i] = LinkRate * 200;
                    SupportedLinkRateNum++;
                }
                else
                {
                    //A table entry containing the value of 0000 indicates that the entry, and all entries at
                    //higher DPCD addresses, contain invalid link rates and must not be selected by the Source device.
                    break;
                }
            }
        }
        if(SupportedLinkRateNum)
        {
            MaxLinkRate = pDPMonitorContext->SupportedLinkRates[SupportedLinkRateNum - 1];
            if(DPCD_00001.MAX_LINK_RATE == 0)
            {
                pDPMonitorContext->bUseLinkRateSet = 1;
            }
            else if(MaxLinkRate < pDPMonitorContext->SinkMaxLinkSpeed)
            {
                pDPMonitorContext->bUseLinkRateSet = 1;
            }
            else
            {
                pDPMonitorContext->bUseLinkRateSet = 0;
            }
        }
        else
        {
            pDPMonitorContext->bUseLinkRateSet = 0;
        }
        if(pDPMonitorContext->bUseLinkRateSet)
        {
            pDPMonitorContext->SupportedLinkRateNum = SupportedLinkRateNum;
            pDPMonitorContext->LinkRateHwIndex = cb_min(SupportedLinkRateNum, HW_SUPPORTED_MAX_LINK_SPEED_ENTRY_NUM);
        }
        else
        {
            pDPMonitorContext->SupportedLinkRateNum = 0;
            pDPMonitorContext->LinkRateHwIndex = 0;
        }
        for(i = SupportedLinkRateNum; i < DP_MAX_SUPPORTED_RATES_NUM; i++)
        {
            pDPMonitorContext->SupportedLinkRates[i] = 0;
        }
    }

    pDPMonitorContext->bpc = DP_Default_bpc;
    pDPMonitorContext->EnhancedMode = 0x01;
    pDPMonitorContext->AsyncMode = 0x01;
    pDPMonitorContext->ColorFormat = 0;
    pDPMonitorContext->DynamicRange = 0;
    pDPMonitorContext->YCbCrCoefficients = 0;
    pDPMonitorContext->TUSize = DP_Default_TUSize;
    pDPMonitorContext->LT_Status = 0;

    cbTraceExit(DP);
    return bStatus;
}

static CBIOS_BOOL cbDPMonitor_ReadAltModeCaps(PCBIOS_EXTENSION_COMMON pcbe, PCBIOS_DP_MONITOR_CONTEXT pDPMonitorContext)
{
    CBIOS_MODULE_INDEX DPModuleIndex = cbGetModuleIndex(pcbe, pDPMonitorContext->pDevCommon->DeviceType, CBIOS_MODULE_TYPE_DP);
    CBIOS_BOOL         bStatus = CBIOS_TRUE;
    REG_MM33570        DPEphyMuxHpd;
    CBIOS_U8           LaneCnt = 0;

    pDPMonitorContext->AltReversePlug = CBIOS_FALSE;

    if (pDPMonitorContext->DpUsbMode == CBIOS_DP_ALTMODE)
    {
        DPEphyMuxHpd.Value = cb_ReadU32(pcbe->pAdapterContext, DP_REG_EPHY_MUX_HPD[DPModuleIndex]);
        cbDebugPrint((MAKE_LEVEL(DP, INFO), "%s Alt-Mode attach_info 0x%x\n", FUNCTION_NAME, DPEphyMuxHpd.DP_Attached_Info));

        if (DPEphyMuxHpd.DP_Attached)
        {
            switch (DPEphyMuxHpd.DP_Attached_Info)
            {
            case 0x7:
                pDPMonitorContext->AltReversePlug = CBIOS_TRUE;
                LaneCnt = 2;  break;
            case 0x8:
                LaneCnt = 2;  break;
            case 0x9:
                pDPMonitorContext->AltReversePlug = CBIOS_TRUE;
                LaneCnt = 4;  break;
            case 0xA:
                LaneCnt = 4;  break;
            default:
                LaneCnt = 0;  break;
            }
        }
        pDPMonitorContext->AltMaxLaneCount = LaneCnt;

        if (!LaneCnt)
        {
            cbDebugPrint((MAKE_LEVEL(DP, ERROR), "%s: alt-mode no avaiable lane\n", FUNCTION_NAME));
            bStatus = CBIOS_FALSE;
        }
    }

    return bStatus;
}

static CBIOS_BOOL cbDPMonitor_ReadTunnelCaps(PCBIOS_EXTENSION_COMMON pcbe, PCBIOS_DP_MONITOR_CONTEXT pDPMonitorContext)
{
    CBIOS_MODULE_INDEX DPModuleIndex = cbGetModuleIndex(pcbe, pDPMonitorContext->pDevCommon->DeviceType, CBIOS_MODULE_TYPE_DP);
    CBIOS_BOOL         bStatus = CBIOS_FALSE;
    CBIOS_U8           Data[6] = {0};
    CBIOS_U8           lttprCnt = 0;

    if (pDPMonitorContext->DpUsbMode != CBIOS_DP_TUNNEL)
    {
        return bStatus;
    }

    bStatus = cbDIU_Aux_Dpcd_Read(pcbe, DPModuleIndex, 0xF0000, Data, 5);
    if (bStatus && (Data[0] >= 0x14) && Data[2])
    {
        switch (Data[2])
        {
        case 0x80: lttprCnt = 1;  break;  // valid: 1-8
        case 0x40: lttprCnt = 2;  break;
        case 0x20: lttprCnt = 3;  break;
        case 0x10: lttprCnt = 4;  break;
        case 0x08: lttprCnt = 5;  break;
        case 0x04: lttprCnt = 6;  break;
        case 0x02: lttprCnt = 7;  break;
        case 0x01: lttprCnt = 8;  break;
        default:   lttprCnt = 0xFF; break;  // invalid
        }

        pDPMonitorContext->LttprCount = lttprCnt;
        pDPMonitorContext->LttprRev = Data[0];
        cbDebugPrint((MAKE_LEVEL(DP, INFO), "%s: DP-%u LTTPR count = %u\n", FUNCTION_NAME, DPModuleIndex, lttprCnt));

        switch (Data[1])
        {
        case CBIOS_DPCD_LINK_RATE_8100Mbps:
            pDPMonitorContext->LttprMaxLinkRate = CBIOS_DP_LINK_SPEED_8100Mbps;
            break;
        case CBIOS_DPCD_LINK_RATE_5400Mbps:
            pDPMonitorContext->LttprMaxLinkRate = CBIOS_DP_LINK_SPEED_5400Mbps;
            break;
        case CBIOS_DPCD_LINK_RATE_2700Mbps:
            pDPMonitorContext->LttprMaxLinkRate = CBIOS_DP_LINK_SPEED_2700Mbps;
            break;
        case CBIOS_DPCD_LINK_RATE_1620Mbps:
        default:
            pDPMonitorContext->LttprMaxLinkRate = CBIOS_DP_LINK_SPEED_1620Mbps;
            break;
        }

        // patch: cne001 cannot support Tunneling 5.4G at present
        if (pDPMonitorContext->LttprMaxLinkRate > CBIOS_DP_LINK_SPEED_2700Mbps)
        {
            pDPMonitorContext->LttprMaxLinkRate = CBIOS_DP_LINK_SPEED_2700Mbps;
        }

        switch (0x1F & Data[4])
        {
            case 0x1:
            case 0x2:
            case 0x4:
                pDPMonitorContext->LttprMaxLaneCount = 0x1F & Data[4];
                break;
            default:
                pDPMonitorContext->LttprMaxLaneCount = 0x4;
                break;
        }
    }
    else
    {
        pDPMonitorContext->LttprRev = 0;
        pDPMonitorContext->LttprCount = 0;
        pDPMonitorContext->LttprMaxLaneCount = 0;
        pDPMonitorContext->LttprMaxLinkRate = 0;
        cbDebugPrint((MAKE_LEVEL(DP, INFO), "%s: DP-%u Non-LTTPR !\n", FUNCTION_NAME, DPModuleIndex));
        bStatus = CBIOS_FALSE;
    }

    return bStatus;
}

static CBIOS_VOID cbDPMonitor_GetLttprsCaps(PCBIOS_EXTENSION_COMMON pcbe, PCBIOS_DP_MONITOR_CONTEXT pDPMonitorContext)
{
    CBIOS_MODULE_INDEX DPModuleIndex = cbGetModuleIndex(pcbe, pDPMonitorContext->pDevCommon->DeviceType, CBIOS_MODULE_TYPE_DP);
    CBIOS_BOOL         bStatus = CBIOS_FALSE;
    CBIOS_U8           i = 0;
    CBIOS_U8           Data[4] = {0};

    if ((pDPMonitorContext->DpUsbMode == CBIOS_DP_TUNNEL) && (pDPMonitorContext->LttprCount > 0))
    {
        // per spec, if LttprCount <= 8(DP_MAX_LTTPR_CNT), tunnel DP can work on Non-Transparent or Transparent mode,
        // otherwise it should Only support Transparent mode;
        // but CNE001 only support LttprCount = 1 for Non-Transparent mode
        pDPMonitorContext->LttprNonTransparent = CBIOS_FALSE;

        // if ((pDPMonitorContext->LttprCount > 0) && (pDPMonitorContext->LttprCount <= DP_MAX_LTTPR_CNT))
        if ((pDPMonitorContext->LttprCount > 0) && (pDPMonitorContext->LttprCount <= 1))
        {
            // Non-Transparent mode
            pDPMonitorContext->LttprNonTransparent = CBIOS_TRUE;

            for (i=0; i < pDPMonitorContext->LttprCount; i++)
            {
                if (cbDIU_Aux_Dpcd_Read(pcbe, DPModuleIndex, 0xF0020 + i * 0x50, Data, 2))
                {
                    pDPMonitorContext->LttprAuxRDInterval[i] = Data[0] & 0x7F;
                    pDPMonitorContext->LttprVolPELevel3[i] = Data[1] & 0x03;
                }
                else
                {
                    pDPMonitorContext->LttprNonTransparent = CBIOS_FALSE;
                    cb_memset(pDPMonitorContext->LttprAuxRDInterval, 0, DP_MAX_LTTPR_CNT);
                    cb_memset(pDPMonitorContext->LttprVolPELevel3, 0, DP_MAX_LTTPR_CNT);
                    cbDebugPrint((MAKE_LEVEL(DP, ERROR), "%s: read LTTPR-%x DPCD fail !\n", FUNCTION_NAME, i));
                    break;
                }
            }
        }
        // hardcode Transparent mode
        pDPMonitorContext->LttprNonTransparent = CBIOS_FALSE;
    }
}

CBIOS_U32 cbDPMonitor_GetMaxSupportedDclk(PCBIOS_VOID pvcbe, PCBIOS_DP_MONITOR_CONTEXT pDPMonitorContext)
{
    CBIOS_U32 MaxDclk = 0;
    CBIOS_U32 Bpc = DP_Default_bpc;
    CBIOS_U32 MaxLinkSpeed = 0;
    CBIOS_U32 MaxLaneCount = 0;

    if (pDPMonitorContext == CBIOS_NULL)
    {
        cbDebugPrint((MAKE_LEVEL(DP, ERROR), "%s: the 2nd param is invalid\n", FUNCTION_NAME));
        return 0;
    }

    MaxLinkSpeed = pDPMonitorContext->SinkMaxLinkSpeed;
    MaxLaneCount = pDPMonitorContext->SinkMaxLaneCount;

    if (pDPMonitorContext->DpUsbMode == CBIOS_DP_ALTMODE)
    {
        if (MaxLaneCount > pDPMonitorContext->AltMaxLaneCount)
        {
            MaxLaneCount = pDPMonitorContext->AltMaxLaneCount;
        }
    }
    else if ((pDPMonitorContext->DpUsbMode == CBIOS_DP_TUNNEL) && pDPMonitorContext->LttprCount)
    {
        if (MaxLaneCount > pDPMonitorContext->LttprMaxLaneCount)
        {
            MaxLaneCount = pDPMonitorContext->LttprMaxLaneCount;
        }
        if (MaxLinkSpeed > pDPMonitorContext->LttprMaxLinkRate)
        {
            MaxLinkSpeed = pDPMonitorContext->LttprMaxLinkRate;
        }
    }

    if(pDPMonitorContext->bpc > DP_Default_bpc)
    {
        Bpc = DP_Default_bpc;
    }
    else
    {
        Bpc = pDPMonitorContext->bpc;
    }

    MaxDclk = (MaxLinkSpeed / (Bpc * 3)) * MaxLaneCount * 8;

    return MaxDclk;
}

CBIOS_VOID cbDPMonitor_GetMonitorParamsFromEdid(PCBIOS_VOID pvcbe, PCBIOS_DP_MONITOR_CONTEXT pDPMonitorContext)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    PCBIOS_DEVICE_COMMON    pDevCommon = pDPMonitorContext->pDevCommon;
    CBIOS_MODULE_INDEX      DPModuleIndex = cbGetModuleIndex(pcbe, pDevCommon->DeviceType, CBIOS_MODULE_TYPE_DP);
    CBIOS_BOOL              IsEDP = CBIOS_FALSE;
    PCBIOS_U8               pEdidBuffer = pDevCommon->EdidData.Buffer;

    if(cbDIU_EDP_IsEDPSupported(pcbe, DPModuleIndex))
    {
        IsEDP = CBIOS_TRUE;
    }

    if(IsEDP)
    {
        pDevCommon->CurrentMonitorType = CBIOS_MONITOR_TYPE_PANEL;
    }
    else
    {
        pDevCommon->CurrentMonitorType = CBIOS_MONITOR_TYPE_DP;
    }

    // Check if HDMI
    if (pDevCommon->EdidStruct.Attribute.IsCEA861HDMI)
    {
        cbDebugPrint((MAKE_LEVEL(DP, DEBUG), "%s: HDMI device on dual mode DP port!\n", FUNCTION_NAME));
    }

    pDPMonitorContext->bpc = DP_Default_bpc;
    //: Should check EDID 1.4 here since majority of DP monitors don't have extended EDID block
    if((pEdidBuffer[0x12] == 0x01) && (pEdidBuffer[0x13] == 0x04))
    {
        if (pEdidBuffer[0x14] & 0x80)
        {
            if((pEdidBuffer[0x14] & 0x70) == 0x10)
            {
                pDPMonitorContext->bpc = 6;
            }
            else if((pEdidBuffer[0x14] & 0x70) == 0x20)
            {
                pDPMonitorContext->bpc = 8;
            }
            else if((pEdidBuffer[0x14] & 0x70) == 0x30)
            {
                pDPMonitorContext->bpc = 10;
            }
            else if((pEdidBuffer[0x14] & 0x70) == 0x40)
            { 
                pDPMonitorContext->bpc = 12;
            }
            else if((pEdidBuffer[0x14] & 0x70) == 0x50)
            {
                pDPMonitorContext->bpc = 14;
            }
            else if((pEdidBuffer[0x14] & 0x70) == 0x60)
            {
                pDPMonitorContext->bpc = 16;
            }
        }
    }

    cbDebugPrint((MAKE_LEVEL(DP, DEBUG), "%s: DP monitor supported bpc = %d\n", FUNCTION_NAME, pDPMonitorContext->bpc));
}

CBIOS_VOID cbDPMonitor_InitUsbPara(PCBIOS_VOID pvcbe, PCBIOS_DP_MONITOR_CONTEXT pDPMonitorContext)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    PCBIOS_DEVICE_COMMON pDevCommon = pDPMonitorContext->pDevCommon;
    CBIOS_MODULE_INDEX DPModuleIndex = cbGetModuleIndex(pcbe, pDevCommon->DeviceType, CBIOS_MODULE_TYPE_DP);
    REG_MM33558    LttprConfig1, LttprConfig1Mask;
    REG_MM33570    DPEphyMuxHpd, DPEphyMuxHpdMask;
    REG_MM8214  DPLinkRegValue, DPLinkRegMask;
    REG_MM334C8 DPLinkCtrlRegValue, DPLinkCtrlRegMask;

    if (pcbe->ChipID != CHIPID_CNE001 || DPModuleIndex >= DP_USB_MODU_NUM)
    {
        pDPMonitorContext->DpUsbMode = CBIOS_DP_STANDARD;
        return;
    }

    // Set DP tunnel/alt-mode flag
    cbDebugPrint((MAKE_LEVEL(DP, INFO), "%s: DP_USB_Mode=0x%x\n", FUNCTION_NAME, pDPMonitorContext->DpUsbMode));

    // Enable DP tunnel/alt-mode
    DPEphyMuxHpd.Value = 0;
    DPEphyMuxHpd.DP_Alt_Mode_EN = (pDPMonitorContext->DpUsbMode == CBIOS_DP_ALTMODE) ? 1 : 0;
    DPEphyMuxHpdMask.Value = 0xFFFFFFFF;
    DPEphyMuxHpdMask.DP_Alt_Mode_EN = 0;
    cbMMIOWriteReg32(pcbe, DP_REG_EPHY_MUX_HPD[DPModuleIndex], DPEphyMuxHpd.Value, DPEphyMuxHpdMask.Value);

    LttprConfig1.Value = 0;
    LttprConfig1.Tunnel_DP_EN = (pDPMonitorContext->DpUsbMode == CBIOS_DP_TUNNEL) ? 1 : 0;
    LttprConfig1Mask.Value = 0xFFFFFFFF;
    LttprConfig1Mask.Tunnel_DP_EN = 0;
    cbMMIOWriteReg32(pcbe, DP_REG_LTTPR_CFG1[DPModuleIndex], LttprConfig1.Value, LttprConfig1Mask.Value);

    pDPMonitorContext->AltReversePlug = CBIOS_FALSE;
    pDPMonitorContext->AltMaxLaneCount = 0;
    pDPMonitorContext->LttprRev = 0;
    pDPMonitorContext->LttprCount = 0;
    pDPMonitorContext->LttprMaxLaneCount = 0;
    pDPMonitorContext->LttprMaxLinkRate = 0;
    pDPMonitorContext->LttprNonTransparent = CBIOS_FALSE;
    cb_memset(pDPMonitorContext->LttprAuxRDInterval, 0, DP_MAX_LTTPR_CNT);
    cb_memset(pDPMonitorContext->LttprVolPELevel3, 0, DP_MAX_LTTPR_CNT);

    // Read tunnel/alt-mode caps
    if (pDPMonitorContext->DpUsbMode == CBIOS_DP_ALTMODE)
    {
        cbDPMonitor_ReadAltModeCaps(pcbe, pDPMonitorContext);
        // If not set the registers, after starting up, first plug_in type-C the link-training could fail.
        if (pDevCommon->PowerState != CBIOS_PM_ON || !pDevCommon->bConnected)
        {
            DPLinkRegValue.Value = 0;
            DPLinkRegValue.Start_Link_Rate_0 = 0;
            DPLinkRegMask.Value = 0xFFFFFFFF;
            DPLinkRegMask.Start_Link_Rate_0 = 0;
            DPLinkCtrlRegValue.Value = 0;
            DPLinkCtrlRegValue.Start_LINK_RATE_1 = 1;
            DPLinkCtrlRegMask.Value = 0xFFFFFFFF;
            DPLinkCtrlRegMask.Start_LINK_RATE_1 = 0;
            cbMMIOWriteReg32(pcbe, DP_REG_LINK[DPModuleIndex], DPLinkRegValue.Value, DPLinkRegMask.Value);
            cbMMIOWriteReg32(pcbe, DP_REG_LINK_CTRL[DPModuleIndex], DPLinkCtrlRegValue.Value, DPLinkCtrlRegMask.Value);
        }
    }
    else if (pDPMonitorContext->DpUsbMode == CBIOS_DP_TUNNEL)
    {
        cbDPMonitor_ReadTunnelCaps(pcbe, pDPMonitorContext);
    }

    cbPHY_DP_InitUDEphy(pcbe, DPModuleIndex, pDPMonitorContext->DpUsbMode, (CBIOS_U8)pDPMonitorContext->AltMaxLaneCount);
}

CBIOS_BOOL cbDPMonitor_Detect(PCBIOS_VOID pvcbe, PCBIOS_DP_MONITOR_CONTEXT pDPMonitorContext, PCBIOS_DETECT_FLAG pDetectFlag)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    CBIOS_BOOL              bConnected = CBIOS_FALSE;
    CBIOS_BOOL              IsDevChanged = CBIOS_FALSE;
    REG_MM8340              DPEphyMpllRegValue;
    REG_MM8348              DPEphyMiscRegValue;
    CBIOS_U8                AuxReadEDIDTime;
    CBIOS_BOOL              bGetEDID = CBIOS_FALSE;
    PCBIOS_DEVICE_COMMON    pDevCommon = pDPMonitorContext->pDevCommon;
    CBIOS_MODULE_INDEX      DPModuleIndex = cbGetModuleIndex(pcbe, pDevCommon->DeviceType, CBIOS_MODULE_TYPE_DP);
    PCBIOS_EDP_PANEL_DESC   pPanelDesc = &(pDevCommon->DeviceParas.EDPPanelDevice.EDPPanelDesc);
    CBIOS_U8            Data[4] = {0};
    DPCD_REG_00202      DPCD_00202;
    DPCD_REG_00203      DPCD_00203;
    CBIOS_BOOL          bNeedLinkTraining = CBIOS_FALSE;

    if (DPModuleIndex >= DP_MODU_NUM)
    {
        cbDebugPrint((MAKE_LEVEL(DP, ERROR), "%s: invalid DP module index!\n", FUNCTION_NAME));
        bConnected = CBIOS_FALSE;
        goto EXIT;
    }

    if(cbDIU_EDP_IsEDPSupported(pcbe, DPModuleIndex))
    {
        cbEDPPanel_PreInit(pcbe);
    }

    cbDIU_DP_ResetAUX(pcbe, DPModuleIndex);

    cbDPMonitor_InitUsbPara(pcbe, pDPMonitorContext);

    if(cbDIU_EDP_IsEDPSupported(pcbe, DPModuleIndex))
    {
        if (pDevCommon->PowerState != CBIOS_PM_ON) //if eDP already power on, skip aux power sequence
        {
            if(!pDPMonitorContext->VddStatus)
            {
                cbDIU_EDP_EDPAuxPowerSeqCtrl(pcbe, pDPMonitorContext, DPModuleIndex, CBIOS_TRUE);
            }

            //if eDP already power on and the panel is in PSR_State 3:PSR active-no RFB update,
            //write dpcd 600h to 1, the sink may consider that it will go to psr exit sequence
            //and may go to incorrect state.
            cbDPMonitor_SetSinkPowerState(pcbe, DPModuleIndex, CBIOS_TRUE);
        }
    }
    else
    {
        //DP LinkCompTest 4.4.3, "Sink shall not respond to any AUX requests
        //prior to initial power save mode exit request."
        //So, we must set sink device to D0 state, otherwise, any AUX requests
        //may fail.
        cbDPMonitor_SetSinkPowerState(pcbe, DPModuleIndex, CBIOS_TRUE);
    }

    if (cbDPMonitor_GetDPCDVersion(pcbe, pDPMonitorContext))
    {
        bConnected = CBIOS_TRUE;

        //Following is patch for the cbDPMonitor_ReadEDID fail due to the conflict usage of Aux Channel between Reading EDID and sending CP_IRQ signal ,
        //which causes the EDID process being interrupted and fail.
        for (AuxReadEDIDTime = 0; AuxReadEDIDTime < 3; AuxReadEDIDTime++)
        {
            bGetEDID = cbGetDeviceEDID(pcbe, pDevCommon, &IsDevChanged, pDetectFlag->FullDetect);
            if (bGetEDID)
            {
                break;
            }
        }
        if(!bGetEDID)
        {
            cbDebugPrint((MAKE_LEVEL(DP, WARNING), "%s: Can't get EDID of DP single mode device!\n", FUNCTION_NAME));
            cbDebugPrint((MAKE_LEVEL(DP, WARNING), "%s: Use fail-safe EDID!\n", FUNCTION_NAME));

            if (sizeof(DPFailSafeModeEdid) != pDevCommon->EdidData.BufferSize || cb_memcmp(DPFailSafeModeEdid, pDevCommon->EdidData.Buffer, pDevCommon->EdidData.BufferSize))
            {
                if (cbCopyBuffer2EdidData(&pDevCommon->EdidData, DPFailSafeModeEdid, sizeof(DPFailSafeModeEdid)))
                {
                    IsDevChanged = CBIOS_TRUE;
                }
                else
                {
                    bConnected = CBIOS_FALSE;
                }
            }
        }
    }
    else
    {
        bConnected = CBIOS_FALSE;
    }

    if(bConnected)
    {
        if (IsDevChanged)
        {
            cbDPMonitor_GetSinkCaps(pcbe, pDPMonitorContext);
            cbDPMonitor_GetLttprsCaps(pcbe, pDPMonitorContext);

            cbEDIDModule_ParseEDID(&pDevCommon->EdidData, &(pDevCommon->EdidStruct));
            
            //patch to disable 24bit L_PCM audio format
            if(pcbe->ChipID >= CHIPID_CHX004)
            {
                cbEDIDModule_SADPatch(&(pDevCommon->EdidStruct));
            }
            cbDPMonitor_GetMonitorParamsFromEdid(pcbe, pDPMonitorContext);

            cbMode_MakeDeviceModeList(pcbe, pDevCommon->DeviceType);
        }

        /*cbDebugPrint((MAKE_LEVEL(DP, INFO), "DP/eDP monitor is detected on port 0x%x, FullDetect:%d!\n", 
                                pDevCommon->DeviceType, pDetectFlag->FullDetect));*/
    }
    else
    {
        cbClearEdidRelatedData(pcbe, pDevCommon);
        cb_memset(((PCBIOS_HDCP_CONTEXT)pDevCommon->pHDCPContext)->BKsv, 0, sizeof(((PCBIOS_HDCP_CONTEXT)pDevCommon->pHDCPContext)->BKsv));
    }

    if(cbDIU_EDP_IsEDPSupported(pcbe, DPModuleIndex))
    {
        if(bConnected && cbEDPPanel_GetPanelDescriptor(pcbe, pDevCommon->EdidData.Buffer) != CBIOS_NULL)
        {
            cb_memcpy(pPanelDesc, cbEDPPanel_GetPanelDescriptor(pcbe, pDevCommon->EdidData.Buffer), sizeof(CBIOS_EDP_PANEL_DESC));
            cbEDPPanel_Init(pcbe, pPanelDesc);
        }
        if (pDevCommon->PowerState != CBIOS_PM_ON)
        {
            if(!bConnected)
            {
                cbDIU_EDP_EDPAuxPowerSeqCtrl(pcbe, pDPMonitorContext, DPModuleIndex, CBIOS_FALSE);
            }
        }
    }

    if(!bConnected)
    {
        cbPHY_DP_AuxPowerOnOff(pcbe, DPModuleIndex, CBIOS_FALSE);
    }

EXIT:
    pDetectFlag->Connected = (bConnected)? 1 : 0;
    pDetectFlag->Changed = (IsDevChanged)? 1 : 0;
    
    return bConnected;
}

CBIOS_VOID cbDPMonitor_UpdateModeInfo(PCBIOS_VOID pvcbe, PCBIOS_DP_MONITOR_CONTEXT pDPMonitorContext, PCBIOS_DISP_MODE_PARAMS pModeParams)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    PCBIOS_DEVICE_COMMON    pDevCommon      = pDPMonitorContext->pDevCommon;
    PCBIOS_AMD_VSDB_EXTENTION   pAMDVSDBData = &(pDevCommon->EdidStruct.Attribute.AMDVSDBData);
    PCBIOS_MRL pMRLData = &(pDevCommon->EdidStruct.Attribute.MRLData);
    CBIOS_U32 IGAIndex = pModeParams->IGAIndex;

    if (0 == pcbe->SpecifyDestTimingSrc[IGAIndex].Flag)
    {
        if (CBIOS_RGBOUTPUT == pModeParams->TargetModePara.OutputSignal)
        {
            pDPMonitorContext->ColorFormat = 0;
        }
        else if(CBIOS_YCBCR422OUTPUT == pModeParams->TargetModePara.OutputSignal)
        {
            pDPMonitorContext->ColorFormat = 1;
        }
        else if(CBIOS_YCBCR444OUTPUT == pModeParams->TargetModePara.OutputSignal)
        {
            pDPMonitorContext->ColorFormat = 2;
        }
        // YUV420 -- may be need to change here in future
        else if(CBIOS_YCBCR420OUTPUT == pModeParams->TargetModePara.OutputSignal)
        {
            cbDebugPrint((MAKE_LEVEL(DP, ERROR), "%s: DP doesn't support YUV420 output up to DP1.2\n", FUNCTION_NAME));
            pDPMonitorContext->ColorFormat = 0;
        }
    }

    // save timing and interlace flag to DP monitor context for link training used
    cb_memcpy(&(pDPMonitorContext->TargetTiming), &(pModeParams->TargetTiming), sizeof(CBIOS_TIMING_ATTRIB));
    pDPMonitorContext->bInterlace = pModeParams->TargetModePara.bInterlace;
    pDPMonitorContext->PixelRepetition = pModeParams->PixelRepitition;

    pDPMonitorContext->bSupportFreeSync = (pAMDVSDBData->IsFreeSyncSupported  | pMRLData->bSupportFreeSync) ? CBIOS_TRUE : CBIOS_FALSE;
}

CBIOS_VOID cbDPMonitor_SetMode(PCBIOS_VOID pvcbe, PCBIOS_DP_MONITOR_CONTEXT pDPMonitorContext, PCBIOS_DISP_MODE_PARAMS pModeParams)
{
    PCBIOS_EXTENSION_COMMON pcbe          = (PCBIOS_EXTENSION_COMMON)pvcbe;
    PCBIOS_DEVICE_COMMON    pDevCommon    = pDPMonitorContext->pDevCommon;
    CBIOS_MODULE_INDEX      DPModuleIndex = cbGetModuleIndex(pcbe, pDevCommon->DeviceType, CBIOS_MODULE_TYPE_DP);
    CBIOS_U8                HVPolarity    = (CBIOS_U8)pModeParams->TargetTiming.HVPolarity;

    if (DPModuleIndex == CBIOS_MODULE_INDEX_INVALID)
    {
        cbDebugPrint((MAKE_LEVEL(DP, ERROR), "%s: invalid DP module index!\n", FUNCTION_NAME));
        return;
    }

    cbDIU_DP_SetHVSync(pcbe, DPModuleIndex, HVPolarity);
}

CBIOS_VOID cbDPMonitor_QueryAttribute(PCBIOS_VOID pvcbe, PCBIOS_DP_MONITOR_CONTEXT pDPMonitorContext, PCBiosMonitorAttribute pMonitorAttribute)
{
    PCBIOS_EXTENSION_COMMON pcbe          = (PCBIOS_EXTENSION_COMMON)pvcbe;
    PCBIOS_EDP_PANEL_DESC pEDPPanelDesc = &(pDPMonitorContext->pDevCommon->DeviceParas.EDPPanelDevice.EDPPanelDesc);
    CBIOS_MODULE_INDEX      DPModuleIndex = cbGetModuleIndex(pcbe, pDPMonitorContext->pDevCommon->DeviceType, CBIOS_MODULE_TYPE_DP);

    if (pDPMonitorContext->bpc == 6)
    {
        pMonitorAttribute->SupportBPC.IsSupport6BPC = CBIOS_TRUE;
    }
    else if (pDPMonitorContext->bpc == 8)
    {
        pMonitorAttribute->SupportBPC.IsSupport8BPC = CBIOS_TRUE;
    }
    else if (pDPMonitorContext->bpc == 10)
    {
        pMonitorAttribute->SupportBPC.IsSupport10BPC = CBIOS_TRUE;
    }
    else if (pDPMonitorContext->bpc == 12)
    {
        pMonitorAttribute->SupportBPC.IsSupport12BPC = CBIOS_TRUE;
    }
    else if (pDPMonitorContext->bpc == 14)
    {
        pMonitorAttribute->SupportBPC.IsSupport14BPC = CBIOS_TRUE;
    }
    else if (pDPMonitorContext->bpc == 16)
    {
        pMonitorAttribute->SupportBPC.IsSupport16BPC = CBIOS_TRUE;
    }

    if(cbDIU_EDP_IsEDPSupported(pcbe, DPModuleIndex))
    {
        pMonitorAttribute->bSupportPsr = pDPMonitorContext->bSupportPsr;
        pMonitorAttribute->bSupporeAuxBl = pDPMonitorContext->bSupportAuxBl;
    }

    if(cbDIU_EDP_IsEDPSupported(pcbe, DPModuleIndex) && pEDPPanelDesc)
    {
        pMonitorAttribute->bSupportBLCtrl = pEDPPanelDesc->EDPCaps.isBLCtrlSupport;
        pMonitorAttribute->MaxBLLevel = pEDPPanelDesc->EDPCaps.BacklightMax;
        pMonitorAttribute->MinBLLevel = pEDPPanelDesc->EDPCaps.BacklightMin;
    }
}

CBIOS_VOID cbDPMonitor_UsbTunnelReset(PCBIOS_VOID pvcbe, PCBIOS_DP_MONITOR_CONTEXT pDPMonitorContext, CBIOS_MODULE_INDEX DPModuleIndex)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    CBIOS_U8  hpdStatus;
    CBIOS_U8  Data[2] = {0};

    hpdStatus = cbDIU_DP_GetHpdStatus(pcbe, DPModuleIndex, pDPMonitorContext->CurrentDpUsbMode);
    if (hpdStatus == CBIOS_HPD_STATUS_IN || hpdStatus == CBIOS_HPD_STATUS_IRQ)
    {
        if (!cbDIU_Aux_Inner_Write(pcbe, DPModuleIndex, 0xFABCD, Data, 1))
        {
            cbDebugPrint((MAKE_LEVEL(DP, ERROR), "%s: Aux write 0xFABCD fail!\n", FUNCTION_NAME));
        }
    }
}

CBIOS_VOID cbDPMonitor_OnOff(PCBIOS_VOID pvcbe, PCBIOS_DP_MONITOR_CONTEXT pDPMonitorContext, CBIOS_BOOL bOn, CBIOS_U32 Flags)
{
    PCBIOS_EXTENSION_COMMON pcbe          = (PCBIOS_EXTENSION_COMMON)pvcbe;
    PCBIOS_DEVICE_COMMON    pDevCommon    = pDPMonitorContext->pDevCommon;
    CBIOS_MODULE_INDEX      DPModuleIndex = cbGetModuleIndex(pcbe, pDevCommon->DeviceType, CBIOS_MODULE_TYPE_DP);
    PCBIOS_EDP_PANEL_DESC   pEDPPanelDesc = &(pDevCommon->DeviceParas.EDPPanelDevice.EDPPanelDesc);
    CBIOS_BOOL              bEDPMode      = cbDIU_EDP_IsEDPSupported(pcbe, DPModuleIndex);
    CBIOS_U32               BacklightVal  = 0;

    if (bOn)
    {
        cbDPMonitor_SetDither(pcbe, pDPMonitorContext->bpc, CBIOS_TRUE, DPModuleIndex);
        // 1. start EDP panel power supply
        if (bEDPMode && !pDPMonitorContext->VddStatus)
        {
            cbDIU_EDP_EDPAuxPowerSeqCtrl(pcbe, pDPMonitorContext, DPModuleIndex, bOn);
        }

        cbPHY_DP_AuxPowerOnOff(pcbe, DPModuleIndex, CBIOS_TRUE);

        // 2. turn on DP EPHY
        if (pDPMonitorContext->CurrentDpUsbMode != CBIOS_DP_ALTMODE)
        {
            cbPHY_DP_DPModeOnOff(pcbe, DPModuleIndex, bOn);
        }

        // 3. Set Sink device to D0(normal operation mode) state
        cbDPMonitor_SetSinkPowerState(pcbe, DPModuleIndex, CBIOS_TRUE);
        cbDelayMilliSeconds(2);

        // 4. turn off DP video before Link Training
        cbDIU_DP_VideoAudioOnOff(pcbe, DPModuleIndex, CBIOS_FALSE);

        // 5. do Link Training
        cbDebugPrint((MAKE_LEVEL(DP, DEBUG), "%s: ON. Need to do Link Training !\n", FUNCTION_NAME));
        if (cbDPMonitor_LinkTraining(pcbe, pDPMonitorContext, CBIOS_FALSE))
        {
            if (cbDPMonitor_SetUpMainLink(pcbe, pDPMonitorContext))
            {
                // set infoframe
                cbDPMonitor_SetInfoFrame(pcbe, pDPMonitorContext);
            }
            else
            {
                cbDebugPrint((MAKE_LEVEL(DP, WARNING),"%s: setting up Main Link failed!\n", FUNCTION_NAME));
            }
        }
        else
        {
            // Even if link training failed, still need to set correct DP path since sink would try to request link training again via hot IRQ.
            cbDebugPrint((MAKE_LEVEL(DP, WARNING),"%s: skip setting up Main Link because Link Training failed!\n", FUNCTION_NAME));
        }

        cbDelayMilliSeconds(2);

        // 6. turn on DP video after Link Training
        cbDIU_DP_VideoAudioOnOff(pcbe, DPModuleIndex, CBIOS_TRUE);

        // 7. turn on EDP panel back light
        if (bEDPMode)
        {
            cbDelayMilliSeconds(80);
            cbDIU_EDP_ControlVEESignal(pcbe, pDPMonitorContext, DPModuleIndex, bOn);
            if (pcbe->SysBiosInfo.bBLGfxMode)
            {
                BacklightVal = pDPMonitorContext->bBlEverSetByOS ? pDPMonitorContext->BrightnessSetByOS : 255;
                cbEDPPanel_OnOff(pcbe, DPModuleIndex, bOn, BacklightVal , pEDPPanelDesc);
            }
        }

        //if monitor support freesync, ignore the MSA Timing Parameters
        if (pDPMonitorContext->bSupportFreeSync)
        {
            cbDPMonitor_SetIgnoreMSATimingPara(pcbe, DPModuleIndex, CBIOS_TRUE);
        }
    }
    else
    {
        if (pDPMonitorContext->bSupportFreeSync)
        {
            cbDPMonitor_SetIgnoreMSATimingPara(pcbe, DPModuleIndex, CBIOS_FALSE);
        }

        // 1. turn off EDP panel back light
        if (bEDPMode)
        {
            if (pcbe->SysBiosInfo.bBLGfxMode)
            {
                cbEDPPanel_OnOff(pcbe, DPModuleIndex, bOn, 0, pEDPPanelDesc);
            }
            cbDIU_EDP_ControlVEESignal(pcbe, pDPMonitorContext, DPModuleIndex, bOn);
            cbDelayMilliSeconds(50);
        }

        // Inform USB to reset status
        if (pDPMonitorContext->CurrentDpUsbMode == CBIOS_DP_TUNNEL)
        {
            cbDPMonitor_UsbTunnelReset(pcbe, pDPMonitorContext, DPModuleIndex);
        }

        //disable both Video and Audio
        cbDIU_DP_VideoAudioOnOff(pcbe, DPModuleIndex, CBIOS_FALSE);

        // 2. reset Link Training
        cbDIU_DP_ResetLinkTraining(pcbe, DPModuleIndex);
        pDPMonitorContext->LT_Status = 0;

        if (bEDPMode)
        {
            //disable aux backlight if it is enabled
            if (pcbe->SysBiosInfo.bBLGfxMode && pDPMonitorContext->bAuxBlEnabled)
            {
                cbDIU_EDP_AUXDisableBacklight(pcbe, DPModuleIndex);
                pDPMonitorContext->bAuxBlEnabled = CBIOS_FALSE;
            }
        }

        // 3. Set Sink device to D3 (power saving) state
        //for driver load period, it needs turn off connected device,if not set aux power on, will can't read/write dpcd rightly
        cbDPMonitor_SetSinkPowerState(pcbe, DPModuleIndex, CBIOS_FALSE);

        // 4. turn off DP EPHY
        if (pDPMonitorContext->CurrentDpUsbMode == CBIOS_DP_ALTMODE)
        {
            cbPHY_DP_UDPHY_OnOff(pcbe, DPModuleIndex, bOn);
        }
        else
        {
            cbPHY_DP_DPModeOnOff(pcbe, DPModuleIndex, bOn);
        }

        // 5. reset AUX
        cbDIU_DP_ResetAUX(pcbe, DPModuleIndex);

        cbPHY_DP_AuxPowerOnOff(pcbe, DPModuleIndex, CBIOS_FALSE);

        // 6. stop EDP panel power supply
        if (bEDPMode)
        {
            if(!(Flags & SKIP_VDD_OFF))
            {
                cbDIU_EDP_EDPAuxPowerSeqCtrl(pcbe, pDPMonitorContext, DPModuleIndex, bOn);
            }
        }

        cbDPMonitor_SetDither(pcbe, pDPMonitorContext->bpc, CBIOS_FALSE, DPModuleIndex);
    }

}

static CBIOS_BOOL cbDPMonitor_ProcTestEdidRead(PCBIOS_EXTENSION_COMMON pcbe, PCBIOS_DP_MONITOR_CONTEXT pDPMonitorContext)
{
    CBIOS_MODULE_INDEX DPModuleIndex = cbGetModuleIndex(pcbe, pDPMonitorContext->pDevCommon->DeviceType, CBIOS_MODULE_TYPE_DP);
    PCBIOS_DEVICE_COMMON  pDevCommon = pDPMonitorContext->pDevCommon;
    CBIOS_U8       Data[4] = {0};
    DPCD_REG_00260 DPCD00260;
    CBIOS_BOOL     bRet = CBIOS_TRUE;
    CBIOS_U8       *pEdid = CBIOS_NULL;
    CBIOS_U32      EdidSize = 0;
    CBIOS_U8       byExtNum = 0;

    // Write Ack to test response fields of DPCD
    DPCD00260.Value = 0;
    DPCD00260.TEST_ACK = 1;
    Data[0] = DPCD00260.Value;
    if(!cbDIU_Aux_Dpcd_Write(pcbe, DPModuleIndex, 0x260, Data, 1))
    {
        cbDebugPrint((MAKE_LEVEL(DP, DEBUG), "%s: Write ack to TEST_RESPONSE fields of DPCD!\n", FUNCTION_NAME));
        bRet = CBIOS_FALSE;
    }

    // 4.2.2.3
    // 4.2.2.4
    // 4.2.2.5
    pEdid = cbGetFullEdid(pcbe, pDPMonitorContext->pDevCommon, &EdidSize);
    if (!pEdid)
    {
        bRet = CBIOS_FALSE;
        goto exit;
    }

    // For CTS EDID read test item: 4.2.2.3
    // Should always check the checksum of last edid block according to linklayer compliance test spec.
    cbDPMonitor_TestEDIDCheckSum(pcbe, pDPMonitorContext, pEdid[EdidSize - 1]);

exit:
    if(!bRet)
    {
        // CTS item: 4.2.2.4
        cbDebugPrint((MAKE_LEVEL(DP, WARNING), "%s: Automated Test: EDID Corruption Test, use fake safe-mode EDID.\n", FUNCTION_NAME));
        if (cbCopyBuffer2EdidData(&pDevCommon->FakeEdid, DPFailSafeModeEdid, sizeof(DPFailSafeModeEdid)))
        {
            bRet = CBIOS_TRUE;
        }
    }
    else
    {
        cbClearEdidData(&pDevCommon->FakeEdid);
        pDevCommon->FakeEdid.Buffer = pEdid;
        pDevCommon->FakeEdid.BufferSize = EdidSize;
    }

    pDPMonitorContext->pDevCommon->isFakeEdid = bRet;

    return bRet;
}

static CBIOS_BOOL cbDPMonitor_ProcTestLinkTraining(PCBIOS_EXTENSION_COMMON pcbe, PCBIOS_DP_MONITOR_CONTEXT pDPMonitorContext)
{
    CBIOS_MODULE_INDEX DPModuleIndex = cbGetModuleIndex(pcbe, pDPMonitorContext->pDevCommon->DeviceType, CBIOS_MODULE_TYPE_DP);
    CBIOS_U8       Data[1] = {0};
    DPCD_REG_00260 DPCD00260;
    CBIOS_BOOL     bStatus = CBIOS_TRUE;

    // Write Ack to test response fields of DPCD
    DPCD00260.Value = 0;
    DPCD00260.TEST_ACK = 1;
    Data[0] = DPCD00260.Value;
    if(!cbDIU_Aux_Dpcd_Write(pcbe, DPModuleIndex, 0x260, Data, 1))
    {
        cbDebugPrint((MAKE_LEVEL(DP, DEBUG), "%s: Write ack to TEST_RESPONSE fields of DPCD!\n", FUNCTION_NAME));
        bStatus = CBIOS_FALSE;
    }
    //4.3.1.2
    //4.3.1.3
    //4.3.1.4
    //4.3.1.5
    //4.3.1.6
    //4.3.1.7
    //4.3.1.8
    //4.3.1.9
    //4.3.1.10
    //4.3.1.11
    //4.3.1.12
                            
    cbDPMonitor_LinkTraining(pcbe, pDPMonitorContext, CBIOS_TRUE);
    
    pDPMonitorContext->LT_Status = 1;

    return bStatus;
}

static CBIOS_BOOL cbDPMonitor_GetAutoTestDpcdTiming(PCBIOS_EXTENSION_COMMON pcbe, PCBIOS_DP_MONITOR_CONTEXT pDPMonitorContext)
{
    CBIOS_MODULE_INDEX DPModuleIndex = cbGetModuleIndex(pcbe, pDPMonitorContext->pDevCommon->DeviceType, CBIOS_MODULE_TYPE_DP);
    CBIOS_U8       Data[16] = {0};
    DPCD_REG_00219 DPCD00219;
    DPCD_REG_00220 DPCD00220;
    DPCD_REG_00221 DPCD00221;
    DPCD_REG_00222 DPCD00222;
    DPCD_REG_00223 DPCD00223;
    DPCD_REG_00224 DPCD00224;
    DPCD_REG_00225 DPCD00225;
    DPCD_REG_00226 DPCD00226;
    DPCD_REG_00227 DPCD00227;
    DPCD_REG_00228 DPCD00228;
    DPCD_REG_00229 DPCD00229;
    DPCD_REG_0022A DPCD0022A;
    DPCD_REG_0022B DPCD0022B;
    DPCD_REG_0022C DPCD0022C;
    DPCD_REG_0022D DPCD0022D;
    DPCD_REG_0022E DPCD0022E;
    DPCD_REG_0022F DPCD0022F;
    DPCD_REG_00230 DPCD00230;
    DPCD_REG_00231 DPCD00231;
    DPCD_REG_00232 DPCD00232;
    DPCD_REG_00233 DPCD00233;
    DPCD_REG_00234 DPCD00234;
    CBIOS_U32 RefreshRate = 0;
    CBIOS_BOOL     bStatus = CBIOS_TRUE;

    cb_memset(&pDPMonitorContext->TestDpcdDataTiming, 0, sizeof(CBiosCustmizedDestTiming));

    if(cbDIU_Aux_Dpcd_Read(pcbe, DPModuleIndex, 0x219, Data, 1))
    {
        DPCD00219.Value = Data[0];
        pDPMonitorContext->TestDpcdDataTiming.LinkRate = DPCD00219.TEST_LINK_RATE;
    }
    else
    {
        cbDebugPrint((MAKE_LEVEL(DP, ERROR), "%s(%d): read DPCD reg(Offset = 0x%x, len = %d) fail?\n", 
            FUNCTION_NAME, LINE_NUM, 0x219, 1));
        bStatus = CBIOS_FALSE;
    }

    if(cbDIU_Aux_Dpcd_Read(pcbe, DPModuleIndex, 0x220, Data, 16))
    {
        DPCD00220.Value = Data[0];
        DPCD00221.Value = Data[1];
        DPCD00222.Value = Data[2];
        DPCD00223.Value = Data[3];
        DPCD00224.Value = Data[4];
        DPCD00225.Value = Data[5];
        DPCD00226.Value = Data[6];
        DPCD00227.Value = Data[7];
        DPCD00228.Value = Data[8];
        DPCD00229.Value = Data[9];
        DPCD0022A.Value = Data[10];
        DPCD0022B.Value = Data[11];
        DPCD0022C.Value = Data[12];
        DPCD0022D.Value = Data[13];
        DPCD0022E.Value = Data[14];
        DPCD0022F.Value = Data[15];

        pDPMonitorContext->TestDpcdDataTiming.LaneCount = DPCD00220.TEST_LANE_COUNT;
        pDPMonitorContext->TestDpcdDataTiming.TestPattern = DPCD00221.TEST_PATTERN;
        pDPMonitorContext->TestDpcdDataTiming.HorTotal = (DPCD00222.TEST_H_TOTAL_15_8 << 8) + DPCD00223.TEST_H_TOTAL_7_0;
        pDPMonitorContext->TestDpcdDataTiming.VerTotal = (DPCD00224.TEST_V_TOTAL_15_8 << 8) + DPCD00225.TEST_V_TOTAL_7_0;
        pDPMonitorContext->TestDpcdDataTiming.HorSyncStart = (DPCD00226.TEST_H_START_15_8 << 8) + DPCD00227.TEST_H_START_7_0;
        pDPMonitorContext->TestDpcdDataTiming.VerSyncStart = (DPCD00228.TEST_V_START_15_8 << 8) + DPCD00229.TEST_V_START_7_0;
        pDPMonitorContext->TestDpcdDataTiming.HSyncPolarity = DPCD0022A.TEST_HSYNC_POLARITY;
        pDPMonitorContext->TestDpcdDataTiming.HorSyncWidth = (DPCD0022A.TEST_HSYNC_WIDTH_14_8 << 8) + DPCD0022B.TEST_HSYNC_WIDTH_7_0;
        pDPMonitorContext->TestDpcdDataTiming.VSyncPolarity = DPCD0022C.TEST_VSYNC_POLARITY;
        pDPMonitorContext->TestDpcdDataTiming.VerSyncWidth = (DPCD0022C.TEST_VSYNC_WIDTH_14_8 << 8) + DPCD0022D.TEST_VSYNC_WIDTH_7_0;
        pDPMonitorContext->TestDpcdDataTiming.HorWidth = (DPCD0022E.TEST_H_WIDTH_15_8 << 8) + DPCD0022F.TEST_H_WIDTH_7_0;
    }
    else
    {
        cbDebugPrint((MAKE_LEVEL(DP, ERROR), "%s(%d): read DPCD reg(Offset = 0x%x, len = %d) fail\n", 
            FUNCTION_NAME, LINE_NUM, 0x220, 16));
        bStatus = CBIOS_FALSE;
    }

    if(cbDIU_Aux_Dpcd_Read(pcbe, DPModuleIndex, 0x230, Data, 5))
    {
        DPCD00230.Value = Data[0];
        DPCD00231.Value = Data[1];
        DPCD00232.Value = Data[2];
        DPCD00233.Value = Data[3];
        DPCD00234.Value = Data[4];

        pDPMonitorContext->TestDpcdDataTiming.VerWidth = (DPCD00230.TEST_V_WIDTH_15_8 << 8) + DPCD00231.TEST_V_WIDTH_7_0;
        pDPMonitorContext->TestDpcdDataTiming.ClockSynAsyn = DPCD00232.TEST_SYNCHRONOUS_CLOCK;
        pDPMonitorContext->TestDpcdDataTiming.ColorFormat = DPCD00232.TEST_COLOR_FORMAT;
        pDPMonitorContext->TestDpcdDataTiming.DynamicRange = DPCD00232.TEST_DYNAMIC_RANGE;
        pDPMonitorContext->TestDpcdDataTiming.YCbCrCoefficients = DPCD00232.TEST_YCBCR_COEFFICIENTS;
        if (DPCD00232.TEST_BIT_DEPTH == 0)
        {
            pDPMonitorContext->TestDpcdDataTiming.BitDepthPerComponet = 6;
        }
        else if (DPCD00232.TEST_BIT_DEPTH == 1)
        {
            pDPMonitorContext->TestDpcdDataTiming.BitDepthPerComponet = 8;
        }
        else if (DPCD00232.TEST_BIT_DEPTH == 2)
        {
            pDPMonitorContext->TestDpcdDataTiming.BitDepthPerComponet = 10;
        }
        else if (DPCD00232.TEST_BIT_DEPTH == 3)
        {
            pDPMonitorContext->TestDpcdDataTiming.BitDepthPerComponet = 12;
        }
        else if (DPCD00232.TEST_BIT_DEPTH == 4)
        {
            pDPMonitorContext->TestDpcdDataTiming.BitDepthPerComponet = 16;
        }
        pDPMonitorContext->TestDpcdDataTiming.IsInterlaced = DPCD00233.TEST_INTERLACED;
        RefreshRate = DPCD00234.TEST_REFRESH_RATE_NUMERATOR;
        pDPMonitorContext->TestDpcdDataTiming.DClk = pDPMonitorContext->TestDpcdDataTiming.HorTotal * pDPMonitorContext->TestDpcdDataTiming.VerTotal
            * RefreshRate / 100;
    }
    else
    {
        cbDebugPrint((MAKE_LEVEL(DP, ERROR), "%s(%d): read DPCD reg(Offset = 0x%x, len = %d) fail?\n", 
            FUNCTION_NAME, LINE_NUM, 0x230, 5));
        bStatus = CBIOS_FALSE;
    }

    // Enhanced Frame Mode supported flag cannot be gotten from DPCD Test Request Field
    pDPMonitorContext->TestDpcdDataTiming.EnhancedFrameMode = pDPMonitorContext->bSupportEnhanceMode;

    cbDebugPrint((MAKE_LEVEL(DP, DEBUG), "%s: LinkRate = 0x%x\n", FUNCTION_NAME, pDPMonitorContext->TestDpcdDataTiming.LinkRate));
    cbDebugPrint((MAKE_LEVEL(DP, DEBUG), "%s: LaneCount = %d\n", FUNCTION_NAME, pDPMonitorContext->TestDpcdDataTiming.LaneCount));
    cbDebugPrint((MAKE_LEVEL(DP, DEBUG), "%s: TestPattern = %d\n", FUNCTION_NAME, pDPMonitorContext->TestDpcdDataTiming.TestPattern));
    cbDebugPrint((MAKE_LEVEL(DP, DEBUG), "%s: ClockSynAsyn = %d\n", FUNCTION_NAME, pDPMonitorContext->TestDpcdDataTiming.ClockSynAsyn));
    cbDebugPrint((MAKE_LEVEL(DP, DEBUG), "%s: DynamicRange = %d\n", FUNCTION_NAME, pDPMonitorContext->TestDpcdDataTiming.DynamicRange));
    cbDebugPrint((MAKE_LEVEL(DP, DEBUG), "%s: ColorFormat = %d\n", FUNCTION_NAME, pDPMonitorContext->TestDpcdDataTiming.ColorFormat));
    cbDebugPrint((MAKE_LEVEL(DP, DEBUG), "%s: YCbCrCoefficients = %d\n", FUNCTION_NAME, pDPMonitorContext->TestDpcdDataTiming.YCbCrCoefficients));
    cbDebugPrint((MAKE_LEVEL(DP, DEBUG), "%s: EnhancedFrameMode = %d\n", FUNCTION_NAME, pDPMonitorContext->TestDpcdDataTiming.EnhancedFrameMode));
    cbDebugPrint((MAKE_LEVEL(DP, DEBUG), "%s: BitDepthPerComponet = %d\n", FUNCTION_NAME, pDPMonitorContext->TestDpcdDataTiming.BitDepthPerComponet));
    cbDebugPrint((MAKE_LEVEL(DP, DEBUG), "%s: IsInterlaced = %d\n", FUNCTION_NAME, pDPMonitorContext->TestDpcdDataTiming.IsInterlaced));
    cbDebugPrint((MAKE_LEVEL(DP, DEBUG), "%s: HorTotal = %d\n", FUNCTION_NAME, pDPMonitorContext->TestDpcdDataTiming.HorTotal));
    cbDebugPrint((MAKE_LEVEL(DP, DEBUG), "%s: HorWidth = %d\n", FUNCTION_NAME, pDPMonitorContext->TestDpcdDataTiming.HorWidth));
    cbDebugPrint((MAKE_LEVEL(DP, DEBUG), "%s: HorSyncStart = %d\n", FUNCTION_NAME, pDPMonitorContext->TestDpcdDataTiming.HorSyncStart));
    cbDebugPrint((MAKE_LEVEL(DP, DEBUG), "%s: HorSyncWidth = %d\n", FUNCTION_NAME, pDPMonitorContext->TestDpcdDataTiming.HorSyncWidth));
    cbDebugPrint((MAKE_LEVEL(DP, DEBUG), "%s: HSyncPolarity = %d\n", FUNCTION_NAME, pDPMonitorContext->TestDpcdDataTiming.HSyncPolarity));
    cbDebugPrint((MAKE_LEVEL(DP, DEBUG), "%s: VerTotal = %d\n", FUNCTION_NAME, pDPMonitorContext->TestDpcdDataTiming.VerTotal));
    cbDebugPrint((MAKE_LEVEL(DP, DEBUG), "%s: VerWidth = %d\n", FUNCTION_NAME, pDPMonitorContext->TestDpcdDataTiming.VerWidth));
    cbDebugPrint((MAKE_LEVEL(DP, DEBUG), "%s: VerSyncStart = %d\n", FUNCTION_NAME, pDPMonitorContext->TestDpcdDataTiming.VerSyncStart));
    cbDebugPrint((MAKE_LEVEL(DP, DEBUG), "%s: VerSyncWidth = %d\n", FUNCTION_NAME, pDPMonitorContext->TestDpcdDataTiming.VerSyncWidth));
    cbDebugPrint((MAKE_LEVEL(DP, DEBUG), "%s: VSyncPolarity = %d\n", FUNCTION_NAME, pDPMonitorContext->TestDpcdDataTiming.VSyncPolarity));
    cbDebugPrint((MAKE_LEVEL(DP, DEBUG), "%s: DClk = %d\n", FUNCTION_NAME, pDPMonitorContext->TestDpcdDataTiming.DClk));

    return bStatus;
}

static CBIOS_BOOL cbDPMonitor_ProcTestPattern(PCBIOS_EXTENSION_COMMON pcbe, PCBIOS_DP_MONITOR_CONTEXT pDPMonitorContext)
{
    CBIOS_MODULE_INDEX DPModuleIndex = cbGetModuleIndex(pcbe, pDPMonitorContext->pDevCommon->DeviceType, CBIOS_MODULE_TYPE_DP);
    CBIOS_U8       Data[1] = {0};
    DPCD_REG_00260 DPCD00260;
    CBIOS_BOOL     bStatus = CBIOS_FALSE;

    //4.4.1.1
    //4.4.1.2
    //4.4.1.3
    //4.4.2
    // Currently not support automated test for pattern test
    // Clear TRAINING_PATTERN_SET byte
    // Write NACK to test response fields of DPCD.
    DPCD00260.Value = 0;
    DPCD00260.TEST_NACK = 1;
    Data[0] = DPCD00260.Value;
    if(!cbDIU_Aux_Dpcd_Write(pcbe, DPModuleIndex, 0x260, Data, 1))
    {
        cbDebugPrint((MAKE_LEVEL(DP, DEBUG), "%s: Write no ack to TEST_RESPONSE fields of DPCD!\n", FUNCTION_NAME));
        bStatus = CBIOS_FALSE;
    }

    //Currently we can not support automation test for pattern test. 
    //If we support this feature, the following code path should be went through.
    if(!cbDPMonitor_GetAutoTestDpcdTiming(pcbe, pDPMonitorContext))
    {
        cbDebugPrint((MAKE_LEVEL(DP, ERROR), "%s: Can not get request pattern fields of DPCD!\n", FUNCTION_NAME));
        bStatus = CBIOS_FALSE;
    }
    else
    {
        bStatus = CBIOS_TRUE;
    }

    return bStatus;
}

static CBIOS_BOOL cbDPMonitor_HandleTestRequest(PCBIOS_EXTENSION_COMMON pcbe, PCBIOS_DP_MONITOR_CONTEXT pDPMonitorContext, PCBIOS_DP_HANDLE_IRQ_PARA pDPHandleIrqPara)
{
    CBIOS_MODULE_INDEX DPModuleIndex = cbGetModuleIndex(pcbe, pDPMonitorContext->pDevCommon->DeviceType, CBIOS_MODULE_TYPE_DP);
    CBIOS_U8       Data[2] = {0};
    DPCD_REG_00218 DPCD00218;
    CBIOS_BOOL     bStatus = CBIOS_TRUE;

    // Read Automated detail request.
    if(cbDIU_Aux_Dpcd_Read(pcbe, DPModuleIndex, 0x218, Data, 1))
    {
        DPCD00218.Value = Data[0];

        if(DPCD00218.TEST_EDID_READ)
        {
            cbDebugPrint((MAKE_LEVEL(DP, DEBUG), "%s: Test Edid Read.\n", FUNCTION_NAME));
            if(cbDPMonitor_ProcTestEdidRead(pcbe, pDPMonitorContext))
            {
                //aux work normally, do detect quickly
                pDPHandleIrqPara->bNeedDetect = 1;
                pDPHandleIrqPara->bDpHpd = 1;
            }
        }

        if(DPCD00218.TEST_LINK_TRAINING)
        {
            cbDebugPrint((MAKE_LEVEL(DP, DEBUG), "%s: Test Link Training.\n", FUNCTION_NAME));
            cbDPMonitor_ProcTestLinkTraining(pcbe, pDPMonitorContext);
        }

        if(DPCD00218.TEST_PATTERN)
        {
            cbDebugPrint((MAKE_LEVEL(DP, DEBUG), "%s: Test Pattern.\n", FUNCTION_NAME));
            if(cbDPMonitor_ProcTestPattern(pcbe, pDPMonitorContext))
            {
                if (cbDPMonitor_LinkTraining(pcbe, pDPMonitorContext, CBIOS_FALSE))
                {
                    if (cbDPMonitor_SetUpMainLink(pcbe, pDPMonitorContext))
                    {
                        // the infoframe will be cleared when link_training, so need set infoframe again after retraining
                        cbDPMonitor_SetInfoFrame(pcbe, pDPMonitorContext);
                    }
                }
                pDPHandleIrqPara->bNeedDetect = 1;
                pDPHandleIrqPara->bDpHpd = 1;
            }
        }
    }
    else
    {
        cbDebugPrint((MAKE_LEVEL(DP, ERROR), "%s(%d): why read DPCD reg TEST_REQUEST fail?\n", FUNCTION_NAME, LINE_NUM));
        bStatus = CBIOS_FALSE;
    }

    return bStatus;
}

static CBIOS_BOOL cbDPMonitor_PutNextCBusRecvEvent_locked(PCBIOS_EXTENSION_COMMON pcbe, PCBIOS_DP_MONITOR_CONTEXT pDPMonitorContext, DP_RECV_EVENT *pRecvEvent)
{
    CBIOS_BOOL bRet = CBIOS_TRUE;
    CBIOS_U64  oldIrql = 0;

    oldIrql = cb_AcquireLock(pcbe->pSpinLock, CBIOS_OS_SPIN_LOCK);

    if (!QUEUE_FULL(pDPMonitorContext->RecvQueue))
    {
        if (pRecvEvent != CBIOS_NULL)
        {
            pDPMonitorContext->RecvQueue.Queue[pDPMonitorContext->RecvQueue.Tail] = *pRecvEvent;
            ADVANCE_QUEUE_TAIL(pDPMonitorContext->RecvQueue);
        }

        bRet = CBIOS_TRUE;
    }
    else
    {
        //queue is full
        bRet = CBIOS_FALSE;
    }

    cb_ReleaseLock(pcbe->pSpinLock, CBIOS_OS_SPIN_LOCK, oldIrql);

    return bRet;
}

static CBIOS_BOOL cbDPMonitor_IsRecvEventQueueEmpty_locked(PCBIOS_EXTENSION_COMMON pcbe, PCBIOS_DP_MONITOR_CONTEXT pDPMonitorContext)
{
    CBIOS_BOOL bRet = CBIOS_TRUE;
    CBIOS_U64  oldIrql = 0;

    oldIrql = cb_AcquireLock(pcbe->pSpinLock, CBIOS_OS_SPIN_LOCK);

    if (QUEUE_EMPTY(pDPMonitorContext->RecvQueue))
    {
        bRet = CBIOS_TRUE;
    }
    else
    {
        bRet = CBIOS_FALSE;
    }

    cb_ReleaseLock(pcbe->pSpinLock, CBIOS_OS_SPIN_LOCK, oldIrql);

    return bRet;
}

static CBIOS_BOOL cbDPMonitor_GetNextCBusRecvEvent_locked(PCBIOS_EXTENSION_COMMON pcbe, PCBIOS_DP_MONITOR_CONTEXT pDPMonitorContext, DP_RECV_EVENT *pRecvEvent)
{
    CBIOS_BOOL bRet = CBIOS_TRUE;
    CBIOS_U64  oldIrql = 0;

    oldIrql = cb_AcquireLock(pcbe->pSpinLock, CBIOS_OS_SPIN_LOCK);

    if (QUEUE_EMPTY(pDPMonitorContext->RecvQueue))
    {
        bRet = CBIOS_FALSE;
    }
    else
    {
        cb_memcpy(pRecvEvent, &(pDPMonitorContext->RecvQueue.Queue[pDPMonitorContext->RecvQueue.Head]), sizeof(DP_RECV_EVENT));
        ADVANCE_QUEUE_HEAD(pDPMonitorContext->RecvQueue)
    }

    cb_ReleaseLock(pcbe->pSpinLock, CBIOS_OS_SPIN_LOCK, oldIrql);

    return bRet;
}

static CBIOS_VOID cbDPMonitor_NotifyDPEvent(PCBIOS_EXTENSION_COMMON pcbe, PCBIOS_DP_MONITOR_CONTEXT pDPMonitorContext, CBIOS_U8 EventCode, CBIOS_U8 EventParam)
{
    CBIOS_DP_NOTIFY_EVENT_PARA NotifyPara;
    NotifyPara.Size = sizeof(CBIOS_DP_NOTIFY_EVENT_PARA);
    NotifyPara.DeviceId = pDPMonitorContext->pDevCommon->DeviceType;
    NotifyPara.EventCode = EventCode;
    NotifyPara.EventParam = EventParam;

    cbTraceEnter(DP);

    if (pDPMonitorContext->Notifications.NotifyDPEvent != CBIOS_NULL)
    {
        pDPMonitorContext->Notifications.NotifyDPEvent(pDPMonitorContext->Notifications.Private, &NotifyPara);
    }

    cbTraceExit(DP);
}


static CBIOS_BOOL cbDPMonitor_ProcAutomatedTestRequest(PCBIOS_EXTENSION_COMMON pcbe, PCBIOS_DP_MONITOR_CONTEXT pDPMonitorContext)
{
    CBIOS_MODULE_INDEX DPModuleIndex = cbGetModuleIndex(pcbe, pDPMonitorContext->pDevCommon->DeviceType, CBIOS_MODULE_TYPE_DP);
    CBIOS_U8       Data[2]= {0};
    DPCD_REG_00218 DPCD00218;
    CBIOS_BOOL     bStatus = CBIOS_TRUE;

    // Read Automated detail request.
    if(cbDIU_Aux_Dpcd_Read(pcbe, DPModuleIndex, 0x218, Data, 1))
    {
        DPCD00218.Value = Data[0];

        if(DPCD00218.TEST_EDID_READ)
        {
            cbDebugPrint((MAKE_LEVEL(DP, DEBUG), "%s: Test Edid Read.\n", FUNCTION_NAME));
            if(cbDPMonitor_ProcTestEdidRead(pcbe, pDPMonitorContext))
            {
                cbDPMonitor_NotifyDPEvent(pcbe, pDPMonitorContext, CBIOS_DP_EVENT_TEST_EDID, 0);
            }
        }

        if(DPCD00218.TEST_LINK_TRAINING)
        {
            cbDebugPrint((MAKE_LEVEL(DP, DEBUG), "%s: Test Link Training.\n", FUNCTION_NAME));
            cbDPMonitor_ProcTestLinkTraining(pcbe, pDPMonitorContext);
        }

        if(DPCD00218.TEST_PATTERN)
        {
            cbDebugPrint((MAKE_LEVEL(DP, DEBUG), "%s: Test Pattern.\n", FUNCTION_NAME));
            if(cbDPMonitor_ProcTestPattern(pcbe, pDPMonitorContext))
            {
                if (cbDPMonitor_LinkTraining(pcbe, pDPMonitorContext, CBIOS_FALSE))
                {
                    if (cbDPMonitor_SetUpMainLink(pcbe, pDPMonitorContext))
                    {
                        // the infoframe will be cleared when link_training, so need set infoframe again after retraining
                        cbDPMonitor_SetInfoFrame(pcbe, pDPMonitorContext);
                    }
                }
                cbDPMonitor_NotifyDPEvent(pcbe, pDPMonitorContext, CBIOS_DP_EVENT_TEST_PATTERN, 0);
            }
        }
    }
    else
    {
        cbDebugPrint((MAKE_LEVEL(DP, ERROR), "%s(%d): why read DPCD reg TEST_REQUEST fail?\n", FUNCTION_NAME, LINE_NUM));
        bStatus = CBIOS_FALSE;
    }

    return bStatus;
}


static CBIOS_BOOL cbDPMonitor_ProcLinkStatusCheck(PCBIOS_EXTENSION_COMMON pcbe, PCBIOS_DP_MONITOR_CONTEXT pDPMonitorContext)
{
    CBIOS_MODULE_INDEX DPModuleIndex = cbGetModuleIndex(pcbe, pDPMonitorContext->pDevCommon->DeviceType, CBIOS_MODULE_TYPE_DP);
    CBIOS_U8  Data[4] = {0};
    CBIOS_U32 ulLaneNum, i;
    CBIOS_BOOL bNeedRetraining = CBIOS_FALSE;
    DPCD_REG_00202 DPCD_00202;
    DPCD_REG_00203 DPCD_00203;
    DPCD_REG_00204 DPCD_00204;
    //DPCD_REG_00205 DPCD_00205;
    CBIOS_BOOL bStatus = CBIOS_TRUE;

    //4.3.2.1
    //4.3.2.2
    //4.3.2.3
    //4.3.2.4

    // Link maintenance hot IRQ signal, need to check link/sink status field of DPCD.
    cbDebugPrint((MAKE_LEVEL(DP, DEBUG), "%s: Not a automated test IRQ, Link maitanance IRQ!\n", FUNCTION_NAME));
    ulLaneNum = pDPMonitorContext->LinkPassLaneNum;

    if(cbDIU_Aux_Dpcd_Read(pcbe, DPModuleIndex, 0x202, Data, 4))
    {
        DPCD_00202.Value = Data[0];
        DPCD_00203.Value = Data[1];
        DPCD_00204.Value = Data[2];

        // 20090317_BC Don't rely on register 204[7] because it's cleared once read, and
        //               we have no idea whether here is the first read instance
        //if(DPCD_00204.LINK_STATUS_UPDATED)  // Link status and Adjust request updated since the last read.
        if(pDPMonitorContext->pDevCommon->PowerState == CBIOS_PM_ON)
        {
            for (i = 0; i < ulLaneNum; i++)
            {
                if ((i == 0) && (!DPCD_00202.LANE0_CR_DONE || !DPCD_00202.LANE0_CHANNEL_EQ_DONE || !DPCD_00202.LANE0_SYMBOL_LOCKED))
                {
                    bNeedRetraining = CBIOS_TRUE;
                    break;
                }
                else if ((i == 1) && (!DPCD_00202.LANE1_CR_DONE || !DPCD_00202.LANE1_CHANNEL_EQ_DONE || !DPCD_00202.LANE1_SYMBOL_LOCKED))
                {
                    bNeedRetraining = CBIOS_TRUE;
                    break;
                }
                else if ((i == 2) && (!DPCD_00203.LANE2_CR_DONE || !DPCD_00203.LANE2_CHANNEL_EQ_DONE || !DPCD_00203.LANE2_SYMBOL_LOCKED))
                {
                    bNeedRetraining = CBIOS_TRUE;
                    break;
                }
                else if ((i == 3) && (!DPCD_00203.LANE3_CR_DONE || !DPCD_00203.LANE3_CHANNEL_EQ_DONE || !DPCD_00203.LANE3_SYMBOL_LOCKED))
                {
                    bNeedRetraining = CBIOS_TRUE;
                    break;
                }
            }

            if(!bNeedRetraining)
            {
                // Check lane align status
                if(!DPCD_00204.INTERLANE_ALIGN_DONE)
                {
                    bNeedRetraining = CBIOS_TRUE;
                }
                /*// check receive port status
                else if(!DPCD_00205.RECEIVE_PORT_0_STATUS) 
                {
                    bNeedRetraining = CBIOS_TRUE;
                }
                else if(!DPCD_00205.RECEIVE_PORT_1_STATUS)
                {
                    bNeedRetraining = CBIOS_TRUE;
                }*/
            }

            cbDebugPrint((MAKE_LEVEL(DP, INFO), "%s: bNeedRetraining: %d.\n", FUNCTION_NAME, bNeedRetraining));

            if(bNeedRetraining)
            {
                if (cbDPMonitor_LinkTraining(pcbe, pDPMonitorContext, CBIOS_FALSE))
                {
                    if (cbDPMonitor_SetUpMainLink(pcbe, pDPMonitorContext))
                    {
                        // the infoframe will be cleared when link_training, so need set infoframe again after retraining
                        cbDPMonitor_SetInfoFrame(pcbe, pDPMonitorContext);
                    }
                    else
                    {
                        cbDebugPrint((MAKE_LEVEL(DP, WARNING),"%s: setting up Main Link failed!\n", FUNCTION_NAME));
                    }
                }
                else
                {
                    cbDebugPrint((MAKE_LEVEL(DP, WARNING), "%s: skip Setting Up MainLink because Link Training failed!\n", FUNCTION_NAME));
                }
        
                pDPMonitorContext->LT_Status = 0;
            }
        }
    }
    else
    {
        cbDebugPrint((MAKE_LEVEL(DP, ERROR), "%s: Can not get sink link status!\n", FUNCTION_NAME));
        bStatus = CBIOS_FALSE;
    }

    return bStatus;
}

static CBIOS_BOOL cbDPMonitor_ProcRecvEvents_Int(PCBIOS_EXTENSION_COMMON pcbe, PCBIOS_DP_MONITOR_CONTEXT pDPMonitorContext, const PDP_RECV_EVENT pRecvEvent)
{
    CBIOS_BOOL bRet = CBIOS_TRUE;
    CBIOS_MODULE_INDEX DPModuleIndex = cbGetModuleIndex(pcbe, pDPMonitorContext->pDevCommon->DeviceType, CBIOS_MODULE_TYPE_DP);
    const DP_RECV_EVENT RecvEvent = *pRecvEvent;
    CBIOS_U8       Data[2] = {0};
    DPCD_REG_00201 DPCD00201;

    cbTraceEnter(DP);

    switch (RecvEvent.HpdStatus)
    {
    case 1:
    {
        if(cbDualModeDetect(pcbe, pDPMonitorContext->pDevCommon))
        {
            cbDebugPrint((MAKE_LEVEL(DP, DEBUG), "%s: HDMI monitor hotplug in!\n", FUNCTION_NAME));
            cbDPMonitor_NotifyDPEvent(pcbe, pDPMonitorContext, CBIOS_DP_EVENT_HDMI_HOTPLUG_IN, 0);
        }
        else
        {
            cbPHY_DP_SelectEphyMode(pcbe, DPModuleIndex, DP_EPHY_DP_MODE);
            if(cbDIU_Aux_Dpcd_Read(pcbe, DPModuleIndex, 0x201, Data, 1))
            {
                DPCD00201.Value = Data[0];
                if(DPCD00201.AUTOMATED_TEST_REQUEST)
                {
                    // clear AUTOMATED_TEST_REQUEST bit
                    Data[0] = DPCD00201.Value;
                    cbDIU_Aux_Dpcd_Write(pcbe, DPModuleIndex, 0x201, Data, 1);

                    cbDPMonitor_ProcAutomatedTestRequest(pcbe, pDPMonitorContext);
                }
                // normal hotplug event
                else
                {
                    cbDPMonitor_NotifyDPEvent(pcbe, pDPMonitorContext, CBIOS_DP_EVENT_DP_HOTPLUG_IN, 0);
                }
            }
            else
            {
                cbDebugPrint((MAKE_LEVEL(DP, DEBUG), "%s: I2C and Aux all fail, regard it as HDMI monitor hotplug in!\n", FUNCTION_NAME));
                cbDPMonitor_NotifyDPEvent(pcbe, pDPMonitorContext, CBIOS_DP_EVENT_HDMI_HOTPLUG_IN, 0);
            }
        }
    }
    break;

    case 2:
    {
        if(cbDPPort_IsDeviceInDpMode(pcbe, pDPMonitorContext->pDevCommon))
        {
            cbDPMonitor_NotifyDPEvent(pcbe, pDPMonitorContext, CBIOS_DP_EVENT_DP_HOTPLUG_OUT, 0);
        }
        else
        {
            cbDPMonitor_NotifyDPEvent(pcbe, pDPMonitorContext, CBIOS_DP_EVENT_HDMI_HOTPLUG_OUT, 0);
        }
    }
    break;

    case 3:
    {
        cbPHY_DP_SelectEphyMode(pcbe, DPModuleIndex, DP_EPHY_DP_MODE);
        if(cbDIU_Aux_Dpcd_Read(pcbe, DPModuleIndex, 0x200, Data, 2))
        {
            DPCD00201.Value = Data[1];
            if(DPCD00201.AUTOMATED_TEST_REQUEST)
            {
                // clear AUTOMATED_TEST_REQUEST bit
                Data[0] = DPCD00201.Value;
                cbDIU_Aux_Dpcd_Write(pcbe, DPModuleIndex, 0x201, Data, 1);

                cbDPMonitor_ProcAutomatedTestRequest(pcbe, pDPMonitorContext);
            }
            else
            {
                cbDPMonitor_ProcLinkStatusCheck(pcbe, pDPMonitorContext);
            }
        }
        else
        {
            cbDebugPrint((MAKE_LEVEL(DP, ERROR), "%s(%d): read DPCD reg(Offset = 0x%x, len = %d) fail?\n", 
                FUNCTION_NAME, LINE_NUM, 0x200, 2));
            bRet = CBIOS_FALSE;
        }
    }
    break;

    default:
        cbDebugPrint((MAKE_LEVEL(DP, ERROR), "## unkown RecvEvent.HpdStatus = %d\n", RecvEvent.HpdStatus));
        bRet = CBIOS_FALSE;
        break;
    }

    cbTraceExit(DP);
    return bRet;
}

static CBIOS_BOOL cbDPMonitor_ProcRecvEvents(PCBIOS_EXTENSION_COMMON pcbe, PCBIOS_DP_MONITOR_CONTEXT pDPMonitorContext)
{
    CBIOS_BOOL bRet = CBIOS_TRUE;
    DP_RECV_EVENT RecvEvent;

    cbTraceEnter(DP);
    
    if(cbDPMonitor_GetNextCBusRecvEvent_locked(pcbe, pDPMonitorContext, &RecvEvent) == CBIOS_TRUE)
    {
        switch (RecvEvent.EventType)
        {
        case 0:
        {
            cbDPMonitor_ProcRecvEvents_Int(pcbe, pDPMonitorContext, &RecvEvent);
        }
        break;

        default:
            cbDebugPrint((MAKE_LEVEL(DP, ERROR), "unkown RecvEvent.EventType\n"));
            break;
        }
    }
    else
    {
        // empty queue
        bRet = CBIOS_FALSE;
    }
    
    cbTraceExit(DP);
    return bRet;
}

CBIOS_STATUS cbDPMonitor_Isr(PCBIOS_VOID pvcbe, PCBIOS_DP_MONITOR_CONTEXT pDPMonitorContext, PCBIOS_DP_ISR_PARA pDPIsrPara)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    CBIOS_STATUS            Status = CBIOS_OK;
    CBIOS_MODULE_INDEX      DPModuleIndex = cbGetModuleIndex(pcbe, pDPMonitorContext->pDevCommon->DeviceType, CBIOS_MODULE_TYPE_DP);
    DP_RECV_EVENT           RecvEvent;
    CBIOS_U8                HPDStatus = 0;

    if (DPModuleIndex == CBIOS_MODULE_INDEX_INVALID)
    {
        cbDebugPrint((MAKE_LEVEL(DP, ERROR), "%s: invalid DP module index!\n", FUNCTION_NAME));
        return CBIOS_ER_INTERNAL;
    }
    cb_memset(&RecvEvent, 0, sizeof(DP_RECV_EVENT));

    HPDStatus = cbDIU_DP_GetHpdStatus(pcbe, DPModuleIndex, pDPMonitorContext->CurrentDpUsbMode);
    if (HPDStatus == 0)
    {
        cbDebugPrint((MAKE_LEVEL(DP, DEBUG), "%s(%d): why got HPD status: %d\n", FUNCTION_NAME, LINE_NUM, HPDStatus));
    }
    else if ((HPDStatus == CBIOS_HPD_STATUS_IN) || (HPDStatus == CBIOS_HPD_STATUS_OUT) || (HPDStatus == CBIOS_HPD_STATUS_IRQ))
    {
        cbDebugPrint((MAKE_LEVEL(DP, DEBUG), "%s(%d): HPD status: %d\n", FUNCTION_NAME, LINE_NUM, HPDStatus));

        RecvEvent.EventType = 0;
        RecvEvent.HpdStatus = HPDStatus;

        if(!cbDPMonitor_PutNextCBusRecvEvent_locked(pcbe, pDPMonitorContext, &RecvEvent))
        {
            cbDebugPrint((MAKE_LEVEL(DP, ERROR), "%s(%d): Recv Queue is Full\n", FUNCTION_NAME, LINE_NUM));
        }
    }
    else
    {
        cbDebugPrint((MAKE_LEVEL(DP, ERROR), "%s(%d): unknown HPD status: %d\n", FUNCTION_NAME, LINE_NUM, HPDStatus));
    }

    return Status;
}

CBIOS_BOOL cbDPMonitor_WorkThreadMainFunc(PCBIOS_VOID pvcbe, PCBIOS_DP_MONITOR_CONTEXT pDPMonitorContext, PCBIOS_DP_WORKTHREAD_PARA pDPWorkThreadPara)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    CBIOS_BOOL bWorkThreadBusy = CBIOS_FALSE;

    cbTraceEnter(DP);

    if (!cbDPMonitor_IsRecvEventQueueEmpty_locked(pcbe, pDPMonitorContext))
    {
        bWorkThreadBusy = CBIOS_TRUE;
    }
    else
    {
        goto Exit;
    }

    while (cbDPMonitor_ProcRecvEvents(pcbe, pDPMonitorContext));

    cbTraceExit(DP);

Exit:
    return bWorkThreadBusy;
}

CBIOS_STATUS cbDPMonitor_SetNotifications(PCBIOS_VOID pvcbe, PCBIOS_DP_MONITOR_CONTEXT pDPMonitorContext, PCBIOS_DP_NOTIFICATIONS pDPNotifications)
{
    CBIOS_STATUS            Status = CBIOS_OK;

    cbTraceEnter(DP);

    pDPMonitorContext->Notifications.Size = pDPNotifications->Size;
    pDPMonitorContext->Notifications.DeviceId = pDPNotifications->DeviceId;
    pDPMonitorContext->Notifications.Private = pDPNotifications->Private;
    pDPMonitorContext->Notifications.NotifyDPEvent = pDPNotifications->NotifyDPEvent;

    cbTraceExit(DP);
    return Status;
}

CBIOS_STATUS cbDPMonitor_HandleIrq(PCBIOS_VOID pvcbe, PCBIOS_DP_MONITOR_CONTEXT pDPMonitorContext, PCBIOS_DP_HANDLE_IRQ_PARA pDPHandleIrqPara)
{
    CBIOS_STATUS Ret = CBIOS_OK;
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    CBIOS_MODULE_INDEX DPModuleIndex = cbGetModuleIndex(pcbe, pDPMonitorContext->pDevCommon->DeviceType, CBIOS_MODULE_TYPE_DP);
    CBIOS_U8       Data[4] = {0};
    DPCD_REG_00201 DPCD00201;
    DPCD_REG_02006 DPCD02006;
    DPCD_REG_02008 DPCD02008;
    DP_EPHY_MODE  Mode;
    CBIOS_BOOL  bNormalHpd = CBIOS_FALSE;

    if (DPModuleIndex >= DP_MODU_NUM)
    {
        cbDebugPrint((MAKE_LEVEL(DP, ERROR), "%s: invalid DP module index!\n", FUNCTION_NAME));
        return CBIOS_ER_INTERNAL;
    }

    pDPHandleIrqPara->bNeedDetect = 0;
    pDPHandleIrqPara->bDpHpd = 0;
    pDPHandleIrqPara->bNeedResetPsr = 0;

    switch (pDPHandleIrqPara->IntType)
    {
    case CBIOS_DP_INT_STATUS_IN:
    {
        if(cbDualModeDetect(pcbe, pDPMonitorContext->pDevCommon))
        {
            cbDebugPrint((MAKE_LEVEL(DP, DEBUG), "%s: HDMI monitor hotplug in!\n", FUNCTION_NAME));
            //HDMI plug in, need do debaunce
            pDPHandleIrqPara->bNeedDetect = 1;
            pDPHandleIrqPara->bDpHpd = 0;
        }
        else
        {
            Mode = cbPHY_DP_GetEphyMode(pcbe, DPModuleIndex);//save ephy mode
            cbPHY_DP_SelectEphyMode(pcbe, DPModuleIndex, DP_EPHY_DP_MODE);
            if(cbDIU_Aux_Dpcd_Read(pcbe, DPModuleIndex, 0x201, Data, 1))
            {
                DPCD00201.Value = Data[0];
                if(DPCD00201.AUTOMATED_TEST_REQUEST)
                {
                    // clear AUTOMATED_TEST_REQUEST bit
                    Data[0] = DPCD00201.Value;
                    cbDIU_Aux_Dpcd_Write(pcbe, DPModuleIndex, 0x201, Data, 1);

                    cbDPMonitor_HandleTestRequest(pcbe, pDPMonitorContext, pDPHandleIrqPara);
                }
                // normal hotplug event
                else
                {
                    //aux work normally, do detect quickly
                    bNormalHpd = CBIOS_TRUE;
                    pDPHandleIrqPara->bNeedDetect = 1;
                    pDPHandleIrqPara->bDpHpd = 1;
                }
            }
            else
            {
                //both I2C and aux can't work, do debaunce
                pDPHandleIrqPara->bNeedDetect = 1;
                pDPHandleIrqPara->bDpHpd = 0;
            }
            cbPHY_DP_SelectEphyMode(pcbe, DPModuleIndex, Mode);//restore ephy mode

            if(cbDIU_EDP_IsEDPSupported(pcbe, DPModuleIndex)  // is eDP panel
                && (pDPMonitorContext->pDevCommon->bConnected == CBIOS_TRUE))    //previous status is connected
            {
                //eDP port, previous status is connected and receive plug in INT, no need driver to detect
                pDPHandleIrqPara->bNeedDetect = 0;
                pDPHandleIrqPara->bDpHpd = 0;

                //Customer's ESD test will make eDP link fail(black screen), which lead some panel trigger plug in INT(no IRQ INT),
                //Call link status check here to fix this issue
                if(pDPMonitorContext->pDevCommon->PowerState == CBIOS_PM_ON && bNormalHpd)
                {
                    cbDPMonitor_ProcLinkStatusCheck(pcbe, pDPMonitorContext);
                }
            }
        }
    }
    break;
    case CBIOS_DP_INT_IRQ:
    {
        //fix issue chx004sw-1017
        //theoretically, it will not come here when connected HDMI, but if hotplug duration too short,hw maybe mistake hotplug for irq
        if(cbDualModeDetect(pcbe, pDPMonitorContext->pDevCommon))
        {
            pDPHandleIrqPara->bNeedDetect = 1;
            pDPHandleIrqPara->bDpHpd = 0;
        }
        else
        {
            // power on aux channel
            cbPHY_DP_SelectEphyMode(pcbe, DPModuleIndex, DP_EPHY_DP_MODE);
            if(cbDIU_Aux_Dpcd_Read(pcbe, DPModuleIndex, 0x200, Data, 2))
            {
                DPCD00201.Value = Data[1];
                if(DPCD00201.AUTOMATED_TEST_REQUEST)
                {
                    Data[0] = DPCD00201.Value;
                    cbDIU_Aux_Dpcd_Write(pcbe, DPModuleIndex, 0x201, Data, 1);

                    cbDPMonitor_HandleTestRequest(pcbe, pDPMonitorContext, pDPHandleIrqPara);
                }
                else
                {
                    cbDPMonitor_ProcLinkStatusCheck(pcbe, pDPMonitorContext);
                }
            }
            else
            {
                cbDebugPrint((MAKE_LEVEL(DP, ERROR), "%s(%d): read DPCD reg(Offset = 0x%x, len = %d) fail?\n",
                                           FUNCTION_NAME, LINE_NUM, 0x200, 2));
                Ret = CBIOS_ER_INTERNAL;
            }

            if(cbDIU_EDP_IsEDPSupported(pcbe, DPModuleIndex) && pDPMonitorContext->bSupportPsr)
            {
                //edp psr related errors
                if(cbDIU_Aux_Dpcd_Read(pcbe, DPModuleIndex, DP_PSR_ERROR_STATUS, Data, 3))
                {
                    DPCD02008.Value = Data[2];
                    if((DPCD02008.SINK_SELF_REFRESH_STATUS & DP_PSR_SINK_STATE_MASK) == DP_PSR_SINK_INTERNAL_ERROR)
                    {
                        cbDebugPrint((MAKE_LEVEL(DP, DEBUG), "DP_PSR_STATUS = 0x%x\n", DPCD02008.Value));
                        pDPHandleIrqPara->bNeedResetPsr = 1;
                    }

                    DPCD02006.Value = Data[0];
                    if(DPCD02006.LINK_CRC_ERROR || DPCD02006.RFB_STORAGE_ERROR || DPCD02006.VSC_SDP_UNCORRECTABLE_ERROR)
                    {
                        cbDebugPrint((MAKE_LEVEL(DP, DEBUG), "DP_PSR_ERROR_STATUS = 0x%x\n", DPCD02006.Value));
                        // clear error bit
                        Data[0] = DPCD02006.Value;
                        cbDIU_Aux_Dpcd_Write(pcbe, DPModuleIndex, DP_PSR_ERROR_STATUS, Data, 1);

                        pDPHandleIrqPara->bNeedResetPsr = 1;
                    }
                }
                else
                {
                    cbDebugPrint((MAKE_LEVEL(DP, ERROR), "%s(%d): read DPCD reg(Offset = 0x%x, len = %d) fail?\n",
                                               FUNCTION_NAME, LINE_NUM, DP_PSR_ERROR_STATUS, 3));
                    Ret = CBIOS_ER_INTERNAL;
                }
            }
        }
    }
    break;
    default:
        cbDebugPrint((MAKE_LEVEL(DP, ERROR), "## unkown RecvEvent.HpdStatus = %d\n", pDPHandleIrqPara->IntType));
        Ret = CBIOS_ER_INVALID_HOTPLUG;
        break;
    }
    
    return Ret;
}

CBIOS_STATUS cbDPMonitor_GetCustomizedTiming(PCBIOS_VOID pvcbe, PCBIOS_DP_MONITOR_CONTEXT pDPMonitorContext, PCBIOS_DP_CUSTOMIZED_TIMING pDPCustomizedTiming)
{
    CBIOS_STATUS            Status = CBIOS_OK;
    PCBiosCustmizedDestTiming pCustomizedTiming = &pDPMonitorContext->TestDpcdDataTiming;

    cbTraceEnter(DP);

    if (pCustomizedTiming->HorWidth && pCustomizedTiming->VerWidth)
    {
        cb_memcpy(&pDPCustomizedTiming->CustmizedTiming, pCustomizedTiming, sizeof(CBiosCustmizedDestTiming));
    }
    else
    {
        cbDebugPrint((MAKE_LEVEL(DP, ERROR), "%s: invalid Customized Timing\n", FUNCTION_NAME));
        Status = CBIOS_ER_INTERNAL;
    }

    cbTraceExit(DP);
    return Status;
}

CBIOS_VOID cbDPMonitor_SetDither(PCBIOS_VOID pvcbe, CBIOS_U32 bpc, CBIOS_BOOL bOn, CBIOS_MODULE_INDEX DPModuleIndex )
{
    PCBIOS_EXTENSION_COMMON pcbe          = (PCBIOS_EXTENSION_COMMON)pvcbe;
    REG_SR36 SR36_value, SR36_mask;

    if(bOn)
    {
        switch(bpc)
        {
        case 5:
            SR36_value.Value = 0;
            SR36_value.Dither_Pattern_Select = 1;
            SR36_value.Dither_EN = 1;
            SR36_value.Dither_Bit_Select = 3;
            break;

        case 6:
            SR36_value.Value = 0;
            SR36_value.Dither_Pattern_Select = 1;
            SR36_value.Dither_EN = 1;
            SR36_value.Dither_Bit_Select = 4;
            break;

        case 7:
            SR36_value.Value = 0;
            SR36_value.Dither_Pattern_Select = 1;
            SR36_value.Dither_EN = 1;
            SR36_value.Dither_Bit_Select = 5;
            break;

        case 8:
            SR36_value.Value = 0;
            SR36_value.Dither_Pattern_Select = 1;
            SR36_value.Dither_EN = 1;
            SR36_value.Dither_Bit_Select = 6;
            break;

        case 9:
            SR36_value.Value = 0;
            SR36_value.Dither_Pattern_Select = 1;
            SR36_value.Dither_EN = 1;
            SR36_value.Dither_Bit_Select = 7;
            break;

        default:
            SR36_value.Value = 0;
            break;
        }
    }
    else
    {
        SR36_value.Value = 0;
    }

    SR36_mask.Value = 0xFF;
    SR36_mask.Dither_Pattern_Select = 0;
    SR36_mask.Dither_EN = 0;
    SR36_mask.Dither_Bit_Select = 0;

    if(DPModuleIndex == CBIOS_MODULE_INDEX2)
    {
        cbMMIOWriteReg(pcbe,SR_B_36, SR36_value.Value, SR36_mask.Value);
    }
    else if (DPModuleIndex == CBIOS_MODULE_INDEX3)
    {
        cbMMIOWriteReg(pcbe,SR_T_36, SR36_value.Value, SR36_mask.Value);
    }
    else
    {
        cbMMIOWriteReg(pcbe,SR_36, SR36_value.Value, SR36_mask.Value);    
    }
}

CBIOS_VOID cbDPMonitor_TestEDIDCheckSum(PCBIOS_VOID pvcbe, PCBIOS_DP_MONITOR_CONTEXT pDPMonitorContext, CBIOS_U8 checksum)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    CBIOS_U8                Data[2] = {0};
    DPCD_REG_00260          DPCD_00260 = {0};
    DPCD_REG_00261          DPCD_00261 = {0};
    CBIOS_MODULE_INDEX  DPModuleIndex;

    DPModuleIndex = cbGetModuleIndex(pcbe, pDPMonitorContext->pDevCommon->DeviceType, CBIOS_MODULE_TYPE_DP);
    if (DPModuleIndex >= DP_MODU_NUM)
    {
        cbDebugPrint((MAKE_LEVEL(DP, ERROR), "%s: invalid DP module index!\n", FUNCTION_NAME));
        return;
    }

    DPCD_00260.Value = 0;
    DPCD_00260.TEST_EDID_CHECKSUM_WRITE = 1;
    DPCD_00261.Value = 0;
    DPCD_00261.TEST_EDID_CHECKSUM = checksum;

    Data[0] = DPCD_00260.Value;
    Data[1] = DPCD_00261.Value;

    if(!cbDIU_Aux_Dpcd_Write(pcbe, DPModuleIndex, 0x260, Data, 2))
    {
        cbDebugPrint((MAKE_LEVEL(DP, ERROR), "%s: Write EDID checksum to TEST_EDID_CHECKSUM fields of DPCD failed!\n", FUNCTION_NAME));
    }
}


#endif



