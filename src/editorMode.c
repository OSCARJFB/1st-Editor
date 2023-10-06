/*
	Writen by: Oscar Bergström
	https://github.com/OSCARJFB

	MIT License
	Copyright (c) 2023 Oscar Bergström
*/

#include "editorMode.h"

textMargins _margins = {MARGIN_SPACE_2, 0, 0, 0};
int _tabSize = 4;
int _viewStart = 0;
int _view = 0;
long _fileSize = 0;

static TEXT *createNewNode(int ch);
static TEXT *addNode(TEXT **headNode, int ch, coordinates xy);
static TEXT *deleteNode(TEXT **headNode, coordinates xy);
static TEXT *getViewStartNode(TEXT *headNode);
static TEXT *edit(TEXT **headNode, coordinates xy, int ch);
static coordinates updateCursor(int ch, coordinates xy, TEXT *editedNode, TEXT *headNode);
static void saveOnFileChange(TEXT *headNode, char *fileName);
static void save(TEXT *headNode, char *fileName);
static void deleteAllNodes(TEXT **headNode);
static void updateCoordinatesInView(TEXT **headNode);
static void printText(TEXT *headNode, coordinates xy);
static void updateMargins(int y, int ch, TEXT *headNode);
static void updateViewPort(coordinates xy, int ch, TEXT *headNode, TEXT *editedNode);
static bool isEndNode(int y, TEXT *startNode);
static inline void setLeftMargin(int NewLines);
static inline void setRightMargin(int y, TEXT *headNode);
static inline void setBottomMargin(TEXT *headNode);
static long getFileSizeFromList(TEXT *headNode);
static int setMode(int ch);
static int countNewLinesInView(TEXT *headNode);
static char *newFileName(void);
static char *saveListToBuffer(TEXT *headNode, long fileSize);

/**
 * Converts text from a buffer into a linked list.
 * The list is required when editing the text later on. 
 */
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
		(void)addNode(&headNode, buffer[i], xy);

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

/**
 * Counts the number of characters in the list.
 * This is necessary when converting the list into a buffer which is done when saving the file.
 */
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

/**
 * Save the TEXT list to a file.
 * Data will be stored in whatever text string the file name pointer stores.
 * If this pointer is NULL, request a new file name from the user. 
 */
