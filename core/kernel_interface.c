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

#include "kernel_interface.h"
#include "zx_adapter.h"
#include "context.h"
#include "global.h"
#include "powermgr.h"
#include "vidmm.h"
#include "vidsch.h"
#include "perfevent.h"


krnl_import_func_list_t *zx = NULL;

void *krnl_create_device(void *data, void *filp)
{
    adapter_t    *adapter = data;
    gpu_device_t *device = NULL;

    // filp == NULL means KMD internal create device call.
    // so use reserved device.
    if(!filp)
        return adapter->reserved_device;

    return cm_create_device(adapter, filp);
}

void krnl_destroy_device(void *data, void *dev)
{
    adapter_t    *adapter = data;
    gpu_device_t *device  = dev;

    if(device != NULL)
        cm_destroy_device(adapter, device);
}

void* krnl_create_context(void *data, void *dev, zx_create_context_t *create_context)
{
    adapter_t        *adapter = data;
    gpu_device_t     *device  = dev;
    gpu_context_t    *context = NULL;
    create_context_t  create  = {0};

    //multi-engine
    create.type                 = create_context->type;
    create.node_ordinal         = (create_context->type == CONTEXT_PAGING_QUEUE) ? (adapter->paging_engine_index) : create_context->engine_index;
    create.priority             = create_context->priority;
    create.null_render          = create_context->flag.null_render;
    create.client_type          = create_context->client_type;
    create.tid                  = zx_get_current_tid();
    create.power_dvfs_indicador = create_context->flag.power_dvfs_indicador;

    return cm_create_context(device, &create);
}

static void* krnl_pre_init_adapter(void *pdev, krnl_adapter_init_info_t *info, krnl_import_func_list_t *import)
{
    adapter_t *adapter     = NULL;
    platform_caps_t caps   = {0};

    zx = import;

    util_init_log();

    adapter = zx_calloc(sizeof(adapter_t));
    if(adapter == NULL)
    {
        zx_error("allocate adapter fail!\n");

        return NULL;
    }

    adapter->paging_lock        = zx_create_mutex();
    adapter->device_list_lock   = zx_create_mutex();
    //adapter->format_buffer_lock = zx_create_mutex();
    adapter->lock               = zx_create_spinlock();
    adapter->gart_table_lock    = zx_create_mutex();

    adapter->vcp_index_cnt[VCP0_INDEX] = 0;
    adapter->vcp_index_cnt[VCP1_INDEX] = 0;
    adapter->vcp_index_cnt[VCP2_INDEX] = 0;
    adapter->start_index = MIN_VIDEO_SEQ_INDEX;
    adapter->end_index = MAX_VIDEO_SEQ_INDEX_ADD1;
    zx_memset((void*)(adapter->bVideoSeqIndex), 0, sizeof(adapter->bVideoSeqIndex));

    adapter->device_uuid        = zx_create_atomic64(1);
    adapter->context_uuid       = zx_create_atomic64(0x70000000);
    adapter->task_uuid          = zx_create_atomic64(1);

    list_init_head(&adapter->device_list);

    adapter->index = info->minor_index;
    adapter->os_device.pdev = pdev;

#if defined(__force_no_pcie__)
    adapter->ctl_flags.paging_enable        = FALSE;
#else
    adapter->ctl_flags.paging_enable        = TRUE;
#endif
    adapter->ctl_flags.worker_thread_enable = TRUE;
    adapter->ctl_flags.split_enable         = TRUE;
    adapter->hw_caps.fence_interrupt_enable = TRUE;
    adapter->ctl_flags.recovery_enable        = FALSE;
    adapter->ctl_flags.dump_hang_info_to_file = FALSE;
    adapter->ctl_flags.swap_enable            = FALSE;
    adapter->ctl_flags.perf_event_enable      = FALSE;
    adapter->ctl_flags.local_for_display_only = TRUE;
    adapter->ctl_flags.hwq_event_enable       = TRUE;

    zx_query_platform_caps(pdev, &caps);

    adapter->sys_caps.iommu_enabled  = caps.iommu_support;
    if(adapter->ctl_flags.run_in_guest)
    {
        zx_info("detect running in guest mode, force iommu enabled.\n");
        adapter->sys_caps.iommu_enabled = TRUE;
    }

    adapter->hw_caps.address_mode_64bit = !caps.system_need_dma32;
    adapter->os_page_size  = caps.page_size;
    adapter->os_page_shift = caps.page_shift;

    glb_get_pci_config_info(adapter);

    glb_init_chip_id(adapter, info);

    return adapter;
}


