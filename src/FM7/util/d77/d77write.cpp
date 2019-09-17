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
	d77.PrintInfo();
	auto diskPtr=d77.GetDisk(0);
	if(nullptr==diskPtr)
	{
		fprintf(stderr,".D77 image contains no disk.\n");
		return 1;
	}


	int trk=FM7Lib::Atoi(av[2]);
	int sid=FM7Lib::Atoi(av[3]);
	int sec=FM7Lib::Atoi(av[4]);

	long long int ptr=0;
	while(ptr<binDat.dat.size())
	{
		auto secDat=diskPtr->ReadSector(trk,sid,sec);

		if(0<secDat.size())
		{
			printf("TRK:%d  SID:%d  SEC:%d\n",trk,sid,sec);
			for(auto &b : secDat)
			{
				b=binDat.dat[ptr++];
				if(binDat.dat.size()<=ptr)
				{
					break;
				}
			}
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
	return 0;
}
