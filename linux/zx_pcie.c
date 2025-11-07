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
#include "zx.h"
#include "zx_driver.h"
#include "os_interface.h"
#include "zx_ioctl.h"
#include "zx_debugfs.h"
#include "zx_version.h"
#include "zx_gem.h"
#include "zx_gem_priv.h"
#include "zx_fence.h"
#include "zx_irq.h"
#include "zx_fbdev.h"
#include "zx_params.h"

#if DRM_VERSION_CODE >= KERNEL_VERSION(5,5,0)
#if DRM_VERSION_CODE < KERNEL_VERSION(5,8,0)
#include <drm/drm_pci.h>
#else
#if DRM_VERSION_CODE < KERNEL_VERSION(6,8,0)
#include <drm/drm_legacy.h>
#endif
#endif
#endif

#if DRM_VERSION_CODE >= KERNEL_VERSION(6, 2, 0)
#if DRM_VERSION_CODE < KERNEL_VERSION(6, 11, 0)
#include <drm/drm_fbdev_generic.h>
#else
#if DRM_VERSION_CODE >= KERNEL_VERSION(6, 13, 0)
#if DRM_VERSION_CODE < KERNEL_VERSION(6, 14, 0)
#include <drm/drm_client_setup.h>
#else
#include <drm/clients/drm_client_setup.h>
#endif
#endif
#include <drm/drm_fbdev_ttm.h>
#endif
#endif

#if DRM_VERSION_CODE >= KERNEL_VERSION(5, 14, 0)
#if DRM_VERSION_CODE < KERNEL_VERSION(6, 13, 0)
#include <drm/drm_aperture.h>
#else
#include <linux/aperture.h>
#endif
#endif

#define DRIVER_DESC         "ZX DRM Pro"

#define __STR(x)    #x
#define STR(x)      __STR(x)

struct zx_device_info {
    u32 display_mmio_offset;
    u32 intrupte_enable_regs;
};

static struct drm_zx_driver zx_drm_driver;

/*
 * A pci_device_id struct {
 *    __u32 vendor, device;
 *    __u32 subvendor, subdevice;
 *    __u32 class, class_mask;
 *    kernel_ulong_t driver_data;
 * };
 * Don't use C99 here because "class" is reserved and we want to
 * give userspace flexibility.
 */
static const struct zx_device_info zx_e3k_info = {
    0,
    0,
};


static struct pci_device_id pciidlist[] =
{
    {0x1d17, 0x3D01, PCI_ANY_ID, PCI_ANY_ID, 0, 0, (kernel_ulong_t)&zx_e3k_info}, //chx004
    {0x1d17, 0x3A05, PCI_ANY_ID, PCI_ANY_ID, 0, 0, (kernel_ulong_t)&zx_e3k_info}, //cne001
    {0, 0, 0}
};

MODULE_DEVICE_TABLE(pci, pciidlist);

static struct pci_driver zx_driver;

static int zx_drm_suspend(struct drm_device *dev, pm_message_t state)
{
    zx_card_t* zx = dev->dev_private;
    int ret;

    zx_info("zx driver suspending, pm event = %d.\n", state.event);
    disp_suspend(dev);
    zx_info("drm suspend: save display status finished.\n");

#if DRM_VERSION_CODE < KERNEL_VERSION(4, 19, 0)
    zx_fbdev_set_suspend(zx, 1);
#else
    drm_fb_helper_set_suspend_unlocked(dev->fb_helper, true);
#endif

    zx_info("drm suspend: save drmfb status finished.\n");

    zx_core_interface->save_state(zx->adapter, state.event == PM_EVENT_FREEZE);

    /* disable IRQ */
    zx_disable_interrupt(zx->pdev);
    tasklet_disable(&zx->fine_tuning_power_tuning);
    tasklet_disable(&zx->fence_notify);
    synchronize_irq(to_pci_dev(dev->dev)->irq);
    zx_info("drm suspend: disable irq finished.\n");

#if DRM_VERSION_CODE < KERNEL_VERSION(4, 8, 0)
    disp_vblank_save(dev);
#endif

    pci_save_state(to_pci_dev(dev->dev));

    if (state.event == PM_EVENT_SUSPEND)
    {
        /*patch for CHX001 A0 and B0, pci_disable_device will clear master bit PCI04[2], and this bit can't be cleared for chx001 hw, so don't call pci_disable_device here*/
        pci_disable_device(to_pci_dev(dev->dev));
        pci_set_power_state(to_pci_dev(dev->dev), PCI_D3hot);
    }

    return 0;
}

#if ENABLE_RUNTIME_PM
static int zx_drm_runtime_suspend(struct drm_device *dev, pm_message_t state)
{
    zx_card_t* zx = dev->dev_private;
    int ret;

    zx_info("zx driver runtime suspend: pm event = %d.\n", state.event);
    disp_runtime_suspend(dev);
    zx_info("runtime suspend: save display status finished.\n");

    /*TODO disable interrupt*/

    zx_info("runtime suspend finsihed \n");

    return 0;
}
#endif

