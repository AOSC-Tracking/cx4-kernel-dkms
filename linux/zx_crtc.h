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
#ifndef  _ZX_CRTC_H_
#define _ZX_CRTC_H_

#include "zx_disp.h"
#include "zx_cbios.h"

zx_crtc_t *zx_drm_crtc_create(disp_info_t *disp_info, struct drm_plane *primary_plane,
                              struct drm_plane *cursor_plane, unsigned int index);


#if DRM_VERSION_CODE >= KERNEL_VERSION(4, 8, 0)

static inline u32 drm_get_crtc_index(struct drm_crtc *crtc)
{
    return crtc->index;
}

static inline struct drm_framebuffer *drm_get_crtc_primary_fb(struct drm_crtc *crtc)
{
    return crtc->primary->fb;
}

static inline void drm_set_crtc_primary_fb(struct drm_crtc *crtc, struct drm_framebuffer *fb)
{
    crtc->primary->fb = fb;
}

#else

static inline u32 drm_get_crtc_index(struct drm_crtc *crtc)
{
    return to_zx_crtc(crtc)->pipe;
}
static inline struct drm_framebuffer *drm_get_crtc_primary_fb(struct drm_crtc *crtc)
{
    return crtc->primary->fb;
}

static inline void drm_set_crtc_primary_fb(struct drm_crtc *crtc, struct drm_framebuffer *fb)
{
    crtc->primary->fb = fb;
}
#endif

#if DRM_VERSION_CODE >= KERNEL_VERSION(4, 4, 0)
typedef unsigned int pipe_t;
#else
typedef int pipe_t;
#endif

#endif
