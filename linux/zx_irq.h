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
#ifndef  _ZX_IRQ_
#define _ZX_IRQ_

#include "zx_disp.h"
#include "zx_cbios.h"
#include "zx_crtc.h"

#define PCI_EN_MEM_SPACE 0x02

#define VIDEO_INT_ERR_WARNING 0x0
#define VIDEO_INT_ERR_SKIP_SLICE 0x01
#define VIDEO_INT_ERR_SKIP_FRAME 0x02
// HEVC dec max error level: 0x04, VC1 dec & H264 dec max error level:0x2
#define VIDEO_INT_SKIP_PRINT_ERR 0x05
#ifdef _DEBUG_
#define VIDEO_INT_ERROR_LEVEL VIDEO_INT_ERR_WARNING
#else
#define VIDEO_INT_ERROR_LEVEL VIDEO_INT_SKIP_PRINT_ERR
#endif

#define OUTPUT_POLL_PERIOD  (HZ)
#define HPD_DELAYED_DETECT_PERIOD  (HZ)

#define MAX_DELAYED_HPD_EVENT 10

typedef struct _irq_chip_funcs
{
    unsigned long long (*get_default_intr)(disp_info_t* disp_info);
    unsigned long long (*get_intr_enable_mask)(disp_info_t* disp_info);
    void (*set_intr_enable_mask)(disp_info_t* disp_info, unsigned long long intr_mask);
    void (*enable_msi)(disp_info_t* disp_info);
    void (*disable_msi)(disp_info_t* disp_info);
    unsigned long long (*get_interrupt_mask)(disp_info_t* disp_info);
    void (*detect_pmu_intr_conflict_recovery)(disp_info_t* disp_info,unsigned long long intr_mask);
}irq_chip_funcs_t;

extern irq_chip_funcs_t irq_chip_funcs;

#if DRM_VERSION_CODE >= KERNEL_VERSION(5, 7, 0)
u32 zx_get_vblank_counter(struct drm_crtc *crtc);
int  zx_enable_vblank(struct drm_crtc *crtc);
void  zx_disable_vblank(struct drm_crtc *crtc);
#else
u32 zx_get_vblank_counter(struct drm_device *dev, pipe_t pipe);
int  zx_enable_vblank(struct drm_device *dev, pipe_t pipe);
void  zx_disable_vblank(struct drm_device *dev, pipe_t pipe);
#endif

#if DRM_VERSION_CODE < KERNEL_VERSION(4, 13, 0)
int zx_get_vblank_timestamp(struct drm_device *dev, pipe_t pipe,
             int *max_error, struct timeval *time, unsigned flags);
#endif

int zx_get_crtc_scanoutpos(struct drm_device *dev, unsigned int pipe,
             unsigned int flags, int *vpos, int *hpos,
             ktime_t *stime, ktime_t *etime,
             const struct drm_display_mode *mode);

int zx_get_crtc_scanoutpos_kernel_4_8(struct drm_device *dev, unsigned int pipe,
             unsigned int flags, int *vpos, int *hpos,
             ktime_t *stime, ktime_t *etime);

int zx_legacy_get_crtc_scanoutpos(struct drm_device *dev, int pipe, unsigned int flags,
                     int *vpos, int *hpos, ktime_t *stime, ktime_t *etime);

bool zx_get_crtc_scanoutpos_kernel_4_10(struct drm_device *dev, unsigned int pipe,
             bool in_vblank_irq, int *vpos, int *hpos,
             ktime_t *stime, ktime_t *etime,
             const struct drm_display_mode *mode);

#if DRM_VERSION_CODE >= KERNEL_VERSION(5, 7, 0)
bool zx_crtc_get_scanout_position(struct drm_crtc *crtc,
                                  bool in_vblank_irq, int *vpos, int *hpos,
                                  ktime_t *stime, ktime_t *etime,
                                  const struct drm_display_mode *mode);
#endif


#if DRM_VERSION_CODE >= KERNEL_VERSION(5, 15, 0)
int zx_irq_install(struct drm_device *drm_dev);
#endif

void  zx_irq_preinstall(struct drm_device *dev);

int  zx_irq_postinstall(struct drm_device *dev);

irqreturn_t zx_irq_handle(int irq, void *arg);

void zx_irq_uninstall (struct drm_device *dev);

void zx_hot_plug_intr_onoff(disp_info_t* disp_info, int on);

void zx_hpd_irq_work_func(struct work_struct *work);

void zx_poll_signal_work_func(struct work_struct *work);

void zx_hda_work_func(struct work_struct* work);

void zx_output_poll_work_func(struct work_struct *work);

void zx_poll_enable(disp_info_t* disp_info);

void zx_poll_disable(disp_info_t *disp_info);

#if ENABLE_RUNTIME_PM
bool zx_kms_helper_is_poll_worker(void);
#endif

void zx_disp_enable_interrupt(disp_info_t*  disp_info);

void zx_disp_disable_interrupt(disp_info_t*  disp_info);

void zx_video_interrupt_handle(disp_info_t*  disp_info, unsigned long long video_int_mask);

#if DRM_VERSION_CODE >= KERNEL_VERSION(4, 15, 0)
void zx_psr_timer(struct timer_list *t);
#else
void zx_psr_timer(unsigned long data);
#endif

int zx_psr_work_thread(void *data);

#endif
