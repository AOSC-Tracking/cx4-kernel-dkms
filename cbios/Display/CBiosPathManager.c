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
** CBios path manager interface function implementation. 
** Generate display source path, DIU module index and devices combination.
**
** NOTE:
**The hw dependent function or structure SHOULD NOT be added to this file.
******************************************************************************/

#include "CBiosPathManager.h"
#include "CBiosChipShare.h"
#include "../Hw/HwBlock/CBiosDIU_HDCP.h"
#include "../Hw/HwBlock/CBiosDIU_HDTV.h"
#include "../Hw/HwBlock/CBiosDIU_DP.h"
#include "../Hw/HwBlock/CBiosDIU_HDMI.h"


CBIOS_STATUS cbPathMgrGetDevComb(PCBIOS_VOID pvcbe, PCBIOS_GET_DEV_COMB pDevComb)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    PCBIOS_DEVICE_COMB pDeviceComb = pDevComb->pDeviceComb;
    CBIOS_ACTIVE_TYPE  Devices = pDeviceComb->Devices;

    pDeviceComb->Iga1Dev = CBIOS_TYPE_NONE;
    pDeviceComb->Iga2Dev = CBIOS_TYPE_NONE;
    pDeviceComb->Iga3Dev = CBIOS_TYPE_NONE;
    pDeviceComb->Iga4Dev = CBIOS_TYPE_NONE;

    if (Devices == CBIOS_TYPE_NONE)
    {
        return  CBIOS_OK;
    }

    if (pcbe->DriverFlags.bRunOnQT)
    {
        pDeviceComb->Iga1Dev = CBIOS_TYPE_CRT;
        pDevComb->bSupported = CBIOS_TRUE;

        return  CBIOS_OK;
    }
    
    if (pcbe->ChipID == CHIPID_E3K)
    {
        if ((cbGetBitsNum(Devices) > CBIOS_IGACOUNTS) || (Devices & ~(pcbe->DeviceMgr.SupportDevices)))
        {
            pDevComb->bSupported = CBIOS_FALSE;
            return CBIOS_ER_INVALID_PARAMETER;
        }
        else
        {
            if(Devices & CBIOS_TYPE_DP1)
            {
                pDevComb->pDeviceComb->Iga1Dev = CBIOS_TYPE_DP1;
            }

            if(Devices & CBIOS_TYPE_DP2)
            {
                pDevComb->pDeviceComb->Iga2Dev = CBIOS_TYPE_DP2;
            }

            if(Devices & CBIOS_TYPE_DP3)
            {
                pDevComb->pDeviceComb->Iga3Dev = CBIOS_TYPE_DP3;
            }

            if(Devices & CBIOS_TYPE_DP4)
            {
                pDevComb->pDeviceComb->Iga4Dev = CBIOS_TYPE_DP4;
            }
            else if(Devices & CBIOS_TYPE_CRT)
            {
                pDevComb->pDeviceComb->Iga4Dev = CBIOS_TYPE_CRT;
            }
            pDevComb->bSupported = CBIOS_TRUE;
            return CBIOS_OK;
        }
    }
    else
    {
        if ((cbGetBitsNum(Devices) > CBIOS_IGACOUNTS_CHX004) || (Devices & ~(pcbe->DeviceMgr.SupportDevices)))
        {
            pDevComb->bSupported = CBIOS_FALSE;
            return CBIOS_ER_INVALID_PARAMETER;
        }
        else
        {
            if (pcbe->ChipID == CHIPID_CHX004)
            {
                if(Devices & CBIOS_TYPE_DP1)
                {
                    pDevComb->pDeviceComb->Iga1Dev = CBIOS_TYPE_DP1;
                }
                if(Devices & CBIOS_TYPE_DP2)
                {
                    pDevComb->pDeviceComb->Iga2Dev = CBIOS_TYPE_DP2;
                }
                if(Devices & CBIOS_TYPE_CRT)
                {
                    pDevComb->pDeviceComb->Iga3Dev = CBIOS_TYPE_CRT;
                }
            }
            else if (pcbe->ChipID == CHIPID_CNE001)
            {
                if(Devices & CBIOS_TYPE_CRT)
                {
                    if (pcbe->SysBiosInfo.Dp3PortConnType == CBIOS_NON_CONN)
                    {
                        pDevComb->pDeviceComb->Iga3Dev = CBIOS_TYPE_CRT;
                    }
                    else if (pcbe->SysBiosInfo.Dp2PortConnType == CBIOS_NON_CONN)
                    {
                        pDevComb->pDeviceComb->Iga2Dev = CBIOS_TYPE_CRT;
                    }
                    else if (pcbe->SysBiosInfo.Dp1PortConnType == CBIOS_NON_CONN)
                    {
                        pDevComb->pDeviceComb->Iga1Dev = CBIOS_TYPE_CRT;
                    }
                    else
                    {
                        if (pcbe->SysBiosInfo.Dp3PortConnType != CBIOS_EDP_CONN)
                        {
                            pDevComb->pDeviceComb->Iga3Dev = CBIOS_TYPE_CRT;
                        }
                        else if (pcbe->SysBiosInfo.Dp2PortConnType != CBIOS_EDP_CONN)
                        {
                            pDevComb->pDeviceComb->Iga2Dev = CBIOS_TYPE_CRT;
                        }
                        else if (pcbe->SysBiosInfo.Dp1PortConnType != CBIOS_EDP_CONN)
                        {
                            pDevComb->pDeviceComb->Iga1Dev = CBIOS_TYPE_CRT;
                        }
                        else
                        {
                            pDevComb->pDeviceComb->Iga3Dev = CBIOS_TYPE_CRT;
                        }
                    }
                }
                //If CRT and DP1/2/3 are connected at the same time, and both bind to same IGA, only DP1/2/3 will be lit up
                if(Devices & CBIOS_TYPE_DP1)
                {
                    pDevComb->pDeviceComb->Iga1Dev = CBIOS_TYPE_DP1;
                }

                if(Devices & CBIOS_TYPE_DP2)
                {
                    pDevComb->pDeviceComb->Iga2Dev = CBIOS_TYPE_DP2;
                }

                if(Devices & CBIOS_TYPE_DP3)
                {
                    pDevComb->pDeviceComb->Iga3Dev = CBIOS_TYPE_DP3;
                }
            }
            pDevComb->bSupported = CBIOS_TRUE;
            return CBIOS_OK;
        }
    }
    
    return  CBIOS_OK;
}

