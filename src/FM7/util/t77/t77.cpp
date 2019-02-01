#include "t77.h"
#include "../lib/cpplib.h"
#include "../lib/fm7lib.h"


void T77File::CleanUp(void)
{
	t77.clear();
}

void T77File::DumpT77(const std::vector <unsigned char> &dat)
{
	t77=dat;
}

void T77File::DumpT77(std::vector <unsigned char> &&dat)
{
	std::swap(t77,dat);
}

T77File::WAVOutOption::WAVOutOption()
{
	Hz=44100;
	levelLow=0;
	levelHigh=255;

	firstSilence=2000;
	silenceAfterFile=4000;
}

T77File::WAVOutDataBuffer::WAVOutDataBuffer()
{
	CleanUp();
}

void T77File::WAVOutDataBuffer::CleanUp(void)
{
	fp=nullptr;
	totalLength=0;
	bufUsed=0;
	buf.resize(1024*1024);
}

bool T77File::WAVOutDataBuffer::Pump(unsigned char dat)
{
	buf[bufUsed]=dat;
	++bufUsed;
	++totalLength;
	if(buf.size()==bufUsed)
	{
		return Flush();
	}
	return true;
}
void T77File::WAVOutDataBuffer::Rewind(void)
{
	bufUsed=0;
}
bool T77File::WAVOutDataBuffer::Flush(void)
{
	bool res=true;
	if(nullptr!=fp && 0<bufUsed)
	{
		auto nWritten=fwrite(buf.data(),1,bufUsed,fp);
		if(nWritten!=buf.size())
		{
			res=false;
		}
	}
	Rewind();
	return res;
}

bool T77File::WriteWav(const char fName[],const WAVOutOption &opt) const
{
	FILE *fp=fopen(fName,"wb");
	if(nullptr!=fp)
	{
		auto res=WriteWav(fp,opt);
		fclose(fp);
		return res;
	}
	return false;
}

bool T77File::WriteWav(FILE *fp,const WAVOutOption &opt) const
{
	long long int ptr=16;
	WAVOutDataBuffer dataBuf;
	dataBuf.fp=nullptr;
	auto dataLen=WriteData(dataBuf,ptr,opt);  // Tentatively calculates the length.

	printf("Data Length=%lld\n",dataLen);

	// WAV Header
	fwrite("RIFF",1,4,fp);

	// File size (will be dataLen+32 bytes)
	unsigned char bin[4];
	FM7Lib::UIntToIntelByteOrder(bin,(unsigned)dataLen+0x20);
	fwrite(bin,1,4,fp);

	// File ID
	fwrite("WAVEfmt ",1,8,fp);

	// Header Size=16 bytes (2 bytes)
	FM7Lib::UIntToIntelByteOrder(bin,0x10);
	fwrite(bin,1,4,fp);

	// Format Tag (2 bytes)
	FM7Lib::UShortToIntelByteOrder(bin,1);
	fwrite(bin,1,2,fp);

	// # of Channels (2 bytes)
	FM7Lib::UShortToIntelByteOrder(bin,1);
	fwrite(bin,1,2,fp);

	// # samples per sec (4 bytes)
	FM7Lib::UIntToIntelByteOrder(bin,opt.Hz);
	fwrite(bin,1,4,fp);

	// # byes per sec (4 bytes)
	FM7Lib::UIntToIntelByteOrder(bin,opt.Hz);
	fwrite(bin,1,4,fp);

	// # block align? (2 bytes)
	FM7Lib::UShortToIntelByteOrder(bin,1);
	fwrite(bin,1,2,fp);

	// # bits per sample (2 bytes)
	FM7Lib::UShortToIntelByteOrder(bin,8);
	fwrite(bin,1,2,fp);



	ptr=16;
	dataBuf.CleanUp();
	dataBuf.fp=fp;

	fwrite("data",1,4,fp);

	FM7Lib::UIntToIntelByteOrder(bin,(unsigned)dataLen);
	fwrite(bin,1,4,fp);

	auto dataLenActual=WriteData(dataBuf,ptr,opt);  // Tentatively calculates the length.
	if(dataLenActual!=dataLen)
	{
		fprintf(stderr,"Temporary-write and actually-write wrote different length.\n");
		fprintf(stderr,"Something went wrong.\n");
		return false;
	}

	return true;
}

