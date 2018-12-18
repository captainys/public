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


int main(void)
{
	YsString str="word {group} { },, ( 12345)  \"This is a pen.\"  \\\"";

	const char *const parsed[]=
	{
		"word",
		"{",
		"group",
		"}",
		"{",
		"}",
		"",
		"",
		"(",
		"12345",
		")",
		"This is a pen.",
		"\"",
		nullptr
	};

	YsStringParser <YsString> parser;
	parser.Parse(str);

	int nFail=0;
	for(int i=0; i<parser.Argv().GetN(); ++i)
	{
		auto arg=parser.Argv()[i];
		printf("%s\n",arg.Txt());
		if(0!=arg.Strcmp(parsed[i]))
		{
			fprintf(stderr,"Error!\n");
			fprintf(stderr,"  Should be \"%s\"\n",parsed[i]); 
			++nFail;
		}
	}

	printf("%d failed\n",nFail);
	if(0==nFail)
	{
		return 0;
	}
	else
	{
		return 1;
	}
}


