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
** Device port sharing function implementation.
**
** NOTE:
** 
******************************************************************************/

#include "CBiosDeviceShare.h"
#include "CBiosChipShare.h"

CBIOS_U8 FPGAHDMIEdid[256] =
{
    0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x21, 0x34, 0x03, 0x7D, 0x43, 0x41, 0x32, 0x01,
    0x1E, 0x11, 0x01, 0x03, 0x80, 0x34, 0x20, 0x64, 0x0A, 0xEF, 0x95, 0xA3, 0x54, 0x4C, 0x9B, 0x26,
    0x0F, 0x50, 0x54, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xD5, 0x09, 0x80, 0xA0, 0x20, 0xE0, 0x2D, 0x10, 0x10, 0x60,
    0xA2, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1E, 0x01, 0x1D, 0x00, 0x72, 0x51, 0xD0, 0x1E, 0x20,
    0x6E, 0x28, 0x55, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1E, 0x02, 0x3A, 0x80, 0x18, 0x71, 0x38,
    0x2D, 0x40, 0x58, 0x2C, 0x45, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1E, 0x00, 0x00, 0x00, 0xFC,
    0x00, 0x57, 0x32, 0x34, 0x30, 0x44, 0x20, 0x44, 0x56, 0x49, 0x0A, 0x20, 0x20, 0x20, 0x01, 0xFB,
    0x02, 0x03, 0x1C, 0x71, 0x23, 0x09, 0x07, 0x07, 0x49, 0x90, 0x14, 0x13, 0x12, 0x05, 0x04, 0x03,
    0x02, 0x01, 0x65, 0x03, 0x0C, 0x00, 0x10, 0x00, 0x83, 0x01, 0x00, 0x00, 0x8C, 0x0A, 0xD0, 0x90,
    0x20, 0x40, 0x31, 0x20, 0x0C, 0x40, 0x55, 0x00, 0x13, 0x8E, 0x21, 0x00, 0x00, 0x18, 0x01, 0x1D,
    0x80, 0x18, 0x71, 0x1C, 0x16, 0x20, 0x58, 0x2C, 0x25, 0x00, 0xC4, 0x8E, 0x21, 0x00, 0x00, 0x9E,
    0x01, 0x1D, 0x00, 0x72, 0x51, 0xD0, 0x1E, 0x20, 0x6E, 0x28, 0x55, 0x00, 0xC4, 0x8E, 0x21, 0x00,
    0x00, 0x1E, 0x8C, 0x0A, 0xD0, 0x8A, 0x20, 0xE0, 0x2D, 0x10, 0x10, 0x3E, 0x96, 0x00, 0xC4, 0x8E,
    0x21, 0x00, 0x00, 0x18, 0x8C, 0x0A, 0xD0, 0x8A, 0x20, 0xE0, 0x2D, 0x10, 0x10, 0x3E, 0x96, 0x00,
    0x13, 0x8E, 0x21, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC4,
};

#if IS_SUPPORT_4K_MODE
CBIOS_U8 Fake4KEdid[256] =
{
    0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x36, 0x74, 0x30, 0x00, 0x01, 0x00, 0x00, 0x00,
    0x0A, 0x16, 0x01, 0x03, 0x80, 0x73, 0x41, 0x78, 0x0A, 0xCF, 0x74, 0xA3, 0x57, 0x4C, 0xB0, 0x23,
    0x09, 0x48, 0x4C, 0x21, 0x08, 0x00, 0x81, 0x80, 0x45, 0x40, 0x61, 0x40, 0x95, 0x00, 0x01, 0x01,
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x02, 0x3A, 0x80, 0x18, 0x71, 0x38, 0x2D, 0x40, 0x58, 0x2C,
    0x45, 0x00, 0xC4, 0x8E, 0x21, 0x00, 0x00, 0x1E, 0x66, 0x21, 0x50, 0xB0, 0x51, 0x00, 0x1B, 0x30,
    0x40, 0x70, 0x36, 0x00, 0xC4, 0x8E, 0x21, 0x00, 0x00, 0x1E, 0x00, 0x00, 0x00, 0xFC, 0x00, 0x4D,
    0x53, 0x74, 0x61, 0x72, 0x20, 0x44, 0x65, 0x6D, 0x6F, 0x0A, 0x20, 0x20, 0x00, 0x00, 0x00, 0xFD,
    0x00, 0x32, 0x4B, 0x1E, 0x50, 0x17, 0x00, 0x0A, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x01, 0xF1,
    0x02, 0x03, 0x30, 0xF2, 0x4D, 0x01, 0x03, 0x04, 0x05, 0x07, 0x90, 0x12, 0x13, 0x14, 0x16, 0x9F,
    0x20, 0x22, 0x26, 0x09, 0x07, 0x07, 0x11, 0x17, 0x50, 0x83, 0x01, 0x00, 0x00, 0x72, 0x03, 0x0C,
    0x00, 0x40, 0x00, 0xB8, 0x44, 0x20, 0xC0, 0x84, 0x01, 0x02, 0x03, 0x04, 0x01, 0x41, 0x00, 0x00,
    0x8C, 0x0A, 0xD0, 0x8A, 0x20, 0xE0, 0x2D, 0x10, 0x10, 0x3E, 0x96, 0x00, 0xC4, 0x8E, 0x21, 0x00,
    0x00, 0x18, 0x8C, 0x0A, 0xD0, 0x90, 0x20, 0x40, 0x31, 0x20, 0x0C, 0x40, 0x55, 0x00, 0xC4, 0x8E,
    0x21, 0x00, 0x00, 0x18, 0x01, 0x1D, 0x00, 0xBC, 0x52, 0xD0, 0x1E, 0x20, 0xB8, 0x28, 0x55, 0x40,
    0xC4, 0x8E, 0x21, 0x00, 0x00, 0x1E, 0x01, 0x1D, 0x80, 0xD0, 0x72, 0x1C, 0x16, 0x20, 0x10, 0x2C,
    0x25, 0x80, 0xC4, 0x8E, 0x21, 0x00, 0x00, 0x9E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3F,
};
#endif


