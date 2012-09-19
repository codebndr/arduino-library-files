/*Copyright (c) 2011, o-labs (o-labs.net)
 All rights reserved.
 
 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:
 * Redistributions of source code must retain the above copyright
 notice, this list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright
 notice, this list of conditions and the following disclaimer in the
 documentation and/or other materials provided with the distribution.
 * Neither the name of the company nor the
 names of its contributors may be used to endorse or promote products
 derived from this software without specific prior written permission.
 
 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL o-labs BE LIABLE FOR ANY
 DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.*/

#include "SensuinoSerial.h"

const int http_timeout = 11000;


SerialHAL::SerialHAL() {
	
}

boolean SensuinoSerial::init(byte *err, ...)
{
	va_list argp;
	va_start(argp,err);
	
	Serial.begin(va_arg(argp,int));
	va_end(argp);
	Serial.println("                                             ");
	return true;
}





boolean SensuinoSerial::postSense(unsigned long feed_id, char* txData) {
	SenseData s;
	s.feed_id = feed_id;
	s.value = txData;
	return postSense(1, &s);
}

boolean SensuinoSerial::postSense(byte size, SenseData* data) {
	
	Serial.write(FLAG);
	Serial.write(BEGIN);	
	_gpclient.write(POST);
	_gpclient.write(size);
	for(byte i=0;i<size;i++)
	{
		_gpclient.write((byte)((data[i].feed_id>>24)&0xFF));
		_gpclient.write((byte)((data[i].feed_id>>16)&0xFF));
		_gpclient.write((byte)((data[i].feed_id>>8)&0xFF));
		_gpclient.write((byte)((data[i].feed_id)&0xFF));
		_gpclient.write((byte)((strlen(data[i].value)>>24)&0xFF));
		_gpclient.write((byte)((strlen(data[i].value)>>16)&0xFF));
		_gpclient.write((byte)((strlen(data[i].value)>>8)&0xFF));
		_gpclient.write((byte)((strlen(data[i].value))&0xFF));
		_gpclient.write(data[i].value);
	}	
	_gpclient.write((byte)((strlen(senseKey)>>24)&0xFF));
	_gpclient.write((byte)((strlen(senseKey)>>16)&0xFF));
	_gpclient.write((byte)((strlen(senseKey)>>8)&0xFF));
	_gpclient.write((byte)((strlen(senseKey))&0xFF));
	_gpclient.print(senseKey);
	Serial.write(END);
	
	unsigned long _start = millis();
	
	//read timeout
	while((millis() < (_start + http_timeout))) {
		if(_gpclient.available()) {
			_gpclient.find("HTTP/1.1 ");
			char c = _gpclient.read();
			char d = 1;
			while(d > 0) {
				d = _gpclient.read();
			}
			if(c != '2') {
				return false;
			}
			return true;
		}
	}
	return false;
}


boolean SensuinoSerial::getSense(unsigned long feed_id) {
	//FIXME: seems buggy.
	boolean ret = false;
	unsigned long timeRef;
	if (longPollSenseImpl(&_gpclient, feed_id, "-1", false, true)) {
		byte check = 0;	
		timeRef = millis();
		do {
			check = longPollCheck(&_gpclient,false);
			if(check == 0) {
				Serial.println();
				ret = true;
				break;
			}
			if(millis()-timeRef>GET_TIMEOUT)
			{
				Serial.println("GET TIMEOUT");
				break;
			}
		} while(check != 2);
	} else {
		Serial.println("longPollSense: false");
	}
 	
	return ret;
}


boolean SensuinoSerial::longPollSenseImpl(unsigned long feed_id, char event_id[16]) {
	return longPollSenseImpl(&_gpclient, feed_id, event_id, true, false);
}

