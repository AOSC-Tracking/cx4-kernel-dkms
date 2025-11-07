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
#include "vidsch_workerthread.h"
#include "vidsch_render.h"
#include "vidsch_submit.h"
#include "vidsch_task_queue.h"

static void vidschi_try_submit_page_table_task_queue(vidsch_mgr_t *sch_mgr, vidsch_task_queue_t *page_table_set_queue)
{
    adapter_t     *adapter   = sch_mgr->adapter;
    task_desc_t   *task      = NULL;
    gpu_context_t *context   = NULL;
    struct list_head task_list;
    int task_num = 0;

    zx_get_nsecs(&sch_mgr->current);

    INIT_LIST_HEAD(&task_list);

    zx_spin_lock(page_table_set_queue->lock);
    list_splice_init(&page_table_set_queue->task_list, &task_list);
    zx_spin_unlock(page_table_set_queue->lock);

    while(!list_empty(&task_list))
    {
        task = list_first_entry(&task_list, task_desc_t, schedule_node);
        list_del(&task->schedule_node);

        context = task->context;

        switch (task->type)
        {
        case task_type_vm_prepare:
            vidsch_submit_vm_prepare_task(sch_mgr, (task_vm_prepare_t *)task);
            break;

        case task_type_page_table_set:
            vidsch_submit_page_table_set_task(sch_mgr, (task_page_table_set_t *)task);
            break;

        default:
            zx_error("unknown task type in page_table_set_queue: %d.\n", task->type);
            break;
        }

        ++task_num;
        zx_up(page_table_set_queue->sema);

        vidsch_task_dec_reference(task);
    }

    zx_spin_lock(page_table_set_queue->lock);
    list_splice_init(&task_list, &page_table_set_queue->task_list);
    page_table_set_queue->task_num -= task_num;
    if ((task_num > 0) || !list_empty(&page_table_set_queue->task_list))
    {
        page_table_set_queue->last_active = sch_mgr->current;
    }
    zx_spin_unlock(page_table_set_queue->lock);

    vidsch_task_queue_try_deactive(page_table_set_queue, sch_mgr->current);
}

static void vidschi_schedule_page_table_set_task(vidsch_mgr_t  *sch_mgr)
{
    vidsch_task_queue_pool_t *page_table_set_pool = sch_mgr->page_table_set_pool;
    vidsch_task_queue_t *q, *next;
    int i;

    if (!page_table_set_pool)
    {
        return;
    }

    zx_mutex_lock(page_table_set_pool->lock);
    zx_get_nsecs(&sch_mgr->current);
    for (i = 0; i <= page_table_set_pool->max_priority; i++)
    {
        list_for_each_entry_safe(q, next, page_table_set_pool->active_list + i, active_link)
        {
            vidschi_try_submit_page_table_task_queue(sch_mgr, q);
        }
    }
    zx_mutex_unlock(page_table_set_pool->lock);
}

static void vidschi_try_submit_task_queue(vidsch_mgr_t *sch_mgr, vidsch_task_queue_t *q)
{
    adapter_t     *adapter   = sch_mgr->adapter;
    task_desc_t   *task      = NULL;
    gpu_context_t *context   = NULL;
    struct list_head task_list;
    int task_num = 0;
    int submitted = FALSE, signaled = FALSE;

    INIT_LIST_HEAD(&task_list);

    zx_spin_lock(q->lock);
    list_splice_init(&q->task_list, &task_list);
    zx_spin_unlock(q->lock);

    while(!list_empty(&task_list))
    {
        vidschi_schedule_page_table_set_task(sch_mgr);

        task = list_first_entry(&task_list, task_desc_t, schedule_node);
        list_del(&task->schedule_node);

        context = task->context;
        submitted = FALSE;

        switch (task->type)
        {
        case task_type_render:

            if (vidschi_can_prepare_dma_task(sch_mgr, (task_dma_t*)task))
            {
                vidschi_prepare_and_submit_dma(sch_mgr, (task_dma_t*)task);

                submitted = TRUE;
            }

            break;

        default:
            submitted = TRUE;
            zx_error("unkonw task type: %d.\n", task->type);
            break;
        }

        if(submitted)
        {
            ++task_num;
            zx_up(q->sema);

            vidsch_task_dec_reference(task);
        }
        else
        {
            list_add(&task->schedule_node, &task_list);
            break;
        }
    }

    zx_spin_lock(q->lock);
    list_splice_init(&task_list, &q->task_list);
    q->task_num -= task_num;
    if ((task_num > 0) || !list_empty(&q->task_list))
    {
        q->last_active = sch_mgr->current;
    }
    zx_spin_unlock(q->lock);

    vidsch_task_queue_try_deactive(q, sch_mgr->current);
}

