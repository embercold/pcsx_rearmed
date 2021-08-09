/* Copyright  (C) 2010-2016 The RetroArch team
 *
 * ---------------------------------------------------------------------------------------
 * The following license statement only applies to this file (wiiu_pthread.h).
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

#include <wiiu/os/condition.h>
#include <wiiu/os/thread.h>
#include <wiiu/os/mutex.h>
#include <malloc.h>
#define STACKSIZE (8 * 1024)

// typedef OSThread* pthread_t;
// typedef OSMutex* pthread_mutex_t;
// typedef void* pthread_mutexattr_t;
// typedef int pthread_attr_t;
// typedef OSCondition* pthread_cond_t;
// typedef OSCondition* pthread_condattr_t;

int pthread_create(pthread_t *thread,
      const pthread_attr_t *attr, void *(*start_routine)(void*), void *arg)
{
   OSThread *t = memalign(8, sizeof(OSThread));
   void *stack = memalign(32, STACKSIZE);
   bool ret = OSCreateThread((OSThread *)t, (OSThreadEntryPointFn)start_routine, 
      (uint32_t)arg, NULL, (void*)(((uint32_t)stack)+STACKSIZE), STACKSIZE, 8, OS_THREAD_ATTRIB_AFFINITY_ANY);
   if(ret == true)
   {
      OSResumeThread((OSThread *)t);
      *thread = (pthread_t)t;
   }
   else
      *thread = (pthread_t)NULL;
   return (ret == true) ? 0 : -1;
}

pthread_t pthread_self(void)
{
   return (pthread_t)OSGetCurrentThread();
}

int pthread_mutex_init(pthread_mutex_t *mutex,
      const pthread_mutexattr_t *attr)
{
   OSMutex *m = malloc(sizeof(OSMutex));
   OSInitMutex((OSMutex *)m);
   *mutex = (pthread_mutex_t)m;
   return 0;
}

int pthread_mutex_destroy(pthread_mutex_t *mutex)
{
   if(*mutex)
      free((OSMutex *)*mutex);
   *mutex = (pthread_mutex_t)NULL;
   return 0;
}

int pthread_mutex_lock(pthread_mutex_t *mutex)
{
   OSLockMutex((OSMutex *)*mutex);
   return 0;
}

int pthread_mutex_unlock(pthread_mutex_t *mutex)
{
   OSUnlockMutex((OSMutex *)*mutex);
   return 0;
}

void pthread_exit(void *retval)
{
   (void)retval;
   OSExitThread(0);
}

int pthread_detach(pthread_t thread)
{
   OSDetachThread((OSThread *)thread);
   return 0;
}

int pthread_join(pthread_t thread, void **retval)
{
   (void)retval;
   bool ret = OSJoinThread((OSThread *)thread, NULL);
   if(ret == true)
   {
      free(((OSThread *)thread)->stackEnd);
      free((OSThread *)thread);
   }
   return (ret == true) ? 0 : -1;
}

int pthread_mutex_trylock(pthread_mutex_t *mutex)
{
   return OSTryLockMutex((OSMutex *)*mutex);
}

int pthread_cond_wait(pthread_cond_t *cond,
      pthread_mutex_t *mutex)
{
   OSWaitCond((OSCondition *)*cond, (OSMutex *)*mutex);
   return 0;
}

int pthread_cond_timedwait(pthread_cond_t *cond,
      pthread_mutex_t *mutex, const struct timespec *abstime)
{
   //FIXME: actual timeout needed
   (void)abstime;
   return pthread_cond_wait(cond, mutex);
}

int pthread_cond_init(pthread_cond_t *cond,
      const pthread_condattr_t *attr)
{
   OSCondition *c = malloc(sizeof(OSCondition));
   OSInitCond(c);
   *cond = (pthread_cond_t)c;
   return 0;
}

int pthread_cond_signal(pthread_cond_t *cond)
{
   OSSignalCond((OSCondition *)*cond);
   return 0;
}

int pthread_cond_broadcast(pthread_cond_t *cond)
{
   //FIXME: no OS equivalent
   (void)cond;
   return 0;
}

int pthread_cond_destroy(pthread_cond_t *cond)
{
   if(*cond)
      free((OSCondition *)*cond);
   *cond = (pthread_cond_t) NULL;
   return 0;
}

extern int pthread_equal(pthread_t t1, pthread_t t2);
