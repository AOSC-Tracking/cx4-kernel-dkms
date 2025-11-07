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
//
//  REGISTERCOMMANDS.H
//
//  Hardware-specific header file for Excalibur-Lite chip.
//
#ifndef _REGISTERCOMMANDS_H_
#define _REGISTERCOMMANDS_H_

/***********************
    Commands
************************/
#include "registerDef.h"
#include "surface_format.h"

#define REG_CHANNEL_MAX_MASK 0xFFFFFFFF

#define OFFSETOF(s,m)   (unsigned long)&(((s *)0)->m)

#define SLOT_OFFSET_OF_GROUP(Offset, RegGrpClassName, Slot) (Offset + (sizeof(RegGrpClassName) / sizeof(DWORD)) * Slot)
#define REG_OFFSET_OF_GROUP(Offset, RegGrpClassName, Reg, Slot) (Offset + (sizeof(RegGrpClassName) / sizeof(DWORD)) * Slot + OFFSETOF(RegGrpClassName, Reg) / sizeof(DWORD))

typedef struct HW_SET_REGISTER_MASK_CMD_E3K
{
    DWORD     dwOpCode;
    DWORD     dwValue;
    DWORD     dwMask;
}HW_SET_REGISTER_MASK_CMD_E3K;

// level 1's PTE definition, should be inner the TTBR registers
typedef struct
{
    union
    {
        unsigned int uint;
        struct __pte
        {
            unsigned int Valid                     : 1;
            unsigned int NStable                   : 1;
            unsigned int Snoop                     : 1;
            unsigned int Segment                   : 1;
            unsigned int Addr                      : 28;
        } pte;
    };
}PTE_L1_t;


typedef struct
{
    union
    {
        unsigned int uint;
        struct __pte_4k_d // 4k's directory
        {
            unsigned int        Valid   : 1;
            unsigned int        NStable : 1;
            unsigned int        Snoop   : 1;
            unsigned int        Segment : 1;
            unsigned int        Addr    : 27;
            unsigned int        LargePage   : 1; // should be 0
        } _4k_d;
        struct __pte_64k_d // 64k's directory
        {
            unsigned int        Valid   : 1;
            unsigned int        NS      : 1;
            unsigned int        Snoop   : 1;
            unsigned int        Segment : 1;
            unsigned int        Addr    : 27;
            unsigned int        LargePage   : 1; // should be 1
        } _64k_d;
    };
}PTE_L2_t;


typedef struct
{
    union
    {
        unsigned int uint;
        struct __pte_4k
        {
            unsigned int        Valid   : 1;
            unsigned int        NS      : 1;
            unsigned int        Snoop   : 1;
            unsigned int        Segment : 1;
            unsigned int        Addr    : 28;
        } pte_4k;
        struct __pte_64k
        {
            unsigned int        Valid   : 1;
            unsigned int        NS      : 1;
            unsigned int        Snoop   : 1;
            unsigned int        Segment : 1;
            unsigned int        Resrved0 : 4;
            unsigned int        Addr    : 24;
        } pte_64k;
    };
}PTE_L3_t;

typedef union
{
    struct
    {
        unsigned int SWRST_0                 :1;
        unsigned int MIU_SwRst               :1;
        unsigned int SwRst_3D                :1;
        unsigned int SwRst_3D1               :1;
        unsigned int SwRst_3D2               :1;
        unsigned int SwRst_3D3               :1;
        unsigned int VPP_SwRst               :1;
        unsigned int S3VD0_SwRst             :1;
        unsigned int S3VD1_SwRst             :1;
        unsigned int mmio_timer_SwRst        :1;
        unsigned int m0pll_div_SwRst         :1;
        unsigned int m1pll_div_SwRst         :1;
        unsigned int m2pll_div_SwRst         :1;
        unsigned int epll_div_SwRst          :1;
        unsigned int vpll_div_SwRst          :1;
        unsigned int Reserved0               :1;
        unsigned int IGA1_SwRst              :1;
        unsigned int IGA2_SwRst              :1;
        unsigned int IGA3_SwRst              :1;
        unsigned int IGA4_SwRst              :1;
        unsigned int DIUREG_SwRst            :1;
        unsigned int MDI_SwRst               :1;
        unsigned int SLV_SwRst               :1;
        unsigned int TLM_SwRst               :1;
        unsigned int Reserved1               :8;
    };
    unsigned int uint;
} Reg_HWReset_E3K;

typedef union
{
    struct
    {
                unsigned int GFX_TOP_RESETB:1;              // bit 0
                unsigned int VPP_RESETB_tmp:1;              // bit 1
                unsigned int S3VD1_RESETB_tmp:1;         // bit 2
                unsigned int S3VD0_RESETB_tmp:1;         // bit 3
                unsigned int PEB1_RESETB_tmp:1;         // bit 4
                unsigned int PEA1_RESETB_tmp:1;         // bit 5
                unsigned int TU1_RESETB_tmp:1;              // bit 6
                unsigned int FFU1_RESETB_tmp:1;         // bit 7
                unsigned int EUC1_RESETB_tmp:1;         // bit 8
                unsigned int PEB0_RESETB_tmp:1;         // bit 9
                unsigned int PEA0_RESETB_tmp:1;         // bit 10
                unsigned int TU0_RESETB_tmp:1;              // bit 11
                unsigned int FFU0_RESETB_tmp:1;         // bit 12
                unsigned int EUC0_RESETB_tmp:1;         // bit 13
                unsigned int SPTFE_RESETB_tmp:1;         // bit 14
                unsigned int SGTBE_RESETB_tmp:1;         // bit 15
                unsigned int POOL_RESETB_tmp:1;         // bit 16
                unsigned int L2_RESETB_tmp:1;               // bit 17
                unsigned int CSP_MXU0_RESETB:1;         // bit 18
                unsigned int MXU1_RESETB:1;                 // bit 19
                unsigned int DIU_RESETB:1;                  // bit 20
                unsigned int BIU_RESETB:1;                  // bit 21
                unsigned int PMP_RESETB:1;                  // bit 22
                unsigned int HOT_WIRE_RESETB_tmp:1;         // bit 23
                unsigned int IGA3_RSTB_tmp:1;               // bit 24
                unsigned int IGA2_RSTB_tmp:1;               // bit 25
                unsigned int IGA1_RSTB_tmp:1;               // bit 26
                unsigned int REG_RSTB_tmp:1;                // bit 27
                unsigned int MXU0_ADB_tmp:1;                // bit 28
                unsigned int MXU1_ADB_RESETB:1;         // bit 29
        unsigned int Reserved:2;
    };
    unsigned int uint;
} Reg_HWReset_004;

typedef union
{
    struct
    {
        unsigned int Wait              : 1; // wait after submit.
        unsigned int PrintFence        : 1; // print out fence id.
        unsigned int ResetHW           : 1; // reset hardware.
        unsigned int PreHangDump       : 1; // before hang,Save DMA,RingBuffer,context and all resources into framebuffer mirror
        unsigned int PostHangDump      : 1; // After hang,Save DMA,RingBuffer,context and all resources into framebuffer mirror
        unsigned int PerFBHangDump     : 1; // Hang Dump and save the FB accurately.
        unsigned int DuplicateHang     : 1; // resore the enviroment saved by PreHangDump/PostHangDump to duplcate the hang
        unsigned int DuplicateHangPerFB: 1; // resore the enviroment saved by PreHangDump/PostHangDump to duplcate the hang
        unsigned int InternalDumpHW    : 1; // Dump framebuffer mirror to file so that duplicat the hang .
        unsigned int InternalBlockSubmit: 1; // block sumbit during replaying
        unsigned int QTDevice          : 1; // to make sure the duplication on QT (MAMM) takes place on the simulated device
        unsigned int LogMemoryUsage    : 1; // create file c:\MemoryUsage.txt
        unsigned int DumpDebugBus      : 1; //fpga dump debug bus information
        unsigned int DumpDebugBusToFile   : 1;
        unsigned int InternalDumpDebugBus : 1;
        unsigned int DmaNum               : 6; // multi dma number
        unsigned int DumpHangDma          : 1;
        unsigned int InternalDumpHangDma  : 1;
        unsigned int Reserved             : 9;
    };
    unsigned int uint;
} Reg_DebugMode_E3K;

typedef struct _Hang_Record
{
    struct _info
    {
        void*                DeviceHandle;
        unsigned int         Fence_Id;
        DWORD                dwDmaAddress;
        DWORD                dwDmaSize;
        DWORD                dwRingbufferAddr;
        DWORD                dwDMAFlag;
    } info[6];
    unsigned int dwCurIndex;
}Hang_Record;

typedef struct _EngineSatus_e3k
{
    Reg_Block_Busy_Bits_Top          Top;
    Reg_Block_Busy_Bits_Gpc0_0       Gpc0_0;
    Reg_Block_Busy_Bits_Gpc0_1       Gpc0_1;
    Reg_Block_Busy_Bits_Gpc1_0       Gpc1_0;
    Reg_Block_Busy_Bits_Gpc1_1       Gpc1_1;
    Reg_Block_Busy_Bits_Gpc2_0       Gpc2_0;
    Reg_Block_Busy_Bits_Gpc2_1       Gpc2_1;
    Reg_Vcp_Vpp_Block_Busy_Bits      ES_VCP_VPP;
} EngineSatus_e3k;

// Immediate Mode: The register data is followed by the command, usually in the DMA buffer. This
// is used to set those frequently changed simple short states
// No maskable SetRegister command
__inline DWORD SET_REGISTER_FD_E3K(DWORD Block, DWORD Offset, DWORD Dwf)
{
    Csp_Opcodes_cmd Cmd = {0};

    Cmd.cmd_Set_Register.Major_Opcode = CSP_OPCODE_Set_Register;
    Cmd.cmd_Set_Register.Block_Id     = Block;
    Cmd.cmd_Set_Register.Start_Offset = Offset;
    Cmd.cmd_Set_Register.Dwc          = Dwf;

    return Cmd.uint;
}

  // Generate set register mask header
  // RegValue, Mask is set outof this function
