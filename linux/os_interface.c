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
#include "os_interface.h"
#include "zx.h"
#include "zx_driver.h"
#include "zx_params.h"
#include "zx_version.h"

#define __STR(x)    #x
#define STR(x)      __STR(x)

/* globals constants */
const unsigned int  ZX_PAGE_SHIFT         = PAGE_SHIFT;
#ifndef __frv__
const unsigned int  ZX_PAGE_SIZE          = PAGE_SIZE;
#else
const unsigned int  ZX_PAGE_SIZE          = 0x1000;
#endif
const unsigned long ZX_PAGE_MASK          = PAGE_MASK;
const unsigned long ZX_LINUX_VERSION_CODE = LINUX_VERSION_CODE;


char *zx_mem_track_result_path      = "/var/log/zx-mem-track.txt";

DEFINE_MUTEX(g_pages_memory_lock);
LIST_HEAD(g_pages_memory_list);

void zx_udelay(unsigned long long usecs_num)
{
    unsigned long long msecs = usecs_num;
    unsigned long      usecs = do_div(msecs, 1000);

    if(msecs != 0)mdelay(msecs);
    if(usecs != 0)udelay(usecs);
}

unsigned long long zx_do_div(unsigned long long x, unsigned long long y)
{
    do_div(x, y);
    return x;
}

void zx_msleep(int num)
{
    msleep(num);
}

void zx_getsecs(long *secs, long *usecs)
{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,6,0)
    struct timespec64 tv = {0};
    ktime_get_ts64(&tv);
#else
    struct timespec tv = {0};

    //do_posix_clock_monotonic_gettime(&tv);
    ktime_get_ts(&tv);
#endif

    if (secs)
    {
        *secs = tv.tv_sec;
    }

    if (usecs)
    {
        *usecs= tv.tv_nsec/1000;
    }
    return;
}

void zx_get_nsecs(unsigned long long *nsec)
{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,6,0)
    struct timespec64 tv = {0};
    ktime_get_ts64(&tv);
#else
    struct timespec tv = {0};

    //do_posix_clock_monotonic_gettime(&tv);
    ktime_get_ts(&tv);
#endif

    *nsec = (unsigned long long)(tv.tv_sec) * 1000000000 + tv.tv_nsec;

    return;
}

void zx_usleep_range(long min, long max)
{
    usleep_range(min, max);
}

void zx_panic(void)
{
    BUG();
}

void zx_dump_stack(void)
{
    dump_stack();
}

int ZX_API_CALL zx_copy_from_user(void* to, const void* from, unsigned long size)
{
    return copy_from_user(to, from, size);
}

int ZX_API_CALL zx_copy_to_user(void* to, const void* from, unsigned long size)
{
    return copy_to_user(to, from, size);
}

void* ZX_API_CALL zx_memset(void* s, int c, unsigned long count)
{
#if defined(__aarch64__)
    //aarch net support device memory memset, need more study.

    int i;
    char* set;

    set = (char*)s;

    for(i=0;i<count;i++)
    {
        set[i] = c;
    }
    return 0;
#else
    return memset(s, c, count);
#endif
}

void* ZX_API_CALL zx_memcpy(void* d, const void* s, unsigned long count)
{
#if defined(__aarch64__)
    int i;
    char *dst;
    char *set;

    set = (char*)s;
    dst = (char*)d;

    for(i=0;i<count;i++)
    {
        dst[i] = set[i];
    }
    return 0;
#else
    return memcpy(d, s, count);
#endif
}

int   ZX_API_CALL zx_memcmp(const void *s1, const void *s2, unsigned long count)
{
    return memcmp(s1, s2, count);
}

void ZX_API_CALL zx_byte_copy(char* dst, char* src, int len)
{
#ifdef __BIG_ENDIAN__

    int i = 0;
    int left;

    for(i = 0; i < len/4; i++)
    {
        *(dst)   =  *(src+3);
        *(dst+1) =  *(src+2);
        *(dst+2) =  *(src+1);
        *(dst+3) =  *(src);
        dst     +=  4;
        src     +=  4;
    }

    left = len & 3;

    for(i = 0; i < left; i++)
    {
        *(dst+3-i) = *(src+i);
    }

#else
    zx_memcpy(dst, src, len);
#endif
}

void ZX_API_CALL zx_console_lock(int lock)
{

   if(lock)
     console_lock();
   else
     console_unlock();
}

int   zx_strcmp(const char *s1, const char *s2)
{
    return strcmp(s1, s2);
}

char* zx_strcpy(char *d, const char *s)
{
    return strcpy(d, s);
}

int   zx_strncmp(const char *s1, const char *s2, unsigned long count)
{
    return strncmp(s1, s2, count);
}

char* zx_strncpy(char *d, const char *s, unsigned long count)
{
    return strncpy(d, s, count);
}

unsigned long zx_strlen(char *s)
{
    return strlen(s);
}

char *zx_strsep(char **s, const char *ct)
{
    return strsep(s, ct);
}

/****************************** IO access functions*********************************/

unsigned long long ZX_API_CALL zx_read64(void* addr)
{
#ifdef CONFIG_64BIT
    return readq(addr);
#else
    return 0ull;
#endif
}

unsigned int ZX_API_CALL  zx_read32(void* addr)
{
#ifndef __BIG_ENDIAN__
    return readl(addr);
#else
    register unsigned int val;
    val = *(volatile unsigned int*)(addr);
    return val;

#endif
}

unsigned short ZX_API_CALL zx_read16(void* addr)
{
#ifndef __BIG_ENDIAN__
   return readw(addr);
#else
    register unsigned short val;
    unsigned long alignedoffset = ((unsigned long)addr/4)*4;
    unsigned long mask = (unsigned long)addr - alignedoffset;

    val = *(volatile unsigned short*)((alignedoffset+(2-mask)));
    return val;
#endif
}

unsigned char ZX_API_CALL zx_read8(void* addr)
{
#ifndef __BIG_ENDIAN__
    return readb((void*)addr);
#else
    register unsigned char val;
    unsigned long alignedoffset = ((unsigned long)addr /4) *4;
    unsigned long mask = (unsigned long)addr - alignedoffset;
    val = *(volatile unsigned char*)((alignedoffset+(3-mask)));

    return val;
#endif
}

void ZX_API_CALL  zx_write32(void* addr, unsigned int val)
{
#ifndef __BIG_ENDIAN__
    writel(val, addr);
#else
    *(volatile unsigned int*)(addr) = val;
#endif
}

void ZX_API_CALL  zx_write16(void* addr, unsigned short val)
{
#ifndef __BIG_ENDIAN__
    writew(val, addr);
#else
    unsigned long alignedoffset = ((unsigned long)addr /4) *4;
    unsigned long mask = (unsigned long)addr - alignedoffset;
    *(volatile unsigned short *)((alignedoffset+(2-mask))) = (val);
#endif
}

void ZX_API_CALL  zx_write8(void* addr, unsigned char val)
{
#ifndef __BIG_ENDIAN__
    writeb(val, addr);
#else
     unsigned long alignedoffset = ((unsigned long)addr /4) *4;
     unsigned long mask = (unsigned long)addr - alignedoffset;
     *(volatile unsigned char *)(alignedoffset+(3-mask)) = (val);
#endif
}

static int zx_get_fcntl_flags(int os_flags)
{
    int o_flags = 0;
    int access  = os_flags & OS_ACCMODE;

    if(access == OS_RDONLY)
    {
        o_flags = O_RDONLY;
    }
    else if(access == OS_WRONLY)
    {
        o_flags = O_WRONLY;
    }
    else if(access == OS_RDWR)
    {
        o_flags = O_RDWR;
    }

    if(os_flags & OS_CREAT)
    {
        o_flags |= O_CREAT;
    }

    if(os_flags & OS_APPEND)
    {
        o_flags |= O_APPEND;
    }

    if (os_flags & OS_LARGEFILE)
    {
        o_flags |= O_LARGEFILE;
    }
    return o_flags;
}

struct os_file *zx_file_open(const char *path, int flags, unsigned short mode)
{
    struct os_file *file    = zx_calloc(sizeof(struct os_file));
    int             o_flags = zx_get_fcntl_flags(flags);

    file->filp = filp_open(path, o_flags, mode);

    if(IS_ERR(file->filp))
    {
        zx_error("open file %s failed %ld.\n", path, PTR_ERR(file->filp));

        zx_free(file);

        file = NULL;
    }

    return file;
}

void zx_file_close(struct os_file *file)
{
    filp_close(file->filp, current->files);

    zx_free(file);
}

int zx_file_read(struct os_file *file, void *buf, unsigned long size, unsigned long long *read_pos)
{
    struct file   *filp  = file->filp;
#if LINUX_VERSION_CODE < KERNEL_VERSION(5, 10, 0)
    mm_segment_t   oldfs = get_fs();
#endif
    loff_t         pos   = 0;
    int            len   = 0;

    if(read_pos)
    {
        pos = *read_pos;
    }

#if LINUX_VERSION_CODE < KERNEL_VERSION(5, 10, 0)
    set_fs(KERNEL_DS);
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(4, 14, 0)
    len = vfs_read(filp, buf, size, &pos);
#else
    len = kernel_read(filp, buf, size, &pos);
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(5, 10, 0)
    set_fs(oldfs);
#endif

    if(read_pos)
    {
        *read_pos = pos;
    }

    return len;
}

int zx_file_write(struct os_file *file, void *buf, unsigned long size)
{
    struct file   *filp  = file->filp;
#if LINUX_VERSION_CODE < KERNEL_VERSION(5, 10, 0)
    mm_segment_t   oldfs = get_fs();
#endif
    int            len   = 0;

#if LINUX_VERSION_CODE < KERNEL_VERSION(5, 10, 0)
    set_fs(KERNEL_DS);
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(4, 14, 0)
    len = vfs_write(filp, buf, size, &filp->f_pos);
#else
    len = kernel_write(filp, buf, size, &filp->f_pos);
#endif

#if LINUX_VERSION_CODE < KERNEL_VERSION(5, 10, 0)
    set_fs(oldfs);
#endif

    return len;
}

/*****************************************************************************/
int ZX_API_CALL zx_vsprintf(char *buf, const char *fmt, ...)
{
    va_list args;
    int ret;

    va_start(args, fmt);

    ret = vsprintf(buf, fmt, args);

    va_end(args);

    return ret;
}

int ZX_API_CALL zx_vsnprintf(char *buf, unsigned long size, const char *fmt, ...)
{
    va_list args;
    int ret;

    va_start(args, fmt);

    ret = vsnprintf(buf, size, fmt, args);

    va_end(args);

    return ret;
}

int ZX_API_CALL zx_sscanf(char *buf, char *fmt, ...)
{
    va_list args;
    int ret;

    va_start(args, fmt);

    ret = vsscanf(buf, fmt, args);

    va_end(args);

    return ret;
}

void ZX_API_CALL zx_cb_printk(const char* msg)
{
    if(msg)
    {
        printk("%s", msg);
    }
}

void ZX_API_CALL zx_cb_vdbgprint(int enable, unsigned int print_level, const char* prefix, const char* msg, ...)
{
    char buffer[256];
    va_list  args;
    unsigned int prefix_len = 0;

    UNUSED(print_level);
    
    if(!enable)
    {
        return;
    }

    if(prefix)
    {
        prefix_len = zx_strlen((char*)prefix);
        zx_strncpy(buffer, prefix, prefix_len);
    }

    va_start(args, msg);

    vsnprintf(buffer+prefix_len, 256-prefix_len, msg, args);

    va_end(args);

    printk("%s", buffer);
}