void krnl_init_adapter(void *padapter, int reserved_vmem, void *disp_info)
{
    adapter_t *adapter = (adapter_t *)padapter;

    adapter->disp_info = disp_info;

    vidmm_init(adapter, reserved_vmem);

    vidsch_init(adapter);

    vidsch_create_reserve_resource(adapter);

    zx_register_trace_events();

    zx_perf_event_init(adapter);

    gfx_hwq_event_init(adapter);

    zx_info("adapter: %p, ctrl flags: worker_thread_enable:%d, share_space_cover_local_space: %d\n", adapter, adapter->ctl_flags.worker_thread_enable, adapter->ctl_flags.share_space_cover_local);

    zx_info("sys caps:iommu_support:%x, os page size:%x\n",
            adapter->sys_caps.iommu_enabled, adapter->os_page_size);

    zx_info("hw caps: secure:%d, snoop:%d, 64K_page:%d, Paging:%d, CG:%d, DFS:%d.\n",
         adapter->hw_caps.secure_range_enable, adapter->hw_caps.support_snooping,
         adapter->hw_caps.page_64k_enable, adapter->ctl_flags.paging_enable,
         adapter->pwm_level.EnableClockGating, adapter->hw_caps.dvfs_enable);
    zx_info("hw cfg: chip slice mask :%x, miu channel index:%x, miu channel size index:%x\n",
            adapter->hw_caps.chip_slice_mask, adapter->hw_caps.miu_channel_num,
            adapter->hw_caps.miu_channel_size);

    zx_info("Ctrl: Recovery:%d, WK-thread:%d, Hang-Dump:%d, IOMMU:%d, RunInGuest:%d, RunOnQT:%d, NonsnoopEnable:%d\n",
        adapter->ctl_flags.recovery_enable, adapter->ctl_flags.worker_thread_enable,
        adapter->ctl_flags.hang_dump, adapter->sys_caps.iommu_enabled, adapter->ctl_flags.run_in_guest,
        adapter->ctl_flags.run_on_qt, adapter->hw_caps.snoop_only ? 0 : 1);
    zx_info("power caps: ClockGating:%d, PowerGating:%d, pwm manual mode:0x%x be mode:%d\n",
        adapter->pwm_level.EnableClockGating, adapter->pwm_level.EnablePowerGating, adapter->pm_caps.pwm_manual, adapter->pm_caps.be_mode);

    if (adapter->chip_id == CHIP_CNE001)
    {
        zx_info("power caps cg auto   mode gfx %d, vcp0 %d, vcp1 %d, vcp2 %d, vpp0 %d vpp1 %d\n",
            adapter->pm_caps.gfx_cg_auto, adapter->pm_caps.vcp0_cg_auto, adapter->pm_caps.vcp1_cg_auto,  adapter->pm_caps.vcp2_cg_auto, adapter->pm_caps.vpp0_cg_auto, adapter->pm_caps.vpp1_cg_auto);
        zx_info("power caps cg manual mode gfx %d, vcp0 %d, vcp1 %d, vcp2 %d, vpp0 %d vpp1 %d \n",
            adapter->pm_caps.gfx_cg_manual, adapter->pm_caps.vcp0_cg_manual, adapter->pm_caps.vcp1_cg_manual, adapter->pm_caps.vcp2_cg_manual, adapter->pm_caps.vpp0_cg_manual, adapter->pm_caps.vpp1_cg_manual);

        zx_info("power caps pg auto   mode gfx %d, vcp0 %d, vcp1 %d, vcp2 %d, vpp0 %d vpp1 %d\n",
            adapter->pm_caps.gfx_pg_auto, adapter->pm_caps.vcp0_pg_auto, adapter->pm_caps.vcp1_pg_auto, adapter->pm_caps.vcp2_pg_auto, adapter->pm_caps.vpp0_pg_auto, adapter->pm_caps.vpp1_pg_auto);
        zx_info("power caps pg manual mode gfx %d, vcp0 %d, vcp1 %d, vcp2 %d, vpp0 %d vpp1 %d \n",
            adapter->pm_caps.gfx_pg_manual, adapter->pm_caps.vcp0_pg_manual, adapter->pm_caps.vcp1_pg_manual, adapter->pm_caps.vcp2_pg_manual, adapter->pm_caps.vpp0_pg_manual, adapter->pm_caps.vpp1_pg_manual);
    }
    else {
        zx_info("power caps cg auto   mode gfx %d, vcp0 %d, vcp1 %d, vpp %d\n",
            adapter->pm_caps.gfx_cg_auto, adapter->pm_caps.vcp0_cg_auto, adapter->pm_caps.vcp1_cg_auto, adapter->pm_caps.vpp0_cg_auto);
        zx_info("power caps cg manual mode gfx %d, vcp0 %d, vcp1 %d, vpp %d \n",
            adapter->pm_caps.gfx_cg_manual, adapter->pm_caps.vcp0_cg_manual, adapter->pm_caps.vcp0_cg_manual, adapter->pm_caps.vpp0_cg_manual);

        zx_info("power caps pg auto   mode gfx %d, vcp0 %d, vcp1 %d, vpp %d\n",
            adapter->pm_caps.gfx_pg_auto, adapter->pm_caps.vcp0_pg_auto, adapter->pm_caps.vcp1_pg_auto, adapter->pm_caps.vpp0_pg_auto);
        zx_info("power caps pg manual mode gfx %d, vcp0 %d, vcp1 %d, vpp %d \n",
            adapter->pm_caps.gfx_pg_manual, adapter->pm_caps.vcp0_pg_manual, adapter->pm_caps.vcp0_pg_manual, adapter->pm_caps.vpp0_pg_manual);
    }
    zx_info("power caps dvfs auto mode     gfx %d, vcp %d, vpp %d, dvfs manual mode gfx %d, vcp %d, vpp %d \n",
        adapter->pm_caps.gfx_dvfs_auto, adapter->pm_caps.vcp_dvfs_auto, adapter->pm_caps.vpp_dvfs_auto,
        adapter->pm_caps.gfx_dvfs_force, adapter->pm_caps.vcp_dvfs_force, adapter->pm_caps.vpp_dvfs_force);

    zx_info("power caps dvfs mem auto mode gfx %d, vcp %d, vpp %d, dvfs output mode gfx %d, vcp %d, vpp %d \n",
        adapter->pm_caps.gfx_mem_dvfs, adapter->pm_caps.vcp_mem_dvfs, adapter->pm_caps.vpp_mem_dvfs,
        adapter->pm_caps.gfx_output_mode, adapter->pm_caps.vcp_output_mode, adapter->pm_caps.vpp_output_mode);
}

void krnl_deinit_adapter(void *data)
{
    adapter_t *adapter = data;

    perf_event_deinit(adapter);

    gfx_hwq_event_deinit(adapter);

    zx_unregister_trace_events();

    cm_destroy_remained_device(adapter);

    vidsch_destroy(adapter);

    vidmm_destroy(adapter);

    zx_destroy_mutex(adapter->gart_table_lock);

    zx_destroy_mutex(adapter->paging_lock);

    zx_destroy_mutex(adapter->device_list_lock);

    zx_destroy_atomic64(adapter->device_uuid);

    zx_destroy_atomic64(adapter->context_uuid);

    zx_destroy_atomic64(adapter->task_uuid);

    zx_destroy_spinlock(adapter->lock);

    glb_fini_bus_config(adapter);

    zx_free(adapter);

    zx_info("exit adapter....\n");
}


