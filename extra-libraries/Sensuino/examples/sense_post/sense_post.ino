#include <SensuinoEth.h>
#include <SensuinoSerial.h>
#include <Ethernet.h>
#include <SPI.h>

//If you use an ethernet shield :
SensuinoEth pf;
//If you use the Sensuino proxy : 
SensuinoSerial pf;

SenseData postData[2];
const long int feed_id1 = 0000; //Set feed ID
const long int feed_id2 = 1111;

void setup() {
  Serial.begin(9600);  
  byte err;
  
  //If you use an ethernet shield : 
  pf.init(&err, 0, "aa:bb:cc:dd:ee:ff", "127.0.0.1", "192.168.1.1");//First IP is for the Arduino, second IP is for the gateway
  
  //If you use the Sensuino proxy : 
  pf.init(&err,9600);//Sets : Serial port speed
  
  pf.setSenseKey("your-sense-key-goes-here");//Enter your Sen.se key
  Serial.println("Init done");
  
  //Post a single value  
  pf.postSense(feed_id1, "hello");

  // Post multiple values at once
  postData[0].feed_id = feed_id1;
  postData[1].feed_id = feed_id2;
  
  postData[0].value = 00; //value for feed1
  postData[1].value = 00; //value for feed2
  
  //postSense(number of value to post, SenseData array)
  pf.postSense(2,postData);
}

void loop() {
  
}