static void save(TEXT *headNode, char *fileName)
{
	FILE *fp = NULL;
	_fileSize = getFileSizeFromList(headNode);
	char *buffer = saveListToBuffer(headNode, _fileSize);

	if (fileName == NULL)
	{
		char *newName = newFileName();
		fileName = memAlloc(malloc(sizeof(char) * FILENAME_SIZE), sizeof(char) * FILENAME_SIZE);
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

/**
 * Will convert the TEXT list into a regular buffer,
 * this buffer will be needed when saving the text. 
 */
static char *saveListToBuffer(TEXT *headNode, long fileSize)
{
	if (fileSize == 0)
	{
		return NULL;
	}

	char *buffer = memAlloc(malloc((fileSize + 2) * sizeof(char)), fileSize * sizeof(char) + 1);
	for (int i = 0; headNode != NULL && i <= fileSize; headNode = headNode->next)
	{
		buffer[i++] = headNode->ch;
	}

	buffer[++fileSize] = '\0';

	return buffer;
}

/**
 * This function will check if any changes have been made to the file.
 * If true it will ask if the user would like to save the file or not. 
 */
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

/**
 * Request a new file name.
 * Loop and check for user input, add the input to the fileName or remove it  
 */
static char *newFileName(void)
{
	char *fileName = memAlloc(malloc(sizeof(char) * FILENAME_SIZE), sizeof(char) * FILENAME_SIZE);
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

/**
 * Iterate the list and delete all nodes.
 * After calling free each pointer should be set to NULL. 
 */
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
		temp = NULL;
	}
}

/**
 * Create a new node. 
 * Set the values of the node. 
 */
static TEXT *createNewNode(int ch)
{
	TEXT *newNode = memAlloc(malloc(sizeof(TEXT)), sizeof(TEXT));
	newNode->ch = ch;
	newNode->next = NULL;
	newNode->prev = NULL;
	return newNode;
}

/**
 * Check the list for a spefic coordinates marking the current cursor position. 
 * At this point the function will add a new character. Returns a pointer to the newly added node. 
 */
static TEXT *addNode(TEXT **headNode, int ch, coordinates xy)
{
	TEXT *newNode = createNewNode(ch), *node = *headNode, *prevNode = NULL;
	if (*headNode == NULL)
	{
		*headNode = newNode;
		return newNode;
	}

	// Add the node and set it as the new headNode of the list.
	if (node->x == xy.x && node->y == xy.y && _viewStart == 0)
	{
		node->prev = newNode;
		*headNode = newNode;
		newNode->next = node;
		return newNode;
	}
	
	// Add the node somewhere in the middle of the list.
	for(int newLines = 0; node->next != NULL;)
	{	 
		newLines += node->ch == '\n' ? 1 : 0;
		node = node->next;
		if(newLines < _viewStart)
		{
			continue; 
		}

		if (node->x == xy.x && node->y == xy.y && node->prev != NULL)
		{
			node->prev->next = newNode;
			newNode->prev = node->prev;
			node->prev = newNode;
			newNode->next = node;
			return newNode; 
		}
	}

	// Add the node at the end of the list.
	prevNode = node;
	node->next = newNode;
	newNode->prev = prevNode;
	return newNode;
}

/**
 * This function will delete an item in the TEXT list.
 * It takes a list and searches the position of the list item to be deleted. It does so by looking at the cursor input (xy).
 * When found it will delete the item and relink the list. Returns NULL or a pointer to the prev node of the deleted node.  
 */
static TEXT *deleteNode(TEXT **headNode, coordinates xy)
{
	// We can't free/delete a node which is NULL or if at end of coordinates.
	if (*headNode == NULL || (_viewStart == 0 && xy.y == 0 && xy.x == _margins.left))
	{
		return NULL;
	}
	
	bool isEndNode = true;
	TEXT *node = *headNode;
	
	// If both prev and next are NULL this is the only node in the list.
	if (node->prev == NULL && node->next == NULL)
	{
		free(*headNode);
		*headNode = NULL;
		return NULL;
	}
	
	// Find the node to be deleted. continue untill inside of the view. 
	for(int newLines = 0; node->next != NULL; node = node->next)
	{	 
		newLines += node->ch == '\n' ? 1 : 0;
		if(newLines < _viewStart)
		{
			continue; 
		}		
		
		// Is a node in the middle of the list.
		if (node->x == xy.x && node->y == xy.y)
		{
			node = node->prev;
			isEndNode = false;
			break;
		}

		// Is the node just before the last node in the list.
		if (node->next->x == xy.x && node->next->y == xy.y && node->next->next == NULL)
		{
			isEndNode = false;
			break;
		}
	}

	// Link the nodes depending on it being the last node or a node in the middle of the list.
	if (isEndNode)
	{
		node->prev->next = NULL;
	}
	else if (!isEndNode)
	{
		if (node->prev != NULL && node->next != NULL)
		{
			node->prev->next = node->next;
			node->next->prev = node->prev;
		}
		else if (node->prev == NULL && node->next != NULL)
		{
			node->next->prev = NULL;
			*headNode = node->next;
		}
	}
	
	TEXT *editedNode = node->prev == NULL ? NULL : node->prev; 
	free(node);
	node = NULL;
	return editedNode;
}

/**
 * Will update the coordinates of the text inside the bounderies of the terminal view.
 * This needs to be done to display the TEXT list nodes at their correct location. 
 * We loop the list until we find the starting point (current view), then we update each item until the end of the view is reached.
 */
static void updateCoordinatesInView(TEXT **headNode)
{
	if(*headNode == NULL)
	{
		return;
	}

	int x = _margins.left, y = 0, newLines = 0, nLinesInView = 0;
	for(TEXT *node = *headNode; node != NULL; node = node->next)
	{
		if(newLines >= _viewStart)
		{
			nLinesInView += node->ch == '\n' ? 1 : 0;
			node->x = x;
			node->y = y;

			if(node->ch == '\t')
			{
				x += _tabSize;
			}
			else
			{
				++x;
			}

			if(node->ch == '\n')
			{
				x = _margins.left;
				++y;
			}
		}

		newLines += node->ch == '\n' ? 1 : 0;
		if(nLinesInView == _view)
		{
			break;
		}
	}
}

/**
 * Prints all the line numbers (starting at 1 if TEXT list is NULL) and the text from the TEXT list. 
 * This function will also print the cursor at its current position. 
 */
static void printText(TEXT *headNode, coordinates xy)
{
	int lineNumber = 0, nLinesInView = 0;
	bool nlFlag = true, pFlag = true; 

	if (headNode == NULL)
	{
		clear();
		printw("%d", lineNumber + 1);
		move(xy.y, xy.x);
		refresh();
		return;
	}

	clear();
	for (TEXT *node = headNode; node != NULL; node = node->next)
	{
		if (lineNumber >= _viewStart && pFlag)
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
			pFlag = false; 
			if(node->ch == '\n')
			{
				break; 
			}
			mvwaddch(stdscr, node->y, node->x, node->ch);
		}
	}

	if (nlFlag && nLinesInView != _view)
	{

		printw("%d", lineNumber + 1);
	}

	move(xy.y, xy.x);
	refresh();
}

