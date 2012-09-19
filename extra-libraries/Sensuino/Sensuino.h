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

#ifndef __SENSUINO_H__
#define __SENSUINO_H__

#include "Arduino.h"

#define RX_BUFFER_SIZE	255
#define SENSE_KEY_SIZE	25

#define SENSE_PROD		{46,4,10,208}
#define SENSE_DEV		{46,4,52,140}
#define SENSE_IP		SENSE_PROD

#define PORT_TO_REACH	80

class SenseData {
public:
	unsigned long feed_id;
	char* value;
};

class NetworkHAL : public Stream {
public:	
	virtual boolean connect() = 0;
	virtual boolean connected() = 0;
	virtual uint8_t status() = 0;
	virtual void disconnect() = 0;
	void skipChar(int);
};

class Sensuino {
public:
	virtual boolean init(byte *err, ...);
	virtual boolean postSense(unsigned long feed_id, char* txData) = 0;
	virtual boolean getSense(unsigned long feed_id) = 0;
  	virtual byte longPollCheck();
	
	virtual boolean longPollSenseImpl(unsigned long feed_id, char event_id[16])=0;
	boolean longPollSense(unsigned long feed_id);

	void setSenseKey(char* senseKey);
	char* getLastValue();
	unsigned long getLastFeedId();

private:
  	
protected:
	char rxBuffer[RX_BUFFER_SIZE];
	char senseKey[SENSE_KEY_SIZE];
	unsigned long rxFeedId;
	
	
	
	boolean parseResponse(char* feed_id, char* value);
  	boolean parseLongPollResponse(NetworkHAL *, boolean saveEventId=true);
};

#endif

