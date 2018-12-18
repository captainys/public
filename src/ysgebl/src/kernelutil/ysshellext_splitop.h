/* ////////////////////////////////////////////////////////////

File Name: ysshellext_splitop.h
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

#ifndef YSSHELLEXT_SPLITOP_IS_INCLUDED
#define YSSHELLEXT_SPLITOP_IS_INCLUDED
/* { */

#include "ysshellext_localop.h"

class YsShell_SplitInfo : public YsShell_LocalOperation
{
public:
	YsSegmentedArray <EdgeSplit,4> edgeSplit;
	YsSegmentedArray <PolygonSplit,4> newPlg;
	YsSegmentedArray <ConstEdgeSplit,4> newConstEdge;

	YsShell_SplitInfo();
	void CleanUp(void);

	/*! This function sets up plg from a polygon, plHd, and split according to cutPoint, which is a set of vertices of the cutting point. 
	    This function checks geometry so that the polygon is correctly cut.
	    This function may fail and returns YSERR if the projection of the polygon cannot be calculated, or returns YSOK if no error. */
	YSRESULT SetAndSplitPolygon(PolygonSplit &plg,const YsShellExt &shl,const YsShellVertexStore &cutPoint,YsShellPolygonHandle plHd);

	/*! This function splita a polygon in plg based on cutPoint, which is a set of vertices of the cutting point. */
	void SplitPolygon(PolygonSplit &plg,const YsShellVertexStore &cutPoint);

	/*! This function sets up ce from a const edge, ceHd, and split according to cutPoint, which is a set of vertices of the cutting point. */
	YSRESULT SetAndSplitConstEdge(ConstEdgeSplit &ce,const YsShellExt &shl,const YsShellVertexStore &cutPoint,YsShellExt::ConstEdgeHandle ceHd);

	/*! This function splits a const edge in ce based on cutPoint.
	    If more than two vertex in a row of the const edge are included in cutPoint, middle segments will not be divided. */
	void SplitConstEdge(ConstEdgeSplit &ce,const YsShellVertexStore &cutPoint);

	/*! This function splits a face group into sub-groups that overlaps with newGrouping. */
	YSRESULT SplitFaceGroup(FaceGroupSplit &fgSplit,const YsShellExt &shl,YsShellExt::FaceGroupHandle fgHd,const YsShellPolygonStore &newGrouping);
};

class YsShell_CutByPlane : public YsShell_SplitInfo
{
private:
	YsPlane pln;
	YsArray <YsVec3,16> plg;
	YsMatrix4x4 prjMat;
	YsArray <YsVec2,16> prjPlg;
	YsArray <YsShellPolygonHandle> plHdToSplit;
	YsArray <YsShellExt::ConstEdgeHandle> ceHdToSplit;

public:
	/*! Cleans up the class. */
	void CleanUp(void);

private:
	void SetPlane(const YsPlane &pln);
	YSRESULT SetPolygon(YSSIZE_T np,const YsVec3 p[]);
	YSBOOL CheckNeedSplit(const YsShellExt &shl,YSSIZE_T np,const YsShellVertexHandle vtHdArray[]);
	void FindPolygonToSplit(const YsShellExt &shl);
	void FindConstEdgeToSplit(const YsShellExt &shl);
	void MakeEdgeSplitInfo(const YsShellExt &shl);
	void MakeEdgeSplitInfo_PerEdge(const YsShellExt &shl,YsShellEdgeStore &visited,const YsShellVertexHandle edVtHd[2]);

public:
	/*! StartByPolygon starts the cutting process by a polygon.
	    Edges that do not intersect with the polygon will not be split. 
	    StartByPolygon populates pln, plHdToSplit, ceHdToSplit, and edgeSplit.

	    Member createdVtHd of edgeSplit[?] needs to be populated in the next step outside the class.  
	    edgeSplit[?].createdVtHd must be merged into polygons and const edges in this step.
	     */
	YSRESULT StartByPolygon(const YsShellExt &shl,YSSIZE_T np,const YsVec3 p[]);

	template <const int N>
	YSRESULT StartByPolygon(const YsShellExt &shl,const YsArray <YsVec3,N> &p);

	YSRESULT StartByPolygon(const YsShellExt &shl,YSSIZE_T np,const YsShellVertexHandle p[]);

	template <const int N>
	YSRESULT StartByPolygon(const YsShellExt &shl,const YsArray <YsShellVertexHandle,N> &p);


	/*! After edgeSplit[?].createdVtHd are populated, this function will make new-polygon and 
	    new-constedge information.  edgeSplit[?].createdVtHd must be used by polygons and const-edges 
	    before using this function. */
	YSRESULT MakeSplitInfoAfterEdgeVertexGeneration(const YsShellExt &shl);
};

template <const int N>
YSRESULT YsShell_CutByPlane::StartByPolygon(const YsShellExt &shl,const YsArray <YsVec3,N> &p)
{
	return StartByPolygon(shl,p.GetN(),p);
}

template <const int N>
YSRESULT YsShell_CutByPlane::StartByPolygon(const YsShellExt &shl,const YsArray <YsShellVertexHandle,N> &p)
{
	return StartByPolygon(shl,p.GetN(),p);
}


/* } */
#endif