static void vidschi_schedule(vidsch_mgr_t  *sch_mgr)
{
    vidsch_task_queue_pool_t *pool = sch_mgr->task_queue_list;
    vidsch_task_queue_t *q, *next;
    int i;

    zx_mutex_lock(pool->lock);
    zx_get_nsecs(&sch_mgr->current);
    for (i = pool->max_priority; i >= 0; i--)
    {
        vidschi_schedule_page_table_set_task(sch_mgr);

        list_for_each_entry_safe(q, next, pool->active_list + i, active_link)
        {
            vidschi_try_submit_task_queue(sch_mgr, q);
        }
    }
    zx_mutex_unlock(pool->lock);
}


int vidschi_is_worker_thread_idle(vidsch_mgr_t  *sch_mgr)
{
    return zx_atomic64_read(sch_mgr->pending_tasks_num) ==
           zx_atomic64_read(sch_mgr->handled_tasks_num);
}

int vidsch_worker_thread_event_handler(void *data, zx_event_status_t ret)
{
    vidsch_mgr_t     *sch_mgr      = data;
    gpu_context_t    *context      = NULL;

    int try_freeze_num = 0;

try_again:
    vidschi_schedule(sch_mgr);

    vidsch_release_completed_tasks(sch_mgr, NULL);

    sch_mgr->worker_thread->can_freeze = FALSE;

    if(zx_freezing())
    {
        /* if need freezing, we should cleanup task in thread firstly */
        if(!vidschi_is_worker_thread_idle(sch_mgr))
        {
            zx_info("*** ENGINE[%d] wait render finished, pending:%lld, handled:%lld, try_freeze_num: %d\n",
                sch_mgr->engine_index, zx_atomic64_read(sch_mgr->pending_tasks_num), zx_atomic64_read(sch_mgr->handled_tasks_num), try_freeze_num);

            zx_msleep(10); //relase CPU wait 10ms, and try_again

            try_freeze_num++;

            if(try_freeze_num > 100)
            {
                zx_info("*** render thread [%d] try cleanup queue: %d.\n", sch_mgr->engine_index, try_freeze_num);
            }

            goto try_again;
        }
        else
        {
            sch_mgr->worker_thread->can_freeze = TRUE;

            zx_info("sleep render thread %d.\n", sch_mgr->engine_index);
        }
    }

    return 0;
}


void vidsch_wait_worker_thread_idle(vidsch_mgr_t *sch_mgr)
{
    unsigned int sleep_cnt = 10000; // 10m
    unsigned long long last_fence_id = 0;

    while(!vidschi_is_worker_thread_idle(sch_mgr))
    {
        zx_msleep(1);
        vidsch_release_completed_tasks(sch_mgr, &last_fence_id);
        if (sleep_cnt > 0)
        {
            sleep_cnt--;
        }
        else
        {
            zx_error("vidsch_wait_worker_thread_idle(%d) timeout: (pending:%lld, handled:%lld)\n",
                    sch_mgr->engine_index, zx_atomic64_read(sch_mgr->pending_tasks_num), zx_atomic64_read(sch_mgr->handled_tasks_num));
            sleep_cnt = 1000;
        }
    }
    //zx_info("wait_worker_thread_idle(%d) done: (pending:%d, handled:%d)\n",
    //        sch_mgr->engine_index, zx_atomic64_read(sch_mgr->pending_tasks_num), zx_atomic64_read(sch_mgr->handled_tasks_num));
}
