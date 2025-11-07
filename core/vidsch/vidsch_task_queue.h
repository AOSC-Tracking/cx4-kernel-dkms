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
#ifndef _H_VIDSCH_TASK_QUEUE_H
#define _H_VIDSCH_TASK_QUEUE_H

#include "core_import.h"
#include "list.h"

struct vidsch_task_queue_pool;

typedef struct
{
    int priority;
    struct os_atomic *refcount;
    struct list_head link;
    struct list_head active_link;
    struct os_atomic *active;
    unsigned long long last_active;
    struct os_spinlock *lock;
    struct list_head task_list;
    unsigned int task_num;
    unsigned int max_num;
    struct os_sema *sema;
    struct vidsch_task_queue_pool *pool;
    int id;
} vidsch_task_queue_t;

typedef struct vidsch_task_queue_pool
{
    unsigned int queue_num;
    struct os_mutex *lock;
    int max_priority;   // 0 ~ max_priority
    struct list_head queue_list;
    struct list_head active_list[0];
} vidsch_task_queue_pool_t;

struct _task_desc;

vidsch_task_queue_t *vidsch_task_queue_alloc(int max_num, int priority);
vidsch_task_queue_t *vidsch_task_queue_get(vidsch_task_queue_t *q);
void vidsch_task_queue_release(vidsch_task_queue_t *q);
void vidsch_task_queue_add(vidsch_task_queue_t *q, struct _task_desc *t);

vidsch_task_queue_pool_t *vidsch_task_queue_pool_alloc(int max_priority);
void vidsch_task_queue_pool_release(vidsch_task_queue_pool_t *pool);
void vidsch_task_queue_pool_add(vidsch_task_queue_pool_t *pool, vidsch_task_queue_t *q);
void vidsch_task_queue_pool_remove(vidsch_task_queue_pool_t *pool, vidsch_task_queue_t *q);

// should hold pool->lock
void vidsch_task_queue_try_deactive(vidsch_task_queue_t *q, unsigned long long current);
#endif
