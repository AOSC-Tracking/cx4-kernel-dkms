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

#ifndef __ZX_TYPES_H__
#define __ZX_TYPES_H__

#include "zx_def.h"

typedef enum
{
    ZX_USAGE_DISPLAY_SURFACE   = 0x00000001,
    ZX_USAGE_OVERLAY_SURFACE   = 0x00000002,
    ZX_USAGE_CURSOR_SURFACE    = 0x00000004,
    ZX_USAGE_RENDER_TARGET     = 0x00000008,
    ZX_USAGE_TEXTURE_BUFFER    = 0x00000010,
    ZX_USAGE_SHADER_BUFFER     = 0x00000020,
    ZX_USAGE_DATA_BUFFER       = 0x00000040,
    ZX_USAGE_SHARE_BUFFER      = 0x00000080,
    ZX_USAGE_VIDEO             = 0x00000100,
    ZX_USAGE_VIDEO_DECODER     = 0x00000200,
    ZX_USAGE_CAMERA            = 0x00000400,
    ZX_USAGE_VIDEO_ENCODER     = 0x00000800,
    ZX_USAGE_RENDER_SCRIPT     = 0x00001000,
    ZX_USAGE_FORCE_LOCAL       = 0x00002000,
    ZX_USAGE_FORCE_PCIE        = 0x00004000,
    ZX_USAGE_PROTECT_CONTENT = 0x00008000,
}zx_usage_bit;

typedef enum
{
    ZX_FORMAT_A8_UNORM       = 1,
    ZX_FORMAT_B5G6R5_UNORM   = 2,        /*rmask 0xf800; gmask 0x7e0; bmask 0x1f*/
    ZX_FORMAT_B5G5R5A1_UNORM = 3,
    ZX_FORMAT_A1B5G5R5_UNORM = 4,
    ZX_FORMAT_B4G4R4A4_UNORM = 5,
    ZX_FORMAT_A4B4G4R4_UNORM = 6,
    ZX_FORMAT_B8G8R8A8_UNORM = 7,      /*rmask 0x00ff0000; gmask 0x0000ff00; bmask 0x000000ff; amask 0xff000000*/
    ZX_FORMAT_B8G8R8X8_UNORM = 8,
    ZX_FORMAT_R8G8B8A8_UNORM = 9,      /*rmask 0x000000ff; gmask 0x0000ff00; bmask 0x00ff0000; amask 0xff000000*/
    ZX_FORMAT_R8G8B8X8_UNORM = 10,
    ZX_FORMAT_A8R8G8B8_UNORM = 11,
    ZX_FORMAT_YUY2           = 12,
    ZX_FORMAT_NV12_LINEAR    = 13,
    ZX_FORMAT_NV12_TILED     = 14,
    ZX_FORMAT_NV21_LINEAR    = 15,
    ZX_FORMAT_YV12           = 16,
    ZX_FORMAT_FLOAT32        = 17,
    ZX_FORMAT_UINT32         = 18,
    ZX_FORMAT_INT32          = 19,
    ZX_FORMAT_R8_UNORM       = 20,

    ZX_FORMAT_RAW10          = 21,
    ZX_FORMAT_RAW16          = 22,
    ZX_FORMAT_RAW_OPAQUE     = 23,
    ZX_FORMAT_B10G10R10A2_UNORM = 24,
    ZX_FORMAT_R8G8_UNORM     = 25,
    ZX_FORMAT_R16_UNORM     = 26,
    ZX_FORMAT_R16G16_UNORM     = 27,
    ZX_FORMAT_P010             = 28,

    ZX_FORMAT_B8G8R8A8_SRGB  = 29,
    ZX_FORMAT_B8G8R8X8_SRGB  = 30,
    ZX_FORMAT_I420           = 31,
}zx_format;

typedef enum _CM_CONTEXT_CLIENT_TYPE
{
    CLIENT_TYPE_UNKNOWN   = 0,
    CLIENT_TYPE_OPENGL    = 1,
    CLIENT_TYPE_OPENGLES  = 2,
    CLIENT_TYPE_VULKAN    = 3,
    CLIENT_TYPE_OPENCL    = 4,
    CLIENT_TYPE_VIDEO     = 5,
} CM_CONTEXT_CLIENT_TYPE;