CBIOS_VOID cbFixEdidChecksum(PCBIOS_VOID pvcbe, PCBIOS_UCHAR  pEdidData, CBIOS_U32  ulLength)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    CBIOS_U32 ulOnceBlockSize = 128, i = 0;
    CBIOS_U8 checksum = 0;

    if(ulLength == 0 || !pEdidData || (ulLength % ulOnceBlockSize) != 0)
    {
        return;
    }

    for(i = 0; i < ulLength; i++)
    {
        checksum += pEdidData[i];

        if(((i + 2) % ulOnceBlockSize) == 0)
        {
            pEdidData[i+1] = (CBIOS_U8)(~checksum) + 1;
            checksum = 0;
            i++;
        }
    }
}

CBIOS_BOOL cbIsEdidChecksumValid(PCBIOS_UCHAR  pEdid, CBIOS_U32  ulLength)
{
    CBIOS_BOOL  bRet = CBIOS_FALSE;
    CBIOS_U32 i = 0;
    CBIOS_U8 checksum = 0;

    if(ulLength == 0 || !pEdid || (ulLength % EDID_BLK_SIZE) != 0)
    {
        return bRet;
    }

    bRet = CBIOS_TRUE;
    
    for(i = 0; i < ulLength; i++)
    {
        checksum += pEdid[i];
        if(((i + 1) % EDID_BLK_SIZE) == 0 && checksum != 0)
        {
            bRet = CBIOS_FALSE;
            break;
        }
    }

    return bRet;
}

CBIOS_STATUS cbI2cNormal_ReadEDID(PCBIOS_VOID pvcbe, PCBIOS_DEVICE_COMMON pDevCommon, CBIOS_U8 *EDIDData, CBIOS_U32 ulReadEdidOffset, CBIOS_U32 ulBufferSize, CBIOS_U8 nSegNum)
{
    PCBIOS_EXTENSION_COMMON    pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    CBIOS_STATUS bStatus = CBIOS_ER_INTERNAL;
    CBIOS_MODULE_I2C_PARAMS par = {0};
    CBIOS_U8 I2CAddress = 0xA0, SegAddress = 0x60, j = 0;
    CBIOS_U8  I2CBUSNum = (CBIOS_U8)pDevCommon->I2CBus;
    CBIOS_U8  I2CDelay = (CBIOS_U8)pDevCommon->I2CDelay;
    // Sometimes I2C is not stably at begining of start, so edid may be incorrct.
    // Here, we read edid maximum three times if edid sum check failed.
    for(j=1; j<4; j++)
    { 
        //First write segment index
        if(nSegNum != 0)
        {
            par.SlaveAddress = SegAddress;
            par.I2CBusNum = I2CBUSNum;
            par.I2CDelay = I2CDelay;
            par.Buffer = &nSegNum;
            par.OffSet = 0;
            par.BufferLen = 1;

            bStatus = cbI2C_WriteData(pcbe, &par);
            if(bStatus != CBIOS_OK)
            {
                cbDebugPrint((MAKE_LEVEL(GENERIC, WARNING),"%s: Write segment num failed for device 0x%x!\n", FUNCTION_NAME, pDevCommon->DeviceType));
                break;
            }
        }

		//get edid
        par.SlaveAddress = I2CAddress;
        par.I2CBusNum = I2CBUSNum;
        par.I2CDelay = I2CDelay;
        par.Buffer = EDIDData;
        par.OffSet = (CBIOS_UCHAR)ulReadEdidOffset;
        par.BufferLen = ulBufferSize;
        bStatus = cbI2C_ReadData(pcbe, &par);
        if(bStatus != CBIOS_OK)
        {
            break;
        }

        if((ulBufferSize % 128) == 0 && (ulReadEdidOffset % 128) == 0
            && !cbIsEdidChecksumValid(EDIDData, ulBufferSize)) //block alignment edid, and checksum failed
        {
            bStatus = CBIOS_ER_INTERNAL;
            cbDebugPrint((MAKE_LEVEL(GENERIC, WARNING), "%s: EDID checksum of device 0x%x is wrong!\n", FUNCTION_NAME, pDevCommon->DeviceType));
            cb_DelayMicroSeconds(j * 50);
            continue;
        }
        else //part of EDID
        {
            bStatus = CBIOS_OK;
            break; 
        }
    }
    
    return bStatus;
}

