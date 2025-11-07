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
#include "ring_buffer.h"
#include "vidsch_render.h"
#include "vidsch_engine_e3k.h"
#include "vidsch_debug_hang_e3k.h"

int engine_render_e3k(gpu_context_t *gpu_context, task_dma_t *task_dma)
{
    int                         result          = S_OK;
    gpu_device_t                *gpu_device     = gpu_context->device;
    adapter_t                   *adapter        = gpu_device->adapter;
    static int    debug = 0;

#if TEST_HW_RESET
    /*
    ** test code for force hang/reset, only enable it when reset enable
    ** trigger hang and reset cycle in every 15 minutes when dma type is present
    */
    {
        unsigned long long  current_time, delta_time;
        unsigned int        init_time = 0;
        long                temp_sec, temp_usec;
        static unsigned int        start_hang_reset_cycle = 1;
        static unsigned long long  start_time = 0;
        /* I saw many small garbage flash in screen after reset about 10? times when use wrong cmd, so add this counter and notes */
        static unsigned int        hang_reset_counters = 0;
        static unsigned int        max_hang_reset_counters = 5000000;

        /* only reset twice and need reboot to restart reset. */
        if (/*adapter->ctl_flags.recovery_enable &&*/ (hang_reset_counters < max_hang_reset_counters))
        {
            if (start_hang_reset_cycle)
            {
                zx_getsecs(&temp_sec, &temp_usec);
                start_time = temp_sec * 1000000 + temp_usec;

                start_hang_reset_cycle = 0;

                zx_info("====== reset test: start hang counters %d ====== \n", hang_reset_counters);
            }

            zx_getsecs(&temp_sec, &temp_usec);

            current_time = temp_sec * 1000000 + temp_usec;
            delta_time   = current_time - start_time;

            /* if 900s, trigger hang reset */
            if (delta_time > TEST_HW_RESET_TIME_STEP)
            {
                zx_info("hang_reset triggerred \n");
                zx_info("hang_reset triggerred \n");
                zx_info("hang_reset triggerred \n");
                zx_info("hang_reset triggerred \n");

                *(unsigned int*)task_dma->dma_buffer_node->virt_base_addr =
                SEND_INTERNAL_WRITE_FENCE_EXC(SYNC_FENCE_ROUTE_ID_CSP_MXU,SYNC_FENCE_WRITE_MODE_COPY);
                *((unsigned int*)task_dma->dma_buffer_node->virt_base_addr + 1) =
                SEND_INTERNAL_FENCE_VALUE_EXC(HWM_SYNC_TYPE_K_FLUSH_PIPES, 0xFFFE);
                *((unsigned int*)task_dma->dma_buffer_node->virt_base_addr + 2) =
                SEND_INTERNAL_WAIT_EXC(SYNC_WAIT_WAIT_STATION_ID_CSP_PARSER, HWM_SYNC_TYPE_K_FLUSH_PIPES, 0xFFFF, SYNC_WAIT_WAIT_METHOD_EQUAL);;

                zx_info("hang_reset triggerred \n");
                zx_info("hang_reset triggerred \n");

                /* start next cycle */
                start_hang_reset_cycle = 1;
                hang_reset_counters++;
            }
        }
    }

#endif

    if(adapter->ctl_flags.hang_dump == 3)
    {
        vidsch_duplicate_hang_e3k(adapter, NULL);
    }
    return result;
}
