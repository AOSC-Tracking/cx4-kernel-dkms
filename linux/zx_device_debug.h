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

#ifndef __ZX_DEVICE_DEUBG_H__
#define __ZX_DEVICE_DEUBG_H__
typedef struct zx_device_debug_info {
    struct dentry               *dentry;    //the device root directly
    struct dentry               *info;
    struct dentry               *vm_info;
    struct dentry               *vm_zone_info;
    void                        *debugfs_dev; //the zx root device
    int                         id;
    char                        name[20];
    struct list_head            list_item;
    unsigned int                hDevice;
    unsigned long               user_pid;
    unsigned int                resident_cnt;
    unsigned int                resident_list_cache_hit;
    unsigned int                resident_list_cache_miss;
}zx_device_debug_info_t;

#endif

