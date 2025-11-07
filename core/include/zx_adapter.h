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

#ifndef __ZX_ADAPTER_H__
#define __ZX_ADAPTER_H__

#include "core_errno.h"
#include "util.h"
#include "list.h"
#include "core_import.h"
#include "zx_chip_id.h"
#include "zx_types.h"
#include "zx_def.h"

//#define ENABLE_UNSNOOPABLE_PCIE 0
#if defined(__aarch64__)
//arm64 only can pass snoop path for diffcult to flush page cache now, will remove this if all is ready
#define ENABLE_UNSNOOPABLE_WROKAROUND    1
#else
#define ENABLE_UNSNOOPABLE_WROKAROUND    1
#endif

#define ACPI_TEMP_CODE 0
#define NEW_CBIOS_FUNC_SUPPORT 1

#define VIDMM_TRACE_ENABLE        0
#define VIDSCH_TRACE_ENABLE       0
#define DISPMGR_TRACE_ENABLE      0
#define MISC_DEBUG_INFO_ENABLE    0
#define SEGMENT_TRACE_ENABLE      0
#define VIDSCH_SYNC_TRACE_ENABLE  0
#define DISPLAY_TRACE_ENABLE      0
#define VIDSCH_POWER_DEBUG        0

#define DEBUG_TEST                0
#define DEBUG_HIGH_4G_MEM         0
#define HIGH_4G_MEM_RESERVE_SIZE  (0x10000000) //64M
#define TEST_VIDEO_RING_BUFFER    0
#define TEST_POWER_MANAGE         0
#define  VIDMM_VM_DEBUG           0

#define MAX_POWER_MASK_NUM        128


#if VIDMM_TRACE_ENABLE
#define vidmm_trace    zx_info
#else
#define vidmm_trace(args...)
#endif

#if VIDSCH_TRACE_ENABLE
#define vidsch_trace    zx_info
#else
#define vidsch_trace(args...)
#endif

#if SEGMENT_TRACE_ENABLE
#define segment_trace   zx_info
#else
#define segment_trace(args...)
#endif

#if VIDSCH_POWER_DEBUG
#define power_trace zx_info
#else
#define power_trace(args...)
#endif

#if VIDMM_VM_DEBUG
#define vm_trace zx_info
#else
#define vm_trace(args...)
#endif

#define VCP0_INDEX      0
#define VCP1_INDEX      1
#define VCP2_INDEX      2
#define VCP_INDEX_COUNT 3
#define VCP_INFO_COUNT  33
#define MAX_VIDEO_SEQ_INDEX_ADD1 128
#define MIN_VIDEO_SEQ_INDEX      0
//it's from RB_INDEX_VCP0
#define RB_INDEX_VIDEO_START     7


#define IS_VIDEO_ENGINE(__chip_id, __engine_idx) \
    ((__engine_idx) == RB_INDEX_VCP0 || (__engine_idx) == RB_INDEX_VCP1 || (__engine_idx) == RB_INDEX_VCP2 || \
     (__engine_idx) == RB_INDEX_VPP || (__engine_idx) == RB_INDEX_VPP0 || (__engine_idx) == RB_INDEX_VPP1) \


#define IS_VCP_ENGINE(__chip_id, __engine_idx) \
    ((__engine_idx) == RB_INDEX_VCP0 || (__engine_idx) == RB_INDEX_VCP1 || \
     ((__chip_id) == CHIP_CNE001 && (__engine_idx) == RB_INDEX_VCP2)) \


#define IS_VPP_ENGINE(__chip_id, __engine_idx) \
    (((__chip_id) == CHIP_CHX004 && (__engine_idx) == RB_INDEX_VPP) || \
     ((__chip_id) == CHIP_CNE001 && ((__engine_idx) == RB_INDEX_VPP0 || (__engine_idx) == RB_INDEX_VPP1))) \


typedef struct engine_index_info
{
    unsigned int    node_ordinal;
    unsigned int    engine_affinity;
} engine_index_info_t;

/* our hw feature put here*/
typedef struct hw_caps
{
    /* hw general features*/
    unsigned int fence_interrupt_enable:    1;
    unsigned int support_snooping   :1;
    unsigned int address_mode_64bit :1;
    unsigned int hw_patch_enable    :1;

    unsigned int video_only         :1;   // fpga option
    unsigned int gfx_only           :1;   // fpga option
    unsigned int local_only         :1;   // only has local memory
    unsigned int snoop_only         :1;   //without non-snoopy path
    unsigned int system_only        :1;   //hw_null option
    unsigned int page_64k_enable    :1;
    unsigned int page_4k_enable     :1;
    unsigned int secure_range_enable:1;
    unsigned int anti_hang_enable   :1; //anti hang

    unsigned int ta_enable          :1;
    unsigned int share_pcie_enable  :1;
    unsigned int legacy_offset_enable:1;
    unsigned int dvfs_enable         :1;
    unsigned int zx_set_high_eclk    :1;
    unsigned int nonsnoop_patch_enable :1;
    unsigned int reserved           :5;

    int             miu_channel_num;  //should be 1~3
    int             miu_channel_size;
    unsigned int    chip_slice_mask;
}hw_caps_t;

