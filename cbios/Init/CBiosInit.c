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
** CBios sw initialization function.
** Initialize CBIOS_EXTENSION_COMMON structure.
**
** NOTE:
** The hw dependent initialization SHOULD NOT be added to this file.
******************************************************************************/

#include "CBiosChipShare.h"
#include "CBiosDevice.h"

extern CBIOS_TIMING_ATTRIB HDMIFormatTimingTbl[CBIOS_HDMIFORMATCOUNTS];
extern CBIOS_HDMI_FORMAT_MTX CEAVideoFormatTable[CBIOS_HDMIFORMATCOUNTS];

static CBIOS_VOID cbCbiosChipCapsInit(PCBIOS_EXTENSION_COMMON pcbe)
{
    switch(pcbe->ChipID)
    {
        case CHIPID_E3K:
        case CHIPID_CHX004:
        case CHIPID_CNE001:
            pcbe->ChipCaps.IsSupportUpScaling         = CBIOS_TRUE;
            pcbe->ChipCaps.bNoMemoryControl             = CBIOS_FALSE;
            pcbe->ChipCaps.Is24MReferenceClock          = CBIOS_FALSE;
            pcbe->ChipCaps.IsSupport3DVideo             = CBIOS_TRUE;
            pcbe->ChipCaps.IsSupportDeepColor           = CBIOS_TRUE;
            pcbe->ChipCaps.IsSupportCEC                 = CBIOS_TRUE;
            pcbe->ChipCaps.bSupportConfigEclkByEfuse    = CBIOS_FALSE;
            pcbe->ChipCaps.bSupportScrambling           = CBIOS_TRUE;
            pcbe->ChipCaps.bSupportReadRequest          = CBIOS_FALSE;
            pcbe->ChipLimits.ulMaxPUHorSrc              = 2560;
            pcbe->ChipLimits.ulMaxPUVerSrc              = 1600;
            pcbe->ChipLimits.ulMaxPUHorDst              = 4096;
            pcbe->ChipLimits.ulMaxPUVerDst              = 4096;
            pcbe->ChipLimits.ulMaxHDMIClock             = 6000000;
            pcbe->ChipLimits.ulMaxIGAClock              = 6000000;
            break;
        default:
            pcbe->ChipCaps.IsSupportUpScaling         = CBIOS_FALSE;
            pcbe->ChipCaps.bNoMemoryControl             = CBIOS_TRUE;
            pcbe->ChipCaps.IsSupportDeepColor           = CBIOS_FALSE;
            pcbe->ChipCaps.IsSupportCEC                 = CBIOS_FALSE;
            pcbe->ChipCaps.bSupportConfigEclkByEfuse    = CBIOS_FALSE;
            pcbe->ChipCaps.bSupportScrambling           = CBIOS_FALSE;
            pcbe->ChipCaps.bSupportReadRequest          = CBIOS_FALSE;
            pcbe->ChipLimits.ulMaxPUHorSrc              = 1600;
            pcbe->ChipLimits.ulMaxPUVerSrc              = 1200;
            pcbe->ChipLimits.ulMaxPUHorDst              = 4096;
            pcbe->ChipLimits.ulMaxPUVerDst              = 4096;
            pcbe->ChipLimits.ulMaxHDMIClock             = 1650000;
            pcbe->ChipLimits.ulMaxIGAClock              = 1650000;
            break;
    }
    if(pcbe->ChipID >= CHIPID_CHX004)
    {
        pcbe->ChipCaps.bNoMemoryControl = CBIOS_TRUE;
    }

    if(pcbe->DriverFlags.bRunOnQT)
    {
        pcbe->ChipCaps.bNoMemoryControl = CBIOS_FALSE;
    }
}


