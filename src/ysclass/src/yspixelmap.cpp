/* ////////////////////////////////////////////////////////////

File Name: yspixelmap.cpp
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

#include "yspixelmap.h"

// Implementation //////////////////////////////////////////
void YsDrawLineByDDA::Set(int ix1,int iy1,int ix2,int iy2)
{
	x1=ix1;
	y1=iy1;
	x2=ix2;
	y2=iy2;

	dx=YsAbs(x2-x1);
	dy=YsAbs(y2-y1);
	vx=(x2-x1!=0 ? (x2-x1)/dx : 0);
	vy=(y2-y1!=0 ? (y2-y1)/dy : 0);

	x=x1;
	y=y1;
	k=0;
}

YSRESULT YsDrawLineByDDA::MoveOneStep(void)
{
	// Based on dx*y-dy*x=0

	if(x==x2 && y==y2)
	{
		return YSERR;
	}

	if(dx==0 || dy==0)
	{
		x+=vx;
		y+=vy;
	}
	else
	{
		if(k==0)
		{
			x+=vx;
			k-=dy;
			y+=vy;
			k+=dx;
		}
		else if(k>0)
		{
			x+=vx;
			k-=dy;
		}
		else //if(k<0)
		{
			y+=vy;
			k+=dx;
		}
	}

	return YSOK;
}

void YsDrawLineByDDA::GetPosition(int &ix,int &iy)
{
	ix=x;
	iy=y;
}

YSBOOL YsDrawLineByDDA::ReachedToTheEnd(void)
{
	if(x==x2 && y==y2)
	{
		return YSTRUE;
	}
	else
	{
		return YSFALSE;
	}
}

