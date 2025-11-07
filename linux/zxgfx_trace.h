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

#undef TRACE_SYSTEM
#define TRACE_SYSTEM zxgfx

#if !defined(_ZXGFX_TRACE_H) || defined(TRACE_HEADER_MULTI_READ)
#define _ZXGFX_TRACE_H

#ifdef ZX_TRACE_EVENT
#include <linux/tracepoint.h>
#include "zx_fence.h"
#include "zx_gem.h"


TRACE_EVENT(zxgfx_task_create,
    TP_PROTO(int engine_index, unsigned long long context, unsigned long long task_id, unsigned int task_type),

    TP_ARGS(engine_index, context, task_id, task_type),

    TP_STRUCT__entry(
        __field(int, engine_index)
        __field(unsigned long long, context)
        __field(unsigned long long, task_id)
        __field(unsigned int, task_type)
    ),

    TP_fast_assign(
        __entry->engine_index = engine_index;
        __entry->context = context;
        __entry->task_id = task_id;
        __entry->task_type = task_type;
   ),

    TP_printk("engine_index=%d, context=0x%llx, task_id=%llu, task_type=%u",
        __entry->engine_index, __entry->context, __entry->task_id, __entry->task_type)
);

TRACE_EVENT(zxgfx_task_enqueue,
    TP_PROTO(int engine_index, unsigned long long context, unsigned long long task_id, int queue_id),

    TP_ARGS(engine_index, context, task_id, queue_id),

    TP_STRUCT__entry(
        __field(int, engine_index)
        __field(unsigned long long, context)
        __field(unsigned long long, task_id)
        __field(int, queue_id)
    ),

    TP_fast_assign(
        __entry->engine_index = engine_index;
        __entry->context = context;
        __entry->task_id = task_id;
        __entry->queue_id = queue_id;
   ),

    TP_printk("engine_index=%d, context=0x%llx, task_id=%llu, queue_id=%u",
        __entry->engine_index, __entry->context, __entry->task_id, __entry->queue_id)
);

TRACE_EVENT(zxgfx_task_submit,
    TP_PROTO(int engine_index, unsigned long long context, unsigned long long task_id, unsigned int task_type, unsigned long long fence_id, unsigned int args),

    TP_ARGS(engine_index, context, task_id, task_type, fence_id, args),

    TP_STRUCT__entry(
        __field(int, engine_index)
        __field(unsigned long long, context)
        __field(unsigned long long, task_id)
        __field(unsigned int, task_type)
        __field(unsigned long long, fence_id)
        __field(unsigned int, args)
    ),

    TP_fast_assign(
        __entry->engine_index = engine_index;
        __entry->context = context;
        __entry->task_id = task_id;
        __entry->task_type = task_type;
        __entry->fence_id = fence_id;
        __entry->args = args;
   ),

    TP_printk("engine_index=%d, context=0x%llx, task_id=%llu, task_type=%u, fence_id=%llu, args=%u",
        __entry->engine_index, __entry->context, __entry->task_id, __entry->task_type, __entry->fence_id, __entry->args)
);

TRACE_EVENT(zxgfx_task_release,
    TP_PROTO(int engine_index, unsigned long long context, unsigned long long task_id, unsigned int task_type),

    TP_ARGS(engine_index, context, task_id, task_type),

    TP_STRUCT__entry(
        __field(int, engine_index)
        __field(unsigned long long, context)
        __field(unsigned long long, task_id)
        __field(unsigned int, task_type)
    ),

    TP_fast_assign(
        __entry->engine_index = engine_index;
        __entry->context = context;
        __entry->task_id = task_id;
        __entry->task_type = task_type;
   ),

    TP_printk("engine_index=%d, context=0x%llx, task_id=%llu, task_type=%u",
        __entry->engine_index, __entry->context, __entry->task_id, __entry->task_type)
);

TRACE_EVENT(zxgfx_fence_back,
    TP_PROTO(int engine_index, unsigned long long fence_id),

    TP_ARGS(engine_index, fence_id),

    TP_STRUCT__entry(
        __field(int, engine_index)
        __field(unsigned long long, fence_id)
    ),

    TP_fast_assign(
        __entry->engine_index = engine_index;
        __entry->fence_id = fence_id;
    ),

    TP_printk("engine_index=%d, fence_id=%llu", __entry->engine_index, __entry->fence_id)
);

TRACE_EVENT(zxgfx_vblank_intrr,
    TP_PROTO(unsigned int index, unsigned int cnt),

    TP_ARGS(index, cnt),

    TP_STRUCT__entry(
        __field(unsigned int, index)
        __field(unsigned int, cnt)
    ),

    TP_fast_assign(
        __entry->index = index;
        __entry->cnt = cnt;
    ),

    TP_printk("crtc_index=%d, vbl_count=%d", __entry->index, __entry->cnt)
);

