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
** I2C module interface function implementation.
** Mainly includes read/write data of I2C bus, GPIO simulate I2C, and DDCCIP
**
** NOTE:
** The functions in this file are hw layer internal functions,
** CAN ONLY be called by files under Hw folder.
******************************************************************************/

#include "CBiosChipShare.h"
#include "CBiosUtilHw.h"
#include "../HwBlock/CBiosDIU_DP.h"

#define I2C_SHORT_DELAY 0x1

/***************************************************************
Function:    cbI2CModule_SetScl

Description: Set I2C clock line to specific lineLvl

Input:       PCBIOS_EXTENSION_COMMON
             PCBIOS_I2C_REG_PARAMS
             lineLvl, can be LEVEL_HIGH or LEVEL_LOW

Output:

Return:
***************************************************************/
static CBIOS_VOID cbI2CModule_SetScl(PCBIOS_EXTENSION_COMMON pcbe,
                                     PCBIOS_I2C_REG_PARAMS pI2CRegParams,
                                     CBIOS_U8 lineLvl)
{
    CBIOS_U8 ulValue = 0, ulMask = 0;

    ulValue = lineLvl << pI2CRegParams->CLineRegWriteBitNum;
    ulValue += pI2CRegParams->CLineRegWriteEnableValue;
    ulMask = 1 << pI2CRegParams->CLineRegWriteBitNum;
    ulMask = pI2CRegParams->CLineRegWriteEnableMask & (~ulMask);

    cbMMIOWriteReg(pcbe, pI2CRegParams->CLineRegType_Index, ulValue, ulMask);
}

/***************************************************************
Function:    cbI2CModule_GetScl

Description: Get I2C clock line level

Input:       PCBIOS_EXTENSION_COMMON
             PCBIOS_I2C_REG_PARAMS

Output:

Return:      the line level of I2C clock line
***************************************************************/
static CBIOS_U8 cbI2CModule_GetScl(PCBIOS_EXTENSION_COMMON pcbe, PCBIOS_I2C_REG_PARAMS pI2CRegParams)
{
    CBIOS_U8 ulValue = 0, ulMask = 0, ulRet = 0;

    //first enable read function
    ulValue = pI2CRegParams->CLineRegReadEnableValue;
    ulMask = pI2CRegParams->CLineRegReadEnableMask;
    cbMMIOWriteReg(pcbe, pI2CRegParams->CLineRegType_Index, ulValue, ulMask);

    ulRet = cbMMIOReadReg(pcbe, pI2CRegParams->CLineRegType_Index);
    ulRet = ulRet >> pI2CRegParams->CLineRegReadBitNum;
    ulRet = ulRet & 0x01;
    return ulRet;
}

/***************************************************************
Function:    cbI2CModule_SetSda

Description: Set I2C data line to specific lineLvl

Input:       PCBIOS_EXTENSION_COMMON
             PCBIOS_MODULE_I2C_PARAMS
             lineLvl, can be LEVEL_HIGH or LEVEL_LOW

Output:

Return:
***************************************************************/
static CBIOS_VOID cbI2CModule_SetSda(PCBIOS_EXTENSION_COMMON pcbe,
                                     PCBIOS_I2C_REG_PARAMS pI2CRegParams,
                                     CBIOS_U8 lineLvl)
{
    CBIOS_U8 ulValue = 0, ulMask = 0;

    ulValue = lineLvl << pI2CRegParams->DLineRegWriteBitNum;
    ulValue += pI2CRegParams->DLineRegWriteEnableValue;
    ulMask = 1 << pI2CRegParams->DLineRegWriteBitNum;
    ulMask = pI2CRegParams->DLineRegWriteEnableMask & (~ulMask);

    cbMMIOWriteReg(pcbe, pI2CRegParams->DLineRegType_Index, ulValue, ulMask);
}

/***************************************************************
Function:    cbI2CModule_GetSda

Description: Get I2C data line level

Input:       PCBIOS_MODULE_I2C_PARAMS
             PCBIOS_I2C_REG_PARAMS

Output:

Return:      the line level of I2C data line
***************************************************************/
static CBIOS_U8 cbI2CModule_GetSda(PCBIOS_EXTENSION_COMMON pcbe, PCBIOS_I2C_REG_PARAMS pI2CRegParams)
{
    CBIOS_U8 ulValue = 0, ulMask = 0, ulRet = 0;

    //first enable read function
    ulValue = pI2CRegParams->DLineRegReadEnableValue;
    ulMask = pI2CRegParams->DLineRegReadEnableMask;
    cbMMIOWriteReg(pcbe, pI2CRegParams->DLineRegType_Index, ulValue, ulMask);

    ulRet = cbMMIOReadReg(pcbe, pI2CRegParams->DLineRegType_Index);
    ulRet = ulRet >> pI2CRegParams->DLineRegReadBitNum;
    ulRet = ulRet & 0x01;
    return ulRet;
}

/***************************************************************
Function:    cbI2CModule_CheckBusNum

Description: Check the I2C bus num is valid.

Input:       PCBIOS_EXTENSION_COMMON
             PCBIOS_MODULE_I2C_PARAMS

Output:

Return:      CBIOS_TURE if I2C bus num is valid
             CBIOS_FALSE if I2C bus num is invalid
***************************************************************/
static CBIOS_BOOL cbI2CModule_CheckBusNum(PCBIOS_EXTENSION_COMMON pcbe, PCBIOS_MODULE_I2C_PARAMS pCBModuleI2CParams)
{
    CBIOS_U8   I2CBusNum = pCBModuleI2CParams->I2CBusNum;
    CBIOS_BOOL bRet = CBIOS_FALSE;

    //normal I2C
    if ((I2CBusNum >= MIN_NORMAL_I2C_BUS) && (I2CBusNum <= MAX_NORMAL_I2C_BUS))
    {
        bRet = CBIOS_TRUE;
    }
    //GPIO simulate I2C
    else if ((I2CBusNum >= MIN_GPIO_I2C_BUS) && (I2CBusNum <= MAX_GPIO_I2C_BUS))
    {
        bRet = CBIOS_TRUE;
    }
    else
    {
        cbDebugPrint((MAKE_LEVEL(GENERIC, DEBUG), "cbI2CModule_CheckBusNum: I2CBusNum invalid! \n"));
        bRet = CBIOS_FALSE;
    }
    return bRet;
}

