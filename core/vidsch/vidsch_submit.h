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
#ifndef __VIDSCH_SUBMIT_H__
#define __VIDSCH_SUBMIT_H__

extern int  vidschi_prepare_and_submit_dma(vidsch_mgr_t *sch_mgr, task_dma_t *task_dma);
extern void vidsch_submit_paging_task(adapter_t *adapter, task_paging_t *paging_task);

extern void vidsch_submit_vm_update_task(adapter_t *adapter, task_vm_update_t *vm_task);

extern task_desc_t *vidschi_uncompleted_task_exceed_wait_time(vidsch_mgr_t *sch_mgr, unsigned long long max_wait_time);

extern void vidschi_set_uncompleted_task_dropped(vidsch_mgr_t *sch_mgr, unsigned long long dropped_task);

extern void vidsch_submit_page_table_set_task(vidsch_mgr_t *sch_mgr, task_page_table_set_t *task);

extern void vidsch_submit_vm_prepare_task(vidsch_mgr_t *sch_mgr, task_vm_prepare_t *vm_preapre_task);
#endif

