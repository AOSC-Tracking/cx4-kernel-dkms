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
** CBios hw independent callback function implementation.
**
** NOTE:
** The hw dependent callback function SHOULD NOT be added to this file.
******************************************************************************/


#include "CBiosCallbacks.h"
#include "../Device/CBiosChipShare.h"

CALLBACK_cbVDbgPrint              cbVDbgPrint = CBIOS_NULL;

CBIOS_CALLBACK_FUNCTIONS FnCallback = {0};

CBIOS_STATUS cbSetCallBackFunctions(PCBIOS_CALLBACK_FUNCTIONS pFnCallBack)
{
    if(pFnCallBack == CBIOS_NULL)
    {
        return CBIOS_ER_INVALID_PARAMETER;
    }

    if((pFnCallBack->pFnDelayMicroSeconds        == CBIOS_NULL) ||
       (pFnCallBack->pFnReadUchar                == CBIOS_NULL) ||
       (pFnCallBack->pFnReadUlong                == CBIOS_NULL) ||
       (pFnCallBack->pFnWriteUchar               == CBIOS_NULL) ||
       (pFnCallBack->pFnWriteUlong               == CBIOS_NULL) ||
       (pFnCallBack->pFnAllocateNonpagedMemory   == CBIOS_NULL) ||
       (pFnCallBack->pFnAllocatePagedMemory      == CBIOS_NULL) ||
       (pFnCallBack->pFnFreePool                 == CBIOS_NULL) ||
       (pFnCallBack->pFnMemset == CBIOS_NULL) ||
       (pFnCallBack->pFnMemcpy == CBIOS_NULL) ||
       (pFnCallBack->pFnMemcmp == CBIOS_NULL))
    {
        return CBIOS_ER_INVALID_PARAMETER;
    }

    // remove PrintToFile function for android 4.4 resume hang issue

    pFnCallBack->pFnDbgPrintToFile = CBIOS_NULL;

    cbVDbgPrint = pFnCallBack->pFnVDbgPrint;

    //FnCallback = *pFnCallBack;
    FnCallback.Size=pFnCallBack->Size;
    FnCallback.pFnDelayMicroSeconds=pFnCallBack->pFnDelayMicroSeconds;
    FnCallback.pFnReadUchar=pFnCallBack->pFnReadUchar;
    FnCallback.pFnReadUshort=pFnCallBack->pFnReadUshort;
    FnCallback.pFnReadUlong=pFnCallBack->pFnReadUlong;
    FnCallback.pFnWriteUchar=pFnCallBack->pFnWriteUchar;
    FnCallback.pFnWriteUshort=pFnCallBack->pFnWriteUshort;
    FnCallback.pFnWriteUlong=pFnCallBack->pFnWriteUlong;
    FnCallback.pFnQuerySystemTime=pFnCallBack->pFnQuerySystemTime;
    FnCallback.pFnAllocateNonpagedMemory=pFnCallBack->pFnAllocateNonpagedMemory;
    FnCallback.pFnAllocatePagedMemory=pFnCallBack->pFnAllocatePagedMemory;
    FnCallback.pFnFreePool=pFnCallBack->pFnFreePool;
    FnCallback.pFnCreateLock=pFnCallBack->pFnCreateLock;
    FnCallback.pFnDestroyLock=pFnCallBack->pFnDestroyLock;
    FnCallback.pFnAcquireLock=pFnCallBack->pFnAcquireLock;
    FnCallback.pFnReleaseLock=pFnCallBack->pFnReleaseLock;
    FnCallback.pFnMemset=pFnCallBack->pFnMemset;
    FnCallback.pFnMemcpy=pFnCallBack->pFnMemcpy;
    FnCallback.pFnMemcmp=pFnCallBack->pFnMemcmp;
    FnCallback.pFnDodiv=pFnCallBack->pFnDodiv;
    FnCallback.pFnWriteRegisterU32=pFnCallBack->pFnWriteRegisterU32;
    FnCallback.pFnReadRegisterU32=pFnCallBack->pFnReadRegisterU32;
    FnCallback.pFnDbgPrintToFile=pFnCallBack->pFnDbgPrintToFile;

    FnCallback.pFnGpioGetValue=pFnCallBack->pFnGpioGetValue;
    FnCallback.pFnGpioSetValue=pFnCallBack->pFnGpioSetValue;
    FnCallback.pFnGpioRequest=pFnCallBack->pFnGpioRequest;
    FnCallback.pFnGpioFree=pFnCallBack->pFnGpioFree;
    FnCallback.pFnGpioDirectionInput=pFnCallBack->pFnGpioDirectionInput;
    FnCallback.pFnGpioDirectionOutput=pFnCallBack->pFnGpioDirectionOutput;
    FnCallback.pFnGetPlatformConfigU32=pFnCallBack->pFnGetPlatformConfigU32;

    FnCallback.pFnRegulatorGet=pFnCallBack->pFnRegulatorGet;
    FnCallback.pFnRegulatorEnable=pFnCallBack->pFnRegulatorEnable;
    FnCallback.pFnRegulatorDisable=pFnCallBack->pFnRegulatorDisable;
    FnCallback.pFnRegulatorIsEnabled=pFnCallBack->pFnRegulatorIsEnabled;
    FnCallback.pFnRegulatorGetVoltage=pFnCallBack->pFnRegulatorGetVoltage;
    FnCallback.pFnRegulatorSetVoltage=pFnCallBack->pFnRegulatorSetVoltage;
    FnCallback.pFnRegulatorPut=pFnCallBack->pFnRegulatorPut;
    FnCallback.pFnVDbgPrint = pFnCallBack->pFnVDbgPrint;
    
    return CBIOS_OK;
}