static int zx_drm_resume(struct drm_device *dev)
{
    zx_card_t* zx = dev->dev_private;
    int         ret = 0;

    zx_info("zx driver resume back.\n");

    pci_set_power_state(to_pci_dev(dev->dev), PCI_D0);

    pci_restore_state(to_pci_dev(dev->dev));

    if (pci_enable_device(to_pci_dev(dev->dev)))
    {
        return -1;
    }

    pci_set_master(to_pci_dev(dev->dev));

    disp_pre_resume(dev);
    zx_info("drm resume: enable and post chip finished.\n");

#if DRM_VERSION_CODE < KERNEL_VERSION(4, 8, 0)
    disp_vblank_restore(dev);
#endif

    tasklet_enable(&zx->fence_notify);
    tasklet_enable(&zx->fine_tuning_power_tuning);
    zx_enable_interrupt(zx->pdev);
    zx_info("drm resume: re-enable irq finished.\n");

    ret = zx_core_interface->restore_state(zx->adapter);

    if (ret != 0)
    {
        return -1;
    }

    zx_power_supply_notify(&zx->power_supply_nb,0,NULL);

#if DRM_VERSION_CODE < KERNEL_VERSION(4, 19, 0)
    zx_fbdev_set_suspend(zx, 0);
#else
    drm_fb_helper_set_suspend_unlocked(dev->fb_helper, false);
#endif
    zx_info("drm resume: restore drmfb status finished.\n");

    disp_post_resume(dev);
    zx_info("drm resume: restore display status finished.\n");

    return 0;
}

#if ENABLE_RUNTIME_PM
static int zx_drm_runtime_resume(struct drm_device *dev)
{
    zx_card_t* zx = dev->dev_private;
    int ret = 0;

    zx_info("zx driver runtime resume back.\n");
    disp_pre_resume(dev);
    zx_info("runtime resume: enable and post chip finished.\n");

    /*TODO enable interrupt */

    disp_post_runtime_resume(dev);
    zx_info("runtime resume: restore display status finished.\n");

    return 0;

}
#endif

static __inline__ void zx_init_adapter_info_by_params(struct krnl_adapter_init_info_s *info, struct zx_params *p)
{
    info->zx_pwm_mode                    = p->zx_pwm_mode;
    info->zx_dvfs_mode                  = p->zx_dfs_mode;
    info->zx_worker_thread_enable       = p->zx_worker_thread_enable;
    info->zx_recovery_enable            = p->zx_recovery_enable;
    info->zx_hang_dump                  = p->zx_hang_dump;
    info->zx_run_on_qt                  = p->zx_run_on_qt;
    info->zx_flag_buffer_verify         = p->zx_flag_buffer_verify;
    info->zx_vesa_tempbuffer_enable     = p->zx_vesa_tempbuffer_enable;

    info->miu_channel_size              = p->miu_channel_size;
    info->chip_slice_mask               = p->chip_slice_mask;
    info->zx_local_size_g               = p->zx_local_size_g;
    info->zx_local_size_m               = p->zx_local_size_m;
    info->debugfs_mask                  = p->debugfs_mask;
    info->nonsnoop_patch                = p->nonsnoop_patch;
    info->page_table_seg                = p->page_table_seg;
    info->zx_set_high_eclk              = p->zx_set_high_eclk;
}

static void zx_drm_irq_install(struct drm_device *drm_dev)
{
#if DRM_VERSION_CODE < KERNEL_VERSION(5, 15, 0)
    drm_irq_install(drm_dev, to_pci_dev(drm_dev->dev)->irq);
#else
    zx_irq_install(drm_dev);
#endif
}

static void zx_drm_irq_uninstall(struct drm_device *drm_dev)
{
#if DRM_VERSION_CODE < KERNEL_VERSION(5, 15, 0)
    drm_irq_uninstall(drm_dev);
#else
    zx_irq_uninstall(drm_dev);
#endif
}

#if  DRM_VERSION_CODE < KERNEL_VERSION(5, 14, 0)
static int zx_kick_out_firmware_fb(struct pci_dev *pdev)
{
    struct apertures_struct *ap;
    bool primary = false;

    ap = alloc_apertures(1);
    if (!ap)
        return -ENOMEM;

    ap->ranges[0].base = pci_resource_start(pdev, 1);
    ap->ranges[0].size = pci_resource_len(pdev, 1);

#ifdef CONFIG_X86
    primary = pdev->resource[PCI_ROM_RESOURCE].flags & IORESOURCE_ROM_SHADOW;
#endif

#if  DRM_VERSION_CODE >= KERNEL_VERSION(4, 9, 0)
    drm_fb_helper_remove_conflicting_framebuffers(ap, "cx4drmfb", primary);
#else
    remove_conflicting_framebuffers(ap, "cx4drmfb", primary);
#endif
    zx_free(ap);

    return 0;
}
#endif

