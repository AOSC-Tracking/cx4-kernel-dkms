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
#ifndef __OS_INTERFACE_H__
#define __OS_INTERFACE_H__

#include "kernel_import.h"

#if !defined(ZX_API_CALL)
#define ZX_API_CALL
#endif

#define ZX_MALLOC_TRACK       0
#define ZX_ALLOC_PAGE_TRACK   0
#define ZX_MAP_PAGES_TRACK    0
#define ZX_MAP_IO_TRACK       0

extern void zx_udelay(unsigned long long usecs_num);
extern unsigned long long zx_do_div(unsigned long long x, unsigned long long y);
extern void zx_msleep(int);
extern void zx_getsecs(long *secs, long *usecs);
extern void zx_get_nsecs(unsigned long long *nsecs);
extern void zx_panic(void);
extern void zx_dump_stack(void);

extern int  zx_find_first_zero_bit(void *buf, unsigned int size);
extern int  zx_find_next_zero_bit(void *buf, unsigned int size, int offset);
extern int  zx_find_first_bit(void *buf, unsigned int size);
extern int  zx_find_next_bit(void *buf, unsigned int size, int offset);
extern void zx_set_bit(unsigned int nr, void *buf);
extern void zx_clear_bit(unsigned int nr, void *buf);
extern int  zx_test_bit(unsigned int nr, void *buf);

extern struct os_atomic *zx_create_atomic(int val);
extern void zx_destroy_atomic(struct os_atomic *atomic);
extern int  zx_atomic_read(struct os_atomic *atomic);
extern int zx_atomic_inc(struct os_atomic *atomic);
extern int zx_atomic_dec(struct os_atomic *atomic);
extern int zx_atomic_cmpxchg(struct os_atomic *atomic, int, int);
extern int zx_atomic_xchg(struct os_atomic *atomic, int);

extern struct os_atomic64 *zx_create_atomic64(long long val);
extern void zx_destroy_atomic64(struct os_atomic64 *atomic64);
extern long long zx_atomic64_read(struct os_atomic64 *atomic64);
extern long long zx_atomic64_inc(struct os_atomic64 *atomic64);
extern long long zx_atomic64_dec(struct os_atomic64 *atomic64);
extern long long zx_atomic64_cmpxchg(struct os_atomic64 *atomic64, long long, long long);
extern long long zx_atomic64_xchg(struct os_atomic64 *atomic64, long long);

extern struct os_mutex *zx_create_mutex(void);
extern void  zx_destroy_mutex(struct os_mutex *mutex);
extern int   zx_mutex_lock_killable(struct os_mutex *mutex);
extern void  zx_mutex_lock_track(struct os_mutex *mutex, const char *file, int line);
extern int   zx_mutex_trylock_track(struct os_mutex *mutex, const char *file, int line);
extern void  zx_mutex_unlock(struct os_mutex *mutex);

extern void  zx_mutex_lock_priv(struct os_mutex *mutex);
extern int   zx_mutex_trylock_priv(struct os_mutex *mutex);


extern struct os_sema *zx_create_sema(int value);
extern void  zx_destroy_sema(struct os_sema *sema);
extern void  zx_down(struct os_sema *sema);
extern int   zx_down_trylock(struct os_sema *sema);
extern void  zx_up(struct os_sema *sema);
extern int   zx_read_sema(struct os_sema *sema);

extern struct os_rwsema *zx_create_rwsema(void);
extern void  zx_destroy_rwsema(struct os_rwsema *sema);
extern void  zx_down_read(struct os_rwsema *sema);
extern void  zx_down_write(struct os_rwsema *sema);
extern void  zx_up_read(struct os_rwsema *sema);
extern void  zx_up_write(struct os_rwsema *sema);

