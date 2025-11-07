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
//    Spec Last Modified Time: 16:58 2020-05-25
#ifndef _CSP_OPCODE_H
#define _CSP_OPCODE_H

#ifndef        COMMAND_OPCODES_BLOCKBASE_INF
    #define    COMMAND_OPCODES_BLOCKBASE_INF
    #define    BLOCK_COMMAND_OPCODES_VERSION 1
    #define    BLOCK_COMMAND_OPCODES_TIMESTAMP "16:58 2020-05-25"
#endif

// CSP command opercode definition
#define  CSP_OPCODE_Skip                    0x0            // 0h: SKIP Command
#define  CSP_OPCODE_Dma                     0x1            // 1h: Command dma. Dw followed normal/flush dma cmd is 2 dw.
#define  CSP_OPCODE_Wait                    0x2            // 2h: Wait Command:external or internal wait.
#define  CSP_OPCODE_Query_Dump              0x3            // 3h: Query Dump Command:Query Dump is the command to dump out GPU
                                                           // status.
#define  CSP_OPCODE_Set_Register            0x4            // 4h: Set Register Command:Set Register Command is CSP only command,
                                                           // CSP would update shadow buffer or just bypass to GPC_Pool.
#define  CSP_OPCODE_Execute_Indirect        0x5            // 5h: ExecuteIndirect Command
#define  CSP_OPCODE_Set_Object              0x6            // 6h: set object. Use to update predicate_en register.
#define  CSP_OPCODE_Dip                     0x7            // 7h: DrawIndexedPrimitive Command
#define  CSP_OPCODE_Fence                   0x8            // 8h: Fence Command:Internal fence is used to allow certain block to
                                                           // update status into internal fence counter at a given slot. CSP will
                                                           // maintain 4 fence slots. Each slot has corresponding 8-bit register
                                                           // (counter); external fence is used to sync with CPU. The register value
                                                           // will be updated according to the update mode.
#define  CSP_OPCODE_Block_Command_Template  0x9            // 9h: BlockCommand Template: BlockCommand targets at some blocks for
                                                           // them to take action. CSP will not parse details about it, but
                                                           // corresponding block will.
#define  CSP_OPCODE_Block_Command_Tas       0x9            // 9h: BlockCommand starting at TAS
#define  CSP_OPCODE_Block_Command_Sg        0x9            // 9h: BlockCommand starting at SG
#define  CSP_OPCODE_Block_Command_Img_Trn   0x9            // 9h: BlockCommand for Image Transfer, starting at SG
#define  CSP_OPCODE_Block_Command_Flush     0x9            // 9h: BlockCommand for FFC Flush
#define  CSP_OPCODE_Block_Command_Eu        0x9            // 9h: BlockCommand for EU
#define  CSP_OPCODE_Block_Command_Tu        0x9            // 9h: BlockCommand for TU
#define  CSP_OPCODE_Block_Command_L2        0x9            // 9h: BlockCommand for L2
#define  CSP_OPCODE_Block_Command_Mxu       0x9            // 9h: BlockCommand for MXU & L2.
#define  CSP_OPCODE_Block_Command_Csp       0x9            // 9h: BlockCommand for CSP
#define  CSP_OPCODE_Blk_Cmd_Csp_Blc         0x9            // 9h: BlockCommand for CSP
#define  CSP_OPCODE_Blk_Cmd_Csp_Save_Rsto   0x9            // 9h: BlockCommand for CSP
#define  CSP_OPCODE_Blk_Cmd_Csp_Indicator   0x9            // 9h:  Pwm adj Indicator command
#define  CSP_OPCODE_Blk_Cmd_Csp_Ld_Des      0x9            // 9h: Load descriptor cmd for CSP
#define CSP_OPCODE_Blk_Cmd_Csp_Force_Flush  0x9            //9h: BlockCommand for CSP
#define  CSP_OPCODE_Block_Command_Video     0x9            // 9h: BlockCommand for VIDEO
#define  CSP_OPCODE_Gp                      0xB            // Bh: General Purpose Computing. According to Paul, in E3KN, the
                                                           // global/local size is set to EU's reg.
#define  CSP_OPCODE_Tbr_Indicator           0xF            // Fh: begin/end indicator for the loop cmd list
#define  CSP_OPCODE_Vpp                     0xC            // Ch: VPP Command. For VPP cmd in VPP RB, VPP engine parse it, csp just
                                                           // bypss cmd in VPP RB into VPP engine in E3K.

