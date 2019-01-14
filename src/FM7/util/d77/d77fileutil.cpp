#include "d77filesystem.h"
#include "../lib/fm7lib.h"
#include "../lib/cpplib.h"



class D77FileUtil
{
public:
	int nextDirDump;
	int diskId;
	bool quit;
	D77File *d77Ptr;
	D77FileSystem d77FileSys;
	std::string d77FName;

	D77FileUtil();

	void Terminal(D77File &d77);
	void ProcessCommand(const std::vector <std::string> &argv);
	static void TerminalHelp(void);

	int CommandMode(D77File &d77,int ac,char *av[]);
	static void CommandHelp(void);
	int ProcessOption(int ac,char *av[],int ptr);


	void Files(void) const;
	void ShowFATDump(void) const;
	void ShowInfo(const char fn[]) const;
	void ShowInfo(int fileIdx) const;

	bool RawRead(const char filenameInD77[],const char rawFName[]) const;
	bool D77FileToFMFile(const char filenameInD77[],const char fmFName[]) const;
	bool D77FileIndexToFMFile(int fileIdx,const char fmFName[]) const;
	bool D77FileToBinaryFile(const char filenameInD77[],const char srecFName[]) const;
	bool D77FileIndexToBinaryFile(int fileIdx,const char srecFName[]) const;
	bool KillFile(const char filenameInD77[]);
	bool WriteFMFile(const char fmFName[],const char filenameInD77[]);
	bool WriteSRECFile(const char srecFName[],const char filenameInD77[]);
	bool SaveTextFileAsBASIC(const char txtFName[],const char filenameInD77[]);
	bool SaveBinaryFileAsMachinGo(const char binFName[],const char filenameInD77[],int startAddr,int execAddr);
	bool CheckD77FileName(const char fName[]) const;
	bool CheckD77FileName(const std::string &fName) const;
	void KillIfFileAlradyExists(const char fName[]);
	void KillIfFileAlradyExists(const std::string &fName);
	void UnWriteProtect(void);
	void WriteProtect(void);
	bool SaveDisk(const char d77FName[]);
	bool ForceSaveDisk(const char d77FName[]);
	void DumpDir(int dirPosition) const;
};

D77FileUtil::D77FileUtil()
{
	nextDirDump=0;
	diskId=0;
	quit=false;
	d77Ptr=nullptr;
}


void D77FileUtil::Terminal(D77File &d77)
{
	diskId=0;
	d77Ptr=&d77;

	{
		auto diskPtr=d77.GetDisk(diskId);
		if(nullptr==diskPtr)
		{
			printf("No disk.\n");
			return;
		}
		if(true!=d77FileSys.ReadDirectory(*diskPtr))
		{
			printf("Cannot read the directory.\n");
			printf("Disk may be broken, not 2D, or not formatted for F-BASIC.\n");
			return;
		}
	}

	Files();

	quit=false;
	while(true!=quit)
	{
		auto diskPtr=d77Ptr->GetDisk(diskId);
		if(nullptr!=diskPtr && true==diskPtr->IsWriteProtected())
		{
			printf("Disk Write Protected.\n");
		}
		if(nullptr!=diskPtr && true==diskPtr->IsModified())
		{
			printf("Disk has been modified.\n");
		}

		printf("Command H:Help>");
		char cmd[256];
		fgets(cmd,255,stdin);

		auto argv=D77File::QuickParser(cmd);
		ProcessCommand(argv);
	}
}