typedef struct
{
    unsigned int dma32       :1;
    unsigned int need_flush  :1;
    unsigned int need_zero   :1;
    unsigned int fixed_page  :1; /*fixed page size*/
    unsigned int need_dma_map:1;

    int          page_size;      /*page size when fixed page flags set*/
}alloc_pages_flags_t;

#define ZX_POOL_UNKNOWN         0
#define ZX_POOL_NONLOCALVIDMEM  1
#define ZX_POOL_LOCALVIDMEM     2
#define ZX_POOL_VIDEOMEMORY     (ZX_POOL_LOCALVIDMEM | ZX_POOL_NONLOCALVIDMEM)

#define ZX_CACHE_NOT_CARE       0
#define ZX_CACHE_CACHABLE       1
#define ZX_CACHE_UNCACHABLE     2

typedef struct zx_create_allocation_info
{
    unsigned long long      alignment;      // in
    unsigned long long      size;           // in
    unsigned int            priority;       // in
    unsigned int            range_type;     // in: the compress range type
    unsigned int            handle;         // out
    unsigned int            user_buf_size;  // in
    zx_ptr64_t              user_ptr;       // in
    unsigned int            pad;
    union
    {
        struct
        {
            unsigned int    pool_type           : 2;
            unsigned int    cache_type          : 2;
            unsigned int    cpu_visible         : 1;
            unsigned int    unpagable           : 1;
            unsigned int    tiled               : 1;
            unsigned int    share               : 1;
            unsigned int    bVideoInternal      : 1;
            unsigned int    continuous          : 1;
            unsigned int    force_clear         : 1;//zero clear when create
            unsigned int    secured             : 1;
            unsigned int    stereo              : 1;
            unsigned int    override_priority   : 1;
            unsigned int    reserved            : 18;
        };
        unsigned int value;
    } flags;
} zx_create_allocation_info_t;

typedef struct zx_create_allocation
{
    unsigned int device;
    unsigned int num_allocations;
    zx_ptr64_t   alloc_info;//zx_create_allocation_info_t
}zx_create_allocation_t;

enum client_type
{
    VULKAN,
    OGL_2D,
};

/*
 * why need this structure?
 * zx_create_allocation_t only contains 2d/vulkan SHARE things.
 * need private structure to pass 2d or vulkan private info when create allocation.
 * 2d private things: format, etc.
 * vulkan private things: CIL3_ALLOCATION_DESC_E3K, etc.
 */
typedef struct zx_create_allocation_private
{
    unsigned int client;  //0:vulkan, 1:2D.
    unsigned int pad;
    union
    {
        struct
        {
            unsigned int private_data_size;
            unsigned int private_data_size_2;
            zx_ptr64_t private_data;
            zx_ptr64_t private_data_2;//for resource_desc
        }_vulkan;
    };
}zx_create_allocation_private_t;

typedef struct
{
    unsigned int gid;
    unsigned int size;
    unsigned int pool_type      :2;
    unsigned int cache_type     :2;
    unsigned int unpagable      :1;
    unsigned int secured        :1;
    unsigned int share          :1;
    unsigned int cpu_visible    :1;
    unsigned int tiled          :1;
    unsigned int alignment      :1;
    unsigned int local_continuous :1;
    unsigned int priority;
    unsigned int bl_slot_index;
    unsigned int compress_format;
    unsigned int segment_id;    // only used for debug
    unsigned int pad;
    unsigned long long cpu_phy_addr;
} zx_allocation_info_t;

typedef struct
{
    char                       pname[64];  //process name
    unsigned int               pid;        //process id
}zx_process_info_t;

typedef struct
{
    int width;
    int height;
    int pitch;
    int bit_cnt;
    unsigned int at_type;
    unsigned int hw_format;
    unsigned int bVideoInternal;
    unsigned int pad;
    unsigned long long gpu_va;
    unsigned long long gpu_pa;
} zx_gem_dbg_info_t;

typedef struct
{
    unsigned int handle;
    unsigned int pad;
    zx_gem_dbg_info_t info;
} zx_set_gem_dbg_info_t;

