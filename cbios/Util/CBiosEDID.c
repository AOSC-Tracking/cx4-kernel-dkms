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
** CBios EDID module interface function implementation.
** Parse the raw EDID data and store to the CBIOS_EDID_STRUCTURE_DATA.
**
** NOTE:
** ONLY EDID related and hw independent function can be added to this file.
******************************************************************************/

#include "CBiosChipShare.h"

static CBIOS_BOOL EstModeSupportTbl[] = {
    CBIOS_TRUE,    // EDID23h[0]: 800x600@60Hz
    CBIOS_FALSE,   // EDID23h[1]: 800x600@56Hz
    CBIOS_TRUE,    // EDID23h[2]: 640x480@75Hz
    CBIOS_TRUE,    // EDID23h[3]: 640x480@72Hz
    CBIOS_FALSE,   // EDID23h[4]: 640x480@67Hz
    CBIOS_TRUE,    // EDID23h[5]: 640x480@60Hz
    CBIOS_FALSE,   // EDID23h[6]: 720x400@88Hz
    CBIOS_FALSE,   // EDID23h[7]: 720x400@70Hz
    CBIOS_TRUE,    // EDID24h[0]: 1280x1024@75Hz
    CBIOS_TRUE,    // EDID24h[1]: 1024x768@75Hz
    CBIOS_TRUE,    // EDID24h[2]: 1024x768@70Hz
    CBIOS_TRUE,    // EDID24h[3]: 1024x768@60Hz
    CBIOS_FALSE,   // EDID24h[4]: 1024x768@87HzInterlace
    CBIOS_FALSE,   // EDID24h[5]: 832x624@75Hz
    CBIOS_TRUE,    // EDID24h[6]: 800x600@75Hz
    CBIOS_TRUE,    // EDID24h[7]: 800x600@72Hz
    CBIOS_FALSE,   // EDID25h[7]: 1152x870@75Hz
};

CBIOS_U8 PHL_24PFL3545_Edid[256] =
{
    0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x41, 0x0C, 0x10, 0x00, 0x01, 0x01, 0x01, 0x01, 
    0x00, 0x17, 0x01, 0x03, 0x80, 0x35, 0x1E, 0x78, 0x2A, 0x47, 0x89, 0xA3, 0x57, 0x47, 0x9F, 0x22, 
    0x11, 0x48, 0x4C, 0xBF, 0xEF, 0x80, 0xB3, 0x00, 0x95, 0x00, 0x81, 0x80, 0x81, 0x40, 0x71, 0x4F, 
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x02, 0x3A, 0x80, 0x18, 0x71, 0x38, 0x2D, 0x40, 0x58, 0x2C, 
    0x45, 0x00, 0x09, 0x25, 0x21, 0x00, 0x00, 0x1E, 0x66, 0x21, 0x50, 0xB0, 0x51, 0x00, 0x1B, 0x30, 
    0x40, 0x70, 0x36, 0x00, 0x09, 0x25, 0x21, 0x00, 0x00, 0x1E, 0x00, 0x00, 0x00, 0xFD, 0x00, 0x37, 
    0x4C, 0x1E, 0x52, 0x11, 0x00, 0x0A, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x00, 0x00, 0x00, 0xFC, 
    0x00, 0x42, 0x32, 0x34, 0x50, 0x46, 0x4C, 0x33, 0x35, 0x34, 0x35, 0x2F, 0x54, 0x33, 0x01, 0x31, 
    0x02, 0x03, 0x22, 0xF2, 0x4F, 0x9F, 0x14, 0x13, 0x12, 0x11, 0x16, 0x15, 0x90, 0x05, 0x04, 0x03, 
    0x02, 0x07, 0x06, 0x01, 0x23, 0x09, 0x07, 0x01, 0x83, 0x01, 0x00, 0x00, 0x65, 0x03, 0x0C, 0x00, 
    0x10, 0x00, 0x02, 0x3A, 0x80, 0xD0, 0x72, 0x38, 0x2D, 0x40, 0x10, 0x2C, 0x45, 0x80, 0x09, 0x25, 
    0x21, 0x00, 0x00, 0x1E, 0x01, 0x1D, 0x80, 0xD0, 0x72, 0x1C, 0x16, 0x20, 0x10, 0x2C, 0x25, 0x80, 
    0x09, 0x25, 0x21, 0x00, 0x00, 0x9E, 0x01, 0x1D, 0x00, 0xBC, 0x52, 0xD0, 0x1E, 0x20, 0xB8, 0x28, 
    0x55, 0x40, 0x09, 0x25, 0x21, 0x00, 0x00, 0x18, 0x8C, 0x0A, 0xD0, 0x90, 0x20, 0x40, 0x31, 0x20, 
    0x0C, 0x40, 0x55, 0x00, 0x09, 0x25, 0x21, 0x00, 0x00, 0x18, 0x02, 0x3A, 0x80, 0x18, 0x71, 0x38, 
    0x2D, 0x40, 0x58, 0x2C, 0x45, 0x00, 0x09, 0x25, 0x21, 0x00, 0x00, 0x1E, 0x00, 0x00, 0x00, 0x26, 
};

CBIOS_U8 AUO_B145QAN01_Edid[128] =
{
    0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x06, 0xaf, 0xa7, 0x7a, 0x00, 0x00, 0x00, 0x00,
    0x25, 0x20, 0x01, 0x04, 0xb5, 0x1f, 0x14, 0x78, 0x03, 0xee, 0x95, 0xa3, 0x54, 0x4c, 0x99, 0x26,
    0x0f, 0x50, 0x54, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
    0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0xD1, 0x9F, 0x00, 0xA2, 0xA0, 0x40, 0x89, 0x63, 0x30, 0x20,
    0x35, 0x00, 0x38, 0xC3, 0x10, 0x00, 0x00, 0x18, 0xD1, 0x9F, 0x00, 0xA2, 0xA0, 0x40, 0x46, 0x60,
    0x30, 0x20, 0x35, 0x00, 0x38, 0xC3, 0x10, 0x00, 0x00, 0x18, 0x00, 0x00, 0x00, 0xFE, 0x00, 0x41,
    0x55, 0x4F, 0x0A, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x00, 0x00, 0x00, 0xFE,
    0x00, 0x4C, 0x45, 0x4E, 0x31, 0x34, 0x35, 0x57, 0x51, 0x58, 0x47, 0x41, 0x20, 0x20, 0x00, 0x29,
};

CBIOS_U8 CSW1473_Edid[256] =
{
    0x00,0xff,0xff,0xff,0xff,0xff,0xff,0x00,0x0e,0x77,0x73,0x14,0x00,0x00,0x00,0x00,
    0x27,0x23,0x01,0x04,0xa5,0x1f,0x14,0x78,0x07,0xee,0x95,0xa3,0x54,0x4c,0x99,0x26,
    0x0f,0x50,0x54,0x00,0x00,0x00,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x01,
    0x01,0x01,0x01,0x01,0x01,0x01,0x67,0x3e,0x80,0xa0,0x70,0xb0,0x50,0x40,0x30,0x20,
    0x36,0x00,0x38,0xc3,0x10,0x00,0x00,0x1a,0x9a,0x5d,0x80,0xa0,0x70,0xb0,0x50,0x40,
    0x30,0x20,0x36,0x00,0x38,0xc3,0x10,0x00,0x00,0x1a,0x00,0x00,0x00,0xfd,0x00,0x30,
    0x5a,0x74,0x74,0x18,0x01,0x0a,0x20,0x20,0x20,0x20,0x20,0x20,0x00,0x00,0x00,0xfc,
    0x00,0x4d,0x4e,0x45,0x35,0x30,0x37,0x51,0x53,0x32,0x2d,0x34,0x0a,0x20,0x01,0x4e,
    0x70,0x20,0x79,0x02,0x00,0x81,0x00,0x15,0x74,0x1a,0x00,0x00,0x03,0x01,0x30,0x5a,
    0x00,0x00,0x00,0x00,0x00,0x00,0x5a,0x00,0x00,0x00,0x00,0xad,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xac,0x90,
};

// This table is for new CBIOS setting mode logic.
// When add item to this table, must notice:
// 1. Can't repeat the item in current table.
// 2. Must stored by sort.
static CBIOS_TIMING_ATTRIB AdapterTimingTbl[] =
{
{
    TIMING_ATTRIB_SIZE,
    0,
    640,350,
    8500,
    VerNEGATIVE + HorPOSITIVE,
    315000,
    832,            /*Hor Total Time*/
    640,            /*Hor Display End*/
    640,            /*Hor Blank Start*/
    640 + 192,      /*Hor Blank End*/
    640 + 32,       /*Hor Sync Start*/
    640 + 32 + 64,  /*Hor Sync End*/
    445,            /*Ver Total Time*/
    350,            /*Ver Display End*/
    350,            /*Ver Blank Start*/
    350 + 95,       /*Ver Blank End*/
    350 + 32,       /*Ver Sync Start*/
    350 + 32 + 3,   /*Ver Sync End*/
},
{
    TIMING_ATTRIB_SIZE,
    0,
    640,400,
    8500,
    VerPOSITIVE + HorNEGATIVE,
    315000,
    832,
    640,
    640,
    640 + 192,
    640 + 32,
    640 + 32 + 64,
    445,
    400,
    400,
    400 + 45,
    400 + 1,
    400 + 1 + 3,
},
{
    TIMING_ATTRIB_SIZE,
    0,
    720,400,
    8500,
    VerPOSITIVE + HorNEGATIVE,
    355000,
    936,
    720,
    720,
    720 + 216,
    720 + 36,
    720 + 36 + 72,
    446,
    400,
    400,
    400 + 46,
    400 + 1,
    400 + 1 + 3,
},
{
    TIMING_ATTRIB_SIZE,
    0,
    640,480,
    6000,
    VerNEGATIVE+HorNEGATIVE,
    251750,
    100*8,
    80*8,
    81*8,
    (100-1)*8 ,
    82*8,
    (82+12)*8   ,
    525,
    480,
    488+1,
    488+29,
    490,
    490+2,
},
{
    TIMING_ATTRIB_SIZE,
    0,
    640,480,
    7200,
    VerNEGATIVE+HorNEGATIVE,
    315000,
    104*8,
    80*8,
    81*8,
    (81+22)*8,
    83*8,
    (83+5) *8,
    520,
    480,
    488,
    488+24,
    489,
    489+3,
},
{
    TIMING_ATTRIB_SIZE,
    0,
    640,480,
    7500,
    VerNEGATIVE+HorNEGATIVE,
    315000,
    105*8,
    80*8,
    80*8,
    (80+25)*8,
    82*8,
    (82+8) *8,
    500,
    480,
    480,
    480+20,
    481,
    481+3,
},
{
    TIMING_ATTRIB_SIZE,
    0,
    640,480,
    8500,
    VerNEGATIVE+HorNEGATIVE,
    360000,
    104*8,
    80*8,
    80*8,
    (80+24) *8,
    87*8,
    (87+7) *8,
    509,
    480,
    480,
    480+29,
    481,
    481+3,
},
{
    TIMING_ATTRIB_SIZE,
    0,
    800,600,
    5600,
    VerPOSITIVE+HorPOSITIVE,
    360000,
    1024,
    800,
    800,
    800 + 224,
    800 + 24,
    800 + 24 + 72,
    625,
    600,
    600,
    600 + 25,
    600 + 1,
    600 + 1 + 2,
},
{
    TIMING_ATTRIB_SIZE,
    0,
    800,600,
    6000,
    VerPOSITIVE+HorPOSITIVE,
    400000,
    132*8,
    100*8,
    100*8,
    (100+32) *8,
    105*8,
    (105+16) *8,
    628,
    600,
    600,
    600+28,
    601,
    601+4,
},
{
    TIMING_ATTRIB_SIZE,
    0,
    800,600,
    7200,
    VerPOSITIVE+HorPOSITIVE,
    500000,
    130*8,
    100*8,
    100*8,
    (100+30)*8,
    107*8,
    (107+15)*8,
    666,
    600,
    600,
    600+66,
    637,
    637+6,
},
{
    TIMING_ATTRIB_SIZE,
    0,
    800,600,
    7500,
    VerPOSITIVE+HorPOSITIVE,
    495000,
    132*8,
    100*8,
    100*8,
    (100+32) *8,
    102*8,
    (102+10) *8,
    625,
    600,
    600,
    600+25,
    601,
    601+3,
},
{
    TIMING_ATTRIB_SIZE,
    0,
    800,600,
    8500,
    VerPOSITIVE+HorPOSITIVE,
    562500,
    131*8,
    100*8,
    100*8,
    (100+31)*8,
    104*8,
    (104+8) *8,
    631,
    600,
    600,
    600+31,
    601,
    601+3,
},
{
    TIMING_ATTRIB_SIZE,
    0,
    800,600,
    10000,
    VerPOSITIVE+HorNEGATIVE,
    672500,
    132*8,
    100*8,
    100*8,
    (100+32) *8,
    106*8,
    (106+10) *8,
    639,
    600,
    600,
    600+39,
    603,
    603+4,
},
/** Reduced Blanking
{
    TIMING_ATTRIB_SIZE,      
    0,
    800,600,
    12000,
    VerNEGATIVE + HorPOSITIVE,
    732500,
    960,
    800,
    800,
    800 + 160,
    800 + 48,
    800 + 48 + 32,
    636,
    600,
    600,
    600 + 36,
    600 + 3,
    600 + 3 + 4,
},
********************/
{
    TIMING_ATTRIB_SIZE,
    0,
    848,480,
    6000,
    VerPOSITIVE + HorPOSITIVE,
    337500,
    1088,
    848,
    848,
    848 + 240,
    848 + 16,
    848 + 16 +112,
    517,
    480,
    480,
    480 + 37,
    480 + 6,
    480 + 6 +8,
},
{
    TIMING_ATTRIB_SIZE,
    0,
    1024,768,
    6000,
    VerNEGATIVE+HorNEGATIVE,
    650000,
    168*8,
    128*8,
    128*8,
    (128+40) *8,
    131*8,
    (131+17) *8,
    806,
    768,
    768,
    768+38,
    771,
    771+6,
},
{
    TIMING_ATTRIB_SIZE,
    0,
    1024,768,
    7000,
    VerNEGATIVE+HorNEGATIVE,
    750000,
    166*8,
    128*8,
    128*8,
    (128+38) *8,
    131*8,
    (131+17) *8,
    806,
    768,
    768,
    768+38,
    771,
    771+6,
},
{
    TIMING_ATTRIB_SIZE,
    0,
    1024,768,
    7500,
    VerPOSITIVE+HorPOSITIVE,
    787500,
    164*8,
    128*8,
    128*8,
    (128+36)*8,
    130*8,
    (130+12) *8,
    800,
    768,
    768,
    768+32,
    769,
    769+3,
},
{
    TIMING_ATTRIB_SIZE,
    0,
    1024,768,
    8500,
    VerPOSITIVE+HorPOSITIVE,
    945000,
    172*8,
    128*8,
    128*8,
    (128+44)*8,
    134*8,
    (134+12) *8,
    808,
    768,
    768,
    768+40,
    769,
    769+3,
},
{
    TIMING_ATTRIB_SIZE,
    0,
    1024,768,
    10000,
    VerPOSITIVE+HorNEGATIVE,
    1122500,
    172*8,
    128*8,
    128*8,
    (128+44) *8,
    137*8,
    (137+13-1) *8,
    816,
    768,
    768,
    768+48,
    771,
    771+4,
},
/** Reduced Blanking
{
    TIMING_ATTRIB_SIZE,     
    0,
    1024,768,
    12000,
    VerNEGATIVE + HorPOSITIVE,
    1155000,
    1184,
    1024,
    1024,
    1024 + 160,
    1024 + 48,
    1024 + 48 + 32,
    813,
    768,
    768,
    768 + 45,
    768 + 3,
    768 + 3 + 4,
},
*******************/
{
    TIMING_ATTRIB_SIZE,
    0,
    1024,768,
    12000,
    VerPOSITIVE+HorNEGATIVE,
    1390000,
    176*8,
    128*8,
    128*8,
    (128+48) *8,
    138*8,
    (138+14) *8,
    823,
    768,
    768,
    768+55,
    769,
    769+3,
},
{
    TIMING_ATTRIB_SIZE,
    0,
    1152,864,
    7500,
    VerPOSITIVE + HorPOSITIVE,
    1080000,
    1600,
    1152,
    1152,
    1152 + 448,
    1152 + 64,
    1152 + 64 + 128,
    900,
    864,
    864,
    864 + 36,
    864 + 1,
    864 + 1 + 3,
},
{
    TIMING_ATTRIB_SIZE,
    0,
    1280,720,
    5000,
    VerPOSITIVE+HorPOSITIVE,
    742500,
    1980,
    1280,
    1280,
    1980,
    1280+440,
    (1280+440+40) ,
    750,
    720,
    720,
    720+30,
    725,
    725+5,
},
{
    TIMING_ATTRIB_SIZE,    
    0,
    1280,720,
    6000,
    VerPOSITIVE+HorPOSITIVE,
    742500,
    1650,
    1280,
    1280,
    1650 ,
    1280+110,
    (1280+110+40) ,
    750,
    720,
    720,
    720+30,
    725,
    725+5,
},
/** Reduced Blanking
{
    TIMING_ATTRIB_SIZE,     
    0,
    1280,768,
    6000,
    VerNEGATIVE + HorPOSITIVE,
    682500,
    1440,
    1280,
    1280,
    1280 + 160,
    1280 + 48,
    1280 + 48 + 32,
    790,
    768,
    768,
    768 + 22,
    768 + 3,
    768 + 3 + 7,
},
*******************/
{
    TIMING_ATTRIB_SIZE,
    0,
    1280,768,
    6000,
    VerPOSITIVE + HorNEGATIVE,
    795000,
    1664,
    1280,
    1280,
    1280 + 384,
    1280 + 64,
    1280 + 64 + 128,
    798,
    768,
    768,
    768 + 30,
    768 + 3,
    768 + 3 + 7,
},
{
    TIMING_ATTRIB_SIZE,
    0,
    1280,768,
    7500,
    VerPOSITIVE + HorNEGATIVE,
    1022500,
    1696,
    1280,
    1280,
    1280 + 416,
    1280 + 80,
    1280 + 80 + 128,
    805,
    768,
    768,
    768 + 37,
    768 + 3,
    768 + 3 + 7,
},
{
    TIMING_ATTRIB_SIZE,
    0,
    1280,768,
    8500,
    VerPOSITIVE + HorNEGATIVE,
    1175000,
    1712,
    1280,
    1280,
    1280 + 432,
    1280 + 80,
    1280 + 80 + 136,
    809,
    768,
    768,
    768 + 41,
    768 + 3,
    768 + 3 + 7,
},
/** Reduced Blanking
{
    TIMING_ATTRIB_SIZE,    
    0,
    1280,768,
    12000,
    VerNEGATIVE + HorPOSITIVE,
    1402500,
    1440,
    1280,
    1280,
    1280 + 160,
    1280 + 48,
    1280 + 48 + 32,
    813,
    768,
    768,
    768 + 45,
    768 + 3,
    768 + 3 + 7,
},
*********************/
/** Reduced Blanking
{
    TIMING_ATTRIB_SIZE,    
    0,
    1280,800,
    6000,
    VerNEGATIVE + HorPOSITIVE,
    710000,
    1440,
    1280,
    1280,
    1280 + 160,
    1280 + 48,
    1280 + 48 + 32,
    823,
    800,
    800,
    800 + 23,
    800 + 3,
    800 + 3 + 6,
},
*****************/
{
    TIMING_ATTRIB_SIZE,
    0,
    1280,800,
    6000,
    VerPOSITIVE + HorNEGATIVE,
    835000,
    1680,
    1280,
    1280,
    1280 + 400,
    1280 + 72,
    1280 + 72 + 128,
    831,
    800,
    800,
    800 + 31,
    800 + 3,
    800 + 3 + 6,
},
{
    TIMING_ATTRIB_SIZE,
    0,
    1280,800,
    7500,
    VerPOSITIVE + HorNEGATIVE,
    1065000,
    1696,
    1280,
    1280,
    1280 + 416,
    1280 + 80,
    1280 + 80 + 128,
    838,
    800,
    800,
    800 + 38,
    800 + 3,
    800 + 3 + 6,
},
{
    TIMING_ATTRIB_SIZE,
    0,
    1280,800,
    8500,
    VerPOSITIVE + HorNEGATIVE,
    1225000,
    1712,
    1280,
    1280,
    1280 + 432,
    1280 + 80,
    1280 + 80 + 136,
    843,
    800,
    800,
    800 + 43,
    800 + 3,
    800 + 3 + 6,
},
/** Reduced Blanking
{
    TIMING_ATTRIB_SIZE,     
    0,
    1280,800,
    12000,
    VerNEGATIVE + HorPOSITIVE,
    1462500,
    1440,
    1280,
    1280,
    1280 + 160,
    1280 + 48,
    1280 + 48 + 32,
    847,
    800,
    800,
    800 + 47,
    800 + 3,
    800 + 3 + 6,
},
******************/
{
    TIMING_ATTRIB_SIZE,
    0,
    1280,960,
    6000,
    VerPOSITIVE + HorPOSITIVE,
    1080000,
    1800,
    1280,
    1280,
    1280 + 520,
    1280 + 96,
    1280 + 96 + 112,
    1000,
    960,
    960,
    960 + 40,
    960 + 1,
    960 + 1 + 3,
},
{
    TIMING_ATTRIB_SIZE,
    0,
    1280,960,
    8500,
    VerPOSITIVE + HorPOSITIVE,
    1485000,
    1728,
    1280,
    1280,
    1280 + 448,
    1280 + 64,
    1280 + 64 + 160,
    1011,
    960,
    960,
    960 + 51,
    960 + 1,
    960 + 1 + 3,
},
/*** Reduced Blanking
{
    TIMING_ATTRIB_SIZE,        
    0,
    1280,960,
    12000,
    VerNEGATIVE + HorPOSITIVE,
    1755000,
    1440,
    1280,
    1280,
    1280 + 160,
    1280 + 48,
    1280 + 48 + 32,
    1017,
    960,
    960,
    960 + 57,
    960 + 3,
    960 + 3 + 4,
},
*********************/
{
     TIMING_ATTRIB_SIZE,
     0,
     1280,1024,
     6000,
     VerPOSITIVE+HorPOSITIVE,
     1080000,
     211*8,
     160*8,
     160*8,
     (160+51)*8,
     166*8,
     (166+14) *8,
     1066,
     1024,
     1024,
     1024+42,
     1025,
     1025+3,
},
{
     TIMING_ATTRIB_SIZE,
     0,
     1280,1024,
     7500,
     VerPOSITIVE+HorPOSITIVE,
     1350000,
     211*8,
     160*8,
     160*8,
     (160+51) *8,
     162*8,
     (162+18) *8,
     1066,
     1024,
     1024,
     1024+42,
     1025,
     1025+3,
},
{
    TIMING_ATTRIB_SIZE,
    0,
    1280,1024,
    8500,
    VerPOSITIVE+HorPOSITIVE,
    1575000,
    1728,
    1280,
    1280,
    1280 + 448,
    1280 + 64,
    1280 + 64 + 160,
    1072,
    1024,
    1024,
    1024 + 48,
    1024 + 1,
    1024 + 1 + 3,
},
/*** Reduced Blanking
{
    TIMING_ATTRIB_SIZE,     
    0,
    1280,1024,
    12000,
    VerNEGATIVE + HorPOSITIVE,
    1872500,
    1440,
    1280,
    1280,
    1280 + 160,
    1280 + 48,
    1280 + 48 + 32,
    1084,
    1024,
    1024,
    1024 + 60,
    1024 + 3,
    1024 + 3 + 7,
},
**********************/
{
    TIMING_ATTRIB_SIZE,
    0,
    1360,768,
    6000,
    VerPOSITIVE+HorPOSITIVE,
    855000,
    1792,
    1360,
    1360,
    1360 + 432,
    1360 + 64,
    1360 + 64 + 112,
    795,
    768,
    768,
    768 + 27,
    768 + 3,
    768 + 3 + 6,
},
/*** Reduced Blanking
{
    TIMING_ATTRIB_SIZE,      
    0,
    1360,768,
    12000,
    VerNEGATIVE + HorPOSITIVE,
    1482500,
    1520,
    1360,
    1360,
    1360 + 160,
    1360 + 48,
    1360 + 48 + 32,
    813,
    768,
    768,
    768 + 45,
    768 + 3,
    768 + 3 + 5,
},
*********************/
/*** Reduced Blanking
{
    TIMING_ATTRIB_SIZE,         
    0,
    1366,768,
    6000,
    VerPOSITIVE+HorPOSITIVE,
    720000,
    1500,
    1366,
    1366,
    1366 + 134,
    1366 + 14,
    1366 + 14 + 56,
    800,
    768,
    768,
    768 + 32,
    768 + 1,
    768 + 1 + 3,
},
******************/
{
    TIMING_ATTRIB_SIZE,
    0,
    1366,768,
    6000,
    VerPOSITIVE+HorPOSITIVE,
    855000,
    1792,
    1366,
    1366,
    1366 + 426,
    1366 + 70,
    1366 + 70 + 143,
    798,
    768,
    768,
    768 + 30,
    768 + 3,
    768 + 3 + 3,
},
/***** Reduced Blanking
{
    TIMING_ATTRIB_SIZE,     
    0,
    1400,1050,
    6000,
    VerNEGATIVE + HorPOSITIVE,
    1010000,
    1560,
    1400,
    1400,
    1400 + 160,
    1400 + 48,
    1400 + 48 + 32,
    1080,
    1050,
    1050,
    1050 + 30,
    1050 + 3,
    1050 + 3 + 4,
},
*********************/
{
    TIMING_ATTRIB_SIZE,
    0,
    1400,1050,
    6000,
    VerPOSITIVE + HorNEGATIVE,
    1217500,
    1864,
    1400,
    1400,
    1400 + 464,
    1400 + 88,
    1400 + 88 + 144,
    1089,
    1050,
    1050,
    1050 + 39,
    1050 + 3,
    1050 + 3 + 4,
},
{
    TIMING_ATTRIB_SIZE,
    0,
    1400,1050,
    7500,
    VerPOSITIVE + HorNEGATIVE,
    1560000,
    1896,
    1400,
    1400,
    1400 + 496,
    1400 + 104,
    1400 + 104 + 144,
    1099,
    1050,
    1050,
    1050 + 49,
    1050 + 3,
    1050 + 3 + 4,
},
{
    TIMING_ATTRIB_SIZE,
    0,
    1400,1050,
    8500,
    VerPOSITIVE + HorNEGATIVE,
    1795000,
    1912,
    1400,
    1400,
    1400 + 512,
    1400 + 104,
    1400 + 104 + 152,
    1105,
    1050,
    1050,
    1050 + 55,
    1050 + 3,
    1050 + 3 + 4,
},
/******* Reduced Blanking
{
    TIMING_ATTRIB_SIZE,
    0,
    1400,1050,
    12000,
    VerNEGATIVE + HorPOSITIVE,
    2080000,
    1560,
    1400,
    1400,
    1400 + 160,
    1400 + 48,
    1400 + 48 + 32,
    1112,
    1050,
    1050,
    1050 + 62,
    1050 + 3,
    1050 + 3 + 4, 
},
***********************/

/****** Reduced Blanking
{
    TIMING_ATTRIB_SIZE,
    0,
    1440,900,
    6000,
    VerNEGATIVE + HorPOSITIVE,
    887500,
    1600,
    1440,
    1440,
    1440 + 160,
    1440 + 48,
    1440 + 48 + 32,
    926,
    900,
    900,
    900 + 26,
    900 + 3,
    900 + 3 + 6,
},
**********************/
{
    TIMING_ATTRIB_SIZE,
    0,
    1440,900,
    6000,
    VerPOSITIVE + HorNEGATIVE,
    1065000,
    1904,
    1440,
    1440,
    1440 + 464,
    1440 + 80,
    1440 + 80 + 152,
    934,
    900,
    900,
    900 + 34,
    900 + 3,
    900 + 3 + 6,
},
{
    TIMING_ATTRIB_SIZE,
    0,
    1440,900,
    7500,
    VerPOSITIVE + HorNEGATIVE,
    1367500,
    1936,
    1440,
    1440,
    1440 + 496,
    1440 + 96,
    1440 + 96 + 152,
    942,
    900,
    900,
    900 + 42,
    900 + 3,
    900 + 3 + 6,
},
{
    TIMING_ATTRIB_SIZE,
    0,
    1440,900,
    8500,
    VerPOSITIVE + HorNEGATIVE,
    1570000,
    1952,
    1440,
    1440,
    1440 + 512,
    1440 + 104,
    1440 + 104 + 152,
    948,
    900,
    900,
    900 + 48,
    900 + 3,
    900 + 3 + 6,
},
/***** Reduced Blanking
{
    TIMING_ATTRIB_SIZE,
    0,
    1440,900,
    12000,
    VerNEGATIVE + HorPOSITIVE,
    1827500,
    1600,
    1440,
    1440,
    1440 + 160,
    1440 + 48,
    1440 + 48 + 32,
    953,
    900,
    900,
    900 + 53,
    900 + 3,
    900 + 3 + 6,
},
*********************/
{
    TIMING_ATTRIB_SIZE,
    0,
    1600,900,
    6000,
    VerPOSITIVE+HorPOSITIVE,
    1080000,
    1800,
    1600,
    1600,
    1600 + 200,
    1600 + 24,
    1600 + 24 + 80,
    1000,
    900,
    900,
    900 + 100,
    900 + 1,
    900 + 1 + 3,
},
{
    TIMING_ATTRIB_SIZE,
    0,
    1600,1200,
    6000,
    VerPOSITIVE+HorPOSITIVE,
    1620000,
    2160,
    1600,
    1600,
    1600 + 560,
    1600 + 64,
    1600 + 64 + 192,
    1250,
    1200,
    1200,
    1200 + 50,
    1200 + 1,
    1200 + 1 + 3,
},
{
    TIMING_ATTRIB_SIZE,
    0,
    1600,1200,
    6500,
    VerPOSITIVE+HorPOSITIVE,
    1755000,
    2160,
    1600,
    1600,
    1600 + 560,
    1600 + 64,
    1600 + 64 + 192,
    1250,
    1200,
    1200,
    1200 + 50,
    1200 + 1,
    1200 + 1 + 3,
},
{
    TIMING_ATTRIB_SIZE,
    0,
    1600,1200,
    7000,
    VerPOSITIVE+HorPOSITIVE,
    1890000,
    2160,
    1600,
    1600,
    1600 + 560,
    1600 + 64,
    1600 + 64 + 192,
    1250,
    1200,
    1200,
    1200 + 50,
    1200 + 1,
    1200 + 1 + 3,
},
{
    TIMING_ATTRIB_SIZE,
    0,
    1600,1200,
    7500,
    VerPOSITIVE+HorPOSITIVE,
    2025000,
    2160,
    1600,
    1600,
    1600 + 560,
    1600 + 64,
    1600 + 64 + 192,
    1250,
    1200,
    1200,
    1200 + 50,
    1200 + 1,
    1200 + 1 + 3,
},
{
    TIMING_ATTRIB_SIZE,
    0,
    1600,1200,
    8500,
    VerPOSITIVE+HorPOSITIVE,
    2295000,
    2160,
    1600,
    1600,
    1600 + 560,
    1600 + 64,
    1600 + 64 + 192,
    1250,
    1200,
    1200,
    1200 + 50,
    1200 + 1,
    1200 + 1 + 3,
},
/***** Reduced Blanking
{
    TIMING_ATTRIB_SIZE,
    0,
    1600,1200,
    12000,
    VerNEGATIVE + HorPOSITIVE,
    2682500,
    1760,
    1600,
    1600,
    1600 + 160,
    1600 + 48,
    1600 + 48 + 32,
    1271,
    1200,
    1200,
    1200 + 71,
    1200 + 3,
    1200 + 3 + 4,
},
********************/
/***** Reduced Blanking
{
    TIMING_ATTRIB_SIZE,
    0,
    1680,1050,
    6000,
    VerNEGATIVE + HorPOSITIVE,
    1190000,
    1840,
    1680,
    1680,
    1680 + 160,
    1680 + 48,
    1680 + 48 + 32,
    1080,
    1050,
    1050,
    1050 + 30,
    1050 + 3,
    1050 + 3 + 6,
},
******************/
{
    TIMING_ATTRIB_SIZE,
    0,
    1680,1050,
    6000,
    VerPOSITIVE + HorNEGATIVE,
    1462500,
    2240,
    1680,
    1680,
    1680 + 560,
    1680 + 104,
    1680 + 104 + 176,
    1089,
    1050,
    1050,
    1050 + 39,
    1050 + 3,
    1050 + 3 + 6,
},
{
    TIMING_ATTRIB_SIZE,
    0,
    1680,1050,
    7500,
    VerPOSITIVE + HorNEGATIVE,
    1870000,
    2272,
    1680,
    1680,
    1680 + 592,
    1680 + 120,
    1680 + 120 + 176,
    1099,
    1050,
    1050,
    1050 + 49,
    1050 + 3,
    1050 + 3 + 6,
},
{
    TIMING_ATTRIB_SIZE,
    0,
    1680,1050,
    8500,
    VerPOSITIVE + HorNEGATIVE,
    2147500,
    2288,
    1680,
    1680,
    1680 + 608,
    1680 + 128,
    1680 + 128 + 176,
    1105,
    1050,
    1050,
    1050 + 55,
    1050 + 3,
    1050 + 3 + 6,
},
/***** Reduced Blanking
{
    TIMING_ATTRIB_SIZE,
    0,
    1680,1050,
    12000,
    VerNEGATIVE + HorPOSITIVE,
    2455000,
    1840,
    1680,
    1680,
    1680 + 160,
    1680 + 48,
    1680 + 48 + 32,
    1112,
    1050,
    1050,
    1050 + 62,
    1050 + 3,
    1050 + 3 + 6,
},
*********************/
{
    TIMING_ATTRIB_SIZE,
    0,
    1792,1344,
    6000,
    VerPOSITIVE + HorNEGATIVE,
    2047500,
    2448,
    1792,
    1792,
    1792 + 656,
    1792 + 128,
    1792 + 128 + 200,
    1394,
    1344,
    1344,
    1344 + 50,
    1344 + 1,
    1344 + 1 + 3,
},
{
    TIMING_ATTRIB_SIZE,
    0,
    1792,1344,
    7500,
    VerPOSITIVE + HorNEGATIVE,
    2610000,
    2456,
    1792,
    1792,
    1792 + 664,
    1792 + 96,
    1792 + 96 + 216,
    1417,
    1344,
    1344,
    1344 + 73,
    1344 + 1,
    1344 + 1 + 3,
},
/*** Reduced Blanking
{
    TIMING_ATTRIB_SIZE,
    0,
    1792,1344,
    12000,
    VerNEGATIVE + HorPOSITIVE,
    3332500,
    1952,
    1792,
    1792,
    1792 + 160,
    1792 + 48,
    1792 + 48 + 32,
    1423,
    1344,
    1344,
    1344 + 79,
    1344 + 3,
    1344 + 3 + 4,
},
****************/
{
    TIMING_ATTRIB_SIZE,
    0,
    1856,1392,
    6000,
    VerPOSITIVE + HorNEGATIVE,
    2182500,
    2528,
    1856,
    1856,
    1856 + 672,
    1856 + 96,
    1856 + 96 + 224,
    1439,
    1392,
    1392,
    1392 + 47,
    1392 + 1,
    1392 + 1 + 3,
},
{
    TIMING_ATTRIB_SIZE,
    0,
    1856,1392,
    7500,
    VerPOSITIVE + HorNEGATIVE,
    2880000,
    2560,
    1856,
    1856,
    1856 + 704,
    1856 + 128,
    1856 + 128 + 224,
    1500,
    1392,
    1392,
    1392 + 108,
    1392 + 1,
    1392 + 1 + 3,
},
/**** Reduced Blanking
{
    TIMING_ATTRIB_SIZE,
    0,
    1856,1392,
    12000,
    VerNEGATIVE + HorPOSITIVE,
    3565000,
    2016,
    1856,
    1856,
    1856 + 160,
    1856 + 48,
    1856 + 48 + 32,
    1474,
    1392,
    1392,
    1392 + 82,
    1392 + 3,
    1392 + 3 + 4,
},
****************/
{
    TIMING_ATTRIB_SIZE,
    0,
    1920,1080,
    5000,
    VerPOSITIVE+HorPOSITIVE,
    1485000,
    2640,
    1920,
    1920,
    2640,
    1920+528,
    (1920+528+44) ,
    1125,
    1080,
    1080,
    1080+45,
    1084,
    1084+5,
},
{
    TIMING_ATTRIB_SIZE,
    0,
    1920,1080,
    6000,
    VerPOSITIVE+HorPOSITIVE,
    1485000,
    2200,
    1920,
    1920,
    2200 ,
    1920+88,
    (1920+88+44) ,
    1125,
    1080,
    1080,
    1080+45,
    1084,
    1084+5,
},
{
    TIMING_ATTRIB_SIZE,
    0,
    1920,1080,
    12000,
    VerPOSITIVE+HorNEGATIVE,
    3687000,
    332*8,
    240*8,
    240*8,
    (240+92) *8,
    259*8,
    (259+27) *8,
    1157,
    1080,
    1080,
    1080+77,
    1081,
    1081+3,
},
/*** Reduced Blanking
{
    TIMING_ATTRIB_SIZE,
    0,
    1920,1200,
    6000,
    VerNEGATIVE + HorPOSITIVE,
    1540000,
    2080,
    1920,
    1920,
    1920 + 160,
    1920 + 48,
    1920 + 48 + 32,
    1235,
    1200,
    1200,
    1200 + 35,
    1200 + 3,
    1200 + 3 + 6, 
},
**************/
{
    TIMING_ATTRIB_SIZE,
    0,
    1920,1200,
    6000,
    VerPOSITIVE+HorNEGATIVE,
    1932500,
    2592,
    1920,
    1920,
    1920 + 672,
    1920 + 136,
    1920 + 136 + 200,
    1245,
    1200,
    1200,
    1200 + 45,
    1200 + 3,
    1200 + 3 + 6,
},
{
    TIMING_ATTRIB_SIZE,
    0,
    1920,1200,
    7500,
    VerPOSITIVE+HorNEGATIVE,
    2452500,
    2608,
    1920,
    1920,
    1920 + 688,
    1920 + 136,
    1920 + 136 + 208,
    1255,
    1200,
    1200,
    1200 + 55,
    1200 + 3,
    1200 + 3 + 6,
},
{
    TIMING_ATTRIB_SIZE,
    0,
    1920,1200,
    8500,
    VerPOSITIVE+HorNEGATIVE,
    2812500,
    2624,
    1920,
    1920,
    1920 + 704,
    1920 + 144,
    1920 + 144 + 208,
    1262,
    1200,
    1200,
    1200 + 62,
    1200 + 3,
    1200 + 3 + 6,
},
/**** Reduced Blanking
{
    TIMING_ATTRIB_SIZE,
    0,
    1920,1200,
    12000,
    VerNEGATIVE + HorPOSITIVE,
    3170000,
    2080,
    1920,
    1920,
    1920 + 160,
    1920 + 48,
    1920 + 48 + 32,
    1271,
    1200,
    1200,
    1200 + 71,
    1200 + 3,
    1200 + 3 + 6,
},
**********************/
{
    TIMING_ATTRIB_SIZE,
    0,
    1920,1440,
    6000,
    VerPOSITIVE+HorNEGATIVE,
    2340000,
    2600,
    1920,
    1920,
    1920 + 680,
    1920 + 128,
    1920 + 128 + 208,
    1500,
    1440,
    1440,
    1440 + 60,
    1440 + 1,
    1440 + 1 + 3,
},
{
    TIMING_ATTRIB_SIZE,
    0,
    1920,1440,
    7500,
    VerPOSITIVE+HorNEGATIVE,
    2970000,
    2640,
    1920,
    1920,
    1920 + 720,
    1920 + 144,
    1920 + 144 + 224,
    1500,
    1440,
    1440,
    1440 + 60,
    1440 + 1,
    1440 + 1 + 3,
},
/*** Reduced Blanking
{
    TIMING_ATTRIB_SIZE,
    0,
    1920,1440,
    12000,
    VerNEGATIVE + HorPOSITIVE,
    3805000,
    2080,
    1920,
    1920,
    1920 + 160,
    1920 + 48,
    1920 + 48 + 32,
    1525,
    1440,
    1440,
    1440 + 85,
    1440 + 3,
    1440 + 3 + 4,
},
******************/
{
    TIMING_ATTRIB_SIZE,
    0,
    2048,1152,
    6000,
    VerPOSITIVE+HorPOSITIVE,
    1620000,
    2250,
    2048,
    2048,
    2048 + 202,
    2048 + 26,
    2048 + 26 + 80,
    1200,
    1152,
    1152,
    1152 + 48,
    1152 + 1,
    1152 + 1 + 3,
},
/*** Reduced Blanking
{
    TIMING_ATTRIB_SIZE,
    0,
    2560,1600,
    6000,
    VerNEGATIVE + HorPOSITIVE,
    2685000,
    2720,
    2560,
    2560,
    2560 + 160,
    2560 + 48,
    2560 + 48 + 32,
    1646,
    1600,
    1600,
    1600 + 46,
    1600 + 3,
    1600 + 3 + 6,
},
****************/
{
    TIMING_ATTRIB_SIZE,
    0,
    2560,1600,
    6000,
    VerPOSITIVE+HorNEGATIVE,
    3485000,
    3504,
    2560,
    2560,
    2560 + 944,
    2560 + 192,
    2560 + 192 + 280,
    1658,
    1600,
    1600,
    1600 + 58,
    1600 + 3,
    1600 + 3 + 6,
},
{
    TIMING_ATTRIB_SIZE,
    0,
    2560,1600,
    7500,
    VerPOSITIVE+HorNEGATIVE,
    4432500,
    3536,
    2560,
    2560,
    2560 + 976,
    2560 + 208,
    2560 + 208 + 280,
    1672,
    1600,
    1600,
    1600 + 72,
    1600 + 3,
    1600 + 3 + 6,
},
{
    TIMING_ATTRIB_SIZE,
    0,
    2560,1600,
    8500,
    VerPOSITIVE+HorNEGATIVE,
    5052500,
    3536,
    2560,
    2560,
    2560 + 976,
    2560 + 208,
    2560 + 208 + 280,
    1682,
    1600,
    1600,
    1600 + 82,
    1600 + 3,
    1600 + 3 + 6,
}
/** Reduced Blanking
{
    TIMING_ATTRIB_SIZE,
    0,
    2560,1600,
    12000,
    VerNEGATIVE + HorPOSITIVE,
    5527500,
    2720,
    2560,
    2560,
    2560 + 160,
    2560 + 48,
    2560 + 48 + 32,
    1694,
    1600,
    1600,
    1600 + 94,
    1600 + 3,
    1600 + 3 + 6,
},
***************/
};

