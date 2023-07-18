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
	char *cpy_List;
	coordinates cpy_start, cpy_end;
	bool isStart, isEnd;
} dataCopied;

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
	MARGIN_SPACE_3 = 3,
	MARGIN_SPACE_4 = 4,
	MARGIN_SPACE_5 = 5,
	MARGIN_SPACE_6 = 6,
};

enum mode
{
	EDIT = 0,
	SAVE = 1,
	COPY = 2,
	PASTE = 3,
	EXIT = 4
};

enum state
{
	ADD_FIRST_NODE = 0, 
	ADD_MIDDLE_NODE = 1,
	ADD_END_NODE = 2,
	DEL_NODE = 3,
	DEL_AT_END = 4
};

extern int _leftMargin;
extern int _rightMargin;
extern int _tabSize;
extern int _copySize;
extern int _viewStart;

TEXT *createNodesFromBuffer(char *buffer, long fileSize);
TEXT *createNewNode(int ch);
coordinates onEditCoordinates(coordinates xy, int sFlag, int ch, TEXT *last_node);
coordinates addNode(TEXT **head, int ch, coordinates xy);
coordinates deleteNode(TEXT **head, coordinates xy);
coordinates getEndNodeCoordinates(TEXT *head);
dataCopied getCopyStart(dataCopied cp_data, coordinates xy);
dataCopied getCopyEnd(dataCopied cp_data, coordinates xy);
char *saveCopiedText(TEXT *head, coordinates cp_start, coordinates cp_end);
void pasteCopiedlist(TEXT **head, char *cpy_List, coordinates xy);
void save(TEXT *head, char *fileName);
int getFileSizeFromList(TEXT *head);
char *saveListToBuffer(TEXT *head, int size);
char *newFileName(void);
void deleteAllNodes(TEXT *head);
void updateCoordinatesInView(TEXT **head);
int countNewLines(TEXT *head);
void setLeftMargin(TEXT *head);
void printNodes(TEXT *head);
int setMode(int ch);
coordinates moveArrowKeys(int ch, coordinates xy);
coordinates edit(TEXT **head, coordinates xy, int ch);
dataCopied copy(dataCopied cpy_data, TEXT *head, coordinates xy);
void editTextFile(TEXT *head, char *fileName);

#endif // EDITORMODE_H