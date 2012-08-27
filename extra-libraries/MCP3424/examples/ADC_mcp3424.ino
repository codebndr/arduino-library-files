/*
Example for using the MCP3424 libary for reading values from a certain channel from the ADC
and setting some of the variables in the configuration register

see MCP3424 datasheet for information on paramters and setting register bits

(c) Jeroen Cappaert for Nanosatisfi, August 2012

*/


#include <Wire.h>
#include "MCP3424.h"


// Configuration variables for MCP3424
#define address  0x69 // address of this MCP3424. For setting address see datasheet
byte gain = 0; // gain = x1
byte resolution = 3; // resolution = 18bits, 3SPS

MCP3424 ADC1(address, gain, resolution); // create MCP3424 instance.



//---------
// SETUP
//---------

void setup()
{
  Serial.begin(9600);  
  Wire.begin(); 
}

//-------
// LOOP
//-------

void loop()
{
  
  // get channel data with function
  double CH1 = ADC1.getChannelmV(0);
  double CH2 = ADC1.getChannelmV(1);
  Serial.print("Channel 1: ");Serial.print(CH1);Serial.println(" mV");
  Serial.print("Channel 2: ");Serial.print(CH2);Serial.println(" mV");


  delay(100);
}

