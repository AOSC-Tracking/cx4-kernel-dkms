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
//    Spec Last Modified Time: 2021/11/15 17:31:25
#ifndef _VCP_OPCODE_DECOUPLE_H
#define _VCP_OPCODE_DECOUPLE_H

#ifndef        COMMAND_OPCODES_BLOCKBASE_INF
    #define    COMMAND_OPCODES_BLOCKBASE_INF
    #define    BLOCK_COMMAND_OPCODES_VERSION 1
    #define    BLOCK_COMMAND_OPCODES_TIMESTAMP "2021/11/15 17:31:25"
#endif

// BCI command opercode definition
#define  VCP_OPCODE_Vcp_Skip                0x0            // 0h: SKIP Command
#define  VCP_OPCODE_Vcp_Set_Data_Buf        0x1            // 1h: VCP set data buffer Command
#define  VCP_OPCODE_Vcp_Internal_Wait       0x2            // 2h: Internal Wait Command
#define  VCP_OPCODE_Vcp_Query_Dump          0x3            // 3h: Query Dump Command
#define  VCP_OPCODE_Vcp_Set_Register        0x4            // 4h: Set Register Command
#define  VCP_OPCODE_Vcp_Set_Register_Short  0x5            // 5h: SetRegisterShort Command
#define  VCP_OPCODE_Vcp_Predication         0x6            // 6h: VCP Predication Command
#define  VCP_OPCODE_Vcp_Kickoff             0x7            // 7h: VCP Kickoff Command
#define  VCP_OPCODE_Vcp_Fence               0x8            // 8h: External Fence Command
#define  VCP_OPCODE_Vcp_Indicator           0x9            // 9h: Set process_id command
#define  VCP_OPCODE_Vcp_Autoclear           0xC            // Ch: Auto clear command
#define  VCP_OPCODE_Vcp_Init                0xD            // Dh: VCP Init Command
#define  VCP_OPCODE_Vcp_Aes                 0xE            // Eh: AES Command
#define  VCP_OPCODE_Vcp_Copy                0xF            // Fh VCP copy Command

