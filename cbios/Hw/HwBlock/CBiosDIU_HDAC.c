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
** Audio codec hw block interface function implementation.
**
** NOTE:
**
******************************************************************************/

#include "CBiosDIU_HDAC.h"
#include "CBiosDIU_HDMI.h"
#include "CBiosDIU_DP.h"
#include "CBiosChipShare.h"
#include "../CBiosHwShare.h"

CBIOS_U32 HDAC_REG_PACKET1[HDAC_MODU_NUM] =       {0x8298,  0x33DA0, 0x344A4};
CBIOS_U32 HDAC_REG_PACKET2[HDAC_MODU_NUM] =       {0x829C,  0x33DA4, 0x344A8};
CBIOS_U32 HDAC_REG_MODE_RESP[HDAC_MODU_NUM] =     {0x82A0,  0x33DA8, 0x344AC};
CBIOS_U32 HDAC_REG_SW_RESP[HDAC_MODU_NUM] =       {0x82A8,  0x33DB0, 0x344B4};
CBIOS_U32 HDAC_REG_CHSTATUS_CTRL[HDAC_MODU_NUM] = {0x82AC,  0x33DB4, 0x344B8};
CBIOS_U32 HDAC_REG_SUP_PARA[HDAC_MODU_NUM] =      {0x82D4,  0x33DBC, 0x344C0};
CBIOS_U32 HDAC_REG_SAMP_RATE[HDAC_MODU_NUM] =     {0x82E0,  0x33DC8, 0x344CC};
CBIOS_U32 HDAC_REG_CONVERT_CAP[HDAC_MODU_NUM] =   {0x82EC,  0x33DD4, 0x344D8};
CBIOS_U32 HDAC_REG_WIDGET_AUDIO[HDAC_MODU_NUM] =  {0x82F0,  0x33DD8, 0x344DC};
CBIOS_U32 HDAC_REG_WIDGET_PIN[HDAC_MODU_NUM] =    {0x82F4,  0x33DDC, 0x344E0};
CBIOS_U32 HDAC_REG_PIN_SENSE[HDAC_MODU_NUM] =     {0x8308,  0x33DF0, 0x344F4};
CBIOS_U32 HDAC_REG_ELD_BUF[HDAC_MODU_NUM] =       {0x834C,  0x33DF8, 0x344Fc};
CBIOS_U32 HDAC_REG_CTRL_WRITE[HDAC_MODU_NUM] =    {0x837C,  0x33E04, 0x34508};
CBIOS_U32 HDAC_REG_READ_SEL[HDAC_MODU_NUM] =      {0x8380,  0x33E08, 0x3450c};
CBIOS_U32 HDAC_REG_READ_OUT[HDAC_MODU_NUM] =      {0x8384,  0x33E0C, 0x34510};
CBIOS_U32 HDAC_REG_CLK_RATIO_L[HDAC_MODU_NUM] =   {0x8388,  0x33E10, 0x34514};
CBIOS_U32 HDAC_REG_CLK_RATIO_H[HDAC_MODU_NUM] =   {0x838C,  0x33E14, 0x34518};
CBIOS_U32 HDAC_REG_CHSTATUS1[HDAC_MODU_NUM] =     {0x8390,  0x33E18, 0x3451C};
CBIOS_U32 HDAC_REG_CHSTATUS2[HDAC_MODU_NUM] =     {0x8394,  0x33E1C, 0x34520};

static AUDIO_CLOCK_TABLE  AudioClockPreGenerated[] =
{
    {44100   , 1795868992ULL},
    {48000   , 1954687338ULL},
    {22050   ,  897934496ULL},
    {14700   ,  598622997ULL},
    {11025   ,  448967248ULL},
    {8820   ,   359173798ULL},
    {7350   ,   299311498ULL},
    {6300   ,   256552713ULL},
    {5512   ,   224463262ULL},
    {88200   , 3591737984ULL},
    {29400   , 1197245994ULL},
    {17640   ,  718347596ULL},
    {12600   ,  513105426ULL},
    {132300   ,5387606976ULL},
    {66150   , 2693803488ULL},
    {33075   , 1346901744ULL},
    {26460   , 1077521395ULL},
    {18900   ,  769658139ULL},
    {16537   ,  673430510ULL},
    {176400   ,7183475968ULL},
    {58800   , 2394491989ULL},
    {35280   , 1436695193ULL},
    {25200   , 1026210852ULL},
    {24000   ,  977343669ULL},
    {16000   ,  651562446ULL},
    {12000   ,  488671834ULL},
    {9600   ,   390937467ULL},
    {8000   ,   325781223ULL},
    {6857   ,   279235230ULL},
    {6000   ,   244335917ULL},
    {96000   , 3909374676ULL},
    {32000   , 1303124892ULL},
    {19200   ,  781874935ULL},
    {13714   ,  558470461ULL},
    {144000   ,5864062014ULL},
    {72000   , 2932031007ULL},
    {36000   , 1466015503ULL},
    {28800   , 1172812402ULL},
    {20571   ,  837705692ULL},
    {18000   ,  733007751ULL},
    {192000   ,7818749353ULL},
    {64000   , 2606249784ULL},
    {38400   , 1563749870ULL},
    {27428   , 1116940923ULL},
};

