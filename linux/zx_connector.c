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
#include "zx_irq.h"
#include "zx_i2c.h"
#include "zx_sink.h"
#include "zx_drm_helper.h"

zx_connector_t* zx_get_connector_by_device_id(disp_info_t*  disp_info, int device_id)
{
    zx_card_t *zx_card = (zx_card_t *)disp_info->zx_card;
    struct drm_device *drm = zx_card->drm_dev;
    zx_connector_t* zx_conn = NULL;
    struct drm_connector *connector = NULL;

    if(!drm || !device_id)
    {
        return zx_conn;
    }

    list_for_each_entry(connector, &drm->mode_config.connector_list, head)
    {
        if((to_zx_connector(connector))->output_type == device_id)
        {
            zx_conn = to_zx_connector(connector);
            break;
        }
    }

    return zx_conn;
}

static enum drm_connector_status
zx_connector_detect_internal(disp_info_t *disp_info, zx_connector_t *zx_connector, int full_detect, int* edid_changed)
{
    disp_output_type output = zx_connector->output_type;
    int ret = 0, output_chg = 0, conflict_status = 0;
    unsigned char* edid = NULL;
    enum drm_connector_status conn_status;
    struct zx_sink_create_data sink_create_data = {0};
    struct drm_device *dev = zx_connector->base_connector.dev;
    zx_card_t *zx_card = (zx_card_t *)disp_info->zx_card;
    int drm_edid_overrd = 0, edid_len = 0;

#if ENABLE_DP3_VGA_EXCLUSIVE
    struct drm_connector* tmp_conn = NULL;
#endif

#if DRM_VERSION_CODE < KERNEL_VERSION(6, 2, 0)
    if(zx_connector->base_connector.override_edid)
    {
#else
    if(zx_connector->base_connector.edid_override)
    {
        drm_edid_connector_update(&zx_connector->base_connector, zx_connector->base_connector.edid_override);
#endif
        drm_edid_overrd = 1;
        if(zx_connector->base_connector.edid_blob_ptr)
        {
            edid = zx_connector->base_connector.edid_blob_ptr->data;
            edid_len = zx_connector->base_connector.edid_blob_ptr->length;
        }
    }
    else
    {
        drm_edid_overrd = 0;
    }

    if(!drm_edid_overrd && (zx_connector->edid_overrd == 1))
    {
        disp_cbios_set_edid(disp_info, output, NULL, 0);
        zx_connector->edid_overrd = 0;
        zx_info("Disable override edid for output 0x%x\n", output);
    }
    else if(drm_edid_overrd && (zx_connector->edid_overrd == 0))
    {
        disp_cbios_set_edid(disp_info, output, edid, edid_len);
        zx_connector->edid_overrd = 1;
        zx_info("Enable override edid for output 0x%x\n", output);
    }

    edid = NULL;
    edid_len = 0;

    zx_mutex_lock(zx_connector->access_lock);
    ret = disp_cbios_detect_output_status(disp_info, output, full_detect, &output_chg);
    zx_mutex_unlock(zx_connector->access_lock);

    conn_status = (ret)? connector_status_connected : connector_status_disconnected;

#if ENABLE_DP3_VGA_EXCLUSIVE
    if(disp_info->iga3_conflict && zx_connector->output_type == DISP_OUTPUT_CRT && conn_status == connector_status_connected)
    {
        list_for_each_entry(tmp_conn, &dev->mode_config.connector_list, head)
        {
            if(to_zx_connector(tmp_conn)->output_type == DISP_OUTPUT_DP3 &&
               tmp_conn->status == connector_status_connected)
            {
                conn_status = connector_status_disconnected;
                conflict_status = 1;
                break;
            }
        }
    }
#endif
    if(zx_card->output_conflict_poll)
    {
       if(conflict_status)
       {
            zx_card->output_conflict_poll->conflict_status |= DP3_CRT_CONFLICT;
       }
       else
       {
            zx_card->output_conflict_poll->conflict_status &= (~DP3_CRT_CONFLICT);
       }
    }


    if(conn_status == connector_status_connected)
    {
        if(zx_connector->base_connector.status != connector_status_connected || output_chg)
        {
            edid = disp_cbios_read_edid(disp_info, output, &edid_len);

            if (zx_connector->sink)
            {
                zx_sink_put(zx_connector->sink);
                zx_connector->sink = NULL;
            }

            sink_create_data.output_type = zx_connector->output_type;
            zx_connector->sink = zx_sink_create(&sink_create_data);
            zx_connector->sink->edid_data = edid;
            zx_connector->sink->edid_len = edid_len;

            disp_cbios_get_connector_attrib(disp_info, zx_connector);
        }
    }
    else
    {
        if(zx_connector->sink)
        {
            zx_sink_put(zx_connector->sink);
            zx_connector->sink = NULL;
        }

        if(zx_connector->edid_overrd)
        {
#if DRM_VERSION_CODE >= KERNEL_VERSION(6, 2, 0)   
            drm_edid_connector_update(&zx_connector->base_connector, NULL);
#endif
            disp_cbios_set_edid(disp_info, output, NULL, 0);
            zx_connector->edid_overrd = 0;
        }

        zx_connector->monitor_type = UT_OUTPUT_TYPE_NONE;
        zx_connector->support_audio = 0;
        zx_connector->support_psr  = 0;
    }

    if(edid_changed)
    {
        *edid_changed = output_chg;
    }

    return conn_status;
}

static void zx_restore_connector_state(struct  drm_connector* connector)
{
#if DRM_VERSION_CODE >=  KERNEL_VERSION(4, 8, 0)
    struct drm_modeset_acquire_ctx ctx;
    struct drm_device*  drm = NULL;
    int ret = 0;

    if(!connector)
    {
        return;
    }

    drm = connector->dev;

    drm_modeset_acquire_init(&ctx, 0);
    while (1)
    {
        ret = drm_modeset_lock_all_ctx(drm, &ctx);
        if (ret != -EDEADLK)
        {
            break;
        }
        drm_modeset_backoff(&ctx);
    }

    if (!ret)
    {
        zx_restore_drm_connector_state(drm, connector, &ctx);
    }

    drm_modeset_drop_locks(&ctx);
    drm_modeset_acquire_fini(&ctx);
#endif
}

//outp_masks: outputs that want to detect
//hpd_detct: 1, called from hpd interrupt, 0, from polling thread or OS poll
//notify_os: if any change, whether to notify OS.
void zx_detect_and_update_connectors(disp_info_t*  disp_info, int outp_masks, int hpd_detect,  int notify_os)
{
    zx_card_t*  zx_card = disp_info->zx_card;
    struct drm_device*  drm = zx_card->drm_dev;
    struct drm_mode_config *mode_config = &drm->mode_config;
    struct  drm_connector* connector = NULL;
    zx_connector_t*  zx_connector = NULL;
    unsigned int changed = 0;
    ZX_DEFINE_DRM_CONN_ITER(conn_iter);

    if(!outp_masks)
    {
        return;
    }

    mutex_lock(&mode_config->mutex);

    zx_drm_connector_list_iter_begin(drm, &conn_iter);

    zx_drm_for_each_connector(connector, drm, &conn_iter)
    {
        int conn_change = 0;
        zx_connector =  to_zx_connector(connector);

        if(!(zx_connector->output_type & outp_masks))
        {
            continue;
        }

        zx_connector_detect_locked(connector, hpd_detect, &conn_change);
        if(conn_change)
        {
            changed = 1;
        }
    }

    zx_drm_connector_list_iter_end(&conn_iter);

    mutex_unlock(&mode_config->mutex);

    if(changed && notify_os)
    {
        drm_kms_helper_hotplug_event(drm);
    }
}

enum drm_connector_status
zx_connector_detect_locked(struct drm_connector *connector, int hpd_detect, int *changed)
{
    struct drm_device *dev = NULL;
    zx_card_t *zx_card = NULL;
    disp_info_t *disp_info = NULL;
    enum drm_connector_status old_status;
    zx_connector_t* zx_connector = NULL;
    int plug_out = 0, plug_in = 0, edid_changed = 0, old_conflict_status = NO_CONFLICT;
    unsigned long irq;

    if(!connector)
    {
        if(changed)
        {
            *changed = 0;
        }
        return connector_status_disconnected;
    }

#if ENABLE_RUNTIME_PM
    if (!zx_kms_helper_is_poll_worker())
    {
        if(pm_runtime_get_sync(connector->dev->dev) < 0)
        {
            pm_runtime_put_autosuspend(connector->dev->dev);
            return connector_status_disconnected;
        }
    }
#endif

    dev = connector->dev;
    zx_card = dev->dev_private;
    disp_info = (disp_info_t *)zx_card->disp_info;

    zx_connector = to_zx_connector(connector);
    old_status = connector->status;

    if(disp_info->iga3_conflict && zx_card->output_conflict_poll)
    {
        old_conflict_status = zx_card->output_conflict_poll->conflict_status;
    }
    //edp's detect requires time-consuming sequence when it start from power-off status, and generally edp is a
    //physically connect device, so skip detect to save time when connector->status is connector_status_connected
    if(zx_connector->conn_type == CBIOS_EDP_CONN && connector->status == connector_status_connected)
    {
        connector->status = connector_status_connected;
    }
    else
    {
        connector->status = zx_connector_detect_internal(disp_info, zx_connector, hpd_detect, &edid_changed);
    }

    if(connector_status_connected == old_status && connector_status_disconnected == connector->status)
    {
        plug_out = 1;
    }
    else if(connector_status_disconnected == old_status && connector_status_connected == connector->status)
    {
        plug_in = 1;
    }

    if(disp_info->iga3_conflict && zx_card->output_conflict_poll && zx_card->output_conflict_poll->conflict_status != old_conflict_status)
    {
        zx_update_conflict_event_property(disp_info, zx_card->output_conflict_poll->conflict_status);
        if(zx_card->output_conflict_poll->conflict_status != NO_CONFLICT)
        {
            zx_info("##### VGA & DP3 output conflict!!! #####\n");
            zx_card->output_conflict_poll->event_status = true;
            wake_up(&zx_card->output_conflict_poll->queue->wait_queue);
        }
    }

    if(plug_out || plug_in)
    {
        atomic_set(&zx_connector->polling_time, 0);
    }

#if ENABLE_RUNTIME_PM
    if (!zx_kms_helper_is_poll_worker())
    {
        pm_runtime_mark_last_busy(connector->dev->dev);
        pm_runtime_put_autosuspend(connector->dev->dev);
    }
#endif

#if DRM_VERSION_CODE >= KERNEL_VERSION(4, 10, 0)
    if(hpd_detect && (plug_in || edid_changed))
    {
        zx_restore_connector_state(connector);
    }
#endif

    if(plug_out)
    {
        zx_info("**** Hot plug detected by %s: plug_out : 0x%x.****\n", (hpd_detect)? "interrupt" : "polling", zx_connector->output_type);
    }
    else if(plug_in)
    {
        zx_info("**** Hot plug detected by %s: plug_in : 0x%x.****\n", (hpd_detect)? "interrupt" : "polling", zx_connector->output_type);
    }
    else if(edid_changed)
    {
        zx_info("**** Hot plug detected by %s: change : 0x%x.****\n", (hpd_detect)? "interrupt" : "polling", zx_connector->output_type);
    }

    if(changed)
    {
        *changed = (plug_out || plug_in || edid_changed)? 1 : 0;
    }

    return connector->status;
}

static enum drm_connector_status
zx_connector_detect(struct drm_connector *connector, bool force)
{
    struct drm_device *dev = connector->dev;
    zx_card_t *zx_card = dev->dev_private;
    disp_info_t *disp_info = (disp_info_t *)zx_card->disp_info;
    zx_connector_t* zx_conn = to_zx_connector(connector);
    enum drm_connector_status  status;

    if(disp_info->worker_detect == 0)
    {
        status = zx_connector_detect_locked(connector, 0, NULL);
    }
    else
    {
        if(connector->status != connector_status_connected &&
            connector->status != connector_status_disconnected)
        {
            status = zx_connector_detect_locked(connector, 0, NULL);
        }
        else
        {
            if(connector->polled == DRM_CONNECTOR_POLL_HPD)
            {
                if(atomic_cmpxchg(&zx_conn->polling_time, 0, 1) == 0)
                {
                    if(!(disp_info->poll_status & POLL_KEEP_RUNNING))
                    {
                        schedule_delayed_work(&dev->mode_config.output_poll_work, OUTPUT_POLL_PERIOD);
                    }
                }
            }

            status = connector->status;
        }
    }

    return status;
}


static int zx_connector_get_modes(struct drm_connector *connector)
{
    struct drm_device *dev = connector->dev;
    zx_card_t *zx_card = dev->dev_private;
    disp_info_t *disp_info = (disp_info_t *)zx_card->disp_info;
    zx_connector_t *zx_connector = to_zx_connector(connector);
    int dev_mode_size = 0, adapter_mode_size = 0, dev_real_num = 0, adapter_real_num = 0, real_num = 0, added_num = 0;
    int i = 0, skip_create = 0;
    void* mode_buf = NULL;
    void* adapter_mode_buf = NULL;
    void* merge_mode_buf = NULL;
    void* cb_mode_list = NULL;
    int  output = zx_connector->output_type;
    struct drm_display_mode *drm_mode = NULL;
    struct edid *drm_edid = NULL;
    int edid_len = 0,  update_edid = 1;

    dev_mode_size = disp_cbios_get_modes_size(disp_info, output);

    if(!dev_mode_size)
    {
        goto END;
    }

    mode_buf = zx_calloc(dev_mode_size);

    if(!mode_buf)
    {
        goto END;
    }

    if(zx_connector->sink)
    {
        drm_edid = (struct edid*)zx_connector->sink->edid_data;
        edid_len = zx_connector->sink->edid_len;
    }

#if DRM_VERSION_CODE >= KERNEL_VERSION(6, 2, 0)
    if(connector->edid_override)
    {
        update_edid = 0;
    }
#endif

    if(update_edid)
    {
        zx_drm_connector_update_edid_property(connector, drm_edid, edid_len);
    }

    dev_real_num = disp_cbios_get_modes(disp_info, output, mode_buf, dev_mode_size);

    cb_mode_list = mode_buf;
    real_num = dev_real_num;

    if(disp_info->scale_support)
    {
        adapter_mode_size = disp_cbios_get_adapter_modes_size(disp_info);

        if(adapter_mode_size != 0)
        {
            adapter_mode_buf = zx_calloc(adapter_mode_size);

            if(!adapter_mode_buf)
            {
                goto END;
            }
            adapter_real_num = disp_cbios_get_adapter_modes(disp_info, adapter_mode_buf, adapter_mode_size);

            merge_mode_buf = zx_calloc((dev_real_num + adapter_real_num) * sizeof(CBiosModeInfoExt));

            if(!merge_mode_buf)
            {
                goto END;
            }

            real_num = disp_cbios_merge_modes(merge_mode_buf, adapter_mode_buf, adapter_real_num, mode_buf, dev_real_num);
            cb_mode_list = merge_mode_buf;
        }
    }

    for (i = 0; i < real_num; i++)
    {
        if(!skip_create)
        {
            drm_mode = drm_mode_create(dev);
        }
        if(!drm_mode)
        {
            skip_create = 0;
            break;
        }
        if(DISP_OK != disp_cbios_cbmode_to_drmmode(disp_info, output, cb_mode_list, i, drm_mode))
        {
            skip_create = 1;
            continue;
        }

        skip_create = 0;
        drm_mode_set_name(drm_mode);
        drm_mode_probed_add(connector, drm_mode);
        added_num++;
    }

    zx_free(mode_buf);
    mode_buf = NULL;

/*    dev_mode_size = disp_cbios_get_3dmode_size(disp_info, output);
    mode_buf = zx_calloc(dev_mode_size);
    if(!mode_buf)
    {
        goto END;
    }

    real_num = disp_cbios_get_3dmodes(disp_info, output, mode_buf, dev_mode_size);

    for(i = 0; i < real_num; i++)
    {
        if(!skip_create)
        {
            drm_mode = drm_mode_create(dev);
        }
        if(!drm_mode)
        {
            skip_create = 0;
            break;
        }
        if(DISP_OK != disp_cbios_3dmode_to_drmmode(disp_info, output, mode_buf, i, drm_mode))
        {
            skip_create = 1;
            continue;
        }

        skip_create = 0;
        drm_mode_set_name(drm_mode);
        drm_mode_probed_add(connector, drm_mode);
        added_num++;
    }*/

END:

    if(skip_create && drm_mode)
    {
        drm_mode_destroy(dev, drm_mode);
    }

    if(mode_buf)
    {
        zx_free(mode_buf);
        mode_buf = NULL;
    }

    if(adapter_mode_buf)
    {
        zx_free(adapter_mode_buf);
        adapter_mode_buf = NULL;
    }

    if(merge_mode_buf)
    {
        zx_free(merge_mode_buf);
        merge_mode_buf = NULL;
    }

    return added_num;
}

enum drm_mode_status
#if DRM_VERSION_CODE >= KERNEL_VERSION(6, 15, 0)
zx_connector_mode_valid(struct drm_connector *connector, const struct drm_display_mode *mode)
#else
zx_connector_mode_valid(struct drm_connector *connector, struct drm_display_mode *mode)
#endif
{
    struct drm_device *dev = connector->dev;
    zx_card_t *zx_card = dev->dev_private;
    disp_info_t *disp_info = (disp_info_t *)zx_card->disp_info;
    adapter_info_t *adp_info = disp_info->adp_info;
    zx_connector_t *zx_connector = to_zx_connector(connector);
    int max_clock;

    if (adp_info->chip_id >= CHIP_CHX004)
    {
        if (zx_connector->output_type == DISP_OUTPUT_CRT)
        {
            max_clock = 400000;    // 400 MHz
        }
        else
        {
            if((adp_info->sub_sys_vendor_id == 0x17AA) && (adp_info->sub_sys_id == 0x350B)
                && (zx_connector->output_type == DISP_OUTPUT_DP1))
            {
                max_clock = 700000;    // 700 MHz
            }
            else
            {
                max_clock = 600000;    // 600 MHz
            }
        }
    }
    else
    {
        // default value
        max_clock = 300000;    // 300 MHz
    }

    if (mode->clock > max_clock)
    {
        return MODE_CLOCK_HIGH;
    }

    return MODE_OK;
}

static void zx_connector_destroy(struct drm_connector *connector)
{
    zx_connector_t *zx_connector = to_zx_connector(connector);

    drm_connector_cleanup(connector);

    if (zx_connector->priv_data)
    {
        zx_free(zx_connector->priv_data);
        zx_connector->priv_data = NULL;
    }

    zx_sink_put(zx_connector->sink);
    zx_connector->sink = NULL;

    zx_i2c_adapter_destroy(zx_connector->i2c_adapter);
    zx_connector->i2c_adapter= NULL;

    zx_destroy_mutex(zx_connector->access_lock);
    zx_connector->access_lock = NULL;

    zx_free(zx_connector);
}

#if DRM_VERSION_CODE >= KERNEL_VERSION(4, 3, 0)
static int zx_connector_dpms(struct drm_connector *connector, int mode)
#else
static void zx_connector_dpms(struct drm_connector *connector, int mode)
#endif
{
    if(connector->encoder)
    {
        if(mode == DRM_MODE_DPMS_ON)
        {
            zx_encoder_enable(connector->encoder);
        }
        else
        {
            zx_encoder_disable(connector->encoder);
        }

        connector->dpms = mode;
    }

#if DRM_VERSION_CODE >= KERNEL_VERSION(4, 3, 0)
    return 0;
#endif
}

#if DRM_VERSION_CODE < KERNEL_VERSION(4, 8, 0)
//in zx chip, connector is fixed to encoder, so we just pick up the 1st encoder from table
static struct drm_encoder *zx_best_encoder(struct drm_connector *connector)
{
    struct drm_mode_object* obj = NULL;
    int encoder_id = connector->encoder_ids[0];

    obj = drm_mode_object_find(connector->dev, encoder_id, DRM_MODE_OBJECT_ENCODER);

    return  obj_to_encoder(obj);
}

#else

static struct drm_connector_state* zx_connector_duplicate_state(struct drm_connector *connector)
{
    zx_connector_state_t* zx_conn_state;

    zx_conn_state = zx_calloc(sizeof(zx_connector_state_t));
    if (!zx_conn_state)
    {
        return NULL;
    }

    __drm_atomic_helper_connector_duplicate_state(connector, &zx_conn_state->base_conn_state);

    return &zx_conn_state->base_conn_state;
}

static void zx_connector_destroy_state(struct drm_connector *connector, struct drm_connector_state *state)
{
    zx_connector_state_t *zx_conn_state = to_zx_conn_state(state);
    __drm_atomic_helper_connector_destroy_state(state);
    zx_free(zx_conn_state);
}

#endif

static int zx_get_conn_type(disp_info_t *disp_info, zx_connector_t *zx_connector)
{
    adapter_info_t* adapter_info = disp_info->adp_info;
    int drm_conn_type = DRM_MODE_CONNECTOR_Unknown;
    int cb_conn_type = CBIOS_NON_CONN;
    int output = zx_connector->output_type;

    zx_connector->conn_type = disp_cbios_get_port_attri(disp_info, output);

    switch (output)
    {
    case DISP_OUTPUT_CRT:
    {
        drm_conn_type = DRM_MODE_CONNECTOR_VGA;
    }
    break;
    case DISP_OUTPUT_DP1:
    case DISP_OUTPUT_DP2:
    case DISP_OUTPUT_DP3:
    {
        drm_conn_type = DRM_MODE_CONNECTOR_DisplayPort;

        if(zx_connector->conn_type == CBIOS_DVI_CONN)
        {
            drm_conn_type = DRM_MODE_CONNECTOR_DVID;
        }
        else if(zx_connector->conn_type == CBIOS_HDMI_CONN)
        {
            drm_conn_type = DRM_MODE_CONNECTOR_HDMIA;
        }
        else if(zx_connector->conn_type == CBIOS_EDP_CONN)
        {
            drm_conn_type = DRM_MODE_CONNECTOR_eDP;
        }
    }
    break;
    default:
    {
        DRM_ERROR("unkown drm connector type!\n");
    }
    break;
    }

    return drm_conn_type;
}

#if DRM_VERSION_CODE >= KERNEL_VERSION(4, 19, 0)

static ssize_t zx_dp_aux_transfer(struct drm_dp_aux *aux, struct drm_dp_aux_msg *msg)
{
    zx_dp_data_t *dp_data = container_of(aux, zx_dp_data_t, aux);
    zx_connector_t *zx_connector = to_zx_connector(dp_data->connector);
    struct drm_device *dev = dp_data->connector->dev;
    zx_card_t *zx_card = dev->dev_private;
    disp_info_t *disp_info = (disp_info_t *)zx_card->disp_info;
    zx_xfer_msg_t msg_param = {0};
    int ret;

    if ((zx_connector->monitor_type != CBIOS_MONITOR_TYPE_DP) &&
       (zx_connector->monitor_type != CBIOS_MONITOR_TYPE_PANEL))
    {
        DRM_DEBUG_DRIVER("only support dp and edp monitors");
        goto err;
    }

    if (zx_connector->base_connector.status != connector_status_connected)
    {
        DRM_DEBUG_DRIVER("invalid i2c request as connector(0x%x) is not connected",
                         zx_connector->output_type);
        goto err;
    }

    msg_param.device_id = zx_connector->output_type;

    if (msg->request & (DP_AUX_NATIVE_WRITE & DP_AUX_NATIVE_READ))
    {
        msg_param.method = ZX_MSG_METHOD_AUX;
    }
    else
    {
        // msg_param.method = ZX_MSG_METHOD_I2C_OVER_AUX;
        return -EIO;
    }

    if (msg_param.method == ZX_MSG_METHOD_AUX && msg->size < 1)
    {
        goto err;
    }

    switch (msg->request & ~DP_AUX_I2C_MOT)
    {
    case DP_AUX_I2C_WRITE:
    case DP_AUX_I2C_WRITE_STATUS_UPDATE:
    case DP_AUX_NATIVE_WRITE:
    {
        msg_param.op = ZX_MSG_OP_WRITE;
    }
    break;

    case DP_AUX_I2C_READ:
    case DP_AUX_NATIVE_READ:
    {
        msg_param.op = ZX_MSG_OP_READ;
    }
    break;

    default:
    {
        goto err;
    }
    break;
    }

    msg_param.addr = msg->address;
    msg_param.buf = msg->buffer;
    msg_param.buf_len = msg->size;

    zx_mutex_lock(zx_connector->access_lock);

    ret = disp_cbios_msg_xfer(disp_info, &msg_param);

    zx_mutex_unlock(zx_connector->access_lock);

    if (ret == DISP_OK)
    {
        return msg->size;
    }

err:
    msg->reply = DP_AUX_NATIVE_REPLY_NACK | DP_AUX_I2C_REPLY_NACK;

    return -EINVAL;
}

static void zx_init_dp_connector(struct drm_connector *connector)
{
    zx_connector_t *zx_connector = to_zx_connector(connector);
    zx_dp_data_t *dp_data = NULL;

    zx_connector->priv_data = zx_calloc(sizeof(zx_dp_data_t));
    if (zx_connector->priv_data == NULL)
    {
        DRM_ERROR("%s, out of mem.\n", __func__);
        zx_assert(0, "zx connector init dp connector");
    }

    dp_data = (zx_dp_data_t *)zx_connector->priv_data;

    dp_data->connector = connector;

    dp_data->aux.transfer = zx_dp_aux_transfer;

#if DRM_VERSION_CODE >= KERNEL_VERSION(5, 14, 0)
    dp_data->aux.drm_dev = connector->dev;
#endif

    drm_dp_aux_init(&dp_data->aux);
}

static int zx_connector_late_register(struct drm_connector *connector)
{
    zx_connector_t *zx_connector = to_zx_connector(connector);
    zx_dp_data_t *dp_data = NULL;
    int ret = 0;

    if (connector->connector_type == DRM_MODE_CONNECTOR_eDP ||
        connector->connector_type == DRM_MODE_CONNECTOR_DisplayPort)
    {
        dp_data = (zx_dp_data_t *)zx_connector->priv_data;

        dp_data->aux.dev = connector->kdev;
        ret = drm_dp_aux_register(&dp_data->aux);
    }

    return ret;
}

static void zx_connector_early_unregister(struct drm_connector *connector)
{
    zx_connector_t *zx_connector = to_zx_connector(connector);
    zx_dp_data_t *dp_data = NULL;

    if (connector->connector_type == DRM_MODE_CONNECTOR_eDP ||
        connector->connector_type == DRM_MODE_CONNECTOR_DisplayPort)
    {
        dp_data = (zx_dp_data_t *)zx_connector->priv_data;

        drm_dp_aux_unregister(&dp_data->aux);
    }
}

#endif

static const struct drm_connector_funcs zx_connector_funcs =
{
    .detect = zx_connector_detect,
    .dpms = zx_connector_dpms,
    .fill_modes = drm_helper_probe_single_connector_modes,
    .destroy = zx_connector_destroy,

#if DRM_VERSION_CODE >= KERNEL_VERSION(4, 19, 0)
    .late_register = zx_connector_late_register,
    .early_unregister = zx_connector_early_unregister,
#endif

#if DRM_VERSION_CODE >= KERNEL_VERSION(4, 8, 0)
    .atomic_destroy_state = zx_connector_destroy_state,
    .atomic_duplicate_state = zx_connector_duplicate_state,
#endif
};

static const struct drm_connector_helper_funcs zx_connector_helper_funcs =
{
    .get_modes = zx_connector_get_modes,
    .mode_valid = zx_connector_mode_valid,
#if DRM_VERSION_CODE < KERNEL_VERSION(4, 8, 0)
    .best_encoder = zx_best_encoder,
#endif
};

static void disp_create_connector_property(disp_info_t* disp_info, zx_connector_t* zx_connector)
{
    struct drm_property *primary_card_prop, *global_id_proph, *global_id_propl, *conn_conflict_event;
    zx_card_t*  zx_card = disp_info->zx_card;
    adapter_info_t*  adapter_info = disp_info->adp_info;
    unsigned int primary = 0, pci_id = 0, internal_id = 0; //pci_id = (vend << 16) | device, internal_id = (bdf << 16) + internal_output_id

    if(vga_default_device() == zx_card->pdev)
    {
        primary = 1;
    }

    pci_id = adapter_info->ven_dev;

#if DRM_VERSION_CODE >= KERNEL_VERSION(5, 2, 0)
    internal_id = (unsigned int)pci_dev_id(zx_card->pdev);
#else
    internal_id = (unsigned int)PCI_DEVID(zx_card->pdev->bus->number, zx_card->pdev->devfn);
#endif

    internal_id <<= 16;

    if(zx_connector->output_type >= 0x100)
    {
        internal_id |= zx_connector->output_type >> 8;
    }
    else
    {
        internal_id |= zx_connector->output_type;
    }

    zx_info("Create property for dev 0x%x, primary = %d, pci_id = 0x%x, internal_id = 0x%x.\n", zx_connector->output_type, primary, pci_id, internal_id);

    if(!disp_info->primary_card_prop)
    {
        primary_card_prop = drm_property_create_bool(zx_card->drm_dev, DRM_MODE_PROP_IMMUTABLE, "primarycard");
        if(primary_card_prop)
        {
            disp_info->primary_card_prop = primary_card_prop;
        }
    }

    if(disp_info->primary_card_prop)
    {
        drm_object_attach_property(&zx_connector->base_connector.base, disp_info->primary_card_prop, primary);
    }

    if(!disp_info->global_id_proph)
    {
        global_id_proph = drm_property_create_range(zx_card->drm_dev, DRM_MODE_PROP_IMMUTABLE, "global_id_h", 0, UINT_MAX);
        if(global_id_proph)
        {
            disp_info->global_id_proph = global_id_proph;
        }
    }

    if(!disp_info->global_id_propl)
    {
        global_id_propl = drm_property_create_range(zx_card->drm_dev, DRM_MODE_PROP_IMMUTABLE, "global_id_l", 0, UINT_MAX);
        if(global_id_propl)
        {
            disp_info->global_id_propl = global_id_propl;
        }
    }

    if(!disp_info->conn_conflict_event)
    {
        conn_conflict_event = drm_property_create_enum(zx_card->drm_dev, DRM_MODE_PROP_IMMUTABLE, "conn_conflict_event",
                                                        output_conflict_enum_list, ARRAY_SIZE(output_conflict_enum_list));
        if(conn_conflict_event)
        {
            disp_info->conn_conflict_event = conn_conflict_event;
        }
    }

    if(disp_info->global_id_proph)
    {
        drm_object_attach_property(&zx_connector->base_connector.base, disp_info->global_id_proph, pci_id);
    }

    if(disp_info->global_id_propl)
    {
        drm_object_attach_property(&zx_connector->base_connector.base, disp_info->global_id_propl, internal_id);
    }

    if(disp_info->conn_conflict_event)
    {
        drm_object_attach_property(&zx_connector->base_connector.base, disp_info->conn_conflict_event, NO_CONFLICT);
    }
}


void zx_find_connector_set_property(disp_info_t*  disp_info, int output_type, int conflict)
{
    zx_card_t*  zx_card = disp_info->zx_card;
    struct drm_device*  dev = zx_card->drm_dev;
    struct drm_connector* tmp_conn = NULL;
    list_for_each_entry(tmp_conn, &dev->mode_config.connector_list, head)
    {
        if(to_zx_connector(tmp_conn)->output_type == output_type)
        {
            drm_object_property_set_value(&tmp_conn->base, disp_info->conn_conflict_event, conflict);
            break;
        }
    }
}

void zx_update_conflict_event_property(disp_info_t* disp_info, int status)
{

    if (((status & DP1_USB_CONFLICT) == DP1_USB_CONFLICT))
    {
        zx_find_connector_set_property(disp_info, DISP_OUTPUT_DP1, DP1_USB_CONFLICT);
    }
    else
    {
        zx_find_connector_set_property(disp_info, DISP_OUTPUT_DP1, NO_CONFLICT);
    }

    if(((status & DP2_USB_CONFLICT) == DP2_USB_CONFLICT))
    {
        zx_find_connector_set_property(disp_info, DISP_OUTPUT_DP2, DP2_USB_CONFLICT);
    }
    else
    {
        zx_find_connector_set_property(disp_info, DISP_OUTPUT_DP2, NO_CONFLICT);
    }

    if (((status & DP3_CRT_CONFLICT) == DP3_CRT_CONFLICT))
    {
        zx_find_connector_set_property(disp_info, DISP_OUTPUT_CRT, DP3_CRT_CONFLICT);
        zx_find_connector_set_property(disp_info, DISP_OUTPUT_DP3, DP3_CRT_CONFLICT);
    }
    else
    {
        zx_find_connector_set_property(disp_info, DISP_OUTPUT_CRT, NO_CONFLICT);
        zx_find_connector_set_property(disp_info, DISP_OUTPUT_DP3, NO_CONFLICT);
    }
}

struct drm_connector* zx_drm_connector_create(disp_info_t* disp_info, disp_output_type output)
{
    zx_card_t*  zx_card = disp_info->zx_card;
    struct drm_device*  drm = zx_card->drm_dev;
    struct drm_connector*  connector = NULL;
    zx_connector_t* zx_connector = NULL;
    struct zx_i2c_adapter *zx_adapter = NULL;
#if DRM_VERSION_CODE >= KERNEL_VERSION(4, 8, 0)
    zx_connector_state_t* zx_conn_state = NULL;
#endif
    int ret = -ENOMEM;
    int drm_conn_type = 0;

    zx_connector = zx_calloc(sizeof(zx_connector_t));
    if (!zx_connector)
    {
        DRM_ERROR("failed to alloc zx connector\n");
        goto failed;
    }

#if DRM_VERSION_CODE >= KERNEL_VERSION(4, 8, 0)
    zx_conn_state = zx_calloc(sizeof(zx_connector_state_t));
    if (!zx_conn_state)
    {
        DRM_ERROR("failed to alloc zx connector state\n");
        goto failed;
    }

    zx_connector->base_connector.state = &zx_conn_state->base_conn_state;
    zx_conn_state->base_conn_state.connector = &zx_connector->base_connector;
#endif

    zx_connector->output_type = output;

    connector = &zx_connector->base_connector;
    connector->doublescan_allowed = FALSE;
    connector->stereo_allowed = FALSE;
    connector->interlace_allowed = FALSE;
    if (output & DISP_OUTPUT_DP_TYPES)
    {
        connector->stereo_allowed = TRUE;
        connector->interlace_allowed = TRUE;
    }

    drm_conn_type = zx_get_conn_type(disp_info, zx_connector);

    zx_connector->access_lock = zx_create_mutex();
    if(!zx_connector->access_lock)
    {
        DRM_ERROR("failed to alloc device lock for connector\n");
        goto failed;
    }

    zx_adapter = zx_i2c_adapter_create(drm, connector);
    if (IS_ERR(zx_adapter))
    {
        ret = PTR_ERR(zx_adapter);
        DRM_ERROR("failed to create zx i2c adapter\n");
        goto failed;
    }

#if DRM_VERSION_CODE >= KERNEL_VERSION(5, 4, 0)
    if(zx_adapter)
    {
        ret = drm_connector_init_with_ddc(drm, connector, &zx_connector_funcs, drm_conn_type, &zx_adapter->adapter);
    }
    else
    {
        ret = drm_connector_init(drm, connector, &zx_connector_funcs, drm_conn_type);
    }
#else
    ret = drm_connector_init(drm, connector, &zx_connector_funcs, drm_conn_type);
#endif
    if (ret != 0)
    {
        DRM_ERROR("failed to initialize drm connector\n");
        goto failed_init_connector;
    }

    drm_connector_helper_add(connector, &zx_connector_helper_funcs);

    if (output & disp_info->supp_polling_outputs)
    {
        connector->polled = DRM_CONNECTOR_POLL_CONNECT | DRM_CONNECTOR_POLL_DISCONNECT;
    }
    else if(output & disp_info->supp_hpd_outputs)
    {
        connector->polled = DRM_CONNECTOR_POLL_HPD;
        if(output == DISP_OUTPUT_DP1)
        {
            zx_connector->hpd_int_bit = INT_DP1;
            zx_connector->hda_codec_index = (1 << 0);
        }
        else if(output == DISP_OUTPUT_DP2)
        {
            zx_connector->hpd_int_bit = INT_DP2;
            zx_connector->hda_codec_index = (1 << 1);
        }
        else if(output == DISP_OUTPUT_DP3)
        {
            zx_connector->hpd_int_bit = INT_DP3;
            zx_connector->hda_codec_index = (1 << 2);
        }
        else if(output == DISP_OUTPUT_DP4)
        {
            zx_connector->hpd_int_bit = INT_DP4;
        }
    }

    atomic_set(&zx_connector->polling_time, 0);

    disp_create_connector_property(disp_info, zx_connector);

#if  DRM_VERSION_CODE >= KERNEL_VERSION(4, 19, 0)
    if (drm_conn_type == DRM_MODE_CONNECTOR_DisplayPort ||
        drm_conn_type == DRM_MODE_CONNECTOR_eDP)
    {
        zx_init_dp_connector(connector);
    }
#endif

    return connector;

failed_init_connector:
    zx_i2c_adapter_destroy(zx_adapter);
    zx_adapter = NULL;

failed:
#if  DRM_VERSION_CODE >= KERNEL_VERSION(4, 8, 0)
    if (zx_conn_state)
    {
        zx_free(zx_conn_state);
        zx_conn_state = NULL;
    }
#endif

    if (zx_connector)
    {
        if (zx_connector->access_lock)
        {
            zx_destroy_mutex(zx_connector->access_lock);
            zx_connector->access_lock = NULL;
        }

        zx_free(zx_connector);
        zx_connector = NULL;
    }

    return ERR_PTR(ret);
}

#if DRM_VERSION_CODE >= KERNEL_VERSION(4, 8, 0)

static void __zx_restore_drm_connector_state(struct drm_connector *connector,
                                             struct drm_modeset_acquire_ctx *ctx)
{
    int ret = 0;
    struct drm_device *dev = connector->dev;
    struct drm_atomic_state *state = drm_atomic_state_alloc(dev);
    struct drm_crtc *crtc = connector->encoder->crtc;
    struct drm_plane *plane = crtc->primary;
    struct drm_plane *cursor_plane = crtc->cursor;
    struct drm_connector_state *conn_state;
    struct drm_crtc_state *crtc_state;
    struct drm_plane_state *plane_state;
    zx_plane_state_t *zx_pstate = NULL;

    if (!state)
    {
        return;
    }

    state->acquire_ctx = ctx;

    conn_state = drm_atomic_get_connector_state(state, connector);
    if (IS_ERR(conn_state))
    {
        ret = PTR_ERR(conn_state);
        goto out;
    }

    crtc_state = drm_atomic_get_crtc_state(state, crtc);
    if (IS_ERR(crtc_state))
    {
        ret = PTR_ERR(crtc_state);
        goto out;
    }

    crtc_state->mode_changed = TRUE;

    plane_state = drm_atomic_get_plane_state(state, plane);
    if (IS_ERR(plane_state))
    {
        ret = PTR_ERR(plane_state);
        goto out;
    }

    zx_pstate = to_zx_plane_state(plane_state);
    zx_pstate->connector_restore = 1;

    plane_state = drm_atomic_get_plane_state(state, cursor_plane);
    if (IS_ERR(plane_state))
    {
        ret = PTR_ERR(plane_state);
        goto out;
    }

    zx_pstate = to_zx_plane_state(plane_state);
    zx_pstate->connector_restore = 1;

    ret = drm_atomic_commit(state);

out:
#if DRM_VERSION_CODE >= KERNEL_VERSION(4, 10, 0)
    drm_atomic_state_put(state);
#else
    drm_atomic_state_free(state);
#endif

    if (ret)
    {
        DRM_DEBUG_KMS("Restoring old connector state failed with %d\n", ret);
    }
}

void zx_restore_drm_connector_state(struct drm_device *dev, struct drm_connector *connector,
                                    struct drm_modeset_acquire_ctx *ctx)
{
  zx_connector_t *zx_connector = to_zx_connector(connector);
  struct drm_crtc *crtc = NULL;
  zx_crtc_state_t *zx_crtc_state = NULL;

  if (!zx_connector->sink || !connector->encoder)
  {
      return;
  }

  crtc = connector->encoder->crtc;
  if (!crtc)
  {
      return;
  }

  zx_crtc_state = to_zx_crtc_state(crtc->state);

  if (zx_crtc_state->sink != zx_connector->sink)
  {
      zx_info("To restore mode after plug in.\n");
      __zx_restore_drm_connector_state(connector, ctx);
  }
}

#endif