__inline DWORD SET_REGISTER_MH_E3K(DWORD Block,
                                   DWORD Offset)
{
    Csp_Opcodes_cmd  Cmd = {0};

    Cmd.cmd_Set_Register.Major_Opcode = CSP_OPCODE_Set_Register;
    Cmd.cmd_Set_Register.Block_Id     = Block;
    Cmd.cmd_Set_Register.Start_Offset = Offset;
    Cmd.cmd_Set_Register.Addr_En      = 0;
    Cmd.cmd_Set_Register.Mask_En      = 1;
    Cmd.cmd_Set_Register.Dwc          = 2;

    return Cmd.uint;
}

__inline DWORD SET_REGISTER_MASK_E3K(DWORD Block,
                                     DWORD Offset,
                                     DWORD RegValue,
                                     DWORD Mask,
                                     DWORD* pCmd)
{
    Csp_Opcodes_cmd  Cmd = {0};

    Cmd.cmd_Set_Register.Major_Opcode = CSP_OPCODE_Set_Register;
    Cmd.cmd_Set_Register.Block_Id     = Block;
    Cmd.cmd_Set_Register.Start_Offset = Offset;
    Cmd.cmd_Set_Register.Addr_En      = 0;
    Cmd.cmd_Set_Register.Mask_En      = 1;
    Cmd.cmd_Set_Register.Dwc          = 2;

    *pCmd++ = Cmd.uint;
    *pCmd++ = RegValue;
    *pCmd++ = Mask;

    return 3;
}

// The register settings are in a memory where the second DW points to can be used to restore
// saved context.
__inline DWORD SET_REGISTER_ADDR_E3K(DWORD Block, DWORD Offset,BOOL AddressMode)
{
    Csp_Opcodes_cmd Cmd = {0};

    Cmd.cmd_Set_Register.Major_Opcode = CSP_OPCODE_Set_Register;
    Cmd.cmd_Set_Register.Block_Id     = Block;
    Cmd.cmd_Set_Register.Start_Offset = Offset;
    Cmd.cmd_Set_Register.Addr_En      = 1;
    Cmd.cmd_Set_Register.Mask_En      = 0;
    Cmd.cmd_Set_Register.Address_Mode = AddressMode;
    Cmd.cmd_Set_Register.Dwc          = 2;

    return Cmd.uint;
}

_inline DWORD SET_REGISTER_ADDR_LOW_E3K(DWORD LowAddress)
{
    Cmd_Set_Register_Addr_Dword1 Cmd = {0};

    Cmd.Address_Low32 = LowAddress & ~0x3;

    return *(DWORD*)&Cmd;
}

_inline DWORD SET_REGISTER_ADDR_HIGH_AND_REGCNT_E3K(DWORD HighAddress, DWORD RegCnt, int IsPA)
{
    Cmd_Set_Register_Addr_Dword2 Cmd = {0};

    Cmd.Address_High8 = HighAddress;
    Cmd.Reg_Cnt       = RegCnt;
    Cmd.If_Physical   = IsPA;
    return *(DWORD*)&Cmd;
}

_inline DWORD SET_REGISTER_ADDR_HIGH_AND_REGCNT_AND_L2_E3K(DWORD HighAddress, DWORD RegCnt, DWORD L2cacheable)
{
    Cmd_Set_Register_Addr_Dword2 Cmd = {0};

    Cmd.Address_High8 = HighAddress;
    Cmd.Reg_Cnt       = RegCnt;
    Cmd.L2_Cachable   = L2cacheable;
    return *(DWORD*)&Cmd;
}

_inline DWORD SEND_SKIP_E3K(DWORD SkipCount)
{
    Csp_Opcodes_cmd SkipCmd = {0};
    SkipCmd.cmd_Skip.Dwc = (DWORD)(SkipCount - 1);
    SkipCmd.cmd_Skip.Major_Opcode = CSP_OPCODE_Skip;

    return SkipCmd.uint;
}

_inline DWORD SEND_TBR_INDICATOR_COMMAND_E3K(DWORD value)
{
    Csp_Opcodes_cmd indicator = {0};

    indicator.cmd_Tbr_Indicator.Block_Id = TASBE_BLOCK;
    indicator.cmd_Tbr_Indicator.Major_Opcode = CSP_OPCODE_Tbr_Indicator;
    indicator.cmd_Tbr_Indicator.Indicator_Info = value;

    return *((DWORD*)&indicator);
}

//Notice:this command only invalidate d,z,s, not include u cache
_inline DWORD SEND_FFCACHE_INVALIDATE_COMMAND_E3K(void)
{
    Csp_Opcodes_cmd Inv = {0};

    Inv.cmd_Block_Command_Flush.Major_Opcode = CSP_OPCODE_Block_Command_Flush;
    Inv.cmd_Block_Command_Flush.Block_Id     = FF_BLOCK;
    //invalidate all cache lines into mxu (only D, Z, S).
    Inv.cmd_Block_Command_Flush.Target       = BLOCK_COMMAND_FLUSH_TARGET_ALL_C;
    Inv.cmd_Block_Command_Flush.Type         = BLOCK_COMMAND_FLUSH_TYPE_INVALIDATE_CACHE;
    Inv.cmd_Block_Command_Flush.Dwc          = 0;

    return Inv.uint;
}

_inline DWORD SEND_UCACHE_3DFE_INVALIDATE_COMMAND_E3K(void)
{
    Csp_Opcodes_cmd Inv = {0};

    Inv.cmd_Block_Command_Flush.Major_Opcode = CSP_OPCODE_Block_Command_Flush;
    Inv.cmd_Block_Command_Flush.Block_Id     = WLS_FE_BLOCK;
    Inv.cmd_Block_Command_Flush.Target       = BLOCK_COMMAND_FLUSH_TARGET_UAV_C;
    Inv.cmd_Block_Command_Flush.Uav_Type     = BLOCK_COMMAND_FLUSH_UAV_TYPE_3DFE;
    Inv.cmd_Block_Command_Flush.Type         = BLOCK_COMMAND_FLUSH_TYPE_INVALIDATE_CACHE;
    Inv.cmd_Block_Command_Flush.Dwc          = 0;

    return Inv.uint;
}

_inline DWORD SEND_UCACHE_3DBE_INVALIDATE_COMMAND_E3K(void)
{
    Csp_Opcodes_cmd Inv = {0};

    Inv.cmd_Block_Command_Flush.Major_Opcode = CSP_OPCODE_Block_Command_Flush;
    Inv.cmd_Block_Command_Flush.Block_Id     = WLS_BE_BLOCK;
    Inv.cmd_Block_Command_Flush.Target       = BLOCK_COMMAND_FLUSH_TARGET_UAV_C;
    Inv.cmd_Block_Command_Flush.Uav_Type     = BLOCK_COMMAND_FLUSH_UAV_TYPE_3DBE;
    Inv.cmd_Block_Command_Flush.Type         = BLOCK_COMMAND_FLUSH_TYPE_INVALIDATE_CACHE;
    Inv.cmd_Block_Command_Flush.Dwc          = 0;

    return Inv.uint;
}

_inline DWORD SEND_UCACHE_CSL_INVALIDATE_COMMAND_E3K(void)
{
    Csp_Opcodes_cmd Inv = {0};

    Inv.cmd_Block_Command_Flush.Major_Opcode = CSP_OPCODE_Block_Command_Flush;
    Inv.cmd_Block_Command_Flush.Block_Id     = WLS_FE_BLOCK;
    Inv.cmd_Block_Command_Flush.Target       = BLOCK_COMMAND_FLUSH_TARGET_UAV_C;
    Inv.cmd_Block_Command_Flush.Uav_Type     = BLOCK_COMMAND_FLUSH_UAV_TYPE_CSL;
    Inv.cmd_Block_Command_Flush.Type         = BLOCK_COMMAND_FLUSH_TYPE_INVALIDATE_CACHE;
    Inv.cmd_Block_Command_Flush.Dwc          = 0;

    return Inv.uint;
}

_inline DWORD SEND_UCACHE_CSH_INVALIDATE_COMMAND_E3K(void)
{
    Csp_Opcodes_cmd Inv = {0};

    Inv.cmd_Block_Command_Flush.Major_Opcode = CSP_OPCODE_Block_Command_Flush;
    Inv.cmd_Block_Command_Flush.Block_Id     = WLS_FE_BLOCK;
    Inv.cmd_Block_Command_Flush.Target       = BLOCK_COMMAND_FLUSH_TARGET_UAV_C;
    Inv.cmd_Block_Command_Flush.Uav_Type     = BLOCK_COMMAND_FLUSH_UAV_TYPE_CSH;
    Inv.cmd_Block_Command_Flush.Type         = BLOCK_COMMAND_FLUSH_TYPE_INVALIDATE_CACHE;
    Inv.cmd_Block_Command_Flush.Dwc          = 0;

    return Inv.uint;
}


_inline DWORD SEND_TU_FORCE_FLUSH_COMMAND_CHX004(void)
{
    Csp_Opcodes_cmd     Inv = { 0 };

    Inv.cmd_Blk_Cmd_Csp_Force_Flush.Major_Opcode = CSP_OPCODE_Blk_Cmd_Csp_Force_Flush;
    Inv.cmd_Blk_Cmd_Csp_Force_Flush.Block_Id = CSP_GLOBAL_BLOCK;
    Inv.cmd_Blk_Cmd_Csp_Force_Flush.Type = BLOCK_COMMAND_CSP_TYPE_FORCE_FLUSH;
    Inv.cmd_Blk_Cmd_Csp_Force_Flush.Reg_Type = BLK_CMD_CSP_FORCE_FLUSH_REG_TYPE_TS;
    Inv.cmd_Blk_Cmd_Csp_Force_Flush.Dwc = 0;

    return Inv.uint;
}


_inline DWORD SEND_DCACHE_INVALIDATE_COMMAND_E3K(void)
{
    Csp_Opcodes_cmd Inv = {0};

    Inv.cmd_Block_Command_Flush.Major_Opcode = CSP_OPCODE_Block_Command_Flush;
    Inv.cmd_Block_Command_Flush.Block_Id     = FF_BLOCK;
    Inv.cmd_Block_Command_Flush.Target       = BLOCK_COMMAND_FLUSH_TARGET_D_C;
    Inv.cmd_Block_Command_Flush.Type         = BLOCK_COMMAND_FLUSH_TYPE_INVALIDATE_CACHE;
    Inv.cmd_Block_Command_Flush.Dwc          = 0;

    return Inv.uint;
}

