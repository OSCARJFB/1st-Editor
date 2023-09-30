#ifndef TEXTDATA_H
#define TEXTDATA_H

#define ESC_KEY 27
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
	ADD_HEAD_NODE,
	ADD_MIDDLE_NODE,
	ADD_END_NODE,
	DEL_NODE,
	DEL_AT_END
};

#endif