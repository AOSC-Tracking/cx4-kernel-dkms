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
#ifndef  _ZX_DISP_H
#define  _ZX_DISP_H

#include "zx_kms.h"
#include "zx_driver.h"
#if DRM_VERSION_CODE >= KERNEL_VERSION(6, 0, 0)
#include <drm/drm_blend.h>
#endif

#if defined(CONFIG_DRM_PANIC)
#include <drm/drm_panic.h>
#endif

/* same to  CBIOS_REGISTER_TYPE */
enum
{
    /* CR_SET = 0,     //CR registers to be set, used in s3postreg */
    /* SR_SET,         //SR registers to be set */
    /* GR_SET,         //GR registers to be set */
    CR=0,                /*CR registers           */
    SR,                /*SR registers           */
    AR,
    GR,
    MISC,
    CR_X,
    SR_X,
    CR_B,               /* B-set of CR */
    CR_C,              /* C-set of CR */
    CR_D,               /* D-set of CR */
    CR_T,               /* CR on IGA3  */
    SR_T,               /* SR on IGA3  */
    SR_B,
    CR_D_0,
    CR_D_1,
    CR_D_2,
    CR_D_3,
    RESERVED=0xFF
};

enum
{
    DP_HPD_NONE = 0,
    DP_HPD_IN,
    DP_HPD_HDMI_OUT,
    DP_HPD_DP_OUT,
    DP_HPD_IRQ,
};

#define GET_LAST_BIT(N) ((~((N)-1))&(N))

//interrupt bit used by SW, it's irrelevant to HW register define, DO NOT set them to register directly
#define INT_VSYNC1  (1ULL << 0)
#define INT_VSYNC2  (1ULL << 1)
#define INT_VSYNC3  (1ULL << 2)
#define INT_VSYNC4  (1ULL << 3)
#define INT_VSYNCS  (INT_VSYNC1 | INT_VSYNC2 | INT_VSYNC3 | INT_VSYNC4)

static const unsigned int vsync_int_tbl[MAX_CORE_CRTCS] = {
    INT_VSYNC1,
    INT_VSYNC2,
    INT_VSYNC3,
    INT_VSYNC4,
};

#define INT_DP1     (1ULL << 4)
#define INT_DP2     (1ULL << 5)
#define INT_DP3     (1ULL << 6)
#define INT_DP4     (1ULL << 7)
#define INT_HOTPLUG  (INT_DP1 | INT_DP2 | INT_DP3 | INT_DP4)

#define INT_HDCODEC  (1ULL << 8)

#define INT_FENCE           (1ULL << 9)
#define INT_FE_HANG_VD0     (1ULL << 10)
#define INT_BE_HANG_VD0     (1ULL << 11)
#define INT_FE_ERROR_VD0    (1ULL << 12)
#define INT_BE_ERROR_VD0    (1ULL << 13)
#define INT_FE_HANG_VD1     (1ULL << 14)
#define INT_BE_HANG_VD1     (1ULL << 15)
#define INT_FE_ERROR_VD1    (1ULL << 16)
#define INT_BE_ERROR_VD1    (1ULL << 17)
#define INT_FE_HANG_VD2     (1ULL << 18)
#define INT_BE_HANG_VD2     (1ULL << 19)
#define INT_FE_ERROR_VD2    (1ULL << 20)
#define INT_BE_ERROR_VD2    (1ULL << 21)

#define INT_THERMAL_SENSOR      (1ULL << 22)
#define INT_PAGE_FAULT          (1ULL << 23)

#define INT_DVFS_3D_CHECKSIZE   (1ULL << 24)
#define INT_DVFS_3D_CHANGE      (1ULL << 25)
#define INT_DVFS_VCP0_CHECKSIZE (1ULL << 26)
#define INT_DVFS_VCP0_CHANGE    (1ULL << 27)
#define INT_DVFS_VCP1_CHECKSIZE (1ULL << 28)
#define INT_DVFS_VCP1_CHANGE    (1ULL << 29)
#define INT_DVFS_VPP_CHECKSIZE  (1ULL << 30)
#define INT_DVFS_VPP_CHANGE     (1ULL << 31)
#define INT_C3D_REFUCE          (1ULL << 32)

#define INT_DVFS   (INT_DVFS_3D_CHECKSIZE | INT_DVFS_3D_CHANGE | INT_DVFS_VCP0_CHECKSIZE | INT_DVFS_VCP0_CHANGE | INT_DVFS_VCP1_CHECKSIZE | INT_DVFS_VCP1_CHANGE | INT_DVFS_VPP_CHECKSIZE | INT_DVFS_VPP_CHANGE)


