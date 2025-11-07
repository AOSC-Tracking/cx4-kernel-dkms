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
#include "zx_irq.h"
#include "zx_crtc.h"
#include "zx_capture_drv.h"
#include "zxgfx_trace.h"
#include "zx_drm_helper.h"
#include "zx_version.h"

#if DRM_VERSION_CODE >= KERNEL_VERSION(4, 13, 0)
/* get_scanout_position() return flags */
#define DRM_SCANOUTPOS_VALID        (1 << 0)
#define DRM_SCANOUTPOS_IN_VBLANK    (1 << 1)
#define DRM_SCANOUTPOS_ACCURATE     (1 << 2)
#endif

#define __STR(x)    #x
#define STR(x)      __STR(x)

static struct drm_crtc* zx_get_crtc_by_pipe(struct drm_device *dev, pipe_t pipe)
{
    struct drm_crtc *crtc = NULL;

    zx_drm_for_each_crtc(crtc, dev)
    {
        if (drm_get_crtc_index(crtc) == pipe)
        {
            return crtc;
        }
    }

    return NULL;
}

#if DRM_VERSION_CODE >= KERNEL_VERSION(5, 7, 0)
u32 zx_get_vblank_counter(struct drm_crtc *crtc)
#else
u32 zx_get_vblank_counter(struct drm_device *dev, pipe_t pipe)
#endif
{
#if DRM_VERSION_CODE >= KERNEL_VERSION(5, 7, 0)
    struct drm_device *dev = crtc->dev;
    unsigned int pipe = crtc->index;
#endif
    zx_card_t*  zx_card = dev->dev_private;
    disp_info_t*  disp_info = (disp_info_t *)zx_card->disp_info;
    zx_get_counter_t  zx_counter;
    int  vblank_cnt = 0;

    zx_memset(&zx_counter, 0, sizeof(zx_get_counter_t));
    zx_counter.crtc_index = pipe;
    zx_counter.vblk = &vblank_cnt;
    disp_cbios_get_counter(disp_info, &zx_counter);

    return  (u32)vblank_cnt;
}

#if DRM_VERSION_CODE < KERNEL_VERSION(4, 13, 0)
int zx_get_vblank_timestamp(struct drm_device *dev, pipe_t pipe,
                            int *max_error, struct timeval *time, unsigned flags)
{
    struct drm_crtc *crtc = zx_get_crtc_by_pipe(dev, pipe);
    struct drm_display_mode *mode;

    if (!crtc)
        return -EINVAL;

    mode = &crtc->hwmode;
#if DRM_VERSION_CODE >= KERNEL_VERSION(4, 4, 0)
    if (dev->mode_config.funcs->atomic_commit)
    {
        mode = &crtc->state->adjusted_mode;
    }

    return drm_calc_vbltimestamp_from_scanoutpos(dev, pipe, max_error, time, flags, mode);
#else
    return drm_calc_vbltimestamp_from_scanoutpos(dev, pipe, max_error, time, flags, crtc, mode);
#endif
}
#endif
int zx_get_crtc_scanoutpos(struct drm_device *dev, unsigned int pipe,
                           unsigned int flags, int *vpos, int *hpos,
                           ktime_t *stime, ktime_t *etime,
                           const struct drm_display_mode *mode)
{
    zx_card_t*  zx_card = dev->dev_private;
    disp_info_t*  disp_info = (disp_info_t *)zx_card->disp_info;
    zx_get_counter_t  zx_counter;
    int  in_vblank = 0, ret = 0, status = 0;

    zx_memset(&zx_counter, 0, sizeof(zx_get_counter_t));
    zx_counter.crtc_index = pipe;
    zx_counter.hpos = hpos;
    zx_counter.vpos = vpos;
    zx_counter.in_vblk = &in_vblank;

    if (stime)
    {
        *stime = ktime_get();
    }

    status = disp_cbios_get_counter(disp_info, &zx_counter);

    if (etime)
    {
        *etime = ktime_get();
    }

    if(status == DISP_OK)
    {
        ret = DRM_SCANOUTPOS_VALID | DRM_SCANOUTPOS_ACCURATE;
        if(in_vblank)
        {
            *vpos -= mode->crtc_vblank_end;
            if(*hpos)
            {
                *hpos -= mode->crtc_htotal;
                *vpos += 1;
            }
            ret |= DRM_SCANOUTPOS_IN_VBLANK;
        }
    }

    return  ret;
}

int zx_legacy_get_crtc_scanoutpos(struct drm_device *dev, int pipe, unsigned int flags,
                                  int *vpos, int *hpos, ktime_t *stime, ktime_t *etime)
{
    zx_card_t*  zx_card = dev->dev_private;
    disp_info_t*  disp_info = (disp_info_t *)zx_card->disp_info;
    zx_get_counter_t  zx_counter;
    struct drm_crtc* crtc = NULL;
    struct drm_display_mode* mode = NULL;
    int  in_vblank = 0, ret = 0, status = 0;

    zx_memset(&zx_counter, 0, sizeof(zx_get_counter_t));
    zx_counter.crtc_index = pipe;
    zx_counter.hpos = hpos;
    zx_counter.vpos = vpos;
    zx_counter.in_vblk = &in_vblank;

    zx_drm_for_each_crtc(crtc, dev)
    {
        if(crtc && to_zx_crtc(crtc)->pipe == pipe)
        {
            mode = &crtc->hwmode;
            break;
        }
    }

    if (stime)
    {
        *stime = ktime_get();
    }
    status = disp_cbios_get_counter(disp_info, &zx_counter);

    if (etime)
    {
        *etime = ktime_get();
    }
    if(status == DISP_OK)
    {
        ret = DRM_SCANOUTPOS_VALID | DRM_SCANOUTPOS_ACCURATE;
        if(in_vblank)
        {
            if(mode)
            {
                *vpos -= mode->crtc_vblank_end;
                if(*hpos)
                {
                    *hpos -= mode->crtc_htotal;
                    *vpos += 1;
                }
            }
            ret |= DRM_SCANOUTPOS_IN_VBLANK;
        }
    }

    return  ret;
}

