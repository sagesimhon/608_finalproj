#ifndef Motion_h
#define Motion_h
#include "Arduino.h"

class Motion
{
  private:
    float canvas_x;
    float canvas_y;
  public:
    float starting_pos_x;
    float starting_pos_y;
    Motion(float width, float height);
    float vel_to_pos(float vel, bool isX);
    float disp_to_pos(float disp, bool isX);
      
};

#endif

