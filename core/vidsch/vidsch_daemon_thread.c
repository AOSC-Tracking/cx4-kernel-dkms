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
#include "vidsch_submit.h"

int vidschi_reset_hw(adapter_t *adapter, unsigned int hang_engines)
{
    vidsch_global_t *sch_global = adapter->sch_global;
    vidsch_mgr_t    *sch_mgr    = adapter->sch_mgr[util_get_lsb_position(hang_engines)];
    unsigned int  i;
    int status;

    zx_disable_interrupt(adapter->os_device.pdev);

    status = sch_global->chip_func->reset_hw(sch_mgr);

    if(status == S_OK)
    {
        for (i = 0; i < MAX_ENGINE_COUNT; i++)
        {
            sch_mgr = adapter->sch_mgr[i];

            if(sch_mgr == NULL) continue;

            vidschi_set_uncompleted_task_dropped(sch_mgr, -1ll);
            vidsch_release_completed_tasks(sch_mgr, NULL);
        }

        zx_info("reset finished.\n");
    }

    if(sch_mgr != NULL)
    {
        sch_mgr->init_submit = TRUE;
    }

    zx_enable_interrupt(adapter->os_device.pdev);

    return S_OK;
}

void vidschi_dump_hang_info(adapter_t *adapter, unsigned int hang_engines)
{
    vidsch_mgr_t *sch_mgr = adapter->sch_mgr[util_get_lsb_position(hang_engines)];
    vidsch_global_t *sch_global = adapter->sch_global;

    if(sch_mgr != NULL && sch_global->chip_func->dump_hang_info != NULL)
    {
        sch_global->chip_func->dump_hang_info(sch_mgr);
    }
}

void vidschi_dump_hang(adapter_t *adapter, unsigned int hang_engines)
{
    vidsch_mgr_t *sch_mgr = adapter->sch_mgr[util_get_lsb_position(hang_engines)];
    vidsch_global_t *sch_global = adapter->sch_global;

    if(sch_mgr != NULL && sch_global->chip_func->dump_hang != NULL)
    {
        sch_global->chip_func->dump_hang(adapter);
    }
}

int vidschi_check_hang_and_recovery(adapter_t *adapter)
{
    vidsch_global_t *sch_global = adapter->sch_global;
    vidsch_mgr_t    *sch_mgr    = NULL;
    task_desc_t     *hang_tasks[MAX_ENGINE_COUNT] = {0, };

    unsigned int i, hang_engines = 0;

    for (i = 0; i < MAX_ENGINE_COUNT; i++)
    {
        sch_mgr       = adapter->sch_mgr[i];
        hang_tasks[i] = NULL;

        if(sch_mgr == NULL) continue;

        hang_tasks[i] = vidschi_uncompleted_task_exceed_wait_time(sch_mgr,
               adapter->ctl_flags.run_on_qt ? HW_HANG_MAX_TIMEOUT_NS_ON_QT : HW_HANG_MAX_TIMEOUT_NS);

        if(hang_tasks[i] != NULL)
        {
            vidschi_dump_task(hang_tasks[i], i, TRUE);

            hang_engines |= 1 << i;
        }
    }

    sch_global->hw_hang |= hang_engines;

    if (hang_engines != 0)
    {
        for (i = 0; i < MAX_ENGINE_COUNT; i++)
        {
            if(hang_engines & (1 << i))
            {
                sch_mgr = adapter->sch_mgr[i];
                break;
            }
        }

        sch_global->disable_schedule = TRUE;

        zx_info("Hang engine bitmaps:0x%x \n", hang_engines);
        vidschi_dump_hang_info(adapter, hang_engines);

        if(adapter->ctl_flags.recovery_enable == TRUE ||
            adapter->ctl_flags.hang_dump == 2)
        {
            int status = 0;

            zx_info("before hw reset\n");
            zx_console_lock(1);
            status = vidschi_reset_hw(adapter, hang_engines);
            zx_console_lock(0);

            if (status == S_OK && adapter->ctl_flags.hang_dump == 2)
            {
                vidschi_dump_hang(adapter, hang_engines);
            }

            zx_msleep(10);
            sch_global->disable_schedule = FALSE;
            vidsch_notify_interrupt(adapter, 0);
            vidsch_restore_dvfs(adapter);
        }
        else
        {
             zx_msleep(100000);//ensure write to file done
             zx_assert(0, "trigger assert,hang detected but recovery not enabled,stop kernel here to avoid endless hang.");
        }
    }

    for (i = 0; i < MAX_ENGINE_COUNT; i++)
    {
        if (hang_tasks[i])
        {
            vidsch_task_dec_reference(hang_tasks[i]);
        }
    }
    return 0;
}


int vidsch_daemon_thread_event_handler(void *data, zx_event_status_t ret)
{
    vidsch_global_t *schedule = data;
    adapter_t       *adapter  = schedule->adapter;

    vidschi_try_power_tuning(adapter);

    vidschi_check_hang_and_recovery(adapter);

    return 0;
}

/* init schedule daemon thread, mainly contains power gating.
 * hw reset function also be called in this daemon thread */
int vidschi_init_daemon_thread(adapter_t *adapter)
{
    vidsch_global_t    *sch_global = adapter->sch_global;
    util_thread_info_t info        = {0};

    zx_memset(&info, 0, sizeof(util_thread_info_t));

    info.private_data = sch_global;
    info.thread_name  = "zx_kthread_daemon";
    info.msec         = DAEMON_THREAD_INTERVAL;
    info.priority     = 99;

    sch_global->daemon_thread = util_create_event_thread(vidsch_daemon_thread_event_handler, &info);

    return 0;
}

int vidschi_deinit_daemon_thread(adapter_t *adapter)
{
    vidsch_global_t *sch_global = adapter->sch_global;

    if (sch_global->daemon_thread)
    {
        util_destroy_event_thread(sch_global->daemon_thread);
    }

    return 0;
}
