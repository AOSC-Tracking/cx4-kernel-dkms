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
#ifndef __VIDSCH_ENGINE_ZX2K_H
#define __VIDSCH_ENGINE_ZX2K_H

#include "vidsch.h"
#include "vidschi.h"
#include "ring_buffer.h"
#include "chip_include_e3k.h"
#include "vidsch_3dblt_e3k.h"

#define EXTERNAL_FENCE_DWSIZE      (5)
#define ALIGNED_8(Value)           (((Value) + 7) & ~7)
#define BEGIN_END_BUF_SIZE_E3K     \
        ((sizeof(RB_PREDEFINE_DMA) + 4095) & (~4095))

#define RING_BUFFER_SIZE_E3K       (64 * 4096) //need align to 4K?
#define KKK_ARGUMENT_BUFFER_SIZE   (0x10000)
#define VIDEO_BRIDGE_BUF_SIZE      (0x800000)   //reserve 8M bridge buffer used as video FE output for one video core

#define POWER_SKIP_COMMAND_SIZE     16

#define HW_CONTEXT_COUNT_E3K       (2)

#define E3K_ENGINE_HW_QUEUE_SIZE   5

//typedef enum
//{
//    HWM_SYNC_VS_SLOT = 0,
//    HWM_SYNC_ZL2_SLOT = 1,
//    HWM_SYNC_PS_SLOT = 2,
//    HWM_SYNC_WBU_SLOT = 3,
//    HWM_SYNC_MXU_SLOT = 4,
#ifndef HWM_SYNC_WBU_SLOT
#define HWM_SYNC_WBU_SLOT 4
#endif
//    HWM_SYNC_MAX_SLOT = HWM_SYNC_MXU_SLOT + 1,
//    HWM_SYNC_CONTEXT_LAST = HWM_SYNC_MAX_SLOT,
//    HWM_SYNC_UMD_FLUSH_SLOT = 5,
    // Above are slots used by 3D UMD
//    HWM_SYNC_KMD_SLOT = 6,
//    HWM_SYNC_SWITCH_SLOT = 7,
    // Above are stored in context buffer

    //Slot used by video
//    HWM_SYNC_MSVD_SLOT = 8,
#ifndef HWM_SYNC_KMD_SLOT
#define HWM_SYNC_KMD_SLOT 9
#endif
//    HWM_SYNC_VCP_SLOT = 9,
//   HWM_SYNC_ISP_SLOT = 10,
//    HWM_SYNC_MSVD_HI_SLOT = 11,
//    HWM_SYNC_VCP_HI_SLOT = 12,
//    HWM_SYNC_ISP_HI_SLOT = 13,
//} HWM_SYNC_SLOT_E3K;
//#endif

//NOTE: the following define is from CIL3/e3k/kmd/os_interface/include/ZxCommand.h
//#define FENCE_VALUE_RESET               0xFF
//#define FENCE_VALUE_BEGIN_DMA           0x1
//#define FENCE_VALUE_END_DMA             0x2
//#define FENCE_VALUE_RANGESET            0x3
#define FENCE_VALUE_CACHEFLUSH_3D_DMA      0x4
//#define FENCE_VALUE_INIT                0x5
//#define FENCE_VALUE_DUMMY_RB            0x6


typedef struct submit_rb_cmd_part1_e3k
{
    Cmd_Blk_Cmd_Csp_Indicator        pwm_trigger_on;
    Cmd_Blk_Cmd_Csp_Indicator_Dword1 trigger_on_dw;
    unsigned int                     flush_cmd[8];
    //According to HW's request, when slice_mask changed, driver should insert 8dw skip cmd after indicator cmd
    Cmd_Skip                         skip0;
    unsigned int                     dummy[8];

    struct
    {
        Cmd_Set_Register        cmd_Set_Register;
        Reg_Reg_Dvfs_Cfg_En     reg_dvfs_cfg;
        unsigned int            reg_dvfs_cfg_mask;
    };
    union
    {
        struct
        {
            Cmd_Dma         restore_dma;
            unsigned int    restore_dma_addr_l;
            unsigned int    restore_dma_addr_h;
            unsigned int    restore_context_address;
        }restore;
        unsigned int skip[4];
    };
}submit_rb_cmd_part1_e3k_t;

typedef struct submit_rb_cmd_part2_e3k
{
    Cmd_Dma         dma_cmd;
    unsigned int    dma_cmd_addr_l;
    unsigned int    dma_cmd_addr_h;
}submit_rb_cmd_part2_e3k_t;

