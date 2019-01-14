#include <vector>
#include <string>
#include "../t77/t77.h"
#include "../d77/d77.h"
#include "../d77/d77filesystem.h"
#include "../lib/cpplib.h"

#include "formatdata.h"
#include "formatm.h"
#include "uraram.h"
#include "dskwrite.h"
#include "tommr.h"





std::string MakeT77FileName(const char c_baseFName[],int count)
{
	std::string baseFName=FM7Lib::RemoveExtension(c_baseFName);
	char suffix[32];
	sprintf(suffix,"-%02d.t77",count);
	baseFName.append(suffix);
	return baseFName;
}



class WriteBuffer
{
private:
	bool for77av;

	bool buf1Full;
	std::vector <unsigned char> buf1; // 0x2000-0x6fff (0x5000 bytes)
	bool buf2Full;
	std::vector <unsigned char> buf2; // 0x8000-0xf7ff (0x7800 bytes)

	bool bufExtFull[4];
	std::vector <unsigned char> bufExt[4];
	// Physical 0x00000 to 0x03fff
	//          0x04000 to 0x07fff
	//          0x08000 to 0x0bfff
	//          0x0c000 to 0x0ffff
	// If for77av option is true.

	int buf1Capacity;
	enum
	{
		buf2Capacity=0x7C00,
		bufExtCapacity=0x4000,
	};

	// Use $1800-$6FFF ($5800 bytes) and $8000-$FBFF ($7C00 bytes)
	// Or  $1800-$7FFF ($6800 bytes) and $8000-$FBFF ($7C00 bytes)
	// Format
	// 1 byte Track
	// 1 byte Side
	// 1 byte Sector
	// 1 byte SizeShift (SectorSize=(128<<SizeShift)
	//        SectorDump
	// Next Sector or FFFFFFFF to terminate

public:
	WriteBuffer();
	void SetForFM77AV(void);
	void Use7000to7FFF(void);
	void CleanUp(void);
	bool Pump(int trk,int sid,int sec,const std::vector <unsigned char> &dat);
	bool Flush(const char baseFName[],int counter,bool isLastFile,int driveNumber);

private:
	std::vector <unsigned char> GetUraramPart1(void) const;
	std::vector <unsigned char> GetUraramPart2(void) const;
	std::vector <unsigned char> GetOmotePart(void) const;
	std::vector <unsigned char> GetMMRPart(int i) const;
};

WriteBuffer::WriteBuffer()
{
	CleanUp();
	for77av=false;
	buf1Capacity=0x5800;
}

void WriteBuffer::SetForFM77AV(void)
{
	for77av=true;
}

void WriteBuffer::Use7000to7FFF(void)
{
	buf1Capacity=0x6800;
}

void WriteBuffer::CleanUp(void)
{
	buf1Full=false;
	buf1.clear();
	buf2Full=false;
	buf2.clear();

	for(auto &bf : bufExtFull)
	{
		bf=false;
	}
	for(auto &buf : bufExt)
	{
		buf.clear();
	}
}

bool WriteBuffer::Pump(int trk,int sid,int sec,const std::vector <unsigned char> &secDat)
{
	int sizeShift=0;
	long long int secSize=128;
	while(secSize<(long long int)secDat.size())
	{
		++sizeShift;
		secSize<<=1;
	}



	if(true!=buf1Full)
	{
		if(buf1.size()+secSize+8<buf1Capacity)
		{
			buf1.push_back(trk);
			buf1.push_back(sid);
			buf1.push_back(sec);
			buf1.push_back(sizeShift);
			for(int i=0; i<secSize; ++i)
			{
				if(i<secDat.size())
				{
					buf1.push_back(secDat[i]);
				}
				else
				{
					buf1.push_back(0);
				}
			}
			return true;
		}
		else
		{
			buf1Full=true;
		}
	}

	if(true!=buf2Full)
	{
		if(buf2.size()+secSize+8<buf2Capacity)
		{
			buf2.push_back(trk);
			buf2.push_back(sid);
			buf2.push_back(sec);
			buf2.push_back(sizeShift);
			for(int i=0; i<secSize; ++i)
			{
				if(i<secDat.size())
				{
					buf2.push_back(secDat[i]);
				}
				else
				{
					buf2.push_back(0);
				}
			}
			return true;
		}
		else
		{
			buf2Full=true;
		}
	}

	if(true==for77av)
	{
		for(int i=0; i<4; ++i)
		{
			if(true!=bufExtFull[i])
			{
				auto &buf=bufExt[i];
				if(buf.size()+secSize+8<bufExtCapacity)
				{
					buf.push_back(trk);
					buf.push_back(sid);
					buf.push_back(sec);
					buf.push_back(sizeShift);
					for(int i=0; i<secSize; ++i)
					{
						if(i<secDat.size())
						{
							buf.push_back(secDat[i]);
						}
						else
						{
							buf.push_back(0);
						}
					}
					return true;
				}
				else
				{
					bufExtFull[i]=true;
				}
			}
		}
	}

	return false;
}

