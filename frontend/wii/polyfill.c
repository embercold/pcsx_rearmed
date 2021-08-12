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

#ifndef NDEBUG
#define TRACE(MSG, ...) \
    printf("[%s:%i]" MSG "\n", __FUNCTION__, __LINE__, ##__VA_ARGS__)
#else
#define TRACE(...)
#endif

//
// <sys/mman.h>
//

void *mmap(void *addr, size_t length,
    int prot, int flags, int fd, off_t offset)
{
    void *block = calloc(1, length);
    TRACE("addr=%08p length=%u prot=%08X ->block=%08p", addr, length, prot, block);
    return block;
}

int munmap(void *addr, size_t length)
{
    TRACE("addr=%p length=%u", addr, length);
    free(addr);
    return 0;
}

int mprotect(void *addr, size_t length, int prot)
{
    TRACE("addr=%08p length=%u prot=%08p", addr, length, prot);
    return 0;
}


//
// <pthread.h>
//

// Threads


#if defined(HW_DOL) | defined(HW_RVL)

#define WII_STACKSIZE 16*1024

int pthread_create(pthread_t *restrict thread,
    const pthread_attr_t *restrict attr,
    void *(*start_routine)(void *),
    void *restrict arg)
{
    TRACE("thread=%p", thread);
    return (LWP_CreateThread(thread, start_routine, arg, NULL, WII_STACKSIZE, 0) < 0) ? -1 : 0;
}

void pthread_exit(void *retval)
{
    TRACE("");
    /* Do nothing and hope for the best */
}

int pthread_join(pthread_t thread, void **retval)
{
    TRACE("thread=%p", thread);
    return (LWP_JoinThread(thread, retval) < 0) ? -1 : 0;
}

// Mutexes

int pthread_mutex_init(pthread_mutex_t *mutex,
    const pthread_mutexattr_t *attr)
{
    TRACE("mutex=%p", mutex);
    bool recursive = attr && (*attr & PTHREAD_MUTEX_RECURSIVE);
    return (LWP_MutexInit(mutex, recursive) < 0) ? -1 : 0;
}

int pthread_mutex_destroy(pthread_mutex_t *mutex)
{
    TRACE("mutex=%p", mutex);
    return (LWP_MutexDestroy(*mutex) < 0) ? -1 : 0;
}

int pthread_mutex_lock(pthread_mutex_t *mutex)
{
    // TRACE("mutex=%p", mutex);
    return (LWP_MutexLock(*mutex) < 0) ? -1 : 0;
}

int pthread_mutex_unlock(pthread_mutex_t *mutex)
{
    // TRACE("mutex=%p", mutex);
    return (LWP_MutexUnlock(*mutex) < 0) ? -1 : 0;
}

// Condition variables

int pthread_cond_init(pthread_cond_t *cond,
    const pthread_condattr_t *attr)
{
    TRACE("cond=%p", cond);
    return (LWP_CondInit(cond) < 0) ? -1 : 0;
}

int pthread_cond_destroy(pthread_cond_t *cond)
{
    TRACE("cond=%p", cond);
    return (LWP_CondDestroy(*cond) < 0) ? -1 : 0;
}

int pthread_cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex)
{
    TRACE("cond=%p mutex=%p", cond, mutex);
    return (LWP_CondWait(*cond, *mutex) < 0) ? -1 : 0;
}

int pthread_cond_signal(pthread_cond_t *cond)
{
    TRACE("cond=%p", cond);
    return (LWP_CondSignal(*cond) < 0) ? -1 : 0;
}

#else

#endif


//
// <semaphore.h>
//

#if defined(HW_DOL) || defined(HW_RVL)

int sem_init(sem_t *sem, int pshared, unsigned int value)
{
    TRACE("sem=%p pshared=%i value=%u", sem, pshared, value);
    return (LWP_SemInit(sem, value, -1) < 0) ? -1 : 0;
}

int sem_destroy(sem_t *sem)
{
    TRACE("sem=%p ", sem);
    return (LWP_SemDestroy(*sem) < 0) ? -1 : 0;
}

int sem_post(sem_t *sem)
{
    TRACE("sem=%p ", sem);
    return (LWP_SemPost(*sem) < 0) ? -1 : 0;
}

int sem_wait(sem_t *sem)
{
    TRACE("sem=%p ", sem);
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
    TRACE("name=%i", name);
    switch (name) {
    case _SC_PAGE_SIZE:
        return WII_PAGE_SIZE;
    case _SC_NPROCESSORS_ONLN:
        return WII_CPU_NUM;
    default:
        return 0;
    }
}
