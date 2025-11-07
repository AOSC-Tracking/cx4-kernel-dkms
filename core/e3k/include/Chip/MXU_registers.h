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
//    Spec Last Modified Time: 12:55 2020-05-14
#ifndef _MXU_REGISTER_H
#define _MXU_REGISTER_H


#ifndef        MXU_BLOCKBASE_INF
#define    MXU_BLOCKBASE_INF
#define    BLOCK_MXU_VERSION 1
#define    BLOCK_MXU_TIMESTAMP "12:55 2020-05-14"
#define    MXU_BLOCK                                           0xc // match with BlockID.h
#define    MXU_REG_START                                       0x0 // match with BlockID.h
#define    MXU_REG_END                                         0x184b // match with BlockID.h
#define    MXU_REG_LIMIT                                       0x184b // match with BlockID.h
#endif

// Register Offset definition
#define        Reg_Fb_Ctrl_Offset                                         0x0
#define        Reg_Mmu_Mode_Offset                                        0x1
#define        Reg_Rserved_0_Offset                                       0x2
#define        Reg_Bl_Size_Offset                                         0x3
#define        Reg_Gart_Base_Offset                                       0x4
#define        Reg_Proc_Sec_Offset                                        0x5
#define        Reg_Pt_Inv_Addr_Offset                                     0x6
#define        Reg_Pt_Inv_Mask_Offset                                     0x7
#define        Reg_Pt_Inv_Trig_Offset                                     0x8
#define        Reg_Mxu_Channel_Control_Offset                             0x9
#define        Reg_Mxu_Dec_Ctrl_Offset                                    0xa
#define        Reg_Bus_Id_Mode_Offset                                     0xb
#define        Reg_Mxu_Ctrl_0_Offset                                      0xc
#define        Reg_Mxu_Ila_Reg_Offset                                     0xd
#define        Reg_Mxu_Int_Status_Offset                                  0xe
#define        Reg_Mxu_Int_Mask_Offset                                    0xf
#define        Reg_Mxu_Page_Fault_Status0_Offset                          0x10
#define        Reg_Mxu_Page_Fault_Status1_Offset                          0x11
#define        Reg_Mem_Addr_For_Page_Fault_Offset                         0x12
#define        Reg_Level_1_Fault_Page_Addr_Offset                         0x13
#define        Reg_Level_2_Fault_Page_Addr_Offset                         0x14
#define        Reg_Rb0_Fl2_Offset                                         0x15
#define        Reg_Rb1_Fl2_Offset                                         0x16
#define        Reg_Rb2_Fl2_Offset                                         0x17
#define        Reg_Argument_Fl2_Offset                                    0x18
#define        Reg_L2_Performance_Redundancy_Offset                       0x19
#define        Reg_Page_Fault_Level_Offset                                0x1a
#define        Reg_Pending_Buf_Ctrl_0_Offset                              0x1b
#define        Reg_Pending_Buf_Ctrl_1_Offset                              0x1c
#define        Reg_Rdata_Buf_Ctrl_0_Offset                                0x1d
#define        Reg_Diu_Reserve_Ctrl_Offset                                0x1e
#define        Reg_Bium_Hdr_Num_Offset                                    0x1f
#define        Reg_Mxu_Reserved_Offset                                    0x20
#define        Reg_Mxu_Dummy_Fence_Addr_Gmi0_Offset                       0x30
#define        Reg_Mxu_Dummy_Fence_Addr_Gmi1_Offset                       0x31
#define        Reg_Mxu_Dummy_Fence_Addr_Bium_Offset                       0x32
#define        Reg_Mxu_Reserved1_Offset                                   0x33
#define        Reg_Fb_Ctrl1_Offset                                        0x40
#define        Reg_Mmu_Mode1_Offset                                       0x41
#define        Reg_Diu_Ctrl1_Offset                                       0x42
#define        Reg_Bl_Size1_Offset                                        0x43
#define        Reg_Mxu_Reserved2_Offset                                   0x44
#define        Reg_Mxu_Channel_Control1_Offset                            0x49
#define        Reg_Mxu_Dec_Ctrl1_Offset                                   0x4a
#define        Reg_Mxu_Resvered_For_Sila_Offset                           0x4b

// Block constant definition
typedef enum
{
    BL_SIZE_BUFFER_SIZE_8MB                  = 0,
    BL_SIZE_BUFFER_SIZE_16MB                 = 1,
    BL_SIZE_BUFFER_SIZE_32MB                 = 2,
    BL_SIZE_BUFFER_SIZE_64MB                 = 3,
    BL_SIZE_BUFFER_SIZE_128MB                = 4,
    BL_SIZE_BUFFER_SIZE_256MB                = 5,
    BL_SIZE_BUFFER_SIZE_RESERVED1            = 6,
    BL_SIZE_BUFFER_SIZE_RESERVED2            = 7,
} BL_SIZE_BUFFER_SIZE;

typedef enum
{
    PT_INV_TRIG_TARGET_OFF                   = 0,
    PT_INV_TRIG_TARGET_UTLB                  = 1,
    PT_INV_TRIG_TARGET_DTLB                  = 2,
    PT_INV_TRIG_TARGET_RESERVED              = 3,
} PT_INV_TRIG_TARGET;

