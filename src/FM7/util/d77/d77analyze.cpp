#include <stdio.h>
#include <stdlib.h>

#include <vector>
#include <string>

#include "d77.h"
#include "../lib/cpplib.h"



////////////////////////////////////////////////////////////

class D77Analyzer
{
public:
	int diskId;
	bool quit;
	D77File *d77Ptr;
	std::string fName;

	int lastDumpTrk,lastDumpSide,lastDumpSec;

	D77Analyzer();

	void Terminal(D77File &d77);
	void ProcessCommand(const std::vector <std::string> &argv);
	void Help(void) const;
	void DumpSector(int diskId,int cyl,int side,int sec) const;
	bool MoveToNextSector(int diskId,int &cyl,int &side,int &sec) const;
	void DiagnoseDuplicateSector(int diskId) const;
	void FindTrackWithSector(int diskId,int sectorId) const;
	void DeleteDuplicateSector(int diskId);
	void DeleteSectorWithId(int diskId,int sectorId);
	void FindData(int diskId,const char str[]) const;
	void ReplaceData(int diskId,const char fromStr[],const char toStr[]);
	void StoreData(int diskId,int trk,int sid,int sec,int addr,const char toStr[]);
	void RenumberSector(int diskId,int track,int side,int secFrom,int secTo) const;
	void ResizeSector(int diskId,int track,int side,int sec,int newSize);
	void SetSectorCRCError(int diskId,int track,int side,int sec,int newError);
	void SetSectorDDM(int diskId,int track,int side,int secId,bool ddm);
	bool FormatTrack(int diskId,int track,int side,int nSec,int secSize);
	bool UnformatTrack(int diskId,int track,int side);
	void Compare(const std::vector <std::string> &argv) const;
};

D77Analyzer::D77Analyzer()
{
	lastDumpTrk=0;
	lastDumpSide=0;
	lastDumpSec=1;
	diskId=0;
	quit=false;
}

void D77Analyzer::Terminal(D77File &d77)
{
	diskId=0;
	d77Ptr=&d77;
	quit=false;
	while(true!=quit)
	{
		printf("Command H:Help>");
		char cmd[256];
		fgets(cmd,255,stdin);

		auto argv=D77File::QuickParser(cmd);
		ProcessCommand(argv);
	}
}

