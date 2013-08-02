/*
mag3110.h
libary for using the I2C mag3110 magnetometer

(c) Written by Jeroen Cappaert for NanoSatisfi, August 2012
*/

#ifndef mag3110_h
#define mag3110_h

#include <Arduino.h>


// define magnetometer I2C address (fixed)

#define MAG_ADDR  0x0E // Magnetometer MAG3110


class mag3110
{
  public:
    mag3110();
    void configMag();
    int readx();
    int ready();
    int readz();
  private:
};




#endif
