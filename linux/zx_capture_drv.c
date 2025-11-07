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

#include "zx_disp.h"
#include "zx_vip.h"
#include "zx_wb.h"
#include "zx_capture_drv.h"

static disp_info_t *g_disp_info = NULL;

void disp_register_capture(disp_info_t *disp_info, zx_capture_t *capture)
{
    capture->cf_next = disp_info->captures;
    disp_info->captures  = capture;
}

void disp_unregister_capture(disp_info_t *disp_info, zx_capture_t *capture)
{
    zx_capture_t **actcf;

    for (actcf = &disp_info->captures; *actcf && *actcf != capture; actcf = &((*actcf)->cf_next))
        ;

    if (*actcf)
        *actcf = (*actcf)->cf_next;
}


zx_capture_t *disp_find_capture(disp_info_t *disp_info, int id)
{

    zx_capture_t *actcf;

    for (actcf = disp_info->captures; actcf && actcf->cf_fmt_id != id; actcf = actcf->cf_next)
        ;

    return actcf;
}


int disp_capture_ctl(void *pdata, zx_capture_id_t id, void *params)
{
    disp_info_t *disp_info = (disp_info_t *)pdata;

    zx_capture_t *capture = disp_find_capture(disp_info, id);


    if (!capture)
    {
        zx_error("wrong capture fmt id !!!");

        return -EINVAL;
    }

    return capture->cf_ctl(disp_info, capture, params);
}


void disp_capture_init(disp_info_t *disp_info)
{
    zx_info("disp cature init enter \n");
    disp_register_vip_capture(disp_info);
    disp_register_wb_capture(disp_info);

    //TODO: refine the card index function
    g_disp_info = disp_info;

    zx_info("disp cature init out \n");
}

void disp_capture_deinit(disp_info_t *disp_info)
{
    zx_info("disp cature deinit enter \n");
    disp_unregister_vip_capture(disp_info);
    disp_unregister_wb_capture(disp_info);

    //TODO: refine the card index function
    g_disp_info = NULL;

    zx_info("disp cature deinit out \n");
}


#define ZX_CF_INT_TBL_ENTRY(_cf_id, _irq_mask)       \
    {                                                \
        .cf_id    = _cf_id,                          \
        .irq_mask = _irq_mask,                       \
    }

static zx_cf_irq_tbl_t cf_irq_tbl[] =
{
    // ZX_CF_INT_TBL_ENTRY(ZX_CAPTURE_VIP1, INT_VIP1),
    // ZX_CF_INT_TBL_ENTRY(ZX_CAPTURE_VIP2, INT_VIP2),
    // ZX_CF_INT_TBL_ENTRY(ZX_CAPTURE_VIP3, INT_VIP3),
    // ZX_CF_INT_TBL_ENTRY(ZX_CAPTURE_VIP4, INT_VIP4),
    ZX_CF_INT_TBL_ENTRY(ZX_CAPTURE_WB1,  INT_VSYNC1),
    ZX_CF_INT_TBL_ENTRY(ZX_CAPTURE_WB2,  INT_VSYNC2),
    ZX_CF_INT_TBL_ENTRY(ZX_CAPTURE_WB3,  INT_VSYNC3),
    ZX_CF_INT_TBL_ENTRY(ZX_CAPTURE_WB4,  INT_VSYNC4),
};

void zx_capture_handle(disp_info_t *disp_info, unsigned int itrr)
{
    zx_capture_t *capture = NULL;
    int i = 0;

    for (; i < sizeof(cf_irq_tbl) / sizeof(zx_cf_irq_tbl_t); i++)
    {
        if (itrr & cf_irq_tbl[i].irq_mask)
        {
            capture = disp_find_capture(disp_info, cf_irq_tbl[i].cf_id);
            if (capture != NULL && (capture->flags & CAPTURE_ENABLE_B))
            {
                capture->notify_interrupt(disp_info, capture);
            }
        }

    }

}


void zx_krnl_get_capture_interface(unsigned int id, void **pdata, PFN_ZX_CAPTURE_OPS_T *interface)
{
    *pdata = (void *)(g_disp_info);
    *interface = disp_capture_ctl;
}

EXPORT_SYMBOL(zx_krnl_get_capture_interface);
