#include "allocHandler.h"

char *_backUpBuffer = NULL; 


/** 
 * This function will allocate and store a backup. 
 * The backup is used if malloc fails which freeing it might yield some empty paging.
 */
void allocateBackUp(void)
{
	const int backUpSize = 100;
	_backUpBuffer = malloc(backUpSize);
	if(_backUpBuffer == NULL)
	{
		perror("Memory allocation failed | No backup buffer was created\n");
	}
}


/**
 * Assert the return value from  memory allocation from functions like malloc. 
 * Check if the returned void pointer is NULL. If true the application will exit with error code 1.
 * Else this function will return the allocated void pointer.
 */
void *memAlloc(void *mem, int size)
{
	if(mem == NULL)
	{
		const int retries = 5;
		if(_backUpBuffer != NULL)
		{
			free(_backUpBuffer);
			_backUpBuffer = NULL;
		}

		for(int i = 0; i < retries; ++i)
		{
			mem = malloc(size);
			if(mem != NULL)
			{
				goto success;
			}
		}

		endwin(); 
		perror("Memory allocation failed, this was retry 5 | Critical error | application will exit with return code 1\n");
		exit(1);
	}

success:
	return mem; 
}