// Block constant definition
typedef enum
{
    DMA_SPECIAL_DMA_TYPE_SAVE_REST_CMD       = 0,
    DMA_SPECIAL_DMA_TYPE_QUERY_SETREG_CMD    = 1,
} DMA_SPECIAL_DMA_TYPE;
typedef enum
{
    DMA_MODE_NORMAL                          = 0,   // normal dma
    DMA_MODE_SAVE                            = 1,   // save dma
    DMA_MODE_RESTORE                         = 2,   // restore dma
    DMA_MODE_FLUSH                           = 3,   // flush dma
} DMA_MODE;
typedef enum
{
    WAIT_WAIT_MODE_NORMAL_WAIT               = 0,   // normal wait for internal fence
    WAIT_WAIT_MODE_KKK_WAIT                  = 1,   // wait for kernel kick off kernel
    WAIT_WAIT_MODE_WAIT_CHIP_IDLE            = 2,   // normal external wait for engine idle
    WAIT_WAIT_MODE_EXTERNAL_WAIT             = 3,   // external wait for vulkan global event sync
} WAIT_WAIT_MODE;
typedef enum
{
    WAIT_METHOD_BIGEQUAL                     = 0,   // 0: Big Equal
    WAIT_METHOD_EQUAL                        = 1,   // 1: Equal
} WAIT_METHOD;
typedef enum
{
    WAIT_STATION_ID_MAIN_PARSER              = 0,   // parser the wait cmd in csp main-parser.
    WAIT_STATION_ID_PRE_PARSER               = 1,   // parser the wait cmd in csp pre-parser. Can only be normal
                                                    // wait mode.
} WAIT_STATION_ID;
typedef enum
{
    QUERY_DUMP_ADDRESS_MODE_ADDRESS          = 0,   // When in this mode, the 2 dw followed cmd is just the query
                                                    // dump address.
    QUERY_DUMP_ADDRESS_MODE_OFFSET           = 1,   // When in this mode, the 2 dw followed cmd is the byte offset in
                                                    // current ctx, should add ctx base address and then to do
                                                    // query dump. Used in save DMA.
} QUERY_DUMP_ADDRESS_MODE;
typedef enum
{
    QUERY_DUMP_CMD_TYPE_NORMAL_QUERY         = 0,   // 0: normal query dump cmd.
    QUERY_DUMP_CMD_TYPE_CPY_QRY_RES          = 1,   // 1: vulkan copy query results cmd.
} QUERY_DUMP_CMD_TYPE;
typedef enum
{
    QUERY_DUMP_BLOCK_ID_CSP                  = 0,
    QUERY_DUMP_BLOCK_ID_GPCPFE               = 1,
    QUERY_DUMP_BLOCK_ID_SPIN                 = 2,
    QUERY_DUMP_BLOCK_ID_EU_FS                = 3,
    QUERY_DUMP_BLOCK_ID_TAS_FE               = 4,
    QUERY_DUMP_BLOCK_ID_TAS_BE               = 5,
    QUERY_DUMP_BLOCK_ID_FF                   = 6,
    QUERY_DUMP_BLOCK_ID_IU                   = 7,
    QUERY_DUMP_BLOCK_ID_WLS_FE               = 8,
    QUERY_DUMP_BLOCK_ID_EU_PS                = 9,
    QUERY_DUMP_BLOCK_ID_TUFE                 = 10,
    QUERY_DUMP_BLOCK_ID_L2                   = 11,
    QUERY_DUMP_BLOCK_ID_MXU                  = 12,
    QUERY_DUMP_BLOCK_ID_MCE                  = 13,
    QUERY_DUMP_BLOCK_ID_EU_CS                = 14,
    QUERY_DUMP_BLOCK_ID_VCP                  = 15,
    QUERY_DUMP_BLOCK_ID_VPP                  = 16,
    QUERY_DUMP_BLOCK_ID_ISP                  = 17,
    QUERY_DUMP_BLOCK_ID_MMU                  = 18,
    QUERY_DUMP_BLOCK_ID_WLS_BE               = 19,
    QUERY_DUMP_BLOCK_ID_TUBE                 = 20,
    QUERY_DUMP_BLOCK_ID_GPCPBE               = 21,
    QUERY_DUMP_BLOCK_ID_SPOUT                = 22,
    QUERY_DUMP_BLOCK_ID_MIU                  = 23,
    QUERY_DUMP_BLOCK_ID_GPCTOP               = 24,
    QUERY_DUMP_BLOCK_ID_MXU1                 = 25,
} QUERY_DUMP_BLOCK_ID;
typedef enum
{
    SET_REGISTER_ADDRESS_MODE_ADDRESS        = 0,   // When in this mode, the 2 dw followed cmd is just the prefetch
                                                    // address.
    SET_REGISTER_ADDRESS_MODE_OFFSET         = 1,   // When in this mode, the 2 dw followed cmd is the byte offset in
                                                    // current ctx, should add ctx base address and then to do
                                                    // prefetch. This mode is only used for cmd in restore DMA.
} SET_REGISTER_ADDRESS_MODE;
typedef enum
{
    SET_OBJECT_OBJECT_TYPE_OCCLUSION_PREDICATE= 0,  // Predication result based on the occlusion results.
                                                    // Occlusion cnt is global, and 64b. Csp should prefetch 4 ddw
                                                    // for this predication type. [0] for begin, [1] for
                                                    // begin_ready, [2] for end, [3] for end_ready
    SET_OBJECT_OBJECT_TYPE_SO_OVERFLOW_PREDICATE_STREAMI= 1,
                                                    // Predication result based on the streamout results for a
                                                    // singla stream.The needed/written cnt are both global,
                                                    // and 64b. Csp should prefetch 32 dw for this predication
                                                    // type.
    SET_OBJECT_OBJECT_TYPE_SO_OVERFLOW_PREDICATE= 2,// Predication result based on the streamout results for the
                                                    // 4 streams.The needed/written cnt are both global, and
                                                    // 64b. Csp should prefetch 32 dw for this predication type.
    SET_OBJECT_OBJECT_TYPE_DIRECTLY_RESULT_PREDICATE= 3,
                                                    // Predication result based on the 64b/32b cnt in memory.
                                                    // Whether 64b or 32b should refer to
                                                    // "Directly_Result_b32".
} SET_OBJECT_OBJECT_TYPE;
typedef enum
{
    DIP_DRAW_MODE_IMM                        = 0,   // immediate draw, index is default 32 bits. For Draw,
                                                    // DrawInstanced and DrawInstancedIndirect APIs
    DIP_DRAW_MODE_INDEX                      = 1,   // Draw with index buffer. For DrawIndexed,
                                                    // DrawIndexedInstanced and
                                                    // DrawIndexedInstancedIndirect APIs
    DIP_DRAW_MODE_AUTO                       = 4,   // Only for DrawAuto API, index is default from 0, 32 bits
} DIP_DRAW_MODE;
typedef enum
{
    DIP_MODE_DRAW_IMM                        = 0,   // immediate draw, index is default 32 bits. For Draw,
                                                    // DrawInstanced and DrawInstancedIndirect APIs
    DIP_MODE_IB_8                            = 1,   // 8 bits index for indexed draw. For DrawIndexed,
                                                    // DrawIndexedInstanced and
                                                    // DrawIndexedInstancedIndirect APIs
    DIP_MODE_IB_16                           = 2,   // 16 bits index for indexed draw
    DIP_MODE_IB_32                           = 3,   // 32 bits index for indexed draw
    DIP_MODE_DRAW_AUTO                       = 4,   // Only for DrawAuto API, index is default from 0, 32 bits
} DIP_MODE;
typedef enum
{
    DIP_INSTANCE_EN_DISABLED                 = 0,
    DIP_INSTANCE_EN_ENABLED                  = 1,
} DIP_INSTANCE_EN;
typedef enum
{
    DIP_P_TYPE_POINTLIST                     = 0,   // 0x0: Point List
    DIP_P_TYPE_LINELIST                      = 1,   // 0x1: LineList
    DIP_P_TYPE_LINESTRIP                     = 2,   // 0x2: Line Strip
    DIP_P_TYPE_TRIANGLELIST                  = 3,   // 0x3: Triangle List
    DIP_P_TYPE_TRIANGLESTRIP                 = 4,   // 0x4: Triangle Strip
    DIP_P_TYPE_LINELIST_ADJ                  = 5,   // 0x5: Line List Adjacent
    DIP_P_TYPE_LINESTRIP_ADJ                 = 6,   // 0x6: Line Strip Adjacent
    DIP_P_TYPE_TRIANGLELIST_ADJ              = 7,   // 0x7: Triangle List Adjacent
    DIP_P_TYPE_TRIANGLESTRIP_ADJ             = 8,   // 0x8: Triangle Strip Adjacent
    DIP_P_TYPE_LINELOOP                      = 9,   // 0x9: Line Loop/ OVG path
    DIP_P_TYPE_TRIANGLEFAN_OGL               = 10,  // 0xa: OGL Triangle Fan
    DIP_P_TYPE_TRIANGLEFAN_D3D_VULKAN        = 11,  // 0xb: D3D/Vulkan Triangle Fan
    DIP_P_TYPE_TRIANGLEFAN                   = DIP_P_TYPE_TRIANGLEFAN_D3D_VULKAN, // CHX004 OGL/d3d/vulkan
    DIP_P_TYPE_PATCHLIST                     = 12,  // 0xc: DX11 Patch List
    DIP_P_TYPE_QUADLIST_OGL                  = 13,  // 0xd: OGL Quad List
    DIP_P_TYPE_QUADSTRIP_OGL                 = 14,  // 0xe: OGL Quad Strip
    DIP_P_TYPE_POLYGON_OGL                   = 15,  // 0xf: OGL Polygon
    DIP_P_TYPE_AXIS_ALIGNED_RECTANGLELIST    = 16,  // 0x10: DX12 Axis Aligned Rectangle list
} DIP_P_TYPE;
typedef enum
{
    FENCE_IRQ_NOP                            = 0,   // 00: Nop
    FENCE_IRQ_INTERRUPT_CPU                  = 1,   // 01: interrupt CPU when external fence done
    FENCE_IRQ_INTERRUPT_GPU                  = 2,   // 02: When it's external fence cmd, notify mxu about the
                                                    // external fence type: 1->normal external fence cmd;
                                                    // 2->external fence cmd for vulkan global event sync(event
                                                    // value filled by HW), now mxu should send an interrupt to
                                                    // csp, to notify the external fence is done.
} FENCE_IRQ;
typedef enum
{
    FENCE_FENCE_TYPE_INTERNAL                = 0,
    FENCE_FENCE_TYPE_IMM_DATA_BLT            = 1,   // Indicate this is just a IMM data blt cmd, not real fence cmd.
    FENCE_FENCE_TYPE_EXTERNAL32              = 2,   // 32bit external fence value
    FENCE_FENCE_TYPE_EXTERNAL64              = 3,   // 64bit external fence value, After 64b external fence, HW
                                                    // can try to do DMA level preempt. If
                                                    // Reg_Preempt_Cmd_Process. Preempt==1, then do preempt
} FENCE_FENCE_TYPE;
typedef enum
{
    FENCE_FENCE_UPDATE_MODE_COPY             = 0,   // 0x0: Copy operation
    FENCE_FENCE_UPDATE_MODE_OR               = 1,   // 0x1: Or operation
} FENCE_FENCE_UPDATE_MODE;
typedef enum
{
    FENCE_RB_TYPE_3DFE                       = 0,   // 0: the fence is from 3DL/H RB for 3D Front end shader
    FENCE_RB_TYPE_3DBE                       = 1,   // 0: the fence is from 3DL/H RB for 3D Back end shader
    FENCE_RB_TYPE_CSL                        = 2,   // 1: the fence is from CS LOW RB
    FENCE_RB_TYPE_CSH                        = 3,   // 2: the fence is from CS HIGH RB
} FENCE_RB_TYPE;
typedef enum
{
    FENCE_ROUTE_ID_CSP_FENCE                 = 0,   // BlockID = CSP; Path: CSP->MXU, fence shadow save,flush
                                                    // BLC,
    FENCE_ROUTE_ID_Z_FENCE                   = 1,   // BlockID = FF; Path:
                                                    // CSP->GPCPFE->GPCPBE->TASBE->SG->TGZ(ZL3)->FFC->HUB.
                                                    // since each resource may be L2 cachable or not, HUB need
                                                    // broadcast the fence command to both L2 and MXU if followed
                                                    // data's "L2Cachable" is enable; furthremore, this fence
                                                    // can also be used to fence "Query dump FF block registers
                                                    // finish"
    FENCE_ROUTE_ID_D_FENCE                   = 2,   // BlockID = FF; Path:
                                                    // CSP->GPCPFE->GPCPBE->TASBE->SG->TGZ->ZL3->WBU->FFC->HUB.
                                                    // since each resource may be L2 cachable or not, HUB need
                                                    // broadcast the fence command to both L2 and MXU
    FENCE_ROUTE_ID_UAVFE_FENCE               = 3,   // Front end shader UAV: BlockID = WLS_FE; Path:
                                                    // CSP->GPCPFE->SPIN->EUFE->WLS->FFC->HUB.  since each
                                                    // resource may be L2 cachable or not, HUB need broadcast the
                                                    // fence command to both L2 and MXU  if followed data's
                                                    // "L2Cachable" is enable. furthermore,  furthremore, this
                                                    // fence can also be used to fence "Query dump IU block
                                                    // registers finish"
    FENCE_ROUTE_ID_UAVBE_FENCE               = 4,   // Back end shader UAV: BlockID = WLS_BE; Path:
                                                    // CSP->GPCPFE->GPCPBE->TASBE->SG->TGZ->IU->EUPS->ZL3->WLS->FFC->HUB.
                                                    //  since each resource may be L2 cachable or not, HUB need
                                                    // broadcast the fence command to both L2 and MXU if followed
                                                    // data's "L2Cachable" is enable. furthermore,
                                                    // furthremore, this fence can also be used to fence "Query
                                                    // dump IU block registers finish"
    FENCE_ROUTE_ID_FS_STO_FENCE              = 5,   // BlockID = GPCPBE; Path:
                                                    // CSP->GPCPFE->SPIN->EUFS->TASFE->GPCPBE->MXU. front
                                                    // end shader read finish and STO use this route
    FENCE_ROUTE_ID_MCE_FENCE                 = 6,   // BlockID = MCE; Path: CSP->MCE-> MXU; Mini Copy Engint
                                                    // fence
    FENCE_ROUTE_ID_L2_FENCE                  = 7,   // BlockID = L2; Path: CSP->MXU->L2->MXU, fence l2 flush
                                                    // finish
    FENCE_ROUTE_ID_VCP_FENCE                 = 8,
    FENCE_ROUTE_ID_VPP_FENCE                 = 9,
    FENCE_ROUTE_ID_ISP_FENCE                 = 10,
} FENCE_ROUTE_ID;
typedef enum
{
    BLOCK_COMMAND_TEMPLATE_TYPE_FLUSH        = 0,   // 0x00: flush ffc or drain eu pipe
    BLOCK_COMMAND_TEMPLATE_TYPE_INVALIDATE_CACHE= 1,// 0x01: invalidate ff cache
    BLOCK_COMMAND_TEMPLATE_TYPE_SG           = 2,   // 0x02: SG command
    BLOCK_COMMAND_TEMPLATE_TYPE_IMAGE_TRANSFER= 3,  // 0x03: SG command image transfer
} BLOCK_COMMAND_TEMPLATE_TYPE;
typedef enum
{
    BLOCK_COMMAND_TAS_TYPE_COLOR_EXCLUDED    = 0,   // color is not followed for DP line. E.g. dw0=vertex0,
                                                    // dw1=vertex1.
    BLOCK_COMMAND_TAS_TYPE_COLOR_INCLUDED    = 1,   // color is followed for DP line besides vertex coordinates.
                                                    // E.g. dw0=color, dw1=vertex0, dw2=vertex1. repeat for
                                                    // more lines.
} BLOCK_COMMAND_TAS_TYPE;
typedef enum
{
    BLOCK_COMMAND_TAS_COMMAND_SPECIFIC_FIELD_DP_LINE= 0,
                                                    // 0x00: Draw 2D Line with input vertex and color
} BLOCK_COMMAND_TAS_COMMAND_SPECIFIC_FIELD;
typedef enum
{
    BLOCK_COMMAND_SG_AREA_TARGET_Z           = 0,   // 0x00: Z,z surface format get from z surface register
    BLOCK_COMMAND_SG_AREA_TARGET_S           = 1,   // 0x01: Stencil
    BLOCK_COMMAND_SG_AREA_TARGET_D           = 2,   // 0x02: RT
    BLOCK_COMMAND_SG_AREA_TARGET_U           = 3,   // 0x03: UAV
} BLOCK_COMMAND_SG_AREA_TARGET;
typedef enum
{
    BLOCK_COMMAND_SG_ACTION_TYPE_FAST_CLEAR_TILE= 0,// 0x00: Fast Clear Tiled Surface where STG generates tile
    BLOCK_COMMAND_SG_ACTION_TYPE_FAST_CLEAR_LINEAR= 1,
                                                    // 0x01: Fast Clear Linear Surface where ZL3 controls
                                                    // traversing
    BLOCK_COMMAND_SG_ACTION_TYPE_BIT_BLT     = 2,   // 0x02:Bit Blt
    BLOCK_COMMAND_SG_ACTION_TYPE_GRADIENT_FILL= 3,  // 0x03:Gradient Fill
    BLOCK_COMMAND_SG_ACTION_TYPE_ROTATION    = 4,   // 0x04:Rotation
    BLOCK_COMMAND_SG_ACTION_TYPE_RESERVED    = 7,   // 0x07:Reserved
} BLOCK_COMMAND_SG_ACTION_TYPE;
typedef enum
{
    BLOCK_COMMAND_SG_TBR_LOOP_STATUS_PERTBRTILE= 0, // 0x0: execute this command per valid TBR tile by default
    BLOCK_COMMAND_SG_TBR_LOOP_STATUS_ONCE    = 1,   // 0x1: execute this command only once between accumulation
                                                    // range,(frame_begin-frame_end or
                                                    // frame_begin--force_kickoff, or
                                                    // force_kickoff--frame_end)
} BLOCK_COMMAND_SG_TBR_LOOP_STATUS;
typedef enum
{
    BLOCK_COMMAND_SG_BLT_OVERLAP_NON_OVERLAP= 0,    // if non-overlap, this blit cmd will be split to multi slice
                                                    // from SG
    BLOCK_COMMAND_SG_BLT_OVERLAP_OVERLAP     = 1,   // if overlap, this blit cmd will be sent to slice0 only.
} BLOCK_COMMAND_SG_BLT_OVERLAP;
typedef enum
{
    BLOCK_COMMAND_SG_ROT_DWORD0_CW_CW        = 0,   // 0x00: CW
    BLOCK_COMMAND_SG_ROT_DWORD0_CW_CCW       = 1,   // 0x01: CCW
} BLOCK_COMMAND_SG_ROT_DWORD0_CW;
typedef enum
{
    BLOCK_COMMAND_IMG_TRN_DATA_FMT_1BPP      = 0,   // 0x000: this mode tells h/w that 1BPP source is enabled, so
                                                    // h/w will lookup fg_color or bg_color using each source
                                                    // bit.
    BLOCK_COMMAND_IMG_TRN_DATA_FMT_4BPP      = 1,   // 0x001: this mode tells h/w that 4 bit alpha from 2D source
                                                    // surface is enabled , so h/w will upscaleit to
                                                    // alpha8=alpha4<<4|alpha4, and use it to blend:
                                                    // (fg_color*alpha8+bg_color*(~alpha8)+128)>>8.
    BLOCK_COMMAND_IMG_TRN_DATA_FMT_8BPP      = 2,   // 0x010: normal color image, 8 bit per pixel
    BLOCK_COMMAND_IMG_TRN_DATA_FMT_16BPP     = 3,   // 0x011: normal color image, 16 bit per pixel
    BLOCK_COMMAND_IMG_TRN_DATA_FMT_32BPP     = 4,   // 0x100: normal color image, 32 bit per pixel
    BLOCK_COMMAND_IMG_TRN_DATA_FMT_64BPP     = 5,   // 0x101: normal color image, 64 bit per pixel
    BLOCK_COMMAND_IMG_TRN_DATA_FMT_128BPP    = 6,   // 0x110: normal color image, 128 bit per pixel
} BLOCK_COMMAND_IMG_TRN_DATA_FMT;
typedef enum
{
    BLOCK_COMMAND_IMG_TRN_TBR_LOOP_STATUS_PERTBRTILE= 0,
                                                    // 0x0: execute this command per valid TBR tile by default
    BLOCK_COMMAND_IMG_TRN_TBR_LOOP_STATUS_ONCE= 1,  // 0x1: execute this command only once between accumulation
                                                    // range,(frame_begin-frame_end or
                                                    // frame_begin--force_kickoff, or
                                                    // force_kickoff--frame_end)
} BLOCK_COMMAND_IMG_TRN_TBR_LOOP_STATUS;
typedef enum
{
    BLOCK_COMMAND_FLUSH_TYPE_FLUSH           = 0,   // 0x00: flush ffc or drain eu pipe
    BLOCK_COMMAND_FLUSH_TYPE_INVALIDATE_CACHE= 1,   // 0x01: invalidate ff cache
    BLOCK_COMMAND_FLUSH_TYPE_SG              = 2,   // 0x02: SG command
    BLOCK_COMMAND_FLUSH_TYPE_IMAGE_TRANSFER  = 3,   // 0x03: SG command image transfer
} BLOCK_COMMAND_FLUSH_TYPE;
typedef enum
{
    BLOCK_COMMAND_FLUSH_TARGET_ALL_C         = 0,   // 0x00: flush all cachelines into mxu (only D, Z, S).
                                                    // Block_id == FF
    BLOCK_COMMAND_FLUSH_TARGET_Z_C           = 1,   // 0x01: Z cachelines. Block_id == FF, PATH:
                                                    // CSP->GPCPFE->GPCPBE->TASBE->SG->TGZ(ZL3)->FFC->HUB/MXU.The
                                                    // same as Fence Z
    BLOCK_COMMAND_FLUSH_TARGET_S_C           = 2,   // 0x02: S cachelines. Block_id == FF, PATH:
                                                    // CSP->GPCPFE->GPCPBE->TASBE->SG->TGZ(ZL3)->FFC->HUB/MXU.
                                                    // The same as Fence S
    BLOCK_COMMAND_FLUSH_TARGET_D_C           = 3,   // 0x03: D cachelines. Block_id == FF, PATH:
                                                    // CSP->GPCPFE->GPCPBE->TASBE->SG->TGZ->ZL3->WBU->FFC->HUB/MXU.
                                                    // The same as Fence D
    BLOCK_COMMAND_FLUSH_TARGET_UAV_C         = 4,   // 0x04: UAV_data cache. Block_id and Path are decided by
                                                    // "UAV_TYPE"
    BLOCK_COMMAND_FLUSH_TARGET_USHARP_DESC   = 5,   // 0x05: invalidate 3DL U# descriptor cache (U#
                                                    // registers),Block_id and Path are decided by "UAV_TYPE"
    BLOCK_COMMAND_FLUSH_TARGET_2D_ONLY       = 6,   // 0x07: only flush/invalidate the cache lines not occupied
                                                    // by 3D TBR.Block_id == FF, PATH:
                                                    // CSP->GPCPFE->GPCPBE->TASBE->SG->TGZ->ZL3->WBU->FFC->HUB/MXU.
                                                    // Path is the same as Fence D
    BLOCK_COMMAND_FLUSH_TARGET_D_SIG_BUF     = 7,   // 0x07: flush the 2Kb D sig buf into memory. Not support
                                                    // invalidate for this buffer.Block_id == FF, PATH:
                                                    // CSP->GPCPFE->GPCPBE->TASBE->SG->TGZ->ZL3->WBU->FFC->HUB/MXU.
                                                    // The same as Fence D
} BLOCK_COMMAND_FLUSH_TARGET;
typedef enum
{
    BLOCK_COMMAND_FLUSH_UAV_TYPE_3DFE        = 0,   // 0x00: 3D UAV Front end shader(VS,GS,HS,DS) reqeust or
                                                    // descriptor. BlockID = WLS_FE; Path:
                                                    // CSP->GPCPFE->SPIN->EUFE->WLS->FFC->HUB.
    BLOCK_COMMAND_FLUSH_UAV_TYPE_3DBE        = 1,   // 0x01: 3D UAV Back end shader(PS) reqeust or descriptor.
                                                    // BlockID = WLS_BE; Path:
                                                    // CSP->GPCPFE->GPCPBE->TASBE->SG->TGZ->IU->EUPS->ZL3->WLS->FFC->HUB
    BLOCK_COMMAND_FLUSH_UAV_TYPE_CSL         = 2,   // 0x02:  CS UAV command or descriptor from CS LOW RB, BlockID =
                                                    // WLS_FE; Path:
                                                    // CSP->GPCPFE->SPIN->EUFE->WLS->FFC->HUB.
    BLOCK_COMMAND_FLUSH_UAV_TYPE_CSH         = 3,   // 0x03:  CS UAV command or descriptor  from CS HIGH RB. BlockID
                                                    // = WLS_FE; Path:
                                                    // CSP->GPCPFE->SPIN->EUFE->WLS->FFC->HUB.
} BLOCK_COMMAND_FLUSH_UAV_TYPE;
typedef enum
{
    BLOCK_COMMAND_EU_TYPE_INVALIDATE_L1I     = 0,   // 0x00: invalidate L1 Instruction cache
    BLOCK_COMMAND_EU_TYPE_DRAIN_EU           = 1,   // 0x01: if block id is "EU_FS", drain Front End
                                                    // Shader((including VS/HS/DS/GS and TESS, TASASM)), for
                                                    // 3D only. If block id is "EU_CS", drain CS, for GP only. The EU
                                                    // needs to make sure all CS threads have finished and the
                                                    // EU-WBU FIFO empty.if block id is "EU_PS",drain PS, for 3D
                                                    // only. The EU needs to make sure all PS threads have finished
                                                    // and the EU-WBU FIFO empty.
} BLOCK_COMMAND_EU_TYPE;
typedef enum
{
    BLOCK_COMMAND_TU_TYPE_INVALIDATE_L1      = 0,   // 0x00: invalidate L1 descriptor cache and texture data
                                                    // cache.
                                                    //  There are 4 zone types inside TU L1 cache. they are
                                                    //  3DFE
                                                    //  3DBE
                                                    //  CSL
                                                    //  CSH
                                                    //  The L1 descriptor cache and texture data cache will be
                                                    // invalidated together, if TU receives invalidte command
                                                    // with specified zone type.
                                                    //  The driver needs to invalidate both 3DFE and 3DBE when a new
                                                    // context come. These 2 invalidate commands have differnt
                                                    // block_id, one is TUFE, the other is TUBE. The GPCP will
                                                    // separate them and make them go to the related path.
                                                    //  TUFE: the path is CSP->GPCPFE->SP->VSTC->TU
                                                    //  TUBE: the path is
                                                    // CSP->GPCPFE->GPCPBE->TASBE->SG->TG->IU->PSTC->TU
                                                    //  CSL: the path is CSP->GPCPFE->SP->CSTC(L)->TU
                                                    //  CSH: the path is CSP->GPCPFE->SP->CSTC(H)->TU
BLOCK_COMMAND_TU_TYPE_PRE_FETCH = 1,                // 0x01: pre_fetch for ts# cache
} BLOCK_COMMAND_TU_TYPE;
typedef enum
{
    BLOCK_COMMAND_L2_TYPE_FLUSH_L2           = 0,   // 0x00: flush L2 pipes to L2 cache and flush L2 cache to memory
    BLOCK_COMMAND_L2_TYPE_INVALIDATE_L2      = 1,   // 0x01: invalidate L2 cache, invalidation could either use
                                                    // usage, or use address, or both.
    BLOCK_COMMAND_L2_TYPE_FLUSH_L1           = 4,   // 0x04: flush L1 in L2, in hw side, it's flush L2 pipes to L2
                                                    // cache
} BLOCK_COMMAND_L2_TYPE;
typedef enum
{
    BLOCK_COMMAND_L2_USAGE_ALL               = 0,   // All possible resource type
    BLOCK_COMMAND_L2_USAGE_TU                = 1,   // only the TU's resource type, including VB, texture
    BLOCK_COMMAND_L2_USAGE_UAV               = 2,   // UAV surface
    BLOCK_COMMAND_L2_USAGE_DZ                = 3,   // D/Z surface, only uncompressed DZ could be cached inner L2
    BLOCK_COMMAND_L2_USAGE_IC                = 4,   // Instruction cache
    BLOCK_COMMAND_L2_USAGE_CSP               = 5,   // ALL CSP resource, including IB/DMA and others.
    BLOCK_COMMAND_L2_USAGE_DESC              = 6,   // Descriptor cache
} BLOCK_COMMAND_L2_USAGE;
typedef enum
{
    BLOCK_COMMAND_MXU_CMD_TYPE_FLUSH_CACHE   = 0,   // flush blc
    BLOCK_COMMAND_MXU_CMD_TYPE_INVALIDATE_CACHE= 1, // invalidate blc
    BLOCK_COMMAND_MXU_CMD_TYPE_INVALIDATE_UTLB_CACHE= 2,
                                                    // invalidate utlb
    BLOCK_COMMAND_MXU_CMD_TYPE_INVALIDATE_DTLB_CACHE= 3,
                                                    // invalidate dtlb
} BLOCK_COMMAND_MXU_CMD_TYPE;
typedef enum
{
    BLOCK_COMMAND_MXU_BLC_DWORD1_USAGE_ALL   = 0,
    BLOCK_COMMAND_MXU_BLC_DWORD1_USAGE_TU    = 1,
    BLOCK_COMMAND_MXU_BLC_DWORD1_USAGE_UAV   = 2,
    BLOCK_COMMAND_MXU_BLC_DWORD1_USAGE_DZ    = 3,
    BLOCK_COMMAND_MXU_BLC_DWORD1_USAGE_IC    = 4,
    BLOCK_COMMAND_MXU_BLC_DWORD1_USAGE_DMA   = 5,
    BLOCK_COMMAND_MXU_BLC_DWORD1_USAGE_DESC  = 6,
} BLOCK_COMMAND_MXU_BLC_DWORD1_USAGE;
typedef enum
{
    BLOCK_COMMAND_CSP_TYPE_INDICATOR         = 0,   // 0: Slice/Pe set cmd, can be set by driver, csp or random.
                                                    // Parsing cmd with "BLK_CMD_CSP_Indicator" header.
    BLOCK_COMMAND_CSP_TYPE_CLEAR_MXU_BLC     = 1,   // 1:clear mxu burst length cache. Parsing cmd with
                                                    // "BLK_CMD_CSP_BLC" header
    BLOCK_COMMAND_CSP_TYPE_SAVE              = 2,   // 2:Save cmd. Parsing cmd with "BLK_CMD_CSP_Save_Rsto"
                                                    // header
    BLOCK_COMMAND_CSP_TYPE_RESTORE           = 3,   // 3:Restore cmd. Parsing cmd with
                                                    // "BLK_CMD_CSP_Save_Restore" header.
    BLOCK_COMMAND_CSP_TYPE_LOAD_DESCRIPTOR   = 4,   // 4:load descriptor, U#/T#/S#/C#/INS. Parsing cmd with
                                                    // "BLK_CMD_CSP_Ld_Des" header.
    BLOCK_COMMAND_CSP_TYPE_FORCE_FLUSH       = 5,   // 5:force set the TS#/U# part dirty in shadow buf
} BLOCK_COMMAND_CSP_TYPE;
typedef enum
{
    BLK_CMD_CSP_SAVE_RSTO_ADDRESS_MODE_ADDRESS= 0,  // When in this mode, 2 dw followed cmd header is just the
                                                    // save/retore address
    BLK_CMD_CSP_SAVE_RSTO_ADDRESS_MODE_OFFSET= 1,   // When in this mode, 2 dw followed cmd header is the offset,
                                                    // should add with save area base address first, then to do
                                                    // save/restore. Used in save DMA.
} BLK_CMD_CSP_SAVE_RSTO_ADDRESS_MODE;
typedef enum
{
    BLK_CMD_CSP_INDICATOR_INFO_OFF_MODE      = 0,   // 000:OFF mode
    BLK_CMD_CSP_INDICATOR_INFO_MCE_MODE      = 1,   // 001:MCE mode, High low ring buffer switch can only be in MCE
                                                    // dma boundary, for other dma any command boundary can
                                                    // switch into HI ringbuffer.
    BLK_CMD_CSP_INDICATOR_INFO_CS_MODE       = 2,   // 010:CS mode
    BLK_CMD_CSP_INDICATOR_INFO_3D_MODE       = 3,   // 011:3D mode
    BLK_CMD_CSP_INDICATOR_INFO_VCP_MODE      = 4,   // 100:VCP mode
} BLK_CMD_CSP_INDICATOR_INFO;
typedef enum
{
    BLK_CMD_CSP_LD_DES_ADDRESS_MODE_FOLLOWED_ADDRESS= 0,
                                                    // The first 2 dw followed are the prefetch address
    BLK_CMD_CSP_LD_DES_ADDRESS_MODE_FOLLOWED_OFFSET= 1,
                                                    // The first 2 dw followed are the byte offset, the base
                                                    // address is in the heap base address of csp's register.
} BLK_CMD_CSP_LD_DES_ADDRESS_MODE_FOLLOWED;
typedef enum
{
    BLK_CMD_CSP_LD_DES_ADDRESS_MODE_ADDRESS  = 0,   // The first 2 dw are the prefetch address
    BLK_CMD_CSP_LD_DES_ADDRESS_MODE_OFFSET   = 1,   // The first 2 dw are the byte offset, the base address is in the
                                                    // heap_reg[Heap_Idx]
} BLK_CMD_CSP_LD_DES_ADDRESS_MODE;
typedef enum
{
    BLK_CMD_CSP_LD_DES_DESC_TYPE_T           = 0,   // T sharp for TU block. Parsing the dw followed with
                                                    // BLK_CMD_CSP_Ld_Des_T_DWORDxx
    BLK_CMD_CSP_LD_DES_DESC_TYPE_S           = 1,   // S sharp for TU block. Parsing the dw followed with
                                                    // BLK_CMD_CSP_Ld_Des_T_DWORDxx
    BLK_CMD_CSP_LD_DES_DESC_TYPE_U           = 2,   // U sharp for EU. Parsing the dw followed with
                                                    // BLK_CMD_CSP_Ld_Des_T_DWORDxx
    BLK_CMD_CSP_LD_DES_DESC_TYPE_S_T         = 3,   // S and T sharp for TU block. Parsing the dw followed with
                                                    // BLK_CMD_CSP_Ld_Des_T_DWORDxx
    BLK_CMD_CSP_LD_DES_DESC_TYPE_C           = 4,   // Constant for EU block. Parsing the dw followed with
                                                    // BLK_CMD_CSP_Ld_Des_C_DWORDxx.
} BLK_CMD_CSP_LD_DES_DESC_TYPE;
typedef enum
{
    BLK_CMD_CSP_FORCE_FLUSH_REG_TYPE_TS = 0, //
    BLK_CMD_CSP_FORCE_FLUSH_REG_TYPE_U = 1, //
} BLK_CMD_CSP_FORCE_FLUSH_REG_TYPE;
typedef enum
{
    BLOCK_COMMAND_VIDEO_IRQ_NOP              = 0,   // 00: Nop
    BLOCK_COMMAND_VIDEO_IRQ_INTERRUPT        = 1,   // 01: interrupt when external fence done
} BLOCK_COMMAND_VIDEO_IRQ;
typedef enum
{
    BLOCK_COMMAND_VIDEO_EXTERNAL_FENCE_NOP   = 0,   // 0x00: Nop
    BLOCK_COMMAND_VIDEO_EXTERNAL_FENCE_EXTERNAL_FENCE= 1,
                                                    // 0x01: external fence, 3 DW will be followed: addr (1st),
                                                    // tag0 (2nd), tag1(3rd)
} BLOCK_COMMAND_VIDEO_EXTERNAL_FENCE;
typedef enum
{
    GP_DW_TYPE_GROUP_NUMBER                  = 0,
    GP_DW_TYPE_GLOBAL_SIZE                   = 1,
} GP_DW_TYPE;
typedef enum
{
    GP_MINOR_OPCODE_NORMAL_GP                = 0,   // nornal GP cmd for CS
    GP_MINOR_OPCODE_CS_FULL_CTX_END          = 1,   // CS full ctx end, add by SW. When CSP see this cmd, it can check
                                                    // whether there is cmd ready in CS LRB or 3D LRB for cs ctx. If
                                                    // CSP is parsing 3D LRB cs ctx, and the CS LRB is kicked off,
                                                    // then CSP would begin to parse cmd in CS LRB.
} GP_MINOR_OPCODE;
typedef enum
{
    TBR_INDICATOR_INDICATOR_INFO_BEGIN       = 0,   // TBR frame begin indicator
    TBR_INDICATOR_INDICATOR_INFO_FORCE_KICKOFF= 1,  // froce TAS_BE stop accumulating and start to render, both
                                                    // CSP and SW can set it, CSP will set it only when global
                                                    // registers changed, in this case, CSP will first send
                                                    // force-kickoff cmd to TAS_BE, then wait chip idle.
    TBR_INDICATOR_INDICATOR_INFO_END         = 2,   // TBR frame end indicator
} TBR_INDICATOR_INDICATOR_INFO;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////            CSP_OPCODES Block (Block ID = 0) Register Definitions                     ///////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct _Cmd_Skip                    // 0h: SKIP Command
{
    unsigned int     Dwc                   : 16; // The number Command will be skipped. In all following cmd,
                                                 // "Dwc" is count of dw followed cmd head.
    unsigned int     Reserved              : 12; // Reserved.
    unsigned int     Major_Opcode          : 4;  // 0000: SKIP.
} Cmd_Skip;

