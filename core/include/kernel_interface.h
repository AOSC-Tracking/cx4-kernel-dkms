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

#ifndef __KERNEL_INTERFACE_H__
#define __KERNEL_INTERFACE_H__

#include "zx_def.h"
#include "zx_types.h"
#include "core_errno.h"
#include "kernel_import.h"

#define krnl_config_chip krnl_query_info
typedef struct
{
    unsigned int create_device  :1;  /* out */
    unsigned int destroy_device :1;  /* out */
    unsigned int hDevice;            /* in/out */
    void         *filp;              /* in */
}krnl_hint_t;

typedef struct
{
    zx_create_allocation_info_t *info;
    void                        *parent;
    unsigned int                debug_gid;
    struct os_pages_memory *(*get_system_pages)(void *gemObj, void *pdev, unsigned long long size, alloc_pages_flags_t *flags);
    void (*release_system_pages)(void *pdev, struct os_pages_memory *pages_mem);
} krnl_create_allocation_info_t;

struct krnl_adapter_init_info_s
{

    int minor_index;
/* init control  parmetes */
    int zx_pwm_mode ;/* control pwm of zx */
    int zx_dvfs_mode ;/* control dvfs of zx */
    int zx_worker_thread_enable;/* control worker thread on/off */
    int zx_recovery_enable ; /* enable recovery when hw hang */
    int zx_hang_dump;/*0-disable, 1-pre hang, 2-post hang, 3-duplicate hang */
    int zx_run_on_qt; /* control wether run on QT */
    int zx_flag_buffer_verify ;/*0 - disable, 1 - enable */
    int zx_vesa_tempbuffer_enable ; /* control wether reserve memory during boot */

    int miu_channel_size;//0/1/2 for 256B/512B/1kb Swizzle
    //gpc/slice setting
    unsigned int chip_slice_mask;//// CHIP_SLICE_MASK own 12 bits(should be set 0x001 ~ 0xfff), driver use this to set the Slice_Mask which HW used.

    int zx_local_size_g;
    int zx_local_size_m;
    int zx_pcie_size_g;
    int zx_pcie_size_m;
    int debugfs_mask;
    int nonsnoop_patch;

    int page_table_seg; /* 0-local_first, 1-force_local 2-force_pcie */

    int zx_set_high_eclk;
};
typedef struct krnl_adapter_init_info_s krnl_adapter_init_info_t;

typedef struct zx_test_mem_para
{
    int snoop;
    int read;
    unsigned long long  phys_addr; // test memory physical address
}zx_test_mem_para_t;

typedef  struct
{
    struct  //get from adapter
    {
        unsigned int  ven_dev;
        unsigned int  family_id;
        unsigned int  generic_id;
        unsigned short  sub_sys_vendor_id;
        unsigned short  sub_sys_id;
        unsigned int  chip_id;
        unsigned char*   mmio;
        unsigned int  mmio_size;
        unsigned int  primary;
        unsigned long long fb_bus_addr;
        unsigned int  fb_total_size;
        unsigned int  patch_fence_intr_lost:1;
        unsigned int  init_render:1;
        union
        {
            unsigned int  adp_flags;
            struct
            {
                unsigned int  run_on_qt:1;
                unsigned int  run_on_hypervisor:1;
                unsigned int  Reserved:30;
            };
        };
    };
    struct   //set to adapter
    {
        unsigned long long low_top_addr;
        unsigned int ta_enable;
        unsigned int snoop_only;
        unsigned int chan_num;
        unsigned int avai_mem_size_mb; //in Mega Bytes
        unsigned int total_mem_size_mb; //in Mega Bytes
        unsigned int vpp_efuse_value;
    };
}adapter_info_t;

struct batch_parser_s
{
    unsigned int        dma_cnt;       //number of dma
    unsigned long long *dma_gpu_va;   //array of dma gpu virtual address
    unsigned int       *dma_length;   //array of dma length

    unsigned int        primary_cnt; // number of primary allocation array
    void                **primary_list;
    unsigned int        *primary_write_op_list;// array of primary allocation write operation op

    unsigned int        resident_cnt;
    void                **resident_list;