CBIOS_BOOL cbI2cOverAux_ReadEDID(PCBIOS_VOID pvcbe, PCBIOS_DEVICE_COMMON pDevCommon, PCBIOS_UCHAR pEDIDBuffer,
                                   CBIOS_U32 ulBufferSize, CBIOS_U32 ulReadEdidOffset,CBIOS_U8  nSegNum)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    CBIOS_BOOL              bRet = CBIOS_FALSE;
    CBIOS_STATUS            bStatus = CBIOS_ER_INVALID_PARAMETER;
    CBIOS_ACTIVE_TYPE       Device = pDevCommon->DeviceType;
    CBIOS_PARAM_AUX_DATA    ParamAuxData = {0};
    PCBIOS_DP_MONITOR_CONTEXT  pDPMonitorContext = cbGetDPMonitorContext(pcbe, pDevCommon);

    cbTraceEnter(DP);

    if(!pDPMonitorContext)
    {
        return CBIOS_FALSE;
    }

    if (pDPMonitorContext->DpAuxWorkingStatus & AUX_WORKING_STATUS_DATA_READING)
    {
        cbDebugPrint((MAKE_LEVEL(DP, ERROR), "%s: this routine re-rentered!!!\n", FUNCTION_NAME));
        return CBIOS_FALSE;
    }
    else if (pDPMonitorContext->DpAuxWorkingStatus & AUX_WORKING_STATUS_LINKTRAINING)
    {
        cbDebugPrint((MAKE_LEVEL(DP, WARNING), "%s: AUX CHANNEL IS BUSY LINKTRAINING!!!\n", FUNCTION_NAME));
    }

    pDPMonitorContext->DpAuxWorkingStatus |= AUX_WORKING_STATUS_DATA_READING;

    ParamAuxData.DeviceId = Device;
    ParamAuxData.Write = 0;
    ParamAuxData.BytesRequested = ulBufferSize;
    ParamAuxData.Address = 0xA0;
    ParamAuxData.Buffer = pEDIDBuffer;
    ParamAuxData.Offset = (CBIOS_UCHAR)ulReadEdidOffset;
    ParamAuxData.EDDCMode = (nSegNum != 0) ? 1 : 0;
    ParamAuxData.SegNum = nSegNum;

    bStatus = cbDevAux_DataAccess(pcbe, &ParamAuxData);

    if(bStatus != CBIOS_OK)
    {
         cbDebugPrint((MAKE_LEVEL(DP, ERROR),"%s: Aux read edid failed for device 0x%x!\n", FUNCTION_NAME, Device));
         goto ExitFunc;
    }

    if((ulReadEdidOffset % 128) == 0 && (ulBufferSize % 128) == 0
        && !cbIsEdidChecksumValid(pEDIDBuffer, ulBufferSize))
    {
        cbDebugPrint((MAKE_LEVEL(DP, WARNING),"%s: Edid checksum of device 0x%x is wrong!\n", FUNCTION_NAME, Device));
        goto ExitFunc;
    }

    bRet = CBIOS_TRUE;

ExitFunc:
    pDPMonitorContext->DpAuxWorkingStatus &= ~AUX_WORKING_STATUS_DATA_READING;

    if (!bRet)
    {
        //Corrupted EDID, zero it to avoid be analyzed elsewhere without check corruption.
        cbDumpBuffer(pcbe, pEDIDBuffer, ulBufferSize);
    }

    return bRet;
}

CBIOS_BOOL cbGetEdidPerSeg(PCBIOS_VOID pvcbe, PCBIOS_DEVICE_COMMON pDevCommon, CBIOS_U8 *EDIDData, CBIOS_U32 ulReadEdidOffset, CBIOS_U32 ulBufferSize, CBIOS_U8  nSegNum)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    CBIOS_BOOL  bRet = CBIOS_FALSE;
    CBIOS_STATUS bStatus = CBIOS_OK;

    if(cbDPPort_IsDeviceInDpMode(pcbe, pDevCommon))
    {
        bRet = cbI2cOverAux_ReadEDID(pcbe, pDevCommon, EDIDData, ulBufferSize, ulReadEdidOffset, nSegNum);
    }
    else
    {
        bStatus = cbI2cNormal_ReadEDID(pcbe, pDevCommon, EDIDData, ulReadEdidOffset, ulBufferSize, nSegNum);
        if((bStatus == CBIOS_ER_HDCP_USING_I2C) && cbEDIDModule_IsEDIDHeaderValid(pDevCommon->EdidData.Buffer, pDevCommon->EdidData.BufferSize) &&
            (nSegNum * 256 + ulReadEdidOffset + ulBufferSize <= pDevCommon->EdidData.BufferSize))
        {
            cb_memcpy(EDIDData, &(pDevCommon->EdidData.Buffer[nSegNum * 256 + ulReadEdidOffset]), ulBufferSize);
            bStatus = CBIOS_OK;
        }

        bRet = (bStatus == CBIOS_OK) ? CBIOS_TRUE : CBIOS_FALSE;
    }

    return  bRet;
}

CBIOS_BOOL cbGetExtBlockNum(PCBIOS_EXTENSION_COMMON pcbe, PCBIOS_DEVICE_COMMON pDevCommon, CBIOS_U32 *pExtBlockNum)
{
    CBIOS_BOOL bStatus = CBIOS_FALSE;
    CBIOS_U8   ExtFlag = 0; // EDID[0x7E]
    CBIOS_U8   pEeodb[3] = {0};

    if (!pcbe || !pDevCommon)
    {
        cbDebugPrint((MAKE_LEVEL(GENERIC, ERROR), "%s: no valid parameter to access EDID\n", FUNCTION_NAME));
        return bStatus;
    }

    bStatus = cbGetEdidPerSeg(pcbe, pDevCommon, &ExtFlag, 0x7E, 1, 0); //read Extension Flag
    if(!bStatus || !ExtFlag)
    {
        *pExtBlockNum = 0;
    }
    else
    {
        bStatus = cbGetEdidPerSeg(pcbe, pDevCommon, pEeodb, 0x84, 3, 0); //read HF-EEODB
        if (bStatus &&
            (pEeodb[0] >> 5 == CEA_EXTENDED_BLOCK_TAG) &&
            (pEeodb[1]  == HF_EDID_EXTENSION_OVERRIDE_DATA_BLOCK))
        {
            *pExtBlockNum = pEeodb[2];
        }
        else
        {
            *pExtBlockNum = ExtFlag;
        }
    }

    if (!bStatus)
    {
        cbDebugPrint((MAKE_LEVEL(GENERIC, ERROR), "%s: read EDID bytes fail\n", FUNCTION_NAME));
    }

    return bStatus;
}

