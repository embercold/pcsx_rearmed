#ifndef WII_UNISTD_H
#define WII_UNISTD_H

#include <sys/types.h>

// typedef __useconds_t useconds_t;

#define _SC_PAGE_SIZE 1
#define _SC_NPROCESSORS_ONLN 2

long sysconf(int name);
int usleep(useconds_t usec);

#endif
