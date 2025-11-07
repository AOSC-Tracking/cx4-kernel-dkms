/*
 * Copyright (c) 2018 Shanghai Zhaoxin Semiconductor Co., Ltd.
 * All Rights Reserved.
 * This is UNPUBLISHED PROPRIETARY SOURCE CODE of Shanghai Zhaoxin Semiconductor Co., Ltd.;
 * the contents of this file may not be disclosed to third parties, copied or
 * duplicated in any form, in whole or in part, without the prior written
 * permission of Shanghai Zhaoxin Semiconductor Co., Ltd.
 *
 * The copyright of the source code is protected by the copyright laws of the People's
 * Republic of China and the related laws promulgated by the People's Republic of China
 * and the international covenant(s) ratified by the People's Republic of China.
 *
 */
#include "zx_disp.h"
#include "zx_cbios.h"
#include "zx_atomic.h"

static void zx_encoder_destroy(struct drm_encoder *encoder)
{
    zx_encoder_t *zx_encoder = to_zx_encoder(encoder);

    drm_encoder_cleanup(encoder);
    zx_free(zx_encoder);
}

static zx_connector_t* zx_encoder_get_connector(zx_encoder_t* zx_encoder)
{
    struct drm_encoder *encoder = &zx_encoder->base_encoder;
    struct drm_device *dev = encoder->dev;
    struct drm_connector *connector;
    zx_connector_t *zx_connector = NULL;

    list_for_each_entry(connector, &dev->mode_config.connector_list, head)
    {
        if (connector->encoder == encoder)
        {
            zx_connector = to_zx_connector(connector);
            break;
        }
    }

    return zx_connector;
}

void zx_encoder_disable(struct drm_encoder *encoder)
{
    struct drm_device *dev = encoder->dev;
    zx_card_t *zx_card = dev->dev_private;
    disp_info_t *disp_info = (disp_info_t *)zx_card->disp_info;
    adapter_info_t*  adapter = disp_info->adp_info;
    psr_data_t* psr_data = disp_info->psr_data;
    zx_encoder_t *zx_encoder = to_zx_encoder(encoder);
    zx_connector_t *zx_connector = zx_encoder_get_connector(zx_encoder);
    struct task_struct *cur_task = current;

    if (!zx_connector)
    {
        return;
    }

    if(zx_encoder->dpms_status)
    {
        disp_cbios_set_hdac_connect_status(disp_info, zx_encoder->output_type, FALSE, FALSE);

        if(zx_connector->support_psr)
        {
            psr_disable(psr_data);
        }

        zx_usleep_range(1000, 1100); //delay 1 ms

        if(cur_task)
        {
            zx_info("Task [%s] turn off power of device: 0x%x.\n", cur_task->comm, zx_encoder->output_type);
        }
        else
        {
            zx_info("To turn off power of device: 0x%x.\n", zx_encoder->output_type);
        }

        zx_mutex_lock(zx_connector->access_lock);

        zx_encoder->dpms_status = 0;

        disp_cbios_set_dpms(disp_info, zx_encoder->output_type, 0);

        zx_mutex_unlock(zx_connector->access_lock);
    }
}

void zx_encoder_enable(struct drm_encoder *encoder)
{
    struct drm_device *dev = encoder->dev;
    zx_card_t *zx_card = dev->dev_private;
    disp_info_t *disp_info = (disp_info_t *)zx_card->disp_info;
    adapter_info_t*  adapter = disp_info->adp_info;
    psr_data_t* psr_data = disp_info->psr_data;
    zx_encoder_t *zx_encoder = to_zx_encoder(encoder);
    zx_connector_t *zx_connector = zx_encoder_get_connector(zx_encoder);
    struct task_struct *cur_task = current;

    if (!zx_connector)
    {
        return;
    }

    if(!zx_encoder->dpms_status)
    {
        if (cur_task)
        {
            zx_info("Task [%s] turn on power of device: 0x%x.\n", cur_task->comm, zx_encoder->output_type);
        }
        else
        {
            zx_info("To turn on power of device: 0x%x.\n", zx_encoder->output_type);
        }

        zx_mutex_lock(zx_connector->access_lock);

        zx_encoder->dpms_status = 1;

        disp_cbios_set_dpms(disp_info, zx_encoder->output_type, 1);

        zx_mutex_unlock(zx_connector->access_lock);

        if (zx_connector->support_audio)
        {
            disp_cbios_set_hdac_connect_status(disp_info, zx_encoder->output_type, TRUE, TRUE);
        }

        if (zx_connector->support_psr)
        {
            psr_enable(psr_data, zx_encoder->output_type);
        }
    }
}

