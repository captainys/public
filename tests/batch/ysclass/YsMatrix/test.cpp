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
 +1.00000000, +2.00000000, +3.00000000,
 +4.00000000, +6.00000000, +5.00000000,
 +7.00000000, +8.00000000, +9.00000000
};

const double correctMatB[]=
{
 -0.77777778, -0.33333333, +0.44444444,
 +0.05555556, +0.66666667, -0.38888889,
 +0.55555556, -0.33333333, +0.11111111
};

const double correctMatAB[]=
{
 +1.00000000, -0.00000000, +0.00000000,
 +0.00000000, +1.00000000, +0.00000000,
 -0.00000000, +0.00000000, +1.00000000
};

void ShowMatrix(YsMatrix &m,char caption[]);
YSRESULT CompareMatrix(const YsMatrix &m,const double correct[]);

int main(void)
{
	printf("@@@ TESTING: YsMatrix\n");

	YsMatrix a,b,c;
	double matTemplate[9]=
	{
		1.0,2.0,3.0,
		4.0,6.0,5.0,
		7.0,8.0,9.0
	};

	a.Create(3,3,matTemplate);

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

void ShowMatrix(YsMatrix &m,char caption[])
{
	int r,c;
	double v;

	printf("[%s]\n",caption);

	for(r=1; r<=3; r++)
	{
		for(c=1; c<=3; c++)
		{
			v=m.v(r,c);
			printf(" %+.8lf",v);
		}
		printf("\n");
	}
}

YSRESULT CompareMatrix(const YsMatrix &m,const double correct[])
{
	int index=0;
	printf("Verifying...\n");
	for(int r=1; r<=3; r++)
	{
		for(int c=1; c<=3; c++)
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
