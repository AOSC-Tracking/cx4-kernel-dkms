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
#ifndef __VIDSCH_PAGING_H__
#define __VIDSCH_PAGING_H__

extern int  vidschi_handle_paging_task(struct gpu_context *context, task_paging_t *paging_task);
extern void vidschi_release_paging_task(vidsch_mgr_t *sch_mgr, task_paging_t *paging_task);
extern void vidschi_dump_paging_task(task_paging_t *paging_task, int idx, int dump_detail);

#endif
