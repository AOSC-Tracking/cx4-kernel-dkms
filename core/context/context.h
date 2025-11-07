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

#ifndef __CONTEXT_H__
#define __CONTEXT_H__
#include "core_import.h"
#include "util.h"
#include "list.h"
#include "vidsch_task_queue.h"

#define RESERVED_ENGINE_INDEX 0
#define ZX_MAX_PNAME_LEN 64

typedef enum
{
    RB_INDEX_GFXL = 0,           //Graphics Low Ring Buffer Index;
    RB_INDEX_GFXH,               //Graphics High Ring Buffer Index;
    RB_INDEX_CSH,                //CS High Ring Buffer Index
    RB_INDEX_CSL0,               //CS  No1 RingBuffer Index
    RB_INDEX_CSL1,               //CS  No2 RingBuffer Index
    RB_INDEX_CSL2,               //CS  No3 RingBuffer Index
    RB_INDEX_CSL3,               //CS  No4 RingBuffer Index
    RB_INDEX_VCP0,
    RB_INDEX_VCP1,
    RB_INDEX_VCP2,
    RB_INDEX_VPP0,
    RB_INDEX_VPP1,
    RB_NUM,
    RB_INDEX_VPP = RB_INDEX_VCP2
} RB_NODE_E3K;

typedef enum context_priority
{
    ZX_CONTEXT_PRIO_LOW   = 0,
    ZX_CONTEXT_PRIO_MEDIA = 1,
    ZX_CONTEXT_PRIO_HIGH  = 2,
    ZX_CONTEXT_PRIO_MAX   = 3,
} CONTEXT_PRIORITY;

typedef enum _CM_CONTEXT_TYPE
{
    CONTEXT_NORMAL_RENDER   = 0,  //the nomal render context
    CONTEXT_PAGING_QUEUE    = 1,  //the VM related operation (do paging) context
    CONTEXT_COMPANION       = 2,  //sparse resource operation context
} CM_CONTEXT_TYPE;

typedef struct context_desc_flag
{
    union
    {
        struct
        {
        unsigned int type: 2;             //the context type, defined by CM_CONTEXT_TYPE
        unsigned int client: 3;           //API client type, defined by CM_CONTEXT_CLIENT_TYPE
        unsigned int null_render: 1;      //if true, the task running on this context will do nothing render operation
        unsigned int reserved: 26;
        };
        unsigned int value;
    };
} context_desc_flag_t;

typedef struct create_context
{
    unsigned int      node_ordinal;    //in, the engine index of the context on which hardware engine the command running
    unsigned int      priority;        //in, the context GPU scheduling priority
    unsigned int      null_render;     //in, if true, the task running on this context will do nothing render operation
    unsigned int      client_type;     //in, API client type, defined by CM_CONTEXT_CLIENT_TYPE
    unsigned int      type;            //in, the context type, defined by CM_CONTEXT_TYPE
    unsigned int      power_dvfs_indicador;

    unsigned long     tid;
} create_context_t;

typedef struct hw_ctxbuf
{
    struct list_head   list_node; //list node of device->hwctx_list

    int index;
    int phys_index;       //global context index, adapter function region
    int is_initialized;
    int is_invalid;

    unsigned int  context_buffer_address;
    unsigned int  *context_buffer;
}hw_ctxbuf_t;

typedef struct gpu_context
{
    struct list_head                   list_node;        //list node of device->context_list
    int                                rb_index;         //the ring buffer index defined by RB_NODE_{CHIP}
    int                                priority;         //the context GPU scheduling priority, the value is CONTEXT_PRIORITY
    context_desc_flag_t                flag;             //describe the context attribute

    unsigned long                      tid;              //the thread id of the context running in

    struct gpu_device                  *device;          //the gpu device to which the context bind

    unsigned long long                 task_id;          //an increment sequence value that describe the task resident to the context,
                                                         //this value focus on the state of executed of task running on the context

    unsigned long long                 render_counter;   //for the render trace event

    struct os_atomic                   *ref_cnt;         //used for recording the reference of the context,
                                                         //generally, the reference is called by the dma task executed on the context,
                                                         //this value can be used for checking whether one context is in idle state

    struct os_sync_timeline            *timeline;        //sync timeline bind to the context for the linux kernel sync fence

    unsigned long long                 wait_delta_check_start;  //the start time stamp for the server wait timeout
    unsigned long long                 wait_delta_check_end;    //the end time stamp for the server wait timeout


    int                                hwctx_is_initialized;

    hw_ctxbuf_t                        hw_ctx;  //the hardware context buffer for the video client, this field is deprecated

    vidsch_task_queue_t                 *task_queue;
    vidsch_task_queue_t                 *page_table_set_task_queue;
    unsigned long long                  uuid;
} gpu_context_t;