//deal with special request, offset is not 0, read count is not times of 1 block.
CBIOS_BOOL cbGetEdidBytes(PCBIOS_VOID pvcbe, PCBIOS_DEVICE_COMMON pDevCommon, CBIOS_U8 *EDIDData, CBIOS_U32 ulReadEdidOffset, CBIOS_U32 ulBufferSize)
{
    //CBIOS_U8 edid_address[] = {0xA0,0x00,0xA2,0x20,0xA6,0x20};
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    CBIOS_BOOL  bStatus = CBIOS_FALSE;
    CBIOS_U32 ulExtsNum = 0, ulBlkIndex = 0;
    CBIOS_U32  ulToBlockIdx = (ulReadEdidOffset + ulBufferSize) / EDID_BLK_SIZE;   //the blk index that holds that last byte, calculate from 1
    CBIOS_U32  ulReadSize = 0;

    if(!EDIDData || ulBufferSize == 0)
    {
        return CBIOS_FALSE;
    }

    if((ulReadEdidOffset + ulBufferSize) % EDID_BLK_SIZE)
    {
        ulToBlockIdx++;
    }

    if(ulToBlockIdx > 1)
    {
        bStatus = cbGetExtBlockNum(pcbe, pDevCommon, &ulExtsNum);
        if(!bStatus || ulToBlockIdx > (ulExtsNum + 1))
        {
            cb_memset(EDIDData, 0, ulBufferSize);
            return CBIOS_FALSE;
        }
    }

    while(ulReadSize < ulBufferSize)
    {
        CBIOS_U32  ulStartSeg = (ulReadEdidOffset + ulReadSize)/256;
        CBIOS_U32  ulOffsetInSeg = (ulReadEdidOffset + ulReadSize) % 256;
        CBIOS_U32  ulOneceSize = cb_min(256 - ulOffsetInSeg, ulBufferSize - ulReadSize);
        bStatus = cbGetEdidPerSeg(pcbe, pDevCommon, EDIDData+ulReadSize, ulOffsetInSeg, ulOneceSize, (CBIOS_U8)ulStartSeg);
        if(bStatus)
        {
            ulReadSize += ulOneceSize;
        }
        else
        {
            cb_memset(EDIDData, 0, ulBufferSize);
            return CBIOS_FALSE;
        }
    }

    return bStatus;
}

PCBIOS_U8 cbGetFullEdid(PCBIOS_VOID pvcbe, PCBIOS_DEVICE_COMMON pDevCommon, CBIOS_U32 *ReadEdidSize)
{
    //CBIOS_U8 edid_address[] = {0xA0,0x00,0xA2,0x20,0xA6,0x20};
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    CBIOS_U16  DCLK = 0;
    CBIOS_BOOL  bStatus = CBIOS_FALSE;
    CBIOS_U32 ulExtsNum = 0, ulBlkIndex = 0;
    CBIOS_U32 EdidSize = 0;
    PCBIOS_U8 EDIDData = cb_AllocateNonpagedPool(EDID_BLK_SIZE);

    if (!EDIDData)
    {
        goto EXIT;
    }
    EdidSize = EDID_BLK_SIZE;

    for(ulBlkIndex = 0; ulBlkIndex < EdidSize/EDID_BLK_SIZE; ulBlkIndex++)
    {
        CBIOS_U8 *Buffer = EDIDData + ulBlkIndex * EDID_BLK_SIZE;
        CBIOS_U32  ulOffsetInSeg = (ulBlkIndex * EDID_BLK_SIZE) % 256;
        CBIOS_U32  ulSegNum = (ulBlkIndex * EDID_BLK_SIZE) / 256;

        CBIOS_BOOL bRet = cbGetEdidPerSeg(pcbe, pDevCommon, Buffer, ulOffsetInSeg, EDID_BLK_SIZE, (CBIOS_U8)ulSegNum);
        if(ulBlkIndex == 0)
        {
            if(!bRet)
            {
                DCLK = (EDIDData[0x37] << 8) | EDIDData[0x36];
                if(cbEDIDModule_IsEDIDHeaderValid(EDIDData, EDID_BLK_SIZE)
                    && !cbIsEdidChecksumValid(EDIDData, EDID_BLK_SIZE) && DCLK > 0)
                {
                    cbFixEdidChecksum(pcbe, EDIDData, EDID_BLK_SIZE);
                    bStatus = CBIOS_TRUE;
                    bRet = CBIOS_TRUE;
                }
                else
                {
                    bStatus = CBIOS_FALSE;
                    break;
                }
            }
            else
            {
                bStatus = CBIOS_TRUE;
            }
            ulExtsNum = EDIDData[EXTENSION_FLAG_INDEX];
        }
        else if (ulBlkIndex == 1)
        {
            // for EEODB
            ulExtsNum = cbEDIDModule_GetExtBlockNum(EDIDData);
        }

        if(!bRet)
        {
            cb_memset(Buffer, 0, EDID_BLK_SIZE);
        }

        // buffer is not enough, reallocate more memory
        if(EdidSize < (ulExtsNum + 1) * EDID_BLK_SIZE)
        {
            PCBIOS_U8 tmpBuffer = cb_AllocateNonpagedPool((ulExtsNum + 1) * EDID_BLK_SIZE);
            if (!tmpBuffer)
            {
                bStatus = CBIOS_FALSE;
                goto EXIT;
            }
            cb_memcpy(tmpBuffer, EDIDData, EdidSize);
            cb_FreePool(EDIDData);
            EDIDData = tmpBuffer;
            EdidSize = (ulExtsNum + 1) * EDID_BLK_SIZE;
        }
    }

EXIT:
    if(!bStatus)
    {
        if (EDIDData)
        {
            cb_FreePool(EDIDData);
            EDIDData = CBIOS_NULL;
        }
        EdidSize = 0;
    }

    if (ReadEdidSize)
    {
        *ReadEdidSize = EdidSize;
    }

    return EDIDData;
}

