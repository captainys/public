#include "cpplib.h"
#include "fm7lib.h"
#include "rawread.h"

#include <string>
#include <iostream>
#include <fstream>
#include <algorithm>

#ifdef max
#undef max
#endif


unsigned int FM7RawDiskRead::HasCheckSum::CalculateCheckSum(void) const
{
	unsigned int sum=0;
	for(auto c : dat)
	{
		sum+=c;
	}
	sum&=0xffff;
	return sum;
}
unsigned int FM7RawDiskRead::HasCheckSum::CalculateXor(void) const
{
	unsigned int sum=0;
	for(auto c : dat)
	{
		sum^=c;
	}
	return sum;
}

unsigned long long int FM7RawDiskRead::Sector::CalculateSize(void) const
{
	unsigned long long nByte=128;
	// FM Music World.  Size was set to F5.
	nByte<<=(N&3);
	return nByte;
}

D77File::D77Disk::D77Sector FM7RawDiskRead::Sector::MakeD77Sector(long long int nSectorTrack,int density) const
{
	//	class D77Sector
	//	{
	//	public:
	//		unsigned char cylinder;
	//		unsigned char head;
	//		unsigned char sector;
	//		unsigned char sizeShift;  // 128<<sizeShift=size
	//		unsigned short nSectorTrack;
	//		unsigned char density;
	//		unsigned char deletedData;
	//		unsigned char crcStatus;
	//		unsigned char reservedByte[5];
	//		unsigned short sectorDataSize; // Excluding the header.
	//		std::vector <unsigned char> sectorData;
	//		D77Sector();
	//		~D77Sector();
	//		void CleanUp(void);
	//	};

	D77File::D77Disk::D77Sector d77sec;
	d77sec.cylinder=C;
	d77sec.head=H;
	d77sec.sector=R;
	d77sec.sizeShift=N;
	d77sec.nSectorTrack=(short)nSectorTrack;
	d77sec.density=density;
	d77sec.deletedData=(0x0E==errCode ? 0x10 : 0);  // FM-7 BIOS Error Code 0E
	d77sec.crcStatus=(0x0D==errCode ? 0xB0 : 0);    // FM-7 BIOS Error Code 0D
	for(auto &c : d77sec.reservedByte)
	{
		c=0;
	}
	d77sec.sectorDataSize=CalculateSize();
	d77sec.sectorData=dat;

	return d77sec;
}

FM7RawDiskRead::Track::Track()
{
	CleanUp();
}
void FM7RawDiskRead::Track::CleanUp(void)
{
	trk=0;
	sid=0;
	lastErr=0;

	sectorInfo.clear();
	sectorDump.clear();
	rawRead.clear();
	sec.clear();
}
bool FM7RawDiskRead::Track::CheckSumTest(void) const
{
	for(auto &s : sectorDump)
	{
		if(s.dat.size()!=s.mustBeSize)
		{
			return false;
		}
	}
	for(auto &rr : rawRead)
	{
		auto checkSum=rr.CalculateCheckSum();
		auto byteWiseXor=rr.CalculateXor();
		if(checkSum!=rr.mustBeCheckSum)
		{
			return false;
		}
		if(byteWiseXor!=rr.mustBeXor)
		{
			return false;
		}
		if(rr.dat.size()!=rr.mustBeSize)
		{
			return false;
		}
	}
	for(auto &si : sectorInfo)
	{
		if(si.dat.size()!=si.mustBeSize)
		{
			return false;
		}
	}
	return true;
}
void FM7RawDiskRead::Track::PrintStat(void) const
{
	printf("Track:%d  Side:%d\n",trk,sid);
	if(0<sectorInfo.size())
	{
		printf("Number of Sectors:%d",(int)sectorInfo[0].dat.size()/5);
		printf("\n");
	}
	for(int i=0; i<sectorInfo.size(); ++i)
	{
		auto &si=sectorInfo[i];
		printf("Sector Info Sample: %d\n",i);
		printf("  Size: %d",(int)si.dat.size());
		if(si.dat.size()!=si.mustBeSize)
		{
			printf("  Mismatch  Expected:%d",(int)si.mustBeSize);
		}
		printf("\n");
	}
	for(int i=0; i<sectorDump.size(); ++i)
	{
		auto &sd=sectorDump[i];
		printf("  Size: %d",(int)sd.dat.size());
		if(sd.dat.size()!=sd.mustBeSize)
		{
			printf("  Mismatch  Expected:%d",(int)sd.mustBeSize);
		}
		printf("\n");

		auto checkSum=sd.CalculateCheckSum();
		auto byteWiseXor=sd.CalculateXor();
		printf("Sector Dump Sample: %d\n",i);
		printf("  Check Sum:%04x",(int)checkSum);
		if(checkSum!=sd.mustBeCheckSum)
		{
			printf("  Mismatch  Expected:%04x",(int)sd.mustBeCheckSum);
		}
		printf("\n");
		printf("  XOR:%04x",(int)byteWiseXor);
		if(byteWiseXor!=sd.mustBeXor)
		{
			printf("  Mismatch  Expected:%04x",(int)sd.mustBeXor);
		}
		printf("\n");
	}
	for(int i=0; i<rawRead.size(); ++i)
	{
		auto &rr=rawRead[i];
		auto checkSum=rr.CalculateCheckSum();
		auto byteWiseXor=rr.CalculateXor();
		printf("Raw Read Sample %d\n",i);
		printf("  Size: %d",(int)rr.dat.size());
		if(rr.dat.size()!=rr.mustBeSize)
		{
			printf("  Mismatch  Expected:%d",(int)rr.mustBeSize);
		}
		printf("\n");
		printf("  Check Sum:%04x",(int)checkSum);
		if(checkSum!=rr.mustBeCheckSum)
		{
			printf("  Mismatch  Expected:%04x",(int)rr.mustBeCheckSum);
		}
		printf("\n");

		printf("  XOR:%04x",(int)byteWiseXor);
		if(byteWiseXor!=rr.mustBeXor)
		{
			printf("  Mismatch  Expected:%04x",(int)rr.mustBeXor);
		}
		printf("\n");
	}
}