void D77Analyzer::ProcessCommand(const std::vector <std::string> &argv)
{
	if(0==argv.size())
	{
		return;
	}
	auto cmd=argv[0];
	FM7Lib::Capitalize(cmd);
	if(cmd[0]=='H')
	{
		Help();
	}
	else if('0'==cmd[0])
	{
		diskId=0;
		printf("Current Disk=%d\n",diskId);
	}
	else if('1'==cmd[0])
	{
		if(nullptr!=d77Ptr->GetDisk(1))
		{
			diskId=1;
		}
		printf("Current Disk=%d\n",diskId);
	}
	else if('2'==cmd[0])
	{
		if(nullptr!=d77Ptr->GetDisk(2))
		{
			diskId=2;
		}
		printf("Current Disk=%d\n",diskId);
	}
	else if('3'==cmd[0])
	{
		if(nullptr!=d77Ptr->GetDisk(3))
		{
			diskId=3;
		}
		printf("Current Disk=%d\n",diskId);
	}
	else if('L'==cmd[0])
	{
		auto diskPtr=d77Ptr->GetDisk(diskId);
		if(nullptr!=diskPtr)
		{
			if(3<=argv.size())
			{
				auto track=FM7Lib::Atoi(argv[1].data());
				auto side=FM7Lib::Atoi(argv[2].data());
				auto trackPtr=diskPtr->GetTrack(track,side);
				if(nullptr!=trackPtr)
				{
					trackPtr->PrintDetailedInfo();
				}
				else
				{
					printf("Track not found.\n");
				}
			}
			else
			{
				diskPtr->PrintInfo();
			}
		}
	}
	else if("C"==cmd)
	{
		if(2<=argv.size())
		{
			Compare(argv);
		}
	}
	else if('D'==cmd[0])
	{
		if(4<=argv.size())
		{
			auto cyl=FM7Lib::Atoi(argv[1].data());
			auto side=FM7Lib::Atoi(argv[2].data());
			auto sec=FM7Lib::Atoi(argv[3].data());
			DumpSector(diskId,cyl,side,sec);
			lastDumpTrk=cyl;
			lastDumpSide=side;
			lastDumpSec=sec;
		}
		else if(1==argv.size())
		{
			if(true==MoveToNextSector(diskId,lastDumpTrk,lastDumpSide,lastDumpSec))
			{
				DumpSector(diskId,lastDumpTrk,lastDumpSide,lastDumpSec);
			}
			else
			{
				printf("No more sector.\n");
			}
		}
		else
		{
			printf("Too few arguments.\n");
		}
	}
	else if('F'==cmd[0] && 2<=argv.size())
	{
		FindData(diskId,argv[1].c_str());
	}
	else if('R'==cmd[0] && 3<=argv.size())
	{
		ReplaceData(diskId,argv[1].c_str(),argv[2].c_str());
	}
	else if('S'==cmd[0] && 6<=argv.size())
	{
		// S trk side sec addr "pattern"
		// S trk side sec addr 8614BDE000
		auto trk=FM7Lib::Atoi(argv[1].c_str());
		auto sid=FM7Lib::Atoi(argv[2].c_str());
		auto sec=FM7Lib::Atoi(argv[3].c_str());
		auto addr=FM7Lib::Atoi(argv[4].c_str());
		StoreData(diskId,trk,sid,sec,addr,argv[5].c_str());
	}
	else if('X'==cmd[0])
	{
		if(3<=argv.size() && 'D'==argv[1][0] && 0==strcmp(argv[2].c_str(),"DS"))
		{
			DiagnoseDuplicateSector(diskId);
		}
		else if(4<=argv.size() && 'F'==argv[1][0] && 0==strcmp(argv[2].c_str(),"SC"))
		{
			FindTrackWithSector(diskId,FM7Lib::Atoi(argv[3].c_str()));
		}
	}
	else if('W'==cmd[0])
	{
		std::string fName;
		if(2<=argv.size())
		{
			fName=argv[1];
		}
		else
		{
			fName=this->fName;
		}

		FILE *fp=fopen(fName.c_str(),"wb");
		if(nullptr!=fp)
		{
			for(auto diskId=0; diskId<d77Ptr->GetNumDisk(); ++diskId)
			{
				auto diskPtr=d77Ptr->GetDisk(diskId);
				if(nullptr!=diskPtr)
				{
					auto img=diskPtr->MakeD77Image();
					if(0<img.size())
					{
						auto wrote=fwrite(img.data(),1,img.size(),fp);
						if(wrote==img.size())
						{
							printf("Wrote Disk %d\n",diskId);
						}
						else
						{
							printf("Could not write all bytes.\n");
							printf("Disk full maybe?\n");
							break;
						}
					}
					else
					{
						printf("Cannot open %s in the writing mode.\n",fName.c_str());
					}
				}
			}
			printf("Saved %s.\n",fName.c_str());
			fclose(fp);
		}
		else
		{
			printf("Failed to create a D77 image.\n");
		}
	}
	else if('M'==cmd[0])
	{
		if(2<=argv.size() && 0==strcmp("DS",argv[1].c_str()))
		{
			DeleteDuplicateSector(diskId);
		}
		else if(3<=argv.size() && 0==strcmp("DL",argv[1].c_str()))
		{
			auto sectorId=FM7Lib::Atoi(argv[2].c_str());
			DeleteSectorWithId(diskId,sectorId);
		}
		else if(6<=argv.size() && 0==strcmp("RN",argv[1].c_str()))
		{
			auto track=FM7Lib::Atoi(argv[2].c_str());
			auto side=FM7Lib::Atoi(argv[3].c_str());
			auto sectorFrom=FM7Lib::Atoi(argv[4].c_str());
			auto sectorTo=FM7Lib::Atoi(argv[5].c_str());
			RenumberSector(diskId,track,side,sectorFrom,sectorTo);
		}
		else if(6<=argv.size() && 0==strcmp("SZ",argv[1].c_str()))
		{
			auto track=FM7Lib::Atoi(argv[2].c_str());
			auto side=FM7Lib::Atoi(argv[3].c_str());
			auto sector=FM7Lib::Atoi(argv[4].c_str());
			auto newSize=FM7Lib::Atoi(argv[5].c_str());
			ResizeSector(diskId,track,side,sector,newSize);
		}
		else if(6<=argv.size() && 0==strcmp("CRC",argv[1].c_str()))
		{
			auto track=FM7Lib::Atoi(argv[2].c_str());
			auto side=FM7Lib::Atoi(argv[3].c_str());
			auto sector=FM7Lib::Atoi(argv[4].c_str());
			auto newError=FM7Lib::Atoi(argv[5].c_str());
			SetSectorCRCError(diskId,track,side,sector,newError);
		}
		else if(6<=argv.size() && "DDM"==argv[1])
		{
			auto track=FM7Lib::Atoi(argv[2].c_str());
			auto side=FM7Lib::Atoi(argv[3].c_str());
			auto sector=FM7Lib::Atoi(argv[4].c_str());
			auto newDDM=FM7Lib::Atoi(argv[5].c_str());
			SetSectorDDM(diskId,track,side,sector,newDDM);
		}
		else if(5<=argv.size() && "CS"==argv[1])
		{
			auto track=FM7Lib::Atoi(argv[2].c_str());
			auto side=FM7Lib::Atoi(argv[3].c_str());
			auto sec=FM7Lib::Atoi(argv[4].c_str());
			auto diskPtr=d77Ptr->GetDisk(diskId);
			if(nullptr!=diskPtr)
			{
				auto secDat=diskPtr->ReadSector(track,side,sec);
				if(0<secDat.size())
				{
					for(auto &d : secDat)
					{
						d=0;
					}
					diskPtr->WriteSector(track,side,sec,secDat.size(),secDat.data());
				}
				else
				{
					fprintf(stderr,"Cannot access the sector.\n");
				}
			}
		}
		else if(4<=argv.size() && "CT"==argv[1])
		{
			auto track=FM7Lib::Atoi(argv[2].c_str());
			auto side=FM7Lib::Atoi(argv[3].c_str());
			auto diskPtr=d77Ptr->GetDisk(diskId);
			if(nullptr!=diskPtr)
			{
				auto trkPtr=diskPtr->FindTrack(track,side);
				if(nullptr!=trkPtr)
				{
					for(auto &s : trkPtr->AllSector())
					{
						auto secData=diskPtr->ReadSector(s.track,s.side,s.sector);
						if(0<secData.size())
						{
							for(auto &b : secData)
							{
								b=0;
							}
							diskPtr->WriteSector(s.track,s.side,s.sector,secData.size(),secData.data());
						}
					}
				}
			}
		}
		else if(2<=argv.size() && "WPON"==argv[1])
		{
			auto diskPtr=d77Ptr->GetDisk(diskId);
			if(nullptr!=diskPtr)
			{
				diskPtr->SetWriteProtected();
				printf("Set Write Protect.\n");
			}
		}
		else if(2<=argv.size() && "WPOFF"==argv[1])
		{
			auto diskPtr=d77Ptr->GetDisk(diskId);
			if(nullptr!=diskPtr)
			{
				diskPtr->ClearWriteProtected();
				printf("Cleared Write Protect.\n");
			}
		}
		else if(6<=argv.size() && "FMT"==argv[1])
		{
			auto track=FM7Lib::Atoi(argv[2].c_str());
			auto side=FM7Lib::Atoi(argv[3].c_str());
			auto nSec=FM7Lib::Atoi(argv[4].c_str());
			auto size=FM7Lib::Atoi(argv[5].c_str());
			FormatTrack(diskId,track,side,nSec,size);
		}
		else if(4<=argv.size() && "UFMT"==argv[1])
		{
			auto track=FM7Lib::Atoi(argv[2].c_str());
			auto side=FM7Lib::Atoi(argv[3].c_str());
			UnformatTrack(diskId,track,side);
		}
		else if(6<=argv.size() && "ADSC"==argv[1])
		{
			auto diskPtr=d77Ptr->GetDisk(diskId);
			auto track=FM7Lib::Atoi(argv[2].c_str());
			auto side=FM7Lib::Atoi(argv[3].c_str());
			auto secId=FM7Lib::Atoi(argv[4].c_str());
			auto size=FM7Lib::Atoi(argv[5].c_str());
			if(nullptr!=diskPtr)
			{
				if(diskPtr->IsWriteProtected())
				{
					printf("Write protected\n");
				}
				else
				{
					diskPtr->AddSector(track,side,secId,size);
				}
			}
		}
		else if(6<=argv.size() && "CPSC"==argv[1])
		{
			auto diskPtr=d77Ptr->GetDisk(diskId);
			auto track1=FM7Lib::Atoi(argv[2].c_str());
			auto side1=FM7Lib::Atoi(argv[3].c_str());
			auto secId1=FM7Lib::Atoi(argv[4].c_str());
			auto track2=FM7Lib::Atoi(argv[5].c_str());
			auto side2=FM7Lib::Atoi(argv[6].c_str());
			auto secId2=FM7Lib::Atoi(argv[7].c_str());
			if(nullptr!=diskPtr)
			{
				if(diskPtr->IsWriteProtected())
				{
					printf("Write protected\n");
				}
				else
				{
					auto dat=diskPtr->ReadSector(track1,side1,secId1);
					auto nByte=diskPtr->WriteSector(track2,side2,secId2,dat.size(),dat.data());
					printf("Wrote %d bytes.\n",nByte);
				}
			}
		}
		else if(9<=argv.size() && "CHRN"==argv[1])
		{
			auto diskPtr=d77Ptr->GetDisk(diskId);
			if(nullptr!=diskPtr)
			{
				auto track=FM7Lib::Atoi(argv[2].c_str());
				auto side=FM7Lib::Atoi(argv[3].c_str());
				auto secId=FM7Lib::Atoi(argv[4].c_str());
				auto c=FM7Lib::Atoi(argv[5].c_str());
				auto h=FM7Lib::Atoi(argv[6].c_str());
				auto r=FM7Lib::Atoi(argv[7].c_str());
				auto n=FM7Lib::Atoi(argv[8].c_str());
				if(diskPtr->IsWriteProtected())
				{
					printf("Write protected\n");
				}
				else
				{
					diskPtr->SetSectorCHRN(track,side,secId,c,h,r,n);
				}
			}
		}
		else if(9<=argv.size() && "CHRN"==argv[1])
		{
			auto diskPtr=d77Ptr->GetDisk(diskId);
			if(nullptr!=diskPtr)
			{
				auto track=FM7Lib::Atoi(argv[2].c_str());
				auto side=FM7Lib::Atoi(argv[3].c_str());
				auto secId=FM7Lib::Atoi(argv[4].c_str());
				auto c=FM7Lib::Atoi(argv[5].c_str());
				auto h=FM7Lib::Atoi(argv[6].c_str());
				auto r=FM7Lib::Atoi(argv[7].c_str());
				auto n=FM7Lib::Atoi(argv[8].c_str());
				if(diskPtr->IsWriteProtected())
				{
					printf("Write protected\n");
				}
				else
				{
					
					diskPtr->SetSectorCHRN(track,side,secId,c,h,r,n);
				}
			}
		}
		else if(10<=argv.size() && "REPLCHRN"==argv[1])
		{
			auto diskPtr=d77Ptr->GetDisk(diskId);
			if(nullptr!=diskPtr)
			{
				auto c0=FM7Lib::Atoi(argv[2].c_str());
				auto h0=FM7Lib::Atoi(argv[3].c_str());
				auto r0=FM7Lib::Atoi(argv[4].c_str());
				auto n0=FM7Lib::Atoi(argv[5].c_str());
				auto c=FM7Lib::Atoi(argv[6].c_str());
				auto h=FM7Lib::Atoi(argv[7].c_str());
				auto r=FM7Lib::Atoi(argv[8].c_str());
				auto n=FM7Lib::Atoi(argv[9].c_str());
				if(diskPtr->IsWriteProtected())
				{
					printf("Write protected\n");
				}
				else
				{
					
					diskPtr->ReplaceSectorCHRN(c0,h0,r0,n0,c,h,r,n);
				}
			}
		}
		else if(6<=argv.size() && "W"==argv[1])
		{
			auto diskPtr=d77Ptr->GetDisk(diskId);
			if(nullptr!=diskPtr)
			{
				if(diskPtr->IsWriteProtected())
				{
					printf("Write protected\n");
				}
				else
				{
					auto track=FM7Lib::Atoi(argv[2].c_str());
					auto side=FM7Lib::Atoi(argv[3].c_str());
					auto secId=FM7Lib::Atoi(argv[4].c_str());

					std::string allText;
					for(auto c : FM7Lib::ReadBinaryFile(argv[5].c_str()))
					{
						if(('a'<=c && c<='f') || ('A'<=c && c<='F') || ('0'<=c && c<='9'))
						{
							allText.push_back(c);
						}
					}
					auto dat=diskPtr->ReadSector(track,side,secId);
					if(0<dat.size())
					{
						for(int i=0,ptr=0; i<dat.size() && ptr+2<=allText.size(); ++i,ptr+=2)
						{
							char wd[3]={allText[ptr],allText[ptr+1],0};
							dat[i]=FM7Lib::Xtoi(wd);
						}
						diskPtr->WriteSector(track,side,secId,dat.size(),dat.data());
					}
					else
					{
						fprintf(stderr,"Cannot find the sector.\n");
					}
				}
			}
		}
	}
	else if('Q'==cmd[0])
	{
		quit=true;
	}
}