void ZX_API_CALL zx_printk(unsigned int msglevel, const char* fmt, ...)
{
    char buffer[256];
    va_list marker;

    {
        va_start(marker, fmt);
        vsnprintf(buffer, 256, fmt, marker);
        va_end(marker);

        switch ( msglevel )
        {
        case ZX_DRV_DEBUG:
            printk(KERN_DEBUG"%s debug: %s", STR(DRIVER_NAME), buffer);
        break;
        case ZX_DRV_WARNING:
            printk(KERN_WARNING"%s warning: %s", STR(DRIVER_NAME), buffer);
        break;
        case ZX_DRV_INFO:
            printk(KERN_INFO"%s info: %s", STR(DRIVER_NAME), buffer);
        break;
        case ZX_DRV_ERROR:
            printk(KERN_ERR"%s error: %s", STR(DRIVER_NAME), buffer);
        break;
        case ZX_DRV_EMERG:
            printk(KERN_EMERG"%s emerg: %s", STR(DRIVER_NAME), buffer);
        break;
        default:
            /* invalidate message level */
            zx_assert(0, "");
        break;
        }
    }
}

#define ZX_MAX_KMALLOC_SIZE 32 * 1024

void* zx_malloc_priv(unsigned long size)
{
    void* addr = NULL;

    if(size <= ZX_MAX_KMALLOC_SIZE)
    {
        addr = kmalloc(size, GFP_KERNEL);
    }

    if(addr == NULL)
    {
        addr = vmalloc(size);
    }

    return addr;
}

void* zx_calloc_priv(unsigned long size)
{
    void* addr = zx_malloc_priv(size);
    if(addr != NULL)
    {
        memset(addr, 0, size);
    }
    return addr;
}

void zx_free_priv(void* addr)
{
    unsigned long addr_l = (unsigned long)addr;
    if(addr == NULL)  return;

#ifndef __frv__
    if(addr_l >= VMALLOC_START && addr_l < VMALLOC_END)
    {
        vfree(addr);
    }
    else
    {
        kfree(addr);
    }
#else
    kfree(addr);
#endif

}

/* bit ops */
int zx_find_first_zero_bit(void *buf, unsigned int size)
{
    return find_first_zero_bit(buf, size);
}

int zx_find_next_zero_bit(void *buf, unsigned int size, int offset)
{
    int pos = find_next_zero_bit(buf, size, offset);

    if(pos >= size)
    {
        pos = find_next_zero_bit(buf, size, 0);
    }

    return pos;
}

int zx_find_first_bit(void *buf, unsigned int size)
{
    int pos = find_first_bit(buf, size);
    return pos;
}

int zx_find_next_bit(void *buf, unsigned int size, int offset)
{
    int pos = find_next_bit(buf, size, offset);
    return pos;
}

void zx_set_bit(unsigned int nr, void *buf)
{
   set_bit(nr, buf);
}

void zx_clear_bit(unsigned int nr, void *buf)
{
   clear_bit(nr, buf);
}

int zx_test_bit(unsigned int nr, void *buf)
{
    return test_bit(nr, buf);
}

struct os_atomic *zx_create_atomic(int val)
{
    struct os_atomic *atomic = zx_calloc(sizeof(struct os_atomic));

    if(atomic)
    {
        atomic_set(&atomic->counter, val);
    }

    return atomic;
}

void zx_destroy_atomic(struct os_atomic *atomic)
{
    zx_free(atomic);
}

int zx_atomic_read(struct os_atomic *atomic)
{
    return atomic_read(&atomic->counter);
}

int zx_atomic_inc(struct os_atomic *atomic)
{
    return atomic_inc_return(&atomic->counter);
}

int zx_atomic_dec(struct os_atomic *atomic)
{
    return atomic_dec_return(&atomic->counter);
}

int zx_atomic_cmpxchg(struct os_atomic *atomic, int old, int new)
{
    return atomic_cmpxchg(&atomic->counter, old, new);
}

int zx_atomic_xchg(struct os_atomic *atomic, int v)
{
    return atomic_xchg(&atomic->counter, v);
}

// atomic64
struct os_atomic64 *zx_create_atomic64(long long val)
{
    struct os_atomic64 *atomic = zx_calloc(sizeof(struct os_atomic64));

    if(atomic)
    {
        atomic64_set(&atomic->counter, val);
    }

    return atomic;
}

void zx_destroy_atomic64(struct os_atomic64 *atomic)
{
    zx_free(atomic);
}

long long zx_atomic64_read(struct os_atomic64 *atomic)
{
    return atomic64_read(&atomic->counter);
}

long long zx_atomic64_inc(struct os_atomic64 *atomic)
{
    return atomic64_inc_return(&atomic->counter);
}

long long zx_atomic64_dec(struct os_atomic64 *atomic)
{
    return atomic64_dec_return(&atomic->counter);
}

long long zx_atomic64_cmpxchg(struct os_atomic64 *atomic, long long old, long long new)
{
    return atomic64_cmpxchg(&atomic->counter, old, new);
}

long long zx_atomic64_xchg(struct os_atomic64 *atomic, long long v)
{
    return atomic64_xchg(&atomic->counter, v);
}

struct os_mutex *zx_create_mutex(void)
{
    struct os_mutex *mutex = zx_calloc(sizeof(struct os_mutex));

    if(mutex)
    {
        mutex_init(&mutex->lock);

        mutex->lock_count = 0;
    }
    return mutex;
}

void zx_destroy_mutex(struct os_mutex *mutex)
{
    if(mutex)
    {
        zx_assert(mutex->lock_count == 0, "");

        zx_free(mutex);
    }
}

void zx_mutex_lock_priv(struct os_mutex *mutex)
{
    if (mutex->owner != current)
    {
        mutex_lock(&mutex->lock);
        mutex->owner = current;
        mutex->lock_count = 1;
    }
    else
    {
        ++mutex->lock_count;
    }
}

int zx_mutex_trylock_priv(struct os_mutex *mutex)
{
    int status = ZX_LOCKED;

    if (mutex->owner != current)
    {
        if(!mutex_trylock(&mutex->lock))
        {
            status = ZX_LOCK_FAILED;
        }
        else
        {
            mutex->owner = current;
            mutex->lock_count = 1;
        }
    }
    else
    {
        mutex->lock_count++;
    }

    return status;
}

int zx_mutex_lock_killable(struct os_mutex *mutex)
{
    return mutex_lock_killable(&mutex->lock);
}

#ifdef ZX_MUTEX_TRACK
struct lock_node {
    struct os_mutex *mutex;
    struct lock_node *parent;
    struct list_head link;
    int locked;
    unsigned long long start_lock_time;
    unsigned long long last_lock_time;

    const char *file;
    int line;
};

struct process_node {
    struct task_struct *process;
    unsigned long pid;
    struct list_head link;
    struct lock_node *top;
};


static DEFINE_MUTEX(g_process_lock);
static LIST_HEAD(g_process_list);

#define STATIC_NODES_NUM    4096
static LIST_HEAD(g_process_freelist);
static struct process_node static_free_nodes[STATIC_NODES_NUM] = {0, };
static int static_free_nodes_idx = 0;

#define STATIC_MUTEX_NUM 4096
static LIST_HEAD(g_mutex_freelist);
static struct lock_node static_free_mutexs[STATIC_MUTEX_NUM] = {0, };
static int static_free_mutexs_idx = 0;

static struct lock_node *get_lnode(struct os_mutex *mutex)
{
    struct lock_node *node = list_first_entry_or_null(&g_mutex_freelist, struct lock_node, link);

    if (node)
        list_del(&node->link);

    if (!node && static_free_mutexs_idx < STATIC_MUTEX_NUM)
    {
        node = static_free_mutexs + static_free_mutexs_idx;
        ++static_free_mutexs_idx;
    }
    if (!node)
    {
        node = vmalloc(sizeof(*node));
    }
    node->locked = 0;
    node->start_lock_time = 0;
    node->last_lock_time = 0;
    node->mutex = mutex;
    node->parent = NULL;
    INIT_LIST_HEAD(&node->link);
    return node;
}

static void put_lnode(struct lock_node *node)
{
    if (node >= static_free_mutexs && node < static_free_mutexs + STATIC_MUTEX_NUM)
    {
        list_add_tail(&node->link, &g_mutex_freelist);
    }
    else
    {
        vfree(node);
    }
}

static struct process_node *get_pnode(struct task_struct *process)
{
    struct process_node *node = list_first_entry_or_null(&g_process_freelist, struct process_node, link);

    if (node)
        list_del(&node->link);

    if (!node && static_free_nodes_idx < STATIC_NODES_NUM)
    {
        node = static_free_nodes + static_free_nodes_idx;
        ++static_free_nodes_idx;
    }
    if (!node)
    {
        node = vmalloc(sizeof(*node));
    }
    node->top = NULL;
    node->process = process;
    node->pid = process->pid;
    INIT_LIST_HEAD(&node->link);
    return node;
}

static void put_pnode(struct process_node *node)
{
    if (node >= static_free_nodes && node < static_free_nodes + STATIC_NODES_NUM)
    {
        list_add_tail(&node->link, &g_process_freelist);
    }
    else
    {
        vfree(node);
    }
}

static void link_pnode(struct process_node *node)
{
    list_add_tail(&node->link, &g_process_list);
}

static void unlink_pnode(struct process_node *node)
{
    list_del(&node->link);

    put_pnode(node);
}

static struct process_node *find_pnode(struct task_struct *process)
{
    struct process_node *node = NULL;

    list_for_each_entry(node, &g_process_list, link)
    {
        if (node->process == process && node->pid == process->pid)
        {
            return node;
        }
    }

    return NULL;
}

static void dump_lnode(struct lock_node *lnode)
{
    struct task_struct *owner = lnode->mutex->owner;

    zx_info("  mutex[%p] owner:%ld locked:%d start:%lld during:%dms %s:%d\n",
            lnode->mutex, owner ? owner->pid : 0, lnode->locked, lnode->start_lock_time,
            (lnode->last_lock_time - lnode->start_lock_time)/1000000,
            lnode->file, lnode->line);
}

static void dump_pnode(struct process_node *node)
{
    struct lock_node *lnode = node->top;

    zx_info("process[%p] pid:%ld mutex_list:\n", node->process, node->pid);
    while(lnode)
    {
        dump_lnode(lnode);
        lnode = lnode->parent;
    }
}

void zx_mutex_lock_track(struct os_mutex *mutex, const char *file, int line)
{
    int i = 0;
#define LOCK_TIMEOUT_NS 20000000000ULL  // 20s
    struct process_node *pnode = NULL;
    struct lock_node *lnode = NULL;

    mutex_lock(&g_process_lock);
    {
        pnode = find_pnode(current);
        lnode = get_lnode(mutex);
        if (!pnode)
        {
            pnode = get_pnode(current);
            link_pnode(pnode);
        }

        lnode->locked = 0;
        lnode->file = file;
        lnode->line = line;
        lnode->parent = pnode->top;
        zx_get_nsecs(&lnode->start_lock_time);
        lnode->last_lock_time = lnode->start_lock_time;
        pnode->top = lnode;
    }
    mutex_unlock(&g_process_lock);

    if (mutex->owner != current)
    {
retry:
        if (mutex_trylock(&mutex->lock))
        {
            goto locked;
        }
        else
        {
            if (lnode->last_lock_time - lnode->start_lock_time < LOCK_TIMEOUT_NS)
            {
                schedule_timeout(msecs_to_jiffies(4));
                zx_get_nsecs(&lnode->last_lock_time);
                goto retry;
            }
            else
            {
                zx_error("^^^^^^ %s failed,current=%ld,now=%lld,mutex(%p) %s:%d:.\n",
                        __func__, current->pid, lnode->last_lock_time, mutex, file, line);
                mutex_lock(&g_process_lock);
                {
                    zx_error("^^^^^^ process list:\n");
                    list_for_each_entry(pnode, &g_process_list, link)
                    {
                        dump_pnode(pnode);
                    }
                }
                mutex_unlock(&g_process_lock);

                dump_stack();
                zx_panic();
            }
        }
locked:
        mutex->owner = current;
        mutex->lock_count = 1;
    }
    else
    {
        ++mutex->lock_count;
    }
    lnode->locked = 1;
}