static CBIOS_BOOL cbGetDeviceSignature(PCBIOS_EXTENSION_COMMON pcbe, PCBIOS_DEVICE_COMMON pDevCommon, PCBIOS_DEVICE_SIGNATURE pDevSignatre)
{
    CBIOS_U32  i = 0;
    CBIOS_BOOL bResult = CBIOS_FALSE;
    CBIOS_U32  ExtBlockNum = 0;
    CBIOS_EDID_DATA EdidData = {0};

    if(CBIOS_NULL == pDevSignatre)
    {
        bResult = CBIOS_FALSE;
        cbDebugPrint((MAKE_LEVEL(GENERIC, ERROR), "%s: the 3rd param is a NULL pointer\n", FUNCTION_NAME));
    }
    else
    {
        cb_memset(pDevSignatre, 0, sizeof(CBIOS_DEVICE_SIGNATURE));

        // get the monitor ID
        if(cbGetEdidBytes(pcbe, pDevCommon, pDevSignatre->MonitorID, MONITORIDINDEX, MONITORIDLENGTH))
        {
            // get the flag of checksum in block 0
            if(cbGetEdidBytes(pcbe, pDevCommon, pDevSignatre->ExtFlagChecksum[0], EXT_FLAG_CHKSUM_IDX, EXT_FLAG_CHKSUM_LEN))
            {
                bResult = cbGetExtBlockNum(pcbe, pDevCommon, &ExtBlockNum);
                if(bResult)
                {
                    for (i = 1; i < cb_min(CBIOS_EDID_MAX_BLK_CNT, ExtBlockNum + 1); i++)
                    {
                        if(!cbGetEdidBytes(pcbe, pDevCommon, pDevSignatre->ExtFlagChecksum[i], EDID_BLK_SIZE * i + EXT_FLAG_CHKSUM_IDX, EXT_FLAG_CHKSUM_LEN))
                        {
                            cbDebugPrint((MAKE_LEVEL(GENERIC, ERROR), "%s: cannot read the checksum flag in EDID block %x\n", FUNCTION_NAME, i));
                            bResult = CBIOS_FALSE;
                            break;
                        }
                        else
                        {
                            bResult = CBIOS_TRUE;
                        }
                    }
                }
            }
            else
            {
                bResult = CBIOS_FALSE;
            }
        }
        else
        {
            bResult = CBIOS_FALSE;
        }

        if(!bResult)
        {
            cb_memset(pDevSignatre, 0, sizeof(CBIOS_DEVICE_SIGNATURE));
        }
    }
    return bResult;
}

CBIOS_BOOL cbGetDeviceEDID(PCBIOS_VOID pvcbe, PCBIOS_DEVICE_COMMON pDevCommon, CBIOS_BOOL *pIsDevChanged, CBIOS_U32 FullDetect)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    CBIOS_BOOL              bGotEDID = CBIOS_FALSE;
    CBIOS_BOOL              bDevChanged = CBIOS_FALSE;
    CBIOS_DEVICE_SIGNATURE  NewSignature = {0};
    CBIOS_EDID_DATA         EdidData = {0};

    if (pDevCommon == CBIOS_NULL)
    {
        cbDebugPrint((MAKE_LEVEL(GENERIC, ERROR), "%s: pDevCommon is NULL!\n", FUNCTION_NAME));
        return bGotEDID;
    }

    if(pDevCommon->isFakeEdid)
    {
        bGotEDID = CBIOS_TRUE;
        if (pDevCommon->FakeEdid.BufferSize != pDevCommon->EdidData.BufferSize || cb_memcmp(pDevCommon->FakeEdid.Buffer, pDevCommon->EdidData.Buffer, pDevCommon->EdidData.BufferSize))
        {
            if (cbCopyBuffer2EdidData(&pDevCommon->EdidData, pDevCommon->FakeEdid.Buffer, pDevCommon->FakeEdid.BufferSize))
            {
                bDevChanged = CBIOS_TRUE;
            }
            else
            {
                bGotEDID = CBIOS_FALSE;
            }
        }
    }
    // read EDID from the monitor side
    else
    {
        //some hdmi cts items like HF1-23 need read full edid when monitor issue a hotplug
        //only 2 cases that get EDID success: 1, Can read full EDID in full detect; 2, Can read signature in non full detect
        if(FullDetect)
        {
            EdidData.Buffer = cbGetFullEdid(pcbe, pDevCommon, &EdidData.BufferSize);
            if (cbEDIDModule_IsEDIDValid(&EdidData))
            {
                bGotEDID = CBIOS_TRUE;
                cbGetDeviceSignatureFromEdid(pcbe, &EdidData, &NewSignature);
                if(cb_memcmp(&pDevCommon->ConnectedDevSignature, &NewSignature, sizeof(CBIOS_DEVICE_SIGNATURE)))
                {
                    bDevChanged = CBIOS_TRUE;
                }
            }
        }
        else if(cbGetDeviceSignature(pcbe, pDevCommon, &NewSignature))  //read signature from I2C
        {
            bGotEDID = CBIOS_TRUE;
            if(cb_memcmp(&pDevCommon->ConnectedDevSignature, &NewSignature, sizeof(CBIOS_DEVICE_SIGNATURE)))
            {
                EdidData.Buffer = cbGetFullEdid(pcbe, pDevCommon, &EdidData.BufferSize);
                if (cbEDIDModule_IsEDIDValid(&EdidData))
                {
                    bDevChanged = CBIOS_TRUE;
                }
                else
                {
                    bGotEDID = CBIOS_FALSE;
                }
            }
        }

        if (bDevChanged)
        {
            cbClearEdidData(&pDevCommon->EdidData);
            cb_memcpy(&pDevCommon->EdidData, &EdidData, sizeof(CBIOS_EDID_DATA));
            cb_memcpy(&pDevCommon->ConnectedDevSignature, &NewSignature, sizeof(CBIOS_DEVICE_SIGNATURE));
        }
        else
        {
            cbClearEdidData(&EdidData);
        }
    }

    if(pIsDevChanged)
    {
        *pIsDevChanged = bDevChanged;
    }

    return bGotEDID;
}