typedef enum
{
    MXU_CHANNEL_CONTROL_CH_SIZE_256B         = 0,
    MXU_CHANNEL_CONTROL_CH_SIZE_512B         = 1,
    MXU_CHANNEL_CONTROL_CH_SIZE_1KB          = 2,
    MXU_CHANNEL_CONTROL_CH_SIZE_RESERVED0    = 3,
    MXU_CHANNEL_CONTROL_CH_SIZE_RESERVED1    = 4,
    MXU_CHANNEL_CONTROL_CH_SIZE_RESERVED2    = 5,
    MXU_CHANNEL_CONTROL_CH_SIZE_RESERVED3    = 6,
    MXU_CHANNEL_CONTROL_CH_SIZE_RESERVED4    = 7,
} MXU_CHANNEL_CONTROL_CH_SIZE;

typedef enum
{
    BL_SIZE1_BUFFER_SIZE1_8MB                = 0,
    BL_SIZE1_BUFFER_SIZE1_16MB               = 1,
    BL_SIZE1_BUFFER_SIZE1_32MB               = 2,
    BL_SIZE1_BUFFER_SIZE1_64MB               = 3,
    BL_SIZE1_BUFFER_SIZE1_128MB              = 4,
    BL_SIZE1_BUFFER_SIZE1_256MB              = 5,
    BL_SIZE1_BUFFER_SIZE1_RESERVED1          = 6,
    BL_SIZE1_BUFFER_SIZE1_RESERVED2          = 7,
} BL_SIZE1_BUFFER_SIZE1;

typedef enum
{
    MXU_CHANNEL_CONTROL1_CH_SIZE1_256B       = 0,
    MXU_CHANNEL_CONTROL1_CH_SIZE1_512B       = 1,
    MXU_CHANNEL_CONTROL1_CH_SIZE1_1KB        = 2,
    MXU_CHANNEL_CONTROL1_CH_SIZE1_RESERVED0  = 3,
    MXU_CHANNEL_CONTROL1_CH_SIZE1_RESERVED1  = 4,
    MXU_CHANNEL_CONTROL1_CH_SIZE1_RESERVED2  = 5,
    MXU_CHANNEL_CONTROL1_CH_SIZE1_RESERVED3  = 6,
    MXU_CHANNEL_CONTROL1_CH_SIZE1_RESERVED4  = 7,
} MXU_CHANNEL_CONTROL1_CH_SIZE1;


 ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////            MXU Block (Block ID = 0xc) Register Definitions                      ///////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define Reg_FB_CTRL_Reserved_BitField_MaxValue ((1u << 32) - 1)

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Reserved                  : 32;
    } reg;
} Reg_Fb_Ctrl;

#define Reg_MMU_MODE_Vmen_BitField_MaxValue ((1u << 1) - 1)
#define Reg_MMU_MODE_Reserved0_BitField_MaxValue ((1u << 3) - 1)
#define Reg_MMU_MODE_Bius_Va_En_BitField_MaxValue ((1u << 1) - 1)
#define Reg_MMU_MODE_Reserved1_BitField_MaxValue ((1u << 3) - 1)
#define Reg_MMU_MODE_Video_Size_BitField_MaxValue ((1u << 8) - 1)
#define Reg_MMU_MODE_Legacy_Size_BitField_MaxValue ((1u << 8) - 1)
#define Reg_MMU_MODE_Legacy_Offset_BitField_MaxValue ((1u << 8) - 1)

typedef union
{
    unsigned int uint;
    struct
    {

        unsigned int Vmen : 1;           // when it's 0, we would use Physical Memory, which would be
                                         // similar with The old Elite series, with local + system.
        unsigned int Reserved0 : 3;
        unsigned int Bius_Va_En : 1;     // whether the address coming from CPU(BIUS) would go
                                         // through MMU(a GPU-VA) or not. Default it's 0, which measn
                                         // the CPU will send GPUPA idrectly. The value 1 is used maily
                                         // for debug purpose for MMU translation work.
        unsigned int Reserved1 : 3;
        unsigned int Video_Size : 8;     // The video memory(or called local memory sometime)'s
                                         // size, in a granul of 256MB. The video memory's size should
                                         // be used both in virtual memory model and physical memory
                                         // model.
        unsigned int Legacy_Size : 8;    // The legacy's range's total size. In a granule size of 4GB.
                                         // To support the maximum 40-bit address, 8-bit size would be
                                         // enough.
        unsigned int Legacy_Offset : 8;  // The legacy's range's start, also in a granule size of 4GB.
                                         //  The legacy range would only be used when VMEN is 1, aka, the
                                         // Virtual Memory Model. When in Physical memory model, the
                                         // legacy_offset and legacy_size would be of no use.
    } reg;
} Reg_Mmu_Mode;

#define Reg_RSERVED_0_Reserved_BitField_MaxValue ((1u << 32) - 1)

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Reserved                  : 32;
    } reg;
} Reg_Rserved_0;

