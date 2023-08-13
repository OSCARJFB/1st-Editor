/*
	Writen by: Oscar Bergström
	https://github.com/OSCARJFB

	MIT License
	Copyright (c) 2023 Oscar Bergström
*/

#include "editorMode.h"

static TEXT *createNewNode(int ch);
static coordinates onEditCoordinates(coordinates xy, int sFlag,
									 int ch, TEXT *last_node);
static coordinates addNode(TEXT **headNode, int ch, coordinates xy);
static coordinates deleteNode(TEXT **headNode, coordinates xy);
static coordinates updateCursor(int ch, coordinates xy);
static coordinates edit(TEXT **headNode, coordinates xy, int ch);
static coordinates resizeWinOnSigwinch(TEXT *headNode, coordinates xy);
static inline coordinates updateXYOnNewLine(coordinates xy, int ch, int newLines);
static dataCopied getCopyStart(dataCopied cp_data, coordinates xy);
static dataCopied getCopyEnd(dataCopied cp_data, coordinates xy);
static dataCopied copy(dataCopied cpyData, TEXT *headNode, coordinates xy);
static void pasteCopiedlist(TEXT **headNode, char *cpyList, coordinates xy);
static void saveOnFileChange(TEXT *headNode, char *fileName);
static void save(TEXT *headNode, char *fileName);
static void deleteAllNodes(TEXT **headNode);
static void updateCoordinatesInView(TEXT **headNode);
static void printText(TEXT *headNode, coordinates xy);
static void updateMargins(int y, int ch, TEXT *headNode);
static void handleSigwinch(int signal);
static inline void setLeftMargin(int NewLines);
static inline void setRightMargin(int y, TEXT *headNode);
static inline void setBottomMargin(int y, TEXT *headNode);
static long getFileSizeFromList(TEXT *headNode);
static int setMode(int ch);
static int countNewLinesInView(TEXT *headNode);
static char *newFileName(void);
static char *saveListToBuffer(TEXT *headNode, long fileSize);

textMargins _margins = {MARGIN_SPACE_2, 0, 0, 0};
bool _sigwinchFlag = false;
int _tabSize = 6;
int _copySize = 0;
int _viewStart = 0;
int _view = 0;
long _fileSize = 0;

void *createNodesFromBuffer(char *buffer, long fileSize)
{
	if (buffer == NULL)
	{
		return NULL;
	}

	coordinates xy = {0, 0};
	TEXT *headNode = NULL;

	// Add each character from the read file to the list.
	for (int i = 0; i < fileSize; ++i)
	{
		addNode(&headNode, buffer[i], xy);

		if (buffer[i] == '\n')
		{
			++xy.y;
			xy.x = 0;
			continue;
		}

		++xy.x;
	}

	updateCoordinatesInView(&headNode);
	return headNode;
}

static long getFileSizeFromList(TEXT *headNode)
{
	long fileSize = 0;
	for (long i = 0; headNode != NULL; ++i)
	{
		headNode = headNode->next;
		if (headNode->next == NULL)
		{
			fileSize = i + 1;
			break;
		}
	}

	return fileSize;
}

static void save(TEXT *headNode, char *fileName)
{
	FILE *fp = NULL;
	_fileSize = getFileSizeFromList(headNode);
	char *buffer = saveListToBuffer(headNode, _fileSize);
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

static char *saveListToBuffer(TEXT *headNode, long fileSize)
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

	for (int i = 0; headNode != NULL && i < fileSize; headNode = headNode->next)
	{
		buffer[i++] = headNode->ch;
	}

	buffer[fileSize] = '\0';

	return buffer;
}

static void saveOnFileChange(TEXT *headNode, char *fileName)
{
	long currentFileSize = getFileSizeFromList(headNode);
	if (currentFileSize == _fileSize)
	{
		return;
	}

	wclear(stdscr);
	printw("%s have been modified, would you like to save? (Y/N)", fileName);
	int ch = wgetch(stdscr);
	if (ch == 'y' || ch == 'Y')
	{
		if (fileName == NULL)
		{
			fileName = newFileName();
		}

		save(headNode, fileName);
	}
	wrefresh(stdscr);
}

static char *newFileName(void)
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

static void deleteAllNodes(TEXT **headNode)
{
	if(headNode == NULL)
	{
		return;
	}

	// Delete and free every single node.
	TEXT *temp = NULL;
	while(*headNode != NULL)
	{
		temp = *headNode;
		*headNode = (*headNode)->next;
		free(temp);
	}

	temp = NULL;
	headNode = NULL;
}

static TEXT *createNewNode(int ch)
{
	TEXT *newNode = malloc(sizeof(TEXT));
	if(newNode == NULL)
	{
		return NULL;
	}

	newNode->ch = ch;
	newNode->next = NULL;
	newNode->prev = NULL;
	return newNode;
}

