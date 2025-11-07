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

#include "zx_plane.h"
#include "zx_drmfb.h"
#include "zx_fence.h"
#include "zx_modifies.h"
#include "zx_drm_helper.h"

static const int chx_plane_formats[] = {
    DRM_FORMAT_C8,
    DRM_FORMAT_RGB565,
    DRM_FORMAT_XRGB8888,
    DRM_FORMAT_XBGR8888,
    DRM_FORMAT_ARGB8888,
    DRM_FORMAT_ABGR8888,
    DRM_FORMAT_XRGB2101010,
    DRM_FORMAT_XBGR2101010,
    DRM_FORMAT_ARGB2101010,
    DRM_FORMAT_ABGR2101010,
    DRM_FORMAT_YUYV,
    DRM_FORMAT_YVYU,
    DRM_FORMAT_UYVY,
    DRM_FORMAT_VYUY,
    DRM_FORMAT_AYUV,
};

static const int chx_cursor_formats[] = {
    DRM_FORMAT_XRGB8888,
    DRM_FORMAT_ARGB8888,
};

static  char*  plane_name[] = {
    "PS",
    "SS",
    "TS",
    "FS",
};

static char*  cursor_name = "cursor";

#if  DRM_VERSION_CODE >= KERNEL_VERSION(4, 8, 0)

#if  DRM_VERSION_CODE >= KERNEL_VERSION(4, 12, 0)
static int zx_atomic_helper_update_plane(struct drm_plane *plane,
                   struct drm_crtc *crtc,
                   struct drm_framebuffer *fb,
                   int crtc_x, int crtc_y,
                   unsigned int crtc_w, unsigned int crtc_h,
                   uint32_t src_x, uint32_t src_y,
                   uint32_t src_w, uint32_t src_h,
                   struct drm_modeset_acquire_ctx *ctx)
#else
static int zx_atomic_helper_update_plane(struct drm_plane *plane,
                   struct drm_crtc *crtc,
                   struct drm_framebuffer *fb,
                   int crtc_x, int crtc_y,
                   unsigned int crtc_w, unsigned int crtc_h,
                   uint32_t src_x, uint32_t src_y,
                   uint32_t src_w, uint32_t src_h)
#endif
{
    struct drm_plane_state*  plane_state = plane->state;
    const struct drm_plane_helper_funcs *funcs;

    zx_card_t *zx = plane->dev->dev_private;
    disp_info_t*  disp_info = (disp_info_t *)zx->disp_info;
    psr_data_t* psr_data = disp_info->psr_data;
    int acquired = 0;

    if(to_zx_plane(plane)->is_cursor == 0)
    {
#if  DRM_VERSION_CODE >= KERNEL_VERSION(4, 12, 0)
        return  drm_atomic_helper_update_plane(plane, crtc, fb, crtc_x, crtc_y, crtc_w, crtc_h, src_x, src_y, src_w, src_h, ctx);
#else
        return  drm_atomic_helper_update_plane(plane, crtc, fb, crtc_x, crtc_y, crtc_w, crtc_h, src_x, src_y, src_w, src_h);
#endif
    }

    acquired = psr_acquire_display(psr_data, PSR_CURSOR_REF, 0);

    plane_state->crtc = crtc;

    crtc->state->plane_mask |= (1 << drm_plane_index(plane));

    drm_atomic_set_fb_for_plane(plane_state, fb);
    plane_state->crtc_x = crtc_x;
    plane_state->crtc_y = crtc_y;
    plane_state->crtc_w = crtc_w;
    plane_state->crtc_h = crtc_h;
    plane_state->src_x = src_x;
    plane_state->src_y = src_y;
    plane_state->src_w = src_w;
    plane_state->src_h = src_h;
    to_zx_plane_state(plane_state)->legacy_cursor = 1;

    funcs = plane->helper_private;
    if(funcs && funcs->atomic_check)
    {
#if  DRM_VERSION_CODE < KERNEL_VERSION(5, 13, 0)
        zx_assert(funcs->atomic_check(plane, plane_state) == 0, "");
#else
        zx_assert(funcs->atomic_check(plane, plane_state->state) == 0, "");
#endif
    }

    if(funcs && funcs->atomic_update)
    {
        funcs->atomic_update(plane, NULL);
    }

    plane->old_fb = plane->fb;

    if(acquired)
    {
        psr_release_display(psr_data, PSR_CURSOR_REF);
    }

    return 0;
}