CBIOS_TIMING_ATTRIB HDMIFormatTimingTbl[CBIOS_HDMIFORMATCOUNTS] =
{
//normal VIC begin
//if new VICs are added, make sure to update CBIOS_HDMI_NORMAL_VIC_COUNTS
//1
{
    TIMING_ATTRIB_SIZE,
    1,
    640,480,                               //XResolution,  YResolution
    6000,                                  //RefreshRate
    VerNEGATIVE+HorNEGATIVE,               //HVPolarity:   Hor/Ver Sync Polarity(MISC:11000000B)
    252000,                                 //DCLK 
    800,                                 //HorTotal:     SR66_0+SR60=Round(Value/8)-5
    640,                                  //HorDisEnd:    SR66_1+SR61=Round(Value/8)-1
    640,                                    //HorBStart:    SR66_2+SR62=Round(Value/8)
    800 ,                  //HorBEnd:      SR66[3]+SR65[7]+SR63[0-4]=Round(Value/8)&   0x003F*/
    656,                                    //HorSyncStart: SR66[4]+SR64=Round(Value/8)
    (656+96)   ,                    //HorSyncEnd:    SR66[5]+SR65[0-4] =Round(Value/8) &   0x001F*/
    525,                                 //VerTotal:     SR6E[0-3]+SR68=Value-2
    480,                                 //VerDisEnd:   SR6E[4-7]+SR69=Value-1
    480,                                   //VerBStart:    SR6F[0-3]+SR6A=Value
    525,                   //VerBEnd:      SR6B
    490,                                   //VerSyncStart: SR6F[4-7]+SR6C   
    490+2,                     //VerSyncEnd:   SR6B
},
//2
{
    TIMING_ATTRIB_SIZE,
    2,
    720,480,
    6000,
    VerNEGATIVE+HorNEGATIVE,
    270270,
    858,
    720,
    720,
    858 ,
    736,
    (736+62) ,
    525,
    480,
    480,
    525,
    489,
    489+6,
},
//3
{
    TIMING_ATTRIB_SIZE,
    3,
    720,480,
    6000,
    VerNEGATIVE+HorNEGATIVE,
    270270,
    858,
    720,
    720,
    858 ,
    736,
    (736+62) ,
    525,
    480,
    480,
    525,
    489,
    489+6,
},
//4
{
    TIMING_ATTRIB_SIZE,
    4,
    1280,720,
    6000,
    VerPOSITIVE+HorPOSITIVE,
    742500,
    1650,
    1280,
    1280,
    1650 ,
    1280+110,
    (1280+110+40) ,
    750,
    720,
    720,
    720+30,
    725,
    725+5,
},
//5
{
    TIMING_ATTRIB_SIZE,
    5,
    1920,1080,
    6000,
    VerPOSITIVE+HorPOSITIVE,
    742500,
    2200,
    1920,
    1920,
    2200 ,
    1920+88,
    (1920+88+44) ,
    1125,
    1080,
    1080,
    1080+22+23,
    1084,
    1084+10,
},
//6
{
    TIMING_ATTRIB_SIZE,
    6,
    720,480,
    6000,
    VerNEGATIVE+HorNEGATIVE,
    270270,
    858,
    720,
    720,
    858 ,
    720+38/2,
    720+38/2+124/2 ,
    525,
    480,
    480,
    525,
    489-1, //adjust to correct 480i timing 
    489-1+6,
},
//7
{
    TIMING_ATTRIB_SIZE,
    7,
    720,480,
    6000,
    VerNEGATIVE+HorNEGATIVE,
    270270,
    858,
    720,
    720,
    858,
    720+38/2,
    720+38/2+124/2 ,
    525,
    480,
    480,
    525,
    489-1, //adjust to correct 480i timing 
    489-1+6,
},
//8
{
    TIMING_ATTRIB_SIZE,
    8,
    720,240,
    6000,
    VerNEGATIVE+HorNEGATIVE,
    270270,
    858,
    720,
    858,
    858,
    720+38/2,
    720+38/2+124/2,
    262,
    240,
    240,
    240+22,
    244,
    244+3,
},
//9
{
    TIMING_ATTRIB_SIZE,
    9,
    720,240,
    6000,
    VerNEGATIVE+HorNEGATIVE,
    270270,
    858,
    720,
    858,
    858,
    720+38/2,
    720+38/2+124/2,
    262,
    240,
    240,
    240+22,
    244,
    244+3,
},
//10
{
    TIMING_ATTRIB_SIZE,
    10,
    2880,480,
    6000,
    VerNEGATIVE+HorNEGATIVE,
    540540,
    3432,
    2880,
    2880,
    3432 ,
    2880+76,
    (2880+76+248) ,
    525,
    480,
    480,
    525,
    488,
    488+6,
},
//11
{
    TIMING_ATTRIB_SIZE,
    11,
    2880,480,
    6000,
    VerNEGATIVE+HorNEGATIVE,
    540540,
    3432,
    2880,
    2880,
    3432,
    2880+76,
    (2880+76+248) ,
    525,
    480,
    480,
    525,
    488,
    488+6,
},
//12
{
    TIMING_ATTRIB_SIZE,
    12,
    2880,240,
    6000,
    VerNEGATIVE+HorNEGATIVE,
    540540,
    3432,
    2880,
    2880,
    3432,
    2880+76,
    (2880+76+248) ,
    262,
    240,
    240,
    240+22,
    244,
    244+3,
},
//13
{
    TIMING_ATTRIB_SIZE,
    13,
    2880,240,
    6000,
    VerNEGATIVE+HorNEGATIVE,
    540540,
    3432,
    2880,
    2880,
    3432 ,
    2880+76,
    (2880+76+248) ,
    263,
    240,
    240,
    240+23,
    245,
    245+3,
},
//14
{
    TIMING_ATTRIB_SIZE,
    14,
    1440,480,
    6000,
    VerNEGATIVE+HorNEGATIVE,
    540540,
    1716,
    1440,
    1440,
    1716,
    1472,
    (1472+124) ,
    525,
    480,
    480,
    480+45,
    490,
    490+6,
},
//15
{
    TIMING_ATTRIB_SIZE,
    15,
    1440,480,
    6000,
    VerNEGATIVE+HorNEGATIVE,
    540540,
    1716,
    1440,
    1440,
    1716,
    1472,
    (1472+124) ,
    525,
    480,
    480,
    480+45,
    490,
    490+6,
},
//16
{
    TIMING_ATTRIB_SIZE,
    16,
    1920,1080,
    6000,
    VerPOSITIVE+HorPOSITIVE,
    1485000,
    2200,
    1920,
    1920,
    2200 ,
    1920+88,
    (1920+88+44) ,
    1125,
    1080,
    1080,
    1080+45,
    1084,
    1084+5,
},
//17
{
    TIMING_ATTRIB_SIZE,
    17,
    720,576,
    5000,
    VerNEGATIVE+HorNEGATIVE,
    270000,
    864,
    720,
    720,
    864,
    732,
    (732+64) ,
    625,
    576,
    576,
    625,
    581,
    581+5,
},
//18
{
    TIMING_ATTRIB_SIZE,
    18,
    720,576,
    5000,
    VerNEGATIVE+HorNEGATIVE,
    270000,
    864,
    720,
    720,
    864 ,
    732,
    (732+64) ,
    625,
    576,
    576,
    625,
    581,
    581+5,
},
//19
{
    TIMING_ATTRIB_SIZE,
    19,
    1280,720,
    5000,
    VerPOSITIVE+HorPOSITIVE,
    742500,
    1980,
    1280,
    1280,
    1980,
    1280+440,
    (1280+440+40) ,
    750,
    720,
    720,
    720+30,
    725,
    725+5,
},
//20
{
    TIMING_ATTRIB_SIZE,
    20,
    1920,1080,
    5000,
    VerPOSITIVE+HorPOSITIVE,
    742500,
    2640,
    1920,
    1920,
    2640,
    1920+528,
    (1920+528+44) ,
    1125,
    1080,
    1080,
    1080+22+23,
    1084,
    1084+10,
},
//21
{
    TIMING_ATTRIB_SIZE,
    21,
    720,576,
    5000,
    VerNEGATIVE+HorNEGATIVE,
    270000,
    864,
    720,
    720,
    864,
    732,
    (732+63) ,
    625,
    576,
    576,
    625,
    580,   //adjust to correct 576i timing 
    580+6,
},
//22
{
    TIMING_ATTRIB_SIZE,
    22,
    720,576,
    5000,
    VerNEGATIVE+HorNEGATIVE,
    270000,
    864,
    720,
    720,
    864 ,
    732,
    (732+63) ,
    625,
    576,
    576,
    625,
    580,//adjust to correct 576i timing 
    580+6,
},
//23
{
    TIMING_ATTRIB_SIZE,
    23,
    720,288,
    5000,
    VerNEGATIVE+HorNEGATIVE,
    270000,
    864,
    720,
    720,
    864,
    732,
    (732+63) ,
    312,
    288,
    288,
    312,
    290,
    290+3,
},
//24
{
    TIMING_ATTRIB_SIZE,
    24,
    720,288,
    5000,
    VerNEGATIVE+HorNEGATIVE,
    270000,
    864,
    720,
    720,
    864,
    732,
    (732+63) ,
    312,
    288,
    288,
    312,
    290,
    290+3,
},
//25
{
    TIMING_ATTRIB_SIZE,
    25,
    2880,576,
    5000,
    VerNEGATIVE+HorNEGATIVE,
    540000,
    3456,
    2880,
    2880,
    3456,
    2880+48,
    (2880+48+252) ,
    625,
    576,
    576,
    625,
    580,
    580+6,
},
//26
{
    TIMING_ATTRIB_SIZE,
    26,
    2880,576,
    5000,
    VerNEGATIVE+HorNEGATIVE,
    540000,
    3456,
    2880,
    2880,
    3456,
    2880+48,
    (2880+48+252) ,
    625,
    576,
    576,
    625,
    580,
    580+6,
},
//27
{
    TIMING_ATTRIB_SIZE,
    27,
    2880,288,
    5000,
    VerNEGATIVE+HorNEGATIVE,
    540000,
    3456,
    2880,
    2880,
    3456,
    2880+48,
    (2880+48+252) ,
    312,
    288,
    288,
    312,
    290,
    290+3,
},
//28
{
    TIMING_ATTRIB_SIZE,
    28,
    2880,288,
    5000,
    VerNEGATIVE+HorNEGATIVE,
    540000,
    3456,
    2880,
    2880,
    3456,
    2880+48,
    (2880+48+252) ,
    313,
    288,
    288,
    313,
    291,
    291+3,
},
//29
{
    TIMING_ATTRIB_SIZE,
    29,
    1440,576,
    5000,
    VerNEGATIVE+HorNEGATIVE,
    540000,
    1728,
    1440,
    1440,
    1728,
    1440+24,
    (1440+24+128) ,
    625,
    576,
    576,
    625,
    581,
    581+5,
},
//30
{
    TIMING_ATTRIB_SIZE,
    30,
    1440,576,
    5000,
    VerNEGATIVE+HorNEGATIVE,
    540000,
    1728,
    1440,
    1440,
    1728,
    1440+24,
    (1440+24+128) ,
    625,
    576,
    576,
    625,
    581,
    581+ 5,
},
//31
{
    TIMING_ATTRIB_SIZE,
    31,
    1920,1080,
    5000,
    VerPOSITIVE+HorPOSITIVE,
    1485000,
    2640,
    1920,
    1920,
    2640,
    1920+528,
    (1920+528+44) ,
    1125,
    1080,
    1080,
    1080+45,
    1084,
    1084+5,
},
//32
{
    TIMING_ATTRIB_SIZE,
    32,
    1920,1080,
    2400,
    VerPOSITIVE+HorPOSITIVE,
    742500,
    2750,
    1920,
    1920,
    2750,
    1920+638,
    (1920+638+44) ,
    1125,
    1080,
    1080,
    1080+45,
    1084,
    1084+5,
},
//33
{
    TIMING_ATTRIB_SIZE,
    33,
    1920,1080,
    2500,
    VerPOSITIVE+HorPOSITIVE,
    742500,
    2640,
    1920,
    1920,
    2640,
    1920+528,
    (1920+528+44) ,
    1125,
    1080,
    1080,
    1080+45,
    1084,
    1084+5,
},
//34
{
    TIMING_ATTRIB_SIZE,
    34,
    1920,1080,
    3000,
    VerPOSITIVE+HorPOSITIVE,
    742500,
    2200,//2640-5*8,
    1920,
    1920,
    2200,
    1920+88,
    (1920+88+44),//(1920+528+44) ,
    1125,
    1080,
    1080,
    1080+45,
    1084,
    1084+5,
},
//35 2880x480p@60Hz 4:3
{
    TIMING_ATTRIB_SIZE,
    35,
    2880, 480,
    6000,
    VerNEGATIVE+HorNEGATIVE,
    1081080,
    3432,
    2880,
    2880,
    3432,
    2880+64,
    2880+64+248,
    525,
    480,
    480, 
    525,
    480+9,
    480+9+6,
},
//36 2880x480p@60Hz 16:9
{
    TIMING_ATTRIB_SIZE,
    36,
    2880, 480,
    6000,
    VerNEGATIVE+HorNEGATIVE,
    1081080,
    3432,
    2880,
    2880,
    3432,
    2880+64,
    2880+64+248,
    525,
    480,
    480, 
    525,
    480+9,
    480+9+6,
},
//37 2880x576p@50Hz 4:3
{
    TIMING_ATTRIB_SIZE,
    37,
    2880, 576,
    5000,
    VerNEGATIVE+HorNEGATIVE,
    1080000,
    3456,
    2880,
    2880,
    3456,
    2880+48,
    2880+48+256,
    625,
    576,
    576, 
    625,
    576+5,
    576+5+5,
},
//38 2880x576p@50Hz 16:9
{
    TIMING_ATTRIB_SIZE,
    38,
    2880, 576,
    5000,
    VerNEGATIVE+HorNEGATIVE,
    1080000,
    3456,
    2880,
    2880,
    3456,
    2880+48,
    2880+48+256,
    625,
    576,
    576, 
    625,
    576+5,
    576+5+5,
},
//39 1920x1080i@50Hz 16:9
{
    TIMING_ATTRIB_SIZE,
    39,
    1920, 1080,
    5000,
    VerPOSITIVE+HorPOSITIVE,
    720000,
    2304,
    1920,
    1920,
    2304,
    1920+32,
    1920+32+168,
    1250,
    1080,
    1080, 
    1250,
    1080+46,
    1080+46+10,
},
//40 1920x1080i@100Hz 16:9
{
    TIMING_ATTRIB_SIZE,
    40,
    1920, 1080,
    10000,
    VerPOSITIVE+HorPOSITIVE,
    1485000,
    2640,
    1920,
    1920,
    2640,
    1920+528,
    1920+528+44,
    1125,
    1080,
    1080, 
    1125,
    1080+4,
    1080+4+10,
},
//41 1280x720p@100Hz 16:9
{
    TIMING_ATTRIB_SIZE,
    41,
    1280, 720,
    10000,
    VerPOSITIVE+HorPOSITIVE,
    1485000,
    1980,
    1280,
    1280,
    1980,
    1280+440,
    1280+440+40,
    750,
    720,
    720, 
    750,
    720+5,
    720+5+5,
},
//42 720*576p@100Hz 4:3 
{
    TIMING_ATTRIB_SIZE,
    42,
    720, 576,
    10000,
    VerNEGATIVE+HorNEGATIVE,
    540000,
    864,
    720,
    720,
    864,
    720+12,
    720+12+64,
    625,
    576,
    576, 
    625,
    576+5,
    576+5+5,
},

//43 720*576p@100Hz 16:9
{
    TIMING_ATTRIB_SIZE,
    43,
    720, 576,
    10000,
    VerNEGATIVE+HorNEGATIVE,
    540000,
    864,
    720,
    720,
    864,
    720+12,
    720+12+64,
    625,
    576,
    576, 
    625,
    576+5,
    576+5+5,
},
//44 720(1440)*576i@100Hz 4:3 
{
    TIMING_ATTRIB_SIZE,
    44,
    720, 576,
    10000,
    VerNEGATIVE+HorNEGATIVE,
    540000,
    864,
    720,
    720,
    864,
    720+12,
    720+12+63,
    625,
    576,
    576, 
    625,
    576+5-1,
    576+5+5,
},
//45 720(1440)*576i@100Hz 16:9
{
    TIMING_ATTRIB_SIZE,
    45,
    720, 576,
    10000,
    VerNEGATIVE+HorNEGATIVE,
    540000,
    864,
    720,
    720,
    864,
    720+12,
    720+12+63,
    625,
    576,
    576, 
    625,
    576+5-1,
    576+5+5,
},
//46 1920x1080i@120Hz 16:9
{
    TIMING_ATTRIB_SIZE,
    46,
    1920, 1080,
    10000,
    VerPOSITIVE+HorPOSITIVE,
    1485000,
    2200,
    1920,
    1920,
    2200,
    1920+88,
    1920+88+44,
    1125,
    1080,
    1080, 
    1125,
    1080+4,
    1080+4+10,
},
//47 1280x720p@120Hz 16:9
{
    TIMING_ATTRIB_SIZE,
    47,
    1280,720,
    12000,
    VerPOSITIVE+HorPOSITIVE,
    1485000,
    1650,
    1280,
    1280,
    1650 ,
    1280+110,
    (1280+110+40) ,
    750,
    720,
    720,
    720+30,
    725,
    725+5,
},
//48 720x480p@120Hz 4:3
{
    TIMING_ATTRIB_SIZE,
    48,
    720,480,
    12000,
    VerNEGATIVE+HorNEGATIVE,
    540540,
    858,
    720,
    720,
    858 ,
    736,
    (736+62) ,
    525,
    480,
    480,
    525,
    489,
    489+6,
},
//49 720x480p@120Hz 16:9
{
    TIMING_ATTRIB_SIZE,
    49,
    720,480,
    12000,
    VerNEGATIVE+HorNEGATIVE,
    540540,
    858,
    720,
    720,
    858 ,
    736,
    (736+62) ,
    525,
    480,
    480,
    525,
    489,
    489+6,
},
//50 720(1440)x480i@120Hz 4:3
{
    TIMING_ATTRIB_SIZE,
    50,
    720,480,
    12000,    
    VerNEGATIVE+HorNEGATIVE,
    540540,
    858,
    720,
    720,
    858 ,
    720+38/2,
    720+38/2+124/2,
    525,
    480,
    480,
    525,
    489-1,
    489-1+6,
},
//51 720(1440)x480i@120Hz 16:9
{
    TIMING_ATTRIB_SIZE,
    51,
    720,480,
    12000,    
    VerNEGATIVE+HorNEGATIVE,
    540540,
    858,
    720,
    720,
    858 ,
    720+38/2,
    720+38/2+124/2,
    525,
    480,
    480,
    525,
    489-1,
    489-1+6,
},
//52 720x576p@200Hz, 4:3
{
    TIMING_ATTRIB_SIZE,
    52,
    720,576,
    20000,
    VerNEGATIVE+HorNEGATIVE,
    1080000,
    864,
    720,
    720,
    864,
    732,
    (732+64) ,
    625,
    576,
    576,
    625,
    581,
    581+5,
},
//53 720x576p@200Hz, 16:9
{
    TIMING_ATTRIB_SIZE,
    53,
    720,576,
    20000,
    VerNEGATIVE+HorNEGATIVE,
    1080000,
    864,
    720,
    720,
    864,
    732,
    (732+64) ,
    625,
    576,
    576,
    625,
    581,
    581+5,
},
//54 720(1440)x576i@200Hz, 4:3
{
    TIMING_ATTRIB_SIZE,
    54,
    720,576,
    20000,
    VerNEGATIVE+HorNEGATIVE,
    1080000,
    864,
    720,
    720,
    864,
    732,
    (732+63) ,
    625,
    576,
    576,
    625,
    581-1,
    581+5,
},
//55 720(1440)x576i@200Hz, 16:9
{
    TIMING_ATTRIB_SIZE,
    55,
    720,576,
    20000,
    VerNEGATIVE+HorNEGATIVE,
    1080000,
    864,
    720,
    720,
    864,
    732,
    (732+63) ,
    625,
    576,
    576,
    625,
    581-1,
    581+5,
},
//56 720x480p@240Hz, 4:3
{
    TIMING_ATTRIB_SIZE,
    56,
    720,480,
    24000,
    VerNEGATIVE+HorNEGATIVE,
    1081080,
    858,
    720,
    720,
    858,
    736,
    (736+62) ,
    525,
    480,
    480,
    525,
    480+9,
    480+9+6,
},
//57 720x480p@240Hz, 16:9
{
    TIMING_ATTRIB_SIZE,
    57,
    720,480,
    24000,
    VerNEGATIVE+HorNEGATIVE,
    1081080,
    858,
    720,
    720,
    858,
    736,
    (736+62) ,
    525,
    480,
    480,
    525,
    480+9,
    480+9+6,
},
//58 720(1440)x480i@240Hz, 4:3
{
    TIMING_ATTRIB_SIZE,
    58,
    720,480,
    24000,
    VerNEGATIVE+HorNEGATIVE,
    1081080,
    858,
    720,
    720,
    858,
    720+19,
    (720+19+62) ,
    525,
    480,
    480,
    525,
    480+9-1,
    480+9-1+6,
},
//59 720(1440)x480i@240Hz, 16:9
{
    TIMING_ATTRIB_SIZE,
    59,
    720,480,
    24000,
    VerNEGATIVE+HorNEGATIVE,
    1081080,
    858,
    720,
    720,
    858,
    720+19,
    (720+19+62) ,
    525,
    480,
    480,
    525,
    480+9-1,
    480+9-1+6,
},
//60 1280x720p@24Hz, 16:9
{
    TIMING_ATTRIB_SIZE,
    60,
    1280,720,
    2400, 
    VerPOSITIVE+HorPOSITIVE,
    594000,
    3300,
    1280,
    1280,
    3300,
    1280+1760,
    1280+1760+40,
    750,
    720,
    720,
    750,
    725,
    725+5,
},
//61 1280x720p@25Hz, 16:9
{
    TIMING_ATTRIB_SIZE,
    61,
    1280,720,
    2500, 
    VerPOSITIVE+HorPOSITIVE,
    742500,
    3960,
    1280,
    1280,
    3960,
    1280+2420,
    1280+2420+40,
    750,
    720,
    720,
    750,
    725,
    725+5,
},
//62 1280x720p@30Hz, 16:9
{
    TIMING_ATTRIB_SIZE,
    62,
    1280,720,
    3000, 
    VerPOSITIVE+HorPOSITIVE,
    742500,
    3300,
    1280,
    1280,
    3300,
    1280+1760,
    1280+1760+40,
    750,
    720,
    720,
    750,
    725,
    725+5,
},
//63 1920x1080p@120Hz, 16:9
{
    TIMING_ATTRIB_SIZE,
    63,
    1920,1080,
    12000, 
    VerPOSITIVE+HorPOSITIVE,
    2970000,
    2200,
    1920,
    1920,
    2200,
    1920+88,
    1920+88+44,
    1125,
    1080,
    1080,
    1125,
    1080+4,
    1084+5,
},
//64 1920x1080p@100Hz, 16:9
{
    TIMING_ATTRIB_SIZE,
    64,
    1920,1080,
    10000, 
    VerPOSITIVE+HorPOSITIVE,
    2970000,
    2640,
    1920,
    1920,
    2640,
    1920+528,
    1920+528+44,
    1125,
    1080,
    1080,
    1125,
    1080+4,
    1084+5,
},

// CEA-861-F
//65 1280x720p@24Hz, 64:27
{
    TIMING_ATTRIB_SIZE,
    65,
    1280,720,
    2400, 
    VerPOSITIVE+HorPOSITIVE,
    594000,
    3300,
    1280,
    1280,
    3300,
    1280+1760,
    1280+1760+40,
    750,
    720,
    720,
    750,
    725,
    725+5,
},
//66 1280x720p@25Hz, 64:27
{
    TIMING_ATTRIB_SIZE,
    66,
    1280,720,
    2500, 
    VerPOSITIVE+HorPOSITIVE,
    742500,
    3960,
    1280,
    1280,
    3960,
    1280+2420,
    1280+2420+40,
    750,
    720,
    720,
    750,
    725,
    725+5,
},
//67 1280x720p@30Hz, 64:27
{
    TIMING_ATTRIB_SIZE,
    67,
    1280,720,
    3000, 
    VerPOSITIVE+HorPOSITIVE,
    742500,
    3300,
    1280,
    1280,
    3300,
    1280+1760,
    1280+1760+40,
    750,
    720,
    720,
    750,
    725,
    725+5,
},
//68 1280x720p@50Hz, 64:27
{
    TIMING_ATTRIB_SIZE,
    68,
    1280,720,
    5000,
    VerPOSITIVE+HorPOSITIVE,
    742500,
    1980,
    1280,
    1280,
    1980,
    1280+440,
    (1280+440+40) ,
    750,
    720,
    720,
    720+30,
    725,
    725+5,
},
//69 1280x720p@60Hz, 64:27
{
    TIMING_ATTRIB_SIZE,
    69,
    1280,720,
    6000,
    VerPOSITIVE+HorPOSITIVE,
    742500,
    1650,
    1280,
    1280,
    1650 ,
    1280+110,
    (1280+110+40) ,
    750,
    720,
    720,
    720+30,
    725,
    725+5,
},
//70 1280x720p@100Hz, 64:27
{
    TIMING_ATTRIB_SIZE,
    70,
    1280, 720,
    10000,
    VerPOSITIVE+HorPOSITIVE,
    1485000,
    1980,
    1280,
    1280,
    1980,
    1280+440,
    1280+440+40,
    750,
    720,
    720, 
    750,
    720+5,
    720+5+5,
},
//71 1280x720p@120Hz, 64:27
{
    TIMING_ATTRIB_SIZE,
    71,
    1280,720,
    12000,
    VerPOSITIVE+HorPOSITIVE,
    1485000,
    1650,
    1280,
    1280,
    1650 ,
    1280+110,
    (1280+110+40) ,
    750,
    720,
    720,
    720+30,
    725,
    725+5,
},
//72 1920x1080p@24Hz, 64:27
{
    TIMING_ATTRIB_SIZE,
    72,
    1920,1080,
    2400,
    VerPOSITIVE+HorPOSITIVE,
    742500,
    2750,
    1920,
    1920,
    2750,
    1920+638,
    (1920+638+44) ,
    1125,
    1080,
    1080,
    1080+45,
    1084,
    1084+5,
},
//73 1920x1080p@25Hz, 64:27
{
    TIMING_ATTRIB_SIZE,
    73,
    1920,1080,
    2500,
    VerPOSITIVE+HorPOSITIVE,
    742500,
    2640,
    1920,
    1920,
    2640,
    1920+528,
    (1920+528+44) ,
    1125,
    1080,
    1080,
    1080+45,
    1084,
    1084+5,
},
//74 1920x1080p@30Hz, 64:27
{
    TIMING_ATTRIB_SIZE,
    74,
    1920,1080,
    3000,
    VerPOSITIVE+HorPOSITIVE,
    741760,
    2200,//2640-5*8,
    1920,
    1920,
    2200,
    1920+88,
    (1920+88+44),//(1920+528+44) ,
    1125,
    1080,
    1080,
    1080+45,
    1084,
    1084+5,
},
//75 1920x1080p@50Hz, 64:27
{
    TIMING_ATTRIB_SIZE,
    75,
    1920,1080,
    5000,
    VerPOSITIVE+HorPOSITIVE,
    1485000,
    2640,
    1920,
    1920,
    2640,
    1920+528,
    (1920+528+44) ,
    1125,
    1080,
    1080,
    1080+45,
    1084,
    1084+5,
},
//76 1920x1080p@60Hz, 64:27
{
    TIMING_ATTRIB_SIZE,
    76,
    1920,1080,
    6000,
    VerPOSITIVE+HorPOSITIVE,
    1485000,
    2200,
    1920,
    1920,
    2200 ,
    1920+88,
    (1920+88+44) ,
    1125,
    1080,
    1080,
    1080+45,
    1084,
    1084+5,
},
//77 1920x1080p@100Hz, 64:27
{
    TIMING_ATTRIB_SIZE,
    77,
    1920,1080,
    12000, 
    VerPOSITIVE+HorPOSITIVE,
    2970000,
    2640,
    1920,
    1920,
    2640,
    1920+528,
    1920+528+44,
    1125,
    1080,
    1080,
    1125,
    1080+4,
    1084+5,
},
//78 1920x1080p@120Hz, 64:27
{
    TIMING_ATTRIB_SIZE,
    78,
    1920,1080,
    12000, 
    VerPOSITIVE+HorPOSITIVE,
    2970000,
    2200,
    1920,
    1920,
    2200,
    1920+88,
    1920+88+44,
    1125,
    1080,
    1080,
    1125,
    1080+4,
    1084+5,
},
//79 1680x720p@24Hz, 64:27
{
    TIMING_ATTRIB_SIZE,
    79,
    1680,720,
    2400, 
    VerPOSITIVE+HorPOSITIVE,
    594000,
    3300,
    1680,
    1680,
    3300,
    1680+1360,
    1680+1360+40,
    750,
    720,
    720,
    750,
    720+5,
    720+5+5,
},
//80 1680x720p@25Hz, 64:27
{
    TIMING_ATTRIB_SIZE,
    80,
    1680,720,
    2500, 
    VerPOSITIVE+HorPOSITIVE,
    594000,
    3168,
    1680,
    1680,
    3168,
    1680+1228,
    1680+1228+40,
    750,
    720,
    720,
    750,
    720+5,
    720+5+5,
},
//81 1680x720p@30Hz, 64:27
{
    TIMING_ATTRIB_SIZE,
    81,
    1680,720,
    3000, 
    VerPOSITIVE+HorPOSITIVE,
    594000,
    2640,
    1680,
    1680,
    2640,
    1680+700,
    1680+700+40,
    750,
    720,
    720,
    750,
    720+5,
    720+5+5,
},
//82 1680x720p@50Hz, 64:27
{
    TIMING_ATTRIB_SIZE,
    82,
    1680,720,
    5000, 
    VerPOSITIVE+HorPOSITIVE,
    825000,
    2200,
    1680,
    1680,
    2200,
    1680+260,
    1680+260+40,
    750,
    720,
    720,
    750,
    720+5,
    720+5+5,
},
//83 1680x720p@60Hz, 64:27
{
    TIMING_ATTRIB_SIZE,
    83,
    1680,720,
    6000, 
    VerPOSITIVE+HorPOSITIVE,
    990000,
    2200,
    1680,
    1680,
    2200,
    1680+260,
    1680+260+40,
    750,
    720,
    720,
    750,
    720+5,
    720+5+5,
},
//84 1680x720p@100Hz, 64:27
{
    TIMING_ATTRIB_SIZE,
    84,
    1680,720,
    10000, 
    VerPOSITIVE+HorPOSITIVE,
    1650000,
    2000,
    1680,
    1680,
    2000,
    1680+60,
    1680+60+40,
    825,
    720,
    720,
    825,
    720+5,
    720+5+5,
},
//85 1680x720p@120Hz, 64:27
{
    TIMING_ATTRIB_SIZE,
    85,
    1680,720,
    12000, 
    VerPOSITIVE+HorPOSITIVE,
    1980000,
    2000,
    1680,
    1680,
    2000,
    1680+60,
    1680+60+40,
    825,
    720,
    720,
    825,
    720+5,
    720+5+5,
},
//86 2560x1080p@24Hz, 64:27
{
    TIMING_ATTRIB_SIZE,
    86,
    2560,1080,
    2400, 
    VerPOSITIVE+HorPOSITIVE,
    990000,
    3750,
    2560,
    2560,
    3750,
    2560+998,
    2560+998+44,
    1100,
    1080,
    1080,
    1100,
    1080+4,
    1080+4+5,
},
//87 2560x1080p@25Hz, 64:27
{
    TIMING_ATTRIB_SIZE,
    87,
    2560,1080,
    2500, 
    VerPOSITIVE+HorPOSITIVE,
    900000,
    3200,
    2560,
    2560,
    3200,
    2560+448,
    2560+448+44,
    1125,
    1080,
    1080,
    1125,
    1080+4,
    1080+4+5,
},
//88 2560x1080p@30Hz, 64:27
{
    TIMING_ATTRIB_SIZE,
    88,
    2560,1080,
    3000, 
    VerPOSITIVE+HorPOSITIVE,
    1188000,
    3520,
    2560,
    2560,
    3520,
    2560+768,
    2560+768+44,
    1125,
    1080,
    1080,
    1125,
    1080+4,
    1080+4+5,
},
//89 2560x1080p@50Hz, 64:27
{
    TIMING_ATTRIB_SIZE,
    89,
    2560,1080,
    5000, 
    VerPOSITIVE+HorPOSITIVE,
    1856250,
    3300,
    2560,
    2560,
    3300,
    2560+548,
    2560+548+44,
    1125,
    1080,
    1080,
    1125,
    1080+4,
    1080+4+5,
},
//90 2560x1080p@60Hz, 64:27
{
    TIMING_ATTRIB_SIZE,
    90,
    2560,1080,
    6000, 
    VerPOSITIVE+HorPOSITIVE,
    1980000,
    3000,
    2560,
    2560,
    3000,
    2560+248,
    2560+248+44,
    1100,
    1080,
    1080,
    1100,
    1080+4,
    1080+4+5,
},
//91 2560x1080p@100Hz, 64:27
{
    TIMING_ATTRIB_SIZE,
    91,
    2560,1080,
    10000, 
    VerPOSITIVE+HorPOSITIVE,
    3712500,
    2970,
    2560,
    2560,
    2970,
    2560+218,
    2560+218+44,
    1250,
    1080,
    1080,
    1250,
    1080+4,
    1080+4+5,
},
//92 2560x1080p@120Hz, 64:27
{
    TIMING_ATTRIB_SIZE,
    92,
    2560,1080,
    12000, 
    VerPOSITIVE+HorPOSITIVE,
    4950000,
    3300,
    2560,
    2560,
    3300,
    2560+548,
    2560+548+44,
    1250,
    1080,
    1080,
    1250,
    1080+4,
    1080+4+5,
},
//93 3840x2160p@24Hz, 16:9
{
    TIMING_ATTRIB_SIZE,
    93,
    3840, 2160,
    2400,
    VerPOSITIVE+HorPOSITIVE,
    2970000,
    3840+1660,
    3840,
    3840,
    3840+1660,
    3840+1276,
    3840+1276+88,
    2160+90,
    2160,
    2160,
    2160+90,
    2160+8,
    2160+8+10
},
//94 3840x2160p@25Hz, 16:9
{
    TIMING_ATTRIB_SIZE,
    94,
    3840, 2160,
    2500,
    VerPOSITIVE+HorPOSITIVE,
    2970000,
    3840+1440,
    3840,
    3840,
    3840+1440,
    3840+1056,
    3840+1056+88,
    2160+90,
    2160,
    2160,
    2160+90,
    2160+8,
    2160+8+10
},
//95 3840x2160p@30Hz, 16:9
{
    TIMING_ATTRIB_SIZE,
    95,
    3840, 2160,
    3000,
    VerPOSITIVE+HorPOSITIVE,
    2970000,
    3840+560,
    3840,
    3840,
    3840+560,
    3840+176,
    3840+176+88,
    2160+90,
    2160,
    2160,
    2160+90,
    2160+8,
    2160+8+10
},
//96 3840x2160p@50Hz, 16:9
{
    TIMING_ATTRIB_SIZE,
    96,
    3840, 2160,
    5000,
    VerPOSITIVE+HorPOSITIVE,
    5940000,
    3840+1440,
    3840,
    3840,
    3840+1440,
    3840+1056,
    3840+1056+88,
    2160+90,
    2160,
    2160,
    2160+90,
    2160+8,
    2160+8+10
},
//97 3840x2160p@60Hz, 16:9
{
    TIMING_ATTRIB_SIZE,
    97,
    3840, 2160,
    6000,
    VerPOSITIVE+HorPOSITIVE,
    5940000,
    3840+560,
    3840,
    3840,
    3840+560,
    3840+176,
    3840+176+88,
    2160+90,
    2160,
    2160,
    2160+90,
    2160+8,
    2160+8+10
},
//98 4096x2160p@24Hz, 256:135
{
    TIMING_ATTRIB_SIZE,
    98,
    4096, 2160,
    2400,
    VerPOSITIVE+HorPOSITIVE,
    2970000,
    4096+1404,
    4096,
    4096,
    4096+1404,
    4096+1020,
    4096+1020+88,
    2160+90,
    2160,
    2160,
    2160+90,
    2160+8,
    2160+8+10
},
//99 4096x2160p@25Hz, 256:135
{
    TIMING_ATTRIB_SIZE,
    99,
    4096, 2160,
    2500,
    VerPOSITIVE+HorPOSITIVE,
    2970000,
    5280,
    4096,
    4096,
    5280,
    4096+968,
    4096+968+88,
    2250,
    2160,
    2160,
    2250,
    2160+8,
    2160+8+10
},
//100 4096x2160p@30Hz, 256:135
{
    TIMING_ATTRIB_SIZE,
    100,
    4096, 2160,
    3000,
    VerPOSITIVE+HorPOSITIVE,
    2970000,
    4400,
    4096,
    4096,
    4400,
    4096+88,
    4096+88+88,
    2250,
    2160,
    2160,
    2250,
    2160+8,
    2160+8+10
},
//101 4096x2160p@50Hz, 256:135
{
    TIMING_ATTRIB_SIZE,
    101,
    4096, 2160,
    5000,
    VerPOSITIVE+HorPOSITIVE,
    5940000,
    5280,
    4096,
    4096,
    5280,
    4096+968,
    4096+968+88,
    2250,
    2160,
    2160,
    2250,
    2160+8,
    2160+8+10
},
//102 4096x2160p@60Hz, 256:135
{
    TIMING_ATTRIB_SIZE,
    102,
    4096, 2160,
    6000,
    VerPOSITIVE+HorPOSITIVE,
    5940000,
    4400,
    4096,
    4096,
    4400,
    4096+88,
    4096+88+88,
    2250,
    2160,
    2160,
    2250,
    2160+8,
    2160+8+10
},
//103 3840x2160p@24Hz, 64:27
{
    TIMING_ATTRIB_SIZE,
    103,
    3840, 2160,
    2400,
    VerPOSITIVE+HorPOSITIVE,
    2970000,
    3840+1660,
    3840,
    3840,
    3840+1660,
    3840+1276,
    3840+1276+88,
    2160+90,
    2160,
    2160,
    2160+90,
    2160+8,
    2160+8+10
},
//104 3840x2160p@25Hz, 64:27
{
    TIMING_ATTRIB_SIZE,
    104,
    3840, 2160,
    2500,
    VerPOSITIVE+HorPOSITIVE,
    2970000,
    3840+1440,
    3840,
    3840,
    3840+1440,
    3840+1056,
    3840+1056+88,
    2160+90,
    2160,
    2160,
    2160+90,
    2160+8,
    2160+8+10
},
//105 3840x2160p@30Hz, 64:27
{
    TIMING_ATTRIB_SIZE,
    105,
    3840, 2160,
    3000,
    VerPOSITIVE+HorPOSITIVE,
    2970000,
    3840+560,
    3840,
    3840,
    3840+560,
    3840+176,
    3840+176+88,
    2160+90,
    2160,
    2160,
    2160+90,
    2160+8,
    2160+8+10
},
//106 3840x2160p@50Hz, 64:27
{
    TIMING_ATTRIB_SIZE,
    106,
    3840, 2160,
    5000,
    VerPOSITIVE+HorPOSITIVE,
    5940000,
    3840+1440,
    3840,
    3840,
    3840+1440,
    3840+1056,
    3840+1056+88,
    2160+90,
    2160,
    2160,
    2160+90,
    2160+8,
    2160+8+10
},
//107 3840x2160p@60Hz, 64:27
{
    TIMING_ATTRIB_SIZE,
    107,
    3840, 2160,
    6000,
    VerPOSITIVE+HorPOSITIVE,
    5940000,
    3840+560,
    3840,
    3840,
    3840+560,
    3840+176,
    3840+176+88,
    2160+90,
    2160,
    2160,
    2160+90,
    2160+8,
    2160+8+10
},
//normal VIC end
//extended resolution begin
//3840x2160p@30Hz, 16:9
{
    TIMING_ATTRIB_SIZE,
    108,
    3840, 2160,
    3000,
    VerPOSITIVE+HorPOSITIVE,
    2970000,
    3840+560,
    3840,
    3840,
    3840+560,
    3840+176,
    3840+176+88,
    2160+90,
    2160,
    2160,
    2160+90,
    2160+8,
    2160+8+10   
},
//3840x2160p@25Hz, 16:9
{
    TIMING_ATTRIB_SIZE,
    109,
    3840, 2160,
    2500,
    VerPOSITIVE+HorPOSITIVE,
    2970000,
    3840+1440,
    3840,
    3840,
    3840+1440,
    3840+1056,
    3840+1056+88,
    2160+90,
    2160,
    2160,
    2160+90,
    2160+8,
    2160+8+10   
},
//3840x2160p@24Hz, 16:9
{
    TIMING_ATTRIB_SIZE,
    110,
    3840, 2160,
    2400,
    VerPOSITIVE+HorPOSITIVE,
    2970000,
    3840+1660,
    3840,
    3840,
    3840+1660,
    3840+1276,
    3840+1276+88,
    2160+90,
    2160,
    2160,
    2160+90,
    2160+8,
    2160+8+10   
},
//4096x2160p@24Hz, 16:9(SMPTE)
{
    TIMING_ATTRIB_SIZE,
    111,
    4096, 2160,
    2400,
    VerPOSITIVE+HorPOSITIVE,
    2970000,
    4096+1404,
    4096,
    4096,
    4096+1404,
    4096+1020,
    4096+1020+88,
    2160+90,
    2160,
    2160,
    2160+90,
    2160+8,
    2160+8+10
},
};

