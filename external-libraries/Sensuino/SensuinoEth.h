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
 
#ifndef __SENSUINO_ETH_H__
#define __SENSUINO_ETH_H__

#include "Sensuino.h"
#include <EthernetClient.h>

class EthHAL : public NetworkHAL {
public:
	EthHAL();
	void setClient(EthernetClient *);
	boolean connect();
	boolean connected();
	uint8_t status();
	void disconnect();
	int read();
	int peek();
	void flush();
	void write(char* buf);
	size_t write(uint8_t val);
	void print(unsigned long val, int base);
	int available();
	void setIpPort(byte *, int);

private:
	EthernetClient *_client;
	byte *_server;
	int _port;
};

class SensuinoEth : public Sensuino {
public:
	boolean init(byte *err, ...);
	boolean postSense(unsigned long feed_id, char* txData);
	boolean postSense(byte size, SenseData* data);
	boolean getSense(unsigned long feed_id);
  	byte longPollCheck();

protected:
	byte _mac[6];
	byte _ip[4];
	byte _gateway[4];
	boolean _dhcp;
	
	EthHAL _gpclient;
	EthHAL _lpclient;
	
	boolean longPollSenseImpl(unsigned long feed_id, char event_id[16]);
	boolean longPollSenseImpl(NetworkHAL *client, unsigned long feed_id, char event_id[16], boolean is_device=true, boolean req_type=false);
	byte longPollCheck(NetworkHAL*, boolean saveEventId=true);
	
	void tokenize(char* stringToBytes, byte* destTab);
	void tokenize16(char* stringToBytes, byte* destTab);
};

#endif
