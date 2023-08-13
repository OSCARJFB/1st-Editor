/*
    Writen by: Oscar Bergström
    https://github.com/OSCARJFB

    MIT License
    Copyright (c) 2023 Oscar Bergström
*/

#ifndef FILEHANDLER_H
#define FILEHANDLER_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "allocHandler.h"
#include "editorMode.h"

void *reStart(char *fileName);
void startUp(int argc, char **argv);

#endif // FILEHANDLER_H
