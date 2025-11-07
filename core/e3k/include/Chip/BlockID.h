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

#ifndef _REGISTER_BLOCKID_H
#define _REGISTER_BLOCKID_H


// below are each Bloch's top information, time stamp, version, ID, limit
#ifndef        CSP_GLOBAL_BLOCKBASE_INF
    #define    CSP_GLOBAL_BLOCKBASE_INF
    #define    BLOCK_CSP_GLOBAL_VERSION 1
    #define    BLOCK_CSP_GLOBAL_TIMESTAMP "9/20/2018 9:44:51 AM"
    #define    CSP_GLOBAL_BLOCK                                           0x0 // match with BlockID.h
    #define    CSP_GLOBAL_REG_START                                       0x0 // match with BlockID.h
    #define    CSP_GLOBAL_REG_END_CHX004                                  0x380 // match with BlockID.h
    #define    CSP_GLOBAL_REG_LIMIT_CHX004                                0x380 // match with BlockID.h
    #define    CSP_GLOBAL_REG_END_Elt3k                                   0x3C0 // match with BlockID.h
    #define    CSP_GLOBAL_REG_LIMIT_Elt3k                                 0x3C0 // match with BlockID.h
#endif

#ifndef        GPCPFE_BLOCKBASE_INF
    #define    GPCPFE_BLOCKBASE_INF
    #define    BLOCK_GPCPFE_VERSION 1
    #define    BLOCK_GPCPFE_TIMESTAMP "2018/1/4 16:28:55"
    #define    GPCPFE_BLOCK                                               0x1 // match with BlockID.h
    #define    GPCPFE_REG_START                                           0x0 // match with BlockID.h
    #define    GPCPFE_REG_END                                             0x28 // match with BlockID.h
    #define    GPCPFE_REG_LIMIT                                           0x28 // match with BlockID.h
#endif


#ifndef        SPIN_BLOCKBASE_INF
    #define    SPIN_BLOCKBASE_INF
    #define    BLOCK_SPIN_VERSION 1
    #define    BLOCK_SPIN_TIMESTAMP "2017/9/27 15:04:28"
    #define    SPIN_BLOCK                                                 0x2 // match with BlockID.h
    #define    SPIN_REG_START                                             0x0 // match with BlockID.h
    #define    SPIN_REG_END                                               0x8 // match with BlockID.h
    #define    SPIN_REG_LIMIT                                             0x8 // match with BlockID.h
#endif

#ifndef        EU_FS_BLOCKBASE_INF
    #define    EU_FS_BLOCKBASE_INF
    #define    BLOCK_EU_FS_VERSION 1
    #define    BLOCK_EU_FS_TIMESTAMP "2018/9/19 23:24:28"
    #define    EU_FS_BLOCK                                                0x3 // match with BlockID.h
    #define    EU_FS_REG_START                                            0x0 // match with BlockID.h
    #define    EU_FS_REG_END                                              0x900 // match with BlockID.h
    #define    EU_FS_REG_LIMIT_CHX004                                     0xA80 // match with BlockID.h
    #define    EU_FS_REG_LIMIT_E3k                                      0xAC0 // match with BlockID.h
#endif

#ifndef        TASFE_BLOCKBASE_INF
    #define    TASFE_BLOCKBASE_INF
    #define    BLOCK_TASFE_VERSION 1
    #define    BLOCK_TASFE_TIMESTAMP "2019/8/21 8:54:48"
    #define    TASFE_BLOCK                                                0x4 // match with BlockID.h
    #define    TASFE_REG_START                                            0x0 // match with BlockID.h
    #define    TASFE_REG_END_CHX004                                       0xE8 // match with BlockID.h
    #define    TASFE_REG_LIMIT_CHX004                                     0xE8 // match with BlockID.h
    #define    TASFE_REG_END_E3K                                        0xC0 // match with BlockID.h
    #define    TASFE_REG_LIMIT_E3k                                      0xC0 // match with BlockID.h
#endif

#ifndef        TASBE_BLOCKBASE_INF
    #define    TASBE_BLOCKBASE_INF
    #define    BLOCK_TASBE_VERSION 1
    #define    BLOCK_TASBE_TIMESTAMP "10/13/2017 10:19:33 AM"
    #define    TASBE_BLOCK                                                0x5 // match with BlockID.h
    #define    TASBE_REG_START                                            0x0 // match with BlockID.h
    #define    TASBE_REG_END                                              0x8 // match with BlockID.h
    #define    TASBE_REG_LIMIT                                            0x8 // match with BlockID.h
#endif


#ifndef        FF_BLOCKBASE_INF
    #define    FF_BLOCKBASE_INF
    #define    BLOCK_FF_VERSION 1
    #define    BLOCK_FF_TIMESTAMP "11:00 2019-09-05"
    #define    FF_BLOCK                                           0x6 // match with BlockID.h
    #define    FF_REG_START                                       0x0 // match with BlockID.h
    #define    FF_REG_END                                         0xd0 // match with BlockID.h
    #define    FF_REG_LIMIT                                       0xd0 // match with BlockID.h