typedef struct _Cmd_Dma                     // 1h: Command dma. Dw followed normal/flush dma cmd is 2 dw.
{
    unsigned int     Dw_Num                : 18; // number of dw cmd in the dma cmd.
    unsigned int     Reserved              : 6;
    unsigned int     Special_Dma_Type      : 1;  // For 3D high RB, can't use shadow buffer. So, when shadow
                                                 // buffer is off(in 3D hiRB, CS ctx in CS RB), driver should use
                                                 // saveDMA with all query dump cmd, and restoreDMA with all
                                                 // set reg addr cmd; when shadwo buffer is on, driver should
                                                 // use saveDMA with save cmd, and restore DMA with restore
                                                 // cmd.
    unsigned int     Reset_Dma             : 1;  // When it's save/restore/flush DMA, force to get DMA from
                                                 // memory. If 0, csp and use local dma buffer data.
    unsigned int     Mode                  : 2;  // DMA mode. Except normal DMA cmd, there will be 2 more dword
                                                 // followed for context base address
    unsigned int     Major_Opcode          : 4;  // 0001: DMA (only allow L1 and L2 DMA in 3D low RB).
} Cmd_Dma;

/////////////////////////////////////////////////////////////////////////
// some other description about the command usage: Start
       typedef struct _Cmd_Dma_Address_Dword1       // 1h DWF1: DWORD for DMA_Address, low DW.
       {
            unsigned int     Address_Low32         : 32; // Here is the Low 32 bits for the 40 bits DW aligned addr.
       } Cmd_Dma_Address_Dword1;
       typedef struct _Cmd_Dma_Address_Dword2       // 1h DWF2: DWORD for DMA_Address, high DW.
       {
            unsigned int     Address_High8         : 8;  // DW aligned address, totally 40bits. Here is the High 8 bits
            unsigned int     L2_Cachable           : 1;  // whether all cmd in this DMA goes into L2 cache, csp bypass to
                                                         // mxu
            unsigned int     Reserved              : 23;
       } Cmd_Dma_Address_Dword2;
       typedef struct _Cmd_Ctx_Address_Dword3       // 1h DWF3: DWORD for CTX Base Address fro Save/Restore DMA, low DW.
       {
            unsigned int     Ctx_Base_Addr         : 29; //  // this is the aligned to 512DW base address of current ctx.
            unsigned int     Reserved              : 3;
       } Cmd_Ctx_Address_Dword3;
// some other description about the command usage: End
/////////////////////////////////////////////////////////////////////////


typedef struct _Cmd_Wait                    // 2h: Wait Command:external or internal wait.
{
    unsigned int     Dwc                   : 16; // dwc. 1, it's wait reference valud for "normal_wait"
                                                 // command; 2,for kkk wait, 0<dwc, and dwc*2 is total event
                                                 // count; 3,for vulkan global event sync, dwc=4, for event
                                                 // addr and event ref value.
    unsigned int     Wait_Mode             : 2;
    unsigned int     Method                : 1;  // Wait Method, only valid for normal internal wait cmd
    unsigned int     Station_Id            : 3;  // Only valid for normal internal wait cmd
    unsigned int     Slot_Id               : 5;  // Slot ID. Totally 32 fence counters, and driver can
                                                 // flexible choose some of them for different CS RB.
    unsigned int     Reserved              : 1;  // reserved
    unsigned int     Major_Opcode          : 4;  // 0010:Wait (external/internal wait)
} Cmd_Wait;
/////////////////////////////////////////////////////////////////////////
// some other description about the command usage: Start
       typedef struct _Cmd_Wait_Kkk_Dword1          // 2h DWF1; DWORD for kkk wait event
       {
            unsigned int     Event_Flag_Idx1       : 16; // totally 1024 events, of each, flag is 2 bits. For 1024
                                                         // events, only 11 bits are enough, here we use 16 bits for
                                                         // event idx for convenience. If the idx==0xffff, this idx is
                                                         // invalid.
            unsigned int     Event_Flag_Idx2       : 16;
       } Cmd_Wait_Kkk_Dword1;
       typedef struct _Cmd_Wait_External_Dword1     // 2h DWF1; DWORD for vulkan global event sync
       {
            unsigned int     Address_Low32         : 16; // low 32b for the 40b event address, should be dw aligned,
                                                         // [1:0]=0.
       } Cmd_Wait_External_Dword1;
       typedef struct _Cmd_Wait_External_Dword2     // 2h DWF2; DWORD for vulkan global event sync
       {
            unsigned int     Address_High8         : 8;  // high 8b for the 40b event address.
            unsigned int     Reserved              : 24;
       } Cmd_Wait_External_Dword2;
       typedef struct _Cmd_Wait_External_Dword3     // 2h DWF3; DWORD for vulkan global event sync
       {
            unsigned int     Event_Cnt_Ref_Low32   : 32; // low 32b for the global event cnt ref value
       } Cmd_Wait_External_Dword3;
       typedef struct _Cmd_Wait_External_Dword4     // 2h DWF4; DWORD for vulkan global event sync
       {
            unsigned int     Event_Cnt_Ref_High32  : 32; // high 32b for the global event cnt ref value
       } Cmd_Wait_External_Dword4;
// some other description about the command usage: End
/////////////////////////////////////////////////////////////////////////


typedef struct _Cmd_Query_Dump              // 3h: Query Dump Command:Query Dump is the command to dump out GPU
                                            // status.
{
    unsigned int     Dwc                   : 3;  // dw followed cmd uint. For normal query dump cmd, no matter
                                                 // addr/offs mode, the dwc=2; for vulkan copy query result
                                                 // cmd, dwc=4.
    unsigned int     Reserved              : 1;
    unsigned int     Query_Ready_En        : 1;  // when query cnt into memory, after register data,
                                                 // csp/GPCTop should pack another 64b into the
                                                 // tail(value=1), to hint the query ready for sw/hw.
    unsigned int     Channle_Id            : 2;  // for MMU query dump
    unsigned int     Dwc_To_Dump           : 13; // Dw number to query dump when Cmd_Type=Normal_Query; And
                                                 // dw number of begin counters when Cmd_Type=Cpy_Qry_Res.
    unsigned int     Address_Mode          : 1;  // Only valid when Cmd_Type=Normal_Query
    unsigned int     Timestamp_En          : 1;  // Only valid when Cmd_Type=Cpy_Qry_Res
    unsigned int     Cmd_Type              : 1;
    unsigned int     Block_Id              : 5;  // Block ID
    unsigned int     Major_Opcode          : 4;  // 0011: QueryDump (including external fence, dump
                                                 // status/context to host)
} Cmd_Query_Dump;
/////////////////////////////////////////////////////////////////////////
// some other description about the command usage: Start
       typedef struct _Cmd_Query_Dump_Address_Dword1// 3h DWF1; DWORD for Query Dump Address when Address_mode=0.
       {
            unsigned int     Address_Low32         : 32; // Here is the Low 32 bits for the 40 bits dw aligned address.
                                                         // According to STO-HW, the address for GPCPBE to query dump
                                                         // should aligned to 512b.
       } Cmd_Query_Dump_Address_Dword1;
       typedef struct _Cmd_Query_Dump_Address_Dword2// 3h DWF2; DWORD for Query Dump Address when Address_mode=0.
       {
            unsigned int     Address_High8         : 8;  // Here is the High 8 bits for the 40 bits dw aligned address
            unsigned int     Reserved              : 11;
            unsigned int     Reg_Offset            : 13; // register start offset for dump.
       } Cmd_Query_Dump_Address_Dword2;
       typedef struct _Cmd_Query_Dump_Offset_Dword1 // 3h DWF1; DWORD for Query Dump Offset when Address_mode=1.
       {
            unsigned int     Offset_Low32          : 32;
       } Cmd_Query_Dump_Offset_Dword1;
       typedef struct _Cmd_Query_Dump_Offset_Dword2 // 3h DWF2; DWORD for Query Dump Offset when Address_mode=1.
       {
            unsigned int     Offset_High8          : 8;
            unsigned int     Reserved              : 11;
            unsigned int     Reg_Offset            : 13; // register start offset for dump.
       } Cmd_Query_Dump_Offset_Dword2;
       typedef struct _Cmd_Cpy_Qry_Res_Dword1       // 3h DWF1; DWORD for vulkan copy query results cmd.
       {
            unsigned int     Begin_Query_Addr_Low32
                                                   : 32; // Low 32b for the 37b begin query address, This address is
                                                         // aligned to 64b, so just delete the lowest 3 bits here
       } Cmd_Cpy_Qry_Res_Dword1;

       typedef struct _Cmd_Cpy_Qry_Res_Dword2       // 3h DWF2; DWORD for vulkan copy query results cmd.
       {
            unsigned int     Begin_Query_Addr_High5
                                                   : 5;  // High 5b for the 37b begin query address, This address is
                                                         // aligned to 64b.
            unsigned int     L2_Cachable           : 1;  // whether the begin/end data goes into L2 cache, csp bypass
                                                         // to mxu
            unsigned int     Bit64_En              : 1;
            unsigned int     Avaliability_Bit      : 1;
            unsigned int     Partial_Bit           : 1;
            unsigned int     Slice_Num             : 4;  // 0 based value. When Slice_Num=0, means 1 valid slice. At
                                                         // most 12 valid slices in E3K.
            unsigned int     Reserved              : 3;
            unsigned int     End_Query_Ddw_Offs    : 8;  // ddw offset from the begin query addr to the end query addr.
                                                         // Get the end query cnt from this address.
            unsigned int     End_Query_Ava_Ddw_Offs
                                                   : 8;  // ddw offset from the begin query addr to the end query
                                                         // avaliable addr.  Get the avaliability_data from this
                                                         // address.
       } Cmd_Cpy_Qry_Res_Dword2;
       typedef struct _Cmd_Cpy_Qry_Res_Dword3       // 3h DWF3: DWORD for vulkan copy query results cmd.
       {
            unsigned int     Dest_Addr_Low32       : 32; // Low 32b for the 38b dst address, This address is aligned to
                                                         // 32b, so just delete the lowest 2 bits here
       } Cmd_Cpy_Qry_Res_Dword3;

       typedef struct _Cmd_Cpy_Qry_Res_Dword4       // 3h DWF4: DWORD for vulkan copy query results cmd.
       {
            unsigned int     Dest_Addr_High8       : 8;  // High 8b for the 37b dst address, This address is aligned to
                                                         // 32b.
            unsigned int     Reserved              : 24;
       } Cmd_Cpy_Qry_Res_Dword4;
// some other description about the command usage: End
/////////////////////////////////////////////////////////////////////////


typedef struct _Cmd_Set_Register            // 4h: Set Register Command:Set Register Command is CSP only command,
                                            // CSP would update shadow buffer or just bypass to GPC_Pool.
{
    unsigned int     Dwc                   : 7;  // Indicate the dw cnt followed the cmd head.
                                                 //  When Mask_En==0 && Addr_En==0, dwc >0, all reg data;
                                                 //  When Mask_En==1 && Addr_En==0, dwc=2, dw[0] is reg data,
                                                 // and dw[1] is mask, only set 1 reg once;
                                                 //  When Mask_En==0 && Addr_En==1, dwc=2 for reg address in
                                                 // memory, and how many reg should be set is referred to
                                                 // "Reg_Cnt";
                                                 //  When Mask_En==1 && Addr_En==1, dwc=3, dw[0] is mask, and
                                                 // dw[1]+dw[2] are reg address in memory, only set 1 reg once.
    unsigned int     Address_Mode          : 1;  // Only valid when Addr_En==1 && Mask_En==0.
    unsigned int     Addr_En               : 1;
    unsigned int     Mask_En               : 1;
    unsigned int     Start_Offset          : 13; // each block has up to 2^13 registers
    unsigned int     Block_Id              : 5;  // Block ID. For INS/C#, block id is Eu_Fs; for T#/S#, block id
                                                 // is TU_FE or TU_BE; for U#, block id is WLS_FE or WLS_BE
    unsigned int     Major_Opcode          : 4;  // 0100: SetRegister  (with multiple DW followed)
} Cmd_Set_Register;
/////////////////////////////////////////////////////////////////////////
// some other description about the command usage: Start
       typedef struct _Cmd_Set_Register_Addr_Dword1 // 4h DWF1: when Addr_En=1&&Address_mode=0, then it is address dw
       {
            unsigned int     Address_Low32         : 32; // Here is the low 32 bits for the 40 bits dw aligned address
       } Cmd_Set_Register_Addr_Dword1;
       typedef struct _Cmd_Set_Register_Addr_Dword2 // 4h DWF2: when Addr_En=1, then it is address dw
       {
            unsigned int     Address_High8         : 8;  // Here is the high 8 bits for the 40 bits dw aligned address
            unsigned int     L2_Cachable           : 1;  // whether the reg data goes into L2 cache, csp bypass to mxu
            unsigned int     If_Physical           : 1;  // whether the address from sw is virtual or physical
            unsigned int     Reserved              : 9;
            unsigned int     Reg_Cnt               : 13; // When Addr_En, this is register number to be set.
       } Cmd_Set_Register_Addr_Dword2;
       typedef struct _Cmd_Set_Register_Offset_Dword1
                                                    // 4h DWF1: when Addr_En=1&&Address_mode=1, then it is offset dw,
                                                    // should add save area base address first, then to do prefetch
       {
            unsigned int     Offset_Low32          : 32; // Here is the low 32 bits for the 40 bits dw aligned address
       } Cmd_Set_Register_Offset_Dword1;
       typedef struct _Cmd_Set_Register_Offset_Dword2
                                                    // 4h DWF2: when Addr_En=1&&Address_mode=1, then it is offset dw,
                                                    // should add save area base address first, then to do prefetch
       {
            unsigned int     Offset_High8          : 8;  // Here is the high 8 bits for the 40 bits dw aligned address
            unsigned int     Reserved              : 11;
            unsigned int     Reg_Cnt               : 13; // When Addr_En, this is register number to be set.
       } Cmd_Set_Register_Offset_Dword2;
       typedef struct _Cmd_Set_Register_Mask_Addr_Dword1
                                                    // 4h DWF1: when Addr_En=1&&Mask_En==1, this is the 32b mask
       {
            unsigned int     Mask                  : 32;
       } Cmd_Set_Register_Mask_Addr_Dword1;
       typedef struct _Cmd_Set_Register_Mask_Addr_Dword2
                                                    // 4h DWF2: when Addr_En=1&&Mask_En==1, this is the low 32b for reg data
                                                    // address
       {
            unsigned int     Address_Low32         : 32; // Here is the low 32 bits for the 40 bits dw aligned address
       } Cmd_Set_Register_Mask_Addr_Dword2;
       typedef struct _Cmd_Set_Register_Mask_Addr_Dword3
                                                    // 4h DWF3: when Addr_En=1&&Mask_En==1, this is the high 8b for reg data
                                                    // address
       {
            unsigned int     Address_High8         : 8;  // Here is the high 8 bits for the 40 bits dw aligned address
            unsigned int     L2_Cachable           : 1;  // whether the reg data goes into L2 cache, csp bypass to mxu
            unsigned int     If_Physical           : 1;  // whether the address from sw is virtual or physical
            unsigned int     Reserved              : 22;
       } Cmd_Set_Register_Mask_Addr_Dword3;
