#include <string>
#include <stdio.h>

#include "d77filesystem.h"
#include "../lib/fm7lib.h"



std::string D77FileSystem::FileInfo::FormatInfo(void) const
{
	const char *typeStr;
	switch(fType)
	{
	default:
		typeStr="UNKNOWN";
		break;
	case FM7File::FTYPE_BASIC_BINARY:
		typeStr="BASIC(BIN)";
		break;
	case FM7File::FTYPE_BASIC_ASCII:
		typeStr="BASIC(ASCII)";
		break;
	case FM7File::FTYPE_DATA_BINARY:
		typeStr="DATA(BINARY)";
		break;
	case FM7File::FTYPE_DATA_ASCII:
		typeStr="DATA(ASCII)";
		break;
	case FM7File::FTYPE_BINARY:
		typeStr="BINARY";
		break;
	};

	char buf[256];
	sprintf(buf,"\"%-8s\"  [%-12s]",fName.c_str(),typeStr);
	return buf;
}

////////////////////////////////////////////////////////////

D77FileSystem::D77FileSystem()
{
	CleanUp();
}
D77FileSystem::~D77FileSystem()
{
	CleanUp();
}
void D77FileSystem::CleanUp(void)
{
	FAT.clear();
	directory.clear();
}

/* static */ void D77FileSystem::ClusterToTrackSideSector(int &trk,int &sid,int &sec,int cluster)
{
	// Track 2 Side 0 Sector 1=Cluster 0
	//                Sector 9=Cluster 1
	// Track 2 Side 1 Sector 1=Cluster 2
	//                Sector 9=Cluster 3
	// Track 3 Side 0 Sector 1=Cluster 4
	//                Sector 9=Cluster 5
	// Track 3 Side 1 Sector 1=Cluster 6
	//                Sector 9=Cluster 7
	// Track 4 Side 0 Sector 1=Cluster 8
	// 				:
	// 				:
	// Cluster X-> Track 2+X/4
	//             Side  (X/2)%2
	//             Sector 1+8*(X%2)

	trk=2+cluster/4;
	sid=(cluster/2)%2;
	sec=1+8*(cluster%2);
}

/* static */ D77FileSystem::FileInfo D77FileSystem::ByteDataToFileInfo(const unsigned char dat[32])
{
	// File List
	// +0B  00 -> BASIC
	//		01 -> Data file?
	//      02 -> Machine Language?
	// 
	// +0C  FF->ASCII
	//      00->Binary
	// 
	// +0E  First Cluster
	// 
	// Track 1 Side 0 Sector 4
	// 0000  59 53 46 4C  49 47 48 54  00 00 00 00  FF 00 00 00 YSFLIGHT........
	// 0010  00 00 00 00  00 00 00 00  00 00 00 00  00 00 00 00 ................

	// BASIC Data Files
	// 0000  48 31 20 20  20 20 20 20  00 00 00 01  FF 00 1B 00 XYZ     ........
	// 0010  00 00 00 00  00 00 00 00  00 00 00 00  00 00 00 00 ................

	// There doesn't seem to be any information of actual number of bytes of a binary BASIC file....
	// That's it that's why I need to pad 0x00 until the end of the sector for a BASIC file (ASCII or BINARY)
	// Disk BASIC must be ignoring last 0x00s in the last sector of a BASIC file.
	// If garbage information is there, Disk BASIC read them as a part of the code!


	FileInfo fInfo;
	for(int i=0; i<8 && 0!=dat[i]; ++i)
	{
		if(dat[i]<32 || 255==dat[i])
		{
			fInfo.fName.clear();
			break;
		}
		fInfo.fName.push_back(dat[i]);
	}

	if(0==dat[0])
	{
		fInfo.state=STATE_DELETED;
	}
	else if(0xff==dat[0])
	{
		fInfo.state=STATE_UNUSED;
	}
	else
	{
		fInfo.state=STATE_USED;
	}

	if(0==dat[0x0b]) // BASIC
	{
		if(0==dat[0x0c])
		{
			fInfo.fType=FM7File::FTYPE_BASIC_BINARY;
		}
		else
		{
			fInfo.fType=FM7File::FTYPE_BASIC_ASCII;
		}
	}
	else if(1==dat[0x0b]) // DATA
	{
		if(0==dat[0x0c])
		{
			fInfo.fType=FM7File::FTYPE_DATA_BINARY; // Logical implecation is 0100 is for Binary Data.  But I haven't seen such a combination.
		}
		else
		{
			fInfo.fType=FM7File::FTYPE_DATA_ASCII;
		}
	}
	else
	{
		fInfo.fType=FM7File::FTYPE_BINARY;
	}

	fInfo.firstCluster=dat[0x0e];

	return fInfo;
}

