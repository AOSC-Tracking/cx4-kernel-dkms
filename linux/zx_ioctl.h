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

#ifndef __ZX_IOCTL_H__
#define __ZX_IOCTL_H__

#if defined(__linux__)
#include <drm/drm.h>
#include <asm/ioctl.h>
#define ZX_IOCTL_NR(n)         _IOC_NR(n)
#define ZX_IOCTL_TYPE(n)       _IOC_TYPE(n)
#define ZX_IOC_VOID            _IOC_NONE
#define ZX_IOC_READ            _IOC_READ
#define ZX_IOC_WRITE           _IOC_WRITE
#define ZX_IOC_READWRITE       _IOC_READ | _IOC_WRITE
#define ZX_IOC(dir, group, nr, size) _IOC(dir, group, nr, size)
#elif defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__) || defined(__DragonFly__)
#if defined(__FreeBSD__) && defined(IN_MODULE)
#undef ioctl
#include <sys/ioccom.h>
#define ioctl(a,b,c)            xf86ioctl(a,b,c)
#else
#include <sys/ioccom.h>
#endif
#define ZX_IOCTL_NR(n)         ((n) & 0xff)
#define ZX_IOCTL_TYPE(n)       (((n) >> 8) & 0xff)
#define ZX_IOC_VOID            IOC_VOID
#define ZX_IOC_READ            IOC_OUT
#define ZX_IOC_WRITE           IOC_IN
#define ZX_IOC_READWRITE       IOC_INOUT
#define ZX_IOC(dir, group, nr, size) _IOC(dir, group, nr, size)
#endif

#ifndef __user
#define __user
#endif

enum zx_ioctl_nr
{
    ioctl_nr_begin_perf_event = 0,
    ioctl_nr_end_perf_event = 1,
    ioctl_nr_get_perf_event = 2,
    ioctl_nr_send_perf_event = 3,
    ioctl_nr_get_perf_status = 4,
    ioctl_nr_begin_miu_dump_perf_event = 5,
    ioctl_nr_end_miu_dump_perf_event = 6,
    ioctl_nr_set_miu_reg_list_perf_event = 7,
    ioctl_nr_get_miu_dump_perf_event = 8,
    ioctl_nr_direct_get_miu_dump_perf_event = 9,
    ioctl_nr_perf_reserved = 10,

    ioctl_nr_render = 11,
    ioctl_nr_signal_sync_object = 12,
    ioctl_nr_set_power_perf_mode = 13,
    ioctl_nr_wait_event = 14,

    ioctl_nr_wait_chip_idle = 15,
    ioctl_nr_query_info = 16,
    ioctl_nr_wait_func = 17,
    ioctl_nr_create_device = 18,

    ioctl_nr_set_allocation_priority = 19,
    ioctl_nr_make_resident = 20,
    ioctl_nr_evict = 21,
    ioctl_nr_map_gpu_virtual_address = 22,
    ioctl_nr_free_gpu_virtual_address = 23,
    ioctl_nr_reserve_gpu_virtual_address = 24,
    ioctl_nr_update_gpu_virtual_address = 25,

    ioctl_nr_get_hw_context = 26,
    ioctl_nr_add_hw_context = 27,
    ioctl_nr_rm_hw_context = 28,

    ioctl_nr_drm_gem_create_allocation = 29,
    ioctl_nr_drm_gem_map = 30,
    ioctl_nr_create_context = 31,
    ioctl_nr_destroy_context = 32,
    ioctl_nr_drm_begin_cpu_access = 33,
    ioctl_nr_drm_end_cpu_access = 34,
    ioctl_nr_kms_get_pipe_from_crtc = 35,

    ioctl_nr_wait_allocation_idle = 36,

    // debug usage
    ioctl_nr_set_gem_dbg_info = 37,
    ioctl_nr_gem_open = 38,

    ioctl_nr_cil2_misc = 39,
    ioctl_nr_fence_value = 40,

    ioctl_nr_test = 41,

    ioctl_nr_drm_syncobj_create = 42,
    ioctl_nr_drm_syncobj_destroy = 43,
    ioctl_nr_drm_syncobj_wait = 44,
    ioctl_nr_drm_syncobj_reset = 45,
    ioctl_nr_drm_syncobj_get_status = 46,

    /* new ioctl MUST add before 'ioctl_nr_total_num' which used to count total ioctl nums */
    ioctl_nr_total_num
};

#ifdef ZX_HW_NULL
#define ZX_DIR_NAME "/dev"
#define ZX_DEV_NAME "zxcard"
#else
#define ZX_DIR_NAME "/dev/dri"
#define ZX_DEV_NAME "card"
#endif

#define ZX_PROC_NAME "driver/dri"
#define CBIOS_PROC_NAME "driver/cbios"
#define DEV_MEM      "/dev/mem"

