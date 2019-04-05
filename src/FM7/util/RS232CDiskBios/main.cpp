#include <list>
#include <chrono>
#include <thread>
#include <vector>
#include <thread>
#include <mutex>
#include <string>


#include <string.h>

#include "disk_bios_hook_client.h"



#include "comport.h"
#include "d77.h"
#include "cpplib.h"
#include "fm7lib.h"



void MainCPU(void);
void SubCPU(void);


enum
{
	SYSTYPE_UNKNOWN,
	SYSTYPE_F_BASIC_3_0,
	SYSTYPE_URADOS
};

const unsigned char BIOS_ERROR_DRIVE_NOT_READY=		0x0A;
const unsigned char BIOS_ERROR_WRITE_PROTECTED=		0x0B;
const unsigned char BIOS_ERROR_HARD_ERROR=			0x0C;
const unsigned char BIOS_ERROR_CRC_ERROR=			0x0D;
const unsigned char BIOS_ERROR_DDM_ERROR=			0x0E;
const unsigned char BIOS_ERROR_TIME_OVER=			0x0F;

////////////////////////////////////////////////////////////

class SectorFilterOption
{
public:
	// Default  JSR $FExx, JMP $FExx
	bool ldxFExx,ldyFExx,lduFExx;

	SectorFilterOption();
};

SectorFilterOption::SectorFilterOption()
{
	ldxFExx=false;
	ldyFExx=false;
	lduFExx=false;
}

////////////////////////////////////////////////////////////

void IdentifyIPL(
	std::vector <unsigned char> &sectorData,
	bool &jmpFBFE,
	bool &ldx6E00, // Signature for F-BASIC 3.0
	bool &ldx4D00, // Signature for URADOS
	int &ldxPtr)
{
	jmpFBFE=false;
	ldx6E00=false;
	ldx4D00=false;
	ldxPtr=0;

	for(int i=0; i<sectorData.size(); ++i)
	{
		if(i+2<sectorData.size() && sectorData[i]==0x8e && sectorData[i+1]==0x4d && sectorData[i+2]==0x00)
		{
			ldx4D00=true;
			ldxPtr=i;
		}
		if(i+2<sectorData.size() && sectorData[i]==0x8e && sectorData[i+1]==0x6e && sectorData[i+2]==0x00)
		{
			ldx6E00=true;
			ldxPtr=i;
		}
		if(i+3<sectorData.size() && sectorData[i]==0x6e && sectorData[i+1]==0x9f && sectorData[i+2]==0xfb && sectorData[i+3]==0xfe)
		{
			jmpFBFE=true;
		}
	}
}

void SetUpClientSecondInstallation(std::vector <unsigned char> &clientCode,D77File::D77Disk &bootDisk)
{
	int jmp6E00Ptr=-1;
	for(int i=0; i<clientCode.size(); ++i)
	{
		if(i+2<clientCode.size() && clientCode[i]==0x7e && clientCode[i+1]==0x6e && clientCode[i+2]==0x00)
		{
			jmp6E00Ptr=i;
			break;
		}
	}

	if(0<=jmp6E00Ptr)
	{
		auto bootSector=bootDisk.ReadSector(0,0,1);

		bool jmpFBFE=false;
		bool ldx6E00=false;
		bool ldx4D00=false;
		int ldxPtr=0;
		IdentifyIPL(bootSector,jmpFBFE,ldx6E00,ldx4D00,ldxPtr);

		// URADOS
		if(true==jmpFBFE && true==ldx4D00)
		{
			//   JMP $6E00 -> JMP $4D00
			clientCode[jmp6E00Ptr+1]=0x4D;
			clientCode[jmp6E00Ptr+2]=0x00;
		}
	}
}

void SetUpClientInstallAddress(
    std::vector <unsigned char> &clientCode,unsigned int instAddr1,unsigned int instAddr2)
{
	clientCode[2]=((instAddr1>>8)&255);
	clientCode[3]= (instAddr1&255);
	clientCode[4]=((instAddr2>>8)&255);
	clientCode[5]= (instAddr2&255);
}

void SetUpClientDosMode(
    std::vector <unsigned char> &clientCode,bool dosMode)
{
	if(true==dosMode)
	{
		for(int i=0; i+6<clientCode.size(); ++i)
		{
			if(clientCode[i  ]==0xB6 && clientCode[i+1]==0xFD && clientCode[i+2]==0x0F)
			{
				clientCode[i  ]=0xB7;
			} 
			if(clientCode[i  ]==0x7E && clientCode[i+1]==0x01 && clientCode[i+2]==0x00)
			{
				clientCode[i+1]=0x03;
			}
			if(clientCode[i  ]==0x0A && // DREAD
			   // clientCode[i+1]==0x00 && // RCBSTA
			   clientCode[i+2]==0x01 && // Buffer Addr High
			   clientCode[i+3]==0x00 && // Buffer Addr Low
			   clientCode[i+4]==0x00 && // Track
			   clientCode[i+5]==0x01 && // Sector
			   clientCode[i+6]==0x00)   // Side
			{
				clientCode[i+2]=0x03;
			}
		}
	}
}

void SubstFExx(unsigned char dat[2],unsigned int fe02Subst,unsigned int fe05Subst,unsigned int fe08Subst)
{
	if(0xFE==dat[0] && 0x02==dat[1])
	{
		dat[0]=(fe02Subst>>8)&0xFF;
		dat[1]=(fe02Subst&0xFF);
	}
	else if(0xFE==dat[0] && 0x05==dat[1])
	{
		dat[0]=(fe05Subst>>8)&0xFF;
		dat[1]=(fe05Subst&0xFF);
	}
	else if(0xFE==dat[0] && 0x08==dat[1])
	{
		dat[0]=(fe08Subst>>8)&0xFF;
		dat[1]=(fe08Subst&0xFF);
	}
}

