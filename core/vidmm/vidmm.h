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
#ifndef __VIDMM_H__
#define __VIDMM_H__

#include "list.h"
#include "context.h"
#include "zx_def.h"
#include "kernel_interface.h"

//Burst length buffer
#define BL_BUFFER_SIZE         (32*1024*1024)//reserve 32M for 16G
//from spec, current bl index bit using 18bit plan.
#define BL_INDEX_BITS          18
//from spec, 1byte slot map to 512byte allocation
#define BL_MAPPING_RATIO       512

#define BL_SLOT_SIZE           (BL_BUFFER_SIZE >> BL_INDEX_BITS)
#define BL_SLICE_ALIGNMENT     (BL_BUFFER_SIZE >> BL_INDEX_BITS)

#define SEGMENT_ID_INVALID    0x0
#define SEGMENT_ID_LOCAL        0x1
#define MAX_FB_PHYSICAL_MEMORY_HEAP_ID        0xF
#define MAX_APERTURES         0x10

#define FLAG_BUFFER_RANGE_NUM 1

#define SECURE_RANGE_CONFIG_SECURE_ON          1
#define SECURE_RANGE_CONFIG_SECURE_OFF         2
#define SECURE_RANGE_CONFIG_HANG_DUMP          3

#define GPU_PTE_SIZE     4

#define ENABLE_SHARE_LOCAL_SCATTER  0

typedef enum
{
    VM_ZONE_ID_SHARE_SPACE_LOCAL = 0,
    VM_ZONE_ID_SHARE_SPACE_PCIE  = 1,
    VM_ZONE_ID_PRIVATE_SPACE     = 2,
    VM_ZONE_ID_MAX               = 3,
}VM_ZONE_ID;

//note: this priority need to be consistent with the macro
typedef enum _ALLOCATION_PRIORITY
{
    PDISCARD  =  0,
    PLOW,
    PNORMAL,
    PHIGH,
    PMAX,
    PALL,
} ALLOCATION_PRIORITY;

typedef struct _vidmm_segment_preference
{
    union
    {
        struct
        {
            unsigned int segment_id :5;
            unsigned int direction  :1;
        };
        unsigned char value;
    };
} vidmm_segment_preference_t;

#define ARRAY_SIZE(x) (sizeof(x)/sizeof(x[0]))


typedef union vidmm_segment_desc_flags
{
    struct
    {
        unsigned int local                 :1;
        unsigned int secure_range          :1;
        unsigned int auto_secure           :1;
        unsigned int secure_in_use         :1;
        unsigned int support_snoop         :1;
        unsigned int support_manual_flush  :1;
        unsigned int support_page_64kb     :1;
        unsigned int mtrr                  :1; //cpu side feature, if this flag set, we can set this GPU memory range as wc by add_mtrr.
        unsigned int invalid               :1;
        unsigned int require_system_pages  :1;
        unsigned int reserved              :22;
    };
    unsigned int value;
}vidmm_segment_desc_flags_t;


/*
the segment concept is used to describe the related range for physical memory. one segment in a vidmm_mgr
can be used to summary the allocation usage in this segment.
for the fb/local memory, the related segment include the detail range information; the pcie segment no needs these
information, and only be the summaried function for vidmm_mgr.
*/
typedef struct vidmm_segment
{
    unsigned int                segment_id;
    vidmm_segment_desc_flags_t  flags;
    unsigned int                property;
    struct os_mutex            *lock;

    //below items are used for memory usage summary.
    unsigned long long          used_size;// total size of the allocation/segment_memory in this segment
    unsigned long long          used_num;// total number of the allocation/segment_memory in this segment
    unsigned long long          used_segment_memory_size;// total size of the segment_memory in this segment
    struct list_head            pagable_resident_list[PALL];
    struct list_head            unpagable_resident_list;


    //below information only used for the fb segment related.
    unsigned long long          start_addr;
    unsigned long long          size;
    unsigned int                page_size;
    unsigned long long          phys_addr_start;
    struct os_mutex            *secure_lock;
    struct range_allocator      *mem_ra;

}vidmm_segment_t;

