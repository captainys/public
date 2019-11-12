#include "comport.h"



class YsCOMPort::PortHandle
{
public:
};




/* static */ std::vector <std::string> YsCOMPort::FindAvailablePortName(void)
{
	std::vector <std::string> available;
	return available;
}
bool YsCOMPort::Open(const std::string &port)
{
	return false;
}

bool YsCOMPort::ChangeBaudRate(int baudRate)
{
	return false;
}

void YsCOMPort::Close(void)
{
}

bool YsCOMPort::Update(void)
{
	return true;
}

long long int YsCOMPort::Send(long long int nDat,const unsigned char dat[])
{
	return 0;
}

void YsCOMPort::SendBreak(void)
{
}

void YsCOMPort::FlushWriteBuffer(void)
{
}

std::vector <unsigned char> YsCOMPort::Receive(void)
{
	std::vector <unsigned char> dat;
	return dat;
}