void D77Analyzer::Help(void) const
{
	printf("Q\n");
	printf("\tQuit\n");
	printf("0\n");
	printf("\tCurrent Disk=0\n");
	printf("1\n");
	printf("\tCurrent Disk=1\n");
	printf("2\n");
	printf("\tCurrent Disk=2\n");
	printf("3\n");
	printf("\tCurrent Disk=3\n");
	printf("L\n");
	printf("\tList Track Info.\n");
	printf("L track side\n");
	printf("\tList Track Info.\n");
	printf("F hexadecimal\n");
	printf("\tFind Data.\n");
	printf("\tExample: F 860AB7FD18\n");
	printf("R hexadecimal hexadecimal\n");
	printf("\tReplace Data.\n");
	printf("\tExample: R 8e7f0086ff 8e7f008600\n");
	printf("C diskimage.d77\n");
	printf("\tCompare disk image.\n");
	printf("D track side sec\n");
	printf("\tDump sector.\n");
	printf("W\n");
	printf("\tWrite disk to the original .D77 file.\n");
	printf("W filename\n");
	printf("\tWrite disk to a .D77 file.\n");
	printf("\tCurrnent disk only.  It doesn't write multi-disk D77.\n");
	printf("X D DS\n");
	printf("\tDiagnose duplicate sectors.\n");
	printf("X F SC secId\n");
	printf("\tFind Tracks that has a specific sector.\n");
	printf("\tExample: X F SC 0xf7\n");
	printf("M DS\n");
	printf("\tRemove duplicate sectors.\n");
	printf("M DL sector\n");
	printf("\tDelete sectors with specific sector ID.\n");
	printf("\tExample: M DL 0xf7\n");
	printf("M RN track side sectorFrom sectorTo\n");
	printf("\tRenumber sector.\n");
	printf("M CS trk sid sec\n");
	printf("\tClear the sector with all zero.\n");
	printf("M SZ trk sid sec size\n");
	printf("\tChange the sector size.  Size must be 128,256,512, or 1024\n");
	printf("M FMT trk side nSec size\n");
	printf("\tFormat a track with nSec sectors each of which is size bytes.\n");
	printf("\tSize must be 128,256,512, or 1024\n");
	printf("M UFMT trk side\n");
	printf("\tUnformat a track.\n");
	printf("M ADSC trk side secId size\n");
	printf("\tAdd a sector in the specified track.\n");
	printf("M CPSC trk1 side1 secId1 trk2 side2 secId2\n");
	printf("\tCopy sector 1 to sector 2.\n");
	printf("M CHRN trk side secId C H R N\n");
	printf("\tSet CHRN to a sector in the specified track.\n");
	printf("M REPLCHRN C0 H0 R0 N0 C H R N\n");
	printf("\tSet CHRN to a sector that has CHRN=C0H0R0N0.\n");
	printf("M CRC trk sid sec 1/0\n");
	printf("\tChange the sector CRC error status.\n");
	printf("\tGive -1 as sec to set CRC error to all the sectors in the track.\n");
	printf("M W trk sid sec input_file\n");
	printf("\tWrite binary dump to the sector.\n");
	printf("\tInput file must be a text file of hexadecimal numbers\n");
	printf("\tSpaces and line breaks will be ignored.\n");
	printf("\tMore bytes than the sector size will be ignored.\n");
	printf("M CT trk sid\n");
	printf("M DDM trk sid sec 1/0\n");
	printf("\tChange the sector DDM.\n");
	printf("\tGive -1 as sec to set DDM to all the sectors in the track.\n");
	printf("M CT trk sid\n");
	printf("\tClear the track with all zero.\n");
	printf("M WPON\n");
	printf("\tWrite protect the disk.\n");
	printf("M WPOFF\n");
	printf("\tRemove write protect.\n");
}

