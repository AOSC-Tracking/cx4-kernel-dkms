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

#ifndef __CORE_IMPORT_H__
#define __CORE_IMPORT_H__

#include "kernel_import.h"

#define   GPU_PAGE_SHIFT         12
#define   GPU_LARGE_PAGE_SHIFT   16
#define   GPU_PAGE_SIZE          (4*1024)
#define   GPU_LARGE_PAGE_SIZE    (64*1024)

#define ZX_MALLOC_TRACK       0
#define ZX_ALLOC_PAGE_TRACK   0
#define ZX_MAP_PAGES_TRACK    0
#define ZX_MAP_IO_TRACK       0

extern krnl_import_func_list_t *zx;

/* CORE referenced funcs list */
#define zx_create_range_allocator        zx->create_range_allocator
#define zx_udelay                        zx->udelay
#define zx_do_div                        zx->do_div
#define zx_msleep                        zx->msleep
#define zx_getsecs                       zx->getsecs
#define zx_get_nsecs                     zx->get_nsecs
#define zx_panic                         zx->panic
#define zx_copy_from_user                zx->copy_from_user
#define zx_copy_to_user                  zx->copy_to_user
#define zx_memset                        zx->memset
#define zx_memcpy                        zx->memcpy
#define zx_memcmp                        zx->memcmp_priv
#define zx_byte_copy                     zx->byte_copy
#define zx_strcmp                        zx->strcmp
#define zx_strcpy                        zx->strcpy
#define zx_strncmp                       zx->strncmp
#define zx_strncpy                       zx->strncpy
#define zx_strlen                        zx->strlen
#define zx_strsep                        zx->strsep
#define zx_read64                        zx->read64
#define zx_read32                        zx->read32
#define zx_read16                        zx->read16
#define zx_read8                         zx->read8
#define zx_write32                       zx->write32
#define zx_write16                       zx->write16
#define zx_write8                        zx->write8
#define zx_file_open                     zx->file_open
#define zx_file_close                    zx->file_close
#define zx_file_read                     zx->file_read
#define zx_file_write                    zx->file_write
#define zx_vsprintf                      zx->vsprintf
#define zx_vsnprintf                     zx->vsnprintf
#define zx_sscanf                        zx->sscanf
#define zx_printk                        zx->printk
#define zx_cb_printk                     zx->cb_printk
#define zx_seq_printf                    zx->seq_printf
#define zx_printf                        zx->os_printf
#define zx_find_first_zero_bit           zx->find_first_zero_bit
#define zx_find_next_zero_bit            zx->find_next_zero_bit
#define zx_find_first_bit                zx->find_first_bit
#define zx_find_next_bit                 zx->find_next_bit
#define zx_set_bit                       zx->set_bit
#define zx_clear_bit                     zx->clear_bit
#define zx_test_bit                      zx->test_bit
#define zx_getsecs                       zx->getsecs
#define zx_get_nsecs                     zx->get_nsecs
#define zx_create_thread                 zx->create_thread
#define zx_destroy_thread                zx->destroy_thread
#define zx_thread_should_stop            zx->thread_should_stop

#define zx_create_atomic                 zx->create_atomic
#define zx_destroy_atomic                zx->destroy_atomic
#define zx_atomic_read                   zx->atomic_read
#define zx_atomic_inc                    zx->atomic_inc
#define zx_atomic_dec                    zx->atomic_dec
#define zx_atomic_cmpxchg                zx->atomic_cmpxchg
#define zx_atomic_xchg                   zx->atomic_xchg

#define zx_create_atomic64               zx->create_atomic64
#define zx_destroy_atomic64              zx->destroy_atomic64
#define zx_atomic64_read                 zx->atomic64_read
#define zx_atomic64_inc                  zx->atomic64_inc
#define zx_atomic64_dec                  zx->atomic64_dec
#define zx_atomic64_cmpxchg              zx->atomic64_cmpxchg
#define zx_atomic64_xchg                 zx->atomic64_xchg

