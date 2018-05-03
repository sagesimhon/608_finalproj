#ifndef motionValues_h
#define motionValues_h
#include "Arduino.h"

class motionValues {
  private:
  unsigned short firmware_length =3070;
  int PIN_SCLK= 18;
  int PIN_MISO = 19; 
  int PIN_MOSI =23;
  int PIN_NCS = 3;
  int PIN_MOTION = 34;
  public:
    void motion_Values();
    void motion_setup();
    void adns_com_begin();
    void adns_com_end();
    byte adns_read_reg(byte reg_addr);
    void adns_write_reg(byte reg_addr, byte data);
    void adns_upload_firmware();
    float motionVals_x();
    float motionVals_y();
    //returns delta x and delta y values
};
#endif

