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
** CBios interface implementation.
**
** NOTE:
** The interface and parameters SHOULD NOT be modified under normal condition.
** If the interface need modify for some special case, please make sure to 
** update the code where calls this interface.
******************************************************************************/

#include "CBiosShare.h"
#include "CBiosTypes.h"
#include "CBiosChipShare.h"
#include "../Hw/HwInterface/CBiosHwInterface.h"
#include "../Hw/HwUtil/CBiosUtilHw.h"
#include "../Hw/HwUtil/CBiosEfuse.h"
#include "../Callback/CBiosCallbacks.h"
#include "../Device/Monitor/CBiosHDMIMonitor.h"

/*
CBIOS_STATUS
DllInitialize( CBIOS_IN PCBIOS_VOID pus )
{
    return CBIOS_OK;
}

CBIOS_STATUS
DllUnload( CBIOS_VOID )
{
    return CBIOS_OK;
}

// The DLL must have an entry point, but it is never called.
//
CBIOS_STATUS
DriverEntry(
  CBIOS_IN PCBIOS_VOID DriverObject,
  CBIOS_IN PCBIOS_VOID RegistryPath
)
{
    return CBIOS_OK;
}
*/

DLLEXPORTS CBIOS_STATUS
CBiosGetExtensionSize(PCBIOS_CHIP_ID pCBChipId, CBIOS_U32 *pulExtensionSize)
{
    if(pulExtensionSize == CBIOS_NULL)
    {
         cbDebugPrint((MAKE_LEVEL(GENERIC, ERROR),("CBiosGetExtensionSize: pulExtensionSize is CBIOS_NULL!\n")));
         return CBIOS_ER_INVALID_PARAMETER;
    }

    cbGetExtensionSize(pulExtensionSize);

    return CBIOS_OK;
}

DLLEXPORTS CBIOS_STATUS
CBiosInit(PCBIOS_VOID pvcbe, PCBIOS_PARAM_INIT pCBParamInit)
{
    if(pvcbe == CBIOS_NULL)
    {
        cbDebugPrint((MAKE_LEVEL(GENERIC, ERROR),"CBiosInit: pvcbe is CBIOS_NULL!\n"));
        return CBIOS_ER_INVALID_PARAMETER;    
    }
    
    if (cbInitialize(pvcbe, pCBParamInit))
    {
        return CBIOS_OK;
    }
    else
    {
        cbDebugPrint((MAKE_LEVEL(GENERIC, ERROR), "cbInitialize: function failure!\n"));
        cbPrintMsgToFile(DBG_LEVEL_ERROR_MSG, "cbInitialize: function failure!\n", CBIOS_NULL, 0);
        return CBIOS_ER_INTERNAL;
    }

}


DLLEXPORTS CBIOS_STATUS
CBiosSetCallBackFunctions(PCBIOS_CALLBACK_FUNCTIONS pFnCallBack)
{
    CBIOS_STATUS Status;

    Status = cbSetCallBackFunctions(pFnCallBack);

    return Status;
}

DLLEXPORTS CBIOS_STATUS
CBiosDetectAttachedDisplayDevices(PCBIOS_VOID pvcbe, PCIP_DEVICES_DETECT_PARAM pDevicesDetectParam)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    PCBIOS_DEVICE_COMMON  pDevCommon = CBIOS_NULL;
    CBIOS_ACTIVE_TYPE CurDevice = CBIOS_TYPE_NONE;
    CBIOS_U32  Devices =0,  Detected = 0;
    CBIOS_BOOL bHardcodeDetected[CBIOS_MAX_DISPLAY_DEVICES_NUM];
    CBIOS_DETECT_FLAG DetectFlag;

    cb_memset(bHardcodeDetected, CBIOS_FALSE, sizeof(bHardcodeDetected));

    if (pDevicesDetectParam == CBIOS_NULL || pDevicesDetectParam->DevicesToDetect == CBIOS_TYPE_NONE)
    {
        cbDebugPrint((MAKE_LEVEL(GENERIC, ERROR), "%s: the 2nd param is NULL!\n", FUNCTION_NAME));
        return CBIOS_ER_NULLPOINTER;
    }

    Devices = pDevicesDetectParam->DevicesToDetect;
    if (pcbe->DriverFlags.bRunOnQT)
    {
        Devices &= (CBIOS_TYPE_CRT | CBIOS_TYPE_DP1);

        if(Devices & CBIOS_TYPE_CRT)
        {
            bHardcodeDetected[cbConvertDeviceBit2Index(CBIOS_TYPE_CRT)] = CBIOS_TRUE;
        }

        if(Devices & CBIOS_TYPE_DP1)
        {
            bHardcodeDetected[cbConvertDeviceBit2Index(CBIOS_TYPE_DP1)] = CBIOS_TRUE;
        }
    }

    while (Devices)
    {
        CurDevice = GET_LAST_BIT(Devices);
        Devices &= ~CurDevice;
        
        pDevCommon = pcbe->DeviceMgr.pDeviceArray[cbConvertDeviceBit2Index(CurDevice)];

        if(pDevCommon == CBIOS_NULL)
        {
            continue;
        }

        DetectFlag.Value = 0;
        DetectFlag.HardcodeDetect = (bHardcodeDetected[cbConvertDeviceBit2Index(CurDevice)])? 1 : 0;
        DetectFlag.FullDetect = (pDevicesDetectParam->FullDetect)? 1 : 0;
        
        if(cbDevDeviceDetect(pcbe, pDevCommon, &DetectFlag))
        {
            Detected |= CurDevice;
            pDevCommon->bConnected = CBIOS_TRUE;
        }
        else
        {
            pDevCommon->bConnected = CBIOS_FALSE;
        }
    }

    pDevicesDetectParam->DetectedDevices = Detected;

    return CBIOS_OK;
}


DLLEXPORTS CBIOS_STATUS
CBiosDetectOutputStatus(PCBIOS_VOID pvcbe, PCBIOS_OUTPUT_DETECT_PARAM pOutputDetect)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    PCBIOS_DEVICE_COMMON  pDevCommon = CBIOS_NULL;
    CBIOS_ACTIVE_TYPE CurDevice = CBIOS_TYPE_NONE;
    CBIOS_BOOL bHardcodeDetected = CBIOS_FALSE;
    CBIOS_DETECT_FLAG DetectFlag;

    if (pOutputDetect == CBIOS_NULL)
    {
        cbDebugPrint((MAKE_LEVEL(GENERIC, ERROR), "%s: the 2nd param is NULL!\n", FUNCTION_NAME));
        return CBIOS_ER_NULLPOINTER;
    }
    else if(pOutputDetect->OutputToDetect == CBIOS_TYPE_NONE || !IS_ONE_BIT(pOutputDetect->OutputToDetect))
    {
        cbDebugPrint((MAKE_LEVEL(GENERIC, ERROR), "%s: Invalid device bit %d!\n", FUNCTION_NAME, pOutputDetect->OutputToDetect));
        return CBIOS_ER_INVALID_PARAMETER;
    }

    CurDevice = pOutputDetect->OutputToDetect;
    pOutputDetect->bConnected = CBIOS_FALSE;
    pOutputDetect->bChanged = CBIOS_FALSE;

    if((CurDevice & pcbe->DeviceMgr.SupportDevices) == 0)
    {
        return CBIOS_ER_INVALID_PARAMETER;
    }
    
    if(pcbe->DriverFlags.bRunOnQT && (CurDevice == CBIOS_TYPE_CRT || CurDevice == CBIOS_TYPE_DP1))
    {
        bHardcodeDetected = CBIOS_TRUE;
    }

    pDevCommon = pcbe->DeviceMgr.pDeviceArray[cbConvertDeviceBit2Index(CurDevice)];
    if(pDevCommon == CBIOS_NULL)
    {
        return CBIOS_ER_INTERNAL;
    }

    DetectFlag.Value = 0;
    DetectFlag.HardcodeDetect = (bHardcodeDetected)? 1 : 0;
    DetectFlag.FullDetect = (pOutputDetect->bFullDetect)? 1 : 0;

    pOutputDetect->bConnected = cbDevDeviceDetect(pcbe, pDevCommon, &DetectFlag);

    //in low level device detect function, DetectFlag.Changed means edid changed, which include no edid(disconnected) to edid(connected) case.
    //but driver only care monitor change case(old monitor to new one) from this flag
    if(DetectFlag.Connected && DetectFlag.Changed && 
       (pDevCommon->bConnected == CBIOS_TRUE))
    {
        pOutputDetect->bChanged = CBIOS_TRUE;
    }

    pDevCommon->bConnected = pOutputDetect->bConnected;

    return CBIOS_OK;
}


DLLEXPORTS CBIOS_STATUS
CBiosInitHW(PCBIOS_VOID pvcbe)
{
    return cbHWInit(pvcbe);
}

DLLEXPORTS CBIOS_STATUS
CBiosGetEdid(PCBIOS_VOID pvcbe, PCBIOS_PARAM_GET_EDID pCBParamGetEdid)
{
    PCBIOS_EXTENSION_COMMON pcbe = pvcbe;
    PCBIOS_DEVICE_COMMON  pDevCommon = CBIOS_NULL;

    if (pCBParamGetEdid == CBIOS_NULL || pCBParamGetEdid->DeviceId == CBIOS_TYPE_NONE)
    {
        cbDebugPrint((MAKE_LEVEL(GENERIC, ERROR), "%s: the 2nd param is NULL!\n", FUNCTION_NAME));
        return CBIOS_ER_NULLPOINTER;
    }

    pDevCommon = pcbe->DeviceMgr.pDeviceArray[cbConvertDeviceBit2Index(pCBParamGetEdid->DeviceId)];

    if(pDevCommon == CBIOS_NULL)
    {
        cbDebugPrint((MAKE_LEVEL(GENERIC, ERROR), "%s: pDevCommon is NULL!\n", FUNCTION_NAME));
        return CBIOS_ER_NULLPOINTER;
    }

    return cbDevGetEdidFromBuffer(pcbe, pDevCommon, pCBParamGetEdid);
}

DLLEXPORTS CBIOS_STATUS
CBiosI2CDataRead(PCBIOS_VOID pvcbe, PCBIOS_PARAM_I2C_DATA pCBParamI2CData)
{
    PCBIOS_EXTENSION_COMMON pcbe = pvcbe;

    CBIOS_STATUS Status = CBIOS_ER_NOT_YET_IMPLEMENTED;

    if (pCBParamI2CData == CBIOS_NULL)
    {
        cbDebugPrint((MAKE_LEVEL(GENERIC, ERROR), "%s: pCBParamI2CData is NULL!\n", FUNCTION_NAME));
        return CBIOS_ER_NULLPOINTER;
    }

    Status = cbDevI2C_ReadData(pcbe,pCBParamI2CData);

    return Status;
}