typedef struct
{
    unsigned int gid;       // input
    unsigned int handle;    // output
} zx_gem_open_t;

typedef union
{
    struct
    {
        unsigned int assume_not_inuse      :1;
        unsigned int synchronous_destroy   :1;
        unsigned int reserved              :30;
    };
    unsigned int value;
} zx_destroy_flag_t;

typedef struct
{
    unsigned int device;
    unsigned int num_allocations;
    zx_destroy_flag_t flag;
    unsigned int pad;
    zx_ptr64_t allocation_list;
} zx_destroy_allocation_t;

typedef struct
{
    unsigned int device;            /* in */
    unsigned int allocation;        /* in */
    unsigned int width;             /* in */
    unsigned int height;            /* in */
    unsigned int width_aligned;     /* in */
    unsigned int height_aligned;    /* in */
    unsigned int pitch;             /* in */
    unsigned int acquire_apeture :1;/* in */
    unsigned int read_only       :1;/* in */
    unsigned int write_only      :1;/* in */

    zx_ptr64_t  yaddr;            /* out */
    zx_ptr64_t  uaddr;            /* out */
    zx_ptr64_t  vaddr;            /* out */
}zx_lock_yv12_allocation_t;

typedef struct
{
    unsigned int device;            /* in */
    unsigned int allocation;        /* in */
    unsigned int width;             /* in */
    unsigned int height;            /* in */
    unsigned int width_aligned;     /* in */
    unsigned int height_aligned;    /* in */
    unsigned int pitch;             /* in */
    unsigned int release_apeture :1;/* in */

    zx_ptr64_t  yaddr;            /* in */
    zx_ptr64_t  uaddr;            /* in */
    zx_ptr64_t  vaddr;            /* in */
}zx_unlock_yv12_allocation_t;


typedef struct
{
    unsigned int device;            /* in */
    unsigned int allocation;        /* in */
    unsigned int offset;            /* in */
    unsigned int size;              /* in */
}zx_flush_allocation_t;


typedef struct
{
    unsigned int device;         /* out */
}zx_create_device_t;


typedef struct
{
    unsigned int  device;          /* in   */
    unsigned int  engine_index;    /* in   */
    int           adapter_index;  /*in*/
    unsigned int  priority;
    unsigned int  type;
    unsigned int  client_type;

    union
    {
        struct
        {
            unsigned int null_render :1;
            unsigned int initial_data :1;
            unsigned int disable_tdr :1;
            unsigned int synchronization_only :1;
            unsigned int power_dvfs_indicador :1;
            unsigned int reserved: 27;
        };
        unsigned int value;
    }flag;

    unsigned int  context;       /* out  */
    zx_ptr64_t    paging_fence_cpu_va; /* out  */
}zx_create_context_t;

typedef struct
{
    unsigned int device;              /* in   */
    unsigned int context;             /* in   */
}zx_destroy_context_t;

typedef struct _zx_cmd_header
{
    unsigned int               Contain3dCmd : 1; // keep consistent with source_new, not used for elite
    unsigned int               ContainLpdpCmd : 1;
    unsigned int               CLCSOnly : 1; // opencl without 2D/3D blt/L2 invalidate
    unsigned int               CommandLength : 20;
    unsigned int               Flag2dCmd : 1; // Xserver 2d command
    unsigned int               Flag3dbltCmd : 1; // Xserver composite 3d blt command
    unsigned int               DvfsForceLevel : 3;
    unsigned int               Reserved : 4;
} zx_cmd_header;


typedef struct
{
    unsigned int         context;        /* in */
    unsigned int         dma_cnt;       //number of dma
    zx_ptr64_t           dma_gpu_va;   //array of dma gpu virtual address  (should be a array of u64)
    zx_ptr64_t           dma_length;   //array of dma length(u64 array)

    zx_ptr64_t           primary_list; // primary alloction list. (u64 array)
    zx_ptr64_t           primary_write_op_list;//primary allocation write op list(u32 array).

    unsigned long long   ctx_buf_va;

    unsigned int         primary_cnt; //number of primary allocation list
    union
    {
        struct
        {
            unsigned int null_rendering     : 1;  //skip render command, only do fence operation
            unsigned int need_signal        : 1;  //for some case which seperately call render and insert fence,
                                                  //that insert fence will generate fence dma and signal by hw, this flag will be false
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
        }render_flag;
        unsigned int uflag;
    };
    unsigned int       wait_fence_cnt;    //number of wait fence
    unsigned int       signal_fence_cnt;  //number of signal fence

    zx_ptr64_t         wait_fence;       //array of wait fence, u32 arrary
    zx_ptr64_t         signal_fence;     //array of signal fence u32 array
}zx_render_t;


