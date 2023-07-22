/*
	Writen by: Oscar Bergström
	https://github.com/OSCARJFB

	MIT License
	Copyright (c) 2023 Oscar Bergström
*/

#include "editorMode.h"

int _leftMargin = MARGIN_SPACE_3;
int _rightMargin = 0;
int _tabSize = 6;
int _copySize = 0;
int _viewStart = 0;
long _fileSize = 0;

TEXT *createNodesFromBuffer(char *buffer, long fileSize)
{
	if (buffer == NULL)
	{
		return NULL;
	}

	coordinates xy = {0, 0};
	TEXT *head = NULL;

	// Add each character from the read file to the list.
	for (int i = 0; i < fileSize; ++i)
	{
		addNode(&head, buffer[i], xy);

		if (buffer[i] == '\n')
		{
			++xy.y;
			xy.x = 0;
			continue;
		}

		++xy.x;
	}

	updateCoordinatesInView(&head);
	return head;
}

long getFileSizeFromList(TEXT *head)
{
	long fileSize = 0;
	for (long i = 0; head != NULL; ++i)
	{
		head = head->next;
		if (head->next == NULL)
		{
			fileSize = i + 1;
			break;
		}
	}

	return fileSize;
}

void save(TEXT *head, char *fileName)
{
	FILE *fp = NULL;
	_fileSize = getFileSizeFromList(head);
	char *buffer = saveListToBuffer(head, _fileSize);
	if (buffer == NULL)
	{
		return;
	}

	if (fileName == NULL)
	{
		char *newName = newFileName();
		if (newName == NULL)
		{
			free(buffer);
			buffer = NULL;
			return;
		}

		fileName = malloc(sizeof(char) * FILENAME_SIZE);
		if (fileName == NULL)
		{
			free(buffer);
			buffer = NULL;
			return;
		}
		strcpy(fileName, newName);
	}

	fp = fopen(fileName, "w");

	if (fp != NULL)
	{
		fprintf(fp, "%s", buffer);
		fclose(fp);
		fp = NULL;
	}
	free(buffer);
	buffer = NULL;
}

char *saveListToBuffer(TEXT *head, long fileSize)
{
	if (fileSize == 0)
	{
		return NULL;
	}

	char *buffer = malloc((fileSize * sizeof(char)) + 1);
	if (buffer == NULL)
	{
		return NULL;
	}

	for (int i = 0; head != NULL && i < fileSize; head = head->next)
	{
		buffer[i++] = head->ch;
	}

	buffer[fileSize] = '\0';

	return buffer;
}

void saveOnFileChange(TEXT *head, char *fileName)
{
	long currentFileSize = getFileSizeFromList(head);
	if (currentFileSize == _fileSize)
	{
		return;
	}
	
	wclear(stdscr);
	printw("%s have been modified, would you like to save? (Y/N)", fileName);
	int ch = wgetch(stdscr);
	if(ch == 'y' || ch == 'Y')
	{
		if(fileName == NULL)
		{
			fileName = newFileName();
		}

		save(head, fileName);
	}
	wrefresh(stdscr);
}

char *newFileName(void)
{
	char *fileName = malloc(sizeof(char) * FILENAME_SIZE);
	if (fileName == NULL)
	{
		return NULL;
	}

	int index = 0;
	for (int ch = 0; ch != '\n' && index < FILENAME_SIZE; ch = wgetch(stdscr))
	{
		if (ch != '\0')
		{
			if (ch == KEY_BACKSPACE)
			{
				--index;
			}
			else
			{
				fileName[index++] = ch;
			}
		}

		wclear(stdscr);
		printw(": ");
		for (int i = 0; i < index; ++i)
		{
			printw("%c", fileName[i]);
		}
		wrefresh(stdscr);
	}

	fileName[index] = '\0';
	return fileName;
}

void deleteAllNodes(TEXT *head)
{
	if (head == NULL)
	{
		return;
	}

	// Delete and free every single node.
	TEXT *temp = NULL;
	while (head != NULL)
	{
		temp = head;
		head = head->next;
		free(temp);
	}

	temp = NULL;
	head = NULL;
}

TEXT *createNewNode(int ch)
{
	TEXT *next_node = malloc(sizeof(TEXT));
	if (next_node == NULL)
	{
		return NULL;
	}

	next_node->ch = ch;
	next_node->next = NULL;
	next_node->prev = NULL;
	return next_node;
}

