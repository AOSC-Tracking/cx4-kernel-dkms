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
#include "os_interface.h"
#include "zx_driver.h"
#include "zx_ioctl.h"
#include "zx_debugfs.h"
#include "zx_fence.h"
#include "zx_irq.h"
#include "zx_fbdev.h"
#include "zx_backlight.h"
#include "zx_params.h"

extern int zx_run_on_qt;
extern const struct attribute *zx_os_gpu_info;
extern const struct attribute_group zx_sysfs_group;
extern const struct bin_attribute zx_sysfs_trace_attr;

static int zx_init_procfs_entry(zx_card_t *zx);
static int zx_deinit_procfs_entry(zx_card_t *zx);

pgprot_t os_get_pgprot_val(unsigned int *cache_type, pgprot_t old_prot, int io_map)
{
    pgprot_t prot = old_prot;

    if(*cache_type == ZX_MEM_UNCACHED)
    {
        prot = pgprot_noncached(old_prot);
    }
    else if(*cache_type == ZX_MEM_WRITE_COMBINED)
    {
#ifdef CONFIG_X86
#ifdef CONFIG_X86_PAT
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,19,0)
        prot = __pgprot((pgprot_val(old_prot) & ~ _PAGE_CACHE_MASK) | cachemode2protval(_PAGE_CACHE_MODE_WC));
#else
        prot = __pgprot((pgprot_val(old_prot) & ~ _PAGE_CACHE_MASK) | _PAGE_CACHE_WC);
#endif
#else
        prot = pgprot_noncached(old_prot);

        *cache_type = ZX_MEM_UNCACHED;
#endif
#else
        prot = pgprot_writecombine(old_prot);
#endif

    }
#ifdef ZX_AHB_BUS
    else if(*cache_type == ZX_MEM_WRITE_BACK)
    {
        // for snoop path, current linux kennel defalut is not WA+WB
        prot = __pgprot((pgprot_val(old_prot)&(~(L_PTE_MT_MASK))) | (L_PTE_MT_WRITEBACK | L_PTE_MT_WRITEALLOC));
    }
#endif
    return prot;
}

#ifndef __frv__
int zx_map_system_io(struct vm_area_struct* vma, zx_map_argu_t *map)
{
    unsigned int  cache_type = map->flags.cache_type;
    int i = 0;
    unsigned long start = vma->vm_start;
    unsigned long pfn;
#if LINUX_VERSION_CODE >= 0x020612
    vma->vm_page_prot = vm_get_page_prot(vma->vm_flags);
#endif
    vma->vm_page_prot = os_get_pgprot_val(&cache_type, vma->vm_page_prot, 1);

    map->flags.cache_type = cache_type;

    if(map->node_num == 1){
        pfn = map->phys_addr >> PAGE_SHIFT;
        if(remap_pfn_range(vma, vma->vm_start, pfn, vma->vm_end-vma->vm_start, vma->vm_page_prot))
            return -EAGAIN;
    }else{
        for(i=0; i<map->node_num; i++)
        {
            pfn = map->phys_addrs[i] >> PAGE_SHIFT;
            if (remap_pfn_range(vma, start, pfn, map->node_sizes[i], vma->vm_page_prot))
                return -EAGAIN;

            start += map->node_sizes[i];
        }
    }

    return 0;
}

int zx_map_system_ram(struct vm_area_struct* vma, zx_map_argu_t *map)
{
    struct os_pages_memory *memory = map->memory;
    unsigned long start = vma->vm_start;
    unsigned long pfn;
    unsigned int  cache_type, curr_block_offset;

    int i;
    int start_page  = _ALIGN_DOWN(map->offset, PAGE_SIZE) / PAGE_SIZE;
    int end_page    = start_page + PAGE_ALIGN(map->size) / PAGE_SIZE;

#if DRM_VERSION_CODE <= 0x02040e
    vma->vm_flags |= VM_LOCKED;
#elif DRM_VERSION_CODE < 0x030700
    vma->vm_flags |= VM_RESERVED;
#else
#if DRM_VERSION_CODE < KERNEL_VERSION(6,3,0)
    vma->vm_flags |= VM_DONTEXPAND | VM_DONTDUMP;
#else
    vm_flags_set(vma, VM_DONTEXPAND | VM_DONTDUMP);
#endif
#endif

    cache_type = map->flags.cache_type;

#if LINUX_VERSION_CODE >= 0x020612
    vma->vm_page_prot = vm_get_page_prot(vma->vm_flags);
#endif
    vma->vm_page_prot = os_get_pgprot_val(&cache_type, vma->vm_page_prot, 0);
    for (i = start_page; i < end_page; i++)
    {
        struct page *pg = memory->pages[i];
        pfn = page_to_pfn(pg);
        if(remap_pfn_range(vma, start, pfn, PAGE_SIZE, vma->vm_page_prot))
        {
            return -EAGAIN;
        }
        start += PAGE_SIZE;
    }

    return 0;
}
#endif

static int zx_notify_fence_interrupt(zx_card_t *zx)
{
    return zx_core_interface->notify_interrupt(zx->adapter, INT_FENCE);
}

static int zx_notify_fence_power_tuning(zx_card_t *zx)
{
    return zx_core_interface->notify_power_tuning(zx->adapter, INT_FENCE);
}

static void zx_page_fault_func(struct work_struct *work)
{
    zx_card_t *zx = container_of(work, zx_card_t, page_fault_work);

    zx_core_interface->notify_page_fault(zx->adapter);
}