/**
 * Here we set the coordinates of the cursor.
 * This function should be called once the TEXT list have been edited.
 */
static coordinates onEditCoordinates(coordinates xy, int sFlag, 
			             int ch, TEXT *node)
{
	switch(sFlag)
	{
		case ADD_FIRST_NODE:
			xy.x = ch == '\n' ? _margins.left : _margins.left + 2;
			xy.y += ch == '\n' ? 1 : 0;
			xy.x += ch == '\t' ? _tabSize : 0;
			break;
		case ADD_HEAD_NODE:
			xy.x = ch == '\n' ? _margins.left : _margins.left + 1;
			xy.y += ch == '\n' ? 1 : 0;
			xy.x += ch == '\t' ? _tabSize : 0;
			break;
		case ADD_MIDDLE_NODE:
			xy.x = ch == '\n' ? _margins.left : node->x + 1;
			xy.y += ch == '\n' ? 1 : 0;
			xy.x += ch == '\t' ? _tabSize : 0;
			break;
		case ADD_END_NODE:
			xy.x = ch == '\n' ? _margins.left : node->ch == '\n' ? _margins.left + 1 : node->x + 2;
			xy.y += ch == '\n' ? 1 : 0;
			xy.x += ch == '\t' ? _tabSize : 0;
			break;
		case DEL_NODE:
			xy.x = node->x;
			xy.y = node->y;
			break;
		case DEL_AT_END:
			xy.x = _margins.left;
			xy.y = 0;
			break;
	}

	return xy;
}

static coordinates addNode(TEXT **headNode, int ch, coordinates xy)
{
	TEXT *newNode = createNewNode(ch), *node = *headNode, *prevNode = NULL;
	if (*headNode == NULL)
	{
		*headNode = newNode;
		return onEditCoordinates(xy, ADD_FIRST_NODE, ch, NULL);
	}

	// Special case, if a newline is the first character in the list!
	if ((*headNode)->prev == NULL && (*headNode)->next == NULL &&
		xy.y == _margins.top && (*headNode)->ch == '\n')
	{
		TEXT *node = *headNode;
		node->prev = newNode;
		*headNode = newNode;
		newNode->next = node;
		return onEditCoordinates(xy, ADD_FIRST_NODE, ch, node);
	}

	while (node->next != NULL)
	{
		// Add the node and set it as the new headNode of the list.
		if (node->x == xy.x && node->y == xy.y && node->prev == NULL)
		{
			node->prev = newNode;
			*headNode = newNode;
			newNode->next = node;
			return onEditCoordinates(xy, ADD_HEAD_NODE, ch, node);
		}

		node = node->next;

		// Add the node somewhere in the middle of the list.
		if (node->x == xy.x && node->y == xy.y && node->prev != NULL)
		{
			node->prev->next = newNode;
			newNode->prev = node->prev;
			node->prev = newNode;
			newNode->next = node;
			return onEditCoordinates(xy, ADD_MIDDLE_NODE, ch, node);
		}
	}

	// Add the node at the end of the list.
	prevNode = node;
	node->next = newNode;
	newNode->prev = prevNode;
	return onEditCoordinates(xy, ADD_END_NODE, ch, node);
}

static coordinates deleteNode(TEXT **headNode, coordinates xy)
{
	// We can't free/delete a node which is NULL or if at end of coordinates.
	if (*headNode == NULL || (xy.x == _margins.left && xy.y == 0))
	{
		return xy;
	}

	bool isEndNode = true;
	TEXT *del_node = *headNode, *temp_node = NULL;

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
		free(*headNode);
		*headNode = NULL;
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
			*headNode = temp_node->next;
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

static void updateCoordinatesInView(TEXT **headNode)
{
	if (*headNode == NULL)
	{
		return;
	}

	int x = _margins.left, y = 0, newLines = 0, nLinesInView = 0;
	for (TEXT *node = *headNode; node != NULL; node = node->next)
	{
		if (newLines >= _viewStart)
		{
			nLinesInView += node->ch == '\n' ? 1 : 0;
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
				x = _margins.left;
				++y;
			}
		}

		newLines += node->ch == '\n' ? 1 : 0;
		if (nLinesInView >= _view)
		{
			break;
		}
	}
}

static dataCopied getCopyStart(dataCopied cpyData, coordinates xy)
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

static dataCopied getCopyEnd(dataCopied cpyData, coordinates xy)
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
				cpyList = malloc(CPY_BUFFER_SIZE * sizeof(char));
				if (cpyList == NULL)
				{
					return NULL;
				}

				start_found = true;
			}

			if (i < CPY_BUFFER_SIZE)
			{
				cpyList[i++] = headNode->ch;
			}
		}

		// If true end of list is found.
		if (headNode->x == cpyEnd.x && headNode->y == cpyEnd.y)
		{
			_copySize = i;
			break;
		}

		headNode = headNode->next;
	}

	return cpyList;
}