CBIOS_STATUS cbPathMgrGetIgaMask(PCBIOS_VOID pvcbe, PCBIOS_GET_IGA_MASK pGetIgaMask)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;

    if (pcbe->DriverFlags.bRunOnQT)
    {
        pGetIgaMask->IgaMask = 1 << IGA1;
        return  CBIOS_OK;
    }

    if (pcbe->ChipID == CHIPID_E3K)
    {
        switch(pGetIgaMask->DeviceId)
        {
        case CBIOS_TYPE_DP1:
            pGetIgaMask->IgaMask = 1 << IGA1;
            break;
        case CBIOS_TYPE_DP2:
            pGetIgaMask->IgaMask = 1 << IGA2;
            break;
        case CBIOS_TYPE_DP3:
            pGetIgaMask->IgaMask = 1 << IGA3;
            break;
        case CBIOS_TYPE_DP4:
            pGetIgaMask->IgaMask = 1 << IGA4;
            break;
        case CBIOS_TYPE_CRT:
            pGetIgaMask->IgaMask = 1 << IGA4;
            break;
        default:
            pGetIgaMask->IgaMask = (1 << IGA1) | (1 << IGA2) | (1 << IGA3) | (1 << IGA4);
            cbDebugPrint((MAKE_LEVEL(GENERIC, WARNING), "%s: invalid device: 0x%x.\n", FUNCTION_NAME, pGetIgaMask->DeviceId));
            break; 
        }
    }
    else if(pcbe->ChipID == CHIPID_CHX004)
    {
        switch(pGetIgaMask->DeviceId)
        {
        case CBIOS_TYPE_CRT:
        case CBIOS_TYPE_DVO:
            pGetIgaMask->IgaMask = 1 << IGA3;
            break;
        case CBIOS_TYPE_DP1:
            pGetIgaMask->IgaMask = 1 << IGA1;
            break;
        case CBIOS_TYPE_DP2:
            pGetIgaMask->IgaMask = 1 << IGA2;
            break;
        default:
            pGetIgaMask->IgaMask = (1 << IGA3) | (1 << IGA2);
            break;
        }
    }
    else if(pcbe->ChipID == CHIPID_CNE001)
    {
        switch(pGetIgaMask->DeviceId)
        {
        case CBIOS_TYPE_CRT: //choose CBIOS_NON_CONN first then choose not CBIOS_EDP_CONN
            if (pcbe->SysBiosInfo.Dp3PortConnType == CBIOS_NON_CONN)
            {
                pGetIgaMask->IgaMask = (1 << IGA3);
            }
            else if (pcbe->SysBiosInfo.Dp2PortConnType == CBIOS_NON_CONN)
            {
                pGetIgaMask->IgaMask = (1 << IGA2);
            }
            else if (pcbe->SysBiosInfo.Dp1PortConnType == CBIOS_NON_CONN)
            {
                pGetIgaMask->IgaMask = (1 << IGA1);
            }
            else
            {
                if (pcbe->SysBiosInfo.Dp3PortConnType != CBIOS_EDP_CONN)
                {
                    pGetIgaMask->IgaMask = (1 << IGA3);
                }
                else if (pcbe->SysBiosInfo.Dp2PortConnType != CBIOS_EDP_CONN)
                {
                    pGetIgaMask->IgaMask = (1 << IGA2);
                }
                else if (pcbe->SysBiosInfo.Dp1PortConnType != CBIOS_EDP_CONN)
                {
                    pGetIgaMask->IgaMask = (1 << IGA1);
                }
                else
                {
                    pGetIgaMask->IgaMask = (1 << IGA3);
                }
            }
            break;
        case CBIOS_TYPE_DVO:
        case CBIOS_TYPE_DP3:
            pGetIgaMask->IgaMask = 1 << IGA3;
            break;
        case CBIOS_TYPE_DP1:
            pGetIgaMask->IgaMask = 1 << IGA1;
            break;
        case CBIOS_TYPE_DP2:
            pGetIgaMask->IgaMask = 1 << IGA2;
            break;
        default:
            pGetIgaMask->IgaMask = (1 << IGA3) | (1 << IGA2);
            break; 
        }
    }
    return  CBIOS_OK;
}


