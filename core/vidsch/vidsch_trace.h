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

void vidschi_trace_task_create(vidsch_mgr_t *sch_mgr, task_desc_t *task);
void vidschi_trace_task_enqueue(vidsch_mgr_t *sch_mgr, task_desc_t *task, int queue_id);
void vidschi_trace_task_submit(vidsch_mgr_t *sch_mgr, task_desc_t *task, int fake_dma);
void vidschi_trace_task_release(vidsch_mgr_t *sch_mgr, task_desc_t *task);
void vidschi_trace_fence_back(vidsch_mgr_t *sch_mgr, unsigned long long timestamp, unsigned long long fence_id);