long long int T77File::WriteData(WAVOutDataBuffer &dataBuf,long long int &ptr,const WAVOutOption &opt) const
{
	bool firstSilence=true;
	while(ptr+2<=(long long)t77.size())
	{
		if(0x7f==t77[ptr] && 0xff==t77[ptr+1])
		{
			long long int count;
			if(true==firstSilence)
			{
				count=CalculateNumSampleForMillisecond(opt.firstSilence,opt);
			}
			else
			{
				count=CalculateNumSampleForMillisecond(opt.silenceAfterFile,opt);
			}
			for(int i=0; i<count; ++i)
			{
				dataBuf.Pump(0x80);
			}
			firstSilence=false;
		}
		else if(0!=t77[ptr+1])
		{
			unsigned int level=0x80;
			if(t77[ptr]<0x40)
			{
				level=opt.levelLow;
			}
			else
			{
				level=opt.levelHigh;
			}
			// Output from T772WAV writes (10 0x40s then 10 0xC0s) or (19 0x40s then 19 0xC0s) for 44100Hz.
			//   0x1A (26) -> 10count     44100*t=10  t=0.000227[sec]   0.23ms
			//   0x30 (48) -> 19count     44100*t=19  t=0.000431[sec]   0.43ms
			// 44100*c*26=10  -> c=10/(44100*26) -> c=10/(44100*26)=0.385/44100=385/44100000
			// 44100*c*48=19  -> c=19/(44100*48) -> c=19/(44100*48)=0.396/44100=396/44100000=99/11025000

			// Count=Hz*t77lng*c=Hz*t77lng*99/11025000

			long long int t77Lng=t77[ptr+1];
			long long int count=opt.Hz*t77Lng*99/11025000;
			for(int i=0; i<count; ++i)
			{
				dataBuf.Pump(level);
			}
		}
		ptr+=2;
	}
	dataBuf.Flush();
	return dataBuf.totalLength;
}

long long int T77File::CalculateNumSampleForMillisecond(long long int milliSec,const WAVOutOption &opt) const
{
	return milliSec*opt.Hz/1000;
}

////////////////////////////////////////////////////////////


T77Encoder::T77Encoder()
{
	CleanUp();
}
T77Encoder::~T77Encoder()
{
	CleanUp();
}
void T77Encoder::CleanUp(void)
{
	T77File::CleanUp();
	debug_makeIntentionalCheckSumError_headerBlock=false;
	debug_makeIntentionalCheckSumError_dataBlock=false;
	debug_mixNonFFinDataGap=false;
}

unsigned char T77Encoder::CalculateCheckSum(const std::vector <unsigned char> &blk)
{
	unsigned int sum=0;
	for(int i=2; i<blk.size()-1; ++i)
	{
		sum+=blk[i];
	}
	return sum&0xff;
}

std::vector <unsigned char> T77Encoder::MakeHeaderBlock(const char t77fName[],int fType)
{
	std::vector <unsigned char> blk;
	blk.push_back(0x01);
	blk.push_back(0x3c);
	blk.push_back(0x00); // 0 for header block.
	blk.push_back(0x14); // Always 20 bytes for header block.

	unsigned char header[20];
	for(int i=0; i<8; ++i)
	{
		header[i]=' ';
	}
	for(int i=0; i<8 && 0!=t77fName[i]; ++i)
	{
		header[i]=t77fName[i];
	}

	// ASCII format BASIC 0x00 0xff 0xff
	if(FM7File::FTYPE_BASIC_ASCII==fType)
	{
		header[8]=0x00;
		header[9]=0xff;
		header[10]=0xff;
	}
	else if(FM7File::FTYPE_BASIC_BINARY==fType)
	{
		header[8]=0x00;
		header[9]=0x00;
		header[10]=0x00;
	}
	if(FM7File::FTYPE_DATA_ASCII==fType)
	{
		header[8]=0x01;
		header[9]=0xff;
		header[10]=0xff;
	}
	else if(FM7File::FTYPE_BINARY==fType)
	{
		header[8]=0x02;
		header[9]=0x00;
		header[10]=0x00;
	}
	for(int i=11; i<20; ++i)
	{
		header[i]=0;
	}
	for(auto h : header)
	{
		blk.push_back(h);
	}
	blk.push_back(0); // Tentative.
	blk.back()=CalculateCheckSum(blk);

	if(true==debug_makeIntentionalCheckSumError_headerBlock)
	{
		blk.back()=(blk.back()+0x80)&0xff;
	}
	return blk;
}

