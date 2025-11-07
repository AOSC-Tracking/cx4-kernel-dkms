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
#include "zx_wb.h"
#include "zx_cbios.h"
#include "zx_drm_helper.h"

int disp_wb_ops(disp_info_t *disp_info, zx_capture_t *capture, void *params);
void disp_wb_notify_interrupt(disp_info_t *disp_info, zx_capture_t *capture);
void disp_wb_work_func(struct work_struct *work);

static zx_capture_desc zx_wb_descs[] =
{
    {
        .cf_fmt_id = ZX_CAPTURE_WB1,
        .cf_name   = "WB1"
    },
    {
        .cf_fmt_id = ZX_CAPTURE_WB2,
        .cf_name   = "WB2"
    },
    {
        .cf_fmt_id = ZX_CAPTURE_WB3,
        .cf_name   = "WB3"
    },
    {
        .cf_fmt_id = ZX_CAPTURE_WB4,
        .cf_name   = "WB4"
    }
};

static unsigned char cf_id2iga(zx_capture_id_t cf_fmt_id)
{
    if (cf_fmt_id == ZX_CAPTURE_WB1)
    {
        return IGA1;
    }

    if (cf_fmt_id == ZX_CAPTURE_WB2)
    {
        return IGA2;
    }

    if (cf_fmt_id == ZX_CAPTURE_WB3)
    {
        return IGA3;
    }

    if (cf_fmt_id == ZX_CAPTURE_WB4)
    {
        return IGA4;
    }

    return -1;
}

void disp_register_wb_capture(disp_info_t *disp_info)
{
    zx_capture_t *capture = NULL;
    int i = 0;

    for (i = 0; i < sizeof(zx_wb_descs) / sizeof(zx_capture_desc); i++)
    {
        capture = (zx_capture_t *)zx_calloc(sizeof(zx_capture_t));
        capture->cf_fmt_id = zx_wb_descs[i].cf_fmt_id;
        capture->cf_name = zx_wb_descs[i].cf_name;
        capture->cf_ctl = disp_wb_ops;
        capture->notify_interrupt = disp_wb_notify_interrupt;
        disp_register_capture(disp_info, capture);
    }
}

void disp_unregister_wb_capture(disp_info_t *disp_info)
{
    zx_capture_t *capture = NULL;
    int i = 0;

    for (i = 0; i < sizeof(zx_wb_descs) / sizeof(zx_capture_desc); i++)
    {
        capture = disp_find_capture(disp_info, zx_wb_descs[i].cf_fmt_id);
        if (capture)
        {
            disp_unregister_capture(disp_info, capture);
            zx_free(capture);
        }
    }
}

int disp_wb_capture_init(zx_capture_t *capture, wb_op_params_t *mparams)
{
    wb_spec_info_t *info;

    capture->priv_info = zx_calloc(sizeof(wb_spec_info_t));

    info = (wb_spec_info_t *)capture->priv_info;

    info->mcapture = capture;
    info->iga = cf_id2iga(capture->cf_fmt_id);

    INIT_WORK(&info->wb_work, disp_wb_work_func);

    capture->flags |=  CAPTURE_INIT_B;

    capture->flags |=  CAPTURE_NEED_SKIP_B;

    zx_info("capture %s init\n", capture->cf_name);

    return 0;
}

int disp_wb_capture_deinit(zx_capture_t *capture, wb_op_params_t *mparams)
{
    wb_spec_info_t *info = (wb_spec_info_t *)capture->priv_info;

    capture->flags  &=  ~CAPTURE_INIT_B;

    cancel_work_sync(&info->wb_work);

    zx_free(capture->priv_info);
    capture->priv_info = NULL;

    zx_info("capture %s deinit \n", capture->cf_name);

    return 0;
}

int disp_wb_query_caps(disp_info_t *disp_info, zx_capture_t *capture, wb_op_params_t *mparams)
{
    wb_spec_info_t *info = (wb_spec_info_t *)capture->priv_info;
    zx_wb_set_t wb_set = {0};
    int   status = 0;

    wb_set.iga_idx = info->iga;
    wb_set.op_flags  = ZX_WB_QUERY_CAPS;

    status = disp_cbios_wb_ctl(disp_info, &wb_set);

    mparams->query_caps_param.width = wb_set.input_mode.src_x;
    mparams->query_caps_param.height = wb_set.input_mode.src_y;
    mparams->query_caps_param.refrate = wb_set.input_mode.refreshrate;
    mparams->query_caps_param.capture_fmt = wb_set.input_mode.capture_fmt;

    if (status)
    {
        zx_error("capture %s query caps failed!! \n", capture->cf_name);
    }
    else
    {
        zx_info("capture %s query mode\n", capture->cf_name);
    }


    return status;
}