typedef struct _vidmm_allocation_flag
{
    union
    {
        struct
        {
            unsigned int pool_type      : 2;
            unsigned int cache_type     : 2;
            unsigned int global         : 1;
            unsigned int unpagable      : 1;
            unsigned int secured        : 1;
            unsigned int tiled          : 1;
            unsigned int force_clear    : 1;
            unsigned int continuous     : 1;
            unsigned int cpu_map        : 1;

            // the follow 3 bits was umd request attributes, not the really allocation's attributes
            unsigned int cpu_visible    : 1;
            unsigned int bVideoInternal : 1;//video only
            unsigned int need_save_when_acpi    :1;
            unsigned int reserved       : 18;
        };
        unsigned int value;
    };
} vidmm_allocation_flag_t;

#define RENAME_NODE_NEW_ALLOCATION           0
#define RENAME_NODE_IN_REFERENCE_LIST        1
#define RENAME_NODE_IN_UNREFERENCE_LIST      2

typedef struct _vidmm_allocation_status
{
    // this indicate the allocation can't be paging...
    unsigned int temp_unpagable;

    union
    {
        struct
        {
            unsigned int    need_restore            : 1; /* means local video memory saved to pages_mem, used restore when resume */
            unsigned int    reserved1               : 31;
        };
        unsigned int value2;
    };
    union
    {
        struct
        {
            unsigned int destroy_from_evict        : 1; // this allocation's destroy is triggered by the evict operation.
            unsigned int reserved2                 : 31;
        };
        unsigned int op_status;
    };
} vidmm_allocation_status_t;


typedef struct vidmm_vm_reserve_gpu_va_arg
{
    unsigned long long   base;
    unsigned long long   min;
    unsigned long long   max;
    unsigned long long   size;//size in byte
    union
    {
        struct
        {
            unsigned int no_access :1;
            unsigned int zero      :1;
            unsigned int no_commit :1;
            unsigned int reserved  :29;
        };
        unsigned int value;
    }type;

    unsigned long long   driver_protection;

    unsigned int         alignment;

}vidmm_vm_reserve_gpu_va_arg_t;

typedef krnl_update_gpu_va_t vidmm_vm_update_gpu_va_t;

typedef struct vidmm_primary_allocation_sync
{
    unsigned long long    fence_id[MAX_ENGINE_COUNT];
    unsigned long long    write_fence_id[MAX_ENGINE_COUNT];
    int                   flip_count[MAX_CORE_CRTCS];
    int                   render_count[MAX_ENGINE_COUNT];//notify the render count of the allocation. deprecated for touch primary issue?
    int                   write_render_count[MAX_ENGINE_COUNT];//notify the write render count of the allocation. deprecated for touch primary issue?
}vidmm_primary_allocation_sync_t;

typedef enum
{
    ZX_PAGE_TYPE_NORMAL = 0,
    ZX_PAGE_TYPE_LARGE  = 1,
    ZX_PAGE_TYPE_MAX    = 2,
}ZX_PAGE_TYPE;

typedef enum
{
    ZX_PT_LEVEL_ROOT = 0,
    ZX_PT_LEVEL_1    = 1,
    ZX_PT_LEVEL_2    = 2,
    ZX_PT_LEVEL_MAX  = 3,
}ZX_PT_LEVEL;



typedef struct zx_vma_space zx_vma_space_t;

typedef union zx_vma_desc_flags
{
    struct
    {
        unsigned int secure         :1; // support secure va.
        unsigned int first_init     :1;//the vma be first init
        unsigned int cpu_update_pte :1;//use cpu to update the pte
        unsigned int reserved       :29;
    };
    unsigned int value;
} zx_vma_desc_flags_t;


typedef union zx_vm_zone_desc_flags_t
{
    struct
    {
        unsigned int share               :1;
        unsigned int reserved            :31;
    };
    unsigned int value;
} zx_vm_zone_desc_flags_t;