#define Reg_BL_SIZE_Buffer_Size_BitField_MaxValue ((1u << 3) - 1)
#define Reg_BL_SIZE_Reserved_BitField_MaxValue ((1u << 1) - 1)
#define Reg_BL_SIZE_Buffer_Offset_BitField_MaxValue ((1u << 19) - 1)
#define Reg_BL_SIZE_Reserved0_BitField_MaxValue ((1u << 1) - 1)
#define Reg_BL_SIZE_Invalid_Bl_To_Rf_En_BitField_MaxValue ((1u << 1) - 1)
#define Reg_BL_SIZE_Reserved1_BitField_MaxValue ((1u << 7) - 1)

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Buffer_Size               : 3;
        unsigned int Reserved                  : 1;
        unsigned int Buffer_Offset             : 19; // The BL buffer's offset inner the local memory, it could
                                                     // only be set just after the chip is reset, and cannot be
                                                     // touched when chip is running.
                                                     //  It would aligned to 64kB.
        unsigned int Reserved0                 : 1;
        unsigned int Invalid_Bl_To_Rf_En       : 1;
        unsigned int Reserved1                 : 7;
    } reg;
} Reg_Bl_Size;

#define Reg_GART_BASE_Reserved_BitField_MaxValue ((1u << 8) - 1)
#define Reg_GART_BASE_Addr_BitField_MaxValue ((1u << 24) - 1)

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Reserved                  : 8;
        unsigned int Addr                      : 24;  // The ADDR will point to the GART, it must be in the local
                                                      // memory.
    } reg;
} Reg_Gart_Base;

#define Reg_PROC_SEC_Sec_Status_BitField_MaxValue ((1u << 16) - 1)
#define Reg_PROC_SEC_Reserved_BitField_MaxValue ((1u << 16) - 1)

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Sec_Status                : 16;
        unsigned int Reserved                  : 16;
    } reg;
} Reg_Proc_Sec;

#define Reg_PT_INV_ADDR_Address_BitField_MaxValue ((1u << 28) - 1)
#define Reg_PT_INV_ADDR_Proc_Id_BitField_MaxValue ((1u << 4) - 1)

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Address                   : 28;  // The virtual address used to be invalidation. Aligned to
                                                      // 4kB.
        unsigned int Proc_Id                   : 4;   // The given process space ID. This field is of no use when in
                                                      // MMU_Mode.VMEN is 0, aka, in physical memory model.
    } reg;
} Reg_Pt_Inv_Addr;

#define Reg_PT_INV_MASK_Mask_BitField_MaxValue ((1u << 28) - 1)
#define Reg_PT_INV_MASK_Proc_Id_BitField_MaxValue ((1u << 4) - 1)

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Mask                      : 28;  // The virtual address mask used to be invalidation. Has the
                                                      // same aligned grnaule with PT_INV_ADDR.Address
        unsigned int Proc_Id                   : 4;   // The given process space ID. This field is of no use when in
                                                      // MMU_Mode.VMEN is 0, aka, in physical memory model.
    } reg;
} Reg_Pt_Inv_Mask;

#define Reg_PT_INV_TRIG_Proc_Id_BitField_MaxValue ((1u << 4) - 1)
#define Reg_PT_INV_TRIG_Addr_Valid_BitField_MaxValue ((1u << 1) - 1)
#define Reg_PT_INV_TRIG_Target_BitField_MaxValue ((1u << 2) - 1)
#define Reg_PT_INV_TRIG_Reserved_BitField_MaxValue ((1u << 25) - 1)

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Proc_Id                   : 4;   // The given process space ID. This field is of no use when in
                                                      // MMU_Mode.VMEN is 0, aka, in physical memory model
        unsigned int Addr_Valid                : 1;   // Whether the invalidation will use the Address value or
                                                      // not. If this is invalid, the invlalidation will only
                                                      // operation on the full given process space..
        unsigned int Target                    : 2;   // invalid pte cache.
        unsigned int Reserved                  : 25;
    } reg;
} Reg_Pt_Inv_Trig;

#define Reg_MXU_CHANNEL_CONTROL_Miu_Channel0_Disable_BitField_MaxValue ((1u << 1) - 1)
#define Reg_MXU_CHANNEL_CONTROL_Miu_Channel1_Disable_BitField_MaxValue ((1u << 1) - 1)
#define Reg_MXU_CHANNEL_CONTROL_Miu_Channel2_Disable_BitField_MaxValue ((1u << 1) - 1)
#define Reg_MXU_CHANNEL_CONTROL_Ch_Size_BitField_MaxValue ((1u << 3) - 1)
#define Reg_MXU_CHANNEL_CONTROL_L2_Internal_Flush_Eco_Disable_BitField_MaxValue ((1u << 1) - 1)
#define Reg_MXU_CHANNEL_CONTROL_Reserved_BitField_MaxValue ((1u << 25) - 1)

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Miu_Channel0_Disable      : 1;  // miu channel0 disable
        unsigned int Miu_Channel1_Disable      : 1;  // miu channel1 disable
        unsigned int Miu_Channel2_Disable      : 1;  // miu channel2 disable
        unsigned int Ch_Size                   : 3;  // normal channel selection.
        unsigned int L2_Internal_Flush_Eco_Disable: 1;
        unsigned int Reserved                  : 25;
    } reg;
} Reg_Mxu_Channel_Control;

