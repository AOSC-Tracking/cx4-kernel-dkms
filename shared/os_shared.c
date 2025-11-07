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

#define ZX_TRACK_OK    0
#define ZX_TRACK_FAIL  1

typedef enum
{
    ZX_TYPE_MALLOC_TRACK       = 0x01,
    ZX_TYPE_ALLOC_PAGES_TRACK  = 0x02,
    ZX_TYPE_MAP_PAGES_TRACK    = 0x03,
    ZX_TYPE_MAP_IO_TRACK       = 0x04,
    ZX_TYPE_MEM_TRACK_LAST     = 0x05,

}zx_mem_track_type;

typedef struct __zx_mem_track* zx_mem_track_ptr;

typedef struct __zx_mem_track
{
   zx_mem_track_ptr next;
   void*             addr;
   const char*       file;
   unsigned int      size;
   unsigned int      line;
   unsigned int      times;
}zx_mem_track;

static struct os_mutex *memory_track_lock = NULL;
static zx_mem_track zx_memory_tracks[ZX_TYPE_MEM_TRACK_LAST-1] = {{0},};

static inline void zx_mem_track_add(zx_mem_track_type type, void* addr, const char* file, int line, unsigned long size)
{
    zx_mem_track *pMemList  = &zx_memory_tracks[type - 1];
    zx_mem_track *pMemTrack = NULL;

    if(addr == NULL)
    {
        return;
    }

    pMemTrack = zx_malloc_priv(sizeof(zx_mem_track));

    if(pMemTrack == NULL)
    {
        zx_error("malloc struct zx_mem_track failed.\n");
        return;
    }

    if(memory_track_lock != NULL)
    {
        zx_mutex_lock(memory_track_lock);
    }

    pMemTrack->addr = addr;
    pMemTrack->file = file;
    pMemTrack->line = line;
    pMemTrack->size = size;
    pMemTrack->next = pMemList->next;
    pMemTrack->times = pMemList->times++;


    pMemList->next  = pMemTrack;
    pMemList->size++;

    if(memory_track_lock != NULL)
    {
        zx_mutex_unlock(memory_track_lock);
    }
}

static inline int zx_mem_track_remove(zx_mem_track_type type, int check_overflow, void* addr, const char* file, int line)
{
    zx_mem_track *pMemList  = &zx_memory_tracks[type - 1];
    zx_mem_track *pTemp, *pPre;
    int status = ZX_TRACK_OK;

    zx_mutex_lock(memory_track_lock);

    pPre  = pMemList;
    pTemp = pMemList->next;

    while((pTemp != NULL) && (pTemp->addr != addr))
    {
        pPre  = pTemp;
        pTemp = pTemp->next;
    }

    if(pTemp != NULL)
    {
        if(check_overflow)
        {
            char *head = (char*)pTemp->addr;
            char *tail = (char*)pTemp->addr + pTemp->size - 32;
            int  i;

            for(i=0; i < 32; i++)
            {
                if(head[i] != 0x55 || tail[i] != 0x55)
                {
                    zx_info("OVERFLOW %s: %d, size = %x, times = %d.\n",
                             pTemp->file, pTemp->line, pTemp->size, pTemp->times);
                    break;
                }
            }
            zx_memset(pTemp->addr, 0, pTemp->size);
        }
        pPre->next = pTemp->next;
        zx_free_priv(pTemp);

        pMemList->size--;
    }
    else
    {
        #if 0
        zx_info("type:%d-memory:0x%p: %s line:%d, no found in track list.\n", pMemList->line, addr, file, line);
        #endif
        status = ZX_TRACK_FAIL;
    }
    zx_mutex_unlock(memory_track_lock);

    return status;
}

