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
** HDMI hw block interface function implementation.
**
** NOTE:
**
******************************************************************************/

#include "CBiosDIU_HDMI.h"
#include "CBiosDIU_HDTV.h"
#include "CBiosChipShare.h"
#include "../CBiosHwShare.h"
#include "CBiosDIU_HDAC.h"


CBIOS_U32 HDMI_REG_GEN_CTRL[HDMI_MODU_NUM] = {0x8280,  0x33D70,  0x34470,  0x34B70};
CBIOS_U32 HDMI_REG_INFO_FRAME[HDMI_MODU_NUM] = {0x8284,  0x33D74,  0x34474,  0x34B74};
CBIOS_U32 HDMI_REG_AUDIO_CTRL[HDMI_MODU_NUM] = {0x8294,  0x33D78,  0x34478,  0x34B78};
CBIOS_U32 HDMI_REG_AUDIO_CTSN[HDMI_MODU_NUM] = {0x83A8,  0x33D7C,  0x3447C,  0x34B7C};
CBIOS_U32 HDMI_REG_AUDIO_CTS[HDMI_MODU_NUM] = {0x83AC,  0x33D80,  0x34480,  0x34B80};
CBIOS_U32 HDMI_REG_CTRL[HDMI_MODU_NUM] = {0x336B0, 0x33D88,  0x34488,  0x34B88};
CBIOS_U32 HDMI_REG_SCDC_CTRL[HDMI_MODU_NUM] = {0x336B8, 0x33D84,  0x34484,  0x34B84};


CBIOS_VOID cbDIU_HDMI_SetHDCPDelay(PCBIOS_VOID pvcbe, CBIOS_MODULE_INDEX HDMIModuleIndex)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    REG_MM8280 HDMIGenCtrlRegValue, HDMIGenCtrlRegMask;

    if (HDMIModuleIndex >= HDMI_MODU_NUM)
    {
        cbDebugPrint((MAKE_LEVEL(HDMI, ERROR), "%s: invalid HDMI module index!\n", FUNCTION_NAME));
        return;
    }

    HDMIGenCtrlRegValue.Value = 0;
    HDMIGenCtrlRegValue.Delay_for_HDCP = HDMI_DELAY_FOR_HDCP - HDMI_LEADING_GUARD_BAND_PERIOD - 
                                         HDMI_PREAMBLE_PERIOD + HDCP_HW_PROCESS_PERIOD;
    HDMIGenCtrlRegValue.Delay_for_HDCP_SEL = 1;

    HDMIGenCtrlRegMask.Value = 0xFFFFFFFF;
    HDMIGenCtrlRegMask.Delay_for_HDCP = 0;
    HDMIGenCtrlRegMask.Delay_for_HDCP_SEL = 0;

    cbMMIOWriteReg32(pcbe, HDMI_REG_GEN_CTRL[HDMIModuleIndex], HDMIGenCtrlRegValue.Value, HDMIGenCtrlRegMask.Value);
}

CBIOS_VOID cbDIU_HDMI_SetHVSync(PCBIOS_VOID pvcbe, CBIOS_MODULE_INDEX HDMIModuleIndex, CBIOS_U8 HVPolarity)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    REG_MM8280 HDMIGenCtrlRegValue, HDMIGenCtrlRegMask;

    if (HDMIModuleIndex >= HDMI_MODU_NUM)
    {
        cbDebugPrint((MAKE_LEVEL(HDMI, ERROR), "%s: invalid HDMI module index!\n", FUNCTION_NAME));
        return;
    }

    HDMIGenCtrlRegValue.Value = 0;
    if (HVPolarity & HorNEGATIVE)
    {
        HDMIGenCtrlRegValue.HSYNC_Invert_Enable = 1;
    }
    if (HVPolarity & VerNEGATIVE)
    {
        HDMIGenCtrlRegValue.VSYNC_Invert_Enable = 1;
    }

    HDMIGenCtrlRegMask.Value = 0xFFFFFFFF;
    HDMIGenCtrlRegMask.HSYNC_Invert_Enable = 0;
    HDMIGenCtrlRegMask.VSYNC_Invert_Enable = 0;

    cbMMIOWriteReg32(pcbe, HDMI_REG_GEN_CTRL[HDMIModuleIndex], HDMIGenCtrlRegValue.Value, HDMIGenCtrlRegMask.Value);
}

