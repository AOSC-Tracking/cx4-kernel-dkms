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
** Defines common function pointer for each device port.
** Defines CBIOS_DEVICE_COMMON.
**
** NOTE:
** 
******************************************************************************/

#ifndef _CBIOS_DEVICE_SHARE_H_
#define _CBIOS_DEVICE_SHARE_H_

#include "CBiosShare.h"
#include "../Display/CBiosMode.h"
#include "../Util/CBiosEDID.h"
#include "Port/CBiosDSI.h"
#include "../Display/CBiosPathManager.h"
#include "Monitor/CBiosEDPPanel.h"
#include "../Hw/HwUtil/CBiosUtilHw.h"

#define CBIOS_MAX_DISPLAY_DEVICES_NUM                  32
#define DP_USB_MODU_NUM  2
#define DP_MAX_LTTPR_CNT  8
#define DP_MAX_SUPPORTED_RATES_NUM  8
#define HW_SUPPORTED_MAX_LINK_SPEED_ENTRY_NUM  3 //there are 4(RBR,HBR,HBR2,HBR3) kinds of link rate enry, but HBR3 seems not used in driver yet

typedef CBIOS_VOID 
(*PFN_cbDeviceHwInit)(PCBIOS_VOID pvcbe, PCBIOS_VOID pDevCommon);

typedef CBIOS_BOOL
(*PFN_cbDeviceDetect)(PCBIOS_VOID pvcbe, PCBIOS_VOID pDevCommon, PCBIOS_DETECT_FLAG pDetectFlag);

typedef CBIOS_VOID 
(*PFN_cbDeviceOnOff)(PCBIOS_VOID pvcbe, PCBIOS_VOID pDevCommon, CBIOS_BOOL bOn, CBIOS_U32 Flags);

typedef CBIOS_VOID 
(*PFN_cbQueryMonitorAttribute)(PCBIOS_VOID pvcbe, PCBIOS_VOID pDevCommon, PCBiosMonitorAttribute pMonitorAttribute);

typedef CBIOS_VOID 
(*PFN_cbUpdateDeviceModeInfo)(PCBIOS_VOID pvcbe, PCBIOS_VOID pDevCommon, PCBIOS_VOID pModeParams);

typedef CBIOS_VOID 
(*PFN_cbDeviceSetMode)(PCBIOS_VOID pvcbe, PCBIOS_VOID pDevCommon, PCBIOS_VOID pModeParams);

typedef struct _CBIOS_DEVICE_SIGNATURE
{
    CBIOS_U8 MonitorID[MONITORIDLENGTH];
    CBIOS_U8 ExtFlagChecksum[CBIOS_EDID_MAX_BLK_CNT][EXT_FLAG_CHKSUM_LEN];
}CBIOS_DEVICE_SIGNATURE, *PCBIOS_DEVICE_SIGNATURE;

typedef enum _CBIOS_PM_STATUS
{
    CBIOS_PM_ON         = 0,
    CBIOS_PM_STANDBY    = 1,
    CBIOS_PM_SUSPEND    = 2,
    CBIOS_PM_OFF        = 4,
    CBIOS_PM_INVALID    = 0xFF
} CBIOS_PM_STATUS, *PCBIOS_PM_STATUS;


typedef struct _CBIOS_DEVICE_COMMON
{
    //Common attribute with fixed value
    struct
    {
        CBIOS_ACTIVE_TYPE           DeviceType;
        CBIOS_CONNECTOR_TYPE        PortConnType;
        CBIOS_MONITOR_TYPE          SupportMonitorType;
        CBIOS_U16                   I2CBus; // real I2C bus used to read EDID from monitor
        CBIOS_U16                   I2CDelay;
        PCBIOS_VOID                 pBusLock;
        PCBIOS_VOID                 pHDCPContext;                  
    };

    //Common attribute with variable value
    struct
    {
        CBIOS_BOOL                  bConnected;
        CBIOS_MONITOR_TYPE          CurrentMonitorType;
        CBIOS_U8                    PowerState;
        CBIOS_EDID_DATA             EdidData;
        CBIOS_EDID_STRUCTURE_DATA   EdidStruct;
        PCBiosModeInfoExt           pDeviceModeList;
        CBIOS_U32                   ModeListArraySize;
        CBIOS_U32                   ValidModeNum;
        CBIOS_DEVICE_SIGNATURE      ConnectedDevSignature; // stores the signature of real device.
        CBIOS_DISPLAY_SOURCE        DispSource;
        CBIOS_EDID_DATA             FakeEdid;
        CBIOS_BOOL                  isFakeEdid;
        CBIOS_MAX_RES_CONFIG        MaxResConfig;
    };

    union
    {
        CBIOS_DSI_PARAMS            DSIDevice;
        CBIOS_EDPPanel_PARAMS       EDPPanelDevice;
    }DeviceParas;

    //common funcs for each device port
    struct
    {
        PFN_cbDeviceHwInit          pfncbDeviceHwInit;
        PFN_cbDeviceDetect          pfncbDeviceDetect;
        PFN_cbDeviceOnOff           pfncbDeviceOnOff;
        PFN_cbDeviceSetMode         pfncbDeviceSetMode;

        PFN_cbQueryMonitorAttribute pfncbQueryMonitorAttribute;
        PFN_cbUpdateDeviceModeInfo  pfncbUpdateDeviceModeInfo;
    };
}CBIOS_DEVICE_COMMON, *PCBIOS_DEVICE_COMMON;