static void krnl_get_adapter_info(void *  adp, adapter_info_t *     adapter_info)
{
    adapter_t *adapter = (adapter_t *)adp;
    if(adapter && adapter_info)
    {
        adapter_info->ven_dev = (((unsigned int)adapter->bus_config.vendor_id) << 16 ) + adapter->bus_config.device_id;
        adapter_info->family_id = adapter->family_id;
        adapter_info->generic_id = adapter->generic_id;
        adapter_info->chip_id = adapter->chip_id;
        adapter_info->sub_sys_vendor_id = adapter->bus_config.sub_sys_vendor_id;
        adapter_info->sub_sys_id = adapter->bus_config.sub_sys_id;
        adapter_info->mmio  = adapter->mmio;
        adapter_info->mmio_size = adapter->mmio_vma->size;
        adapter_info->primary = adapter->primary;
        adapter_info->fb_bus_addr = adapter->vidmm_bus_addr;
        adapter_info->fb_total_size = adapter->visible_local_memory_size;
        adapter_info->run_on_qt = adapter->ctl_flags.run_on_qt;
        adapter_info->patch_fence_intr_lost = (adapter->hw_patch_mask0 & PATCH_FENCE_INTERRUPT_LOST) ? 1 : 0;
    }
}

static  void  krnl_update_adapter_info(void* adp, adapter_info_t*  adapter_info)
{
    adapter_t *adapter = (adapter_t *)adp;
    if(adapter && adapter_info)
    {
        adapter->hw_caps.snoop_only = (adapter_info->snoop_only)? 1 : 0;
        adapter->low_top_address    = adapter_info->low_top_addr;
        adapter->hw_caps.ta_enable = adapter_info->ta_enable;
        if (adapter_info->run_on_hypervisor)
        {
            adapter->ctl_flags.run_in_guest = 1;
        }

        if (adapter->chip_id >= CHIP_CHX004)
        {
            adapter_info->chan_num = 2;

            if(adapter->low_top_address == 0) {
                zx_warning("Fix up: get low_top_address 0, hard code to 0xc0000000\n");
                adapter->low_top_address = 0xc0000000;
            }
        }

        adapter->hw_caps.miu_channel_num = adapter_info->chan_num;
        if(adapter->Real_vram_size == 0)
        {
            adapter->Real_vram_size = adapter_info->total_mem_size_mb;
            adapter->Real_vram_size <<= 20;
        }
        else if((adapter->Real_vram_size >> 20) > adapter_info->total_mem_size_mb)
        {
            zx_info("use zx_local_size as local memory, Real_vram_size:%lldM, total_mem_size_mb:%lldM\n", (adapter->Real_vram_size>>20), adapter_info->total_mem_size_mb);
            zx_warning("set local memory out range actual memory size, force adjust actual memory");
            adapter->Real_vram_size = adapter_info->total_mem_size_mb;
            adapter->Real_vram_size <<= 20;
        }
        else
        {
            zx_info("use params zx_local_size_g to set local memory, not total_mem_size_mb\n");
        }

        adapter->total_local_memory_size = adapter->Real_vram_size;
        adapter->UnAval_vram_size   = (adapter_info->total_mem_size_mb - adapter_info->avai_mem_size_mb);
        adapter->UnAval_vram_size <<= 20;

        if (adapter->hw_caps.snoop_only)
        {
            adapter->ctl_flags.page_table_pcie_enable = 0;
            adapter->ctl_flags.page_table_local_enable = 1;
        }

        if (adapter->chip_id == CHIP_CNE001)
        {
            adapter->vpp_efuse_value = adapter_info->vpp_efuse_value;
            zx_info("vpp_efuse_value: 0x%x.\n", adapter->vpp_efuse_value);
        }
        //zx_info("video memory size: %lldM.\n", adapter->Real_vram_size >> 20);
        //zx_info("video unav size:   %lldM.\n", adapter->UnAval_vram_size >> 20);
        //zx_info("low_top_address:   %lld.\n", adapter->low_top_address);
    }
}


#define ZX_DUMP_POWER_STATUS_INDEX    0x1000
#define ZX_DUMP_SENSOR_TEMP_INDEX     0x2000

static void krnl_dump_resource(struct os_printer *p, void *data, int dump_index, int iga_index)
{
    adapter_t *adapter = data;

    switch(dump_index)
    {
       case 0:
           util_print_log();
           break;
       case 1:
           vidmm_dump_resource(p, adapter);
           break;
       case 2:
       {
            zx_test_mem_para_t  test_mem_para ={0};
            vidsch_test_memory(p, adapter, &test_mem_para);
       }
           break;
       case 3:
           vidsch_dump(p, adapter);
           break;
       case 4:
        //todo: cm
           //cm_dump_resource(adapter);
           break;
       case 5:
           util_print_log();
           vidmm_dump_resource(p, adapter);
           vidsch_dump(p, adapter);
           //todo: cm
           //cm_dump_resource(adapter);
           break;
       //case 0x7000:
       //    vidsch_force_wakup(adapter);
       //    break;
       case ZX_DUMP_POWER_STATUS_INDEX:
           //vidsch_dump_power_status(adapter);
           break;
       default:
           break;
    }
}

static void krnl_debugfs_dump(struct os_printer *p, void *data, int type, void* arg)
{
    adapter_t *adapter = data;
    switch(type)
    {
        case DEBUGFS_NODE_DEVICE:
        {
            gpu_device_t *device  = arg;

            if(device != NULL)
            {
                //todo: cm
                //cm_dump_device_alloctions(seq_file, adapter, device);
            }
            break;
        }

        case DEBUGFS_NODE_VMA:
        {
            gpu_device_t *device = arg ? arg : adapter->reserved_device;
            vidmm_vm_dump_vma_space(p, adapter, device->vm);
            break;
        }

        case DEBUGFS_NODE_VMA_ZONE:
        {
            int i;
            gpu_device_t *device = arg ? arg : adapter->reserved_device;
            zx_vma_space_t *vma = device->vm;

            for (i = 0; i < vma->zone_cnt; i++)
            {
                if (vma->zones[i])
                {
                    vidmm_vm_dump_zone(p, vma->zones[i]);
                }
            }

            break;
        }

        case DEBUGFS_NODE_VM_NODE_INFO:
        {
            vidmm_allocation_t *allocation = arg;

            vidmm_vm_dump_vm_node_info(p, allocation);

            break;
        }

        case DEBUGFS_NODE_HEAP:
        {
            int id = *((int*)arg);

            //todo: vidmm
            vidmm_dump_heap(p, adapter, id);
            break;
        }
        case DEBUGFS_NODE_INFO:
            //todo: vidsch
            //vidsch_dump_info(seq_file, adapter);
            break;

        case DEBUGFS_NODE_MEMTRACK:
        {
            int pid = *((int *)arg);
            //todo: wait vidmm
            vidmm_dump_memtrack(p, adapter, pid);
            break;
        }
        case DEBUGFS_NODE_VIDSCH:
        {
            vidsch_dump(p, adapter);
            break;
        }
        case DEBUGFS_NODE_DEBUGBUS:
        {
            //todo: debugbus
            vidsch_dump_debugbus(p, adapter);
            break;
        }
        case DEBUGFS_NODE_TEST_MEM:
        {
            vidsch_test_memory(p, adapter, arg);
            break;
        }
        default:
            break;
    }
}



