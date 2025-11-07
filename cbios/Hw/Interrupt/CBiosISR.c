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
** CBios interrupt service routine functions implementation.
**
** NOTE:
** The print, delay and mutex lock SHOULD NOT be called in isr functions.
******************************************************************************/

#include "CBiosChipShare.h"
#include "../CBiosHwShare.h"
#include "../Register/BIU_SBI_registers.h"
#include "../HwBlock/CBiosDIU_DP.h"


CBIOS_STATUS cbGetCECInterruptInfo(PCBIOS_VOID pvcbe, PCBIOS_CEC_INTERRUPT_INFO pCECIntInfo)
{
    CBIOS_STATUS    Status = CBIOS_OK;
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    CEC_MISC_REG2   CECMiscReg2;

    if (pCECIntInfo == CBIOS_NULL)
    {
        Status = CBIOS_ER_NULLPOINTER;
        cbDebugPrint((MAKE_LEVEL(HDMI, ERROR), "cbGetCECInterruptInfo: pCECIntInfo is NULL!"));
    }
    else if (!pcbe->ChipCaps.IsSupportCEC)
    {
        Status = CBIOS_ER_HARDWARE_LIMITATION;
        cbDebugPrint((MAKE_LEVEL(HDMI, ERROR), "cbGetCECInterruptInfo: Can't support CEC!"));
    }
    else if (!(pCECIntInfo->InterruptBitMask & BIT6))
    {
        Status = CBIOS_OK;
        pCECIntInfo->InterruptType = INVALID_CEC_INTERRUPT;
    }
    else
    {
        //set to invalid interrupt by default
        pCECIntInfo->InterruptType = INVALID_CEC_INTERRUPT;

        //first check CEC1
        CECMiscReg2.CECMiscReg2Value = cb_ReadU32(pcbe->pAdapterContext, 0x33150);

        if (CECMiscReg2.FolReceiveReady)
        {
            pCECIntInfo->CEC1MsgReceived = 1;
            pCECIntInfo->InterruptType = NORMAL_CEC_INTERRUPT;
        }
        else
        {
            pCECIntInfo->CEC1MsgReceived = 0;
        }


        //CEC2
        CECMiscReg2.CECMiscReg2Value = cb_ReadU32(pcbe->pAdapterContext, 0x33e3c);

        if (CECMiscReg2.FolReceiveReady)
        {
            pCECIntInfo->CEC2MsgReceived = 1;
            pCECIntInfo->InterruptType = NORMAL_CEC_INTERRUPT;
        }
        else
        {
            pCECIntInfo->CEC2MsgReceived = 0;
        }

        //CEC3
        CECMiscReg2.CECMiscReg2Value = cb_ReadU32(pcbe->pAdapterContext, 0x34540);

        if (CECMiscReg2.FolReceiveReady)
        {
            pCECIntInfo->CEC3MsgReceived = 1;
            pCECIntInfo->InterruptType = NORMAL_CEC_INTERRUPT;
        }
        else
        {
            pCECIntInfo->CEC3MsgReceived = 0;
        }

        //CEC4
        CECMiscReg2.CECMiscReg2Value = cb_ReadU32(pcbe->pAdapterContext, 0x34c40);

        if (CECMiscReg2.FolReceiveReady)
        {
            pCECIntInfo->CEC4MsgReceived = 1;
            pCECIntInfo->InterruptType = NORMAL_CEC_INTERRUPT;
        }
        else
        {
            pCECIntInfo->CEC4MsgReceived = 0;
        }

        Status = CBIOS_OK;

    }
    return Status;
}

