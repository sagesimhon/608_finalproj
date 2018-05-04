#include "Arduino.h"
#include "Motion.h"

Motion::Motion(float width, float height) {
  
  // width: width of canvas
  // height: height of canvas
  // starting_pos_x and _y: initial x/y starting positions for drawings (0,0 defined at top left corner of canvas -- calibrated so that origin is now at center of canvas)
  
  starting_pos_x = 0 + width/2;
  starting_pos_y = 0 + height/2; 
  canvas_x = width;
  canvas_y = height;
  
}

float Motion::vel_to_pos(float vel, bool isX) {
  
// NOTE: NOT NECESSARY FOR OUR PURPOSES; ADNS RETURNS DISPLACEMENT VALUES; SEE OTHER FUNCTIONS. BACKUP IN CASE WE MEASURE V. Has not been tested 
// returns pos (in/s for now) by converting velocity vel (in/s) values to position using initial starting_pos 
// updates starting_pos_x or starting_pos_y (sets starting_pos _x or _y as returned position) 
// origin is defined as top left of canvas
// isX true if vel is x value, false if y value 
// if isX = false, negative velocity corresponds to increasing y position based on origin definition
  float starting_pos;
  float negate; 
  if (isX) {
    negate = 1; }
  else if (!isX) {
    negate = -1; }
  float dt = 20; // ms
  float pos = starting_pos + dt*vel*negate;
  if (isX) {
    starting_pos_x = pos; }
  else {
    starting_pos_y = pos;
  }

  return pos;

}

int Motion::disp_to_pos(float disp, bool isX) {
  
// NOTE: ADNS reads relative x and y displacement values (DeltaX and DeltaY)
// disp: relative displacement value [units]
// isX: true if disp is x value, false if y value 

// Takes in relative displacement value disp and RETURNS position on canvas using starting_pos_x or _y as initial position
// Updates starting_pos_x or starting_pos_y to new initial position (sets starting_pos_x or _y to returned position) 
// if isX = false, negative disp corresponds to increase in y position based on origin definition (use var negate for this)

// TODO FIX UNITS/SCALING FROM ADNS 

float negate; 
float starting_pos; //will be set to either starting_pos_x or starting_pos_y

if (isX) {
  negate = 1;
  starting_pos = starting_pos_x; 
  }
else {
  negate = -1;
  starting_pos = starting_pos_y; 
  }
    
int pos = int(starting_pos + disp/30.0*negate);

if (isX && pos > canvas_x) {
  pos = canvas_x;
}
else if (!isX && pos > canvas_y) {
  pos = canvas_y;
}

if (pos < 0) {
  pos = 0;
}

if (isX) {
  starting_pos_x = pos;
}
else {
  starting_pos_y = pos; 
}


return pos;
}

/*float Motion::scaleReading(float reading) { // scales disp by some order of magnitude to fit on the screen (and depending on how sensitive we want motion to be) 
  // reading: sensor reading to be scaled and modified
  return reading/30;
}*/