/* static */ void D77FileSystem::FileInfoToByteData(unsigned char dat[32],const FileInfo &fInfo)
{
	if(STATE_DELETED==fInfo.state)
	{
		for(int i=0; i<32; ++i)
		{
			dat[i]=0xff;
		}
		dat[0]=0;
	}
	else if(STATE_UNUSED==fInfo.state)
	{
		for(int i=0; i<32; ++i)
		{
			dat[i]=0xff;
		}
	}
	else
	{
		for(int i=0; i<16; ++i)
		{
			dat[i]=0;
		}

		for(int i=0; i<8; ++i)
		{
			if(i<fInfo.fName.size())
			{
				dat[i]=fInfo.fName[i];
			}
			else
			{
				dat[i]=' ';
			}
		}

		switch(fInfo.fType)
		{
		case FM7File::FTYPE_BASIC_ASCII:
			dat[0x0b]=0;
			dat[0x0c]=0xff;
			break;
		case FM7File::FTYPE_BASIC_BINARY:
			dat[0x0b]=0;
			dat[0x0c]=0;
			break;
		case FM7File::FTYPE_DATA_ASCII:
			dat[0x0b]=1;
			dat[0x0c]=0xff;
			break;
		case FM7File::FTYPE_DATA_BINARY:
			dat[0x0b]=1; // This is a logical implecation.
			dat[0x0c]=0; // But, binary-data file may not exist.
			break;
		case FM7File::FTYPE_BINARY:
			dat[0x0b]=2;
			dat[0x0c]=0;
			break;
		}

		dat[0x0e]=fInfo.firstCluster;

		for(int i=16; i<32; ++i)
		{
			dat[i]=0xff;
		}
	}
}

/* static */ D77FileSystem::FileInfo D77FileSystem::FMFileHeaderToFileInfo(const unsigned char fmHeader[16])
{
	FileInfo fInfo;
	for(int i=0; i<8; ++i)
	{
		fInfo.fName.push_back(fmHeader[i]);
	}
	while(0<fInfo.fName.size() && fInfo.fName.back()==' ')
	{
		fInfo.fName.pop_back();
	}
	fInfo.state=STATE_USED;

	fInfo.fType=FM7File::DecodeFMHeaderFileType(fmHeader[0x0a],fmHeader[0x0b]);
	fInfo.firstCluster=0xff;

	return fInfo;
}

/* static */ void D77FileSystem::MoveToNextSector(int &trk,int &sid,int &sec)
{
	++sec;
	if(16<sec)
	{
		sec=1;
		++sid;
		if(2<=sid)
		{
			sid=0;
			++trk;
		}
	}
}

bool D77FileSystem::ReadDirectory(const D77File::D77Disk &dsk)
{
	// F-BASIC File System FAT at track 1 side 0 sector 1.
	// From the fifth byte.

	// Directory starts from track 1 side 0 sector 4, 
	// and continues all the way to the end of track 1 side 1.

	auto secDat=dsk.ReadSector(1,0,1);
	if(256==secDat.size())
	{
		this->FAT.resize(152);
		for(int i=0; i<152; ++i)
		{
			this->FAT[i]=secDat[5+i];
		}

		int trk=1,sid=0,sec=4;
		this->directory.clear();
		while(trk<2)
		{
			auto secDat=dsk.ReadSector(trk,sid,sec);
			if(256==secDat.size())
			{
				for(int i=0; i<256; i+=32)
				{
					this->directory.push_back(ByteDataToFileInfo(secDat.data()+i));
				}
			}
			else
			{
				CleanUp();
				return false;
			}
			++sec;
			if(16<sec)
			{
				sec=1;
				++sid;
				if(2<=sid)
				{
					sid=0;
					++trk;
				}
			}
		}

		return true;
	}
	return false;
}

bool D77FileSystem::WriteDirectory(D77File::D77Disk &dsk) const
{
	if(true==dsk.IsWriteProtected())
	{
		return false;
	}

	auto secDat=dsk.ReadSector(1,0,1);
	if(256==secDat.size())
	{
		secDat[0]=0;
		// I still cannot find the meaning of the first byte of Track 1 Side 0 Sector 1.
		// It looks to be 0xE5 immediately after formatting, and turns 0x00 when the first file is saved???
		for(int i=0; i<152; ++i)
		{
			secDat[5+i]=this->FAT[i];
		}
		dsk.WriteSector(1,0,1,secDat.size(),secDat.data());

		int trk=1,sid=0,sec=4,fileIdx=0;
		while(trk<2 && fileIdx<152)
		{
			auto secDat=dsk.ReadSector(trk,sid,sec);
			if(256==secDat.size())
			{
				for(int i=0; i<256; i+=32)
				{
					FileInfoToByteData(secDat.data()+i,directory[fileIdx]);
					++fileIdx;
				}
				dsk.WriteSector(trk,sid,sec,secDat.size(),secDat.data());
			}

			++sec;
			if(16<sec)
			{
				sec=1;
				++sid;
				if(2<=sid)
				{
					sid=0;
					++trk;
				}
			}
		}
	}

	return true;
}