//RefRateIndex init to 0FFh, which means not legal index.
CBIOS_HDMI_FORMAT_MTX CEAVideoFormatTable[CBIOS_HDMIFORMATCOUNTS] =
{
//AR, 1: 4:3; 2: 16:9; 3: 64:27; 4: 256:135
//VIC, AR, interlace, support
    //normal VICs
    {1, 1, 0, 1},            //640x480p@59.94
    {2, 1, 0, 1},           //720x480p@59.94
    {3, 2, 0, 1},           //720x480p@59.94
    {4, 2, 0, 1},          //1280x720p@60
    {5, 2, 1, 1},          //1920x1080i@60
    {6, 1, 1, 0},           //720x480i@59.94
    {7, 2, 1, 0},           //720x480i@59.94
    {8, 1, 0, 0},           //720x240p@59.94
    {9, 2, 0, 0},          //720x240p@59.94
    {10, 1, 1, 0},         //1440x480i@59.94
    {11, 2, 1, 0},         //1440x480i@59.94
    {12, 1, 0, 0},         //1440x240p@59.94
    {13, 2, 0, 0},         //1440x240p@59.94
    {14, 1, 0, 0},         //1440x480p@59.94
    {15, 2, 0, 0},         //1440x480p@59.94
    {16, 2, 0, 1},         //1920x1080p@60
    {17, 1, 0, 1},         //720x576p@50
    {18, 2, 0, 1},         //720x576p@50
    {19, 2, 0, 1},         //1280x720p@50
    {20, 2, 1, 1},         //1920x1080i@50
    {21, 1, 1, 0},         //720x576i@50
    {22, 2, 1, 0},         //720x576p@50
    {23, 1, 0, 0},         //720x288p@50
    {24, 2, 0, 0},         //720x288p@50
    {25, 1, 1, 0},         //1440x576i@50
    {26, 2, 1, 0},         //1440x576i@50
    {27, 1, 0, 0},         //1440x288p@50
    {28, 2, 0, 0},         //1440x288p@50
    {29, 1, 0, 0},         //1440x576p@50
    {30, 2, 0, 0},         //1440x576p@50
    {31, 2, 0, 1},         //1920x1080p@50
    {32, 2, 0, 1},         //1920x1080p@24
    {33, 2, 0, 1},         //1920x1080p@25
    {34, 2, 0, 1},         //1920x1080p@30
    {35, 1,  0, 0},         //2880x480p@59.94
    {36, 2,  0, 0},         //2880x480p@59.94
    {37, 1,  0, 0},         //2880x576p@50
    {38, 2,  0, 0},         //2880x576p@50
    {39, 2, 1, 0},         //1920x1080i@50
    {40, 2, 1, 0},         //1920x1080i@100
    {41, 2, 0, 1},         //1280x720p@100
    {42, 1, 0, 1},         //720x576p@100
    {43, 2, 0, 1},         //720x576p@100
    {44, 1, 1, 0},         //720x576i@100
    {45, 2, 1, 0},         //720x576i@100
    {46, 2, 1, 0},         //1920x1080i@120
    {47, 2, 0, 1},         //1280x720p@120
    {48, 1, 0, 1},           //720x480p@119.88
    {49, 2, 0, 1},           //720x480p@119.88
    {50, 1, 1, 0},           //720x480i@119.88
    {51, 2, 1, 0},           //720x480i@119.88
    {52, 1, 0, 1},         //720x576p@200
    {53, 2, 0, 1},         //720x576p@200
    {54, 1, 1, 0},         //720x576i@200
    {55, 2, 1, 0},         //720x576i@100
    {56, 1, 0, 1},           //720x480p@239.76
    {57, 2, 0, 1},           //720x480p@239.76
    {58, 1, 1, 0},           //720x480i@239.76
    {59, 2, 1, 0},           //720x480i@239.76
    {60, 2, 0, 1},         //1280x720p@24
    {61, 2, 0, 1},         //1280x720p@25
    {62, 2, 0, 1},         //1280x720p@30
    {63, 2, 0, 1},         //1920x1080p@120
    {64, 2, 0, 1},         //1920x1080p@100
    // CEA-861-F
    {65, 3, 0, 1},         //1280x720p@24
    {66, 3, 0, 1},         //1280x720p@25
    {67, 3, 0, 1},         //1280x720p@30
    {68, 3, 0, 1},         //1280x720p@50
    {69, 3, 0, 1},         //1280x720p@60
    {70, 3, 0, 1},         //1280x720p@100
    {71, 3, 0, 1},         //1280x720p@120
    {72, 3, 0, 1},         //1920x1080p@24
    {73, 3, 0, 1},         //1920x1080p@25
    {74, 3, 0, 1},         //1920x1080p@30
    {75, 3, 0, 1},         //1920x1080p@50
    {76, 3, 0, 1},         //1920x1080p@60
    {77, 3, 0, 1},         //1920x1080p@100
    {78, 3, 0, 1},         //1920x1080p@120
    {79, 3, 0, 0},         //1680x720p@24
    {80, 3, 0, 0},         //1680x720p@25
    {81, 3, 0, 0},         //1680x720p@30
    {82, 3, 0, 0},         //1680x720p@50
    {83, 3, 0, 0},         //1680x720p@60
    {84, 3, 0, 0},         //1680x720p@100
    {85, 3, 0, 0},         //1680x720p@120
    {86, 3, 0, 1},         //2560x1080p@24
    {87, 3, 0, 1},         //2560x1080p@25
    {88, 3, 0, 1},         //2560x1080p@30
    {89, 3, 0, 1},         //2560x1080p@50
    {90, 3, 0, 1},         //2560x1080p@60
    {91, 3, 0, 1},         //2560x1080p@100
    {92, 3, 0, 1},         //2560x1080p@120
    {93, 2, 0, 1},         //3840x2160p@24
    {94, 2, 0, 1},         //3840x2160p@25
    {95, 2, 0, 1},         //3840x2160p@30
    {96, 2, 0, 1},         //3840x2160p@50
    {97, 2, 0, 1},         //3840x2160p@60
    {98, 4, 0, 1},         //4096x2160p@24
    {99, 4, 0, 1},         //4096x2160p@25
    {100, 4, 0, 1},         //4096x2160p@30
    {101, 4, 0, 1},         //4096x2160p@50
    {102, 4, 0, 1},         //4096x2160p@60
    {103, 3, 0, 1},         //3840x2160p@24
    {104, 3, 0, 1},         //3840x2160p@25
    {105, 3, 0, 1},         //3840x2160p@30
    {106, 3, 0, 1},         //3840x2160p@50
    {107, 3, 0, 1},         //3840x2160p@60

    //normal VIC end
    //extened VIC begin
    { CBIOS_HDMI_NORMAL_VIC_COUNTS + 1, 2, 0, 1},        //3840x2160p@30
    { CBIOS_HDMI_NORMAL_VIC_COUNTS + 2, 2, 0, 1},         //3840x2160p@25
    { CBIOS_HDMI_NORMAL_VIC_COUNTS + 3, 2, 0, 1},         //3840x2160p@24
    { CBIOS_HDMI_NORMAL_VIC_COUNTS + 4, 2, 0, 1},         //4096x2160p@24
    //extened VIC end
};