CBIOS_BOOL cbGetDeviceSignatureFromEdid(PCBIOS_VOID pvcbe, CBIOS_EDID_DATA *pEdidData, CBIOS_DEVICE_SIGNATURE *pSignture)
{
    CBIOS_U32 TotalBlockNum = 0, i = 0;

    if (pEdidData == CBIOS_NULL || pEdidData->Buffer == CBIOS_NULL || pSignture == CBIOS_NULL)
    {
        cbDebugPrint((MAKE_LEVEL(GENERIC, ERROR),"%s: the 2nd param is NULL!\n", FUNCTION_NAME));
        return CBIOS_FALSE;
    }

    cb_memset(pSignture, 0, sizeof(CBIOS_DEVICE_SIGNATURE));
    cb_memcpy(pSignture->MonitorID, &pEdidData->Buffer[MONITORIDINDEX], MONITORIDLENGTH);

    TotalBlockNum = cb_min(cbEDIDModule_GetTotalBlockNum(pEdidData), CBIOS_EDID_MAX_BLK_CNT);
    // copy the flag of checksum in every block
    for (i = 0; i < TotalBlockNum; i++)
    {
        cb_memcpy(pSignture->ExtFlagChecksum[i],
                    &pEdidData->Buffer[i * EDID_BLK_SIZE + EXT_FLAG_CHKSUM_IDX], EXT_FLAG_CHKSUM_LEN);
    }

    return CBIOS_TRUE;
}

CBIOS_MONITOR_TYPE cbGetSupportMonitorType(PCBIOS_VOID pvcbe, CBIOS_ACTIVE_TYPE devices)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    CBIOS_MONITOR_TYPE  MonitorType = CBIOS_MONITOR_TYPE_NONE;

    if(devices & CBIOS_TYPE_CRT)
    {
        MonitorType |= CBIOS_MONITOR_TYPE_CRT;
    }

    if(devices & CBIOS_TYPE_TV)
    {
        MonitorType |= CBIOS_MONITOR_TYPE_TV;
    }

    if(devices & CBIOS_TYPE_HDTV)
    {
        MonitorType |= CBIOS_MONITOR_TYPE_HDTV;
    }

    if(devices & CBIOS_TYPE_DSI)
    {
        MonitorType |= CBIOS_MONITOR_TYPE_PANEL;
    }

    if(devices & CBIOS_TYPE_DP1)
    {
        if ((pcbe->SysBiosInfo.Version >= 0x11 && pcbe->SysBiosInfo.Dp1PortConnType == CBIOS_EDP_CONN)
            || (pcbe->SysBiosInfo.Version < 0x11 && pcbe->FeatureSwitch.IsEDP1Enabled))
        {
            MonitorType |= CBIOS_MONITOR_TYPE_PANEL;
        }
        else
        {
            MonitorType |=  CBIOS_MONITOR_TYPE_DP | CBIOS_MONITOR_TYPE_DVI | CBIOS_MONITOR_TYPE_HDMI;
        }
    }

    if(devices & CBIOS_TYPE_DP2)
    {
        if ((pcbe->SysBiosInfo.Version >= 0x11 && pcbe->SysBiosInfo.Dp2PortConnType == CBIOS_EDP_CONN)
            || (pcbe->SysBiosInfo.Version < 0x11 && pcbe->FeatureSwitch.IsEDP2Enabled))
        {
            MonitorType |= CBIOS_MONITOR_TYPE_PANEL;
        }
        else
        {
            MonitorType |=  CBIOS_MONITOR_TYPE_DP | CBIOS_MONITOR_TYPE_DVI | CBIOS_MONITOR_TYPE_HDMI;
        }
    }

    if(devices & CBIOS_TYPE_DP3)
    {
        if ((pcbe->SysBiosInfo.Version >= 0x11 && pcbe->SysBiosInfo.Dp3PortConnType == CBIOS_EDP_CONN)
            || (pcbe->SysBiosInfo.Version < 0x11 && pcbe->FeatureSwitch.IsEDP3Enabled))
        {
            MonitorType |= CBIOS_MONITOR_TYPE_PANEL;
        }
        else
        {
            MonitorType |=  CBIOS_MONITOR_TYPE_DP | CBIOS_MONITOR_TYPE_DVI | CBIOS_MONITOR_TYPE_HDMI;
        }
    }

    if(devices & CBIOS_TYPE_DP4)
    {
        if (pcbe->FeatureSwitch.IsEDP4Enabled)
        {
            MonitorType |= CBIOS_MONITOR_TYPE_PANEL;
        }
        else
        {
            MonitorType |=  CBIOS_MONITOR_TYPE_DP | CBIOS_MONITOR_TYPE_DVI | CBIOS_MONITOR_TYPE_HDMI;
        }
    }

    if(devices & CBIOS_TYPE_DVO)
    {
        cbDVOPort_GetSupportMonitorType(pcbe, &MonitorType);
    }

    /*
    if(devices & CBIOS_TYPE_MHL)
    {
        if (pcbe->FeatureSwitch.IsCfgMHLMode)
        {
            MonitorType |= CBIOS_MONITOR_TYPE_MHL;
        }
        else
        {
            MonitorType |= CBIOS_MONITOR_TYPE_HDMI | CBIOS_MONITOR_TYPE_DVI;
        }
    }
    */

    if(MonitorType == CBIOS_MONITOR_TYPE_NONE)
    {
        cbDebugPrint((MAKE_LEVEL(GENERIC, WARNING), "%s: invalid device type 0x%x!\n", FUNCTION_NAME, devices));
    }

    return MonitorType;
}

