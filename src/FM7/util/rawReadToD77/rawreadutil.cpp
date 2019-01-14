#include "fm7lib.h"
#include "cpplib.h"
#include "rawreadutil.h"



FM7RawDiskReadUtil::CommandParameterInfo::CommandParameterInfo()
{
	CleanUp();
}
void FM7RawDiskReadUtil::CommandParameterInfo::CleanUp(void)
{
	inFName="";
	d77OutFName="";
	verifyRawRead=true;
	makeD77=true;
}
bool FM7RawDiskReadUtil::CommandParameterInfo::RecognizeCommandParameter(int ac,char *av[])
{
	int nOption=0;
	for(int i=1; i<ac; ++i)
	{
		std::string cmd(av[i]);
		if(cmd=="-verifyonly")
		{
			verifyRawRead=true;
			makeD77=false;
		}
		else if(cmd=="-help" || cmd=="-h" || cmd=="-?")
		{
			ShowHelp();
		}
		else
		{
			if(""==inFName)
			{
				inFName=av[i];
			}
			else if(""==d77OutFName)
			{
				d77OutFName=av[i];
			}
			else
			{
				fprintf(stderr,"Error! Unrecognized command option %s\n",av[i]);
				return false;
			}
		}
	}
	if(inFName=="")
	{
		fprintf(stderr,"Error! No input file.\n");
		return false;
	}
	if(true==makeD77 && d77OutFName=="")
	{
		fprintf(stderr,"D77 file name not specified.\n");
		return false;
	}
	return true;
}

/* static */ void FM7RawDiskReadUtil::CommandParameterInfo::ShowHelp(void)
{
	printf("Command Option Help:\n");
	printf("  -h, -help, -?\n");
	printf("     Show this help.\n");
	printf("  -verifyonly\n");
	printf("     Verify checksum and xor of Raw Read and do not make D77 file.\n");
}

////////////////////////////////////////////////////////////

FM7RawDiskReadUtil::FM7RawDiskReadUtil()
{
	CleanUp();
}
void FM7RawDiskReadUtil::CleanUp(void)
{
	rawDisk.clear();
}
int FM7RawDiskReadUtil::RunBatchMode(const CommandParameterInfo &cpmi)
{
	if(true!=Load(cpmi.inFName.c_str()))
	{
		fprintf(stderr,"Error! Failed to open %s\n",cpmi.inFName.c_str());
		return 1;
	}

	if(true==cpmi.verifyRawRead)
	{
		printf("Verifying Check Sum, Size, and XOR\n");
		for(auto &dsk : rawDisk)
		{
			dsk.VerifyCheckSum();
		}
	}

	if(true==cpmi.makeD77 && cpmi.d77OutFName!="")
	{
		if(true==SaveAsD77(cpmi.d77OutFName.c_str()))
		{
			printf("Wrote %s\n",cpmi.d77OutFName.c_str());
		}
		else
		{
			printf("Error! Failed to write %s\n",cpmi.d77OutFName.c_str());
		}
	}

	return 0;
}
bool FM7RawDiskReadUtil::Load(const char fn[])
{
	rawDisk=FM7RawDiskRead::Load(fn);
	if(0<rawDisk.size())
	{
		return true;
	}
	return false;
}
bool FM7RawDiskReadUtil::SaveAsD77(const char fn[]) const
{
	std::vector <unsigned char> d77Image;

	for(auto &d : rawDisk)
	{
		auto d77Disk=d.MakeD77Disk();
		auto img=d77Disk.MakeD77Image();
		for(auto c : img)
		{
			d77Image.push_back(c);
		}
	}

	return FM7Lib::WriteBinaryFile(fn,d77Image);
}
