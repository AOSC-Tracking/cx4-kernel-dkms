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
#include "zx_debugfs.h"
#include "zx_kms.h"
#include "zx_drm_helper.h"

typedef struct zx_mmio_segment
{
    const char *name;
    unsigned int start;
    unsigned int end;
    unsigned int alignment;
}zx_mmio_segment_t;

zx_mmio_segment_t  mmio_seg_e3k[] = {
    {"MMIO1", 0x8000, 0x80E4, 4},
    {"MMIO2", 0x8180, 0x83B0, 4},
    {"MMIO3", 0x8400, 0x8600, 4},
    {"MMIO4", 0x8600, 0x8B00, 1},
    {"MMIO5", 0x9000, 0x9200, 1},
    {"MMIO6", 0x9400, 0x9600, 1},
    {"MMIO7", 0x30000, 0x30380,4}, /* CSP */
    {"MMIO8", 0x33000, 0x35000, 4},
    {"MMIO9", 0x48C00, 0x48CD0, 4},
    {"MMIO10", 0x49000, 0x49080, 4},
    {"MMIO11", 0x4a000, 0x4f000, 4},
};

static int zx_debugfs_node_show(struct seq_file *s, void *unused)
{
    zx_debugfs_node_t *node = (zx_debugfs_node_t *)(s->private);
    struct os_seq_file seq_file={0};
    struct os_printer seq_p = zx_seq_file_printer(&seq_file);

    seq_file.seq_file = s;

    zx_mutex_lock(node->dev->lock);

    switch(node->type)
    {
        case DEBUGFS_NODE_DEVICE:
            zx_core_interface->debugfs_dump(&seq_p, node->priv_data, node->type, &node->hDevice);
            break;

        case DEBUGFS_NODE_HEAP:
            zx_core_interface->debugfs_dump(&seq_p, node->priv_data, node->type, &node->id);
            break;

        case DEBUGFS_NODE_INFO:
            zx_core_interface->debugfs_dump(&seq_p, node->priv_data, node->type, NULL);
            seq_printf(s, "VA=1\n");
            break;

        case DEBUGFS_NODE_MEMTRACK:
            zx_core_interface->debugfs_dump(&seq_p, node->priv_data, node->type, &node->id);
            break;
        case DEBUGFS_NODE_DVFS:
            zx_core_interface->debugfs_dump(&seq_p, node->priv_data, node->type, NULL);
            break;
        case DEBUGFS_NODE_CG:
            zx_core_interface->debugfs_dump(&seq_p, node->priv_data, node->type, NULL);
            break;
        case DEBUGFS_NODE_VIDSCH:
            zx_core_interface->debugfs_dump(&seq_p, node->priv_data, node->type, &seq_p);
            break;
        case DEBUGFS_NODE_DEBUGBUS:
            zx_core_interface->debugfs_dump(&seq_p, node->priv_data, node->type, &seq_p);
            break;
        default:
            zx_info("dump unknow node :%d\n", node->type);
            break;
    }

    zx_mutex_unlock(node->dev->lock);

    return 0;
}

static int zx_debugfs_node_open(struct inode *inode, struct file *file)
{
    return single_open(file, zx_debugfs_node_show, inode->i_private);
}

static ssize_t zx_debugfs_node_write(struct file *file, const char __user *buf, size_t size, loff_t *ppos)
{
    struct os_seq_file seq_file = {0};
    zx_debugfs_node_t *node = NULL;
    char info[32] = {'\0'};
    int ret = 0;

    seq_file.seq_file = file->private_data;
    node = (zx_debugfs_node_t *)((seq_file.seq_file)->private);

    if(size > 1 && size < 32)
    {
        ret = zx_copy_from_user(info, buf, size);
        if(!ret)
        {
            info[size-1] = '\0';
        }
        else
            return -1;
    }

    zx_mutex_lock(node->dev->lock);

    if (!zx_strcmp(info, "1"))
    {
        zx_core_interface->dump_hang_info_flag(node->priv_data, 1);
    }
    else
    {
        zx_core_interface->dump_hang_info_flag(node->priv_data, 0);
    }


    zx_mutex_unlock(node->dev->lock);

    return (ret == 0) ? size : ret;
}

static const struct file_operations debugfs_node_fops = {
        .open       = zx_debugfs_node_open,
        .read       = seq_read,
        .write      = zx_debugfs_node_write,
        .llseek     = seq_lseek,
        .release    = single_release,
};

// displayinfo node
static int zx_debugfs_node_displayinfo_show(struct seq_file *s, void *unused)
{
    zx_debugfs_node_t *node = (zx_debugfs_node_t *)(s->private);
    struct drm_device* dev = node->dev->zx->drm_dev;

    zx_debugfs_displayinfo_dump(s, dev);

    return 0;
}

static int zx_debugfs_node_displayinfo_open(struct inode *inode, struct file *file)
{
    return single_open(file, zx_debugfs_node_displayinfo_show, inode->i_private);
}


static const struct file_operations debugfs_node_displayinfo_fops = {
        .open       = zx_debugfs_node_displayinfo_open,
        .read       = seq_read,
        .llseek     = seq_lseek,
        .release    = single_release,
};

static int zx_debugfs_node_psr_show(struct seq_file *s, void *unused)
{
    zx_debugfs_node_t *node = (zx_debugfs_node_t *)(s->private);
    struct drm_device* dev = node->dev->zx->drm_dev;

    zx_debugfs_psr_dump(s, dev);

    return 0;
}

