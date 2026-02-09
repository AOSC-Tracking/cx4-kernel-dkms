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
#ifndef __ZX_DRM_HELPER_H__
#define __ZX_DRM_HELPER_H__

#if DRM_VERSION_CODE >= KERNEL_VERSION(6, 0, 0)
#include <drm/drm_framebuffer.h>
#include <drm/drm_edid.h>
#endif

#if defined(drm_for_each_plane)
#define zx_drm_for_each_plane(plane, dev) \
    drm_for_each_plane(plane, dev)
#else
#define zx_drm_for_each_plane(plane, dev) \
    list_for_each_entry(plane, &(dev)->mode_config.plane_list, head)
#endif

#if defined(drm_for_each_crtc)
#define zx_drm_for_each_crtc(crtc, dev) \
    drm_for_each_crtc(crtc, dev)
#else
#define zx_drm_for_each_crtc(crtc, dev) \
    list_for_each_entry(crtc, &(dev)->mode_config.crtc_list, head)
#endif

#if DRM_VERSION_CODE >= KERNEL_VERSION(4, 11, 0)
#define ZX_DEFINE_DRM_CONN_ITER(conn_iter) \
    struct drm_connector_list_iter conn_iter
#else
#define ZX_DEFINE_DRM_CONN_ITER(conn_iter)
#endif

#if DRM_VERSION_CODE >= KERNEL_VERSION(4, 12, 0)
#define zx_drm_connector_list_iter_begin(dev, conn_iter) \
        drm_connector_list_iter_begin(dev, conn_iter)
#elif DRM_VERSION_CODE >= KERNEL_VERSION(4, 11, 0)
#define zx_drm_connector_list_iter_begin(dev, conn_iter) \
        drm_connector_list_iter_get(dev, conn_iter)
#else
#define zx_drm_connector_list_iter_begin(dev, conn_iter) \
    do {} while(0)
#endif

#if DRM_VERSION_CODE >= KERNEL_VERSION(4, 12, 0)
#define zx_drm_connector_list_iter_end(conn_iter) \
    drm_connector_list_iter_end(conn_iter)
#elif DRM_VERSION_CODE >= KERNEL_VERSION(4, 11, 0)
#define zx_drm_connector_list_iter_end(conn_iter) \
    drm_connector_list_iter_put(conn_iter)
#else
#define zx_drm_connector_list_iter_end(conn_iter) \
    do {} while(0)
#endif

#if DRM_VERSION_CODE >= KERNEL_VERSION(4, 11, 0)
#define zx_drm_for_each_connector(connector, dev, conn_iter) \
        drm_for_each_connector_iter(connector, conn_iter)
#elif DRM_VERSION_CODE >= KERNEL_VERSION(4, 8, 0)
#define zx_drm_for_each_connector(connector, dev, conn_iter) \
        drm_for_each_connector(connector, dev)
#else
#define zx_drm_for_each_connector(connector, dev, conn_iter) \
        list_for_each_entry(connector, &(dev)->mode_config->connector_list, head)
#endif

static inline
void zx_drm_helper_mode_fill_fb_struct(struct drm_device *dev, struct drm_framebuffer *fb,
#if DRM_VERSION_CODE >= KERNEL_VERSION(6, 17, 0)
                                       const struct drm_format_info *info,
#endif
                                       struct drm_mode_fb_cmd2 *mode_cmd)
{
#if DRM_VERSION_CODE >= KERNEL_VERSION(6, 17, 0)
    drm_helper_mode_fill_fb_struct(dev, fb, info, mode_cmd);
#elif DRM_VERSION_CODE >= KERNEL_VERSION(4, 11, 0)
    drm_helper_mode_fill_fb_struct(dev, fb, mode_cmd);
#else
    drm_helper_mode_fill_fb_struct(fb, mode_cmd);
#endif

}

static inline
void zx_drm_framebuffer_put(struct drm_framebuffer *fb)
{
#if DRM_VERSION_CODE < KERNEL_VERSION(4, 12, 0)
    drm_framebuffer_unreference(fb);
#else
    drm_framebuffer_put(fb);
#endif
}

static inline
int zx_drm_connector_update_edid_property(struct drm_connector *connector,
                                      const struct edid *edid, int edid_len)
{
#if DRM_VERSION_CODE >= KERNEL_VERSION(6,0,0)
    const struct drm_edid* drm_edid = NULL;
    int ret = 0;
    if(edid && edid_len)
    {
        drm_edid = drm_edid_alloc(edid, edid_len);
    }
    ret = drm_edid_connector_update(connector, drm_edid);
    if(drm_edid)
    {
        drm_edid_free(drm_edid);
        drm_edid = NULL;
    }
    return ret;
#elif DRM_VERSION_CODE >= KERNEL_VERSION(4,19,0)
    return drm_connector_update_edid_property(connector, edid);
#else
    return drm_mode_connector_update_edid_property(connector, edid);
#endif
}

