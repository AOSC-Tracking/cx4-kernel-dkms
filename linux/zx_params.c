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

#include "zx_params.h"
#include "zx_driver.h"

struct zx_params zx_modparams __read_mostly = {
    .zx_fb_mode = NULL,
#ifdef ZX_HW_NULL
    .zx_fb = 0,
#else
    .zx_fb = 1,
#endif
    .zx_pwm_mode = 0x61, /*default value, which means enable 3D auto mode, VCP/VPP manual mode            
                            for chx005:
                            bit| 15     14   13  12  |11   10    9   8   |   7    6    5   4   | 3   2    1   0  |
                               |       vpp  vcp  gfx |     vpp  vcp  gfx |       vpp  vcp  gfx |     vpp  vcp gfx| 
                               |-------manual--------|------auto---------|--------manual-------|-----auto--------|
                               |-------------pg--------------------------|--------------------cg-----------------|
                            bit15~bit12 ：  pg manual
                            bit11~bit8  :   pg auto
                            bit7~bit4   :   cg manual
                            bit3~bit0   :   cg auto    */
    .zx_dfs_mode = 0xc9,
    .zx_worker_thread_enable = 1,
    .zx_recovery_enable = 1,
    .zx_hang_dump = 0,            /*0-disable, 1-pre hang, 2-post hang, 3-duplicate hang */
    .zx_run_on_qt = 0,
    .zx_flag_buffer_verify = 1,  /*0 - disable, 1 - enable */
    .miu_channel_size = 0,   //0/1/2 for 256B/512B/1kb Swizzle
    .page_table_seg = 0,   /* 0-local_first, 1-force_local 2-force_pcie */

    //gpc/slice setting
    .chip_slice_mask = 0x3, //// CHIP_SLICE_MASK own 12 bits(should be set 0x001 ~ 0xfff), driver use this to set the Slice_Mask which HW used.
    .zx_local_size_g = 1,
    .zx_local_size_m = 0,
    .debugfs_mask   =  0x1,
    .nonsnoop_patch = 0,
    .zx_set_high_eclk = 0,
};

#define zx_param_named(name, T, perm, desc) \
    module_param_named(name, zx_modparams.name, T, perm); \
    MODULE_PARM_DESC(name, desc)


zx_param_named(zx_fb, int, 0600, "enable zx drm fb 0=disable, 1=enable");
zx_param_named(zx_fb_mode, charp, 0600, "The fb mode, like string 1920x1080@60");
zx_param_named(zx_pwm_mode, int, 0444,"control pwm mode, 1: manual 0:auto");
zx_param_named(zx_dfs_mode, int, 0444, "control of dfs");
zx_param_named(zx_worker_thread_enable, int, 0444, "enable work thread to submit");
zx_param_named(zx_recovery_enable, int, 0444, "enable recovery");
zx_param_named(zx_hang_dump, int, 0444, "0-disable, 1-pre hang, 2-post hang, 3-duplicate hang");
zx_param_named(zx_flag_buffer_verify, int, 0444, "");
zx_param_named(zx_run_on_qt, int, 0444, "");
zx_param_named(miu_channel_size, int, 0444, "0/1/2 for 256B/512B/1kb Swizzle");
zx_param_named(chip_slice_mask, int, 0444, "12 bits(should be set 0x001 ~ 0xfff),");
zx_param_named(zx_local_size_g, int, 0444, "manual set the local vram size, uint in GB, the size should not larger than real vram size");
zx_param_named(zx_local_size_m, int, 0444, "manual set the local vram size, uint in MB, the size should not larger than real vram size");
zx_param_named(debugfs_mask, int, 0444, "debugfs control bits");
zx_param_named(page_table_seg, int, 0444, "page_table location, 0-local_first 1-force_local 2-force_pcie");
zx_param_named(zx_set_high_eclk, int, 0444, "elck set, 1:600M, 0:default");



