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

#include "zx_disp.h"
#include "zx_fbdev.h"
#include "zx_debugfs.h"
#include <drm/drm_fb_helper.h>
#if DRM_VERSION_CODE < KERNEL_VERSION(4, 11, 0)
#include <linux/async.h>
#endif
#include <linux/fb.h>
#include "zx_drm_helper.h"


#if DRM_VERSION_CODE < KERNEL_VERSION(4, 19, 0)

static int zx_drm_fb_set_par(struct fb_info *info)
{
    struct drm_fb_helper *helper = info->par;
    struct drm_device *dev = helper->dev;
    struct zx_fbdev *fbdev = to_zx_fbdev(helper);
    zx_card_t* zx = (zx_card_t*)dev->dev_private;
    disp_info_t* disp_info = (disp_info_t*)zx->disp_info;

    zx_info("To set par on drm fb.\n");

    return drm_fb_helper_set_par(info);
}

static struct fb_ops zxfb_ops = {
    .owner = THIS_MODULE,
#if DRM_VERSION_CODE >= KERNEL_VERSION(4, 8, 0)
#if DRM_VERSION_CODE >= KERNEL_VERSION(4, 9, 0)
    DRM_FB_HELPER_DEFAULT_OPS,
#endif
    .fb_set_par = zx_drm_fb_set_par,
    .fb_fillrect = drm_fb_helper_cfb_fillrect,
    .fb_copyarea = drm_fb_helper_cfb_copyarea,
    .fb_imageblit = drm_fb_helper_cfb_imageblit,
#else
    .fb_check_var = drm_fb_helper_check_var,
    .fb_set_par = drm_fb_helper_set_par,
    .fb_pan_display = drm_fb_helper_pan_display,
    .fb_blank = drm_fb_helper_blank,
    .fb_debug_enter = drm_fb_helper_debug_enter,
    .fb_debug_leave = drm_fb_helper_debug_leave,
    .fb_setcmap = drm_fb_helper_setcmap,

    .fb_fillrect = cfb_fillrect,
    .fb_copyarea = cfb_copyarea,
    .fb_imageblit = cfb_imageblit,
#endif
};

static bool zx_fb_initial_config(struct drm_fb_helper *fb_helper,
        struct drm_fb_helper_crtc **crtcs,
        struct drm_display_mode **modes,
        struct drm_fb_offset *offsets,
        bool *enabled, int width, int height)
{
    struct drm_device *dev = fb_helper->dev;
    zx_card_t *zx = dev->dev_private;


    return false;
}

#if DRM_VERSION_CODE < KERNEL_VERSION(4, 14, 0)
static void zx_crtc_fb_gamma_set(struct drm_crtc *crtc, u16 red, u16 green, u16 blue, int reqno)
{
    DRM_DEBUG_KMS("crtc=%d,red=%d,green=%d,blue=%d\n", to_zx_crtc(crtc)->pipe, red, green, blue);
}

static void zx_crtc_fb_gamma_get(struct drm_crtc *crtc, u16 *red, u16 *green, u16 *blue, int reqno)
{
    DRM_DEBUG_KMS("crtc=%d\n", to_zx_crtc(crtc)->pipe);
}
#endif

#define ALIGNED_2KBits(Value)       (((Value) + 0xFF) & ~0xFF)

