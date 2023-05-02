/*
    Writen by: Oscar Bergström
    https://github.com/OSCARJFB
*/

#include "textList.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <ncurses.h>

bufList *createNodesFromBuffer(char *buffer, bufList *head, long fileSize)
{
	int x = 0, y = 0;

	for (int i = 0; i < fileSize; ++i)
	{
		addNode(&head, buffer[i],
				x, y);

		if (buffer[i] == '\n')
		{
			++y;
			x = 0;
			continue;
		}

		++x;
	}

	return head;
}

void deleteAllNodes(bufList *head)
{
	bufList *temp = NULL;
	while (head != NULL)
	{
		temp = head;
		head = head->next;
		free(temp);
	}

	temp = NULL;
	head = NULL;
}

void updateXYNodesAdd(bufList **head, int *x, int *y)
{
}

void updateXYNodesDel(bufList **head, int *x, int *y)
{
	int lx = (*head)->x;
	int ly = (*head)->y;

	// Set cursor position.
	if((*head)->prev != NULL)
	{
		*x = (*head)->prev->x;
		*y = (*head)->prev->y;
	}

	// Newline character
	if ((*head)->ch == '\n')
	{
		lx = (*head)->x;
		for (*head = (*head)->next; *head != NULL; *head = (*head)->next)
		{
			if ((*head)->ch == '\n')
			{
				++ly;
				lx = -1;
				for (bufList *line_node = (*head)->next;
					 line_node != NULL && line_node->y == ly;
					 line_node = line_node->next)
				{
					++lx;
				}
			}

			(*head)->y = ly;
			(*head)->x = lx;
			++lx;
		}
		
		return;
	}

	// Is on the same line
	for (*head = (*head)->next;
		 *head != NULL && (*head)->ch != '\n';
		 *head = (*head)->next)
	{
		(*head)->x = lx;
		(*head)->y = ly;
		++lx;
	}

	*x = lx + 1;
	*y = ly;
}

void addNode(bufList **head, int ch, 
			 int x, int y)
{
	if (*head == NULL)
	{
		*head = malloc(sizeof(bufList));
		if (*head == NULL)
		{
			puts("addNode: malloc(), couldn't allocate memory for node");
			return;
		}

		(*head)->x = x;
		(*head)->y = y;
		(*head)->ch = ch;
		(*head)->next = NULL;
		(*head)->prev = NULL;

		return;
	}

	bufList *new_node = malloc(sizeof(bufList));
	if (new_node == NULL)
	{
		puts("addNode: malloc(), couldn't allocate memory for node");
		return;
	}

	new_node->x = x;
	new_node->y = y;
	new_node->ch = ch;
	new_node->next = NULL;
	new_node->prev = NULL;

	bufList *last_node = *head;
	bufList *prev_node = NULL;

	while (last_node->next != NULL)
	{
		last_node = last_node->next;
	}

	prev_node = last_node;
	last_node->next = new_node;
	new_node->prev = prev_node;
}

void deleteNode(bufList **head, int *x, int *y)
{
	// We can't free a node which is NULL.
	if (*head == NULL || (*x == 0 && *y == 0))
	{
		*x = *y = 0;
		return;
	}

	bool isEndNode = true;
	bufList *del_node = *head, *temp_node = NULL;

	// Find the node to be deleted.
	while (del_node->next != NULL)
	{
		// Is a node in the middle of the list.
		if (del_node->x == *x && del_node->y == *y)
		{
			del_node = del_node->prev;
			isEndNode = false;
			break;
		}

		// Is the node just before the last node in the list.
		if (del_node->next->x == *x && del_node->next->y == *y && del_node->next->next == NULL)
		{
			isEndNode = false;
			break;
		}

		del_node = del_node->next;
	}

	// If both prev and next are NULL this is the only node in the list.
	if (del_node->prev == NULL && del_node->next == NULL)
	{
		*x = *y = 0;
		free(*head);
		*head = NULL;
		return;
	}

	// Adjust the linking of nodes depending on it being the last node or a node in the middle of the list.
	if (isEndNode)
	{
		del_node->prev->next = NULL;
	}
	else if (!isEndNode)
	{
		if (del_node->prev == NULL && del_node->next != NULL)
		{
			temp_node = del_node;
			temp_node = temp_node->next;
			temp_node->prev = NULL;
			*head = temp_node;
			temp_node->x = del_node->x;
			temp_node->y = del_node->y;
			updateXYNodesDel(&temp_node, x, y); 
		}

		if (del_node->prev != NULL && del_node->next != NULL)
		{
			temp_node = del_node;
			temp_node->prev->next = temp_node->next;
			temp_node->next->prev = temp_node->prev;
			updateXYNodesDel(&temp_node, x, y);
		}
	}

	if (del_node == NULL)
	{
		return;
	}

	*x = del_node->x;
	*y = del_node->y;
	free(del_node);
	del_node = NULL;
}

void printNodes(bufList *head)
{
	clear();
	while (head != NULL)
	{
		mvwaddch(stdscr, head->y, head->x, head->ch);
		head = head->next;
	}
	refresh();
}

void getEndNodeCoordinates(bufList *head, int *x, int *y)
{
	while (head != NULL)
	{
		if (head->next == NULL)
		{
			break;
		}

		head = head->next;
	}

	*x = head->x + 1;
	*y = head->y;
}

#define ESC 0x1b
#define NUL 0x00

void editTextFile(bufList *head)
{
	int ch = 0x00, x = 0, y = 0;
	getEndNodeCoordinates(head, &x, &y);

	initscr();
	nodelay(stdscr, 1);
	curs_set(1);
	keypad(stdscr, 1);

	if (head != NULL)
	{
		printNodes(head);
	}

	while ((ch = getch()) != ESC)
	{
		if (ch > NUL)
		{
			switch (ch)
			{
			case KEY_UP:
				if (y != 0)
				{
					--y;
				}
				break;
			case KEY_DOWN:
				++y;
				break;
			case KEY_LEFT:
				if (x != 0)
				{
					--x;
				}
				break;
			case KEY_RIGHT:
				++x;
				break;
			case KEY_BACKSPACE:
				if (x == 0 && y == 0)
				{
					break;
				}
				deleteNode(&head, &x, &y);
				printNodes(head);
				break;
			default:
				addNode(&head, ch, 
						x, y);
				printNodes(head);

				++x;
				if (ch == '\n')
				{
					++y;
					x = 0;
				}
			}
			move(y, x);
		}
	}

	endwin();
}

// TEST FUNCTIONS

void testFunctionPrintAllNode(bufList *head)
{
	endwin();
	printf("\n\n");
	for (int i = 1; head != NULL; ++i)
	{
		printf("Item:%d ", i);
		if (head->next == NULL)
			printf("next == NULL");
		if (head->prev == NULL)
			printf("prev == NULL");

		printf("\n");

		head = head->next;
	}

	exit(1);
}
