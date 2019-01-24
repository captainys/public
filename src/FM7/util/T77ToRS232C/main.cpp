#include <chrono>
#include <thread>
#include <vector>
#include <thread>
#include <mutex>
#include <string>


#include "comport.h"
#include "t77.h"
#include "cpplib.h"

#include "bioshook.h"


void MainCPU(void);
void SubCPU(void);


// 6809 code to look for
//  AD9FFBFA                                                JSR             [$FBFA]
//  6E9FFBFA                                                JMP             [$FBFA]
//  BDF17D                                                  JSR             $F17D
//  7EF17D                                                  JMP             $F17D

////////////////////////////////////////////////////////////



void ShowOptionHelp(void)
{
	printf("Usage:\n");
	printf("  T77Server d77file.d77 comPort\n");
	printf("\n");
	printf("-h, -help, -?\n");
	printf("\tShow this help.\n");
	printf("-xm7\n");
	printf("\tConnecting to XM7, not actual FM-7/77.\n");
	printf("\tDue to RS232C-handling bug, XM7 requires 5ms delay after receiving\n");
	printf("\tand after transmitting.  Real FM-7/77 does not require such delay.\n");
	printf("-install ADDR\n");
	printf("\tSpecify install address of BIOS hook in FM-7.\n");
	printf("\tDefault location is at the back end of the URA RAM (shadow RAM)\n");
	printf("\tADDR must be hexadecimal WITHOUT $ or &H or 0x in front of it.\n");
	printf("-bridge ADDR\n");
	printf("\tSpecify bridge address in FM-7.\n");
	printf("\tIf the same address is given as the bridge address, bridge will not \n");
	printf("\tbe used, but the BIOS hook needs to be in the main RAM, not in URA RAM\n");
	printf("\tDefault location is FC00.\n");
	printf("\tADDR must be hexadecimal WITHOUT $ or &H or 0x in front of it.\n");
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


////////////////////////////////////////////////////////////


unsigned int GetDefaultInstallAddress(void)
{
	FM7BinaryFile binFile;
	binFile.DecodeSREC(clientBinary);
	return 0x100*binFile.dat[2]+binFile.dat[3];
}

unsigned int GetDefaultBridgeAddress(void)
{
	FM7BinaryFile binFile;
	binFile.DecodeSREC(clientBinary);
	return 0x100*binFile.dat[4]+binFile.dat[5];
}


////////////////////////////////////////////////////////////


class T77ServerCommandParameterInfo
{
public:
	std::string portStr;
	std::string t77FName;
	bool xm7;

	unsigned int instAddr,bridgeAddr;
	bool redirectBiosCallMachingo;
	bool redirectBiosCallBinaryString;

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
	xm7=false;
	instAddr=GetDefaultInstallAddress();
	bridgeAddr=GetDefaultBridgeAddress();
	printf("Default Install Address=%04x\n",instAddr);
	printf("Default Bridge Address =%04x\n",bridgeAddr);

	redirectBiosCallMachingo=true;
	redirectBiosCallBinaryString=false;
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
		else if("-xm7"==arg || "-XM7"==arg || "-Xm7"==arg)
		{
			xm7=true;
		}
		else if("-install"==arg || "-INSTALL"==arg || "-Install"==arg)
		{
			if(i+1<ac)
			{
				instAddr=FM7Lib::Xtoi(av[i+1]);
				++i;
			}
			else
			{
				fprintf(stderr,"Insufficient parameter for -install.\n");
				return false;
			}
		}
		else if("-bridge"==arg || "-BRIDGE"==arg || "-Bridge"==arg)
		{
			if(i+1<ac)
			{
				bridgeAddr=FM7Lib::Xtoi(av[i+1]);
				++i;
			}
			else
			{
				fprintf(stderr,"Insufficient parameter for -install.\n");
				return false;
			}
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


class FM7CassetteTape
{
public:
	class FileInfo
	{
	public:
		int fType;
		std::string fName;
		long long int ptr;
		unsigned int len,storeAddr,execAddr;

		FileInfo()
		{
			CleanUp();
		}
		void CleanUp(void)
		{
			fType=FM7File::FTYPE_UNKNOWN;
			fName="";
			ptr=0;
			len=0;
			storeAddr=0;
			execAddr=0;
		}
	};
	std::vector <unsigned char> rawT77file;
	std::vector <FileInfo> dir;
	std::vector <unsigned char> byteString;
	long long int currentPtr,nextPrintPtr;

	/*! Returns how many occurrences have been replaced.
	*/
	static int RedirectBiosCall(std::vector<unsigned char> &bin,unsigned int bridgeAddr);

	void CleanUp(void);
	void Make(
	    const std::vector <unsigned char> &t77File,
	    bool redirectBiosCallMachingo,
	    bool redirectBiosCallByteString,
	    unsigned int bridgeAddr);
	void Remake(
	    bool redirectBiosCallMachingo,
	    bool redirectBiosCallByteString,
	    unsigned int bridgeAddr);
	void Files(void) const;
};

/* static */ int FM7CassetteTape::RedirectBiosCall(std::vector<unsigned char> &bin,unsigned int bridgeAddr)
{
	int nRepl=0;

	for(long long int ptr=0; ptr+3<=bin.size(); ++ptr)
	{
		// 6809 code to look for
		//  AD9FFBFA    JSR             [$FBFA]
		//  6E9FFBFA    JMP             [$FBFA]
		//  BDF17D      JSR             $F17D
		//  7EF17D      JMP             $F17D

		if(ptr+4<=bin.size() &&
		   bin[ptr  ]==0xAD &&
		   bin[ptr+1]==0x9F &&
		   bin[ptr+2]==0xFB &&
		   bin[ptr+3]==0xFA)
		{
			printf("Found JSR [$FBFA] at offset $%04x\n",ptr);
			bin[ptr  ]=0xBD;				// JSR
			bin[ptr+1]=((bridgeAddr>>8)&0xff);
			bin[ptr+2]=(bridgeAddr&0xff);
			bin[ptr+3]=0x12;				// NOP
			++nRepl;
		}
		else if(ptr+4<=bin.size() &&
		   bin[ptr  ]==0x6E &&
		   bin[ptr+1]==0x9F &&
		   bin[ptr+2]==0xFB &&
		   bin[ptr+3]==0xFA)
		{
			printf("Found JMP [$FBFA] at offset $%04x\n",ptr);
			bin[ptr  ]=0x7E;				// JMP
			bin[ptr+1]=((bridgeAddr>>8)&0xff);
			bin[ptr+2]=(bridgeAddr&0xff);
			bin[ptr+3]=0x12;				// NOP
			++nRepl;
		}
		else if(
		   bin[ptr  ]==0xBD &&
		   bin[ptr+1]==0xF1 &&
		   bin[ptr+2]==0x7D)
		{
			printf("Found JSR $F17D at offset $%04x\n",ptr);
			bin[ptr+1]=((bridgeAddr>>8)&0xff);
			bin[ptr+2]=(bridgeAddr&0xff);
			++nRepl;
		}
		else if(
		   bin[ptr  ]==0x7E &&
		   bin[ptr+1]==0xF1 &&
		   bin[ptr+2]==0x7D)
		{
			printf("Found JMP $F17D at offset $%04x\n",ptr);
			bin[ptr+1]=((bridgeAddr>>8)&0xff);
			bin[ptr+2]=(bridgeAddr&0xff);
			++nRepl;
		}
	}

	return nRepl;
}

void FM7CassetteTape::CleanUp(void)
{
	rawT77file.clear();
	dir.clear();
	byteString.clear();

	currentPtr=0;
	nextPrintPtr=0;
}

void FM7CassetteTape::Make(
	    const std::vector <unsigned char> &t77File,
	    bool redirectBiosCallMachingo,
	    bool redirectBiosCallBinaryString,
	    unsigned int bridgeAddr)
{
	CleanUp();
	rawT77file=t77File;
	Remake(redirectBiosCallMachingo,redirectBiosCallBinaryString,bridgeAddr);
}

void FM7CassetteTape::Remake(
	    bool redirectBiosCallMachingo,
	    bool redirectBiosCallBinaryString,
	    unsigned int bridgeAddr)
{
	T77Decoder t77Dec;
	t77Dec.t77=rawT77file;
	t77Dec.Decode();

	auto ptrStop=t77Dec.filePtr;
	ptrStop.push_back(t77Dec.t77.size());

	for(long long int fileIdx=0; fileIdx<t77Dec.fileDump.size(); ++fileIdx)
	{
		auto fmDat=t77Dec.DumpToFMFormat(t77Dec.fileDump[fileIdx]);
		bool processed=false;

		FM7CassetteTape::FileInfo file;
		file.ptr=byteString.size();

		if(16<=fmDat.size())
		{
			auto fName=t77Dec.GetDumpFileName(t77Dec.fileDump[fileIdx]);
			file.fType=FM7File::DecodeFMHeaderFileType(fmDat[10],fmDat[11]);
			if(file.fType!=FM7File::FTYPE_UNKNOWN)
			{
				file.fName=fName;
			}

			if(file.fType==FM7File::FTYPE_BINARY)
			{
				FM7BinaryFile binFile;
				if(true==binFile.Decode2B0(t77Dec.DumpToFMFormat(t77Dec.fileDump[fileIdx])))
				{
					file.len=binFile.dat.size();
					file.storeAddr=binFile.storeAddr;
				    file.execAddr=binFile.execAddr;
					if(true==redirectBiosCallMachingo &&
					   0<FM7CassetteTape::RedirectBiosCall(binFile.dat,bridgeAddr))
					{
						T77Encoder enc;
						enc.StartT77Header();
						enc.Encode(file.fType,fName.c_str(),binFile);

						T77Decoder dec;
						std::swap(dec.t77,enc.t77);

						auto info=dec.BeginRawDecoding();
						while(true!=info.endOfFile)
						{
							info=dec.RawReadByte(info);
							byteString.push_back(info.byteData);
						}

						processed=true;
					}
				}
			}
			else if(file.fType==FM7File::FTYPE_UNKNOWN &&
			    true==redirectBiosCallBinaryString)
			{
				auto info=t77Dec.BeginRawDecoding();
				info.ptr=ptrStop[fileIdx];
				std::vector <unsigned char> bin;
				while(info.ptr<ptrStop[fileIdx+1] && true!=info.endOfFile)
				{
					info=t77Dec.RawReadByte(info);
					bin.push_back(info.byteData);
				}
				FM7CassetteTape::RedirectBiosCall(bin,bridgeAddr);
				byteString.insert(byteString.end(),bin.begin(),bin.end());
				processed=true;
			}
		}

		if(true!=processed)
		{
			auto info=t77Dec.BeginRawDecoding();
			info.ptr=ptrStop[fileIdx];
			while(info.ptr<ptrStop[fileIdx+1] && true!=info.endOfFile)
			{
				info=t77Dec.RawReadByte(info);
				byteString.push_back(info.byteData);
			}
		}

		dir.push_back(file);
	}
}

void FM7CassetteTape::Files(void) const
{
	printf("%d files\n",(int)dir.size());

	int i=0;
	for(auto f : dir)
	{
		printf("%10d:",(int)f.ptr);

		if(f.fType!=FM7File::FTYPE_UNKNOWN)
		{
			printf("%-8s  ",f.fName.c_str());
		}
		else
		{
			printf("--------  ");
		}

		printf("%-12s  ",FM7File::FileTypeToString(f.fType));
		if(f.fType==FM7File::FTYPE_BINARY)
		{
			printf("LEN:0x%04x FRM:0x%04x EXE:0x%04x",
			    (int)f.len,
			    (int)f.storeAddr,
			    (int)f.execAddr);
		}
		printf("\n");
	}
}

////////////////////////////////////////////////////////////


static std::mutex fd05;


class FC80
{
public:
	bool terminate;
	bool subCPUready;
	bool fatalError;
	bool verbose;
	bool installASCII;

	FM7CassetteTape loadTape;
	FM7CassetteTape *tapePtr;

	T77ServerCommandParameterInfo cpi;



	FC80()
	{
		terminate=false;
		subCPUready=false;

		verbose=false;
		installASCII=false;

		tapePtr=&loadTape;
	}

	void SetInstallAddress(unsigned int instAddr)
	{
		std::lock_guard <std::mutex> lock(fd05);
		cpi.instAddr=instAddr;
	}
	void SetBridgeAddress(unsigned int bridgeAddr)
	{
		std::lock_guard <std::mutex> lock(fd05);
		cpi.bridgeAddr=bridgeAddr;
	}
	bool GetSubCPUReady(void) const
	{
		std::lock_guard <std::mutex> lock(fd05);
		return subCPUready;
	}
	void SetFatalError(void)
	{
		std::lock_guard <std::mutex> lock(fd05);
		fatalError=true;
	}
	bool GetFatalError(void) const
	{
		std::lock_guard <std::mutex> lock(fd05);
		return fatalError;
	}
	void SetTerminate(bool t)
	{
		std::lock_guard <std::mutex> lock(fd05);
		terminate=t;
	}
	bool GetTerminate(void) const
	{
		std::lock_guard <std::mutex> lock(fd05);
		return terminate;
	}
	void SetVerboseMode(bool v)
	{
		std::lock_guard <std::mutex> lock(fd05);
		verbose=v;
	}
	bool GetVerboseMode(void) const
	{
		std::lock_guard <std::mutex> lock(fd05);
		return verbose;
	}
	void RewindAllTheWay(void)
	{
		std::lock_guard <std::mutex> lock(fd05);
		tapePtr->currentPtr=0;
		tapePtr->nextPrintPtr=0;
	}
	void InstallASCII(void)
	{
		std::lock_guard <std::mutex> lock(fd05);
		installASCII=true;
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


////////////////////////////////////////////////////////////


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
	if(true!=fc80.cpi.Recognize(ac,av))
	{
		return 1;
	}

	{
		auto t77file=FM7Lib::ReadBinaryFile(fc80.cpi.t77FName.c_str());
		if(0==t77file.size())
		{
			fprintf(stderr,"Cannot open .T77 file.\n");
			return 1;
		}
		fc80.tapePtr->Make(
		    t77file,
		    fc80.cpi.redirectBiosCallMachingo,
		    fc80.cpi.redirectBiosCallBinaryString,
		    fc80.cpi.bridgeAddr);
	}

	Title();
	fc80.tapePtr->Files();

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

void GetInstallAddressFromIACommand(bool &useInstAddr,unsigned int &instAddr,bool &useBridgeAddr,unsigned int &bridgeAddr,std::string cmdStr)
{
	if(6==cmdStr.size())
	{
		useInstAddr=true;
		useBridgeAddr=false;

		char hex[8];
		hex[0]=cmdStr[2];
		hex[1]=cmdStr[3];
		hex[2]=cmdStr[4];
		hex[3]=cmdStr[5];
		hex[4]=0;
		instAddr=FM7Lib::Xtoi(hex);
		bridgeAddr=0;
	}
	else if(10==cmdStr.size())
	{
		useInstAddr=true;
		useBridgeAddr=true;

		char hex[8];
		hex[0]=cmdStr[2];
		hex[1]=cmdStr[3];
		hex[2]=cmdStr[4];
		hex[3]=cmdStr[5];
		hex[4]=0;
		instAddr=FM7Lib::Xtoi(hex);

		hex[0]=cmdStr[6];
		hex[1]=cmdStr[7];
		hex[2]=cmdStr[8];
		hex[3]=cmdStr[9];
		bridgeAddr=FM7Lib::Xtoi(hex);;
	}
}

void MainCPU(void)
{
	while(true!=fc80.GetSubCPUReady())
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}

	for(;;)
	{
		if(true==fc80.GetFatalError())
		{
			return;
		}

		printf("Command>");

		char cmdIn[256];
		Fgets(cmdIn,255,stdin);

		std::string CMD(cmdIn);
		FM7Lib::Capitalize(CMD);

		if('H'==CMD[0])
		{
			ShowCommandHelp();
		}
		else if('I'==CMD[0])
		{
			bool useInstAddr,useBridgeAddr;
			unsigned int instAddr,bridgeAddr;
			if('A'==CMD[1])
			{
				GetInstallAddressFromIACommand(useInstAddr,instAddr,useBridgeAddr,bridgeAddr,CMD);
				if(true==useInstAddr)
				{
					fc80.SetInstallAddress(useInstAddr);
				}
				if(true==useBridgeAddr)
				{
					fc80.SetBridgeAddress(bridgeAddr);
				}
				fc80.InstallASCII();
			}
			else
			{
				fprintf(stderr,"Unknown installer option %c\n",CMD[1]);
			}
		}
		else if('Q'==CMD[0])
		{
			fc80.SetTerminate(true);
			break;
		}
		else if('R'==CMD[0])
		{
			fc80.RewindAllTheWay();
			printf("Rewind all the way.\n");
		}
		else if('V'==CMD[0])
		{
			auto v=(true==fc80.GetVerboseMode() ? false : true);
			fc80.SetVerboseMode(v);
			printf("VerboseMode=%s\n",FM7Lib::BoolToStr(v));
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(5));
	}
}

void SubCPU(void)
{
	YsCOMPort comPort;
	auto activityTimer=std::chrono::system_clock::now();

	comPort.SetDesiredBaudRate(19200);
	comPort.SetDesiredBitLength(YsCOMPort::BITLENGTH_8);
	comPort.SetDesiredStopBit(YsCOMPort::STOPBIT_1);
	comPort.SetDesiredParity(YsCOMPort::PARITY_NOPARITY);
	comPort.SetDesiredFlowControl(YsCOMPort::FLOWCONTROL_NONE);
	if(true!=comPort.Open(FM7Lib::Atoi(fc80.cpi.portStr.c_str())))
	{
		fprintf(stderr,"Cannot open port.\n");
		fc80.SetFatalError();
		return;
	}

	fc80.Halt();
	fc80.subCPUready=true;
	fc80.Unhalt();
	for(;;)
	{
		if(true==fc80.GetTerminate())
		{
			break;
		}

		fc80.Halt();

		if(true==fc80.installASCII)
		{
			FM7BinaryFile binFile;
			binFile.DecodeSREC(clientBinary);

			binFile.dat[2]=((fc80.cpi.instAddr>>8)&255);
			binFile.dat[3]=(fc80.cpi.instAddr&255);
			binFile.dat[4]=((fc80.cpi.bridgeAddr>>8)&255);
			binFile.dat[5]=(fc80.cpi.bridgeAddr&255);

			printf("Install Addr=%04x\n",fc80.cpi.instAddr);
			printf("Bridge Addr =%04x\n",fc80.cpi.bridgeAddr);

			printf("\nTransmitting Installer ASCII\n");
			int ctr=0;
			for(auto c : binFile.dat)
			{
				char str[256];
				sprintf(str,"%d%c%c",(unsigned int)c,0x0d,0x0a);
				comPort.Send(strlen(str),(unsigned char *)str);
				std::this_thread::sleep_for(std::chrono::milliseconds(30));
				ctr++;
				if(ctr%16==0)
				{
					printf("%d/256\n",ctr);
				}
			}
			for(auto i=binFile.dat.size(); i<256; ++i)
			{
				unsigned char zero[]={'0',0x0d,0x0a};
				comPort.Send(3,zero);
				std::this_thread::sleep_for(std::chrono::milliseconds(30));
				ctr++;
				if(ctr%16==0)
				{
					printf("%d/256\n",ctr);
				}
			}
			printf("Transmition finished.\n");
			printf("Do EXEC &H6000 on FM-7/77\n");
			fc80.installASCII=false;
		}


		auto recv=comPort.Receive();
		for(auto c : recv)
		{
			// If something incoming, don't sleep next 10ms.
			activityTimer=std::chrono::system_clock::now()+std::chrono::milliseconds(10);

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
				unsigned char toSend=0xff;
				if(fc80.tapePtr->currentPtr<fc80.tapePtr->byteString.size())
				{
					toSend=fc80.tapePtr->byteString[fc80.tapePtr->currentPtr++];
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

				if(0==toSend || 1==toSend)
				{
					nSend=2;
					sendByte[0]=1;
					sendByte[1]=toSend+1;
				}
				else
				{
					nSend=1;
					sendByte[0]=toSend;
				}


				if(true==fc80.cpi.xm7)
				{
					// In XM7, I need to make sure ThreadRcv is idle, then
					std::this_thread::sleep_for(std::chrono::milliseconds(5));
				}
				// send data, and then
				comPort.Send(nSend,sendByte);
				if(true==fc80.cpi.xm7)
				{
					// make sure rs232c_senddata is over.
					std::this_thread::sleep_for(std::chrono::milliseconds(5));
				}

				if(fc80.tapePtr->nextPrintPtr<fc80.tapePtr->currentPtr)
				{
					printf("\r%d/%d<<<<\n",(int)fc80.tapePtr->currentPtr,(int)fc80.tapePtr->byteString.size());
					fc80.tapePtr->nextPrintPtr+=200;
				}
				if(fc80.tapePtr->byteString.size()<=fc80.tapePtr->currentPtr)
				{
					printf("\rEOF<<<<\n");
				}
			}
		}
		fc80.Unhalt();
		if(activityTimer<std::chrono::system_clock::now())
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
		}
	}

	comPort.Close();
}
