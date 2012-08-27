/*
bmp085.cpp
libary for using the I2C bmp085 pressure sensor

(c) Written by Jeroen Cappaert for NanoSatisfi, August 2012
*/

#include <Arduino.h>
#include "bmp085.h"
#include <Wire.h>


//Constructor
bmp085::bmp085()
{
  barOSS = 2;
}

//configure barometer
void bmp085::configBaro()
{
  ac1 = bmp085ReadInt(BAR_ADDR, 0xAA);
  ac2 = bmp085ReadInt(BAR_ADDR, 0xAC);
  ac3 = bmp085ReadInt(BAR_ADDR, 0xAE);
  ac4 = bmp085ReadInt(BAR_ADDR, 0xB0);
  ac5 = bmp085ReadInt(BAR_ADDR, 0xB2);
  ac6 = bmp085ReadInt(BAR_ADDR, 0xB4);
  b1 = bmp085ReadInt(BAR_ADDR, 0xB6);
  b2 = bmp085ReadInt(BAR_ADDR, 0xB8);
  mb = bmp085ReadInt(BAR_ADDR, 0xBA);
  mc = bmp085ReadInt(BAR_ADDR, 0xBC);
  md = bmp085ReadInt(BAR_ADDR, 0xBE);
}

// user function: get pressure and temperature from sensor with one command
void bmp085::getBmpData(float bmp[])
{ 
 bmp[0] = getTempFromBaro(baroReadUT());
 bmp[1] = getPressFromBaro(baroReadUP());
}


// calculate temperature given ut 
float bmp085::getTempFromBaro(unsigned int ut)
{
  long x1, x2;
  
  x1 = (((long)ut - (long)ac6)*(long)ac5) >> 15;
  x2 = ((long)mc << 11)/(x1 + md);
  b5 = x1 + x2;

  return (float) ((b5 + 8)>>4)/10.;  
}


// Calculate pressure given up
// calibration values must be known
// b5 is also required so bmp085GetTemperature(...) must be called first.
// Value returned will be pressure in units of Pa.
long bmp085::getPressFromBaro(unsigned long up) 
{
  long x1, x2, x3, b3, b6, p;
  unsigned long b4, b7;
  
  b6 = b5 - 4000;
  // Calculate B3
  x1 = (b2 * (b6 * b6)>>12)>>11;
  x2 = (ac2 * b6)>>11;
  x3 = x1 + x2;
  b3 = (((((long)ac1)*4 + x3)<<barOSS) + 2)>>2;
  
  // Calculate B4
  x1 = (ac3 * b6)>>13;
  x2 = (b1 * ((b6 * b6)>>12))>>16;
  x3 = ((x1 + x2) + 2)>>2;
  b4 = (ac4 * (unsigned long)(x3 + 32768))>>15;
  
  b7 = ((unsigned long)(up - b3) * (50000>>barOSS));
  if (b7 < 0x80000000)
    p = (b7<<1)/b4;
  else
    p = (b7/b4)<<1;
    
  x1 = (p>>8) * (p>>8);
  x1 = (x1 * 3038)>>16;
  x2 = (-7357 * p)>>16;
  p += (x1 + x2 + 3791)>>4;
  
  return p;
}


// read uncorrected temperature value from bmp085
unsigned int bmp085::baroReadUT()
{
  unsigned int ut;
  
  // Write 0x2E into Register 0xF4
  // This requests a temperature reading
  Wire.beginTransmission(BAR_ADDR);
  Wire.write(0xF4);
  Wire.write(0x2E);
  Wire.endTransmission();
  
  // Wait at least 4.5ms
  delay(5);
  
  // Read two bytes from registers 0xF6 and 0xF7
  ut = bmp085ReadInt(BAR_ADDR,0xF6);
  return ut;
}


// read uncorrected pressure value from bmp085
unsigned long bmp085::baroReadUP() 
{
  unsigned char msb, lsb, xlsb;
  unsigned long up = 0;
  
  // Write 0x34+(OSS<<6) into register 0xF4
  // Request a pressure reading w/ oversampling setting
  Wire.beginTransmission(BAR_ADDR);
  Wire.write(0xF4);
  Wire.write(0x34 + (barOSS<<6));
  Wire.endTransmission();
  
  // Wait for conversion, delay time dependent on OSS
  delay(2 + (3<<barOSS));
  
  // Read register 0xF6 (MSB), 0xF7 (LSB), and 0xF8 (XLSB)
  Wire.beginTransmission(BAR_ADDR);
  Wire.write(0xF6);
  Wire.endTransmission();
  Wire.requestFrom(BAR_ADDR,3);
  
  // Wait for data to become available
  while(Wire.available() < 3)
    ;
  msb = Wire.read();
  lsb = Wire.read();
  xlsb = Wire.read();
  
  up = (((unsigned long) msb << 16) | ((unsigned long) lsb << 8) | (unsigned long) xlsb) >> (8-barOSS);
  
  return up;
}



// read integer on bmp085
int bmp085::bmp085ReadInt(int device, byte address)
{
  unsigned char msb, lsb;
  
  Wire.beginTransmission(device);
  Wire.write(address);
  Wire.endTransmission();
  
  Wire.requestFrom(device, 2);
  while(Wire.available()<2)
    ;
  msb = Wire.read();
  lsb = Wire.read();
  
  return (int) msb<<8 | lsb;
}




