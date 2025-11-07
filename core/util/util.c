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
#include "util.h"

unsigned int util_log2(unsigned int s)
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

unsigned int util_crc32(unsigned char *pBuf, unsigned int nSize)
{
    static unsigned int s_u32CrcTables[256];
    static unsigned int  s_bInited = 0;
    unsigned int crc;
    const unsigned char *p;

    if (!s_bInited)
    {
        unsigned int poly = 0x04C11DB7;
        unsigned int i;
        unsigned int j;

        for (i = 0; i < 256; i++)
        {
            crc = i << 24;
            for (j = 0; j < 8; j++)
            {
                crc = (crc << 1) ^ ((crc & 0x80000000) ? (poly) : 0);
            }
            s_u32CrcTables[i] = crc;
        }

        s_bInited =1;
    }

    crc = 0xFFFFFFFF;
    for (p = pBuf; p < pBuf + nSize; ++p)
    {
        crc = (crc << 8) ^ s_u32CrcTables[((crc >> 24) ^ *p) & 0xFF];
    }

    return crc;
}

/*
** buffer: memory to dump
** size  : dumped size in DWORD
*/

void util_dump_memory(void *buffer, unsigned int size, unsigned char *msg)
{
    unsigned int dword = size >> 2;
    unsigned int row   = dword >> 2;
    unsigned int left  = dword%4;
    unsigned int *data = buffer;
    unsigned int i;

    if((size == 0) || (buffer == NULL)) return;

    zx_info("<< ------ Start dump %s - Size:0x%x, dword:0x%x ------>>\n", msg, size, dword);

    for(i = 0; i < row; i++)
    {
        zx_info("#%p %08x, %08x, %08x, %08x\n", data, data[0], data[1], data[2], data[3]);
        data = data + 4;
    }

    if(left == 3)
    {
        zx_info("#%p %08x, %08x, %08x\n", data, data[0], data[1], data[2]);
    }
    else if(left == 2)
    {
        zx_info("#%p %08x, %08x\n", data, data[0], data[1]);
    }
    else if(left == 1)
    {
        zx_info("#%p %08x\n", data, data[0]);
    }

    zx_info("<< ----------- End dump %s ---------------->>\n", msg);
}

/*
 * buffer: memory to dump
 * size  : dumped size in byte
 */
void util_dump_raw_memory_to_file(void *buffer, unsigned int size, char *filename)
{
    struct os_file *file = zx_file_open(filename, OS_WRONLY | OS_CREAT, 0644);

    if(file == NULL)
    {
        zx_error("open file %s failed\n", filename);
        return;
    }

    if(buffer != NULL && size != 0)
    {
        zx_file_write(file, buffer, size);
    }

    zx_file_close(file);
}

void util_dump_memory_to_file(void *buffer, unsigned int size, char *private_msg, char *filename)
{
    struct os_file *file = zx_file_open(filename, OS_WRONLY | OS_CREAT | OS_APPEND, 0644);
    int    len           = zx_strlen(private_msg);
    char   *msg          = zx_calloc(256);

    if(file == NULL)
    {
        zx_free(msg);
        return;
    }

    if(len > 0)
    {
        zx_file_write(file, private_msg, len);
    }

    if(buffer != NULL && size != 0)
    {
        unsigned int dword = size >> 2;
        unsigned int row   = dword >> 2;
        unsigned int left  = dword%4;
        unsigned int *data = buffer;
        unsigned int i;

        zx_vsnprintf(msg, 256, "-------------------->\nmemory_dump: 0x%p -> 0x%p, size:0x%x.\n", buffer, (char*)buffer+size, size);
        zx_file_write(file, msg, zx_strlen(msg));

        zx_vsnprintf(msg, 256, "dword: 0x%x, row:0x%x, left:0x%x.\n", dword, row, left);
        zx_file_write(file, msg, zx_strlen(msg));
        for(i = 0; i < row; i++)
        {
            zx_vsnprintf(msg, 256, "#%p %08x, %08x, %08x, %08x\n", data, data[0], data[1], data[2], data[3]);
            zx_file_write(file, msg, zx_strlen(msg));
            data += 4;
        }

        if(left == 3)
        {
            zx_vsnprintf(msg, 256, "#%p %08x, %08x, %08x\n", data, data[0], data[1], data[2]);
            zx_file_write(file, msg, zx_strlen(msg));
        }
        else if(left == 2)
        {
            zx_vsnprintf(msg, 256, "#%p %08x, %08x\n", data, data[0], data[1], data[2]);
            zx_file_write(file, msg, zx_strlen(msg));
        }
        else if(left == 1)
        {
            zx_vsnprintf(msg, 256, "#%p %08x\n", data, data[0], data[1], data[2]);
            zx_file_write(file, msg, zx_strlen(msg));
        }
        zx_vsnprintf(msg, 256, "-------------------->");
        zx_file_write(file, msg, zx_strlen(msg));
    }

    zx_free(msg);
    zx_file_close(file);

}

