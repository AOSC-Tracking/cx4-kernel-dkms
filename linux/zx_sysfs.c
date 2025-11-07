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

#include "zx.h"
#include "zx_disp.h"
#include "zx_cbios.h"
#include "zx_driver.h"
#include "zx_version.h"


/*
    DEVICE_ATTR() will do PERMISSIONS Check
    Kernel Code VERIFY_OCTAL_PERMISSIONS() not allowed to Give "Other User" Write permissions
*/


#define ZX_DEVICE_ATTR_RO(name) \
    static DEVICE_ATTR(name,0444,zx_##name##_show,NULL)

#define ZX_DEVICE_ATTR_RW(name) \
    static DEVICE_ATTR(name,0664,zx_##name##_show,zx_##name##_store)

#define ZX_DEVICE_ATTR_WO(name) \
    static DEVICE_ATTR(name,0220,NULL,zx_##name##_store)

/*
    "zx_string_show" and "zx_string_store" is demo shows
    how to add sys read write file with ZX_DEVICE_ATTR_RW()
*/


static char string_buffer [64];

static ssize_t zx_string_show(struct device *dev, struct device_attribute *attr,char *buf)
{
    return sprintf(buf, "Store String:%s\n",string_buffer);
}

static ssize_t zx_string_store(struct device *dev,struct device_attribute *attr, const char *buf,size_t len)
{
    if(len>=64)
    {
        len = 63;
    }
    memcpy(string_buffer,buf,len);
    string_buffer[len] = '\0';
    return len;
}

extern int hwq_get_hwq_info(void *adp,zx_hwq_info *hwq_info);
static ssize_t zx_enable_usage_store(struct device *dev,struct device_attribute *attr, const char *buf,size_t len)
{
    int ret = 0;
    struct pci_dev*     pdev        = container_of(dev,struct pci_dev,dev);
    struct drm_device* drm_dev      = pci_get_drvdata(pdev);
    zx_card_t*         zx_card      = drm_dev->dev_private;

    unsigned long value = 0;
    ret = kstrtoul(buf, 0, &value);
    if(ret < 0)
    {
        return ret;
    }
    value = (value==0)? 0:1;
    zx_core_interface->ctl_flags_set(zx_card->adapter,1,(1UL<<21),(value<<21)); //First Int Bit21 hwq_event_enable
    if(value)
    {
        zx_core_interface->ctl_flags_set(zx_card->adapter,1,(1UL<<9),(value<<9)); //First Int Bit11, perf_event_enable
    }

    return len;
}

static ssize_t zx_enable_perf_store(struct device *dev,struct device_attribute *attr, const char *buf,size_t len)
{
    //First Int Bit15
    int ret = 0;
    struct pci_dev*     pdev        = container_of(dev,struct pci_dev,dev);
    struct drm_device* drm_dev      = pci_get_drvdata(pdev);
    zx_card_t*         zx_card      = drm_dev->dev_private;

    unsigned long value = 0;
    ret = kstrtoul(buf, 0, &value);
    if(ret < 0)
    {
        return ret;
    }

    value = (value==0)? 0:1;
    zx_core_interface->ctl_flags_set(zx_card->adapter,1,(1UL<<9),(value<<9)); //First Int Bit11 perf_event_enable
    if(value==0)
    {
        zx_core_interface->ctl_flags_set(zx_card->adapter,1,(1UL<<21),(value<<21)); //First Int Bit21 hwq_event_enable
    }

    return len;
}

static ssize_t zx_engine_3d_usage_show(struct device *dev, struct device_attribute *attr,char *buf)
{
    int ret = 0;
    struct pci_dev*     pdev        = container_of(dev,struct pci_dev,dev);
    struct drm_device* drm_dev      = pci_get_drvdata(pdev);
    zx_card_t*         zx_card      = drm_dev->dev_private;

    zx_hwq_info hwq_info;
    ret = hwq_get_hwq_info(zx_card->adapter,&hwq_info);

    if(ret == 0)
    {
        return sprintf(buf, "%d\n",hwq_info.Usage_3D);
    }

    return sprintf(buf, "Not Enable!\n");
}

static ssize_t zx_engine_vcp_usage_show(struct device *dev, struct device_attribute *attr,char *buf)
{
    int ret = 0;
    struct pci_dev*     pdev        = container_of(dev,struct pci_dev,dev);
    struct drm_device* drm_dev      = pci_get_drvdata(pdev);
    zx_card_t*         zx_card      = drm_dev->dev_private;

    zx_hwq_info hwq_info;
    ret = hwq_get_hwq_info(zx_card->adapter,&hwq_info);

    if(ret == 0)
    {
        return sprintf(buf, "%d\n",hwq_info.Usage_VCP);
    }

    return sprintf(buf, "Not Enable!\n");
}

static ssize_t zx_engine_vpp_usage_show(struct device *dev, struct device_attribute *attr,char *buf)
{
    int ret = 0;
    struct pci_dev*     pdev        = container_of(dev,struct pci_dev,dev);
    struct drm_device* drm_dev      = pci_get_drvdata(pdev);
    zx_card_t*         zx_card      = drm_dev->dev_private;

    zx_hwq_info hwq_info;
    ret = hwq_get_hwq_info(zx_card->adapter,&hwq_info);

    if(ret == 0)
    {
        return sprintf(buf, "%d\n",hwq_info.Usage_VPP);
    }

    return sprintf(buf, "Not Enable!\n");
}

static ssize_t zx_mclk_show(struct device *dev, struct device_attribute *attr,char *buf)
{
    struct pci_dev*     pdev        = container_of(dev,struct pci_dev,dev);
    struct drm_device* drm_dev      = pci_get_drvdata(pdev);
    zx_card_t*         zx_card      = drm_dev->dev_private;
    disp_info_t*       disp_info    = (disp_info_t*)zx_card->disp_info;

    unsigned int value = 0;
    if(DISP_OK == disp_cbios_get_clock(disp_info, ZX_QUERY_MCLK, &value))
    {
        return sprintf(buf,"%dMHz\n", (value + 500)/1000);
    }

    return sprintf(buf, "Mclk Read Error\n");
}

static ssize_t zx_fps_count_show(struct device *dev, struct device_attribute *attr,char *buf)
{
    struct pci_dev*     pdev        = container_of(dev,struct pci_dev,dev);
    struct drm_device* drm_dev      = pci_get_drvdata(pdev);
    zx_card_t*         zx_card      = drm_dev->dev_private;
    unsigned long long flip_timestamp;
    zx_get_nsecs(&flip_timestamp);
    if(zx_card->fps_count)
    {
        return sprintf(buf,"%u %lld\n", zx_card->fps_count,flip_timestamp);
    }

    return sprintf(buf, "fps Read Error\n");
}

static ssize_t zx_rxa_blt_scn_cnt_store(struct device *dev,struct device_attribute *attr, const char *buf,size_t len)
{
    struct pci_dev*     pdev        = container_of(dev,struct pci_dev,dev);
    struct drm_device* drm_dev      = pci_get_drvdata(pdev);
    zx_card_t*         zx_card      = drm_dev->dev_private;
    if(len>=16)
    {
        len = 16;
    }

    sscanf(buf, "%d", &zx_card->rxa_blt_scn_cnt);
    //zx_info("zx_rxa_blt_scn_cnt_store set data %d",  zx_card->rxa_blt_scn_cnt);

    return len;
}


static ssize_t zx_rxa_blt_scn_cnt_show(struct device *dev, struct device_attribute *attr,char *buf)
{
    struct pci_dev*     pdev        = container_of(dev,struct pci_dev,dev);
    struct drm_device* drm_dev      = pci_get_drvdata(pdev);
    zx_card_t*         zx_card      = drm_dev->dev_private;

    //zx_info("zx_rxa_blt_scn_cnt_store get data %d",  zx_card->rxa_blt_scn_cnt);

    if(zx_card->rxa_blt_scn_cnt > 0)
    {
        return sprintf(buf,"%d\n", zx_card->rxa_blt_scn_cnt);
    }

    return sprintf(buf, "rxa_blt_scn_cnt Read Error\n");
}

static ssize_t zx_vclk_show(struct device *dev, struct device_attribute *attr,char *buf)
{
    struct pci_dev*     pdev        = container_of(dev,struct pci_dev,dev);
    struct drm_device* drm_dev      = pci_get_drvdata(pdev);
    zx_card_t*         zx_card      = drm_dev->dev_private;
    disp_info_t*       disp_info    = (disp_info_t*)zx_card->disp_info;

    unsigned int value = 0;
    if(DISP_OK == disp_cbios_get_clock(disp_info, ZX_QUERY_VCLK, &value))
    {
        return sprintf(buf,"%dMHz\n", (value + 500)/1000);
    }

    return sprintf(buf, "Vclk Read Error\n");
}

static ssize_t zx_eclk_show(struct device *dev, struct device_attribute *attr,char *buf)
{
    struct pci_dev*     pdev        = container_of(dev,struct pci_dev,dev);
    struct drm_device* drm_dev      = pci_get_drvdata(pdev);
    zx_card_t*         zx_card      = drm_dev->dev_private;
    disp_info_t*       disp_info    = (disp_info_t*)zx_card->disp_info;

    unsigned int value = 0;
    if(DISP_OK == disp_cbios_get_clock(disp_info, ZX_QUERY_ENGINE_CLOCK, &value))
    {
        return sprintf(buf, "%dMHz\n", (value + 500)/1000);
    }

    return sprintf(buf, "Eclk Read Error\n");

}

static ssize_t zx_free_fb_mem_show(struct device *dev, struct device_attribute *attr,char *buf)
{
    struct pci_dev*     pdev        = container_of(dev,struct pci_dev,dev);
    struct drm_device* drm_dev      = pci_get_drvdata(pdev);
    zx_card_t*         zx_card      = drm_dev->dev_private;
    adapter_info_t* adapter_info    = &zx_card->adapter_info;

    zx_query_info_t  query_info;
    int mem_total = 0;
    query_info.type = ZX_QUERY_SEGMENT_FREE_SIZE;
    query_info.argu = 1;
    zx_core_interface->query_info(zx_card->adapter,&query_info);
    mem_total += query_info.signed_value;

    if (mem_total < 0)
        mem_total = 0;

    if (mem_total > adapter_info->total_mem_size_mb * 1024)
        mem_total = adapter_info->total_mem_size_mb * 1024;
    return sprintf(buf,"%d\n",mem_total);
}

static ssize_t zx_fb_size_show(struct device *dev, struct device_attribute *attr,char *buf)
{
    struct pci_dev*     pdev        = container_of(dev,struct pci_dev,dev);
    struct drm_device* drm_dev      = pci_get_drvdata(pdev);
    zx_card_t*         zx_card      = drm_dev->dev_private;
    adapter_info_t* adapter_info    = &zx_card->adapter_info;

    return sprintf(buf,"%d M\n",adapter_info->total_mem_size_mb);
}

static ssize_t zx_slice_mask_show(struct device *dev, struct device_attribute *attr,char *buf)
{
    struct pci_dev*     pdev        = container_of(dev,struct pci_dev,dev);
    struct drm_device* drm_dev      = pci_get_drvdata(pdev);
    zx_card_t*         zx_card      = drm_dev->dev_private;
    adapter_info_t* adapter_info    = &zx_card->adapter_info;

    return sprintf(buf,"N/A\n");
}

static ssize_t zx_miu_channel_num_show(struct device *dev, struct device_attribute *attr,char *buf)
{
    struct pci_dev*     pdev        = container_of(dev,struct pci_dev,dev);
    struct drm_device* drm_dev      = pci_get_drvdata(pdev);
    zx_card_t*         zx_card      = drm_dev->dev_private;
    adapter_info_t* adapter_info    = &zx_card->adapter_info;

    return sprintf(buf,"%d\n",adapter_info->chan_num);

}

static ssize_t zx_driver_version_show(struct device *dev, struct device_attribute *attr,char *buf)
{
    return sprintf(buf,"%02x.%02x.%02x%s\n",DRIVER_MAJOR,DRIVER_MINOR,DRIVER_PATCHLEVEL,DRIVER_CLASS);
}

static ssize_t zx_release_date_show(struct device *dev, struct device_attribute *attr,char *buf)
{
    return sprintf(buf,"%s\n",DRIVER_DATE);
}

static ssize_t zx_pmp_version_show(struct device *dev, struct device_attribute *attr,char *buf)
{
    struct pci_dev*     pdev        = container_of(dev,struct pci_dev,dev);
    struct drm_device* drm_dev      = pci_get_drvdata(pdev);
    zx_card_t*         zx_card      = drm_dev->dev_private;
    disp_info_t*       disp_info    = (disp_info_t*)zx_card->disp_info;

    unsigned char* pmpversion       = disp_info->pmp_version;
    int  pmpdatelen                 = 0;
    int  pmptimelen                 = 0;

    if(*pmpversion)
    {
        // pmpVersion:str1 -> pmp version,str2 -> pmp build date,str3 -> pmp build time
        pmpdatelen = zx_strlen(pmpversion) + 1;
        pmptimelen = zx_strlen(pmpversion + pmpdatelen) + 1;
        return sprintf(buf, "%s %s %s\n",pmpversion,(pmpversion + pmpdatelen),(pmpversion  + pmpdatelen + pmptimelen) );
    }
    return sprintf(buf,"get error\n");

}

static ssize_t zx_vbios_version_show(struct device *dev, struct device_attribute *attr,char *buf)
{
    struct pci_dev*     pdev        = container_of(dev,struct pci_dev,dev);
    struct drm_device* drm_dev      = pci_get_drvdata(pdev);
    zx_card_t*         zx_card      = drm_dev->dev_private;
    disp_info_t*       disp_info    = (disp_info_t*)zx_card->disp_info;
    int  vbiosVer                   = disp_info->vbios_version;
    return sprintf(buf, "%02x.%02x.%02x.%02x\n", (vbiosVer>>24)&0xff,(vbiosVer>>16)&0xff,(vbiosVer>>8)&0xff,vbiosVer&0xff);
}

ZX_DEVICE_ATTR_RW(string);
ZX_DEVICE_ATTR_WO(enable_usage);
ZX_DEVICE_ATTR_WO(enable_perf);

ZX_DEVICE_ATTR_RO(engine_3d_usage);
ZX_DEVICE_ATTR_RO(engine_vcp_usage);
ZX_DEVICE_ATTR_RO(engine_vpp_usage);
ZX_DEVICE_ATTR_RO(vbios_version);
ZX_DEVICE_ATTR_RO(pmp_version);
ZX_DEVICE_ATTR_RO(release_date);
ZX_DEVICE_ATTR_RO(driver_version);
ZX_DEVICE_ATTR_RO(miu_channel_num);
ZX_DEVICE_ATTR_RO(slice_mask);
ZX_DEVICE_ATTR_RO(fb_size);
ZX_DEVICE_ATTR_RO(free_fb_mem);
ZX_DEVICE_ATTR_RO(eclk);
ZX_DEVICE_ATTR_RO(vclk);
ZX_DEVICE_ATTR_RO(mclk);
ZX_DEVICE_ATTR_RO(fps_count);
ZX_DEVICE_ATTR_RW(rxa_blt_scn_cnt);

static struct attribute *zx_info_attributes[] = {
    &dev_attr_string.attr,
    &dev_attr_enable_usage.attr,
    &dev_attr_enable_perf.attr,

    &dev_attr_engine_3d_usage.attr,
    &dev_attr_engine_vpp_usage.attr,
    &dev_attr_engine_vcp_usage.attr,
    &dev_attr_vbios_version.attr,
    &dev_attr_pmp_version.attr,
    &dev_attr_release_date.attr,
    &dev_attr_driver_version.attr,
    &dev_attr_miu_channel_num.attr,
    &dev_attr_slice_mask.attr,
    &dev_attr_fb_size.attr,
    &dev_attr_free_fb_mem.attr,
    &dev_attr_eclk.attr,
    &dev_attr_vclk.attr,
    &dev_attr_mclk.attr,
    &dev_attr_fps_count.attr,
    &dev_attr_rxa_blt_scn_cnt.attr,
    NULL
};

const struct attribute_group zx_sysfs_group = {
    .attrs = zx_info_attributes,
    .name  = "zx_info"
};

/**
 * Format such as:
 * VRAM total size:0x80000000
*/
static ssize_t zx_gpu_info_show(struct device *dev, struct device_attribute *attr,char *buf)
{
    struct pci_dev*     pdev        = container_of(dev,struct pci_dev,dev);
    struct drm_device* drm_dev      = pci_get_drvdata(pdev);
    zx_card_t*         zx_card      = drm_dev->dev_private;
    adapter_info_t* adapter_info    = &zx_card->adapter_info;
    unsigned long long total_mem_size = adapter_info->total_mem_size_mb;
    total_mem_size                  = total_mem_size *1024*1024;

    return sprintf(buf,"VRAM total size:0x%llx\n",total_mem_size);
}

static struct device_attribute dev_attr_gpu_info = __ATTR(gpu-info,0444,zx_gpu_info_show,NULL);

const struct attribute *zx_os_gpu_info[] = {
    &dev_attr_gpu_info.attr,
    NULL
};

#if DRM_VERSION_CODE < KERNEL_VERSION(6, 16, 0)
static ssize_t zx_sysfs_trace_read(struct file *filp, struct kobject *kobj, struct bin_attribute *bin_attr, char *buf, loff_t pos, size_t size)
#else
static ssize_t zx_sysfs_trace_read(struct file *filp, struct kobject *kobj, const struct bin_attribute *bin_attr, char *buf, loff_t pos, size_t size)
#endif
{
    struct pci_dev*    pdev    = to_pci_dev(kobj_to_dev(kobj));
    struct drm_device* drm_dev = pci_get_drvdata(pdev);
    zx_card_t*         zx_card = drm_dev->dev_private;
    ssize_t            ret     = 0;

    if (zx_card->trace_buffer_vma && zx_card->trace_buffer_vma->virt_addr)
    {
        char val_buf[32];
        unsigned int len;

        len = sprintf(val_buf, "%llu\n", *((uint64_t *)zx_card->trace_buffer_vma->virt_addr));
        ret = memory_read_from_buffer(buf, size, &pos, val_buf, len);
    }

    return ret;
}

#if DRM_VERSION_CODE < KERNEL_VERSION(6, 16, 0)
static ssize_t zx_sysfs_trace_write(struct file *filp, struct kobject *kobj, struct bin_attribute *bin_attr, char *buf, loff_t pos, size_t size)
#else
static ssize_t zx_sysfs_trace_write(struct file *filp, struct kobject *kobj, const struct bin_attribute *bin_attr, char *buf, loff_t pos, size_t size)
#endif
{
    struct pci_dev*    pdev    = to_pci_dev(kobj_to_dev(kobj));
    struct drm_device* drm_dev = pci_get_drvdata(pdev);
    zx_card_t*         zx_card = drm_dev->dev_private;
    unsigned long val;
    int ret;

    if (!zx_card->trace_buffer_vma || !zx_card->trace_buffer_vma->virt_addr)
        return 0;

    ret = kstrtoul(buf, 0, &val);
    if (ret)
        return ret;

    *((uint64_t *)zx_card->trace_buffer_vma->virt_addr) = val;

    return size;
}

// for gdb (ptrace) to access this memory by access_process_vm
static int zx_sysfs_trace_access(struct vm_area_struct *vma, unsigned long addr, void *buf, int len, int write)
{
    zx_card_t* zx_card = vma->vm_private_data;
    unsigned long offset = addr - vma->vm_start;

    // zx_info("zx_sysfs_trace_access: vma=%p, addr=%lx, vma->vm_start=%llx\n", vma, addr, vma->vm_start);

    if (offset + len > zx_card->trace_buffer->size)
        return -EINVAL;

    if (!zx_card->trace_buffer_vma || !zx_card->trace_buffer_vma->virt_addr)
        return 0;

    if (write)
        zx_memcpy(zx_card->trace_buffer_vma->virt_addr + offset, buf, len);
    else
        zx_memcpy(buf, zx_card->trace_buffer_vma->virt_addr + offset, len);

    return len;
}

static const struct vm_operations_struct zx_sysfs_trace_vmops = {
    .access = zx_sysfs_trace_access,
};

extern unsigned char zx_validate_page_cache(struct os_pages_memory *memory, int start_page, int end_page, unsigned char request_cache_type);

#if DRM_VERSION_CODE < KERNEL_VERSION(6, 13, 0)
static int zx_sysfs_trace_mmap(struct file *filp, struct kobject *kobj,
                                   struct bin_attribute *attr,
                                   struct vm_area_struct *vma)
#else
static int zx_sysfs_trace_mmap(struct file *filp, struct kobject *kobj,
                                   const struct bin_attribute *attr,
                                   struct vm_area_struct *vma)
#endif
{
    struct pci_dev*    pdev    = to_pci_dev(kobj_to_dev(kobj));
    struct drm_device* drm_dev = pci_get_drvdata(pdev);
    zx_card_t*         zx_card = drm_dev->dev_private;
    unsigned int       cache_type;

    zx_map_argu_t      map_argu = {0};
    int                start_page, end_page;

    // zx_info("zx_sysfs_trace_mmap: vma=%p, vma->vm_start=%llx\n", vma, vma->vm_start);

    vma->vm_ops = &zx_sysfs_trace_vmops;
    vma->vm_private_data = zx_card;

    map_argu.memory = zx_card->trace_buffer;
    map_argu.flags.read_only = true;
    map_argu.flags.mem_type = ZX_SYSTEM_RAM;
    map_argu.size = zx_card->trace_buffer->size;
    map_argu.offset = 0;
    start_page = _ALIGN_DOWN(map_argu.offset, PAGE_SIZE)/PAGE_SIZE;
    end_page = start_page + ALIGN(map_argu.size, PAGE_SIZE) / PAGE_SIZE;
    map_argu.flags.cache_type = zx_validate_page_cache(map_argu.memory, start_page, end_page, ZX_MEM_WRITE_BACK);

    return zx_map_system_ram(vma, &map_argu);
}

const struct bin_attribute zx_sysfs_trace_attr = {
    .attr = {.name = "trace", .mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH}, // 0644
    .size = PAGE_SIZE,
    .read = zx_sysfs_trace_read,
    .write = zx_sysfs_trace_write,
    .mmap = zx_sysfs_trace_mmap,
};