// some other description about the command usage: End
/////////////////////////////////////////////////////////////////////////


typedef struct _Cmd_Execute_Indirect        // 5h: ExecuteIndirect Command
{
    unsigned int     Cmd_List_Dwc          : 16; // how many dw are used for the command list template, and the
                                                 // command list template is in memory.
    unsigned int     Reserved              : 10;
    unsigned int     Countbuf_En           : 1;  // Whether there is a valid count buffer. If yes, the dwc
                                                 // followed should be
                                                 // 8(1maxCnt+1Stride+2CmdListTemp_Addr+2CountBuf_Addr+2CountBuf_Offs);
                                                 // otherwise there are
                                                 // 4(1maxCnt+1Stride+2CmdListTemp_Addr).
    unsigned int     Major_Opcode          : 5;  // 0005: DX12's ExecuteIndirect
} Cmd_Execute_Indirect;
/////////////////////////////////////////////////////////////////////////
// some other description about the command usage: Start
       typedef struct _Cmd_Execute_Indirectdword1   // 5h DWF1
       {
            unsigned int     Maxcommandcount       : 32; // max draw count, always have this dw followed.
       } Cmd_Execute_Indirectdword1;
       typedef struct _Cmd_Execute_Indirectdword2   // 5h DWF2
       {
            unsigned int     Argument_Stride       : 32; // totally indirect data byte size for each loop, always have
                                                         // this dw followed.
       } Cmd_Execute_Indirectdword2;
       typedef struct _Cmd_Execute_Indirectdword3   // 5h DWF3
       {
            unsigned int     Cmdlist_Addr_Low32    : 32; // low 32b for the cmd list template byte address.
       } Cmd_Execute_Indirectdword3;
       typedef struct _Cmd_Execute_Indirectdword4   // 5h DWF4
       {
            unsigned int     Cmdlist_Addr_High8    : 8;  // high 8b for the cmd list template byte address.
            unsigned int     Reserved              : 24;
       } Cmd_Execute_Indirectdword4;
       typedef struct _Cmd_Execute_Indirectdword5   // 5h DWF5
       {
            unsigned int     Countbuffer_Addr_Low32
                                                   : 32; // low 32b for the count buffer byte address. Only have this dw
                                                         // when CountBuf_En=1.
       } Cmd_Execute_Indirectdword5;
       typedef struct _Cmd_Execute_Indirectdword6   // 5h DWF6
       {
            unsigned int     Countbuffer_Addr_High8
                                                   : 8;  // high 8b for the count buffer byte address. Only have this dw
                                                         // when CountBuf_En=1.
            unsigned int     Reserved              : 24;
       } Cmd_Execute_Indirectdword6;
       typedef struct _Cmd_Execute_Indirectdword7   // 5h DWF7
       {
            unsigned int     Countbuffer_Offs_Low32
                                                   : 32; // low 32b for the count buffer byte offset. Only have this dw
                                                         // when CountBuf_En=1.
       } Cmd_Execute_Indirectdword7;
       typedef struct _Cmd_Execute_Indirectdword8   // 5h DWF8
       {
            unsigned int     Countbuffer_Offs_High8
                                                   : 8;  // high 8b for the count buffer byte offset. Only have this dw
                                                         // when CountBuf_En=1.
            unsigned int     Reserved              : 24;
       } Cmd_Execute_Indirectdword8;
// some other description about the command usage: End
/////////////////////////////////////////////////////////////////////////


typedef struct _Cmd_Set_Object              // 6h: set object. Use to update predicate_en register.
{
    unsigned int     Dwc                   : 2;  // 2 for set object cmd, for prefetch address
    unsigned int     Object_Type           : 2;
    unsigned int     Directly_Result_B32   : 1;  // Only valid when object_type=3.
    unsigned int     Reserved              : 21;
    unsigned int     Predicate_Not         : 1;
    unsigned int     Hint                  : 1;
    unsigned int     Major_Opcode          : 4;  // 0110: set object for predicate dip
} Cmd_Set_Object;

/////////////////////////////////////////////////////////////////////////
// some other description about the command usage: Start
       typedef struct _Cmd_Set_Object_Dword1        // 6h DWF1: DWORD for Set_object_Address
       {
            unsigned int     Address_Low32         : 32; // Here is the low 32 bits for the 40 bits ddw aligned address
       } Cmd_Set_Object_Dword1;
       typedef struct _Cmd_Set_Object_Dword2        // 6h DWF2: DWORD for Set_object_Address
       {
            unsigned int     Address_High8         : 8;  // Here is the High 8 bits for the 40 bits ddw aligned address
            unsigned int     Reserved              : 24;
       } Cmd_Set_Object_Dword2;
// some other description about the command usage: End
/////////////////////////////////////////////////////////////////////////


typedef struct _Cmd_Dip                     // 7h: DrawIndexedPrimitive Command
{
    unsigned int     Predicate_En          : 1;
    unsigned int     Indirect              : 1;  // 1: DX11 indirect draw. All the parameters are read from
                                                 // memory
    unsigned int     Draw_Mode             : 3;  // draw mode
    unsigned int     Instance_En           : 1;  // Instancing Enabled. 32-bit instance counter present.
    unsigned int     Start_Index_Valid     : 1;  // Whether there is startindexlocation in the draw api. When
                                                 // 1, dw "Start_Index" is in the dw followed dip cmd head;
                                                 // else, no such dw followed. For IMM draw, when OGL/OES,
                                                 // should set it to 1, and set the "fist" parameter into
                                                 // "start_index" in dip cmd, csp would use "fist" to auto
                                                 // generated Ib data; when DX IMM draw, just set this bit to 0,
                                                 // and csp would use default value 0 to auto generated IB data.
    unsigned int     Start_Vertex_Valid    : 1;  // Whether there is
                                                 // startvertexlocation/basevertexlocation in the draw
                                                 // api. When 1, dw "Start_Vertex" is in the dw followed dip cmd
                                                 // head; else, no such dw followed. For IMM draw, when
                                                 // OGL/OES, should set it to 0, and csp would send 0 as
                                                 // startvertexlocation into eu; when DX IMM draw, just set
                                                 // this bit to 1, and csp would use "Start_vertex" to send to
                                                 // EU.
    unsigned int     Start_Instance_Valid  : 1;  // For direct draw, use this bit to hint whether there is
                                                 // startinstancelocation in the draw api. When 1, dw
                                                 // "Start_Instance" is in the dw followed dip cmd head; else,
                                                 // no such dw followed. 2. For indirect draw, use this bit to
                                                 // hint whether the dw[3](for drawIndirectInstanced) or
                                                 // the dw[4](for drawIndirectIndexedInstanced) is the
                                                 // baseInstance(OGL) or just the reserved dw(OES).
    unsigned int     P_Type                : 5;  // Primitive Type for input
    unsigned int     Indirect_Drawcnt_En   : 1;  // when the drawCnt is in memory, should set this bit to 1(for
                                                 // vkCmdDrawIndirectCountAMD/vkCmdDrawIndexedIndirectCountAMD).
                                                 // When this bit is set to 1, the "reg_IA_DrawCount" stores
                                                 // the MaxDrawCnt; else, the "reg_IA_DrawCount" is just the
                                                 // drawCnt when MultiDraw_En. This bit can only be 1 for
                                                 // indirect draw.
    unsigned int     Indirect_Ib_En        : 1;  // Whether the IB information is indirect mode, can only be 1
                                                 // for indirect draw. If the IB information is indirect, then
                                                 // driver only provide a IB_Infor_Addr, csp prefetch
                                                 // IB_Addr(2dw)+IB_ByteSize(1dw)+IB_Fmt(1dw) from
                                                 // memory, IB_Offset is default to 0 here; if the IB
                                                 // information is direct, driver append the IB_Addr(2dw,
                                                 // per cmd)+IB_ByteSize(1dw, per cmd)+IB_Fmt(1dw, per
                                                 // cmd)+IB_ByteOffset(1dw, per draw) in dw followed.
    unsigned int     Patch_Vertex_Count    : 6;  // 1~32, control points count for patch list.
    unsigned int     Reserved              : 5;
    unsigned int     Inherit_Ib_Infor      : 1;  // 0: not inherit from last indexed dip cmd; 1: inherit all IB infor(including IB base addr, IB byte size, IB fmt) from the last indexed dip cmd, the IB infor in current dip cmd is useless.
    unsigned int     Major_Opcode          : 4;  // 0111: DrawIndexPrimitive (with index to be loaded)
} Cmd_Dip;

