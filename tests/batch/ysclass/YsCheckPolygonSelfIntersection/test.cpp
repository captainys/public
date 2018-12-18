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

YSRESULT Test(void)
{
	YSRESULT res=YSOK;

	YsVec3 noItsc[4]={
		YsVec3(-10,-10,1),
		YsVec3( 10,-10,1),
		YsVec3( 10, 10,1),
		YsVec3(-10, 10,1),
	};
	YsVec3 itsc[4]={
		YsVec3(-10,-10,1),
		YsVec3( 10,-10,1),
		YsVec3(-10, 10,1),
		YsVec3( 10, 10,1),
	};


	for(int i=0; i<=100; ++i)
	{
		printf("%d/%d\n",i,100);

		if(YSTRUE==YsCheckPolygonSelfIntersection(YsConstArrayMask <YsVec3> (4,noItsc)))
		{
			printf("It says intersection where it shouldn't.\n");
			res=YSERR;
		}
		if(YSTRUE!=YsCheckPolygonSelfIntersection(YsConstArrayMask <YsVec3> (4,itsc)))
		{
			printf("It says no intersection where it should.\n");
			res=YSERR;
		}
		if(YSTRUE==YsCheckPolygonSelfIntersection(4,noItsc))
		{
			printf("It says intersection where it shouldn't.\n");
			res=YSERR;
		}
		if(YSTRUE!=YsCheckPolygonSelfIntersection(4,itsc))
		{
			printf("It says no intersection where it should.\n");
			res=YSERR;
		}

		for(auto &p : noItsc)
		{
			p.RotateXY(YsPi/6.0);
			p.RotateXZ(YsPi/4.0);
			p.RotateYZ(YsPi/3.0);
			p.AddX(10.0);
		}
		for(auto &p : itsc)
		{
			p.RotateXY(YsPi/6.0);
			p.RotateXZ(YsPi/4.0);
			p.RotateYZ(YsPi/3.0);
			p.AddX(10.0);
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
	if(YSOK!=Test())
	{
		++nFail;
	}

	printf("%d failed.\n",nFail);
	if(0<nFail)
	{
		return 1;
	}
	return 0;
}
