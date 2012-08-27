/*
tmp102.cpp
libary for using the I2C tmp102 temperature sensor

(c) Written by Jeroen Cappaert for NanoSatisfi, August 2012
*/

#include <Arduino.h>
#include "tmp102.h"
#include <Wire.h>

//Constructor
tmp102::tmp102()
{
  
}


// get temperature value
float tmp102::getTemperature()
{
  Wire.requestFrom(TEMP_ADDR,2); 

  byte MSB = Wire.read();
  byte LSB = Wire.read();

  //it's a 12bit int, using two's compliment for negative
  int TemperatureSum = ((MSB << 8) | LSB) >> 4; 

  float celsius = TemperatureSum*0.0625;
  return celsius;
}
