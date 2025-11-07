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
 * fence-array: aggregates fence to be waited together
 */

#ifndef __LINUX_DMA_FENCE_ARRAY_H
#define __LINUX_DMA_FENCE_ARRAY_H

#include "zx_fence.h"

/**
 * struct zx_dma_fence_array_cb - callback helper for fence array
 * @cb: fence callback structure for signaling
 * @array: reference to the parent fence array object
 */
struct zx_dma_fence_array_cb {
    dma_fence_cb_t cb;
    struct zx_dma_fence_array *array;
};

/**
 * struct zx_dma_fence_array - fence to represent an array of fences
 * @base: fence base class
 * @lock: spinlock for fence handling
 * @num_fences: number of fences in the array
 * @num_pending: fences in the array still pending
 * @fences: array of the fences
 */
struct zx_dma_fence_array {
    dma_fence_t base;

    spinlock_t lock;
    unsigned num_fences;
    atomic_t num_pending;
    dma_fence_t **fences;
};

extern const dma_fence_ops_t zx_dma_fence_array_ops;

/**
 * zx_dma_fence_is_array - check if a fence is from the array subsclass
 * @fence: fence to test
 *
 * Return true if it is a zx_dma_fence_array and false otherwise.
 */
static inline bool zx_dma_fence_is_array(dma_fence_t *fence)
{
    return fence->ops == &zx_dma_fence_array_ops;
}

/**
 * zx_to_dma_fence_array - cast a fence to a zx_dma_fence_array
 * @fence: fence to cast to a zx_dma_fence_array
 *
 * Returns NULL if the fence is not a zx_dma_fence_array,
 * or the zx_dma_fence_array otherwise.
 */
static inline struct zx_dma_fence_array * zx_to_dma_fence_array(dma_fence_t *fence)
{
    if (fence->ops != &zx_dma_fence_array_ops)
        return NULL;

    return container_of(fence, struct zx_dma_fence_array, base);
}

struct zx_dma_fence_array *zx_dma_fence_array_create(int num_fences, dma_fence_t **fences, u64 context, unsigned seqno, bool signal_on_any);

#endif /* __LINUX_DMA_FENCE_ARRAY_H */