DLLEXPORTS CBIOS_STATUS
CBiosI2CDataWrite(PCBIOS_VOID pvcbe, PCBIOS_PARAM_I2C_DATA pCBParamI2CData)
{
    PCBIOS_EXTENSION_COMMON pcbe = pvcbe;

    CBIOS_STATUS Status = CBIOS_ER_NOT_YET_IMPLEMENTED;

    if (pCBParamI2CData == CBIOS_NULL)
    {
        cbDebugPrint((MAKE_LEVEL(GENERIC, ERROR), "%s: pCBParamI2CData is NULL!\n", FUNCTION_NAME));
        return CBIOS_ER_NULLPOINTER;
    }

    Status = cbDevI2C_WriteData(pcbe, pCBParamI2CData);

    return Status;
}

DLLEXPORTS CBIOS_STATUS
CBiosUnload(PCBIOS_VOID pvcbe)
{
    PCBIOS_EXTENSION_COMMON pcbe = pvcbe;
    CBIOS_STATUS Status = CBIOS_OK;

    if (!pcbe)
    {
        return Status;
    }

    Status = cbHWUnload(pcbe);

    if (Status == CBIOS_OK)
    {
        cb_DestroyLock(pcbe->pSpinLock, CBIOS_OS_SPIN_LOCK);

        if (pcbe->pBootDevPriority != CBIOS_NULL)
        {
            cb_FreePool(pcbe->pBootDevPriority);
            pcbe->pBootDevPriority = CBIOS_NULL;
        }
        if (pcbe->PostRegTable[VCP_TABLE].pCRDefault != CBIOS_NULL)
        {
            cb_FreePool(pcbe->PostRegTable[VCP_TABLE].pCRDefault);
            pcbe->PostRegTable[VCP_TABLE].pCRDefault = CBIOS_NULL;
        }
        if (pcbe->PostRegTable[VCP_TABLE].pSRDefault != CBIOS_NULL)
        {
            cb_FreePool(pcbe->PostRegTable[VCP_TABLE].pSRDefault);
            pcbe->PostRegTable[VCP_TABLE].pSRDefault = CBIOS_NULL;
        }
        if (pcbe != CBIOS_NULL)
        {
            cb_FreePool(pcbe);
            pcbe = CBIOS_NULL;
        }
    }
    else
    {
        cbDebugPrint((MAKE_LEVEL(GENERIC, ERROR), " CBiosUnload failed!\n"));
    }

    return Status;

}

//The following interface is for new setting mode logic 
DLLEXPORTS CBIOS_STATUS
CBiosGetDeviceModeListBufferSize(CBIOS_IN PCBIOS_VOID pvcbe,
                                 CBIOS_IN CBIOS_U32  DeviceId,
                                 CBIOS_OUT CBIOS_U32 *pBufferSize)
{
    PCBIOS_EXTENSION_COMMON pcbe = pvcbe;
    PCBIOS_DEVICE_COMMON  pDevCommon = CBIOS_NULL; 

    if(pBufferSize == CBIOS_NULL || DeviceId == CBIOS_TYPE_NONE)
    {
        cbDebugPrint((MAKE_LEVEL(GENERIC, ERROR), "%s: pBufferSize or DeviceId is NULL!\n", FUNCTION_NAME));
        return CBIOS_ER_INVALID_PARAMETER;
    }

    pDevCommon = pcbe->DeviceMgr.pDeviceArray[cbConvertDeviceBit2Index(DeviceId)];

    if(pDevCommon == CBIOS_NULL)
    {
        cbDebugPrint((MAKE_LEVEL(GENERIC, ERROR), "%s: pDevCommon is NULL!\n", FUNCTION_NAME));
        return CBIOS_ER_INVALID_PARAMETER;
    }

    return  cbDevGetDeviceModeListBufferSize(pcbe, pDevCommon, pBufferSize);
}

DLLEXPORTS CBIOS_STATUS
CBiosGetDeviceModeList(CBIOS_IN PCBIOS_VOID pvcbe,
                       CBIOS_IN CBIOS_U32  DeviceId,
                       CBIOS_OUT PCBiosModeInfoExt pModeList,
                       CBIOS_IN CBIOS_OUT CBIOS_U32 * pBufferSize)
{
    PCBIOS_EXTENSION_COMMON pcbe = pvcbe;
    PCBIOS_DEVICE_COMMON  pDevCommon = CBIOS_NULL;

    if(pBufferSize == CBIOS_NULL || DeviceId == CBIOS_TYPE_NONE)
    {
        cbDebugPrint((MAKE_LEVEL(GENERIC, ERROR), "%s: pBufferSize or DeviceId is NULL!\n", FUNCTION_NAME));
        return CBIOS_ER_INVALID_PARAMETER;
    }

    pDevCommon = pcbe->DeviceMgr.pDeviceArray[cbConvertDeviceBit2Index(DeviceId)];

    if(pDevCommon == CBIOS_NULL)
    {
        cbDebugPrint((MAKE_LEVEL(GENERIC, ERROR), "%s: pDevCommon is NULL!\n", FUNCTION_NAME));
        return CBIOS_ER_INVALID_PARAMETER;
    }

    return  cbDevGetDeviceModeList(pcbe, pDevCommon, pModeList, pBufferSize);
}


DLLEXPORTS CBIOS_STATUS
CBiosGetModeFromIGA(CBIOS_IN CBIOS_VOID* pvcbe, 
                    CBIOS_OUT PCBiosSettingModeParams pModeParams)
{
    CBIOS_STATUS status = CBIOS_ER_INVALID_PARAMETER;
    PCBIOS_EXTENSION_COMMON pcbe = pvcbe; 
    if(pModeParams)
    {
        status = cbDispMgrGetMode(pcbe, &pcbe->DispMgr, pModeParams);
    }
    return status;
}

DLLEXPORTS  CBIOS_STATUS
CBiosGetHwCounter(CBIOS_IN PCBIOS_VOID pvcbe, 
                       CBIOS_IN CBIOS_OUT PCBIOS_GET_HW_COUNTER  pGetCounter)
{
    CBIOS_STATUS status = CBIOS_ER_INVALID_PARAMETER;
    PCBIOS_EXTENSION_COMMON pcbe = pvcbe; 

    if(pGetCounter)
    {
        status = cbDispMgrGetHwCounter(pcbe, pGetCounter);
    }
    
    return status;    
}

DLLEXPORTS CBIOS_STATUS
CBiosGetModeFromReg(CBIOS_IN CBIOS_VOID* pvcbe,
                    CBIOS_IN CBIOS_OUT PCBiosSettingModeParams pModeParams)
{
    CBIOS_STATUS status = CBIOS_ER_INVALID_PARAMETER;
    PCBIOS_EXTENSION_COMMON pcbe = pvcbe;
    if(pModeParams)
    {
        status = cbDispMgrGetModeFromReg(pcbe, &pcbe->DispMgr, pModeParams);
    }
    return status;
}

DLLEXPORTS CBIOS_STATUS
CBiosSetModeToIGA(CBIOS_IN PCBIOS_VOID pvcbe,
                  CBIOS_IN PCBiosSettingModeParams pSettingModeParams)
{
    PCBIOS_EXTENSION_COMMON pcbe = pvcbe; 
    return cbDispMgrSetMode(pcbe, &pcbe->DeviceMgr, &pcbe->DispMgr, pSettingModeParams);
}

DLLEXPORTS   CBIOS_STATUS  
CBiosUpdateFrame(CBIOS_IN PCBIOS_VOID  pvcbe, 
                 CBIOS_IN PCBIOS_UPDATE_FRAME_PARA pUpdateFrame)
{
    PCBIOS_EXTENSION_COMMON  pcbe = pvcbe;
    return cbDispMgrUpdateFrame(pcbe, pUpdateFrame);
}

DLLEXPORTS   CBIOS_STATUS
CBiosDoCSCAdjust(CBIOS_IN PCBIOS_VOID  pvcbe,
                 CBIOS_IN PCBIOS_CSC_ADJUST_PARA pCSCAdjustPara)
{
    PCBIOS_EXTENSION_COMMON  pcbe = pvcbe;

    return cbDispMgrDoCSCAdjust(pcbe, &pcbe->DispMgr, pCSCAdjustPara);
}

DLLEXPORTS   CBIOS_STATUS
CBiosAdjustStreamCSC(CBIOS_IN PCBIOS_VOID  pvcbe,
                 CBIOS_IN PCBIOS_STREAM_CSC_PARA pCSCAdjustPara)
{
    PCBIOS_EXTENSION_COMMON  pcbe = pvcbe;

    return cbDispMgrAdjustStreamCSC(pcbe, pCSCAdjustPara);
}

DLLEXPORTS CBIOS_STATUS  
CBiosGetDispResource(CBIOS_IN PCBIOS_VOID  pvcbe, 
                  CBIOS_IN CBIOS_OUT PCBIOS_GET_DISP_RESOURCE pGetDispRes)
{
    PCBIOS_EXTENSION_COMMON pcbe = pvcbe; 
    return cbDispMgrGetDispResource(pcbe, pGetDispRes);
}

DLLEXPORTS CBIOS_STATUS  
CBiosGetDisplayCaps(CBIOS_IN PCBIOS_VOID  pvcbe, 
                   CBIOS_IN CBIOS_OUT PCBIOS_DISPLAY_CAPS pDispCaps)
{
    PCBIOS_EXTENSION_COMMON pcbe = pvcbe; 
    return cbDispMgrGetDisplayCaps(pcbe, pDispCaps);
}


DLLEXPORTS   CBIOS_STATUS
CBiosSetCursor(CBIOS_IN PCBIOS_VOID  pvcbe,
                  CBIOS_IN   PCBIOS_CURSOR_PARA    pSetCursor)
{
    PCBIOS_EXTENSION_COMMON  pcbe = pvcbe;

    if(CBIOS_NULL == pSetCursor)
    {
        cbDebugPrint((MAKE_LEVEL(GENERIC, ERROR), "%s: Cursor param is NULL!\n", FUNCTION_NAME));
        return CBIOS_ER_INVALID_PARAMETER;
    }

    return cbDispMgrSetHwCursor(pcbe, pSetCursor);
}

DLLEXPORTS  CBIOS_STATUS  
CBiosGetDisplayAddr(CBIOS_IN  PCBIOS_VOID  pvcbe,
                  CBIOS_IN  CBIOS_OUT  PCBIOS_GET_DISP_ADDR  pGetDispAddr)
{
    PCBIOS_EXTENSION_COMMON pcbe = pvcbe;
    if (pGetDispAddr == CBIOS_NULL)
    {
        return CBIOS_ER_NULLPOINTER;
    }
    return  cbDispMgrGetDispAddr(pcbe, pGetDispAddr);  
}