void D77Analyzer::DumpSector(int diskId,int cyl,int side,int sec) const
{
	auto diskPtr=d77Ptr->GetDisk(diskId);
	if(nullptr!=diskPtr)
	{
		bool appeared=false;
		auto trackPtr=diskPtr->FindTrack(cyl,side);
		for(auto &s : trackPtr->sector)
		{
			if(s.sector==sec)
			{
				appeared=true;
				printf("Disk:%d Track:%d Side:%d Sector:%d\n",diskId,cyl,side,sec);
				for(int i=0; i<s.sectorData.size(); ++i)
				{
					if(0==i%16)
					{
						printf("%04x ",i);
					}
					printf(" %02x",s.sectorData[i]);
					if(15==i%16 || i==s.sectorData.size()-1)
					{
						printf("|");
						int i0=(i&0xfffffff0);
						for(int j=i0; j<=i; ++j)
						{
							if(' '<=s.sectorData[j] && s.sectorData[j]<128)
							{
								printf("%c",s.sectorData[j]);
							}
							else
							{
								printf(".");
							}
						}

						printf("\n");
					}
				}
			}
		}
		if(true!=appeared)
		{
			printf("No such sector.\n");
		}
		else
		{
			printf("\n");
		}
	}
}

bool D77Analyzer::MoveToNextSector(int diskId,int &trk,int &side,int &sec) const
{
	auto diskPtr=d77Ptr->GetDisk(diskId);
	if(nullptr!=diskPtr)
	{
		auto trkPtr=diskPtr->FindTrack(trk,side);
		if(nullptr!=trkPtr)
		{
			while(sec<256)
			{
				++sec;
				if(nullptr!=trkPtr->FindSector(sec))
				{
					return true;
				}
			}
		}

		while(trk<90)
		{
			++side;
			if(2<=side)
			{
				side=0;
				++trk;
			}

			auto trkPtr=diskPtr->FindTrack(trk,side);
			if(nullptr!=trkPtr)
			{
				for(sec=1; sec<256; ++sec)
				{
					if(nullptr!=trkPtr->FindSector(sec))
					{
						return true;
					}
				}
			}
		}
	}
	return false;
}

