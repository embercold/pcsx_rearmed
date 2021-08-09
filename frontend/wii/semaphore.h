#ifndef WII_SEMAPHORE_H
#define WII_SEMAPHORE_H

#include <stdint.h>

// Semaphores

// sem_t must be compatible to one on the host platform
typedef uint32_t sem_t;

int sem_init(sem_t *sem, int pshared, unsigned int value);
int sem_destroy(sem_t *sem);
int sem_post(sem_t *sem);
int sem_wait(sem_t *sem);

#endif
