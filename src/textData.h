/*
	Writen by: Oscar Bergström
	https://github.com/OSCARJFB

	MIT License
	Copyright (c) 2023 Oscar Bergström
*/

#ifndef TEXTDATA_H
#define TEXTDATA_H

#define ESC_KEY 27
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
	char *copiedList;
	coordinates cpyStart, cpyEnd;
	bool isStart, isEnd;
	int copySize;
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
	CUT,
	PASTE,
	OPEN_FILE,
	EXIT
};

#endif