static CBIOS_VOID cbDIU_HDAC_SetHDACSettings(PCBIOS_EXTENSION_COMMON pcbe, CBIOS_MODULE_INDEX HDACModuleIndex, CBIOS_U32 StreamFormat)
{
    CBIOS_U64 AudioPacketClock = 40722;    // 2^40
    CBIOS_U32 i = 0;
    CBIOS_BOOL bMatchAudioClock = CBIOS_FALSE;
    CBIOS_U32  HDACReadSelRegIndex;
    REG_MM8298 HDACPacket1RegValue, HDACPacket1RegMask;
    REG_MM829C HDACPacket2RegValue, HDACPacket2RegMask;
    REG_MM82AC HDACChStatusCtrlRegValue, HDACChStatusCtrlRegMask;
    REG_MM8380 HDACReadSelRegValue, HDACReadSelRegMask;
    REG_MM8290_CNE001 HDAC3ReadSelRegValue, HDAC3ReadSelRegMask;
    REG_MM8388 HDACWallClkLRegValue, HDACWallClkLRegMask;
    REG_MM838C HDACWallClkHRegValue, HDACWallClkHRegMask;

    if (HDACModuleIndex >= HDAC_MODU_NUM)
    {
        cbDebugPrint((MAKE_LEVEL(DP, ERROR), "%s: invalid HDAC module index!\n", FUNCTION_NAME));
        return;
    }
    HDACReadSelRegIndex = 0x8380;

    // 1. Audio Packet to Clock Ratio = (Fs * 2^40)/ 27Mhz
    for(i = 0; i < sizeofarray(AudioClockPreGenerated); i++)
    {
        if(StreamFormat == (AudioClockPreGenerated[i].StreamFormat))
        {
            AudioPacketClock = AudioClockPreGenerated[i].AudioPacketClock;
            bMatchAudioClock = CBIOS_TRUE;
            break;
        }
    }

    if(!bMatchAudioClock)
    {
        AudioPacketClock *= StreamFormat;
        cbDebugPrint((MAKE_LEVEL(DP, ERROR), "Invalid StreamFormat, could not match the pre-computed AudioPacketClock!!\n "));
    }

    HDACPacket1RegValue.Value = (CBIOS_U32)AudioPacketClock;
    HDACPacket1RegMask.Value = 0;
    cbMMIOWriteReg32(pcbe, HDAC_REG_PACKET1[HDACModuleIndex], HDACPacket1RegValue.Value, HDACPacket1RegMask.Value);

    HDACPacket2RegValue.Value = 0;
    HDACPacket2RegValue.CODEC1_Audio_Packet_to_DClk_Ratio_39to32 = (CBIOS_U32)((AudioPacketClock >> 32) & 0xFF);
    HDACPacket2RegValue.CODEC1_MUTE_EN = 1;  //for mute speaker
    HDACPacket2RegMask.Value = 0xFFFFFFFF;
    HDACPacket2RegMask.CODEC1_Audio_Packet_to_DClk_Ratio_39to32 = 0;
    HDACPacket2RegMask.CODEC1_MUTE_EN = 0;
    cbMMIOWriteReg32(pcbe, HDAC_REG_PACKET2[HDACModuleIndex], HDACPacket2RegValue.Value, HDACPacket2RegMask.Value);

    // 2. Ratio_Clk_Select, using Xclock
    HDACChStatusCtrlRegValue.Value = 0;
    HDACChStatusCtrlRegValue.Ratio_CLK_Select = 1;
    HDACChStatusCtrlRegMask.Value = 0xFFFFFFFF;
    HDACChStatusCtrlRegMask.Ratio_CLK_Select = 0;
    cbMMIOWriteReg32(pcbe, HDAC_REG_CHSTATUS_CTRL[HDACModuleIndex], HDACChStatusCtrlRegValue.Value, HDACChStatusCtrlRegMask.Value);
    //    Short Audio patch
    HDACChStatusCtrlRegValue.Value = 0;
#ifdef __LINUX__
    //This patch will cause a black screen when playing audio
    HDACChStatusCtrlRegValue.Always_Output_Audio = 0;
#else
    HDACChStatusCtrlRegValue.Always_Output_Audio = 1;
#endif
    HDACChStatusCtrlRegMask.Value = 0xFFFFFFFF;
    HDACChStatusCtrlRegMask.Always_Output_Audio = 0;
    cbMMIOWriteReg32(pcbe, HDAC_REG_CHSTATUS_CTRL[HDACModuleIndex], HDACChStatusCtrlRegValue.Value, HDACChStatusCtrlRegMask.Value);
    
    HDACReadSelRegValue.Value = 0;

    // 3.1 Sw_Strm_Fifo_Depth_Select
    HDACReadSelRegValue.SW_Strm1_FIFO_Depth_Select = 1;
    HDACReadSelRegValue.SW_Strm2_FIFO_Depth_Select = 1;
    // 3.2 Sw_Strm_Fifo_Depth
    HDACReadSelRegValue.SW_Strm1_FIFO_Depth = 3;
    HDACReadSelRegValue.SW_Strm2_FIFO_Depth = 3;
    // 3.3 Sw_Strm_Link_Position_Select
    HDACReadSelRegValue.Strm1_Link_Position_Select = 1;
    HDACReadSelRegValue.Strm2_Link_Position_Select = 1;
    if(pcbe->ChipID == CHIPID_CNE001)
    {
        HDAC3ReadSelRegValue.Value = 0;
        // 3.1 Sw_Strm_Fifo_Depth_Select
        HDAC3ReadSelRegValue.SW_Strm3_FIFO_Depth_Select = 1;
        // 3.2 Sw_Strm_Fifo_Depth
        HDAC3ReadSelRegValue.SW_Strm3_FIFO_Depth = 3;
        // 3.3 Sw_Strm_Link_Position_Select
        HDAC3ReadSelRegValue.Strm3_Link_Position_Select = 1;

        HDAC3ReadSelRegMask.Value = 0xFFFFFFFF;
        HDAC3ReadSelRegMask.SW_Strm3_FIFO_Depth_Select = 0;
        HDAC3ReadSelRegMask.SW_Strm3_FIFO_Depth = 0;
        HDAC3ReadSelRegMask.Strm3_Link_Position_Select = 0;
        cbMMIOWriteReg32(pcbe, 0x8290, HDAC3ReadSelRegValue.Value, HDAC3ReadSelRegMask.Value);
    }
    // 3.4 Wall_Clk_Select
    HDACReadSelRegValue.HDAUDIO_Wall_Clock_Select = 0;
    // 3.5 Wall_Clk_Cnt_sel
    HDACReadSelRegValue.Wal_Clk_Cnt_Sel = 1;
    // 3.6 Wall_Clk_Cnt_clock_Sel
    HDACReadSelRegValue.Wal_Clk_Cnt_Clock_Sel1 = 0;
    HDACReadSelRegValue.Wal_Clk_Cnt_Clock_Sel2 = 1;

    HDACReadSelRegMask.Value = 0;
    cbMMIOWriteReg32(pcbe, HDACReadSelRegIndex, HDACReadSelRegValue.Value, HDACReadSelRegMask.Value);

    // 4. Wall Clock Ratio / Wall clock enable
    HDACWallClkLRegValue.Value = 0;
    HDACWallClkLRegMask.Value = 0;
    cbMMIOWriteReg32(pcbe, 0x8388, HDACWallClkLRegValue.Value, HDACWallClkLRegMask.Value);
    HDACWallClkHRegValue.Value = 0;
    HDACWallClkHRegValue.Wall_clock__ratio__hi = 0;
    HDACWallClkHRegValue.Wall_clock_ratio_enable = 0;
    HDACWallClkHRegMask.Value = 0;
    cbMMIOWriteReg32(pcbe, 0x838C, HDACWallClkHRegValue.Value, HDACWallClkHRegMask.Value);
}

