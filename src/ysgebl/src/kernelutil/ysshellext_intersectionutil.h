/* ////////////////////////////////////////////////////////////

File Name: ysshellext_intersectionutil.h
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

#ifndef YSSHELLEXT_INTERSECTIONUTIL_IS_INCLUDED
#define YSSHELLEXT_INTERSECTIONUTIL_IS_INCLUDED
/* { */

#include <ysshellext.h>
#include "ysshellext_localop.h"

class YsShellExt_IntersectionUtil
{
protected:
	const YsShellExt *shl;
	YsShellPolygonStore itscPlg;
	YsShellVertexStore itscVtx;
	class EdgeIntersection
	{
	public:
		YsShellVertexHandle edVtHd[2];
		YsVec3 pos;
		YsShellVertexHandle newVtHd;
	};
	YsArray <EdgeIntersection> edgeItscArray;

	/*! Clean up. */
	void CleanUp(void);

	/*! This function returns points where the shell intersects with the cutting plane.  Can be used for display purpose. */
	YsArray <YsVec3> GetIntersectingPoint(void) const;

	/*! This function returns points of the vertices lying ont the cutting plane.  Can be used for display purpose. */
	YsArray <YsVec3> GetOnPlaneVertex(void) const;

	/*! Returns all EdgeIntersection's */
	YsArray <EdgeIntersection> &AllEdgeIntersection(void);
};

class YsShellExt_CutByPlaneUtil : private YsShellExt_IntersectionUtil
{
public:
	using YsShellExt_IntersectionUtil::GetIntersectingPoint;
	using YsShellExt_IntersectionUtil::GetOnPlaneVertex;
	using YsShellExt_IntersectionUtil::AllEdgeIntersection;

	YsSegmentedArray <YsShell_LocalOperation::PolygonSplit,4> polygonSplitArray;
	YsSegmentedArray <YsShell_LocalOperation::ConstEdgeSplit,4> constEdgeSplitArray;
	YsSegmentedArray <YsShell_LocalOperation::FaceGroupSplit,4> faceGroupSplitArray;

private:
	YsPlane cutPln;
	YsShellEdgeStore onPlaneEdge;
	YsArray <YsVec3,4> cutPlg;

	YsMatrix4x4 prjMat;
	YsArray <YsVec2,4> cutPlgPrj;

	void MakeOnPlaneVertex(const YsPlane &cutPln);
	void MakeEdgePlaneIntersection(const YsPlane &cutPln);

	/*! This function uses itscVtx.  Therefore must be called after MakeOnPlaneVertex. */
	void MakeOnPlaneEdge(void);

public:
	/*! Clean up. */
	void CleanUp(void);

	/*! Returns the cutting plane set by SetCutByPlane or SetCutByPolygon function. */
	const YsPlane GetCuttingPlane(void) const;

	/*! This function sets up cutting by a plane. */
	YSRESULT SetCutByPlane(const YsShellExt &shl,const YsPlane &pln);

	/*! This function sets up cutting by a polygon defined by the given vertices. */
	YSRESULT SetCutByPolygon(const YsShellExt &shl,YsShellPolygonHandle plHd);
	YSRESULT SetCutByPolygon(const YsShellExt &shl,YSSIZE_T nVtx,const YsShellVertexHandle plVtHd[]);
	template <const int N>
	YSRESULT SetCutByPolygon(const YsShellExt &shl,const YsArray <YsShellVertexHandle,N> &plVtHd);

	/*! After populating edgeItscArray, this function makes polygonSplitArray and constEdgeSplitArray. 
	    Face-group split info can be created after polygons and const-edges are split. */
	void MakeSplitInfo(void);

	/*! Make polygons and const-edges to drop.  dropWhichSide should be -1 or 1. */
	YSRESULT MakeToDrop(YsShellPolygonStore &plgToDrop,YsShellExt::ConstEdgeStore &ceToDrop,int dropWhichSide);

	/*! Makes face group split info. */
	void MakeFaceGroupSplitInfo(const YsShellPolygonStore &plgToDrop);
};

template <const int N>
YSRESULT YsShellExt_CutByPlaneUtil::SetCutByPolygon(const YsShellExt &shl,const YsArray <YsShellVertexHandle,N> &plVtHd)
{
	return SetCutByPolygon(shl,plVtHd.GetN(),plVtHd);
}

/* } */
#endif