CBIOS_VOID cbClearEdidRelatedData(PCBIOS_VOID pvcbe, PCBIOS_DEVICE_COMMON pDevCommon)
{
    if (pDevCommon == CBIOS_NULL)
    {
        cbDebugPrint((MAKE_LEVEL(GENERIC, ERROR),"%s: the 2nd param is NULL!\n", FUNCTION_NAME));
        return;
    }

    //reset buffers
    cbClearEdidData(&pDevCommon->EdidData);

    cb_memset(&pDevCommon->ConnectedDevSignature, 0, sizeof(CBIOS_DEVICE_SIGNATURE));
    // cb_memset(&pDevCommon->EdidStruct, 0, sizeof(CBIOS_EDID_STRUCTURE_DATA));

    cbClearEdidData(&pDevCommon->FakeEdid);
    pDevCommon->isFakeEdid = CBIOS_FALSE;
}



CBIOS_BOOL cbDualModeDetect(PCBIOS_VOID pvcbe, PCBIOS_DEVICE_COMMON pDevCommon)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    CBIOS_BOOL    bIsDualMode = CBIOS_FALSE;
    CBIOS_U8      byData = 0;
    CBIOS_STATUS  bStatus = CBIOS_ER_INTERNAL;

    pDevCommon->I2CDelay = pcbe->I2CDelay;
    bStatus = cbI2cNormal_ReadEDID(pcbe, pDevCommon, &byData, 0x00, 1, 0);
    //using DP dual mode char byte to see HDMI or DP
    if (bStatus == CBIOS_OK || bStatus == CBIOS_ER_HDCP_USING_I2C)
    {
        bIsDualMode = CBIOS_TRUE;
    }
    else
    {
        //default i2cdelay fail, reduce the frequency
        pDevCommon->I2CDelay = pcbe->I2CDelay + 4;
        bStatus = cbI2cNormal_ReadEDID(pcbe, pDevCommon, &byData, 0x00, 1, 0);
        if (bStatus == CBIOS_OK || bStatus == CBIOS_ER_HDCP_USING_I2C)
        {
            bIsDualMode = CBIOS_TRUE;
        }
        else
        {
            pDevCommon->I2CDelay = pcbe->I2CDelay;
            bIsDualMode = CBIOS_FALSE;
        }
    }
    return bIsDualMode;
}

PCBIOS_VOID cbGetDPMonitorContext(PCBIOS_VOID pvcbe, PCBIOS_DEVICE_COMMON pDevCommon)
{
    PCBIOS_VOID pDPMonitorContext = CBIOS_NULL;

    if (pDevCommon->DeviceType & ALL_DP_TYPES)
    {
        pDPMonitorContext = cbDPPort_GetDPMonitorContext(pvcbe, pDevCommon);
    }

    return pDPMonitorContext;
}

PCBIOS_VOID cbGetHDMIMonitorContext(PCBIOS_VOID pvcbe, PCBIOS_DEVICE_COMMON pDevCommon)
{
    PCBIOS_VOID pHDMIMonitorContext = CBIOS_NULL;

    if (pDevCommon->DeviceType & ALL_DP_TYPES)
    {
        pHDMIMonitorContext = cbDPPort_GetHDMIMonitorContext(pvcbe, pDevCommon);
    }

    return pHDMIMonitorContext;
}