//SW Initialization: init CBIOS_EXTENSION_COMMON structure, and port struct init
CBIOS_BOOL cbInitialize(PCBIOS_VOID pvcbe, PCBIOS_PARAM_INIT pCBParamInit)
{
    PCBIOS_EXTENSION_COMMON    pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    PCBIOS_VOID                pRomBase = pCBParamInit->RomImage; 
    CBIOS_U32                  i = 0;
    PVCP_INFO              pVCP = CBIOS_NULL;

    cbTraceEnter(GENERIC);

    pcbe->pSpinLock = cb_CreateLock(CBIOS_OS_SPIN_LOCK);

    pcbe->pAdapterContext = pCBParamInit->pAdapterContext;
    pcbe->bMAMMPrimaryAdapter = pCBParamInit->MAMMPrimaryAdapter;
    pcbe->PCIDeviceID = pCBParamInit->PCIDeviceID;
    pcbe->ChipID = pCBParamInit->ChipID;
    pcbe->SVID = pCBParamInit->SVID;
    pcbe->SSID = pCBParamInit->SSID;
    pcbe->ChipRevision = pCBParamInit->ChipRevision;
    pcbe->BoardVersion = CBIOS_BOARD_VERSION_DEFAULT;

    pcbe->DriverFlags.bRunOnQT = pCBParamInit->DriverFlags.bRunOnQT;
    pcbe->DriverFlags.bDriverLoadQTiming = pCBParamInit->DriverFlags.bDriverLoadQTiming;
    pcbe->DriverFlags.bRunHDCPCTS = pCBParamInit->DriverFlags.bRunHDCPCTS;
    pcbe->DriverFlags.bBgaPatchCPU = pCBParamInit->DriverFlags.bBgaPatchCPU;

    cbDebugPrint((MAKE_LEVEL(GENERIC, INFO), "Get DriverFlags.bBgaPatchCPU = %d!\n", pCBParamInit->DriverFlags.bBgaPatchCPU));
    
    cbHWInitChipAttribute((PCBIOS_VOID)pcbe, pcbe->ChipID);

    //init HDMI format table
    pcbe->pCEAVideoTable = CEAVideoFormatTable;
    pcbe->pHDMITimingTable = HDMIFormatTimingTbl;
    
    //init Memory Type
    pcbe->MemoryType = Default_Mem_Type; //It will be reset once when change clocks
    
    if ((pcbe->ChipID >= CHIPID_CHX004) && pRomBase != CBIOS_NULL)
    {
        CBIOS_PARAM_SHADOWINFO ShadowInfo;
        ShadowInfo.SysShadowAddr = (PCBIOS_VOID)((CBIOS_U8*)(pRomBase) + KB(64));
        ShadowInfo.SysShadowLength = pCBParamInit->RomImageLength - KB(64);

        if( !cbUpdateShadowInfo(pcbe, &ShadowInfo) )
        {
            cbDebugPrint((MAKE_LEVEL(GENERIC, ERROR), "Failed to get shadow info!\n"));
        }
    }

    //Init cbios chip caps first
    cbCbiosChipCapsInit(pcbe);
    
    //Init VCP info structure.
    pVCP = cb_AllocatePagedPool(sizeof(VCP_INFO));

    if(pVCP == CBIOS_NULL)
    {
        cbDebugPrint((MAKE_LEVEL(GENERIC, ERROR), "%s: pVCP allocate error\n", FUNCTION_NAME));
        return CBIOS_FALSE;
    }

    if(!cbInitVCP(pcbe, pVCP, pRomBase))
    {
        return CBIOS_FALSE;
    }
    else
    {
        //Init table.
        pcbe->sizeofBootDevPriority = pVCP->sizeofBootDevPriority;
        pcbe->PostRegTable[VCP_TABLE].sizeofCRDefault = pVCP->sizeofCR_DEFAULT_TABLE;
        pcbe->PostRegTable[VCP_TABLE].sizeofSRDefault = pVCP->sizeofSR_DEFAULT_TABLE;
        pcbe->PostRegTable[VCP_TABLE].sizeofModeExtRegDefault_TBL = 0;

        pcbe->pBootDevPriority = cb_AllocateNonpagedPool(sizeof(CBIOS_U16) * pVCP->sizeofBootDevPriority);
        pcbe->PostRegTable[VCP_TABLE].pCRDefault = cb_AllocateNonpagedPool(sizeof(CBREGISTER) * pVCP->sizeofCR_DEFAULT_TABLE);
        pcbe->PostRegTable[VCP_TABLE].pSRDefault = cb_AllocateNonpagedPool(sizeof(CBREGISTER) * pVCP->sizeofSR_DEFAULT_TABLE);
        pcbe->PostRegTable[VCP_TABLE].pModeExtRegDefault_TBL = CBIOS_NULL;

        cb_memcpy(pcbe->pBootDevPriority, pVCP->BootDevPriority, sizeof(CBIOS_U16) * pVCP->sizeofBootDevPriority);
        cb_memcpy(pcbe->PostRegTable[VCP_TABLE].pCRDefault, pVCP->pCR_DEFAULT_TABLE, sizeof(CBREGISTER) * pVCP->sizeofCR_DEFAULT_TABLE);
        cb_memcpy(pcbe->PostRegTable[VCP_TABLE].pSRDefault, pVCP->pSR_DEFAULT_TABLE, sizeof(CBREGISTER) * pVCP->sizeofSR_DEFAULT_TABLE);
    }

    cb_memcpy(&pcbe->FeatureSwitch, &pVCP->FeatureSwitch, sizeof(CBIOS_U32));
   
    pcbe->bUseVCP = pVCP->bUseVCP;
    pcbe->BiosVersion = pVCP->BiosVersion;
    pcbe->EClock = pVCP->EClock;
    pcbe->EVcoLow = pVCP->EVcoLow;
    pcbe->DVcoLow = pVCP->DVcoLow;
    pcbe->DeviceMgr.SupportDevices = pVCP->SupportDevices;
    pcbe->SliceNum = pVCP->SliceNum;

    for (i = IGA1; i < CBIOS_IGACOUNTS; i++)
    {
        pcbe->DispMgr.ActiveDevices[i] = CBIOS_TYPE_NONE;
    }

    //RealVision requires I2C speed up to 400kbps
    if ((pVCP->SubVendorID == 0x12EA) && (pVCP->SubSystemID == 0x0002))
    {
        pcbe->I2CDelay = 3;
    }
    else
    {
        pcbe->I2CDelay = I2C_DELAY_DEFAULT;
    }

    //initialize CEC para
    for (i = 0; i < CBIOS_CEC_INDEX_COUNT; i++)
    {
        pcbe->CECPara[i].CECEnable = CBIOS_FALSE;
        pcbe->CECPara[i].LogicalAddr = CEC_UNREGISTERED_DEVICE;
        pcbe->CECPara[i].PhysicalAddr = CEC_INVALID_PHYSICAL_ADDR;
    }
    
    cbInitDeviceArray(pcbe, pVCP);

    cbDispMgrInit(pcbe);

    if(pVCP->pCR_DEFAULT_TABLE)
    {
        cb_FreePool(pVCP->pCR_DEFAULT_TABLE);
        pVCP->pCR_DEFAULT_TABLE = CBIOS_NULL;
    }
    if(pVCP->pSR_DEFAULT_TABLE)
    {
        cb_FreePool(pVCP->pSR_DEFAULT_TABLE);
        pVCP->pSR_DEFAULT_TABLE = CBIOS_NULL;
    }
    cb_FreePool(pVCP);

    return CBIOS_TRUE;
}


