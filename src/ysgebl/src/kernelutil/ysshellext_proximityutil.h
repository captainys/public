/* ////////////////////////////////////////////////////////////

File Name: ysshellext_proximityutil.h
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

#ifndef YSSHELLEXT_PROXIMITYUTIL_IS_INCLUDED
#define YSSHELLEXT_PROXIMITYUTIL_IS_INCLUDED
/* { */

#include <ysshellext.h>

class YsShellExt_EdgeVertexProximityUtil
{
public:
	class EdgeVertexPair
	{
	public:
		YsShellVertexHandle edVtHd[2];
		YsShellVertexHandle vtHd;
		int nSharingEdge;  // The number of vertices (including this vertex) using this edge as the nearest edge.
	};

	typedef YsBinaryTree2 <double,EdgeVertexPair>::NodeHandle EDVTPAIR_HANDLE;

private:
	const YsShellExt *shlPtr;
	YsAVLTree <double,EdgeVertexPair> edVtPairTree;
	YsShellEdgeMultiAttribTable <EDVTPAIR_HANDLE> edgeToEdVtPairHandle;
	YsShellVertexAttribTable <EDVTPAIR_HANDLE> vertexToEdVtPairHandle;

public:
	YsShellExt_EdgeVertexProximityUtil();
	void CleanUp(void);
	void SetShell(const YsShellExt &shl);
	EDVTPAIR_HANDLE AddEdgeVertexPair(const YsShellVertexHandle edVtHd[2],YsShellVertexHandle vtHd,const double dist);
	EDVTPAIR_HANDLE GetNearestEdgeVertexPairHandle(void) const;
	void DeleteEdgeVertexPair(EDVTPAIR_HANDLE hd);

	/*! Delete all pairs that the edge matches edVtHd[0]-edVtHd[1].
	*/
	void DeleteEdge(const YsShell::VertexHandle edVtHd[2]);

	void MoveToNextEdgeVertexPair(EDVTPAIR_HANDLE &hd) const;
	EdgeVertexPair GetEdgeVertexPair(EDVTPAIR_HANDLE hd) const;

	YsArray <EDVTPAIR_HANDLE> FindEdgeVertexPairFromEdgePiece(const YsShellVertexHandle edVtHd[2]) const;
	YsArray <EDVTPAIR_HANDLE> FindEdgeVertexPairFromEdgePiece(YsShellVertexHandle edVtHd0,YsShellVertexHandle edVtHd1) const;
	EDVTPAIR_HANDLE FindEdgeVertexPairFromVertex(YsShellVertexHandle vtHd) const;

	YSSIZE_T GetEdgeIsClosestOfHowManyVertex(const YsShellVertexHandle edVtHd[2]) const;
	YSSIZE_T GetEdgeIsClosestOfHowManyVertex(YsShellVertexHandle edVtHd0,YsShellVertexHandle edVtHd1) const;

	/*! Returns the vertices whose nearest edge is edVtHd[0]-edVtHd[1]. 
	    Vertices are sorted ascending order of the distance from edVtHd[0].
	*/
	YsArray <YsShellVertexHandle> FindNearestVertexFromEdgePiece(const YsShellVertexHandle edVtHd[2]) const;

	/*! Returns the vertices whose nearest edge is edVtHd0-edVtHd1. 
	    Vertices are sorted ascending order of the distance from edVtHd0.
	*/
	YsArray <YsShellVertexHandle> FindNearestVertexFromEdgePiece(YsShellVertexHandle edVtHd0,YsShellVertexHandle edVtHd1) const;

	/*! Finds an edge piece registered in the lattice (ltc) from the given vertex (vtHd).
	    It ignores the edge pieces that shares a same face grop from the vertex.
	    The result edge piece is returned in edVtHdMin[], and dMin will be the distance.
	    If an edge piece is found, this function returns YSOK.
	    If no such edge piece is found, this function returns YSERR. */
	YSRESULT FindNearestEdgeFromVertexNotSharingFaceGroup(YsShellVertexHandle edVtHdMin[2],double &dMin,const YsShellExt &shl,const YsShellLattice &ltc,YsShellVertexHandle vtHd) const;
};



/*! Returns the handle to the polygon that is nearest to FROM.  All polygons that COND returns YSERR will be ignored.
    Use YsShellExt::PassAll if all polygons need to be checked.

    The distance to the nearest polygon and the nearest point will be returned in DIST and NEARPOS.

    This function exhaustively search for the nearest point.  Therefore, it is inappropriate for using many vertices.  
    It is good when proximity check is needed for small number of points.

    When it is necessary for checking proximity for large number of points, use YsShellLattice instead.
*/
YsShell::PolygonHandle YsShellExt_FindNearestPolygon(
    double &DIST,YsVec3 &NEARPOS,const YsShellExt &shl,const YsVec3 &FROM,const YsShellExt::Condition &COND);



/* } */
#endif
