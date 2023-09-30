/*
	Writen by: Oscar Bergström
	https://github.com/OSCARJFB

	MIT License
	Copyright (c) 2023 Oscar Bergström
*/

#ifndef EDITORMODE_H
#define EDITORMODE_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <ncurses.h>
#include <signal.h>
#include "textData.h"
#include "fileHandler.h"
#include "allocHandler.h"
#include "copy.h"

void *createNodesFromBuffer(char *buffer, long fileSize);
void runApp(TEXT *headNode, char *fileName);

#endif // EDITORMODE_H
