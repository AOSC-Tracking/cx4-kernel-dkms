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
#ifndef __ZX_VIDMMI_H__
#define __ZX_VIDMMI_H__

#include "list.h"

//this structure is used for chip-dependent information
typedef struct _vidmm_describe_allocation
{
    vidmm_allocation_t       *allocation;
} vidmm_describe_allocation_t;

typedef enum _build_paging_buffer_operation
{
    BUILDING_PAGING_OPERATION_TRANSFER  = 1,
    BUILDING_PAGING_OPERATION_FILL      = 2,
} build_paging_buffer_operation_t;

typedef struct _vidmm_private_build_paging_buffer_arg
{
    vidmm_allocation_t *allocation;
    void *dma_buffer;
    unsigned int dma_size;
    build_paging_buffer_operation_t operation;
    unsigned int multi_pass_offset;

    union
    {
        struct
        {
            unsigned int    transfer_offset;
            unsigned int    transfer_size;
            struct
            {
                unsigned int segment_id;
                unsigned long long phy_addr;
                unsigned int compress_format;
            } src;
            struct
            {
                unsigned int segment_id;
                unsigned long long phy_addr;
                unsigned int compress_format;
            } dst;
        } transfer;
        struct
        {
            unsigned long long  fill_size;
            unsigned int        fill_pattern;
            struct
            {
                unsigned long long gpu_virt_addr;
                unsigned long long offset;
            };
        } fill;
    };
} vidmm_private_build_paging_buffer_arg_t;



typedef enum
{
    PAGING_IN_SRC  = 0,
    PAGING_IN_DST  = 1,
    PAGING_OUT_SRC = 2,
    PAGING_OUT_DST = 3,
}vidmm_paging_allocation_type;

typedef struct vidmm_unresident_arg
{
    unsigned long long  *allocation_list;
    unsigned int         allocation_list_count;
    int                  must_success;
    int                  implicit;
    unsigned long long  *unresident_size; //out.
}vidmm_unresident_arg_t;

typedef struct _vidmm_chip_func
{
    int  (*describe_allocation)(adapter_t *, vidmm_describe_allocation_t *);
    void (*query_chip_vm_info)(adapter_t*, vidmm_chip_vm_info_t*);
    int  (*build_paging_buffer)(adapter_t*, vidmm_private_build_paging_buffer_arg_t*);
    int  (*init_shared_pt)(adapter_t *);
    void (*dump_ptes)(struct os_printer *p, unsigned int *virt, unsigned int large_page, int level, int index);
    void (*dump_mapped_entity)(adapter_t *adapter, void * mapped_entity, ZX_VM_NODE_TYPE mapped_entity_type, unsigned long long address, unsigned long long start_addr);
    int  (*update_ptes)(zx_vma_space_t*, int engine_index, vidmm_vm_update_ptes_arg_t*);
    int (*map_shared_buffer)(adapter_t *adapter, ZX_VM_NODE_TYPE entity_type, void *buffer, unsigned long long offset, unsigned long long size, unsigned long long gpu_va);
    int  (*clear_bl_buffer)(void *dma, unsigned int dma_size, unsigned int bl_slot_index, unsigned int alloc_size, unsigned long long clear_value);
    unsigned int  (*alloc_bl_slot)(adapter_t *, vidmm_allocation_t *);
    int  (*prepare_and_check_compress)(adapter_t *, vidmm_allocation_t *);
    int  (*free_bl_slot)(adapter_t *, vidmm_allocation_t *);
    void  (*init_bl_heap)(adapter_t *adapter);
    void  (*fini_bl_heap)(adapter_t *adapter);
    void  (*save)(adapter_t *adapter);
    void  (*restore)(adapter_t *adapter);
    void (*init_segments)(adapter_t *adapter);
    void (*fini_segments)(adapter_t *adapter);
    void (*init_private_vma_space)(adapter_t *);
    void (*update_private_vma_space)(adapter_t*, void*, vidmm_vm_pt_t*, unsigned int, unsigned int);
} vidmm_chip_func_t;

typedef struct vidmm_paging_mgr
{
    unsigned int        system_allocation_size;
    unsigned int        system_allocation_num;
    struct list_head    system_allocation_list;

    unsigned int        swapout_allocation_size;
    unsigned int        swapout_allocation_num;
    struct list_head    swapout_allocation_list;
} vidmm_paging_mgr_t;


typedef struct _vidmm_mgr
{
    adapter_t                   *adapter;

    vidmm_chip_vm_info_t        *vm_info;

    vidmm_paging_mgr_t          *paging_mgr;

    unsigned int                reserved_vmem;
    unsigned long               max_pages_num;//in 4KB unit
    unsigned long               emergency_pages;
    unsigned long               used_pages_num;

    struct os_mutex             *mgr_lock;

    struct list_head            defer_destroy_list;

    struct os_mutex             *list_lock;

    unsigned int                segment_cnt;
    vidmm_segment_t             *segment;

    unsigned int                paging_segment_id;

    zx_vma_space_t              *reserved_vma;;

    vidmm_chip_func_t           *chip_func;

    struct range_allocator      *bl_ra;
    struct range_node           *bl_reserve_node;
    vidmm_segment_memory_t *    bl_reserved_memory;
    void                        *bl_buf_backup;

    //share gart info
    vidmm_segment_memory_t *    share_pt_buffer;
    void                        *share_pt_backup;
} vidmm_mgr_t;

extern vidmm_chip_func_t   vidmm_chip_func;


extern void vidmmi_try_destroy_one_allocation(adapter_t *adapter, vidmm_allocation_t *allocation, int not_in_use);
extern int  vidmmi_unresident_allocations(adapter_t *adapter, gpu_device_t *device, gpu_context_t* context, vidmm_unresident_arg_t *unresident_arg);

//extern void vidmmi_dump_video_memory(vidmm_mgr_t *mm_mgr, unsigned int segment_id);
extern void vidmmi_destroy_allocation(adapter_t *adapter, vidmm_allocation_t *allocation);
extern void vidmmi_release_system_memory(adapter_t *adapter, vidmm_allocation_t *allocation);
extern int  vidmmi_do_swap_out(vidmm_mgr_t *mm_mgr, int need_pages);

extern void vidmmi_release_video_memory(vidmm_mgr_t *mm_mgr, vidmm_allocation_t *allocation);

extern int vidmmi_unresident_allocations(adapter_t *adapter, gpu_device_t *device, gpu_context_t* context, vidmm_unresident_arg_t *unresident_arg);

extern int vidmmi_allocate_fb_memory_try(vidmm_mgr_t *mm_mgr, vidmm_allocation_t *allocation);
extern void vidmmi_vm_get_node_info(zx_vm_node_t *node, unsigned long long *offset, unsigned long long *size);
extern void vidmmi_dump_vm_info(vidmm_chip_vm_info_t          *vm_info);
extern int vidmmi_get_segment_id_by_property(adapter_t* adapter, unsigned int property);
extern int vidmm_make_resident_locked(vidmm_allocation_t *allocation);
extern int vidmmi_init_allocation(adapter_t *adapter, vidmm_allocation_t *allocation);
extern int vidmmi_segment_unresident_allocations(vidmm_mgr_t *mm_mgr, vidmm_segment_t *segment);

#endif