/////////////////////////////////////////////////////////////////////////
// some other description about the command usage: Start
       typedef struct _Cmd_Dip_Drawauto_Dw1         // 7h DWF1: for non-instanced drawauto
       {
            unsigned int     Draw_Count            : 32; // For non-instanced drawAuto, only this dw followed, to
                                                         // easy HW implementation. And driver would always set this
                                                         // field into 1.
       } Cmd_Dip_Drawauto_Dw1;
       typedef struct _Cmd_Dip_Insdrawauto_Dw1      // 7h DWF1: for instance drawauto, OGL
                                                    // DrawTransformFeedbackInstanced/DrawTransformFeedbackStreamInstanced,
                                                    // and Vulkan's vkCmdDrawIndirectByteCountEXT.
       {
            unsigned int     Draw_Count            : 32; // For instanced drawAuto, 2/3 dw followed, and the
                                                         // Draw_Count should always be set to 1 by driver. Always have
                                                         // this dw in instanced drawAuto.
       } Cmd_Dip_Insdrawauto_Dw1;
       typedef struct _Cmd_Dip_Insdrawauto_Dw2      // 7h DWF2: for instance drawauto, OGL
                                                    // DrawTransformFeedbackInstanced/DrawTransformFeedbackStreamInstanced,
                                                    // and Vulkan's vkCmdDrawIndirectByteCountEXT.
       {
            unsigned int     Start_Instance        : 32; // Only when "mode==DrawAuto && Instance_En==1", this dip
                                                         // is instanced-drawAuto. And this dw is the instance_Cnt.
                                                         // Only when "Start_Instance_En==1", have this dw
                                                         // followed. Only vulkan's driver would set
                                                         // "Start_Instance_En" into 1 when DrawAuto, for
                                                         // vkCmdDrawIndirectByteCountEXT API.
       } Cmd_Dip_Insdrawauto_Dw2;
       typedef struct _Cmd_Dip_Insdrawauto_Dw3      // 7h DWF3: for instance drawauto, OGL
                                                    // DrawTransformFeedbackInstanced/DrawTransformFeedbackStreamInstanced,
                                                    // and Vulkan's vkCmdDrawIndirectByteCountEXT.
       {
            unsigned int     Instance_Count        : 32; // Only when "mode==DrawAuto && Instance_En==1", this dip
                                                         // is instanced-drawAuto. And this dw is the instance_Cnt.
                                                         // Always have this dw when instanced drawAuto.
       } Cmd_Dip_Insdrawauto_Dw3;
       typedef struct _Cmd_Dip_Direct_Imm_Dw1       // 7h DWF1: for direct multi-draw, without index buf
       {
            unsigned int     Draw_Count            : 32; // Only when "mode==IMM && inidrect_En==0", can be direct
                                                         // IMM draw. Always has this dw for direct multi-draw. Per
                                                         // command dw.
       } Cmd_Dip_Direct_Imm_Dw1;
       typedef struct _Cmd_Dip_Direct_Imm_Dw2       // 7h DWF2: for direct multi-draw, without index buf
       {
            unsigned int     Start_Instance        : 32; // There won't be such dword if "Start_Instance_Valid==0".
                                                         // For multiDirectDraw, this dw is for per command, all draw
                                                         // share the same Start_Instance.
       } Cmd_Dip_Direct_Imm_Dw2;
       typedef struct _Cmd_Dip_Direct_Imm_Dw3       // 7h DWF3: for direct multi-draw, without index buf
       {
            unsigned int     Instance_Count        : 32; // There won't be such dword if "Instance_En==0". For
                                                         // multiDirectDraw, this dw is for per command, all draw
                                                         // share the same Instance_Count.
       } Cmd_Dip_Direct_Imm_Dw3;
       typedef struct _Cmd_Dip_Direct_Imm_Dw4       // 7h DWF4: for direct multi-draw, without index buf
       {
            unsigned int     Vertex_Count          : 32; // Always have this dw for direct IMM draw. For
                                                         // multiDirectDraw, this dw is for per draw, and each draw has
                                                         // its Index_Count.
       } Cmd_Dip_Direct_Imm_Dw4;
       typedef struct _Cmd_Dip_Direct_Imm_Dw5       // 7h DWF5: for direct multi-draw, without index buf
       {
            unsigned int     Start_Vertex          : 32; // There won't be such dword if "Start_Vertex_Valid==0".
                                                         // For muitlDirectDraw, if Start_Vertex_Valid=1, then
                                                         // this dw is for per draw, each draw has its Start_Vertex.
       } Cmd_Dip_Direct_Imm_Dw5;
       typedef struct _Cmd_Dip_Direct_Imm_Dw6       // 7h DWF6: for direct multi-draw, without index buf
       {
            unsigned int     Start_Index           : 32; // There won't be such dword if "Start_Index_Valid==0". For
                                                         // muitlDirectDraw, if Start_Index_Valid=1, then this dw
                                                         // is for per draw, each draw has its Start_Index.
       } Cmd_Dip_Direct_Imm_Dw6;
       typedef struct _Cmd_Dip_Direct_Idx_Dw1       // 7h DWF1: for direct multi-draw, with index buf
       {
            unsigned int     Draw_Count            : 32; // Only when "mode==index && inidrect_En==0", can be direct
                                                         // indexed draw. Always has this dw for direct multi-draw.
                                                         // Per command dw.
       } Cmd_Dip_Direct_Idx_Dw1;
       typedef struct _Cmd_Dip_Direct_Idx_Dw2       // 7h DWF2: for direct multi-draw, with index buf
       {
            unsigned int     Start_Instance        : 32; // There won't be such dword if "Start_Instance_Valid==0".
                                                         // For multiDirectDraw, this dw is for per command, all draw
                                                         // share the same Start_Instance.
       } Cmd_Dip_Direct_Idx_Dw2;
       typedef struct _Cmd_Dip_Direct_Idx_Dw3       // 7h DWF3: for direct multi-draw, with index buf
       {
            unsigned int     Instance_Count        : 32; // There won't be such dword if "Instance_En==0". For
                                                         // multiDirectDraw, this dw is for per command, all draw
                                                         // share the same Instance_Count.
       } Cmd_Dip_Direct_Idx_Dw3;
       typedef struct _Cmd_Dip_Direct_Idx_Dw4       // 7h DWF4: for direct multi-draw, with index buf
       {
            unsigned int     Ib_Base_Addr_Low32    : 32; // Always have this dw for indexed direct multi-draw, and
                                                         // this dw is for per command.
       } Cmd_Dip_Direct_Idx_Dw4;
       typedef struct _Cmd_Dip_Direct_Idx_Dw5       // 7h DWF5: for direct multi-draw, with index buf
       {
            unsigned int     Ib_Base_Addr_High8    : 8;  // Always have this dw for indexed direct multi-draw, and
                                                         // this dw is for per command.
            unsigned int     L2_Cachable           : 1;  // whether index buffer data goes into L2 cache, csp bypass to
                                                         // mxu
            unsigned int     Outofbound_Mode       : 1;  // For out of boundary index, use this bit to set the value. 0->
                                                         // set to 0; 1-> set to 0xffffffff.
            unsigned int     Reserved              : 22;
       } Cmd_Dip_Direct_Idx_Dw5;
       typedef struct _Cmd_Dip_Direct_Idx_Dw6       // 7h DWF6: for direct multi-draw, with index buf
       {
            unsigned int     Ib_Byte_Size          : 32; // Always have this dw for indexed direct multi-draw, and
                                                         // this dw is for per command.
       } Cmd_Dip_Direct_Idx_Dw6;
       typedef struct _Cmd_Dip_Direct_Idx_Dw7       // 7h DWF7: for direct multi-draw, with index buf
       {
            unsigned int     Ib_Fmt                : 10; // Always have this dw for indexed direct multi-draw, and
                                                         // this dw is for per command.
            unsigned int     Reserved              : 22;
       } Cmd_Dip_Direct_Idx_Dw7;
       typedef struct _Cmd_Dip_Direct_Idx_Dw8       // 7h DWF8: for direct multi-draw, with index buf
       {
            unsigned int     Index_Count           : 32; // Always have this dw for direct indexed multi-draw. For
                                                         // multiDirectDraw, this dw is for per draw, and each draw has
                                                         // its Index_Count.
       } Cmd_Dip_Direct_Idx_Dw8;
       typedef struct _Cmd_Dip_Direct_Idx_Dw9       // 7h DWF9: for direct multi-draw, with index buf
       {
            unsigned int     Start_Vertex          : 32; // There won't be such dword if "Start_Vertex_Valid==0".
                                                         // For muitlDirectDraw, if Start_Vertex_Valid=1, then
                                                         // this dw is for per draw, each draw has its Start_Vertex.
       } Cmd_Dip_Direct_Idx_Dw9;
       typedef struct _Cmd_Dip_Direct_Idx_Dw10      // 7h DWF10: for direct multi-draw, with index buf
       {
            unsigned int     Start_Index           : 32; // There won't be such dword if "Start_Index_Valid==0". For
                                                         // muitlDirectDraw, if Start_Index_Valid=1, then this dw
                                                         // is for per draw, each draw has its Start_Index.
       } Cmd_Dip_Direct_Idx_Dw10;
       typedef struct _Cmd_Dip_Direct_Idx_Dw11      // 7h DW11: for direct multi-draw, with index buf
       {
            unsigned int     Ib_Offset             : 32; // Always have this dw for direct indexed multi-draw. For
                                                         // multiDirectDraw, this dw is for per draw, and each draw has
                                                         // its Index_Count.
       } Cmd_Dip_Direct_Idx_Dw11;
       typedef struct _Cmd_Dip_Indirect_Imm_Dw1     // 7h DWF1: for indirect IMM multi-draw
       {
            unsigned int     Draw_Count            : 32; // Always have this dw. Per command.
       } Cmd_Dip_Indirect_Imm_Dw1;
       typedef struct _Cmd_Dip_Indirect_Imm_Dw2     // 7h DWF2: for indirect IMM multi-draw
       {
            unsigned int     Drawcount_Addr_Low32  : 32; // Have this dw only when Indirect_DrawCnt_En=1, to give the
                                                         // addr to prefetch the drawCnt of
                                                         // vkCmdDrawIndirectCountAMD/vkCmdDrawIndexedIndirectCountAMD.
       } Cmd_Dip_Indirect_Imm_Dw2;
       typedef struct _Cmd_Dip_Indirect_Imm_Dw3     // 7h DWF3: for indirect IMM multi-draw
       {
            unsigned int     Drawcount_Addr_High8  : 8;  // Have this dw only when Indirect_DrawCnt_En=1, to give the
                                                         // addr to prefetch the drawCnt of
                                                         // vkCmdDrawIndirectCountAMD/vkCmdDrawIndexedIndirectCountAMD.
            unsigned int     L2_Cachable           : 1;  // whether draw cnt goes into L2 cache, csp bypass to mxu
            unsigned int     Reserved              : 23;
       } Cmd_Dip_Indirect_Imm_Dw3;
       typedef struct _Cmd_Dip_Indirect_Imm_Dw4     // 7h DWF4: for indirect IMM multi-draw
       {
            unsigned int     Para_Base_Addr_Low32  : 32; // Always have this dw for inidrect draw. Per command.
       } Cmd_Dip_Indirect_Imm_Dw4;
       typedef struct _Cmd_Dip_Indirect_Imm_Dw5     // 7h DWF5: for indirect IMM multi-draw
       {
            unsigned int     Para_Base_Addr_High8  : 8;  // Always have this dw for inidrect draw. Per command.
            unsigned int     L2_Cachable           : 1;  // whether parameter data goes into L2 cache, csp bypass to
                                                         // mxu
            unsigned int     Reserved              : 23;
       } Cmd_Dip_Indirect_Imm_Dw5;
       typedef struct _Cmd_Dip_Indirect_Imm_Dw6     // 7h DWF6: for indirect IMM multi-draw
       {
            unsigned int     Para_Stride           : 32; // Always have this dw for inidrect draw. Dw to calculate
                                                         // parameter prefetch address for multiple indirect draw.
                                                         // If Para_stride==0, then the prefetch addr for the i-th
                                                         // indirect draw is para_base_addr+i*16(or 20 or indexed
                                                         // draw); if stride!=0, then the prefetch addr for the i-th
                                                         // indirect draw is para_base_Addr+i*stride. Per command.
       } Cmd_Dip_Indirect_Imm_Dw6;
       typedef struct _Cmd_Dip_Idr_Idx_Imm_Ib_Dw1   // 7h DWF1: for indirect indexed multi-draw, with IMM IB information in
                                                    // dw followed.
       {
            unsigned int     Draw_Count            : 32; // Always have this dw. Per command.
       } Cmd_Dip_Idr_Idx_Imm_Ib_Dw1;
       typedef struct _Cmd_Dip_Idr_Idx_Imm_Ib_Dw2   // 7h DWF2:  for indirect indexed multi-draw, with IMM IB information in
                                                    // dw followed.
       {
            unsigned int     Drawcount_Addr_Low32  : 32; // Have this dw only when Indirect_DrawCnt_En=1, to give the
                                                         // addr to prefetch the drawCnt of
                                                         // vkCmdDrawIndirectCountAMD/vkCmdDrawIndexedIndirectCountAMD.
       } Cmd_Dip_Idr_Idx_Imm_Ib_Dw2;
       typedef struct _Cmd_Dip_Idr_Idx_Imm_Ib_Dw3   // 7h DWF3:  for indirect indexed multi-draw, with IMM IB information in
                                                    // dw followed.
       {
            unsigned int     Drawcount_Addr_High8  : 8;  // Have this dw only when Indirect_DrawCnt_En=1, to give the
                                                         // addr to prefetch the drawCnt of
                                                         // vkCmdDrawIndirectCountAMD/vkCmdDrawIndexedIndirectCountAMD.
            unsigned int     L2_Cachable           : 1;  // whether draw cnt goes into L2 cache, csp bypass to mxu
            unsigned int     Reserved              : 23;
       } Cmd_Dip_Idr_Idx_Imm_Ib_Dw3;

       typedef struct _Cmd_Dip_Idr_Idx_Imm_Ib_Dw4
       {
           unsigned int Ib_Base_Addr_Low32 : 32; //Only have this dw when "mode==index && Indirect_IB_En==0". DWORD for index buffer addr of all indexed draw. Per command for indirect draw.
       } Cmd_Dip_Idr_Idx_Imm_Ib_Dw4;
       typedef struct _Cmd_Dip_Idr_Idx_Imm_Ib_Dw5
       {
           unsigned int Ib_Base_Addr_High8 : 8; //Only have this dw when "mode==index && Indirect_IB_En==0". DWORD for index buffer addr of all indexed draw. Per command for indirect draw.
           unsigned int L2_Cachable : 1; //whether index buffer data goes into L2 cache, csp bypass to mxu
           unsigned int Outofbound_Mode : 1; //For out of boundary index, use this bit to set the value. 0-> set to 0; 1-> set to 0xffffffff.
           unsigned int Reserved : 22; //
       } Cmd_Dip_Idr_Idx_Imm_Ib_Dw5;
       typedef struct _Cmd_Dip_Idr_Idx_Imm_Ib_Dw6
       {
           unsigned int Ib_Byte_Size : 32; //Only have this dw when "mode==index && Indirect_IB_En==0". DWORD for index buffer addr of all indexed draw. Per command for indirect draw.
       } Cmd_Dip_Idr_Idx_Imm_Ib_Dw6;
       typedef struct _Cmd_Dip_Idr_Idx_Imm_Ib_Dw7
       {
           unsigned int Ib_Fmt : 10; //Only have this dw when "mode==index && Indirect_IB_En==0". DWORD for index buffer addr of all indexed draw. Per command for indirect draw.
           unsigned int Reserved : 22; //
       } Cmd_Dip_Idr_Idx_Imm_Ib_Dw7;
       typedef struct _Cmd_Dip_Idr_Idx_Imm_Ib_Dw8
       {
           unsigned int Para_Base_Addr_Low32 : 32; //Always have this dw for inidrect draw. Per command.
       } Cmd_Dip_Idr_Idx_Imm_Ib_Dw8;
       typedef struct _Cmd_Dip_Idr_Idx_Imm_Ib_Dw9
       {
           unsigned int Para_Base_Addr_High8 : 8; //Always have this dw for inidrect draw. Per command.
           unsigned int L2_Cachable : 1; //whether parameter data goes into L2 cache, csp bypass to mxu
           unsigned int Reserved : 23; //
       } Cmd_Dip_Idr_Idx_Imm_Ib_Dw9;
       typedef struct _Cmd_Dip_Idr_Idx_Imm_Ib_Dw10
       {
           unsigned int Para_Stride : 32; //Always have this dw for inidrect draw. Dw to calculate parameter prefetch address for multiple indirect draw. If Para_stride==0, then the prefetch addr for the i-th indirect draw is para_base_addr+i*16(or 20 or indexed draw); if stride!=0, then the prefetch addr for the i-th indirect draw is para_base_Addr+i*stride. Per command.
       } Cmd_Dip_Idr_Idx_Imm_Ib_Dw10;

       typedef struct _Cmd_Dip_Idr_Idx_Idr_Ib_Dw1   // 7h DWF1: for indirect indexed multi-draw, with indirect IB
                                                    // information in memory.
       {
            unsigned int     Draw_Count            : 32; // Have this dw only when Indirect_DrawCnt_En=0. Per
                                                         // command.
       } Cmd_Dip_Idr_Idx_Idr_Ib_Dw1;
       typedef struct _Cmd_Dip_Idr_Idx_Idr_Ib_Dw2   // 7h DWF2:  for indirect indexed multi-draw, with indirect IB
                                                    // information in memory.
       {
            unsigned int     Drawcount_Addr_Low32  : 32; // Have this dw only when Indirect_DrawCnt_En=1, to give the
                                                         // addr to prefetch the drawCnt of
                                                         // vkCmdDrawIndirectCountAMD/vkCmdDrawIndexedIndirectCountAMD.
       } Cmd_Dip_Idr_Idx_Idr_Ib_Dw2;
       typedef struct _Cmd_Dip_Idr_Idx_Idr_Ib_Dw3   // 7h DWF3:  for indirect indexed multi-draw, with indirect IB
                                                    // information in memory.
       {
            unsigned int     Drawcount_Addr_High8  : 8;  // Have this dw only when Indirect_DrawCnt_En=1, to give the
                                                         // addr to prefetch the drawCnt of
                                                         // vkCmdDrawIndirectCountAMD/vkCmdDrawIndexedIndirectCountAMD.
            unsigned int     L2_Cachable           : 1;  // whether draw cnt goes into L2 cache, csp bypass to mxu
            unsigned int     Reserved              : 23;
       } Cmd_Dip_Idr_Idx_Idr_Ib_Dw3;
       typedef struct _Cmd_Dip_Idr_Idx_Idr_Ib_Dw4   // 7h DWF4:  for indirect indexed multi-draw, with indirect IB
                                                    // information in memory.
       {
            unsigned int     Para_Base_Addr_Low32  : 32; // Always have this dw for inidrect draw. Per command.
       } Cmd_Dip_Idr_Idx_Idr_Ib_Dw4;
       typedef struct _Cmd_Dip_Idr_Idx_Idr_Ib_Dw5   // 7h DWF5:  for indirect indexed multi-draw, with indirect IB
                                                    // information in memory.
       {
            unsigned int     Para_Base_Addr_High8  : 8;  // Always have this dw for inidrect draw. Per command.
            unsigned int     L2_Cachable           : 1;  // whether parameter data goes into L2 cache, csp bypass to
                                                         // mxu
            unsigned int     Reserved              : 23;
       } Cmd_Dip_Idr_Idx_Idr_Ib_Dw5;
       typedef struct _Cmd_Dip_Idr_Idx_Idr_Ib_Dw6   // 7h DWF6: for indirect indexed multi-draw, with indirect IB
                                                    // information in memory.
       {
            unsigned int     Para_Stride           : 32; // Always have this dw for inidrect draw. Dw to calculate
                                                         // parameter prefetch address for multiple indirect draw.
                                                         // If Para_stride==0, then the prefetch addr for the i-th
                                                         // indirect draw is para_base_addr+i*16(or 20 or indexed
                                                         // draw); if stride!=0, then the prefetch addr for the i-th
                                                         // indirect draw is para_base_Addr+i*stride. Per command.
       } Cmd_Dip_Idr_Idx_Idr_Ib_Dw6;
       typedef struct _Cmd_Dip_Idr_Idx_Idr_Ib_Dw7   // 7h DWF7: for indirect indexed multi-draw, with indirect IB
                                                    // information in memory.
       {
            unsigned int     Ib_Infor_Addr_Low32   : 32; // Only have this dw when "mode==index &&
                                                         // Indirect_IB_En==1". Dw to pre-fetch IB
                                                         // information(layout in memory should be
                                                         // IB_BaseAddr(2dw)+IB_ByteSize(1dw)+IB_Fmt(1dw), and
                                                         // IB_ByteOffset is default to 0. Per command.
       } Cmd_Dip_Idr_Idx_Idr_Ib_Dw7;
       typedef struct _Cmd_Dip_Idr_Idx_Idr_Ib_Dw8   // 7h DWF8: for indirect indexed multi-draw, with indirect IB
                                                    // information in memory.
       {
            unsigned int     Ib_Infor_Addr_High8   : 8;  // Only have this dw when "mode==index &&
                                                         // Indirect_IB_En==1". Dw to pre-fetch IB
                                                         // information(layout in memory should be
                                                         // IB_BaseAddr(2dw)+IB_ByteSize(1dw)+IB_Fmt(1dw), and
                                                         // IB_ByteOffset is default to 0.Per command.
            unsigned int     Reserved              : 24;
       } Cmd_Dip_Idr_Idx_Idr_Ib_Dw8;

       typedef struct _Cmd_Dip_Dword1               // 7h DWF1
       {
            unsigned int     Draw_Count            : 32; // For all dip cmd with Indirect_DrawCnt_En=0, always has
                                                         // this dw followed, to hint the draw cnt. And if not
                                                         // multiDraw, driver set it to 1. No this dw only if
                                                         // Indirect_DrawCnt_En=1. For drawAuto, this dw should
                                                         // always be 1.
       } Cmd_Dip_Dword1;
       typedef struct _Cmd_Dip_Dword2               // 7h DWF2
       {
            unsigned int     Drawcount_Addr_Low32  : 32; // For all dip cmd with Indirect_DrawCnt_En=1, always has
                                                         // this dw followed, to give the addr to prefetch the drawCnt
                                                         // of
                                                         // vkCmdDrawIndirectCountAMD/vkCmdDrawIndexedIndirectCountAMD.
                                                         // No this dw if Indirect_DrawCnt_En=0.
       } Cmd_Dip_Dword2;
       typedef struct _Cmd_Dip_Dword3               // 7h DWF3
       {
            unsigned int     Drawcount_Addr_High8  : 8;  // For all dip cmd with Indirect_DrawCnt_En=1, always has
                                                         // this dw followed, to give the addr to prefetch the drawCnt
                                                         // of
                                                         // vkCmdDrawIndirectCountAMD/vkCmdDrawIndexedIndirectCountAMD.
                                                         // No this dw if Indirect_DrawCnt_En=0.
            unsigned int     L2_Cachable           : 1;  // whether draw cnt goes into L2 cache, csp bypass to mxu
            unsigned int     Reserved              : 23;
       } Cmd_Dip_Dword3;
       typedef struct _Cmd_Dip_Dword4               // 7h DWF4
       {
            unsigned int     Start_Instance        : 32; // DWORD for start instance for all instanced non-indirect
                                                         // draw.There won't be such dword if
                                                         // "Start_Instance_Valid==0 || Indirect==1". For
                                                         // multiDirectDraw, this dw is for per command, all draw
                                                         // share the same Start_Instance.
       } Cmd_Dip_Dword4;
       typedef struct _Cmd_Dip_Dword5               // 7h DWF5
       {
            unsigned int     Instance_Count        : 32; // DWORD for instance count for all instanced non-indirect
                                                         // draw. There won't be such dword if "Instance_En==0 ||
                                                         // Indirect==1". For multiDirectDraw, this dw is for per
                                                         // command, all draw share the same Instance_Count.
       } Cmd_Dip_Dword5;
       typedef struct _Cmd_Dip_Dword6               // 7h DWF6
       {
            unsigned int     Index_Count           : 32; // DWORD for index count per instance of non-indirct
                                                         // draw.There won't be such dword if "Indirect==1". For
                                                         // multiDirectDraw, this dw is for per draw, and each draw has
                                                         // its Index_Count.
       } Cmd_Dip_Dword6;
       typedef struct _Cmd_Dip_Dword7               // 7h DWF7
       {
            unsigned int     Ib_Base_Addr_Low32    : 32; // DWORD for index buffer addr of all indexed draw.There
                                                         // won't be such dword if "mode==0||mode==4". For
                                                         // multiDirectIndexedDraw, this dw is for per draw, and each
                                                         // draw has its index buf addr; for
                                                         // multiIndirectIndexedDraw, this dw is for per cmd.
       } Cmd_Dip_Dword7;
       typedef struct _Cmd_Dip_Dword8               // 7h DWF8
       {
            unsigned int     Ib_Base_Addr_High8    : 8;  // DWORD for index buffer addr of indexed draw.There won't be
                                                         // such dword if "mode==0||mode==4". For
                                                         // multiDirectIndexedDraw, this dw is for per draw, and each
                                                         // draw has its index buf addr; for
                                                         // multiIndirectIndexedDraw, this dw is for per cmd.
            unsigned int     L2_Cachable           : 1;  // whether index buffer data goes into L2 cache, csp bypass to
                                                         // mxu
            unsigned int     Outofbound_Mode       : 1;  // For out of boundary index, use this bit to set the value. 0->
                                                         // set to 0; 1-> set to 0xffffffff.
            unsigned int     Reserved              : 22;
       } Cmd_Dip_Dword8;
       typedef struct _Cmd_Dip_Dword9               // 7h DWF9
       {
            unsigned int     Ib_Offset             : 32; // DWORD for index buffer byte offset of indexed draw.There
                                                         // won't be such dword if "mode==0||mode==4". For
                                                         // multiDirectIndexedDraw, this dw is for per draw, and each
                                                         // draw has its index buf offset; for
                                                         // multiIndirectIndexedDraw, this dw is for per cmd.
       } Cmd_Dip_Dword9;
       typedef struct _Cmd_Dip_Dword10              // 7h DWF10
       {
            unsigned int     Ib_Byte_Size          : 32; // DWORD for index buffer byte size of non-indirct indexed
                                                         // draw.There won't be such dword if "(mode==0||mode==4".
                                                         // For multiDirectIndexedDraw, this dw is for per draw, and
                                                         // each draw has its index buf size; for
                                                         // multiIndirectIndexedDraw, this dw is for per cmd.
       } Cmd_Dip_Dword10;
       typedef struct _Cmd_Dip_Dword11              // 7h DWF11
       {
            unsigned int     Start_Vertex          : 32; // DWORD for start vertex(or base vertex) for all
                                                         // non-indexed non-indirect draw. There won't be such dword
                                                         // if "Indirect==1 || Start_Vertex_Valid==0". For
                                                         // muitlDirectDraw, if Start_Vertex_Valid=1, then this dw
                                                         // is for per draw, each draw has its Start_Vertex.
       } Cmd_Dip_Dword11;
       typedef struct _Cmd_Dip_Dword12              // 7h DWF12
       {
            unsigned int     Start_Index           : 32; // DWORD for start index for all indexed non-indirect draw.
                                                         // There won't be such dword if "Indirect==1 ||
                                                         // Start_Index_Valid==0". For muitlDirectDraw, if
                                                         // Start_Index_Valid=1, then this dw is for per draw, each
                                                         // draw has its Start_Index.
       } Cmd_Dip_Dword12;
       typedef struct _Cmd_Dip_Dword13              // 7h DWF13
       {
            unsigned int     Para_Base_Addr_Low32  : 32; // DWORD for parameter base addr for indirect draw. There
                                                         // won't be such dword if "Indirect==0". For
                                                         // multiIndirectDraw, this dw is for per cmd.
       } Cmd_Dip_Dword13;
       typedef struct _Cmd_Dip_Dword14              // 7h DWF14
       {
            unsigned int     Para_Base_Addr_High8  : 8;  // DWORD for parameter base addr for indirect draw. There
                                                         // won't be such dword if "Indirect==0". For
                                                         // multiIndirectDraw, this dw is for per cmd.
            unsigned int     L2_Cachable           : 1;  // whether parameter data goes into L2 cache, csp bypass to
                                                         // mxu
            unsigned int     Reserved              : 23;
       } Cmd_Dip_Dword14;
       typedef struct _Cmd_Dip_Dword15              // 7h DWF15
       {
            unsigned int     Para_Stride           : 32; // DWORD to calculate parameter prefetch address for
                                                         // multiple indirect draw. If Para_stride==0, then the
                                                         // prefetch addr for the i-th indirect draw is
                                                         // para_base_addr+i*16(or 20 or indexed draw); if
                                                         // stride!=0, then the prefetch addr for the i-th indirect
                                                         // draw is para_base_Addr+i*stride. There won't be such
                                                         // dword if "Indirect==0"(for indirect draw with
                                                         // drawCnt=0, driver can set Para_Stride to 0). For
                                                         // multiIndirectDraw, this dw is for per cmd.
       } Cmd_Dip_Dword15;
       typedef struct _Cmd_Dip_Insdrawauto_Dword1   // 7h DWF1
       {
            unsigned int     Instance_Count        : 32; // DWORD for instance count, for instanced drawauto for OGL
                                                         // DrawTransformFeedbackInstanced and
                                                         // DrawTransformFeedbackStreamInstanced. There is this
                                                         // dw only if the "Mode==4 && Instance_En==1". For "Mode==4
                                                         // && Instance_En==0" drawAuto cmd, no dw followed.
       } Cmd_Dip_Insdrawauto_Dword1;
