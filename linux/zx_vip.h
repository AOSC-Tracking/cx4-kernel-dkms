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

#ifndef __ZX_VIP_H_
#define __ZX_VIP_H_

#include "zx_capture_drv.h"

#define ZX_VIP_ENABLE       0
#define ZX_VIP_DISABLE      1
#define ZX_VIP_SET_MODE     2
#define ZX_VIP_SET_BUFFER   3
#define ZX_VIP_QUERY_CAPS   4

#define ZX_CAPTURE_VIP_INTERFACE

typedef struct
{
    zx_capture_t *mcapture;
    unsigned char fb_num;
    unsigned char vip;
    void (*cb)(zx_capture_id_t id, void *data, unsigned int flags);
    void *cb_data;
    struct work_struct vip_work;
}vip_spec_info_t;

typedef struct
{
    unsigned char vip;
    unsigned int  op;
    union{
        struct
        {
            unsigned int fmt;
            unsigned int chip;
            unsigned int x_res;
            unsigned int y_res;
            unsigned int refs;
        }mode;

        struct
        {
            unsigned char fb_num;
            unsigned char fb_idx;
            unsigned long long fb_addr;
        }fb;

        struct
        {
            unsigned int mode_num;
            zx_vip_mode_t *mode;
        }caps;
    };
}zx_vip_set_t;

void disp_register_vip_capture(disp_info_t *disp_info);
void disp_unregister_vip_capture(disp_info_t *disp_info);

#endif
