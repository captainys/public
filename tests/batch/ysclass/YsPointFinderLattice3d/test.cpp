/* ////////////////////////////////////////////////////////////

File Name: test.cpp
Copyright (c) 2017 Soji Yamakawa.  All rights reserved.
http://www.ysflight.com

Redistribution and use in source and binary forms, with or without modification, 
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, 
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice, 
   this list of conditions and the following disclaimer in the documentation 
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR 
PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS 
BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE 
GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) 
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT 
LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT 
OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

//////////////////////////////////////////////////////////// */

#include <ysclass.h>

#include <stdio.h>
#include <stdlib.h>
#include <time.h>


int main(void)
{
	auto seed=1487379624;  // This seed was giving a crash with VS2015
	srand((int)seed);

	printf("%lld\n",seed);

	YSRESULT res=YSOK;

	YsPointFinderLattice3d <int> ltc;
	YsArray <YsPair <YsVec3,int> > record;

	ltc.Create(100,YsVec3(-20.0,-10.5,-12.0),YsVec3(24.0,10.6,13.0));
	auto blkDim=ltc.GetBlockDimension();
	printf("Block Dimension=%s\n",blkDim.Txt());

	const int nSample=1000;
	for(int i=0; i<nSample; ++i)
	{
		double x=(double)rand();
		double y=(double)rand();
		double z=(double)rand();

		x=(x/(double)RAND_MAX)*20.0-10.0;
		y=(y/(double)RAND_MAX)*20.0-10.0;
		z=(z/(double)RAND_MAX)*20.0-10.0;

		YsVec3 pos(x,y,z);
		ltc.Add(pos,i);

		record.Increment();
		record.Last().a=pos;
		record.Last().b=i;
	}


	for(int i=0; i<1000; ++i)
	{
		double x=(double)rand();
		double y=(double)rand();
		double z=(double)rand();

		x=(x/(double)RAND_MAX)*20.0-10.0;
		y=(y/(double)RAND_MAX)*20.0-10.0;
		z=(z/(double)RAND_MAX)*20.0-10.0;

		const int N=5;

		YsVec3 from(x,y,z);
		auto found=ltc.FindNNearest(from,N);
		if(0==found.GetN())
		{
			printf("Error!  Could not find a nearest point!\n");
			res=YSERR;
		}
		else
		{
			if(N!=found.GetN())
			{
				printf("Error!  Less than N nearest points were found.\n");
				res=YSERR;
			}

			auto recordCopy=record;
			YsArray <double> dist(record.GetN(),NULL);

			for(int i=0; i<record.GetN(); ++i)
			{
				dist[i]=(record[i].a-from).GetLength();
			}

			YsQuickSort(dist.GetN(),dist.GetEditableArray(),recordCopy.GetEditableArray());

			for(int i=0; i<N && i<found.GetN(); ++i)
			{
				if(recordCopy[i].a!=found[i]->pos || found[i]->attrib!=recordCopy[i].b)
				{
					printf("Error!  Correct=%s\n",recordCopy[i].a.Txt());
					printf("        Found=  %s\n",found[i]->pos.Txt());
					printf("        From=   %s\n",from.Txt());
					printf("        %dth nearest\n",(i+1));

					auto blkIdx=ltc.GetBlockIndex(from);
					printf("        Block Index of From:    %d %d %d\n",blkIdx[0],blkIdx[1],blkIdx[2]);
					blkIdx=ltc.GetBlockIndex(recordCopy[i].a);
					printf("        Block Index of Correct: %d %d %d\n",blkIdx[0],blkIdx[1],blkIdx[2]);
					blkIdx=ltc.GetBlockIndex(found[i]->pos);
					printf("        Block Index of Found:   %d %d %d\n",blkIdx[0],blkIdx[1],blkIdx[2]);

					res=YSERR;
				}
			}
		}
	}

	printf("Test done.\n");
	if(YSOK==res)
	{
		printf("OK!\n");
		return 0;
	}
	else
	{
		printf("Error!\n");
		return 1;
	}
}