typedef struct
{
    unsigned int crtc_id;   /* in */
    unsigned int pipe;      /* out */
} zx_kms_get_pipe_from_crtc_t;

typedef struct
{
    unsigned int         device;                /* in */
    unsigned int         context;               /* in */
    unsigned int         hw_ctx_buf_index;          /* in */
} zx_add_hw_ctx_buf_t;

typedef struct
{
    unsigned int          device;                /* in */
    unsigned int          context;               /* in */
    unsigned int          hw_ctx_buf_index;          /* in */
} zx_rm_hw_ctx_buf_t;


/* ZX blending */

enum {
    /* no blending */
    ZX_BLENDING_NONE     = 0x00,

    /* ONE / ONE_MINUS_SRC_ALPHA */
    ZX_BLENDING_PREMULT  = 0x01,

    /* SRC_ALPHA / ONE_MINUS_SRC_ALPHA */
    ZX_BLENDING_COVERAGE = 0x02,

    /* ONE_MINUS_DST_ALPHA / DST_ALPHA */
    ZX_BLENDING_COVERAGE_INVERT = 0x03
};

/* ZX transform */
enum {
    ZX_TRANSFORM_NONE      = 0x00,
    /* flip source image horizontally (around the vertical axis) */
    ZX_TRANSFORM_FLIP_H    = 0x01,
    /* flip source image vertically (around the horizontal axis)*/
    ZX_TRANSFORM_FLIP_V    = 0x02,
    /* rotate source image 90 degrees counter clockwise */
    ZX_TRANSFORM_ROT_90    = 0x04,
    /* rotate source image 180 degrees */
    ZX_TRANSFORM_ROT_180   = 0x03,
    /* rotate source image 270 degrees counter clockwise */
    ZX_TRANSFORM_ROT_270   = 0x07,
    /* rotate source image 90 degrees counter clockwise, then do flip vertically */
    ZX_TRANSFORM_90_FLIP_H = 0x05,
    /* rotate source image 90 degrees counter clockwise, then do flip horizontally */
    ZX_TRANSFORM_90_FLIP_V = 0x06,
};

typedef enum
{
    sync_type_mutex            = 1,
    sync_type_semaphore        = 2,
    sync_type_fence            = 3,
    sync_type_cpu_notification = 4,
    sync_type_dma_fence        = 5,
}sync_type_t;

typedef struct
{
    unsigned int       device;             /* in */
    unsigned int       sync_object;        /* in */
    unsigned int       status;             /* out*/
}zx_get_sync_status_t;

#define ZX_SYNC_OBJ_ERROR                -1
#define ZX_SYNC_OBJ_INVALID_ARGU         -2
#define ZX_SYNC_OBJ_ALREAD_SIGNALED       1
#define ZX_SYNC_OBJ_TIMEOUT_EXPIRED       2
#define ZX_SYNC_OBJ_CONDITION_SATISFIED   3
#define ZX_SYNC_OBJ_WAIT_ON_SERVER        4
#define ZX_SYNC_OBJ_UNSIGNALED            5

typedef struct zx_signal_sync_object
{
    unsigned int num_context;
    unsigned int sync_object_cnt;
    zx_ptr64_t context_list;
    zx_ptr64_t sync_object_list;
}zx_signal_sync_object_t;

typedef struct
{
    unsigned int        device;
    unsigned int        num_allocations;
    zx_ptr64_t          allocation_list;
    zx_ptr64_t          priority_list;
}zx_set_allocation_priority_t;

