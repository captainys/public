/* ////////////////////////////////////////////////////////////

File Name: makec.cpp
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


void MakeC(const char fn[])
{
	FILE *fp=fopen(fn,"r");

	YsString ful(fn);
	YsString pth,fil;
	ful.SeparatePathFile(pth,fil);
	fil.RemoveExtension();

	int nLine=0;

	printf("const char *YSGLSL_%s[]=\n",fil.Txt());
	printf("{\n");

	YsString str;
	while(NULL!=str.Fgets(fp))
	{
		printf("\t\"%s\\n\",\n",str.Txt());
		++nLine;
	}

	printf("};\n");

	printf("const int YSGLSL_%s_nLine=%d;\n",fil.Txt(),nLine);
}


int main(int ac,char *av[])
{
	printf("#include \"ysgldef.h\"\n");
	printf("#include \"ysglslprogram.h\"\n");

	for(int i=1; i<ac; ++i)
	{
		MakeC(av[i]);
	}
	return 0;
}

