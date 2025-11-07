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
#include "global.h"
#include "kernel_interface.h"

void glb_init_chip_id(adapter_t *adapter, krnl_adapter_init_info_t *info)
{
    adapter->hw_caps.support_snooping = TRUE;

    {
        adapter->family_id = FAMILY_ELT;
        adapter->generic_id= PCI_ID_GENERIC_ELITE3K;

        adapter->chip_id= adapter->bus_config.device_id == PCI_ID_CHX004 ? CHIP_CHX004 : CHIP_CNE001;
        adapter->hw_caps.legacy_offset_enable = TRUE;

        adapter->hw_caps.support_snooping = TRUE;
        //from loongson, loonson 3A3000 not support snoop
#if defined(__mips64__)
        adapter->hw_caps.support_snooping = FALSE;
#endif
        adapter->hw_caps.page_4k_enable   = TRUE;

        adapter->pm_caps.pwm_manual       = FALSE;

        adapter->hw_caps.zx_set_high_eclk = info->zx_set_high_eclk;

        if (adapter->chip_id == CHIP_CNE001)
        {
            adapter->ctl_flags.force_cpu_cm_save_restore = TRUE;
        }
        else
        {
            adapter->ctl_flags.force_cpu_cm_save_restore = TRUE;
        }

        //for elite3000, only cg manual mode is supported.
        //zx_pwm_mode == 1 means enable cg manual mode.

        if (adapter->chip_id == CHIP_CHX004)
        {
            if(adapter->bus_config.sub_sys_vendor_id == 0x17AA && adapter->bus_config.sub_sys_id == 0x350B)
            {
                info->zx_dvfs_mode = 0;
            }
        }

        if (info->zx_pwm_mode)
        {
            int zx_pwm_mode = adapter->chip_id == CHIP_CNE001 ? 0x0616 : info->zx_pwm_mode;
            unsigned int bit_shift = adapter->chip_id == CHIP_CNE001 ? 8 : 0;
            unsigned int cg_mode = zx_pwm_mode & 0xFF;
            unsigned int pg_mode = (zx_pwm_mode >> bit_shift) & 0xFF;
            unsigned int cg_manual_mode = (cg_mode >> 4) & 0xF;
            unsigned int cg_auto_mode = cg_mode & 0xF;
            unsigned int pg_manual_mode = (pg_mode >> 4) & 0xF;
            unsigned int pg_auto_mode = pg_mode & 0xF;

            info->zx_pwm_mode = zx_pwm_mode;

            if (cg_auto_mode & cg_manual_mode)
            {
                cg_manual_mode &= ~cg_auto_mode;
                zx_warning("cg auto and manual mode cannot coexist! Will use default auto mode!cg_auto_mode=0x%x cg_manual_mode=0x%x\n", cg_auto_mode, cg_manual_mode );
            }
            if (pg_auto_mode & pg_manual_mode)
            {
                pg_manual_mode &= ~pg_auto_mode;
                zx_warning("pg auto and manual mode cannot coexist! Will use default auto mode!pg_auto_mode=0x%x pg_manual_mode=0x%x\n", pg_auto_mode, pg_manual_mode );
            }

            adapter->pwm_level.EnableClockGating = cg_mode ? 1 : 0;
            adapter->pwm_level.EnablePowerGating = pg_mode ? 1 : 0;

#define PWM_3D_MODE 0x1
#define PWM_VCP_MODE 0x2
#define PWM_VPP_MODE 0x4
#define PWM_VCP0_MODE 0x2
#define PWM_VCP1_MODE 0x4
#define PWM_VCP2_MODE 0x8
#define PWM_VPP0_MODE 0x10
#define PWM_VPP1_MODE 0x20
#define ASSIGN_PWM_MODE(type1, name, type2)\
        do { \
           adapter->pm_caps.type1##_cg_##type2 = cg_##type2##_mode & PWM_##name##_MODE ? 1:0;\
           adapter->pm_caps.type1##_pg_##type2 = pg_##type2##_mode & PWM_##name##_MODE ? 1:0;\
        }while(0)

            ASSIGN_PWM_MODE(gfx, 3D, manual);
            ASSIGN_PWM_MODE(gfx, 3D, auto);

            /*for chx005 internal:
            bit| 23    22     21     20     19     18 |17    16     15     14   13  12 |11   10    9     8    7    6   | 5     4    3    2    1   0  |
               | vpp1 vpp0    vcp2   vcp1   vcp0   gfx|vpp1  vpp0  vcp2  vcp1 vcp0 gfx |vpp1 vpp0  vcp2  vcp1 vcp0 gfx | vpp1 vpp0 vcp2 vcp1 vcp0 gfx| 
               |                                       0      0      0     0    0   0     0     0    0    0    0     1    1    1     1    1   1    0 |
               |---------------manual-----------------|----------------auto------------|--------------manual-----------|------------auto-------------|
               |-----------------------------------pg----------------------------------|-------------------------------cg----------------------------|
            */
            if (adapter->chip_id == CHIP_CNE001)
            {
                cg_auto_mode = 0x3e;
                cg_manual_mode = 0x1;
                pg_auto_mode = 0x0;
                pg_manual_mode = 0;
                ASSIGN_PWM_MODE(vcp0, VCP0, manual);
                ASSIGN_PWM_MODE(vcp1, VCP1, manual);
                ASSIGN_PWM_MODE(vcp2, VCP2, manual);
                ASSIGN_PWM_MODE(vpp0, VPP0, manual);
                ASSIGN_PWM_MODE(vpp1, VPP1, manual);

                ASSIGN_PWM_MODE(vcp0, VCP0, auto);
                ASSIGN_PWM_MODE(vcp1, VCP1, auto);
                ASSIGN_PWM_MODE(vcp2, VCP2, auto);
                ASSIGN_PWM_MODE(vpp0, VPP0, auto);
                ASSIGN_PWM_MODE(vpp1, VPP1, auto);
            }
            else 
            {
#define EXPANSION_MODE(mode) ((((mode & 0x4) ? 3 : 0 ) << 4) | (((mode & 0x2) ? 7 : 0) << 1) |  ((mode & 0x1) ? 1 : 0))
                cg_auto_mode = EXPANSION_MODE(cg_auto_mode);
                pg_auto_mode = EXPANSION_MODE(pg_auto_mode);
                cg_manual_mode = EXPANSION_MODE(cg_manual_mode);
                pg_manual_mode = EXPANSION_MODE(pg_manual_mode);
                ASSIGN_PWM_MODE(vcp0, VCP0, manual);
                ASSIGN_PWM_MODE(vcp1, VCP1, manual);
                ASSIGN_PWM_MODE(vpp0, VPP0, manual);

                ASSIGN_PWM_MODE(vcp0, VCP0, auto);
                ASSIGN_PWM_MODE(vcp1, VCP1, auto);
                ASSIGN_PWM_MODE(vpp0, VPP0, auto);
            }

        }

        if(info->zx_dvfs_mode && info->zx_set_high_eclk == 0)
        {
            adapter->hw_caps.dvfs_enable    =  TRUE;

            adapter->pm_caps.gfx_dvfs_auto = info->zx_dvfs_mode & 0x1 ? 1:0;
            adapter->pm_caps.vcp_dvfs_auto = info->zx_dvfs_mode & 0x2 ? 1:0;
            adapter->pm_caps.vpp_dvfs_auto = info->zx_dvfs_mode & 0x4 ? 1:0;
            adapter->pm_caps.gfx_mem_dvfs  = info->zx_dvfs_mode & 0x8 ? 1:0;
            adapter->pm_caps.vcp_mem_dvfs  = info->zx_dvfs_mode & 0x10 ? 1:0;
            adapter->pm_caps.vpp_mem_dvfs  = info->zx_dvfs_mode & 0x20 ? 1:0;
            adapter->pm_caps.gfx_output_mode = info->zx_dvfs_mode & 0xc0 ? 1:0;
            adapter->pm_caps.vcp_output_mode = info->zx_dvfs_mode & 0x300 ? 1:0;
            adapter->pm_caps.vpp_output_mode = info->zx_dvfs_mode & 0xc00 ? 1:0;
        }

        adapter->pm_caps.pwm_mode = info->zx_pwm_mode;

        adapter->hw_caps.hw_patch_enable  = FALSE;
#ifndef ZX_HW_NULL
        adapter->hw_caps.local_only       = FALSE;
#else
        adapter->hw_caps.local_only       = FALSE;
#endif
        adapter->hw_caps.secure_range_enable = adapter->sys_caps.secure_on;        //perhaps need get from cbios?
        adapter->hw_caps.share_pcie_enable   = (adapter->hw_caps.local_only ? FALSE:TRUE);

#ifdef VIDEO_ONLY_FPGA
        adapter->hw_caps.video_only        = TRUE;
#else
        adapter->hw_caps.video_only        = FALSE;
#endif

#ifdef GFX_ONLY_FPGA
        adapter->hw_caps.gfx_only          = TRUE;
#else
        adapter->hw_caps.gfx_only          = FALSE;
#endif

        if (adapter->hw_caps.video_only)
        {
            adapter->ctl_flags.paging_enable = FALSE;
            adapter->ctl_flags.worker_thread_enable = FALSE;
        }
        else
        {
            adapter->ctl_flags.paging_enable        = FALSE;
            adapter->ctl_flags.worker_thread_enable = info->zx_worker_thread_enable;
        }

        adapter->ctl_flags.split_enable         = TRUE;

        adapter->ctl_flags.vesa_tempbuffer_enable = info->zx_vesa_tempbuffer_enable ? TRUE : FALSE;
    }

    adapter->ctl_flags.recovery_enable = info->zx_recovery_enable;
    adapter->ctl_flags.run_on_qt = info->zx_run_on_qt;

    if(info->zx_hang_dump)
    {
        adapter->ctl_flags.hang_dump            = info->zx_hang_dump;
        adapter->ctl_flags.recovery_enable      = FALSE;
        adapter->ctl_flags.flag_buffer_verify   = info->zx_flag_buffer_verify;
        adapter->ctl_flags.page_table_local_enable = 1;
        adapter->ctl_flags.page_table_pcie_enable = 0;  // for hang_dump, page_table force local
    }
    else
    {
        adapter->ctl_flags.hang_dump            = 0;
        if (info->page_table_seg == 1)
        {
            adapter->ctl_flags.page_table_local_enable = 1;
            adapter->ctl_flags.page_table_pcie_enable = 0;
        }
        else if (info->page_table_seg == 2 && !adapter->hw_caps.local_only)
        {
            adapter->ctl_flags.page_table_local_enable = 0;
            adapter->ctl_flags.page_table_pcie_enable = 1;
        }
        else
        {
            adapter->ctl_flags.page_table_local_enable = 1;
            adapter->ctl_flags.page_table_pcie_enable = 1;
        }
    }

    adapter->hw_caps.secure_range_enable       = FALSE;
    adapter->hw_caps.miu_channel_size   = info->miu_channel_size;
    adapter->hw_caps.chip_slice_mask    = info->chip_slice_mask;

#if defined(ZX_VMI_MODE)
    adapter->hw_caps.miu_channel_num    = 1;
#endif

    if (adapter->ctl_flags.hang_dump == 3)
    {
        adapter->ctl_flags.worker_thread_enable = FALSE;
    }
    adapter->hw_caps.nonsnoop_patch_enable = (info->nonsnoop_patch == 1) ? TRUE : FALSE;

    if (adapter->chip_id == CHIP_CNE001)
    {
        adapter->sys_caps.fast_clear_bl_slot = TRUE;
    }

    zx_info("adapter->ctl_flags.worker_thread_enable:%x, page_table_local:%d, page_table_pcie:%d.\n",
            adapter->ctl_flags.worker_thread_enable,
            adapter->ctl_flags.page_table_local_enable,
            adapter->ctl_flags.page_table_pcie_enable
            );
    zx_info("hw caps: secure:%d, snoop:%d, 4K_page:%d, Paging:%d, local_only:%d, hang_dump:%d.\n",
        adapter->hw_caps.secure_range_enable, adapter->hw_caps.support_snooping, adapter->hw_caps.page_4k_enable,
        adapter->ctl_flags.paging_enable,
        adapter->hw_caps.local_only, adapter->ctl_flags.hang_dump);

    glb_init_chip_interface(adapter);
}