static ssize_t zx_debugfs_psr_node_write(struct file *file, const char __user *buf, size_t size, loff_t *ppos)
{
    struct os_seq_file seq_file = {0};
    zx_debugfs_node_t *node = NULL;
    struct drm_device* dev = NULL;
    char info[32] = {'\0'};
    int ret = 0;

    seq_file.seq_file = file->private_data;
    node = (zx_debugfs_node_t *)((seq_file.seq_file)->private);
    dev = node->dev->zx->drm_dev;

    if(size > 1 && size < 32)
    {
        ret = zx_copy_from_user(info, buf, size);
        if(!ret)
        {
            info[size-1] = '\0';
        }
        else
            return -1;
    }

    if (!zx_strcmp(info, "0"))
    {
        zx_debugfs_psr_onoff(dev, 0);
    }
    else
    {
        zx_debugfs_psr_onoff(dev, 1);
    }

    return (ret == 0) ? size : ret;
}

static int zx_debugfs_node_psr_open(struct inode *inode, struct file *file)
{
    return single_open(file, zx_debugfs_node_psr_show, inode->i_private);
}

static const struct file_operations debugfs_node_psr_fops = {
        .open       = zx_debugfs_node_psr_open,
        .read       = seq_read,
        .write      = zx_debugfs_psr_node_write,
        .llseek     = seq_lseek,
        .release    = single_release,
};

static int zx_debugfs_node_umd_trace_show(struct seq_file *s, void *unused)
{
    zx_debugfs_node_t *node = (zx_debugfs_node_t *)(s->private);

    seq_printf(s, "0x%lx\n", node->dev->zx->umd_trace_tags);

    return 0;
}

static int zx_debugfs_node_umd_trace_open(struct inode *inode, struct file *file)
{
    return single_open(file, zx_debugfs_node_umd_trace_show, inode->i_private);
}

static ssize_t zx_debugfs_node_umd_trace_write(struct file *f, const char __user *buf,
                                               size_t size, loff_t *pos)
{
    zx_debugfs_node_t* node = file_inode(f)->i_private;
    zx_card_t *zx_card = node->dev->zx;
    unsigned long val;
    int ret;
    char event_string[32];
    char *envp[] = { event_string, NULL };

    ret = kstrtoul_from_user(buf, size, 0, &val);
    if (ret)
        return ret;

    zx_card->umd_trace_tags = val;

    // trace_buffer is the new way to control trace
    if (zx_card->trace_buffer_vma && zx_card->trace_buffer_vma->virt_addr)
    {
        *((uint64_t *)zx_card->trace_buffer_vma->virt_addr) = val;
    }

    zx_vsprintf(event_string, "ZX_TRACE_TAGS=0x%lx", val);
    // send uevent to usermode to enable/disable trace
    kobject_uevent_env(&zx_card->pdev->dev.kobj, KOBJ_CHANGE, envp);

    *pos += size;

    return size;
}

static const struct file_operations debugfs_node_umd_trace_fops = {
        .open       = zx_debugfs_node_umd_trace_open,
        .read       = seq_read,
        .write      = zx_debugfs_node_umd_trace_write,
        .llseek     = seq_lseek,
        .release    = single_release,
};

static int zx_debugfs_node_test_mem_show(struct seq_file *s, void *unused)
{
    zx_debugfs_node_t *node = (zx_debugfs_node_t *)(s->private);
    struct os_seq_file seq_file={0};
    struct os_printer seq_p = zx_seq_file_printer(&seq_file);
    zx_test_mem_para_t *para = &node->dev->zx->test_mem_para;

    seq_file.seq_file = s;

    seq_printf(s, "test gpu %s physical address = 0x%llx result:\n", para->read ? "read" : "write", para->phys_addr);

    zx_core_interface->debugfs_dump(&seq_p, node->dev->zx->adapter, DEBUGFS_NODE_TEST_MEM, para);

    return 0;
}

static int zx_debugfs_node_test_mem_open(struct inode *inode, struct file *file)
{
    return single_open(file, zx_debugfs_node_test_mem_show, inode->i_private);
}

static ssize_t zx_debugfs_node_test_mem_write(struct file *f, const char __user *buf,
                                               size_t size, loff_t *pos)
{
    zx_debugfs_node_t* node = file_inode(f)->i_private;
    int ret;
    char *val = zx_calloc(size+1);
    zx_test_mem_para_t *para = &node->dev->zx->test_mem_para;
    char read = 0;

    zx_copy_from_user(val, buf, size);

    zx_info("test mem input:%s", val);

    zx_sscanf(val, "%llx %c %d", &para->phys_addr, &read, &para->snoop);

    para->read = read == 'r' ? 1 : 0;

    zx_info("test mem para: phys_addr=0x%llx read=%d snoop=%d\n", para->phys_addr, para->read, para->snoop);

    *pos += size;

    zx_free(val);

    return size;
}

static const struct file_operations debugfs_node_test_mem_fops = {
        .open       = zx_debugfs_node_test_mem_open,
        .read       = seq_read,
        .write      = zx_debugfs_node_test_mem_write,
        .llseek     = seq_lseek,
        .release    = single_release,
};

int  zx_mmio_find_segment(zx_mmio_segment_t*  seg, int seg_num, int mmio_size, int pos, int* seg_index, int* empty)
{
    int i = 0;

    if(!seg || !seg_num || !mmio_size || (pos >= mmio_size))
    {
        return  -EINVAL;
    }

    for(i = 0; i < seg_num; i++)
    {
        if(pos >= 0 && pos < seg[0].start)
        {
            *empty = 1;
            *seg_index = 0;
            break;
        }
        else if(pos >= seg[seg_num-1].end)
        {
            *empty = 1;
            *seg_index = seg_num;
            break;
        }
        else if(pos >= seg[i].start && pos < seg[i].end)
        {
            *empty = 0;
            *seg_index = i;
            break;
        }
        else if(pos >= seg[i].end && pos < seg[i+1].start)
        {
            *empty = 1;
            *seg_index = i+1;
            break;
        }
    }

    return 0;
}

