# oscarjfb Editor

## This is a terminal-based text editor
The editors basic features are working, you can load and edit files, navigate the data with the arrow keys, copy and paste content etc. 
However, there are some minor issues that are the legacy of poor design decisions. 
Despite this, I think it's a cool little program and my basic goals with the project have been achieved. 

Conclusion:
Use at will, but be aware that the application is not perfect! =) 

### COMMAND LIST:
 
    	- ESC + S = save mode, which enables saving of a file.  
 
    	- ESC + e = exit mode, this will exit the application (without saving).  

    	- ESC + y = copy mode, When first pressed the cursers current coordinates will be stored, once pressed again, 
		the second curser coordinate is stored. All text between the coordinates points will be stored in a buffer.  

    	- ESC + p = paste mode, this command will paste the buffer saved from copy mode, the buffer will remain until ESC + c is pressed again. 
		The reason for this is to allow for multiple pastes.  

	
