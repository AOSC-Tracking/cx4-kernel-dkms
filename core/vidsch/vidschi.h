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
#ifndef __VIDSCHI_H__
#define __VIDSCHI_H__

#include "zx_adapter.h"
#include "context.h"
#include "vidsch_task_queue.h"
#include "kernel_interface.h"

#define DAEMON_THREAD_INTERVAL              500 //0.5s

#if defined(ANDROID)
#define HW_HANG_LOG_FILE                "/data/zx_hw_hang.log"
#else
#define HW_HANG_LOG_FILE                "/var/log/zx_hw_hang.log"
#endif

#ifdef GFX_ONLY_FPGA
#define HW_HANG_MAX_TIMEOUT_NS              (200000 *  1000000ll)  //200s
#define SYNC_OBJECT_MAX_SERVER_WAIT_TIME_NS (2000000 * 1000000ll) //2000s
#else
#define HW_HANG_MAX_TIMEOUT_NS              (60000 *  1000000ll)  //60s
#define SYNC_OBJECT_MAX_SERVER_WAIT_TIME_NS (90000 * 1000000ll) //90s
#endif

#define HW_HANG_MAX_TIMEOUT_NS_ON_QT                (200000 *  1000000ll) // 200s
#define SYNC_OBJECT_MAX_SERVER_WAIT_TIME_NS_ON_QT   (200000 * 1000000ll) // 200s

/* engine support task type */
#define ENGINE_CAPS_PAGING              bit_0
#define ENGINE_CAPS_PRESENT_2DBLT       bit_1
#define ENGINE_CAPS_PRESENT_3DBLT       bit_2
#define ENGINE_CAPS_PRESENT_DMAFLIP     bit_3
#define ENGINE_CAPS_PRESENT_MMIOFLIP    bit_4
#define ENGINE_CAPS_PRESENT_OVERLAYFLIP bit_5
#define ENGINE_CAPS_2D_GRAPHICS         bit_6
#define ENGINE_CAPS_3D_GRAPHICS         bit_7
#define ENGINE_CAPS_VIDEO_DECODE        bit_8
#define ENGINE_CAPS_VIDEO_ENCODE        bit_9
#define ENGINE_CAPS_VIDEO_VPP           bit_10
#define ENGINE_CAPS_VIDEO_CAPTURE       bit_11
#define ENGINE_CAPS_CS                  bit_12

/* misc hw engine caps*/

#define ENGINE_CAPS_NO_HWCTX            bit_30


#define ENGINE_CAPS_PRESENT_ALL (ENGINE_CAPS_PRESENT_2DBLT | ENGINE_CAPS_PRESENT_3DBLT | \
                                 ENGINE_CAPS_PRESENT_DMAFLIP | ENGINE_CAPS_PRESENT_MMIOFLIP | \
                                 ENGINE_CAPS_PRESENT_OVERLAYFLIP)


#define ENGINE_CAPS_SUPPORT_ALL (ENGINE_CAPS_PAGING | ENGINE_CAPS_PRESENT_ALL | \
                                 ENGINE_CAPS_2D_GRAPHICS | ENGINE_CAPS_3D_GRAPHICS | \
                                 ENGINE_CAPS_VIDEO_DECODE | ENGINE_CAPS_VIDEO_ENCODE | \
                                 ENGINE_CAPS_VIDEO_VPP)


#define ENGINE_CAPS_VIDEO       (ENGINE_CAPS_VIDEO_DECODE | ENGINE_CAPS_VIDEO_ENCODE | \
                                 ENGINE_CAPS_VIDEO_VPP)


#define ENGINE_CAPS_GFX         (ENGINE_CAPS_2D_GRAPHICS | ENGINE_CAPS_3D_GRAPHICS)

#define ENGINE_CAPS_MANUAL_POWER_CLOCK (ENGINE_CAPS_VIDEO_DECODE | ENGINE_CAPS_VIDEO_ENCODE | \
                                 ENGINE_CAPS_VIDEO_VPP)

#define ENGINE_CTRL_DISABLE       bit_0
#define ENGINE_CTRL_THREAD_ENABLE bit_1

#define ZX_BITMAP_SIZE ((((ZX_CONTEXT_PRIO_MAX+1+7)/8) +sizeof (unsigned int)-1)/sizeof (unsigned int))

#define FLIP_QUEUE_LENGTH               16

