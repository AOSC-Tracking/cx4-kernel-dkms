#include "zx_adapter.h"
#include "global.h"
#include "vidmm.h"
#include "vidsch.h"
#include "vidschi.h"
#include "vidsch_submit.h"
#include "e3k/vidsch/vidsch_engine_e3k.h"
#include "e3k/include/mm_e3k.h"
#include "vidmm/vidmmi.h"

#define MEM_FILL_PATTERN 0x74369216
#define VALIDATE_MEM_START_ADDR 0x100000000ULL //4G start
//#define VALIDATE_MEM_START_ADDR 0x140000000ULL //for 256M local mem, the right addr start over 4G: 5G
#define VALIDATE_MEM_SIZE 0x1000 //4k size
#define VALIDATE_MEM_PIXEL_SIZE 1024
#define TOTAL_MEM_SIZE  0x100000000ULL //4G start

unsigned long long vidsch_get_pte_value(adapter_t *adapter, int snoop, unsigned long long phy_addr){

    if (adapter->hw_caps.nonsnoop_patch_enable && !snoop)
    {
        phy_addr >>= 12;
        if(phy_addr > (adapter->low_top_address >> 12))
            phy_addr -= (0x100000 - (adapter->low_top_address >> 12));
        phy_addr <<= 12;
    }

    //zx_info("%s phy_addr=0x%llx\n", __func__, phy_addr);

    return phy_addr;
}