bool FM7RawDiskRead::Track::SectorDumpToSector(const SectorInfo &info,const SectorDump &dump)
{
	long long int dumpPtr=0;
	if(info.dat.size()!=info.mustBeSize)
	{
		fprintf(stderr,"Received sector info size and actual size mismatch.\n");
		return false;
	}
	if(dump.dat.size()!=dump.mustBeSize)
	{
		fprintf(stderr,"Received sector info size and actual size mismatch.\n");
		return false;
	}

	if(2==dump.version)
	{
		// From Silpheed copy protection (was not able to confirm due to floppy-disk contamination):
		// Track 0 Sector 8 CHRS tells it is 256 bytes.  However, actually it seems to return only 220 bytes or so with no CRC error.
		// I didn't know it was possible, but since it happens, it is a good idea to return actually-read size for future inspection.
		// Version 2 does it, although it doesn't transfer to D77.
		int nSecFromInfo=info.dat.size()/5;
		sec.clear();
		while(dumpPtr+16<=dump.dat.size())
		{
			Sector s;
			s.C=dump.dat[dumpPtr];
			s.H=dump.dat[dumpPtr+1];
			s.R=dump.dat[dumpPtr+2];
			s.N=dump.dat[dumpPtr+3];
			s.fdcState=dump.dat[dumpPtr+6];
			s.errCode=dump.dat[dumpPtr+7];

			unsigned int actualSizeHigh=dump.dat[dumpPtr+4]; // Caution! Motorola byte order
			unsigned int actualSizeLow=dump.dat[dumpPtr+5];
			unsigned int actualSize=256*actualSizeHigh+actualSizeLow;

			s.dat.resize(s.CalculateSize());
			for(auto &c : s.dat)
			{
				c=0;
			}
			for(int j=0; j<actualSize && j<s.CalculateSize() && j+dumpPtr+16<dump.dat.size(); ++j)
			{
				s.dat[j]=dump.dat[dumpPtr+16+j];
			}
			dumpPtr+=16;
			dumpPtr+=((actualSize+15)&0xfffffff0);
			sec.push_back((Sector &&)s);
		}
		if(nSecFromInfo!=sec.size())
		{
			fprintf(stderr,"Actual number of sectors and format-info size mismatch (%d vs %d).\n",(int)sec.size(),nSecFromInfo);
			return false;
		}
	}
	else // Assume version 1
	{
		int nSec=info.dat.size()/5;
		sec.resize(nSec);
		for(int i=0; i<nSec; ++i)
		{
			auto &s=sec[i];
			s.C=info.dat[i*5];
			s.H=info.dat[i*5+1];
			s.R=info.dat[i*5+2];
			s.N=info.dat[i*5+3];
			s.errCode=info.dat[i*5+4];
			switch(info.dat[i*5+4])
			{
			case DISKERR_CRCERROR:
				s.fdcState=FDCSTATE_CRCERROR;
				break;
			case DISKERR_DDMARK:
				s.fdcState=FDCSTATE_DELETEDDATA;
				break;
			default:
				s.fdcState=0;
				break;
			}
			s.dat.resize(s.CalculateSize());
			for(auto &c : s.dat)
			{
				c=0;
			}
			for(long long j=0; j<s.CalculateSize() && j+dumpPtr<dump.dat.size(); ++j)
			{
				s.dat[j]=dump.dat[j+dumpPtr];
			}
			dumpPtr+=s.CalculateSize();
		}
		if(dumpPtr!=dump.dat.size())
		{
			fprintf(stderr,"Calculated sector size and sector dump size mismatch.\n");
			return false;
		}
		return true;
	}
	return false;
}

