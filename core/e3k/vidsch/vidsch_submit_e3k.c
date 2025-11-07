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
#include "vidmm.h"
#include "ring_buffer.h"
#include "chip_include_e3k.h"
#include "vidsch_engine_e3k.h"
#include "vidsch_debug_hang_e3k.h"
#include "bit_op.h"
#include "vidsch_trace.h"
#include "perfevent.h"

reg_debug_mode_e3k debug_mode_e3k = {0};
extern void enginei_mmio_invalidate_tlb_e3k (adapter_t * adapter);
extern void vidsch_dump_engine_info_e3k(vidsch_mgr_t *sch_mgr);

int enginei_ring_buffer_wrap_e3k(ring_buffer_t *ring, int skip_cnt, void *argu)
{
    engine_e3k_t    *engine     = argu;
    unsigned int    RbIndex     = engine->sch_mgr->engine_index;
    adapter_t         *adapter    = engine->sch_mgr->adapter;
    unsigned int    head_reg_off = EngineRbOffset(adapter->chip_id, RbIndex);
    unsigned int    dwOffset = 0;
    unsigned int    *cmd        = engine->ring_buf_virt_addr + (ring->tail >> 2);

    if(0 && (skip_cnt > 7*4) &&
            !IS_VCP_ENGINE(adapter->chip_id, RbIndex) &&
            !IS_VPP_ENGINE(adapter->chip_id, RbIndex))//For raw skip cmd need add pwdIndicator, for High need add TBR indicator.
    {
        Csp_Opcodes_cmd                     skip_cmd         = {0};
        Csp_Opcodes_cmd                     cmdTbr           = {0};
        Cmd_Blk_Cmd_Csp_Indicator           cmdPwmTrigger    = {0};
        Cmd_Blk_Cmd_Csp_Indicator_Dword1    cmdTrigger_Dw    = {0};
        unsigned int                        dwSkip           = (RbIndex == RB_INDEX_GFXH)?((skip_cnt >> 2) - 7):((skip_cnt >> 2) - 5);

        cmdPwmTrigger.Major_Opcode         = CSP_OPCODE_Blk_Cmd_Csp_Indicator;
        cmdPwmTrigger.Block_Id             = CSP_GLOBAL_BLOCK;
        cmdPwmTrigger.Type                 = BLOCK_COMMAND_CSP_TYPE_INDICATOR;
        cmdPwmTrigger.Info                 = BLK_CMD_CSP_INDICATOR_INFO_3D_MODE;
        cmdPwmTrigger.Dwc                  = 1;

        cmdTrigger_Dw.Slice_Mask = adapter->hw_caps.chip_slice_mask;

        skip_cmd.cmd_Skip.Dwc = dwSkip;
        skip_cmd.cmd_Skip.Major_Opcode = CSP_OPCODE_Skip;

        if(RbIndex == RB_INDEX_GFXH)//add TBR indicator for high
        {
            cmdTbr.cmd_Tbr_Indicator.Skip_En = 1;
            cmdTbr.cmd_Tbr_Indicator.Indicator_Info = TBR_INDICATOR_INDICATOR_INFO_END;
            cmdTbr.cmd_Tbr_Indicator.Block_Id = TASBE_BLOCK;
            cmdTbr.cmd_Tbr_Indicator.Major_Opcode = 0xF;

            *cmd++ = cmdTbr.uint;
        }

        *cmd++ = *(DWORD*)&cmdPwmTrigger;
        *cmd++ = *(DWORD*)&cmdTrigger_Dw;

        *cmd++ = skip_cmd.uint;
        cmd   += dwSkip;

        if(RbIndex != RB_INDEX_GFXH)// only high leave with pwd indicator on.
        {
            cmdPwmTrigger.Info       = BLK_CMD_CSP_INDICATOR_INFO_OFF_MODE;
            cmdTrigger_Dw.Slice_Mask = 0;
        }

        *cmd++ = *(DWORD*)&cmdPwmTrigger;
        *cmd++ = *(DWORD*)&cmdTrigger_Dw;

        if(RbIndex == RB_INDEX_GFXH)
        {
            cmdTbr.cmd_Tbr_Indicator.Indicator_Info = TBR_INDICATOR_INDICATOR_INFO_BEGIN;

            *cmd++ = cmdTbr.uint;
        }
    }
    else if(skip_cnt > 0)//not enough for store indicators.
    {
       zx_memset(cmd, 0, skip_cnt);
    }

    return zx_read32(adapter->mmio + (head_reg_off + 2)*4 + MMIO_CSP_START_ADDRESS);
}

int enginei_enable_ring_buffer_e3k(engine_e3k_t *engine)
{
    adapter_t                  *adapter               = engine->sch_mgr->adapter;
    unsigned int RbIndex = engine->sch_mgr->engine_index;
    Reg_Run_List_Ctx_Addr1      reg_Run_List_Ctx_Addr1      = { 0 };
    Reg_Ring_Buf_Size           reg_Ring_Buf_Size           = { 0 };
    Reg_Ring_Buf_Head           reg_Ring_Buf_Head           = { 0 };
    Reg_Ring_Buf_Tail           reg_Ring_Buf_Tail           = { 0 };
    unsigned int                RegRbOffset = EngineRbOffset(adapter->chip_id, RbIndex);

    ring_buffer_init(&engine->ring, RING_BUFFER_SIZE_E3K, 0, enginei_ring_buffer_wrap_e3k, engine);

    reg_Ring_Buf_Tail.reg.Rb_Tail =
    reg_Ring_Buf_Head.reg.Rb_Head = 0;
    reg_Ring_Buf_Size.reg.Rb_Size = RING_BUFFER_SIZE_E3K;

    reg_Run_List_Ctx_Addr1.reg.Addr = (engine->ring_buf_phys_addr >> 12) & 0xFFFFFFF;
    reg_Run_List_Ctx_Addr1.reg.Kickoff = 0;

    if  ((RbIndex == RB_INDEX_CSL1) || (RbIndex ==RB_INDEX_CSL2) || (RbIndex == RB_INDEX_CSL3))
    {
        reg_Run_List_Ctx_Addr1.reg.L2_Cachable = 1;
    }

    zx_write32(adapter->mmio + RegRbOffset     * 4 + MMIO_CSP_START_ADDRESS, reg_Run_List_Ctx_Addr1.uint );
    zx_write32(adapter->mmio + (RegRbOffset+1) * 4 + MMIO_CSP_START_ADDRESS, reg_Ring_Buf_Size.uint );
    zx_write32(adapter->mmio + (RegRbOffset+2) * 4 + MMIO_CSP_START_ADDRESS, reg_Ring_Buf_Head.uint );
    zx_write32(adapter->mmio + (RegRbOffset+3) * 4 + MMIO_CSP_START_ADDRESS, reg_Ring_Buf_Tail.uint );

    reg_Run_List_Ctx_Addr1.reg.Kickoff = 1;

    zx_mb();

    zx_write32( adapter->mmio + RegRbOffset * 4 + MMIO_CSP_START_ADDRESS, reg_Run_List_Ctx_Addr1.uint );

    return 0;
}

unsigned int *enginei_get_ring_buffer_space_e3k(engine_e3k_t *engine, unsigned int size_uint)
{
    unsigned int *ring_buffer = engine->ring_buf_virt_addr;
    int offset = -1;

    while(offset == -1)
    {
        offset = ring_buffer_get_space(&engine->ring, size_uint << 2);
    }

    // zx_info("idx:%d, offset:%x, end:%x\n", engine->sch_mgr->engine_index, offset, offset + (size_uint <<2));
    engine->current_ring_buffer = ring_buffer + (offset >> 2);
    return engine->current_ring_buffer;
}

// void enginei_do_kickoff_cmd_e3k(engine_e3k_t *engine)
void enginei_release_ring_buffer_space_e3k(engine_e3k_t *engine, unsigned size_uint)
{
    adapter_t *adapter   = engine->sch_mgr->adapter;
    engine_share_e3k_t  *share = (engine_share_e3k_t*)adapter->private_data;
    unsigned int RbIndex       = engine->sch_mgr->engine_index;
    unsigned int RegRbOffset = EngineRbOffset(adapter->chip_id, RbIndex);
    unsigned int offset = 0;
    int ret = S_OK;
    vidsch_mgr_t *vidsch = engine->sch_mgr;

    zx_mb();

    if(1)
    {
        volatile unsigned int* flush_fifo_buffer_2 = (unsigned int*)(share->flush_fifo_buffer->krnl_cpu_vma->virt_addr + 0x200) + RbIndex;

        *((volatile unsigned int*)share->flush_fifo_buffer->krnl_cpu_vma->virt_addr + RbIndex) =  (unsigned int)vidsch->last_send_fence_id;
        while(*((volatile unsigned int*)share->flush_fifo_buffer->krnl_cpu_vma->virt_addr + RbIndex) !=  (unsigned int)vidsch->last_send_fence_id)
        {
            zx_error("flush fifo issue index:%x\n",RbIndex);
            zx_error("krnl virt addr: %p, write value: %x, read value: %x\n",
                ((volatile unsigned int*)share->flush_fifo_buffer->krnl_cpu_vma->virt_addr + RbIndex),
                (unsigned int)vidsch->last_send_fence_id,
                *((volatile unsigned int*)share->flush_fifo_buffer->krnl_cpu_vma->virt_addr + RbIndex));
        }

        *flush_fifo_buffer_2 = (unsigned int)vidsch->last_send_fence_id;
        while(*flush_fifo_buffer_2 != (unsigned int)vidsch->last_send_fence_id)
        {
            zx_error("flush fifo issue index:%x\n",RbIndex);
            zx_error("krnl virt addr: %p, write value: %x, read value: %x\n",
                flush_fifo_buffer_2, (unsigned int)vidsch->last_send_fence_id, *flush_fifo_buffer_2);
        }

    }
    zx_mb();

    if (adapter->pwm_level.EnableClockGating && adapter->pm_caps.pwm_manual)
    {
        unsigned long flags;
        flags = zx_spin_lock_irqsave(vidsch->power_status_lock);
        if (!vidsch->engine_dvfs_power_on)
        {
           if (vidsch->chip_func->power_clock)
           {
              ret = vidsch->chip_func->power_clock(vidsch, FALSE);
              if (S_OK == ret)
              {
                  vidsch->engine_dvfs_power_on = TRUE;
              }
           }
        }

        zx_write32(adapter->mmio + (RegRbOffset+3) * 4 + MMIO_CSP_START_ADDRESS, engine->ring.tail);

        zx_spin_unlock_irqrestore(vidsch->power_status_lock, flags);
    }
    else
    {
       zx_write32(adapter->mmio + (RegRbOffset+3) * 4 + MMIO_CSP_START_ADDRESS, engine->ring.tail);
    }

    zx_mb();

    do{
        offset = zx_read32(adapter->mmio +  (RegRbOffset+3) * 4 + MMIO_CSP_START_ADDRESS);
        if(offset != engine->ring.tail)
        {
            zx_error("idx:%d, kick off:%x, read:%x\n", RbIndex, engine->ring.tail, offset);
        }
    }while(engine->ring.tail != offset);
}

