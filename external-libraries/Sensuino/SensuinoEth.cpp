#include "SensuinoEth.h"

#include <Ethernet.h>

EthernetClient _client;
EthernetClient _clientLongPoll;

byte _server[4] = SENSE_IP;

const int http_timeout = 10000;

boolean SensuinoEth::init(byte *err,  ...) {
	//TODO: error handling
	va_list argp;
	char *c;
	byte b;
	va_start(argp, err);
	
	b = va_arg(argp,int);
	
	c = va_arg(argp,char *);
	tokenize16(c,_mac);
	
	if(b==0) {
		c = va_arg(argp,char *);
		Serial.println(c);
		tokenize(c, _ip);
	
		c = va_arg(argp,char *);
		Serial.println(c);
		tokenize(c,_gateway);
	
		Ethernet.begin(_mac, _ip, _gateway);
		_dhcp=false;
		delay(1000);
	} else if(b==1) {
		Ethernet.begin(_mac);
		_dhcp=true;
	} else {
		//fishy
	}
	
	va_end(argp);
	_gpclient.setIpPort(_server,PORT_TO_REACH);
	_lpclient.setIpPort(_server,PORT_TO_REACH);
	
	_gpclient.setClient(&_client);
	_lpclient.setClient(&_clientLongPoll);
	
	*err = 0;
	
	return true;
}

boolean SensuinoEth::postSense(unsigned long feed_id, char* txData) {
	SenseData s;
	s.feed_id = feed_id;
	s.value = txData;
	return postSense(1, &s);
}

boolean SensuinoEth::postSense(byte size, SenseData* data) {
	byte feed_id_size = 0;
	unsigned long tx_data_len=0;
	
	for(byte i=0; i<size; i++) { 
		feed_id_size++;
		unsigned long temp = data[i].feed_id;
		while((temp = temp/10)>0){feed_id_size++;}
		
		tx_data_len += strlen(data[i].value);
		tx_data_len += 26;
		tx_data_len += 1;
	}
	
	tx_data_len += 6;
	tx_data_len -= 1;
	Serial.println("In postSense");
	
	if (_gpclient.connect()) {		
		_gpclient.write("POST /events/?sense_key=");
		_gpclient.write(senseKey);
		_gpclient.write(" HTTP/1.1\nHost: ");
		_gpclient.write("api.sen.se\r\n"); 
		_gpclient.write("Content-Type:application/json\r\n");
		_gpclient.write("Content-Length: ");
		_gpclient.print(tx_data_len+feed_id_size,DEC);
		_gpclient.write("\r\nConnection: Keep-Alive");
		_gpclient.write("\r\n\r\n");
		_gpclient.write("[");
		for(byte i=0;i<size;i++) {
			_gpclient.write("{\"feed_id\": ");
			_gpclient.print(data[i].feed_id,DEC);
			_gpclient.write(", \"value\": \"");
			_gpclient.write(data[i].value);
			_gpclient.write("\"}");
			
			if(size > 1 && i <= (size-2)) {
				_gpclient.write(",");
			}
		}
		
		_gpclient.write("]");
		_gpclient.write("\r\n\r\n");
		
		unsigned long _start = millis();
		
		while(_gpclient.connected() && (millis() < (_start + http_timeout))) {
			if(_gpclient.available()) {
				_gpclient.find("HTTP/1.1 ");	
				char c = _gpclient.read();
				char d = 1;
				while(d != 0) {
					d = _gpclient.read();
				}
				_gpclient.disconnect();
				if(c != '2') {
#ifdef DEBUG
						Serial.println("Failed posting");
#endif
					return false;
				}
#ifdef DEBUG
					Serial.print("Done posting: ");Serial.println(_gpclient.status(),DEC);
#endif
				return true;
			}
		}
		_gpclient.disconnect();
#ifdef DEBUG
		Serial.print("Failed posting, client disconnected: ");Serial.println(_gpclient.status(),DEC);
#endif
	} else {
#ifdef DEBUG
		Serial.print("failed to connect: ");
		Serial.println(_gpclient.status(),DEC);
#endif
		if(_dhcp) {
			Ethernet.begin(_mac);
		} else {
			Ethernet.begin(_mac, _ip, _gateway);
		}
#ifdef DEBUG
		Serial.println("Restarting Ethernet shield");
#endif
		delay(2000);
#ifdef DEBUG
		Serial.println("Done.");
#endif
	}
	return false;
}