static CBIOS_VOID cbDIU_HDAC_SetHDAudioCapability(PCBIOS_EXTENSION_COMMON pcbe, 
                                                  CBIOS_ACTIVE_TYPE Device, 
                                                  CBIOS_MODULE_INDEX HDACModuleIndex)
{
    CBIOS_U32     AudioFmtNum = 0, i = 0;
    PCBIOS_HDMI_AUDIO_INFO   pAudioInfo = CBIOS_NULL;
    CBIOS_BOOL    bSupportStero = CBIOS_FALSE;
    PCBIOS_DEVICE_COMMON pDevCommon = cbGetDeviceCommon(&pcbe->DeviceMgr, Device);
    REG_MM82D4 HDACSupParaRegValue, HDACSupParaRegMask;
    REG_MM82E0 HDACSampRateRegValue, HDACSampRateRegMask;
    REG_MM82EC HDACConvertCapRegValue, HDACConvertCapRegMask;
    REG_MM82F0 HDACPinWidgetCapRegValue, HDACPinWidgetCapRegMask;

    pAudioInfo = pDevCommon->EdidStruct.HDAudioFormat;
    AudioFmtNum = pDevCommon->EdidStruct.HDAudioFormatNum;

    if (HDACModuleIndex >= HDAC_MODU_NUM)
    {
        cbDebugPrint((MAKE_LEVEL(DP, ERROR), "%s: Invalid HDAC module index!\n", FUNCTION_NAME));
        return;
    }

    if(!pAudioInfo)
    {
        cbDebugPrint((MAKE_LEVEL(DP, ERROR), "Invalid audio info!"));
        return;
    }

    HDACSupParaRegValue.Value = 0;
    HDACSupParaRegValue.PCM_Support = 0;
    HDACSupParaRegValue.PCM_Float32_Only_Support = 0;
    HDACSupParaRegValue.AC3_16_bit_only_Support = 0;
    HDACSupParaRegValue.SuppPowerState_D1Sup = 0;
    HDACSupParaRegValue.SuppPowerState_D2Sup = 0;

    HDACSupParaRegMask.Value = 0xFFFFFFFF;
    HDACSupParaRegMask.PCM_Support = 0;
    HDACSupParaRegMask.PCM_Float32_Only_Support = 0;
    HDACSupParaRegMask.AC3_16_bit_only_Support = 0;
    HDACSupParaRegMask.SuppPowerState_D1Sup = 0;
    HDACSupParaRegMask.SuppPowerState_D2Sup = 0;

    HDACSampRateRegValue.Value = 0;  
    HDACSampRateRegValue.R1 = 0;  //8kHz
    HDACSampRateRegValue.R2 = 0;  //11.025kHz
    HDACSampRateRegValue.R3 = 0; //16kHz
    HDACSampRateRegValue.R4 = 0; //22.05kHz
    HDACSampRateRegValue.R7 = 1; //must support 48kHz
    HDACSampRateRegValue.R12 = 0;  //384kHz
    HDACSampRateRegValue.B8 = 0;  //8bit PCM
    HDACSampRateRegValue.B32 = 0; //32bit PCM
    for(i = 0; i < AudioFmtNum; i++)
    {
        if(pAudioInfo[i].Format == CBIOS_AUDIO_FORMAT_LPCM)
        {
            HDACSupParaRegValue.PCM_Support = 1;
            if(pAudioInfo[i].SampleRate.SR_32kHz)
            {
                HDACSampRateRegValue.R5 = 1;
            }
            if(pAudioInfo[i].SampleRate.SR_44_1kHz)
            {
                HDACSampRateRegValue.R6 = 1;
            }
            if(pAudioInfo[i].SampleRate.SR_88_2kHz)
            {
                HDACSampRateRegValue.R8 = 1;
            }
            if(pAudioInfo[i].SampleRate.SR_96kHz)
            {
                HDACSampRateRegValue.R9 = 1;
            }
            if(pAudioInfo[i].SampleRate.SR_176_4kHz)
            {
                HDACSampRateRegValue.R10 = 1;
            }
            if(pAudioInfo[i].SampleRate.SR_192kHz)
            {
                HDACSampRateRegValue.R11 = 1;
            }
            if(pAudioInfo[i].BitDepth.BD_16bit)
            {
                HDACSampRateRegValue.B16 = 1;
            }
            if(pAudioInfo[i].BitDepth.BD_20bit)
            {
                HDACSampRateRegValue.B20 = 1;
            }
            if(pAudioInfo[i].BitDepth.BD_24bit)
            {
                HDACSampRateRegValue.B24 = 1;
            }
        }
        if(pAudioInfo[i].MaxChannelNum > 1)
        {
            bSupportStero = CBIOS_TRUE;
        }
    }
    HDACSampRateRegMask.Value = 0;
    HDACSampRateRegMask.Reserved_15to12 = 0xF;
    HDACSampRateRegMask.Reserved_31to21 = 0x7FF;

    HDACConvertCapRegValue.Value = 0;
    HDACConvertCapRegValue.Stereo = bSupportStero? 1 : 0;
    HDACConvertCapRegMask.Value = 0xFFFFFFFF;
    HDACConvertCapRegMask.Stereo = 0;
    if ((pcbe->SVID == 0x3A05) && (pcbe->SSID == 0x2001))//patch for cvte,hardcode max number of channels that widgets support to 2
    {
        HDACConvertCapRegValue.Chan_Count_Ext = 0;
        HDACConvertCapRegMask.Chan_Count_Ext = 0;
    }
    HDACPinWidgetCapRegValue.Value = 0;
    HDACPinWidgetCapRegValue.Stereo = bSupportStero? 1 : 0;
    HDACPinWidgetCapRegMask.Value = 0xFFFFFFFF;
    HDACPinWidgetCapRegMask.Stereo = 0;

    cbMMIOWriteReg32(pcbe, HDAC_REG_SUP_PARA[HDACModuleIndex], HDACSupParaRegValue.Value, HDACSupParaRegMask.Value);
    cbMMIOWriteReg32(pcbe, HDAC_REG_SAMP_RATE[HDACModuleIndex], HDACSampRateRegValue.Value, HDACSampRateRegMask.Value);
    cbMMIOWriteReg32(pcbe, HDAC_REG_CONVERT_CAP[HDACModuleIndex], HDACConvertCapRegValue.Value, HDACConvertCapRegMask.Value);
    cbMMIOWriteReg32(pcbe, HDAC_REG_WIDGET_AUDIO[HDACModuleIndex], HDACPinWidgetCapRegValue.Value, HDACPinWidgetCapRegMask.Value);
}

static CBIOS_VOID cbDIU_HDAC_SetChannelStatusBlock(PCBIOS_EXTENSION_COMMON pcbe,
                                                   CBIOS_MODULE_INDEX HDACModuleIndex,
                                                   CBIOS_U32 ReadOutValue)
{
    CBIOS_U32  sampling_frequency = 0;
    CBIOS_U32  bits_per_sample = (ReadOutValue & 0x70) >> 4;
    REG_MM8390 HDACChStatus1RegValue, HDACChStatus1RegMake;
    REG_MM8394 HDACChStatus2RegValue, HDACChStatus2RegMake;
    REG_MM82AC HDACChStatusCtrlRegValue, HDACChStatusCtrlRegMask;

    if(ReadOutValue & 0x4000)
    {
        sampling_frequency = 44100;
    }
    else
    {
        sampling_frequency = 48000;
    }
    sampling_frequency *= (((ReadOutValue & 0x3800) >> 11) + 1);
    sampling_frequency /= (((ReadOutValue & 0x700) >> 8) + 1);

    HDACChStatus1RegValue.Value = 0;
    HDACChStatus1RegMake.Value = 0;
    HDACChStatus2RegValue.Value = 0;
    HDACChStatus2RegMake.Value = 0;
    HDACChStatus1RegValue.Use = 0; // Consumer use of channel status block
    HDACChStatus1RegValue.Linear_PCM = (ReadOutValue & 0x80) ? 1 : 0;

    if (!HDACChStatus1RegValue.Linear_PCM)
    {
        HDACChStatus1RegValue.Category_code = 0x82; // PCM encoder
    }

    HDACChStatus1RegValue.Copyright = 0; // Sofaware for which copyright is asserted
    HDACChStatus1RegValue.Linear_PCM_mode = 0; // 2 audio channels without pre-emphasis
    HDACChStatus1RegValue.Channel_status_mode = 0; // Channel status mode 0
    HDACChStatus1RegValue.Source_number = 0; // Do not take into account
    HDACChStatus1RegValue.Channel_number = (ReadOutValue & 0xF) + 1;

    if (sampling_frequency == 22050)
    {
        HDACChStatus1RegValue.Sampling_frequency = 4;
        HDACChStatus2RegValue.Original_sample_frequency = 0xB;
    }
    else if (sampling_frequency == 44100)
    {
        HDACChStatus1RegValue.Sampling_frequency = 0;
        HDACChStatus2RegValue.Original_sample_frequency = 0xF;
    }
    else if (sampling_frequency == 88200)
    {
        HDACChStatus1RegValue.Sampling_frequency = 8;
        HDACChStatus2RegValue.Original_sample_frequency = 0x7;
    }
    else if (sampling_frequency == 176400)
    {
        HDACChStatus1RegValue.Sampling_frequency = 0xC;
        HDACChStatus2RegValue.Original_sample_frequency = 0x3;
    }
    else if (sampling_frequency == 24000)
    {
        HDACChStatus1RegValue.Sampling_frequency = 0x6;
        HDACChStatus2RegValue.Original_sample_frequency = 0x9;
    }
    else if (sampling_frequency == 48000)
    {
        HDACChStatus1RegValue.Sampling_frequency = 2;
        HDACChStatus2RegValue.Original_sample_frequency = 0xD;
    }
    else if (sampling_frequency == 96000)
    {
        HDACChStatus1RegValue.Sampling_frequency = 0xA;
        HDACChStatus2RegValue.Original_sample_frequency = 0x5;
    }
    else if (sampling_frequency == 192000)
    {
        HDACChStatus1RegValue.Sampling_frequency = 0xE;
        HDACChStatus2RegValue.Original_sample_frequency = 0x1;
    }
    else if (sampling_frequency == 32000)
    {
        HDACChStatus1RegValue.Sampling_frequency = 0x3;
        HDACChStatus2RegValue.Original_sample_frequency = 0xC;
    }
    else if (sampling_frequency == 768000)
    {
        HDACChStatus1RegValue.Sampling_frequency = 0x9;
    }
    else
    {
        HDACChStatus1RegValue.Sampling_frequency = 0x1;
        HDACChStatus2RegValue.Original_sample_frequency = 0;
    }

    HDACChStatus1RegValue.Clock_accurary = 0;

    if (bits_per_sample == 1)
    {
        HDACChStatus2RegValue.Max_word_length = 0; // 20 bits
        HDACChStatus2RegValue.Sample_word_length = 1; // 16 bits
    }
    else if (bits_per_sample == 2)
    {
        HDACChStatus2RegValue.Max_word_length = 0; // 20 bits
        HDACChStatus2RegValue.Sample_word_length = 5; // 16 bits
    }
    else if (bits_per_sample == 3)
    {
        HDACChStatus2RegValue.Max_word_length = 1; // 24 bits
        HDACChStatus2RegValue.Sample_word_length = 5; // 16 bits
    }

    cbMMIOWriteReg32(pcbe, HDAC_REG_CHSTATUS1[HDACModuleIndex], HDACChStatus1RegValue.Value, HDACChStatus1RegMake.Value);
    cbMMIOWriteReg32(pcbe, HDAC_REG_CHSTATUS2[HDACModuleIndex], HDACChStatus2RegValue.Value, HDACChStatus2RegMake.Value);

    HDACChStatusCtrlRegValue.Value = 0;
    HDACChStatusCtrlRegValue.Channel_status_control = 2;
    HDACChStatusCtrlRegMask.Value = 0xFFFFFFFF;
    HDACChStatusCtrlRegMask.Channel_status_control = 0; // Channel status provided by SW
    cbMMIOWriteReg32(pcbe, HDAC_REG_CHSTATUS_CTRL[HDACModuleIndex], HDACChStatusCtrlRegValue.Value, HDACChStatusCtrlRegMask.Value);

    cbDebugPrint((MAKE_LEVEL(DP, DEBUG), "%s: HDACModuleIndex=%d, ReadOutValue=%x, Linear_PCM=%d, Channel_number=%d, Sampling_frequency=%d, Max_word_length=%d, Sample_word_length=%d\n", FUNCTION_NAME, HDACModuleIndex, ReadOutValue, HDACChStatus1RegValue.Linear_PCM, HDACChStatus1RegValue.Channel_number, HDACChStatus1RegValue.Sampling_frequency, HDACChStatus2RegValue.Max_word_length, HDACChStatus2RegValue.Sample_word_length));
}