#define PCI_EN_IO_SPACE     1
static int zx_drm_load_kms(struct drm_device *dev, unsigned long flags)
{
    struct pci_dev *pdev = to_pci_dev(dev->dev);
    struct device* device = &pdev->dev;
    zx_card_t  *zx = NULL;
    int         ret = 0;
    unsigned short command = 0;

    zx = (zx_card_t *)zx_calloc(sizeof(zx_card_t));
    if (!zx) {
        zx_error("allocate failed for zx card!\n");
        return -ENOMEM;
    }

    dev->dev_private = (void*)zx;

    zx->drm_dev = dev;
    //TODO..FIXME. need remove zx->index.
    zx->index  = dev->primary->index;

    pci_set_drvdata(pdev, dev);

    dma_set_mask_and_coherent(device, DMA_BIT_MASK(36));

    zx->pdev = pdev;

#if  DRM_VERSION_CODE >= KERNEL_VERSION(5, 15, 0)
#if  DRM_VERSION_CODE < KERNEL_VERSION(6, 13, 0)
    ret = drm_aperture_remove_conflicting_pci_framebuffers(pdev, (const struct drm_driver *)&zx_drm_driver.base);
#else
    ret = aperture_remove_conflicting_pci_devices(pdev, zx_drm_driver.base.name);
#endif
#elif DRM_VERSION_CODE >= KERNEL_VERSION(5, 14, 0)
    ret = drm_aperture_remove_conflicting_pci_framebuffers(pdev, "cx4drmfb");
#else
    ret = zx_kick_out_firmware_fb(pdev);
#endif

    if (ret)
    {
        zx_error("remove conflicting framebuffer failed. ret:%x.\n", ret);
    }

    pci_set_master(pdev);

   //fpga only
#if 0
    zx_get_command_status16(pdev,&command);
    zx_info("command:%x\n",command);
    if(!(command & PCI_EN_IO_SPACE))
    {
        zx_write_command_status16(pdev,(command|PCI_EN_IO_SPACE));
        zx_info("set primary command:%x\n",zx_get_command_status16(pdev,&command));
    }
#endif

    /*don't use the vga arbiter*/
#if defined(CONFIG_VGA_ARB)
    vga_set_legacy_decoding(pdev, VGA_RSRC_NONE);
#endif

    zx_init_adapter_info_by_params(&zx->a_info, &zx_modparams);
    zx->a_info.minor_index = zx->index;
    ret = zx_card_init(zx, pdev);
    if(ret)
    {
        zx_error("%s_card_init() failed. ret:0x%x\n", STR(DRIVER_NAME), ret);
    }

    zx->fence_drv = zx_calloc(sizeof(struct zx_dma_fence_driver));

    zx->fence_drv->zx_card = zx;

    zx_dma_fence_driver_init(zx->adapter, zx->fence_drv);

    zx_info("%s = %p, %s->pdev = %p, dev = %p, dev->primary = %p\n", STR(DRIVER_NAME), zx, STR(DRIVER_NAME), zx->pdev,  dev, dev ? dev->primary : NULL);

    zx_drm_irq_install(dev);

#if DRM_VERSION_CODE < KERNEL_VERSION(4, 19, 0)
    /* Add debugfs init here for fbdev will create gem object which use debugfs  to trace */
    zx_debugfs_init(zx);
    zx_fbdev_init(zx);
#endif

#if ENABLE_RUNTIME_PM
    dev_pm_set_driver_flags(dev->dev, DPM_FLAG_NEVER_SKIP);
    pm_runtime_use_autosuspend(dev->dev);
    pm_runtime_set_autosuspend_delay(dev->dev, 5000); //5s
    pm_runtime_set_active(dev->dev);
    pm_runtime_allow(dev->dev);             //-1
    pm_runtime_mark_last_busy(dev->dev);
    pm_runtime_put_autosuspend(dev->dev);   //-1
#endif

    return ret;
}

static int zx_drm_open(struct drm_device *dev, struct drm_file *file)
{
    zx_card_t *zx = dev->dev_private;
    zx_file_t *priv;
    void *gpu_dev = NULL;
    int r = 0;

#if ENABLE_RUNTIME_PM
    r = pm_runtime_get_sync(dev->dev);
    if (r < 0)
        goto pm_put;
#endif

    if (!zx->adapter)
    {
        r = -ENODEV;
        goto out_suspend;
    }

    priv = zx_calloc(sizeof(zx_file_t));
    if (!priv)
    {
        r = -ENOMEM;
        goto out_suspend;
    }

    file->driver_priv = priv;
    priv->parent_file = file;
    priv->card  = zx;

    priv->resident_lock  = zx_create_mutex();
    INIT_LIST_HEAD(&priv->resident_list);

    idr_init(&priv->syncobj_idr);
    spin_lock_init(&priv->syncobj_table_lock);

    idr_init(&priv->ctx_idr);
    spin_lock_init(&priv->ctx_table_lock);

    priv->gpu_device = zx_core_interface->create_device(zx->adapter, file);
    zx_assert(priv->gpu_device, "");

    spin_lock(&zx->dev_table_lock);
    priv->gpu_device_handle = idr_alloc_cyclic(&zx->dev_idr, priv->gpu_device, 0x2000000, 0, GFP_NOWAIT);
    spin_unlock(&zx->dev_table_lock);

    if(zx->debugfs_dev)
    {
        priv->debug = zx_debugfs_add_device_node(zx->debugfs_dev, zx_get_current_pid(), priv->gpu_device_handle);
    }
out_suspend:
#if ENABLE_RUNTIME_PM
    pm_runtime_mark_last_busy(dev->dev);
#endif

#if ENABLE_RUNTIME_PM
pm_put:
    pm_runtime_put_autosuspend(dev->dev);
#endif
    return r;
}

