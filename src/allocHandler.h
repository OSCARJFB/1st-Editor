#ifndef ALLOCHANDLER_H
#define ALLOCHANDLER_H

#include <stdlib.h>
#include <ncurses.h>
#include <error.h>

extern char *_backUpBuffer; 

void allocateBackUp(void);
void *memAlloc(void *mem, int size);

#endif //  ALLOCHANDLER_H