bool WriteBuffer::Flush(const char baseFName[],int counter,bool isLastFile,int driveNumber)
{
	buf1.push_back(0xff);
	buf1.push_back(0xff);
	buf1.push_back(0xff);
	buf1.push_back(0xff);
	buf2.push_back(0xff);
	buf2.push_back(0xff);
	buf2.push_back(0xff);
	buf2.push_back(0xff);

	for(auto &buf : bufExt)
	{
		buf.push_back(0xff);
		buf.push_back(0xff);
		buf.push_back(0xff);
		buf.push_back(0xff);
	}


	auto t77Fn=MakeT77FileName(baseFName,counter);
	printf("Saving %s\n",t77Fn.c_str());

	std::vector <std::string> basCode;
	basCode.push_back("100 CLEAR ,&H13FF");
	basCode.push_back("110 DEFINT A-Z");
	{
		std::string titleLine;
		titleLine="120 PRINT \"LOADING ";

		auto nextT77Fn=MakeT77FileName(baseFName,counter);
		auto basename=FM7Lib::GetBaseName(nextT77Fn.c_str());
		auto withoutExt=FM7Lib::RemoveExtension(basename.c_str());
		titleLine.append(withoutExt);

		titleLine.push_back('\"');

		basCode.push_back(titleLine);
	}

	basCode.push_back("130 POKE &HFD1E,&H5F");
	basCode.push_back("140 POKE &HFD1E,&H5A");
	basCode.push_back("150 POKE &HFD1E,&H55");
	basCode.push_back("160 POKE &HFD1E,&H50");

	// Drive 0 Motor ON
	char line150[256];
	sprintf(line150,"170 POKE &HFD1D,&H8%d",driveNumber);
	basCode.push_back(line150);

	// For FM77AV40 and newer, reset disk drive mapping.
	if(true==for77av)
	{
		basCode.push_back("200 PRINT \"TOMMR\"");
		basCode.push_back("210 LOADM \"CAS0:TOMMR\"");
		basCode.push_back("220 FOR ADDR=0 TO 3");
		basCode.push_back("230   PRINT \"MMR\";ADDR");
		basCode.push_back("240   LOADM \"CAS0:MMR\"");
		basCode.push_back("250   POKE &H1402,ADDR*4");
		basCode.push_back("260   EXEC &H1400");
		basCode.push_back("270 NEXT");
		basCode.push_back("280 POKE &HFD93,0");
	}

	basCode.push_back("400 PRINT \"URA1\"");
	basCode.push_back("410 LOADM \"CAS0:URA1\"");
	basCode.push_back("420 PRINT \"URA1TFM\"");
	basCode.push_back("430 LOADM \"CAS0:URA1TFM\",,R");
	basCode.push_back("440 PRINT \"URA2\"");
	basCode.push_back("450 LOADM \"CAS0:URA2\"");
	basCode.push_back("460 PRINT \"URA2TFM\"");
	basCode.push_back("470 LOADM \"CAS0:URA2TFM\",,R");
	basCode.push_back("480 PRINT \"OMOTE\"");
	basCode.push_back("490 LOADM \"CAS0:OMOTE\"");

	basCode.push_back("800 PRINT \"DSKWRITE\"");
	basCode.push_back("810 LOADM \"CAS0:DSKWRITE\"");

	char line8x0[256];
	sprintf(line8x0,"820 POKE &H1404,%d",driveNumber);
	basCode.push_back(line8x0);

	sprintf(line8x0,"830 POKE &H1406,%d",(true!=for77av ? 0 : 1));
	basCode.push_back(line8x0);

	// basCode.push_back("830 EXEC &H1402");  Skip Restore
	basCode.push_back("840 EXEC &H1400");

	basCode.push_back("950 PLAY \"V15O4T110L16CEGO5C\"");
	if(true!=isLastFile)
	{
		std::string lastLine;
		lastLine="999 PRINT \"NEXT: ";

		auto nextT77Fn=MakeT77FileName(baseFName,counter+1);
		auto basename=FM7Lib::GetBaseName(nextT77Fn.c_str());
		auto withoutExt=FM7Lib::RemoveExtension(basename.c_str());
		lastLine.append(withoutExt);

		lastLine.push_back('\"');

		basCode.push_back(lastLine);
	}
	else
	{
		basCode.push_back("999 PRINT \"All done!  Hope everything worked!\"");
	}


	T77Encoder encoder;
	encoder.EncodeFromFMFile("",FM7Lib::TextTo0A0(basCode,"WRITE"));

	if(true==for77av)
	{
		FM7BinaryFile binFile;
		binFile.DecodeSREC(toMMRSREC);
		encoder.EncodeFromFMFile("",FM7Lib::BinaryTo2B0(binFile.dat,"TOMMR",binFile.storeAddr,binFile.execAddr));

		for(int i=0; i<4; ++i)
		{
			encoder.EncodeFromFMFile("",FM7Lib::BinaryTo2B0(GetMMRPart(i),"MMR",0x2000,0x1400));
		}
	}



	encoder.EncodeFromFMFile("",FM7Lib::BinaryTo2B0(GetUraramPart1(),"URA1",0x2000,0x1400));

	{
		FM7BinaryFile binFile;
		binFile.DecodeSREC(ura1SREC);
		encoder.EncodeFromFMFile("",FM7Lib::BinaryTo2B0(binFile.dat,"URA1TFM",binFile.storeAddr,binFile.execAddr));
	}

	encoder.EncodeFromFMFile("",FM7Lib::BinaryTo2B0(GetUraramPart2(),"URA2",0x2000,0x1400));

	{
		FM7BinaryFile binFile;
		binFile.DecodeSREC(ura2SREC);
		encoder.EncodeFromFMFile("",FM7Lib::BinaryTo2B0(binFile.dat,"URA2TFM",binFile.storeAddr,binFile.execAddr));
	}

	encoder.EncodeFromFMFile("",FM7Lib::BinaryTo2B0(GetOmotePart(),"OMOTE",0x1800,0x1400));


	{
		FM7BinaryFile binFile;
		binFile.DecodeSREC(dskWriteSREC);
		encoder.EncodeFromFMFile("",FM7Lib::BinaryTo2B0(binFile.dat,"DSKWRITE",binFile.storeAddr,binFile.execAddr));
	}


	if(true!=FM7Lib::WriteBinaryFile(t77Fn.c_str(),encoder.t77))
	{
		fprintf(stderr,"Cannot open %s for writing.\n",t77Fn.c_str());
		return false;
	}

	auto wavFName=FM7Lib::ChangeExtension(t77Fn.c_str(),".wav");
	T77File::WAVOutOption wavOutOption;
	if(true!=encoder.WriteWav(wavFName.c_str(),wavOutOption))
	{
		fprintf(stderr,"Cannot open %s for writing.\n",wavFName.c_str());
		return false;
	}

	CleanUp();

	return true;
}