/*
describe one functional part of one vma space, used to allocate the vm node.
each vm_zone normaly has different physical memory type, like system memory, on-chip memory or on-board reserved memory by bios.
the vm node in vm zone is organized as heap, as below layout:
  |<--------------------- gpu_vm_size ------------------->|
  |<--reserved_vm_size-->|
  +----------------------+--------------------------------+
  |       reserved_vmem  |              heap              |
  +----------------------+--------------------------------+
  ^                      ^
  gpu_vm_start          reserved_vm_end
*/
typedef struct zx_vm_zone
{
    unsigned int                zone_id;//the zone index in zone array.
    zx_vma_space_t              *vma_space;//the vma space of this zone belong to.

    zx_vm_zone_desc_flags_t     flags;

    unsigned long long          gpu_vm_start;// the start virtual address of the zone.
    unsigned long long          gpu_vm_size;//the size of the VM range of this zone.
    unsigned long long          reserved_vm_end;//the end address of the reserved area for one zone.
    unsigned long long          reserved_vm_size;//the size of the reserved area for one zone.
    struct os_mutex             *lock;

    struct range_allocator *ra;         // the VM address space pool.
} zx_vm_zone_t;


typedef struct zx_vm_zone_desc
{
    unsigned int                  zone_id;
    zx_vm_zone_desc_flags_t       flags;
    unsigned long long            gpu_vm_start;
    unsigned long long            gpu_vm_size;
    unsigned long long            reserved_vm_start;
    unsigned long long            reserved_vm_size;
    unsigned long long            phys_addr_start;
} zx_vm_zone_desc_t;

typedef struct vidmm_chip_vm_info
{
    unsigned int         zone_cnt;
    zx_vm_zone_desc_t    desc[VM_ZONE_ID_MAX];
    unsigned int         max_pfn;
    unsigned int         pt_levels_num;
    unsigned int         pt_bits_array[ZX_PAGE_TYPE_MAX][ZX_PT_LEVEL_MAX];
    unsigned long long   pt_bits_mask[ZX_PAGE_TYPE_MAX][ZX_PT_LEVEL_MAX];
    unsigned int         pt_segment_id;//for normal page table buffer.
    unsigned int         share_pt_segment_id;//for shared page table buffer.
    unsigned long long   share_level3_gpu_va_offset;
    unsigned int        *share_level3_cpu_va_offset;//the level3 page table cpu va in shared page table buffer.
    unsigned int        *share_level1_cpu_va_offset;//the level1 page table cpu va in shared page table buffer.
}vidmm_chip_vm_info_t;

typedef enum
{
    ZX_VM_NODE_TYPE_ALLOCATION = 0,
    ZX_VM_NODE_TYPE_SEGMENET_MEMORY = 1,
    ZX_VM_NODE_TYPE_SPARSE_RESOURCE = 2,
}ZX_VM_NODE_TYPE;

typedef struct vidmm_vm_update_ptes_arg
{
    void                 *dma_buffer; //the dma buffer for update pte operation.
    unsigned int          pt_level; //indicate the pt level, each pt level has its own pte definition.
    unsigned long long    dst_pte_page_va; //the entry page's va of the start pte to be filled.
    unsigned long long    cur_buf_va_on_fill; //the first pte's related VA.
    unsigned long long    buf_start_va; //the start va of vm node.
    unsigned long         npte; // the count of pte to be updated.
    void                 *entity; // the entity of the page table mapping to.
    unsigned int          pte_offset; // the start offset of pte in the page.
    ZX_VM_NODE_TYPE       entity_type;// the type of the entity.
    adapter_t            *adapter;
    unsigned long long   entity_offset;
} vidmm_vm_update_ptes_arg_t;

typedef struct vidmm_vm_transform_pt_arg
{
    void                 *dma_buffer;
    void                 *entity;
    unsigned int          entity_offset;
    ZX_VM_NODE_TYPE      entity_type;
    unsigned int          pt_index;
}vidmm_vm_transform_pt_arg_t;

typedef struct zx_sparse_node
{
    struct range_node *range;
    struct range_allocator *ra;
} zx_sparse_node_t;

