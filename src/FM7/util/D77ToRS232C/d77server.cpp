#include <vector>
#include <string>
#include <chrono>
#include <thread>
#include <unordered_set>
#include <d77.h>
#include <d77filesystem.h>
#include <comport.h>

#include "cpplib.h"
#include "fm7lib.h"
#include "formatdata.h"

#include "clientbin.h"
#include "diskimg.h"



const int FM7_Side_Data_Ptr=0x2800;



////////////////////////////////////////////////////////////

std::vector <std::string> MakeFromBinary(
    const std::vector <unsigned char> &bin,const char filenameInFM7[],int storeAddr,int execAddr)
{
	std::vector <std::string> txt;
	std::string s;

	printf("Store Addr=%X  End Addr=%X  Exec Addr=%x\n",storeAddr,storeAddr+bin.size()-1,execAddr);

	s="F";
	s=s+filenameInFM7;
	txt.push_back(s);


	unsigned int checkSum=0,checkXor=0;
	for(auto c : bin)
	{
		checkSum+=c;
		checkXor^=c;
	}

	char idStr[256];
	sprintf(idStr,"B%04x%04x%04x%04x%02x",
		storeAddr,
		(int)bin.size(),
		execAddr,
		(checkSum&0xffff),
		(checkXor&0xff));
	txt.push_back(idStr);


	const long long int unit=16;
	for(long long int ptr=0; ptr<bin.size(); ptr+=unit)
	{
		std::string s;
		long long int sizeLeft=bin.size()-ptr;
		long long int toCopy=unit;
		if(unit<sizeLeft)
		{
			toCopy=unit;
		}
		else
		{
			toCopy=sizeLeft;
		}
		for(int i=0; i<toCopy; ++i)
		{
			char x[16];
			sprintf(x,"%02x",bin[ptr+i]);
			s.push_back(x[0]);
			s.push_back(x[1]);
		}
		txt.push_back(s);
	}

	txt.push_back("QQQ");
	return txt;
}

std::vector <unsigned char> MakeTrackDump(const D77File::D77Disk &dsk,int trk,int sid)
{
	std::vector <unsigned char> dump;
	auto trkPtr=dsk.GetTrack(trk,sid);
	if(nullptr!=trkPtr)
	{
		for(auto secLoc : trkPtr->AllSector())
		{
			auto secPtr=trkPtr->FindSector(secLoc.sector);
			if(nullptr!=secPtr)
			{
				int sizeShift=0;

				long long int secSize=128;
				while(secSize<(long long int)secPtr->sectorData.size())
				{
					++sizeShift;
					secSize<<=1;
				}

				dump.push_back(trk);
				dump.push_back(sid);
				dump.push_back(secLoc.sector);
				dump.push_back(sizeShift);
				for(int i=0; i<secSize; ++i)
				{
					if(i<secPtr->sectorData.size())
					{
						dump.push_back(secPtr->sectorData[i]);
					}
					else
					{
						dump.push_back(0);
					}
				}
			}
		}
	}
	dump.push_back(0xff);
	dump.push_back(0xff);
	dump.push_back(0xff);
	dump.push_back(0xff);
	dump.push_back(0xff);
	dump.push_back(0xff);
	dump.push_back(0xff);
	dump.push_back(0xff);
	return dump;
}

////////////////////////////////////////////////////////////

