#ifndef COMPORT_IS_INCLUDED
#define COMPORT_IS_INCLUDED
/* { */



#include <vector>

class YsCOMPort
{
public:
	class PortHandle;
	enum BITLENGTH
	{
		BITLENGTH_DONTCARE,
		BITLENGTH_7,
		BITLENGTH_8
	};
	enum STOPBIT
	{
		STOPBIT_DONTCARE,
		STOPBIT_1,
		STOPBIT_1_5,
		STOPBIT_2
	};
	enum PARITY
	{
		PARITY_DONTCARE,
		PARITY_NOPARITY,
		PARITY_EVENPARITY,
		PARITY_ODDPARITY,
		PARITY_MARKPARITY,
		PARITY_SPACEPARITY
	};
	enum FLOWCONTROL
	{
		FLOWCONTROL_DONTCARE,
		FLOWCONTROL_NONE,
		FLOWCONTROL_HARDWARE,
		FLOWCONTROL_XON_XOFF
	};

private:
	/*  Platform-dependent code must be written so that portPtr is nullptr when it is disconnected.
	*/
	PortHandle *portPtr;
	std::vector <unsigned char> sendBuf;
	std::vector <unsigned char> receiveBuf;
	int portNumber;
	int desiredBaudRate;
	BITLENGTH desiredBitLength;
	STOPBIT desiredStopBit;
	PARITY desiredParity;
	FLOWCONTROL desiredFlowControl;

public:
	YsCOMPort();
	~YsCOMPort();
	void CleanUp(void);

	static std::vector <int> FindAvailablePort(void);

	/*! Set desired baud rate.  Call this before Open if necessary.
	    By default, it tries to make it the fastest baud rate.
	*/
	void SetDesiredBaudRate(int baud);

	/*! Set desired bit length.  Call this before Open if necessary.
	    By default, it is BITLENGTH_DONTCARE. */
	void SetDesiredBitLength(BITLENGTH bitLen);

	/*! Set desired stop bit.  Call this before Open if necessary.
	    By default, it is STOPBIT_DONTCARE.
	*/
	void SetDesiredStopBit(STOPBIT stopBit);

	/*! Set desired parity.  Call this before Open if necessary.
	    By default, it is PARITY_DONTCARE.
	*/
	void SetDesiredParity(PARITY parity);

	/*! Set desired flow control.  Call this before Open if necessary.
	    By default, it is FLOWCONTROL_DONTCARE.
	*/
	void SetDesiredFlowControl(FLOWCONTROL flowCtrl);


	/*! 
	*/
	bool Open(int portNumber);

	/*! Returns true if port is connected.
	*/
	bool IsConnected(void) const;

	/*! Returns connected port number.
	*/
	int GetPort(void) const;

	/*! 
	*/
	void Close(void);

	/*! Call this function regularly to update the COM port status.
	*/
	bool Update(void);

	/*! Sends byte data and returns number of bytes sent.
	    It may just have copied data to the send buffer, 
	    in which case the return value will be the number of bytes copied to the send buffer.
	*/
	long long int Send(long long int nDat,const unsigned char dat[]);

	/*! Send BREAK signal.
	*/
	void SendBreak(void);

	/*! Flush the write-buffer.
	*/
	void FlushWriteBuffer(void);

	/*! Get CTS state. */
	bool GetCTS(void) const;

	/*! Get DSR state. */
	bool GetDSR(void) const;

	/*! Set DTR signal. */
	void SetDTR(bool dtr);

	/*! Set RTS signal. */
	void SetRTS(bool rts);

	/*! Reads byte data from the port.
	*/
	std::vector <unsigned char> Receive(void);
};



/* } */
#endif
