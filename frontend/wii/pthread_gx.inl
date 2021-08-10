/* Copyright  (C) 2010-2020 The RetroArch team
 *
 * ---------------------------------------------------------------------------------------
 * The following license statement only applies to this file (gx_pthread.h).
 * ---------------------------------------------------------------------------------------
 *
 * Permission is hereby granted, free of charge,
 * to any person obtaining a copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,
 * INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include <ogcsys.h>
#include <gccore.h>
#include <ogc/cond.h>

#ifndef OSThread
#define OSThread lwp_t
#endif

#ifndef OSCond
#define OSCond lwpq_t
#endif

#ifndef OSThreadQueue
#define OSThreadQueue lwpq_t
#endif

#ifndef OSInitMutex
#define OSInitMutex(mutex) LWP_MutexInit(mutex, 0)
#endif

#ifndef OSLockMutex
#define OSLockMutex(mutex) LWP_MutexLock(mutex)
#endif

#ifndef OSUnlockMutex
#define OSUnlockMutex(mutex) LWP_MutexUnlock(mutex)
#endif

#ifndef OSTryLockMutex
#define OSTryLockMutex(mutex) LWP_MutexTryLock(mutex)
#endif

#ifndef OSInitCond
#define OSInitCond(cond) LWP_CondInit(cond)
#endif

#ifndef OSWaitCond
#define OSWaitCond(cond, mutex) LWP_CondWait(cond, mutex)
#endif

#ifndef OSInitThreadQueue
#define OSInitThreadQueue(queue) LWP_InitQueue(queue)
#endif

#ifndef OSSleepThread
#define OSSleepThread(queue) LWP_ThreadSleep(queue)
#endif

#ifndef OSJoinThread
#define OSJoinThread(thread, val) LWP_JoinThread(thread, val)
#endif

#ifndef OSCreateThread
#define OSCreateThread(thread, \
   func, intarg, ptrarg, stackbase, stacksize, priority, attrs) \
   LWP_CreateThread(thread, func, ptrarg, stackbase, stacksize, priority)
#endif

#define STACKSIZE (8 * 1024)

typedef OSThread pthread_t;
typedef mutex_t pthread_mutex_t;
typedef OSCond pthread_cond_t;

#if defined(GX_PTHREAD_LEGACY)
typedef void* pthread_mutexattr_t;
typedef int pthread_attr_t;
typedef OSCond pthread_condattr_t;
#endif

int pthread_create(pthread_t *thread,
      const pthread_attr_t *attr, void *(*start_routine)(void*), void *arg)
{
   *thread = 0;
   return OSCreateThread(/* TODO types*/thread, start_routine, 0 /* unused */, arg,
         0, STACKSIZE, 64, 0 /* unused */);
}

pthread_t pthread_self(void)
{
   /* zero 20-mar-2016: untested */
   return LWP_GetSelf();
}

int pthread_mutex_init(pthread_mutex_t *mutex,
      const pthread_mutexattr_t *attr)
{
   return OSInitMutex(mutex);
}

int pthread_mutex_destroy(pthread_mutex_t *mutex)
{
   return LWP_MutexDestroy(*mutex);
}

int pthread_mutex_lock(pthread_mutex_t *mutex)
{
   return OSLockMutex(*mutex);
}

int pthread_mutex_unlock(pthread_mutex_t *mutex)
{
   return OSUnlockMutex(*mutex);
}

void pthread_exit(void *retval)
{
   /* FIXME: No LWP equivalent for this? */
   (void)retval;
}

int pthread_detach(pthread_t thread)
{
   /* FIXME: pthread_detach equivalent missing? */
   (void)thread;
   return 0;
}

int pthread_join(pthread_t thread, void **retval)
{
   return OSJoinThread(thread, retval);
}

int pthread_mutex_trylock(pthread_mutex_t *mutex)
{
   return OSTryLockMutex(*mutex);
}

int pthread_cond_wait(pthread_cond_t *cond,
      pthread_mutex_t *mutex)
{
   return OSWaitCond(*cond, *mutex);
}

int pthread_cond_timedwait(pthread_cond_t *cond,
      pthread_mutex_t *mutex, const struct timespec *abstime)
{
   return LWP_CondTimedWait(*cond, *mutex, abstime);
}

int pthread_cond_init(pthread_cond_t *cond,
      const pthread_condattr_t *attr)
{
   return OSInitCond(cond);
}

int pthread_cond_signal(pthread_cond_t *cond)
{
   return LWP_CondSignal(*cond);
}

int pthread_cond_broadcast(pthread_cond_t *cond)
{
   return LWP_CondBroadcast(*cond);
}

int pthread_cond_destroy(pthread_cond_t *cond)
{
   return LWP_CondDestroy(*cond);
}

extern int pthread_equal(pthread_t t1, pthread_t t2);