bool zx_get_crtc_scanoutpos_kernel_4_10(struct drm_device *dev, unsigned int pipe,
                bool in_vblank_irq, int *vpos, int *hpos,
                ktime_t *stime, ktime_t *etime,
                const struct drm_display_mode *mode)
{
    return zx_get_crtc_scanoutpos(dev, pipe, 0, vpos, hpos, stime, etime, mode);
}

#if DRM_VERSION_CODE >= KERNEL_VERSION(5, 7, 0)
bool zx_crtc_get_scanout_position(struct drm_crtc *crtc,
                                  bool in_vblank_irq, int *vpos, int *hpos,
                                  ktime_t *stime, ktime_t *etime,
                                  const struct drm_display_mode *mode)
{
    struct drm_device *dev = crtc->dev;
    unsigned int pipe = crtc->index;

    return zx_get_crtc_scanoutpos(dev, pipe, 0, vpos, hpos, stime, etime, mode);
}
#endif

#if DRM_VERSION_CODE >= KERNEL_VERSION(5, 7, 0)
int zx_enable_vblank(struct drm_crtc *crtc)
#else
int zx_enable_vblank(struct drm_device *dev, pipe_t pipe)
#endif
{
#if DRM_VERSION_CODE >= KERNEL_VERSION(5, 7, 0)
    struct drm_device *dev = crtc->dev;
    unsigned int pipe = crtc->index;
#endif

    zx_card_t*  zx_card = dev->dev_private;
    disp_info_t*  disp_info = (disp_info_t *)zx_card->disp_info;
    psr_data_t* psr_data = disp_info->psr_data;
    irq_chip_funcs_t* chip_func = (irq_chip_funcs_t*)disp_info->irq_chip_func;
    unsigned long long  intrrpt = 0;
    unsigned long flags = 0;

    if(!chip_func || !chip_func->get_intr_enable_mask || !chip_func->set_intr_enable_mask)
    {
        return 0;
    }

    if(pipe == IGA1)
    {
        intrrpt = INT_VSYNC1;
    }
    else if(pipe == IGA2)
    {
        intrrpt = INT_VSYNC2;
    }
    else if(pipe == IGA3)
    {
        intrrpt = INT_VSYNC3;
    }
    else if(pipe == IGA4)
    {
        intrrpt = INT_VSYNC4;
    }

    psr_acquire_display(psr_data, PSR_VBLANK_REF, 0);

    flags = zx_spin_lock_irqsave(disp_info->intr_lock);

    if(disp_info->irq_enabled)
    {
        disp_info->intr_en_bits = chip_func->get_intr_enable_mask(disp_info);
        disp_info->intr_en_bits |= intrrpt;
        chip_func->set_intr_enable_mask(disp_info, disp_info->intr_en_bits);
    }
    else
    {
        disp_info->intr_en_bits |= intrrpt;
    }

    zx_spin_unlock_irqrestore(disp_info->intr_lock, flags);

    trace_zxgfx_vblank_onoff(pipe, 1);

    return  0;
}

#if DRM_VERSION_CODE >= KERNEL_VERSION(5, 7, 0)
void  zx_disable_vblank(struct drm_crtc *crtc)
#else
void  zx_disable_vblank(struct drm_device *dev, pipe_t pipe)
#endif
{
#if DRM_VERSION_CODE >= KERNEL_VERSION(5, 7, 0)
    struct drm_device *dev = crtc->dev;
    unsigned int pipe = crtc->index;
#endif
    zx_card_t*  zx_card = dev->dev_private;
    disp_info_t*  disp_info = (disp_info_t *)zx_card->disp_info;
    psr_data_t* psr_data = disp_info->psr_data;
    irq_chip_funcs_t* chip_func = (irq_chip_funcs_t*)disp_info->irq_chip_func;
    unsigned long long  intrrpt = 0;
    unsigned long flags = 0;

    if(!chip_func || !chip_func->get_intr_enable_mask || !chip_func->set_intr_enable_mask)
    {
        return;
    }

    if(pipe == IGA1)
    {
        intrrpt = INT_VSYNC1;
    }
    else if(pipe == IGA2)
    {
        intrrpt = INT_VSYNC2;
    }
    else if(pipe == IGA3)
    {
        intrrpt = INT_VSYNC3;
    }
    else if(pipe == IGA4)
    {
        intrrpt = INT_VSYNC4;
    }

    flags = zx_spin_lock_irqsave(disp_info->intr_lock);

    if(disp_info->irq_enabled)
    {
        disp_info->intr_en_bits = chip_func->get_intr_enable_mask(disp_info);
        disp_info->intr_en_bits &= ~intrrpt;
        chip_func->set_intr_enable_mask(disp_info, disp_info->intr_en_bits);
    }
    else
    {
        disp_info->intr_en_bits &= ~intrrpt;
    }

    zx_spin_unlock_irqrestore(disp_info->intr_lock, flags);

    psr_release_display(psr_data, PSR_VBLANK_REF);

    trace_zxgfx_vblank_onoff(pipe, 0);
}


void zx_disp_enable_interrupt(disp_info_t*  disp_info)
{
    zx_card_t* zx_card = disp_info->zx_card;
    irq_chip_funcs_t* chip_func = (irq_chip_funcs_t*)disp_info->irq_chip_func;
    unsigned long long intr_en;
    unsigned long flags = 0;

    if(!chip_func)
    {
        return;
    }

    flags = zx_spin_lock_irqsave(disp_info->intr_lock);

    intr_en = disp_info->intr_en_bits;  //intr_en_bits is saved by disable_interrupt func

    if(!intr_en)
    {
        intr_en = chip_func->get_default_intr(disp_info);
    }

    chip_func->set_intr_enable_mask(disp_info, intr_en);

    if(zx_card->pdev->msi_enabled && chip_func->enable_msi)
    {
        chip_func->enable_msi(disp_info);
    }

    disp_info->irq_enabled = 1;

    zx_spin_unlock_irqrestore(disp_info->intr_lock, flags);
}