extern int zx_drm_syncobj_release_handle(int fd, void *ptr, void *data);
static void zx_drm_postclose(struct drm_device *dev, struct drm_file *file)
{
    zx_file_t *priv = file->driver_priv;
    zx_card_t *zx  = priv->card;

#if ENABLE_RUNTIME_PM
    int ret;
    ret = pm_runtime_get_sync(dev->dev);
    if (ret< 0) {
        pm_runtime_mark_last_busy(dev->dev);
        pm_runtime_put_autosuspend(dev->dev);
        return;
    }
#endif
    if(priv->hold_lock)
    {
        zx_mutex_unlock(zx->lock);
    }

    if(priv->gpu_device)
    {
        if(zx->debugfs_dev)
        {
            zx_debugfs_remove_device_node(zx->debugfs_dev, priv->debug);
            priv->debug = NULL;
        }

        spin_lock(&zx->dev_table_lock);
        if (priv->gpu_device_handle)
            idr_remove(&zx->dev_idr, priv->gpu_device_handle);
        priv->gpu_device_handle = 0;
        spin_unlock(&zx->dev_table_lock);

        zx_core_interface->destroy_device(zx->adapter, priv->gpu_device);
        priv->gpu_device = NULL;
    }

    zx_gem_cleanup_resident(file);
    zx_destroy_mutex(priv->resident_lock);

    idr_for_each(&priv->syncobj_idr, &zx_drm_syncobj_release_handle, priv);
    idr_destroy(&priv->syncobj_idr);

    // context already destroy in cm_destroy_device
    idr_destroy(&priv->ctx_idr);

    zx_free(priv);

    file->driver_priv = NULL;
#if ENABLE_RUNTIME_PM
    pm_runtime_mark_last_busy(dev->dev);
    pm_runtime_put_autosuspend(dev->dev);
#endif
}

void  zx_drm_last_close(struct drm_device* dev)
{
#if DRM_VERSION_CODE < KERNEL_VERSION(4, 19, 0)
    zx_card_t *zx = dev->dev_private;
    zx_fbdev_restore_mode(zx);
#else
    drm_fb_helper_lastclose(dev);
#endif
}

#if DRM_VERSION_CODE < KERNEL_VERSION(4,11,0)
static int zx_drm_device_is_agp(struct drm_device * dev)
{
    return 0;
}
#endif

static long zx_unlocked_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
    long ret;
    struct drm_file *file_priv = filp->private_data;
    struct drm_device *dev = file_priv->minor->dev;

#if ENABLE_RUNTIME_PM
    ret = pm_runtime_get_sync(dev->dev);
    if (ret < 0) {
        pm_runtime_put_autosuspend(dev->dev);
        return ret;
    }
#endif

    ret = drm_ioctl(filp, cmd, arg);

#if ENABLE_RUNTIME_PM
    pm_runtime_mark_last_busy(dev->dev);
    pm_runtime_put_autosuspend(dev->dev);
#endif
   return ret;
}

#if (defined(__x86_64__) || defined(__mips64__)) && defined(CONFIG_COMPAT)
static long zx_compat_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
    long ret;
    unsigned int nr = ZX_IOCTL_NR(cmd);

    if (nr < DRM_COMMAND_BASE)
    {
        ret = drm_compat_ioctl(filp, cmd, arg);
    }
    else
    {
        ret = drm_ioctl(filp, cmd, arg);
    }

    return ret;
}
#endif

static void zx_drm_unload_kms(struct drm_device *dev)
{
    zx_card_t *zx = dev->dev_private;
    zx_info("drm unload.\n");

    if(zx == NULL)
        return;

    zx_info("zx_drm_irq_uninstall.\n");
    zx_drm_irq_uninstall(dev);

    zx_dma_fence_driver_fini(zx->fence_drv);
    zx_card_deinit(zx);

    if(zx->fence_drv){
        zx_free(zx->fence_drv);
        zx->fence_drv = NULL;
    }

    zx_free(zx);
    dev->dev_private = NULL;
}

static struct file_operations zx_drm_fops = {
    .owner      = THIS_MODULE,
    .open       = drm_open,
    .release    = drm_release,
    .unlocked_ioctl = zx_unlocked_ioctl,
#if defined(__x86_64__) && defined(CONFIG_COMPAT)
    .compat_ioctl = zx_compat_ioctl,
#endif
    .mmap       = zx_drm_gem_mmap,
    .read       = drm_read,
    .poll       = drm_poll,
    .llseek     = noop_llseek,
#if DRM_VERSION_CODE >= KERNEL_VERSION(6, 12, 0)
    .fop_flags  = FOP_UNSIGNED_OFFSET,
#endif
};

#if DRM_VERSION_CODE >= KERNEL_VERSION(5, 4, 0)
#define  ZX_DRM_FEATURE \
    ( DRIVER_MODESET | DRIVER_RENDER | DRIVER_GEM)