void D77FileUtil::ProcessCommand(const std::vector <std::string> &argv)
{
	if(0==argv.size())
	{
		return;
	}

	auto diskPtr=d77Ptr->GetDisk(diskId);

	auto cmd=argv[0];
	D77File::Capitalize(cmd);
	if(cmd[0]=='H')
	{
		TerminalHelp();
	}
	else if(cmd=="FILES")
	{
		Files();
	}
	else if(cmd=="INFO")
	{
		for(int i=1; i<argv.size(); ++i)
		{
			ShowInfo(argv[i].c_str());
		}
	}
	else if(cmd=="INFON")
	{
		for(int i=1; i<argv.size(); ++i)
		{
			ShowInfo(FM7Lib::Atoi(argv[i].c_str()));
		}
	}
	else if(cmd=="KILL" && nullptr!=diskPtr)
	{
		for(int i=0; i<argv.size(); ++i)
		{
			auto &s=argv[i];
			KillFile(s.c_str());
		}
	}
	else if(cmd=="KILLN" && nullptr!=diskPtr)
	{
		for(int i=0; i<argv.size(); ++i)
		{
			auto &s=argv[i];
			d77FileSys.KillN(*diskPtr,FM7Lib::Atoi(s.c_str()));
			d77FileSys.WriteDirectory(*diskPtr);
		}
	}
	else if(cmd=="FAT")
	{
		ShowFATDump();
	}
	else if(cmd=="DIR")
	{
		if(2<=argv.size())
		{
			auto dirPos=FM7Lib::Atoi(argv[1].c_str());
			DumpDir(dirPos);
			nextDirDump=dirPos+8;
		}
		else
		{
			DumpDir(nextDirDump);
			nextDirDump+=8;
		}
	}
	else if(cmd=="RAWREAD" && 3<=argv.size() && nullptr!=diskPtr)
	{
		RawRead(argv[1].c_str(),argv[2].c_str());
	}
	else if(cmd=="FMREAD" && 3<=argv.size() && nullptr!=diskPtr)
	{
		D77FileToFMFile(argv[1].c_str(),argv[2].c_str());
	}
	else if(cmd=="FMREADN" && 3<=argv.size() && nullptr!=diskPtr)
	{
		D77FileIndexToFMFile(FM7Lib::Atoi(argv[1].c_str()),argv[2].c_str());
	}
	else if(cmd=="BINREAD" && 3<=argv.size() && nullptr!=diskPtr)
	{
		D77FileToBinaryFile(argv[1].c_str(),argv[2].c_str());
	}
	else if(cmd=="BINREADN" && 3<=argv.size() && nullptr!=diskPtr)
	{
		D77FileIndexToBinaryFile(FM7Lib::Atoi(argv[1].c_str()),argv[2].c_str());
	}
	else if(cmd=="FMWRITE" && 2<=argv.size())
	{
		if(3<=argv.size())
		{
			WriteFMFile(argv[1].c_str(),argv[2].c_str());
		}
		else
		{
			WriteFMFile(argv[1].c_str(),nullptr);
		}
	}
	else if(cmd=="SRECWRITE" && 3<=argv.size())
	{
		WriteSRECFile(argv[1].c_str(),argv[2].c_str());
	}
	else if(cmd=="SAVE" && 3<=argv.size())
	{
		SaveTextFileAsBASIC(argv[1].c_str(),argv[2].c_str());
	}
	else if(cmd=="SAVEM" && 5<=argv.size())
	{
		SaveBinaryFileAsMachinGo(argv[1].c_str(),argv[2].c_str(),FM7Lib::Atoi(argv[3].c_str()),FM7Lib::Atoi(argv[4].c_str()));
	}
	else if(cmd=="UNPROTECT")
	{
		UnWriteProtect();
	}
	else if(cmd=="PROTECT")
	{
		WriteProtect();
	}
	else if(cmd=="SAVEDISK")
	{
		auto fName=this->d77FName;
		if(2<=argv.size())
		{
			fName=argv[1];
		}
		if(true!=SaveDisk(fName.c_str()))
		{
			fprintf(stderr,"Write error.\n");
		}
		else
		{
			this->d77FName=fName;
			fprintf(stderr,"Saved to %s\n",fName.c_str());
		}
	}
	else if('Q'==cmd[0])
	{
		quit=true;
	}
}

/*static*/ void D77FileUtil::TerminalHelp(void)
{
	printf("Q\n");
	printf("\tQuit\n");
	printf("FILES\n");
	printf("\tList files.\n");
	printf("FAT\n");
	printf("\tPrint FAT dump.\n");
	printf("DIR pos\n");
	printf("\tPrint directory dump.\n");
	printf("\tIf you don't type pos, it shows the next 8 entries.\n");
	printf("INFO filename\n");
	printf("INFON file-number\n");
	printf("\tShow information of the file.\n");
	printf("KILL filename\n");
	printf("KILLN file-number\n");
	printf("\tDelete a file.\n");
	printf("RAWREAD filename-in-d77 raw-filename\n");
	printf("\tRead raw bytes of fm7-file and write to raw-file.\n");
	printf("FMREAD filename-in-d77 .FM-filename\n");
	printf("\tRead filename-in-d77 and write to a FM-FILE format.\n");
	printf("FMREADN file-number .FM-filename\n");
	printf("\tRead file-number and write to a FM-FILE format.\n");
	printf("BINREAD filename-in-d77 binary-filename\n");
	printf("\tRead filename-in-d77 and write to a binary file.\n");
	printf("\tThe file must be a binary.\n");
	printf("\tDifference from RAWREAD is BINREAD chops off first 0x00, size, \n");
	printf("\tstore address, last 0xff0000, and exec address.\n");
	printf("BINREADN file-number binary-filename\n");
	printf("\tRead file-number and write to a binary file.\n");
	printf("\tThe file must be a binary.\n");
	printf("\tDifference from RAWREAD is BINREAD chops off first 0x00, size, \n");
	printf("\tstore address, last 0xff0000, and exec address.\n");
	printf("FMWRITE FM-filename\n");
	printf("FMWRITE FM-filename filename-in-d77\n");
	printf("\tWrite FM-FILE format file to .D77.\n");
	printf("\tIf filename-in-d77 is omitted, the file name is taken from the input file.\n");
	printf("SRECWRITE SREC-filename filename-in-d77\n");
	printf("\tWrite SREC file to D77.\n");
	printf("\tASM6809 v2.9 writes wrong check sum (not taking a complement?).\n");
	printf("\tThis program ignores the check sum to accept SREC from ASM6809 v2.9.\n");
	printf("SAVE text-file-name filename-in-d77\n");
	printf("\tSave a text file as an ASCII-format BASIC file to D77.\n");
	printf("SAVEM binary-file-name filename-in-d77 startAddr execAddr\n");
	printf("\tSave a binary file to D77.  Like to save a file you typed from I/O magazine.\n");
	printf("\tUnlike SAVEM command of F-BASIC endAddr is automatically calculated from\n");
	printf("\tthe file size.  Therefore, you only need to give start and exec addresses.\n");
	printf("\tYou can use &H of 0x to specify address in hexa-decimal number.\n");
	printf("UNPROTECT\n");
	printf("\tRemoves write-protected flag of the disk.\n");
	printf("\tIt does not modify the original disk.\n");
	printf("\tYou can un-write-protect, edit, and then save to a different .D77 file.\n");
	printf("PROTECT\n");
	printf("\tSets write-protected flag of the disk.\n");
	printf("SAVEDISK filename.D77\n");
	printf("\tSave changes to the disk.\n");
	printf("\tIf the specified disk image is write-protected or\n");
	printf("\tor consists of multiple disks, it fails.\n");
	printf("\n");
	printf("File-writing commands will overwrite a file without prompt if the file\n");
	printf("already exists.\n");

}