// Block constant definition
typedef enum
{
    VCP_SET_DATA_BUF_DATA_EXCH_TYPE_MEM      = 0,   // 0x00: Data exchange through memory
    VCP_SET_DATA_BUF_DATA_EXCH_TYPE_FIFO     = 1,   // 0x01: Data exchange through FIFO
} VCP_SET_DATA_BUF_DATA_EXCH_TYPE;
typedef enum
{
    VCP_INTERNAL_WAIT_WAIT_MODE_NORMAL_WAIT  = 0,   // normal wait for internal fence
    VCP_INTERNAL_WAIT_WAIT_MODE_KKK_WAIT     = 1,   // wait for kernel kick off kernel
    VCP_INTERNAL_WAIT_WAIT_MODE_WAIT_CHIP_IDLE= 2,  // normal external wait for engine idle
    VCP_INTERNAL_WAIT_WAIT_MODE_EXTERNAL_WAIT= 3,   // external wait for vulkan global event sync
} VCP_INTERNAL_WAIT_WAIT_MODE;
typedef enum
{
    VCP_INTERNAL_WAIT_METHOD_BIGEQUAL        = 0,   // 0: Big Equal
    VCP_INTERNAL_WAIT_METHOD_EQUAL           = 1,   // 1: Equal
} VCP_INTERNAL_WAIT_METHOD;
typedef enum
{
    VCP_INTERNAL_WAIT_STATION_ID_MAIN_PARSER= 0,    // parser the wait cmd in csp main-parser.
    VCP_INTERNAL_WAIT_STATION_ID_PRE_PARSER  = 1,   // parser the wait cmd in csp pre-parser. Can only be normal
                                                    // wait mode.
} VCP_INTERNAL_WAIT_STATION_ID;
typedef enum
{
    VCP_INTERNAL_WAIT_SLOT_ID_TO_WAIT_VCP0_BE= 8,   // TO_WAIT_VCP0_BE
    VCP_INTERNAL_WAIT_SLOT_ID_TO_WAIT_VCP0_FE= 9,   // TO_WAIT_VCP0_FE
    VCP_INTERNAL_WAIT_SLOT_ID_TO_WAIT_VCP1_BE= 11,  // TO_WAIT_VCP1_BE
    VCP_INTERNAL_WAIT_SLOT_ID_TO_WAIT_VCP1_FE= 12,  // TO_WAIT_VCP1_FE
} VCP_INTERNAL_WAIT_SLOT_ID;
typedef enum
{
    VCP_INTERNAL_WAIT_FE_BE_FLAG_VCP_FE      = 0,
    VCP_INTERNAL_WAIT_FE_BE_FLAG_VCP_BE      = 1,
} VCP_INTERNAL_WAIT_FE_BE_FLAG;
typedef enum
{
    VCP_QUERY_DUMP_IRQ_NOP                   = 0,   // 00: Nop
    VCP_QUERY_DUMP_IRQ_INTERRUPT             = 1,   // 01: interrupt when external fence done
} VCP_QUERY_DUMP_IRQ;
typedef enum
{
    VCP_QUERY_DUMP_DATA_TYPE_SIGNATURE       = 0,   // 00: Signature data
    VCP_QUERY_DUMP_DATA_TYPE_COUNTER         = 1,   // 01: ILA counters
    VCP_QUERY_DUMP_DATA_TYPE_OTHERS          = 2,   // 10: Other data
} VCP_QUERY_DUMP_DATA_TYPE;
typedef enum
{
    VCP_QUERY_DUMP_BLOCK_ID_VCP_FE           = 0,   // 00: VCP_FE
    VCP_QUERY_DUMP_BLOCK_ID_VCP_BE           = 1,   // 01: VCP_BE, 2~15 are reserved.
} VCP_QUERY_DUMP_BLOCK_ID;
typedef enum
{
    VCP_SET_REGISTER_SHORT_SET_WHOLE_BYTE_SET_NIBBLE_MASK= 0,
                                                    // 0x00: set nibble with nibble mask
    VCP_SET_REGISTER_SHORT_SET_WHOLE_BYTE_SET_WHOLE_BYTE= 1,
                                                    // 0x01: set whole byte, LSB of nibble offset =0
} VCP_SET_REGISTER_SHORT_SET_WHOLE_BYTE;
typedef enum
{
    VCP_PREDICATION_INFO_PREDICATION_MODE_ALL_ZERO= 0,
                                                    // 0: enalbe predication feature if all 64-bit data are zero
    VCP_PREDICATION_INFO_PREDICATION_MODE_NOT_ALL_ZERO= 1,
                                                    // 1: enalbe predication feature if not all 64-bit data are
                                                    // zero
} VCP_PREDICATION_INFO_PREDICATION_MODE;
typedef enum
{
    VCP_KICKOFF_KICKOFF_MODE_START           = 0,   // 0: Frame start indicator, should be the first command of
                                                    // frame
    VCP_KICKOFF_KICKOFF_MODE_EXECUTE         = 1,   // 1: Real kickoff, start processing
} VCP_KICKOFF_KICKOFF_MODE;
typedef enum
{
    VCP_FENCE_IRQ_NOP                        = 0,   // 00: Nop
    VCP_FENCE_IRQ_INTERRUPT                  = 1,   // 01: interrupt when external fence done
    VCP_FENCE_IRQ_INTERRUPT_GPU              = 2,   // 02: When it's external fence cmd, notify mxu about the
                                                    // external fence type: 0->normal external fence cmd;
                                                    // 1->external fence cmd for vulkan global event sync(event
                                                    // value filled by HW), now mxu should set CSP's reg
                                                    // "Reg_Gpu_Set".
} VCP_FENCE_IRQ;
typedef enum
{
    VCP_FENCE_FENCE_TYPE_INTERNAL            = 0,   // internal fence
    VCP_FENCE_FENCE_TYPE_EXTERNAL32          = 2,   // 32bit external fence value
    VCP_FENCE_FENCE_TYPE_EXTERNAL64          = 3,   // 64bit external fence value, After 64b external fence, HW
                                                    // can try to do DMA level preempt. If
                                                    // Reg_Preempt_Cmd_Process. Preempt==1, then do preempt
} VCP_FENCE_FENCE_TYPE;
typedef enum
{
    VCP_FENCE_SLOT_ID_INTERNAL_FENCE_VCP0_BE= 8,    // INTERNAL_FENCE_VCP0_BE
    VCP_FENCE_SLOT_ID_INTERNAL_FENCE_VCP0_FE= 9,    // INTERNAL_FENCE_VCP0_FE
    VCP_FENCE_SLOT_ID_INTERNAL_FENCE_VCP1_BE= 11,   // INTERNAL_FENCE_VCP0_BE
    VCP_FENCE_SLOT_ID_INTERNAL_FENCE_VCP1_FE= 12,   // INTERNAL_FENCE_VCP0_FE
} VCP_FENCE_SLOT_ID;
typedef enum
{
    VCP_FENCE_FENCE_UPDATE_MODE_COPY         = 0,   // 0x0: Copy operation
    VCP_FENCE_FENCE_UPDATE_MODE_OR           = 1,   // 0x1: Or operation
} VCP_FENCE_FENCE_UPDATE_MODE;
typedef enum
{
    VCP_FENCE_RB_TYPE_3DFE                   = 0,   // 0: the fence is from 3DL/H RB for 3D Front end shader
    VCP_FENCE_RB_TYPE_3DBE                   = 1,   // 0: the fence is from 3DL/H RB for 3D Back end shader
    VCP_FENCE_RB_TYPE_CSL                    = 2,   // 1: the fence is from CS LOW RB
    VCP_FENCE_RB_TYPE_CSH                    = 3,   // 2: the fence is from CS HIGH RB
} VCP_FENCE_RB_TYPE;
typedef enum
{
    VCP_FENCE_BLOCK_ID_VCP_FE                = 0,   // Set  block_ID as 0 for VCP_FE
    VCP_FENCE_BLOCK_ID_VCP_BE                = 1,   // Set  block_ID as 0 for VCP_BE, 2~15 are reserved.
} VCP_FENCE_BLOCK_ID;
typedef enum
{
    VCP_INDICATOR_TYPE_SET_PROC_ID           = 0,   // 0: the indicator command is used for setting process ID
    VCP_INDICATOR_TYPE_FRAME_BEGIN           = 1,   // 1: the indicator command is used to identify frame begin
    VCP_INDICATOR_TYPE_FRAME_END             = 2,   // 2: the indicator command is used to identify frame end
} VCP_INDICATOR_TYPE;
typedef enum
{
    VCP_INIT_DWC_DWC_0_RESET                 = 0,   // 1. For true HW CMD,  the following cases, no DWF is needed.
                                                    //      A. to swapping timeout-reset-signal.
                                                    //      B. invalid/Flush I$/D$.
                                                    //  2. For fake init in Cmodel, there's case that no DWF is
                                                    // needed.
    VCP_INIT_DWC_DWC_1_SET_CYCLE_THRESHOLD   = 1,   // 1.For true HW CMD, to send time-out-reset cycle threshold
                                                    // by using 1 DWF.
                                                    //  2.For chip debug, to send one special value such as
                                                    // 0X12345678 as some debugging signal in real chip.
                                                    //  3. For fake init in Cmodel, The DWF is used to send a pointer
                                                    // to keep debug-info and Cmodel-mem-range-check-list.
    VCP_INIT_DWC_DWC_4_BLC                   = 4,   // 1. For true HW CMD, to invalid BLC and some info else.
} VCP_INIT_DWC;
typedef enum
{
    VCP_AES_MODE_NONE                        = 0,   // 00: No encryption operation on source data
    VCP_AES_MODE_XOR                         = 1,   // 01: Dest = Source XOR Constant_Key
    VCP_AES_MODE_AES_CTR                     = 2,   // 10: AES-CTR Encryption/Decryption
    VCP_AES_MODE_CIPHER                      = 3,   // 11: Cascaded Cipher Decryption
} VCP_AES_MODE;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////            VCP_DECOUPLE_OPCODES Block (Block ID = 0) Register Definitions            ///////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct _Cmd_Vcp_Skip                // 0h: SKIP Command
{
    unsigned int     Dwc                   : 22; // The number Command will be skipped
    unsigned int     Reserved              : 2;  // Reserved
    unsigned int     Block_Id              : 4;  // Block ID
    unsigned int     Major_Opcode          : 4;  // 0000: SKIP.
} Cmd_Vcp_Skip;