typedef struct submit_rb_cmd_part3_e3k
{
    Cmd_Dma                      cacheflush_cmd;
    unsigned int                 cacheflush_addr_l;
    unsigned int                 cacheflush_addr_h;

    union
    {
        struct
        {
            Cmd_Dma              save_dma;
            unsigned int         save_dma_addr_l;
            unsigned int         save_dma_addr_h;
            unsigned int         save_context_address;
        }save;
        unsigned int             skip[4];
    };

    Cmd_Fence                    fence;
    unsigned int                 fence_addr_l;
    unsigned int                 fence_addr_h;
    unsigned int                 fence_data_l;
    unsigned int                 fence_data_h;

    //double send the fence for e3k fence lost patch.
    Cmd_Fence                    fence_1;
    unsigned int                 fence_addr_l_1;
    unsigned int                 fence_addr_h_1;
    unsigned int                 fence_data_l_1;
    unsigned int                 fence_data_h_1;

    unsigned int                 skip_2[11];
    Cmd_Blk_Cmd_Csp_Indicator        pwm_trigger_off;
    Cmd_Blk_Cmd_Csp_Indicator_Dword1 trigger_off_dw;
}submit_rb_cmd_part3_e3k_t;


typedef struct submit_rb_cmd_e3k
{
    submit_rb_cmd_part1_e3k_t   rb_cmd_part1;
    submit_rb_cmd_part2_e3k_t  *rb_cmd_part2;
    Cmd_Skip                    skip;
    unsigned int               *dummy;
    submit_rb_cmd_part3_e3k_t   rb_cmd_part3;
}submit_rb_cmd_e3k_t;


typedef struct submit_rb_fence_template_e3k
{
    Cmd_Fence                    fence;
    unsigned int                 fence_addr_l;
    unsigned int                 fence_addr_h;
    unsigned int                 fence_data_l;
    unsigned int                 fence_data_h;
}submit_rb_fence_template_e3k_t;

typedef struct submit_rb_fence_cmd_e3k
{
    Cmd_Blk_Cmd_Csp_Indicator       pwm_trigger;
    Cmd_Blk_Cmd_Csp_Indicator_Dword1 trigger_on_dw;
    submit_rb_fence_template_e3k_t  *rb_cmd_fence;
    Cmd_Skip                        skip;
    unsigned int                    *dummy;
    Cmd_Blk_Cmd_Csp_Indicator       pwm_trigger_off;
    Cmd_Blk_Cmd_Csp_Indicator_Dword1 trigger_off_dw;
}submit_rb_fence_cmd_e3k_t;

typedef struct _engine_e3k
{
    vidsch_mgr_t  *sch_mgr;

    unsigned long long      *fence_buffer_cpu_va;      /* fence buffer addr */
    unsigned long long       fence_buffer_gpu_va;       /* fence buffer addr */

    unsigned long long      *fence_buffer_cpu_va_fake;       /*fence buffer addr, patch for fence isr lost issue*/
    unsigned long long       fence_buffer_gpu_va_fake;       /* fence buffer addr, patch for fence isr lost issue*/

    unsigned int        *ring_buf_virt_addr;
    unsigned long long  ring_buf_phys_addr;

    ring_buffer_t       ring;
    unsigned int       *current_ring_buffer;       //point to current ring buffer head of cpu va

    unsigned int       *last_ring_buffer;
    unsigned int        last_ring_buffer_size;
    unsigned long long  last_send_fence_id;

    unsigned int        last_process_id;
    unsigned long long  last_ttbr_gpu_va;
    void               *share;
    EngineSatus_e3k     dumped_engine_status;           // save status to dump to file //common
}engine_e3k_t;

#define _3DBLT_DATA_RESERVE_SIZE    (1024 * 16)