    unsigned long long ctx_buf_va;
    union {
        struct
        {
            unsigned int null_rendering     : 1;  //skip render command, only do fence operation
            unsigned int need_signal        : 1;  //for some case which seperately call render and insert fence,
                                                  //that insert fence will generate fence dma and signal by hw, this flag will     be false
                                                  //for other case, no need to generate dma,
                                                  //just do force signal in task_dma_t->completed_fence_cb
            unsigned int initialize_context : 1; // whether the ctx_buf_va was initialized.
            unsigned int _3dblt_cmd         : 1;
            unsigned int _2dcmd             : 1;
            unsigned int clcs_only          : 1;
            unsigned int contain_lpdp_cmd   : 1;

            unsigned int dump_rb            : 1;    // for debug, dump ring buffer when kickoff
            unsigned int dump_dma           : 1;    // for debug, dump dma when kickoff
            unsigned int ignore_dma         : 1;    // generate ring cmd, but skip dma, difference from null_rendering
            unsigned int reserved           :22;

        }submit_flag;
        unsigned int uflag;
    };
    unsigned int       wait_fence_cnt;      //number of wait fence
    unsigned int       *wait_fence;         //array of wait fence
    unsigned int       signal_fence_cnt;    //number of signal fence
    unsigned int       *signal_fence;       //array of signal fence
    void               *wait_dma_fence;
    void               *dma_fence;

    void               *os_priv;            // os private data, core should not touch it
    unsigned int       bos_cnt;
    void               *bos;                // gem objs array
    void               (*release)(struct batch_parser_s *);
};

typedef struct batch_parser_s batch_parser_t;

typedef struct
{
    void            *device;
    unsigned int    num_allocations;
    unsigned int    *priority_list;
    void            **allocation_list;
} krnl_set_allocation_priority_t;

typedef struct
{
    void            *device;
    unsigned long long base;
    unsigned long long size;
} krnl_free_gpu_va_t;

typedef struct
{
    void                *device;
    void                *allocation;
    unsigned long long  size;
    unsigned long long  base_addr;
    unsigned long long  min_addr;
    unsigned long long  max_addr;
    unsigned long long  offset;
    unsigned long long  driver_protection;
    zx_map_gpu_flag_t   protection_flag;

    unsigned long long  gpu_va; // out
} krnl_map_gpu_va_t;

typedef struct
{
    RESERVED_RESOURCE_OPERATION_TYPE op_type;
    union
    {
        struct
        {
            unsigned long long tile_gpu_va;
            unsigned long long tile_size;//tile resource
            void               *allocation;
            unsigned long long allocation_offset;//in byte
            unsigned long long allocation_size;//in byte
        }map;
        struct
        {
           unsigned long long tile_gpu_va;
           unsigned long long tile_size;
        }unmap;
        struct
        {
            unsigned long long src_tile_gpu_va;
            unsigned long long tile_size;
            unsigned long long dst_tile_gpu_va;
        }copy;
    };
} krnl_update_sparse_mapping_info_t;

typedef struct
{
    void         *context;
    unsigned int fence_obj;
    unsigned int num_operation;
    unsigned long long fence_value;
    krnl_update_sparse_mapping_info_t *ops;
    zx_update_gpu_va_flags_t flags;
} krnl_update_gpu_va_t;


typedef struct
{
    void                *device;
    unsigned long long  base;
    unsigned long long  min;
    unsigned long long  max;
    unsigned long long  size;//size in byte
    zx_reserve_gpu_va_type_t type;
    unsigned long long  driver_protection;
    zx_ptr64_t          gpu_va;//out
} krnl_reserve_gpu_va_t;

typedef struct
{
    struct gpu_device *device;
    struct gpu_context *context;
    zx_cil2_misc_t *zx_misc;
} krnl_cil2_misc_t;


typedef struct
{
    unsigned int        type;
    void                *device;
    void                *allocation;
    union {
        unsigned int v_uint;
        unsigned long long v_long;
        void *v_ptr;
    };
} krnl_test_t;