/***************************************************************
Function:    cbI2CModule_Start

Description: Send the start signal to the I2C bus

Input:       PCBIOS_MODULE_I2C_PARAMS

Output:

Return:
***************************************************************/
static CBIOS_BOOL cbI2CModule_Start(PCBIOS_EXTENSION_COMMON pcbe, PCBIOS_MODULE_I2C_PARAMS pCBModuleI2CParams)
{
    CBIOS_U8  bySCL = 0, bySDL = 0;
    CBIOS_U32 i = 0;
    CBIOS_U32 MaxLoop = MAXI2CWAITLOOP;
    CBIOS_U16  I2CDELAY = pCBModuleI2CParams->I2CDelay;
    CBIOS_BOOL bAck = CBIOS_FALSE;
    PCBIOS_I2C_REG_PARAMS pI2CRegParams = &(pCBModuleI2CParams->I2CRegParams);

    //check I2CBusNum
    if(cbI2CModule_CheckBusNum(pcbe, pCBModuleI2CParams) == CBIOS_FALSE)
    {
        cbDebugPrint((MAKE_LEVEL(GENERIC, DEBUG), "cbI2CModule_Start: I2CBusNum invalid! \n"));
        return CBIOS_FALSE;
    }

    //check the SCL status
    bySCL = cbI2CModule_GetScl(pcbe, pI2CRegParams);

    //SCL Low
    if (bySCL == LEVEL_LOW)
    {
        //wait for SCL High
        for (i = 0; i < MaxLoop; i++)
        {
            cbI2CModule_SetScl(pcbe, pI2CRegParams, LEVEL_HIGH);
            cb_DelayMicroSeconds(I2CDELAY);

            bySCL = cbI2CModule_GetScl(pcbe, pI2CRegParams);
            if (bySCL == LEVEL_HIGH)
            {
                break;
            }
        }
        if (i >= MaxLoop)
        {
            return CBIOS_FALSE;
        }
    }

    //check the SDL status
    bySDL = cbI2CModule_GetSda(pcbe, pI2CRegParams);

    //SCL High + SDA Low
    if ((bySCL == LEVEL_HIGH) && (bySDL == LEVEL_LOW))
    {
        //set SCL Low
        cbI2CModule_SetScl(pcbe, pI2CRegParams, LEVEL_LOW);
        cb_DelayMicroSeconds(I2C_SHORT_DELAY);

        //set SDA High
        cbI2CModule_SetSda(pcbe, pI2CRegParams, LEVEL_HIGH);
        cb_DelayMicroSeconds(I2CDELAY);

        //wait for SCLHigh
        for (i = 0; i < MaxLoop; i++)
        {
            cbI2CModule_SetScl(pcbe, pI2CRegParams, LEVEL_HIGH);
            cb_DelayMicroSeconds(I2CDELAY);

            bySCL = cbI2CModule_GetScl(pcbe, pI2CRegParams);
            if (bySCL == LEVEL_HIGH)
            {
                break;
            }
        }
        if (i >= MaxLoop)
        {
            return CBIOS_FALSE;
        }
    }

    //check SCL High + SDA High
    bySCL = cbI2CModule_GetScl(pcbe, pI2CRegParams);
    bySDL = cbI2CModule_GetSda(pcbe, pI2CRegParams);
    if ((bySCL == LEVEL_HIGH) && (bySDL == LEVEL_HIGH))
    {
        //I2C start: SCL High, SDA High->Low
        cbI2CModule_SetSda(pcbe, pI2CRegParams, LEVEL_LOW);
        cb_DelayMicroSeconds(I2CDELAY);

        cbI2CModule_SetScl(pcbe, pI2CRegParams, LEVEL_LOW);
        cb_DelayMicroSeconds(I2CDELAY);
        bAck = CBIOS_TRUE;
    }
    else
    {
        bAck = CBIOS_FALSE;
        //cbDebugPrint((MAKE_LEVEL(GENERIC, ERROR), "I2C start failed for bus %d!\n", pCBModuleI2CParams->I2CBusNum));
    }
    return bAck;
}

/***************************************************************
Function:    cbI2CModule_Stop

Description: Send the stop signal to the I2C bus

Input:       PCBIOS_EXTENSION_COMMON
             PCBIOS_MODULE_I2C_PARAMS

Output:

Return:      CBIOS_TRUE for success, CBIOS_FALSE for fail
***************************************************************/
static CBIOS_BOOL cbI2CModule_Stop(PCBIOS_EXTENSION_COMMON pcbe, PCBIOS_MODULE_I2C_PARAMS pCBModuleI2CParams)
{
    CBIOS_U8  byTemp = 0;
    CBIOS_U32 i = 0;
    CBIOS_U32 MaxLoop = MAXI2CWAITLOOP;
    CBIOS_U16  I2CDELAY = pCBModuleI2CParams->I2CDelay;
    PCBIOS_I2C_REG_PARAMS pI2CRegParams = &(pCBModuleI2CParams->I2CRegParams);

    //check I2CBusNum
    if(cbI2CModule_CheckBusNum(pcbe, pCBModuleI2CParams) == CBIOS_FALSE)
    {
        cbDebugPrint((MAKE_LEVEL(GENERIC, DEBUG), "cbI2CModule_Stop: I2CBusNum invalid! \n"));
        return CBIOS_FALSE;
    }

    cbI2CModule_SetScl(pcbe, pI2CRegParams, LEVEL_LOW);
    cb_DelayMicroSeconds(I2C_SHORT_DELAY);
    cbI2CModule_SetSda(pcbe, pI2CRegParams, LEVEL_LOW);
    cb_DelayMicroSeconds(I2CDELAY);

    for (i = 0; i < MaxLoop; i++)
    {
        cbI2CModule_SetScl(pcbe, pI2CRegParams, LEVEL_HIGH);
        cb_DelayMicroSeconds(I2CDELAY);
        byTemp = cbI2CModule_GetScl(pcbe, pI2CRegParams);
        if (byTemp == LEVEL_HIGH)
        {
            break;
        }
    }

    if (byTemp == LEVEL_HIGH)
    {
        cbI2CModule_SetSda(pcbe, pI2CRegParams, LEVEL_HIGH);
        cb_DelayMicroSeconds(I2CDELAY);
    }

    return CBIOS_TRUE;
}

/***************************************************************
Function:    cbI2CModule_Enable

Description: enable I2C input and output

Input:       PCBIOS_MODULE_I2C_PARAMS

Output:

Return:
***************************************************************/
static CBIOS_VOID cbI2CModule_Enable(PCBIOS_EXTENSION_COMMON pcbe, PCBIOS_MODULE_I2C_PARAMS pCBModuleI2CParams)
{
    CBIOS_U16  I2CDELAY = pCBModuleI2CParams->I2CDelay;
    PCBIOS_I2C_REG_PARAMS pI2CRegParams = &(pCBModuleI2CParams->I2CRegParams);

    //check I2CBusNum
    if(cbI2CModule_CheckBusNum(pcbe, pCBModuleI2CParams) == CBIOS_FALSE)
    {
        cbDebugPrint((MAKE_LEVEL(GENERIC, DEBUG), "cbI2CModule_Enable: I2CBusNum invalid! \n"));
        return;
    }

    //release Sda
    cbI2CModule_SetSda(pcbe, pI2CRegParams, LEVEL_HIGH);
    cb_DelayMicroSeconds(I2C_SHORT_DELAY);
    //release Scl
    cbI2CModule_SetScl(pcbe, pI2CRegParams, LEVEL_HIGH);
    cb_DelayMicroSeconds(I2CDELAY);
}

