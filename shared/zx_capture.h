//*****************************************************************************
//  Copyright (c) 2019 Shanghai Zhaoxin Semiconductor Co., Ltd.
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
#ifndef __ZX_CAPTURE_H__
#define __ZX_CAPTURE_H__


#define ZX_CAPTURE_SKIP_FRAME    (0x1 << 0)
#define ZX_CAPTURE_SET_BUFFER    (0x1 << 1)

typedef enum _zx_capture_id_t
{
    ZX_CAPTURE_VIP1       = 0,
    ZX_CAPTURE_VIP2       = 1,
    ZX_CAPTURE_VIP3       = 2,
    ZX_CAPTURE_VIP4       = 3,
    ZX_CAPTURE_WB1        = 4,
    ZX_CAPTURE_WB2        = 5,
    ZX_CAPTURE_WB3        = 6,
    ZX_CAPTURE_WB4        = 7,
}zx_capture_id_t;

typedef enum _zx_capture_op_t
{
    ZX_CAPTURE_OP_INIT                    = 0,
    ZX_CAPTURE_OP_DEINIT                  = 1,
    ZX_CAPTURE_OP_ENABLE                  = 2,
    ZX_CAPTURE_OP_DISABLE                 = 3,
    ZX_CAPTURE_OP_QUERY_CAPS              = 4,
    ZX_CAPTURE_OP_SET_MODE                = 5,
    ZX_CAPTURE_OP_SET_BUFF                = 6,
    ZX_CAPTURE_OP_SET_CALLBACK            = 7,
    ZX_CAPTURE_OP_QUERY_CHIPINFO          = 8,
    ZX_CAPTURE_OP_GET_FRONT_MODE          = 9,
    ZX_CAPTURE_OP_SET_PREFER_MODE         = 10,
}zx_capture_op_t;

enum
{
    ZX_VIP_CARD_AVD7611       = 0,
};

enum
{
    ZX_WB_MODE_P2P            = 0,
};

enum
{
    ZX_WB_FMT_RGB888                  = 0,
    ZX_VIP_FMT_RGB444_24BIT_SDR       = 1,
    ZX_VIP_FMT_YCBCR444_24BIT_SDR     = 2,
    ZX_VIP_FMT_YCBCR422_8BIT_SDR_ES   = 3,
    ZX_VIP_FMT_YCBCR422_8BIT_DDR_ES   = 4,
};

typedef struct
{
    unsigned int xRes;
    unsigned int yRes;
    unsigned int refreshrate;
    unsigned int fmt;
}zx_vip_mode_t;

typedef struct
{
    zx_capture_op_t op;

    union
    {
        struct
        {
            unsigned long long fbAddr;
        }set_buffer_param;

        struct
        {
            unsigned int bByPass;
            unsigned int srcX;
            unsigned int srcY;
            unsigned int dstX;
            unsigned int dstY;
            unsigned int bDoubleBuffer;
            unsigned int bUpdateImme;
            unsigned int mode;
            unsigned int outFmt;
        }set_mode_param;

        struct
        {
            unsigned int width;
            unsigned int height;
            unsigned int refrate;
            unsigned int capture_fmt;
        }query_caps_param;

        struct
        {
            void (*cb)(zx_capture_id_t id, void *data, unsigned int flags);
            void *cb_data;
        }set_callback_param;

    };

}wb_op_params_t;

typedef struct
{
    zx_capture_op_t op;

    union
    {
        struct
        {
            unsigned int fbNum;
        }init_param;

        struct
        {
            unsigned int fbIdx;
            unsigned long long  fbAddr;
        }set_buffer_param;

        struct
        {
            unsigned int fmt;
            unsigned int chip;
            unsigned int xres;
            unsigned int yres;
            unsigned int refreshrate;
        }set_mode_param;

        struct
        {
            unsigned int modeNum;
            zx_vip_mode_t *mode;
        }query_caps_param;

        struct
        {
            unsigned int vipCard;
        }query_chipinfo_param;

        struct
        {
            unsigned int hasInput;
            zx_vip_mode_t mode;
        }get_front_mode_param;

        struct
        {
            zx_vip_mode_t mode;
        }set_prefer_mode_param;

        struct
        {
            void (*cb)(zx_capture_id_t id, void *data, unsigned int flags);
            void *cb_data;
        }set_callback_param;
    };

}vip_op_params_t;

typedef int (*PFN_ZX_CAPTURE_OPS_T)(void *pdata, zx_capture_id_t id, void *params);
void zx_krnl_get_capture_interface(unsigned int id, void **pdata, PFN_ZX_CAPTURE_OPS_T *interface);

#endif
