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
#ifndef __VIDSCH_DEBUG_HANG_E3K_H__
#define __VIDSCH_DEBUG_HANG_E3K_H__

#include "vidsch_engine_e3k.h"
#include "vidsch_dump_image_e3k.h"

typedef enum DEBUG_BUS_SCOPE {
    WHOLE_SCOPE_DEBUG_BUS  = 0,
    CHIP1_SCOPE_DEBUG_BUS = 1,
    CHIP2_SCOPE_DEBUG_BUS = 2,
    CHIP3_SCOPE_DEBUG_BUS = 3,
    CHIP4_SCOPE_DEBUG_BUS = 4,
}DEBUG_BUS_SCOPE;

typedef struct debug_bus_info
{
    char             group_name[32];
    DEBUG_BUS_SCOPE  scope;
    unsigned int     group_start_offset;
    unsigned int     group_end_offset;
}debug_bus_info;

static const debug_bus_info  debug_bus_info_e3k[] =
{
    "ZXVD0 group"     ,       CHIP1_SCOPE_DEBUG_BUS,    0x2000,   0x201F,
    "CMU group"       ,       CHIP1_SCOPE_DEBUG_BUS,    0x2800,   0x29FF,
    "MIU2 group"      ,       CHIP1_SCOPE_DEBUG_BUS,    0x3000,   0x301F,
    "BIU0 group"      ,       CHIP1_SCOPE_DEBUG_BUS,    0x3800,   0x3809,
    "BIU1 group"      ,       CHIP1_SCOPE_DEBUG_BUS,    0x3C00,   0x3F7F,

    "ZXVD1 group"     ,       CHIP2_SCOPE_DEBUG_BUS,    0x2000,   0x201F,
    "MXUB group"      ,       CHIP2_SCOPE_DEBUG_BUS,    0x2800,   0x28FF,
    "MIU1 group"      ,       CHIP2_SCOPE_DEBUG_BUS,    0x3000,   0x301F,
    "L2 group"        ,       CHIP2_SCOPE_DEBUG_BUS,    0x3800,   0x387F,

    "VPP group"       ,       CHIP3_SCOPE_DEBUG_BUS,    0x2000,   0x207F,
    "MIU0 group"      ,       CHIP3_SCOPE_DEBUG_BUS,    0x2800,   0x281F,
    "DIU group"       ,       CHIP3_SCOPE_DEBUG_BUS,    0x3000,   0x3EE6,

    "SLICE0 EUC group",       WHOLE_SCOPE_DEBUG_BUS,    0x000,    0x0FF,
    "SLICE0 PEA group",       WHOLE_SCOPE_DEBUG_BUS,    0x100,    0x17F,
    "SLICE0 PEB group",       WHOLE_SCOPE_DEBUG_BUS,    0x180,    0x1FF,
    "SLICE0 TU  group",       WHOLE_SCOPE_DEBUG_BUS,    0x200,    0x23F,
    "SLICE0 FFU group",       WHOLE_SCOPE_DEBUG_BUS,    0x280,    0x2FF,

    "SLICE1 EUC group",       WHOLE_SCOPE_DEBUG_BUS,    0x400,    0x4FF,
    "SLICE1 PEA group",       WHOLE_SCOPE_DEBUG_BUS,    0x500,    0x57F,
    "SLICE1 PEB group",       WHOLE_SCOPE_DEBUG_BUS,    0x580,    0x5FF,
    "SLICE1 TU  group",       WHOLE_SCOPE_DEBUG_BUS,    0x600,    0x63F,
    "SLICE1 FFU group",       WHOLE_SCOPE_DEBUG_BUS,    0x680,    0x6FF,

    "SLICE2 EUC group",       WHOLE_SCOPE_DEBUG_BUS,    0x800,    0x8FF,
    "SLICE2 PEA group",       WHOLE_SCOPE_DEBUG_BUS,    0x900,    0x97F,
    "SLICE2 PEB group",       WHOLE_SCOPE_DEBUG_BUS,    0x980,    0x9FF,
    "SLICE2 TU  group",       WHOLE_SCOPE_DEBUG_BUS,    0xA00,    0xA3F,
    "SLICE2 FFU group",       WHOLE_SCOPE_DEBUG_BUS,    0xA80,    0xAFF,

    "SLICE3 EUC group",       WHOLE_SCOPE_DEBUG_BUS,    0xC00,    0xCFF,
    "SLICE3 PEA group",       WHOLE_SCOPE_DEBUG_BUS,    0xD00,    0xD7F,
    "SLICE3 PEB group",       WHOLE_SCOPE_DEBUG_BUS,    0xD80,    0xDFF,
    "SLICE3 TU  group",       WHOLE_SCOPE_DEBUG_BUS,    0xE00,    0xE3F,
    "SLICE3 FFU group",       WHOLE_SCOPE_DEBUG_BUS,    0xE80,    0xEFF,

    "CENTRAL SPTFE group",    WHOLE_SCOPE_DEBUG_BUS,    0x1000,    0x103F,
    "CENTRAL SGTBE group",    WHOLE_SCOPE_DEBUG_BUS,    0x1800,    0x183F,

};

