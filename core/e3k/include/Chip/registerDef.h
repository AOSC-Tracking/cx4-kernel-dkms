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
#ifndef _REGISTER_DEF_H
#define _REGISTER_DEF_H

#include     "BlockID.h"
#include      "CSP_GLOBAL_Register.h"
#include     "CSP_OPCODE.h"
#include      "TASFE_reg.h"
#include      "TASBE_reg.h"
#include      "EU_FS_reg.h"
#include      "EU_CS_reg.h"
#include      "IU_reg.h"
#include      "SPIN_register.h"
#include      "SPOUT_register.h"
#include      "EU_PS_reg.h"
#include      "FF_registers.h"
#include      "TU_Reg.h"
#include      "GPCPBE_register.h"
#include      "GPCPFE_register.h"
#include      "MMU_registers.h"
#include      "MXU_registers.h"
#include      "Vcp_Registers.h"
#include      "WLS_Registers.h"
#include      "L2_Register.h"
#include     "VCP_OPCODE_DECOUPLE.h"

#define MMIO_FLAG 0x000FF000
#define MMIO_MIU_START_ADDRESS                  0x00008000
#define MMIO_MIU_DYNAMIC_FB_START_ADDRESS       0x00008B00
#define MMIO_CSP_START_ADDRESS                  0x00030000
#define MMIO_MMU_START_ADDRESS                  0x00050000
#define MMIO_MXU_START_ADDRESS                  0x00049000
#define MMIO_VCP0_START_ADDRESS                 0x0004A000    //decided by HW, range [0x4_A000, 0x4_AFFF]
#define MMIO_VPP0_START_ADDRESS                 0x0004B000  //decided by HW, range [0x4_B000, 0x4_BFFF]
#define MMIO_VCP1_START_ADDRESS                 0x0004C000    //decided by HW, range [0x4_C000, 0x4_CFFF]
#define MMIO_VCP2_START_ADDRESS                 0x0004D000    //decided by HW, range [0x4_D000, 0x4_DFFF]
#define MMIO_VCP3_START_ADDRESS                 0x0004E000    //decided by HW, range [0x4_E000, 0x4_EFFF]
#define MMIO_VPP1_START_ADDRESS                 0x0004F000    //decided by HW, range [0x4_F000, 0x4_FFFF]
#define MMIO_MIU_BASE_ADDRESS                   0xD8130000  //decided by Video, video don't need MIU reg base 0xd8130000

//  [7/21/2018 PaulZhang] use the macro to define EU MMIO address:  start 0x200 (dword) in CSP space
#define MMIO_EU_START_ADDRESS_CHX004            (MMIO_CSP_START_ADDRESS|(Reg_Eu_Dbg_Reg_Offset<<2))
#define MMIO_EU_END_ADDRESS_CHX004              (MMIO_CSP_START_ADDRESS|(CSP_GLOBAL_REG_LIMIT_CHX004<<2))
#define MMIO_EU_START_ADDRESS_Elt3k             (MMIO_CSP_START_ADDRESS|(Reg_Eu_Dbg_Cfg_Offset<<2))
#define MMIO_EU_END_ADDRESS_Elt3k               (MMIO_CSP_START_ADDRESS|(CSP_GLOBAL_REG_LIMIT_Elt3k<<2))

// Complement to FF_registers.h
#define FF_ZCLIPMIN_OFFSET 0
#define FF_ZCLIPMAX_OFFSET 1

#define FF_SFRONTFACE_OFFSET 0
#define FF_SBACKFACE_OFFSET  1

#define FF_RT_ADDR_OFFSET 0
#define FF_RT_DEPTH_OFFSET 1
#define FF_RT_VIEW_CTRL_OFFSET 3

#define FF_RT_FMT_OFFSET  0
#define FF_RT_SIZE_OFFSET 1
#define FF_RT_DESC_OFFSET_CHX004 1
#define FF_RT_DESC_OFFSET_Elt3k 0
#define FF_RT_MISC_OFFSET 2
#define FF_RT_REG_SIZE 3

#define FF_BS_BLEND_CTL_OFFSET 6

#define FF_RT_CTRL_REG_SIZE (sizeof(Reg_Rt_Ctrl_Group)>>2)
#define FF_RT_ADDR_CTRL_REG_SIZE (sizeof(Reg_Rt_Addr_Ctrl_Group)>>2)
#define FF_RT_DST 0
#define FF_RT_SRC 1

#define FF_16FL_COLOR_OFFSET     0 //16 Float low dword
#define FF_16FH_COLOR_OFFSET     1 //16 Float high dword
#define FF_8888_COLOR_OFFSET     2 //8 8 8 8 unorm
#define FF_2101010L_COLOR_OFFSET 3 //2 10 10 10 unorm low dword store B10 G10 R10
#define FF_2101010H_COLOR_OFFSET 4 //2 10 10 10 unorm high dword extend A2 to A10

#define FF_FG_COLOR_OFFSET 0 //Front ground color offset
#define FF_BG_COLOR_OFFSET 1 //Back ground color offset

#endif
