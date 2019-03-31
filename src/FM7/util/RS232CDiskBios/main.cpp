#include <chrono>
#include <thread>
#include <vector>
#include <thread>
#include <mutex>
#include <string>


#include <string.h>

#include "disk_bios_hook_client.h"



#include "comport.h"
#include "d77.h"
#include "cpplib.h"
#include "fm7lib.h"



void MainCPU(void);
void SubCPU(void);


enum
{
	SYSTYPE_UNKNOWN,
	SYSTYPE_F_BASIC_3_0,
	SYSTYPE_URADOS
};

const unsigned char BIOS_ERROR_DRIVE_NOT_READY=		0x0A;
const unsigned char BIOS_ERROR_WRITE_PROTECTED=		0x0B;
const unsigned char BIOS_ERROR_HARD_ERROR=			0x0C;
const unsigned char BIOS_ERROR_CRC_ERROR=			0x0D;
const unsigned char BIOS_ERROR_DDM_ERROR=			0x0E;
const unsigned char BIOS_ERROR_TIME_OVER=			0x0F;

////////////////////////////////////////////////////////////

void IdentifyIPL(
	std::vector <unsigned char> &sectorData,
	bool &jmpFBFE,
	bool &ldx6E00, // Signature for F-BASIC 3.0
	bool &ldx4D00, // Signature for URADOS
	int &ldxPtr)
{
	jmpFBFE=false;
	ldx6E00=false;
	ldx4D00=false;
	ldxPtr=0;

	for(int i=0; i<sectorData.size(); ++i)
	{
		if(i+2<sectorData.size() && sectorData[i]==0x8e && sectorData[i+1]==0x4d && sectorData[i+2]==0x00)
		{
			ldx4D00=true;
			ldxPtr=i;
		}
		if(i+2<sectorData.size() && sectorData[i]==0x8e && sectorData[i+1]==0x6e && sectorData[i+2]==0x00)
		{
			ldx6E00=true;
			ldxPtr=i;
		}
		if(i+3<sectorData.size() && sectorData[i]==0x6e && sectorData[i+1]==0x9f && sectorData[i+2]==0xfb && sectorData[i+3]==0xfe)
		{
			jmpFBFE=true;
		}
	}
}

void SetUpSecondInstallation(std::vector <unsigned char> &clientCode,D77File::D77Disk &bootDisk)
{
	int jmp6E00Ptr=-1;
	for(int i=0; i<clientCode.size(); ++i)
	{
		if(i+2<clientCode.size() && clientCode[i]==0x7e && clientCode[i+1]==0x6e && clientCode[i+2]==0x00)
		{
			jmp6E00Ptr=i;
			break;
		}
	}

	if(0<=jmp6E00Ptr)
	{
		auto bootSector=bootDisk.ReadSector(0,0,1);

		bool jmpFBFE=false;
		bool ldx6E00=false;
		bool ldx4D00=false;
		int ldxPtr=0;
		IdentifyIPL(bootSector,jmpFBFE,ldx6E00,ldx4D00,ldxPtr);

		// URADOS
		if(true==jmpFBFE && true==ldx4D00)
		{
			//   JMP $6E00 -> JMP $4D00
			clientCode[jmp6E00Ptr+1]=0x4D;
			clientCode[jmp6E00Ptr+2]=0x00;
		}
	}
}

void AlterSectorData(
	int systemType,
	int track,
	int side,
	int sector,
	std::vector <unsigned char> &dat,
	unsigned int hookInstAddr,
	unsigned int fe02Offset,
	unsigned int fe05Offset,
	unsigned int fe08Offset)
{
	if(0==track && 0==side && 1==sector)
	{
		bool jmpFBFE=false;
		bool ldx6E00=false;
		bool ldx4D00=false;
		int ldxPtr=0;
		IdentifyIPL(dat,jmpFBFE,ldx6E00,ldx4D00,ldxPtr);

		// F-BASIC 3.0 IPL
		if(true==jmpFBFE && true==ldx6E00)
		{
			printf("Tweaking IPL for Disk F-BASIC 3.0\n");
			dat[ldxPtr+1]=0x60;
			dat[ldxPtr+2]=0x00;
		}
		// URADOS IPL
		if(true==jmpFBFE && true==ldx4D00)
		{
			printf("Tweaking IPL for URADOS\n");
			dat[ldxPtr+1]=0x40;	// Use one of the clones of the installer.
			dat[ldxPtr+2]=0x00;
		}
	}

	auto fe02Subst=hookInstAddr+fe02Offset;
	auto fe05Subst=hookInstAddr+fe05Offset;
	auto fe08Subst=hookInstAddr+fe08Offset;

	for(int i=0; i<dat.size(); ++i)
	{
		// JSR $FE0x / JMP $FE0x
		if(i+2<dat.size() && (dat[i]==0xbd || dat[i]==0x7e) && dat[i+1]==0xfe)
		{
			if(dat[i+2]==0x02)
			{
				dat[i+1]=(fe02Subst>>8)&0xff;
				dat[i+2]= fe02Subst&0xff;
			}
			else if(dat[i+2]==0x05)
			{
				dat[i+1]=(fe05Subst>>8)&0xff;
				dat[i+2]= fe05Subst&0xff;
			}
			else if(dat[i+2]==0x08)
			{
				dat[i+1]=(fe08Subst>>8)&0xff;
				dat[i+2]= fe08Subst&0xff;
			}
		}

		if(systemType==SYSTYPE_URADOS)
		{
			if(i+3<dat.size() && dat[i]==0x10 && dat[i+1]==0x27 && dat[i+2]==0x42 && dat[i+3]==0xCB)
			{
				// URADOS first failure at 0xBB39.  Calling FE08 as:
				//   LBEQ $FE08
				// but no disk.
				int offset=fe08Subst-0xBB3D;
				dat[i+2]=(offset & 0xff00)>>8;
				dat[i+3]=(offset&0xff);
			}
		}
	}
}