typedef struct
{
    void* (*pre_init_adapter)(void *pdev, krnl_adapter_init_info_t *info, krnl_import_func_list_t *import);
    void  (*init_adapter)(void *adp, int reserved_vmem, void *disp_info);
    void  (*deinit_adapter)(void *data);
    void  (*get_adapter_info)(void* adp, adapter_info_t*  adapter_info);
    void  (*update_adapter_info)(void* adp, adapter_info_t*  adapter_info);
    unsigned int (*get_lda_handle)(unsigned int index);
    void (*dump_resource)(struct os_printer *p, void *data, int index, int iga_index);
    void (*debugfs_dump)(struct os_printer *p, void *data, int type, void* arg);
    void (*wait_chip_idle)(void *adapter);
    void (*save_state)(void *adapter, int need_save_memory);
    int  (*restore_state)(void *adapter);
    int (*get_map_allocation_info)(void *data, void *allocation, zx_map_argu_t *map);
    void (*cabc_set)(void *data, unsigned int cabc);
    int (*query_info)(void* data, zx_query_info_t *info);
    void *(*create_device)(void *data, void *filp);
    void (*destroy_device)(void *data, void *dev);
    void (*update_device_info)(void *data, void *dev, unsigned long pid, unsigned long tid, const char *pname);
    void*  (*create_allocation)(void *data, krnl_create_allocation_info_t *create_data);
    void (*destroy_allocation)(void *data, void *allocation, zx_destroy_flag_t flags);
    int (*prepare_and_mark_unpagable)(void *data, void *_allocation);
    void (*mark_pagable)(void *data, void *_allocation);
    int (*begin_perf_event)(void *data, zx_begin_perf_event_t *begin_perf_event);
    int (*end_perf_event)(void *data, zx_end_perf_event_t *end_perf_event);
    int (*get_perf_event)(void *data, zx_get_perf_event_t *get_event);
    int (*send_perf_event)(void *data, zx_perf_event_t *perf_event);
    int (*get_perf_status)(void *data, zx_perf_status_t *perf_status);
    int (*begin_miu_dump_perf_event)(void *data, zx_begin_miu_dump_perf_event_t *begin_miu_perf_event);
    int (*end_miu_dump_perf_event)(void *data, zx_end_miu_dump_perf_event_t *end_miu_perf_event);
    int (*set_miu_reg_list_perf_event)(void *data, zx_miu_reg_list_perf_event_t *miu_reg_list);
    int (*get_miu_dump_perf_event)(void *data, zx_get_miu_dump_perf_event_t *get_miu_dump);
    int (*direct_get_miu_dump_perf_event)(void *data, zx_direct_get_miu_dump_perf_event_t *direct_get_miu);
    void* (*create_context)(void *data, void *dev, zx_create_context_t *create_context);
    int (*destroy_context)(void *data, void *device, void *ctx);
    int (*render)(void *data, void *gpu_context, batch_parser_t *batch);
    void (*wakeup_worker_thread)(void *data, int engine_index);
    int (*is_fence_back)(void *data, unsigned char engine_index, unsigned long long fence_id);

    int (*begin_end_isp_dvfs)(void *data, unsigned int on);
    int (*add_hw_ctx_buf)(void *data, void *device, void *ctx, unsigned int ctx_buf_idx);
    int (*rm_hw_ctx_buf)(void *data, void *device, void *ctx, unsigned int ctx_buf_idx);
    int  (*notify_interrupt)(void *data, unsigned int interrupt_event);
    int  (*notify_power_tuning)(void *data, unsigned int interrupt_event);
    int  (*notify_page_fault)(void *data);
    void (*perf_event_add_isr_event)(void *data, zx_perf_event_t *perf_event);
    void (*perf_event_add_event)(void *data, zx_perf_event_t *perf_event);

    void (*enable_trace_events)(void);

    int  (*set_allocation_priority)(void *data, krnl_set_allocation_priority_t *prio);
    int  (*map_gpu_virtual_address)(void *data, krnl_map_gpu_va_t *map);
    int  (*free_gpu_virtual_address)(void *data, krnl_free_gpu_va_t *free);
    int  (*reserve_gpu_virtual_address)(void *data, krnl_reserve_gpu_va_t *reserve);
    int  (*update_gpu_virtual_address)(void *data, krnl_update_gpu_va_t *update);
    void (*update_global_mapping)(void *data, void *alloc, unsigned long long *gpu_va);
    int (*cil2_misc)(void *data, krnl_cil2_misc_t *misc, void* allocation);
    int (*test)(void *data, krnl_test_t *arg);
    void (*dump_hang_info_flag)(void *data, int flag);
    void (*ctl_flags_set)(void *data,unsigned int num,unsigned int mask,unsigned int value);
    int (*hwq_process_vsync_event)(void *data, unsigned long long time);
} core_interface_t;


core_interface_t *krnl_get_core_interface(void);

extern core_interface_t *zx_core_interface;

#endif