static DETAILEDTIMING_TABLE EDIDPixelClock[]= {
    {EDIDTIMING,0x00,0xFF},
    {EDIDTIMING,0x01,0xFF},
    {EDIDTIMINGEXIT,},
};
static DETAILEDTIMING_TABLE EDIDHorizontalActive[] = {
    {EDIDTIMING,0x02,0xFF},
    {EDIDTIMING,0X04,0XF0},
    {EDIDTIMINGEXIT,},
};
static DETAILEDTIMING_TABLE EDIDHorizontalBlanking[] = {
    {EDIDTIMING,0x03,0xFF},
    {EDIDTIMING,0X04,0X0F},
    {EDIDTIMINGEXIT,},
};
static DETAILEDTIMING_TABLE EDIDVerticalActive[] = {
    {EDIDTIMING,0x05,0xFF},
    {EDIDTIMING,0X07,0XF0},
    {EDIDTIMINGEXIT,},
};
static DETAILEDTIMING_TABLE EDIDVerticalBlanking[] = {
    {EDIDTIMING,0x06,0xFF},
    {EDIDTIMING,0X07,0X0F},
    {EDIDTIMINGEXIT,},
};
static DETAILEDTIMING_TABLE EDIDHorizontalSyncOffset[] = {
    {EDIDTIMING,0x08,0xFF},
    {EDIDTIMING,0x0B,0xC0},
    {EDIDTIMINGEXIT,},
};
static DETAILEDTIMING_TABLE EDIDHorizontalSyncPulseWidth[] = {
    {EDIDTIMING,0x09,0xFF},
    {EDIDTIMING,0x0B,0x30},
    {EDIDTIMINGEXIT,},
};
static DETAILEDTIMING_TABLE EDIDVerticalSyncOffset[] = {
    {EDIDTIMING,0x0A,0xF0},
    {EDIDTIMING,0x0B,0x0C},
    {EDIDTIMINGEXIT,},
};
static DETAILEDTIMING_TABLE EDIDVerticalSyncPulseWidth[] = {
    {EDIDTIMING,0x0A,0x0F},
    {EDIDTIMING,0x0B,0x03},
    {EDIDTIMINGEXIT,},
};
// The following is just for DTV
static DETAILEDTIMING_TABLE EDIDHorizontalImageSize[] = {
    {EDIDTIMING,0x0C,0xFF},
    {EDIDTIMING,0x0E,0xF0},
    {EDIDTIMINGEXIT,},
};
static DETAILEDTIMING_TABLE EDIDVerticalImageSize[] = {
    {EDIDTIMING,0x0D,0xFF},
    {EDIDTIMING,0x0E,0x0F},
    {EDIDTIMINGEXIT,},
};
/*
static DETAILEDTIMING_TABLE EDIDIsInterlaced[] = {
    {EDIDTIMING,0x11,0x80},
    {EDIDTIMINGEXIT,},
};
*/
static CBIOS_Module_EDID_ESTTIMINGS EstTiming[] = {
    { 0x320, 0x258, 6000},// EDID23h[0]: 800x600@60Hz
    { 0x320, 0x258, 5600},// EDID23h[1]: 800x600@56Hz
    { 0x280, 0x1E0, 7500},// EDID23h[2]: 640x480@75Hz
    { 0x280, 0x1E0, 7200},// EDID23h[3]: 640x480@72Hz
    { 0x280, 0x1E0, 6700},// EDID23h[4]: 640x480@67Hz
    { 0x280, 0x1E0, 6000},// EDID23h[5]: 640x480@60Hz
    { 0x2D0, 0x190, 8800},// EDID23h[6]: 720x400@88Hz
    { 0x2D0, 0x190, 7000},// EDID23h[7]: 720x400@70Hz
//Est timing byte 2
    { 0x500, 0x400, 7500},// EDID24h[0]: 1280x1024@75Hz
    { 0x400, 0x300, 7500},// EDID24h[1]: 1024x768@75Hz
    { 0x400, 0x300, 7000},// EDID24h[2]: 1024x768@70Hz
    { 0x400, 0x300, 6000},// EDID24h[3]: 1024x768@60Hz
    { 0x400, 0x300, 8700},// EDID24h[4]: 1024x768@87HzInterlace
    { 0x340, 0x270, 7500},// EDID24h[5]: 832x624@75Hz
    { 0x320, 0x258, 7500},// EDID24h[6]: 800x600@75Hz
    { 0x320, 0x258, 7200},// EDID24h[7]: 800x600@72Hz
//Est timing byte 3
    { 0x480, 0x366, 7500},// EDID25h[7]: 1152x870@75Hz
};

static CBIOS_U32  cbGetCEAFractRRClock(CBIOS_U32 OriClock)
{
    return  cbRound(OriClock * 100, 1001, ROUND_NEAREST) * 10;
}

static CBIOS_U32  cbGetCEAFractRefrate(CBIOS_U32 OriRefrate)
{
    return  (OriRefrate % 600) ? 0 : cbRound(OriRefrate * 1000, 1001, ROUND_NEAREST);
}

// The vertical frequencies of the 240p, 480p and 480i Video Formats are typically adjusted by a factor of exactly 1000/1001 for NTSC video compatibility
static inline CBIOS_BOOL cbIsCeaNTSCCompatMode(PCBIOS_TIMING_ATTRIB pTiming)
{
    return (pTiming->FormatVIC != 0 && (pTiming->YRes == 240 || pTiming->YRes == 480));
}

CBIOS_BOOL cbEdidModule_IsEquivalentModeExist(CBIOS_U16 VIC, CBIOS_U32 *pRefresh, CBIOS_U32 *pClock)
{
    CBIOS_U32 i = 0;

    // A Video Timing with a vertical frequency that is an integer multiple of 6.00 Hz is considered to be the same as a Video Timing
    // with the equivalent detailed timing information but where the frequency is adjusted by a factor of 1000/1001
    if(!VIC || !cbGetCEAFractRefrate(HDMIFormatTimingTbl[VIC-1].RefreshRate))
    {
        return CBIOS_FALSE;
    }

    if (cbIsCeaNTSCCompatMode(&HDMIFormatTimingTbl[VIC-1]))
    {
        if(pRefresh)
        {
            *pRefresh = HDMIFormatTimingTbl[VIC-1].RefreshRate;
        }
        if(pClock)
        {
            *pClock = HDMIFormatTimingTbl[VIC-1].PixelClock;
        }
    }
    else  //add the fractional value
    {
        if(pRefresh)
        {
            *pRefresh = cbGetCEAFractRefrate(HDMIFormatTimingTbl[VIC-1].RefreshRate);
        }
        if(pClock)
        {
            *pClock = cbGetCEAFractRRClock(HDMIFormatTimingTbl[VIC-1].PixelClock);
        }
    }

    return CBIOS_TRUE;
}

#define cbGetDiff(a, b)   ((a) > (b) ? (a - b) : (b - a))

CBIOS_BOOL cbEdidModule_IsRefreshInCEARange(CBIOS_U16 RefrashRate,
                                          CBIOS_U16 FormatVIC,
                                          CBIOS_U16 *pFixRefresh,
                                          CBIOS_U32 *pFixClock)
{
    CBIOS_BOOL bStatus = CBIOS_FALSE;
    CBIOS_U32 Difference = 0;
    CBIOS_U16 RefValue = 0, FractRefRate = 0, IntRefRate = 0, FormatIndex = 0;

    if(!FormatVIC || FormatVIC > CBIOS_HDMIFORMATCOUNTS || !RefrashRate)
    {
        return CBIOS_FALSE;
    }

    FormatIndex = FormatVIC-1;

    FractRefRate = cbGetCEAFractRefrate(HDMIFormatTimingTbl[FormatIndex].RefreshRate);  //fractional refresh rate(such as 6000->5994), if not exist, it's 0(such as 5000->0)
    IntRefRate = HDMIFormatTimingTbl[FormatIndex].RefreshRate; //interger refresh rate(such as 6000)

    if ((FractRefRate == 0) || (RefrashRate > ((FractRefRate + IntRefRate) / 2)))
    {
        RefValue = IntRefRate;
    }
    else
    {
        RefValue = FractRefRate;
    }

    if(RefValue == 0)
    {
        return CBIOS_FALSE;
    }

    Difference = cbGetDiff(RefrashRate, RefValue);

    if ((Difference * 1000 / RefValue) <= 5)//0.5%
    {
        bStatus = CBIOS_TRUE;
    }
    else
    {
        bStatus = CBIOS_FALSE;
    }

    if(bStatus)
    {
        if(pFixRefresh)
        {
            *pFixRefresh = (RefValue == IntRefRate)? IntRefRate : FractRefRate;
        }
        if(pFixClock)
        {
            CBIOS_U32 IntClock = HDMIFormatTimingTbl[FormatIndex].PixelClock;
            *pFixClock = (RefValue == IntRefRate)? IntClock : cbGetCEAFractRRClock(IntClock);
        }
    }

    return bStatus;
}

static CBIOS_BOOL cbEDIDModule_GetFmtIdxFromSVD(CBIOS_U8 SVD, CBIOS_U8 *pFormatIdx, CBIOS_BOOL *pIsNative)
{
    CBIOS_BOOL bRet = CBIOS_TRUE;

    *pIsNative = CBIOS_FALSE;
/*
According to CEA-861-F:
    If SVD >=1 and SVD <=64 then  
        7-bit VIC is defined (7-LSB\u2019s) and NOT a native code 
    Elseif SVD >=65 and SVD <=127 then  
        8-bit VIC is defined (from first new set)
    Elseif SVD >=129 and SVD <=192 then  
        7-bit VIC is defined (7-LSB\u2019s) and IS a native code 
    Elseif SVD >=193 and SVD <=253 then  
        8-bit VIC is defined (from second new set) 
    Elseif SVD == 0/128/254/255 then
        Reserved
    End if
*/
    if (SVD >=1 && SVD <= 64)
    {
        *pFormatIdx = SVD & 0x7F;
    }
    else if (SVD >= 65 && SVD <= 127)
    {
        *pFormatIdx = SVD;
    }
    else if (SVD >= 129 && SVD <= 192)
    {
        *pFormatIdx = SVD & 0x7F;
        *pIsNative = CBIOS_TRUE;
    }
    else if ((SVD >= 193) && (SVD <= 253))
    {
        *pFormatIdx = SVD;
    }
    else
    {
        cbDebugPrint((MAKE_LEVEL(GENERIC, ERROR), "cbEDIDModule_GetFmtIdxFromSVD: SVD = %d which is a reserved SVD code\n", SVD));
        bRet = CBIOS_FALSE;
    }

    if ((*pFormatIdx == 0) || (*pFormatIdx > CBIOS_HDMI_NORMAL_VIC_COUNTS))
    {
        cbDebugPrint((MAKE_LEVEL(GENERIC, ERROR), "cbEDIDModule_GetFmtIdxFromSVD: FormatIdx = %d which is invalid\n", *pFormatIdx));
        bRet = CBIOS_FALSE;
    }

    return bRet;
}


//Check whether the SVD format already exists in device timing list
static CBIOS_BOOL cbEDIDModule_isSvdFormatExist(PCBIOS_EDID_STRUCTURE_DATA pEDIDStruct, CBIOS_U16 FmtVIC, CBIOS_U32* TimingIdx)
{
    CBIOS_BOOL bExist = CBIOS_FALSE;
    CBIOS_U32 index = 0;
    PCBIOS_DEVICE_TIMING_INFO  pTiming = CBIOS_NULL;

    if(!pEDIDStruct->pDeviceTimingList || !pEDIDStruct->TimingNum || !FmtVIC)
    {
        return bExist;
    }

    pTiming = pEDIDStruct->pDeviceTimingList;

    for(index = 0; index < pEDIDStruct->TimingNum; index++)
    {
        if(pTiming[index].FullTiming.FormatVIC == FmtVIC)
        {
            bExist = CBIOS_TRUE;
            break;
        }
    }

    if(bExist && TimingIdx)
    {
        *TimingIdx = index;
    }

    return bExist;
}

//Loop detail/dtd/displayid timing to check whether a timing has the same timing attrib with pSvdTiming
static CBIOS_BOOL  cbEDIDModule_hasSameSvdTiming(PCBIOS_EDID_STRUCTURE_DATA pEDIDStruct, 
                                          PCBIOS_DEVICE_TIMING_INFO pSvdTiming,
                                          CBIOS_U32* TimingIdx)
{
    CBIOS_BOOL bExist = CBIOS_FALSE;
    CBIOS_U32 index = 0;
    PCBIOS_DEVICE_TIMING_INFO  pTiming = CBIOS_NULL;
    CBIOS_TIMING_ATTRIB tempTiming;

    if(!pEDIDStruct->pDeviceTimingList || !pEDIDStruct->TimingNum || !pSvdTiming)
    {
        return bExist;
    }

    pTiming = pEDIDStruct->pDeviceTimingList;

    for(index = 0; index < pEDIDStruct->TimingNum; index++)
    {
        //same SVD case is handled in isSvdFormatExist function
        if(pTiming[index].FullTiming.FormatVIC)
        {
            continue;
        }

        cb_memcpy(&tempTiming, &pTiming[index].FullTiming, sizeof(CBIOS_TIMING_ATTRIB));
        tempTiming.FormatVIC = pSvdTiming->FullTiming.FormatVIC;
        tempTiming.RefreshRate = pSvdTiming->FullTiming.RefreshRate; //refresh rate in detail timing is not accurate(calculate with cbRound method)
        tempTiming.HorSyncStart = pSvdTiming->FullTiming.HorSyncStart;    //ignore different in h/v sync position
        tempTiming.HorSyncEnd = pSvdTiming->FullTiming.HorSyncEnd;
        tempTiming.VerSyncStart = pSvdTiming->FullTiming.VerSyncStart;
        tempTiming.VerSyncEnd = pSvdTiming->FullTiming.VerSyncEnd;

        //compare svd table timing with detail timing in active/total/clock/ar/interlace
        if(!cb_memcmp(&tempTiming, &pSvdTiming->FullTiming, sizeof(CBIOS_TIMING_ATTRIB)) &&
            pTiming[index].AspectRatio == pSvdTiming->AspectRatio &&
            pTiming[index].IsInterLaced == pSvdTiming->IsInterLaced)
        {
            bExist = CBIOS_TRUE;
            break;
        }
    }

    if(bExist && TimingIdx)
    {
        *TimingIdx = index;
    }

    return bExist;
}


#define MAX_DEVICE_TIMING_NUM  48

static CBIOS_BOOL  cbEDIDModule_AddDeviceTiming(PCBIOS_EDID_STRUCTURE_DATA pEDIDStruct, CBIOS_DEVICE_TIMING_INFO * pTiming)
{
    CBIOS_BOOL bAdded = CBIOS_FALSE;

    if (!pEDIDStruct || !pTiming)
    {
        return bAdded;
    }

    if (!pEDIDStruct->pDeviceTimingList)
    {
        pEDIDStruct->TimingNum = 0;
        pEDIDStruct->pDeviceTimingList = cb_AllocatePagedPool(MAX_DEVICE_TIMING_NUM * sizeof(CBIOS_DEVICE_TIMING_INFO));
        pEDIDStruct->TimingArraySize = (pEDIDStruct->pDeviceTimingList) ? MAX_DEVICE_TIMING_NUM : 0;
    }

    if (pEDIDStruct->TimingNum >= pEDIDStruct->TimingArraySize && pEDIDStruct->pDeviceTimingList)
    {
        CBIOS_U32 NewSize = pEDIDStruct->TimingArraySize + MAX_DEVICE_TIMING_NUM / 2;
        PCBIOS_DEVICE_TIMING_INFO  pTempTable = cb_AllocatePagedPool(NewSize * sizeof(CBIOS_DEVICE_TIMING_INFO));
        if (pTempTable)
        {
            cb_memcpy(pTempTable, pEDIDStruct->pDeviceTimingList, pEDIDStruct->TimingArraySize * sizeof(CBIOS_DEVICE_TIMING_INFO));
            cb_FreePool(pEDIDStruct->pDeviceTimingList);
            pEDIDStruct->pDeviceTimingList = pTempTable;
            pEDIDStruct->TimingArraySize = NewSize;
        }
    }

    if (pEDIDStruct->pDeviceTimingList && pEDIDStruct->TimingNum < pEDIDStruct->TimingArraySize)
    {
        cb_memcpy(&pEDIDStruct->pDeviceTimingList[pEDIDStruct->TimingNum], pTiming, sizeof(CBIOS_DEVICE_TIMING_INFO));
        pEDIDStruct->TimingNum += 1;
        bAdded = CBIOS_TRUE;
    }

    return bAdded;
}

//Add SVD format timing to timing list. If add success or the timing already exists, return TRUE, 
//otherwise return FALSE. If return TRUE,  update AddedIndex
static CBIOS_BOOL  cbEDIDModule_AddSvdFormat(PCBIOS_EDID_STRUCTURE_DATA pEDIDStruct, 
                                CBIOS_U16 FmtVic, CBIOS_BOOL bNative, CBIOS_U32 *AddedIndex)
{
    CBIOS_BOOL bAdded = CBIOS_TRUE;
    CBIOS_DEVICE_TIMING_INFO  DeviceTiming;
    CBIOS_U32  Index = 0;

    if(!pEDIDStruct)
    {
        return CBIOS_FALSE;
    }

    if(!CEAVideoFormatTable[FmtVic-1].Supported)
    {
        return CBIOS_FALSE;
    }

    //SVD already exists?
    if(cbEDIDModule_isSvdFormatExist(pEDIDStruct, FmtVic, &Index))
    {
        if(AddedIndex)
        {
            *AddedIndex = Index;
        }
        return CBIOS_TRUE;
    }

    cb_memset(&DeviceTiming, 0, sizeof(CBIOS_DEVICE_TIMING_INFO));
    DeviceTiming.TimingType = CBIOS_SVD_TIMING;
    cb_memcpy(&DeviceTiming.FullTiming, &HDMIFormatTimingTbl[FmtVic-1], sizeof(CBIOS_TIMING_ATTRIB));
    DeviceTiming.AspectRatio = CEAVideoFormatTable[FmtVic-1].AspectRatio;
    DeviceTiming.IsNativeMode = (bNative)? 1 : 0;
    DeviceTiming.IsInterLaced = CEAVideoFormatTable[FmtVic-1].Interlace;

    if (cbIsCeaNTSCCompatMode(&HDMIFormatTimingTbl[FmtVic-1]))
    {
        DeviceTiming.FullTiming.RefreshRate = cbGetCEAFractRefrate(HDMIFormatTimingTbl[FmtVic-1].RefreshRate);
        DeviceTiming.FullTiming.PixelClock = cbGetCEAFractRRClock(DeviceTiming.FullTiming.PixelClock);
    }

    //Some other type detail timing report the same timing?
    if(cbEDIDModule_hasSameSvdTiming(pEDIDStruct, &DeviceTiming, &Index))
    {
        pEDIDStruct->pDeviceTimingList[Index].FullTiming.RefreshRate = DeviceTiming.FullTiming.RefreshRate; //use table refresh instead of calculated value
        pEDIDStruct->pDeviceTimingList[Index].FullTiming.FormatVIC = FmtVic;
        if(DeviceTiming.IsNativeMode)
        {
            pEDIDStruct->pDeviceTimingList[Index].IsNativeMode = 1;
        }
        if(AddedIndex)
        {
            *AddedIndex = Index;
        }
        return CBIOS_TRUE;
    }

    bAdded = cbEDIDModule_AddDeviceTiming(pEDIDStruct, &DeviceTiming);
    if(bAdded && AddedIndex)
    {
        *AddedIndex = pEDIDStruct->TimingNum-1;
    }

    return bAdded;
}

//Get/Calc timing that only have x/y/rr.
CBIOS_BOOL cbEDIDModule_GetXYRTiming(PCBIOS_EDID_STRUCTURE_DATA pEDIDStruct, 
                                  CBIOS_U16 XRes, CBIOS_U16 YRes, CBIOS_U16 RefreshRate, CBIOS_TIMING_ATTRIB *pTiming)
{
    CBIOS_BOOL bFound = CBIOS_FALSE, bFractMode = CBIOS_FALSE;
    CBIOS_TIMING_ATTRIB *pTimingTbl = CBIOS_NULL;
    CBIOS_U32  NumOfTimings = 0, i = 0;

    if(!pEDIDStruct)
    {
        return CBIOS_FALSE;
    }

    pTimingTbl = AdapterTimingTbl;
    NumOfTimings = sizeofarray(AdapterTimingTbl);      

    //patch for HDMI 640*480@60HZ timing.When set 640*480 of DP and HDMI device,use HDMI timing table 
    //since AdapterTiming also have this mode
    if((XRes== 640) && (YRes == 480) && (RefreshRate == 6000) 
        && (pEDIDStruct->Attribute.IsCEA861HDMI))
    {
        pTimingTbl = HDMIFormatTimingTbl;
        NumOfTimings = sizeofarray(HDMIFormatTimingTbl);
    }

    for(i = 0; i < NumOfTimings; i++)
    {            
        if((pTimingTbl[i].XRes == XRes) && (pTimingTbl[i].YRes == YRes))
        {
            if ((RefreshRate == 0) || ((pTimingTbl[i].RefreshRate/100) == (RefreshRate/100)))
            {
                bFound = CBIOS_TRUE;
                break;
            }
        }
    }
    
    if(bFound)
    {
        cb_memcpy(pTiming, &pTimingTbl[i],sizeof(CBIOS_TIMING_ATTRIB));
    }

    if(!bFound && pTimingTbl != HDMIFormatTimingTbl)
    {        
        pTimingTbl = HDMIFormatTimingTbl;
        NumOfTimings = sizeofarray(HDMIFormatTimingTbl);

        for (i = 0; i < NumOfTimings; i++)
        {
            if(CEAVideoFormatTable[i].Supported &&
                (pTimingTbl[i].XRes == XRes) &&
                (pTimingTbl[i].YRes == YRes) &&
                (CEAVideoFormatTable[i].Interlace == 0))
            {          
                if(RefreshRate / 100 == pTimingTbl[i].RefreshRate/100)
                {
                    bFractMode = CBIOS_FALSE;
                    bFound = CBIOS_TRUE;
                    break;
                }
                else if(RefreshRate / 100 == cbGetCEAFractRefrate(pTimingTbl[i].RefreshRate) /100)
                {
                    bFractMode = CBIOS_TRUE;
                    bFound = CBIOS_TRUE;
                    break;
                }
            }
        }

        if (bFound)
        {
            cb_memcpy(pTiming, &pTimingTbl[i], sizeof(CBIOS_TIMING_ATTRIB));

            if (bFractMode)//fractional refreshrate
            {
                pTiming->RefreshRate = cbGetCEAFractRefrate(pTiming->RefreshRate);
                pTiming->PixelClock = cbGetCEAFractRRClock(pTiming->PixelClock);
            }
        }
    }

    if(!bFound)
    {   
        //Customize timing not support interlace mode.
        //Timing value has no difference between interlace mode and non-interlace mode.
        bFound = cbCalcCustomizedTiming(XRes, YRes, RefreshRate, pTiming);                              
    }

    return bFound;
}

static CBIOS_U32 cbEDIDModule_MapMaskGetEdidInfo(CBIOS_U8* DetailedTimings, CBIOS_U32 Base, DETAILEDTIMING_TABLE* TimingTable)
{
    CBIOS_U32 uRet = 0;
    CBIOS_U8 byRegValue = 0;
    DETAILEDTIMING_TABLE *reg = TimingTable;
    CBIOS_U32 i, j=0;

    while(reg->type != EDIDTIMINGEXIT)
    {
        byRegValue = DetailedTimings[18*Base + reg->index];
        for(i = 0;i<8;i++)
        {
            if( reg->mask & 1<<i )
            {
                uRet += byRegValue&(1<<i)? 1<<j : 0; 
                j++;
            }
        }
        reg++;
    }
    return uRet;

}

static inline CBIOS_BOOL cbEDIDModule_MatchTiming(PCBIOS_DEVICE_TIMING_INFO pTiming,
                                                  CBIOS_U32 XResolution,
                                                  CBIOS_U32 YResolution,
                                                  CBIOS_U32 RefreshRate,
                                                  CBIOS_U32 InterlaceFlag)
{
    return (pTiming->FullTiming.XRes == XResolution && pTiming->FullTiming.YRes == YResolution &&
            !!pTiming->IsInterLaced == !!InterlaceFlag &&
            (pTiming->FullTiming.RefreshRate >= (RefreshRate - 50) && pTiming->FullTiming.RefreshRate <= (RefreshRate + 50)));
}

// Patch or filter timing
static CBIOS_BOOL cbEDIDModule_ValidateTiming(PCBIOS_EDID_STRUCTURE_DATA pEDIDStruct, PCBIOS_DEVICE_TIMING_INFO pTiming)
{
    PCBIOS_MONITOR_MISC_ATTRIB  pMonitorAttrib = CBIOS_NULL;
    CBIOS_U32 i = 0;

    if (!pEDIDStruct || !pTiming)
    {
        return CBIOS_FALSE;
    }

    pMonitorAttrib = &(pEDIDStruct->Attribute);

    if (pTiming->FullTiming.PixelClock < CBIOS_MIN_PIXEL_CLK ||
        pTiming->FullTiming.XRes < CBIOS_MIN_TIMING_X  ||
        pTiming->FullTiming.YRes < CBIOS_MIN_TIMING_Y)
    {
        return CBIOS_FALSE;
    }

    //check whether the same timing exists
    for(i = 0; i < pEDIDStruct->TimingNum; i++)
    {
        if(!cb_memcmp(&pEDIDStruct->pDeviceTimingList[i], pTiming, sizeof(CBIOS_DEVICE_TIMING_INFO)))
        {
            return CBIOS_FALSE;
        }
    }

    if (pTiming->TimingType == CBIOS_DTD_TIMING)
    {
        // patch for BenQ EL2870U monitor, filter 3840x2160@29.97Hz mode in DTDTimings for no sound issue.
        if ((!cbStrnCmp(pMonitorAttrib->MonitorID, (CBIOS_UCHAR*)"BNQ7949", 7)) && (!cbStrnCmp(pMonitorAttrib->MonitorName, (CBIOS_UCHAR*)"BenQ EL2870U", 12)))
        {
            if (cbEDIDModule_MatchTiming(pTiming, 3840, 2160, 3000, 0))
            {
                return CBIOS_FALSE;
            }
        }
    }

    return CBIOS_TRUE;
}