// some other description about the command usage: End
/////////////////////////////////////////////////////////////////////////


typedef struct _Cmd_Fence                   // 8h: Fence Command:Internal fence is used to allow certain block to
                                            // update status into internal fence counter at a given slot. CSP will
                                            // maintain 4 fence slots. Each slot has corresponding 8-bit register
                                            // (counter); external fence is used to sync with CPU. The register value
                                            // will be updated according to the update mode.
{
    unsigned int     Dwc                   : 3;  //for internal fence(Fence_Type=0), should be 1; for 32b external fence(Fence_Type=2), should be 3;
                                                 //for 64b external fence(Fence_Type=3), should be 4. And when just use this cmd to blt IMM data(Fence_Type=1), this filed should be 2.

    unsigned int     Irq                   : 2;  // Interrupt
    unsigned int     Fence_Type            : 2;
    unsigned int     Slot_Id               : 5;  // totally 32 fence counter here. When 4 CS RB run
                                                 // simultaneously, driver can use the same
                                                 // route_id==UAV_CS_Fence, but 4 different slot id for each
                                                 // RB.
    unsigned int     Reserved              : 1;  // reserved
    unsigned int     Fence_Update_Mode     : 1;  // Fence update mode
    unsigned int     Imm_Dw_Num            : 8;  // IMM dw num. When this is an external fence, driver should set this field into 0.
                                                 // When driver use this cmd to blt IMM data into memory, then DWC=2(address), and IMM_Dw_Num is the number of IMM dw that driver ask csp to blt.
    unsigned int     Rb_Type               : 2;  // 3d,csl,csh, all module will drain task according to
                                                 // process_id & rb type
    unsigned int     Route_Id              : 4;  // Down side blocks parsing/bypass fence cmd according to
                                                 // route_id. And in MXU, write the fence counter according to
                                                 // slot_id, not route_id in E3K.
    unsigned int     Major_Opcode          : 4;  // 1000: Internal/External Fence (wait for internal block
                                                 // completion before next command can be executed)
} Cmd_Fence;
/////////////////////////////////////////////////////////////////////////
// some other description about the command usage: Start
       typedef struct _Cmd_Fence_Internal_Dword1    // 8h DWF1: DWORD for internal fence, fence update value
       {
            unsigned int     Update_Value          : 16;
            unsigned int     Slice_Mask            : 16; // This bit will not be touched by driver. CSP will set it
                                                         // according to current ring buffer status indicator. And
                                                         // MXU will according to this bit to decide which slice's
                                                         // fence need to be wait.
       } Cmd_Fence_Internal_Dword1;
       typedef struct _Cmd_Fence_External_Addr_Dword1
                                                    // 8h DWF1: DWORD for external fence write address, dw aligned for 32b
                                                    // external fence, and ddw aligned for 64b external fence cmd.
       {
            unsigned int     External_Addr_Low32   : 32;
       } Cmd_Fence_External_Addr_Dword1;
       typedef struct _Cmd_Fence_External_Addr_Dword2
                                                    // 8h DWF2: DWORD for external fence write address, dw aligned for 32b
                                                    // external fence, and ddw aligned for 64b external fence cmd.
       {
            unsigned int     External_Addr_High8   : 8;
            unsigned int     Reserved              : 23;
            unsigned int     Fence_Update_Timing   : 1;  // When this bit is 1, MXU will send all write requests out then
                                                         // update the fence value.
       } Cmd_Fence_External_Addr_Dword2;
       typedef struct _Cmd_Fence_External_Data_Dword3
                                                    // 8h DWF3: DWORD for external fence write data. For 32b external fence,
                                                    // this value is enough; for 64b external fence, it��s the low 32b data.
       {
            unsigned int     External_Data1        : 32;
       } Cmd_Fence_External_Data_Dword3;
       typedef struct _Cmd_Fence_External_Data_Dword4
                                                    // 8h DWF4: DWORD for external fence write data, high 32b data for 64b
                                                    // external fence
       {
            unsigned int     External_Data2        : 32;
       } Cmd_Fence_External_Data_Dword4;
// some other description about the command usage: End
/////////////////////////////////////////////////////////////////////////


typedef struct _Cmd_Block_Command_Template  // 9h: BlockCommand Template: BlockCommand targets at some blocks for
                                            // them to take action. CSP will not parse details about it, but
                                            // corresponding block will.
{
    unsigned int     Dwc                   : 12; // Number of dw followed. 0 to 2^12-1 DW may be followed. CSP
                                                 // will not send dwc, but end of command flag to EU. The
                                                 // command_specific_filed and actual dwc count is per
                                                 // command based.
    unsigned int     Type                  : 2;  // Type
    unsigned int     Command_Specific_Field
                                           : 7;  // Defined at block level.
    unsigned int     Reserved0             : 2;  // Reserved
    unsigned int     Block_Id              : 5;  // Block ID, refer to "TYPE" comments, each module bypass
                                                 // command with other blockid
    unsigned int     Major_Opcode          : 4;  // 1001: BlockCommand(With multiple dw followed, for block
                                                 // level parsing, such as 2D draw)
} Cmd_Block_Command_Template;

typedef struct _Cmd_Block_Command_Tas       // 9h: BlockCommand starting at TAS
{
    unsigned int     Dwc                   : 12; // Number of dw followed
    unsigned int     Type                  : 2;  // Type
    unsigned int     Command_Specific_Field
                                           : 6;  // Command for TAS
    unsigned int     Reserved0             : 3;
    unsigned int     Block_Id              : 5;  // Block ID, set to TASFE or TASBE
    unsigned int     Major_Opcode          : 4;  // 1001: BlockCommand(With multiple dw followed, for block
                                                 // level parsing, such as 2D draw)
} Cmd_Block_Command_Tas;

typedef struct _Cmd_Block_Command_Sg        // 9h: BlockCommand starting at SG
{
    unsigned int     Dwc                   : 12; // Number of dw followed
    unsigned int     Type                  : 2;  // Type
    unsigned int     Area_Target           : 2;  // Area Target
    unsigned int     Action_Type           : 3;  // Action Type
    unsigned int     Tbr_Loop_Status       : 1;  // if this 2d command is sent to normal 2d in GPC, and it's per
                                                 // frame kickoff mode, then all registers and commands are
                                                 // accumulated in TASBE, in rendering stage, we need to know
                                                 // whether this 2d comamnd need tobe excecute once_for_all
                                                 // or once_per_TBRtile
    unsigned int     Reserved0             : 1;  // Reserved
    unsigned int     Blt_Overlap           : 1;  // whether bit blit cmd's srouce and destination are
                                                 // overlapped.
    unsigned int     Predicate_En          : 1;  // For predicate rendering of fast clear cmd and 2dblt cmd.
    unsigned int     Block_Id              : 5;  // Block ID, set to Setup/SG. SG block will do a corresponding
                                                 // action, i.e. doing BB-only Span Gen. SG will forward this
                                                 // command to other blocks further. if this 2d command is
                                                 // going to MCE, just set block id = MCE, other wise set as FF,
                                                 // which means csp will send this command into normal 2d pipe
                                                 // in GPC.
    unsigned int     Major_Opcode          : 4;  // 1001: BlockCommand(With multiple dw followed, for block
                                                 // level parsing, such as 2D draw)
} Cmd_Block_Command_Sg;
/////////////////////////////////////////////////////////////////////////
// some other description about the command usage: Start
       typedef struct _Cmd_Block_Command_Sg_Dword0  // 9h DWF1: DWORD for BlockCommand SG. ClearArea X extents for FastClear
                                                    // cmd, Blt Src X extents for BitBlt cmd, defined in below.
       {
            unsigned int     Xmin                  : 16; // X min.
            unsigned int     Xmax                  : 16; // X max.
       } Cmd_Block_Command_Sg_Dword0;
       typedef struct _Cmd_Block_Command_Sg_Rot_Dword0
                                                    // 9h DWF1:DWORD for BlockCommandSG.For Rotation
       {
            unsigned int     Cw                    : 1;
            unsigned int     Reserved0             : 31; // Reserved
       } Cmd_Block_Command_Sg_Rot_Dword0;
       typedef struct _Cmd_Block_Command_Sg_Dword1  // 9h DWF2: DWORD for BlockCommand SG. ClearArea Y extents for FastClear
                                                    // cmd, Blt Src Y extents for BitBlt cmd, defined in below.
       {
            unsigned int     Ymin                  : 16; // Y min.
            unsigned int     Ymax                  : 16; // Y max.
       } Cmd_Block_Command_Sg_Dword1;
       typedef struct _Cmd_Block_Command_Sg_Blt_Dword2
                                                    // 9h DWF3:DWORD for BlockCommandSG.For BitBlt
       {
            unsigned int     Dx                    : 15; // dX from src
            unsigned int     Reserved0             : 1;  // Reserved0
            unsigned int     Dy                    : 15; // dY from src
            unsigned int     Reserved1             : 1;  // Reserved1
       } Cmd_Block_Command_Sg_Blt_Dword2;
       typedef struct _Cmd_Block_Command_Sg_Gradient_Fill_Dword
                                                    // 9h DWF3-14:DWORD for BlockCommandSG.For GradientFill
       {
            unsigned int     Color                 : 28; // 15.13, for GradientFill
            unsigned int     Reserved              : 4;  // Reserved
       } Cmd_Block_Command_Sg_Gradient_Fill_Dword;
// some other description about the command usage: End
/////////////////////////////////////////////////////////////////////////


typedef struct _Cmd_Block_Command_Img_Trn   // 9h: BlockCommand for Image Transfer, starting at SG
{
    unsigned int     Dwc                   : 12; // Number of dw followed
    unsigned int     Type                  : 2;  // Type
    unsigned int     Is_Dword_Aligned      : 1;  // Whether it is dword aligned or byte aligned.
                                                 //  In practice, only in 1/4/8/16 bpp case this bit is useful.
                                                 //  To driver, set this bit if the data will be dword aligned due
                                                 // to pitch size.
                                                 //  If this bit is not set, hw will always treat the data as byte
                                                 // aligned when x reaches end.
    unsigned int     Data_Fmt              : 3;  // the image data format
    unsigned int     Tbr_Loop_Status       : 1;  // if this 2d command is sent to normal 2d in GPC, then it will be
                                                 // accumulate in TASBE, in rendering stage, we need to know
                                                 // whether this 2d comamnd need tobe excecute once for all or
                                                 // once per TBRtile
    unsigned int     Reserved              : 3;  // reserved
    unsigned int     Predicate_En          : 1;  // For predicate rendering of imagetransfer cmd.
    unsigned int     Block_Id              : 5;  // Block ID, if this 2d command is going to MCE, just set block
                                                 // id = MCE, other wise set as FF, which means csp will send this
                                                 // command into normal 2d pipe in GPC.
    unsigned int     Major_Opcode          : 4;  // 1001: BlockCommand(With multiple dw followed, for block
                                                 // level parsing, such as 2D draw)
} Cmd_Block_Command_Img_Trn;
/////////////////////////////////////////////////////////////////////////
// some other description about the command usage: Start
       typedef struct _Cmd_Block_Command_Img_Trn_Dword0
                                                    // 9h DWF1: DWORD for BlockCommand Image Transfer
       {
            unsigned int     Xmin                  : 16; // X min.
            unsigned int     Xmax                  : 16; // Y min.
       } Cmd_Block_Command_Img_Trn_Dword0;
       typedef struct _Cmd_Block_Command_Img_Trn_Dword1
                                                    // 9h DWF2: DWORD for BlockCommand Image Transfer
       {
            unsigned int     Ymin                  : 16;
            unsigned int     Ymax                  : 16;
       } Cmd_Block_Command_Img_Trn_Dword1;
// some other description about the command usage: End
/////////////////////////////////////////////////////////////////////////


typedef struct _Cmd_Block_Command_Flush     // 9h: BlockCommand for FFC Flush
{
    unsigned int     Dwc                   : 12; // Number of dw followed, should be 0 for ffc
                                                 // flush/invalidate cmd
    unsigned int     Type                  : 2;  // Type. The same as Block_Command_IMG_TRN to specify
                                                 // "Flush or Invalidate"
    unsigned int     Target                : 5;  // Target. depend on cache implementation. for coarse-gain
                                                 // cache management, we may set to all.
    unsigned int     Uav_Type              : 2;  // UAV surface and descriptor request type, cache need check
                                                 // both "Target & UAV_TYPE" to decide which part of uav data
                                                 // cache or descriptor cache need tobe
                                                 // flushed/invalidated.
    unsigned int     Reserved              : 2;  // Reserved
    unsigned int     Block_Id              : 5;  // Block ID: when target is 4&5, block id is WLS_FE or
                                                 // WLS_BE(refer to comments in "uav_type"), other wise,
                                                 // block id is FF
    unsigned int     Major_Opcode          : 4;  // 1001: BlockCommand(With multiple dw followed, for block
                                                 // level parsing, such as 2D draw)
} Cmd_Block_Command_Flush;

typedef struct _Cmd_Block_Command_Eu        // 9h: BlockCommand for EU
{
    unsigned int     Dwc                   : 12; // Number of dw followed
    unsigned int     Type                  : 2;  // Type
    unsigned int     Command_Specific_Field
                                           : 7;  // Defined at block level.
                                                 //  if (type==invalidate_L1I)  // it is zone type actually.
                                                 //  {
                                                 //      cmd_spec_field==0, zone_type==3DFE
                                                 //      cmd_spec_field==1, zone_type==3DBE
                                                 //      cmd_spec_field==2, zone_type==CSL
                                                 //      cmd_spec_field==3, zone_type==CSH
                                                 //  }
    unsigned int     Reserved              : 2;  // Reserved
    unsigned int     Block_Id              : 5;  // Block ID set to EU_FS or EU_PS or EU_CS
    unsigned int     Major_Opcode          : 4;  // 1001: BlockCommand(With multiple dw followed, for block
                                                 // level parsing)
} Cmd_Block_Command_Eu;

typedef struct _Cmd_Block_Command_Tu        // 9h: BlockCommand for TU
{
    unsigned int     Dwc : 12; // Number of dw followed
    unsigned int     Type : 2;  // Type
    unsigned int     Command_Specific_Field
        : 7;  // Defined at block level.
              //  if (type==invalidate_L1)  // it is zone type actually.
              //  {
              //      cmd_spec_field==0, zone_type==3DFE
              //      cmd_spec_field==1, zone_type==3DBE
              //      cmd_spec_field==2, zone_type==CSL
              //      cmd_spec_field==3, zone_type==CSH
              //  }
    unsigned int     Reserved : 2;  // Reserved
    unsigned int     Block_Id : 5;  // it could be TUFE or TUBE
    unsigned int     Major_Opcode : 4;  // 1001: BlockCommand(With multiple dw followed, for block
                                        // level parsing)
} Cmd_Block_Command_Tu;

typedef struct _Cmd_Block_Command_Tu_CHX004        // 9h: BlockCommand for TU
{
    unsigned int     Dwc                   : 12; // Number of dw followed
    unsigned int     Type                  : 2;  // Type
    unsigned int     Command_Specific_Field
                                           : 2;  // Defined at block level.
                                                 //  if (type==invalidate_L1)  // it is zone type actually.
                                                 //  {
                                                 //      cmd_spec_field==0, zone_type==3DFE
                                                 //      cmd_spec_field==1, zone_type==3DBE
                                                 //      cmd_spec_field==2, zone_type==CSL
                                                 //      cmd_spec_field==3, zone_type==CSH
                                                 //  }
    unsigned int     Pre_Fetch_End         : 1;  //when pre-fetch end, set it to 1
    unsigned int     Heap_Idx              : 4;  //heap index: TU will load heap base in which heap
    unsigned int     Reserved              : 2;  // Reserved
    unsigned int     Block_Id              : 5;  // it could be TUFE or TUBE
    unsigned int     Major_Opcode          : 4;  // 1001: BlockCommand(With multiple dw followed, for block
                                                 // level parsing)
} Cmd_Block_Command_Tu_CHX004;
    typedef struct _Cmd_Block_Command_Tu_Dword0_CHX004
    {
        unsigned int Slot_Ofst : 32; //when TYPE==PRE_FETCH, offset align to 256bit in heap
    } Cmd_Block_Command_Tu_Dword0_CHX004;
    typedef struct _Cmd_Block_Command_Tu_Dword1_CHX004
    {
        unsigned int Slot_Cnt : 8; //when TYPE==PRE_FETCH, cnt align to 256bit in heap
        unsigned int Reserved : 24; //
    } Cmd_Block_Command_Tu_Dword1_CHX004;