int disp_wb_set_buffer(disp_info_t *disp_info, zx_capture_t *capture, wb_op_params_t *mparams)
{
    wb_spec_info_t *info = (wb_spec_info_t *)capture->priv_info;
    zx_wb_set_t wb_set = {0};

    int   status = 0;

    if (info->wb_status & ZX_WB_MODE_BYPASS_S)
    {
        wb_set.op_flags |= ZX_WB_SET_BYPASS_MODE;
    }

    if (info->wb_status & ZX_WB_VSYNC_OFF_S)
    {
        wb_set.op_flags |= ZX_WB_SET_VSYNC_OFF;
    }

    wb_set.iga_idx = info->iga;
    wb_set.op_flags |= ZX_WB_SET_BUFFER;
    wb_set.fb.fb_addr = mparams->set_buffer_param.fbAddr;

    status = disp_cbios_wb_ctl(disp_info, &wb_set);

    if (status)
    {
        zx_error("capture %s set buffer failed!! \n", capture->cf_name);
    }
    else
    {
        zx_info("capture %s set buffer, buf %x \n", capture->cf_name, mparams->set_buffer_param.fbAddr);
    }

    return status;
}


int disp_wb_set_mode(disp_info_t *disp_info, zx_capture_t *capture, wb_op_params_t *mparams)
{
    wb_spec_info_t *info = (wb_spec_info_t *)capture->priv_info;
    zx_wb_set_t wb_set = {0};
    int   status = 0;

    info->wb_status |= ZX_WB_MODE_SETTING_S;

    if (mparams->set_mode_param.bByPass)
    {
        info->wb_status |= ZX_WB_MODE_BYPASS_S;
    }

    if (mparams->set_mode_param.bUpdateImme)
    {
        info->wb_status |= ZX_WB_VSYNC_OFF_S;
    }

    info->mode_set.src_x      = mparams->set_mode_param.srcX;
    info->mode_set.src_y      = mparams->set_mode_param.srcY;
    info->mode_set.out_fmt    = mparams->set_mode_param.outFmt;
    info->mode_set.work_mode  = mparams->set_mode_param.mode;
    info->mode_set.dst_x      = mparams->set_mode_param.dstX;
    info->mode_set.dst_y      = mparams->set_mode_param.dstY;
    info->mode_set.double_buf = mparams->set_mode_param.bDoubleBuffer;

    if (status)
    {
        zx_error("capture %s set mode failed!! \n", capture->cf_name);
    }
    else
    {
        zx_info("capture %s set mode\n", capture->cf_name);
    }


    return status;
}


int disp_wb_capture_enable(disp_info_t *disp_info, zx_capture_t *capture, wb_op_params_t *mparams)
{
    zx_card_t*  zx_card = disp_info->zx_card;
    struct drm_device*  dev = zx_card->drm_dev;
    struct drm_crtc* crtc = NULL;
    wb_spec_info_t *info = (wb_spec_info_t *)capture->priv_info;
    zx_wb_set_t wb_set = {0};
    int   status = 0;

    wb_set.iga_idx = info->iga;
    wb_set.op_flags = ZX_WB_ENABLE;

    if (info->wb_status & ZX_WB_MODE_SETTING_S)
    {
        wb_set.op_flags |= ZX_WB_SET_MODE;
        info->wb_status &= ~ZX_WB_MODE_SETTING_S;
        zx_memcpy(&wb_set.mode, &info->mode_set, sizeof(zx_wb_mode_set_t));
    }

    if (info->wb_status & ZX_WB_MODE_BYPASS_S)
    {
        wb_set.op_flags |= ZX_WB_SET_BYPASS_MODE;
    }

    if (info->wb_status & ZX_WB_VSYNC_OFF_S)
    {
        wb_set.op_flags |= ZX_WB_SET_VSYNC_OFF;
    }

    status = disp_cbios_wb_ctl(disp_info, &wb_set);

    if (status)
    {
        zx_error("capture %s enable failed!! \n", capture->cf_name);
    }
    else
    {
        capture->flags  |=  CAPTURE_ENABLE_B;
        zx_info("capture %s enable\n", capture->cf_name);

        //vblank off should be disabled after wb enable, because wb rely on vsync interrupt
        zx_drm_for_each_crtc(crtc, dev)
        {
            if(to_zx_crtc(crtc)->pipe == info->iga)
            {
                drm_crtc_vblank_get(crtc);
                break;
            }
        }
    }

    return status;
}