static int zxfb_create(struct drm_fb_helper *helper, struct drm_fb_helper_surface_size *sizes)
{
    int ret = 0;
    struct fb_info *info;
    struct zx_fbdev *fbdev = to_zx_fbdev(helper);
    struct drm_zx_framebuffer *fb = fbdev->fb;
    struct drm_device *dev = helper->dev;
    zx_card_t *zx = dev->dev_private;
    disp_info_t *disp_info = (disp_info_t *)zx->disp_info;
    struct drm_mode_fb_cmd2 mode_cmd = {0, };
    struct drm_zx_gem_object *obj = NULL;
    zx_cpu_vm_area_t *vma = NULL;
    unsigned int pitch = 0;
    DRM_DEBUG_KMS("fb_width=%d,fb_height=%d,surface_width=%d,surface_height=%d,surface_bpp=%d,surface_depth=%d\n",
            sizes->fb_width, sizes->fb_height, sizes->surface_width, sizes->surface_height, sizes->surface_bpp, sizes->surface_depth);

    mutex_lock(&dev->struct_mutex);

    if (fb &&
        (sizes->fb_width > fb->base.width ||
         sizes->fb_height > fb->base.height))
    {
        zx_drm_framebuffer_put(&fb->base);

        fbdev->fb = NULL;
        fb = NULL;
    }

    if (!fb)
    {
        zx_create_allocation_info_t alloc_info = {0,};

        pitch = ALIGNED_2KBits((sizes->surface_width * sizes->surface_bpp  + 7)/8);
        alloc_info.alignment                    = 4096;
        alloc_info.size                         = (pitch * sizes->surface_height + alloc_info.alignment - 1) & ~(alloc_info.alignment - 1);
        alloc_info.flags.pool_type              = ZX_POOL_VIDEOMEMORY;
        alloc_info.flags.cache_type             = ZX_CACHE_UNCACHABLE;
        alloc_info.flags.unpagable              = 1;
        alloc_info.flags.share                  = 1;
        alloc_info.flags.cpu_visible            = 1;
        alloc_info.flags.continuous             = 1;
        alloc_info.flags.force_clear            = 1;

        obj = zx_drm_gem_create_object(zx, &alloc_info);
        zx_assert(obj != NULL, "");

        {
            zx_gem_dbg_info_t * debug = &obj->debug.dbginfo;

            debug->width = sizes->surface_width;
            debug->height = sizes->surface_height;
            debug->pitch = pitch;
            debug->bit_cnt = sizes->surface_bpp;
            if (sizes->surface_bpp == 32)
            {
                debug->hw_format = 89;
            }
            else if (sizes->surface_bpp == 16)
            {
                debug->hw_format = 153;
            }
        }

        mode_cmd.width = sizes->surface_width;
        mode_cmd.height = sizes->surface_height;
        mode_cmd.pitches[0] = pitch;
        mode_cmd.pixel_format = drm_mode_legacy_fb_format(sizes->surface_bpp, sizes->surface_depth);
        fb = __zx_framebuffer_create(dev, &mode_cmd, obj);
        fbdev->fb = fb;
    }
    else
    {
        sizes->fb_width = fb->base.width;
        sizes->fb_height = fb->base.height;
    }

#if DRM_VERSION_CODE < KERNEL_VERSION(4, 8, 0)
    info = framebuffer_alloc(0, &dev->pdev->dev);
    zx_assert(info != NULL, "");

    info->par = helper;
    info->flags = FBINFO_DEFAULT | FBINFO_CAN_FORCE_OUTPUT;
    info->fbops = &zxfb_ops;

    ret = fb_alloc_cmap(&info->cmap, 256, 0);
    zx_assert(ret == 0, "");

    info->apertures = alloc_apertures(1);
    zx_assert(info->apertures != NULL, "");

    info->apertures->ranges[0].base = dev->mode_config.fb_base;
    info->apertures->ranges[0].size = pitch * sizes->surface_height;
    helper->fbdev = info;
#else
    info = drm_fb_helper_alloc_fbi(helper);

    info->par = helper;
    info->flags = FBINFO_DEFAULT;
    info->fbops = &zxfb_ops;
#endif

    info->flags |= FBINFO_CAN_FORCE_OUTPUT;

    info->skip_vt_switch = true;
    fbdev->helper.fb = &fb->base;
    zx_vsprintf(info->fix.id, "drmfb");

    info->fix.smem_start = obj ? obj->info.cpu_phy_addr : 0;
    info->fix.smem_len = pitch * sizes->surface_height;

    // debug info
    {
        zx_info("smem_start:%llx, gpu_virt_addr:%llx.\n", info->fix.smem_start, fb->gpu_virt_addr);
        zx_drm_gem_dump(obj);
    }

//map fbdev fb  gem object
    vma = zx_gem_object_vmap(obj);
    info->screen_base = vma->virt_addr;
    info->screen_size = pitch * sizes->fb_height;

    info->skip_vt_switch = true;
#if DRM_VERSION_CODE < KERNEL_VERSION(4, 11, 0)
    drm_fb_helper_fill_fix(info, fb->base.pitches[0], fb->base.depth);
    drm_fb_helper_fill_var(info, &fbdev->helper, sizes->fb_width, sizes->fb_height);
#elif DRM_VERSION_CODE < KERNEL_VERSION(5, 2, 0)
    drm_fb_helper_fill_fix(info, fb->base.pitches[0], fb->base.format->depth);
    drm_fb_helper_fill_var(info, &fbdev->helper, sizes->fb_width, sizes->fb_height);
#else
    drm_fb_helper_fill_info(info, &fbdev->helper, sizes);
#endif

    DRM_DEBUG_KMS("screen_base=0x%p,screen_size=0x%lx,smem_start=0x%lx,smem_len=0x%x,xres=%d,yres=%d\n",
            info->screen_base, info->screen_size, info->fix.smem_start, info->fix.smem_len, info->var.xres, info->var.yres);

    mutex_unlock(&dev->struct_mutex);
    return 0;
}