TRACE_EVENT(zxgfx_vblank_onoff,
    TP_PROTO(int index, int on),

    TP_ARGS(index, on),

    TP_STRUCT__entry(
        __field(int, index)
        __field(int, on)
    ),

    TP_fast_assign(
        __entry->index = index;
        __entry->on = on;
    ),

    TP_printk("crtc_index=%d, vblank on=%d", __entry->index, __entry->on)
);

TRACE_EVENT(zxgfx_drm_gem_create_object,
    TP_PROTO(long long card_index, struct drm_zx_gem_object *obj),
    TP_ARGS(card_index, obj),

    TP_STRUCT__entry(
        __field(unsigned long long, allocation)
        __field(unsigned int, size)
        ),

    TP_fast_assign(
        __entry->allocation = card_index << 32 | obj->gid;
        __entry->size = obj->base.size;
        ),

    TP_printk("allocation=%llx, size=%d", __entry->allocation, __entry->size)
);

TRACE_EVENT(zxgfx_drm_gem_set_metadata,
    TP_PROTO(long long card_index, struct drm_zx_gem_object *obj),
    TP_ARGS(card_index, obj),

    TP_STRUCT__entry(
        __field(unsigned long long, allocation)
        __field(unsigned int, size)
        __field(unsigned int, width)
        __field(unsigned int, height)
        __field(unsigned int, bit_cnt)
        __field(unsigned int, pitch)
        __field(unsigned int, segment_id)
        __field(unsigned int, hw_format)
        __field(unsigned int, compress_format)
        __field(unsigned int, tiled)
        ),

    TP_fast_assign(
        __entry->allocation = card_index << 32 | obj->gid;
        __entry->size = obj->base.size;
        __entry->width = obj->debug.dbginfo.width;
        __entry->height = obj->debug.dbginfo.height;
        __entry->bit_cnt = obj->debug.dbginfo.bit_cnt;
        __entry->pitch = obj->debug.dbginfo.pitch;
        __entry->segment_id = obj->info.segment_id;
        __entry->hw_format = obj->debug.dbginfo.hw_format;
        __entry->compress_format = obj->info.compress_format;
        __entry->tiled = obj->info.tiled;
        ),

    TP_printk("allocation=%llx, size=%d, width=%d, height=%d, bit_cnt=%d, pitch=%d, segment_id=%d, hw_format=%d, compress_format=%d, tiled=%d",
        __entry->allocation, __entry->size, __entry->width, __entry->height, __entry->bit_cnt, __entry->pitch,
        __entry->segment_id, __entry->hw_format, __entry->compress_format, __entry->tiled)
);

TRACE_EVENT(zxgfx_drm_gem_update_object,
    TP_PROTO(long long card_index, struct drm_zx_gem_object *obj, int event_type, unsigned long long args),
    TP_ARGS(card_index, obj, event_type, args),

    TP_STRUCT__entry(
        __field(unsigned long long, allocation)
        __field(int, event_type)
        __field(unsigned long long, args)
        ),

    TP_fast_assign(
        __entry->allocation = card_index << 32 | obj->gid;
        __entry->event_type = event_type;
        __entry->args = args;
        ),

    TP_printk("allocation=%llx, event_type=%d, args=%llu", __entry->allocation, __entry->event_type, __entry->args)
);

TRACE_EVENT(zxgfx_drm_gem_release_object,
    TP_PROTO(long long card_index, struct drm_zx_gem_object *obj),
    TP_ARGS(card_index, obj),

    TP_STRUCT__entry(
        __field(unsigned long long, allocation)
        ),

    TP_fast_assign(
        __entry->allocation = card_index << 32 | obj->gid;
        ),

    TP_printk("allocation=%llx", __entry->allocation)
);

TRACE_EVENT(zxgfx_gem_prime_import,
    TP_PROTO(long long card_index, struct dma_buf *dma_buf, struct drm_zx_gem_object* obj),
    TP_ARGS(card_index, dma_buf, obj),

    TP_STRUCT__entry(
        __field(struct dma_buf *, dma_buf)
        __field(unsigned long long, allocation)
        __field(u64, size)
        ),

    TP_fast_assign(
        __entry->dma_buf = dma_buf;
        __entry->allocation = card_index << 32 | obj->gid;
        __entry->size = obj->base.size;
        ),

    TP_printk("dma_buf=%pK, allocation=%llx/0x%llx", __entry->dma_buf, __entry->allocation, __entry->size)
);