std::vector <unsigned char> WriteBuffer::GetUraramPart1(void) const
{
	std::vector <unsigned char> dat;
	for(int i=0; i<buf2.size() && i<0x4000; ++i)
	{
		dat.push_back(buf2[i]);
	}
	while(dat.size()<128)
	{
		dat.push_back(0);
	}
	return dat;
}
std::vector <unsigned char> WriteBuffer::GetUraramPart2(void) const
{
	std::vector <unsigned char> dat;
	for(int i=0x4000; i<buf2.size(); ++i)
	{
		dat.push_back(buf2[i]);
	}
	while(dat.size()<128)
	{
		dat.push_back(0);
	}
	return dat;
}
std::vector <unsigned char> WriteBuffer::GetOmotePart(void) const
{
	return buf1;
}

std::vector <unsigned char> WriteBuffer::GetMMRPart(int i) const
{
	return bufExt[i];
}


////////////////////////////////////////////////////////////





class DiskToTape
{
private:
	bool needFormatter;
	bool needSectorData;

	bool fbasicDisk;
	int driveNumber;
	bool for77av;
	bool use7000to7fff;

	std::vector <D77File::D77Disk::TrackLocation> limitedTrackOnly;

	D77File d77;
public:
	static void CommandHelp(void);

	DiskToTape();
	bool Run(int ac,char *av[]);
	int ProcessOption(int ac,char *av[],int ptr,D77File::D77Disk &disk);