int krnl_enum_sub_device(void *data)
{
    return S_OK;
}

void krnl_wait_chip_idle(void *data)
{
    adapter_t *adapter = data;

    vidsch_wait_chip_idle(adapter, ALL_ENGINE_MASK);
}

void* krnl_create_allocation(void *adapter, krnl_create_allocation_info_t *create_info)
{
    return vidmm_create_allocation(adapter, create_info);
}

void krnl_destroy_allocation(void* adapter, void* allocation, zx_destroy_flag_t flags)
{
    vidmm_destroy_allocation(allocation, &flags);
}

static int krnl_get_map_allocation_info(void* adapter, void* allocation, zx_map_argu_t *map)
{
    return vidmm_get_map_allocation_info(adapter, allocation, map);
}

static void krnl_save_state(void *data, int need_save_memory)
{
    adapter_t *adapter = data;

    adapter->in_acpi_stage = 1;
    pm_save_state(adapter, need_save_memory);
}

static int krnl_restore_state(void *data)
{
    adapter_t *adapter = data;
    int        result  = S_OK;

    result = pm_restore_state(adapter);
    adapter->in_acpi_stage = 0;

    return result;
}

static int krnl_query_info(void* data, zx_query_info_t *info)
{
    adapter_t     *adapter       = data;
    int           status         = 0;
    int           search_num     = 0;
    unsigned int  temp_index     = 0;
    gpu_device_t *device         = NULL;

    switch (info->type)
    {
    case ZX_QUERY_REGISTER_U32:
    case ZX_SET_REGISTER_U32:
        //TODO: wait display
        status = vidsch_query_info(adapter, info);
        break;
    case ZX_QUERY_TOTAL_VRAM_SIZE:
    case ZX_QUERY_CPU_VISIBLE_VRAM_SIZE:
    case ZX_QUERY_LOCAL_VRAM_TYPE:
    case ZX_QUERY_HEIGHT_ALIGN:
    case ZX_QUERY_ALLOCATION_RESIDENCY:
    case ZX_QUERY_ALLOCATION_INFO:
    case ZX_QUERY_SEGMENT_FREE_SIZE:
        status = vidmm_query_info(adapter, info);
        break;
    case ZX_QUERY_PROCESS_INFO:
        {
            zx_process_info_t proc_info = {0};
            device = (gpu_device_t*)ptr64_to_ptr(info->in_buf);
            proc_info.pid = device->pid;
            zx_memcpy(proc_info.pname, device->pname, 64);
            zx_memcpy(ptr64_to_ptr(info->buf), &proc_info, sizeof(proc_info));
        }
        break;
    case ZX_QUERY_ENGINE_USAGE_STATUS:
        {
            zx_hwq_info   hwq_info_info={0} ;
            hwq_info_info.Usage_3D=adapter->usage_3d;
            hwq_info_info.Usage_VCP=adapter->usage_vcp;
            hwq_info_info.Usage_VPP=adapter->usage_vpp;
            zx_copy_to_user(ptr64_to_ptr(info->buf), &hwq_info_info, sizeof(zx_hwq_info));
        }
        break;
    case ZX_QUERY_ENGINE_USAGE_STATUS_EXT:
        {
            gfx_hwq_info_ext   tmp_hwq_info_ext={0} ;
            hwq_get_hwq_info_ext(adapter,&tmp_hwq_info_ext);
            zx_copy_to_user(ptr64_to_ptr(info->buf), &tmp_hwq_info_ext, sizeof(gfx_hwq_info_ext));
        }
        break;
    case ZX_QUERY_HW_HANG:
    case ZX_QUERY_PENDING_FRAME_NUM:
    case ZX_QUERY_GET_VIDEO_BRIDGE_BUFFER:
    case ZX_QUERY_GPU_TIME_STAMP:
        status = vidsch_query_info(adapter, info);
        break;

    case ZX_QUERY_PAGE_SWIZZLE_SUPPORT:
        info->value = 0;
        break;

    case ZX_QUERY_CHIP_ID:
        info->value = adapter->chip_id;
        break;

    case ZX_QUERY_VENDOR_ID:
        info->value = adapter->bus_config.vendor_id;
        break;

    case ZX_QUERY_DEVICE_ID:
        info->value = adapter->bus_config.device_id;
        break;

    case ZX_QUERY_REVISION_ID:
        info->value = adapter->bus_config.revision_id;
        break;

    case ZX_QUERY_CHIP_SLICE_MASK:
        info->value = adapter->hw_caps.chip_slice_mask;
        break;

    case ZX_QUERY_SECURED_ON:
        info->value = adapter->sys_caps.secure_on;
        break;

    case ZX_QUERY_VPP_INDEX:
        if (adapter->chip_id == CHIP_CNE001)
        {
            info->value = adapter->vpp_efuse_value; //vpp config from efuse. bit0:vpp0 config, bit1:vpp1 config, 1 is failed chip, 0 is pass. bit2:config is valid or not, 1 is valid.
        }
        else
        {
            info->value = 0;
        }
        break;

    case ZX_QUERY_VCP_INDEX:
    {
        //info->value = adapter->vcp_index_cnt[VCP0_INDEX] > adapter->vcp_index_cnt[VCP1_INDEX] ? VCP1_INDEX : VCP0_INDEX;
        if (info->argu != 0)
        {   //encoder or AVS2
            info->value = VCP0_INDEX;        //core id must be 0
            // set an increment to balance the VCP core usage in transcode mode
            adapter->vcp_index_cnt[info->value] += info->argu;
        } else {
            int i = 0;
            int payload_count = adapter->vcp_index_cnt[0];
            int min_value = 1;

            // find the minimum payload core
            if(adapter->chip_id == CHIP_CNE001) {
                for(i = VCP_INDEX_COUNT - 1; i >= 0 ; i--)
                {
                    if(i == VCP2_INDEX)
                    {
                        continue;
                    }

                    if(adapter->vcp_index_cnt[i] <= payload_count)
                    {
                        payload_count = adapter->vcp_index_cnt[i];
                        min_value = i;
                    }
                }
            } else {
                // for chx004
                for(i = 0; i < VCP_INDEX_COUNT - 1 ; i++)
                {
                    if(adapter->vcp_index_cnt[i] <= payload_count)
                    {
                        payload_count = adapter->vcp_index_cnt[i];
                        min_value = i;
                    }
                }
            }

            info->value = min_value;
            adapter->vcp_index_cnt[info->value]++;
        }
        break;
    }
    case ZX_QUERY_RET_VCP_INDEX:
    {
        int core_index = (info->argu & 0xff);
        if((adapter->chip_id == CHIP_CHX004 && (core_index > VCP1_INDEX || core_index < VCP0_INDEX)) ||
           (adapter->chip_id == CHIP_CNE001 && (core_index > VCP2_INDEX || core_index < VCP0_INDEX))) {
            zx_error("unknown arg (%d) for ZX_QUERY_RET_VCP_INDEX\n", core_index);
            status = -1;
        } else {
            int increment = (info->argu >> 8);
            adapter->vcp_index_cnt[core_index] = adapter->vcp_index_cnt[core_index] - increment;
        }
        break;
    }

    case ZX_QUERY_VIDEO_SEQ_INDEX:
        while(adapter->start_index < adapter->end_index)
        {
            if(adapter->bVideoSeqIndex[adapter->start_index] == 0)
            {
                info->value = adapter->start_index;
                adapter->bVideoSeqIndex[adapter->start_index] = 1;
                device = ptr64_to_ptr(info->in_buf);
                if(device)
                {
                    device->video_seq_index = adapter->start_index;
                    adapter->start_index = (adapter->start_index + 1) % MAX_VIDEO_SEQ_INDEX_ADD1;
                    adapter->end_index = MAX_VIDEO_SEQ_INDEX_ADD1;
                }
                status = 0;
                break;
            }

            adapter->start_index++;

            if(adapter->start_index == adapter->end_index)
            {
                search_num++;
                adapter->end_index  = adapter->start_index;
                adapter->start_index = MIN_VIDEO_SEQ_INDEX;
            }

            if(search_num == 2)
            {
                int i = 0;
                zx_error("Select video seq index failed, reset all video seq index and start from 0\n");
                for (i = MIN_VIDEO_SEQ_INDEX; i < MAX_VIDEO_SEQ_INDEX_ADD1; i++) {
                    adapter->bVideoSeqIndex[i] = 0;
                }
                adapter->start_index = MIN_VIDEO_SEQ_INDEX;
                info->value = adapter->start_index;
                adapter->bVideoSeqIndex[adapter->start_index] = 1;
                device = ptr64_to_ptr(info->in_buf);
                if(device)
                {
                    device->video_seq_index = adapter->start_index;
                    adapter->start_index = (adapter->start_index + 1) % MAX_VIDEO_SEQ_INDEX_ADD1;
                    adapter->end_index = MAX_VIDEO_SEQ_INDEX_ADD1;
                }
                status = 0;
                break;
            }
        }
        break;
    case ZX_QUERY_ADAPTER_INFO:
        {
            adapter_t           *adapter   = data;
            zx_adapter_info_t   adapter_info = {0};
            bus_config_t        *bus_info  = &adapter->bus_config;
            int i = 0;

            adapter_info.bus_config.DeviceID    = bus_info->device_id;
            adapter_info.bus_config.VendorID    = bus_info->vendor_id;
            adapter_info.bus_config.Command     = bus_info->command;
            adapter_info.bus_config.Status      = bus_info->status;
            adapter_info.bus_config.RevisionID  = bus_info->revision_id;
            adapter_info.bus_config.ProgIf      = bus_info->prog_if;
            adapter_info.bus_config.SubClass    = bus_info->sub_class;
            adapter_info.bus_config.BaseClass   = bus_info->base_class;
            adapter_info.bus_config.LatencyTimer= bus_info->latency_timer;
            adapter_info.bus_config.HeaderType  = bus_info->header_type;
            adapter_info.bus_config.BIST        = bus_info->bist;
            adapter_info.bus_config.LinkStatus  = bus_info->link_status;

            adapter_info.bus_config.ulBaseAddresses[0] = 0;
            adapter_info.bus_config.ulBaseAddresses[1] = 0;
            adapter_info.bus_config.ulBaseAddresses[2] = 0;
            adapter_info.bus_config.ulBaseAddresses[3] = 0;
            adapter_info.bus_config.ulBaseAddresses[4] = 0;
            adapter_info.bus_config.ulBaseAddresses[5] = 0;

            adapter_info.bus_config.CacheLineSize      = bus_info->cache_line_size;
            adapter_info.bus_config.SubsystemVendorID  = bus_info->sub_sys_vendor_id;

            adapter_info.bus_config.memorySize         = adapter->Real_vram_size - adapter->UnAval_vram_size;
            adapter_info.chipslicemask                 = adapter->hw_caps.chip_slice_mask;
            adapter_info.gpucount                      = 1;
            adapter_info.osversion                     = 0;
            adapter_info.bVideoOnly                    = adapter->hw_caps.video_only;
            adapter_info.bSnoopOnly                    = adapter->hw_caps.snoop_only;
            adapter_info.bCTEDumpEnable                = 0;
            adapter_info.bDvfsEnable                    = adapter->hw_caps.dvfs_enable;

            adapter_info.segmentMapTable[0]            = 0;
            adapter_info.segmentMapTable[1]            = 1;//fb low
            adapter_info.segmentMapTable[4]            = 2;//non-snoop
            adapter_info.segmentMapTable[3]            = 3;//snoop
            adapter_info.segmentMapTable[2]            = 4;//fb high
            adapter_info.bl_slot_size                  = BL_BUFFER_SIZE >> 18;
            adapter_info.RangeBufferGpuVa              = adapter->RangeBufferGpuVa;

            adapter_info.segment_info_cnt = vidmm_get_segment_count(adapter);

            adapter_info.segment_info[0] = adapter->visible_local_memory_size;
            adapter_info.segment_info[1] = adapter->total_local_memory_size - adapter->visible_local_memory_size;
            for(i=2; i<adapter_info.segment_info_cnt; i++){
                vidmm_segment_t *segment = NULL;
                segment = vidmm_get_segment_by_id(adapter, i);
                if(segment)
                    adapter_info.segment_info[i] = segment->size;
            }

            zx_copy_to_user(ptr64_to_ptr(info->buf), &adapter_info, sizeof(zx_adapter_info_t));
        }
        break;
    case ZX_QUERY_ACTIVE_ENGINE_COUNT:
        info->value = adapter->active_engine_count;
        break;

    case ZX_QUERY_RET_VIDEO_SEQ_INDEX:
        if(info->argu >= MIN_VIDEO_SEQ_INDEX && (info->argu < MAX_VIDEO_SEQ_INDEX_ADD1))
        {
            adapter->bVideoSeqIndex[info->argu] = 0;
        }
        break;
    case ZX_QUERY_VCP_INFO:
        {
            int index = (info->argu << 16) >> 16;
            int op    = (info->argu >> 16);

            if(index < 0 || index >= VCP_INFO_COUNT) {
                zx_error("invalid index-%d\n", index);
                return -1;
            }

            switch(op) {
                case 0:
                {
                    int i;
                    for(i=0; i<VCP_INFO_COUNT; i++) {
                        if(adapter->vcp_info[i].enable == 0) {
                            zx_copy_from_user(&adapter->vcp_info[i], ptr64_to_ptr(info->buf), sizeof(zx_vcp_info));
                            adapter->vcp_info[i].enable = 1;
                            index = i;
                            break;
                        }
                    }
                    if (i >= VCP_INFO_COUNT) {
                        zx_error("Query vcp info index is larger than VCP_INFO_COUNT, reset all vcp info and select 0!\n");
                        for (i=0; i<VCP_INFO_COUNT; i++) {
                            adapter->vcp_info[i].enable = 0;
                            adapter->vcp_info[i].pid = 0;
                        }
                        zx_copy_from_user(&adapter->vcp_info[0], ptr64_to_ptr(info->buf), sizeof(zx_vcp_info));
                        adapter->vcp_info[0].enable = 1;
                        index = 0;
                    }
                    break;
                }
                case 1:
                    adapter->vcp_info[index].enable = 0;
                    adapter->vcp_info[index].pid = 0;
                    break;
                case 2:
                    zx_copy_to_user(ptr64_to_ptr(info->buf), &adapter->vcp_info[index], sizeof(zx_vcp_info));
                    break;
                case 4:
                {
                    int i;
                    for(i=0; i<VCP_INFO_COUNT; i++) {
                        adapter->vcp_info[i].enable = 0;
                        adapter->vcp_info[i].pid = 0;
                    }
                    break;
                }
                case 3:
                default:
                    zx_copy_from_user(&adapter->vcp_info[index], ptr64_to_ptr(info->buf), sizeof(zx_vcp_info));
                    break;
            }

            info->argu = index;
        }
        break;

    default:
        zx_error("%s: unknown query type: %d\n", __func__, info->type);
        status = -1;
        break;
    }

    return status;
}