void zx_interrupt_init(zx_card_t *zx)
{
    tasklet_init(&zx->fence_notify, (void*)zx_notify_fence_interrupt, (unsigned long)zx);
    tasklet_disable(&zx->fence_notify);
    tasklet_enable(&zx->fence_notify);

    tasklet_init(&zx->fine_tuning_power_tuning, (void*)zx_notify_fence_power_tuning, (unsigned long)zx);
    tasklet_disable(&zx->fine_tuning_power_tuning);
    tasklet_enable(&zx->fine_tuning_power_tuning);

    INIT_WORK(&zx->page_fault_work, zx_page_fault_func);
}

void zx_interrupt_reinit(zx_card_t *zx)
{
    tasklet_enable(&zx->fence_notify);
    tasklet_enable(&zx->fine_tuning_power_tuning);
}

void zx_interrupt_deinit(zx_card_t *zx)
{
    tasklet_disable(&zx->fence_notify);
    synchronize_irq(to_pci_dev(zx->drm_dev->dev)->irq);
    tasklet_kill(&zx->fence_notify);

    tasklet_disable(&zx->fine_tuning_power_tuning);
    synchronize_irq(to_pci_dev(zx->drm_dev->dev)->irq);
    tasklet_kill(&zx->fine_tuning_power_tuning);

    cancel_work_sync(&zx->page_fault_work);
}

void zx_enable_interrupt(void *pdev_)
{
    struct drm_device *dev = _to_drm_device(pdev_);
    zx_card_t *zx = dev->dev_private;

    zx_disp_enable_interrupt(zx->disp_info);
}

void zx_disable_interrupt(void *pdev_)
{
    struct drm_device *dev = _to_drm_device(pdev_);
    zx_card_t *zx = dev->dev_private;

    zx_disp_disable_interrupt(zx->disp_info);

    cancel_work_sync(&zx->page_fault_work);
}

static void zx_allocate_trace_buffer(zx_card_t *zx)
{
    alloc_pages_flags_t alloc_flags = {0};
    zx_map_argu_t      map_argu = {0};
    int i;
    unsigned int page_cnt;

    alloc_flags.fixed_page = 1;
    alloc_flags.need_dma_map = 0;
    alloc_flags.need_zero  = 1;
    alloc_flags.need_flush = 1;
    zx->trace_buffer = zx_allocate_pages_memory_priv(zx->pdev, PAGE_SIZE, PAGE_SIZE, alloc_flags);
    if (!zx->trace_buffer)
    {
        zx->trace_buffer_vma = NULL;
        return;
    }

    map_argu.memory = zx->trace_buffer;
    map_argu.flags.read_only = false;
    map_argu.flags.mem_type = ZX_SYSTEM_RAM;
    map_argu.size = zx->trace_buffer->size;
    map_argu.offset = 0;
    map_argu.flags.cache_type = ZX_MEM_WRITE_BACK;
    zx->trace_buffer_vma = zx_map_pages_memory_priv(&map_argu);

    // zx_info("zx_allocate_trace_buffer: trace_buffer_vma->virt_addr=%px, pfn=0x%x\n",
    //     zx->trace_buffer_vma->virt_addr, page_to_pfn(zx->trace_buffer->pages[0]));
}

static void zx_free_trace_buffer(zx_card_t *zx)
{
    if (zx->trace_buffer_vma)
    {
        zx_unmap_pages_memory_priv(zx->trace_buffer_vma);
        zx->trace_buffer_vma = NULL;
    }
    if (zx->trace_buffer)
    {
        zx_free_pages_memory_priv(zx->pdev, zx->trace_buffer);
        zx->trace_buffer = NULL;
    }
}

