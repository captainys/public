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


const double correctMatA[]=
{
	 +0.73919892, -0.57322330, -0.35355339, +0.00000000,
	 +0.61237244, +0.35355339, +0.70710678, +0.00000000,
	 -0.28033009, -0.73919892, +0.61237244, +0.00000000,
	 +0.00000000, +0.00000000, +0.00000000, +1.00000000
};

const double correctMatB[]=
{
	 +0.73919892, +0.61237244, -0.28033009, +0.00000000,
	 -0.57322330, +0.35355339, -0.73919892, +0.00000000,
	 -0.35355339, +0.70710678, +0.61237244, +0.00000000,
	 +0.00000000, +0.00000000, +0.00000000, +1.00000000
};

const double correctMatAB[]=
{
	 +1.00000000, +0.00000000, -0.00000000, +0.00000000,
	 +0.00000000, +1.00000000, +0.00000000, +0.00000000,
	 -0.00000000, +0.00000000, +1.00000000, +0.00000000,
	 +0.00000000, +0.00000000, +0.00000000, +1.00000000
};

void ShowMatrix(YsMatrix4x4 &m,char caption[]);
YSRESULT CompareMatrix(const YsMatrix4x4 &m,const double correct[]);

int main(void)
{
	printf("@@@ TESTING: YsMatrix4x4\n");


	YsMatrix4x4 a,b,c;

	a.Initialize();
	a.RotateXZ(YsDegToRad(30.0));
	a.RotateZY(YsDegToRad(45.0));
	a.RotateXY(YsDegToRad(60.0));

	b=a;
	b.Invert();

	c=a*b;

	ShowMatrix(a,"Matrix A");
	ShowMatrix(b,"Matrix B is inverse of Matrix A");
	ShowMatrix(c,"Matrix A x Matrix B must be an identity matrix");

	if(YSOK==CompareMatrix(a,correctMatA) &&
	   YSOK==CompareMatrix(b,correctMatB) &&
	   YSOK==CompareMatrix(c,correctMatAB))
	{
		printf("@@@ TEST RESULT: OK\n");
		return 0;
	}
	else
	{
		printf("@@@ TEST RESULT: ERROR\n");
		return 1;
	}
}

void ShowMatrix(YsMatrix4x4 &m,char caption[])
{
	int r,c;
	double v;

	printf("[%s]\n",caption);

	for(r=1; r<=4; r++)
	{
		for(c=1; c<=4; c++)
		{
			v=m.v(r,c);
			printf(" %+.8lf",v);
		}
		printf("\n");
	}
}

YSRESULT CompareMatrix(const YsMatrix4x4 &m,const double correct[])
{
	int index=0;
	printf("Verifying...\n");
	for(int r=1; r<=4; r++)
	{
		for(int c=1; c<=4; c++)
		{
			if(YSOK!=YsEqual(correct[index],m.v(r,c)))
			{
				printf("Error! Row %d Column %d\n",r,c);
				printf("Calculated: %lf\n",m.v(r,c));
				printf("Correct:    %lf\n",correct[index]);
				return YSERR;
			}
			index++;
		}
	}

	return YSOK;
}