#if  DRM_VERSION_CODE >= KERNEL_VERSION(4, 12, 0)
static int zx_atomic_helper_disable_plane(struct drm_plane *plane, struct drm_modeset_acquire_ctx *ctx)
#else
static int zx_atomic_helper_disable_plane(struct drm_plane *plane)
#endif
{
    struct drm_plane_state*  plane_state = plane->state;
    const struct drm_plane_helper_funcs *funcs;

    if(to_zx_plane(plane)->is_cursor == 0)
    {
#if  DRM_VERSION_CODE >= KERNEL_VERSION(4, 12, 0)
        return  drm_atomic_helper_disable_plane(plane, ctx);
#else
        return  drm_atomic_helper_disable_plane(plane);
#endif
    }
    if((plane_state->crtc)&&(plane_state->crtc->state))
    {
        plane_state->crtc->state->plane_mask &= ~(1 << drm_plane_index(plane));
    }
    plane_state->crtc = NULL;
    drm_atomic_set_fb_for_plane(plane_state, NULL);
    plane_state->crtc_x = 0;
    plane_state->crtc_y = 0;
    plane_state->crtc_w = 0;
    plane_state->crtc_h = 0;
    plane_state->src_x = 0;
    plane_state->src_y = 0;
    plane_state->src_w = 0;
    plane_state->src_h = 0;
    to_zx_plane_state(plane_state)->legacy_cursor = 1;

    funcs = plane->helper_private;
    if(funcs && funcs->atomic_check)
    {
#if  DRM_VERSION_CODE < KERNEL_VERSION(5, 13, 0)
        zx_assert(funcs->atomic_check(plane, plane_state) == 0, "");
#else
        zx_assert(funcs->atomic_check(plane, plane_state->state) == 0, "");
#endif
    }

    if(funcs && funcs->atomic_disable)
    {
        funcs->atomic_disable(plane, NULL);
    }

    plane->old_fb = plane->fb;

    return 0;
}

void zx_plane_destroy(struct drm_plane *plane)
{
    drm_plane_cleanup(plane);
    zx_free(to_zx_plane(plane));
}

static void zx_plane_destroy_state(struct drm_plane *plane, struct drm_plane_state *state)
{
    zx_plane_state_t* zx_pstate = to_zx_plane_state(state);

    __drm_atomic_helper_plane_destroy_state(state);

    zx_free(zx_pstate);
}

static struct drm_plane_state*  zx_plane_duplicate_state(struct drm_plane *plane)
{
    struct drm_plane_state *state;
    zx_plane_state_t *zx_pstate;

    zx_pstate = zx_calloc(sizeof(zx_plane_state_t));

    if (!zx_pstate)
    {
        return NULL;
    }

    state = &zx_pstate->base_pstate;

    __drm_atomic_helper_plane_duplicate_state(plane, state);

#if DRM_VERSION_CODE < KERNEL_VERSION(4, 20, 0)
    zx_pstate->pixel_blend_mode = to_zx_plane_state(plane->state)->pixel_blend_mode;

#if DRM_VERSION_CODE < KERNEL_VERSION(4, 18, 0)
    zx_pstate->alpha = to_zx_plane_state(plane->state)->alpha;
#endif

#endif

#if DRM_VERSION_CODE >= KERNEL_VERSION(4, 8, 0)
    zx_pstate->connector_restore = to_zx_plane_state(plane->state)->connector_restore;
    zx_pstate->disable = to_zx_plane_state(plane->state)->disable;
#endif

    return state;
}

//get/set driver private property, we can add this later
static int zx_plane_atomic_get_property(struct drm_plane *plane,
                const struct drm_plane_state *state,
                struct drm_property *property,
                uint64_t *val)
{
    int ret = -EINVAL;
    zx_plane_t* zx_plane = to_zx_plane(plane);
    zx_plane_state_t* zx_plane_state = to_zx_plane_state(state);

#if DRM_VERSION_CODE < KERNEL_VERSION(4, 20, 0)
    if(property == zx_plane->blend_mode_property)
    {
        *val = zx_plane_state->pixel_blend_mode;
        ret = 0;
    }
#endif

#if DRM_VERSION_CODE < KERNEL_VERSION(4, 18, 0)
    if(property == zx_plane->alpha_property)
    {
        *val = zx_plane_state->alpha;
        ret = 0;
    }
#endif

    if(ret)
    {
#if DRM_VERSION_CODE >= KERNEL_VERSION(4, 9, 0)
        DRM_WARN("Invalid driver-private property '%s'\n", property->name);
#endif
    }
    return ret;
}