void enginei_write_commands_to_gfx_low_e3k(engine_e3k_t *engine, task_dma_t *dma_task)
{
//  engine_e3k_t         *engine    = &gfx_engine->common;
    engine_gfx_e3k_t *gfx_engine = (engine_gfx_e3k_t *)engine;
    engine_share_e3k_t   *share     = engine->share;
    vidsch_mgr_t         *sch_mgr   = engine->sch_mgr;
    adapter_t            *adapter   = sch_mgr->adapter;

    gpu_context_t *context   = dma_task->desc.context;

    unsigned int  engine_idx = sch_mgr->engine_index;
    unsigned int  ctx_switch = 0;
    unsigned int *ring_buffer;             // current ring buffer pointer
    unsigned int  ring_buffer_size = 0;     //estimate size

    unsigned int client_type        = context->flag.client;
    unsigned int dma_cnt            = dma_task->desc.dma_cnt;
    unsigned int skip_count_uint    = 0;
    unsigned int align_rb_size_uint = 0;
    unsigned int *current_rb        = NULL;
    dma_node_t   *node              = NULL;
    submit_rb_cmd_part1_e3k_t  part1 = {0};
    submit_rb_cmd_part2_e3k_t  part2 = {0};
    submit_rb_cmd_part3_e3k_t  part3 = {0};
    unsigned long long restore_addr, save_addr;
    int i;

    if(adapter->ctl_flags.hang_dump == 2 && sch_mgr->engine_index == RB_INDEX_GFXL)
    {
        vidsch_pre_save_misc_e3k(adapter, sch_mgr, SAVE_BEFORE_POSTHANG);
    }

    enginei_mmio_invalidate_tlb_e3k(adapter);

    switch (client_type)
    {
        case CLIENT_TYPE_OPENGL:
            if(dma_task->flag.need_hwctx_switch && engine_idx == RB_INDEX_GFXL)
            {
                ctx_switch = 1;
            }
            //Todo: not handle the case of dma_cnt = 0, which is seperate handle by the submit signal task
            break;

        case CLIENT_TYPE_VULKAN:
            //Note: For vulkan, the normally state is rb_cmd_template. reset the ctx_switch flag to 0
            //      when the vulkan umd be ready to prepare the whole state.
            ctx_switch = 1;
            break;

        case CLIENT_TYPE_OPENGLES:
        case CLIENT_TYPE_OPENCL:
            ctx_switch = 1;
            zx_info("OpenCL cmd kickoff \n");
            break;
        default:
            zx_error("enginei_write_commands_to_gfx_low_e3k context:%p unkonwn client type:%d\n", context, client_type);
            return;
    }

    ring_buffer_size = (sizeof(submit_rb_cmd_part1_e3k_t) >> 2) +
                       (sizeof(submit_rb_cmd_part2_e3k_t) >> 2) * dma_cnt +
                       (sizeof(submit_rb_cmd_part3_e3k_t) >> 2);

    align_rb_size_uint = util_align(ring_buffer_size, 16);
    skip_count_uint    = align_rb_size_uint - ring_buffer_size;

     //zx_debug("ring_buffer_size: %d, dma_cnt: %d, align_rb_size_uint: %d, skip_count_uint: %d, vm_id:%d\n", ring_buffer_size, dma_cnt, align_rb_size_uint, skip_count_uint, dma_task->desc.vm_id);

    ring_buffer = enginei_get_ring_buffer_space_e3k(engine, align_rb_size_uint);
    current_rb  = ring_buffer;

    zx_memcpy(&part1, &share->rb_cmd_template.rb_cmd_part1, sizeof(submit_rb_cmd_part1_e3k_t));
    zx_memcpy(&part3, &share->rb_cmd_template.rb_cmd_part3, sizeof(submit_rb_cmd_part3_e3k_t));

    part1.reg_dvfs_cfg_mask = (zx_find_first_bit(adapter->sch_global->dvfs_indicator,MAX_POWER_MASK_NUM) < MAX_POWER_MASK_NUM) ? 0 : 0x1;

    if(ctx_switch == 0)
    {
        zx_memset(&part1.skip, 0, 4 * sizeof(unsigned int));
        zx_memset(&part3.skip, 0, 4 * sizeof(unsigned int));
    }

    if(ctx_switch == 1)
    {
        Cmd_Dma_Address_Dword2 cmd_dma_address_dw2 = {0};
        switch(context->flag.client)
        {
            case CLIENT_TYPE_VULKAN:
                part1.restore.restore_dma_addr_l               = (unsigned int)share->context_restore_dma_gpu_va_for_vk;
                cmd_dma_address_dw2.Address_High8 = share->context_restore_dma_gpu_va_for_vk >> 32;
                cmd_dma_address_dw2.L2_Cachable   = 1;
                part1.restore.restore_dma_addr_h               = *(unsigned int*)&cmd_dma_address_dw2;
                part1.restore.restore_context_address          = (unsigned int)(share->context_buf_gpu_va_for_vk >> 11);

                zx_memset(&part3.skip, 0, 4 * sizeof(unsigned int));
                break;
            case CLIENT_TYPE_OPENGL:
            case CLIENT_TYPE_OPENCL:
                part1.restore.restore_dma_addr_l               = (context->flag.client == CLIENT_TYPE_OPENCL) ? ((unsigned int)share->context_restore_dma_gpu_va_cs):((unsigned int)share->context_restore_dma_gpu_va);
                cmd_dma_address_dw2.Address_High8              = share->context_restore_dma_gpu_va >> 32;
                cmd_dma_address_dw2.L2_Cachable                = 1;
                part1.restore.restore_dma_addr_h               = *(unsigned int*)&cmd_dma_address_dw2;

                if (context->hwctx_is_initialized || dma_task->flag.initialize_context)
                {
                    part1.restore.restore_context_address          = (unsigned int)(dma_task->ctx_buf_va >> 11);
                }
                else
                {
                    context->hwctx_is_initialized = 1;
                    part1.restore.restore_context_address          = (unsigned int)(share->context_buf_gpu_va >> 11);
                }

                cmd_dma_address_dw2.Address_High8 = (share->context_save_dma_gpu_va >> 32) & 0xff;
                cmd_dma_address_dw2.L2_Cachable   = 1;

                part3.save.save_dma_addr_l = (unsigned int)share->context_save_dma_gpu_va;
                part3.save.save_dma_addr_h = *(unsigned int*)&cmd_dma_address_dw2;
                part3.save.save_context_address = (unsigned int)(dma_task->ctx_buf_va >> 11);

                // zx_info("ctx:%p, ctx_restore addr:%llx, ctx_save addr:%llx, ctx_restore_dma:%llx, ctx_save_dma:%llx.\n",
                //         context, restore_addr, dma_task->ctx_buf_va,
                //         share->context_restore_dma_gpu_va, share->context_save_dma_gpu_va);
                break;
            default:
                zx_error("not support client API (%d) for context-switch.\n", context->flag.client);
                break;
        }
    }

    part1.pwm_trigger_on.Process_Id = dma_task->desc.vm_id;
    part1.trigger_on_dw.Slice_Mask  = adapter->hw_caps.chip_slice_mask;

    if(dma_task->dma_type == PAGING_DMA)
    {
        part1.pwm_trigger_on.Info = BLK_CMD_CSP_INDICATOR_INFO_3D_MODE;
    }

   //todo: RB_CS_INDEX for part1->pwm_trigger_on.Info = BLK_CMD_CSP_INDICATOR_INFO_CS_MODE

    zx_memcpy(current_rb, &part1, sizeof(submit_rb_cmd_part1_e3k_t));

    current_rb += sizeof(submit_rb_cmd_part1_e3k_t) >> 2;

    if (dma_task->flag.ignore_dma)
        goto fill_part3;

    zx_memset(&part2, 0, sizeof(submit_rb_cmd_part2_e3k_t));

    part2.dma_cmd.Major_Opcode = CSP_OPCODE_Dma;
    part2.dma_cmd.Mode         = DMA_MODE_NORMAL;

    for(i = 0; i < dma_cnt; i++)
    {
        node                 = &dma_task->dma_buffer_list[i];
        part2.dma_cmd.Dw_Num = node->command_length >> 2;//unit in dwords.
        part2.dma_cmd_addr_l = (unsigned int)(node->gpu_virtual_address & 0xFFFFFFFF);
        part2.dma_cmd_addr_h = (unsigned int)((node->gpu_virtual_address >> 32) & 0xFFFFFFFF);

        zx_memcpy(current_rb, &part2, sizeof(submit_rb_cmd_part2_e3k_t));

        current_rb += sizeof(submit_rb_cmd_part2_e3k_t) >> 2;
    }

    if(skip_count_uint)
    {
        *current_rb  = SEND_SKIP_E3K(skip_count_uint);
        current_rb  += skip_count_uint;
    }
fill_part3:
    if(engine_idx == RB_INDEX_GFXL)
    {
       part3.cacheflush_addr_l  = (unsigned int)(share->_3d_cacheflush_buffer_gpu_va & 0xFFFFFFFF);
       part3.cacheflush_addr_h  = (unsigned int)((share->_3d_cacheflush_buffer_gpu_va >> 32) & 0xFFFFFFFF);
    }

    part3.fence_addr_l = (unsigned int)(engine->fence_buffer_gpu_va & 0xFFFFFFFF);
    part3.fence_addr_h = (unsigned int)((engine->fence_buffer_gpu_va >> 32) & 0xFFFFFFFF);
    part3.fence_data_l = (unsigned int)(dma_task->desc.fence_id & 0xFFFFFFFF);
    part3.fence_data_h = (unsigned int)((dma_task->desc.fence_id >> 32) & 0xFFFFFFFF);

    part3.fence_addr_l_1 = (unsigned int)(engine->fence_buffer_gpu_va & 0xFFFFFFFF);
    part3.fence_addr_h_1 = (unsigned int)((engine->fence_buffer_gpu_va >> 32) & 0xFFFFFFFF);
    part3.fence_data_l_1 = (unsigned int)(dma_task->desc.fence_id & 0xFFFFFFFF);
    part3.fence_data_h_1 = (unsigned int)((dma_task->desc.fence_id >> 32) & 0xFFFFFFFF);


    if (dma_task->desc.type != task_type_render || dma_task->desc.need_send_irq || sch_mgr->monitor_count >= 4)
    {
        *(unsigned int*)(&part3.fence)         = SEND_EXTERNAL_FENCE_E3K(FENCE_IRQ_INTERRUPT_CPU);
        *(unsigned int*)(&part3.fence_1)       = SEND_EXTERNAL_FENCE_E3K(FENCE_IRQ_INTERRUPT_CPU);
        sch_mgr->monitor_count = 0;
    }
    else
    {
        sch_mgr->monitor_count++;
    }

    part3.trigger_off_dw.Slice_Mask = adapter->hw_caps.chip_slice_mask;

    part3.pwm_trigger_off.Process_Id = dma_task->desc.vm_id;
    zx_memcpy(current_rb, &part3, sizeof(submit_rb_cmd_part3_e3k_t));

    engine->last_send_fence_id = dma_task->desc.fence_id;
    engine->last_process_id = dma_task->desc.vm_id;

    if(adapter->ctl_flags.hang_dump == 2 && sch_mgr->engine_index == RB_INDEX_GFXL)
    {
        engine->last_ring_buffer = ring_buffer;
        engine->last_ring_buffer_size = (align_rb_size_uint << 2);
        engine->last_ttbr_gpu_va = dma_task->desc.context->device->vm->pt_info.root.addr;

        vidsch_pre_save_misc_e3k(adapter, sch_mgr, SAVE_RING_BUFFER);
    }

    if (dma_task->flag.dump_rb)
    {
        zx_info("vm_id = %d, fence = 0x%llx, client_type=%d, ctx_switch=%d\n",
                dma_task->desc.vm_id, dma_task->desc.fence_id, client_type, ctx_switch);
        util_dump_memory(ring_buffer, align_rb_size_uint << 2, "ring_buffer");
    }

    if (dma_task->flag.dump_dma)
    {
        for (i = 0; i < dma_cnt; i++)
        {
            node = &dma_task->dma_buffer_list[i];
            if (node->cpu_virtual_address)
            {
                zx_info("%p[%d]: gpu_va:%llx, cpu_va:0x%llx\n",
                        dma_task, i, node->gpu_virtual_address, node->cpu_virtual_address);

                util_dump_memory(node->cpu_virtual_address, node->command_length, "Dma");
            }
        }
    }

    enginei_release_ring_buffer_space_e3k(engine, align_rb_size_uint);
}

int enginei_write_commands_to_hw_video_e3k(engine_e3k_t *engine, task_dma_t *task)
{
    engine_share_e3k_t *share   = engine->share;
    adapter_t          *adapter = engine->sch_mgr->adapter;
    unsigned long long FenceOffset = engine->fence_buffer_gpu_va;
    unsigned long long FenceOffsetfake = engine->fence_buffer_gpu_va_fake;
    unsigned long long FenceId = task->desc.fence_id;
    dma_node_t    *node = &task->dma_buffer_list[0];//video should have only one dma
    unsigned int dwDmasize = node->command_length>>2;
    unsigned int SubmitSize = 0, dwCmdSize = 0;
    unsigned int RbIndex = engine->sch_mgr->engine_index;
    unsigned int *pRb = NULL;
    BOOL EnableVcpDecouple = TRUE;
    Cmd_Vcp_Fence Cmd_Vcp_Fence={ 0 };
    Cmd_Blk_Cmd_Csp_Indicator pwmTrigger = {0};
    Cmd_Vcp_Indicator         vcpProcIdIndicator = {0};
    int need_send_irq = 0;

    // add invalidate tlb before submitting the video command.
    enginei_mmio_invalidate_tlb_e3k(adapter);

    dwCmdSize = dwDmasize + EXTERNAL_FENCE_DWSIZE*2; // 4 is for external fence;

    if (IS_VPP_ENGINE(adapter->chip_id, RbIndex))
        dwCmdSize += 1; //for Process_Id

    if (IS_VCP_ENGINE(adapter->chip_id, RbIndex))
        dwCmdSize += 2; //for FE & BE Process_Id

    dwCmdSize = ((dwCmdSize +15)&~15);

    pRb = enginei_get_ring_buffer_space_e3k(engine, dwCmdSize);
    zx_memset(pRb, 0, (dwCmdSize << 2));

    if (IS_VPP_ENGINE(adapter->chip_id, RbIndex))
    {
        pwmTrigger.Major_Opcode = CSP_OPCODE_Blk_Cmd_Csp_Indicator;
        pwmTrigger.Block_Id     = VPP_BLOCK;
        pwmTrigger.Type         = BLOCK_COMMAND_CSP_TYPE_INDICATOR;
        pwmTrigger.Dwc          = 0;
        pwmTrigger.Process_Id   = task->desc.vm_id;

        *pRb++ = *(unsigned int *)&pwmTrigger;

        //zx_info("VPP vm_id %d \n", task->desc.vm_id);
    }

    if (IS_VCP_ENGINE(adapter->chip_id, RbIndex))
    {
        vcpProcIdIndicator.Major_Opcode = CSP_OPCODE_Blk_Cmd_Csp_Indicator;
        vcpProcIdIndicator.Block_Id     = 1; //for VCP BE
        vcpProcIdIndicator.Dwc          = 0;
        vcpProcIdIndicator.Process_Id   = task->desc.vm_id;
        *pRb++ = *(unsigned int *)&vcpProcIdIndicator;

        vcpProcIdIndicator.Major_Opcode = CSP_OPCODE_Blk_Cmd_Csp_Indicator;
        vcpProcIdIndicator.Block_Id = 0;    //for VCP FE
        vcpProcIdIndicator.Dwc = 0;
        vcpProcIdIndicator.Process_Id = task->desc.vm_id;

        *pRb++ = *(DWORD*)&vcpProcIdIndicator;
    }

    {
        unsigned char *cpu_virt_addr = node->cpu_virtual_address;;
        zx_memcpy(pRb, cpu_virt_addr, node->command_length);
    }

    pRb += dwDmasize;

    if (task->desc.need_send_irq || engine->sch_mgr->monitor_count >= 4)
    {
        need_send_irq = 1;
        engine->sch_mgr->monitor_count = 0;
    }
    else
    {
        engine->sch_mgr->monitor_count++;
    }

    if (IS_VCP_ENGINE(adapter->chip_id, RbIndex))
    {
        Cmd_Vcp_Fence.Fence_Type           = VCP_FENCE_FENCE_TYPE_EXTERNAL64;
        Cmd_Vcp_Fence.Dwc                  = 4;
        Cmd_Vcp_Fence.Irq                  = need_send_irq;
        Cmd_Vcp_Fence.Fence_Update_Mode    = 1;
        //On E3K chip, encoding use BE only, decoding use decouple mode, so always use BE to send external fence.
        if(EnableVcpDecouple)
        {
            Cmd_Vcp_Fence.Block_Id          = VCP_QUERY_DUMP_BLOCK_ID_VCP_BE;
            Cmd_Vcp_Fence.Major_Opcode      = VCP_OPCODE_Vcp_Fence; //0x8
        }
        else
        {
            //To do: for encoding, still needs BE to send external fence, but we need a flag to know it's encoding.
            Cmd_Vcp_Fence.Block_Id          = VCP_QUERY_DUMP_BLOCK_ID_VCP_FE;
            Cmd_Vcp_Fence.Major_Opcode      = VCP_OPCODE_Vcp_Fence; //0x8
        }
    }
    else if (IS_VPP_ENGINE(adapter->chip_id, RbIndex))
    {
        // VPP using the same fence struct as VCP.
        Cmd_Vcp_Fence.Major_Opcode      = CSP_OPCODE_Fence;
        Cmd_Vcp_Fence.Fence_Type        = FENCE_FENCE_TYPE_EXTERNAL64;
        Cmd_Vcp_Fence.Block_Id          = FENCE_ROUTE_ID_VPP_FENCE;
        Cmd_Vcp_Fence.Fence_Update_Mode = FENCE_FENCE_UPDATE_MODE_COPY;
        Cmd_Vcp_Fence.Dwc               = 4;
        Cmd_Vcp_Fence.Irq               = need_send_irq;
    }
    else
    {
        zx_error("%s should not enter here, engine_index-%d\n", __func__, RbIndex);
    }

    *pRb++ = *(DWORD*)&Cmd_Vcp_Fence;
    *pRb++ = (FenceOffset & 0xFFFFFFFF);
    *pRb++ = ((FenceOffset >> 32) &0xFFFFFFFF);
    *pRb++ = (FenceId & 0xFFFFFFFF);
    *pRb++ = ((FenceId >> 32) &0xFFFFFFFF);

    *pRb++ = *(DWORD*)&Cmd_Vcp_Fence;
    *pRb++ = (FenceOffsetfake & 0xFFFFFFFF);
    *pRb++ = ((FenceOffsetfake >> 32) &0xFFFFFFFF);
    *pRb++ = (FenceId & 0xFFFFFFFF);
    *pRb++ = ((FenceId >> 32) &0xFFFFFFFF);

    SubmitSize = ALIGNED_8(dwCmdSize);
    if (dwCmdSize < SubmitSize)
    {
        DWORD dwSkipSize = SubmitSize - dwCmdSize - 1;
        Cmd_Vcp_Skip cmd_Vcp_Skip = { 0 };

        if (dwSkipSize)
        {
            cmd_Vcp_Skip.Dwc          = dwSkipSize;
            cmd_Vcp_Skip.Major_Opcode = VCP_OPCODE_Vcp_Skip;
        }

        *pRb++ = *(DWORD*)&cmd_Vcp_Skip;
    }

    //enginei_do_kickoff_cmd_e3k(engine);
    enginei_release_ring_buffer_space_e3k(engine, dwCmdSize);

    engine->last_send_fence_id = task->desc.fence_id;
    engine->last_process_id = task->desc.vm_id;
    return S_OK;
}