CBIOS_VOID cbDIU_HDMI_SendInfoFrame(PCBIOS_VOID pvcbe, CBIOS_U32 HDMIMaxPacketNum, CBIOS_ACTIVE_TYPE Device, CBIOS_U32 Length)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    CBIOS_U32 StartAddress = 0;
    CBIOS_MODULE_INDEX HDMIModuleIndex = CBIOS_MODULE_INDEX_INVALID;
    REG_MM8284 HDMIInfoFrameRegValue;

    HDMIModuleIndex = cbGetModuleIndex(pcbe, Device, CBIOS_MODULE_TYPE_HDMI);
    if (HDMIModuleIndex >= HDMI_MODU_NUM)
    {
        cbDebugPrint((MAKE_LEVEL(HDMI, ERROR), "%s: invalid HDMI module index!\n", FUNCTION_NAME));
        return;
    }

    HDMIInfoFrameRegValue.Value = 0;
    if (Length <= 8)
    {
        HDMIInfoFrameRegValue.InfoFrame_FIFO_1_Ready = 1;
        HDMIInfoFrameRegValue.Horiz_Blank_Max_Packets = HDMIMaxPacketNum;
        HDMIInfoFrameRegValue.InfoFrame_FIFO_1_Start_Address = StartAddress;
        HDMIInfoFrameRegValue.InfoFrame_FIFO_1_Length = (Length - 1);
    }
    else
    {
        HDMIInfoFrameRegValue.InfoFrame_FIFO_2_Select = 1;
        HDMIInfoFrameRegValue.INFOFRAME_FIFO_2_READY = 1;
        HDMIInfoFrameRegValue.Horiz_Blank_Max_Packets = HDMIMaxPacketNum;
        HDMIInfoFrameRegValue.InfoFrame_FIFO_2_Start_Address = StartAddress + 8;
        HDMIInfoFrameRegValue.InfoFrame_FIFO_2_Length = (Length - 1 - 8);
    }
    cb_WriteU32(pcbe->pAdapterContext, HDMI_REG_INFO_FRAME[HDMIModuleIndex], HDMIInfoFrameRegValue.Value);
}

CBIOS_VOID cbDIU_HDMI_SetPixelFormat(PCBIOS_VOID pvcbe, CBIOS_MODULE_INDEX HDMIModuleIndex, CBIOS_U32 OutputSignal)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    REG_MM8280 HDMIGenCtrlRegValue, HDMIGenCtrlRegMask;

    if (HDMIModuleIndex >= HDMI_MODU_NUM)
    {
        cbDebugPrint((MAKE_LEVEL(HDMI, ERROR), "%s: invalid HDMI module index!\n", FUNCTION_NAME));
        return;
    }

    HDMIGenCtrlRegValue.Value = 0;
    HDMIGenCtrlRegMask.Value = 0xFFFFFFFF;
    HDMIGenCtrlRegMask.Convert_to_YCbCr422_Enable = 0;
    HDMIGenCtrlRegMask.TMDS_Video_Pixel_Format_Select = 0;
    
    // Make the TMDS's source is RGB TV data or YPbPr
    if(OutputSignal == CBIOS_YCBCR422OUTPUT)  // YCbCr 4:2:2 output
    {
        HDMIGenCtrlRegValue.Convert_to_YCbCr422_Enable = 1;
        HDMIGenCtrlRegValue.TMDS_Video_Pixel_Format_Select = 2;
    }
    else if(OutputSignal == CBIOS_YCBCR444OUTPUT) // YCbCr 4:4:4 output
    {
        HDMIGenCtrlRegValue.Convert_to_YCbCr422_Enable = 0;
        HDMIGenCtrlRegValue.TMDS_Video_Pixel_Format_Select = 1;
    }
    else if(OutputSignal == CBIOS_YCBCR420OUTPUT) // YCbCr 4:2:0 output
    {
        HDMIGenCtrlRegValue.Convert_to_YCbCr422_Enable = 0;
        HDMIGenCtrlRegValue.TMDS_Video_Pixel_Format_Select = 3;
    }
    else // For RGB output
    {
        HDMIGenCtrlRegValue.Convert_to_YCbCr422_Enable = 0;
        HDMIGenCtrlRegValue.TMDS_Video_Pixel_Format_Select = 0;
    }
    cbMMIOWriteReg32(pcbe, HDMI_REG_GEN_CTRL[HDMIModuleIndex], HDMIGenCtrlRegValue.Value, HDMIGenCtrlRegMask.Value);


    if(OutputSignal == CBIOS_YCBCR420OUTPUT)
    {
        cbDIU_HDMI_EnableYCbCr420(pcbe, HDMIModuleIndex, CBIOS_TRUE);
    }
    else
    {
        cbDIU_HDMI_EnableYCbCr420(pcbe, HDMIModuleIndex, CBIOS_FALSE);
    }
}

