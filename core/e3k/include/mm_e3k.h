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

#ifndef __MM_E3K_H
#define __MM_E3K_H

#define MAX_WIDTH_2DBLT                  (16 * 1024)  //16K
#define MAX_HEIGHT_2DBLT                 (16 * 1024)  //16K

typedef struct HWM_TILE_INFO_E3K
{
    int                        bTileSupported;    // Display & Render target tiled support
    int                        bTileTexSupported; // Texture tiled support
    int                        bTileZWSupported;  // Z write tiled support
    unsigned int       tileSizeInBytes;
    unsigned int       dwTileSizeX[7];
    unsigned int       dwTileSizeY[7];
} HWM_TILE_INFO_E3K;

//[TODO: need refine
//should be as same as hwm_caps_e3k.cpp
static HWM_TILE_INFO_E3K hwmTileInfo_e3k =
{
    TRUE,       // bTileSupported
    TRUE,       // bTileTexSupported
    TRUE,       // bTileZWSupported
    1024*64,    // tileSizeInBytes = 16k

    // dwTileSizeX
    {
            256,        // 0 8bit or less
            256,        // 1 16bit
            128,         // 2 32bit
            128,         // 3 64bit
            64,         // 4 128bit
            64,         // 5 256bit
            32,         // 6 512bit
    },

    // dwTileSizeY
    {
            256,        // 0 8bit or less
            128,         // 1 16bit
            128,         // 2 32bit
            64,         // 3 64bit
            64,         // 4 128bit
            32,         // 5 256bit
            32,         // 6 512bit
        },
};

#define UNIT_SIZE_E3K 8

static inline unsigned int cacl_log2(unsigned int s)
{
    unsigned int   iter = (unsigned int)-1;
    switch (s)
    {
    case 1:
        iter = 0;
        break;
    case 2:
        iter = 1;
        break;
    case 4:
        iter = 2;
        break;
    case 8:
        iter = 3;
        break;
    case 16:
        iter = 4;
        break;
    case 32:
        iter = 5;
        break;
    case 64:
        iter = 6;
        break;
    case 128:
        iter = 7;
        break;
    case 256:
        iter = 8;
        break;
    case 512:
        iter = 9;
        break;
    case 1024:
        iter = 10;
        break;
    default:
        {
            unsigned int   d = 1;
            do {
                d *= 2;
                iter++;
            } while (d < s);
            iter += ((s << 1) != d);
        }
    }
    return iter;
}


static unsigned int getTileSizeInBytes(void)
{
    return hwmTileInfo_e3k.tileSizeInBytes;
}

static unsigned int calcTileWidth_e3k (unsigned int bit_cnt)
{
    unsigned int bpp = cacl_log2(bit_cnt);
    unsigned int idx = (bit_cnt > 8) ? (bpp - 3) : 0;

    return hwmTileInfo_e3k.dwTileSizeX[idx];
}

static unsigned int calcTileHeight_e3k (unsigned int bit_cnt)
{
    unsigned int bpp = cacl_log2(bit_cnt);
    unsigned int idx = (bit_cnt > 8) ? (bpp - 3) : 0;

    return hwmTileInfo_e3k.dwTileSizeY[idx];
}

static unsigned int calcWidthInTiles_e3k (unsigned int width, unsigned int bit_cnt)
{
    unsigned int tsize = calcTileWidth_e3k(bit_cnt);

    return (width + tsize - 1) / tsize;
}

static unsigned int calcHeightInTiles_e3k (unsigned int height, unsigned int bit_cnt)
{
    unsigned int tsize = calcTileHeight_e3k(bit_cnt);

    return (height + tsize - 1) / tsize;
}

static unsigned int  calcUnitWidth_e3k (unsigned int bit_cnt)
{
    unsigned int bpp = cacl_log2(bit_cnt);
    unsigned int idx = (bit_cnt > 8) ? (bpp - 3) : 0;

    return (1 << ((UNIT_SIZE_E3K - idx + 1) / 2));
}

static unsigned int calcUnitHeight_e3k (unsigned int bit_cnt)
{
    unsigned int bpp = cacl_log2(bit_cnt);
    unsigned int idx = (bit_cnt > 8) ? (bpp - 3) : 0;

    return (1 << ((UNIT_SIZE_E3K - idx) / 2));
}