int engine_submit_command_e3k(gpu_context_t *gpu_context, vidsch_mgr_t *sch_mgr, task_dma_t *dma_task)
{
    adapter_t *adapter = sch_mgr->adapter;
    engine_e3k_t *engine = sch_mgr->private_data;

    if (debug_mode_e3k.internal_dump_hw)
    {
        zx_info("engine_submit_task_dma_e3k start dump image, need skip this function...\n");
        return 0;
    }

    /*FIXME:
    Tmp move this trace submit to uniform vidschi_set_task_submitted.
    From esperf team, thus will miss some esperf data, will debug with them together if meet again
    */
    //vidschi_trace_task_submit(sch_mgr, &dma_task->desc, 0);
    if(adapter->ctl_flags.hwq_event_enable)
    {
        unsigned long long timestamp = 0;
        if(timestamp==0)
        {
            zx_get_nsecs(&timestamp);
        }
        hwq_process_submit_event(adapter,timestamp,dma_task->desc.fence_id & 0xffffffff,sch_mgr->engine_index);
    }

    switch(sch_mgr->engine_index)
    {
    case RB_INDEX_GFXL:
    case RB_INDEX_CSH:
    case RB_INDEX_CSL0:
    case RB_INDEX_CSL1:
    case RB_INDEX_CSL2:
    case RB_INDEX_CSL3:
        enginei_write_commands_to_gfx_low_e3k(engine, dma_task);

        break;
    case RB_INDEX_GFXH:
        break;

    case RB_INDEX_VCP0:
    case RB_INDEX_VCP1:
//    case RB_INDEX_VCP2:
    case RB_INDEX_VPP:
    case RB_INDEX_VPP0:
    case RB_INDEX_VPP1:
        enginei_write_commands_to_hw_video_e3k(engine, dma_task);
        break;

    default:
        zx_error("unknown rb index in submit %d", sch_mgr->engine_index);
        break;
    }



    return S_OK;
}


/*
begin/end buffer layout. both OGL/VULKAN share the same cacheflush_dma_3d.
|--OGL---     cacheflush_dma_3d
              context_restore_dma
              context_save_dma
|- VULKAN---  context_restore_dma
              context_save_dma
*/