static krnl_import_func_list_t zx_export =
{
    .create_range_allocator        = zx_create_range_allocator,

    .udelay                        = zx_udelay,
    .do_div                        = zx_do_div,
    .msleep                        = zx_msleep,
    .getsecs                       = zx_getsecs,
    .get_nsecs                     = zx_get_nsecs,
    .panic                         = zx_panic,
    .dump_stack                    = zx_dump_stack,
    .copy_from_user                = zx_copy_from_user,
    .copy_to_user                  = zx_copy_to_user,
    .memset                        = zx_memset,
    .memcpy                        = zx_memcpy,
    .memcmp_priv                   = zx_memcmp,
    .byte_copy                     = zx_byte_copy,
    .strcmp                        = zx_strcmp,
    .strcpy                        = zx_strcpy,
    .strncmp                       = zx_strncmp,
    .strncpy                       = zx_strncpy,
    .strlen                        = zx_strlen,
    .strsep                        = zx_strsep,
    .read64                        = zx_read64,
    .read32                        = zx_read32,
    .read16                        = zx_read16,
    .read8                         = zx_read8,
    .write32                       = zx_write32,
    .write16                       = zx_write16,
    .write8                        = zx_write8,
    .file_open                     = zx_file_open,
    .file_close                    = zx_file_close,
    .file_read                     = zx_file_read,
    .file_write                    = zx_file_write,
    .vsprintf                      = zx_vsprintf,
    .vsnprintf                     = zx_vsnprintf,
    .sscanf                        = zx_sscanf,
    .printk                        = zx_printk,
    .cb_printk                     = zx_cb_printk,
    .seq_printf                    = zx_seq_printf,
    .find_first_zero_bit           = zx_find_first_zero_bit,
    .find_next_zero_bit            = zx_find_next_zero_bit,
    .find_first_bit                = zx_find_first_bit,
    .find_next_bit                 = zx_find_next_bit,
    .set_bit                       = zx_set_bit,
    .clear_bit                     = zx_clear_bit,
    .test_bit                      = zx_test_bit,
    .create_thread                 = zx_create_thread,
    .destroy_thread                = zx_destroy_thread,
    .thread_should_stop            = zx_thread_should_stop,
    .create_atomic                 = zx_create_atomic,
    .destroy_atomic                = zx_destroy_atomic,
    .atomic_read                   = zx_atomic_read,
    .atomic_inc                    = zx_atomic_inc,
    .atomic_dec                    = zx_atomic_dec,
    .atomic_cmpxchg                = zx_atomic_cmpxchg,
    .atomic_xchg                   = zx_atomic_xchg,
    .create_atomic64               = zx_create_atomic64,
    .destroy_atomic64              = zx_destroy_atomic64,
    .atomic64_read                 = zx_atomic64_read,
    .atomic64_inc                  = zx_atomic64_inc,
    .atomic64_dec                  = zx_atomic64_dec,
    .atomic64_cmpxchg              = zx_atomic64_cmpxchg,
    .atomic64_xchg                 = zx_atomic64_xchg,
    .create_mutex                  = zx_create_mutex,
    .destroy_mutex                 = zx_destroy_mutex,
    .mutex_lock_track              = zx_mutex_lock_track,
    .mutex_trylock_track           = zx_mutex_trylock_track,
    .mutex_lock                    = zx_mutex_lock_priv,
    .mutex_trylock                 = zx_mutex_trylock_priv,
    .mutex_lock_killable           = zx_mutex_lock_killable,
    .mutex_unlock                  = zx_mutex_unlock,
    .create_sema                   = zx_create_sema,
    .destroy_sema                  = zx_destroy_sema,
    .down                          = zx_down,
    .down_trylock                  = zx_down_trylock,
    .up                            = zx_up,
    .read_sema                     = zx_read_sema,
    .create_rwsema                 = zx_create_rwsema,
    .destroy_rwsema                = zx_destroy_rwsema,
    .down_read                     = zx_down_read,
    .down_write                    = zx_down_write,
    .up_read                       = zx_up_read,
    .up_write                      = zx_up_write,
    .create_spinlock               = zx_create_spinlock,
    .destroy_spinlock              = zx_destroy_spinlock,
    .spin_lock                     = zx_spin_lock,
    .spin_unlock                   = zx_spin_unlock,
    .spin_lock_irqsave             = zx_spin_lock_irqsave,
    .spin_unlock_irqrestore        = zx_spin_unlock_irqrestore,
    .spin_lock_bh                  = zx_spin_lock_bh,
    .spin_unlock_bh                = zx_spin_unlock_bh,
    .create_event                  = zx_create_event,
    .destroy_event                 = zx_destroy_event,
    .wait_event_thread_safe        = zx_wait_event_thread_safe,
    .wait_event                    = zx_wait_event,
    .wake_up_event                 = zx_wake_up_event,
    .thread_wait                   = zx_thread_wait,
    .create_wait_queue             = zx_create_wait_queue,
    .wait_for_events               = thread_wait_for_events,
    .thread_wake_up                = zx_thread_wake_up,
    ._thread_wake_up               = general_thread_wake_up,
    .try_to_freeze                 = zx_try_to_freeze,
    .freezable                     = zx_freezable,
    .clear_freezable               = zx_clear_freezable,
    .set_freezable                 = zx_set_freezable,
    .freezing                      = zx_freezing,
    .get_current_pid               = zx_get_current_pid,
    .get_current_tid               = zx_get_current_tid,
    .flush_cache                   = zx_flush_cache,
    .inv_cache                     = zx_inv_cache,
    .get_page_phys_address         = zx_get_page_phys_address,
    .pages_memory_for_each_continues = zx_pages_memory_for_each_continues,
    .for_each_pages_memory         = zx_for_each_pages_memory,
    .ioremap                       = zx_ioremap,
    .iounmap_priv                  = zx_iounmap,
    .mtrr_add                      = zx_mtrr_add,
    .mtrr_del                      = zx_mtrr_del,
    .get_mem_info                  = zx_get_mem_info,
    .pages_memory_swapout          = zx_pages_memory_swapout,
    .pages_memory_swapin           = zx_pages_memory_swapin,
    .release_file_storage          = zx_release_file_storage,
    .get_bus_config                = zx_get_bus_config,
    .get_platform_config           = zx_get_platform_config,
    .get_command_status16          = zx_get_command_status16,
    .write_command_status16        = zx_write_command_status16,
    .get_command_status32          = zx_get_command_status32,
    .write_command_status32        = zx_write_command_status32,
    .get_rom_start_addr            = zx_get_rom_start_addr,
    .get_rom_save_addr             = zx_get_rom_save_addr,
    .write_rom_save_addr           = zx_write_rom_save_addr,
    .get_bar1                      = zx_get_bar1,

    .malloc_track                  = zx_malloc_track,
    .calloc_track                  = zx_calloc_track,
    .free_track                    = zx_free_track,
    .malloc_priv                   = zx_malloc_priv,
    .calloc_priv                   = zx_calloc_priv,
    .free_priv                     = zx_free_priv,
    .allocate_pages_memory_track   = zx_allocate_pages_memory_track,
    .free_pages_memory_track       = zx_free_pages_memory_track,
    .allocate_pages_memory_priv    = zx_allocate_pages_memory_priv,
    .free_pages_memory_priv        = zx_free_pages_memory_priv,

    .map_pages_memory_track        = zx_map_pages_memory_track,
    .unmap_pages_memory_track      = zx_unmap_pages_memory_track,
    .map_pages_memory_priv         = zx_map_pages_memory_priv,
    .unmap_pages_memory_priv       = zx_unmap_pages_memory_priv,
    .map_io_memory_track           = zx_map_io_memory_track,
    .unmap_io_memory_track         = zx_unmap_io_memory_track,
    .map_io_memory_priv            = zx_map_io_memory_priv,
    .unmap_io_memory_priv          = zx_unmap_io_memory_priv,
    .register_trace_events         = zx_register_trace_events,
    .unregister_trace_events       = zx_unregister_trace_events,
    .task_create_trace_event       = zx_task_create_trace_event,
    .task_enqueue_trace_event      = zx_task_enqueue_trace_event,
    .task_submit_trace_event       = zx_task_submit_trace_event,
    .task_release_trace_event      = zx_task_release_trace_event,
    .fence_back_trace_event        = zx_fence_back_trace_event,
    .begin_section_trace_event     = zx_begin_section_trace_event,
    .end_section_trace_event       = zx_end_section_trace_event,
    .counter_trace_event           = zx_counter_trace_event,
    .allocation_trace_event        = zx_allocation_trace_event,

    .query_platform_caps           = zx_query_platform_caps,

    .console_lock                  = zx_console_lock,
    .enable_interrupt              = zx_enable_interrupt,
    .disable_interrupt             = zx_disable_interrupt,
    .os_printf                     = zx_printf,

    .create_workqueue              = zx_create_workqueue,
    .init_work_struct              = zx_init_work_struct,
    .deinit_work_struct            = zx_deinit_work_struct,
    .get_work_struct_priv          = zx_get_work_struct_priv,
    .queue_work                    = zx_queue_work,
    .schedule_work                 = zx_schedule_work,
    .flush_work                    = zx_flush_work,
    .flush_workqueue               = zx_flush_workqueue,
    .disp_wait_idle                = zx_disp_wait_idle,
    .mb                            = zx_mb,
    .rmb                           = zx_rmb,
    .wmb                           = zx_wmb,
    .flush_wc                      = zx_flush_wc,
    .dsb                           = zx_dsb,

    .fence_update_value            = zx_dma_fence_update_value,
    .fence_release                 = zx_dma_fence_release_cb,
    .fence_notify_event            = zx_dma_fence_notify_event,
    .fence_is_signaled             = zx_dma_fence_is_signaled,
    .fence_need_send_irq           = zx_dma_fence_need_send_irq,

};