typedef enum _PHYSICAL_MEMORY_SEGMENT_ID_ELT3K
{
    SEGMENT_ID_INVALID_ELT3K                     = 0x0,
    SEGMENT_ID_LOCAL_ELT3K                       = 0x1,
    SEGMENT_ID_PCIE_UNSNOOPABLE_ELT3K            = 0x2,
    SEGMENT_ID_PCIE_SNOOPABLE_ELT3K              = 0x3,
    SEGMENT_ID_MAX_ELT3K                         = 0x4,
} PHYSICAL_MEMORY_SEGMENT_ID_ELT3K;

typedef struct CIL3_ALLOCATION_DESC_E3K CIL3_ALLOCATION_DESC_E3K;


#define SMALL_HEAP_SIZE_ELT3K                   (4*1024*1024)       //4M
#define SMALL_HEAP_MAX_ALLOCATE_SIZE_ELT3K     (32*1024)           //32K
#define SMALL_HEAP_MAX_ALLOCATE_SIZE_GART_ELT3K (64*1024)           //64K

// hang dump define
#define E3K_RINGBUFFER_FOR_HANG_SIZE        (0x4000)          // include DH_COMMON_INFO
#define E3K_DMABUFFER_FOR_HANG_SIZE         (0x400000)
#define E3K_CONTEXTBUFFER_FOR_HANG_SIZE     (0x160000)
#define E3K_TRANSFERBUFFER_FOR_HANG_SIZE    (0x10000000) //256M

#define GPU_VA_SPACE_TOTAL_SIZE               0x10000000000ull //total 40bits, 1T

#define GPU_VA_SHARED_SPACE_PCIE_SIZE_ELT3K        0x100000000ull  //4G
#define GPU_VA_PRIVATE_VMA_SPACE_SIZE              0x400000000ull  //16G
#define GPU_VA_SHARED_SPACE_OFFSET_ELT3K      0

typedef enum _MEMORY_BUDGET_ID_ELT3K
{
   MEMORY_BUDGET_ID_LOCAL_ELT3K  = 0,
   MEMORY_BUDGET_ID_SYSTEM_ELT3K = 1,
   MEMORY_BUDGET_ID_MAX_ELT3K    = 2,
}MEMORY_BUDGET_ID_ELT3K;

#define SECURE_RANGE_NUM 8
#define SECURE_RANGE_BUFFER_SIZE             (16*1024*1024)
#define SECURE_RANGE_ENCRYPTEN_ADDRESS       (0xD3F0)
#define SECURE_RANGE_SCRAMBLE_PATTEN_ADDRESS (0xD300)

#define MIU0_RANGE_BASE_ADDRESS              (0xD390)
#define MIU1_RANGE_BASE_ADDRESS              (0xD790)