void enginei_init_gfx_begin_end_commands_e3k(engine_gfx_e3k_t *engine)
{
    CACHEFLUSH_DMA_E3K      *pCacheFlushDMA3DL, *pCacheFlushDMACSL = NULL;
    CONTEXT_RESTORE_DMA_E3K *pRestoreDMA, *pRestoreDMACS,*pRestoreDMA_vk  = NULL;
    CONTEXT_SAVE_DMA_E3K    *pSaveDMA, *pSaveDMA_vk        = NULL;
    RB_PREDEFINE_DMA *pRBDMA = NULL;
    Reg_Eu_Full_Glb          reg_Eu_Full_Glb    = { 0 };
    Reg_Tu_Tssharp_Ctrl reg_Tu_Tssharp_Ctrl     = { 0 };
    Reg_Ffc_Ubuf_Golbalconfig reg_Ffc_Ubuf_Golbalconfig    = { 0 };
    unsigned int         reg_value = 0;
    unsigned int         offset    = 0;

    engine_e3k_t         *common    = &engine->common;
    engine_share_e3k_t   *share     = common->share;
    vidsch_mgr_t         *sch_mgr   = common->sch_mgr;
    adapter_t            *adapter   = sch_mgr->adapter;

    unsigned int dwSliceMask = adapter->hw_caps.chip_slice_mask;
    unsigned int GpcBitMask = 0, ShiftNum = 1, bitMask = 0;

    zx_memset(share->begin_end_buffer_cpu_va, 0, sizeof(RB_PREDEFINE_DMA));

    while (dwSliceMask)
    {
        if (dwSliceMask & 0xf)
        {
            GpcBitMask |= ShiftNum;
        }
        dwSliceMask >>= 4;
        ShiftNum <<= 1;
    }

    pRBDMA = (RB_PREDEFINE_DMA *)share->begin_end_buffer_cpu_va;
    pRestoreDMA = &pRBDMA->RestoreDMA3D;
    share->context_restore_dma_gpu_va = share->begin_end_buffer_gpu_va + (unsigned char*)pRestoreDMA - (unsigned char*)pRBDMA;

    pRestoreDMACS = &pRBDMA->RestoreDMACS;
    share->context_restore_dma_gpu_va_cs= share->begin_end_buffer_gpu_va + (unsigned char*)pRestoreDMACS - (unsigned char*)pRBDMA;

    pRestoreDMA_vk= &pRBDMA->RestoreDMAvk;
    share->context_restore_dma_gpu_va_for_vk = share->begin_end_buffer_gpu_va + (unsigned char*)pRestoreDMA_vk - (unsigned char*)pRBDMA;

    pCacheFlushDMA3DL = &pRBDMA->FlushDMA3DL;
    share->_3d_cacheflush_buffer_gpu_va = share->begin_end_buffer_gpu_va + (unsigned char*)pCacheFlushDMA3DL - (unsigned char*)pRBDMA;

    pCacheFlushDMACSL = &pRBDMA->FlushDMACSL;
    share->_ocl_cacheflush_buffer_gpu_va = share->begin_end_buffer_gpu_va +(unsigned char*) pCacheFlushDMACSL - (unsigned char*)pRBDMA;

    pSaveDMA = &pRBDMA->SaveDMA;
    share->context_save_dma_gpu_va = share->begin_end_buffer_gpu_va + (unsigned char*)pSaveDMA - (unsigned char*)pRBDMA;

    pSaveDMA_vk= &pRBDMA->SaveDMAvk;
    share->context_save_dma_gpu_va_for_vk = share->begin_end_buffer_gpu_va + (unsigned char*)pSaveDMA_vk - (unsigned char*)pRBDMA;

    reg_Eu_Full_Glb.reg.Uav_En = 1;
    reg_Eu_Full_Glb.reg.Uav_Continue_Chk = 1;
    reg_Eu_Full_Glb.reg.Uav_Buf_Size = EU_FULL_GLB_UAV_BUF_SIZE_32LINES;
    reg_Eu_Full_Glb.reg.U_3d_Base = 6;//U_SHARP_3D_SLOT_START / 8;
    reg_Eu_Full_Glb.reg.Cb_3d_Base = 0;//CB_3D_SLOT_START >> 4;
    reg_Eu_Full_Glb.reg.Ic_To_L2 = 0; //temply turn on IC to L2

    reg_Ffc_Ubuf_Golbalconfig.reg.U_3d_Base = 6;//U_SHARP_3D_SLOT_START/8

    //RESTORE DMA
    pRestoreDMA->RestoreContext.Major_Opcode = CSP_OPCODE_Blk_Cmd_Csp_Save_Rsto;
    pRestoreDMA->RestoreContext.Block_Id = CSP_GLOBAL_BLOCK;
    pRestoreDMA->RestoreContext.Type = BLOCK_COMMAND_CSP_TYPE_RESTORE;
    pRestoreDMA->RestoreContext.Address_Mode = SET_REGISTER_ADDRESS_MODE_OFFSET;
    pRestoreDMA->RestoreContext.Dwc = 2;
    pRestoreDMA->ContextBufferOffset_L = 0;
    pRestoreDMA->ContextBufferOffset_H = 0;

    pRestoreDMA->RestoreEuFullGlb.uint = SET_REGISTER_FD_E3K(EU_FS_BLOCK, Reg_Eu_Full_Glb_Offset, 1);
    pRestoreDMA->EuFullGlb = reg_Eu_Full_Glb.uint;

    pRestoreDMA->RestoreTsSharpCtrl.uint = SET_REGISTER_FD_E3K(TUFE_BLOCK, Reg_Tu_Tssharp_Ctrl_Offset, 1);
    pRestoreDMA->TsSharpCtrl = reg_Tu_Tssharp_Ctrl.uint;

    pRestoreDMA->RestoreFFCUbufConfig.uint = SET_REGISTER_FD_E3K(WLS_FE_BLOCK, Reg_Ffc_Ubuf_Golbalconfig_Offset, 1);
    pRestoreDMA->FFCUbufConfig = reg_Ffc_Ubuf_Golbalconfig.uint;

    if (adapter->chip_id >= CHIP_CHX004)
    {
        pRestoreDMA->RestoreCsp.uint = SET_REGISTER_ADDR_E3K(CSP_GLOBAL_BLOCK, Reg_Csp_Ref_Total_Gpu_Timestamp_Offset, SET_REGISTER_ADDRESS_MODE_OFFSET);
        pRestoreDMA->CSPOffset_L = SET_REGISTER_ADDR_LOW_E3K((unsigned long)(&(((CONTEXT_BUFFER_CHX004*)0)->CSPRegs)));
        pRestoreDMA->CSPOffset_H = SET_REGISTER_ADDR_HIGH_AND_REGCNT_E3K(0x0, sizeof(CSP_CONTEXTBUFFER_REGS_CHX004) >> 2, 0);

        pRestoreDMA->RestoreGpcpFe.uint = SET_REGISTER_ADDR_E3K(GPCPFE_BLOCK, Reg_Gpcpfe_Iidcnt_Offset, SET_REGISTER_ADDRESS_MODE_OFFSET);
        pRestoreDMA->GpcpFeOffset_L = SET_REGISTER_ADDR_LOW_E3K((unsigned long)(&(((CONTEXT_BUFFER_CHX004*)0)->GpcpFeRegs)));
        pRestoreDMA->GpcpFeOffset_H = SET_REGISTER_ADDR_HIGH_AND_REGCNT_E3K(0x0, sizeof(GPCPFE_CONTEXTBUFFER_REGS) >> 2, 0);

        pRestoreDMA->RestoreGpcpBe.uint = SET_REGISTER_ADDR_E3K(GPCPBE_BLOCK, Reg_Sto_Cfg_Offset, SET_REGISTER_ADDRESS_MODE_OFFSET);
        pRestoreDMA->GpcpBeOffset_L = SET_REGISTER_ADDR_LOW_E3K((unsigned long)(&(((CONTEXT_BUFFER_CHX004*)0)->GpcpBeRegs)));
        pRestoreDMA->GpcpBeOffset_H = SET_REGISTER_ADDR_HIGH_AND_REGCNT_E3K(0x0, sizeof(Gpcpbe_regs) >> 2, 0);

        pRestoreDMA->RestoreGpcTop[0].RestoreGpcTopCmd.uint = SET_REGISTER_ADDR_E3K(24, 0, SET_REGISTER_ADDRESS_MODE_OFFSET);
        pRestoreDMA->RestoreGpcTop[0].GpcTopOffset_L = SET_REGISTER_ADDR_LOW_E3K(
                                                        (unsigned long)(&(((CONTEXT_BUFFER_CHX004*)0)->GpcTopRegs[0])));
        pRestoreDMA->RestoreGpcTop[0].GpcTopOffset_H = SET_REGISTER_ADDR_HIGH_AND_REGCNT_E3K(0x0, 40, 0);
    }
    else
    {
        pRestoreDMA->RestoreCsp.uint = SET_REGISTER_ADDR_E3K(CSP_GLOBAL_BLOCK, Reg_Csp_Ms_Total_Gpu_Timestamp_Offset, SET_REGISTER_ADDRESS_MODE_OFFSET);
        pRestoreDMA->CSPOffset_L = SET_REGISTER_ADDR_LOW_E3K((unsigned long)(&(((CONTEXT_BUFFER*)0)->CSPRegs)));
        pRestoreDMA->CSPOffset_H = SET_REGISTER_ADDR_HIGH_AND_REGCNT_E3K(0x0, sizeof(CSP_CONTEXTBUFFER_REGS) >> 2, 0);

        pRestoreDMA->RestoreGpcpFe.uint = SET_REGISTER_ADDR_E3K(GPCPFE_BLOCK, Reg_Gpcpfe_Iidcnt_Offset, SET_REGISTER_ADDRESS_MODE_OFFSET);
        pRestoreDMA->GpcpFeOffset_L = SET_REGISTER_ADDR_LOW_E3K((unsigned long)(&(((CONTEXT_BUFFER*)0)->GpcpFeRegs)));
        pRestoreDMA->GpcpFeOffset_H = SET_REGISTER_ADDR_HIGH_AND_REGCNT_E3K(0x0, sizeof(GPCPFE_CONTEXTBUFFER_REGS) >> 2, 0);

        pRestoreDMA->RestoreGpcpBe.uint = SET_REGISTER_ADDR_E3K(GPCPBE_BLOCK, Reg_Sto_Cfg_Offset, SET_REGISTER_ADDRESS_MODE_OFFSET);
        pRestoreDMA->GpcpBeOffset_L = SET_REGISTER_ADDR_LOW_E3K((unsigned long)(&(((CONTEXT_BUFFER*)0)->GpcpBeRegs)));
        pRestoreDMA->GpcpBeOffset_H = SET_REGISTER_ADDR_HIGH_AND_REGCNT_E3K(0x0, sizeof(Gpcpbe_regs) >> 2, 0);
        bitMask = GpcBitMask;
        while (bitMask)
        {
            unsigned int gpcIndex = 0;
            _BitScanForward(&gpcIndex, bitMask);
            _bittestandreset(&bitMask, gpcIndex);
            pRestoreDMA->RestoreGpcTop[gpcIndex].RestoreGpcTopCmd.uint = SET_REGISTER_ADDR_E3K(24, 0+ gpcIndex *40, SET_REGISTER_ADDRESS_MODE_OFFSET);
            pRestoreDMA->RestoreGpcTop[gpcIndex].GpcTopOffset_L = SET_REGISTER_ADDR_LOW_E3K((unsigned long)(&(((CONTEXT_BUFFER*)0)->GpcTopRegs[gpcIndex])));
            pRestoreDMA->RestoreGpcTop[gpcIndex].GpcTopOffset_H = SET_REGISTER_ADDR_HIGH_AND_REGCNT_E3K(0x0, 40, 0);
        }
    }


    pRestoreDMA->CspFenceCmd.uint = SEND_INTERNAL_WRITEFENCE_E3K(FENCE_ROUTE_ID_CSP_FENCE, _RBT_3DFE, HWM_SYNC_KMD_SLOT);
    pRestoreDMA->CspFenceValue = FENCE_VALUE_BEGIN_DMA;
    pRestoreDMA->CspWaitCmd.uint = SEND_INTERNAL_WAIT_E3K(HWM_SYNC_KMD_SLOT, WAIT_METHOD_EQUAL, FENCE_VALUE_BEGIN_DMA);
    pRestoreDMA->CspWaitMainCmd.uint = SEND_INTERNAL_WAIT_MAIN_E3K(HWM_SYNC_KMD_SLOT, WAIT_METHOD_EQUAL, FENCE_VALUE_BEGIN_DMA);

    //Add EU fs ps L1 invalidate in restore context, DTM mipgen.exe has 2 context switch case.
    //Context restore may modify EU code register. Add this code to fix mipgen crash.
    pRestoreDMA->EUFSL1Invalidate.uint = SEND_FS_L1I_INVALIDATE_COMMAND_E3K();
    pRestoreDMA->EUPSL1Invalidate.uint = SEND_PS_L1I_INVALIDATE_COMMAND_E3K();

    //CS RESTORE DMA
    zx_memcpy(pRestoreDMACS, pRestoreDMA, sizeof(CONTEXT_RESTORE_DMA_E3K));

    reg_Eu_Full_Glb.reg.Uav_Buf_Size                = EU_FULL_GLB_UAV_BUF_SIZE_128LINES;
    pRestoreDMACS->EuFullGlb                        = reg_Eu_Full_Glb.uint;

    //SAVE DMA
    pSaveDMA->SaveContext.Major_Opcode = CSP_OPCODE_Blk_Cmd_Csp_Save_Rsto;
    pSaveDMA->SaveContext.Block_Id = CSP_GLOBAL_BLOCK;
    pSaveDMA->SaveContext.Type = BLOCK_COMMAND_CSP_TYPE_SAVE;
    pSaveDMA->SaveContext.Address_Mode = SET_REGISTER_ADDRESS_MODE_OFFSET;
    pSaveDMA->SaveContext.Dwc = 2;
    pSaveDMA->ContextBufferOffset_L = 0;
    pSaveDMA->ContextBufferOffset_H = 0;

    if (adapter->chip_id >= CHIP_CHX004)
    {
        pSaveDMA->SaveCSPRegister.uint = SEND_QUERY_DUMP_E3K(CSP_GLOBAL_BLOCK, sizeof(CSP_CONTEXTBUFFER_REGS_CHX004) >> 2, QUERY_DUMP_ADDRESS_MODE_OFFSET);
        pSaveDMA->CSPOffset_L = SEND_QUERY_ADDR1_E3K((unsigned long)(&(((CONTEXT_BUFFER_CHX004*)0)->CSPRegs)));
        pSaveDMA->CSPOffset_H = SEND_QUERY_ADDR2_AND_OFFSET_E3K(0x0, Reg_Csp_Ref_Total_Gpu_Timestamp_Offset, TRUE, FALSE);

        pSaveDMA->SaveGpcpFeRegister.uint = SEND_QUERY_DUMP_E3K(GPCPFE_BLOCK, sizeof(GPCPFE_CONTEXTBUFFER_REGS) >> 2, QUERY_DUMP_ADDRESS_MODE_OFFSET);
        pSaveDMA->GpcpFeOffset_L = SEND_QUERY_ADDR1_E3K((unsigned long)(&(((CONTEXT_BUFFER_CHX004*)0)->GpcpFeRegs)));
        pSaveDMA->GpcpFeOffset_H = SEND_QUERY_ADDR2_AND_OFFSET_E3K(0x0, Reg_Gpcpfe_Iidcnt_Offset, TRUE, FALSE);

        pSaveDMA->SaveGpcpBeRegister.uint = SEND_QUERY_DUMP_E3K(GPCPBE_BLOCK, sizeof(Gpcpbe_regs) >> 2, QUERY_DUMP_ADDRESS_MODE_OFFSET);
        pSaveDMA->GpcpBeOffset_L = SEND_QUERY_ADDR1_E3K((unsigned long)(&(((CONTEXT_BUFFER_CHX004*)0)->GpcpBeRegs)));
        pSaveDMA->GpcpBeOffset_H = SEND_QUERY_ADDR2_AND_OFFSET_E3K(0x0, Reg_Sto_Cfg_Offset, TRUE, FALSE);
        pSaveDMA->SaveGpcTop[0].SaveGpcTopRegister.uint = SEND_QUERY_DUMP_E3K(24, 40, SET_REGISTER_ADDRESS_MODE_OFFSET);
        pSaveDMA->SaveGpcTop[0].GpcTopOffset_L = SEND_QUERY_ADDR1_E3K((unsigned long)(&(((CONTEXT_BUFFER_CHX004*)0)->GpcTopRegs[0])));
        pSaveDMA->SaveGpcTop[0].GpcTopOffset_H = SEND_QUERY_ADDR2_AND_OFFSET_E3K(0x0, 0, TRUE, FALSE);
    }
    else
    {
        pSaveDMA->SaveCSPRegister.uint = SEND_QUERY_DUMP_E3K(CSP_GLOBAL_BLOCK, sizeof(CSP_CONTEXTBUFFER_REGS) >> 2, QUERY_DUMP_ADDRESS_MODE_OFFSET);
        pSaveDMA->CSPOffset_L = SEND_QUERY_ADDR1_E3K((unsigned long)(&(((CONTEXT_BUFFER*)0)->CSPRegs)));
        pSaveDMA->CSPOffset_H = SEND_QUERY_ADDR2_AND_OFFSET_E3K(0x0, Reg_Csp_Ms_Total_Gpu_Timestamp_Offset, TRUE, FALSE);

        pSaveDMA->SaveGpcpFeRegister.uint = SEND_QUERY_DUMP_E3K(GPCPFE_BLOCK, sizeof(GPCPFE_CONTEXTBUFFER_REGS) >> 2, QUERY_DUMP_ADDRESS_MODE_OFFSET);
        pSaveDMA->GpcpFeOffset_L = SEND_QUERY_ADDR1_E3K((unsigned long)(&(((CONTEXT_BUFFER*)0)->GpcpFeRegs)));
        pSaveDMA->GpcpFeOffset_H = SEND_QUERY_ADDR2_AND_OFFSET_E3K(0x0, Reg_Gpcpfe_Iidcnt_Offset, TRUE, FALSE);

        pSaveDMA->SaveGpcpBeRegister.uint = SEND_QUERY_DUMP_E3K(GPCPBE_BLOCK, sizeof(Gpcpbe_regs) >> 2, QUERY_DUMP_ADDRESS_MODE_OFFSET);
        pSaveDMA->GpcpBeOffset_L = SEND_QUERY_ADDR1_E3K((unsigned long)(&(((CONTEXT_BUFFER*)0)->GpcpBeRegs)));
        pSaveDMA->GpcpBeOffset_H = SEND_QUERY_ADDR2_AND_OFFSET_E3K(0x0, Reg_Sto_Cfg_Offset, TRUE, FALSE);
        bitMask = GpcBitMask;
        while (bitMask)
        {
            unsigned int gpcIndex = 0;
            _BitScanForward(&gpcIndex, bitMask);
            _bittestandreset(&bitMask, gpcIndex);
            pSaveDMA->SaveGpcTop[gpcIndex].SaveGpcTopRegister.uint = SEND_QUERY_DUMP_E3K(24, 40, SET_REGISTER_ADDRESS_MODE_OFFSET);
            pSaveDMA->SaveGpcTop[gpcIndex].GpcTopOffset_L = SEND_QUERY_ADDR1_E3K((unsigned long)(&(((CONTEXT_BUFFER*)0)->GpcTopRegs[gpcIndex])));
            pSaveDMA->SaveGpcTop[gpcIndex].GpcTopOffset_H = SEND_QUERY_ADDR2_AND_OFFSET_E3K(0x0, 40* gpcIndex, TRUE, FALSE);
        }
    }

    pSaveDMA->CspFenceCmd.uint = SEND_INTERNAL_WRITEFENCE_E3K(FENCE_ROUTE_ID_CSP_FENCE, _RBT_3DFE, HWM_SYNC_KMD_SLOT);
    pSaveDMA->CspFenceValue = SEND_INTERNAL_FENCE_VALUE_E3K(FENCE_VALUE_END_DMA_CSPFENCE);
    pSaveDMA->CspWaitCmd.uint = SEND_INTERNAL_WAIT_E3K(HWM_SYNC_KMD_SLOT, WAIT_METHOD_EQUAL, FENCE_VALUE_END_DMA_CSPFENCE);
    pSaveDMA->CspWaitMainCmd.uint = SEND_INTERNAL_WAIT_MAIN_E3K(HWM_SYNC_KMD_SLOT, WAIT_METHOD_EQUAL, FENCE_VALUE_END_DMA_CSPFENCE);

    pSaveDMA->GpcpBeFenceCmd.uint = SEND_INTERNAL_WRITEFENCE_E3K(FENCE_ROUTE_ID_FS_STO_FENCE, _RBT_3DFE, HWM_SYNC_KMD_SLOT);
    pSaveDMA->GpcpBeFenceValue = SEND_INTERNAL_FENCE_VALUE_E3K(FENCE_VALUE_END_DMA_STOFENCE);
    pSaveDMA->GpcpBeWaitCmd.uint = SEND_INTERNAL_WAIT_E3K(HWM_SYNC_KMD_SLOT, WAIT_METHOD_EQUAL, FENCE_VALUE_END_DMA_STOFENCE);
    pSaveDMA->GpcpBeWaitMainCmd.uint = SEND_INTERNAL_WAIT_MAIN_E3K(HWM_SYNC_KMD_SLOT, WAIT_METHOD_EQUAL, FENCE_VALUE_END_DMA_STOFENCE);

    //CACHEFLUSH DMA
    pCacheFlushDMA3DL->DZS_Flush.Type               = BLOCK_COMMAND_TEMPLATE_TYPE_FLUSH;
    pCacheFlushDMA3DL->DZS_Flush.Target             = BLOCK_COMMAND_FLUSH_TARGET_ALL_C;
    pCacheFlushDMA3DL->DZS_Flush.Block_Id           = FF_BLOCK;
    pCacheFlushDMA3DL->DZS_Flush.Major_Opcode       = CSP_OPCODE_Block_Command_Flush;

    pCacheFlushDMA3DL->DZS_Invalidate.Type          = BLOCK_COMMAND_TEMPLATE_TYPE_INVALIDATE_CACHE;
    pCacheFlushDMA3DL->DZS_Invalidate.Target        = BLOCK_COMMAND_FLUSH_TARGET_ALL_C;
    pCacheFlushDMA3DL->DZS_Invalidate.Block_Id      = FF_BLOCK;
    pCacheFlushDMA3DL->DZS_Invalidate.Major_Opcode  = CSP_OPCODE_Block_Command_Flush;

    pCacheFlushDMA3DL->UAVFE_Flush.uint             = SEND_UCACHE_3DFE_FLUSH_COMMAND_E3K();
    pCacheFlushDMA3DL->UAVFE_Invalidate.uint        = SEND_UCACHE_3DFE_INVALIDATE_COMMAND_E3K();
    pCacheFlushDMA3DL->UAVBE_Flush.uint             = SEND_UCACHE_3DBE_FLUSH_COMMAND_E3K();
    pCacheFlushDMA3DL->UAVBE_Invalidate.uint        = SEND_UCACHE_3DBE_INVALIDATE_COMMAND_E3K();
    pCacheFlushDMA3DL->UAVCS_Flush.uint             = 0;
    pCacheFlushDMA3DL->UAVCS_Invalidate.uint        = 0;

    pCacheFlushDMA3DL->L2Flush.Block_Id             = L2_BLOCK;
    pCacheFlushDMA3DL->L2Flush.Usage                = BLOCK_COMMAND_L2_USAGE_ALL;
    pCacheFlushDMA3DL->L2Flush.Type                 = BLOCK_COMMAND_L2_TYPE_FLUSH_L2;
    pCacheFlushDMA3DL->L2Flush.Major_Opcode         = CSP_OPCODE_Block_Command_L2;
    pCacheFlushDMA3DL->L2Flush.Dwc                  = 0;

    pCacheFlushDMA3DL->L2Invalidate.Block_Id        = L2_BLOCK;
    pCacheFlushDMA3DL->L2Invalidate.Usage           = BLOCK_COMMAND_L2_USAGE_ALL;
    pCacheFlushDMA3DL->L2Invalidate.Type            = BLOCK_COMMAND_L2_TYPE_INVALIDATE_L2;
    pCacheFlushDMA3DL->L2Invalidate.Major_Opcode    = CSP_OPCODE_Block_Command_L2;
    pCacheFlushDMA3DL->L2Invalidate.Dwc             = 0;

    pCacheFlushDMA3DL->TUFEL1Invalidate.uint        = SEND_TUFE_L1CACHE_INVALIDATE_COMMAND_E3K();
    pCacheFlushDMA3DL->TUBEL1Invalidate.uint        = SEND_TUBE_L1CACHE_INVALIDATE_COMMAND_E3K();

    pCacheFlushDMA3DL->EUFSL1Invalidate.uint        = SEND_FS_L1I_INVALIDATE_COMMAND_E3K();
    pCacheFlushDMA3DL->EUPSL1Invalidate.uint        = SEND_PS_L1I_INVALIDATE_COMMAND_E3K();

    pCacheFlushDMA3DL->PreUAVFEFenceCmd.uint        = SEND_INTERNAL_WRITEFENCE_E3K(FENCE_ROUTE_ID_UAVFE_FENCE,_RBT_3DFE, HWM_SYNC_KMD_SLOT);
    pCacheFlushDMA3DL->PreUAVFEFenceValue           = SEND_INTERNAL_FENCE_VALUE_E3K(FENCE_VALUE_CACHE_DMA_UAVFEFENCE);
    pCacheFlushDMA3DL->PreUAVFEWaitCmd.uint         = SEND_INTERNAL_WAIT_E3K(HWM_SYNC_KMD_SLOT, WAIT_METHOD_EQUAL, FENCE_VALUE_CACHE_DMA_UAVFEFENCE);
    pCacheFlushDMA3DL->PreUAVFEWaitMainCmd.uint     = SEND_INTERNAL_WAIT_MAIN_E3K(HWM_SYNC_KMD_SLOT, WAIT_METHOD_EQUAL, FENCE_VALUE_CACHE_DMA_UAVFEFENCE);

    pCacheFlushDMA3DL->PreUAVBEFenceCmd.uint        = SEND_INTERNAL_WRITEFENCE_E3K(FENCE_ROUTE_ID_UAVBE_FENCE,_RBT_3DBE, HWM_SYNC_KMD_SLOT);
    pCacheFlushDMA3DL->PreUAVBEFenceValue           = SEND_INTERNAL_FENCE_VALUE_E3K(FENCE_VALUE_CACHE_DMA_UAVBEFENCE);
    pCacheFlushDMA3DL->PreUAVBEWaitCmd.uint         = SEND_INTERNAL_WAIT_E3K(HWM_SYNC_KMD_SLOT, WAIT_METHOD_EQUAL, FENCE_VALUE_CACHE_DMA_UAVBEFENCE);
    pCacheFlushDMA3DL->PreUAVBEWaitMainCmd.uint     = SEND_INTERNAL_WAIT_MAIN_E3K(HWM_SYNC_KMD_SLOT, WAIT_METHOD_EQUAL, FENCE_VALUE_CACHE_DMA_UAVBEFENCE);

    pCacheFlushDMA3DL->PreDFenceCmd.uint            = SEND_INTERNAL_WRITEFENCE_E3K(FENCE_ROUTE_ID_D_FENCE,_RBT_3DBE, HWM_SYNC_KMD_SLOT);
    pCacheFlushDMA3DL->PreDFenceValue               = SEND_INTERNAL_FENCE_VALUE_E3K(FENCE_VALUE_CACHE_DMA_DFENCE);
    pCacheFlushDMA3DL->PreDWaitCmd.uint             = SEND_INTERNAL_WAIT_E3K(HWM_SYNC_KMD_SLOT, WAIT_METHOD_EQUAL, FENCE_VALUE_CACHE_DMA_DFENCE);
    pCacheFlushDMA3DL->PreDWaitMainCmd.uint         = SEND_INTERNAL_WAIT_MAIN_E3K(HWM_SYNC_KMD_SLOT, WAIT_METHOD_EQUAL, FENCE_VALUE_CACHE_DMA_DFENCE);


    pCacheFlushDMA3DL->PreZFenceCmd.uint            = SEND_INTERNAL_WRITEFENCE_E3K(FENCE_ROUTE_ID_Z_FENCE,_RBT_3DBE, HWM_SYNC_KMD_SLOT);
    pCacheFlushDMA3DL->PreZFenceValue               = SEND_INTERNAL_FENCE_VALUE_E3K(FENCE_VALUE_CACHE_DMA_ZFENCE);
    pCacheFlushDMA3DL->PreZWaitCmd.uint             = SEND_INTERNAL_WAIT_E3K(HWM_SYNC_KMD_SLOT, WAIT_METHOD_EQUAL, FENCE_VALUE_CACHE_DMA_ZFENCE);
    pCacheFlushDMA3DL->PreZWaitMainCmd.uint         = SEND_INTERNAL_WAIT_MAIN_E3K(HWM_SYNC_KMD_SLOT, WAIT_METHOD_EQUAL, FENCE_VALUE_CACHE_DMA_ZFENCE);
    //post is totally same as pre ones.
    pCacheFlushDMA3DL->PostUAVFEFenceCmd.uint       = SEND_INTERNAL_WRITEFENCE_E3K(FENCE_ROUTE_ID_UAVFE_FENCE,_RBT_3DFE, HWM_SYNC_KMD_SLOT);
    pCacheFlushDMA3DL->PostUAVFEFenceValue          = SEND_INTERNAL_FENCE_VALUE_E3K(FENCE_VALUE_CACHE_DMA_UAVFEFENCE);
    pCacheFlushDMA3DL->PostUAVFEWaitCmd.uint        = SEND_INTERNAL_WAIT_E3K(HWM_SYNC_KMD_SLOT, WAIT_METHOD_EQUAL, FENCE_VALUE_CACHE_DMA_UAVFEFENCE);
    pCacheFlushDMA3DL->PostUAVFEWaitMainCmd.uint    = SEND_INTERNAL_WAIT_MAIN_E3K(HWM_SYNC_KMD_SLOT, WAIT_METHOD_EQUAL, FENCE_VALUE_CACHE_DMA_UAVFEFENCE);

    pCacheFlushDMA3DL->PostUAVBEFenceCmd.uint       = SEND_INTERNAL_WRITEFENCE_E3K(FENCE_ROUTE_ID_UAVBE_FENCE,_RBT_3DBE, HWM_SYNC_KMD_SLOT);
    pCacheFlushDMA3DL->PostUAVBEFenceValue          = SEND_INTERNAL_FENCE_VALUE_E3K(FENCE_VALUE_CACHE_DMA_UAVBEFENCE);
    pCacheFlushDMA3DL->PostUAVBEWaitCmd.uint        = SEND_INTERNAL_WAIT_E3K(HWM_SYNC_KMD_SLOT, WAIT_METHOD_EQUAL, FENCE_VALUE_CACHE_DMA_UAVBEFENCE);
    pCacheFlushDMA3DL->PostUAVBEWaitMainCmd.uint    = SEND_INTERNAL_WAIT_MAIN_E3K(HWM_SYNC_KMD_SLOT, WAIT_METHOD_EQUAL, FENCE_VALUE_CACHE_DMA_UAVBEFENCE);

    pCacheFlushDMA3DL->PostDFenceCmd.uint           = SEND_INTERNAL_WRITEFENCE_E3K(FENCE_ROUTE_ID_D_FENCE,_RBT_3DBE, HWM_SYNC_KMD_SLOT);
    pCacheFlushDMA3DL->PostDFenceValue              = SEND_INTERNAL_FENCE_VALUE_E3K(FENCE_VALUE_CACHE_DMA_DFENCE);
    pCacheFlushDMA3DL->PostDWaitCmd.uint            = SEND_INTERNAL_WAIT_E3K(HWM_SYNC_KMD_SLOT, WAIT_METHOD_EQUAL, FENCE_VALUE_CACHE_DMA_DFENCE);
    pCacheFlushDMA3DL->PostDWaitMainCmd.uint        = SEND_INTERNAL_WAIT_MAIN_E3K(HWM_SYNC_KMD_SLOT, WAIT_METHOD_EQUAL, FENCE_VALUE_CACHE_DMA_DFENCE);

    pCacheFlushDMA3DL->PostZFenceCmd.uint           = SEND_INTERNAL_WRITEFENCE_E3K(FENCE_ROUTE_ID_Z_FENCE,_RBT_3DBE, HWM_SYNC_KMD_SLOT);
    pCacheFlushDMA3DL->PostZFenceValue              = SEND_INTERNAL_FENCE_VALUE_E3K(FENCE_VALUE_CACHE_DMA_ZFENCE);
    pCacheFlushDMA3DL->PostZWaitCmd.uint            = SEND_INTERNAL_WAIT_E3K(HWM_SYNC_KMD_SLOT, WAIT_METHOD_EQUAL, FENCE_VALUE_CACHE_DMA_ZFENCE);
    pCacheFlushDMA3DL->PostZWaitMainCmd.uint        = SEND_INTERNAL_WAIT_MAIN_E3K(HWM_SYNC_KMD_SLOT, WAIT_METHOD_EQUAL, FENCE_VALUE_CACHE_DMA_ZFENCE);

    pCacheFlushDMA3DL->StoFenceCmd.uint             = SEND_INTERNAL_WRITEFENCE_E3K(FENCE_ROUTE_ID_FS_STO_FENCE,_RBT_3DFE, HWM_SYNC_KMD_SLOT);
    pCacheFlushDMA3DL->StoFenceValue                = SEND_INTERNAL_FENCE_VALUE_E3K(FENCE_VALUE_CACHE_DMA_STOFENCE);
    pCacheFlushDMA3DL->StoWaitCmd.uint              = SEND_INTERNAL_WAIT_E3K(HWM_SYNC_KMD_SLOT, WAIT_METHOD_EQUAL, FENCE_VALUE_CACHE_DMA_STOFENCE);
    pCacheFlushDMA3DL->StoWaitMainCmd.uint          = SEND_INTERNAL_WAIT_MAIN_E3K(HWM_SYNC_KMD_SLOT, WAIT_METHOD_EQUAL, FENCE_VALUE_CACHE_DMA_STOFENCE);

    pCacheFlushDMA3DL->L2FenceCmd.uint              = SEND_INTERNAL_WRITEFENCE_E3K(FENCE_ROUTE_ID_L2_FENCE,_RBT_3DBE, HWM_SYNC_KMD_SLOT);
    pCacheFlushDMA3DL->L2FenceValue                 = FENCE_VALUE_CACHE_DMA_L2C_FENCE;

    pCacheFlushDMA3DL->L2WaitCmd.uint               = SEND_INTERNAL_WAIT_E3K(HWM_SYNC_KMD_SLOT, WAIT_METHOD_EQUAL, FENCE_VALUE_CACHE_DMA_L2C_FENCE);
    pCacheFlushDMA3DL->L2WaitMainCmd.uint           = SEND_INTERNAL_WAIT_MAIN_E3K(HWM_SYNC_KMD_SLOT, WAIT_METHOD_EQUAL, FENCE_VALUE_CACHE_DMA_L2C_FENCE);

    pCacheFlushDMA3DL->PreCspFenceCmd.uint          = SEND_INTERNAL_WRITEFENCE_E3K(FENCE_ROUTE_ID_CSP_FENCE,_RBT_3DFE, HWM_SYNC_KMD_SLOT);
    pCacheFlushDMA3DL->PreCspFenceValue             = SEND_INTERNAL_FENCE_VALUE_E3K(FENCE_VALUE_CACHE_DMA_PRECSPFENCE);
    pCacheFlushDMA3DL->PreCspWaitCmd.uint           = SEND_INTERNAL_WAIT_E3K(HWM_SYNC_KMD_SLOT, WAIT_METHOD_EQUAL, FENCE_VALUE_CACHE_DMA_PRECSPFENCE);
    pCacheFlushDMA3DL->PreCspWaitMainCmd.uint       = SEND_INTERNAL_WAIT_MAIN_E3K(HWM_SYNC_KMD_SLOT, WAIT_METHOD_EQUAL, FENCE_VALUE_CACHE_DMA_PRECSPFENCE);

    /*we need first flush the cache, ensure last write will flush out to memory*/
    reg_value                                        = SEND_FLAGBUFFER_FLUSH_COMMAND_E3K();
    pCacheFlushDMA3DL->BLC_Flush                    = *(Cmd_Block_Command_Mxu*)&reg_value;
    pCacheFlushDMA3DL->FlushAddr_L                  = 0;
    pCacheFlushDMA3DL->FlushAddr_H                  = 0;
    pCacheFlushDMA3DL->FlushMask_L                  = 0;
    pCacheFlushDMA3DL->FlushMask_H                  = 0;

    pCacheFlushDMA3DL->PostCspFenceCmd.uint         = SEND_INTERNAL_WRITEFENCE_E3K(FENCE_ROUTE_ID_CSP_FENCE,_RBT_3DFE, HWM_SYNC_KMD_SLOT);
    pCacheFlushDMA3DL->PostCspFenceValue            = SEND_INTERNAL_FENCE_VALUE_E3K(FENCE_VALUE_CACHE_DMA_POSTCSPFENCE);
    pCacheFlushDMA3DL->PostCspWaitCmd.uint          = SEND_INTERNAL_WAIT_E3K(HWM_SYNC_KMD_SLOT, WAIT_METHOD_EQUAL, FENCE_VALUE_CACHE_DMA_POSTCSPFENCE);
    pCacheFlushDMA3DL->PostCspWaitMainCmd.uint      = SEND_INTERNAL_WAIT_MAIN_E3K(HWM_SYNC_KMD_SLOT, WAIT_METHOD_EQUAL, FENCE_VALUE_CACHE_DMA_POSTCSPFENCE);

    //CSL
    pCacheFlushDMACSL->UAVFE_Flush.uint             = 0;
    pCacheFlushDMACSL->UAVFE_Invalidate.uint        = 0;
    pCacheFlushDMACSL->UAVBE_Flush.uint             = 0;
    pCacheFlushDMACSL->UAVBE_Invalidate.uint        = 0;
    pCacheFlushDMACSL->UAVCS_Flush.uint             = SEND_UCACHE_CSL_FLUSH_COMMAND_E3K();
    pCacheFlushDMACSL->UAVCS_Invalidate.uint        = SEND_UCACHE_CSL_INVALIDATE_COMMAND_E3K();

    pCacheFlushDMACSL->L2Flush.Block_Id             = L2_BLOCK;
    pCacheFlushDMACSL->L2Flush.Usage                = BLOCK_COMMAND_L2_USAGE_ALL;
    pCacheFlushDMACSL->L2Flush.Type                 = BLOCK_COMMAND_L2_TYPE_FLUSH_L2;
    pCacheFlushDMACSL->L2Flush.Major_Opcode         = CSP_OPCODE_Block_Command_L2;
    pCacheFlushDMACSL->L2Flush.Dwc                  = 0;

    pCacheFlushDMACSL->L2Invalidate.Block_Id        = L2_BLOCK;
    pCacheFlushDMACSL->L2Invalidate.Usage           = BLOCK_COMMAND_L2_USAGE_ALL;
    pCacheFlushDMACSL->L2Invalidate.Type            = BLOCK_COMMAND_L2_TYPE_INVALIDATE_L2;
    pCacheFlushDMACSL->L2Invalidate.Major_Opcode    = CSP_OPCODE_Block_Command_L2;
    pCacheFlushDMACSL->L2Invalidate.Dwc             = 0;

    pCacheFlushDMACSL->TUFEL1Invalidate.uint        = SEND_TUFE_L1CACHE_INVALIDATE_COMMAND_E3K();
    pCacheFlushDMACSL->TUBEL1Invalidate.uint        = 0;

    pCacheFlushDMACSL->EUFSL1Invalidate.uint        = SEND_FS_L1I_INVALIDATE_COMMAND_E3K();
    pCacheFlushDMACSL->EUPSL1Invalidate.uint        = 0;

    pCacheFlushDMACSL->PreUAVFEFenceCmd.uint        = SEND_INTERNAL_WRITEFENCE_E3K(FENCE_ROUTE_ID_UAVFE_FENCE,_RBT_3DFE, HWM_SYNC_KMD_SLOT);
    pCacheFlushDMACSL->PreUAVFEFenceValue           = SEND_INTERNAL_FENCE_VALUE_E3K(FENCE_VALUE_CACHE_DMA_UAVFEFENCE);
    pCacheFlushDMACSL->PreUAVFEWaitCmd.uint         = SEND_INTERNAL_WAIT_E3K(HWM_SYNC_KMD_SLOT, WAIT_METHOD_EQUAL, FENCE_VALUE_CACHE_DMA_UAVFEFENCE);
    pCacheFlushDMACSL->PreUAVFEWaitMainCmd.uint     = SEND_INTERNAL_WAIT_MAIN_E3K(HWM_SYNC_KMD_SLOT, WAIT_METHOD_EQUAL, FENCE_VALUE_CACHE_DMA_UAVFEFENCE);

    pCacheFlushDMACSL->PostUAVFEFenceCmd.uint       = SEND_INTERNAL_WRITEFENCE_E3K(FENCE_ROUTE_ID_UAVFE_FENCE,_RBT_3DFE, HWM_SYNC_KMD_SLOT);
    pCacheFlushDMACSL->PostUAVFEFenceValue          = SEND_INTERNAL_FENCE_VALUE_E3K(FENCE_VALUE_CACHE_DMA_UAVFEFENCE);
    pCacheFlushDMACSL->PostUAVFEWaitCmd.uint        = SEND_INTERNAL_WAIT_E3K(HWM_SYNC_KMD_SLOT, WAIT_METHOD_EQUAL, FENCE_VALUE_CACHE_DMA_UAVFEFENCE);
    pCacheFlushDMACSL->PostUAVFEWaitMainCmd.uint    = SEND_INTERNAL_WAIT_MAIN_E3K(HWM_SYNC_KMD_SLOT, WAIT_METHOD_EQUAL, FENCE_VALUE_CACHE_DMA_UAVFEFENCE);

    pCacheFlushDMACSL->L2FenceCmd.uint              = SEND_INTERNAL_WRITEFENCE_E3K(FENCE_ROUTE_ID_L2_FENCE,_RBT_3DBE, HWM_SYNC_KMD_SLOT);
    pCacheFlushDMACSL->L2FenceValue                 = FENCE_VALUE_CACHE_DMA_L2C_FENCE;

    pCacheFlushDMACSL->L2WaitCmd.uint               = SEND_INTERNAL_WAIT_E3K(HWM_SYNC_KMD_SLOT, WAIT_METHOD_EQUAL, FENCE_VALUE_CACHE_DMA_L2C_FENCE);
    pCacheFlushDMACSL->L2WaitMainCmd.uint           = SEND_INTERNAL_WAIT_MAIN_E3K(HWM_SYNC_KMD_SLOT, WAIT_METHOD_EQUAL, FENCE_VALUE_CACHE_DMA_L2C_FENCE);

 /*   pCacheFlushDMACSL->PreCspFenceCmd.uint            = SEND_INTERNAL_WRITEFENCE_E3K(FENCE_ROUTE_ID_CSP_FENCE,_RBT_3DFE, HWM_SYNC_KMD_SLOT);
    pCacheFlushDMACSL->PreCspFenceValue                = SEND_INTERNAL_FENCE_VALUE_E3K(FENCE_VALUE_CSPFENCE);
    pCacheFlushDMACSL->PreCspWaitCmd.uint              = SEND_INTERNAL_WAIT_E3K(HWM_SYNC_KMD_SLOT, WAIT_METHOD_EQUAL, FENCE_VALUE_CSPFENCE);
    pCacheFlushDMACSL->PreCspWaitMainCmd.uint          = SEND_INTERNAL_WAIT_MAIN_E3K(HWM_SYNC_KMD_SLOT, WAIT_METHOD_EQUAL, FENCE_VALUE_CSPFENCE);*/

    /*we need first flush the cache, ensure last write will flush out to memory*/
    reg_value                                        = SEND_FLAGBUFFER_FLUSH_COMMAND_E3K();
    pCacheFlushDMACSL->BLC_Flush                    = *(Cmd_Block_Command_Mxu*)&reg_value;
    pCacheFlushDMACSL->FlushAddr_L                  = 0;
    pCacheFlushDMACSL->FlushAddr_H                  = 0;
    pCacheFlushDMACSL->FlushMask_L                  = 0;
    pCacheFlushDMACSL->FlushMask_H                  = 0;

   /* pCacheFlushDMACSL->PostCspFenceCmd.uint            = SEND_INTERNAL_WRITEFENCE_E3K(FENCE_ROUTE_ID_CSP_FENCE,_RBT_3DFE, HWM_SYNC_KMD_SLOT);
    pCacheFlushDMACSL->PostCspFenceValue               = SEND_INTERNAL_FENCE_VALUE_E3K(FENCE_VALUE_CSPFENCE);
    pCacheFlushDMACSL->PostCspWaitCmd.uint             = SEND_INTERNAL_WAIT_E3K(HWM_SYNC_KMD_SLOT, WAIT_METHOD_EQUAL, FENCE_VALUE_CSPFENCE);
    pCacheFlushDMACSL->PostCspWaitMainCmd.uint         = SEND_INTERNAL_WAIT_MAIN_E3K(HWM_SYNC_KMD_SLOT, WAIT_METHOD_EQUAL, FENCE_VALUE_CSPFENCE);*/

    zx_memcpy(pRestoreDMA_vk, pRestoreDMA, sizeof(CONTEXT_RESTORE_DMA_E3K));

    zx_memcpy(pSaveDMA_vk, pSaveDMA, sizeof(CONTEXT_SAVE_DMA_E3K));
}


