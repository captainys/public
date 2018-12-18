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

const char *const correctPicture[]=
{
	"................................................CCC.......................",
	"..............................................CCC.........................",
	"............................................CCC...........................",
	"...............B..........................CCC.............................",
	"..............B.........................CCC...............................",
	"...A.........BB......................CCCC...................HFFFFFFFFFI...",
	"....AAAA.....B.....................CCC......................HD.......EI...",
	".......AAAAABB...................CCC........................H.D.....E.I...",
	"...........ABAAA...............CCC..........................H..D...E..I...",
	"...........BB..AAAAA.........CCC............................H...D.E...I...",
	"..........BB.......AAAA....CCC..............................H....E....I...",
	"..........B............ACCCC................................H...E.D...I...",
	".........BB...........CCC.AAAAA.............................H..E...D..I...",
	".........B..........CCC.......AAAAA.........................H.E.....D.I...",
	"........BB........CCC.............AAAAA.....................HE.......DI...",
	".......BB.......CCC...................AAAA..................HGGGGGGGGGI...",
	".......B......CCC.........................AAAA............................",
	"......BB...CCCC..............................AAAAA........................",
	"......B..CCC.....................................AAAAA....................",
	".....BBCCC...........................................AAAAA................",
	".....CCC.................................................AAAA.............",
	"...CCC....................................................................",
	".CCC......................................................................",
	"C........................................................................."
};



char vScreen[24][75];

void ClearVirtualScreen(void);
void DrawLine(int x1,int y1,int x2,int y2,char dot);
void ExposeVirtualScreen(void);
YSRESULT VerifyVirtualScreen(void);

YsDrawLineByDDA ddaDriver;

int main(void)
{
	printf("@@@ TESTING: YsDrawLineByDDA\n");

	ClearVirtualScreen();
	DrawLine(3,5,60,20,'A');
	DrawLine(15,3,5,20,'B');
	DrawLine(0,23,50,0,'C');
	DrawLine(60,5,70,15,'D');
	DrawLine(70,5,60,15,'E');
	DrawLine(60,5,70,5,'F');
	DrawLine(70,15,60,15,'G');
	DrawLine(60,5,60,15,'H');
	DrawLine(70,15,70,5,'I');
	ExposeVirtualScreen();

	if(YSOK==VerifyVirtualScreen())
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

void ClearVirtualScreen(void)
{
	int i,j;
	for(i=0; i<24; i++)
	{
		for(j=0; j<75; j++)
		{
			vScreen[i][j]='.';
		}
		vScreen[i][74]=0;
	}
}

void DrawLine(int x1,int y1,int x2,int y2,char dot)
{
	ddaDriver.Set(x1,y1,x2,y2);
	vScreen[y1][x1]=dot;
	while(ddaDriver.ReachedToTheEnd()!=YSTRUE)
	{
		int x,y;
		ddaDriver.MoveOneStep();
		ddaDriver.GetPosition(x,y);
		vScreen[y][x]=dot;
	}
}

void ExposeVirtualScreen(void)
{
	int i;
	for(i=0; i<24; i++)
	{
		printf("%s\n",vScreen[i]);
	}
}

YSRESULT VerifyVirtualScreen(void)
{
	printf("Verifying...\n");
	for(int y=0; y<24; y++)
	{
		for(int x=0; vScreen[y][x]!=0 && correctPicture[y][x]!=0; x++)
		{
			if(vScreen[y][x]!=correctPicture[y][x])
			{
				printf("E\n");
				printf("Error at %d %d\n",x,y);
				return YSERR;
			}
			printf(" ");
		}
		printf("\n");
	}
	return YSOK;
}