TRACE_EVENT(zxgfx_gem_prime_export,
    TP_PROTO(long long card_index, struct dma_buf *dma_buf, struct drm_zx_gem_object* obj),
    TP_ARGS(card_index, dma_buf, obj),

    TP_STRUCT__entry(
        __field(struct dma_buf *, dma_buf)
        __field(unsigned long long, allocation)
        ),

    TP_fast_assign(
        __entry->dma_buf = dma_buf;
        __entry->allocation = card_index << 32 | obj->gid;
        ),

    TP_printk("dma_buf=%pK, allocation=%llx", __entry->dma_buf, __entry->allocation)
);

TRACE_EVENT(zxgfx_dma_fence_wait,
    TP_PROTO(dma_fence_t *fence, bool intr, signed long timeout),
    TP_ARGS(fence, intr, timeout),

    TP_STRUCT__entry(
        __field(dma_fence_t *, fence)
        __field(bool , intr)
        __field(signed long , timeout)
        ),

    TP_fast_assign(
        __entry->fence = fence;
        __entry->intr = intr;
        __entry->timeout = timeout;
        ),

    TP_printk("fence=%pK, intr=%d, timeout=%ld", __entry->fence, __entry->intr, __entry->timeout)
);

TRACE_EVENT(zxgfx_dma_fence_create,
    TP_PROTO(struct zx_dma_fence *fence, unsigned int engine_index),
    TP_ARGS(fence, engine_index),

    TP_STRUCT__entry(
        __field(struct zx_dma_fence *, fence)
        __field(unsigned int, engine_index)
        ),

    TP_fast_assign(
        __entry->fence = fence;
        __entry->engine_index = engine_index;
        ),

    TP_printk("fence=%pK, engine_index=%d", __entry->fence, __entry->engine_index)
);

TRACE_EVENT(zxgfx_dma_fence_release,
    TP_PROTO(struct zx_dma_fence *fence, unsigned int engine_index),
    TP_ARGS(fence, engine_index),

    TP_STRUCT__entry(
        __field(struct zx_dma_fence *, fence)
        __field(unsigned int, engine_index)
        ),

    TP_fast_assign(
        __entry->fence = fence;
        __entry->engine_index = engine_index;
        ),

    TP_printk("fence=%pK, engine_index=%d", __entry->fence, __entry->engine_index)
);

TRACE_EVENT(zxgfx_dma_fence_signaled,
    TP_PROTO(struct zx_dma_fence *fence, unsigned int engine_index),
    TP_ARGS(fence, engine_index),

    TP_STRUCT__entry(
        __field(struct zx_dma_fence *, fence)
        __field(unsigned int, engine_index)
        ),

    TP_fast_assign(
        __entry->fence = fence;
        __entry->engine_index = engine_index;
        ),

    TP_printk("fence=%pK, engine_index=%d", __entry->fence, __entry->engine_index)
);

TRACE_EVENT(zxgfx_dma_fence_enable_signaling,
    TP_PROTO(dma_fence_t *fence),
    TP_ARGS(fence),

    TP_STRUCT__entry(
        __field(dma_fence_t *, fence)
        ),

    TP_fast_assign(
        __entry->fence = fence;
        ),

    TP_printk("fence=%pK", __entry->fence)
);

TRACE_EVENT(zxgfx_dma_fence_array_create,
    TP_PROTO(dma_fence_t *fence, unsigned int engine_index),
    TP_ARGS(fence, engine_index),

    TP_STRUCT__entry(
        __field(dma_fence_t *, fence)
        __field(unsigned int, engine_index)
        ),

    TP_fast_assign(
        __entry->fence = fence;
        __entry->engine_index = engine_index;
        ),

    TP_printk("fence=%pK, engine_index=%d", __entry->fence, __entry->engine_index)
);

TRACE_EVENT(zxgfx_gem_object_begin_cpu_access,
    TP_PROTO(long long card_index, struct drm_zx_gem_object *obj, long timeout, int write),
    TP_ARGS(card_index, obj, timeout, write),

    TP_STRUCT__entry(
        __field(unsigned long long, allocation)
        __field(long, timeout)
        __field(int, write)
        ),

    TP_fast_assign(
        __entry->allocation = card_index << 32 | obj->gid;
        __entry->timeout = timeout;
        __entry->write = write;
        ),

    TP_printk("allocation=%llx, timeout=%ld, write=%d", __entry->allocation, __entry->timeout, __entry->write)
);

TRACE_EVENT(zxgfx_gem_object_end_cpu_access,
    TP_PROTO(long long card_index, struct drm_zx_gem_object *obj),
    TP_ARGS(card_index, obj),

    TP_STRUCT__entry(
        __field(unsigned long long, allocation)
        ),

    TP_fast_assign(
        __entry->allocation = card_index << 32 | obj->gid;
        ),

    TP_printk("allocation=%llx", __entry->allocation)
);