/***************************************************************
Function:    cbI2CModule_Disable

Description: disable I2C input and output

Input:       PCBIOS_MODULE_I2C_PARAMS

Output:

Return:
***************************************************************/
static CBIOS_VOID cbI2CModule_Disable(PCBIOS_EXTENSION_COMMON pcbe, PCBIOS_MODULE_I2C_PARAMS pCBModuleI2CParams)
{

    CBIOS_U8 ulValue = 0, ulMask = 0;
    PCBIOS_I2C_REG_PARAMS pI2CRegParams = &(pCBModuleI2CParams->I2CRegParams);

    //check I2CBusNum
    if(cbI2CModule_CheckBusNum(pcbe, pCBModuleI2CParams) == CBIOS_FALSE)
    {
        cbDebugPrint((MAKE_LEVEL(GENERIC, DEBUG), "cbI2CModule_Disable: I2CBusNum invalid! \n"));
        return;
    }

    //disable Scl
    ulValue = pI2CRegParams->CLineRegReadEnableValue;
    ulMask = pI2CRegParams->CLineRegReadEnableMask;
    ulValue = (~ulValue) & (~ulMask);   //cline read disable value
    cbMMIOWriteReg(pcbe, pI2CRegParams->CLineRegType_Index, ulValue, ulMask);
    ulValue = pI2CRegParams->CLineRegWriteEnableValue;
    ulMask = pI2CRegParams->CLineRegWriteEnableMask;
    ulValue = (~ulValue) & (~ulMask);   //cline write disable value
    cbMMIOWriteReg(pcbe, pI2CRegParams->CLineRegType_Index, ulValue, ulMask);

    //disable Sda
    ulValue = pI2CRegParams->DLineRegReadEnableValue;
    ulMask = pI2CRegParams->DLineRegReadEnableMask;
    ulValue = (~ulValue) & (~ulMask);   //dline read disable value
    cbMMIOWriteReg(pcbe, pI2CRegParams->DLineRegType_Index, ulValue, ulMask);
    ulValue = pI2CRegParams->DLineRegWriteEnableValue;
    ulMask = pI2CRegParams->DLineRegWriteEnableMask;
    ulValue = (~ulValue) & (~ulMask);   //dline write disable value
    cbMMIOWriteReg(pcbe, pI2CRegParams->DLineRegType_Index, ulValue, ulMask);
}

/***************************************************************
Function:    cbI2CModule_AckRead

Description: Read the Acknowledge signal from the I2C bus.

Input:       PCBIOS_EXTENSION_COMMON
             PCBIOS_MODULE_I2C_PARAMS

Output:

Return:      CBIOS_TRUE if read ACK successfully,
             CBIOS_FALSE if read ACK failed
***************************************************************/
static CBIOS_BOOL cbI2CModule_AckRead(PCBIOS_EXTENSION_COMMON pcbe, PCBIOS_MODULE_I2C_PARAMS pCBModuleI2CParams)
{
    CBIOS_U8  byTemp = 0;
    CBIOS_U32 j = 0;
    CBIOS_U32 MaxLoop = MAXI2CWAITLOOP;
    CBIOS_BOOL bAck = CBIOS_FALSE;
    CBIOS_U16  I2CDELAY = pCBModuleI2CParams->I2CDelay;
    PCBIOS_I2C_REG_PARAMS pI2CRegParams = &(pCBModuleI2CParams->I2CRegParams);

    //check I2CBusNum
    if(cbI2CModule_CheckBusNum(pcbe, pCBModuleI2CParams) == CBIOS_FALSE)
    {
        cbDebugPrint((MAKE_LEVEL(GENERIC, DEBUG), "%s: I2CBusNum invalid! \n", FUNCTION_NAME));
        return CBIOS_FALSE;
    }
/*
    cbI2CModule_SetScl(pcbe, pI2CRegParams,  LEVEL_LOW);
    cb_DelayMicroSeconds(I2C_SHORT_DELAY);
    cbI2CModule_SetSda(pcbe, pI2CRegParams,  LEVEL_HIGH);
    cb_DelayMicroSeconds(I2CDELAY);*/

    for (j = 0; j < MaxLoop; j++)
    {
        cbI2CModule_SetScl(pcbe, pI2CRegParams,  LEVEL_HIGH);
        cb_DelayMicroSeconds(I2CDELAY);

        byTemp = cbI2CModule_GetScl(pcbe, pI2CRegParams);

        if (byTemp == LEVEL_HIGH)
        {
            break;
        }
    }

    if (j >= MaxLoop)
    {
        cbDebugPrint((MAKE_LEVEL(GENERIC, WARNING), "%s: can't pull up SCL for bus %d!\n", FUNCTION_NAME, pCBModuleI2CParams->I2CBusNum));
        bAck = CBIOS_FALSE;
    }
    else
    {
        byTemp = cbI2CModule_GetSda(pcbe, pI2CRegParams);
        if (byTemp == LEVEL_LOW)
        {
            bAck = CBIOS_TRUE;
        }
        else
        {
            bAck = CBIOS_FALSE;
        }
    }

    cbI2CModule_SetScl(pcbe, pI2CRegParams,  LEVEL_LOW);
    cb_DelayMicroSeconds(I2C_SHORT_DELAY);
    cbI2CModule_SetSda(pcbe, pI2CRegParams,  LEVEL_HIGH);
    cb_DelayMicroSeconds(I2CDELAY);

    return bAck;
}