static  int krnl_begin_perf_event(void *data, zx_begin_perf_event_t *begin_perf_event)
{
    adapter_t    *adapter = data;
    int result = 0;

    result = perf_event_begin(adapter, begin_perf_event);

    return result;
}

static int krnl_begin_miu_dump_perf_event(void *data, zx_begin_miu_dump_perf_event_t *begin_miu_perf_event)
{
    adapter_t *adapter = data;
    int result = 0;

    // result = perf_event_begin_miu_dump(adapter, begin_miu_perf_event);

    return result;
}

static int krnl_end_perf_event(void *data, zx_end_perf_event_t *end_perf_event)
{
    adapter_t    *adapter = data;
    int result = 0;

    result = perf_event_end(adapter, end_perf_event);

    return result;
}

static int krnl_end_miu_dump_perf_event(void *data, zx_end_miu_dump_perf_event_t *end_miu_perf_event)
{
    adapter_t *adapter = data;
    int result = 0;

    // result = perf_event_end_miu_dump(adapter, end_miu_perf_event);

    return result;
}

static int krnl_get_miu_dump_perf_event(void *data, zx_get_miu_dump_perf_event_t *get_miu_dump)
{
    adapter_t *adapter = data;
    int result = 0;

    // result = perf_event_get_miu_event(adapter, get_miu_dump);

    return result;
}

