#ifndef WII_SYS_MMAN_H
#define WII_SYS_MMAN_H

#include <stdlib.h>
#include <sys/types.h>

#define MAP_PRIVATE 1
#define MAP_ANON 2
#define MAP_FIXED 4

#define PROT_READ 1
#define PROT_WRITE 2
#define PROT_EXEC 4

#define MAP_FAILED ((void *) -1)

void *mmap(void *addr, size_t length,
    int prot, int flags, int fd, off_t offset);
int munmap(void *addr, size_t length);
int mprotect(void *addr, size_t len, int prot);

#endif