typedef union
{
    struct
    {
        unsigned int can_not_trim_further :1;
        unsigned int must_succeed         :1;
        unsigned int reserved             :30;
    };
    unsigned int value;
} zx_resident_flag_t;

typedef struct
{
    unsigned int                device;
    unsigned int                num_allocations;
    zx_resident_flag_t          flags;
    unsigned int                pad;
    zx_ptr64_t                  allocation_list;
    zx_ptr64_t                  priority_list;
    unsigned long long          fence_value;//out
    unsigned long long          trim_size;//out
}zx_make_resident_t;


typedef union
{
    struct
    {
        unsigned int evict_if_only_necessary :1;
        unsigned int not_written_to          :1;
        unsigned int reserved                :30;
    };
    unsigned int value;
} zx_evict_flag_t;


typedef struct
{
    unsigned int              device;
    unsigned int              num_allocations;
    zx_evict_flag_t           flags;
    unsigned int              pad;
    zx_ptr64_t                allocation_list;
    unsigned long long        trim_size; //out: if the value is non-zero, indicates how much the app should evict more to meet its current memory budget.
} zx_evict_t;


typedef union
{
    struct
    {
        unsigned int write :1;
        unsigned int execute :1;
        unsigned int zero :1;
        unsigned int no_access :1;
        unsigned int system_use_only :1;
        unsigned int reserved :27;
    };
    unsigned int value;
} zx_map_gpu_flag_t;

typedef struct
{
    unsigned int        device;
    unsigned int        allocation;
    zx_map_gpu_flag_t   protection_flag;
    unsigned int        pad;
    unsigned long long    size;
    unsigned long long    base_addr;
    unsigned long long    min_addr;
    unsigned long long    max_addr;
    unsigned long long    offset;
    unsigned long long    driver_protection;

    unsigned long long  gpu_va;//out
    zx_ptr64_t             fence_value; //out
}zx_map_gpu_virtual_address_t;

typedef struct
{
    unsigned int device;
    unsigned int pad;
    unsigned long long base;
    unsigned long long size;
}zx_free_gpu_virtual_address_t;

typedef union
{
    struct
    {
        unsigned int no_access :1;
        unsigned int zero      :1;
        unsigned int no_commit :1;
        unsigned int reserved  :29;
    };
    unsigned int value;
} zx_reserve_gpu_va_type_t;

typedef struct
{
    unsigned int         device;
    zx_reserve_gpu_va_type_t type;
    unsigned long long   base;
    unsigned long long   min;
    unsigned long long   max;
    unsigned long long   size;//size in byte
    unsigned long long   driver_protection;
    zx_ptr64_t          gpu_va;//out
} zx_reserve_gpu_virtual_address_t;

typedef enum RESERVED_RESOURCE_OPERATION_TYPE
 {
     RESERVED_RESOURCE_MAP   = 0,
     RESERVED_RESOURCE_UNMAP = 1,
     RESERVED_RESOURCE_COPY  = 2,
     RESERVED_RESOURCE_MAP_PROTECT = 3,
 }RESERVED_RESOURCE_OPERATION_TYPE;