std::vector <unsigned char> T77Encoder::MakeDataBlock(const std::vector <unsigned char> &dat,long long int ptr,long long int nByte)
{
	std::vector <unsigned char> blk;
	blk.push_back(0x01);
	blk.push_back(0x3c);
	blk.push_back(0x01); // 1 for data block.
	blk.push_back((unsigned char)nByte);
	for(int i=0; i<nByte; ++i)
	{
		blk.push_back(dat[ptr+i]);
	}
	blk.push_back(0); // Tentative
	blk.back()=CalculateCheckSum(blk);

	if(true==debug_makeIntentionalCheckSumError_dataBlock)
	{
		blk.back()=(blk.back()+0x80)&0xff;
	}
	return blk;
}

std::vector <unsigned char> T77Encoder::MakeTerminalBlock(void)
{
	std::vector <unsigned char> blk;
	blk.push_back(0x01);
	blk.push_back(0x3c);
	blk.push_back(0xff);
	blk.push_back(0);
	blk.push_back(0xff);
	return blk;
}

void T77Encoder::AddBit(unsigned int bitData)
{
	if(0==bitData)
	{
		t77.push_back(0x80);
		t77.push_back(0x1a);
		t77.push_back(0x00);
		t77.push_back(0x1a);
	}
	else
	{
		t77.push_back(0x80);
		t77.push_back(0x30);
		t77.push_back(0x00);
		t77.push_back(0x30);
	}
}

void T77Encoder::AddByte(unsigned char byteData)
{
	AddBit(0);

	unsigned char andPtn=1;
	for(int i=0; i<8; ++i)
	{
		AddBit((byteData&andPtn));
		andPtn<<=1;
	}

	AddBit(1);
	AddBit(1);
}

void T77Encoder::StartT77Header(void)
{
	for(int i=0; i<14; ++i)
	{
		const char *s="XM7 TAPE IMAGE";
		t77.push_back(s[i]);
	}
	t77.push_back(0x20);
	t77.push_back(0x30);
	t77.push_back(0);
	t77.push_back(0);
	t77.push_back(0x7f);
	t77.push_back(0xff);
}

void T77Encoder::AddGapBetweenFile(void)
{
	t77.push_back(0);
	t77.push_back(0);
	t77.push_back(0x7f);
	t77.push_back(0xff);
}

bool T77Encoder::Encode(int fType,const char t77fName[],const std::vector <unsigned char> &dat)
{
	if(0==t77.size())
	{
		StartT77Header();
	}
	t77.push_back(0);
	t77.push_back(0);

	t77.push_back(0x7f);
	t77.push_back(0xff);

	for(int i=0; i<256; ++i)
	{
		AddByte(0xff);
	}

	for(auto b : MakeHeaderBlock(t77fName,fType))
	{
		AddByte(b);
	}

	for(int i=0; i<259; ++i)
	{
		AddByte(0xff);
	}


	{
		long long int ptr=0;
		while(ptr<(long long int)dat.size())
		{
			auto nByteLeft=dat.size()-ptr;
			auto nByteWrite=(255<nByteLeft ? 255 : nByteLeft);
			for(auto b : MakeDataBlock(dat,ptr,nByteWrite))
			{
				AddByte(b);
			}
			ptr+=nByteWrite;

			if(FM7File::FTYPE_BASIC_BINARY==fType ||
			   FM7File::FTYPE_DATA_BINARY==fType ||
			   FM7File::FTYPE_BINARY==fType)
			{
				for(int i=0; i<20; ++i)
				{
					if(10==i && true==debug_mixNonFFinDataGap)
					{
						AddByte(0x77);
					}
					else
					{
						AddByte(0xff);
					}
				}
			}
			else
			{
				// Probably, ASCII file needs some time to interpret.
				// Therefore it puts a bigger gap between blocks.
				for(int i=0; i<259; ++i)
				{
					AddByte(0xff);
				}
			}
		}
	}

	for(auto b : MakeTerminalBlock())
	{
		AddByte(b);
	}
	for(int i=0; i<4; ++i)
	{
		AddByte(0xff);
	}

	return true;
}

