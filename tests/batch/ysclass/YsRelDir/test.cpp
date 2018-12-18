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
#include <ysreldir.h>

YSRESULT TestYsMakeRelativePath(const char fulPath[],const char relativeTo[],const char mustBe[])
{
	YsString relPath;
	YsMakeRelativePath(relPath,fulPath,relativeTo);
	printf("Full Path: %s\n",fulPath);
	printf("Relative To: %s\n",relativeTo);
	printf("Relative Path: %s\n",relPath.Txt());

	if(0==relPath.Strcmp(mustBe))
	{
		printf("OK!\n");
		return YSOK;
	}
	else
	{
		fprintf(stderr,"Error!\n");
		fprintf(stderr,"  Relative Path must be: %s\n",mustBe);
		return YSERR;
	}
}

YSRESULT TestYsMakeRelativePath(const wchar_t fulPath[],const wchar_t relativeTo[],const wchar_t mustBe[])
{
	YsWString relPath;
	YsMakeRelativePath(relPath,fulPath,relativeTo);
	printf("Full Path: %ls\n",fulPath);
	printf("Relative To: %ls\n",relativeTo);
	printf("Relative Path: %ls\n",relPath.Txt());

	if(0==relPath.Strcmp(mustBe))
	{
		printf("OK!\n");
		return YSOK;
	}
	else
	{
		fprintf(stderr,"Error!\n");
		fprintf(stderr,"  Relative Path must be: %ls\n",mustBe);
		return YSERR;
	}
}

int main(void)
{
	int nFail=0;

	if(YSOK!=TestYsMakeRelativePath(
	   "e:\\trunk\\data\\Aircraft\\PittsS2B\\refbmp\\front.png",
	   "e:\\trunk\\data\\Aircraft\\PittsS2B",
	   "refbmp\\front.png"))
	{
		++nFail;
	}

	if(YSOK!=TestYsMakeRelativePath(
	    "e:\\trunk\\data\\Aircraft\\PittsS2B\\refbmp\\front.png",
	    "e:\\trunk\\data\\Aircraft\\Concorde",
	    "../PittsS2B\\refbmp\\front.png"))
	{
		++nFail;
	}

	if(YSOK!=TestYsMakeRelativePath(
	   L"e:\\trunk\\data\\Aircraft\\PittsS2B\\refbmp\\front.png",
	   L"e:\\trunk\\data\\Aircraft\\PittsS2B",
	   L"refbmp\\front.png"))
	{
		++nFail;
	}

	if(YSOK!=TestYsMakeRelativePath(
	    L"e:\\trunk\\data\\Aircraft\\PittsS2B\\refbmp\\front.png",
	    L"e:\\trunk\\data\\Aircraft\\Concorde",
	    L"../PittsS2B\\refbmp\\front.png"))
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
