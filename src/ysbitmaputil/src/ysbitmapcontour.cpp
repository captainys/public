/* ////////////////////////////////////////////////////////////

File Name: ysbitmapcontour.cpp
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

#include "ysbitmapcontour.h"

YsBitmapContourExtraction::YsBitmapContourExtraction(const YsBitmap &bmp)
{
	bmpPtr=&bmp;
}

YSRESULT YsBitmapContourExtraction::RemovePointConnection(YsBitmap &bmp)
{
	if(&bmp!=bmpPtr)
	{
		return YSERR;
	}

	for(int y=0; y<bmp.GetHeight()-1; ++y)
	{
		for(int x=0; x<bmp.GetWidth()-1; ++x)
		{
			const YSBOOL inout[2][2]=
			{
				{IsInside(YsVec2i(x,y)),    IsInside(YsVec2i(x+1,y))},
				{IsInside(YsVec2i(x,y+1)),  IsInside(YsVec2i(x+1,y+1))}
			};
			if(inout[0][0]!=inout[0][1] &&
			   inout[0][0]==inout[1][1] &&
			   inout[0][1]==inout[1][0])
			{
				auto from=bmp.GetRGBAPixelPointer(x,y);
				auto to=bmp.GetEditableRGBAPixelPointer(x,y+1);
				to[0]=from[0];
				to[1]=from[1];
				to[2]=from[2];
				to[3]=from[3];
			}
		}
	}
	return YSOK;
}

void YsBitmapContourExtraction::CleanUp(void)
{
	visited.CleanUpThin();
	visited.SetOrderSensitivity(YSTRUE);
	contourArray.CleanUp();
}

void YsBitmapContourExtraction::ExtractContour(void)
{
	CleanUp();

	for(int x=0; x<bmpPtr->GetWidth(); ++x)
	{
		for(int y=0; y<bmpPtr->GetHeight(); ++y)
		{
			const YSHASHKEY up[3]=
			{
				(YSHASHKEY)x,
				(YSHASHKEY)y,
				(YSHASHKEY)0
			};
			if(YSTRUE==WallIsBoundary(x,y,0) && YSTRUE!=visited.CheckKeyExist(3,up))
			{
				auto vtSeq=TrackBoundary(x,y,0);
				if(0<vtSeq.GetN())
				{
					contourArray.Increment();
					contourArray.Last().MoveFrom(vtSeq);
				}
			}
			const YSHASHKEY left[3]=
			{
				(YSHASHKEY)x,
				(YSHASHKEY)y,
				(YSHASHKEY)1
			};
			if(YSTRUE==WallIsBoundary(x,y,1) && YSTRUE!=visited.CheckKeyExist(3,left))
			{
				auto vtSeq=TrackBoundary(x,y,1);
				if(0<vtSeq.GetN())
				{
					contourArray.Increment();
					contourArray.Last().MoveFrom(vtSeq);
				}
			}
		}
	}
}

void YsBitmapContourExtraction::Resample(int NEdge)
{
	double totalLength=0.0;
	for(auto &contour : contourArray)
	{
		for(auto edIdx : contour.AllIndex())
		{
			totalLength+=(contour[edIdx]-contour.GetCyclic(edIdx+1)).GetLength();
		}
	}
	const double unitLength=totalLength/(double)NEdge;


	for(auto contourIdx : contourArray.ReverseIndex())
	{
		auto &contour=contourArray[contourIdx];

		double contourLength=0.0;
		for(auto edIdx : contour.AllIndex())
		{
			contourLength+=(contour[edIdx]-contour.GetCyclic(edIdx+1)).GetLength();
		}

		const int nSample=YsSmaller<int>((int)contour.GetN(),contourLength/unitLength);
		if(3>nSample)
		{
			contourArray.Delete(contourIdx);
			continue;
		}

		YsArray <YsVec2i> newContour;
		newContour.Resize(nSample);
		for(YSSIZE_T i=0; i<nSample; ++i)
		{
			newContour[i]=contour[contour.GetN()*i/nSample];
		}

		contour.MoveFrom(newContour);
	}
}

const YsArray <YsArray <YsVec2i> > YsBitmapContourExtraction::GetContour(void) const
{
	return contourArray;
}

YsArray <YsVec2i> YsBitmapContourExtraction::TrackBoundary(int x,int y,int s)
{
	YsArray <YsVec2i> contour;

	YsVec2i pos,vec;
	if(s==0)
	{
		// Always go counter-clockwise
		/*
          +-----U------+-->X
          |            |
          |            |
          L   (x,y-1)  |
          |            |
          |            |
          +------------+
          |            |
          |            |
          L   (x,y)    |
          |            |
          |            |
          +------------+
          |
          V Y

		*/

		if(YSTRUE==IsInside(YsVec2i(x,y)))
		{
			pos.Set(x+1,y);
			vec.Set(-1,0);
		}
		else // if(YSTRUE==IsInside(YsVec2i(x,y-1)))
		{
			pos.Set(x,y);
			vec.Set(1,0);
		}
	}
	else if(s==1)
	{
		// Always go counter-clockwise
		/*
          +-----U------+------------+-->X
          |            |            |
          |            |            |
          L   (x-1,y)  |   (x,y)    |
          |            |            |
          |            |            |
          +------------+------------+
          |
          V Y

		*/
		if(YSTRUE==IsInside(YsVec2i(x,y)))
		{
			pos.Set(x,y);
			vec.Set(0,1);
		}
		else // if(YSTRUE==IsInside(YsVec2i(x-1,y)))
		{
			pos.Set(x,y+1);
			vec.Set(0,-1);
		}
	}
	else
	{
		return contour;
	}

	YSHASHKEY wallHash[3]=
	{
		(YSHASHKEY)x,
		(YSHASHKEY)y,
		(YSHASHKEY)s
	};
	visited.Add(3,wallHash,0);

	auto prevPos=pos;
	contour.Add(pos);
	pos+=vec;
	contour.Add(pos);
	for(;;)
	{
		auto curVec=pos-prevPos;

		int nNextPos=0;
		YsVec2i nextPos[4];
		Wall nextWall[4];

		auto x=pos.x();
		auto y=pos.y();
		if(YsVec2i(x,y-1)!=prevPos && YSTRUE==WallIsBoundary(x,y-1,1))
		{
			nextWall[nNextPos].Set(x,y-1,1);
			nextPos[nNextPos++].Set(x,y-1);
		}
		if(YsVec2i(x,y+1)!=prevPos && YSTRUE==WallIsBoundary(x,y,1))
		{
			nextWall[nNextPos].Set(x,y,1);
			nextPos[nNextPos++].Set(x,y+1);
		}
		if(YsVec2i(x-1,y)!=prevPos && YSTRUE==WallIsBoundary(x-1,y,0))
		{
			nextWall[nNextPos].Set(x-1,y,0);
			nextPos[nNextPos++].Set(x-1,y);
		}
		if(YsVec2i(x+1,y)!=prevPos && YSTRUE==WallIsBoundary(x,y,0))
		{
			nextWall[nNextPos].Set(x,y,0);
			nextPos[nNextPos++].Set(x+1,y);
		}

		int nextPosIdx=-1;
		if(0==nNextPos)
		{
			break;
		}
		else if(1==nNextPos)
		{
			nextPosIdx=0;
		}
		else if(1<nNextPos)
		{
			// Take the left-most turn.
			/*
	          +-----U------+------------+-->X
	          |            |            |
	          |            |            |
	          L            |            |
	          |            |            |
	          |            |            |
	          +------------+------------+
	          |            |pos         |
	          |            |            |
	          L            |            |
	          |            |            |
	          |            |            |
	          +------------+------------+
	          |
	          V Y
	             
	          ^  (0,-1)   1
	          |
	          |
	          
	          <---  (-1,0)
	          
	          
	          |
	          |  (0,1)    -1
	          V
			*/

			int minCrsProd=2;
			for(int i=0; i<nNextPos; ++i)
			{
				auto crsProd=curVec^(nextPos[i]-pos);

				if(0>nextPosIdx || crsProd<minCrsProd)
				{
					nextPosIdx=i;
					minCrsProd=crsProd;
				}
			}
		}

		if(0>nextPosIdx)
		{
			break;
		}

		YSHASHKEY wallHash[3]=
		{
			(YSHASHKEY)nextWall[nextPosIdx].xy.x(),
			(YSHASHKEY)nextWall[nextPosIdx].xy.y(),
			(YSHASHKEY)nextWall[nextPosIdx].s
		};
		if(YSTRUE==visited.CheckKeyExist(3,wallHash))
		{
			break;
		}

		visited.Add(3,wallHash,0);
		prevPos=pos;
		pos=nextPos[nextPosIdx];
		if(contour[0]==pos)
		{
			break;
		}
		contour.Add(pos);
	}
	return contour;
}