bool T77Encoder::Encode(int fType,const char t77fName[],const FM7BinaryFile &dat)
{
	std::vector <unsigned char> rawData;
	rawData.push_back(0);

	rawData.push_back((dat.dat.size()&0xff00)>>8);
	rawData.push_back( dat.dat.size()&0xff);

	rawData.push_back((dat.storeAddr&0xff00)>>8);
	rawData.push_back( dat.storeAddr&0xff);

	rawData.insert(rawData.end(),dat.dat.begin(),dat.dat.end());

	rawData.push_back(0xff);
	rawData.push_back(0);
	rawData.push_back(0);

	rawData.push_back((dat.execAddr&0xff00)>>8);
	rawData.push_back( dat.execAddr&0xff);

	// Why not 0x1A?  Disk files end with 0x1A.  I don't know.

	return Encode(fType,t77fName,rawData);
}

bool T77Encoder::EncodeFromFMFile(const char c_t77fName[],const std::vector <unsigned char> &dat)
{
	if(dat.size()<16)
	{
		return false;
	}

	std::string t77fName;

	if(nullptr!=c_t77fName && 0!=c_t77fName[0])
	{
		t77fName=c_t77fName;
	}
	else
	{
		t77fName=FM7Lib::GetFileNameFromFMFile(dat);
		printf("Taking T77 file name from the input file.\n");
		printf("  %s\n",t77fName.c_str());
	}

	const int fType=FM7File::DecodeFMHeaderFileType(dat[0x0a],dat[0x0b]);

	if(FM7File::FTYPE_BINARY==fType)
	{
		FM7BinaryFile binFile;
		if(true==binFile.Decode2B0(dat))
		{
			return Encode(FM7File::FTYPE_BINARY,t77fName.c_str(),binFile.MakeByteArrayForT77());
		}
	}
	else if(FM7File::FTYPE_UNKNOWN!=fType)
	{
		long long int lastByte=dat.size();
		--lastByte;
		while(0<lastByte && 0==dat[lastByte])
		{
			--lastByte;
		}
		if(0x1a==dat[lastByte])
		{
			--lastByte;  // No 0x1a termination in T77
		}

		std::vector <unsigned char> toWrite;
		for(long long int x=16; x<=lastByte; ++x)
		{
			toWrite.push_back(dat[x]);
		}

		return Encode(fType,t77fName.c_str(),toWrite);
	}
	return false;
}

////////////////////////////////////////////////////////////


T77Decoder::T77Decoder()
{
	// Short 0x1A+0x1A=0x34
	// Long  0x30+0x30=0x60
	OneZeroThr=0x48;
}

long long int T77Decoder::SkipLead(long long int ptr) const
{
	int state=0;  // 0:Not yet in the lead.  1:In the lead.
	int longCtr=0;
	int leadCtr=0;
	while(ptr+4<(long long)t77.size())
	{
		if(0==state)
		{
			if(0x40<t77[ptr] && t77[ptr+2]<0x40)
			{
				if((unsigned int)t77[ptr+1]+(unsigned int)t77[ptr+3]<OneZeroThr)
				{
					longCtr=0;
				}
				else
				{
					++longCtr;
					if(10==longCtr)
					{
						state=1;
						longCtr=0;
						leadCtr=1;
					}
				}
				ptr+=4;
			}
			else
			{
				ptr+=2;
			}
		}
		else
		{
			if(0x40<t77[ptr] && t77[ptr+2]<0x40)
			{
				if((unsigned int)t77[ptr+1]+(unsigned int)t77[ptr+3]<OneZeroThr)
				{
					for(int i=0; i<10; ++i)
					{
						if((unsigned)t77[ptr+4+i*4+1]+(unsigned)t77[ptr+4+i*4+3]<OneZeroThr)
						{
							printf("%d repeats.\n",leadCtr);
							return ptr;
						}
					}
				}
				ptr+=44;
				leadCtr++;
			}
			else
			{
				ptr+=2;
			}
		}
	}
	return ptr;
}

int T77Decoder::GetBit(long long int &ptr) const
{
	unsigned int nLow=0,nHigh=0;
	while(ptr+2<(long long)t77.size() && (0==nLow || 0==nHigh))
	{
		if(t77[ptr]<0x40)
		{
			nLow+=t77[ptr+1];
		}
		else
		{
			nHigh+=t77[ptr+1];
		}
		ptr+=2;
	}
	if(OneZeroThr<nLow+nHigh)
	{
		return 1;
	}
	return 0;
}

