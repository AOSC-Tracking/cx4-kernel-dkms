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
#include "vidschi.h"

/* must called sch_mgr->engine_lock locked */
void vidschi_engine_dvfs_power_on(vidsch_mgr_t *sch_mgr)
{
    adapter_t *adapter = sch_mgr->adapter;
    int ret = S_OK;

    if (!adapter->pwm_level.EnableClockGating || !adapter->pm_caps.pwm_manual)
    {
        return;
    }

    if(!sch_mgr->engine_dvfs_power_on)
    {
        if(!list_empty(&sch_mgr->submitted_task_list) || sch_mgr->last_send_fence_id != sch_mgr->returned_fence_id)
        {
            if(sch_mgr->chip_func->power_clock)
            {
                ret = sch_mgr->chip_func->power_clock(sch_mgr, FALSE);
                if (S_OK == ret)
                {
                    sch_mgr->engine_dvfs_power_on = TRUE;
                }
            }
        }
    }
}

void vidschi_try_power_tuning(adapter_t *adapter)
{
    vidsch_global_t  *sch_global = adapter->sch_global;

    if (!adapter->pwm_level.EnableClockGating || !adapter->pm_caps.pwm_manual || adapter->in_acpi_stage)
    {
        return;
    }

    if(sch_global->chip_func->power_tuning)
    {
        sch_global->chip_func->power_tuning(adapter);
    }
}


