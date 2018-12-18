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

#include <stdio.h>
#include <ysclass.h>
#include <ysport.h>

YSRESULT CompareShell(const YsShell &shl1,const YsShell &shl2)
{
	YSRESULT res=YSOK;

	const double area1=shl1.ComputeTotalArea();
	const double area2=shl2.ComputeTotalArea();

	const double vol1=shl1.ComputeVolume();
	const double vol2=shl2.ComputeVolume();

	if(YSTRUE!=YsEqual(vol1,vol2))
	{
		printf("Volumes don't match.  %lf-%lf=%lf\n",vol1,vol2,vol1-vol2);
		res=YSERR;
	}
	if(YSTRUE!=YsEqual(area1,area2))
	{
		printf("Areas don't match.  %lf-%lf=%lf\n",area1,area2,area1-area2);
		res=YSERR;
	}

	if(YSOK==res)
	{
		printf("Match!\n");
	}

	return res;
}

int main(void)
{
	YsFileIO::ChDir(YsSpecialPath::GetProgramBaseDirW());

	printf("@@@ TESTING: YsShell Boolean Operations\n");

	YsShell block,cylinder1,cylinder2,cylinder3,sphere;
	YsShell ref1,ref2,ref3,ref4;

	if(YSOK==block.LoadSrf("block.srf") &&
	   YSOK==cylinder1.LoadSrf("cylinder1.srf") && 
	   YSOK==cylinder2.LoadSrf("cylinder2.srf") && 
	   YSOK==cylinder3.LoadSrf("cylinder3.srf") && 
	   YSOK==sphere.LoadSrf("sphere.srf") &&
	   YSOK==ref1.LoadSrf("ref1.srf") &&
	   YSOK==ref2.LoadSrf("ref2.srf") &&
	   YSOK==ref3.LoadSrf("ref3.srf") &&
	   YSOK==ref4.LoadSrf("ref4.srf"))
	{
		YsShell shl1,shl2,shl3,shl4;
		YsShellSearchTable search1,search2,search3,search4;

		shl1.AttachSearchTable(&search1);
		shl2.AttachSearchTable(&search2);
		shl3.AttachSearchTable(&search3);
		shl4.AttachSearchTable(&search4);

		if(YSOK==YsBlendShell2(shl1,block,sphere,YSBOOLMINUS,64,64,64,10000) &&
		   YSOK==YsBlendShell2(shl2,shl1,cylinder1,YSBOOLMINUS,64,64,64,10000) &&
		   YSOK==YsBlendShell2(shl3,shl2,cylinder2,YSBOOLMINUS,64,64,64,10000) &&
		   YSOK==YsBlendShell2(shl4,shl3,cylinder3,YSBOOLOR,64,64,64,10000))
		{
			shl1.SaveSrf("shl1.srf");
			shl2.SaveSrf("shl2.srf");
			shl3.SaveSrf("shl3.srf");
			shl4.SaveSrf("shl4.srf");

			if(YSOK==search1.SelfDiagnostic(shl1) &&
			   YSOK==search2.SelfDiagnostic(shl2) &&
			   YSOK==search3.SelfDiagnostic(shl3) &&
			   YSOK==search4.SelfDiagnostic(shl4) &&
			   YSOK==CompareShell(shl1,ref1) &&
			   YSOK==CompareShell(shl2,ref2) &&
			   YSOK==CompareShell(shl3,ref3) &&
			   YSOK==CompareShell(shl4,ref4))
			{
				printf("@@@ TEST RESULT: OK\n");
				return 0;
			}
		}
	}

	printf("@@@ TEST RESULT: ERROR\n");
	return 1;
}

