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

#ifndef _ZX_ATOMIC_H_
#define _ZX_ATOMIC_H_

#include "zx_disp.h"
#include "zx_cbios.h"

#if DRM_VERSION_CODE >= KERNEL_VERSION(4, 8, 0)

struct drm_atomic_state* zx_atomic_state_alloc(struct drm_device *dev);

void zx_atomic_state_clear(struct drm_atomic_state *s);

void zx_atomic_state_free(struct drm_atomic_state *s);

void zx_atomic_helper_commit_tail(struct drm_atomic_state *old_state);

#endif

#endif
