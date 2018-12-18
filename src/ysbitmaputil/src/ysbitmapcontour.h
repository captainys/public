/* ////////////////////////////////////////////////////////////

File Name: ysbitmapcontour.h
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

#ifndef YSBITMAPCONTOUR_IS_INCLUDED
#define YSBITMAPCONTOUR_IS_INCLUDED
/* { */

#include <ysclass.h>
#include <ysbitmap.h>


/*

  Vertx(x,y)
      |
      V
      +-----U------+-->X
      |            |
      |   Block    |
      L   (x,y)    |
      |            |
      |            |
      +------------+
      |
      V
       Y

  U: s=0
  L: s=1

  Edge (x,y,s)

  (x,y,0) connects vertex (x,y)-(x+1,y)
  (x,y,1) connects vertex (x,y)-(x,y+1)

*/



class YsBitmapContourExtraction
{
private:
	const YsBitmap *bmpPtr;
	YsFixedLengthHashTable <int,3> visited;
	YsArray <YsArray <YsVec2i> > contourArray;

public:
	class Wall
	{
	public:
		YsVec2i xy;
		int s;

		void Set(int x,int y,int s)
		{
			this->xy.Set(x,y);
			this->s=s;
		}
	};

	YsBitmapContourExtraction(const YsBitmap &bmp);

	/*! Bmp must be the same bitmap passed to the constructor. */
	YSRESULT RemovePointConnection(YsBitmap &Bmp);

	void CleanUp(void);
	void ExtractContour(void);


	/*! This function re-samples contours so that total number of edges will be close to NEdge. */
	void Resample(int NEdge);

	const YsArray <YsArray <YsVec2i> > GetContour(void) const;

	YsArray <YsVec2i> TrackBoundary(int x,int y,int s);

	YSBOOL WallIsBoundary(YsVec2i pos,int s) const;
	YSBOOL WallIsBoundary(int x,int y,int s) const;

	YSBOOL IsInside(YsVec2i pos) const;
	virtual YSBOOL IsInside(const unsigned char rgba[4]) const;

	static inline void RotateLeft(YsVec2i &vec)
	{
		if(0<vec.y())
		{
			vec.SubX(1);
			if(vec.x()<-1)
			{
				vec.SetX(-1);
				vec.SetY(0);
			}
		}
		else if(vec.x()<0)
		{
			vec.SubY(1);
			if(vec.y()<-1)
			{
				vec.SetX(0);
				vec.SetY(-1);
			}
		}
		else if(vec.y()<0)
		{
			vec.AddX(1);
			if(1<vec.x())
			{
				vec.SetX(1);
				vec.SetY(0);
			}
		}
		else
		{
			vec.AddY(1);
			if(1<vec.y())
			{
				vec.SetX(0);
				vec.SetY(1);
			}
		}
	}
	static inline void RotateRight(YsVec2i &vec)
	{
		if(0<vec.y())
		{
			vec.AddX(1);
			if(1<vec.x())
			{
				vec.SetX(1);
				vec.SetY(0);
			}
		}
		else if(0<vec.x())
		{
			vec.SubY(1);
			if(vec.y()<-1)
			{
				vec.SetX(0);
				vec.SetY(-1);
			}
		}
		else if(vec.y()<0)
		{
			vec.SubX(1);
			if(vec.x()<-1)
			{
				vec.SetX(-1);
				vec.SetY(0);
			}
		}
		else
		{
			vec.AddY(1);
			if(1<vec.y())
			{
				vec.SetX(0);
				vec.SetY(1);
			}
		}
	}
};

/* } */
#endif
