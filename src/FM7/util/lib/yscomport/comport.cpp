#include "comport.h"



YsCOMPort::YsCOMPort()
{
	portPtr=nullptr;
	portNumber=-1;

	desiredBaudRate=0;
	desiredBitLength=BITLENGTH_DONTCARE;
	desiredStopBit=STOPBIT_DONTCARE;
	desiredParity=PARITY_DONTCARE;
	desiredFlowControl=FLOWCONTROL_DONTCARE;
}
YsCOMPort::~YsCOMPort()
{
	CleanUp();
}
void YsCOMPort::SetDesiredBaudRate(int baud)
{
	desiredBaudRate=baud;
}
void YsCOMPort::SetDesiredBitLength(BITLENGTH bitLen)
{
	desiredBitLength=bitLen;
}

void YsCOMPort::SetDesiredStopBit(STOPBIT stopBit)
{
	desiredStopBit=stopBit;
}

void YsCOMPort::SetDesiredParity(PARITY parity)
{
	desiredParity=parity;
}

void YsCOMPort::SetDesiredFlowControl(FLOWCONTROL flowCtrl)
{
	desiredFlowControl=flowCtrl;
}

bool YsCOMPort::IsConnected(void) const
{
	return (nullptr!=portPtr);
}
int YsCOMPort::GetPort(void) const
{
	return portNumber;
}
void YsCOMPort::CleanUp(void)
{
	Close();
	sendBuf.clear();
	receiveBuf.clear();
}