static int zx_plane_atomic_set_property(struct drm_plane *plane,
                struct drm_plane_state *state,
                struct drm_property *property,
                uint64_t val)
{
    int ret = -EINVAL;
    zx_plane_t* zx_plane = to_zx_plane(plane);
    zx_plane_state_t* zx_plane_state = to_zx_plane_state(state);

#if DRM_VERSION_CODE < KERNEL_VERSION(4, 20, 0)
    if(property == zx_plane->blend_mode_property)
    {
        zx_plane_state->pixel_blend_mode = val;
        ret = 0;
    }
#endif

#if DRM_VERSION_CODE < KERNEL_VERSION(4, 18, 0)
    if(property == zx_plane->alpha_property)
    {
        zx_plane_state->alpha = val;
        ret = 0;
    }
#endif

    if(ret)
    {
#if DRM_VERSION_CODE >= KERNEL_VERSION(4, 9, 0)
        DRM_WARN("Invalid driver-private property '%s'\n", property->name);
#endif
    }
    return ret;
}

#if DRM_VERSION_CODE >= KERNEL_VERSION(5, 13, 0)
static int zx_plane_atomic_check(struct drm_plane *plane, struct drm_atomic_state *state)
#else
static int zx_plane_atomic_check(struct drm_plane *plane, struct drm_plane_state *new_state)
#endif
{
#if DRM_VERSION_CODE >= KERNEL_VERSION(5, 13, 0)
    struct drm_plane_state *new_state = NULL;
#endif
    zx_plane_t* zx_plane = to_zx_plane(plane);
    unsigned int src_w, src_h, dst_w, dst_h;
    int  status = 0;

    DRM_DEBUG_KMS("plane=%d\n", plane->index);

#if DRM_VERSION_CODE >= KERNEL_VERSION(5, 13, 0)
    if (zx_plane->is_cursor)
    {
        new_state = plane->state;
    }
    else
    {
        new_state = drm_atomic_get_new_plane_state(state, plane);
    }
#endif

    src_w = (new_state->src_w >> 16) & 0xFFFF;
    src_h = (new_state->src_h >> 16) & 0xFFFF;

    dst_w = new_state->crtc_w;
    dst_h = new_state->crtc_h;

    if(!zx_plane->can_window && (new_state->crtc_x != 0 || new_state->crtc_y != 0))
    {
        status = -EINVAL;
        goto END;
    }

    if(!zx_plane->can_up_scale)
    {
        if((src_w < dst_w) || (src_h < dst_h))
        {
            status = -EINVAL;
            goto END;
        }
    }

    if(!zx_plane->can_down_scale)
    {
        if((src_w > dst_w) || (src_h > dst_h))
        {
            status = -EINVAL;
            goto END;
        }
    }

    //max cursor size if 128x128, no new_state for cursor
    if (zx_plane->is_cursor)
    {
        if((dst_w > 128) || (dst_h > 128))
        {
            status = -EINVAL;
            goto END;
        }
    }

END:
    return  status;
}

//add this interface to make kernel 4.9 and above version compatible
#if DRM_VERSION_CODE >= KERNEL_VERSION(4, 9, 0)
static void drm_atomic_set_fence_for_plane_zx(struct drm_plane_state *plane_state, dma_fence_t *fence)
#else
static void drm_atomic_set_fence_for_plane_zx(const struct drm_plane_state *plane_state, dma_fence_t *fence)
#endif
{
    if (plane_state->fence) {
        dma_fence_put(fence);

        return;
    }

#if DRM_VERSION_CODE >= KERNEL_VERSION(4, 9, 0)
    plane_state->fence = fence;
#endif
}

#if DRM_VERSION_CODE >= KERNEL_VERSION(4, 9, 0)
static int zx_prepare_plane_fb(struct drm_plane *plane, struct drm_plane_state *new_state)
#else
static int zx_prepare_plane_fb(struct drm_plane *plane, const struct drm_plane_state *new_state)
#endif
{
    signed long timeout;
    struct drm_framebuffer *fb = new_state->fb;
    struct drm_zx_framebuffer *zxfb = to_zxfb(fb);
    zx_card_t *zx = plane->dev->dev_private;
    int ret;

    if (!fb || !zxfb->obj)
    {
        return 0;
    }

    /* resident */
    if ((ret = zx_core_interface->prepare_and_mark_unpagable(zx->adapter, zxfb->obj->priv)) != S_OK)
    {
        return ret;
    }
    zx_core_interface->update_global_mapping(zx->adapter, zxfb->obj->priv, &zxfb->gpu_virt_addr);

    if (plane->state->fb != fb)
    {
        dma_fence_t *fence = reservation_object_get_excl_rcu(bo_resv(zxfb->obj));
        if (fence)
        {
            drm_atomic_set_fence_for_plane_zx(new_state, fence);
        }
    }

    return  0;
}

