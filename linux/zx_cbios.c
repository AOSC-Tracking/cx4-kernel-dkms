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

#include "zx_disp.h"
#include "zx_vip.h"
#include "zx_wb.h"
#include "zx_cbios.h"
#include "zx_drmfb.h"
#include "zx_fbdev.h"
#include "zxgfx_trace.h"

void disp_delay_micro_seconds(unsigned int usecs)
{
    if(usecs <= 20)
    {
        zx_udelay(usecs);
    }
    else if(usecs < 20000)
    {
        zx_usleep_range(usecs, usecs + (usecs >> 3));
    }
    else
    {
        zx_msleep(usecs/1000 + 1);
    }
}

unsigned char disp_read_uchar(void *data, unsigned int port)
{
    disp_info_t *disp_info = data;
    adapter_info_t* adapter = disp_info->adp_info;

    return zx_read8(adapter->mmio + port);
}

unsigned short disp_read_ushort(void *data, unsigned int port)
{
    disp_info_t *disp_info = data;
    adapter_info_t *adapter = disp_info->adp_info;

    return zx_read16(adapter->mmio + port);
}

unsigned int disp_read_ulong(void *data, unsigned int port)
{
    disp_info_t *disp_info = data;
    adapter_info_t *adapter = disp_info->adp_info;

    return zx_read32(adapter->mmio + port);
}

void disp_write_uchar(void *data, unsigned int port, unsigned char value)
{
    disp_info_t *disp_info = data;
    adapter_info_t *adapter = disp_info->adp_info;

    zx_write8(adapter->mmio + port, value);
}

void disp_write_ushort(void *data, unsigned int port, unsigned short value)
{
    disp_info_t *disp_info = data;
    adapter_info_t *adapter = disp_info->adp_info;

    zx_write16(adapter->mmio + port, value);
}

void disp_write_ulong(void *data, unsigned int port, unsigned int value)
{
    disp_info_t *disp_info = data;
    adapter_info_t *adapter = disp_info->adp_info;

    zx_write32(adapter->mmio + port, value);
}

void* disp_alloc_memory(unsigned int size)
{
    return zx_calloc(size);
}

void disp_free_pool(void *pool)
{
    zx_free(pool);
}

void *disp_create_lock(int lock_type)
{
    void *os_lock = NULL;

    switch (lock_type)
    {
    case CBIOS_OS_SPIN_LOCK:
    {
        os_lock =  (void *)zx_create_spinlock();
    }
    break;

    case CBIOS_OS_MUTEX_LOCK:
    {
        os_lock = (void *)zx_create_mutex();
    }
    break;

    default:
    {
        zx_error("invalid lock type.\n");
    }
    break;
    }

    return os_lock;
}

void disp_destroy_lock(void *os_lock, int lock_type)
{
    if (!os_lock)
    {
        return;
    }

    switch (lock_type)
    {
    case CBIOS_OS_SPIN_LOCK:
    {
        struct os_spinlock *spin_lock = (struct os_spinlock *)os_lock;
        zx_destroy_spinlock(spin_lock);
    }
    break;

    case CBIOS_OS_MUTEX_LOCK:
    {
        struct os_mutex *mutex_lock = (struct os_mutex *)os_lock;
        zx_destroy_mutex(mutex_lock);
    }
    break;

    default:
    {
        zx_error("invalid lock type.\n");
    }
    break;
    }
}

unsigned long long disp_acquire_lock(void *os_lock, int lock_type)
{
    unsigned long long irq_flags = 0;

    if (!os_lock)
    {
        zx_assert(0, "lock is NULL\n");
        return 0;
    }

    switch (lock_type)
    {
    case CBIOS_OS_SPIN_LOCK:
    {
        struct os_spinlock *spin_lock = (struct os_spinlock *)os_lock;

        irq_flags = zx_spin_lock_irqsave(spin_lock);
    }
    break;

    case CBIOS_OS_MUTEX_LOCK:
    {
        struct os_mutex *mutex_lock = (struct os_mutex *)os_lock;

        zx_mutex_lock(mutex_lock);
    }
    break;
    default:
    {
        zx_error("invalid lock type.\n");
    }
    break;
    }

    return irq_flags;
}

void disp_release_lock(void *os_lock, int lock_type, unsigned long long flags)
{
    if (!os_lock)
    {
        zx_assert(0, "lock is NULL\n");
        return;
    }

    switch (lock_type)
    {
    case CBIOS_OS_SPIN_LOCK:
    {
        struct os_spinlock *spin_lock = (struct os_spinlock *)os_lock;

        zx_spin_unlock_irqrestore(spin_lock, flags);

    }
    break;

    case CBIOS_OS_MUTEX_LOCK:
    {
        struct os_mutex *mutex_lock = (struct os_mutex *)os_lock;

        zx_mutex_unlock(mutex_lock);
    }
    break;
    default:
    {
        zx_error("invalid lock type.\n");
    }
    break;
    }
}

void disp_write_log_file(unsigned int dbgleverl, unsigned char * dbgmsg, void * buffer, unsigned int size)
{
    //util_dump_memory_to_file(buffer, size, dbgmsg, CBIOS_LOG_FILE);
}

unsigned int disp_get_platform_config(void *data, const char* config_name, int *buffer, int length)
{
    disp_info_t *disp_info = data;
    zx_card_t*  zx_card = disp_info->zx_card;
    unsigned int ret = FALSE;

    if (!zx_get_platform_config(zx_card->pdev, config_name, buffer, length))
    {
        ret = TRUE;
    }
    else
    {
        ret = FALSE;
    }

    return ret;
}

void disp_query_sys_time(unsigned long long *u64_time)
{
    unsigned long sec = 0, usec = 0;

    if(u64_time == NULL)
    {
        return ;
    }

    zx_getsecs(&sec, &usec);

    *u64_time = (unsigned long long)sec * 1000000 + usec;
}

int  disp_get_output_num(int  outputs)
{
    int  num = 0;
    while(outputs)
    {
        if(outputs & 1)
        {
            num++;
        }
        outputs >>= 1;
    }
    return  num;
}

int  disp_biosmonitor_to_output(int bios_monitor)
{
    int  output = 0;
    if(bios_monitor & CBIOS_MONITOR_TYPE_CRT)
    {
        output |= UT_OUTPUT_TYPE_CRT;
    }
    if(bios_monitor & CBIOS_MONITOR_TYPE_TV)
    {
        output |= UT_OUTPUT_TYPE_TV;
    }
    if(bios_monitor & CBIOS_MONITOR_TYPE_HDTV)
    {
        output |= UT_OUTPUT_TYPE_HDTV;
    }
    if(bios_monitor & CBIOS_MONITOR_TYPE_PANEL)
    {
        output |= UT_OUTPUT_TYPE_PANEL;
    }
    if(bios_monitor & CBIOS_MONITOR_TYPE_DVI)
    {
        output |= UT_OUTPUT_TYPE_DVI;
    }
    if(bios_monitor & CBIOS_MONITOR_TYPE_HDMI)
    {
        output |= UT_OUTPUT_TYPE_HDMI;
    }
    if(bios_monitor & CBIOS_MONITOR_TYPE_DP)
    {
        output |= UT_OUTPUT_TYPE_DP;
    }
    if(bios_monitor & CBIOS_MONITOR_TYPE_MHL)
    {
        output |= UT_OUTPUT_TYPE_MHL;
    }

    return output;
}

int  disp_output_to_biosmonitor(int output)
{
    int  bios_monitor = 0;
    if(output & UT_OUTPUT_TYPE_CRT)
    {
        bios_monitor |= CBIOS_MONITOR_TYPE_CRT;
    }
    if(output & UT_OUTPUT_TYPE_TV)
    {
        bios_monitor |= CBIOS_MONITOR_TYPE_TV;
    }
    if(output & UT_OUTPUT_TYPE_HDTV)
    {
        bios_monitor |= CBIOS_MONITOR_TYPE_HDTV;
    }
    if(output & UT_OUTPUT_TYPE_PANEL)
    {
        bios_monitor |= CBIOS_MONITOR_TYPE_PANEL;
    }
    if(output & UT_OUTPUT_TYPE_DVI)
    {
        bios_monitor |= CBIOS_MONITOR_TYPE_DVI;
    }
    if(output & UT_OUTPUT_TYPE_HDMI)
    {
        bios_monitor |= CBIOS_MONITOR_TYPE_HDMI;
    }
    if(output & UT_OUTPUT_TYPE_DP)
    {
        bios_monitor |= CBIOS_MONITOR_TYPE_DP;
    }
    if(output & UT_OUTPUT_TYPE_MHL)
    {
        bios_monitor |= CBIOS_MONITOR_TYPE_MHL;
    }

    return bios_monitor;
}

CHIPID_HW disp_chipid_to_cbios_chipid(unsigned int chip_id)
{
     CHIPID_HW cb_chip_id = 0;

     switch(chip_id)
     {
     case CHIP_ELT3K:
        cb_chip_id = CHIPID_E3K;
        break;
     case CHIP_CHX004:
        cb_chip_id = CHIPID_CHX004;
        break;
     case CHIP_CNE001:
        cb_chip_id = CHIPID_CNE001;
        break;
     default:
         zx_assert(0, "Invalid chip id.");
         break;
     }

     return cb_chip_id;
}

int disp_get_shadow_rom_image(disp_info_t *disp_info)
{
    void *shadow_rom_image = NULL;
    void *src_image = NULL;
    unsigned char cr_a0_c, sr_1f;
    unsigned int mm850c = 0, vid_boundary = 0, bound_reg = 0;
    adapter_info_t*  adapter_info = disp_info->adp_info;
    zx_map_argu_t    map  = {0};
    zx_cpu_vm_area_t *vma = NULL;

    if((adapter_info->run_on_qt))
    {
        return 0;
    }

    vid_boundary = ((adapter_info->fb_total_size >> 24) -1 ) & 0xFF;
    bound_reg = zx_read32(adapter_info->mmio + 0x490a0);
    zx_write32(adapter_info->mmio + 0x490a0, (bound_reg & 0xFFFFFF00) | vid_boundary);

    shadow_rom_image = zx_calloc(ZX_SHADOW_VBIOS_SIZE);
    if(!shadow_rom_image)
    {
        zx_error("malloc shadow rom memory failed\n");
        goto fail;
    }

    map.flags.cache_type = ZX_MEM_UNCACHED;
    map.flags.mem_type   = ZX_SYSTEM_IO;
    map.node_num         = 1;
    map.phys_addr        = adapter_info->fb_bus_addr + adapter_info->fb_total_size - ZX_SHADOW_VBIOS_SIZE;
    map.size             = ZX_SHADOW_VBIOS_SIZE;

    vma = zx_map_io_memory(&map);

    src_image = vma->virt_addr;
    if(!src_image)
    {
        zx_error("map shadow vbios failed\n");
        goto fail;
    }

    /* enable eclk */
    sr_1f = zx_read8(adapter_info->mmio + 0x861F);
    zx_write8(adapter_info->mmio + 0x861F, sr_1f & 0xfe);

    /* enable linear address */
    cr_a0_c = zx_read8(adapter_info->mmio + 0x8AA0);
    zx_write8(adapter_info->mmio + 0x8AA0, cr_a0_c | 0x10);

    mm850c = zx_read32(adapter_info->mmio + 0x850c);
    if(!(mm850c & 0x2))
    {
        zx_write8(adapter_info->mmio + 0x850c, mm850c | 0x2);
    }

    zx_memcpy(shadow_rom_image, src_image, ZX_SHADOW_VBIOS_SIZE);

    zx_unmap_io_memory(vma);

    /* restore cr_a0_c */
    zx_write8(adapter_info->mmio + 0x8AA0, cr_a0_c);
    zx_write32(adapter_info->mmio + 0x850c, mm850c);

    if(*(unsigned short*)shadow_rom_image != 0xAA55)
    {
        zx_error("Invalid shadow rom_image \n");
        goto fail;
    }

    disp_info->rom_image = shadow_rom_image;

    return  ZX_SHADOW_VBIOS_SIZE;

fail:
    if(shadow_rom_image)
    {
        zx_free(shadow_rom_image);
        shadow_rom_image = NULL;
    }

    return  0;
}


int disp_get_rom_bar_image(disp_info_t *disp_info)
{
    zx_card_t        *zx_card = disp_info->zx_card;
    void             *rom_bar_image = NULL;
    void             *src_image = NULL;

    rom_bar_image = zx_calloc(ZX_VBIOS_ROM_SIZE);
    if (!rom_bar_image)
    {
        zx_error("malloc rom image memory failed!\n");
        disp_info->rom_image = NULL;
        return 0;
    }

    src_image = zx_pci_map_rom(zx_card->pdev);

    if(src_image)
    {
        if(*(unsigned short*)src_image == 0xaa55)
        {
            zx_memcpy(rom_bar_image, src_image, ZX_VBIOS_ROM_SIZE);
        }
        else
        {
            zx_info("invalid rom head(0x%x)=0x%x.\n", src_image,*(unsigned short*)src_image);
        }
        zx_pci_unmap_rom(zx_card->pdev, src_image);
    }

    disp_info->rom_image = rom_bar_image;

    return ZX_VBIOS_ROM_SIZE;
}

int disp_wait_for_vblank(disp_info_t* disp_info, int pipe, int timeout)
{
    unsigned long timeout_j = jiffies + msecs_to_jiffies(timeout) + 1;
    unsigned int ori_vblcnt = 0, curr_vblcnt = 0;
    int ret = 0;
    zx_get_counter_t   get_cnt = {0};

    get_cnt.crtc_index = pipe;
    get_cnt.vblk = &ori_vblcnt;

    disp_cbios_get_counter(disp_info, &get_cnt);

    curr_vblcnt = ori_vblcnt;
    get_cnt.vblk = &curr_vblcnt;

    while(curr_vblcnt == ori_vblcnt)
    {
        if(time_after(jiffies, timeout_j))
        {
            ret = -ETIMEDOUT;
            break;
        }
        if(drm_can_sleep())
        {
            zx_msleep(1);
        }
        else
        {
            zx_udelay(1000);
        }
        disp_cbios_get_counter(disp_info, &get_cnt);
    }

    return  ret;
}

static int is_bga_patch_cpu(disp_info_t *disp_info)
{
    adapter_info_t*  adapter_info = disp_info->adp_info;
    unsigned int lo, hi;
    int ret = 0;

    if (adapter_info->chip_id == CHIP_CNE001)
    {
        rdmsr(0x112, lo, hi);
        zx_info("read msr 0x112 hi = 0x%x, lo = 0x%x.\n", hi, lo);
        if ((hi == 0) && (lo == 0x25))
        {
            ret = 1;
        }
    }

    return ret;
}

/*CBIOS Initialization sequence:
 * 1) set Call back function
 * 2) Set Mmio Endian Mode
 * 3) CbiosInit
 * 4) CbiosInitHW
 * Before 2, we can't let CBIOS access MMIO as we don't set MMIO mode as we required
 * */