int zx_mutex_trylock_track(struct os_mutex *mutex, const char *file, int line)
{
    int status = ZX_LOCKED;

    if (mutex->owner != current)
    {
        if(!mutex_trylock(&mutex->lock))
        {
            status = ZX_LOCK_FAILED;
        }
        else
        {
            mutex->owner = current;
            mutex->lock_count = 1;
        }
    }
    else
    {
        mutex->lock_count++;
    }

    if (status == ZX_LOCKED)
    {
        mutex_lock(&g_process_lock);
        {
            struct process_node *pnode = find_pnode(current);
            struct lock_node *lnode = get_lnode(mutex);
            if (!pnode)
            {
                pnode = get_pnode(current);
                link_pnode(pnode);
            }
            lnode->parent = pnode->top;
            lnode->file = file;
            lnode->line = line;
            pnode->top = lnode;
        }
        mutex_unlock(&g_process_lock);
    }

   return status;
}
#else
void zx_mutex_lock_track(struct os_mutex *mutex, const char *file, int line)
{
    zx_mutex_lock_priv(mutex);
}

int zx_mutex_trylock_track(struct os_mutex *mutex, const char *file, int line)
{
    return zx_mutex_trylock_priv(mutex);
}
#endif

void zx_mutex_unlock(struct os_mutex *mutex)
{
#ifdef ZX_MUTEX_TRACK
    mutex_lock(&g_process_lock);
    {
        struct process_node *pnode = find_pnode(mutex->owner);
        if (pnode)
        {
            struct lock_node *top = pnode->top;

            if (!top || top->mutex != mutex)
            {
                zx_error("^^^^^^ mutex(%p) pnode->top = %p:\n", mutex, top);
                dump_pnode(pnode);

                dump_stack();
                zx_panic();
            }

            pnode->top = top->parent;
            top->parent = NULL;
            put_lnode(top);

            if (!pnode->top)
            {
                unlink_pnode(pnode);
            }
        }
        else
        {
             zx_error("^^^^^^ mutex(%p) pnode == NULL\n", mutex);
             dump_stack();
             zx_panic();
        }
    }
    mutex_unlock(&g_process_lock);
#endif
    if (--mutex->lock_count == 0)
    {
        mutex->owner = NULL;
        mutex_unlock(&mutex->lock);
    }
}

struct os_sema *zx_create_sema(int val)
{
    struct os_sema *sem = zx_calloc(sizeof(struct os_sema));

    if(sem != NULL)
    {
        sema_init(&sem->lock, val);
    }

    return sem;
}

void zx_destroy_sema(struct os_sema *sem)
{
    zx_free(sem);
}

void zx_down(struct os_sema *sem)
{
    down(&sem->lock);
}

int zx_down_trylock(struct os_sema *sem)
{
    int status = ZX_LOCKED;

    if(down_trylock(&sem->lock))
    {
        status = ZX_LOCK_FAILED;
    }

    return status;
}

void zx_up(struct os_sema *sem)
{
    up(&sem->lock);
}

int zx_read_sema(struct os_sema *sem)
{
    return sem->lock.count;
}

struct os_rwsema *zx_create_rwsema(void)
{
    struct os_rwsema *sem = zx_calloc(sizeof(struct os_rwsema));

    if(sem != NULL)
    {
        init_rwsem(&sem->lock);
    }

    return sem;
}

void zx_destroy_rwsema(struct os_rwsema *sem)
{
    zx_free(sem);
}

void zx_down_read(struct os_rwsema *sem)
{
    down_read(&sem->lock);
}

void zx_down_write(struct os_rwsema *sem)
{
    down_write(&sem->lock);
}

void zx_up_read(struct os_rwsema *sem)
{
    up_read(&sem->lock);
}

void zx_up_write(struct os_rwsema *sem)
{
    up_write(&sem->lock);
}

struct os_spinlock *zx_create_spinlock(void)
{
    struct os_spinlock *spin = zx_calloc(sizeof(struct os_spinlock));

    if(spin != NULL)
    {
        spin_lock_init(&spin->lock);
    }

    return spin;
}

void zx_destroy_spinlock(struct os_spinlock *spin)
{
    if(spin_is_locked(&spin->lock))
    {
        spin_unlock(&spin->lock);
    }

    zx_free(spin);
}

void zx_spin_lock(struct os_spinlock *spin)
{
    spin_lock(&spin->lock);
}

void zx_spin_unlock(struct os_spinlock *spin)
{
    spin_unlock(&spin->lock);
}

void zx_spin_lock_bh(struct os_spinlock *spin)
{
    spin_lock_bh(&spin->lock);
}

void zx_spin_unlock_bh(struct os_spinlock *spin)
{
    spin_unlock_bh(&spin->lock);
}

unsigned long zx_spin_lock_irqsave(struct os_spinlock *spin)
{
    unsigned long flags;

    spin_lock_irqsave(&spin->lock, flags);

    return flags;
}

void zx_spin_unlock_irqrestore(struct os_spinlock *spin, unsigned long flags)
{
    spin_unlock_irqrestore(&spin->lock, flags);
}

struct os_wait_queue* zx_create_wait_queue(void)
{
    struct os_wait_queue *queue = zx_calloc(sizeof(struct os_wait_queue));
    if(queue)
    {
        init_waitqueue_head(&queue->wait_queue);
    }

    return queue;
}

struct os_wait_event* zx_create_event(int task_id)
{
    struct os_wait_event *event = zx_calloc(sizeof(struct os_wait_event));

    if(event)
    {
        atomic_set(&event->state, 0);
        init_waitqueue_head(&event->wait_queue);
    }

    return event;
}

void zx_destroy_event(void *event)
{
    if(event)
    {
        zx_free(event);
    }
}

zx_event_status_t zx_wait_event_thread_safe(struct os_wait_event *event, condition_func_t condition, void *argu, int msec)
{
    zx_event_status_t status = ZX_EVENT_UNKNOWN;

    if(msec)
    {
        long timeout = msecs_to_jiffies(msec);

        timeout = wait_event_timeout(event->wait_queue, (*condition)(argu), timeout);

        if(timeout > 0)
        {
            status = ZX_EVENT_BACK;
        }
        else if(timeout == 0)
        {
            status = ZX_EVENT_TIMEOUT;
        }
        else if(timeout == -ERESTARTSYS)
        {
            status = ZX_EVENT_SIGNAL;
        }
    }
    else
    {
        wait_event(event->wait_queue, (*condition)(argu));
        status = ZX_EVENT_BACK;
    }

    return status;
}

static int zx_generic_condition_func(void *argu)
{
    struct os_wait_event *event = argu;

    return atomic_xchg(&event->state, 0);
}

static int event_condition(void** events, int cnt,  wakeup_event  *wakeup)
{
    struct general_wait_event ** p_events = (struct general_wait_event **)events;
    int                   wake = FALSE;
    int                   i = 0;

    if(cnt > WAIT_EVENT_MAX)
        cnt = WAIT_EVENT_MAX;

    for(i = 0; i < cnt; i++)
    {
        if(p_events[i]->event)
        {
            wake = TRUE;
            break;
        }
    }

    if(wake)
    {
        (*wakeup) = WAIT_EVENT0 + i;
    }
    else
    {
        (*wakeup) = WAIT_EVENT_MAX;
    }
    return wake;
}

zx_event_status_t zx_wait_event(struct os_wait_event *event, int msec)
{
    return zx_wait_event_thread_safe(event, &zx_generic_condition_func, event, msec);
}

void zx_wake_up_event(struct os_wait_event *event)
{
    atomic_set(&event->state, 1);
    wake_up(&event->wait_queue);
}

void* zx_create_thread(zx_thread_func_t func, void *data, const char *thread_name)
{
    struct task_struct* thread;
    struct sched_param param = {.sched_priority = 99};

    thread = (struct task_struct*)kthread_run(func, data, thread_name);
#if DRM_VERSION_CODE < KERNEL_VERSION(5,9,0)
    sched_setscheduler(thread, SCHED_RR, &param);
#else
    sched_set_fifo(thread);
#endif

    return thread;
}

void zx_destroy_thread(void* thread)
{
    if(thread)
    {
        kthread_stop(thread);
    }
}

int zx_thread_should_stop(void)
{
    return kthread_should_stop();
}

void  thread_wait_for_events(struct os_wait_queue *wqueue, struct general_wait_event**  events, int event_cnt, int msec, event_wait_status* ret)
{
    event_wait_status wakeup_status = {0};
    wakeup_event      wake = WAIT_EVENT_MAX;
    unsigned long        flags;

    if(msec)
    {
        long timeout = msecs_to_jiffies(msec);

        timeout = wait_event_interruptible_timeout(wqueue->wait_queue,
                      event_condition((void**)events, event_cnt, &wake) || freezing(current) || kthread_should_stop(),
                      timeout);

        if(timeout > 0)
        {
            if(kthread_should_stop())
            {
                wakeup_status.status = ZX_EVENT_TIMEOUT;
                wakeup_status.event = WAIT_EVENT_MAX;
            }
            else
            {
                if(wake != WAIT_EVENT_MAX)
                {
                    wakeup_status.status = ZX_EVENT_BACK;
                    wakeup_status.event = wake;

                    spin_lock_irqsave(&events[wake]->event_lock->lock, flags);
                    if(events[wake]->need_queue)
                        events[wake]->event --;
                    else
                        events[wake]->event = 0;
                    spin_unlock_irqrestore(&events[wake]->event_lock->lock, flags);
                }
                else  //freezing without any event
                {
                    wakeup_status.status = ZX_EVENT_TIMEOUT;
                    wakeup_status.event = WAIT_EVENT_MAX;
                }
            }
        }
        else if(timeout == 0)
        {
            wakeup_status.status = ZX_EVENT_TIMEOUT;
            wakeup_status.event = WAIT_EVENT_MAX;
        }
        else if(timeout == -ERESTARTSYS)
        {
            wakeup_status.status = ZX_EVENT_SIGNAL;
            wakeup_status.event = WAIT_EVENT_MAX;
        }
    }
    else
    {
        int ret = wait_event_interruptible(wqueue->wait_queue,
                      event_condition((void**)events, event_cnt, &wake) || freezing(current) || kthread_should_stop());

        if(ret == 0)
        {
            if(wake != WAIT_EVENT_MAX)
            {
                wakeup_status.status = ZX_EVENT_BACK;
                wakeup_status.event = wake;

                spin_lock_irqsave(&events[wake]->event_lock->lock, flags);
                if(events[wake]->need_queue)
                    events[wake]->event --;
                else
                    events[wake]->event = 0;
                spin_unlock_irqrestore(&events[wake]->event_lock->lock, flags);
            }
        }
        else if(ret == -ERESTARTSYS)
        {
            wakeup_status.status = ZX_EVENT_SIGNAL;
            wakeup_status.event = WAIT_EVENT_MAX;
        }
    }

    ret->status = wakeup_status.status;
    ret->event = wakeup_status.event;

    return;
}