/***************************************************************
Function:    cbI2CModule_AckWrite

Description: Send ACKnowledgement when reading information.
             A ACK is DATA low during one clock pulse.

Input:       PCBIOS_EXTENSION_COMMON
             PCBIOS_MODULE_I2C_PARAMS

Output:

Return:      CBIOS_TURE if write ACK successfully
             CBIOS_FALSE if write failed
***************************************************************/
static CBIOS_BOOL cbI2CModule_AckWrite(PCBIOS_EXTENSION_COMMON pcbe, PCBIOS_MODULE_I2C_PARAMS pCBModuleI2CParams)
{
    CBIOS_U8  byTemp = 0;
    CBIOS_U32 i = 0;
    CBIOS_U32 MaxLoop = MAXI2CWAITLOOP;
    CBIOS_BOOL bStatus = CBIOS_FALSE;
    CBIOS_U16  I2CDELAY = pCBModuleI2CParams->I2CDelay;
    PCBIOS_I2C_REG_PARAMS pI2CRegParams = &(pCBModuleI2CParams->I2CRegParams);

    //check I2CBusNum
    if(cbI2CModule_CheckBusNum(pcbe, pCBModuleI2CParams) == CBIOS_FALSE)
    {
        cbDebugPrint((MAKE_LEVEL(GENERIC, DEBUG), "%s: I2CBusNum invalid! \n", FUNCTION_NAME));
        return CBIOS_FALSE;
    }

    cbI2CModule_SetScl(pcbe, pI2CRegParams,  LEVEL_LOW);
    //cb_DelayMicroSeconds(I2CDELAY);
    cbI2CModule_SetSda(pcbe, pI2CRegParams,  LEVEL_LOW);
    cb_DelayMicroSeconds(I2C_SHORT_DELAY);

    for ( i = 0; i < MaxLoop; i++)
    {
        cbI2CModule_SetScl(pcbe, pI2CRegParams,  LEVEL_HIGH);
        cb_DelayMicroSeconds(I2CDELAY);

        byTemp = cbI2CModule_GetScl(pcbe, pI2CRegParams);

        if (byTemp == LEVEL_HIGH)
        {
            break;
        }
    }

    if (i < MaxLoop)
    {
        bStatus = CBIOS_TRUE;
    }
    else
    {
        bStatus = CBIOS_FALSE;
        cbDebugPrint((MAKE_LEVEL(GENERIC, ERROR), "%s: write ack failed for bus %d!\n", FUNCTION_NAME, pCBModuleI2CParams->I2CBusNum));
    }

    cbI2CModule_SetScl(pcbe, pI2CRegParams,  LEVEL_LOW);
    cb_DelayMicroSeconds(I2C_SHORT_DELAY);
    cbI2CModule_SetSda(pcbe, pI2CRegParams,  LEVEL_HIGH);
    cb_DelayMicroSeconds(I2CDELAY);

    return bStatus;
}

/***************************************************************
Function:    cbI2CModule_NackWrite

Description: Send Not ACKnowledgement when reading information.
             A NACK is DATA high during one clock pulse.

Input:       PCBIOS_EXTENSION_COMMON
             PCBIOS_MODULE_I2C_PARAMS

Output:

Return:      CBIOS_TURE if write NACK successfully
             CBIOS_FALSE if write failed
***************************************************************/
static CBIOS_VOID cbI2CModule_NackWrite(PCBIOS_EXTENSION_COMMON pcbe, PCBIOS_MODULE_I2C_PARAMS pCBModuleI2CParams)
{
    CBIOS_U8  byTemp   = 0;
    CBIOS_U32 j = 0;
    CBIOS_U32 MaxLoop = MAXI2CWAITLOOP;
    CBIOS_U16  I2CDELAY = pCBModuleI2CParams->I2CDelay;
    PCBIOS_I2C_REG_PARAMS pI2CRegParams = &(pCBModuleI2CParams->I2CRegParams);

    //check I2CBusNum
    if(cbI2CModule_CheckBusNum(pcbe, pCBModuleI2CParams) == CBIOS_FALSE)
    {
        cbDebugPrint((MAKE_LEVEL(GENERIC, DEBUG), "%s: I2CBusNum invalid! \n", FUNCTION_NAME));
        return;
    }

    cbI2CModule_SetScl(pcbe, pI2CRegParams, LEVEL_LOW);
    //cb_DelayMicroSeconds(I2CDELAY);
    cbI2CModule_SetSda(pcbe, pI2CRegParams, LEVEL_HIGH);
    cb_DelayMicroSeconds(I2C_SHORT_DELAY);

    for (j = 0; j < MaxLoop; j++)
    {
        cbI2CModule_SetScl(pcbe, pI2CRegParams, LEVEL_HIGH);
        cb_DelayMicroSeconds(I2CDELAY);

        byTemp = cbI2CModule_GetScl(pcbe, pI2CRegParams);

        if (byTemp == LEVEL_HIGH)
            break;
    }

    if (j >= MaxLoop)
    {
        cbDebugPrint((MAKE_LEVEL(GENERIC, WARNING), "%s: can't pull up SCL for bus %d!\n", FUNCTION_NAME, pCBModuleI2CParams->I2CBusNum));
    }

    //check SDA
    byTemp = cbI2CModule_GetSda(pcbe, pI2CRegParams);
    if (byTemp == LEVEL_LOW)
    {
        cbDebugPrint((MAKE_LEVEL(GENERIC, WARNING), "%s: can't pull up SDA for bus %d!\n", FUNCTION_NAME, pCBModuleI2CParams->I2CBusNum));
    }
}