void enginei_init_gfx_submit_rb_cmd_e3k(engine_gfx_e3k_t *engine)
{
    engine_e3k_t         *common    = &engine->common;
    engine_share_e3k_t   *share     = common->share;
    adapter_t          *adapter     = common->sch_mgr->adapter;
    submit_rb_cmd_e3k_t   *rb_cmd_template     = &share->rb_cmd_template;

    //init rb_cmd_template rb template
    zx_memset(rb_cmd_template, 0, sizeof(submit_rb_cmd_e3k_t));

    rb_cmd_template->rb_cmd_part1.flush_cmd[0] = SEND_INTERNAL_WRITEFENCE_E3K(FENCE_ROUTE_ID_D_FENCE, _RBT_3DFE, HWM_SYNC_KMD_SLOT);
    rb_cmd_template->rb_cmd_part1.flush_cmd[1] = SEND_INTERNAL_FENCE_VALUE_E3K(FENCE_VALUE_RESET);
    rb_cmd_template->rb_cmd_part1.flush_cmd[2] = SEND_INTERNAL_WAIT_E3K(HWM_SYNC_KMD_SLOT, WAIT_METHOD_EQUAL, FENCE_VALUE_RESET);
    rb_cmd_template->rb_cmd_part1.flush_cmd[3] = SEND_INTERNAL_WAIT_MAIN_E3K(HWM_SYNC_KMD_SLOT, WAIT_METHOD_EQUAL, FENCE_VALUE_RESET);

    rb_cmd_template->rb_cmd_part1.flush_cmd[4] = SEND_INTERNAL_WRITEFENCE_E3K(FENCE_ROUTE_ID_D_FENCE, _RBT_3DFE, HWM_SYNC_KMD_SLOT);
    rb_cmd_template->rb_cmd_part1.flush_cmd[5] = SEND_INTERNAL_FENCE_VALUE_E3K(FENCE_VALUE_RANGESET);
    rb_cmd_template->rb_cmd_part1.flush_cmd[6] = SEND_INTERNAL_WAIT_E3K(HWM_SYNC_KMD_SLOT, WAIT_METHOD_EQUAL, FENCE_VALUE_RANGESET);
    rb_cmd_template->rb_cmd_part1.flush_cmd[7] = SEND_INTERNAL_WAIT_MAIN_E3K(HWM_SYNC_KMD_SLOT, WAIT_METHOD_EQUAL, FENCE_VALUE_RANGESET);

    rb_cmd_template->rb_cmd_part1.pwm_trigger_on.Major_Opcode      = CSP_OPCODE_Blk_Cmd_Csp_Indicator;
    rb_cmd_template->rb_cmd_part1.pwm_trigger_on.Block_Id          = CSP_GLOBAL_BLOCK;
    rb_cmd_template->rb_cmd_part1.pwm_trigger_on.Type              = BLOCK_COMMAND_CSP_TYPE_INDICATOR;
    rb_cmd_template->rb_cmd_part1.pwm_trigger_on.Info              =  BLK_CMD_CSP_INDICATOR_INFO_3D_MODE;
    rb_cmd_template->rb_cmd_part1.pwm_trigger_on.Dwc               = 1;

    rb_cmd_template->rb_cmd_part1.skip0.Major_Opcode                = CSP_OPCODE_Skip;
    rb_cmd_template->rb_cmd_part1.skip0.Dwc                         = 8;//9-1

    rb_cmd_template->rb_cmd_part1.restore.restore_dma.Major_Opcode         = CSP_OPCODE_Dma;
    rb_cmd_template->rb_cmd_part1.restore.restore_dma.Special_Dma_Type     = DMA_SPECIAL_DMA_TYPE_SAVE_REST_CMD;
    rb_cmd_template->rb_cmd_part1.restore.restore_dma.Mode                 = DMA_MODE_RESTORE;
    rb_cmd_template->rb_cmd_part1.restore.restore_dma.Dw_Num               = sizeof(CONTEXT_RESTORE_DMA_E3K) >> 2;
    rb_cmd_template->rb_cmd_part1.restore.restore_dma.Reset_Dma            = 1;

    rb_cmd_template->rb_cmd_part1.cmd_Set_Register.Major_Opcode            = CSP_OPCODE_Set_Register;
    rb_cmd_template->rb_cmd_part1.cmd_Set_Register.Block_Id                = CSP_GLOBAL_BLOCK;
    rb_cmd_template->rb_cmd_part1.cmd_Set_Register.Start_Offset            = Reg_Reg_Dvfs_Cfg_En_Offset;
    rb_cmd_template->rb_cmd_part1.cmd_Set_Register.Dwc                     = 1;
    rb_cmd_template->rb_cmd_part1.cmd_Set_Register.Addr_En                 = 0;
    rb_cmd_template->rb_cmd_part1.cmd_Set_Register.Mask_En                 = 1;
    rb_cmd_template->rb_cmd_part1.cmd_Set_Register.Dwc                     = 2;

    rb_cmd_template->rb_cmd_part1.reg_dvfs_cfg.reg.Dvfs_3d_Auto_En         = adapter->pm_caps.gfx_dvfs_auto;
    rb_cmd_template->rb_cmd_part1.reg_dvfs_cfg.reg.Dvfs_Vcp0_Auto_En       = adapter->pm_caps.vcp_dvfs_auto;
    rb_cmd_template->rb_cmd_part1.reg_dvfs_cfg.reg.Dvfs_Vcp1_Auto_En       = adapter->pm_caps.vcp_dvfs_auto;
    rb_cmd_template->rb_cmd_part1.reg_dvfs_cfg.reg.Dvfs_Vpp_Auto_En        = adapter->pm_caps.vpp_dvfs_auto;

    rb_cmd_template->rb_cmd_part3.cacheflush_cmd.Major_Opcode      = CSP_OPCODE_Dma;
    rb_cmd_template->rb_cmd_part3.cacheflush_cmd.Dw_Num            = sizeof(CACHEFLUSH_DMA_E3K) >> 2;
    rb_cmd_template->rb_cmd_part3.cacheflush_cmd.Mode              = DMA_MODE_FLUSH;
    rb_cmd_template->rb_cmd_part3.cacheflush_cmd.Reset_Dma         = 1;

    rb_cmd_template->rb_cmd_part3.save.save_dma.Major_Opcode            = CSP_OPCODE_Dma;
    rb_cmd_template->rb_cmd_part3.save.save_dma.Dw_Num                  = sizeof(CONTEXT_SAVE_DMA_E3K) >> 2;
    rb_cmd_template->rb_cmd_part3.save.save_dma.Mode                    = DMA_MODE_SAVE;
    rb_cmd_template->rb_cmd_part3.save.save_dma.Special_Dma_Type        = DMA_SPECIAL_DMA_TYPE_SAVE_REST_CMD;
    rb_cmd_template->rb_cmd_part3.save.save_dma.Reset_Dma               = 1;

    *(unsigned int*)(&rb_cmd_template->rb_cmd_part3.fence)         = SEND_EXTERNAL_FENCE_E3K(0);
    *(unsigned int*)(&rb_cmd_template->rb_cmd_part3.fence_1)       = SEND_EXTERNAL_FENCE_E3K(0);

    rb_cmd_template->rb_cmd_part3.pwm_trigger_off.Major_Opcode      = CSP_OPCODE_Blk_Cmd_Csp_Indicator;
    rb_cmd_template->rb_cmd_part3.pwm_trigger_off.Block_Id          = CSP_GLOBAL_BLOCK;
    rb_cmd_template->rb_cmd_part3.pwm_trigger_off.Type              = BLOCK_COMMAND_CSP_TYPE_INDICATOR;
    rb_cmd_template->rb_cmd_part3.pwm_trigger_off.Info              = BLK_CMD_CSP_INDICATOR_INFO_OFF_MODE;
    rb_cmd_template->rb_cmd_part3.pwm_trigger_off.Dwc               = 1;

}