typedef struct zx_vm_node
{
    struct list_head              mapped_list_item;

    int                           age;

    //mapped entity starting page.
    unsigned long long            offset;

    // the vma zone of this node belong to.
    zx_vm_zone_t                *zone;

    // the device of this node belong to, maybe different with zone->device for share allocation
    gpu_device_t                *device;

    struct range_node           *range;

    // the mapped physical memory entity type, allocation or segment memory.
    ZX_VM_NODE_TYPE              mapped_entity_type;

    // the mapped physical memory entity.
    void                         *mapped_entity;

} zx_vm_node_t;


typedef struct fb_pages_memory
{
    struct range_node **range_node_list;//for heap manager
    int range_node_num;
    unsigned long long size;
} fb_pages_memory_t;

typedef struct vidmm_segment_memory
{
    struct list_head        list_item;
    unsigned int            segment_id;

    unsigned long long      gpu_va;
    zx_vm_node_t            *gpu_vm;
    unsigned long long      size;
    unsigned int            aligned_page_size;//for pcie mem, means os page size, for local mem, means 64K or 4k

    struct os_pages_memory *pages_mem;
    struct fb_pages_memory *fb_pages_mem;

    struct
    {
        unsigned int snooping_enabled :1;
        unsigned int security         :1;
        unsigned int local            :1;
        unsigned int reserved         :29;
    }flag;

    zx_cpu_vm_area_t        *krnl_cpu_vma;
} vidmm_segment_memory_t;


/*
the structure of one page table (directory)
*/
typedef struct vidmm_vm_pt
{
    vidmm_segment_memory_t  *buffer;//the storage buffer for the page table
    unsigned long long       addr;//the gpu address of this page entry, inherited from the buffer
    unsigned long           *update_bitmap;// used to identify the dirty (in used) pte idx of the page directory (table), combined use with the entry array.
    struct vidmm_vm_pt      *entries;//the array of page tables that points to the next level's page table.
    struct vidmm_vm_pt      *parent;//the parent's entry
    unsigned int             large_page;// this pt is large page or not, init value is 0, and will be updated when map a buffer, and only ZX_PT_LEVEL_2 will use this bit
    int age;                        // used for suspend/resume case, pd must reset after resot
} vidmm_vm_pt_t;


/*
describe the information of the page table(directory) of one vma space.
*/
typedef struct zx_vm_pt_info
{
    vidmm_vm_pt_t       root;//the root page table for the vma space
} zx_vm_pt_info_t;

/*
describe the vma space of one device/process.
*/
struct zx_vma_space
{
    adapter_t                   *adapter;
    zx_vma_desc_flags_t         flag;
    unsigned int                zone_cnt;// the VM zone goup count
    zx_vm_zone_t               *zones[VM_ZONE_ID_MAX];
    zx_vm_pt_info_t             pt_info; //describe the page table information per adapter

    gart_dirty_t               utlb_dirtys[MAX_ENGINE_COUNT];
    gart_dirty_t               dtlb_dirtys[MAX_ENGINE_COUNT];

    long long                   root_pd_age;    // increase when root pd update
    struct os_mutex             *lock;
};

typedef int (*pfn_update_pte)(zx_vma_space_t* vma_space, int engine_index, vidmm_vm_update_ptes_arg_t *update_pte_arg);

typedef struct _vidmm_allocation
{
    struct list_head                   list_item; //  item may in segment pagable list, unpagable list, accord allocation status
    struct list_head                   gpu_vm_list;//mapped gpu vm node list, currently only one mapped item

    struct os_mutex                    *paging_lock;

    int                                resident_age;    // inc when page changed, for example paging in
                                                        // need update ptes when resident_node->resident_age < allocation->resident_age
    adapter_t                          *adapter;
    unsigned int                       alignment;
    unsigned long long                 aligned_page_size;//for PTE calculate, should be 64K/4k /os page size
    unsigned long long                 size;
    unsigned int                       priority;

    vidmm_segment_preference_t         preferred_segment[3];
    unsigned int                       segment_id;//segment id of the allocation's physical memory

    unsigned int                       need_remap;

    vidmm_allocation_flag_t            flag;

    zx_cpu_vm_area_t                  *krnl_cpu_vma;

    unsigned long long                 cpu_phys_addr;

    //todo: add interface to make a transfer from fb_pages_mem to pages_mem
    struct os_pages_memory            *pages_mem;//allocation' memory resident in system pages.
    struct fb_pages_memory            *fb_pages_mem; //fb_pages_memory block array.

    void                              *file_storage;

    unsigned long long                 last_paging;//last paging fence id
    vidmm_allocation_status_t          status;

    struct os_spinlock                *lock;
    struct os_mutex                   *va_lock;//va related lock, like map, update, release

    struct range_node                  *bl_range;
    unsigned int                       slot_index; //out.compress only: burst length slot
    unsigned int                       compress_format;//in.compress only: range type

    void                              *parent;//attach gem object
    unsigned int                       debug_gid;  // created in OS layer, only for debug
    struct os_pages_memory            *(*get_system_pages)(void *gemObj, void *pdev, unsigned long long size, alloc_pages_flags_t *flags);
    void                              (*release_system_pages)(void *pdev, struct os_pages_memory *pages_mem);
} vidmm_allocation_t;

