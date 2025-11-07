//*****************************************************************************
//  Copyright (c) 2018 Shanghai Zhaoxin Semiconductor Co., Ltd.
//  All Rights Reserved.
//
//  This is UNPUBLISHED PROPRIETARY SOURCE CODE of Shanghai Zhaoxin Semiconductor Co., Ltd.;
//  the contents of this file may not be disclosed to third parties, copied or
//  duplicated in any form, in whole or in part, without the prior written
//  permission of Shanghai Zhaoxin Semiconductor Co., Ltd.
//
//  The copyright of the source code is protected by the copyright laws of the People's
//  Republic of China and the related laws promulgated by the People's Republic of China
//  and the international covenant(s) ratified by the People's Republic of China.
//*****************************************************************************

#ifndef __zx_perf_h__
#define __zx_perf_h__


typedef struct zx_begin_perf_event_tag
{
    unsigned int max_event_num;     /* in */
    unsigned int attrib_list[64];   /* in */
} zx_begin_perf_event_t;

typedef struct zx_end_perf_event_tag
{
    unsigned int placeholder;       /* in */
} zx_end_perf_event_t;

typedef struct zx_get_perf_event_tag
{
    unsigned int max_event_num;             /* in */
    unsigned int max_event_buffer_size;     /* in */
    void *event_buffer;                     /* in/out */
    unsigned int event_filled_num;          /* out */
    unsigned int event_buffer_filled_size;  /* out */
    unsigned int event_lost_num;            /* out */
    unsigned long long event_lost_timestamp;/* out */

    void *isr_event_buffer;                 /* in/out */
    unsigned int isr_filled_num;            /* out */
    unsigned int isr_filled_size;           /* out */
} zx_get_perf_event_t;

typedef struct _zx_miu_list_item
{
    unsigned int write : 1;
    unsigned int value_valid : 1;
    unsigned int miu_type : 6;
    unsigned int miu_offset : 24;
    unsigned int miu_value;
}zx_miu_list_item_t;

typedef struct zx_begin_miu_dump_tag
{
    unsigned int max_event_num;
}zx_begin_miu_dump_perf_event_t;

typedef struct zx_end_miu_dump_tag
{
    unsigned int place_hoder;
}zx_end_miu_dump_perf_event_t;

typedef struct zx_get_miu_dump_tag
{
    unsigned int max_event_num;
    unsigned int max_event_buffer_size;
    void *event_buffer;
    unsigned int event_filled_num;
    unsigned int event_buffer_filled_size;
    unsigned int event_lost_num;
    unsigned long long event_lost_timestamp;
}zx_get_miu_dump_perf_event_t;

typedef struct zx_direct_get_miu_dump_tag
{
    zx_miu_list_item_t *miu_table;
    unsigned int        miu_table_length;
    unsigned int        timestamp_high;
    unsigned int        timestamp_low;
}zx_direct_get_miu_dump_perf_event_t;

typedef struct zx_miu_reg_list_tag
{
    unsigned int          miu_table_length;
    zx_miu_list_item_t   *miu_table;
}zx_miu_reg_list_perf_event_t;

typedef struct zx_hwq_info_t
{
    unsigned int        Usage_3D;
    unsigned int        Usage_VCP;
    unsigned int        Usage_VPP;
    unsigned int sample_time;
}zx_hwq_info;

typedef struct gfx_hwq_info_ext_t
{
    unsigned int        ext_version;
    unsigned int        sample_time;
    unsigned int        Usage_3D;
    unsigned int        Usage_VPP;
    unsigned int        Usage_VCP0;
    unsigned int        Usage_VCP1;
    unsigned int        Usage_3D_High;
    unsigned int        Usage_VPP0;
    unsigned int        Usage_VPP1;
    unsigned int        Usage_VCP2;
    unsigned int        reserved4;
    unsigned int        reserved5;
    unsigned int        reserved6;
    unsigned int        reserved7;
}gfx_hwq_info_ext;

typedef struct zx_perf_event_header_tag zx_perf_event_header_t;

