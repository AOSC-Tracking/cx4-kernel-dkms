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
#include "zx."
#include "zx_driver.h"
#include "os_interface.h"


/* for elite in ahb bus, only use it to check if current card is primary(0x01) */
int zx_get_command_status16(void *dev, unsigned short *command)
{
    *command = 0x01;
    return 0;
}

int zx_get_command_status32(void *dev, unsigned int *command)
{
    *command = 0x01;
    return 0;
}

/* useless for elite in ahb bus */
int zx_write_command_status16(void *dev, unsigned short command)
{
    return 0;
}

/* useless for elite in ahb bus */
int zx_write_command_status32(void *dev, unsigned int command)
{
    return 0;
}

/* useless for elite in ahb bus */
int zx_get_bar1(void *dev, unsigned int *bar1)
{
    *bar1 = 0x0;
    return 0;
}

/* useless for elite in ahb bus */
int zx_get_rom_save_addr(void *dev, unsigned int *romsave)
{
    *romsave = 0x0;
    return 0;
}

/* useless for elite in ahb bus */
int zx_write_rom_save_addr(void *dev, unsigned int romsave)
{
    return 0;
}

int zx_get_platform_config(void *dev, const char* config_name, int *buffer, int length)
{

    return 0;
}
void zx_get_bus_config(void *dev, bus_config_t *bus)
{
    bus->device_id         = 0x3D00;//E3K
    bus->vendor_id         = 0x1d17;
    bus->command           = 0x01;
    bus->status            = 0;
    bus->revision_id       = 0;
    bus->prog_if           = 0;
    bus->sub_class         = 0;
    bus->base_class        = 0;
    bus->cache_line_size   = 0;
    bus->latency_timer     = 0;
    bus->header_type       = 0;
    bus->bist              = 0;
    bus->sub_sys_vendor_id = 0;
    bus->sub_sys_id        = 0;
    bus->link_status       = 0;

    bus->mem_start_addr[0] = 10*1024*1024;
    bus->mem_end_addr[0]   = 266*1024*1024;

    bus->reg_start_addr[0] = 0;
    bus->reg_start_addr[1] = 0x700*1024*1024;
    bus->reg_start_addr[2] = 0;
    bus->reg_start_addr[3] = 0;
    bus->reg_start_addr[4] = 0;

    bus->reg_end_addr[0]   = 0;
    bus->reg_end_addr[1]   = 0x800*1024*1024ul;//total 256M local reserved in linux kernel
    bus->reg_end_addr[2]   = 0;
    bus->reg_end_addr[3]   = 0;
    bus->reg_end_addr[4]   = 0;
}

/* useless for elite in ahb bus */
unsigned long zx_get_rom_start_addr(void *dev)
{
    return 0;
}

/* useless for elite in ahb bus */
void zx_init_bus_id(zx_card_t *zx)
{
}

static zx_card_t *zx_dev = NULL;

int zx_register_driver(void)
{
    zx_card_t *zx = NULL;

    int result;

    zx = zx_calloc(sizeof(zx_card_t));
    if (!zx) {
        zx_error("register driver failed!\N");
    }
    zx_memset(zx, 0, sizeof(zx_card_t));

    zx_info("zx HW_NULL init driver\n");

    zx_fb_mode = "640x480-32@60";

    result = zx_card_init(zx, NULL);
    zx_dev = zx;
    return result;
}

void zx_unregister_driver(void)
{
    zx_card_t *zx = zx_dev;

    zx_card_deinit(zx);
    zx_free(zx);
    zx_dev = NULL;
}

int zx_register_interrupt(zx_card_t *zx, void *isr)
{
    zx_info("zx interrupt irq :NULL\n");

    return 0;
}

void zx_unregister_interrupt(zx_card_t *zx)
{
}