#define Reg_MXU_DEC_CTRL_Ddec_Antilock_Time_Threshold_BitField_MaxValue ((1u << 3) - 1)
#define Reg_MXU_DEC_CTRL_Ddec_Antilock_BitField_MaxValue ((1u << 1) - 1)
#define Reg_MXU_DEC_CTRL_Reserved0_BitField_MaxValue ((1u << 23) - 1)
#define Reg_MXU_DEC_CTRL_Rf_High_Priority_Than_Ddec_BitField_MaxValue ((1u << 1) - 1)
#define Reg_MXU_DEC_CTRL_Miu0_1_Bypass_Priority_Than_Miu2_To_Diu_BitField_MaxValue ((1u << 1) - 1)
#define Reg_MXU_DEC_CTRL_Miu0_1_Bypass_High_Priority_Than_Miu2_To_L2_BitField_MaxValue ((1u << 1) - 1)
#define Reg_MXU_DEC_CTRL_Bypass_Rf_Ddec_Fix_Priority_BitField_MaxValue ((1u << 1) - 1)
#define Reg_MXU_DEC_CTRL_Reserved1_BitField_MaxValue ((1u << 1) - 1)

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Ddec_Antilock_Time_Threshold: 3; // 0: 9bit counters
                                                      //  1: 8bits counter
                                                      //  2: 7bits
                                                      //  3: 6bits
                                                      //  4: 5bnits
                                                      //  5: 4bits
        unsigned int Ddec_Antilock             : 1;   //  0: disable this feature.
                                                      //  1: enable DDEC anti-hang feature even if the BL and data is
                                                      // not matched.
        unsigned int Reserved0                 : 23;
        unsigned int Rf_High_Priority_Than_Ddec: 1;
        unsigned int Miu0_1_Bypass_Priority_Than_Miu2_To_Diu: 1;
        unsigned int Miu0_1_Bypass_High_Priority_Than_Miu2_To_L2: 1;
        unsigned int Bypass_Rf_Ddec_Fix_Priority: 1;
        unsigned int Reserved1                 : 1;
    } reg;
} Reg_Mxu_Dec_Ctrl;

#define Reg_BUS_ID_MODE_Biu_Axi_Id_Mode_BitField_MaxValue ((1u << 1) - 1)
#define Reg_BUS_ID_MODE_Reserved0_BitField_MaxValue ((1u << 31) - 1)

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Biu_Axi_Id_Mode           : 1;     // 0: fixed ID for agent.
                                                        //  1: dynamic ID for agent.
        unsigned int Reserved0                 : 31;
    } reg;
} Reg_Bus_Id_Mode;

#define Reg_MXU_CTRL_0_Reserved0_BitField_MaxValue ((1u << 1) - 1)
#define Reg_MXU_CTRL_0_Reseved1_BitField_MaxValue ((1u << 2) - 1)
#define Reg_MXU_CTRL_0_Reserved3_BitField_MaxValue ((1u << 2) - 1)
#define Reg_MXU_CTRL_0_Reserved2_BitField_MaxValue ((1u << 27) - 1)

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Reserved0                 : 1;   // RESERVED
        unsigned int Reseved1                  : 2;   // 0: 2KB
                                                      //  1: 4KB
                                                      //  2: 8KB
                                                      //  3: 16KB
        unsigned int Reserved3                 : 2;   // 0: 4banks
                                                      //  1: 8banks
                                                      //  2: 16banks
                                                      //  3: reserved.
        unsigned int Reserved2                 : 27;  // RESERVED
    } reg;
} Reg_Mxu_Ctrl_0;

#define Reg_MXU_ILA_REG_Latency_BitField_MaxValue ((1u << 10) - 1)
#define Reg_MXU_ILA_REG_Reserve_BitField_MaxValue ((1u << 22) - 1)

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Latency                   : 10;    // Req number that latency is > X (configurable). The latency
                                                        // is from request going into MIU to data return to MXU
        unsigned int Reserve                   : 22;
    } reg;
} Reg_Mxu_Ila_Reg;

#define Reg_MXU_INT_STATUS_Page_Fault_BitField_MaxValue ((1u << 30) - 1)
#define Reg_MXU_INT_STATUS_Reserved1_BitField_MaxValue ((1u << 1) - 1)
#define Reg_MXU_INT_STATUS_Reserved0_BitField_MaxValue ((1u << 1) - 1)

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Page_Fault                : 30;    // page fault cause interrupt
        unsigned int Reserved1                 : 1;
        unsigned int Reserved0                 : 1;
    } reg;
} Reg_Mxu_Int_Status;

#define Reg_MXU_INT_MASK_Reserved0_BitField_MaxValue ((1u << 30) - 1)
#define Reg_MXU_INT_MASK_Reserved1_BitField_MaxValue ((1u << 1) - 1)
#define Reg_MXU_INT_MASK_Reserved2_BitField_MaxValue ((1u << 1) - 1)

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Reserved0                 : 30;
        unsigned int Reserved1                 : 1;
        unsigned int Reserved2                 : 1;
    } reg;
} Reg_Mxu_Int_Mask;

#define Reg_MXU_PAGE_FAULT_STATUS0_Reserved_BitField_MaxValue ((1u << 18) - 1)
#define Reg_MXU_PAGE_FAULT_STATUS0_Fault_Level_BitField_MaxValue ((1u << 2) - 1)
#define Reg_MXU_PAGE_FAULT_STATUS0_Proc_Id_BitField_MaxValue ((1u << 4) - 1)
#define Reg_MXU_PAGE_FAULT_STATUS0_Va_BitField_MaxValue ((1u << 8) - 1)

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Va                        : 8; // The VA[39:32] of the virtual address which cuase page
        unsigned int Proc_Id                   : 4; // The process id of the address caued the page fault
        unsigned int Fault_Level               : 2; // which level's translation cause the page fault:
                                                    //  0: TTBR casue page fault
                                                    //  1: UTLB cause page fault
                                                    //  2: DTLB cause page fault
                                                    //  attention that the page fault always report TLB's case
        unsigned int Reserved                  : 18;
    } reg;
} Reg_Mxu_Page_Fault_Status0;