CBIOS_VOID cbDIU_HDMI_SetColorDepth(PCBIOS_VOID pvcbe, CBIOS_MODULE_INDEX HDMIModuleIndex, CBIOS_U8 ColorDepth, CBIOS_U32 OutputSignal)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    CBIOS_BOOL bNeedSetColorDepth = CBIOS_FALSE;
    REG_MM8280 HDMIGenCtrlRegValue, HDMIGenCtrlRegMask;
    REG_MM8294 HDMIAudioCtrlRegValue, HDMIAudioCtrlRegMask;
    
    if (HDMIModuleIndex >= HDMI_MODU_NUM)
    {
        cbDebugPrint((MAKE_LEVEL(HDMI, ERROR), "%s: invalid HDMI module index!\n", FUNCTION_NAME));
        return;
    }

    HDMIGenCtrlRegValue.Value = 0;
    HDMIGenCtrlRegMask.Value = 0xFFFFFFFF;
    HDMIGenCtrlRegMask.Deep_Color_Mode = 0;

    HDMIAudioCtrlRegValue.Value = 0;
    HDMIAudioCtrlRegMask.Value = 0xFFFFFFFF;
    HDMIAudioCtrlRegMask.DC_Gen_Cntl_Pkt_EN = 0;
    HDMIAudioCtrlRegMask.PP_SELECT = 0;
    HDMIAudioCtrlRegMask.CD = 0; // color depth
    HDMIAudioCtrlRegMask.Default_Phase = 0;

    if (pcbe->ChipCaps.IsSupportDeepColor)
    {
        bNeedSetColorDepth = CBIOS_TRUE;
    }
    else
    {
        bNeedSetColorDepth = CBIOS_FALSE;
    }

    if (bNeedSetColorDepth)
    {
        if (ColorDepth == 30)
        {
            HDMIGenCtrlRegValue.Deep_Color_Mode = 1;

            HDMIAudioCtrlRegValue.DC_Gen_Cntl_Pkt_EN = 1;
            HDMIAudioCtrlRegValue.PP_SELECT = 0;
            HDMIAudioCtrlRegValue.CD = 5;
            HDMIAudioCtrlRegValue.Default_Phase = 0;
        }
        else if ((ColorDepth == 36) && (OutputSignal != CBIOS_YCBCR422OUTPUT))
        {
            HDMIGenCtrlRegValue.Deep_Color_Mode = 2;

            HDMIAudioCtrlRegValue.DC_Gen_Cntl_Pkt_EN = 1;
            HDMIAudioCtrlRegValue.PP_SELECT = 0;
            HDMIAudioCtrlRegValue.CD = 6;
            HDMIAudioCtrlRegValue.Default_Phase = 0;
        }
        else//set to 24 bit by default
        {
            HDMIGenCtrlRegValue.Deep_Color_Mode = 0;

            HDMIAudioCtrlRegValue.DC_Gen_Cntl_Pkt_EN = 0;
            HDMIAudioCtrlRegValue.PP_SELECT = 0;
            HDMIAudioCtrlRegValue.CD = 0;
            HDMIAudioCtrlRegValue.Default_Phase = 0;
        }

        cbMMIOWriteReg32(pcbe, HDMI_REG_GEN_CTRL[HDMIModuleIndex], HDMIGenCtrlRegValue.Value, HDMIGenCtrlRegMask.Value);
        cbMMIOWriteReg32(pcbe, HDMI_REG_AUDIO_CTRL[HDMIModuleIndex], HDMIAudioCtrlRegValue.Value, HDMIAudioCtrlRegMask.Value);
    }
}

// set HDMI module mode between HDMI mode and DVI mode
CBIOS_VOID cbDIU_HDMI_SetModuleMode(PCBIOS_VOID pvcbe, CBIOS_MODULE_INDEX HDMIModuleIndex, CBIOS_BOOL bHDMIMode)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    REG_MM8280 HDMIGenCtrlRegValue, HDMIGenCtrlRegMask;

    if (HDMIModuleIndex >= HDMI_MODU_NUM)
    {
        cbDebugPrint((MAKE_LEVEL(HDMI, ERROR), "%s: invalid HDMI module index!\n", FUNCTION_NAME));
        return;
    }

    HDMIGenCtrlRegValue.Value = 0;
    HDMIGenCtrlRegMask.Value = 0xFFFFFFFF;
    HDMIGenCtrlRegMask.DVI_Mode_during_HDMI_Enable = 0;

    if (!bHDMIMode)
    {
        HDMIGenCtrlRegValue.DVI_Mode_during_HDMI_Enable = 1; // DVI mode
    }
    else
    {
        HDMIGenCtrlRegValue.DVI_Mode_during_HDMI_Enable = 0; // HDMI mode
    }

    cbMMIOWriteReg32(pcbe, HDMI_REG_GEN_CTRL[HDMIModuleIndex], HDMIGenCtrlRegValue.Value, HDMIGenCtrlRegMask.Value);
}

CBIOS_VOID cbDIU_HDMI_SelectSource(PCBIOS_VOID pvcbe, CBIOS_MODULE *pHDMIModule, CBIOS_MODULE *pNextModule)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
       
    if ((pHDMIModule == CBIOS_NULL) || (pNextModule == CBIOS_NULL))
    {
        cbDebugPrint((MAKE_LEVEL(DP, ERROR), "%s: the 2nd param or 3rd param is NULL!\n", FUNCTION_NAME));
        return;
    }

    if (pcbe->ChipID == CHIPID_E3K || pcbe->ChipID == CHIPID_CHX004 || pcbe->ChipID == CHIPID_CNE001)
    {
        if (pNextModule->Type == CBIOS_MODULE_TYPE_HDTV)
        {
            cbDIU_HDTV_LBBypass(pcbe, pHDMIModule->Index, CBIOS_FALSE);
        }
        else if (pNextModule->Type == CBIOS_MODULE_TYPE_IGA)
        {
            cbDIU_HDTV_LBBypass(pcbe, pHDMIModule->Index, CBIOS_TRUE);
        }
        else
        {
            cbDebugPrint((MAKE_LEVEL(DP, ERROR),"%s: cannot select HDMI source.\n", FUNCTION_NAME));
        }
    }
}

