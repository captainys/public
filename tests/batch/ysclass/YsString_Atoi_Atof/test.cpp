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

YSRESULT TestAtoi(int value,const char str[])
{
	YsString a(str);
	printf("Atoi Value=%d, Calculated=%d\n",value,a.Atoi());
	if(value!=a.Atoi())
	{
		printf("Error!\n");
		return YSERR;
	}
	return YSOK;
}

YSRESULT TestAtoi(void)
{
	YSRESULT res=YSOK;

	if(YSOK!=TestAtoi(100,"100") ||
	   YSOK!=TestAtoi(-100,"-100") ||
	   YSOK!=TestAtoi(1234567,"  1234567xyz") ||
	   YSOK!=TestAtoi(-1234567,"  -1234567xyz") ||
	   YSOK!=TestAtoi(10000000,"+10,000,000") ||
	   YSOK!=TestAtoi(10000000,"10,000,000") ||
	   YSOK!=TestAtoi(-10000000,"-10,000,000"))
	{
		res=YSERR;
		fprintf(stderr,"Failed!\n");
	}
	return res;
}

YSRESULT TestAtol(long long int value,const char str[])
{
	YsString a(str);
	printf("Atol Value=%lld, Calculated=%lld\n",value,a.Atol());
	if(value!=a.Atol())
	{
		printf("Error!\n");
		return YSERR;
	}
	return YSOK;
}

YSRESULT TestAtol(void)
{
	YSRESULT res=YSOK;

	if(YSOK!=TestAtol(10000000000,"10000000000") ||
	   YSOK!=TestAtol(-10000000000,"-10000000000") ||
	   YSOK!=TestAtol(123456700000,"   123456700000xyz") ||
	   YSOK!=TestAtol(-123456700000,"   -123456700000xyz") ||
	   YSOK!=TestAtol(10000000000000,"\t10,000,000,000,000") ||
	   YSOK!=TestAtol(10000000000000,"+10,000,000,000,000") ||
	   YSOK!=TestAtol(-10000000000000,"-10,000,000,000,000"))
	{
		res=YSERR;
		fprintf(stderr,"Failed!\n");
	}
	return res;
}

YSRESULT TestAtof(double value,const char str[])
{
	YsString a(str);
	printf("Atof Value=%lf Calculated=%lf\n",value,a.Atof());
	if(YSTRUE!=YsEqual(value,a.Atof()))
	{
		printf("Error!\n");
		return YSERR;
	}
	return YSOK;
}

YSRESULT TestAtof(void)
{
	YSRESULT res=YSOK;

	if(YSOK!=TestAtof(100000.12345,"100000.12345") ||
	   YSOK!=TestAtof(-100000.12345,"-100000.12345") ||
	   YSOK!=TestAtof(12345670.4567,"  12345670.4567xyz") ||
	   YSOK!=TestAtof(-12345670.4567,"  -12345670.4567xyz") ||
	   YSOK!=TestAtof(100000000.67891,"100,000,000.67891") ||
	   YSOK!=TestAtof(100000000.67891,"+100,000,000.67891") ||
	   YSOK!=TestAtof(-100000000.67891,"-100,000,000.67891") ||
	   YSOK!=TestAtof(12.3456e+3,"12.3456e+3") ||
	   YSOK!=TestAtof(-12.3456e+3,"-12.3456e+3") ||
	   YSOK!=TestAtof(8.7645e+23,"8.7645e+23") ||
	   YSOK!=TestAtof(-8.7645e+23,"-8.7645e+23"))
	{
		res=YSERR;
		fprintf(stderr,"Failed!\n");
	}
	return res;
}

YSRESULT TestAtof32(float value,const char str[])
{
	YsString a(str);
	printf("Atof32 Value=%lf Calculated=%lf\n",value,a.Atof32());
	if(YSTRUE!=YsEqual(value,a.Atof32()))
	{
		printf("Error!\n");
		return YSERR;
	}
	return YSOK;
}

YSRESULT TestAtof32(void)
{
	YSRESULT res=YSOK;

	if(YSOK!=TestAtof32(10001.5,"10001.5") ||
	   YSOK!=TestAtof32(-10001.5,"-10001.5") ||
	   YSOK!=TestAtof32(123.456,"  123.456xyz") ||
	   YSOK!=TestAtof32(-123.456,"  -123.456xyz") ||
	   YSOK!=TestAtof32(1000.678,"1,000.678") ||
	   YSOK!=TestAtof32(1000.678,"+1,000.678") ||
	   YSOK!=TestAtof32(-100001.6,"-100,001.6") ||
	   YSOK!=TestAtof32(12.345e+3,"12.345e+3") ||
	   YSOK!=TestAtof32(-12.345e+3,"-12.345e+3") ||
	   YSOK!=TestAtof32(8.764e+10,"8.764e+10") ||
	   YSOK!=TestAtof32(-8.764e+10,"-8.764e+10"))
	{
		res=YSERR;
		fprintf(stderr,"Failed!\n");
	}
	return res;
}

int main(void)
{
	int nFail=0;
	if(YSOK!=TestAtoi())
	{
		++nFail;
	}
	if(YSOK!=TestAtol())
	{
		++nFail;
	}
	if(YSOK!=TestAtof())
	{
		++nFail;
	}
	if(YSOK!=TestAtof32())
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
