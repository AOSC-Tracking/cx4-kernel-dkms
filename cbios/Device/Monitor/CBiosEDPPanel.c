/* 
* Copyright 2019-2020 Shanghai Zhaoxin Semiconductor Co., Ltd. All Rights Reserved.
*
* 
* 
* This file is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, either version 3 of the License, or
* (at your option) any later version.
*
* This file is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this file.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "CBiosChipShare.h"
#include "CBiosEDPPanel.h"
#include "../../Hw/HwBlock/CBiosDIU_DP.h"

extern CBIOS_EDP_PANEL_DESC ITN156_Panel_Desc;
extern CBIOS_EDP_PANEL_DESC EDO156_Panel_Desc;
extern CBIOS_EDP_PANEL_DESC DHUDH_Panel_Desc;

#define DEFAULT_EDP_PANEL_INDEX 0
#define ITN156_PANEL_INDEX 0

static PCBIOS_EDP_PANEL_DESC EDPPanelDescTbl[] = 
{
    &DHUDH_Panel_Desc,
    &EDO156_Panel_Desc,
    &ITN156_Panel_Desc,
};

static CBIOS_EDP_PANEL_DESC Default_EDP_Desc =
{
    /*.VersionNum = */CBIOS_EDP_VERSION,
    /*.MonitorID = */"zx",
    /*.EDPPreCaps =*/
    {
        /*.IsNeedPreInit = */CBIOS_FALSE,
        /*.pFnEDPPanelPreInit =*/ 0,
    },
    /*.EDPCaps = */
    {
        /*.LinkSpeed = */0,
        /*.LaneNum = */0,
        /*.BacklightMax = */255,
        /*.BacklightMin = */0,
        /*.Flags = */0x1,//backlight control = 1, not use hard code link para
    },
    /*.pFnEDPPanelInit = */CBIOS_NULL,
    /*.pFnEDPPanelDeInit = */CBIOS_NULL,
    /*.pFnEDPPanelOnOff = */CBIOS_NULL,
    /*.pFnEDPPanelSetBacklight = */CBIOS_NULL,
    /*.pFnEDPPanelGetBacklight = */CBIOS_NULL,
};

static CBIOS_BOOL cbEDPPanel_GetMonitorID(CBIOS_U8 *pEDID, CBIOS_U8 *pMnitorID)
{
    CBIOS_U8 *index = "0ABCDEFGHIJKLMNOPQRSTUVWXYZ[/]^_";
    CBIOS_U8 ProductID[3] = {0};
    CBIOS_BOOL bRet = CBIOS_FALSE;
    CBIOS_U8 *pMonitorIDinEDID = pEDID + 0x08;

    if ((pMonitorIDinEDID == CBIOS_NULL) || (pMnitorID == CBIOS_NULL))
    {
        cbDebugPrint((MAKE_LEVEL(GENERIC, ERROR),"cbEDPPanel_GetMonitorID: input buffer is null!\n"));
        return CBIOS_FALSE;
    }

    //get manufacturer ID
    pMnitorID[0] = index[(pMonitorIDinEDID[0] >> 2) & 0x1F];
    pMnitorID[1] = index[((pMonitorIDinEDID[1] >> 5) & 0x07) | ((pMonitorIDinEDID[0] << 3) & 0x18)];
    pMnitorID[2] = index[pMonitorIDinEDID[1] & 0x1F];
    pMnitorID[3] = '\0';

    if (cbItoA((CBIOS_U32)pMonitorIDinEDID[3], ProductID, 16, 2))
    {
        cbStrCat(pMnitorID, ProductID);
        if (cbItoA((CBIOS_U32)pMonitorIDinEDID[2], ProductID, 16, 2))
        {
            cbStrCat(pMnitorID, ProductID);
            bRet = CBIOS_TRUE;
            //cbDebugPrint((MAKE_LEVEL(GENERIC, INFO), "Monitor ID is: %s\n", pMnitorID));
        }
    }

    return bRet;

}

