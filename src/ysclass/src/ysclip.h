/* ////////////////////////////////////////////////////////////

File Name: ysclip.h
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

#ifndef YSCLIP_IS_INCLUDED
#define YSCLIP_IS_INCLUDED
/* { */

#include <stdio.h>
#include "ysgeometry.h"
#include "ysarray.h"

YSRESULT YsClipLineSeg2(YsVec2 &c1,YsVec2 &c2,const YsVec2 &l1,const YsVec2 &l2,const YsVec2 &range1,const YsVec2 &range2);

YSRESULT YsClipInfiniteLine2
    (YsVec2 &c1,YsVec2 &c2,
     const YsVec2 &org,const YsVec2 &vec,const YsVec2 &range1,const YsVec2 &range2);

YSRESULT YsClipInfiniteLine3
    (YsVec3 &c1,YsVec3 &c2,
     const YsVec3 &org,const YsVec3 &vec,const YsVec3 &range1,const YsVec3 &range2);

YSRESULT YsClipPolygon3
   (int &newNp,YsVec3 newp[],int bufLen,int np,const YsVec3 p[],const YsVec3 &range1,const YsVec3 &range2);

/*! Clips a polygon defined by p[0]...p[np-1] by the given plane.  The last parameter whichSideToLive needs to be zero or one. */
YSRESULT YsClipPolygonByPlane(int &newNp,YsVec3 newp[],int bufLen,int np,const YsVec3 p[],const YsPlane &pln,int whichSideToLive);

YSRESULT YsClipLineSeg3
   (YsVec3 &newP1,YsVec3 &newP2,
    const YsVec3 &p1,const YsVec3 &p2,const YsVec3 &range1,const YsVec3 &range2);


/*! This function calculates the point on the line defined by two points p1 and p2 
    at Z=clipz and returns the value in clipPos.  */
YSRESULT YsZClip(YsVec3 &clipPos,const YsVec3 &p1,const YsVec3 &p2,const double &clipz);

/*! This function clips a line p1-p2 with the near plane z=nearz, and returns the 
    result in newP1 and newP2.  
    
    This function returns YSOK if some part of the line is in front of the near plane,
    YSERR if the two points are both behind the near plane.

    If positivez==YSTRUE, near plane is facing positive Z direction, negative Z 
    direction if it is YSFALSE.  For simulating OpenGL's clipping, this value must be YSFALSE.

	If successful, the normalized direction newP1 to newP2 will stay same as
	p1 to p2.
    */
YSRESULT YsClipLineWithNearZ(YsVec3 &newP1,YsVec3 &newP2,const YsVec3 &p1,const YsVec3 &p2,const double &nearz,YSBOOL positivez=YSFALSE);

/*! This function clips a polygon consisting of p[0] to p[np-1] with a clip plane Z=nearz
    and returns the result in newPlg.

	This function returns YSOK if some part of the polygon is in front of the near plane,
	YSERR otherwise.

    If positivez==YSTRUE, near plane is facing positive Z direction, negative Z 
    direction if it is YSFALSE.  For simulating OpenGL's clipping, this value must be YSFALSE.
    */
template <const int N>
YSRESULT YsClipPolygonWithNearZ(
    YsArray <YsVec3,N> &newPlg,const int np,const YsVec3 p[],const double &nearz,YSBOOL positivez=YSFALSE)
{
	if(newPlg.GetArray()==p)
	{
		YsArray <YsVec3,N> tmp;
		const YSRESULT res=YsClipPolygonWithNearZ(tmp,np,p,nearz,positivez);
		newPlg=tmp;
		return res;
	}

	class TestIsFrontSide
	{
	public:
		inline YSBOOL IsFrontSide(const double tstz,const double nearz,YSBOOL positivez)
		{
			if(YSTRUE==positivez && nearz<=tstz)
			{
				return YSTRUE;
			}
			else if(YSTRUE!=positivez && tstz<=nearz)
			{
				return YSTRUE;
			}
			return YSFALSE;
		}
	};

	TestIsFrontSide tester;

	newPlg.Clear();
	for(int i=0; i<np; ++i)
	{
		const YsVec3 &pa=p[i];
		const YsVec3 &pb=p[(i+1)%np];

		const YSBOOL aIsFront=tester.IsFrontSide(pa.z(),nearz,positivez);
		const YSBOOL bIsFront=tester.IsFrontSide(pb.z(),nearz,positivez);

		if(YSTRUE==aIsFront)
		{
			newPlg.Append(pa);
		}

		if(aIsFront!=bIsFront) // Coming in or going out.
		{
			YsVec3 clipPos;
			if(YSOK==YsZClip(clipPos,pa,pb,nearz))
			{
				newPlg.Append(clipPos);
			}
		}
	}

	if(3<=newPlg.GetN())
	{
		return YSOK;
	}
	return YSERR;
}

/* } */
#endif
