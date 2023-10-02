# oscarjfb Editor

## This is a terminal-based text editor
The editors basic features are working, you can load and edit files, navigate the data with the arrow keys, copy and paste content etc. 
However, there are some minor issues that are the legacy of poor design decisions. 
Despite this, I think it's a cool little program and my basic goals with the project have been achieved. 

Conclusion:
Use at will, but be aware that the application is not perfect! =) 

### COMMAND LIST:
 
    	- ESC + S = save, which enables saving of a file.  
 
    	- ESC + e = exit, this will exit the application (without saving).  

    	- ESC + y = copy, When first pressed the cursers current coordinates will be stored, once pressed again, 
		the second curser coordinate is stored. All text between the coordinates points will be stored in a buffer.  

	- ESC + d = Cut, this will enable cutting and pasting of text, once pressed again, 
		the second curser coordinate is stored. All text between the coordinates points will be stored in a buffer and cut.	

	- ESC + p = paste, this command will paste the buffer saved from copy mode, the buffer will remain until ESC + c is pressed again. 
		The reason for this is to allow for multiple pastes.  

	
