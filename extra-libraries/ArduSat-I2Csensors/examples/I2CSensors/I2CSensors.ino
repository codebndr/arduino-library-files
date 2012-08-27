 ///=========================================================
// Many-Sensor-on I2C Test
//---------------------------------------------------------
//
// Connect a number of I2C sensors to one Arduino and poll 
//
// Author: pplatzer & jcappaert 2012
// (c) nanosatisfi.com
//=========================================================

//Includes
#include <Wire.h>          //I2C communication
#include <FreeSixIMU.h>    //6DOF
#include <FIMU_ADXL345.h>  //Accelerometer
#include <FIMU_ITG3200.h>  //Gyro
#include "mag3110.h"
#include "vcnl4000.h"
#include "tmp102.h"
#include "bmp085.h"

//Constants
//General
#define CycleTime 1000 //wait time for next measurements
const int MyI2C = 11;       //set own I2C address ID=11

//create instances for sensors
FreeSixIMU sixDOF = FreeSixIMU(); //FreeSixIMU object
mag3110 mag;
vcnl4000 light;
tmp102 temp;
bmp085 pres;

//Define Variables
float fltTemp;      //temperature
int   intMag[3];      //Magnetic field - x-axis
int   intAmbLight;  //Ambient Light value
float fltAngles[3]; //Euler Angles
float bmp[2];


//--------------------------------------------------------
//           S E T U P
//--------------------------------------------------------
void setup() {
  //Serial
  Serial.begin(115200);
  Serial.println("Set up....");

  //I2C Protocoll
  Wire.begin(MyI2C); //Join I2C bus as slave # MyI2C
  Serial.println("Joining I2C...");
  
  //Configure Sensors
  mag.configMag();Serial.println("Magnetometer configured....");//configure Magnetometer
  light.configAmb(); Serial.println("Ambient Light configured...");//configure Ambient Light Sensor
  sixDOF.init(); Serial.println("Euler Angles configured...");//configure 6DOF Unit
  //configBaro();Serial.println("Barometer configured.......");//configure Barometric sensor
  pres.configBaro();
  
  //Done Setup
  Serial.println("...done setup");
}

//--------------------------------------------------------
//           M A I N   L O O P
//--------------------------------------------------------
void loop(){
  //Measure Temperature
  Serial.print("requesting temperature....");
  fltTemp = temp.getTemperature();
  Serial.print(" Celsius: ");
  Serial.println(fltTemp);
  
  //Measure Magnetic Field
  Serial.print("requesting mag values.....");
  //intMagX = getMagX();
  intMag[0] = mag.readx();intMag[1] = mag.ready();intMag[2] = mag.readz();
  Serial.print(" myTsla:  ");
  Serial.print(intMag[0]);Serial.print(" | ");
  Serial.print(intMag[1]);Serial.print(" | ");
  Serial.println(intMag[2]);

  //Measure Ambient Light
  Serial.print("requesting Ambient Light..");
  intAmbLight = light.readAmbient();
  Serial.print(" lux:     ");
  Serial.println(intAmbLight, DEC);

  //Measure Euler Angles
  Serial.print("requesting Euler Angles...");
  sixDOF.getEuler(fltAngles);
  Serial.print(" Angles:  ");
  Serial.print(fltAngles[0]); Serial.print(" | ");
  Serial.print(fltAngles[1]); Serial.print(" | "); 
  Serial.println(fltAngles[2]);
  
  //Barometric Pressure
  Serial.print("requesting Pressure.......");
  pres.getBmpData(bmp);
  Serial.print(" Pressure: "); Serial.print(bmp[1]);
  Serial.print(" BaroTemp: "); Serial.println(bmp[0]);

  
  
  //Wait 1 second for next measurements
  Serial.println("---------------------------------------------------------");
  delay(CycleTime);
}

