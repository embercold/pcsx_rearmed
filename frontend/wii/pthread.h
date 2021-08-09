#ifndef WII_PTHREAD_H
#define WII_PTHREAD_H

#include <stdint.h>

#define PTHREAD_COND_INITIALIZER 0

// lwp_t, mutex_t and cond_t must be compatible to those on the host platforms
typedef uint32_t pthread_t;
typedef int32_t pthread_attr_t;
typedef uint32_t pthread_mutex_t;
typedef int32_t pthread_mutexattr_t;
typedef uint32_t pthread_cond_t;
typedef int32_t pthread_condattr_t;

// Threads

int pthread_create(pthread_t *restrict thread,
    const pthread_attr_t *restrict attr,
    void *(*start_routine)(void *),
    void *restrict arg);
void pthread_exit(void *retval);
int pthread_join(pthread_t thread, void **retval);

// Mutexes

#define PTHREAD_MUTEX_RECURSIVE 0x1

int pthread_mutex_init(pthread_mutex_t *mutex,
    const pthread_mutexattr_t *attr);
int pthread_mutex_destroy(pthread_mutex_t *mutex);
int pthread_mutex_lock(pthread_mutex_t *mutex);
int pthread_mutex_unlock(pthread_mutex_t *mutex);

// Condition variables

int pthread_cond_init(pthread_cond_t *cond,
    const pthread_condattr_t *attr);
int pthread_cond_destroy(pthread_cond_t *cond);
// pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
int pthread_cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex);
int pthread_cond_signal(pthread_cond_t *cond);

#endif