extern struct os_spinlock *zx_create_spinlock(void);
extern void   zx_destroy_spinlock(struct os_spinlock *spin);
extern void   zx_spin_lock(struct os_spinlock *spin);
extern void   zx_spin_unlock(struct os_spinlock *spin);
extern unsigned long zx_spin_lock_irqsave(struct os_spinlock *spin);
extern void   zx_spin_unlock_irqrestore(struct os_spinlock *spin, unsigned long flags);

extern void zx_spin_lock_bh(struct os_spinlock *spin);
extern void zx_spin_unlock_bh(struct os_spinlock *spin);

extern int ZX_API_CALL zx_copy_from_user(void *to, const void *from, unsigned long size);
extern int ZX_API_CALL zx_copy_to_user(void *to, const void *from, unsigned long size);

extern void* ZX_API_CALL zx_memset(void *s, int c, unsigned long count);
extern void* ZX_API_CALL zx_memcpy(void *d, const void *s, unsigned long count);
extern int   ZX_API_CALL zx_memcmp(const void *, const void *, unsigned long);
extern void  ZX_API_CALL zx_byte_copy(char* dst, char* src, int len);

extern int   zx_strcmp(const char *, const char *);
extern char* zx_strcpy(char *, const char *);
extern int   zx_strncmp(const char *, const char *, unsigned long);
extern char* zx_strncpy(char *, const char *, unsigned long);
extern unsigned long zx_strlen(char *s);
extern char* zx_strsep(char **s, const char *ct);

extern void zx_usleep_range(long min, long max);
extern int zx_get_platform_config(void *dev, const char* config_name, int *buffer, int length);
extern void zx_printf(struct os_printer *p, const char *f, ...);
extern struct os_printer zx_info_printer(void *dev);
extern struct os_printer zx_seq_file_printer(struct os_seq_file *f);
extern unsigned long zx_get_current_pid(void);
extern unsigned long zx_get_current_tid(void);
extern void zx_get_current_pname(char *, int);




/****************************** IO access functions*********************************/
extern unsigned long long ZX_API_CALL  zx_read64(void *addr);
extern unsigned int ZX_API_CALL  zx_read32(void *addr);
extern unsigned short ZX_API_CALL zx_read16(void *addr);
extern unsigned char ZX_API_CALL zx_read8(void *addr);
extern void ZX_API_CALL  zx_write32(void *addr, unsigned int val);
extern void ZX_API_CALL  zx_write16(void *addr, unsigned short val);
extern void ZX_API_CALL  zx_write8(void *addr, unsigned char val);

extern int ZX_API_CALL zx_vsprintf(char *buf, const char *fmt, ...);
extern int ZX_API_CALL zx_vsnprintf(char *buf, unsigned long size, const char *fmt, ...);

extern int ZX_API_CALL zx_sscanf(char *buf, char *fmt, ...);

extern void ZX_API_CALL zx_printk(unsigned int msglevel, const char* fmt, ...);
extern void ZX_API_CALL zx_cb_printk(const char* msg);
extern void ZX_API_CALL zx_cb_vdbgprint(int enable, unsigned int print_level, const char* prefix, const char* msg, ...);

#ifdef _DEBUG_
#define ZX_MSG_LEVEL ZX_DRV_DEBUG
#define zx_debug(args...) zx_printk(ZX_DRV_DEBUG, ##args)
#else
#define ZX_MSG_LEVEL ZX_DRV_DEBUG
#define zx_debug(args...) zx_printk(ZX_DRV_DEBUG, ##args)
#endif

#define zx_emerg(args...)   zx_printk(ZX_DRV_EMERG, ##args)
#define zx_error(args...)   zx_printk(ZX_DRV_ERROR, ##args)
#define zx_info(args...)    zx_printk(ZX_DRV_INFO,  ##args)
#define zx_warning(args...) zx_printk(ZX_DRV_WARNING, ##args)

#define zx_assert(expr, fmt, args...) do {\
    if (!(expr)) { \
        zx_printk(ZX_DRV_ERROR, "Assert (%s) failed at %s:%d: " fmt "\n", #expr, __func__, __LINE__, ##args); \
        BUG(); \
    } } while(0)