static CBIOS_BOOL cbEDIDModule_ParseDtlTiming(CBIOS_U8 *pEdidDtlData, PCBIOS_DEVICE_TIMING_INFO pTiming)
{
    CBIOS_U16 Ratio = 0, SyncOffset = 0, SyncWidth = 0, Blank = 0;
    CBIOS_U32 HImageSize = 0, VImageSize = 0;
    CBIOS_U8 HSync = HorNEGATIVE, VSync = VerNEGATIVE;

    pTiming->FullTiming.FormatVIC = 0;
    pTiming->FullTiming.Size = TIMING_ATTRIB_SIZE;

    pTiming->FullTiming.PixelClock = cbEDIDModule_MapMaskGetEdidInfo(pEdidDtlData, 0, EDIDPixelClock)*100;
    if(pTiming->FullTiming.PixelClock == 0)
    {
        return CBIOS_FALSE;
    }
    
    pTiming->FullTiming.XRes = (CBIOS_U16)cbEDIDModule_MapMaskGetEdidInfo(pEdidDtlData,0,EDIDHorizontalActive);
    SyncOffset = (CBIOS_U16)cbEDIDModule_MapMaskGetEdidInfo(pEdidDtlData,0,EDIDHorizontalSyncOffset);
    SyncWidth = (CBIOS_U16)cbEDIDModule_MapMaskGetEdidInfo(pEdidDtlData,0,EDIDHorizontalSyncPulseWidth);
    Blank = (CBIOS_U16)cbEDIDModule_MapMaskGetEdidInfo(pEdidDtlData,0,EDIDHorizontalBlanking);
    pTiming->FullTiming.HorTotal = pTiming->FullTiming.XRes + Blank;
    pTiming->FullTiming.HorDisEnd = pTiming->FullTiming.XRes;
    pTiming->FullTiming.HorBStart = pTiming->FullTiming.XRes;
    pTiming->FullTiming.HorBEnd = pTiming->FullTiming.XRes + Blank; 
    pTiming->FullTiming.HorSyncStart = pTiming->FullTiming.XRes + SyncOffset;
    pTiming->FullTiming.HorSyncEnd = pTiming->FullTiming.HorSyncStart + SyncWidth;  

    pTiming->FullTiming.YRes = (CBIOS_U16)cbEDIDModule_MapMaskGetEdidInfo(pEdidDtlData,0,EDIDVerticalActive);
    SyncOffset = (CBIOS_U16)cbEDIDModule_MapMaskGetEdidInfo(pEdidDtlData,0,EDIDVerticalSyncOffset);
    SyncWidth = (CBIOS_U16)cbEDIDModule_MapMaskGetEdidInfo(pEdidDtlData,0,EDIDVerticalSyncPulseWidth);
    Blank = (CBIOS_U16)cbEDIDModule_MapMaskGetEdidInfo(pEdidDtlData,0,EDIDVerticalBlanking);

    pTiming->IsInterLaced = 0;
    if(pEdidDtlData[0x11] & 0x80)
    {
       pTiming->IsInterLaced = 1;
       pTiming->FullTiming.YRes *= 2;
       Blank *=2;
       Blank += 1;
       SyncOffset *= 2;
       SyncWidth *= 2;
    }
        
    pTiming->FullTiming.VerTotal = pTiming->FullTiming.YRes + Blank;
    pTiming->FullTiming.VerDisEnd = pTiming->FullTiming.YRes;
    pTiming->FullTiming.VerBStart = pTiming->FullTiming.YRes;    
    pTiming->FullTiming.VerBEnd = pTiming->FullTiming.YRes + Blank;
    pTiming->FullTiming.VerSyncStart = pTiming->FullTiming.YRes + SyncOffset;
    pTiming->FullTiming.VerSyncEnd = pTiming->FullTiming.VerSyncStart+SyncWidth;
    
    HImageSize = (CBIOS_U16)cbEDIDModule_MapMaskGetEdidInfo(pEdidDtlData,0,EDIDHorizontalImageSize);
    VImageSize =  (CBIOS_U16)cbEDIDModule_MapMaskGetEdidInfo(pEdidDtlData,0,EDIDVerticalImageSize); 

    if((pTiming->FullTiming.XRes == 0 ) || (pTiming->FullTiming.YRes == 0) ||
        (pTiming->FullTiming.HorTotal == 0) || (pTiming->FullTiming.VerTotal == 0))
    {
        return CBIOS_FALSE;
    }      

    if(VImageSize != 0)   // Prevent being divided by zero
    {
        Ratio = (CBIOS_U16)(HImageSize * 10000 / VImageSize);
        pTiming->AspectRatio = ((Ratio>13000) && (Ratio<14000))? 1 : 2;
    }
    else
    {
        pTiming->AspectRatio = 2;
    }

    // HSync = 1, means HSync is positive.
    // else, means negative. the same as VSync.
    HSync = HorNEGATIVE;
    VSync = VerNEGATIVE;
    if((pEdidDtlData[0x11] & 0x18) == 0x18)
    {
        VSync = (pEdidDtlData[0x11] & 0x04)? VerPOSITIVE : VerNEGATIVE;
        HSync = (pEdidDtlData[0x11] & 0x02)? HorPOSITIVE : HorNEGATIVE;
    }

    pTiming->FullTiming.HVPolarity = HSync + VSync;
    pTiming->FullTiming.RefreshRate = cbCalcRefreshRate(pTiming->FullTiming.PixelClock,
                                                pTiming->FullTiming.HorTotal,
                                                pTiming->FullTiming.VerTotal);

    pTiming->FullTiming.RefreshRate = 100 * cbRound(pTiming->FullTiming.RefreshRate, 100, ROUND_NEAREST);

    pTiming->IsSupportMono = 1;

    return CBIOS_TRUE;
}

/***************************************************************
Function:    cbEDIDModule_GetEstablishMode

Description: Get EDID establish mode

Input:       pEDID, EDID data buffer

Output:      pEstablishMode, statistic establish mode in EDID

Return:      the number of establish mode supported in EDID
***************************************************************/
static CBIOS_U32 cbEDIDModule_GetEstablishMode(CBIOS_U8 *pEDID, PCBIOS_EDID_STRUCTURE_DATA pEDIDStruct)
{
    CBIOS_U32 ModeNumOld = pEDIDStruct->TimingNum;
    CBIOS_U32 i = 0, j = 0;
    CBIOS_U32 ulMask = 0x01, EstMasks = 0;    
    CBIOS_U8 *pEstTimingsInEDID = pEDID + ESTABLISH_TIMINGS_INDEX;
    CBIOS_DEVICE_TIMING_INFO DeviceTiming = {0};
    CBIOS_BOOL bFound = CBIOS_FALSE;
    CBIOS_U16 XRes, YRes, Refresh;

    EstMasks = *pEstTimingsInEDID;
    EstMasks += ((CBIOS_U32)*(pEstTimingsInEDID+1)) << 8;
    EstMasks += ((CBIOS_U32)*(pEstTimingsInEDID+2) & 0x80) << 9;

    for(i = 0; i < CBIOS_ESTABLISHMODECOUNT; i++)
    {
        if(!(EstMasks & ulMask ) || !EstModeSupportTbl[i])
        {
            ulMask = ulMask << 1;
            continue;
        }

        cb_memset(&DeviceTiming, 0, sizeof(DeviceTiming));
        bFound = CBIOS_FALSE;
        
        XRes = EstTiming[i].XResolution;
        YRes = EstTiming[i].YResolution;
        Refresh = EstTiming[i].RefreshRate;

        //check whether the same timing is already added
        for(j = 0; j < pEDIDStruct->TimingNum; j++)
        {
            if(pEDIDStruct->pDeviceTimingList[j].FullTiming.XRes == XRes &&
                pEDIDStruct->pDeviceTimingList[j].FullTiming.YRes == YRes &&
                pEDIDStruct->pDeviceTimingList[j].FullTiming.RefreshRate == Refresh &&
                pEDIDStruct->pDeviceTimingList[j].IsInterLaced == 0)
            {
                bFound = CBIOS_TRUE;
                break;
            }
        }

        if(!bFound)
        {
            bFound = cbEDIDModule_GetXYRTiming(pEDIDStruct, XRes, YRes, Refresh, &DeviceTiming.FullTiming);
            if(bFound)
            {
                DeviceTiming.FullTiming.FormatVIC = 0;
                DeviceTiming.TimingType = CBIOS_EST_TIMING;
                cbEDIDModule_AddDeviceTiming(pEDIDStruct, &DeviceTiming);
            }
        }
        
        ulMask = ulMask << 1;
    }

    return (pEDIDStruct->TimingNum - ModeNumOld);
}

/***************************************************************
Function:    cbEDIDModule_GetStandardMode

Description: Get EDID standard mode

Input:       pEDID, EDID data buffer

Output:      pStandardMode, statistic standard mode in EDID

Return:      the number of standard mode supported in EDID
***************************************************************/
static CBIOS_U32 cbEDIDModule_GetStandardMode(CBIOS_U8 *pEDID, PCBIOS_EDID_STRUCTURE_DATA pEDIDStruct)
{
    CBIOS_U32 ModeNumOld = pEDIDStruct->TimingNum;
    CBIOS_U32 i = 0, j = 0;
    CBIOS_U8 *pStdTimingsInEDID = pEDID + STANDARD_TIMINGS_INDEX;
    CBIOS_DEVICE_TIMING_INFO  DeviceTiming = {0};
    CBIOS_BOOL bFound = CBIOS_FALSE;
    CBIOS_U16 XRes, YRes, Refresh;

    for (i = 0; i < CBIOS_STDMODECOUNT; i++)
    {
        if ((*(pStdTimingsInEDID + i*2) == 0x01) || (*(pStdTimingsInEDID + i*2) == 0x00))
        {
            continue;
        }

        XRes = (CBIOS_U16)(*(pStdTimingsInEDID + i*2) + 31) * 8;
        Refresh = ((*(pStdTimingsInEDID + i*2 + 1) & 0x3F) + 60) * 100;

        switch((*(pStdTimingsInEDID + i*2 + 1) & 0xC0) >> 6) //Image Aspect Ratio
        {
            case 0: // 16:10 AR
            YRes = XRes*10 / 16;
            break;
            case 1: // 4:3 AR
            YRes = XRes*3 / 4;
            break;
            case 2: // 5:4 AR
            YRes = XRes*4 / 5;
            break;
            case 3: // 16:9 AR
            YRes = XRes*9 / 16;
            break;
        }

        bFound = CBIOS_FALSE;

        //check whether the same timing is already added
        for(j = 0; j < pEDIDStruct->TimingNum; j++)
        {
            if(pEDIDStruct->pDeviceTimingList[j].FullTiming.XRes == XRes &&
                pEDIDStruct->pDeviceTimingList[j].FullTiming.YRes == YRes &&
                pEDIDStruct->pDeviceTimingList[j].FullTiming.RefreshRate == Refresh &&
                pEDIDStruct->pDeviceTimingList[j].IsInterLaced == 0)
            {
                bFound = CBIOS_TRUE;
                break;
            }
        }

        if(!bFound)
        {
            bFound = cbEDIDModule_GetXYRTiming(pEDIDStruct, XRes, YRes, Refresh, &DeviceTiming.FullTiming);
            if(bFound)
            {
                DeviceTiming.FullTiming.FormatVIC = 0;
                DeviceTiming.TimingType = CBIOS_STD_TIMING;
                cbEDIDModule_AddDeviceTiming(pEDIDStruct, &DeviceTiming);
            }
        }
    }
    
    return (pEDIDStruct->TimingNum - ModeNumOld);
}

/***************************************************************
Function:    cbEDIDModule_GetDetailedMode

Description: Get detailed timings in base EDID

Input:       pEDID, EDID data buffer
             byTotalModeNum, specify how many 18 byte data blocks want to parse

Output:      pDetailedMode, statistic detailed timings in base EDID

Return:      the number of detailed timings get in base EDID
***************************************************************/
static CBIOS_U32 cbEDIDModule_GetDetailedMode(CBIOS_U8 *pEDID, PCBIOS_EDID_STRUCTURE_DATA pEDIDStruct)
{
    CBIOS_U32  ulNumOfDtlMode = 0;
    CBIOS_U32  i = 0;
    CBIOS_U8   *pDtlTimingsInBaseEDID = pEDID + DETAILED_TIMINGS_INDEX;
    PCBIOS_MONITOR_MISC_ATTRIB pMonitorAttrib = &(pEDIDStruct->Attribute);
    CBIOS_DEVICE_TIMING_INFO  DeviceTiming;

    //get detailed timings in base EDID 18 byte data blocks
    for(i = 0; i < CBIOS_DTLMODECOUNT; i++, pDtlTimingsInBaseEDID += 18)
    {
        if (pDtlTimingsInBaseEDID + 18 >= pEDID + 128)
        {
            break;
        }

        cb_memset(&DeviceTiming, 0, sizeof(CBIOS_DEVICE_TIMING_INFO));

        if(!cbEDIDModule_ParseDtlTiming(pDtlTimingsInBaseEDID, &DeviceTiming))
        {
            continue;
        }

        DeviceTiming.TimingType = CBIOS_DTL_TIMING;
        if(pMonitorAttrib->NativeFormatNum > pEDIDStruct->NativeNumUsed)
        {
            DeviceTiming.IsNativeMode = 1;
            pEDIDStruct->NativeNumUsed++;
        }

        if (!cbEDIDModule_ValidateTiming(pEDIDStruct, &DeviceTiming))
        {
            continue;
        }
        //generally,prefered mode is indicated in the first detailed timing block,but for AUO44B5, prefered mode
        //is indicated in both first detailed timing block and displayid block,driver use the latter as prefer mode
        if(i == 0)
        {
            if((!cbStrnCmp(pMonitorAttrib->MonitorID, (CBIOS_UCHAR*)"AUO44B5", 7)) && (!cbStrnCmp(pMonitorAttrib->MonitorName, (CBIOS_UCHAR*)"B145QAN01.K ", 12)))
            {
                DeviceTiming.IsPreferMode = 0;
            }
            else
            {
                DeviceTiming.IsPreferMode = 1;
            }
        }

        if(cbEDIDModule_AddDeviceTiming(pEDIDStruct, &DeviceTiming))
        {
            ulNumOfDtlMode++;
        }
    }

    return ulNumOfDtlMode;
}

/***************************************************************
Function:    cbEDIDModule_GetCEADetailedMode

Description: Get detailed timing descriptor in CEA extension

Input:       pEDID, EDID data buffer

Output:      pExtDtlMode, statistic detailed timings in CEA extension

Return:      the number of detailed timings get in CEA extension
***************************************************************/
static CBIOS_U32 cbEDIDModule_GetCEADetailedMode(CBIOS_U8 *pEdidBlock, CBIOS_U8 DtlOffset, PCBIOS_EDID_STRUCTURE_DATA pEDIDStruct)
{
    CBIOS_U32   ulNumMode = 0;
    CBIOS_U32   i;
    CBIOS_DEVICE_TIMING_INFO  TmpTiming;

    // Parse the extension detailed timing descriptors
    for (i = DtlOffset; (i + 18) < EDID_BLK_SIZE; i += 18)
    {
        cb_memset(&TmpTiming, 0, sizeof(CBIOS_DEVICE_TIMING_INFO));
        
        if(!cbEDIDModule_ParseDtlTiming(&pEdidBlock[i], &TmpTiming))
        {
            break;
        }

        TmpTiming.TimingType = CBIOS_DTD_TIMING;
        if(pEDIDStruct->Attribute.NativeFormatNum > pEDIDStruct->NativeNumUsed)
        {
            TmpTiming.IsNativeMode = 1;
            pEDIDStruct->NativeNumUsed++;
        }

        if (!cbEDIDModule_ValidateTiming(pEDIDStruct, &TmpTiming))
        {
            continue;
        }

        if(cbEDIDModule_AddDeviceTiming(pEDIDStruct, &TmpTiming))
        {
            ulNumMode++;
        }
    }

    return ulNumMode;
}

/***************************************************************
Function:    cbEDIDModule_GetSVDMode

Description: Get short video descriptor in video data block of CEA extension

Input:       pSVDDataInEDID, short video descriptor data buffer
             BlockIndex, the index of data block

Output:      pEDIDStruct, statistic video format in short video descriptor and SVD data block

Return:      the number of video format get in short video descriptor
***************************************************************/
static CBIOS_VOID cbEDIDModule_GetSVDMode(CBIOS_U8 *pSVDDataInEDID, PCBIOS_EDID_STRUCTURE_DATA pEDIDStruct, CBIOS_U32 BlockIndex)
{
    CBIOS_U32  PayloadLength = 0, j = 0;
    CBIOS_U8    SVD = 0;
    CBIOS_BOOL  IsNative = CBIOS_FALSE, Status = CBIOS_FALSE;
    CBIOS_CEA_SVD_DATA  *pSvdData = pEDIDStruct->Attribute.SVDData;
    
    //decode short video descriptor
    PayloadLength = pSVDDataInEDID[0] & 0x1F;

    pSvdData[BlockIndex - 1].SVDCount = (CBIOS_U8)PayloadLength;
    cb_memcpy(pSvdData[BlockIndex - 1].SVD, &pSVDDataInEDID[1], cb_min(PayloadLength, MAX_SVD_COUNT));

    for (j = 0; j < PayloadLength; j++)
    {
        CBIOS_U8 FormatVIC = 0;

        SVD = pSVDDataInEDID[1 + j];
        Status = cbEDIDModule_GetFmtIdxFromSVD(SVD, &FormatVIC, &IsNative);
        if (!Status)
        {
            continue;
        }

        cbEDIDModule_AddSvdFormat(pEDIDStruct, FormatVIC, IsNative, CBIOS_NULL);
    }
}

/***************************************************************
Function:    cbEDIDModule_GetSVDMode

Description: Get short audio descriptor in audio data block of CEA extension

Input:       pAudioFormatDataInEDID, short audio descriptor data buffer

Output:      pCEAAudioFormat, statistic audio format in short audio descriptor

Return:      the number of audio format get in short audio descriptor
***************************************************************/
static CBIOS_U32 cbEDIDModule_GetHDMIAudioFormat(CBIOS_U8 *pAudioFormatDataInEDID, PCBIOS_EDID_STRUCTURE_DATA pEdidStruct)
{
    CBIOS_U32   AudioFormatCode = 0, MaxBitRateIndex = 0, j = 0, PayloadLength = 0;
    CBIOS_HDMI_AUDIO_INFO * pCEAAudioFormat = pEdidStruct->HDAudioFormat;
    CBIOS_U32  ulNumOfAudioFormat = pEdidStruct->HDAudioFormatNum;
    CBIOS_U32  AddedNum = 0;
    
    //decode short video descriptor
    PayloadLength = pAudioFormatDataInEDID[0] & 0x1F;

    for (j = 0; j < PayloadLength/3; j++)
    {
        if(ulNumOfAudioFormat >= CBIOS_HDMI_AUDIO_FORMAT_COUNTS)
        {
            break;
        }
        
        AudioFormatCode = (pAudioFormatDataInEDID[1 + j * 3] >> 3) & 0xF;
        if ((AudioFormatCode > 0) && (AudioFormatCode < 16))
        {
            if (AudioFormatCode < 15)
            {
                pCEAAudioFormat[ulNumOfAudioFormat].Format = AudioFormatCode;
            }

            pCEAAudioFormat[ulNumOfAudioFormat].MaxChannelNum = (pAudioFormatDataInEDID[1 + j * 3] & 0x7) + 1;
            pCEAAudioFormat[ulNumOfAudioFormat].SampleRateUnit = pAudioFormatDataInEDID[2 + j * 3] & 0x7F;
            if (AudioFormatCode == 1)
            {
                pCEAAudioFormat[ulNumOfAudioFormat].BitDepth.BD_16bit = pAudioFormatDataInEDID[3 + j * 3] & 0x1;
                pCEAAudioFormat[ulNumOfAudioFormat].BitDepth.BD_20bit = (pAudioFormatDataInEDID[3 + j * 3] >> 1) & 0x1;
                pCEAAudioFormat[ulNumOfAudioFormat].BitDepth.BD_24bit = (pAudioFormatDataInEDID[3 + j * 3] >> 2) & 0x1;
            }
            else if (AudioFormatCode <= 8)
            {
                MaxBitRateIndex = pAudioFormatDataInEDID[3 + j * 3];
                pCEAAudioFormat[ulNumOfAudioFormat].MaxBitRate = MaxBitRateIndex * 8;
            }
            else if (AudioFormatCode <= 13)
            {
                pCEAAudioFormat[ulNumOfAudioFormat].AudioFormatDependValue = pAudioFormatDataInEDID[3 + j * 3];
            }
            else if (AudioFormatCode == 14)
            {
                pCEAAudioFormat[ulNumOfAudioFormat].Profile.Value = pAudioFormatDataInEDID[3 + j * 3] & 0x7;
            }
            else
            {
                if (((pAudioFormatDataInEDID[3 + j * 3] >> 3) & 0x1F) == 1)
                {
                    pCEAAudioFormat[ulNumOfAudioFormat].Format = CBIOS_AUDIO_FORMAT_HE_AAC;
                }
                else if(((pAudioFormatDataInEDID[3 + j * 3] >> 3) & 0x1F) == 2)
                {
                    pCEAAudioFormat[ulNumOfAudioFormat].Format = CBIOS_AUDIO_FORMAT_HE_AAC_V2;
                }
                else if(((pAudioFormatDataInEDID[3 + j * 3] >> 3) & 0x1F) == 3)
                {
                    pCEAAudioFormat[ulNumOfAudioFormat].Format = CBIOS_AUDIO_FORMAT_MPEG_SURROUND;
                }
            }

            ulNumOfAudioFormat++;
        }
    }

    AddedNum = ulNumOfAudioFormat - pEdidStruct->HDAudioFormatNum;
    pEdidStruct->HDAudioFormatNum = ulNumOfAudioFormat;

    return AddedNum;
}

static CBIOS_VOID cbEDIDPatchHDMIAudio(PCBIOS_EDID_STRUCTURE_DATA pEDIDStruct)
{
    CBIOS_U32 i = 0;
    CBIOS_HDMI_AUDIO_INFO *pCEAAudioFormat = pEDIDStruct->HDAudioFormat;
    CBIOS_U32 AudioFormatNum = pEDIDStruct->HDAudioFormatNum;
    CBIOS_BOOL PcmSupport = CBIOS_FALSE;

    if(!pEDIDStruct->Attribute.IsCEA861Audio)
    {
        return;
    }
    
    for(i = 0; i < AudioFormatNum; i++)
    {
        if(pCEAAudioFormat[i].Format == CBIOS_AUDIO_FORMAT_LPCM)
        {
            PcmSupport = CBIOS_TRUE;
            break;
        }
    }
    //if all AudioFormat not support LPCM,but it indicates support basic audio,should patch here
    if(!PcmSupport)
    {
        if(i >= CBIOS_HDMI_AUDIO_FORMAT_COUNTS)
        {
            i = CBIOS_HDMI_AUDIO_FORMAT_COUNTS - 1;
        }
        else
        {
            pEDIDStruct->HDAudioFormatNum += 1;
        }
        pCEAAudioFormat[i].Format = CBIOS_AUDIO_FORMAT_LPCM;
        pCEAAudioFormat[i].MaxChannelNum = 2;//two channel
        pCEAAudioFormat[i].SampleRateUnit = 7;//sample rates of 32kHz,44.1kHz,48kHz
        pCEAAudioFormat[i].BitDepth.BD_16bit = 1;//16 bits
    }
}

/***************************************************************
Function:    cbEDIDModule_ParseHDMIVSDB

Description: Decode vendor specific data block of CEA extension

Input:       pVSDBDataInEDID, vendor specific data buffer            

Output:      pVSDBData, decoded VSDB data

Return:      the total length of vendor specific data block
***************************************************************/

/*
                                                HDMI Vendor Specific Data Block
    -------------------------------------------------------------------------------------------------------------------------
    |Byte # |      7      |      6      |      5      |      4      |      3      |      2      |      1      |      0      |
    |-------|---------------------------------------------------------------------------------------------------------------|
    |   0   |     Vendor-specific tag code (=3)       |                             Length (=N)                             |
    |-------|---------------------------------------------------------------------------------------------------------------|
    |   1   |                                                                                                               |
    |-------|                         24-bit IEEE Registration Identifier (0x000C03)                                        |
    |   2   |                                 (least significant byte first)                                                |
    |-------|                                                                                                               |
    |   3   |                                                                                                               |
    |-------|---------------------------------------------------------------------------------------------------------------|
    |   4   |                       A                               |           B                                           |
    |-------|---------------------------------------------------------------------------------------------------------------|
    |   5   |                       C                               |           D                                           |
    |-------|---------------------------------------------------------------------------------------------------------------|-----------
    |       |   Supports  |     DC_     |    DC_      |    DC_      |     DC_      |    Rsvd     |     Rsvd   |    DVI_     |  extension
    |   6   |    _AI      |     48bit   |    36bit    |    30bit    |     Y444     |    (0)      |     (0)    |    Dual     |  fields   
    |-------|---------------------------------------------------------------------------------------------------------------|
    |   7   |                                                Max_TMDS_Clock                                                 |
    |-------|---------------------------------------------------------------------------------------------------------------|
    |       |    Latency_ |   I_Latency_|   HDMI_V    |   Rsvd      |              |             |            |             |
    |   8   |    Fields_  |   Fields_   |   ideo_pre  |   (0)       |     CNC3     |    CNC2     |     CNC1   |     CNC0    |
    |       |    Present  |   Present   |   sent      |             |              |             |            |             |
    |-------|---------------------------------------------------------------------------------------------------------------|
    |  (9)  |                                         Video_Latency                                                         |
    |-------|---------------------------------------------------------------------------------------------------------------|
    |  (10) |                                         Audio_Latency                                                         |
    |-------|---------------------------------------------------------------------------------------------------------------|
    |  (11) |                                   Interlaced_Video_Latency                                                    |
    |-------|---------------------------------------------------------------------------------------------------------------|
    |  (12) |                                   Interlaced_Audio_Latency                                                    |
    |-------|---------------------------------------------------------------------------------------------------------------|
    |  (13) |    3D_pres  |      3D_Multi_present     |          Image_Size        |    Rsvd     |     Rsvd   |     Rsvd    |
    |       |      ent    |                           |                            |    (0)      |     (0)    |     (0)     |
    |-------|---------------------------------------------------------------------------------------------------------------|
    |  (14) |              HDMI_VIC_LEN               |                               HDMI_3D_LEN                           |
    |-------|---------------------------------------------------------------------------------------------------------------|
    |  (15) |(if HDMI_VIC_LEN > 0)                                                                                          |
    |       |                                                  HDMI_VIC_1                                                   |
    |-------|---------------------------------------------------------------------------------------------------------------|
    |   ... |                                                     ...                                                       |
    |-------|---------------------------------------------------------------------------------------------------------------|
    |       |                                                  HDMI_VIC_M                                                   |
    |-------|---------------------------------------------------------------------------------------------------------------|
    |       |(if 3D_Multi_present = 01 or 10)                                                                               |
    |       |                                             3D_Structure_ALL_15...8                                           |
    |-------|---------------------------------------------------------------------------------------------------------------|
    |       |                                             3D_Structure_ALL_7...0                                            |
    |-------|---------------------------------------------------------------------------------------------------------------|
    |       |(if 3D_Multi_present = 10)                                                                                     |
    |       |                                                 3D_MASK_15...8                                                |
    |-------|---------------------------------------------------------------------------------------------------------------|
    |       |                                                 3D_MASK_7...                                                  |
    |-------|---------------------------------------------------------------------------------------------------------------|
    |       |                 2D_VIC_order_1                       |                       3D_Structure_1                   |
    |-------|---------------------------------------------------------------------------------------------------------------|
    |       |                 3D_Detail_1 ***                      |                       Reserved(0) ***                  |
    |-------|---------------------------------------------------------------------------------------------------------------|
    |       |                                                     ...                                                       |
    |-------|---------------------------------------------------------------------------------------------------------------|
    |       |                 2D_VIC_order_L                       |                       3D_Structure_L                   |
    |-------|---------------------------------------------------------------------------------------------------------------|
    |()*...N|                                                 Reserved (0)**                                                |
    -------------------------------------------------------------------------------------------------------------------------------------
*/

