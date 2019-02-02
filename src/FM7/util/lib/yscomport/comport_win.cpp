#include <thread>
#include <chrono>

#include <windows.h>

#include "comport.h"



class YsCOMPort::PortHandle
{
public:
	HANDLE hComm;
};




/* static */ std::vector <int> YsCOMPort::FindAvailablePort(void)
{
	std::vector <int> available;
	for(int port=1; port<10; ++port)
	{
		char fn[256];
		sprintf(fn,"COM%d",port);
		printf("Testing %s\n",fn);

		HANDLE hComm=CreateFile(
			fn,
			GENERIC_READ|GENERIC_WRITE,
			0,
			NULL,
			OPEN_EXISTING,
			0,
			NULL);
		if(INVALID_HANDLE_VALUE!=hComm)
		{
			printf("Found Port %d.\n",port);
			available.push_back(port);
			CloseHandle(hComm);
		}
	}
	return available;
}

static void TryBaudRate(HANDLE hComm,DCB &dcb,int baud)
{
	dcb.BaudRate=baud;
	if(0==SetCommState(hComm,&dcb))
	{
		printf("Failed to set %d bits-per-second.\n",baud);
	}
}

bool YsCOMPort::Open(int portNumber)
{
	Close();

	char fn[256];
	sprintf(fn,"COM%d",portNumber);

	HANDLE hComm=CreateFile(
		fn,
		GENERIC_READ|GENERIC_WRITE,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_FLAG_NO_BUFFERING,
		NULL);
	if(INVALID_HANDLE_VALUE==hComm)
	{
		return false;
	}

	this->portNumber=portNumber;
	this->portPtr=new PortHandle;
	this->portPtr->hComm=hComm;


	// Thanks Mr. Apollo!
	// FM77AV keyboard takes 12.5us on and 12.5us off cycle.
	// One on or off cycle needs two bytes of data.
	// 16 bits for 12.5us -> 80000x16=1280000 bits-per-second.
	// Win32 API tells it can go up to 1600000 bits-per-second, but the question is if it is real??

	DCB dcb;
	if(TRUE==GetCommState(hComm,&dcb))
	{
		// Set up polling-based operation.
		// Based on the information
		// https://groups.google.com/forum/#!topic/comp.os.ms-windows.programmer.win32/SotVc2_Eiig
		COMMTIMEOUTS timeOut;
		ZeroMemory(&timeOut,sizeof(timeOut));
		timeOut.ReadIntervalTimeout=MAXDWORD;
		timeOut.ReadTotalTimeoutMultiplier=0;
		timeOut.ReadTotalTimeoutConstant=0;
		timeOut.WriteTotalTimeoutMultiplier=1000;
		timeOut.WriteTotalTimeoutConstant=0;
		SetCommTimeouts(hComm,&timeOut);


		printf("Default Bits/Byte=%d\n",dcb.ByteSize);
		switch(desiredBitLength)
		{
		case BITLENGTH_7:
			dcb.ByteSize=7;
			break;
		case BITLENGTH_8:
			dcb.ByteSize=8;
			break;
		}

		switch(dcb.StopBits)
		{
		case ONESTOPBIT:
			printf("Default StopBits=1\n");
			break;
		case ONE5STOPBITS:
			printf("Default StopBits=1.5\n");
			break;
		case TWOSTOPBITS:
			printf("Default StopBits=2\n");
			break;
		}
		switch(desiredStopBit)
		{
		case STOPBIT_1:
			dcb.StopBits=ONESTOPBIT;
			break;
		case STOPBIT_1_5:
			dcb.StopBits=ONE5STOPBITS;
			break;
		case STOPBIT_2:
			dcb.StopBits=TWOSTOPBITS;
			break;
		}

		switch(dcb.Parity)
		{
		case EVENPARITY:
			printf("Default Parity=2\n");
			break;
		case MARKPARITY:
			printf("Default Parity=Mark\n");
			break;
		case NOPARITY:
			printf("Default Parity=None\n");
			break;
		case ODDPARITY:
			printf("Default Parity=Odd\n");
			break;
		case SPACEPARITY:
			printf("Default Parity=Space\n");
			break;
		}
		switch(desiredParity)
		{
		case PARITY_NOPARITY:
			dcb.Parity=NOPARITY;
			break;
		case PARITY_EVENPARITY:
			dcb.Parity=EVENPARITY;
			break;
		case PARITY_ODDPARITY:
			dcb.Parity=ODDPARITY;
			break;
		case PARITY_MARKPARITY:
			dcb.Parity=MARKPARITY;
			break;
		case PARITY_SPACEPARITY:
			dcb.Parity=SPACEPARITY;
			break;
		}

		switch(desiredFlowControl)
		{
		case FLOWCONTROL_NONE:
			dcb.fOutxCtsFlow=FALSE;
			dcb.fRtsControl=RTS_CONTROL_ENABLE;
			dcb.fDtrControl=DTR_CONTROL_ENABLE;
			dcb.fOutX=FALSE;
			dcb.fInX=FALSE;
			break;
		case FLOWCONTROL_HARDWARE:
			dcb.fOutxCtsFlow=TRUE;
			dcb.fRtsControl=RTS_CONTROL_ENABLE;
			dcb.fDtrControl=DTR_CONTROL_ENABLE;
			break;
		case FLOWCONTROL_XON_XOFF:
			// Note: Good for text transmission only
			dcb.fOutX=TRUE;
			dcb.fInX=TRUE;
			break;
		}



		printf("Default Baud Rate=%d\n",dcb.BaudRate);

		if(0!=desiredBaudRate)
		{
			dcb.BaudRate=desiredBaudRate;
			if(0!=SetCommState(hComm,&dcb))
			{
				goto SPEEDSET;
			}
		}

		printf("Set up for polling-mode.  I need to be fast!\n");

		TryBaudRate(hComm,dcb,38400);
		TryBaudRate(hComm,dcb,57600);
		TryBaudRate(hComm,dcb,115200);
		TryBaudRate(hComm,dcb,128000);
		TryBaudRate(hComm,dcb,256000);
		TryBaudRate(hComm,dcb,921600);
		TryBaudRate(hComm,dcb,1280000);
		TryBaudRate(hComm,dcb,1600000);

	SPEEDSET:
		GetCommState(hComm,&dcb);
		printf("Speed set to %d bits-per-second.\n",dcb.BaudRate);
	}
	else
	{
		Close();
		return false;
	}

	EscapeCommFunction(this->portPtr->hComm,CLRDTR);
	std::this_thread::sleep_for(std::chrono::milliseconds(200));
	EscapeCommFunction(this->portPtr->hComm,SETDTR);
	std::this_thread::sleep_for(std::chrono::milliseconds(200));
	EscapeCommFunction(this->portPtr->hComm,CLRRTS);
	std::this_thread::sleep_for(std::chrono::milliseconds(200));
	EscapeCommFunction(this->portPtr->hComm,SETRTS);
	std::this_thread::sleep_for(std::chrono::milliseconds(200));

	return true;
}

