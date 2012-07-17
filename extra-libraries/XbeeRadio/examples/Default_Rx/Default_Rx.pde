/**
 * XbeeRadio Arduino library by Vasilis (tzikis) Georgitzikis (tzikis.com).
 * XbeeRadio uses another awesome Open Source Arduino library, XBee-Arduino.
 *
 * XbeeRadio is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 *
 * This is an example of a default receiver. Upon receiving a packet, 
 * we blink the status led on pin 13 twice.
 *
 */
 
 
//Include Libraries
#include <XBee.h>
#include <XbeeRadio.h>

//Create the XbeeRadio object we'll be using
XBeeRadio xbee = XBeeRadio();
// create reusable response objects for responses we expect to handle 
XBeeRadioResponse response = XBeeRadioResponse();

//We'll use pin 13 as our LED
int statusLed = 13;


//Taken from XBee-Arduino library, used for blinking the LED
void flashLed(int pin, int times, int wait) {
    
    for (int i = 0; i < times; i++) {
      digitalWrite(pin, HIGH);
      delay(wait);
      digitalWrite(pin, LOW);
      
      if (i + 1 < times) {
        delay(wait);
      }
    }
}


//Runs only once
void setup()
{
  //set our led
  pinMode(statusLed, OUTPUT);
  //start our XbeeRadio object and set our baudrate to 38400.
  xbee.begin(38400);
  //Initialize our XBee module with the correct values (using the default channel, channel 12)
  xbee.init();
}

void loop()
{
  //returns true if there is a packet for us on the default port
  if(xbee.checkForData())
  {
    //flash our led twice
    flashLed(statusLed, 2, 100);
  } 
    
}