static CBIOS_U32 cbEDIDModule_ParseHDMIVSDB(CBIOS_U8 *pVSDBDataInEDID, PCBIOS_HDMI_VSDB_EXTENTION pVSDBData)
{
    CBIOS_U8    *pCurByte = pVSDBDataInEDID;
    CBIOS_U32   i = 0;
    CBIOS_U32   PayloadLen = 0;
    CBIOS_U8    dataLow = 0;
    CBIOS_U8    dataHigh = 0;

    if ((pVSDBDataInEDID == CBIOS_NULL) || (pVSDBData == CBIOS_NULL))
    {
        cbDebugPrint((MAKE_LEVEL(GENERIC, ERROR), "cbEDIDModule_ParseVSDB: NULL pointer!\n"));
        return 0;
    }

    //initialize CBIOS_HDMI_VSDB_EXTENTION
    cb_memset(pVSDBData, 0, sizeof(CBIOS_HDMI_VSDB_EXTENTION));
    pVSDBData->HDMI3DMask = 0xFFFF;
    
    //check tag and length
    cb_memcpy(&(pVSDBData->Tag), pCurByte++, sizeof(pVSDBData->Tag));
    
    if ((pVSDBData->Tag.VSDBTag != VENDOR_SPECIFIC_DATA_BLOCK_TAG)
        ||(pVSDBData->Tag.VSDBLength < 5))
    {
        cbDebugPrint((MAKE_LEVEL(GENERIC, ERROR), "cbEDIDModule_ParseHDMIVSDB: invalid VSDB data!\n"));
        pVSDBData->Tag.VSDBLength = 0;
    }
    else//tag OK
    {
        PayloadLen = 0;
        //Byte 1-5, VSDB header
        pVSDBData->VSDBHeader.RegistrationIDByte0 = *(pCurByte++);
        pVSDBData->VSDBHeader.RegistrationIDByte1 = *(pCurByte++);
        pVSDBData->VSDBHeader.RegistrationIDByte2 = *(pCurByte++);
        dataHigh = *(pCurByte++);
        dataLow = *(pCurByte++);
        pVSDBData->VSDBHeader.SourcePhyAddr = (CBIOS_U16) (dataHigh << 8) | dataLow;
        
        PayloadLen += 5;
        if (PayloadLen >= pVSDBData->Tag.VSDBLength)
        {
            goto VSDB_DONE;
        }
        
        //byte 6
        cb_memcpy(&(pVSDBData->SupportCaps), pCurByte, sizeof(CBIOS_U8));
        pCurByte++;
        
        PayloadLen++;
        if (PayloadLen >= pVSDBData->Tag.VSDBLength)
        {
            goto VSDB_DONE;
        }

        //byte 7, max TMDS clock in MHz / 5
        pVSDBData->MaxTMDSClock = (CBIOS_U16)(*pCurByte) * 5;//max tmds clock in MHz
        pCurByte++;

        PayloadLen++;
        if (PayloadLen >= pVSDBData->Tag.VSDBLength)
        {
            goto VSDB_DONE;
        }

        //byte 8
        cb_memcpy(&(pVSDBData->PresentFlags), pCurByte, sizeof(CBIOS_U8));
        pCurByte++;

        PayloadLen++;
        if (PayloadLen >= pVSDBData->Tag.VSDBLength)
        {
            goto VSDB_DONE;
        }

        //These 2 bytes are present only if LatencyFieldsPresent is set to 1
        if (pVSDBData->LatencyFieldsPresent)
        {
            pVSDBData->VideoLatency = *(pCurByte++);
            pVSDBData->AudioLatency = *(pCurByte++);

            PayloadLen += 2;
            if (PayloadLen >= pVSDBData->Tag.VSDBLength)
            {
                goto VSDB_DONE;
            }
        }
        else
        {
            pVSDBData->VideoLatency = 0;
            pVSDBData->AudioLatency = 0;
        }

        //These 2 bytes are present only if ILatencyFieldsPresent is set to 1
        //In fact, if ILatencyFieldsPresent == 1, LatencyFieldsPresent must = 1
        if (pVSDBData->ILatencyFieldsPresent)
        {
            pVSDBData->InterlacedVideoLatency = *(pCurByte++);
            pVSDBData->InterlacedAudioLatency = *(pCurByte++);

            PayloadLen += 2;
            if (PayloadLen >= pVSDBData->Tag.VSDBLength)
            {
                goto VSDB_DONE;
            }
        }
        else
        {
            pVSDBData->InterlacedVideoLatency = 0;
            pVSDBData->InterlacedAudioLatency = 0;
        }

        // If HDMIVideoPresent =1 then additional video format capabilities are described by using the fields starting after the Latency area.
        if (pVSDBData->HDMIVideoPresent)
        {
            CBIOS_U32 HDMI3DFormatLength = 0;
            CBIOS_U32 HDMI3DFormatCount = 0;
            CBIOS_U8 ExtVICCnt = 0;


            cb_memcpy(&(pVSDBData->HDMI3DPresentFlags), pCurByte++, sizeof(pVSDBData->HDMI3DPresentFlags));

            PayloadLen++;
            if (PayloadLen >= pVSDBData->Tag.VSDBLength)
            {
                goto VSDB_DONE;
            }

            //HDMI_3D_LEN[5bits] indicates the total length of following 3D video format capabilities,
            //including 3D_Structure_ALL_15...0, 3D_MASK_15...0, 2D_VIC_order_X,3D_Structure_X and 3D_Detail_X fields.
            pVSDBData->HDMI3DLen = (*pCurByte) & 0x1F;

            //HDMI_VIC_LEN [3bits] indicates the total length of HDMI_VIC_X
            pVSDBData->HDMIVICLen = (*pCurByte >> 5) & 0x07;
            pCurByte++;

            PayloadLen++;
            if (PayloadLen >= pVSDBData->Tag.VSDBLength)
            {
                goto VSDB_DONE;
            }

            //HDMI_VIC_X
            for (i = 0; i < pVSDBData->HDMIVICLen; i++)
            {

                if ((*pCurByte >= 1) && (*pCurByte <= CBIOS_HDMI_EXTENED_VIC_COUNTS))
                {
                    pVSDBData->HDMIVIC[ExtVICCnt++] = *pCurByte;
                }

                pCurByte++;

            }
            pVSDBData->HDMIVICLen = ExtVICCnt;

            PayloadLen += i;
            if (PayloadLen >= pVSDBData->Tag.VSDBLength)
            {
                goto VSDB_DONE;
            }

            HDMI3DFormatLength = pVSDBData->HDMI3DLen;

            // If 3D_Multi_present = 01, 3D_Structure_ALL_15...0 is present
            // and assigns 3D formats to all of the VICs listed in the first 16 entries in the EDID.
            // 3D_MASK_15...0 is not present.
            if (pVSDBData->HDMI3DMultiPresent == 0x01)
            {
                // 2 Byte 3D_Structure_ALL_15...8 and 3D_Structure_ALL_7...0
                dataHigh = *(pCurByte++);
                dataLow = *(pCurByte++);

                pVSDBData->HDMI3DStructAll = (dataHigh << 8) | dataLow;

                // 3D_Multi_Present = 01, 3D_MASK_X is not present
                pVSDBData->HDMI3DMask = 0xFFFF;

                //subtract 3D_Structure_ALL_15...0 length from HDMI_3D_LEN
                HDMI3DFormatLength -= 2;

                PayloadLen += 2;
                if (PayloadLen >= pVSDBData->Tag.VSDBLength)
                {
                    goto VSDB_DONE;
                }
            }
            else if (pVSDBData->HDMI3DMultiPresent == 0x02)
            {
                // If 3D_Multi_present = 10,
                // 3D_Structure_ALL_15...0 and 3D_MASK_15...0 are present and assign 3D formats to
                // some of the VICs listed in the first 16 entries in the EDID.

                // 2 Byte 3D_Structure_ALL_15...8 and 3D_Structure_ALL_7...0
                dataHigh = *(pCurByte++);
                dataLow = *(pCurByte++);

                pVSDBData->HDMI3DStructAll = (dataHigh << 8) | dataLow;

                // 2 Byte 3D_MASK_15...8 and 3D_MASK_7...0
                dataHigh = *(pCurByte++);
                dataLow = *(pCurByte++);

                pVSDBData->HDMI3DMask = (dataHigh << 8) | dataLow;

                //subtract 3D_Structure_ALL_15...0 and 3D_MASK_15...0 length from HDMI_3D_LEN
                HDMI3DFormatLength -= 4;

                PayloadLen += 4;
                if (PayloadLen >= pVSDBData->Tag.VSDBLength)
                {
                    goto VSDB_DONE;
                }
            }
            else
            {
                // 3D_Multi_Present = 00 or 11, 3D_Structure_ALL_15...0 and 3D_MASK_15...0 are not present
                pVSDBData->HDMI3DStructAll = 0x0000;
                pVSDBData->HDMI3DMask = 0xFFFF;
            }

            if (pVSDBData->HDMI3DLen < HDMI3DFormatLength)
            {
                cbDebugPrint((MAKE_LEVEL(GENERIC, WARNING), "cbEDIDModule_ParseHDMIVSDB: HDMI_3D_LEN error!\n"));
            }
            else
            {
                i = 0;
                HDMI3DFormatCount = 0;
                while (i < HDMI3DFormatLength)
                {
                    // 2D_VIC_order_X: a pointer to a particular VIC in the EDID based on the order in which the VICs are stored in the EDID.
                    pVSDBData->HDMI3DForamt[HDMI3DFormatCount].HDMI2DVICOrder = (*pCurByte >> 4) & 0x0F;
                    // 3D_Structure_X: This field indicates the 3D capability for the corresponding VIC code indicated by 2D_VIC_order_X.
                    pVSDBData->HDMI3DForamt[HDMI3DFormatCount].HDMI3DStructure = (*pCurByte) & 0x0F;
                    pCurByte++;
                    i++;

                    // 3D_Detail_X: This field indicates the 3D capability for the corresponding VIC code indicated by 2D_VIC_order_X.
                    // If If 3D_Structure_X is 0000~0111, this field shall not be present
                    // If 3D_Structure_X is 1000~1111 (including Side-by-Side (Half)), this field shall be present
                    if (pVSDBData->HDMI3DForamt[HDMI3DFormatCount].HDMI3DStructure >= 0x08)
                    {
                        pVSDBData->HDMI3DForamt[HDMI3DFormatCount].HDMI3DDetail = ((*pCurByte++) >> 4) & 0x0F;
                        i++;
                    }
                    HDMI3DFormatCount++;
                }
                pVSDBData->HDMI3DFormatCount = HDMI3DFormatCount;

                PayloadLen += HDMI3DFormatLength;
                if (PayloadLen >= pVSDBData->Tag.VSDBLength)
                {
                    goto VSDB_DONE;
                }
            }


        }
     
    }

VSDB_DONE:

    //check payload length
    if (PayloadLen != pVSDBData->Tag.VSDBLength)
    {
        cbDebugPrint((MAKE_LEVEL(GENERIC, WARNING), "cbEDIDModule_ParseHDMIVSDB: payload length error!\n"));
    }

    //block total length = payload len + 1
    return (pVSDBData->Tag.VSDBLength + 1);
 
}

/***************************************************************
Function:    cbEDIDModule_ParseHFSCDS

Description: Decode HF-VSDB or HF-SCDB of CEA extension

Input:       pSCDSDataInEDID, HDMI Forum vendor specific data buffer, or Sink Capability data block buffer

Output:      pSCDSData, decoded Sink capability data structure

Return:      the total length of HDMI Forum vendor specific data block, or Sink capability data block
***************************************************************/

/*
                                            SCDS (Sink Capability Data Structure)
    |-----------|---------------------------------------------------------------------------------------------------------------|
    |    PB1    |                                                   Version (=1)                                                |
    |-----------|---------------------------------------------------------------------------------------------------------------|
    |    PB2    |                                              Max_TMDS_Character_Rate                                          |
    |-----------|---------------------------------------------------------------------------------------------------------------|
    |           |   SCDC_     |    RR_      |    Rsvd     |    Rsvd     |  LTE_340Mcsc| Independent |     Dual    |   3D_OSD_   |
    |    PB3    |   Present   |    Capable  |    (0)      |    (0)      |  _scramble  | _view       |     _View   |   Disparity |
    |-----------|---------------------------------------------------------------------------------------------------------------|
    |           |    Rsvd     |      Rsvd   |    Rsvd     |    Rsvd     |    Rsvd     |   DC_48Bit  |   DC_36Bit  |   DC_30Bit  |
    |    PB4    |    (0)      |      (0)    |    (0)      |    (0)      |    (0)      |   _420      |   _420      |   _420      |
    |-----------|---------------------------------------------------------------------------------------------------------------|
    |   ...N    |                                                   ...                                                         |
    |---------------------------------------------------------------------------------------------------------------------------|

** Sinks shall include only one SCDS, in HF-VSDB or HF-SCDB

                                        HDMI Forum Vendor Specific Data Block
    -----------------------------------------------------------------------------------------------------------------------------
    |Byte\Bit # |      7      |      6      |      5      |      4      |      3      |      2      |      1      |      0      |
    |-----------|---------------------------------------------------------------------------------------------------------------|
    |     0     |     Vendor-specific tag code (=3)       |                             Length (=N)                             |
    |-----------|---------------------------------------------------------------------------------------------------------------|
    |     1     |                                           IEEE OUI, Third Octet  (0xD8)                                       |
    |-----------|---------------------------------------------------------------------------------------------------------------|
    |     2     |                                           IEEE OUI, Second Octet (0x5D)                                       |
    |-----------|---------------------------------------------------------------------------------------------------------------|
    |     3     |                                           IEEE OUI, First Octet  (0xC4)                                       |
    |-----------|---------------------------------------------------------------------------------------------------------------|
    |   4..n    |                                                   ** SCDS                                                     |
    |-----------|---------------------------------------------------------------------------------------------------------------|


                                        HDMI Forum Sink Capability Data Block
    -----------------------------------------------------------------------------------------------------------------------------
    |Byte\Bit # |      7      |      6      |      5      |      4      |      3      |      2      |      1      |      0      |
    |-----------|---------------------------------------------------------------------------------------------------------------|
    |     0     |     Vendor-specific tag code (=7)       |                             Length (=N)                             |
    |-----------|---------------------------------------------------------------------------------------------------------------|
    |     1     |                                           Extended Tag Code (0x79)                                            |
    |-----------|---------------------------------------------------------------------------------------------------------------|
    |     2     |                                                   Reserved                                                    |
    |-----------|---------------------------------------------------------------------------------------------------------------|
    |     3     |                                                   Reserved                                                    |
    |-----------|---------------------------------------------------------------------------------------------------------------|
    |   4..n    |                                                   ** SCDS                                                     |
    |-----------|---------------------------------------------------------------------------------------------------------------|
*/

static CBIOS_U32 cbEDIDModule_ParseHFSCDS(CBIOS_U8 *pSCDSDataInEDID, PCBIOS_HF_SCDS_DATA pSCDSData)
{
    CBIOS_U8    *pCurByte = pSCDSDataInEDID;
    CBIOS_U32   PayloadLen = 0;

    if ((pSCDSDataInEDID == CBIOS_NULL) || (pSCDSData == CBIOS_NULL))
    {
        cbDebugPrint((MAKE_LEVEL(GENERIC, ERROR), "%s: NULL pointer!\n", FUNCTION_NAME));
        return 0;
    }

    cb_memset(pSCDSData, 0, sizeof(CBIOS_HF_SCDS_DATA));

    cb_memcpy(&(pSCDSData->Tag), pCurByte++, sizeof(pSCDSData->Tag));
    //byte 0 Tag and length
    if (pSCDSData->Tag.CTATag == VENDOR_SPECIFIC_DATA_BLOCK_TAG
        && pSCDSData->Tag.Length >= 7)
    {
        // HF-VSDB
        // byte 1-3, The IEEE Organizationally Unique Identifier (OUI) of C4-5D-D8
        pSCDSData->HFVSDBOUI.IEEEOUIByte0 = *(pCurByte++);
        pSCDSData->HFVSDBOUI.IEEEOUIByte1 = *(pCurByte++);
        pSCDSData->HFVSDBOUI.IEEEOUIByte2 = *(pCurByte++);
        PayloadLen += 3;
    }
    else if (pSCDSData->Tag.CTATag == CEA_EXTENDED_BLOCK_TAG
        && *pCurByte == HF_SINK_CAPABILITY_DATA_BLOCK
        && pSCDSData->Tag.Length >= 7)
    {
        // HF-SCDB
        // byte 1, Extended Tag Code
        pSCDSData->ExtTagCode = *(pCurByte++);
        // byte 2~3, reserved
        pCurByte += 2;
        PayloadLen += 3;
    }
    else
    {
        cbDebugPrint((MAKE_LEVEL(GENERIC, ERROR), "%s: no valid HF-VSDB or HF-SCDB data!\n", FUNCTION_NAME));
        pSCDSData->Tag.Length = 0;
    }

    if (pSCDSData->Tag.Length)  //tag ok
    {
        //byte 4  version
        pSCDSData->Version = *(pCurByte++);
        PayloadLen++;

        //byte 5  Max_TMDS_Character_Rate in MHz / 5
        pSCDSData->MaxTMDSCharacterRate = (CBIOS_U16)(*pCurByte) * 5;
        pCurByte++;
        PayloadLen++;

        //byte 6-7
        cb_memcpy(&(pSCDSData->SupportCaps), pCurByte, sizeof(CBIOS_U16));
        // Patch for Issue 21429, Issue21458: SCDCPresent is false that can't call cbHDMIMonitor_SCDC_Configure().
        if (pSCDSData->MaxTMDSCharacterRate > 340)
        {
            pSCDSData->IsSCDCPresent = CBIOS_TRUE;
        }
        pCurByte += 2;   
        PayloadLen += 2;
    }

    //check payload length
    if (PayloadLen != pSCDSData->Tag.Length)
    {
        cbDebugPrint((MAKE_LEVEL(GENERIC, WARNING), "%s: payload length error !\n", FUNCTION_NAME));
    }

    //block total length = payload len + 1
    return (pSCDSData->Tag.Length + 1);
}

static CBIOS_U32 cbEDIDModule_ParseAMDVSDB(CBIOS_U8 *pVSDBDataInEDID, PCBIOS_AMD_VSDB_EXTENTION pVSDBData)
{
    CBIOS_U8    *pCurByte = pVSDBDataInEDID;
    CBIOS_U32   PayloadLen = 0;

    if ((pVSDBDataInEDID == CBIOS_NULL) || (pVSDBData == CBIOS_NULL))
    {
        cbDebugPrint((MAKE_LEVEL(GENERIC, ERROR), "cbEDIDModule_ParseAMDVSDB: NULL pointer!\n"));
        return 0;
    }

    //Initialize CBIOS_AMD_VSDB_EXTENTION
    cb_memset(pVSDBData, 0, sizeof(CBIOS_AMD_VSDB_EXTENTION));

    //check Tag and length
    cb_memcpy(&(pVSDBData->Tag), pCurByte++, sizeof(pVSDBData->Tag));

    if ((pVSDBData->Tag.VSDBTag != VENDOR_SPECIFIC_DATA_BLOCK_TAG)
        ||(pVSDBData->Tag.VSDBLength < 8))
    {
        cbDebugPrint((MAKE_LEVEL(GENERIC, ERROR), "cbEDIDModule_ParseAMDVSDB: invalid VSDB data!\n"));
        pVSDBData->Tag.VSDBLength = 0;
    }
    else
    {
        PayloadLen = 0;
        //Byte 1-3, The IEEE Organizationlly Unique Identifier(OUI) of 1A-00-00
        pVSDBData->AMDVSDBOUI.IEEEOUIByte0 = *(pCurByte++);
        pVSDBData->AMDVSDBOUI.IEEEOUIByte1 = *(pCurByte++);
        pVSDBData->AMDVSDBOUI.IEEEOUIByte2 = *(pCurByte++);
        PayloadLen += 3;

        //Byte 4 version
        pVSDBData->Version = *(pCurByte++);
        PayloadLen++;

        //Byte 5 FreeSync capability
        pVSDBData->FreeSyncCaps = *(pCurByte++);
        PayloadLen++;

        //Byte 6 Min supported refresh rate
        pVSDBData->MinSupportedRefRate = *(pCurByte++);
        //Byte 7 Max supported refresh rate
        pVSDBData->MaxSupportedRefRate = *(pCurByte++);
        PayloadLen += 2;

        //Byte 8 FreeSync MCCS VCP Code
        pVSDBData->MCCSVcpCode = *(pCurByte++);
        PayloadLen++;
    }

    if (PayloadLen != pVSDBData->Tag.VSDBLength)
    {
        cbDebugPrint((MAKE_LEVEL(GENERIC, WARNING), "cbEDIDModule_ParseAMDVSDB: payload length error!\n"));
    }

    return (pVSDBData->Tag.VSDBLength + 1);
}

/***************************************************************
Function:    cbEDIDModule_ParseCEAExtBlock

Description: Parse extended data block of CEA extension

Input:       pExtBlockDataInEDID, extended block data buffer            

Output:      pEDIDStruct, decoded extend block data

Return:      the total length of vendor specific data block
***************************************************************/
static CBIOS_U32 cbEDIDModule_ParseCEAExtBlock(CBIOS_U8 *pExtBlockDataInEDID, PCBIOS_EDID_STRUCTURE_DATA pEDIDStruct, CBIOS_U32 BlockIndex)
{
    CBIOS_U32   PayloadLen = 0;
    CBIOS_U8    ExtTagCode = 0;
    CBIOS_U8    FormatVIC = 0;
    PCBIOS_CEA_SVD_DATA pSVDData = CBIOS_NULL;
    PCBIOS_CEA_EXTENED_BLOCK pCEAExtData = CBIOS_NULL;
    CBIOS_U8    SVD = 0;
    CBIOS_BOOL  IsNative = CBIOS_FALSE;
    CBIOS_BOOL  Status = CBIOS_FALSE;
    CBIOS_U8    i = 0, j = 0;

    pSVDData = &pEDIDStruct->Attribute.SVDData[BlockIndex - 1];

    //check tag code
    if (((((*pExtBlockDataInEDID) >> 5) & 0x07) != CEA_EXTENDED_BLOCK_TAG)
        || (((*pExtBlockDataInEDID) & 0x1F) == 0))
    {
        PayloadLen = 0;
    }
    else
    {
        PayloadLen = (*pExtBlockDataInEDID) & 0x1F;
        ExtTagCode = pExtBlockDataInEDID[1];

        if(ExtTagCode == VIDEO_CAPABILITY_DATA_BLOCK_TAG)
        {
            //Video Capability Data Block
            pCEAExtData = &pEDIDStruct->Attribute.ExtDataBlock[VIDEO_CAPABILITY_DATA_BLOCK_TAG];
            cb_memcpy(&pCEAExtData->VideoCapabilityData, &pExtBlockDataInEDID[2], sizeof(CBIOS_VIDEO_CAPABILITY_DATA));
        }
        else if(ExtTagCode == VENDOR_SPECIFIC_VIDEO_DATA_BLOCK_TAG)
        {
            //Vendor Specific Video Data Block
        }
        else if(ExtTagCode == COLORIMETRY_DATA_BLOCK_TAG)
        {
            //Colorimetry Data Block
            pCEAExtData = &pEDIDStruct->Attribute.ExtDataBlock[COLORIMETRY_DATA_BLOCK_TAG];
            cb_memcpy(&pCEAExtData->ColorimetryData, &pExtBlockDataInEDID[2], sizeof(CBIOS_COLORIMETRY_DATA));
        }
        else if(ExtTagCode == HDR_STATIC_META_DATA_BLOCK)
        {
            pCEAExtData = &pEDIDStruct->Attribute.ExtDataBlock[HDR_STATIC_META_DATA_BLOCK];
            
            if(PayloadLen > 6)
            {
                cbDebugPrint((MAKE_LEVEL(GENERIC, ERROR), "%s: static meta data block length error\n", FUNCTION_NAME));
            }
            else
            {
                cb_memcpy(&pCEAExtData->HDRStaticMetaData, &pExtBlockDataInEDID[2], PayloadLen - 1);
            }
        }
        else if(ExtTagCode == VIDEO_FMT_PREFERENCE_DATA_BLOCK)
        {
            /*
                Video Format Preference Data Block
                VFPDB indicates the order of preference for(selected)Video Formats listed as DTDs and/or 
                SVDs throughout the entire EDID. When present, the VFPDB shall take precedence over preferred 
                indications defined elsewhere in CEA-861-F.
            */
            CBIOS_U8 SVR = 0;
            for (i = 0; i < PayloadLen - 1; i++)
            {
                SVR = pExtBlockDataInEDID[2 + i];
                if(((SVR >= 1) && (SVR <= 127)) || ((SVR >= 193) && (SVR <= 253)))
                {
                    //Interpret as a VIC
                    pEDIDStruct->Attribute.ShortVideoRef[i].SVRValue = SVR;
                    pEDIDStruct->Attribute.ShortVideoRef[i].SVRFlag = SVR_FLAG_VIC;
                }
                else if((SVR >= 129) && (SVR <= 144))
                {
                    //Interpret as the Kth DTD in the EDID, where K = SVR -128
                    pEDIDStruct->Attribute.ShortVideoRef[i].SVRValue = SVR - 128;
                    pEDIDStruct->Attribute.ShortVideoRef[i].SVRFlag = SVR_FLAG_DTD_INDEX;
                }
                else
                {
                    cbDebugPrint((MAKE_LEVEL(GENERIC,INFO), "Reserved SVR!\n"));
                }
            }
        }
        else if(ExtTagCode == YCBCR420_VIDEO_DATA_BLOCK)
        {
            /*
                According to CEA-861-F 7.5.10
                A YCBCR4:2:0 Video Data Block (Y420VDB) lists Video Formats, supported by the Sink, that only allow 
                YCBCR4:2:0 sampling mode (i.e., do not support RGB, YCBCR4:4:4, or YCBCR4:2:2 sampling modes). 
            */
            CBIOS_U32  EntryIndex;
            for (i = 0; i < PayloadLen - 1; i++)
            {
                SVD = pExtBlockDataInEDID[2 + i];
                Status = cbEDIDModule_GetFmtIdxFromSVD(SVD, &FormatVIC, &IsNative);
                if (!Status)
                {
                    continue;
                }

                if(cbEDIDModule_AddSvdFormat(pEDIDStruct, FormatVIC, IsNative, &EntryIndex))
                {
                    pEDIDStruct->pDeviceTimingList[EntryIndex].IsSupportYCbCr420 = 1;
                    pEDIDStruct->pDeviceTimingList[EntryIndex].IsSupportOtherFormats = 0;
                }
            }
        }
        else if(ExtTagCode == YCBCR420_CAP_MAP_DATA_BLOCK)
        {
            /*
                According to CEA-861-F 7.5.11
                A YCBCR4:2:0 Capability Map Data Block (Y420CMDB) indicates exactly which SVDs, listed in one or 
                more regular Video Data Blocks (see Section 7.5.1), also support YCBCR4:2:0 sampling mode - in 
                addition to other modes such as RGB, YCBCR4:4:4, and/or YCBCR4:2:2. The Y420CMDB does not indicate which RGB, 
                YCBCR 4:4:4, and/or YCBCR4:2:2 modes are supported. 
            */
            CBIOS_U32  EntryIndex;
            CBIOS_U8 YCbCr420CapMap = 0;
            CBIOS_U8 Step = 0;

            if (PayloadLen == 1)
            {
                // payload length == 1, all regular SVDs supports YCbCr420
                for (i = 0; i < pSVDData->SVDCount; i++)
                {
                    SVD = pSVDData->SVD[i];
                    Status = cbEDIDModule_GetFmtIdxFromSVD(SVD, &FormatVIC, &IsNative);
                    if (!Status)
                    {
                        continue;
                    }

                    if(cbEDIDModule_isSvdFormatExist(pEDIDStruct, FormatVIC, &EntryIndex))
                    {
                        pEDIDStruct->pDeviceTimingList[EntryIndex].IsSupportYCbCr420 = 1;
                        pEDIDStruct->pDeviceTimingList[EntryIndex].IsSupportOtherFormats = 1;
                    }
                }
            }
            else
            {
                for (i = 0; i < PayloadLen - 1; i++)
                {
                    YCbCr420CapMap = pExtBlockDataInEDID[2 + i];

                    for (j = 0; j < 8; j++)
                    {
                        if (((YCbCr420CapMap >> j) & 0x1) && (j + Step < pSVDData->SVDCount))
                        {
                            SVD = pSVDData->SVD[j+Step];
                            Status = cbEDIDModule_GetFmtIdxFromSVD(SVD, &FormatVIC, &IsNative);
                            if (!Status)
                            {
                                continue;
                            }

                            if(cbEDIDModule_isSvdFormatExist(pEDIDStruct, FormatVIC, &EntryIndex))
                            {
                                pEDIDStruct->pDeviceTimingList[EntryIndex].IsSupportYCbCr420 = 1;
                                pEDIDStruct->pDeviceTimingList[EntryIndex].IsSupportOtherFormats = 1;
                            }
                        }
                    }
                    Step += 8;
                }
            }
        }
        else if(ExtTagCode == VENDOR_SPECIFIC_AUDIO_DATA_BLOCK_TAG)
        {
            //Vendor Specific Audio Data Block
        }
        else if(ExtTagCode == RSVD_HDMI_AUDIO_DATA_BLOCK_TAG)
        {
            //HDMI Audio Data Block
            pCEAExtData = &pEDIDStruct->Attribute.ExtDataBlock[RSVD_HDMI_AUDIO_DATA_BLOCK_TAG];
            
            pCEAExtData->HDMIAudioData.MaxStreamCount = pExtBlockDataInEDID[2] & 0x3;
            pCEAExtData->HDMIAudioData.SupportsMSNonMixed = (pExtBlockDataInEDID[2] >> 2) & 0x1;
            pCEAExtData->HDMIAudioData.NumHDMI3DAD = pExtBlockDataInEDID[3] & 0x7;
            
            if(pCEAExtData->HDMIAudioData.NumHDMI3DAD > 0)
            {
                CBIOS_U32   AudioFormatCode = 0;
                for(i = 0;i < pCEAExtData->HDMIAudioData.NumHDMI3DAD;i++)
                {
                    AudioFormatCode = pExtBlockDataInEDID[4 + i*4] & 0xF;
                    pCEAExtData->HDMIAudioData.HDMI3DAudioDesc[i].Format = AudioFormatCode;
                    pCEAExtData->HDMIAudioData.HDMI3DAudioDesc[i].MaxChannelNum = (pExtBlockDataInEDID[4 + i*4 + 1] & 0x1F) + 1;
                    pCEAExtData->HDMIAudioData.HDMI3DAudioDesc[i].SampleRateUnit = pExtBlockDataInEDID[4 + i*4 + 2] & 0x7F;

                    if (AudioFormatCode == 1)
                    {
                        pCEAExtData->HDMIAudioData.HDMI3DAudioDesc[i].BitDepth.BD_16bit = pExtBlockDataInEDID[4 + i*4 + 3] & 0x1;
                        pCEAExtData->HDMIAudioData.HDMI3DAudioDesc[i].BitDepth.BD_20bit = (pExtBlockDataInEDID[4 + i*4 + 3] >> 1) & 0x1;
                        pCEAExtData->HDMIAudioData.HDMI3DAudioDesc[i].BitDepth.BD_24bit = (pExtBlockDataInEDID[4 + i*4 + 3] >> 2) & 0x1;
                    }
                    else if (AudioFormatCode <= 8)
                    {
                        pCEAExtData->HDMIAudioData.HDMI3DAudioDesc[i].MaxBitRate = pExtBlockDataInEDID[4 + i*4 + 3] * 8;
                    }
                    else if (AudioFormatCode <= 13)
                    {
                        pCEAExtData->HDMIAudioData.HDMI3DAudioDesc[i].AudioFormatDependValue = pExtBlockDataInEDID[4 + i*4 + 3];
                    }
                    else if (AudioFormatCode == 14)
                    {
                        pCEAExtData->HDMIAudioData.HDMI3DAudioDesc[i].Profile.Value = pExtBlockDataInEDID[4 + i*4 + 3] & 0x7;
                    }
                    else
                    {
                        if (((pExtBlockDataInEDID[4 + i*4 + 3] >> 3) & 0x1F) == 1)
                        {
                            pCEAExtData->HDMIAudioData.HDMI3DAudioDesc[i].Format = CBIOS_AUDIO_FORMAT_HE_AAC;
                        }
                        else if(((pExtBlockDataInEDID[4 + i*4 + 3] >> 3) & 0x1F) == 2)
                        {
                            pCEAExtData->HDMIAudioData.HDMI3DAudioDesc[i].Format = CBIOS_AUDIO_FORMAT_HE_AAC_V2;
                        }
                        else if(((pExtBlockDataInEDID[4 + i*4 + 3] >> 3) & 0x1F) == 3)
                        {
                            pCEAExtData->HDMIAudioData.HDMI3DAudioDesc[i].Format = CBIOS_AUDIO_FORMAT_MPEG_SURROUND;
                        }
                    }
                }
            }
        }
        else if(ExtTagCode == INFOFRAME_DATA_BLOCK)
        {
            //InfoFrame Data Block
            CBIOS_U8 Index = 2; 
            CBIOS_U8 TypeCode = 0;
            CBIOS_U8 SIDHeader = 0;
            CBIOS_U8 SupportPriority = 0;
            
            //InfoFrame Processing Descriptor
            CBIOS_U8 Len = (pExtBlockDataInEDID[Index++] >> 5) & 0x7;
            pEDIDStruct->Attribute.NumOfAdditionalVSIFs = pExtBlockDataInEDID[Index++];
            Index += Len;

            /*
             Declared InfoFrame types shall be listed in order of priority; 
             meaning that the first is the one that the display manufacturer 
             has identified as most desirable. Sources may use this infomation 
             as a basis for selecting which InfoFrame to send(e.g. in case where 
             the Sources may not be capable of delivering all defined or 
             supported InfoFrame types).
            */
            
            for(;Index < PayloadLen;)
            {
                SIDHeader = pExtBlockDataInEDID[Index++];
                Len = (SIDHeader >> 5) & 0x7;
                TypeCode = SIDHeader & 0x1F;

                if(TypeCode == VENDOR_SPECIFIC_INFO_FRAME_TYPE)
                {
                    if((pExtBlockDataInEDID[Index + 1] == 0x03) && 
                       (pExtBlockDataInEDID[Index + 2] == 0x0c) && 
                       (pExtBlockDataInEDID[Index + 3] == 0x00))
                    {
                        pEDIDStruct->Attribute.InfoFrameSupCaps.HDMISpecificInfoFrameCaps.bSupport = CBIOS_TRUE;
                        pEDIDStruct->Attribute.InfoFrameSupCaps.HDMISpecificInfoFrameCaps.Priority = SupportPriority;
                    }
                    else if((pExtBlockDataInEDID[Index + 1] == 0xD8) && 
                       (pExtBlockDataInEDID[Index + 2] == 0x5D) && 
                       (pExtBlockDataInEDID[Index + 3] == 0xC4))
                    {
                        pEDIDStruct->Attribute.InfoFrameSupCaps.HFSpecificInfoFrameCaps.bSupport = CBIOS_TRUE;
                        pEDIDStruct->Attribute.InfoFrameSupCaps.HFSpecificInfoFrameCaps.Priority = SupportPriority;
                    }

                    Index += 3;//24-bit OUI Registration Identifier
                }
                else if(TypeCode == AVI_INFO_FRAME_TYPE)
                {
                    pEDIDStruct->Attribute.InfoFrameSupCaps.AVIInfoFrameCaps.bSupport = CBIOS_TRUE;
                    pEDIDStruct->Attribute.InfoFrameSupCaps.AVIInfoFrameCaps.Priority = SupportPriority;
                }
                else if(TypeCode == SOURCE_PRODUCT_DESCRIPTION_INFO_FRAME_TYPE)
                {
                    pEDIDStruct->Attribute.InfoFrameSupCaps.SourceProductDescInfoFrameCaps.bSupport = CBIOS_TRUE;
                    pEDIDStruct->Attribute.InfoFrameSupCaps.SourceProductDescInfoFrameCaps.Priority = SupportPriority;
                }
                else if(TypeCode == AUDIO_INFO_FRAME_TYPE)
                {
                    pEDIDStruct->Attribute.InfoFrameSupCaps.AudioInfoFrameCaps.bSupport = CBIOS_TRUE;
                    pEDIDStruct->Attribute.InfoFrameSupCaps.AudioInfoFrameCaps.Priority = SupportPriority;
                }
                else if(TypeCode == MPEG_SOURCE_INFO_FRAME_TYPE)
                {
                    pEDIDStruct->Attribute.InfoFrameSupCaps.MpegSourceInfoFrameCaps.bSupport = CBIOS_TRUE;
                    pEDIDStruct->Attribute.InfoFrameSupCaps.MpegSourceInfoFrameCaps.Priority = SupportPriority;
                }
                else if(TypeCode == NTSC_VBI_INFO_FRAME_TYPE)
                {
                    pEDIDStruct->Attribute.InfoFrameSupCaps.NTSCVBIInfoFrameCaps.bSupport = CBIOS_TRUE;
                    pEDIDStruct->Attribute.InfoFrameSupCaps.NTSCVBIInfoFrameCaps.Priority = SupportPriority;
                }

                SupportPriority++;
                Index += Len;
            }
        }
        else if(ExtTagCode == HF_EDID_EXTENSION_OVERRIDE_DATA_BLOCK)
        {
            // HF-EEODB for ext block num, already parsed by cbEDIDModule_GetExtBlockNum
            PayloadLen = (*pExtBlockDataInEDID) & 0x1F;
        }
        else if(ExtTagCode == HF_SINK_CAPABILITY_DATA_BLOCK)
        {
            PayloadLen = cbEDIDModule_ParseHFSCDS(pExtBlockDataInEDID, &(pEDIDStruct->Attribute.HFSCDSData)) - 1;
        }
        else
        {
            cbDebugPrint((MAKE_LEVEL(GENERIC, ERROR), "cbEDIDModule_ParseCEAExtBlock: ExtTagCode = 0x%x which is not parsed yet!\n", ExtTagCode));
        }
    }

    //return total block len
    return PayloadLen + 1;
}

