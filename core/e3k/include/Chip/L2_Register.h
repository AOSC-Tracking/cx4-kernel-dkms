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
//    Spec Last Modified Time: 2019/3/1 9:46:22
#ifndef _L2_REGISTER_H
#define _L2_REGISTER_H


#ifndef        L2_BLOCKBASE_INF
    #define    L2_BLOCKBASE_INF
    #define    BLOCK_L2_VERSION 1
    #define    BLOCK_L2_TIMESTAMP "2019/3/1 9:46:22"
    #define    L2_BLOCK                                                   0xB // match with BlockID.h
    #define    L2_REG_START                                               0x0 // match with BlockID.h
    #define    L2_REG_END                                                 0x10 // match with BlockID.h
    #define    L2_REG_LIMIT                                               0x10 // match with BlockID.h
#endif

// Register offset definition
#define        Reg_Rb0_Offset                                             0x0
#define        Reg_Rb1_Offset                                             0x1
#define        Reg_Rb2_Offset                                             0x2
#define        Reg_Argument_Offset                                        0x3
#define        Reg_L2_Performance_Offset                                  0x4
#define        Reg_L2_Reserved_Offset                                     0x5
#define        Reg_L2_Reserved1_Offset                                    0xC

// Block constant definition

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////            L2 Block (Block ID = 11) Register Definitions                             ///////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define  Reg_Rb0_Reserved_0_BitField_MaxValue ((1u << 20) - 1)
#define  Reg_Rb0_Reserved_1_BitField_MaxValue ((1u << 12) - 1)

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Reserved_0                : 20;  // ring buffer base address, aligned to 64KB
        unsigned int Reserved_1                : 12;  // size of the ring buffer, aligned to 64KB
    } reg;
} Reg_Rb0;

#define  Reg_Rb1_Reserved_0_BitField_MaxValue ((1u << 20) - 1)
#define  Reg_Rb1_Reserved_1_BitField_MaxValue ((1u << 12) - 1)

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Reserved_0                : 20;  // ring buffer base address, aligned to 64KB
        unsigned int Reserved_1                : 12;  // size of the ring buffer, aligned to 64KB
    } reg;
} Reg_Rb1;

#define  Reg_Rb2_Reserved_0_BitField_MaxValue ((1u << 20) - 1)
#define  Reg_Rb2_Reserved_1_BitField_MaxValue ((1u << 12) - 1)

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Reserved_0                : 20;  // ring buffer base address, aligned to 64KB
        unsigned int Reserved_1                : 12;  // size of the ring buffer, aligned to 64KB
    } reg;
} Reg_Rb2;

#define  Reg_Argument_Reserved_0_BitField_MaxValue ((1u << 20) - 1)
#define  Reg_Argument_Reserved_1_BitField_MaxValue ((1u << 12) - 1)

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Reserved_0                : 20;  // argument buffer base address, aligned to 64KB
        unsigned int Reserved_1                : 12;  // size of the argument buffer, aligned to 64KB
    } reg;
} Reg_Argument;

#define  Reg_L2_Performance_L2_Dist_Ctrl_BitField_MaxValue ((1u << 1) - 1)
#define  Reg_L2_Performance_Reserved0_BitField_MaxValue ((1u << 1) - 1)
#define  Reg_L2_Performance_L2_Latency_Buf_Dist_Ctrl_BitField_MaxValue ((1u << 1) - 1)
#define  Reg_L2_Performance_Reserved1_BitField_MaxValue ((1u << 1) - 1)
#define  Reg_L2_Performance_L2_Slc_Osd_Num_Ctrl_BitField_MaxValue ((1u << 4) - 1)
#define  Reg_L2_Performance_L2_Tu_Block_By_Refcnt_BitField_MaxValue ((1u << 1) - 1)
#define  Reg_L2_Performance_L2_Fully_W_Opt_BitField_MaxValue ((1u << 1) - 1)
#define  Reg_L2_Performance_Reserved2_BitField_MaxValue ((1u << 22) - 1)

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int L2_Dist_Ctrl              : 1;   // used for sub discributation for gpc, 0 -> default conf,
                                                      // addr/3 to get sub id
                                                      //                                                          1 ->  sub0-1 is distributed to gpc0
                                                      //                                                                  sub2-3 is distributed to gpc1
                                                      //                                                                  sub4-5 is distributed to gpc2
        unsigned int Reserved0                 : 1;   // RESERVED
        unsigned int L2_Latency_Buf_Dist_Ctrl : 1;    // used for each sub to allocate the pending fifo for
                                                      // different TU
                                                      //  0 -> the defaut conf, Tff0  -  Tff7 will mapped to pe0 - pe7 for
                                                      // each gpc
                                                      //  1 -> Tff0-Tff3 will mapped to slc0-slc3 of gpc0
                                                      //          Tff4-Tff7 will mapped to slc0-slc3 of gpc1
                                                      //          Tff0-Tff7 will mapped to pe0-pe7 of gpc2
                                                      //
                                                      //
        unsigned int Reserved1                 : 1;   // RESERVED
        unsigned int L2_Slc_Osd_Num_Ctrl       : 4;   // used for limiting the req to internal buf after SLIP, to
                                                      // lighten the return conflict of TU
                                                      //  4'd0 will be the default conf, will not limit the num of the
                                                      // req
                                                      //  other value will be the threadhold
        unsigned int L2_Tu_Block_By_Refcnt     : 1;   // used for line replacement when the ref_cnt of the line
                                                      // reach the maximum value.
                                                      //  0  -> if the ref_cnt reach the maximum value, it will halt the
                                                      // hit_test pipe
                                                      //          until the ref_cnt is released and smaller than maximum
                                                      // value.
                                                      //  1  -> if the ref_cnt reach the maximum value, it will be treat
                                                      // as hit-miss, and
                                                      //           allocate a new line for it, and will not halt the hit-test
                                                      // pipe.
        unsigned int L2_Fully_W_Opt            : 1;   // used for the optimization of fully_write from FFC at
                                                      // range_off type.
                                                      //  0  -> even the FFC wirte mask is all valid, L2 still need to
                                                      // read the data from MXU.
                                                      //  1  -> if the FFC write mask is all valid, L2 will not read the
                                                      // data from MXU, since the
                                                      //           read data is useless.
        unsigned int Reserved2                 : 22;  // RESERVED
    } reg;
} Reg_L2_Performance;

#define  Reg_L2_Reserved_Reserved_BitField_MaxValue ((1u << 32) - 1)

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Reserved                  : 32;  // RESERVED
    } reg;
} Reg_L2_Reserved;

#define  Reg_L2_Reserved1_Reserved_BitField_MaxValue ((1u << 32) - 1)

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Reserved                  : 32;  // RESERVED
    } reg;
} Reg_L2_Reserved1;

typedef struct _L2_regs
{
    Reg_Rb0                          reg_Rb0;
    Reg_Rb1                          reg_Rb1;
    Reg_Rb2                          reg_Rb2;
    Reg_Argument                     reg_Argument;
    Reg_L2_Performance               reg_L2_Performance;
    Reg_L2_Reserved                  reg_L2_Reserved[7];
    Reg_L2_Reserved1                 reg_L2_Reserved1[4];
} L2_regs;

#endif
