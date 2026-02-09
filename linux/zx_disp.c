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
#include "zx_crtc.h"
#include "zx_plane.h"
#include "zx_drmfb.h"
#include "zx_irq.h"
#include "zx_fbdev.h"
#include "zx_capture_drv.h"
#if DRM_VERSION_CODE < KERNEL_VERSION(4, 8, 0)
#include <drm/drm_plane_helper.h>
#endif
#include "zx_drm_helper.h"
#include "zx_version.h"

static const struct drm_mode_config_funcs  zx_kms_mode_funcs = {
    .fb_create = zx_fb_create,
#if DRM_VERSION_CODE < KERNEL_VERSION(4, 19, 0)
    .output_poll_changed = zx_fbdev_poll_changed,
#else
#if DRM_VERSION_CODE < KERNEL_VERSION(6, 12, 0)
    .output_poll_changed = drm_fb_helper_output_poll_changed,
#endif
#endif

#if DRM_VERSION_CODE >= KERNEL_VERSION(4, 8, 0)
    .atomic_check = drm_atomic_helper_check,
    .atomic_commit = drm_atomic_helper_commit,
    .atomic_state_alloc = zx_atomic_state_alloc,
    .atomic_state_clear = zx_atomic_state_clear,
    .atomic_state_free   = zx_atomic_state_free,
#endif
};

#if  DRM_VERSION_CODE >= KERNEL_VERSION(4, 8, 0)