////////////////////////////////////////////////////////////



void ShowOptionHelp(void)
{
	printf("Usage:\n");
}

void ShowCommandHelp(void)
{
	printf("Command Help:\n");
	printf("IA..Transmit BIOS redirector installer to FM-7/77\n");
	printf("Q...Quit.\n");
	printf("H...Show this help.\n");
}

void Title(void)
{
	printf("********************************\n");
	printf("*                              *\n");
	printf("*  D77 Image Server            *\n");
	printf("*  by CaptainYS                *\n");
	printf("*  http://www.ysflight.com     *\n");
	printf("*                              *\n");
	printf("********************************\n");
	printf("Make sure to configure FM-7/77 side computer at 19200bps.\n");
}

void ShowPrompt(void)
{
	printf("Command {? for Help}>");
}

////////////////////////////////////////////////////////////


unsigned int GetDefaultInstallAddress(void)
{
	FM7BinaryFile binFile;
	binFile.DecodeSREC(clientBinary);
	return 0x100*binFile.dat[2]+binFile.dat[3];
}


////////////////////////////////////////////////////////////


class D77ServerCommandParameterInfo
{
public:
	std::string portStr;
	std::string d77FName;

	bool instAddrSpecified;
	unsigned int instAddr;

	bool redirectBiosCallMachingo;
	bool redirectBiosCallBinaryString;

	D77ServerCommandParameterInfo();
	bool Recognize(int ac,char *av[]);
	void CleanUp(void);

	void FinalizeInstallAddress(D77File::D77Disk *bootDiskPtr);
};

D77ServerCommandParameterInfo::D77ServerCommandParameterInfo()
{
	CleanUp();
}

void D77ServerCommandParameterInfo::CleanUp(void)
{
	portStr="";
	d77FName="";

	instAddrSpecified=false;
	instAddr=GetDefaultInstallAddress();

	redirectBiosCallMachingo=true;
	redirectBiosCallBinaryString=false;
}

