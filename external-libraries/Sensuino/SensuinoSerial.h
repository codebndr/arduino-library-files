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

#ifndef __SENSUINO_SERIAL_H__
#define	__SENSUINO_SERIAL_H__

#include "Sensuino.h"


#define FLAG 0x7E
#define ESCAPE 0xCE
#define FLAG_REPLACE 0x5E
#define ESCAPE_REPLACE 0x9E
#define BEGIN 0x2E
#define END 0xBE
#define END_REPLACE 0xDE

#define POST 0x04
#define GET 0x05
#define RESP_POST 0x06
#define RESP_GET 0x07
#define	SERV_CONF 0x08
#define ASK_POST 0x0B
#define NO_POST 0x0C
#define LONGPOLL 0x0D
#define LONGPOLL_CHECK 0x0E
#define RESP_LONGPOLL 0x0F
#define REQ_LAST 0x01
#define NREQ_LAST 0x00
#define DEVICE 0x01
#define FEED 0x00

#define TIMEOUT_RECEIVE 2000
#define READ_TIMEOUT	100
#define GET_TIMEOUT		5000


class SerialHAL : public NetworkHAL {
public:
	SerialHAL();
	boolean connect();
	boolean connected();
	uint8_t status();
	void disconnect();

	int available(void);
    int peek(void);
	int read(void);
    void flush(void);
	size_t write(uint8_t);
    using Print::write;
	
//private:
	

};


class SensuinoSerial : public Sensuino {

public:
	boolean init(byte *err, ...);
	boolean postSense(unsigned long feed_id, char* txData);
	boolean postSense(byte size, SenseData* data);
	boolean getSense(unsigned long feed_id);
  	byte longPollCheck();
	
protected:
	
	SerialHAL _gpclient;
	
	boolean longPollSenseImpl(unsigned long feed_id, char event_id[16]);
	boolean longPollSenseImpl(NetworkHAL *client, unsigned long feed_id, char event_id[16], boolean is_device=true, boolean req_type=false);
	byte longPollCheck(NetworkHAL*, boolean saveEventId=true);
};

#endif