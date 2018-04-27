# 608_finalproj

### USEFUL STUFF:

project proposal: https://docs.google.com/document/d/1MmS5aJmTf7h9zif0waNb0SbHrCHV8-rOJm8-NlFZWDQ/edit
milestones: https://docs.google.com/spreadsheets/d/1dAaHvU9l2HsWnL9xeRB0oCax8zR0Zb-WK1GE9FGS7pE/edit#gid=0

#### General Workflow:
1. PULL BEFORE YOU START WORKING!!
2. Message out the files you'll be working on and the changes you'll make.
   This way, others know not to work on the same file / section and we'll minimize merge conflicts
3. COMMIT OFTEN!!! and please write descriptive commit statements in case someone has to pick up where you left off.
   Good commit statements means its easier to follow your train of thought.
4. PUSH WHEN YOU'RE DONE. 
   Push when you've completed something. ie try not to push half-finished broken code.
   But don't hoard the code you've completed either. 
5. COMMUNICATE OFTEN. Message out when you've pushed especially if it's a big / relevant change
   so that everyone else knows to pull.

### UPDATES
4/27/18:
1. ESP `POST_request` function is defined
	- two new relevant global variables are defined at the top, under label /// POST REQUESTS ///
	- IMPORTANT: you need to change these variables for your local code
2. Major adjustments are made to the `modifyReading` function
	- now has only one input, `reading`: float value to be scaled
	- all other inputs are hardcoded at the top, under label /// CANVAS DIMENSIONS ///
	- the function now returns an int (instead of void) because the float pointer is not happy about changing float to int
	- simplified some math based on the assumption that variable lowerbound (left/bottom bound of canvas) is always 0
	- (old function is commented out at the bottom; will delete later)
3. uses of `modifyReading` and `POST_request` are included inside loop under case DRAW
4. TODOs:
	- clean up docstring on `modifyReading` after confirming new fxn with team
	- server needs GET request implemented so the drawing shows up
	- (optional) update `modifyReading` to enable separate scaling for x and y coords, in case canvas is not square
	- (optional) currently, arrays _xCoords and _yCoords used by ImageCoords is float-based; change to int if necessary
	- package pretty print, mod, and `modifyReading` into a c++ class

4/20/18:

1. ESP TODOs:
   post instead of serial print (you can also keep the serial prints for testing purposes)
   Sample actual sensor data instead of using dummy value
   Expand upon the states. Keep in mind ESP is currently using ONE button tho we have two.
2. Server TODOs:
   Finish `request_handler` so that it properly unpacks `request` 
   Add more `GET` options. ie return a list of all image IDs, make the number of images to display variable, etc etc
3. Overall TODOs:
   Incorporate a z coordinate so that we know when the pen should draw a line, or
   be lifted and moved for example

4/19/18:
1. Button class is done.
2. ImageCoords class has been made. It stores the last k (x,y) coordinates, where k is user defined. 
   ImageCoords also conveniently returns its stored values as a comma separated string.
3. Rough ESP32 loop is completed. 
   Currently data is being added to our ImageCoords object. TODO format the data into a post request instead of just serial printing it
4. Rough server is done. It currently returns the html and js that displays an image
   based on variable lists of x and y coordinates.