bool FM7RawDiskRead::Track::SectorDumpToSector(void)
{
	if(0<sectorInfo.size() && 0<rawRead.size() && 0<sectorDump.size())
	{
		return SectorDumpToSector(sectorInfo[0],sectorDump[0]);
	}
	return false;
}

D77File::D77Disk::D77Sector FM7RawDiskRead::Track::MakeD77Sector(int sectorId) const
{
	if(0<=sectorId && sectorId<sec.size())
	{
		return sec[sectorId].MakeD77Sector(sec.size(),0);
	}
	else
	{
		D77File::D77Disk::D77Sector empty;
		return empty;
	}
}

////////////////////////////////////////////////////////////

void FM7RawDiskRead::CleanUp(void)
{
	diskName="";
	trk.clear();
}

void FM7RawDiskRead::VerifyCheckSum(void) const
{
	printf("Verifying Disk Name: %s\n",diskName.c_str());
	bool foundErr=false;
	for(auto &t : trk)
	{
		if(true!=t.CheckSumTest())
		{
			t.PrintStat();
			foundErr=true;
		}
	}
	if(true!=foundErr)
	{
		printf("Checksum, xor, and size look all OK.\n");
	}
}

/* static */ std::vector <FM7RawDiskRead> FM7RawDiskRead::Load(const char fn[])
{
	std::vector <FM7RawDiskRead> diskArray;

	std::ifstream ifp;
	ifp.open(fn);

	std::string str;
	while(std::getline(ifp,str))
	{
		if(true==FM7Lib::StrStartsWith(str,"++++BEGIN_DISK"))
		{
			diskArray.push_back(LoadDisk(ifp));

			auto ptr=str.c_str();
			while(nullptr!=ptr && 0!=*ptr)
			{
				ptr=FM7Lib::StrSkipNonSpace(ptr);
				ptr=FM7Lib::StrSkipSpace(ptr);

				if(true==FM7Lib::StrStartsWith(ptr,"NAME:"))
				{
					ptr+=5;
					ptr=FM7Lib::StrSkipSpace(ptr);
					diskArray.back().diskName=ptr;
				}
				else if(true==FM7Lib::StrStartsWith(ptr,"2DD"))
				{
					diskArray.back().mediaType=FM7RawDiskRead::MEDIA_2DD;
				}
			}
		}
	}

	return diskArray;
}

D77File::D77Disk FM7RawDiskRead::MakeD77Disk(void) const
{
	int nTrk=0;
	for(auto &t : trk)
	{
		nTrk=std::max(nTrk,t.trk+1);
	}
	nTrk*=2;

	D77File::D77Disk disk;
	disk.CreateUnformatted(nTrk,diskName.c_str());
	disk.header.mediaType=this->mediaType;

	for(auto &t : trk)
	{
		int c=t.trk;
		int h=t.sid;
		std::vector <D77File::D77Disk::D77Sector> trackSector;
		// First add all sectors with matching c and h
		for(int s=0; s<t.sec.size(); ++s)
		{
			if(c==t.sec[s].C && h==t.sec[s].H)
			{
				trackSector.push_back(t.MakeD77Sector(s));
			}
		}
		// And then mismatching c and h.
		for(int s=0; s<t.sec.size(); ++s)
		{
			if(c!=t.sec[s].C || h!=t.sec[s].H)
			{
				trackSector.push_back(t.MakeD77Sector(s));
			}
		}
		disk.WriteTrack(c,h,trackSector.size(),trackSector.data());
	}
	return disk;
}

/* static */ FM7RawDiskRead FM7RawDiskRead::LoadDisk(std::istream &ifp)
{
	FM7RawDiskRead disk;

	std::string str;
	while(std::getline(ifp,str))
	{
		if(true==FM7Lib::StrStartsWith(str,"++++TRACK"))
		{
			disk.trk.push_back(LoadTrack(ifp,str));
			disk.trk.back().SectorDumpToSector();
			disk.trk.back().PrintStat();
		}
		else if(true==FM7Lib::StrStartsWith(str,"++++END_DISK"))
		{
			break;
		}
	}

	return disk;
}

