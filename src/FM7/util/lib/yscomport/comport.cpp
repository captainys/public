#include "comport.h"



YsCOMPort::YsCOMPort()
{
	portPtr=nullptr;
	portName="";

	desiredBaudRate=0;
	desiredBitLength=BITLENGTH_8;
	desiredStopBit=STOPBIT_DONTCARE;
	desiredParity=PARITY_DONTCARE;
	desiredFlowControl=FLOWCONTROL_NONE;
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
#ifdef _WIN32
int YsCOMPort::GetPort(void) const
{
	return atoi(portName.c_str());
}
#endif

std::string YsCOMPort::GetPortName(void) const
{
	return portName;
}

int YsCOMPort::GetDesiredBaudRate(void) const
{
	return desiredBaudRate;
}

void YsCOMPort::CleanUp(void)
{
	Close();
	sendBuf.clear();
	receiveBuf.clear();
}
