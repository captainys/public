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

YSRESULT Test(YsConstArrayMask <YsVec2> plg,int nExpect)
{
	YSRESULT res=YSOK;

	YsBoundingBoxMaker <YsVec2> bbx;
	bbx.Make(plg);

	YsPositiveAreaCalculator posiArea;
	posiArea.Initialize(bbx.Min(),bbx.Max());

	for(auto idx : plg.AllIndex())
	{
		auto v=YsVec2::UnitVector(plg.GetCyclic(idx+1)-plg[idx]);
		YsVec2 n(-v.y(),v.x());
		posiArea.AddLine(plg[idx],n);
		printf("Remaining %d\n",(int)posiArea.GetPolygon().size());
	}

	// posiArea.SaveSrf("left.srf");

	printf("%d\n",posiArea.GetPolygon().size());

	if(nExpect!=posiArea.GetPolygon().size())
	{
		fprintf(stderr,"The polygon is supposed to have %d corners.\n",nExpect);
		fprintf(stderr,"YsPositiveAreaCalculator returned %d.\n",(int)posiArea.GetPolygon().size());
		res=YSERR;
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

	const YsVec2 star[]=
	{
		YsVec2( 7.5387638256343532,  1.9945608327911504),
		YsVec2( 3.0087443070917383,  1.9945608327911504),
		YsVec2( 0.6648536109303828,  6.4569681197137285),
		YsVec2(-1.8593363695510767,  2.0847104749512040),
		YsVec2(-7.3359271307742349,  2.0847104749512040),
		YsVec2(-3.8200910865322042, -2.0396356538711773),
		YsVec2(-5.5780091086532178, -7.6965256994144440),
		YsVec2( 0.3718672739102146, -3.3242680546519194),
		YsVec2( 4.0229277813923234, -6.9978659726740400),
		YsVec2( 3.7074040338321410, -1.4536629798308394),
	};
	if(YSOK!=Test(YsConstArrayMask<YsVec2>(10,star),6))
	{
		++nFail;
	}

	const YsVec2 star2[]=
	{
		YsVec2( 7.5387638256343532,  1.9945608327911504),
		YsVec2(-7.3359271307742349,  2.0847104749512040),
		YsVec2( 4.0229277813923234, -6.9978659726740400),
		YsVec2( 0.6648536109303828,  6.4569681197137285),
		YsVec2(-5.5780091086532178, -7.6965256994144440),
	};
	if(YSOK!=Test(YsConstArrayMask<YsVec2>(5,star2),5))
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
