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

#ifndef __ZX_MODIFIES_H_
#define __ZX_MODIFIES_H_

/*this piece code should be place in drm/drm-fourcc.h in kernel/mesa*/

#include <drm/drm_fourcc.h>

/*here just for compatiable for drm linear mod*/
/* ZX Linear mode define as same as system define, can be shared with other app/vendors */

#ifndef DRM_FORMAT_MOD_VENDOR_NONE
#define DRM_FORMAT_MOD_VENDOR_NONE    0
#endif

#ifndef DRM_FORMAT_MOD_NONE
#define DRM_FORMAT_MOD_NONE    0
#endif

#ifndef DRM_FORMAT_RESERVED
#define DRM_FORMAT_RESERVED    ((1ULL << 56) - 1)
#endif

#ifndef DRM_FORMAT_MOD_INVALID
#define DRM_FORMAT_MOD_INVALID    fourcc_mod_code(NONE, DRM_FORMAT_RESERVED)
#endif

/*
 * Linear Layout
 *
 * Just plain linear layout. Note that this is different from no specifying any
 * modifier (e.g. not setting DRM_MODE_FB_MODIFIERS in the DRM_ADDFB2 ioctl),
 * which tells the driver to also take driver-internal information into account
 * and so might actually result in a tiled framebuffer.
 */
#ifndef DRM_FORMAT_MOD_LINEAR
#define DRM_FORMAT_MOD_LINEAR    fourcc_mod_code(NONE, 0)
#endif

#define DRM_FORMAT_MOD_VENDOR_ZX 0x19
/*usage  for display specially requirement*/
#define DRM_FORMAT_MOD_ZX_DISPLAY               fourcc_mod_code(ZX, 1)
#define DRM_FORMAT_MOD_ZX_LINEAR                DRM_FORMAT_MOD_LINEAR
#define DRM_FORMAT_MOD_ZX_TILED                 fourcc_mod_code(ZX, 3)
#define DRM_FORMAT_MOD_ZX_COMPRESS              fourcc_mod_code(ZX, 4)
#define DRM_FORMAT_MOD_ZX_TILED_COMPRESS        fourcc_mod_code(ZX, 5)


#define DRM_FORMAT_MOD_ZX_LOCAL                 fourcc_mod_code(ZX, 6)
#define DRM_FORMAT_MOD_ZX_PCIE                  fourcc_mod_code(ZX, 7)

#define DRM_FORMAT_MOD_ZX_INVALID               DRM_FORMAT_MOD_INVALID
#endif

