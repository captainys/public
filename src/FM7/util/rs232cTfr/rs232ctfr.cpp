#include <stdio.h>
#include <vector>

#include "../lib/fm7lib.h"
#include "../lib/cpplib.h"

const int addrBarrier=0x1A00;

void CommandHelp(void)
{
	printf("Usage: rs232ctfr output.txt <options>\n");
	printf("  Make a text file that can be transmitted to FM-7/77 via RS-232C\n");
	printf("  from a ASCII BASIC file, or binary file, or SREC file.\n");
	printf("Options:\n");
	printf("-h\n");
	printf("-help\n");
	printf("-?\n");
	printf("\tShow this help.\n");
	printf("-offset offset\n");
	printf("\tAdd offset to the store- and exec-addresses.\n");
	printf("-srecwrite srec-file-name filename-in-FM7\n");
	printf("\tMake from a SREC file.\n");
	printf("\tASM6809 v2.9 writes wrong check sum (not taking a complement?).\n");
	printf("\tThis program ignores the check sum to accept SREC from ASM6809 v2.9.\n");
	printf("-save text-file-name filename-in-FM7\n");
	printf("\tMake from a text file as an ASCII-format BASIC file.\n");
	printf("\tThe input file is suppsoed to be a BASIC program.  If it has $ at the beginning,\n");
	printf("\tThe receiver will stop reading the file there.\n");
	printf("-savem binary-file-name filename-in-FM7 startAddr execAddr\n");
	printf("\tMake from a binary file.  Like to save a file you typed from I/O magazine.\n");
	printf("\tUnlike SAVEM command of F-BASIC endAddr is automatically calculated from\n");
	printf("\tthe file size.  Therefore, you only need to give start and exec addresses.\n");
	printf("\tYou can use &H of 0x to specify address in hexa-decimal number.\n");
}