/***************************************************************
Function:    cbI2CModule_ReadByte

Description: Read one byte data from I2C bus.

Input:       PCBIOS_EXTENSION_COMMON
             PCBIOS_MODULE_I2C_PARAMS

Output:      pData, the one byte data read

Return:      CBIOS_TURE if read data successfully
             CBIOS_FALSE if read data failed
***************************************************************/
static CBIOS_BOOL cbI2CModule_ReadByte(PCBIOS_EXTENSION_COMMON pcbe,
                                       PCBIOS_MODULE_I2C_PARAMS pCBModuleI2CParams,
                                       CBIOS_U8* pData, CBIOS_BOOL bWriteAck, CBIOS_BOOL bWriteNAck)
{
    CBIOS_U8  byTemp;
    CBIOS_U8  data = 0;
    CBIOS_U32 i = 0, j = 0, MaxLoop = MAXI2CWAITLOOP;
    CBIOS_BOOL bStatus = CBIOS_TRUE;
    CBIOS_U16  I2CDELAY = pCBModuleI2CParams->I2CDelay;
    PCBIOS_I2C_REG_PARAMS pI2CRegParams = &(pCBModuleI2CParams->I2CRegParams);

    //check I2CBusNum
    if(cbI2CModule_CheckBusNum(pcbe, pCBModuleI2CParams) == CBIOS_FALSE)
    {
        cbDebugPrint((MAKE_LEVEL(GENERIC, DEBUG), "%s: I2CBusNum invalid! \n", FUNCTION_NAME));
        return CBIOS_FALSE;
    }

    if(bWriteAck && bWriteNAck)
    {
        cbDebugPrint((MAKE_LEVEL(GENERIC, WARNING), "%s: Invalid op: both WriteAck and WriteNAck are TRUE! \n", FUNCTION_NAME));
        bWriteNAck = CBIOS_FALSE;
    }

    for (i=0; i<8; i++)
    {
        for (j = 0; j < MaxLoop; j++)
        {
            cbI2CModule_SetScl(pcbe, pI2CRegParams, LEVEL_HIGH);
            cb_DelayMicroSeconds(I2CDELAY);
            if(LEVEL_HIGH == cbI2CModule_GetScl(pcbe, pI2CRegParams))
            {
                break;
            }
        }

        if(j >= MaxLoop)
        {
            cbDebugPrint((MAKE_LEVEL(GENERIC, WARNING), "%s: can't pull up SCL for bus %d!\n", FUNCTION_NAME, pCBModuleI2CParams->I2CBusNum));
            bStatus = CBIOS_FALSE;
            break;
        }

        data = (data << 1) + cbI2CModule_GetSda(pcbe, pI2CRegParams);

        cbI2CModule_SetScl(pcbe, pI2CRegParams, LEVEL_LOW);
        cb_DelayMicroSeconds(I2CDELAY);
    }

    if(bStatus && (bWriteAck || bWriteNAck))
    {
        cbI2CModule_SetSda(pcbe, pI2CRegParams,  (bWriteAck)? LEVEL_LOW : LEVEL_HIGH);
        cb_DelayMicroSeconds(I2C_SHORT_DELAY);

        for ( i = 0; i < MaxLoop; i++)
        {
            cbI2CModule_SetScl(pcbe, pI2CRegParams,  LEVEL_HIGH);
            cb_DelayMicroSeconds(I2CDELAY);
            if(LEVEL_HIGH == cbI2CModule_GetScl(pcbe, pI2CRegParams))
            {
                break;
            }
        }

        if (i < MaxLoop)
        {
            bStatus = CBIOS_TRUE;
        }
        else
        {
            bStatus = CBIOS_FALSE;
            cbDebugPrint((MAKE_LEVEL(GENERIC, ERROR), "%s: write ack/nack failed for bus %d!\n", FUNCTION_NAME, pCBModuleI2CParams->I2CBusNum));
        }

        cbI2CModule_SetScl(pcbe, pI2CRegParams,  LEVEL_LOW);
        if(bWriteAck)
        {
            cb_DelayMicroSeconds(I2C_SHORT_DELAY);
            cbI2CModule_SetSda(pcbe, pI2CRegParams,  LEVEL_HIGH);
        }
        cb_DelayMicroSeconds(I2CDELAY);
    }

    *pData = data;

    return bStatus;
}

/***************************************************************
Function:    cbI2CModule_WriteByte

Description: Write one byte data to I2C bus.

Input:       PCBIOS_EXTENSION_COMMON
             PCBIOS_MODULE_I2C_PARAMS

Output:

Return:
***************************************************************/
static CBIOS_BOOL cbI2CModule_WriteByte(PCBIOS_EXTENSION_COMMON pcbe,
                                        PCBIOS_MODULE_I2C_PARAMS pCBModuleI2CParams,
                                        CBIOS_U8 data, CBIOS_BOOL bReadAck)
{
    CBIOS_S32 i, value;
    CBIOS_U32 j = 0;
    CBIOS_U32 MaxLoop = MAXI2CWAITLOOP;
    CBIOS_U16  I2CDELAY = pCBModuleI2CParams->I2CDelay;
    CBIOS_BOOL bRet = CBIOS_TRUE;
    PCBIOS_I2C_REG_PARAMS pI2CRegParams = &(pCBModuleI2CParams->I2CRegParams);

    //check I2CBusNum
    if(cbI2CModule_CheckBusNum(pcbe, pCBModuleI2CParams) == CBIOS_FALSE)
    {
        cbDebugPrint((MAKE_LEVEL(GENERIC, DEBUG), "%s: I2CBusNum invalid! \n", FUNCTION_NAME));
        return CBIOS_FALSE;
    }

    //loop for 1 byte, start from MSB
    for (i = 7; i >= 0; i--)
    {
        value = (data >> i) & 0x01;

        //set SCL low
        cbI2CModule_SetScl(pcbe, pI2CRegParams, LEVEL_LOW);
        cb_DelayMicroSeconds(I2C_SHORT_DELAY);
        //Set SDA
        cbI2CModule_SetSda(pcbe, pI2CRegParams, (value == 0)? LEVEL_LOW : LEVEL_HIGH);
        cb_DelayMicroSeconds(I2CDELAY);

        //wait for SCL high
        for (j = 0; j < MaxLoop; j++)
        {
            cbI2CModule_SetScl(pcbe, pI2CRegParams, LEVEL_HIGH);
            cb_DelayMicroSeconds(I2CDELAY);
            if(LEVEL_HIGH == cbI2CModule_GetScl(pcbe, pI2CRegParams))
            {
                break;
            }
        }
        //check SCL/SDA
        if (j >= MaxLoop || value != cbI2CModule_GetSda(pcbe, pI2CRegParams))
        {
            cbDebugPrint((MAKE_LEVEL(GENERIC, WARNING), "%s: can't toggle SCL/SDA for bus %d!\n", FUNCTION_NAME, pCBModuleI2CParams->I2CBusNum));
            bRet = CBIOS_FALSE;
            break;
        }

        //Should release SDA when send out the LSB of one byte
        if(i == 0)
        {
            cbI2CModule_SetScl(pcbe, pI2CRegParams, LEVEL_LOW);
            cb_DelayMicroSeconds(I2C_SHORT_DELAY);
            cbI2CModule_SetSda(pcbe, pI2CRegParams, LEVEL_HIGH);
            cb_DelayMicroSeconds(I2CDELAY);
        }
    }

    if(bReadAck && bRet)
    {
        for (j = 0; j < MaxLoop; j++)
        {
            cbI2CModule_SetScl(pcbe, pI2CRegParams,  LEVEL_HIGH);
            cb_DelayMicroSeconds(I2CDELAY);
            if(LEVEL_HIGH == cbI2CModule_GetScl(pcbe, pI2CRegParams))
            {
                break;
            }
        }

        if (j >= MaxLoop)
        {
            cbDebugPrint((MAKE_LEVEL(GENERIC, WARNING), "%s: can't toggle SCL for bus %d!\n", FUNCTION_NAME, pCBModuleI2CParams->I2CBusNum));
            bRet = CBIOS_FALSE;
        }
        else
        {
            bRet = (cbI2CModule_GetSda(pcbe, pI2CRegParams) == LEVEL_LOW)? CBIOS_TRUE : CBIOS_FALSE;
        }

        cbI2CModule_SetScl(pcbe, pI2CRegParams,  LEVEL_LOW);
        cb_DelayMicroSeconds(I2C_SHORT_DELAY);
        cbI2CModule_SetSda(pcbe, pI2CRegParams,  LEVEL_HIGH);
        cb_DelayMicroSeconds(I2CDELAY);
    }

    return bRet;
}

