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

#ifndef __ZX_GEM_DEUBG_H__
#define __ZX_GEM_DEUBG_H__
#include "zx_device_debug.h"

typedef struct zx_gem_debug_info {
    char                    name[20];    /*in*/
    struct dentry           *root;      /*in*/

    unsigned int            device;

    void                    *parent_gem;

    struct dentry          *self_dir;
    struct dentry          *alloc_info;


    struct dentry           *data;
    bool                    is_cpu_accessable;

    struct dentry           *control;
    bool                    mark_unpagable;

    struct dentry           *link;

    bool                    is_dma_buf_import;

    zx_gem_dbg_info_t       dbginfo;

    struct dentry          *vm_info;
}zx_gem_debug_info_t;



#endif