CBIOS_VOID cbDIU_HDAC_SetHDACodecPara(PCBIOS_VOID pvcbe, PCBIOS_HDAC_PARA pCbiosHDACPara)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    CBIOS_U32 ReadOutValue = 0, StreamFormat = 0;
    CBIOS_BOOL bEnableHDMI1 = CBIOS_FALSE;
    CBIOS_ACTIVE_TYPE  Device = (CBIOS_ACTIVE_TYPE)pCbiosHDACPara->DeviceId;
    CBIOS_MODULE_INDEX HDACModuleIndex = CBIOS_MODULE_INDEX_INVALID;
    PCBIOS_DEVICE_COMMON pDevCommon = cbGetDeviceCommon(&pcbe->DeviceMgr, Device);
    CBIOS_BOOL           bHDMIDevice = (pDevCommon->CurrentMonitorType & CBIOS_MONITOR_TYPE_HDMI);
    REG_MM82A0 HDACModeRespRegValue, HDACModeRespRegMask;
    REG_MM82AC HDACChStatusCtrlRegValue, HDACChStatusCtrlRegMask;
    REG_MM8380 HDACReadSelRegValue, HDACReadSelRegMask;

    HDACModuleIndex = cbGetModuleIndex(pcbe, Device, CBIOS_MODULE_TYPE_HDAC);

    if (HDACModuleIndex >= HDAC_MODU_NUM)
    {
        cbDebugPrint((MAKE_LEVEL(DP, ERROR), "%s: Invalid HDAC module index!\n", FUNCTION_NAME));
        return;
    }

    if (((pDevCommon->CurrentMonitorType != CBIOS_MONITOR_TYPE_HDMI)
        && (pDevCommon->CurrentMonitorType != CBIOS_MONITOR_TYPE_DP))
        || !pDevCommon->EdidStruct.Attribute.IsCEA861Audio)
    {
        cbDebugPrint((MAKE_LEVEL(DP, ERROR), "%s: Invalid monitor type 0x%x for codec#%d, or this monitor not support audio!\n",
            FUNCTION_NAME, pDevCommon->CurrentMonitorType, HDACModuleIndex));
        return;
    }

    // step 0. Set Codec_Type to HDMI1
    if(cb_ReadU32(pcbe->pAdapterContext, 0x8280) & BIT1)
    {
        bEnableHDMI1 = CBIOS_TRUE;
    }
    if(bEnableHDMI1 == CBIOS_FALSE)
    {
        cbMMIOWriteReg32(pcbe, 0x8280, 0x2, ~0x2);
    }

    //enable HDAUDIO mode
    HDACModeRespRegValue.Value = 0;
    HDACModeRespRegValue.HD_AUDIO_MODE_SELECT = 1;  
    HDACModeRespRegMask.Value = 0xFFFFFFFF;
    HDACModeRespRegMask.HD_AUDIO_MODE_SELECT = 0;
    cbMMIOWriteReg32(pcbe, HDAC_REG_MODE_RESP[HDACModuleIndex], HDACModeRespRegValue.Value, HDACModeRespRegMask.Value); 

    // step 1. read out stream format 
    HDACReadSelRegValue.Value = 0;
    HDACReadSelRegValue.Read_Out_Control_Select = 1;
    HDACReadSelRegMask.Value = 0xFFFFFFFF;
    HDACReadSelRegMask.Read_Out_Control_Select = 0;
    cbMMIOWriteReg32(pcbe, HDAC_REG_READ_SEL[HDACModuleIndex], HDACReadSelRegValue.Value, HDACReadSelRegMask.Value);
    ReadOutValue = cb_ReadU32(pcbe->pAdapterContext, HDAC_REG_READ_OUT[HDACModuleIndex]); // [15:0]

    HDACReadSelRegValue.Value = 0;
    HDACReadSelRegValue.Read_Out_Control_Select = 0;
    cbMMIOWriteReg32(pcbe, HDAC_REG_READ_SEL[HDACModuleIndex], HDACReadSelRegValue.Value, HDACReadSelRegMask.Value);

    //step 2. decode ReadOutValue by definition
    // [15] Converter Format TYPE
    // BASE. Converter Format Sample Base Rate. (PCM Format structure bit 15)
    //              0: PCM
    //              1: Non-PCM
    // [14] Converter_Format_Base
    // BASE. Converter Format Sample Base Rate. (PCM Format structure bit 14)
    //              0: 48kHz
    //              1: 44.1kHz
    // [13:11] Converter_Format_Mult
    // MULT. Converter Format Sample Base Rate Multiple. (PCM Format structure bits 13:11)
    //              000: 48kHz/ 44.1 kHz or lesss
    //              001: x2 (96kHz, 88.2kHz, 32kHz)
    //              010: x3 (144kHz)
    //              011: x4 (192kHz, 176.4kHz)
    //              100-111: Reserved
    // [10:8] Converter_Format_Div
    // DIV. Converter Format Sample Base Rate Divisor. (PCM Format structure bits 10:8)
    //              000: Divide by 1 (48kHz, 44.1 kHz)
    //              001: Divide by 2 (24kHz, 22.05kHz)
    //              010: Divide by 3 (16kHz, 32kHz)
    //              011: Divide by 4 (11.025kHz)
    //              100: Divide by 5 (9.6kHz)
    //              101: Divide by 6 (8kHz)
    //              110: Divide by 7
    //              111: Divide by 8 (6kHz)
    // [6:4] Converter Format Bits per sample.
    //              000: 8 bits
    //              001: 16 bits
    //              010: 24 bits
    //              011: 32 bits
    //              100-111: Reserved
    // [3:0] Converter Format CHAN
    //              0000: 1 Channel in each frame of the stream
    //              0001: 2 Channels in each frame of the stream
    //              0010: 3 Channels in each frame of the stream...
    //              1111: 16 Channels in each frame of the stream
    if(ReadOutValue & 0x4000)
    {
        StreamFormat = 44100;
    }
    else 
    {
        StreamFormat = 48000;
    }

    StreamFormat *= (((ReadOutValue & 0x3800) >> 11) + 1);

    StreamFormat /= (((ReadOutValue & 0x700) >> 8) + 1);

    // step 3. Audio packet clock, Wall clock ratio
    cbDIU_HDAC_SetHDACSettings(pcbe, HDACModuleIndex, StreamFormat);
    cbDIU_HDAC_SetChannelStatusBlock(pcbe, HDACModuleIndex, ReadOutValue);

    // step 4. fill CTS/N for HDMI/MHL, Maud/Naud for DP
    if(bHDMIDevice)
    {
        CBIOS_MODULE_INDEX HDMIModuleIndex = cbGetModuleIndex(pcbe, Device, CBIOS_MODULE_TYPE_HDMI);

        cbDIU_HDMI_SetCTSN(pcbe, pDevCommon, HDMIModuleIndex, HDACModuleIndex, StreamFormat);
    }
