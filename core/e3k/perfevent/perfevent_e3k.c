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
#include "perfeventi.h"
#include "perfevent.h"
#include "../vidsch/vidsch_engine_e3k.h"

int perf_calculate_engine_usage_e3k(adapter_t * adapter, zx_hwq_info * phwq_info)
{
    unsigned int  engine           = 0;
    hwq_event_mgr_t *hwq_event_mgr = adapter->hwq_event_mgr;
    hwq_event_info  *p_hwq_event   = NULL;
    unsigned int usage             = 0;

    for(engine = 0; engine < adapter->active_engine_count; engine++)
    {

        p_hwq_event   = (hwq_event_info*)(hwq_event_mgr->hwq_event)+engine;
        usage=p_hwq_event->engine_usage;
        //zx_info(" EngineNum=%d usage %d \n", engine, usage);
        if(engine==RB_INDEX_GFXL || engine==RB_INDEX_GFXH)
        {
            if(usage > phwq_info->Usage_3D)
            {
                phwq_info->Usage_3D=usage;
            }
        }

        if(IS_VCP_ENGINE(adapter->chip_id, engine))
        {
            if(usage > phwq_info->Usage_VCP)
            {
                phwq_info->Usage_VCP=usage;
            }
        }

        if(IS_VPP_ENGINE(adapter->chip_id, engine))
        {
            if(usage > phwq_info->Usage_VPP)
            {
                phwq_info->Usage_VPP=usage;
            }
        }
    }
    return 0;
}


int perf_calculate_engine_usage_ext_e3k(adapter_t * adapter, gfx_hwq_info_ext *phwq_info_ext)
{
    unsigned int         usage            = 0;
    unsigned int  engine           = 0;
    hwq_event_info       *p_hwq_event;
    hwq_event_mgr_t *hwq_event_mgr = adapter->hwq_event_mgr;

    zx_memset(phwq_info_ext,0,sizeof(gfx_hwq_info_ext));
    for(engine = 0; engine < adapter->active_engine_count; engine++)
    {

        p_hwq_event   = (hwq_event_info*)(hwq_event_mgr->hwq_event)+engine;
        usage=p_hwq_event->engine_usage;
        //zx_info(" EngineNum=%d usage %d \n", engine, usage);
        switch(engine)
        {
            case RB_INDEX_GFXL:
                phwq_info_ext->Usage_3D=usage;
                break;
            case RB_INDEX_GFXH:
                phwq_info_ext->Usage_3D_High=usage;
                break;
            case RB_INDEX_VCP0:
                phwq_info_ext->Usage_VCP0=usage;
                break;
            case RB_INDEX_VCP1:
                phwq_info_ext->Usage_VCP1=usage;
                break;
//            case RB_INDEX_VPP:
            case RB_INDEX_VCP2:
                if(adapter->chip_id == CHIP_CNE001) {
                    phwq_info_ext->Usage_VCP2 = usage;
                }
                if(adapter->chip_id == CHIP_CHX004){
                    phwq_info_ext->Usage_VPP=usage;
                }
                break;
            case RB_INDEX_VPP0:
                phwq_info_ext->Usage_VPP0=usage;
                break;
            case RB_INDEX_VPP1:
                phwq_info_ext->Usage_VPP1=usage;
                break;
            default:
                break;
        }
    }
    return 0;
}


perf_chip_func_t   perf_chip_func =
{
    .direct_get_miu_counter = NULL,
    .calculate_engine_usage = perf_calculate_engine_usage_e3k,
    .calculate_engine_usage_ext = perf_calculate_engine_usage_ext_e3k,
};
