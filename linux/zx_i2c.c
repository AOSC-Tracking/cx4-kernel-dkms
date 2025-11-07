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
#include "zx_cbios.h"
#include "os_interface.h"
#include "zx_i2c.h"

static int zx_i2c_xfer(struct i2c_adapter *i2c_adapter, struct i2c_msg *msgs, int num)
{
    struct zx_i2c_adapter *zx_adapter = i2c_get_adapdata(i2c_adapter);
    struct drm_device *dev = zx_adapter->dev;
    zx_connector_t *zx_connector = (zx_connector_t*)(zx_adapter->pzx_connector);
    zx_card_t *zx_card = dev->dev_private;
    disp_info_t *disp_info = (disp_info_t *)zx_card->disp_info;
    zx_xfer_msg_t msg_param;
    int i = 0, retval = 0, i2c_over_aux = 0;

    if (zx_connector->base_connector.status != connector_status_connected)
    {
        DRM_DEBUG_DRIVER("invalid i2c request as connector(0x%x) is not connected",
                         zx_connector->output_type);
        return -EIO;
    }

    if ((zx_connector->monitor_type == CBIOS_MONITOR_TYPE_DP) ||
        (zx_connector->monitor_type == CBIOS_MONITOR_TYPE_PANEL))
    {
        i2c_over_aux = 1;
    }

    for (i = 0; i < num; i++)
    {
        zx_memset(&msg_param, 0, sizeof(zx_xfer_msg_t));
        msg_param.device_id = zx_connector->output_type;
        msg_param.addr = ((msgs[i].addr)<<1);
        msg_param.offset = 0;
        msg_param.buf = msgs[i].buf;
        msg_param.buf_len = msgs[i].len;

        msg_param.method = i2c_over_aux ? ZX_MSG_METHOD_I2C_OVER_AUX : ZX_MSG_METHOD_I2C;

        if ((msg_param.addr == 0x6E) || (msg_param.addr == 0x60)) //DDC operation
        {
            msg_param.is_ddcci = 1;
        }

        if(msg_param.addr == 0xA4)         // A4 customer operation panel firmware
        {
            msg_param.customer_op = 1;
        }

        if (msgs[i].flags & I2C_M_RD)
        {
            msg_param.op = ZX_MSG_OP_READ;
        }
        else
        {
            msg_param.op = ZX_MSG_OP_WRITE;
        }

        if (msgs[i].flags & I2C_M_STOP)
        {
            msg_param.i2c_stop = 1;
        }

        // if msg operation is write, but not used to adjust brightness or write segment ,just skip it
        if (msg_param.op == ZX_MSG_OP_WRITE && !(msg_param.is_ddcci || msg_param.customer_op))
        {
            continue;
        }

        zx_mutex_lock(zx_connector->access_lock);

        retval = disp_cbios_msg_xfer(disp_info, &msg_param);

        zx_mutex_unlock(zx_connector->access_lock);

        if (retval < 0)
        {
            return retval;
        }
    }

    return num;
}

static u32 zx_i2c_func(struct i2c_adapter *adapter)
{
    return I2C_FUNC_I2C | I2C_FUNC_SMBUS_EMUL;
}

static const struct i2c_algorithm zx_i2c_algo =
{
    .master_xfer = zx_i2c_xfer,
    .functionality = zx_i2c_func,
};

struct zx_i2c_adapter *zx_i2c_adapter_create(struct drm_device *dev, struct drm_connector *connector)
{
    struct i2c_adapter *adapter = NULL;
    struct zx_i2c_adapter *zx_adapter = NULL;
    zx_connector_t *zx_connector = to_zx_connector(connector);
    char *name = "CRT";
    int ret = 0, skip_create = 0;

    switch (zx_connector->output_type)
    {
    case DISP_OUTPUT_CRT:
    {
        name = "CRT";
    }
    break;
    case DISP_OUTPUT_DP1:
    {
        name = "DP1";
    }
    break;
    case DISP_OUTPUT_DP2:
    {
        name = "DP2";
    }
    break;
    case DISP_OUTPUT_DP3:
    {
        name = "DP3";
    }
    break;
    default:
    {
        skip_create = 1;
        DRM_DEBUG_KMS("skip create i2c adapter for output 0x%x\n", zx_connector->output_type);
    }
    }

    if (skip_create)
    {
        return NULL;
    }

    zx_adapter = zx_calloc(sizeof(struct zx_i2c_adapter));
    if (!zx_adapter)
    {
        DRM_ERROR("failed to allo zx_adapter\n");
        return ERR_PTR(-ENOMEM);
    }

    adapter = &zx_adapter->adapter;
    adapter->owner = THIS_MODULE;
#if DRM_VERSION_CODE < KERNEL_VERSION(6,8,0)
    adapter->class = I2C_CLASS_DDC;
#endif
    adapter->dev.parent = dev->dev;
    zx_adapter->dev = dev;
    zx_adapter->pzx_connector = (void *)zx_connector;
    i2c_set_adapdata(adapter, zx_adapter);

    zx_vsnprintf(adapter->name, sizeof(adapter->name), "zx_i2c_%s", name);

    adapter->algo = &zx_i2c_algo;

    ret = i2c_add_adapter(adapter);

    if (ret)
    {
        DRM_ERROR("failed to register i2c\n");
        zx_free(zx_adapter);
        zx_adapter = NULL;
        return ERR_PTR(ret);
    }

    zx_connector->i2c_adapter = zx_adapter;

    return zx_adapter;
}

void zx_i2c_adapter_destroy(struct zx_i2c_adapter *zx_adapter)
{
    if (!zx_adapter)
    {
        return;
    }
    i2c_del_adapter(&zx_adapter->adapter);
    zx_free(zx_adapter);
}