#if DP_MONITOR_SUPPORT
    else
    {
        PCBIOS_DP_MONITOR_CONTEXT pDPMonitorContext = cbGetDPMonitorContext(pcbe, pDevCommon);
        CBIOS_MODULE_INDEX DPModuleIndex = cbGetModuleIndex(pcbe, Device, CBIOS_MODULE_TYPE_DP);
        CBIOS_U32  LinkSpeed = pDPMonitorContext->LinkPassSpeed;

        if(!LinkSpeed)
        {
            if(pDPMonitorContext->SinkMaxLinkSpeed)
            {
                LinkSpeed = pDPMonitorContext->SinkMaxLinkSpeed;
            }
            else
            {
                LinkSpeed = CBIOS_DP_LINK_SPEED_5400Mbps;
            }
        }

        cbDIU_DP_SetMaudNaud(pcbe, DPModuleIndex, LinkSpeed, StreamFormat);
    }
#endif

    //step 5. set to correct source
    HDACChStatusCtrlRegValue.Value = 0;
    HDACChStatusCtrlRegMask.Value = 0xFFFFFFFF;
    HDACChStatusCtrlRegMask.Codec_Type = 0;

    if(HDACModuleIndex == CBIOS_MODULE_INDEX1)
    {
        if(bHDMIDevice)
        {
            //codec_1 is used for HDMI1
            HDACChStatusCtrlRegValue.Codec_Type = 0;
        }
        else
        {
            //codec_2 is used for DP1
            HDACChStatusCtrlRegValue.Codec_Type = 1;
        }
    }
    else if(HDACModuleIndex == CBIOS_MODULE_INDEX2)
    {
        if(bHDMIDevice)
        {
            //codec_2 is used for HDMI2
            HDACChStatusCtrlRegValue.Codec_Type = 3;
        }
        else
        {
            //codec_2 is used for DP2
            HDACChStatusCtrlRegValue.Codec_Type = 2;
        }
    }
    else if(HDACModuleIndex == CBIOS_MODULE_INDEX3)
    {
        if(bHDMIDevice)
        {
            //codec_3 is used for HDMI3
            HDACChStatusCtrlRegValue.Codec_Type = 4;
        }
        else
        {
            //codec_3 is used for DP3
            HDACChStatusCtrlRegValue.Codec_Type = 5;
        }
    }
    cbMMIOWriteReg32(pcbe, HDAC_REG_CHSTATUS_CTRL[HDACModuleIndex], HDACChStatusCtrlRegValue.Value, HDACChStatusCtrlRegMask.Value);

    cbDIU_HDAC_SetHDAudioCapability(pcbe, Device, HDACModuleIndex);

    // step 6. disable HDMI1
    if(bEnableHDMI1 == CBIOS_FALSE)
    {
        cbMMIOWriteReg32(pcbe, 0x8280, 0x0, ~0x2);
    }
}

CBIOS_VOID cbDIU_HDAC_SetStatus(PCBIOS_VOID pvcbe)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;

    REG_MM336B4 RegMM336B4Value, RegMM336B4Mask;

    RegMM336B4Value.Value = 0;
    RegMM336B4Value.HW_CORB_RST_SEL = 1;
    RegMM336B4Mask.Value = 0xFFFFFFFF;
    RegMM336B4Mask.HW_CORB_RST_SEL = 0;
    cbMMIOWriteReg32(pcbe, 0x336B4, RegMM336B4Value.Value, RegMM336B4Mask.Value);
}

#if 0
CBIOS_VOID cbDIU_HDAC_SetStatus(PCBIOS_VOID pvcbe)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    CBIOS_U8 index = 0, NumofCodec = 0;
    CBIOS_BOOL bDisableCodec[3] = {0};
    REG_MM82A0 HDACModeRespRegValue, HDACModeRespRegMask;

    HDACModeRespRegValue.Value = 0;
    HDACModeRespRegMask.Value = 0xFFFFFFFF;
    HDACModeRespRegMask.HDAUDIO_CODEC1_Enable = 0;

    if(pcbe->ChipID == CHIPID_CHX004)
    {
        NumofCodec = HDAC_MODU_NUM - 1;
        bDisableCodec[0] = (pcbe->FeatureSwitch.IsDisableCodec1) ? CBIOS_TRUE : CBIOS_FALSE;
        bDisableCodec[1] = (pcbe->FeatureSwitch.IsDisableCodec2) ? CBIOS_TRUE : CBIOS_FALSE;
    }
    else if(pcbe->ChipID == CHIPID_CNE001)
    {
        NumofCodec = HDAC_MODU_NUM;
        bDisableCodec[0] = (pcbe->FeatureSwitch.IsDisableCodec1) ? CBIOS_TRUE : CBIOS_FALSE;
        bDisableCodec[1] = (pcbe->FeatureSwitch.IsDisableCodec2) ? CBIOS_TRUE : CBIOS_FALSE;
        bDisableCodec[2] = 0; // (pcbe->FeatureSwitch.IsDisableCodec3) ? CBIOS_TRUE : CBIOS_FALSE;
    }

    for(index = 0; index < NumofCodec; index++)
    {
        cbMMIOWriteReg32(pcbe, HDAC_REG_WIDGET_AUDIO[index], 0x00400381, 0);
        cbMMIOWriteReg32(pcbe, HDAC_REG_WIDGET_PIN[index], 0x00000094, 0);

        HDACModeRespRegValue.HDAUDIO_CODEC1_Enable = (bDisableCodec[index])? 0 : 1;
        cbMMIOWriteReg32(pcbe, HDAC_REG_MODE_RESP[index], HDACModeRespRegValue.Value, HDACModeRespRegMask.Value);

        cbMMIOWriteReg32(pcbe, HDAC_REG_CTRL_WRITE[index], 0x00000080, 0xFFFFFF7F);
        cbMMIOWriteReg32(pcbe, HDAC_REG_CTRL_WRITE[index], 0x00000000, 0xFFFFFF7F);
    }
}
#endif