DLLEXPORTS CBIOS_STATUS  
CBiosGetActiveDevice(CBIOS_IN CBIOS_VOID* pvcbe, CBIOS_OUT PCIP_ACTIVE_DEVICES pActiveDevices)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    CBIOS_STATUS Status = CBIOS_OK;

    if (pActiveDevices == CBIOS_NULL)
    {
        cbDebugPrint((MAKE_LEVEL(GENERIC, ERROR), "%s: the 2nd param is NULL!\n", FUNCTION_NAME));
        return CBIOS_ER_NULLPOINTER;
    }

    Status = cbDispMgrGetActiveDevice(pcbe, &pcbe->DispMgr, pActiveDevices);
    return Status;
}

DLLEXPORTS CBIOS_STATUS  
CBiosSetActiveDevice(CBIOS_IN CBIOS_VOID* pvcbe, CBIOS_IN PCIP_ACTIVE_DEVICES pActiveDevices)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    CBIOS_STATUS Status = CBIOS_OK;

    if (pActiveDevices == CBIOS_NULL)
    {
        cbDebugPrint((MAKE_LEVEL(GENERIC, ERROR), "%s: the 2nd param is NULL!\n", FUNCTION_NAME));
        return CBIOS_ER_NULLPOINTER;
    }

    Status = cbDispMgrSetActiveDevice(pcbe, &pcbe->DispMgr, pActiveDevices);
    return Status;
}

DLLEXPORTS CBIOS_STATUS
CBiosGetModeTiming(CBIOS_IN CBIOS_VOID* pvcbe, CBIOS_IN CBIOS_OUT PCBIOS_GET_MODE_TIMING_PARAM pGetModeTiming)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    PCBIOS_DEVICE_COMMON  pDevCommon = CBIOS_NULL;
    
    if(pGetModeTiming == CBIOS_NULL || pGetModeTiming->DeviceId == CBIOS_TYPE_NONE)
    {
        cbDebugPrint((MAKE_LEVEL(GENERIC, ERROR), "%s: the 2nd param is NULL!\n", FUNCTION_NAME));
        return CBIOS_ER_NULLPOINTER;
    }

    pDevCommon = pcbe->DeviceMgr.pDeviceArray[cbConvertDeviceBit2Index(pGetModeTiming->DeviceId)];

    if(pDevCommon == CBIOS_NULL)
    {
        cbDebugPrint((MAKE_LEVEL(GENERIC, ERROR), "%s: pDevCommon is NULL!\n", FUNCTION_NAME));
        return CBIOS_ER_NULLPOINTER;
    }

    return  cbDevGetModeTiming(pcbe, pDevCommon, pGetModeTiming);
}

DLLEXPORTS CBIOS_STATUS
CBiosGetVBiosInfo(CBIOS_IN CBIOS_VOID* pvcbe, CBIOS_IN CBIOS_OUT PCBIOS_VBINFO_PARAM  pVbiosInfo)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    CBIOS_STATUS status;
    
    status = cbHWGetVBiosInfo(pcbe, pVbiosInfo);
    return status;
}

DLLEXPORTS CBIOS_STATUS
CBiosSyncDataWithVbios(CBIOS_IN PCBIOS_VOID pvcbe, CBIOS_IN PCBIOS_VBIOS_DATA_PARAM pDataParam)
{
    PCBIOS_EXTENSION_COMMON pcbe = pvcbe;
    //check function pointer
    if(!pDataParam)
    {
        cbDebugPrint((MAKE_LEVEL(GENERIC, ERROR), "%s: pDataParam is NULL!\n", FUNCTION_NAME));
        return CBIOS_ER_NULLPOINTER;
    }
    
    return  cbHwSyncDataWithVbios(pcbe, pDataParam);
}

DLLEXPORTS CBIOS_STATUS
CBiosSetMmioEndianMode(CBIOS_IN CBIOS_VOID* pAdapterContext)
{
    CBIOS_STATUS Status;
    Status = cbHWSetMmioEndianMode(pAdapterContext);
    return Status;
}


DLLEXPORTS CBIOS_STATUS
CBiosSetWriteback(CBIOS_IN PCBIOS_VOID pvcbe,
                  CBIOS_IN CBIOS_OUT PCBIOS_WB_PARA pWBPara)
{
    PCBIOS_EXTENSION_COMMON pcbe = pvcbe; 
    return cbHWSetWriteback(pcbe, pWBPara);
}

DLLEXPORTS  CBIOS_STATUS
CBiosDbgLevelCtl(PCBIOS_DBG_LEVEL_CTRL  pDbgLevelCtl)
{
    CBIOS_STATUS status = CBIOS_ER_INVALID_PARAMETER;
    if(pDbgLevelCtl)
    {
        status = cbDbgLevelCtl(pDbgLevelCtl);
    }
    return status;
}

DLLEXPORTS CBIOS_STATUS
CBiosGetVersion(PCBIOS_VOID pvcbe, PCBIOS_VERSION pCbiosVersion)
{
    PCBIOS_EXTENSION_COMMON pcbe = pvcbe;
    if(pCbiosVersion == CBIOS_NULL)
    {
        cbDebugPrint((MAKE_LEVEL(GENERIC, ERROR),("CBiosGetVersion: pCbiosVersion is CBIOS_NULL!\n")));
        return CBIOS_ER_INVALID_PARAMETER;
    } 
    return cbGetVersion(pcbe, pCbiosVersion);
}

DLLEXPORTS CBIOS_STATUS
CBiosIsChipEnable(PCBIOS_VOID pvcbe, PCBIOS_PARAM_CHECK_CHIPENABLE pCBParamCheckChipenable)
{
    CBIOS_STATUS status = CBIOS_ER_MMIO;
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;

    if ((pvcbe == CBIOS_NULL) || (pCBParamCheckChipenable == CBIOS_NULL))
    {
        status = CBIOS_ER_NULLPOINTER;
        return status;
    }

    if (cbHWIsChipEnable(pcbe) == CBIOS_FALSE)
    {
        pCBParamCheckChipenable->IsChipEnable = CBIOS_FALSE;
        status = CBIOS_ER_MMIO;
    }
    else
    {
        pCBParamCheckChipenable->IsChipEnable = CBIOS_TRUE;
        status = CBIOS_OK;
    }
    return status;
}

DLLEXPORTS CBIOS_STATUS
CBiosDumpReg(PCBIOS_VOID pvcbe, PCBIOS_PARAM_DUMP_REG pCBParamDumpReg)
{
    PCBIOS_EXTENSION_COMMON pcbe = pvcbe;
    return cbHWDumpReg(pcbe,pCBParamDumpReg);
}

DLLEXPORTS CBIOS_STATUS 
CBiosReadReg(PCBIOS_VOID pvcbe,
                   CBIOS_U8             type,
                   CBIOS_U8             index,
                   CBIOS_U8 *           result)
{
    PCBIOS_EXTENSION_COMMON pcbe = pvcbe;
    return cbHWReadReg(pcbe, type, index, result);
}

DLLEXPORTS CBIOS_STATUS 
CBiosWriteReg(PCBIOS_VOID pvcbe,
                    CBIOS_U8             type,
                    CBIOS_U8             index,
                    CBIOS_U8             value,
                    CBIOS_U8             mask)
{
    PCBIOS_EXTENSION_COMMON pcbe = pvcbe;

    return cbHWWriteReg(pcbe, type, index, value, mask);
}

DLLEXPORTS CBIOS_STATUS
CBiosSetIgaScreenOnOffState(PCBIOS_VOID pvcbe, CBIOS_BOOL status, CBIOS_U8 IGAIndex)
{
    PCBIOS_EXTENSION_COMMON pcbe = pvcbe;
    return cbHWSetIgaScreenOnOffState(pcbe, status, IGAIndex);
}

DLLEXPORTS CBIOS_STATUS
CBiosSetIgaOnOffState(PCBIOS_VOID pvcbe, CBIOS_BOOL status, CBIOS_U8 IGAIndex)
{
    PCBIOS_EXTENSION_COMMON pcbe = pvcbe;
    return cbHWSetIgaOnOffState(pcbe, status, IGAIndex);
}

DLLEXPORTS CBIOS_STATUS
CBiosSetDisplayDevicePowerState(PCBIOS_VOID pvcbe, CBIOS_U32 DevicesId, CBIOS_BOOL bPowerOn, CBIOS_U32 Flags)
{
    PCBIOS_EXTENSION_COMMON pcbe = pvcbe;
    CBIOS_ACTIVE_TYPE CurDev = CBIOS_TYPE_NONE;
    PCBIOS_DEVICE_COMMON pDevCommon = CBIOS_NULL;
    CBIOS_U32 i = 0, Devices = DevicesId;

    if(Devices == CBIOS_TYPE_NONE)
    {
        cbDebugPrint((MAKE_LEVEL(GENERIC, ERROR),("DeviceId is 0!\n")));
        return CBIOS_ER_INVALID_PARAMETER;
    }
    
#ifdef CHECK_CHIPENABLE
    if (!cbHWIsChipEnable(pcbe))
        return CBIOS_ER_CHIPDISABLE;
#endif
    
    while (Devices)
    {
        CurDev = GET_LAST_BIT(Devices);
        Devices &= (~CurDev);
        pDevCommon = pcbe->DeviceMgr.pDeviceArray[cbConvertDeviceBit2Index(CurDev)];

        if(pDevCommon == CBIOS_NULL)
        {
            continue;
        }

        for (i = IGA1; i < pcbe->DispMgr.IgaCount; i++)
        {
            if (CurDev & pcbe->DispMgr.ActiveDevices[i])
            {
                break;
            }
        }

        if (i == pcbe->DispMgr.IgaCount && bPowerOn)
        {
            cbDebugPrint((MAKE_LEVEL(GENERIC, WARNING), "%s: device 0x%x is not active device!\n", FUNCTION_NAME, CurDev));
            continue;
        }

        cbDevSetDisplayDevicePowerState(pcbe, pDevCommon, bPowerOn, Flags);
    }

    return CBIOS_OK;
}

DLLEXPORTS CBIOS_STATUS
CBiosGetDisplayDevicePowerState(PCBIOS_VOID pvcbe, CBIOS_U32 DeviceId, PCBIOS_BOOL pIsPowerOn)
{
    PCBIOS_EXTENSION_COMMON pcbe = pvcbe;
    PCBIOS_DEVICE_COMMON  pDevCommon = CBIOS_NULL;
    
    if(DeviceId == CBIOS_TYPE_NONE || !pIsPowerOn)
    {
        cbDebugPrint((MAKE_LEVEL(GENERIC, ERROR),("DeviceId is 0!\n")));
        return CBIOS_ER_INVALID_PARAMETER;
    }

    pDevCommon = pcbe->DeviceMgr.pDeviceArray[cbConvertDeviceBit2Index(DeviceId)];
    if(pDevCommon == CBIOS_NULL)
    {
        cbDebugPrint((MAKE_LEVEL(GENERIC, ERROR),("pDevCommon is CBIOS_NULL!\n")));
        return CBIOS_ER_INVALID_PARAMETER;
    }
    
    return cbDevGetDisplayDevicePowerState(pcbe, pDevCommon, pIsPowerOn);
}