typedef struct _Cmd_Vcp_Set_Data_Buf        // 1h: VCP set data buffer Command
{
    unsigned int     Dwc                   : 4;  // Number of dw followed
    unsigned int     Reset_Buf_Base_En     : 1;  // Reset buffer base/size enable flag
    unsigned int     Reserved1             : 1;  // Reserved
    unsigned int     Bandwidth_Limit_En    : 1;  // Limit bandwidth consumption enable flag
    unsigned int     Data_Exch_Type        : 1;  // Data input/output selection
    unsigned int     Reserved2             : 16; // Reserved
    unsigned int     Block_Id              : 4;  // Block ID
    unsigned int     Major_Opcode          : 4;  // 0001:VCP decouple buffer intialize command
} Cmd_Vcp_Set_Data_Buf;
/////////////////////////////////////////////////////////////////////////
// some other description about the command usage: Start
       typedef struct _Cmd_Vcp_Buf_Base_Addr_L32    // 1h DWF1: DWORD for  VCP_SET_DATA_BUF command
       {
            unsigned int     Reserved              : 5;
            unsigned int     Buf_Base_L            : 27; // VLD buffer base address, 32 byte aligned
       } Cmd_Vcp_Buf_Base_Addr_L32;
       typedef struct _Cmd_Vcp_Buf_Base_Addr_H32    // 1h DWF2: DWORD for  VCP_SET_DATA_BUF command
       {
            unsigned int     Address_H8            : 8;  // high 8 bits of the 40-bit-address
            unsigned int     Bl_Slot_Index         : 18; // Start bl slot index from KMD driver side.
            unsigned int     Reserved              : 6;  // Reserved
       } Cmd_Vcp_Buf_Base_Addr_H32;
       typedef struct _Cmd_Vcp_Buf_Size             // 1h DWF3: DWORD for VCP_SET_DATA_BUF command
       {
            unsigned int     Reserved              : 5;
            unsigned int     Buf_Size              : 27; // VLD buffer size, 32 byte aligned
       } Cmd_Vcp_Buf_Size;
       typedef struct _Cmd_Vcp_Bandwidth_Size       // 1h DWF4: DWORD for VCP_SET_DATA_BUF command
       {
            unsigned int     Reserved              : 5;
            unsigned int     Bandwdith_Size        : 27; // Total bandwidth consumption allowed
       } Cmd_Vcp_Bandwidth_Size;
// some other description about the command usage: End
/////////////////////////////////////////////////////////////////////////


typedef struct _Cmd_Vcp_Internal_Wait       // 2h: Internal Wait Command
{
    unsigned int     Ref_Value             : 16; // ref_value
    unsigned int     Wait_Mode             : 2;  // 0, it's wait reference valud for "normal_wait" command;
                                                 // 1, for kkk wait, ref_value > 0, and ref_value*2 is total
                                                 // event count; 2, for vulkan global event sync; 3, for event
                                                 // addr and event ref value.
    unsigned int     Method                : 1;  // Wait Method, only valid for normal internal wait cmd
    unsigned int     Station_Id            : 3;  // Only valid for normal internal wait cmd
    unsigned int     Slot_Id               : 5;  // Slot ID. Totally 32 fence counters, and driver can
                                                 // flexible choose some of them for different CS RB.
    unsigned int     Fe_Be_Flag            : 1;  // FE/BE flag
    unsigned int     Major_Opcode          : 4;  // 0010:Wait (external/internal wait)
} Cmd_Vcp_Internal_Wait;
/////////////////////////////////////////////////////////////////////////
// some other description about the command usage: Start
       typedef struct _Cmd_Vcp_Internal_Wait_Dword1 // 2h DWF1; DWORD for kkk wait event
       {
            unsigned int     Event_Flag_Idx1       : 16; // totally 1024 events, of each, flag is 2 bits. For 1024
                                                         // events, only 11 bits are enough, here we use 16 bits for
                                                         // event idx for convenience. If the idx==0xffff, this idx is
                                                         // invalid.
            unsigned int     Event_Flag_Idx2       : 16;
       } Cmd_Vcp_Internal_Wait_Dword1;
// some other description about the command usage: End
/////////////////////////////////////////////////////////////////////////


