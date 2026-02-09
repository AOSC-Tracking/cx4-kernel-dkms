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
#include "zx_drmfb.h"
#include "zx_drm_helper.h"

static void zx_drm_framebuffer_destroy(struct drm_framebuffer *fb)
{
    struct drm_device *dev = fb->dev;
    struct drm_zx_framebuffer *zxfb = to_zxfb(fb);
    zx_card_t *zx = dev->dev_private;

    // unmap gpuva

    krnl_free_gpu_va_t free_va = {0, };
    free_va.base = zxfb->gpu_virt_addr;
    free_va.size = zxfb->obj->info.size;
    zx_core_interface->free_gpu_virtual_address(zx->adapter, &free_va);

    drm_framebuffer_cleanup(fb);
    zx_gem_object_put(zxfb->obj);
    zx_free(zxfb);
}

static int zx_drm_framebuffer_create_handle(struct drm_framebuffer *fb,
                        struct drm_file *file,
                        unsigned int *handle)
{
    struct drm_zx_framebuffer *zxfb = to_zxfb(fb);

    return drm_gem_handle_create(file, &zxfb->obj->base, handle);
}

static int zx_drm_framebuffer_dirty(struct drm_framebuffer *fb,
                        struct drm_file *file,
                        unsigned int flags,
                        unsigned color,
                        struct drm_clip_rect *clips,
                        unsigned int num_clips)
{
    return 0;
}

static const struct drm_framebuffer_funcs zx_fb_funcs =
{
    .destroy = zx_drm_framebuffer_destroy,
    .create_handle = zx_drm_framebuffer_create_handle,
    .dirty = zx_drm_framebuffer_dirty,
};

struct drm_zx_framebuffer*
__zx_framebuffer_create(struct drm_device *dev,
#if DRM_VERSION_CODE >= KERNEL_VERSION(6, 17, 0)
                        const struct drm_format_info *info,
#endif
                        struct drm_mode_fb_cmd2 *mode_cmd,
                        struct drm_zx_gem_object *obj)
{
    int ret;
    zx_card_t *zx = dev->dev_private;
    struct drm_zx_framebuffer *zxfb;
    krnl_map_gpu_va_t map_va= {0};

    // fb only support local+share allocation
    // if (obj->info.has_pages || !obj->info.share)
    //     return ERR_PTR(-EINVAL);

    zxfb = zx_calloc(sizeof(*zxfb));
    if (!zxfb)
        return ERR_PTR(-ENOMEM);

#if DRM_VERSION_CODE >= KERNEL_VERSION(6, 17, 0)
    zx_drm_helper_mode_fill_fb_struct(dev, &zxfb->base, info, mode_cmd);
#else
    zx_drm_helper_mode_fill_fb_struct(dev, &zxfb->base, mode_cmd);
#endif

    zxfb->obj = obj;

    // map gpuva
    map_va.allocation = obj->priv;
    map_va.size = obj->info.size;
    ret = zx_core_interface->map_gpu_virtual_address(zx->adapter, &map_va);
    if (ret != 0)
        goto err_free;

    zxfb->gpu_virt_addr = map_va.gpu_va;

    // cpu_phy_addr only valid after allocation resident
    zx_drm_gem_query_info(zx, obj, &obj->info);

    ret = drm_framebuffer_init(dev, &zxfb->base, &zx_fb_funcs);
    if (ret)
        goto err_unmap;

    return zxfb;
err_unmap:
    {
        krnl_free_gpu_va_t free_va = {0, };
        free_va.base = zxfb->gpu_virt_addr;
        free_va.size = map_va.size;
        zx_core_interface->free_gpu_virtual_address(zx->adapter, &free_va);
    }
err_free:
    zx_free(zxfb);
    return ERR_PTR(-ENOMEM);
}

struct drm_framebuffer *
zx_fb_create(struct drm_device *dev,
             struct drm_file *file,
#if DRM_VERSION_CODE >= KERNEL_VERSION(6, 17, 0)
             const struct drm_format_info *info,
#endif
#if DRM_VERSION_CODE < KERNEL_VERSION(4, 5, 0)
             struct drm_mode_fb_cmd2 *user_mode_cmd
#else
             const struct drm_mode_fb_cmd2 *user_mode_cmd
#endif
              )
{
    struct drm_zx_framebuffer *fb;
    struct drm_zx_gem_object *obj;
    struct drm_mode_fb_cmd2 mode_cmd = *user_mode_cmd;

    obj = zx_drm_gem_object_lookup(dev, file, mode_cmd.handles[0]);
    if (!obj)
        return ERR_PTR(-ENOENT);


#if DRM_VERSION_CODE >= KERNEL_VERSION(6, 17, 0)
    fb = __zx_framebuffer_create(dev, info, &mode_cmd, obj);
#else
    fb = __zx_framebuffer_create(dev, &mode_cmd, obj);
#endif

    if (IS_ERR(fb))
        zx_gem_object_put(obj);

    return &fb->base;
}

