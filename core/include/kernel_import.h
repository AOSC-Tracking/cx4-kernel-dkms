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

#ifndef __KERNEL_IMPORT_H__
#define __KERNEL_IMPORT_H__

#include "zx_types.h"

#ifndef TRUE
#define TRUE  1
#define FALSE 0
#endif

#ifndef NULL
#define NULL ((void *)0)
#endif

#define ZX_MEM_TRACK_RESULT_TO_FILE    0


#define OS_ACCMODE 0x00000003
#define OS_RDONLY  0x00000000
#define OS_WRONLY  0x00000001
#define OS_RDWR    0x00000002
#define OS_CREAT   0x00000100
#define OS_APPEND  0x00002000
#define OS_LARGEFILE 0x00100000

#define DEBUGFS_NODE_DEVICE                         1
#define DEBUGFS_NODE_HEAP                           2
#define DEBUGFS_NODE_INFO                           3
#define DEBUGFS_NODE_MEMTRACK                       4
#define DEBUGFS_NODE_DVFS                           5
#define DEBUGFS_NODE_VIDSCH                         6
#define DEBUGFS_NODE_CG                             7
#define DEBUGFS_NODE_DEBUGBUS                       8
#define DEBUGFS_NODE_VMA                            9
#define DEBUGFS_NODE_VMA_ZONE                       10
#define DEBUGFS_NODE_TEST_MEM                       11
#define DEBUGFS_NODE_VM_NODE_INFO                   12
#define DEBUGFS_NODE_DRM_CRTC                       13
#define DEBUGFS_NODE_DRM_CONNECTOR                  14

struct os_seq_file;

typedef struct
{
    unsigned short  vendor_id;
    unsigned short  device_id;
    unsigned short  command;
    unsigned short  status;
    unsigned char   revision_id;
    unsigned char   prog_if;
    unsigned char   sub_class;
    unsigned char   base_class;
    unsigned char   cache_line_size;
    unsigned char   latency_timer;
    unsigned char   header_type;
    unsigned char   bist;
    unsigned short  sub_sys_vendor_id;
    unsigned short  sub_sys_id;
    unsigned short  link_status;
    /*force set to unsigned long long*/
    unsigned long long    reg_start_addr[5];
    unsigned long long    reg_end_addr[5];
    unsigned long long    mem_start_addr[5];
    unsigned long long    mem_end_addr[5];
    unsigned long long    secure_start_addr[5];
    unsigned long long    secure_end_addr[5];
    int             secure_on;
} bus_config_t;

typedef struct
{
    unsigned long totalram; //zx can used system pages num
    unsigned long freeram;
}mem_info_t;

#define ZX_CPU_CACHE_UNKNOWN               0
#define ZX_CPU_CACHE_VIVT                  1
#define ZX_CPU_CACHE_VIPT_ALIASING         2
#define ZX_CPU_CACHE_VIPT_NONALIASING      3
#define ZX_CPU_CACHE_PIPT                  4

typedef struct
{
    unsigned int dcache_type;
    unsigned int icache_type;
    unsigned int iommu_support;
    unsigned int page_size;
    unsigned int page_shift;
    unsigned int system_need_dma32;
#ifdef __aarch64__
    unsigned int is_ft1500a;
#endif
}platform_caps_t;


/*****************************************************************************/
typedef struct
{
    void *pdev;
}os_device_t;

typedef int (*condition_func_t)(void *argu);

enum zx_mem_type
{
    ZX_SYSTEM_IO             = 0x01,
    ZX_SYSTEM_RAM            = 0x02,
};

enum zx_mem_attribute
{
    ZX_MEM_WRITE_BACK     = 0x01,
    ZX_MEM_WRITE_THROUGH  = 0x02,
    ZX_MEM_WRITE_COMBINED = 0x03,
    ZX_MEM_UNCACHED       = 0x04,
};

typedef struct
{
    union
    {
        struct
        {
            unsigned char mem_type;
            unsigned char cache_type;
            unsigned char read_only  :1;
            unsigned char write_only :1;
        };
        unsigned int  value;
    };
}zx_map_flags_t;

typedef struct
{
    zx_map_flags_t flags;

    struct os_pages_memory *memory;
    unsigned long long     phys_addr; //For most cases only one node to avoid alloc mem too many times
    unsigned long long     *phys_addrs;
    unsigned long long     *node_sizes;
    int                    node_num;

    unsigned int  offset;
    unsigned long size;
}zx_map_argu_t;

