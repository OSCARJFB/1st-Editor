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
- Tab is not working correctly.