CBIOS_VOID cbDIU_HDMI_ModuleOnOff(PCBIOS_VOID pvcbe, CBIOS_MODULE_INDEX HDMIModuleIndex, CBIOS_BOOL bTurnOn)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    REG_MM8280 HDMIGenCtrlRegValue, HDMIGenCtrlRegMask;

    if (HDMIModuleIndex >= HDMI_MODU_NUM)
    {
        cbDebugPrint((MAKE_LEVEL(HDMI, ERROR), "%s: invalid HDMI module index!\n", FUNCTION_NAME));
        return;
    }

    HDMIGenCtrlRegValue.Value = 0;
    HDMIGenCtrlRegMask.Value = 0xFFFFFFFF;
    HDMIGenCtrlRegMask.HDMI_Reset= 0;

    if (bTurnOn)
    {
        HDMIGenCtrlRegValue.HDMI_Reset= 1;
    }
    cbMMIOWriteReg32(pcbe, HDMI_REG_GEN_CTRL[HDMIModuleIndex], HDMIGenCtrlRegValue.Value, HDMIGenCtrlRegMask.Value);

    cb_DelayMicroSeconds(1);

    HDMIGenCtrlRegValue.Value = 0;
    HDMIGenCtrlRegMask.Value = 0xFFFFFFFF;
    HDMIGenCtrlRegMask.HDMI_Reset= 0;
    
    HDMIGenCtrlRegValue.HDMI_Reset= 0;
    cbMMIOWriteReg32(pcbe, HDMI_REG_GEN_CTRL[HDMIModuleIndex], HDMIGenCtrlRegValue.Value, HDMIGenCtrlRegMask.Value);

    HDMIGenCtrlRegValue.Value = 0;
    HDMIGenCtrlRegMask.Value = 0xFFFFFFFF;
    HDMIGenCtrlRegMask.HDMI_Enable = 0;

#ifdef __LINUX__
    // Always set HDMI_Enable = 1 and fix HDA Link Position can't work issue when HDMI is off.
    HDMIGenCtrlRegValue.HDMI_Enable = 1;
#else
    if (bTurnOn)
    {
        HDMIGenCtrlRegValue.HDMI_Enable = 1;
    }
    else
    {
        HDMIGenCtrlRegValue.HDMI_Enable = 0;
    }
#endif

    cbMMIOWriteReg32(pcbe, HDMI_REG_GEN_CTRL[HDMIModuleIndex], HDMIGenCtrlRegValue.Value, HDMIGenCtrlRegMask.Value);
}

CBIOS_VOID cbDIU_HDMI_DisableVideoAudio(PCBIOS_VOID pvcbe, CBIOS_MODULE_INDEX HDMIModuleIndex)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    REG_MM8294  HDMIAudioCtrlReg, HDMIAudioCtrlMask;

    if (HDMIModuleIndex >= HDMI_MODU_NUM)
    {
        cbDebugPrint((MAKE_LEVEL(HDMI, ERROR), "%s: invalid HDMI module index!\n", FUNCTION_NAME));
        return;
    }

    //disable video and audio
    HDMIAudioCtrlReg.Value = 0;
    HDMIAudioCtrlReg.Set_AVMUTE_Enable = 1;
    HDMIAudioCtrlReg.Clear_AVMUTE_Enable = 0;

    HDMIAudioCtrlMask.Value = 0xFFFFFFFF;
    HDMIAudioCtrlMask.Set_AVMUTE_Enable = 0;
    HDMIAudioCtrlMask.Clear_AVMUTE_Enable =0;
    cbMMIOWriteReg32(pcbe, HDMI_REG_AUDIO_CTRL[HDMIModuleIndex], HDMIAudioCtrlReg.Value, HDMIAudioCtrlMask.Value);
}

CBIOS_VOID cbDIU_HDMI_EnableVideoAudio(PCBIOS_VOID pvcbe, CBIOS_MODULE_INDEX HDMIModuleIndex)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    REG_MM8294  HDMIAudioCtrlReg, HDMIAudioCtrlMask;

    if (HDMIModuleIndex >= HDMI_MODU_NUM)
    {
        cbDebugPrint((MAKE_LEVEL(HDMI, ERROR), "%s: invalid HDMI module index!\n", FUNCTION_NAME));
        return;
    }

    //disable video and audio
    HDMIAudioCtrlReg.Value = 0;
    HDMIAudioCtrlReg.Set_AVMUTE_Enable = 0;
    HDMIAudioCtrlReg.Clear_AVMUTE_Enable = 1;

    HDMIAudioCtrlMask.Value = 0xFFFFFFFF;
    HDMIAudioCtrlMask.Set_AVMUTE_Enable = 0;
    HDMIAudioCtrlMask.Clear_AVMUTE_Enable =0;
    cbMMIOWriteReg32(pcbe, HDMI_REG_AUDIO_CTRL[HDMIModuleIndex], HDMIAudioCtrlReg.Value, HDMIAudioCtrlMask.Value);
}