/***************************************************************
Function:    cbI2CModule_GetI2CParams

Description: Get the clock and data line params of specific I2C bus.

Input:       PCBIOS_EXTENSION_COMMON
             PCBIOS_MODULE_I2C_PARAMS

Output:

Return:      CBIOS_TURE if get params successfully
             CBIOS_FALSE if get params failed
***************************************************************/
static CBIOS_BOOL cbI2CModule_GetI2CParams(PCBIOS_EXTENSION_COMMON pcbe, PCBIOS_MODULE_I2C_PARAMS pCBModuleI2CParams)
{
    CBIOS_U8 I2CBusNum = pCBModuleI2CParams->I2CBusNum;
    PCBIOS_I2C_REG_PARAMS pI2CRegParams = &(pCBModuleI2CParams->I2CRegParams);

    //check I2CBusNum
    if(cbI2CModule_CheckBusNum(pcbe, pCBModuleI2CParams) == CBIOS_FALSE)
    {
        cbDebugPrint((MAKE_LEVEL(GENERIC, DEBUG), "cbI2CModule_GetI2CParams: I2CBusNum invalid! \n"));
        return CBIOS_FALSE;
    }

    switch(I2CBusNum)
    {
    case I2CBUS0: //Normal I2C bus0
        {
            pI2CRegParams->CLineRegType_Index = CR_A0;
            pI2CRegParams->CLineRegReadEnableMask = 0xFF;
            pI2CRegParams->CLineRegReadEnableValue = 0;
            pI2CRegParams->CLineRegReadBitNum = 2;
            pI2CRegParams->CLineRegWriteBitNum = 0;
            pI2CRegParams->CLineRegWriteEnableMask = 0xEF;
            pI2CRegParams->CLineRegWriteEnableValue = 0x10;


            pI2CRegParams->DLineRegType_Index = CR_A0;
            pI2CRegParams->DLineRegReadEnableMask = 0xFF;
            pI2CRegParams->DLineRegReadEnableValue = 0;
            pI2CRegParams->DLineRegReadBitNum = 3;
            pI2CRegParams->DLineRegWriteBitNum = 1;
            pI2CRegParams->DLineRegWriteEnableMask = 0xEF;
            pI2CRegParams->DLineRegWriteEnableValue = 0x10;
        }
        break;
    case I2CBUS1: //Normal I2C bus1
        {
            pI2CRegParams->CLineRegType_Index = CR_AA;
            pI2CRegParams->CLineRegReadEnableMask = 0xFF;
            pI2CRegParams->CLineRegReadEnableValue = 0;
            pI2CRegParams->CLineRegReadBitNum = 2;
            pI2CRegParams->CLineRegWriteBitNum = 0;
            pI2CRegParams->CLineRegWriteEnableMask = 0xEF;
            pI2CRegParams->CLineRegWriteEnableValue = 0x10;


            pI2CRegParams->DLineRegType_Index = CR_AA;
            pI2CRegParams->DLineRegReadEnableMask = 0xFF;
            pI2CRegParams->DLineRegReadEnableValue = 0;
            pI2CRegParams->DLineRegReadBitNum = 3;
            pI2CRegParams->DLineRegWriteBitNum = 1;
            pI2CRegParams->DLineRegWriteEnableMask = 0xEF;
            pI2CRegParams->DLineRegWriteEnableValue = 0x10;
        }
        break;
    case I2CBUS2: //Normal I2C bus2
        {
            pI2CRegParams->CLineRegType_Index = CR_B_C5;
            pI2CRegParams->CLineRegReadEnableMask = 0xFF;
            pI2CRegParams->CLineRegReadEnableValue = 0;
            pI2CRegParams->CLineRegReadBitNum = 2;
            pI2CRegParams->CLineRegWriteBitNum = 0;
            pI2CRegParams->CLineRegWriteEnableMask = 0xEF;
            pI2CRegParams->CLineRegWriteEnableValue = 0x10;


            pI2CRegParams->DLineRegType_Index = CR_B_C5;
            pI2CRegParams->DLineRegReadEnableMask = 0xFF;
            pI2CRegParams->DLineRegReadEnableValue = 0;
            pI2CRegParams->DLineRegReadBitNum = 3;
            pI2CRegParams->DLineRegWriteBitNum = 1;
            pI2CRegParams->DLineRegWriteEnableMask = 0xEF;
            pI2CRegParams->DLineRegWriteEnableValue = 0x10;
        }
        break;
    case I2CBUS3: //Normal I2C bus3
        {
            pI2CRegParams->CLineRegType_Index = CR_B_C6;
            pI2CRegParams->CLineRegReadEnableMask = 0xFF;
            pI2CRegParams->CLineRegReadEnableValue = 0;
            pI2CRegParams->CLineRegReadBitNum = 2;
            pI2CRegParams->CLineRegWriteBitNum = 0;
            pI2CRegParams->CLineRegWriteEnableMask = 0xEF;
            pI2CRegParams->CLineRegWriteEnableValue = 0x10;


            pI2CRegParams->DLineRegType_Index = CR_B_C6;
            pI2CRegParams->DLineRegReadEnableMask = 0xFF;
            pI2CRegParams->DLineRegReadEnableValue = 0;
            pI2CRegParams->DLineRegReadBitNum = 3;
            pI2CRegParams->DLineRegWriteBitNum = 1;
            pI2CRegParams->DLineRegWriteEnableMask = 0xEF;
            pI2CRegParams->DLineRegWriteEnableValue = 0x10;
        }
        break;
    case I2CBUS4: //Normal I2C bus4
        {
            pI2CRegParams->CLineRegType_Index = CR_B_F8;
            pI2CRegParams->CLineRegReadEnableMask = 0xFF;
            pI2CRegParams->CLineRegReadEnableValue = 0;
            pI2CRegParams->CLineRegReadBitNum = 2;
            pI2CRegParams->CLineRegWriteBitNum = 0;
            pI2CRegParams->CLineRegWriteEnableMask = 0xEF;
            pI2CRegParams->CLineRegWriteEnableValue = 0x10;


            pI2CRegParams->DLineRegType_Index = CR_B_F8;
            pI2CRegParams->DLineRegReadEnableMask = 0xFF;
            pI2CRegParams->DLineRegReadEnableValue = 0;
            pI2CRegParams->DLineRegReadBitNum = 3;
            pI2CRegParams->DLineRegWriteBitNum = 1;
            pI2CRegParams->DLineRegWriteEnableMask = 0xEF;
            pI2CRegParams->DLineRegWriteEnableValue = 0x10;
        }
        break;
    case I2CBUS_SCLGPIO4_SDAGPIO5: //GPIO4-SCLK   GPIO5-SDAT
        {
            pI2CRegParams->CLineRegType_Index = SR_48;
            pI2CRegParams->CLineRegReadEnableMask = 0xBF;
            pI2CRegParams->CLineRegReadEnableValue = 0x40;
            pI2CRegParams->CLineRegReadBitNum = 7;
            pI2CRegParams->CLineRegWriteBitNum = 4;
            pI2CRegParams->CLineRegWriteEnableMask = 0xDF;
            pI2CRegParams->CLineRegWriteEnableValue = 0x20;


            pI2CRegParams->DLineRegType_Index = SR_4A;
            pI2CRegParams->DLineRegReadEnableMask = 0xFB;
            pI2CRegParams->DLineRegReadEnableValue = 0x04;
            pI2CRegParams->DLineRegReadBitNum = 3;
            pI2CRegParams->DLineRegWriteBitNum = 0;
            pI2CRegParams->DLineRegWriteEnableMask = 0xFD;
            pI2CRegParams->DLineRegWriteEnableValue = 0x02;
        }
        break;
    case I2CBUS_SCLGPIO5_SDAGPIO2: //GPIO5-SCLK GPIO2-SDAT
        {
            pI2CRegParams->CLineRegType_Index = SR_4A;
            pI2CRegParams->CLineRegReadEnableMask = 0xFB;
            pI2CRegParams->CLineRegReadEnableValue = 0x04;
            pI2CRegParams->CLineRegReadBitNum = 3;
            pI2CRegParams->CLineRegWriteBitNum = 0;
            pI2CRegParams->CLineRegWriteEnableMask = 0xFD;
            pI2CRegParams->CLineRegWriteEnableValue = 0x02;


            pI2CRegParams->DLineRegType_Index = CR_B_DA;
            pI2CRegParams->DLineRegReadEnableMask = 0xBF;
            pI2CRegParams->DLineRegReadEnableValue = 0x40;
            pI2CRegParams->DLineRegReadBitNum = 7;
            pI2CRegParams->DLineRegWriteBitNum = 4;
            pI2CRegParams->DLineRegWriteEnableMask = 0xDF;
            pI2CRegParams->DLineRegWriteEnableValue = 0x20;
        }
        break;
    case I2CBUS_SCLGPIO3_SDAGPIO4: // GPIO3-SCLK GPIO4-SDAT
        {
            pI2CRegParams->CLineRegType_Index = SR_48;
            pI2CRegParams->CLineRegReadEnableMask = 0xFB;
            pI2CRegParams->CLineRegReadEnableValue = 0x04;
            pI2CRegParams->CLineRegReadBitNum = 3;
            pI2CRegParams->CLineRegWriteBitNum = 0;
            pI2CRegParams->CLineRegWriteEnableMask = 0xFD;
            pI2CRegParams->CLineRegWriteEnableValue = 0x02;

            pI2CRegParams->DLineRegType_Index = SR_48;
            pI2CRegParams->DLineRegReadEnableMask = 0xBF;
            pI2CRegParams->DLineRegReadEnableValue = 0x40;
            pI2CRegParams->DLineRegReadBitNum = 7;
            pI2CRegParams->DLineRegWriteBitNum = 4;
            pI2CRegParams->DLineRegWriteEnableMask = 0xDF;
            pI2CRegParams->DLineRegWriteEnableValue = 0x20;
        }
        break;
    case I2CBUS_SCLGPIO2_SDAGPIO5://GPIO2-SCLK GPIO5-SDAT
        {
            pI2CRegParams->CLineRegType_Index = CR_B_DA;
            pI2CRegParams->CLineRegReadEnableMask = 0xBF;
            pI2CRegParams->CLineRegReadEnableValue = 0x40;
            pI2CRegParams->CLineRegReadBitNum = 7;
            pI2CRegParams->CLineRegWriteBitNum = 4;
            pI2CRegParams->CLineRegWriteEnableMask = 0xDF;
            pI2CRegParams->CLineRegWriteEnableValue = 0x20;

            pI2CRegParams->DLineRegType_Index = SR_4A;
            pI2CRegParams->DLineRegReadEnableMask = 0xFB;
            pI2CRegParams->DLineRegReadEnableValue = 0x04;
            pI2CRegParams->DLineRegReadBitNum = 3;
            pI2CRegParams->DLineRegWriteBitNum = 0;
            pI2CRegParams->DLineRegWriteEnableMask = 0xFD;
            pI2CRegParams->DLineRegWriteEnableValue = 0x02;
        }
        break;
    case I2CBUS_SCLGPIO2_SDAGPIO3://GPIO2-SCLK GPIO3-SDAT
        {
            pI2CRegParams->CLineRegType_Index = CR_B_DA;
            pI2CRegParams->CLineRegReadEnableMask = 0xBF;
            pI2CRegParams->CLineRegReadEnableValue = 0x40;
            pI2CRegParams->CLineRegReadBitNum = 7;
            pI2CRegParams->CLineRegWriteBitNum = 4;
            pI2CRegParams->CLineRegWriteEnableMask = 0xDF;
            pI2CRegParams->CLineRegWriteEnableValue = 0x20;

            pI2CRegParams->DLineRegType_Index = SR_48;
            pI2CRegParams->DLineRegReadEnableMask = 0xFB;
            pI2CRegParams->DLineRegReadEnableValue = 0x04;
            pI2CRegParams->DLineRegReadBitNum = 3;
            pI2CRegParams->DLineRegWriteBitNum = 0;
            pI2CRegParams->DLineRegWriteEnableMask = 0xFD;
            pI2CRegParams->DLineRegWriteEnableValue = 0x02;
        }
        break;
    default:
        {
            ASSERT(CBIOS_FALSE);
            cbDebugPrint((MAKE_LEVEL(GENERIC, ERROR),("Not support this I2C bus number %d!\n"), I2CBusNum));
            return CBIOS_FALSE;
        }
        break;
    }
    return CBIOS_TRUE;
}

