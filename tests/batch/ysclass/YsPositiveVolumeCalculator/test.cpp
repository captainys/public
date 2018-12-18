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


	YsPositiveVolumeCalculator posVolCal;
	posVolCal.Initialize(-YsXYZ(),YsXYZ());

	for(int x=-1; x<=1; x+=2)
	{
		for(int y=-1; y<=1; y+=2)
		{
			for(int z=-1; z<=1; z+=2)
			{
				YsVec3 o(x,y,z),n(-x,-y,-z);
				o.Normalize();
				n.Normalize();
				posVolCal.AddPlane(YsPlane(o,n));
			}
		}
	}

	auto &shl=posVolCal.GetVolume();
	if(shl.GetNumVertex()!=24 || shl.GetNumPolygon()!=14)
	{
		fprintf(stderr,"Polygon or Vertex count is different from expected.\n");
		fprintf(stderr,"Expected 24 vertices and 14 polygons.\n");
		fprintf(stderr,"Actual %d vertices and %d polygons.\n",(int)shl.GetNumVertex(),(int)shl.GetNumPolygon());
		res=YSERR;
	}

	auto searchPtr=shl.GetSearchTable();
	if(nullptr==searchPtr)
	{
		fprintf(stderr,"Search table is missing.\n");
		res=YSERR;
	}
	else
	{
		YsShellEdgeEnumHandle edHd=nullptr;
		while(YSOK==searchPtr->FindNextEdge(shl,edHd))
		{
			if(2!=searchPtr->GetNumPolygonUsingEdge(shl,edHd))
			{
				fprintf(stderr,"Non-manifold edges.\n");
				fprintf(stderr,"Edge-Use count %d\n",(int)searchPtr->GetNumPolygonUsingEdge(shl,edHd));
				res=YSERR;
			}
		}
	}


	// posVolCal.SaveSrf("test.srf");


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