CBIOS_VOID cbDIU_HDMI_SetCTSN(PCBIOS_VOID pvcbe, PCBIOS_DEVICE_COMMON pDevCommon, CBIOS_MODULE_INDEX HDMIModuleIndex,
                              CBIOS_MODULE_INDEX HDACModuleIndex, CBIOS_U32 StreamFormat)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    CBIOS_U32   N = 0, CTS = 0, tmp = 0;
    CBIOS_BOOL  bCloseDummyAudio = CBIOS_FALSE;
    REG_MM8294  HDMIAudioCtrlRegValue, HDMIAudioCtrlRegMask;
    REG_MM829C  HDACPacket2RegValue, HDACPacket2RegMask;
    REG_MM82AC  HDACChStatusCtrlRegValue, HDACChStatusCtrlRegMask;
    REG_MM83A8  HDACCtsNRegValue, HDACCtsNRegMask;
    REG_MM83AC  HDACCtsRegValue, HDACCtsRegMask;
    REG_MM336B4 RegMM336B4Value, RegMM336B4Mask;
    REG_MM336B0 RegMM336B0Value, RegMM336B0Mask;
    REG_MM8290_CNE001  RegMM8290Value_cne001, RegMM8290Mask_cne001;

    PCBIOS_DP_CONTEXT pDpContext = container_of(pDevCommon, PCBIOS_DP_CONTEXT, Common);
    CBIOS_U32   TMDS_Clock = pDpContext->HDMIMonitorContext.HDMIClock;
    CBIOS_BOOL  bUseSWValue = CBIOS_FALSE;

    if (HDMIModuleIndex >= HDMI_MODU_NUM)
    {
        cbDebugPrint((MAKE_LEVEL(HDMI, ERROR), "%s: invalid HDMI module index!\n", FUNCTION_NAME));
        return;
    }

    if (HDACModuleIndex >= HDAC_MODU_NUM)
    {
        cbDebugPrint((MAKE_LEVEL(HDMI, ERROR), "%s: invalid HDAC module index!\n", FUNCTION_NAME));
        return;
    }
    // 1. Audio enable
    HDMIAudioCtrlRegValue.Value = 0;
    HDMIAudioCtrlRegValue.HDMI_Audio_Enable = 1;
    HDMIAudioCtrlRegMask.Value = 0xFFFFFFFF;
    HDMIAudioCtrlRegMask.HDMI_Audio_Enable = 0;
    cbMMIOWriteReg32(pcbe, HDMI_REG_AUDIO_CTRL[HDMIModuleIndex], HDMIAudioCtrlRegValue.Value, HDMIAudioCtrlRegMask.Value);

    // 2. Audio Source select
    // DP1/DP2/MHL Dual Mode
    HDMIAudioCtrlRegValue.Value = 0;
    HDMIAudioCtrlRegValue.Select_HDMI_Audio_Source = (HDMIModuleIndex == CBIOS_MODULE_INDEX1)? 0 : 1;
    HDMIAudioCtrlRegMask.Value = 0xFFFFFFFF;
    HDMIAudioCtrlRegMask.Select_HDMI_Audio_Source = 0;
    cbMMIOWriteReg32(pcbe, HDMI_REG_AUDIO_CTRL[HDMIModuleIndex], HDMIAudioCtrlRegValue.Value, HDMIAudioCtrlRegMask.Value);

    // 3. Set CTS/N
    switch(StreamFormat)
    {
    case 192000:
        N = 24576;
        break;
    case 176400:
        N = 25088;
        break;
    case 96000:
        N = 12288;
        break;
    case 88200:
        N = 12544;
        break;
    case 48000:
        N = 6144;
        break;
    case 44100:
        N = 6272;
        break;
    case 32000:
        N = 4096;
        break;
    default:
        N = 6144;
        break;
    }

    if((pcbe->SVID == 0x3A05) && (pcbe->SSID == 0x2001))//patch for cvte, use SW CTS/N
    {
        if(TMDS_Clock)
        {
            tmp = StreamFormat/100;
            CTS = ((CBIOS_U64)TMDS_Clock*N)/(tmp*128);//128*fs=N*ftmds/CTS
            bUseSWValue = CBIOS_TRUE;
        }
    }

    HDACCtsNRegValue.Value = 0;
    HDACCtsNRegValue.N = N;
    HDACCtsNRegValue.CTS = (CTS & 0xFFF);
    HDACCtsNRegMask.Value = 0xFFFFFFFF;
    HDACCtsNRegMask.N = 0;
    HDACCtsNRegMask.CTS = 0;
    cbMMIOWriteReg32(pcbe, HDMI_REG_AUDIO_CTSN[HDMIModuleIndex], HDACCtsNRegValue.Value, HDACCtsNRegMask.Value);

    // set hw CTS
    HDACCtsRegValue.Value = 0;
    HDACCtsRegValue.CTS_Select = bUseSWValue ? 1 : 0;
    HDACCtsRegValue.CTS = (CTS & 0xFF000)>>12;
    HDACCtsRegMask.Value = 0xFFFFFFFF;
    HDACCtsRegMask.CTS_Select = 0;
    HDACCtsRegMask.CTS = 0;
    cbMMIOWriteReg32(pcbe, HDMI_REG_AUDIO_CTS[HDMIModuleIndex], HDACCtsRegValue.Value, HDACCtsRegMask.Value);

    // 4. Set hw ACR ratio & ACR enable [28]
    HDACPacket2RegValue.Value = 0;
    HDACPacket2RegValue.CODEC1_ACR_ENABLE = 1;
    HDACPacket2RegValue.CODEC1_ACR_ratio = CTS;
    HDACPacket2RegMask.Value = 0xFFFFFFFF;
    HDACPacket2RegMask.CODEC1_ACR_ENABLE = 0;
    HDACPacket2RegValue.CODEC1_ACR_ratio = 0;
    cbMMIOWriteReg32(pcbe, HDAC_REG_PACKET2[HDACModuleIndex], HDACPacket2RegValue.Value, HDACPacket2RegMask.Value);

    HDMIAudioCtrlRegValue.Value = 0;
    // HDMIAudioCtrlRegValue.ACR_ratio_select = 1;
    HDMIAudioCtrlRegValue.ACR_ratio_select = bUseSWValue ? 0 : 1;
    HDMIAudioCtrlRegMask.Value = 0xFFFFFFFF;
    HDMIAudioCtrlRegMask.ACR_ratio_select = 0;
    cbMMIOWriteReg32(pcbe, HDMI_REG_AUDIO_CTRL[HDMIModuleIndex], HDMIAudioCtrlRegValue.Value, HDMIAudioCtrlRegMask.Value);

    // set snoop
    RegMM336B4Value.Value = 0;
    RegMM336B4Value.CORB_SNOOP = 0;
    RegMM336B4Value.STRM1_BDL_SNOOP = 0;
    RegMM336B4Value.STRM1_SNOOP = 0;
    RegMM336B4Value.STRM2_BDL_SNOOP = 0;
    RegMM336B4Value.STRM2_SNOOP = 0;
    RegMM336B4Value.DMAP_DES1_SNOOP = 0;
    RegMM336B4Value.DMAP_DES2_SNOOP = 0;
    RegMM336B4Value.RIRB_SNOOP = 0;
    RegMM336B4Value.RIRB_FB = 0;
    RegMM336B4Value.DMAP_DES2_FB = 0;
    RegMM336B4Value.DMAP_DES1_FB = 0;
    RegMM336B4Value.STRM2_FB = 0;
    RegMM336B4Value.STRM2_BDL_FB = 0;
    RegMM336B4Value.STRM1_FB = 0;
    RegMM336B4Value.STRM1_BDL_FB = 0;
    RegMM336B4Value.CORB_FB = 0;
    RegMM336B4Mask.Value = 0xFFFFFFFF;
    RegMM336B4Mask.CORB_SNOOP = 0;
    RegMM336B4Mask.STRM1_BDL_SNOOP = 0;
    RegMM336B4Mask.STRM1_SNOOP = 0;
    RegMM336B4Mask.STRM2_BDL_SNOOP = 0;
    RegMM336B4Mask.STRM2_SNOOP = 0;
    RegMM336B4Mask.DMAP_DES1_SNOOP = 0;
    RegMM336B4Mask.DMAP_DES2_SNOOP = 0;
    RegMM336B4Mask.RIRB_SNOOP = 0;
    RegMM336B4Mask.RIRB_FB = 0;
    RegMM336B4Mask.DMAP_DES2_FB = 0;
    RegMM336B4Mask.DMAP_DES1_FB = 0;
    RegMM336B4Mask.STRM2_FB = 0;
    RegMM336B4Mask.STRM2_BDL_FB = 0;
    RegMM336B4Mask.STRM1_FB = 0;
    RegMM336B4Mask.STRM1_BDL_FB = 0;
    RegMM336B4Mask.CORB_FB = 0;
    cbMMIOWriteReg32(pcbe, 0x336B4, RegMM336B4Value.Value, RegMM336B4Mask.Value);

    if(pcbe->ChipID == CHIPID_CNE001)
    {
        RegMM8290Value_cne001.Value = 0;
        RegMM8290Value_cne001.STRM3_BDL_SNOOP = 0;
        RegMM8290Value_cne001.STRM3_SNOOP = 0;
        RegMM8290Value_cne001.DMAP_DES3_SNOOP = 0;
        RegMM8290Value_cne001.DMAP_DES3_FB = 0;
        RegMM8290Value_cne001.STRM3_FB = 0;
        RegMM8290Value_cne001.STRM3_BDL_FB = 0;
        RegMM8290Mask_cne001.Value = 0xFFFFFFFF;
        RegMM8290Mask_cne001.STRM3_BDL_SNOOP = 0;
        RegMM8290Mask_cne001.STRM3_SNOOP = 0;
        RegMM8290Mask_cne001.DMAP_DES3_SNOOP = 0;
        RegMM8290Mask_cne001.DMAP_DES3_FB = 0;
        RegMM8290Mask_cne001.STRM3_FB = 0;
        RegMM8290Mask_cne001.STRM3_BDL_FB = 0;
        cbMMIOWriteReg32(pcbe, 0x8290, RegMM8290Value_cne001.Value, RegMM8290Mask_cne001.Value);
    }

    if(pcbe->ChipID == CHIPID_E3K || pcbe->ChipID == CHIPID_CHX004 || pcbe->ChipID == CHIPID_CNE001)  //for 480p to play 192k audio
    {
        HDACChStatusCtrlRegValue.Value = 0;
        HDACChStatusCtrlRegValue.multiple_sample = 1;
        HDACChStatusCtrlRegMask.Value = 0xFFFFFFFF;
        HDACChStatusCtrlRegMask.multiple_sample = 0;
        cbMMIOWriteReg32(pcbe, HDAC_REG_CHSTATUS_CTRL[HDACModuleIndex], HDACChStatusCtrlRegValue.Value, HDACChStatusCtrlRegMask.Value);

        RegMM336B0Value.Value = 0;
        RegMM336B0Value.PKTLEG_SEL_DISABLE = 0;
        RegMM336B0Value.PKTLEG_VBLANK = 0XFF;
        RegMM336B0Mask.Value = 0xFFFFFFFF;
        RegMM336B0Mask.PKTLEG_SEL_DISABLE = 0;
        RegMM336B0Mask.PKTLEG_VBLANK = 0;
        cbMMIOWriteReg32(pcbe, HDMI_REG_CTRL[HDMIModuleIndex], RegMM336B0Value.Value, RegMM336B0Mask.Value);
    }

    // Short Audio patch
    if(bCloseDummyAudio)
    {
        HDACChStatusCtrlRegValue.Value = 0;
        HDACChStatusCtrlRegValue.Always_Output_Audio = 0;
        HDACChStatusCtrlRegMask.Value = 0xFFFFFFFF;
        HDACChStatusCtrlRegMask.Always_Output_Audio = 0;
        cbMMIOWriteReg32(pcbe, HDAC_REG_CHSTATUS_CTRL[HDACModuleIndex], HDACChStatusCtrlRegValue.Value, HDACChStatusCtrlRegMask.Value);
    }
}