static ssize_t zx_mmio_write(struct file *f, const char __user *buf,
                                         size_t size, loff_t *pos)
{
    zx_debugfs_node_t* node = file_inode(f)->i_private;
    adapter_info_t *ainfo = &node->dev->zx->adapter_info;
    unsigned int  mmio_size = ainfo->mmio_size + 1;
    unsigned char  *mmio_base = ainfo->mmio;
    zx_mmio_segment_t*  mmio_seg = mmio_seg_e3k;
    unsigned int   seg_num = sizeof(mmio_seg_e3k)/sizeof(mmio_seg_e3k[0]);
    unsigned int  length = 0, seg_end = 0, empty_hole = 0, seg_align = 0, index = 0;
    ssize_t result = 0;
    uint32_t value = 0;

    if(!size || zx_mmio_find_segment(mmio_seg, seg_num, mmio_size, *pos, &index, &empty_hole))
    {
        return  0;
    }

    seg_align = (empty_hole)? 4 : mmio_seg[index].alignment;
    if(*pos & (seg_align - 1))
    {
        return  -EINVAL;
    }

    if(*pos + size > mmio_size)
    {
        size = mmio_size - *pos;
    }

    zx_mmio_find_segment(mmio_seg, seg_num, mmio_size, *pos+size-1, &index, &empty_hole);
    seg_align = (empty_hole)? 4 : mmio_seg[index].alignment;
    if((*pos + size) & (seg_align - 1))
    {
        return  -EINVAL;
    }

    while(size)
    {
        zx_mmio_find_segment(mmio_seg, seg_num, mmio_size, *pos, &index, &empty_hole);
        if(empty_hole)
        {
            seg_end = (index == seg_num)? mmio_size : mmio_seg[index].start;
        }
        else
        {
            seg_end = mmio_seg[index].end;
        }
        seg_align = (empty_hole)? 4 : mmio_seg[index].alignment;

        if(*pos + size > seg_end)
        {
            length = seg_end - *pos;
        }
        else
        {
            length = size;
        }

        if(empty_hole)
        {
            *pos += length;
            buf += length;
            result += length;
            size -= length;
        }
        else
        {
            size -= length;
            while(length)
            {
                if(seg_align == 4)
                {
                    get_user(value, (uint32_t __user *)buf);
                    zx_write32(mmio_base + *pos, value);
                }
                else if(seg_align == 2)
                {
                    get_user(value, (uint16_t __user *)buf);
                    zx_write16(mmio_base + *pos, (uint16_t)value);
                }
                else if(seg_align == 1)
                {
                    get_user(value, (uint8_t __user *)buf);
                    zx_write8(mmio_base + *pos, (uint8_t)value);
                }
                else
                {
                    zx_assert(0, "seg_align:%d", seg_align);
                }
                *pos += seg_align;
                buf += seg_align;
                result += seg_align;
                length -= seg_align;
            }
        }
    }

    return result;
}

static ssize_t zx_mmio_read(struct file *f, char __user *buf,
                    size_t size, loff_t *pos)
{
    zx_debugfs_node_t* node = file_inode(f)->i_private;
    adapter_info_t *ainfo = &node->dev->zx->adapter_info;
    unsigned int  mmio_size = ainfo->mmio_size + 1;
    unsigned char  *mmio_base = ainfo->mmio;
    zx_mmio_segment_t*  mmio_seg = mmio_seg_e3k;
    unsigned int   seg_num = sizeof(mmio_seg_e3k)/sizeof(mmio_seg_e3k[0]);
    unsigned int  length = 0, seg_end = 0, empty_hole = 0, seg_align = 0, index = 0;
    ssize_t result = 0;
    uint32_t value = 0;

    if(!size || zx_mmio_find_segment(mmio_seg, seg_num, mmio_size, *pos, &index, &empty_hole))
    {
        return  0;
    }

    seg_align = (empty_hole)? 4 : mmio_seg[index].alignment;
    if(*pos & (seg_align - 1))
    {
        return  -EINVAL;
    }

    if(*pos + size > mmio_size)
    {
        size = mmio_size - *pos;
    }

    zx_mmio_find_segment(mmio_seg, seg_num, mmio_size, *pos+size-1, &index, &empty_hole);
    seg_align = (empty_hole)? 4 : mmio_seg[index].alignment;
    if((*pos + size) & (seg_align - 1))
    {
        return  -EINVAL;
    }

    while(size)
    {
        zx_mmio_find_segment(mmio_seg, seg_num, mmio_size, *pos, &index, &empty_hole);
        if(empty_hole)
        {
            seg_end = (index == seg_num)? mmio_size : mmio_seg[index].start;
        }
        else
        {
            seg_end = mmio_seg[index].end;
        }
        seg_align = (empty_hole)? 4 : mmio_seg[index].alignment;

        if(*pos + size > seg_end)
        {
            length = seg_end - *pos;
        }
        else
        {
            length = size;
        }

        if(empty_hole)
        {
            value = 0;
            size -= length;
            while(length)
            {
                put_user(value, (uint32_t __user *)buf);
                *pos += seg_align;
                buf += seg_align;
                result += seg_align;
                length -= seg_align;
            }
        }
        else
        {
            size -= length;
            while(length)
            {
                if(seg_align == 4)
                {
                    value = zx_read32(mmio_base + *pos);
                    put_user(value, (uint32_t __user *)buf);
                }
                else if(seg_align == 2)
                {
                    value = zx_read16(mmio_base + *pos);
                    put_user((uint16_t)value, (uint16_t __user *)buf);
                }
                else if(seg_align == 1)
                {
                    value = zx_read8(mmio_base + *pos);
                    put_user((uint8_t)value, (uint8_t __user *)buf);
                }
                else
                {
                    zx_assert(0, "seg_align:%d", seg_align);
                }
                *pos += seg_align;
                buf += seg_align;
                result += seg_align;
                length -= seg_align;
            }
        }
    }

    return result;
}

