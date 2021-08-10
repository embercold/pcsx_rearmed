// Polyfill for POSIX/Linux functions not available in GC/Wii/WiiU
// Written by Ember Cold in August 2021

#if !defined(HW_DOL) && !defined(HW_RVL) && !defined(HW_WUP)
#error The polyfill only supports GC/Wii/WiiU hosts
#endif

#if defined(HW_DOL) | defined(HW_RVL)
#include <gccore.h>
#include <ogcsys.h>
#include <ogc/cond.h>
#else
#include <wiiu/os/semaphore.h>
#endif

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "sys/mman.h"
#include "pthread.h"
#include "semaphore.h"
#include "unistd.h"

#define arr_countof(X) (sizeof(X) / sizeof(*(X)))

//
// <sys/mman.h>
//

static void *mmapped_addrs[256] = { 0 };

void *mmap(void *addr, size_t length,
    int prot, int flags, int fd, off_t offset)
{
    if (length == 0 || addr != NULL || fd != -1) {
        return addr;
    }
    // Return the existing address if it's already allocated
    for (int i = 0; i < arr_countof(mmapped_addrs); i++) {
        if (mmapped_addrs[i] == addr) {
            return mmapped_addrs[i];
        }
    }
    // Otherwise allocate memory and store it into the table
    for (int i = 0; i < arr_countof(mmapped_addrs); i++) {
        if (mmapped_addrs[i] == NULL) {
            mmapped_addrs[i] = calloc(1, length);
            return mmapped_addrs[i];
        }
    }
    // If the table is full, memory will leak
    return calloc(1, length);
}

int munmap(void *addr, size_t length)
{
    // Only free addresses allocated by mmap
    if (addr != NULL) {
        for (int i = 0; i < arr_countof(mmapped_addrs); i++) {
            if (mmapped_addrs[i] == addr) {
                free(addr);
                mmapped_addrs[i] = 0;
                break;
            }
        }
    }
    return 0;
}

int mprotect(void *addr, size_t len, int prot)
{
    return 0;
}


//
// <pthread.h>
//

// Headers here were internal to libretro-common and had to be repurposed
// so that they could be used in the core directly
#if defined(HW_DOL) || defined(HW_RVL)
#include "pthread_gx.inl"
#else
#include "pthread_wiiu.inl"
#endif


//
// <semaphore.h>
//

#if defined(HW_DOL) || defined(HW_RVL)

int sem_init(sem_t *sem, int pshared, unsigned int value)
{
    return (LWP_SemInit(sem, value, -1) < 0) ? -1 : 0;
}

int sem_destroy(sem_t *sem)
{
    return (LWP_SemDestroy(*sem) < 0) ? -1 : 0;
}

int sem_post(sem_t *sem)
{
    return (LWP_SemPost(*sem) < 0) ? -1 : 0;
}

int sem_wait(sem_t *sem)
{
    return (LWP_SemWait(*sem) < 0) ? -1 : 0;
}

#else

int sem_init(sem_t *sem, int pshared, unsigned int value)
{
    *sem = (sem_t) calloc(1, sizeof(OSSemaphore));
    OSInitSemaphore((OSSemaphore *)*sem, value);
    return 0;
}

int sem_destroy(sem_t *sem)
{
    free((OSSemaphore *)*sem);
    *sem = 0;
    return 0;
}

int sem_post(sem_t *sem)
{
    OSSignalSemaphore((OSSemaphore *)*sem);
    return 0;
}

int sem_wait(sem_t *sem)
{
    OSWaitSemaphore((OSSemaphore *)*sem);
    return 0;
}

#endif


//
// <unistd.h>
//

#if defined(HW_DOL) || defined(HW_RVL)
// System information for GC/Wii hosts
#define WII_PAGE_SIZE 1024
#define WII_CPU_NUM 1
#else
// System information for WiiU hosts
#define WII_PAGE_SIZE 1024
#define WII_CPU_NUM 3
#endif

long sysconf(int name)
{
    switch (name) {
    case _SC_PAGE_SIZE:
        return WII_PAGE_SIZE;
    case _SC_NPROCESSORS_ONLN:
        return WII_CPU_NUM;
    default:
        return 0;
    }
}
