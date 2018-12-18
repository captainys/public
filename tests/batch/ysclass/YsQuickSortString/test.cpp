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
	printf("TESTING: YsQuickSortString\n");

	srand(1414877825);

	YsArray <YsWString> value;
	YsArray <YsWString> assoc;

	for(int i=0; i<16; i++)
	{
		value.Increment();
		assoc.Increment();
		for(int j=0; j<4; ++j)
		{
			value.Last().Append('0'+rand()%(10));
		}
		for(int j=0; j<4; ++j)
		{
			assoc.Last().Append('a'+rand()%('z'-'a'));
		}
	}

	auto valueBackUp=value;
	auto assocBackUp=assoc;

	YsQuickSortString <YsWString,YsWString> (value.GetN(),value,assoc);

	for(int i=0; i<value.GetN(); i++)
	{
		printf("%d %ls %ls\n",i,value[i].Txt(),assoc[i].Txt());

		if(0<i && YsWString::Strcmp(value[i-1],value[i])>0)
		{
			fprintf(stderr,"Incorrect order!\n");
			fprintf(stderr,"TEST RESULT: ERROR\n");
			return YSERR;
		}
	}


	int nChecked=0;
	for(auto idx0 : value.AllIndex())
	{
		for(auto idx1 : valueBackUp.AllIndex())
		{
			if(0==YsWString::Strcmp(value[idx0],valueBackUp[idx1]))
			{
				if(0!=YsWString::Strcmp(assoc[idx0],assocBackUp[idx1]))
				{
					fprintf(stderr,"Assoc Value Mismatch!\n");
					fprintf(stderr,"  Sorted Value:%ls  Assoc:%ls\n",value[idx0].Txt(),assoc[idx0].Txt());
					fprintf(stderr,"  Backup      :%ls  Assoc:%ls\n",valueBackUp[idx1].Txt(),assocBackUp[idx1].Txt());
					fprintf(stderr,"TEST RESULT: ERROR\n");
					return YSERR;
				}
			}
		}
	}


	fprintf(stderr,"TEST RESULT: OK\n");
	return YSOK;
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