#endif

#ifndef        IU_BLOCKBASE_INF
    #define    IU_BLOCKBASE_INF
    #define    BLOCK_IU_VERSION 1
    #define    BLOCK_IU_TIMESTAMP "8/15/2019 1:51:58 PM"
    #define    IU_BLOCK                                                   0x7
    #define    IU_REG_START                                               0x0
    #define    IU_REG_END_CHX004                                          0x48
    #define    IU_REG_LIMIT_CHX004                                        0x48
    #define    IU_REG_END_E3K                                           0x40
    #define    IU_REG_LIMIT_E3K                                         0x40
#endif


#ifndef        WLS_FE_BLOCKBASE_INF
    #define    WLS_FE_BLOCKBASE_INF
    #define    BLOCK_WLS_FE_VERSION 1
    #define    BLOCK_WLS_FE_TIMESTAMP "2017-09-27 15:40:09"
    #define    WLS_FE_BLOCK                                               0x8 // match with BlockID.h
    #define    WLS_FE_REG_START                                           0x0 // match with BlockID.h
    #define    WLS_FE_REG_END                                             0x418 // match with BlockID.h
    #define    WLS_FE_REG_LIMIT                                           0x418 // match with BlockID.h
#endif

#ifndef        EU_PS_BLOCKBASE_INF
    #define    EU_PS_BLOCKBASE_INF
    #define    BLOCK_EU_PS_VERSION 1
    #define    BLOCK_EU_PS_TIMESTAMP "2018/8/2 16:58:57"
    #define    EU_PS_BLOCK                                                0x9 // match with BlockID.h
    #define    EU_PS_REG_START                                            0x0 // match with BlockID.h
    #define    EU_PS_REG_END                                              0x900 // match with BlockID.h
    #define    EU_PS_REG_LIMIT                                            0x900 // match with BlockID.h
#endif

#ifndef        TUFE_BLOCKBASE_INF
    #define    TUFE_BLOCKBASE_INF
    #define    BLOCK_TUFE_VERSION 1
    #define    BLOCK_TUFE_TIMESTAMP "2017-08-18 15:00:40"
    #define    TUFE_BLOCK                                                 0xA
    #define    TUFE_REG_START                                             0x0
    #define    TUFE_REG_END_CHX004                                        0x700
    #define    TUFE_REG_END_Elt3k                                         0x580
    #define    TUFE_REG_LIMIT_CHX004                                      0x700
    #define    TUFE_REG_LIMIT_Elt3k                                       0x580
#endif

#ifndef        L2_BLOCKBASE_INF
    #define    L2_BLOCKBASE_INF
    #define    BLOCK_L2_VERSION 1
    #define    BLOCK_L2_TIMESTAMP "2017-08-17 15:47:22"
    #define    L2_BLOCK                                                   0xB
    #define    L2_REG_START                                               0x0
    #define    L2_REG_END_CHX004                                          0x10
    #define    L2_REG_END_Elt3k                                           0x8
    #define    L2_REG_LIMIT                                               0x0
#endif

#ifndef        MXU_BLOCKBASE_INF
    #define    MXU_BLOCKBASE_INF
    #define    BLOCK_MXU_VERSION 1
    #define    BLOCK_MXU_TIMESTAMP "2017-08-17 15:47:22"
    #define    MXU_BLOCK                                                  0xC
    #define    MXU_REG_START                                              0x0
    #define    MXU_REG_END                                                0xD
    #define    MXU_REG_LIMIT                                              0xD
#endif

// below are each Bloch's top information, time stamp, version, ID, limit
#ifndef        MCE_BLOCKBASE_INF
    #define    MCE_BLOCKBASE_INF
    #define    BLOCK_MCE_VERSION 1
    #define    BLOCK_MCE_TIMESTAMP "2017/11/20 9:05:57"
    #define    MCE_BLOCK                                                  0xD
    #define    MCE_REG_START                                              0x0
    #define    MCE_REG_END                                                0x18
    #define    MCE_REG_LIMIT                                              0x18
#endif

#ifndef        EU_CS_BLOCKBASE_INF
    #define    EU_CS_BLOCKBASE_INF
    #define    BLOCK_EU_CS_VERSION 1
    #define    BLOCK_EU_CS_TIMESTAMP "2018/8/2 16:58:57"
    #define    EU_CS_BLOCK                                                0xE // match with BlockID.h
    #define    EU_CS_REG_START                                            0x0 // match with BlockID.h
    #define    EU_CS_REG_END                                              0x900 // match with BlockID.h
    #define    EU_CS_REG_LIMIT                                            0x900 // match with BlockID.h
