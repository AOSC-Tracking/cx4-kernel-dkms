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
#include "os_interface.h"
#include "zx_driver.h"
#include "zx_ioctl.h"
#include "zx_version.h"
#include "zx_debugfs.h"

core_interface_t *zx_core_interface = NULL;
//drm device no need to call register_chrdev.
static int __init zx_init(void)
{
    int ret = -ENOMEM;

#if DRM_VERSION_CODE >= KERNEL_VERSION(5, 17, 0)
    if (drm_firmware_drivers_only())
    {
        return -EINVAL;
    }
#endif

    zx_core_interface = krnl_get_core_interface();

    zx_info("%s\n", DRIVER_VENDOR);
    //zx_info("Version: %0d.%02d.%02d%s Build on: %s\n", DRIVER_MAJOR, DRIVER_MINOR, DRIVER_PATCHLEVEL, DRIVER_BRANCH, DRIVER_DATE);
    zx_info("Version: %02x.%02x.%02x%s Build on: %s\n", DRIVER_MAJOR, DRIVER_MINOR, DRIVER_PATCHLEVEL, DRIVER_CLASS, DRIVER_DATE);

#if ZX_MALLOC_TRACK | ZX_ALLOC_PAGE_TRACK | ZX_MAP_PAGES_TRACK | ZX_MAP_IO_TRACK
    zx_mem_track_init();
#endif
    ret = zx_register_driver();

    if(ret)
    {
        zx_error("register_driver() failed in zx_init. ret:%x.\n", ret);
    }

    return 0;
}

static void __exit zx_exit(void)
{
    zx_unregister_driver();

#if ZX_MALLOC_TRACK | ZX_ALLOC_PAGE_TRACK | ZX_MAP_PAGES_TRACK | ZX_MAP_IO_TRACK
    zx_mem_track_list_result();
#endif

    zx_info("exit driver.\n");
}

module_init(zx_init);
module_exit(zx_exit);
MODULE_LICENSE("GPL");
MODULE_VERSION(DRIVER_VERSION_CHAR);
MODULE_DESCRIPTION("Zhaoxin DRM PRO Driver");