#ifdef ZX_MUTEX_TRACK
#define zx_mutex_lock(mtx)      zx_mutex_lock_track((mtx), __FILE__, __LINE__)
#define zx_mutex_trylock(mtx)   zx_mutex_trylock_track((mtx), __FILE__, __LINE__)
#else
#define zx_mutex_lock(mtx)      zx_mutex_lock_priv((mtx))
#define zx_mutex_trylock(mtx)   zx_mutex_trylock_priv((mtx))
#endif

extern struct os_wait_queue* zx_create_wait_queue(void);
extern struct os_wait_event* zx_create_event(int);
extern void zx_destroy_event(void *event);
extern zx_event_status_t zx_wait_event_thread_safe(struct os_wait_event *event, condition_func_t condition, void *argu, int msec);
extern void  thread_wait_for_events(struct os_wait_queue *wqueue, struct general_wait_event**  events, int event_cnt, int msec, event_wait_status* ret);
extern void zx_wake_up_event(struct os_wait_event *event);
extern zx_event_status_t zx_wait_event(struct os_wait_event *event, int msec);

extern void* zx_create_thread(zx_thread_func_t func, void *data, const char *thread_name);
extern void zx_destroy_thread(void *thread);
extern int  zx_thread_should_stop(void);
extern void zx_thread_wake_up(struct os_wait_event *event);
extern void general_thread_wake_up(struct os_wait_queue  *os_queue, struct general_wait_event  *event);
extern zx_event_status_t zx_thread_wait(struct os_wait_event *event, int msec);

extern struct os_file *zx_file_open(const char *path, int flags, unsigned short mode);
extern void zx_file_close(struct os_file *file);
extern int  zx_file_read(struct os_file *file, void *buf, unsigned long size, unsigned long long *read_pos);
extern int  zx_file_write(struct os_file *file, void *buf, unsigned long size);

extern int  zx_try_to_freeze(void);
extern void zx_set_freezable(void);
extern void zx_old_set_freezable(void);
extern void zx_clear_freezable(void);
extern int  zx_freezing(void);
extern int  zx_freezable(void);

/* os private alloc pages func*/
extern struct os_pages_memory* zx_allocate_pages_memory_priv(void *pdev, unsigned long long size, int page_size, alloc_pages_flags_t alloc_flags);
extern void zx_free_pages_memory_priv(void *pdev, struct os_pages_memory *memory);
extern zx_cpu_vm_area_t *zx_map_pages_memory_priv(zx_map_argu_t *map);
extern void zx_unmap_pages_memory_priv(zx_cpu_vm_area_t *vm_area);
extern zx_cpu_vm_area_t *zx_map_io_memory_priv(zx_map_argu_t *map);
extern void zx_unmap_io_memory_priv(zx_cpu_vm_area_t *map);
extern unsigned long long zx_get_page_phys_address(struct os_pages_memory *memory, int page_num, int *page_size);

extern void zx_flush_cache(zx_cpu_vm_area_t *vma, struct os_pages_memory* memory, unsigned int offset, unsigned int size);
extern void zx_inv_cache(zx_cpu_vm_area_t *vma, struct os_pages_memory* memory, unsigned int offset, unsigned int size);
extern int  zx_mtrr_add(unsigned long base, unsigned long size);
extern int  zx_mtrr_del(int reg, unsigned long base, unsigned long size);

/* os private malloc func */
extern void* zx_malloc_priv(unsigned long size);
extern void* zx_calloc_priv(unsigned long size);
extern void  zx_free_priv(void *addr);