void D77Analyzer::DiagnoseDuplicateSector(int diskId) const
{
	auto diskPtr=d77Ptr->GetDisk(diskId);
	if(nullptr!=diskPtr)
	{
		for(auto &loc : diskPtr->AllTrack())
		{
			auto trkPtr=diskPtr->FindTrack(loc.track,loc.side);
			if(nullptr==trkPtr)
			{
				continue;
			}
			auto &t=*trkPtr;
			for(int i=0; i<t.sector.size(); ++i)
			{
				auto &s0=t.sector[i];
				for(int j=i+1; j<t.sector.size(); ++j)
				{
					auto &s1=t.sector[j];
					if(s0.sector==s1.sector)
					{
						bool diff=false;
						if(s0.sectorData.size()!=s1.sectorData.size())
						{
							diff=true;
						}
						else
						{
							for(int i=0; i<s0.sectorData.size(); ++i)
							{
								if(s0.sectorData[i]!=s1.sectorData[i])
								{
									diff=true;
									break;
								}
							}
						}

						printf("Duplicate Sector Track:%d Side:%d Sector:%d ",s0.cylinder,s0.head,s0.sector);
						if(true==diff)
						{
							printf("!!!Different Content!!!\n");
						}
						else
						{
							printf("Identical Content.\n");
						}
					}
				}
			}
		}
	}
}

