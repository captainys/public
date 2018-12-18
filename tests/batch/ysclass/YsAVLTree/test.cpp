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

YSRESULT SortAndPrint(int numItem)
{
	YsAVLTree <int,int> tree;

	for(int i=0; i<numItem; i++)
	{
		tree.Insert(rand()%100,0);
	}

	printf("%d items\n",numItem);

	int i0,prev;
	i0=0;

	for(auto ndHd=tree.First(); ndHd!=nullptr; ndHd=tree.FindNext(ndHd))
	{
		if(numItem<=100)
		{
			printf("%d ",tree.GetKey(ndHd));
		}
		if(0<i0 && tree.GetKey(ndHd)<prev)
		{
			fprintf(stderr,"Error!  Incorrect order!\n");
			return YSERR;
		}

		i0++;
		prev=tree.GetKey(ndHd);
	}
	printf("\n");

	if(i0!=numItem)
	{
		fprintf(stderr,"Error!  Node count inconsistency.\n");
		return YSERR;
	}

	i0=0;

	for(auto ndHd=tree.Last(); ndHd!=nullptr; ndHd=tree.FindPrev(ndHd))
	{
		if(numItem<=100)
		{
			printf("%d ",tree.GetKey(ndHd));
		}

		if(0<i0 && tree.GetKey(ndHd)>prev)
		{
			fprintf(stderr,"Error!  Incorrect order!\n");
			return YSERR;
		}

		i0++;
		prev=tree.GetKey(ndHd);
	}
	printf("\n");

	return YSOK;
}

YSRESULT SomethingTest(void)
{
	YSRESULT res0,res1,res2,res3,res4,res5;

	printf("TESTING: YsAVLTree\n");

	srand(79154726);
	res0=SortAndPrint(20);
	res1=SortAndPrint(3);
	res2=SortAndPrint(2);
	res3=SortAndPrint(1);
	res4=SortAndPrint(0);
	res5=SortAndPrint(1000000);

	if(YSOK==res0 && YSOK==res1 && YSOK==res2 && YSOK==res3 && YSOK==res4 && YSOK==res5)
	{
		printf("TEST RESULT: OK\n");
		return YSOK;
	}
	else
	{
		printf("TEST RESULT: ERROR\n");
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