int  engine_set_root_page_table_e3k(vidsch_mgr_t *sch_mgr, gpu_device_t *device, unsigned int vm_id, task_page_table_set_t *page_table_set_task)
{
    adapter_t          *adapter     = sch_mgr->adapter;
    int                engine_index = sch_mgr->engine_index;
    engine_e3k_t       *engine      = IS_VIDEO_ENGINE(adapter->chip_id, sch_mgr->engine_index) ? adapter->sch_mgr[0]->private_data : (engine_e3k_t *)sch_mgr->private_data;
    zx_vma_space_t     *vm          = device->vm;
    unsigned long long  root_gpu_va = vm->pt_info.root.addr;

    //Per Jane, 112 skips are enough. But seems work on QT, but on board
    //still easy to hang with 112 skips. Currently at 224 skips. Will refine this later
    static int skipCmds = 224;
    Reg_Pt_Inv_Addr     reg_pt_inv_addr = { 0 };
    Reg_Pt_Inv_Mask     reg_pt_inv_mask = { 0 };
    Reg_Pt_Inv_Trig     reg_pt_inv_trig = { 0 };
    Cmd_Blk_Cmd_Csp_Indicator       pwm_trigger_on = {0,};
    Cmd_Blk_Cmd_Csp_Indicator_Dword1 trigger_on_dw = {0,};
    Cmd_Blk_Cmd_Csp_Indicator       pwm_trigger_off = {0, };
    Cmd_Blk_Cmd_Csp_Indicator_Dword1 trigger_off_dw = {0,};

    unsigned int        *rb          = NULL;
    unsigned int        rb_size_uint = util_align(48 + skipCmds + POWER_SKIP_COMMAND_SIZE, 16);
    int i;

    unsigned int index = 0;

    //todo: the is_pa is the root_pt_addr whether be pa or va. for in the shared space, is va.
    unsigned int is_pa = 0;

    zx_assert(vm_id < 16, "vm id:%d is not correct\n", vm_id);
    enginei_mmio_invalidate_tlb_e3k(adapter);

    //zx_info("%s: with vm_id: %d, root addr:%llx\n", __func__, vm_id, root_gpu_va);
    reg_pt_inv_addr.reg.Proc_Id = vm_id;  //HW always get proc-id from this register. They will fix this next version.
    reg_pt_inv_mask.reg.Proc_Id = vm_id;

    // TODO
    reg_pt_inv_addr.reg.Address = 0;
    reg_pt_inv_mask.reg.Mask = 0;

    reg_pt_inv_trig.reg.Proc_Id = vm_id;
    reg_pt_inv_trig.reg.Target  = PT_INV_TRIG_TARGET_UTLB;

    pwm_trigger_on.Major_Opcode = CSP_OPCODE_Blk_Cmd_Csp_Indicator;
    pwm_trigger_on.Block_Id = CSP_GLOBAL_BLOCK;
    pwm_trigger_on.Type = BLOCK_COMMAND_CSP_TYPE_INDICATOR;
    pwm_trigger_on.Info = BLK_CMD_CSP_INDICATOR_INFO_3D_MODE;
    pwm_trigger_on.Dwc = 1;

    pwm_trigger_off.Major_Opcode = CSP_OPCODE_Blk_Cmd_Csp_Indicator;
    pwm_trigger_off.Block_Id = CSP_GLOBAL_BLOCK;
    pwm_trigger_off.Type = BLOCK_COMMAND_CSP_TYPE_INDICATOR;
    pwm_trigger_off.Info = BLK_CMD_CSP_INDICATOR_INFO_OFF_MODE;
    pwm_trigger_off.Dwc = 1;

    trigger_on_dw.Slice_Mask = adapter->hw_caps.chip_slice_mask;
    trigger_off_dw.Slice_Mask = adapter->hw_caps.chip_slice_mask;

    rb = enginei_get_ring_buffer_space_e3k(engine, rb_size_uint);

    zx_memset(rb, 0, rb_size_uint<<2);
    *rb++ = *(unsigned int*)&pwm_trigger_on;
    *rb++ = *(unsigned int*)&trigger_on_dw;

    //*rb = SEND_SKIP_E3K(8);
    rb += 8;

    *rb++ = SET_REGISTER_ADDR_E3K(MMU_BLOCK, Reg_Mmu_Ttbr_Offset + vm_id * 256 + (adapter->shared_gpu_va_size >>32), SET_REGISTER_ADDRESS_MODE_ADDRESS);
    //Skip set the shared space ttbr regs setting, since shared ttbr is setted by mmio, and the vm's root will not fill this regs.
    //Or we can copy shared space ttbr regs into the vm's root mem, and update all 256 ttbr every time here.
    root_gpu_va += (adapter->shared_gpu_va_size >>32)*GPU_PTE_SIZE;
    *rb++ = SET_REGISTER_ADDR_LOW_E3K((unsigned int)root_gpu_va);
    *rb++ = SET_REGISTER_ADDR_HIGH_AND_REGCNT_E3K((unsigned int)(root_gpu_va >> 32), (256 -(adapter->shared_gpu_va_size >>32)), is_pa);

    //*rb = SEND_SKIP_E3K(skipCmds);
    rb += skipCmds;

    *rb++ = SET_REGISTER_FD_E3K(MXU_BLOCK, Reg_Pt_Inv_Addr_Offset, 1);
    *rb++ = reg_pt_inv_addr.uint;
    *rb++ = SET_REGISTER_FD_E3K(MXU_BLOCK, Reg_Pt_Inv_Mask_Offset, 1);
    *rb++ = reg_pt_inv_mask.uint;
    *rb++ = SET_REGISTER_FD_E3K(MXU_BLOCK, Reg_Pt_Inv_Trig_Offset, 1);
    *rb++ = reg_pt_inv_trig.uint;

    *rb++ = SET_REGISTER_FD_E3K(MXU_BLOCK, Reg_Pt_Inv_Trig_Offset, 1);
    reg_pt_inv_trig.reg.Target = PT_INV_TRIG_TARGET_DTLB;
    *rb++ = reg_pt_inv_trig.uint;

    *rb++ = SEND_INTERNAL_WRITEFENCE_E3K(FENCE_ROUTE_ID_CSP_FENCE, _RBT_3DBE, HWM_SYNC_KMD_SLOT);
    *rb++ = SEND_INTERNAL_FENCE_VALUE_E3K(FENCE_VALUE_SET_MXU_REG_DFENCE);
    *rb++ = SEND_INTERNAL_WAIT_E3K(HWM_SYNC_KMD_SLOT, WAIT_METHOD_EQUAL, FENCE_VALUE_SET_MXU_REG_DFENCE);
    *rb++ = SEND_INTERNAL_WAIT_MAIN_E3K(HWM_SYNC_KMD_SLOT, WAIT_METHOD_EQUAL, FENCE_VALUE_SET_MXU_REG_DFENCE);
    *rb++ = SEND_INTERNAL_WRITEFENCE_E3K(FENCE_ROUTE_ID_CSP_FENCE, _RBT_3DBE, HWM_SYNC_KMD_SLOT);
    *rb++ = SEND_INTERNAL_FENCE_VALUE_E3K(FENCE_VALUE_RESET);
    *rb++ = SEND_INTERNAL_WAIT_E3K(HWM_SYNC_KMD_SLOT, WAIT_METHOD_EQUAL, FENCE_VALUE_RESET);
    *rb++ = SEND_INTERNAL_WAIT_MAIN_E3K(HWM_SYNC_KMD_SLOT, WAIT_METHOD_EQUAL, FENCE_VALUE_RESET);

    //unsigned long long FenceAddress;
    //FenceAddress = pPhyAdapter->FenceBufferGpuVa + FENCEOFFSET;
    *rb++ = SEND_EXTERNAL_FENCE_E3K(FENCE_IRQ_INTERRUPT_CPU);
    *rb++ = engine->fence_buffer_gpu_va & 0xFFFFFFFF;
    *rb++ = (engine->fence_buffer_gpu_va >> 32) & 0xFFFFFFFF;
    *rb++ = (unsigned int)((page_table_set_task->desc.fence_id) & 0xFFFFFFFF);
    *rb++ = (unsigned int)((page_table_set_task->desc.fence_id >> 32) & 0xFFFFFFFF);

    for (i = 0; i < POWER_SKIP_COMMAND_SIZE; i++)
    {
        *rb++  = 0;
    }

    *rb++ = *(unsigned int*)&pwm_trigger_off;
    *rb++ = *(unsigned int*)&trigger_off_dw;

    enginei_release_ring_buffer_space_e3k(engine, rb_size_uint);

    engine->last_send_fence_id = page_table_set_task->desc.fence_id;
    engine->last_process_id = page_table_set_task->desc.vm_id;

    return S_OK;
}

