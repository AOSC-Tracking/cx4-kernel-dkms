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
#include "zx_adapter.h"
#include "vidsch.h"
#include "vidmm.h"
#include "context.h"
#include "global.h"

void pm_save_state(adapter_t *adapter, int need_save_memory)
{
    cm_save(adapter, need_save_memory);

    vidsch_save(adapter);

    vidmm_save(adapter);

    /* system will suspend, set our card mode as uninitialize */
}


//temp use only, will remove after resume stable.
static inline void util_print_time(char *info)
{
    long time_sec = 0, time_usec = 0;

    zx_getsecs(&time_sec, &time_usec);
    zx_info("%s %ld(ms)\n",info,(time_sec * 1000 + zx_do_div(time_usec, 1000)));
}

int pm_restore_state(adapter_t *adapter)
{
    int ret = S_OK;

    ++adapter->resume_age;

    //temp use only, will remove all this function called after resume stable.
    util_print_time("pm_restore_state enter, cur time");

    glb_init_chip_interface(adapter);
    util_print_time("glb_init_chip_interface finish, cur time");

    if(ret != S_OK)
    {
        zx_error("dispmgr restore error\n");
        return ret;
    }

    vidmm_restore(adapter);
    util_print_time("vidmm_restore finish, cur time");

    vidsch_restore(adapter);
    util_print_time("vidsch_restore finish, cur time");

    cm_restore(adapter);
    util_print_time("cm_restore finish, cur time");

    vidsch_restore_dvfs(adapter);
    util_print_time("restore_dvfs finish, cur time");

    return ret;
}