static const debug_bus_info  debug_bus_info_CHX004[] =
{
    "BPM group"       ,       CHIP1_SCOPE_DEBUG_BUS,    0x3800,   0x3FFF,
    "MXU1 group"      ,       CHIP1_SCOPE_DEBUG_BUS,    0x3A00,   0x3A30,
    "DIU group"       ,       CHIP2_SCOPE_DEBUG_BUS,    0x0000,   0x0FFF,
    "ZXVD0 group"     ,       CHIP3_SCOPE_DEBUG_BUS,    0x2000,   0x27FF,
    "CMU group"       ,       CHIP3_SCOPE_DEBUG_BUS,    0x2800,   0x2990,
    "VPP group"       ,       CHIP3_SCOPE_DEBUG_BUS,    0x3000,   0x37FF,

    "ZXVD1 group"     ,       CHIP4_SCOPE_DEBUG_BUS,    0x2000,   0x27FF,
    "MXUB group"      ,       CHIP4_SCOPE_DEBUG_BUS,    0x2800,   0x28C8,
    "POOL group"      ,       CHIP4_SCOPE_DEBUG_BUS,    0x3000,   0x30FA,
    "L2 group"        ,       CHIP4_SCOPE_DEBUG_BUS,    0x3800,   0x383C,

    "SLICE0 EUC group",       CHIP3_SCOPE_DEBUG_BUS,    0x000,    0x0DC,
    "SLICE0 PEA group",       CHIP3_SCOPE_DEBUG_BUS,    0x100,    0x16E,
    "SLICE0 PEB group",       CHIP3_SCOPE_DEBUG_BUS,    0x180,    0x1EE,
    "SLICE0 TU  group",       CHIP3_SCOPE_DEBUG_BUS,    0x200,    0x23F,
    "SLICE0 FFU group",       CHIP3_SCOPE_DEBUG_BUS,    0x280,    0x2EE,

    "SLICE1 EUC group",       CHIP3_SCOPE_DEBUG_BUS,    0x400,    0x4DC,
    "SLICE1 PEA group",       CHIP3_SCOPE_DEBUG_BUS,    0x500,    0x56E,
    "SLICE1 PEB group",       CHIP3_SCOPE_DEBUG_BUS,    0x580,    0x5EE,
    "SLICE1 TU  group",       CHIP3_SCOPE_DEBUG_BUS,    0x600,    0x63F,
    "SLICE1 FFU group",       CHIP3_SCOPE_DEBUG_BUS,    0x680,    0x6EE,

    "GPC CENTRAL SPTFE group",       CHIP3_SCOPE_DEBUG_BUS,    0x1000,    0x103F,
    "GPC CENTRAL SGTBE group",       CHIP3_SCOPE_DEBUG_BUS,    0x1800,    0x183F,
};
static const debug_bus_info  debug_bus_info_CNE001[] =
{
    "BPM group"       ,       CHIP1_SCOPE_DEBUG_BUS,    0x3800,   0x3AFF,
    "DIU group"       ,       CHIP1_SCOPE_DEBUG_BUS,    0x8000,   0x9FFF,

    "VCP0 group"      ,       CHIP2_SCOPE_DEBUG_BUS,    0xA000,   0xA3FF,
    "VDP0 group"      ,       CHIP2_SCOPE_DEBUG_BUS,    0xC000,   0xC3FF,
    "VDP1 group"      ,       CHIP2_SCOPE_DEBUG_BUS,    0xC400,   0xC7FF,
    "VDP2 group"      ,       CHIP2_SCOPE_DEBUG_BUS,    0xC800,   0xCBFF,
    "VPP0 group"      ,       CHIP2_SCOPE_DEBUG_BUS,    0xE000,   0xE3FF,
    "VPP1 group"      ,       CHIP2_SCOPE_DEBUG_BUS,    0xE400,   0xE4FF,

    "CMU group"       ,       CHIP2_SCOPE_DEBUG_BUS,    0x8000,   0x81FF,
    "MXUB group"      ,       CHIP2_SCOPE_DEBUG_BUS,    0x8400,   0x84FF,
    "POOL group"      ,       CHIP2_SCOPE_DEBUG_BUS,    0x8200,   0x82FF,
    "L2 group"        ,       CHIP2_SCOPE_DEBUG_BUS,    0x8300,   0x837F,

    "SLICE0 EUC group",       CHIP2_SCOPE_DEBUG_BUS,    0x000,    0x0FF,
    "SLICE0 PEA group",       CHIP2_SCOPE_DEBUG_BUS,    0x100,    0x17F,
    "SLICE0 PEB group",       CHIP2_SCOPE_DEBUG_BUS,    0x180,    0x1FF,
    "SLICE0 TU  group",       CHIP2_SCOPE_DEBUG_BUS,    0x200,    0x27F,
    "SLICE0 FFU group",       CHIP2_SCOPE_DEBUG_BUS,    0x280,    0x2FF,

    "SLICE1 EUC group",       CHIP2_SCOPE_DEBUG_BUS,    0x400,    0x4FF,
    "SLICE1 PEA group",       CHIP2_SCOPE_DEBUG_BUS,    0x500,    0x57F,
    "SLICE1 PEB group",       CHIP2_SCOPE_DEBUG_BUS,    0x580,    0x5FF,
    "SLICE1 TU  group",       CHIP2_SCOPE_DEBUG_BUS,    0x600,    0x67F,
    "SLICE1 FFU group",       CHIP2_SCOPE_DEBUG_BUS,    0x680,    0x6FF,

    "SPTFE group",            CHIP2_SCOPE_DEBUG_BUS,    0x1000,    0x107F,
    "SGTBE group",            CHIP2_SCOPE_DEBUG_BUS,    0x1080,    0x10FF,
};
#define SAVE_BEFORE_POSTHANG                3 //for posthangdump:save ttbr
#define SAVE_AFTER_POSTHANG                 4 //for posthangdump:save status
#define SAVE_RING_BUFFER                    5 //save ringbuffer only.