static int krnl_direct_get_miu_dump_perf_event(void *data, zx_direct_get_miu_dump_perf_event_t *direct_get_dump)
{
    adapter_t *adapter = data;
    int result = 0;

    // result = perf_event_direct_get_miu_dump_event(adapter, direct_get_dump);

    return result;
}

static int krnl_get_perf_event(void *data, zx_get_perf_event_t *get_event)
{
    adapter_t    *adapter = data;
    int result = 0;

    result = perf_event_get_event(adapter, get_event);

    return result;
}

static int krnl_set_miu_reg_list_perf_event(void *data, zx_miu_reg_list_perf_event_t *miu_reg_list)
{
    adapter_t *adapter = data;
    int result = 0;

    // result = perf_event_set_miu_reg_list(adapter, miu_reg_list);

    return result;
}

static int krnl_send_perf_event(void *data, zx_perf_event_t *perf_event)
{
    adapter_t    *adapter = data;
    int result = 0;

    result = perf_event_add_event(adapter, perf_event);

    return result;
}

static int krnl_get_perf_status(void *data, zx_perf_status_t *perf_status)
{
    adapter_t   *adapter = data;
    int result = 0;

    result = perf_event_get_status(adapter, perf_status);

    return result;
}

static void krnl_perf_event_add_isr_event(void *data, zx_perf_event_t *perf_event)
{
    adapter_t       *adapter = data;
    if (adapter->ctl_flags.perf_event_enable && adapter->perf_event_mgr)
    {
        perf_event_add_isr_event(adapter, perf_event);
    }
}

