# oscarjfb-editor

This is a terminal-based text editor

COMMAND LIST:
 
    	- ESC + S = save mode, which enables saving of a file. 
 
    	- ESC + e = exit mode, this will exit the application (without saving).

    	- ESC + c = copy mode, When first pressed the cursers current coordinates will be stored, once pressed again, 
		the second curser coordinate is stored. All text between the coordinates points will be stored in a buffer. 

    	- ESC + p = paste mode, this command will paste the buffer saved from copy mode, the buffer will remain until ESC + c is pressed again. 
		The reason for this is to allow for multiple pastes.  

BUGS/KNOWN ISSUES:

	- When using KEY_DOWN at the view, on some occasions if next action is to add text, the text will be written to the newline outside of paging view.  

	- Deleting a tab character may result in cursor ending up at strange location.
	