#define Reg_MXU_PAGE_FAULT_STATUS1_Va_BitField_MaxValue ((1u << 32) - 1)

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Va                        : 32;// The VA[31:0] of the virtual address which cuase page fault

    } reg;
} Reg_Mxu_Page_Fault_Status1;

#define Reg_MEM_ADDR_FOR_PAGE_FAULT_Reserved_BitField_MaxValue ((1u << 2) - 1)
#define Reg_MEM_ADDR_FOR_PAGE_FAULT_Va_BitField_MaxValue ((1u << 30) - 1)

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Reserved                  : 2;   // reserved
        unsigned int Va                        : 30;  // mem_addr[35:6] , if mxu get invalid page address, mxu use
                                                      // this address to read page table
    } reg;
} Reg_Mem_Addr_For_Page_Fault;

#define Reg_LEVEL_1_FAULT_PAGE_ADDR_Va_BitField_MaxValue ((1u << 28) - 1)
#define Reg_LEVEL_1_FAULT_PAGE_ADDR_Proc_Id_BitField_MaxValue ((1u << 4) - 1)

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Va                        : 28;// L1 fault page address
        unsigned int Proc_Id                   : 4;
    } reg;
} Reg_Level_1_Fault_Page_Addr;

#define Reg_LEVEL_2_FAULT_PAGE_ADDR_Va_BitField_MaxValue ((1u << 28) - 1)
#define Reg_LEVEL_2_FAULT_PAGE_ADDR_Proc_Id_BitField_MaxValue ((1u << 4) - 1)

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Va                        : 28;// L2 fault page address
        unsigned int Proc_Id                   : 4;
    } reg;
} Reg_Level_2_Fault_Page_Addr;

#define Reg_RB0_FL2_Base_Addr_BitField_MaxValue ((1u << 20) - 1)
#define Reg_RB0_FL2_Size_BitField_MaxValue ((1u << 12) - 1)

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Base_Addr                 : 20;
        unsigned int Size                      : 12;
    } reg;
} Reg_Rb0_Fl2;

#define Reg_RB1_FL2_Base_Addr_BitField_MaxValue ((1u << 20) - 1)
#define Reg_RB1_FL2_Size_BitField_MaxValue ((1u << 12) - 1)

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Base_Addr                 : 20;
        unsigned int Size                      : 12;
    } reg;
} Reg_Rb1_Fl2;

#define Reg_RB2_FL2_Base_Addr_BitField_MaxValue ((1u << 20) - 1)
#define Reg_RB2_FL2_Size_BitField_MaxValue ((1u << 12) - 1)

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Base_Addr                 : 20;
        unsigned int Size                      : 12;
    } reg;
} Reg_Rb2_Fl2;

#define Reg_ARGUMENT_FL2_Base_Addr_BitField_MaxValue ((1u << 20) - 1)
#define Reg_ARGUMENT_FL2_Size_BitField_MaxValue ((1u << 12) - 1)

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Base_Addr                 : 20;  // argument buffer base address, aligned to 64KB
        unsigned int Size                      : 12;  // size of the argument buffer, aligned to 64KB
    } reg;
} Reg_Argument_Fl2;

#define Reg_L2_PERFORMANCE_REDUNDANCY_Reserved_BitField_MaxValue ((1u << 32) - 1)

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Reserved                  : 32;
    } reg;
} Reg_L2_Performance_Redundancy;

#define Reg_PAGE_FAULT_LEVEL_Mmu_L1_Invalid_BitField_MaxValue ((1u << 1) - 1)
#define Reg_PAGE_FAULT_LEVEL_Mmu_L2_Invalid_BitField_MaxValue ((1u << 1) - 1)
#define Reg_PAGE_FAULT_LEVEL_Reserved0_BitField_MaxValue ((1u << 1) - 1)
#define Reg_PAGE_FAULT_LEVEL_Sec_Fault_Valid_BitField_MaxValue ((1u << 1) - 1)
#define Reg_PAGE_FAULT_LEVEL_Reserved1_BitField_MaxValue ((1u << 28) - 1)

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Mmu_L1_Invalid     : 1;   // level_1 invalid
        unsigned int Mmu_L2_Invalid     : 1;   // level_2 invalid
        unsigned int Reserved0          : 1;   // reserved
        unsigned int Sec_Fault_Valid    : 1;   // level_2 invalid
        unsigned int Reserved1          : 28;  // reserved
    } reg;
} Reg_Page_Fault_Level;

#define Reg_PENDING_BUF_CTRL_0_Ctrl_0_BitField_MaxValue ((1u << 32) - 1)

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Ctrl_0                    : 32;
    } reg;
} Reg_Pending_Buf_Ctrl_0;

#define Reg_PENDING_BUF_CTRL_1_Ctrl_1_BitField_MaxValue ((1u << 32) - 1)

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Ctrl_1                    : 32;
    } reg;
} Reg_Pending_Buf_Ctrl_1;