boolean SensuinoSerial::longPollSenseImpl(NetworkHAL *client, unsigned long feed_id, char event_id[16], boolean is_device, boolean req_type) {
	
	Serial.write(FLAG);
	Serial.write(BEGIN);

	client->write(LONGPOLL);
	if(is_device)
		client->write((byte)DEVICE);
	else
		client->write((byte)FEED);
	client->write((byte)((feed_id>>24)&0xFF));
	client->write((byte)((feed_id>>16)&0xFF));
	client->write((byte)((feed_id>>8)&0xFF));
	client->write((byte)((feed_id)&0xFF));
	if(req_type)
		client->write((byte)REQ_LAST);
	else
		client->write((byte)NREQ_LAST);
	client->write((byte)strlen(event_id));
	client->print(event_id);
	client->write((byte)((strlen(senseKey)>>24)&0xFF));
	client->write((byte)((strlen(senseKey)>>16)&0xFF));
	client->write((byte)((strlen(senseKey)>>8)&0xFF));
	client->write((byte)((strlen(senseKey))&0xFF));
	client->print(senseKey);
	Serial.write(END);
	
	boolean ret = false;
	unsigned long timeRef;
	
	timeRef = millis();
	while(client->available()<1)
	{
		if(millis()-timeRef>TIMEOUT_RECEIVE)
		{
			Serial.println("TIMEOUT (nothing on Serial)");
			return false; 
		}
	}
	
	
	if(client->available()>0)
	{
		timeRef = millis();
		while(client->read()!=FLAG)
		{
			if(millis()-timeRef>TIMEOUT_RECEIVE)
			{
				Serial.println("Data format error (no FLAG)");
				return false; 
			}
		}
		
		if(client->read() == BEGIN)
		{
			byte cmd = client->read();
			if (cmd == RESP_LONGPOLL)
			{
				return client->read();
			}
			else
			{
				return false;
			}
		}
	}
}

byte SensuinoSerial::longPollCheck() {
	return longPollCheck(&_gpclient);
}

byte SensuinoSerial::longPollCheck(NetworkHAL *client, boolean saveEventId) {
	unsigned long timeRef;
	byte ret = 2;
	
	
	Serial.write(FLAG);
	Serial.write(BEGIN);
	client->write((byte)LONGPOLL);
	client->write((byte)LONGPOLL_CHECK);
	Serial.write(END);
	
	
	
	timeRef = millis();
	while(client->available()<1)
	{
		if(millis()-timeRef>TIMEOUT_RECEIVE)
		{
			Serial.println("TIMEOUT (nothing on Serial)");
			return 1;
		}
	}
	
	
	if(client->available()>0)
	{
		timeRef = millis();
		while(client->read()!=FLAG)
		{
			if(millis()-timeRef>TIMEOUT_RECEIVE)
			{
				Serial.println("Data format error (no FLAG)");
				return 1;
			}
		}
	}
	
	if(client->read() == BEGIN)
	{
		if (client->read() == RESP_LONGPOLL)
		{
			if(client->read() == '0')
			{
				parseLongPollResponse(client, saveEventId) ? ret = 0 : ret = 2;
			}
			else
			{
				ret = 1;
			}
		}
		else
		{
			ret = 1;
		}
	}
	else
	{
		ret = 1;
	}
	
	return ret;
}


boolean SerialHAL::connect()
{
	return true;
}


boolean SerialHAL::connected()
{
	return true;
}


uint8_t SerialHAL::status()
{
	return 0;
}

void SerialHAL::disconnect()
{

}

int SerialHAL::read()
{
	int ret;
	long timeRef;
	
	timeRef = millis();
	while (Serial.available()<1) 
	{
		if ((millis()-timeRef)>READ_TIMEOUT) 
		{
			return -1;
		}
	}
	
	byte readByte = Serial.read();
	if (readByte == ESCAPE)
	{
		long timeRef = millis();
		while (Serial.available()<1) 
		{
			if ((millis()-timeRef)>READ_TIMEOUT) 
			{
				return -1;
			}
		}
		switch (Serial.peek()) 
		{
			case ESCAPE_REPLACE:
				Serial.read();
				ret = ESCAPE;
				break;
				
			case FLAG_REPLACE:
				Serial.read();
				ret = FLAG;
				break;
				
			case END_REPLACE:
				Serial.read();
				ret = END;
				break;

			default:
				ret = readByte; 
				break;
		}
	}
	else 
	{
		ret = readByte;
	}
	
	return ret;
}


int SerialHAL::available()
{
	return Serial.available();
}	

int SerialHAL::peek()
{
	return Serial.peek();
}

void SerialHAL::flush()
{
	Serial.flush();
}


size_t SerialHAL::write(uint8_t c)
{
	if (c == FLAG) 
	{
		Serial.write(ESCAPE);
		Serial.write(FLAG_REPLACE);
	}
	else if (c == ESCAPE)
	{
		Serial.write(ESCAPE);
		Serial.write(ESCAPE_REPLACE);
	}
	else if (c == END)
	{
		Serial.write(ESCAPE);
		Serial.write(END_REPLACE);
	}
	else 
	{
		Serial.write(c);
	}
	return 1;	
}