void D77Analyzer::FindTrackWithSector(int diskId,int sectorId) const
{
	bool found=false;
	auto diskPtr=d77Ptr->GetDisk(diskId);
	if(nullptr!=diskPtr)
	{
		for(auto trkLoc : diskPtr->AllTrack())
		{
			auto trkPtr=diskPtr->FindTrack(trkLoc.track,trkLoc.side);
			if(nullptr==trkPtr)
			{
				continue;
			}
			auto &t=*trkPtr;
			for(auto secLoc : t.AllSector())
			{
				if(secLoc.sector==sectorId)
				{
					found=true;
					t.PrintInfo();
					break;
				}
			}
		}
	}
	if(true!=found)
	{
		printf("No track has sector %d(0x%02x)\n",sectorId,sectorId);
	}
}

void D77Analyzer::DeleteDuplicateSector(int diskId)
{
	auto diskPtr=d77Ptr->GetDisk(diskId);
	if(nullptr!=diskPtr)
	{
		for(auto trkLoc : diskPtr->AllTrack())
		{
			diskPtr->DeleteDuplicateSector(trkLoc.track,trkLoc.side);
		}
	}
}

void D77Analyzer::DeleteSectorWithId(int diskId,int sectorId)
{
	auto diskPtr=d77Ptr->GetDisk(diskId);
	if(nullptr!=diskPtr)
	{
		for(auto trkLoc : diskPtr->AllTrack())
		{
			diskPtr->DeleteSectorWithId(trkLoc.track,trkLoc.side,sectorId);
		}
	}
}

void D77Analyzer::FindData(int diskId,const char str[]) const
{
	auto diskPtr=d77Ptr->GetDisk(diskId);
	if(nullptr!=diskPtr)
	{
		auto ptn=D77File::StrToByteArray(str);
		for(auto trkLoc : diskPtr->AllTrack())
		{
			auto trkPtr=diskPtr->FindTrack(trkLoc.track,trkLoc.side);
			if(nullptr==trkPtr)
			{
				continue;
			}
			auto &t=*trkPtr;
			for(auto found : t.Find(ptn))
			{
				printf("Found Track:%d Side:%d Sector:%d(0x%x) Addr:%04x\n",
				    found.track,
				    found.side,
				    found.sector,found.sector,
				    found.addr);
			}
		}
	}
}