/***************************************************************
Function:    cbEDIDModule_GetHDMIVICMode

Description: Get HDMI VIC mode number in VSDB

Input:       pVSDBData, VSDB data buffer

Output:      pCEAVideoFormat, statistic HDMI VIC format in VSDB

Return:      the number of HDMI VIC mode
***************************************************************/
static CBIOS_VOID cbEDIDModule_GetHDMIVICMode(PCBIOS_EDID_STRUCTURE_DATA pEdidStruct)
{
    PCBIOS_HDMI_VSDB_EXTENTION pVSDBData = &pEdidStruct->Attribute.VSDBData;
    CBIOS_U16 FormatVIC = 0;
    CBIOS_U32  i = 0;

    //extended VICs
    for (i = 0; i < pVSDBData->HDMIVICLen; i++)
    {
        FormatVIC = pVSDBData->HDMIVIC[i] + CBIOS_HDMI_NORMAL_VIC_COUNTS;  

        cbEDIDModule_AddSvdFormat(pEdidStruct, FormatVIC, CBIOS_FALSE, CBIOS_NULL); 
    } 
}

static CBIOS_U32 cbEDIDModule_GetDisplayIDTiming(CBIOS_U8 *pData, PCBIOS_EDID_STRUCTURE_DATA pEDIDStruct)
{
    CBIOS_U32   i = 0, PayloadLen = 0, ulNumOfModes = 0;
    CBIOS_U32   DescriptorLen = DID_TYPE1_TIMING_DESCRIPTOR_LENGTH;
    CBIOS_U16  SyncOffset = 0, SyncWidth = 0, Blank = 0;
    CBIOS_U8 HSync = HorNEGATIVE, VSync = VerNEGATIVE;
    CBIOS_U8    BlockRevision = pData[1] & 7;
    CBIOS_U8    SteroSupport = 0;
    CBIOS_DEVICE_TIMING_INFO DeviceTiming = {0};

    PayloadLen = pData[2];
    if (pData[0] == VIDEO_TIMING_MODES_DATA_BLOCK2_TYPE7_TAG)
    {
        if (BlockRevision == 1 || BlockRevision == 2)
        {
            DescriptorLen += (pData[1] & 0x70) >> 4;
        }
    }

    if(!DescriptorLen)
    {
        return ulNumOfModes;
    }

    for (i = 0; i < PayloadLen/DescriptorLen; i++)
    {
        cb_memset(&DeviceTiming, 0, sizeof(DeviceTiming));

        DeviceTiming.TimingType = CBIOS_DISP_ID_TIMING;
        DeviceTiming.FullTiming.FormatVIC = 0;
        DeviceTiming.FullTiming.Size = TIMING_ATTRIB_SIZE;
            
        DeviceTiming.FullTiming.PixelClock = ((CBIOS_U32)pData[5 + i*DescriptorLen]) << 16;
        DeviceTiming.FullTiming.PixelClock += ((CBIOS_U32)pData[4 + i*DescriptorLen]) << 8;
        DeviceTiming.FullTiming.PixelClock += (CBIOS_U32)pData[3 + i*DescriptorLen] + 1;
        if (pData[0] == VIDEO_TIMING_MODES_DATA_BLOCK2_TYPE7_TAG)
        {
            DeviceTiming.FullTiming.PixelClock *= 10;
        }
        else
        {
            DeviceTiming.FullTiming.PixelClock *= 100;
        }

        DeviceTiming.FullTiming.XRes = (((CBIOS_U16)pData[8 + i*DescriptorLen]) << 8) + pData[7 + i*DescriptorLen] + 1;
        Blank = (((CBIOS_U16)pData[10 + i*DescriptorLen]) << 8) + pData[9 + i*DescriptorLen] + 1;
        SyncOffset= (((CBIOS_U16)pData[12 + i*DescriptorLen] & 0x7F) << 8) + pData[11 + i*DescriptorLen] + 1;
        SyncWidth = (((CBIOS_U16)pData[14 + i*DescriptorLen]) << 8) + pData[13 + i*DescriptorLen] + 1;
        DeviceTiming.FullTiming.HorTotal = DeviceTiming.FullTiming.XRes + Blank;
        DeviceTiming.FullTiming.HorDisEnd = DeviceTiming.FullTiming.XRes;    
        DeviceTiming.FullTiming.HorBStart = DeviceTiming.FullTiming.XRes;     
        DeviceTiming.FullTiming.HorBEnd = DeviceTiming.FullTiming.XRes + Blank;     
        DeviceTiming.FullTiming.HorSyncStart = DeviceTiming.FullTiming.XRes + SyncOffset;    
        DeviceTiming.FullTiming.HorSyncEnd = DeviceTiming.FullTiming.HorSyncStart + SyncWidth;  
        
        HSync = (pData[12 + i*DescriptorLen] & 0x80) ? HorPOSITIVE : HorNEGATIVE;
        
        DeviceTiming.FullTiming.YRes = (((CBIOS_U16)pData[16 + i*DescriptorLen]) << 8) + pData[15 + i*DescriptorLen] + 1;
        Blank = (((CBIOS_U16)pData[18 + i*DescriptorLen]) << 8) + pData[17 + i*DescriptorLen] + 1;
        SyncOffset = (((CBIOS_U16)pData[20 + i*DescriptorLen] & 0x7F) << 8) + pData[19 + i*DescriptorLen] + 1;
        SyncWidth = (((CBIOS_U16)pData[22 + i*DescriptorLen]) << 8) + pData[21 + i*DescriptorLen] + 1;
        DeviceTiming.FullTiming.VerTotal = DeviceTiming.FullTiming.YRes + Blank;    
        DeviceTiming.FullTiming.VerDisEnd = DeviceTiming.FullTiming.YRes;    
        DeviceTiming.FullTiming.VerBStart = DeviceTiming.FullTiming.YRes;        
        DeviceTiming.FullTiming.VerBEnd = DeviceTiming.FullTiming.YRes + Blank;
        DeviceTiming.FullTiming.VerSyncStart = DeviceTiming.FullTiming.YRes + SyncOffset;  
        DeviceTiming.FullTiming.VerSyncEnd = DeviceTiming.FullTiming.VerSyncStart+SyncWidth;
        
        VSync = (pData[20 + i*DescriptorLen] & 0x80) ? VerPOSITIVE : VerNEGATIVE;
        DeviceTiming.IsInterLaced = (pData[6 + i*DescriptorLen] & 0x10) ? 1 : 0;

        DeviceTiming.FullTiming.HVPolarity = HSync + VSync;

        if((DeviceTiming.FullTiming.XRes == 0 ) || (DeviceTiming.FullTiming.YRes == 0))
        {
            continue;
        }

        DeviceTiming.FullTiming.RefreshRate = cbCalcRefreshRate(DeviceTiming.FullTiming.PixelClock,
                                                                                DeviceTiming.FullTiming.HorTotal,
                                                                                DeviceTiming.FullTiming.VerTotal);
        
        DeviceTiming.FullTiming.RefreshRate = 100 * cbRound(DeviceTiming.FullTiming.RefreshRate, 100, ROUND_NEAREST);

        if (BlockRevision < 2)
        {
            if (pData[6 + i*DescriptorLen] & 0x80)
            {
                DeviceTiming.IsPreferMode = 1;
            }
        }
        else if (BlockRevision == 2)
        {
            if (pData[6 + i*DescriptorLen] & 0x80)
            {
                DeviceTiming.IsSupportYCbCr420 = 1;
            }
        }
        SteroSupport = (pData[6 + i*DescriptorLen] & 0x60) >> 5;
        DeviceTiming.AspectRatio = pData[6 + i*DescriptorLen] & 0xF;

        if (SteroSupport == 0)
        {
            DeviceTiming.IsSupportMono = 1;
        }
        else if (SteroSupport == 1)
        {
            DeviceTiming.IsSupportStereo = 1;
        }
        else if (SteroSupport == 2)
        {
            DeviceTiming.IsSupportMono = 1;
            DeviceTiming.IsSupportStereo = 1;
        }

        if (!cbEDIDModule_ValidateTiming(pEDIDStruct, &DeviceTiming))
        {
            continue;
        }

        if(cbEDIDModule_AddDeviceTiming(pEDIDStruct, &DeviceTiming))
        {
            ulNumOfModes++;
        }
    }
    
    return ulNumOfModes;
}

CBIOS_U32 cbEDIDModule_GetExtBlockNum(CBIOS_U8 *pEDID)
{
    CBIOS_U32 ExtBlockNum = 0;

    if (!pEDID)
    {
        cbDebugPrint((MAKE_LEVEL(GENERIC, ERROR), "%s: invalid EDID\n", FUNCTION_NAME));
        return 0;
    }

    if (!pEDID[0x7E])  // Extension Flag
    {
        ExtBlockNum = 0;
    }
    else
    {
        if ((pEDID[0x84] >> 5 == CEA_EXTENDED_BLOCK_TAG) &&
            (pEDID[0x85] == HF_EDID_EXTENSION_OVERRIDE_DATA_BLOCK))  // be HF-EEODB
        {
            ExtBlockNum = pEDID[0x86];
        }
        else
        {
            ExtBlockNum = pEDID[0x7E];
        }
    }

    return ExtBlockNum;
}

CBIOS_U32 cbEDIDModule_GetTotalBlockNum(CBIOS_EDID_DATA *pEdidData)
{
    if (!pEdidData || !pEdidData->Buffer)
    {
        return 0;
    }

    return (pEdidData->BufferSize / EDID_BLK_SIZE);
}

/***************************************************************
Function:    cbEDIDModule_GetCEADetailedMode

Description: 

Input:       pEDID, EDID data buffer

Output:      pEDIDStruct, store decoded CEA data 

Return:      the number of mode get in CEA extension
***************************************************************/
static CBIOS_U32 cbEDIDModule_GetCEA861Mode(CBIOS_EDID_DATA *pEdidData, PCBIOS_EDID_STRUCTURE_DATA pEDIDStruct)
{
    CBIOS_U32   TotalBlocks = 0, BlockIndex = 0;
    CBIOS_U32   ModeNumOld = pEDIDStruct->TimingNum;
    CBIOS_U8    *pEDIDBlock = CBIOS_NULL;
    CBIOS_U8    DtlTimingOffset = 0, PayloadLen = 0, TagCode = 0, i = 0;

    TotalBlocks = cbEDIDModule_GetTotalBlockNum(pEdidData);

    //parse extension blocks
    for (BlockIndex = 1; BlockIndex < TotalBlocks; BlockIndex++)
    {
        pEDIDBlock = pEdidData->Buffer + BlockIndex * EDID_BLK_SIZE;

        //check CEA Tag
        if (pEDIDBlock[0x00] != CEA_TAG)
        {
            continue;
        }

        // Check CEA861 Version.
        if (pEDIDBlock[0x01] == 0x00)
        {
            cbDebugPrint((MAKE_LEVEL(GENERIC, WARNING), "EDID block%d indicates invalid CEA861 block version!\n", BlockIndex));
        }

        DtlTimingOffset = pEDIDBlock[0x02];
        if (DtlTimingOffset == 0 || DtlTimingOffset >= EDID_BLK_SIZE)
        {
            continue;
        }

        cbEDIDModule_GetCEADetailedMode(pEDIDBlock, DtlTimingOffset, pEDIDStruct);

        //parse data blocks
        for (i = 4; i < DtlTimingOffset;)
        {
            TagCode = (pEDIDBlock[i] >> 5) & 0x07;
            PayloadLen = pEDIDBlock[i] & 0x1F;
            
            if (TagCode == AUDIO_DATA_BLOCK_TAG)
            {
                cbEDIDModule_GetHDMIAudioFormat(&pEDIDBlock[i], pEDIDStruct);
            }        
            else if (TagCode == VIDEO_DATA_BLOCK_TAG)
            {
                //decode short video descriptor
                cbEDIDModule_GetSVDMode(&pEDIDBlock[i], pEDIDStruct, BlockIndex);
            }
            else if (TagCode == VENDOR_SPECIFIC_DATA_BLOCK_TAG)//now consider HDMI VSDB and HDMI Forum VSDB only, ignore other VSDBs
            {
                if((pEDIDBlock[i + 1] == 0x03) && (pEDIDBlock[i + 2] == 0x0c) && (pEDIDBlock[i + 3] == 0x00))
                {
                    cbEDIDModule_ParseHDMIVSDB(&pEDIDBlock[i], &(pEDIDStruct->Attribute.VSDBData));
                }
                else if((pEDIDBlock[i + 1] == 0xD8) && (pEDIDBlock[i + 2] == 0x5D) && (pEDIDBlock[i + 3] == 0xC4))
                {
                    cbEDIDModule_ParseHFSCDS(&pEDIDBlock[i], &(pEDIDStruct->Attribute.HFSCDSData));
                }
#if 0
                //Because freesync still has some problems, disable this function.
                else if((pEDIDBlock[i + 1] == 0x1A) && (pEDIDBlock[i + 2] == 0x00) && (pEDIDBlock[i + 3] == 0x00))
                {
                    i += cbEDIDModule_ParseAMDVSDB(&pEDIDBlock[i], &(pEDIDStruct->Attribute.AMDVSDBData));
                }
#endif          
            }
            else if (TagCode == CEA_EXTENDED_BLOCK_TAG)
            {
                cbEDIDModule_ParseCEAExtBlock(&pEDIDBlock[i], pEDIDStruct, BlockIndex);
            }

            i += PayloadLen + 1;
        }
    }
    
    //some monitor's AUDIO_DATA_BLOCK not support LPCM,but it indicates support basic audio, should patch here
    cbEDIDPatchHDMIAudio(pEDIDStruct);

    // get HDMI VIC mode
    cbEDIDModule_GetHDMIVICMode(pEDIDStruct);

    return (pEDIDStruct->TimingNum - ModeNumOld);
}

static CBIOS_U32 cbEDIDModule_GetDisplayIDMode(CBIOS_EDID_DATA *pEdidData, PCBIOS_EDID_STRUCTURE_DATA pEDIDStruct)
{
    CBIOS_U32   TotalBlocks = 0, BlockIndex = 0;
    CBIOS_U8    *pEDIDBlock = CBIOS_NULL;
    CBIOS_U32   PayloadLength = 0;
    CBIOS_U32   i = 0;
    CBIOS_U32   ulModeNum = 0;

    TotalBlocks = cbEDIDModule_GetTotalBlockNum(pEdidData);

    //parse extension blocks
    for (BlockIndex = 1; BlockIndex < TotalBlocks; BlockIndex++)
    {
        pEDIDBlock = pEdidData->Buffer + BlockIndex * 128;
        //check DisplayID Tag
        if (pEDIDBlock[0x00] != DISPLAYID_TAG)
        {
            continue;
        }
        // check Section Size
        if(pEDIDBlock[2] == 0)
        {
            continue;
        }

        for(i = 5; i < 126;)
        {
            if(pEDIDBlock[i] == VIDEO_TIMING_MODES_DATA_BLOCK_TYPE1_TAG || 
                pEDIDBlock[i] == VIDEO_TIMING_MODES_DATA_BLOCK2_TYPE7_TAG)
            {
                ulModeNum += cbEDIDModule_GetDisplayIDTiming(&pEDIDBlock[i], pEDIDStruct);
            }
            else
            {
                //TODO for other block
            }
            PayloadLength = pEDIDBlock[i+2];
            i += PayloadLength + 3;
        }
    }
    
    return ulModeNum;
}

/***************************************************************
Function:    cbEDIDModule_GetMonitorSize

Description: Get monitor image size.

Input:       pEDID, EDID data buffer

Output:       

Return:      Monitor DPI
***************************************************************/
static CBIOS_VOID cbEDIDModule_GetMonitorSize(CBIOS_U8 *pEDID, PCBIOS_MONITOR_MISC_ATTRIB pMonitorAttrib)
{
    CBIOS_U16   HImageSize = 0, VImageSize = 0;
    CBIOS_U8   *pDtlTimingsInBaseEDID = pEDID + DETAILED_TIMINGS_INDEX;

    HImageSize  = (CBIOS_U16)cbEDIDModule_MapMaskGetEdidInfo(pDtlTimingsInBaseEDID,0,EDIDHorizontalImageSize);
    VImageSize  = (CBIOS_U16)cbEDIDModule_MapMaskGetEdidInfo(pDtlTimingsInBaseEDID,0,EDIDVerticalImageSize); 
    if ((HImageSize != 0) && (VImageSize != 0))
    {
        pMonitorAttrib->MonitorHorSize = HImageSize;    // monitor screen image horizontal size in millimeter(mm)
        pMonitorAttrib->MonitorVerSize = VImageSize;    // monitor screen image vertical size in millimeter(mm)
    }
    else
    {
        cbDebugPrint((MAKE_LEVEL(GENERIC, ERROR),"cbEDIDModule_GetMonitorDPI: error -- monitor image size is zero!\n")); 
    }

    return;

}

/***************************************************************
Function:    cbEDIDModule_GetChromaticity

Description: Get monitor display x, y chromaticity coordinates and default white point.

Input:       pEDID, EDID data buffer

Output:       

Return:      chromaticity coordinates and default white point
***************************************************************/
static CBIOS_VOID cbEDIDModule_GetChromaticity(CBIOS_U8 *pEDID, PCBIOS_MONITOR_CHROMATICITY pChromaticity)
{
    pChromaticity->Red_X = (CBIOS_U16)pEDID[0x1B] << 2 | ((pEDID[0x19] & 0xC0) >> 6);
    pChromaticity->Red_Y = (CBIOS_U16)pEDID[0x1C] << 2 | ((pEDID[0x19] & 0x30) >> 4);
    pChromaticity->Green_X = (CBIOS_U16)pEDID[0x1D] << 2 | ((pEDID[0x19] & 0x0C) >> 2);
    pChromaticity->Green_Y = (CBIOS_U16)pEDID[0x1E] << 2 | (pEDID[0x19] & 0x03);
    pChromaticity->Blue_X = (CBIOS_U16)pEDID[0x1F] << 2 | ((pEDID[0x1A] & 0xC0) >> 6);
    pChromaticity->Blue_Y = (CBIOS_U16)pEDID[0x20] << 2 | ((pEDID[0x1A] & 0x30) >> 4);
    pChromaticity->White_X = (CBIOS_U16)pEDID[0x21] << 2 | ((pEDID[0x1A] & 0x0C) >> 2);
    pChromaticity->White_Y = (CBIOS_U16)pEDID[0x22] << 2 | (pEDID[0x1A] & 0x03);

    return;
}