CBIOS_U32 cbDIU_HDAC_GetChannelNums(PCBIOS_VOID pvcbe, CBIOS_MODULE_INDEX HDACModuleIndex)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    CBIOS_U32  ReadOutValue = 0, NumofChannels = 0;
    REG_MM82A0 HDACModeRespRegValue, HDACModeRespRegMask;
    REG_MM82AC HDACChStatusCtrlRegValue, HDACChStatusCtrlRegMask;
    REG_MM8380 HDACReadSelRegValue, HDACReadSelRegMask;

    if (HDACModuleIndex >= HDAC_MODU_NUM)
    {
        cbDebugPrint((MAKE_LEVEL(DP, ERROR), "%s: invalid HDAC module index!\n", FUNCTION_NAME));
        return 0;
    }

    //enable HDAUDIO mode
    HDACModeRespRegValue.Value = 0;
    HDACModeRespRegValue.HD_AUDIO_MODE_SELECT = 1;
    HDACModeRespRegMask.Value = 0xFFFFFFFF;
    HDACModeRespRegMask.HD_AUDIO_MODE_SELECT = 0;
    cbMMIOWriteReg32(pcbe, HDAC_REG_MODE_RESP[HDACModuleIndex], HDACModeRespRegValue.Value, HDACModeRespRegMask.Value);

    HDACChStatusCtrlRegValue.Value = 0;
    HDACChStatusCtrlRegMask.Value = 0xFFFFFFFF;
    HDACChStatusCtrlRegMask.Codec_Type = 0;
    if(HDACModuleIndex == CBIOS_MODULE_INDEX1)
    {
        HDACChStatusCtrlRegValue.Codec_Type = 0;
    }
    else if(HDACModuleIndex == CBIOS_MODULE_INDEX2)
    {
        HDACChStatusCtrlRegValue.Codec_Type = 3;
    }
    else if(HDACModuleIndex == CBIOS_MODULE_INDEX3)
    {
        HDACChStatusCtrlRegValue.Codec_Type = 4;
    }
    cbMMIOWriteReg32(pcbe, HDAC_REG_CHSTATUS_CTRL[HDACModuleIndex], HDACChStatusCtrlRegValue.Value, HDACChStatusCtrlRegMask.Value);

    // step 1. read out stream format
    HDACReadSelRegValue.Value = 0;
    HDACReadSelRegValue.Read_Out_Control_Select = 1;
    HDACReadSelRegMask.Value = 0xFFFFFFFF;
    HDACReadSelRegMask.Read_Out_Control_Select = 0;
    cbMMIOWriteReg32(pcbe, HDAC_REG_READ_SEL[HDACModuleIndex], HDACReadSelRegValue.Value, HDACReadSelRegMask.Value);
    
    ReadOutValue = cb_ReadU32(pcbe->pAdapterContext, HDAC_REG_READ_OUT[HDACModuleIndex]); // [15:0]

    HDACReadSelRegValue.Value = 0;
    HDACReadSelRegValue.Read_Out_Control_Select = 0;
    cbMMIOWriteReg32(pcbe, HDAC_REG_READ_SEL[HDACModuleIndex], HDACReadSelRegValue.Value, HDACReadSelRegMask.Value);

    //step 2. decode ReadOutValue by definition
    // [3:0] Number of channels(CHAN)
    NumofChannels = ReadOutValue & 0xF;

    return NumofChannels;
}

#if 0

static CBIOS_VOID cbDIU_HDAC_DevicesSwitchPatched(PCBIOS_VOID pvcbe, PCBIOS_HDAC_PARA pCbiosHDACPara)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    CBIOS_ACTIVE_TYPE       Device = (CBIOS_ACTIVE_TYPE)pCbiosHDACPara->DeviceId;
    PCBIOS_DEVICE_COMMON    pDevCommon1 = cbGetDeviceCommon(&pcbe->DeviceMgr, CBIOS_TYPE_DP1);
    PCBIOS_DEVICE_COMMON    pDevCommon2 = cbGetDeviceCommon(&pcbe->DeviceMgr, CBIOS_TYPE_DP2);
    CBIOS_BOOL              bHDMIDevice1 = (pDevCommon1->CurrentMonitorType & CBIOS_MONITOR_TYPE_HDMI);
    CBIOS_BOOL              bHDMIDevice2 = (pDevCommon2->CurrentMonitorType & CBIOS_MONITOR_TYPE_HDMI);
    REG_MM82AC              HDACChStatusCtrlRegValue, HDACChStatusCtrlRegMask;

    if((!pCbiosHDACPara->bPresent) && (Device == CBIOS_TYPE_DP1)) //DP1 is disconnected and DP2 is connected.
    {
        if(pDevCommon2->PowerState == CBIOS_PM_ON)
        {
            cb_WriteU32(pcbe->pAdapterContext, HDAC_REG_CHSTATUS_CTRL[CBIOS_MODULE_INDEX1],
                (cb_ReadU32(pcbe->pAdapterContext, HDAC_REG_CHSTATUS_CTRL[CBIOS_MODULE_INDEX1]) & 0xFFFF9FFF) | (cb_ReadU32(pcbe->pAdapterContext, HDAC_REG_CHSTATUS_CTRL[CBIOS_MODULE_INDEX2]) & 0x00006000));
        }
    }
    else if((!pCbiosHDACPara->bPresent) && (Device == CBIOS_TYPE_DP2)) //DP2 is disconnected and DP1 is connected.
    {
        if(pDevCommon1->PowerState == CBIOS_PM_ON)
        {
            cb_WriteU32(pcbe->pAdapterContext, HDAC_REG_CHSTATUS_CTRL[CBIOS_MODULE_INDEX2],
                (cb_ReadU32(pcbe->pAdapterContext, HDAC_REG_CHSTATUS_CTRL[CBIOS_MODULE_INDEX2]) & 0xFFFF9FFF) | (cb_ReadU32(pcbe->pAdapterContext, HDAC_REG_CHSTATUS_CTRL[CBIOS_MODULE_INDEX1]) & 0x00006000));
        }
    }
    else if(pCbiosHDACPara->bPresent && (Device == CBIOS_TYPE_DP1)) //DP1 is connected and DP2 is disconnected.
    {
        // set codec1 source
        HDACChStatusCtrlRegValue.Value = 0;
        if(bHDMIDevice1)
        {
            HDACChStatusCtrlRegValue.Codec_Type = 0; //codec_1 is used for HDMI1
        }
        else
        {
            HDACChStatusCtrlRegValue.Codec_Type = 1; //codec_2 is used for DP1
        }
        HDACChStatusCtrlRegMask.Value = 0xFFFFFFFF;
        HDACChStatusCtrlRegMask.Codec_Type = 0;
        cbMMIOWriteReg32(pcbe, HDAC_REG_CHSTATUS_CTRL[CBIOS_MODULE_INDEX1], HDACChStatusCtrlRegValue.Value, HDACChStatusCtrlRegMask.Value);

        if(pDevCommon2->PowerState == CBIOS_PM_OFF)
        {
            cb_WriteU32(pcbe->pAdapterContext, HDAC_REG_CHSTATUS_CTRL[CBIOS_MODULE_INDEX2],
                (cb_ReadU32(pcbe->pAdapterContext, HDAC_REG_CHSTATUS_CTRL[CBIOS_MODULE_INDEX2]) & 0xFFFF9FFF) | (cb_ReadU32(pcbe->pAdapterContext, HDAC_REG_CHSTATUS_CTRL[CBIOS_MODULE_INDEX1]) & 0x00006000));
        }
    }
    else if(pCbiosHDACPara->bPresent && (Device == CBIOS_TYPE_DP2)) //DP2 is connected and DP1 is disconnected.
    {
        // set codec2 source
        HDACChStatusCtrlRegValue.Value = 0;
        if(bHDMIDevice2)
        {
            HDACChStatusCtrlRegValue.Codec_Type = 3; //codec_2 is used for HDMI2
        }
        else
        {
            HDACChStatusCtrlRegValue.Codec_Type = 2; //codec_2 is used for DP2
        }
        HDACChStatusCtrlRegMask.Value = 0xFFFFFFFF;
        HDACChStatusCtrlRegMask.Codec_Type = 0;
        cbMMIOWriteReg32(pcbe, HDAC_REG_CHSTATUS_CTRL[CBIOS_MODULE_INDEX2], HDACChStatusCtrlRegValue.Value, HDACChStatusCtrlRegMask.Value);

        if(pDevCommon1->PowerState == CBIOS_PM_OFF)
        {
            cb_WriteU32(pcbe->pAdapterContext, HDAC_REG_CHSTATUS_CTRL[CBIOS_MODULE_INDEX1],
                (cb_ReadU32(pcbe->pAdapterContext, HDAC_REG_CHSTATUS_CTRL[CBIOS_MODULE_INDEX1]) & 0xFFFF9FFF) | (cb_ReadU32(pcbe->pAdapterContext, HDAC_REG_CHSTATUS_CTRL[CBIOS_MODULE_INDEX2]) & 0x00006000));
        }
    }
}

#endif