#endif

// #ifndef        VCP_BLOCKBASE_INF
// #define    VCP_BLOCKBASE_INF
// #define    BLOCK_VCP_VERSION 1
// #define    BLOCK_VCP_TIMESTAMP "2017-08-10 13:57:28"
// #define    VCP_BLOCK                                                0xF
// #define    VCP_REG_START                                            0x0
// #define    VCP_REG_END                                              0x0
// #define    VCP_REG_LIMIT                                            0x0
// #endif

#ifndef        VPP_BLOCKBASE_INF
    #define    VPP_BLOCKBASE_INF
    #define    BLOCK_VPP_VERSION 1
    #define    BLOCK_VPP_TIMESTAMP "2017-08-10 13:57:28"
    #define    VPP_BLOCK                                                0x10
    #define    VPP_REG_START                                            0x0
    #define    VPP_REG_END                                              0x0
    #define    VPP_REG_LIMIT                                            0x0
#endif

#ifndef        ISP_BLOCKBASE_INF
    #define    ISP_BLOCKBASE_INF
    #define    BLOCK_ISP_VERSION 1
    #define    BLOCK_ISP_TIMESTAMP "2017-08-10 13:57:28"
    #define    ISP_BLOCK                                                0x11
    #define    ISP_REG_START                                            0x0
    #define    ISP_REG_END                                              0x0
    #define    ISP_REG_LIMIT                                            0x0
#endif

#ifndef        MMU_BLOCKBASE_INF
    #define    MMU_BLOCKBASE_INF
    #define    BLOCK_MMU_VERSION 1
    #define    BLOCK_MMU_TIMESTAMP "2017-08-17 16:42:23"
    #define    MMU_BLOCK                                                  0x12
    #define    MMU_REG_START                                              0x0
    #define    MMU_REG_END                                                0x1000
    #define    MMU_REG_LIMIT                                              0x1000
#endif

#ifndef        WLS_BE_BLOCKBASE_INF
    #define    WLS_BE_BLOCKBASE_INF
    #define    BLOCK_WLS_BE_VERSION 1
    #define    BLOCK_WLS_BE_TIMESTAMP "2017-08-31 12:06:11"
    #define    WLS_BE_BLOCK                                               0x13
    #define    WLS_BE_REG_START                                           0x0
    #define    WLS_BE_REG_END                                             0x418
    #define    WLS_BE_REG_LIMIT                                           0x418
#endif

#ifndef        TUBE_BLOCKBASE_INF
    #define    TUBE_BLOCKBASE_INF
    #define    BLOCK_TUBE_VERSION 1
    #define    BLOCK_TUBE_TIMESTAMP "2017-08-17 16:42:23"
    #define    TUBE_BLOCK                                                 0x14
    #define    TUBE_REG_START                                             0x0
    #define    TUBE_REG_END                                               0x700
    #define    TUBE_REG_LIMIT                                             0x700
#endif

#ifndef        GPCPBE_BLOCKBASE_INF
    #define    GPCPBE_BLOCKBASE_INF
    #define    BLOCK_GPCPBE_VERSION 1
    #define    BLOCK_GPCPBE_TIMESTAMP "2018/5/4 10:06:54"
    #define    GPCPBE_BLOCK                                               0x15
    #define    GPCPBE_REG_START                                           0x0
    #define    GPCPBE_REG_END                                             0x1C8
    #define    GPCPBE_REG_LIMIT                                           0x1C8
#endif


#ifndef        SPOUT_BLOCKBASE_INF
    #define    SPOUT_BLOCKBASE_INF
    #define    BLOCK_SPOUT_VERSION 1
    #define    BLOCK_SPOUT_TIMESTAMP "2017/12/8 16:13:49"
    #define    SPOUT_BLOCK                                                0x16 // match with BlockID.h
    #define    SPOUT_REG_START                                            0x0 // match with BlockID.h
    #define    SPOUT_REG_END                                              0x20 // match with BlockID.h
    #define    SPOUT_REG_LIMIT                                            0x20 // match with BlockID.h
#endif

#ifndef        MIU_BLOCKBASE_INF
    #define    MIU_BLOCKBASE_INF
    #define    BLOCK_MIU_VERSION 1
    #define    BLOCK_MIU_TIMESTAMP "2011-7-13 11:14:15"
    #define    MIU_BLOCK                                                  0x17 // match with BlockID.h
    #define    MIU_REG_START                                              0x0 // match with BlockID.h
    #define    MIU_REG_END                                                0x38 // match with BlockID.h
    #define    MIU_REG_LIMIT                                              0x38 // match with BlockID.h
#endif

typedef struct _TimeStampTable_Struct
{
    char BlockName[96];
    char TimeStamp[96];
} TimeStampTable_Struct;

extern TimeStampTable_Struct TimeStampTable[];

#endif