typedef struct gpu_device
{
    struct list_head            list_node;  //list node of adapter->device_list

    adapter_t                   *adapter;   //the lead physical adapter in the linked chain
    void                        *filp;      //linux file pointer, used for vidmm mapping to user space

    unsigned int                priority;   //scheduling priority

    // device info
    unsigned long               pid;        //process id
    unsigned long               tid;        //thread id
    char                        pname[ZX_MAX_PNAME_LEN];  //process name

    struct os_mutex             *lock;      //used by cm_device_lock(),
                                            //lock device's critical area,
                                            //like the following list head, resource_ref_num

    struct list_head            context_list;                  //contexts bind to the device,
                                                               //list head of context->list_node

    struct list_head            hwctx_list;                    //only used for OpenGL/ES API,
                                                               //list head of hw_ctxbuf->list_node

    struct list_head            bl_slot_list;    //list head of bl_slot_t->list_node, only used for vulkan

    struct list_head            sync_obj_list;                 //synchronization object list,
                                                               //list head of sync_obj->list_node

    int                         resource_ref_num;  // device create resource (device, context, hwctx, sync_obj, allocation) num
                                                   // since all resource use device ptr and device->lock so device struct and
                                                   // lock only can be free when this num = 0, note we also treat device itself as a num,
                                                   // when destroy_device called num--, then other resource destroy only can free struct after
                                                   // destroy deivce called when num-- lead num == 0. currently only for allocation, device

    struct zx_vma_space        *vm;  // gpu virtual memory space

    struct zx_vm_id_obj        *vm_id;  //vm id array currently used by this device

    int                         task_dropped;  //be set if any context in the device has dropped task
                                               //if so, need force set wait fence sync obj signal to avoid dead lock
                                               //since fence obj signal cmd always put in gerneal dma task.

    unsigned int                 video_seq_index;

    long long                   uuid;
    unsigned int                power_dvfs_indicador;
} gpu_device_t;

static inline void cm_device_lock(gpu_device_t *device)
{
    zx_mutex_lock(device->lock);
}

static inline void cm_device_unlock(gpu_device_t *device)
{
    zx_mutex_unlock(device->lock);
}

struct _vidmm_allocation;

extern gpu_device_t* cm_create_device(adapter_t *adapter, void *process_context);
extern void cm_destroy_device(adapter_t *adapter, gpu_device_t *device);
extern void cm_destroy_remained_device(adapter_t *adapter);
extern gpu_context_t* cm_create_context(gpu_device_t *device, create_context_t *create_context);
extern void cm_destroy_context(gpu_device_t *device, gpu_context_t *context);
extern void cm_wait_context_idle(gpu_context_t *context);
extern int cm_add_hwctx_buffer(gpu_device_t *device, gpu_context_t *context, unsigned int hw_buf_index);
extern int cm_remove_hwctx_buffer(gpu_device_t *device, gpu_context_t *context, unsigned int hw_buf_index);
extern hw_ctxbuf_t *cm_get_hwctx_buffer(gpu_device_t *device, gpu_context_t *context, unsigned int index);

extern int cm_save(adapter_t *adapter, int need_save_memory);
extern int cm_restore(adapter_t *adapter);

extern void cm_device_resource_ref_num_inc(gpu_device_t *device);
extern void cm_device_resource_ref_num_dec(gpu_device_t *device);

extern void cm_dump_resource(adapter_t *adapter);
extern void cm_dump_device_alloctions(struct os_seq_file *seq_file, adapter_t *adapter,  gpu_device_t *device);

#endif