	bool MakeFormatData(D77File::D77Disk &dsk,const char d77FName[]);
	bool MakeSectorData(D77File::D77Disk &dsk,const char d77FName[],int counter);
};

/* static */ void DiskToTape::CommandHelp(void)
{
	printf("Usage: disk2tape D77file.d77 <options>\n");
	printf("  This program will create:\n");
	printf("     D77file-01.t77\n");
	printf("     D77file-02.t77\n");
	printf("     D77file-03.t77\n");
	printf("      :\n");
	printf("\n");
	printf("In real FM-7/77 computers,\n");
	printf("  1. Start in tape mode (no-disk mode)\n");
	printf("  2. Insert a disk on drive 0\n");
	printf("  3. Type RUN \"\" for each t77 file.\n");
	printf("     (Run in the order)\n");
	printf("If the disk is not copy protected, this will\n");
	printf("hopefully create a usable disk.\n");
	printf("\n");
	printf("Options:");
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
	printf("-drive DriveNo\n");
	printf("\tSpecify which drive to write.\n");
	printf("\tIn some situations like drive 0 can read but no longer write,\n");
	printf("\tThis option may be useful.\n");
	printf("-77av\n");
	printf("\tCreate .T77/.WAV files for FM77AV series.\n");
	printf("\tThis option will use MMR so that fewer .WAV files will be created.\n");
	printf("-fm77av\n");
	printf("\tSame as -77av.\n");
	printf("-use7000\n");
	printf("\tUse $7000 to $7fff to store disk image.\n");
	printf("\tIf this option is used, write-back cannot be started from\n");
	printf("\tDisk BASIC.\n");
	printf("-track trk side\n");
	printf("\tMake writer for limited tracks.  -fbasic option will be ignored with this option.\n");
	printf("\tCan specify multiple -track options for multiple tracks.\n");
	printf("\tFormatter will not be generated with this option.\n");
}

DiskToTape::DiskToTape()
{
	needFormatter=true;
	needSectorData=true;

	fbasicDisk=false;
	driveNumber=0;
	for77av=false;
	use7000to7fff=false;
}

bool DiskToTape::Run(int ac,char *av[])
{
	if(2>ac)
	{
		DiskToTape::CommandHelp();
		return 1;
	}



	auto rawD77=FM7Lib::ReadBinaryFile(av[1]);
	if(0==rawD77.size())
	{
		fprintf(stderr,"Cannot read %s\n",av[1]);
		return 1;
	}
	d77.SetData(rawD77);
	if(1!=d77.GetNumDisk())
	{
		fprintf(stderr,"This program does not support multi-disk image: %s\n",av[1]);
		return 1;
	}


	auto diskPtr=d77.GetDisk(0);
	diskPtr->ClearWriteProtected();

	int ptr=2;
	while(ptr<ac)
	{
		int nUsed=ProcessOption(ac,av,ptr,*diskPtr);
		if(nUsed<0)
		{
			fprintf(stderr,"Option error.\n");
			return 1;
		}
		ptr+=nUsed;
	}


	if(true==needFormatter && true!=MakeFormatData(*diskPtr,av[1]))
	{
		fprintf(stderr,"Error while making a formatting program.\n");
		return 1;
	}



	if(true==needSectorData && true!=MakeSectorData(*diskPtr,av[1],2))
	{
		fprintf(stderr,"Error while making a sector-writing program.\n");
		return 1;
	}



	return 0;
}