static CBIOS_VOID cbDIU_HDAC_UpdateEldStatus(PCBIOS_VOID pvcbe, PCBIOS_HDAC_PARA pCbiosHDACPara)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    CBIOS_U32 PinSense = 0, UnsolResponse = 0, UnsolCtl = 0, ulTemp = 0;
    CBIOS_MODULE_INDEX HDACModuleIndex = CBIOS_MODULE_INDEX_INVALID;
    REG_MM82A0 HDACModeRespRegValue, HDACModeRespRegMask;
    REG_MM8308 HDACPinSenseRegValue, HDACPinSenseRegMask;
    REG_MM837C HDACCtrlWriteRegValue, HDACCtrlWriteRegMask;
    REG_MM8380 HDACReadSelRegValue, HDACReadSelRegMask;

    if((pCbiosHDACPara->bPresent == CBIOS_TRUE)
        && (pCbiosHDACPara->bEldValid== CBIOS_TRUE))
    {
        PinSense |= 0x80000000;
        UnsolResponse |= 0x1;
        PinSense |= 0x40000000;
        UnsolResponse |= 0x2;
    }
    else if((pCbiosHDACPara->bPresent == CBIOS_TRUE)
        && (pCbiosHDACPara->bEldValid == CBIOS_FALSE))
    {
        PinSense |= 0x80000000;
        UnsolResponse |= 0x1;
        PinSense |= 0x00000000;
        UnsolResponse |= 0x2;
    }

    HDACModuleIndex = cbGetModuleIndex(pcbe, (CBIOS_ACTIVE_TYPE)pCbiosHDACPara->DeviceId, CBIOS_MODULE_TYPE_HDAC);
    if (HDACModuleIndex >= HDAC_MODU_NUM)
    {
        cbDebugPrint((MAKE_LEVEL(DP, ERROR), "%s: invalid HDAC module index!\n", FUNCTION_NAME));
        return;
    }
    
    //load PinSense
    HDACPinSenseRegValue.Value = PinSense;
    HDACPinSenseRegMask.Value = 0;
    cbMMIOWriteReg32(pcbe, HDAC_REG_PIN_SENSE[HDACModuleIndex], HDACPinSenseRegValue.Value, HDACPinSenseRegMask.Value);
    HDACCtrlWriteRegValue.Value = 0;
    HDACCtrlWriteRegValue.Load_Pinwidget1_PinSense = 1;
    HDACCtrlWriteRegMask.Value = 0xFFFFFFFF;
    HDACCtrlWriteRegMask.Load_Pinwidget1_PinSense = 0;
    cbMMIOWriteReg32(pcbe, HDAC_REG_CTRL_WRITE[HDACModuleIndex], HDACCtrlWriteRegValue.Value, HDACCtrlWriteRegMask.Value);

    cb_DelayMicroSeconds(20);

    HDACCtrlWriteRegValue.Value = 0;
    HDACCtrlWriteRegValue.Load_Pinwidget1_PinSense = 0;
    HDACCtrlWriteRegMask.Value = 0xFFFFFFFF;
    HDACCtrlWriteRegMask.Load_Pinwidget1_PinSense = 0;
    cbMMIOWriteReg32(pcbe, HDAC_REG_CTRL_WRITE[HDACModuleIndex], HDACCtrlWriteRegValue.Value, HDACCtrlWriteRegMask.Value);

    //read out
    HDACReadSelRegValue.Value = 0;
    HDACReadSelRegValue.Read_Out_Control_Select = 0x06;
    HDACReadSelRegMask.Value = 0xFFFFFFFF;
    HDACReadSelRegMask.Read_Out_Control_Select = 0;
    cbMMIOWriteReg32(pcbe, HDAC_REG_READ_SEL[HDACModuleIndex], HDACReadSelRegValue.Value, HDACReadSelRegMask.Value);
    ulTemp = cb_ReadU32(pcbe->pAdapterContext, HDAC_REG_READ_OUT[HDACModuleIndex]);
    if(PinSense == ulTemp)
    {
        //if(bSendUnsol)
        {
            HDACReadSelRegValue.Value = 0;
            HDACReadSelRegValue.Read_Out_Control_Select = 0x04;
            HDACReadSelRegMask.Value = 0xFFFFFFFF;
            HDACReadSelRegMask.Read_Out_Control_Select = 0;
            cbMMIOWriteReg32(pcbe, HDAC_REG_READ_SEL[HDACModuleIndex], HDACReadSelRegValue.Value, HDACReadSelRegMask.Value);
            UnsolCtl = cb_ReadU32(pcbe->pAdapterContext, HDAC_REG_READ_OUT[HDACModuleIndex]);
            if(UnsolCtl & 0x80)
            {    // Unsolicited response enabled
                UnsolResponse |= (UnsolCtl & 0x3f) << 26;
                cb_WriteU32(pcbe->pAdapterContext, HDAC_REG_SW_RESP[HDACModuleIndex], UnsolResponse);

                HDACModeRespRegValue.Value = 0;
                HDACModeRespRegMask.Value = 0xFFFFFFFF;
                HDACModeRespRegValue.Send_UNSOLRESP = 1;
                HDACModeRespRegMask.Send_UNSOLRESP = 0;
                // send it 3 times to make sure it be sent out.
                cbMMIOWriteReg32(pcbe, HDAC_REG_MODE_RESP[HDACModuleIndex], HDACModeRespRegValue.Value, HDACModeRespRegMask.Value);
                cb_DelayMicroSeconds(20);
                cbMMIOWriteReg32(pcbe, HDAC_REG_MODE_RESP[HDACModuleIndex], HDACModeRespRegValue.Value, HDACModeRespRegMask.Value);
                cb_DelayMicroSeconds(20);
                cbMMIOWriteReg32(pcbe, HDAC_REG_MODE_RESP[HDACModuleIndex], HDACModeRespRegValue.Value, HDACModeRespRegMask.Value);
            }
        }
    }
    else
    {
        cbDebugPrint((MAKE_LEVEL(HDMI, ERROR), "%s: Write Codec PinSense Failed!\n", FUNCTION_NAME));
    }

    HDACReadSelRegValue.Value = 0;
    HDACReadSelRegValue.Read_Out_Control_Select = 0;
    HDACReadSelRegMask.Value = 0xFFFFFFFF;
    HDACReadSelRegMask.Read_Out_Control_Select = 0;
    cbMMIOWriteReg32(pcbe, HDAC_REG_READ_SEL[HDACModuleIndex], HDACReadSelRegValue.Value, HDACReadSelRegMask.Value);
}