#elif DRM_VERSION_CODE >= KERNEL_VERSION(5, 1, 0)
#define  ZX_DRM_FEATURE \
    ( DRIVER_MODESET | DRIVER_RENDER | DRIVER_PRIME | DRIVER_GEM)
#else
#define  ZX_DRM_FEATURE \
    (DRIVER_HAVE_IRQ | DRIVER_IRQ_SHARED | DRIVER_MODESET | DRIVER_RENDER | DRIVER_PRIME | DRIVER_GEM)
#endif

static struct drm_zx_driver zx_drm_driver = {
    .file_priv = NULL,
//    .lock = __MUTEX_INITIALIZER(zx_drm_driver_lock),
    .base = {
        .driver_features    = ZX_DRM_FEATURE,
    #if DRM_VERSION_CODE >= KERNEL_VERSION(4, 8, 0)
        .driver_features = ZX_DRM_FEATURE | DRIVER_ATOMIC,
    #endif
    #if DRM_VERSION_CODE >= KERNEL_VERSION(4, 13, 0)
        .driver_features = ZX_DRM_FEATURE | DRIVER_ATOMIC | DRIVER_SYNCOBJ,
    #endif

    #if DRM_VERSION_CODE < KERNEL_VERSION(4, 19, 0)
        .load               = zx_drm_load_kms,
    #endif
        .open               = zx_drm_open,

    #if DRM_VERSION_CODE >= KERNEL_VERSION(4, 14, 0)
        .unload             = zx_drm_unload_kms,
    #endif

        .postclose          = zx_drm_postclose,
    #if DRM_VERSION_CODE < KERNEL_VERSION(6,12,0)
        .lastclose           = zx_drm_last_close,
    #endif
        .ioctls             = zx_ioctls,
        .num_ioctls         = ioctl_nr_total_num,
    #if DRM_VERSION_CODE < KERNEL_VERSION(4,14,0) && DRM_VERSION_CODE >= KERNEL_VERSION(3,18,0)
        .set_busid          = drm_pci_set_busid,
    #endif

    #if DRM_VERSION_CODE < KERNEL_VERSION(4,0,0)
    #ifndef __ARM_ARCH
        .suspend            = zx_drm_suspend,
        .resume             = zx_drm_resume,
    #endif
    #endif
    #if DRM_VERSION_CODE < KERNEL_VERSION(4,11,0)
        .device_is_agp      = zx_drm_device_is_agp,
    #endif
        .fops               = &zx_drm_fops,

    #if DRM_VERSION_CODE < KERNEL_VERSION(5,11,0)
        .gem_vm_ops         = &zx_gem_vm_ops,
    #endif
    #if DRM_VERSION_CODE >=  KERNEL_VERSION(6, 13, 0)
        DRM_FBDEV_TTM_DRIVER_OPS,
    #endif

    #if DRM_VERSION_CODE <  KERNEL_VERSION(5, 11, 0)
        .gem_prime_vmap = zx_gem_prime_vmap,
        .gem_prime_vunmap = zx_gem_prime_vunmap,
    #endif

    #if DRM_VERSION_CODE < KERNEL_VERSION(5,9,0)
        .gem_free_object    = zx_gem_free_object,
    #else
    #if DRM_VERSION_CODE < KERNEL_VERSION(5,11,0)
        .gem_free_object_unlocked = zx_gem_free_object,
    #endif
    #endif
    #if DRM_VERSION_CODE < KERNEL_VERSION(6,6,0)
        .prime_handle_to_fd = drm_gem_prime_handle_to_fd,
        .prime_fd_to_handle = zx_gem_prime_fd_to_handle,
    #endif

    #if DRM_VERSION_CODE < KERNEL_VERSION(5,11,0)
        .gem_open_object    = zx_gem_open,
        .gem_prime_export   = zx_gem_prime_export,
    #endif
        .gem_prime_import   = zx_gem_prime_import,

    #if DRM_VERSION_CODE < KERNEL_VERSION(5,15,0)
        .irq_preinstall  = zx_irq_preinstall,
        .irq_postinstall = zx_irq_postinstall,
        .irq_handler     = zx_irq_handle,
        .irq_uninstall   = zx_irq_uninstall,
    #endif

        .dumb_create        = zx_gem_dumb_create,
        .dumb_map_offset    = zx_gem_dumb_map_offset,
    #if DRM_VERSION_CODE < KERNEL_VERSION(5, 12, 0)
        .dumb_destroy       = drm_gem_dumb_destroy,
    #endif
        .name               = STR(DRIVER_NAME),
        .desc               = DRIVER_DESC,
    #if DRM_VERSION_CODE < KERNEL_VERSION(6, 14, 0)
        .date               = DRIVER_DATE,
    #endif
        .major              = DRIVER_MAJOR,
        .minor              = DRIVER_MINOR,
        .patchlevel         = DRIVER_PATCHLEVEL,
    }
};