typedef struct __zx_cpu_vm_area
{
    zx_map_flags_t flags;
    unsigned int    ref_cnt;
    unsigned long   size;
    unsigned long   owner;
    unsigned int    need_flush_cache;
    unsigned int    node_num;
    void            *virt_addr; //For most cases only one node to avoid alloc mem too many times
    void            **virt_addrs;
    unsigned int    *node_sizes;
}zx_cpu_vm_area_t;

#define ZX_LOCKED       0
#define ZX_LOCK_FAILED  1

typedef enum
{
    ZX_EVENT_UNKNOWN = 0,
    ZX_EVENT_BACK    = 1, /* condtion meet event back */
    ZX_EVENT_TIMEOUT = 2, /* wait timeout */
    ZX_EVENT_SIGNAL  = 3, /* wait interrupt by a signal */
}zx_event_status_t;


typedef enum
{
    WAIT_EVENT0 = 0,
    WAIT_EVENT1 = 1,
    WAIT_EVENT2 = 2,
    WAIT_EVENT3 = 3,
    WAIT_EVENT_MAX = 4,
}wakeup_event;

typedef enum
{
    ALLOCATION_EVENT_ALLOC_BEGIN            = 1,
    ALLOCATION_EVENT_ALLOC_END              = 2,
    ALLOCATION_EVENT_FILL_BEGIN             = 3,
    ALLOCATION_EVENT_FILL_END               = 4,
    ALLOCATION_EVENT_RESIDENT_BEGIN         = 5,
    ALLOCATION_EVENT_RESIDENT_END           = 6,
    ALLOCATION_EVENT_EVICT_BEGIN            = 7,
    ALLOCATION_EVENT_EVICT_END              = 8,
    ALLOCATION_EVENT_SAVE_BEGIN             = 9,
    ALLOCATION_EVENT_SAVE_END               = 10,
    ALLOCATION_EVENT_RESTORE_BEGIN          = 11,
    ALLOCATION_EVENT_RESTORE_END            = 12,
}allocation_event;

typedef struct
{
    zx_event_status_t  status;
    wakeup_event         event;
}event_wait_status;


struct general_wait_event
{
    int                           need_queue;
    unsigned int             event;
    struct os_spinlock*  event_lock;
};

#define RANGE_CREATE_FROM_TOP   (1 << 0)

struct range_allocator;

struct range_node
{
    unsigned long long start;
    unsigned long long size;
    unsigned long long tag; // debug usage
    void (*release)(struct range_node *node);
    void *private;
};

// @ret : if return 0 then stop the travel otherwise continue
typedef int (*range_node_visitor_t)(struct range_node *node, void *priv);

struct range_allocator
{
    void (*release)(struct range_allocator *ra);

    void (*dump)(void *printer, struct range_allocator *ra, const char *prefix);

    // alloc a fixed position/size node
    // @start: the fixed start address
    // @size: the size of the range
    struct range_node* (*reserve)(struct range_allocator *ra,
            unsigned long long start,
            unsigned long long size);

    // alloc a new range
    // @size: size of the allocation
    // @alignment: alignment of the allocation
    // @start: start of the allowed range for this node
    // @end: end of the allowed range for this node
    struct range_node* (*alloc)(struct range_allocator *ra,
            unsigned long long size,
            unsigned int alignment,
            unsigned long long start,
            unsigned long long end,
            unsigned int flags);

    struct range_node* (*lookup)(struct range_allocator *ra,
            unsigned long long start,
            unsigned int size);

    int (*for_each_node)(struct range_allocator *ra,
            unsigned long long start,
            unsigned long long end,
            void *priv,
            range_node_visitor_t visitor);

    unsigned long long start;

    unsigned long long size;

    unsigned long long free_size;

    unsigned int alignment; // the minimal alignment
};


typedef int (*util_event_handler_u)(void *data, event_wait_status state);

typedef int (*zx_thread_func_t)(void *data);