void D77Analyzer::ReplaceData(int diskId,const char fromStr[],const char toStr[])
{
	auto diskPtr=d77Ptr->GetDisk(diskId);
	if(nullptr!=diskPtr)
	{
		if(true==diskPtr->IsWriteProtected())
		{
			printf("Write protected.\n");
			return;
		}

		auto from=D77File::StrToByteArray(fromStr);
		auto to=D77File::StrToByteArray(toStr);
		if(from.size()!=to.size())
		{
			printf("FROM and TO must be the same size.\n");
			return;
		}
		diskPtr->ReplaceData(from,to);
	}
}

void D77Analyzer::StoreData(int diskId,int trk,int sid,int sec,int addr,const char toStr[])
{
	auto diskPtr=d77Ptr->GetDisk(diskId);
	if(nullptr!=diskPtr)
	{
		if(true==diskPtr->IsWriteProtected())
		{
			printf("Write protected.\n");
			return;
		}

		auto to=D77File::StrToByteArray(toStr);

		auto secData=diskPtr->ReadSector(trk,sid,sec);
		if(secData.size()<addr+to.size())
		{
			fprintf(stderr,"Overflow.\n");
			return;
		}

		for(auto d : to)
		{
			secData[addr]=d;
			++addr;
		}
		diskPtr->WriteSector(trk,sid,sec,secData.size(),secData.data());
	}
}

void D77Analyzer::RenumberSector(int diskId,int track,int side,int secFrom,int secTo) const
{
	auto diskPtr=d77Ptr->GetDisk(diskId);
	if(nullptr!=diskPtr)
	{
		diskPtr->RenumberSector(track,side,secFrom,secTo);
	}
}

void D77Analyzer::ResizeSector(int diskId,int track,int side,int sec,int newSize)
{
	auto diskPtr=d77Ptr->GetDisk(diskId);
	if(nullptr!=diskPtr)
	{
		if(true!=diskPtr->ResizeSector(track,side,sec,newSize))
		{
			fprintf(stderr,"Failed to resize sector.\n");
		}
	}
}
void D77Analyzer::SetSectorCRCError(int diskId,int track,int side,int sec,int newError)
{
	auto diskPtr=d77Ptr->GetDisk(diskId);
	if(nullptr!=diskPtr)
	{
		if(true!=diskPtr->SetCRCError(track,side,sec,(0!=newError)))
		{
			fprintf(stderr,"Failed to change CRC error status.\n");
		}
	}
}

void D77Analyzer::SetSectorDDM(int diskId,int track,int side,int secId,bool ddm)
{
	auto diskPtr=d77Ptr->GetDisk(diskId);
	if(nullptr!=diskPtr)
	{
		if(true!=diskPtr->SetDDM(track,side,secId,ddm))
		{
			fprintf(stderr,"Failed to change DDM (Deleted Data) status.\n");
		}
	}
}

bool D77Analyzer::FormatTrack(int diskId,int track,int side,int nSec,int secSize)
{
	auto diskPtr=d77Ptr->GetDisk(diskId);
	if(nullptr!=diskPtr)
	{
		if(true==diskPtr->IsWriteProtected())
		{
			printf("Write protected.\n");
			return false;
		}

		std::vector <D77File::D77Disk::D77Sector> sec;
		sec.resize(nSec);
		for(int i=0; i<nSec; ++i)
		{
			if(true!=sec[i].Make(track,side,i+1,secSize))
			{
				return false;
			}
			sec[i].nSectorTrack=nSec;
		}
		diskPtr->WriteTrack(track,side,sec.size(),sec.data());
		return true;
	}
	return false;
}

bool D77Analyzer::UnformatTrack(int diskId,int track,int side)
{
	auto diskPtr=d77Ptr->GetDisk(diskId);
	if(nullptr!=diskPtr)
	{
		if(true==diskPtr->IsWriteProtected())
		{
			printf("Write protected.\n");
			return false;
		}
		diskPtr->WriteTrack(track,side,0,nullptr);
		return true;
	}
	return false;
}