#define ZX_IOCTL_BASE                     'd'
#define ZX_IO(nr)                         _IO(ZX_IOCTL_BASE, nr)
#define ZX_IOR(nr, type)                  _IOR(ZX_IOCTL_BASE, nr, type)
#define ZX_IOW(nr, type)                  _IOW(ZX_IOCTL_BASE, nr, type)
#define ZX_IOWR(nr, type)                 _IOWR(ZX_IOCTL_BASE, nr, type)

#define ZX_IOCTL_WAIT_CHIP_IDLE                 ZX_IO(DRM_COMMAND_BASE + ioctl_nr_wait_chip_idle)
#define ZX_IOCTL_DRM_GEM_CREATE_ALLOCATION      ZX_IOWR(DRM_COMMAND_BASE + ioctl_nr_drm_gem_create_allocation, zx_create_allocation_t)
#define ZX_IOCTL_WAIT_ALLOCATION_IDLE           ZX_IOW(DRM_COMMAND_BASE + ioctl_nr_wait_allocation_idle, zx_wait_allocation_idle_t)
#define ZX_IOCTL_QUERY_INFO                     ZX_IOWR(DRM_COMMAND_BASE + ioctl_nr_query_info, zx_query_info_t)
#define ZX_IOCTL_WAIT_FUNC                      ZX_IOWR(DRM_COMMAND_BASE + ioctl_nr_wait_func, zx_wait_func_t)
#define ZX_IOCTL_CEC_CTL                        ZX_IOWR(DRM_COMMAND_BASE + ioctl_nr_cec_ctl, zx_cec_ctl_t)
#define ZX_IOCTL_CREATE_DEVICE                  ZX_IOWR(DRM_COMMAND_BASE + ioctl_nr_create_device, zx_create_device_t)
#define ZX_IOCTL_BEGIN_PERF_EVENT               ZX_IOW(DRM_COMMAND_BASE + ioctl_nr_begin_perf_event, zx_begin_perf_event_t)
#define ZX_IOCTL_END_PERF_EVENT                 ZX_IOW(DRM_COMMAND_BASE + ioctl_nr_end_perf_event, zx_end_perf_event_t)
#define ZX_IOCTL_GET_PERF_EVENT                 ZX_IOWR(DRM_COMMAND_BASE + ioctl_nr_get_perf_event, zx_get_perf_event_t)
#define ZX_IOCTL_SEND_PERF_EVENT                ZX_IOW(DRM_COMMAND_BASE + ioctl_nr_send_perf_event, zx_perf_event_t)
#define ZX_IOCTL_GET_PERF_STATUS                ZX_IOWR(DRM_COMMAND_BASE + ioctl_nr_get_perf_status, zx_perf_status_t)
#define ZX_IOCTL_BEGIN_MIU_DUMP_PERF_EVENT      ZX_IOWR(DRM_COMMAND_BASE + ioctl_nr_begin_miu_dump_perf_event, zx_begin_miu_dump_perf_event_t)
#define ZX_IOCTL_END_MIU_DUMP_PERF_EVENT        ZX_IOWR(DRM_COMMAND_BASE + ioctl_nr_end_miu_dump_perf_event, zx_end_miu_dump_perf_event_t)
#define ZX_IOCTL_SET_MIU_REG_LIST_PERF_EVENT    ZX_IOWR(DRM_COMMAND_BASE + ioctl_nr_set_miu_reg_list_perf_event, zx_miu_reg_list_perf_event_t)
#define ZX_IOCTL_GET_MIU_DUMP_PERF_EVENT        ZX_IOWR(DRM_COMMAND_BASE + ioctl_nr_get_miu_dump_perf_event, zx_get_miu_dump_perf_event_t)
#define ZX_IOCTL_DIRECT_GET_MIU_DUMP_PERF_EVENT ZX_IOWR(DRM_COMMAND_BASE + ioctl_nr_direct_get_miu_dump_perf_event, zx_direct_get_miu_dump_perf_event_t)
#define ZX_IOCTL_RENDER                         ZX_IOWR(DRM_COMMAND_BASE + ioctl_nr_render, zx_render_t)
#define ZX_IOCTL_SET_GEM_DBG_INFO          ZX_IOWR(DRM_COMMAND_BASE + ioctl_nr_set_gem_dbg_info, zx_set_gem_dbg_info_t)
#define ZX_IOCTL_GEM_OPEN                  ZX_IOWR(DRM_COMMAND_BASE + ioctl_nr_gem_open, zx_gem_open_t)

#define ZX_IOCTL_FENCE_VALUE              ZX_IOWR(DRM_COMMAND_BASE + ioctl_nr_fence_value, zx_fence_value_t)
#define ZX_IOCTL_SIGNAL_SYNC_OBJECT       ZX_IOWR(DRM_COMMAND_BASE + ioctl_nr_signal_sync_object, zx_signal_sync_object_t)

