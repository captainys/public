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


YSRESULT TestAssembleCommandLine(void)
{
	{
		const char *const param[]=
		{
			"hm",
			"/g",
			"test.cpp",
		};
		YsArray <YsString> argv;
		for(auto p : param)
		{
			argv.push_back(p);
		}
		auto cmdLine=YsAssembleCommandLine(argv);
		printf("%s\n",cmdLine.data());

		if(0!=cmdLine.Strcmp("hm /g test.cpp"))
		{
			printf("Wrong command line: %s\n",cmdLine.data());
			return YSERR;
		}
		printf("Pass\n");
	}
	{
		const char *const param[]=
		{
			"ysgebl64",
			"d:/file with space/datafile.srf"
		};
		YsArray <YsString> argv;
		for(auto p : param)
		{
			argv.push_back(p);
		}
		auto cmdLine=YsAssembleCommandLine(argv);
		printf("%s\n",cmdLine.data());

		if(0!=cmdLine.Strcmp("ysgebl64 \"d:/file with space/datafile.srf\""))
		{
			printf("Wrong command line: %s\n",cmdLine.data());
			return YSERR;
		}
		printf("Pass\n");
	}
	return YSOK;
}


int main(void)
{
	int nFail=0;
	if(YSOK!=TestAssembleCommandLine())
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