#define Reg_RDATA_BUF_CTRL_0_Rdata_Ctrl_0_BitField_MaxValue ((1u << 16) - 1)
#define Reg_RDATA_BUF_CTRL_0_Reserve_BitField_MaxValue ((1u << 16) - 1)

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Rdata_Ctrl_0              : 16;
        unsigned int Reserve                   : 16;
    } reg;
} Reg_Rdata_Buf_Ctrl_0;

#define Reg_DIU_RESERVE_CTRL_Wr_Entry_Reserve_For_Diu_BitField_MaxValue ((1u << 3) - 1)
#define Reg_DIU_RESERVE_CTRL_Diu_Rdata_Buf_Cnt_Ctrl_En_BitField_MaxValue ((1u << 1) - 1)
#define Reg_DIU_RESERVE_CTRL_Diu_Rdatabuf_Osd_BitField_MaxValue ((1u << 12) - 1)
#define Reg_DIU_RESERVE_CTRL_Diu_Bius_Wdatabuf_Reserve_BitField_MaxValue ((1u << 8) - 1)
#define Reg_DIU_RESERVE_CTRL_Biu_Wreqbuf_Reserve_BitField_MaxValue ((1u << 3) - 1)
#define Reg_DIU_RESERVE_CTRL_Biu_Rreqbuf_Reserve_BitField_MaxValue ((1u << 3) - 1)
#define Reg_DIU_RESERVE_CTRL_Reserve_BitField_MaxValue ((1u << 2) - 1)

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Wr_Entry_Reserve_For_Diu  : 3;
        unsigned int Diu_Rdata_Buf_Cnt_Ctrl_En : 1;
        unsigned int Diu_Rdatabuf_Osd          : 12;
        unsigned int Diu_Bius_Wdatabuf_Reserve : 8;
        unsigned int Biu_Wreqbuf_Reserve       : 3;
        unsigned int Biu_Rreqbuf_Reserve       : 3;
        unsigned int Reserve                   : 2;
    } reg;
} Reg_Diu_Reserve_Ctrl;

#define Reg_BIUM_HDR_NUM_Reg_Bium_Rd_Hdr_BitField_MaxValue ((1u << 8) - 1)
#define Reg_BIUM_HDR_NUM_Reg_Diu_Wr_Hdr_BitField_MaxValue ((1u << 4) - 1)
#define Reg_BIUM_HDR_NUM_Reserve0_BitField_MaxValue ((1u << 4) - 1)
#define Reg_BIUM_HDR_NUM_Wr_Combine_Disable_BitField_MaxValue ((1u << 1) - 1)
#define Reg_BIUM_HDR_NUM_Return_Data_Path_Disable_BitField_MaxValue ((1u << 1) - 1)
#define Reg_BIUM_HDR_NUM_Reserve1_BitField_MaxValue ((1u << 14) - 1)

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Reg_Bium_Rd_Hdr           : 8;
        unsigned int Reg_Diu_Wr_Hdr            : 4;
        unsigned int Reserve0                  : 4;
        unsigned int Wr_Combine_Disable        : 1;
        unsigned int Return_Data_Path_Disable  : 1;
        unsigned int Reserve1                  : 14;
    } reg;
} Reg_Bium_Hdr_Num;

#define Reg_MXU_RESERVED_Reserved_BitField_MaxValue ((1u << 32) - 1)

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Reserved                  : 32;
    } reg;
} Reg_Mxu_Reserved;

#define Reg_MXU_DUMMY_FENCE_ADDR_GMI0_Address_BitField_MaxValue ((1u << 32) - 1)

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Address                   : 32;
    } reg;
} Reg_Mxu_Dummy_Fence_Addr_Gmi0;

#define Reg_MXU_DUMMY_FENCE_ADDR_GMI1_Address_BitField_MaxValue ((1u << 32) - 1)

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Address                   : 32;
    } reg;
} Reg_Mxu_Dummy_Fence_Addr_Gmi1;

#define Reg_MXU_DUMMY_FENCE_ADDR_BIUM_Address_BitField_MaxValue ((1u << 32) - 1)

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Address                   : 32;
    } reg;
} Reg_Mxu_Dummy_Fence_Addr_Bium;

#define Reg_MXU_RESERVED1_Reserved_BitField_MaxValue ((1u << 32) - 1)

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Reserved                  : 32;
    } reg;
} Reg_Mxu_Reserved1;

#define Reg_FB_CTRL1_Reg_Osd_Num_BitField_MaxValue ((1u << 7) - 1)
#define Reg_FB_CTRL1_Reg_Dis_Rdata_Block_BitField_MaxValue ((1u << 1) - 1)
#define Reg_FB_CTRL1_Reserved0_BitField_MaxValue ((1u << 20) - 1)
#define Reg_FB_CTRL1_Allbe_Cfg_BitField_MaxValue ((1u << 2) - 1)
#define Reg_FB_CTRL1_Reserved1_BitField_MaxValue ((1u << 2) - 1)

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Reg_Osd_Num               : 7;
        unsigned int Reg_Dis_Rdata_Block       : 1;
        unsigned int Reserved0                 : 20;
        unsigned int Allbe_Cfg                 : 2;
        unsigned int Reserved1                 : 2;
    } reg;
} Reg_Fb_Ctrl1;

