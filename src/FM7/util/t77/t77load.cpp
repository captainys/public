#include <stdio.h>
#include <vector>

#include "../lib/fm7lib.h"
#include "../lib/cpplib.h"
#include "t77.h"



void CommandHelp(void)
{
	printf("Usage: t77load t77file.t77\n");
}

bool ReadT77(T77File &t77,const char fName[])
{
	auto rawT77=FM7Lib::ReadBinaryFile(fName);
	if(0==rawT77.size())
	{
		return false;
	}
	t77.DumpT77((std::vector <unsigned char> &&)rawT77);
	return true;
}

int main(int ac,char *av[])
{
	if(ac<2)
	{
		CommandHelp();
		return 1;
	}

	T77Decoder t77Dec;
	if(true!=ReadT77(t77Dec,av[1]))
	{
		fprintf(stderr,"Cannot read %s\n",av[1]);
		return 1;
	}

	if(true!=t77Dec.Decode())
	{
		fprintf(stderr,"Device I/O Error\n");
		return 1;
	}

	printf("%d files found.\n",(int)t77Dec.fileDump.size());
	for(auto &dump : t77Dec.fileDump)
	{
		auto fmDat=t77Dec.DumpToFMFormat(dump);
		if(16<=fmDat.size())
		{
			auto fName=t77Dec.GetDumpFileName(dump);
			printf("%-8s  ",fName.c_str());
			printf("RawSize:%-8d",fmDat.size());

			auto fType=FM7File::DecodeFMHeaderFileType(fmDat[10],fmDat[11]);
			printf("%-12s  ",FM7File::FileTypeToString(fType));
			if(fType==FM7File::FTYPE_BINARY)
			{
				FM7BinaryFile binFile;
				binFile.Decode2B0(fmDat);
				printf("LEN:0x%04x FRM:0x%04x EXE:0x%04x",
				    (int)binFile.dat.size(),
				    binFile.storeAddr,
				    binFile.execAddr);
			}
			printf("\n");
		}
	}

	int num=0;
	for(auto &dump : t77Dec.fileDump)
	{
		auto fmDat=t77Dec.DumpToFMFormat(dump);
		if(16<=fmDat.size())
		{
			auto fName=t77Dec.GetDumpFileName(dump);
			auto fType=FM7File::DecodeFMHeaderFileType(fmDat[10],fmDat[11]);

			char c_outFName[256];
			sprintf(c_outFName,"%03d-",num);

			std::string outFName=c_outFName;
			outFName.append(fName);
			outFName.append(FM7File::GetDefaultFMFileExtensionForType(fType));

			if(true==FM7Lib::WriteBinaryFile(outFName.c_str(),fmDat))
			{
				printf("Saved: %s\n",outFName.c_str());
			}
			else
			{
				printf("Error while saving: %s\n",outFName.c_str());
			}
			++num;
		}
	}

	return 0;
}