struct os_printer;
struct os_wait_queue;
typedef struct
{
    struct range_allocator* (*create_range_allocator)(unsigned long long start, unsigned long long size, unsigned int alignment);

    void (*udelay)(unsigned long long usecs_num);
    unsigned long long (*do_div)(unsigned long long x, unsigned long long y);
    void  (*msleep)(int num);
    void  (*getsecs)(long *secs, long *usecs);
    void  (*get_nsecs)(unsigned long long *nsecs);
    void  (*panic)(void);
    void  (*dump_stack)(void);
    int   (*copy_from_user)(void* to, const void* from, unsigned long size);
    int   (*copy_to_user)(void* to, const void* from, unsigned long size);
    void* (*memset)(void* s, int c, unsigned long count);
    void* (*memcpy)(void* d, const void* s, unsigned long count);
    int   (*memcmp_priv)(const void *s1, const void *s2, unsigned long count);
    void  (*byte_copy)(char* dst, char* src, int len);
    int   (*strcmp)(const char *s1, const char *s2);
    char* (*strcpy)(char *d, const char *s);
    char *(*strsep)(char **s, const char *ct);
    int   (*strncmp)(const char *s1, const char *s2, unsigned long count);
    char* (*strncpy)(char *d, const char *s, unsigned long count);
    unsigned long (*strlen)(char *s);

/****************************** IO access functions*********************************/
    unsigned long long   (*read64)(void* addr);
    unsigned int   (*read32)(void* addr);
    unsigned short (*read16)(void* addr);
    unsigned char  (*read8)(void* addr);

    void  (*write32)(void* addr, unsigned int val);
    void  (*write16)(void* addr, unsigned short val);
    void  (*write8)(void* addr, unsigned char val);

    struct os_file* (*file_open)(const char *path, int flags, unsigned short mode);
    void  (*file_close)(struct os_file *file);
    int   (*file_read)(struct os_file *file, void *buf, unsigned long size, unsigned long long *read_pos);
    int   (*file_write)(struct os_file *file, void *buf, unsigned long size);

/*****************************************************************************/
    int   (*vsprintf)(char *buf, const char *fmt, ...);
    int   (*vsnprintf)(char *buf, unsigned long size, const char *fmt, ...);
    int   (*sscanf)(char *buf, char *fmt, ...);
    void  (*printk)(unsigned int msglevel, const char* fmt, ...);
    void  (*cb_printk)(const char* msg);

    int   (*seq_printf)(struct os_seq_file* file, const char *f, ...);

    void   (*os_printf)(struct os_printer *p, const char *f, ...);

    void* (*malloc_priv)(unsigned long size);
    void* (*calloc_priv)(unsigned long size);
    void  (*free_priv)(void* addr);

    void* (*malloc_track)(unsigned long size, const char *file, unsigned int line);
    void* (*calloc_track)(unsigned long size, const char *file, unsigned int line);
    void  (*free_track)(void* addr, const char *file, unsigned int line);

/* bit ops */
    int   (*find_first_zero_bit)(void *buf, unsigned int size);
    int   (*find_next_zero_bit)(void *buf, unsigned int size, int offset);
    int   (*find_first_bit)(void *buf, unsigned int size);
    int   (*find_next_bit)(void *buf, unsigned int size, int offset);
    void  (*set_bit)(unsigned int nr, void *buf);
    void  (*clear_bit)(unsigned int nr, void *buf);
    int   (*test_bit)(unsigned int nr, void *buf);

    struct os_atomic* (*create_atomic)(int val);
    void  (*destroy_atomic)(struct os_atomic *atomic);
    int   (*atomic_read)(struct os_atomic *atomic);
    int   (*atomic_inc)(struct os_atomic *atomic);
    int   (*atomic_dec)(struct os_atomic *atomic);
    int   (*atomic_cmpxchg)(struct os_atomic *atomic, int old, int new);
    int   (*atomic_xchg)(struct os_atomic *atomic, int v);

    struct os_atomic64* (*create_atomic64)(long long val);
    void  (*destroy_atomic64)(struct os_atomic64 *atomic64);
    long long   (*atomic64_read)(struct os_atomic64 *atomic64);
    long long   (*atomic64_inc)(struct os_atomic64 *atomic64);
    long long   (*atomic64_dec)(struct os_atomic64 *atomic64);
    long long   (*atomic64_cmpxchg)(struct os_atomic64 *atomic64, long long old, long long new);
    long long   (*atomic64_xchg)(struct os_atomic64 *atomic64, long long v);

    struct os_mutex* (*create_mutex)(void);
    void  (*destroy_mutex)(struct os_mutex *mutex);
    int   (*mutex_lock_killable)(struct os_mutex *mutex);
    void  (*mutex_unlock)(struct os_mutex *mutex);
    void  (*mutex_lock)(struct os_mutex *mutex);
    int   (*mutex_trylock)(struct os_mutex *mutex);

    void  (*mutex_lock_track)(struct os_mutex *mutex, const char *file, int line);
    int   (*mutex_trylock_track)(struct os_mutex *mutex, const char *file, int line);

    struct os_sema* (*create_sema)(int val);
    void  (*destroy_sema)(struct os_sema *sem);
    void  (*down)(struct os_sema *sem);
    int   (*down_trylock)(struct os_sema *sem);
    void  (*up)(struct os_sema *sem);
    int   (*read_sema)(struct os_sema *sem);

    struct os_rwsema *(*create_rwsema)(void);
    void (*destroy_rwsema)(struct os_rwsema *sem);
    void (*down_read)(struct os_rwsema *sem);
    void (*down_write)(struct os_rwsema *sem);
    void (*up_read)(struct os_rwsema *sem);
    void (*up_write)(struct os_rwsema *sem);

    struct os_spinlock* (*create_spinlock)(void);
    void  (*destroy_spinlock)(struct os_spinlock *spin);
    void  (*spin_lock)(struct os_spinlock *spin);
    void  (*spin_unlock)(struct os_spinlock *spin);
    unsigned long (*spin_lock_irqsave)(struct os_spinlock *spin);
    void  (*spin_unlock_irqrestore)(struct os_spinlock *spin, unsigned long flags);
    void (*spin_lock_bh)(struct os_spinlock *spin);
    void  (*spin_unlock_bh)(struct os_spinlock *spin);

    struct os_wait_event* (*create_event)(int task_id);
    void  (*destroy_event)(void *event);
    zx_event_status_t (*wait_event_thread_safe)(struct os_wait_event *event, condition_func_t condition, void *argu, int msec);
    zx_event_status_t (*wait_event)(struct os_wait_event *event, int msec);
    void  (*wake_up_event)(struct os_wait_event *event);

    void* (*create_thread)(zx_thread_func_t func, void *data, const char *thread_name);
    void  (*destroy_thread)(void* thread);
    int   (*thread_should_stop)(void);
    struct os_wait_queue* (*create_wait_queue)(void);
    void  (*wait_for_events)(struct os_wait_queue *queue, struct general_wait_event  **events, int event_cnt, int msec, event_wait_status* ret);

    zx_event_status_t (*thread_wait)(struct os_wait_event *event, int msec);
    void  (*thread_wake_up)(struct os_wait_event *event);
    void  (*_thread_wake_up)(struct os_wait_queue  *os_queue, struct general_wait_event  *event);

    int   (*try_to_freeze)(void);
    int   (*freezable)(void);
    void  (*clear_freezable)(void);

    void  (*set_freezable)(void);
    int   (*freezing)(void);
    unsigned long (*get_current_pid)(void);
    unsigned long (*get_current_tid)(void);

    void  (*flush_cache)(zx_cpu_vm_area_t *vma, struct os_pages_memory* memory, unsigned int offset, unsigned int size);
    void  (*inv_cache)(zx_cpu_vm_area_t *vma, struct os_pages_memory* memory, unsigned int offset, unsigned int size);
    struct os_pages_memory* (*allocate_pages_memory_priv)(void *pdev, unsigned long long size, int page_size, alloc_pages_flags_t alloc_flags);
    void  (*free_pages_memory_priv)(void *pdev, struct os_pages_memory *memory);

    struct os_pages_memory* (*allocate_pages_memory_track)(void *pdev, unsigned long long size, int page_size, alloc_pages_flags_t alloc_flags, const char *file, unsigned int line);
    void  (*free_pages_memory_track)(void *pdev, struct os_pages_memory *memory, const char *file, unsigned int line);

    unsigned long long (*get_page_phys_address)(struct os_pages_memory *memory, int page_num, int *page_size);
    void (*pages_memory_for_each_continues)(struct os_pages_memory *memory, void *arg,
        int (*cb)(void *arg, int page_start, int page_cnt, unsigned long long dma_addr));

    void (*for_each_pages_memory)(int (*cb)(struct os_pages_memory*, unsigned int, void *), void *arg);

    zx_cpu_vm_area_t *(*map_pages_memory_priv)(zx_map_argu_t *map_argu);
    void  (*unmap_pages_memory_priv)(zx_cpu_vm_area_t *vm_area);
    zx_cpu_vm_area_t *(*map_io_memory_priv)(zx_map_argu_t *map_argu);
    void  (*unmap_io_memory_priv)(zx_cpu_vm_area_t *vm_area);
    zx_cpu_vm_area_t *(*map_pages_memory_track)(zx_map_argu_t *map_argu, const char *file, unsigned int line);
    void  (*unmap_pages_memory_track)(zx_cpu_vm_area_t *vm_area, const char *file, unsigned int line);
    zx_cpu_vm_area_t *(*map_io_memory_track)(zx_map_argu_t *map_argu, const char *file, unsigned int line);
    void  (*unmap_io_memory_track)(zx_cpu_vm_area_t *vm_area, const char *file, unsigned int line);
    void* (*ioremap)(unsigned long long  io_base, unsigned long long size);
    void  (*iounmap_priv)(void *map_address);

    int   (*mtrr_add)(unsigned long start, unsigned long size);
    int   (*mtrr_del)(int reg, unsigned long base, unsigned long size);

    int   (*get_mem_info)(mem_info_t *mem);

    void* (*pages_memory_swapout)(struct os_pages_memory *pages_memory);
    int   (*pages_memory_swapin)(struct os_pages_memory *pages_memory, void *file);
    void  (*release_file_storage)(void *file);

    void  (*get_bus_config)(void *dev, bus_config_t *bus);
    int   (*get_platform_config)(void *dev, const char* config_name, int *buffer, int length);

    int   (*get_command_status16)(void *dev, unsigned short *command);
    int   (*write_command_status16)(void *dev, unsigned short command);
    int   (*get_command_status32)(void *dev, unsigned int *command);
    int   (*write_command_status32)(void *dev, unsigned int command);
    unsigned long (*get_rom_start_addr)(void *dev);
    int   (*get_rom_save_addr)(void *dev, unsigned int *romsave);
    int   (*write_rom_save_addr)(void *dev, unsigned int romsave);
    int   (*get_bar1)(void *dev, unsigned int *bar1);

    void  (*register_trace_events)(void);
    void  (*unregister_trace_events)(void);
    void  (*task_create_trace_event)(int engine_index, unsigned long long context,
                                     unsigned long long task_id, unsigned int task_type);
    void  (*task_enqueue_trace_event)(int engine_index, unsigned long long context,
                                      unsigned long long task_id, int queue_id);
    void  (*task_submit_trace_event)(int engine_index, unsigned long long context,
                                     unsigned long long task_id, unsigned int task_type,
                                     unsigned long long fence_id, unsigned int args);
    void  (*task_release_trace_event)(int engine_index, unsigned long long context,
                                      unsigned long long task_id, unsigned int task_type);
    void  (*fence_back_trace_event)(int engine_index, unsigned long long fence_id);
    void  (*begin_section_trace_event)(const char* desc);
    void  (*end_section_trace_event)(int result);
    void  (*counter_trace_event)(const char* desc, unsigned long long value);
    void  (*allocation_trace_event)(int card_index, void *allocation, int event_type, unsigned long long args);

    int   (*query_platform_caps)(void *pdev, platform_caps_t *caps);

    struct os_workqueue_struct* (*create_workqueue)(char *name);
    struct os_work_struct *(*init_work_struct)(void *priv, void *func);
    void (*deinit_work_struct)(struct os_work_struct *work);
    void *(*get_work_struct_priv)(struct os_work_struct *work);
    unsigned int (*queue_work)(struct os_workqueue_struct *wq, struct os_work_struct *work);
    unsigned int (*schedule_work)(struct os_work_struct *work);
    unsigned int (*flush_work)(struct os_work_struct *work);
    void (*flush_workqueue)(struct os_workqueue_struct *wq);

    void  (*enable_interrupt)(void *pdev);
    void  (*disable_interrupt)(void *pdev);
    void  (*console_lock)(int);
    int   (*disp_wait_idle)(void *disp_info);

    /* barrier */
    void (*mb)(void);
    void (*rmb)(void);
    void (*wmb)(void);
    void (*flush_wc)(void);
    void (*dsb)(void);

    /* dma fence interfaces for core layer callback */
    void (*fence_update_value)(void *fence, unsigned long long value);
    void (*fence_release)(void *fence);
    void (*fence_notify_event)(void *pdev);
    int (*fence_is_signaled)(void *fence);
    int (*fence_need_send_irq)(void *fence, unsigned long long value);
#ifdef ZX_VMI_MODE
    void (*set_pages_addr)(struct os_pages_memory *mem, unsigned long long addr);
#endif

}krnl_import_func_list_t;

#define ZX_DRV_DEBUG     0x00
#define ZX_DRV_WARNING   0x01
#define ZX_DRV_INFO      0x02
#define ZX_DRV_ERROR     0x03
#define ZX_DRV_EMERG     0x04

#endif /*__KERNEL_IMPORT_H__*/