void AlterSectorData(
	int systemType,
	const SectorFilterOption &opt,
	int track,
	int side,
	int sector,
	std::vector <unsigned char> &dat,
	unsigned int hookInstAddr1,	// For IPL
	unsigned int hookInstAddr2, // For Others
	unsigned int fe02Offset,
	unsigned int fe05Offset,
	unsigned int fe08Offset)
{
	auto hookInstAddr=hookInstAddr2;
	if(0==track && 0==side && 1==sector)
	{
		hookInstAddr=hookInstAddr1;
	}

	auto fe02Subst=hookInstAddr+fe02Offset;
	auto fe05Subst=hookInstAddr+fe05Offset;
	auto fe08Subst=hookInstAddr+fe08Offset;

	if(0==track && 0==side && 1==sector)
	{
		bool jmpFBFE=false;
		bool ldx6E00=false;
		bool ldx4D00=false;
		int ldxPtr=0;
		IdentifyIPL(dat,jmpFBFE,ldx6E00,ldx4D00,ldxPtr);

		// F-BASIC 3.0 IPL
		if(true==jmpFBFE && true==ldx6E00)
		{
			printf("Tweaking IPL for Disk F-BASIC 3.0\n");
			dat[ldxPtr+1]=0x60;
			dat[ldxPtr+2]=0x00;
		}
		// URADOS IPL
		if(true==jmpFBFE && true==ldx4D00)
		{
			printf("Tweaking IPL for URADOS\n");
			dat[ldxPtr+1]=0x40;	// Use one of the clones of the installer.
			dat[ldxPtr+2]=0x00;
		}


		// If FE02, FE05, FE08 are called with LBSR inside IPL, it still can be detected.
		for(int i=0; i<dat.size(); ++i)
		{
			if(i+2<dat.size() && dat[i]==0x17) // LBSR
			{
				unsigned int jmpOffset=dat[i+1]*256+dat[i+2];
				unsigned int jmpAddr=0x100+i+3+jmpOffset;

				if(0xFE08==(jmpAddr&0xffff))
				{
					dat[i  ]=0xbd; // JSR
					dat[i+1]=(fe08Subst>>8)&0xff;
					dat[i+2]= fe08Subst&0xff;
				}
				else if(0xFE05==(jmpAddr&0xffff))
				{
					dat[i  ]=0xbd; // JSR
					dat[i+1]=(fe05Subst>>8)&0xff;
					dat[i+2]= fe05Subst&0xff;
				}
				else if(0xFE02==(jmpAddr&0xffff))
				{
					dat[i  ]=0xbd; // JSR
					dat[i+1]=(fe02Subst>>8)&0xff;
					dat[i+2]= fe02Subst&0xff;
				}
			}
		}
	}


	for(int i=0; i<dat.size(); ++i)
	{
		// JSR $FE0x / JMP $FE0x
		if(i+2<dat.size() && (dat[i]==0xbd || dat[i]==0x7e) && dat[i+1]==0xfe)
		{
			SubstFExx(dat.data()+i+1,fe02Subst,fe05Subst,fe08Subst);
		}
		if(i+2<dat.size() && (dat[i]==0xbd || dat[i]==0x7e) && dat[i+1]==0xF1 && dat[i+2]==0x7D)
		{
			dat[i+1]=(hookInstAddr>>8)&0xff;
			dat[i+2]= hookInstAddr&0xff;
		}
		if(i+3<dat.size() && dat[i+1]==0x9F && dat[i+2]==0xFB && dat[i+3]==0xFA)
		{
			if(dat[i]==0xAD) // JSR [$FBFA]
			{
				dat[i  ]=0xBD;  // JSR
				dat[i+1]=(hookInstAddr>>8)&0xff;
				dat[i+2]= hookInstAddr&0xff;
				dat[i+3]=0x12; // NOP
			}
			else if(dat[i]==0x6E) // JMP [$FBFA]
			{
				dat[i  ]=0x7E;  // JMP
				dat[i+1]=(hookInstAddr>>8)&0xff;
				dat[i+2]= hookInstAddr&0xff;
				dat[i+3]=0x12; // NOP
			}
		}

		if(true==opt.ldxFExx)
		{
			if(i+3<dat.size() && dat[i]!=0x10 && dat[i+1]==0x8E && dat[i+2]==0xFE)
			{
				SubstFExx(dat.data()+i+2,fe02Subst,fe05Subst,fe08Subst);
			}
		}
		if(true==opt.ldyFExx)
		{
			if(i+3<dat.size() && dat[i]==0x10 && dat[i+1]==0x8E && dat[i+2]==0xFE)
			{
				SubstFExx(dat.data()+i+2,fe02Subst,fe05Subst,fe08Subst);
			}
		}
		if(true==opt.lduFExx)
		{
			if(i+2<dat.size() && dat[i]==0xCE && dat[i+1]==0xFE)
			{
				SubstFExx(dat.data()+i+1,fe02Subst,fe05Subst,fe08Subst);
			}
		}


		if(systemType==SYSTYPE_URADOS)
		{
			if(i+3<dat.size() && dat[i]==0x10 && dat[i+1]==0x27 && dat[i+2]==0x42 && dat[i+3]==0xCB)
			{
				// URADOS first failure at 0xBB39.  Calling FE08 as:
				//   LBEQ $FE08
				// but no disk.
				int offset=fe08Subst-0xBB3D;
				dat[i+2]=(offset & 0xff00)>>8;
				dat[i+3]=(offset&0xff);
			}

			if(i+5<dat.size() && 
			   dat[i  ]==0x8E && dat[i+1]==0x7F && dat[i+2]==0xFF &&
			   dat[i+3]==0xBF && dat[i+4]==0x05 && dat[i+5]==0x9D)
			{
				// The following part defines upper limit of the F-BASIC memory.
				//   4DB0 8E 7F FF       LDX   #$7FFF
				//   4DB3 BF 05 9D       STX   $059D
				//   4DB6 9F 45          STX   <$45
				//   4DB8 30 89 FE D4    LEAX  -$012C,X
				//   4DBC 9F 3F          STX   <$3F
				// Need to be made before the BIOS hook.
				auto limit=hookInstAddr2-1;
				dat[i+1]=(limit>>8)&0xFF;
				dat[i+2]=(limit&0xFF);
			}
		}
	}
}