typedef struct _Cmd_Vcp_Query_Dump          // 3h: Query Dump Command
{
    unsigned int     Dump_Dw_Cnt           : 13; // Number of DW to be dumped
    unsigned int     Irq                   : 1;  // Interrupt flag
    unsigned int     Data_Type             : 2;  // Dump data type
    unsigned int     Reg_Offset            : 8;  // Start register offset to be dumped
    unsigned int     Block_Id              : 4;  // Block ID
    unsigned int     Major_Opcode          : 4;  // 0011: Query Dump (including external fence, dump
                                                 // status/context to host)
} Cmd_Vcp_Query_Dump;
/////////////////////////////////////////////////////////////////////////
// some other description about the command usage: Start
       typedef struct _Cmd_Vcp_Query_Dump_Address_Dword_L32
                                                    // 3h DWF1; DWORD for Query Dump Address
       {
            unsigned int     Reserved              : 2;  // Reserved
            unsigned int     Address_L             : 30; // DW aligned memory address to dump
       } Cmd_Vcp_Query_Dump_Address_Dword_L32;
       typedef struct _Cmd_Vcp_Query_Dump_Address_Dword_H32
                                                    // 3h DWF2; DWORD for Query Dump Address
       {
            unsigned int     Address_H8            : 8;  // high 8 bits of the 40-bit-address
            unsigned int     Bl_Slot_Index         : 18; // Start bl slot index from KMD driver side.
            unsigned int     Reserved              : 6;  // Reserved
       } Cmd_Vcp_Query_Dump_Address_Dword_H32;
// some other description about the command usage: End
/////////////////////////////////////////////////////////////////////////


typedef struct _Cmd_Vcp_Set_Register        // 4h: Set Register Command
{
    unsigned int     Dwc                   : 7;  // the number of following dws, at least 1 DW and up to 64 DW will
                                                 // follow, but not exceeding block boundry
    unsigned int     Reserved              : 9;  // Reserved
    unsigned int     Start_Offset          : 8;  // Register start offset in DW
    unsigned int     Block_Id              : 4;  // Block ID
    unsigned int     Major_Opcode          : 4;  // 0100: Set Register  (with multiple DW followed)
} Cmd_Vcp_Set_Register;
/////////////////////////////////////////////////////////////////////////
// some other description about the command usage: Start
       typedef struct _Cmd_Vcp_Set_Register_Value_Dword
                                                    // 4h DWF1: DWORD for SET_REGISTER
       {
            unsigned int     Reg_Val               : 32; // Register value
       } Cmd_Vcp_Set_Register_Value_Dword;
// some other description about the command usage: End
/////////////////////////////////////////////////////////////////////////


typedef struct _Cmd_Vcp_Set_Register_Short  // 5h: SetRegisterShort Command
{
    unsigned int     Register_Value        : 8;  // The regsiter value this command want to set, only 4 bits are
                                                 // valid (either high or low nibble) when set_whole_byte=0,
                                                 //  where nibble offset bit 0 determines which nibble is
                                                 // valid.
    unsigned int     Nibble_Mask           : 4;  // Nibble mask
    unsigned int     Set_Whole_Byte        : 1;  // Set whole byte
    unsigned int     Nibble_Offset         : 3;  // Offset of nibbles from LSB, a nibble = 4 bits
    unsigned int     Reserved              : 8;  // Reserved
    unsigned int     Block_Id              : 4;  // Block ID
    unsigned int     Major_Opcode          : 4;  // 0101: Set Register Short
} Cmd_Vcp_Set_Register_Short;
/////////////////////////////////////////////////////////////////////////
// some other description about the command usage: Start
       typedef struct _Cmd_Vcp_Set_Register_Short_Offset_Dword
                                                    // 5h DWF: DWORD for SET_REGISTER_SHORT
       {
            unsigned int     Register_Offset       : 13; // Register offset
            unsigned int     Reserved              : 19; // Reserved
       } Cmd_Vcp_Set_Register_Short_Offset_Dword;
// some other description about the command usage: End
/////////////////////////////////////////////////////////////////////////

typedef struct _Cmd_Vcp_Predication         // 6h: VCP Predication Command
{
    unsigned int     Dwc                   : 2;  // Number of dw followed
    unsigned int     Sequence_Idx          : 7;  // How many bytes in the result_buffer
    unsigned int     Codec_Type            : 8;  // predication mode
    unsigned int     Ignore                : 1;  // 0: can't ignore predication 1: ignore predication
    unsigned int     Ap_Pred               : 1;  // 0: predication set by driver; 1: predication set by AP
    unsigned int     Reserved              : 5;  // Reserved
    unsigned int     Block_Id              : 4;  // Block ID
    unsigned int     Major_Opcode          : 4;  // 0110: VCP set predication command
} Cmd_Vcp_Predication;
/////////////////////////////////////////////////////////////////////////
// some other description about the command usage: Start
       typedef struct _Cmd_Vcp_Predication_Info     // 6h DWF0: DWORD for predication command info
       {
            unsigned int     Byte_Num              : 17; // How many bytes in the result_buffer
            unsigned int     Predication_Mode      : 1;  // predication mode
            unsigned int     Reserved              : 14; // Reserved
       } Cmd_Vcp_Predication_Info;
       typedef struct _Cmd_Vcp_Predication_Address_Dword1_L32
                                                    // 6h DWF1: low part address of predication result_buffer
       {
            unsigned int     Address_L             : 32; // Data base address, dw alignment
       } Cmd_Vcp_Predication_Address_Dword1_L32;
       typedef struct _Cmd_Vcp_Predication_Address_Dword2_H32
                                                    // 6h DWF2: high part address of predication result_buffer
       {
            unsigned int     Address_H8            : 8;  // high 8 bits of the 40-bit-address
            unsigned int     Bl_Slot_Index         : 18; // Start bl slot index from KMD driver side.
            unsigned int     Reserved              : 6;  // Reserved
       } Cmd_Vcp_Predication_Address_Dword2_H32;
// some other description about the command usage: End
/////////////////////////////////////////////////////////////////////////