CBIOS_VOID cbPathMgrGeneratePath(PCBIOS_VOID pvcbe, CBIOS_ACTIVE_TYPE Device)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    PCBIOS_DEVICE_COMMON    pDevCommon = cbGetDeviceCommon(&pcbe->DeviceMgr, Device);
    CBIOS_DISPLAY_SOURCE   *pSource = CBIOS_NULL;
    CBIOS_MODULE_LIST *pModuleList = CBIOS_NULL;
    CBIOS_U32                index = 0;

    if (pDevCommon == CBIOS_NULL)
    {
        cbDebugPrint((MAKE_LEVEL(GENERIC, ERROR), "%s: the 2nd param is NULL!\n", FUNCTION_NAME));
        return;
    }

    pSource = &pDevCommon->DispSource;
    pModuleList = &pSource->ModuleList;

    cb_memset(pSource->ModulePath, 0, sizeof(CBIOS_MODULE*) * CBIOS_MAX_MODULE_NUM);
    if (pModuleList->HDCPModule.Index != CBIOS_MODULE_INDEX_INVALID)
    {
        pSource->ModulePath[index] = &pModuleList->HDCPModule;
        index++;
    }

    if (pModuleList->HDMIModule.Index != CBIOS_MODULE_INDEX_INVALID)
    {
        pSource->ModulePath[index] = &pModuleList->HDMIModule;
        index++;
    }

    if (pModuleList->HDTVModule.Index != CBIOS_MODULE_INDEX_INVALID)
    {
        pSource->ModulePath[index] = &pModuleList->HDTVModule;
        index++;
    }

    if (pModuleList->IGAModule.Index != CBIOS_MODULE_INDEX_INVALID)
    {
        pSource->ModulePath[index] = &pModuleList->IGAModule;
        index++;
    }
}