////////////////////////////////////////////////////////////



void ShowOptionHelp(void)
{
	printf("Usage:\n");
	printf("  D77DiskBios d77 file.d77 comPort\n");
	printf("-h, -help, -?\n");
	printf("\tShow this help.\n");
	printf("-install ADDR\n");
	printf("\tSpecify install address of BIOS hook in FM-7.\n");
	printf("\tDefault location is at the back end of the URA RAM (shadow RAM)\n");
	printf("\tADDR must be hexadecimal WITHOUT $ or &H or 0x in front of it.\n");

	printf("-encoder XOR valueInHex\n");
	printf("\tApply XOR filter.\n");
	printf("-encoder NEG|COM\n");
	printf("\tApply NEG|COM filter.\n");

	printf("-ldxFExx\n");
	printf("\tAlter LDX #$FE02, LDX #$FE05, LDX #$FE05\n");
	printf("\tto the BIOS hook address.\n");
	printf("-ldyFExx\n");
	printf("\tAlter LDY #$FE02, LDY #$FE05, LDY #$FE05\n");
	printf("\tto the BIOS hook address.\n");
	printf("-lduFExx\n");
	printf("\tAlter LDU #$FE02, LDU #$FE05, LDU #$FE05\n");
	printf("\tto the BIOS hook address.\n");
}

void ShowCommandHelp(void)
{
	printf("Command Help:\n");
	printf("IA..Transmit BIOS redirector installer to FM-7/77\n");
	printf("Q...Quit.\n");
	printf("H...Show this help.\n");
}

void Title(void)
{
	printf("********************************\n");
	printf("*                              *\n");
	printf("*  D77 Image Server            *\n");
	printf("*  by CaptainYS                *\n");
	printf("*  http://www.ysflight.com     *\n");
	printf("*                              *\n");
	printf("********************************\n");
	printf("Make sure to configure FM-7/77 side computer at 19200bps.\n");
}

void ShowPrompt(void)
{
	printf("Command {? for Help}>");
}

////////////////////////////////////////////////////////////


unsigned int GetDefaultInstallAddress(void)
{
	FM7BinaryFile binFile;
	binFile.DecodeSREC(clientBinary);
	return 0x100*binFile.dat[2]+binFile.dat[3];
}


////////////////////////////////////////////////////////////


class Encoder
{
public:
	enum
	{
		ENC_NONE,
		ENC_XOR, // Byte-by-Byte XOR
		ENC_NEG, // Byte-by-Byte Negate
		ENC_COM, // Byte-by-Byte Compliment
	};

	unsigned int encType;
	unsigned int encKey;

	inline Encoder()
	{
		encType=ENC_NONE;
		encKey=0;
	}
	inline Encoder(unsigned int encType,unsigned int encKey)
	{
		this->encType=encType;
		this->encKey=encKey;
	}
	void Encode(std::vector <unsigned char> &dat) const;
	void Decode(std::vector <unsigned char> &dat) const;
};
void Encoder::Encode(std::vector <unsigned char> &dat) const
{
	switch(encType)
	{
	case ENC_XOR:
		for(auto &u : dat)
		{
			u^=encKey;
		}
		break;
	case ENC_NEG:
		for(auto &u : dat)
		{
			u=((0x100-u)&0xff);
		}
		break;
	case ENC_COM:
		for(auto &u : dat)
		{
			u=~u;
		}
		break;
	}
}
void Encoder::Decode(std::vector <unsigned char> &dat) const
{
	switch(encType)
	{
	case ENC_XOR:
	case ENC_COM:
	case ENC_NEG:
		Encode(dat);
		return;
	}
}

////////////////////////////////////////////////////////////

class DiskFileInfo
{
public:
	// If drive 1 is index!=0 of the same file as drive 0, fName may be !=saveFName.
	// Otherwise fName=saveFName by default.
	// For saving, saveFName must be used.
	std::string fName;
	std::string saveFName;
	bool writeProtect;
	bool newFile;
	bool forceNewFile;
	int diskIdx;
	int reuseDisk;

	DiskFileInfo();
	void CleanUp(void);
	bool Recognize(const std::string &fName);
};