TRACE_EVENT(zxgfx_atomic_flush,
    TP_PROTO(long pipe, void *e),
    TP_ARGS(pipe, e),

    TP_STRUCT__entry(
        __field(long, pipe)
        __field(void *, e)
        ),

    TP_fast_assign(
        __entry->pipe = pipe;
        __entry->e = e
        ),

    TP_printk("flush pipe=%ld,e=%pK ", __entry->pipe, __entry->e)
);

TRACE_EVENT(zxgfx_crtc_flip,
    TP_PROTO(int crtc, int stream_type, struct drm_zx_gem_object* obj),
    TP_ARGS(crtc, stream_type, obj),

    TP_STRUCT__entry(
        __field(int, crtc)
        __field(int, stream_type)
        __field(unsigned int, allocation)
        ),

    TP_fast_assign(
        __entry->crtc = crtc;
        __entry->stream_type = stream_type;
        __entry->allocation = obj->gid;
        ),

    TP_printk("crtc=%d, stream_type=%d, allocation=%x", __entry->crtc, __entry->stream_type, __entry->allocation)
);

TRACE_EVENT(zxgfx_begin_section,
    TP_PROTO(const char *desc),
    TP_ARGS(desc),

    TP_STRUCT__entry(
        __array(char, desc, 32)
    ),

    TP_fast_assign(
        strncpy(__entry->desc, desc, 32);
    ),

    TP_printk("begin=%s", __entry->desc)
);

TRACE_EVENT(zxgfx_end_section,
    TP_PROTO(int result),
    TP_ARGS(result),

    TP_STRUCT__entry(
        __field(int, result)
    ),

    TP_fast_assign(
        __entry->result = result;
    ),

    TP_printk("end=%d", __entry->result)
);

TRACE_EVENT(zxgfx_counter,
    TP_PROTO(const char *desc, unsigned long long value),
    TP_ARGS(desc, value),

    TP_STRUCT__entry(
        __array(char, desc, 32)
        __field(unsigned long long, value)
    ),

    TP_fast_assign(
        strncpy(__entry->desc, desc, 32);
        __entry->value = value;
    ),

    TP_printk("desc=%s, val=0x%llx", __entry->desc, __entry->value)
);

/* This part must be outside protection */
#undef TRACE_INCLUDE_PATH
#define TRACE_INCLUDE_PATH .
#define TRACE_INCLUDE_FILE zxgfx_trace
#include <trace/define_trace.h>

#else

#define trace_zxgfx_task_create(args...) do {} while(0)
#define trace_zxgfx_task_enqueue(args...) do {} while(0)
#define trace_zxgfx_task_submit(args...) do {} while(0)
#define trace_zxgfx_task_release(args...) do {} while(0)
#define trace_zxgfx_fence_back(args...) do {} while(0)
#define trace_zxgfx_vblank_intrr(args...) do {} while(0)
#define trace_zxgfx_vblank_onoff(args...) do {} while(0)

#define trace_zxgfx_drm_gem_create_object(args...) do {} while(0)
#define trace_zxgfx_drm_gem_set_metadata(args...) do {} while(0)
#define trace_zxgfx_drm_gem_update_object(args...) do {} while(0)
#define trace_zxgfx_drm_gem_release_object(args...) do {} while(0)
#define trace_zxgfx_gem_object_begin_cpu_access(args...) do {} while(0)
#define trace_zxgfx_gem_object_end_cpu_access(args...) do {} while(0)
#define trace_zxgfx_gem_prime_import(args...) do {} while(0)
#define trace_zxgfx_gem_prime_export(args...) do {} while(0)
#define trace_zxgfx_dma_fence_wait(args...) do {} while(0)
#define trace_zxgfx_dma_fence_create(args...) do {} while(0)
#define trace_zxgfx_dma_fence_release(args...) do {} while(0)
#define trace_zxgfx_dma_fence_signaled(args...) do {} while(0)
#define trace_zxgfx_dma_fence_enable_signaling(args...) do {} while(0)
#define trace_zxgfx_dma_fence_array_create(args...) do {} while(0)
#define trace_zxgfx_atomic_flush(args...) do {} while(0)
#define trace_zxgfx_crtc_flip(args...) do {} while(0)

#define trace_zxgfx_begin_section(args...) do {} while(0)
#define trace_zxgfx_end_section(args...) do {} while(0)
#define trace_zxgfx_counter(args...) do {} while(0)

#endif

#endif

/*_ZXGFX_TRACE_H */