void glb_get_pci_config_info(adapter_t *adapter)
{
    unsigned long long mmio;
    unsigned long long fb;
    unsigned long  claim_fb_size;
    unsigned short link_status;
    bus_config_t*  bus_config = &adapter->bus_config;
    zx_map_argu_t map    = {0};
    unsigned char  cache_type  = 0;
    alloc_pages_flags_t alloc_flags = {0};

    zx_get_bus_config(adapter->os_device.pdev, bus_config);

    adapter->sys_caps.secure_on = bus_config->secure_on;

    claim_fb_size = bus_config->mem_end_addr[0] - bus_config->mem_start_addr[0] + 1;

    zx_info("bus claimed cpu access-able vram size: 0x%x, %dM, secure_on: %d\n",
        claim_fb_size, claim_fb_size >> 20, adapter->sys_caps.secure_on);

#if defined(__i386__) || defined(__x86_64__) || defined(__mips64__)
    adapter->primary = (bus_config->command & 0x01) ? 1 : 0;
#else
    adapter->primary = 0;
#endif

    adapter->link_width = (bus_config->link_status>>4) & 0x1F;

    mmio = bus_config->reg_start_addr[0];
    fb   = bus_config->mem_start_addr[0] & (~0xFF);/*The base address not right under QNX*/


#if defined(ZX_PCIE_BUS)
    map.flags.cache_type = ZX_MEM_UNCACHED;
    map.flags.mem_type   = ZX_SYSTEM_IO;
    map.node_num         = 1;
    map.phys_addr        = mmio;
    map.size             = bus_config->reg_end_addr[0] - bus_config->reg_start_addr[0];

    adapter->mmio_vma = zx_map_io_memory(&map);

    adapter->mmio     = adapter->mmio_vma->virt_addr;
    adapter->bci_base = (unsigned int*)(adapter->mmio + 0x10000);

    adapter->vidmm_bus_addr = fb;

    adapter->physical_bus_base_length[0] = bus_config->reg_end_addr[0] - bus_config->reg_start_addr[0];
    adapter->physical_bus_base_length[1] = claim_fb_size;

    adapter->visible_local_memory_size = adapter->physical_bus_base_length[1];

    zx_debug("vidmm_bus_addr: 0x%x, visiable_local_size: %dM\n", adapter->vidmm_bus_addr, adapter->visible_local_memory_size >> 20);

#elif defined(ZX_HW_NULL)
    alloc_flags.need_flush = 1;
    alloc_flags.need_zero  = 1;
    alloc_flags.fixed_page = 1;

    map.memory = zx_allocate_pages_memory(adapter->os_device.pdev, 0x80000, adapter->os_page_size, alloc_flags);
    if(map.memory == NULL)
    {
        zx_error("allocate command buffer fail: out of memory!\n");
    }

    map.flags.mem_type   = ZX_SYSTEM_RAM;
    map.size             = 0x80000;

    adapter->mmio_vma = zx_map_pages_memory(&map);
    adapter->mmio_mem = map.memory;

    adapter->mmio     = adapter->mmio_vma->virt_addr;
    adapter->bci_base = (unsigned int*)(adapter->mmio + 0x10000);
    adapter->vidmm_bus_addr = fb;

    claim_fb_size = 0;

    adapter->physical_bus_base_length[0] = 0x80000;
    adapter->physical_bus_base_length[1] = claim_fb_size;

    adapter->total_local_memory_size     =
    adapter->visible_local_memory_size   = claim_fb_size;

    zx_info("video memory size: %dM, visiable_local_size: %dM\n", adapter->total_local_memory_size >> 20, adapter->visible_local_memory_size >> 20);
    zx_info("mmio virt addr: %x.\n", adapter->mmio);

#else
    //engine mmio
    map.flags.cache_type = ZX_MEM_UNCACHED;
    map.flags.mem_type   = ZX_SYSTEM_IO;
    map.node_num         = 1;
    map.phys_addr        = mmio;
    map.size             = bus_config->reg_end_addr[0] - bus_config->reg_start_addr[0];

    adapter->mmio_vma    = zx_map_io_memory(&map);
    adapter->mmio        = adapter->mmio_vma->virt_addr;
    adapter->bci_base    = (unsigned int*)(adapter->mmio + 0x10000);
    adapter->vidmm_bus_addr = fb;

    adapter->physical_bus_base_length[0] = 0x80000;
    adapter->physical_bus_base_length[1] = claim_fb_size;

    zx_info("sys_secure_on: %d, mmio: %x-%d-%p\n",
        bus_config->secure_on,
        bus_config->reg_end_addr[0], bus_config->reg_end_addr[0] - bus_config->reg_start_addr[0], adapter->mmio);

#endif
}
void glb_fini_bus_config(adapter_t *adapter)
{
    if(adapter->mmio_vma)
    {
        zx_unmap_io_memory(adapter->mmio_vma);
    }
}