static void krnl_perf_event_add_event(void *data, zx_perf_event_t *perf_event)
{
    adapter_t       *adapter = data;
    if (adapter->ctl_flags.perf_event_enable && adapter->perf_event_mgr)
    {
         perf_event_add_event(adapter, perf_event);
    }
}

static int krnl_hwq_process_vsync_event(void *data,unsigned long long time)
{
    adapter_t       *adapter = data;
    int ret =0;
    if (adapter->ctl_flags.hwq_event_enable && adapter->hwq_event_mgr)
    {
        ret=hwq_process_vsync_event(adapter,time);
    }
    return ret;
}

static int krnl_destroy_context(void *data, void *dev, void *ctx)
{
    adapter_t     *adapter = data;
    gpu_device_t  *device  = dev;
    gpu_context_t *context = ctx;

    if((device == NULL) || (context == NULL))
    {
        return S_OK;
    }

    cm_destroy_context(device, context);

    return S_OK;
}

static int krnl_render(void *data, void *gpu_context,  batch_parser_t* batch)
{
    adapter_t     *adapter     = data;

    return vidsch_batch_parser(gpu_context, batch);
}

static int krnl_need_process_task_sync(void *data)
{
    adapter_t *adapter = data;
    // return !adapter->ctl_flags.worker_thread_enable;
    return 1;
}

static int krnl_is_fence_back(void *data, unsigned char engine_index, unsigned long long fence_id)
{
    adapter_t     *adapter = data;
    return vidsch_is_fence_back(adapter, engine_index, fence_id);
}

static int krnl_add_hw_ctx_buf(void *data, void *device, void *ctx, unsigned int ctx_buf_index)
{
    return cm_add_hwctx_buffer(device, ctx, ctx_buf_index);
}

static int  krnl_rm_hw_ctx_buf(void *data, void *device, void *ctx, unsigned int ctx_buf_index)
{
    return cm_remove_hwctx_buffer(device, ctx, ctx_buf_index);
}

static int krnl_set_allocation_priority(void *data, krnl_set_allocation_priority_t *prio)
{
    vidmm_set_allocation_priority(prio);
    return 0;
}

static int krnl_map_gpu_virtual_address(void *data, krnl_map_gpu_va_t *map)
{
    adapter_t               *adapter    = data;
    vidmm_map_desc_args_t   arg         = {0};
    gpu_device_t            *device     = map->device ? map->device : adapter->reserved_device;
    int                     res         = S_OK;
    vidmm_allocation_t      *allocation = map->allocation;

    arg.size      = map->size;
    arg.base_addr = map->base_addr;
    arg.min_addr  = map->min_addr;
    arg.max_addr  = map->max_addr;
    arg.offset    = map->offset;

    res = vidmm_vm_map_gpu_virtual_address(device, map->allocation, &arg);

    map->gpu_va = arg.out_va;

    return res;
}

/*
 * caller should make sure the free range was unused by gpu
 */
static int krnl_free_gpu_virtual_address(void *data, krnl_free_gpu_va_t *free)
{
    adapter_t *adapter = data;
    gpu_device_t *device = free->device ? free->device : adapter->reserved_device;

    vidmm_vm_free_gpu_virtual_address(device->vm, free->base, free->size);

    return 0;
}

static int krnl_reserve_gpu_virtual_address(void *data, krnl_reserve_gpu_va_t *reserve)
{
    adapter_t                      *adapter  = data;
    vidmm_vm_reserve_gpu_va_arg_t   arg      = {0};
    gpu_device_t                   *device   = reserve->device ? reserve->device : adapter->reserved_device;
    int                             res      = S_OK;

    zx_assert(device != NULL, "");

    arg.base = reserve->base;
    arg.size = reserve->size;
    arg.min  = reserve->min;
    arg.max  = reserve->max;
    arg.type.value = reserve->type.value;
    arg.driver_protection = reserve->driver_protection;

    arg.alignment = GPU_PAGE_SIZE;

    res = vidmm_vm_reserve_gpu_virtual_address(device, &arg, &reserve->gpu_va);

    return res;
}

static int krnl_update_gpu_virtual_address(void *data, krnl_update_gpu_va_t *update)
{
    adapter_t                      *adapter  = data;
    gpu_context_t                  *context  = update->context;
    int                             res      = S_OK;

    return vidmm_vm_update_gpu_virtual_address(context, update);
}

static int krnl_notify_interrupt(void *data, unsigned int interrupt_event)
{
    adapter_t *adapter = data;
    return vidsch_notify_interrupt(adapter, interrupt_event);
}

static int krnl_notify_power_tuning(void *data, unsigned int interrupt_event)
{
    adapter_t *adapter = data;
    return vidsch_notify_power_tuning(adapter, interrupt_event);
}

static int krnl_notify_page_fault(void *data)
{
    adapter_t *adapter = data;
    return vidsch_notify_page_fault(adapter);
}

static int krnl_prepare_and_mark_unpagable(void *data, void *allocation)
{
    adapter_t          *adapter     = data;

    return vidmm_prepare_and_mark_unpagable(allocation);
}

static void krnl_mark_pagable(void *data, void *allocation)
{
    adapter_t          *adapter    = data;

    vidmm_mark_pagable(allocation);
}

