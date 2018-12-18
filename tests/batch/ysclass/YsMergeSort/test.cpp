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

#include <stdio.h>
#include <stdlib.h>

YSRESULT TestSimple(void)
{
	YSRESULT res=YSOK;

	printf("%s\n",__FUNCTION__);

	std::vector <int> key;
	for(int i=0; i<10000; ++i)
	{
		key.push_back(rand());
	}

	if(YSOK!=YsSimpleMergeSort(key.size(),key.data()))
	{
		fprintf(stderr,"YsMergeSort returned error.\n");
		return YSERR;
	}

	for(int i=0; i<key.size()-1; ++i)
	{
		if(key[i]>key[i+1])
		{
			res=YSERR;
			fprintf(stderr,"Keys were not sorted.\n");
		}
	}

	if(YSOK!=res)
	{
		fprintf(stderr,"Failed!\n");
	}
	return res;
}

YSRESULT TestSimpleWithValue(void)
{
	YSRESULT res=YSOK;

	printf("%s\n",__FUNCTION__);

	std::vector <int> key;
	std::vector <YsString> value;
	for(int i=0; i<10000; ++i)
	{
		auto k=rand();

		key.push_back(k);

		YsString str;
		str.Printf("%d",k);
		value.push_back(str);
	}

	if(YSOK!=YsSimpleMergeSort(key.size(),key.data(),value.data()))
	{
		fprintf(stderr,"YsMergeSort returned error.\n");
		return YSERR;
	}

	for(int i=0; i<key.size()-1; ++i)
	{
		if(key[i]>key[i+1])
		{
			res=YSERR;
			fprintf(stderr,"Keys were not sorted.\n");
		}
	}
	for(int i=0; i<key.size(); ++i)
	{
		YsString str;
		str.Printf("%d",key[i]);
		if(str!=value[i])
		{
			res=YSERR;
			fprintf(stderr,"Values were not sorted.\n");
		}
	}

	if(YSOK!=res)
	{
		fprintf(stderr,"Failed!\n");
	}
	return res;
}

YSRESULT TestString(void)
{
	YSRESULT res=YSOK;

	printf("%s\n",__FUNCTION__);

	std::vector <YsString> key;
	for(int i=0; i<10000; ++i)
	{
		auto k=rand()%50000;

		YsString str;
		str.Printf("%05d",k);
		key.push_back(str);
	}

	if(YSOK!=YsSimpleMergeSort(key.size(),key.data()))
	{
		fprintf(stderr,"YsMergeSort returned error.\n");
		return YSERR;
	}

	for(int i=0; i<key.size()-1; ++i)
	{
		int k0=atoi(key[i]);
		int k1=atoi(key[i+1]);
		if(k0>k1)
		{
			res=YSERR;
			fprintf(stderr,"Keys were not sorted.\n");
		}
	}

	if(YSOK!=res)
	{
		fprintf(stderr,"Failed!\n");
	}
	return res;
}

int main(void)
{
	srand(20170508);

	int nFail=0;
	if(YSOK!=TestSimple())
	{
		++nFail;
	}
	if(YSOK!=TestSimpleWithValue())
	{
		++nFail;
	}
	if(YSOK!=TestString())
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
