#include <stdio.h>
#include <vector>

#include "../lib/fm7lib.h"
#include "../lib/cpplib.h"


void CommandHelp(void)
{
	printf("Usage: mangoToBas <options>\n");
	printf("  Convert binary file to a BASIC file\n");
	printf("  The file will be created when it is run.\n");
	printf("  By the way 'Go' means 'language' in Japanese.\n");
	printf("Options will be interpreted from left to right.\n");
	printf("Options:\n");
	printf("-h\n");
	printf("-help\n");
	printf("-?\n");
	printf("\tShow this help.\n");
	printf("-out filename.txt\n");
	printf("-o filename.txt\n");
	printf("\tSpecify output file name.\n");
	printf("-autoexec\n");
	printf("\tAuto exec machine-go program from BASIC program.\n");
	printf("-dontsaveFM7\n");
	printf("\tDo not SAVEM at the end of the program.  Use -autoexec and -dontsaveFM7 together to make it a\n");
	printf("\tdistributable executable package.\n");
	printf("-forRS232C\n");
	printf("-tMake text file paste-able to a terminal and transmitted to 232CFILE in FM-7 side.\n");
	printf("-srecoffset offset\n");
	printf("\tAdd offset to the binary.\n");
	printf("\tIf BASIC cannot fit in the range, it can offset the address in FM-7\n");
	printf("\tThis option is only valid for SREC file.  Must be before -srecwrite.\n");
	printf("-srecwrite srec-file-name filename-in-FM7\n");
	printf("\tEncode a SREC file.\n");
	printf("\tASM6809 v2.9 writes wrong check sum (not taking a complement?).\n");
	printf("\tThis program ignores the check sum to accept SREC from ASM6809 v2.9.\n");
	printf("-savem binary-file-name filename-in-FM7 startAddr execAddr\n");
	printf("\Encode a binary file to FM7.  Like to save a file you typed from I/O magazine.\n");
	printf("\tUnlike SAVEM command of F-BASIC endAddr is automatically calculated from\n");
	printf("\tthe file size.  Therefore, you only need to give start and exec addresses.\n");
	printf("\tYou can use &H of 0x to specify address in hexa-decimal number.\n");
}

void AddBasicLine(std::vector <std::string> &bas,int &lineNum,const std::string &str)
{
	char s[256];
	sprintf(s,"%d ",lineNum);
	std::string newLine(s);
	newLine=newLine+str;
	bas.push_back((std::string &&)newLine);
	lineNum+=10;
}

std::vector <std::string> BinaryAsMachineGo(const unsigned char *dat,const char filenameInFM7[],int storeAddr,int siz,int execAddr,bool autoExec,bool saveFM7File,bool forRS232C)
{
	std::vector <std::string> bas;
	int lineNum=10;

	if(true==forRS232C)
	{
		bas.push_back("Ffilename");
		bas.push_back("A");
	}

	char str[256];
	sprintf(str,"CLEAR ,&H%X",storeAddr-1);
	AddBasicLine(bas,lineNum,str);

	AddBasicLine(bas,lineNum,"DEFINT A-Z");

	sprintf(str,"A=&H%X",storeAddr);
	AddBasicLine(bas,lineNum,str);

	sprintf(str,"FOR I=0 TO %d",siz-1);
	AddBasicLine(bas,lineNum,str);
	AddBasicLine(bas,lineNum,"READ X:POKE A,X:A=A+1");
	AddBasicLine(bas,lineNum,"NEXT");

	if(true==saveFM7File)
	{
		sprintf(str,"SAVEM \"%s\",&H%X,&H%X,&H%X",filenameInFM7,storeAddr,storeAddr+siz-1,execAddr);
		AddBasicLine(bas,lineNum,str);
	}
	if(true==autoExec)
	{
		sprintf(str,"EXEC &H%X",execAddr);
		AddBasicLine(bas,lineNum,str);
	}

	const int unit=16;
	for(int i=0; i<siz; i+=unit)
	{
		std::string dataLine="DATA ";
		for(int j=0; i+j<siz && j<unit; ++j)
		{
			if(0<j)
			{
				dataLine=dataLine+",";
			}
			sprintf(str,"%d",dat[i+j]);
			dataLine=dataLine+str;
		}
		AddBasicLine(bas,lineNum,dataLine);
	}

	if(true==forRS232C)
	{
		bas.push_back("QQQ");
	}

	printf("Store Addr=%X  End Addr=%X  Exec Addr=%x\n",storeAddr,storeAddr+siz-1,execAddr);

	return bas;
}