CBIOS_VOID cbPathMgrSelectDIUPath(PCBIOS_VOID pvcbe, CBIOS_ACTIVE_TYPE Device, CBIOS_U32 IGAIndex, PCBiosDestModeParams pDestModeParams)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    CBIOS_DISPLAY_SOURCE   *pSrc = CBIOS_NULL;
    CBIOS_BOOL              isHDMIDevice = CBIOS_FALSE;
    PCBIOS_DEVICE_COMMON    pDevCommon = cbGetDeviceCommon(&pcbe->DeviceMgr, Device);
    CBIOS_U32               bInterlace = pDestModeParams->InterlaceFlag;
    CBIOS_MODULE_INDEX      ModuleIndex = CBIOS_MODULE_INDEX_INVALID;

    pSrc = &(pDevCommon->DispSource);
    isHDMIDevice = pDevCommon->EdidStruct.Attribute.IsCEA861HDMI;

    if (IGAIndex == IGA1)
    {
        pSrc->ModuleList.IGAModule.Index = CBIOS_MODULE_INDEX1;
    }
    else if (IGAIndex == IGA2)
    {
        pSrc->ModuleList.IGAModule.Index = CBIOS_MODULE_INDEX2;
    }
    else if (IGAIndex == IGA3)
    {
        pSrc->ModuleList.IGAModule.Index = CBIOS_MODULE_INDEX3;
    }
    else if(IGAIndex == IGA4)
    {
        pSrc->ModuleList.IGAModule.Index = CBIOS_MODULE_INDEX4;
    }
    else
    {
        pSrc->ModuleList.IGAModule.Index = CBIOS_MODULE_INDEX_INVALID;
    }

    switch (Device)
    {
    case CBIOS_TYPE_DP1: // currently DVI mode doesn't support interlaced timing
    case CBIOS_TYPE_DP2:
    case CBIOS_TYPE_DP3:
    case CBIOS_TYPE_DP4:
        ModuleIndex = CBIOS_MODULE_INDEX_INVALID;
        if (Device == CBIOS_TYPE_DP1)
        {
            ModuleIndex = CBIOS_MODULE_INDEX1;
        }
        else if (Device == CBIOS_TYPE_DP2)
        {
            ModuleIndex = CBIOS_MODULE_INDEX2;
        }
        else if (Device == CBIOS_TYPE_DP3)
        {
            ModuleIndex = CBIOS_MODULE_INDEX3;
        }
        else if (Device == CBIOS_TYPE_DP4)
        {
            ModuleIndex = CBIOS_MODULE_INDEX4;
        }
        
        pSrc->ModuleList.HDTVModule.Index = CBIOS_MODULE_INDEX_INVALID;
        if (pDevCommon->CurrentMonitorType & (CBIOS_MONITOR_TYPE_HDMI | CBIOS_MONITOR_TYPE_DVI))
        {
            if (isHDMIDevice)
            {
                pSrc->ModuleList.HDMIModule.Index = ModuleIndex;
                if (Device == CBIOS_TYPE_DP1)
                {
                    pSrc->ModuleList.HDACModule.Index = CBIOS_MODULE_INDEX1;
                }
                else if (Device == CBIOS_TYPE_DP2)
                {
                    pSrc->ModuleList.HDACModule.Index = CBIOS_MODULE_INDEX2;
                }
                else if (Device == CBIOS_TYPE_DP3)
                {
                    pSrc->ModuleList.HDACModule.Index = CBIOS_MODULE_INDEX3;
                }
                else
                {
                    pSrc->ModuleList.HDACModule.Index = CBIOS_MODULE_INDEX_INVALID;
                }

                if(bInterlace || ((pDestModeParams->XRes == 720) && (pDestModeParams->YRes == 576)))
                {
                    pSrc->ModuleList.HDTVModule.Index = ModuleIndex;
                }
            }
            else
            {
                pSrc->ModuleList.HDMIModule.Index = ModuleIndex;
                pSrc->ModuleList.HDACModule.Index = CBIOS_MODULE_INDEX_INVALID;
            }
        }
        else
        {
            pSrc->ModuleList.HDMIModule.Index = CBIOS_MODULE_INDEX_INVALID;
            if (Device == CBIOS_TYPE_DP1)
            {
                pSrc->ModuleList.HDACModule.Index = CBIOS_MODULE_INDEX1;
            }
            else if (Device == CBIOS_TYPE_DP2)
            {
                pSrc->ModuleList.HDACModule.Index = CBIOS_MODULE_INDEX2;
            }
            else if (Device == CBIOS_TYPE_DP3)
            {
                pSrc->ModuleList.HDACModule.Index = CBIOS_MODULE_INDEX3;
            }
            else
            {
                pSrc->ModuleList.HDACModule.Index = CBIOS_MODULE_INDEX_INVALID;
            }

            if(bInterlace || ((pDestModeParams->XRes == 720) && (pDestModeParams->YRes == 576)))
            {
                pSrc->ModuleList.HDTVModule.Index = ModuleIndex;
            }
        }
        cbPathMgrGeneratePath(pcbe, Device);
        break;
    case CBIOS_TYPE_MHL: // currently DVI mode doesn't support interlaced timing
        pSrc->ModuleList.HDCPModule.Index = CBIOS_MODULE_INDEX1;
        pSrc->ModuleList.HDTVModule.Index = CBIOS_MODULE_INDEX_INVALID;

        if (isHDMIDevice)
        {
            pSrc->ModuleList.HDMIModule.Index = CBIOS_MODULE_INDEX1;
            pSrc->ModuleList.HDACModule.Index = CBIOS_MODULE_INDEX1;
        }
        else
        {
            pSrc->ModuleList.HDMIModule.Index = CBIOS_MODULE_INDEX_INVALID;
            pSrc->ModuleList.HDACModule.Index = CBIOS_MODULE_INDEX_INVALID;
        }

        cbPathMgrGeneratePath(pcbe, Device);
        break;
    case CBIOS_TYPE_CRT:
    case CBIOS_TYPE_DVO:
    case CBIOS_TYPE_DSI:
        cbPathMgrGeneratePath(pcbe, Device);
        break;
    default:
        cbDebugPrint((MAKE_LEVEL(GENERIC, ERROR),"%s: invalid device type: 0x%x.\n", FUNCTION_NAME, Device));
        return;
    }

    pSrc->bIsSrcChanged = CBIOS_TRUE;
}


