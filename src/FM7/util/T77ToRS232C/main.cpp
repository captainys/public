#include <chrono>
#include <thread>
#include <vector>
#include <thread>
#include <mutex>
#include <string>


#include "comport.h"
#include "t77.h"
#include "cpplib.h"



void MainCPU(void);
void SubCPU(void);



////////////////////////////////////////////////////////////



void ShowOptionHelp(void)
{
	printf("Usage:\n");
	printf("  T77Server d77file.d77 comPort\n");
	printf("\n");
	printf("-h, -help, -?\n");
	printf("\tShow this help.\n");
}

void ShowCommandHelp(void)
{
	printf("Command Help:\n");
	printf("Q...Quit.\n");
	printf("H...Show this help.\n");
	printf("V...Verbose mode on/off\n");
	printf("R...Rewind all the way.\n");
}

void Title(void)
{
	printf("********************************\n");
	printf("*                              *\n");
	printf("*  T77 Image Server            *\n");
	printf("*  by CaptainYS                *\n");
	printf("*  http://www.ysflight.com     *\n");
	printf("*                              *\n");
	printf("********************************\n");
	printf("Make sure to configure FM-7/77 side computer at 19200bps.\n");
}


class T77ServerCommandParameterInfo
{
public:
	std::string portStr;
	std::string t77FName;

	T77ServerCommandParameterInfo();
	bool Recognize(int ac,char *av[]);
	void CleanUp(void);
};

T77ServerCommandParameterInfo::T77ServerCommandParameterInfo()
{
	CleanUp();
}

void T77ServerCommandParameterInfo::CleanUp(void)
{
	portStr="";
	t77FName="";
}

bool T77ServerCommandParameterInfo::Recognize(int ac,char *av[])
{
	int fixedOrderIndex=0;
	for(int i=1; i<ac; ++i)
	{
		std::string arg(av[i]);
		if("-h"==arg || "-help"==arg || "-?"==arg)
		{
			ShowOptionHelp();
		}
		else if('-'==arg[0])
		{
			printf("Unknown option: %s\n",arg.c_str());
			return false;
		}
		else
		{
			switch(fixedOrderIndex)
			{
			case 0:
				t77FName=arg;
				break;
			case 1:
				portStr=arg;
				break;
			}
			++fixedOrderIndex;
		}
	}

	if(fixedOrderIndex<2)
	{
		return false;
	}

	return true;
}



////////////////////////////////////////////////////////////


static std::mutex fd05;
static T77Decoder t77;
static T77ServerCommandParameterInfo cpi;


class FC80
{
public:
	bool terminate;
	bool verbose;
	long long int nextPtrPrint;
	T77Decoder::RawDecodingInfo decodeInfo;

	FC80()
	{
		terminate=false;
		nextPtrPrint=0;
		verbose=false;
	}
	void SetTerminate(bool t)
	{
		std::lock_guard <std::mutex> lock(fd05);
		terminate=t;
	}
	bool GetTerminate(void)
	{
		std::lock_guard <std::mutex> lock(fd05);
		return terminate;
	}
	void SetVerboseMode(bool v)
	{
		std::lock_guard <std::mutex> lock(fd05);
		verbose=v;
	}
	bool GetVerboseMode(void)
	{
		std::lock_guard <std::mutex> lock(fd05);
		return verbose;
	}
	void RewindAllTheWay(void)
	{
		std::lock_guard <std::mutex> lock(fd05);
		decodeInfo=t77.BeginRawDecoding();
		nextPtrPrint=0;
	}

	void Halt(void)
	{
		fd05.lock();
	}
	void Unhalt(void)
	{
		fd05.unlock();
	}
};

static FC80 fc80;



char *Fgets(char str[],int len,FILE *fp)
{
	auto ret=fgets(str,len,fp);
	if(nullptr!=ret)
	{
		for(int i=0; str[i]!=0; ++i)
		{
			if(str[i]<' ')
			{
				str[i]=0;
				break;
			}
		}
	}
	return ret;
}