static inline
int zx_drm_connector_attach_encoder(struct drm_connector *connector,
                                struct drm_encoder *encoder)
{
#if DRM_VERSION_CODE >= KERNEL_VERSION(4,19,0)
    return drm_connector_attach_encoder(connector, encoder);
#else
    return drm_mode_connector_attach_encoder(connector, encoder);
#endif
}

static inline
struct drm_crtc *zx_drm_crtc_find(struct drm_device *dev, struct drm_file *filep, uint32_t id)
{
#if DRM_VERSION_CODE >= KERNEL_VERSION(4, 15, 0)
    return drm_crtc_find(dev, filep, id);
#else
    return drm_crtc_find(dev, id);
#endif
}

static inline
bool zx_drm_is_async_flip_requested(struct drm_crtc_state *crtc_state)
{
#if DRM_VERSION_CODE >= KERNEL_VERSION(5, 4, 0)
    return crtc_state->async_flip;
#elif DRM_VERSION_CODE >= KERNEL_VERSION(4, 12, 0)
    return !!(crtc_state->pageflip_flags & DRM_MODE_PAGE_FLIP_ASYNC);
#else
    return false;
#endif
}

static inline
int zx_drm_crtc_in_use(struct drm_crtc *crtc)
{
    int enabled = 0;

#if  DRM_VERSION_CODE < KERNEL_VERSION(4, 8, 0)
    struct drm_device *dev = crtc->dev;

    mutex_lock(&dev->mode_config.mutex);

    drm_modeset_lock(&dev->mode_config.connection_mutex, NULL);
    enabled = drm_helper_crtc_in_use(crtc);
    drm_modeset_unlock(&dev->mode_config.connection_mutex);

    mutex_unlock(&dev->mode_config.mutex);
#else
    enabled = (crtc->state->enable && crtc->state->mode_blob);
#endif

    return enabled;
}

/**
 * before drm_atomic_helper_swap_state(), connector_state points to
 * the new_connector_state, in the commit_tail stage, it points to the
 * old_connector_state.
 */
#if !defined(for_each_connector_in_state)
#define zx_drm_for_each_connector_in_state(__state, connector,          \
                                           connector_state, __i)        \
    for ((__i) = 0;                                                     \
         (__i) < (__state)->num_connector &&                            \
         ((connector) = (__state)->connectors[__i].ptr,                 \
         (connector_state) = (__state)->connectors[__i].state, 1);      \
         (__i)++)                                                       \
            for_each_if (connector_state)
#else
#define zx_drm_for_each_connector_in_state(__state, connector,          \
                                           connector_state, __i)        \
    for_each_connector_in_state(__state, connector, connector_state, __i)
#endif


/**
 * before drm_atomic_helper_swap_state(), crtc_state points to
 * the new_crtc_state, in the commit_tail stage, it points to the
 * old_crtc_state.
 */
#if (!defined for_each_crtc_in_state)
#define zx_drm_for_each_crtc_in_state(__state, crtc, crtc_state, __i) \
    for ((__i) = 0;                                                   \
         (__i) < (__state)->dev->mode_config.num_crtc &&              \
         ((crtc) = (__state)->crtcs[__i].ptr,                         \
         (crtc_state) = (__state)->crtcs[__i].state, 1);              \
         (__i)++)                                                     \
            for_each_if (crtc_state)
#else
#define zx_drm_for_each_crtc_in_state(__state, crtc, crtc_state, __i) \
    for_each_crtc_in_state(__state, crtc, crtc_state, __i)
#endif


/**
 * before drm_atomic_helper_swap_state(), plane_state points to
 * the new_plane_state, in the commit_tail stage, it points to the
 * old_plane_state.
 */
#if (!defined for_each_plane_in_state)
#define zx_drm_for_each_plane_in_state(__state, plane, plane_state, __i)   \
    for ((__i) = 0;                                                        \
         (__i) < (__state)->dev->mode_config.num_total_plane &&            \
         ((plane) = (__state)->planes[__i].ptr,                            \
         (plane_state) = (__state)->planes[__i].state, 1);                 \
         (__i)++)                                                          \
            for_each_if (plane)
#else
#define zx_drm_for_each_plane_in_state(__state, plane, plane_state, __i)   \
    for_each_plane_in_state(__state, plane, plane_state, __i)
#endif

#endif