DLLEXPORTS CBIOS_STATUS
CBiosGetAdapterModeListBufferSize(CBIOS_IN PCBIOS_VOID pvcbe,
                                  CBIOS_OUT CBIOS_U32 * pBufferSize)
{
    PCBIOS_EXTENSION_COMMON pcbe = pvcbe;
    CBIOS_U32 ulModeCount = 0;
    
    cbMode_GetAdapterModeNum(pcbe, &ulModeCount);
    *pBufferSize = ulModeCount*sizeof(CBiosModeInfoExt);
        
    return CBIOS_OK;
}

DLLEXPORTS CBIOS_STATUS
CBiosGetAdapterModeList(CBIOS_IN PCBIOS_VOID pvcbe,
                        CBIOS_OUT PCBiosModeInfoExt pModeList,
                        CBIOS_IN CBIOS_OUT CBIOS_U32 * pBufferSize)
{
    PCBIOS_EXTENSION_COMMON pcbe = pvcbe;
    return cbMode_FillAdapterModeList(pcbe, pModeList, pBufferSize);
}

DLLEXPORTS CBIOS_STATUS
CBiosGetHDMIAudioFomatList(CBIOS_IN PCBIOS_VOID pvcbe,
                       CBIOS_IN CBIOS_U32  DeviceId,
                       CBIOS_OUT PCBiosHDMIAudioFormat pAudioFormatList,
                       CBIOS_IN CBIOS_OUT CBIOS_U32 *pBufferSize)
{
    PCBIOS_EXTENSION_COMMON pcbe = pvcbe;
    PCBIOS_DEVICE_COMMON  pDevCommon = CBIOS_NULL;
    CBIOS_HDMI_AUDIO_INFO HDAFormatList[CBIOS_HDMI_AUDIO_FORMAT_COUNTS];
    CBIOS_U8 i = 0, HDAFormatNum = 0;

    if ((pBufferSize == CBIOS_NULL) || (pAudioFormatList == CBIOS_NULL) || (DeviceId == CBIOS_TYPE_NONE))
    {
        cbDebugPrint((MAKE_LEVEL(GENERIC, ERROR), "%s: the 2nd param is NULL!\n", FUNCTION_NAME));
        return CBIOS_ER_NULLPOINTER;
    }

    pDevCommon = pcbe->DeviceMgr.pDeviceArray[cbConvertDeviceBit2Index(DeviceId)];

    if(pDevCommon == CBIOS_NULL)
    {
        return CBIOS_ER_INVALID_PARAMETER;
    }

    HDAFormatNum = (CBIOS_U8)cbDevGetHDAFormatList(pcbe, pDevCommon, HDAFormatList);

    for (i = 0; i < HDAFormatNum; i++)
    {
        pAudioFormatList[i].Format = HDAFormatList[i].Format;
        pAudioFormatList[i].MaxChannelNum = HDAFormatList[i].MaxChannelNum;
        pAudioFormatList[i].SampleRateUnit = HDAFormatList[i].SampleRateUnit;
        pAudioFormatList[i].Unit = HDAFormatList[i].Unit;
    }
    *pBufferSize = HDAFormatNum * sizeof(CBiosHDMIAudioFormat);

    return CBIOS_OK;
}

DLLEXPORTS CBIOS_STATUS
CBiosGetHDMIAudioFomatListBufferSize(CBIOS_IN PCBIOS_VOID pvcbe,
                                  CBIOS_IN CBIOS_U32  DeviceId,
                                  CBIOS_OUT CBIOS_U32 * pBufferSize)
{
    PCBIOS_EXTENSION_COMMON pcbe = pvcbe;
    PCBIOS_DEVICE_COMMON  pDevCommon = CBIOS_NULL;
    CBIOS_U32 HDAFormatNum = 0;

    if (pBufferSize == CBIOS_NULL || DeviceId == CBIOS_TYPE_NONE)
    {
        cbDebugPrint((MAKE_LEVEL(GENERIC, ERROR), "%s: the 2nd param is NULL!\n", FUNCTION_NAME));
        return CBIOS_ER_INVALID_PARAMETER;
    }

    pDevCommon = pcbe->DeviceMgr.pDeviceArray[cbConvertDeviceBit2Index(DeviceId)];

    if(pDevCommon == CBIOS_NULL)
    {
        return CBIOS_ER_INVALID_PARAMETER;
    }

    HDAFormatNum = cbDevGetHDAFormatList(pcbe, pDevCommon, CBIOS_NULL);
    *pBufferSize = HDAFormatNum * sizeof(CBiosHDMIAudioFormat);
        
    return CBIOS_OK;
}


DLLEXPORTS CBIOS_STATUS
CBiosQueryMonitorAttribute(PCBIOS_VOID pvcbe, PCBiosMonitorAttribute pMonitorAttribute)
{
    PCBIOS_EXTENSION_COMMON pcbe = pvcbe;
    PCBIOS_DEVICE_COMMON  pDevCommon = CBIOS_NULL;

    if(pMonitorAttribute == CBIOS_NULL || pMonitorAttribute->ActiveDevId == CBIOS_TYPE_NONE)
    {
        cbDebugPrint((MAKE_LEVEL(GENERIC, ERROR), "%s: the 2nd param is CBIOS_NULL!\n", FUNCTION_NAME));
        return  CBIOS_ER_NULLPOINTER;
    }

    pDevCommon = pcbe->DeviceMgr.pDeviceArray[cbConvertDeviceBit2Index(pMonitorAttribute->ActiveDevId)];

    if(pDevCommon == CBIOS_NULL)
    {
        cbDebugPrint((MAKE_LEVEL(GENERIC, ERROR), "%s: pDevCommon is CBIOS_NULL!\n", FUNCTION_NAME));
        return  CBIOS_ER_NULLPOINTER;
    }

    return cbDevQueryMonitorAttribute(pcbe, pDevCommon, pMonitorAttribute);
}

DLLEXPORTS CBIOS_STATUS
CBiosQueryPortAttribute(PCBIOS_VOID pvcbe, PCBiosPortAttribute pPortAttribute)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON) pvcbe;
    PCBIOS_DEVICE_COMMON pDevCommon = CBIOS_NULL;

    if(pPortAttribute == CBIOS_NULL || pPortAttribute->DeviceId == CBIOS_TYPE_NONE)
    {
        cbDebugPrint((MAKE_LEVEL(GENERIC, ERROR), "%s: the 2nd param is CBIOS_NULL!\n", FUNCTION_NAME));
        return CBIOS_ER_NULLPOINTER;
    }

    pDevCommon = pcbe->DeviceMgr.pDeviceArray[cbConvertDeviceBit2Index(pPortAttribute->DeviceId)];

    if(pDevCommon == CBIOS_NULL)
    {
        cbDebugPrint((MAKE_LEVEL(GENERIC, ERROR), "%s: pDevCommon is CBIOS_NULL!\n", FUNCTION_NAME));
        return CBIOS_ER_NULLPOINTER;
    }

    pPortAttribute->PortConnType = pDevCommon->PortConnType;
    pPortAttribute->SupportMonitorType = pDevCommon->SupportMonitorType;
    
    return  CBIOS_OK;    
}

DLLEXPORTS CBIOS_STATUS
CBiosContentProtectionOnOff(PCBIOS_VOID pvcbe, PCBiosContentProtectionOnOffParams pContentProtectionOnOffParams)
{
    PCBIOS_EXTENSION_COMMON pcbe = pvcbe;
    PCBIOS_DEVICE_COMMON  pDevCommon = CBIOS_NULL;
    CBIOS_U32  i, DeviceId;

    if(pContentProtectionOnOffParams == CBIOS_NULL || pContentProtectionOnOffParams->DevicesId == CBIOS_TYPE_NONE)
    {
        cbDebugPrint((MAKE_LEVEL(GENERIC, ERROR), "%s: the 2nd param is CBIOS_NULL!\n", FUNCTION_NAME));
        return  CBIOS_ER_INVALID_PARAMETER;
    }

    DeviceId = pContentProtectionOnOffParams->DevicesId;
    pDevCommon = pcbe->DeviceMgr.pDeviceArray[cbConvertDeviceBit2Index(DeviceId)];

    for(i = 0; i < pcbe->DispMgr.IgaCount; i++)
    {
        if(DeviceId == pcbe->DispMgr.ActiveDevices[i])
        {
            break;
        }
    }

    if(pDevCommon == CBIOS_NULL || i == pcbe->DispMgr.IgaCount)
    {
        return  CBIOS_ER_INVALID_PARAMETER;
    }
    
    return cbDevContentProtectionOnOff(pcbe, pDevCommon, (CBIOS_U8)i, pContentProtectionOnOffParams);
}

DLLEXPORTS CBIOS_STATUS  
CBiosResetHWBlock(CBIOS_IN CBIOS_VOID* pvcbe, CBIOS_HW_BLOCK HWBlock)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    return cbHWResetBlock(pcbe, HWBlock);
}

CBIOS_STATUS
CBiosSetEdid(CBIOS_VOID* pvcbe, CBIOS_IN PCBIOS_PARAM_SET_EDID pCBParamSetEdid)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    PCBIOS_DEVICE_COMMON  pDevCommon = CBIOS_NULL;

    if(pCBParamSetEdid == CBIOS_NULL || pCBParamSetEdid->DeviceId  == CBIOS_TYPE_NONE)
    {
        cbDebugPrint((MAKE_LEVEL(GENERIC, ERROR), "%s: the 2nd param is CBIOS_NULL!\n", FUNCTION_NAME));
        return  CBIOS_ER_NULLPOINTER;
    }

    pDevCommon = pcbe->DeviceMgr.pDeviceArray[cbConvertDeviceBit2Index(pCBParamSetEdid->DeviceId)];

    if(pDevCommon == CBIOS_NULL)
    {
        cbDebugPrint((MAKE_LEVEL(GENERIC, ERROR), "%s: pDevCommon is CBIOS_NULL!\n", FUNCTION_NAME));
        return  CBIOS_ER_NULLPOINTER;
    }

    return cbDevSetEdid(pcbe, pDevCommon, pCBParamSetEdid);
}

CBIOS_STATUS
CBiosSetGamma(CBIOS_IN PCBIOS_VOID pvcbe, CBIOS_IN CBIOS_OUT PCBIOS_GAMMA_PARA pGammaParam)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    return cbSetGamma(pcbe, pGammaParam);
}

