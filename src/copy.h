/*
    Writen by: Oscar Bergström
    https://github.com/OSCARJFB

    MIT License
    Copyright (c) 2023 Oscar Bergström
*/

#ifndef COPY_H
#define COPY_H

#include <stdio.h> 
#include <stdbool.h>
#include "textData.h"
#include "allocHandler.h"

void paste(TEXT **headNode, char *cpyList, coordinates xy);
dataCopied copy(dataCopied cpyData, TEXT *headNode, coordinates xy);
dataCopied cut(dataCopied cpyData, TEXT **headNode, coordinates xy);

#endif