static void pasteCopiedlist(TEXT **headNode, char *cpyList, coordinates xy)
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
		TEXT *new_node = createNewNode(cpyList[i]);
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

static int countNewLinesInView(TEXT *headNode)
{
	int newlines = 0;

	// Count all the newlines found in the text until max view in y axis is reached.
	while (headNode != NULL && newlines != _view + 1)
	{
		if (headNode->ch == '\n')
		{
			++newlines;
		}

		headNode = headNode->next;
	}
	return newlines;
}

static void printText(TEXT *headNode, coordinates xy)
{
	int lineNumber = 0;
	bool nlFlag = true;

	// We need to clear terminal screen (empty) if no characters exists.
	if (headNode == NULL)
	{
		clear();
		printw("%d", lineNumber + 1);
		move(xy.y, xy.x);
		refresh();
		return;
	}

	// Print the nodes at x and y position.
	clear();
	int nLinesInView = 0;
	for (TEXT *node = headNode; node != NULL; node = node->next)
	{
		if (lineNumber >= _viewStart)
		{
			if (nlFlag)
			{
				nlFlag = false;
				printw("%d", lineNumber + 1);
				++nLinesInView;
			}
			mvwaddch(stdscr, node->y, node->x, node->ch);
		}

		if (node->ch == '\n')
		{
			nlFlag = true;
			++lineNumber;
		}

		if (nLinesInView == _view)
		{
			break;
		}
	}

	if (nlFlag && nLinesInView != _view)
	{
		printw("%d", lineNumber + 1);
	}

	move(xy.y, xy.x);
	refresh();
}