int D77FileSystem::FindFile(const D77File::D77Disk &,const char c_fn[]) const
{
	std::string fn(c_fn);
	while(fn.size()<8)
	{
		fn.push_back(' ');
	}

	for(int fileIdx=0; fileIdx<directory.size(); ++fileIdx)
	{
		if(directory[fileIdx].fName==fn)
		return fileIdx;
	}
	return -1;
}

D77FileSystem::FileInfo D77FileSystem::GetFileInfo(const D77File::D77Disk &dsk,int fileIdx) const
{
	if(0<=fileIdx && fileIdx<directory.size())
	{
		return directory[fileIdx];
	}
	FileInfo fInfo;
	fInfo.state=STATE_DELETED;
	return fInfo;
}

int D77FileSystem::GetUnusedFileIndex(const D77File::D77Disk &) const
{
	for(int i=0; i<directory.size(); ++i)
	{
		if(STATE_USED!=directory[i].state)
		{
			return i;
		}
	}
	return -1;
}

int D77FileSystem::GetClusterConnection(const D77File::D77Disk &dsk,int cluster) const
{
	if(0<=cluster && cluster<152)
	{
		return FAT[cluster];
	}
	return 0xff;
}

bool D77FileSystem::Kill(D77File::D77Disk &dsk,const char c_fn[])
{
	return KillN(dsk,FindFile(dsk,c_fn));
}

bool D77FileSystem::KillN(D77File::D77Disk &dsk,int n)
{
	if(true==dsk.IsWriteProtected())
	{
		return false;
	}

	if(0<=n && n<directory.size() && STATE_USED==directory[n].state)
	{
		auto chain=GetClusterChain(dsk,directory[n]);
		for(auto c : chain)
		{
			FAT[c]=0xff;
		}
		directory[n].state=STATE_DELETED;
		WriteDirectory(dsk);
		return true;
	}
	return false;
}

std::vector <int> D77FileSystem::GetFAT(void) const
{
	return FAT;
}

std::vector <unsigned char> D77FileSystem::GetDirDump(const D77File::D77Disk &dsk,int dirIdx) const
{
	// Dir0 is at trk=1,sid=0,sec=4;
	std::vector <unsigned char> dump;
	if(0<=dirIdx && dirIdx<152)
	{
		int trk=1,sid=0,sec=4+dirIdx/8;
		while(16<sec)
		{
			sid++;
			if(2<=sid)
			{
				sid=0;
				trk++;
			}
			sec-=16;
		}

		const int offset=32*(dirIdx%8);
		auto sector=dsk.ReadSector(trk,sid,sec);
		dump.resize(32);
		for(int i=0; i<32; ++i)
		{
			if(offset+i<sector.size())
			{
				dump[i]=sector[offset+i];
			}
		}
	}

	return dump;
}

std::vector <D77FileSystem::FileInfo> D77FileSystem::Files(const D77File::D77Disk &) const
{
	return directory;
}

std::vector <unsigned char> D77FileSystem::ReadCluster(const D77File::D77Disk &dsk,int cluster,int nSector) const
{
	std::vector <unsigned char> dat;
	if(0<=cluster && cluster<152)
	{
		int trk,sid,sec;
		ClusterToTrackSideSector(trk,sid,sec,cluster);
		for(int c=0; c<8 && c<nSector; ++c)
		{
			auto secDat=dsk.ReadSector(trk,sid,sec);
			dat.insert(dat.end(),secDat.begin(),secDat.end());
			MoveToNextSector(trk,sid,sec);
		}
	}
	return dat;
}

std::vector <int> D77FileSystem::FindUnusedCluster(const D77File::D77Disk &,int nCluster) const
{
	std::vector <int> clusterChain;
	for(int c=0; c<FAT.size() && 0<nCluster; ++c)
	{
		if(FAT[c]==0xff)
		{
			clusterChain.push_back(c);
			--nCluster;
		}
	}

	if(0<nCluster) // Not enough space
	{
		clusterChain.clear();
	}
	return clusterChain;
}