bool T77Decoder::FindFirstFFFFFF(long long int &ptr) const
{
	int bit[33];

	while(ptr<(long long)t77.size())
	{
		auto searchPtr=ptr;

		for(int i=0; i<33; ++i)
		{
			bit[i]=GetBit(searchPtr);
		}

		// for(int i=0; i<33; ++i)
		// {
		// 	printf("%d ",bit[i]);
		// }
		// printf("\n");
		//printf(">");
		//getchar();


		bool isFf=false;
		if(0==bit[0] && 0==bit[11] && 0==bit[22])
		{
			for(int i=1; i<33; ++i)
			{
				if(0!=(i%11) && 1!=bit[i])
				{
					goto NEXTPTR;
				}
			}
			return true;
		}
	NEXTPTR:
		ptr+=2;
	}

	return false;
}

bool T77Decoder::Decode(void)
{
	long long int ptr=0x10;  // Skip 16-byte header.
	long long int dumpStart;
	std::vector <unsigned char> dump;

	int state=0; // 0 Outside   1 Inside   2 Following the end-block
	bool passed013c=false;

	unsigned char lastBlkIdent[4],lastFive[5];
	long long int blkSize=0;

	filePtr.clear();
	fileDump.clear();

	while(ptr+4<(long long)t77.size())
	{
		if(0==state)
		{
			if(true==FindFirstFFFFFF(ptr))
			{
				printf("Found FFFFFF at 0x%08x\n",(int)ptr);
				state=1;
				dumpStart=ptr;
				dump.clear();
				lastFive[0]=0;
				lastFive[1]=0;
				lastFive[2]=0;
				lastFive[3]=0;
				lastFive[4]=0;
				lastBlkIdent[0]=0;
				lastBlkIdent[1]=0;
				lastBlkIdent[2]=0;
				lastBlkIdent[3]=0;
				passed013c=false;
			}
			else
			{
				break;
			}
		}
		else if(1==state)
		{
			auto ptr0=ptr;
			int bit[11];
			for(int i=0; i<11; ++i)
			{
				bit[i]=GetBit(ptr);
			}

			if(0!=bit[0] || 0==bit[9] || 0==bit[10])
			{
				if(true!=passed013c)
				{
					if(true==FindFirstFFFFFF(ptr))
					{
						printf("Re-synced FFFFFF\n");
						for(int i=0; i<11; ++i)
						{
							bit[i]=GetBit(ptr);
						}
					}
				}
			}
			// Try again?
			if(0!=bit[0] || 0==bit[9] || 0==bit[10])
			{
				printf("Device I/O Error at ptr=%d (0x%08x)\n",(int)ptr0,(int)ptr0);
				filePtr.push_back(dumpStart);
				fileDump.push_back((std::vector <unsigned char> &&)dump);
				dump.clear();
				state=0;
				continue;
			}


			unsigned int add=1;
			unsigned int sum=0;
			for(int i=1; i<9; ++i)
			{
				if(0!=bit[i])
				{
					sum+=add;
				}
				add<<=1;
			}

			dump.push_back(sum);

			lastFive[0]=lastFive[1];
			lastFive[1]=lastFive[2];
			lastFive[2]=lastFive[3];
			lastFive[3]=lastFive[4];
			lastFive[4]=sum;
			if(true!=passed013c)
			{
				if(0xff==lastFive[0] && 0x01==lastFive[1] && 0x3c==lastFive[2])
				{
					passed013c=true;
					lastBlkIdent[0]=lastFive[1];
					lastBlkIdent[1]=lastFive[2];
					lastBlkIdent[2]=lastFive[3];
					lastBlkIdent[3]=lastFive[4];

					auto blkType=lastBlkIdent[2];
					blkSize=lastBlkIdent[3];
					if(0xff==blkType)
					{
						printf("Last block detected.\n");
						state=2;
					}
				}
			}
			else
			{
				--blkSize;
				if(0==blkSize)
				{
					passed013c=false;
				}
			}
		}
		else
		{
			auto ptr0=ptr;
			int bit[11];
			for(int i=0; i<11; ++i)
			{
				bit[i]=GetBit(ptr);
			}

			if(0!=bit[0] || 0==bit[9] || 0==bit[10])
			{
				filePtr.push_back(dumpStart);
				fileDump.push_back((std::vector <unsigned char> &&)dump);
				dump.clear();
				printf("End of file.\n");
				state=0;
			}
			else
			{
				unsigned int add=1;
				unsigned int sum=0;
				for(int i=1; i<9; ++i)
				{
					if(0!=bit[i])
					{
						sum+=add;
					}
					add<<=1;
				}

				dump.push_back(sum);
			}
		}
	}

	if(0<dump.size())
	{
		filePtr.push_back(dumpStart);
		fileDump.push_back((std::vector <unsigned char> &&)dump);
	}

	if(0!=state && 2!=state)
	{
		fprintf(stderr,"Warning!  The last file may not be chopped before the end.\n");
	}

	return true;
}

