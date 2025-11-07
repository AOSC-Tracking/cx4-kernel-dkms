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
//    Spec Last Modified Time: 2018/9/7 14:21:01
#ifndef _GPCPFE_REGISTER_H
#define _GPCPFE_REGISTER_H


#ifndef        GPCPFE_BLOCKBASE_INF
    #define    GPCPFE_BLOCKBASE_INF
    #define    BLOCK_GPCPFE_VERSION 1
    #define    BLOCK_GPCPFE_TIMESTAMP "2018/9/7 14:21:01"
    #define    GPCPFE_BLOCK                                               0x1 // match with BlockID.h
    #define    GPCPFE_REG_START                                           0x0 // match with BlockID.h
    #define    GPCPFE_REG_END                                             0x70 // match with BlockID.h
    #define    GPCPFE_REG_LIMIT                                           0x70 // match with BlockID.h
#endif

// Register offset definition
#define        Reg_Gpcpfe_Instance_Offset                                 0x0
#define        Reg_Gpcpfe_Inst_Mask_Offset                                0x20
#define        Reg_Gpcpfe_Scheduler_Ctrl_Offset                           0x21
#define        Reg_Gpcpfe_Reserved_Offset                                 0x22
#define        Reg_Gpcpfe_Iidcnt_Offset                                   0x28
#define        Reg_Gpcpfe_Iid_Offset                                      0x68
#define        Reg_Gpcpfe_Pid_Offset                                      0x69
#define        Reg_Gpcpfe_Gp_Global_Offset                                0x6A
#define        Reg_Gpcpfe_Gp_Group_Offset                                 0x6D

// Block constant definition
typedef enum
{
    GPCPFE_SCHEDULER_CTRL_SCHEDULER_MODE_SLICE_FIRST_LOOP= 0,
                                                    // Loop the valid Slice firstly, then GPC
    GPCPFE_SCHEDULER_CTRL_SCHEDULER_MODE_GPC_FIRST_LOOP= 1,
                                                    // Loop the valid GPC firstly, then Slice
} GPCPFE_SCHEDULER_CTRL_SCHEDULER_MODE;
typedef enum
{
    GPCPFE_SCHEDULER_CTRL_SCHEDULER_CAPACITY_CTRL_DISABLE= 0,
                                                    // Not use capacity of each index fifo to do scheduler work
    GPCPFE_SCHEDULER_CTRL_SCHEDULER_CAPACITY_CTRL_ENABLE= 1,
                                                    // Use capacity of each index fifo to do scheduler work
} GPCPFE_SCHEDULER_CTRL_SCHEDULER_CAPACITY_CTRL;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////            GPCPFE Block (Block ID = 1) Register Definitions                          ///////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Step_Rate                 : 26;  // StepRate is how many instances to draw before stepping one
                                                      // unit forward in a vertex buffer containing instance Data.
        unsigned int Reserved                  : 6;
    } reg;
} Reg_Gpcpfe_Instance_Ctl;

typedef struct _Group_Gpcpfe_Instance
{
    Reg_Gpcpfe_Instance_Ctl          reg_Gpcpfe_Instance_Ctl;
} Reg_Gpcpfe_Instance_Group;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Mask                      : 32;  // every bit in this register will mark whether the
                                                      // corresponding instance_ctrl register is valid or not.
                                                      //  0: invalid
                                                      //  1: valid
    } reg;
} Reg_Gpcpfe_Inst_Mask;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Scheduler_Mode            : 1;   // How to loop the valid GPC/SLICE
        unsigned int Reserved_0                : 3;   // reserve 3-extra bit, if any new shceduler mode would be
                                                      // designed, we could use this extra field directly
        unsigned int Scheduler_Capacity_Ctrl   : 1;   // Whether we would do the scheduling work based on the
                                                      // capacity of the index FIFO or not
        unsigned int Reserved_1                : 11;  // reserved
        unsigned int Fifolimit                 : 8;   // A limitation of the fifo depth of GPCPFE's index fifo. The
                                                      // real fifo depth is 256 currently. If a GPC's index fifo
                                                      // available capacity is less than this provided number,
                                                      // this GPC cannot attend the scheduler. This number should
                                                      // be calculated by Driver based on teh batch size setting to
                                                      // CSP.
        unsigned int Reserved_2                : 8;   // reserved
    } reg;
} Reg_Gpcpfe_Scheduler_Ctrl;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Reserved                  : 32;  // RESERVED
    } reg;
} Reg_Gpcpfe_Reserved;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Quotient                  : 32;  // The quotient value of the IID
    } reg;
} Reg_Cnt0;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Remainder                 : 32;  // The remainder value of the IID
    } reg;
} Reg_Cnt1;

typedef struct _Group_Gpcpfe_Iidcnt
{
    Reg_Cnt0                         reg_Cnt0;
    Reg_Cnt1                         reg_Cnt1;
} Reg_Gpcpfe_Iidcnt_Group;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Iid                       : 32;  // Instance ID value
    } reg;
} Reg_Gpcpfe_Iid;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Pid                       : 32;  // Primitive ID value
    } reg;
} Reg_Gpcpfe_Pid;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int X                         : 32;  // X Component
    } reg;
} Reg_Globalx;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Y                         : 32;  // Y component
    } reg;
} Reg_Globaly;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Z                         : 32;  // Z component
    } reg;
} Reg_Globalz;

typedef struct _Group_Gpcpfe_Gp_Global
{
    Reg_Globalx                      reg_Globalx;
    Reg_Globaly                      reg_Globaly;
    Reg_Globalz                      reg_Globalz;
} Reg_Gpcpfe_Gp_Global_Group;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int X                         : 32;  // X Component
    } reg;
} Reg_Groupx;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Y                         : 32;  // Y component
    } reg;
} Reg_Groupy;

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Z                         : 32;  // Z component
    } reg;
} Reg_Groupz;

typedef struct _Group_Gpcpfe_Gp_Group
{
    Reg_Groupx                       reg_Groupx;
    Reg_Groupy                       reg_Groupy;
    Reg_Groupz                       reg_Groupz;
} Reg_Gpcpfe_Gp_Group_Group;

typedef struct _Gpcpfe_regs
{
    Reg_Gpcpfe_Instance_Group        reg_Gpcpfe_Instance[32];
    Reg_Gpcpfe_Inst_Mask             reg_Gpcpfe_Inst_Mask;
    Reg_Gpcpfe_Scheduler_Ctrl        reg_Gpcpfe_Scheduler_Ctrl;
    Reg_Gpcpfe_Reserved              reg_Gpcpfe_Reserved[6];
    Reg_Gpcpfe_Iidcnt_Group          reg_Gpcpfe_Iidcnt[32];
    Reg_Gpcpfe_Iid                   reg_Gpcpfe_Iid;
    Reg_Gpcpfe_Pid                   reg_Gpcpfe_Pid;
    Reg_Gpcpfe_Gp_Global_Group       reg_Gpcpfe_Gp_Global;
    Reg_Gpcpfe_Gp_Group_Group        reg_Gpcpfe_Gp_Group;
} Gpcpfe_regs;

#endif