typedef struct zx_vm_id_obj
{
    struct list_head                 vm_id_node;          //list node of vidsch_vm_id_mgr_t->ids_lru
    int                              vm_id;               //id index, validate id range is [0, vidsch_vm_id_mgr->num_ids-1]

    // device_uuid + pd_age can Uniquely determine the tlb contents for this vm_id
    long long                        device_uuid;         //gpu device which is using this vm id
    long long                        pd_age;

    unsigned int                     bitmap;                          //bitmap for last flush fence array
    unsigned long long               last_flush[MAX_ENGINE_COUNT];   //per engine fence array of last flushed task with this vm id,
                                                                      //the task can be render task or paging task that involved with
                                                                      //GPU vm allocation access, if this fence is signaled,
                                                                      //it indicates the vm id is idle in that engine.
                                                                      //if used by the same gpu device, no need to wait this fence,
                                                                      //but used by other device, need to wait all last flush fence
                                                                      //except the same engine
} zx_vm_id_obj_t;

typedef struct vidsch_vm_id_mgr
{
    struct os_mutex         *lock;              // used for vm id manager to do vm id operation
    unsigned int            num_ids;            // total number of vm id of the hardware, queries from chip information
    struct list_head        vm_id_list;         // list of vm_id_obj
    struct zx_vm_id_obj    *vm_id_array;        // vm id list with the count of num_ids
    unsigned int            reserved_num_ids;   // reserved vm id start from index 0 in the vm_id_list,
                                                // used for specific process/vm space which is running permanent
} vidsch_vm_id_mgr_t;

typedef struct vidsch_global
{
    adapter_t                      *adapter;            //the adapter that the global video scheduler binds to, should be the lead physical adapter
    int                            disable_schedule;    //disable sw schedule when tdr happen
    util_event_thread_t            *daemon_thread;      //global kernel thread to check deamon states, like tdr
    unsigned int                   hw_hang;             //indicates the engine index combined value when the GPU hang happen
    struct os_rwsema               *rw_lock;            //used for the sync operation for vidsch

    struct vidsch_vm_id_mgr        vm_id_mgr;           //manage global vm id for hw update pte of
                                                        //mapping GPU virtual memory to physical memory

    struct vidschedule_chip_func   *chip_func;          //chip layer function, lke check hang
    struct os_spinlock  *power_status_lock;
    unsigned int busy_engine_mask;
    unsigned long                  *dvfs_indicator;
} vidsch_global_t;

typedef struct vidsch_fence_buffer
{
    struct vidmm_segment_memory   *fence_reserved_memory;   //reserved physical video memory for fence buffer

    unsigned int                  bitmap_size;    //bitmap size in BYTE
    unsigned int                  total_num;      //max number of fence
    unsigned int                  left_num;       //left number of fence can be used
    int                           free_start;     //the next bit offset from the last allocated bit

    struct os_spinlock            *bitmap_lock;   //bitmap lock
    unsigned long                 *bitmap;        //bitmap for managing fence buffer

}vidsch_fence_buffer_t;

typedef struct vidsch_mgr
{
    adapter_t                      *adapter;               //the adapter that the global video scheduler binds to, should be the lead physical adapter
    unsigned int                   engine_caps;            //engine support task type, a mask of caps like ENGINE_CAP_PAGING, ENGINE_CAP_2D_GRAPHICS
    unsigned int                   engine_ctrl;            //the control flag of engine, value of ENGINE_CTRL_THREAD_ENABLE or ENGINE_CTRL_DISABLE
    unsigned int                   engine_index;           //ring buffer index, value like RB_INDEX_GFXL

    unsigned int                   engine_schedule;
    vidsch_task_queue_pool_t       *task_queue_list;
    vidsch_task_queue_pool_t       *page_table_set_pool;

    unsigned long long             wait_delta_check_start; //the start time stamp for the server wait timeout
    unsigned long long             wait_delta_check_end;   //the end time stamp for the server wait timeout

    unsigned long long             current;                // timestamp

    struct os_mutex                *engine_lock;           //lock for engine operation
    struct os_spinlock             *power_status_lock;

    unsigned int                   hw_queue_size;          //size of scheduler hardware queue,
                                                           //the entity in this queue is able to submit to GPU to execute

    int                            uncomplete_task_num;    //uncompleted task number of this engine scheduler,
                                                           //the value is calculated based on the schedulaer last send fence id and returned fence id

    unsigned long long             last_send_fence_id;     //fence id of last send task to scheduler hardware queue
    unsigned long long             returned_fence_id;      //fence id of newest returned task running on the engine
    unsigned long long             returned_timestamp;     //timestamp of last returned fence

    struct os_wait_event           *prepare_event;           //os wait vm_prepare event for vm_update_task
    struct os_wait_event           *fence_event;           //os wait fence event for wait event thread
    struct os_spinlock             *fence_lock;            //sync lock for fence operation in this engine scheduler

    struct os_mutex                *task_list_lock;        //lock for task list operation of this engine scheduler
    struct list_head               submitted_task_list;    //task list that already submitted to hardware queue

    util_event_thread_t            *worker_thread;         //kernel thread for process pending task in software schedule queue to be executed,
                                                           //each engine scheduler manages this thread seperately, include initialize, freeze and wake up

    struct vidmm_segment_memory    *dma_reserved_memory;   //reserve memory for dma buffer
    struct range_allocator         *dma_ra;                //dma allocator
    struct os_mutex                *dma_lock;              //lock for dma_ra since that is thread safe

    struct vidmm_segment_memory    *local_reserved_memory; //reserved local video memory for context buffer,
                                                           //this memory should be map to global share zone of virtual memory space,
                                                           //the physical memory is the segment_memory->fb_page_mem

    struct vidmm_segment_memory    *pcie_reserved_memory;  //reserved system memory for engine begin/end buffer to save/restore hardware context,
                                                           //this memory should be map to global share zone of virtual memory space,
                                                           //the physical memory is the segment_memory->page_mem

    int                            init_submit;            //a flag to enable interrupt just before first submit dma
                                                           //because if enable early, sometime there will be no interrupt generated
                                                           //don't know why, maybe some VBIOS operation lead this
                                                           //TODO: should test if still have this issue.

    void                           *private_data;          //chip dependent data

    struct vidsch_chip_func        *chip_func;             //chip dependent function

    unsigned long long             *fence_buffer_cpu_va;   /* fence buffer addr */

    struct os_atomic64 *pending_tasks_num;
    struct os_atomic64 *handled_tasks_num;
    unsigned int        engine_dvfs_power_on;
    unsigned long long  last_idle;
    unsigned long long  last_busy;
    unsigned int  completely_idle;
    unsigned long long  idle_elapse;
    unsigned int        monitor_count;
} vidsch_mgr_t;

