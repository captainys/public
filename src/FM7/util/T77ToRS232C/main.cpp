#include <chrono>
#include <thread>
#include <vector>
#include <thread>
#include <mutex>
#include <string>


#include "comport.h"
#include "t77.h"
#include "cpplib.h"

#include "bioshook_small.h"
#include "bioshook_buffered.h"


// This value must match the value in bioshook_buffered.asm 
static const int READBUFFER_SIZE=8;


void MainCPU(void);
void SubCPU(void);

enum T77CLIENT_TYPE
{
	T77CLI_SMALL,
	T77CLI_BUFFERED
};


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
	printf("-rdnonfbasic\n");
	printf("\tRedirect BIOS call non-fbasic binary stream.\n");
	printf("-buffered\n");
	printf("\tBuffered mode.  Uses 8-byte read-buffer for faster transfer.\n");
	printf("-small\n");
	printf("\tSmall mode.  Smallest footprint.\n");
}

void ShowCommandHelp(void)
{
	printf("Command Help:\n");
	printf("IA..Transmit BIOS redirector installer to FM-7/77\n");
	printf("Q...Quit.\n");
	printf("H...Show this help.\n");
	printf("V...Verbose mode on/off\n");
	printf("R...Rewind all the way.\n");
	printf("D...Dump raw bytes in T77\n");
	printf("    Dxxxxxxxx for dump from specific offset.\n");
	printf("F...Show files in T77 file.\n");
	printf("SV..Save current save-tape to a T77 file.\n");
	printf("    SVfilename for specifying a save file name.\n");
	printf("    No space between SVfilename\n");
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

void SaveCommandInstruction(void)
{
	printf("Save file name not specified.\n");
	printf("Type: SVfilename.t77\n");
	printf("(No space between SV and filename)\n");
}

void ShowPrompt(T77CLIENT_TYPE cli)
{
	switch(cli)
	{
	case T77CLI_SMALL:
		printf("Command {? for Help}>");
		break;
	case T77CLI_BUFFERED:
		printf("Command(BufferedMode) {? for Help}>");
		break;
	}
}

////////////////////////////////////////////////////////////


unsigned int GetDefaultInstallAddress(T77CLIENT_TYPE cli)
{
	FM7BinaryFile binFile;
	switch(cli)
	{
	case T77CLI_SMALL:
		binFile.DecodeSREC(clientBinary_small);
		break;
	case T77CLI_BUFFERED:
		binFile.DecodeSREC(clientBinary_buffered);
		break;
	}
	return 0x100*binFile.dat[2]+binFile.dat[3];
}

unsigned int GetDefaultBridgeAddress(T77CLIENT_TYPE cli)
{
	FM7BinaryFile binFile;
	switch(cli)
	{
	case T77CLI_SMALL:
		binFile.DecodeSREC(clientBinary_small);
		break;
	case T77CLI_BUFFERED:
		binFile.DecodeSREC(clientBinary_buffered);
		break;
	}
	return 0x100*binFile.dat[4]+binFile.dat[5];
}


////////////////////////////////////////////////////////////


class T77ServerCommandParameterInfo
{
public:
	std::string portStr;
	std::string t77FName;
	std::string saveT77FName;
	T77CLIENT_TYPE cliType;

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
	saveT77FName="";

	cliType=T77CLI_SMALL;
	instAddr=GetDefaultInstallAddress(cliType);
	bridgeAddr=GetDefaultBridgeAddress(cliType);

	redirectBiosCallMachingo=true;
	redirectBiosCallBinaryString=false;
}

bool T77ServerCommandParameterInfo::Recognize(int ac,char *av[])
{
	bool instAddrSet=false,bridgeAddrSet=false;

	if(ac<=1)
	{
		ShowOptionHelp();
		return false;
	}

	int fixedOrderIndex=0;
	for(int i=1; i<ac; ++i)
	{
		std::string arg(av[i]);
		FM7Lib::Capitalize(arg);
		if("-H"==arg || "-HELP"==arg || "-?"==arg)
		{
			ShowOptionHelp();
		}
		else if("-INSTALL"==arg)
		{
			if(i+1<ac)
			{
				instAddr=FM7Lib::Xtoi(av[i+1]);
				instAddrSet=true;
				++i;
			}
			else
			{
				fprintf(stderr,"Insufficient parameter for -install.\n");
				return false;
			}
		}
		else if("-BRIDGE"==arg)
		{
			if(i+1<ac)
			{
				bridgeAddr=FM7Lib::Xtoi(av[i+1]);
				bridgeAddrSet=true;
				++i;
			}
			else
			{
				fprintf(stderr,"Insufficient parameter for -install.\n");
				return false;
			}
		}
		else if("-SAVE"==arg)
		{
			if(i+1<ac)
			{
				saveT77FName=av[i+1];
				++i;
			}
			else
			{
				fprintf(stderr,"Insufficient parameter for -save.\n");
				return false;
			}
		}
		else if("-RDNONFBASIC"==arg)
		{
			redirectBiosCallBinaryString=true;
		}
		else if("-BUFFERED"==arg)
		{
			cliType=T77CLI_BUFFERED;
		}
		else if("-SMALL"==arg)
		{
			cliType=T77CLI_SMALL;
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

	if(true!=instAddrSet)
	{
		instAddr=GetDefaultInstallAddress(cliType);
	}
	if(true!=bridgeAddrSet)
	{
		bridgeAddr=GetDefaultBridgeAddress(cliType);
	}
	printf("Default Install Address=%04x\n",instAddr);
	printf("Default Bridge Address =%04x\n",bridgeAddr);

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
	std::string t77FName;
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
		printf("0x%-8x:",(int)f.ptr);

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
	FM7CassetteTape *loadTapePtr;
	T77Encoder saveTape;
	std::chrono::time_point<std::chrono::system_clock> lastByteReceivedClock;
	bool tapeSaved;

	T77ServerCommandParameterInfo cpi;



	FC80()
	{
		terminate=false;
		subCPUready=false;

		verbose=false;
		installASCII=false;

		tapeSaved=true;
		lastByteReceivedClock=std::chrono::system_clock::now();

		loadTapePtr=&loadTape;
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
		loadTapePtr->currentPtr=0;
		loadTapePtr->nextPrintPtr=0;
	}
	void InstallASCII(void)
	{
		std::lock_guard <std::mutex> lock(fd05);
		installASCII=true;
	}
	void SaveT77(const char fName[]) const
	{
		std::lock_guard <std::mutex> lock(fd05);
		T77Encoder enc;

		FILE *fp=fopen(fName,"wb");
		if(nullptr==fp)
		{
			fprintf(stderr,"Error!  Cannot save to %s\n",fName);
		}
		else
		{
			FM7Lib::WriteBinaryFile(fp,saveTape.t77);
			fclose(fp);
			fprintf(stderr,"Saved %s\n",fName);
		}
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

	if("####EMPTY####"!=fc80.cpi.t77FName)
	{
		auto t77file=FM7Lib::ReadBinaryFile(fc80.cpi.t77FName.c_str());
		if(0==t77file.size())
		{
			fprintf(stderr,"Cannot open .T77 file.\n");
			return 1;
		}
		fc80.loadTapePtr->Make(
		    t77file,
		    fc80.cpi.redirectBiosCallMachingo,
		    fc80.cpi.redirectBiosCallBinaryString,
		    fc80.cpi.bridgeAddr);
		fc80.loadTapePtr->t77FName=fc80.cpi.t77FName;
	}

	if(""!=fc80.cpi.saveT77FName)
	{
		auto t77file=FM7Lib::ReadBinaryFile(fc80.cpi.saveT77FName.c_str());
		if(0==t77file.size())
		{
			FILE *fp=fopen(fc80.cpi.saveT77FName.c_str(),"ab");
			if(nullptr==fp)
			{
				fclose(fp);
				printf("Cannot open a T77 for save.\n");
				return 1;
			}
			printf("Creating a new T77 for saving.\n");
			fc80.saveTape.CleanUp();
			fc80.saveTape.StartT77Header();
		}
		else
		{
			fc80.saveTape.t77=t77file;
			fc80.saveTape.AddGapBetweenFile();
		}
	}
	else
	{
		fc80.saveTape.CleanUp();
		fc80.saveTape.StartT77Header();
	}


	Title();
	fc80.loadTapePtr->Files();

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

	unsigned long long int dumpPtr=0;
	for(;;)
	{
		if(true==fc80.GetFatalError())
		{
			return;
		}

		ShowPrompt(fc80.cpi.cliType);

		char cmdIn[256];
		Fgets(cmdIn,255,stdin);

		std::string CMD(cmdIn);
		FM7Lib::Capitalize(CMD);

		bool processed=false;
		switch(CMD[0])
		{
		case 'H':
		case '?':
			ShowCommandHelp();
			processed=true;
			break;
		case 'I':
			if("IA"==CMD)
			{
				fc80.InstallASCII();
				processed=true;
			}
			break;
		case 'Q':
			fc80.SetTerminate(true);
			processed=true;
			return;
		case 'R':
			fc80.RewindAllTheWay();
			printf("Rewind all the way.\n");
			processed=true;
			break;
		case 'V':
			fc80.SetVerboseMode(true==fc80.GetVerboseMode() ? false : true);
			printf("VerboseMode=%s\n",FM7Lib::BoolToStr(fc80.GetVerboseMode()));
			processed=true;
			break;
		case 'D':
			if(2<=CMD.size())
			{
				dumpPtr=FM7Lib::Xtoi(CMD.data()+1);
			}
			if(nullptr!=fc80.loadTapePtr)
			{
				char ascii[17];
				for(long long int i=0; i<256 && dumpPtr<fc80.loadTapePtr->byteString.size(); ++dumpPtr,++i)
				{
					if(0==i%16)
					{
						printf("%08llx|",dumpPtr);
					}
					printf("%02x ",fc80.loadTapePtr->byteString[dumpPtr]);
					if(' '<=fc80.loadTapePtr->byteString[dumpPtr] && fc80.loadTapePtr->byteString[dumpPtr]<128)
					{
						ascii[i%16]=fc80.loadTapePtr->byteString[dumpPtr];
					}
					else
					{
						ascii[i%16]=' ';
					}
					if(15==i%16 || dumpPtr==fc80.loadTapePtr->byteString.size()-1)
					{
						ascii[1+i%16]=0;
						for(auto j=i; 15!=j%16; ++j)
						{
							printf("   ");
						}
						printf("|%s\n",ascii);
					}
				}
				printf("\n");
			}
			if(nullptr==fc80.loadTapePtr || fc80.loadTapePtr->byteString.size()<=dumpPtr)
			{
				printf("[EOF]\n");
			}
			processed=true;
			break;
		case 'F':
			if(nullptr!=fc80.loadTapePtr)
			{
				printf("T77>> %s\n",fc80.loadTapePtr->t77FName.data());
				fc80.loadTapePtr->Files();
			}
			if(0<fc80.cpi.saveT77FName.size())
			{
				printf("Incoming data will be auto-saved to: %s\n",fc80.cpi.saveT77FName.c_str());
			}
			else
			{
				printf("Incoming data will not be auto-saved.\n");
			}
			processed=true;
			break;
		case 'S':
			if('V'==CMD[1])
			{
				if(0==CMD[2])
				{
					if(0==fc80.cpi.saveT77FName.size())
					{
						SaveCommandInstruction();
					}
					else
					{
						fc80.SaveT77(fc80.cpi.saveT77FName.c_str());
					}
				}
				else
				{
					fc80.SaveT77(cmdIn+2);
				}
				processed=true;
			}
			break;
		}
		if(true!=processed)
		{
			printf("Syntax Error\n");
			printf("  %s\n",cmdIn);
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(5));
	}
}

static void SendOneByte(YsCOMPort &comPort);
static void SaveOneByte(unsigned char c);

void SubCPU(void)
{
	YsCOMPort comPort;
	bool activity=false;
	auto activityTimer=std::chrono::system_clock::now();
	auto lastSentTimer=std::chrono::system_clock::now();

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

	enum STATE
	{
		STATE_NORMAL,
		STATE_WAIT_WRITE_BYTE,
	};
	STATE state=STATE_NORMAL;

	// Why 0xB6 and 0xB7?
	// I have observed some occasions in which MOTOR-ON BIOS-call was not
	// issued between subsequent LOADMs.  Good part is that the first LOADM
	// seems to always issue the MOTOR-ON BIOS-call.
	//
	// Therefore, RS232C is made ready in the first BIOS-call, but in the
	// middle, the MOTOR is turned off, which is interpreted as RS232C
	// TxRDY, RxRDY clear.
	//
	// So, at the beginning of CTBRED, CTBWRT override, I need to make sure
	// RS232C (8251A) is in TxRDY, RxRDY state, which requires to write
	// #$B7 to $FD07.  So, I have one LDA #$B7.
	//
	// Then, why not use this value as a command.  I can directly write it, or 
	// DECA and write it to RS232C.  I save three bytes compared to using a 
	// different REQUEST code.
	const unsigned int READ_REQUEST=0xB6;
	const unsigned int WRITE_REQUEST=0xB7;
	const unsigned int READ_REQUESTM=0xB8;


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
			switch(fc80.cpi.cliType)
			{
			case T77CLI_SMALL:
				binFile.DecodeSREC(clientBinary_small);
				break;
			case T77CLI_BUFFERED:
				binFile.DecodeSREC(clientBinary_buffered);
				break;
			}

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
				std::this_thread::sleep_for(std::chrono::milliseconds(15));
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
				std::this_thread::sleep_for(std::chrono::milliseconds(15));
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
			activity=true;
			activityTimer=std::chrono::system_clock::now()+std::chrono::milliseconds(100);

			switch(state)
			{
			case STATE_NORMAL:
				if(READ_REQUEST==c)
				{
					SendOneByte(comPort);
				}
				else if(READ_REQUESTM==c)
				{
					for(int i=0; i<READBUFFER_SIZE; ++i)
					{
						SendOneByte(comPort);
					}
				}
				else if(WRITE_REQUEST==c)
				{
					state=STATE_WAIT_WRITE_BYTE;
				}
				break;
			case STATE_WAIT_WRITE_BYTE:
				{
					SaveOneByte(c);
					state=STATE_NORMAL;
				}
				break;
			}
		}
		fc80.Unhalt();

		if(true!=fc80.tapeSaved && 
		   std::chrono::milliseconds(50)<std::chrono::system_clock::now()-fc80.lastByteReceivedClock)
		{
			printf("Received data.\n");
			if(""!=fc80.cpi.saveT77FName)
			{
				fc80.SaveT77(fc80.cpi.saveT77FName.c_str());
				fc80.saveTape.AddGapBetweenFile();
			}
			else
			{
				SaveCommandInstruction();
			}
			fc80.tapeSaved=true;
			ShowPrompt(fc80.cpi.cliType);
		}


		if(activityTimer<std::chrono::system_clock::now())
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
			if(true==activity)
			{
				activity=false;
				ShowPrompt(fc80.cpi.cliType);
			}
		}
	}

	comPort.Close();
}

void SendOneByte(YsCOMPort &comPort)
{
	unsigned char toSend=0xff;
	if(fc80.loadTapePtr->currentPtr<fc80.loadTapePtr->byteString.size())
	{
		toSend=fc80.loadTapePtr->byteString[fc80.loadTapePtr->currentPtr++];
	}

	long long int nSend=1;
	unsigned char sendByte[1]={toSend};
	comPort.Send(nSend,sendByte);

	if(true==fc80.verbose)
	{
		printf("R%02x\n",toSend);
	}

	if(fc80.loadTapePtr->nextPrintPtr<fc80.loadTapePtr->currentPtr)
	{
		printf("\r%d/%d<<<<\n",(int)fc80.loadTapePtr->currentPtr,(int)fc80.loadTapePtr->byteString.size());
		fc80.loadTapePtr->nextPrintPtr+=200;
	}
	if(fc80.loadTapePtr->byteString.size()<=fc80.loadTapePtr->currentPtr)
	{
		printf("\rEOF<<<<\n");
	}
}

void SaveOneByte(unsigned char c)
{
	if(true==fc80.verbose)
	{
		printf("W%02x\n",c);
	}
	fc80.saveTape.AddByte(c);

	if(std::chrono::milliseconds(500)<std::chrono::system_clock::now()-fc80.lastByteReceivedClock)
	{
		printf("\rReceiving Data...\n");
	}
	fc80.lastByteReceivedClock=std::chrono::system_clock::now();
	fc80.tapeSaved=false;
}
