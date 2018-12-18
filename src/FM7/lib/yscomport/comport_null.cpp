#include "comport.h"



class YsCOMPort::PortHandle
{
public:
};




/* static */ std::vector <int> YsCOMPort::FindAvailablePort(void)
{
	std::vector <int> available;
	return available;
}
bool YsCOMPort::Open(int portNumber)
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