typedef struct _vidsch_query_private
{
    unsigned char                 engine_count;                           //out: total engine count
    unsigned int                  pcie_segment_id;                        //out: system memory segment id
    unsigned int                  local_segment_id;                       //out: local video memory segment id
    unsigned int                  fence_buffer_segment_id;                //out: fence buffer segment id
    unsigned int                  engine_caps[MAX_ENGINE_COUNT];          //out: engine support task type, a mask of caps like ENGINE_CAP_PAGING, ENGINE_CAP_2D_GRAPHICS
    unsigned int                  engine_ctrl[MAX_ENGINE_COUNT];          //out: the control flag of engine, value of ENGINE_CTRL_THREAD_ENABLE or ENGINE_CTRL_DISABLE
    unsigned int                  engine_hw_queue_size[MAX_ENGINE_COUNT]; //out: size of scheduler hardware queue
    unsigned int                  dma_segment[MAX_ENGINE_COUNT];          //out: dma heap segment
    unsigned int                  dma_buffer_size[MAX_ENGINE_COUNT];      //out: dma heap size */
    unsigned int                  pcie_memory_size[MAX_ENGINE_COUNT];     //out: pcie memory size need be reserved
    unsigned int                  local_memory_size[MAX_ENGINE_COUNT];    //out: local memory size need be reserved
    struct vidsch_chip_func       *engine_func[MAX_ENGINE_COUNT];         //out: vidsch mgr chip dependent function
}vidsch_query_private_t;

typedef struct _vidsch_query_vm_id
{
    unsigned int  total_vm_id_num;      //out: total number of vm id of the hardware
    unsigned int  reserved_vm_id_num;   //out: number of reserved vm id
}vidsch_query_vm_id_t;

typedef struct flip_sch_argu
{
    struct vidsch_display     *sch_display;
    int                       iga_idx;
}flip_sch_argu_t;

typedef struct vidsch_wait_fence_back_arg
{
    adapter_t          *adapter;
    int                engine;
    unsigned long long fence_id;
}vidsch_wait_fence_back_arg_t;