coordinates onEditCoordinates(coordinates xy, int sFlag, int ch, TEXT *node)
{
	// Set cursor according to what type of edit was done.
	// This is used both when deleting and adding to the list.

	switch (sFlag)
	{
	case ADD_FIRST_NODE:
		xy.x = _leftMargin;
		xy.y += ch == '\n' ? 1 : 0;
		xy.x += ch == '\t' ? _tabSize : 0;
		break;
	case ADD_MIDDLE_NODE:
		xy.x = ch == '\n' ? _leftMargin : node->x + 1;
		xy.y += ch == '\n' ? 1 : 0;
		xy.x += ch == '\t' ? _tabSize : 0;
		break;
	case ADD_END_NODE:
		xy.x = ch == '\n' ? _leftMargin : node->ch == '\n' ? _leftMargin + 1
														   : node->x + 2;
		xy.y += ch == '\n' ? 1 : 0;
		xy.x += ch == '\t' ? _tabSize : 0;
		break;
	case DEL_NODE:
		xy.x = node->x;
		xy.y = node->y;
		break;
	case DEL_AT_END:
		xy.x = _leftMargin;
		xy.y = 0;
		break;
	}

	return xy;
}

coordinates addNode(TEXT **head, int ch, coordinates xy)
{
	// Currently there is no list existing.
	if (*head == NULL)
	{
		*head = createNewNode(ch);
		xy = onEditCoordinates(xy, ADD_FIRST_NODE, ch, NULL);
		return xy;
	}

	// Create a new node and add base values, depending on parameter input.
	TEXT *next_node = createNewNode(ch), *last_node = *head, *prev_node = NULL;

	// Find the last node in the list, for each step check if ch was added in between list bounderies.
	while (last_node->next != NULL)
	{
		if (last_node->x == xy.x && last_node->y == xy.y && last_node->prev == NULL)
		{
			last_node->prev = next_node;
			*head = next_node;
			next_node->next = last_node;

			xy = onEditCoordinates(xy, ADD_MIDDLE_NODE, ch, last_node);
			return xy;
		}

		last_node = last_node->next;

		if (last_node->x == xy.x && last_node->y == xy.y && last_node->prev != NULL)
		{
			last_node->prev->next = next_node;
			next_node->prev = last_node->prev;
			last_node->prev = next_node;
			next_node->next = last_node;

			xy = onEditCoordinates(xy, ADD_MIDDLE_NODE, ch, last_node);
			return xy;
		}
	}

	// Add the node at the end since ch was not added within the bounderies of the list.
	prev_node = last_node;
	last_node->next = next_node;
	next_node->prev = prev_node;

	xy = onEditCoordinates(xy, ADD_END_NODE, ch, last_node);
	return xy;
}

coordinates deleteNode(TEXT **head, coordinates xy)
{
	// We can't free/delete a node which is NULL or if at end of coordinates.
	if (*head == NULL || (xy.x == _leftMargin && xy.y == 0))
	{
		return xy;
	}

	bool isEndNode = true;
	TEXT *del_node = *head, *temp_node = NULL;

	// Find the node to be deleted.
	while (del_node->next != NULL)
	{
		// Is a node in the middle of the list.
		if (del_node->x == xy.x && del_node->y == xy.y)
		{
			del_node = del_node->prev;
			isEndNode = false;
			break;
		}

		// Is the node just before the last node in the list.
		if (del_node->next->x == xy.x && del_node->next->y == xy.y && del_node->next->next == NULL)
		{
			isEndNode = false;
			break;
		}

		del_node = del_node->next;
	}

	// If both prev and next are NULL this is the only node in the list.
	if (del_node->prev == NULL && del_node->next == NULL)
	{
		xy = onEditCoordinates(xy, DEL_AT_END, 0, NULL);
		free(*head);
		*head = NULL;
		return xy;
	}

	// Adjust the linking of nodes depending on it being the last node or a node in the middle of the list.
	if (isEndNode)
	{
		del_node->prev->next = NULL;
	}
	else if (!isEndNode)
	{
		temp_node = del_node;

		if (del_node->prev != NULL && del_node->next != NULL)
		{
			temp_node->prev->next = temp_node->next;
			temp_node->next->prev = temp_node->prev;
		}
		else if (del_node->prev == NULL && del_node->next != NULL)
		{
			temp_node->next->prev = NULL;
			*head = temp_node->next;
		}
	}

	if (del_node != NULL)
	{
		xy = onEditCoordinates(xy, DEL_NODE, 0, del_node);
		free(del_node);
		del_node = NULL;
	}

	return xy;
}

coordinates getEndNodeCoordinates(TEXT *head)
{
	updateCoordinatesInView(&head);
	coordinates xy = {0, 0};

	// Will find the last node in viewport and set its x and y value to be the cursor position.
	for (int i = 0; head != NULL && i < getmaxy(stdscr); ++i)
	{
		if (head->next == NULL)
		{
			break;
		}
		head = head->next;
	}

	if (head != NULL)
	{
		xy.x = head->x + 1;
		xy.y = head->y;
	}
	else
	{
		xy.x = _leftMargin;
		xy.y = 0;
	}

	return xy;
}