DLLEXPORTS CBIOS_STATUS
CBiosQueryMonitor3DCapability(CBIOS_IN CBIOS_VOID* pvcbe, CBIOS_IN CBIOS_OUT PCBIOS_MONITOR_3D_CAPABILITY_PARA p3DCapability)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    PCBIOS_DEVICE_COMMON  pDevCommon = CBIOS_NULL;

    if (p3DCapability == CBIOS_NULL || p3DCapability->DeviceId == CBIOS_TYPE_NONE)
    {
        cbDebugPrint((MAKE_LEVEL(GENERIC, ERROR), "%s: the 2nd param is CBIOS_NULL!\n", FUNCTION_NAME));
        return  CBIOS_ER_NULLPOINTER;
    }

    pDevCommon = pcbe->DeviceMgr.pDeviceArray[cbConvertDeviceBit2Index(p3DCapability->DeviceId)];

    if(pDevCommon == CBIOS_NULL)
    {
        cbDebugPrint((MAKE_LEVEL(GENERIC, ERROR), "%s: pDevCommon is CBIOS_NULL!\n", FUNCTION_NAME));
        return  CBIOS_ER_NULLPOINTER;
    }

    return cbDevQueryMonitor3DCapability(pcbe, pDevCommon, p3DCapability);
}

DLLEXPORTS CBIOS_STATUS
CBiosSetHDACodecPara(CBIOS_IN CBIOS_VOID* pvcbe, CBIOS_IN PCBIOS_HDAC_PARA pCbiosHDACPara)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    PCBIOS_DEVICE_COMMON  pDevCommon = CBIOS_NULL;

    if(pCbiosHDACPara == CBIOS_NULL || pCbiosHDACPara->DeviceId == CBIOS_TYPE_NONE)
    {
        cbDebugPrint((MAKE_LEVEL(GENERIC, ERROR), "%s: the 2nd param is NULL!\n", FUNCTION_NAME));
        return CBIOS_ER_INVALID_PARAMETER;
    }

    pDevCommon = pcbe->DeviceMgr.pDeviceArray[cbConvertDeviceBit2Index(pCbiosHDACPara->DeviceId)];

    if(pDevCommon == CBIOS_NULL)
    {
        return  CBIOS_ER_INVALID_PARAMETER;
    }

    return cbDevSetHDACodecPara(pcbe, pDevCommon, pCbiosHDACPara);
}

DLLEXPORTS CBIOS_STATUS
CBiosSetHDACConnectStatus(CBIOS_IN CBIOS_VOID* pvcbe, CBIOS_IN PCBIOS_HDAC_PARA pCbiosHDACPara)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    PCBIOS_DEVICE_COMMON  pDevCommon = CBIOS_NULL;

    if(pCbiosHDACPara == CBIOS_NULL || pCbiosHDACPara->DeviceId == CBIOS_TYPE_NONE)
    {
        cbDebugPrint((MAKE_LEVEL(GENERIC, ERROR), "%s: the 2nd param is NULL!\n", FUNCTION_NAME));
        return CBIOS_ER_INVALID_PARAMETER;
    }

    pDevCommon = pcbe->DeviceMgr.pDeviceArray[cbConvertDeviceBit2Index(pCbiosHDACPara->DeviceId)];

    if(pDevCommon == CBIOS_NULL)
    {
        return  CBIOS_ER_INVALID_PARAMETER;
    }
    
    return cbDevSetHDACConnectStatus(pcbe, pDevCommon, pCbiosHDACPara);
}

DLLEXPORTS CBIOS_STATUS 
CBiosCECEnableDisable(CBIOS_IN CBIOS_VOID* pvcbe, CBIOS_IN PCBIOS_CEC_ENABLE_DISABLE_PARA pCECEnableDisablePara)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    CBIOS_U32 Status = CBIOS_OK;
    Status = cbCECEnableDisable(pcbe, pCECEnableDisablePara);
    return Status;
}

DLLEXPORTS CBIOS_STATUS 
CBiosCECTransmitMessage(CBIOS_IN CBIOS_VOID* pvcbe, CBIOS_IN PCBIOS_CEC_TRANSIMIT_MESSAGE_PARA pCECPara)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    CBIOS_U32 Status = CBIOS_OK;
    CBIOS_CEC_MESSAGE   CECMessage;

    if (pCECPara == CBIOS_NULL)
    {
        Status = CBIOS_ER_NULLPOINTER;
        cbDebugPrint((MAKE_LEVEL(HDMI, ERROR), "CBiosCECTransmitMessage: pCECPara is NULL!"));
    }
    else if (!pcbe->ChipCaps.IsSupportCEC)
    {
        Status = CBIOS_ER_HARDWARE_LIMITATION;
        cbDebugPrint((MAKE_LEVEL(HDMI, ERROR), "CBiosCECTransmitMessage: Can't support CEC!"));
    }
    else if (pCECPara->CECIndex >= CBIOS_CEC_INDEX_COUNT)
    {
        Status = CBIOS_ER_INVALID_PARAMETER;
        cbDebugPrint((MAKE_LEVEL(HDMI, ERROR), "CBiosCECTransmitMessage: invalid CEC index!"));
    }
    else if (!pcbe->CECPara[pCECPara->CECIndex].CECEnable)
    {
        Status = CBIOS_ER_INVALID_PARAMETER;
        cbDebugPrint((MAKE_LEVEL(HDMI, ERROR), "CBiosCECTransmitMessage: CEC module is not enabled!"));
    }
    else
    {
        cb_memset(&CECMessage, 0, sizeof(CBIOS_CEC_MESSAGE));

        CECMessage.SourceAddr = pcbe->CECPara[pCECPara->CECIndex].LogicalAddr;
        CECMessage.DestAddr = pCECPara->DestAddr;
        CECMessage.CmdLen = pCECPara->CmdLen;
        if (CECMessage.CmdLen > 16)
        {
            CECMessage.CmdLen = 16;
        }
        cb_memcpy(CECMessage.Command, pCECPara->Command, CECMessage.CmdLen);
        CECMessage.bBroadcast = pCECPara->bBroadcast;
        CECMessage.RetryCnt = pCECPara->RetryCnt;

        Status = cbHWCECTransmitMessage(pcbe, &CECMessage, pCECPara->CECIndex);
    }

    return Status;
}

DLLEXPORTS CBIOS_STATUS 
CBiosCECReceiveMessage(CBIOS_IN CBIOS_VOID* pvcbe, CBIOS_IN PCBIOS_CEC_RECEIVE_MESSAGE_PARA pCECPara)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    CBIOS_U32 Status = CBIOS_OK;
    CBIOS_CEC_MESSAGE   CECMessage;

    if (pCECPara == CBIOS_NULL)
    {
        Status = CBIOS_ER_NULLPOINTER;
        cbDebugPrint((MAKE_LEVEL(HDMI, ERROR), "CBiosCECReceiveMessage: pCECPara is NULL!"));
    }
    else if (!pcbe->ChipCaps.IsSupportCEC)
    {
        Status = CBIOS_ER_HARDWARE_LIMITATION;
        cbDebugPrint((MAKE_LEVEL(HDMI, ERROR), "CBiosCECReceiveMessage: Can't support CEC!"));
    }
    else if (pCECPara->CECIndex >= CBIOS_CEC_INDEX_COUNT)
    {
        Status = CBIOS_ER_INVALID_PARAMETER;
        cbDebugPrint((MAKE_LEVEL(HDMI, ERROR), "CBiosCECReceiveMessage: invalid CEC index!"));
    }
    else if (!pcbe->CECPara[pCECPara->CECIndex].CECEnable)
    {
        Status = CBIOS_ER_INVALID_PARAMETER;
        cbDebugPrint((MAKE_LEVEL(HDMI, ERROR), "CBiosCECReceiveMessage: CEC module is not enabled!"));
    }
    else
    {
        cb_memset(&CECMessage, 0, sizeof(CBIOS_CEC_MESSAGE));

        Status = cbHWCECReceiveMessage(pcbe, &CECMessage, pCECPara->CECIndex);

        pCECPara->SourceAddr = CECMessage.SourceAddr;
        pCECPara->CmdLen = CECMessage.CmdLen;
        cb_memcpy(pCECPara->Command, CECMessage.Command, CECMessage.CmdLen);
        pCECPara->bBroadcast = CECMessage.bBroadcast;

    }
    return Status;
}

DLLEXPORTS CBIOS_STATUS 
CBiosCECGetInfo(CBIOS_IN CBIOS_VOID* pvcbe, CBIOS_OUT PCBIOS_CEC_INFO pCECInfo)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    CBIOS_U32 Status = CBIOS_OK;

    if (pCECInfo == CBIOS_NULL)
    {
        Status = CBIOS_ER_NULLPOINTER;
        cbDebugPrint((MAKE_LEVEL(HDMI, ERROR), "CBiosCECGetInfo: pCECInfo is NULL!"));
    }
    else if (!pcbe->ChipCaps.IsSupportCEC)
    {
        Status = CBIOS_ER_HARDWARE_LIMITATION;
        cbDebugPrint((MAKE_LEVEL(HDMI, ERROR), "CBiosCECReceiveMessage: Can't support CEC!"));
    }
    else if (pCECInfo->CECIndex >= CBIOS_CEC_INDEX_COUNT)
    {
        Status = CBIOS_ER_INVALID_PARAMETER;
        cbDebugPrint((MAKE_LEVEL(HDMI, ERROR), "CBiosCECReceiveMessage: invalid CEC index!"));
    }
    else if (!pcbe->CECPara[pCECInfo->CECIndex].CECEnable)
    {
        Status = CBIOS_ER_INVALID_PARAMETER;
        cbDebugPrint((MAKE_LEVEL(HDMI, ERROR), "CBiosCECReceiveMessage: CEC module is not enabled!"));
    }
    else
    {
        pCECInfo->PhysicalAddr = pcbe->CECPara[pCECInfo->CECIndex].PhysicalAddr;
        pCECInfo->LogicalAddr = pcbe->CECPara[pCECInfo->CECIndex].LogicalAddr;
    }

    return Status;
}

DLLEXPORTS CBIOS_STATUS 
CBiosGetCECInterruptInfo(CBIOS_IN CBIOS_VOID* pvcbe, CBIOS_IN PCBIOS_CEC_INTERRUPT_INFO pCECIntInfo)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    CBIOS_U32 Status = CBIOS_OK;
    Status = cbGetCECInterruptInfo(pcbe, pCECIntInfo);
    return Status;
}

