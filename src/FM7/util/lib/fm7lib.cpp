#include "fm7lib.h"
#include "cpplib.h"



void FM7File::CleanUp(void)
{
	fName="";
}

/* static */ int FM7File::DecodeFMHeaderFileType(unsigned char byte10,unsigned char byte11)
{
	if(2==byte10 && 0==byte11)
	{
		//printf(".2B0 Binary file.\n");
		return FTYPE_BINARY;
	}
	else if(0==byte10 && 0xff==byte11)
	{
		//printf(".0A0 BASIC (ASCII) file.\n");
		return FTYPE_BASIC_ASCII;
	}
	else if(0==byte10 && 0==byte11)
	{
		//printf(".0B0 BASIC (ASCII) file.\n");
		return FTYPE_BASIC_BINARY;
	}
	else if(1==byte10 && 0xff==byte11)
	{
		//printf(".1A0 ASCII file.\n");
		return FTYPE_DATA_ASCII;
	}
	//printf("Unknown-type file.\n");
	return FTYPE_UNKNOWN;
}

/* static */ const char *FM7File::FileTypeToString(int fType)
{
	switch(fType)
	{
	case FTYPE_BASIC_BINARY:
		return "BASIC_BINARY";
	case FTYPE_BASIC_ASCII:
		return "BASIC_ASCII";
	case FTYPE_BINARY:
		return "BINARY";
	case FTYPE_DATA_BINARY:
		return "DATA_BINARY";
	case FTYPE_DATA_ASCII:
		return "DATA_ASCII";
	}
	return "UNKNOWN";
}

const char *FM7File::GetDefaultFMFileExtensionForType(int fType)
{
	switch(fType)
	{
	default:
		break;
	case FM7File::FTYPE_BASIC_BINARY:
		return ".0B0";
	case FM7File::FTYPE_BASIC_ASCII:
		return ".0A0";
	case FM7File::FTYPE_DATA_BINARY:
		return ".1B0";
	case FM7File::FTYPE_DATA_ASCII:
		return ".1A0";
	case FM7File::FTYPE_BINARY:
		return ".2B0";
	}
	return ".FM";
}




////////////////////////////////////////////////////////////

FM7BinaryFile::FM7BinaryFile()
{
	FM7File::CleanUp();
	CleanUp();
}
FM7BinaryFile::~FM7BinaryFile()
{
	CleanUp();
}
void FM7BinaryFile::CleanUp(void)
{
	storeAddr=0;
	execAddr=0;
	dat.clear();
}

bool FM7BinaryFile::DecodeSREC(const std::vector <std::string> &srec)
{
	SRECDecoder decoder;
	if(true==decoder.Decode(srec))
	{
		this->storeAddr=decoder.storeAddr;
		this->execAddr=decoder.execAddr;
		this->dat=decoder.data();
		return true;
	}
	return false;
}

bool FM7BinaryFile::DecodeSREC(const char *const srec[])
{
	SRECDecoder decoder;
	if(true==decoder.Decode(srec))
	{
		this->storeAddr=decoder.storeAddr;
		this->execAddr=decoder.execAddr;
		this->dat=decoder.data();
		return true;
	}
	return false;
}

bool FM7BinaryFile::Decode2B0(const std::vector <unsigned char> &dat,bool verbose)
{
	if(dat.size()<19)
	{
		// 16byte header + 1byte(0) + 2byte FileSize
		return false;
	}

	if(true==verbose)
	{
		printf("Decoding 2B0\n");
	}

	if(dat[8]!=0 || dat[9]!=0 || dat[10]!=2 || dat[11]!=0)
	{
		if(true==verbose)
		{
			fprintf(stderr,"Warning!  9th,10th,11th, and 12th bytes of 2B0 file are supposed to be 0,0,2,0\n");
			fprintf(stderr,"In this data: 0x%02x 0x%02x 0x%02x 0x%02x\n",dat[4],dat[5],dat[6],dat[7]);
		}
	}


	// 8 bytes		File Name
	// 4 bytes		0 0 2 0
	// 4 bytes		' ' 'X' 'M' '7' ?
	// ----
	// 1 byte		0 ?
	// 2 bytes		File size
	// 2 bytes		Store address
	// n bytes		Binary data
	// 3 bytes		255 0 0
	// 2 bytes		Entry address
	// 1 byte		0x1A

	this->CleanUp();
	for(int i=0; i<8 && i<dat.size() && 0!=dat[i]; ++i)
	{
		fName.push_back(dat[i]);
	}

	const int fileSize=dat[0x11]*256+dat[0x12];
	if(true==verbose)
	{
		printf("File Size=%d (0x%04x) bytes\n",fileSize,fileSize);
	}

	const int storeAddr=dat[0x13]*256+dat[0x14];
	if(true==verbose)
	{
		printf("Store Address=%d (0x%04x)\n",storeAddr,storeAddr);
	}

	const int expectedSize=16+5+fileSize+6;
	int execAddr=storeAddr;
	if(dat.size()==expectedSize-1)
	{
		execAddr=dat[16+5+fileSize+3]*256+dat[16+5+fileSize+4];
		if(true==verbose)
		{
			printf("Warning! Given data size is 1 byte shorter. (Is last 0x1A omitted?)\n");
			printf("Exec Addr=%d (0x%04x)\n",execAddr,execAddr);
		}
	}
	else if(dat.size()<expectedSize-1)
	{
		if(true==verbose)
		{
			printf("Warning! Given data size is shorter than expected.\n");
			printf("         The file may be corrupted.\n");
			printf("         Using the store addr as exec addr.\n");
		}
	}
	else
	{
		execAddr=dat[16+5+fileSize+3]*256+dat[16+5+fileSize+4];
		if(true==verbose)
		{
			printf("Exec Addr=%d (0x%04x)\n",execAddr,execAddr);
		}
	}

	this->storeAddr=storeAddr;
	this->execAddr=execAddr;
	this->dat.resize(fileSize);
	for(auto &c : this->dat)
	{
		c=0;
	}

	for(int i=0; i<fileSize && 16+5+i<dat.size(); ++i)
	{
		this->dat[i]=dat[16+5+i];
	}

	return true;
}

