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
#ifndef __zx_def_h__
#define __zx_def_h__

#include "zx_perf.h"

#define MAX_SCREENS         4

//Fix-me:
// need set MAX_CORE_CRTCS as 3 in CHX001, just keep it as 2 in e2uma validation.
// currently cannot set 3, because there is no vsync interrupt for the 3rd IGA, this may lead KMD pending in some cases
//
#define MAX_CORE_CRTCS      4

#define ZX_MAJOR           227
#define OUTPUT_NAME_LEN     16

#define MAX_ENGINE_COUNT   12
#define ALL_ENGINE_MASK    ((1 << MAX_ENGINE_COUNT) - 1)

typedef union
{
#ifndef __BIG_ENDIAN__
 struct
 {
    unsigned int low32;
    unsigned int  high32;
 };
 #else
 struct
 {
    unsigned int  high32;
    unsigned int  low32;
 };
 #endif
    unsigned long long quad64;
}large_inter;

#define ALIGN8 __attribute__ ((aligned(8)))

#ifndef UNUSED
#define UNUSED(x) (void)(x)
#endif

typedef unsigned long long zx_ptr64_t;

#define ZX_SEGMENT_PROPERTY_LOCAL          0x1
#define ZX_SEGMENT_PROPERTY_PCIE           0x2
#define ZX_SEGMENT_PROPERTY_SNOOPABLE      0x20

typedef struct
{
    int server_index; /*in */
    int screen_index; /*in */

    int device_index; /*out, index of device(adapter)*/
    int sub_device_index; /*out, index of framebuffer device */
}zx_sub_device_info_t;

typedef struct
{
    unsigned int timeout;       // ms
    unsigned int write;
    zx_ptr64_t   allocation;    /* in */
}zx_wait_allocation_idle_t;

typedef struct
{
    unsigned long long mmioPhyAddr;  /* output */
}zx_get_address_config_t;

typedef enum
{
    ZX_QUERY_VBIOS_VERSION,
    ZX_QUERY_TOTAL_VRAM_SIZE,
    ZX_QUERY_CPU_VISIBLE_VRAM_SIZE,
    ZX_QUERY_RESERV_VRAM_SIZE,
    ZX_QUERY_CHIP_ID,

    ZX_QUERY_CRTC_OUTPUT,
    ZX_QUERY_VSYNC_CNT,
    ZX_QUERY_HEIGHT_ALIGN,
    ZX_QUERY_LOCAL_VRAM_TYPE,
    ZX_QUERY_ENGINE_CLOCK,

    ZX_QUERY_VCLK,
    ZX_QUERY_MCLK,
    ZX_QUERY_VMEM_CLOCK,
    ZX_QUERY_VRAM_USED,
    ZX_QUERY_GART_USED,

    ZX_QUERY_REGISTER_U32,
    ZX_QUERY_REGISTER_U64,
    ZX_QUERY_INFO_MAX,
    ZX_QUERY_I_CLOCK,
    ZX_QUERY_CPU_FREQUENCE,

    ZX_SET_I_CLOCK,
    ZX_SET_VIDEO_CLOCK,
    ZX_SET_DPMS_FORCE_OFF,    /* argu = 1, mean enable force off, 0 mean disable force off */
    ZX_QUERY_PMU_REGISTER_U32,
    ZX_SET_CPU_FREQUENCE,

    ZX_CANCEL_DPMS_FORCE_OFF, /* cancel dpms force off, and light previous skipped monitor when force off enable */
    ZX_QUERY_VENDOR_ID,
    ZX_QUERY_DEVICE_ID,
    ZX_QUERY_REVISION_ID,
    ZX_QUERY_SEGMENT_FREE_SIZE,

    ZX_QUERY_HW_HANG,
    ZX_SET_HPD_MASK,
    ZX_SET_PMU_REGISTER_U32,
    ZX_QUERY_SECURED_ON,
    ZX_QUERY_PAGE_SWIZZLE_SUPPORT,

    ZX_QUERY_VSYNC_TIMESTAMP,
    ZX_QUERY_PENDING_FRAME_NUM,
    ZX_QUERY_PMC_REGISTER_U32,
    ZX_SET_PMC_REGISTER_U32,
    ZX_QUERY_ALLOCATION_INFO,

    ZX_QUERY_GPU_TEMPERATURE,
    ZX_QUERY_GPU_TIME_STAMP,
    ZX_QUERY_FLIP_COUNT,
    ZX_QUERY_LOCAL_ALLOCATION_MAX_SIZE,
    ZX_QUERY_VCP_INDEX,

    ZX_QUERY_RET_VCP_INDEX,
    ZX_QUERY_VCP_INFO,
    ZX_QUERY_VIDEO_SEQ_INDEX,
    ZX_QUERY_RET_VIDEO_SEQ_INDEX,
    ZX_QUERY_ADAPTER_INFO,

    ZX_QUERY_PCIE_LINK_WIDTH,
    ZX_QUERY_ACTIVE_ENGINE_COUNT,      // 50
    ZX_QUERY_GET_VIDEO_BRIDGE_BUFFER,
    ZX_QUERY_CHIP_SLICE_MASK,
    ZX_QUERY_ALLOCATION_RESIDENCY,
    ZX_QUERY_UNSNOOPABLE_INFO,
    ZX_QUERY_VCP0_CLOCK,
    ZX_QUERY_VCP1_CLOCK,
    ZX_QUERY_CMU_CLOCK,
    ZX_SET_REGISTER_U32,
    ZX_QUERY_ENGINE_USAGE_STATUS,
    ZX_QUERY_ENGINE_USAGE_STATUS_EXT,
    ZX_QUERY_PROCESS_INFO,
    ZX_QUERY_VPP_INDEX,
    ZX_QUERY_RET_VPP_INDEX,
}ZX_QUERY_INFO_TYPE;

