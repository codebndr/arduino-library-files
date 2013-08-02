#include <SensuinoEth.h>
#include <SensuinoSerial.h>
#include <Ethernet.h>
#include <SPI.h>

//If you use the Sensuino proxy
SensuinoSerial pf;
//If you use an ethernet shield
SensuinoEthernet pf;

const long int device_id = 0000;//Set your device's ID
const long int actuator_feed_id = 1111;//Set feed ID 

void setup() {
  Serial.begin(9600);  
  byte err;
  //Ethernet shield
  // If you don't want to use DHCP
  // pf.init(&err, 0, "aa:bb:cc:dd:ee:ff", "127.0.0.1", "192.168.1.1");// First IP is for the Arduino, second IP is for the gateway
  pf.init(&err, 1, "aa:bb:cc:dd:ee:ff");
  //USB connection via Proxy
  pf.init(&err,9600);
  
  pf.setSenseKey("your-sense-key-goes-here");//Enter your Sen.se key
  Serial.println("Init done");
  
  //Get value last value
  pf.getSense(actuator_feed_id);
  Serial.println(pf.getLastValue());
  
  //LongPoll
  pf.longPollSense(device_id);
}

void loop() {
  byte _check = pf.longPollCheck();
    if(_check == 0) {
      pf.longPollSense(device_id);
      
      int feed_id = pf.getLastFeedId();
      char *str = pf.getLastValue();
      
      if(feed_id == actuator_feed_id) {
        //do sth with feed value
        Serial.println(str);
      }
      
    } else if(_check == 2) {
      pf.longPollSense(device_id);
    }
    
    delay(200);
}