#define zx_create_mutex                  zx->create_mutex
#define zx_destroy_mutex                 zx->destroy_mutex
#define zx_mutex_lock_killable           zx->mutex_lock_killable
#define zx_mutex_unlock                  zx->mutex_unlock
#define zx_create_sema                   zx->create_sema
#define zx_destroy_sema                  zx->destroy_sema
#define zx_down                          zx->down
#define zx_down_trylock                  zx->down_trylock
#define zx_up                            zx->up
#define zx_read_sema                     zx->read_sema
#define zx_create_rwsema                 zx->create_rwsema
#define zx_destroy_rwsema                zx->destroy_rwsema
#define zx_down_read                     zx->down_read
#define zx_down_write                    zx->down_write
#define zx_up_read                       zx->up_read
#define zx_up_write                      zx->up_write
#define zx_create_spinlock               zx->create_spinlock
#define zx_destroy_spinlock              zx->destroy_spinlock
#define zx_spin_lock                     zx->spin_lock
#define zx_spin_unlock                   zx->spin_unlock
#define zx_spin_lock_irqsave             zx->spin_lock_irqsave
#define zx_spin_unlock_irqrestore        zx->spin_unlock_irqrestore
#define zx_spin_lock_bh                  zx->spin_lock_bh
#define zx_spin_unlock_bh                zx->spin_unlock_bh
#define zx_create_event                  zx->create_event
#define zx_destroy_event                 zx->destroy_event
#define zx_wait_event_thread_safe        zx->wait_event_thread_safe
#define zx_wait_event                    zx->wait_event
#define zx_wake_up_event                 zx->wake_up_event
#define zx_thread_wait                   zx->thread_wait
#define zx_create_wait_queue         zx->create_wait_queue
#define thread_wait_for_events       zx->wait_for_events
#define zx_thread_wake_up                zx->thread_wake_up
#define general_thread_wake_up       zx->_thread_wake_up
#define zx_dump_stack                    zx->dump_stack
#define zx_try_to_freeze                 zx->try_to_freeze
#define zx_freezable                     zx->freezable
#define zx_clear_freezable               zx->clear_freezable
#define zx_set_freezable                 zx->set_freezable
#define zx_freezing                      zx->freezing
#define zx_get_current_pid               zx->get_current_pid
#define zx_get_current_tid               zx->get_current_tid
#define zx_flush_cache                   zx->flush_cache
#define zx_inv_cache                     zx->inv_cache
#define zx_get_page_phys_address         zx->get_page_phys_address
#define zx_pages_memory_for_each_continues      zx->pages_memory_for_each_continues
#define zx_for_each_pages_memory         zx->for_each_pages_memory
#define zx_get_page_count                zx->get_page_count
#define zx_fill_continues_page_memory    zx->fill_continues_page_memory
#define zx_allocate_pages_memory_dynamic zx->allocate_pages_memory_dynamic
#define zx_fill_pages_memory_dynamic     zx->fill_pages_memory_dynamic
#define zx_free_pages_memory_dynamic     zx->free_pages_memory_dynamic
#define zx_ioremap                       zx->ioremap
#define zx_iounmap                       zx->iounmap_priv
#define zx_mtrr_add                      zx->mtrr_add
#define zx_mtrr_del                      zx->mtrr_del
#define zx_get_mem_info                  zx->get_mem_info
#define zx_pages_memory_swapout          zx->pages_memory_swapout
#define zx_pages_memory_swapin           zx->pages_memory_swapin
#define zx_release_file_storage          zx->release_file_storage
#define zx_get_bus_config                zx->get_bus_config
#define zx_get_platform_config           zx->get_platform_config
#define zx_get_command_status16          zx->get_command_status16
#define zx_write_command_status16        zx->write_command_status16
#define zx_get_command_status32          zx->get_command_status32
#define zx_write_command_status32        zx->write_command_status32
#define zx_get_rom_start_addr            zx->get_rom_start_addr
#define zx_get_rom_save_addr             zx->get_rom_save_addr
#define zx_write_rom_save_addr           zx->write_rom_save_addr
#define zx_get_bar1                      zx->get_bar1
#define zx_disp_wait_idle                zx->disp_wait_idle

#define zx_dma_fence_update_value        zx->fence_update_value
#define zx_dma_fence_release_cb          zx->fence_release
#define zx_dma_fence_notify_event        zx->fence_notify_event
#define zx_dma_fence_is_signaled         zx->fence_is_signaled
#define zx_dma_fence_need_send_irq       zx->fence_need_send_irq

#define zx_enable_interrupt              zx->enable_interrupt
#define zx_disable_interrupt             zx->disable_interrupt

#define zx_mb                            zx->mb
#define zx_rmb                           zx->rmb
#define zx_wmb                           zx->wmb
#define zx_flush_wc                      zx->flush_wc
#define zx_dsb                           zx->dsb

#if ZX_MALLOC_TRACK
#define zx_malloc(size)                  zx->malloc_track(size, __FILE__, __LINE__)
#define zx_calloc(size)                  zx->calloc_track(size, __FILE__, __LINE__)
#define zx_free(addr)                    zx->free_track(addr, __FILE__, __LINE__)
#else
#define zx_malloc(size)                  zx->malloc_priv(size)
#define zx_calloc(size)                  zx->calloc_priv(size)
#define zx_free(addr)                    zx->free_priv(addr)
#endif

