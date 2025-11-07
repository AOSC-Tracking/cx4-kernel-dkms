//*****************************************************************************
//  Copyright (c) 2018 Shanghai Zhaoxin Semiconductor Co., Ltd.
//  All Rights Reserved.
//
//  This is UNPUBLISHED PROPRIETARY SOURCE CODE of Shanghai Zhaoxin Semiconductor Co., Ltd.;
//  the contents of this file may not be disclosed to third parties, copied or
//  duplicated in any form, in whole or in part, without the prior written
//  permission of Shanghai Zhaoxin Semiconductor Co., Ltd.
//
//  The copyright of the source code is protected by the copyright laws of the People's
//  Republic of China and the related laws promulgated by the People's Republic of China
//  and the international covenant(s) ratified by the People's Republic of China.
//*****************************************************************************
#ifndef __ZX_CHIP_ID_H
#define __ZX_CHIP_ID_H

#define PCI_ID_CHX004   0x3D01

#define PCI_ID_ELT3K   0x3D00 //Elite3000

#define PCI_ID_GENERIC_ELITE3K     PCI_ID_ELT3K

#define DEVICE_MASK     0xFF00
#define CHIP_MASK       0x00FF

#define DEVICE_ELITE3K     (PCI_ID_ELT3K  & DEVICE_MASK)
#define DEVICE_CHX004     (PCI_ID_CHX004  & DEVICE_MASK)

#define CHIP_EXCALIBUR2_CHX004     (PCI_ID_CHX004 & CHIP_MASK)

#define CHIP_ELITE3K     (PCI_ID_ELT3K & CHIP_MASK)

enum
{
    FAMILY_CMODEL,
    FAMILY_CLB,
    FAMILY_DST,
    FAMILY_CSR,
    FAMILY_INV,
    FAMILY_EXC,
    FAMILY_ELT,
    FAMILY_LAST,
};

enum
{
    CHIP_CMODEL,
    CHIP_CLB,
    CHIP_DST,
    CHIP_CSR,
    CHIP_INV,
    CHIP_H5,
    CHIP_H5S1,
    CHIP_H6S2,
    CHIP_CMS,
    CHIP_METRO,
    CHIP_MANHATTAN,
    CHIP_MATRIX,
    CHIP_DST2,
    CHIP_DST3,
    CHIP_DUMA,
    CHIP_H6S1,
    CHIP_DST4,
    CHIP_EXC1,      //Excalibur-1
    CHIP_E2UMA,     //E2UMA
    CHIP_ELT,       //Elite
    CHIP_ELT1K,     //Elite1k
    CHIP_ELT2K,     //Elite2k
    CHIP_ELT2K5,    //Elite2500
    CHIP_ZX2000,    //ZX2000
    CHIP_ELT3K,      //ELITE3K
    CHIP_CHX001,    //CHX001
    CHIP_CHX002,    //CHX002
    CHIP_ZX2100,    //ZX2100
    CHIP_CHX004,    //CHX004
    CHIP_CNE001,    //CNE001
    CHIP_LAST,      //Maximum number of chips supported.
};

#endif /*__ZX_CHIP_ID_H*/