std::vector <std::string> MakeFromBinary(
    const std::vector <unsigned char> &bin,const char filenameInFM7[],int storeAddr,int execAddr)
{
	std::vector <std::string> txt;
	std::string s;

	if(storeAddr<addrBarrier)
	{
		printf("Error!\n");
		printf("Before 0x1A00 is used by the file receiver.\n");
		return txt;
	}

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

std::vector <std::string> MakeFromSRECFile(const char srecFName[],const char filenameInFM7[],int offset)
{
	auto srec=FM7Lib::ReadTextFile(srecFName);
	FM7BinaryFile binDat;
	if(0<srec.size() && true==binDat.DecodeSREC(srec))
	{
		return MakeFromBinary(binDat.dat,filenameInFM7,binDat.storeAddr+offset,binDat.execAddr+offset);
	}
	else
	{
		fprintf(stderr,"Cannot read file: %s\n",srecFName);
	}
	std::vector <std::string> empty;
	return empty;
}

std::vector <std::string> MakeFromBinaryFileAsMachinGo(
    const char binFName[],const char filenameInFM7[],int storeAddr,int execAddr,int offset)
{
	auto bin=FM7Lib::ReadBinaryFile(binFName);
	return MakeFromBinary(bin,filenameInFM7,storeAddr+offset,execAddr+offset);
}

std::vector <std::string> MakeFromTextFileAsBASIC(const char txtFName[],const char filenameInFM7[])
{
	std::vector <std::string> txt;

	auto text=FM7Lib::ReadTextFile(txtFName);
	if(0<text.size())
	{
		std::string s;
		s="F";
		s=s+filenameInFM7;
		txt.push_back(s);

		txt.push_back("A");

		for(auto &s : text)
		{
			txt.push_back(s);
		}

		txt.push_back("QQQ");
	}
	else
	{
		fprintf(stderr,"Cannot read file: %s\n",txtFName);
	}

	return txt;
}

class RS232CConverter
{
public:
	int offset;

	RS232CConverter();
	int PreProcessOption(int ac,char *av[],int ptr);
	int ProcessOption(int ac,char *av[],int ptr);
};

RS232CConverter::RS232CConverter()
{
	offset=0;
}

int RS232CConverter::PreProcessOption(int ac,char *av[],int ptr)
{
	std::string opt=av[ptr];
	FM7Lib::Capitalize(opt);
	printf("Option: %s\n",opt.c_str());
	if("-H"==opt || "-HELP"==opt || "-?"==opt)
	{
		return 1;
	}
	else if("-OFFSET"==opt)
	{
		offset=FM7Lib::Atoi(av[ptr+1]);
		return 2;
	}
	else if("-SRECWRITE"==opt)
	{
		return 3;
	}
	else if("-SAVE"==opt)
	{
		return 3;
	}
	else if("-SAVEM"==opt)
	{
		return 5;
	}
	return -1;
}

int RS232CConverter::ProcessOption(int ac,char *av[],int ptr)
{
	std::string opt=av[ptr];
	FM7Lib::Capitalize(opt);
	printf("Option: %s\n",opt.c_str());
	if("-H"==opt || "-HELP"==opt || "-?"==opt)
	{
		CommandHelp();
		return 1;
	}
	else if("-OFFSET"==opt)
	{
		offset=FM7Lib::Atoi(av[ptr+1]);
		return 2;
	}
	else if("-SRECWRITE"==opt)
	{
		if(ptr+3<=ac)
		{
			auto txt=MakeFromSRECFile(av[ptr+1],av[ptr+2],offset);
			if(0<txt.size())
			{
				FM7Lib::WriteTextFile(av[1],txt);
				return 3;
			}
			goto ERROR_PROCESSING;
		}
		else if(ptr+2<=ac)
		{
			auto base=FM7Lib::GetBaseName(av[ptr+1]);
			base=FM7Lib::RemoveExtension(base.c_str());
			auto txt=MakeFromSRECFile(av[ptr+1],base.c_str(),offset);
			if(0<txt.size())
			{
				FM7Lib::WriteTextFile(av[1],txt);
				return 2;
			}
			goto ERROR_PROCESSING;
		}
		else
		{
			goto TOO_FEW_ARG;
		}
	}
	else if("-SAVE"==opt)
	{
		if(ptr+3<=ac)
		{
			auto txt=MakeFromTextFileAsBASIC(av[ptr+1],av[ptr+2]);
			if(0<txt.size())
			{
				FM7Lib::WriteTextFile(av[1],txt);
				return 3;
			}
			goto ERROR_PROCESSING;
		}
		else if(ptr+2<=ac)
		{
			auto base=FM7Lib::GetBaseName(av[ptr+1]);
			base=FM7Lib::RemoveExtension(base.c_str());
			auto txt=MakeFromTextFileAsBASIC(av[ptr+1],base.c_str());
			if(0<txt.size())
			{
				FM7Lib::WriteTextFile(av[1],txt);
				return 2;
			}
			goto ERROR_PROCESSING;
		}
		else
		{
			goto TOO_FEW_ARG;
		}
	}
	else if("-SAVEM"==opt)
	{
		if(ptr+5<=ac)
		{
			auto txt=MakeFromBinaryFileAsMachinGo(
			    av[ptr+1],av[ptr+2],FM7Lib::Atoi(av[ptr+3]),FM7Lib::Atoi(av[ptr+4]),offset);
			if(0<txt.size())
			{
				FM7Lib::WriteTextFile(av[1],txt);
				return 5;
			}
			goto ERROR_PROCESSING;
		}
		else if(ptr+4<=ac)
		{
			auto base=FM7Lib::GetBaseName(av[ptr+1]);
			base=FM7Lib::RemoveExtension(base.c_str());
			auto txt=MakeFromBinaryFileAsMachinGo(
			    av[ptr+1],base.c_str(),FM7Lib::Atoi(av[ptr+2]),FM7Lib::Atoi(av[ptr+3]),offset);
			if(0<txt.size())
			{
				FM7Lib::WriteTextFile(av[1],txt);
				return 4;
			}
			goto ERROR_PROCESSING;
		}
		else
		{
			goto TOO_FEW_ARG;
		}
	}

	fprintf(stderr,"Unrecognized option %s\n",av[ptr]);
	return -1;

TOO_FEW_ARG:
	fprintf(stderr,"Too few arguments for %s\n",av[ptr]);
	return -1;
ERROR_PROCESSING:
	fprintf(stderr,"Error while processing %s\n",av[ptr]);
	return -1;
}



int main(int ac,char *av[])
{
	if(ac<3)
	{
		CommandHelp();
		return 1;
	}

	RS232CConverter RS232CConverter;

	int ptr=2;
	while(ptr<ac)
	{
		int nUsed=RS232CConverter.PreProcessOption(ac,av,ptr);
		if(nUsed<0)
		{
			fprintf(stderr,"Fatal error. Cannot continue.\n");
			return 1;
		}
		ptr+=nUsed;
	}

	ptr=2;
	while(ptr<ac)
	{
		int nUsed=RS232CConverter.ProcessOption(ac,av,ptr);
		if(nUsed<0)
		{
			fprintf(stderr,"Fatal error. Cannot continue.\n");
			return 1;
		}
		ptr+=nUsed;
	}

	return 0;
}