static inline void zx_mem_track_list(zx_mem_track* pMemList, struct os_file *file)
{
    char msg[512];
    zx_mem_track *pList = pMemList->next;

    zx_info("memory type: %d, leaks number: %d\n", pMemList->line, pMemList->size);

    if(file != NULL)
    {
        zx_vsnprintf(msg, 512, "memory type: %d, leaks number: %d\n", pMemList->line, pMemList->size);
        zx_file_write(file, msg, zx_strlen(msg));
    }

    while(pList)
    {
        zx_info("%s: %d, size = %u, times = %d.\n", pList->file, pList->line, pList->size, pList->times);
        if(file != NULL)
        {
            zx_vsnprintf(msg, 512, "%s: %d, size = %u, times = %d.\n", pList->file, pList->line, pList->size, pList->times);
            zx_file_write(file, msg, zx_strlen(msg));
        }
        pList = pList->next;
    }

}

void zx_mem_track_init(void)
{
    zx_mem_track_type type;
    zx_mem_track  *header = NULL;

    memory_track_lock = zx_create_mutex();

    for(type = ZX_TYPE_MALLOC_TRACK; type < ZX_TYPE_MEM_TRACK_LAST; type++)
    {
        header = &zx_memory_tracks[type-1];
        zx_memset(header, 0, sizeof(*header));
        header->line = type;
    }
}

void zx_mem_track_list_result(void)
{
    zx_mem_track_type type;
    zx_mem_track  *header = NULL;
    struct os_file *file = NULL;
#if ZX_MEM_TRACK_RESULT_TO_FILE
    file = zx_file_open(zx_mem_track_result_path);
#endif

    zx_info("************leak************\n");

    zx_mutex_lock(memory_track_lock);

    for(type = ZX_TYPE_MALLOC_TRACK; type < ZX_TYPE_MEM_TRACK_LAST; type++)
    {
        header = &zx_memory_tracks[type-1];

        if(header->next)
        {
            zx_mem_track_list(header, file);
        }
    }
    zx_mutex_unlock(memory_track_lock);

#if ZX_MEM_TRACK_RESULT_TO_FILE
    zx_file_close(file);
#endif
}

void zx_mem_track_clear_result(void)
{
    int type;
    zx_mutex_lock(memory_track_lock);

    for(type = ZX_TYPE_MALLOC_TRACK; type < ZX_TYPE_MEM_TRACK_LAST; type++)
    {
        zx_mem_track *track = zx_memory_tracks[type-1].next;
        zx_mem_track *next = NULL;

        while(track)
        {
            next = track->next;

            zx_free_priv(track);

            track = next;
        }

        zx_memory_tracks[type-1].times = 0;
        zx_memory_tracks[type-1].next = NULL;
        zx_memory_tracks[type-1].size = 0;
    }

    zx_mutex_unlock(memory_track_lock);
}

void *zx_malloc_track(unsigned long size, const char *file, unsigned int line)
{
    unsigned long track_size     = size;
    void          *returned_addr = NULL;
    char          *malloc_addr   = NULL;

#if ZX_CHECK_MALLOC_OVERFLOW
    track_size += 2 * 32;
#endif

    malloc_addr = zx_malloc_priv(track_size);

#if ZX_CHECK_MALLOC_OVERFLOW
    zx_memset(malloc_addr, 0x55, 32);
    zx_memset(malloc_addr + track_size - 32, 0x55, 32);
    returned_addr = malloc_addr + 32;
#else
    returned_addr = malloc_addr;
#endif

    zx_mem_track_add(ZX_TYPE_MALLOC_TRACK, malloc_addr, file, line, track_size);

    return returned_addr;
}

void *zx_calloc_track(unsigned long size, const char *file, unsigned int line)
{
    unsigned long track_size     = size;
    void          *returned_addr = NULL;
    char          *malloc_addr   = NULL;

#if ZX_CHECK_MALLOC_OVERFLOW
    track_size += 2 * 32;
#endif

    malloc_addr = zx_calloc_priv(track_size);

#if ZX_CHECK_MALLOC_OVERFLOW
    zx_memset(malloc_addr, 0x55, 32);
    zx_memset(malloc_addr + track_size - 32, 0x55, 32);
    returned_addr = malloc_addr + 32;
#else
    returned_addr = malloc_addr;
#endif

    zx_mem_track_add(ZX_TYPE_MALLOC_TRACK, malloc_addr, file, line, track_size);

    return returned_addr;
}