static void zx_plane_atomic_update_internal(struct drm_plane *plane,  struct drm_plane_state *old_state, struct drm_plane_state *new_state)
{
    zx_card_t *card = plane->dev->dev_private;
    zx_plane_state_t*  zx_plane_state = to_zx_plane_state(plane->state);

    DRM_DEBUG_KMS("plane=%d, crtc=%d\n", plane->index, (to_zx_plane(plane))->crtc_index);

    if (to_zx_plane(plane)->is_cursor)
    {
        zx_cursor_update_t arg = {0, };

        arg.crtc        = to_zx_plane(plane)->crtc_index;

        if (zx_plane_state->legacy_cursor)
        {
            arg.vsync_on = 0;
        }
        else
        {
            arg.vsync_on = 1;
        }

        if (plane->state->crtc && !zx_plane_state->disable)
        {
            arg.fb          = new_state->fb;
            arg.pos_x       = new_state->crtc_x;
            arg.pos_y       = new_state->crtc_y;
            arg.width       = new_state->crtc_w;
            arg.height      = new_state->crtc_h;
        }

        disp_cbios_update_cursor(card->disp_info, &arg);
    }
    else
    {
        zx_crtc_flip_t arg = {0};

        arg.crtc        = to_zx_plane(plane)->crtc_index;
        arg.stream_type = to_zx_plane(plane)->plane_type;
        arg.oldfb = (old_state)? old_state->fb : NULL;

        if (new_state->crtc && !zx_plane_state->disable)
        {
            arg.fb = new_state->fb;
            arg.crtc_x = new_state->crtc_x;
            arg.crtc_y = new_state->crtc_y;
            arg.crtc_w = new_state->crtc_w;
            arg.crtc_h = new_state->crtc_h;
            arg.src_x = new_state->src_x >> 16;
            arg.src_y = new_state->src_y >> 16;
            arg.src_w = new_state->src_w >> 16;
            arg.src_h = new_state->src_h >> 16;
            if(plane->type == DRM_PLANE_TYPE_OVERLAY)
            {
#if  DRM_VERSION_CODE < KERNEL_VERSION(4, 20, 0)
                arg.blend_mode = zx_plane_state->pixel_blend_mode;
#if  DRM_VERSION_CODE < KERNEL_VERSION(4, 18, 0)
                arg.const_alpha = zx_plane_state->alpha;
#else
                arg.const_alpha = new_state->alpha;
#endif
#else
                arg.blend_mode = new_state->pixel_blend_mode;
#endif
            }

            if (zx_drm_is_async_flip_requested(new_state->crtc->state))
            {
                arg.async_flip = 1;
            }
        }

        disp_cbios_crtc_flip(card->disp_info, &arg);

        {
            zx_perf_event_t perf_event = {0, };
            unsigned long long timestamp;

            zx_get_nsecs(&timestamp);
            perf_event.header.timestamp_high = timestamp >> 32;
            perf_event.header.timestamp_low = timestamp & 0xffffffff;

            perf_event.header.size = sizeof(zx_perf_event_ps_flip_t);
            perf_event.header.type = ZX_PERF_EVENT_PS_FLIP;
            perf_event.ps_flip_event.iga_idx = to_zx_plane(plane)->crtc_index + 1;

            zx_core_interface->perf_event_add_event(card->adapter, &perf_event);
            card->fps_count++;
        }
    }
}

#if DRM_VERSION_CODE >= KERNEL_VERSION(5, 13, 0)
static void zx_plane_atomic_update(struct drm_plane* plane, struct drm_atomic_state* state)
#else
static void zx_plane_atomic_update(struct drm_plane* plane, struct drm_plane_state* old_state)
#endif
{
    struct drm_plane_state *new_state = NULL;
#if DRM_VERSION_CODE >= KERNEL_VERSION(5, 13, 0)
    struct drm_plane_state *old_state = NULL;
#endif
    zx_plane_t* zx_plane = to_zx_plane(plane);
    zx_plane_state_t* zx_pstate = to_zx_plane_state(plane->state);

    DRM_DEBUG_KMS("Update plane=%d\n", plane->index);

#if DRM_VERSION_CODE >= KERNEL_VERSION(5, 13, 0)
    if (zx_plane->is_cursor || state == NULL)
    {
        new_state = plane->state;
    }
    else
    {
        old_state = drm_atomic_get_old_plane_state(state, plane);
        new_state = drm_atomic_get_new_plane_state(state, plane);
    }
#else
    new_state = plane->state;
#endif

    zx_pstate->disable = 0;

    if (zx_pstate->connector_restore)
    {
        if (old_state)
        {
            zx_pstate->disable = to_zx_plane_state(old_state)->disable;
        }
        zx_pstate->connector_restore = 0;
    }
    else if (zx_pstate->resume_from_s4)
    {
        zx_pstate->disable = 1;
        zx_pstate->resume_from_s4 = 0;
    }

    zx_plane_atomic_update_internal(plane, old_state, new_state);
}