/***************************************************************
Function:    cbEDIDModule_GetMonitorAttrib

Description: Get monitor attribute

Input:       pEDID, EDID data buffer

Output:      pMonitorAttrib, store decoded monitor attributes

Return:      CEA861 MonitorCaps
***************************************************************/
CBIOS_U32 cbEDIDModule_GetMonitorAttrib(CBIOS_EDID_DATA *pEdidData, PCBIOS_MONITOR_MISC_ATTRIB pMonitorAttrib)
{
    CBIOS_BOOL  bIsHDMIDevice = CBIOS_FALSE;
    CBIOS_U32   TotalBlocks = 0, BlockIndex = 0;
    CBIOS_U8    *pEDIDBlock = CBIOS_NULL;
    CBIOS_U32   i = 0, j = 0;
    CBIOS_U8    *pEDID = CBIOS_NULL;
    CBIOS_U8    *pDtlTimingsInBaseEDID = CBIOS_NULL;
    CBIOS_U8    SADCnt = 0;

    if(!cbEDIDModule_IsEDIDValid(pEdidData))
    {
        cbDebugPrint((MAKE_LEVEL(GENERIC, ERROR), "%s: Not valid EDID!\n", FUNCTION_NAME));        
        return CBIOS_ER_EDID_INVALID;
    }

    pEDID = pEdidData->Buffer;
    pDtlTimingsInBaseEDID = pEDID + DETAILED_TIMINGS_INDEX;

    // Manufacture name, EDID base block offset 0x08~0x09
    cb_memcpy(pMonitorAttrib->ManufactureName, &pEDID[0x08], 0x02);
    // ProductCode, EDID base block offset 0x0a~0x0b
    cb_memcpy(pMonitorAttrib->ProductCode, &pEDID[0x0A], 0x02);

    //get monitor Chromaticity and Default White Point
    cbEDIDModule_GetChromaticity(pEDID, &pMonitorAttrib->Chromaticity);

    cbEDIDModule_GetMonitorID(pEDID, pMonitorAttrib->MonitorID);

    // scan base EDID
    for (i = 0; i < 4; i++)
    {
        // detail timing
        if ((pDtlTimingsInBaseEDID[i*18] != 0x00) && 
            (pDtlTimingsInBaseEDID[i*18+0x1] != 0x00))
        {
            CBIOS_U8 byte17 = 0;
            byte17 = (pDtlTimingsInBaseEDID[i*18+0x11]&0x61);

            if(byte17)
            {
                pMonitorAttrib->bStereoViewSupport = CBIOS_TRUE;
                switch(byte17)
                {
                    case 0x20:
                        pMonitorAttrib->StereoViewType = FIELD_SEQ_RIGHT;
                        break;
                    case 0x40:
                        pMonitorAttrib->StereoViewType = FIELD_SEQ_LEFT;
                        break;
                    case 0x21:
                        pMonitorAttrib->StereoViewType = TWO_WAY_RIGHT;
                        break;
                    case 0x41:
                        pMonitorAttrib->StereoViewType = TWO_WAY_LEFT;
                        break;
                    case 0x60:
                        pMonitorAttrib->StereoViewType = FOUR_WAY;
                        break;
                    case 0x61:
                        pMonitorAttrib->StereoViewType = SIDE_BY_SIDE_INTERLEAVE;
                        break;
                    default:
                        cbDebugPrint((MAKE_LEVEL(GENERIC, WARNING), "cbEDIDModule_GetMonitorAttrib: No matched StereoViewType, byte17 is %d!\n", byte17));
                        break;
                }
            }   
        }

            
        //  the monitor descriptor block
        if((pDtlTimingsInBaseEDID[i*18] == 0x00) && 
           (pDtlTimingsInBaseEDID[i*18+0x1] == 0x00) &&
           (pDtlTimingsInBaseEDID[i*18+0x2] == 0x00) &&
           (pDtlTimingsInBaseEDID[i*18+0x3] == 0xFC) &&
           (pDtlTimingsInBaseEDID[i*18+0x4] == 0x00))
        {
            for (j = 0; j < 13; j++)
            {
                if (pDtlTimingsInBaseEDID[i*18+5+j] == 0x0A)   // end signature of monitor name
                {
                    break;
                }

                pMonitorAttrib->MonitorName[j] = pDtlTimingsInBaseEDID[i*18+5+j];
            }
        }

        // the monitor descriptor block
        if((pDtlTimingsInBaseEDID[i*18] == 0x00) &&
           (pDtlTimingsInBaseEDID[i*18+0x1] == 0x00) &&
           (pDtlTimingsInBaseEDID[i*18+0x2] == 0x00) &&
           (pDtlTimingsInBaseEDID[i*18+0x3] == 0xFD))
        {
            pMonitorAttrib->MRLData.Flags = pDtlTimingsInBaseEDID[i*18+0x4];
            pMonitorAttrib->MRLData.MinVerRate = pDtlTimingsInBaseEDID[i*18+0x5];   //Hz
            pMonitorAttrib->MRLData.MaxVerRate = pDtlTimingsInBaseEDID[i*18+0x6];  //Hz
            pMonitorAttrib->MRLData.MinHorRate = pDtlTimingsInBaseEDID[i*18+0x7];   //KHz
            pMonitorAttrib->MRLData.MaxHorRate = pDtlTimingsInBaseEDID[i*18+0x8];  //KHz
            pMonitorAttrib->MRLData.MaxPixelClock = pDtlTimingsInBaseEDID[i*18+0x9] * 10;  //MHz
	
            if((pMonitorAttrib->MRLData.Flags & 0x3) == 0x2)
            {
                pMonitorAttrib->MRLData.MaxVerRate += 255;
            }
            else if((pMonitorAttrib->MRLData.Flags & 0x3) == 0x3)
            {
                pMonitorAttrib->MRLData.MaxVerRate += 255;
                pMonitorAttrib->MRLData.MinVerRate += 255;
            }
	
            if((pMonitorAttrib->MRLData.Flags & 0xC) == 0x8)
            {
                pMonitorAttrib->MRLData.MaxHorRate += 255;
            }
            else if((pMonitorAttrib->MRLData.Flags & 0xC) == 0xC)
            {
                pMonitorAttrib->MRLData.MaxHorRate += 255;
                pMonitorAttrib->MRLData.MinHorRate += 255;
            }
	
            if(pMonitorAttrib->MRLData.MaxVerRate > pMonitorAttrib->MRLData.MinVerRate)
            {
                pMonitorAttrib->MRLData.bSupportFreeSync = CBIOS_TRUE;
            }
        }

        //  the Alphanumeric data string  descriptor block
        if((pDtlTimingsInBaseEDID[i*18] == 0x00) &&
           (pDtlTimingsInBaseEDID[i*18+0x1] == 0x00) &&
           (pDtlTimingsInBaseEDID[i*18+0x2] == 0x00) &&
           (pDtlTimingsInBaseEDID[i*18+0x3] == 0xFE) &&
           (pDtlTimingsInBaseEDID[i*18+0x4] == 0x00))
        {
            for (j = 0; j < 13; j++)
            {
                if (pDtlTimingsInBaseEDID[i*18+5+j] == 0x0A)   // end signature of Alphanumeric
                {
                    break;
                }

                pMonitorAttrib->Alphanumeric[j] = pDtlTimingsInBaseEDID[i*18+5+j];
            }
        }
    }

    // get monitor screen image horizontal and vertical size in base EDID detailed timing
    cbEDIDModule_GetMonitorSize(pEDID, pMonitorAttrib);

    pMonitorAttrib->CEA861MonitorCaps = 0;

    // check EDID version 1.4, since majority of DP monitors don't have extended EDID block
    if ((pEDID[0x12] == 0x01) && (pEDID[0x13] == 0x04))
    {
        if (pEDID[0x14] & 0x80)
        {
            pMonitorAttrib->IsCEA861RGB = 1;
            if ((pEDID[0x18] & 0x18) == 0x08)
            {
                pMonitorAttrib->IsCEA861YCbCr444 = 1;
            }
            else if ((pEDID[0x18] & 0x18) == 0x10)
            {
                pMonitorAttrib->IsCEA861YCbCr422 = 1;
            }
            else if ((pEDID[0x18] & 0x18) == 0x18)
            {
                pMonitorAttrib->IsCEA861YCbCr444 = 1;
                pMonitorAttrib->IsCEA861YCbCr422 = 1;
            }
        }
    }

    // CEA extension
    TotalBlocks = cbEDIDModule_GetTotalBlockNum(pEdidData);
    
    for (BlockIndex = 1; BlockIndex < TotalBlocks; BlockIndex++)
    {
        pEDIDBlock = pEDID + BlockIndex * 128;

        //check CEA Tag
        if (pEDIDBlock[0x00] != CEA_TAG)
        {
            continue;
        }

        // Check CEA861 Version.
        if (pEDIDBlock[0x01] == 0x00)
        {
            cbDebugPrint((MAKE_LEVEL(GENERIC, WARNING), "%s: EDID block%d indicates invalid CEA861 block version!\n", FUNCTION_NAME, BlockIndex));
            continue;
        }

        pMonitorAttrib->IsCEA861Monitor = CBIOS_TRUE;
        pMonitorAttrib->IsCEA861RGB = CBIOS_TRUE;

        // Initialize the CEA861 misc attribution.
        pMonitorAttrib->Tag = pEDIDBlock[0x00];
        pMonitorAttrib->RevisionNumber = pEDIDBlock[0x01];
        pMonitorAttrib->OffsetOfDetailedTimingBlock = pEDIDBlock[0x02];
        pMonitorAttrib->NativeFormatNum = pEDIDBlock[0x03] & 0x0F;
        pMonitorAttrib->IsCEA861UnderScan = (pEDIDBlock[0x03] & 0x80) >> 7;
        pMonitorAttrib->IsCEA861Audio = (pEDIDBlock[0x03] & 0x40) >> 6;
        pMonitorAttrib->IsCEA861YCbCr444 = (pEDIDBlock[0x03] & 0x20) >> 5;
        pMonitorAttrib->IsCEA861YCbCr422 = (pEDIDBlock[0x03] & 0x10) >> 4;

        for(i = 4; i < pMonitorAttrib->OffsetOfDetailedTimingBlock;)
        {
            if (((pEDIDBlock[i] >> 5) & 0x07) == VENDOR_SPECIFIC_DATA_BLOCK_TAG)
            {
                CBIOS_U8 PayloadLength = pEDIDBlock[i++] & 0x1F;

                if ((pEDIDBlock[i] == 0x03)
                    && (pEDIDBlock[i + 1] == 0x0C)
                    && (pEDIDBlock[i + 2] == 0x00))
                {
                    bIsHDMIDevice = CBIOS_TRUE;
                }

                i += PayloadLength;
            }
            else if (((pEDIDBlock[i] >> 5) & 0x07) == AUDIO_DATA_BLOCK_TAG)
            {
                //audio data block
                CBIOS_U8 PayloadLength = pEDIDBlock[i++] & 0x1F;
                SADCnt += PayloadLength/3;  // 3bytes makes one audio format

                if (SADCnt > 15)
                {
                    SADCnt = 15; //ELD spec define, at most 15 SADs
                }
                if (SADCnt > pMonitorAttrib->SAD_Count)
                {
                    cb_memcpy(((CBIOS_U8 *)pMonitorAttrib->CEA_SADs + pMonitorAttrib->SAD_Count * 3),
                           &pEDIDBlock[i], (SADCnt - pMonitorAttrib->SAD_Count) * 3);
                }

                pMonitorAttrib->SAD_Count = SADCnt;

                i += PayloadLength;
            }
            else if (((pEDIDBlock[i] >> 5) & 0x07) == SPEAKER_ALLOCATION_DATA_BLOCK_TAG)
            {
                //This payload is preceded by a Tag Code Byte that includes a tag equal to 4 and a length of 3
                pMonitorAttrib->SpeakerAllocationData = pEDIDBlock[i+1];
                i += 4;
            }
            else
            {
                CBIOS_U32 PayloadLength = (pEDIDBlock[i++] & 0x1F);

                i += PayloadLength;
            }
        }

        if (bIsHDMIDevice
            && (pMonitorAttrib->IsCEA861UnderScan
            | pMonitorAttrib->IsCEA861Audio
            | pMonitorAttrib->IsCEA861YCbCr422
            | pMonitorAttrib->IsCEA861YCbCr444))
        {
            pMonitorAttrib->IsCEA861HDMI = CBIOS_TRUE;
        }
    }

    return pMonitorAttrib->CEA861MonitorCaps;

}

/***************************************************************
Function:    cbEDIDModule_GetMonitor3DCaps

Description: Get monitor 3D capability

Input:       pEDIDStruct, decoded EDID timings and attributes
             IsSupport3DVideo, whether the monitor supports 3D video

Output:      p3DCapability, store decoded monitor 3D capabilitys

Return:
***************************************************************/
CBIOS_STATUS cbEDIDModule_GetMonitor3DCaps(PCBIOS_EDID_STRUCTURE_DATA pEDIDStruct,
                                           PCBIOS_MONITOR_3D_CAPABILITY_PARA p3DCapability,
                                           CBIOS_U32 IsHWSupportHDMI3DVideo)
{
    CBIOS_U32 Monitor3DModeNum = 0;
    PCBIOS_3D_VIDEO_MODE_LIST pModeList = p3DCapability->pMonitor3DModeList;
    CBIOS_U32 i;

    p3DCapability->bStereoViewSupport = pEDIDStruct->Attribute.bStereoViewSupport;
    p3DCapability->StereoViewType = pEDIDStruct->Attribute.StereoViewType;

    if (IsHWSupportHDMI3DVideo && pEDIDStruct->Attribute.VSDBData.HDMI3DPresent)
    {
        p3DCapability->bIsSupport3DVideo = CBIOS_TRUE;

        for (i = 0; i < pEDIDStruct->TimingNum; i++)
        {
            /*if (pSupportFormat[i].Video3DSupportCaps & CBIOS_3D_VIDEO_FORMAT_MASK)
            {
                Monitor3DModeNum++;

                if (pModeList != CBIOS_NULL)
                {
                    pModeList->XRes = HDMIFormatTimingTbl[FmtIndex].XRes;
                    pModeList->YRes = HDMIFormatTimingTbl[FmtIndex].YRes;
                    pModeList->RefreshRate = CEAVideoFormatTable[FmtIndex].RefRate[RefIndex];
                    pModeList->bIsInterlace = (CBIOS_BOOL)CEAVideoFormatTable[FmtIndex].Interlace;
                    pModeList->SupportCaps = pSupportFormat[i].Video3DSupportCaps & CBIOS_3D_VIDEO_FORMAT_MASK;
                    pModeList->IsSupport3DOSDDisparity = pEDIDStruct->Attribute.HFSCDSData.IsSupport3DOSDDisparity;
                    pModeList->IsSupport3DDualView = pEDIDStruct->Attribute.HFSCDSData.IsSupport3DDualView;
                    pModeList->IsSupport3DIndependentView = pEDIDStruct->Attribute.HFSCDSData.IsSupport3DIndependentView;
                    pModeList++;
                }
            }*/
        }
    }

    /* need parse Stereo Display Interface Data Block, future TODO
    for (i = 0; i < pEDIDStruct->DispIdTimingNum; i++)
    {
        if (pEDIDStruct->pDisplayIdDtlTimings[i].IsSupportStereo)
        {
            Monitor3DModeNum++;
            if (pModeList != CBIOS_NULL)
            {
                pModeList->XRes = pEDIDStruct->pDisplayIdDtlTimings[i].XResolution;
                pModeList->YRes = pEDIDStruct->pDisplayIdDtlTimings[i].YResolution;
                pModeList->RefreshRate = pEDIDStruct->pDisplayIdDtlTimings[i].Refreshrate;
                pModeList->bIsInterlace = (CBIOS_BOOL)pEDIDStruct->pDisplayIdDtlTimings[i].IsInterLaced;
                pModeList->SupportCaps = 0;
                pModeList->IsSupport3DOSDDisparity = 0;
                pModeList->IsSupport3DDualView = 0;
                pModeList->IsSupport3DIndependentView = 0;
                pModeList++;
            }
        }
    }
    */

    p3DCapability->Monitor3DModeNum = Monitor3DModeNum;
    //if no 3D mode is supported, set support flags to not support 3D video
    if (Monitor3DModeNum == 0)
    {
        p3DCapability->bIsSupport3DVideo = CBIOS_FALSE;
    }
    else
    {
        p3DCapability->bIsSupport3DVideo = CBIOS_TRUE;
    }

    return CBIOS_OK;
}

/***************************************************************
Function:    cbEDIDModule_IsEDIDValid

Description: if the EDID header is valid

Input:       pEDID, EDID data buffer

Output:      

Return:      CBIOS_TRUE if EDID header vaild
             CBIOS_FALSE if EDID header invaild
***************************************************************/
CBIOS_BOOL cbEDIDModule_IsEDIDHeaderValid(CBIOS_U8  *pEDIDBuffer, CBIOS_U32 ulBufferSize)
{
    CBIOS_U8 EDID1header[] = {0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00};
    CBIOS_BOOL  bRet = CBIOS_FALSE;

    if(ulBufferSize < sizeof(EDID1header))
    {
        return CBIOS_FALSE;
    }
    
    if((pEDIDBuffer != CBIOS_NULL) && 
        (!cb_memcmp(pEDIDBuffer, EDID1header, sizeof(EDID1header))))
    {
        bRet = CBIOS_TRUE;
    }
    
    return bRet;
}

/***************************************************************
Function:    cbEDIDModule_IsEDIDValid

Description: if the EDID data is CBIOS_NULL, the header is not correct 
             or the check sum is not zero, return false;
             otherwise return true.

Input:       pEDID, EDID data buffer

Output:      

Return:      CBIOS_TRUE if EDID vaild
             CBIOS_FALSE if EDID is null, header is not correct 
             or check sum is not zero
***************************************************************/
CBIOS_BOOL cbEDIDModule_IsEDIDValid(CBIOS_EDID_DATA *pEdidData)
{
    CBIOS_BOOL  bRet = CBIOS_FALSE;

    if (pEdidData && pEdidData->Buffer &&
        cbEDIDModule_IsEDIDHeaderValid(pEdidData->Buffer, pEdidData->BufferSize) &&
        cbIsEdidChecksumValid(pEdidData->Buffer, pEdidData->BufferSize))
    {
        bRet = CBIOS_TRUE;
    }

    return bRet;
}

/***************************************************************
Function:    cbEDIDModule_GetMonitorID

Description: Get monitor ID manufacturer name

Input:       pEDID, monitor EDID data buffer

Output:      pMnitorID, monitor ID manufacturer name buffer

Return:      CBIOS_TRUE if get monitor ID successfully
             CBIOS_FALSE if get monitor ID failed
***************************************************************/
CBIOS_BOOL cbEDIDModule_GetMonitorID(CBIOS_U8 *pEDID, CBIOS_U8 *pMonitorID)
{
    CBIOS_U8 *index = "0ABCDEFGHIJKLMNOPQRSTUVWXYZ[/]^_";
    CBIOS_U8 ProductID[3] = {0};
    CBIOS_BOOL bRet = CBIOS_FALSE;
    CBIOS_U8 *pMonitorIDinEDID = pEDID + MONITORIDINDEX;

    if ((pMonitorIDinEDID == CBIOS_NULL) || (pMonitorID == CBIOS_NULL))
    {
        cbDebugPrint((MAKE_LEVEL(GENERIC, ERROR),"cbEDIDModule_GetMonitorID: input buffer is null!\n"));
        return CBIOS_FALSE;
    }

    //get manufacturer ID
    pMonitorID[0] = index[(pMonitorIDinEDID[0] >> 2) & 0x1F];
    pMonitorID[1] = index[((pMonitorIDinEDID[1] >> 5) & 0x07) | ((pMonitorIDinEDID[0] << 3) & 0x18)];
    pMonitorID[2] = index[pMonitorIDinEDID[1] & 0x1F];
    pMonitorID[3] = '\0';

    if (cbItoA((CBIOS_U32)pMonitorIDinEDID[3], ProductID, 16, 2))
    {
        cbStrCat(pMonitorID, ProductID);
        if (cbItoA((CBIOS_U32)pMonitorIDinEDID[2], ProductID, 16, 2))
        {
            cbStrCat(pMonitorID, ProductID);
            bRet = CBIOS_TRUE;
            cbDebugPrint((MAKE_LEVEL(GENERIC, DEBUG), "Monitor ID is: %s\n", pMonitorID));
        }
    }

    return bRet;

}


//For HDMI and DP:Patch to disable 24bit for L_PCM audio format to pass HLK wave test
CBIOS_VOID cbEDIDModule_SADPatch(PCBIOS_EDID_STRUCTURE_DATA pEDIDStruct)
{
    CBIOS_U32 Index = 0;
    CBIOS_U8 SADCnt = pEDIDStruct->Attribute.SAD_Count;
    PCBIOS_MONITOR_MISC_ATTRIB pMonitorAttrib = &(pEDIDStruct->Attribute);

    //modify SAD to disable 24bit for L_PCM audio format
    for (Index = 0; Index < SADCnt; Index++)
    {
        if (((pMonitorAttrib->CEA_SADs[Index][0] >> 3) & 0xF) == 1)
        {
            if ((pMonitorAttrib->CEA_SADs[Index][2] >> 2) & 0x1)
            {
                pMonitorAttrib->CEA_SADs[Index][2] &= 0xFB;
            }
        }
    }

    for (Index = 0; Index < pEDIDStruct->HDAudioFormatNum; Index++)
    {
        if (pEDIDStruct->HDAudioFormat[Index].Format == CBIOS_AUDIO_FORMAT_LPCM)
        {
            pEDIDStruct->HDAudioFormat[Index].BitDepth.BD_24bit = 0;
        }
    }

}


static CBIOS_BOOL cbEDIDModule_Patch(CBIOS_EDID_DATA *pEdidData, CBIOS_MONITOR_MISC_ATTRIB *pAttribute)
{
    CBIOS_BOOL  bPatched = CBIOS_FALSE;
    CBIOS_BOOL  bFreeSyncToSupport = CBIOS_FALSE;

    if(!pEdidData || !pAttribute)
    {
        return CBIOS_FALSE;
    }

    // PHILIPS 24PFL3545 monitor exist a vertical garbage of mode 1920x1080i@50Hz as Hsync offset 
    // is not correct in EDID CEA extension. Patch this issue by correct EDID data.
    if ((!cbStrnCmp(pAttribute->MonitorID, (CBIOS_UCHAR*)"PHL0010", 7)) && 
        (!cbStrnCmp(pAttribute->MonitorName, (CBIOS_UCHAR*)"B24PFL3545/T3", 13)) && (pAttribute->IsCEA861HDMI))
    {
        cbCopyBuffer2EdidData(pEdidData, PHL_24PFL3545_Edid, sizeof(PHL_24PFL3545_Edid));
        bPatched = CBIOS_TRUE;
    }
    else if ((!cbStrnCmp(pAttribute->MonitorID, (CBIOS_UCHAR*)"AUO7AA7", 7)) && 
               (!cbStrnCmp(pAttribute->Alphanumeric, (CBIOS_UCHAR*)"B145QAN01.H ", 12)))//need add 2560x1600@60Hz
    {
        cbCopyBuffer2EdidData(pEdidData, AUO_B145QAN01_Edid, sizeof(AUO_B145QAN01_Edid));
        bPatched = CBIOS_TRUE;
    }
    else if((!cbStrnCmp(pAttribute->MonitorID, (CBIOS_UCHAR*)"CSW1473", 7)) &&
        (!cbStrnCmp(pAttribute->MonitorName, (CBIOS_UCHAR*)"MNE507QS2-4", 11)))//set 1920x1200@60Hz as prefer mode
    {
        cbCopyBuffer2EdidData(pEdidData, CSW1473_Edid, sizeof(CSW1473_Edid));
        bPatched = CBIOS_TRUE;
    }

    //only enable free sync for P1 panel
    if((!cbStrnCmp(pAttribute->MonitorID, (CBIOS_UCHAR*)"CSW1434", 7)) &&
        (!cbStrnCmp(pAttribute->Alphanumeric, (CBIOS_UCHAR*)"MNE507DA1-1", 11)))
    {
        bFreeSyncToSupport = CBIOS_TRUE;
    }
    else if((!cbStrnCmp(pAttribute->MonitorID, (CBIOS_UCHAR*)"TMA07E7", 7)) &&
        (!cbStrnCmp(pAttribute->Alphanumeric, (CBIOS_UCHAR*)"TL145ADMP010", 12)))
    {
        bFreeSyncToSupport = CBIOS_TRUE;
    }
	
    if(!bFreeSyncToSupport && pAttribute->MRLData.bSupportFreeSync)
    {
        pAttribute->MRLData.bSupportFreeSync = CBIOS_FALSE;
    }
    
    return bPatched;
}


/***************************************************************
Function:    cbEDIDModule_ParseEDID

Description: parse the whole EDID data

Input:       pEDID, EDID data buffer
             ulBufferSize, specify the size of EDID data buffer

Output:      pEDIDStruct, store decoded EDID timings and attributes

Return:      
***************************************************************/
CBIOS_BOOL cbEDIDModule_ParseEDID(CBIOS_EDID_DATA *pEdidData, PCBIOS_EDID_STRUCTURE_DATA pEDIDStruct)
{
    if (pEDIDStruct->pDeviceTimingList)
    {
        cb_FreePool(pEDIDStruct->pDeviceTimingList);
    }
    cb_memset(pEDIDStruct, 0, sizeof(CBIOS_EDID_STRUCTURE_DATA));

    if(!cbEDIDModule_IsEDIDValid(pEdidData))
    {
        cbDebugPrint((MAKE_LEVEL(GENERIC, WARNING), "cbEDIDModule_ParseEDID: EDID is invalid!\n"));
        return CBIOS_FALSE;
    }
    
    cbEDIDModule_GetMonitorAttrib(pEdidData, &(pEDIDStruct->Attribute));
    if(cbEDIDModule_Patch(pEdidData, &(pEDIDStruct->Attribute)))
    {
        cb_memset(&(pEDIDStruct->Attribute), 0, sizeof(pEDIDStruct->Attribute));
        cbEDIDModule_GetMonitorAttrib(pEdidData, &(pEDIDStruct->Attribute));
    }

    //First, parse detail/svd+dtd/displayid timing which has full timing para.
    //When parse est/std mode which just has x/y/refresh, we may search timing in above detailed timing struct
    cbEDIDModule_GetDetailedMode(pEdidData->Buffer, pEDIDStruct);

    if (pEdidData->BufferSize > 128)
    {
        cbEDIDModule_GetCEA861Mode(pEdidData, pEDIDStruct);
        cbEDIDModule_GetDisplayIDMode(pEdidData, pEDIDStruct);
    }

    cbEDIDModule_GetEstablishMode(pEdidData->Buffer, pEDIDStruct);
    cbEDIDModule_GetStandardMode(pEdidData->Buffer, pEDIDStruct);
    
    return CBIOS_TRUE;
}


/***************************************************************
Function:    cbEDIDModule_FakeDetailedTiming

Description: Fake Detailed Timing EDID data according to detailed timing.

Input:       pDtlTiming, detailed timing

Output:      pEdid, faked EDID buffer.
***************************************************************/
static CBIOS_VOID cbEDIDModule_FakeDetailedTiming(CBIOS_U8 *pEdid, PCBIOS_TIMING_ATTRIB pDtlTiming)
{
    CBIOS_U16 HorBlanking = pDtlTiming->HorBEnd - pDtlTiming->HorBStart;
    CBIOS_U16 VerBlanking = pDtlTiming->VerBEnd - pDtlTiming->VerBStart;
    CBIOS_U16 HorSyncOffset = pDtlTiming->HorSyncStart - pDtlTiming->HorBStart;
    CBIOS_U16 HorSyncWidth = pDtlTiming->HorSyncEnd - pDtlTiming->HorSyncStart;
    CBIOS_U16 VerSyncOffset = pDtlTiming->VerSyncStart - pDtlTiming->VerBStart;
    CBIOS_U16 VerSyncWidth = pDtlTiming->VerSyncEnd - pDtlTiming->VerSyncStart;

    pEdid[0x38] = (CBIOS_U8)(pDtlTiming->XRes & 0xFF);
    pEdid[0x39] = (CBIOS_U8)(HorBlanking & 0xFF);
    pEdid[0x3A] = (CBIOS_U8)(((pDtlTiming->XRes & 0xF00) >> 4) | ((HorBlanking & 0xF00) >> 8));
    pEdid[0x3B] = (CBIOS_U8)(pDtlTiming->YRes & 0xFF);
    pEdid[0x3C] = (CBIOS_U8)(VerBlanking & 0xFF);
    pEdid[0x3D] = (CBIOS_U8)(((pDtlTiming->YRes & 0xF00) >> 4) | ((VerBlanking & 0xF00) >> 8));
    pEdid[0x3E] = (CBIOS_U8)(HorSyncOffset & 0xFF);
    pEdid[0x3F] = (CBIOS_U8)(HorSyncWidth & 0xFF);
    pEdid[0x40] = (CBIOS_U8)((VerSyncOffset & 0x0F) << 4 | (VerSyncWidth & 0x0F));
    pEdid[0x41] = (CBIOS_U8)(((HorSyncOffset & 0x300) >> 2) | ((HorSyncWidth & 0x300) >> 4)
                            | ((VerSyncOffset & 0x30) >> 2) | ((VerSyncWidth & 0x30) >> 4));
    pEdid[0x42] = 0x90;
    pEdid[0x43] = 0x2C;
    pEdid[0x44] = 0x11;
    pEdid[0x47] = 0x18; // Digital Separate

    // H polarity
    if (!(pDtlTiming->HVPolarity & HorNEGATIVE))
    {
        pEdid[0x47] |= 0x2;
    }

    // V polarity
    if (!(pDtlTiming->HVPolarity & VerNEGATIVE))
    {
        pEdid[0x47] |= 0x4;
    }
}

/***************************************************************
Function:    cbEDIDModule_FakePanelEDID

Description: Fake EDID data for Panel Device since it doesn't have 
             EDID data in its local side.

Input:       pFakeEdidParam, params used to fake EDID
             EdidBufferLength, the length of edid buffer (currently just support 128 bytes)

Output:      pEdid, faked EDID buffer.

Return:      CBIOS_TRUE if fake panel EDID success, else, CBIOS_FALSE.
***************************************************************/
CBIOS_BOOL cbEDIDModule_FakePanelEDID(PCBIOS_FAKE_EDID_PARAMS pFakeEdidParam, PCBIOS_U8 pEdid, const CBIOS_U32 EdidBufferLength)
{
    CBIOS_BOOL bRet = CBIOS_FALSE;
    CBIOS_U32 i = 0;
    CBIOS_U8  CheckSum = 0;

    if ((pFakeEdidParam == CBIOS_NULL) || (pEdid == CBIOS_NULL))
    {
        cbDebugPrint((MAKE_LEVEL(GENERIC, ERROR), "%s: the 1st or 2nd param is NULL!\n", FUNCTION_NAME));
        return bRet;
    }

    if (EdidBufferLength != 128)
    {
        cbDebugPrint((MAKE_LEVEL(GENERIC, ERROR), "%s: currently just support faking 128-byte EDID!\n", FUNCTION_NAME));
        return bRet;
    }

    //fake 128 Bytes EDID only
    cb_memset(pEdid, 0, EdidBufferLength);

    //EDID header
    pEdid[0] = 0x00;
    pEdid[1] = 0xFF;
    pEdid[2] = 0xFF;
    pEdid[3] = 0xFF;
    pEdid[4] = 0xFF;
    pEdid[5] = 0xFF;
    pEdid[6] = 0xFF;
    pEdid[7] = 0x00;

    //ID Manufacturer Name. Compressed ASCII.  Expands to 'MS_' for Microsoft
    pEdid[8] = 0x36;
    pEdid[9] = 0x7f;

    //ID Product Code
    pEdid[0x0A] = 0x00;
    pEdid[0x0B] = 0x00;

    //ID Serial Number
    pEdid[0x0C] = 0x00;
    pEdid[0x0D] = 0x00;
    pEdid[0x0E] = 0x00;
    pEdid[0x0F] = 0x00;

    //Week and Year
    pEdid[0x10] = 1;
    pEdid[0x11] = 10;

    //Version
    pEdid[0x12] = 0x01;
    pEdid[0x13] = 0x03;
    
    //Mark LCD as digital device
    pEdid[0x14] = 0x80;

    //Max. Horizontal Image Size, cm
    pEdid[0x15] = 40;

    //Max. Vertical Image Size, cm
    pEdid[0x16] = 30;

    //Display Transfer (gamma)
    pEdid[0x17] = 0;
    
    //Add Preferred Timing Mode flag
    pEdid[0x18] |= 0x02;

    //Color Characteristics
    pEdid[0x19] = 0x78;
    pEdid[0x1A] = 0xA0;
    pEdid[0x1B] = 0x56;
    pEdid[0x1C] = 0x48;
    pEdid[0x1D] = 0x9A;
    pEdid[0x1E] = 0x26;
    pEdid[0x1F] = 0x12;
    pEdid[0x20] = 0x48;
    pEdid[0x21] = 0x4C;
    pEdid[0x22] = 0xFF;

    //DCLK
    pEdid[0x36] = (CBIOS_U8)(pFakeEdidParam->DtlTiming.PixelClock);
    pEdid[0x37] = (CBIOS_U8)(pFakeEdidParam->DtlTiming.PixelClock >> 8);

    //detailed timing
    if (pFakeEdidParam->bProvideDtlTimingEDID)
    {
        cb_memcpy(pEdid + 0x38, pFakeEdidParam->DtlTimingEDID, 16);
    }
    else
    {
        cbEDIDModule_FakeDetailedTiming(pEdid, &(pFakeEdidParam->DtlTiming));
    }

    //second 18 byte data block, display product name descriptor
    //flags and tag
    pEdid[0x48] = 0x00;
    pEdid[0x49] = 0x00;
    pEdid[0x4A] = 0x00;
    pEdid[0x4B] = 0xFC;    //display product name tag
    pEdid[0x4C] = 0x00;
    //fake monitor model name, "XXX LCD"
    pEdid[0x4D] = 0x5A;
    pEdid[0x4E] = 0x58;
    pEdid[0x4F] = 0x47;
    pEdid[0x50] = 0x20;
    pEdid[0x51] = 0x4C;
    pEdid[0x52] = 0x43;
    pEdid[0x53] = 0x44;
    pEdid[0x54] = 0x0A;
    pEdid[0x55] = 0x20;
    pEdid[0x56] = 0x20;
    pEdid[0x57] = 0x20;
    pEdid[0x58] = 0x20;
    pEdid[0x59] = 0x20;
    
    //no extension block
    pEdid[0x7E] = 0;

    //checksum
    for (i = 0; i < EdidBufferLength - 1; i++)
    {
        CheckSum += pEdid[i];
    }

    CheckSum = 0xFF - CheckSum + 1;
    pEdid[0x7F] = CheckSum;

    bRet = CBIOS_TRUE;
    return bRet;
}