// In the process of encoding I frame, hw will send an interrupt,but the encoding result is correct.
// Interrupts will occupy CPU resources,  affect AP codec resource scheduling.So remove INT_BE_ERROR_VD0 in INT_VIDEO_EVENTS by default.
#define INT_VIDEO_EVENTS (INT_FE_HANG_VD0 | INT_BE_HANG_VD0 | INT_FE_ERROR_VD0 | \
                          INT_FE_HANG_VD1 | INT_BE_HANG_VD1 | INT_FE_ERROR_VD1 | INT_BE_ERROR_VD1 | \
                          INT_FE_HANG_VD2 | INT_BE_HANG_VD2 | INT_FE_ERROR_VD2 | INT_BE_ERROR_VD2)

#define INT_PMU_CPU  (INT_C3D_REFUCE)

//default interrupt to be enabled at irq install, vsync and hotplug intr is controlled by independent module
#define DEF_INTR  (INT_VIDEO_EVENTS | INT_FENCE | INT_HDCODEC | INT_DVFS | INT_PAGE_FAULT | INT_PMU_CPU)

#define AR_INIT_REG     0x83DA
#define AR_INDEX        0x83C0
#define AR_DATA         0x83C1
#define CR_INDEX        0x83D4
#define CR_DATA         0x83D5
#define SR_INDEX        0x83C4
#define SR_DATA         0x83C5

//mmioOffset of registers
#define MMIO_OFFSET_SR_GROUP_A_EXC  0x8600
#define MMIO_OFFSET_SR_GROUP_B_EXC  0x8700
#define MMIO_OFFSET_CR_GROUP_A_EXC  0x8800
#define MMIO_OFFSET_CR_GROUP_B_EXC  0x8900
#define MMIO_OFFSET_CR_GROUP_C_EXC  0x8A00
#define MMIO_OFFSET_CR_GROUP_D_EXC  0x8B00  // for 4-channel MIU CR_D registers
#define MMIO_OFFSET_CR_GROUP_D0_EXC 0x8C00  // MIU channel 0 CR_D registers
#define MMIO_OFFSET_CR_GROUP_D1_EXC 0x8D00  // MIU channel 1 CR_D registers
#define MMIO_OFFSET_CR_GROUP_D2_EXC 0x8E00  // MIU channel 2 CR_D registers
#define MMIO_OFFSET_CR_GROUP_D3_EXC 0x8F00  // MIU channel 3 CR_D registers
#define MMIO_OFFSET_SR_GROUP_T_EXC   0x9400
#define MMIO_OFFSET_CR_GROUP_T_EXC   0x9500

#define MMIO_OFFSET_GPU_TEMPERATURE  0x688c8

// H/V sync Polarity
#define HOR_NEGATIVE         0x40
#define HOR_POSITIVE         0x00
#define VER_NEGATIVE         0x80
#define VER_POSITIVE         0x00


#define DISP_OK  0

#define DISP_FAIL  (-1)

#ifndef  zx_mb
#if defined(__i386__) || defined(__x86_64__)
#define zx_mb()       asm volatile("mfence":::"memory")
#define zx_rmb()      asm volatile("lfence":::"memory")
#define zx_wmb()      asm volatile("sfence":::"memory")
#define zx_flush_wc() zx_wmb()
#else
#define zx_mb()
#define zx_rmb()
#define zx_wmb()
#define zx_flush_wc()
#endif
#endif

#define ENABLE_DP3_VGA_EXCLUSIVE 1

#define  SKIP_POLLING  0x1
#define  POLL_KEEP_RUNNING  0x2

typedef enum
{
    DISP_OUTPUT_NONE = 0x00,
    DISP_OUTPUT_CRT  = 0X01,
    DISP_OUTPUT_DP1  = 0X8000,
    DISP_OUTPUT_DP2  = 0X10000,
    DISP_OUTPUT_DP3  = 0x20000,
    DISP_OUTPUT_DP4  = 0x40000,
}disp_output_type;


#define NO_CONFLICT        0x0
#define DP1_USB_CONFLICT   0X001
#define DP2_USB_CONFLICT   0X010
#define DP3_CRT_CONFLICT   0x100

//enum_list name len must less 32
static const struct drm_prop_enum_list output_conflict_enum_list[] = {
    { NO_CONFLICT, "NO conflict!" },
    { DP1_USB_CONFLICT, "USB & DP3 conflict!" },
    { DP2_USB_CONFLICT, "USB & DP2 conflict!" },
    { DP3_CRT_CONFLICT, "CRT & DP3 conflict!" }
};

#define DISP_OUTPUT_DP_TYPES (DISP_OUTPUT_DP1 | DISP_OUTPUT_DP2 | DISP_OUTPUT_DP3 | DISP_OUTPUT_DP4)

typedef  struct
{
    int device;
    int int_type;
    unsigned long ack_delay;
}DP_EVENT, *PDP_EVENT;

