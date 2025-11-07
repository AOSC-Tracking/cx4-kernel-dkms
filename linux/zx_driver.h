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
#ifndef __ZX_DRIVER_H
#define __ZX_DRIVER_H

#include "zx.h"
#include "zx_def.h"
#include "zx_types.h"
#include "os_interface.h"
#include "kernel_interface.h"
#include "zx_device_debug.h"

#define ZX_POLL_SIGNAL         1
#define ZX_SEND_INTERRUPT      2

typedef struct zx_file zx_file_t;

typedef int (*zx_ioctl_t)(struct drm_device *dev, void *data,struct drm_file *filp);

typedef int (*irq_func_t)(void*);

typedef struct  zx_dvfs_info
{
    unsigned int dvfs_3d_checksize;
    unsigned int dvfs_3d_change_num;
    unsigned int dvfs_vcp0_checksize;
    unsigned int dvfs_vcp0_change_num;
    unsigned int dvfs_vcp1_checksize;
    unsigned int dvfs_vcp1_change_num;
    unsigned int dvfs_vpp_checksize;
    unsigned int dvfs_vpp_change_num;
}zx_dvfs_info_t;

struct zx_output_conflict_poll
{
    struct os_wait_queue *queue;
    int event_status; //poll_event status
    int conflict_status; //output conflict status
};
typedef struct
{
    void              *adapter;
    struct pci_dev    *pdev;

    struct drm_device *drm_dev;
    adapter_info_t    adapter_info;
    void*             disp_info;
    char              busId[64];
    int               len;
    int               index;

    struct os_mutex   *lock;

    int               support_msi;

    struct tasklet_struct fence_notify;
    struct tasklet_struct fine_tuning_power_tuning;
    struct work_struct  page_fault_work;

    int               reserved_vmem;

    void                *debugfs_dev;
    void                *reserved_dev;  // debugfs node for reserved_device/reserved_vma
    struct zx_dma_fence_driver *fence_drv;
    void                *fbdev;
    struct backlight_device *backlight_dev;
    bool have_disp_power_ref;
    struct krnl_adapter_init_info_s a_info;
    struct proc_dir_entry *procfs_entry;
    struct proc_dir_entry *procfs_poll_entry;
    struct proc_dir_entry *procfs_gpuinfo_entry;
    struct zx_output_conflict_poll *output_conflict_poll;

    spinlock_t          dev_table_lock;
    struct idr          dev_idr;

    spinlock_t          gem_gid_lock;
    struct idr          gem_gid_table;  // just for debug usage, not hold refcnt of gem object
    unsigned long       umd_trace_tags; // usermode trace tags, 0 to disable.
    struct os_pages_memory *trace_buffer;
    zx_cpu_vm_area_t    *trace_buffer_vma;
    zx_test_mem_para_t  test_mem_para;
    zx_dvfs_info_t      *dvfs_info;
    void*              primary_allocation[MAX_CORE_CRTCS];

#define ZX_S4_RESUME 0x01
    unsigned int flags;
    unsigned int fps_count;
    unsigned int rxa_blt_scn_cnt;
    struct notifier_block power_supply_nb;
    struct work_struct signal_poll_work;
    unsigned int poll_status;
    unsigned int signal_poll_status;
    struct os_spinlock   *poll_lock;
}zx_card_t;

static inline struct drm_device *_to_drm_device(void *pdev)
{
    struct drm_device *drm = NULL;

    drm = pci_get_drvdata(pdev);

    return drm;
}

struct zx_resident_node
{
    int resident_cnt;
    struct drm_zx_gem_object *obj;
    struct list_head link;
};

struct zx_resident_list_cache
{
    unsigned int    in_use;
    unsigned int    dirty_count;
    unsigned int    resident_caps;
    unsigned int    resident_count;
    void            *resident_list;    // this will pass to core as resident_list
    struct drm_zx_gem_object **gem_list;
};

struct zx_file
{
    struct drm_file   *parent_file;
    zx_card_t        *card;

    unsigned int      gpu_device_handle;
    void              *gpu_device;

    zx_device_debug_info_t     *debug;

    int               hold_lock;
    int               freezable;

    struct idr syncobj_idr;
    spinlock_t syncobj_table_lock;

    struct idr ctx_idr;
    spinlock_t ctx_table_lock;

    struct os_mutex *resident_lock;
    struct list_head resident_list;
    unsigned int resident_cnt;
    unsigned int resident_dirty_count;
    struct zx_resident_list_cache resident_list_cache[4];
};

extern char *zx_fb_mode;
extern int   zx_fb;

extern struct class *zx_class;
extern struct drm_ioctl_desc zx_ioctls[];
extern struct drm_ioctl_desc zx_ioctls_compat[];

void  zx_init_signal(zx_card_t* zx);
void  zx_deinit_signal(zx_card_t* zx);
extern int  zx_card_init(zx_card_t *zx, void *pdev);
extern int  zx_card_deinit(zx_card_t *zx);
extern int  zx_init_modeset(struct drm_device *dev);
extern void  zx_deinit_modeset(struct drm_device *dev);
extern int zx_debugfs_crtc_dump(struct seq_file* file, void *data);
extern int zx_debugfs_clock_dump(struct seq_file* file, struct drm_device* dev);
extern int zx_debugfs_displayinfo_dump(struct seq_file* file, struct drm_device* dev);
extern int zx_debugfs_psr_dump(struct seq_file* file, struct drm_device* dev);
extern void zx_handle_hpd_from_debugfs(void *data, int int_bit);
extern int zx_debugfs_connector_dump(struct seq_file* file, void *data);
extern int zx_power_supply_notify(struct notifier_block *nb,unsigned long event,void *data);
extern int zx_debugfs_psr_onoff(struct drm_device* dev, int enable);