#if DRM_VERSION_CODE >= KERNEL_VERSION(5, 13, 0)
static void zx_plane_atomic_disable(struct drm_plane *plane, struct drm_atomic_state *state)
#else
static void zx_plane_atomic_disable(struct drm_plane *plane, struct drm_plane_state *old_state)
#endif
{
#if DRM_VERSION_CODE >= KERNEL_VERSION(5, 13, 0)
    struct drm_plane_state *old_state = NULL;
#endif
    zx_plane_t* zx_plane = to_zx_plane(plane);
    struct drm_plane_state *new_state = NULL;
    zx_plane_state_t* zx_pstate = to_zx_plane_state(plane->state);

    DRM_DEBUG_KMS("Disable plane=%d\n", plane->index);

#if DRM_VERSION_CODE >= KERNEL_VERSION(5, 13, 0)
    if (zx_plane->is_cursor || state == NULL)
    {
        new_state = plane->state;
    }
    else
    {
        old_state = drm_atomic_get_old_plane_state(state, plane);
        new_state = drm_atomic_get_new_plane_state(state, plane);
    }
#else
    new_state = plane->state;
#endif

    zx_pstate->disable = 1;

    zx_plane_atomic_update_internal(plane, old_state, new_state);
}

#if DRM_VERSION_CODE >= KERNEL_VERSION(4, 9, 0)
static void zx_cleanup_plane_fb(struct drm_plane *plane, struct drm_plane_state *old_state)
#else
static void zx_cleanup_plane_fb(struct drm_plane *plane, const struct drm_plane_state *old_state)
#endif
{
    zx_card_t *zx = plane->dev->dev_private;
    struct drm_framebuffer *fb = old_state->fb;

    if (fb && to_zxfb(fb)->obj)
    {
        zx_core_interface->mark_pagable(zx->adapter, to_zxfb(fb)->obj->priv);
    }
}

#if DRM_VERSION_CODE >= KERNEL_VERSION(5, 14, 0)
static bool zx_plane_format_mod_supported(struct drm_plane *plane, uint32_t format,
                                   uint64_t modifier)
{
    int i;
    bool ret = false;

    switch(modifier)
    {
    case DRM_FORMAT_MOD_ZX_DISPLAY:
    case DRM_FORMAT_MOD_ZX_LINEAR:
    case DRM_FORMAT_MOD_ZX_TILED:
    case DRM_FORMAT_MOD_ZX_COMPRESS:
    case DRM_FORMAT_MOD_ZX_TILED_COMPRESS:
    case DRM_FORMAT_MOD_ZX_LOCAL:
    case DRM_FORMAT_MOD_ZX_PCIE:
    case DRM_FORMAT_MOD_ZX_INVALID:
        /* TODO: should do something? */
        ret = true;
        goto check_done;
    default:
        break;
    }

    /* check that modifier is on the list of the plane's supported modifiers. */
    for (i = 0; i < plane->modifier_count; i++)
    {
        if (modifier == plane->modifiers[i])
            break;
    }
    if (i == plane->modifier_count)
    {
        ret = false;
        goto check_done;
    }

    /*
      TODO: check the format and modifier whether being supported
    */

check_done:
    if (!ret)
    {
        zx_info("^^^ not support modifier %lld\n", modifier);
    }
    return ret;
}

#endif

#if DRM_VERSION_CODE < KERNEL_VERSION(4, 20, 0)
static int zx_plane_create_blend_mode_property(struct drm_plane *plane, unsigned int supported_modes)
{
    struct drm_device *dev = plane->dev;
    struct drm_property *prop;
    zx_plane_t* zx_plane = to_zx_plane(plane);
    static const struct drm_prop_enum_list props[] = {
        { DRM_MODE_BLEND_PIXEL_NONE, "None" },
        { DRM_MODE_BLEND_PREMULTI, "Pre-multiplied" },
        { DRM_MODE_BLEND_COVERAGE, "Coverage" },
    };
    unsigned int valid_mode_mask = BIT(DRM_MODE_BLEND_PIXEL_NONE) |
        BIT(DRM_MODE_BLEND_PREMULTI) |
        BIT(DRM_MODE_BLEND_COVERAGE);
    int i = 0, j = 0;

    if ((supported_modes & ~valid_mode_mask) ||((supported_modes & BIT(DRM_MODE_BLEND_PREMULTI)) == 0))
    {
        return -EINVAL;
    }

    prop = drm_property_create(dev, DRM_MODE_PROP_ENUM, "pixel blend mode", hweight32(supported_modes));
    if (!prop)
    {
        return -ENOMEM;
    }

    for (i = 0; i < ARRAY_SIZE(props); i++)
    {
        int ret;

        if (!(BIT(props[i].type) & supported_modes))
        {
            continue;
        }

#if DRM_VERSION_CODE >= KERNEL_VERSION(4, 18, 0)
        ret = drm_property_add_enum(prop, props[i].type, props[i].name);
#else
        ret = drm_property_add_enum(prop, j++, props[i].type, props[i].name);
#endif

        if (ret)
        {
            drm_property_destroy(dev, prop);
            return ret;
        }
    }

    drm_object_attach_property(&plane->base, prop, DRM_MODE_BLEND_PREMULTI);
    zx_plane->blend_mode_property = prop;

    if(plane->state)
    {
        to_zx_plane_state(plane->state)->pixel_blend_mode = DRM_MODE_BLEND_PREMULTI;
    }

    return 0;
}
#endif

