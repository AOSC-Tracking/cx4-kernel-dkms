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
#ifndef  _ZX_DRMFB_H_
#define _ZX_DRMFB_H_

#include "zx_disp.h"
#include "zx_gem.h"
#include "zx_gem_priv.h"
#if DRM_VERSION_CODE >= KERNEL_VERSION(6, 0, 0)
#include <drm/drm_framebuffer.h>
#endif

struct drm_zx_framebuffer
{
    struct drm_framebuffer base;
    struct drm_zx_gem_object *obj;
    unsigned long long gpu_virt_addr;
};
#define to_zxfb(fb) container_of((fb), struct drm_zx_framebuffer, base)

struct drm_framebuffer *
zx_fb_create(struct drm_device *dev,
                              struct drm_file *file_priv,
                              #if DRM_VERSION_CODE >= KERNEL_VERSION(4, 5, 0)
                              const struct drm_mode_fb_cmd2 *mode_cmd
                              #else
                              struct drm_mode_fb_cmd2 *mode_cmd
                              #endif
                              );

void zx_cleanup_fb(struct drm_plane *plane,  struct drm_plane_state *old_state);

struct drm_zx_framebuffer*
__zx_framebuffer_create(struct drm_device *dev,
                        struct drm_mode_fb_cmd2 *mode_cmd,
                        struct drm_zx_gem_object *obj);
#endif