/***/
#include <linux/proc_fs.h>

#ifdef NO_PROC_CREATE_FUNC
static int zx_read_proc(char *page, char **start, off_t off,
               int count, int *eof, void *data)
{

    zx_card_t    *zx = data;
    struct os_printer p = zx_info_printer(zx);
    zx_core_interface->dump_resource(&p, zx->adapter, 5, 0);

    return 0;
}

static int zx_gpuinfo_read_proc(char *page, char **start, off_t off,
               int count, int *eof, void *data)
{
    return 0;
}
#else
static ssize_t zx_proc_read(struct file *filp, char *buf, size_t count, loff_t *offp)
{
#if DRM_VERSION_CODE < KERNEL_VERSION(5, 17, 0)
    zx_card_t *zx = PDE_DATA(file_inode(filp));
#else
    zx_card_t *zx = pde_data(file_inode(filp));
#endif
    struct os_printer p = zx_info_printer(zx);
    zx_core_interface->dump_resource(&p, zx->adapter, 1, 0);

    return 0;
}

extern int hwq_get_hwq_info(void *adp, zx_hwq_info *hwq_info);
static ssize_t zx_gpuinfo_proc_read(struct file *filp, char *buf, size_t count, loff_t *offp)
{
#if DRM_VERSION_CODE < KERNEL_VERSION(5, 17, 0)
    zx_card_t *zx = PDE_DATA(file_inode(filp));
#else
    zx_card_t *zx = pde_data(file_inode(filp));
#endif

    zx_hwq_info hwq_info;
    zx_query_info_t query_info;
    struct pci_dev *pdev = zx->pdev;
    adapter_info_t *adapter_info = &zx->adapter_info;
    disp_info_t *disp_info = (disp_info_t *)zx->disp_info;

    int ret = 0;
    size_t len = 0;
    char *buffer = NULL, *pmp_version = NULL, *product_name = NULL, *type_dp = "\0";
    char *output_type_vga_hdmi = "VGA, HDMI", *output_type_full = "VGA, HDMI, DP", *dp_resolution = "3840 x 2160", *hdmi_resolution = "3840 x 2160", *vga_resolution = "1920 x 1080", *output_type = output_type_full, *vbios_name = "ZhaoXin Video Bios";
    unsigned int eclk, free_mem, mem_usage, usage_3d, usage_vcp, usage_vpp;
    unsigned int technology, pixel_fillrate, texture_fillrate;
    unsigned int output_cnt = 3, hdmi_fps, dp_fps;
    unsigned int gpu_temperature = 0;
    int vbios_version = 0;

    if (!offp || *offp)
        goto exit;

    buffer = zx_calloc(1024);
    if (!buffer)
        goto exit_nomem;

    zx_core_interface->ctl_flags_set(zx->adapter, 1, (1UL << 21), (1 << 21));
    zx_core_interface->ctl_flags_set(zx->adapter, 1, (1UL << 9), (1 << 9));

    if(DISP_OK == disp_cbios_get_clock(disp_info, ZX_QUERY_ENGINE_CLOCK, &eclk))
        eclk /= 1000;
    else
        eclk = 0;

    query_info.type = ZX_QUERY_SEGMENT_FREE_SIZE;
    query_info.argu = 1;// local
    zx_core_interface->query_info(zx->adapter, &query_info);
    free_mem = query_info.signed_value;

    free_mem /= 1024;

    mem_usage = ((adapter_info->total_mem_size_mb - free_mem) * 100) / adapter_info->total_mem_size_mb;

    ret = hwq_get_hwq_info(zx->adapter, &hwq_info);
    if (!ret)
    {
        usage_3d = hwq_info.Usage_3D;
        usage_vcp = hwq_info.Usage_VCP;
        usage_vpp = hwq_info.Usage_VPP;
    }
    else
    {
        usage_3d = usage_vcp = usage_vpp = 0;
    }

    gpu_temperature = zx_get_chip_temp(zx->disp_info);

    switch (pdev->device)
    {
    case 0x3d01:
        dp_fps = 60;
        hdmi_fps = 60;
        technology = 16;
        pixel_fillrate = 16 * eclk;
        texture_fillrate = pixel_fillrate *2;
        product_name = "ZX KX-6000G C-1080";
        break;

    case 0x3a05:
        dp_fps = 60;
        hdmi_fps = 60;
        technology = 16;
        pixel_fillrate = 16 * eclk;
        texture_fillrate = pixel_fillrate *2;
        product_name = "ZX KX-7000 C-1190";
        break;

    default:
        dp_fps = 60;
        hdmi_fps = 60;
        technology = 0;
        pixel_fillrate = 0;
        texture_fillrate = 0;
        product_name = "ZX N/A";
        break;
    }
    vbios_version = disp_info->vbios_version;

    len += sprintf(buffer + len, "Vendor : %s\n", DRIVER_VENDOR);
    len += sprintf(buffer + len, "Product Name : %s\n", product_name);
    len += sprintf(buffer + len, "Vendor ID : %X\n", pdev->vendor);
    len += sprintf(buffer + len, "Device ID : %X\n", pdev->device);
    len += sprintf(buffer + len, "Technology : %u nm\n", technology);
    len += sprintf(buffer + len, "Driver Version : %02x.%02x.%02x%s\n", DRIVER_MAJOR, DRIVER_MINOR, DRIVER_PATCHLEVEL, DRIVER_CLASS);
    //uma use Vbios instead of FW
    len += sprintf(buffer + len, "Firmware Version : %02x.%02x.%02x.%02x\n", (vbios_version>>24)&0xff, (vbios_version>>16)&0xff, (vbios_version>>8)&0xff, vbios_version&0xff);
    len += sprintf(buffer + len, "Firmware Name : %s\n", vbios_name);
    len += sprintf(buffer + len, "Pixel Fillrate : %d MPixel/s\n", pixel_fillrate);
    len += sprintf(buffer + len, "Texture Fillrate : %d MTexel/s\n", texture_fillrate);
    len += sprintf(buffer + len, "Memory Size : %d MB\n", adapter_info->total_mem_size_mb);
    len += sprintf(buffer + len, "Memory ddr Remain Size : %d MB\n", free_mem);
    len += sprintf(buffer + len, "Max Display Port : %d\n", output_cnt);
    len += sprintf(buffer + len, "Support Display Type : %s\n", output_type);
    len += sprintf(buffer + len, "DP%s Max Display Resolution : %s@%dHz\n", type_dp, dp_resolution, dp_fps);
    len += sprintf(buffer + len, "HDMI%s Max Display Resolution : %s@%dHz\n", type_dp, hdmi_resolution, hdmi_fps);
    len += sprintf(buffer + len, "VGA Max Display Resolution : %s@60Hz\n", vga_resolution);
    len += sprintf(buffer + len, "Realtime Temperature : %d Degree\n",gpu_temperature & 0xFFFFF);

    len += sprintf(buffer + len, "GPU ENGINE CLOCK : %d MHz\n", eclk);
    len += sprintf(buffer + len, "GPU Memory Usage : %d%%\n", mem_usage);
    len += sprintf(buffer + len, "GPU Use Rate(3d) : %d%%\n", usage_3d);
    len += sprintf(buffer + len, "GPU Use Rate(video) : %d%%\n", usage_vcp);
    len += sprintf(buffer + len, "GPU Use Rate(vpp) : %d%%\n", usage_vpp);

    ret = copy_to_user(buf, buffer, len);
    zx_free(buffer);

    if (ret)
        goto exit_err;

    *offp += len;

    return len;

exit_err:
    return -EFAULT;

exit_nomem:
    return -ENOMEM;

exit:
    return 0;
}