typedef struct vidmm_sparse_resource
{
    gpu_device_t      *device;
    unsigned int       handle;
    unsigned long long size;
    unsigned long long gpu_va;
    zx_vm_node_t     *gpu_vm;
}vidmm_sparse_resource_t;

typedef struct vidmm_map_desc_args
{

    unsigned long long   size;//the mapped size, PAGE_SIZE aligned.
    unsigned long long   base_addr;
    unsigned long long   min_addr;
    unsigned long long   max_addr;
    unsigned long long   offset;// the starting page of the specified allocation, PAGE_SIZE aligned.
    unsigned long long   protection;
    unsigned int         force_base;//if use the force base, the base must be satisfied, or return E_FAIL.
    unsigned int         is_paging;//indicate the allocation is paging
    unsigned int         is_sparse;//the mapping for sparse
    unsigned long long   out_va;//the mapped gpu va
}vidmm_map_desc_args_t;

typedef struct vidmm_make_resident_arg
{
    unsigned int          num_allocations;
    unsigned long long   *allocation_list;
    unsigned int         *priority_list;//in: residency operation priority of the residency allocations.
    unsigned long long    fence_value;//out: the paging queue returned fence
    unsigned long long    trim_size;// out: the trim size for umd should do the evict when resident failed.
    union
    {
        struct
        {
            unsigned int can_not_trim_further :1; //todo.
            unsigned int must_succeed         :1;
            unsigned int reserved             :30;
        };
        unsigned int value;
    }flags;
} vidmm_make_resident_arg_t;


typedef struct vidmm_evict_arg
{
    unsigned int          num_allocations;
    unsigned long long   *allocation_list;
    unsigned long long    trim_size; //out: if the value is non-zero, indicates how much the app should evict more to meet its current memory budget.
    union
    {
        struct
        {
            unsigned int evict_if_only_necessary :1;
            unsigned int not_written_to          :1;
            unsigned int reserved                :30;
        };
        unsigned int value;
    }flags;
}vidmm_evict_arg_t;

typedef struct vidmm_paging_allocation
{
    unsigned long long  size;
    unsigned int        swap_out;
    unsigned int        success;

    vidmm_allocation_t *allocation;
    vidmm_allocation_t *temp_allocation_src;
    vidmm_allocation_t *temp_allocation_dst;

    unsigned long long src_gpu_va;
    unsigned long long dst_gpu_va;
}vidmm_paging_allocation_t;

typedef struct vidmm_vm_create_desc_args
{
    unsigned long long prefer_start_addr;
    unsigned long long size;
    unsigned long long offset;
    unsigned int       force_start;
}vidmm_vm_create_desc_args_t;

typedef enum
{
    ZX_ROOT_PT_UPDATE_MODE_MMIO = 0,
    ZX_ROOT_PT_UPDATE_MODE_DMA  = 1,
}ZX_ROOT_PT_UPDATE_MODE;

typedef enum
{
    PAGING_RESIDENT_LIST = 0,
    PAGING_EVICT_LIST    = 1,
    PAGING_SYSTEM_LIST   = 2,
    PAGING_SWAPOUT_LIST  = 3,
}VIDMM_PAGEING_LIST_TYPE;