static const struct drm_fb_helper_funcs zx_fb_helper_funcs = {
#if DRM_VERSION_CODE < KERNEL_VERSION(5, 2, 0)
    .initial_config     = zx_fb_initial_config,
#endif
#if DRM_VERSION_CODE < KERNEL_VERSION(4, 14, 0)
    .gamma_set          = zx_crtc_fb_gamma_set,
    .gamma_get          = zx_crtc_fb_gamma_get,
#endif
    .fb_probe           = zxfb_create,
#if DRM_VERSION_CODE >= KERNEL_VERSION(6, 2, 0)
    .fb_dirty           = zxfb_dirty,
#endif
};

int zx_fbdev_init(zx_card_t *zx)
{
    int ret;
    struct zx_fbdev *fbdev;

    DRM_DEBUG_KMS("zx=%p\n", zx);

    fbdev = zx_calloc(sizeof(*fbdev));
    if (!fbdev)
    {
        zx_error("alloc fbdev failed\n");
        return -ENOMEM;
    }

    drm_fb_helper_prepare(zx->drm_dev, &fbdev->helper, &zx_fb_helper_funcs);

#if DRM_VERSION_CODE < KERNEL_VERSION(4, 11, 0)
    ret = drm_fb_helper_init(zx->drm_dev, &fbdev->helper, ((disp_info_t*)(zx->disp_info))->num_crtc, 4);
#else
    ret = drm_fb_helper_init(zx->drm_dev, &fbdev->helper, 4);
#endif
    if (ret)
    {
        goto err;
    }

    drm_fb_helper_single_add_all_connectors(&fbdev->helper);

    ret = drm_fb_helper_initial_config(&fbdev->helper, 32);
    if (ret)
    {
        goto err_fini;
    }

    zx->fbdev = fbdev;

    return 0;

err_fini:
    drm_fb_helper_fini(&fbdev->helper);
err:
    zx_free(fbdev);

    return ret;
}

int zx_fbdev_deinit(zx_card_t *zx)
{
    struct zx_fbdev *fbdev = zx->fbdev;
    struct drm_zx_framebuffer *fb = NULL;
    struct fb_info *info;

    if (!fbdev)
    {
        return 0;
    }

    fb = fbdev->fb;

    info = fbdev->helper.fbdev;

#if DRM_VERSION_CODE < KERNEL_VERSION(4, 8, 0)
    unregister_framebuffer(info);
#else
    drm_fb_helper_unregister_fbi(&fbdev->helper);
#endif

#if DRM_VERSION_CODE < KERNEL_VERSION(4, 8, 0)
    iounmap(info->screen_base);
    if (info->cmap.len)
        fb_dealloc_cmap(&info->cmap);
    framebuffer_release(info);
#elif DRM_VERSION_CODE < KERNEL_VERSION(4, 12, 0)
    drm_fb_helper_release_fbi(&fbdev->helper);
#endif

    if (fb->obj)
    {
        zx_gem_object_vunmap(fb->obj);
        zx_gem_object_put(fb->obj);
        fb->obj = NULL;
        drm_framebuffer_unregister_private(&fb->base);
        drm_framebuffer_cleanup(&fb->base);
    }
    drm_fb_helper_fini(&fbdev->helper);
    zx_free(fbdev);

    return 0;
}

void zx_fbdev_set_suspend(zx_card_t *zx, int state)
{
    struct zx_fbdev *fbdev = zx->fbdev;
    struct fb_info *info;

    if (fbdev)
    {
        console_lock();

        info = fbdev->helper.fbdev;

#if DRM_VERSION_CODE < KERNEL_VERSION(4, 8, 0)
        if (info)
            fb_set_suspend(info, state);
#else
        drm_fb_helper_set_suspend(&fbdev->helper, state);
#endif
        console_unlock();
    }
}

void zx_fbdev_restore_mode(zx_card_t *zx)
{
    struct zx_fbdev *fbdev = zx->fbdev;

    if (fbdev)
    {
        drm_fb_helper_restore_fbdev_mode_unlocked(&fbdev->helper);
    }
}

void zx_fbdev_poll_changed(struct drm_device *dev)
{
    zx_card_t*  zx = dev->dev_private;
    struct zx_fbdev *fbdev = zx->fbdev;

#if DRM_VERSION_CODE < KERNEL_VERSION(4, 14, 0)
    if (fbdev && fbdev->fb)
#else
    if (fbdev)
#endif
    {
        drm_fb_helper_hotplug_event(&fbdev->helper);
    }
}

#endif
