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

	FILE *FP = fopen(argv[1], "r");
	if (FP == NULL)
	{
		return NULL;
	}

	return FP;
}

FILE *getFile(const char *path)
{
	FILE *FP = fopen(path, "a");
	if (FP == NULL)
	{
		return NULL;
	}

	return FP;
}

void closeFile(FILE *FP)
{
	if(FP == NULL)
	{
		return; 
	}
	
	fclose(FP);
	FP = NULL;
}

int getFileSize(FILE *FP)
{
	if(FP == NULL)
	{
		return 0; 
	}
	
	long bufferSize = 0;
	if (fseek(FP, 0, SEEK_END) == -1)
	{
		return -1;
	}

	bufferSize = ftell(FP);
	if (bufferSize == -1)
	{
		return -1;
	}

	return bufferSize;
}

char *allocateBuffer(int fileSize)
{
	if(fileSize == 0)
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

void loadBuffer(char *buffer, FILE *FP, long fileSize)
{
	if(buffer == NULL)
	{
		return;
	}
	
	rewind(FP);
	while (fread(buffer, fileSize, 1, FP) > 0)
	{
	};
}

void startUp(int argc, char **argv)
{
	// Set the file pointer according to args provided. Check the size of the file provided. 
	FILE *FP = getFileFromArg(argc, argv);
	long fileSize = getFileSize(FP);
	
	// Depending on the size allocate a buffer storing the file, then close it. 
	char *buffer = allocateBuffer(fileSize);
	loadBuffer(buffer, FP, fileSize);
	closeFile(FP);

	// Load the buffer into a linked list, then free the buffer. 
	TEXT *head = createNodesFromBuffer(buffer, fileSize);
	freeBuffer(buffer);
	editTextFile(head, argv[1]);
}