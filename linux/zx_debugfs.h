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
#ifndef __ZX_DEUBGFS_H__
#define __ZX_DEUBGFS_H__

#include "zx.h"
#include "zx_driver.h"
#include "zx_device_debug.h"

#define DEBUGFS_ESCAPE_CREATE   1
#define DEBUGFS_ZX_CREATE      2

#define DEBUGFS_HEAP_NUM        4
#define DEBUGFS_MEMTRACK        (DEBUGFS_HEAP_NUM + 1)

struct zx_debugfs_device;

typedef struct zx_debugfs_node
{
    int                         type;
    void                        *priv_data;
    struct zx_debugfs_device    *dev;
    int                         id;
    char                        name[20];
    struct dentry               *node_dentry;
    int                         create_hint;
    struct list_head            list_item;
    unsigned int                hDevice;
}zx_debugfs_node_t;

struct zx_debugfs_device;
typedef struct zx_debugfs_mmio
{
    struct dentry                  *mmio_root;
    struct zx_debugfs_node         regs;
    struct zx_debugfs_node         info;
    struct zx_debugfs_device       *debug_dev;
}zx_debugfs_mmio_t;

typedef struct zx_heap_info
{
    struct zx_debugfs_node         heap[DEBUGFS_HEAP_NUM];
    struct dentry                  *heap_dirs[DEBUGFS_HEAP_NUM];
    struct dentry                  *heap_dir;
}zx_heap_info_t;

typedef  struct zx_debugfs_crtcs
{
    struct zx_debugfs_device*  debug_dev;
    struct dentry*             crtcs_root;
    struct zx_debugfs_node*    crtcs_nodes;
    int                        node_num;
}zx_debugfs_crtcs_t;

typedef struct zx_debugfs_conn
{
    struct zx_debugfs_device*  debug_dev;
    struct dentry*             conn_root;
    struct dentry**            conn_dirs;
    struct zx_debugfs_node**   conn_nodes;
    int                        node_num;
}zx_debugfs_conn_t;

typedef struct zx_debugfs_device
{
    zx_card_t                      *zx;
    struct list_head                node_list;

    struct dentry                   *device_root;
    struct list_head                device_node_list;

    struct dentry                   *allocation_root;

    zx_heap_info_t                 heap_info;

    struct zx_debugfs_node         info;
    struct zx_debugfs_node         memtrack;
    struct zx_debugfs_node         vidsch;
    struct zx_debugfs_node         displayinfo;
    struct zx_debugfs_node         psr;
    struct zx_debugfs_node         debugbus;
    struct zx_debugfs_node         umd_trace;
    struct zx_debugfs_node         test_mem;
    struct dentry                   *debug_root;
    struct os_mutex                 *lock;
    zx_debugfs_mmio_t              mmio;
    zx_debugfs_crtcs_t             crtcs;
    zx_debugfs_conn_t              connectors;
}zx_debugfs_device_t;


zx_debugfs_device_t* zx_debugfs_create(zx_card_t *zx, struct dentry *minor_root);
int zx_debugfs_destroy(zx_debugfs_device_t* dev);
zx_device_debug_info_t* zx_debugfs_add_device_node(zx_debugfs_device_t* dev, int id, unsigned int handle);
int zx_debugfs_remove_device_node(zx_debugfs_device_t* dev, zx_device_debug_info_t *dnode);
int zx_debugfs_init(zx_card_t *zx);

static __inline__ struct dentry *zx_debugfs_get_allocation_root(void *debugfs_dev)
{
    zx_debugfs_device_t *dev =  (zx_debugfs_device_t *)debugfs_dev;
    return dev->allocation_root;
}
#endif