#define ZX_DUMP_POWER_STATUS_INDEX    0x1000
#define ZX_DUMP_SENSOR_TEMP_INDEX     0x2000
#define ZX_DUMP_FENCE_INDEX           0x5000
#define ZX_CLEAR_FENCE_INDEX          0x6000
#define ZX_FORCE_WAKEUP               0x7000
#define ZX_DO_TEST                    0x7001
#define ZX_DUMP_MEMTRACK              0x8000
#define ZX_CLEAR_MEMTRACK             0x8001

#define ZX_DUMP_COMPRESS_PS_INDEX    (0x100 + ZX_STREAM_PS)
#define ZX_DUMP_COMPRESS_SS_INDEX    (0x100 + ZX_STREAM_SS)
#define ZX_DUMP_COMPRESS_TS_INDEX    (0x100 + ZX_STREAM_TS)
#define ZX_DUMP_COMPRESS_4S_INDEX    (0x100 + ZX_STREAM_4S)
#define ZX_DUMP_COMPRESS_5S_INDEX    (0x100 + ZX_STREAM_5S)
#define ZX_DUMP_COMPRESS_6S_INDEX    (0x100 + ZX_STREAM_6S)
#define ZX_DUMP_COMPRESS_MAX_INDEX   (0x100 + ZX_STREAM_MAX)