typedef struct bl_slot{
    struct list_head list_node;
    struct range_node *bl_node;
}bl_slot_t;

extern int  vidmm_init(adapter_t *adapter, int reserved_mem);
extern void vidmm_destroy(adapter_t *adapter);
extern void vidmm_save(adapter_t *adapter);
extern void vidmm_restore(adapter_t *adapter);
extern int vidmm_save_allocation(adapter_t *adapter, vidmm_allocation_t *allocation);
extern int vidmm_restore_allocation(adapter_t *adapter, vidmm_allocation_t *allocation);
extern void*  vidmm_create_allocation(adapter_t *adapter, krnl_create_allocation_info_t *data);
extern void vidmm_destroy_allocation(vidmm_allocation_t *allocation, zx_destroy_flag_t *flags);
extern int vidmm_unresident_allocations(adapter_t *adapter, gpu_device_t *device, gpu_context_t *context);
extern int vidmm_query_info(adapter_t *adapter, zx_query_info_t *info);
extern vidmm_segment_t* vidmm_get_segment_by_id(adapter_t *adapter, unsigned int segment_id);
extern vidmm_segment_t *vidmm_get_segment_by_property(adapter_t *adapter, unsigned int property);
extern int vidmm_get_segment_count(adapter_t *adapter);
extern vidmm_segment_memory_t *vidmm_create_segment_memory(adapter_t *adapter, int segment_id, unsigned long long size, int direction, int continous);
extern void vidmm_destroy_segment_memory(adapter_t *adapter, vidmm_segment_memory_t *segment_memory);
extern int vidmm_map_segment_memory(adapter_t *adapter, vidmm_segment_memory_t *segment_memory);
extern void vidmm_unmap_segment_memory(vidmm_segment_memory_t *segment_memory);
extern unsigned long long vidmm_get_segment_memory_gpuva_cont(vidmm_segment_memory_t *segment_memory, unsigned int page_index);
extern unsigned long long vidmm_get_segment_memory_pa_cont(vidmm_segment_memory_t *segment_memory, unsigned int page_index);
extern void *vidmm_get_segment_memory_cpu_va(vidmm_segment_memory_t *segment_memory);
extern void* vidmm_lock_segment_memory(adapter_t *adapter, vidmm_segment_memory_t *segment_memory);
extern void vidmm_unlock_segment_memory(adapter_t *adapter, vidmm_segment_memory_t *segment_memory);
extern void vidmm_release_temp_paging_allocation(adapter_t *adapter, void *ptask);
extern zx_cpu_vm_area_t *vidmm_get_current_user_vma(gpu_device_t *device, zx_cpu_vm_area_t *prev, int remove);

extern void vidmm_add_allocation_to_paging_list(gpu_device_t *device, vidmm_allocation_t *allocation, VIDMM_PAGEING_LIST_TYPE list_type);
extern void vidmm_remove_allocation_from_paging_list(gpu_device_t *device, vidmm_allocation_t *allocation, VIDMM_PAGEING_LIST_TYPE list_type);


extern int vidmm_get_map_allocation_info(adapter_t *adapter, vidmm_allocation_t *allocation, zx_map_argu_t *map);

extern int vidmm_vm_map_gpu_virtual_address(gpu_device_t *device, vidmm_allocation_t *allocation, vidmm_map_desc_args_t *arg);

extern int vidmm_unlock_allocation(vidmm_allocation_t *allocation);
extern int vidmm_lock_allocation(gpu_device_t *device, vidmm_allocation_t *allocation);

extern void vidmm_add_allocation_to_segment_resident_list(adapter_t *adapter, vidmm_allocation_t *allocation);
extern void vidmm_remove_allocation_from_segment_resident_list(adapter_t *adapter, vidmm_allocation_t *allocation);