static const struct file_operations debugfs_mmio_reg_fops = {
        .owner = THIS_MODULE,
        .read       = zx_mmio_read,
        .write      = zx_mmio_write,
        .llseek     = default_llseek,
};

static int zx_debugfs_mmio_info_show(struct seq_file *s, void *unused)
{
    int i;
    zx_debugfs_node_t *node = (zx_debugfs_node_t *)(s->private);
    adapter_info_t *ainfo = &node->dev->zx->adapter_info;

    seq_printf(s, "mmio virtual base=0x%p\n", ainfo->mmio);
    seq_printf(s, "mmio size=0x%x\n", ainfo->mmio_size);
    for (i = 0; i < sizeof(mmio_seg_e3k)/sizeof(mmio_seg_e3k[0]); i++)
    {
        seq_printf(s, "mmio seg %s: start=0x%0x, end=0x%0x, alignment=%d\n",
            mmio_seg_e3k[i].name, mmio_seg_e3k[i].start, mmio_seg_e3k[i].end, mmio_seg_e3k[i].alignment);
    }
    return 0;
}

static int zx_debugfs_mmio_info_open(struct inode *inode, struct file *file)
{
    return single_open(file, zx_debugfs_mmio_info_show, inode->i_private);
}

static const struct file_operations debugfs_mmio_info_fops = {
        .open       = zx_debugfs_mmio_info_open,
        .read       = seq_read,
        .llseek     = seq_lseek,
        .release    = single_release,
};

static void zx_debugfs_init_mmio_nodes(zx_debugfs_device_t *debug_dev)
{
    zx_debugfs_mmio_t  *debug_mmio = &debug_dev->mmio;
    debug_mmio->mmio_root = debugfs_create_dir("mmio", debug_dev->debug_root);
    if (debug_mmio->mmio_root)
    {

        debug_mmio->regs.type       = 0;
        debug_mmio->regs.id         = 0;
        debug_mmio->regs.priv_data  = debug_dev->zx->adapter;
        debug_mmio->regs.dev        = debug_dev;

        zx_vsprintf(debug_mmio->regs.name, "regs");

        debug_mmio->regs.node_dentry = debugfs_create_file(debug_mmio->regs.name, 0664, debug_mmio->mmio_root, &(debug_mmio->regs), &debugfs_mmio_reg_fops);

        if (debug_mmio->regs.node_dentry == NULL)
        {
             zx_error("Failed to create debugfs node %s\n", debug_mmio->regs.name);
             return ;
        }

        debug_mmio->info.type       = 0;
        debug_mmio->info.id         = 0;
        debug_mmio->info.priv_data  = debug_dev->zx->adapter;
        debug_mmio->info.dev        = debug_dev;

        zx_vsprintf(debug_mmio->info.name, "info");

        debug_mmio->regs.node_dentry = debugfs_create_file(debug_mmio->info.name, 0444, debug_mmio->mmio_root, &(debug_mmio->info), &debugfs_mmio_info_fops);

        if (debug_mmio->regs.node_dentry == NULL)
        {
             zx_error("Failed to create debugfs node %s\n", debug_mmio->info.name);
             return ;
        }
    }
    else
    {
        zx_error("debugfs: failed to create debugfs root directory.\n");
    }
}

static int zx_debugfs_crtc_show(struct seq_file *s, void *unused)
{
    zx_debugfs_node_t *node = (zx_debugfs_node_t *)(s->private);

    return  zx_debugfs_crtc_dump(s, node->priv_data);
}

static int zx_debugfs_crtc_open(struct inode *inode, struct file *file)
{
    return single_open(file, zx_debugfs_crtc_show, inode->i_private);
}

static const struct file_operations debugfs_crtcs_fops = {
        .open       = zx_debugfs_crtc_open,
        .read       = seq_read,
        .llseek     = seq_lseek,
        .release    = single_release,
};

void zx_debugfs_init_crtcs_nodes(zx_debugfs_device_t *debug_dev)
{
    struct drm_device *drm_dev = debug_dev->zx->drm_dev;
    zx_debugfs_crtcs_t *crtcs = &debug_dev->crtcs;
    struct drm_crtc *crtc;
    int i = 0;

    crtcs->node_num = drm_dev->mode_config.num_crtc;
    if (!crtcs->node_num)
    {
        return;
    }

    crtcs->crtcs_root = debugfs_create_dir("crtcs", debug_dev->debug_root);
    if (!crtcs->crtcs_root)
    {
        zx_error("Failed to create dir for debugfs crtcs root.\n");
        goto FAIL;
    }

    crtcs->debug_dev = debug_dev;

    crtcs->crtcs_nodes = zx_calloc(sizeof(struct zx_debugfs_node) * crtcs->node_num);
    if (!crtcs->crtcs_nodes)
    {
        zx_error("Failed to alloc mem for debugfs crtcs nodes.\n");
        goto  FAIL;
    }

    zx_drm_for_each_crtc(crtc, drm_dev)
    {
        struct dentry *node_dentry = NULL;
        zx_crtc_t *zx_crtc = NULL;

        zx_crtc = to_zx_crtc(crtc);

        crtcs->crtcs_nodes[i].type = DEBUGFS_NODE_DRM_CRTC;
        crtcs->crtcs_nodes[i].id = zx_crtc->pipe;

        crtcs->crtcs_nodes[i].dev = debug_dev;
        zx_vsprintf(crtcs->crtcs_nodes[i].name, "IGA%d", (zx_crtc->pipe + 1));

        crtcs->crtcs_nodes[i].priv_data = zx_crtc;

        node_dentry = debugfs_create_file(crtcs->crtcs_nodes[i].name,
                                          0444, crtcs->crtcs_root,
                                          &crtcs->crtcs_nodes[i], &debugfs_crtcs_fops);
        if (!node_dentry)
        {
            zx_error("Failed to create debugfs file for crtc node.\n");
            goto FAIL;
        }

        crtcs->crtcs_nodes[i].node_dentry = node_dentry;

        i++;
    }

    return;

FAIL:
    if (crtcs->crtcs_root)
    {
        debugfs_remove_recursive(crtcs->crtcs_root);
    }

    if (crtcs->crtcs_nodes)
    {
        zx_free(crtcs->crtcs_nodes);
        crtcs->crtcs_nodes = NULL;
    }
}

