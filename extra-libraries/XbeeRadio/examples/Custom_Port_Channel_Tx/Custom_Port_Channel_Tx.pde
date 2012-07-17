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
 * This is an example of a custom transmiter on a custom port and channel.
 * Every 5 seconds, we broadcast (send to addr 0xffff) a dummy packet.
 *
 */
 
 
//Include Libraries
#include <XBee.h>
#include <XbeeRadio.h>

//Create the XbeeRadio object we'll be using
XBeeRadio xbee = XBeeRadio();
// create a reusable response object for responses we expect to handle 
XBeeRadioResponse response = XBeeRadioResponse();

//Create some data to send
uint8_t payload[] = {42, 42, 42};

// create a tx16 request object
Tx16Request tx = Tx16Request(0xffff, payload, sizeof(payload));
//Runs only once
void setup()
{
  //start our XbeeRadio object and set our baudrate to 38400.
  xbee.begin(38400);
  //Initialize our XBee module with the correct values using channel 13
  xbee.init(13);
}

void loop()
{
  //Send our package using port 112 instead of the default one (110)
  xbee.send(tx, 112);
  
   // after sending a tx request, we expect a status response
  // wait up to 5 seconds for the status response
  if (xbee.readPacket(5000))
  {
      // got a response! should be a znet tx status            	
      if (xbee.getResponse().getApiId() == TX_STATUS_RESPONSE)
      {
        xbee.getResponse().getZBTxStatusResponse(txStatus);
        
        // get the delivery status, the fifth byte
        if (txStatus.getStatus() == SUCCESS)
        {
          // success.  time to celebrate
        } 
        else
        {
          // the remote XBee did not receive our packet. is it powered on?
        }
      }      
  }
  else
  {
    // local XBee did not provide a timely TX Status Response -- should not happen
  }
  
  //wait for 5 seconds before transmitting again
  delay(5000);
}