/* our system feature put here*/
typedef struct sys_caps
{
    unsigned int bus_pcie_inuse     :1;
    unsigned int bus_ahb_inuse      :1;
    unsigned int secure_on          :1;
    unsigned int iommu_enabled      :1;
    unsigned int fast_clear_bl_slot :1;
    unsigned int reserved           :27;
}sys_caps_t;

/* our power feature put here*/
typedef struct power_caps
{
    union
    {
        unsigned int pwm_auto;
        struct
        {
            unsigned int gfx_cg_auto    :1;
            unsigned int vcp0_cg_auto    :1;
            unsigned int vcp1_cg_auto    :1;
            unsigned int vcp2_cg_auto  :1;
            unsigned int vpp0_cg_auto    :1;
            unsigned int vpp1_cg_auto    :1;
            unsigned int gfx_pg_auto    :1;
            unsigned int vcp0_pg_auto    :1;
            unsigned int vcp1_pg_auto    :1;
            unsigned int vcp2_pg_auto  :1;
            unsigned int vpp0_pg_auto    :1;
            unsigned int vpp1_pg_auto    :1;
            unsigned int auto_resv      :20;
        };
    };
    union
    {
        unsigned int pwm_manual;
        struct
        {
            unsigned int gfx_cg_manual  :1;
            unsigned int vcp0_cg_manual  :1;
            unsigned int vcp1_cg_manual  :1;
            unsigned int vcp2_cg_manual  :1;
            unsigned int vpp0_cg_manual  :1;
            unsigned int vpp1_cg_manual  :1;
            unsigned int gfx_pg_manual  :1;
            unsigned int vcp0_pg_manual  :1;
            unsigned int vcp1_pg_manual  :1;
            unsigned int vcp2_pg_manual  :1;
            unsigned int vpp0_pg_manual  :1;
            unsigned int vpp1_pg_manual  :1;
            unsigned int manual_rsv     :20;
        };
    };
    unsigned int pwm_mode;
    unsigned int be_mode;
    union
    {
        unsigned int dvfs_auto;
        struct
        {
            unsigned int gfx_dvfs_auto  :1;
            unsigned int vcp_dvfs_auto  :1;
            unsigned int vpp_dvfs_auto  :1;
            unsigned int gfx_mem_dvfs   :1;
            unsigned int vcp_mem_dvfs   :1;
            unsigned int vpp_mem_dvfs   :1;
            unsigned int gfx_output_mode:2;
            unsigned int vcp_output_mode:2;
            unsigned int vpp_output_mode:2;
            unsigned int dvfs_auto_rsv  :20;
        };
    };
    union
    {
        unsigned int dvfs_force;
        struct
        {
            unsigned int gfx_dvfs_force :1;
            unsigned int vcp_dvfs_force :1;
            unsigned int vpp_dvfs_force :1;
            unsigned int dvfs_force_rsv :29;
        };
    };
    unsigned int dvfs_interrupt;
    union
    {
        unsigned int slice_balance;
        struct
        {
            unsigned int slice_balance_auto  :1;//auto mode
            unsigned int slice_balance_force :1;//simulate auto mode
            unsigned int slice_force_enable  :1;//force mode
            unsigned int slice_force_value   :1;//force mode value
            unsigned int slice_rsv           :28;
        };
    };
}power_caps_t;


/* adapter control flag put here */
typedef struct ctl_flags
{
    unsigned int  paging_enable         :1;
    unsigned int  swap_enable           :1;  /* swap pages to shmem */
    unsigned int  split_enable          :1;
    unsigned int  worker_thread_enable  :1;
    unsigned int  recovery_enable       :1;  /* reset hw if hang, must disable it if debug hang */
    unsigned int  manual_flush_cache    :1;  /* if this flag set, default use wb for none snoop segment, and flush cache before GPU use manual,
                                                for ARM use since arm snoop performance not good */
    unsigned int  dump_hang_info_to_file :1;
    unsigned int  hang_dump              :2;
    unsigned int  perf_event_enable      :1;
    unsigned int  miu_counter_enable     :1;
    unsigned int  flag_buffer_verify     :1;
    unsigned int  local_for_display_only :1;
    unsigned int  submit_to_queue        :1; /* if true, submit task to queue directly. only used when worker thread enable!!! */
    unsigned int  run_on_qt              :1;
    unsigned int  run_in_guest           :1;
    unsigned int  vesa_tempbuffer_enable :1;
    unsigned int  share_space_cover_local :1;//if true, indicates the shared space cover the local, the shared va directly get from the pa.
    unsigned int  cpu_disable_c2cp_patch :1;
    unsigned int  page_table_pcie_enable :1;
    unsigned int  page_table_local_enable :1;
    unsigned int  hwq_event_enable       :1;
    unsigned int  force_cpu_cm_save_restore :1;
    unsigned int  reserved               :9;
}ctl_flags_t;

