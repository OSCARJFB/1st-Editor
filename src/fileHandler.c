/*
	Writen by: Oscar Bergström
	https://github.com/OSCARJFB

	MIT License
	Copyright (c) 2023 Oscar Bergström
*/

#include "fileHandler.h"

static FILE *getFileFromArg(int argc, char **argv);
static FILE *getFile(const char *path);
static void closeFile(FILE *fp);
static long getFileSize(FILE *fp);
static char *allocateBuffer(int fileSize);
static void freeBuffer(char *buffer);
static void loadBuffer(char *buffer, FILE *fp, long fileSize);

static FILE *getFileFromArg(int argc, char **argv)
{
	if (argc < 2)
	{
		return NULL;
	}

	FILE *fp = fopen(argv[1], "r");
	if (fp == NULL)
	{
		return NULL;
	}

	return fp;
}

static FILE *getFile(const char *path)
{
	FILE *fp = fopen(path, "r");
	if (fp == NULL)
	{
		return NULL;
	}

	return fp;
}

static void closeFile(FILE *fp)
{
	if(fp == NULL)
	{
		return; 
	}
	
	fclose(fp);
	fp = NULL;
}

static long getFileSize(FILE *fp)
{
	if(fp == NULL)
	{
		return 0; 
	}
	
	long bufferSize = 0;
	if (fseek(fp, 0, SEEK_END) == -1)
	{
		return -1;
	}

	bufferSize = ftell(fp);
	if (bufferSize == -1)
	{
		return -1;
	}

	return bufferSize;
}

static char *allocateBuffer(int fileSize)
{
	if(fileSize == 0 || fileSize == -1)
	{
		return NULL; 
	}

	char *buffer = memAlloc(malloc(fileSize), fileSize);
	return buffer;
}

static void freeBuffer(char *buffer)
{
	if (buffer == NULL)
	{
		return;
	}

	free(buffer);
	buffer = NULL;
}

static void loadBuffer(char *buffer, FILE *fp, long fileSize)
{
	if(buffer == NULL)
	{
		return;
	}
	
	rewind(fp);
	while (fread(buffer, fileSize, 1, fp) > 0)
	{
	};
}

static void curseMode(bool isCurse)
{
	if (isCurse)
	{
		initscr();
		cbreak();
		noecho();
		curs_set(1);
		keypad(stdscr, TRUE);
	}
	else
	{
		endwin();
	}
}

/**
 * This function is very similar to startUp.
 * It is used when loading a new file.
 */
void *reStart(char *fileName)
{
	FILE *fp = getFile(fileName);
	long fileSize = getFileSize(fp);

	char *buffer = allocateBuffer(fileSize);
	loadBuffer(buffer, fp, fileSize);
	createNodesFromBuffer(buffer, fileSize);

	void *newHeadNode = createNodesFromBuffer(buffer, fileSize);
	freeBuffer(buffer);
	return newHeadNode;
}

/**
 * This function will call necessary operations to start editing of a file. 
 * It will try to open a file if specified in the arguments, it will allocate a buffer loading the data. 
 * Once done it will create a list ready for editing.  
 */
void startUp(int argc, char **argv)
{
	allocateBackUp();
	FILE *fp = getFileFromArg(argc, argv);
	long fileSize = getFileSize(fp);
	char *buffer = allocateBuffer(fileSize);
	loadBuffer(buffer, fp, fileSize);
	closeFile(fp);
	void *headNode = createNodesFromBuffer(buffer, fileSize);
	freeBuffer(buffer);
	curseMode(true);
	runApp(headNode, argv[1]);
	curseMode(false);
}
