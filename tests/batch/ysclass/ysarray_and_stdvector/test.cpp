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

YSRESULT ConstTest(const YsArray <int> &array)
{
	if(123456!=array.back())
	{
		fprintf(stderr,"error in back() const\n");
		return YSERR;
	}

	auto data=array.data();
	for(int i=0; i<array.size(); ++i)
	{
		if(array[i]!=data[i])
		{
			fprintf(stderr,"error in data() const\n");
			return YSERR;
		}
	}

	return YSOK;
}

YSRESULT SomethingTest(void)
{
	YSRESULT res=YSOK;

	YsArray <int> array;
	array.push_back(1);
	array.push_back(2);
	array.push_back(3);

	if(3!=array.size())
	{
		fprintf(stderr,"Size is supposed to be 3.\n");
		return YSERR;
	}

	array.back()=100;
	if(array[0]!=1 || array[1]!=2 || array[2]!=100)
	{
		fprintf(stderr,"Array contents are broken.\n");
		res=YSERR;
	}

	int ctr=0;
	for(auto &x : array)
	{
		++ctr;
	}
	if(ctr!=array.size())
	{
		fprintf(stderr,"Error in iterator.\n");
		res=YSERR;
	}

	array.pop_back();
	if(2!=array.size())
	{
		fprintf(stderr,"Error in pop_back().\n");
		fprintf(stderr,"Size must be 2, actually is %d\n",(int)array.size());
		res=YSERR;
	}

	array.push_back(123456);
	res=ConstTest(array);

	array.resize(100);
	ctr=0;
	for(auto &x : array)
	{
		x=ctr;
		++ctr;
	}
	if(ctr!=array.size() || 100!=array.size())
	{
		fprintf(stderr,"Error in resize.\n");
		res=YSERR;
	}

	auto data=array.data();
	for(int i=0; i<array.size(); ++i)
	{
		if(array[i]!=data[i])
		{
			fprintf(stderr,"error in data()\n");
			return YSERR;
		}
		data[i]=100; // Must be non-const
	}

	array.clear();
	ctr=0;
	for(auto &x : array)
	{
		++ctr;
	}
	if(ctr!=array.size() || 0!=array.size())
	{
		fprintf(stderr,"Error in clear.\n");
		res=YSERR;
	}
	if(true!=array.empty())
	{
		fprintf(stderr,"Error in empty.\n");
		res=YSERR;
	}


	if(YSOK!=res)
	{
		fprintf(stderr,"Failed!\n");
	}
	return res;
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
