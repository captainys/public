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

YSRESULT SomethingTest(void)
{
	YsString str="word {group} { } ( 12345)  \"This is a pen.\"";
	auto args=str.Argv();

	for(auto &arg : args)
	{
		printf("%s\n",arg.Txt());
	}

	bool passed=true;
	if(7!=args.GetN())
	{
		fprintf(stderr,"Wrong argument count.\n");
		passed=false;
	}
	if(0!=args[0].Strcmp("word") ||
	   0!=args[1].Strcmp("{group}") ||
	   0!=args[2].Strcmp("{") ||
	   0!=args[3].Strcmp("}") ||
	   0!=args[4].Strcmp("(") ||
	   0!=args[5].Strcmp("12345)") ||
	   0!=args[6].Strcmp("This is a pen."))
	{
		fprintf(stderr,"Wrong argument(s).\n");
		passed=false;
	}

	if(passed)
	{
		return YSOK;
	}
	else
	{
		fprintf(stderr,"Failed!\n");
		return YSERR;
	}
}

int main(void)
{
	int nFail=0;
	if(YSOK!=SomethingTest())
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
