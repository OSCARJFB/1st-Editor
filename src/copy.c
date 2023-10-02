/*
   Writen by: Oscar Bergström
https://github.com/OSCARJFB

MIT License
Copyright (c) 2023 Oscar Bergström
*/

#include "copy.h"

int _copySize = 0;

static inline  dataCopied startPoint(dataCopied cpyData, coordinates xy);
static inline dataCopied endPoint(dataCopied cpyData, coordinates xy);
static char *saveCopiedText(TEXT *headNode, coordinates cpyStart, coordinates cpyEnd);

/**
 * This will set the start coordinate.
 */
static inline dataCopied startPoint(dataCopied cpyData, coordinates xy)
{
	if (cpyData.isStart)
	{
		return cpyData;
	}

	cpyData.cpyStart.x = xy.x;
	cpyData.cpyStart.y = xy.y;
	cpyData.isStart = true;

	return cpyData;
}

/**
 *  This will set the end coordinate. 
 */
static inline dataCopied endPoint(dataCopied cpyData, coordinates xy)
{
	if (cpyData.isStart && cpyData.isEnd)
	{
		cpyData.cpyEnd.x = xy.x;
		cpyData.cpyEnd.y = xy.y;
		cpyData.isStart = cpyData.isEnd = false;
	}

	if (cpyData.isStart)
	{
		cpyData.isEnd = true;
	}

	return cpyData;
}

/** 
 * Deletes the copied list, 
 * this is done when using the cut operation. 
 */
static void deleteCpyList(dataCopied cpyData, TEXT **headNode)
{
	TEXT *node = *headNode, *startNode = NULL, *endNode = NULL, *del = NULL; 

	while(node != NULL)
	{
		if(node->x == cpyData.cpyStart.x && node->y == cpyData.cpyStart.y)	
		{
			if(node->prev != NULL)
			{
				startNode = node->prev; 
			}
			break;
		}
		node = node->next; 
	}

	while(node != NULL)
	{
		if(node->x == cpyData.cpyEnd.x && node->y == cpyData.cpyEnd.y)	
		{
			if(node->next != NULL)
			{
				endNode = node->next; 
			}
			break;
		}
	
		del = node; 
		node = node->next;
	      	free(del);
		del = NULL; 	
	}

	if(endNode != NULL && startNode != NULL)
	{
		endNode->prev = startNode; 
		startNode->next = endNode;
	}
	else if(endNode != NULL && startNode == NULL)
	{
		*headNode = endNode; 
	}
	else if(endNode == NULL && startNode != NULL)
	{
		*headNode = startNode; 
	}
}

/**
 * This function will save and create a list of data between two coordinate points.
 * It will check xy -> xy and then allocate an array of data store any values between those points.
 */
static char *saveCopiedText(TEXT *headNode, coordinates cpyStart, coordinates cpyEnd)
{
	char *cpyList = NULL;
	int i = 0;
	bool start_found = false;

	if (cpyStart.y > cpyEnd.y || (cpyStart.y == cpyEnd.y && cpyStart.x > cpyEnd.x))
	{
		coordinates temp = cpyStart;
		cpyStart = cpyEnd;
		cpyEnd = temp;
	}

	while (headNode != NULL)
	{
		// Start were copy point is found, add every node until the end of the list is found.
		if (((headNode->x == cpyStart.x && headNode->y == cpyStart.y) || start_found))
		{
			if (cpyList == NULL)
			{
				cpyList = memAlloc(malloc(CPY_BUFFER_SIZE * sizeof(char)), CPY_BUFFER_SIZE * sizeof(char));
				start_found = true;
			}

			if (i < CPY_BUFFER_SIZE)
			{
				cpyList[i++] = headNode->ch;
			}
		}

		// If true end of list was found.
		if (headNode->x == cpyEnd.x && headNode->y == cpyEnd.y)
		{
			_copySize = i;
			break;
		}

		headNode = headNode->next;
	}

	return cpyList;
}

/**
 * Paste and line items to the TEXT list. 
 * Items will be pasted between xy -> xy. 
 */
void paste(TEXT **headNode, char *cpyList, coordinates xy)
{
	if (*headNode == NULL || cpyList == NULL)
	{
		return;
	}

	// First find the paste start location should.
	TEXT *preList = *headNode;
	for (; preList->next != NULL; preList = preList->next)
	{
		if (preList->x == xy.x && preList->y == xy.y)
		{
			break;
		}
	}

	// Last character should not be a newline.
	if (preList->ch == '\n')
	{
		preList = preList->prev;
	}

	TEXT *postList = preList->next;

	// Create and chain each new node from the copy buffer.
	for (int i = 0; i < _copySize; ++i)
	{
		TEXT *new_node = memAlloc(malloc(sizeof(TEXT)), sizeof(TEXT));
		new_node->ch = cpyList[i];
		preList->next = new_node;
		new_node->prev = preList;
		preList = preList->next;
	}

	// If any part of the list is remaining chain it to the new list.
	if (postList != NULL)
	{
		preList->next = postList;
		postList->prev = preList;
	}
}

/**
 * This function requests a start and end location in the terminal.
 * Finally it will save the sub list found between these coordinates.
 */
dataCopied copy(dataCopied cpyData, TEXT *headNode, coordinates xy)
{
	if(cpyData.cpyList != NULL)
	{
		free(cpyData.cpyList);
		cpyData.cpyList = NULL;
	}

	cpyData = startPoint(cpyData, xy);
	cpyData = endPoint(cpyData, xy);

	if(!cpyData.isStart && !cpyData.isEnd)
	{
		cpyData.cpyList = saveCopiedText(headNode, cpyData.cpyStart, cpyData.cpyEnd);
	}

	return cpyData;
}

/**
 * This function requests a start and end location in the terminal.
 * Finally it will save the coordinates found between these characters and also remove the from the main list.
 */
dataCopied cut(dataCopied cpyData, TEXT **headNode, coordinates xy)
{
	if(cpyData.cpyList != NULL)
	{
		free(cpyData.cpyList);
		cpyData.cpyList = NULL;
	}

	cpyData = startPoint(cpyData, xy);
	cpyData = endPoint(cpyData, xy);

	if(!cpyData.isStart && !cpyData.isEnd)
	{
		cpyData.cpyList = saveCopiedText(*headNode, cpyData.cpyStart, cpyData.cpyEnd);
		deleteCpyList(cpyData, headNode); 
	}

	return cpyData;
}