void YsCOMPort::Close(void)
{
	if(nullptr!=portPtr)
	{
		CloseHandle(portPtr->hComm);
		delete portPtr;
		portPtr=nullptr;
		portNumber=-1;
	}
}

bool YsCOMPort::Update(void)
{
	return true;
}

long long int YsCOMPort::Send(long long int nDat,const unsigned char dat[])
{
	if(nullptr!=this->portPtr && INVALID_HANDLE_VALUE!=this->portPtr->hComm)
	{
		DWORD byteWritten=0;
		WriteFile(this->portPtr->hComm,dat,(DWORD)nDat,&byteWritten,NULL);
		return byteWritten;
	}
	return 0;
}

void YsCOMPort::SendBreak(void)
{
	std::this_thread::sleep_for(std::chrono::milliseconds(1));
}

void YsCOMPort::FlushWriteBuffer(void)
{
	if(nullptr!=this->portPtr && INVALID_HANDLE_VALUE!=this->portPtr->hComm)
	{
		FlushFileBuffers(this->portPtr->hComm);
	}
}

bool YsCOMPort::GetCTS(void) const
{
	DWORD stat=0;
	if(nullptr!=this->portPtr && INVALID_HANDLE_VALUE!=this->portPtr->hComm)
	{
		GetCommModemStatus(this->portPtr->hComm,&stat);
	}
	return 0!=(MS_CTS_ON & stat);
}
bool YsCOMPort::GetDSR(void) const
{
	DWORD stat=0;
	if(nullptr!=this->portPtr && INVALID_HANDLE_VALUE!=this->portPtr->hComm)
	{
		GetCommModemStatus(this->portPtr->hComm,&stat);
	}
	return 0!=(MS_DSR_ON & stat);
}
void YsCOMPort::SetDTR(bool dtr)
{
	if(true!=dtr)
	{
		EscapeCommFunction(this->portPtr->hComm,CLRDTR);
	}
	else
	{
		EscapeCommFunction(this->portPtr->hComm,SETDTR);
	}
}
void YsCOMPort::SetRTS(bool rts)
{
	if(true!=rts)
	{
		EscapeCommFunction(this->portPtr->hComm,CLRRTS);
	}
	else
	{
		EscapeCommFunction(this->portPtr->hComm,SETRTS);
	}
}

std::vector <unsigned char> YsCOMPort::Receive(void)
{
	std::vector <unsigned char> dat;

	if(nullptr!=this->portPtr && INVALID_HANDLE_VALUE!=this->portPtr->hComm)
	{
		const int maxNByte=256;
		unsigned char readBuf[maxNByte];

		for(;;)
		{
			DWORD byteRead=0;
			if(TRUE==ReadFile(this->portPtr->hComm,readBuf,maxNByte,&byteRead,NULL))
			{
				for(int i=0; i<byteRead; ++i)
				{
					dat.push_back(readBuf[i]);
				}
			}
			else
			{
				break;
			}
			if(0==byteRead)
			{
				break;
			}
		}
	}

	return dat;
}