bool D77ServerCommandParameterInfo::Recognize(int ac,char *av[])
{
	instAddrSpecified=false;

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
				instAddrSpecified=true;
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
				d77FName=arg;
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

void D77ServerCommandParameterInfo::FinalizeInstallAddress(D77File::D77Disk *bootDiskPtr)
{
	if(nullptr!=bootDiskPtr && true!=instAddrSpecified)
	{
		auto sectorData=bootDiskPtr->ReadSector(0,0,1);

		bool jmpFBFE=false;
		bool ldx6E00=false;
		bool ldx4D00=false;
		int ldxPtr=0;
		IdentifyIPL(sectorData,jmpFBFE,ldx6E00,ldx4D00,ldxPtr);

		// URADOS
		if(true==jmpFBFE && true==ldx4D00)
		{
			// Cannot use $FC00-FC7F
			instAddr=0x7F80;
		}
	}
	printf("Bios Hook Install Address=%04x\n",instAddr);
}

////////////////////////////////////////////////////////////


class FM7Disk
{
public:
	std::string d77FName;
	D77File::D77Disk *diskPtr;

	FM7Disk();
	~FM7Disk();
	void CleanUp();
};

FM7Disk::FM7Disk()
{
	CleanUp();
}
FM7Disk::~FM7Disk()
{
	CleanUp();
}
void FM7Disk::CleanUp()
{
	diskPtr=nullptr;
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


	int systemType;
	std::string d77FName[2];
	D77File d77File[2];
	FM7Disk drive[2];

	std::chrono::time_point<std::chrono::system_clock> lastByteReceivedClock;
	bool tapeSaved;

	D77ServerCommandParameterInfo cpi;



	FC80()
	{
		systemType=SYSTYPE_UNKNOWN;

		terminate=false;
		subCPUready=false;

		verbose=false;
		installASCII=false;

		lastByteReceivedClock=std::chrono::system_clock::now();
	}

	void SetInstallAddress(unsigned int instAddr)
	{
		std::lock_guard <std::mutex> lock(fd05);
		cpi.instAddr=instAddr;
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
	void InstallASCII(void)
	{
		std::lock_guard <std::mutex> lock(fd05);
		installASCII=true;
	}
	void SaveD77(const char fName[],int diskId) const
	{
		std::lock_guard <std::mutex> lock(fd05);
	}

	D77File::D77Disk *GetDiskFromBiosCmd(const unsigned char biosCmd[])
	{
		auto d=biosCmd[7];
		if(d<2)
		{
			return drive[d].diskPtr;
		}
		return nullptr;
	}

	void Halt(void)
	{
		fd05.lock();
	}
	void Unhalt(void)
	{
		fd05.unlock();
	}

	void IdentifySystemType(D77File::D77Disk *bootDiskPtr);
};

void FC80::IdentifySystemType(D77File::D77Disk *bootDiskPtr)
{
	if(nullptr!=bootDiskPtr)
	{
		auto sectorData=bootDiskPtr->ReadSector(0,0,1);

		bool jmpFBFE=false;
		bool ldx6E00=false;
		bool ldx4D00=false;
		int ldxPtr=0;
		IdentifyIPL(sectorData,jmpFBFE,ldx6E00,ldx4D00,ldxPtr);

		// URADOS
		if(true==jmpFBFE && true==ldx4D00)
		{
			printf("Identified URADOS IPL\n");
			systemType=SYSTYPE_URADOS;
		}
		else if(true==jmpFBFE && true==ldx6E00)
		{
			printf("Identified F-BASIC 3.0 IPL\n");
			systemType=SYSTYPE_F_BASIC_3_0;
		}
	}
}

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

	if("####EMPTY####"!=fc80.cpi.d77FName)
	{
		auto bin=FM7Lib::ReadBinaryFile(fc80.cpi.d77FName.c_str());
		if(0==bin.size())
		{
			fprintf(stderr,"Cannot open .D77 file.\n");
			return 1;
		}

		fc80.d77FName[0]=fc80.cpi.d77FName;
		fc80.d77File[0].SetData(bin);
		fc80.drive[0].diskPtr=fc80.d77File[0].GetDisk(0);
	}

	Title();

	fc80.IdentifySystemType(fc80.drive[0].diskPtr);
	fc80.cpi.FinalizeInstallAddress(fc80.drive[0].diskPtr);

	std::thread t(SubCPU);
	MainCPU();
	t.join();

	return 0;
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

		ShowPrompt();

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
		case 'V':
			fc80.SetVerboseMode(true==fc80.GetVerboseMode() ? false : true);
			printf("VerboseMode=%s\n",FM7Lib::BoolToStr(fc80.GetVerboseMode()));
			processed=true;
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
		STATE_WAIT_WRITE_DATA,
	};
	STATE state=STATE_NORMAL;

	int biosCmdFilled=0;
	unsigned char biosCmdBuf[8];
	int sectorDataFilled=0;
	int sectorDataNeeded=256;
	unsigned char sectorDataBuf[1024];


	FM7BinaryFile clientCode;
	clientCode.DecodeSREC(clientBinary);
	if(nullptr!=fc80.drive[0].diskPtr)
	{
		SetUpSecondInstallation(clientCode.dat,*fc80.drive[0].diskPtr);
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
			clientCode.dat[2]=((fc80.cpi.instAddr>>8)&255);
			clientCode.dat[3]=(fc80.cpi.instAddr&255);

			printf("Install Addr=%04x\n",fc80.cpi.instAddr);

			printf("\nTransmitting Installer ASCII\n");
			int ctr=0;
			for(auto c : clientCode.dat)
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
			for(auto i=clientCode.dat.size(); i<256; ++i)
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
				biosCmdBuf[biosCmdFilled++]=c;
				if(8==biosCmdFilled)
				{
					if(0x09==biosCmdBuf[0]) // Disk Write
					{
						state=STATE_WAIT_WRITE_DATA;
						sectorDataFilled=0;
						sectorDataNeeded=256;

						int track=biosCmdBuf[4];
						int sector=biosCmdBuf[5];
						int side=biosCmdBuf[6];

						auto diskPtr=fc80.GetDiskFromBiosCmd(biosCmdBuf);
						if(nullptr!=diskPtr)
						{
							auto sectorData=diskPtr->ReadSector(track,side,sector);
							unsigned int sectorSize=sectorData.size();
							if(1024<sectorSize)
							{
								sectorSize=1024;
							}
							else if(sectorSize<128)
							{
								sectorSize=128;
							}
							sectorSize>>=7;
							const unsigned char sendBuf[1]={sectorSize};
							comPort.Send(1,sendBuf);

							sectorDataFilled=0;
							sectorDataNeeded=sectorSize;
						}
						else
						{
							const unsigned char sendBuf[1]={1};  // Make it fake 128-byte sector
							comPort.Send(1,sendBuf);
							sectorDataFilled=0;
							sectorDataNeeded=128;
						}
					}
					else if(0x0a==biosCmdBuf[0]) // Disk Read
					{
						int track=biosCmdBuf[4];
						int sector=biosCmdBuf[5];
						int side=biosCmdBuf[6];

						printf("R Trk:%d Sid:%d Sec:%d\n",track,side,sector);

						auto diskPtr=fc80.GetDiskFromBiosCmd(biosCmdBuf);
						if(nullptr!=diskPtr)
						{
							auto sectorData=diskPtr->ReadSector(track,side,sector);

							AlterSectorData(
								fc80.systemType,
								track,side,sector,
							    sectorData,
							    fc80.cpi.instAddr,
							    clientCode.dat[clientCode.dat.size()-3],
							    clientCode.dat[clientCode.dat.size()-2],
							    clientCode.dat[clientCode.dat.size()-1]);

							unsigned int sectorSize=sectorData.size();
							sectorSize>>=7;
							const unsigned char sendBuf[1]={sectorSize};
							comPort.Send(1,sendBuf);

							comPort.Send(sectorData.size(),sectorData.data());

							unsigned char errCode[1]={0};
							if(true==diskPtr->GetDDM(track,side,sector))
							{
								errCode[0]=BIOS_ERROR_DDM_ERROR;
							}
							if(true==diskPtr->GetCRCError(track,side,sector))
							{
								errCode[0]=BIOS_ERROR_CRC_ERROR;
							}
							comPort.Send(1,errCode);
						}
						else
						{
							unsigned char sendBuf[130]={1};  // Make it fake 128-byte sector
							sendBuf[0]=1;
							for(int i=0; i<128; ++i)
							{
								sendBuf[1+i]=0;
							}
							sendBuf[129]=BIOS_ERROR_HARD_ERROR;
							comPort.Send(1,sendBuf);
						}
					}
					biosCmdFilled=0;
				}
				break;
			case STATE_WAIT_WRITE_DATA:
				{
					int track=biosCmdBuf[4];
					int sector=biosCmdBuf[5];
					int side=biosCmdBuf[6];

					sectorDataBuf[sectorDataFilled++]=c;
					if(sectorDataFilled==sectorDataNeeded)
					{
						auto diskPtr=fc80.GetDiskFromBiosCmd(biosCmdBuf);
						if(nullptr!=diskPtr)
						{
							if(true==diskPtr->IsWriteProtected())
							{
								const unsigned char errCode[]={BIOS_ERROR_WRITE_PROTECTED};
								comPort.Send(1,errCode);
							}
							else
							{
								auto written=diskPtr->WriteSector(track,side,sector,sectorDataFilled,sectorDataBuf);
								if(0<written)
								{
									const unsigned char errCode[]={0};
									comPort.Send(1,errCode);
									diskPtr->SetModified();
								}
								else
								{
									const unsigned char errCode[]={BIOS_ERROR_HARD_ERROR};
									comPort.Send(1,errCode);
								}
							}
						}
						else
						{
							const unsigned char errCode[]={BIOS_ERROR_DRIVE_NOT_READY};
							comPort.Send(1,errCode);
						}
					}
					state=STATE_NORMAL;
				}
				break;
			}
			fc80.lastByteReceivedClock=std::chrono::system_clock::now();
		}
		fc80.Unhalt();

		for(auto &d77 : fc80.d77File)
		{
			bool modified=false;
			for(int i=0; i<d77.GetNumDisk(); ++i)
			{
				if(true==d77.GetDisk(i)->IsModified())
				{
					modified=true;
					break;
				}
			}

			if(true==modified && 
			   std::chrono::milliseconds(50)<std::chrono::system_clock::now()-fc80.lastByteReceivedClock)
			{
				printf("Received data.\n");
				printf("Auto-Save not implemented yet.\n"); // Save here.
				for(int i=0; i<d77.GetNumDisk(); ++i)
				{
					d77.GetDisk(i)->ClearModified();
				}
				ShowPrompt();
			}
		}


		if(activityTimer<std::chrono::system_clock::now())
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
			if(true==activity)
			{
				activity=false;
				ShowPrompt();
			}
		}
	}

	comPort.Close();
}
