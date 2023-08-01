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
#include "fileHandler.h"

#define ESC_KEY 27
#define NO_KEY -1
#define CPY_BUFFER_SIZE 1000
#define FILENAME_SIZE 100

typedef struct coordinates
{
	int x, y;
} coordinates;

typedef struct TEXT
{
	int ch, x, y;
	struct TEXT *next;
	struct TEXT *prev;
} TEXT;

typedef struct dataCopied
{
	char *cpyList;
	coordinates cpyStart, cpyEnd;
	bool isStart, isEnd;
} dataCopied;

typedef struct textMargins
{
	int left, right, top, bottom;
} textMargins;

enum lineLimit
{
	LIM_1 = 10,
	LIM_2 = 100,
	LIM_3 = 1000,
	LIM_4 = 10000,
	LIM_5 = 100000,
};

enum marginSize
{
	MARGIN_SPACE_2 = 2,
	MARGIN_SPACE_3,
	MARGIN_SPACE_4,
	MARGIN_SPACE_5,
	MARGIN_SPACE_6,
};

enum mode
{
	EDIT,
	SAVE,
	COPY,
	PASTE,
	OPEN_FILE,
	EXIT
};

enum state
{
	ADD_FIRST_NODE,
	ADD_MIDDLE_NODE,
	ADD_END_NODE,
	DEL_NODE,
	DEL_AT_END
};

extern textMargins _margins;
extern int _tabSize;
extern int _copySize;
extern int _viewStart;
extern int _view;
extern long _fileSize;

void *createNodesFromBuffer(char *buffer, long fileSize);
void runApp(TEXT *headNode, char *fileName);

#endif // EDITORMODE_H