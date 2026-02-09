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
#ifndef __VIDMMI_ELT3K_H__
#define __VIDMMI_ELT3K_H__

#define  FAIL    0xffffffff

typedef union vidmm_vm_pte_elt3k
{
    unsigned int value;
    //ttbr?
    PTE_L1_t pte_level1;
    PTE_L2_t pte_level2;
    PTE_L3_t pte_level3;
} vidmm_vm_pte_elt3k_t;


//inherit from cil3/e3k/kmd/os_interface/include/zxcommand.h
typedef struct _2d_bltimm_cmd_elt3k
{
    unsigned int            SetDstRTAddrCtrl;      // set Dst RT Addr Ctrl Register 3 DWORD
    Reg_Rt_Addr             DstAddr;
    Reg_Rt_Depth            DstDepth;
    Reg_Rt_View_Ctrl        DstViewCtrl;
    unsigned int            SetDstRTCtrl;         // set Dst RT Ctrl Register 3 DWORD
    Reg_Rt_Fmt              DstFormat;
    Reg_Rt_Size             DstSize;
    Reg_Rt_Misc             DstMisc;              // might need disable D_Read_En, disable alphablend, need to disable Rop

    unsigned int            SetSrcRTAddrCtrl;      // set Dst RT Addr Ctrl Register 3 DWORD
    Reg_Rt_Addr             SrcAddr;
    Reg_Rt_Depth            SrcDepth;
    Reg_Rt_View_Ctrl        SrcViewCtrl;
    unsigned int            SetSrcRTCtrl;          // set Dst RT Ctrl Register 3 DWORD
    Reg_Rt_Fmt              SrcFormat;
    Reg_Rt_Size             SrcSize;
    Reg_Rt_Misc             SrcMisc;

    unsigned int            Set_Zs_Req_Ctrl;   //?? why need this???
    Reg_Zs_Req_Ctrl         Zs_Req_Ctrl;        // might need disable Zl2_End_Pipe_En

    unsigned int            Set_Rast_Ctrl;
    Reg_Rast_Ctrl           Rast_Ctrl;

    DWORD                   Set_Tasbe_Ctrl;
    Reg_Tasbe_Ctrl          reg_Tasbe_Ctrl;

    unsigned int            Set_Tasfe_Ctrl;
    Reg_Tasfe_Ctrl          reg_Tasfe_Ctrl;

    unsigned int            Set_Resolution_Ctrl;
    Reg_Resolution_Ctrl     reg_Resolution_Ctrl;

    DWORD                   Set_Tasfe_Ctrl_Misc;
    Reg_Tasfe_Ctrl_Misc     reg_Tasfe_Ctrl_Misc;

    unsigned int            Set_Dzs_Ctrl;  //??//?? why need this???
    Reg_Dzs_Ctrl            Dzs_Ctrl;

    unsigned int            Set_Ff_Glb_Ctrl;  //??//?? why need this???
    Reg_Ff_Glb_Ctrl         Ff_Glb_Ctrl;

} _2d_bltimm_cmd_elt3k_t;



extern int  vidmm_describe_allocation_e3k(adapter_t*, vidmm_describe_allocation_t*);
extern void vidmm_query_chip_vm_info_e3k(adapter_t *adapter, vidmm_chip_vm_info_t *vm_info);
extern int  vidmm_update_ptes_e3k(zx_vma_space_t* vma_space, int engine_index, vidmm_vm_update_ptes_arg_t *update_pte_arg);
extern int  vidmm_clear_bl_buffer_e3k(void *dma, unsigned int dma_size, unsigned int bl_slot_index, unsigned int alloc_size, unsigned long long clear_value);
extern unsigned int  vidmm_alloc_bl_slot_e3k(adapter_t *adapter, vidmm_allocation_t *allocation);
extern int  vidmm_free_bl_slot_e3k(adapter_t *adapter, vidmm_allocation_t *allocation);
extern int  vidmm_build_page_buffer_e3k(adapter_t*, vidmm_private_build_paging_buffer_arg_t*);

#endif