static int setMode(int ch)
{
	if(ch != ESC_KEY)
	{
		return EDIT;
	}

	ch = wgetch(stdscr);
	switch(ch)
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

static inline void setLeftMargin(int newLines)
{
	// Set margin depending on the amount of newlines
	if (newLines < LIM_1)
	{
		_margins.left = MARGIN_SPACE_3;
	}
	else if (newLines < LIM_2)
	{
		_margins.left = MARGIN_SPACE_4;
	}
	else if (newLines < LIM_3)
	{
		_margins.left = MARGIN_SPACE_5;
	}
	else if (newLines < LIM_4)
	{
		_margins.left = MARGIN_SPACE_6;
	}
}

static inline void setRightMargin(int y, TEXT *node)
{
	if (node->y == y && node->ch != '\n')
	{
		if (node->next != NULL)
		{
			_margins.right = node->next->ch == '\n' ? node->x + 1 : node->x + 2;
		}
		else
		{
			_margins.right = node->x + 1;
		}
	}
}

static inline void setBottomMargin(int newLines, TEXT *node)
{
	if (newLines >= _viewStart)
	{
		_margins.bottom = node->y;
	}

	if (node->next == NULL && node->ch == '\n')
	{
		_margins.bottom += _margins.bottom < _view ? 1 : 0;
	}
}

static void updateMargins(int y, int ch, TEXT *headNode)
{
	_margins.right = _margins.left;
	if (headNode == NULL)
	{
		return;
	}

	if (ch == KEY_UP)
	{
		y += y != _margins.top ? -1 : 0;
	}
	else if (ch == KEY_DOWN)
	{
		y += y <= _margins.bottom ? 1 : 0;
	}

	int newLines = 0;
	for (TEXT *node = headNode; node != NULL; node = node->next)
	{
		if (node == NULL)
		{
			break;
		}

		setBottomMargin(newLines, node);
		setRightMargin(y, node);

		newLines += node->ch == '\n' ? 1 : 0;
		if (newLines >= _view + _viewStart)
		{
			break;
		}
	}
	setLeftMargin(newLines);
}

static coordinates updateCursor(int ch, coordinates xy)
{
	switch(ch)
	{
		case KEY_UP:
			xy.y += xy.y > _margins.top ? -1 : 0;
			xy.x = xy.x > _margins.right ? _margins.right : xy.x;
			break;
		case KEY_DOWN:
			xy.x = xy.x > _margins.right && xy.y < _margins.bottom ? _margins.right : xy.x;
			xy.y += xy.y < _margins.bottom ? 1 : 0;
			break;
		case KEY_LEFT:
			xy.x += xy.x > _margins.left ? -1 : 0;
			break;
		case KEY_RIGHT:
			xy.x += xy.x < _margins.right ? 1 : 0;
			break;
	}

	return xy;
}

static coordinates edit(TEXT **headNode, coordinates xy, int ch)
{
	// If backspace is pressed delete a node at the current cursor location.
	// Else if ch is within the bounds of the condition add it in a new node.

	if(ch == KEY_BACKSPACE)
	{
		xy = deleteNode(headNode, xy);
	}
	else if((ch >= ' ' && ch <= '~') || (ch == '\t' || ch == '\n'))
	{
		xy = addNode(headNode, ch, xy);
	}

	return xy;
}

static dataCopied copy(dataCopied cpyData, TEXT *headNode, coordinates xy)
{
	if(cpyData.cpyList != NULL)
	{
		free(cpyData.cpyList);
		cpyData.cpyList = NULL;
	}

	cpyData = getCopyStart(cpyData, xy);
	cpyData = getCopyEnd(cpyData, xy);

	if(!cpyData.isStart && !cpyData.isEnd)
	{
		cpyData.cpyList = saveCopiedText(headNode, cpyData.cpyStart, cpyData.cpyEnd);
	}

	return cpyData;
}

static void updateViewPort(coordinates xy, int ch, int newLines)
{
	// Update view port of the text.
	// This could be seen as some kind of paging making editing possible outside of terminal max xy,

	if (newLines >= _view && ch == '\n')
	{
		++_viewStart;
	}
	else if (newLines >= _view && ch == KEY_BACKSPACE)
	{
		--_viewStart;
	}
	else if (xy.y <= 0 && _viewStart > 0 && ch == KEY_UP)
	{
		--_viewStart;
	}
	else if (xy.y == _view - 1 && newLines > _view && ch == KEY_DOWN)
	{
		++_viewStart;
	}
}

static TEXT *openFile(TEXT *headNode, char *fileName)
{
	char *path = newFileName();
	if (path == NULL)
	{
		wclear(stdscr);
		printw("Couldn't open file");
		wrefresh(stdscr);
		return headNode;
	}

	saveOnFileChange(headNode, fileName);

	if (fileName != NULL)
	{
		strcpy(fileName, path);
	}
	else
	{
		fileName = path;
	}

	TEXT *newHeadNode = reStart(fileName);
	if (newHeadNode == NULL)
	{
		return headNode;
	}

	deleteAllNodes(&headNode);
	headNode = newHeadNode;
	_fileSize = getFileSizeFromList(headNode);

	return headNode;
}

static inline coordinates updateXYOnNewLine(coordinates xy, int ch, int newLines)
{
	if (ch == '\n' && newLines >= _view)
	{
		--xy.y;
	}

	return xy;
}

static coordinates resizeWinOnSigwinch(TEXT *headNode, coordinates xy)
{
	// If true resize terminal, redraw the screen.
	if (_sigwinchFlag)
	{
		endwin();
		refresh();
		initscr();
		if (xy.y > getmaxy(stdscr))
		{
			xy.y = getmaxy(stdscr) - 1;
			xy.x = 0;
		}
		_view = getmaxy(stdscr);
		printText(headNode, xy);
		_sigwinchFlag = false;
	}
	else
	{
		_view = getmaxy(stdscr);
	}

	return xy;
}

static void handleSigwinch(int signal)
{
	// This will allow resizing of terminal window.
	if (signal == SIGWINCH)
	{
		_sigwinchFlag = true;
	}
}

void runApp(TEXT *headNode, char *fileName)
{
	dataCopied cpyData = {NULL, {0, 0}, {0, 0}, false, false};
	coordinates xy = {_margins.left, 0};
	updateCoordinatesInView(&headNode);
	printText(headNode, xy);
	_fileSize = getFileSizeFromList(headNode);
	signal(SIGWINCH, handleSigwinch);

	for (int ch = 0, is_running = true; is_running; ch = getch())
	{
		xy = resizeWinOnSigwinch(headNode, xy);
		switch (setMode(ch))
		{
			case EDIT:
				xy = edit(&headNode, xy, ch);
				break;
			case SAVE:
				save(headNode, fileName);
				break;
			case COPY:
				cpyData = copy(cpyData, headNode, xy);
				break;
			case PASTE:
				pasteCopiedlist(&headNode, cpyData.cpyList, xy);
				break;
			case OPEN_FILE:
				headNode = openFile(headNode, fileName);
				break;
			case EXIT:
				saveOnFileChange(headNode, fileName);
				is_running = false;
				break;
		}

		int newLines = countNewLinesInView(headNode);
		updateMargins(xy.y, ch, headNode);
		updateViewPort(xy, ch, newLines);
		updateCoordinatesInView(&headNode);
		xy = updateXYOnNewLine(xy, ch, newLines);
		xy = updateCursor(ch, xy);
		printText(headNode, xy);
	}

	deleteAllNodes(&headNode);
}
