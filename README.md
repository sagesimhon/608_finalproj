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
