#include <stdio.h>
#include <string>
#include <fstream>
#include "cpplib.h"
#include "fm7lib.h"


#define WAIT "\\\\\\WAIT"
#define BREAK "\\\\\\BREAK"


class FileInfo
{
public:
	std::string fName;
	bool isBasic;
	int store,size,exec;

	std::string GetFM7FileName(void) const
	{
		auto base=FM7Lib::GetBaseName(fName.c_str());
		if(8<base.size())
		{
			base.resize(8);
		}
		return base;
	}
};

bool TypeItUp(FILE *ofp,std::vector <FileInfo> &fInfo,const char fn[])
{
	std::string FN(fn);
	FM7Lib::Capitalize(FN);
	auto EXT=FM7Lib::GetExtension(FN.c_str());

	if(EXT==".SREC")
	{
		auto fileDat=FM7Lib::ReadTextFile(fn);
		SRECDecoder srec;
		if(true!=srec.Decode(fileDat))
		{
			return false;
		}

		fprintf(ofp,"CLEAR ,&H%04X\n",srec.storeAddr-1);
		fprintf(ofp,WAIT WAIT WAIT "\n");
		fprintf(ofp,"MON\n");
		fprintf(ofp,WAIT WAIT WAIT "\n");
		fprintf(ofp,"M %x\n",srec.storeAddr);

		for(auto c : srec.data())
		{
			fprintf(ofp,"%x\n",(unsigned int)c);
		}
		fprintf(ofp,BREAK WAIT WAIT);

		FileInfo info;
		info.fName=fn;
		info.isBasic=false;
		info.store=srec.storeAddr;
		info.size=srec.data().size();
		info.exec=srec.execAddr;
		fInfo.push_back(info);

		return true;
	}
	else if(EXT==".BAS" || EXT==".TXT")
	{
		fprintf(ofp,"NEW\n");
		for(auto str : FM7Lib::ReadTextFile(fn))
		{
			fprintf(ofp,"%s\n",str.c_str());
		}

		FileInfo info;
		info.fName=fn;
		info.isBasic=true;
		fInfo.push_back(info);
		return true;
	}

	return false;
}

int main(int ac,char *av[])
{
	std::vector <FileInfo> fInfo;
	std::vector <std::string> srcFile;
	std::string outFile;


	for(int i=1; i<ac; ++i)
	{
		std::string arg=av[i];
		if((arg=="-o" || arg=="-out") && i+1<ac)
		{
			outFile=av[i+1];
			++i;
		}
		else
		{
			srcFile.push_back(arg);
		}
	}


	FILE *ofp=stdout;
	if(0<outFile.size())
	{
		ofp=fopen(outFile.c_str(),"w");
		if(nullptr==ofp)
		{
			fprintf(stderr,"Cannot open %s for writing.\n",outFile.c_str());
			return 1;
		}
	}



	for(auto fName : srcFile)
	{
		if(true!=TypeItUp(ofp,fInfo,fName.c_str()))
		{
			fprintf(stderr,"Error in %s\n",fName.c_str());
			return 1;
		}
	}


	bool first=true;
	for(auto f : fInfo)
	{
		if(true!=f.isBasic)
		{
			if(true!=first)
			{
				fprintf(ofp,":");
			}
			auto base=f.GetFM7FileName();
			fprintf(ofp,"SAVEM \"%s\",&H%04X,&H%04X,&H%04X",base.c_str(),
			    f.store,
			    f.store+f.size-1,
			    f.exec);
			first=false;
		}
	}
	for(auto f : fInfo)
	{
		if(true==f.isBasic)
		{
			if(true!=first)
			{
				fprintf(ofp,":");
			}
			auto base=f.GetFM7FileName();
			fprintf(ofp,"SAVE \"%s\"",base.c_str());
			first=false;
		}
	}
	fprintf(ofp,"\n");

	if(stdout!=ofp)
	{
		fclose(ofp);
	}

	return 0;
}