typedef struct _CBIOS_DEVICE_MANAGER
{
    CBIOS_ACTIVE_TYPE    SupportDevices;
    PCBIOS_DEVICE_COMMON pDeviceArray[CBIOS_MAX_DISPLAY_DEVICES_NUM];
}CBIOS_DEVICE_MANAGER, *PCBIOS_DEVICE_MANAGER;

#define cbGetDeviceCommon(pDevMgr, Device)   ((pDevMgr)->pDeviceArray[cbConvertDeviceBit2Index(Device)])

static inline CBIOS_VOID cbInitialModuleList(PCBIOS_MODULE_LIST pModuleList)
{
    pModuleList->DPModule.Type = CBIOS_MODULE_TYPE_DP;
    pModuleList->DPModule.Index = CBIOS_MODULE_INDEX_INVALID;
    pModuleList->MHLModule.Type = CBIOS_MODULE_TYPE_MHL;
    pModuleList->MHLModule.Index = CBIOS_MODULE_INDEX_INVALID;
    pModuleList->HDCPModule.Type = CBIOS_MODULE_TYPE_HDCP;
    pModuleList->HDCPModule.Index = CBIOS_MODULE_INDEX_INVALID;
    pModuleList->HDMIModule.Type = CBIOS_MODULE_TYPE_HDMI;
    pModuleList->HDMIModule.Index = CBIOS_MODULE_INDEX_INVALID;
    pModuleList->HDTVModule.Type = CBIOS_MODULE_TYPE_HDTV;
    pModuleList->HDACModule.Index = CBIOS_MODULE_INDEX_INVALID;
    pModuleList->HDACModule.Type = CBIOS_MODULE_TYPE_HDAC;
    pModuleList->HDTVModule.Index = CBIOS_MODULE_INDEX_INVALID;
    pModuleList->IGAModule.Type = CBIOS_MODULE_TYPE_IGA;
    pModuleList->IGAModule.Index = CBIOS_MODULE_INDEX_INVALID;
}

#define IS_SUPPORT_4K_MODE        1

extern CBIOS_U8 FPGAHDMIEdid[256];
#if IS_SUPPORT_4K_MODE
extern CBIOS_U8 Fake4KEdid[256];
#endif

CBIOS_BOOL cbIsEdidChecksumValid(PCBIOS_UCHAR  pEdidData, CBIOS_U32  ulLength);
CBIOS_VOID cbFixEdidChecksum(PCBIOS_VOID pvcbe, PCBIOS_UCHAR  pEdidData, CBIOS_U32  ulLength);
CBIOS_STATUS cbI2cNormal_ReadEDID(PCBIOS_VOID pvcbe, PCBIOS_DEVICE_COMMON pDevCommon, CBIOS_U8 *EDIDData, CBIOS_U32 ulReadEdidOffset, CBIOS_U32 ulBufferSize, CBIOS_U8 nSegNum);
CBIOS_BOOL cbGetEdidBytes(PCBIOS_VOID pvcbe, PCBIOS_DEVICE_COMMON pDevCommon, CBIOS_U8 *EDIDData, CBIOS_U32 ulReadEdidOffset, CBIOS_U32 ulBufferSize);
PCBIOS_U8 cbGetFullEdid(PCBIOS_VOID pvcbe, PCBIOS_DEVICE_COMMON pDevCommon, CBIOS_U32 *ReadEdidSize);
CBIOS_BOOL cbGetDeviceEDID(PCBIOS_VOID pvcbe, PCBIOS_DEVICE_COMMON pDevCommon, CBIOS_BOOL *pIsDevChanged, CBIOS_U32 FullDetect);
CBIOS_BOOL cbGetDeviceSignatureFromEdid(PCBIOS_VOID pvcbe, CBIOS_EDID_DATA *pEdidData, CBIOS_DEVICE_SIGNATURE *pSignture);
CBIOS_MONITOR_TYPE cbGetSupportMonitorType(PCBIOS_VOID pvcbe, CBIOS_ACTIVE_TYPE devices);
CBIOS_VOID cbClearEdidRelatedData(PCBIOS_VOID pvcbe, PCBIOS_DEVICE_COMMON pDevCommon);
CBIOS_BOOL cbDualModeDetect(PCBIOS_VOID pvcbe, PCBIOS_DEVICE_COMMON pDevCommon);
PCBIOS_VOID cbGetDPMonitorContext(PCBIOS_VOID pvcbe, PCBIOS_DEVICE_COMMON pDevCommon);
PCBIOS_VOID cbGetHDMIMonitorContext(PCBIOS_VOID pvcbe, PCBIOS_DEVICE_COMMON pDevCommon);
CBIOS_STATUS cbGetDeviceELD(PCBIOS_VOID pvcbe, CBIOS_ACTIVE_TYPE DeviceType, PCBIOS_ELD_MEM_STRUCT pELD);
CBIOS_BOOL cbGetEdidPerSeg(PCBIOS_VOID pvcbe, PCBIOS_DEVICE_COMMON pDevCommon, CBIOS_U8 *EDIDData, CBIOS_U32 ulReadEdidOffset, CBIOS_U32 ulBufferSize, CBIOS_U8  nSegNum);

CBIOS_VOID *cbGetDeviceBusLock(PCBIOS_VOID pvcbe, CBIOS_U32 Index, CBIOS_U32  IndexType);

#endif