/* function: Trigger HPD redetection on connector
 *
 * int_type:
 *
 * DP_HPD_IN: 1
 * DP_HPD_HDMI_OUT: 2
 * DP_HPD_DP_OUT: 3
 *
 * trigger HPD on an existing connector by echoing (output_type << 4 | int_type)
 *
 *  echo 0x80001 > /sys/kernel/debug/dri/0/zx/connectors/HDMI-A-1/hpd_trigger
 *
 * This function can perform HDMI HPD unplug:
 *
 *	echo 0x80002 > /sys/kernel/debug/dri/0/zx/connectors/HDMI-A-1/hpd_trigger
 *
 */
static ssize_t zx_debugfs_trig_hotplug(struct file *f, const char __user *buf, size_t size, loff_t *pos)
{
    zx_debugfs_node_t *node = file_inode(f)->i_private;
    zx_card_t *zx_card = node->dev->zx;
    zx_connector_t *zx_conn = (zx_connector_t *)node->priv_data;
    int ret, val;

    if (!zx_conn)
    {
        return -EINVAL;
    }

    if (size == 0)
    {
        return -EINVAL;
    }


    ret = kstrtoint_from_user(buf, size, 0, &val);
    if (ret)
    {
        return ret;
    }

    if (zx_conn->output_type != (val >> 4))
    {
        return -EINVAL;
    }

    zx_handle_hpd_from_debugfs(zx_conn, val & 0xf);

    *pos += size;

    return size;
}

static const struct file_operations debugfs_conn_hpd_fops = {
    .owner = THIS_MODULE,
    .write = zx_debugfs_trig_hotplug,
    .llseek = default_llseek
};

#define HPD_DEBUGFS_OUTPUT_MASK \
    (BIT(DRM_MODE_CONNECTOR_HDMIA) | BIT(DRM_MODE_CONNECTOR_DisplayPort))

static int zx_debugfs_conn_show(struct seq_file *s, void *unused)
{
    zx_debugfs_node_t *node = (zx_debugfs_node_t *)(s->private);

    return  zx_debugfs_connector_dump(s, node->priv_data);
}

static int zx_debugfs_conn_open(struct inode *inode, struct file *file)
{
    return single_open(file, zx_debugfs_conn_show, inode->i_private);
}

static const struct file_operations debugfs_conn_info_fops = {
        .open       = zx_debugfs_conn_open,
        .read       = seq_read,
        .llseek     = seq_lseek,
        .release    = single_release,
};

static const struct {
    char *name;
    unsigned int conn_mask;
    const struct file_operations *fops;
} connector_debugfs_entries[] = {
    {"hpd_trigger", HPD_DEBUGFS_OUTPUT_MASK, &debugfs_conn_hpd_fops},
    {"info", 0, &debugfs_conn_info_fops}
};

void zx_debugfs_init_connectors_nodes(zx_debugfs_device_t *debug_dev)
{
    struct drm_device *drm_dev = debug_dev->zx->drm_dev;
    zx_debugfs_conn_t *connectors = &debug_dev->connectors;
    struct drm_connector *connector;
    zx_connector_t *zx_conn = NULL;
    int i = 0, j = 0;
    ZX_DEFINE_DRM_CONN_ITER(conn_iter);

    connectors->node_num = drm_dev->mode_config.num_connector;
    if (!connectors->node_num)
    {
        return;
    }

    connectors->conn_root = debugfs_create_dir("connectors", debug_dev->debug_root);
    if (!connectors->conn_root)
    {
        zx_error("Failed to create dir for debugfs connectors root.\n");
        goto FAIL;
    }

    connectors->debug_dev = debug_dev;
    connectors->conn_dirs = zx_calloc(sizeof(struct dentry *) * connectors->node_num);
    if (!connectors->conn_dirs)
    {
        zx_error("Failed to alloc mem for debugfs connectors dirs.\n");
        goto  FAIL;
    }

    connectors->conn_nodes = zx_calloc(sizeof(struct zx_debugfs_node *) * connectors->node_num);
    if (!connectors->conn_nodes)
    {
        zx_error("Failed to alloc mem for debugfs connectors dirs.\n");
        goto  FAIL;
    }

    zx_drm_connector_list_iter_begin(drm_dev, &conn_iter);

    zx_drm_for_each_connector(connector, drm_dev, &conn_iter)
    {
        zx_conn = to_zx_connector(connector);

        connectors->conn_dirs[i] = debugfs_create_dir(connector->name, connectors->conn_root);
        if (!connectors->conn_dirs[i])
        {
            zx_error("Failed to create dir for debugfs connectors dirs.\n");
            goto FAIL_ITER_CLEAN;
        }

        connectors->conn_nodes[i] = zx_calloc(sizeof(struct zx_debugfs_node) * ARRAY_SIZE(connector_debugfs_entries));
        if (!connectors->conn_nodes[i])
        {
            zx_error("Failed to alloc mem for debugfs connectors nodes.\n");
            goto FAIL_ITER_CLEAN;
        }

        for (j = 0; j < ARRAY_SIZE(connector_debugfs_entries); j++)
        {
            struct dentry* node_dentry = NULL;

            if (connector_debugfs_entries[j].conn_mask != 0 &&
                !(connector_debugfs_entries[j].conn_mask & BIT(connector->connector_type)))
            {
                continue;
            }

            connectors->conn_nodes[i][j].type = DEBUGFS_NODE_DRM_CONNECTOR;
            connectors->conn_nodes[i][j].id = i;
            connectors->conn_nodes[i][j].dev = debug_dev;

            zx_vsprintf(connectors->conn_nodes[i][j].name, connector_debugfs_entries[j].name);

            connectors->conn_nodes[i][j].priv_data = zx_conn;

            node_dentry = debugfs_create_file(connectors->conn_nodes[i][j].name,
                                            0644, connectors->conn_dirs[i],
                                            &connectors->conn_nodes[i][j], connector_debugfs_entries[j].fops);
            if (!node_dentry)
            {
                zx_error("Failed to create debugfs file for connector %s node.\n", connectors->conn_nodes[i][j].name);
                goto FAIL_ITER_CLEAN;
            }
            connectors->conn_nodes[i][j].node_dentry = node_dentry;
        }

        i++;
    }

    zx_drm_connector_list_iter_end(&conn_iter);

    return;

FAIL_ITER_CLEAN:
    zx_drm_connector_list_iter_end(&conn_iter);

FAIL:
    if (connectors->conn_root)
    {
        debugfs_remove_recursive(connectors->conn_root);
    }

    if (connectors->conn_dirs)
    {
        zx_free(connectors->conn_dirs);
        connectors->conn_dirs = NULL;
    }

    if (connectors->conn_nodes)
    {
        for (i = 0; i < connectors->node_num; i++)
        {
            zx_free(connectors->conn_nodes[i]);
        }

        zx_free(connectors->conn_nodes);
        connectors->conn_nodes = NULL;
    }
}