int DiskToTape::ProcessOption(int ac,char *av[],int ptr,D77File::D77Disk &dsk)
{
	std::string opt=av[ptr];
	FM7Lib::Capitalize(opt);
	if("-DELDUPSEC"==opt)
	{
		for(auto loc : dsk.AllTrack())
		{
			dsk.DeleteDuplicateSector(loc.track,loc.side);
		}
		return 1;
	}
	else if("-RENUMFX"==opt)
	{
		for(auto trkLoc : dsk.AllTrack())
		{
			auto trkPtr=dsk.FindTrack(trkLoc.track,trkLoc.side);
			if(nullptr!=trkPtr)
			{
				for(auto secLoc : trkPtr->AllSector())
				{
					if(0xf5==secLoc.sector ||
					   0xf6==secLoc.sector ||
					   0xf7==secLoc.sector)
					{
						dsk.RenumberSector(trkLoc.track,trkLoc.side,secLoc.sector,secLoc.sector-0x10);
					}
				}
			}
		}
		return 1;
	}
	else if("-FBASIC"==opt)
	{
		fbasicDisk=true;
		return 1;
	}
	else if("-DRIVE"==opt)
	{
		if(ptr+1<ac)
		{
			driveNumber=(atoi(av[ptr+1])&3);
			return 2;
		}
		else
		{
			fprintf(stderr,"Too few arguments for -drive option.\n");
			return -1;
		}
	}
	else if("-77AV"==opt || "-FM77AV"==opt)
	{
		for77av=true;
		return 1;
	}
	else if("-USE7000"==opt)
	{
		use7000to7fff=true;
		return 1;
	}
	else if("-TRACK"==opt)
	{
		if(ptr+2<ac)
		{
			D77File::D77Disk::TrackLocation trkLoc;
			trkLoc.track=atoi(av[ptr+1]);
			trkLoc.side=atoi(av[ptr+2]);
			limitedTrackOnly.push_back(trkLoc);
			needFormatter=false;
			return 3;
		}
		else
		{
			fprintf(stderr,"Too few arguments for -track option.\n");
			return -1;
		}
	}

	fprintf(stderr,"Unrecognized option: %s\n",opt.c_str());

	return -1;
}

bool DiskToTape::MakeFormatData(D77File::D77Disk &dsk,const char d77FName[])
{
	FormatData fom;
	if(true!=fom.MakeFormatData(dsk))
	{
		return false;
	}



	std::vector <std::string> formatBas;
	formatBas.push_back("110 CLEAR ,&H13FF");
	formatBas.push_back("120 WIDTH 40,25");
	formatBas.push_back("130 PRINT \"LOADING FORMATTER\"");
	formatBas.push_back("140 DEFINT A-Z");

	formatBas.push_back("200 REM For FM77AV40 and newer.");
	formatBas.push_back("210 REM Clear disk-mappings");
	formatBas.push_back("220 POKE &HFD1E,&H5F");
	formatBas.push_back("230 POKE &HFD1E,&H5A");
	formatBas.push_back("240 POKE &HFD1E,&H55");
	formatBas.push_back("250 POKE &HFD1E,&H50");
	formatBas.push_back("260 REM Disk Drive 0 Motor ON");

	char line270[256];
	sprintf(line270,"270 POKE &HFD1D,&H8%d",driveNumber);
	formatBas.push_back(line270);

	formatBas.push_back("400 LOADM \"CAS0:FORMATD\"");
	formatBas.push_back("410 LOADM \"CAS0:FORMATM\"");

	char line420[256];
	sprintf(line420,"420 POKE &H1804,%d",driveNumber);
	formatBas.push_back(line420);

	formatBas.push_back("800 EXEC &H1802");  // Restore
	formatBas.push_back("810 WHILE INKEY$<>\"\":WEND");
	formatBas.push_back("820 EXEC &H1800");
	formatBas.push_back("830 FOR I=0 TO 1000:NEXT");
	formatBas.push_back("840 EXEC &H1802");  // Restore

	formatBas.push_back("950 PLAY \"V15O4T110L16CEGO5C\"");
	{
		std::string lastLine;
		lastLine="999 PRINT \"NEXT: ";

		auto nextT77Fn=MakeT77FileName(d77FName,2);
		auto basename=FM7Lib::GetBaseName(nextT77Fn.c_str());
		auto withoutExt=FM7Lib::RemoveExtension(basename.c_str());
		lastLine.append(withoutExt);

		lastLine.push_back('\"');

		formatBas.push_back(lastLine);
	}

	T77Encoder encoder;
	encoder.EncodeFromFMFile("FORMAT",FM7Lib::TextTo0A0(formatBas,"FORMAT"));

	encoder.EncodeFromFMFile("FORMATD",FM7Lib::BinaryTo2B0(fom.formatData,"FORMATD",0x2000,0x1800));

	FM7BinaryFile binFile;
	binFile.DecodeSREC(formatmSREC);
	encoder.EncodeFromFMFile("FORMATM",FM7Lib::BinaryTo2B0(binFile.dat,"FORMATM",binFile.storeAddr,binFile.execAddr));

	auto t77Fn=MakeT77FileName(d77FName,1);
	printf("Saving %s\n",t77Fn.c_str());
	if(true!=FM7Lib::WriteBinaryFile(t77Fn.c_str(),encoder.t77))
	{
		fprintf(stderr,"Cannot open %s for writing.\n",t77Fn.c_str());
		return false;
	}

	auto wavFName=FM7Lib::ChangeExtension(t77Fn.c_str(),".wav");
	T77File::WAVOutOption wavOutOption;
	if(true!=encoder.WriteWav(wavFName.c_str(),wavOutOption))
	{
		fprintf(stderr,"Cannot open %s for writing.\n",wavFName.c_str());
		return false;
	}

	return true;
}

