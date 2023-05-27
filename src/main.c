/*
    Writen by: Oscar Bergström
    https://github.com/OSCARJFB

	MIT License
	Copyright (c) 2023 Oscar Bergström
*/

#include <stdio.h>
#include <stdlib.h>
#include "editorList.h"

FILE *getFileFromArg(int argc, char **argv)
{
	if (argc < 2)
	{
		return NULL;
	}

	FILE *FP = fopen(argv[1], "r");
	if (FP == NULL)
	{
		printf("getFileFromArg: Couldn't read File at path %s\n", argv[1]);
		return NULL;
	}

	return FP;
}

FILE *getFile(const char *path)
{
	FILE *FP = fopen(path, "a");
	if (FP == NULL)
	{
		printf("getFile: Couldn't read File at path %s\n", path);
		return NULL;
	}

	return FP;
}

void closeFile(FILE *FP)
{
	fclose(FP);
	FP = NULL;
}

int getFileSize(FILE *FP)
{
	long bufferSize = 0;
	if (fseek(FP, 0, SEEK_END) == -1)
	{
		puts("getFileSize: Couldn't get file size");
		return -1;
	}

	bufferSize = ftell(FP);
	if (bufferSize == -1)
	{
		puts("getFileSize: Couldn't get file size");
		return -1;
	}

	return bufferSize;
}

char *allocateBuffer(char *buffer, int fileSize)
{
	buffer = malloc(fileSize);
	if (buffer == NULL)
	{
		puts("allocateBuffer: malloc() error");
		return NULL;
	}

	return buffer;
}

void freeBuffer(char *buffer)
{
	if (buffer == NULL)
	{
		puts("Buffer was NULL, no deallocation was needed");
		return;
	}

	free(buffer);
	buffer = NULL;
}

void loadBuffer(char *buffer, FILE *FP, long fileSize)
{
	rewind(FP);
	while (fread(buffer, fileSize, 1, FP) > 0)
	{
	};
}

int main(int argc, char **argv)
{
	system("clear"); // replace with ncurses here. 
	long fileSize = 0;
	char *buffer = NULL;
	bufList *head = NULL;

	FILE *FP = getFileFromArg(argc, argv);
	if (FP == NULL)
	{
		exit(-1);
	}

	fileSize = getFileSize(FP);
	if (fileSize == -1)
	{
		exit(-1);
	}

	buffer = allocateBuffer(buffer, fileSize);
	if (buffer == NULL)
	{
		exit(-1);
	}

	loadBuffer(buffer, FP, fileSize);

	head = createNodesFromBuffer(buffer, head, fileSize);
	if (head == NULL)
	{
		exit(-1);
	}

	closeFile(FP);
	editTextFile(head, argv[1]);
	freeBuffer(buffer);

	return 0;
}
