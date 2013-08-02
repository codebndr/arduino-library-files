/*
mag3110.cpp
libary for using the I2C  vcnl4000 light sensor

(c) Written by Jeroen Cappaert for NanoSatisfi, August 2012
*/

#include <Arduino.h>
#include "vcnl4000.h"
#include <Wire.h>

vcnl4000::vcnl4000()
{
  
  
}

// configure sensor
void vcnl4000::configAmb(void) {
  writeByte(AMB_ADDR, AMBIENT_PARAMETER, 0x0F); // Single conversion mode, 128 averages
  writeByte(AMB_ADDR, IR_CURRENT, 20);          // Set IR current to 200mA
  writeByte(AMB_ADDR, PROXIMITY_FREQ, 2);       // 781.25 kHz
  writeByte(AMB_ADDR, PROXIMITY_MOD, 0x81);     // 129, recommended by Vishay
}

// read ambient light values
unsigned int vcnl4000::readAmbient(void)
{
  unsigned int data;
  byte temp;
  
  //trigger the ambient light sensor
  temp = readByte(AMB_ADDR, COMMAND_0);
  
  // command the sensor to perform ambient measure
  writeByte(AMB_ADDR, COMMAND_0, temp | 0x10);  
  
  // wait for the proximity data ready bit to be set
  while(!(readByte(AMB_ADDR, COMMAND_0)&0x40))
    ;
  
  //read value from 2 bytes  
  byte amb_MSB = readByte(AMB_ADDR, AMBIENT_RESULT_MSB);
  byte amb_LSB = readByte(AMB_ADDR, AMBIENT_RESULT_LSB);

  //combine the bytes
  data = ( amb_LSB | (amb_MSB << 8) );

  return data;
}




//-----------------------------
// HELP FUNCTIONS
//-----------------------------

void vcnl4000::writeByte(int device, byte address, byte data)
{
  Wire.beginTransmission(device);
  Wire.write(address);
  Wire.write(data);
  Wire.endTransmission();
}

byte vcnl4000::readByte(int device, byte address)
{
  byte data;
  
  //trigger the device address
  Wire.beginTransmission(device);
  Wire.write(address);
  Wire.endTransmission();
  
  //request one byte
  Wire.requestFrom(device, 1);
  
  //read data
  while(Wire.available())   { 
    data = Wire.read(); // receive the byte
  }
  
  return data;
}