void updateCoordinatesInView(TEXT **head)
{
	if (*head == NULL)
	{
		return;
	}

	int x = _leftMargin, y = 0, newLines = 0;
	for (TEXT *node = *head; node != NULL; node = node->next)
	{
		if (newLines >= _viewStart)
		{
			node->x = x;
			node->y = y;

			if (node->ch == '\t')
			{
				x += _tabSize;
			}
			else
			{
				++x;
			}

			if (node->ch == '\n')
			{
				x = _leftMargin;
				++y;
			}
		}

		newLines += node->ch == '\n' ? 1 : 0;
		if (newLines >= getmaxy(stdscr) + _viewStart)
		{
			break;
		}
	}
}

dataCopied getCopyStart(dataCopied cpy_data, coordinates xy)
{
	if (cpy_data.isStart)
	{
		return cpy_data;
	}

	cpy_data.cpy_start.x = xy.x;
	cpy_data.cpy_start.y = xy.y;
	cpy_data.isStart = true;

	return cpy_data;
}

dataCopied getCopyEnd(dataCopied cpy_data, coordinates xy)
{
	if (cpy_data.isStart && cpy_data.isEnd)
	{
		cpy_data.cpy_end.x = xy.x;
		cpy_data.cpy_end.y = xy.y;
		cpy_data.isStart = cpy_data.isEnd = false;
	}

	if (cpy_data.isStart)
	{
		cpy_data.isEnd = true;
	}

	return cpy_data;
}

char *saveCopiedText(TEXT *head, coordinates cpy_start, coordinates cpy_end)
{
	char *cpy_List = NULL;
	int i = 0;
	bool start_found = false;

	if (cpy_start.y > cpy_end.y || (cpy_start.y == cpy_end.y && cpy_start.x > cpy_end.x))
	{
		coordinates temp = cpy_start;
		cpy_start = cpy_end;
		cpy_end = temp;
	}

	while (head != NULL)
	{
		// Start were copy point is found, add every node until the end of the list is found.
		if (((head->x == cpy_start.x && head->y == cpy_start.y) || start_found))
		{
			if (cpy_List == NULL)
			{
				cpy_List = malloc(CPY_BUFFER_SIZE * sizeof(char));
				if (cpy_List == NULL)
				{
					return NULL;
				}

				start_found = true;
			}

			if (i < CPY_BUFFER_SIZE)
			{
				cpy_List[i++] = head->ch;
			}
		}

		// If true end of list is found.
		if (head->x == cpy_end.x && head->y == cpy_end.y)
		{
			_copySize = i;
			break;
		}

		head = head->next;
	}

	return cpy_List;
}

