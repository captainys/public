#include <stdio.h>
#include <vector>

#include "../lib/fm7lib.h"
#include "../lib/cpplib.h"
#include "t77.h"



void CommandHelp(void)
{
	printf("Usage: t77save output.t77 <options>\n");
	printf("Options will be interpreted from left to right.\n");
	printf("Options:\n");
	printf("-h\n");
	printf("-help\n");
	printf("-?\n");
	printf("\tShow this help.\n");
	printf("-new\n");
	printf("\tIgnore existing output.t77 and make a fresh T77.\n");
	printf("-fmwrite fm-file-name filename-in-T77\n");
	printf("\tWrite fm-file to T77.  If \"()\" or \"[]\" or \" \" or \"\" is specified as filename-in-T77,\n");
	printf("\tT77 file name will be taken from the name given in an fm-file.\n");
	printf("-srecwrite srec-file-name filename-in-T77\n");
	printf("\tWrite SREC file to T77.\n");
	printf("\tASM6809 v2.9 writes wrong check sum (not taking a complement?).\n");
	printf("\tThis program ignores the check sum to accept SREC from ASM6809 v2.9.\n");
	printf("-save text-file-name filename-in-T77\n");
	printf("\tSave a text file as an ASCII-format BASIC file to T77.\n");
	printf("-savem binary-file-name filename-in-T77 startAddr execAddr\n");
	printf("\tSave a binary file to T77.  Like to save a file you typed from I/O magazine.\n");
	printf("\tUnlike SAVEM command of F-BASIC endAddr is automatically calculated from\n");
	printf("\tthe file size.  Therefore, you only need to give start and exec addresses.\n");
	printf("\tYou can use &H of 0x to specify address in hexa-decimal number.\n");
	printf("-saveasciidump dumpfile.txt\n");
	printf("\tAppend raw binary dump to .T77\n");
	printf("\tCan be used for creating a .T77 file from raw tape dump.\n");
	printf("-wav\n");
	printf("\tSave as .WAV.  Just specifying this option will create both .T77 and .WAV files.\n");
	printf("\tIf you don't need .T77, also specify -not77\n");
	printf("\tBy default, this program only saves .T77 file.\n");
	printf("\tThis option only affects what file will be saved in the end.\n");
	printf("\tFile name of the .WAV file will be same as argv[1] with .WAV extension.\n");
	printf("-not77\n");
	printf("\tDon't save .T77 file.\n");
	printf("\tThis option only affects what file will be saved in the end.\n");
	printf("-dbgcsum\n");
	printf("\tThis option causes intentional check-sum error.\n");
	printf("\tExperiment if FM-7 gives Device I/O Error if check sum is wrong.\n");
	printf("-dbgcsumd\n");
	printf("\tThis option causes intentional check-sum error of data blocks.\n");
	printf("-dbgcsumh\n");
	printf("\tThis option causes intentional check-sum error of header blocks.\n");
	printf("-dbgmix77datagap\n");
	printf("\tThis option mixes 0x77 in the data-gap where it needs to be 0xFF.\n");
	printf("\tFor testing an effect of error in the data-gap.\n");
}

bool WriteFMFile(T77Encoder &encoder,const char fmFName[],const char filenameInT77[])
{
	auto dat=FM7Lib::ReadBinaryFile(fmFName);
	if(0<dat.size())
	{
		return encoder.EncodeFromFMFile(filenameInT77,dat);
	}
	return false;
}

bool WriteSRECFile(T77Encoder &encoder,const char srecFName[],const char filenameInT77[])
{
	auto srec=FM7Lib::ReadTextFile(srecFName);
	FM7BinaryFile binDat;
	if(0<srec.size() && true==binDat.DecodeSREC(srec))
	{
		auto fmDat=FM7Lib::BinaryTo2B0(binDat.dat,filenameInT77,binDat.storeAddr,binDat.execAddr);
		if(true!=encoder.EncodeFromFMFile(filenameInT77,fmDat))
		{
			fprintf(stderr,"Failed to write a file.\n");
		}
		else
		{
			return true;
		}
	}
	else
	{
		fprintf(stderr,"Cannot read file: %s\n",srecFName);
	}
	return false;
}

bool SaveTextFileAsBASIC(T77Encoder &encoder,const char txtFName[],const char filenameInT77[])
{
	auto text=FM7Lib::ReadTextFile(txtFName);
	if(0<text.size())
	{
		auto fmDat=FM7Lib::TextTo0A0(text,filenameInT77);
		if(true!=encoder.EncodeFromFMFile(filenameInT77,fmDat))
		{
			fprintf(stderr,"Failed to write a file.\n");
		}
		else
		{
			return true;
		}
	}
	else
	{
		fprintf(stderr,"Cannot read file: %s\n",txtFName);
	}

	return false;
}

bool SaveBinaryFileAsMachinGo(
    T77Encoder &encoder,const char binFName[],const char filenameInT77[],int storeAddr,int execAddr)
{
	auto bin=FM7Lib::ReadBinaryFile(binFName);
	auto fmDat=FM7Lib::BinaryTo2B0(bin,filenameInT77,storeAddr,execAddr);
	if(true!=encoder.EncodeFromFMFile(filenameInT77,fmDat))
	{
		fprintf(stderr,"Failed to write a file.\n");
	}
	else
	{
		return true;
	}
	return false;
}