#define MAX_DP_EVENT_NUM    64

#define PSR_ACTIVE_FAIL_THRESHOLD  5

typedef enum _PSR_OPERATION_RESULT
{
   PSR_OP_OK = 0,
   PSR_OP_TO_ACTIVE_FAIL,
   PSR_OP_TO_INACTIVE_FAIL,
   PSR_OP_TO_ACTIVE_CRITICAL,
} PSR_OPERATION_RESULT_T;

typedef enum _PSR_STATE
{
    PSR_STATE_0_DISABLED = 0,
    PSR_STATE_1_INACTIVE,
    PSR_STATE_2_EXIT,
    PSR_STATE_2_TRANSITION,
    PSR_STATE_3_ACTIVE,
    PSR_STATE_3_1_EPHY_OFF,
    PSR_STATE_3_2_IGA_OFF,
    PSR_STATE_3_3_DCLK_OFF
} PSR_STATE;

typedef enum _PSR_OPERATION
{
    PSR_OP_NONE = 0x00,
    PSR_OP_TRANS_TO_INACTIVE = 0x1,
    PSR_OP_TRANS_TO_ACTIVE = 0x2,
    PSR_OP_TRANS_TO_LEQ_IGA_OFF = 0x4,   //psr state less or equal to IGA_OFF is ok, it's used for vblank ref which just need enable DCLK/vblank 
} PSR_OPERATION;

typedef enum _PSR_REFERENCE
{
    PSR_ENABLE_REF = 0,
    PSR_FLIP_REF,
    PSR_CURSOR_REF,
    PSR_FB_REF,
    PSR_ONSCREENDRAW_REF,
    PSR_VBLANK_REF,
    PSR_MAX_REF
}PSR_REFERENCE;

typedef struct
{
    void *disp_info;
    int enabled;
    int forceoff;
    PSR_STATE current_state;
    atomic_t operation;
    struct os_mutex    *psr_mutex;
    struct os_spinlock *psr_ref_lock;
    unsigned int active_fail_num;
    unsigned int ref_count;
    int output_type;
    struct os_wait_event *psr_event;
    void    *psr_thread;
    struct timer_list psr_timer;
} psr_data_t;

//#define GET_LAST_BIT(a) ((a) & ((a)-1) ^ (a))

typedef enum
{
    ZX_MSG_METHOD_I2C,
    ZX_MSG_METHOD_I2C_OVER_AUX,
    ZX_MSG_METHOD_AUX,
}zx_msg_method_t;

typedef enum
{
    ZX_MSG_OP_NONE,
    ZX_MSG_OP_READ,
    ZX_MSG_OP_WRITE,
}zx_msg_op_t;

typedef struct
{
    unsigned int device_id;
    unsigned int addr;
    unsigned int offset;
    unsigned char *buf;
    unsigned int buf_len;
    zx_msg_method_t method;
    zx_msg_op_t op;
    struct
    {
        unsigned int  is_ddcci      : 1;
        unsigned int  customer_op   : 1;
        unsigned int  i2c_stop      : 1;
        unsigned int  resvd         : 29;
    };
}zx_xfer_msg_t;

typedef struct
{
    void            *rom_image;
    void            *cbios_ext;
    void*      zx_card;
    adapter_info_t*  adp_info;

    unsigned int     num_crtc;
    unsigned int     num_output;
    unsigned int     num_plane[MAX_CORE_CRTCS];
    unsigned int     scale_support;  //support panel up scale
    unsigned int     up_scale_plane_mask[MAX_CORE_CRTCS]; //stream mask for each crtc
    unsigned int     down_scale_plane_mask[MAX_CORE_CRTCS];

    unsigned int     support_output;
    unsigned int     active_output[MAX_CORE_CRTCS];

    void*            irq_chip_func;
    unsigned long long     intr_en_bits;
    unsigned int     poll_status;
    unsigned int     supp_polling_outputs;  //outputs that have no hpd intr, but still can be detected, such as CRT
    unsigned int     supp_hpd_outputs;
    unsigned int     hda_intr_outputs;
    unsigned int     irq_installing;
    struct os_spinlock *intr_lock;
    struct os_spinlock *hpd_lock;
    struct os_spinlock *hda_lock;
    struct os_mutex *gamma_lock;
    atomic_t         atomic_irq_lock;
    struct delayed_work hpd_irq_work;
    struct work_struct hda_work;
    struct
    {
        unsigned int head;
        unsigned int tail;
        DP_EVENT  event[MAX_DP_EVENT_NUM];
    };

    psr_data_t *psr_data;

    void            *modeset_restore_state;

    int              vbios_version;
    int              vbios_revision;
    unsigned char    pmp_version[64];     // PMP info,include version and build time

    struct zx_capture_type  *captures;

    struct
    {
        unsigned int  irq_enabled   : 1;
        unsigned int  worker_detect : 1;
        unsigned int  iga3_conflict : 1;
        unsigned int  bl_gfx_mode   : 1;
        unsigned int  resvd         : 28;
    };

    struct drm_property *primary_card_prop;
    struct drm_property *global_id_proph;
    struct drm_property *global_id_propl;
    struct drm_property *conn_conflict_event;
}disp_info_t;

