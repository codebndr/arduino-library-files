/*
  XBeeRadio.h - Library for communicating with heterogenous 802.15.4 networks.
  Created by Vasileios Georgitzikis, November 23, 2010.
*/

#ifndef XbeeRadio_h
#define XbeeRadio_h

#include <XBee.h>
#include "Arduino.h"

#ifdef DEBUG
  #define DBG(X) X
#else
  #define DBG(X) 
#endif
class XBeeRadioResponse : public XBeeResponse
{
public:
	// const static uint8_t LP1  = 0x7f;
	// const static uint8_t LP2  = 0x69;
	// const static uint8_t DEFAULT_PORT = 110;
	XBeeRadioResponse() : XBeeResponse(){}
	uint8_t getDataLength();
	uint8_t getData(int index);
	uint8_t* getData();
	uint8_t validPacket();
	uint8_t validPacket(uint8_t valid_port);
	uint8_t validPacket(uint8_t lp1,uint8_t lp2,uint8_t port);
	uint8_t getRssi();
};

class XBeeRadio: public XBee
{
public:
	const static uint8_t LP1  = 0x7f;
	const static uint8_t LP2  = 0x69;
	const static uint8_t DEFAULT_PORT = 110;
	const static uint8_t PROGRAMMING_PORT = 42;
	const static uint8_t LED_PIN = 13;
	XBeeRadio() : XBee(){}
	void getResponse(XBeeRadioResponse &response);
	XBeeRadioResponse& getResponse();
	uint16_t myAddress;
	uint16_t getMyAddress();
	bool setChannel(uint8_t channel);
	void send(Tx16Request &request);
	void send(Tx16Request &request, uint8_t port);
	void send(AtCommandRequest);
	bool sendAndCheck(Tx16Request &request);
	bool sendAndCheck(Tx16Request &request, uint8_t port);
	void sendAtCommand(uint8_t command[], uint8_t reply[]);
	void sendAtCommand(uint8_t command[], uint8_t value[], uint8_t length);
	uint8_t init(/*NewSoftSerial mySerial*/void);
	uint8_t init(uint8_t channel);
	bool checkForData(void);
	bool checkForData(uint8_t valid_port);
	bool checkForData(uint8_t lp1,uint8_t lp2,uint8_t port);
	
	void initialize_xbee_module();
	void initialize_xbee_module(long baudrate);
private:
	uint8_t trySendingCommand(uint8_t buffer[2], AtCommandRequest atRequest,AtCommandResponse atResponse);
	void getReadyForProgramming(uint16_t programmer_address);
	
	bool setup_command(char* command);
	long setup_baudrate(void);
	long setup_baudrate(long starting_baud);
	bool setup_xbee(long baudrate);
	bool wait_for_response(unsigned long milliseconds);
	bool check_for_response(void);
};

#endif