/* static */ FM7RawDiskRead::Track FM7RawDiskRead::LoadTrack(std::istream &ifp,const std::string &firstLine)
{
	enum STATE
	{
		STATE_OUTSIDE,
		STATE_SECTOR_INFO,
		STATE_SECTOR_DUMP,
		STATE_RAW_READ
	};
	STATE state=STATE_OUTSIDE;
	Track track;


	{
		const char *ptr=FM7Lib::StrSkip(firstLine.c_str(),"TRACK:");
		if(nullptr!=ptr)
		{
			track.trk=FM7Lib::Atoi(FM7Lib::StrSkipSpace(ptr));
		}
		ptr=FM7Lib::StrSkip(firstLine.c_str(),"SIDE:");
		if(nullptr!=ptr)
		{
			track.sid=FM7Lib::Atoi(FM7Lib::StrSkipSpace(ptr));
		}
	}

	printf("%d %d\n",track.trk,track.sid);


	std::string str;
	while(std::getline(ifp,str))
	{
		if(true==FM7Lib::StrStartsWith(str,"++++END_TRACK"))
		{
			break;
		}
		if(true==FM7Lib::StrStartsWith(str,"++++SECTORINFO"))
		{
			track.sectorInfo.push_back(LoadSectorInfo(ifp));
			continue;
		}
		else if(true==FM7Lib::StrStartsWith(str,"++++SECTORDUMP"))
		{
			track.sectorDump.push_back(LoadSectorDump(ifp));
			continue;
		}
		else if(true==FM7Lib::StrStartsWith(str,"++++RAWREAD"))
		{
			track.rawRead.push_back(LoadRawRead(ifp));
			continue;
		}
		else if(true==FM7Lib::StrStartsWith(str,"++++ADDRMARK"))
		{
			LoadAddrMark(ifp);
			continue;
		}

		auto ptr=FM7Lib::StrSkip(str.c_str(),"LAST_IRQ:");
		if(nullptr!=ptr)
		{
			continue;
		}
		ptr=FM7Lib::StrSkip(str.c_str(),"LAST_ERR:");
		if(nullptr!=ptr)
		{
			track.lastErr=FM7Lib::Atoi(FM7Lib::StrSkipSpace(ptr));
			continue;
		}
	}

	return track;
}

void FM7RawDiskRead::LoadAddrMark(std::istream &ifp)
{
	std::string str;
	while(std::getline(ifp,str))
	{
		if(true==FM7Lib::StrStartsWith(str,"++++END_ADDRMARK"))
		{
			break;
		}
		str=ShortFormat(str);
	}
}