zx_event_status_t zx_thread_wait(struct os_wait_event *event, int msec)
{
    zx_event_status_t status = ZX_EVENT_UNKNOWN;

    if(msec)
    {
        long timeout = msecs_to_jiffies(msec);

        timeout = wait_event_interruptible_timeout(event->wait_queue,
                      zx_generic_condition_func(event) || freezing(current) || kthread_should_stop(),
                      timeout);

        if(timeout > 0)
        {
            status = ZX_EVENT_BACK;
        }
        else if(timeout == 0)
        {
            status = ZX_EVENT_TIMEOUT;
        }
        else if(timeout == -ERESTARTSYS)
        {
            status = ZX_EVENT_SIGNAL;
        }
    }
    else
    {
        int ret = wait_event_interruptible(event->wait_queue,
                      zx_generic_condition_func(event) || freezing(current) || kthread_should_stop());

        if(ret == 0)
        {
            status = ZX_EVENT_BACK;
        }
        else if(ret == -ERESTARTSYS)
        {
            status = ZX_EVENT_SIGNAL;
        }
    }

    return status;
}


void zx_thread_wake_up(struct os_wait_event *event)
{
    atomic_set(&event->state, 1);
    wake_up_interruptible(&event->wait_queue);
}

void general_thread_wake_up(struct os_wait_queue  *os_queue, struct general_wait_event  *event)
{
    if(event->need_queue)
        event->event++;
    else
        event->event = 1;

    wake_up_interruptible(&os_queue->wait_queue);
}

int  zx_try_to_freeze(void)
{
#ifdef CONFIG_PM_SLEEP
    return try_to_freeze();
#else
    return 0;
#endif
}

int zx_freezable(void)
{
    return !(current->flags & PF_NOFREEZE);
}

void zx_clear_freezable(void)
{
    current->flags |= PF_NOFREEZE;
}

void zx_old_set_freezable(void)
{
    current->flags &= ~PF_NOFREEZE;
}

void zx_set_freezable(void)
{
#ifdef CONFIG_PM_SLEEP
    set_freezable();
#else
    current->flags &= ~PF_NOFREEZE;
#endif
}

int zx_freezing(void)
{
    return freezing(current);
}

unsigned long zx_get_current_pid(void)
{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,26)
    return (unsigned long)current->tgid;
#else
    return (unsigned long)current;
#endif
}

unsigned long zx_get_current_tid(void)
{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2,6,26)
    return current->pid;
#else
    return (unsigned long)current;
#endif
}

void zx_get_current_pname(char *pname, int size)
{
    char buf[TASK_COMM_LEN];

    get_task_comm(buf, current);

    zx_strncpy(pname, buf, size);
}

static void zx_flush_page_cache(struct page *pages, unsigned int flush_size)
{
#ifdef __mips64__
    int  page_count = flush_size/PAGE_SIZE;
    void *ptr;

    while(page_count--)
    {
        ptr  = kmap(pages);
        //dma_cache_wback_inv
        dma_cache_sync(NULL, ptr, PAGE_SIZE, DMA_BIDIRECTIONAL);
        kunmap(pages);
        pages++;
    }
#elif !defined(CONFIG_X86)
    int  page_count = flush_size/PAGE_SIZE;
    void *ptr;

    while(page_count--)
    {
        ptr  = kmap(pages);
#if __ARM_ARCH >= 8
        flush_icache_range((unsigned long)ptr, (unsigned long)(ptr+PAGE_SIZE));
#else
        dmac_flush_range(ptr, ptr + PAGE_SIZE);
        outer_flush_range(page_to_phys(pages), page_to_phys(pages) + PAGE_SIZE);
#endif
        kunmap(pages);
        pages++;
    }
#else
    int  page_count = flush_size/PAGE_SIZE;
    unsigned long ptr;
    pte_t *pte;
    unsigned int level;
    while(page_count--)
    {
        if(PageHighMem(pages))
            ptr = (unsigned long)kmap(pages);
        else
            ptr = (unsigned long)page_address(pages);

        pte  = lookup_address(ptr,&level);

        if(pte&& (pte_val(*pte)& _PAGE_PRESENT))
        {
           clflush_cache_range((void*)ptr,PAGE_SIZE);
        }

        if(PageHighMem(pages))
            kunmap(pages);

        pages++;
   }

#endif
}
static void zx_free_st_internal(struct device *dev, struct sg_table *st, bool userptr, bool need_dma_unmap)
{
    struct scatterlist *sg;
    struct sg_page_iter sg_iter;

    if (need_dma_unmap)
    {
        dma_unmap_sg(dev, st->sgl, st->nents, DMA_BIDIRECTIONAL);
    }
    if (userptr)
    {
        for_each_sg_page(st->sgl, &sg_iter, st->nents, 0)
        {
            struct page *page = sg_page_iter_page(&sg_iter);
            put_page(page);
        }
    }
    else
    {
        for (sg = st->sgl; sg; sg = sg_next(sg))
        {
            __free_pages(sg_page(sg), get_order(sg->length));
        }
    }

    sg_free_table(st);
    zx_free(st);
}

void zx_free_pages_memory_priv(void *pdev,struct os_pages_memory *memory)
{
    int i;
    int page_cnt;
    int pg_arr_index = 0;
    struct pci_dev *pci_dev = pdev;

    if(memory == NULL)
    {
        return;
    }
    page_cnt = memory->size / PAGE_SIZE;

#ifdef CONFIG_X86_PAT
#ifdef HAS_SET_PAGES_ARRAY_WC
    for (i = 0; i < page_cnt; i++) {
        if (memory->cache_type_per_page[i] != ZX_MEM_WRITE_BACK) {
            memory->pages_temp[pg_arr_index++] = memory->pages[i];
        }
    }
    set_pages_array_wb(memory->pages_temp, pg_arr_index);
#else
    for (i = 0; i < page_cnt; i++) {
        if (memory->cache_type_per_page[i] != ZX_MEM_WRITE_BACK) {
            struct page *pg = memory->pages[i];
            set_memory_wb((unsigned long)page_address(pg), 1);
        }
    }
#endif
#endif

    /* shared means this pages was import from other, shouldn't free!! */
    if (!memory->shared) {
        zx_free_st_internal(&pci_dev->dev, memory->st, memory->userptr, memory->has_dma_map);
    }

    mutex_lock(&g_pages_memory_lock);
    list_del(&memory->link);
    mutex_unlock(&g_pages_memory_lock);

    zx_free(memory);
}

struct os_pages_memory* zx_allocate_pages_memory_struct(int page_cnt, struct sg_table *st)
{
    int ret, i;
    int extra_size  = sizeof(struct page*) * page_cnt * 2 + sizeof(char) * page_cnt;
    struct os_pages_memory *memory = zx_calloc(sizeof(struct os_pages_memory) + extra_size);

    if (memory)
    {
        memory->pages = (void*)(memory + 1);
        memory->pages_temp = memory->pages + page_cnt;
        memory->cache_type_per_page = (char*)(memory->pages_temp + page_cnt);
        if (st)
        {
            memory->st = st;
        }
        else
        {
            memory->st  = zx_calloc(sizeof(*memory->st));
            ret = sg_alloc_table(memory->st, page_cnt, GFP_KERNEL);
            if (ret < 0)
                goto sg_alloc_failed;
        }
        #ifdef CONFIG_X86_PAT
        if (ZX_MEM_WRITE_BACK != 0) {
            for(i = 0; i < page_cnt; i++)
                memory->cache_type_per_page[i] = ZX_MEM_WRITE_BACK;
        }
        #endif
    }
    return memory;
sg_alloc_failed:
    zx_free(memory);
    return NULL;
}

void zx_pages_memory_extract_st(struct os_pages_memory *memory)
{
    int i = 0;
    struct sg_page_iter sg_iter;

    for_each_sg_page(memory->st->sgl, &sg_iter, memory->st->nents, 0)
    {
        struct page *page = sg_page_iter_page(&sg_iter);
        memory->pages[i] = page;
        i++;
    }
}

// 2^order <= size
static int zx_get_alloc_order(int size)
{
#define MAX_ALLOC_ORDER     9
    int order = get_order(size);

    if ((((1 << (order)) << PAGE_SHIFT) > size) && order)
       order--;

    return order > MAX_ALLOC_ORDER ? MAX_ALLOC_ORDER : order;
}


struct os_pages_memory *zx_allocate_pages_memory_priv(void *pdev, unsigned long long size, int page_size, alloc_pages_flags_t alloc_flags)
{
    int ret;
    unsigned long long rest_size;
    int order;
    int page_cnt = PAGE_ALIGN(size) / PAGE_SIZE;
    struct page *page;
    struct os_pages_memory *memory = NULL;
    struct scatterlist *sg = NULL, *end_sg = NULL;
    gfp_t  gfp_mask = GFP_KERNEL;
    struct pci_dev *pci_dev = pdev;
    struct page **pages = NULL;
    int  *orders = NULL;
    struct sg_table *st = NULL;
    int n_pages = 0;
    int i;

    if(size <= 0)
    {
        return NULL;
    }

    pages = zx_calloc(page_cnt * sizeof(struct page *));
    orders = zx_calloc(page_cnt * sizeof(int));
    st = zx_calloc(sizeof(struct sg_table));

    if(!pages || !orders || !st)
    {
        goto alloc_pages_failed;
    }

    if (alloc_flags.need_zero)
    {
        gfp_mask |= __GFP_ZERO;
    }
    if (alloc_flags.dma32 && sizeof(dma_addr_t) == 8)
    {
        gfp_mask |= GFP_DMA32;
    }
    else
    {
#ifdef HAS_SET_PAGES_ARRAY_WC
        gfp_mask |= __GFP_HIGHMEM;
#endif
    }

    rest_size = PAGE_ALIGN(size);
    order = zx_get_alloc_order(rest_size);
    while(rest_size > 0)
    {
        page = alloc_pages(gfp_mask | (order ? (__GFP_NORETRY | __GFP_NOWARN) : 0), order);
        if (page == NULL)
        {
            if (order == 0)
                goto alloc_pages_failed;
            else
                order--;
        }
        else
        {
            orders[n_pages] = order;
            pages[n_pages++] = page;

            if (alloc_flags.need_flush)
                zx_flush_page_cache(page, PAGE_SIZE << order);
            rest_size -= (PAGE_SIZE << order);

            if (rest_size > 0)
            {
                order = min(zx_get_alloc_order(rest_size), order);
            }
        }
    }

    if(sg_alloc_table(st, n_pages, GFP_KERNEL))
    {
        goto alloc_pages_failed;
    }

    for_each_sg(st->sgl, sg, st->orig_nents, i)
    {
        sg_set_page(sg, pages[i], PAGE_SIZE << orders[i], 0);
        end_sg = sg;
    }

    sg_mark_end(end_sg);

    memory = zx_allocate_pages_memory_struct(page_cnt, st);
    zx_assert(memory != NULL, "");
    zx_assert(alloc_flags.fixed_page == TRUE, "");
    zx_assert(page_size == PAGE_SIZE, "only support os page size for page mem");

    memory->shared      = FALSE;
    memory->size        = PAGE_ALIGN(size);
    memory->fixed_page  = alloc_flags.fixed_page;
    memory->page_size   = memory->fixed_page ? PAGE_ALIGN(page_size) : 0;
    memory->dma32       = alloc_flags.dma32;
    memory->need_zero   = alloc_flags.need_zero;

    sg = memory->st->sgl;

    zx_pages_memory_extract_st(memory);

