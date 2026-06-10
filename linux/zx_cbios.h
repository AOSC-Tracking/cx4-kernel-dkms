#ifndef __ZX_CBIOS_H__
#define __ZX_CBIOS_H__
#include "CBios.h"

enum
{
    FAMILY_CMODEL,
    FAMILY_CLB,
    FAMILY_DST,
    FAMILY_CSR,
    FAMILY_INV,
    FAMILY_EXC,
    FAMILY_ELT,
    FAMILY_LAST,
};

enum
{
    CHIP_CMODEL,
    CHIP_CLB,
    CHIP_DST,
    CHIP_CSR,
    CHIP_INV,
    CHIP_H5,
    CHIP_H5S1,
    CHIP_H6S2,
    CHIP_CMS,
    CHIP_METRO,
    CHIP_MANHATTAN,
    CHIP_MATRIX,
    CHIP_DST2,
    CHIP_DST3,
    CHIP_DUMA,
    CHIP_H6S1,
    CHIP_DST4,
    CHIP_EXC1,      //Excalibur-1
    CHIP_E2UMA,     //E2UMA
    CHIP_ELT,       //Elite
    CHIP_ELT1K,     //Elite1k
    CHIP_ELT2K,     //Elite2k
    CHIP_ELT2K5,    //Elite2500
    CHIP_ZX2000,    //ZX2000
    CHIP_ELT3K,      //ELITE3K
    CHIP_CHX001,    //CHX001
    CHIP_CHX002,    //CHX002
    CHIP_ZX2100,    //ZX2100
    CHIP_CHX004,    //CHX004
    CHIP_CNE001,    //CNE001
    CHIP_LAST,      //Maximum number of chips supported.
};

typedef struct
{
    int crtc_index;
    int *hpos;
    int *vpos;
    int *vblk;
    int *in_vblk;
}zx_get_counter_t;

typedef struct
{
    unsigned int support_brightness_ctrl;
    unsigned int max_brightness_value;
    unsigned int min_brightness_value;
}zx_brightness_caps_t;

typedef enum
{
    ZX_AUDIO_FORMAT_REFER_TO_STREAM_HEADER,
    ZX_AUDIO_FORMAT_LPCM,
    ZX_AUDIO_FORMAT_AC_3,
    ZX_AUDIO_FORMAT_MPEG_1,
    ZX_AUDIO_FORMAT_MP3,
    ZX_AUDIO_FORMAT_MPEG_2,
    ZX_AUDIO_FORMAT_AAC_LC,
    ZX_AUDIO_FORMAT_DTS,
    ZX_AUDIO_FORMAT_ATRAC,
    ZX_AUDIO_FORMAT_DSD,
    ZX_AUDIO_FORMAT_E_AC_3,
    ZX_AUDIO_FORMAT_DTS_HD,
    ZX_AUDIO_FORMAT_MLP,
    ZX_AUDIO_FORMAT_DST,
    ZX_AUDIO_FORMAT_WMA_PRO,
    ZX_AUDIO_FORMAT_HE_AAC,
    ZX_AUDIO_FORMAT_HE_AAC_V2,
    ZX_AUDIO_FORMAT_MPEG_SURROUND
}ZX_HDMI_AUDIO_FORMAT_TYPE;

typedef struct
{
    ZX_HDMI_AUDIO_FORMAT_TYPE   format;
    unsigned int                 max_channel_num;
    union
    {
        struct
        {
            unsigned int         SR_32kHz             :1; /* Bit0 = 1, support sample rate of 32kHz */
            unsigned int         SR_44_1kHz           :1; /* Bit1 = 1, support sample rate of 44.1kHz */
            unsigned int         SR_48kHz             :1; /* Bit2 = 1, support sample rate of 48kHz */
            unsigned int         SR_88_2kHz           :1; /* Bit3 = 1, support sample rate of 88.2kHz */
            unsigned int         SR_96kHz             :1; /* Bit4 = 1, support sample rate of 96kHz */
            unsigned int         SR_176_4kHz          :1; /* Bit5 = 1, support sample rate of 176.4kHz */
            unsigned int         SR_192kHz            :1; /* Bit6 = 1, support sample rate of 192kHz */
            unsigned int         reserved             :25;
        }sample_rate;

        unsigned int             sample_rate_unit;
    };

    union
    {
        unsigned int             unit;

        // for audio format: LPCM
        struct
        {
            unsigned int         BD_16bit             :1; /* Bit0 = 1, support bit depth of 16 bits */
            unsigned int         BD_20bit             :1; /* Bit1 = 1, support bit depth of 20 bits */
            unsigned int         BD_24bit             :1; /* Bit2 = 1, support bit depth of 24 bits */
            unsigned int         reserved             :29;
        }bit_depth;

        // for audio format: AC-3, MPEG-1, MP3, MPED-2, AAC LC, DTS, ATRAC
        unsigned int             max_bit_Rate; // unit: kHz

        // for audio format: DSD, E-AC-3, DTS-HD, MLP, DST
        unsigned int             audio_format_depend_value; /* for these audio formats, this value is defined in
                                                            it's corresponding format-specific documents*/

        // for audio format: WMA Pro
        struct
        {
            unsigned int         value                :3;
            unsigned int         reserved             :29;
        }profile;
    };
}zx_hdmi_audio_format_t;

