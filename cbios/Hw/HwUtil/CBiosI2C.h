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
** I2C module interface prototype and parameter definition.
**
** NOTE:
** This header file CAN ONLY be included by hw layer those files under Hw folder. 
******************************************************************************/

#ifndef _CBIOS_I2C_H_
#define _CBIOS_I2C_H_

#define LEVEL_HIGH              1       //line level high
#define LEVEL_LOW               0       //line level low
#define MAXI2CWAITLOOP          4000 

#define I2C_DDCCI_SLAVE_ADDR          0x6E
#define I2C_EDID_SEGNUM_SLAVE_ADDR    0x60

// I2C & GPIO pair as I2C index definition
typedef enum _CBIOS_I2CBUS_INDEX
{
    I2CBUS0                     =  0,
    I2CBUS1                     =  1,
    I2CBUS2                     =  2,
    I2CBUS3                     =  3,
    I2CBUS4                     =  4,
    RSVD_I2CBUS5                =  5,
    RSVD_I2CBUS6                =  6,
    RSVD_I2CBUS7                =  7,
    I2CBUS_SCLGPIO4_SDAGPIO5    =  8,
    I2CBUS_SCLGPIO5_SDAGPIO2    =  9, 
    I2CBUS_SCLGPIO3_SDAGPIO4    = 10, 
    I2CBUS_SCLGPIO2_SDAGPIO5    = 11,
    I2CBUS_SCLGPIO2_SDAGPIO3    = 12,
    RSVD_I2CBUS13               = 13,
    RSVD_I2CBUS14               = 14,
    RSVD_I2CBUS15               = 15,
    I2CBUS_VIRTUAL_DP1          = 16,
    I2CBUS_VIRTUAL_DP2          = 17,
    I2CBUS_VIRTUAL_DAC1         = 18,   //for no-edid device to store fake edid
    I2CBUS_VIRTUAL_DAC2         = 19,   //for no-edid device to store fake edid
    I2CBUS_VIRTUAL_DVO1         = 20,   //for no-edid device to store fake edid
    I2CBUS_VIRTUAL_DVO2         = 21,   //for no-edid device to store fake edid
    I2CBUS_VIRTUAL_COMBO1       = 22,   //for no-edid device to store fake edid
    I2CBUS_VIRTUAL_COMBO2       = 23,   //for no-edid device to store fake edid
    TOTAL_I2CBUS_NUM,
}CBIOS_I2CBUS_INDEX;

#define MIN_NORMAL_I2C_BUS      I2CBUS0
#define MAX_NORMAL_I2C_BUS      I2CBUS4
#define MIN_GPIO_I2C_BUS        I2CBUS_SCLGPIO4_SDAGPIO5
#define MAX_GPIO_I2C_BUS        I2CBUS_SCLGPIO2_SDAGPIO3

typedef struct _CBIOS_I2C_REG_PARAMS {
    CBIOS_U16         DLineRegType_Index;
    CBIOS_U8          DLineRegReadBitNum;
    CBIOS_U8          DLineRegReadEnableValue;   // only used for GPIO, should set value to zero for I2C
    CBIOS_U8          DLineRegReadEnableMask;    // only used for GPIO, should set mask to 0xFF for I2C 
    CBIOS_U8          DLineRegWriteBitNum;
    CBIOS_U8          DLineRegWriteEnableValue;  // equal port enable value for I2C
    CBIOS_U8          DLineRegWriteEnableMask;   // equal port enable mask for I2C
    CBIOS_U16         CLineRegType_Index;
    CBIOS_U8          CLineRegReadBitNum;
    CBIOS_U8          CLineRegReadEnableValue;   // only used for GPIO, should set value to zero for I2C
    CBIOS_U8          CLineRegReadEnableMask;    // only used for GPIO, should set mask to 0xFF for I2C
    CBIOS_U8          CLineRegWriteBitNum;
    CBIOS_U8          CLineRegWriteEnableValue;  // equal port enable value for I2C
    CBIOS_U8          CLineRegWriteEnableMask;   // equal port enable mask for I2C
} CBIOS_I2C_REG_PARAMS, *PCBIOS_I2C_REG_PARAMS;

typedef struct _CBIOS_MODULE_I2C_PARAMS {
    CBIOS_U32             Size;  
    CBIOS_I2C_REG_PARAMS  I2CRegParams;
    CBIOS_U8              I2CBusNum;
    CBIOS_U8              SlaveAddress;
    CBIOS_U8              OffSet;
    CBIOS_U8*             Buffer;
    CBIOS_U32             BufferLen;
    CBIOS_U32             Flags;
    CBIOS_U16             I2CDelay;
} CBIOS_MODULE_I2C_PARAMS, *PCBIOS_MODULE_I2C_PARAMS;

//I2C module interfaces
CBIOS_BOOL cbI2CModule_ReadData(PCBIOS_VOID pvcbe, PCBIOS_MODULE_I2C_PARAMS pCBModuleI2CParams);
CBIOS_BOOL cbI2CModule_WriteData(PCBIOS_VOID pvcbe, PCBIOS_MODULE_I2C_PARAMS pCBModuleI2CParams);
CBIOS_VOID cbI2CModule_HDCPI2CEnableDisable(PCBIOS_VOID pvcbe, PCBIOS_MODULE_I2C_PARAMS pCBModuleI2CParams, CBIOS_BOOL bIsEnable);

#endif