bool AppendAsciiDump(T77Encoder &encoder,const char dumpFName[])
{
	auto txt=FM7Lib::ReadTextFile(dumpFName);
	if(0<txt.size())
	{
		for(auto str : txt)
		{
			for(int i=0; i+1<str.size(); i+=2)
			{
				char hex[3]={str[i],str[i+1],0};
				auto num=FM7Lib::Xtoi(hex);
				encoder.AddByte(num);
			}
		}
		return true;
	}
	return false;
}

class T77Save
{
public:
	T77Encoder encoder;
	T77File::WAVOutOption wavOutOption;
	bool saveAsT77;
	bool saveAsWav;

	T77Save();
	int ProcessOption(int ac,char *av[],int ptr);
};

T77Save::T77Save()
{
	saveAsT77=true;
	saveAsWav=false;
}

int T77Save::ProcessOption(int ac,char *av[],int ptr)
{
	std::string opt=av[ptr];
	FM7Lib::Capitalize(opt);
	printf("Option: %s\n",opt.c_str());
	if("-NEW"==opt)
	{
		encoder.CleanUp();
		encoder.StartT77Header();
		return 1;
	}
	else if("-H"==opt || "-HELP"==opt || "-?"==opt)
	{
		CommandHelp();
	}
	else if("-FMWRITE"==opt)
	{
		if(ptr+3<=ac)
		{
			std::string t77FName=av[ptr+2];
			if(""==t77FName || " "==t77FName || "()"==t77FName || "[]"==t77FName)
			{
				if(true==WriteFMFile(encoder,av[ptr+1],nullptr))
				{
					return 3;
				}
			}
			else
			{
				if(true==WriteFMFile(encoder,av[ptr+1],av[ptr+2]))
				{
					return 3;
				}
			}
			goto ERROR_PROCESSING;
		}
		else
		{
			goto TOO_FEW_ARG;
		}
	}
	else if("-SRECWRITE"==opt)
	{
		if(ptr+3<=ac)
		{
			if(true==WriteSRECFile(encoder,av[ptr+1],av[ptr+2]))
			{
				return 3;
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
			if(true==SaveTextFileAsBASIC(encoder,av[ptr+1],av[ptr+2]))
			{
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
			if(true==SaveBinaryFileAsMachinGo(
			    encoder,av[ptr+1],av[ptr+2],FM7Lib::Atoi(av[ptr+3]),FM7Lib::Atoi(av[ptr+4])))
			{
				return 5;
			}
			goto ERROR_PROCESSING;
		}
		else
		{
			goto TOO_FEW_ARG;
		}
	}
	else if("-SAVEASCIIDUMP"==opt)
	{
		if(ptr+1<=ac)
		{
			if(true==AppendAsciiDump(encoder,av[ptr+1]))
			{
				return 2;
			}
			goto ERROR_PROCESSING;
		}
		else
		{
			goto TOO_FEW_ARG;
		}
	}
	else if("-WAV"==opt)
	{
		saveAsWav=true;
		return 1;
	}
	else if("-NOT77"==opt)
	{
		saveAsT77=false;
		return 1;
	}
	else if("-DBGCSUM"==opt)
	{
		encoder.debug_makeIntentionalCheckSumError_headerBlock=true;
		encoder.debug_makeIntentionalCheckSumError_dataBlock=true;
		return 1;
	}
	else if("-DBGCSUMD"==opt)
	{
		encoder.debug_makeIntentionalCheckSumError_dataBlock=true;
		return 1;
	}
	else if("-DBGCSUMH"==opt)
	{
		encoder.debug_makeIntentionalCheckSumError_headerBlock=true;
		return 1;
	}
	else if("-DBGMIX77DATAGAP"==opt)
	{
		encoder.debug_mixNonFFinDataGap=true;
		return 1;
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

	T77Save t77save;
	if(true==FM7Lib::FileExist(av[1]))
	{
		t77save.encoder.DumpT77(FM7Lib::ReadBinaryFile(av[1]));
	}

	int ptr=2;
	while(ptr<ac)
	{
		int nUsed=t77save.ProcessOption(ac,av,ptr);
		if(nUsed<0)
		{
			fprintf(stderr,"Fatal error. Cannot continue.\n");
			return 1;
		}
		ptr+=nUsed;
	}

	if(true==t77save.saveAsT77)
	{
		if(true!=FM7Lib::WriteBinaryFile(av[1],t77save.encoder.t77))
		{
			fprintf(stderr,"Cannot open %s for writing.\n",av[1]);
			return 1;
		}
		else
		{
			printf("Saved %s\n",av[1]);
		}
	}

	if(true==t77save.saveAsWav)
	{
		std::string wavFName=FM7Lib::ChangeExtension(av[1],".wav");
		if(true!=t77save.encoder.WriteWav(wavFName.c_str(),t77save.wavOutOption))
		{
			fprintf(stderr,"Cannot open %s for writing.\n",wavFName.c_str());
			return 1;
		}
		else
		{
			printf("Saved %s\n",wavFName.c_str());
		}
	}
	return 0;
}