void zx_disp_disable_interrupt(disp_info_t*  disp_info)
{
    zx_card_t* zx_card = disp_info->zx_card;
    irq_chip_funcs_t* chip_func = (irq_chip_funcs_t*)disp_info->irq_chip_func;
    unsigned long flags = 0;
    unsigned long long intr_en_bits;

    if(!chip_func)
    {
        return;
    }

    flags = zx_spin_lock_irqsave(disp_info->intr_lock);

    intr_en_bits = chip_func->get_intr_enable_mask(disp_info);

    chip_func->set_intr_enable_mask(disp_info, 0);

    if(zx_card->pdev->msi_enabled && chip_func->disable_msi)
    {
        chip_func->disable_msi(disp_info);
    }

    if(disp_info->irq_installing)
    {
        disp_info->intr_en_bits |= chip_func->get_default_intr(disp_info);
    }
    else
    {
        disp_info->intr_en_bits = intr_en_bits;
    }

    disp_info->irq_enabled = 0;

    zx_spin_unlock_irqrestore(disp_info->intr_lock, flags);
}

void  zx_irq_preinstall(struct drm_device *dev)
{
    zx_card_t*  zx_card = dev->dev_private;
    disp_info_t*  disp_info = (disp_info_t *)zx_card->disp_info;

    disp_info->irq_installing = 1;
    //disable all interrupt
    zx_disp_disable_interrupt(disp_info);
}

int zx_irq_postinstall(struct drm_device *dev)
{
    zx_card_t*  zx_card = dev->dev_private;
    disp_info_t*  disp_info = (disp_info_t *)zx_card->disp_info;

    zx_disp_enable_interrupt(disp_info);

    disp_info->irq_installing = 0;

    return 0;
}

#if DRM_VERSION_CODE >= KERNEL_VERSION(5, 15, 0)
int zx_irq_install(struct drm_device *drm_dev)
{
    int irq = to_pci_dev(drm_dev->dev)->irq;
    int ret = 0;

    zx_irq_preinstall(drm_dev);

    ret = request_irq(irq, zx_irq_handle, IRQF_SHARED, STR(DRIVER_NAME), drm_dev);

    if (ret < 0)
    {
        zx_error("request irq failed\n");
        return ret;
    }

    zx_irq_postinstall(drm_dev);

    return ret;
}
#endif

static void zx_vblank_intrr_handle(struct drm_device *dev, unsigned int intrr)
{
    zx_card_t *zx = dev->dev_private;
    struct  drm_crtc* crtc = NULL;
    zx_crtc_t *zx_crtc = NULL;
    unsigned int crtc_idx = 0;

    zx_drm_for_each_crtc(crtc, dev)
    {
        zx_crtc = to_zx_crtc(crtc);

        if (intrr & zx_crtc->vsync_int)
        {
            zx_perf_event_t perf_event = {0, };
            zx_get_counter_t get_cnt = {0, };
            unsigned int vblcnt = 0;
            unsigned long long timestamp;

            drm_crtc_handle_vblank(crtc);

            crtc_idx = drm_get_crtc_index(crtc);

            get_cnt.crtc_index = crtc_idx;
            get_cnt.vblk = &vblcnt;
            disp_cbios_get_counter(zx->disp_info, &get_cnt);

            trace_zxgfx_vblank_intrr(crtc_idx, vblcnt);

            zx_get_nsecs(&timestamp);
            perf_event.header.timestamp_high = timestamp >> 32;
            perf_event.header.timestamp_low = timestamp & 0xffffffff;
            perf_event.header.size = sizeof(zx_perf_event_vsync_t);
            perf_event.header.type = ZX_PERF_EVENT_VSYNC;
            perf_event.vsync_event.iga_idx = crtc_idx + 1;
            perf_event.vsync_event.vsync_cnt_low = vblcnt;
            perf_event.vsync_event.vsync_cnt_high = 0;

            zx_core_interface->perf_event_add_isr_event(zx->adapter, &perf_event);
            zx_core_interface->hwq_process_vsync_event(zx->adapter, timestamp);
        }
    }
}

#define MAX_DP_QUEUE_DEPTH    (MAX_DP_EVENT_NUM -1)
#define DP_QUEUE_DEPTH(head, tail)        ((head <= tail)? (tail-head) : (MAX_DP_EVENT_NUM -head + tail))
#define DP_QUEUE_FULL(head, tail)         (DP_QUEUE_DEPTH(head, tail) >= MAX_DP_QUEUE_DEPTH)
#define DP_QUEUE_EMPTY(head, tail)        (DP_QUEUE_DEPTH(head, tail) == 0)
#define DP_ADVANCE_QUEUE_POS(pos)  {pos = (pos < MAX_DP_QUEUE_DEPTH)? (pos + 1) : 0; }


static bool zx_hpd_irq_delay_ack(struct drm_connector *connector, int dp_int)
{
    zx_encoder_t *zx_encoder = NULL;

    if (connector->encoder)
    {
        zx_encoder = to_zx_encoder(connector->encoder);

        if (zx_encoder->dpms_status != 0)
        {
            return false;
        }
    }

    if (dp_int != DP_HPD_HDMI_OUT && dp_int != DP_HPD_DP_OUT)
    {
        return false;
    }

    return true;
}