static bool zx_encoder_mode_fixup(struct drm_encoder *encoder,
                                   const struct drm_display_mode *mode,
                                   struct drm_display_mode *adjusted_mode)
{
    struct drm_device* dev = encoder->dev;
    zx_card_t*  zx_card = dev->dev_private;
    disp_info_t*  disp_info = (disp_info_t *)zx_card->disp_info;
    zx_encoder_t *zx_encoder = to_zx_encoder(encoder);
    int output  = zx_encoder->output_type;
    struct drm_display_mode tmp_mode = {0};
    unsigned int dev_mode_size = 0, dev_real_num = 0, i = 0;
    unsigned int adapter_mode_size = 0, adapter_mode_num = 0;
    void *dev_mode_buf = NULL, *adapter_mode_buf = NULL;
    PCBiosModeInfoExt pcbios_mode = NULL, matched_mode = NULL;
    PCBiosModeInfoExt ppreferred_mode = NULL, pmaxium_mode = NULL;

    if (!adjusted_mode)
    {
        return FALSE;
    }

    dev_mode_size = disp_cbios_get_modes_size(disp_info, output);
    if (!dev_mode_size)
    {
        goto End;
    }

    dev_mode_buf = zx_calloc(dev_mode_size);
    if (!dev_mode_buf)
    {
        goto End;
    }

    if(dev_mode_buf)
    {
        dev_real_num = disp_cbios_get_modes(disp_info, output, dev_mode_buf, dev_mode_size);
        for(i = 0; i < dev_real_num; i++)
        {
            pcbios_mode = (PCBiosModeInfoExt)dev_mode_buf + i;
            if((pcbios_mode->XRes == mode->hdisplay) &&
                (pcbios_mode->YRes == mode->vdisplay) &&
                (abs(pcbios_mode->RefreshRate - drm_mode_vrefresh(mode) * 100) < 50) &&
                ((mode->flags & DRM_MODE_FLAG_INTERLACE) ? (pcbios_mode->InterlaceProgressiveCaps == 0x02) : (pcbios_mode->InterlaceProgressiveCaps == 0x01)))
            {
                //hw mode == sw mode
                goto End;
            }
        }
    }


    if (!disp_info->scale_support)
    {
        goto End;
    }

    adapter_mode_size = disp_cbios_get_adapter_modes_size(disp_info);
    if (!adapter_mode_size)
    {
        goto End;
    }

    adapter_mode_buf = zx_calloc(adapter_mode_size);
    if (!adapter_mode_buf)
    {
        goto End;
    }

    ppreferred_mode = disp_cbios_get_preferred_mode((PCBiosModeInfoExt)dev_mode_buf, dev_real_num);
    pmaxium_mode = disp_cbios_get_maxium_mode((PCBiosModeInfoExt)dev_mode_buf);

    adapter_mode_num = disp_cbios_get_adapter_modes(disp_info, adapter_mode_buf, adapter_mode_size);
    for (i = 0; i < adapter_mode_num; i++)
    {
        pcbios_mode = (PCBiosModeInfoExt)adapter_mode_buf + i;

        if (pcbios_mode->XRes == mode->hdisplay &&
            pcbios_mode->YRes == mode->vdisplay &&
            pcbios_mode->RefreshRate/100 == drm_mode_vrefresh(mode))
        {
            if (ppreferred_mode != NULL &&
                ppreferred_mode->XRes >= mode->hdisplay &&
                ppreferred_mode->YRes >= mode->vdisplay &&
                ppreferred_mode->RefreshRate/100 >= drm_mode_vrefresh(mode))
            {
                //perferred as the hw mode
                matched_mode = ppreferred_mode;
                break;
            }

            if (pmaxium_mode != NULL &&
                pmaxium_mode->XRes >= mode->hdisplay &&
                pmaxium_mode->YRes >= mode->vdisplay &&
                pmaxium_mode->RefreshRate/100 >= drm_mode_vrefresh(mode))
            {
                //maxium as the hw mode
                matched_mode = pmaxium_mode;
                break;
            }
        }
    }

    if (matched_mode)
    {
        disp_cbios_cbmode_to_drmmode(disp_info, output, matched_mode, 0, adjusted_mode);
    }

End:

#if DRM_VERSION_CODE < KERNEL_VERSION(5,9,0)
        adjusted_mode->vrefresh = drm_mode_vrefresh(adjusted_mode);
#endif
        disp_cbios_get_mode_timing(disp_info, output, adjusted_mode);

    if(dev_mode_buf)
    {
        zx_free(dev_mode_buf);
    }

    if (adapter_mode_buf)
    {
        zx_free(adapter_mode_buf);
        adapter_mode_buf = NULL;
    }

