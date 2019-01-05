#ifndef IRTOY_IS_INCLUDED
#define IRTOY_IS_INCLUDED
/* { */

#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <queue>
#include <string>
#include <chrono>

#include "comport.h"

class IRToy_Controller
{
public:
	enum
	{
		CMD_RESET=0,
		CMD_SELFTEST='t',
		CMD_SAMPLERMODE='S',
		CMD_ENABLE_BYTECOUNT=0x24,
		CMD_ENABLE_TRANSMISSION_NOTIFY=0x25,
		CMD_ENABLE_HANDSHAKE=0x26,
		CMD_TRANSMIT=0x03,
	};
	enum
	{
		STATE_DISCONNECT,

		STATE_INITIALIZING,
		STATE_HANDSHAKE,
		STATE_SELFTEST,
		STATE_SETSAMPLERMODE,
		STATE_CONFIGURE,
		STATE_GOWILD,
		STATE_RECORDING,
		STATE_TRANSMITTING,

		STATE_CLOSING,
		STATE_CLOSED,

		STATE_ERROR,

	STATE_NULL=0x7fffffff,
	};
	enum
	{
		ERROR_NOERROR,
		ERROR_INITIALIZATION_TIMEOUT,
		ERROR_PACKET_SEND_FAILURE,
		ERROR_IRTOY_NOT_FOUND,

	ERROR_NULL=0x7fffffff,
	};

private:
	int state;
	int errorCode;
	int stateCounter;
	bool stateCommandSent;
	int nAccumByte;
	std::chrono::time_point <std::chrono::system_clock> stateTimer0;

	std::queue <unsigned char> log;

	char irToyVersion[5];
	char protocolVersion[4];

	int transmitPtr;
	std::vector <unsigned char> recording;
	std::vector <unsigned char> feedBackBuf;

	// If not STATE_NULL, it will not change the state from it.
	int holdStateForDebug;

	YsCOMPort comPort;

	void AddLog(unsigned char d1,unsigned char d2);
public:
	/*! Returns COM-port transaction log.
	    For FM77AV keyboard emulator, it is possible to recover what you typed.  
	    But, first of all, I'm not sending it to anywhere else.
	    I have no intention to write a key-code decoder.
	    Don't worry.
	*/
	std::vector <unsigned char> GetLog(void) const;

private:
	void Send(long long int nByte,const unsigned char dat[]);
	std::vector <unsigned char> Read(void);

	/*  Once the state becomes STATE_ERROR, it can only change to STATE_INITIALIZING or STATE_DISCONNECT
	*/
	void ChangeState(int nextState);
	static void MakeTerminator(std::vector <unsigned char> &dat);
	static void PrintRecording(const std::vector <unsigned char> &dat);
	static void ShowBuf(long long int nByte,const unsigned char dat[]);

	/*!
	If IRToy enters a firmware flash mode, there is no recovery with Windows 10.

	I was lucky I still had one Windows 7 PC.  Still it was tricky.  I first thought
	the device was completely destroyed.

	It can happen if while sending 0xff 0xff is sent prematurely before the buffer is done,
	and then '$' is sent.
	Therefore, I need to make sure:
	(1) Nothing after 0xff 0xff in recording, and
	(2) Recording terminates with 0xff 0xff
	*/
	static void CleanRecording(std::vector <unsigned char> &dat);

public:
	IRToy_Controller();

	bool Connect(int portNumber);
	bool IsConnected(void) const;
	int GetPort(void) const;
	void Disconnect();

public:
	void Reset(void);

public:
	void SetIRToyNotFoundError(void);
	void SetHoldStateForDebug(int state);
	void StartCloseIRToySequence(void);

private:
	void ResetStateTimer(void);
	long long int StateTimer(void) const;

	void State_Initializing(void);
	void State_Handshake(void);
	void State_SelfTest(void);
	void State_SetSamplerMode(void);
	void State_Configure(void);
	void State_GoWild(void);

	void State_Closing(void);
	void State_Closed(void);

public:
	bool StartRecording(void);
	bool EndRecording(void);
	bool IsInRecordingMode(void) const;
	std::vector <unsigned char> GetRecording(void) const;
private:
	void State_Recording(void);

public:
	/*! Load recorded data and make it ready for transmission.
	*/
	bool SetRecording(const std::vector <unsigned char> &recorded);

	/*! Transmit recorded data.
	*/
	bool StartTransmit(void);
private:
	void State_Transmitting(void);

public:
	void SaveRecording(const char fName[]) const;
	void LoadRecording(const char fName[]);

	void Make100usPulse(const char ptn[],bool verbose=false);

	void RunOneStep(void);

	int GetState(void) const;
	int GetErrorCode(void) const;

	std::string GetIRToyVersion(void) const;
	std::string GetProtocolVersion(void) const;
};

/* } */
#endif