_inline DWORD SEND_2D_ONLY_INVALIDATE_COMMAND_E3K(void)
{
    Csp_Opcodes_cmd Inv = {0};

    Inv.cmd_Block_Command_Flush.Major_Opcode = CSP_OPCODE_Block_Command_Flush;
    Inv.cmd_Block_Command_Flush.Block_Id     = FF_BLOCK;
    Inv.cmd_Block_Command_Flush.Target       = BLOCK_COMMAND_FLUSH_TARGET_2D_ONLY;
    Inv.cmd_Block_Command_Flush.Type         = BLOCK_COMMAND_FLUSH_TYPE_INVALIDATE_CACHE;
    Inv.cmd_Block_Command_Flush.Dwc          = 0;

    return Inv.uint;
}

_inline DWORD SEND_DEPTH_INVALIDATE_COMMAND_E3K(void)
{
    Csp_Opcodes_cmd Inv = {0};

    Inv.cmd_Block_Command_Flush.Major_Opcode = CSP_OPCODE_Block_Command_Flush;
    Inv.cmd_Block_Command_Flush.Block_Id     = FF_BLOCK;
    Inv.cmd_Block_Command_Flush.Target       = BLOCK_COMMAND_FLUSH_TARGET_Z_C;
    Inv.cmd_Block_Command_Flush.Type         = BLOCK_COMMAND_FLUSH_TYPE_INVALIDATE_CACHE;
    Inv.cmd_Block_Command_Flush.Dwc          = 0;

    return Inv.uint;
}

_inline DWORD SEND_STENCIL_INVALIDATE_COMMAND_E3K(void)
{
    Csp_Opcodes_cmd Inv = {0};

    Inv.cmd_Block_Command_Flush.Major_Opcode = CSP_OPCODE_Block_Command_Flush;
    Inv.cmd_Block_Command_Flush.Block_Id     = FF_BLOCK;
    Inv.cmd_Block_Command_Flush.Target       = BLOCK_COMMAND_FLUSH_TARGET_S_C;
    Inv.cmd_Block_Command_Flush.Type         = BLOCK_COMMAND_FLUSH_TYPE_INVALIDATE_CACHE;
    Inv.cmd_Block_Command_Flush.Dwc          = 0;

    return Inv.uint;
}

_inline DWORD SEND_FLAGBUFFER_INVALIDATE_COMMAND_E3K(void)
{
    Csp_Opcodes_cmd FbInvCmd = {0};

    FbInvCmd.cmd_Block_Command_Mxu.Major_Opcode     = CSP_OPCODE_Block_Command_Mxu;
    FbInvCmd.cmd_Block_Command_Mxu.Block_Id         = MXU_BLOCK;
    FbInvCmd.cmd_Block_Command_Mxu.Cmd_Type         = BLOCK_COMMAND_MXU_CMD_TYPE_INVALIDATE_CACHE;
    FbInvCmd.cmd_Block_Command_Mxu.Dwc              = 4;

    return FbInvCmd.uint;
}

_inline DWORD SEND_GMCACHE_INVALIDATE_COMMAND_E3K(void)
{
    return 0;
}

_inline DWORD SEND_TUFE_CSL_L1CACHE_INVALIDATE_COMMAND_E3K(void)
{
    Csp_Opcodes_cmd TexInvCmd = {0};

    TexInvCmd.cmd_Block_Command_Template.Major_Opcode             = CSP_OPCODE_Block_Command_Tu;
    TexInvCmd.cmd_Block_Command_Template.Block_Id                 = TUFE_BLOCK;
    TexInvCmd.cmd_Block_Command_Template.Command_Specific_Field   = 2;//cmd_spec_field==2, zone_type==CSL
    TexInvCmd.cmd_Block_Command_Template.Type                     = BLOCK_COMMAND_TU_TYPE_INVALIDATE_L1;
    TexInvCmd.cmd_Block_Command_Template.Dwc                      = 0;

    return TexInvCmd.uint;
}

_inline DWORD SEND_TUFE_CSH_L1CACHE_INVALIDATE_COMMAND_E3K(void)
{
    Csp_Opcodes_cmd TexInvCmd = {0};

    TexInvCmd.cmd_Block_Command_Template.Major_Opcode             = CSP_OPCODE_Block_Command_Tu;
    TexInvCmd.cmd_Block_Command_Template.Block_Id                 = TUFE_BLOCK;
    TexInvCmd.cmd_Block_Command_Template.Command_Specific_Field   = 3;//cmd_spec_field==3, zone_type==CSH
    TexInvCmd.cmd_Block_Command_Template.Type                     = BLOCK_COMMAND_TU_TYPE_INVALIDATE_L1;
    TexInvCmd.cmd_Block_Command_Template.Dwc                      = 0;

    return TexInvCmd.uint;
}

_inline DWORD SEND_TUFE_L1CACHE_INVALIDATE_COMMAND_E3K(void)
{
    Csp_Opcodes_cmd TexInvCmd = {0};

    TexInvCmd.cmd_Block_Command_Template.Major_Opcode             = CSP_OPCODE_Block_Command_Tu;
    TexInvCmd.cmd_Block_Command_Template.Block_Id                 = TUFE_BLOCK;
    TexInvCmd.cmd_Block_Command_Template.Command_Specific_Field   = 0;//cmd_spec_field==0, zone_type==3DFE
    TexInvCmd.cmd_Block_Command_Template.Type                     = BLOCK_COMMAND_TU_TYPE_INVALIDATE_L1;
    TexInvCmd.cmd_Block_Command_Template.Dwc                      = 0;

    return TexInvCmd.uint;
}

_inline DWORD SEND_TUBE_L1CACHE_INVALIDATE_COMMAND_E3K(void)
{
    Csp_Opcodes_cmd TexInvCmd = {0};

    TexInvCmd.cmd_Block_Command_Template.Major_Opcode             = CSP_OPCODE_Block_Command_Tu;
    TexInvCmd.cmd_Block_Command_Template.Block_Id                 = TUBE_BLOCK;
    TexInvCmd.cmd_Block_Command_Template.Command_Specific_Field   = 1;//cmd_spec_field==1, zone_type==3DBE
    TexInvCmd.cmd_Block_Command_Template.Type                     = BLOCK_COMMAND_TU_TYPE_INVALIDATE_L1;
    TexInvCmd.cmd_Block_Command_Template.Dwc                      = 0;

    return TexInvCmd.uint;
}

__inline DWORD SEND_TUFE_PRE_FETCH_COMMAND_E3K(DWORD HeapIdx,
                                               BOOL PreFetchEnd,
                                               DWORD Offset,
                                               DWORD SlotNum,
                                               DWORD* pCmd)
{
    Csp_Opcodes_cmd PreFetch = { 0 };

    PreFetch.cmd_Block_Command_Tu_CHX004.Major_Opcode = CSP_OPCODE_Block_Command_Tu;
    PreFetch.cmd_Block_Command_Tu_CHX004.Block_Id = TUFE_BLOCK;
    PreFetch.cmd_Block_Command_Tu_CHX004.Type = BLOCK_COMMAND_TU_TYPE_PRE_FETCH;
    PreFetch.cmd_Block_Command_Tu_CHX004.Command_Specific_Field = 0;
    PreFetch.cmd_Block_Command_Tu_CHX004.Heap_Idx = HeapIdx;
    PreFetch.cmd_Block_Command_Tu_CHX004.Pre_Fetch_End = PreFetchEnd;
    PreFetch.cmd_Block_Command_Tu_CHX004.Dwc = 2;

    *pCmd++ = PreFetch.uint;
    *pCmd++ = Offset;
    *pCmd++ = SlotNum;

    return 3;
}

__inline DWORD SEND_TUBE_PRE_FETCH_COMMAND_E3K(DWORD HeapIdx,
                                               BOOL PreFetchEnd,
                                               DWORD Offset,
                                               DWORD SlotNum,
                                               DWORD* pCmd)
{
    Csp_Opcodes_cmd PreFetch = { 0 };

    PreFetch.cmd_Block_Command_Tu_CHX004.Major_Opcode = CSP_OPCODE_Block_Command_Tu;
    PreFetch.cmd_Block_Command_Tu_CHX004.Block_Id = TUBE_BLOCK;
    PreFetch.cmd_Block_Command_Tu_CHX004.Type = BLOCK_COMMAND_TU_TYPE_PRE_FETCH;
    PreFetch.cmd_Block_Command_Tu_CHX004.Command_Specific_Field = 1;
    PreFetch.cmd_Block_Command_Tu_CHX004.Heap_Idx = HeapIdx;
    PreFetch.cmd_Block_Command_Tu_CHX004.Pre_Fetch_End = PreFetchEnd;
    PreFetch.cmd_Block_Command_Tu_CHX004.Dwc = 2;

    *pCmd++ = PreFetch.uint;
    *pCmd++ = Offset;
    *pCmd++ = SlotNum;

    return 3;
}

__inline DWORD SEND_FFCACHE_FLUSH_COMMAND_E3K(void)
{
    Csp_Opcodes_cmd Flush = {0};

    Flush.cmd_Block_Command_Flush.Major_Opcode = CSP_OPCODE_Block_Command_Flush;
    Flush.cmd_Block_Command_Flush.Block_Id     = FF_BLOCK;
    //flush all cache lines into mxu (only D, Z, S).
    Flush.cmd_Block_Command_Flush.Target       = BLOCK_COMMAND_FLUSH_TARGET_ALL_C;
    Flush.cmd_Block_Command_Flush.Type         = BLOCK_COMMAND_FLUSH_TYPE_FLUSH;
    Flush.cmd_Block_Command_Flush.Dwc          = 0;

    return Flush.uint;
}

_inline DWORD SEND_DSIGBUF_FLUSH_COMMAND_E3K(void)
{
    Csp_Opcodes_cmd DSigBufFlushCmd = {0};

    DSigBufFlushCmd.cmd_Block_Command_Flush.Major_Opcode = CSP_OPCODE_Block_Command_Flush;
    DSigBufFlushCmd.cmd_Block_Command_Flush.Block_Id     = FF_BLOCK;
    DSigBufFlushCmd.cmd_Block_Command_Flush.Target       = BLOCK_COMMAND_FLUSH_TARGET_D_SIG_BUF;
    DSigBufFlushCmd.cmd_Block_Command_Flush.Type         = BLOCK_COMMAND_TEMPLATE_TYPE_FLUSH;
    DSigBufFlushCmd.cmd_Block_Command_Flush.Dwc          = 0;

    return DSigBufFlushCmd.uint;
}

