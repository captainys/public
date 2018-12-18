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

YSRESULT Test(YsLattice3d <int> &ltc,YSSIZE_T np,const YsVec3 p[])
{
	YSRESULT res=YSOK;

	auto blkIdx=ltc.GetIntersectingBlockListPolygon(np,p);
	printf("%lld intersecting blocks.\n",blkIdx.size());

	YsKeyStore itscBlk;
	for(auto i : blkIdx)
	{
		itscBlk.Add((YSHASHKEY)ltc.GetLinearIndex(i));
	}

	YsCollisionOfPolygon coll;
	coll.SetPolygon1(np,p);

	for(int x=0; x<ltc.Nx(); ++x)
	{
		for(int y=0; y<ltc.Ny(); ++y)
		{
			for(int z=0; z<ltc.Nz(); ++z)
			{
				YsVec3 range[2];
				ltc.GetBlockRange(range[0],range[1],x,y,z);
				auto itsc=coll.CheckIntersectionBetweenPolygon1AndBlock(range[0],range[1]);
				auto included=itscBlk.IsIncluded(ltc.GetLinearIndex(YsVec3i(x,y,z)));
				if(itsc!=included)
				{
					fprintf(stderr,"Error! %d %d %d %d %d\n",x,y,z,itsc,included);
					for(int i=0; i<np; ++i)
					{
						printf("V %s\n",p[i].Txt());
					}
					printf("V %s\n",range[0].Txt());
					printf("V %s\n",range[1].Txt());
					printf("GE 0 3 4\n");
					res=YSERR;
				}
			}
		}
	}

	if(YSOK!=res)
	{
		fprintf(stderr,"Failed!\n");
	}
	return res;
}

int main(void)
{
	int nFail=0;

	{
		YsTolerance=1e-6;

		YsLattice3d <int> ltc;
		ltc.Create(11,13,17,-YsXYZ(),YsXYZ());

		YsArray <YsVec3> plg;
		plg.Add(YsVec3(-1.0,-1.0,0.5));
		plg.Add(YsVec3( 1.0,-1.0,0.5));
		plg.Add(YsVec3( 1.0, 1.0,0.5));
		plg.Add(YsVec3(-1.0, 1.0,0.5));
		for(int i=0; i<180; ++i)
		{
			if(YSOK!=Test(ltc,plg.size(),plg.data()))
			{
				++nFail;
			}
			plg[0].RotateXY(YsPi/180.0);
			plg[1].RotateXY(YsPi/180.0);
			plg[2].RotateXY(YsPi/180.0);
			plg[3].RotateXY(YsPi/180.0);
		}
		plg.clear();


		plg.Add(YsVec3( 0.0, 0.0,-2.0));
		plg.Add(YsVec3( 0.0, 0.0, 1.0));
		plg.Add(YsVec3( 0.2, 0.0, 1.0));
		for(int i=0; i<180; ++i)
		{
			if(YSOK!=Test(ltc,plg.size(),plg.data()))
			{
				++nFail;
			}
			plg[0].RotateYZ(YsPi/180.0);
			plg[1].RotateYZ(YsPi/180.0);
			plg[2].RotateYZ(YsPi/180.0);
		}
		plg.clear();



		plg.Add(YsVec3( 0.0, 0.0, 0.0));
		plg.Add(YsVec3( 0.0, 0.0, 1.0));
		plg.Add(YsVec3( 0.0, 1.0, 0.0));
		for(int i=0; i<180; ++i)
		{
			if(YSOK!=Test(ltc,plg.size(),plg.data()))
			{
				++nFail;
			}
			plg[0].RotateYZ(YsPi/180.0);
			plg[1].RotateYZ(YsPi/180.0);
			plg[2].RotateYZ(YsPi/180.0);

			plg[0].RotateXY(YsPi/180.0);
			plg[1].RotateXY(YsPi/180.0);
			plg[2].RotateXY(YsPi/180.0);
		}
		plg.clear();
	}

	printf("%d failed.\n",nFail);
	if(0<nFail)
	{
		return 1;
	}
	return 0;
}
