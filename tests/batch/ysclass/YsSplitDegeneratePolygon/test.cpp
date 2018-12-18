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

#include <ysedgeloop.h>

void ShowResult(const char title[],YsArray <YsArray <int> > &newPlg)
{
	printf("[%s]\n",title);
	for(auto &p : newPlg)
	{
		printf(">>");
		for(auto i : p)
		{
			printf(" %d",i);
		}
		printf("\n");
	}
}

YSRESULT RunTest(void)
{
	YSRESULT res=YSOK;

	YsArray <YsArray <int> > newPlg;

	const int eyeGlasses[10]={1,2,3,4,5,6,7,4,3,8};
	// Should be decomposed to {1,2,3,8} and {4,5,6,7}  (Two squares connected by 3-4)

	if(YSOK==YsSplitDegeneratePolygon(newPlg,10,eyeGlasses))
	{
		if(2!=newPlg.GetN() ||
			newPlg[0][0]!=4 ||
			newPlg[0][1]!=5 ||
			newPlg[0][2]!=6 ||
			newPlg[0][3]!=7 ||
			newPlg[1][0]!=1 ||
			newPlg[1][1]!=2 ||
			newPlg[1][2]!=3 ||
			newPlg[1][3]!=8)
		{
			printf("EyeGlasses polygons not split as expected.\n");
			res=YSERR;
		}

		ShowResult("eyeGlasses",newPlg);
	}
	else
	{
		printf("Failed to split eyeGlasses.\n");
		res=YSERR;
	}

	const int dango[11]={1,2,3,4,5,6,7,5,8,3,9};
	// Should be decomposed to {5,6,7},{3,4,5,8},{1,2,3,9},
	if(YSOK==YsSplitDegeneratePolygon(newPlg,11,dango))
	{
		if(3!=newPlg.GetN() ||
			newPlg[0][0]!=5 ||
			newPlg[0][1]!=6 ||
			newPlg[0][2]!=7 ||
			newPlg[1][0]!=3 ||
			newPlg[1][1]!=4 ||
			newPlg[1][2]!=5 ||
			newPlg[1][3]!=8 ||
			newPlg[2][0]!=1 ||
			newPlg[2][1]!=2 ||
			newPlg[2][2]!=3 ||
			newPlg[2][3]!=9)
		{
			printf("Dango polygons not split as expected.\n");
			res=YSERR;
		}
		ShowResult("dango",newPlg);
	}
	else
	{
		printf("Failed to split dango.\n");
		res=YSERR;
	}


	const int collapseTri1[5]={1,1,1,2,2};
	if(YSOK==YsSplitDegeneratePolygon(newPlg,5,collapseTri1))
	{
		if(1<=newPlg.GetN())
		{
			printf("collapseTri1 not split as expected.\n");
			res=YSERR;
		}
		ShowResult("collapseTri1",newPlg);
	}
	else
	{
		printf("Failed to split collapseTri1\n");
		res=YSERR;
	}


	const int collapseTri2[5]={1,2,2,2,1};
	if(YSOK==YsSplitDegeneratePolygon(newPlg,5,collapseTri2))
	{
		if(1<=newPlg.GetN())
		{
			printf("collapseTri2 not split as expected.\n");
			res=YSERR;
		}
		ShowResult("collapseTri2",newPlg);
	}
	else
	{
		printf("Failed to split collapseTri2\n");
		res=YSERR;
	}


	const int misuzu[13]={1,2,3,4,5,6,7,8,5,2,8,9,1};
	// 1-2-3-4-5-6-7-8-9 makes an outer loop.
	// 8-5-2 makes an inner loop.
	if(YSOK==YsSplitDegeneratePolygon(newPlg,13,misuzu))
	{
		printf("misuzu didn't fail.  It's an error!\n");
		ShowResult("misuzu",newPlg);
		res=YSERR;
	}
	else
	{
		printf("Misuzu failed as expected.\n");
		ShowResult("misuzu",newPlg);
	}


	const int togetoge[20]={1,2,3,4,5,6,5,7,8,7,4,11,4,3,12,3,13,14,13,2};
	// Decomposed to {4,5,7} and {2,3,13}
	if(YSOK==YsSplitDegeneratePolygon(newPlg,20,togetoge))
	{
		if(2!=newPlg.GetN() ||
			newPlg[0][0]!=4 ||
			newPlg[0][1]!=5 ||
			newPlg[0][2]!=7 ||
			newPlg[1][0]!=2 ||
			newPlg[1][1]!=3 ||
			newPlg[1][2]!=13)
		{
			printf("Togetoge polygons not split as expected.\n");
			res=YSERR;
		}
		ShowResult("togetoge",newPlg);
	}
	else
	{
		printf("Failed to split togetoge.\n");
		res=YSERR;
	}


	return res;
}


int main(void)
{
	if(YSOK!=RunTest())
	{
		return 1;
	}
	return 0;
}