CBIOS_VOID cb_DelayMicroSeconds(CBIOS_U32 Microseconds)
{
    if (FnCallback.pFnDelayMicroSeconds != CBIOS_NULL)
    {
        ((CALLBACK_cbDelayMicroSeconds)FnCallback.pFnDelayMicroSeconds)(Microseconds);
    }
    else
    {
        cbDebugPrint((MAKE_LEVEL(GENERIC, ERROR), "%s: callback not defined!\n",FUNCTION_NAME));
    }
}

CBIOS_U64 cb_QuerySystemTime(void)
{
    CBIOS_U64 time = 0;

    if (FnCallback.pFnQuerySystemTime != CBIOS_NULL)
    {
        ((CALLBACK_cbQuerySystemTime)FnCallback.pFnQuerySystemTime)(&time);
    }
    else
    {
        cbDebugPrint((MAKE_LEVEL(GENERIC, ERROR), "%s: callback not defined!\n",FUNCTION_NAME));
    }

    return time;
}

// memory allocation and free function stub
PCBIOS_VOID cb_AllocateNonpagedPool(CBIOS_U32 NumberOfBytes)
{
    PCBIOS_VOID Ret = CBIOS_NULL;

    if (FnCallback.pFnAllocateNonpagedMemory != CBIOS_NULL)
    {
        Ret = ((CALLBACK_cbAllocateNonpagedPool)FnCallback.pFnAllocateNonpagedMemory)(NumberOfBytes);
    }
    else
    {
        cbDebugPrint((MAKE_LEVEL(GENERIC, ERROR), "%s: callback not defined!\n",FUNCTION_NAME));
    }
    return Ret;
}

PCBIOS_VOID cb_AllocatePagedPool(CBIOS_U32 NumberOfBytes)
{
    PCBIOS_VOID Ret = CBIOS_NULL;

    if (FnCallback.pFnAllocatePagedMemory != CBIOS_NULL)
    {
        Ret = ((CALLBACK_cbAllocatePagedPool)FnCallback.pFnAllocatePagedMemory)(NumberOfBytes);
    }
    else
    {
        cbDebugPrint((MAKE_LEVEL(GENERIC, ERROR), "%s: callback not defined!\n",FUNCTION_NAME));
    }
    return Ret;
}

CBIOS_VOID cb_FreePool(PCBIOS_VOID pPoolMem)
{
    if (FnCallback.pFnFreePool != CBIOS_NULL)
    {
        ((CALLBACK_cbFreePool)FnCallback.pFnFreePool)(pPoolMem);
    }
    else
    {
        cbDebugPrint((MAKE_LEVEL(GENERIC, ERROR), "%s: callback func is not defined!\n", FUNCTION_NAME));
    }
}

PCBIOS_VOID cb_CreateLock(CBIOS_OS_LOCK_TYPE lockType)
{
    if (FnCallback.pFnCreateLock != CBIOS_NULL)
    {
        return ((CALLBACK_cbCreateLock)FnCallback.pFnCreateLock)(lockType);
    }
    else
    {
        cbDebugPrint((MAKE_LEVEL(GENERIC, WARNING), "%s: callback func is not defined!\n", FUNCTION_NAME));

        return CBIOS_NULL;
    }
}

CBIOS_VOID cb_DestroyLock(PCBIOS_VOID pOsLock, CBIOS_OS_LOCK_TYPE lockType)
{
    if (FnCallback.pFnDestroyLock != CBIOS_NULL)
    {
        ((CALLBACK_cbDestroyLock)FnCallback.pFnDestroyLock)(pOsLock, lockType);
    }
    else
    {
        cbDebugPrint((MAKE_LEVEL(GENERIC, WARNING), "%s: callback func is not defined!\n", FUNCTION_NAME));
    }
}