_inline DWORD SEND_UCACHE_3DFE_FLUSH_COMMAND_E3K(void)
{
    Csp_Opcodes_cmd Flush = {0};

    Flush.cmd_Block_Command_Flush.Major_Opcode = CSP_OPCODE_Block_Command_Flush;
    Flush.cmd_Block_Command_Flush.Block_Id     = WLS_FE_BLOCK;
    Flush.cmd_Block_Command_Flush.Target       = BLOCK_COMMAND_FLUSH_TARGET_UAV_C;
    Flush.cmd_Block_Command_Flush.Uav_Type     = BLOCK_COMMAND_FLUSH_UAV_TYPE_3DFE;
    Flush.cmd_Block_Command_Flush.Type         = BLOCK_COMMAND_FLUSH_TYPE_FLUSH;
    Flush.cmd_Block_Command_Flush.Dwc          = 0;

    return Flush.uint;
}

_inline DWORD SEND_UCACHE_3DBE_FLUSH_COMMAND_E3K(void)
{
    Csp_Opcodes_cmd Flush = {0};

    Flush.cmd_Block_Command_Flush.Major_Opcode = CSP_OPCODE_Block_Command_Flush;
    Flush.cmd_Block_Command_Flush.Block_Id     = WLS_BE_BLOCK;
    Flush.cmd_Block_Command_Flush.Target       = BLOCK_COMMAND_FLUSH_TARGET_UAV_C;
    Flush.cmd_Block_Command_Flush.Uav_Type     = BLOCK_COMMAND_FLUSH_UAV_TYPE_3DBE;
    Flush.cmd_Block_Command_Flush.Type         = BLOCK_COMMAND_FLUSH_TYPE_FLUSH;
    Flush.cmd_Block_Command_Flush.Dwc          = 0;

    return Flush.uint;
}

_inline DWORD SEND_UCACHE_CSL_FLUSH_COMMAND_E3K(void)
{
    Csp_Opcodes_cmd Flush = {0};

    Flush.cmd_Block_Command_Flush.Major_Opcode = CSP_OPCODE_Block_Command_Flush;
    Flush.cmd_Block_Command_Flush.Block_Id     = WLS_FE_BLOCK;
    Flush.cmd_Block_Command_Flush.Target       = BLOCK_COMMAND_FLUSH_TARGET_UAV_C;
    Flush.cmd_Block_Command_Flush.Uav_Type     = BLOCK_COMMAND_FLUSH_UAV_TYPE_CSL;
    Flush.cmd_Block_Command_Flush.Type         = BLOCK_COMMAND_FLUSH_TYPE_FLUSH;
    Flush.cmd_Block_Command_Flush.Dwc          = 0;

    return Flush.uint;
}

_inline DWORD SEND_UCACHE_CSH_FLUSH_COMMAND_E3K(void)
{
    Csp_Opcodes_cmd Flush = {0};

    Flush.cmd_Block_Command_Flush.Major_Opcode = CSP_OPCODE_Block_Command_Flush;
    Flush.cmd_Block_Command_Flush.Block_Id     = WLS_FE_BLOCK;
    Flush.cmd_Block_Command_Flush.Target       = BLOCK_COMMAND_FLUSH_TARGET_UAV_C;
    Flush.cmd_Block_Command_Flush.Uav_Type     = BLOCK_COMMAND_FLUSH_UAV_TYPE_CSH;
    Flush.cmd_Block_Command_Flush.Type         = BLOCK_COMMAND_FLUSH_TYPE_FLUSH;
    Flush.cmd_Block_Command_Flush.Dwc          = 0;

    return Flush.uint;
}

_inline DWORD SEND_UCACHE_3DL_FLUSH_COMMAND_E3K(void)
{
    Csp_Opcodes_cmd Flush = {0};

    Flush.cmd_Block_Command_Flush.Major_Opcode = CSP_OPCODE_Block_Command_Flush;
    Flush.cmd_Block_Command_Flush.Block_Id     = WLS_FE_BLOCK;
    Flush.cmd_Block_Command_Flush.Target       = BLOCK_COMMAND_FLUSH_TARGET_USHARP_DESC;
    Flush.cmd_Block_Command_Flush.Type         = BLOCK_COMMAND_FLUSH_TYPE_FLUSH;
    Flush.cmd_Block_Command_Flush.Dwc          = 0;

    return Flush.uint;
}

_inline DWORD SEND_DCACHE_FLUSH_COMMAND_E3K(void)
{
    Csp_Opcodes_cmd Flush = {0};

    Flush.cmd_Block_Command_Flush.Major_Opcode = CSP_OPCODE_Block_Command_Flush;
    Flush.cmd_Block_Command_Flush.Block_Id     = FF_BLOCK;
    Flush.cmd_Block_Command_Flush.Target       = BLOCK_COMMAND_FLUSH_TARGET_D_C;
    Flush.cmd_Block_Command_Flush.Type         = BLOCK_COMMAND_FLUSH_TYPE_FLUSH;
    Flush.cmd_Block_Command_Flush.Dwc          = 0;

    return Flush.uint;
}

_inline DWORD SEND_2D_ONLY_FLUSH_COMMAND_E3K(void)
{
    Csp_Opcodes_cmd Flush = {0};

    Flush.cmd_Block_Command_Flush.Major_Opcode = CSP_OPCODE_Block_Command_Flush;
    Flush.cmd_Block_Command_Flush.Block_Id     = FF_BLOCK;
    Flush.cmd_Block_Command_Flush.Target       = BLOCK_COMMAND_FLUSH_TARGET_2D_ONLY;
    Flush.cmd_Block_Command_Flush.Type         = BLOCK_COMMAND_FLUSH_TYPE_FLUSH;
    Flush.cmd_Block_Command_Flush.Dwc          = 0;

    return Flush.uint;
}

_inline DWORD SEND_DEPTH_FLUSH_COMMAND_E3K(void)
{
    Csp_Opcodes_cmd Flush = {0};

    Flush.cmd_Block_Command_Flush.Major_Opcode = CSP_OPCODE_Block_Command_Flush;
    Flush.cmd_Block_Command_Flush.Block_Id     = FF_BLOCK;
    Flush.cmd_Block_Command_Flush.Target       = BLOCK_COMMAND_FLUSH_TARGET_Z_C;
    Flush.cmd_Block_Command_Flush.Type         = BLOCK_COMMAND_FLUSH_TYPE_FLUSH;
    Flush.cmd_Block_Command_Flush.Dwc          = 0;

    return Flush.uint;
}

_inline DWORD SEND_STENCIL_FLUSH_COMMAND_E3K(void)
{
    Csp_Opcodes_cmd Flush = {0};

    Flush.cmd_Block_Command_Flush.Major_Opcode = CSP_OPCODE_Block_Command_Flush;
    Flush.cmd_Block_Command_Flush.Block_Id     = FF_BLOCK;
    Flush.cmd_Block_Command_Flush.Target       = BLOCK_COMMAND_FLUSH_TARGET_S_C;
    Flush.cmd_Block_Command_Flush.Type         = BLOCK_COMMAND_FLUSH_TYPE_FLUSH;
    Flush.cmd_Block_Command_Flush.Dwc          = 0;

    return Flush.uint;
}

_inline DWORD SEND_FLAGBUFFER_FLUSH_COMMAND_E3K(void)
{
    Csp_Opcodes_cmd FbInvCmd = {0};

    FbInvCmd.cmd_Block_Command_Mxu.Major_Opcode     = CSP_OPCODE_Block_Command_Mxu;
    FbInvCmd.cmd_Block_Command_Mxu.Block_Id         = MXU_BLOCK;
    FbInvCmd.cmd_Block_Command_Mxu.Cmd_Type         = BLOCK_COMMAND_MXU_CMD_TYPE_FLUSH_CACHE;
    FbInvCmd.cmd_Block_Command_Mxu.Dwc              = 4;

    return FbInvCmd.uint;
}

_inline DWORD SEND_GMCACHE_FLUSH_COMMAND_E3K(void)
{
    return 0;
}

_inline DWORD SEND_BL_CLEAR_COMMAND_E3K(DWORD dwCounter)
{
    Csp_Opcodes_cmd     clearCmd={0};

    clearCmd.cmd_Blk_Cmd_Csp_Blc.Type         = BLOCK_COMMAND_CSP_TYPE_CLEAR_MXU_BLC;
    clearCmd.cmd_Blk_Cmd_Csp_Blc.Counter      = dwCounter;
    clearCmd.cmd_Blk_Cmd_Csp_Blc.Block_Id     = CSP_GLOBAL_BLOCK;
    clearCmd.cmd_Blk_Cmd_Csp_Blc.Major_Opcode = CSP_OPCODE_Blk_Cmd_Csp_Blc;
    clearCmd.cmd_Blk_Cmd_Csp_Blc.Dwc          = 3;

    return (DWORD)clearCmd.uint;
}
_inline DWORD SEND_RT_CLEAR_COMMAND_E3K(BOOL bPredicate,BOOL bTiled)
{
    Csp_Opcodes_cmd  Cmd = {0};

    Cmd.cmd_Block_Command_Sg.Major_Opcode = CSP_OPCODE_Block_Command_Sg;
    Cmd.cmd_Block_Command_Sg.Block_Id     = FF_BLOCK;
    Cmd.cmd_Block_Command_Sg.Predicate_En = bPredicate;
    Cmd.cmd_Block_Command_Sg.Action_Type  = (bTiled)
                                          ? BLOCK_COMMAND_SG_ACTION_TYPE_FAST_CLEAR_TILE
                                          : BLOCK_COMMAND_SG_ACTION_TYPE_FAST_CLEAR_LINEAR;

    Cmd.cmd_Block_Command_Sg.Area_Target  = BLOCK_COMMAND_SG_AREA_TARGET_D;
    Cmd.cmd_Block_Command_Sg.Type         = BLOCK_COMMAND_TEMPLATE_TYPE_SG;
    Cmd.cmd_Block_Command_Sg.Dwc          = 2;

    return (DWORD)Cmd.uint;
}