PCBIOS_EDP_PANEL_DESC cbEDPPanel_GetPanelDescriptor(PCBIOS_VOID pvcbe, PCBIOS_U8 pEdidData)
{
    PCBIOS_EDP_PANEL_DESC pPanelDesc = CBIOS_NULL;
    CBIOS_UCHAR  MonitorID[8];
    CBIOS_U32 i = 0;

    if (cbEDPPanel_GetMonitorID(pEdidData, MonitorID))// use monitor ID to get desc temporary,need sync with sysinfo
    {
        for (i = 0; i <  sizeofarray(EDPPanelDescTbl); i++)
        {
            if (cbStrnCmp(MonitorID, EDPPanelDescTbl[i]->MonitorID, 8) == 0)
            {
                pPanelDesc = EDPPanelDescTbl[i];
                break;
            }
        }
    }

    if (pPanelDesc == CBIOS_NULL)
    {
        pPanelDesc = &Default_EDP_Desc;
    }

    return pPanelDesc;
}

CBIOS_STATUS cbEDPPanel_PreInit(PCBIOS_VOID pvcbe)
{
    PCBIOS_EDP_PANEL_DESC pPanelDesc = CBIOS_NULL;
    CBIOS_U32 i = 0;

    for (i = 0; i <  sizeofarray(EDPPanelDescTbl); i++)
    {
        pPanelDesc = EDPPanelDescTbl[i];
        if (pPanelDesc->EDPPreCaps.IsNeedPreInit)
        {
            if(pPanelDesc->EDPPreCaps.pFnEDPPanelPreInit != CBIOS_NULL)
            {
                pPanelDesc->EDPPreCaps.pFnEDPPanelPreInit(pvcbe);
            }
        }
    }
    
    return CBIOS_OK;
}

CBIOS_STATUS cbEDPPanel_Init(PCBIOS_VOID pvcbe, PCBIOS_EDP_PANEL_DESC pPanelDesc)
{
    CBIOS_STATUS Status = CBIOS_ER_NOT_YET_IMPLEMENTED;
    
    if (pPanelDesc != CBIOS_NULL)
    {
        if (pPanelDesc->pFnEDPPanelInit != CBIOS_NULL)
        {
            Status = pPanelDesc->pFnEDPPanelInit(pvcbe);
        }
        else
        {
            cbDebugPrint((MAKE_LEVEL(DP, DEBUG), "function %s not implemented.\n", FUNCTION_NAME));
        }
        Status = CBIOS_OK;
        
    }
    else
    {
        Status = CBIOS_ER_NULLPOINTER;
    }
    
 
    return Status;
}


CBIOS_STATUS cbEDPPanel_DeInit(PCBIOS_VOID pvcbe, PCBIOS_EDP_PANEL_DESC pPanelDesc)
{
    CBIOS_STATUS Status = CBIOS_ER_NOT_YET_IMPLEMENTED;
    
    if (pPanelDesc != CBIOS_NULL)
    {
        if (pPanelDesc->pFnEDPPanelDeInit != CBIOS_NULL)
        {
            Status = pPanelDesc->pFnEDPPanelDeInit(pvcbe);
        }
        else
        {
            cbDebugPrint((MAKE_LEVEL(DP, DEBUG), "function %s not implemented.\n", FUNCTION_NAME));
        }
        Status = CBIOS_OK;
        
    }
    else
    {
        Status = CBIOS_ER_NULLPOINTER;
    }
    
 
    return Status;
}


