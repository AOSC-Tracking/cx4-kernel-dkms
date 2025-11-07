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
#include "os_interface.h"
#include "zx.h"

#define CREATE_TRACE_POINTS
#include "zxgfx_trace.h"


/* os interface for trace points */
void zx_fence_back_trace_event(int engine_index, unsigned long long fence_id)
{
    trace_zxgfx_fence_back(engine_index, fence_id);
}

void zx_task_create_trace_event(int engine_index, unsigned long long context,
                                unsigned long long task_id, unsigned int task_type)
{
    trace_zxgfx_task_create(engine_index, context, task_id, task_type);
}

void zx_task_enqueue_trace_event(int engine_index, unsigned long long context,
                                 unsigned long long task_id, int queue_id)
{
    trace_zxgfx_task_enqueue(engine_index, context, task_id, queue_id);
}

void zx_task_submit_trace_event(int engine_index, unsigned long long context,
                                unsigned long long task_id, unsigned int task_type,
                                unsigned long long fence_id, unsigned int args)
{
    trace_zxgfx_task_submit(engine_index, context, task_id, task_type, fence_id, args);
}

void zx_task_release_trace_event(int engine_index, unsigned long long context,
                                 unsigned long long task_id, unsigned int task_type)
{
    trace_zxgfx_task_release(engine_index, context, task_id, task_type);
}

void zx_begin_section_trace_event(const char* desc)
{
    trace_zxgfx_begin_section(desc);
}

void zx_end_section_trace_event(int result)
{
    trace_zxgfx_end_section(result);
}

void zx_counter_trace_event(const char* desc, unsigned long long value)
{
    trace_zxgfx_counter(desc, value);
}

void zx_allocation_trace_event(int card_index, void *allocation, int event_type, unsigned long long args)
{
    trace_zxgfx_drm_gem_update_object(card_index, allocation, event_type, args);
}

/* register/unregister probe functions */
void zx_register_trace_events(void)
{

}

void zx_unregister_trace_events(void)
{

}