const unsigned int hwmTypeToRouteIdTbl[HWM_SYNC_MAX_TYPE] =
{
    FENCE_ROUTE_ID_Z_FENCE,        // HWM_SYNC_FS_TEX  (DZ-only draw)    0
    FENCE_ROUTE_ID_UAVFE_FENCE,    // HWM_SYNC_FS_TEX_UAVFE   1
    FENCE_ROUTE_ID_UAVFE_FENCE,    // HWM_SYNC_FS_UAV 2
    FENCE_ROUTE_ID_D_FENCE,        // HWM_SYNC_PS_TEX_D  (DZ-only draw)     3
    FENCE_ROUTE_ID_Z_FENCE,        // HWM_SYNC_PS_TEX_Z  (DZ-only draw)     4
    FENCE_ROUTE_ID_UAVBE_FENCE,    // HWM_SYNC_PS_TEX_UAVBE   5
    FENCE_ROUTE_ID_UAVBE_FENCE,    // HWM_SYNC_PS_UAV  6
    FENCE_ROUTE_ID_FS_STO_FENCE,   // HWM_SYNC_STO        7
    FENCE_ROUTE_ID_D_FENCE,        // HWM_SYNC_RT    8
    FENCE_ROUTE_ID_D_FENCE,        // HWM_SYNC_2DBLT_SRC   9
    FENCE_ROUTE_ID_D_FENCE,        // HWM_SYNC_2DBLT_DST   a
    FENCE_ROUTE_ID_Z_FENCE,        // HWM_SYNC_Z_R         b
    FENCE_ROUTE_ID_Z_FENCE,        // HWM_SYNC_Z_RW            c
    FENCE_ROUTE_ID_CSP_FENCE,      // HWM_SYNC_AUTO_CLEAR    d
    FENCE_ROUTE_ID_L2_FENCE,       // HWM_SYNC_INVALIDATE_L2_CACHE    e
    FENCE_ROUTE_ID_CSP_FENCE,      // HWM_SYNC_SETREGADDR    f  //don't send fence of this type, only for wait
};

