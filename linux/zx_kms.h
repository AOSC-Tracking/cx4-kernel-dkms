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
#ifndef  _ZX_KMS_
#define _ZX_KMS_

#include <linux/version.h>

#ifndef DRM_VERSION_CODE
#define DRM_VERSION_CODE LINUX_VERSION_CODE
#endif

#if DRM_VERSION_CODE >= KERNEL_VERSION(4, 8, 0)
#include <drm/drm_atomic.h>
#include <drm/drm_atomic_helper.h>
#include <drm/drm_plane_helper.h>
#endif

#if DRM_VERSION_CODE >= KERNEL_VERSION(4, 20, 0)
#include <drm/drm_atomic_uapi.h>
#endif

#include <drm/drm_crtc_helper.h>

#if DRM_VERSION_CODE >= KERNEL_VERSION(5, 1, 0)
#include <drm/drm_probe_helper.h>
#endif

#if DRM_VERSION_CODE >= KERNEL_VERSION(5, 14, 0)
#if DRM_VERSION_CODE < KERNEL_VERSION(6, 13, 0)
#include <drm/drm_aperture.h>
#else
#include <linux/aperture.h>
#endif
#endif

#if DRM_VERSION_CODE >= KERNEL_VERSION(5, 19, 0)
#include <drm/display/drm_dp_helper.h>
#elif DRM_VERSION_CODE >= KERNEL_VERSION(5, 18, 0)
#include <drm/dp/drm_dp_helper.h>
#elif DRM_VERSION_CODE >= KERNEL_VERSION(4, 19, 0)
#include <drm/drm_dp_helper.h>
#endif

#if DRM_VERSION_CODE >= KERNEL_VERSION(5, 5, 0)
#include <drm/drm_fourcc.h>
#include <drm/drm_vblank.h>
#if DRM_VERSION_CODE < KERNEL_VERSION(5, 15, 0)
#include <drm/drm_irq.h>
#endif
#endif
#include <drm/drm_fb_helper.h>
#include <linux/screen_info.h>

#ifndef  container_of
#define offsetof(TYPE, MEMBER) ((size_t) &((TYPE *)0)->MEMBER)
#define container_of(ptr, type, member) (type *)((char *)(ptr) - offsetof(type, member))
#endif

#if  DRM_VERSION_CODE >= KERNEL_VERSION(4, 8, 0)
#define to_zx_atomic_state(x) container_of(x, zx_ato_state_t, base_ato_state)
#define to_zx_crtc_state(x)  container_of(x, zx_crtc_state_t, base_cstate)
#define to_zx_plane_state(x)  container_of(x, zx_plane_state_t, base_pstate)
#define to_zx_conn_state(x)  container_of(x, zx_connector_state_t, base_conn_state)
#endif

#define to_zx_crtc(x)  container_of(x, zx_crtc_t, base_crtc)
#define to_zx_connector(x)  container_of(x, zx_connector_t, base_connector)
#define to_zx_plane(x)  container_of(x, zx_plane_t, base_plane)
#define to_zx_encoder(x)  container_of(x, zx_encoder_t, base_encoder)

#if DRM_VERSION_CODE < KERNEL_VERSION(4, 20, 0)
#define DRM_MODE_BLEND_PREMULTI    0
#define DRM_MODE_BLEND_COVERAGE    1
#define DRM_MODE_BLEND_PIXEL_NONE  2
#endif

#if DRM_VERSION_CODE < KERNEL_VERSION(4, 18, 0)
#define DRM_BLEND_ALPHA_OPAQUE  0xffff
#endif

#define COLOR_LEGACY_LUT_LENGTH  256

typedef enum _OUTPUT_SIGNAL
{
    OUTPUT_SIGNAL_RGB = 0,
    OUTPUT_SIGNAL_Y422,
    OUTPUT_SIGNAL_Y444,
    OUTPUT_SIGNAL_Y420,
}OUTPUT_SIGNAL;

typedef struct
{
    unsigned int set_crtc         :1;
    unsigned int set_encoder      :1;
    unsigned int output_signal    :2; // bit num must equel with OUTPUT_SIGNAL
    unsigned int reserverd        :28;
}update_mode_para_t;

typedef struct
{
    struct drm_crtc    base_crtc;
    unsigned int       pipe;
    int                crtc_dpms;
    int                support_scale;
    unsigned int       scaler_width;
    unsigned int       scaler_height;
    unsigned int       dst_width;
    unsigned int       dst_height;
    unsigned int       plane_cnt;
    unsigned int       vsync_int;
    struct zx_flip_work *flip_work;
#if DRM_VERSION_CODE < KERNEL_VERSION(4, 8, 0)
    struct drm_zx_gem_object *cursor_bo;
    unsigned short  cursor_x, cursor_y;
    unsigned short  cursor_w, cursor_h;
    unsigned int  lut_entry[256];  //(b | g << 8 | r << 16) for 8bit lut or (b | g << 10 | r << 20) for 10bit lut
#endif
}zx_crtc_t;