typedef struct zx_perf_event_gl_draw_enter_tag zx_perf_event_gl_draw_enter_t;
typedef struct zx_perf_event_gl_draw_exit_tag zx_perf_event_gl_draw_exit_t;
typedef struct zx_perf_event_cm_flush_enter_tag zx_perf_event_cm_flush_enter_t;
typedef struct zx_perf_event_cm_flush_exit_tag zx_perf_event_cm_flush_exit_t;
typedef struct zx_perf_event_swap_buffer_enter_tag zx_perf_event_swap_buffer_enter_t;
typedef struct zx_perf_event_swap_buffer_exit_tag zx_perf_event_swap_buffer_exit_t;
typedef struct zx_perf_event_present_enter_tag zx_perf_event_present_enter_t;
typedef struct zx_perf_event_present_exit_tag zx_perf_event_present_exit_t;
typedef struct zx_perf_event_dma_buffer_queued_tag zx_perf_event_dma_buffer_queued_t;
typedef struct zx_perf_event_dma_buffer_submitted_tag zx_perf_event_dma_buffer_submitted_t;
typedef struct zx_perf_event_dma_buffer_completed_tag zx_perf_event_dma_buffer_completed_t;
typedef struct zx_perf_event_lost_event_tag zx_perf_event_lost_event_t;
typedef struct zx_perf_event_vsync_tag  zx_perf_event_vsync_t;
typedef struct zx_perf_event_ps_flip_tag zx_perf_event_ps_flip_t;
typedef struct zx_perf_event_overlay_flip_tag zx_perf_event_overlay_flip_t;

typedef struct zx_perf_event_lock_enter_tag zx_perf_event_lock_enter_t;
typedef struct zx_perf_event_lock_exit_tag zx_perf_event_lock_exit_t;
typedef struct zx_perf_event_unlock_enter_tag zx_perf_event_unlock_enter_t;
typedef struct zx_perf_event_unlock_exit_tag zx_perf_event_unlock_exit_t;

typedef struct zx_perf_event_enqueue_native_kernel_enter_tag zx_perf_event_enqueue_native_kernel_enter_t;
typedef struct zx_perf_event_enqueue_native_kernel_exit_tag zx_perf_event_enqueue_native_kernel_exit_t;
typedef struct zx_perf_event_enqueue_task_enter_tag zx_perf_event_enqueue_task_enter_t;
typedef struct zx_perf_event_enqueue_task_exit_tag zx_perf_event_enqueue_task_exit_t;
typedef struct zx_perf_event_enqueue_ndr_kernel_enter_tag zx_perf_event_enqueue_ndr_kernel_enter_t;
typedef struct zx_perf_event_enqueue_ndr_kernel_exit_tag zx_perf_event_enqueue_ndr_kernel_exit_t;

typedef struct zx_perf_event_sync_event_tag zx_perf_event_sync_event_t;
typedef struct zx_perf_event_wait_start_tag zx_perf_event_wait_start_t;
typedef struct zx_perf_event_wait_finish_tag zx_perf_event_wait_finish_t;
typedef struct zx_perf_event_wait_on_server_finish_tag zx_perf_event_wait_on_server_finish_t;

typedef struct zx_perf_event_miu_counter_dump_tag zx_perf_event_miu_counter_dump_t;

typedef struct zx_perf_event_mm_lock_enter_tag zx_perf_event_mm_lock_enter_t;
typedef struct zx_perf_event_mm_lock_exit_tag zx_perf_event_mm_lock_exit_t;
typedef struct zx_perf_event_mm_unlock_enter_tag zx_perf_event_mm_unlock_enter_t;
typedef struct zx_perf_event_mm_unlock_exit_tag zx_perf_event_mm_unlock_exit_t;
typedef struct zx_perf_event_mm_alloc_enter_tag zx_perf_event_mm_alloc_enter_t;
typedef struct zx_perf_event_mm_alloc_exit_tag zx_perf_event_mm_alloc_exit_t;
typedef struct zx_perf_event_mm_free_enter_tag zx_perf_event_mm_free_enter_t;
typedef struct zx_perf_event_mm_free_exit_tag zx_perf_event_mm_free_exit_t;