/**
 * Sets the editor mode when ESC is pressed.  
 */
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
		case 'y':
			return COPY;
		case 'p':
			return PASTE;
		case 'd':
			return CUT;
		case 'o':
			return OPEN_FILE;
		case 'e':
			return EXIT;
	}

	return EDIT;
}

/**
 * This function will set the left margin. 
 * The size of the left margin is decided depending on the amount of rows in the file.  
 */
static inline void setLeftMargin(int newLines)
{
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


/**
 * The right margin is make sure the user can't navigate outside the bounds of the text.
 * Making sure we keep the cursor within the editor area. This value is found by looking at the current y rows x coordinate limit. 
 */
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

/**
 * Set the bottom margin.
 * This margin will prevent the user from navigating downwards outside the bounds of the TEXT list. 
 */
static inline void setBottomMargin(TEXT *node)
{
	_margins.bottom = node->y;
	if (node->next == NULL && node->ch == '\n')
	{
		_margins.bottom += _margins.bottom < _view ? 1 : 0;
	}
}

/**
 * This function will call for an update of the terminals margins.
 * It fetches and sets left, right, and bottom margin (top is always 0).
 */
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
		newLines += node->ch == '\n' ? 1 : 0;
		if (newLines >= _view + _viewStart  || node->next == NULL)
		{	
			setLeftMargin(newLines);
			setBottomMargin(node);
			break;
		}
		setRightMargin(y, node);
	}
}

/**
 * When using the arrow keys, this function will update the cursor position. This will ensure the cursor is placed at the correct location. 
 * Any update made of the cursor position must follow the bounderies set by the terminal margins.
 */
static coordinates updateCursor(int ch, coordinates xy, TEXT *editedNode, TEXT *headNode)
{
	if(headNode == NULL)
	{
		xy.x = _margins.left;
		xy.y = 0;
		return xy; 
	}

	switch(ch)
	{
		case KEY_UP:
			xy.y += xy.y > _margins.top ? -1 : 0;
			xy.x = xy.x > _margins.right ? _margins.right : xy.x;
			break;
		case KEY_DOWN:			
			xy.x = xy.x > _margins.right && xy.y < _margins.bottom ? _margins.right : xy.x; 
			xy.y += xy.y < _margins.bottom && xy.y < _view ? 1 : 0;
			break;
		case KEY_LEFT:
			xy.x += xy.x > _margins.left ? -1 : 0;
			break;
		case KEY_RIGHT:
			xy.x += xy.x < _margins.right ? 1 : 0;
			break;
		default:
			if(editedNode == NULL)
			{
				xy.x = _margins.left; 
				xy.y = 0; 
				return xy; 
			}
			
			if(editedNode->ch == '\t')
			{	
				if(ch == KEY_BACKSPACE)
				{	
					xy.x -= _tabSize + 1;
				}
				else
				{
					xy.x += _tabSize;
				}
				xy.y = editedNode->y;
				return xy; 
			}

			xy.x = editedNode->ch == '\n' ? _margins.left : editedNode->x + 1;
			xy.y = editedNode->ch == '\n' ? editedNode->y + 1 : editedNode->y;
			break;
	}
	return xy;
}