DiskFileInfo::DiskFileInfo()
{
	CleanUp();
}
void DiskFileInfo::CleanUp(void)
{
	fName="";
	newFile=false;
	forceNewFile=false;
	writeProtect=false;
	diskIdx=0;
	reuseDisk=-1;
}
bool DiskFileInfo::Recognize(const std::string &fName)
{
	// diskimage.d77#NEW    Create new disk.  Error if file already exists.
	// diskimage.d77#FNEW   Force create new disk.
	// diskimage.d77#WP     Write protect.
	// diskimage.d77#1      Use disk index 1 of a multi-disk image.
	// diskimage.d77#1#WP   Use disk index 1 of a multi-disk image.  Also write protect.
	// #DSK0#1              Use same disk file as D77[0] index=1
	CleanUp();

	int EON=fName.size();
	for(int i=0; i<fName.size(); ++i)
	{
		auto ext=fName.substr(i,4);
		FM7Lib::Capitalize(ext);
		if(".D77"==ext || ".D88"==ext)
		{
			EON=i+4;
			break;
		}
	}

	this->fName=fName.substr(0,EON);
	this->saveFName=fName.substr(0,EON);

	auto opt=fName.substr(EON);
	FM7Lib::Capitalize(opt);

	int ptr=0;
	for(;;)
	{
		if(0==opt[ptr])
		{
			break;
		}
		else if('#'==opt[ptr])
		{
			++ptr;

			std::string oneOpt;
			while(ptr<opt.size() && opt[ptr]!='#')
			{
				oneOpt.push_back(opt[ptr++]);
			}

			if("NEW"==oneOpt)
			{
				newFile=true;
				forceNewFile=false;
			}
			else if("FNEW"==oneOpt)
			{
				newFile=true;
				forceNewFile=true;
			}
			else if("WP"==oneOpt)
			{
				writeProtect=true;
			}
			else if('0'<=oneOpt[0] && oneOpt[0]<='9')
			{
				diskIdx=FM7Lib::Atoi(oneOpt.c_str());
			}
			else if("DSK0"==oneOpt)
			{
				reuseDisk=0;
			}
			else
			{
				printf("Unrecognized D77 file-name option (%s).\n",oneOpt.c_str());
				printf("Given: %s\n",opt.c_str());
				return false;
			}
		}
		else
		{
			printf("D77 file-name option must start with #.\n");
			printf("Given: %s\n",opt.c_str());
			return false;
		}
	}

	if(true==newFile && 0<=reuseDisk)
	{
		fprintf(stderr,"#DSK0 and #NEW cannot be combined.\n");
		return false;
	}
	if(0<this->fName.size() && 0<=reuseDisk)
	{
		fprintf(stderr,"D77 file name and #DSK0 cannot be combined.\n");
		return false;
	}

	return true;
}

////////////////////////////////////////////////////////////


class FM7Disk
{
public:
	DiskFileInfo fileInfo;
	D77File *filePtr;
	D77File::D77Disk *diskPtr;

	FM7Disk();
	~FM7Disk();
	void CleanUp();
};

FM7Disk::FM7Disk()
{
	CleanUp();
}
FM7Disk::~FM7Disk()
{
	CleanUp();
}
void FM7Disk::CleanUp()
{
	diskPtr=nullptr;
}


////////////////////////////////////////////////////////////


class DiskSet
{
public:
	FM7Disk fm7Disk[2];
	std::list <D77File> d77File;

	bool LoadDiskSet(const std::string fName0,const std::string fName1);
	bool LoadDisk(int idx,const std::string fName);
	void CollectGarbage(void);
};

bool DiskSet::LoadDiskSet(const std::string fName0,const std::string fName1)
{
	if(true!=LoadDisk(0,fName0))
	{
		printf("Cannot load %s\n",fName0.c_str());
		return false;
	}
	if(true!=LoadDisk(1,fName1))
	{
		printf("Cannot load %s\n",fName1.c_str());
		return false;
	}
	if(0==d77File.size())
	{
		printf("No disk images loaded.\n");
		return false;
	}

	if(nullptr==fm7Disk[1].diskPtr && nullptr!=fm7Disk[0].filePtr && nullptr!=fm7Disk[0].filePtr->GetDisk(1))
	{
		fm7Disk[1].fileInfo=fm7Disk[0].fileInfo;
		fm7Disk[1].filePtr=fm7Disk[0].filePtr;
		fm7Disk[1].diskPtr=fm7Disk[0].filePtr->GetDisk(1);
	}

	return true;
}

bool DiskSet::LoadDisk(int idx,const std::string fName)
{
	if(""==fName)
	{
		return true;
	}

	if(true!=fm7Disk[idx].fileInfo.Recognize(fName))
	{
		return false;
	}

	if((0==idx && 0<=fm7Disk[idx].fileInfo.reuseDisk) ||
	   (idx==fm7Disk[idx].fileInfo.reuseDisk))
	{
		fprintf(stderr,"Use-same-D77file option is given wrong.\n");
		fprintf(stderr,"This option can be used for non-zero drive only.\n");
		return false;
	}

	D77File *filePtr=nullptr;
	D77File::D77Disk *diskPtr=nullptr;

	if(0>fm7Disk[idx].fileInfo.reuseDisk)
	{
		D77File f;
		this->d77File.push_back(f);
		filePtr=&this->d77File.back();
	}
	else
	{
		auto reuseDisk=fm7Disk[idx].fileInfo.reuseDisk;
		filePtr=fm7Disk[reuseDisk].filePtr;
		fm7Disk[idx].fileInfo.saveFName=fm7Disk[reuseDisk].fileInfo.fName;
	}


	if(true==fm7Disk[idx].fileInfo.newFile)
	{
		if(true!=fm7Disk[idx].fileInfo.forceNewFile)
		{
			FILE *tst=fopen(fm7Disk[idx].fileInfo.fName.c_str(),"rb");
			if(nullptr!=tst)
			{
				fclose(tst);
				printf("Disk file already exists.\n");
				printf("Use #FNEW if you want to force overwrite.\n");
				return false;
			}
		}
		this->d77File.back().CreateStandardFormatted();
		printf("Disk created.\n");
		printf("Will not be actually saved to a D77 file until something is written.\n");

		diskPtr=d77File.back().GetDisk(0);
	}
	else
	{
		if(0>fm7Disk[idx].fileInfo.reuseDisk)
		{
			auto bin=FM7Lib::ReadBinaryFile(fm7Disk[idx].fileInfo.fName.c_str());
			if(0==bin.size())
			{
				fprintf(stderr,"Cannot open .D77 file.\n");
				return false;
			}

			filePtr->SetData(bin);
		}
		diskPtr=filePtr->GetDisk(fm7Disk[idx].fileInfo.diskIdx);
		if(nullptr==diskPtr)
		{
			fprintf(stderr,"Disk index out of range.\n");
			fprintf(stderr,"This disk image has %d disks.\n",(int)filePtr->GetNumDisk());
			fprintf(stderr,"Given index=%d\n",(int)fm7Disk[idx].fileInfo.diskIdx);
			return false;
		}
	}

	fm7Disk[idx].filePtr=filePtr;
	fm7Disk[idx].diskPtr=diskPtr;

	if(nullptr!=diskPtr && true==fm7Disk[idx].fileInfo.writeProtect)
	{
		diskPtr->SetWriteProtected();
	}

	return true;
}