_inline DWORD SEND_DEPTH_CLEAR_COMMAND_E3K(BOOL bPredicate)
{
    Csp_Opcodes_cmd     Zc={0};

    Zc.cmd_Block_Command_Sg.Major_Opcode  = CSP_OPCODE_Block_Command_Sg;
    Zc.cmd_Block_Command_Sg.Block_Id      = FF_BLOCK;
    Zc.cmd_Block_Command_Sg.Predicate_En  = bPredicate;
    Zc.cmd_Block_Command_Sg.Action_Type   = BLOCK_COMMAND_SG_ACTION_TYPE_FAST_CLEAR_TILE;
    Zc.cmd_Block_Command_Sg.Area_Target   = BLOCK_COMMAND_SG_AREA_TARGET_Z;
    Zc.cmd_Block_Command_Sg.Type          = BLOCK_COMMAND_TEMPLATE_TYPE_SG;
    Zc.cmd_Block_Command_Sg.Dwc           = 2;

    return (DWORD)Zc.uint;
}

_inline DWORD SEND_STENCIL_CLEAR_COMMAND_E3K(BOOL bPredicate)
{
    Csp_Opcodes_cmd     Sc={0};

    Sc.cmd_Block_Command_Sg.Major_Opcode = CSP_OPCODE_Block_Command_Sg;
    Sc.cmd_Block_Command_Sg.Block_Id     = FF_BLOCK;
    Sc.cmd_Block_Command_Sg.Predicate_En = bPredicate;
    Sc.cmd_Block_Command_Sg.Action_Type  = BLOCK_COMMAND_SG_ACTION_TYPE_FAST_CLEAR_TILE;
    Sc.cmd_Block_Command_Sg.Area_Target  = BLOCK_COMMAND_SG_AREA_TARGET_S;
    Sc.cmd_Block_Command_Sg.Type         = BLOCK_COMMAND_TEMPLATE_TYPE_SG;
    Sc.cmd_Block_Command_Sg.Dwc          = 2;

    return (DWORD)Sc.uint;
}

_inline DWORD SEND_MCE_RT_CLEAR_COMMAND_E3K(BOOL bTiled)
{
    Csp_Opcodes_cmd  Cmd = {0};

    Cmd.cmd_Block_Command_Sg.Major_Opcode = CSP_OPCODE_Block_Command_Sg;
    Cmd.cmd_Block_Command_Sg.Block_Id     = MCE_BLOCK;
    Cmd.cmd_Block_Command_Sg.Predicate_En = 0;
    Cmd.cmd_Block_Command_Sg.Action_Type  = (bTiled)
                                          ? BLOCK_COMMAND_SG_ACTION_TYPE_FAST_CLEAR_TILE
                                          : BLOCK_COMMAND_SG_ACTION_TYPE_FAST_CLEAR_LINEAR;

    Cmd.cmd_Block_Command_Sg.Area_Target  = BLOCK_COMMAND_SG_AREA_TARGET_D;
    Cmd.cmd_Block_Command_Sg.Type         = BLOCK_COMMAND_TEMPLATE_TYPE_SG;
    Cmd.cmd_Block_Command_Sg.Dwc          = 2;

    return (DWORD)Cmd.uint;
}

_inline DWORD SEND_MCE_DEPTH_CLEAR_COMMAND_E3K(void)
{
    Csp_Opcodes_cmd     Zc={0};

    Zc.cmd_Block_Command_Sg.Major_Opcode  = CSP_OPCODE_Block_Command_Sg;
    Zc.cmd_Block_Command_Sg.Block_Id      = MCE_BLOCK;
    Zc.cmd_Block_Command_Sg.Predicate_En  = 0;
    Zc.cmd_Block_Command_Sg.Action_Type   = BLOCK_COMMAND_SG_ACTION_TYPE_FAST_CLEAR_TILE;
    Zc.cmd_Block_Command_Sg.Area_Target   = BLOCK_COMMAND_SG_AREA_TARGET_Z;
    Zc.cmd_Block_Command_Sg.Type          = BLOCK_COMMAND_TEMPLATE_TYPE_SG;
    Zc.cmd_Block_Command_Sg.Dwc           = 2;

    return (DWORD)Zc.uint;
}

_inline DWORD SEND_MCE_STENCIL_CLEAR_COMMAND_E3K(void)
{
    Csp_Opcodes_cmd     Sc={0};

    Sc.cmd_Block_Command_Sg.Major_Opcode = CSP_OPCODE_Block_Command_Sg;
    Sc.cmd_Block_Command_Sg.Block_Id     = MCE_BLOCK;
    Sc.cmd_Block_Command_Sg.Predicate_En = 0;
    Sc.cmd_Block_Command_Sg.Action_Type  = BLOCK_COMMAND_SG_ACTION_TYPE_FAST_CLEAR_TILE;
    Sc.cmd_Block_Command_Sg.Area_Target  = BLOCK_COMMAND_SG_AREA_TARGET_S;
    Sc.cmd_Block_Command_Sg.Type         = BLOCK_COMMAND_TEMPLATE_TYPE_SG;
    Sc.cmd_Block_Command_Sg.Dwc          = 2;

    return (DWORD)Sc.uint;
}

__inline DWORD SEND_2DCOPY_COMMAND_E3K(BOOL bPredicate, BOOL bOverlay)
{
    Csp_Opcodes_cmd     Cmd = {0};

    Cmd.cmd_Block_Command_Sg.Major_Opcode = CSP_OPCODE_Block_Command_Sg;
    Cmd.cmd_Block_Command_Sg.Block_Id     = FF_BLOCK;
    Cmd.cmd_Block_Command_Sg.Predicate_En = bPredicate;
    Cmd.cmd_Block_Command_Sg.Blt_Overlap  = bOverlay;
    Cmd.cmd_Block_Command_Sg.Action_Type  = BLOCK_COMMAND_SG_ACTION_TYPE_BIT_BLT;
    Cmd.cmd_Block_Command_Sg.Area_Target  = BLOCK_COMMAND_SG_AREA_TARGET_D;
    Cmd.cmd_Block_Command_Sg.Type         = BLOCK_COMMAND_TEMPLATE_TYPE_SG;
    Cmd.cmd_Block_Command_Sg.Dwc          = 3;

    return (DWORD)Cmd.uint;
}


_inline DWORD SEND_GRADIENTFILL_COMMAND_E3K(void)
{
    Csp_Opcodes_cmd     Cmd = {0};

    Cmd.cmd_Block_Command_Sg.Major_Opcode = CSP_OPCODE_Block_Command_Sg;
    Cmd.cmd_Block_Command_Sg.Block_Id     = FF_BLOCK;
    Cmd.cmd_Block_Command_Sg.Predicate_En = 0;
    Cmd.cmd_Block_Command_Sg.Action_Type  = BLOCK_COMMAND_SG_ACTION_TYPE_GRADIENT_FILL;
    Cmd.cmd_Block_Command_Sg.Area_Target  = BLOCK_COMMAND_SG_AREA_TARGET_D;
    Cmd.cmd_Block_Command_Sg.Type         = BLOCK_COMMAND_TEMPLATE_TYPE_SG;
    Cmd.cmd_Block_Command_Sg.Dwc          = 14;

    return (DWORD)Cmd.uint;
}

__inline DWORD SEND_2D_ROTATION_E3K(void)
{
    Csp_Opcodes_cmd     Cmd = {0};

    Cmd.cmd_Block_Command_Sg.Major_Opcode = CSP_OPCODE_Block_Command_Sg;
    Cmd.cmd_Block_Command_Sg.Block_Id     = FF_BLOCK;
    Cmd.cmd_Block_Command_Sg.Predicate_En = 0;
    Cmd.cmd_Block_Command_Sg.Action_Type  = BLOCK_COMMAND_SG_ACTION_TYPE_ROTATION;
    Cmd.cmd_Block_Command_Sg.Area_Target  = BLOCK_COMMAND_SG_AREA_TARGET_D;
    Cmd.cmd_Block_Command_Sg.Type         = BLOCK_COMMAND_TEMPLATE_TYPE_SG;
    Cmd.cmd_Block_Command_Sg.Dwc          = 1;

    return (DWORD)Cmd.uint;
}

__inline DWORD SEND_STRETCHBLT_COMMAND_E3K(void)
{
    Csp_Opcodes_cmd     Cmd = {0};

    Cmd.cmd_Block_Command_Sg.Major_Opcode = CSP_OPCODE_Block_Command_Sg;
    Cmd.cmd_Block_Command_Sg.Block_Id     = FF_BLOCK;
    Cmd.cmd_Block_Command_Sg.Predicate_En = 0;
    Cmd.cmd_Block_Command_Sg.Action_Type  = BLOCK_COMMAND_SG_ACTION_TYPE_BIT_BLT;
    Cmd.cmd_Block_Command_Sg.Area_Target  = BLOCK_COMMAND_SG_AREA_TARGET_D;
    Cmd.cmd_Block_Command_Sg.Type         = BLOCK_COMMAND_TEMPLATE_TYPE_SG;
    Cmd.cmd_Block_Command_Sg.Dwc          = 9;

    return (DWORD)Cmd.uint;
}


_inline DWORD SEND_COPY_IMM_COMMAND_E3K(DWORD Dwf, BOOL bPredicate)
{
    Csp_Opcodes_cmd     CopyImm={0};

    CopyImm.cmd_Block_Command_Img_Trn.Major_Opcode = CSP_OPCODE_Block_Command_Img_Trn;
    CopyImm.cmd_Block_Command_Img_Trn.Block_Id     = FF_BLOCK;
    CopyImm.cmd_Block_Command_Img_Trn.Type         = BLOCK_COMMAND_TEMPLATE_TYPE_IMAGE_TRANSFER;
    CopyImm.cmd_Block_Command_Img_Trn.Predicate_En = bPredicate;
    CopyImm.cmd_Block_Command_Img_Trn.Dwc          = Dwf;

    return (DWORD)CopyImm.uint;
}

_inline DWORD SEND_DP_LINE_COMMAND_E3K(BOOL ColorIncluded, DWORD lineNum)
{
    Csp_Opcodes_cmd     DPLine={0};
    DWORD dwc = ColorIncluded ? (1 + lineNum*2) : (lineNum*2);

    DPLine.cmd_Block_Command_Tas.Major_Opcode           = CSP_OPCODE_Block_Command_Tas;
    DPLine.cmd_Block_Command_Tas.Type                   = ColorIncluded;
    DPLine.cmd_Block_Command_Tas.Command_Specific_Field = BLOCK_COMMAND_TAS_COMMAND_SPECIFIC_FIELD_DP_LINE;
    DPLine.cmd_Block_Command_Tas.Block_Id               = TASFE_BLOCK;
    DPLine.cmd_Block_Command_Tas.Dwc                    = dwc;

    return (DWORD)DPLine.uint;
}