void D77Analyzer::Compare(const std::vector <std::string> &argv) const
{
	printf("Compare Disks\n");

	if(2>argv.size())
	{
		fprintf(stderr,"Too few arguments.\n");
		return;
	}


	D77File bDiskD77;
	auto raw=FM7Lib::ReadBinaryFile(argv[1].c_str());
	if(0<raw.size())
	{
		bDiskD77.SetData(raw);
	}
	else
	{
		fprintf(stderr,"Cannot open %s\n",argv[1].c_str());
		return;
	}

	if(1>bDiskD77.GetNumDisk())
	{
		fprintf(stderr,"No disk in %s\n",argv[1].c_str());
		return;
	}

	auto aDiskPtr=d77Ptr->GetDisk(diskId);
	if(nullptr==aDiskPtr)
	{
		fprintf(stderr,"No disk is open.\n");
		return;
	}

	auto &aDisk=*aDiskPtr;
	auto &bDisk=*bDiskD77.GetDisk(0);

	auto aTrkLoc=aDisk.AllTrack();
	auto bTrkLoc=bDisk.AllTrack();
	if(aTrkLoc.size()!=bTrkLoc.size())
	{
		printf("ADisk and BDisk have different number of tracks.\n");
		printf("ADisk %d tracks.\n",(int)aTrkLoc.size());
		printf("BDisk %d tracks.\n",(int)bTrkLoc.size());
	}

	for(auto trkLoc : aTrkLoc)
	{
		auto aTrackPtr=aDisk.FindTrack(trkLoc.track,trkLoc.side);
		auto bTrackPtr=bDisk.FindTrack(trkLoc.track,trkLoc.side);

		if(nullptr==bTrackPtr)
		{
			printf("BDisk does not have track %d side %d formatted.\n",trkLoc.track,trkLoc.side);
			continue;
		}

		auto aSecLoc=aTrackPtr->AllSector();
		auto bSecLoc=aTrackPtr->AllSector();
		for(auto secLoc : aSecLoc)
		{
			if(nullptr==bTrackPtr->FindSector(secLoc.sector))
			{
				printf("Track %d Side %d Sector %d does not exist in BDisk\n",secLoc.track,secLoc.side,secLoc.sector);
			}
		}
		for(auto secLoc : bSecLoc)
		{
			if(nullptr==aTrackPtr->FindSector(secLoc.sector))
			{
				printf("Track %d Side %d Sector %d does not exist in ADisk\n",secLoc.track,secLoc.side,secLoc.sector);
			}
		}

		for(auto secLoc : aSecLoc)
		{
			auto aSecPtr=aTrackPtr->FindSector(secLoc.sector);
			auto bSecPtr=bTrackPtr->FindSector(secLoc.sector);
			if(nullptr!=aSecPtr && nullptr!=bSecPtr)
			{
				if(aSecPtr->sectorData.size()!=bSecPtr->sectorData.size())
				{
					printf("Track %d Side %d Sector %d have different size.\n",secLoc.track,secLoc.side,secLoc.sector);
					printf("ADisk %d bytes\n",(int)aSecPtr->sectorData.size());
					printf("BDisk %d bytes\n",(int)bSecPtr->sectorData.size());
				}
				else
				{
					for(int i=0; i<aSecPtr->sectorData.size(); ++i)
					{
						if(aSecPtr->sectorData[i]!=bSecPtr->sectorData[i])
						{
							printf("Different!  Track %d Side %d Sector %d (Offset %04x)\n",secLoc.track,secLoc.side,secLoc.sector,i);
							goto NEXTSECTOR;
						}
					}
				}
			}
		NEXTSECTOR:
			;
		}
	}
}

////////////////////////////////////////////////////////////

int main(int ac,char *av[])
{
	if(3<=ac && (0==strcmp("-new",av[1]) || 0==strcmp("-NEW",av[1])))
	{
		D77File d77;
		d77.CreateStandardFormatted();
		D77Analyzer term;
		term.fName=av[2];
		term.Terminal(d77);
		return 0;
	}

	if(2>ac)
	{
		printf("Usage:\n");
		printf("  d77analyze filename.d77\n");
		printf("    or\n");
		printf("  d77analyze -new filename.d77\n");
		return 1;
	}

	std::vector <unsigned char> dat;
	FILE *fp=fopen(av[1],"rb");
	if(nullptr==fp)
	{
		return 1;
	}

	fseek(fp,0,SEEK_END);
	auto fsize=ftell(fp);
	fseek(fp,0,SEEK_SET);

	dat.resize(fsize);
	fread(dat.data(),1,fsize,fp);

	fclose(fp);


	D77File d77;
	d77.SetData(dat);
	d77.PrintInfo();

	D77Analyzer term;
	term.fName=av[1];
	term.Terminal(d77);

	return 0;
}