CBIOS_STATUS cbEDPPanel_OnOff(PCBIOS_VOID pvcbe, CBIOS_MODULE_INDEX DPModuleIndex, CBIOS_BOOL bTurnOn, CBIOS_U32 BacklightValue, PCBIOS_EDP_PANEL_DESC pPanelDesc)
{
    CBIOS_STATUS Status = CBIOS_ER_NOT_YET_IMPLEMENTED;

    if (DPModuleIndex >= DP_MODU_NUM)
    {
        cbDebugPrint((MAKE_LEVEL(DP, ERROR), "%s: invalid DP module index!\n", FUNCTION_NAME));
        return  CBIOS_ER_INTERNAL;
    }

    if (pPanelDesc != CBIOS_NULL)
    {
        if (pPanelDesc->pFnEDPPanelOnOff != CBIOS_NULL)
        {
            Status = pPanelDesc->pFnEDPPanelOnOff(pvcbe, DPModuleIndex, bTurnOn, BacklightValue);
        }
        else
        {
            cbDIU_EDP_SetBacklight(pvcbe, DPModuleIndex, BacklightValue);
        }

        Status = CBIOS_OK;
    }
    else
    {
        Status = CBIOS_ER_NULLPOINTER;
    }
 
    return Status;
}

CBIOS_STATUS cbEDPPanel_SetBacklight(PCBIOS_VOID pvcbe, CBIOS_MODULE_INDEX DPModuleIndex, CBIOS_U32 BacklightValue, PCBIOS_EDP_PANEL_DESC pPanelDesc)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    PCBIOS_EDP_CAPS pEDPCaps = CBIOS_NULL;
    CBIOS_STATUS Status = CBIOS_OK;
    CBIOS_U32 BlValue = 0;

    if (DPModuleIndex >= DP_MODU_NUM)
    {
        cbDebugPrint((MAKE_LEVEL(DP, ERROR), "%s: invalid DP module index!\n", FUNCTION_NAME));
        return  CBIOS_ER_INTERNAL;
    }

    if (pPanelDesc != CBIOS_NULL)
    {
        BlValue = BacklightValue;

        if (pPanelDesc->pFnEDPPanelSetBacklight != CBIOS_NULL)
        {
            pEDPCaps = &(pPanelDesc->EDPCaps);

            if (BacklightValue < pEDPCaps->BacklightMin)
            {
                cbDebugPrint((MAKE_LEVEL(DP, WARNING), "%s: backlight value: %d is smaller than the min backlight value, use the min.\n", FUNCTION_NAME, BacklightValue));
                BlValue = pEDPCaps->BacklightMin;
            }
            if (BacklightValue > pEDPCaps->BacklightMax)
            {
                cbDebugPrint((MAKE_LEVEL(DP, WARNING), "%s: backlight value: %d is greater than the max backlight value, use the max.\n", FUNCTION_NAME, BacklightValue));
                BlValue = pEDPCaps->BacklightMax;
            }

            Status = pPanelDesc->pFnEDPPanelSetBacklight(pvcbe, DPModuleIndex, BlValue);
        }
        else
        {
            Status = cbDIU_EDP_SetBacklight(pcbe, DPModuleIndex, BlValue);
        }
    }
    else
    {
        Status = CBIOS_ER_NULLPOINTER;
    }

    return Status;
}

CBIOS_STATUS cbEDPPanel_GetBacklight(PCBIOS_VOID pvcbe, CBIOS_MODULE_INDEX DPModuleIndex, CBIOS_U32 *pBacklightValue, PCBIOS_EDP_PANEL_DESC pPanelDesc)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    CBIOS_STATUS Status = CBIOS_OK;

    if (DPModuleIndex >= DP_MODU_NUM)
    {
        cbDebugPrint((MAKE_LEVEL(DP, ERROR), "%s: invalid DP module index!\n", FUNCTION_NAME));
        return  CBIOS_ER_INTERNAL;
    }

    if (pPanelDesc != CBIOS_NULL)
    {
        if (pPanelDesc->pFnEDPPanelGetBacklight!= CBIOS_NULL)
        {
            Status = pPanelDesc->pFnEDPPanelGetBacklight(pvcbe, DPModuleIndex, pBacklightValue);
        }
        else
        {
            Status = cbDIU_EDP_GetBacklight(pcbe, DPModuleIndex, pBacklightValue);
        }
    }
    else
    {
        Status = CBIOS_ER_NULLPOINTER;
    }

    return Status;
}
