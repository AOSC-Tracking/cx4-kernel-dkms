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

#include "zx_atomic.h"
#include "zx_sink.h"
#include "zx_drm_helper.h"

#if DRM_VERSION_CODE >= KERNEL_VERSION(4, 8, 0)

struct drm_atomic_state* zx_atomic_state_alloc(struct drm_device *dev)
{
    zx_ato_state_t *state = zx_calloc(sizeof(zx_ato_state_t));

    if (!state || drm_atomic_state_init(dev, &state->base_ato_state) < 0)
    {
        zx_free(state);
        return NULL;
    }

    return &state->base_ato_state;
}

void zx_atomic_state_clear(struct drm_atomic_state *s)
{
    zx_ato_state_t *state = to_zx_atomic_state(s);
    drm_atomic_state_default_clear(s);
}

void zx_atomic_state_free(struct drm_atomic_state *s)
{
    zx_ato_state_t *state = to_zx_atomic_state(s);
    drm_atomic_state_default_release(s);
    zx_free(state);
}

static void zx_update_crtc_sink(struct drm_atomic_state *old_state)
{
    struct drm_crtc *crtc;
    struct drm_crtc_state *old_crtc_state, *new_crtc_state;
    zx_crtc_state_t *new_zx_crtc_state;
    struct drm_connector_state *new_conn_state, *old_conn_state;
    struct drm_connector *connector;
    zx_connector_t *zx_connector;
    int i, j;

    zx_drm_for_each_crtc_in_state(old_state, crtc, old_crtc_state, i)
    {
        new_crtc_state = crtc->state;

        new_zx_crtc_state = to_zx_crtc_state(new_crtc_state);

        zx_connector = NULL;

        if (new_crtc_state->active && drm_atomic_crtc_needs_modeset(new_crtc_state))
        {
            //find the first crtc matching connector
            zx_drm_for_each_connector_in_state(old_state, connector, old_conn_state, j)
            {
                new_conn_state = connector->state;

                if (new_conn_state->crtc == crtc)
                {
                    zx_connector = to_zx_connector(connector);
                    break;
                }
            }


            if (zx_connector && new_zx_crtc_state->sink != zx_connector->sink)
            {
                zx_sink_put(new_zx_crtc_state->sink);

                new_zx_crtc_state->sink = zx_connector->sink;
                zx_sink_get(new_zx_crtc_state->sink);
            }
        }

        if (old_crtc_state->active &&
            drm_atomic_crtc_needs_modeset(new_crtc_state))
        {
            if (!new_crtc_state->active)
            {
                zx_sink_put(new_zx_crtc_state->sink);

                new_zx_crtc_state->sink = NULL;
            }
        }

    }
}

void zx_atomic_helper_commit_tail(struct drm_atomic_state *old_state)
{
    struct drm_device *dev = old_state->dev;
    zx_card_t*  zx_card = dev->dev_private;
    disp_info_t*  disp_info = (disp_info_t*)zx_card->disp_info;
    psr_data_t* psr_data = disp_info->psr_data;

#if DRM_VERSION_CODE >= KERNEL_VERSION(4, 9, 0)
    uint32_t flags = DRM_PLANE_COMMIT_NO_DISABLE_AFTER_MODESET;
#else
    bool flags = false;
#endif
    int acquired = 0;

#if ENABLE_RUNTIME_PM
   int ret;
   bool active = false;
   struct drm_crtc *crtc;
    ret = pm_runtime_get_sync(dev->dev);
    if (ret < 0)
    {
        pm_runtime_mark_last_busy(dev->dev);
        pm_runtime_put_autosuspend(dev->dev);
        return;
    }
#endif

    acquired = psr_acquire_display(psr_data, PSR_FLIP_REF, 1);

    drm_atomic_helper_commit_modeset_disables(dev, old_state);

    drm_atomic_helper_commit_modeset_enables(dev, old_state);

    zx_update_crtc_sink(old_state);

    drm_atomic_helper_commit_planes(dev, old_state, flags);

    drm_atomic_helper_commit_hw_done(old_state);

#if DRM_VERSION_CODE >= KERNEL_VERSION(4, 19, 0)
    drm_atomic_helper_fake_vblank(old_state);
#endif

#if DRM_VERSION_CODE >= KERNEL_VERSION(4, 14, 0)
    drm_atomic_helper_wait_for_flip_done(dev, old_state);
#else
    drm_atomic_helper_wait_for_vblanks(dev, old_state);
#endif

    drm_atomic_helper_cleanup_planes(dev, old_state);

    if(acquired)
    {
        psr_release_display(psr_data, PSR_FLIP_REF);
    }

#if ENABLE_RUNTIME_PM
    zx_drm_for_each_crtc(crtc, dev)
    {
        if (crtc->enabled)
        {
            active = true;
        }
    }
    pm_runtime_mark_last_busy(dev->dev);
    /* if we have active crtcs and we don't have a power ref,take the current one */
    if (active && !zx_card->have_disp_power_ref)
    {
        zx_info("Runtime PM: have active crtcs and reference drm device\n");
        zx_card->have_disp_power_ref = true;
        return ;
    }
    /* if we have no active crtcs, then drop the power ref  we got before */
    if(!active && zx_card->have_disp_power_ref)
    {
        zx_info("Runtime PM: have no active crtcs ,drop the power ref we got before\n");
        pm_runtime_put_autosuspend(dev->dev);
        zx_card->have_disp_power_ref = false;
    }
    pm_runtime_put_autosuspend(dev->dev);
#endif

}
#endif

