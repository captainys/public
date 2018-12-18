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

YSRESULT TestNoSearch(void)
{
	YSRESULT res=YSOK;

	YsEditArray <int> ary;
	auto hd=ary.Create();
	auto key=ary.GetSearchKey(hd);

	auto found=ary.FindObject(key);
	if(nullptr!=found)
	{
		fprintf(stderr,"Supposed to be nullptr.\n");
		res=YSERR;
	}

	if(YSOK!=res)
	{
		fprintf(stderr,"%s Failed!\n",__FUNCTION__);
	}
	return res;
}

YSRESULT TestSearch(void)
{
	YSRESULT res=YSOK;

	YsEditArray <int> ary;
	ary.EnableSearch();

	auto hd=ary.Create();
	auto key=ary.GetSearchKey(hd);

	auto found=ary.FindObject(key);
	if(hd!=found)
	{
		fprintf(stderr,"Object not found.\n");
		res=YSERR;
	}

	if(YSOK!=res)
	{
		fprintf(stderr,"%s Failed!\n",__FUNCTION__);
	}
	return res;
}

YSRESULT TestFreeze(void)
{
	YSRESULT res=YSOK;

	YsEditArray <int> ary;
	ary.EnableSearch();

	auto hd=ary.Create();
	auto key=ary.GetSearchKey(hd);

	ary.Freeze(hd);
	if(nullptr!=ary.FindObject(key))
	{
		fprintf(stderr,"Supposed to be nullptr.\n");
		res=YSERR;
	}
	if(nullptr==ary.FindFrozenObject(key))
	{
		fprintf(stderr,"Object not found.\n");
		res=YSERR;
	}
	if(nullptr==ary.FindObjectMaybeFrozen(key))
	{
		fprintf(stderr,"Object not found.\n");
		res=YSERR;
	}



	ary.Melt(hd);
	if(nullptr==ary.FindObject(key))
	{
		fprintf(stderr,"Object not found.\n");
		res=YSERR;
	}
	if(nullptr!=ary.FindFrozenObject(key))
	{
		fprintf(stderr,"Supposed to be nullptr.\n");
		res=YSERR;
	}
	if(nullptr==ary.FindObjectMaybeFrozen(key))
	{
		fprintf(stderr,"Object not found.\n");
		res=YSERR;
	}



	if(YSOK!=res)
	{
		fprintf(stderr,"%s Failed!\n",__FUNCTION__);
	}
	return res;
}

int main(void)
{
	int nFail=0;
	if(YSOK!=TestNoSearch())
	{
		++nFail;
	}

	if(YSOK!=TestSearch())
	{
		++nFail;
	}

	if(YSOK!=TestFreeze())
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