    return TRUE;
}

#if  DRM_VERSION_CODE >= KERNEL_VERSION(4, 8, 0)

void zx_encoder_atomic_mode_set(struct drm_encoder *encoder,
                                struct drm_crtc_state *crtc_state,
                                struct drm_connector_state *conn_state)
{
    struct drm_device* dev = encoder->dev;
    zx_card_t*  zx_card = dev->dev_private;
    disp_info_t*  disp_info = (disp_info_t *)zx_card->disp_info;
    struct drm_display_mode* mode = &crtc_state->mode;
    struct drm_display_mode* adj_mode = &crtc_state->adjusted_mode;
    struct drm_crtc*  crtc = NULL;
    int  flag = 0;

    crtc = encoder->crtc;

    DRM_DEBUG_KMS("encoder=%d,crtc=%d\n", encoder->index, crtc->index);
    flag |= UPDATE_ENCODER_MODE_FLAG;
    disp_cbios_set_mode(disp_info, drm_crtc_index(crtc), mode, adj_mode, flag);
}

#else

void zx_encoder_mode_set(struct drm_encoder *encoder,
                          struct drm_display_mode *mode,
                          struct drm_display_mode *adjusted_mode)
{
    struct drm_device* dev = encoder->dev;
    struct drm_crtc* crtc = encoder->crtc;
    zx_card_t*  zx_card = dev->dev_private;
    disp_info_t*  disp_info = (disp_info_t *)zx_card->disp_info;
    int flag = 0;

    flag = UPDATE_ENCODER_MODE_FLAG;

    disp_cbios_set_mode(disp_info, to_zx_crtc(crtc)->pipe, mode, adjusted_mode, flag);
}

#endif

static const struct drm_encoder_funcs zx_encoder_funcs =
{
    .destroy = zx_encoder_destroy,
};

#if  DRM_VERSION_CODE >= KERNEL_VERSION(4, 8, 0)

static const struct drm_encoder_helper_funcs zx_encoder_helper_funcs =
{
    .disable = zx_encoder_disable,
    .enable = zx_encoder_enable,
    .mode_fixup = zx_encoder_mode_fixup,
#if DRM_VERSION_CODE >= KERNEL_VERSION(4, 9, 0)
    .atomic_mode_set = zx_encoder_atomic_mode_set,
#endif
};

#else

static const struct drm_encoder_helper_funcs zx_encoder_helper_funcs =
{
    .mode_fixup = zx_encoder_mode_fixup,
    .prepare = zx_encoder_disable,
    .commit = zx_encoder_enable,
    .mode_set = zx_encoder_mode_set,
    .disable = zx_encoder_disable,
};

#endif

struct drm_encoder* zx_drm_encoder_create(disp_info_t* disp_info, disp_output_type output)
{
    zx_card_t*  zx_card = disp_info->zx_card;
    struct drm_device* drm = zx_card->drm_dev;
    struct drm_encoder* encoder = NULL;
    zx_encoder_t* zx_encoder = NULL;
    int encoder_type = 0;
    int ret = -ENOMEM;

    zx_encoder = zx_calloc(sizeof(zx_encoder_t));
    if (!zx_encoder)
    {
        DRM_ERROR("failed to alloc zx encoder\n");
        goto failed;
    }

    encoder = &zx_encoder->base_encoder;
    encoder->possible_clones = 0;
    encoder->possible_crtcs = disp_cbios_get_crtc_mask(disp_info, output);

    switch (output)
    {
    case DISP_OUTPUT_CRT:
        encoder_type = DRM_MODE_ENCODER_DAC;
        break;
    case DISP_OUTPUT_DP1:
    case DISP_OUTPUT_DP2:
    case DISP_OUTPUT_DP3:
    case DISP_OUTPUT_DP4:
        encoder_type = DRM_MODE_ENCODER_TMDS;
        break;
    default:
        encoder_type = DRM_MODE_ENCODER_NONE;
        break;
    }

    ret = drm_encoder_init(drm, encoder, &zx_encoder_funcs, encoder_type
#if DRM_VERSION_CODE >= KERNEL_VERSION(4, 5, 0)
                     , NULL
#endif
        );

    if (ret != 0)
    {
        DRM_ERROR("failed to initialize encoder\n");
        goto failed_init_encoder;
    }

    drm_encoder_helper_add(encoder, &zx_encoder_helper_funcs);

    zx_encoder->output_type = output;

    zx_encoder->dpms_status = 0;

    return encoder;

failed_init_encoder:
    zx_free(zx_encoder);
    zx_encoder = NULL;

failed:
    return ERR_PTR(ret);
}