CBIOS_MODULE_INDEX cbGetModuleIndex(PCBIOS_VOID pvcbe, CBIOS_ACTIVE_TYPE Device, CBIOS_MODULE_TYPE ModuleType)
{
    PCBIOS_EXTENSION_COMMON pcbe        = (PCBIOS_EXTENSION_COMMON)pvcbe;
    CBIOS_MODULE_INDEX      ModuleIndex = CBIOS_MODULE_INDEX_INVALID;
    CBIOS_DISPLAY_SOURCE   *pSource     = CBIOS_NULL;
    PCBIOS_DEVICE_COMMON    pDevCommon  = cbGetDeviceCommon(&pcbe->DeviceMgr, Device);

    if (pDevCommon == CBIOS_NULL)
    {
        cbDebugPrint((MAKE_LEVEL(GENERIC, ERROR), "%s: pDevCommon is NULL!\n", FUNCTION_NAME));
        return ModuleIndex;
    }

    pSource = &(pDevCommon->DispSource);

    switch (ModuleType)
    {
    case CBIOS_MODULE_TYPE_DP:
        ModuleIndex = pSource->ModuleList.DPModule.Index;
        if(ModuleIndex >= DP_MODU_NUM)
        {
            ModuleIndex = CBIOS_MODULE_INDEX_INVALID;
        }
        break;
    case CBIOS_MODULE_TYPE_MHL:
        ModuleIndex = pSource->ModuleList.MHLModule.Index;
        break;
    case CBIOS_MODULE_TYPE_HDMI:
        ModuleIndex = pSource->ModuleList.HDMIModule.Index;
        if(ModuleIndex >= HDMI_MODU_NUM)
        {
            ModuleIndex = CBIOS_MODULE_INDEX_INVALID;
        }
        break;
    case CBIOS_MODULE_TYPE_HDTV:
        ModuleIndex = pSource->ModuleList.HDTVModule.Index;
        break;
    case CBIOS_MODULE_TYPE_HDCP:
        ModuleIndex = pSource->ModuleList.HDCPModule.Index;
        break;
    case CBIOS_MODULE_TYPE_HDAC:
        ModuleIndex = pSource->ModuleList.HDACModule.Index;
        break;
    case CBIOS_MODULE_TYPE_IGA:
        ModuleIndex = pSource->ModuleList.IGAModule.Index;
        if((CBIOS_U32)ModuleIndex >= pcbe->DispMgr.IgaCount)
        {
            ModuleIndex = CBIOS_MODULE_INDEX_INVALID;
        }
        break;
    default:
        cbDebugPrint((MAKE_LEVEL(GENERIC, ERROR), "%s: invalid module type: %d!\n", FUNCTION_NAME, ModuleType));
        break;
    }

    if (ModuleIndex == CBIOS_MODULE_INDEX_INVALID)
    {
        cbDebugPrint((MAKE_LEVEL(GENERIC, DEBUG), "%s: Invalid module %d for device 0x%x!\n", FUNCTION_NAME, ModuleType, Device));
    }

    return ModuleIndex;
}