static void  zx_hpd_handle(struct drm_device* dev, unsigned int hpd)
{
    zx_card_t*  zx_card = dev->dev_private;
    disp_info_t*  disp_info = (disp_info_t *)zx_card->disp_info;
    struct drm_connector* connector = NULL;
    zx_connector_t* zx_connector = NULL;
    int  dp_int = 0, queue_work = 0;
    unsigned long flags;

    if(!hpd)
    {
        return;
    }

    flags = zx_spin_lock_irqsave(disp_info->hpd_lock);

    list_for_each_entry(connector, &dev->mode_config.connector_list, head)
    {
        zx_connector = to_zx_connector(connector);

        if((connector->polled == DRM_CONNECTOR_POLL_HPD) &&
            (zx_connector->hpd_int_bit & hpd) && (zx_connector->hpd_enable))
        {
            dp_int = disp_cbios_get_dpint_type(disp_info, zx_connector->output_type);
            if(dp_int == DP_HPD_NONE)
            {
                continue;
            }

            if(DP_QUEUE_EMPTY(disp_info->head, disp_info->tail))
            {
                queue_work = 1;
            }

            if(!DP_QUEUE_FULL(disp_info->head, disp_info->tail))
            {
                disp_info->event[disp_info->tail].device = zx_connector->output_type;
                disp_info->event[disp_info->tail].int_type = dp_int;

                if (zx_hpd_irq_delay_ack(connector, dp_int))
                {
                    disp_info->event[disp_info->tail].ack_delay = jiffies + HPD_DELAYED_DETECT_PERIOD;
                }
                else
                {
                    disp_info->event[disp_info->tail].ack_delay = 0;
                    queue_work = 1;
                }

                DP_ADVANCE_QUEUE_POS(disp_info->tail);
            }
        }
    }

    zx_spin_unlock_irqrestore(disp_info->hpd_lock, flags);

    if (queue_work)
    {
        mod_delayed_work(system_wq, &disp_info->hpd_irq_work, 0);
    }
}

#define  HDAC_INT_REG1   0x8288
#define  HDAC_INT_REG2   0x33D9C
#define  HDAC_INT_REG3   0x344A0
#define  HDAC_INT_BITS  (1 << 25)

static void  zx_hdaudio_handle(struct drm_device* dev)
{
    zx_card_t*  zx_card = dev->dev_private;
    disp_info_t*  disp_info = (disp_info_t*)zx_card->disp_info;
    adapter_info_t* adapter = disp_info->adp_info;
    struct drm_connector* connector = NULL;
    zx_connector_t* zx_connector = NULL;
    unsigned long flags;
    unsigned int hdac_int_value = 0;
    unsigned int hda_codec_index = 0, hda_intr_outputs = 0;

    hdac_int_value = zx_read32(adapter->mmio + HDAC_INT_REG1);
    zx_write32(adapter->mmio + HDAC_INT_REG1, (hdac_int_value & ~HDAC_INT_BITS));
    if(hdac_int_value & HDAC_INT_BITS)
    {
        hda_codec_index |= (1 << 0);
    }

    hdac_int_value = zx_read32(adapter->mmio + HDAC_INT_REG2);
    zx_write32(adapter->mmio + HDAC_INT_REG2, (hdac_int_value & ~HDAC_INT_BITS));
    if(hdac_int_value & HDAC_INT_BITS)
    {
        hda_codec_index |= (1 << 1);
    }

    if(adapter->chip_id == CHIP_CNE001)
    {
        hdac_int_value = zx_read32(adapter->mmio + HDAC_INT_REG3);
        zx_write32(adapter->mmio + HDAC_INT_REG3, (hdac_int_value & ~HDAC_INT_BITS));
        if(hdac_int_value & HDAC_INT_BITS)
        {
            hda_codec_index |= (1 << 2);
        }
    }

    if(!hda_codec_index)
    {
        return;
    }

    list_for_each_entry(connector, &dev->mode_config.connector_list, head)
    {
        zx_connector = to_zx_connector(connector);

        if(zx_connector->hda_codec_index & hda_codec_index)
        {
            hda_intr_outputs |= zx_connector->output_type;
        }
    }

    if(hda_intr_outputs)
    {
        flags = zx_spin_lock_irqsave(disp_info->hda_lock);

        disp_info->hda_intr_outputs |= hda_intr_outputs;

        zx_spin_unlock_irqrestore(disp_info->hda_lock, flags);

        schedule_work(&disp_info->hda_work);
    }
}

