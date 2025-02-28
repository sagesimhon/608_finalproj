# 608_finalproj

### USEFUL STUFF:

project proposal: https://docs.google.com/document/d/1MmS5aJmTf7h9zif0waNb0SbHrCHV8-rOJm8-NlFZWDQ/edit
milestones: https://docs.google.com/spreadsheets/d/1dAaHvU9l2HsWnL9xeRB0oCax8zR0Zb-WK1GE9FGS7pE/edit#gid=0

ADNS 9800 Sensor: 
https://github.com/mrjohnk/ADNS-9800 //useful github code
https://www.tindie.com/products/jkicklighter/adns-9800-laser-motion-sensor/ //useful information about sensor
https://datasheet.octopart.com/ADNS-9800-Avago-datasheet-10666463.pdf //the datasheet
https://github.com/pepijndevos/Dwergmuis
https://github.com/joshuajnoble/ArduinoADNS-9800.git //arduino code
Circuitry:
MOT- not plugged in
SS- RXD- 1O3
DG- GND
AG - GND
MO- 1O23
SC- IO18
MI- IO19
VI- 3v3

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
5/10/18
1. Added "shake to reset" functionality to .ino
how it works:
- need to shake at least three times (both ways) to register as a reset.
- you can see the exact threshold numbers on the script, defined under label /// SHAKE TO RESET FXN ///
- it sends a `DELETE_request` (new function defined at the bottom) with inputs `image_Id`, `num_entries`, and `color`
- at the moment, `num_entries` is hardcoded to 1000 (delete 1000 most recent entries) but subject to change
- after deleting, the oled displays "RESET" for three seconds and returns to the "INSTRUCTIONS" state


5/4/18
1.integrated the sensor into the 608_finalproj.ino
2. tested it on the server and it works!
3. TODO: work on the accuracy of the drawings.

5/3/18

1. added a class that returns the delta x and delta y readings from sensor
2. TODO: covert these to position values and scale them for the canvas

5/1/2018

tldr; the state of esp + server ultimately has not changed. However, the server has more capabilities 
that are functional provided that the ESP code changes to use these capabilities.

1. Server code is functional provided that its given proper get and post requests. 
   the javascript essentially "picks up the pen" when colors have changed.
2. POST requests have changed. Need to specify `color`. 
3. Added Artist class. Purpose of this class is to encapsulate all image related attributes, specifically
   The image name, the color the artist is using to draw, and the actual data points of the image.
   Purpose of ths class is to also give us the option to NOT hard-code things such as colors.
4. ESP code has been updated to format correct posts. 
5. TODOS:
   - integrate delta x and y values and read these values in the ESP code
   - add states to the ESP:
        a. one to allow the user to input which image he would like to draw on
        b. allow the user to pick his color
-------------------
1. Added ESP code that uses the ADNS 9800 sensor and returns delta x and delta y values
3. TODOS:
1. integrate the delta x and y values
2. replace these values with the imu values in the 608_finalproj.ino code
3. Hardware: create better mechanism for drawing 

4/27/ 18

1. ESP code uses autoincrementing image ID
2. server code now keys into `request` with `form`.
3. TODO: integrate sensor
-------------------
1. The request functions take in the x_coords, y_coords, entry_id
2. The server will display a picture based on the coordinates
3. I created an autoincrement id, but I did not add it to the post request-- should this id replace kerberos?
4. TODOS:
    -~~unhardcode the kerberos in the server code-- work on implementing autoincrement id~~
    -We should also start hacking out the sensors
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