extern int zx_get_command_status16(void *dev, unsigned short *command);
extern int zx_get_command_status32(void *dev, unsigned int *command);
extern int zx_write_command_status16(void *dev, unsigned short command);
extern int zx_write_command_status32(void *dev, unsigned int command);
extern int zx_get_rom_save_addr(void *dev, unsigned int *romsave);
extern int zx_write_rom_save_addr(void *dev, unsigned int romsave);
extern int zx_get_bar1(void *dev, unsigned int *bar1);
extern void* zx_pci_map_rom(void *dev);
extern void zx_pci_unmap_rom(void *dev, void* rom);

extern void zx_get_bus_config(void *pdev, bus_config_t *bus);
extern unsigned long zx_get_rom_start_addr(void *dev);
extern void         *zx_ioremap( unsigned long long io_base, unsigned long long size);
extern void          zx_iounmap(void *map_address);

#if ZX_MALLOC_TRACK | ZX_ALLOC_PAGE_TRACK | ZX_MAP_PAGES_TRACK | ZX_MAP_IO_TRACK
/* mem track func*/
extern void zx_mem_track_init(void);
extern void zx_mem_track_list_result(void);
extern void zx_mem_track_clear_result(void);
extern void  zx_mem_leak_list(void);
#endif

/* malloc track func*/
extern void* zx_malloc_track(unsigned long size, const char *file, unsigned int line);
extern void* zx_calloc_track(unsigned long size, const char *file, unsigned int line);
extern void  zx_free_track(void *addr, const char *file, unsigned int line);
extern void  zx_mem_leak_list(void);

#if ZX_MALLOC_TRACK
#define zx_malloc(size)         zx_malloc_track(size, __FILE__, __LINE__)
#define zx_calloc(size)         zx_calloc_track(size, __FILE__, __LINE__)
#define zx_free(addr)           zx_free_track(addr, __FILE__, __LINE__)
#else
#define zx_malloc(size)         zx_malloc_priv(size)
#define zx_calloc(size)         zx_calloc_priv(size)
#define zx_free(addr)           zx_free_priv(addr)
#endif

/* alloc pages track func*/
extern struct os_pages_memory *zx_allocate_pages_memory_track(void *pdev, unsigned long long size, int page_size,
                                   alloc_pages_flags_t alloc_flags,
                                   const char *file, unsigned int line);
extern void zx_free_pages_memory_track(void *pdev, struct os_pages_memory *memory,
                                        const char *file, unsigned int line);
#if ZX_ALLOC_PAGE_TRACK
#define zx_allocate_pages_memory(pdev, size, page_size, flag) \
        zx_allocate_pages_memory_track(pdev, size, page_size, flag, __FILE__, __LINE__)
#define zx_free_pages_memory(pdev, memory)         \
        zx_free_pages_memory_track(pdev, memory, __FILE__, __LINE__)
#else
#define zx_allocate_pages_memory(pdev, size, page_size, flag) \
        zx_allocate_pages_memory_priv(pdev, size, page_size, flag)
#define zx_free_pages_memory(pdev, memory)         \
        zx_free_pages_memory_priv(pdev, memory)
#endif

extern zx_cpu_vm_area_t *zx_map_pages_memory_track(zx_map_argu_t *map,
                                        const char *file, unsigned int line);
extern void zx_unmap_pages_memory_track(zx_cpu_vm_area_t *vm_area,
                                         const char *file, unsigned int line);

#if ZX_MAP_PAGES_TRACK
#define zx_map_pages_memory(argu)     \
        zx_map_pages_memory_track(argu, __FILE__, __LINE__)
#define zx_unmap_pages_memory(map) \
        zx_unmap_pages_memory_track(map, __FILE__, __LINE__)
#else
#define zx_map_pages_memory(argu)     \
        zx_map_pages_memory_priv(argu)
#define zx_unmap_pages_memory(vma) \
        zx_unmap_pages_memory_priv(vma)
#endif

extern zx_cpu_vm_area_t *zx_map_io_memory_track(zx_map_argu_t *map,
                                     const char *file, unsigned int line);
