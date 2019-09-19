#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <vector>
#include <string>

#include "d77.h"
#include "../lib/cpplib.h"
#include "../lib/fm7lib.h"


// Write SREC directly to the sectors.



int main(int ac,char *av[])
{
	if(ac<6)
	{
		printf("Usage: D77Write diskimage.d77 track side sector binary.srec\n");
		return 1;
	}

	auto srec=FM7Lib::ReadTextFile(av[5]);
	FM7BinaryFile binDat;
	if(0==srec.size() || true!=binDat.DecodeSREC(srec))
	{
		fprintf(stderr,"Empty or broken srec file.\n");
		return 1;
	}


	auto dat=FM7Lib::ReadBinaryFile(av[1]);
	if(0==dat.size())
	{
		fprintf(stderr,"Cannot open .D77 image.\n");
		return 1;
	}

	D77File d77;
	d77.SetData(dat);
	// d77.PrintInfo();
	auto diskPtr=d77.GetDisk(0);
	if(nullptr==diskPtr)
	{
		fprintf(stderr,".D77 image contains no disk.\n");
		return 1;
	}

	printf("Binary %lld bytes.\n",binDat.dat.size());

	int trk=FM7Lib::Atoi(av[2]);
	int sid=FM7Lib::Atoi(av[3]);
	int sec=FM7Lib::Atoi(av[4]);

	printf("Start Track/Side/Sector %d/%d/%d\n",trk,sid,sec);

	long long int ptr=0;
	while(ptr<binDat.dat.size())
	{
		auto secDat=diskPtr->ReadSector(trk,sid,sec);

		if(0<secDat.size())
		{
			printf("PTR:%lld  TRK:%d  SID:%d  SEC:%d\n",ptr,trk,sid,sec);
			for(auto &b : secDat)
			{
				b=binDat.dat[ptr++];
				if(binDat.dat.size()<=ptr)
				{
					break;
				}
			}
			diskPtr->WriteSector(trk,sid,sec,secDat.size(),secDat.data());
			++sec;
		}
		else
		{
			sec=1;
			++sid;
			if(2<=sid)
			{
				sid=0;
				++trk;
			}
			if(40<=trk)
			{
				fprintf(stderr,"Exceeded 40 tracks.\n");
				return 1;
			}
		}
	}


	std::vector <unsigned char> outDat;
	for(auto diskId=0; diskId<d77.GetNumDisk(); ++diskId)
	{
		auto diskPtr=d77.GetDisk(diskId);
		if(nullptr!=diskPtr)
		{
			auto img=diskPtr->MakeD77Image();
			if(0<img.size())
			{
				outDat.insert(outDat.end(),img.begin(),img.end());
			}
			else
			{
				fprintf(stderr,"Failed to make a disk image.\n");
				return 1;
			}
		}
	}

	if(0<outDat.size() && true==FM7Lib::WriteBinaryFile(av[1],outDat))
	{
		printf("Saved %s.\n",av[1]);
	}
	else
	{
		fprintf(stderr,"Failed to write to a D77 image.\n");
		return 1;
	}

	return 0;
}
