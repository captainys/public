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

#include <ysclass11.h>

YSRESULT BasicTest(YsThreadPool &thrPool)
{
	int r[48]=
	{
		13,47,89,01,68,51,23,04,56,80,
		17,01,34,10,23,19,41,81,23,48,
		15,68,90,61,85,50,17,12,30,16,
		03,46,95,13,49,56,13,18,23,74,
		01,63,95,76,19,10,23,65
	};

	YSRESULT res=YSOK;
	YsSimpleParallelMergeSort(48,r,thrPool);

	for(auto R : r)
	{
		printf("%d\n",R);
	}

	for(int i=0; i<47; ++i)
	{
		if(r[i]>r[i+1])
		{
			fprintf(stderr,"Basic Sorting failed.\n");
			res=YSERR;
		}
	}

	if(YSOK!=res)
	{
		fprintf(stderr,"Failed!\n");
	}
	return res;
}
YSRESULT TestSimple(YsThreadPool &thrPool)
{
	YSRESULT res=YSOK;

	printf("%s\n",__FUNCTION__);

	std::vector <int> key;
	for(int i=0; i<8000000; ++i)
	{
		key.push_back(rand());
	}

	printf("Sorting\n");
	if(YSOK!=YsSimpleParallelMergeSort(key.size(),key.data(),thrPool))
	{
		fprintf(stderr,"YsMergeSort returned error.\n");
		return YSERR;
	}

	printf("Verifying\n");
	for(int i=0; i<key.size()-1; ++i)
	{
		if(key[i]>key[i+1])
		{
			res=YSERR;
			fprintf(stderr,"Keys were not sorted.\n");
		}
	}

	printf("Done\n");
	if(YSOK!=res)
	{
		fprintf(stderr,"Failed!\n");
	}
	return res;
}

YSRESULT TestSimpleWithValue(YsThreadPool &thrPool)
{
	YSRESULT res=YSOK;

	printf("%s\n",__FUNCTION__);

	std::vector <int> key;
	std::vector <YsString> value;
	for(int i=0; i<8000000; ++i)
	{
		auto k=rand();

		key.push_back(k);

		YsString str;
		str.Printf("%d",k);
		value.push_back(str);
	}

	printf("Sorting\n");
	if(YSOK!=YsSimpleParallelMergeSort(key.size(),key.data(),value.data(),thrPool))
	{
		fprintf(stderr,"YsMergeSort returned error.\n");
		return YSERR;
	}

	printf("Verifying\n");
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

	printf("Done\n");
	if(YSOK!=res)
	{
		fprintf(stderr,"Failed!\n");
	}
	return res;
}

YSRESULT TestString(YsThreadPool &thrPool)
{
	YSRESULT res=YSOK;

	printf("%s\n",__FUNCTION__);

	std::vector <YsString> key;
	for(int i=0; i<8000000; ++i)
	{
		auto k=rand()%50000;

		YsString str;
		str.Printf("%05d",k);
		key.push_back(str);
	}

	printf("Sorting\n");
	if(YSOK!=YsSimpleParallelMergeSort(key.size(),key.data(),thrPool))
	{
		fprintf(stderr,"YsMergeSort returned error.\n");
		return YSERR;
	}

	printf("Verifying\n");
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

	printf("Done\n");
	if(YSOK!=res)
	{
		fprintf(stderr,"Failed!\n");
	}
	return res;
}

int main(void)
{
	int nFail=0;
	{
		fprintf(stderr,"Testing with 8 threads\n");
		YsThreadPool thrPool(8);

		if(YSOK!=BasicTest(thrPool))
		{
			++nFail;
		}
		if(YSOK!=TestSimple(thrPool))
		{
			++nFail;
		}
		if(YSOK!=TestSimpleWithValue(thrPool))
		{
			++nFail;
		}
		if(YSOK!=TestString(thrPool))
		{
			++nFail;
		}
	}

	{
		fprintf(stderr,"Testing with 7 threads\n");
		YsThreadPool thrPool(7);

		if(YSOK!=BasicTest(thrPool))
		{
			++nFail;
		}
		if(YSOK!=TestSimple(thrPool))
		{
			++nFail;
		}
		if(YSOK!=TestSimpleWithValue(thrPool))
		{
			++nFail;
		}
		if(YSOK!=TestString(thrPool))
		{
			++nFail;
		}
	}

	printf("%d failed.\n",nFail);
	if(0<nFail)
	{
		return 1;
	}
	return 0;
}
