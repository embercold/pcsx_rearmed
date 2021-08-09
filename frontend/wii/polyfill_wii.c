// Polyfill for several POSIX/Linux functions not available in Wii
// Written by Ember Cold in August 2021

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <gccore.h>
#include <ogcsys.h>
#include <ogc/cond.h>
#include "sys/mman.h"
#include "pthread.h"
#include "semaphore.h"
#include "unistd.h"

#define countof(X) (sizeof(X) / sizeof((X)[0]))

//
// <sys/mman.h>
//

#define WII_ALLOWED_FD -1
#define WII_ALLOWED_ADDR NULL

static void *mmapped_addrs[256] /* = { 0 }; */;

void *mmap(void *addr, size_t length,
    int prot, int flags, int fd, off_t offset)
{
    if (length == 0 || addr != WII_ALLOWED_ADDR || fd != WII_ALLOWED_FD) {
        return addr;
    }
    // Return the existing address if it's already allocated
    for (int i = 0; i < countof(mmapped_addrs); i++) {
        if (mmapped_addrs[i] == addr) {
            return mmapped_addrs[i];
        }
    }
    // Otherwise allocate memory and store it into the table
    for (int i = 0; i < countof(mmapped_addrs); i++) {
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
        for (int i = 0; i < countof(mmapped_addrs); i++) {
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

// Threads

#define WII_STACKSIZE 16*1024

int pthread_create(pthread_t *restrict thread,
    const pthread_attr_t *restrict attr,
    void *(*start_routine)(void *),
    void *restrict arg)
{
    return (LWP_CreateThread(thread, start_routine, arg, NULL, WII_STACKSIZE, 0) < 0) ? -1 : 0;
}

void pthread_exit(void *retval)
{
    /* Do nothing and hope for the best */
}

int pthread_join(pthread_t thread, void **retval)
{
    return (LWP_JoinThread(thread, retval) < 0) ? -1 : 0;
}

// Mutexes

int pthread_mutex_init(pthread_mutex_t *mutex,
    const pthread_mutexattr_t *attr)
{
    bool recursive = attr && (*attr & PTHREAD_MUTEX_RECURSIVE);
    return (LWP_MutexInit(mutex, recursive) < 0) ? -1 : 0;
}

int pthread_mutex_destroy(pthread_mutex_t *mutex)
{
    return (LWP_MutexDestroy(*mutex) < 0) ? -1 : 0;
}

int pthread_mutex_lock(pthread_mutex_t *mutex)
{
    return (LWP_MutexLock(*mutex) < 0) ? -1 : 0;
}

int pthread_mutex_unlock(pthread_mutex_t *mutex)
{
    return (LWP_MutexUnlock(*mutex) < 0) ? -1 : 0;
}

// Condition variables

int pthread_cond_init(pthread_cond_t *cond,
    const pthread_condattr_t *attr)
{
    return (LWP_CondInit(cond) < 0) ? -1 : 0;
}

int pthread_cond_destroy(pthread_cond_t *cond)
{
    return (LWP_CondDestroy(*cond) < 0) ? -1 : 0;
}

int pthread_cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex)
{
    return (LWP_CondWait(*cond, *mutex) < 0) ? -1 : 0;
}

int pthread_cond_signal(pthread_cond_t *cond)
{
    return (LWP_CondSignal(*cond) < 0) ? -1 : 0;
}


//
// <semaphore.h>
//

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


//
// <unistd.h>
//

#define WII_PAGE_SIZE 1024
#define WII_CPU_NUM 1

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