static __inline__ unsigned char read_reg_exc(unsigned char *mmio, int type, unsigned char index)
{
    unsigned int  offset = 0;
    unsigned char temp   = 0;

    switch(type)
    {
    case CR:
        offset = MMIO_OFFSET_CR_GROUP_A_EXC + index;
        break;
    case CR_B:
        offset = MMIO_OFFSET_CR_GROUP_B_EXC + index;
        break;
    case CR_C:
        offset = MMIO_OFFSET_CR_GROUP_C_EXC + index;
        break;
    case SR:
        offset = MMIO_OFFSET_SR_GROUP_A_EXC + index;
        break;
    case SR_B:
        offset = MMIO_OFFSET_SR_GROUP_B_EXC + index;
        break;
    case AR:
        zx_read8(mmio + AR_INIT_REG);
        zx_write8(mmio + AR_INDEX, index);
        temp = zx_read8(mmio + AR_DATA);
        return temp;
    default:
        zx_assert(0, "type:%d", type);
        break;
    }

    temp = zx_read8(mmio + offset);

    return temp;
}

static __inline__ void write_reg_exc(unsigned char *mmio, int type, unsigned char index, unsigned char value, unsigned char mask)
{
    unsigned int  offset = 0;
    unsigned char temp   = 0;

    switch(type)
    {
    case CR:
        offset = MMIO_OFFSET_CR_GROUP_A_EXC + index;
        break;
    case CR_B:
        offset = MMIO_OFFSET_CR_GROUP_B_EXC + index;
        break;
    case CR_C:
        offset = MMIO_OFFSET_CR_GROUP_C_EXC + index;
        break;
    case SR:
        offset = MMIO_OFFSET_SR_GROUP_A_EXC + index;
        break;
    case SR_B:
        offset = MMIO_OFFSET_SR_GROUP_B_EXC + index;
        break;
    case SR_T:
        offset = MMIO_OFFSET_SR_GROUP_T_EXC + index;
        break;
    case CR_T:
        offset = MMIO_OFFSET_CR_GROUP_T_EXC + index;
        break;
    default:
        zx_assert(0, "type:%d", type);
        break;
    }

    temp   = zx_read8(mmio + offset);
    temp   = (temp & mask) | (value & ~mask);

    zx_write8(mmio + offset, temp);
}

void  disp_irq_init(disp_info_t* disp_info);
void  disp_irq_deinit(disp_info_t* disp_info);
struct drm_connector* zx_drm_connector_create(disp_info_t* disp_info, disp_output_type output);
struct drm_encoder* zx_drm_encoder_create(disp_info_t* disp_info, disp_output_type output);
int disp_get_pipe_from_crtc(zx_file_t *priv, zx_kms_get_pipe_from_crtc_t *get);
int disp_suspend(struct drm_device *dev);
#if ENABLE_RUNTIME_PM
int disp_runtime_suspend(struct drm_device *dev);
#endif

#if DRM_VERSION_CODE < KERNEL_VERSION(4, 8, 0)
void disp_vblank_save(struct drm_device* dev);
void disp_vblank_restore(struct drm_device* dev);
void  zx_disp_suspend_helper(struct drm_device *dev);
#endif

void zx_disp_post_shutdown(struct drm_device *dev);

void disp_pre_resume(struct drm_device *dev);
void disp_post_resume(struct drm_device *dev);
#if ENABLE_RUNTIME_PM
void disp_post_runtime_resume(struct drm_device *dev);
#endif
void zx_encoder_disable(struct drm_encoder *encoder);
void zx_encoder_enable(struct drm_encoder *encoder);

int disp_cbios_get_clock(disp_info_t *disp_info, unsigned int type, unsigned int *output);

void zx_detect_and_update_connectors(disp_info_t*  disp_info, int outp_masks, int hpd_detect,  int notify_os);

enum drm_connector_status
zx_connector_detect_locked(struct drm_connector *connector, int hpd_detect, int *changed);

void zx_update_conflict_event_property(disp_info_t* disp_info, int status);

zx_connector_t* zx_get_connector_by_device_id(disp_info_t*  disp_info, int device_id);

#if DRM_VERSION_CODE >= KERNEL_VERSION(4, 8, 0)
void zx_restore_drm_connector_state(struct drm_device *dev, struct drm_connector *connector,
                                    struct drm_modeset_acquire_ctx *ctx);
#endif

#endif