    if (alloc_flags.need_dma_map)
    {
        memory->st->nents = dma_map_sg(&pci_dev->dev, memory->st->sgl, memory->st->orig_nents, DMA_BIDIRECTIONAL);
        if (!memory->st->nents)
        {
            goto alloc_pages_failed;
        }
        memory->has_dma_map = 1;
        if (0)
        {
            zx_info("alloc[%p] st:%p pages:%d size:0x%x nents:%d orig_nents:%d\n",
                memory, memory->st, page_cnt, memory->size, memory->st->nents, memory->st->orig_nents);
            for_each_sg(memory->st->sgl, sg, memory->st->nents, i) {
                zx_info("  [%p] sg:%p i:%d pgcnt:%d off:%x length:%x dmalen:%x dma:[%llx ~ %llx] phys:%llx\n",
                    memory, sg, i, PAGE_ALIGN(sg->offset + sg_dma_len(sg)) >> PAGE_SHIFT, sg->offset,
                    sg->length, sg_dma_len(sg), sg_dma_address(sg), sg_dma_address(sg) + sg_dma_len(sg), sg_phys(sg));
            }
        }
    }

    zx_free(pages);
    zx_free(orders);

    mutex_lock(&g_pages_memory_lock);
    list_add_tail(&memory->link, &g_pages_memory_list);
    mutex_unlock(&g_pages_memory_lock);
    return memory;
alloc_pages_failed:
    zx_error("%s: alloc page failed, size:0x%x\n", __func__, size);

    if(pages)
    {
        zx_free(pages);
    }

    if(orders)
    {
        zx_free(orders);
    }

    if(st)
    {
        zx_free_st_internal(&pci_dev->dev, st, FALSE, memory ? memory->has_dma_map : FALSE);
    }

    zx_free(memory);
    return NULL;
}

unsigned long long zx_get_page_phys_address(struct os_pages_memory *memory, int page_num, int *page_size)
{
    int i;
    int page_start = 0;
    struct scatterlist *sg;
    unsigned long long phys_addr = 0;

    for_each_sg(memory->st->sgl, sg, memory->st->nents, i)
    {
        unsigned int length = memory->has_dma_map ? sg_dma_len(sg) : sg->length;
        int page_cnt = PAGE_ALIGN(sg->offset + length) >> PAGE_SHIFT;

        if((page_start + page_cnt -1) >= page_num)//page_num start from 0
        {
             phys_addr   = memory->has_dma_map ? sg_dma_address(sg) : sg_phys(sg);
             phys_addr += ((page_num - page_start) << PAGE_SHIFT);
             break;
        }
        page_start += page_cnt;
    }

    return phys_addr;
}

void zx_pages_memory_for_each_continues(struct os_pages_memory *memory, void *arg,
    int (*cb)(void* arg, int page_start, int page_cnt, unsigned long long dma_addr))
{
    int i;
    int page_start = 0;
    struct scatterlist *sg;

    for_each_sg(memory->st->sgl, sg, memory->st->nents, i)
    {
        unsigned int length = memory->has_dma_map ? sg_dma_len(sg) : sg->length;
        int page_cnt = PAGE_ALIGN(sg->offset + length) >> PAGE_SHIFT;

        if (0 != cb(arg, page_start, page_cnt, memory->has_dma_map ? sg_dma_address(sg) : sg_phys(sg))) {
            break;
        }
        page_start += page_cnt;
    }
}

void zx_for_each_pages_memory(int (*cb)(struct os_pages_memory*, unsigned int, void *), void *arg)
{
    struct os_pages_memory *memory;

    mutex_lock(&g_pages_memory_lock);
    list_for_each_entry(memory, &g_pages_memory_list, link)
    {
        if (0 != cb(memory, memory->size, arg))
            break;
    }
    mutex_unlock(&g_pages_memory_lock);
}

static struct page** zx_acquire_os_pages(struct os_pages_memory *memory, unsigned int size, int m_offset, int *pg_cnt)
{
    unsigned int mapped_size = PAGE_ALIGN(size);
    int pages_cnt   = mapped_size / PAGE_SIZE;
    int offset      = _ALIGN_DOWN(m_offset, PAGE_SIZE);

    if (pg_cnt)
        *pg_cnt = pages_cnt;

    return memory->pages + (offset / PAGE_SIZE);
}

static void zx_release_os_pages(struct page **pages)
{
}

unsigned char zx_validate_page_cache(struct os_pages_memory *memory, int start_page, int end_page, unsigned char request_cache_type)
{
    int i;
    int pg_arr_index = 0;
#ifdef CONFIG_X86_PAT
#ifdef HAS_SET_PAGES_ARRAY_WC
    for (i = start_page; i < end_page; i++)
    {
        if (memory->cache_type_per_page[i] == ZX_MEM_WRITE_BACK && request_cache_type != ZX_MEM_WRITE_BACK)
        {
            memory->pages_temp[pg_arr_index++] = memory->pages[i];
            memory->cache_type_per_page[i] = request_cache_type;
        }
    }
    if (pg_arr_index > 0)
    {
        if(request_cache_type == ZX_MEM_WRITE_COMBINED)
        {
           set_pages_array_wc(memory->pages_temp, pg_arr_index);
        }
        else if(request_cache_type == ZX_MEM_UNCACHED)
        {
           set_pages_array_uc(memory->pages_temp, pg_arr_index);
        }
    }
#else
    for (i = start_page; i < end_page; i++)
    {
        if (memory->cache_type_per_page[i] == ZX_MEM_WRITE_BACK && request_cache_type != ZX_MEM_WRITE_BACK)
        {
            struct page* pg = memory->pages[i];
            if(request_cache_type == ZX_MEM_WRITE_COMBINED)
            {
               set_memory_wc((unsigned long)page_address(pg), 1);
            }
            else if(request_cache_type == ZX_MEM_UNCACHED)
            {
               set_memory_uc((unsigned long)page_address(pg), 1);
            }
            memory->cache_type_per_page[i] = request_cache_type;
        }
    }
#endif
#endif

#if defined(__aarch64__)
   //only map arm64 to write back because cache flush code is not ready. wb is safe for snoop path.
   //now only enable snoop path in arm64.
    return ZX_MEM_WRITE_BACK;
//   map_argu->flags.cache_type = ZX_MEM_WRITE_COMBINED;
#endif

#if defined(__mips64__)
   //force to ZX_MEM_WRITE_BACK, but acqually prot will still be PAGE_KERNEL
   //can not be other type, which will change prot to uncached and mips will random hang or blackscreen
    return ZX_MEM_WRITE_BACK;
#endif

    return request_cache_type;
}
zx_cpu_vm_area_t *zx_map_pages_memory_priv(zx_map_argu_t *map_argu)
{
    struct os_pages_memory *memory  = map_argu->memory;
    zx_cpu_vm_area_t      *vm_area = zx_calloc(sizeof(zx_cpu_vm_area_t));
    int start                       = _ALIGN_DOWN(map_argu->offset, PAGE_SIZE) / PAGE_SIZE;
    int end                         = start + PAGE_ALIGN(map_argu->size) / PAGE_SIZE;
    struct file * fp = NULL;

    pgprot_t     prot       = PAGE_KERNEL;
    unsigned int cache_type = 0;
    int          page_cnt   = 0;
    struct page  **pages;

    map_argu->flags.cache_type = zx_validate_page_cache(memory, start, end, map_argu->flags.cache_type);
    cache_type = map_argu->flags.cache_type;

    prot  = os_get_pgprot_val(&cache_type, prot, 0);

    pages = zx_acquire_os_pages(memory, map_argu->size, map_argu->offset, &page_cnt);

    vm_area->flags.value = map_argu->flags.value;
    vm_area->size        = map_argu->size;
    vm_area->node_num    = 1;
    vm_area->virt_addr   = vmap(pages, page_cnt, VM_MAP, prot);

    zx_release_os_pages(pages);

    return vm_area;
}

void zx_unmap_pages_memory_priv(zx_cpu_vm_area_t *vm_area)
{
    vunmap(vm_area->virt_addr);

    zx_free(vm_area);
}


void zx_flush_cache(zx_cpu_vm_area_t *vma, struct os_pages_memory* memory, unsigned int offset, unsigned int size)
{
#ifdef CONFIG_X86
    struct page **acquired_pages;
    struct page *pages;
    unsigned long ptr;
    pte_t *pte;
    int page_start = offset/PAGE_SIZE;
    int page_end   = (offset + size -1)/PAGE_SIZE;
    int page_start_offset = offset%PAGE_SIZE;
    int page_end_offset   = (offset + size -1)%PAGE_SIZE;
    int i;
    int page_count;
    int level;


    if((offset % 32 != 0) || (size % 32 != 0) )
    {
        zx_error("offset does not align to data cache line boundary(32B)");
    }

    zx_assert(offset>=0, "");
    zx_assert(size>0, "");
    zx_assert(offset+size <= memory->size, "offset:%d, size:%d, memory->size:%d", offset, size, memory->size);
    zx_assert(page_start <= page_end, "page_start:%d, page_end:%d", page_start, page_end);

    acquired_pages = zx_acquire_os_pages(memory, PAGE_SIZE*(page_end-page_start+1), 0, &page_count);
    if(page_count != (page_end-page_start+1))
    {
        zx_error("page count unequal, need to check!");
    }

    if(page_start == page_end)
    {
        pages = acquired_pages[0];
        ptr  = (unsigned long)page_address(pages);
        pte  = lookup_address(ptr,&level);
        if(pte&& (pte_val(*pte)& _PAGE_PRESENT))
        {
            clflush_cache_range((void*)ptr+page_start_offset,page_end_offset-page_start_offset);
        }

    }
    else
    {
        pages = acquired_pages[0];

        ptr  = (unsigned long)page_address(pages);
        pte  = lookup_address(ptr,&level);
        if(pte&& (pte_val(*pte)& _PAGE_PRESENT))
        {
            clflush_cache_range((void*)ptr+page_start_offset,PAGE_SIZE);
        }

        pages = acquired_pages[page_end-page_start];

        ptr  = (unsigned long)page_address(pages);
        pte  = lookup_address(ptr,&level);
        if(pte&& (pte_val(*pte)& _PAGE_PRESENT))
        {
            clflush_cache_range((void*)ptr,page_end_offset);
        }
    }

    for(i = page_start+1 ; i <= page_end-1; i++)
    {
        pages = acquired_pages[i-page_start];

        ptr  = (unsigned long)page_address(pages);
        pte  = lookup_address(ptr,&level);
        if(pte&& (pte_val(*pte)& _PAGE_PRESENT))
        {
            clflush_cache_range((void*)ptr,PAGE_SIZE);
        }
    }
    zx_release_os_pages(acquired_pages);

#else
    struct page **acquired_pages;
    struct page *pages;
    void *ptr;
    int page_start = offset/PAGE_SIZE;
    int page_end   = (offset + size -1)/PAGE_SIZE;
    int page_start_offset = offset%PAGE_SIZE;
    int page_end_offset   = (offset + size -1)%PAGE_SIZE;
    int i;
    int page_count;


    if((offset % 32 != 0) || (size % 32 != 0) )
    {
        zx_error("offset does not align to data cache line boundary(32B)");
    }

    zx_assert(offset>=0, "");
    zx_assert(size>0, "");
    zx_assert(offset+size <= memory->size, "offset:%d, size:%d, memory->size:%d", offset, size, memory->size);
    zx_assert(page_start <= page_end, "page_start:%d, page_end:%d", page_start, page_end);

    acquired_pages = zx_acquire_os_pages(memory, PAGE_SIZE*(page_end-page_start+1), 0, &page_count);
    if(page_count != (page_end-page_start+1))
    {
        zx_error("page count unequal, need to check!");
    }

    if(page_start == page_end)
    {
        pages = acquired_pages[0];
        ptr  = kmap(pages);
#ifdef __mips64__
        dma_cache_sync(NULL, ptr + page_start_offset, page_end_offset - page_start_offset, DMA_BIDIRECTIONAL);
#elif __ARM_ARCH >= 8
        flush_icache_range((unsigned long)(ptr + page_start_offset), (unsigned long)(ptr + page_end_offset));
#else
        dmac_flush_range(ptr + page_start_offset, ptr + page_end_offset);
        outer_flush_range(page_to_phys(pages) + page_start_offset, page_to_phys(pages) + page_end_offset);
#endif
        kunmap(pages);
    }
    else
    {
        pages = acquired_pages[0];
        ptr  = kmap(pages);
#ifdef __mips64__
        dma_cache_sync(NULL, ptr + page_start_offset, PAGE_SIZE - page_start_offset, DMA_BIDIRECTIONAL);
#else
        dmac_flush_range(ptr + page_start_offset, ptr + PAGE_SIZE);
        outer_flush_range(page_to_phys(pages) + page_start_offset, page_to_phys(pages) + PAGE_SIZE);
#endif
        kunmap(pages);

        pages = acquired_pages[page_end-page_start];
        ptr  = kmap(pages);
#ifdef __mips64__
        dma_cache_sync(NULL, ptr, page_end_offset, DMA_BIDIRECTIONAL);
#elif __ARM_ARCH >= 8
        flush_icache_range((unsigned long)(ptr + page_start_offset), (unsigned long)(ptr + PAGE_SIZE));
#else
        dmac_flush_range(ptr, ptr + PAGE_SIZE);
        outer_flush_range(page_to_phys(pages), page_to_phys(pages) + page_end_offset);
#endif
        kunmap(pages);
    }

    for(i = page_start+1 ; i <= page_end-1; i++)
    {
        pages = acquired_pages[i-page_start];
        ptr  = kmap(pages);
#ifdef __mips64__
        dma_cache_sync(NULL, ptr, PAGE_SIZE, DMA_BIDIRECTIONAL);
#elif  __ARM_ARCH >= 8
        flush_icache_range((unsigned long)ptr, (unsigned long)(ptr + PAGE_SIZE));
#else
        dmac_flush_range(ptr, ptr + PAGE_SIZE);
        outer_flush_range(page_to_phys(pages), page_to_phys(pages) + PAGE_SIZE);
#endif
        kunmap(pages);
    }
    zx_release_os_pages(acquired_pages);
#endif
}

