/* ////////////////////////////////////////////////////////////

File Name: yspixelmap.h
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

#ifndef YSSCREENPOLYGON_IS_INCLUDED
#define YSSCREENPOLYGON_IS_INCLUDED
/* { */

#include "ysdef.h"
#include "ysmath.h"

// Declaration /////////////////////////////////////////////
class YsDrawLineByDDA  // Let this class independent
{
public:
	void Set(int x1,int y1,int x2,int y2);
	YSRESULT MoveOneStep(void);
	void GetPosition(int &x,int &y);
	YSBOOL ReachedToTheEnd(void);

protected:
	int x1,y1,x2,y2;
	int dx,dy;
	int vx,vy;
	int x,y,k;
};


////////////////////////////////////////////////////////////

template <const int maxNXBuf,const int hei> class YsScreenPolygon
{
protected:
	int nXBuf[hei];
	int xBuf[hei][maxNXBuf];
	int miny,maxy;

	void AddIntersection(int x,int y);
	YsDrawLineByDDA dda;

public:
	YsScreenPolygon();
	YSRESULT MakeHorizontalIntersectionBuffer(int np,int p[]);
	void GetRangeOfY(int &ymin,int &ymax);
	int GetNumIntersectionAtY(int y);
	int *GetIntersectionBufferAtY(int y);
};

template <const int maxNXBuf,const int hei>
YsScreenPolygon<maxNXBuf,hei>::YsScreenPolygon()
{
	int y;
	for(y=0; y<hei; y++)
	{
		nXBuf[y]=0;
	}
}

template <const int maxNXBuf,const int hei>
YSRESULT YsScreenPolygon<maxNXBuf,hei>::MakeHorizontalIntersectionBuffer
    (int np,int p[])
{
	int i;
	int x,y,x1,y1,x2,y2,vy;
	int lastVy,lastY;

	if(np<3)
	{
		return YSERR;
	}


	for(y=0; y<hei; y++)
	{
		nXBuf[y]=0;
	}

	lastVy=0;
	for(i=0; i<np; i++)
	{
		y1=p[(i*2)+1];
		y2=p[((i+1)%np)*2+1];

		vy=y2-y1;
		if(vy!=0)
		{
			lastVy=vy;
		}
	}


	miny=p[1];
	maxy=p[1];

	for(i=0; i<np; i++)
	{
		x1=p[(i*2)];
		y1=p[(i*2)+1];
		x2=p[((i+1)%np)*2];
		y2=p[((i+1)%np)*2+1];

		miny=(y1<miny ? y1 : miny);
		maxy=(y1>maxy ? y1 : maxy);

		vy=y2-y1;
		if(vy!=0)
		{
			if(vy*lastVy<0)
			{
				AddIntersection(x1,y1);
			}

			dda.Set(x1,y1,x2,y2);
			lastY=y1;
			while(dda.ReachedToTheEnd()!=YSTRUE)
			{
				dda.MoveOneStep();
				dda.GetPosition(x,y);
				if(lastY!=y)
				{
					AddIntersection(x,y);
					lastY=y;
				}
			}

			lastVy=vy;
		}
	}
	return YSOK;
}


template <const int maxNXBuf,const int hei>
void YsScreenPolygon<maxNXBuf,hei>::GetRangeOfY(int &ymin,int &ymax)
{
	ymin=miny;
	ymax=maxy;
}

template <const int maxNXBuf,const int hei>
int YsScreenPolygon<maxNXBuf,hei>::GetNumIntersectionAtY(int y)
{
	if(0<=y && y<hei)
	{
		return nXBuf[y];
	}
	else
	{
		return 0;
	}
}

template <const int maxNXBuf,const int hei>
int *YsScreenPolygon<maxNXBuf,hei>::GetIntersectionBufferAtY(int y)
{
	if(0<=y && y<hei)
	{
		return xBuf[y];
	}
	else
	{
		return NULL;
	}
}

template <const int maxNXBuf,const int hei>
void YsScreenPolygon<maxNXBuf,hei>::AddIntersection(int x,int y)
{
	int i;
	if(0<=y && y<hei)
	{
		if(nXBuf[y]<=0)
		{
			nXBuf[y]=1;
			xBuf[y][0]=x;
		}
		else if(x<xBuf[y][0])
		{
			if(nXBuf[y]>=maxNXBuf)
			{
				nXBuf[y]=maxNXBuf-1;
			}
			for(i=nXBuf[y]; i>0; i--)
			{
				xBuf[y][i]=xBuf[y][i-1];
			}
			xBuf[y][0]=x;
			nXBuf[y]++;
		}
		else if(xBuf[y][nXBuf[y]-1]<=x)
		{
			if(nXBuf[y]<maxNXBuf)
			{
				xBuf[y][nXBuf[y]]=x;
				nXBuf[y]++;
			}
		}
		else
		{
			if(nXBuf[y]>=maxNXBuf)
			{
				nXBuf[y]=maxNXBuf-1;
			}

			for(i=nXBuf[y]; i>1; i--)  // Why 1? Because 0 is already checked.
			{
				xBuf[y][i]=xBuf[y][i-1];
				if(xBuf[y][i-2]<=x && x<xBuf[y][i])
				{
					xBuf[y][i-1]=x;
					break;
				}
			}

			nXBuf[y]++;
		}
	}
}

/* } */
#endif