typedef struct _Cmd_Vcp_Kickoff             // 7h: VCP Kickoff Command
{
    unsigned int     Dwc                   : 4;  // Number of dw followed
    unsigned int     Kickoff_Mode          : 1;  // Kickoff mode
    unsigned int     Set_Dma_Flag          : 1;  // Set frame DMA info flag
    unsigned int     Sub_Frame_End         : 1;  // Set it as 1 for the last slice kickoff of one frame.
    unsigned int     Secure_Video          : 1;  // 0 = no secure video; 1= secure video
    unsigned int     Slice_Group_Idx       : 2;  // Slice group index, each frame can have 4 slice group in max
    unsigned int     Frame_Idx             : 6;  // frame index, keep lower 6 bit
    unsigned int     Sequence_Idx          : 7;  // Seq index
    unsigned int     Stop_Reset_Enable     : 1;  // 0: enable the Reset; 1: stop reset.
    unsigned int     Block_Id              : 4;  // Block ID
    unsigned int     Major_Opcode          : 4;  // 0111: VCP frame level kickoff command
} Cmd_Vcp_Kickoff;
/////////////////////////////////////////////////////////////////////////
// some other description about the command usage: Start
       typedef struct _Cmd_Vcp_Data_Address_L32     // 7h DWF1: DWORD for  Video Command
       {
            unsigned int     Address_L             : 32; // Data base address, dw alignment
       } Cmd_Vcp_Data_Address_L32;
       typedef struct _Cmd_Vcp_Data_Address_H32     // 7h DWF2: DWORD for  Video Command
       {
            unsigned int     Address_H8            : 8;  // high 8 bits of the 40-bit-address
            unsigned int     Bl_Slot_Index         : 18; // Start bl slot index from KMD driver side.
            unsigned int     Reserved              : 6;  // Reserved
       } Cmd_Vcp_Data_Address_H32;
       typedef struct _Cmd_Vcp_Output_Address_L32   // 7h DWF3: DWORD for  Video Command
       {
            unsigned int     Address_L             : 32; // Debug info output base address, dw alignment, not be used
                                                         // in the time being and reserved here.
       } Cmd_Vcp_Output_Address_L32;
       typedef struct _Cmd_Vcp_Output_Address_H32   // 7h DWF4: DWORD for  Video Command
       {
            unsigned int     Address_H8            : 8;  // high 8 bits of the 40-bit-address
            unsigned int     Bl_Slot_Index         : 18; // Start bl slot index from KMD driver side.
            unsigned int     Reserved              : 6;  // Reserved
       } Cmd_Vcp_Output_Address_H32;
       typedef struct _Cmd_Vcp_Pc_L32               // 7h DWF5: DWORD for  Video Command
       {
            unsigned int     Pc_Value              : 24; // Start of the program address, The address is 256 byte
                                                         // aligned
            unsigned int     Reserved              : 8;
       } Cmd_Vcp_Pc_L32;
       typedef struct _Cmd_Vcp_Pc_H32               // 7h DWF6: DWORD for  Video Command
       {
            unsigned int     Address_H8            : 8;  // high 8 bits of the 40-bit-address
            unsigned int     Bl_Slot_Index         : 18; // Start bl slot index from KMD driver side.
            unsigned int     Reserved              : 6;  // Reserved
       } Cmd_Vcp_Pc_H32;
       typedef struct _Cmd_Vcp_Top_Mvd_Surf_L32     // 7h DWF7: DWORD for  Video Command
       {
            unsigned int     Address_L             : 32; // Start address for top MVD surface (H264 encoder only for
                                                         // CABAC top MVD context.)
       } Cmd_Vcp_Top_Mvd_Surf_L32;
       typedef struct _Cmd_Vcp_Top_Mvd_Surf_H32     // 7h DWF8: DWORD for  Video Command
       {
            unsigned int     Address_H8            : 8;  // high 8 bits of the 40-bit-address
            unsigned int     Bl_Slot_Index         : 18; // Start bl slot index from KMD driver side.
            unsigned int     Reserved              : 6;  // Reserved
       } Cmd_Vcp_Top_Mvd_Surf_H32;
// some other description about the command usage: End
/////////////////////////////////////////////////////////////////////////


typedef struct _Cmd_Vcp_Fence               // 8h: External Fence Command
{
    unsigned int     Dwc                   : 3;  // Set as 1 for internal fence. Set as 3 for 32b external fence.
                                                 // Set as 4 for 64b external fence.
    unsigned int     Irq                   : 2;  // Interrupt
    unsigned int     Fence_Type            : 2;  // internal/external fence type
    unsigned int     Slot_Id               : 5;  // internal fence slot ID
    unsigned int     Reserved              : 1;  // Reserved
    unsigned int     Fence_Update_Mode     : 1;  // Fence update mode
    unsigned int     Reserved1             : 8;  // reserved1
    unsigned int     Rb_Type               : 2;  // 3d,csl,csh, all module will drain task according to
                                                 // process_id & rb type.
    unsigned int     Block_Id              : 4;  // The usage is the same as the Route_Id. In VCP, it is parsed by
                                                 // VCP only, so the value defined is ignored in CSP/MXU.
    unsigned int     Major_Opcode          : 4;  // 1000: Internal/External Fence (wait for internal block
                                                 // completion before next command can be executed)
} Cmd_Vcp_Fence;
/////////////////////////////////////////////////////////////////////////
// some other description about the command usage: Start
       typedef struct _Cmd_Vcp_Fence_Internal_Dword1// 8h DWF1: DWORD for internal fence, fence update value
       {
            unsigned int     Update_Value          : 16;
            unsigned int     Slice_Mask            : 16; // This bit will not be touched by driver. CSP will set it
                                                         // according to current ring buffer status indicator. And
                                                         // MXU will according to this bit to decide which slice's
                                                         // fence need to be wait.
       } Cmd_Vcp_Fence_Internal_Dword1;
       typedef struct _Cmd_Vcp_External_Fence_Command_Dword0
                                                    // 8h DWF1:DWORD for external fence write address, dw aligned for 32b
                                                    // external fence, and ddw aligned for 64b external fence cmd.
       {
            unsigned int     External_Addr_Low32   : 32; // The 40-bit address is supposed to be 32bits aligned.
       } Cmd_Vcp_External_Fence_Command_Dword0;
       typedef struct _Cmd_Vcp_External_Fence_Command_Dword1
                                                    // 8h DWF2: DWORD for external fence write address, dw aligned for 32b
                                                    // external fence, and ddw aligned for 64b external fence cmd.
       {
            unsigned int     External_Addr_High8   : 8;
            unsigned int     Reserved              : 23;
            unsigned int     Fence_Update_Timing   : 1;  // When this bit is 1, MXU will send all write requests out then
                                                         // update the fence value.
       } Cmd_Vcp_External_Fence_Command_Dword1;
       typedef struct _Cmd_Vcp_External_Fence_Command_Dword2
                                                    // 8h DWF3: DWORD for external fence write data. For 32b external fence,
                                                    // this value is enough; for 64b external fence, it’s the low 32b data.
       {
            unsigned int     External_Data1        : 32; // Output DWORD0
       } Cmd_Vcp_External_Fence_Command_Dword2;
       typedef struct _Cmd_Vcp_External_Fence_Command_Dword3
                                                    // 8h DWF4: DWORD for external fence write data, high 32b data for 64b
                                                    // external fence
       {
            unsigned int     External_Data2        : 32; // Output DWORD1
       } Cmd_Vcp_External_Fence_Command_Dword3;