void DiskSet::CollectGarbage(void)
{
}

////////////////////////////////////////////////////////////


class D77ServerCommandParameterInfo
{
public:
	std::string portStr;
	std::string d77FName[2];

	bool instAddrSpecified;
	unsigned int instAddr;

	std::vector <Encoder> encoder;

	/* In most cases, instAddr and instAddr2 are the same.

	   For Disk BASIC based images, the BIOS Hook is installed twice.
	   First while IPL is loaded and the IPL is loading the Disk BASIC.

	   Then the IPL will set call-back address in X and calls [$FBFE].
	   F-BASIC will clear everything before X and then call X.

	   Since the installed Hook is cleared, this X is tweaked so that 
	   the hook-installer is called for the second time, from there it 
	   calls the original call-back address.

	   If the Disk BASIC based title uses up pretty much entire conventional
	   RAM and FC00-FC7F, only remaining location for the hook to be installed
	   is $7F25 where Disk BASIC error messages are stored.  Of course, the
	   system will crash if there is a disk error.  But, majority of the
	   titles never show an error.

	   But, $7F25 to $7FC4 will be overwritten by Disk BASIC IPL.
	   Therefore the hook needs to reside somewhere else in the first 
	   installation.

	   For this purpose, instAddr2 is introduced.  In such applications,
	   instAddr can be set to like $FC00.  Then, instAddr2 can be set to $7F25.
	*/
	bool instAddr2Specified;
	unsigned int instAddr2;

	bool dosMode;

	SectorFilterOption filterOpt;

	D77ServerCommandParameterInfo();
	bool Recognize(int ac,char *av[]);
	void CleanUp(void);

	void FinalizeInstallAddress(D77File::D77Disk *bootDiskPtr);
};

D77ServerCommandParameterInfo::D77ServerCommandParameterInfo()
{
	CleanUp();
}

void D77ServerCommandParameterInfo::CleanUp(void)
{
	portStr="";
	d77FName[0]="";
	d77FName[1]="";

	instAddrSpecified=false;
	instAddr=GetDefaultInstallAddress();

	instAddr2Specified=false;
	instAddr2=GetDefaultInstallAddress();

	dosMode=false;

	encoder.push_back(Encoder());
}

bool D77ServerCommandParameterInfo::Recognize(int ac,char *av[])
{
	instAddrSpecified=false;
	instAddr2Specified=false;

	if(ac<=1)
	{
		ShowOptionHelp();
		return false;
	}

	int fixedOrderIndex=0;
	for(int i=1; i<ac; ++i)
	{
		std::string arg(av[i]);
		FM7Lib::Capitalize(arg);
		if("-H"==arg || "-HELP"==arg || "-?"==arg)
		{
			ShowOptionHelp();
		}
		else if("-INSTALL"==arg)
		{
			if(i+1<ac)
			{
				instAddr=FM7Lib::Xtoi(av[i+1]);
				instAddrSpecified=true;
				++i;
			}
			else
			{
				fprintf(stderr,"Insufficient parameter for -install.\n");
				return false;
			}
		}
		else if("-INSTALL2"==arg)
		{
			if(i+1<ac)
			{
				instAddr2=FM7Lib::Xtoi(av[i+1]);
				instAddr2Specified=true;
				++i;
			}
			else
			{
				fprintf(stderr,"Insufficient parameter for -install2.\n");
				return false;
			}
		}
		else if("-DOSMODE"==arg)
		{
			dosMode=true;
		}
		else if("-ENCODER"==arg)
		{
			if(i+1<ac)
			{
				std::string encTypeStr(av[i+1]);
				FM7Lib::Capitalize(encTypeStr);
				if("XOR"==encTypeStr)
				{
					if(i+2<ac)
					{
						unsigned int encKey=FM7Lib::Xtoi(av[i+2]);
						encoder.push_back(Encoder(Encoder::ENC_XOR,encKey));
						i+=2;
					}
					else
					{
						fprintf(stderr,"Insufficient parameter for -encoder xor/neg/com.\n");
						return false;
					}
				}
				else if("NEG"==encTypeStr)
				{
					encoder.push_back(Encoder(Encoder::ENC_NEG,0));
					++i;
				}
				else if("COM"==encTypeStr)
				{
					encoder.push_back(Encoder(Encoder::ENC_COM,0));
					++i;
				}
			}
		}
		else if("-LDXFEXX"==arg)
		{
			filterOpt.ldxFExx=true;
		}
		else if("-LDYFEXX"==arg)
		{
			filterOpt.ldyFExx=true;
		}
		else if("-LDUFEXX"==arg)
		{
			filterOpt.lduFExx=true;
		}
		else if("-1"==arg && i+1<=ac)
		{
			d77FName[1]=av[i+1];
			++i;
		}
		else if('-'==arg[0])
		{
			printf("Unknown option: %s\n",arg.c_str());
			return false;
		}
		else
		{
			switch(fixedOrderIndex)
			{
			case 0:
				d77FName[0]=arg;
				break;
			case 1:
				portStr=arg;
				break;
			}
			++fixedOrderIndex;
		}
	}

	if(fixedOrderIndex<2)
	{
		return false;
	}

	return true;
}