typedef struct zx_update_sparse_mapping_info
{
    RESERVED_RESOURCE_OPERATION_TYPE op_type;
    unsigned int       pad2;
    union
    {
        struct
        {
            unsigned int       allocation;
            unsigned int       pad;
            unsigned long long tile_gpu_va;
            unsigned long long tile_size;//tile resource
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
} zx_update_sparse_mapping_info_t;

typedef union
{
    struct
    {
        unsigned int do_not_wait  :1;
        unsigned int reserved     :1;
    };
    unsigned int value;
} zx_update_gpu_va_flags_t;

typedef struct
{
    unsigned int context;
    unsigned int fence_obj;
    unsigned int num_operation;
    zx_update_gpu_va_flags_t flags;
    zx_ptr64_t  ops;//zx_update_sparse_mapping_info_t
    unsigned long long  fence_value;
} zx_update_gpu_virtual_address_t;

/*now only have perf mode, and only change dvfs clamp
slice switch will default use force two slice since power not saving muchand perf increase
*/

typedef enum
{
    ZX_POWER_MODE_PERF       = 0,   //force two slice, eclock can force
    ZX_POWER_MODE_SAVING     = 1,   //slice switch auto, eclock can force
    ZX_POWER_MODE_VIDEO      = 2,   //can use to force vcp/cpp 450M for 4k*2k video
    ZX_POWER_MODE_BALANCE    = 3,
    ZX_POWER_MODE_AGGRESSION = 4,
    ZX_POWER_MODE_NUM        = 5,
}zx_power_mode;

#define zx_align_down(val, align)  ((val) & ~((align) - 1))
#define zx_align_up(val, align)    (((val) + ((align) - 1)) & ~((align) - 1))

typedef enum
{
    FF_CACHE_INVALIDATE_KER                             , // All FF cache invalidate
    WBU_D_CACHE_INVALIDATE_KER                          , // D cache invalidate normal
    WBU_D_CACHE_INVALIDATE_2D_ONLY_KER                  , // D cache invalidate 2D only
    ZL2_Z_CACHE_INVALIDATE_KER                          , // Zl2 z cache invalidate
    ZL2_S_CACHE_INVALIDATE_KER                          , // Zl2 s cache invalidate
    UAV_FE_CACHE_INVALIDATE_KER                      , // UAV FE cache invalidate
    UAV_BE_CACHE_INVALIDATE_KER                         , // UAV BE cache invalidate
    FF_CACHE_FLUSH_KER                                  , // All FF cache flush
    WBU_D_CACHE_FLUSH_KER                               , // D cache flush Normal
    WBU_D_CACHE_FLUSH_2D_ONLY_KER                       , // D cache flush 2D only
    ZL2_Z_CACHE_FLUSH_KER                               , // Zl2 z cache flush
    ZL2_S_CACHE_FLUSH_KER                               , // Zl2 s cache flush
    UAV_FE_CACHE_FLUSH_KER                              , // UAV FE cache flush
    UAV_BE_CACHE_FLUSH_KER                              , // UAV BE cache flush
    FB_CACHE_FLUSH_KER                                  , // Flag buffer cache flush
    HW_TRIGGER_CMDTYPE_LAST_KER
} HW_TRIGGER_CMDTYPE;

#define ZX_HDCP_ENABLED            0x0
#define ZX_HDCP_FAILED             0x1
#define ZX_HDCP_NO_SUPPORT_DEVICE  0x2

typedef struct zx_query_residency
{
    unsigned int        device;
    unsigned int        num_allocations;
    zx_ptr64_t          allocations;
    zx_ptr64_t          residency_status;
}zx_query_residency_t;

typedef struct
{
    unsigned int handle;    // in
    unsigned int offset;    // in
    unsigned int size;      // in
    unsigned int readonly:1;// in
} zx_drm_gem_begin_cpu_access_t;

typedef struct
{
    unsigned int handle;
} zx_drm_gem_end_cpu_access_t;

typedef struct
{
    unsigned int prefault_num;
    unsigned int delay_map;
    unsigned long long gem_handle;
    unsigned long long offset;
} zx_drm_gem_map_t;

#define ZX_TEST_PAGING_IN   1
#define ZX_TEST_PAGING_OUT  2
#define ZX_TEST_SUSPEND     3
#define ZX_TEST_RESUME      4
typedef struct
{
    unsigned int type;
    unsigned int device;
    union {
        unsigned int v_uint;
        unsigned long long v_long;
        void *v_ptr;
    };
} zx_test_t;

typedef struct
{
    unsigned int adapter_index;
    unsigned int alignment;
    unsigned int size;

    /* exactly is MM_SEGMENT_PREFERENCE union value. */
    unsigned int preferred_segment;
    unsigned int maximumRenamingListLength;

    unsigned int cpu_visible : 1;
    unsigned int secured     : 1;
    unsigned int cachable    : 1;
    unsigned int reserved    : 29;

}zx_cil2_allocation_desc_t;

typedef struct zx_drm_syncobj_create{
    unsigned int handle;
#define ZX_DRM_SYNCOBJ_CREATE_SIGNALED (1 << 0)
    unsigned int flags;
} zx_drm_syncobj_create_t;

typedef struct zx_drm_syncobj_destroy{
    unsigned int handle;
    unsigned int pad;
} zx_drm_syncobj_destroy_t;

#define ZX_DRM_SYNCOBJ_WAIT_FLAGS_WAIT_ALL (1 << 0)
#define ZX_DRM_SYNCOBJ_WAIT_FLAGS_WAIT_FOR_SUBMIT (1 << 1)
#define ZX_DRM_SYNCOBJ_WAIT_FLAGS_WAIT_AVAILABLE (1 << 2)
typedef struct zx_drm_syncobj_wait{
    unsigned long long handles;
    long long timeout_nsec;
    unsigned int count_handles;
    unsigned int flags;
    unsigned int first_signaled;
    unsigned int pad;
} zx_drm_syncobj_wait_t;

typedef struct zx_drm_syncobj_timeline_wait{
    unsigned long long handles;
    unsigned long long points;
    long long timeout_nsec;
    unsigned int count_handles;
    unsigned int flags;
    unsigned int first_signaled;
    unsigned int pad;
} zx_drm_syncobj_timeline_wait_t;

typedef struct zx_drm_syncobj_array{
    unsigned long long handles;
    unsigned int count_handles;
    unsigned int pad;
} zx_drm_syncobj_array_t;

typedef struct zx_drm_syncobj_status{
    unsigned int handle;
    int value;
}zx_drm_syncobj_status_t;

typedef int (*FillAllocationDescription)(void *private_data, unsigned int data_size, zx_cil2_allocation_desc_t *desc);


#define ZX_MISC_GET_3DBLT_CODE  (1)
typedef struct
{
    unsigned int size;          // output
    unsigned int capacity;      // output
    unsigned long long base;    // output
} zx_misc_get_3dblt_code_t;

#define ZX_MISC_SET_3DBLT_CODE  (2)
typedef struct
{
    unsigned int size;          // input
    unsigned int pad;           // for alignment
    zx_ptr64_t data;            // input
} zx_misc_set_3dblt_code_t;

#define ZX_MISC_MAP_GPUVA       (3)
typedef struct
{
    unsigned int allocation;    // input
    unsigned int pad;           // for alignment
    unsigned long long gpu_va;  // input
} zx_misc_map_gpuva_t;

#define ZX_MISC_VIDEO_FENCE_GET (4)
typedef struct
{
    unsigned int index;         // input
    unsigned int fence;         // output
} zx_misc_video_fence_get_t;

#define ZX_MISC_VIDEO_FENCE_CLEAR (5)
typedef struct
{
    unsigned int index; // input
} zx_misc_video_fence_clear_t;

#define ZX_MISC_MMIO_WRITE      (6)
#define ZX_MISC_MMIO_READ       (7)
typedef struct
{
    unsigned int address;
    unsigned int value;
} zx_misc_mmio_t;

#define ZX_MISC_UPDATE_HEAP_SLOT (8)
typedef struct
{
    unsigned int delFlag;
    unsigned int size;
    unsigned int bl_slot_index;
}zx_misc_update_bl_slot_t;

#define ZX_MISC_SET_HEAP_BL_INFO (9)
typedef struct
{
    unsigned int range_type;
    unsigned int bl_slot_index;
}zx_misc_set_bl_info_t;

#define ZX_MISC_GET_HEAP_BL_INFO (10)
typedef struct
{
    unsigned int range_type;
    unsigned int bl_slot_index;
}zx_misc_get_bl_info_t;


typedef struct
{
    unsigned int op_code;
    unsigned int device;
    unsigned int context;
    unsigned int handle;
    union {
        zx_misc_get_3dblt_code_t    get_3dblt_code;
        zx_misc_set_3dblt_code_t    set_3dblt_code;
        zx_misc_map_gpuva_t         map_gpuva;
        zx_misc_video_fence_get_t   video_fence_get;
        zx_misc_video_fence_clear_t video_fence_clear;
        zx_misc_mmio_t              mmio;
        zx_misc_update_bl_slot_t    update_bl_slot;
        zx_misc_set_bl_info_t       set_bl_info;
        zx_misc_get_bl_info_t       get_bl_info;
    };
} zx_cil2_misc_t;

#endif /* __ZX_TYPES_H__*/

