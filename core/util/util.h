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
#ifndef __UTIL_H_
#define __UTIL_H_

#include "core_errno.h"
#include "core_import.h"

#define bit_0    (1 <<  0)
#define bit_1    (1 <<  1)
#define bit_2    (1 <<  2)
#define bit_3    (1 <<  3)
#define bit_4    (1 <<  4)
#define bit_5    (1 <<  5)
#define bit_6    (1 <<  6)
#define bit_7    (1 <<  7)
#define bit_8    (1 <<  8)
#define bit_9    (1 <<  9)

#define bit_10   (1 << 10)
#define bit_11   (1 << 11)
#define bit_12   (1 << 12)
#define bit_13   (1 << 13)
#define bit_14   (1 << 14)
#define bit_15   (1 << 15)
#define bit_16   (1 << 16)
#define bit_17   (1 << 17)
#define bit_18   (1 << 18)
#define bit_19   (1 << 19)

#define bit_20   (1 << 20)
#define bit_21   (1 << 21)
#define bit_22   (1 << 22)
#define bit_23   (1 << 23)
#define bit_24   (1 << 24)
#define bit_25   (1 << 25)
#define bit_26   (1 << 26)
#define bit_27   (1 << 27)
#define bit_28   (1 << 28)
#define bit_29   (1 << 29)

#define bit_30   (1 << 30)
#define bit_31   (1u << 31)


#define util_get_offset(type, element) (long)(&(((type *)0)->element))
#define util_4k_align(size)      ((size + 4095) & ~4095)
#define util_256byte_align(size) ((size + 255) & ~255)
#define util_2kbits_align(size)  util_256byte_align(size)
#define util_align(size, align)  ((size + align - 1) & (~(align - 1)))

#define util_fabs(v)   ((v) > 0.0 ? (v) : -(v))
#define util_ffabs(v)  v&0x7fffffff
#define util_max(a, b) ( a > b ? a : b)
#define util_min(a, b) ( a > b ? b : a)

#define DIV_ROUND_UP(n, d)  (((n) + (d) - 1) / (d))
#define BITS_TO_LONGS(nr) DIV_ROUND_UP(nr, 8 * sizeof(long))


typedef int (*util_event_handler_t)(void *data, zx_event_status_t ret);

typedef struct util_thread_info
{
    void       *private_data;  //engine/global scheduler struct
    const char *thread_name;   //thread name
    int         msec;          //thread timeout scope, unit in millisecond
    int         priority;      //thread scheduler priority, defalut value is 99,
                               //user mode can set this value to change the priority
} util_thread_info_t;

typedef struct
{
    void                 *os_thread;     //os thread
    struct os_wait_event *event;         //wait event
    void                 *private_data;  //engine/global scheduler struct
    util_event_handler_t event_handler;  //thread handler
    int                  timeout_msec;   //thread timeout scope, unit in millisecond
    int                  can_freeze;     //flag if thread can freeze
    int                  priority;       //thread scheduler priority, defalut value is 99,
                                         //user mode can set this value to change the priority
}util_event_thread_t;


typedef struct
{
    void                                        *os_thread;
    struct general_wait_event   **events;
    int                                            event_cnt;
    void                                        *private_data;
    struct os_wait_queue              *wait_queue;
    util_event_handler_u                event_handler;
    int                                           timeout_msec;
    int                                           can_freeze;
}util_multiple_event_thread_t;

static inline int util_IsPow2(unsigned int value)
{
    return (!(value & (value-1)));
}

/*---------------------------------------------------------------------------*/
/* NearestLog2()                                                             */
/*                                                                           */
/* Finds the closest integer base-2 log of a value.                          */
/* (0 will return 0xFFFFFFFF)                                                */
/*---------------------------------------------------------------------------*/
static inline unsigned int util_NearestLog2(unsigned int  val)
{
    unsigned int result = 0xFFFFFFFF;
    unsigned int input  = val;

    while (val & 0xFFFFFFF0)
    {
        val >>= 4;
        result += 4;
    }

    while (val)
    {
        val >>= 1;
        result++;
    }

    //
    // if val is not a pow2, increment by one, since
    // this function floors
    //

    if (util_IsPow2(input) == 0)
    {
        result++;
    }

    return(result);
}

static inline int util_get_msb_position(unsigned int num)
{
    unsigned int msbpos = 0;

    zx_assert(num > 0, "num = %d", num);
    while (num > 1)
    {
        num >>= 1;
        msbpos++;
    }
    return msbpos;
}

static inline int util_get_lsb_position(unsigned int data)
{
    int i;

    zx_assert(data > 0, "data = %d", data);

    for(i = 0; i < sizeof(unsigned int)*8; i++)
    {
        if(data & (1u << i))
            break;
    }

    return i;
}

extern void util_dump_memory(void *buffer, unsigned int size, unsigned char *msg);
extern void util_dump_raw_memory_to_file(void *buffer, unsigned int size, char *filename);
extern void util_dump_memory_to_file(void *buffer, unsigned int size, char *private_msg, char *filename);
extern void util_write_to_file(char *buffer, unsigned int size, char *private_msg, char *filename);
extern long util_get_ptr_span_trace(void *start, void *end, const char *func, int line);
extern int  util_wait_condition_timeout(condition_func_t condition, void *argu, int timeout);

#define util_get_ptr_span(start, end)  util_get_ptr_span_trace(start, end, __func__, __LINE__)

extern unsigned int util_log2(unsigned int s);

extern util_event_thread_t *util_create_event_thread(util_event_handler_t handler, util_thread_info_t *info);
extern struct general_wait_event*  util_create_event(int  need_queue);
extern                        util_multiple_event_thread_t  *util_create_multiple_event_thread(util_event_handler_u handler, void *private_data, struct general_wait_event **events, int cnt, const char *thread_name, int msec);
extern void                 util_destroy_multiple_event_thread(util_multiple_event_thread_t  *thread);
extern void                 util_destroy_event_thread(util_event_thread_t *thread);
extern void                 util_wakeup_event_thread(util_event_thread_t *thread);
extern void                 util_wakeup_multiple_event_thread(util_multiple_event_thread_t  *thread, struct general_wait_event  *event);
extern unsigned int util_crc32(unsigned char *pBuf, unsigned int nSize);
#define ENABLE_UTIL_LOG 0

#if ENABLE_UTIL_LOG
extern void util_init_log(void);
extern void util_log(const char *fmt, ...);
extern void util_print_log(void);
#else
#define util_init_log()
#define util_log(args...)
#define util_print_log()
#endif


#endif