DLLEXPORTS  CBIOS_STATUS
CBiosGetDevComb(CBIOS_IN PCBIOS_VOID pvcbe, CBIOS_IN CBIOS_OUT PCBIOS_GET_DEV_COMB pGetDevComb)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON) pvcbe;
    CBIOS_STATUS    status;
    
    if(pGetDevComb != CBIOS_NULL && pGetDevComb->pDeviceComb != CBIOS_NULL)
    {
        status = cbPathMgrGetDevComb(pcbe, pGetDevComb);
    }
    else
    {
        cbDebugPrint((MAKE_LEVEL(GENERIC, ERROR), "CBiosGetDevComb: pGetDevComb is CBIOS_NULL!\n"));
        status = CBIOS_ER_NULLPOINTER;
    }
    return  status;
}

DLLEXPORTS CBIOS_STATUS
CBiosGetIgaMask(CBIOS_IN PCBIOS_VOID pvcbe, CBIOS_IN CBIOS_OUT PCBIOS_GET_IGA_MASK  pGetIgaMask)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON) pvcbe;
    CBIOS_STATUS    status;
    
    if(pGetIgaMask != CBIOS_NULL)
    {
        status = cbPathMgrGetIgaMask(pcbe, pGetIgaMask);
    }
    else
    {
        cbDebugPrint((MAKE_LEVEL(GENERIC, ERROR), "CBiosGetIgaMask: pGetIgaMask is CBIOS_NULL!\n"));
        status = CBIOS_ER_NULLPOINTER;
    }
    return  status;
}

DLLEXPORTS CBIOS_STATUS
CBiosGetDeviceName(PCBIOS_VOID pvcbe, PCBIOS_GET_DEVICE_NAME  pGetName)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON) pvcbe;
    CBIOS_STATUS    status;

    status = cbGetDeviceName(pcbe, pGetName);

    return  status;
}

DLLEXPORTS CBIOS_STATUS
CBiosGetClock(PCBIOS_VOID pvcbe, PCBios_GetClock_Params pCBiosGetClockParams)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    CBIOS_U32 Status = CBIOS_OK;
    Status = cbHWGetClockByType(pcbe, pCBiosGetClockParams);
    return Status;
}

DLLEXPORTS CBIOS_STATUS
CBiosSetClock(PCBIOS_VOID pvcbe, PCBios_SetClock_Params pCBiosSetClockParams)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    CBIOS_U32 Status = CBIOS_OK;
    Status = cbHWSetClockByType(pcbe, pCBiosSetClockParams);
    return Status;
}

DLLEXPORTS CBIOS_STATUS
CBiosGetHDCPStatus(PCBIOS_VOID pvcbe, PCBIOS_HDCP_STATUS_PARA pCBiosHdcpStatusParams)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON) pvcbe;
    PCBIOS_DEVICE_COMMON  pDevCommon = CBIOS_NULL;
    CBIOS_U32  DeviceId;

    if(pCBiosHdcpStatusParams == CBIOS_NULL || pCBiosHdcpStatusParams->DevicesId == CBIOS_TYPE_NONE)
    {
        cbDebugPrint((MAKE_LEVEL(GENERIC, ERROR), "%s: the 2nd param is CBIOS_NULL!\n", FUNCTION_NAME));
        return  CBIOS_ER_INVALID_PARAMETER;
    }

    DeviceId = pCBiosHdcpStatusParams->DevicesId;
    pDevCommon = pcbe->DeviceMgr.pDeviceArray[cbConvertDeviceBit2Index(DeviceId)];

    if(pDevCommon == CBIOS_NULL)
    {
        return  CBIOS_ER_INVALID_PARAMETER;
    }

    return  cbDevGetHDCPStatus(pcbe, pDevCommon, pCBiosHdcpStatusParams);
}

DLLEXPORTS CBIOS_STATUS
CBiosHDCPWorkThread(PCBIOS_VOID pvcbe, PCBIOS_HDCP_WORK_PARA pCBiosHdcpWorkParams)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON) pvcbe;
    PCBIOS_DEVICE_COMMON  pDevCommon = CBIOS_NULL;
    CBIOS_U32  i, DeviceId;

    if(pCBiosHdcpWorkParams == CBIOS_NULL || pCBiosHdcpWorkParams->DevicesId == CBIOS_TYPE_NONE)
    {
        cbDebugPrint((MAKE_LEVEL(GENERIC, ERROR), "%s: the 2nd param is NULL!\n", FUNCTION_NAME));
        return  CBIOS_ER_INVALID_PARAMETER;
    }

    DeviceId = pCBiosHdcpWorkParams->DevicesId;
    pDevCommon = pcbe->DeviceMgr.pDeviceArray[cbConvertDeviceBit2Index(DeviceId)];

    for(i = 0; i < pcbe->DispMgr.IgaCount; i++)
    {
        if(DeviceId == pcbe->DispMgr.ActiveDevices[i])
        {
            break;
        }
    }

    if(pDevCommon == CBIOS_NULL || i == pcbe->DispMgr.IgaCount)
    {
        return  CBIOS_ER_INVALID_PARAMETER;
    }

    return  cbDevHDCPWorkThread(pcbe, pDevCommon, (CBIOS_U8)i, pCBiosHdcpWorkParams);
}

DLLEXPORTS CBIOS_STATUS
CBiosHDCPIsr(PCBIOS_VOID pvcbe, PCBIOS_HDCP_ISR_PARA pHdcpIsrParam)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    PCBIOS_DEVICE_COMMON pDevCommon = CBIOS_NULL;

    if(pHdcpIsrParam == CBIOS_NULL || pHdcpIsrParam->DevicesId == CBIOS_TYPE_NONE)
    {
        cbDebugPrint((MAKE_LEVEL(GENERIC, ERROR), "%s: the 2nd param is NULL!\n", FUNCTION_NAME));
        return  CBIOS_ER_INVALID_PARAMETER;
    }

    pDevCommon = pcbe->DeviceMgr.pDeviceArray[cbConvertDeviceBit2Index(pHdcpIsrParam->DevicesId)];
    if(pDevCommon == CBIOS_NULL)
    {
        return  CBIOS_ER_INVALID_PARAMETER;
    }
    
    return  cbDevHDCPIsr(pcbe, pDevCommon, pHdcpIsrParam);
}

DLLEXPORTS CBIOS_STATUS
CBiosGetHDCPInterruptInfo(PCBIOS_VOID pvcbe, PCBIOS_HDCP_INFO_PARA pHdcpInfoParam)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    CBIOS_STATUS Status;
    Status = cbGetHDCPInterruptInfo(pcbe, pHdcpInfoParam);
    return Status;
}

DLLEXPORTS CBIOS_STATUS
CBiosGetHDACInterruptInfo(PCBIOS_VOID pvcbe, PCBIOS_HDAC_INFO_PARA pHdacInfoParam)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    CBIOS_STATUS Status;
    Status = cbGetHDACInterruptInfo(pcbe, pHdacInfoParam);
    return Status;
}

 DLLEXPORTS CBIOS_STATUS
CBiosGetDPIntType(CBIOS_IN PCBIOS_VOID pvcbe, CBIOS_IN PCBIOS_DP_INT_PARA pDPIntPara) 
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    PCBIOS_DEVICE_COMMON  pDevCommon = CBIOS_NULL;

    if(pDPIntPara == CBIOS_NULL || pDPIntPara->DeviceId == CBIOS_TYPE_NONE)
    {
        cbDebugPrint((MAKE_LEVEL(GENERIC, ERROR), "%s: the 2nd param is NULL!\n", FUNCTION_NAME));
        return  CBIOS_ER_INVALID_PARAMETER;
    }

    pDevCommon = pcbe->DeviceMgr.pDeviceArray[cbConvertDeviceBit2Index(pDPIntPara->DeviceId)];
    if(pDevCommon == CBIOS_NULL)
    {
        return  CBIOS_ER_INVALID_PARAMETER;
    }

    return cbDevGetDPIntType(pcbe, pDevCommon, pDPIntPara);
}

DLLEXPORTS CBIOS_STATUS
CBiosHandleDPIrq(CBIOS_IN PCBIOS_VOID pvcbe, CBIOS_IN PCBIOS_DP_HANDLE_IRQ_PARA pDPHandleIrqPara)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    PCBIOS_DEVICE_COMMON  pDevCommon = CBIOS_NULL;

    if(pDPHandleIrqPara == CBIOS_NULL || pDPHandleIrqPara->DeviceId == CBIOS_TYPE_NONE)
    {
        cbDebugPrint((MAKE_LEVEL(GENERIC, ERROR), "%s: the 2nd param is NULL!\n", FUNCTION_NAME));
        return  CBIOS_ER_INVALID_PARAMETER;
    }

    pDevCommon = pcbe->DeviceMgr.pDeviceArray[cbConvertDeviceBit2Index(pDPHandleIrqPara->DeviceId)];
    if(pDevCommon == CBIOS_NULL)
    {
        return CBIOS_ER_INVALID_PARAMETER;
    }

    return  cbDevDPHandleIrq(pcbe, pDevCommon, pDPHandleIrqPara);    
}

DLLEXPORTS CBIOS_STATUS
CBiosDumpInfo(PCBIOS_VOID pvcbe, PCBIOS_DUMP_PARA pDumpPara)
{
    PCBIOS_EXTENSION_COMMON pcbe = pvcbe;
    CBIOS_STATUS status = CBIOS_ER_INTERNAL;

    if (pDumpPara != CBIOS_NULL)
    {
        if (cbHWDumpInfo(pcbe, pDumpPara->DumpType))
        {
            status = CBIOS_OK;
        }
        else
        {
            status = CBIOS_ER_INTERNAL;
        }
    }
    else 
    {
        status = CBIOS_ER_NULLPOINTER;
        cbDebugPrint((MAKE_LEVEL(GENERIC, ERROR), "%s: pDumpPara is NULL\n", FUNCTION_NAME));
    }

    return status;
}

DLLEXPORTS CBIOS_STATUS
CBiosDPGetCustomizedTiming(CBIOS_IN PCBIOS_VOID pvcbe, CBIOS_IN PCBIOS_DP_CUSTOMIZED_TIMING pDPCustomizedTiming)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    PCBIOS_DEVICE_COMMON  pDevCommon = CBIOS_NULL;

    if(pDPCustomizedTiming == CBIOS_NULL || pDPCustomizedTiming->DeviceId == CBIOS_TYPE_NONE)
    {
        cbDebugPrint((MAKE_LEVEL(GENERIC, ERROR), "%s: the 2nd param is NULL!\n", FUNCTION_NAME));
        return  CBIOS_ER_INVALID_PARAMETER;
    }

    pDevCommon = pcbe->DeviceMgr.pDeviceArray[cbConvertDeviceBit2Index(pDPCustomizedTiming->DeviceId)];
    if(pDevCommon == CBIOS_NULL)
    {
        return  CBIOS_ER_INVALID_PARAMETER;
    }

    return  cbDevDPGetCustomizedTiming(pcbe, pDevCommon, pDPCustomizedTiming);
}