#define zx_emerg(args...)   zx->printk(ZX_DRV_EMERG, ##args)
#define zx_error(args...)   zx->printk(ZX_DRV_ERROR, ##args)
#define zx_info(args...)    zx->printk(ZX_DRV_INFO,  ##args)
#define zx_warning(args...) zx->printk(ZX_DRV_WARNING, ##args)

#define zx_assert(expr, fmt, args...) do {\
    if (!(expr)) { \
        zx->printk(ZX_DRV_ERROR, "Assert (%s) failed at %s:%d: " fmt "\n", #expr, __func__, __LINE__, ##args); \
        zx->panic(); \
    } } while(0)

#ifdef ZX_MUTEX_TRACK
#define zx_mutex_lock(mtx)          zx->mutex_lock_track((mtx), __FILE__, __LINE__)
#define zx_mutex_trylock(mtx)       zx->mutex_trylock_track((mtx), __FILE__, __LINE__)
#else
#define zx_mutex_lock(mtx)          zx->mutex_lock((mtx))
#define zx_mutex_trylock(mtx)       zx->mutex_trylock((mtx))
#endif

#if ZX_ALLOC_PAGE_TRACK
#define zx_allocate_pages_memory(pdev,size, page_size, flag) \
        zx->allocate_pages_memory_track(pdev, size, page_size, flag, __FILE__, __LINE__)
#define zx_free_pages_memory(pdev,memory)         \
        zx->free_pages_memory_track(pdev,memory, __FILE__, __LINE__)
#else
#define zx_allocate_pages_memory(pdev, size, page_size, flag) \
        zx->allocate_pages_memory_priv(pdev, size, page_size,  flag)
#define zx_free_pages_memory(pdev, memory)         \
        zx->free_pages_memory_priv(pdev,memory)
#endif

#if ZX_MAP_PAGES_TRACK
#define zx_map_pages_memory(argu)     \
        zx->map_pages_memory_track(argu, __FILE__, __LINE__)
#define zx_unmap_pages_memory(map) \
        zx->unmap_pages_memory_track(map, __FILE__, __LINE__)
#else
#define zx_map_pages_memory(argu)     \
        zx->map_pages_memory_priv(argu)
#define zx_unmap_pages_memory(vma) \
        zx->unmap_pages_memory_priv(vma)
#endif

#if ZX_MAP_IO_TRACK
#define zx_map_io_memory(argu) \
        zx->map_io_memory_track(argu, __FILE__, __LINE__)
#define zx_unmap_io_memory(argu) \
        zx->unmap_io_memory_track(argu, __FILE__, __LINE__)
#else
#define zx_map_io_memory(argu) \
        zx->map_io_memory_priv(argu)
#define zx_unmap_io_memory(argu) \
        zx->unmap_io_memory_priv(argu)
#endif

#ifdef _DEBUG_
#define ZX_MSG_LEVEL ZX_DRV_DEBUG
#define zx_debug(args...) zx_printk(ZX_DRV_DEBUG, ##args)
#else
#define ZX_MSG_LEVEL ZX_DRV_INFO
#define zx_debug(args...)
#endif

#define zx_register_trace_events         zx->register_trace_events
#define zx_unregister_trace_events       zx->unregister_trace_events
#define zx_task_create_trace_event       zx->task_create_trace_event
#define zx_task_enqueue_trace_event      zx->task_enqueue_trace_event
#define zx_task_submit_trace_event       zx->task_submit_trace_event
#define zx_task_release_trace_event      zx->task_release_trace_event
#define zx_fence_back_trace_event        zx->fence_back_trace_event
#define zx_begin_section_trace_event     zx->begin_section_trace_event
#define zx_end_section_trace_event       zx->end_section_trace_event
#define zx_counter_trace_event           zx->counter_trace_event
#define zx_allocation_trace_event        zx->allocation_trace_event

#define zx_query_platform_caps           zx->query_platform_caps
#define zx_secure_read32                 zx->secure_read32
#define zx_secure_write32                zx->secure_write32
#define zx_console_lock                  zx->console_lock

#define zx_create_workqueue                zx->create_workqueue
#define zx_init_work_struct                zx->init_work_struct
#define zx_deinit_work_struct              zx->deinit_work_struct
#define zx_get_work_struct_priv            zx->get_work_struct_priv
#define zx_queue_work                      zx->queue_work
#define zx_schedule_work                   zx->schedule_work
#define zx_flush_work                      zx->flush_work
#define zx_flush_workqueue                 zx->flush_workqueue

#endif /*__CORE_IMPORT_H__*/