#define Reg_MMU_MODE1_Vmen_BitField_MaxValue ((1u << 1) - 1)
#define Reg_MMU_MODE1_Reserved0_BitField_MaxValue ((1u << 3) - 1)
#define Reg_MMU_MODE1_Bius_Va_En_BitField_MaxValue ((1u << 1) - 1)
#define Reg_MMU_MODE1_Reserved1_BitField_MaxValue ((1u << 3) - 1)
#define Reg_MMU_MODE1_Video_Size_BitField_MaxValue ((1u << 8) - 1)
#define Reg_MMU_MODE1_Legacy_Size_BitField_MaxValue ((1u << 8) - 1)
#define Reg_MMU_MODE1_Legacy_Offset_BitField_MaxValue ((1u << 8) - 1)

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Vmen                      : 1;
        unsigned int Reserved0                 : 3;
        unsigned int Bius_Va_En                : 1;
        unsigned int Reserved1                 : 3;
        unsigned int Video_Size                : 8;
        unsigned int Legacy_Size               : 8;
        unsigned int Legacy_Offset             : 8;
    } reg;
} Reg_Mmu_Mode1;

#define Reg_DIU_CTRL1_Biu_Hpri_En_BitField_MaxValue ((1u << 1) - 1)
#define Reg_DIU_CTRL1_Ptc_Hpri_En_BitField_MaxValue ((1u << 1) - 1)
#define Reg_DIU_CTRL1_Blc_Hpri_En_BitField_MaxValue ((1u << 1) - 1)
#define Reg_DIU_CTRL1_Diu_Timeout_Check_Bl_BitField_MaxValue ((1u << 1) - 1)
#define Reg_DIU_CTRL1_Diu_Timeout_Cnt_BitField_MaxValue ((1u << 8) - 1)
#define Reg_DIU_CTRL1_Reserved_BitField_MaxValue ((1u << 20) - 1)

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Biu_Hpri_En               : 1;
        unsigned int Ptc_Hpri_En               : 1;
        unsigned int Blc_Hpri_En               : 1;
        unsigned int Diu_Timeout_Check_Bl      : 1;
        unsigned int Diu_Timeout_Cnt           : 8;
        unsigned int Reserved                  : 20;
    } reg;
} Reg_Diu_Ctrl1;

#define Reg_BL_SIZE1_Buffer_Size1_BitField_MaxValue ((1u << 3) - 1)
#define Reg_BL_SIZE1_Reserved_BitField_MaxValue ((1u << 1) - 1)
#define Reg_BL_SIZE1_Buffer_Offset_BitField_MaxValue ((1u << 19) - 1)
#define Reg_BL_SIZE1_Reserved0_BitField_MaxValue ((1u << 1) - 1)
#define Reg_BL_SIZE1_Invalid_Bl_To_Rf_En_BitField_MaxValue ((1u << 1) - 1)
#define Reg_BL_SIZE1_Reserved1_BitField_MaxValue ((1u << 7) - 1)

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Buffer_Size1              : 3;
        unsigned int Reserved                  : 1;
        unsigned int Buffer_Offset             : 19;
        unsigned int Reserved0                 : 1;
        unsigned int Invalid_Bl_To_Rf_En       : 1;
        unsigned int Reserved1                 : 7;
    } reg;
} Reg_Bl_Size1;

#define Reg_MXU_RESERVED2_Reserved_BitField_MaxValue ((1u << 32) - 1)

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Reserved : 32;
    } reg;
} Reg_Mxu_Reserved2;

#define Reg_MXU_CHANNEL_CONTROL1_Miu_Channel0_Disable_BitField_MaxValue ((1u << 1) - 1)
#define Reg_MXU_CHANNEL_CONTROL1_Miu_Channel1_Disable_BitField_MaxValue ((1u << 1) - 1)
#define Reg_MXU_CHANNEL_CONTROL1_Miu_Channel2_Disable_BitField_MaxValue ((1u << 1) - 1)
#define Reg_MXU_CHANNEL_CONTROL1_Ch_Size1_BitField_MaxValue ((1u << 3) - 1)
#define Reg_MXU_CHANNEL_CONTROL1_L2_Internal_Flush_Eco_Disable_BitField_MaxValue ((1u << 1) - 1)
#define Reg_MXU_CHANNEL_CONTROL1_Reserved_BitField_MaxValue ((1u << 25) - 1)

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Miu_Channel0_Disable      : 1;
        unsigned int Miu_Channel1_Disable      : 1;
        unsigned int Miu_Channel2_Disable      : 1;
        unsigned int Ch_Size1                  : 3;
        unsigned int L2_Internal_Flush_Eco_Disable: 1;
        unsigned int Reserved                  : 25;
    } reg;
} Reg_Mxu_Channel_Control1;