std::vector <unsigned char> FM7BinaryFile::MakeByteArrayForT77(void) const
{
	std::vector <unsigned char> t77;
	t77.push_back(0);

	t77.push_back((dat.size()&0xff00)>>8);
	t77.push_back( dat.size()&0xff);

	t77.push_back((storeAddr&0xff00)>>8);
	t77.push_back( storeAddr&0xff);

	t77.insert(t77.end(),dat.begin(),dat.end());

	t77.push_back(0xff);
	t77.push_back(0);
	t77.push_back(0);

	t77.push_back((execAddr&0xff00)>>8);
	t77.push_back( execAddr&0xff);

	return t77;
}

////////////////////////////////////////////////////////////

SRECDecoder::SRECDecoder()
{
	execAddr=0x7fffffff;
	for(int i=0; i<65536; ++i)
	{
		dat[i]=0;
		used[i]=0;
	}
}

bool SRECDecoder::Decode(const std::vector <std::string> &fileDat)
{
	for(auto &str : fileDat)
	{
		DecodeOneLine(str.c_str());
	}
	return PostProc();
}

bool SRECDecoder::Decode(const char *const fileDat[])
{
	for(int i=0; nullptr!=fileDat[i]; ++i)
	{
		DecodeOneLine(fileDat[i]);
	}
	return PostProc();
}

std::vector <unsigned char> SRECDecoder::data(void) const
{
	std::vector <unsigned char> v;
	for(unsigned int i=storeAddr; i<storeAddr+length; ++i)
	{
		v.push_back(this->dat[i]);
	}
	return v;
}

bool SRECDecoder::PostProc(void)
{
	int endAddr=0;
	for(storeAddr=0; storeAddr<65536; ++storeAddr)
	{
		if(0!=used[storeAddr])
		{
			break;
		}
	}

	if(65536==storeAddr)
	{
		fprintf(stderr,"No data.\n");
		return false;
	}

	for(endAddr=65535; 0<=endAddr; --endAddr)
	{
		if(0!=used[endAddr])
		{
			break;
		}
	}

	length=endAddr-storeAddr+1;

	// Apparently there is a possibility.
	// for(unsigned int i=storeAddr; i<=endAddr; ++i)
	// {
	// 	if(used[i]==0)
	// 	{
	// 		fprintf(stderr,"File not in one piece.\n");
	// 		exit(1);
	// 	}
	// }

	if(65536<=execAddr)
	{
		execAddr=storeAddr;
	}

	return true;
}

bool SRECDecoder::DecodeOneLine(const char str[])
{
	if(0==str[0])
	{
		// Empty line ignore.
		return true;
	}

	if(str[0]!='S' && str[0]!='s')
	{
		return false;
	}
	// Ignore any line that doesn't start with S.

	if('1'==str[1])  // Data
	{
		auto v=GetByteData(str);
		unsigned int addr=v[0]*256+v[1];
		//printf("From %x ",addr);
		for(int i=2; i<v.size()-1; ++i)
		{
			dat[addr]=v[i];
			used[addr]=1;
			++addr;
		}
		//printf("to %x\n",addr-1);
	}
	else if('9'==str[1]) // Entry addr
	{
		auto v=GetByteData(str);
		this->execAddr=v[0]*256+v[1];
	}
	return true;
}

std::vector <unsigned char> SRECDecoder::GetByteData(const char str[]) const
{
	std::vector <unsigned char> dat;
	for(int i=4; str[i]!=0 && str[i+1]!=0; i+=2)
	{
		unsigned int v=FM7Lib::LetterToNum(str[i])*16+FM7Lib::LetterToNum(str[i+1]);
		dat.push_back((unsigned char)(v));
	}
	return dat;
}
