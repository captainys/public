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
#include <time.h>
#include <vector>

#include <ysclass.h>


static const int nSource=1024;
static int intSource[nSource];

YSRESULT YsArrayTest(void)
{
	YSRESULT res=YSOK;

	YsArray <int> ary1;
	YsArray <int,7> ary7;
	YsArray <int,4> ary4;

	for(auto i : intSource)
	{
		ary7.Add(i);
	}
	ary1=ary7;
	ary4=ary1;
	ary7=ary7;


	YsTightArray <int> aryT;
	aryT=ary4;
	aryT=aryT;

	YsArray <int,8> ary8(aryT);



	std::vector <int> vec;
	for(auto i : intSource)
	{
		vec.push_back(i);
	}
	YsArray <int,10> ary10=vec;


	if(ary1.size()!=ary7.size() || ary7.size()!=ary4.size() || ary4.size()!=aryT.size() || aryT.size()!=ary8.size())
	{
		fprintf(stderr,"Sizes don't match!\n");
		return YSERR;
	}
	printf("Passed size tests.\n");

	for(auto idx : ary1.AllIndex())
	{
		if(ary1[idx]!=ary7[idx] || ary7[idx]!=ary4[idx] || ary4[idx]!=aryT[idx] || aryT[idx]!=ary8[idx])
		{
			fprintf(stderr,"Contents don't match!\n");
			return YSERR;
		}
	}
	printf("Passed contents tests.\n");


	{
		auto addTest=aryT;
		addTest.Add(addTest);
		if(addTest.size()!=aryT.size()*2)
		{
			fprintf(stderr,"Size doesn't match after self-add (YsTightArray)!\n");
			return YSERR;
		}
		for(auto idx : addTest.AllIndex())
		{
			if(addTest[idx]!=aryT.GetCyclic(idx))
			{
				fprintf(stderr,"Contents don't match after self-add (YsTightArray)!\n");
				return YSERR;
			}
		}
	}
	{
		auto addTest=ary4;
		addTest.Add(addTest);
		if(addTest.size()!=aryT.size()*2)
		{
			fprintf(stderr,"Size doesn't match after self-add (YsArray)!\n");
			return YSERR;
		}
		for(auto idx : addTest.AllIndex())
		{
			if(addTest[idx]!=aryT.GetCyclic(idx))
			{
				fprintf(stderr,"Contents don't match after self-add (YsArray)!\n");
				return YSERR;
			}
		}
	}



	if(YSOK!=res)
	{
		fprintf(stderr,"Failed!\n");
	}
	return res;
}

YSRESULT AddOperatorTest(void)
{
	YsArray <int> a,b;
	for(int i=0; i<10; ++i)
	{
		a.push_back(intSource[i]);
	}
	for(int i=10; i<20; ++i)
	{
		b.push_back(intSource[i]);
	}

	auto c=a+b;
	if(c.size()!=a.size()+b.size())
	{
		fprintf(stderr,"c.size() does not match a.size()+b.size()\n");
		return YSERR;
	}

	for(int i=0; i<c.size(); ++i)
	{
		if(c[i]!=intSource[i])
		{
			fprintf(stderr,"Contents were not copied correctly\n");
			return YSERR;
		}
	}
	printf("Passed opertor+ test\n");
	return YSOK;
}


void YsArrayMaskConversion(YsArrayMask <int>)
{
	// Just make sure it is compiled.
}

void YsConstArrayMaskConversion(YsConstArrayMask <int>)
{
	// Just make sure it is compiled.
}

int main(void)
{
	srand((int)time(nullptr));
	for(auto &i : intSource)
	{
		i=rand();
	}

	int nFail=0;
	if(YSOK!=YsArrayTest())
	{
		++nFail;
	}

	if(YSOK!=AddOperatorTest())
	{
		++nFail;
	}

	YsArray <int> ary;
	YsArrayMaskConversion(ary);
	YsConstArrayMaskConversion(ary);

	YsArrayMask <int> aryMask=ary;
	YsConstArrayMask <int> constAryMask=ary;

	printf("%d failed.\n",nFail);
	if(0<nFail)
	{
		return 1;
	}
	return 0;
}
