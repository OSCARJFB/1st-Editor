/*
	Writen by: Oscar Bergström
	https://github.com/OSCARJFB

	MIT License
	Copyright (c) 2023 Oscar Bergström
*/

#include "fileHandler.h"

FILE *getFileFromArg(int argc, char **argv)
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

FILE *getFile(const char *path)
{
	FILE *fp = fopen(path, "r");
	if (fp == NULL)
	{
		return NULL;
	}

	return fp;
}

void closeFile(FILE *fp)
{
	if(fp == NULL)
	{
		return; 
	}
	
	fclose(fp);
	fp = NULL;
}

long getFileSize(FILE *fp)
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

char *allocateBuffer(int fileSize)
{
	if(fileSize == 0 || fileSize == -1)
	{
		return NULL; 
	}

	char *buffer = malloc(fileSize);
	if (buffer == NULL)
	{
		return NULL;
	}

	return buffer;
}

void freeBuffer(char *buffer)
{
	if (buffer == NULL)
	{
		return;
	}

	free(buffer);
	buffer = NULL;
}

void loadBuffer(char *buffer, FILE *fp, long fileSize)
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

void startUp(int argc, char **argv)
{
	// Set the file pointer according to args provided. Check the size of the file provided. 
	FILE *fp = getFileFromArg(argc, argv);
	long fileSize = getFileSize(fp);
	
	// Depending on the size allocate a buffer storing the file, then close it. 
	char *buffer = allocateBuffer(fileSize);
	loadBuffer(buffer, fp, fileSize);
	closeFile(fp);

	// Load the buffer into a linked list, then free the buffer. 
	TEXT *head = createNodesFromBuffer(buffer, fileSize);
	freeBuffer(buffer);
	editTextFile(head, argv[1]);
}