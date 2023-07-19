# oscarjfb-editor

This is a terminal-based text editor, It takes args from the main function to determine whether the user is reading from an existing file or not. It loads this data into a buffer, this buffer is divided character by character into a linked list.

Each node in this linked list should have an x and y coordinate indicating character position in the terminal, it should also contain a storage type of (32bit int) for the character itself and finally a *next and *prev pointer. This linked list thus forms the structure of the text, which together with ncurses enables text-editing. 

COMMAND LIST: 
    ESC + S = save mode, which enables saving of a file. 
    ESC + e = exit mode, this will exit the application (without saving).
    ESC + c = copy mode, one pressed the cursor coordinates will be stored, once pressed again, 
                the second cursor coordinate is stored. All text between the coordinates will be stored in a buffer. 
    ESC + p = paste mode, this will paste the buffer save from copying, the buffer will remain until ESC + c is pressed again. 
          The reason for this is to allow for multiple pastes.  

BUGS/KNOWN ISSUES:
- Tab is not working correctly.