/*
 * If backspace is pressed delete a node at the current cursor location.
 * Else if ch is within the bounds of the condition add it in a new node.
 */
static TEXT *edit(TEXT **headNode, coordinates xy, int ch)
{
	static TEXT *node = NULL; 
	if(ch == KEY_BACKSPACE)
	{
		node = deleteNode(headNode, xy);
	}
	else if((ch >= ' ' && ch <= '~') || (ch == '\t' || ch == '\n'))
	{
	 	node = addNode(headNode, ch, xy);
	}

	return node;
}

/**
 * Count how many newlines that exists within the bounds of the terminal view.
 * This is usefull to help determine when we've added more items to the TEXT list than may be viewed in the terminal. 
 */
static int countNewLinesInView(TEXT *headNode)
{
	int newlines = 0;

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


static bool isEndNode(int y, TEXT *startNode)
{
	for(TEXT *node = startNode; node != NULL; node = node->next)
	{
		if(node->ch == '\n' && node->y == y)
		{
			return false;
		}
	}

	return true; 
}

static TEXT *getViewStartNode(TEXT *headNode)
{
	int newLines = 0; 
	for(TEXT *node = headNode; node->next != NULL; node = node->next)
	{
		if(newLines == _viewStart) 
		{
			return node; 
		}
		newLines += node->ch == '\n' ? 1 : 0; 
	}
	
	return NULL; 
}

/**
 * Update view port of the text.
 * This could be seen as some kind of paging making editing possible outside of terminal max bounds for xy.
 */
static void updateViewPort(coordinates xy, int ch, TEXT *headNode, TEXT *editedNode)
{
	int newLines = countNewLinesInView(headNode); 
	if(newLines >= _view && ch == '\n')
	{
		++_viewStart;
	}
	else if(newLines >= _view - 1 && _viewStart != 0 && ch == KEY_BACKSPACE && editedNode->ch == '\n')
	{
		--_viewStart;
	}
	else if(xy.y <= 0 && _viewStart > 0 && ch == KEY_UP)
	{
		--_viewStart;
	}
	else if(ch == KEY_DOWN) 	
	{
		TEXT *startNode = getViewStartNode(headNode);
		if(startNode == NULL)
		{
			return;
		}
		
		if(xy.y == _view - 1 && newLines > _view && !isEndNode(xy.y, editedNode))
		{
			++_viewStart;
		}
	}
}

/**
 * Open a new file at path location (fileName).
 * Freeing old data and setting the new filesize. 
 */
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

/**
 * Run text editor mode. 
 * While looping switch user action. 
 */
void runApp(TEXT *headNode, char *fileName)
{
	TEXT *editedNode = NULL;
	dataCopied cpyData = {NULL, {0, 0}, {0, 0}, false, false, 0};
	coordinates xy = {_margins.left + 1, 0};
	
	updateCoordinatesInView(&headNode);
	printText(headNode, xy);
	_fileSize = getFileSizeFromList(headNode);

	for (int ch = 0, is_running = true; is_running; ch = getch())
	{
		_view = getmaxy(stdscr); 
		
		switch (setMode(ch))
		{
			case EDIT:  
				editedNode = edit(&headNode, xy, ch);
				break;
			case SAVE: 
				save(headNode, fileName);
				break;
			case COPY: 
				cpyData = copy(cpyData, headNode, xy);
				continue; 
			case CUT: 
				cpyData = cut(cpyData, &headNode, xy);
				continue;
			case PASTE: 
				paste(&headNode, cpyData, xy);
				break;
			case OPEN_FILE:  
				headNode = openFile(headNode, fileName);
				break;
			case EXIT:  
				saveOnFileChange(headNode, fileName);
				is_running = false;
				continue; 
		}
		
		updateViewPort(xy, ch, headNode, editedNode);
		updateMargins(xy.y, ch, headNode);
		updateCoordinatesInView(&headNode);
		
		xy = updateCursor(ch, xy, editedNode, headNode);
		printText(headNode, xy);
	}

	deleteAllNodes(&headNode);
}
