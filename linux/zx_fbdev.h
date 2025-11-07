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
#ifndef _H_ZX_FBDEV_H
#define _H_ZX_FBDEV_H
#include "zx_drmfb.h"
#include "zx_device_debug.h"

#if DRM_VERSION_CODE < KERNEL_VERSION(4, 19, 0)

struct zx_fbdev
{
    struct drm_fb_helper helper;
    struct drm_zx_framebuffer *fb;
};

#define to_zx_fbdev(helper) container_of(helper, struct zx_fbdev, helper)

int zx_fbdev_init(zx_card_t *zx);
int zx_fbdev_deinit(zx_card_t *zx);
void zx_fbdev_set_suspend(zx_card_t *zx, int state);
void zx_fbdev_restore_mode(zx_card_t *zx);
void zx_fbdev_poll_changed(struct drm_device *dev);
#endif

#endif