typedef struct
{
    unsigned int            num_formats;
    zx_hdmi_audio_format_t audio_formats[16];
}zx_hdmi_audio_formats;


#define ZX_HDCP_ENABLED            0x0
#define ZX_HDCP_FAILED             0x1
#define ZX_HDCP_NO_SUPPORT_DEVICE  0x2

typedef struct
{
    unsigned int    enable;                  /* in, 1 enable, 0 disable */
    unsigned int    output_type;             /* in, if 0, all HDMIs will be enable/disable */
    unsigned int    result;                  /* out, see ZX_HDCP_XXX */
}zx_hdcp_op_t;

typedef enum
{
    ZX_HW_NONE = 0x00,
    ZX_HW_IGA  = 0x01,
}zx_hw_block;



typedef enum
{
    ZX_STREAM_PS = 0,
    ZX_STREAM_SS,
    ZX_STREAM_TS,
    ZX_STREAM_4S,
    ZX_STREAM_5S,
    ZX_STREAM_6S,
    ZX_STREAM_MAX,
}ZX_STREAM_TYPE;

typedef enum
{
    ZX_PLANE_PS = 0,
    ZX_PLANE_SS,
    ZX_PLANE_TS,
    ZX_PLANE_FS,
    ZX_PLANE_NUM,
}ZX_PLANE_TYPE;

#define ZX_PLANE_CURSOR         (ZX_PLANE_NUM + 1)

#define ZX_SELECT_MCLK          0x00
#define ZX_SELECT_DCLK1         0x01
#define ZX_SELECT_DCLK2         0x02
#define ZX_SELECT_TVCLK         0x03
#define ZX_SELECT_ECLK          0x04
#define ZX_SELECT_ICLK          0x05

#define ZX_VBIOS_ROM_SIZE       0x10000
#define ZX_SHADOW_VBIOS_SIZE    0x20000


#define DUMP_REGISTER_STREAM   0x1

#define  UPDATE_CRTC_MODE_FLAG   0x1
#define  UPDATE_ENCODER_MODE_FLAG  0x2

int         disp_get_output_num(int  outputs);
int         disp_init_cbios(disp_info_t *disp_info);
int         disp_cbios_init_hw(disp_info_t *disp_info);
int         disp_cbios_cleanup(disp_info_t *disp_info);
void        disp_cbios_get_crtc_resource(disp_info_t *disp_info);
void        disp_cbios_get_crtc_caps(disp_info_t *disp_info);
int         disp_cbios_get_port_attri(disp_info_t *disp_info, int output);
void        disp_cbios_query_vbeinfo(disp_info_t *disp_info);
void        disp_cbios_read_config_from_efuse(disp_info_t *disp_info);
int         disp_cbios_get_modes_size(disp_info_t *disp_info, int output);
int         disp_cbios_get_modes(disp_info_t *disp_info, int output, void* buffer, int buf_size);
int         disp_cbios_get_adapter_modes_size(disp_info_t *disp_info);
int         disp_cbios_get_adapter_modes(disp_info_t *disp_info, void* buffer, int buf_size);
CBiosModeInfoExt* disp_cbios_get_preferred_mode(CBiosModeInfoExt *dev_mode_list, unsigned int mode_num);
int         disp_cbios_merge_modes(CBiosModeInfoExt* merge_mode_list, CBiosModeInfoExt * adapter_mode_list, unsigned int const adapter_mode_num,
    CBiosModeInfoExt const * dev_mode_list, unsigned int const dev_mode_num);
