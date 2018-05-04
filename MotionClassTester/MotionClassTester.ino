#include "Motion.h"
#include "motionValues.h"


Motion tester(200.0, 200.0);
motionValues mv;


void setup() {
  Serial.begin(115200);
  mv.motion_setup();
 
}



void loop() { //repeatedly serial print the 50 new position values 
  delay(50);
  float dis =  mv.motionVals_y(); //gets DeltaY from sensor via MotionValues class 
  int pos = tester.disp_to_pos(dis, false);
  Serial.println("displacement: " +  String(dis));
  Serial.println("position: " + String(pos));
  Serial.println("starty" + String(tester.starting_pos_y));
}