static ssize_t zx_proc_write(struct file *filp, const char __user *buf, size_t count, loff_t *offp)
{
#if DRM_VERSION_CODE < KERNEL_VERSION(5, 17, 0)
    zx_card_t *zx = PDE_DATA(file_inode(filp));
#else
    zx_card_t *zx = pde_data(file_inode(filp));
#endif

    char mode = '0';
    int dump_index = 0;
    unsigned int ret = 0;
    char temp[20] = {0};
    unsigned int interval = 0, crtc = 0;
    unsigned long value = 0;

//maybe later we can change it to using zx_seq_file_printer
    struct os_printer p = zx_info_printer(zx);

    if(count > 1 && count < 20)
    {
        ret = copy_from_user(&temp[0], buf, count);
        temp[count-1] = '\0';

        if(count == 2)
        {
            mode = temp[0];

            if(mode >= '0' && mode <= '9')
            {
                dump_index = (mode - '0');
            }
            else if((mode >= 'a' && mode < 'e'))
            {
                dump_index =  mode - 'a' + 0xa;
            }
            else if(mode == 'p')
            {
                dump_index = ZX_DUMP_POWER_STATUS_INDEX;
            }
            else if (mode == 'f')
            {
                dump_index = ZX_DUMP_FENCE_INDEX;
            }
            else if (mode == 'g')
            {
                dump_index = ZX_CLEAR_FENCE_INDEX;
            }
            else if (mode == 'w')
            {
                dump_index = ZX_FORCE_WAKEUP;
            }
            else if (mode == 'm')
            {
                dump_index = ZX_DUMP_MEMTRACK;
            }
            else if (mode == 'n')
            {
                dump_index = ZX_CLEAR_MEMTRACK;
            }
            else if (mode == 't')
            {
                dump_index = ZX_DO_TEST;
            }
            else
            {
               zx_error("echo error proc mode, should between 0 and d \n");
               return -EFAULT;
            }
        }
        else
        {
            value = simple_strtoul(&temp[1], NULL, 16);

            zx_info("value %d \n", value);

            if((temp[0] == 'c' || temp[0] == 'C') &&
                    (temp[2] == 's' || temp[2] == 'S'))
            {
                if(zx_strlen(temp) != 4)
                {
                    zx_info("invalid flag buffer dump parameter!\n");
                }

                crtc = simple_strtoul(&temp[1], NULL, 16);
                dump_index = simple_strtoul(&temp[3], NULL, 16);

                dump_index += ZX_DUMP_COMPRESS_PS_INDEX;
            }
            else
            {
               zx_error("invalid index\n");
            }
         }
    }
    else
    {
        zx_error("proc write pass too long param, should less then 20\n");
    }

    if (dump_index == ZX_DUMP_FENCE_INDEX)
    {
        zx_dma_track_fences_dump(zx);
    }
    else if (dump_index == ZX_CLEAR_FENCE_INDEX)
    {
        zx_dma_track_fences_clear(zx);
    }
    else if((dump_index >=0 && dump_index <=5) || dump_index == ZX_DUMP_POWER_STATUS_INDEX)
    {
        zx_core_interface->dump_resource(&p, zx->adapter, dump_index, 0);
    }
    else if (dump_index == ZX_FORCE_WAKEUP)
    {
        zx_core_interface->notify_interrupt(zx->adapter, INT_FENCE);
        zx_core_interface->dump_resource(&p, zx->adapter, dump_index, 0);
    }
#if ZX_MALLOC_TRACK | ZX_ALLOC_PAGE_TRACK | ZX_MAP_PAGES_TRACK | ZX_MAP_IO_TRACK
    else if (dump_index == ZX_DUMP_MEMTRACK)
    {
        zx_mem_track_list_result();
    }
    else if (dump_index == ZX_CLEAR_MEMTRACK)
    {
        zx_mem_track_clear_result();
    }
#endif

    return count;
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,6,0)
static const struct proc_ops zx_proc_fops =
{
    .proc_read  = zx_proc_read,
    .proc_write = zx_proc_write,
};

static const struct proc_ops zx_gpuinfo_proc_fops =
{
    .proc_read  = zx_gpuinfo_proc_read,
};

#else
static const struct file_operations zx_proc_fops =
{
    .owner = THIS_MODULE,
    .read  = zx_proc_read,
    .write = zx_proc_write,
};

static const struct file_operations zx_gpuinfo_proc_fops =
{
    .owner = THIS_MODULE,
    .read  = zx_gpuinfo_proc_read,
};

#endif
#endif

void zx_disable_async_suspend(zx_card_t *zx)
{
    struct pci_dev *pdev = zx->pdev;
    struct pci_dev *pdev_audio = NULL;
    unsigned int devfn_audio;

    pdev->dev.power.async_suspend = 0;

    devfn_audio = PCI_DEVFN(PCI_SLOT(pdev->devfn), 1);
    pdev_audio = pci_get_slot(pdev->bus, devfn_audio);

    if(pdev_audio)
    {
        zx_info("===== found audio device, devfn = 0x%x \n", pdev_audio->devfn);
        pdev_audio->dev.power.async_suspend = 0;
    }
    else
    {
        zx_error("===== Can't find audio device.\n");
    }
}