#if DRM_VERSION_CODE < KERNEL_VERSION(4, 18, 0)
static int zx_plane_create_alpha_property(struct drm_plane *plane)
{
    struct drm_property *prop;
    zx_plane_t* zx_plane = to_zx_plane(plane);

    prop = drm_property_create_range(plane->dev, 0, "alpha", 0, DRM_BLEND_ALPHA_OPAQUE);
    if (!prop)
    {
        return -ENOMEM;
    }

    drm_object_attach_property(&plane->base, prop, DRM_BLEND_ALPHA_OPAQUE);
    zx_plane->alpha_property = prop;

    if (plane->state)
    {
        to_zx_plane_state(plane->state)->alpha = DRM_BLEND_ALPHA_OPAQUE;
    }

    return 0;
}
#endif

static void zx_plane_create_property(struct drm_device* dev, zx_plane_t* zx_plane)
{
    struct drm_property *prop = NULL;
    int  zpos = 0;

#if DRM_VERSION_CODE >= KERNEL_VERSION(4, 10, 0)
#if DRM_VERSION_CODE < KERNEL_VERSION(4, 13, 0)
    drm_plane_create_rotation_property(&zx_plane->base_plane,
                                       DRM_ROTATE_0,
                                       DRM_ROTATE_0);
#else
    drm_plane_create_rotation_property(&zx_plane->base_plane,
                                       DRM_MODE_ROTATE_0,
                                       DRM_MODE_ROTATE_0);
#endif
#else
    prop = drm_mode_create_rotation_property(dev, DRM_ROTATE_0);
    if(prop)
    {
        drm_object_attach_property(&zx_plane->base_plane.base, prop, DRM_ROTATE_0);
    }

    if(zx_plane->base_plane.state)
    {
        zx_plane->base_plane.state->rotation = DRM_ROTATE_0;
    }
#endif

#if DRM_VERSION_CODE >= KERNEL_VERSION(4, 18, 0)
    drm_plane_create_alpha_property(&zx_plane->base_plane);
#else
    zx_plane_create_alpha_property(&zx_plane->base_plane);
#endif

#if DRM_VERSION_CODE >= KERNEL_VERSION(4, 20, 0)
    drm_plane_create_blend_mode_property(&zx_plane->base_plane,
                                        BIT(DRM_MODE_BLEND_PIXEL_NONE) |
                                        BIT(DRM_MODE_BLEND_PREMULTI) |
                                        BIT(DRM_MODE_BLEND_COVERAGE));
#else
    zx_plane_create_blend_mode_property(&zx_plane->base_plane,
                                        BIT(DRM_MODE_BLEND_PIXEL_NONE) |
                                        BIT(DRM_MODE_BLEND_PREMULTI) |
                                        BIT(DRM_MODE_BLEND_COVERAGE));
#endif

    zpos = (zx_plane->is_cursor)? 32 : zx_plane->plane_type;
    drm_plane_create_zpos_immutable_property(&zx_plane->base_plane, zpos); //we do not support dynamic plane order
}

static const struct drm_plane_funcs zx_plane_funcs = {
    .update_plane = zx_atomic_helper_update_plane,
    .disable_plane = zx_atomic_helper_disable_plane,
    .destroy = zx_plane_destroy,
#if DRM_VERSION_CODE < KERNEL_VERSION(4, 14, 0)
    .set_property = drm_atomic_helper_plane_set_property,
#endif
    .atomic_duplicate_state = zx_plane_duplicate_state,
    .atomic_destroy_state = zx_plane_destroy_state,
    .atomic_set_property = zx_plane_atomic_set_property,
    .atomic_get_property = zx_plane_atomic_get_property,
#if DRM_VERSION_CODE >= KERNEL_VERSION(5, 14, 0)
    .format_mod_supported = zx_plane_format_mod_supported,
#endif
};

