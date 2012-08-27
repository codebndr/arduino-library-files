/*
mag3110.h
libary for using the I2C tmp102 temperature sensor

(c) Written by Jeroen Cappaert for NanoSatisfi, August 2012
*/

#ifndef tmp102_h
#define tmp102_h

#include <Arduino.h>

#define TEMP_ADDR 0x48 // Temp-sensor data register


class tmp102
{
  public:
    tmp102();
   float getTemperature();
  private:
};



#endif