zx_debugfs_device_t* zx_debugfs_create(zx_card_t *zx, struct dentry *minor_root)
{
    zx_debugfs_device_t *dev;
    int i=0;
    zx_heap_info_t  *heap;
    dev = zx_calloc(sizeof(struct zx_debugfs_device));
    if (!dev)
    {
        return NULL;
    }

    dev->zx        = zx;
    dev->lock       = zx_create_mutex();
    INIT_LIST_HEAD(&(dev->node_list));
    INIT_LIST_HEAD(&(dev->device_node_list));

    dev->debug_root = debugfs_create_dir("zx", minor_root);
    if (!dev->debug_root)
    {
        zx_error("debugfs: failed to create debugfs root directory.\n");
        return NULL;
    }


    dev->device_root = debugfs_create_dir("devices", dev->debug_root);

    dev->allocation_root = debugfs_create_dir("allocations", dev->debug_root);

/*HEAP info*/
    heap = &dev->heap_info;
    heap->heap_dir = debugfs_create_dir("heaps", dev->debug_root);
    if (!heap->heap_dir)
    {
        zx_error("debugfs: failed to create debugfs root directory.\n");
        return NULL;
    }

    for(i = 0; i<DEBUGFS_HEAP_NUM; i++)
    {
        heap->heap[i].type       = DEBUGFS_NODE_HEAP;
        heap->heap[i].id         = i;
        heap->heap[i].priv_data  = dev->zx->adapter;
        heap->heap[i].dev        = dev;

        switch(i){
            case 0:
                zx_vsprintf(heap->heap[i].name, "heap_invalid");
                break;
            case 1:
                zx_vsprintf(heap->heap[i].name, "heap_local");
                break;
            case 2:
                zx_vsprintf(heap->heap[i].name, "heap_pcie_unsnoop");
                break;
            case 3:
                zx_vsprintf(heap->heap[i].name, "heap_pcie_snoop");
                break;
            default:
                break;
        }

        heap->heap_dirs[i] = debugfs_create_dir(heap->heap[i].name, heap->heap_dir);
        if (!heap->heap_dirs[i])
        {
            zx_error("Failed to create debugfs node %s\n", heap->heap[i].name);
            return NULL;
        }

        heap->heap[i].node_dentry = debugfs_create_file("info", 0444, heap->heap_dirs[i], &(heap->heap[i]), &debugfs_node_fops);
        if (heap->heap[i].node_dentry == NULL)
        {
            zx_error("Failed to create debugfs node %s info\n", heap->heap[i].name);
            return NULL;
        }
    }

    dev->info.type       = DEBUGFS_NODE_INFO;
    dev->info.priv_data  = dev->zx->adapter;
    dev->info.dev        = dev;

    zx_vsprintf(dev->info.name, "info");

    dev->info.node_dentry = debugfs_create_file(dev->info.name, 0664, dev->debug_root, &(dev->info), &debugfs_node_fops);

    if (dev->info.node_dentry == NULL)
    {
        zx_error("Failed to create debugfs node %s\n", dev->info.name);
        return NULL;
    }

    dev->memtrack.type       = DEBUGFS_NODE_MEMTRACK;
    dev->memtrack.priv_data  = dev->zx->adapter;
    dev->memtrack.dev        = dev;
    dev->memtrack.id         = -1;

    zx_vsprintf(dev->memtrack.name, "memtrack");

    dev->memtrack.node_dentry = debugfs_create_file(dev->memtrack.name, 0664, dev->debug_root, &(dev->memtrack), &debugfs_node_fops);

    if (dev->memtrack.node_dentry == NULL)
    {
        zx_error("Failed to create debugfs node %s\n", dev->memtrack.name);
        return NULL;
    }

    dev->vidsch.type       = DEBUGFS_NODE_VIDSCH;
    dev->vidsch.priv_data  = dev->zx->adapter;
    dev->vidsch.dev        = dev;

    zx_vsprintf(dev->vidsch.name, "vidsch");

    dev->vidsch.node_dentry = debugfs_create_file(dev->vidsch.name, 0664, dev->debug_root, &(dev->vidsch), &debugfs_node_fops);

    if (dev->vidsch.node_dentry == NULL)
    {
        zx_error("Failed to create debugfs node %s\n", dev->vidsch.name);
        return NULL;
    }

    // here to create  displayinfo node to get info
    dev->displayinfo.type       = 0;
    dev->displayinfo.priv_data  = dev->zx->adapter;
    dev->displayinfo.dev        = dev;

    zx_vsprintf(dev->displayinfo.name, "displayinfo");

    dev->displayinfo.node_dentry = debugfs_create_file(dev->displayinfo.name, 0664, dev->debug_root, &(dev->displayinfo), &debugfs_node_displayinfo_fops);

    if (dev->displayinfo.node_dentry == NULL)
    {
        zx_error("Failed to create debugfs node %s\n", dev->displayinfo.name);
        return NULL;
    }


    dev->psr.type       = 0;
    dev->psr.priv_data  = dev->zx->adapter;
    dev->psr.dev        = dev;

    zx_vsprintf(dev->displayinfo.name, "psr");

    dev->displayinfo.node_dentry = debugfs_create_file(dev->displayinfo.name, 0664, dev->debug_root, &(dev->psr), &debugfs_node_psr_fops);

    dev->debugbus.type      = DEBUGFS_NODE_DEBUGBUS;
    dev->debugbus.priv_data = dev->zx->adapter;
    dev->debugbus.dev       = dev;

    zx_vsprintf(dev->debugbus.name, "debugbus");

    dev->debugbus.node_dentry = debugfs_create_file(dev->debugbus.name, 0664, dev->debug_root, &(dev->debugbus), &debugfs_node_fops);

    if (dev->debugbus.node_dentry == NULL)
    {
        zx_error("Failed to create debugfs node %s\n", dev->debugbus.name);
    }

    // create file to enable/disable usermode trace.
    // NOTE: kmd trace is controlled by /sys/kernel/debug/tracing/events/zxgfx/enable
    dev->umd_trace.type       = 0;
    dev->umd_trace.priv_data  = dev->zx->adapter;
    dev->umd_trace.dev        = dev;

    zx_vsprintf(dev->umd_trace.name, "umd_trace");

    dev->umd_trace.node_dentry = debugfs_create_file(dev->umd_trace.name, 0664, dev->debug_root, &(dev->umd_trace), &debugfs_node_umd_trace_fops);

    if (dev->umd_trace.node_dentry == NULL)
    {
        zx_error("Failed to create debugfs node %s\n", dev->umd_trace.name);
        return NULL;
    }

/*
    // create file to test physical address ~ +4k.
    dev->test_mem.type       = 0;
    dev->test_mem.priv_data  = dev->zx->adapter;
    dev->test_mem.dev        = dev;

    zx_vsprintf(dev->test_mem.name, "test_mem");

    dev->test_mem.node_dentry = debugfs_create_file(dev->test_mem.name, 0664, dev->debug_root, &(dev->test_mem), &debugfs_node_test_mem_fops);

    if (dev->test_mem.node_dentry == NULL)
    {
        zx_error("Failed to create debugfs node %s\n", dev->test_mem.name);
        return NULL;
    }
*/

    zx_debugfs_init_mmio_nodes(dev);

    zx_debugfs_init_crtcs_nodes(dev);

    zx_debugfs_init_connectors_nodes(dev);

    return dev;
}