__inline DWORD SEND_DP_LINE_COMMAND_CHX004(BOOL ColorIncluded)
{
    Csp_Opcodes_cmd     DPLine={0};

    DPLine.cmd_Block_Command_Tas.Major_Opcode           = CSP_OPCODE_Block_Command_Tas;
    DPLine.cmd_Block_Command_Tas.Type                   = ColorIncluded;
    DPLine.cmd_Block_Command_Tas.Command_Specific_Field = BLOCK_COMMAND_TAS_COMMAND_SPECIFIC_FIELD_DP_LINE;
    DPLine.cmd_Block_Command_Tas.Block_Id               = TASBE_BLOCK;
    DPLine.cmd_Block_Command_Tas.Dwc                    = ColorIncluded ? 3 : 2;

    return (DWORD)DPLine.uint;
}


_inline DWORD SEND_DRAWAUTO_COMMAND_E3K(DWORD P_Type, BOOL bInstanceMode, BOOL bPredicate)
{
    Csp_Opcodes_cmd     DIPCmd={0};

    DIPCmd.cmd_Dip.Major_Opcode    = CSP_OPCODE_Dip;
    DIPCmd.cmd_Dip.P_Type          = P_Type;
    DIPCmd.cmd_Dip.Draw_Mode       = DIP_DRAW_MODE_IMM;
    DIPCmd.cmd_Dip.Instance_En     = bInstanceMode;
    DIPCmd.cmd_Dip.Predicate_En     = bPredicate;

    return (DWORD)DIPCmd.uint;
}

__inline DWORD SEND_DRAW_COMMAND_CHX004(DWORD P_Type, DWORD IndexSize, BOOL bInstanceMode)
{
    Csp_Opcodes_cmd     DIPCmd={0};
    DWORD Index_Mode = 0;

//    switch (IndexSize)
//    {
//    case 1:
//        Index_Mode   = DIP_MODE_IB_8;
//        break;
//    case 2:
//        Index_Mode   = DIP_MODE_IB_16;
//        break;
//    case 4:
//        Index_Mode   = DIP_MODE_IB_32;
//        break;
//    default:
//#ifndef __linux__
//        /*only for win32 can use _asm keywords*/
//#if ((_WIN32) && (!_WIN64) && (!_ARM_))
//        _asm int 3;
//#endif
//#endif
//        Index_Mode   = DIP_MODE_IB_32;
//        break;
//    }

    DIPCmd.cmd_Dip.Major_Opcode    = CSP_OPCODE_Dip;
    DIPCmd.cmd_Dip.P_Type          = P_Type;
    DIPCmd.cmd_Dip.Draw_Mode       = Index_Mode;
    DIPCmd.cmd_Dip.Instance_En     = bInstanceMode;

    return (DWORD)DIPCmd.uint;
}

__inline DWORD SEND_DRAW_COMMAND_E3K(DWORD P_Type, DWORD IndexSize, BOOL bInstanceMode)
{
    Csp_Opcodes_cmd     DIPCmd={0};
    DWORD Index_Mode;

    switch (IndexSize)
    {
    case 1:
        Index_Mode   = DIP_MODE_IB_8;
        break;
    case 2:
        Index_Mode   = DIP_MODE_IB_16;
        break;
    case 4:
        Index_Mode   = DIP_MODE_IB_32;
        break;
    default:
#ifndef __linux__
        /*only for win32 can use _asm keywords*/
#if ((_WIN32) && (!_WIN64) && (!_ARM_))
        _asm int 3;
#endif
#endif
        Index_Mode   = DIP_MODE_IB_32;
        break;
    }

    DIPCmd.cmd_Dip.Major_Opcode = CSP_OPCODE_Dip;
    DIPCmd.cmd_Dip.P_Type = P_Type;
    DIPCmd.cmd_Dip.Draw_Mode = Index_Mode;
    DIPCmd.cmd_Dip.Instance_En = bInstanceMode;

    return (DWORD)DIPCmd.uint;
}

// Internal Fence Command
__inline DWORD SEND_INTERNAL_WRITEFENCE_E3K(DWORD Route, DWORD RbType, DWORD Slot)
{
    Csp_Opcodes_cmd     Cmd = {0};

    Cmd.cmd_Fence.Major_Opcode         = CSP_OPCODE_Fence;
    Cmd.cmd_Fence.Fence_Type           = FENCE_FENCE_TYPE_INTERNAL;
    Cmd.cmd_Fence.Slot_Id              = Slot;
    Cmd.cmd_Fence.Route_Id             = Route;
    Cmd.cmd_Fence.Rb_Type              = RbType;
    Cmd.cmd_Fence.Fence_Update_Mode    = FENCE_FENCE_UPDATE_MODE_COPY;
    Cmd.cmd_Fence.Dwc                  = 1;
    return (DWORD)Cmd.uint;
}

// Internal Write Fence Value
__inline DWORD SEND_INTERNAL_FENCE_VALUE_E3K(DWORD FenceId)
{
    Cmd_Fence_Internal_Dword1   Cmd = {0};
    Cmd.Update_Value    = (FenceId & 0xFFFF);
    Cmd.Slice_Mask        = 0;
    return *(DWORD*) &Cmd;
}

// Internal Wait Command
__inline DWORD SEND_INTERNAL_WAIT_E3K(DWORD Slot, DWORD WaitMethod, DWORD WaitValue)
{
    Csp_Opcodes_cmd    Cmd = {0};

    Cmd.cmd_Wait.Major_Opcode = CSP_OPCODE_Wait;
    Cmd.cmd_Wait.Slot_Id      = Slot;
    Cmd.cmd_Wait.Station_Id   = WAIT_STATION_ID_PRE_PARSER;
    Cmd.cmd_Wait.Wait_Mode    = WAIT_WAIT_MODE_NORMAL_WAIT;
    Cmd.cmd_Wait.Method       = WaitMethod;
    Cmd.cmd_Wait.Dwc          = WaitValue; //for internal fence, dwc == waitvalue
    return (DWORD)Cmd.uint;
 }


__inline DWORD SEND_INTERNAL_WAIT_MAIN_E3K(DWORD         Slot,
                                           DWORD         WaitMethod,
                                           DWORD         WaitValue)
{
    Csp_Opcodes_cmd     WaitCmd = {0};

    WaitCmd.cmd_Wait.Major_Opcode           = CSP_OPCODE_Wait;
    WaitCmd.cmd_Wait.Slot_Id                = Slot;
    WaitCmd.cmd_Wait.Station_Id             = WAIT_STATION_ID_MAIN_PARSER;
    WaitCmd.cmd_Wait.Wait_Mode              = WAIT_WAIT_MODE_NORMAL_WAIT;
    WaitCmd.cmd_Wait.Method                 = WaitMethod;
    WaitCmd.cmd_Wait.Dwc                    = WaitValue;

    return (DWORD)WaitCmd.uint;
}


// Internal Wait Value
// Can't find the waitValue define, use the FenceValue temporarily
__inline DWORD SEND_INTERNAL_WAIT_VALUE_E3K(DWORD FenceId)
{
    Cmd_Fence_Internal_Dword1      Cmd = {0};

    Cmd.Update_Value        =   (FenceId & 0xFFFF);
    Cmd.Slice_Mask          =   0;
    return *(DWORD*)&Cmd;
}

typedef enum RBTYPE_E3K
{
    _RBT_3DFE  = 0,
    _RBT_3DBE  = 1,
    _RBT_CSL  = 2,
    _RBT_CSH  = 3,
}RBTYPE_E3K;

// Internal Write Fence Command for ring buffer swap
__inline DWORD SEND_RING_BUFFER_SWAP_INTERNALFENCE_E3K(RBTYPE_E3K   RbType,
                                                       DWORD        RouteID,
                                                       DWORD        Slot,
                                                       DWORD        Value,
                                                       DWORD*       pCmd)
{
    Csp_Opcodes_cmd     FenceCmd = {0};

    FenceCmd.cmd_Fence.Major_Opcode        = CSP_OPCODE_Fence;
    FenceCmd.cmd_Fence.Fence_Type          = FENCE_FENCE_TYPE_INTERNAL;
    FenceCmd.cmd_Fence.Dwc                 = 1;

    FenceCmd.cmd_Fence.Route_Id            = RouteID;
    FenceCmd.cmd_Fence.Slot_Id             = Slot;
    FenceCmd.cmd_Fence.Fence_Update_Mode   = FENCE_FENCE_UPDATE_MODE_COPY;
    FenceCmd.cmd_Fence.Rb_Type             = RbType;

    *pCmd++ = FenceCmd.uint;
    *pCmd++ = Value;

    return 2;
}


__inline DWORD UPDATE_INTERNAL_WRITEFENCE_E3K(RBTYPE_E3K   RbType,
                                              DWORD        RouteID,
                                              DWORD        Slot,
                                              DWORD        Value,
                                              DWORD*       pCmd)
{
    Csp_Opcodes_cmd     FenceCmd = {0};

    FenceCmd.cmd_Fence.Major_Opcode        = CSP_OPCODE_Fence;
    FenceCmd.cmd_Fence.Fence_Type          = FENCE_FENCE_TYPE_INTERNAL;
    FenceCmd.cmd_Fence.Dwc                 = 1;

    FenceCmd.cmd_Fence.Route_Id            = RouteID;
    FenceCmd.cmd_Fence.Slot_Id             = Slot;
    FenceCmd.cmd_Fence.Fence_Update_Mode   = FENCE_FENCE_UPDATE_MODE_OR;
    FenceCmd.cmd_Fence.Rb_Type             = RbType;

    *pCmd++ = FenceCmd.uint;
    *pCmd++ = Value;

    return 2;
}