std::vector <int> D77FileSystem::GetClusterChain(const D77File::D77Disk &,const FileInfo &fInfo) const
{
	std::vector <int> chain;
	if(fInfo.firstCluster<0 || 152<=fInfo.firstCluster || 0xff==FAT[fInfo.firstCluster])
	{
		return chain;
	}
	chain.push_back(fInfo.firstCluster);
	for(;;)
	{
		auto nextCluster=FAT[chain.back()];
		for(auto c : chain)
		{
			if(c==nextCluster)
			{
				printf("There is a loop in the cluster chain!!!\n");
				chain.clear();
				return chain;
			}
		}

		if(0xc0==(nextCluster&0xf0))
		{
			break;
		}
		chain.push_back(nextCluster);
	}
	return chain;
}

std::vector <unsigned char> D77FileSystem::RawFileRead(const D77File::D77Disk &dsk,const char fName[]) const
{
	return RawFileRead(dsk,FindFile(dsk,fName));
}
std::vector <unsigned char> D77FileSystem::RawFileRead(const D77File::D77Disk &dsk,int fileIdx) const
{
	std::vector <unsigned char> dat;

	if(0<=fileIdx && fileIdx<directory.size())
	{
		auto &fInfo=directory[fileIdx];
		auto chain=GetClusterChain(dsk,fInfo);
		for(int idx=0; idx<chain.size(); ++idx)
		{
			int nSector=8;
			if(idx+1>=chain.size())
			{
				nSector=1+(FAT[chain[idx]]&0x0f);
			}
			auto clusterDat=ReadCluster(dsk,chain[idx],nSector);
			dat.insert(dat.end(),clusterDat.begin(),clusterDat.end());
		}
	}

	return dat;
}

std::vector <int> D77FileSystem::ReserveClusterChain(D77File::D77Disk &dsk,int nCluster,int lastClusterSectorUsage)
{
	if(true==dsk.IsWriteProtected())
	{
		std::vector <int> empty;
		return empty;
	}
	if(8<lastClusterSectorUsage)
	{
		std::vector <int> empty;
		return empty;
	}

	auto clusterChain=FindUnusedCluster(dsk,nCluster);
	if(0<clusterChain.size())
	{
		for(int i=0; i<clusterChain.size(); ++i)
		{
			if(i==clusterChain.size()-1)
			{
				FAT[clusterChain[i]]=0xc0+(lastClusterSectorUsage-1);
			}
			else
			{
				FAT[clusterChain[i]]=clusterChain[i+1];
			}
		}

		return clusterChain;
	}

	std::vector <int> empty;
	return empty;
}

int D77FileSystem::RawFileWrite(D77File::D77Disk &dsk,const std::vector <unsigned char> &dat)
{
	if(true==dsk.IsWriteProtected())
	{
		return -1;
	}

	int nClusterRequired=(int)(dat.size()+0x7ff)/0x800;
	int lastClusterByteUsage=(dat.size()%0x800);
	int lastClusterSectorUsage=(lastClusterByteUsage+255)/256;
	if(0==nClusterRequired)
	{
		return -1;
	}
	auto clusterChain=ReserveClusterChain(dsk,nClusterRequired,lastClusterSectorUsage);
	if(0==clusterChain.size())
	{
		return -1;
	}

	for(auto c : clusterChain)
	{
		ClearCluster(dsk,c);
	}

	long long int ptr=0;
	for(auto c : clusterChain)
	{
		long long int nWritten=WriteCluster(dsk,c,dat.size()-ptr,dat.data()+ptr);
		if(0==nWritten)
		{
			fprintf(stderr,"Warning!  Maybe write failure.\n");
		}
		ptr+=nWritten;
	}

	return clusterChain[0];
}

bool D77FileSystem::SaveFMFile(D77File::D77Disk &dsk,const std::vector <unsigned char> &dat,const char fileInD77[])
{
	if(true==dsk.IsWriteProtected())
	{
		return false;
	}

	auto fileIdx=GetUnusedFileIndex(dsk);
	if(0>fileIdx)
	{
		return false;
	}

	if(dat.size()<16)
	{
		return false;
	}

	auto temporaryFileInfo=FMFileHeaderToFileInfo(dat.data());
	if(nullptr!=fileInD77 && 0!=fileInD77[0])
	{
		temporaryFileInfo.fName.clear();
		for(int i=0; i<8 && 0!=fileInD77[i]; ++i)
		{
			temporaryFileInfo.fName.push_back(fileInD77[i]);
		}
	}
	if(0<=FindFile(dsk,temporaryFileInfo.fName.c_str()))
	{
		return false;
	}


	auto &fInfo=directory[fileIdx];
	fInfo=temporaryFileInfo;

	std::vector <unsigned char> toWrite;
	for(long long int i=16; i<(int)dat.size(); ++i)
	{
		toWrite.push_back(dat[i]);
	}
	fInfo.firstCluster=RawFileWrite(dsk,toWrite);
	if(fInfo.firstCluster<0)
	{
		fInfo.state=STATE_DELETED;
		return false;
	}

	printf("Saved %s\n",fInfo.fName.c_str());
	return true;
}