typedef struct zx_perf_event_bandwidth_tag zx_perf_event_bandwidth_t;


typedef union zx_perf_event_tag zx_perf_event_t;

// perf event type
#define ZX_PERF_EVENT_GL_DRAW_ENTER                0x1000
#define ZX_PERF_EVENT_GL_DRAW_EXIT                 0x1001
#define ZX_PERF_EVENT_CM_FLUSH_ENTER               0x1002
#define ZX_PERF_EVENT_CM_FLUSH_EXIT                0x1003
#define ZX_PERF_EVENT_SWAP_BUFFER_ENTER            0x1004
#define ZX_PERF_EVENT_SWAP_BUFFER_EXIT             0x1005
#define ZX_PERF_EVENT_PRESENT_ENTER                0x1006
#define ZX_PERF_EVENT_PRESENT_EXIT                 0x1007
#define ZX_PERF_EVENT_DMA_BUFFER_QUEUED            0x1008
#define ZX_PERF_EVENT_DMA_BUFFER_SUBMITTED         0x1009
#define ZX_PERF_EVENT_DMA_BUFFER_COMPLETED         0x100A
#define ZX_PERF_EVENT_LOST_EVENT                   0x100B
#define ZX_PERF_EVENT_VSYNC                        0x100C
#define ZX_PERF_EVENT_PS_FLIP                      0x100D
#define ZX_PERF_EVENT_OVERLAY_FLIP                 0x100E

#define ZX_PERF_EVENT_CL_ENQUEUE_NATIVE_KERNEL_ENTER 0x100F
#define ZX_PERF_EVENT_CL_ENQUEUE_NATIVE_KERNEL_EXIT  0x1010
#define ZX_PERF_EVENT_CL_ENQUEUE_TASK_ENTER        0x1011
#define ZX_PERF_EVENT_CL_ENQUEUE_TASK_EXIT         0x1012
#define ZX_PERF_EVENT_CL_ENQUEUE_NDR_KERNEL_ENTER  0x1013
#define ZX_PERF_EVENT_CL_ENQUEUE_NDR_KERNEL_EXIT   0x1014

#define ZX_PERF_EVENT_LOCK_ENTER                   0x1015
#define ZX_PERF_EVENT_LOCK_EXIT                    0x1016
#define ZX_PERF_EVENT_UNLOCK_ENTER                 0x1017
#define ZX_PERF_EVENT_UNLOCK_EXIT                  0x1018
#define ZX_PERF_EVENT_SYNC_EVENT                   0x1019
#define ZX_PERF_EVENT_WAIT_START                   0x101A
#define ZX_PERF_EVENT_WAIT_FINISH                  0x101B
#define ZX_PERF_EVENT_WAIT_ON_SERVER_FINISH        0x101C

#define ZX_PERF_EVENT_HWC_FLUSH_ENTER              0x101E
#define ZX_PERF_EVENT_HWC_FLUSH_EXIT               0x101F

#define ZX_PERF_EVENT_MIU_COUNTER                  0x1020

#define ZX_PERF_EVENT_MM_LOCK_ENTER                0x1050
#define ZX_PERF_EVENT_MM_LOCK_EXIT                 0x1051
#define ZX_PERF_EVENT_MM_UNLOCK_ENTER              0x1052
#define ZX_PERF_EVENT_MM_UNLOCK_EXIT               0x1053
#define ZX_PERF_EVENT_MM_ALLOC_ENTER               0x1054
#define ZX_PERF_EVENT_MM_ALLOC_EXIT                0x1055
#define ZX_PERF_EVENT_MM_FREE_ENTER                0x1056
#define ZX_PERF_EVENT_MM_FREE_EXIT                 0x1057

#define ZX_PERF_EVENT_BANDWIDTH                    0x1060

