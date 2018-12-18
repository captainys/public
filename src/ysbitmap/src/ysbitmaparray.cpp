/* ////////////////////////////////////////////////////////////

File Name: ysbitmaparray.cpp
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

#include "ysbitmaparray.h"


YsBitmapArray::YsBitmapArray()
{
	bmpArray=NULL;
	CleanUp();
}

YsBitmapArray::YsBitmapArray(const YsBitmapArray &from)
{
	bmpArray=NULL;
	CopyFrom(from);
}

const YsBitmapArray &YsBitmapArray::operator=(const YsBitmapArray &from)
{
	CopyFrom(from);
	return *this;
}

void YsBitmapArray::CopyFrom(const YsBitmapArray &from)
{
	CleanUp();

	totalWid=from.totalWid;
	totalHei=from.totalHei;
	tileWid=from.tileWid;
	tileHei=from.tileHei;
	nDivX=from.nDivX;
	nDivY=from.nDivY;

	if(0<nDivX*nDivY)
	{
		bmpArray=new YsBitmap [nDivX*nDivY];
		for(int i=0; i<nDivX*nDivY; ++i)
		{
			bmpArray[i]=from.bmpArray[i];
		}
	}
}

YsBitmapArray::~YsBitmapArray()
{
	ResizeArray(0);
}

void YsBitmapArray::CleanUp(void)
{
	totalWid=0;
	totalHei=0;
	tileWid=0;
	tileHei=0;
	nDivX=0;
	nDivY=0;
	if(NULL!=bmpArray)
	{
		delete [] bmpArray;
		bmpArray=NULL;
	}
}

YSRESULT YsBitmapArray::SetAndBreakUp(const YsBitmap &from,int maxBlockWidth,int maxBlockHeight)
{
	CleanUp();
	totalWid=from.GetWidth();
	totalHei=from.GetHeight();

	int blockWid=1,blockHei=1;
	while(blockWid*2<=maxBlockWidth)
	{
		blockWid*=2;
	}
	while(blockHei*2<=maxBlockHeight)
	{
		blockHei*=2;
	}

	totalWid=from.GetWidth();
	totalHei=from.GetHeight();
	nDivX=(totalWid+blockWid-1)/blockWid;
	nDivY=(totalHei+blockHei-1)/blockHei;

	if(0==nDivX || 0==nDivY)
	{
		nDivX=0;
		nDivY=0;
		return YSERR;
	}

	tileWid=blockWid;
	tileHei=blockHei;

	ResizeArray(nDivX*nDivY);
	for(int y=0; y<nDivY; ++y)
	{
		const int fromY0=y*blockHei;
		int tileHei;
		if(blockHei<=totalHei-fromY0)
		{
			tileHei=blockHei;
		}
		else
		{
			tileHei=1;
			while(tileHei<totalHei-fromY0)
			{
				tileHei*=2;
			}
		}

		for(int x=0; x<nDivX; ++x)
		{
			const int fromX0=x*blockWid;

			int offset=y*nDivX+x;

			int tileWid;
			if(blockWid<=totalWid-fromX0)
			{
				tileWid=blockWid;
			}
			else
			{
				tileWid=1;
				while(tileWid<totalWid-fromX0)
				{
					tileWid*=2;
				}
			}

			YsBitmap &tile=bmpArray[offset];
			tile.PrepareBitmap(tileWid,tileHei);
			tile.Clear(0,0,0,0);

			tile.Copy(from,fromX0,fromY0,0,0,tileWid,tileHei);
		}
	}

	return YSOK;
}

void YsBitmapArray::ResizeArray(int n)
{
	if(NULL!=bmpArray)
	{
		delete [] bmpArray;
		bmpArray=NULL;
	}
	if(0<n)
	{
		bmpArray=new YsBitmap [n];
	}
}

int YsBitmapArray::GetTotalWidth(void) const
{
	return totalWid;
}

int YsBitmapArray::GetTotalHeight(void) const
{
	return totalHei;
}

int YsBitmapArray::GetNumBlockX(void) const
{
	return nDivX;
}

int YsBitmapArray::GetNumBlockY(void) const
{
	return nDivY;
}

int YsBitmapArray::GetTileWidth(void) const
{
	return tileWid;
}

int YsBitmapArray::GetTileHeight(void) const
{
	return tileHei;
}

const YsBitmap *YsBitmapArray::GetTileBitmap(int x,int y) const
{
	if(0<=x && x<GetNumBlockX() && 0<=y && y<GetNumBlockY())
	{
		return &bmpArray[y*GetNumBlockX()+x];
	}
	return NULL;
}

YsBitmap *YsBitmapArray::GetTileBitmap(int x,int y)
{
	if(0<=x && x<GetNumBlockX() && 0<=y && y<GetNumBlockY())
	{
		return &bmpArray[y*GetNumBlockX()+x];
	}
	return NULL;
}