typedef struct vidsch_chip_func
{
    int  (*initialize)(adapter_t *adapter, vidsch_mgr_t *sch_mgr);  //initilize engine hardware
    int  (*destroy)(vidsch_mgr_t *sch_mgr);  //destroy engine

    int (*save)(vidsch_mgr_t *sch_mgr);
    void (*restore)(vidsch_mgr_t *sch_mgr, unsigned int pm);
    void (*restore_dvfs)(vidsch_mgr_t *sch_mgr);

    int (*render)(gpu_context_t *gpu_context, task_dma_t *dma_task);  //render
    int (*submit)(gpu_context_t *gpu_context, vidsch_mgr_t *sch_mgr, task_dma_t *dma_task);  //submit batches of command buffer to hardware
    int (*set_root_page_table)(vidsch_mgr_t *sch_mgr, gpu_device_t *device, unsigned int vm_id, task_page_table_set_t *page_table_set_task);
    int (*invalidate_tlb)(vidsch_mgr_t *sch_mgr, gpu_device_t *device, unsigned int vm_id, task_page_table_set_t *page_table_set_task);

    unsigned long long (*update_fence_id)(vidsch_mgr_t *sch_mgr); //update fence id on one engine when fence interrupt happens

    void (*set_fence_id)(vidsch_mgr_t *sch_mgr, unsigned long long fence_id);//for hw null use only. set the fence id with cpu.

    int  (*cil2_misc)(vidsch_mgr_t *sch_mgr, krnl_cil2_misc_t *cil2);
    void (*get_timestamp)(adapter_t *adapter, unsigned long long *escRet);

    int  (*power_clock)(vidsch_mgr_t *sch_mgr, unsigned int off);
}vidsch_chip_func_t;

typedef struct vidschedule_chip_func
{
    int  (*reset_hw)(vidsch_mgr_t *sch_mgr);  //reset engine
    void (*dump_hang_info)(vidsch_mgr_t *sch_mgr);  //dump hang information when hardware hang happens
    void (*dump_hang)(adapter_t *adapter);  //dump hang information when hardware hang happens
    void (*dump_debugbus)(struct os_printer *p, adapter_t *adapter);

    void (*power_tuning)(adapter_t* adapter);
    void (*disable_dvfs)(adapter_t* adapter);
    void (*get_set_reg)(adapter_t *adapter, zx_query_info_t *info);
    unsigned long long (*notify_page_fault)(adapter_t *adapter, int *vm_id);
}vidschedule_chip_func_t;

extern int vidschi_init_daemon_thread(adapter_t *adapter);
extern int vidschi_deinit_daemon_thread(adapter_t *adapter);

extern unsigned long long vidschi_inc_send_fence_id(vidsch_mgr_t *sch_mgr, int dec_prepare_num);

extern void vidschi_dump_page_table_set_task(task_page_table_set_t *page_table_set_task, int idx, int dump_detail);

extern void vidschi_init_vm_id_mgr(adapter_t *adapter);
extern void vidschi_fini_vm_id_mgr(adapter_t *adapter);
extern void vidsch_release_completed_tasks(vidsch_mgr_t *sch_mgr, unsigned long long *uncompleted_dma);

extern void vidsch_task_inc_reference(task_desc_t *desc);
extern void vidsch_task_dec_reference(task_desc_t *desc);
extern void vidsch_task_init_desc(vidsch_mgr_t *sch_mgr, gpu_context_t *context, task_desc_t *desc, task_type_t type);

extern void vidschi_dump_general_task_info(task_desc_t *task);
extern void vidschi_dump_task(task_desc_t *task, int idx, int dump_detail);

extern int vidschi_process_task_sync_list(vidsch_mgr_t *sch_mgr);

extern int vidsch_query_chip(adapter_t *adapter, vidsch_query_private_t *info);
extern int vidsch_query_vm_id(adapter_t *adapter, vidsch_query_vm_id_t *info);

extern dma_node_t *vidschi_allocate_dma_node(vidsch_mgr_t *sch_mgr, unsigned int dma_size);
extern void vidschi_release_dma_node(vidsch_mgr_t *sch_mgr, dma_node_t *dma);
extern void vidsch_update_dma_command_length(dma_node_t *dma, unsigned int command_length);

extern void vidschi_dump_allocation(int index, void *allocation);

#ifdef ZX_HW_NULL
void engine_set_fence_id_e3k(vidsch_mgr_t *sch_mgr, unsigned long long fence_id);
#endif

extern void vidsch_update_engine_idle_status(adapter_t *adapter, unsigned int engine_mask);
extern void vidschi_update_and_release_dma_fence(task_desc_t *task);
extern void vidschi_notify_dma_fence(adapter_t *adapter);

extern void vidschi_engine_dvfs_power_on(vidsch_mgr_t *sch_mgr);
extern void vidschi_try_power_tuning(adapter_t *adapter);

extern void vidsch_prepare_dma_task(task_dma_t *dma_task);
extern int vidsch_wait_prepare_event(vidsch_mgr_t *sch_mgr, task_desc_t *task_desc, unsigned int force_wait, task_type_t wait_task_type);

extern void vidschi_vm_id_add_sync(adapter_t *adapter, zx_vm_id_obj_t *vm_id_obj, unsigned long long fence_id, unsigned int engine_idx);
extern int vidschi_update_fence_id(vidsch_mgr_t *sch_mgr, unsigned long long *curr_returned);
#endif