// some other description about the command usage: End
/////////////////////////////////////////////////////////////////////////


typedef struct _Cmd_Vcp_Indicator           // 9h: Set process_id command
{
    unsigned int     Dwc                   : 3;  // Number of DWORD followed
    unsigned int     Type                  : 2;
    unsigned int     Codec_Type            : 8;  // codec type
    unsigned int     Process_Id            : 4;
    unsigned int     Sequence_Id           : 7;
    unsigned int     Block_Id              : 4;  // Block ID, 0 for FE, 1 for BE, 2~15 are reserved
    unsigned int     Major_Opcode          : 4;  // 1001: cmd to set process_id
} Cmd_Vcp_Indicator;

typedef struct _Cmd_Vcp_Autoclear           // Ch: Auto clear command
{
    unsigned int     Dwc                   : 3;  // Number of DWORD followed
    unsigned int     Counter               : 17; // for clear MXU's BLC cmd, counter tells? how many bytes need
                                                 // to be cleared
    unsigned int     Reserved              : 4;
    unsigned int     Block_Id              : 4;  // Block ID, 0 for FE, 1 for BE, 2~15 are reserved
    unsigned int     Major_Opcode          : 4;  // 1100: Auto clear Command OPCODE Ch Major Opcode Extension
} Cmd_Vcp_Autoclear;
/////////////////////////////////////////////////////////////////////////
// some other description about the command usage: Start
       typedef struct _Cmd_Vcp_Autoclear_Dword0     // Ch DWF1: DWORD for  Auto clear Command
       {
            unsigned int     Reserved              : 5;
            unsigned int     Start_Address_L       : 27; // The address is 256 bits aligned. Or just 18-bits for save/
                                                         // restore addr which is 16K bytes aligned
       } Cmd_Vcp_Autoclear_Dword0;
       typedef struct _Cmd_Vcp_Autoclear_Dword1     // Ch DWF1: DWORD for  Auto clear Command
       {
            unsigned int     Address_H8            : 8;  // high 8 bits of the 40-bit-address
            unsigned int     Bl_Slot_Index         : 18; // Start bl slot index from KMD driver side. Reserved for
                                                         // auto-clear in the time being.
            unsigned int     Reserved              : 6;  // Reserved
       } Cmd_Vcp_Autoclear_Dword1;
       typedef struct _Cmd_Vcp_Autoclear_Dword2     // Ch DWF2: DWORD for  Auto clear Command
       {
            unsigned int     Reserved              : 24;
            unsigned int     Clear_Value1          : 4;  // 4-bit-clear-value for Y
            unsigned int     Clear_Value0          : 4;  // 4-bit-clear-value for Cb/Cr
       } Cmd_Vcp_Autoclear_Dword2;
       typedef struct _Cmd_Vcp_Autoclear_Dword3     // Ch DWF3: DWORD for  Auto clear Command
       {
            unsigned int     Clear_Mask0           : 32; // for each bit, 1 means use "CLEAR_VALUE1 | (CLEAR_VALUE1 <<
                                                         // 4)"; 0 means use "CLEAR_VALUE0 | (CLEAR_VALUE0 << 4)",
                                                         // 1-bit mask map to 8-bit clearvalue. We have 4 DW masks here,
                                                         // so there're total 256*4 bits BL-data can be controlled by
                                                         // the 4 DW masks. If BL-data size is more than 256*4 bit,
                                                         // duplicate the 4 DW masks to control the clear values.
       } Cmd_Vcp_Autoclear_Dword3;
       typedef struct _Cmd_Vcp_Autoclear_Dword4     // Ch DWF4: DWORD for  Auto clear Command
       {
            unsigned int     Clear_Mask1           : 32; // for each bit, 1 means use "CLEAR_VALUE1 | (CLEAR_VALUE1 <<
                                                         // 4)"; 0 means use "CLEAR_VALUE0 | (CLEAR_VALUE0 << 4)",
                                                         // 1-bit mask map to 8-bit clearvalue. We have 4 DW masks here,
                                                         // so there're total 256*4 bits BL-data can be controlled by
                                                         // the 4 DW masks. If BL-data size is more than 256*4 bit,
                                                         // duplicate the 4 DW masks to control the clear values.
       } Cmd_Vcp_Autoclear_Dword4;
       typedef struct _Cmd_Vcp_Autoclear_Dword5     // Ch DWF5: DWORD for  Auto clear Command
       {
            unsigned int     Clear_Mask2           : 32; // for each bit, 1 means use "CLEAR_VALUE1 | (CLEAR_VALUE1 <<
                                                         // 4)"; 0 means use "CLEAR_VALUE0 | (CLEAR_VALUE0 << 4)",
                                                         // 1-bit mask map to 8-bit clearvalue. We have 4 DW masks here,
                                                         // so there're total 256*4 bits BL-data can be controlled by
                                                         // the 4 DW masks. If BL-data size is more than 256*4 bit,
                                                         // duplicate the 4 DW masks to control the clear values.
       } Cmd_Vcp_Autoclear_Dword5;
       typedef struct _Cmd_Vcp_Autoclear_Dword6     // Ch DWF6: DWORD for  Auto clear Command
       {
            unsigned int     Clear_Mask3           : 32; // for each bit, 1 means use "CLEAR_VALUE1 | (CLEAR_VALUE1 <<
                                                         // 4)"; 0 means use "CLEAR_VALUE0 | (CLEAR_VALUE0 << 4)",
                                                         // 1-bit mask map to 8-bit clearvalue. We have 4 DW masks here,
                                                         // so there're total 256*4 bits BL-data can be controlled by
                                                         // the 4 DW masks. If BL-data size is more than 256*4 bit,
                                                         // duplicate the 4 DW masks to control the clear values.
       } Cmd_Vcp_Autoclear_Dword6;
