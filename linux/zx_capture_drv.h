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

#ifndef __ZX_CAPTURE_DRV_H_
#define __ZX_CAPTURE_DRV_H_

#include "zx_capture.h"

#define CAPTURE_INIT_B          (0x1 << 1)
#define CAPTURE_ENABLE_B        (0x1 << 2)
#define CAPTURE_NEED_SKIP_B     (0x1 << 3)

typedef struct _zx_capture_desc
{
    zx_capture_id_t cf_fmt_id;
    char* cf_name;
}zx_capture_desc;

struct zx_capture_type
{
    zx_capture_id_t cf_fmt_id;
    char *cf_name;
    int (*cf_ctl)(disp_info_t *disp_info, struct zx_capture_type *capture, void *params);
    void (*notify_interrupt)(disp_info_t *disp_info, struct zx_capture_type *capture);
    unsigned int flags;
    void *priv_info;
    struct zx_capture_type *cf_next;
};

typedef struct zx_capture_type zx_capture_t;

typedef struct _zx_cf_irq_tbl_t
{
    unsigned int irq_mask;
    zx_capture_id_t cf_id;
}zx_cf_irq_tbl_t;


void zx_capture_handle(disp_info_t *disp_info, unsigned int itrr);
void disp_register_capture(disp_info_t *disp_info, zx_capture_t *capture);
void disp_unregister_capture(disp_info_t *disp_info, zx_capture_t *capture);
zx_capture_t *disp_find_capture(disp_info_t *disp_info, int id);
void disp_capture_init(disp_info_t *disp_info);
void disp_capture_deinit(disp_info_t *disp_info);


#endif
