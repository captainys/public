#define _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES 1
#define _CRT_SECURE_CPP_OVERLOAD_STANDARD_NAMES_COUNT 1
#define _CRT_SECURE_NO_WARNINGS 1

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
	for(int port=1; port<32; ++port)
	{
		char fn[256];
		if(port<10)
		{
			sprintf(fn,"COM%d",port);
		}
		else
		{
			sprintf(fn,"\\\\.\\COM%d",port);
		}
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

/* static */ std::vector <std::string> YsCOMPort::FindAvailablePortName(void)
{
	std::vector <std::string> availablePortName;
	for(auto portNum : FindAvailablePort())
	{
		char portStr[256];
		sprintf(portStr,"%d",portNum);
		availablePortName.push_back(portStr);
	}
	return availablePortName;
}


static void TryBaudRate(HANDLE hComm,DCB &dcb,int baud)
{
	dcb.BaudRate=baud;
	if(0==SetCommState(hComm,&dcb))
	{
		printf("Failed to set %d bits-per-second.\n",baud);
	}
}

bool YsCOMPort::Open(const std::string &port)
{
	Close();

	char fn[256];
	if(atoi(port.c_str())<10)
	{
		sprintf(fn,"COM%s",port.c_str());
	}
	else
	{
		sprintf(fn,"\\\\.\\COM%s",port.c_str());
	}

	printf("Opening:\n");
	printf("%s\n",fn);

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

	this->portName=portName;
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
		timeOut.WriteTotalTimeoutMultiplier=100;
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
		actualBaudRate=dcb.BaudRate;
	}
	else
	{
		Close();
		return false;
	}

	EscapeCommFunction(this->portPtr->hComm,CLRDTR);
	EscapeCommFunction(this->portPtr->hComm,SETDTR);
	EscapeCommFunction(this->portPtr->hComm,CLRRTS);
	EscapeCommFunction(this->portPtr->hComm,SETRTS);

	// Arduino version needs 800ms sleep to be safe.
	std::this_thread::sleep_for(std::chrono::milliseconds(800));

	return true;
}

#ifdef _WIN32
bool YsCOMPort::Open(int portNumber)
{
	char portStr[256];
	sprintf(portStr,"%d",portNumber);
	return Open(portStr);
}
#endif

bool YsCOMPort::ChangeBaudRate(int baudRate)
{
	if(nullptr!=this->portPtr->hComm)
	{
		desiredBaudRate=baudRate;
		DCB dcb;
		if(TRUE==GetCommState(this->portPtr->hComm,&dcb))
		{
			dcb.BaudRate=baudRate;
			if(0!=SetCommState(this->portPtr->hComm,&dcb))
			{
				// Set and then verify.
				GetCommState(this->portPtr->hComm,&dcb);
				actualBaudRate=dcb.BaudRate;
				printf("Speed set to %d bits-per-second.\n",dcb.BaudRate);
				return true;
			}
		}
	}
	return false;
}

void YsCOMPort::Close(void)
{
	if(nullptr!=portPtr)
	{
		CloseHandle(portPtr->hComm);
		delete portPtr;
		portPtr=nullptr;
		portName="";
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