CBIOS_VOID cbDIU_HDMI_ConfigScrambling(PCBIOS_VOID pvcbe, CBIOS_MODULE_INDEX HDMIModuleIndex, CBIOS_BOOL bEnableScrambling)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    REG_MM336B0 HDMICtrlRegValue, HDMICtrlRegMask;

    if (HDMIModuleIndex >= HDMI_MODU_NUM)
    {
        cbDebugPrint((MAKE_LEVEL(HDMI, ERROR), "%s: invalid HDMI module index!\n", FUNCTION_NAME));
        return;
    }

    HDMICtrlRegValue.Value = 0;
    HDMICtrlRegValue.HDMI1_SCRAMBLE_EN = (bEnableScrambling)? 1 : 0;
    HDMICtrlRegMask.Value = 0xFFFFFFFF;
    HDMICtrlRegMask.HDMI1_SCRAMBLE_EN = 0;

    cbMMIOWriteReg32(pcbe, HDMI_REG_CTRL[HDMIModuleIndex], HDMICtrlRegValue.Value, HDMICtrlRegMask.Value);
}

CBIOS_VOID cbDIU_HDMI_EnableReadRequest(PCBIOS_VOID pvcbe, CBIOS_MODULE_INDEX HDMIModuleIndex, CBIOS_BOOL bEnableRR)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    REG_MM336B8 HDMIScdcCtrlRegValue, HDMIScdcCtrlRegMask;

    if (HDMIModuleIndex >= HDMI_MODU_NUM)
    {
        cbDebugPrint((MAKE_LEVEL(HDMI, ERROR), "%s: invalid HDMI module index!\n", FUNCTION_NAME));
        return;
    }
    
	
    HDMIScdcCtrlRegValue.Value = 0;
    HDMIScdcCtrlRegMask.Value = 0xFFFFFFFF;
    HDMIScdcCtrlRegMask.HDMI1_SCDC_RR_ENABLE = 0;
    HDMIScdcCtrlRegMask.HDMI1_SCDC_HW_DRV_START_ENABLE = 0;
    HDMIScdcCtrlRegMask.HDMI1_SCDC_HW_DRV_STOP_ENABLE = 0;
    HDMIScdcCtrlRegMask.HDMI1_SCDC_START_STOP_ENABLE = 0;

    if(bEnableRR)
    {
        HDMIScdcCtrlRegValue.HDMI1_SCDC_RR_ENABLE = 0;
        HDMIScdcCtrlRegValue.HDMI1_SCDC_HW_DRV_START_ENABLE = 0;
        HDMIScdcCtrlRegValue.HDMI1_SCDC_HW_DRV_STOP_ENABLE = 0;
        HDMIScdcCtrlRegValue.HDMI1_SCDC_START_STOP_ENABLE = 1;

        cbMMIOWriteReg32(pcbe, HDMI_REG_SCDC_CTRL[HDMIModuleIndex], HDMIScdcCtrlRegValue.Value, HDMIScdcCtrlRegMask.Value);
        cbDebugPrint((MAKE_LEVEL(HDMI, DEBUG), "%s: Enable Source Read Request!\n", FUNCTION_NAME));
    }
    else
    {
        HDMIScdcCtrlRegValue.HDMI1_SCDC_RR_ENABLE = 0;
        HDMIScdcCtrlRegValue.HDMI1_SCDC_HW_DRV_START_ENABLE = 0;
        HDMIScdcCtrlRegValue.HDMI1_SCDC_HW_DRV_STOP_ENABLE = 0;
        HDMIScdcCtrlRegValue.HDMI1_SCDC_START_STOP_ENABLE = 0;        

        cbMMIOWriteReg32(pcbe, HDMI_REG_SCDC_CTRL[HDMIModuleIndex], HDMIScdcCtrlRegValue.Value, HDMIScdcCtrlRegMask.Value);
        cbDebugPrint((MAKE_LEVEL(HDMI, DEBUG), "%s: Disable Source Read Request!\n", FUNCTION_NAME));
    }
}