DLLEXPORTS CBIOS_STATUS
CBiosDPIsr(CBIOS_IN PCBIOS_VOID pvcbe, CBIOS_IN PCBIOS_DP_ISR_PARA pDPIsrPara)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    PCBIOS_DEVICE_COMMON  pDevCommon = CBIOS_NULL;

    if (pDPIsrPara == CBIOS_NULL || pDPIsrPara->DeviceId == CBIOS_TYPE_NONE)
    {
        cbDebugPrint((MAKE_LEVEL(GENERIC, ERROR), "%s: the 2nd param is NULL!\n", FUNCTION_NAME));
        return CBIOS_ER_INVALID_PARAMETER;
    } 

    pDevCommon = pcbe->DeviceMgr.pDeviceArray[cbConvertDeviceBit2Index(pDPIsrPara->DeviceId)];

    return  cbDevDPIsr(pcbe, pDevCommon, pDPIsrPara);
}

DLLEXPORTS CBIOS_BOOL
CBiosDPWorkThreadMainFunc(CBIOS_IN PCBIOS_VOID pvcbe, CBIOS_IN PCBIOS_DP_WORKTHREAD_PARA pDPWorkThreadPara) 
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    PCBIOS_DEVICE_COMMON  pDevCommon = CBIOS_NULL;
    
    if (pDPWorkThreadPara == CBIOS_NULL || pDPWorkThreadPara->DeviceId == CBIOS_TYPE_NONE)
    {
        cbDebugPrint((MAKE_LEVEL(GENERIC, ERROR), "%s: the 2nd param is NULL!\n", FUNCTION_NAME));
        return CBIOS_FALSE;
    } 

    pDevCommon = pcbe->DeviceMgr.pDeviceArray[cbConvertDeviceBit2Index(pDPWorkThreadPara->DeviceId)];
    
    return cbDevDPWorkThreadMainFunc(pcbe, pDevCommon, pDPWorkThreadPara);
}

DLLEXPORTS CBIOS_STATUS
CBiosDPSetNotifications(CBIOS_IN PCBIOS_VOID pvcbe, CBIOS_IN PCBIOS_DP_NOTIFICATIONS pDPNotifications) 
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    PCBIOS_DEVICE_COMMON  pDevCommon = CBIOS_NULL;
    
    if (pDPNotifications == CBIOS_NULL || pDPNotifications->DeviceId == CBIOS_TYPE_NONE) 
    {
        cbDebugPrint((MAKE_LEVEL(GENERIC, ERROR), "%s: the 2nd param is NULL!\n", FUNCTION_NAME));
        return  CBIOS_ER_INVALID_PARAMETER;
    } 

    pDevCommon = pcbe->DeviceMgr.pDeviceArray[cbConvertDeviceBit2Index(pDPNotifications->DeviceId)];
    
    return cbDevDPSetNotifications(pcbe, pDevCommon, pDPNotifications);
}

DLLEXPORTS CBIOS_STATUS
CBiosWaitVBlank(CBIOS_IN PCBIOS_VOID pvcbe, CBIOS_IN CBIOS_U32 IGAIndex)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;

    return (CBIOS_TRUE == cbWaitVBlank(pcbe, (CBIOS_U8)IGAIndex)) ? CBIOS_OK : CBIOS_ER_INVALID_PARAMETER;
}

DLLEXPORTS CBIOS_STATUS
CBiosVIPCtl(PCBIOS_VOID pvcbe, PCBIOS_VIP_CTRL_DATA pCbiosVIPCtlData)
{
    PCBIOS_EXTENSION_COMMON pcbe = pvcbe;
    CBIOS_STATUS status = CBIOS_ER_INTERNAL;

    status = cbHWVIPCtl(pcbe, pCbiosVIPCtlData);

    return status;
}

DLLEXPORTS CBIOS_STATUS
CBiosGetSliceNum(CBIOS_IN PCBIOS_VOID pvcbe, CBIOS_OUT PCBIOS_U8 pSliceNum, CBIOS_OUT PCBIOS_U32 pGPCReg)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    CBIOS_STATUS status = CBIOS_ER_INTERNAL;
    CBIOS_U32 RegValue, i;

    *pSliceNum = pcbe->SliceNum;

    /* read GPC register value from efuse
       step 1: write address 0x70 to MM82B8[22:16]
       step 2: Using A_READ_MODE and Set Efuse_Read_Request bit in MM82C0[25] to start request.
               HW will clear this bit after data of EUFSE has been put on MM82BC[31:0]
       step 3: Read data from MM82BC[31:0]
    */

    cbMMIOWriteReg32(pcbe, 0x82B8, 0x700000, 0xFF80FFFF);
    cbMMIOWriteReg32(pcbe, 0x82C0, 0x22000000, 0x81FFFFFF);
    for (i = 0; i < 10; i++)
    {
        RegValue = cb_ReadU32(pcbe->pAdapterContext, 0x82C0);
        if ((RegValue & 0x2000000) == 0)
        {
            status = CBIOS_OK;
            break;
        }
        cb_DelayMicroSeconds(10);
    }

    if (status == CBIOS_OK)
    {
        *pGPCReg = cb_ReadU32(pcbe->pAdapterContext, 0x82BC);
    }
    else
    {
        cbDebugPrint((MAKE_LEVEL(GENERIC, ERROR), "%s: read EFUSE GPC register failed!\n", FUNCTION_NAME));
    }

    return status;
}

DLLEXPORTS CBIOS_STATUS
CBiosSetHDRInfo(CBIOS_IN PCBIOS_VOID pvcbe, CBIOS_IN PCBIOS_HDR_PARA pHDRPara)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    PCBIOS_DEVICE_COMMON    pDevCommon = CBIOS_NULL;
    
    if (pHDRPara == CBIOS_NULL || pHDRPara->DeviceId == CBIOS_TYPE_NONE) 
    {
        cbDebugPrint((MAKE_LEVEL(GENERIC, ERROR), "%s: the 2nd param is NULL!\n", FUNCTION_NAME));
        return  CBIOS_ER_INVALID_PARAMETER;
    } 

    pDevCommon = pcbe->DeviceMgr.pDeviceArray[cbConvertDeviceBit2Index(pHDRPara->DeviceId)];
    
    cbHDMIMonitor_SetHDRInfo(pcbe, pDevCommon, pHDRPara);

    return CBIOS_OK;
}

DLLEXPORTS CBIOS_BOOL
CBiosPsrOp0_1(CBIOS_IN CBIOS_VOID* pvcbe, CBIOS_IN CBIOS_U32 DeviceID)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;

    if(DeviceID == CBIOS_TYPE_NONE)
    {
        cbDebugPrint((MAKE_LEVEL(GENERIC, ERROR),"%s: DeviceID is 0!\n", FUNCTION_NAME));
        return CBIOS_FALSE;
    }

    return cbDevPsrOp0_1(pcbe, DeviceID);
}

DLLEXPORTS CBIOS_STATUS
CBiosPsrOp1_0(CBIOS_IN CBIOS_VOID* pvcbe, CBIOS_IN CBIOS_U32 DeviceID)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;

    if(DeviceID == CBIOS_TYPE_NONE)
    {
        cbDebugPrint((MAKE_LEVEL(GENERIC, ERROR),"%s: DeviceID is 0!\n", FUNCTION_NAME));
        return CBIOS_ER_INVALID_PARAMETER;
    }

    return cbDevPsrOp1_0(pcbe, DeviceID);
}

DLLEXPORTS CBIOS_STATUS
CBiosPsrOp1_2(CBIOS_IN CBIOS_VOID* pvcbe, CBIOS_IN CBIOS_U32 DeviceID)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;

    if(DeviceID == CBIOS_TYPE_NONE)
    {
        cbDebugPrint((MAKE_LEVEL(GENERIC, ERROR),"%s: DeviceID is 0!\n", FUNCTION_NAME));
        return CBIOS_ER_INVALID_PARAMETER;
    }

    return cbDevPsrOp1_2(pcbe, DeviceID);
}

DLLEXPORTS CBIOS_STATUS
CBiosPsrOp2_0(CBIOS_IN CBIOS_VOID* pvcbe, CBIOS_IN CBIOS_U32 DeviceID)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;

    if(DeviceID == CBIOS_TYPE_NONE)
    {
        cbDebugPrint((MAKE_LEVEL(GENERIC, ERROR),"%s: DeviceID is 0!\n", FUNCTION_NAME));
        return CBIOS_ER_INVALID_PARAMETER;
    }

    return cbDevPsrOp2_0(pcbe, DeviceID);
}

DLLEXPORTS CBIOS_BOOL
CBiosPsrOp2_3(CBIOS_IN CBIOS_VOID* pvcbe, CBIOS_IN CBIOS_U32 DeviceID)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;

    if(DeviceID == CBIOS_TYPE_NONE)
    {
        cbDebugPrint((MAKE_LEVEL(GENERIC, ERROR),"%s: DeviceID is 0!\n", FUNCTION_NAME));
        return CBIOS_FALSE;
    }

    return cbDevPsrOp2_3(pcbe, DeviceID);
}

DLLEXPORTS CBIOS_STATUS
CBiosPsrOp2_5(CBIOS_IN CBIOS_VOID* pvcbe, CBIOS_IN CBIOS_U32 DeviceID)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;

    if(DeviceID == CBIOS_TYPE_NONE)
    {
        cbDebugPrint((MAKE_LEVEL(GENERIC, ERROR),"%s: DeviceID is 0!\n", FUNCTION_NAME));
        return CBIOS_ER_INVALID_PARAMETER;
    }

    return cbDevPsrOp2_5(pcbe, DeviceID);
}

DLLEXPORTS CBIOS_STATUS
CBiosPsrOp3_0(CBIOS_IN CBIOS_VOID* pvcbe, CBIOS_IN CBIOS_U32 DeviceID)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;

    if(DeviceID == CBIOS_TYPE_NONE)
    {
        cbDebugPrint((MAKE_LEVEL(GENERIC, ERROR),"%s: DeviceID is 0!\n", FUNCTION_NAME));
        return CBIOS_ER_INVALID_PARAMETER;
    }

    return cbDevPsrOp3_0(pcbe, DeviceID);
}

DLLEXPORTS CBIOS_STATUS
CBiosPsrOp3_1(CBIOS_IN CBIOS_VOID* pvcbe, CBIOS_IN CBIOS_U32 DeviceID)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;

    if(DeviceID == CBIOS_TYPE_NONE)
    {
        cbDebugPrint((MAKE_LEVEL(GENERIC, ERROR),"%s: DeviceID is 0!\n", FUNCTION_NAME));
        return CBIOS_ER_INVALID_PARAMETER;
    }

    return cbDevPsrOp3_1(pcbe, DeviceID);
}