void D77ServerCommandParameterInfo::FinalizeInstallAddress(D77File::D77Disk *bootDiskPtr)
{
	if(nullptr!=bootDiskPtr && true!=instAddrSpecified)
	{
		auto sectorData=bootDiskPtr->ReadSector(0,0,1);

		bool jmpFBFE=false;
		bool ldx6E00=false;
		bool ldx4D00=false;
		int ldxPtr=0;
		IdentifyIPL(sectorData,jmpFBFE,ldx6E00,ldx4D00,ldxPtr);

		// URADOS
		if(true==jmpFBFE && true==ldx4D00)
		{
			// Cannot use $FC00-FC7F
			instAddr=0x7FA0;
		}
	}
	printf("Bios Hook Install Address=%04x\n",instAddr);

	if(true!=instAddr2Specified)
	{
		instAddr2=instAddr;
	}
	else
	{
		printf("Bios Hook Secondary Install Address=%04x\n",instAddr2);
	}
}

////////////////////////////////////////////////////////////


static std::mutex fd05;


class FC80
{
public:
	bool terminate;
	bool subCPUready;
	bool fatalError;
	bool verbose;
	bool installASCII;


	int systemType;
	DiskSet diskSet;

	std::chrono::time_point<std::chrono::system_clock> lastByteReceivedClock;
	bool tapeSaved;

	D77ServerCommandParameterInfo cpi;



	FC80()
	{
		systemType=SYSTYPE_UNKNOWN;

		terminate=false;
		subCPUready=false;

		verbose=false;
		installASCII=false;

		lastByteReceivedClock=std::chrono::system_clock::now();
	}

	bool GetSubCPUReady(void) const
	{
		std::lock_guard <std::mutex> lock(fd05);
		return subCPUready;
	}
	void SetFatalError(void)
	{
		std::lock_guard <std::mutex> lock(fd05);
		fatalError=true;
	}
	bool GetFatalError(void) const
	{
		std::lock_guard <std::mutex> lock(fd05);
		return fatalError;
	}
	void SetTerminate(bool t)
	{
		std::lock_guard <std::mutex> lock(fd05);
		terminate=t;
	}
	bool GetTerminate(void) const
	{
		std::lock_guard <std::mutex> lock(fd05);
		return terminate;
	}
	void SetVerboseMode(bool v)
	{
		std::lock_guard <std::mutex> lock(fd05);
		verbose=v;
	}
	bool GetVerboseMode(void) const
	{
		std::lock_guard <std::mutex> lock(fd05);
		return verbose;
	}
	void InstallASCII(void)
	{
		std::lock_guard <std::mutex> lock(fd05);
		installASCII=true;
	}
	void SaveD77(const char fName[],int diskId) const
	{
		std::lock_guard <std::mutex> lock(fd05);
	}

	D77File::D77Disk *GetDiskFromBiosCmd(const unsigned char biosCmd[])
	{
		// FM-7 BIOS takes &3.
		auto d=(biosCmd[7]&3);
		if(d<2)
		{
			return diskSet.fm7Disk[d].diskPtr;
		}
		return nullptr;
	}

	void Halt(void)
	{
		fd05.lock();
	}
	void Unhalt(void)
	{
		fd05.unlock();
	}

	void IdentifySystemType(D77File::D77Disk *bootDiskPtr);
};

void FC80::IdentifySystemType(D77File::D77Disk *bootDiskPtr)
{
	if(nullptr!=bootDiskPtr)
	{
		auto sectorData=bootDiskPtr->ReadSector(0,0,1);

		bool jmpFBFE=false;
		bool ldx6E00=false;
		bool ldx4D00=false;
		int ldxPtr=0;
		IdentifyIPL(sectorData,jmpFBFE,ldx6E00,ldx4D00,ldxPtr);

		// URADOS
		if(true==jmpFBFE && true==ldx4D00)
		{
			printf("Identified URADOS IPL\n");
			systemType=SYSTYPE_URADOS;
		}
		else if(true==jmpFBFE && true==ldx6E00)
		{
			printf("Identified F-BASIC 3.0 IPL\n");
			systemType=SYSTYPE_F_BASIC_3_0;
		}
	}
}

static FC80 fc80;


////////////////////////////////////////////////////////////


char *Fgets(char str[],int len,FILE *fp)
{
	auto ret=fgets(str,len,fp);
	if(nullptr!=ret)
	{
		for(int i=0; str[i]!=0; ++i)
		{
			if(str[i]<' ')
			{
				str[i]=0;
				break;
			}
		}
	}
	return ret;
}

int main(int ac,char *av[])
{
	if(true!=fc80.cpi.Recognize(ac,av))
	{
		return 1;
	}

	if(true!=fc80.diskSet.LoadDiskSet(fc80.cpi.d77FName[0],fc80.cpi.d77FName[1]))
	{
		fprintf(stderr,"Disk Image error.\n");
		return 1;
	}

	Title();

	fc80.IdentifySystemType(fc80.diskSet.fm7Disk[0].diskPtr);
	fc80.cpi.FinalizeInstallAddress(fc80.diskSet.fm7Disk[0].diskPtr);

	std::thread t(SubCPU);
	MainCPU();
	t.join();

	return 0;
}