int         disp_cbios_cbmode_to_drmmode(disp_info_t *disp_info, int output, void* cbmode, int i, struct drm_display_mode *drm_mode);
int         disp_cbios_3dmode_to_drmmode(disp_info_t *disp_info, int output, void* mode, int i, struct drm_display_mode *drm_mode);
int         disp_cbios_get_3dmode_size(disp_info_t* disp_info, int output);
int         disp_cbios_get_3dmodes(disp_info_t *disp_info, int output, void* buffer, int buf_size);
int         disp_cbios_get_mode_timing(disp_info_t *disp_info, int output, void* cb_mode, struct drm_display_mode *drm_mode);
void*       disp_cbios_read_edid(disp_info_t *disp_info, int output, int* edid_len);
void        disp_cbios_set_edid(disp_info_t *disp_info, int output, char* edid, int size);
int         disp_cbios_update_output_active(disp_info_t *disp_info, int* outputs);
int         disp_cbios_set_mode(disp_info_t *disp_info, int crtc, struct drm_display_mode* mode, struct drm_display_mode* adjusted_mode, update_mode_para_t flag);
int         disp_cbios_set_hdac_connect_status(disp_info_t *disp_info, int device , int bPresent, int bEldValid);
int         disp_cbios_turn_onoff_screen(disp_info_t *disp_info, int iga, int on);
int         disp_cbios_turn_onoff_iga(disp_info_t *disp_info, int iga, int on);
int         disp_cbios_detect_output_status(disp_info_t *disp_info, int device_bit, int full_detect, int* changed);
int         disp_cbios_set_dpms(disp_info_t *disp_info, int device, int dpms_on, unsigned int flags);
int         disp_cbios_sync_vbios(disp_info_t *disp_info);
int         disp_cbios_get_active_devices(disp_info_t *disp_info, int* devices);
int         disp_cbios_set_gamma(disp_info_t *disp_info, int pipe, void* data);
void        disp_delay_micro_seconds(unsigned int usecs);
int         disp_cbios_dbg_level_get(disp_info_t *disp_info);
void        disp_cbios_dbg_level_set(disp_info_t *disp_info, int dbg_level);
int         disp_cbios_get_connector_attrib(disp_info_t *disp_info, zx_connector_t *zx_connector);
int         disp_cbios_get_crtc_mask(disp_info_t *disp_info,  int device);
int         disp_cbios_get_clock(disp_info_t *disp_info, unsigned int type, unsigned int *clock);
int         disp_cbios_set_clock(disp_info_t *disp_info, unsigned int type, unsigned int para);
int         disp_cbios_enable_hdcp(disp_info_t *disp_info, unsigned int enable, unsigned int devices);
int         disp_cbios_get_hdcp_status(disp_info_t *disp_info, zx_hdcp_op_t *dhcp_op, unsigned int devices);
int         disp_cbios_get_dpint_type(disp_info_t *disp_info,unsigned int device);
int         disp_cbios_handle_dp_irq(disp_info_t *disp_info, unsigned int device, int int_type, int* detect, int* dp_port, int* reset_psr);
void        disp_cbios_dump_registers(disp_info_t *disp_info, int type);
int         disp_cbios_set_hda_codec(disp_info_t *disp_info, zx_connector_t*  zx_connector);
int         disp_cbios_get_hdmi_audio_format(disp_info_t *disp_info, unsigned int device_id, zx_hdmi_audio_formats *audio_formats);
void        disp_cbios_reset_hw_block(disp_info_t *disp_info, zx_hw_block hw_block);
int         disp_cbios_get_counter(disp_info_t* disp_info, zx_get_counter_t* get_counter);
int         disp_cbios_crtc_flip(disp_info_t *disp_info, zx_crtc_flip_t *arg);
int         disp_cbios_update_cursor(disp_info_t *disp_info, zx_cursor_update_t *arg);
int         disp_wait_for_vblank(disp_info_t* disp_info, int pipe, int timeout);
#ifdef ZX_CAPTURE_VIP_INTERFACE
int         disp_cbios_vip_ctl(disp_info_t *disp_info, zx_vip_set_t *v_set);
#endif

#ifdef ZX_CAPTURE_WB_INTERFACE
int         disp_cbios_wb_ctl(disp_info_t *disp_info,  zx_wb_set_t *wb_set);
#endif
void        disp_cbios_brightness_set(disp_info_t* disp_info, unsigned int brightness);
unsigned int    disp_cbios_brightness_get(disp_info_t* disp_info);
void        disp_cbios_query_brightness_caps(disp_info_t* disp_info, zx_brightness_caps_t *brightness_caps);

int disp_cbios_psrop0_1(psr_data_t *psr_data);
int disp_cbios_psrop1_0(psr_data_t *psr_data);
int disp_cbios_psrop1_2(psr_data_t *psr_data);
int disp_cbios_psrop2_0(psr_data_t *psr_data);
int disp_cbios_psrop2_3(psr_data_t *psr_data);
int disp_cbios_psrop2_5(psr_data_t *psr_data);
int disp_cbios_psrop3_0(psr_data_t *psr_data);
int disp_cbios_psrop3_1(psr_data_t *psr_data);
int disp_cbios_psrop3_5(psr_data_t *psr_data);
int disp_cbios_psrop5_0(psr_data_t *psr_data);
int disp_cbios_psrop5_1(psr_data_t *psr_data);
int disp_cbios_psrop3_3_1(psr_data_t *psr_data);
int disp_cbios_psrop3_1_3_2(psr_data_t *psr_data);
int disp_cbios_psrop3_2_3_3(psr_data_t *psr_data);
int disp_cbios_psrop3_3_3_2(psr_data_t *psr_data);
int disp_cbios_psrop3_2_3_1(psr_data_t *psr_data);
int disp_cbios_psrop3_1_3(psr_data_t *psr_data);
int psr_acquire_display(void *data, unsigned int ref_type, int wait_done);
void psr_release_display(void *data, unsigned int ref_type);
void psr_enable(void *data, unsigned int device);
void psr_disable(void *data);
void psr_reset(void *data);
void psr_operate_trans_state(void* data);
int disp_cbios_msg_xfer(disp_info_t *disp_info, zx_xfer_msg_t *msg_param);
#endif