extern void zx_interrupt_init(zx_card_t *zx);
extern void zx_interrupt_reinit(zx_card_t *zx);
extern void zx_interrupt_deinit(zx_card_t *zx);

extern int  zx_map_system_io(struct vm_area_struct *vma, zx_map_argu_t *map);
extern int  zx_map_system_ram(struct vm_area_struct *vma, zx_map_argu_t *map);

extern void zx_enable_interrupt(void *pdev);
extern void zx_disable_interrupt(void *pdev);

extern void zx_init_bus_id(zx_card_t *zx);
extern int  zx_register_driver(void);
extern void zx_unregister_driver(void);
extern int  zx_register_interrupt(zx_card_t *zx, void *isr);
extern void zx_unregister_interrupt(zx_card_t *zx);

extern int  zx_create_device(int card, unsigned int *device);
extern void zx_destroy_device(int card, unsigned int device);

extern int  zx_sync_init(zx_card_t *zx);
extern void zx_sync_deinit(zx_card_t *zx);

extern int zx_ioctl_get_busid(struct drm_device *dev, void *data,struct drm_file *filp);
extern int zx_ioctl_wait_chip_idle(struct drm_device *dev, void *data,struct drm_file *filp);

extern int zx_ioctl_gem_create_allocation(struct drm_device *dev, void *data,struct drm_file *filp);
extern int zx_ioctl_create_context(struct drm_device *dev, void *data,struct drm_file *filp);
extern int zx_ioctl_gem_map_gtt(struct drm_device *dev, void *data,struct drm_file *filp);
extern int zx_ioctl_gem_begin_cpu_access(struct drm_device *dev, void *data,struct drm_file *filp);
extern int zx_ioctl_gem_end_cpu_access(struct drm_device *dev, void *data,struct drm_file *filp);
extern int zx_ioctl_render(struct drm_device *dev, void *data,struct drm_file *filp);
extern int zx_ioctl_add_hw_ctx_buf(struct drm_device *dev, void *data,struct drm_file *filp);
extern int zx_ioctl_rm_hw_ctx_buf(struct drm_device *dev, void *data,struct drm_file *filp);

extern int zx_ioctl_query_info(struct drm_device *dev, void *data,struct drm_file *filp);
extern int zx_ioctl_create_device(struct drm_device *dev, void *data,struct drm_file *filp);
extern int zx_ioctl_destroy_device(struct drm_device *dev, void *data,struct drm_file *filp);
extern int zx_ioctl_begin_perf_event(struct drm_device *dev, void *data,struct drm_file *filp);
extern int zx_ioctl_begin_miu_dump_perf_event(struct drm_device *dev, void *data,struct drm_file *filp);
extern int zx_ioctl_end_perf_event(struct drm_device *dev, void *data,struct drm_file *filp);
extern int zx_ioctl_end_miu_dump_perf_event(struct drm_device *dev, void *data,struct drm_file *filp);
extern int zx_ioctl_send_perf_event(struct drm_device *dev, void *data,struct drm_file *filp);
extern int zx_ioctl_get_perf_status(struct drm_device *dev, void *data,struct drm_file *filp);
extern int zx_ioctl_query_chip_id(struct drm_device *dev, void *data,struct drm_file *filp);
extern int zx_ioctl_create_sync_object(struct drm_device *dev, void *data,struct drm_file *filp);
extern int zx_ioctl_destroy_sync_object(struct drm_device *dev, void *data,struct drm_file *filp);
extern int zx_ioctl_wait_sync_object(struct drm_device *dev, void *data,struct drm_file *filp);
extern int zx_ioctl_signal_sync_object(struct drm_device *dev, void *data,struct drm_file *filp);
extern int zx_ioctl_fence_value(struct drm_device *dev, void *data,struct drm_file *filp);
extern int zx_ioctl_create_fence_fd(struct drm_device *dev, void *data,struct drm_file *filp);
extern int zx_mmap(struct file* filp, struct vm_area_struct *vma);

extern int zx_ioctl_set_allocation_priority(struct drm_device *dev, void *data,struct drm_file *filp);
extern int zx_ioctl_make_resident(struct drm_device *dev, void *data,struct drm_file *filp);
extern int zx_ioctl_evict(struct drm_device *dev, void *data,struct drm_file *filp);
extern int zx_ioctl_map_gpu_virtual_address(struct drm_device *dev, void *data,struct drm_file *filp);
extern int zx_ioctl_free_gpu_virtual_address(struct drm_device *dev, void *data,struct drm_file *filp);
extern int zx_ioctl_reserve_gpu_virtual_address(struct drm_device *dev, void *data,struct drm_file *filp);
extern int zx_ioctl_update_gpu_virtual_address(struct drm_device *dev, void *data,struct drm_file *filp);

extern int zx_drm_syncobj_create_ioctl(struct drm_device *dev, void *data,struct drm_file *filp);
extern int zx_drm_syncobj_destroy_ioctl(struct drm_device *dev, void *data,struct drm_file *filp);
extern int zx_drm_syncobj_wait_ioctl(struct drm_device *dev, void *data,struct drm_file *filp);
extern int zx_drm_syncobj_reset_ioctl(struct drm_device *dev, void *data,struct drm_file *filp);
extern int zx_drm_syncobj_get_status_ioctl(struct drm_device *dev, void *data,struct drm_file *filp);

extern unsigned int zx_get_chip_temp(void *dispi);

#endif

