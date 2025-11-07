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

#ifndef __ZX_I2C_H__
#define __ZX_I2C_H__

struct zx_i2c_adapter
{
    struct i2c_adapter adapter;
    struct drm_device *dev;
    void *pzx_connector;
};

struct zx_i2c_adapter *zx_i2c_adapter_create(struct drm_device *dev, struct drm_connector *connector);
void zx_i2c_adapter_destroy(struct zx_i2c_adapter *zx_adapter);

#endif