typedef struct
{
    unsigned int type;    /* in */
    unsigned int argu;    /* in */
    zx_ptr64_t   in_buf;
    unsigned int buf_len; /*in */
    unsigned int pad;
    union
    {
        int          signed_value;           /* out */
        unsigned int value;                  /* out */
        unsigned long long value64;          /* out */
        zx_ptr64_t   buf;                   /* out */
    };
}zx_query_info_t;

typedef zx_query_info_t zx_config_info_t;

typedef struct
{
    unsigned short  Vendor_ID;
    unsigned char   Device_ID;
    unsigned char   Revision_ID;
}zx_chip_id_t;

typedef struct
{
    unsigned int  allocation;
    unsigned int  engine_mask;
    unsigned int  state;
}zx_get_allocation_state_t;

#define     UT_OUTPUT_TYPE_NONE      0x00
#define     UT_OUTPUT_TYPE_CRT       0x01
#define     UT_OUTPUT_TYPE_TV        0x02
#define     UT_OUTPUT_TYPE_HDTV      0x04
#define     UT_OUTPUT_TYPE_PANEL     0x08
#define     UT_OUTPUT_TYPE_DVI       0x10
#define     UT_OUTPUT_TYPE_HDMI      0x20
#define     UT_OUTPUT_TYPE_DP        0x40
#define     UT_OUTPUT_TYPE_MHL       0x80

static inline zx_ptr64_t ptr_to_ptr64(void *ptr)
{
    return (zx_ptr64_t)(unsigned long)ptr;
}

static inline void *ptr64_to_ptr(zx_ptr64_t ptr64)
{
    return (void*)(unsigned long)ptr64;
}

typedef struct {
    unsigned int         pid;
    unsigned int         enable;
    unsigned int         dwDecodeLevel;
    unsigned int         dwDecodeType;
    unsigned int         dwFrameType;
    int                  DecodeWidth;
    int                  DecodeHeight;
    unsigned int         DecodeRTNum;
    unsigned int         SliceNum;

    unsigned int         TotalFrameNum;
    unsigned int         TotalIFrameNum;
    unsigned int         TotalPFrameNum;
    unsigned int         TotalBFrameNum;
    unsigned int         TotalBltFrameNum;
    unsigned int         TotalBitstreamSize;
}zx_vcp_info;

typedef struct
{
    unsigned short  VendorID;                   // (ro)
    unsigned short  DeviceID;                   // (ro)
    unsigned short  Command;                    // Device control
    unsigned short  Status;
    unsigned char   RevisionID;                 // (ro)
    unsigned char   ProgIf;                     // (ro)
    unsigned char   SubClass;                   // (ro)
    unsigned char   BaseClass;                  // (ro)
    unsigned char   CacheLineSize;              // (ro+)
    unsigned char   LatencyTimer;               // (ro+)
    unsigned char   HeaderType;                 // (ro)
    unsigned char   BIST;                       // Built in self test
    unsigned int    ulBaseAddresses[6]; // 6 == PCI_TYPE0_ADDRESSES
    unsigned int    CardBus;
    unsigned short  SubsystemVendorID;
    unsigned short  SubSystemID;
    unsigned int    PCIConfig_Unused[4];
    unsigned int    AGPCapability;
    unsigned int    AGPStatus;
    unsigned int    AGPCommand;
    unsigned int    BusNum;
    unsigned int    SlotNum;

    // for PCI Express support
    unsigned int    LinkCaps;
    unsigned short  LinkStatus;

    int             bSwizzled;
    unsigned int    memoryType;      // 0: 128bits bus; 1: 64bits bus
    unsigned char   pageSelect;      // For memory (2K, 4K, 8K page sizes)
    unsigned char   bankSelect;      // Which address bits to use for bank select
    unsigned char   bankSelectShift; // Which address bits to use for bank select
    unsigned char   bankSelectMask;  // BitMask for bank select bits
    unsigned long long   memorySize;
} zx_pci_config_t;

typedef struct {
    unsigned int VidmemSize;
    unsigned int cjGARTTable;
    unsigned int GARTTableSize;
    unsigned int cjVidmemPageTable;
    unsigned int VidmemPageTableSize;
    unsigned int cjPageKeyBuffer;
    unsigned int PageKeyBufferSize;
    unsigned int cjFBRingBuffer;
    unsigned int FBRingBufferSize;
    int    bNullHW;
    int    bNullBridge;
    int pad;
    unsigned long long UserCModelVidMemBase;
    unsigned long long pfnCModelFlushEngine;
    unsigned long long pfnGetCModelInfo;
    unsigned long long pfnGetRegistryFromCModelIni;
} zx_model2_cil_info_t;

typedef struct
{
    zx_pci_config_t            bus_config;
    zx_model2_cil_info_t       model2cilinfo;
    unsigned int               chipslicemask;
    unsigned int               gpucount;
    unsigned int               osversion;
    int                        bVideoOnly;
    int                        bCTEDumpEnable;
    int                        bSnoopOnly;
    int                        segmentMapTable[5];
    unsigned int               bl_slot_size;
    unsigned int               LegacyOffset;
    unsigned int               segment_info_cnt;
    unsigned long long         segment_info[4];
    unsigned long long         RangeBufferGpuVa;
    unsigned int               bDvfsEnable;
    unsigned int               Reserved;
} zx_adapter_info_t;

#endif

