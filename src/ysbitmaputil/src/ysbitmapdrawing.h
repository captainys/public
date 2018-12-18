/* ////////////////////////////////////////////////////////////

File Name: ysbitmapdrawing.h
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

#ifndef YSBITMAPDRAWING_IS_INCLUDED
#define YSBITMAPDRAWING_IS_INCLUDED
/* { */

#include <ysclass.h>
#include <ysbitmap.h>

class YsBitmapDrawing
{
public:
	class Option
	{
	public:
		unsigned char rgba[4];
		Option()
		{
			rgba[0]=0;
			rgba[1]=0;
			rgba[2]=0;
			rgba[3]=255;
		}
		void SetColor(const unsigned char rgba[4])
		{
			this->rgba[0]=rgba[0];
			this->rgba[1]=rgba[1];
			this->rgba[2]=rgba[2];
			this->rgba[3]=rgba[3];
		}
		void SetColor(YsColor col)
		{
			this->rgba[0]=(unsigned char)col.Ri();
			this->rgba[1]=(unsigned char)col.Gi();
			this->rgba[2]=(unsigned char)col.Bi();
			this->rgba[3]=(unsigned char)col.Ai();
		}
	};
	class LineOption : public Option
	{
	public:
		unsigned int thickness;
		LineOption()
		{
			thickness=1;
		}
	};

private:
	YsBitmap *bmpPtr;

	int minPlgItscY,maxPlgItscY;
	YsArray <int> plgItscBuf;

public:
	YsBitmapDrawing(YsBitmap &bmp);

	void Clear(const unsigned char rgba[]);

	void DrawLine(int x0,int y0,int x1,int y1,const LineOption &option);
	void DrawLine(int x0,int y0,int x1,int y1,const unsigned char rgba[]);
	void DrawLineNoClip(int x0,int y0,int x1,int y1,const unsigned char rgba[]);

	void DrawPolygon(YSSIZE_T n,const int xy[],const unsigned char rgba[]);

	void DrawCircle(int x,int y,int r,const unsigned char rgba[],YSBOOL fill,int nDiv=64);

	void FillPolygonLineIntersection(const unsigned char rgba[4]);

	/*! This function returns last VY of the polygon, taking bitmap height into account.
	    If the polygon is completely out of the bitmap's Y range, this function returns 0.
	*/
	int GetPolygonLastVy(YSSIZE_T n,const int xy[]) const;

private:
	void CleanUpPolygonLineIntersection(void);
	void AddPolygonLineIntersectionLine(int x0,int y0,int x1,int y1,int &prevVy);
	void ErasePolygonLineIntersectionForOneY(int y);
	void MakePolygonLineIntersectionBufferForXSymmetric(int x);

public:
	inline void SetPixel(int x,int y,const unsigned char rgba[])
	{
		bmpPtr->SetRGBA(x,y,rgba[0],rgba[1],rgba[2],rgba[3]);
	}

private:
	inline void AddPolygonIntersection(int x,int y)
	{
		YsMakeSmaller(x,bmpPtr->GetWidth()-1);
		YsMakeGreater(x,0);

		YsMakeSmaller(minPlgItscY,y);
		YsMakeGreater(maxPlgItscY,y);

		if(plgItscBuf[y*2]<0)
		{
			plgItscBuf[y*2]=x;
		}
		else
		{
			if(x<plgItscBuf[y*2])
			{
				plgItscBuf[y*2+1]=plgItscBuf[y*2];
				plgItscBuf[y*2]  =x;
			}
			else
			{
				plgItscBuf[y*2+1]=x;
			}
		}
	}
};


/* } */
#endif