boolean SensuinoEth::getSense(unsigned long feed_id) {
	//FIXME: seems buggy.
	boolean ret = false;
 	if (longPollSenseImpl(&_gpclient, feed_id, "-1", false, true)) {
		byte check = 0;	
		do {
			check = longPollCheck(&_gpclient,false);
			if(check == 0) {
				ret = true;
				break;
			}
		} while(check != 2);
	} else {
		Serial.println("longPollSense: false");
	}
 	
	return ret;
}

boolean SensuinoEth::longPollSenseImpl(unsigned long feed_id, char event_id[16]) {
	return longPollSenseImpl(&_lpclient, feed_id, event_id, true, false);
}

boolean SensuinoEth::longPollSenseImpl(NetworkHAL *client, unsigned long feed_id, char event_id[16], boolean is_device, boolean req_type) {
	if (client->connect()) {
		if(is_device)
			client->write("GET /rt/device/");
		else
			client->write("GET /rt/feed/");
		
		client->print(feed_id,DEC);
		client->write("/events?last=");
		client->write(event_id);
		client->write("&amp;sense_key=");
		client->write(senseKey); 
		if(req_type) {
			client->write("&amp;req_type=last");
		}
		client->write(" HTTP/1.1\nHost: ");
		client->write("api.sen.se\n");
		client->write("\r\n\r\n");
		
		return true;
	} else {
		Serial.println("longPollSenseImpl::failed to connect");
	}
	return false;
}

byte SensuinoEth::longPollCheck() {
	return longPollCheck(&_lpclient);
}

byte SensuinoEth::longPollCheck(NetworkHAL *client, boolean saveEventId) {
	byte ret = 2;
	if(client->connected()) {
		if(client->available()) {
			parseLongPollResponse(client, saveEventId) ? ret = 0 : ret = 2;
		} else {
			ret = 1;
		}
	} 
	
	return ret;
}

void SensuinoEth::tokenize(char* stringToBytes, byte* destTab)
{
	byte index = 0;
	byte indexDest = 0;
	byte value = 0;
	
	for(index = 0;index<strlen(stringToBytes);index++)
	{
		if(stringToBytes[index] == '.' || stringToBytes[index] == ':')
		{
			destTab[indexDest] = value;
			value = 0;
			indexDest++;
		}
		else 
		{
			value = (value*10)+(stringToBytes[index]-48);
		}
		
	}
	destTab[indexDest] = value;

}

void SensuinoEth::tokenize16(char* stringToBytes, byte* destTab)
{
	byte index = 0;
	byte indexDest = 0;
	byte value = 0;
	
	for(index = 0;index<strlen(stringToBytes);index++)
	{
		if(stringToBytes[index] == '.' || stringToBytes[index] == ':')
		{
			destTab[indexDest] = value;
			value = 0;
			indexDest++;
		}
		else 
		{
			if ((stringToBytes[index] >= '0') && (stringToBytes[index] <= '9'))
			{
				value = (value*16)+(stringToBytes[index]-48);
			}
			else if ((stringToBytes[index] >= 'A') && (stringToBytes[index] <= 'F'))
			{
				value = (value*16)+(stringToBytes[index]-55);
			}	
			else if ((stringToBytes[index] >= 'a') && (stringToBytes[index] <= 'f'))
			{
				value = (value*16)+(stringToBytes[index]-87);
			}
			else 
			{
				//???
			}
			
		}
		
	}
	destTab[indexDest] = value;
}

EthHAL::EthHAL() {
}

void EthHAL::setClient(EthernetClient *c) {
	this->_client = c;
}

boolean EthHAL::connect() {
	IPAddress addr(_server[0],_server[1],_server[2],_server[3]);
	return _client->connect(addr, _port);
}

void EthHAL::disconnect() {
	_client->stop();
}

size_t EthHAL::write(uint8_t val) {
	return _client->write(val);
}

void EthHAL::write(char* txBuf) {
	if (!_client->connected()) {
		if (connect()) {
			_client->print(txBuf);
		}
	} else {
		_client->print(txBuf);
	}
}

void EthHAL::flush() {
}

void EthHAL::print(unsigned long val, int base) {
	if (!_client->connected()) {
		if (connect()) {
			_client->print(val,base);
		}
	} else {
		_client->print(val,base);
	}
}

int EthHAL::available() {
	return _client->available();
}

boolean EthHAL::connected() {
	return _client->connected();
}

uint8_t EthHAL::status() {
	return _client->status();
}

int EthHAL::read() {
	if(available()) { 
		return _client->read();
	}
	
	return 0;
}

int EthHAL::peek() {
	return _client->peek();
}

void EthHAL::setIpPort(byte *s, int p) {
	_server = s;
	_port = p;
}