static int zx_pcie_probe(struct pci_dev *pdev, const struct pci_device_id *ent)
{
#if DRM_VERSION_CODE >= KERNEL_VERSION(4, 19, 0)
    struct drm_device *dev;
    unsigned long flags = ent->driver_data;
    zx_card_t *zx = NULL;
    int ret;

    dev = drm_dev_alloc(&zx_drm_driver.base, &pdev->dev);
    if (IS_ERR(dev))
            return PTR_ERR(dev);

    ret = pci_enable_device(pdev);
    if (ret)
            goto err_free;

#if DRM_VERSION_CODE < KERNEL_VERSION(5,14,0)
    dev->pdev = pdev;
#endif

    ret = zx_drm_load_kms(dev, flags);
    if (ret)
    {
        goto err_pci;
    }

    ret = drm_dev_register(dev, flags);
    if (ret)
    {
        goto err_pci;
    }

    zx = (zx_card_t *)dev->dev_private;

    zx_debugfs_init(zx);

#if DRM_VERSION_CODE < KERNEL_VERSION(6, 11, 0)
    drm_fbdev_generic_setup(dev, 32);
#elif DRM_VERSION_CODE < KERNEL_VERSION(6, 13, 0)
    drm_fbdev_ttm_setup(dev, 32);
#else
    drm_client_setup(dev, NULL);
#endif

#if DRM_VERSION_CODE < KERNEL_VERSION(5, 2, 0)
    //skip_vt_switch is not set in drm_fb_helper_alloc_fbi function under kernel version of 5.2.0, patch it here
    if (dev->fb_helper && dev->fb_helper->fbdev &&
        dev->fb_helper->fbdev->dev)
    {
        pm_vt_switch_required(dev->fb_helper->fbdev->dev, false);
    }
#endif
    return 0;

err_pci:
    pci_disable_device(pdev);
err_free:
    drm_dev_put(dev);
    return ret;

#elif DRM_VERSION_CODE > KERNEL_VERSION(3,10,52)
    return drm_get_pci_dev(pdev, ent, &zx_drm_driver.base);
#else
    return 0;
#endif
}

#if DRM_VERSION_CODE >= KERNEL_VERSION(3,10,52)
#if DRM_VERSION_CODE >= KERNEL_VERSION(6,11,0)
static void zx_pcie_cleanup(struct pci_dev *pdev)
#else
static void __exit zx_pcie_cleanup(struct pci_dev *pdev)
#endif
{
    struct drm_device *dev = pci_get_drvdata(pdev);

    zx_info("zx_pcie_cleanup.\n");

#if DRM_VERSION_CODE >= KERNEL_VERSION(4, 14, 0)
    drm_dev_unplug(dev);
#else
    zx_drm_unload_kms(dev);
#endif

    drm_put_dev(dev);

    pci_set_drvdata(pdev, NULL);

    pci_release_regions(pdev);
}
#endif

static int zx_pmops_suspend(struct device *dev)
{
    struct pci_dev *pdev = to_pci_dev(dev);

    zx_info("pci device(vendor:0x%X, device:0x%X) pm suspend\n", pdev->vendor, pdev->device);

#if DRM_VERSION_CODE >= KERNEL_VERSION(4,0,0)
    zx_drm_suspend(pci_get_drvdata(pdev), PMSG_SUSPEND);
#endif

    return 0;
}
static int zx_pmops_resume(struct device *dev)
{
    struct pci_dev *pdev = to_pci_dev(dev);

    zx_info("pci device(vendor:0x%X, device:0x%X) pm resume\n", pdev->vendor, pdev->device);

#if ENABLE_RUNTIME_PM
    /* GPU comes up enabled by the bios on resume */
    pm_runtime_disable(dev);
    pm_runtime_set_active(dev);
    pm_runtime_enable(dev);
#endif

#if DRM_VERSION_CODE >= KERNEL_VERSION(4,0,0)
    zx_drm_resume(pci_get_drvdata(pdev));
#endif

    return 0;
}

static int zx_pmops_freeze(struct device *dev)
{
    struct pci_dev *pdev = to_pci_dev(dev);

    zx_info("pci device(vendor:0x%X, device:0x%X) pm freeze\n", pdev->vendor, pdev->device);

#if DRM_VERSION_CODE >= KERNEL_VERSION(4,0,0)
    zx_drm_suspend(pci_get_drvdata(pdev), PMSG_FREEZE);
#endif

    return 0;
}

static int zx_pmops_thaw(struct device *dev)
{
    struct pci_dev *pdev = to_pci_dev(dev);
    struct drm_device *drm_dev = NULL;
    zx_card_t *zx_card = NULL;

    zx_info("pci device(vendor:0x%X, device:0x%X) pm thaw\n", pdev->vendor, pdev->device);

#if DRM_VERSION_CODE >= KERNEL_VERSION(4,0,0)
    drm_dev = (struct drm_device *)pci_get_drvdata(pdev);
    zx_card = (zx_card_t *)drm_dev->dev_private;
    zx_card->flags |= ZX_S4_RESUME;

    zx_drm_resume(pci_get_drvdata(pdev));
#endif

    return 0;
}

