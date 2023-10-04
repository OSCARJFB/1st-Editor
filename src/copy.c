/*
   	Writen by: Oscar Bergström
	https://github.com/OSCARJFB

	MIT License
	Copyright (c) 2023 Oscar Bergström
*/

#include "copy.h"

static inline  dataCopied startPoint(dataCopied cpyData, coordinates xy);
static inline dataCopied endPoint(dataCopied cpyData, coordinates xy);
static dataCopied saveCopiedText(TEXT *headNode, dataCopied cpyData);

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
static dataCopied saveCopiedText(TEXT *headNode, dataCopied cpyData)
{
	const int bufferSize = 1000;
	int currentSize = 0;
	bool start_found = false;

	// Swap coordinates if text selection was done backwards.
	if (cpyData.cpyStart.y > cpyData.cpyEnd.y || (cpyData.cpyStart.y == cpyData.cpyEnd.y && cpyData.cpyStart.x > cpyData.cpyEnd.x))
	{
		coordinates temp = cpyData.cpyStart;
		cpyData.cpyStart = cpyData.cpyEnd;
		cpyData.cpyEnd = temp;
	}

	// Create a buffer. 
	while (headNode != NULL)
	{
		// Start were copy point is found, add every node until the end of the list is found.
		if ((headNode->x == cpyData.cpyStart.x && headNode->y == cpyData.cpyStart.y) || start_found)
		{
			if (cpyData.copiedList == NULL)
			{
				cpyData.copiedList = memAlloc(malloc(bufferSize * sizeof(char)), bufferSize * sizeof(char));
				start_found = true;
			}

			if (currentSize < bufferSize)
			{
				cpyData.copiedList[currentSize++] = headNode->ch;
			}
		}

		// If true end of list was found.
		if (headNode->x == cpyData.cpyEnd.x && headNode->y == cpyData.cpyEnd.y)
		{
			break;
		}

		headNode = headNode->next;
	}
	
	// Set the end size of the buffer list. 
	cpyData.copySize = currentSize;
	return cpyData;
}

/**
 * Paste and line items to the TEXT list. 
 * Items will be pasted between xy -> xy. 
 */
void paste(TEXT **headNode, dataCopied cpyData, coordinates xy)
{
	if (*headNode == NULL || cpyData.copiedList == NULL)
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

	// Create and chain each new node from the copy buffer.
	TEXT *postList = preList->next;
	for (int i = 0; i < cpyData.copySize; ++i)
	{
		TEXT *new_node = memAlloc(malloc(sizeof(TEXT)), sizeof(TEXT));
		
		new_node->ch = cpyData.copiedList[i];
		preList->next = new_node;
		new_node->prev = preList;
		preList = preList->next;
	}

	// If any part of the list in other words, we're not at the end of the list, chain the list together.
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
	if(cpyData.copiedList != NULL)
	{
		free(cpyData.copiedList);
		cpyData.copiedList = NULL;
	}

	cpyData = startPoint(cpyData, xy);
	cpyData = endPoint(cpyData, xy);

	if(!cpyData.isStart && !cpyData.isEnd)
	{
		cpyData = saveCopiedText(headNode, cpyData);
	}

	return cpyData;
}

/**
 * This function requests a start and end location in the terminal.
 * Finally it will save the coordinates found between these characters and also remove the from the main list.
 */
dataCopied cut(dataCopied cpyData, TEXT **headNode, coordinates xy)
{
	if(cpyData.copiedList != NULL)
	{
		free(cpyData.copiedList);
		cpyData.copiedList = NULL;
	}

	cpyData = startPoint(cpyData, xy);
	cpyData = endPoint(cpyData, xy);

	if(!cpyData.isStart && !cpyData.isEnd)
	{
		cpyData = saveCopiedText(*headNode, cpyData);
		deleteCpyList(cpyData, headNode); 
	}

	return cpyData;
}