typedef struct
{
    struct drm_encoder  base_encoder;
    int                 output_type;
    int                 dpms_status;
#if DRM_VERSION_CODE < KERNEL_VERSION(4, 8, 0)
    zx_crtc_t*  new_crtc;
#endif
}zx_encoder_t;

#define HPD_DEVICE_POLL_TIME 5

#if DRM_VERSION_CODE >= KERNEL_VERSION(4, 19, 0)

typedef struct
{
    struct drm_dp_aux aux;
    struct drm_connector *connector;
}zx_dp_data_t;

#endif

typedef struct
{
    struct drm_connector  base_connector;
    int                   output_type;
    int                   conn_type;
    int                   monitor_type;
    int                   hda_codec_index;
    unsigned long long    hpd_int_bit;
    int                   hpd_enable;
    OUTPUT_SIGNAL         prefer_signal;
#if DRM_VERSION_CODE < KERNEL_VERSION(4, 8, 0)
    zx_encoder_t*  new_encoder;
#endif
    struct os_mutex* access_lock;
    struct zx_sink *sink;
    struct zx_i2c_adapter *i2c_adapter;
    atomic_t          polling_time;
    struct
    {
        unsigned int support_audio : 1;
        unsigned int support_psr   : 1;
        unsigned int edid_overrd   : 1;
        unsigned int reserved      : 29;
    };

    void *priv_data;
}zx_connector_t;

typedef  struct
{
    struct drm_plane base_plane;
#if DRM_VERSION_CODE < KERNEL_VERSION(4, 8, 0)
    unsigned int src_pos;
    unsigned int src_size;
    unsigned int dst_pos;
    unsigned int dst_size;
#else
#if DRM_VERSION_CODE < KERNEL_VERSION(4, 20, 0)
    struct drm_property *blend_mode_property;
#if DRM_VERSION_CODE < KERNEL_VERSION(4, 18, 0)
    struct drm_property *alpha_property;
#endif
#endif
#endif
    int              crtc_index;
    int              plane_type;
    unsigned int     can_window     : 1;  //except PS
    unsigned int     can_up_scale   : 1;
    unsigned int     can_down_scale : 1;
    unsigned int     is_cursor      : 1;
}zx_plane_t;

typedef struct
{
    struct drm_framebuffer *oldfb;
    struct drm_framebuffer *fb;
    int  crtc;
    int  stream_type;
    int  crtc_x;
    int  crtc_y;
    int  crtc_w;
    int  crtc_h;
    int  src_x;
    int  src_y;
    int  src_w;
    int  src_h;
#if  DRM_VERSION_CODE >= KERNEL_VERSION(4, 8, 0)
    struct
    {
        int  blend_mode;
        union
        {
            unsigned int  const_alpha;
            unsigned int  plane_alpha;
            unsigned int  color_key;
        };
    };
#endif

    int async_flip;
}zx_crtc_flip_t;
//int disp_cbios_crtc_flip(disp_info_t *disp_info, struct drm_crtc *crtc, struct drm_framebuffer *fb,
//        int stream_type, int visible, int src_x, int src_y, int src_w, int src_h, int dst_x, int dst_y, int dst_w, int dst_h);

typedef struct
{
    int crtc;
    int vsync_on;
    int pos_x;
    int pos_y;
    int width;
    int height;
    struct drm_framebuffer *fb;
}zx_cursor_update_t;

#if  DRM_VERSION_CODE >= KERNEL_VERSION(4, 8, 0)

typedef struct
{
    struct drm_connector_state base_conn_state;
}zx_connector_state_t;

typedef  struct
{
    struct drm_crtc_state base_cstate;
    struct zx_sink *sink;
    struct
    {
        unsigned int scale_change  : 1;
        unsigned int dst_change    : 1;
        unsigned int reserved      : 30;
    };
}zx_crtc_state_t;

typedef struct
{
    struct drm_plane_state base_pstate;
#if DRM_VERSION_CODE < KERNEL_VERSION(4, 20, 0)
    unsigned int pixel_blend_mode;
#if DRM_VERSION_CODE < KERNEL_VERSION(4, 18, 0)
    unsigned int alpha;
#endif
#endif
    unsigned  int  color_key;
    struct
    {
        unsigned int  disable           : 1;
        unsigned int  legacy_cursor     : 1;
        unsigned int  connector_restore : 1;
        unsigned int  resume_from_s4    : 1;
        unsigned int  reserved          : 28;
    };
}zx_plane_state_t;

typedef struct
{
    struct drm_atomic_state base_ato_state;
}zx_ato_state_t;

#endif

#endif