#define PATCH_E2UMA_FENCE_ID_LOST   (1 << 0)
#define PATCH_FENCE_INTERRUPT_LOST  (1 << 1)
#define PATCH_E2UMA_HW66 (1<<2)

typedef struct
{
    int EnableClockGating;
    int EnablePowerGating;
    int DonotInitPowerSet;
}pwm_level_t;

typedef struct{
    unsigned long long dirty_addr;
    unsigned long long dirty_mask;
    unsigned int dirty;
}gart_dirty_t;

typedef struct
{
    ctl_flags_t                 ctl_flags;
    sys_caps_t                  sys_caps;
    hw_caps_t                   hw_caps;
    pwm_level_t                 pwm_level;
    power_caps_t                pm_caps;
    unsigned int                hw_patch_mask0;
    unsigned int                hw_patch_mask1;

    os_device_t                 os_device;

    unsigned char               *mmio;
    unsigned int                *bci_base;
    zx_cpu_vm_area_t           *mmio_vma;
    struct os_pages_memory      *mmio_mem;

    bus_config_t                bus_config;
    unsigned int                primary;
    unsigned int                index;

    unsigned int                family_id;
    unsigned int                chip_id;
    unsigned int                generic_id;
    unsigned short              link_width;

    unsigned long long          physical_bus_base_length[2];
    unsigned long long          vidmm_bus_addr;

    unsigned int                os_page_size;
    unsigned int                os_page_shift;

    /* total memory size in gpu, include reserved */
    /* when hang dump is on, this is half of orignal value */
    unsigned long long          total_local_memory_size;
    unsigned long long          visible_local_memory_size;//cpu visible local memory.

    unsigned char               context_buffer_inuse[0xFFFF];

    struct os_mutex             *paging_lock;  /* lock for paging */

    struct os_mutex             *device_list_lock;  /* lock for paging */

    struct os_atomic64          *device_uuid;

    struct os_atomic64          *context_uuid;

    struct os_atomic64          *task_uuid;

    struct os_spinlock          *lock;   /* adapter lock for device list and sync_obj list */

    struct list_head            device_list;   /* device create in this adapter*/

    struct _vidmm_mgr           *mm_mgr;

    int                         paging_engine_index;

    struct vidsch_fence_buffer  *fence_buf;
    struct vidsch_mgr           *sch_mgr[MAX_ENGINE_COUNT];
    struct vidsch_global        *sch_global;
    int                         active_engine_count;
    engine_index_info_t         engine_index_table[MAX_ENGINE_COUNT];

    struct os_mutex             *hw_reset_lock;
    unsigned int                 hw_reset_times;

    struct _perf_event_mgr      *perf_event_mgr;
    struct _hwq_event_mgr       *hwq_event_mgr;
    unsigned int usage_3d;
    unsigned int usage_vcp;
    unsigned int usage_vpp;

    unsigned long long          shared_gpu_va_size;
    unsigned long long          shared_gpu_va_offset;
    unsigned long long          private_size_of_cne001;

    unsigned long long          share_pcie_level3_gpu_va;    // share to diu

    unsigned long long          Real_vram_size;     // Total Video Mem
    unsigned long long          UnAval_vram_size;   // for 3 miu, need dig out some mem from end of vram.
    struct os_mutex             *hang_dump_lock;    // for elite

    void                        *private_data;

    void                        *flag_buffer_virt_addr; // for elite flag buffer debug

    struct os_mutex             *gart_table_lock;

    int                         boot_noise_stop;
    void                        *vesafb_tempbuffer;
    void                        *mem_unavailable_for_3miu;   //AVAILABLE

    unsigned long long          low_top_address;

    int                         vpp_efuse_value;
    int                         vcp_index_cnt[VCP_INDEX_COUNT];
    zx_vcp_info                 vcp_info[VCP_INFO_COUNT];
    unsigned  char              bVideoSeqIndex[MAX_VIDEO_SEQ_INDEX_ADD1];
    int                         start_index;
    int                         end_index;

    void                       *reserved_vma;
    void                       *reserved_device;//for kmd global use
    void                       *reserved_context;//for kmd global use

    void                       *private_vma;
    unsigned int               private_vm_id;

    int log_fd;
    gart_dirty_t               gart_dirty;

    void                       *reserv_uaval;
    void                       *disp_info;
    int                        resume_age;  // ++ when resume
    int                display_debugbus_flag;
    unsigned long long RangeBufferGpuVa;
    int                        in_acpi_stage;
} adapter_t;

#endif

