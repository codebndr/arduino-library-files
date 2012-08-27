/*
MCP3424.cpp
Library for using the mcp3424 adc to I2C converter
	
	
written by jeroen cappaert (c) for nanosatisfi August 2012

Make an instance of MCP3423 by:
MCP3424 ADC1(I2Caddress, gain, resolution)
- gain values 0-3 represent {x1,x2,x4,x8}
- resolution 0-3 represents {12bits, 14bits, 16bits, 18bits}

User functions:
 - setChannel(channelnr.) - for channel 1-4, enter a value 0-1
 - getMvDivisor() - get the conversion value between the ADC value and mV
 - readData() - read data from the I2C channel
 - getChannelmV(channelnr.) - get data in mV directly from the I2C channel chosen by channelnr.
*/

#include <Arduino.h>
#include <Wire.h>
#include "MCP3424.h"

// constructor takes I2C address, required channel, gain and resolution and configures chip.
MCP3424::MCP3424(byte address, byte gain, byte res)
{ 
   _address = address;
   _gain = gain;
   _res = res;
}


// function sets channel
void MCP3424::setChannel(byte chan) 
{
  byte adcConfig = MCP342X_START | MCP342X_CHANNEL_1 | MCP342X_CONTINUOUS;
  adcConfig |= chan << 5 | _res << 2 | _gain;
  MCP3424Write(adcConfig);
}


// calculate and return mV devisor from gain and resolution.
int MCP3424::getMvDivisor()
{
  int mvDivisor = 1 << (_gain + 2*_res);
  return mvDivisor;
}

// Write byte to register on MCP3424
void MCP3424::MCP3424Write(byte msg) // address needs to be hardcoded or doesn't work
{
 //Serial.println(_address,HEX);
 Wire.beginTransmission(_address);
 Wire.write(msg);
 Wire.endTransmission(); 
  
}

long MCP3424::readData()
{
  long data;
  // pointer used to form int32 data
  byte *p = (byte *)&data;
  // timeout - not really needed?
  long start = millis();
  do {
    // assume 18-bit mode
    Wire.requestFrom(MCP3424_ADDRESS, 4);
    if (Wire.available() != 4) {
      Serial.println("read failed");
      return false;
    }
    for (int i = 2; i >= 0; i--) {
      p[i] = Wire.read();
    }
    // extend sign bits
    p[3] = p[2] & 0X80 ? 0XFF : 0;
    // read config/status byte
    byte s = Wire.read();
    if ((s & MCP342X_RES_FIELD) != MCP342X_18_BIT) {
      // not 18 bits - shift bytes for 12, 14, or 16 bits
      p[0] = p[1];
      p[1] = p[2];
      p[2] = p[3];
    }
    if ((s & MCP342X_BUSY) == 0) return data;
  }
  while (millis() - start < 500); //allows rollover of millis()
  Serial.println("read timeout"); 
  return false; 
}

double MCP3424::getChannelmV(byte chan)
{
  long data;
  setChannel(chan);
  int mvDivisor = getMvDivisor();
    if (readData()!=0){
    data = readData();
    }
    else Serial.println("Read error");
    
  double mv = (double)data/mvDivisor;
  return mv;
}