#define ZX_PERF_EVENT_INVALID_PID                  0
#define ZX_PERF_EVENT_INVALID_TID                  0

#define ZX_PERF_EVENT_DMA_TYPE_2D                  0x1
#define ZX_PERF_EVENT_DMA_TYPE_3D                  0x2
#define ZX_PERF_EVENT_DMA_TYPE_3DBLT               0x3
#define ZX_PERF_EVENT_DMA_TYPE_PRESENT             0x4
#define ZX_PERF_EVENT_DMA_TYPE_PAGING              0x5
#define ZX_PERF_EVENT_DMA_TYPE_OVERLAY             0x6

struct zx_perf_event_header_tag
{
    unsigned int size; /* total event size */
    unsigned int type; /* event type */
    unsigned int pid; /* process id that generates the event */
    unsigned int tid; /* thead id that generates the event */
    unsigned int timestamp_low; /* ticks (or nanoseconds?) when generating the event */
    unsigned int timestamp_high;
};

struct zx_perf_event_gl_draw_enter_tag
{
    zx_perf_event_header_t header;
    unsigned int frame_num;
    unsigned int draw_num;
};

struct zx_perf_event_gl_draw_exit_tag
{
    zx_perf_event_header_t header;
    unsigned int frame_num;
    unsigned int draw_num;
};

struct zx_perf_event_cm_flush_enter_tag
{
    zx_perf_event_header_t header;
};

struct zx_perf_event_cm_flush_exit_tag
{
    zx_perf_event_header_t header;
};

struct zx_perf_event_swap_buffer_enter_tag
{
    zx_perf_event_header_t header;
};

struct zx_perf_event_swap_buffer_exit_tag
{
    zx_perf_event_header_t header;
};

struct zx_perf_event_present_enter_tag
{
    zx_perf_event_header_t header;
};

struct zx_perf_event_present_exit_tag
{
    zx_perf_event_header_t header;
};

struct zx_perf_event_dma_buffer_queued_tag
{
    zx_perf_event_header_t header;
    unsigned int gpu_context;
    unsigned int dma_idx_low;
    unsigned int dma_idx_high;
    unsigned int engine_idx;
};

struct zx_perf_event_dma_buffer_submitted_tag
{
    zx_perf_event_header_t header;
    unsigned int dma_type;
    unsigned int gpu_context; /* through gpu_context, dma_idx_low, dma_idx_high we can make connection */
    unsigned int dma_idx_low; /* between dma_buffer_queued and dma_buffer_submitted */
    unsigned int dma_idx_high;
    unsigned int engine_idx;  /* through engine_idx, fence_id_low, fence_id_high we can make connection */
    unsigned int fence_id_low; /* between dma_buffer_submitted and dma_buffer_completed */
    unsigned int fence_id_high;
};

struct zx_perf_event_dma_buffer_completed_tag
{
    zx_perf_event_header_t header;
    unsigned int engine_idx;
    unsigned int fence_id_low;
    unsigned int fence_id_high;
};

struct zx_perf_event_lost_event_tag
{
    zx_perf_event_header_t header;
    unsigned int lost_event_num;
    unsigned int lost_timestamp_low;
    unsigned int lost_timestamp_high;
};

struct zx_perf_event_vsync_tag
{
    zx_perf_event_header_t header;
    unsigned int iga_idx;
    unsigned int vsync_cnt_low;
    unsigned int vsync_cnt_high;
};

struct zx_perf_event_ps_flip_tag
{
    zx_perf_event_header_t header;
    unsigned int iga_idx;
    unsigned int allocation;
};

struct zx_perf_event_overlay_flip_tag
{
    zx_perf_event_header_t header;
    unsigned int iga_idx;
    unsigned int overlay_idx;
    unsigned int allocation;
};

struct zx_perf_event_lock_enter_tag
{
    zx_perf_event_header_t header;
    unsigned int device;
    unsigned int handle; // locked allocation handle
    unsigned int flag; // lock flags
};