#define SET_FIRST_BIT(reg, value)    value ? (reg | 0x1) : (reg & (~0x1))
#define MSR_IA32_CTL    0x1936
int zx_power_supply_notify(struct notifier_block *nb,unsigned long event,void *data)
{
    zx_card_t * zx = container_of(nb,zx_card_t, power_supply_nb);
    int ac_power_status = 0;
    unsigned int lo, hi;

    if (zx == NULL)
    {
        zx_error("zx is NULL. maybe an error\n");
        goto exit;
    }
    ac_power_status = (power_supply_is_system_supplied() > 0) ? AC_POWER_SUPPLY:DC_POWER_SUPPLY;

    if (zx->adapter_info.chip_id == CHIP_CNE001)
    {
        rdmsr(MSR_IA32_CTL, lo, hi);
        hi = SET_FIRST_BIT(hi, (ac_power_status == DC_POWER_SUPPLY));
        wrmsr(MSR_IA32_CTL, lo, hi);
    }

exit:
    return 0;
}

void zx_init_signal(zx_card_t* zx_card)
{
    zx_card->signal_poll_status = 0;
    INIT_WORK(&zx_card->signal_poll_work, zx_poll_signal_work_func);
}

void  zx_deinit_signal(zx_card_t* zx_card)
{
    cancel_work_sync(&zx_card->signal_poll_work);
}

int zx_card_init(zx_card_t *zx, void *pdev)
{
    int ret = 0;

    spin_lock_init(&zx->gem_gid_lock);
    idr_init(&zx->gem_gid_table);

    spin_lock_init(&zx->dev_table_lock);
    idr_init(&zx->dev_idr);

    zx_init_procfs_entry(zx);
    zx->poll_lock = zx_create_spinlock();

    zx_init_bus_id(zx);

#ifdef ZX_PCIE_BUS
    zx->support_msi = 1;
#else
    zx->support_msi = 0;
#endif

#ifdef CONFIG_X86
#ifdef CONFIG_HYPERVISOR_GUEST
#if LINUX_VERSION_CODE < KERNEL_VERSION(4,14,9)
    if(x86_hyper)
    {
        zx_info("detect hypervisor: %s\n", x86_hyper->name);

        zx->adapter_info.run_on_hypervisor = 1;
    }
#else
    if(!hypervisor_is_type(X86_HYPER_NATIVE))
    {
        zx_info("detect hypervisor: %d\n", x86_hyper_type);

        zx->adapter_info.run_on_hypervisor = 1;
    }
#endif
#endif
#endif

#if 0
    // keep head range memory unused for host use if driver run in virtual machine,
    // because host always use this head memory range as frame buffer, guest should
    // not touch this memory.
    if(zx->adapter_info.run_on_hypervisor)
    {
        // patch reserved video memory size.
        zx->reserved_vmem += vmem_reserve_for_host;
    }
#endif

    zx->adapter = zx_core_interface->pre_init_adapter(pdev, &zx->a_info, &zx_export);

    if(zx->adapter == NULL)
    {
        ret = -1;

        zx_error("init adapter failed\n");
    }

    // this should be put before init mode set, since init mode set will
    // set up interrrupt, and if dvfs interrupt comes, and dvfs_info not allocated
    // may cause kernel crash

    zx->dvfs_info = zx_calloc(sizeof(zx_dvfs_info_t));
    if(zx->dvfs_info == NULL)
    {
        ret = -1;

        zx_error("init adapter dvfs info failed\n");
    }

    zx_init_modeset(zx->drm_dev);

    zx_disable_async_suspend(zx);

    zx_core_interface->init_adapter(zx->adapter, zx->reserved_vmem, zx->disp_info);


#ifndef ZX_HW_NULL
    zx_interrupt_init(zx);
#endif

    zx->lock = zx_create_mutex();

    ret = sysfs_create_files(&zx->pdev->dev.kobj, &zx_os_gpu_info);
    if(ret)
    {
        zx_warning("Could not create device attr\n");
    }

    ret = sysfs_create_group(&zx->pdev->dev.kobj,&zx_sysfs_group);

    if(ret)
    {
        zx_warning("Could not create attr group\n");
    }

    zx_backlight_init(pdev, zx);

    zx_allocate_trace_buffer(zx);
    ret = sysfs_create_bin_file(&zx->pdev->dev.kobj, &zx_sysfs_trace_attr);
    if(ret)
    {
        zx_warning("Could not create sysfs trace attr\n");
    }

    zx->power_supply_nb.notifier_call = zx_power_supply_notify;
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,14,0)
    power_supply_reg_notifier(&zx->power_supply_nb);
#endif

    zx_power_supply_notify(&zx->power_supply_nb,0,NULL);

    zx_init_signal(zx);

    zx->fps_count=0;
    zx->rxa_blt_scn_cnt=0;
    return ret;
}