void util_write_to_file(char *buffer, unsigned int size, char *private_msg, char *filename)
{
    struct os_file *file = zx_file_open(filename, OS_WRONLY | OS_CREAT | OS_APPEND, 0644);
    int    len           = zx_strlen(private_msg);

    if(file == NULL)
    {
        return;
    }

    if(len > 0)
    {
        zx_file_write(file, private_msg, len);
    }

    if(buffer != NULL && size != 0)
    {
        zx_file_write(file, buffer, size);
    }

    zx_file_close(file);
}

long util_get_ptr_span_trace(void *start, void *end, const char *func, int line)
{
    long span = (char*)start - (char*)end;

#ifdef _DEBUG_
    if(span < 0)
    {
        zx_error("span calc failed: start:%p, end:%p, func:%s, line:%d.\n",
            start, end, func, line);
    }
#endif
    return span;
}

int util_wait_condition_timeout(condition_func_t condition, void *argu, int timeout)
{
    unsigned long long      start_time = 0, current_time, delta_time;
    long                    temp_sec, temp_usec;
    int                     time_out   = FALSE;
    //save the time on entry
    zx_getsecs(&temp_sec, &temp_usec);
    start_time = temp_sec * 1000000 + temp_usec;

    for(;;)
    {
        //zx_msleep(1);

        if(condition(argu))
        {
            break;
        }

        zx_getsecs(&temp_sec, &temp_usec);

        current_time = temp_sec * 1000000 + temp_usec;
        delta_time   = current_time - start_time;

        if(timeout != -1 && delta_time > timeout)
        {
            time_out = TRUE;
            break;
        }
    }

    return time_out;
}

static int util_event_thread(void *data)
{
    util_event_thread_t *thread = data;
    zx_event_status_t ret;

    zx_set_freezable();

    do
    {
        ret = zx_thread_wait(thread->event, thread->timeout_msec);

        thread->event_handler(thread->private_data, ret);

        if(thread->can_freeze)
        {
            zx_try_to_freeze();
        }

    } while(!zx_thread_should_stop());

    return 0;
}

struct general_wait_event*  util_create_event(int  need_queue)
{
    struct general_wait_event     *event = zx_calloc(sizeof(struct general_wait_event));

    if(event)
    {
        event->need_queue = need_queue;
        event->event = 0;
        event->event_lock = zx_create_spinlock();

        return event;
    }

    return NULL;
}

util_event_thread_t *util_create_event_thread(util_event_handler_t handler, util_thread_info_t *info)
{
    util_event_thread_t *thread = zx_calloc(sizeof(util_event_thread_t));

    thread->timeout_msec  = info->msec;
    thread->can_freeze    = TRUE; //default freeze value, if want freeze control by thread, adjust this value in thread handler
    thread->event         = zx_create_event(0);
    thread->event_handler = handler;
    thread->private_data  = info->private_data;
    thread->priority      = info->priority ? info->priority : 99; //defalut value is 99

    thread->os_thread     = zx_create_thread(util_event_thread, thread, info->thread_name);

    return thread;
}

