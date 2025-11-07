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
#ifndef __PM_DFS_E3K_H__

#include "zx_adapter.h"
#include "vidsch.h"
#include "vidschi.h"
#include "chip_include_e3k.h"

void vidsch_dfs_init_e3k(adapter_t *adapter);
void vidschi_init_dvfs_setting_004(adapter_t *adapter);
void vidsch_disable_dvfs_004(adapter_t *adapter);

void vidsch_dfs_tuning_e3k(adapter_t *adapter);

int vidsch_power_clock_on_off_e3k(vidsch_mgr_t *sch_mgr, unsigned int off);
void vidsch_power_tuning_e3k(adapter_t *adapter);
void vidschi_adjust_gfx_voltage_cne001(adapter_t *adapter);
#endif
