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
#ifndef __VIDSCH_H__
#define __VIDSCH_H__

#include "context.h"
#include "kernel_interface.h"

#define PENDING_TASK_QUEUE_LENGTH       16
#define MAX_SYNC_OBJ_WAITED_ON          32

typedef struct dma_node
{
    struct range_node   *range;           //list node from dma heap, only valid if allocated from heap
    unsigned char       *cpu_virtual_address; //cpu virtual address
    unsigned long long  gpu_virtual_address;  //gpu virtual address
    unsigned int        command_length;       //current command length, should be less than orig_size
    unsigned long long  size;                 //allocated dma node size
}dma_node_t;

typedef enum
{
    DRAW_2D_DMA    = 1,
    DRAW_3D_DMA    = 2,
    DRAW_3D_BLT    = 3,
    PAGING_DMA     = 4,
    LPDP_DMA       = 5,
    VM_UPDATE_DMA  = 6,
    OCL_DMA        = 7,
    PAGE_TABLE_SET_DMA = 8,
}DMA_TYPE;

typedef enum _task_type
{
    task_type_render       = 1,                     /* task dma */
    task_type_vm_prepare   = 2,
    task_type_paging       = 4,
    task_type_vm_update    = 6,
    task_type_page_table_set = 8,
}task_type_t;

typedef struct _task_desc
{
    struct list_head          list_item;      //list node of vidsch_mgr_t->submitted_task_list
    struct list_head          schedule_node;  //list node of pending_task_pool_t->task_list
    task_type_t               type;           //task type
    DMA_TYPE                  dma_type;
    unsigned int              dma_cnt;        //primitive task number
    int                       vm_id;          //zx_vm_id_obj_t->vm_id, describes hardware page table mapping context index
    struct gpu_context        *context;           //gpu_context
    void                      *dma_fence;

    unsigned int              submitted      : 1; //if task has been submitted
    unsigned int              fake_submitted : 1; //if submit task failed and remove task in scheduler
    unsigned int              reset_dropped  : 1; //task dropped by reset operation, task maybe not be executed completed
    unsigned int              has_prepared   : 1;

    struct os_atomic          *ref_cnt;
    int                       cmd_size;           //sum of all primitive task command size

    void                      (*release)(struct vidsch_mgr *vidsch_mgr, struct _task_desc *);
    unsigned long long        timestamp;          //executed timestamp
    unsigned long long        enqueue_timestamp;  //executed timestamp
    unsigned long long        fence_id;           //task scheduler completed fence id, this value is from vidsch_mgr_t->last_send_fence_id
    unsigned long long        uuid;               //for debug only, to identify the task globally
    unsigned int              need_send_irq;
}task_desc_t;

typedef struct task_dma
{
    task_desc_t           desc;              //task description
    DMA_TYPE              dma_type;          //dma command type, affects hardware ringbuffer command
    struct hw_ctxbuf     *hw_ctx_info;      //hw ctx infomation for hardware context switch
    unsigned int          engine_index;      //hardware engine index

    unsigned long long   ctx_buf_va;
    struct dma_node      *dma_buffer_list;  //dma buffer list, each item is a seperately dma buffer
    //unsigned int         submit_dma_start;  //dma buffer submit start index of dma_buffer_list, for debug only
    //unsigned int         submit_dma_end;    //dma buffer submit end index of dma_buffer_list, for debug only

    void                *wait_dma_fence;

    unsigned int               primary_cnt;//number of primary allocations in this task_dma.
    struct _vidmm_allocation  **primary_allocation_list; //primary allocation array.

    unsigned int               resident_cnt;
    struct _vidmm_allocation   **resident_allocation_list; // for vulkan

    struct
    {
        unsigned int    need_hwctx_switch    :   1;  //need hardware context switch
        unsigned int    null_rendering       :   1;  //skip render command, only do fence operation
        unsigned int    dump_rb              :   1;  // dump ring buffer when kickoff, for debug
        unsigned int    dump_dma             :   1;  // dump dma when kickoff, for debug
        unsigned int    ignore_dma           :   1;
        unsigned int    has_resident         :   1; // indicate has mark_unpagable, so need mark_pagable when release task
                                                    // so for fake_submit case, no need mark_unpagable/mark_pagable
        unsigned int    initialize_context   :   1; // whether the ctx_buf_va was initialized?
        unsigned int    reserved             :  25;
    }flag;

    void *private_data;
} task_dma_t;

typedef struct task_paging
{
    task_desc_t                     desc;            //task description
    struct dma_node                 *dma;            //paging task dma node

    struct
    {
        unsigned int                paging_fill  :  1;  //fill allocation
        unsigned int                paging_in    :  1;  //paging in
        unsigned int                paging_out   :  1;  //paging out
        unsigned int                save_restore :  1;
        unsigned int                reserved     : 28;
    }flag;

    int                             must_success;                 //indicates must success, especially for paging in/out
    int                             paging_allocation_num;        //actual number of allocations that need do paging
    int                             paging_allocation_list_size;  //size of paging_allocation_list
    struct vidmm_paging_allocation  *paging_allocation_list;      //the list restores paging allocation
} task_paging_t;