CBIOS_ACTIVE_TYPE cbGetDeviceType(PCBIOS_VOID pvcbe, CBIOS_MODULE_INDEX ModuleIndex, CBIOS_MODULE_TYPE ModuleType)
{
    PCBIOS_EXTENSION_COMMON pcbe        = (PCBIOS_EXTENSION_COMMON)pvcbe;
    CBIOS_U32                TmpDev = pcbe->DeviceMgr.SupportDevices;
    CBIOS_ACTIVE_TYPE        CurDev = CBIOS_TYPE_NONE;
    CBIOS_ACTIVE_TYPE        ActDev = CBIOS_TYPE_NONE;
    CBIOS_MODULE_INDEX      TmpModuleIndex = CBIOS_MODULE_INDEX_INVALID;

    if(ModuleIndex == CBIOS_MODULE_INDEX_INVALID || ModuleType == CBIOS_MODULE_TYPE_NONE)
    {
        return ActDev;
    }

    while (TmpDev)
    {
        CurDev = GET_LAST_BIT(TmpDev);
        TmpDev &= (~CurDev);

        TmpModuleIndex = cbGetModuleIndex(pcbe, CurDev, ModuleType);

        if(TmpModuleIndex == ModuleIndex)
        {
            ActDev = CurDev;
            break;
        }
    }

    return ActDev;
}

CBIOS_BOOL cbPathMgrModuleOnOff(PCBIOS_VOID pvcbe, CBIOS_MODULE **pModulePath, CBIOS_BOOL bTurnOn)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    CBIOS_U32 i = 0;

    if ((pModulePath == CBIOS_NULL) || (*pModulePath == CBIOS_NULL))
    {
        cbDebugPrint((MAKE_LEVEL(GENERIC, INFO), "%s: No sub module exists in this path!\n", FUNCTION_NAME));
        return CBIOS_FALSE;
    }

    if (bTurnOn)
    {
        while (pModulePath[i])
        {
            ++i;
        }
        --i;
    }

    while (pModulePath[i])
    {
        switch (pModulePath[i]->Type)
        {
        case CBIOS_MODULE_TYPE_HDMI:
            cbDIU_HDMI_ModuleOnOff(pcbe, pModulePath[i]->Index, bTurnOn);
            break;
        case CBIOS_MODULE_TYPE_HDTV:
            cbDIU_HDTV_ModuleOnOff(pcbe, pModulePath[i]->Index, bTurnOn);
            break;
        case CBIOS_MODULE_TYPE_HDCP:
        default:
            cbDebugPrint((MAKE_LEVEL(GENERIC, DEBUG), "%s: module %d does not need to be on/ff here!\n", FUNCTION_NAME, pModulePath[i]->Index));
            break;
        }

        if (bTurnOn)
        {
            if (i)
            {
                --i;
            }
            else
            {
                break;
            }
        }
        else
        {
            ++i;
        }
    }
    return CBIOS_TRUE;
}

CBIOS_BOOL cbPathMgrSelectDIUSource(PCBIOS_VOID pvcbe, CBIOS_DISPLAY_SOURCE *pSource)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    CBIOS_MODULE *pCurModule  = CBIOS_NULL;
    CBIOS_MODULE *pNextModule = CBIOS_NULL;
    CBIOS_U32 i = 0;

    if (pSource == CBIOS_NULL)
    {
        cbDebugPrint((MAKE_LEVEL(GENERIC, ERROR), "%s: the 2nd param is null!\n", FUNCTION_NAME));
        return CBIOS_FALSE;
    }

    while (pSource->ModulePath[i])
    {
        pCurModule = pSource->ModulePath[i];
        pNextModule = pSource->ModulePath[i + 1];

        switch (pCurModule->Type)
        {
        case CBIOS_MODULE_TYPE_HDMI:
            cbDIU_HDMI_SelectSource(pcbe, pCurModule, pNextModule);
            break;
        case CBIOS_MODULE_TYPE_HDCP:
        default:
            cbDebugPrint((MAKE_LEVEL(GENERIC, DEBUG), "%s: module %d does not need to select source here!\n", FUNCTION_NAME, pCurModule->Index));
            break;
        }
        i++;
    }

    return CBIOS_TRUE;
}