void util_destroy_event_thread(util_event_thread_t *thread)
{
    zx_destroy_thread(thread->os_thread);
    zx_destroy_event(thread->event);

    zx_free(thread);
}

void util_wakeup_event_thread(util_event_thread_t *thread)
{
    zx_thread_wake_up(thread->event);
}

void util_wakeup_multiple_event_thread(util_multiple_event_thread_t  *thread, struct general_wait_event  *event)
{
    zx_spin_lock_bh(event->event_lock);
    general_thread_wake_up(thread->wait_queue, event);
    zx_spin_unlock_bh(event->event_lock);
}


int util_multiple_event_thread(void *data)
{
    util_multiple_event_thread_t *thread = data;
    event_wait_status               ret;

    zx_set_freezable();

    do
    {
        thread_wait_for_events(thread->wait_queue, thread->events, thread->event_cnt, thread->timeout_msec, &ret);

        thread->event_handler(thread->private_data, ret);

        if(thread->can_freeze)
        {
            zx_try_to_freeze();
        }

    } while(!zx_thread_should_stop());

    return 0;
}

util_multiple_event_thread_t  *util_create_multiple_event_thread(util_event_handler_u handler, void *private_data, struct general_wait_event **events, int cnt, const char *thread_name, int msec)
{
    util_multiple_event_thread_t *thread = zx_calloc(sizeof(util_multiple_event_thread_t));

    thread->timeout_msec  = msec;
    thread->can_freeze    = TRUE; //default freeze value, if want freeze control by thread, adjust this value in thread handler
    thread->events          = events;
    thread->event_cnt = cnt;
    thread->event_handler = handler;
    thread->private_data  = private_data;

    thread->wait_queue =   zx_create_wait_queue();
    thread->os_thread     = zx_create_thread(util_multiple_event_thread, thread, thread_name);

    return thread;
}

void util_destroy_multiple_event_thread(util_multiple_event_thread_t  *thread)
{
    struct general_wait_event *  event;
    int event_num = thread->event_cnt;
    int  i = 0;

    zx_destroy_thread(thread->os_thread);

    for(i = 0; i < event_num; i++)
    {
        event = thread->events[i];
        zx_destroy_event(event);
    }

    zx_free((int*)thread->wait_queue);

    zx_free(thread);
}

#if ENABLE_UTIL_LOG

#include <stdarg.h>

#define MAX_LINE_NUM   512
#define MAX_STR_LINGTH 256

static struct os_mutex *util_log_lock = NULL;
static char util_log_str[MAX_LINE_NUM][MAX_STR_LINGTH];
static int  util_log_id = 0;

void util_init_log(void)
{
    int i;
    util_log_lock = zx_create_mutex();

    for(i = 0; i < MAX_LINE_NUM; i++)
    {
        zx_memset(util_log_str[i], 0, MAX_STR_LINGTH);
    }
}

#include <linux/kernel.h>

void util_log(const char *fmt, ...)
{
    char     buffer[MAX_STR_LINGTH];
    int      index = 0;
    va_list  args;

    va_start(args, fmt);

    vsnprintf(buffer, sizeof(buffer), fmt, args);

    va_end(args);

    zx_mutex_lock(util_log_lock);

    index = (util_log_id % MAX_LINE_NUM);

    zx_vsnprintf(util_log_str[index], MAX_STR_LINGTH-1, "%8x: %s", util_log_id, buffer);

    util_log_id++;

    zx_mutex_unlock(util_log_lock);
}

void util_print_log(void)
{
    int i = 0;

    zx_mutex_lock(util_log_lock);

    zx_info("CURR_LOG_ID: %x.\n", util_log_id);

    for(i = 0; i < MAX_LINE_NUM; i++)
    {
        zx_info("%s", util_log_str[i]);
    }

    zx_mutex_unlock(util_log_lock);
}

#endif