long long int D77FileSystem::WriteCluster(D77File::D77Disk &dsk,int cluster,long long int nByte,const unsigned char byteData[])
{
	if(true==dsk.IsWriteProtected())
	{
		return 0;
	}

	int trk,sid,sec;
	ClusterToTrackSideSector(trk,sid,sec,cluster);

	long long int ptr=0;
	for(int i=0; i<8 && ptr<nByte; ++i)
	{
		auto secDat=dsk.ReadSector(trk,sid,sec+i);
		for(auto &b : secDat)
		{
			b=0;
		}
		for(int j=0; j<secDat.size() && ptr<nByte; ++j)
		{
			secDat[j]=byteData[ptr];
			++ptr;
		}
		dsk.WriteSector(trk,sid,sec+i,secDat.size(),secDat.data());
	}

	return ptr;
}
int D77FileSystem::ClearCluster(D77File::D77Disk &dsk,int cluster)
{
	if(true==dsk.IsWriteProtected())
	{
		return 0;
	}

	int trk,sid,sec;
	ClusterToTrackSideSector(trk,sid,sec,cluster);

	for(int i=0; i<8; ++i)
	{
		auto secDat=dsk.ReadSector(trk,sid,sec+i);
		for(auto &b : secDat)
		{
			b=0;
		}
		dsk.WriteSector(trk,sid,sec+i,secDat.size(),secDat.data());
	}

	return 0x800;
}

std::vector <unsigned char> D77FileSystem::EncodeFMFile(const D77File::D77Disk &dsk,const char fm7FileName[]) const
{
	return EncodeFMFile(dsk,FindFile(dsk,fm7FileName));
}
std::vector <unsigned char> D77FileSystem::EncodeFMFile(const D77File::D77Disk &dsk,int fileIdx) const
{
	std::vector <unsigned char> dat;

	const auto fInfo=GetFileInfo(dsk,fileIdx);
	const auto rawRead=RawFileRead(dsk,fileIdx);
	if(STATE_USED==fInfo.state)
	{
		if(FM7File::FTYPE_BINARY==fInfo.fType ||
		   FM7File::FTYPE_BASIC_ASCII==fInfo.fType ||
		   FM7File::FTYPE_BASIC_BINARY==fInfo.fType ||
		   FM7File::FTYPE_DATA_ASCII==fInfo.fType)
		{
			unsigned char fmHeader[16]=
			{
				0x20,0x20,0x20,0x20,0x20,0x20,0x20,0x20,
				0,0,0,0,0,'X','M','7'
			};
			for(int i=0; i<8 && i<fInfo.fName.size(); ++i)
			{
				fmHeader[i]=fInfo.fName[i];
			}
			switch(fInfo.fType)
			{
			case FM7File::FTYPE_BINARY:
				fmHeader[0x0a]=2;
				fmHeader[0x0b]=0;
				break;
			case FM7File::FTYPE_BASIC_ASCII:
				fmHeader[0x0a]=0;
				fmHeader[0x0b]=0xff;
				break;
			case FM7File::FTYPE_BASIC_BINARY:
				fmHeader[0x0a]=0;
				fmHeader[0x0b]=0;
				break;
			case FM7File::FTYPE_DATA_ASCII:
				fmHeader[0x0a]=1;
				fmHeader[0x0b]=0xff;
				break;
			}

			for(auto c : fmHeader)
			{
				dat.push_back(c);
			}
			dat.insert(dat.end(),rawRead.begin(),rawRead.end());
		}
	}

	return dat;
}

const char *D77FileSystem::GetFMFileExtensionForFile(const D77File::D77Disk &dsk,const char fm7FileName[]) const
{
	return GetFMFileExtensionForFile(dsk,FindFile(dsk,fm7FileName));
}
const char *D77FileSystem::GetFMFileExtensionForFile(const D77File::D77Disk &dsk,int fileIdx) const
{
	if(0<=fileIdx && fileIdx<directory.size())
	{
		return FM7File::GetDefaultFMFileExtensionForType(directory[fileIdx].fType);
	}
	return ".FM";
}