DLLEXPORTS CBIOS_STATUS
CBiosPsrOp3_5(CBIOS_IN CBIOS_VOID* pvcbe, CBIOS_IN CBIOS_U32 DeviceID)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;

    if(DeviceID == CBIOS_TYPE_NONE)
    {
        cbDebugPrint((MAKE_LEVEL(GENERIC, ERROR),"%s: DeviceID is 0!\n", FUNCTION_NAME));
        return CBIOS_ER_INVALID_PARAMETER;
    }

    return cbDevPsrOp3_5(pcbe, DeviceID);
}

DLLEXPORTS CBIOS_STATUS
CBiosPsrOp5_0(CBIOS_IN CBIOS_VOID* pvcbe, CBIOS_IN CBIOS_U32 DeviceID)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;

    if(DeviceID == CBIOS_TYPE_NONE)
    {
        cbDebugPrint((MAKE_LEVEL(GENERIC, ERROR),"%s: DeviceID is 0!\n", FUNCTION_NAME));
        return CBIOS_ER_INVALID_PARAMETER;
    }

    return cbDevPsrOp5_0(pcbe, DeviceID);
}

DLLEXPORTS CBIOS_BOOL
CBiosPsrOp5_1(CBIOS_IN CBIOS_VOID* pvcbe, CBIOS_IN CBIOS_U32 DeviceID)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;

    if(DeviceID == CBIOS_TYPE_NONE)
    {
        cbDebugPrint((MAKE_LEVEL(GENERIC, ERROR),"%s: DeviceID is 0!\n", FUNCTION_NAME));
        return CBIOS_FALSE;
    }

    return cbDevPsrOp5_1(pcbe, DeviceID);
}

DLLEXPORTS CBIOS_STATUS
CBiosPsrOp3_3_1(CBIOS_IN CBIOS_VOID* pvcbe, CBIOS_IN CBIOS_U32 DeviceID)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;

    if(DeviceID == CBIOS_TYPE_NONE)
    {
        cbDebugPrint((MAKE_LEVEL(GENERIC, ERROR),"%s: DeviceID is 0!\n", FUNCTION_NAME));
        return CBIOS_ER_INVALID_PARAMETER;
    }

    return cbDevPsrOp3_3_1(pcbe, DeviceID);
}

DLLEXPORTS CBIOS_STATUS
CBiosPsrOp3_1_3_2(CBIOS_IN CBIOS_VOID* pvcbe, CBIOS_IN CBIOS_U32 DeviceID)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;

    if(DeviceID == CBIOS_TYPE_NONE)
    {
        cbDebugPrint((MAKE_LEVEL(GENERIC, ERROR),"%s: DeviceID is 0!\n", FUNCTION_NAME));
        return CBIOS_ER_INVALID_PARAMETER;
    }

    return cbDevPsrOp3_1_3_2(pcbe, DeviceID);
}

DLLEXPORTS CBIOS_STATUS
CBiosPsrOp3_2_3_3(CBIOS_IN CBIOS_VOID* pvcbe, CBIOS_IN CBIOS_U32 DeviceID)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;

    if(DeviceID == CBIOS_TYPE_NONE)
    {
        cbDebugPrint((MAKE_LEVEL(GENERIC, ERROR),"%s: DeviceID is 0!\n", FUNCTION_NAME));
        return CBIOS_ER_INVALID_PARAMETER;
    }

    return cbDevPsrOp3_2_3_3(pcbe, DeviceID);
}

DLLEXPORTS CBIOS_STATUS
CBiosPsrOp3_3_3_2(CBIOS_IN CBIOS_VOID* pvcbe, CBIOS_IN CBIOS_U32 DeviceID)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;

    if(DeviceID == CBIOS_TYPE_NONE)
    {
        cbDebugPrint((MAKE_LEVEL(GENERIC, ERROR),"%s: DeviceID is 0!\n", FUNCTION_NAME));
        return CBIOS_ER_INVALID_PARAMETER;
    }

    return cbDevPsrOp3_3_3_2(pcbe, DeviceID);
}

DLLEXPORTS CBIOS_STATUS
CBiosPsrOp3_2_3_1(CBIOS_IN CBIOS_VOID* pvcbe, CBIOS_IN CBIOS_U32 DeviceID)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;

    if(DeviceID == CBIOS_TYPE_NONE)
    {
        cbDebugPrint((MAKE_LEVEL(GENERIC, ERROR),"%s: DeviceID is 0!\n", FUNCTION_NAME));
        return CBIOS_ER_INVALID_PARAMETER;
    }

    return cbDevPsrOp3_2_3_1(pcbe, DeviceID);
}

DLLEXPORTS CBIOS_STATUS
CBiosPsrOp3_1_3(CBIOS_IN CBIOS_VOID* pvcbe, CBIOS_IN CBIOS_U32 DeviceID)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;

    if(DeviceID == CBIOS_TYPE_NONE)
    {
        cbDebugPrint((MAKE_LEVEL(GENERIC, ERROR),"%s: DeviceID is 0!\n", FUNCTION_NAME));
        return CBIOS_ER_INVALID_PARAMETER;
    }

    return cbDevPsrOp3_1_3(pcbe, DeviceID);
}

DLLEXPORTS CBIOS_STATUS
CBiosAuxDataAccess(PCBIOS_VOID pvcbe, PCBIOS_PARAM_AUX_DATA pCBParamAuxData)
{
    PCBIOS_EXTENSION_COMMON pcbe = pvcbe;
    CBIOS_U32 Status = CBIOS_OK;

    if (pCBParamAuxData == CBIOS_NULL)
    {
        cbDebugPrint((MAKE_LEVEL(GENERIC, ERROR), "%s: pCBParamAuxData is NULL!\n", FUNCTION_NAME));
        return CBIOS_ER_NULLPOINTER;
    }
    if (pCBParamAuxData->Buffer == CBIOS_NULL)
    {
        cbDebugPrint((MAKE_LEVEL(GENERIC, ERROR), "%s: Aux Data Buffer is NULL!\n", FUNCTION_NAME));
        return CBIOS_ER_NULLPOINTER;
    }
    if(pCBParamAuxData->DeviceId == CBIOS_TYPE_NONE)
    {
        cbDebugPrint((MAKE_LEVEL(GENERIC, ERROR),"%s: pCBParamAuxData DeviceId is 0!\n", FUNCTION_NAME));
        return CBIOS_ER_INVALID_PARAMETER;
    }

    Status = cbDevAux_DataAccess(pcbe, pCBParamAuxData);

    if(Status == CBIOS_OK)
    {
        pCBParamAuxData->BytesDone = pCBParamAuxData->BytesRequested;
    }
    else
    {
        pCBParamAuxData->BytesDone = 0;
    }

    return Status;
}

DLLEXPORTS CBIOS_STATUS
CBiosSetBacklight(CBIOS_IN PCBIOS_VOID pvcbe, CBIOS_IN PCBIOS_BACKLIGHT_PARA pBacklightPara)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    PCBIOS_DEVICE_COMMON  pDevCommon = CBIOS_NULL;
    CBIOS_U32 Status = CBIOS_OK;

    if(pBacklightPara == CBIOS_NULL || pBacklightPara->DeviceId == CBIOS_TYPE_NONE)
    {
        cbDebugPrint((MAKE_LEVEL(GENERIC, ERROR), "%s: the 2nd param is CBIOS_NULL!\n", FUNCTION_NAME));
        return  CBIOS_ER_NULLPOINTER;
    }

    pDevCommon = pcbe->DeviceMgr.pDeviceArray[cbConvertDeviceBit2Index(pBacklightPara->DeviceId)];

    if(pDevCommon == CBIOS_NULL)
    {
        cbDebugPrint((MAKE_LEVEL(GENERIC, ERROR), "%s: pDevCommon is CBIOS_NULL!\n", FUNCTION_NAME));
        return  CBIOS_ER_NULLPOINTER;
    }

    Status = cbDevEDPSetBacklight(pcbe, pDevCommon, pBacklightPara);

    return Status;
}

DLLEXPORTS CBIOS_STATUS
CBiosGetBacklight(CBIOS_IN PCBIOS_VOID pvcbe, CBIOS_OUT PCBIOS_BACKLIGHT_PARA pBacklightPara)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    PCBIOS_DEVICE_COMMON  pDevCommon = CBIOS_NULL;
    CBIOS_U32 Status = CBIOS_OK;

    if(pBacklightPara == CBIOS_NULL || pBacklightPara->DeviceId == CBIOS_TYPE_NONE)
    {
        cbDebugPrint((MAKE_LEVEL(GENERIC, ERROR), "%s: the 2nd param is CBIOS_NULL!\n", FUNCTION_NAME));
        return  CBIOS_ER_NULLPOINTER;
    }

    pDevCommon = pcbe->DeviceMgr.pDeviceArray[cbConvertDeviceBit2Index(pBacklightPara->DeviceId)];

    if(pDevCommon == CBIOS_NULL)
    {
        cbDebugPrint((MAKE_LEVEL(GENERIC, ERROR), "%s: pDevCommon is CBIOS_NULL!\n", FUNCTION_NAME));
        return  CBIOS_ER_NULLPOINTER;
    }

    Status = cbDevEDPGetBacklight(pcbe, pDevCommon, pBacklightPara);

    return Status;
}

DLLEXPORTS CBIOS_STATUS
CBiosEfuseReadByte(CBIOS_IN PCBIOS_VOID pvcbe, CBIOS_OUT PCBIOS_EFUSE_READBYTE_PARA pReadBytePara)
{
    CBIOS_U32 Status = CBIOS_OK;

    if(pReadBytePara == CBIOS_NULL || pReadBytePara->ByteAddr > 0x3FF)
    {
        return CBIOS_ER_INVALID_PARAMETER;
    }

    Status = cbEfuseReadByte(pvcbe, pReadBytePara->ByteAddr, &pReadBytePara->Data);

    return Status;
}

DLLEXPORTS CBIOS_STATUS
CBiosEfuseWriteByte(CBIOS_IN PCBIOS_VOID pvcbe, CBIOS_OUT PCBIOS_EFUSE_WRITEBYTE_PARA pWriteBytePara)
{
    CBIOS_U32 Status = CBIOS_OK;

    if(pWriteBytePara == CBIOS_NULL || pWriteBytePara->ByteAddr > 0x3FF)
    {
        return CBIOS_ER_INVALID_PARAMETER;
    }

    Status = cbEfuseWriteByteWithMask(pvcbe, pWriteBytePara->ByteAddr, pWriteBytePara->Data, pWriteBytePara->Mask);

    return Status;
}