int zx_debugfs_destroy(zx_debugfs_device_t* dev)
{
    zx_debugfs_node_t *node1 = NULL;
    zx_debugfs_node_t *node2 = NULL;
    int i = 0;

    if (dev == NULL)
    {
        zx_error("destroy a null dubugfs dev\n");
        return -1;
    }

    if (dev->lock)
    {
        zx_destroy_mutex(dev->lock);
    }

    if (dev->debug_root)
    {
        debugfs_remove_recursive(dev->debug_root);
    }

    list_for_each_entry_safe(node1, node2, &(dev->node_list), list_item)
    {
        zx_free(node1);
    }

    if (dev->crtcs.crtcs_nodes)
    {
        zx_free(dev->crtcs.crtcs_nodes);
    }

    if (dev->connectors.conn_dirs)
    {
        zx_free(dev->connectors.conn_dirs);
    }

    if (dev->connectors.conn_nodes)
    {
        for (i = 0; i < dev->connectors.node_num; i++)
        {
            zx_free(dev->connectors.conn_nodes[i]);
        }

        zx_free(dev->connectors.conn_nodes);
    }

    zx_free(dev);

    return 0;
}


static int zx_debugfs_vm_info_show(struct seq_file *s, void *unused)
{
    zx_device_debug_info_t *node = (zx_device_debug_info_t *)(s->private);
    zx_debugfs_device_t *dev = node->debugfs_dev;
    zx_card_t *zx = dev->zx;
    struct os_seq_file seq_file={0};
    struct os_printer seq_p = zx_seq_file_printer(&seq_file);
    void *gpu_device = NULL;

    seq_file.seq_file = s;

    if (node->hDevice)
    {
        spin_lock(&zx->dev_table_lock);
        gpu_device = idr_find(&zx->dev_idr, node->hDevice);
        spin_unlock(&zx->dev_table_lock);

        if (!gpu_device && node->hDevice)
            return -ENODEV;
    }

    zx_core_interface->debugfs_dump(&seq_p, zx->adapter, DEBUGFS_NODE_VMA, gpu_device);

    return 0;
}

