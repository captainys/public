/* ////////////////////////////////////////////////////////////

File Name: ysdelaunaytri.h
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

#ifndef YSDELAUNAYTRI_IS_INCLUDED
#define YSDELAUNAYTRI_IS_INCLUDED
/* { */

#include "ysdef.h"
#include "ysgeometry.h"
#include "ysadvgeometry.h"

/*!  Triangle node and edge convention.  Edge[i] connects Node[i] and node[(i+1)%3].

   nd2  
      +
      |\
      | \
      |  \
   ed2|   \ed1
      |    \
      |     \
      +------+
   nd0  ed0  nd1
*/
class YsUnconstrainedDelaunayTriangulation
{
public:
	class Edge
	{
	public:
		int ndIdx[2];
	};
	class Triangle
	{
	public:
		int ndIdx[3];
		int neiTriIdx[3];

		YSBOOL alive;
		int testedCounter;  // If testedCounter<trueFalseThreshold, it hasn't been tested for Delaunay condition per node insertion.
		int cavityCounter;  // If cavityCounter<trueFalseThreshold, it is not in the cavity for node insertion.
		mutable int visitedCounter; // If visitedCounter<trueFalseThreshold, it is not visited during the enclosing-tri search.

		Edge GetEdge(int edIdx) const;
	};
	class Node
	{
	public:
		YsVec2 pos;
		YSBOOL isInitialNode;
		int intAttrib;

		int usingTriIdx; // Used during the node insertion.  Don't use for other purposes.
	};
	class EdgeAndTriangleIndex : public Edge
	{
	public:
		int triIdx;
	};

private:
	YsArray <Node> node;
	YsArray <Triangle> tri;
	YsArray <YSSIZE_T> unusedTriIdx;
	int lastAddedTriIndex;
	mutable int trueFalseThreshold;

	YsArray <YSSIZE_T> boundaryEdge;

private:
	int CreateTri(void);
	void FalsifyAll(void) const;
	YSBOOL HasBeenDelaunayTested(const Triangle &tri) const;
	YSBOOL IsIncludedInCavity(const Triangle &tri) const;
	YSBOOL HasBeenVisitedForEnclosingTriSearch(const Triangle &tri) const;
	void MarkDelaunayTested(Triangle &tri) const;
	void MarkIncludedInCavity(Triangle &tri) const;
	void MarkVisitedForEnclosingTriSearch(const Triangle &tri) const;

	void DeleteTri(YSSIZE_T triIdx); // This function won't update connections.
	void ConnectTri(int triIdx0,int triidx1);

	void DeleteTriAndClearConnection(YSSIZE_T  triIdx); // This function deletes a triangles and also clears connections.
	void DisconnectTri(YSSIZE_T triIdx);
	void DisconnectTri(Triangle &from,YSSIZE_T triIdxToCut);

public:
	YsUnconstrainedDelaunayTriangulation();
	~YsUnconstrainedDelaunayTriangulation();

	void CleanUp(void);

	int AddInitialNode(const YsVec2 &pos,int intAttrib=0);

	/*! AddInitialTri adds an initial triangle.  The assumption is that the initial triangle count is small.
	    As of 2015/01/30 version, it exaustively search for neighboring triangles for re-connection. */
	int AddInitialTri(const int ndIdx[3]);

	/*! AddInitialTri adds an initial triangle.  The assumption is that the initial triangle count is small. */
	int AddInitialTri(int ndIdx0,int ndIdx1,int ndIdx2);

	/*! Find a triangle that encloses the point.  If none is found, returns -1.  */
	int FindEnclosingTriangleIndex(const YsVec2 &pos) const;

	/*! Find a triangle that is using the node.  Multiple triangles may be using the node, this will find only one of them.
	    If none is found, returns -1. */
	int FindTriangleUsingNode(YSSIZE_T ndIdx) const;

	/*! Find a triangle using the node, and pointing toward the goal position. */
	int FindFirstTriangleForBuildingPipe(YSSIZE_T ndIdx,const YsVec2 &goalPos) const;

	/*! Find a pipe connecting from ndIdx0 to ndIdx1. */
	YsArray <int> FindPipe(YSSIZE_T ndIdx0,YSSIZE_T ndIdx1) const;

	/*! Reduce a pipe triangle. 
	    This currently can take care of edge-recovery if it is easy enough to 
	    deal with a sequence of single-step edge-swappings.  It is not yet
	    smart enough to deal with more complex situations. */
	YSRESULT ReducePipe(YsArray <int> &pipe,YSSIZE_T ndIdx0,YSSIZE_T ndIdx1);

	/*! Recovers a boundary edge. */
	YSRESULT RecoverBoundaryEdge(YSSIZE_T ndIdx0,YSSIZE_T ndIdx1);