struct zx_perf_event_lock_exit_tag
{
    zx_perf_event_header_t header;
    unsigned int device;
    unsigned int handle; // locked allocation handle
    unsigned int flag; // lock flags
};

struct zx_perf_event_unlock_enter_tag
{
    zx_perf_event_header_t header;
    unsigned int device;
    unsigned int handle; // locked allocation handle

};

struct zx_perf_event_unlock_exit_tag
{
    zx_perf_event_header_t header;
    unsigned int device;
    unsigned int handle; // locked allocation handle
};

struct zx_perf_event_enqueue_native_kernel_enter_tag
{
    zx_perf_event_header_t header;
    int cl_event;// is cl event, always be 1
};

struct zx_perf_event_enqueue_native_kernel_exit_tag
{
    zx_perf_event_header_t header;
    int cl_event;
};

struct zx_perf_event_enqueue_task_enter_tag
{
    zx_perf_event_header_t header;
    int cl_event;
};

struct zx_perf_event_enqueue_task_exit_tag
{
    zx_perf_event_header_t header;
    int cl_event;
};

struct zx_perf_event_enqueue_ndr_kernel_enter_tag
{
    zx_perf_event_header_t header;
    int cl_event;
};

struct zx_perf_event_enqueue_ndr_kernel_exit_tag
{
    zx_perf_event_header_t header;
    int cl_event;
};

struct zx_perf_event_sync_event_tag
{
    zx_perf_event_header_t header;
    unsigned int engine_idx;
    unsigned int type; // the sync type
    unsigned int handle;
    unsigned int fence_value_high;
    unsigned int fence_value_low;
    unsigned int gpu_context;
    unsigned int ctx_task_id_high;
    unsigned int ctx_task_id_low;
};

typedef struct zx_perf_event_wait_instance_tag
{
    unsigned int handle;
    unsigned int fence_value_high;
    unsigned int fence_value_low;
    unsigned int timeout;
}zx_perf_event_wait_instance_t;

struct zx_perf_event_wait_start_tag
{
    zx_perf_event_header_t header;
    unsigned int engine_idx;
    unsigned int gpu_context;
    unsigned int task_id_high;
    unsigned int task_id_low;
    zx_perf_event_wait_instance_t instance[32];
};

struct zx_perf_event_wait_finish_tag
{
    zx_perf_event_header_t header;
    unsigned int engine_idx;
    unsigned int gpu_context;
    unsigned int status;
    unsigned int task_id_high;
    unsigned int task_id_low;
};

struct zx_perf_event_wait_on_server_finish_tag
{
    zx_perf_event_header_t header;
    unsigned int engine_idx;
    unsigned int gpu_context;
    unsigned int task_id_high;
    unsigned int task_id_low;
};

struct zx_perf_event_miu_counter_dump_tag
{
    zx_perf_event_header_t header;
    unsigned int fence_id_high;
    unsigned int fence_id_low;
    unsigned int task_id_high;
    unsigned int task_id_low;
    unsigned int gpu_context;

    unsigned int counter_buffer_offset;
    unsigned int buffer_length;
};

struct zx_perf_event_mm_lock_enter_tag
{
    zx_perf_event_header_t header;
    unsigned int handle; //  allocation handle

};

struct zx_perf_event_mm_lock_exit_tag
{
    zx_perf_event_header_t header;
    unsigned int handle; //  allocation handle

};

struct zx_perf_event_mm_unlock_enter_tag
{
    zx_perf_event_header_t header;
    unsigned int handle; //  allocation handle

};

struct zx_perf_event_mm_unlock_exit_tag
{
    zx_perf_event_header_t header;
    unsigned int handle; //  allocation handle

};

struct zx_perf_event_mm_alloc_enter_tag
{
    zx_perf_event_header_t header;

};

struct zx_perf_event_mm_alloc_exit_tag
{
    zx_perf_event_header_t header;
    unsigned int handle; //  allocation handle

};

struct zx_perf_event_mm_free_enter_tag
{
    zx_perf_event_header_t header;
    unsigned int handle; //  allocation handle

};

