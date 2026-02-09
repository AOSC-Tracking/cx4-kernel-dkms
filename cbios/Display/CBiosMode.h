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
** CBios mode module interface prototype and parameter definition.
**
** NOTE:
** The hw dependent function or structure SHOULD NOT be added to this file.
******************************************************************************/

#ifndef _CBIOS_MODE_H_
#define _CBIOS_MODE_H_

#define  CBIOS_MODE_BUFFER_INCRE         8

typedef enum _CBIOS_MODE_TIMING_TYPE{  //can extend to support 14 different types  
    CBIOS_MODE_NONEDID_TIMING = 0x0000,    //The mode is not from EDID
    CBIOS_MODE_EST_TIMING = 0x0001,
    CBIOS_MODE_STD_TIMING = 0x0002,
    CBIOS_MODE_DTL_TIMING = 0x0004,
    CBIOS_MODE_SVD_TIMING = 0x0008,        //Detailed timing descriptor
    CBIOS_MODE_DTD_TIMING = 0x0010,        //CBIOS_S16 video descriptor
    CBIOS_MODE_DISP_ID_TIMING = 0x0020
}CBIOS_MODE_TIMING_TYPE, *PCBIOS_MODE_TIMING_TYPE;

typedef enum _CBIOS_MODE_ASPECTRATIO{
    CBIOS_MODE_AR1B1 = 0x00,
    CBIOS_MODE_AR5B4,
    CBIOS_MODE_AR4B3,
    CBIOS_MODE_AR15B9,
    CBIOS_MODE_AR16B9,
    CBIOS_MODE_AR16B10,
    CBIOS_MODE_AR64B27,
    CBIOS_MODE_AR256B135,
    CBIOS_MODE_AR_NOT_DEFINED
}CBIOS_MODE_ASPECTRATIO;

typedef struct _CBIOS_MODE_INFO {
    CBIOS_U16 XResolution;
    CBIOS_U16 YResolution;
    CBIOS_U16 Refreshrate;
    CBIOS_BOOL Valid;
}CBIOS_MODE_INFO, *PCBIOS_MODE_INFO;

typedef struct _CBIOS_DETAILED_TIMING_INFO {
    CBIOS_U16 XResolution;
    CBIOS_U16 YResolution;
    CBIOS_U16 HBlank;
    CBIOS_U16 HSyncOffset;
    CBIOS_U16 HSyncPulseWidth;
    CBIOS_U16 VBlank;
    CBIOS_U16 VSyncOffset;
    CBIOS_U16 VSyncPulseWidth;
    CBIOS_U16 HImageSize;
    CBIOS_U16 VImageSize;
    CBIOS_U32 PixelClock;
    CBIOS_U16 Refreshrate;
    CBIOS_BOOL Valid;
    CBIOS_U8 VSync;
    CBIOS_U8 HSync;
    union{
        CBIOS_U8 AspectRatio;    /* 0 means default, 1 means 4:3, 2 means 16:9 */
        CBIOS_MODE_ASPECTRATIO DIDAspectRatio; // DisplayID AspectRatio Flag
    };
    union
    {
        CBIOS_U32 TimingFlags; /* the TimingType flags can be used for patch, if IsPreferMode=1, need check the first EDID detailed timing is invalid */
        struct
        {
            CBIOS_U32   IsDetailTiming          :1; /*    bit0 = 1, the timing is from base EDID Detailed timing block */
            CBIOS_U32   IsCEATiming             :1; /*    bit1 = 1, the timing is from CEA Detailed timing Descriptor */
            CBIOS_U32   IsDIDTiming             :1; /*    bit2 = 1, the timing is from DisplayID Detailed Timing Descriptor */
            CBIOS_U32   RsvdTimingType          :5; /*    bit3:7  for future timing types use   */
            CBIOS_U32   IsSupportMono           :1; /* bit8 = 1, the timing is support 2D display */
            CBIOS_U32   IsSupportStereo         :1; /* bit9 = 1, the timing is support 3D display */
            CBIOS_U32   IsNativeMode            :1; /* bit10 = 1, the timing is a native mode's timing */
            CBIOS_U32   IsPreferMode            :1; /* bit11 = 1, the timing can be used for prefer mode */
            CBIOS_U32   RsvdTimingAttribute     :4; /* bit12:15  for future timing attributes use  */
            CBIOS_U32   IsInterLaced            :1; /* bit16 = 1, the timing is an interlated mode  */
            CBIOS_U32   IsSupportYCbCr420       :1; /* bit17 = 1, the timing is support ycbcr420  */
            CBIOS_U32   RsvdTimingFlags         :14;/* Other bits reserved for future use */
        };
    };
}CBIOS_DETAILED_TIMING_INFO, *PCBIOS_DETAILED_TIMING_INFO;

typedef struct _CBios_Equivalent_Device_Mode
{
    CBIOS_U32 XRes;
    CBIOS_U32 YRes;
    CBIOS_U32 RefRateRange[2];
    CBIOS_U32 RefRateToAdd;
}CBiosEquivalentDeviceMode, PCBiosEquivalentDeviceMode;

typedef struct _CBIOS_MODE_FILTER_PARA{
    CBIOS_BOOL bFilterInterlace;
    CBIOS_U32  MaxDclk;
}CBIOS_MODE_FILTER_PARA, *PCBIOS_MODE_FILTER_PARA;


CBIOS_BOOL cbMode_GetHVTiming(PCBIOS_VOID pvcbe,
                              CBIOS_U32 XRes,
                              CBIOS_U32 YRes,
                              CBIOS_U32 Refresh,
                              CBIOS_U32 isInterlace,
                              CBIOS_ACTIVE_TYPE Device,
                              PCBIOS_TIMING_ATTRIB pTiming);
CBIOS_U32 cbMode_GetDefaultModeList(PCBIOS_VOID pvcbe, PCBiosModeInfoExt pModeList, CBIOS_ACTIVE_TYPE Device);
CBIOS_VOID cbMode_GetAdapterModeNum(PCBIOS_VOID pvcbe, CBIOS_U32* AdapterModeNum);
CBIOS_STATUS cbMode_FillAdapterModeList(PCBIOS_VOID pvcbe, PCBiosModeInfoExt pModeList, CBIOS_U32 *pBufferSize);
CBIOS_VOID cbMode_GetFilterPara(PCBIOS_VOID pvcbe, CBIOS_ACTIVE_TYPE Device, PCBIOS_MODE_FILTER_PARA pFilter);
CBIOS_U32 cbMode_GenerateDeviceModeList(PCBIOS_VOID pvcbe, CBIOS_ACTIVE_TYPE  Device);

#endif