typedef enum MM_DRIVERID_E3K
{
    MM_DRIVERID_INVALID = 0,
    // 2D
    MM_DRIVERID_2D_SOURCE,
    MM_DRIVERID_2D_DESTINATION,
    // 3D
    // IA
    MM_DRIVERID_VERTEXBUFFER_ADDRESS,//  32
    MM_DRIVERID_INDEXBUFFER_ADDRESS,
    // STO
    MM_DRIVERID_SO_BUFFER_ADDRESS,
    MM_DRIVERID_SO_BUFFER_BOUND,
    // VS
    MM_DRIVERID_VS_SHADER,
    MM_DRIVERID_VS_TEXTURE,          // 128
    MM_DRIVERID_VS_TSHARP,           //   8
    MM_DRIVERID_VS_SSHARP,           //   8
    MM_DRIVERID_VS_BCSHARP,

    // HS
    MM_DRIVERID_HS_SHADER,
    MM_DRIVERID_HS_TEXTURE,          // 128
    MM_DRIVERID_HS_TSHARP,           //   8
    MM_DRIVERID_HS_SSHARP,           //   8
    MM_DRIVERID_HS_BCSHARP,

    // DS
    MM_DRIVERID_DS_SHADER,
    MM_DRIVERID_DS_TEXTURE,          // 128
    MM_DRIVERID_DS_TSHARP,           //   8
    MM_DRIVERID_DS_SSHARP,           //   8
    MM_DRIVERID_DS_BCSHARP,

    // GS
    MM_DRIVERID_GS_SHADER,
    MM_DRIVERID_GS_TEXTURE,          // 128
    MM_DRIVERID_GS_TSHARP,           //   8
    MM_DRIVERID_GS_SSHARP,           //   8
    MM_DRIVERID_GS_BCSHARP,

    // PS
    MM_DRIVERID_PS_SHADER,
    MM_DRIVERID_PS_TEXTURE,          // 128
    MM_DRIVERID_PS_TSHARP,           //   8
    MM_DRIVERID_PS_SSHARP,           //   8
    MM_DRIVERID_PS_BCSHARP,

    // OM
    MM_DRIVERID_RENDERTARGET,        //   8
    MM_DRIVERID_DEPTHBUFFER,
    MM_DRIVERID_STENCILBUFFER,

    // EU ThreadSpace
    MM_DRIVERID_THREADSPACE,
    MM_DRIVERID_CSHARP,

    MM_DRIVERID_RANGE_FLAGBUFFER,    // 32
    MM_DRIVERID_RANGE,               // 32
    MM_DRIVERID_RANGE_BOUND,         // 32

                                     // CS
    MM_DRIVERID_CS_SHADER,
    MM_DRIVERID_CS_TEXTURE,          // 128
    MM_DRIVERID_CS_TSHARP,           // 8
    MM_DRIVERID_CS_SSHARP,           // 8
    MM_DRIVERID_CSUNORDEREDACCESS,   // 8

    MM_DRIVERID_CSL1,
    MM_DRIVERID_CSL2,
    MM_DRIVERID_CSL3,

    MM_DRIVERID_3D_LAST,             //  All above are state related

    // RM 3DBLT,Clear,MASS,AutoGen
    MM_DRIVERID_RM_DEPTHBUFFER,
    MM_DRIVERID_RM_STENCILBUFFER,
    MM_DRIVERID_RM_PS_TEXTURE,
    MM_DRIVERID_RM_RENDERTARGET,
    MM_DRIVERID_RM_VERTEXBUFFER,


    // MISC
    MM_DRIVERID_CONTEXT,
    MM_DRIVERID_FENCE,
    MM_DRIVERID_SIGNATURE,
    MM_DRIVERID_SIGNATURE_PER_DRAW,
    MM_DRIVERID_DIP_PARAM_BUFFER_ADDRESS,
    MM_DRIVERID_DIP_INDIRECT_BUFFER_ADDRESS,
    MM_DRIVERID_DISPATCH_INDIRECT_BUFFER_ADDRESS,
    MM_DRIVERID_SO_BUFFER_FILL_SIZE_POOL,
    MM_DRIVERID_UA_APPEND_FILLED_SIZE_POOL,
    MM_DRIVERID_FLAGBUFFER,
    MM_DRIVERID_CMDBUFFER,
    MM_DRIVERID_SVM_POINTER,
    MM_DRIVERID_MIUCOUNTER,
    MM_DRIVERID_MXU1COUNTER,
    // Video
    MM_DRIVERID_VIDEO_RESERVEDMEM,          // 16
    MM_DRIVERID_VIDEO_SIGNATURE,            // 8
    MM_DRIVERID_VIDEO_FENCE,                // 1
    MM_DRIVERID_VIDEO_PATCH,                // 8
    MM_DRIVERID_VIDEO_DECODE_COMPBUFF,      // 1
    MM_DRIVERID_VIDEO_DECODE_MSVD,          // 20
    MM_DRIVERID_VIDEO_DECODE_CMD,           // 1
    MM_DRIVERID_VIDEO_DECODE_INS,           // 1
    MM_DRIVERID_VIDEO_DECODE_VCPDMA,
    MM_DRIVERID_VIDEO_DECODE_VCPSBDMA,
    MM_DRIVERID_VIDEO_DECODE_QTMDMA,
    MM_DRIVERID_VIDEO_DECODE_DRVDMA,
    MM_DRIVERID_VIDEO_DRAWID,

    // DIU
    MM_DRIVERID_VIDEO_PS1_ADDR0,
    MM_DRIVERID_VIDEO_PS2_ADDR0,
    MM_DRIVERID_VIDEO_SS1_ADDR0,
    MM_DRIVERID_VIDEO_SS2_ADDR0,
    MM_DRIVERID_VIDEO_SS1_MMIOUPDATE_ADDR0,
    MM_DRIVERID_VIDEO_SS2_MMIOUPDATE_ADDR0,
    MM_DRIVERID_VIDEO_SS1_MMIOFLIP_ADDR0,
    MM_DRIVERID_VIDEO_SS2_MMIOFLIP_ADDR0,
    MM_DRIVERID_VIDEO_BACKLIGHT_CTRL_VALUE,

    //ISP
    MM_DRIVERID_ISP_VPP_SRC,
    MM_DRIVERID_ISP_VPP_DST,
    MM_DRIVERID_ISP_CAPTURE,
    MM_DRIVERID_ISP_BACKLIGHT_CTRL_BUFF,

    // Cache invalidate
    MM_DRIVERID_INVALID_STREAMCACHE_VS_ADDRESS,
    MM_DRIVERID_INVALID_STREAMCACHE_HS_ADDRESS,
    MM_DRIVERID_INVALID_STREAMCACHE_DS_ADDRESS,
    MM_DRIVERID_INVALID_STREAMCACHE_GS_ADDRESS,
    MM_DRIVERID_INVALID_STREAMCACHE_PS_ADDRESS,
    MM_DRIVERID_INVALID_STREAMCACHE_CS_ADDRESS,
    MM_DRIVERID_INVALID_L2_CACHE_ADDRESS,
    MM_DRIVERID_INVALID_L2_CACHE_MASK,

    // split 32 bits address to two 16 bits address in ppmode
    MM_DRIVERID_PPMODE_SPLIT_ADDR,

    // only for model extension SetBufferAddrToCB
    MM_DRIVERID_VERTEXBUFFER_40BIT_ADDRESS,

    MM_DRIVERID_HEAP,
    MM_DRIVERID_SECOND_BUFFER_ADDRESS,
    MM_DRIVERID_DESCRIPTOR_HEAP,
    MM_DRIVERID_QUERYPOOL,
    MM_DRIVERID_TBR_SIGNATURE_BUFFER_ADDRESS,
    //new added 2 items for CIL3 UMD 3dblt, not added for Linux so put it in the end
    MM_DRIVERID_RM_VERTEXSHADER,
    MM_DRIVERID_RM_PIXELSHADER,
    MM_DRIVERID_LAST,

} MM_DRIVERID_E3K;