int zx_card_deinit(zx_card_t *zx)
{
    char *name;

#if DRM_VERSION_CODE < KERNEL_VERSION(4, 19, 0)
    zx_fbdev_deinit(zx);
#endif

    zx_core_interface->wait_chip_idle(zx->adapter);

    zx_deinit_signal(zx);

    zx_deinit_modeset(zx->drm_dev);

#ifndef ZX_HW_NULL
    zx_interrupt_deinit(zx);
#endif
    zx_core_interface->deinit_adapter(zx->adapter);

    zx->len = 0;

    if(zx->lock)
    {
        zx_destroy_mutex(zx->lock);
        zx->lock = NULL;
    }

    if(zx->dvfs_info)
    {
        zx_free(zx->dvfs_info);
    }
    if (zx->reserved_dev)
    {
        zx_debugfs_remove_device_node(zx->debugfs_dev, zx->reserved_dev);

        zx->reserved_dev = NULL;
    }

    if(zx->debugfs_dev)
    {
        zx_debugfs_destroy((zx_debugfs_device_t*)(zx->debugfs_dev));
        zx->debugfs_dev = NULL;
    }

    sysfs_remove_files(&zx->pdev->dev.kobj, &zx_os_gpu_info);

    sysfs_remove_group(&zx->pdev->dev.kobj, &zx_sysfs_group);

    if(zx->backlight_dev)
    {
        zx_backlight_deinit(zx);
        zx->backlight_dev = NULL;
    }

    if(zx->procfs_entry)
    {
        zx_deinit_procfs_entry(zx);
        zx->procfs_entry = NULL;
    }
    zx_destroy_spinlock(zx->poll_lock);
    sysfs_remove_bin_file(&zx->pdev->dev.kobj, &zx_sysfs_trace_attr);
    zx_free_trace_buffer(zx);

#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,14,0)
    power_supply_unreg_notifier(&zx->power_supply_nb);
#endif

    zx->fps_count=0;
    return 0;
}

unsigned int zx_poll(struct file *filp, poll_table *wait)
{
    #if DRM_VERSION_CODE < KERNEL_VERSION(5, 17, 0)
    zx_card_t *zx = PDE_DATA(file_inode(filp));
#else
    zx_card_t *zx = pde_data(file_inode(filp));
#endif
    struct os_wait_queue *poll_queue = NULL;
    unsigned int mask = 0;

    if(!zx->output_conflict_poll)
    {
        return mask;
    }
    poll_queue = zx->output_conflict_poll->queue;

    poll_wait(filp, &poll_queue->wait_queue, wait);
    if(zx->output_conflict_poll->event_status)
    {
        mask |= POLLIN;
    }
    zx->output_conflict_poll->event_status = false;
    return mask;
}

int zx_poll_file_attach_queue(zx_card_t *zx)
{
    int ret = -1;
    struct zx_output_conflict_poll *data = zx_calloc(sizeof(struct zx_output_conflict_poll));

    if(data != NULL)
    {
        data->queue = zx_create_wait_queue();
        data->event_status = false;
        zx->output_conflict_poll = data;
        ret = 0;
    }
    return ret;
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,6,0)
static const struct proc_ops zx_poll_fops = {
    .proc_poll = zx_poll
};
#else
static const struct file_operations zx_poll_fops = {
    .poll = zx_poll
};
#endif

static int zx_init_procfs_entry(zx_card_t *zx)
{
    char name[64] = "";

    zx_vsnprintf(name, 64, "driver/zx_poll%d", zx->index);
    zx->procfs_poll_entry = proc_create_data(name, 0, NULL, &zx_poll_fops, zx);

    zx_vsnprintf(name, 64, "driver/dri%d", zx->index);

#ifdef NO_PROC_CREATE_FUNC
    //create_proce_read_entry is not used any more in Linux 3.10 and above. use proc_create_data instead.
    zx->procfs_entry = create_proc_read_entry(name, 0, NULL, zx_read_proc, zx);

    zx_vsnprintf(name, 64, "gpuinfo_%d", zx->index);
    zx->procfs_gpuinfo_entry = create_proc_read_entry(name, 0, NULL, zx_gpuinfo_read_proc, zx);
#else
    zx->procfs_entry = proc_create_data(name, 0, NULL, &zx_proc_fops, zx);

    zx_vsnprintf(name, 64, "gpuinfo_%d", zx->index);
    zx->procfs_gpuinfo_entry = proc_create_data(name, 0, NULL, &zx_gpuinfo_proc_fops, zx);
    zx_vsnprintf(name, 64, "driver/dri%d", zx->index);
#endif

    if(zx_poll_file_attach_queue(zx))
    {
        zx_warning("poll attach queue fail!");
    }
    zx_info("%s() name-%s, procfs_entry-0x%x\n", __func__, name, zx->procfs_entry);

    return 0;
}

static int zx_deinit_procfs_entry(zx_card_t *zx)
{
    char name[64] = "";

    zx_vsnprintf(name, 64, "driver/dri%d", zx->index);
#ifdef NO_PROC_CREATE_FUNC
    //this path is not verified yet
    remove_proc_entry(name, zx->procfs_entry);

    zx_vsnprintf(name, 64, "driver/zx_poll%d", zx->index);
    remove_proc_entry(name, zx->procfs_poll_entry);

    zx_vsnprintf(name, 64, "gpuinfo_%d", zx->index);
    remove_proc_entry(name, zx->procfs_gpuinfo_entry);
#else
    proc_remove(zx->procfs_entry);
    zx_vsnprintf(name, 64, "driver/zx_poll%d", zx->index);
    proc_remove(zx->procfs_poll_entry);
    proc_remove(zx->procfs_gpuinfo_entry);
#endif
    if(zx->output_conflict_poll)
    {
        zx_free(zx->output_conflict_poll->queue);
    }
    zx_free(zx->output_conflict_poll);

    return 0;
}