void MainCPU(void)
{
	while(true!=fc80.GetSubCPUReady())
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}

	for(;;)
	{
		if(true==fc80.GetFatalError())
		{
			return;
		}

		ShowPrompt();

		char cmdIn[256];
		Fgets(cmdIn,255,stdin);

		std::string CMD(cmdIn);
		FM7Lib::Capitalize(CMD);

		bool processed=false;
		switch(CMD[0])
		{
		case 'H':
		case '?':
			ShowCommandHelp();
			processed=true;
			break;
		case 'I':
			if("IA"==CMD)
			{
				fc80.InstallASCII();
				processed=true;
			}
			break;
		case 'Q':
			fc80.SetTerminate(true);
			processed=true;
			return;
		case 'V':
			fc80.SetVerboseMode(true==fc80.GetVerboseMode() ? false : true);
			printf("VerboseMode=%s\n",FM7Lib::BoolToStr(fc80.GetVerboseMode()));
			processed=true;
			break;
		}
		if(true!=processed)
		{
			printf("Syntax Error\n");
			printf("  %s\n",cmdIn);
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(5));
	}
}

static void SaveOneByte(unsigned char c);

void SubCPU(void)
{
	YsCOMPort comPort;
	bool activity=false;
	auto activityTimer=std::chrono::system_clock::now();
	auto lastSentTimer=std::chrono::system_clock::now();

	comPort.SetDesiredBaudRate(19200);
	comPort.SetDesiredBitLength(YsCOMPort::BITLENGTH_8);
	comPort.SetDesiredStopBit(YsCOMPort::STOPBIT_1);
	comPort.SetDesiredParity(YsCOMPort::PARITY_NOPARITY);
	comPort.SetDesiredFlowControl(YsCOMPort::FLOWCONTROL_NONE);
	if(true!=comPort.Open(FM7Lib::Atoi(fc80.cpi.portStr.c_str())))
	{
		fprintf(stderr,"Cannot open port.\n");
		fc80.SetFatalError();
		return;
	}

	enum STATE
	{
		STATE_NORMAL,
		STATE_WAIT_WRITE_DATA,
	};
	STATE state=STATE_NORMAL;

	int biosCmdFilled=0;
	unsigned char biosCmdBuf[8];
	int sectorDataFilled=0;
	int sectorDataNeeded=256;
	unsigned char sectorDataBuf[1024];


	FM7BinaryFile clientCode;
	clientCode.DecodeSREC(clientBinary);
	if(nullptr!=fc80.diskSet.fm7Disk[0].diskPtr)
	{
		SetUpClientSecondInstallation(clientCode.dat,*fc80.diskSet.fm7Disk[0].diskPtr);
	}
	SetUpClientInstallAddress(clientCode.dat,fc80.cpi.instAddr,fc80.cpi.instAddr2);
	SetUpClientDosMode(clientCode.dat,fc80.cpi.dosMode);


	fc80.Halt();
	fc80.subCPUready=true;
	fc80.Unhalt();
	for(;;)
	{
		if(true==fc80.GetTerminate())
		{
			break;
		}

		bool verboseMode=fc80.GetVerboseMode();
		fc80.Halt();

		if(true==fc80.installASCII)
		{
			printf("Install Addr=%04x\n",fc80.cpi.instAddr);
			if(fc80.cpi.instAddr!=fc80.cpi.instAddr2)
			{
				printf("Secondary Install Addr=%04x\n",fc80.cpi.instAddr2);
			}

			printf("\nTransmitting Installer ASCII\n");
			int ctr=0;
			for(auto c : clientCode.dat)
			{
				char str[256];
				sprintf(str,"%d%c%c",(unsigned int)c,0x0d,0x0a);
				comPort.Send(strlen(str),(unsigned char *)str);
				std::this_thread::sleep_for(std::chrono::milliseconds(15));
				ctr++;
				if(ctr%16==0)
				{
					printf("%d/256\n",ctr);
				}
			}
			for(auto i=clientCode.dat.size(); i<256; ++i)
			{
				unsigned char zero[]={'0',0x0d,0x0a};
				comPort.Send(3,zero);
				std::this_thread::sleep_for(std::chrono::milliseconds(15));
				ctr++;
				if(ctr%16==0)
				{
					printf("%d/256\n",ctr);
				}
			}
			printf("Transmition finished.\n");
			printf("Do EXEC &H6000 on FM-7/77\n");
			fc80.installASCII=false;
		}


		auto recv=comPort.Receive();
		for(auto c : recv)
		{
			// If something incoming, don't sleep next 10ms.
			activity=true;
			activityTimer=std::chrono::system_clock::now()+std::chrono::milliseconds(100);

			switch(state)
			{
			case STATE_NORMAL:
				if(true==verboseMode)
				{
					printf("[%02x]",c);
				}
				biosCmdBuf[biosCmdFilled++]=c;
				if(8==biosCmdFilled)
				{
					if(0x09==biosCmdBuf[0]) // Disk Write
					{
						state=STATE_WAIT_WRITE_DATA;
						sectorDataFilled=0;
						sectorDataNeeded=256;

						int track=biosCmdBuf[4];
						int sector=biosCmdBuf[5];
						int side=(biosCmdBuf[6]&1);

						auto diskPtr=fc80.GetDiskFromBiosCmd(biosCmdBuf);
						if(nullptr!=diskPtr)
						{
							auto sectorData=diskPtr->ReadSector(track,side,sector);
							unsigned int sectorSize=sectorData.size();
							if(1024<sectorSize)
							{
								sectorSize=1024;
							}
							else if(sectorSize<128)
							{
								sectorSize=128;
							}
							const unsigned char sendBuf[1];
							sendBuf[0]=(sectorSize>>7);
							comPort.Send(1,sendBuf);

							sectorDataFilled=0;
							sectorDataNeeded=sectorSize;
						}
						else
						{
							const unsigned char sendBuf[1]={1};  // Make it fake 128-byte sector
							comPort.Send(1,sendBuf);
							sectorDataFilled=0;
							sectorDataNeeded=128;
						}
					}
					else if(0x0a==biosCmdBuf[0]) // Disk Read
					{
						int track=biosCmdBuf[4];
						int sector=biosCmdBuf[5];
						int side=(biosCmdBuf[6]&1);

						printf("R Trk:%d Sid:%d Sec:%d\n",track,side,sector);

						auto diskPtr=fc80.GetDiskFromBiosCmd(biosCmdBuf);
						if(nullptr!=diskPtr)
						{
							auto sectorData=diskPtr->ReadSector(track,side,sector);

							if(0==fc80.cpi.encoder.size())
							{
								printf("No encoder!\n");
							}
							for(auto encoder : fc80.cpi.encoder)
							{
								encoder.Decode(sectorData);

								AlterSectorData(
									fc80.systemType,
									fc80.cpi.filterOpt,
									track,side,sector,
								    sectorData,
								    fc80.cpi.instAddr,
								    fc80.cpi.instAddr2,
								    clientCode.dat[clientCode.dat.size()-3],
								    clientCode.dat[clientCode.dat.size()-2],
								    clientCode.dat[clientCode.dat.size()-1]);

								encoder.Encode(sectorData);
							}

							unsigned int sectorSize=sectorData.size();
							sectorSize>>=7;
							const unsigned char sendBuf[1];
							sendBuf[0]=sectorSize;
							comPort.Send(1,sendBuf);

							comPort.Send(sectorData.size(),sectorData.data());

							unsigned char errCode[1]={0};
							if(true==diskPtr->GetDDM(track,side,sector))
							{
								errCode[0]=BIOS_ERROR_DDM_ERROR;
							}
							if(true==diskPtr->GetCRCError(track,side,sector))
							{
								errCode[0]=BIOS_ERROR_CRC_ERROR;
							}
							comPort.Send(1,errCode);
						}
						else
						{
							unsigned char sendBuf[130]={1};  // Make it fake 128-byte sector
							sendBuf[0]=1;
							for(int i=0; i<128; ++i)
							{
								sendBuf[1+i]=0;
							}
							sendBuf[129]=BIOS_ERROR_HARD_ERROR;
							comPort.Send(130,sendBuf);
						}
					}
					biosCmdFilled=0;
				}
				break;
			case STATE_WAIT_WRITE_DATA:
				if(true==verboseMode)
				{
					printf("(%02x)",c);
				}
				{
					int track=biosCmdBuf[4];
					int sector=biosCmdBuf[5];
					int side=(biosCmdBuf[6]&1);

					sectorDataBuf[sectorDataFilled++]=c;
					if(sectorDataFilled==sectorDataNeeded)
					{
						printf("W Trk:%d Sid:%d Sec:%d\n",track,side,sector);

						auto diskPtr=fc80.GetDiskFromBiosCmd(biosCmdBuf);
						if(nullptr!=diskPtr)
						{
							if(true==diskPtr->IsWriteProtected())
							{
								const unsigned char errCode[]={BIOS_ERROR_WRITE_PROTECTED};
								comPort.Send(1,errCode);
							}
							else
							{
								auto written=diskPtr->WriteSector(track,side,sector,sectorDataFilled,sectorDataBuf);
								if(0<written)
								{
									const unsigned char errCode[]={0};
									comPort.Send(1,errCode);
									diskPtr->SetModified();
								}
								else
								{
									const unsigned char errCode[]={BIOS_ERROR_HARD_ERROR};
									comPort.Send(1,errCode);
								}
							}
						}
						else
						{
							const unsigned char errCode[]={BIOS_ERROR_DRIVE_NOT_READY};
							comPort.Send(1,errCode);
						}
						state=STATE_NORMAL;
					}
				}
				break;
			}
			fc80.lastByteReceivedClock=std::chrono::system_clock::now();
		}
		fc80.Unhalt();

		for(auto &fm7Disk : fc80.diskSet.fm7Disk)
		{
			if(nullptr!=fm7Disk.filePtr &&
			   nullptr!=fm7Disk.diskPtr &&
			   true==fm7Disk.diskPtr->IsModified() &&
			   std::chrono::milliseconds(50)<std::chrono::system_clock::now()-fc80.lastByteReceivedClock)
			{
				printf("Received data.  Auto-Saving\n");

				bool res=true;
				FILE *fp=fopen(fm7Disk.fileInfo.saveFName.c_str(),"wb");
				if(nullptr!=fp)
				{
					for(int i=0; i<fm7Disk.filePtr->GetNumDisk(); ++i)
					{
						auto img=fm7Disk.filePtr->GetDisk(i)->MakeD77Image();
						auto wrote=fwrite(img.data(),1,img.size(),fp);
						if(wrote!=img.size())
						{
							fprintf(stderr,"Warning! Failed to save disk image!\n");
							res=false;
						}
					}
					fclose(fp);
				}
				else
				{
					fprintf(stderr,"Warning! Auto-Save failed!\n");
					res=false;
				}
				if(true==res)
				{
					printf("Auto-Saved %s\n",fm7Disk.fileInfo.saveFName.c_str());
				}

				for(int i=0; i<fm7Disk.filePtr->GetNumDisk(); ++i)
				{
					fm7Disk.filePtr->GetDisk(i)->ClearModified();
				}
				ShowPrompt();
			}
		}


		if(activityTimer<std::chrono::system_clock::now())
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
			if(true==activity)
			{
				activity=false;
				ShowPrompt();
			}
		}
	}

	comPort.Close();
}
