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

#ifndef _ZX_PLANE_H_
#define _ZX_PLANE_H_

#include "zx_disp.h"
#include "zx_cbios.h"

void zx_plane_destroy(struct drm_plane *plane);

zx_plane_t *zx_drm_plane_create(disp_info_t *disp_info, int index, ZX_PLANE_TYPE type);

#endif