typedef struct _Cmd_Block_Command_L2        // 9h: BlockCommand for L2
{
    unsigned int     Dwc                   : 12; // Number of dw followed, should be 0 for flush command, and 4
                                                 // for invaldation command.
    unsigned int     Type                  : 3;  // Type
    unsigned int     Usage                 : 3;
    unsigned int     Reserved              : 5;  // Reserved
    unsigned int     Block_Id              : 5;  // should be set to L2
    unsigned int     Major_Opcode          : 4;  // 1001: BlockCommand(With multiple dw followed, for block
                                                 // level parsing)
} Cmd_Block_Command_L2;
/////////////////////////////////////////////////////////////////////////
// some other description about the command usage: Start
       typedef struct _Cmd_Block_Command_L2_Dword0  // 9h DWF1: DWORD for BlockCommand L2
       {
            unsigned int     Start_Address_Low32   : 32; // Here is the low 32 bits for the addr.
       } Cmd_Block_Command_L2_Dword0;
       typedef struct _Cmd_Block_Command_L2_Dword1  // 9h DWF2: DWORD for BlockCommand L2
       {
            unsigned int     Start_Address_High8   : 8;  // The address is 256bits aligned. Here is the high 8 bits for
                                                         // the addr.
            unsigned int     Reserved              : 24;
       } Cmd_Block_Command_L2_Dword1;
       typedef struct _Cmd_Block_Command_L2_Dword2  // 9h DWF3: DWORD for BlockCommand L2
       {
            unsigned int     Address_Byte_Mask_Low32
                                                   : 32;
       } Cmd_Block_Command_L2_Dword2;
       typedef struct _Cmd_Block_Command_L2_Dword3  // 9h DWF4: DWORD for BlockCommand L2
       {
            unsigned int     Address_Byte_Mask_High8
                                                   : 8;  // The mask used to invalidate/flush memory based on start
                                                         // address. If the MASK is set to 0, this measn the full address
                                                         // space.
            unsigned int     Reserved              : 24;
       } Cmd_Block_Command_L2_Dword3;
// some other description about the command usage: End
/////////////////////////////////////////////////////////////////////////


typedef struct _Cmd_Block_Command_Mxu       // 9h: BlockCommand for MXU & L2.
{
    unsigned int     Dwc                   : 12; // Number of dw followed, always set to 4 for all types. For
                                                 // MXU's block cmd with type=0/1, use
                                                 // "BLOCK_COMMAND_MXU_BLC_DWORD0"~"BLOCK_COMMAND_MXU_BLC_DWORD3";
                                                 // for MXU's block cmd with type=2/3, use
                                                 // "BLOCK_COMMAND_MXU_PTC_DWORD0"
                                                 // ~"BLOCK_COMMAND_MXU_PTC_DWORD3"; for L2's block cmd,
                                                 // use
                                                 // "BLOCK_COMMAND_MXU_L2_DWORD0"~"BLOCK_COMMAND_MXU_L2_DWORD3"
    unsigned int     Cmd_Type              : 2;  // block cmd type for mxu
    unsigned int     Reserved              : 4;
    unsigned int     Proc_Id               : 4;
    unsigned int     Reserved1             : 1;  // delete the bypass_to_l2 bit, since for L2 block cmd,
                                                 // should use "BLOCK_COMMAND_L2"
    unsigned int     Block_Id              : 5;  // Block ID set to MXU
    unsigned int     Major_Opcode          : 4;  // 1001: BlockCommand(With multiple dw followed, for block
                                                 // level parsing)
} Cmd_Block_Command_Mxu;
/////////////////////////////////////////////////////////////////////////
// some other description about the command usage: Start
       typedef struct _Cmd_Block_Command_Mxu_Blc_Dword0
                                                    // 9h DWF1: DWORD for BlockCommand MXU when flush/invalidate BLC
       {
            unsigned int     Start_Address_Low32   : 32; // Here is the low 32 bits for the addr.
       } Cmd_Block_Command_Mxu_Blc_Dword0;
       typedef struct _Cmd_Block_Command_Mxu_Blc_Dword1
                                                    // 9h DWF2: DWORD for BlockCommand MXU when flush/invalidate BLC
       {
            unsigned int     Start_Address_High8   : 8;  // The address is 256bits aligned. Here is the high 8 bits for
                                                         // the addr.
            unsigned int     Usage                 : 3;
            unsigned int     Reserved              : 21;
       } Cmd_Block_Command_Mxu_Blc_Dword1;
       typedef struct _Cmd_Block_Command_Mxu_Blc_Dword2
                                                    // 9h DWF3: DWORD for BlockCommand MXU when flush/invalidate BLC
       {
            unsigned int     Address_Byte_Mask_Low32
                                                   : 32;
       } Cmd_Block_Command_Mxu_Blc_Dword2;
       typedef struct _Cmd_Block_Command_Mxu_Blc_Dword3
                                                    // 9h DWF4: DWORD for BlockCommand MXU when flush/invalidate BLC
       {
            unsigned int     Address_Byte_Mask_High8
                                                   : 8;  // The mask used to invalidate/flush memory based on start
                                                         // address.
            unsigned int     Reserved              : 24;
       } Cmd_Block_Command_Mxu_Blc_Dword3;
       typedef struct _Cmd_Block_Command_Mxu_Ptc_Dword0
                                                    // 9h DWF1: DWORD for BlockCommand MXU when invalidate PTC
       {
            unsigned int     Reserved              : 12;
            unsigned int     Start_Address_L       : 20; // For the L32 bit addr, the 8b LSB are default to 0.
       } Cmd_Block_Command_Mxu_Ptc_Dword0;
       typedef struct _Cmd_Block_Command_Mxu_Ptc_Dword1
                                                    // 9h DWF2: DWORD for BlockCommand MXU when invalidate PTC
       {
            unsigned int     Start_Address_High8   : 8;  // The address is 256bits aligned. Here is the high 8 bits for
                                                         // the addr.
            unsigned int     Reserved              : 24;
       } Cmd_Block_Command_Mxu_Ptc_Dword1;
       typedef struct _Cmd_Block_Command_Mxu_Ptc_Dword2
                                                    // 9h DWF3: DWORD for BlockCommand MXU when invalidate PTC
       {
            unsigned int     Reserved              : 12;
            unsigned int     Address_Byte_Mask_L   : 20; // For the L32 bit addr, the 8b LSB are default to 0.
       } Cmd_Block_Command_Mxu_Ptc_Dword2;
       typedef struct _Cmd_Block_Command_Mxu_Ptc_Dword3
                                                    // 9h DWF4: DWORD for BlockCommand MXU when invalidate PTC
       {
            unsigned int     Address_Byte_Mask_High8
                                                   : 8;  // The mask used to invalidate/flush memory based on start
                                                         // address.
            unsigned int     Reserved              : 24;
       } Cmd_Block_Command_Mxu_Ptc_Dword3;
       typedef struct _Cmd_Block_Command_Mxu_L2_Dword0
                                                    // 9h DWF1: DWORD for BlockCommand MXU
       {
            unsigned int     Address_Low32         : 32; // Here is the low 32 bits for the addr.
       } Cmd_Block_Command_Mxu_L2_Dword0;
       typedef struct _Cmd_Block_Command_Mxu_L2_Dword1
                                                    // 9h DWF2: DWORD for BlockCommand MXU
       {
            unsigned int     Address_High8         : 8;  // The address is 2048bits aligned. Here is the high 8 bits for
                                                         // the 40bit byte addr.
            unsigned int     Reserved              : 24;
       } Cmd_Block_Command_Mxu_L2_Dword1;
       typedef struct _Cmd_Block_Command_Mxu_L2_Dword2
                                                    // 9h DWF3: DWORD for BlockCommand MXU
       {
            unsigned int     Mask_Low32            : 32;
       } Cmd_Block_Command_Mxu_L2_Dword2;
       typedef struct _Cmd_Block_Command_Mxu_L2_Dword3
                                                    // 9h DWF4: DWORD for BlockCommand MXU
       {
            unsigned int     Mask_High8            : 8;  // The mask used to invalidate L2 cache. Driver may
                                                         // invalidate more to avoid send multiple commands in case
                                                         // the address/size is not power of 2.
            unsigned int     Reserved              : 24;
       } Cmd_Block_Command_Mxu_L2_Dword3;
// some other description about the command usage: End
/////////////////////////////////////////////////////////////////////////


typedef struct _Cmd_Block_Command_Csp       // 9h: BlockCommand for CSP
{
    unsigned int     Dwc                   : 3;  // Number of dw followed. it should be 4 for MXU BLC clear cmd, 2
                                                 // for save/restore cmd, 0 for indicator/trigger.
    unsigned int     Header                : 17; // header
    unsigned int     Type                  : 3;  // Type of current cmd
    unsigned int     Block_Id              : 5;  // Block ID set to CSP
    unsigned int     Major_Opcode          : 4;  // 1001: BlockCommand(With multiple dw followed, for block
                                                 // level parsing)
} Cmd_Block_Command_Csp;

typedef struct _Cmd_Blk_Cmd_Csp_Blc         // 9h: BlockCommand for CSP
{
    unsigned int     Dwc                   : 3;  // Number of dw followed, should be 3 for clear BLC cmd.
    unsigned int     Counter               : 17; // for clear MXU's BLC cmd, counter tells  how many bytes need
                                                 // to be cleared,counter== 16 is equivalent to  32bit full
                                                 // mask , if counter >16, the data will be duplicated from lsb
                                                 // of 1st 128bit.
    unsigned int     Type                  : 3;  // Type of current cmd
    unsigned int     Block_Id              : 5;  // Block ID set to CSP
    unsigned int     Major_Opcode          : 4;  // 1001: BlockCommand(With multiple dw followed, for block
                                                 // level parsing)
} Cmd_Blk_Cmd_Csp_Blc;
/////////////////////////////////////////////////////////////////////////
// some other description about the command usage: Start
       typedef struct _Cmd_Blk_Cmd_Csp_Blc_Dword1   // 9h DWF1: DWORD for BlockCommand CSP
       {
            unsigned int     Start_Address_Low32   : 32; // Here is the low 32 bits for the 40 bits byte addr.
       } Cmd_Blk_Cmd_Csp_Blc_Dword1;
       typedef struct _Cmd_Blk_Cmd_Csp_Blc_Dword2   // 9h DWF2: DWORD for BlockCommand CSP
       {
            unsigned int     Start_Address_High8   : 8;  // The address is byte aligned. Here is the high 8 bits for the
                                                         // 40 bits byte addr
            unsigned int     Reserved              : 16;
            unsigned int     Clear_Value1          : 4;  // The clear value means the 4 bit that flag buffer will be
                                                         // cleared. HW will duplicate it to get full counter*8 bits to
                                                         // send to MXU.
            unsigned int     Clear_Value0          : 4;  // The clear value means the 4 bit that flag buffer will be
                                                         // cleared. HW will duplicate it to get full counter*8 bits to
                                                         // send to MXU.
       } Cmd_Blk_Cmd_Csp_Blc_Dword2;
       typedef struct _Cmd_Blk_Cmd_Csp_Blc_Dword3
       {
            unsigned int     Blc_Mask              : 32; // for each bit, 1 means use "CLEAR_VALUE1"; 0 means use
                                                         // "CLEAR_VALUE0",
       } Cmd_Blk_Cmd_Csp_Blc_Dword3;
// some other description about the command usage: End
/////////////////////////////////////////////////////////////////////////


typedef struct _Cmd_Blk_Cmd_Csp_Save_Rsto   // 9h: BlockCommand for CSP
{
    unsigned int     Dwc                   : 3;  // Number of dw followed. it should be 2 for save/restore cmd,
                                                 // no matter the addr/offs mode.
    unsigned int     Reserved              : 16;
    unsigned int     Address_Mode          : 1;  // address mode
    unsigned int     Type                  : 3;  // Type of current cmd
    unsigned int     Block_Id              : 5;  // Block ID set to CSP
    unsigned int     Major_Opcode          : 4;  // 1001: BlockCommand(With multiple dw followed, for block
                                                 // level parsing)
} Cmd_Blk_Cmd_Csp_Save_Rsto;
/////////////////////////////////////////////////////////////////////////
// some other description about the command usage: Start
       typedef struct _Cmd_Blk_Cmd_Csp_Save_Rsto_Addr_Dword1
                                                    // 9h DWF1: DWORD for BlockCommand CSP. When Address_mode=0, this is the
                                                    // dw followed.
       {
            unsigned int     Start_Address_Low32   : 32; // Here is the low 32 bits for the 40 bits byte addr.
       } Cmd_Blk_Cmd_Csp_Save_Rsto_Addr_Dword1;
       typedef struct _Cmd_Blk_Cmd_Csp_Save_Rsto_Addr_Dword2
                                                    // 9h DWF2: DWORD for BlockCommand CSP. When Address_mode=0, this is the
                                                    // dw followed.
       {
            unsigned int     Start_Address_High8   : 8;  // Here is the high 8 bits for the 40 bits byte addr
            unsigned int     L2_Cachable           : 1;  // whether the restore reg data goes into L2 cache, csp bypass
                                                         // to mxu
            unsigned int     Reserved              : 23;
       } Cmd_Blk_Cmd_Csp_Save_Rsto_Addr_Dword2;
       typedef struct _Cmd_Blk_Cmd_Csp_Save_Rsto_Offset_Dword1
                                                    // 9h DWF1: DWORD for BlockCommand CSP. When Address_mode=1, this is the
                                                    // dw followed.
       {
            unsigned int     Offset_Low32          : 32;
       } Cmd_Blk_Cmd_Csp_Save_Rsto_Offset_Dword1;
       typedef struct _Cmd_Blk_Cmd_Csp_Save_Rsto_Offset_Dword2
                                                    // 9h DWF2: DWORD for BlockCommand CSP. When Address_mode=1, this is the
                                                    // dw followed.
       {
            unsigned int     Offset_High8          : 8;
            unsigned int     Reserved              : 24;
       } Cmd_Blk_Cmd_Csp_Save_Rsto_Offset_Dword2;
// some other description about the command usage: End
/////////////////////////////////////////////////////////////////////////


typedef struct _Cmd_Blk_Cmd_Csp_Indicator   // 9h:  Pwm adj Indicator command
{
    unsigned int     Dwc                   : 3;  // Number of dw followed, should be 1 for trigger cmd. And if
                                                 // mode=off, then the dw followed should be 0.
    unsigned int     Reserved              : 10; // Reserved
    unsigned int     Process_Id            : 4;
    unsigned int     Info                  : 3;  // info, used in indicator type
    unsigned int     Type                  : 3;
    unsigned int     Block_Id              : 5;  // Block ID set to CSP
    unsigned int     Major_Opcode          : 4;  // 1001: BlockCommand(With multiple dw followed, for block
                                                 // level parsing)
} Cmd_Blk_Cmd_Csp_Indicator;
/////////////////////////////////////////////////////////////////////////
// some other description about the command usage: Start
       typedef struct _Cmd_Blk_Cmd_Csp_Indicator_Dword1
                                                    // 9h:  Pwm adj Indicator command dw1
       {
            unsigned int     Slice_Mask            : 12; // slice mask for totally 12 slices in the 3 GPCs
            unsigned int     Reserved              : 20; // Reserved
       } Cmd_Blk_Cmd_Csp_Indicator_Dword1;
// some other description about the command usage: End
/////////////////////////////////////////////////////////////////////////


typedef struct _Cmd_Blk_Cmd_Csp_Ld_Des      // 9h: Load descriptor cmd for CSP
{
    unsigned int     Dwc                   : 3;  // Number of dw followed. Should be 7 for C#, and 5 for T/S/U#.
    unsigned int     Src_Desc_Stride       : 5;  // If Src_Desc_Stride_En==1, then csp should pre-fetch
                                                 // Src_Desc_Stride*Desc_Num dw, and then pick the valid
                                                 // descriptor to update shadow buf. "Src_Desc_Stride" is
                                                 // the dw size for a conbined descriptor.
    unsigned int     Src_Desc_Stride_En    : 1;  // Whether the stride for descriptor in memory is enable.
                                                 // This is for DX12's T+S conbined case. For this case, sw may
                                                 // only want to update the T/S in this T+S conbined range.
    unsigned int     Pack_En               : 1;  // For T#/S#/T+S conbined, dwc is not aligned to 8 for each
                                                 // descriptor. In OGL/vulkan, they may be packed tightly.
                                                 // But in DX12, each descriptor is 8 dw, and csp should pick the
                                                 // valid dw and set into each block.
    unsigned int     Heap_Idx              : 3;  // when "Address_Mode=1", then the base address are set into
                                                 // heap_reg[Heap_Idx].
    unsigned int     Address_Mode_Followed : 1;  // The mode for the 2 address dw followed
    unsigned int     Reserved              : 2;  // Reg offset to fill the descriptor dw
    unsigned int     Desc_Buffer_En        : 1;  // Only valid when Desc_Type is 0~3. When 1, then the
                                                 // descriptor is buffer type, bci should update the
                                                 // base_addr and range info for the descriptor after
                                                 // prefetch.
    unsigned int     Desc_Type             : 3;  // type for the loading descriptor
    unsigned int     Type                  : 3;  // Type of current cmd
    unsigned int     Block_Id              : 5;  // Block ID set to CSP
    unsigned int     Major_Opcode          : 4;  // 1001: BlockCommand(With multiple dw followed, for block
                                                 // level parsing)
} Cmd_Blk_Cmd_Csp_Ld_Des;