bool DiskToTape::MakeSectorData(D77File::D77Disk &dsk,const char d77FName[],int counter)
{
	std::vector <D77File::D77Disk::D77Track::SectorLocation> targetSector;
	if(0<limitedTrackOnly.size())
	{
		for(auto trkLoc : limitedTrackOnly)
		{
			auto trkPtr=dsk.FindTrack(trkLoc.track,trkLoc.side);
			if(nullptr!=trkPtr)
			{
				for(auto secLoc : trkPtr->AllSector())
				{
					if(secLoc.track!=trkLoc.track || secLoc.side!=trkLoc.side)
					{
						fprintf(stderr,"Sector location inconsistent with track location.\n");
						return false;
					}
					targetSector.push_back(secLoc);
				}
			}
		}
	}
	else if(true!=fbasicDisk)
	{
		for(auto trkLoc : dsk.AllTrack())
		{
			auto trkPtr=dsk.FindTrack(trkLoc.track,trkLoc.side);
			if(nullptr!=trkPtr)
			{
				for(auto secLoc : trkPtr->AllSector())
				{
					if(secLoc.track!=trkLoc.track || secLoc.side!=trkLoc.side)
					{
						fprintf(stderr,"Sector location inconsistent with track location.\n");
						return false;
					}
					targetSector.push_back(secLoc);
				}
			}
		}
	}
	else
	{
		for(int trk=0; trk<2; ++trk)
		{
			for(int side=0; side<2; ++side)
			{
				auto trkPtr=dsk.FindTrack(trk,side);
				if(nullptr!=trkPtr)
				{
					for(auto secLoc : trkPtr->AllSector())
					{
						targetSector.push_back(secLoc);
					}
				}
			}
		}

		D77FileSystem fileSys;
		if(true!=fileSys.ReadDirectory(dsk))
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
				for(int i=0; i<8; ++i)
				{
					D77File::D77Disk::D77Track::SectorLocation secLoc;
					secLoc.track=trk;
					secLoc.side=sid;
					secLoc.sector=sec+i;
					targetSector.push_back(secLoc);
				}
			}
		}
	}



	WriteBuffer writeBuf;
	if(true==for77av)
	{
		writeBuf.SetForFM77AV();
	}
	if(true==use7000to7fff)
	{
		writeBuf.Use7000to7FFF();
	}

	for(auto secLoc : targetSector)
	{
		auto secDat=dsk.ReadSector(secLoc.track,secLoc.side,secLoc.sector);
		if(true!=writeBuf.Pump(secLoc.track,secLoc.side,secLoc.sector,secDat))
		{
			if(true!=writeBuf.Flush(d77FName,counter,false,driveNumber))
			{
				fprintf(stderr,"Something went wrong. Aborting.\n");
				return false;
			}
			++counter;
			writeBuf.Pump(secLoc.track,secLoc.side,secLoc.sector,secDat);
		}
		printf("Trk:%d Sid:%d Sec:%d\n",secLoc.track,secLoc.side,secLoc.sector);
	}

	if(true!=writeBuf.Flush(d77FName,counter,true,driveNumber))
	{
		fprintf(stderr,"Something went wrong. Aborting.\n");
		return false;
	}
	++counter;

	return true;
}




////////////////////////////////////////////////////////////



int main(int ac,char *av[])
{
	DiskToTape disk2tape;
	return disk2tape.Run(ac,av);
}