// Debug hang fence offset
#define DH_FENCE_OFFSET         (0x40)
#define DH_FENCE_VALUE          (0x12345678)
#define DH_XBUFFER_SIZE         (0x10000)
#define DUPLICATE_TIME_OUT      (10) //10s

//multi dma number
#define     MAX_HANG_DUMP_DATA_POOL_NUMBER         16
#define     HANG_DUMP_RING_BUFFER_OFFSET           1
#define     HANG_DUMP_DH_COMMON_INFO_OFFSET        2

typedef struct dh_rb_info_e3k
{
    unsigned int    last_process_id;
    unsigned int    last_rb_size;
    unsigned int    last_rb_index;
    unsigned long long last_ttbr_gpu_va;
    unsigned long long fence_gpu_va;
    unsigned long long fence_value;
}dh_rb_info_e3k;

typedef struct dh_common_info_e3k
{
    unsigned int    current_hang_dump_index;
    unsigned int    hang_dump_counter;
    unsigned long long last_return_fence_id;
    unsigned long long share_ttbr_gpu_va;
    dh_rb_info_e3k  rb_info[MAX_HANG_DUMP_DATA_POOL_NUMBER];
    EngineSatus_e3k hang_status;
    unsigned int    flush_fifo[64];
}dh_common_info_e3k;

typedef struct submit_info
{
    unsigned long long ring_buffer_gpu_va;
    unsigned int ring_buffer_size;
    void* ring_buffer_cpu_va;

    unsigned long long fence_buffer_gpu_va;
    void* fence_buffer_cpu_va;

    unsigned long long fence_value;

    unsigned int process_id;

    int rb_index;
    EngineSatus_e3k status;
} submit_info_t;

typedef struct _addr_map_entry
{
    unsigned long long phyAddr;
    unsigned long long size;
    unsigned long long target;
    unsigned long long offset;      // offset in memory
    struct os_pages_memory *memory;
    zx_cpu_vm_area_t *vma;   // vma for memory
    struct list_head link;
} addr_map_entry_t;

typedef struct _ptes_memory
{
    void *virt_addr;
    unsigned long long size;
    zx_cpu_vm_area_t *vma;
    struct list_head link;
} ptes_memory_t;

typedef struct _dup_hang_ctx
{
    struct os_file      *file;
    const char          *image;
    adapter_t           *adapter;
    unsigned int         chip_id;
    unsigned int         slice_mask;
    unsigned long long   local_memory_size;
    unsigned long long   pcie_memory_size;

    unsigned long long   record_number;

    unsigned long long   ring_buffer_offset;
    unsigned long long   single_ring_buffer_size_in_byte;

    unsigned long long   dummy_page_entry;

    void                *local_visible_memory_cpu_va;

    void                *fence_cpu_va[MAX_HANG_DUMP_DATA_POOL_NUMBER];

    submit_info_t       submit_info;

    int                 hang_index;

    struct list_head    addr_map_list;

    struct list_head    ptes_list;
}dup_hang_ctx_t;

extern int use_hw_dump;

extern void vidsch_duplicate_hang_e3k(adapter_t *adapter, const char *image);
extern void vidsch_dump_hang_e3k(adapter_t *adapter);
extern unsigned int vidsch_pre_save_misc_e3k(adapter_t *adapter, vidsch_mgr_t *sch_mgr,  unsigned int what_to_save);

typedef union
{
    struct
    {
        unsigned int internal_dump_hw      : 1;

        unsigned int internal_block_submit : 1; // block sumbit during replaying
    };
    unsigned int uint;
} reg_debug_mode_e3k;

extern reg_debug_mode_e3k debug_mode_e3k;
extern void vidsch_display_debugbus_info_e3k(adapter_t *adapter, struct os_printer *p, int video);
extern void vidsch_dump_debugbus_label(adapter_t *adapter, struct os_printer *p);

#define HangDump_RingBufferBlockSize        (16 * 1024)

#endif