// some other description about the command usage: End
/////////////////////////////////////////////////////////////////////////


typedef struct _Cmd_Vcp_Init                // Dh: VCP Init Command
{
    unsigned int     Dbg_Shader_Load_En    : 1;  // Debug shader load flag (For C-Model only)
    unsigned int     Cmodel_Mem_Check_En   : 1;  // Multiple surface memory check enable flag (For C-Model
                                                 // only)
    unsigned int     Hw_Cross_Mem_Check_En : 1;  // HW cross memory check enable flag (For C-Model only)
    unsigned int     Codec_Type            : 8;  // codec type
    unsigned int     Is_Normal_Vector      : 1;  // is normal vector or fuzzy vector(For C-model only)
    unsigned int     Reserved              : 1;  // reserved
    unsigned int     Flush_Blc             : 1;  // 1: flush burst length cache
    unsigned int     Invalidate_Blc        : 1;  // 1: invalidate burst length cache
    unsigned int     Timeout_Reset         : 1;  // Vcp Time Out Reset enable bit
    unsigned int     Vcp_Reset             : 1;  // Vcp Reset enable bit
    unsigned int     Flush                 : 1;  // FLUSH VCP data cache
    unsigned int     Invalidate_Ic         : 1;  // Invalidate VCP Instruction cache
    unsigned int     Invalidate_Dc         : 1;  // Invalidate VCP Data cache
    unsigned int     Dwc                   : 4;  // Number of DWORDs to follow this command.
    unsigned int     Block_Id              : 4;  // Block ID
    unsigned int     Major_Opcode          : 4;  // 1101: VCP init command
} Cmd_Vcp_Init;
/////////////////////////////////////////////////////////////////////////
// some other description about the command usage: Start
       typedef struct _Cmd_Vcp_Blc_Surface_Addr_L32 // Dh DWF1: DWORD for VCP_INIT Command
       {
            unsigned int     Surf_Addr_L           : 32; // Invalidate BLC address in byte
       } Cmd_Vcp_Blc_Surface_Addr_L32;
       typedef struct _Cmd_Vcp_Blc_Surface_Addr_H32 // Dh DWF2: DWORD for VCP_INIT Command
       {
            unsigned int     Address_H8            : 8;  // high 8 bits of the 40-bit-address
            unsigned int     Bl_Slot_Index         : 18; // Start bl slot index from KMD driver side.
            unsigned int     Reserved              : 6;  // Reserved
       } Cmd_Vcp_Blc_Surface_Addr_H32;
       typedef struct _Cmd_Vcp_Blc_Surface_Size     // Dh DWF3: DWORD for VCP_INIT Command
       {
            unsigned int     Byte_Size             : 32; // Invalidate BLC size in byte
       } Cmd_Vcp_Blc_Surface_Size;
       typedef struct _Cmd_Vcp_Timeout_Reset_Value  // Dh DWF4: DWORD for VCP_INIT command
       {
            unsigned int     Cycle_Count           : 32; // Cycle count for timeout reset
       } Cmd_Vcp_Timeout_Reset_Value;
// some other description about the command usage: End
/////////////////////////////////////////////////////////////////////////


