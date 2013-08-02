/*
bmp085.h
libary for using the I2C bmp085 pressure sensor

(c) Written by Jeroen Cappaert for NanoSatisfi, August 2012
*/


#ifndef bmp085_h
#define bmp085_h

#include <Arduino.h>

#define BAR_ADDR  0x77 // Barometric pressure sensor I2C address


class bmp085
{
 public:
   //constructor
    bmp085();
    //functions
    void configBaro();
    int bmp085ReadInt(int device, byte address);
    unsigned int baroReadUT();
    unsigned long baroReadUP();
    float getTempFromBaro(unsigned int ut);
    long getPressFromBaro(unsigned long up);
    void getBmpData(float bmp[]);
    //variables
    int ac1; 
    int ac2;   
    int ac3; 
    unsigned int ac4;
    unsigned int ac5;
    unsigned int ac6;
    int b1; 
    int b2;
    int mb;
    int mc;
    int md;
    int barOSS;
    long b5; 
  private: 
};


#endif
