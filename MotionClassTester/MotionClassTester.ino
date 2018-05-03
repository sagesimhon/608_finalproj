#include "Motion.h"
#include "MotionValues.h"

float displacements[50]; //list of y values for displacement
int j = 0;


Motion tester(200, 200);
MotionValues mv;


void setup() {
  Serial.begin(115200);
  mv.motion_setup();

  
   //fill up displacements list
  for (int i = 0; i < 50; i++) {
  //int dis = rand() % 21 + (-10); // generates random displacement int between -10 and 10
  float dis =  mv.motionVals_y() //gets DeltaY from sensor via MotionValues class 
  displacements[i] = dis;
  }

}

void loop() { //repeatedly serial print the 50 new position values 
  float new_pos = tester.disp_to_pos(displacements[j], false);
  Serial.print(new_pos);
  j++;
  j %=50;
}