int D77FileUtil::CommandMode(D77File &d77,int ac,char *av[])
{
	// New file?
	std::string opt;
	opt=av[2];
	FM7Lib::Capitalize(opt);

	this->d77Ptr=&d77;
	this->diskId=0;

	if("-NEW"==opt)
	{
		d77.CreateStandardFormatted();
		if(true!=FM7Lib::FileExist(av[1]))
		{
			// Case 1.  The file does not exist.
			if(true!=SaveDisk(av[1]))
			{
				fprintf(stderr,"Cannot create a .D77 file.\n");
				return 1;
			}
		}
		else
		{
			// Case 2.  The file does exist.
			if(true!=SaveDisk(av[1]))
			{
				fprintf(stderr,"Cannot overwrite a .D77 file.\n");
				fprintf(stderr,"Existing .D77 file may be write protected, or\n");
				fprintf(stderr,"is a multi-disk image.\n");
				return 1;
			}
		}
	}
	else
	{
		auto dat=FM7Lib::ReadBinaryFile(av[1]);
		if(0==dat.size())
		{
			fprintf(stderr,"Cannot read file: %s\n",av[1]);
			return 1;
		}
		d77FName=av[1];
		d77.SetData(dat,/*verboseMode=*/false);
	}

	if("-SPLIT"==opt)
	{
		if(d77.GetNumDisk()<=1)
		{
			fprintf(stderr,"Already a single-disk D77.\n");
			return 1;
		}

		std::string fNameBase=FM7Lib::RemoveExtension(d77FName.c_str());
		for(int i=0; i<d77.GetNumDisk(); ++i)
		{
			auto fName=fNameBase;
			char suffix[32];
			sprintf(suffix,"-%d.d77",i);
			fName.append(suffix);
			if(true==FM7Lib::FileExist(fName.c_str()))
			{
				fprintf(stderr,"File %s already exists.\n",fName.c_str());
				return 1;
			}
		}

		for(int i=0; i<d77.GetNumDisk(); ++i)
		{
			auto fName=fNameBase;
			char suffix[32];
			sprintf(suffix,"-%d.d77",i);
			fName.append(suffix);

			this->diskId=i;
			SaveDisk(fName.c_str());
		}

		return 0;
	}


	if(1<d77.GetNumDisk())
	{
		fprintf(stderr,"This program only deals with a single-disk D77 file.\n");
		return 1;
	}



	auto diskPtr=d77Ptr->GetDisk(diskId);
	if(nullptr==diskPtr)
	{
		printf("No disk.\n");
		return 1;
	}
	if(true!=this->d77FileSys.ReadDirectory(*diskPtr))
	{
		printf("Cannot read the directory.\n");
		printf("Disk may be broken, not 2D, or not formatted for F-BASIC.\n");
		return 1;
	}



	bool initiallyProtected=diskPtr->IsWriteProtected();

	int ptr=2;
	while(ptr<ac)
	{
		int nUsed=ProcessOption(ac,av,ptr);
		if(nUsed<0)
		{
			fprintf(stderr,"Fatal error. Cannot continue.\n");
			return 1;
		}
		ptr+=nUsed;
	}



	if(true==diskPtr->IsModified())
	{
		// One of original or new must be write-unprotected.
		if(true!=initiallyProtected ||
		   true!=diskPtr->IsWriteProtected())
		{
			if(true!=ForceSaveDisk(d77FName.c_str()))
			{
				fprintf(stderr,"Cannot save disk image.\n");
				return 1;
			}
		}
	}

	return 0;
}