static int zx_pmops_poweroff(struct device *dev)
{
    struct pci_dev *pdev = to_pci_dev(dev);

    zx_info("pci device(vendor:0x%X, device:0x%X) pm poweroff\n", pdev->vendor, pdev->device);

#if DRM_VERSION_CODE >= KERNEL_VERSION(4,0,0)
    zx_drm_suspend(pci_get_drvdata(pdev), PMSG_SUSPEND);
#endif

    return 0;
}

static int zx_pmops_restore(struct device *dev)
{
    struct pci_dev *pdev = to_pci_dev(dev);

    zx_info("pci device(vendor:0x%X, device:0x%X) pm restore\n", pdev->vendor, pdev->device);

#if DRM_VERSION_CODE >= KERNEL_VERSION(4,0,0)
    zx_drm_resume(pci_get_drvdata(pdev));
#endif

    return 0;
}

static int zx_pmops_runtime_suspend(struct device *dev)
{
    struct pci_dev *pdev = to_pci_dev(dev);
    struct drm_device *drm_dev = pci_get_drvdata(pdev);

    zx_info("pci device(vendor:0x%X, device:0x%X) pm runtime_suspend\n", pdev->vendor, pdev->device);
#if ENABLE_RUNTIME_PM
    zx_drm_runtime_suspend(drm_dev,PMSG_SUSPEND);
#endif
    return 0;
}

static int zx_pmops_runtime_resume(struct device *dev)
{
    struct pci_dev *pdev = to_pci_dev(dev);
    struct drm_device *drm_dev = pci_get_drvdata(pdev);

    zx_info("pci device(vendor:0x%X, device:0x%X) pm runtime_resume\n", pdev->vendor, pdev->device);

#if ENABLE_RUNTIME_PM
    zx_drm_runtime_resume(drm_dev);
#endif
    return 0;
}
static int zx_pmops_runtime_idle(struct device *dev)
{
    struct pci_dev *pdev = to_pci_dev(dev);
    struct drm_device *drm_dev = pci_get_drvdata(pdev);
    struct drm_crtc *crtc;

    zx_info("pci device(vendor:0x%X, device:0x%X) pm runtime_idle\n", pdev->vendor, pdev->device);

#if ENABLE_RUNTIME_PM
    zx_drm_for_each_crtc(crtc, drm_dev)
    {
        if (crtc->enabled) {
            return -EBUSY;
        }
    }

    pm_runtime_mark_last_busy(dev);
    pm_runtime_autosuspend(dev);
#endif

    return 0;
}

static void zx_shutdown(struct pci_dev *pdev)
{
    struct drm_device *dev = pci_get_drvdata(pdev);
    zx_card_t *zx = dev->dev_private;

    zx_info("pci device(vendor:0x%X, device:0x%X) shutting down.\n", pdev->vendor, pdev->device);

    zx_core_interface->wait_chip_idle(zx->adapter);

#if DRM_VERSION_CODE >= KERNEL_VERSION(4, 8, 0)
    drm_atomic_helper_suspend(dev);
#else
    zx_disp_suspend_helper(dev);
#endif
    zx_info("pci device(vendor:0x%X, device:0x%X) already shut down.\n", pdev->vendor, pdev->device);
}

static const struct dev_pm_ops zx_pm_ops = {
    .suspend = zx_pmops_suspend,
    .resume = zx_pmops_resume,
    .freeze = zx_pmops_freeze,
    .thaw = zx_pmops_thaw,
    .poweroff = zx_pmops_poweroff,
    .restore = zx_pmops_restore,
    .runtime_suspend = zx_pmops_runtime_suspend,
    .runtime_resume = zx_pmops_runtime_resume,
    .runtime_idle = zx_pmops_runtime_idle,
};

static struct pci_driver zx_driver =
{
    .name = STR(DRIVER_NAME),
    .id_table = pciidlist,
    .probe = zx_pcie_probe,
#if DRM_VERSION_CODE < KERNEL_VERSION(3,10,52)
   //.remove   = __devexit_p(zx_pcie_cleanup),
   .remove   = NULL,
#else
    .remove   = __exit_p(zx_pcie_cleanup),
#endif
    .driver.pm = &zx_pm_ops,
    .shutdown  = zx_shutdown,
};


/*************************************** PCI functions ********************************************/

int zx_get_command_status16(void *dev, unsigned short *command)
{
    return pci_read_config_word((struct pci_dev*)dev, 0x4, command);
}

int zx_get_command_status32(void *dev, unsigned int *command)
{
    return pci_read_config_dword((struct pci_dev*)dev, 0x4, command);
}

int zx_write_command_status16(void *dev, unsigned short command)
{
    return pci_write_config_word((struct pci_dev*)dev, 0x4, command);
}

int zx_write_command_status32(void *dev, unsigned int command)
{
    return pci_write_config_dword((struct pci_dev*)dev, 0x4, command);
}


int zx_get_bar1(void *dev, unsigned int *bar1)
{
    return pci_read_config_dword((struct pci_dev*)dev, 0x14, bar1);
}

int zx_get_rom_save_addr(void *dev, unsigned int *romsave)
{
    return pci_read_config_dword((struct pci_dev*)dev, 0x30, romsave);
}