int disp_wb_capture_disable(disp_info_t *disp_info, zx_capture_t *capture, wb_op_params_t *mparams)
{
    zx_card_t*  zx_card = disp_info->zx_card;
    struct drm_device*  dev = zx_card->drm_dev;
    struct drm_crtc* crtc = NULL;
    wb_spec_info_t *info = (wb_spec_info_t *)capture->priv_info;
    zx_wb_set_t wb_set = {0};
    int   status = 0;

    wb_set.iga_idx = info->iga;
    wb_set.op_flags = ZX_WB_DISABLE;

    status = disp_cbios_wb_ctl(disp_info, &wb_set);

    if (status)
    {
        zx_error("capture %s disable failed!! \n", capture->cf_name);
    }
    else
    {
        capture->flags  &=  ~CAPTURE_ENABLE_B;
        zx_info("capture %s disable\n", capture->cf_name);

        //After wb disable, should enable vblank off
        zx_drm_for_each_crtc(crtc, dev)
        {
            if(to_zx_crtc(crtc)->pipe == info->iga)
            {
                drm_crtc_vblank_put(crtc);
                break;
            }
        }
    }

    return status;
}

void disp_wb_notify_interrupt(disp_info_t *disp_info, zx_capture_t *capture)
{
    wb_spec_info_t*  info = (wb_spec_info_t *)capture->priv_info;

    schedule_work(&info->wb_work);
}

void disp_wb_work_func(struct work_struct *work)
{
    wb_spec_info_t*  info = container_of(work, wb_spec_info_t, wb_work);
    zx_capture_t* capture  = info->mcapture;
    unsigned int flags = 0;

    if (!(capture->flags & CAPTURE_ENABLE_B))
    {
        return;
    }

    flags |= ZX_CAPTURE_SET_BUFFER;

    //the first capture should skip as it just triggered
    if (capture->flags & CAPTURE_NEED_SKIP_B)
    {
        flags |= ZX_CAPTURE_SKIP_FRAME;
        capture->flags &= ~CAPTURE_NEED_SKIP_B;
    }

    info->cb(capture->cf_fmt_id, info->cb_data, flags);
}

int disp_wb_ops(disp_info_t *disp_info, zx_capture_t *capture, void *params)
{
    wb_op_params_t *mparams = (wb_op_params_t *)params;

    int   status = 0;

    switch(mparams->op)
    {
    case ZX_CAPTURE_OP_INIT:
    {
        status = disp_wb_capture_init(capture, mparams);
    }
    break;

    case ZX_CAPTURE_OP_DEINIT:
    {
        status = disp_wb_capture_deinit(capture, mparams);
    }
    break;

    case ZX_CAPTURE_OP_ENABLE:
    {
        status = disp_wb_capture_enable(disp_info, capture, mparams);
    }
    break;

    case ZX_CAPTURE_OP_DISABLE:
    {
        status = disp_wb_capture_disable(disp_info, capture, mparams);
    }
    break;

    case ZX_CAPTURE_OP_QUERY_CAPS:
    {
        status = disp_wb_query_caps(disp_info, capture, mparams);
    }
    break;

    case ZX_CAPTURE_OP_SET_MODE:
    {
        status = disp_wb_set_mode(disp_info, capture, mparams);
    }
    break;

    case ZX_CAPTURE_OP_SET_BUFF:
    {
        status = disp_wb_set_buffer(disp_info, capture, mparams);
    }
    break;

    case ZX_CAPTURE_OP_SET_CALLBACK:
    {
        wb_spec_info_t *info = (wb_spec_info_t *)capture->priv_info;
        info->cb = mparams->set_callback_param.cb;
        info->cb_data = mparams->set_callback_param.cb_data;
    }
    break;

    default:
    {
        zx_error("op not support on write back\n");
    }
    break;
    }

    return status;
}