/*static*/void D77FileUtil::CommandHelp(void)
{
	printf("To start a terminal mode with an existing .D77 file:\n");
	printf("\td77fileutil d77file.d77\n");
	printf("To start a terminal mode with a new .D77 file:\n");
	printf("\td77fileutil\n");
	printf("Command mode with an existing .D77 file:\n");
	printf("\td77fileutil d77file.d77 <options>\n");
	printf("Command mode with a new .D77 file:\n");
	printf("\td77fileutil d77file.d77 -new <options>\n");
	printf("Command options (case insensitive):\n");
	printf("-h\n");
	printf("-help\n");
	printf("-?\n");
	printf("\tShow this help.\n");
	printf("-new\n");
	printf("\tStart with a new .D77 file.  This option can only be immediately\n");
	printf("\tafter the .D77 file name.  If -new is specified, and specified\n");
	printf("\t.D77 file already exists, and if the specified .D77 file is\n");
	printf("\tmarked as write-protected, this option will fail and the program\n");
	printf("\tquits immediately.\n");
	printf("-split\n");
	printf("\tSplits a multi-disk .D77 into multiple single-disk .D77 files.\n");
	printf("\tThis option can only be immediately after the .D77 file name.\n");
	printf("\tFile name will automatically be created.\n");
	printf("\tIt does nothing if a new .D77 file name is already used.\n");
	printf("\tIt does nothing if the input .D77 file is a single-disk image.\n");
	printf("\tAll options following -split will be ignored.\n");
	printf("-files\n");
	printf("\tShow files in .D77\n");
	printf("-fat\n");
	printf("\tShow FAT dump.\n");
	printf("-dir\n");
	printf("\tShow directory dump.\n");
	printf("-rawread filename-in-d77 raw-file-name\n");
	printf("\tRead raw file from D77 and write to raw-file-name.\n");
	printf("-fmread filename-in-d77 fm-file-name\n");
	printf("\tRead a file from D77 and write to FM-FILE format.\n");
	printf("-binread filename-in-d77 bin-file-name\n");
	printf("\tRead a binary file from D77 and write to a file.\n");
	printf("-kill filename-in-d77\n");
	printf("\tDelete a file.\n");
	printf("-fmwrite fm-file-name filename-in-d77\n");
	printf("\tWrite fm-file to D77.  If \"()\" or \"[]\" or \" \" or \"\" is specified as filename-in-d77,\n");
	printf("\tD77 file name will be taken from the name given in an fm-file.\n");
	printf("-srecwrite srec-file-name filename-in-d77\n");
	printf("\tWrite SREC file to D77.\n");
	printf("\tASM6809 v2.9 writes wrong check sum (not taking a complement?).\n");
	printf("\tThis program ignores the check sum to accept SREC from ASM6809 v2.9.\n");
	printf("-save text-file-name filename-in-d77\n");

	printf("\tSave a text file as an ASCII-format BASIC file to D77.\n");
	printf("-savem binary-file-name filename-in-d77 startAddr execAddr\n");
	printf("\tSave a binary file to D77.  Like to save a file you typed from I/O magazine.\n");
	printf("\tUnlike SAVEM command of F-BASIC endAddr is automatically calculated from\n");
	printf("\tthe file size.  Therefore, you only need to give start and exec addresses.\n");
	printf("\tYou can use &H of 0x to specify address in hexa-decimal number.\n");
	printf("-unprotect\n");
	printf("\tRemoves write-protected flag of the disk.\n");
	printf("\tIt does not modify the original disk.\n");
	printf("\tYou can un-write-protect, edit, and then save to a different .D77 file.\n");
	printf("-protect\n");
	printf("\tSets write-protected flag of the disk.\n");
}