static void zx_dvfs_handle(struct drm_device *dev, unsigned int dvfs)
{
    zx_card_t*  zx_card = dev->dev_private;
    disp_info_t*  disp_info = (disp_info_t*)zx_card->disp_info;
    adapter_info_t* adapter = disp_info->adp_info;
    zx_dvfs_info_t *dvfs_info = zx_card->dvfs_info;

#define DVFS_INFO_(type, name)      \
    do {                            \
       if (dvfs & INT_DVFS_##type)  \
          dvfs_info->dvfs_##name++; \
    } while(0)

#define DVFS_INFO(type, name)                          \
    do {                                               \
       DVFS_INFO_(type##_CHECKSIZE, name##_checksize); \
       DVFS_INFO_(type##_CHANGE, name##_change_num);   \
    }while(0)

    DVFS_INFO(3D, 3d);
    DVFS_INFO(VCP0, vcp0);
    DVFS_INFO(VCP1, vcp1);
    DVFS_INFO(VPP, vpp);

}

irqreturn_t zx_irq_handle(int irq, void *arg)
{
    struct drm_device* dev = arg;
    zx_card_t*  zx_card = dev->dev_private;
    disp_info_t*  disp_info = (disp_info_t *)zx_card->disp_info;
    irq_chip_funcs_t* chip_func = (irq_chip_funcs_t*)disp_info->irq_chip_func;
    unsigned short  command;
    unsigned long long  intrr = 0;

    if(!disp_info->irq_enabled)
    {
        return  IRQ_NONE;
    }

    if(atomic_xchg(&disp_info->atomic_irq_lock, 1) == 1)
    {
        return IRQ_NONE;
    }

    if(!chip_func || !chip_func->get_interrupt_mask)
    {
        return IRQ_NONE;
    }
    intrr = chip_func->get_interrupt_mask(disp_info);

    if(intrr & INT_VSYNCS)
    {
        zx_vblank_intrr_handle(dev, intrr & INT_VSYNCS);

        // zx_capture_handle(disp_info, intrr & INT_VSYNCS);
    }

    if(intrr & INT_HDCODEC)
    {
        zx_hdaudio_handle(dev);
    }

    if(intrr & INT_HOTPLUG)
    {
        zx_hpd_handle(dev, intrr & INT_HOTPLUG);
    }

    // if(intrr & INT_VIPS)
    // {
    //     zx_capture_handle(disp_info, intrr & INT_VIPS);
    // }

    if(intrr & INT_VIDEO_EVENTS)
    {
        zx_video_interrupt_handle(disp_info, intrr & INT_VIDEO_EVENTS);
    }

    if (!zx_card->adapter_info.init_render &&
        zx_card->adapter_info.patch_fence_intr_lost)
    {
        intrr |= INT_FENCE;
    }

    if(intrr & INT_FENCE)
    {
        tasklet_schedule(&zx_card->fence_notify);

        zx_card->adapter_info.init_render = 0;
    }

    if (intrr & INT_DVFS)
    {
        zx_dvfs_handle(dev, intrr & INT_DVFS);
    }

    if (intrr & INT_PAGE_FAULT)
    {
        schedule_work(&zx_card->page_fault_work);
    }

    if (intrr & INT_PMU_CPU)
    {
        tasklet_schedule(&zx_card->fine_tuning_power_tuning);
    }

    /* workload patch: if the pmu interrupt cannot be clear,
       subsequent pmu interrupt cannot be accepted normally  */

    if (chip_func->detect_pmu_intr_conflict_recovery && disp_info->intr_en_bits & INT_PMU_CPU )
    {
        chip_func->detect_pmu_intr_conflict_recovery(disp_info , intrr);
    }

    atomic_set(&disp_info->atomic_irq_lock, 0);

    return  IRQ_HANDLED;
}

static void zx_hot_plug_intr_ctrl(disp_info_t* disp_info, unsigned long long intr, int enable)
{
    irq_chip_funcs_t* chip_func = (irq_chip_funcs_t*)disp_info->irq_chip_func;
    unsigned long  flags = 0;
    unsigned long long  intr_en = 0;

    if(!chip_func || !chip_func->get_intr_enable_mask || !chip_func->set_intr_enable_mask)
    {
        return;
    }

    intr &= INT_HOTPLUG;

    flags = zx_spin_lock_irqsave(disp_info->intr_lock);

    if(disp_info->irq_enabled)
    {
        intr_en = chip_func->get_intr_enable_mask(disp_info);
    }
    else
    {
        intr_en = disp_info->intr_en_bits;
    }

    if(enable)
    {
        intr_en |= intr;
    }
    else
    {
        intr_en &= ~intr;
    }

    if(disp_info->irq_enabled)
    {
        chip_func->set_intr_enable_mask(disp_info, intr_en);
    }

    disp_info->intr_en_bits = intr_en;

    zx_spin_unlock_irqrestore(disp_info->intr_lock, flags);
}

void zx_hot_plug_intr_onoff(disp_info_t* disp_info, int on)
{
    zx_card_t* zx_card = disp_info->zx_card;
    struct drm_device* drm = zx_card->drm_dev;
    struct drm_connector* connector = NULL;
    zx_connector_t* zx_connector = NULL;
    unsigned long long  hpd_int_bits = 0;
    ZX_DEFINE_DRM_CONN_ITER(conn_iter);

    mutex_lock(&drm->mode_config.mutex);

    zx_drm_connector_list_iter_begin(drm, &conn_iter);

    zx_drm_for_each_connector(connector, drm, &conn_iter)
    {
        //mark status to enable for all outputs that support hot plug
        zx_connector = to_zx_connector(connector);
        if ((connector->polled == DRM_CONNECTOR_POLL_HPD) && zx_connector->hpd_int_bit)
        {
            zx_connector->hpd_enable = on;
            hpd_int_bits |= zx_connector->hpd_int_bit;
        }
    }

    zx_drm_connector_list_iter_end(&conn_iter);

    mutex_unlock(&drm->mode_config.mutex);

    if (hpd_int_bits)
    {
        zx_hot_plug_intr_ctrl(disp_info, hpd_int_bits, on);
    }
}

void zx_handle_hpd_from_debugfs(void *data, int int_bit)
{
    zx_connector_t *zx_connector = (zx_connector_t *)data;
    struct drm_device *drm_dev = zx_connector->base_connector.dev;
    zx_card_t *zx_card = (zx_card_t *)drm_dev->dev_private;
    disp_info_t *disp_info = (disp_info_t *)zx_card->disp_info;
    unsigned long flags;

    if (int_bit != DP_HPD_HDMI_OUT && int_bit != DP_HPD_DP_OUT &&
        int_bit != DP_HPD_IN)
    {
        zx_error("invalid dp int value\n");
        return;
    }

    flags = zx_spin_lock_irqsave(disp_info->hpd_lock);

    if (!DP_QUEUE_FULL(disp_info->head, disp_info->tail))
    {
        disp_info->event[disp_info->tail].device = zx_connector->output_type;
        disp_info->event[disp_info->tail].int_type = int_bit;
        disp_info->event[disp_info->tail].ack_delay = 0;

        DP_ADVANCE_QUEUE_POS(disp_info->tail);
    }

    zx_spin_unlock_irqrestore(disp_info->hpd_lock, flags);

    mod_delayed_work(system_wq, &disp_info->hpd_irq_work, 0);
}

void zx_hpd_irq_work_func(struct work_struct *work)
{
    struct delayed_work *delayed_work = to_delayed_work(work);
    disp_info_t *disp_info = container_of(delayed_work, disp_info_t, hpd_irq_work);
    zx_card_t *zx_card = disp_info->zx_card;
    struct drm_device *drm = zx_card->drm_dev;
    psr_data_t* psr_data = disp_info->psr_data;
    zx_connector_t* zx_conn = NULL;
    unsigned long irq = 0;
    int device = 0, int_type = 0, to_detect = 0, poll_dev = 0, reset_psr = 0;
    int dequeue = 0, need_poll = 0;
    DP_EVENT delayed_event[MAX_DELAYED_HPD_EVENT];
    int delayed_event_num = 0, i = 0;
    unsigned long ack_delay = 0, max_jiffies = 0, now_jiffies = jiffies;

    while(1)
    {
        int  empty = 0;

        irq = zx_spin_lock_irqsave(disp_info->hpd_lock);

        if(dequeue)
        {
            DP_ADVANCE_QUEUE_POS(disp_info->head);
        }

        if(DP_QUEUE_EMPTY(disp_info->head, disp_info->tail))
        {
            empty = 1;
            dequeue = 0;
        }
        else
        {
            device = disp_info->event[disp_info->head].device;
            int_type = disp_info->event[disp_info->head].int_type;
            ack_delay = disp_info->event[disp_info->head].ack_delay;
            dequeue = 1;

            if (ack_delay > 0 && time_before(now_jiffies, ack_delay))
            {
                if (delayed_event_num >= MAX_DELAYED_HPD_EVENT)
                {
                    delayed_event_num = delayed_event_num % MAX_DELAYED_HPD_EVENT;
                    zx_info("delayed event reaches max limit!!\n");
                }

                delayed_event[delayed_event_num].device = device;
                delayed_event[delayed_event_num].int_type = int_type;
                delayed_event[delayed_event_num].ack_delay = ack_delay;
                delayed_event_num++;

                max_jiffies = ack_delay > max_jiffies ? ack_delay : max_jiffies;

                zx_spin_unlock_irqrestore(disp_info->hpd_lock, irq);
                continue;
            }

        }

        zx_spin_unlock_irqrestore(disp_info->hpd_lock, irq);

        if(empty)
        {
            break;
        }

        zx_conn = zx_get_connector_by_device_id(disp_info, device);
        if(!zx_conn)
        {
            continue;
        }

        poll_dev = 0;

        if(int_type == DP_HPD_HDMI_OUT)
        {
            to_detect |= device;
            poll_dev = device;
        }
        else if(int_type == DP_HPD_DP_OUT)
        {
            to_detect |= device;
        }
        else
        {
            //to do irq handle
            int need_detect = 0, dp_port = 0, psr_rst = 0;

            zx_mutex_lock(zx_conn->access_lock);
            disp_cbios_handle_dp_irq(disp_info, device, int_type, &need_detect, &dp_port, &psr_rst);
            zx_mutex_unlock(zx_conn->access_lock);

            if(need_detect)
            {
                to_detect |= device;
                if(!dp_port)      //non DP port interrupt(aux can't work), do poll detect also
                {
                    poll_dev = device;
                }
            }

            if(zx_conn->monitor_type == UT_OUTPUT_TYPE_PANEL && psr_rst)
            {
                reset_psr = 1;
            }
        }
      
        if(poll_dev)
        {
            atomic_set(&zx_conn->polling_time, HPD_DEVICE_POLL_TIME);
            need_poll = 1;
        }
    }

    //fast detect  at 1st time, if in/out detected on poll dev, polling_time will cleared to 0
    if(to_detect)
    {
        zx_detect_and_update_connectors(disp_info, to_detect, 1, 1);
    }

    if(reset_psr)
    {
        psr_reset(psr_data);
    }

    //slow detect by polling thread for poll dev, repeat 5 times until in/out detected
    if(need_poll && !(disp_info->poll_status & POLL_KEEP_RUNNING))
    {
        schedule_delayed_work(&drm->mode_config.output_poll_work, OUTPUT_POLL_PERIOD);
    }

    irq = zx_spin_lock_irqsave(disp_info->hpd_lock);
    while (i < delayed_event_num)
    {
        if (!DP_QUEUE_FULL(disp_info->head, disp_info->tail))
        {
            disp_info->event[disp_info->tail].device = delayed_event[i].device;
            disp_info->event[disp_info->tail].int_type = delayed_event[i].int_type;
            disp_info->event[disp_info->tail].ack_delay = delayed_event[i].ack_delay;

            DP_ADVANCE_QUEUE_POS(disp_info->tail);
        }

        i++;
    }

    zx_spin_unlock_irqrestore(disp_info->hpd_lock, irq);

    if (delayed_event_num > 0)
    {
        mod_delayed_work(system_wq, delayed_work, max_jiffies - now_jiffies);
    }
}

void zx_poll_signal_work_func(struct work_struct *work)
{
    zx_card_t*  zx_card  = container_of(work, zx_card_t, signal_poll_work);

    struct list_head process_list;
    struct zx_dma_fence *fence, *tmp;
    struct zx_dma_fence_driver *driver = zx_card->fence_drv;
    int ret;

    zx_spin_lock(zx_card->poll_lock);
    ret = zx_card->signal_poll_status == SKIP_POLLING;
    zx_spin_unlock(zx_card->poll_lock);
    if (ret)
    {
        return;
    }

    zx_spin_lock(zx_card->poll_lock);
    zx_card->signal_poll_status = SKIP_POLLING;
    zx_spin_unlock(zx_card->poll_lock);

    while(atomic_read(&driver->poll_count))
    {
        tasklet_schedule(&zx_card->fence_notify);
        zx_msleep(1);
    }

    zx_spin_lock(zx_card->poll_lock);
    zx_card->signal_poll_status = POLL_KEEP_RUNNING;
    zx_spin_unlock(zx_card->poll_lock);
}

void zx_hda_work_func(struct work_struct* work)
{
    disp_info_t*  disp_info = container_of(work, disp_info_t, hda_work);
    zx_card_t*  zx_card = disp_info->zx_card;
    struct drm_device*  drm = zx_card->drm_dev;
    struct drm_mode_config *mode_config = &drm->mode_config;
    struct  drm_connector* connector = NULL;
    zx_connector_t*  zx_connector = NULL;
    unsigned long irq = 0;
    unsigned int hda_outputs = 0;
    enum drm_connector_status old_status;

    irq = zx_spin_lock_irqsave(disp_info->hda_lock);

    hda_outputs = disp_info->hda_intr_outputs;
    disp_info->hda_intr_outputs = 0;

    zx_spin_unlock_irqrestore(disp_info->hda_lock, irq);

    if(!hda_outputs)
    {
        return;
    }

    list_for_each_entry(connector, &mode_config->connector_list, head)
    {
        zx_connector =  to_zx_connector(connector);
        if(zx_connector->output_type & hda_outputs && zx_connector->support_audio)
        {
            disp_cbios_set_hda_codec(disp_info, zx_connector);
        }
    }
}

void zx_irq_uninstall (struct drm_device *drm_dev)
{
    int irq = to_pci_dev(drm_dev->dev)->irq;
    zx_card_t*  zx_card = drm_dev->dev_private;
    disp_info_t*  disp_info = (disp_info_t *)zx_card->disp_info;

#if DRM_VERSION_CODE >= KERNEL_VERSION(5, 15, 0)
    free_irq(irq, drm_dev);
#endif

    //disable all interrupt
    zx_disp_disable_interrupt(disp_info);

}

void zx_poll_enable(disp_info_t* disp_info)
{
    zx_card_t*  zx_card = disp_info->zx_card;
    struct drm_device* drm = zx_card->drm_dev;

    if (!drm->mode_config.poll_enabled)
    {
        //driver does not support poll, do nothing
        return;
    }

    disp_info->poll_status &= ~SKIP_POLLING;

    schedule_delayed_work(&drm->mode_config.output_poll_work, OUTPUT_POLL_PERIOD);
}

void zx_poll_disable(disp_info_t *disp_info)
{
    zx_card_t*  zx_card = disp_info->zx_card;
    struct drm_device* drm = zx_card->drm_dev;

    if (!drm->mode_config.poll_enabled)
    {
        //driver does not support poll, do nothing
        return;
    }

    disp_info->poll_status |= SKIP_POLLING;

    cancel_delayed_work_sync(&drm->mode_config.output_poll_work);
}

#if ENABLE_RUNTIME_PM
bool zx_kms_helper_is_poll_worker(void)
{
    struct work_struct *work = current_work();
    return work && work->func == zx_output_poll_work_func;
}
#endif

void zx_output_poll_work_func(struct work_struct *work)
{
    struct delayed_work *delayed_work = to_delayed_work(work);
    struct drm_device *dev = container_of(delayed_work, struct drm_device, mode_config.output_poll_work);
    zx_card_t*  zx_card = dev->dev_private;
    disp_info_t*  disp_info = (disp_info_t *)zx_card->disp_info;
    struct drm_connector *connector = NULL;
    zx_connector_t *zx_connector = NULL;
    int repoll = 0, changed = 0;

    ZX_DEFINE_DRM_CONN_ITER(conn_iter);

    if(disp_info->poll_status & SKIP_POLLING)
    {
        return;
    }

    if (!mutex_trylock(&dev->mode_config.mutex))
    {
        repoll = true;
        goto out;
    }

    zx_drm_connector_list_iter_begin(dev, &conn_iter);

    zx_drm_for_each_connector(connector, dev, &conn_iter)
    {
        int conn_change = 0;

        zx_connector = to_zx_connector(connector);

        /* Ignore forced connectors. */
        if (connector->force || !connector->polled ||
            (connector->polled == DRM_CONNECTOR_POLL_HPD && atomic_dec_if_positive(&zx_connector->polling_time) < 0))
        {
            continue;
        }

        /* if we are connected and don't want to poll for disconnect skip it */
        if (connector->status == connector_status_connected &&
            connector->polled != DRM_CONNECTOR_POLL_HPD &&
            !(connector->polled & DRM_CONNECTOR_POLL_DISCONNECT))
        {
            continue;
        }

        repoll = 1;

        zx_connector_detect_locked(connector, 0, &conn_change);
        if(conn_change)
        {
            changed = 1;
        }
    }

    zx_drm_connector_list_iter_end(&conn_iter);

    mutex_unlock(&dev->mode_config.mutex);

out:
    if (changed)
    {
        drm_kms_helper_hotplug_event(dev);
    }

    if (repoll)
    {
        disp_info->poll_status |= POLL_KEEP_RUNNING;
        schedule_delayed_work(delayed_work, OUTPUT_POLL_PERIOD);
    }
    else
    {
        disp_info->poll_status &= ~POLL_KEEP_RUNNING;
    }
}

void zx_video_interrupt_handle(disp_info_t*  disp_info, unsigned long long video_int_mask)
{
    adapter_info_t*  adapter = disp_info->adp_info;
    unsigned int  video0_info0 = 0, video0_info1 = 0, video0_info2 = 0, video0_info3 = 0;
    unsigned int  video1_info0 = 0, video1_info1 = 0, video1_info2 = 0, video1_info3 = 0;
    int error_level = 0;

    if(video_int_mask & INT_FE_HANG_VD0 )
    {
        video0_info0 = zx_read32(adapter->mmio + 0x4A81C);
        zx_info("*******************CORE0_FE_HANG!*******************\n");
        zx_info("0x4A81C: %x\n", video0_info0);
    }

    if(video_int_mask & INT_BE_HANG_VD0 )
    {
        video0_info0 = zx_read32(adapter->mmio + 0x4A81C);
        zx_info("*******************CORE0_BE_HANG!*******************\n");
        zx_info("0x4A81C: %x\n", video0_info0);
    }

    if(video_int_mask & INT_FE_HANG_VD1 )
    {
        video1_info0 = zx_read32(adapter->mmio + 0x4C81C);
        zx_info("*******************CORE1_FE_HANG!*******************\n");
        zx_info("0x4C81C: %x\n", video1_info0);
    }

    if(video_int_mask & INT_BE_HANG_VD1 )
    {
        video1_info0 = zx_read32(adapter->mmio + 0x4C81C);
        zx_info("*******************CORE1_BE_HANG!*******************\n");
        zx_info("0x4C81C: %x\n", video1_info0);
    }

    if(adapter->chip_id == CHIP_CNE001)
    {
        if(video_int_mask & INT_FE_HANG_VD2 )
        {
            video1_info0 = zx_read32(adapter->mmio + 0x4D81C);
            zx_info("*******************CORE2_FE_HANG!*******************\n");
            zx_info("0x4D81C: %x\n", video1_info0);
        }
        if(video_int_mask & INT_BE_HANG_VD2 )
        {
            video1_info0 = zx_read32(adapter->mmio + 0x4D81C);
            zx_info("*******************CORE2_BE_HANG!*******************\n");
            zx_info("0x4D81C: %x\n", video1_info0);
        }
    }

    error_level = (zx_read32(adapter->mmio + 0x4A824) >> 8) & 0x7; // bit[10:8] is error level
    if((video_int_mask & INT_FE_ERROR_VD0) && (error_level >= VIDEO_INT_ERROR_LEVEL))
    {
        video0_info0 = zx_read32(adapter->mmio + 0x4A81C);
        video0_info1 = zx_read32(adapter->mmio + 0x4A820);
        video0_info2 = zx_read32(adapter->mmio + 0x4A824);
        zx_info("*******************CORE0_FE_ERROR!*******************\n");
        zx_info("0x4A81C: %x 0x4A820: %x 0x4A824: %x\n", video0_info0, video0_info1, video0_info2);
    }

    if((video_int_mask & INT_BE_ERROR_VD0) && (error_level >= VIDEO_INT_ERROR_LEVEL))
    {
        video0_info0 = zx_read32(adapter->mmio + 0x4A81C);
        video0_info1 = zx_read32(adapter->mmio + 0x4A820);
        video0_info2 = zx_read32(adapter->mmio + 0x4A824);
        if(!((video0_info2 << 4) & 0x32))
        {
            zx_info("*******************CORE0_BE_ERROR!*******************\n");
            zx_info("0x4A81C: %x 0x4A820: %x 0x4A824: %x\n", video0_info0, video0_info1, video0_info2);
        }
    }

    error_level = (zx_read32(adapter->mmio + 0x4C824) >> 8) & 0x7; // bit[10:8] is error level
    if((video_int_mask & INT_FE_ERROR_VD1) && (error_level >= VIDEO_INT_ERROR_LEVEL))
    {
        video1_info0 = zx_read32(adapter->mmio + 0x4C81C);
        video1_info1 = zx_read32(adapter->mmio + 0x4C820);
        video1_info2 = zx_read32(adapter->mmio + 0x4C824);
        zx_info("*******************CORE1_FE_ERROR!*******************\n");
        zx_info("0x4C81C: %x 0x4C820: %x 0x4C824: %x\n", video1_info0, video1_info1, video1_info2);
    }

    if((video_int_mask & INT_BE_ERROR_VD1) && (error_level >= VIDEO_INT_ERROR_LEVEL))
    {
        video1_info0 = zx_read32(adapter->mmio + 0x4C81C);
        video1_info1 = zx_read32(adapter->mmio + 0x4C820);
        video1_info2 = zx_read32(adapter->mmio + 0x4C824);
        if(!((video0_info2 << 4) & 0x32))
        {
            zx_info("*******************CORE1_BE_ERROR!*******************\n");
            zx_info("0x4C81C: %x 0x4C820: %x 0x4C824: %x\n", video1_info0, video1_info1, video1_info2);
        }
    }

    if(adapter->chip_id == CHIP_CNE001)
    {
        error_level = (zx_read32(adapter->mmio + 0x4D824) >> 8) & 0x7; // bit[10:8] is error level
        if((video_int_mask & INT_FE_ERROR_VD2) && (error_level >= VIDEO_INT_ERROR_LEVEL))
        {
            video1_info0 = zx_read32(adapter->mmio + 0x4D81C);
            video1_info1 = zx_read32(adapter->mmio + 0x4D820);
            video1_info2 = zx_read32(adapter->mmio + 0x4D824);
            zx_info("*******************CORE2_FE_ERROR!*******************\n");
            zx_info("0x4D81C: %x 0x4D820: %x 0x4D824: %x\n", video1_info0, video1_info1, video1_info2);
        }

        if((video_int_mask & INT_BE_ERROR_VD2) && (error_level >= VIDEO_INT_ERROR_LEVEL))
        {
            video1_info0 = zx_read32(adapter->mmio + 0x4D81C);
            video1_info1 = zx_read32(adapter->mmio + 0x4D820);
            video1_info2 = zx_read32(adapter->mmio + 0x4D824);
            if(!((video0_info2 << 4) & 0x32))
            {
                zx_info("*******************CORE2_BE_ERROR!*******************\n");
                zx_info("0x4D81C: %x 0x4D820: %x 0x4D824: %x\n", video1_info0, video1_info1, video1_info2);
            }
        }
    }
}

#if DRM_VERSION_CODE >= KERNEL_VERSION(4, 15, 0)
void zx_psr_timer(struct timer_list *t)
#else
void zx_psr_timer(unsigned long data)
#endif
{
#if DRM_VERSION_CODE >= KERNEL_VERSION(6, 16, 0)
    psr_data_t *psr_data = timer_container_of(psr_data,t,psr_timer);
#elif DRM_VERSION_CODE >= KERNEL_VERSION(4, 15, 0)
    psr_data_t *psr_data = from_timer(psr_data,t,psr_timer);
#else
    psr_data_t *psr_data = (psr_data_t *)data;
#endif

    if(psr_data && psr_data->enabled)
    {
        atomic_or(PSR_OP_TRANS_TO_ACTIVE, &psr_data->operation);
        zx_thread_wake_up(psr_data->psr_event);
    }
}

int zx_psr_work_thread(void *data)
{
    psr_data_t *psr_data = (psr_data_t *)data;
    int ret = 0;

    zx_set_freezable();

    do {
        ret = zx_thread_wait(psr_data->psr_event, 0);

        psr_operate_trans_state(psr_data);

        if(zx_freezing())
        {
            psr_disable(psr_data);
        }

        zx_try_to_freeze();
        
    } while(!zx_thread_should_stop());

    return 0;
}