struct zx_perf_event_mm_free_exit_tag
{
    zx_perf_event_header_t header;
    unsigned int handle; //  allocation handle

};

typedef struct  zx_perf_event_bandwidth_data_tag
{
    unsigned int  diu_rd_db;
    unsigned int  diu_wr_db;

    unsigned int  gfx_rd_db;
    unsigned int  gfx_wr_db;

    unsigned int  vpp_rd_db;
    unsigned int  vpp_wr_db;

    unsigned int  vcp_rd_db;
    unsigned int  vcp_wr_db;

    unsigned int  local_rd_db;
    unsigned int  local_wr_db;

    unsigned int  pcie_rd_db;
    unsigned int  pcie_wr_db;

} zx_perf_event_bandwidth_data_t;

struct zx_perf_event_bandwidth_tag
{
    zx_perf_event_header_t header;
    zx_perf_event_bandwidth_data_t bandwidth_data;
};


union zx_perf_event_tag
{
    zx_perf_event_header_t header;
    zx_perf_event_gl_draw_enter_t gl_draw_enter;
    zx_perf_event_gl_draw_exit_t gl_draw_exit;
    zx_perf_event_cm_flush_enter_t cm_flush_enter;
    zx_perf_event_cm_flush_exit_t cm_flush_exit;
    zx_perf_event_swap_buffer_enter_t swap_buffer_enter;
    zx_perf_event_swap_buffer_exit_t swap_buffer_exit;
    zx_perf_event_present_exit_t present_enter;
    zx_perf_event_present_exit_t present_exit;
    zx_perf_event_dma_buffer_queued_t dma_buffer_queued;
    zx_perf_event_dma_buffer_submitted_t dma_buffer_submitted;
    zx_perf_event_dma_buffer_completed_t dma_buffer_completed;
    zx_perf_event_lost_event_t lost_event;
    zx_perf_event_vsync_t vsync_event;
    zx_perf_event_ps_flip_t ps_flip_event;
    zx_perf_event_overlay_flip_t overlay_flip_event;

    zx_perf_event_lock_enter_t lock_enter;
    zx_perf_event_lock_exit_t lock_exit;
    zx_perf_event_unlock_enter_t unlock_enter;
    zx_perf_event_unlock_exit_t unlock_exit;

    zx_perf_event_enqueue_native_kernel_enter_t eq_native_kernel_enter;
    zx_perf_event_enqueue_native_kernel_exit_t eq_native_kernel_exit;
    zx_perf_event_enqueue_task_enter_t eq_task_enter;
    zx_perf_event_enqueue_task_exit_t eq_task_exit;
    zx_perf_event_enqueue_ndr_kernel_enter_t eq_ndr_kernel_enter;
    zx_perf_event_enqueue_ndr_kernel_exit_t eq_ndr_kernel_exit;

    zx_perf_event_sync_event_t sync_event;
    zx_perf_event_wait_start_t wait_start;
    zx_perf_event_wait_finish_t wait_finish;
    zx_perf_event_wait_on_server_finish_t wait_on_server_finish;
    zx_perf_event_miu_counter_dump_t miu_counter;

    zx_perf_event_mm_lock_enter_t   mm_lock_enter;
    zx_perf_event_mm_lock_exit_t    mm_lock_exit;
    zx_perf_event_mm_unlock_enter_t mm_unlock_enter;
    zx_perf_event_mm_unlock_exit_t  mm_unlock_exit;
    zx_perf_event_mm_alloc_enter_t  mm_alloc_enter;
    zx_perf_event_mm_alloc_exit_t   mm_alloc_exit;
    zx_perf_event_mm_free_enter_t   mm_free_enter;
    zx_perf_event_mm_free_exit_t    mm_free_exit;

    zx_perf_event_bandwidth_t       bandwidth;

};

typedef struct zx_perf_status_tag
{
    int started; /* out */
    int miu_started; /* out */
} zx_perf_status_t;

#endif