int D77FileUtil::ProcessOption(int ac,char *av[],int ptr)
{
	std::string opt=av[ptr];
	FM7Lib::Capitalize(opt);
	if("-NEW"==opt)
	{
		if(2==ptr)
		{
			return 1;
		}
		else
		{
			fprintf(stderr,"-new can only be used immediately after the D77 file name.\n");
			return -1;
		}
	}
	else if("-H"==opt || "-HELP"==opt || "-?"==opt)
	{
		CommandHelp();
	}
	else if("-FILES"==opt)
	{
		Files();
		return 1;
	}
	else if("-FAT"==opt)
	{
		ShowFATDump();
		return 1;
	}
	else if("-DIR"==opt)
	{
		for(int i=0; i<152; i+=8)
		{
			DumpDir(i);
		}
		return 1;
	}
	else if("-RAWREAD"==opt)
	{
		if(ptr+3<=ac)
		{
			if(true==RawRead(av[ptr+1],av[ptr+2]))
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
	else if("-FMREAD"==opt)
	{
		if(ptr+3<=ac)
		{
			if(true==D77FileToFMFile(av[ptr+1],av[ptr+2]))
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
	else if("-BINREAD"==opt)
	{
		if(ptr+3<=ac)
		{
			if(true==D77FileToBinaryFile(av[ptr+1],av[ptr+2]))
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
	else if("-KILL"==opt)
	{
		if(ptr+2<=ac)
		{
			if(true==KillFile(av[ptr+1]))
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
	else if("-FMWRITE"==opt)
	{
		if(ptr+3<=ac)
		{
			std::string d77FName=av[ptr+2];
			if(""==d77FName || " "==d77FName || "()"==d77FName || "[]"==d77FName)
			{
				if(true==WriteFMFile(av[ptr+1],nullptr))
				{
					return 3;
				}
			}
			else
			{
				if(true==WriteFMFile(av[ptr+1],av[ptr+2]))
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
			if(true==WriteSRECFile(av[ptr+1],av[ptr+2]))
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
			if(true==SaveTextFileAsBASIC(av[ptr+1],av[ptr+2]))
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
			if(true==SaveBinaryFileAsMachinGo(av[ptr+1],av[ptr+2],FM7Lib::Atoi(av[ptr+3]),FM7Lib::Atoi(av[ptr+4])))
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
	else if("-unprotect"==opt)
	{
		UnWriteProtect();
		return 1;
	}
	else if("-protect"==opt)
	{
		WriteProtect();
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

void D77FileUtil::Files(void) const
{
	auto diskPtr=d77Ptr->GetDisk(diskId);
	if(nullptr!=diskPtr)
	{
		int idx=0;
		for(auto f : d77FileSys.Files(*diskPtr))
		{
			if(D77FileSystem::STATE_USED==f.state)
			{
				auto clusterChain=d77FileSys.GetClusterChain(*diskPtr,f);

				auto str=f.FormatInfo();
				printf("[%3d] %s (%3d clusters)\n",idx,str.c_str(),(int)clusterChain.size());
			}
			++idx;
		}
	}
}

void D77FileUtil::ShowFATDump(void) const
{
	printf("            ");
	int ctr=4;
	for(auto c : d77FileSys.GetFAT())
	{
		if(0==ctr%16)
		{
			printf("\n");
		}
		printf("%02x ",c);
		++ctr;
	}
	printf("\n");
}

void D77FileUtil::ShowInfo(const char fn[]) const
{
	auto diskPtr=d77Ptr->GetDisk(diskId);
	if(nullptr!=diskPtr)
	{
		auto fileIdx=d77FileSys.FindFile(*diskPtr,fn);
		ShowInfo(fileIdx);
	}
}
void D77FileUtil::ShowInfo(int fileIdx) const
{
	auto diskPtr=d77Ptr->GetDisk(diskId);
	if(nullptr!=diskPtr)
	{
		auto fInfo=d77FileSys.GetFileInfo(*diskPtr,fileIdx);
		if(D77FileSystem::STATE_USED==fInfo.state)
		{
			auto label=fInfo.FormatInfo();
			printf("%s\n",label.c_str());

			auto clusterChain=d77FileSys.GetClusterChain(*diskPtr,fInfo);
			printf("Cluster Chain:");
			for(auto c : clusterChain)
			{
				printf(" %d",c);
			}
			printf("\n");
			if(0<clusterChain.size())
			{
				printf("Last Cluster:%02x\n",d77FileSys.GetClusterConnection(*diskPtr,clusterChain.back()));
			}

			switch(fInfo.fType)
			{
			case FM7File::FTYPE_BASIC_BINARY:
				break;
			case FM7File::FTYPE_BASIC_ASCII:
				break;
			case FM7File::FTYPE_BINARY:
				{
					auto rawFile=d77FileSys.RawFileRead(*diskPtr,fileIdx);
					if(11<=rawFile.size())
					{
						int rawSize=(int)rawFile[1]*256+rawFile[2];
						int startAddr=(int)rawFile[3]*256+rawFile[4];
						printf("Size     =$%04x (%d)\n",rawSize,rawSize);
						printf("StartAddr=$%04x\n",startAddr);

						if(11+rawSize<=rawFile.size())
						{
							int tailInfoIdx=5+rawSize;
							int execAddr=(int)rawFile[tailInfoIdx+3]*256+rawFile[tailInfoIdx+4];
							printf("ExecAddr =$%04x\n",execAddr);
						}
						printf("Expected 2B0 Size=$%04x (%d)\n",rawSize+11,rawSize+11);
					}
				}
				break;
			};
		}
	}
}

void D77FileUtil::DumpDir(int dirPosition) const
{
	auto diskPtr=d77Ptr->GetDisk(diskId);
	if(nullptr!=diskPtr)
	{
		for(int i=0; i<8; ++i)
		{
			auto dirDump=d77FileSys.GetDirDump(*diskPtr,dirPosition+i);
			if(16<=dirDump.size())
			{
				printf("%04d | ",dirPosition+i);
				for(int j=0; j<16; ++j)
				{
					printf("%02x ",dirDump[j]);
				}
				printf("| ");
				for(int j=0; j<16; ++j)
				{
					if(' '<=dirDump[j] && dirDump[j]<128)
					{
						printf("%c",dirDump[j]);
					}
					else
					{
						printf(" ");
					}
				}
				printf("\n");

				printf("     | ");
				for(int j=16; j<32; ++j)
				{
					printf("%02x ",dirDump[j]);
				}
				printf("| ");
				for(int j=16; j<32; ++j)
				{
					if(' '<=dirDump[j] && dirDump[j]<128)
					{
						printf("%c",dirDump[j]);
					}
					else
					{
						printf(" ");
					}
				}
				printf("\n");
			}
		}
	}
}

bool D77FileUtil::RawRead(const char filenameInD77[],const char rawFName[]) const
{
	auto diskPtr=d77Ptr->GetDisk(diskId);
	if(nullptr!=diskPtr)
	{
		auto rawFile=d77FileSys.RawFileRead(*diskPtr,filenameInD77);
		if(0==rawFile.size())
		{
			fprintf(stderr,"Cannot read file from .D77.\n");
		}
		else
		{
			if(true==FM7Lib::WriteBinaryFile(rawFName,rawFile))
			{
				printf("Saved %s\n",rawFName);
				return true;
			}
			else
			{
				fprintf(stderr,"Cannot open %s for write.\n",rawFName);
			}
		}
	}
	return false;
}

bool D77FileUtil::D77FileToFMFile(const char filenameInD77[],const char fmFName[]) const
{
	auto diskPtr=d77Ptr->GetDisk(diskId);
	if(nullptr!=diskPtr)
	{
		auto fileIdx=d77FileSys.FindFile(*diskPtr,filenameInD77);
		if(0<=fileIdx)
		{
			return D77FileIndexToFMFile(fileIdx,fmFName);
		}
		else
		{
			fprintf(stderr,"File not found.\n");
			fprintf(stderr,"Note: File names are case sensitive.\n");
		}
	}
	return false;
}
bool D77FileUtil::D77FileIndexToFMFile(int fileIdx,const char fmFName[]) const
{
	auto diskPtr=d77Ptr->GetDisk(diskId);
	if(nullptr!=diskPtr)
	{
		auto fmFile=d77FileSys.EncodeFMFile(*diskPtr,fileIdx);
		if(0<fmFile.size())
		{
			auto fExt=d77FileSys.GetFMFileExtensionForFile(*diskPtr,fileIdx);
			std::string fmFName2b0=fmFName;
			auto givenExt=FM7Lib::GetExtension(fmFName2b0.c_str());
			FM7Lib::Capitalize(givenExt);
			if(".FM"!=givenExt)
			{
				fmFName2b0=FM7Lib::ChangeExtension(fmFName,fExt);
			}
			if(true!=FM7Lib::WriteBinaryFile(fmFName2b0.c_str(),fmFile))
			{
				fprintf(stderr,"Write error.  The file may be corrupted.\n");
			}
			else
			{
				printf("Saved %s\n",fmFName2b0.c_str());
				return true;
			}
		}
		else
		{
			fprintf(stderr,"File does not exist or deleted.\n");
		}
	}
	return false;
}

bool D77FileUtil::D77FileToBinaryFile(const char filenameInD77[],const char binFName[]) const
{
	auto diskPtr=d77Ptr->GetDisk(diskId);
	if(nullptr!=diskPtr)
	{
		auto fileIdx=d77FileSys.FindFile(*diskPtr,filenameInD77);
		if(0<=fileIdx)
		{
			return D77FileIndexToBinaryFile(fileIdx,binFName);
		}
		else
		{
			fprintf(stderr,"File Not Found\n");
			fprintf(stderr,"Note: File names are case sensitive.\n");
		}
	}
	return false;
}
bool D77FileUtil::D77FileIndexToBinaryFile(int fileIdx,const char binFName[]) const
{
	auto diskPtr=d77Ptr->GetDisk(diskId);
	if(nullptr!=diskPtr)
	{
		auto fInfo=d77FileSys.GetFileInfo(*diskPtr,fileIdx);
		if(D77FileSystem::STATE_USED!=fInfo.state)
		{
			fprintf(stderr,"File is deleted or not found.\n");
			return false;
		}
		
		if(FM7File::FTYPE_BINARY!=fInfo.fType)
		{
			fprintf(stderr,"Bad File Mode\n");
			return false;
		}

		FM7BinaryFile binFile;
		if(true==binFile.Decode2B0(d77FileSys.EncodeFMFile(*diskPtr,fileIdx)))
		{
			printf("Store Address=0x%04x\n",binFile.storeAddr);
			printf("Exec Address =0x%04x\n",binFile.execAddr);
			printf("Size         =%d (0x%04x)\n",(int)binFile.dat.size(),(int)binFile.dat.size());
			if(true!=FM7Lib::WriteBinaryFile(binFName,binFile.dat))
			{
				fprintf(stderr,"Failed to write: %s\n",binFName);
				return false;
			}
			return true;
		}
		else
		{
			fprintf(stderr,"Failed to decode file\n");
			return false;
		}
	}
	return false;
}

bool D77FileUtil::KillFile(const char filenameInD77[])
{
	auto diskPtr=d77Ptr->GetDisk(diskId);
	if(nullptr!=diskPtr)
	{
		if(true==d77FileSys.Kill(*diskPtr,filenameInD77) &&
		   true==d77FileSys.WriteDirectory(*diskPtr))
		{
			return true;
		}
	}
	return false;
}

bool D77FileUtil::WriteFMFile(const char fmFName[],const char c_filenameInD77[])
{
	// Only this writing function can take nullptr as filenameInD77.
	// File name can be taken from fm-file.
	if(nullptr!=c_filenameInD77 && true!=CheckD77FileName(c_filenameInD77))
	{
		return false;
	}

	std::string filenameInD77;
	if(nullptr!=c_filenameInD77)
	{
		filenameInD77=c_filenameInD77;
	}

	auto diskPtr=d77Ptr->GetDisk(diskId);
	if(nullptr!=diskPtr)
	{
		auto dat=FM7Lib::ReadBinaryFile(fmFName);
		if(0<dat.size())
		{
			if(nullptr==c_filenameInD77 || 0==c_filenameInD77[0])
			{
				filenameInD77=FM7Lib::GetFileNameFromFMFile(dat);
			}

			KillIfFileAlradyExists(filenameInD77);

			if(true!=d77FileSys.SaveFMFile(*diskPtr,dat,filenameInD77.c_str()))
			{
				fprintf(stderr,"Failed to write a file.\n");
			}
			else
			{
				return d77FileSys.WriteDirectory(*diskPtr);
			}
		}
		else
		{
			fprintf(stderr,"Cannot read file: %s\n",fmFName);
		}
	}
	return false;
}

bool D77FileUtil::WriteSRECFile(const char srecFName[],const char filenameInD77[])
{
	if(true!=CheckD77FileName(filenameInD77))
	{
		return false;
	}

	auto diskPtr=d77Ptr->GetDisk(diskId);
	if(nullptr!=diskPtr)
	{
		auto srec=FM7Lib::ReadTextFile(srecFName);
		FM7BinaryFile binDat;
		if(0<srec.size() && true==binDat.DecodeSREC(srec))
		{
			KillIfFileAlradyExists(filenameInD77);

			auto fmDat=FM7Lib::BinaryTo2B0(binDat.dat,filenameInD77,binDat.storeAddr,binDat.execAddr);
			if(true!=d77FileSys.SaveFMFile(*diskPtr,fmDat,filenameInD77))
			{
				fprintf(stderr,"Failed to write a file.\n");
			}
			else
			{
				return d77FileSys.WriteDirectory(*diskPtr);
			}
		}
		else
		{
			fprintf(stderr,"Cannot read file: %s\n",srecFName);
		}
	}
	return false;
}

bool D77FileUtil::SaveTextFileAsBASIC(const char txtFName[],const char filenameInD77[])
{
	if(true!=CheckD77FileName(filenameInD77))
	{
		return false;
	}

	auto diskPtr=d77Ptr->GetDisk(diskId);
	if(nullptr!=diskPtr)
	{
		auto text=FM7Lib::ReadTextFile(txtFName);
		if(0<text.size())
		{
			KillIfFileAlradyExists(filenameInD77);

			auto fm0A0=FM7Lib::TextTo0A0(text,filenameInD77);
			if(true!=d77FileSys.SaveFMFile(*diskPtr,fm0A0,filenameInD77))
			{
				fprintf(stderr,"Failed to write a file.\n");
			}
			else
			{
				return d77FileSys.WriteDirectory(*diskPtr);
			}
		}
		else
		{
			fprintf(stderr,"Cannot read file: %s\n",txtFName);
		}
	}

	return false;
}

bool D77FileUtil::SaveBinaryFileAsMachinGo(const char binFName[],const char filenameInD77[],int storeAddr,int execAddr)
{
	if(true!=CheckD77FileName(filenameInD77))
	{
		return false;
	}

	auto diskPtr=d77Ptr->GetDisk(diskId);
	if(nullptr!=diskPtr)
	{
		KillIfFileAlradyExists(filenameInD77);

		auto bin=FM7Lib::ReadBinaryFile(binFName);
		auto fmDat=FM7Lib::BinaryTo2B0(bin,filenameInD77,storeAddr,execAddr);
		if(true!=d77FileSys.SaveFMFile(*diskPtr,fmDat,filenameInD77))
		{
			fprintf(stderr,"Failed to write a file.\n");
		}
		else
		{
			return d77FileSys.WriteDirectory(*diskPtr);
		}
	}
	return false;
}

bool D77FileUtil::CheckD77FileName(const char fName[]) const
{
	return CheckD77FileName(std::string(fName));
}
bool D77FileUtil::CheckD77FileName(const std::string &fName) const
{
	if(8<fName.size())
	{
		fprintf(stderr,"Bad File Descriptor\n");
		fprintf(stderr,"D77 file name must be up to 8 letters.\n");
		return false;
	}
	for(auto c : fName)
	{
		if(c<' ')
		{
			fprintf(stderr,"Bad File Descriptor\n");
			fprintf(stderr,"Control code in the file name.\n");
			return false;
		}
		if('\"'==c || ':'==c || '('==c || ')'==c || '['==c || ']'==c)
		{
			fprintf(stderr,"Bad File Descriptor\n");
			fprintf(stderr,"Unusable character in the file name.\n");
			return false;
		}
	}
	return true;
}

void D77FileUtil::KillIfFileAlradyExists(const char fName[])
{
	auto diskPtr=d77Ptr->GetDisk(diskId);
	if(nullptr!=diskPtr)
	{
		auto fileIdx=d77FileSys.FindFile(*diskPtr,fName);
		if(0<=fileIdx)
		{
			d77FileSys.KillN(*diskPtr,fileIdx);
		}
	}
}
void D77FileUtil::KillIfFileAlradyExists(const std::string &fName)
{
	KillIfFileAlradyExists(fName.c_str());
}

void D77FileUtil::UnWriteProtect(void)
{
	auto diskPtr=d77Ptr->GetDisk(diskId);
	if(nullptr!=diskPtr)
	{
		diskPtr->header.writeProtected=0;
	}
}
void D77FileUtil::WriteProtect(void)
{
	auto diskPtr=d77Ptr->GetDisk(diskId);
	if(nullptr!=diskPtr)
	{
		diskPtr->header.writeProtected=0x10;
	}
}
bool D77FileUtil::SaveDisk(const char d77FName[])
{
	auto curImg=FM7Lib::ReadBinaryFile(d77FName);
	if(0<curImg.size())
	{
		D77File curD77;
		curD77.SetData(curImg,/*verboseMode=*/false);

		if(2<=curD77.GetNumDisk())
		{
			fprintf(stderr,"Cannot write to a multi-disk image.\n");
			return false;
		}

		auto diskPtr=curD77.GetDisk(0);
		if(nullptr!=diskPtr && true==diskPtr->IsWriteProtected())
		{
			fprintf(stderr,"Cannot write to a write-protected image.\n");
			return false;
		}
	}

	return ForceSaveDisk(d77FName);
}

bool D77FileUtil::ForceSaveDisk(const char d77FName[])
{
	auto diskPtr=d77Ptr->GetDisk(diskId);
	if(nullptr!=diskPtr)
	{
		if(true==FM7Lib::WriteBinaryFile(d77FName,diskPtr->MakeD77Image()))
		{
			this->d77FName=d77FName;
			return true;
		}
	}
	return false;
}

int TerminalMode(int ac,char *av[])
{
	D77File d77;
	D77FileUtil term;

	if(2==ac)
	{
		auto dat=FM7Lib::ReadBinaryFile(av[1]);
		if(0==dat.size())
		{
			fprintf(stderr,"Cannot read file: %s\n",av[1]);
			return 1;
		}

		term.d77FName=av[1];
		d77.SetData(dat,/*verboseMode=*/false);
	}
	else
	{
		printf("Entering terminal mode with a new D77 file.\n");
		printf("Type:\n");
		printf("  d77fileutil -h\n");
		printf("for command help.\n");
		d77.CreateStandardFormatted();
	}

	d77.PrintInfo();
	term.Terminal(d77);

	return 0;
}

int CommandMode(int ac,char *av[])
{
	if(ac<3)
	{
		fprintf(stderr,"Too few arguments.\n");
		return 1;
	}

	D77File d77;
	D77FileUtil term;
	return term.CommandMode(d77,ac,av);
}

int main(int ac,char *av[])
{
	if(2<=ac)
	{
		std::string argv1(av[1]);
		FM7Lib::Capitalize(argv1);
		if("-H"==argv1 || "-HELP"==argv1 || "-?"==argv1)
		{
			D77FileUtil::CommandHelp();
			return 1;
		}
	}

	if(ac<=2)
	{
		return TerminalMode(ac,av);
	}
	return CommandMode(ac,av);
}