static int krnl_cil2_misc(void *data, krnl_cil2_misc_t* misc, void *allocation)
{
    adapter_t *adapter = data;

    switch(misc->zx_misc->op_code)
    {
    case ZX_MISC_MMIO_WRITE:
        {
            unsigned int address = misc->zx_misc->mmio.address;
            unsigned int value = misc->zx_misc->mmio.value;
            zx_write32(adapter->mmio + address, value);
            return S_OK;
        }
        break;
    case ZX_MISC_MMIO_READ:
        {
            unsigned int address = misc->zx_misc->mmio.address;
            misc->zx_misc->mmio.value = zx_read32(adapter->mmio + address);
            return S_OK;
        }
        break;
    case ZX_MISC_UPDATE_HEAP_SLOT:
        {
            vidmm_update_heap_slot(adapter, misc);
            return S_OK;
        }
        break;
    case ZX_MISC_SET_HEAP_BL_INFO:
        {
            vidmm_allocation_t* alloc = (vidmm_allocation_t*) allocation;

            alloc->compress_format = misc->zx_misc->set_bl_info.range_type;
            alloc->slot_index      = misc->zx_misc->set_bl_info.bl_slot_index;

            return S_OK;
        }
        break;
    case ZX_MISC_GET_HEAP_BL_INFO:
        {
            vidmm_allocation_t* alloc = (vidmm_allocation_t*) allocation;

            misc->zx_misc->get_bl_info.range_type    = alloc->compress_format;
            misc->zx_misc->get_bl_info.bl_slot_index = alloc->slot_index;


            return S_OK;
        }
        break;
    default:
        return vidsch_cil2_misc(misc->device, misc);
    }
}

static int krnl_test(void *data, krnl_test_t *test)
{
    int result = 0;
    adapter_t *adapter = data;
    switch(test->type)
    {
    case ZX_TEST_PAGING_IN:
        {
            gpu_device_t  *device  = test->device;
            vidmm_allocation_t* allocation = test->allocation;
            zx_mutex_lock(allocation->paging_lock);
            result = vidmm_resident_one_allocation(adapter, allocation);
            zx_mutex_unlock(allocation->paging_lock);
        }
        break;
    case ZX_TEST_PAGING_OUT:
        {
            gpu_device_t  *device  = test->device;
            vidmm_allocation_t* allocation = test->allocation;
            zx_mutex_lock(allocation->paging_lock);
            result = vidmm_unresident_one_allocation(adapter, allocation);
            zx_mutex_unlock(allocation->paging_lock);
        }
        break;
    case ZX_TEST_SUSPEND:
        {
            pm_save_state(adapter, 1);
            break;
        }
    case ZX_TEST_RESUME:
        {
            pm_restore_state(adapter);
            break;
        }
    default:
        result = E_NOINTERFACE;
        break;
    }
    return result;
}

static void krnl_update_global_mapping(void *data, void *alloc, unsigned long long *gpu_va)
{
    adapter_t *adapter = data;

    vidmm_allocation_update_mapping(adapter->reserved_device, adapter->reserved_context, alloc, gpu_va);
}

static void krnl_wakeup_worker_thread(void *data, int engine_index)
{
    adapter_t *adapter = data;

    vidsch_wakeup_worker_thread(adapter, engine_index);
}

static void krnl_update_device_info(void *data, void *dev, unsigned long pid, unsigned long tid, const char *pname)
{
    adapter_t    *adapter = data;
    gpu_device_t *device  = dev;

    device->pid = pid;
    device->tid = tid;
    zx_strncpy(device->pname, pname, sizeof(device->pname));
}

static void krnl_dump_hang_info_flag(void *data, int flag)
{
    adapter_t     *adapter = data;

    adapter->display_debugbus_flag = flag;
    zx_info("adapter->display_debugbus_flag = %d\n", flag);
}

static void krnl_ctl_flags_set(void *data,unsigned int num,unsigned int mask,unsigned int value)
{
    adapter_t *adapter = data;
    unsigned int *ctl_flags  = (unsigned int *)&adapter->ctl_flags;

    if( num > (sizeof(adapter->ctl_flags)/sizeof(unsigned int)) || num <=0 ) return;

    ctl_flags   += (num-1);
    *ctl_flags  &= ~mask;
    *ctl_flags  |= value;
}

static core_interface_t zx_gpu_core = {
#define INTERFACE(item) .item = krnl_##item
    INTERFACE(pre_init_adapter),
    INTERFACE(init_adapter),
    INTERFACE(deinit_adapter),
    INTERFACE(get_adapter_info),
    INTERFACE(update_adapter_info),
    INTERFACE(dump_resource),
    INTERFACE(debugfs_dump),
    INTERFACE(wait_chip_idle),
    INTERFACE(save_state),
    INTERFACE(restore_state),
    INTERFACE(query_info),
    INTERFACE(create_device),
    INTERFACE(destroy_device),
    INTERFACE(update_device_info),
    INTERFACE(create_allocation),
    INTERFACE(destroy_allocation),
    INTERFACE(begin_perf_event),
    INTERFACE(end_perf_event),
    INTERFACE(get_perf_event),
    INTERFACE(send_perf_event),
    INTERFACE(get_perf_status),
    INTERFACE(begin_miu_dump_perf_event),
    INTERFACE(end_miu_dump_perf_event),
    INTERFACE(set_miu_reg_list_perf_event),
    INTERFACE(get_miu_dump_perf_event),
    INTERFACE(direct_get_miu_dump_perf_event),
    INTERFACE(perf_event_add_isr_event),
    INTERFACE(perf_event_add_event),
    INTERFACE(create_context),
    INTERFACE(destroy_context),
    INTERFACE(render),
    INTERFACE(wakeup_worker_thread),

    INTERFACE(is_fence_back),
    INTERFACE(add_hw_ctx_buf),
    INTERFACE(rm_hw_ctx_buf),
    INTERFACE(notify_interrupt),
    INTERFACE(notify_power_tuning),
    INTERFACE(notify_page_fault),
    INTERFACE(get_map_allocation_info),
    INTERFACE(prepare_and_mark_unpagable),
    INTERFACE(mark_pagable),
    INTERFACE(set_allocation_priority),
    INTERFACE(map_gpu_virtual_address),
    INTERFACE(free_gpu_virtual_address),
    INTERFACE(reserve_gpu_virtual_address),
    INTERFACE(update_gpu_virtual_address),
    INTERFACE(cil2_misc),
    INTERFACE(update_global_mapping),

    INTERFACE(test),
    INTERFACE(dump_hang_info_flag),
    INTERFACE(ctl_flags_set),
    INTERFACE(hwq_process_vsync_event),
#undef INTERFACE
};

core_interface_t *krnl_get_core_interface(void)
{
    return &zx_gpu_core;
}


