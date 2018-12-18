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


YSRESULT TestSegmentedArray(void)
{
	printf("@@@ TESTING: YsSegmentedArray\n");


	YSRESULT res=YSOK;

	YsSegmentedArray <int,3> intSegAry; // BitShift=3 -> Unit=8

	for(int i=0; i<16; i++)
	{
		intSegAry.Increment();
		printf("GetN=%d  NAvailable=%d\n",(int)intSegAry.GetN(),(int)intSegAry.NAvailable());
	}

	int *ptr=&intSegAry[8];

	for(int i=0; i<16; i++)
	{
		intSegAry.Increment();
		printf("GetN=%d  NAvailable=%d\n",(int)intSegAry.GetN(),(int)intSegAry.NAvailable());
	}

	if(ptr==&intSegAry[8])
	{
		printf("Pointer didn't change (as expected).\n");
	}
	else
	{
		printf("Error!  Pointer moved!\n");
		res=YSERR;
	}

	for(int i=0; i<32; i++)
	{
		intSegAry[i]=i;
	}
	for(int i=0; i<32; i++)
	{
		printf("%d ",intSegAry[i]);
	}
	printf("\n");


	for(int i=0; i<16; i++)
	{
		intSegAry.DeleteLast();
		printf("GetN=%d  NAvailable=%d\n",(int)intSegAry.GetN(),(int)intSegAry.NAvailable());
	}

	if(16!=intSegAry.GetN() || 24!=intSegAry.NAvailable())
	{
		printf("Size contradicts from expected values.\n");
		res=YSERR;
	}

	if(ptr==&intSegAry[8])
	{
		printf("Pointer didn't change (as expected).\n");
	}
	else
	{
		printf("Error!  Pointer moved!\n");
		res=YSERR;
	}


	// Intentionally make some unused items.
	intSegAry.DeleteLast();
	intSegAry.DeleteLast();
	intSegAry.DeleteLast();
	printf("GetN=%d  NAvailable=%d\n",(int)intSegAry.GetN(),(int)intSegAry.NAvailable());

	intSegAry.Shrink();
	printf("GetN=%d  NAvailable=%d\n",(int)intSegAry.GetN(),(int)intSegAry.NAvailable());
	if((intSegAry.GetN()+7)/8!=intSegAry.NAvailable()/8)
	{
		printf("Shrink failed!\n");
		res=YSERR;
	}


	YsSegmentedArray <int,3> sameBitShiftArray; // BitShift=3 -> Unit=8
	YsSegmentedArray <int,2> differentBitShiftArray; // BitShift=2 -> Unit=4
	sameBitShiftArray=intSegAry;
	differentBitShiftArray=intSegAry;

	if(intSegAry.GetN()!=sameBitShiftArray.GetN() || intSegAry.GetN()!=differentBitShiftArray.GetN())
	{
		printf("Copy failed! (size)\n");
		res=YSERR;
	}

	for(YSSIZE_T i=0; i<intSegAry.GetN(); i++)
	{
		if(intSegAry[i]!=sameBitShiftArray[i] ||
		   intSegAry[i]!=differentBitShiftArray[i])
		{
			printf("Copy failed (contents)!\n");
			res=YSERR;
		}
	}

	if(YSOK==res)
	{
		printf("@@@ TEST RESULT: OK\n");
	}
	else
	{
		printf("@@@ TEST RESULT: ERROR\n");
	}

	return res;
}



int main(void)
{
	int nFail=0;
	if(YSOK!=TestSegmentedArray())
	{
		++nFail;
	}


}