CBIOS_STATUS cbGetHDCPInterruptInfo(PCBIOS_VOID pvcbe, PCBIOS_HDCP_INFO_PARA pHdcpInfoParam)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    CBIOS_STATUS Status = CBIOS_OK;
    REG_MM82C8  RegMM82C8Value, RegMM33C70Value, RegMM34370Value, RegMM34A70Value;
    REG_MM8374  RegMM8374Value, RegMM33C78Value, RegMM34378Value, RegMM34A78Value;
    REG_MM3368C RegMM3368CValue, RegMM33C88Value, RegMM34388Value, RegMM34A88Value;

    if(pHdcpInfoParam == CBIOS_NULL || pHdcpInfoParam->InterruptType != CBIOS_HDCP_INT)
    {
        Status = CBIOS_ER_INVALID_PARAMETER;
    }
    else
    {
        pHdcpInfoParam->IntDevicesId = CBIOS_TYPE_NONE;
        RegMM82C8Value.Value = cb_ReadU32(pcbe->pAdapterContext, 0x82C8);
        RegMM33C70Value.Value = cb_ReadU32(pcbe->pAdapterContext, 0x33C70);
        RegMM34370Value.Value = cb_ReadU32(pcbe->pAdapterContext, 0x34370);
        RegMM34A70Value.Value = cb_ReadU32(pcbe->pAdapterContext, 0x34A70);

        if(RegMM82C8Value.HDCP1_Interrupt)
        {
            pHdcpInfoParam->IntDevicesId |= CBIOS_TYPE_DP1;
            RegMM82C8Value.HDCP1_Interrupt = 0;
            cb_WriteU32(pcbe->pAdapterContext, 0x82C8, RegMM82C8Value.Value);
        }

        if(RegMM33C70Value.HDCP1_Interrupt)
        {
            pHdcpInfoParam->IntDevicesId |= CBIOS_TYPE_DP2;
            RegMM33C70Value.HDCP1_Interrupt = 0;
            cb_WriteU32(pcbe->pAdapterContext, 0x33C70, RegMM33C70Value.Value);
        }

        if(RegMM34370Value.HDCP1_Interrupt)
        {
            pHdcpInfoParam->IntDevicesId |= CBIOS_TYPE_DP3;
            RegMM34370Value.HDCP1_Interrupt = 0;
            cb_WriteU32(pcbe->pAdapterContext, 0x34370, RegMM34370Value.Value);
        }

        if(RegMM34A70Value.HDCP1_Interrupt)
        {
            pHdcpInfoParam->IntDevicesId |= CBIOS_TYPE_DP4;
            RegMM34A70Value.HDCP1_Interrupt = 0;
            cb_WriteU32(pcbe->pAdapterContext, 0x34A70, RegMM34A70Value.Value);
        }

        RegMM8374Value.Value = cb_ReadU32(pcbe->pAdapterContext, 0x8374);
        RegMM33C78Value.Value = cb_ReadU32(pcbe->pAdapterContext, 0x33C78);
        RegMM34378Value.Value = cb_ReadU32(pcbe->pAdapterContext, 0x34378);
        RegMM34A78Value.Value = cb_ReadU32(pcbe->pAdapterContext, 0x34A78);
        
        if(RegMM8374Value.DP_HDCP_INT)
        {
            pHdcpInfoParam->IntDevicesId |= CBIOS_TYPE_DP1;
            RegMM8374Value.DP_HDCP_INT = 0;
            cb_WriteU32(pcbe->pAdapterContext, 0x8374, RegMM8374Value.Value);
        }

        if(RegMM33C78Value.DP_HDCP_INT)
        {
            pHdcpInfoParam->IntDevicesId |= CBIOS_TYPE_DP2;
            RegMM33C78Value.DP_HDCP_INT = 0;
            cb_WriteU32(pcbe->pAdapterContext, 0x33C78, RegMM33C78Value.Value);
        }
        
        if(RegMM34378Value.DP_HDCP_INT)
        {
            pHdcpInfoParam->IntDevicesId |= CBIOS_TYPE_DP3;
            RegMM34378Value.DP_HDCP_INT = 0;
            cb_WriteU32(pcbe->pAdapterContext, 0x34378, RegMM34378Value.Value);
        }
        
        if(RegMM34A78Value.DP_HDCP_INT)
        {
            pHdcpInfoParam->IntDevicesId |= CBIOS_TYPE_DP4;
            RegMM34A78Value.DP_HDCP_INT = 0;
            cb_WriteU32(pcbe->pAdapterContext, 0x34A78, RegMM34A78Value.Value);
        }

        RegMM3368CValue.Value = cb_ReadU32(pcbe->pAdapterContext, 0x3368C);
        RegMM33C88Value.Value = cb_ReadU32(pcbe->pAdapterContext, 0x33C88);
        RegMM34388Value.Value = cb_ReadU32(pcbe->pAdapterContext, 0x34388);
        RegMM34A88Value.Value = cb_ReadU32(pcbe->pAdapterContext, 0x34A88);

        if(RegMM3368CValue.Value != 0)
        {
            pHdcpInfoParam->IntDevicesId |= CBIOS_TYPE_DP1;
        }

        if(RegMM33C88Value.Value != 0)
        {
            pHdcpInfoParam->IntDevicesId |= CBIOS_TYPE_DP2;
        }

        if(RegMM34388Value.Value != 0)
        {
            pHdcpInfoParam->IntDevicesId |= CBIOS_TYPE_DP3;
        }

        if(RegMM34A88Value.Value != 0)
        {
            pHdcpInfoParam->IntDevicesId |= CBIOS_TYPE_DP4;
        }
    }

    return Status;
}

CBIOS_STATUS cbGetHDACInterruptInfo(PCBIOS_VOID pvcbe, PCBIOS_HDAC_INFO_PARA pHdacInfoParam)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    CBIOS_STATUS Status = CBIOS_OK;
    REG_MM8288 RegMM8288Value;
    REG_MM33D9C RegMM33D9CValue;
    REG_MM344A0 RegMM344A0Value;

    if(pHdacInfoParam == CBIOS_NULL || pHdacInfoParam->InterruptType != CBIOS_HDA_CODEC_INT)
    {
        Status = CBIOS_ER_INVALID_PARAMETER;
    }
    else
    {
        pHdacInfoParam->IntDevicesId = CBIOS_TYPE_NONE;
        RegMM8288Value.Value = cb_ReadU32(pcbe->pAdapterContext, 0x8288);

        if(RegMM8288Value.Int_Src_Codec1)
        {
            pHdacInfoParam->IntDevicesId |= CBIOS_TYPE_DP1;
            RegMM8288Value.Int_Src_Codec1 = 0;
            cb_WriteU32(pcbe->pAdapterContext, 0x8288, RegMM8288Value.Value);
        }

        RegMM33D9CValue.Value = cb_ReadU32(pcbe->pAdapterContext, 0x33D9C);
        if(RegMM33D9CValue.Int_Src_Codec2)
        {
            pHdacInfoParam->IntDevicesId |= CBIOS_TYPE_DP2;
            RegMM33D9CValue.Int_Src_Codec2 = 0;
            cb_WriteU32(pcbe->pAdapterContext, 0x33D9C, RegMM33D9CValue.Value);
        }

        if(pcbe->ChipID == CHIPID_CNE001)
        {
            RegMM344A0Value.Value = cb_ReadU32(pcbe->pAdapterContext, 0x344A0);
            if(RegMM344A0Value.Int_Src_Codec3)
            {
                pHdacInfoParam->IntDevicesId |= CBIOS_TYPE_DP3;
                RegMM344A0Value.Int_Src_Codec3 = 0;
                cb_WriteU32(pcbe->pAdapterContext, 0x344A0, RegMM344A0Value.Value);
            }
        }
    }

    return Status;
}