static int zx_debugfs_vm_info_open(struct inode *inode, struct file *file)
{
    return single_open(file, zx_debugfs_vm_info_show, inode->i_private);
}

static const struct file_operations debugfs_vm_info_fops = {
        .open       = zx_debugfs_vm_info_open,
        .read       = seq_read,
        .llseek     = seq_lseek,
        .release    = single_release,
};


static int zx_debugfs_vm_zone_info_show(struct seq_file *s, void *unused)
{
    zx_device_debug_info_t *node = (zx_device_debug_info_t *)(s->private);
    zx_debugfs_device_t *dev = node->debugfs_dev;
    zx_card_t *zx = dev->zx;
    struct os_seq_file seq_file={0};
    struct os_printer seq_p = zx_seq_file_printer(&seq_file);
    void *gpu_device = NULL;

    seq_file.seq_file = s;

    if (node->hDevice)
    {
        spin_lock(&zx->dev_table_lock);
        gpu_device = idr_find(&zx->dev_idr, node->hDevice);
        spin_unlock(&zx->dev_table_lock);

        if (!gpu_device && node->hDevice)
            return -ENODEV;
    }

    zx_core_interface->debugfs_dump(&seq_p, zx->adapter, DEBUGFS_NODE_VMA_ZONE, gpu_device);

    return 0;
}

static int zx_debugfs_vm_zone_info_open(struct inode *inode, struct file *file)
{
    return single_open(file, zx_debugfs_vm_zone_info_show, inode->i_private);
}

static const struct file_operations debugfs_vm_zone_info_fops = {
        .open       = zx_debugfs_vm_zone_info_open,
        .read       = seq_read,
        .llseek     = seq_lseek,
        .release    = single_release,
};

static int zx_debugfs_device_show(struct seq_file *s, void *unused)
{
    zx_device_debug_info_t *node = (zx_device_debug_info_t *)(s->private);

    seq_printf(s, "pid = %lu\n", node->user_pid);
    seq_printf(s, "handle = 0x%x\n", node->hDevice);
    seq_printf(s, "resident_cnt = %d\n", node->resident_cnt);
    seq_printf(s, "resident_cache_hit = %d\n", node->resident_list_cache_hit);
    seq_printf(s, "resident_cache_miss = %d\n", node->resident_list_cache_miss);

    return 0;
}

static int zx_debugfs_device_open(struct inode *inode, struct file *file)
{
    return single_open(file, zx_debugfs_device_show, inode->i_private);
}

static const struct file_operations debugfs_device_fops = {
        .open       = zx_debugfs_device_open,
        .read       = seq_read,
        .llseek     = seq_lseek,
        .release    = single_release,
};

zx_device_debug_info_t* zx_debugfs_add_device_node(zx_debugfs_device_t* dev, int id, unsigned int handle)
{
    zx_device_debug_info_t *node  = zx_calloc(sizeof(zx_device_debug_info_t));

    zx_device_debug_info_t *node1   = NULL;
    zx_device_debug_info_t *node2   = NULL;
    struct dentry *parent = dev->device_root;

    node->id                = id;
    node->debugfs_dev       = dev;
    node->hDevice           = handle;

    zx_vsprintf(node->name, "%08x", handle);
    node->dentry = debugfs_create_dir(node->name, parent);
    if (node->dentry) {
        node->info = debugfs_create_file("info", 0664, node->dentry, node, &debugfs_device_fops);
        if (!node->info)
        {
            zx_error("Failed to create debugfs node %s\n", node->name);
            zx_free(node);

            return NULL;
        }

        node->vm_info = debugfs_create_file("vm_info", 0664, node->dentry, node, &debugfs_vm_info_fops);
        if (!node->vm_info)
        {
            zx_error("Failed to create debugfs node %s\n", node->name);
            zx_free(node);

            return NULL;
        }

        node->vm_zone_info = debugfs_create_file("vm_zone_info", 0664, node->dentry, node, &debugfs_vm_zone_info_fops);
        if (!node->vm_info)
        {
            zx_error("Failed to create debugfs node %s\n", node->name);
            zx_free(node);

            return NULL;
        }
    } else {
            zx_error("Failed to create debugfs dir %s\n", node->name);
            zx_free(node);
            return NULL;
    }

    zx_mutex_lock(dev->lock);
    list_add_tail(&(node->list_item), &(dev->device_node_list));
    zx_mutex_unlock(dev->lock);

    return node;
}

int zx_debugfs_remove_device_node(zx_debugfs_device_t* dev, zx_device_debug_info_t *dnode)
{
    zx_device_debug_info_t *node1 = NULL;
    zx_device_debug_info_t *node2 = NULL;

    if (dnode == NULL)
    {
        return 0;
    }

    zx_mutex_lock(dev->lock);

    list_for_each_entry_safe(node1, node2, &(dev->device_node_list), list_item)
    {
        if(node1 && (node1->hDevice == dnode->hDevice))
        {
            debugfs_remove_recursive(node1->dentry);
            list_del(&(node1->list_item));

            zx_free(node1);
            break;
        }
    }

    zx_mutex_unlock(dev->lock);

    return 0;
}

int zx_debugfs_init(zx_card_t *zx)
{
    int ret = 0;

    zx->debugfs_dev = zx_debugfs_create(zx, zx->drm_dev->primary->debugfs_root);
    if(!zx->debugfs_dev)
        return -1;

    zx->reserved_dev = zx_debugfs_add_device_node(zx->debugfs_dev, zx_get_current_pid(), 0);
    if(!zx->reserved_dev)
    {
        zx_debugfs_destroy(zx->debugfs_dev);
        ret  = -1;
    }

    return ret;
}
