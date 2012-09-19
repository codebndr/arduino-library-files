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
 
#include "Sensuino.h"

char _event_id[16] = "-2"; 

boolean Sensuino::parseResponse(char* feed_id, char* value) {
	return true;
}

boolean Sensuino::parseLongPollResponse(NetworkHAL *client, boolean saveEventId) {
	char readChar;
	byte i = 0;
	
	client->find("HTTP/1.1 ");
	
	char c = client->read();
	
	//parse http code: 200
	if(c != '2') {
		while(c != 0) {
			c = client->read();
		}

		client->disconnect();
		return false;
	}
	
	//strip headers
	client->find("\r\n\r\n");
	client->find("\"");
	
	i = 0;
	
	if(saveEventId)
		memset(_event_id, '\0', 16);
	while((readChar = client->read()) != '"') {
		if(saveEventId)
			_event_id[i] = readChar;
		i++;
	}
	
	client->find("\"");
	
	memset(rxBuffer, '\0', RX_BUFFER_SIZE);
	i=0;
	while((readChar = client->read()) != '"') {
		rxBuffer[i] = readChar;
		i++;
	}
	
	client->find("\"");
	client->find("\"");
	client->skipChar(2);
	
	char buf[16];
	memset(buf, '\0', 16);
	i=0;
	while((readChar = client->read()) != ',') {
		buf[i] = readChar;
		i++;
	}
	
	rxFeedId = atoi(buf);
	
	client->disconnect();
	return true;

}

boolean Sensuino::longPollSense(unsigned long feed_id) {
	return longPollSenseImpl(feed_id, _event_id);
}

void Sensuino::setSenseKey(char* senseKey) {
	for (byte b=0; b<min(strlen(senseKey),SENSE_KEY_SIZE); b++) {
		this->senseKey[b] = senseKey[b];
	}
	this->senseKey[min(strlen(senseKey),SENSE_KEY_SIZE)] = 0;
}

char* Sensuino::getLastValue() {
	return rxBuffer;
}

unsigned long Sensuino::getLastFeedId() {
	return rxFeedId;
}

void NetworkHAL::skipChar(int count) {
	int i=0;
	for(i=0;i<count;i++)
		read();
}