#define Reg_MXU_DEC_CTRL1_Ddec_Antilock_Time_Threshold_BitField_MaxValue ((1u << 3) - 1)
#define Reg_MXU_DEC_CTRL1_Ddec_Antilock_BitField_MaxValue ((1u << 1) - 1)
#define Reg_MXU_DEC_CTRL1_Reserved0_BitField_MaxValue ((1u << 23) - 1)
#define Reg_MXU_DEC_CTRL1_Rf_High_Priority_Than_Ddec_BitField_MaxValue ((1u << 1) - 1)
#define Reg_MXU_DEC_CTRL1_Miu0_1_Bypass_Priority_Than_Miu2_To_Diu_BitField_MaxValue ((1u << 1) - 1)
#define Reg_MXU_DEC_CTRL1_Miu0_1_Bypass_High_Priority_Than_Miu2_To_L2_BitField_MaxValue ((1u << 1) - 1)
#define Reg_MXU_DEC_CTRL1_Bypass_Rf_Ddec_Fix_Priority_BitField_MaxValue ((1u << 1) - 1)
#define Reg_MXU_DEC_CTRL1_Reserved1_BitField_MaxValue ((1u << 1) - 1)

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Ddec_Antilock_Time_Threshold: 3;
        unsigned int Ddec_Antilock             : 1;
        unsigned int Reserved0                 : 23;
        unsigned int Rf_High_Priority_Than_Ddec: 1;
        unsigned int Miu0_1_Bypass_Priority_Than_Miu2_To_Diu: 1;
        unsigned int Miu0_1_Bypass_High_Priority_Than_Miu2_To_L2: 1;
        unsigned int Bypass_Rf_Ddec_Fix_Priority: 1;
        unsigned int Reserved1                 : 1;
    } reg;
} Reg_Mxu_Dec_Ctrl1;

#define Reg_MXU_RESVERED_FOR_SILA_Reserved_BitField_MaxValue ((1u << 32) - 1)

typedef union
{
    unsigned int uint;
    struct
    {
        unsigned int Reserved                  : 32;
    } reg;
} Reg_Mxu_Resvered_For_Sila;

typedef struct _Mxu_regs
{
    Reg_Fb_Ctrl                      reg_Fb_Ctrl;
    Reg_Mmu_Mode                     reg_Mmu_Mode;
    Reg_Rserved_0                    reg_Rserved_0;
    Reg_Bl_Size                      reg_Bl_Size;
    Reg_Gart_Base                    reg_Gart_Base;
    Reg_Proc_Sec                     reg_Proc_Sec;
    Reg_Pt_Inv_Addr                  reg_Pt_Inv_Addr;
    Reg_Pt_Inv_Mask                  reg_Pt_Inv_Mask;
    Reg_Pt_Inv_Trig                  reg_Pt_Inv_Trig;
    Reg_Mxu_Channel_Control          reg_Mxu_Channel_Control;
    Reg_Mxu_Dec_Ctrl                 reg_Mxu_Dec_Ctrl;
    Reg_Bus_Id_Mode                  reg_Bus_Id_Mode;
    Reg_Mxu_Ctrl_0                   reg_Mxu_Ctrl_0;
    Reg_Mxu_Ila_Reg                  reg_Mxu_Ila_Reg;
    Reg_Mxu_Int_Status               reg_Mxu_Int_Status;
    Reg_Mxu_Int_Mask                 reg_Mxu_Int_Mask;
    Reg_Mxu_Page_Fault_Status0       reg_Mxu_Page_Fault_Status0;
    Reg_Mxu_Page_Fault_Status1       reg_Mxu_Page_Fault_Status1;
    Reg_Mem_Addr_For_Page_Fault      reg_Mem_Addr_For_Page_Fault;
    Reg_Level_1_Fault_Page_Addr      reg_Level_1_Fault_Page_Addr;
    Reg_Level_2_Fault_Page_Addr      reg_Level_2_Fault_Page_Addr;
    Reg_Rb0_Fl2                      reg_Rb0_Fl2;
    Reg_Rb1_Fl2                      reg_Rb1_Fl2;
    Reg_Rb2_Fl2                      reg_Rb2_Fl2;
    Reg_Argument_Fl2                 reg_Argument_Fl2;
    Reg_L2_Performance_Redundancy    reg_L2_Performance_Redundancy;
    Reg_Page_Fault_Level             reg_Page_Fault_Level;
    Reg_Pending_Buf_Ctrl_0           reg_Pending_Buf_Ctrl_0;
    Reg_Pending_Buf_Ctrl_1           reg_Pending_Buf_Ctrl_1;
    Reg_Rdata_Buf_Ctrl_0             reg_Rdata_Buf_Ctrl_0;
    Reg_Diu_Reserve_Ctrl             reg_Diu_Reserve_Ctrl;
    Reg_Bium_Hdr_Num                 reg_Bium_Hdr_Num;
    Reg_Mxu_Reserved                 reg_Mxu_Reserved[16];
    Reg_Mxu_Dummy_Fence_Addr_Gmi0    reg_Mxu_Dummy_Fence_Addr_Gmi0;
    Reg_Mxu_Dummy_Fence_Addr_Gmi1    reg_Mxu_Dummy_Fence_Addr_Gmi1;
    Reg_Mxu_Dummy_Fence_Addr_Bium    reg_Mxu_Dummy_Fence_Addr_Bium;
    Reg_Mxu_Reserved1                reg_Mxu_Reserved1[13];
    Reg_Fb_Ctrl1                     reg_Fb_Ctrl1;
    Reg_Mmu_Mode1                    reg_Mmu_Mode1;
    Reg_Diu_Ctrl1                    reg_Diu_Ctrl1;
    Reg_Bl_Size1                     reg_Bl_Size1;
    Reg_Mxu_Reserved2                reg_Mxu_Reserved2[5];
    Reg_Mxu_Channel_Control1         reg_Mxu_Channel_Control1;
    Reg_Mxu_Dec_Ctrl1                reg_Mxu_Dec_Ctrl1;
    Reg_Mxu_Resvered_For_Sila        reg_Mxu_Resvered_For_Sila[6144];
} Mxu_regs;

#endif