typedef struct task_vm_update
{
    task_desc_t             desc;         //task description
    unsigned int            dma_list_cnt; // caps of dma_array
    unsigned int            dma_size;
    dma_node_t              **dma_list;   //initial dma buffer to store the update ptes command
                                          //maybe not used at all
                                          //the virtual address is fetched from vidmm_mgr->paging_vma

    dma_node_t              *reserved[4];
} task_vm_update_t;

typedef struct task_page_root_set
{
    task_desc_t                     desc;         //task description
    int                             target_engine_idx;
    gpu_device_t                    *target_device;
    void                            *target_task_desc;
    int                             is_set_root_page_table;
} task_page_table_set_t;

typedef struct task_vm_prepare
{
    task_desc_t                     desc;         //task description
    void                            *target_task_desc;
    void                            *target_sch_mgr;
} task_vm_prepare_t;

typedef struct _allocate_dma_task
{
    DMA_TYPE            dma_type;        //dma task type
    unsigned int        dma_cnt;         //number of dmas
    unsigned int        primary_cnt; //number of primary allocation array
}vidsch_allocate_dma_task_t;


typedef struct batch_parser_s vidsch_batch_parser_t;

typedef struct _vidsch_allocate_paging_task
{
    unsigned int   dma_size;         //dma size which need to allocate
    unsigned int   allocation_num;   //allocation number for paging
}vidsch_allocate_paging_task_t;

typedef struct _vidsch_allocate_vm_update_task
{
    unsigned int   dma_size;   //dma size which need to allocate
}vidsch_allocate_vm_update_task_t;

typedef struct vidsch_cil2_escape
{
    unsigned int             op_code;
    gpu_context_t            *context;
    gpu_device_t             *device;
    void                    *package;
}vidsch_cil2_escape_t;

extern task_dma_t*        vidsch_allocate_dma_task(struct gpu_context *context, vidsch_allocate_dma_task_t *arg);
extern task_paging_t*     vidsch_allocate_paging_task(struct gpu_context *context, vidsch_allocate_paging_task_t *arg);
extern task_vm_update_t*  vidsch_allocate_vm_update_task(struct gpu_context *context, vidsch_allocate_vm_update_task_t *arg);
extern void* vidsch_vm_update_task_request_space(task_vm_update_t *task, unsigned int size);
extern void vidsch_vm_update_task_release_space(task_vm_update_t *task, void *cmd_buf);


extern int  vidsch_batch_parser(struct gpu_context *context, vidsch_batch_parser_t *parser);

extern int  vidsch_wait_fence_back(adapter_t *adapter, unsigned char engine_index, unsigned long long fence_id, unsigned int force_wait);
#define vidsch_wait_fence_interrupt  vidsch_wait_fence_back
extern unsigned long long vidsch_get_current_returned_fence(adapter_t *adapter, int engine_index);

extern int vidsch_notify_interrupt(adapter_t *adapter, unsigned int interrupt_event);
extern int vidsch_notify_power_tuning(adapter_t *adapter, unsigned int interrupt_event);

extern int  vidsch_cil2_misc(gpu_device_t *device, krnl_cil2_misc_t *misc);

extern int vidsch_dump_debugbus(struct os_printer *p, adapter_t *adapter);

extern int  vidsch_is_fence_back(adapter_t *adapter, unsigned char engine_index, unsigned long long fence_id);

extern int  vidsch_request_fence_space(adapter_t *adapter, unsigned long long **virt_addr, unsigned long long **virt_addr_user,  void *filp, unsigned long long *gpu_addr);
extern int  vidsch_release_fence_space(adapter_t *adapter, unsigned long long gpu_addr);

extern void vidsch_wait_engine_idle(adapter_t *adapter, int idx);
extern void vidsch_wait_chip_idle(adapter_t *adapter, unsigned int engine_mask);

extern void vidsch_choose_vm_id(struct gpu_device *device, task_desc_t *task_desc);

extern int vidsch_init(adapter_t *adapter);
extern void vidsch_destroy(adapter_t *adapter);
extern int vidsch_create_reserve_resource(adapter_t *adapter);

extern int vidsch_query_info(adapter_t *adapter, zx_query_info_t *info);
extern int vidsch_save(adapter_t *adapter);
extern void vidsch_restore(adapter_t *adapter);
extern void vidsch_restore_dvfs(adapter_t *adapter);
extern void vidsch_dump(struct os_printer *p, adapter_t *adapter);
extern int  vidsch_cil2_zx_escape(gpu_device_t *device, vidsch_cil2_escape_t *cil2);

extern void vidsch_init_task_queue(struct gpu_context *contxt);
extern void vidsch_deinit_task_queue(struct gpu_context *context);

extern void vidsch_wakeup_worker_thread(adapter_t *adapter, int engine_index);

extern void vidsch_test_memory(struct os_printer *p, void *adpt , void *para);

extern int vidsch_notify_page_fault(adapter_t *adapter);
extern void vidsch_power_mask_acquire(adapter_t *adapter, gpu_device_t *device);
extern void vidsch_power_mask_clear(gpu_device_t *device);
#endif