#define ZX_IOCTL_SET_ALLOCATION_PRIORITY            ZX_IOW(DRM_COMMAND_BASE + ioctl_nr_set_allocation_priority, zx_set_allocation_priority_t)
#define ZX_IOCTL_MAKE_RESIDENT                      ZX_IOW(DRM_COMMAND_BASE + ioctl_nr_make_resident, zx_make_resident_t)
#define ZX_IOCTL_EVICT                              ZX_IOW(DRM_COMMAND_BASE + ioctl_nr_evict, zx_evict_t)
#define ZX_IOCTL_MAP_GPU_VIRTUAL_ADDRESS            ZX_IOWR(DRM_COMMAND_BASE + ioctl_nr_map_gpu_virtual_address, zx_map_gpu_virtual_address_t)
#define ZX_IOCTL_FREE_GPU_VIRTUAL_ADDRESS           ZX_IOW(DRM_COMMAND_BASE + ioctl_nr_free_gpu_virtual_address, zx_free_gpu_virtual_address_t)
#define ZX_IOCTL_RESERVE_GPU_VIRTUAL_ADDRESS        ZX_IOWR(DRM_COMMAND_BASE + ioctl_nr_reserve_gpu_virtual_address, zx_reserve_gpu_virtual_address_t)
#define ZX_IOCTL_UPDATE_GPU_VIRTUAL_ADDRESS         ZX_IOW(DRM_COMMAND_BASE + ioctl_nr_update_gpu_virtual_address, zx_update_gpu_virtual_address_t)

#define ZX_IOCTL_ADD_HW_CTX_BUF                     ZX_IOWR(DRM_COMMAND_BASE + ioctl_nr_add_hw_context, zx_add_hw_ctx_buf_t)
#define ZX_IOCTL_RM_HW_CTX_BUF                      ZX_IOWR(DRM_COMMAND_BASE + ioctl_nr_rm_hw_context, zx_rm_hw_ctx_buf_t)

#define ZX_IOCTL_CREATE_CONTEXT                     ZX_IOWR(DRM_COMMAND_BASE + ioctl_nr_create_context, zx_create_context_t)
#define ZX_IOCTL_DESTROY_CONTEXT                    ZX_IOWR(DRM_COMMAND_BASE + ioctl_nr_destroy_context, zx_destroy_context_t)
#define ZX_IOCTL_DRM_GEM_MAP_GTT                    ZX_IOWR(DRM_COMMAND_BASE + ioctl_nr_drm_gem_map, zx_drm_gem_map_t)
#define ZX_IOCTL_KMS_GET_PIPE_FROM_CRTC             ZX_IOWR(DRM_COMMAND_BASE + ioctl_nr_kms_get_pipe_from_crtc, zx_kms_get_pipe_from_crtc_t)
#define ZX_IOCTL_DRM_BEGIN_CPU_ACCESS               ZX_IOWR(DRM_COMMAND_BASE + ioctl_nr_drm_begin_cpu_access, zx_drm_gem_begin_cpu_access_t)
#define ZX_IOCTL_DRM_END_CPU_ACCESS                 ZX_IOWR(DRM_COMMAND_BASE + ioctl_nr_drm_end_cpu_access, zx_drm_gem_end_cpu_access_t)

#define ZX_IOCTL_CIL2_MISC              ZX_IOWR(DRM_COMMAND_BASE + ioctl_nr_cil2_misc, zx_cil2_misc_t)
#define ZX_IOCTL_TEST                   ZX_IOWR(DRM_COMMAND_BASE + ioctl_nr_test, zx_test_t)

#define ZX_DRM_IOCTL_SYNCOBJ_CREATE         ZX_IOWR(DRM_COMMAND_BASE + ioctl_nr_drm_syncobj_create, zx_drm_syncobj_create_t)
#define ZX_DRM_IOCTL_SYNCOBJ_DESTROY        ZX_IOWR(DRM_COMMAND_BASE + ioctl_nr_drm_syncobj_destroy, zx_drm_syncobj_destroy_t)
#define ZX_DRM_IOCTL_SYNCOBJ_WAIT           ZX_IOWR(DRM_COMMAND_BASE + ioctl_nr_drm_syncobj_wait, zx_drm_syncobj_wait_t)
#define ZX_DRM_IOCTL_SYNCOBJ_RESET          ZX_IOWR(DRM_COMMAND_BASE + ioctl_nr_drm_syncobj_reset, zx_drm_syncobj_array_t)
#define ZX_DRM_IOCTL_SYNCOBJ_GET_STATUS     ZX_IOWR(DRM_COMMAND_BASE + ioctl_nr_drm_syncobj_get_status, zx_drm_syncobj_status_t)


#endif //__ZX_IOCTL_H__