/////////////////////////////////////////////////////////////////////////
// some other description about the command usage: Start
       typedef struct _Cmd_Blk_Cmd_Csp_Ld_Des_T_Dword1
                                                    // 9h DWF1: DWORD for BlockCommand BCI load descriptor cmd when load
                                                    // T#/S#/U#/T&S#
       {
            unsigned int     Desc_Base_Addr_Low32  : 32; // base address for T#/S#/U# group, byte address.
       } Cmd_Blk_Cmd_Csp_Ld_Des_T_Dword1;
       typedef struct _Cmd_Blk_Cmd_Csp_Ld_Des_T_Dword2
                                                    // 9h DWF2: DWORD for BlockCommand BCI load descriptor cmd when load
                                                    // T#/S#/U#/T&S#
       {
            unsigned int     Desc_Base_Addr_High8  : 8;
            unsigned int     L2_Cachable           : 1;  // whether the descriptor data goes into L2 cache, csp bypass
                                                         // to mxu
            unsigned int     Reserved              : 16; // reserved
            unsigned int     Desc_Num              : 7;  // Number of descriptors need to load. When
                                                         // Desc_Buffer_En=1, then Desc_Num can only be 1, since bci
                                                         // can only update one descriptor info once.
       } Cmd_Blk_Cmd_Csp_Ld_Des_T_Dword2;
       typedef struct _Cmd_Blk_Cmd_Csp_Ld_Des_T_Dword3
                                                    // 9h DWF3: DWORD for BlockCommand BCI load descriptor cmd when load
                                                    // T#/S#/U#/T&S#. This dw is only valid when Desc_Buffer_En=1.
       {
            unsigned int     Buf_Range             : 28; // Set with "VkDeviceSize range" in
                                                         // VkDescriptorBufferInfo.
            unsigned int     Reserved              : 4;  // reserved
       } Cmd_Blk_Cmd_Csp_Ld_Des_T_Dword3;

       typedef struct _Cmd_Blk_Cmd_Csp_Ld_Des_T_Dword4
                                                    // 9h DWF4: DWORD for BlockCommand BCI load descriptor cmd when load
                                                    // T#/S#/U#/T&S#. This dw is only valid when Desc_Buffer_En=1.
       {
            unsigned int     Desc_Dynamic_Offset   : 28; // When the descriptor is buffer, then it's buffer's dynamic
                                                         // offset in byte.
            unsigned int     Reserved              : 4;  // reserved
       } Cmd_Blk_Cmd_Csp_Ld_Des_T_Dword4;

       typedef struct _Cmd_Blk_Cmd_Csp_Ld_Des_T_Dword5_CHX004
                                                    // 9h DWF5: DWORD for BlockCommand BCI load descriptor cmd when load
                                                    // T#/S#/U#/T&S#
       {
            unsigned int     Reg_Offset1           : 13; // Des reg offset for T# and U#. Only valid when type=T# , U# or
                                                         // S_T#
            unsigned int     Reg_Offset2           : 13; // Des reg offset for S#. Only valid when type=S# or S_T#.
            unsigned int     Reserved              : 1;
            unsigned int     Des_Block_Id          : 5;  // Des reg block id for T/S/U/T+S
       } Cmd_Blk_Cmd_Csp_Ld_Des_T_Dword5_CHX004;
       typedef struct _Cmd_Blk_Cmd_Csp_Ld_Des_T_Dword5_Elt3k
                                                    // 9h DWF5: DWORD for BlockCommand BCI load descriptor cmd when load
                                                    // T#/S#/U#/T&S#. This dw is only valid when Desc_Buffer_En=1.
       {
            unsigned int     Desc_Dynamic_Offset   : 28; // When the descriptor is buffer, then it's buffer's dynamic
                                                         // offset in byte.
            unsigned int     Reserved              : 4;  // reserved
       } Cmd_Blk_Cmd_Csp_Ld_Des_T_Dword5_Elt3k;
       typedef struct _Cmd_Blk_Cmd_Csp_Ld_Des_T_Dword6_Elt3k
                                                    // 9h DWF6: DWORD for BlockCommand BCI load descriptor cmd when load
                                                    // T#/S#/U#/T&S#
       {
            unsigned int     Reg_Offset1           : 13; // Des reg offset for T# and U#. Only valid when type=T# , U# or
                                                         // S_T#
            unsigned int     Reg_Offset2           : 13; // Des reg offset for S#. Only valid when type=S# or S_T#.
            unsigned int     Reserved              : 6;
       } Cmd_Blk_Cmd_Csp_Ld_Des_T_Dword6_Elt3k;
       typedef struct _Cmd_Blk_Cmd_Csp_Ld_Des_C_Dword1
                                                    // 9h DWF1: DWORD for BlockCommand BCI load descriptor cmd when load C#
       {
            unsigned int     Desc_Base_Addr_Low32  : 32; // base address for T#/S#/U# group, byte address.
       } Cmd_Blk_Cmd_Csp_Ld_Des_C_Dword1;
       typedef struct _Cmd_Blk_Cmd_Csp_Ld_Des_C_Dword2
                                                    // 9h DWF2: DWORD for BlockCommand BCI load descriptor cmd when load C#
       {
            unsigned int     Desc_Base_Addr_High8  : 8;
            unsigned int     L2_Cachable           : 1;  // whether the descriptor data goes into L2 cache, csp bypass
                                                         // to mxu
            unsigned int     Reserved              : 23; // reserved
       } Cmd_Blk_Cmd_Csp_Ld_Des_C_Dword2;
       typedef struct _Cmd_Blk_Cmd_Csp_Ld_Des_C_Dword3
                                                    // 9h DWF3: DWORD for BlockCommand BCI load descriptor cmd when load C#
       {
            unsigned int     Cb_Range              : 28; // Byte size of constant to load, 0~2^27. Should be aligned to
                                                         // 4. Set with "VkDeviceSize range" in
                                                         // VkDescriptorBufferInfo.
            unsigned int     Reserved              : 4;
       } Cmd_Blk_Cmd_Csp_Ld_Des_C_Dword3;
       typedef struct _Cmd_Blk_Cmd_Csp_Ld_Des_C_Dword4
                                                    // 9h DWF4: DWORD for BlockCommand BCI load descriptor cmd when load C#
       {
            unsigned int     Cb_Full_Range         : 28; // When CB_Range=-1, then should use this CB_Full_Range to
                                                         // calculate how many byte cb to load. Byte size here, should
                                                         // aligend to 4.
            unsigned int     Reserved              : 4;
       } Cmd_Blk_Cmd_Csp_Ld_Des_C_Dword4;
       typedef struct _Cmd_Blk_Cmd_Csp_Ld_Des_C_Dword5
                                                    // 9h DWF5: DWORD for BlockCommand BCI load descriptor cmd when load C#
       {
            unsigned int     Cb_Original_Offset    : 28; // Original byte offset for constant buffer. Should be
                                                         // aligned to 4 bytes.
            unsigned int     Reserved              : 4;
       } Cmd_Blk_Cmd_Csp_Ld_Des_C_Dword5;
       typedef struct _Cmd_Blk_Cmd_Csp_Ld_Des_C_Dword6
                                                    // 9h DWF6: DWORD for BlockCommand BCI load descriptor cmd when load C#
       {
            unsigned int     Cb_Dynamic_Offset     : 28; // Dynamic byte offset for constant buffer. Should be
                                                         // aligned to 4 bytes. When prefetch for constant data, calc
                                                         // the prefetch address with
                                                         // CB_Base_Addr+CB_Origin_Offset+CB_Dynamic_Offset.
            unsigned int     Reserved              : 4;
       } Cmd_Blk_Cmd_Csp_Ld_Des_C_Dword6;
       typedef struct _Cmd_Blk_Cmd_Csp_Ld_Des_C_Dword7
                                                    // 9h DWF7: DWORD for BlockCommand BCI load descriptor cmd when load C#
       {
            unsigned int     Reg_Offset            : 13; // Des reg offset for C#
            unsigned int     Reserved              : 14;
            unsigned int     Des_Block_Id          : 5;
       } Cmd_Blk_Cmd_Csp_Ld_Des_C_Dword7;
// some other description about the command usage: End
/////////////////////////////////////////////////////////////////////////

 typedef struct _Cmd_Blk_Cmd_Csp_Force_Flush
{
    unsigned int Dwc        :3; //Number of dw followed, should be 0 for force flush cmd
    unsigned int Reserved        :14; //
    unsigned int Reg_Type        :3; //reg type need to force flush
    unsigned int Type        :3; //
    unsigned int Block_Id        :5; //Block ID set to CSP
    unsigned int Major_Opcode        :4; //1001: BlockCommand(With multiple dw followed, for block level parsing)
} Cmd_Blk_Cmd_Csp_Force_Flush;

typedef struct _Cmd_Block_Command_Video     // 9h: BlockCommand for VIDEO
{
    unsigned int     Dwc                   : 15; // Number of dw followed, set to 4 for external fence
    unsigned int     Irq                   : 1;  // Interrupt
    unsigned int     External_Fence        : 1;  // External fence
    unsigned int     Reserved_1            : 2;
    unsigned int     Fence_Update_Timing   : 1;  // When this bit is 1, MXU will send all write requests out then
                                                 // update the fence value.
    unsigned int     Reserved_2            : 3;
    unsigned int     Block_Id              : 5;  // Block ID set to VIDEO. Should be set to 0xC.
    unsigned int     Major_Opcode          : 4;  // 1001: BlockCommand(With multiple dw followed, for block
                                                 // level parsing)
} Cmd_Block_Command_Video;
/////////////////////////////////////////////////////////////////////////
// some other description about the command usage: Start
       typedef struct _Cmd_Block_Command_Video_Dword0
                                                    // 9h DWF1: DWORD for BlockCommand VIDEO
       {
            unsigned int     Address_Low32         : 32; // Here is the low 32 bit addr.
       } Cmd_Block_Command_Video_Dword0;
       typedef struct _Cmd_Block_Command_Video_Dword1
                                                    // 9h DWF2: DWORD for BlockCommand VIDEO
       {
            unsigned int     Address_High8         : 8;  // The address is supposed to be 32bits aligned. Here is the
                                                         // high 8 bits
            unsigned int     Reserved              : 24;
       } Cmd_Block_Command_Video_Dword1;
       typedef struct _Cmd_Block_Command_Video_Dword2
                                                    // 9h DWF3: DWORD for BlockCommand VIDEO
       {
            unsigned int     Value                 : 32; // Output DWORD0
       } Cmd_Block_Command_Video_Dword2;
       typedef struct _Cmd_Block_Command_Video_Dword3
                                                    // 9h DWF4: DWORD for BlockCommand VIDEO
       {
            unsigned int     Value                 : 32; // Output DWORD1
       } Cmd_Block_Command_Video_Dword3;
// some other description about the command usage: End
/////////////////////////////////////////////////////////////////////////


typedef struct _Cmd_Gp                      // Bh: General Purpose Computing. According to Paul, in E3KN, the
                                            // global/local size is set to EU's reg.
{
    unsigned int     Dwc                   : 4;  // For indirect GP, Dwc=12, the first 2 dw are for address, and
                                                 // the 10 dw left are GP parameters. For direct GP, Dwc=13.
    unsigned int     Wait_Done_En          : 1;  // When CSP needs to wait the group splitter done in GPCPFE to
                                                 // update the main-parser pointer
    unsigned int     Predicate_En          : 1;  // For predicate rendering of gp cmd
    unsigned int     Reserved              : 14; // Reserved
    unsigned int     Indirect_En           : 1;  // For indirect GP, the extra dwc=3.
    unsigned int     Dw_Type               : 1;  // Type of the 1st 3 dw sent to GPCPool
    unsigned int     Minor_Opcode          : 1;
    unsigned int     Block_Id              : 5;  // Block ID set to EUP
    unsigned int     Major_Opcode          : 4;  // 1011: General Purpose Computing
} Cmd_Gp;
/////////////////////////////////////////////////////////////////////////
// some other description about the command usage: Start
       typedef struct _Cmd_Gp_Dword1                // Bh DWF1: DWORD for GP cmd
       {
            unsigned int     Global_Size_X         : 32; // whether it's global size or group number is decided by
                                                         // "Dw_Type"
       } Cmd_Gp_Dword1;
       typedef struct _Cmd_Gp_Dword2                // Bh DWF2: DWORD for GP cmd
       {
            unsigned int     Global_Size_Y         : 32;
       } Cmd_Gp_Dword2;
       typedef struct _Cmd_Gp_Dword3                // Bh DWF3: DWORD for GP cmd
       {
            unsigned int     Global_Size_Z         : 32;
       } Cmd_Gp_Dword3;
       typedef struct _Cmd_Gp_Dword4                // Bh DWF4: DWORD for GP cmd
       {
            unsigned int     Work_Group_Size_X     : 32;
       } Cmd_Gp_Dword4;
       typedef struct _Cmd_Gp_Dword5                // Bh DWF5: DWORD for GP cmd
       {
            unsigned int     Work_Group_Size_Y     : 32;
       } Cmd_Gp_Dword5;
       typedef struct _Cmd_Gp_Dword6                // Bh DWF6: DWORD for GP cmd
       {
            unsigned int     Work_Group_Size_Z     : 32;
       } Cmd_Gp_Dword6;
       typedef struct _Cmd_Gp_Dword7                // Bh DWF7: DWORD for GP cmd
       {
            unsigned int     Global_Id_Offset_X    : 32;
       } Cmd_Gp_Dword7;
       typedef struct _Cmd_Gp_Dword8                // Bh DWF8: DWORD for GP cmd
       {
            unsigned int     Global_Id_Offset_Y    : 32;
       } Cmd_Gp_Dword8;
       typedef struct _Cmd_Gp_Dword9                // Bh DWF9: DWORD for GP cmd
       {
            unsigned int     Global_Id_Offset_Z    : 32;
       } Cmd_Gp_Dword9;
       typedef struct _Cmd_Gp_Dword10               // Bh DWF10: DWORD for GP cmd
       {
            unsigned int     Shader_Offset         : 32;
       } Cmd_Gp_Dword10;
       typedef struct _Cmd_Gp_Dword11               // Bh DWF11: DWORD for GP cmd
       {
            unsigned int     Shader_Range          : 32;
       } Cmd_Gp_Dword11;
       typedef struct _Cmd_Gp_Dword12               // Bh DWF12: DWORD for GP cmd
       {
            unsigned int     Argumentaddr_X        : 32;
       } Cmd_Gp_Dword12;
       typedef struct _Cmd_Gp_Dword13               // Bh DWF13: DWORD for GP cmd
       {
            unsigned int     Argumentaddr_Y        : 32;
       } Cmd_Gp_Dword13;
       typedef struct _Cmd_Indirect_Gp_Dword1       // Bh DWF1: DWORD for indirect GP cmd
       {
            unsigned int     Address_Low32         : 32;
       } Cmd_Indirect_Gp_Dword1;
       typedef struct _Cmd_Indirect_Gp_Dword2       // Bh DWF2: DWORD for indirect GP cmd
       {
            unsigned int     Address_High8         : 8;
            unsigned int     L2_Cachable           : 1;  // whether indirct GP's prefetch data goes into L2 cache, csp
                                                         // bypass to mxu
            unsigned int     Reserved              : 23;
       } Cmd_Indirect_Gp_Dword2;
// some other description about the command usage: End
/////////////////////////////////////////////////////////////////////////


typedef struct _Cmd_Tbr_Indicator           // Fh: begin/end indicator for the loop cmd list
{
    unsigned int     Reserved              : 20;
    unsigned int     Skip_En               : 1;  // For tbr indicator cmd at the beginning/end of high RB, set
                                                 // this bit into 1. If csp switch to HRB in IMM mode, csp should
                                                 // skip the tbr indicator cmd with "Skip_En=1".
    unsigned int     Indicator_Info        : 2;
    unsigned int     Block_Id              : 5;  // set to TASBE
    unsigned int     Major_Opcode          : 4;  // 1111: TBR begin/end indicator
} Cmd_Tbr_Indicator;

typedef struct _Cmd_Vpp                     // Ch: VPP Command. For VPP cmd in VPP RB, VPP engine parse it, csp just
                                            // bypss cmd in VPP RB into VPP engine in E3K.
{
    unsigned int     Dwf                   : 3;  // Number of dw followed
    unsigned int     Reserved              : 7;
    unsigned int     Clear_Mode            : 3;
    unsigned int     Color_Mode            : 1;  // Color mode, 4bit clear value or 8 bit clear value
    unsigned int     Counter               : 11;
    unsigned int     Auto_Clear            : 1;  // 1: enable auto clear
    unsigned int     Flush_Blc             : 1;  // 1: flush burst length cache
    unsigned int     Invalidate_Blc        : 1;  // 1: invalidate burst length cache
    unsigned int     Major_Opcode          : 4;  // 1100: VPP Cmd
} Cmd_Vpp;

typedef union Csp_Opcodes_cmds
{
    Cmd_Skip                        cmd_Skip;
    Cmd_Dma                         cmd_Dma;
    Cmd_Wait                        cmd_Wait;
    Cmd_Query_Dump                  cmd_Query_Dump;
    Cmd_Set_Register                cmd_Set_Register;
    Cmd_Execute_Indirect            cmd_Execute_Indirect;
    Cmd_Set_Object                  cmd_Set_Object;
    Cmd_Dip                         cmd_Dip;
    Cmd_Fence                       cmd_Fence;
    Cmd_Block_Command_Template      cmd_Block_Command_Template;
    Cmd_Block_Command_Tas           cmd_Block_Command_Tas;
    Cmd_Block_Command_Sg            cmd_Block_Command_Sg;
    Cmd_Block_Command_Img_Trn       cmd_Block_Command_Img_Trn;
    Cmd_Block_Command_Flush         cmd_Block_Command_Flush;
    Cmd_Block_Command_Eu            cmd_Block_Command_Eu;
    Cmd_Block_Command_Tu            cmd_Block_Command_Tu;
    Cmd_Block_Command_Tu_CHX004     cmd_Block_Command_Tu_CHX004;
    Cmd_Block_Command_L2            cmd_Block_Command_L2;
    Cmd_Block_Command_Mxu           cmd_Block_Command_Mxu;
    Cmd_Block_Command_Csp           cmd_Block_Command_Csp;
    Cmd_Blk_Cmd_Csp_Blc             cmd_Blk_Cmd_Csp_Blc;
    Cmd_Blk_Cmd_Csp_Save_Rsto       cmd_Blk_Cmd_Csp_Save_Rsto;
    Cmd_Blk_Cmd_Csp_Indicator       cmd_Blk_Cmd_Csp_Indicator;
    Cmd_Blk_Cmd_Csp_Ld_Des          cmd_Blk_Cmd_Csp_Ld_Des;
    Cmd_Blk_Cmd_Csp_Force_Flush        cmd_Blk_Cmd_Csp_Force_Flush;
    Cmd_Block_Command_Video         cmd_Block_Command_Video;
    Cmd_Gp                          cmd_Gp;
    Cmd_Tbr_Indicator               cmd_Tbr_Indicator;
    Cmd_Vpp                         cmd_Vpp;
    unsigned int                    uint ;
}Csp_Opcodes_cmd;


#endif