// Internal Wait Command
//_inline DWORD SEND_INTERNAL_WAIT_E3K(DWORD         Slot,
//                                      DWORD         Value,
//                                      DWORD         WaitMethod,
//                                      DWORD*        pCmd)
//{
//    Csp_Opcodes_cmd     WaitCmd = {0};
//
//    WaitCmd.cmd_Wait.Major_Opcode           = CSP_OPCODE_Wait;
//    WaitCmd.cmd_Wait.Slot_Id                = Slot;
//    WaitCmd.cmd_Wait.Dwc                    = 1;
//    WaitCmd.cmd_Wait.Station_Id             = WAIT_STATION_ID_PRE_PARSER;
//    WaitCmd.cmd_Wait.Wait_Mode              = WAIT_WAIT_MODE_NORMAL_WAIT;
//    WaitCmd.cmd_Wait.Method                 = WaitMethod;
//
//    *pCmd++ = WaitCmd.uint;
//    *pCmd++ = Value;
//
//    return 2;
//}
//
// Internal Wait Command
//__inline DWORD SEND_INTERNAL_WAIT_MAIN_E3K(DWORD         Slot,
//                                           DWORD         Value,
//                                           DWORD         WaitMethod,
//                                           DWORD*        pCmd)
//{
//    Csp_Opcodes_cmd     WaitCmd = {0};
//
//    WaitCmd.cmd_Wait.Major_Opcode           = CSP_OPCODE_Wait;
//    WaitCmd.cmd_Wait.Slot_Id                = Slot;
//    WaitCmd.cmd_Wait.Dwc                    = 1;
//    WaitCmd.cmd_Wait.Station_Id             = WAIT_STATION_ID_MAIN_PARSER;
//    WaitCmd.cmd_Wait.Wait_Mode              = WAIT_WAIT_MODE_NORMAL_WAIT;
//    WaitCmd.cmd_Wait.Method                 = WaitMethod;
//
//    *pCmd++ = WaitCmd.uint;
//    *pCmd++ = Value;
//
//    return 2;
//}

__inline DWORD SEND_QUERY_DUMP_E3K(DWORD BlockId, DWORD DumpSize, BOOL AddressMode)
{
    Csp_Opcodes_cmd    Cmd = {0};

    Cmd.cmd_Query_Dump.Major_Opcode     = CSP_OPCODE_Query_Dump;
    Cmd.cmd_Query_Dump.Block_Id         = BlockId;
    Cmd.cmd_Query_Dump.Address_Mode     = AddressMode;
    Cmd.cmd_Query_Dump.Cmd_Type         = QUERY_DUMP_CMD_TYPE_NORMAL_QUERY;
    Cmd.cmd_Query_Dump.Dwc_To_Dump      = DumpSize;
    Cmd.cmd_Query_Dump.Dwc              = 2;
    return (DWORD)Cmd.uint;
}

__inline DWORD SEND_QUERY_ADDR1_E3K(DWORD Address1)
{
    Cmd_Query_Dump_Address_Dword1 DumpAddr1 = {0};

    DumpAddr1.Address_Low32 = Address1;
    return *(DWORD*)&DumpAddr1;
}

__inline DWORD SEND_QUERY_ADDR2_AND_OFFSET_E3K(DWORD Address2, DWORD Offset, BOOL Gart_En, BOOL Top256T)
{
    Cmd_Query_Dump_Address_Dword2 DumpAddr2 = {0};

    DumpAddr2.Address_High8     = Address2;
    DumpAddr2.Reg_Offset        = Offset;
    return *(DWORD*)&DumpAddr2;
}

__inline DWORD SEND_EXTERNAL_FENCE_E3K(DWORD IrqType)
{
    Csp_Opcodes_cmd   Cmd = {0};

    Cmd.cmd_Fence.Major_Opcode = CSP_OPCODE_Fence;
    Cmd.cmd_Fence.Fence_Type   = FENCE_FENCE_TYPE_EXTERNAL64;
    Cmd.cmd_Fence.Irq          = IrqType;
    Cmd.cmd_Fence.Dwc          = 4;
    return (DWORD)Cmd.uint;
}
//Fence Blt imm data csp->mxu->mem
__inline DWORD SEND_EXTERNAL_FENCE_BLT_E3K(DWORD IrqType, DWORD dataSizeDw)
{
    Csp_Opcodes_cmd   Cmd = { 0 };

    Cmd.cmd_Fence.Major_Opcode = CSP_OPCODE_Fence;
    Cmd.cmd_Fence.Fence_Type = FENCE_FENCE_TYPE_IMM_DATA_BLT;
    Cmd.cmd_Fence.Irq = IrqType;
    Cmd.cmd_Fence.Dwc = 2;
    Cmd.cmd_Fence.Imm_Dw_Num = dataSizeDw;
    return (DWORD)Cmd.uint;
}

//Write External Fence Address1
__inline DWORD SEND_EXTERNAL_FENCE_ADDR1_E3K(DWORD Address1)
{
    Cmd_Fence_External_Addr_Dword1  Cmd = {0};

    Cmd.External_Addr_Low32 =  Address1;
    return *(DWORD*)&Cmd;
}

//Write External Fence Address2
__inline DWORD SEND_EXTERNAL_FENCE_ADDR2_E3K(DWORD Address2)
{
    Cmd_Fence_External_Addr_Dword2  Cmd = {0};

    Cmd.External_Addr_High8 = Address2;
    return *(DWORD*)&Cmd;
}

// External wait command
__inline DWORD SEND_EXTERNAL_WAIT_E3K(void)
{
    Csp_Opcodes_cmd     WaitCmd = {0};

    WaitCmd.cmd_Wait.Major_Opcode           = CSP_OPCODE_Wait;
    WaitCmd.cmd_Wait.Dwc                    = 4;
    WaitCmd.cmd_Wait.Wait_Mode              = WAIT_WAIT_MODE_EXTERNAL_WAIT;

    return WaitCmd.uint;
}

// Disable shadow buffer in FF
__inline DWORD DISABLE_FF_SHADOW_E3K(void)
{
    return 0;
}

// Enable shadow buffer in FF and restore reg from shadow buffer
__inline DWORD RESTORE_AND_ENABLE_FF_SHADOW_E3K(void)
{
    return 0;
}

// Dump context of block from offset
// Dump size is DumpSize(of DWORD)
// context include:
// BCI
// EU VS/GP
// EU PS
// Setup * SG => RU?
// TU ZU FFC
// WBU
// Shared Memory => blockid? offset?
// can dump consecutive blocks
__inline DWORD SEND_CONTEXT_DUMP_E3K(DWORD Block, DWORD Offset, DWORD DumpSize, DWORD address, DWORD* pCmd)
{
    return 0;
}

// dump status to address where the second DW points,
// dump size is DumpSize(of DWORD)
// status includes
// perfomanece counter // ILA
// timestamp
// engine busy status
// fence counter
__inline DWORD SEND_QUERY_STATUS_E3K(DWORD BlockId, DWORD Offset, DWORD DumpSize, UINT64 address, DWORD* pCmd)
{
    Csp_Opcodes_cmd     DumpCmd={0};
    DWORD* pCmd0 = pCmd;
    DWORD addressLowPart = (address & 0xffffffff);
    DWORD addressHighPart = (address >> 32) & 0xffffffff;

    DumpCmd.cmd_Query_Dump.Major_Opcode     = CSP_OPCODE_Query_Dump;
    DumpCmd.cmd_Query_Dump.Block_Id         = BlockId;
    DumpCmd.cmd_Query_Dump.Cmd_Type         = QUERY_DUMP_CMD_TYPE_NORMAL_QUERY;
    DumpCmd.cmd_Query_Dump.Dwc_To_Dump      = DumpSize;
    DumpCmd.cmd_Query_Dump.Dwc              = 2;

    *pCmd++ = DumpCmd.uint;

    *pCmd++ = addressLowPart;

    *pCmd++ = (Offset << 19) | (addressHighPart & 0xFF);

    return (DWORD)(pCmd - pCmd0);
}

__inline DWORD SEND_QUERY_COPY_RESULT_E3K(DWORD     BlockId,
                                          BOOL      b64Bit,
                                          BOOL      bAvail,
                                          BOOL      bPartial,
                                          BOOL      bTwoSlice,
                                          BOOL      bTimestamp,
                                          DWORD     Offset,
                                          DWORD     AvailOffset,
                                          DWORD     srcAddress,
                                          DWORD     dstAddress,
                                          DWORD*    pCmd)
{
    return 0;
}

__inline DWORD SEND_QUERY_STATUS_E3K_VIDEO(DWORD Offset, DWORD DumpSize, DWORD address, DWORD* pCmd)
{
    return 0;
}

// Gp command for executing a CS shader
__inline DWORD SEND_GP_COMMAND_E3K(BOOL bPredicateEn, BOOL bWaitDoneEn,DWORD DwType)
{
    Csp_Opcodes_cmd     GpCmd={0};

    GpCmd.cmd_Gp.Major_Opcode               = CSP_OPCODE_Gp;
    GpCmd.cmd_Gp.Block_Id                   = EU_CS_BLOCK;
    GpCmd.cmd_Gp.Dw_Type                    = DwType;
    GpCmd.cmd_Gp.Dwc                        = 13;
    GpCmd.cmd_Gp.Indirect_En                = 0;
    GpCmd.cmd_Gp.Predicate_En               = bPredicateEn ? 1 : 0;
    GpCmd.cmd_Gp.Wait_Done_En               = bWaitDoneEn; // low rb: TRUE; high rb: FALSE

    return (DWORD)GpCmd.uint;
}

// Gp command for executing a cl kernel
__inline DWORD SEND_GP_OCL_COMMAND_E3K(BOOL bWaitDoneEn)
{
    Csp_Opcodes_cmd     GpCmd={0};

    GpCmd.cmd_Gp.Major_Opcode               = CSP_OPCODE_Gp;
    GpCmd.cmd_Gp.Block_Id                   = EU_CS_BLOCK;
    GpCmd.cmd_Gp.Dw_Type                    = GP_DW_TYPE_GLOBAL_SIZE;
    GpCmd.cmd_Gp.Dwc                        = 13;
    GpCmd.cmd_Gp.Wait_Done_En               = bWaitDoneEn;
    GpCmd.cmd_Gp.Indirect_En                = 0;

    return (DWORD)GpCmd.uint;
}