extern void zx_unmap_io_memory_track(zx_cpu_vm_area_t *vm_area,
                                      const char *file, unsigned int line);

#if ZX_MAP_IO_TRACK
#define zx_map_io_memory(argu) \
        zx_map_io_memory_track(argu, __FILE__, __LINE__)
#define zx_unmap_io_memory(argu) \
        zx_unmap_io_memory_track(argu, __FILE__, __LINE__)
#else
#define zx_map_io_memory(argu) \
        zx_map_io_memory_priv(argu)
#define zx_unmap_io_memory(argu) \
        zx_unmap_io_memory_priv(argu)
#endif

extern int zx_get_mem_info(mem_info_t *mem);
extern int zx_query_platform_caps(void *pdev, platform_caps_t *caps);

extern int   zx_pages_memory_swapin(struct os_pages_memory *pages_memory, void *file);
extern void *zx_pages_memory_swapout(struct os_pages_memory *pages_memory);

void zx_release_file_storage(void *file);

extern char *zx_mem_track_result_path;

extern void zx_register_trace_events(void);
extern void zx_unregister_trace_events(void);
extern void zx_task_create_trace_event(int engine_index, unsigned long long context,
                                       unsigned long long task_id, unsigned int task_type);
extern void zx_task_enqueue_trace_event(int engine_index, unsigned long long context,
                                        unsigned long long task_id, int queue_id);
extern void zx_task_submit_trace_event(int engine_index, unsigned long long context,
                                       unsigned long long task_id, unsigned int task_type,
                                       unsigned long long fence_id, unsigned int args);
extern void zx_task_release_trace_event(int engine_index, unsigned long long context,
                                        unsigned long long task_id, unsigned int task_type);
extern void zx_fence_back_trace_event(int engine_index, unsigned long long fence_id);
extern void zx_begin_section_trace_event(const char* desc);
extern void zx_end_section_trace_event(int result);
extern void zx_counter_trace_event(const char* desc, unsigned long long value);
extern void zx_allocation_trace_event(int card_index, void *allocation, int event_type, unsigned long long args);

extern int zx_seq_printf(struct os_seq_file *, const char *, ...);

extern unsigned int ZX_API_CALL  zx_secure_read32(unsigned long addr);
extern void ZX_API_CALL  zx_secure_write32(unsigned long addr, unsigned int val);
extern void zx_console_lock(int lock);
extern int disp_wait_idle(void *disp_info);
extern int zx_disp_wait_idle(void *disp_info);
extern struct os_workqueue_struct *zx_create_workqueue(char *name);
extern void zx_destroy_workqueue(struct os_workqueue_struct *os_wq);
extern struct os_work_struct *zx_init_work_struct(void *parent, void *func);
extern void zx_deinit_work_struct(struct os_work_struct *work);
extern void *zx_get_work_struct_priv(struct os_work_struct *work);
extern unsigned int zx_queue_work(struct os_workqueue_struct *wq, struct os_work_struct *work);
extern unsigned int zx_schedule_work(struct os_work_struct *work);
extern unsigned int zx_flush_work(struct os_work_struct *work);
extern void zx_flush_workqueue(struct os_workqueue_struct *wq);

extern void zx_pages_memory_for_each_continues(struct os_pages_memory *memory, void *arg,
    int (*cb)(void *arg, int page_start, int page_cnt, unsigned long long dma_addr));

extern void zx_for_each_pages_memory(int (*cb)(struct os_pages_memory*, unsigned int size, void *arg), void *arg);

extern struct range_allocator* zx_create_range_allocator(unsigned long long start, unsigned long long size, unsigned int alignment);

extern void zx_mb(void);
extern void zx_rmb(void);
extern void zx_wmb(void);
extern void zx_flush_wc(void);
extern void zx_dsb(void);
#ifdef ZX_VMI_MODE
extern void zx_set_pages_addr(struct os_pages_memory *mem, unsigned long long addr);
#endif

#endif

