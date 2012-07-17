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
 * This is an example of a default receiver and transmiter.
 * Upon receiving a packet, we send it back to the sender.
 *
 */
 
 
//Include Libraries
#include <XBee.h>
#include <XbeeRadio.h>

//Create the XbeeRadio object we'll be using
XBeeRadio xbee = XBeeRadio();
// create a reusable response object for responses we expect to handle 
XBeeRadioResponse response = XBeeRadioResponse();
// create a reusable rx16 response object to get the address
Rx16Response rx = Rx16Response();
// create a tx16 request object
Tx16Request tx;
//Runs only once
void setup()
{
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
    //get our response and save it on our response variable
    xbee.getResponse().getRx16Response(rx);
    //get the first byte
    uint8_t* data = xbee.getResponse().getData();
    uint16_t sender = rx.getRemoteAddress16();
    uint8_t dataLength = xbee.getResponse().getDataLength(); 
    tx = Tx16Request(sender, data, dataLength);
    xbee.send(tx);
  } 
    
}