void zx_free_track(void *addr, const char *file, unsigned int line)
{
    void *malloc_addr   = addr;
    int  overflow_check = 0;
#if ZX_CHECK_MALLOC_OVERFLOW
    overflow_check = 1;
    malloc_addr    = (char*)addr - 32;
#endif
    if(addr == NULL)
    {
        zx_error("free a NULL pointer: %s, %d.\n", file, line);
        return;
    }

    if(zx_mem_track_remove(ZX_TYPE_MALLOC_TRACK, overflow_check, malloc_addr, file, line) == ZX_TRACK_OK)
    {
    }
    zx_free_priv(malloc_addr);
}

struct os_pages_memory *
zx_allocate_pages_memory_track(void *pdev, unsigned long long size, int page_size, alloc_pages_flags_t alloc_flags, const char *file, unsigned int line)
{
    struct os_pages_memory *memory = NULL;

    memory = zx_allocate_pages_memory_priv(pdev, size, page_size, alloc_flags);

    zx_mem_track_add(ZX_TYPE_ALLOC_PAGES_TRACK, memory, file, line, size);

    return memory;
}

void zx_free_pages_memory_track(void *pdev, struct os_pages_memory *memory, const char *file, unsigned int line)
{
    if(memory == NULL)
    {
        zx_error("free a NULL pages memory: %s, %d.\n", file, line);
        return;
    }
    zx_mem_track_remove(ZX_TYPE_ALLOC_PAGES_TRACK, 0, memory, file, line);
    zx_free_pages_memory_priv(pdev, memory);
}

zx_cpu_vm_area_t *zx_map_pages_memory_track(zx_map_argu_t *map,
                                      const char *file, unsigned int line)
{
    zx_cpu_vm_area_t *vm_area = zx_map_pages_memory_priv(map);

    zx_mem_track_add(ZX_TYPE_MAP_PAGES_TRACK, vm_area->virt_addr, file, line, vm_area->size);

    return vm_area;
}

void zx_unmap_pages_memory_track(zx_cpu_vm_area_t *vm_area,
                                  const char *file, unsigned int line)
{
    if(vm_area->virt_addr == NULL)
    {
        zx_error("unmap a NULL pages memory: %s, %d.\n", file, line);
        return;
    }
    zx_mem_track_remove(ZX_TYPE_MAP_PAGES_TRACK, 0, vm_area->virt_addr, file, line);
    zx_unmap_pages_memory_priv(vm_area);
}

zx_cpu_vm_area_t *zx_map_io_memory_track(zx_map_argu_t *map,
                              const char *file, unsigned int line)
{
    zx_cpu_vm_area_t *vm_area = zx_map_io_memory_priv(map);
    int i = 0;

    if(vm_area->node_num == 1)
        zx_mem_track_add(ZX_TYPE_MAP_IO_TRACK, vm_area->virt_addr, file, line, vm_area->size);
    else{
        for(i=0; i<vm_area->node_num; i++)
            zx_mem_track_add(ZX_TYPE_MAP_IO_TRACK, vm_area->virt_addrs[i], file, line, vm_area->node_sizes[i]);
    }

    return vm_area;
}

void zx_unmap_io_memory_track(zx_cpu_vm_area_t *vm_area,
                               const char *file, unsigned int line)
{
    int i = 0;

    if(vm_area->node_num == 0)
    {
        zx_error("unmap a NULL io memory: %s, %d.\n", file, line);
        return;
    }

    if(vm_area->node_num == 1)
        zx_mem_track_remove(ZX_TYPE_MAP_IO_TRACK, 0, vm_area->virt_addr, file, line);
    else{
        for(i=0; i<vm_area->node_num; i++)
            zx_mem_track_remove(ZX_TYPE_MAP_IO_TRACK, 0, vm_area->virt_addrs[i], file, line);
    }

    zx_unmap_io_memory_priv(vm_area);
}
