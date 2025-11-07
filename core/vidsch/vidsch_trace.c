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
#include "vidsch_trace.h"
#include "core_import.h"
#include "perfevent.h"

void vidschi_trace_task_create(vidsch_mgr_t *sch_mgr, task_desc_t *task)
{
    zx_task_create_trace_event(sch_mgr->adapter->index << 16 | sch_mgr->engine_index, task->context->uuid, task->uuid, task->type);
}

void vidschi_trace_task_enqueue(vidsch_mgr_t *sch_mgr, task_desc_t *task, int queue_id)
{
    zx_task_enqueue_trace_event(sch_mgr->adapter->index << 16 | sch_mgr->engine_index, task->context->uuid, task->uuid, queue_id);

    if(sch_mgr->adapter->ctl_flags.perf_event_enable)
    {
        zx_perf_event_t             *perf_event = zx_malloc(sizeof(zx_perf_event_t));
        unsigned long long          timestamp = 0;

        perf_event->header.size = sizeof(zx_perf_event_dma_buffer_queued_t);
        perf_event->header.type = ZX_PERF_EVENT_DMA_BUFFER_QUEUED;
        perf_event->header.pid = zx_get_current_pid();
        perf_event->header.tid = zx_get_current_tid();

        zx_get_nsecs(&timestamp);
        perf_event->header.timestamp_high = timestamp >> 32;
        perf_event->header.timestamp_low = timestamp & 0xffffffff;
        perf_event->dma_buffer_queued.gpu_context = task->context->uuid;
        perf_event->dma_buffer_queued.engine_idx =sch_mgr->engine_index;
        ////perf_event.dma_buffer_queued.dma_idx_low = render_data->render_counter & 0xffffffff;
        ////perf_event.dma_buffer_queued.dma_idx_high = (render_data->render_counter >> 32) & 0xffffffff;

        perf_event_add_event(sch_mgr->adapter, perf_event);
        zx_free(perf_event);
    }
}

void vidschi_trace_task_submit(vidsch_mgr_t *sch_mgr, task_desc_t *task, int fake_dma)
{
    zx_task_submit_trace_event(sch_mgr->adapter->index << 16 | sch_mgr->engine_index, task->context->uuid, task->uuid, task->type, task->fence_id, task->dma_type);

    if(!fake_dma && sch_mgr->adapter->ctl_flags.perf_event_enable)
    {
        unsigned long long timestamp = 0;
        zx_perf_event_t *perf_event = zx_malloc(sizeof(zx_perf_event_t));
        zx_get_nsecs(&timestamp);
        perf_event->header.timestamp_high = timestamp >> 32;
        perf_event->header.timestamp_low = (timestamp) & 0xffffffff;
        perf_event->header.size = sizeof(zx_perf_event_dma_buffer_submitted_t);
        perf_event->header.type = ZX_PERF_EVENT_DMA_BUFFER_SUBMITTED;
        perf_event->dma_buffer_submitted.dma_type = task->dma_type;
        perf_event->dma_buffer_submitted.gpu_context = task->context->uuid;
        perf_event->dma_buffer_submitted.dma_idx_low = task->context->render_counter & 0xffffffff;
        perf_event->dma_buffer_submitted.dma_idx_high = (task->context->render_counter >> 32) & 0xffffffff;
        perf_event->dma_buffer_submitted.engine_idx = sch_mgr->engine_index;
        perf_event->dma_buffer_submitted.fence_id_low = task->fence_id & 0xffffffff;
        perf_event->dma_buffer_submitted.fence_id_high = (task->fence_id >> 32) & 0xffffffff;

        perf_event_add_event(sch_mgr->adapter, perf_event);
        zx_free(perf_event);
    }
}

void vidschi_trace_task_release(vidsch_mgr_t *sch_mgr, task_desc_t *task)
{
    zx_task_release_trace_event(sch_mgr->adapter->index << 16 | sch_mgr->engine_index, task->context->uuid, task->uuid, task->type);
}

void vidschi_trace_fence_back(vidsch_mgr_t *sch_mgr, unsigned long long timestamp, unsigned long long fence_id)
{
    zx_fence_back_trace_event(sch_mgr->adapter->index << 16 | sch_mgr->engine_index, fence_id);

    if(sch_mgr->adapter->ctl_flags.perf_event_enable)
    {
        zx_perf_event_t   perf_event        = {0};
        perf_event.header.timestamp_high = timestamp >> 32;
        perf_event.header.timestamp_low = (timestamp) & 0xffffffff;

        perf_event.header.size = sizeof(zx_perf_event_dma_buffer_completed_t);
        perf_event.header.type = ZX_PERF_EVENT_DMA_BUFFER_COMPLETED;

        perf_event.dma_buffer_completed.engine_idx = sch_mgr->engine_index;
        perf_event.dma_buffer_completed.fence_id_low = fence_id & 0xffffffff;
        perf_event.dma_buffer_completed.fence_id_high = (fence_id >> 32) & 0xffffffff;
        perf_event_add_isr_event(sch_mgr->adapter, &perf_event);
    }
}

