# oscarjfb-editor

This is a terminal-based text editor

COMMAND LIST: 
    ESC + S = save mode, which enables saving of a file.  
    ESC + e = exit mode, this will exit the application (without saving).
    ESC + c = copy mode, one pressed the cursor coordinates will be stored, once pressed again, 
                the second cursor coordinate is stored. All text between the coordinates will be stored in a buffer. 
    ESC + p = paste mode, this will paste the buffer save from copying, the buffer will remain until ESC + c is pressed again. 
          The reason for this is to allow for multiple pastes.  

BUGS/KNOWN ISSUES:
- When deleting several nodes and reaching head node, if any node remain cursor may end up at the end of the list. it should stay firm at location 0,0
- Arrow key down does not work when paging, (in progress)