/* static */ long long int T77Decoder::GetHeaderBlockPointer(const std::vector <unsigned char> &dump)
{
	for(int i=0; i<=dump.size()-0x20; ++i)
	{
		if(0x01==dump[i] && 0x3c==dump[i+1] && 0x00==dump[i+2])
		{
			return i;
		}
	}
	return -1;
}

/* static */ std::string T77Decoder::GetDumpFileName(const std::vector <unsigned char> &dump)
{
	std::string fName;
	auto ptr=GetHeaderBlockPointer(dump);
	if(0<=ptr)
	{
		auto header=dump.data()+ptr+4;
		for(int j=0; j<8 && 0!=header[j]; ++j)
		{
			fName.push_back(header[j]);
		}
	}
	while(0<fName.size() && ' '==fName.back())
	{
		fName.pop_back();
	}

	return fName;
}
/* static */ std::vector <unsigned char> T77Decoder::DumpToFMFormat(const std::vector <unsigned char> &dump)
{
	std::vector <unsigned char> fm;
	auto fName=GetDumpFileName(dump);
	auto headerBlockPtr=GetHeaderBlockPointer(dump);
	if(0<=headerBlockPtr)
	{
		auto header=dump.data()+headerBlockPtr+4; // +4 for skippking 0x01 0x3c type size
		auto fileType=FM7File::DecodeFMHeaderFileType(header[8],header[9]);

		fm=FM7Lib::MakeFMFileHeader(fName.c_str(),header[8],header[9]);
		for(auto ptr=headerBlockPtr; ptr+4<=dump.size(); )
		{
			unsigned int blkType=dump[ptr+2];
			unsigned int blkSize=dump[ptr+3];
			if(0xff==blkType || 0==blkSize)
			{
				// End-Block
				break;
			}
			else if(0x01==blkType)
			{
				for(int i=0; i<blkSize; ++i)
				{
					fm.push_back(dump[ptr+4+i]);
				}
			}

			ptr+=blkSize+5;
			while(ptr+4<=dump.size() && (0x01!=dump[ptr] || 0x3c!=dump[ptr+1]))
			{
				++ptr;
			}
		}
		fm.push_back(0x1A);
	}
	return fm;
}



T77Decoder::RawDecodingInfo T77Decoder::BeginRawDecoding(void) const
{
	RawDecodingInfo info;
	info.endOfFile=false;
	info.ptr=0x10;  // Skip 16 byte header
	info.byteData=0;
	info.byteCtr=0;
	return info;
}
T77Decoder::RawDecodingInfo T77Decoder::RawReadByte(RawDecodingInfo info) const
{
	if(true!=info.endOfFile)
	{
		int bit[11];
		for(int i=0; i<11; ++i)
		{
			bit[i]=GetBit(info.ptr);
		}

		if(0!=bit[0] || 0==bit[9] || 0==bit[10])
		{
			if(true==FindFirstFFFFFF(info.ptr))
			{
				for(int i=0; i<11; ++i)
				{
					bit[i]=GetBit(info.ptr);
				}
				info.byteData=0xff;
				++info.byteCtr;
			}
			else
			{
				info.endOfFile=true;
			}
		}
		else
		{
			++info.byteCtr;
			info.byteData=
				 bit[1]+
				(bit[2]<<1)+
				(bit[3]<<2)+
				(bit[4]<<3)+
				(bit[5]<<4)+
				(bit[6]<<5)+
				(bit[7]<<6)+
				(bit[8]<<7);
		}
	}
	return info;
}