/***********************************************************************************/
/*                              I2C Module Interfaces                              */
/***********************************************************************************/


/***************************************************************
Function:    cbI2CModule_ReadData

Description: Read data from I2C bus.

Input:       PCBIOS_VOID
             PCBIOS_MODULE_I2C_PARAMS

Output:      The data read is in pCBModuleI2CParams->Buffer

Return:      CBIOS_TURE if read data successfully
             CBIOS_FALSE if read data failed
***************************************************************/
CBIOS_BOOL cbI2CModule_ReadData(PCBIOS_VOID pvcbe, PCBIOS_MODULE_I2C_PARAMS pCBModuleI2CParams)
{
    CBIOS_U32 i = 0;
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    PCBIOS_MODULE_I2C_PARAMS  pI2CPara = pCBModuleI2CParams;
    CBIOS_U8  SlaveAddr = pI2CPara->SlaveAddress;
    CBIOS_U8  Offset = pI2CPara->OffSet;
    CBIOS_U32 BufferLen = pI2CPara->BufferLen;
    CBIOS_U8  I2CBusNum = pI2CPara->I2CBusNum;
    CBIOS_BOOL  bStatus = CBIOS_FALSE;

    cbTraceEnter(GENERIC);

    if (cbI2CModule_GetI2CParams(pcbe, pI2CPara) == CBIOS_FALSE)
    {
        return CBIOS_FALSE;
    }

    cbI2CModule_Enable(pcbe, pI2CPara);

    if((SlaveAddr != I2C_DDCCI_SLAVE_ADDR) && (SlaveAddr != I2C_EDID_SEGNUM_SLAVE_ADDR))
    {
        if (cbI2CModule_Start(pcbe, pI2CPara) == CBIOS_FALSE)
        {
            bStatus = CBIOS_FALSE;
            goto exit;
        }

        if (cbI2CModule_WriteByte(pcbe, pI2CPara, SlaveAddr, CBIOS_TRUE) == CBIOS_FALSE)
        {
            bStatus = CBIOS_FALSE;
            goto exit;
        }

        if (cbI2CModule_WriteByte(pcbe, pI2CPara, Offset, CBIOS_TRUE) == CBIOS_FALSE)
        {
            bStatus = CBIOS_FALSE;
            goto exit;
        }
    }

    if (cbI2CModule_Start(pcbe, pI2CPara) == CBIOS_FALSE)
    {
        bStatus = CBIOS_FALSE;
        goto exit;
    }

    if (cbI2CModule_WriteByte(pcbe, pI2CPara, SlaveAddr + 1, CBIOS_TRUE) == CBIOS_FALSE)  //LSB=1, indicate read
    {
        bStatus = CBIOS_FALSE;
        goto exit;
    }

    for(i = 0; i < BufferLen-1; i++)
    {
        if(cbI2CModule_ReadByte(pcbe, pI2CPara, &(pI2CPara->Buffer[i]), CBIOS_TRUE, CBIOS_FALSE) == CBIOS_FALSE)
        {
            bStatus = CBIOS_FALSE;
            goto exit;
        }
    }
    if(cbI2CModule_ReadByte(pcbe, pI2CPara, &(pI2CPara->Buffer[i]), CBIOS_FALSE, CBIOS_TRUE) == CBIOS_FALSE)
    {
        bStatus = CBIOS_FALSE;
        goto exit;
    }

    if(cbI2CModule_Stop(pcbe, pI2CPara) == CBIOS_FALSE)
    {
        bStatus = CBIOS_FALSE;
        goto exit;
    }
    bStatus = CBIOS_TRUE;

exit:
    if(!bStatus)
    {
        cbI2CModule_Stop(pcbe, pI2CPara);
    }
    cbI2CModule_Disable(pcbe, pI2CPara);
    
    cbTraceExit(GENERIC);
    
    return bStatus;
}