void vidsch_test_memory(struct os_printer *p, void *adpt, void *para)
{
    zx_test_mem_para_t *test_mem_para = para;
    adapter_t *adapter = adpt;
    unsigned long long phys_addr = test_mem_para->phys_addr;
    int snoop = test_mem_para->snoop;
    int j = 0;
    int loop = 0;
    unsigned int *mem=NULL, *tmpMem=NULL;
    int rightCnt=0, wrongCnt=0;
    int total_rightCnt=0, total_wrongCnt=0;
    unsigned int last_read_value = 0;
    task_dma_t task_dma = {0};
    unsigned long long fence_id = 0;
    vidmm_mgr_t *mm_mgr = adapter->mm_mgr;
    int pte_offset = 0;
    unsigned long long total_mem_size = VALIDATE_MEM_SIZE;
    unsigned long long phys_addr0 = 0;

    int ret;
    unsigned int          unsnoop_seg_id = snoop ? 3 : 2;
    vidmm_segment_memory_t   *reserved_memory = NULL;
    vidmm_segment_memory_t   *reserved_memory_dst = NULL;
    vidsch_allocate_paging_task_t           allocate_paging_task= {0,};
    task_paging_t                           *paging_task        = NULL;
    vidmm_private_build_paging_buffer_arg_t build_paging_buffer = {0};

    unsigned long long gpu_va;
    unsigned long long pte_start;
    unsigned int *pte_base;
    PTE_L3_t pte_level3 = {0,};

    reserved_memory = vidmm_create_segment_memory(adapter, unsnoop_seg_id, VALIDATE_MEM_SIZE, 1, 1);
    if (reserved_memory == NULL)
    {
        zx_printf(p, "create reserved memory fail!\n");
        return;
    }

    if (test_mem_para->read)
    {
        reserved_memory_dst = vidmm_create_segment_memory(adapter, 1, VALIDATE_MEM_SIZE, 1, 1);
        if(reserved_memory_dst == NULL)
        {
            zx_printf(p, "create dst reserved memory fail!\n");
            return;
        }
    }

    ret = vidmm_map_segment_memory(adapter, reserved_memory);
    if(ret != S_OK)
    {
        goto map_failed;
    }
    vidmm_lock_segment_memory(adapter, reserved_memory);

    if (reserved_memory_dst)
    {
        ret = vidmm_map_segment_memory(adapter, reserved_memory_dst);
        if(ret != S_OK)
        {
            goto map_failed;
        }
        vidmm_lock_segment_memory(adapter, reserved_memory_dst);
    }

    if (!phys_addr)
    {
        phys_addr = VALIDATE_MEM_START_ADDR;
        total_mem_size = TOTAL_MEM_SIZE;
    }
    phys_addr0 = phys_addr;

    /* debug use
    if (test_mem_para->read)
    {
        mem = tmpMem = (unsigned int*)reserved_memory_dst->krnl_cpu_vma->virt_addr;
        for(j=0; j<VALIDATE_MEM_PIXEL_SIZE; j++){
            *tmpMem = 0xeeff0011;
            //zx_info("###cpu write value:0x%x j=%d\n", *tmpMem, j);
            tmpMem++;
        }
    }
    else*/
    {
        mem = tmpMem = (unsigned int*)zx_ioremap(phys_addr, VALIDATE_MEM_SIZE);
        for(j=0; j<VALIDATE_MEM_PIXEL_SIZE; j++){
            *tmpMem = 0xdeadbeef;
            //zx_info("###cpu write value:0x%x j=%d\n", *tmpMem, j);
            tmpMem++;
        }
    }

    for(loop = 0; loop < total_mem_size/VALIDATE_MEM_SIZE; loop++)
    {
        allocate_paging_task.dma_size = 16*4*1024;
        allocate_paging_task.allocation_num = 1;
        paging_task = vidsch_allocate_paging_task(adapter->reserved_context, &allocate_paging_task);
        paging_task->flag.paging_fill              = 1;
        paging_task->paging_allocation_list[0].allocation = NULL;
        paging_task->paging_allocation_num         = 1;

        if (test_mem_para->read)
        {
            build_paging_buffer.operation              = BUILDING_PAGING_OPERATION_TRANSFER;
            build_paging_buffer.dma_buffer             = paging_task->dma->cpu_virtual_address;
            build_paging_buffer.dma_size               = paging_task->dma->size;
            build_paging_buffer.multi_pass_offset      = 0;
            build_paging_buffer.transfer.transfer_offset    = 0;
            build_paging_buffer.transfer.transfer_size = 1024 * 4;
            build_paging_buffer.transfer.src.phy_addr  = reserved_memory->gpu_va;
            build_paging_buffer.transfer.dst.phy_addr  = reserved_memory_dst->gpu_va;
            build_paging_buffer.allocation             = NULL;
        }
        else
        {
            build_paging_buffer.allocation             = NULL;
            build_paging_buffer.operation              = BUILDING_PAGING_OPERATION_FILL;
            build_paging_buffer.multi_pass_offset      = 0;
            build_paging_buffer.fill.fill_pattern      = MEM_FILL_PATTERN;
            build_paging_buffer.dma_buffer             = paging_task->dma->cpu_virtual_address;
            build_paging_buffer.dma_size               = paging_task->dma->size;
            build_paging_buffer.fill.fill_size         = 1024 * 4;
            build_paging_buffer.fill.gpu_virt_addr     = reserved_memory->gpu_va;
        }
        ret = mm_mgr->chip_func->build_paging_buffer(adapter, &build_paging_buffer);

        {
            pte_level3.pte_4k.Valid   = 1;
            pte_level3.pte_4k.Segment = 0;
            pte_level3.pte_4k.NS      = 1;
            pte_level3.pte_4k.Snoop   = snoop;

            gpu_va     = reserved_memory->gpu_va;
            pte_start  = gpu_va / GPU_PAGE_SIZE;
            pte_base = mm_mgr->vm_info->share_level3_cpu_va_offset;

            pte_level3.pte_4k.Addr = vidsch_get_pte_value(adapter, snoop, phys_addr) >> 12; //snoop param need to be set to snoop, here only for non-snoop patch

            //zx_info("before map gart: %llx, re-set pte: %llx", *(fill.pte_base + fill.pte_start), fill.pte_level3.uint);
            *(pte_base + pte_start) = pte_level3.uint;

        }

        /* invalidate the all gart table */
        zx_write32(adapter->mmio + 0x49000 + 0x6*4 , 0);
        zx_write32(adapter->mmio + 0x49000 + 0x7*4,  0);
        zx_write32(adapter->mmio + 0x49000 + 0x8*4, (0x2<<5));

        paging_task->dma->command_length = paging_task->dma->size - build_paging_buffer.dma_size;

#if 0
        zx_printf(p, "^^^ fill_allocation command_length=%d, size1=0x%llx size2=0x%x phys_addr=0x%llx\n",
                paging_task->dma->command_length, paging_task->dma->size, build_paging_buffer.dma_size, phys_addr);
#endif
        vidsch_submit_paging_task(adapter, paging_task);

        //force wait fence back.
        vidsch_wait_fence_back(adapter, adapter->paging_engine_index, paging_task->desc.fence_id, TRUE);

        vidsch_task_dec_reference(&paging_task->desc);

        if (test_mem_para->read)
        {
            mem = tmpMem = (unsigned int*)reserved_memory_dst->krnl_cpu_vma->virt_addr;
            zx_printf(p, "@@@gpu read expect value:0xdeadbeef\n");
            for(j=0; j<VALIDATE_MEM_PIXEL_SIZE; j++){
                if(*tmpMem == 0xdeadbeef){
                    if (last_read_value != *tmpMem)
                        zx_printf(p, "gpu read right! value:0x%x at addr 0x%llx rightCnt:%d j:%d\n", *tmpMem, phys_addr + j*4, rightCnt, j);
                    rightCnt++;
                }else{
                    if (last_read_value != *tmpMem)
                        zx_printf(p, "gpu read wrong! value:0x%x at addr 0x%llx wrongCnt:%d j:%d\n", *tmpMem, phys_addr + j*4, wrongCnt, j);
                    wrongCnt++;
                }
                last_read_value = *tmpMem;
                tmpMem++;
            }
        }
        else
        {
            mem = tmpMem = (unsigned int*)zx_ioremap(phys_addr, VALIDATE_MEM_SIZE);
            zx_printf(p, "@@@gpu write expect value:0x%x\n", MEM_FILL_PATTERN);
            for(j=0; j<VALIDATE_MEM_PIXEL_SIZE; j++){
                if(*tmpMem == MEM_FILL_PATTERN){
                    if (last_read_value != *tmpMem)
                        zx_printf(p, "gpu write right! value:0x%x at addr 0x%llx rightCnt:%d j:%d\n", *tmpMem, phys_addr + j*4, rightCnt, j);
                    rightCnt++;
                }else{
                    if (last_read_value != *tmpMem)
                        zx_printf(p, "gpu write wrong! value:0x%x at addr 0x%llx wrongCnt:%d j:%d\n", *tmpMem, phys_addr + j*4, wrongCnt, j);
                    wrongCnt++;
                }
                last_read_value = *tmpMem;
                tmpMem++;
            }
        }
        total_rightCnt += rightCnt;
        total_wrongCnt += wrongCnt;
        phys_addr += VALIDATE_MEM_SIZE;
        wrongCnt = 0;
        rightCnt = 0;
    }

    zx_printf(p,"^^^test from 0x%llx to 0x%llx, total int cnt:%d, validate total right cnt:%d, total wrong cnt:%d\n",
        phys_addr0, phys_addr0 + loop * VALIDATE_MEM_SIZE, loop * VALIDATE_MEM_PIXEL_SIZE, total_rightCnt, total_wrongCnt);

map_failed:
    if(reserved_memory->gpu_vm)
    {
        vidmm_unmap_segment_memory(reserved_memory);
    }
    if(reserved_memory_dst && reserved_memory_dst->gpu_vm)
    {
        vidmm_unmap_segment_memory(reserved_memory_dst);
    }

    if (reserved_memory)
    {
        vidmm_destroy_segment_memory(adapter, reserved_memory);
    }
    if (reserved_memory_dst)
    {
        vidmm_destroy_segment_memory(adapter, reserved_memory_dst);
    }
    if(ret != S_OK)
    {
        zx_printf(p, "map test segment memory failed, with err:0x%x", ret);
    }
}