typedef enum MM_ALLOCATION_TYPE_E3K
{
    MM_AT_GENERIC_E3K = 0,
    MM_AT_VERTEXBUFFER_E3K,
    MM_AT_INDEXBUFFER_E3K,
    MM_AT_CSHARP_E3K,
    MM_AT_TEXTURE_E3K,
    MM_AT_TSHARP_E3K,
    MM_AT_SSHARP_E3K,
    MM_AT_BCSHARP_E3K,
    MM_AT_RT_TEXTURE_E3K,
    MM_AT_DS_TEXTURE_E3K,
    MM_AT_RENDERTARGET_E3K,
    MM_AT_RENDERTARGET_SSB_E3K,
    MM_AT_DEPTHBUFFER_E3K,
    MM_AT_DEPTHBUFFER_SSB_E3K,
    MM_AT_STENCILBUFFER_E3K,
    MM_AT_STENCILBUFFER_SSB_E3K,
    MM_AT_SO_BUFFER_E3K,
    MM_AT_SO_DESC_E3K,
    MM_AT_EUPFSHADER_E3K,
    MM_AT_EUPBSHADER_E3K,
    MM_AT_THREADSPACE_E3K,
    MM_AT_BACKBUFFER_E3K,
    MM_AT_CONTEXT_E3K,
    MM_AT_FENCE_E3K,
    MM_AT_FLAGBUFFER_E3K,
    MM_AT_2D_E3K,
    MM_AT_VIDEO_RESERVEDMEM_E3K,
    MM_AT_VIDEO_DECODE_RT_E3K,
    MM_AT_VIDEO_DECODE_COMPBUFF_E3K,
    MM_AT_VIDEO_VIDEOPROCESS_RT_E3K,
    MM_AT_ISP_BACKLIGHT_CTRL_BUFF_E3K,
    MM_AT_HEAP,
    MM_AT_SECOND_BUFFER_ADDRESS,
    MM_AT_DESCRIPTOR_HEAP,
    MM_AT_QUERYPOOL_E3K,
    MM_AT_TBR_SIGNATURE_BUFFER_ADDRESS_E3K,
    MM_AT_MIUCOUNTER_E3K,
    MM_AT_MXU1COUNTER_E3K,
    MM_AT_SIGNATURE_PER_DRAW_E3K,
    MM_AT_SVM_POINTER_E3K,
    MM_AT_OCL_SPECIAL_BUFFER_E3K,
    MM_AT_LAST_E3K,
} MM_ALLOCATION_TYPE_E3K;

#endif

