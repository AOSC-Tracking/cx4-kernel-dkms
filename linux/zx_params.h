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

#ifndef __ZX_PARAMS_H__
#define __ZX_PARAMS_H__
#include <linux/cache.h> /* for __read_mostly */

struct zx_params {
    char *zx_fb_mode;
    int  zx_fb;

    int zx_pwm_mode ; /*0-disable PG, 1-enable 3D PG auto mode, 2-enable VCP auto mode, 4-enable VPP auto mode,
                                     0x10-enable 3D manual mode, 0x20-enableV VCP manual mode, 0x40-enable VPP manual mode*/
    int zx_dfs_mode ;/* control dvfs of zx, 0-disable , 1-enable kmd auto tuning */
    int zx_worker_thread_enable;/* control worker thread on/off */
    int zx_recovery_enable ; /* enable recovery when hw hang */
    int zx_hang_dump;/*0-disable, 1-pre hang, 2-post hang, 3-duplicate hang */
    int zx_run_on_qt; /* control wether run on QT */
    int zx_flag_buffer_verify ;/*0 - disable, 1 - enable */
    int zx_vesa_tempbuffer_enable ; /* control wether reserve memory during boot */

    int miu_channel_num;    // 0/1/2 for 1/2/3 Miu channel, Elite 3000 support up to 3 MIU channel
    int miu_channel_size;   //0/1/2 for 256B/512B/1kb Swizzle

    //gpc/slice setting
    unsigned int chip_slice_mask;//// CHIP_SLICE_MASK own 12 bits(should be set 0x001 ~ 0xfff), driver use this to set the Slice_Mask which HW used.
    int mem_size;  // 1/2/3/4 for 4G/8G/12G/16G

    int page_table_seg; /* 0-local_first, 1-force_local 2-force_pcie */

    int zx_set_high_eclk;

    int zx_local_size_g;
    int zx_local_size_m;
    int  debugfs_mask;    //debugfs mask, bit0: gem_enable
    int nonsnoop_patch;
};

extern struct zx_params zx_modparams;
#endif
