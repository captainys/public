/* ////////////////////////////////////////////////////////////

File Name: ysshellext_constedgeutil.h
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

#ifndef YSSHELLEXT_CONSTEDGEUTIL_IS_INCLUDED
#define YSSHELLEXT_CONSTEDGEUTIL_IS_INCLUDED
/* { */

#include <ysshellext.h>
#include <ysshellextmisc.h>

YsShellExt::ConstEdgeAndPos YsShellExt_FindNearestPointOnConstEdgeInPolygon(
    const YsShellExt &shl,YsShellPolygonHandle plHd,const YsVec3 &from);


/*!  This function adds const edges on the non-manifold edges, 
     and returns a YsShellVertexStore that includes all vertices used in the new const edges.
*/
template <class SHLCLASS>
YsShellVertexStore YsShellExt_AddConstEdgeAlongNonManifoldEdge(SHLCLASS &shl)
{
	YsShellVertexStore usedVtx(shl.Conv());

	YsShellEdgeEnumHandle edHd=NULL;
	while(YSOK==shl.MoveToNextEdge(edHd))
	{
		YsShellVertexHandle edVtHd[2];
		shl.GetEdge(edVtHd,edHd);

		if(0==shl.GetNumConstEdgeUsingEdgePiece(edVtHd))
		{
			if(2!=shl.GetNumPolygonUsingEdge(edVtHd))
			{
				shl.AddConstEdge(2,edVtHd,YSFALSE);
				usedVtx.AddVertex(edVtHd[0]);
				usedVtx.AddVertex(edVtHd[1]);
			}
		}
	}
	return usedVtx;
}

/*!  This function adds const edges on the color boundary, 
     and returns a YsShellVertexStore that includes all vertices used in the new const edges.
*/
template <class SHLCLASS>
YsShellVertexStore YsShellExt_AddConstEdgeAlongColorBoundary(SHLCLASS &shl)
{
	YsShellVertexStore usedVtx(shl.Conv());

	YsShellEdgeEnumHandle edHd=NULL;
	while(YSOK==shl.MoveToNextEdge(edHd))
	{
		auto edPlHd=shl.FindPolygonFromEdgePiece(edHd);
		if(1<edPlHd.size())
		{
			YSBOOL diff=YSFALSE;
			auto col0=shl.GetColor(edPlHd[0]);
			for(auto plHd : edPlHd.Subset(1))
			{
				if(shl.GetColor(plHd)!=col0)
				{
					diff=YSTRUE;
					break;
				}
			}
			if(YSTRUE==diff)
			{
				YsShellVertexHandle edVtHd[2];
				shl.GetEdge(edVtHd,edHd);

				if(0==shl.GetNumConstEdgeUsingEdgePiece(edVtHd))
				{
					shl.AddConstEdge(2,edVtHd,YSFALSE);
					usedVtx.AddVertex(edVtHd[0]);
					usedVtx.AddVertex(edVtHd[1]);
				}
			}
		}
	}
	return usedVtx;
}

/*! This function returns the closest position on the const edge.
    If the closest position is at a vertex, edVtHd[0] and edVtHd[1] is one of the const-edge
    segment that includes the nearest vertex.
*/
YsShell::EdgeAndPos YsShellExt_FindNearestPointOnConstEdge(
    const YsShellExt &shl,YsShellExt::ConstEdgeHandle ceHd,const YsVec3 &from);



/*! This class provides a functionality for finding proximity const-edges starting from a vertex 
    or an edge.
    It is also possible to restart search from an edge without visiting already-visited
    const-edges and polygons.
*/
class YsShellExt_ProximityConstEdgeFinder
{
private:
	const YsShellExt *shlPtr;
	YSBOOL stopAtConstEdge;
	YsShellExt::ConstEdgeStore visitedCe;
	YsShellExt::ConstEdgeStore foundCe;
	YsShellPolygonStore visitedPlg;
public:
	YsShellExt_ProximityConstEdgeFinder();
	void CleanUp(void);
	void SetShell(const YsShellExt &shl);

	/*! If it is set to YSTRUE, the search continues across a const edge.  
	    If YSFALSE, the search will not cross a const edge.
	    Default is YSTRUE.
	*/
	void SetStopAtConstEdge(YSBOOL sw);

	/*! Start a fresh search from a vertex or an edge.
	*/
	const YsShellExt::ConstEdgeStore &Search(YsShell::VertexHandle fromVtHd,const double radius);
	const YsShellExt::ConstEdgeStore &Search(const YsShell::Edge &fromEdge,const YsVec3 &startPos,const double radius);

	/*! Resume a search from a vertex or an edge.  It will not traverse through an already-visited polygon,
	    and will not re-find an already-found const-edge.

	    If you call this function before running any search, the result is same as just calling Search.
	*/
	const YsShellExt::ConstEdgeStore &ResumeSearch(YsShell::VertexHandle fromVtHd,const double radius);
	const YsShellExt::ConstEdgeStore &ResumeSearch(const YsShell::Edge &fromEdge,const YsVec3 &startPos,const double radius);
private:
	void Search(YsArray <YsShell::PolygonHandle> &seed,const YsVec3 &startPos,const double radius);
	void TransferFoundConstEdgeToVisitedConstEdge(void);
};

/* } */
#endif
