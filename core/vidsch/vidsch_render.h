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
#ifndef __VIDSCH_RENDER_H__
#define __VIDSCH_RENDER_H__


extern int  vidschi_handle_dma_task(struct gpu_context *context, task_dma_t *dma_task);
extern void vidschi_release_dma_task(vidsch_mgr_t *sch_mgr, task_dma_t *dma_task);
extern void vidschi_dump_dma_task(task_dma_t *dma_task, int idx, int dump_detail);

extern int  vidschi_can_prepare_dma_task(vidsch_mgr_t *sch_mgr, task_dma_t *task);
extern void vidsch_dma_sync_object_trigger(void *arg);
extern void vidschi_update_and_release_dma_fence(task_desc_t *task);


#endif