//vm related interface
extern int vidmm_vm_init_vma_space(zx_vma_space_t *vma_space, adapter_t *adapter);
extern void vidmm_vm_fini_vma_space(zx_vma_space_t *vma_space);
extern int vidmm_vm_allocate_segment_memory(adapter_t *adapter, vidmm_segment_memory_t *segment_memory);
extern void vidmm_vm_release_segment_memory(vidmm_segment_memory_t *segment_memory);
extern void vidmm_vm_release_allocation(vidmm_allocation_t *allocation);
extern void vidmm_vm_free_gpu_virtual_address(zx_vma_space_t *vma_space, unsigned long long base, unsigned long long size);
extern int vidmm_vm_reserve_gpu_virtual_address(gpu_device_t *device, vidmm_vm_reserve_gpu_va_arg_t *reserve_arg, unsigned long long *gpu_va);
extern int vidmm_vm_update_gpu_virtual_address(gpu_context_t *comp_context, vidmm_vm_update_gpu_va_t *update);
extern int vidmm_vm_update_mapping(zx_vma_space_t *vma_space, gpu_context_t *context, zx_vm_node_t *vm_node, vidmm_allocation_t *allocation, void* update_task);
extern void vidmm_vm_clear_engine_dirty_range(zx_vma_space_t *vma_space, int engine_index);
extern int vidmm_vm_alloc_pd(zx_vma_space_t *vma_space, vidmm_vm_pt_t *parent, unsigned long long saddr, unsigned long long eaddr, unsigned int level, int update_private_vma);

extern vidmm_allocation_t *vidmm_fetch_allocation_by_address(gpu_device_t *device, vidmm_allocation_t **allocation_list, unsigned int cnt, unsigned long long base, unsigned long long size, unsigned long long *offset, zx_vm_node_t **pnode);
extern void vidmm_set_allocation_priority(krnl_set_allocation_priority_t *prio);
extern void vidmm_init_memory_budget(adapter_t *adapter, gpu_device_t *device);
extern void vidmm_destroy_memory_budget(gpu_device_t *device);
extern vidmm_allocation_t *vidmm_get_allocation_from_share(adapter_t *adapter, unsigned long long base, unsigned long long size);

extern void vidmm_mark_pagable(vidmm_allocation_t *allocation);
extern int vidmm_prepare_and_mark_unpagable(vidmm_allocation_t *allocation);
extern void vidmm_fill_allocation_info(adapter_t *adapter, vidmm_allocation_t *allocation, zx_allocation_info_t *info);
extern void* vidmm_get_from_gem_handle(adapter_t *adapter, gpu_device_t *device, unsigned int gem_handle);
extern struct range_allocator* vidmm_get_burst_length_allocator(adapter_t *adapter);
extern vidmm_allocation_t* vidmm_validate_allocation(vidmm_allocation_t *allocation);
extern int vidmm_allocation_update_mapping(gpu_device_t *device, gpu_context_t *context, vidmm_allocation_t *allocation, unsigned long long *gpu_va);
extern int vidmm_allocation_list_update_mapping(gpu_device_t *device, gpu_context_t *context, unsigned int list_count, struct _vidmm_allocation  **alloction_list);
extern int vidmm_resident_one_allocation(adapter_t *adapter, vidmm_allocation_t *allocation);
extern int vidmm_unresident_one_allocation(adapter_t *adapter, vidmm_allocation_t *allocation);
extern void vidmm_vm_dump_vma_space(struct os_printer*, adapter_t *adapter, zx_vma_space_t *vma_space);
extern void vidmm_vm_dump_zone(struct os_printer*, zx_vm_zone_t *zone);
extern void vidmm_vm_dump_vm_node_info(struct os_printer *p, vidmm_allocation_t *allocation);
extern void vidmm_dump_resource(struct os_printer *p, adapter_t *adapter);
extern void vidmm_dump_memtrack(struct os_printer *p, adapter_t *adapter, int pid);
extern void vidmm_dump_heap(struct os_printer *p, adapter_t *adapter, int id);
extern void vidmm_vm_dump_address_info(adapter_t *adapter, zx_vma_space_t *vma_space, zx_vm_node_t *vm_node, unsigned long long start_addr, unsigned long long address);

extern void vidmm_update_heap_slot(adapter_t *adapter, krnl_cil2_misc_t *misc);

extern zx_vm_zone_t *vidmm_vm_get_zone_from_range(zx_vma_space_t *vma_space, unsigned long long base, unsigned long long size);
#endif