void pasteCopiedlist(TEXT **head, char *cpy_List, coordinates xy)
{
	if (*head == NULL || cpy_List == NULL)
	{
		return;
	}

	// First find the paste start location should.
	TEXT *preList = *head;
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
		TEXT *new_node = createNewNode(cpy_List[i]);
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

int countNewLines(TEXT *head)
{
	int newlines = 0;

	// Count all the newlines found in the text.
	while (head != NULL)
	{
		if (head->ch == '\n')
		{
			++newlines;
		}

		head = head->next;
	}

	return newlines;
}

void setLeftMargin(TEXT *head)
{
	int newLines = countNewLines(head);

	// Set margin depending on the amount of newlines
	if (newLines < LIM_1)
	{
		_leftMargin = MARGIN_SPACE_3;
	}
	else if (newLines < LIM_2)
	{
		_leftMargin = MARGIN_SPACE_4;
	}
	else if (newLines < LIM_3)
	{
		_leftMargin = MARGIN_SPACE_5;
	}
	else if (newLines < LIM_4)
	{
		_leftMargin = MARGIN_SPACE_6;
	}
}

void printNodes(TEXT *head)
{
	int lineNumber = 0;
	bool nlFlag = true;

	// We need to clear terminal screen (empty) if no characters exists.
	if (head == NULL)
	{
		wclear(stdscr);
		printw("%d:", lineNumber + 1);
		wrefresh(stdscr);
		return;
	}

	// Print the nodes at x and y position.
	wclear(stdscr);
	for (TEXT *node = head; node != NULL; node = node->next)
	{
		if (lineNumber >= _viewStart)
		{
			if (nlFlag)
			{
				nlFlag = false;
				printw("%d:", lineNumber + 1);
			}

			mvwaddch(stdscr, node->y, node->x, node->ch);
		}

		if (node->ch == '\n')
		{
			nlFlag = true;
			++lineNumber;
		}

		if (lineNumber >= getmaxy(stdscr) + _viewStart)
		{
			break;
		}
	}

	if (nlFlag)
	{
		printw("%d:", lineNumber + 1);
	}

	wrefresh(stdscr);
}

int setMode(int ch)
{
	if (ch != ESC_KEY)
	{
		return EDIT;
	}

	ch = wgetch(stdscr);
	switch (ch)
	{
	case 's':
		return SAVE;
	case 'c':
		return COPY;
	case 'v':
		return PASTE;
	case 'o':
		return OPEN_FILE;
	case 'e':
		return EXIT;
	}

	return EDIT;
}

coordinates moveArrowKeys(int ch, coordinates xy)
{
	switch (ch)
	{
	case KEY_UP:
		xy.y += xy.y != 0 ? -1 : 0;
		break;
	case KEY_DOWN:
		++xy.y;
		break;
	case KEY_LEFT:
		xy.x += xy.x != _leftMargin ? -1 : 0;
		break;
	case KEY_RIGHT:
		++xy.x;
		break;
	}

	return xy;
}

coordinates edit(TEXT **head, coordinates xy, int ch)
{
	// If backspace is pressed delete a node at the current cursor location.
	// Else if ch is within the bounds of the condition add it in a new node.

	if (ch == KEY_BACKSPACE)
	{
		xy = deleteNode(head, xy);
	}
	else if ((ch >= ' ' && ch <= '~') || (ch == '\t' || ch == '\n'))
	{
		xy = addNode(head, ch, xy);
	}

	return xy;
}

dataCopied copy(dataCopied cpy_data, TEXT *head, coordinates xy)
{
	if (cpy_data.cpy_List != NULL)
	{
		free(cpy_data.cpy_List);
		cpy_data.cpy_List = NULL;
	}

	cpy_data = getCopyStart(cpy_data, xy);
	cpy_data = getCopyEnd(cpy_data, xy);

	if (!cpy_data.isStart && !cpy_data.isEnd)
	{
		cpy_data.cpy_List = saveCopiedText(head, cpy_data.cpy_start, cpy_data.cpy_end);
	}

	return cpy_data;
}

void updateViewPort(coordinates xy, int ch)
{
	// Update view port of the text.
	// This could be seen as some kind of paging making editing possible outside of terminal max xy,

	if (xy.y >= getmaxy(stdscr) && (ch == '\n' || ch == KEY_DOWN))
	{
		++_viewStart;
	}

	if (xy.y == 0 && _viewStart > 0 && ch == KEY_UP)
	{
		--_viewStart;
	}
}

TEXT *openFile(TEXT *head, char *fileName)
{
	// Get the name of the file to be open.
	char *path = newFileName();
	if (path == NULL)
	{
		wclear(stdscr);
		printw("Couldn't open file");
		wrefresh(stdscr);
		return head;
	}

	saveOnFileChange(head, fileName);

	// Set assigned path as the file name.
	strcpy(fileName, path);

	// Open the file and get its size.
	FILE *fp = getFile(fileName);
	long fileSize = getFileSize(fp);

	// Allocate a buffer and load the data from the file into the buffer.
	char *buffer = allocateBuffer(fileSize);
	loadBuffer(buffer, fp, fileSize);

	// Create a new list from the buffer, when done free and return the new list.
	deleteAllNodes(head);
	head = createNodesFromBuffer(buffer, fileSize);
	_fileSize = getFileSizeFromList(head);

	freeBuffer(buffer);
	return head;
}

void editTextFile(TEXT *head, char *fileName)
{
	dataCopied cpy_data = {NULL, {0, 0}, {0, 0}, false, false};
	coordinates xy = getEndNodeCoordinates(head);
	printNodes(head);
	_fileSize = getFileSizeFromList(head);

	for (int ch = 0, is_running = true; is_running; ch = wgetch(stdscr))
	{
		int mode = setMode(ch);
		xy = moveArrowKeys(ch, xy);

		switch (mode)
		{
		case EDIT:
			xy = edit(&head, xy, ch);
			break;
		case SAVE:
			save(head, fileName);
			break;
		case COPY:
			cpy_data = copy(cpy_data, head, xy);
			break;
		case PASTE:
			pasteCopiedlist(&head, cpy_data.cpy_List, xy);
			break;
		case OPEN_FILE:
			head = openFile(head, fileName);
			break;
		case EXIT:
			saveOnFileChange(head, fileName);
			is_running = false;
			break;
		}

		updateViewPort(xy, ch);
		setLeftMargin(head);
		updateCoordinatesInView(&head);
		printNodes(head);
		wmove(stdscr, xy.y, xy.x);
	}
}