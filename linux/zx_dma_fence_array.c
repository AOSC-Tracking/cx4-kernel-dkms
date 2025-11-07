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

/*
 * zx_dma_fence_array: aggregate fences to be waited together
 */

#include <linux/export.h>
#include <linux/slab.h>
#include "zx_dma_fence_array.h"

static const char *zx_dma_fence_array_get_driver_name(dma_fence_t *fence)
{
    return "zx_dma_fence_array";
}

static const char *zx_dma_fence_array_get_timeline_name(dma_fence_t *fence)
{
    return "unbound";
}

static void zx_dma_fence_array_cb_func(dma_fence_t *f,
                    dma_fence_cb_t *cb)
{
    struct zx_dma_fence_array_cb *array_cb =
        container_of(cb, struct zx_dma_fence_array_cb, cb);
    struct zx_dma_fence_array *array = array_cb->array;

    if (atomic_dec_and_test(&array->num_pending))
        dma_fence_signal(&array->base);
    dma_fence_put(&array->base);
}

static bool zx_dma_fence_array_enable_signaling(dma_fence_t *fence)
{
    struct zx_dma_fence_array *array = zx_to_dma_fence_array(fence);
    struct zx_dma_fence_array_cb *cb = (void *)(&array[1]);
    unsigned i;

    for (i = 0; i < array->num_fences; ++i) {
        cb[i].array = array;
    /*
     * As we may report that the fence is signaled before all
     * callbacks are complete, we need to take an additional
     * reference count on the array so that we do not free it too
     * early. The core fence handling will only hold the reference
     * until we signal the array as complete (but that is now
     * insufficient).
     */
        dma_fence_get(&array->base);
        if (dma_fence_add_callback(array->fences[i], &cb[i].cb, zx_dma_fence_array_cb_func)) {
            dma_fence_put(&array->base);
            if (atomic_dec_and_test(&array->num_pending))
                return false;
        }
    }

    return true;
}

static bool zx_dma_fence_array_signaled(dma_fence_t *fence)
{
    struct zx_dma_fence_array *array = zx_to_dma_fence_array(fence);

    return atomic_read(&array->num_pending) <= 0;
}

static void zx_dma_fence_array_release(dma_fence_t *fence)
{
    struct zx_dma_fence_array *array = zx_to_dma_fence_array(fence);
    unsigned i;

    for (i = 0; i < array->num_fences; ++i)
        dma_fence_put(array->fences[i]);

    kfree(array->fences);
    dma_fence_free(fence);
}

const dma_fence_ops_t zx_dma_fence_array_ops = {
    .get_driver_name = zx_dma_fence_array_get_driver_name,
    .get_timeline_name = zx_dma_fence_array_get_timeline_name,
    .enable_signaling = zx_dma_fence_array_enable_signaling,
    .signaled = zx_dma_fence_array_signaled,
    .wait = dma_fence_default_wait,
    .release = zx_dma_fence_array_release,
};
EXPORT_SYMBOL(zx_dma_fence_array_ops);

/**
 * zx_dma_fence_array_create - Create a custom fence array
 * @num_fences:        [in]    number of fences to add in the array
 * @fences:        [in]    array containing the fences
 * @context:        [in]    fence context to use
 * @seqno:        [in]    sequence number to use
 * @signal_on_any:    [in]    signal on any fence in the array
 *
 * Allocate a zx_dma_fence_array object and initialize the base fence with
 * dma_fence_init().
 * In case of error it returns NULL.
 *
 * The caller should allocate the fences array with num_fences size
 * and fill it with the fences it wants to add to the object. Ownership of this
 * array is taken and dma_fence_put() is used on each fence on release.
 *
 * If @signal_on_any is true the fence array signals if any fence in the array
 * signals, otherwise it signals when all fences in the array signal.
 */
struct zx_dma_fence_array *zx_dma_fence_array_create(int num_fences,
                           dma_fence_t **fences,
                           u64 context, unsigned seqno,
                           bool signal_on_any)
{
    struct zx_dma_fence_array *array;
    size_t size = sizeof(*array);

    /* Allocate the callback structures behind the array. */
    size += num_fences * sizeof(struct zx_dma_fence_array_cb);
    array = kzalloc(size, GFP_KERNEL);
    if (!array)
        return NULL;

    spin_lock_init(&array->lock);
    dma_fence_init(&array->base, &zx_dma_fence_array_ops, &array->lock, context, seqno);

    array->num_fences = num_fences;
    atomic_set(&array->num_pending, signal_on_any ? 1 : num_fences);
    array->fences = fences;

    return array;
}
EXPORT_SYMBOL(zx_dma_fence_array_create);