#if  DRM_VERSION_CODE >= KERNEL_VERSION(4, 12, 0)
static const struct drm_mode_config_helper_funcs  zx_kms_mode_helper_funcs = {
#else
static struct drm_mode_config_helper_funcs  zx_kms_mode_helper_funcs = {
#endif
    .atomic_commit_tail = zx_atomic_helper_commit_tail,
};
#endif

static void disp_info_pre_init(disp_info_t*  disp_info)
{
    adapter_info_t*  adapter_info = disp_info->adp_info;
    unsigned long long scrn_base = 0;

    scrn_base = screen_info.lfb_base;
    
#ifdef VIDEO_CAPABILITY_64BIT_BASE
    if(screen_info.capabilities & VIDEO_CAPABILITY_64BIT_BASE)
    {
        scrn_base |= (unsigned long long)screen_info.ext_lfb_base << 32;
    }
#endif

    zx_info("Screen info base = 0x%lx.\n", scrn_base);

    disp_info->intr_lock = zx_create_spinlock();
    disp_info->hpd_lock = zx_create_spinlock();
    disp_info->hda_lock = zx_create_spinlock();
    disp_info->gamma_lock = zx_create_mutex();
}

static void disp_info_deinit(disp_info_t*  disp_info)
{
    adapter_info_t*  adapter_info = disp_info->adp_info;

    zx_destroy_spinlock(disp_info->intr_lock);
    disp_info->intr_lock = NULL;

    zx_destroy_spinlock(disp_info->hpd_lock);
    disp_info->hpd_lock = NULL;

    zx_destroy_spinlock(disp_info->hda_lock);
    disp_info->hda_lock = NULL;

    zx_destroy_mutex(disp_info->gamma_lock);
    disp_info->gamma_lock = NULL;
}

void disp_irq_init(disp_info_t* disp_info)
{
    zx_card_t*  zx_card = disp_info->zx_card;
    struct drm_device*  drm = zx_card->drm_dev;

    INIT_DELAYED_WORK(&disp_info->hpd_irq_work, zx_hpd_irq_work_func);
    INIT_WORK(&disp_info->hda_work, zx_hda_work_func);
    disp_info->irq_chip_func = &irq_chip_funcs;

    drm->vblank_disable_immediate = true;

    drm->max_vblank_count = 0xFFFF;

#if DRM_VERSION_CODE < KERNEL_VERSION(5, 7, 0)
    drm->driver->get_vblank_counter = zx_get_vblank_counter;
    drm->driver->enable_vblank   = zx_enable_vblank;
    drm->driver->disable_vblank  = zx_disable_vblank;
#endif

#if DRM_VERSION_CODE < KERNEL_VERSION(4, 13, 0)
    drm->driver->get_vblank_timestamp = zx_get_vblank_timestamp;
#if DRM_VERSION_CODE < KERNEL_VERSION(4, 4, 0)
    drm->driver->get_scanout_position = zx_legacy_get_crtc_scanoutpos;
#else
    drm->driver->get_scanout_position = zx_get_crtc_scanoutpos;
#endif
#elif DRM_VERSION_CODE < KERNEL_VERSION(5, 7, 0)
    drm->driver->get_vblank_timestamp = drm_calc_vbltimestamp_from_scanoutpos;
    drm->driver->get_scanout_position = zx_get_crtc_scanoutpos_kernel_4_10;
#endif

    if(zx_card->support_msi && !zx_card->pdev->msi_enabled)
    {
        pci_enable_msi(zx_card->pdev);
    }
}

void  disp_irq_deinit(disp_info_t* disp_info)
{
    zx_card_t*  zx_card = disp_info->zx_card;

    cancel_delayed_work_sync(&disp_info->hpd_irq_work);
    cancel_work_sync(&disp_info->hda_work);

    if(zx_card->pdev->msi_enabled)
    {
        pci_disable_msi(zx_card->pdev);
    }
}

static int disp_crtc_init(disp_info_t *disp_info, unsigned int index)
{
    zx_plane_t *zx_plane[ZX_PLANE_NUM] = {NULL};
    zx_plane_t *zx_cursor = NULL;
    zx_crtc_t *zx_crtc = NULL;
    struct drm_plane *primary_plane = NULL, *cursor_plane = NULL;
    int type = 0, ret = 0;

#if  DRM_VERSION_CODE >= KERNEL_VERSION(4, 8, 0)
    for (type = 0; type < disp_info->num_plane[index]; type++)
    {
        zx_plane[type] = zx_drm_plane_create(disp_info, index, type);
        if (IS_ERR(zx_plane[type]))
        {
            ret = PTR_ERR(zx_plane[type]);
            zx_plane[type] = NULL;
            DRM_ERROR("failed to create plane\n");
            goto failed;
        }
    }

    zx_cursor = zx_drm_plane_create(disp_info, index, ZX_PLANE_CURSOR);
    if (IS_ERR(zx_cursor))
    {
        ret = PTR_ERR(zx_cursor);
        zx_cursor = NULL;
        DRM_ERROR("failed to create cursor\n");
        goto failed;
    }

    primary_plane = &zx_plane[ZX_PLANE_PS]->base_plane;
    cursor_plane = &zx_cursor->base_plane;
#else
    //in legacy kms, plane is stand for overlay exclude primary stream and cursor
    for(type = ZX_PLANE_SS; type < disp_info->num_plane[index]; type++)
    {
        zx_plane[type] = zx_drm_plane_create(disp_info, index, type);
        if (IS_ERR(zx_plane[type]))
        {
            ret = PTR_ERR(zx_plane[type]);
            zx_plane[type] = NULL;
            DRM_ERROR("failed to create plane\n");
            goto failed;
        }
    }

#endif

    zx_crtc = zx_drm_crtc_create(disp_info, primary_plane, cursor_plane, index);

    if (IS_ERR(zx_crtc))
    {
        ret = PTR_ERR(zx_crtc);
        DRM_ERROR("failed to create zx crtc\n");
        goto failed;
    }

#if  DRM_VERSION_CODE >= KERNEL_VERSION(4, 8, 0)
    DRM_DEBUG_KMS("crtc=%d,name=%s\n", zx_crtc->base_crtc.index, zx_crtc->base_crtc.name);
#endif

    return ret;

failed:
    for (type = 0; type < disp_info->num_plane[index]; type++)
    {
        if (zx_plane[type])
        {
            zx_plane_destroy(&zx_plane[type]->base_plane);
            zx_plane[type] = NULL;
        }
    }

    if (zx_cursor)
    {
        zx_plane_destroy(&zx_cursor->base_plane);
        zx_cursor = NULL;
    }

    return ret;
}

static int disp_output_init(disp_info_t* disp_info)
{
    unsigned int  support_output = disp_info->support_output;
    struct drm_connector* connector = NULL;
    struct drm_encoder* encoder = NULL;
    int  output, CRTMask, DP3Mask;
    int  ret = 0;

    while (support_output)
    {
        output = GET_LAST_BIT(support_output);
        encoder = zx_drm_encoder_create(disp_info, output);
        connector = zx_drm_connector_create(disp_info, output);
        if (connector && encoder)
        {
            zx_drm_connector_attach_encoder(connector, encoder);

#if DRM_VERSION_CODE < KERNEL_VERSION(4, 8, 0)
            drm_connector_register(connector);
#endif
        }
        else
        {
            ret = -ENOMEM;
        }

        support_output &= (~output);
    }

    disp_info->iga3_conflict = 0;
    CRTMask = disp_cbios_get_crtc_mask(disp_info, DISP_OUTPUT_CRT);
    DP3Mask = disp_cbios_get_crtc_mask(disp_info, DISP_OUTPUT_DP3);
    if(CRTMask && CRTMask == DP3Mask)
    {
        disp_info->iga3_conflict = 1;
    }

    return ret;
}

static void  disp_hotplug_init(disp_info_t* disp_info)
{
    zx_card_t*  zx_card = disp_info->zx_card;
    struct drm_device*  drm = zx_card->drm_dev;

    //at boot/resume stage, no hpd event for all output, we need poll the hpd outputs once
    drm_helper_hpd_irq_event(drm);

    //enable hot plug interrupt
    zx_hot_plug_intr_onoff(disp_info, 1);
}

static void  disp_polling_init(disp_info_t* disp_info)
{
    zx_card_t*  zx_card = disp_info->zx_card;
    struct drm_device*  drm = zx_card->drm_dev;

    INIT_DELAYED_WORK(&drm->mode_config.output_poll_work, zx_output_poll_work_func);
    drm->mode_config.poll_enabled = 1; //it means driver can support poll, must enable it as framework also check its value

    disp_info->poll_status = 0;

    zx_poll_enable(disp_info);
}

int disp_get_pipe_from_crtc(zx_file_t *priv, zx_kms_get_pipe_from_crtc_t *get)
{
    zx_card_t *zx = priv->card;
    struct drm_crtc *drmmode_crtc = NULL;

    drmmode_crtc = zx_drm_crtc_find(zx->drm_dev, (struct drm_file*)priv->parent_file, get->crtc_id);

    if (!drmmode_crtc)
        return -ENOENT;

    get->pipe = to_zx_crtc(drmmode_crtc)->pipe;

    return 0;
}

#if DRM_VERSION_CODE < KERNEL_VERSION(4, 8, 0)

void  zx_disp_suspend_helper(struct drm_device *dev)
{
    struct drm_crtc *crtc = NULL;
    struct drm_encoder *encoder = NULL;
    struct drm_connector* connector = NULL;
    const struct drm_connector_funcs *conn_funcs;
    const struct drm_encoder_helper_funcs *encoder_funcs;
    const struct drm_crtc_helper_funcs *crtc_funcs;
    bool  enc_in_use, has_valid_encoder;

    zx_drm_for_each_crtc(crtc, dev)
    {
        has_valid_encoder = false;

        list_for_each_entry(encoder, &dev->mode_config.encoder_list, head)
        {
            enc_in_use = false;

            if(encoder->crtc != crtc)
            {
                continue;
            }

            list_for_each_entry(connector, &dev->mode_config.connector_list, head)
            {
                if(connector->encoder != encoder)
                {
                    continue;
                }

                enc_in_use = true;
                has_valid_encoder = true;

                conn_funcs = connector->funcs;
                if(conn_funcs->dpms)
                {
                    (*conn_funcs->dpms)(connector, DRM_MODE_DPMS_OFF);
                }
            }

            if(enc_in_use)
            {
                encoder_funcs = encoder->helper_private;
                if(encoder_funcs->disable)
                {
                    (*encoder_funcs->disable)(encoder);
                }
                else if(encoder_funcs->dpms)
                {
                    (*encoder_funcs->dpms)(encoder, DRM_MODE_DPMS_OFF);
                }
            }
        }

        if(has_valid_encoder)
        {
            crtc_funcs = crtc->helper_private;
            if(crtc_funcs->disable)
            {
                (*crtc_funcs->disable)(crtc);
            }
            else if(crtc_funcs->dpms)
            {
                (*crtc_funcs->dpms)(crtc, DRM_MODE_DPMS_OFF);
            }
        }
    }
}

#endif

int disp_suspend(struct drm_device *dev)
{
    zx_card_t  *zx = dev->dev_private;
    disp_info_t *disp_info = (disp_info_t *)zx->disp_info;
    int ret = 0;
#if DRM_VERSION_CODE >= KERNEL_VERSION(4, 8, 0)
    struct drm_atomic_state *state;
#endif

    zx_hot_plug_intr_onoff(disp_info, 0);

    zx_poll_disable(disp_info);

#if DRM_VERSION_CODE >= KERNEL_VERSION(4, 8, 0)
    state = drm_atomic_helper_suspend(dev);
    ret = PTR_ERR_OR_ZERO(state);

    if (ret)
    {
        DRM_ERROR("Suspending crtc's failed with %i\n", ret);
    }
    else
    {
        disp_info->modeset_restore_state = state;
    }
#else
    zx_disp_suspend_helper(dev);
#endif

    cancel_delayed_work_sync(&disp_info->hpd_irq_work);
    cancel_work_sync(&disp_info->hda_work);

    return ret;
}

#if ENABLE_RUNTIME_PM
int disp_runtime_suspend(struct drm_device *dev)
{
    zx_card_t  *zx = dev->dev_private;
    disp_info_t *disp_info = (disp_info_t *)zx->disp_info;
    struct drm_connector *connector;
    int ret = 0;

    zx_poll_disable(disp_info);

    /* turn off display hw (crtc,encoder,connector)*/
    list_for_each_entry(connector, &dev->mode_config.connector_list, head) {
        drm_helper_connector_dpms(connector, DRM_MODE_DPMS_OFF);
    }

    cancel_delayed_work_sync(&disp_info->hpd_irq_work);
    cancel_work_sync(&disp_info->hda_work);

    return ret;
}
#endif

#if DRM_VERSION_CODE < KERNEL_VERSION(4, 8, 0)

void disp_vblank_save(struct drm_device* dev)
{
    struct drm_crtc* crtc;
    zx_drm_for_each_crtc(crtc, dev)
    {
        drm_crtc_vblank_off(crtc);
    }
}

void disp_vblank_restore(struct drm_device* dev)
{
    struct drm_crtc* crtc;
    zx_drm_for_each_crtc(crtc, dev)
    {
        drm_crtc_vblank_on(crtc);
    }
}

#endif

void disp_pre_resume(struct drm_device *dev)
{
    zx_card_t  *zx = dev->dev_private;
    disp_info_t *disp_info = (disp_info_t *)zx->disp_info;
    adapter_info_t*  adapter_info = disp_info->adp_info;

    disp_cbios_init_hw(disp_info);
}

void disp_post_resume(struct drm_device *dev)
{
    zx_card_t  *zx = dev->dev_private;
    disp_info_t *disp_info = (disp_info_t *)zx->disp_info;
#if DRM_VERSION_CODE >= KERNEL_VERSION(4, 8, 0)
    struct drm_atomic_state *state = disp_info->modeset_restore_state;
    struct drm_crtc_state *crtc_state;
    struct drm_plane_state *plane_state;
    struct drm_modeset_acquire_ctx ctx;
#endif
    struct drm_crtc *crtc;
    struct drm_plane *plane;
    int i, ret;

#if DRM_VERSION_CODE >= KERNEL_VERSION(4, 8, 0)
    disp_info->modeset_restore_state = NULL;
    if (state)
    {
        state->acquire_ctx = &ctx;
    }
#endif

    drm_mode_config_reset(dev);

#if DRM_VERSION_CODE >= KERNEL_VERSION(4, 8, 0)
    mutex_lock(&dev->mode_config.mutex);
    drm_modeset_acquire_init(&ctx, 0);

    while (1)
    {
        ret = drm_modeset_lock_all_ctx(dev, &ctx);
        if (ret != -EDEADLK)
            break;
        drm_modeset_backoff(&ctx);
    }

    if (!ret && state)
    {
        if (zx->flags & ZX_S4_RESUME)
        {
            zx_drm_for_each_plane_in_state(state, plane, plane_state, i)
            {
                if (plane_state->crtc != NULL && plane_state->fb != NULL)
                {
                    to_zx_plane_state(plane_state)->resume_from_s4 = 1;
                }
            }

            zx->flags &= ~ZX_S4_RESUME;
        }

#if DRM_VERSION_CODE >= KERNEL_VERSION(4, 12, 0)
        ret = drm_atomic_helper_commit_duplicated_state(state, &ctx);
#else
        ret = drm_atomic_commit(state);
#endif
    }
    drm_modeset_drop_locks(&ctx);
    drm_modeset_acquire_fini(&ctx);
    mutex_unlock(&dev->mode_config.mutex);

    if (ret)
    {
        DRM_ERROR("Restoring old state failed with %i\n", ret);
    }

#if DRM_VERSION_CODE < KERNEL_VERSION(4, 10, 0)
    if (ret)
    {
        drm_atomic_state_free(state);
    }
#else
    if (state)
    {
        drm_atomic_state_put(state);
    }
#endif

#endif

#if DRM_VERSION_CODE < KERNEL_VERSION(4, 8, 0)
    drm_helper_resume_force_mode(dev);
#endif

    zx_detect_and_update_connectors(disp_info, disp_info->supp_hpd_outputs, 0, 1);

    zx_poll_enable(disp_info);

    zx_hot_plug_intr_onoff(disp_info, 1);
}

#if ENABLE_RUNTIME_PM
void disp_post_runtime_resume(struct drm_device *dev)
{
    zx_card_t  *zx = dev->dev_private;
    disp_info_t *disp_info = (disp_info_t *)zx->disp_info;

    mutex_unlock(&dev->mode_config.mutex);
    drm_modeset_unlock(&dev->mode_config.connection_mutex);

    zx_poll_enable(disp_info);

/*
* Most of the connector probing functions try to acquire runtime pm
* refs to ensure that the GPU is powered on when connector polling is
* performed. Since we're calling this from a runtime PM callback,
* trying to acquire rpm refs will cause us to deadlock.
*
* Since we're guaranteed to be holding the rpm lock, it's safe to
* temporarily disable the rpm helpers so this doesn't deadlock us.
*/
#ifdef CONFIG_PM
    dev->dev->power.disable_depth++;
#endif
    drm_helper_hpd_irq_event(dev);

#ifdef CONFIG_PM
    dev->dev->power.disable_depth--;
#endif
    zx_info("drm_helper_hpd_irq_event successfully.\n");

}
#endif

static int disp_mode_config_init(disp_info_t* disp_info)
{
    zx_card_t*  zx_card = disp_info->zx_card;
    adapter_info_t* adapter_info = &zx_card->adapter_info;
    struct drm_device*  drm = zx_card->drm_dev;

    drm_mode_config_init(drm);

    drm->mode_config.min_width = 0;
    drm->mode_config.min_height = 0;

    drm->mode_config.max_width = 3840*4;   // 4*4k
    drm->mode_config.max_height = 2160*4;
    drm->mode_config.cursor_width = 128;
    drm->mode_config.cursor_height = 128;

    drm->mode_config.preferred_depth = 24;
    drm->mode_config.prefer_shadow = 1;

#if  DRM_VERSION_CODE < KERNEL_VERSION(5, 14, 0)
    drm->mode_config.allow_fb_modifiers = TRUE;
#endif

#if  DRM_VERSION_CODE < KERNEL_VERSION(6, 2, 0)
    drm->mode_config.fb_base = adapter_info->fb_bus_addr;
#endif

    drm->mode_config.funcs = &zx_kms_mode_funcs;

    drm->mode_config.async_page_flip = FALSE;

#if  DRM_VERSION_CODE >= KERNEL_VERSION(4, 8, 0)
    drm->mode_config.helper_private = &zx_kms_mode_helper_funcs;
#endif

    return 0;
}

static void  disp_turn_off_crtc_output(disp_info_t* disp_info)
{
    unsigned int devices[MAX_CORE_CRTCS] = {0};
    unsigned int index, detect_devices = 0, output = 0, flags = 0;
    int changed = 0;
    zx_card_t*  zx_card = disp_info->zx_card;
    struct drm_device*    drm = zx_card->drm_dev;
    struct drm_connector* connector = NULL;

    zx_info("To turn off igas and devices.\n");

    disp_cbios_sync_vbios(disp_info);
    disp_cbios_get_active_devices(disp_info, devices);
    for(index = 0; index < MAX_CORE_CRTCS; index++)
    {
        if (!devices[index])
        {
            continue;
        }
        disp_cbios_detect_output_status(disp_info, devices[index], 0, &changed);
        detect_devices |= devices[index];
    }

    while(detect_devices)
    {
        flags = 0;
        output = GET_LAST_BIT(detect_devices);
        list_for_each_entry(connector, &drm->mode_config.connector_list, head)
        {
            if(to_zx_connector(connector)->output_type == output)
            {
                break;
            }
        }
        if(connector->connector_type == DRM_MODE_CONNECTOR_eDP)
        {
            flags |= SKIP_VDD_OFF;
        }
        disp_cbios_set_dpms(disp_info, output, 0, flags);
        detect_devices &= (~output);
    }

    for(index = 0; index < disp_info->num_crtc; index++)
    {
        disp_cbios_turn_onoff_screen(disp_info, index, 0);
        disp_cbios_turn_onoff_iga(disp_info, index, 0);
    }
}

static void disp_info_print(disp_info_t* disp_info)
{
    adapter_info_t* adapter_info = disp_info->adp_info;
    unsigned char* pmpversion = disp_info->pmp_version;
    unsigned int value = 0;
    int  vbiosVer = 0;
    int  pmpdatelen = 0;
    int  pmptimelen = 0;

    vbiosVer = disp_info->vbios_version;
    if(vbiosVer && (vbiosVer != 0xffffffff))
    {
        zx_info("displayinfo Vbios Version:%02x.%02x.%02x.%02x\n", (vbiosVer>>24)&0xff,(vbiosVer>>16)&0xff,(vbiosVer>>8)&0xff,vbiosVer&0xff);
    }

    if(*pmpversion)
    {
        // pmpVersion:str1 -> pmp version,str2 -> pmp build date,str3 -> pmp build time
        pmpdatelen = zx_strlen(pmpversion) + 1;
        pmptimelen = zx_strlen(pmpversion + pmpdatelen) + 1;
        zx_info("displayinfo PMP Version:%s Build Time:%s %s\n",pmpversion,(pmpversion + pmpdatelen),(pmpversion  + pmpdatelen + pmptimelen) );
    }

    zx_info("displayinfo Driver Version:%02x.%02x.%02x%s\n",DRIVER_MAJOR,DRIVER_MINOR,DRIVER_PATCHLEVEL,DRIVER_CLASS);
    zx_info("displayinfo Driver Release Date:%s\n",DRIVER_DATE);
    zx_info("displayinfo FB Size:%d M\n",adapter_info->total_mem_size_mb);
    //zx_info("displayinfo Chip Slice Mask:0x%x\n",adapter_info->chip_slice_mask);
    //zx_info("displayinfo MIU channel num:%d\n",adapter_info->chan_num);

     //becuse the Unit from cbios is KHZ,so divide 1000 to MHZ
    if(DISP_OK == disp_cbios_get_clock(disp_info, ZX_QUERY_ENGINE_CLOCK, &value))
    {
        zx_info("displayinfo Eclk:%dMHz\n", (value + 500)/1000);
    }
    if(DISP_OK == disp_cbios_get_clock(disp_info, ZX_QUERY_VCLK, &value))
    {
        zx_info("displayinfo Vclk:%dMHz\n", (value + 500)/1000);
    }
    if(DISP_OK == disp_cbios_get_clock(disp_info, ZX_QUERY_MCLK, &value))
    {
        //zx_info("displayinfo Mclk:%dMHz\n", (value + 500)/1000);
    }
}

int  disp_init_psr(disp_info_t* disp_info)
{
    unsigned int  ret = -1;

    psr_data_t *psr_data = NULL;
    psr_data = zx_calloc(sizeof(psr_data_t));
    if(!psr_data)
    {
        return ret;
    }

    disp_info->psr_data = psr_data;
    psr_data->disp_info = disp_info;

    psr_data->current_state = PSR_STATE_0_DISABLED;
    psr_data->enabled = 0;
    atomic_set(&psr_data->operation, 0);
    psr_data->active_fail_num = 0;
    psr_data->ref_count = 0;
    psr_data->output_type = 0;
    psr_data->psr_mutex = zx_create_mutex();
    psr_data->psr_ref_lock = zx_create_spinlock();
    psr_data->psr_event = zx_create_event(0);
    psr_data->psr_thread = zx_create_thread(zx_psr_work_thread, psr_data, "psr_work");

#if DRM_VERSION_CODE >= KERNEL_VERSION(4, 15, 0)
    timer_setup(&psr_data->psr_timer, zx_psr_timer, TIMER_IRQSAFE);
#else
    __setup_timer(&psr_data->psr_timer, zx_psr_timer, (unsigned long)psr_data, TIMER_IRQSAFE);
#endif

    return 0;
}

void disp_set_fast_detect(disp_info_t  *disp_info)
{
    if(disp_info->adp_info && 
        disp_info->adp_info->sub_sys_id == 0x2001 && 
        disp_info->adp_info->sub_sys_vendor_id == 0x3A05)
    {
        zx_info("Enable fast detect for OS polling connector.\n");
        disp_info->worker_detect = 1;
    }
    else
    {
        disp_info->worker_detect = 0;
    }

    if(disp_info->worker_detect)
    {
        //if use delayed worker in os detect, the first detect result may be incorrect, so detect all outputs at init
        zx_detect_and_update_connectors(disp_info, disp_info->support_output, 0, 0);
    }
}

int  zx_init_modeset(struct drm_device *dev)
{
    zx_card_t*  zx_card = dev->dev_private;
    adapter_info_t* adapter_info = &zx_card->adapter_info;
    disp_info_t*  disp_info = NULL;
    unsigned int  index = 0, ret = -1;

    //drm_debug = 0xffffffff;

    disp_info = zx_calloc(sizeof(disp_info_t));

    if(!disp_info)
    {
        return  ret;
    }

    zx_card->disp_info = disp_info;
    disp_info->zx_card = zx_card;
    disp_info->adp_info = adapter_info;
    adapter_info->init_render = 1;

    zx_core_interface->get_adapter_info(zx_card->adapter, adapter_info);

    disp_info_pre_init(disp_info);

    disp_init_cbios(disp_info);

    disp_cbios_init_hw(disp_info);

    disp_cbios_query_vbeinfo(disp_info);

    disp_cbios_read_config_from_efuse(disp_info);

    zx_core_interface->update_adapter_info(zx_card->adapter, adapter_info);

    disp_cbios_get_crtc_resource(disp_info);

    disp_cbios_get_crtc_caps(disp_info);

    disp_irq_init(disp_info);

    if(disp_info->num_crtc)
    {
        ret = drm_vblank_init(dev, disp_info->num_crtc);
        if (ret)
        {
            goto err_vblk;
        }
    }

    disp_mode_config_init(disp_info);

    for(index = 0; index < disp_info->num_crtc; index++)
    {
        ret = disp_crtc_init(disp_info, index);
        if (ret)
        {
            goto err_crtc;
        }
    }

    disp_output_init(disp_info);

    disp_turn_off_crtc_output(disp_info);

    disp_hotplug_init(disp_info);

    disp_polling_init(disp_info);

    disp_capture_init(disp_info);

    disp_init_psr(disp_info);

    disp_set_fast_detect(disp_info);

    disp_info_print(disp_info);

    return  ret;

err_crtc:
    //drm_vblank_cleanup(dev);

    drm_mode_config_cleanup(dev);

err_vblk:
    disp_capture_deinit(disp_info);

    disp_irq_deinit(disp_info);

    disp_cbios_cleanup(disp_info);

    disp_info_deinit(disp_info);

    zx_free(disp_info);

    zx_card->disp_info = NULL;

    return  ret;
}

void disp_deinit_psr(disp_info_t* disp_info)
{
    psr_data_t* psr_data = disp_info->psr_data;

    zx_destroy_thread(psr_data->psr_thread);

#if DRM_VERSION_CODE >= KERNEL_VERSION(6, 15, 0)
    timer_delete_sync(&psr_data->psr_timer);
#else
    del_timer_sync(&psr_data->psr_timer);
#endif

    zx_destroy_mutex(psr_data->psr_mutex);
    psr_data->psr_mutex = NULL;

    zx_destroy_spinlock(psr_data->psr_ref_lock);
    psr_data->psr_ref_lock = NULL;

    zx_destroy_event(psr_data->psr_event);
    psr_data->psr_event = NULL;

    zx_free(psr_data);

    disp_info->psr_data = NULL;
}

void  zx_deinit_modeset(struct drm_device *dev)
{
    zx_card_t*  zx_card = dev->dev_private;
    disp_info_t*  disp_info = (disp_info_t*)zx_card->disp_info;

    if(!disp_info)
    {
        return;
    }

#if DRM_VERSION_CODE >= KERNEL_VERSION(4, 12, 0)
    drm_atomic_helper_shutdown(dev);
#endif

    disp_capture_deinit(disp_info);

    disp_irq_deinit(disp_info);

    drm_kms_helper_poll_fini(dev);

    drm_mode_config_cleanup(dev);

    disp_cbios_cleanup(disp_info);

    disp_deinit_psr(disp_info);

    disp_info_deinit(disp_info);

    zx_free(disp_info);

    zx_card->disp_info = NULL;
}

unsigned int zx_get_chip_temp(void *dispi)
{
    disp_info_t*  disp_info = (disp_info_t*)dispi;
    adapter_info_t*  adp_info = disp_info->adp_info;
    unsigned int temper;

    temper = zx_read32(adp_info->mmio + MMIO_OFFSET_GPU_TEMPERATURE);

    return temper;
}

int zx_debugfs_crtc_dump(struct seq_file* file, void *data)
{
    struct drm_device *dev = NULL;
    zx_crtc_t *zx_crtc = (zx_crtc_t *)data;
    struct drm_crtc *crtc = NULL;
    struct drm_display_mode *mode, *hwmode;
    struct drm_plane *plane = NULL;
    zx_plane_t *zx_plane= NULL;
    struct drm_zx_gem_object *obj = NULL;
    int enabled, h, v;

    if (!zx_crtc)
    {
        return 0;
    }

    crtc = &zx_crtc->base_crtc;
    dev = crtc->dev;

    enabled = zx_drm_crtc_in_use(crtc);
    if (!enabled)
    {
        seq_printf(file, "IGA status: disabled.\n");
        return 0;
    }

#if DRM_VERSION_CODE < KERNEL_VERSION(4, 8, 0)
    mode = &crtc->mode;
    hwmode = &crtc->hwmode;
#else
    mode = &crtc->state->mode;
    hwmode = &crtc->state->adjusted_mode;
#endif

    h = mode->hdisplay;
    v = mode->vdisplay;

    seq_printf(file, "IGA status: enabled.\n");
    seq_printf(file, "SW timing: active: %d x %d. total: %d x %d. clock: %dk\n",
         h, v, mode->htotal, mode->vtotal, mode->clock);
    seq_printf(file, "HW timing: active: %d x %d. total: %d x %d. clock: %dk\n",
        hwmode->hdisplay, hwmode->vdisplay, hwmode->htotal, hwmode->vtotal, hwmode->clock);

#if  DRM_VERSION_CODE < KERNEL_VERSION(4, 8, 0)
    //primary
#if  DRM_VERSION_CODE >= KERNEL_VERSION(3, 15, 0)
    obj = (crtc->primary->fb)? to_zxfb(crtc->primary->fb)->obj : NULL;
#else
    obj = (crtc->fb)? to_zxfb(crtc->fb)->obj : NULL;
#endif
    seq_printf(file, "IGA%d-PS: src window: [%d, %d, %d, %d], dst window: [0, 0, %d, %d], handle: 0x%x, gpu vt addr: 0x%llx.\n",
                      (zx_crtc->pipe+1), crtc->x, crtc->y, crtc->x+h, crtc->y + v, h, v, obj->info.allocation, obj->info.gpu_virt_addr);
    //overlay
    zx_drm_for_each_plane(plane, dev)
    {
        zx_plane = to_zx_plane(plane);
        if (zx_plane->crtc_index != zx_crtc->pipe)
        {
            continue;
        }

        if (zx_plane->plane_type == ZX_PLANE_PS || zx_plane->plane_type == ZX_PLANE_CURSOR)
        {
            continue;
        }

        if (!plane->crtc || !plane->fb)
        {
            seq_printf(file, "IGA%d-%s: disabled.\n", (zx_crtc->pipe+1), plane_name[zx_plane->plane_type]);
        }
        else
        {
            int src_x = zx_plane->src_pos & 0xFFFF;
            int src_y = (zx_plane->src_pos >> 16) & 0xFFFF;
            int src_w = zx_plane->src_size & 0xFFFF;
            int src_h = (zx_plane->src_size >> 16) & 0xFFFF;
            int dst_x = zx_plane->dst_pos & 0xFFFF;
            int dst_y = (zx_plane->dst_pos >> 16) & 0xFFFF;
            int dst_w = zx_plane->dst_size & 0xFFFF;
            int dst_h = (zx_plane->dst_size >> 16) & 0xFFFF;
            obj = to_zxfb(plane->fb)->obj;
            seq_printf(file, "IGA%d-%s: src window: [%d, %d, %d, %d], dst window: [%d, %d, %d, %d], handle: 0x%x, gpu vt addr: 0x%llx.\n",
                      (zx_crtc->pipe+1), plane_name[zx_plane->plane_type], src_x, src_y, src_x + src_w, src_y + src_h,
                      dst_x, dst_y, dst_x + dst_w, dst_y + dst_h, obj->info.allocation, obj->info.gpu_virt_addr);
        }
    }
    //cursor
    if (!zx_crtc->cursor_bo)
    {
        seq_printf(file, "IGA%d-cursor: disabled.\n", (zx_crtc->pipe+1));
    }
    else
    {
        obj = zx_crtc->cursor_bo;
        seq_printf(file, "IGA%d-cursor: src window: [%d, %d, %d, %d], dst window: [%d, %d, %d, %d], handle: 0x%x, gpu vt addr: 0x%llx.\n",
                    (zx_crtc->pipe+1), 0, 0, zx_crtc->cursor_w, zx_crtc->cursor_h, zx_crtc->cursor_x, zx_crtc->cursor_y,
                    zx_crtc->cursor_x + zx_crtc->cursor_w, zx_crtc->cursor_y + zx_crtc->cursor_h, obj->info.allocation, obj->info.gpu_virt_addr);
    }
#else
    zx_drm_for_each_plane(plane, dev)
    {
        zx_plane = to_zx_plane(plane);
        if (zx_plane->crtc_index != zx_crtc->pipe)
        {
            continue;
        }

        if (!zx_plane->base_plane.state->crtc || ! zx_plane->base_plane.state->fb)
        {
            seq_printf(file, "%s: disabled.\n", zx_plane->base_plane.name);
        }
        else
        {
            struct drm_zx_framebuffer *zxfb = to_zxfb(zx_plane->base_plane.state->fb);
            int src_x = zx_plane->base_plane.state->src_x >> 16;
            int src_y = zx_plane->base_plane.state->src_y >> 16;
            int src_w = zx_plane->base_plane.state->src_w >> 16;
            int src_h = zx_plane->base_plane.state->src_h >> 16;
            int dst_x = zx_plane->base_plane.state->crtc_x;
            int dst_y = zx_plane->base_plane.state->crtc_y;
            int dst_w = zx_plane->base_plane.state->crtc_w;
            int dst_h = zx_plane->base_plane.state->crtc_h;
            obj = to_zxfb(zx_plane->base_plane.state->fb)->obj;
            seq_printf(file, "%s: src window: [%d, %d, %d, %d], dst window: [%d, %d, %d, %d], handle: 0x%x, gpu vt addr: 0x%llx.\n",
                      zx_plane->base_plane.name, src_x, src_y, src_x + src_w, src_y + src_h,
                      dst_x, dst_y, dst_x + dst_w, dst_y + dst_h, obj->gid, zxfb->gpu_virt_addr);
        }
    }
#endif

    return 0;
}

int zx_debugfs_clock_dump(struct seq_file* file, struct drm_device* dev)
{
    zx_card_t*  zx_card = dev->dev_private;
    disp_info_t*  disp_info = (disp_info_t*)zx_card->disp_info;
    unsigned int value = 0;

    if(DISP_OK == disp_cbios_get_clock(disp_info, ZX_QUERY_ENGINE_CLOCK, &value))
    {
        seq_printf(file, "Engine clock = %dMHz.\n", value/10000);
    }

    return 0;
}


int zx_debugfs_displayinfo_dump(struct seq_file* file, struct drm_device* dev)
{
    zx_card_t*  zx_card = dev->dev_private;
    disp_info_t*  disp_info = (disp_info_t*)zx_card->disp_info;
    adapter_info_t* adapter_info = &zx_card->adapter_info;
    unsigned char* pmpversion = disp_info->pmp_version;
    unsigned int value = 0;
    int  vbiosVer = 0;
    int  pmpdatelen = 0;
    int  pmptimelen = 0;

    vbiosVer = disp_info->vbios_version;
    if(vbiosVer && (vbiosVer != 0xffffffff))
    {
        seq_printf(file, "Vbios Version:%02x.%02x.%02x.%02x\n", (vbiosVer>>24)&0xff,(vbiosVer>>16)&0xff,(vbiosVer>>8)&0xff,vbiosVer&0xff);
    }

    if(*pmpversion)
    {
        // pmpVersion:str1 -> pmp version,str2 -> pmp build date,str3 -> pmp build time
        pmpdatelen = zx_strlen(pmpversion) + 1;
        pmptimelen = zx_strlen(pmpversion + pmpdatelen) + 1;
        seq_printf(file, "PMP Version:%s Build Time:%s %s\n",pmpversion,(pmpversion + pmpdatelen),(pmpversion  + pmpdatelen + pmptimelen) );
    }

    seq_printf(file,"Driver Version:%02x.%02x.%02x%s\n",DRIVER_MAJOR,DRIVER_MINOR,DRIVER_PATCHLEVEL,DRIVER_CLASS);
    seq_printf(file,"Driver Release Date:%s\n",DRIVER_DATE);
    seq_printf(file,"FB Size:%d M\n",adapter_info->total_mem_size_mb);
    //seq_printf(file,"Chip Slice Mask:0x%x\n",adapter_info->chip_slice_mask);
    //seq_printf(file,"MIU channel num:%d\n",adapter_info->chan_num);

     //becuse the Unit from cbios is KHZ,so divide 1000 to MHZ
    if(DISP_OK == disp_cbios_get_clock(disp_info, ZX_QUERY_ENGINE_CLOCK, &value))
    {
        seq_printf(file, "Eclk:%dMHz\n", (value + 500)/1000);
        seq_printf(file, "Eclk_2X:%dMHz\n", (value * 2 + 500)/1000);
    }
    if(DISP_OK == disp_cbios_get_clock(disp_info, ZX_QUERY_VCP0_CLOCK, &value))
    {
        //seq_printf(file, "VCP0 clk:%dMHz\n", (value + 500)/1000);
    }
    if(DISP_OK == disp_cbios_get_clock(disp_info, ZX_QUERY_VCP1_CLOCK, &value))
    {
        //seq_printf(file, "VCP1 clk:%dMHz\n", (value + 500)/1000);
    }
    if(DISP_OK == disp_cbios_get_clock(disp_info, ZX_QUERY_VCLK, &value))
    {
        //seq_printf(file, "VPP  clk:%dMHz\n", (value + 500)/1000);
    }
    if(DISP_OK == disp_cbios_get_clock(disp_info, ZX_QUERY_CMU_CLOCK, &value))
    {
        //seq_printf(file, "CMU  clk:%dMHz\n", (value + 500)/1000);
    }
    if(DISP_OK == disp_cbios_get_clock(disp_info, ZX_QUERY_MCLK, &value))
    {
        //seq_printf(file, "Mclk:%dMHz\n", (value + 500)/1000);
    }

    return 0;
}

int zx_debugfs_psr_dump(struct seq_file* file, struct drm_device* dev)
{
    zx_card_t*  zx_card = dev->dev_private;
    disp_info_t*  disp_info = (disp_info_t*)zx_card->disp_info;
    struct drm_connector *connector;
    int support = 0;
    ZX_DEFINE_DRM_CONN_ITER(conn_iter);

    zx_drm_connector_list_iter_begin(dev, &conn_iter);

    zx_drm_for_each_connector(connector, dev, &conn_iter)
    {
        if((to_zx_connector(connector))->support_psr)
        {
            support = 1;
            break;
        }
    }

    zx_drm_connector_list_iter_end(&conn_iter);

    seq_printf(file, "PSR: supprot=%s \n", support? "Yes" : "Not");

    if(disp_info->psr_data)
    {
        seq_printf(file, "PSR: enable=%d, curr_state=%d, ref_cnt=0x%x.\n", disp_info->psr_data->enabled, disp_info->psr_data->current_state, disp_info->psr_data->ref_count);
    }
    return 0;
}

int zx_debugfs_psr_onoff(struct drm_device* dev, int enable)
{
    zx_card_t*  zx_card = dev->dev_private;
    disp_info_t*  disp_info = (disp_info_t*)zx_card->disp_info;

    if (disp_info->psr_data && !enable)
    {
        disp_info->psr_data->forceoff = 1;
        if (disp_info->psr_data->enabled)
        {
            psr_disable(disp_info->psr_data);
        }
    }
    else if (disp_info->psr_data && enable)
    {
        disp_info->psr_data->forceoff = 0;
    }
    return 0;
}


int zx_debugfs_connector_dump(struct seq_file* file, void *data)
{
    zx_connector_t *zx_connector = (zx_connector_t *)data;

    seq_printf(file, "Output_type: 0x%x \n", zx_connector->output_type);

    if (zx_connector->base_connector.status == connector_status_connected)
    {
        seq_printf(file, "Status: Connected \n");
        seq_printf(file, "Audio: %s \n", zx_connector->support_audio ? "Yes" :"No");
    }
    else
    {
        seq_printf(file, "Status: Disconnected \n");
    }

    return 0;
}
