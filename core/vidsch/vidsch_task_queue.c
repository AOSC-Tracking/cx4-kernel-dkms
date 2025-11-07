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
#include "vidsch_task_queue.h"
#include "vidsch_trace.h"

static int task_queue_id;

vidsch_task_queue_t *vidsch_task_queue_alloc(int max_num, int priority)
{
    vidsch_task_queue_t *q = zx_calloc(sizeof(*q));
    q->refcount = zx_create_atomic(1);
    q->active = zx_create_atomic(0);
    q->max_num = max_num;
    q->priority = priority;
    q->lock = zx_create_spinlock();
    q->task_num = 0;
    q->sema = zx_create_sema(max_num);
    q->pool = NULL;
    q->last_active = 0;
    q->id = task_queue_id++;
    INIT_LIST_HEAD(&q->task_list);
    INIT_LIST_HEAD(&q->link);
    INIT_LIST_HEAD(&q->active_link);

    return q;
}

vidsch_task_queue_t *vidsch_task_queue_get(vidsch_task_queue_t *q)
{
    zx_atomic_inc(q->refcount);
    return q;
}

void vidsch_task_queue_release(vidsch_task_queue_t *q)
{
    if (zx_atomic_dec(q->refcount) == 0)
    {
        zx_destroy_sema(q->sema);
        zx_destroy_atomic(q->refcount);
        zx_destroy_atomic(q->active);
        zx_destroy_spinlock(q->lock);
        zx_free(q);
    }
}

void vidsch_task_queue_try_deactive(vidsch_task_queue_t *q, unsigned long long current)
{
    zx_spin_lock(q->lock);
    if (list_empty(&q->task_list) && (current - q->last_active > 2000000000ULL))
    {
        if (zx_atomic_cmpxchg(q->active, 1, 0) == 1)
        {
            list_del(&q->active_link);
        }
    }
    zx_spin_unlock(q->lock);
}

void vidsch_task_queue_add(vidsch_task_queue_t *q, task_desc_t *t)
{
    vidsch_mgr_t *sch_mgr = t->context->device->adapter->sch_mgr[t->context->rb_index];
    zx_down(q->sema);

    zx_spin_lock(q->lock);
    list_add_tail(&t->schedule_node, &q->task_list);
    ++q->task_num;
    zx_spin_unlock(q->lock);
    zx_get_nsecs(&t->enqueue_timestamp);

    if (zx_atomic_cmpxchg(q->active, 0, 1) == 0)
    {
        zx_mutex_lock(q->pool->lock);
        list_add_tail(&q->active_link, q->pool->active_list + q->priority);
        zx_mutex_unlock(q->pool->lock);
    }

    vidschi_trace_task_enqueue(sch_mgr, t, q->id);
}

vidsch_task_queue_pool_t *vidsch_task_queue_pool_alloc(int max_priority)
{
    int i;
    vidsch_task_queue_pool_t *pool = zx_calloc(sizeof(*pool) + (max_priority + 1) * sizeof(struct list_head));

    pool->queue_num = 0;
    pool->max_priority = max_priority;
    pool->lock = zx_create_mutex();
    INIT_LIST_HEAD(&pool->queue_list);

    for (i = 0; i <= max_priority; i++)
    {
        INIT_LIST_HEAD(pool->active_list + i);
    }
    return pool;
}

void vidsch_task_queue_pool_release(vidsch_task_queue_pool_t *pool)
{
    vidsch_task_queue_t *q;

    while(!list_empty(&pool->queue_list))
    {
        q = list_first_entry(&pool->queue_list, vidsch_task_queue_t, link);
        list_del(&q->link);

        vidsch_task_queue_release(q);
    }
    zx_destroy_mutex(pool->lock);
    zx_free(pool);
}

void vidsch_task_queue_pool_add(vidsch_task_queue_pool_t *pool, vidsch_task_queue_t *q)
{
    q = vidsch_task_queue_get(q);
    if (q->priority < 0)
    {
        q->priority = 0;
    }
    else if (q->priority > pool->max_priority)
    {
        q->priority = pool->max_priority;
    }
    q->pool = pool;
    zx_mutex_lock(pool->lock);
    list_add_tail(&q->link, &pool->queue_list);
    ++pool->queue_num;
    zx_mutex_unlock(pool->lock);
}

void vidsch_task_queue_pool_remove(vidsch_task_queue_pool_t *pool, vidsch_task_queue_t *q)
{
    zx_mutex_lock(pool->lock);
    list_del(&q->link);
    --pool->queue_num;
    if (zx_atomic_cmpxchg(q->active, 1, 0) == 1)
    {
        list_del(&q->active_link);
    }
    zx_mutex_unlock(pool->lock);
    q->pool = NULL;
}
