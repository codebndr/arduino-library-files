/*
mag3110.h
libary for using the I2C vcnl4000 light sensor

(c) Written by Jeroen Cappaert for NanoSatisfi, August 2012
*/

#ifndef vcnl4000_h
#define vcnl4000_h

#include <Arduino.h>

// define I2C address
#define AMB_ADDR  0x13 // IR and Ambient Light 0x26 write, 0x27 read

// Register Map Ambient Light Sensor
#define COMMAND_0 0x80            // starts measurments, relays data ready info
#define PRODUCT_ID 0x81           // product ID/revision ID, should read 0x11
#define IR_CURRENT 0x83           // sets IR current in steps of 10mA 0-200mA
#define AMBIENT_PARAMETER 0x84    // Configures ambient light measures
#define AMBIENT_RESULT_MSB 0x85   // high byte of ambient light measure
#define AMBIENT_RESULT_LSB 0x86   // low byte of ambient light measure
#define PROXIMITY_RESULT_MSB 0x87 // High byte of proximity measure
#define PROXIMITY_RESULT_LSB 0x88 // low byte of proximity measure
#define PROXIMITY_FREQ 0x89       // Proximity IR test signal freq, 0-3
#define PROXIMITY_MOD 0x8A        // proximity modulator timing



class vcnl4000
{
  public:
    vcnl4000();
    void configAmb();
    void writeByte(int device, byte address, byte data);
    byte readByte(int device, byte address);
    unsigned int readAmbient(void);
  private:
  
};




#endif