void zx_inv_cache(zx_cpu_vm_area_t *vma, struct os_pages_memory* memory, unsigned int offset, unsigned int size)
{
#ifdef CONFIG_X86
    //clflush_cache_range(start, size);
    wbinvd();
#else
    struct page **acquired_pages;
    struct page *pages;
    void *ptr;
    int page_start = offset/PAGE_SIZE;
    int page_end   = (offset + size -1)/PAGE_SIZE;
    int page_start_offset = offset%PAGE_SIZE;
    int page_end_offset   = (offset + size - 1)%PAGE_SIZE;
    int i;

    if((offset % 32 != 0) || (size % 32 != 0) )
    {
        zx_error("offset does not align to data cache line boundary(32B)");
    }

    zx_assert(offset>=0, "");
    zx_assert(size>0, "");
    zx_assert(offset+size <= memory->size, "offset:%d, size:%d, memory->size:%d", offset, size, memory->size);
    zx_assert(page_start <= page_end, "page_start:%d, page_end:%d", page_start, page_end);

    acquired_pages = zx_acquire_os_pages(memory, PAGE_SIZE*(page_end-page_start+1), 0, NULL);
    if(page_start == page_end)
    {
        pages = acquired_pages[0];
        ptr  = kmap(pages);
#ifdef __mips64__
        dma_cache_sync(NULL, ptr + page_start_offset, page_end_offset - page_start_offset, DMA_BIDIRECTIONAL);
#elif __ARM_ARCH >= 8
        flush_icache_range((unsigned long)(ptr + page_start_offset), (unsigned long)(ptr + page_end_offset));
#else
        dmac_flush_range(ptr + page_start_offset, ptr + page_end_offset);
        outer_inv_range(page_to_phys(pages) + page_start_offset, page_to_phys(pages) + page_end_offset);
#endif
        kunmap(pages);
    }
    else
    {
        pages = acquired_pages[0];
        ptr  = kmap(pages);
#ifdef __mips64__
        dma_cache_sync(NULL, ptr + page_start_offset, PAGE_SIZE - page_start_offset, DMA_BIDIRECTIONAL);
#elif __ARM_ARCH >= 8
        flush_icache_range((unsigned long)(ptr + page_start_offset), (unsigned long)(ptr + PAGE_SIZE));
#else
        dmac_flush_range(ptr + page_start_offset, ptr + PAGE_SIZE);
        outer_inv_range(page_to_phys(pages) + page_start_offset, page_to_phys(pages) + PAGE_SIZE);
#endif
        kunmap(pages);

        pages = acquired_pages[page_end-page_start];
        ptr  = kmap(pages);
#ifdef __mips64__
        dma_cache_sync(NULL, ptr, page_end_offset, DMA_BIDIRECTIONAL);
#elif __ARM_ARCH >= 8
        flush_icache_range((unsigned long)ptr, (unsigned long)(ptr + PAGE_SIZE));
#else
        dmac_flush_range(ptr, ptr + PAGE_SIZE);
        outer_inv_range(page_to_phys(pages), page_to_phys(pages) + page_end_offset);
#endif
        kunmap(pages);
    }

    for(i = page_start+1 ; i <= page_end-1; i++)
    {
        pages = acquired_pages[i-page_start];
        ptr  = kmap(pages);
#ifdef __mips64__
        dma_cache_sync(NULL, ptr, PAGE_SIZE, DMA_BIDIRECTIONAL);
#elif __ARM_ARCH >= 8
        flush_icache_range((unsigned long)ptr, (unsigned long)(ptr + PAGE_SIZE));
#else
        dmac_flush_range(ptr, ptr + PAGE_SIZE);
        outer_inv_range(page_to_phys(pages), page_to_phys(pages) + PAGE_SIZE);
#endif
        kunmap(pages);
    }
    zx_release_os_pages(acquired_pages);
#endif
}

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,6,0)
#define zx_ioremap_nocache  ioremap
#else
#define zx_ioremap_nocache  ioremap_nocache
#endif

zx_cpu_vm_area_t *zx_map_io_memory_priv(zx_map_argu_t *map_argu)
{
    zx_cpu_vm_area_t *vm_area   = zx_calloc(sizeof(zx_cpu_vm_area_t));
    void          *virt_addr = NULL;
    int i = 0;
    vm_area->node_num = map_argu->node_num;
    if(vm_area->node_num > 1){
        vm_area->virt_addrs = zx_calloc(sizeof(void*) * vm_area->node_num);
        vm_area->node_sizes = zx_calloc(sizeof(unsigned int) * vm_area->node_num);
    }

#if defined(__aarch64__)
    //current on pass uc in phyton platform.need to be think over.
    map_argu->flags.cache_type = ZX_MEM_UNCACHED;
#endif

    if(map_argu->flags.cache_type == ZX_MEM_WRITE_COMBINED)
    {
#if defined(CONFIG_X86) && !defined(CONFIG_X86_PAT)
       if(vm_area->node_num == 1){
            vm_area->virt_addr = zx_ioremap_nocache(map_argu->phys_addr, map_argu->size);

       }else{
            for(i=0; i<vm_area->node_num; i++){
                vm_area->virt_addrs[i] = zx_ioremap_nocache(map_argu->phys_addrs[i], map_argu->node_sizes[i]);
                vm_area->node_sizes[i] = map_argu->node_sizes[i];
            }
       }
       map_argu->flags.cache_type = ZX_MEM_UNCACHED;
#else
       if(zx_modparams.zx_hang_dump)
       {
           if(vm_area->node_num == 1){
                vm_area->virt_addr = zx_ioremap_nocache(map_argu->phys_addr, map_argu->size);
           }else{
                for(i=0; i<vm_area->node_num; i++){
                    vm_area->virt_addrs[i] = zx_ioremap_nocache(map_argu->phys_addrs[i], map_argu->node_sizes[i]);
                    vm_area->node_sizes[i] = map_argu->node_sizes[i];
                }
           }
       }
       else
       {
           if(vm_area->node_num == 1){
                vm_area->virt_addr = ioremap_wc(map_argu->phys_addr, map_argu->size);
           }else{
                for(i=0; i<vm_area->node_num; i++){
                    vm_area->virt_addrs[i] = ioremap_wc(map_argu->phys_addrs[i], map_argu->node_sizes[i]);
                    vm_area->node_sizes[i] = map_argu->node_sizes[i];
                }
           }
       }
#endif
    }
    else//UNCACHED/CACHED
    {
        if(vm_area->node_num == 1){
            vm_area->virt_addr = zx_ioremap_nocache(map_argu->phys_addr, map_argu->size);
        }else{
            for(i=0; i<vm_area->node_num; i++){
                vm_area->virt_addrs[i] = zx_ioremap_nocache(map_argu->phys_addrs[i], map_argu->node_sizes[i]);
                vm_area->node_sizes[i] = map_argu->node_sizes[i];
            }
        }
    }

    vm_area->flags.value = map_argu->flags.value;
    vm_area->size        = map_argu->size;
    vm_area->owner       = zx_get_current_pid();

    return vm_area;
}

void zx_unmap_io_memory_priv(zx_cpu_vm_area_t *vm_area)
{
    int i = 0;
    if(vm_area->node_num > 1){
        for(i=0; i<vm_area->node_num; i++)
            iounmap(vm_area->virt_addrs[i]);

        zx_free(vm_area->virt_addrs);
        zx_free(vm_area->node_sizes);
    }else
        iounmap(vm_area->virt_addr);

    zx_free(vm_area);
}

void *zx_ioremap(unsigned long long io_base, unsigned long long size)
{
#if LINUX_VERSION_CODE >= KERNEL_VERSION(5,6,0)
    return ioremap(io_base, size);
#else
    return ioremap_nocache(io_base, size);
#endif
}

void zx_iounmap(void *map_address)
{
    iounmap(map_address);
}

int zx_mtrr_add(unsigned long start, unsigned long size)
{
    int reg = -1;

#ifdef CONFIG_MTRR
#if LINUX_VERSION_CODE < KERNEL_VERSION(4,3,0)
    reg = mtrr_add(start, size, MTRR_TYPE_WRCOMB, 1);
#else
    reg = arch_phys_wc_add(start, size);
#endif

    if(reg < 0)
    {
         zx_info("set mtrr range %x -> %x failed. \n", start, start + size);
    }
#endif

    return reg;
}

int zx_mtrr_del(int reg, unsigned long base, unsigned long size)
{
    int err = -1;

#ifdef CONFIG_MTRR
#if LINUX_VERSION_CODE < KERNEL_VERSION(4,3,0)
    err =  mtrr_del(reg, base, size);
#else
    /* avoid build warning */
    arch_phys_wc_del(reg);
    err = 0;
#endif

#endif

    return err;
}

int zx_get_mem_info(mem_info_t *mem)
{
    struct sysinfo si;

    si_meminfo(&si);

    /* we need set pages cache type accord usage, before set_pages_array_wc defined
     * kernel only support set cache type to normal zone pages, add check if can use hight
     */

#ifdef HAS_SET_PAGES_ARRAY_WC
    mem->totalram = si.totalram;
    mem->freeram  = si.freeram;
#else
    mem->totalram = si.totalram - si.totalhigh;
    mem->freeram  = si.freeram  - si.freehigh;
#endif

    return 0;
}