unsigned int hwmRouteIdToRBTypeTbl[] =
{
    _RBT_3DFE,    // FENCE_ROUTE_ID_CSP_FENCE    //TBD
    _RBT_3DFE,    // FENCE_ROUTE_ID_Z_FENCE
    _RBT_3DBE,    // FENCE_ROUTE_ID_D_FENCE
    _RBT_3DFE,    // FENCE_ROUTE_ID_UAVFE_FENCE
    _RBT_3DBE,    // FENCE_ROUTE_ID_UAVBE_FENCE
    _RBT_3DFE,    // FENCE_ROUTE_ID_FS_STO_FENCE
    _RBT_3DBE,    // FENCE_ROUTE_ID_MCE_FENCE    //TBD
    _RBT_3DBE,    // FENCE_ROUTE_ID_L2_FENCE    //TBD
    _RBT_3DBE,    // FENCE_ROUTE_ID_VCP_FENCE    //TBD
    _RBT_3DBE,    // FENCE_ROUTE_ID_VPP_FENCE    //TBD
    _RBT_3DBE,    // FENCE_ROUTE_ID_ISP_FENCE    //TBD
};

const DWORD HwmTypeToSlotTbl[HWM_SYNC_MAX_TYPE] =
{
    HWM_SYNC_Z_SLOT,        // HWM_SYNC_FS_TEX  (DZ-only draw)
    HWM_SYNC_VS_SLOT,       // HWM_SYNC_FS_TEX_UAV
    HWM_SYNC_VS_SLOT,       // HWM_SYNC_FS_UAV
    HWM_SYNC_WBU_SLOT,      // HWM_SYNC_PS_TEX_D  (DZ-only draw)
    HWM_SYNC_Z_SLOT,        // HWM_SYNC_PS_TEX_Z  (DZ-only draw)
    HWM_SYNC_PS_SLOT,       // HWM_SYNC_PS_TEX_UAVBE
    HWM_SYNC_PS_SLOT,       // HWM_SYNC_PS_UAV
    HWM_SYNC_STO_SLOT,      // HWM_SYNC_STO
    HWM_SYNC_WBU_SLOT,      // HWM_SYNC_RT
    HWM_SYNC_WBU_SLOT,      // HWM_SYNC_2DBLT_SRC
    HWM_SYNC_WBU_SLOT,      // HWM_SYNC_2DBLT_DST
    HWM_SYNC_Z_SLOT,        // HWM_SYNC_Z_R
    HWM_SYNC_Z_SLOT,        // HWM_SYNC_Z_RW
    HWM_SYNC_MXU_SLOT,      // HWM_SYNC_AUTO_CLEAR
    HWM_SYNC_L2_SLOT,       // HWM_SYNC_INVALIDATE_L2_CACHE
    HWM_SYNC_MXU_SLOT,      // HWM_SYNC_SETREGADDR
};

static int send_internal_fence_wait_pair_e3k(unsigned int **ppCmdBuf, DWORD Usage)
{
    unsigned int Type = HwmUsageType(Usage);
    unsigned int Slot = HwmTypeToSlotTbl[Type];
    unsigned int *pCmdBuf = *ppCmdBuf;
    unsigned int FenceId;

    FenceId = 0x12345678;
    *pCmdBuf++ = SEND_INTERNAL_WRITEFENCE_E3K(hwmTypeToRouteIdTbl[Type], hwmRouteIdToRBTypeTbl[hwmTypeToRouteIdTbl[Type]], Slot);
    *pCmdBuf++ = SEND_INTERNAL_FENCE_VALUE_E3K(FenceId);
    *pCmdBuf++ = SEND_INTERNAL_WAIT_E3K(Slot, WAIT_METHOD_EQUAL, FenceId);
    *pCmdBuf++ = SEND_INTERNAL_WAIT_MAIN_E3K(Slot, WAIT_METHOD_EQUAL, FenceId);

    FenceId = 0x87654321;
    *pCmdBuf++ = SEND_INTERNAL_WRITEFENCE_E3K(hwmTypeToRouteIdTbl[Type], hwmRouteIdToRBTypeTbl[hwmTypeToRouteIdTbl[Type]], Slot);
    *pCmdBuf++ = SEND_INTERNAL_FENCE_VALUE_E3K(FenceId);
    *pCmdBuf++ = SEND_INTERNAL_WAIT_E3K(Slot, WAIT_METHOD_EQUAL, FenceId);
    *pCmdBuf++ = SEND_INTERNAL_WAIT_MAIN_E3K(Slot, WAIT_METHOD_EQUAL, FenceId);

    *ppCmdBuf = pCmdBuf;

    return 0;
}

int engine_invalidate_tlb_command_e3k(vidsch_mgr_t *sch_mgr, gpu_device_t *device, unsigned int vm_id, task_page_table_set_t *page_table_set_task)
{
    adapter_t          *adapter     = sch_mgr->adapter;
    int                engine_index = sch_mgr->engine_index;
    engine_e3k_t       *engine      = IS_VIDEO_ENGINE(adapter->chip_id, sch_mgr->engine_index) ? adapter->sch_mgr[0]->private_data : (engine_e3k_t *)sch_mgr->private_data;
    zx_vma_space_t     *vm          = device->vm;

    gart_dirty_t            utlb_dirty = vm->utlb_dirtys[engine_index];
    gart_dirty_t            dtlb_dirty = vm->dtlb_dirtys[engine_index];

    DWORD               DtlbShift    = 0x10; //one Dtlb Cachelline is 512bit, one cache line size maps to 16*4KB virtual address.
    DWORD               UtlbShift    = 0x1a; //one Utlb Cacheline is 512bit, one cache line size maps to 16*4MB virtual address.

    Reg_Pt_Inv_Addr     reg_pt_inv_addr = { 0 };
    Reg_Pt_Inv_Mask     reg_pt_inv_mask = { 0 };
    Reg_Pt_Inv_Trig     reg_pt_inv_trig = { 0 };
    Cmd_Blk_Cmd_Csp_Indicator       pwm_trigger_on = {0,};
    Cmd_Blk_Cmd_Csp_Indicator_Dword1 trigger_on_dw = {0,};
    Cmd_Blk_Cmd_Csp_Indicator       pwm_trigger_off = {0, };
    Cmd_Blk_Cmd_Csp_Indicator_Dword1 trigger_off_dw = {0,};

    unsigned int        *rb          = NULL;
    unsigned int        rb_size_uint = 64 + POWER_SKIP_COMMAND_SIZE;
    int i;

    enginei_mmio_invalidate_tlb_e3k(adapter);

    pwm_trigger_on.Major_Opcode = CSP_OPCODE_Blk_Cmd_Csp_Indicator;
    pwm_trigger_on.Block_Id = CSP_GLOBAL_BLOCK;
    pwm_trigger_on.Type = BLOCK_COMMAND_CSP_TYPE_INDICATOR;
    pwm_trigger_on.Info = BLK_CMD_CSP_INDICATOR_INFO_3D_MODE;
    pwm_trigger_on.Dwc = 1;

    pwm_trigger_off.Major_Opcode = CSP_OPCODE_Blk_Cmd_Csp_Indicator;
    pwm_trigger_off.Block_Id = CSP_GLOBAL_BLOCK;
    pwm_trigger_off.Type = BLOCK_COMMAND_CSP_TYPE_INDICATOR;
    pwm_trigger_off.Info = BLK_CMD_CSP_INDICATOR_INFO_OFF_MODE;
    pwm_trigger_off.Dwc = 1;

    trigger_on_dw.Slice_Mask = adapter->hw_caps.chip_slice_mask;
    trigger_off_dw.Slice_Mask = adapter->hw_caps.chip_slice_mask;

    rb = enginei_get_ring_buffer_space_e3k(engine, rb_size_uint);

    zx_memset(rb, 0, rb_size_uint<<2);
    *rb++ = *(unsigned int*)&pwm_trigger_on;
    *rb++ = *(unsigned int*)&trigger_on_dw;

    //*rb = SEND_SKIP_E3K(8);
    rb += 8;

    if (1)
    {
        // move follow command to update_ptes?

        //before flush/inv any cache, need wait full pipe idle.
        send_internal_fence_wait_pair_e3k(&rb, HWM_SYNC_D);

        //TODO: gambit
        *rb++ = SEND_DCACHE_FLUSH_COMMAND_E3K();
        *rb++ = SEND_DCACHE_INVALIDATE_COMMAND_E3K();
        //wait flush/inv done
        send_internal_fence_wait_pair_e3k(&rb, HWM_SYNC_FFC_D_FLUSH);
    }

    //1 Generate normal invalidate command, need refine. Here just invalidate
    //all 16 hw context cache since don't know what's the exact hw context the app context mapped to,
    //should patch the exact hw context id before submit such flush command
    reg_pt_inv_addr.reg.Proc_Id = vm_id;
    reg_pt_inv_mask.reg.Proc_Id = vm_id;
    reg_pt_inv_trig.reg.Proc_Id = vm_id;

    if (utlb_dirty.dirty)
    {
        reg_pt_inv_addr.reg.Address = (utlb_dirty.dirty_addr >> UtlbShift) << 14;
        reg_pt_inv_mask.reg.Mask = ((~utlb_dirty.dirty_mask) >> UtlbShift) << 14;
        reg_pt_inv_trig.reg.Target = PT_INV_TRIG_TARGET_UTLB;
        *rb++ = SET_REGISTER_FD_E3K(MXU_BLOCK, Reg_Pt_Inv_Addr_Offset, 1);
        *rb++ = reg_pt_inv_addr.uint;
        *rb++ = SET_REGISTER_FD_E3K(MXU_BLOCK, Reg_Pt_Inv_Mask_Offset, 1);
        *rb++ = reg_pt_inv_mask.uint;
        *rb++ = SET_REGISTER_FD_E3K(MXU_BLOCK, Reg_Pt_Inv_Trig_Offset, 1);
        *rb++ = reg_pt_inv_trig.uint;
    }

    if (dtlb_dirty.dirty)
    {
        reg_pt_inv_addr.reg.Address = (dtlb_dirty.dirty_addr >> DtlbShift) << 4;
        reg_pt_inv_mask.reg.Mask = ((~dtlb_dirty.dirty_mask) >> DtlbShift) << 4;
        reg_pt_inv_trig.reg.Target = PT_INV_TRIG_TARGET_DTLB;
        *rb++ = SET_REGISTER_FD_E3K(MXU_BLOCK, Reg_Pt_Inv_Addr_Offset, 1);
        *rb++ = reg_pt_inv_addr.uint;
        *rb++ = SET_REGISTER_FD_E3K(MXU_BLOCK, Reg_Pt_Inv_Mask_Offset, 1);
        *rb++ = reg_pt_inv_mask.uint;
        *rb++ = SET_REGISTER_FD_E3K(MXU_BLOCK, Reg_Pt_Inv_Trig_Offset, 1);
        *rb++ = reg_pt_inv_trig.uint;
    }

    *rb++ = SEND_INTERNAL_WRITEFENCE_E3K(FENCE_ROUTE_ID_CSP_FENCE, _RBT_3DBE, HWM_SYNC_KMD_SLOT);
    *rb++ = SEND_INTERNAL_FENCE_VALUE_E3K(FENCE_VALUE_SET_MXU_REG_DFENCE);
    *rb++ = SEND_INTERNAL_WAIT_E3K(HWM_SYNC_KMD_SLOT, WAIT_METHOD_EQUAL, FENCE_VALUE_SET_MXU_REG_DFENCE);
    *rb++ = SEND_INTERNAL_WAIT_MAIN_E3K(HWM_SYNC_KMD_SLOT, WAIT_METHOD_EQUAL, FENCE_VALUE_SET_MXU_REG_DFENCE);
    *rb++ = SEND_INTERNAL_WRITEFENCE_E3K(FENCE_ROUTE_ID_CSP_FENCE, _RBT_3DBE, HWM_SYNC_KMD_SLOT);
    *rb++ = SEND_INTERNAL_FENCE_VALUE_E3K(FENCE_VALUE_RESET);
    *rb++ = SEND_INTERNAL_WAIT_E3K(HWM_SYNC_KMD_SLOT, WAIT_METHOD_EQUAL, FENCE_VALUE_RESET);
    *rb++ = SEND_INTERNAL_WAIT_MAIN_E3K(HWM_SYNC_KMD_SLOT, WAIT_METHOD_EQUAL, FENCE_VALUE_RESET);

    //unsigned long long FenceAddress;
    //FenceAddress = pPhyAdapter->FenceBufferGpuVa + FENCEOFFSET;
    *rb++ = SEND_EXTERNAL_FENCE_E3K(FENCE_IRQ_INTERRUPT_CPU);
    *rb++ = engine->fence_buffer_gpu_va & 0xFFFFFFFF;
    *rb++ = (engine->fence_buffer_gpu_va >> 32) & 0xFFFFFFFF;
    *rb++ = (unsigned int)((page_table_set_task->desc.fence_id) & 0xFFFFFFFF);
    *rb++ = (unsigned int)((page_table_set_task->desc.fence_id >> 32) & 0xFFFFFFFF);

    for (i = 0; i < POWER_SKIP_COMMAND_SIZE; i++)
    {
        *rb++  = 0;
    }

    *rb++ = *(unsigned int*)&pwm_trigger_off;
    *rb++ = *(unsigned int*)&trigger_off_dw;

    enginei_release_ring_buffer_space_e3k(engine, rb_size_uint);

    engine->last_send_fence_id = page_table_set_task->desc.fence_id;
    engine->last_process_id = page_table_set_task->desc.vm_id;

    return S_OK;
}

static int enginei_is_idle_e3k(void *argu)
{
    vidsch_mgr_t *sch_mgr = argu;
    int          idle     = TRUE;

#ifdef ZX_VMI_MODE
    // force update fence, each time
    vidsch_notify_interrupt(sch_mgr->adapter, 0);
#endif

    if (sch_mgr->returned_fence_id < sch_mgr->last_send_fence_id)
    {
        idle = FALSE;
    }
    return idle;
}