int disp_init_cbios(disp_info_t *disp_info)
{
    CBIOS_PARAM_INIT                 CBParamInit = {0};
    CBIOS_CALLBACK_FUNCTIONS fnCallBack = {0};
    CBIOS_CHIP_ID CBChipId;
    adapter_info_t*  adapter_info = disp_info->adp_info;
    void *pcbe = NULL;
    int  rom_lenth = 0;
    unsigned int CBiosStatus;
    unsigned int CBiosExtensionSize;

    fnCallBack.Size = sizeof(CBIOS_CALLBACK_FUNCTIONS);

    fnCallBack.pFnDelayMicroSeconds = disp_delay_micro_seconds;
    fnCallBack.pFnReadUchar         = disp_read_uchar;
    fnCallBack.pFnReadUshort        = disp_read_ushort;
    fnCallBack.pFnReadUlong         = disp_read_ulong;
    fnCallBack.pFnWriteUchar        = disp_write_uchar;
    fnCallBack.pFnWriteUshort       = disp_write_ushort;
    fnCallBack.pFnWriteUlong        = disp_write_ulong;
    fnCallBack.pFnQuerySystemTime   = disp_query_sys_time;
    fnCallBack.pFnAllocateNonpagedMemory = disp_alloc_memory;
    fnCallBack.pFnAllocatePagedMemory= disp_alloc_memory;
    fnCallBack.pFnFreePool          = disp_free_pool;
    fnCallBack.pFnCreateLock        = disp_create_lock;
    fnCallBack.pFnDestroyLock       = disp_destroy_lock;
    fnCallBack.pFnAcquireLock       = disp_acquire_lock;
    fnCallBack.pFnReleaseLock       = disp_release_lock;
    fnCallBack.pFnDbgPrintToFile    = disp_write_log_file;
    fnCallBack.pFnGetPlatformConfigU32 = disp_get_platform_config;

    fnCallBack.pFnMemset            = zx_memset;
    fnCallBack.pFnMemcpy            = zx_memcpy;
    fnCallBack.pFnMemcmp            = zx_memcmp;
    fnCallBack.pFnDodiv             = zx_do_div;
    fnCallBack.pFnVDbgPrint         = zx_cb_vdbgprint;

    if(CBiosSetCallBackFunctions(&fnCallBack) != CBIOS_OK)
    {
        zx_info("CBios set call back func failed.\n");
    }

    if(adapter_info->chip_id >= CHIP_CHX004)
    {
        rom_lenth = disp_get_shadow_rom_image(disp_info);
    }

    zx_memset(&CBChipId, 0, sizeof(CBIOS_CHIP_ID));
    CBChipId.Size = sizeof(CBIOS_CHIP_ID);
    CBChipId.GenericChipID = adapter_info->generic_id;
    CBChipId.ChipID = disp_chipid_to_cbios_chipid(adapter_info->chip_id);

    CBiosStatus = CBiosGetExtensionSize(&CBChipId, &CBiosExtensionSize);

    if(CBiosExtensionSize == 0 || CBiosStatus != CBIOS_OK)
    {
        zx_error("Get cbios extension size failed\n");
    }

    pcbe = zx_calloc(CBiosExtensionSize);

    if(pcbe == NULL)
    {
        zx_error("Alloc memory for cbios pcbe failed\n");
    }

    CBParamInit.pAdapterContext    = disp_info;
    CBParamInit.MAMMPrimaryAdapter = adapter_info->primary;
    CBParamInit.GeneralChipID      = adapter_info->generic_id;
    CBParamInit.ChipID             = disp_chipid_to_cbios_chipid(adapter_info->chip_id);
    CBParamInit.SVID               = adapter_info->sub_sys_vendor_id;
    CBParamInit.SSID               = adapter_info->sub_sys_id;
    CBParamInit.RomImage           = disp_info->rom_image;
    CBParamInit.RomImageLength     = rom_lenth;

    CBParamInit.Size               = sizeof(CBIOS_PARAM_INIT);

#ifdef GFX_ONLY_FPGA
    CBParamInit.DriverFlags.bRunOnQT = 0x1;
#endif

    if (adapter_info->run_on_qt)
    {
        CBParamInit.DriverFlags.bRunOnQT = 0x1;
    }

    if (is_bga_patch_cpu(disp_info))
    {
        CBParamInit.DriverFlags.bBgaPatchCPU = 0x1;
    }

    CBiosInit(pcbe, &CBParamInit);

    disp_info->cbios_ext = pcbe;

    return DISP_OK;
}

int disp_cbios_init_hw(disp_info_t *disp_info)
{
    int                    ret        = DISP_OK;
    int                    cb_status  = CBIOS_OK;

    cb_status = CBiosInitHW(disp_info->cbios_ext);

    if(cb_status != CBIOS_OK)
    {
        zx_error("CBiosInitHW failed. cbios status is %x\n",cb_status);
        ret = DISP_FAIL;
    }

    zx_info("disp cbios_init_hw finished.\n");

    return ret;
}

void disp_cbios_get_crtc_resource(disp_info_t *disp_info)
{
    CBIOS_GET_DISP_RESOURCE   cb_disp_res = {0};
    int  i = 0;

    if (CBIOS_OK == CBiosGetDispResource(disp_info->cbios_ext, &cb_disp_res))
    {
        zx_assert(MAX_CORE_CRTCS >=  cb_disp_res.CrtcNum, "crtc num > MAX_CORE_CRTCS");

        disp_info->num_crtc = cb_disp_res.CrtcNum;

        for (i = 0; i < cb_disp_res.CrtcNum; i++)
        {
            disp_info->num_plane[i]= cb_disp_res.PlaneNum[i];

            if (disp_info->num_plane[i] > ZX_PLANE_NUM)
            {
                zx_error("plane num exceeds max plane num.\n");
                disp_info->num_plane[i] = ZX_PLANE_NUM;
            }
        }
    }
}

void disp_cbios_get_crtc_caps(disp_info_t *disp_info)
{
    CBIOS_DISPLAY_CAPS cb_disp_caps = {0};
    CBIOS_U32      up_plane[MAX_CORE_CRTCS] = {0};
    CBIOS_U32      down_plane[MAX_CORE_CRTCS] = {0};
    int i = 0;

    cb_disp_caps.pUpScalePlaneMask = up_plane;
    cb_disp_caps.pDownScalePlaneMask = down_plane;

    if(CBIOS_OK == CBiosGetDisplayCaps(disp_info->cbios_ext, &cb_disp_caps))
    {
        disp_info->scale_support = cb_disp_caps.SuppCrtcUpScale;   //only use crtc upscaler

        for(i = 0; i < disp_info->num_crtc; i++)
        {
            disp_info->up_scale_plane_mask[i] = up_plane[i];
        }
        for(i = 0; i < disp_info->num_crtc; i++)
        {
            disp_info->down_scale_plane_mask[i] = down_plane[i];
        }
    }
}


int disp_cbios_get_port_attri(disp_info_t *disp_info, int output)
{
    void* pcbe = disp_info->cbios_ext;
    CBiosPortAttribute portattri = {0};
    int conn_type = CBIOS_NON_CONN;

    portattri.Size = sizeof(portattri);

    portattri.DeviceId = output;
    CBiosQueryPortAttribute(pcbe, &portattri);
    conn_type = portattri.PortConnType;

    return conn_type;
}

int disp_cbios_cleanup(disp_info_t *disp_info)
{
    CBiosUnload(disp_info->cbios_ext);
    if(disp_info->rom_image)
    {
        zx_free(disp_info->rom_image);

        disp_info->rom_image = NULL;
    }

    return DISP_OK;
}

void disp_cbios_query_vbeinfo(disp_info_t *disp_info)
{
    adapter_info_t* adapter_info = disp_info->adp_info;
    void* pcbe = disp_info->cbios_ext;
    CBIOS_VBINFO_PARAM vbeinfo = {0};
    int status = CBIOS_OK;

    vbeinfo.Size = sizeof(vbeinfo);
    vbeinfo.BiosVersion = CBIOSVERSION;

    status = CBiosGetVBiosInfo(pcbe, &vbeinfo);
    if(status != CBIOS_OK)
    {
        zx_error("Get cbios vbeinfo failed\n");
    }
    else
    {
        if(adapter_info->chip_id >= CHIP_CHX004)
        {
            adapter_info->low_top_addr = vbeinfo.LowTopAddress;
            adapter_info->snoop_only = vbeinfo.SnoopOnly;
            adapter_info->avai_mem_size_mb = vbeinfo.AvalMemSize;
            adapter_info->total_mem_size_mb = vbeinfo.TotalMemSize;
        }
        adapter_info->ta_enable  = vbeinfo.bTAEnable;
        if(adapter_info->run_on_hypervisor)
        {
            adapter_info->snoop_only = 1;
        }

        disp_info->support_output = vbeinfo.SupportDev;
        disp_info->num_output = disp_get_output_num(vbeinfo.SupportDev);

        disp_info->supp_hpd_outputs   = disp_info->support_output & vbeinfo.HPDDevicesMask;
        disp_info->supp_polling_outputs = disp_info->support_output & vbeinfo.PollingDevMask;

        disp_info->vbios_version = vbeinfo.BiosVersion;

        disp_info->bl_gfx_mode = vbeinfo.bBLGfxMode;

        zx_info("bios supported device: 0x%x\n", disp_info->support_output);
        zx_info("low_top_address : 0x%x\n", adapter_info->low_top_addr);
        zx_info("snoop only: %d\n", adapter_info->snoop_only);
        zx_info("ta_enable (iov enable): %d\n", adapter_info->ta_enable);
        zx_info("bl gfx mode : %d\n", disp_info->bl_gfx_mode);
    }
}

void disp_cbios_read_config_from_efuse(disp_info_t *disp_info)
{
    adapter_info_t* adapter_info = disp_info->adp_info;
    CBIOS_EFUSE_READBYTE_PARA efuse ={0};
    int status = CBIOS_OK;
    
    efuse.Size = sizeof(efuse);
    efuse.ByteAddr = 0x71 * 4;

    adapter_info->vpp_efuse_value = 0;
    
    if (adapter_info->chip_id == CHIP_CNE001)
    {
        status = CBiosEfuseReadByte(disp_info->cbios_ext, &efuse);
        if(status != CBIOS_OK)
        {
            zx_error("Get cbios efuse failed\n");
        }
        else
        {
            adapter_info->vpp_efuse_value = (unsigned int)efuse.Data;
            zx_info("Vpp efuse value: 0x%x\n", adapter_info->vpp_efuse_value);
        }
    }
}

int disp_cbios_get_modes_size(disp_info_t *disp_info, int output)
{
    void              *pcbe = disp_info->cbios_ext;
    int                 mode_size = 0;

    if(CBIOS_OK != CBiosGetDeviceModeListBufferSize(pcbe, output, &mode_size))
    {
        mode_size = 0;
    }

    return  mode_size;
}

int disp_cbios_get_modes(disp_info_t *disp_info, int output, void* buffer, int buf_size)
{
    void                *pcbe = disp_info->cbios_ext;
    int                 real_size = buf_size;
    int                 real_num = 0;

    if(!buffer || !buf_size)
    {
        return 0;
    }

    CBiosGetDeviceModeList(pcbe, output, (PCBiosModeInfoExt)buffer, &real_size);

    if(real_size > buf_size)
    {
        zx_error("OVERFLOW detected: malloc_size: %d, used size: %x.\n", buf_size, real_size);
    }

    real_num = real_size / sizeof(CBiosModeInfoExt);

    return  real_num;
}

int disp_cbios_get_adapter_modes_size(disp_info_t *disp_info)
{
    void              *pcbe = disp_info->cbios_ext;
    int                 mode_size = 0;

    if(CBIOS_OK != CBiosGetAdapterModeListBufferSize(pcbe, &mode_size))
    {
        mode_size = 0;
    }

    return  mode_size;
}

int disp_cbios_get_adapter_modes(disp_info_t *disp_info, void* buffer, int buf_size)
{
    void                *pcbe = disp_info->cbios_ext;
    int                 real_size = buf_size;
    int                 real_num = 0;

    if(!buffer || !buf_size)
    {
        return 0;
    }

    CBiosGetAdapterModeList(pcbe, (PCBiosModeInfoExt)buffer, &real_size);

    if(real_size > buf_size)
    {
        zx_error("OVERFLOW detected: malloc_size: %d, used size: %x.\n", buf_size, real_size);
    }

    real_num = real_size / sizeof(CBiosModeInfoExt);

    return  real_num;
}

CBiosModeInfoExt* disp_cbios_get_preferred_mode(CBiosModeInfoExt *dev_mode_list, unsigned int mode_num)
{
    CBiosModeInfoExt *pcbios_mode = NULL;
    int i = 0;

    if(!dev_mode_list || !mode_num)
    {
        return NULL;
    }

    for (i = 0; i < mode_num; i++)
    {
        pcbios_mode = dev_mode_list + i;

        if (pcbios_mode->isPreferredMode)
        {
            break;
        }
    }

    if(i >= mode_num)
    {
        pcbios_mode = dev_mode_list; //not find prefer, use mode[0]
    }

    return pcbios_mode;
}