int zx_query_platform_caps(void *pdev, platform_caps_t *caps)
{
    struct sysinfo si;
    struct device* device = &((struct pci_dev *)pdev)->dev;

    si_meminfo(&si);

#if defined(__i386__) || defined(__x86_64__)
    caps->dcache_type = ZX_CPU_CACHE_PIPT;
#elif defined(CONFIG_CPU_CACHE_VIPT)
    caps->dcache_type = cache_is_vipt_nonaliasing() ?
                        ZX_CPU_CACHE_VIPT_NONALIASING :
                        ZX_CPU_CACHE_VIPT_ALIASING;
#elif defined(CONFIG_CPU_CACHE_VIVT)
    caps->dcache_type = ZX_CPU_CACHE_VIVT;
#else
    caps->dcache_type = ZX_CPU_CACHE_UNKNOWN;
#endif

#if DRM_VERSION_CODE < KERNEL_VERSION(6,0,0)
    if (intel_iommu_enabled)
#else
    if (device_iommu_mapped(device))
#endif
    {
        caps->iommu_support = TRUE;
    }
    else
    {
        caps->iommu_support = FALSE;
    }

    caps->page_size  = PAGE_SIZE;
    caps->page_shift = PAGE_SHIFT;

    if (BITS_PER_LONG == 64 && !caps->iommu_support && (si.totalram > 64 * 1024 * 1024 * 1024ULL / caps->page_size))
    {
         caps->system_need_dma32 = TRUE;
    }
#ifdef __aarch64__
#ifdef is_ft1500a
    caps->is_ft1500a = is_ft1500a();
#else
    if (read_cpuid_part_number() == PHYTIUM_CPU_PART_1500A)
    {
        caps->is_ft1500a = 1;
    }
    else
    {
        caps->is_ft1500a = 0;
    }
#endif
#endif
    caps->system_need_dma32 = caps->iommu_support ? FALSE : dma_get_required_mask(device) > DMA_BIT_MASK(36);
    return 0;
}

void *zx_pages_memory_swapout(struct os_pages_memory *pages_memory)
{
    struct file          *file_storage = shmem_file_setup("zx gmem", pages_memory->size, 0);
    struct address_space *file_addr_space;
    struct page          **pages;
    struct page          *src_page, *dst_page;
    void                 *src_addr, *dst_addr;
    int i = 0, j = 0, page_count = 0;

    if(file_storage == NULL)
    {
        zx_info("create shmem file failed. size: %dk.\n", pages_memory->size << 10);

        return NULL;
    }

    file_addr_space = file_storage->f_path.dentry->d_inode->i_mapping;

    pages = zx_acquire_os_pages(pages_memory, pages_memory->size, 0, &page_count);
    if(pages == NULL) return NULL;

    for(i = 0; i < page_count; i++)
    {
        src_page = pages[i];

        for(j = 0; j < 100; j++)
        {
            dst_page = os_shmem_read_mapping_page(file_addr_space, i, NULL);

            if(unlikely(IS_ERR(dst_page)))
            {
                msleep(5);
            }
            else
            {
                break;
            }
        }

        if(unlikely(IS_ERR(dst_page)))
        {
            fput(file_storage);
            file_storage = NULL;
            zx_info("when swapout read mapping failed. %d, %d, size: %dk\n", i, j, pages_memory->size >> 10);

            goto __failed;
        }

        preempt_disable();

        src_addr = os_kmap_atomic(src_page, OS_KM_USER0);
        dst_addr = os_kmap_atomic(dst_page, OS_KM_USER1);

        memcpy(dst_addr, src_addr, PAGE_SIZE);

        os_kunmap_atomic(dst_addr, OS_KM_USER1);
        os_kunmap_atomic(src_addr, OS_KM_USER0);

        preempt_enable();

        set_page_dirty(dst_page);
        mark_page_accessed(dst_page);
#if LINUX_VERSION_CODE < KERNEL_VERSION(4,6,0)
        page_cache_release(dst_page);
#else
        put_page(dst_page);
#endif
        balance_dirty_pages_ratelimited(file_addr_space);
        cond_resched();
    }

__failed:

    zx_release_os_pages(pages);
    return file_storage;
}

int zx_pages_memory_swapin(struct os_pages_memory *pages_memory, void *file)
{
    struct file          *file_storage = file;
    struct address_space *file_addr_space;
    struct page          **pages;
    struct page          *src_page, *dst_page;
    void                 *src_addr, *dst_addr;
    int i, page_count = 0;

    file_addr_space = file_storage->f_path.dentry->d_inode->i_mapping;

    pages = zx_acquire_os_pages(pages_memory, pages_memory->size, 0, &page_count);

    if(pages == NULL) return -1;

    for(i = 0; i < page_count; i++)
    {
        src_page = os_shmem_read_mapping_page(file_addr_space, i, NULL);
        dst_page = pages[i];

        if(unlikely(IS_ERR(src_page)))
        {
            zx_info("when swapin read mapping failed. %d\n", i);

            zx_release_os_pages(pages);

            pages = NULL;

            return -1;
        }

        preempt_disable();

        src_addr = os_kmap_atomic(src_page, OS_KM_USER0);
        dst_addr = os_kmap_atomic(dst_page, OS_KM_USER1);

        memcpy(dst_addr, src_addr, PAGE_SIZE);

        os_kunmap_atomic(dst_addr, OS_KM_USER1);
        os_kunmap_atomic(src_addr, OS_KM_USER0);

        preempt_enable();

#if LINUX_VERSION_CODE < KERNEL_VERSION(4,6,0)
        page_cache_release(dst_page);
#else
        put_page(dst_page);
#endif
    }

    fput(file_storage);

    zx_release_os_pages(pages);

    return 0;
}

void zx_release_file_storage(void *file)
{
    fput(file);
}

int zx_seq_printf(struct os_seq_file *seq_file, const char *f, ...)
{
    int ret;

#ifdef SEQ_PRINTF
    ret = seq_printf(seq_file->seq_file, f);
#else
    va_list args;

    va_start(args, f);
    seq_vprintf(seq_file->seq_file, f, args);
    va_end(args);

    ret = 0;
#endif
    return ret;
}

unsigned int ZX_API_CALL  zx_secure_read32(unsigned long addr)
{
#if CONFIG_X86 || defined(__mips64__) || (__aarch64__)
    zx_assert(0, "");
    return 0;
#else
    return svc_system_read_pmu_reg(addr);
#endif
}

void ZX_API_CALL  zx_secure_write32(unsigned long addr, unsigned int val)
{
#if CONFIG_X86 || defined(__mips64__) || defined(__aarch64__)
    zx_assert(0, "");
#else
    svc_system_write_pmu_reg(addr, val);
#endif
}

static void __zx_printfn_seq_file(struct os_printer *p, struct va_format *vaf)
{
    zx_seq_printf(p->arg, "%pV", vaf);
}

struct os_printer zx_seq_file_printer(struct os_seq_file *f)
{
    struct os_printer p = {
        .printfn = __zx_printfn_seq_file,
        .arg = f,
    };
    return p;
}

static void __zx_printfn_info(struct os_printer *p, struct va_format *vaf)
{
    zx_info("%pV", vaf);
}

struct os_printer zx_info_printer(void *dev)
{
    struct os_printer p = {
        .printfn = __zx_printfn_info,
        .arg = dev,
    };
    return p;
}

void zx_printf(struct os_printer *p, const char *f, ...)
{
    struct va_format vaf;
    va_list args;
    struct os_printer info = zx_info_printer(NULL);
    va_start(args, f);
    vaf.fmt = f;
    vaf.va = &args;
    if (p == NULL) {
        p = &info;
    }
    p->printfn(p, &vaf);
    va_end(args);
}

/*
 * zx_create_workqueue - create workqueue if necessary for driver.
 * @name: workqueue name
 *
 * Return:
 * Pointer to the allocated workqueue on success, %NULL on failure
 */
struct os_workqueue_struct *zx_create_workqueue(char *name)
{
    struct os_workqueue_struct *os_wq = zx_calloc(sizeof(struct os_workqueue_struct));

    if(!os_wq)
        return NULL;

    os_wq->wq = create_workqueue(name);
    if(!os_wq->wq)
    {
        zx_free(os_wq);
        return NULL;
    }

    return os_wq;
}

/*
 * zx_destroy_workqueue - destory workqueue created before.
 * @wq: workqueue to be destroied.
 */
void zx_destroy_workqueue(struct os_workqueue_struct *os_wq)
{
    if(!os_wq)
        return;

    destroy_workqueue(os_wq->wq);

    os_wq->wq = NULL;

    zx_free(os_wq);
}

/*
 * zx_work_func - the actual callback function provide to linux kernel
 * @work: work struct instance.
 *
 * this function is not exported to outside. Callback function provided by outside
 * is not compitable with linux(as work_struct is wrapped with os_work_struct), so
 * also wrap callback function.
 */
static void zx_work_func(struct work_struct *work)
{
    struct os_work_struct *os_work = container_of(work, struct os_work_struct, work);

    return os_work->func(os_work);
}

/*
 * zx_init_work_struct - create and initialize work struct instance.
 * @private_data: private data bound with this work struct, caller use it in private way.
 * @func: work struct callback function
 *
 * Return:
 * Pointer to work struct instance, %NULL on failure.
 *
 * this callback function will be provided to linux kernel, and will be called when work struct is scheduled.
 */
struct os_work_struct *zx_init_work_struct(void *private_data, void *func)
{
    struct os_work_struct *os_work = zx_calloc(sizeof(struct os_work_struct));

    if(!os_work)
        return NULL;

    os_work->priv = private_data;
    os_work->func = (os_work_func_t)func;

    INIT_WORK(&os_work->work, zx_work_func);

    return os_work;
}

/*
 * zx_deinit_work_struct - destroy work struct instance.
 * @work: work struct to be destroied
 */
void zx_deinit_work_struct(struct os_work_struct *os_work)
{
    if(!os_work)
        return;

    os_work->priv = NULL;
    os_work->func = NULL;

    zx_free(os_work);
}

/*
 * zx_get_work_struct_priv - get work struct private data.
 * @work: work struct instance
 *
 * Return
 * pointer to private data.
 *
 * this field is used to store some private data caller will use.
 */
void *zx_get_work_struct_priv(struct os_work_struct *os_work)
{
    if(!os_work)
        return NULL;

    return os_work->priv;
}

/*
 * zx_queue_work - queue work struct to specific work queue.
 * @wq: work queue to be queued in
 * @work: work struct to be queued
 *
 * Returns:
 * %0xFFFFFFFF if any paramenter is invalid.
 * %false if @work was already on a queue, %true otherwise.
 */
unsigned int zx_queue_work(struct os_workqueue_struct *os_wq, struct os_work_struct *os_work)
{
    if(!os_wq || !os_wq->wq || !os_work)
        return -1;

    return queue_work(os_wq->wq, &os_work->work);
}

/*
 * zx_schedule_work - schedule work struct, put work task in global workqueue.
 * @work: work struct to schedule
 *
 * this work struct will be queue to system-wide global work queue in linux kernel.
 *
 * Returns %false if @work was already on the kernel-global workqueue and
 * %true otherwise.
 */
unsigned int zx_schedule_work(struct os_work_struct *os_work)
{
    if(!os_work)
        return -1;

    return schedule_work(&os_work->work);
}