CBIOS_U64 cb_AcquireLock(PCBIOS_VOID pOsLock, CBIOS_OS_LOCK_TYPE lockType)
{
    if (FnCallback.pFnAcquireLock != CBIOS_NULL)
    {
        return ((CALLBACK_cbAcquireLock)FnCallback.pFnAcquireLock)(pOsLock, lockType);
    }
    else
    {
        return 0;
    }
}

CBIOS_VOID cb_ReleaseLock(PCBIOS_VOID pOsLock, CBIOS_OS_LOCK_TYPE lockType, CBIOS_U64 flags)
{
    if (FnCallback.pFnReleaseLock != CBIOS_NULL)
    {
        ((CALLBACK_cbReleaseLock)FnCallback.pFnReleaseLock)(pOsLock, lockType, flags);
    }
}

PCBIOS_VOID cb_memset(PCBIOS_VOID pBuf, CBIOS_U32 value, CBIOS_U32 length)
{
    PCBIOS_VOID Ret = CBIOS_NULL;

    if (FnCallback.pFnMemset != CBIOS_NULL)
    {
        //new driver has the callback function
        Ret = ((CALLBACK_cbMemSet)FnCallback.pFnMemset)(pBuf, value, length);
    }
    else
    {
        //old driver has no callback function, then use standard lib function
        cbDebugPrint((MAKE_LEVEL(GENERIC, ERROR), "%s: callback func is not defined!\n", FUNCTION_NAME));
    }

    return Ret;
}

PCBIOS_VOID cb_memcpy(PCBIOS_VOID pBuf1, PCBIOS_VOID pBuf2, CBIOS_U32 length)
{
    PCBIOS_VOID Ret = CBIOS_NULL;
    
    if (FnCallback.pFnMemcpy!= CBIOS_NULL)
    {
        //new driver has the callback function
        Ret = ((CALLBACK_cbMemCpy)FnCallback.pFnMemcpy)(pBuf1, pBuf2, length);
    }
    else
    {
        //old driver has no callback function, then use standard lib function
        cbDebugPrint((MAKE_LEVEL(GENERIC, ERROR), "%s: callback func is not defined!\n", FUNCTION_NAME));
    }

    return Ret;
}

CBIOS_S32 cb_memcmp(PCBIOS_VOID pBuf1, PCBIOS_VOID pBuf2, CBIOS_U32 length)
{
    CBIOS_S32 Ret = 0;
    
    if(FnCallback.pFnMemcmp != CBIOS_NULL)
    {    
        //new driver has the callback function
        Ret = ((CALLBACK_cbMemCmp)FnCallback.pFnMemcmp)(pBuf1, pBuf2, length);
    }
    else
    {
        //old driver has no callback function, then use standard lib function
        cbDebugPrint((MAKE_LEVEL(GENERIC, ERROR), "%s: callback func is not defined!\n", FUNCTION_NAME));
    }

    return Ret;
}


CBIOS_U64 cb_do_div(CBIOS_U64 a, CBIOS_U64 b)
{
    CBIOS_U64 Ret = 0;
    
    if (FnCallback.pFnDodiv != CBIOS_NULL)

    {
        //new driver has the callback function
        Ret = ((CALLBACK_cbDoDiv)FnCallback.pFnDodiv)(a, b);
    }
    else
    {
        Ret = a / b;
    }

    return Ret;
}

CBIOS_VOID cbPrintMsgToFile(CBIOS_U32 DebugPrintLevel, PCBIOS_CHAR DebugMessage, PCBIOS_VOID pBuffer, CBIOS_U32 Size)
{
    if (FnCallback.pFnDbgPrintToFile != CBIOS_NULL)
    {
        ((CALLBACK_cbDbgPrintToFile)FnCallback.pFnDbgPrintToFile)(DebugPrintLevel, DebugMessage, pBuffer, Size);
    }

    return;
}

CBIOS_BOOL cbGetPlatformConfigurationU32(PCBIOS_VOID pvcbe, CBIOS_U8 *pName, CBIOS_U32 *pBuffer, CBIOS_U32 Length)
{

    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;

    CBIOS_BOOL bRet = CBIOS_FALSE;

    if (FnCallback.pFnGetPlatformConfigU32 != CBIOS_NULL)
    {
        bRet = ((CALLBACK_cbGetPlatformConfigU32)FnCallback.pFnGetPlatformConfigU32)(pcbe->pAdapterContext, pName, pBuffer, Length);
    }
    else
    {
        bRet = CBIOS_FALSE;
    }

    return bRet;
}