static int is_adapter_mode_valid(CBiosModeInfoExt *pmode, CBiosModeInfoExt *prefer_mode)
{
    if(pmode->XRes == prefer_mode->XRes && pmode->YRes == prefer_mode->YRes)
    {
        return 0;
    }
    
    if (pmode->XRes <= prefer_mode->XRes && pmode->YRes <= prefer_mode->YRes)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

int disp_cbios_merge_modes(CBiosModeInfoExt* merge_mode_list, CBiosModeInfoExt * adapter_mode_list, const unsigned int adapter_mode_num,
    const CBiosModeInfoExt * dev_mode_list, const unsigned int dev_mode_num)
{
    CBiosModeInfoExt    tmpBuf;
    unsigned int        i = 0;
    unsigned int        adapter_index      = 0;
    unsigned int        dev_index          = 0;
    unsigned int        mode_index         = 0;
    unsigned int        num_mode           = 0;
    CBiosModeInfoExt* prefer_mode = NULL;

    // reverse adapter modelist
    for (i = 0; i < adapter_mode_num/2; i++)
    {
        zx_memcpy(&tmpBuf, &adapter_mode_list[i], sizeof(CBiosModeInfoExt));
        zx_memcpy(&adapter_mode_list[i], &adapter_mode_list[adapter_mode_num - i -1], sizeof(CBiosModeInfoExt));
        zx_memcpy(&adapter_mode_list[adapter_mode_num - i -1], &tmpBuf, sizeof(CBiosModeInfoExt));
    }

    prefer_mode = disp_cbios_get_preferred_mode((PCBiosModeInfoExt)dev_mode_list, dev_mode_num);

    // merge
    dev_index  = 0;
    mode_index = 0;
    num_mode   = 0;
    adapter_index = 0;
    while ((dev_index < dev_mode_num) && (adapter_index < adapter_mode_num))
    {
        if(!is_adapter_mode_valid(&adapter_mode_list[adapter_index], prefer_mode))
        {
            adapter_index++;
            continue;
        }
        
        if ((dev_mode_list[dev_index].XRes > adapter_mode_list[adapter_index].XRes) ||
            ((dev_mode_list[dev_index].XRes == adapter_mode_list[adapter_index].XRes) &&
             (dev_mode_list[dev_index].YRes > adapter_mode_list[adapter_index].YRes)))
        {
            zx_memcpy(&merge_mode_list[mode_index], &dev_mode_list[dev_index], sizeof(CBiosModeInfoExt));
            mode_index++;
            dev_index++;
            num_mode++;
        }
        else if ((dev_mode_list[dev_index].XRes == adapter_mode_list[adapter_index].XRes) &&
                 (dev_mode_list[dev_index].YRes == adapter_mode_list[adapter_index].YRes))
        {
            zx_memcpy(&merge_mode_list[mode_index], &dev_mode_list[dev_index], sizeof(CBiosModeInfoExt));
            merge_mode_list[mode_index].ColorDepthCaps |= adapter_mode_list[adapter_index].ColorDepthCaps;
            mode_index++;
            dev_index++;
            adapter_index++;
            num_mode++;
        }
        else
        {
            zx_memcpy(&merge_mode_list[mode_index], &adapter_mode_list[adapter_index], sizeof(CBiosModeInfoExt));
            mode_index++;
            adapter_index++;
            num_mode++;
        }
    }

    if ((dev_index == dev_mode_num) && (adapter_index < adapter_mode_num))
    {
        while(adapter_index < adapter_mode_num)
        {
            if(is_adapter_mode_valid(&adapter_mode_list[adapter_index], prefer_mode))
            {
                zx_memcpy(&merge_mode_list[mode_index], &adapter_mode_list[adapter_index], sizeof(CBiosModeInfoExt));
                num_mode++;
                mode_index++;
            }
            adapter_index++;
        }
    }

    if ((dev_index < dev_mode_num) && (adapter_index == adapter_mode_num))
    {
        zx_memcpy(&merge_mode_list[mode_index], &dev_mode_list[dev_index],
                   sizeof(CBiosModeInfoExt)*(dev_mode_num - dev_index));
        num_mode += (dev_mode_num - dev_index);
    }

    return num_mode;
}

int disp_cbios_cbmode_to_drmmode(disp_info_t *disp_info, int output, void* cbmode, int i, struct drm_display_mode *drm_mode)
{
    void                *pcbe = disp_info->cbios_ext;
    CBIOS_GET_MODE_TIMING_PARAM   get_timing = {0};
    CBIOS_TIMING_ATTRIB    timing_attrib = {0};
    PCBiosModeInfoExt  cbios_mode = (PCBiosModeInfoExt)cbmode + i;

    if(!cbios_mode || !drm_mode)
    {
        return  DISP_FAIL;
    }

    get_timing.DeviceId = output;
    get_timing.pMode = cbios_mode;
    get_timing.pTiming = &timing_attrib;
    if(CBIOS_OK != CBiosGetModeTiming(pcbe, &get_timing))
    {
        return  DISP_FAIL;
    }

    drm_mode->clock = timing_attrib.PixelClock / 10;
    drm_mode->hdisplay = timing_attrib.XRes;    
    drm_mode->hsync_start = timing_attrib.HorSyncStart;
    drm_mode->hsync_end = timing_attrib.HorSyncEnd;
    drm_mode->htotal = timing_attrib.HorTotal;
    drm_mode->hskew = 0;

    drm_mode->vdisplay = timing_attrib.YRes;
    drm_mode->vsync_start = timing_attrib.VerSyncStart;
    drm_mode->vsync_end = timing_attrib.VerSyncEnd;
    drm_mode->vtotal = timing_attrib.VerTotal;
    drm_mode->vscan = 0;

#if DRM_VERSION_CODE < KERNEL_VERSION(5,9,0)
    drm_mode->vrefresh = timing_attrib.RefreshRate / 100;
#endif
    drm_mode->type |= DRM_MODE_TYPE_DRIVER;
    if (cbios_mode->isPreferredMode)
    {
        drm_mode->type |= DRM_MODE_TYPE_PREFERRED;
    }

    if (cbios_mode->isInterlaceMode)
    {
        drm_mode->flags |= DRM_MODE_FLAG_INTERLACE;
    }

    drm_mode->flags |= (timing_attrib.HVPolarity & HOR_NEGATIVE) ?
                DRM_MODE_FLAG_NHSYNC : DRM_MODE_FLAG_PHSYNC;
    drm_mode->flags |= (timing_attrib.HVPolarity & VER_NEGATIVE) ?
                DRM_MODE_FLAG_NVSYNC : DRM_MODE_FLAG_PVSYNC;
    drm_mode->flags |= DRM_MODE_FLAG_3D_NONE;

    return  DISP_OK;
}

int disp_cbios_3dmode_to_drmmode(disp_info_t *disp_info, int output, void* mode, int i, struct drm_display_mode *drm_mode)
{
    void                *pcbe = disp_info->cbios_ext;
    CBIOS_GET_MODE_TIMING_PARAM   get_timing = {0};
    CBIOS_TIMING_ATTRIB    timing_attrib = {0};
    CBiosModeInfoExt  cbios_mode = {0};
    PCBIOS_3D_VIDEO_MODE_LIST  cb_3d_mode = (PCBIOS_3D_VIDEO_MODE_LIST)mode + i;

    if(!cb_3d_mode || !drm_mode)
    {
        return  DISP_FAIL;
    }

    cbios_mode.XRes = cb_3d_mode->XRes;
    cbios_mode.YRes = cb_3d_mode->YRes;
    cbios_mode.RefreshRate = cb_3d_mode->RefreshRate;
    cbios_mode.isInterlaceMode = (cb_3d_mode->bIsInterlace)? 1 : 0;

    get_timing.DeviceId = output;
    get_timing.pMode = &cbios_mode;
    get_timing.pTiming = &timing_attrib;

    if(CBIOS_OK != CBiosGetModeTiming(pcbe, &get_timing))
    {
        return  DISP_FAIL;
    }

    drm_mode->clock = timing_attrib.PixelClock / 10;
    drm_mode->hdisplay = timing_attrib.XRes;
    drm_mode->hsync_start = timing_attrib.HorSyncStart;
    drm_mode->hsync_end = timing_attrib.HorSyncEnd;
    drm_mode->htotal = timing_attrib.HorTotal;
    drm_mode->hskew = 0;

    drm_mode->vdisplay = timing_attrib.YRes;
    drm_mode->vsync_start = timing_attrib.VerSyncStart;
    drm_mode->vsync_end = timing_attrib.VerSyncEnd;
    drm_mode->vtotal = timing_attrib.VerTotal;
    drm_mode->vscan = 0;

#if DRM_VERSION_CODE < KERNEL_VERSION(5,9,0)
    drm_mode->vrefresh = timing_attrib.RefreshRate / 100;
#endif

    drm_mode->type |= DRM_MODE_TYPE_DRIVER;

    if (cb_3d_mode->bIsInterlace)
    {
        drm_mode->flags |= DRM_MODE_FLAG_INTERLACE;
    }
    drm_mode->flags |= (timing_attrib.HVPolarity & HOR_NEGATIVE) ?
             DRM_MODE_FLAG_NHSYNC : DRM_MODE_FLAG_PHSYNC;
    drm_mode->flags |= (timing_attrib.HVPolarity & VER_NEGATIVE) ?
             DRM_MODE_FLAG_NVSYNC : DRM_MODE_FLAG_PVSYNC;

    if (cb_3d_mode->SupportStructures.FramePacking)
    {
        drm_mode->flags |= DRM_MODE_FLAG_3D_FRAME_PACKING;
    }
    if (cb_3d_mode->SupportStructures.FieldAlternative)
    {
        drm_mode->flags |= DRM_MODE_FLAG_3D_FIELD_ALTERNATIVE;
    }
    if (cb_3d_mode->SupportStructures.LineAlternative)
    {
        drm_mode->flags |= DRM_MODE_FLAG_3D_LINE_ALTERNATIVE;
    }
    if (cb_3d_mode->SupportStructures.SideBySideFull)
    {
        drm_mode->flags |= DRM_MODE_FLAG_3D_SIDE_BY_SIDE_FULL;
    }
    if (cb_3d_mode->SupportStructures.LDepth)
    {
        drm_mode->flags |= DRM_MODE_FLAG_3D_L_DEPTH;
    }
    if (cb_3d_mode->SupportStructures.LDepthGraphics)
    {
        drm_mode->flags |= DRM_MODE_FLAG_3D_L_DEPTH_GFX_GFX_DEPTH;
    }
    if (cb_3d_mode->SupportStructures.TopAndBottom)
    {
        drm_mode->flags |= DRM_MODE_FLAG_3D_TOP_AND_BOTTOM;
    }
    if (cb_3d_mode->SupportStructures.SideBySideHalf)
    {
        drm_mode->flags |= DRM_MODE_FLAG_3D_SIDE_BY_SIDE_HALF;
    }

    return  DISP_OK;
}

int disp_cbios_get_3dmode_size(disp_info_t* disp_info, int output)
{
    void                               *pcbe = disp_info->cbios_ext;
    CBIOS_MONITOR_3D_CAPABILITY_PARA   CbiosMonitor3DCapablitityPara    = {0};
    int  buf_size = 0, support = 0;

    CbiosMonitor3DCapablitityPara.DeviceId           = output;
    CbiosMonitor3DCapablitityPara.pMonitor3DModeList = NULL;

    if(CBIOS_OK == CBiosQueryMonitor3DCapability(pcbe, &CbiosMonitor3DCapablitityPara))
    {
        support = CbiosMonitor3DCapablitityPara.bIsSupport3DVideo;
    }

    if(support)
    {
        buf_size = CbiosMonitor3DCapablitityPara.Monitor3DModeNum;
        buf_size *= sizeof(CBIOS_3D_VIDEO_MODE_LIST);
    }

    return  buf_size;
}

int disp_cbios_get_3dmodes(disp_info_t *disp_info, int output, void* buffer, int buf_size)
{
    void                               *pcbe = disp_info->cbios_ext;
    CBIOS_MONITOR_3D_CAPABILITY_PARA   CbiosMonitor3DCapablitityPara    = {0};
    int  real_size = 0, real_num = 0;

    if(!buffer || !buf_size)
    {
        return real_num;
    }

    CbiosMonitor3DCapablitityPara.DeviceId           = output;
    CbiosMonitor3DCapablitityPara.pMonitor3DModeList = (PCBIOS_3D_VIDEO_MODE_LIST)buffer;

    CBiosQueryMonitor3DCapability(pcbe, &CbiosMonitor3DCapablitityPara);

    if(CbiosMonitor3DCapablitityPara.bIsSupport3DVideo)
    {
        real_size = CbiosMonitor3DCapablitityPara.Monitor3DModeNum * sizeof(CBIOS_3D_VIDEO_MODE_LIST);
        if(real_size > buf_size)
        {
            zx_error("OVERFLOW detected: malloc_size: %d, used size: %x.\n", buf_size, real_size);
        }
        real_num = CbiosMonitor3DCapablitityPara.Monitor3DModeNum;
    }

    return  real_num;
}

int disp_cbios_get_mode_timing(disp_info_t *disp_info, int output, void* cb_mode, struct drm_display_mode *drm_mode)
{
    void                *pcbe = disp_info->cbios_ext;
    CBIOS_GET_MODE_TIMING_PARAM   get_timing = {0};
    CBIOS_TIMING_ATTRIB    timing_attrib = {0};

    if(!cb_mode || !drm_mode)
    {
        return  DISP_FAIL;
    }

    get_timing.DeviceId = output;
    get_timing.pMode = (PCBiosModeInfoExt)cb_mode;
    get_timing.pTiming = &timing_attrib;
    if(CBIOS_OK != CBiosGetModeTiming(pcbe, &get_timing))
    {
        return  DISP_FAIL;
    }

    drm_mode->crtc_clock = timing_attrib.PixelClock / 10;
    drm_mode->crtc_hdisplay = timing_attrib.XRes;
    drm_mode->crtc_hblank_start = timing_attrib.HorBStart;
    drm_mode->crtc_hblank_end = timing_attrib.HorBEnd;
    drm_mode->crtc_hsync_start = timing_attrib.HorSyncStart;
    drm_mode->crtc_hsync_end = timing_attrib.HorSyncEnd;
    drm_mode->crtc_htotal = timing_attrib.HorTotal;
    drm_mode->crtc_hskew = 0;

    drm_mode->crtc_vdisplay = timing_attrib.YRes;
    drm_mode->crtc_vblank_start = timing_attrib.VerBStart;
    drm_mode->crtc_vblank_end = timing_attrib.VerBEnd;
    drm_mode->crtc_vsync_start = timing_attrib.VerSyncStart;
    drm_mode->crtc_vsync_end = timing_attrib.VerSyncEnd;
    drm_mode->crtc_vtotal = timing_attrib.VerTotal;

    return DISP_OK;
}

void* disp_cbios_read_edid(disp_info_t *disp_info, int output, int* edid_len)
{
    void         *pcbe    = disp_info->cbios_ext;
    CBIOS_PARAM_GET_EDID  cbParamGetEdid = {0};

    cbParamGetEdid.Size          = sizeof(CBIOS_PARAM_GET_EDID);
    cbParamGetEdid.DeviceId      = output;

    if(CBiosGetEdid(pcbe, &cbParamGetEdid) == CBIOS_ER_EDID_INVALID)
    {
        zx_info("device id: 0x%x has no valid EDID.\n", output);
        return NULL;
    }

    cbParamGetEdid.EdidBuffer = zx_malloc(cbParamGetEdid.EdidLen);
    if(!cbParamGetEdid.EdidBuffer)
    {
        return  NULL;
    }

    cbParamGetEdid.EdidBufferLen = cbParamGetEdid.EdidLen;
    if (CBiosGetEdid(pcbe, &cbParamGetEdid) != CBIOS_OK)
    {
        zx_info("device id: 0x%x read EDID fail.\n", output);
        zx_free(cbParamGetEdid.EdidBuffer);
        cbParamGetEdid.EdidBuffer = NULL;
        return NULL;
    }

    if(edid_len)
    {
        *edid_len = (int)cbParamGetEdid.EdidLen;
    }

    return cbParamGetEdid.EdidBuffer;
}

void disp_cbios_set_edid(disp_info_t *disp_info, int output, char* edid, int size)
{
    void         *pcbe    = disp_info->cbios_ext;
    CBIOS_PARAM_SET_EDID set_edid = {0};

    if(!output)
    {
        return;
    }

    set_edid.Size = sizeof(set_edid);
    set_edid.DeviceId = output;
    set_edid.EdidData.Buffer = (PCBIOS_UCHAR)edid;
    set_edid.EdidData.BufferSize = (CBIOS_U32)size;

    if(CBiosSetEdid(pcbe, &set_edid) != CBIOS_OK)
    {
        zx_info("device id: 0x%x Override edid failed.\n", output);
    }
}

int disp_cbios_update_output_active(disp_info_t *disp_info, int* outputs)
{
    void                        *pcbe = disp_info->cbios_ext;
    CIP_ACTIVE_DEVICES          activeDevices = {0};

    zx_memcpy(activeDevices.DeviceId, outputs, sizeof(activeDevices.DeviceId));

    return (CBiosSetActiveDevice(pcbe, &activeDevices) == CBIOS_OK)? 0 : -1;
}

int disp_cbios_sync_vbios(disp_info_t *disp_info)
{
    int status = DISP_OK;
    CBIOS_VBIOS_DATA_PARAM DataParam = {0};

    if(CBIOS_OK != CBiosSyncDataWithVbios(disp_info->cbios_ext, &DataParam))
    {
        status = DISP_FAIL;
    }
    return status;
}

int disp_cbios_get_active_devices(disp_info_t *disp_info, int* devices)
{
    int status = DISP_OK;
    CIP_ACTIVE_DEVICES          activeDevices = {0};

    if(CBIOS_OK != CBiosGetActiveDevice(disp_info->cbios_ext, &activeDevices))
    {
        status = DISP_FAIL;
    }
    else
    {
        zx_memcpy(devices, activeDevices.DeviceId, sizeof(activeDevices.DeviceId));
    }
    return status;
}

int disp_cbios_detect_output_status(disp_info_t *disp_info, int device_bit, int full_detect, int* changed)
{
    void                        *pcbe = disp_info->cbios_ext;
    CBIOS_OUTPUT_DETECT_PARAM    detect_para = {0};
    int                         status;
    int                         connected = 0;

    if(changed)
    {
        *changed = 0;
    }

    if(!device_bit || (device_bit & (device_bit - 1)))
    {
        return 0;
    }

    detect_para.Size = sizeof(detect_para);
    detect_para.OutputToDetect = device_bit;
    detect_para.bFullDetect = (full_detect)? 1 : 0;

    status = CBiosDetectOutputStatus(pcbe, &detect_para);

    if(status == CBIOS_OK)
    {
        connected = (detect_para.bConnected)? 1 : 0;
        if(changed)
        {
            *changed = (detect_para.bChanged)? 1 : 0;
        }
    }

    return connected;
}


int disp_cbios_set_hdac_connect_status(disp_info_t *disp_info, int device , int bPresent, int bEldValid)
{
    void                    *pcbe         = disp_info->cbios_ext;
    CBIOS_HDAC_PARA         CbiosHDACPara = {0};
    CBiosPortAttribute      portattri = {0};

    portattri.Size = sizeof(portattri);
    portattri.DeviceId = device;
    CBiosQueryPortAttribute(pcbe, &portattri);

    // zx_connector->monitor_type == 0 if plugout monitor, use SupportMonitorType here.
    if (portattri.SupportMonitorType & (UT_OUTPUT_TYPE_HDMI | UT_OUTPUT_TYPE_DP | UT_OUTPUT_TYPE_MHL))
    {
        CbiosHDACPara.Size = sizeof(CBIOS_HDAC_PARA);
        CbiosHDACPara.DeviceId = device;
        CbiosHDACPara.bPresent  = bPresent;
        CbiosHDACPara.bEldValid = bEldValid;

        CBiosSetHDACConnectStatus(pcbe, &CbiosHDACPara);
    }
    return DISP_OK;
}

static CBIOS_U32 disp_output_siganl_map[] = {CBIOS_RGBOUTPUT, CBIOS_YCBCR422OUTPUT, CBIOS_YCBCR444OUTPUT, CBIOS_YCBCR420OUTPUT};
int disp_cbios_set_mode(disp_info_t *disp_info, int crtc, struct drm_display_mode* mode, struct drm_display_mode* adjusted_mode, update_mode_para_t update_para)
{
    void*                   pcbe = disp_info->cbios_ext;
    CBIOS_STATUS            cb_status;
    int  temp = 0;

    CBiosSettingModeParams  mode_param = {0};

    temp = adjusted_mode->clock * 1000/adjusted_mode->htotal;
    mode_param.DestModeParams.RefreshRate = DIV_ROUND_CLOSEST(temp * 100, adjusted_mode->vtotal);

    mode_param.SourcModeParams.XRes = mode->hdisplay;
    mode_param.SourcModeParams.YRes = mode->vdisplay;

    mode_param.DestModeParams.XRes = adjusted_mode->hdisplay;
    mode_param.DestModeParams.YRes = adjusted_mode->vdisplay;
    mode_param.DestModeParams.XTotal = adjusted_mode->htotal;
    mode_param.DestModeParams.YTotal = adjusted_mode->vtotal;
    mode_param.DestModeParams.PixelClock = adjusted_mode->clock * 10;

    mode_param.DestModeParams.InterlaceFlag = (adjusted_mode->flags & DRM_MODE_FLAG_INTERLACE)? 1 : 0;
    mode_param.DestModeParams.OutputSignal = disp_output_siganl_map[update_para.output_signal];

    mode_param.ScalerSizeParams.XRes = adjusted_mode->hdisplay;
    mode_param.ScalerSizeParams.YRes = adjusted_mode->vdisplay;

    mode_param.IGAIndex = crtc;
    mode_param.BitPerComponent = 8;
    mode_param.SkipIgaMode = update_para.set_crtc ? 0 : 1;
    mode_param.SkipDeviceMode = update_para.set_encoder ? 0 : 1;

    if(mode_param.SkipIgaMode == 0)
    {
        zx_info("KMS set mode to path(iga_index-->active_device): %d-->0x%x.\n", crtc, disp_info->active_output[crtc]);
    }

    if(adjusted_mode->flags & DRM_MODE_FLAG_3D_MASK)
    {
        mode_param.Is3DVideoMode = 1;
        mode_param.IsSingleBuffer= 0;
        if(mode->flags & DRM_MODE_FLAG_3D_FRAME_PACKING)
        {
            mode_param.Video3DStruct = FRAME_PACKING;
        }
        else if(mode->flags & DRM_MODE_FLAG_3D_FIELD_ALTERNATIVE)
        {
            mode_param.Video3DStruct = FIELD_ALTERNATIVE;
        }
        else if(mode->flags & DRM_MODE_FLAG_3D_LINE_ALTERNATIVE)
        {
            mode_param.Video3DStruct = LINE_ALTERNATIVE;
        }
        else if(mode->flags & DRM_MODE_FLAG_3D_SIDE_BY_SIDE_FULL)
        {
            mode_param.Video3DStruct = SIDE_BY_SIDE_FULL;
        }
        else if(mode->flags & DRM_MODE_FLAG_3D_L_DEPTH)
        {
            mode_param.Video3DStruct = L_DEPTH;
        }
        else if(mode->flags & DRM_MODE_FLAG_3D_L_DEPTH_GFX_GFX_DEPTH)
        {
            mode_param.Video3DStruct = L_DEPTH_GRAPHICS;
        }
        else if(mode->flags & DRM_MODE_FLAG_3D_TOP_AND_BOTTOM)
        {
            mode_param.Video3DStruct = TOP_AND_BOTTOM;
        }
        else if(mode->flags & DRM_MODE_FLAG_3D_SIDE_BY_SIDE_HALF)
        {
            mode_param.Video3DStruct = SIDE_BY_SIDE_HALF;
        }
    }

    cb_status = CBiosSetModeToIGA(pcbe, &mode_param);

    return (cb_status == CBIOS_OK) ? DISP_OK : DISP_FAIL;
}

int disp_cbios_turn_onoff_screen(disp_info_t *disp_info, int iga, int bOn)
{
    void                        *pcbe = disp_info->cbios_ext;
    int                         cb_status;

    cb_status = CBiosSetIgaScreenOnOffState(pcbe, bOn, iga);

    return (cb_status == CBIOS_OK) ? DISP_OK : DISP_FAIL;
}

int disp_cbios_turn_onoff_iga(disp_info_t *disp_info, int iga, int bOn)
{
    void                        *pcbe = disp_info->cbios_ext;
    int                         cb_status;

    cb_status = CBiosSetIgaOnOffState(pcbe, bOn, iga);

    return (cb_status == CBIOS_OK) ? DISP_OK : DISP_FAIL;

}

int disp_cbios_set_dpms(disp_info_t *disp_info, int device, int dpms_on, unsigned int flags)
{
    int status = DISP_OK;

    if(CBIOS_OK != CBiosSetDisplayDevicePowerState(disp_info->cbios_ext, device, dpms_on, flags))
    {
        status = DISP_FAIL;
    }

    return status;
}

int disp_cbios_set_gamma(disp_info_t *disp_info, int pipe, void* data)
{
    int  status;
    void  *pcbe = disp_info->cbios_ext;
    CBIOS_GAMMA_PARA gamma_para = {0};

    if(pipe >= disp_info->num_crtc)
    {
        return DISP_FAIL;
    }

    gamma_para.pGammaTable = data;
    gamma_para.IGAIndex = pipe;
    gamma_para.FisrtEntryIndex = 0;
    gamma_para.EntryNum = 256;
    gamma_para.Flags.bConfigGamma = 1;
    gamma_para.Flags.bSetLUT = 1;

    status = CBiosSetGamma(pcbe, &gamma_para);

    return (status == CBIOS_OK)? DISP_OK : DISP_FAIL;
}

int disp_cbios_get_connector_attrib(disp_info_t *disp_info, zx_connector_t *zx_connector)
{
    void*  pcbe = disp_info->cbios_ext;
    CBIOS_STATUS   status = CBIOS_OK;
    CBiosMonitorAttribute attrib = {0};

    attrib.Size = sizeof(CBiosMonitorAttribute);
    attrib.ActiveDevId = zx_connector->output_type;

    status = CBiosQueryMonitorAttribute(pcbe, &attrib);

    if (status == CBIOS_OK)
    {
        zx_connector->base_connector.display_info.width_mm = attrib.MonitorHorSize;
        zx_connector->base_connector.display_info.height_mm = attrib.MonitorVerSize;
        zx_connector->monitor_type = disp_biosmonitor_to_output(attrib.MonitorType);
        zx_connector->support_audio = attrib.bSupportHDAudio ? 1 : 0;
        if(zx_connector->monitor_type == UT_OUTPUT_TYPE_PANEL)
        {
            zx_connector->support_psr = attrib.bSupportPsr ? 1:0;
        }
    }

    return  (status == CBIOS_OK)? DISP_OK : DISP_FAIL;
}

int  disp_cbios_get_crtc_mask(disp_info_t *disp_info,  int device)
{
    void*  pcbe = disp_info->cbios_ext;
    CBIOS_STATUS   status = CBIOS_OK;
    CBIOS_GET_IGA_MASK   GetIgaMask = {0};

    GetIgaMask.Size = sizeof(CBIOS_GET_IGA_MASK);
    GetIgaMask.DeviceId = device;

    status = CBiosGetIgaMask(pcbe, &GetIgaMask);

    return  (status == CBIOS_OK)?  GetIgaMask.IgaMask : 0;
}

int disp_cbios_get_clock(disp_info_t *disp_info, unsigned int type, unsigned int *output)
{
    CBios_GetClock_Params GetClock = {0};

    int   status = CBIOS_OK;

    GetClock.Size      = sizeof(CBios_GetClock_Params);
    GetClock.ClockFreq = output;

    if(type == ZX_QUERY_ENGINE_CLOCK)
    {
        GetClock.ClockType = CBIOS_ECLKTYPE;
    }
    else if(type == ZX_QUERY_I_CLOCK)
    {
        GetClock.ClockType = CBIOS_ICLKTYPE;
    }
    else if (type == ZX_QUERY_CPU_FREQUENCE)
    {
       GetClock.ClockType = CBIOS_CPUFRQTYPE;
    }
    else if(type == ZX_QUERY_VCLK)
    {
        GetClock.ClockType = CBIOS_VCLKTYPE;
    }
    else if(type == ZX_QUERY_VCP0_CLOCK)
    {
        GetClock.ClockType = CBIOS_VCP0CLKTYPE;
    }
    else if(type == ZX_QUERY_VCP1_CLOCK)
    {
        GetClock.ClockType = CBIOS_VCP1CLKTYPE;
    }
    else if(type == ZX_QUERY_MCLK)
    {
        GetClock.ClockType = CBIOS_MCLKTYPE;
    }
    else if(type == ZX_QUERY_CMU_CLOCK)
    {
        GetClock.ClockType = CBIOS_CMUCLKTYPE;
    }
    else
    {
        zx_error("unknown get clock type: %d.\n", type);

        zx_assert(0, "Invalid clock type.");
    }

    status = CBiosGetClock(disp_info->cbios_ext, &GetClock);

    if(status != CBIOS_OK)
    {
        zx_error("dispmgri_get_clock failed: %d.\n", status);
    }

    return (status == CBIOS_OK) ? DISP_OK : DISP_FAIL;
}

int disp_cbios_set_clock(disp_info_t *disp_info, unsigned int type, unsigned int para)
{
    CBios_SetClock_Params SetClock = {0};

    int   status = CBIOS_OK;

    SetClock.Size = sizeof(CBios_SetClock_Params);

    if (type == ZX_SET_I_CLOCK)
    {
        SetClock.ClockType = CBIOS_ICLKTYPE;
        SetClock.ClockFreq = para;
    }
    else if (type == ZX_SET_CPU_FREQUENCE)
    {
        SetClock.ClockType = CBIOS_CPUFRQTYPE;
        SetClock.ClockFreq = para;
    }
    else
    {
        zx_error("unknown set clock type: %d.\n", type);

        zx_assert(0, "Invalid clock type.");
    }

    status = CBiosSetClock(disp_info->cbios_ext, &SetClock);

    if(status != CBIOS_OK)
    {
        zx_error("dispmgri_set_clock failed: %d.\n", status);
    }

    return (status == CBIOS_OK) ? DISP_OK : DISP_FAIL;
}

int disp_cbios_enable_hdcp(disp_info_t *disp_info, unsigned int enable, unsigned int devices)
{
    CBiosContentProtectionOnOffParams hdcp_para = {0};

    int   status = CBIOS_OK;

    hdcp_para.Size = sizeof(CBiosContentProtectionOnOffParams);
    hdcp_para.DevicesId = devices;
    hdcp_para.bHdcpStatus = enable ? CBIOS_TRUE : CBIOS_FALSE;

    status = CBiosContentProtectionOnOff(disp_info->cbios_ext, &hdcp_para);

    if (status != CBIOS_OK)
    {
        zx_error("dispmgri_cbios_enable_dhcp failed\n");
    }

    return (status == CBIOS_OK) ? DISP_OK : DISP_FAIL;
}

int disp_cbios_get_hdcp_status(disp_info_t *disp_info, zx_hdcp_op_t *dhcp_op, unsigned int devices)
{
    CBIOS_HDCP_STATUS_PARA     hdcp_status = {0};

    int status = CBIOS_OK;

    hdcp_status.Size = sizeof(CBIOS_HDCP_STATUS_PARA);
    hdcp_status.DevicesId = devices;

    status = CBiosGetHDCPStatus(disp_info->cbios_ext, &hdcp_status);

    if (status == CBIOS_OK)
    {
        dhcp_op->result = hdcp_status.HdcpStatus == CBIOS_TRUE ? ZX_HDCP_ENABLED : ZX_HDCP_FAILED;
    }
    else
    {
        zx_error("dispmgri_cbios_get_hdcp_status failed\n");
    }

    return (status == CBIOS_OK) ? DISP_OK : DISP_FAIL;
}

static unsigned int DrmFormat2CBiosFormat(unsigned int drm_format)
{
    CBIOS_FORMAT cbios_format = CBIOS_FMT_INVALID;
    switch (drm_format)
    {
        case DRM_FORMAT_RGB565:
            cbios_format = CBIOS_FMT_R5G6B5;
            break;
        case DRM_FORMAT_ARGB8888:
            cbios_format = CBIOS_FMT_A8R8G8B8;
            break;
        case DRM_FORMAT_ABGR8888:
            cbios_format = CBIOS_FMT_A8B8G8R8;
            break;
        case DRM_FORMAT_XBGR8888:
            cbios_format = CBIOS_FMT_X8B8G8R8;
            break;
        case DRM_FORMAT_XRGB8888:
            cbios_format = CBIOS_FMT_X8R8G8B8;
            break;
        case DRM_FORMAT_ARGB2101010:
            cbios_format = CBIOS_FMT_A2R10G10B10;
            break;
        case DRM_FORMAT_ABGR2101010:
            cbios_format = CBIOS_FMT_A2B10G10R10;
            break;
        case DRM_FORMAT_XRGB2101010:
            cbios_format = CBIOS_FMT_X2R10G10B10;
            break;
        case DRM_FORMAT_XBGR2101010:
            cbios_format = CBIOS_FMT_X2B10G10R10;
            break;
        case DRM_FORMAT_YUYV:
        case DRM_FORMAT_YVYU:
            cbios_format = CBIOS_FMT_CRYCBY422_16BIT;
            break;
        case DRM_FORMAT_UYVY:
        case DRM_FORMAT_VYUY:
            cbios_format = CBIOS_FMT_YCRYCB422_16BIT;
            break;
        case DRM_FORMAT_AYUV:
            cbios_format = CBIOS_FMT_YCBCR8888_32BIT;
            break;
        default:
            zx_assert(0, "invalid drm format");
            cbios_format = CBIOS_FMT_A8R8G8B8;
            break;
    }

    return cbios_format;
}

int disp_cbios_crtc_flip(disp_info_t *disp_info, zx_crtc_flip_t *arg)
{
    zx_card_t* zx_card = disp_info->zx_card;
    struct drm_framebuffer *fb = arg->fb;
    struct drm_zx_framebuffer *zxfb = fb? to_zxfb(fb) : NULL;
    struct drm_zx_framebuffer *zxoldfb = (arg->oldfb)? to_zxfb(arg->oldfb) : NULL;
    psr_data_t* psr_data = disp_info->psr_data;
    CBIOS_OVERLAY_INFO   overlay = {0};
    CBIOS_STREAM_PARA   input_stream = {0};
    CBIOS_PLANE_PARA      disp_plane = {0};
    CBIOS_UPDATE_FRAME_PARA  update_frame = {0};
    struct drm_fb_helper *fb_helper = NULL;
    unsigned int acq_psr = 0, rel_psr = 0;

    update_frame.Size = sizeof(update_frame);
    update_frame.IGAIndex = arg->crtc;
    update_frame.pPlanePara[0] = &disp_plane;

    disp_plane.PlaneIndex = arg->stream_type;
    disp_plane.StreamType = arg->stream_type;
    disp_plane.pInputStream = &input_stream;
    if(fb)
    {
        disp_plane.FlipMode.FlipType = CBIOS_PLANE_FLIP_WITH_ENABLE;
    }
    else
    {
        disp_plane.FlipMode.FlipType = CBIOS_PLANE_FLIP_WITH_DISABLE;
    }

    zx_card->primary_allocation[arg->crtc] = zxfb ? zxfb->obj->priv:0;

#if DRM_VERSION_CODE >= KERNEL_VERSION(4, 19, 0)
    fb_helper = zx_card->drm_dev->fb_helper;
#else
    if(zx_card->fbdev)
    {
        fb_helper = &((struct zx_fbdev*)zx_card->fbdev)->helper;
    }
#endif

    if(fb_helper && fb_helper->fb
        && fb_helper->fb != arg->oldfb && fb_helper->fb == fb)
    {
        acq_psr = 1;
    }
    else if(fb_helper && fb_helper->fb
                && fb_helper->fb == arg->oldfb && fb_helper->fb != fb)
    {
        rel_psr = 1;
    }

    if((!zxoldfb || (zxoldfb && !zxoldfb->obj->dumb_mapped)) 
        && (zxfb && zxfb->obj->dumb_mapped))
    {
        acq_psr = 1;
    }
    else if((zxoldfb && zxoldfb->obj->dumb_mapped)
        && (!zxfb || (zxfb && !zxfb->obj->dumb_mapped)))
    {
        rel_psr = 1;
    }

    if(acq_psr && !rel_psr)
    {
        psr_acquire_display(psr_data, PSR_FB_REF, 0);
    }
    else if(!acq_psr && rel_psr)
    {
        psr_release_display(psr_data, PSR_FB_REF);
    }

    if(zxfb)
    {
        trace_zxgfx_crtc_flip(zx_card->index << 16 | arg->crtc, arg->stream_type, zxfb->obj);
        input_stream.SurfaceAttrib.StartAddr = zxfb->gpu_virt_addr;
        input_stream.SurfaceAttrib.SurfaceSize = (zxfb->base.width) | (zxfb->base.height << 16);
#if DRM_VERSION_CODE < KERNEL_VERSION(4, 11, 0)
        input_stream.SurfaceAttrib.SurfaceFmt = DrmFormat2CBiosFormat(zxfb->base.pixel_format);
#else
        input_stream.SurfaceAttrib.SurfaceFmt = DrmFormat2CBiosFormat(zxfb->base.format->format);
#endif
        input_stream.SurfaceAttrib.Pitch = zxfb->base.pitches[0];
        input_stream.SurfaceAttrib.bCompress = (zxfb->obj->info.compress_format != 0);
        if(input_stream.SurfaceAttrib.bCompress)
        {
            input_stream.SurfaceAttrib.BLIndex = zxfb->obj->info.bl_slot_index;
            input_stream.SurfaceAttrib.Range_Type = zxfb->obj->info.compress_format;
        }

        input_stream.SrcWindow.Position = arg->src_x | (arg->src_y << 16);
        input_stream.SrcWindow.WinSize = arg->src_w | (arg->src_h << 16);

        input_stream.DispWindow.Position = arg->crtc_x | (arg->crtc_y << 16);
        input_stream.DispWindow.WinSize = arg->crtc_w | (arg->crtc_h << 16);

        if(disp_plane.PlaneIndex == CBIOS_STREAM_PS)
        {
            disp_plane.pOverlayInfo = &overlay;
            overlay.KeyMode = CBIOS_WINDOW_KEY;
            overlay.WindowKey.Ka = 0;
            overlay.WindowKey.Kb = 8;
        }
#if  DRM_VERSION_CODE >= KERNEL_VERSION(4, 8, 0)
        else
        {
            disp_plane.pOverlayInfo = &overlay;
            if(arg->blend_mode == DRM_MODE_BLEND_PIXEL_NONE)
            {
                overlay.KeyMode = CBIOS_CONSTANT_ALPHA;
                overlay.ConstantAlphaBlending.ConstantAlpha = (CBIOS_U8)(arg->const_alpha >> 8);
            }
            else if(arg->blend_mode == DRM_MODE_BLEND_COVERAGE)  //coverage with plane alpha
            {
                overlay.KeyMode = CBIOS_ALPHA_BLENDING;
                overlay.AlphaBlending.bUseAAlpha = 0;
                overlay.AlphaBlending.bUsePlaneAlpha = 1;
                overlay.AlphaBlending.PlaneValue = (CBIOS_U8)(arg->plane_alpha >> 8);
            }
            else  //premultied with plane alpha
            {
                overlay.KeyMode = CBIOS_ALPHA_BLENDING;
                overlay.AlphaBlending.bUseAAlpha = 0;
                overlay.AlphaBlending.bPremulBlend = 1;
                overlay.AlphaBlending.bUsePlaneAlpha = 1;
                overlay.AlphaBlending.PlaneValue = (CBIOS_U8)(arg->plane_alpha >> 8);
            }
        }
#endif

        //NOTE: disable async flip for chx004
        if(arg->async_flip)
        {
            disp_plane.FlipMode.FlipImme = 0;
        }

    }

    return (CBiosUpdateFrame(disp_info->cbios_ext, &update_frame) == CBIOS_OK) ? DISP_OK : DISP_FAIL;
}


int disp_cbios_update_cursor(disp_info_t *disp_info, zx_cursor_update_t *arg)
{
    CBIOS_CURSOR_PARA  update_cursor = {0};

    //update_cursor.bVsyncOn = arg->vsync_on;
    if (arg->fb)
    {
        update_cursor.bDisable = 0;
        if (arg->width == 64 && arg->height == 64)
        {
            update_cursor.Position.CursorSize = CBIOS_CURSOR_SIZE_64x64;
        }
        else if (arg->width == 128 && arg->height == 128)
        {
            update_cursor.Position.CursorSize = CBIOS_CURSOR_SIZE_128x128;
        }
        update_cursor.CursorAttrib.Type = CBIOS_PREMULT_CURSOR;
        update_cursor.CursorAttrib.CurAddr = to_zxfb(arg->fb)->gpu_virt_addr;
        update_cursor.Position.PositionX = arg->pos_x;
        update_cursor.Position.PositionY = arg->pos_y;
    }
    else
    {
        update_cursor.bDisable = 1;
    }

    update_cursor.Size = sizeof(update_cursor);
    update_cursor.IGAIndex = arg->crtc;

    return  (CBiosSetCursor(disp_info->cbios_ext, &update_cursor) == CBIOS_OK)? DISP_OK : DISP_FAIL;
}

int   disp_cbios_get_dpint_type(disp_info_t *disp_info,unsigned int device)
{
    CBIOS_DP_INT_PARA DpIntPara = {0};
    int hpd = DP_HPD_NONE;

    DpIntPara.Size = sizeof(CBIOS_DP_INT_PARA);
    DpIntPara.DeviceId = device;
    if(CBIOS_OK == CBiosGetDPIntType(disp_info->cbios_ext, &DpIntPara))
    {
        hpd = DpIntPara.IntType;
    }

    return hpd;
}

int  disp_cbios_handle_dp_irq(disp_info_t *disp_info, unsigned int device, int int_type, int* detect, int* dp_port, int* reset_psr)
{
    CBIOS_DP_HANDLE_IRQ_PARA  handle_irq = {0};
    int  status = DISP_FAIL;

    handle_irq.Size = sizeof(CBIOS_DP_HANDLE_IRQ_PARA);
    handle_irq.DeviceId = device;
    handle_irq.IntType = int_type;
    if(CBIOS_OK == CBiosHandleDPIrq(disp_info->cbios_ext, &handle_irq))
    {
        status = DISP_OK;
    }

    if(detect)
    {
        *detect = (status == DISP_OK)? handle_irq.bNeedDetect : 0;
    }

    if(dp_port)
    {
        *dp_port = (status == DISP_OK)? handle_irq.bDpHpd : 0;
    }

    if(reset_psr)
    {
        *reset_psr = (status == DISP_OK)? handle_irq.bNeedResetPsr : 0;
    }


    return status;
}


void disp_cbios_dump_registers(disp_info_t *disp_info, int type)
{
    CBIOS_DUMP_PARA para = {0};

    para.Size = sizeof(CBIOS_DUMP_PARA);

    para.DumpType = CBIOS_DUMP_VCP_INFO  |
                    CBIOS_DUMP_MODE_INFO |
                    CBIOS_DUMP_CLOCK_INFO;

    if(type &  DUMP_REGISTER_STREAM)
    {
        para.DumpType |= CBIOS_DUMP_PS1_REG |
                         CBIOS_DUMP_PS2_REG |
                         CBIOS_DUMP_SS1_REG |
                         CBIOS_DUMP_SS2_REG |
                         CBIOS_DUMP_TS1_REG |
                         CBIOS_DUMP_TS2_REG;
    }

    CBiosDumpInfo(disp_info->cbios_ext, &para);
}

int disp_cbios_set_hda_codec(disp_info_t *disp_info, zx_connector_t*  zx_connector)
{
    void                        *pcbe = disp_info->cbios_ext;
    CBIOS_HDAC_PARA             CbiosHDACPara = {0};
    int                         cb_status = CBIOS_OK;

    if((zx_connector->monitor_type == UT_OUTPUT_TYPE_HDMI) ||
       (zx_connector->monitor_type == UT_OUTPUT_TYPE_MHL) ||
       (zx_connector->monitor_type == UT_OUTPUT_TYPE_DP))
    {
        CbiosHDACPara.Size      = sizeof(CBIOS_HDAC_PARA);
        CbiosHDACPara.DeviceId = zx_connector->output_type;

        cb_status = CBiosSetHDACodecPara(pcbe, &CbiosHDACPara);
    }
    return (cb_status == CBIOS_OK) ?  DISP_OK : DISP_FAIL;
}

int disp_cbios_get_hdmi_audio_format(disp_info_t *disp_info, unsigned int device_id, zx_hdmi_audio_formats *audio_formats)
{
    void                          *pcbe            = disp_info->cbios_ext;

    unsigned int                  buffer_size      = 0;
    unsigned int                  real_buffer_size = 0;
    unsigned int                  num_formats      = 0;
    unsigned int                  i                = 0;
    CBiosHDMIAudioFormat         *cb_audio_formats = NULL;
    CBIOS_STATUS                  cb_status        = CBIOS_OK;

    cb_status = CBiosGetHDMIAudioFomatListBufferSize(pcbe, device_id, &buffer_size);
    if ((cb_status != CBIOS_OK) || (buffer_size == 0))
    {
        zx_error("can not get the audio format buffer size\n");
        return DISP_FAIL;
    }

    cb_audio_formats = zx_calloc(buffer_size);
    if (cb_audio_formats == NULL)
    {
        return DISP_FAIL;
    }

    num_formats = buffer_size / sizeof(CBiosHDMIAudioFormat);
    for(i = 0; i < num_formats; i++)
    {
       cb_audio_formats[i].Size = sizeof(CBiosHDMIAudioFormat);
    }

    cb_status = CBiosGetHDMIAudioFomatList(pcbe, device_id, cb_audio_formats, &real_buffer_size);

    if ((cb_status != CBIOS_OK) || (real_buffer_size == 0))
    {
        zx_error("can not get the audio format\n");
        zx_free(cb_audio_formats);
        return DISP_FAIL;
    }

    if (num_formats > sizeof(audio_formats->audio_formats)/sizeof(zx_hdmi_audio_format_t))
    {
        zx_error("the num of audio formats exceeds the buffer size, so cut it.");
        num_formats = sizeof(audio_formats->audio_formats)/sizeof(zx_hdmi_audio_format_t);
    }

    audio_formats->num_formats = num_formats;
    for (i = 0; i < num_formats; i++)
    {
        audio_formats->audio_formats[i].format = (ZX_HDMI_AUDIO_FORMAT_TYPE)cb_audio_formats[i].Format;
        audio_formats->audio_formats[i].max_channel_num= cb_audio_formats[i].MaxChannelNum;
        audio_formats->audio_formats[i].sample_rate_unit = cb_audio_formats[i].SampleRateUnit;
        audio_formats->audio_formats[i].unit = cb_audio_formats[i].Unit;
    }

    if (cb_audio_formats)
    {
        zx_free(cb_audio_formats);
        cb_audio_formats = NULL;
    }
    return DISP_OK;
}

void disp_cbios_reset_hw_block(disp_info_t *disp_info, zx_hw_block hw_block)
{
    CBIOS_HW_BLOCK  cbios_hw_block = {0};

    if(hw_block == ZX_HW_IGA)
    {
        cbios_hw_block = CBIOS_HW_IGA;
    }
    else
    {
        cbios_hw_block = CBIOS_HW_NONE;
    }

    CBiosResetHWBlock(disp_info->cbios_ext, cbios_hw_block);
}

int disp_cbios_get_counter(disp_info_t* disp_info, zx_get_counter_t* get_counter)
{
    CBIOS_GET_HW_COUNTER   CbiosGetHwCnt = {0};
    int  status = DISP_FAIL;

    if(!get_counter || get_counter->crtc_index >= disp_info->num_crtc)
    {
        return  status;
    }

    CbiosGetHwCnt.IgaIndex = get_counter->crtc_index;

    if(get_counter->hpos)
    {
        CbiosGetHwCnt.bGetPixelCnt = 1;
    }

    if(get_counter->vpos)
    {
        CbiosGetHwCnt.bGetLineCnt = 1;
    }

    if(get_counter->vblk)
    {
        CbiosGetHwCnt.bGetFrameCnt = 1;
    }

    if(CBIOS_OK == CBiosGetHwCounter(disp_info->cbios_ext, &CbiosGetHwCnt))
    {
        status = DISP_OK;
    }

    if(get_counter->hpos)
    {
        *get_counter->hpos = (status == DISP_OK)? CbiosGetHwCnt.Value[CBIOS_COUNTER_PIXEL] : 0;
    }
    if(get_counter->vpos)
    {
        *get_counter->vpos = (status == DISP_OK)? CbiosGetHwCnt.Value[CBIOS_COUNTER_LINE] : 0;
    }
    if(get_counter->vblk)
    {
        *get_counter->vblk = (status == DISP_OK)? CbiosGetHwCnt.Value[CBIOS_COUNTER_FRAME] : 0;
    }
    if(get_counter->in_vblk)
    {
        *get_counter->in_vblk = (status == DISP_OK && CbiosGetHwCnt.bInVblank)? 1 : 0;
    }

    return status;
}

int disp_get_vip_capture_fmt(unsigned int fmt)
{
    unsigned int capture_fmt = ZX_VIP_FMT_RGB444_24BIT_SDR;

    switch (fmt)
    {
    case CBIOS_VIP_FMT_RGB444_24BIT_SDR:
    {
        capture_fmt = ZX_VIP_FMT_RGB444_24BIT_SDR;
    }
    break;

    case CBIOS_VIP_FMT_YCBCR444_24BIT_SDR:
    {
        capture_fmt = ZX_VIP_FMT_YCBCR444_24BIT_SDR;
    }
    break;

    case CBIOS_VIP_FMT_YCBCR422_8BIT_SDR_ES:
    {
        capture_fmt = ZX_VIP_FMT_YCBCR422_8BIT_SDR_ES;
    }
    break;

    case CBIOS_VIP_FMT_YCBCR422_8BIT_DDR_ES:
    {
        capture_fmt = ZX_VIP_FMT_YCBCR422_8BIT_DDR_ES;
    }
    break;


    default:
    {
        zx_error("not supported yet, default to RGB444\n");
    }
    break;

    }

    return capture_fmt;

}

int disp_get_wb_capture_fmt(unsigned int fmt)
{
    unsigned int capture_fmt = ZX_WB_FMT_RGB888;

    switch (fmt)
    {
    case CBIOS_RGBOUTPUT:
    {
        capture_fmt = ZX_WB_FMT_RGB888;
    }
    break;

    default:
    {
        capture_fmt = ZX_WB_FMT_RGB888;
        zx_error("not supported fmt %d, default to RGB\n", fmt);
    }
    break;

    }

    return capture_fmt;

}

int disp_cbios_vip_ctl(disp_info_t *disp_info, zx_vip_set_t *v_set)
{
    void *pcbe = disp_info->cbios_ext;
    CBIOS_VIP_CTRL_DATA  cbios_vip_params = {0};
    int cb_status = CBIOS_OK;
    int i = 0;

    cbios_vip_params.vip = v_set->vip;
    cbios_vip_params.cmd = CBIOS_VIP_NOP;

    switch (v_set->op)
    {
    case ZX_VIP_ENABLE:
    case ZX_VIP_DISABLE:
    {
        cbios_vip_params.cmd = CBIOS_VIP_ENABLE;
        cbios_vip_params.enable = v_set->op == ZX_VIP_ENABLE ? 1 : 0;
    }
    break;

    case ZX_VIP_QUERY_CAPS:
    {
        cbios_vip_params.cmd = CBIOS_VIP_QUERY_CAPS;
    }
    break;

    case ZX_VIP_SET_MODE:
    {
        cbios_vip_params.cmd = CBIOS_VIP_SET_MODE;
        cbios_vip_params.modeSet.fmt = CBIOS_VIP_FMT_RGB444_24BIT_SDR;
        cbios_vip_params.modeSet.vCard = v_set->mode.chip;
        cbios_vip_params.modeSet.xRes  = v_set->mode.x_res;
        cbios_vip_params.modeSet.yRes  = v_set->mode.y_res;
        cbios_vip_params.modeSet.refs  = v_set->mode.refs;
    }
    break;

    case ZX_VIP_SET_BUFFER:
    {
        cbios_vip_params.cmd = CBIOS_VIP_SET_BUFFER;
        cbios_vip_params.fbSet.num  = v_set->fb.fb_num;
        cbios_vip_params.fbSet.idx  = v_set->fb.fb_idx;
        cbios_vip_params.fbSet.addr = v_set->fb.fb_addr;
    }
    break;

    default:
    {
        zx_error("unspported vip set cmd: %d", v_set->op);
    }
    break;

    }

    if (cbios_vip_params.cmd != CBIOS_VIP_NOP)
    {
        cb_status = CBiosVIPCtl(pcbe, &cbios_vip_params);
    }

    if (cbios_vip_params.cmd == CBIOS_VIP_QUERY_CAPS && cb_status == CBIOS_OK)
    {
        v_set->caps.mode_num = cbios_vip_params.caps.supportModeNum;
        if (v_set->caps.mode != NULL)
        {
            for (; i < v_set->caps.mode_num; i++)
            {
                v_set->caps.mode[i].xRes = cbios_vip_params.caps.mode[i].xRes;
                v_set->caps.mode[i].yRes = cbios_vip_params.caps.mode[i].yRes;
                v_set->caps.mode[i].refreshrate = cbios_vip_params.caps.mode[i].refs;
                v_set->caps.mode[i].fmt = disp_get_vip_capture_fmt(cbios_vip_params.caps.mode[i].fmt);
            }
        }
    }

    return (cb_status == CBIOS_OK) ?  DISP_OK : DISP_FAIL;
}

int disp_cbios_wb_ctl(disp_info_t *disp_info, zx_wb_set_t *wb_set)
{
    void *pcbe = disp_info->cbios_ext;
    CBIOS_WB_PARA cbios_wb_params = {0};
    CBiosSettingModeParams cbios_mode = {0};
    int cb_status = CBIOS_OK;

    if (wb_set->op_flags & ZX_WB_QUERY_CAPS)
    {
        cbios_mode.IGAIndex = wb_set->iga_idx;
        CBiosGetModeFromIGA(pcbe, &cbios_mode);

        wb_set->input_mode.src_x = cbios_mode.DestModeParams.XRes;
        wb_set->input_mode.src_y = cbios_mode.DestModeParams.YRes;
        wb_set->input_mode.refreshrate = cbios_mode.DestModeParams.RefreshRate;
        wb_set->input_mode.capture_fmt = disp_get_wb_capture_fmt(cbios_mode.DestModeParams.OutputSignal);

        return cb_status;
    }

    cbios_wb_params.IGAIndex = wb_set->iga_idx;

    if (wb_set->op_flags & ZX_WB_SET_BYPASS_MODE)
    {
        cbios_wb_params.bByPass   = 1;
    }

    if (wb_set->op_flags & ZX_WB_SET_VSYNC_OFF)
    {
        cbios_wb_params.bUpdateImme   = 1;
    }

    if ((wb_set->op_flags & ZX_WB_ENABLE) || (wb_set->op_flags & ZX_WB_DISABLE))
    {
        cbios_wb_params.bEnableOP = 1;
        cbios_wb_params.bEnable = (wb_set->op_flags & ZX_WB_ENABLE) ? 1 : 0;
    }

    if (wb_set->op_flags & ZX_WB_SET_MODE)
    {
        cbios_wb_params.bSetModeOP = 1;
        cbios_wb_params.SrcSize = (wb_set->mode.src_x & 0xFFFF) | ((wb_set->mode.src_y & 0xFFFF) << 16);
        //FIXME: to refine the fmt.
        cbios_wb_params.SrcFmt = CSC_FMT_RGB;
        cbios_wb_params.CscOutFmt = CSC_FMT_RGB;

        if (wb_set->op_flags & ZX_WB_SET_DOWNSCALER_MODE)
        {
            cbios_wb_params.DSCL.Mode = CBIOS_WB_P2P;
            cbios_wb_params.DSCL.DstSize = (wb_set->mode.dst_x & 0xFFFF) | ((wb_set->mode.dst_x & 0xFFFF) << 16);
            cbios_wb_params.DSCL.bDoubleBuffer = wb_set->mode.double_buf;
        }
    }

    if (wb_set->op_flags & ZX_WB_SET_BUFFER)
    {
        cbios_wb_params.bSetAddrOP = 1;
        cbios_wb_params.DstBaseAddr = wb_set->fb.fb_addr;
    }

    cb_status = CBiosSetWriteback(pcbe, &cbios_wb_params);

    return (cb_status == CBIOS_OK) ?  DISP_OK : DISP_FAIL;
}

int disp_wait_idle(void *_disp_info)
{
    disp_info_t *disp_info = (disp_info_t *)_disp_info;
    void *pcbe = NULL;
    int ret = DISP_OK;
    unsigned int i = 0;

    if (!disp_info)
    {
        zx_info("disp_info is null! \n");
        return DISP_FAIL;
    }

    pcbe = disp_info->cbios_ext;

    i = 0;
    while (i < MAX_CORE_CRTCS)
    {
        if (disp_info->active_output[i])
        {
            break;
        }
        i++;
    }

    //case1): has one active crtc, wait the crtc's vblank
    //case2): no active crtc,  no need to wait.
    if (i != MAX_CORE_CRTCS)  //has active crtc
    {

        ret = (CBIOS_OK == CBiosWaitVBlank(pcbe, i))? DISP_OK : DISP_FAIL;

    }

    return ret;
}

void  disp_cbios_brightness_set(disp_info_t* disp_info, unsigned int brightness)
{
    void                        *pcbe = disp_info->cbios_ext;
    int  status = CBIOS_OK;
    int  bEDPLighted = 0, device_id = 0;
    CBIOS_BACKLIGHT_PARA  blPara = {0};
    CBiosMonitorAttribute  monitorAttr = {0};
    zx_card_t*  zx_card = disp_info->zx_card;
    struct drm_device*  drm = zx_card->drm_dev;
    struct drm_connector* connector = NULL;
    zx_connector_t*  zx_connector = NULL;

    list_for_each_entry(connector, &drm->mode_config.connector_list, head)
    {
        zx_connector = to_zx_connector(connector);
        if(zx_connector->monitor_type == UT_OUTPUT_TYPE_PANEL)
        {
            bEDPLighted = 1;
            device_id = zx_connector->output_type;
            break;
        }
    }

//till now, only edp support backlight adjust
    if(bEDPLighted)
    {
        monitorAttr.Size = sizeof(monitorAttr);
        monitorAttr.ActiveDevId = device_id;

        CBiosQueryMonitorAttribute(pcbe, &monitorAttr);

        if(monitorAttr.bSupportBLCtrl)
        {
            blPara.Size = sizeof(CBIOS_BACKLIGHT_PARA);
            blPara.BacklightValue = brightness;
            blPara.DeviceId = device_id;

            status = CBiosSetBacklight(pcbe, &blPara);
        }
    }
}

unsigned int  disp_cbios_brightness_get(disp_info_t* disp_info)
{
    void *pcbe = disp_info->cbios_ext;
    int  status = CBIOS_OK;
    int  bEDPLighted = 0, device_id = 0;
    CBIOS_BACKLIGHT_PARA  blPara = {0};
    unsigned int brightness = 0;
    zx_card_t*  zx_card = disp_info->zx_card;
    struct drm_device*  drm = zx_card->drm_dev;
    struct drm_connector* connector = NULL;
    zx_connector_t*  zx_connector = NULL;

    list_for_each_entry(connector, &drm->mode_config.connector_list, head)
    {
        zx_connector = to_zx_connector(connector);
        if(zx_connector->monitor_type == UT_OUTPUT_TYPE_PANEL)
        {
            bEDPLighted = 1;
            device_id = zx_connector->output_type;
            break;
        }
    }

    if(bEDPLighted)
    {
        blPara.Size = sizeof(CBIOS_DSI_BACKLIGHT_PARA);
        blPara.DeviceId = device_id;

        status = CBiosGetBacklight(pcbe, &blPara);

        brightness = blPara.BacklightValue;
    }

    return brightness;
}

void  disp_cbios_query_brightness_caps(disp_info_t* disp_info, zx_brightness_caps_t *brightness_caps)
{
    void *pcbe = disp_info->cbios_ext;
    int  status = CBIOS_OK;
    int  bEDPLighted = 0, device_id = 0;
    CBiosMonitorAttribute  monitorAttr = {0};
    zx_card_t*  zx_card = disp_info->zx_card;
    struct drm_device*  drm = zx_card->drm_dev;
    struct drm_connector* connector = NULL;
    zx_connector_t*  zx_connector = NULL;

    list_for_each_entry(connector, &drm->mode_config.connector_list, head)
    {
        zx_connector = to_zx_connector(connector);

        if(zx_connector->monitor_type == UT_OUTPUT_TYPE_PANEL)
        {
            bEDPLighted = 1;
            device_id = zx_connector->output_type;
            break;
        }
    }

    if (bEDPLighted)
    {
        monitorAttr.Size = sizeof(monitorAttr);
        monitorAttr.ActiveDevId = device_id;

        status = CBiosQueryMonitorAttribute(pcbe, &monitorAttr);

        brightness_caps->support_brightness_ctrl = monitorAttr.bSupportBLCtrl;
        brightness_caps->max_brightness_value = monitorAttr.MaxBLLevel;
        brightness_caps->min_brightness_value = monitorAttr.MinBLLevel;
    }
}

int disp_cbios_psrop0_1(psr_data_t *psr_data)
{
    disp_info_t* disp_info = (disp_info_t*)psr_data->disp_info;
    void *pcbe = disp_info->cbios_ext;

    return (CBIOS_TRUE == CBiosPsrOp0_1(pcbe, psr_data->output_type))? DISP_OK : DISP_FAIL;
}

int disp_cbios_psrop1_0(psr_data_t *psr_data)
{
    disp_info_t* disp_info = (disp_info_t*)psr_data->disp_info;
    void *pcbe = disp_info->cbios_ext;

    CBiosPsrOp1_0(pcbe, psr_data->output_type);

    return DISP_OK;
}

int disp_cbios_psrop1_2(psr_data_t *psr_data)
{
    disp_info_t* disp_info = (disp_info_t*)psr_data->disp_info;
    void *pcbe = disp_info->cbios_ext;

    CBiosPsrOp1_2(pcbe, psr_data->output_type);

    return DISP_OK;
}

int disp_cbios_psrop2_0(psr_data_t *psr_data)
{
    disp_info_t* disp_info = (disp_info_t*)psr_data->disp_info;
    void *pcbe = disp_info->cbios_ext;

    CBiosPsrOp2_0(pcbe, psr_data->output_type);

    return DISP_OK;
}

int disp_cbios_psrop2_3(psr_data_t *psr_data)
{
    disp_info_t* disp_info = (disp_info_t*)psr_data->disp_info;
    void *pcbe = disp_info->cbios_ext;

    return (CBIOS_TRUE == CBiosPsrOp2_3(pcbe, psr_data->output_type))? DISP_OK : DISP_FAIL;
}

int disp_cbios_psrop2_5(psr_data_t *psr_data)
{
    disp_info_t* disp_info = (disp_info_t*)psr_data->disp_info;
    void *pcbe = disp_info->cbios_ext;

    CBiosPsrOp2_5(pcbe, psr_data->output_type);

    return DISP_OK;
}

int disp_cbios_psrop3_0(psr_data_t *psr_data)
{
    disp_info_t* disp_info = (disp_info_t*)psr_data->disp_info;
    void *pcbe = disp_info->cbios_ext;

    CBiosPsrOp3_0(pcbe, psr_data->output_type);

    return DISP_OK;
}

int disp_cbios_psrop3_1(psr_data_t *psr_data)
{
    disp_info_t* disp_info = (disp_info_t*)psr_data->disp_info;
    void *pcbe = disp_info->cbios_ext;

    CBiosPsrOp3_1(pcbe, psr_data->output_type);

    return DISP_OK;
}

int disp_cbios_psrop3_5(psr_data_t *psr_data)
{
    disp_info_t* disp_info = (disp_info_t*)psr_data->disp_info;
    void *pcbe = disp_info->cbios_ext;

    CBiosPsrOp3_5(pcbe, psr_data->output_type);

    return DISP_OK;
}

int disp_cbios_psrop5_0(psr_data_t *psr_data)
{
    disp_info_t* disp_info = (disp_info_t*)psr_data->disp_info;
    void *pcbe = disp_info->cbios_ext;

    CBiosPsrOp5_0(pcbe, psr_data->output_type);

    return DISP_OK;
}

int disp_cbios_psrop5_1(psr_data_t *psr_data)
{
    disp_info_t* disp_info = (disp_info_t*)psr_data->disp_info;
    void *pcbe = disp_info->cbios_ext;

    return (CBIOS_TRUE == CBiosPsrOp5_1(pcbe, psr_data->output_type))? DISP_OK : DISP_FAIL;
}

int disp_cbios_psrop3_3_1(psr_data_t *psr_data)
{
    disp_info_t* disp_info = (disp_info_t*)psr_data->disp_info;
    void *pcbe = disp_info->cbios_ext;

    CBiosPsrOp3_3_1(pcbe, psr_data->output_type);

    return DISP_OK;
}

int disp_cbios_psrop3_1_3_2(psr_data_t *psr_data)
{
    disp_info_t* disp_info = (disp_info_t*)psr_data->disp_info;
    void *pcbe = disp_info->cbios_ext;

    CBiosPsrOp3_1_3_2(pcbe, psr_data->output_type);

    return DISP_OK;
}

int disp_cbios_psrop3_2_3_3(psr_data_t *psr_data)
{
    disp_info_t* disp_info = (disp_info_t*)psr_data->disp_info;
    void *pcbe = disp_info->cbios_ext;

    CBiosPsrOp3_2_3_3(pcbe, psr_data->output_type);

    return DISP_OK;
}

int disp_cbios_psrop3_3_3_2(psr_data_t *psr_data)
{
    disp_info_t* disp_info = (disp_info_t*)psr_data->disp_info;
    void *pcbe = disp_info->cbios_ext;

    CBiosPsrOp3_3_3_2(pcbe, psr_data->output_type);

    return DISP_OK;
}

int disp_cbios_psrop3_2_3_1(psr_data_t *psr_data)
{
    disp_info_t* disp_info = (disp_info_t*)psr_data->disp_info;
    void *pcbe = disp_info->cbios_ext;

    CBiosPsrOp3_2_3_1(pcbe, psr_data->output_type);

    return DISP_OK;
}

int disp_cbios_psrop3_1_3(psr_data_t *psr_data)
{
    disp_info_t* disp_info = (disp_info_t*)psr_data->disp_info;
    void *pcbe = disp_info->cbios_ext;

    CBiosPsrOp3_1_3(pcbe, psr_data->output_type);

    return DISP_OK;
}

int psr_acquire_display(void *data, unsigned int ref_type, int wait_done)
{
    psr_data_t *psr_data = (psr_data_t *)data;
    unsigned int request = 0;
    unsigned long flags = 0;
    int psr_op = PSR_OP_NONE;

    if(!psr_data->enabled || ref_type >= PSR_MAX_REF)
    {
        return 0;
    }

    if(ref_type == PSR_VBLANK_REF)
    {
        psr_op = PSR_OP_TRANS_TO_LEQ_IGA_OFF;
    }
    else
    {
        psr_op = PSR_OP_TRANS_TO_INACTIVE;
    }

    flags = zx_spin_lock_irqsave(psr_data->psr_ref_lock);

    if(!psr_data->ref_count)
    {
        request = 1;
    }
    else if(psr_data->ref_count == (1 << PSR_VBLANK_REF) 
                && psr_op != PSR_OP_TRANS_TO_LEQ_IGA_OFF)
    {
        //previous acquire is vblank ref, psr would be requested to IGA_OFF state, current acquire is refs which require psr goto inactive state, need request again
        request = 1;
    }

    psr_data->ref_count |= (1 << ref_type);

    zx_spin_unlock_irqrestore(psr_data->psr_ref_lock, flags);

    if(request)
    {
#if DRM_VERSION_CODE >= KERNEL_VERSION(6, 15, 0)
        timer_delete_sync(&psr_data->psr_timer);
#else
        del_timer_sync(&psr_data->psr_timer);
#endif

        atomic_or(psr_op, &psr_data->operation);

        if(!wait_done)
        {
            zx_thread_wake_up(psr_data->psr_event);
        }
        else
        {
            psr_operate_trans_state(psr_data);
        }
    }

    return 1;
}

void psr_release_display(void *data, unsigned int ref_type)
{
    psr_data_t *psr_data = (psr_data_t *)data;
    unsigned int request = 0;
    unsigned long flags = 0;

    if(!psr_data->enabled || ref_type >= PSR_MAX_REF)
    {
        return;
    }

    flags = zx_spin_lock_irqsave(psr_data->psr_ref_lock);

    if(psr_data->ref_count == (1 << ref_type))
    {
        request = 1;
    }

    psr_data->ref_count &= ~(1 << ref_type);

    zx_spin_unlock_irqrestore(psr_data->psr_ref_lock, flags);

    if(request)
    {
        mod_timer(&psr_data->psr_timer, jiffies+HZ);
    }
}

static void psr_op_enable(psr_data_t *psr_data)
{
    int cnt = 0, error = 0;

    if(psr_data->current_state != PSR_STATE_0_DISABLED)
    {
        return;
    }
    
    while(cnt < 1000)
    {
        error = disp_cbios_psrop0_1(psr_data);
        if (DISP_OK == error)
        {
            psr_data->current_state = PSR_STATE_1_INACTIVE;
            psr_data->enabled = 1;
            break;
        }
        else
        {
            cnt++;
            continue;
        }
    }
}

static void psr_op_disable(psr_data_t *psr_data)
{
    psr_data->enabled = 0;
    
    if(psr_data->current_state == PSR_STATE_0_DISABLED)
    {
        return;
    }
    
    if(psr_data->current_state == PSR_STATE_3_3_DCLK_OFF)
    {
        disp_cbios_psrop3_3_3_2(psr_data);
        psr_data->current_state = PSR_STATE_3_2_IGA_OFF;
    }

    if(psr_data->current_state == PSR_STATE_3_2_IGA_OFF)
    {
        disp_cbios_psrop3_2_3_1(psr_data);
        psr_data->current_state = PSR_STATE_3_1_EPHY_OFF;
    }

    if(psr_data->current_state == PSR_STATE_3_1_EPHY_OFF)
    {
        disp_cbios_psrop3_1_3(psr_data);
        psr_data->current_state = PSR_STATE_3_ACTIVE;
    }

    if(psr_data->current_state == PSR_STATE_3_ACTIVE)
    {
        disp_cbios_psrop3_0(psr_data);
        psr_data->current_state = PSR_STATE_0_DISABLED;
    }
    else if(psr_data->current_state == PSR_STATE_2_TRANSITION)
    {
        disp_cbios_psrop2_0(psr_data);
        psr_data->current_state = PSR_STATE_0_DISABLED;
    }
    else if(psr_data->current_state == PSR_STATE_1_INACTIVE)
    {
        disp_cbios_psrop1_0(psr_data);
        psr_data->current_state = PSR_STATE_0_DISABLED;
    }
    else if(psr_data->current_state == PSR_STATE_2_EXIT)
    {
        disp_cbios_psrop5_0(psr_data);
        psr_data->current_state = PSR_STATE_0_DISABLED;
    }
}

void  psr_enable(void* data, unsigned int device)
{
    psr_data_t *psr_data = (psr_data_t *)data;
    int acquired = 0;

    if (psr_data && !psr_data->forceoff && !psr_data->enabled)
    {
        zx_mutex_lock(psr_data->psr_mutex);

        psr_data->output_type = device;

        psr_data->active_fail_num = 0;

        psr_op_enable(psr_data);

        zx_mutex_unlock(psr_data->psr_mutex);

        zx_info("PSR is enabled on device 0x%x!\n", device);

        acquired = psr_acquire_display(psr_data, PSR_ENABLE_REF, 0);
        if(acquired)
        {
            psr_release_display(psr_data, PSR_ENABLE_REF);
        }
    }
}

void psr_disable(void *data)
{
    psr_data_t *psr_data = (psr_data_t *)data;

    if(psr_data && psr_data->enabled)
    {
        zx_mutex_lock(psr_data->psr_mutex);
        
        psr_op_disable(psr_data);
        
        zx_mutex_unlock(psr_data->psr_mutex);

        zx_info("PSR is disabled on device 0x%x!\n", psr_data->output_type);
    }
}

void psr_reset(void *data)
{
    psr_data_t *psr_data = (psr_data_t *)data;

    if(psr_data)
    {
        zx_mutex_lock(psr_data->psr_mutex);

        if(psr_data->enabled)
        {
            psr_op_disable(psr_data);
        }

        psr_op_enable(psr_data);

        zx_mutex_unlock(psr_data->psr_mutex);

        zx_info("PSR is reset!\n");
    }
}

static PSR_OPERATION_RESULT_T psr_op_single_step(psr_data_t* psr_data, int to_deep)
{
    PSR_OPERATION_RESULT_T status = PSR_OP_OK;
    int cnt = 0;

    if (to_deep)
    {
        if (psr_data->current_state == PSR_STATE_1_INACTIVE)
        {
            disp_cbios_psrop1_2(psr_data);
            psr_data->current_state = PSR_STATE_2_TRANSITION;
        }
        else if (psr_data->current_state == PSR_STATE_2_TRANSITION)
        {
            cnt = 0;
            while (cnt < 40)
            {
                if (DISP_OK == disp_cbios_psrop2_3(psr_data))
                {
                    if (psr_data->active_fail_num > 0)
                    {
                        psr_data->active_fail_num--;
                    }
                    psr_data->current_state = PSR_STATE_3_ACTIVE;
                    break;
                }
                else
                {
                    cnt++;
                    continue;
                }
            }

            if (cnt >= 40)
            {
                psr_data->active_fail_num++;

                if (psr_data->active_fail_num >= PSR_ACTIVE_FAIL_THRESHOLD)
                {
                    status = PSR_OP_TO_ACTIVE_CRITICAL;
                }
                else
                {
                    zx_info("PSR trans to STATE_3_ACTIVE failed, to do reset.\n");
                    status = PSR_OP_TO_ACTIVE_FAIL;
                }
            }
        }
        else if(psr_data->current_state == PSR_STATE_3_ACTIVE)
        {
            disp_cbios_psrop3_3_1(psr_data);
            psr_data->current_state = PSR_STATE_3_1_EPHY_OFF;
        }
        else if(psr_data->current_state == PSR_STATE_3_1_EPHY_OFF)
        {
            disp_cbios_psrop3_1_3_2(psr_data);
            psr_data->current_state = PSR_STATE_3_2_IGA_OFF;
        }
        else if(psr_data->current_state == PSR_STATE_3_2_IGA_OFF)
        {
            disp_cbios_psrop3_2_3_3(psr_data);
            psr_data->current_state = PSR_STATE_3_3_DCLK_OFF;
        }
    }
    else
    {
        if(psr_data->current_state == PSR_STATE_3_3_DCLK_OFF)
        {
            disp_cbios_psrop3_3_3_2(psr_data);
            psr_data->current_state = PSR_STATE_3_2_IGA_OFF;
        }
        else if(psr_data->current_state == PSR_STATE_3_2_IGA_OFF)
        {
            disp_cbios_psrop3_2_3_1(psr_data);
            psr_data->current_state = PSR_STATE_3_1_EPHY_OFF;
        }
        else if(psr_data->current_state == PSR_STATE_3_1_EPHY_OFF)
        {
            disp_cbios_psrop3_1_3(psr_data);
            psr_data->current_state = PSR_STATE_3_ACTIVE;
        }
        else if(psr_data->current_state == PSR_STATE_3_ACTIVE)
        {
            disp_cbios_psrop3_5(psr_data);
            psr_data->current_state = PSR_STATE_2_EXIT;
        }
        else if(psr_data->current_state == PSR_STATE_2_TRANSITION)
        {
            disp_cbios_psrop2_5(psr_data);
            psr_data->current_state = PSR_STATE_2_EXIT;
        }

        //Can't trans EXIT to any deeper state, that means EXIT can only be changed to INACTIVE or DISABLED
        //So do one more step here: EXIT-->INACTIVE; if failed, EXIT-->DISABLED will execute at reset func
        if(psr_data->current_state == PSR_STATE_2_EXIT)
        {
            cnt = 0;
            while(cnt < 40)
            {
                if(DISP_OK == disp_cbios_psrop5_1(psr_data))
                {
                    psr_data->current_state = PSR_STATE_1_INACTIVE;
                    break;
                }
                else
                {
                    cnt++;
                    continue;
                }
            }
            if(cnt >= 40)
            {
                zx_info("PSR trans to STATE_1_INACTIVE failed, to do reset.\n");
                status = PSR_OP_TO_INACTIVE_FAIL;
            }
        }
    }

    return status;
}

static int psr_get_dst_state(psr_data_t *psr_data)
{
    int operation = 0, ret = -1;

    operation = atomic_xchg(&psr_data->operation, 0);

    if(operation & PSR_OP_TRANS_TO_INACTIVE)
    {
        ret = PSR_STATE_1_INACTIVE;
    }
    else if(operation & PSR_OP_TRANS_TO_LEQ_IGA_OFF)
    {
        if(psr_data->current_state <= PSR_STATE_3_2_IGA_OFF)
        {
            ret = psr_data->current_state;
        }
        else
        {
            ret = PSR_STATE_3_2_IGA_OFF;
        }
    }
    else if(operation & PSR_OP_TRANS_TO_ACTIVE)
    {
        ret = PSR_STATE_3_3_DCLK_OFF;
    }

    return ret;
}

void psr_operate_trans_state(void* data)
{
    psr_data_t *psr_data = (psr_data_t *)data;
    int dst_state = PSR_STATE_1_INACTIVE;
    int new_state = 0;
    PSR_OPERATION_RESULT_T ret = PSR_OP_OK;

    if (!psr_data)
    {
        return;
    }

    zx_mutex_lock(psr_data->psr_mutex);

    dst_state = psr_get_dst_state(psr_data);

    if (psr_data->enabled && dst_state >= 0 && dst_state != psr_data->current_state)
    {
        while (psr_data->current_state != dst_state)
        {
            if (dst_state > psr_data->current_state)  // sleep to deep state
            {
                ret = psr_op_single_step(psr_data, 1);
            }
            else     //resume to shallow state
            {
                ret = psr_op_single_step(psr_data, 0);
            }

            if (ret != PSR_OP_OK)
            {
                psr_op_disable(psr_data);

                if (ret != PSR_OP_TO_ACTIVE_CRITICAL)
                {
                    psr_op_enable(psr_data);
                }
                break;
            }

            new_state = psr_get_dst_state(psr_data);
            if (new_state >= 0)
            {
                dst_state = new_state;
            }
        }
    }

    zx_mutex_unlock(psr_data->psr_mutex);
}

/*
void psr_op_exeucate(void *data)
{
    psr_data_t *psr_data = (psr_data_t *)data;
    PSR_OPERATION operation = 0;
    int error = 0;
    int cnt = 0;
    int status = 0;

    while(psr_data->ops)
    {
        operation = psr_data->ops;
        //zx_info("*****psr ops: %02X, enter state: %s*****\n", operation, psr_state_name[psr_data->current_state]);
        psr_data->ops = operation ^ psr_data->ops;
        operation = GET_LAST_BIT(operation);
        psr_data->current_op = operation;
        error = 0;

        switch (psr_data->current_state)
        {
        case PSR_STATE_0_DISABLED:
            switch (operation)
            {
            case PSR_OP_DISABLE:
                psr_data->ops = 0;
                zx_up(psr_data->psr_sema_disable);
                break;
            case PSR_OP_ENABLE:
                if(cnt < 1000)
                {
                    //error = psr_op_0_1();
                    error = disp_cbios_psrop0_1(psr_data);
                    if (!error)
                    {
                        psr_data->current_state = PSR_STATE_1_INACTIVE;
                        psr_data->enabled = 1;
                        cnt  = 0;
                    }
                    else
                    {
                        cnt++;
                    }
                    psr_data->ops |= PSR_OP_ENABLE;
                }
                else
                {
                    psr_data->ops = 0;
                    cnt = 0;
                }
                break;
            case PSR_OP_RESET:
                if(cnt < 1000)
                {
                    //error = psr_op_0_1();
                    error = disp_cbios_psrop0_1(psr_data);
                    if (!error)
                    {
                        psr_data->current_state = PSR_STATE_1_INACTIVE;
                        cnt = 0;
                    }
                    else
                    {
                        psr_data->ops |= PSR_OP_RESET;
                        cnt++;
                    }
                }
                else
                {
                    psr_data->ops = 0;
                    cnt = 0;
                }
                break;
            default:
                //psr_data->ops |= PSR_OP_RESET;
                break;
            }
            break;

        case PSR_STATE_1_INACTIVE:
            switch (operation)
            {
            case PSR_OP_DISABLE:
                //psr_op_1_0();
                disp_cbios_psrop1_0(psr_data);
                psr_data->enabled = 0;
                psr_data->current_state = PSR_STATE_0_DISABLED;
                psr_data->ops |= PSR_OP_DISABLE;
                break;
            case PSR_OP_ACTIVE:
                //psr_op_1_2();
                disp_cbios_psrop1_2(psr_data);
                psr_data->current_state = PSR_STATE_2_TRANSITION;
                psr_data->ops |= PSR_OP_ACTIVE;
                break;
            case PSR_OP_RESET:
                //psr_op_1_0();
                disp_cbios_psrop1_0(psr_data);
                psr_data->current_state = PSR_STATE_0_DISABLED;
                psr_data->ops |= PSR_OP_RESET;
                break;
            case PSR_OP_INACTIVE:
            case PSR_OP_SLEEP:
                break;
            case PSR_OP_ENABLE:
                zx_up(psr_data->psr_sema_enable);
                break;
            default:
                //error
                zx_info("psr error!\n");
                break;
            }
            break;

        case PSR_STATE_2_TRANSITION:
            switch (operation)
            {
            case PSR_OP_DISABLE:
                //psr_op_2_0();
                disp_cbios_psrop2_0(psr_data);
                psr_data->current_state = PSR_STATE_0_DISABLED;
                psr_data->enabled = 0;
                psr_data->ops |= PSR_OP_DISABLE;
                break;
            case PSR_OP_SLEEP:
                //psr_op_2_1();
                psr_data->current_state = PSR_STATE_1_INACTIVE;
                break;
            case PSR_OP_ACTIVE:
                //psr_op_2_3();
                if(cnt < 20)
                {
                    status = disp_cbios_psrop2_3(psr_data);
                    if(status == DISP_OK)
                    {
                        psr_data->current_state = PSR_STATE_3_ACTIVE;
                        psr_data->ops |= PSR_OP_ACTIVE;
                        cnt = 0;
                    }
                    else
                    {
                        psr_data->ops |= PSR_OP_ACTIVE;
                        cnt++;
                    }
                }
                else
                {
                    psr_data->ops |= PSR_OP_RESET;
                    cnt = 0;
                }
                break;
            case PSR_OP_INACTIVE:
                //psr_op_2_5();
                disp_cbios_psrop2_5(psr_data);
                psr_data->current_state = PSR_STATE_5_EXIT;
                psr_data->ops |= PSR_OP_INACTIVE;
                break;
            case PSR_OP_RESET:
                //psr_op_2_0();
                disp_cbios_psrop2_0(psr_data);
                psr_data->current_state = PSR_STATE_0_DISABLED;
                psr_data->ops |= PSR_OP_RESET;
                break;
            default:
                //error
                zx_info("psr error!\n");
                break;
            }
            break;

        case PSR_STATE_3_ACTIVE:
            switch (operation)
            {
            case PSR_OP_DISABLE:
                //psr_op_3_0();
                disp_cbios_psrop3_0(psr_data);
                psr_data->current_state = PSR_STATE_0_DISABLED;
                psr_data->enabled = 0;
                psr_data->ops |= PSR_OP_DISABLE;
                break;
            case PSR_OP_SLEEP:
                //psr_op_3_1();
                disp_cbios_psrop3_1(psr_data);
                psr_data->current_state = PSR_STATE_1_INACTIVE;
                break;
            case PSR_OP_ACTIVE:
                //psr_op_3_3_1();
                disp_cbios_psrop3_3_1(psr_data);
                psr_data->current_state = PSR_STATE_3_1_EPHY_OFF;
                psr_data->ops |= PSR_OP_ACTIVE;
                break;
            case PSR_OP_INACTIVE:
                //psr_op_3_5();
                disp_cbios_psrop3_5(psr_data);
                psr_data->current_state = PSR_STATE_5_EXIT;
                psr_data->ops |= PSR_OP_INACTIVE;
                break;
            case PSR_OP_RESET:
                //psr_op_3_0();
                disp_cbios_psrop3_0(psr_data);
                psr_data->current_state = PSR_STATE_0_DISABLED;
                psr_data->ops |= PSR_OP_RESET;
                break;
            default:
                //error
                zx_info("psr error!\n");
                break;
            }
            break;

        case PSR_STATE_3_1_EPHY_OFF:
            switch (operation)
            {
            case PSR_OP_DISABLE:
            case PSR_OP_SLEEP:
            case PSR_OP_INACTIVE:
            case PSR_OP_RESET:
                //psr_op_3_1_3();
                disp_cbios_psrop3_1_3(psr_data);
                psr_data->current_state = PSR_STATE_3_ACTIVE;
                psr_data->ops |= operation;
                break;
            case PSR_OP_ACTIVE:
                //psr_op_3_1_3_2();
                disp_cbios_psrop3_1_3_2(psr_data);
                psr_data->current_state = PSR_STATE_3_2_IGA_OFF;
                psr_data->ops |= PSR_OP_ACTIVE;
                break;
            default:
                //error
                zx_info("psr error!\n");
                break;
            }
            break;

        case PSR_STATE_3_2_IGA_OFF:
            switch (operation)
            {
            case PSR_OP_DISABLE:
            case PSR_OP_SLEEP:
            case PSR_OP_INACTIVE:
            case PSR_OP_RESET:
                //psr_op_3_2_3_1();
                disp_cbios_psrop3_2_3_1(psr_data);
                psr_data->current_state = PSR_STATE_3_1_EPHY_OFF;
                psr_data->ops |= operation;
                break;
            case PSR_OP_ACTIVE:
                //psr_op_3_2_3_3();
                disp_cbios_psrop3_2_3_3(psr_data);
                psr_data->current_state = PSR_STATE_3_3_DCLK_OFF;
                psr_data->ops |= PSR_OP_ACTIVE;
                break;
            default:
                //error
                zx_info("psr error!\n");
                break;
            }
            break;

        case PSR_STATE_3_3_DCLK_OFF:
            switch(operation)
            {
            case PSR_OP_DISABLE:
            case PSR_OP_SLEEP:
            case PSR_OP_INACTIVE:
            case PSR_OP_RESET:
                //psr_op_3_3_3_2();
                disp_cbios_psrop3_3_3_2(psr_data);
                psr_data->current_state = PSR_STATE_3_2_IGA_OFF;
                psr_data->ops |= operation;
                break;
            case PSR_OP_ACTIVE:
                break;
            default:
                //error
                zx_info("psr error!\n");
                break;
            }
            break;

        case PSR_STATE_5_EXIT:
            switch (operation)
            {
            case PSR_OP_DISABLE:
                //psr_op_5_0();
                disp_cbios_psrop5_0(psr_data);
                psr_data->current_state = PSR_STATE_0_DISABLED;
                psr_data->enabled = 0;
                psr_data->ops |= PSR_OP_DISABLE;
                break;
            case PSR_OP_SLEEP:
            case PSR_OP_INACTIVE:
                //psr_op_5_1();
                if(cnt < 20)
                {
                    status = disp_cbios_psrop5_1(psr_data);
                    if(status == DISP_OK)
                    {
                        psr_data->current_state = PSR_STATE_1_INACTIVE;
                        psr_data->ops |= operation;
                        cnt = 0;
                    }
                    else
                    {
                        psr_data->ops |= operation;
                        cnt++;
                    }
                }
                else
                {
                    psr_data->ops |= PSR_OP_RESET;
                    cnt = 0;
                }
                break;
            case PSR_OP_RESET:
                //psr_op_5_0();
                disp_cbios_psrop5_0(psr_data);
                psr_data->current_state = PSR_STATE_0_DISABLED;
                psr_data->ops |= PSR_OP_RESET;
                break;
            default:
                //error
                zx_info("psr error!\n");
                break;
            }
            break;
        default:
            //error
            zx_info("psr error!\n");
            break;
        }
    }
}

void psr_wait_disable_done(void *data)
{
    psr_data_t *psr_data = (psr_data_t *)data;

    while(1)
    {
        if (!zx_down_trylock(psr_data->psr_sema_disable))
        {
            psr_data->output_type = 0;
            psr_data->enabled = 0;
            break;
        }
        zx_msleep(1);
    }
}

void psr_wait_enable_done(void *data)
{
    psr_data_t *psr_data = (psr_data_t *)data;

    while(1)
    {
        if (!zx_down_trylock(psr_data->psr_sema_enable))
        {
            break;
        }
        zx_msleep(1);
    }
}*/

int disp_cbios_i2c_xfer(disp_info_t *disp_info, zx_xfer_msg_t *msg_param)
{
    CBIOS_PARAM_I2C_DATA i2c_data = {0};
    int cb_status = CBIOS_OK;

    i2c_data.DeviceId = msg_param->device_id;
    i2c_data.SlaveAddress = msg_param->addr;
    i2c_data.OffSet = msg_param->offset;
    i2c_data.Buffer = msg_param->buf;
    i2c_data.BufferLen = msg_param->buf_len;

    if (msg_param->op == ZX_MSG_OP_READ)
    {
        cb_status =  CBiosI2CDataRead(disp_info->cbios_ext, &i2c_data);
    }
    else
    {
        cb_status =  CBiosI2CDataWrite(disp_info->cbios_ext, &i2c_data);
    }

    return (cb_status == CBIOS_OK) ? DISP_OK : DISP_FAIL;
}

int disp_cbios_aux_xfer(disp_info_t *disp_info, zx_xfer_msg_t *msg_param)
{
    CBIOS_PARAM_AUX_DATA aux_data = {0};
    int cb_status = CBIOS_OK;

    aux_data.DeviceId = msg_param->device_id;
    aux_data.Address = msg_param->addr;
    aux_data.Offset = msg_param->offset;
    aux_data.Buffer = msg_param->buf;
    aux_data.BytesRequested = msg_param->buf_len;
    aux_data.IsDDCCI = msg_param->is_ddcci;
    aux_data.NativeAux = !!(msg_param->method == ZX_MSG_METHOD_AUX);
    aux_data.Write = !!(msg_param->op == ZX_MSG_OP_WRITE);
    aux_data.I2cStop = msg_param->i2c_stop;

    cb_status = CBiosAuxDataAccess(disp_info->cbios_ext, &aux_data);

    return (cb_status == CBIOS_OK) ? DISP_OK : DISP_FAIL;
}

int disp_cbios_msg_xfer(disp_info_t *disp_info, zx_xfer_msg_t *msg_param)
{
    int status = DISP_FAIL;

    if (msg_param->method == ZX_MSG_METHOD_I2C)
    {
        status = disp_cbios_i2c_xfer(disp_info, msg_param);
    }
    else
    {
        status = disp_cbios_aux_xfer(disp_info, msg_param);
    }

    return status;
}