CBIOS_VOID cbDIU_HDMI_EnableYCbCr420(PCBIOS_VOID pvcbe, CBIOS_MODULE_INDEX HDMIModuleIndex, CBIOS_BOOL bEnable420)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    REG_MM336B0 HDMICtrlRegValue, HDMICtrlRegMask;

    if (HDMIModuleIndex >= HDMI_MODU_NUM)
    {
        cbDebugPrint((MAKE_LEVEL(HDMI, ERROR), "%s: invalid HDMI module index!\n", FUNCTION_NAME));
        return;
    }

    HDMICtrlRegValue.Value = 0;
    HDMICtrlRegValue.HDMI1_YC_420_EN = (bEnable420)? 1 : 0;
    HDMICtrlRegValue.HDMI1_YC_420_MODE = (bEnable420)? 1 : 0;
    HDMICtrlRegMask.Value = 0xFFFFFFFF;
    HDMICtrlRegMask.HDMI1_YC_420_EN = 0;
    HDMICtrlRegMask.HDMI1_YC_420_MODE = 0;
    cbMMIOWriteReg32(pcbe, HDMI_REG_CTRL[HDMIModuleIndex], HDMICtrlRegValue.Value, HDMICtrlRegMask.Value);
}

CBIOS_VOID cbDIU_HDMI_EnableClkLane(PCBIOS_VOID pvcbe, CBIOS_MODULE_INDEX HDMIModuleIndex, CBIOS_BOOL bEnableClkLane)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    REG_MM336B0 HDMICtrlRegValue, HDMICtrlRegMask;

    if (HDMIModuleIndex >= HDMI_MODU_NUM)
    {
        cbDebugPrint((MAKE_LEVEL(HDMI, ERROR), "%s: invalid HDMI module index!\n", FUNCTION_NAME));
        return;
    }

    HDMICtrlRegValue.Value = 0;
    HDMICtrlRegValue.HDMI1_CLK_LANE_EN = (bEnableClkLane)? 1 : 0;
    HDMICtrlRegMask.Value = 0xFFFFFFFF;
    HDMICtrlRegMask.HDMI1_CLK_LANE_EN = 0;
    cbMMIOWriteReg32(pcbe, HDMI_REG_CTRL[HDMIModuleIndex], HDMICtrlRegValue.Value, HDMICtrlRegMask.Value);
}

CBIOS_BOOL cbDIU_HDMI_IsOn(PCBIOS_VOID pvcbe, CBIOS_MODULE_INDEX HDMIModuleIndex)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    REG_MM8280     HDMIGenCtrlRegValue;
    CBIOS_BOOL     status = CBIOS_FALSE;

    if (HDMIModuleIndex == CBIOS_MODULE_INDEX_INVALID)
    {
        cbDebugPrint((MAKE_LEVEL(HDMI, ERROR), "%s: invalid HDMI module index!\n", FUNCTION_NAME));
        return status;
    }

    HDMIGenCtrlRegValue.Value = cb_ReadU32(pcbe->pAdapterContext, HDMI_REG_GEN_CTRL[HDMIModuleIndex]);

    if(HDMIGenCtrlRegValue.HDMI_Enable)
    {
        status = CBIOS_TRUE;
    }
    else
    {
        status = CBIOS_FALSE;
    }

    return status;
}