	/*! Deletes triangles outside of the domain boundary.
	    BoundaryContour must be a storage of YsArrays of node indices. */
	template <class T>
	YSRESULT DeleteOutsideTri(const T &contour)
	{
		boundaryEdge.CleanUp();
		for(auto &c : contour)
		{
			for(auto i : c.AllIndex())
			{
				boundaryEdge.Add(c[i]);
				boundaryEdge.Add(c.GetCyclic(i+1));
			}
		}
		return DeleteOutsideTriAfterEdgeRegistration();
	}
	

private:
	YSRESULT DeleteOutsideTriAfterEdgeRegistration(void);
	YSSIZE_T FindStartingTriangle(void) const;
	YSSIZE_T StepToNextTriangle(YSSIZE_T curTriIdx,const YsVec2 &goalPos) const;
	YSRESULT CheckPipeValidity(YsArray <int> &pipe) const;

public:
	class SwapInfo
	{
	public:
		//        n[2]                               n[2]
		//   q[3]      q[2]                     q[3]      q[2]
		//    +--------+                         +--------+
		//    |\       |                         |       /|
		//    | \      |                         |      / |
		//    |  \  t1 |                         | t1  /  |
		//    |   \    |  n[1]                   |    /   |  n[1]
		//n[3]|    \   |            =>       n[3]|   /    |       
		//    | t0  \  |                         |  /  t0 |
		//    |      \ |                         | /      |
		//    |       \|                         |/       |
		//    +--------+                         +--------+
		//   q[0]      q[1]                     q[0]      q[1]
		//        n[0]                               n[0]
		YSSIZE_T triIdx[2];
		YSSIZE_T q[4],n[4];
	};
	YSRESULT MakeSwapInfo(SwapInfo &info,YSSIZE_T triIdx0,YSSIZE_T triIdx1) const;
	YSBOOL IsSwappable(const SwapInfo &info) const;
	YSRESULT Swap(SwapInfo &info);
private:
	YSRESULT GetSharedEdge(YSSIZE_T edNdIdx[2],YSSIZE_T triIdx0,YSSIZE_T triIdx1) const;
	YSSIZE_T GetNonSharedTriangleNodeIndex(YSSIZE_T triIdx,const YSSIZE_T edNdIdx[2]) const;

public:
	/*! Returns an index to the neighboring triangle from a triangle and an edge index.  If there is no neighbor, it returns -1. */
	int GetNeighborTriangleIndex(YSSIZE_T fromTriIdx,int edIdx) const;

	/*! Returns an index to the neighboring triangle from a triangle and an edge index.  If there is no neighbor, it returns -1. */
	int GetNeighborTriangleIndex(YSSIZE_T fromTriIdx,YSSIZE_T edNdIdx0,YSSIZE_T edNdIdx1) const;

	/*! Returns the circum circle. */
	YSRESULT GetCircumCircle(YsVec2 &cen,double &rad,int triIdx) const;

	/*! Check if the triangle is alive or deleted. */
	YSBOOL IsTriangleAlive(int triIdx) const;

	/*! Builds and returns the indices of the triangles included in the cavity. 
	    Also it marks IncludedInCavity flags of the triangles. */
	YsArray <int> BuildCavity(const YsVec2 &pos); // Since it changes the flags of the tris, it is not const.

	/*! Perform Delaunay node insertion.  Returns the node index.  -1 indicates that the insertion failed. */
	int InsertNode(const YsVec2 &pos,int intAttrib=0);

	/*! Deletes triangles using one of the initial nodes. */
	void DeleteInitialTriangle(void);

	/*! Returns node indices of the triangle.  If the triangle is not alive, it returns YSERR. */
	YSRESULT GetTriangleNodeIndex(int ndIdx[3],YSSIZE_T triIdx) const;

	/*! Returns integer attribute assigned to the node. */
	int GetNodeIntAttrib(YSSIZE_T ndIdx) const;

	/*! Returns YSTRUE if the node is an initial node.  YSFALSE if not. */
	YSBOOL IsInitialNode(YSSIZE_T ndIdx) const;

	YSSIZE_T GetNumNode(void) const;
	const YsVec2 GetNodePos(YSSIZE_T ndIdx) const;

	YSSIZE_T GetNumTri(void) const;
	YSRESULT GetTriangleNodePos(YsVec2 ndPos[3],YSSIZE_T triIdx) const;
	YsVec2 GetTriangleCenter(int triIdx) const;

	YSRESULT SaveSrf(const char fn[]) const;

protected:
	Triangle &GetTri(YSSIZE_T triIdx)
	{
		return tri[triIdx];
	}
	const Triangle &GetTri(YSSIZE_T triIdx) const
	{
		return tri[triIdx];
	}
	Node &GetNode(YSSIZE_T ndIdx)
	{
		return node[ndIdx];
	}
	const Node &GetNode(YSSIZE_T ndIdx) const
	{
		return node[ndIdx];
	}
};

////////////////////////////////////////////////////////////

/*! Triangulate a non-triangular polygon by using the Delaunay method.
    If the input polygon consists of three vertices, it always returns {0,1,2}.
    If the triangulation fails, it returns as many triangles as it could salvage, and YSERR is set in res.
    The failure is mainly due to boundary-recovery failure, which can be caused by intersecting edges, 
    two vertices at the same location, or numerical difficulties.
    Two overloads that does not take a reference to res will ignore errors.

	It does not take C-loops.  Use YsUnconstrainedDelaunayTriangulation class directly if C-loops are necessary.
*/
YsArray <YSSIZE_T> YsTriangulatePolygonDelaunay(YSRESULT &res,const YsConstArrayMask <YsVec2> &plg);
YsArray <YSSIZE_T> YsTriangulatePolygonDelaunay(YSRESULT &res,const YsConstArrayMask <YsVec3> &plg);
YsArray <YSSIZE_T> YsTriangulatePolygonDelaunay(const YsConstArrayMask <YsVec2> &plg);
YsArray <YSSIZE_T> YsTriangulatePolygonDelaunay(const YsConstArrayMask <YsVec3> &plg);


/* } */
#endif