int main(int ac,char *av[])
{
	if(true!=cpi.Recognize(ac,av))
	{
		return 1;
	}


	auto t77file=FM7Lib::ReadBinaryFile(cpi.t77FName.c_str());
	std::swap(t77.t77,t77file);

// T77 Dump.  I'll make it a command output.
//	{
//		auto info=t77.BeginRawDecoding();
//		while(true!=info.endOfFile)
//		{
//			info=t77.RawReadByte(info);
//			if(true!=info.endOfFile)
//			{
//				printf("%02x",info.byteData);
//				if(0==info.byteCtr%16)
//				{
//					printf("\n");
//				}
//			}
//		}
//		printf("\n");
//	}

	std::thread t(SubCPU);
	MainCPU();
	t.join();

	return 0;
}

void MainCPU(void)
{
	for(;;)
	{
		printf("Command>");

		char cmdIn[256];
		Fgets(cmdIn,255,stdin);

		auto cmdLetter=cmdIn[0];
		if('a'<=cmdLetter && cmdLetter<='z')
		{
			cmdLetter=cmdLetter+'A'-'a';
		}
		if('H'==cmdLetter)
		{
			ShowCommandHelp();
		}
		else if('Q'==cmdLetter)
		{
			fc80.SetTerminate(true);
			break;
		}
		else if('R'==cmdLetter)
		{
			fc80.RewindAllTheWay();
			printf("Rewind all the way.\n");
		}
		else if('V'==cmdLetter)
		{
			auto v=(true==fc80.GetVerboseMode() ? false : true);
			fc80.SetVerboseMode(v);
			printf("VerboseMode=%s\n",FM7Lib::BoolToStr(v));
		}
	}
}

void SubCPU(void)
{
	YsCOMPort comPort;

	comPort.SetDesiredBaudRate(19200);
	comPort.SetDesiredBitLength(YsCOMPort::BITLENGTH_8);
	comPort.SetDesiredStopBit(YsCOMPort::STOPBIT_1);
	comPort.SetDesiredParity(YsCOMPort::PARITY_NOPARITY);
	comPort.SetDesiredFlowControl(YsCOMPort::FLOWCONTROL_NONE);
	comPort.Open(FM7Lib::Atoi(cpi.portStr.c_str()));

	fc80.Halt();
	fc80.decodeInfo=t77.BeginRawDecoding();
	fc80.Unhalt();
	for(;;)
	{
		if(true==fc80.GetTerminate())
		{
			break;
		}

		auto recv=comPort.Receive();
		for(auto c : recv)
		{
			//printf("%02x",c);
			//if(0x20<=c && c<=0x7f)
			//{
			//	printf("(%c)",c);
			//}
			//else
			//{
			//	printf("   ");
			//}

			if('R'==c)
			{
				fc80.Halt();

				if(true!=fc80.decodeInfo.endOfFile)
				{
					fc80.decodeInfo=t77.RawReadByte(fc80.decodeInfo);
				}
				long long int nSend=1;
				unsigned char sendByte[2];
				// FM-7 Emulator XM7 cannot receive a binary number zero.
				// The source code w32_comm.c is looking for EV_RXCHAR event.
				// Probably this event ignores zero.
				// I use #1 as escape.  The actual number is the subsequent number -1.
				// unsigned 8-bit integers, both of which are non-zero.
				// The client loses six bytes to deal with it, but otherwise I cannot test.

				// Also w32_comm.c is running a RS232C reader thread, which is not correctly
				// locking and unlocking resources.
				// Therefore if something is received immediately after sending something,
				// it breaks.
				// At this time, I need 5ms delay after sending and receiving.

				if(0==fc80.decodeInfo.byteData || 1==fc80.decodeInfo.byteData)
				{
					nSend=2;
					sendByte[0]=1;
					sendByte[1]=fc80.decodeInfo.byteData+1;
				}
				else
				{
					nSend=1;
					sendByte[0]=fc80.decodeInfo.byteData;
				}


				// In XM7, I need to make sure ThreadRcv is idle, then
				std::this_thread::sleep_for(std::chrono::milliseconds(5));
				// send data, and then
				comPort.Send(nSend,sendByte);
				// make sure rs232c_senddata is over.
				std::this_thread::sleep_for(std::chrono::milliseconds(5));

				if(fc80.nextPtrPrint<fc80.decodeInfo.ptr)
				{
					printf("\r%d/%d<<<<\n",(int)fc80.decodeInfo.ptr,(int)t77.t77.size());
					fc80.nextPtrPrint+=5000;
				}

				fc80.Unhalt();
			}
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}

	comPort.Close();
}