static CBIOS_VOID cbDIU_HDAC_WriteFIFO(PCBIOS_VOID pvcbe, CBIOS_ACTIVE_TYPE Device, PCBIOS_ELD_MEM_STRUCT pEld)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    CBIOS_U8  SR47Value = 0, Data = 0, LookupTable = 0;
    CBIOS_U32 index = 0, eldIndex = 0;
    CBIOS_MODULE_INDEX HDACModuleIndex = CBIOS_MODULE_INDEX_INVALID;
    REG_MM82AC HDACChStatusCtrlRegValue, HDACChStatusCtrlRegMask;
    REG_MM834C HDACEldBufRegValue, HDACEldBufRegMask;
    REG_SR47   RegSR47Value, RegSR47Mask;

    HDACModuleIndex = cbGetModuleIndex(pcbe, Device, CBIOS_MODULE_TYPE_HDAC);

    if (HDACModuleIndex >= HDAC_MODU_NUM)
    {
        cbDebugPrint((MAKE_LEVEL(DP, ERROR), "%s: invalid HDAC module index!\n", FUNCTION_NAME));
        return;
    }

    if(HDACModuleIndex == CBIOS_MODULE_INDEX1)    // CODEC1
    {
        LookupTable = 4;
    }
    else if(HDACModuleIndex == CBIOS_MODULE_INDEX2)    // CODEC2
    {
        LookupTable = 5;
    }
    else if(HDACModuleIndex == CBIOS_MODULE_INDEX3)    // CODEC3
    {
        LookupTable = 0xD;
    }

    HDACChStatusCtrlRegValue.Value = 0;
    HDACChStatusCtrlRegValue.Set_ELD_Default = 1;
    HDACChStatusCtrlRegValue.ELD_Use_LUT = 1;
    HDACChStatusCtrlRegMask.Value = 0xFFFFFFFF;
    HDACChStatusCtrlRegMask.Set_ELD_Default = 0;
    HDACChStatusCtrlRegMask.ELD_Use_LUT = 0;
    cbMMIOWriteReg32(pcbe, HDAC_REG_CHSTATUS_CTRL[HDACModuleIndex], HDACChStatusCtrlRegValue.Value, HDACChStatusCtrlRegMask.Value);

    HDACEldBufRegValue.Value = 0;
    HDACEldBufRegValue.Byte_Offset_into_ELD_memory = 0;
    HDACEldBufRegMask.Value = 0xFFFFFFFF;
    HDACEldBufRegMask.Byte_Offset_into_ELD_memory = 0;
    cbMMIOWriteReg32(pcbe, HDAC_REG_ELD_BUF[HDACModuleIndex], HDACEldBufRegValue.Value, HDACEldBufRegMask.Value);
    HDACEldBufRegValue.Value = 0;
    HDACEldBufRegValue.ELD_Buffer_Size = (pEld->Size & 0xff);
    HDACEldBufRegMask.Value = 0xFFFFFFFF;
    HDACEldBufRegMask.ELD_Buffer_Size = 0;
    cbMMIOWriteReg32(pcbe, HDAC_REG_ELD_BUF[HDACModuleIndex], HDACEldBufRegValue.Value, HDACEldBufRegMask.Value);

    SR47Value = cbMMIOReadReg(pcbe, SR_47);

    //select eld lookup table
    RegSR47Value.Value = 0;
    RegSR47Value.CLUT_Select = LookupTable;
    RegSR47Mask.Value = 0xFF;
    RegSR47Mask.CLUT_Select = 0;
    cbMMIOWriteReg(pcbe, SR_47, RegSR47Value.Value, RegSR47Mask.Value);

    cb_WriteU8(pcbe->pAdapterContext, 0x83C8, 0);

    for(index = 0; index < ((pEld->Size + 31) / 32); index++)
    {
        for(eldIndex = (index + 1) * 32; eldIndex > 0; eldIndex--)
        {
            if(eldIndex > pEld->Size)
                Data = 0;
            else
                Data = pEld->Data[eldIndex - 1];
            cb_WriteU8(pcbe->pAdapterContext, 0x83C9, Data);
        }
    }

    //clear ELD_Use_LUT as DP3/4 not bind to codec, so it can not send out info frame
    HDACChStatusCtrlRegValue.Value = 0;
    HDACChStatusCtrlRegValue.ELD_Use_LUT = 0;
    HDACChStatusCtrlRegMask.Value = 0xFFFFFFFF;
    HDACChStatusCtrlRegMask.ELD_Use_LUT = 0;
    cbMMIOWriteReg32(pcbe, HDAC_REG_CHSTATUS_CTRL[HDACModuleIndex], HDACChStatusCtrlRegValue.Value, HDACChStatusCtrlRegMask.Value);

    //restore SR47
    RegSR47Value.Value = SR47Value;
    RegSR47Mask.Value = 0;
    cbMMIOWriteReg(pcbe, SR_47, RegSR47Value.Value, RegSR47Mask.Value);
}


static CBIOS_VOID cbDIU_HDAC_UpdateEldMemory(PCBIOS_EXTENSION_COMMON pcbe, PCBIOS_HDAC_PARA pCbiosHDACPara)
{
    PCBIOS_ELD_MEM_STRUCT pEld;

    pEld = cb_AllocateNonpagedPool(sizeof(CBIOS_ELD_MEM_STRUCT));
    if (pEld == CBIOS_NULL)
    {
        cbDebugPrint((MAKE_LEVEL(HDMI, ERROR), "%s: pEld allocate error.\n", FUNCTION_NAME));
        return;
    }

    /* Get ELD data from the sink device base EDID */
    cbGetDeviceELD(pcbe, pCbiosHDACPara->DeviceId, pEld);

    /* using PortID to group monitors embedded audio */
    if ((pCbiosHDACPara->ManufacturerName == 0) && (pCbiosHDACPara->ProductCode == 0))
    {
        /* for compatible */
        pEld->ELD_Data.Port_ID.HighPart = 0; /* hAdapter->AdapterLUID.HighPart; */
        pEld->ELD_Data.Port_ID.LowPart = 0; /* hAdapter->AdapterLUID.LowPart */
    }
    else
    {
        /* for new DDI pass one param like 'ELD Info' to cbios to overwrite hardware ELD PortID.*/
        cb_memcpy(&(pEld->ELD_Data.Port_ID), &pCbiosHDACPara->PortId, 8);
        cb_memcpy(&(pEld->ELD_Data.ManufactureName), &pCbiosHDACPara->ManufacturerName, 2);
        cb_memcpy(&(pEld->ELD_Data.ProductCode), &pCbiosHDACPara->ProductCode, 2);
    }

    cbDIU_HDAC_WriteFIFO(pcbe, (CBIOS_ACTIVE_TYPE)pCbiosHDACPara->DeviceId, pEld);
    cb_FreePool(pEld);
}

CBIOS_VOID cbDIU_HDAC_DisableAudioSamples(PCBIOS_VOID pvcbe, PCBIOS_HDAC_PARA pCbiosHDACPara)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    CBIOS_ACTIVE_TYPE  Device = (CBIOS_ACTIVE_TYPE)pCbiosHDACPara->DeviceId;
    PCBIOS_DEVICE_COMMON pDevCommon = cbGetDeviceCommon(&pcbe->DeviceMgr, Device);
    CBIOS_BOOL           bHDMIDevice = (pDevCommon->CurrentMonitorType & CBIOS_MONITOR_TYPE_HDMI);
    CBIOS_MODULE_INDEX HDACModuleIndex = CBIOS_MODULE_INDEX_INVALID;
    REG_MM82AC HDACChStatusCtrlRegValue, HDACChStatusCtrlRegMask;
    REG_MM829C HDACPacket2RegValue, HDACPacket2RegMask;

    HDACModuleIndex = cbGetModuleIndex(pcbe, Device, CBIOS_MODULE_TYPE_HDAC);
    if (HDACModuleIndex >= HDAC_MODU_NUM)
    {
        cbDebugPrint((MAKE_LEVEL(DP, ERROR), "%s: Invalid HDAC module index!\n", FUNCTION_NAME));
        return;
    }

    HDACChStatusCtrlRegValue.Value = 0;
    HDACChStatusCtrlRegValue.Always_Output_Audio = 0;
    HDACChStatusCtrlRegMask.Value = 0xFFFFFFFF;
    HDACChStatusCtrlRegMask.Always_Output_Audio = 0;
    cbMMIOWriteReg32(pcbe, HDAC_REG_CHSTATUS_CTRL[HDACModuleIndex], HDACChStatusCtrlRegValue.Value, HDACChStatusCtrlRegMask.Value);

    if(bHDMIDevice)
    {
        HDACPacket2RegValue.Value = 0;
        HDACPacket2RegValue.CODEC1_ACR_ENABLE = 0;
        HDACPacket2RegMask.Value = 0xFFFFFFFF;
        HDACPacket2RegMask.CODEC1_ACR_ENABLE = 0;
        cbMMIOWriteReg32(pcbe, HDAC_REG_PACKET2[HDACModuleIndex], HDACPacket2RegValue.Value, HDACPacket2RegMask.Value);
    }
}

CBIOS_VOID cbDIU_HDAC_SetConnectStatus(PCBIOS_VOID pvcbe, PCBIOS_HDAC_PARA pCbiosHDACPara)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;

    //cbDIU_HDAC_DevicesSwitchPatched(pcbe, pCbiosHDACPara);
    /*
    *1,use cbios macro 'SIZEOF_STRUCT_TILL_MEMBER'to avoid overborder
    *2, check the value of BIT31, if it equal TRUE,and the flag of bPresent=TRUE;
    *   we should always update ELD Info first.
    *3, set BIT31, and BIT30, according to flags 'bPresent & bEldValid'
    */
    if(pCbiosHDACPara->bPresent == CBIOS_TRUE)
    {
        cbDIU_HDAC_UpdateEldMemory(pcbe, pCbiosHDACPara);
    }

    cbDIU_HDAC_UpdateEldStatus(pcbe, pCbiosHDACPara);

    if(pCbiosHDACPara->bPresent == CBIOS_FALSE)
    {
        cbDIU_HDAC_DisableAudioSamples(pcbe, pCbiosHDACPara);
    }
}