/*
 * zx_flush_work - wait for a work to finish executing the last queueing instance.
 * @work: work struct to flush
 *
 * Wait until @work has finished execution.  @work is guaranteed to be idle
 * on return if it hasn't been requeued since flush started.

 * Return:
 * %0xFFFFFFFF if work struct is invalid.
 * %true if flush_work() waited for the work to finish execution,
 * %false if it was already idle.
 */
unsigned int zx_flush_work(struct os_work_struct *os_work)
{
    if(!os_work)
        return -1;

    return flush_work(&os_work->work);
}

/*
 * zx_flush_work - ensure that any scheduled work has run to completion.
 * @work: work struct to flush
 *
 * This function sleeps until all work items which were queued on entry
 * have finished execution, but it is not livelocked by new incoming ones.
 */
void zx_flush_workqueue(struct os_workqueue_struct *os_wq)
{
    if(!os_wq || !os_wq->wq)
        return;

    return flush_workqueue(os_wq->wq);
}

int zx_disp_wait_idle(void *disp_info)
{
    return disp_wait_idle(disp_info);
}

#if defined(__i386__) || defined(__x86_64__)
#define zx_mb_asm()       asm volatile("mfence":::"memory")
#define zx_rmb_asm()      asm volatile("lfence":::"memory")
#define zx_wmb_asm()      asm volatile("sfence":::"memory")
#define zx_flush_wc_asm() zx_wmb_asm()
#define zx_dsb_asm()
#elif defined(__mips64__)
#include <asm/barrier.h>
#define zx_mb_asm()    mb()
#define zx_rmb_asm()   rmb()
#define zx_wmb_asm()   wmb()
#define zx_flush_wc_asm() mb()
#define zx_dsb_asm()
#else
#if __ARM_ARCH >= 7
#define dmb(opt)        asm volatile("dmb " #opt : : : "memory")
#define dsb(opt)        asm volatile("dsb " #opt : : : "memory")

#define zx_mb_asm()    dsb(sy)
#define zx_rmb_asm()   dsb(ld)
#define zx_wmb_asm()   dsb(st)
#define zx_flush_wc_asm() dsb(sy)
#define zx_dsb_asm()   dsb(sy)
#endif
#endif

void zx_mb(void)
{
    zx_mb_asm();
}

void zx_rmb(void)
{
    zx_rmb_asm();
}

void zx_wmb(void)
{
    zx_wmb_asm();
}

void zx_flush_wc(void)
{
    zx_flush_wc_asm();
}

void zx_dsb(void)
{
    zx_dsb_asm();
}

struct zx_drm_range_allocator
{
    struct range_allocator base;
    struct drm_mm mm;
};
#define to_zx_ra(drm_mm) container_of(drm_mm, struct zx_drm_range_allocator, mm)

struct zx_drm_range_node
{
    struct range_node base;
    struct drm_mm_node mm_node;
};
#define to_zx_node(drm_mm_node) container_of(drm_mm_node, struct zx_drm_range_node, mm_node)


// range allocator
static void zx_drm_mm_ra_release(struct range_allocator *ra)
{
    struct zx_drm_range_allocator *zxra = (struct zx_drm_range_allocator*)ra;

    drm_mm_takedown(&zxra->mm);

    zx_free(zxra);
}

static void zx_drm_mm_ra_node_release(struct range_node *node)
{
    struct zx_drm_range_node *zxnode = (struct zx_drm_range_node*)node;
    struct zx_drm_range_allocator *zxra = to_zx_ra(zxnode->mm_node.mm);

    zxra->base.free_size += zxnode->base.size;

    drm_mm_remove_node(&zxnode->mm_node);

    zx_free(zxnode);
}


// alloc a fixed position/size node
// @start: the fixed start address
// @size: the size of the range
static struct range_node* zx_drm_mm_ra_reserve(struct range_allocator *ra,
            unsigned long long start,
            unsigned long long size)
{
    int ret = 0;
    struct zx_drm_range_allocator *zxra = (struct zx_drm_range_allocator*)ra;
    struct zx_drm_range_node *zxnode = zx_calloc(sizeof(*zxnode));

    zx_assert((start & (zxra->base.alignment - 1)) == 0, "alignment mismatch");

    zxnode->mm_node.start = start;
    zxnode->mm_node.size = size;

    ret = drm_mm_reserve_node(&zxra->mm, &zxnode->mm_node);

    if (ret != 0)
    {
        zx_free(zxnode);
        zxnode = NULL;
    }
    else
    {
        zxnode->base.start = zxnode->mm_node.start;
        zxnode->base.size = zxnode->mm_node.size;
        zxnode->base.release = zx_drm_mm_ra_node_release;
        ra->free_size -= zxnode->base.size;
    }
    return zxnode ? &zxnode->base : NULL;
}

// alloc a new range
// @size: size of the allocation
// @alignment: alignment of the allocation, it's the address alignment but not size alignment
// @start: start of the allowed range for this node
// @end: end of the allowed range for this node
static struct range_node* zx_drm_mm_ra_alloc(struct range_allocator *ra,
            unsigned long long size,
            unsigned int alignment,
            unsigned long long start,
            unsigned long long end,
            unsigned int flags)
{
    int ret = 0;
    struct zx_drm_range_allocator *zxra = (struct zx_drm_range_allocator*)ra;
    struct zx_drm_range_node *zxnode = zx_calloc(sizeof(*zxnode));
#if DRM_VERSION_CODE >= KERNEL_VERSION(4, 11, 0)
    enum drm_mm_insert_mode mode;
#else
    enum drm_mm_search_flags sflags;
    enum drm_mm_allocator_flags aflags;
#endif

#if DRM_VERSION_CODE >= KERNEL_VERSION(4, 11, 0)
    mode = (flags & RANGE_CREATE_FROM_TOP) ? DRM_MM_INSERT_HIGH : DRM_MM_INSERT_BEST;
#else
    if (flags & RANGE_CREATE_FROM_TOP)
    {
        sflags = DRM_MM_SEARCH_BELOW;
        aflags = DRM_MM_CREATE_TOP;
    }
    else
    {
        sflags = DRM_MM_SEARCH_DEFAULT;
        aflags = DRM_MM_CREATE_DEFAULT;
    }
#endif

    if (alignment < zxra->base.alignment)
        alignment = zxra->base.alignment;

#if DRM_VERSION_CODE >= KERNEL_VERSION(4, 11, 0)
    ret = drm_mm_insert_node_in_range(&zxra->mm, &zxnode->mm_node, size, alignment, 0, start, end, mode);
#else
    ret = drm_mm_insert_node_in_range_generic(&zxra->mm, &zxnode->mm_node, size, alignment, 0, start, end, sflags, aflags);
#endif

    if (ret != 0)
    {
        zx_free(zxnode);
        zxnode = NULL;
    }
    else
    {
        zxnode->base.start = zxnode->mm_node.start;
        zxnode->base.size = zxnode->mm_node.size;
        zxnode->base.release = zx_drm_mm_ra_node_release;

        ra->free_size -= zxnode->base.size;
    }
    return zxnode ? &zxnode->base : NULL;
}

static struct range_node* zx_drm_mm_ra_lookup(struct range_allocator *ra,
            unsigned long long start,
            unsigned int size)
{
    struct zx_drm_range_allocator *zxra = (struct zx_drm_range_allocator*)ra;
    struct drm_mm_node *node, *best;
    struct rb_node *iter;
    unsigned long long offset;

#if LINUX_VERSION_CODE >= KERNEL_VERSION(4, 14, 0)
    iter = zxra->mm.interval_tree.rb_root.rb_node;
#else
    iter = zxra->mm.interval_tree.rb_node;
#endif
    best = NULL;

//    ra->dump(ra, "  ");

    while(likely(iter))
    {
        node = rb_entry(iter, struct drm_mm_node, rb);
        offset = node->start;
//        zx_info("start:%llx, size:%llx, node->start:%llx, node->size:%llx.\n",
//                start, size, node->start, node->size);

        if (start >= offset)
        {
            iter = iter->rb_right;
            best = node;
            if (start == offset)
                break;
        }
        else
        {
            iter = iter->rb_left;
        }
    }

    if (best) {
        offset = best->start + best->size;
        if (offset < start + size)
            best = NULL;
    }

    return best ? &(to_zx_node(best)->base) : NULL;
}

static int zx_drm_mm_ra_for_each_node(struct range_allocator *ra,
            unsigned long long start,
            unsigned long long end,
            void *priv,
            range_node_visitor_t visitor)
{
    int ret;
    int node_num = 0;
    struct drm_mm_node *node, *next;
    struct zx_drm_range_allocator *zxra = (struct zx_drm_range_allocator*)ra;

#if DRM_VERSION_CODE >= KERNEL_VERSION(4, 15, 0)
    list_for_each_entry_safe(node, next, drm_mm_nodes(&zxra->mm), node_list)
#else
    list_for_each_entry_safe(node, next, &zxra->mm.head_node.node_list, node_list)
#endif
    {
        if (node->start >= start && node->start + node->size <= end)
        {
            ++node_num;
            ret = visitor(&(to_zx_node(node)->base), priv);
            if (!ret)
                break;
        }
    }

    return node_num;
}

static unsigned long long zx_drm_mm_ra_dump_hole(struct os_printer *p, struct drm_mm_node *entry, const char *prefix)
{

#if DRM_VERSION_CODE >= KERNEL_VERSION(4, 11, 0)
    if (entry->hole_size)
#else
    if (entry->hole_follows)
#endif
    {
        unsigned long long hole_start = drm_mm_hole_node_start(entry);
        unsigned long long hole_end = drm_mm_hole_node_end(entry);
        unsigned long long hole_size = hole_end - hole_start;
        zx_printf(p, "%s %#llx-%#llx: %llu: free\n",
            prefix, hole_start, hole_end, hole_size);

        return hole_size;
    }
    return 0;
}

static void zx_drm_mm_ra_dump(void *printer, struct range_allocator *ra, const char *prefix)
{
    struct os_printer *p = printer;
    struct drm_mm_node *entry;
    unsigned long long total_used = 0, total_free = 0, total = 0;
    struct zx_drm_range_allocator *zxra = (struct zx_drm_range_allocator*)ra;

    total_free += zx_drm_mm_ra_dump_hole(p, &zxra->mm.head_node, prefix);

    drm_mm_for_each_node(entry, &zxra->mm)
    {
        zx_printf(p, "%s %#llx-%#llx: %llu(t%llx): used\n",
                prefix, entry->start, entry->start + entry->size, entry->size, to_zx_node(entry)->base.tag);

        total_used += entry->size;
        total_free += zx_drm_mm_ra_dump_hole(p, entry, prefix);
    }

    total = total_free + total_used;

    zx_printf(p, "%s total: %llu, used %llu, free %llu\n", prefix, total,
            total_used, total_free);
}

struct range_allocator* zx_create_range_allocator(unsigned long long start, unsigned long long size, unsigned int alignment)
{
    struct zx_drm_range_allocator *zxra = zx_calloc(sizeof(*zxra));

    zxra->base.release = zx_drm_mm_ra_release;
    zxra->base.reserve = zx_drm_mm_ra_reserve;
    zxra->base.alloc = zx_drm_mm_ra_alloc;
    zxra->base.lookup = zx_drm_mm_ra_lookup;
//    zxra->base.remove = zx_drm_mm_ra_remove;
    zxra->base.for_each_node = zx_drm_mm_ra_for_each_node;
    zxra->base.dump = zx_drm_mm_ra_dump;

    zxra->base.start = start;
    zxra->base.size = size;
    zxra->base.free_size = size;
    zxra->base.alignment = alignment;
    drm_mm_init(&zxra->mm, start, size);
    return &zxra->base;
}