YSBOOL YsBitmapContourExtraction::WallIsBoundary(YsVec2i pos,int s) const
{
	return WallIsBoundary(pos.x(),pos.y(),s);
}
YSBOOL YsBitmapContourExtraction::WallIsBoundary(int x,int y,int s) const
{
	if(0==s)
	{
		if(IsInside(YsVec2i(x,y))!=IsInside(YsVec2i(x,y-1)))
		{
			return YSTRUE;
		}
	}
	else if(1==s)
	{
		if(IsInside(YsVec2i(x,y))!=IsInside(YsVec2i(x-1,y)))
		{
			return YSTRUE;
		}
	}
	return YSFALSE;
}

YSBOOL YsBitmapContourExtraction::IsInside(YsVec2i pos) const
{
	auto &bmp=*bmpPtr;
	if(0<=pos.x() && pos.x()<bmp.GetWidth() && 0<=pos.y() && pos.y()<bmp.GetHeight())
	{
		auto pix=bmp.GetRGBAPixelPointer(pos.x(),pos.y());
		return IsInside(pix);
	}
	return YSFALSE;
}
/* virtual */ YSBOOL YsBitmapContourExtraction::IsInside(const unsigned char rgba[4]) const
{
	if(128>rgba[0] && 128>rgba[1] && 128>rgba[2])
	{
		return YSTRUE;
	}
	return YSFALSE;
}