// Gp command with indirect buffer for executing a CS shader
__inline DWORD SEND_GP_COMMAND_WITH_INDIRECT_BUFFER_E3K(BOOL bPredicateEn, BOOL bWaitDoneEn)
{
    Csp_Opcodes_cmd     GpCmd={0};

    GpCmd.cmd_Gp.Major_Opcode               = CSP_OPCODE_Gp;
    GpCmd.cmd_Gp.Block_Id                   = EU_CS_BLOCK;
    GpCmd.cmd_Gp.Dw_Type                    = GP_DW_TYPE_GROUP_NUMBER;
    GpCmd.cmd_Gp.Dwc                        = 12;
    GpCmd.cmd_Gp.Indirect_En                = 1;
    GpCmd.cmd_Gp.Predicate_En               = bPredicateEn ? 1 : 0;
    GpCmd.cmd_Gp.Wait_Done_En               = bWaitDoneEn; // low rb: TRUE; high rb: FALSE

    return (DWORD)GpCmd.uint;
}

// Drain FS
__inline DWORD SEND_DRAIN_FS_COMMAND_E3K(void)
{
    Csp_Opcodes_cmd   Cmd = {0};

    Cmd.cmd_Block_Command_Eu.Major_Opcode = CSP_OPCODE_Block_Command_Eu;
    Cmd.cmd_Block_Command_Eu.Block_Id = EU_FS_BLOCK;
    Cmd.cmd_Block_Command_Eu.Command_Specific_Field = 0;
    Cmd.cmd_Block_Command_Eu.Type  = BLOCK_COMMAND_EU_TYPE_DRAIN_EU;
    Cmd.cmd_Block_Command_Eu.Dwc = 0;
    return Cmd.uint;
}

// Drain CS
__inline DWORD SEND_DRAIN_CS_COMMAND_E3K(void)
{
    return 0;
}

// Drain CSL
__inline DWORD SEND_DRAIN_CSL_COMMAND_E3K(void)
{
    Csp_Opcodes_cmd   Cmd = {0};

    Cmd.cmd_Block_Command_Eu.Major_Opcode               = CSP_OPCODE_Block_Command_Eu;
    Cmd.cmd_Block_Command_Eu.Block_Id                   = EU_CS_BLOCK;
    Cmd.cmd_Block_Command_Eu.Command_Specific_Field     = 2;
    Cmd.cmd_Block_Command_Eu.Type                       = BLOCK_COMMAND_EU_TYPE_DRAIN_EU;
    Cmd.cmd_Block_Command_Eu.Dwc                        = 0;

    return Cmd.uint;
}

// Drain CSH
__inline DWORD SEND_DRAIN_CSH_COMMAND4(void)
{
    Csp_Opcodes_cmd   Cmd = {0};

    Cmd.cmd_Block_Command_Eu.Major_Opcode               = CSP_OPCODE_Block_Command_Eu;
    Cmd.cmd_Block_Command_Eu.Block_Id                   = EU_CS_BLOCK;
    Cmd.cmd_Block_Command_Eu.Command_Specific_Field     = 3;
    Cmd.cmd_Block_Command_Eu.Type                       = BLOCK_COMMAND_EU_TYPE_DRAIN_EU;
    Cmd.cmd_Block_Command_Eu.Dwc                        = 0;

    return Cmd.uint;
}

// Drain PS
__inline DWORD SEND_DRAIN_PS_COMMAND_E3K(void)
{
    Csp_Opcodes_cmd   Cmd = {0};

    Cmd.cmd_Block_Command_Eu.Major_Opcode               = CSP_OPCODE_Block_Command_Eu;
    Cmd.cmd_Block_Command_Eu.Block_Id                   = EU_PS_BLOCK;
    Cmd.cmd_Block_Command_Eu.Command_Specific_Field     = 1;
    Cmd.cmd_Block_Command_Eu.Type                       = BLOCK_COMMAND_EU_TYPE_DRAIN_EU;
    Cmd.cmd_Block_Command_Eu.Dwc                        = 0;

    return Cmd.uint;
}

// Insert FS_Cfg before FS INVALIDATE COMMAND
__inline DWORD SEND_FS_CFG_COMMAND_E3K(DWORD* pCmd)
{
    return 0;
}

// Invalidate FS L1 instruction cache
_inline DWORD SEND_FS_L1I_INVALIDATE_COMMAND_E3K(void)
{
    Csp_Opcodes_cmd     BlockCommandEu = {0};

    BlockCommandEu.cmd_Block_Command_Eu.Major_Opcode           = CSP_OPCODE_Block_Command_Eu;
    BlockCommandEu.cmd_Block_Command_Eu.Block_Id               = EU_FS_BLOCK;
    BlockCommandEu.cmd_Block_Command_Eu.Command_Specific_Field = 0;
    BlockCommandEu.cmd_Block_Command_Eu.Type                   = BLOCK_COMMAND_EU_TYPE_INVALIDATE_L1I;
    BlockCommandEu.cmd_Block_Command_Eu.Dwc                    = 0;

    return (DWORD)BlockCommandEu.uint;
}

// Invalidate FS/CS L1 instruction cache
_inline DWORD SEND_CS_L1I_INVALIDATE_COMMAND_E3K(void)
{
    Csp_Opcodes_cmd     BlockCommandEu = {0};

    BlockCommandEu.cmd_Block_Command_Eu.Major_Opcode           = CSP_OPCODE_Block_Command_Eu;
    BlockCommandEu.cmd_Block_Command_Eu.Block_Id               = EU_CS_BLOCK;
    BlockCommandEu.cmd_Block_Command_Eu.Command_Specific_Field = 2;
    BlockCommandEu.cmd_Block_Command_Eu.Type                   = BLOCK_COMMAND_EU_TYPE_INVALIDATE_L1I;
    BlockCommandEu.cmd_Block_Command_Eu.Dwc                    = 0;

    return (DWORD)BlockCommandEu.uint;
}

// Insert PS_Cfg before PS INVALIDATE COMMAND
__inline DWORD SEND_PS_CFG_COMMAND_E3K(DWORD* pCmd)
{
    return 0;
}

// Invalidate PS L1 instruction cache
__inline DWORD SEND_PS_L1I_INVALIDATE_COMMAND_E3K(void)
{
    Csp_Opcodes_cmd     BlockCommandEu = {0};

    BlockCommandEu.cmd_Block_Command_Eu.Major_Opcode           = CSP_OPCODE_Block_Command_Eu;
    BlockCommandEu.cmd_Block_Command_Eu.Block_Id               = EU_PS_BLOCK;
    BlockCommandEu.cmd_Block_Command_Eu.Command_Specific_Field = 1;
    BlockCommandEu.cmd_Block_Command_Eu.Type                   = BLOCK_COMMAND_EU_TYPE_INVALIDATE_L1I;
    BlockCommandEu.cmd_Block_Command_Eu.Dwc                    = 0;

    return (DWORD)BlockCommandEu.uint;
}

// Invalidate L2 cache
__inline DWORD SEND_L2_INVALIDATE_COMMAND_E3K(DWORD usage)
{
    Csp_Opcodes_cmd     InvalidateCmd={0};

    InvalidateCmd.cmd_Block_Command_L2.Major_Opcode     = CSP_OPCODE_Block_Command_L2;
    InvalidateCmd.cmd_Block_Command_L2.Block_Id         = QUERY_DUMP_BLOCK_ID_L2;
    // Cmd_Type == 1 : incalidate
    // Cmd_Type == 0 : flush
    InvalidateCmd.cmd_Block_Command_L2.Type                = BLOCK_COMMAND_L2_TYPE_INVALIDATE_L2;
    InvalidateCmd.cmd_Block_Command_L2.Usage            = usage;
    InvalidateCmd.cmd_Block_Command_L2.Dwc              = 0;

    return (DWORD)InvalidateCmd.uint;
}



// Flush L2 cache
__inline DWORD SEND_L2_FLUSH_COMMAND_E3K(DWORD usage, BOOL bInternalFlush)
{
    Csp_Opcodes_cmd     InvalidateCmd={0};

    InvalidateCmd.cmd_Block_Command_L2.Major_Opcode     = CSP_OPCODE_Block_Command_L2;
    InvalidateCmd.cmd_Block_Command_L2.Block_Id         = QUERY_DUMP_BLOCK_ID_L2;
    // Cmd_Type == 1 : incalidate
    // Cmd_Type == 0 : flush
    InvalidateCmd.cmd_Block_Command_L2.Type                = bInternalFlush ? BLOCK_COMMAND_L2_TYPE_FLUSH_L1 : BLOCK_COMMAND_L2_TYPE_FLUSH_L2;
    InvalidateCmd.cmd_Block_Command_L2.Usage            = usage;
    InvalidateCmd.cmd_Block_Command_L2.Dwc              = 0;

    return (DWORD)InvalidateCmd.uint;
}

// Set One Slice
__inline DWORD SET_ONE_SLICE_CMD_E3K(BOOL bElt1K, DWORD* pCmd)
{
    return 0;
}

__inline DWORD SEND_CSP_RESV_CACHE_SIZE_CMD_E3K(DWORD staticSize, DWORD dynamic0Size, DWORD dynamic1Size)
{
    return 0;
}

__inline DWORD SEND_DIP_CMD_E3K(BOOL  bPredicate,
                                DWORD DipMode,
                                BOOL  bInstance,
                                BOOL  bIbOffset,
                                BOOL  bVbOffset,
                                BOOL  bInstanceOffset,
                                DWORD PrimitiveType,
                                DWORD PatchVetexCount,
                                BOOL  bIndirect)
{
    Csp_Opcodes_cmd  DIPcmd = {0};

    DIPcmd.cmd_Dip.Major_Opcode         = CSP_OPCODE_Dip;
    DIPcmd.cmd_Dip.Predicate_En         = bPredicate;
    DIPcmd.cmd_Dip.Draw_Mode            = DipMode;
    DIPcmd.cmd_Dip.Instance_En          = bInstance;
    DIPcmd.cmd_Dip.Start_Index_Valid    = ((DipMode == DIP_DRAW_MODE_INDEX) && bIndirect) || bIbOffset;
    DIPcmd.cmd_Dip.Start_Vertex_Valid   = ((DipMode == DIP_DRAW_MODE_INDEX) && bIndirect) || bVbOffset;
    DIPcmd.cmd_Dip.Start_Instance_Valid = bIndirect ? 1 : bInstanceOffset;//! for indirect draw, should always set it
    DIPcmd.cmd_Dip.P_Type               = PrimitiveType;
    DIPcmd.cmd_Dip.Patch_Vertex_Count   = PatchVetexCount;

    DIPcmd.cmd_Dip.Indirect             = bIndirect;

    return DIPcmd.uint;
}

#endif //_REGISTERCOMMANDS_H_