static  const struct drm_plane_helper_funcs zx_plane_helper_funcs = {
    .prepare_fb = zx_prepare_plane_fb,
    .cleanup_fb = zx_cleanup_plane_fb,
    .atomic_check = zx_plane_atomic_check,
    .atomic_update = zx_plane_atomic_update,
    .atomic_disable = zx_plane_atomic_disable,
};

#if DRM_VERSION_CODE >= KERNEL_VERSION(5, 14, 0)
//TODO: add format modifiers
static uint64_t chx_modifiers[] = {
    DRM_FORMAT_MOD_LINEAR,
    DRM_FORMAT_MOD_INVALID,
};
#endif

zx_plane_t *zx_drm_plane_create(disp_info_t *disp_info, int index, ZX_PLANE_TYPE type)
{
    zx_card_t*  zx_card = disp_info->zx_card;
    struct  drm_device*  drm = zx_card->drm_dev;
    zx_plane_t*  zx_plane = NULL;
    zx_plane_state_t*  zx_pstate = NULL;
    const int*  formats = 0;
    uint64_t *zx_modifiers = NULL;
    int fmt_count = 0, drm_ptype;
    char* name;
    int ret = -ENOMEM;

    zx_plane = zx_calloc(sizeof(zx_plane_t));
    if (!zx_plane)
    {
        DRM_ERROR("failed to alloc zx plane\n");
        goto failed;
    }

#if DRM_VERSION_CODE >= KERNEL_VERSION(5, 14, 0)
    zx_modifiers = chx_modifiers;
#endif

    zx_plane->crtc_index = index;

    zx_plane->plane_type = type;

    zx_pstate = zx_calloc(sizeof(zx_plane_state_t));
    if (!zx_pstate)
    {
        DRM_ERROR("failed to alloc zx plane state\n");
        goto failed;
    }

    zx_pstate->base_pstate.plane = &zx_plane->base_plane;
    zx_plane->base_plane.state = &zx_pstate->base_pstate;

    if (type == ZX_PLANE_CURSOR)
    {
        zx_plane->is_cursor = 1;
        zx_plane->can_window = 1;
        formats = chx_cursor_formats;
        fmt_count = sizeof(chx_cursor_formats)/sizeof(chx_cursor_formats[0]);
        name = cursor_name;
        drm_ptype = DRM_PLANE_TYPE_CURSOR;
    }
    else
    {
        zx_plane->can_window = (type != ZX_PLANE_PS)? 1 : 0;
        zx_plane->can_up_scale = (disp_info->up_scale_plane_mask[index] & (1 << type))? 1 : 0;
        zx_plane->can_down_scale = (disp_info->down_scale_plane_mask[index] & (1 << type))? 1 : 0;
        formats = chx_plane_formats;
        fmt_count = sizeof(chx_plane_formats)/sizeof(chx_plane_formats[0]);
        name = plane_name[type];
        drm_ptype = (type == ZX_PLANE_PS)? DRM_PLANE_TYPE_PRIMARY : DRM_PLANE_TYPE_OVERLAY;
    }

#if DRM_VERSION_CODE < KERNEL_VERSION(4, 14, 0)
    ret = drm_universal_plane_init(drm, &zx_plane->base_plane,
                                   (1 << index), &zx_plane_funcs,
                                   formats, fmt_count,
                                   drm_ptype,
                                   "IGA%d-%s", (index+1), name);
#else
    ret = drm_universal_plane_init(drm, &zx_plane->base_plane,
                                   (1 << index), &zx_plane_funcs,
                                   formats, fmt_count, zx_modifiers,
                                   drm_ptype,
                                   "IGA%d-%s", (index+1), name);
#endif

    if (ret)
    {
        DRM_ERROR("failed to init plane\n");
        goto failed;
    }

    drm_plane_helper_add(&zx_plane->base_plane, &zx_plane_helper_funcs);

    zx_plane_create_property(drm, zx_plane);

    DRM_DEBUG_KMS("create plane=%d,name=%s\n", zx_plane->base_plane.index, zx_plane->base_plane.name);
    return  zx_plane;

failed:
    if (zx_pstate)
    {
        zx_free(zx_pstate);
        zx_pstate = NULL;
    }

    if (zx_plane)
    {
        zx_free(zx_plane);
        zx_plane = NULL;
    }

    return ERR_PTR(ret);
}

#else