void ShowHelp(void)
{
	printf("Usage:\n");
	printf("  D77Server d77file.d77 comPort\n");
	printf("For multi-disk D77 image,\n");
	printf("  D77Server d77file.d77@diskId comPort\n");
	printf("\n");
	printf("diskId....0-based index to the disk.\n");
	printf("comPort...A number\n");
	printf("\n");
	printf("Specify ####UTILDISK#### as d77 file name to transmit a RS232C utility disk.\n");
	printf("Case sensitive.\n");
	printf("\n");
	printf("-fbasic\n");
	printf("\tConsider disk as F-BASIC formatted.\n");
	printf("\tIf the disk is not used fully based on the File Allocation Table,\n");
	printf("\tit will create .WAV/.T77 only for used part of the disk.\n");
	printf("-deldupsec\n");
	printf("\tDelete duplicate sectors.\n");
	printf("-renumfx\n");
	printf("\tRenumber F5,F6,F7, and F8 sectors to E5,E6,E7, and E8 sectors.\n");
	printf("\tThis option may write sector contents to the disk, but it\n");
	printf("\tdoes not break copy protection.\n");
	printf("-waitscale number\n");
	printf("\tGive 2 or 3 when FM-7 side misses bytes or re-tries too often.\n");
	printf("\tIt will slow down the transmission, but reduces the chance of\n");
	printf("\tbuffer overflow on the FM-7 side.\n");
	printf("-noquit\n");
	printf("\tWith this option, the server will not terminate when client signals\n");
	printf("\tend of disk.  Use this option when you need to burn multiple disks\n");
	printf("\tfrom a same image.\n");
	printf("-h, -help, -?\n");
	printf("\tShow this help.\n");
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

////////////////////////////////////////////////////////////

class D77ServerCommandParameterInfo
{
public:
	std::string portStr;
	int diskNumber;
	std::string d77FName;

	bool fBasic;
	bool renumFx;
	bool delDuplicateSec;
	bool autoQuit;
	int waitScale;

	D77ServerCommandParameterInfo();
	bool Recognize(int ac,char *av[]);
	void CleanUp(void);
};

D77ServerCommandParameterInfo::D77ServerCommandParameterInfo()
{
	CleanUp();
}

void D77ServerCommandParameterInfo::CleanUp(void)
{
	portStr="";
	diskNumber=0;
	d77FName="";

	fBasic=false;
	renumFx=false;
	delDuplicateSec=false;
	autoQuit=true;
	waitScale=1;
}

bool D77ServerCommandParameterInfo::Recognize(int ac,char *av[])
{
	int fixedOrderIndex=0;
	for(int i=1; i<ac; ++i)
	{
		std::string arg(av[i]);
		if("-h"==arg || "-help"==arg || "-?"==arg)
		{
			ShowHelp();
		}
		else if("-fbasic"==arg)
		{
			fBasic=true;
		}
		else if("-deldupsecn"==arg)
		{
			delDuplicateSec=true;
		}
		else if("-renumfx"==arg)
		{
			renumFx=true;
		}
		else if("-noquit"==arg)
		{
			autoQuit=false;
		}
		else if("-waitscale"==arg && i+1<ac)
		{
			waitScale=FM7Lib::Atoi(av[i+1]);
			if(waitScale<1)
			{
				waitScale=1;
			}
			++i;
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
				for(auto i=d77FName.size()-1; 3<=i; --i)
				{
					if('@'==d77FName[i] &&
					   ('7'==d77FName[i-1] || '8'==d77FName[i-1]) &&
					   ('7'==d77FName[i-2] || '8'==d77FName[i-2]) &&
					   ('d'==d77FName[i-3] || 'D'==d77FName[i-3]))
					{
						diskNumber=atoi(d77FName.data()+i+1);
						d77FName.resize(i);
						break;
					}
				}
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

class D77Server
{
public:
	D77ServerCommandParameterInfo cpi;
	D77File d77;
	FormatData fomData;
	YsCOMPort comPort;

	D77Server();
	bool Run(int ac,char *av[]);
};



D77Server::D77Server()
{
}

bool D77Server::Run(int ac,char *av[])
{
	D77ServerCommandParameterInfo cpi;
	if(true!=cpi.Recognize(ac,av))
	{
		fprintf(stderr,"Cannot recognize command parameters.  (-h for help)\n");
		return false;
	}


	std::vector <unsigned char> rawD77;

	if("####UTILDISK####"==cpi.d77FName)
	{
		cpi.fBasic=true;
		rawD77.assign(utilDiskImg,utilDiskImg+utilDiskImg_size);
	}
	else
	{
		rawD77=FM7Lib::ReadBinaryFile(av[1]);
		if(0==rawD77.size())
		{
			fprintf(stderr,"Cannot read %s\n",av[1]);
			return false;
		}
	}
	d77.SetData(rawD77);

	auto diskPtr=d77.GetDisk(cpi.diskNumber);
	if(nullptr==diskPtr)
	{
		fprintf(stderr,"D77 file does not have disk #%d\n",cpi.diskNumber);
		return false;
	}


	if(true==cpi.delDuplicateSec)
	{
		for(auto loc : diskPtr->AllTrack())
		{
			diskPtr->DeleteDuplicateSector(loc.track,loc.side);
		}
	}
	if(true==cpi.renumFx)
	{
		for(auto trkLoc : diskPtr->AllTrack())
		{
			auto trkPtr=diskPtr->FindTrack(trkLoc.track,trkLoc.side);
			if(nullptr!=trkPtr)
			{
				for(auto secLoc : trkPtr->AllSector())
				{
					if(0xf5==secLoc.sector ||
					   0xf6==secLoc.sector ||
					   0xf7==secLoc.sector)
					{
						diskPtr->RenumberSector(trkLoc.track,trkLoc.side,secLoc.sector,secLoc.sector-0x10);
					}
				}
			}
		}
	}


	std::unordered_set <int> targetTrack; // 0-79
	if(true!=cpi.fBasic)
	{
		for(int i=0; i<160; ++i)
		{
			targetTrack.insert(i);
		}
	}
	else
	{
		for(int trk=0; trk<4; ++trk)
		{
			targetTrack.insert(trk);
		}

		D77FileSystem fileSys;
		if(true!=fileSys.ReadDirectory(*diskPtr))
		{
			fprintf(stderr,"Bad File Structure\n");
			return false;
		}

		auto FAT=fileSys.GetFAT();
		for(int clst=0; clst<FAT.size(); ++clst)
		{
			if(0xff!=FAT[clst])
			{
				int trk,sid,sec;
				fileSys.ClusterToTrackSideSector(trk,sid,sec,clst);
				targetTrack.insert(trk*2+sid);
			}
		}
	}



	if(true!=fomData.MakeFormatData(*diskPtr))
	{
		fprintf(stderr,"Failed to create a format data.\n");
		fprintf(stderr,"Maybe a copy-protected disk?\n");
		return false;
	}

	printf("Format data ready.\n");


	Title();


	comPort.SetDesiredBaudRate(19200);
	comPort.SetDesiredBitLength(YsCOMPort::BITLENGTH_8);
	comPort.SetDesiredStopBit(YsCOMPort::STOPBIT_1);
	comPort.SetDesiredParity(YsCOMPort::PARITY_NOPARITY);
	comPort.SetDesiredFlowControl(YsCOMPort::FLOWCONTROL_NONE);
	if(true!=comPort.Open(atoi(cpi.portStr.data())))
	{
		fprintf(stderr,"Cannot open port %s.\n",cpi.portStr.data());
		return false;
	}


	bool quitFlag=false;
	std::string cmdLine;
	while(true!=quitFlag)
	{
		// Regading PC -> FM-7 transmission
		// There is no FIFO buffer on FM-7 side.
		//
		// 34 bytes (32 hexadecimal+0x0d+0x0a)=288 bits
		// 19200 bps
		// 19200/288=66 lines per second
		// Plus incrementing counter, address, etc.  To be safe, assume 40 lines per sec.

		auto recv=comPort.Receive();
		for(auto b : recv)
		{
			printf("(%02x)\n",b);

			if(0x0d==b || 0x0a==b)
			{
				printf("Command: %s\n",cmdLine.c_str());
				
				// Process command
				if("REQFMT"==cmdLine)
				{
					// Wait for old 6809 to be ready to receive.
					std::this_thread::sleep_for(std::chrono::milliseconds(100*cpi.waitScale));
					printf("Transmit Format Info\n");
					for(auto str : MakeFromBinary(fomData.formatData,"FORMATD",FM7_Side_Data_Ptr,FM7_Side_Data_Ptr))
					{
						printf("FMT>> %s\n",str.data());
						str.push_back(0x0d);
						str.push_back(0x0a);
						comPort.Send(str.size(),(const unsigned char *)str.data());

						std::this_thread::sleep_for(std::chrono::milliseconds(25*cpi.waitScale));
					}
					printf("Transmitted Format Info\n");
				}
				else if(true==FM7Lib::StrStartsWith(cmdLine,"REQTRK"))
				{
					if(cmdLine.size()<10)
					{
						fprintf(stderr,"Request-Track command cut short.\n");
						continue;
					}

					std::string trkStr,sidStr;
					trkStr.push_back(cmdLine[6]);
					trkStr.push_back(cmdLine[7]);
					sidStr.push_back(cmdLine[8]);
					sidStr.push_back(cmdLine[9]);

					std::string fName="TRK";
					fName.push_back(cmdLine[6]);
					fName.push_back(cmdLine[7]);
					fName.push_back(cmdLine[8]);
					fName.push_back(cmdLine[9]);

					auto trk=FM7Lib::Atoi(trkStr.c_str());
					auto sid=FM7Lib::Atoi(sidStr.c_str());

					printf("Transmit Track %d Side %d\n",trk,sid);
					auto dump=MakeTrackDump(*diskPtr,trk,sid);
					if(0==targetTrack.count(trk*2+sid))
					{
						dump.clear();
						dump.push_back(0xff);
						dump.push_back(0xff);
						dump.push_back(0xff);
						dump.push_back(0xff);
						dump.push_back(0xff);
						dump.push_back(0xff);
						dump.push_back(0xff);
						dump.push_back(0xff);
					}

					for(auto str : MakeFromBinary(dump,fName.c_str(),FM7_Side_Data_Ptr,FM7_Side_Data_Ptr))
					{
						std::this_thread::sleep_for(std::chrono::milliseconds(25*cpi.waitScale));

						printf("TRK:%02d SID:%02d>> %s\n",trk,sid,str.data());
						str.push_back(0x0d);
						str.push_back(0x0a);
						comPort.Send(str.size(),(const unsigned char *)str.data());
					}
					
					printf("Transmitted Track %d Side %d\n",trk,sid);
				}
				else if("REQCLI"==cmdLine)
				{
					FM7BinaryFile binFile;
					binFile.DecodeSREC(clientBinary);

					printf("Transmitting Client\n");
					char str[256];
					for(auto c : binFile.dat)
					{
						std::this_thread::sleep_for(std::chrono::milliseconds(20*cpi.waitScale));
						sprintf(str,"%02x",(unsigned int)c);
						str[2]=0x0d;
						str[3]=0x0a;
						str[4]=0;
						comPort.Send(4,(unsigned char *)str);
					}

					std::this_thread::sleep_for(std::chrono::milliseconds(20*cpi.waitScale));
					str[0]='Q';
					str[1]='Q';
					str[2]='Q';
					str[3]=0x0d;
					str[4]=0x0a;
					str[5]=0;
					comPort.Send(5,(unsigned char *)str);
					printf("Transmitted Client\n");
				}
				else if(true==FM7Lib::StrStartsWith(cmdLine,"QQQ"))
				{
					printf("Disk Finished.\n");
					quitFlag=cpi.autoQuit;
				}
				cmdLine="";
			}
			else
			{
				cmdLine.push_back(b);
			}
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(20*cpi.waitScale));
	}


	return true;
}

int main(int ac,char *av[])
{
	D77Server server;
	server.Run(ac,av);
	return 0;
}