/* static */ FM7RawDiskRead::RawRead FM7RawDiskRead::LoadRawRead(std::istream &ifp)
{
	RawRead rawRead;
	std::string str;
	while(std::getline(ifp,str))
	{
		if(true==FM7Lib::StrStartsWith(str,"++++END_RAWREAD"))
		{
			break;
		}

		str=ShortFormat(str);
		auto ptr=FM7Lib::StrSkip(str.c_str(),"SIZE:");
		if(nullptr!=ptr)
		{
			rawRead.mustBeSize=FM7Lib::Atoi(ptr);
			continue;
		}
		ptr=FM7Lib::StrSkip(str.c_str(),"CHECKSUM:");
		if(nullptr!=ptr)
		{
			rawRead.mustBeCheckSum=FM7Lib::Atoi(ptr);
			continue;
		}
		ptr=FM7Lib::StrSkip(str.c_str(),"XOR:");
		if(nullptr!=ptr)
		{
			rawRead.mustBeXor=FM7Lib::Atoi(ptr);
			continue;
		}

		for(int i=0; i+1<str.size(); i+=2)
		{
			char w[4]={'0','0','0',0};
			w[1]=str[i];
			w[2]=str[i+1];
			rawRead.dat.push_back(FM7Lib::Xtoi(w));
		}
	}
	return rawRead;
}
/* static */ FM7RawDiskRead::SectorInfo FM7RawDiskRead::LoadSectorInfo(std::istream &ifp)
{
	SectorInfo secInfo;
	std::string str;
	while(std::getline(ifp,str))
	{
		if(true==FM7Lib::StrStartsWith(str,"++++END_SECTORINFO"))
		{
			break;
		}

		str=ShortFormat(str);
		auto ptr=FM7Lib::StrSkip(str.c_str(),"SIZE:");
		if(nullptr!=ptr)
		{
			secInfo.mustBeSize=FM7Lib::Atoi(FM7Lib::StrSkipSpace(ptr));
			continue;
		}
		if(10<=str.size())
		{
			char w[4]={'0','0','0',0};
			w[1]=str[0];
			w[2]=str[1];
			secInfo.dat.push_back(FM7Lib::Xtoi(w));

			w[1]=str[2];
			w[2]=str[3];
			secInfo.dat.push_back(FM7Lib::Xtoi(w));

			w[1]=str[4];
			w[2]=str[5];
			secInfo.dat.push_back(FM7Lib::Xtoi(w));

			w[1]=str[6];
			w[2]=str[7];
			secInfo.dat.push_back(FM7Lib::Xtoi(w));

			w[1]=str[8];
			w[2]=str[9];
			secInfo.dat.push_back(FM7Lib::Xtoi(w));
		}
	}
	return secInfo;
}
/* static */ FM7RawDiskRead::SectorDump FM7RawDiskRead::LoadSectorDump(std::istream &ifp)
{
	SectorDump secDump;
	std::string str;
	while(std::getline(ifp,str))
	{
		if(true==FM7Lib::StrStartsWith(str,"++++END_SECTORDUMP"))
		{
			break;
		}

		str=ShortFormat(str);
		auto ptr=FM7Lib::StrSkip(str.c_str(),"SIZE:");
		if(nullptr!=ptr)
		{
			secDump.mustBeSize=FM7Lib::Atoi(FM7Lib::StrSkipSpace(ptr));
			continue;
		}
		ptr=FM7Lib::StrSkip(str.c_str(),"CHECKSUM:");
		if(nullptr!=ptr)
		{
			secDump.mustBeCheckSum=FM7Lib::Atoi(FM7Lib::StrSkipSpace(ptr));
			continue;
		}
		ptr=FM7Lib::StrSkip(str.c_str(),"XOR:");
		if(nullptr!=ptr)
		{
			secDump.mustBeXor=FM7Lib::Atoi(FM7Lib::StrSkipSpace(ptr));
			continue;
		}
		ptr=FM7Lib::StrSkip(str.c_str(),"VER:");
		if(nullptr!=ptr)
		{
			secDump.version=FM7Lib::Atoi(FM7Lib::StrSkipSpace(ptr));
			continue;
		}
		for(int i=0; i+1<str.size(); i+=2)
		{
			char w[4]={'0','0','0',0};
			w[1]=str[i];
			w[2]=str[i+1];
			secDump.dat.push_back(FM7Lib::Xtoi(w));
		}
	}
	return secDump;
}

/* static */ std::string FM7RawDiskRead::ShortFormat(const std::string &str)
{
	auto ptr=FM7Lib::StrSkip(str.c_str(),"RAW_READ_SIZE:");
	if(nullptr!=ptr)
	{
		std::string newStr="SIZE:";
		newStr=newStr+ptr;
		return newStr;
	}
	ptr=FM7Lib::StrSkip(str.c_str(),"RAW_READ_CHECKSUM:");
	if(nullptr!=ptr)
	{
		std::string newStr="CHECKSUM:";
		newStr=newStr+ptr;
		return newStr;
	}
	ptr=FM7Lib::StrSkip(str.c_str(),"RAW_READ_XOR:");
	if(nullptr!=ptr)
	{
		std::string newStr="XOR:";
		newStr=newStr+ptr;
		return newStr;
	}
	ptr=FM7Lib::StrSkip(str.c_str(),"SEC_DUMP_SIZE:");
	if(nullptr!=ptr)
	{
		std::string newStr="SIZE:";
		newStr=newStr+ptr;
		return newStr;
	}
	ptr=FM7Lib::StrSkip(str.c_str(),"SEC_DUMP_CHECKSUM:");
	if(nullptr!=ptr)
	{
		std::string newStr="CHECKSUM:";
		newStr=newStr+ptr;
		return newStr;
	}
	ptr=FM7Lib::StrSkip(str.c_str(),"SEC_DUMP_XOR:");
	if(nullptr!=ptr)
	{
		std::string newStr="XOR:";
		newStr=newStr+ptr;
		return newStr;
	}
	ptr=FM7Lib::StrSkip(str.c_str(),"FMT_INFO_SIZE:");
	if(nullptr!=ptr)
	{
		std::string newStr="SIZE:";
		newStr=newStr+ptr;
		return newStr;
	}
	return str;
}