std::vector <std::string> SRECFile(const char srecFName[],const char filenameInFM7[],int offset,bool autoExec,bool saveFM7File,bool forRS232C)
{
	std::vector <std::string> txt;

	auto srec=FM7Lib::ReadTextFile(srecFName);
	FM7BinaryFile binDat;
	if(0<srec.size() && true==binDat.DecodeSREC(srec))
	{
		return BinaryAsMachineGo(binDat.dat.data(),filenameInFM7,binDat.storeAddr+offset,binDat.dat.size(),binDat.execAddr+offset,autoExec,saveFM7File,forRS232C);
	}
	else
	{
		fprintf(stderr,"Cannot read file: %s\n",srecFName);
	}
	return txt;
}

std::vector <std::string> BinaryFileAsMachinGo(
    const char binFName[],const char filenameInFM7[],int storeAddr,int execAddr,bool autoExec,bool saveFM7File,bool forRS232C)
{
	auto bin=FM7Lib::ReadBinaryFile(binFName);
	return BinaryAsMachineGo(bin.data(),filenameInFM7,storeAddr,bin.size(),execAddr,autoExec,saveFM7File,forRS232C);
}

class MachineGoToBASICConverter
{
public:
	int offset;
	bool autoExec;
	bool saveFM7File;
	bool forRS232C;
	std::string outFName;

	MachineGoToBASICConverter();
	int ProcessOption(int ac,char *av[],int ptr);
	void TransmitEmptyLine(int n);
};

MachineGoToBASICConverter::MachineGoToBASICConverter()
{
	offset=0;
	autoExec=false;
	saveFM7File=true;
	forRS232C=false;
}

int MachineGoToBASICConverter::ProcessOption(int ac,char *av[],int ptr)
{
	std::string opt=av[ptr];
	FM7Lib::Capitalize(opt);
	printf("Option: %s\n",opt.c_str());
	if("-H"==opt || "-HELP"==opt || "-?"==opt)
	{
		CommandHelp();
	}
	else if("-O"==opt || "-OUT"==opt)
	{
		outFName=av[ptr+1];
		return 2;
	}
	else if("-AUTOEXEC"==opt)
	{
		autoExec=true;
		return 1;
	}
	else if("-DONTSAVEFM7"==opt)
	{
		saveFM7File=false;
		return 1;
	}
	else if("-FORRS232C"==opt)
	{
		forRS232C=true;
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
			auto txt=SRECFile(av[ptr+1],av[ptr+2],offset,autoExec,saveFM7File,forRS232C);
			if(0<txt.size())
			{
				FM7Lib::WriteTextFile(outFName.c_str(),txt);
				return 3;
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
			auto txt=BinaryFileAsMachinGo(av[ptr+1],av[ptr+2],FM7Lib::Atoi(av[ptr+3]),FM7Lib::Atoi(av[ptr+4]),autoExec,saveFM7File,forRS232C);
			if(0<txt.size())
			{
				FM7Lib::WriteTextFile(outFName.c_str(),txt);
				return 5;
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

	MachineGoToBASICConverter MachineGoToBASICConverter;

	int ptr=1;
	while(ptr<ac)
	{
		int nUsed=MachineGoToBASICConverter.ProcessOption(ac,av,ptr);
		if(nUsed<0)
		{
			fprintf(stderr,"Fatal error. Cannot continue.\n");
			return 1;
		}
		ptr+=nUsed;
	}

	return 0;
}