#define GET_BIT(val, bit) ((val & (1 << bit)) >> bit)
/* ELD: EDID-Like Data, for passing sink device's audio EDID info to audio software */
CBIOS_STATUS cbGetDeviceELD(PCBIOS_VOID pvcbe, CBIOS_ACTIVE_TYPE DeviceType, PCBIOS_ELD_MEM_STRUCT pELD)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    PCBIOS_DEVICE_COMMON pDevCommon = cbGetDeviceCommon(&pcbe->DeviceMgr, DeviceType);
    PCBIOS_MONITOR_MISC_ATTRIB pMonitorAttrib = &(pDevCommon->EdidStruct.Attribute);
    CBIOS_U8    *pEldPayLoad    = CBIOS_NULL;
    CBIOS_U32   Length  =   0;
    CBIOS_U32   i  =   0;

    cb_memset(pELD, 0, sizeof(CBIOS_ELD_MEM_STRUCT));

    pELD->ELD_Data.HDCP = 1;
    pELD->ELD_Data.S_AI = 0;
    pELD->ELD_Data.ELD_Ver = ELD_VER_CEA_861D;

    cb_memcpy(pELD->ELD_Data.ManufactureName, pMonitorAttrib->ManufactureName, 0x02); /* Manufacture name */
    cb_memcpy(pELD->ELD_Data.ProductCode, pMonitorAttrib->ProductCode, 0x02); /* ProductCode */

    pEldPayLoad = pELD->ELD_Data.private_data;
    while (i < 16 && (pMonitorAttrib->MonitorName[i] != 0x00))
    {
        *(pEldPayLoad++) = pMonitorAttrib->MonitorName[i];  /* Monitor Name */
        Length++;
        i++;
    }
    pELD->ELD_Data.MNL = (CBIOS_U8)Length;  /* Monitor Name Length */

    if (pDevCommon->CurrentMonitorType & (CBIOS_MONITOR_TYPE_DP | CBIOS_MONITOR_TYPE_PANEL))
    {
        pELD->ELD_Data.Conn_Type = 0x01; // connect type is DP
    }
    else
    {
        pELD->ELD_Data.Conn_Type = 0; // HDMI
    }

    if (pMonitorAttrib->IsCEA861Monitor)
    {
        pELD->ELD_Data.CEA_EDID_Ver = pMonitorAttrib->RevisionNumber;

        if (!pMonitorAttrib->IsCEA861Audio)
        {
            cbDebugPrint((MAKE_LEVEL(HDMI, INFO), "%s: Not support basic audio!\n", FUNCTION_NAME));
        }

        pELD->ELD_Data.SAD_Count = pMonitorAttrib->SAD_Count;
        if  (pELD->ELD_Data.SAD_Count != 0x00)
        {
            for (i = 0; i < pELD->ELD_Data.SAD_Count; i++)
            {
                *(pEldPayLoad++) = pMonitorAttrib->CEA_SADs[i][0];
                *(pEldPayLoad++) = pMonitorAttrib->CEA_SADs[i][1];
                *(pEldPayLoad++) = pMonitorAttrib->CEA_SADs[i][2];
                Length += 3;
            }
        }
        else
        {
            /*
             * From High-Definitiddon Multimedia Interface Specification Version 1.4b  7.3(Page 125):
             *
             * For CEA-861-D references to Sources:
             *  "Basic Audio" is defined as two channel L-PCM audio at sample rates of 32kHZ,44.1Khz, or 48kHZ,
             *   With a sample size of at least 16 bits
             *
             *   CEA short Audio Descriptor for audio code = 1(LPCM)
             * --------------------------------------------------------------------------------
             *  Byte offset  |                         BIts                                   |
             *  -------------------------------------------------------------------------------
             *               |   7   |   6   |   5   |   4   |   3   |   2   |   1   |   0    |
             *  -------------------------------------------------------------------------------
             *       1       | F17=0 |   Audio Format Code = 0001    | Max_num_channels - 1   |
             *  -------------------------------------------------------------------------------
             *       2       | F27=0 |192kHz |176.4kHz|96kHz |88.2kHz| 48kHz | 44.1kHz| 32kHz |
             *  -------------------------------------------------------------------------------
             *       3       | F37=0 | F36=0 | F35=0  | F34=0| F33=0 | 24bit | 20bit  | 16bit |
             *  -----------------------------------------------------------------------------
             */
            pELD->ELD_Data.SAD_Count = 1;
            *(pEldPayLoad++) = 0x9;     //Linear PCM
            *(pEldPayLoad++) = 0x7;     //48kHZ, 44.1Khz, 32kHZ
            *(pEldPayLoad++) = 0x3;     //20 bit, 16 bit
            Length += 3;
        }

        pELD->ELD_Data.BaseLine_Eld_len = (16 + Length + 3) / 4;
        pELD->Size = pELD->ELD_Data.BaseLine_Eld_len * 4 + 4;

        pELD->ELD_Data.FLR  =   GET_BIT(pMonitorAttrib->SpeakerAllocationData, 0);
        pELD->ELD_Data.LFE  =   GET_BIT(pMonitorAttrib->SpeakerAllocationData, 1);
        pELD->ELD_Data.FC   =   GET_BIT(pMonitorAttrib->SpeakerAllocationData, 2);
        pELD->ELD_Data.RLR  =   GET_BIT(pMonitorAttrib->SpeakerAllocationData, 3);
        pELD->ELD_Data.RC   =   GET_BIT(pMonitorAttrib->SpeakerAllocationData, 4);
        pELD->ELD_Data.FLRC =   GET_BIT(pMonitorAttrib->SpeakerAllocationData, 5);
        pELD->ELD_Data.RLRC =   GET_BIT(pMonitorAttrib->SpeakerAllocationData, 6);
    }

    return CBIOS_OK;
}

CBIOS_VOID *cbGetDeviceBusLock(PCBIOS_VOID pvcbe, CBIOS_U32 Index, CBIOS_U32  IndexType)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    PCBIOS_DEVICE_COMMON pDevCommon = CBIOS_NULL;
    CBIOS_U32  DeviceMasks = pcbe->DeviceMgr.SupportDevices;
    CBIOS_U32  DeviceBit = 0;
    CBIOS_BOOL bFindDevice = CBIOS_FALSE;
    CBIOS_VOID *pLock = CBIOS_NULL;

    if(IndexType > CBIOS_MODULE_TYPE_NONE 
        && IndexType < CBIOS_MODULE_TYPE_NUM
        && CBIOS_MODULE_INDEX_INVALID == Index)
    {
        cbDebugPrint((MAKE_LEVEL(GENERIC, ERROR), "Invalid parameter at get bus lock: index=%d, type=%d.\n", Index, IndexType));
        return CBIOS_NULL;
    }

    while(DeviceMasks)
    {
        DeviceBit = GET_LAST_BIT(DeviceMasks);
        DeviceMasks &= ~DeviceBit;

        pDevCommon = cbGetDeviceCommon(&pcbe->DeviceMgr, DeviceBit);
        if(!pDevCommon)
        {
            continue;
        }

        if(IndexType == CBIOS_MODULE_TYPE_I2C)
        {
            if(pDevCommon->I2CBus == Index)
            {
                bFindDevice = CBIOS_TRUE;
                break;
            }
        }
        else if(IndexType > CBIOS_MODULE_TYPE_NONE && IndexType < CBIOS_MODULE_TYPE_NUM)
        {
            if(cbGetModuleIndex(pcbe, DeviceBit, IndexType) == Index)
            {
                bFindDevice = CBIOS_TRUE;
                break;
            }
        }
    }

    pLock = (bFindDevice)? pDevCommon->pBusLock : CBIOS_NULL;

    if(!pLock)
    {
        cbDebugPrint((MAKE_LEVEL(GENERIC, WARNING), "Get bus lock failed: index=%d, type=%d.\n", Index, IndexType));
    }

    return pLock;
}