typedef struct _Cmd_Vcp_Aes                 // Eh: AES Command
{
    unsigned int     Mode                  : 2;  // AES operation mode
    unsigned int     Remove003_En          : 1;  // Enable/disable remove 003 feature, 1 = do remove 003
    unsigned int     Remove002_En          : 1;  // 0: disable remove 02; 1: enable remove 02
                                                 // (non-byte-align)
    unsigned int     To_Memory             : 1;  // 0: AES output to VCP; 1: AES output to memory
    unsigned int     Secure_Video          : 1;  // 0 = no secure video; 1= secure video
    unsigned int     Reserved              : 15; // Transfer size in 128 bits unit for AES Transfer, 4M byte at
                                                 // most
    unsigned int     Dwc                   : 3;  // DWF[0] Encrypted-bits-in-addr; DWF[1] bitstream byte
                                                 // size; DWF[2] Decrypted-bits-out-addr
    unsigned int     Block_Id              : 4;  // Block ID
    unsigned int     Major_Opcode          : 4;  // 1110: AES
} Cmd_Vcp_Aes;
/////////////////////////////////////////////////////////////////////////
// some other description about the command usage: Start
       typedef struct _Cmd_Vcp_Aes_Source_Addr_L32  // Eh DWF1: DWORD for  AES command
       {
            unsigned int     Start_Byte            : 5;  // Start byte of AES Address
            unsigned int     Aes_Source_Addr_L     : 27; // AES source Address, must be 256 bits aligned.
       } Cmd_Vcp_Aes_Source_Addr_L32;
       typedef struct _Cmd_Vcp_Aes_Source_Addr_H32  // Eh DWF2: DWORD for  AES command
       {
            unsigned int     Address_H8            : 8;  // high 8 bits of the 40-bit-address
            unsigned int     Bl_Slot_Index         : 18; // Start bl slot index from KMD driver side.
            unsigned int     Reserved              : 6;  // Reserved
       } Cmd_Vcp_Aes_Source_Addr_H32;
       typedef struct _Cmd_Vcp_Aes_Size             // Eh DWF3: DWORD for  AES command
       {
            unsigned int     Byte_Size             : 32; // bit stream size in byte
       } Cmd_Vcp_Aes_Size;
       typedef struct _Cmd_Vcp_Aes_Dst_Addr_L32     // Eh DWF4: DWORD for  AES command
       {
            unsigned int     Start_Byte            : 5;  // Start byte of AES Address
            unsigned int     Aes_Dst_Addr_L        : 27; // AES dst Address, must be 256 bits aligned.
       } Cmd_Vcp_Aes_Dst_Addr_L32;
       typedef struct _Cmd_Vcp_Aes_Dst_Addr_H32     // Eh DWF5: DWORD for  AES command
       {
            unsigned int     Address_H8            : 8;  // high 8 bits of the 40-bit-address
            unsigned int     Bl_Slot_Index         : 18; // Start bl slot index from KMD driver side.
            unsigned int     Reserved              : 6;  // Reserved
       } Cmd_Vcp_Aes_Dst_Addr_H32;
 // some other description about the command usage: End
/////////////////////////////////////////////////////////////////////////

typedef struct _Cmd_Vcp_Copy                // Fh VCP copy Command
{
    unsigned int     Dwc                   : 4;  // DWF[0] copy memory size in bytes
                                                 //  DWF[1] src low 32 address, DWF[2] src high 8 address
                                                 //  DWF[3] dst high 32 address, DWF[4] dst high 8 address
    unsigned int     Reserved              : 20; // Reserved
    unsigned int     Block_Id              : 4;  // Block ID
    unsigned int     Major_Opcode          : 4;  // 1111: Memory COPY
} Cmd_Vcp_Copy;
/////////////////////////////////////////////////////////////////////////
// some other description about the command usage: Start
       typedef struct _Cmd_Vcp_Copy_Src_Addr_L32    // Fh DWF1:  DWORD for  VCP copy Command
       {
            unsigned int     Copy_Src_Addr_L       : 32; // The address is byte align.
       } Cmd_Vcp_Copy_Src_Addr_L32;
       typedef struct _Cmd_Vcp_Copy_Src_Addr_H32    // Fh DWF2:  DWORD for  VCP copy Command
       {
            unsigned int     Copy_Src_Addr_H       : 8;  // high 8 bits of the 40-bit-address
            unsigned int     Bl_Slot_Index         : 18; // Start bl slot index from KMD driver side.
            unsigned int     Reserved              : 6;  // Reserved
       } Cmd_Vcp_Copy_Src_Addr_H32;
       typedef struct _Cmd_Vcp_Copy_Size            // Fh DWF0: DWORD for  VCP copy Command
       {
            unsigned int     Byte_Size             : 32; // memory size in byte
       } Cmd_Vcp_Copy_Size;
       typedef struct _Cmd_Vcp_Copy_Dst_Addr_L32    // Fh DWF3:  DWORD for  VCP copy Command
       {
            unsigned int     Copy_Dst_Addr_L       : 32; // The address is byte align.
       } Cmd_Vcp_Copy_Dst_Addr_L32;
       typedef struct _Cmd_Vcp_Copy_Dst_Addr_H32    // Fh DWF4:  DWORD for  VCP copy Command
       {
            unsigned int     Copy_Dst_Addr_H       : 8;  // high 8 bits of the 40-bit-address
            unsigned int     Bl_Slot_Index         : 18; // Start bl slot index from KMD driver side.
            unsigned int     Reserved              : 6;  // Reserved
       } Cmd_Vcp_Copy_Dst_Addr_H32;

typedef union Vcp_Decouple_Opcodes_cmds
{
    unsigned int                    uint ;
    Cmd_Vcp_Skip                    cmd_Vcp_Skip;
    Cmd_Vcp_Set_Data_Buf            cmd_Vcp_Set_Data_Buf;
    Cmd_Vcp_Internal_Wait           cmd_Vcp_Internal_Wait;
    Cmd_Vcp_Query_Dump              cmd_Vcp_Query_Dump;
    Cmd_Vcp_Set_Register            cmd_Vcp_Set_Register;
    Cmd_Vcp_Set_Register_Short      cmd_Vcp_Set_Register_Short;
    Cmd_Vcp_Predication             cmd_Vcp_Predication;
    Cmd_Vcp_Kickoff                 cmd_Vcp_Kickoff;
    Cmd_Vcp_Fence                   cmd_Vcp_Fence;
    Cmd_Vcp_Indicator               cmd_Vcp_Indicator;
    Cmd_Vcp_Autoclear               cmd_Vcp_Autoclear;
    Cmd_Vcp_Init                    cmd_Vcp_Init;
    Cmd_Vcp_Aes                     cmd_Vcp_Aes;
    Cmd_Vcp_Copy                    cmd_Vcp_Copy;
}Vcp_Decouple_Opcodes_cmd;

#endif