int zx_write_rom_save_addr(void *dev, unsigned int romsave)
{
    return pci_write_config_dword((struct pci_dev*)dev, 0x30, romsave);
}

void* zx_pci_map_rom(void *dev)
{
    size_t size;
#ifdef __aarch64__
    struct pci_dev  *pdev = (struct pci_dev*)dev;
    struct resource *res = &pdev->resource[PCI_ROM_RESOURCE];
    void            *virt_addr = NULL;

    if ((res == NULL) || (res->start == 0) || (res->start == ~0))
    {
        zx_info("invalid pci resource!\n");
        return NULL;
    }

    if (pci_enable_rom(pdev))
    {
        zx_info("enable pci rom failed!\n");
        return NULL;
    }

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,6,0)
    virt_addr = ioremap((phys_addr_t)res->start, 0x10000);
#else
    virt_addr = ioremap_nocache((phys_addr_t)res->start, 0x10000);
#endif
    return virt_addr;

#else
    return pci_map_rom((struct pci_dev*)dev, &size);
#endif
}

void zx_pci_unmap_rom(void *dev, void* rom)
{
#ifdef __aarch64__
    struct pci_dev  *pdev = (struct pci_dev*)dev;
    struct resource *res = &pdev->resource[PCI_ROM_RESOURCE];
    if(rom)
    {
        iounmap(rom);
    }
    if ((res != NULL) && (!(res->flags & IORESOURCE_ROM_ENABLE)))
    {
        pci_disable_rom(pdev);
    }
#else
    if(rom)
    {
        pci_unmap_rom((struct pci_dev*)dev, rom);
    }
#endif

}

unsigned long zx_get_rom_start_addr(void *dev)
{
    return pci_resource_start((struct pci_dev*)dev, 6);
}

int zx_get_platform_config(void *dev, const char* config_name, int *buffer, int length)
{
    return 0;
}

void zx_get_bus_config(void *dev, bus_config_t *bus)
{
    struct pci_dev *pdev = dev;

    pci_read_config_word(pdev, 0x2,  &bus->device_id);
    pci_read_config_word(pdev, 0x0,  &bus->vendor_id);
    pci_read_config_word(pdev, 0x4,  &bus->command);
    pci_read_config_word(pdev, 0x6,  &bus->status);
    pci_read_config_byte(pdev, 0x8,  &bus->revision_id);
    pci_read_config_byte(pdev, 0x9,  &bus->prog_if);

    pci_read_config_byte(pdev, 0xa,  &bus->sub_class);
    pci_read_config_byte(pdev, 0xb,  &bus->base_class);
    pci_read_config_byte(pdev, 0xc,  &bus->cache_line_size);
    pci_read_config_byte(pdev, 0xd,  &bus->latency_timer);
    pci_read_config_byte(pdev, 0xe,  &bus->header_type);
    pci_read_config_byte(pdev, 0xf,  &bus->bist);
    pci_read_config_word(pdev, 0x2c, &bus->sub_sys_vendor_id);
    pci_read_config_word(pdev, 0x2e, &bus->sub_sys_id);
    //pci_read_config_word(pdev, 0x52, &bus->link_status);
    pcie_capability_read_word(pdev,0x12,  &bus->link_status); //PCI_EXP_LNKST

    zx_info("bus_command: 0x%x.\n", bus->command);

    //pci_write_config_word(pdev, 0x4,  7);

    bus->reg_start_addr[0] = pci_resource_start(pdev, 0);

    bus->mem_start_addr[0] = pci_resource_start(pdev, 1);

    bus->reg_start_addr[2] = 0;
    bus->reg_start_addr[3] = 0;
    bus->reg_start_addr[4] = 0;

    bus->reg_end_addr[0]   = pci_resource_end(pdev, 0);
    bus->mem_end_addr[0]   = pci_resource_end(pdev, 1);

    bus->reg_end_addr[2]   = 0;
    bus->reg_end_addr[3]   = 0;
    bus->reg_end_addr[4]   = 0;

    zx_info("device id:%x\n",bus->device_id);
}

void zx_init_bus_id(zx_card_t *zx)
{
    struct pci_dev *pdev = zx->pdev;

    int pci_domain = 0;
    int pci_bus    = pdev->bus->number;
    int pci_slot   = PCI_SLOT(pdev->devfn);
    int pci_func   = PCI_FUNC(pdev->devfn);

    zx->len = snprintf(zx->busId, 40, "pci:%04x:%02x:%02x.%d", pci_domain, pci_bus, pci_slot, pci_func);
}

int zx_register_driver(void)
{
    int ret = 0;

    mutex_init(&zx_drm_driver.lock);

    ret = pci_register_driver(&zx_driver);//register driver first,  register drm device during pci probe
#if DRM_VERSION_CODE <= KERNEL_VERSION(3,10,52)
    ret = drm_pci_init(&zx_drm_driver.base, &zx_driver);
#endif

    return ret;
}

void zx_unregister_driver(void)
{
#if DRM_VERSION_CODE <= KERNEL_VERSION(3,10,52)
    drm_pci_exit(&zx_drm_driver.base, &zx_driver);
#endif

    pci_unregister_driver(&zx_driver);
}
