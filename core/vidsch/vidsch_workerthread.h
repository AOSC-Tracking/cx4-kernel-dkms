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
#ifndef __VIDSCH_WORKER_THREAD_H__
#define __VIDSCH_WORKER_THREAD_H__

static inline int vidschi_add_task_to_pending_queue(vidsch_mgr_t *sch_mgr, task_desc_t *task)
{
    gpu_context_t       *context = task->context;
    vidsch_task_queue_t *q = context->task_queue;

    int idx = 0;

    vidsch_task_inc_reference(task);

    vidsch_task_queue_add(q, task);

    util_wakeup_event_thread(sch_mgr->worker_thread);

    return S_OK;
}

static inline int vidschi_add_task_to_page_table_set_queue(vidsch_mgr_t *sch_mgr, task_desc_t *task)
{
    gpu_context_t       *context = task->context;
    vidsch_task_queue_t *q = context->page_table_set_task_queue;

    zx_assert (context->page_table_set_task_queue, "context->page_table_set_task_queue != NULL");

    vidsch_task_inc_reference(task);

    vidsch_task_queue_add(q, task);

    util_wakeup_event_thread(sch_mgr->worker_thread);

    return S_OK;
}

extern int  vidsch_worker_thread_event_handler(void *data, zx_event_status_t ret);
extern void vidsch_wait_worker_thread_idle(vidsch_mgr_t *sch_mgr);
extern int vidschi_is_worker_thread_idle(vidsch_mgr_t  *sch_mgr);
#endif