typedef struct engine_share_e3k
{
    unsigned int          ref_cnt;

    //begin end buffer group
    unsigned long long    begin_end_buffer_gpu_va;
    void                 *begin_end_buffer_cpu_va;
    unsigned int          begin_end_buffer_size;

    unsigned long long    _3d_cacheflush_buffer_gpu_va;
    unsigned long long    _ocl_cacheflush_buffer_gpu_va;

    //begin/end buffer for ogl
    unsigned long long    context_restore_dma_gpu_va;
    unsigned long long    context_restore_dma_gpu_va_cs;
    unsigned long long    context_save_dma_gpu_va;
    unsigned long long    context_buf_gpu_va;//high and low use same hwctx template, first time used

    //begin/end buffer for vulkan
    unsigned long long    context_restore_dma_gpu_va_for_vk;
    unsigned long long    context_save_dma_gpu_va_for_vk;
    unsigned long long    context_buf_gpu_va_for_vk;

    unsigned int  internal_fence_value[8]; //patch video engine issue.

    unsigned long long    _3dblt_data_gpu_va;
    void *                _3dblt_data_cpu_va;

    struct os_mutex     *_3dblt_data_lock;
    unsigned int        _3dblt_data_size;
    void                *_3dblt_data_shadow;

    //CSLx patch need ringbuffer phy addr
    unsigned long long    ring_buffer_phy_addr[RB_NUM];

    BITBLT_REGSETTING_E3K       _2dblt_cmd_e3k;
    FASTCLEAR_REGSETTING_E3K    fast_clear_cmd_e3k;
    submit_rb_cmd_e3k_t   rb_cmd_template;
    struct vidmm_segment_memory *flush_fifo_buffer;

    struct os_pages_memory* dummyfence[2];  // 0 for snoop, 1 for nonsnoop
    unsigned long long dummyfence_phy[2];

    void *ring_buffer_for_hang_backup;
    struct vidmm_segment_memory *ring_buffer_for_hang;
    struct vidmm_segment_memory *transfer_buffer_for_hang;
    struct os_spinlock *lock; //use for pretect internal fence value;
}engine_share_e3k_t;


typedef struct _engine_gfx_e3k
{
    engine_e3k_t          common;
    unsigned int         is_high_engine;

    unsigned long long    context_buffer_gpu_va;
    void                 *context_buffer_cpu_va;

    unsigned long long    context_buffer_gpu_va_for_vk;
    void                 *context_buffer_cpu_va_for_vk;
}engine_gfx_e3k_t;

 typedef engine_e3k_t engine_cs_e3k_t;
 typedef engine_e3k_t engine_vcp_e3k_t;
 typedef engine_e3k_t engine_vpp_e3k_t;

 _inline int EngineRbOffset(DWORD chip, DWORD RbIndex)
{
    unsigned int RegRbOffset = 0;

    if (IS_VCP_ENGINE(chip, RbIndex))
    {
        RegRbOffset = Reg_Vcp_Ring_Buf_Offset + (RbIndex - RB_INDEX_VCP0) * 4;
    }
    else if (IS_VPP_ENGINE(chip, RbIndex))
    {
        if(chip == CHIP_CHX004 && RbIndex == RB_INDEX_VPP){
            RegRbOffset = Reg_Vpp_Ring_Buf_Offset + (RbIndex - RB_INDEX_VPP) * 4;
        }
        if(chip == CHIP_CNE001 && (RbIndex == RB_INDEX_VPP0 || RbIndex == RB_INDEX_VPP1)){
            RegRbOffset = Reg_Vpp_Ring_Buf_Offset_Cne001 + (RbIndex - RB_INDEX_VPP0) * 4;
        }
    }
    else
    {
        RegRbOffset = Reg_Ring_Buf_Offset + RbIndex * 4;
    }
    return RegRbOffset;
}

extern int  engine_submit_command_e3k(gpu_context_t *gpu_context, vidsch_mgr_t *sch_mgr, task_dma_t *dma_task);

extern int  engine_set_root_page_table_e3k(vidsch_mgr_t *sch_mgr, gpu_device_t *device, unsigned int vm_id, task_page_table_set_t *page_table_set_task);
extern int engine_invalidate_tlb_command_e3k(vidsch_mgr_t *sch_mgr, gpu_device_t *device, unsigned int vm_id, task_page_table_set_t *page_table_set_task);

extern int  enginei_enable_ring_buffer_e3k(engine_e3k_t *engine);

extern void enginei_init_gfx_begin_end_commands_e3k(engine_gfx_e3k_t *share);
extern void enginei_init_gfx_submit_rb_cmd_e3k(engine_gfx_e3k_t *engine);

extern int  enginei_init_hardware_context_e3k(engine_gfx_e3k_t *engine);

extern int engine_render_e3k(gpu_context_t *gpu_context, task_dma_t *task_dma);
extern unsigned int *enginei_get_ring_buffer_space_e3k(engine_e3k_t *engine, unsigned int size_uint);
extern void enginei_release_ring_buffer_space_e3k(engine_e3k_t *engine, unsigned size_uint);
int engine_reset_hw_e3k(vidsch_mgr_t *sch_mgr);
extern void vidschi_reset_adapter_e3k(adapter_t *adapter);

extern void vidschi_init_hw_settings_e3k(adapter_t *adapter);
extern void vidsch_dump_hang_info_e3k(vidsch_mgr_t * sch_mgr);
#endif