static int zx_update_plane(struct drm_plane *plane, struct drm_crtc *crtc,
           struct drm_framebuffer *fb, int crtc_x, int crtc_y,
           unsigned int crtc_w, unsigned int crtc_h,
           uint32_t src_x, uint32_t src_y,
           uint32_t src_w, uint32_t src_h)
{
    struct drm_device *dev = plane->dev;
    zx_crtc_t *zx_crtc = to_zx_crtc(crtc);
    zx_plane_t *zx_plane = to_zx_plane(plane);
    zx_card_t *card = dev->dev_private;
    int pipe = zx_plane->crtc_index;
    int ret = 0;
    int primary_w = crtc->mode.hdisplay, primary_h = crtc->mode.vdisplay;
    zx_crtc_flip_t arg = {0};

    src_x = src_x >> 16;
    src_y = src_y >> 16;
    src_w = src_w >> 16;
    src_h = src_h >> 16;

    if (crtc_x >= primary_w || crtc_y >= primary_h)
    {
        return -EINVAL;
    }

    /* Don't modify another pipe's plane */
    if (pipe != zx_crtc->pipe)
    {
        return -EINVAL;
    }

    if ((crtc_x + crtc_w) > primary_w)
    {
        crtc_w = primary_w - crtc_x;
    }

    if (crtc_y + crtc_h > primary_h)
    {
        crtc_h = primary_h - crtc_y;
    }

    if (!crtc_w || !crtc_h) /* Again, nothing to display */
    {
        goto out;
    }

    if(!zx_plane->can_window && (crtc_w != primary_w || crtc_h != primary_h))
    {
        return  -EINVAL;
    }

    if(!zx_plane->can_up_scale && (src_w < crtc_w || src_h < crtc_h))
    {
        return  -EINVAL;
    }

    if(!zx_plane->can_down_scale && (src_w > crtc_w || src_h > crtc_h))
    {
        return  -EINVAL;
    }

    arg.fb = fb;
    arg.oldfb = (plane->state)? plane->state->fb : NULL;
    arg.crtc = to_zx_crtc(crtc)->pipe;
    arg.stream_type = zx_plane->plane_type;
    arg.crtc_x = crtc_x;
    arg.crtc_y = crtc_y;
    arg.crtc_w = crtc_w;
    arg.crtc_h = crtc_h;
    arg.src_x = src_x;
    arg.src_y = src_y;
    arg.src_w = src_w;
    arg.src_h = src_h;

    mutex_lock(&dev->struct_mutex);

    //need add wait fence back? intel is to pin fence
    disp_cbios_crtc_flip(card->disp_info, &arg);

    //need add wait_vblank if fb changed
    disp_wait_for_vblank(card->disp_info, pipe, 50);

    zx_plane->src_pos = src_x | (src_y << 16);
    zx_plane->src_size = src_w | (src_h << 16);
    zx_plane->dst_pos = crtc_x | (crtc_y << 16);
    zx_plane->dst_size = crtc_w | (crtc_h << 16);

    mutex_unlock(&dev->struct_mutex);
out:
    return ret;
}

static int zx_disable_plane(struct drm_plane *plane)
{
    return 0;
}

void zx_plane_destroy(struct drm_plane* plane)
{
    zx_disable_plane(plane);
    drm_plane_cleanup(plane);
    zx_free(to_zx_plane(plane));
}

static const struct drm_plane_funcs zx_plane_funcs = {
    .update_plane = zx_update_plane,
    .disable_plane = zx_disable_plane,
    .destroy = zx_plane_destroy,
};

zx_plane_t *zx_drm_plane_create(disp_info_t *disp_info, int index, ZX_PLANE_TYPE type)
{
    zx_card_t *zx_card = disp_info->zx_card;
    struct drm_device *drm = zx_card->drm_dev;
    zx_plane_t *zx_plane = NULL;
    const int *formats = 0;
    int ret = -ENOMEM, fmt_count = 0;

    zx_plane = zx_calloc(sizeof(zx_plane_t));
    if (!zx_plane)
    {
        DRM_ERROR("failed to alloc zx plane\n");
        goto failed;
    }

    zx_plane->crtc_index = index;
    zx_plane->plane_type = type;
    zx_plane->can_window = (type != ZX_PLANE_PS)? 1 : 0;
    zx_plane->can_up_scale = (disp_info->up_scale_plane_mask[index] & (1 << type))? 1 : 0;
    zx_plane->can_down_scale = (disp_info->down_scale_plane_mask[index] & (1 << type))? 1 : 0;

    formats = chx_plane_formats;
    fmt_count = sizeof(chx_plane_formats)/sizeof(chx_plane_formats[0]);

    ret = drm_plane_init(drm, &zx_plane->base_plane, (1 << index), &zx_plane_funcs, formats, fmt_count, FALSE);
    if (ret)
    {
        DRM_ERROR("failed to init plane\n");
        goto failed_init_plane;
    }

    return  zx_plane;

failed_init_plane:
    zx_free(zx_plane);
    zx_plane = NULL;

failed:
    return ERR_PTR(ret);
}


#endif
