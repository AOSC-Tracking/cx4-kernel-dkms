/*
 * Copyright (c) 2018 Shanghai Zhaoxin Semiconductor Co., Ltd.
 * All Rights Reserved.
 * This is UNPUBLISHED PROPRIETARY SOURCE CODE of Shanghai Zhaoxin Semiconductor Co., Ltd.;
 * the contents of this file may not be disclosed to third parties, copied or
 * duplicated in any form, in whole or in part, without the prior written
 * permission of Shanghai Zhaoxin Semiconductor Co., Ltd.
 *
 * The copyright of the source code is protected by the copyright laws of the People's
 * Republic of China and the related laws promulgated by the People's Republic of China
 * and the international covenant(s) ratified by the People's Republic of China.
 *
 */
#ifndef _H_vidsch_dump_image_e3k_H
#define _H_vidsch_dump_image_e3k_H

typedef struct _HangDumpFileHeader
{
    unsigned int        nHeaderVersion;
    unsigned int        nChipID;
    unsigned int        nDeviceID;
    unsigned int        nSliceMask;
    unsigned int        nHangDumpFileHeaderSizeInByte;
    unsigned int        reserved0[3];

    unsigned long long  lowTopAddress;
    unsigned long long  nAdapterMemorySizeInByte;
    unsigned long long  nPCIEMemorySizeInByte;          // default 3G

    unsigned long long  nRecordNums;                    // the DMA/Context/Ring Buffer numbers which DUMP file include.

                                                        // DMA
    unsigned long long  nDmaOffset;
    unsigned long long  nSingleDmaSizeInByte;           // nRecordNums*nSingleDmaSizeInByte = 0x40000

                                                        // Context
    unsigned long long  nContextOffset;
    unsigned long long  nSingleContextSizeInByte;       // nRecordNums*nSingleContextSizeInByte = 0x160000

                                                        // Ring Buffer
    unsigned long long  nRingBufferOffset;
    unsigned long long  nSingleRingBufferSizeInByte;    // nRecordNums*nSingleRingBufferSizeInByte

    unsigned long long  nTransferBufferOffsetInFBFile;  // useless, remove it
    unsigned long long  nGartTableL3Offset;
    unsigned long long  nGartTableL2Offset;
    unsigned long long  dummyPageEntry;

    unsigned long long  nBlBufferOffset;

    unsigned long long  nTtbrBufferOffset;

    unsigned long long  reserved1[0x1f];
} HangDumpFileHeader;

#define MEMORY_TYPE_INVALID         0xdeadbeef
#define MEMORY_TYPE_PTES            0x80000000
#define MEMORY_TYPE_LOCAL_VISIBLE   0x97
#define MEMORY_TYPE_LOCAL_INVISIBLE 0x98
#define MEMORY_TYPE_PCIE            0x195
typedef struct _HangDumpMemoryRangeHeader
{
    unsigned int        type;       // Type = 0 means the end of memory range
    unsigned int        size;
    unsigned long long  phyAddr;    // for local means gpu phyaddr, for pcie means cpu phyAddr and used for patch L3 table
} HangDumpMemoryRangeHeader;

#endif
