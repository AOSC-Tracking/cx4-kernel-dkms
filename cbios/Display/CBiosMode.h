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

typedef struct _CBIOS_MODE_FILTER_PARA{
    CBIOS_BOOL bFilterInterlace;
    CBIOS_U32  MaxDclk;
}CBIOS_MODE_FILTER_PARA, *PCBIOS_MODE_FILTER_PARA;

CBIOS_BOOL cbMode_GetHVTiming(PCBIOS_VOID pvcbe, PCBIOS_VOID  pMode, CBIOS_ACTIVE_TYPE Device, PCBIOS_TIMING_ATTRIB pTiming);
CBIOS_U32  cbMode_GetTimingSignal(PCBIOS_VOID pvcbe, CBIOS_ACTIVE_TYPE Device, CBIOS_U32 ReqSignal, PCBIOS_TIMING_ATTRIB pTiming);
CBIOS_U32 cbMode_GetDefaultModeList(PCBIOS_VOID pvcbe, PCBiosModeInfoExt pModeList, CBIOS_ACTIVE_TYPE Device);
CBIOS_VOID cbMode_GetAdapterModeNum(PCBIOS_VOID pvcbe, CBIOS_U32* AdapterModeNum);
CBIOS_STATUS cbMode_FillAdapterModeList(PCBIOS_VOID pvcbe, PCBiosModeInfoExt pModeList, CBIOS_U32 *pBufferSize);
CBIOS_VOID cbMode_GetFilterPara(PCBIOS_VOID pvcbe, CBIOS_ACTIVE_TYPE Device, PCBIOS_MODE_FILTER_PARA pFilter);
CBIOS_U32 cbMode_MakeDeviceModeList(PCBIOS_VOID pvcbe, CBIOS_ACTIVE_TYPE  Device);

#endif