/***************************************************************
Function:    cbI2CModule_WriteData

Description: Write data to I2C bus.

Input:       PCBIOS_VOID
             PCBIOS_MODULE_I2C_PARAMS

Output:

Return:      CBIOS_TURE if write data successfully
             CBIOS_FALSE if write data failed
***************************************************************/
CBIOS_BOOL cbI2CModule_WriteData(PCBIOS_VOID pvcbe, PCBIOS_MODULE_I2C_PARAMS pCBModuleI2CParams)
{
    CBIOS_U32 i = 0;
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;
    PCBIOS_MODULE_I2C_PARAMS  pI2CPara = pCBModuleI2CParams;
    CBIOS_U8  SlaveAddr = pCBModuleI2CParams->SlaveAddress;
    CBIOS_U8  Offset = pCBModuleI2CParams->OffSet;
    CBIOS_U32 BufferLen = pCBModuleI2CParams->BufferLen;
    CBIOS_U8  I2CBusNum = pCBModuleI2CParams->I2CBusNum;
    CBIOS_BOOL bStatus = CBIOS_FALSE;

    cbTraceEnter(GENERIC);

    if (cbI2CModule_GetI2CParams(pcbe, pI2CPara) == CBIOS_FALSE)
    {
        return CBIOS_FALSE;
    }

    cbI2CModule_Enable(pcbe, pI2CPara);
    if (cbI2CModule_Start(pcbe, pI2CPara) == CBIOS_FALSE)
    {
        bStatus = CBIOS_FALSE;
        goto exit;
    }

    if (cbI2CModule_WriteByte(pcbe, pI2CPara, SlaveAddr, CBIOS_TRUE) == CBIOS_FALSE)
    {
        bStatus = CBIOS_FALSE;
        goto exit;
    }

    if((SlaveAddr != I2C_DDCCI_SLAVE_ADDR) && (SlaveAddr != I2C_EDID_SEGNUM_SLAVE_ADDR))
    {
        if (cbI2CModule_WriteByte(pcbe, pI2CPara, Offset, CBIOS_TRUE) == CBIOS_FALSE)
        {
            bStatus = CBIOS_FALSE;
            goto exit;
        }
    }

    for(i=0; i< BufferLen; i++)
    {
        if (cbI2CModule_WriteByte(pcbe, pI2CPara, pI2CPara->Buffer[i], CBIOS_TRUE) == CBIOS_FALSE)
        {
            bStatus = CBIOS_FALSE;
            goto exit;
        }
    }

    if(SlaveAddr != 0x60)//can't stop when write segment idx
    {
        if(cbI2CModule_Stop(pcbe, pI2CPara) == CBIOS_FALSE)
        {
            bStatus = CBIOS_FALSE;
            goto exit;
        }
    }

    bStatus = CBIOS_TRUE;

exit:
    if(!bStatus)
    {
        cbI2CModule_Stop(pcbe, pI2CPara);
    }
    cbI2CModule_Disable(pcbe, pI2CPara);
    
    cbTraceExit(GENERIC);
    
    return bStatus;

}

CBIOS_VOID cbI2CModule_HDCPI2CEnableDisable(PCBIOS_VOID pvcbe, PCBIOS_MODULE_I2C_PARAMS pCBModuleI2CParams, CBIOS_BOOL bIsEnable)
{
    PCBIOS_EXTENSION_COMMON pcbe = (PCBIOS_EXTENSION_COMMON)pvcbe;

    cbI2CModule_GetI2CParams(pcbe, pCBModuleI2CParams);

    if (bIsEnable)
    {
        cbI2CModule_Enable(pcbe, pCBModuleI2CParams);
    }
    else
    {
        cbI2CModule_Disable(pcbe, pCBModuleI2CParams);
    }
}
