/* ////////////////////////////////////////////////////////////

File Name: ysshellext_trackingutil.h
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

#ifndef YSSHELLEXT_TRACKINGUTIL_IS_INCLUDED
#define YSSHELLEXT_TRACKINGUTIL_IS_INCLUDED
/* { */

#include <ysclass.h>
#include <ysshellext.h>

#include "ysshellext_condition.h"

class YsShellExt_TrackingUtil
{
public:
	class SearchCondition : public YsShellExt::Condition
	{
	public:
		// /*! Combination of flags. */
	    // unsigned int searchFlag;

	    /*! If withinThesePlHd is non-NULL, this function will only search within polygons included in withinThesePlHd. */
	    const YsShellPolygonStore *withinThesePlHd;

		/*! If excludeThesePlHd is non-NULL, this function will not go through the polygons included in excludeThesePlHd. */
	    const YsShellPolygonStore *excludeThesePlHd;

		/*! If onlyInThisFgHd is non-NULL, this function will not search outside this face group. */
		YsShellExt::FaceGroupHandle onlyInThisFgHd;

		/*! If YSTRUE, stops at a const-edge.  (TestEdge returns YSERR if the edge piece is used from a const edge.) */
		YSBOOL stopAtConstEdge;

		SearchCondition();

		void Initialize(void);

		/*! This condition will be used for checking if the search should go into the polygon. */
		virtual YSRESULT TestPolygon(const YsShellExt &shl,YsShellPolygonHandle plHd) const;

		/*! This condition will be used 
		    (1) while traversing through polygons to check if the neighboring polygon across this edge must be included in the search, and
		    (2) while traversing through edges to check if the next edge must be included in the search. */
		virtual YSRESULT TestEdge(const YsShellExt &shl,YsShellVertexHandle edVtHd0,YsShellVertexHandle edVtHd1) const;
	};

	class Option
	{
	public:
		YSBOOL crossNonManifoldEdge;
		Option();
	};


	/*! This function tracks vertices based on the connection.  
	   It starts from fromVtHd toward toVtHd and stops when it can no longer find a unique path.
	   If the tracking makes a loop, the first and the last vertices of the returned array will be the same. */
	static YsArray <YsShellVertexHandle> TrackVertexConnection(
		const YsShell &shl,
		YsShellVertexHandle fromVtHd,YsShellVertexHandle toVtHd,
		const YsFixedLengthToMultiHashTable <YsShellVertexHandle,1,2> &connection);

	/*! This function finds and returns a set of polygons on one side of a sequence of vertices. 
	    Every segment in the vertex sequence must at least be used by a polygon.
	    If it cannot find a set of polygons incident to the vertex sequence and
	    on the same side as the given polygons it returns an empty array.  */
	static YsArray <YsShellPolygonHandle> TrackPolygonOneSideOfVertexSequence(
	    const YsShellExt &shl,
	    YSSIZE_T nVt,const YsShellVertexHandle vtHdArray[],
		YSSIZE_T nFlagPl,const YsShellPolygonHandle flagPlHdArray[]);

	/*! This function finds a sequence of vertices by tracking edges starting from vtHd0 toward vtHd1.  
	    If the tracking ends up closing a loop, the first and the last vertices in the array will be identical. 
	    For this function, YsShellExt::Condition is mandatory.  YsShellExt::Condition cond must implement TestEdge function.
	    */
	static YsArray <YsShellVertexHandle> TrackEdge(const YsShellExt &shl,YsShellVertexHandle vtHd0,YsShellVertexHandle vtHd1,YsShellExt::Condition &cond);

	/*! This function finds a sequence of vertices by tracking edges starting from vtHd0.  
	    First edge piece will be selected from one of the connected edges that satisfies the condition.
	    If the tracking ends up closing a loop, the first and the last vertices in the array will be identical. 
	    For this function, YsShellExt::Condition is mandatory.  YsShellExt::Condition cond must implement TestEdge function.
	    */
	static YsArray <YsShellVertexHandle> TrackEdge(const YsShellExt &shl,YsShellVertexHandle vtHd0,YsShellExt::Condition &cond);

	/*! Templated version of TrackPolygonOneSideOfVertexSequence */
	template <const int N,const int M>
	static YsArray <YsShellPolygonHandle> TrackPolygonOneSideOfVertexSequence(const YsShellExt &shl,const YsArray <YsShellVertexHandle,N> &vtHdArray,const YsArray <YsShellPolygonHandle,M> &plHdArray);

	/*! Returns all connected polygons that satisfies the condition starting from plHd0.
	*/
	// By the way, haven't I written this function already?
	static YsArray <YsShell::PolygonHandle> FloodFill(const YsShellExt &shl,YsShell::PolygonHandle plHd0,YsShellExt::Condition &cond);

	/*! This function finds all edge-connected polygons starting from seedPlHd.
	    If YsShellExt::Condition is non-NULL, the search space will be limited by the condition described in YsShellExt::Condition.
	*/
	static YsArray <YsShellPolygonHandle> MakeEdgeConnectedPolygonGroup(
	    const YsShellExt &shl,YsShellPolygonHandle seedPlHd,YsShellExt::Condition *searchCondition);
	static YsArray <YsShellPolygonHandle> MakeEdgeConnectedPolygonGroup(
	    const YsShellExt &shl,YsShellPolygonHandle seedPlHd,const Option &opt,YsShellExt::Condition *searchCondition);

	/*! This function returns a sequence of vertices by connecting specified constraint edges. */
	static YsArray <YsShellVertexHandle> MakeVertexSequenceFromMultiConstEdge(
	    const YsShellExt &shl,YSSIZE_T nCe,const YsShellExt::ConstEdgeHandle ceHdArray[]);

	/*! This function returns a sequence of vertices by connecting specified constraint edges. */
	template <const int N>
	static YsArray <YsShellVertexHandle> MakeVertexSequenceFromMultiConstEdge(
	    const YsShellExt &shl,const YsArray <YsShellExt::ConstEdgeHandle,N> &ceHdArray);

	/*! This function makes a set of polygons that satisfies cond, and is using vertex vtHd0, and edge-connected from plHd0. 
	    Edge vtHd0-vtHd1 must be one of the edges of plHd0.
	    This function assumes that plHd0 satisfies cond and will not check TestPolygon again.
	    The traversal stops at the edge where cond.TestEdge returns YSERR.  

	       <--Direction of traversal
	     +-------+vtHd1
	      \ plg0/
	       \   /
	        \ /
	         + vtHd0

	    If it closes a loop, lastVtHd==vtHd1.
	*/
	static YsArray <YsShellPolygonHandle> MakePolygonFanAroundVertex(
	    YsShellVertexHandle &lastVtHd,
	    const YsShellExt &shl,YsShellVertexHandle vtHd0,YsShellVertexHandle vtHd1,YsShellPolygonHandle plHd0,YsShellExt::Condition &cond);

	/*! This function makes a set of polygons that satisfies cond, and is using vertex vtHd0, and edge-connected from plHd0. 
	    This function assumes that plHd0 satisfies cond and will not check TestPolygon again.
	    The traversal stops at the edge where cond.TestEdge returns YSERR.  */
	static YsArray <YsShellPolygonHandle> MakePolygonFanAroundVertex(
	    const YsShellExt &shl,YsShellVertexHandle vtHd0,YsShellPolygonHandle plHd0,YsShellExt::Condition &cond);

	/*! This function makes a set of polygons that satisfies cond, and is using vertex vtHd0, by tracking edge-connected polygons. 
	    Since polygons may be vertex-connected at vtHd0, or only some of the polygons sharing vtHd0 satisfy cond, there can be multiple set of polygon fans.
	    Therefore it returns an array of PolygonHandle-array.
	    The traversal stops at the edge where cond.TestEdge returns YSERR.  */
	static YsArray <YsArray <YsShell::PolygonHandle> > MakePolygonFanAroundVertex(const YsShellExt &shl,YsShellVertexHandle vtHd0,YsShellExt::Condition &cond);


	/*! This function makes a chain of vertices connected to vtHd0.
	    Every vertex in the returned array is directly connected to vtHd0, therefore, if a quadrilateral is using vtHd0, 
	    the two consecutive vertices in the returned array may not form an edge. 
	    
	    If all polygons are oriented, and if the output orientation needs to be the same orientations as the traversed polygons,
	    vtHd1 needs to be the next vertes of vtHd0 in plHd0.
	    
	    */
	static YsArray <YsShellVertexHandle> MakeVertexFanAroundVertex(
	    YsShellPolygonHandle &lastPlHd,
	    const YsShellExt &shl,YsShell::VertexHandle vtHd0,YsShell::VertexHandle vtHd1,YsShellPolygonHandle plHd0,YsShellExt::Condition &cond);

	/*! This function returns N-neighbor vertices starting from vertex FromVtHd. 
	    The returned array will include FromVtHd. */
	static YsArray <YsShell::VertexHandle> GetNNeighbor(const YsShellExt &Shl,YsShell::VertexHandle FromVtHd,int N);

	/*! This function returns N-neighbor vertices starting from the given set of vertices. 
	    The returned array will include FromVtHd. */
	static YsArray <YsShell::VertexHandle> GetNNeighbor(const YsShellExt &Shl,const YsArray <YsShell::VertexHandle> &FromVtHd,int N);
};

template <const int N,const int M>
/*static*/ YsArray <YsShellPolygonHandle> YsShellExt_TrackingUtil::TrackPolygonOneSideOfVertexSequence(const YsShellExt &shl,const YsArray <YsShellVertexHandle,N> &vtHdArray,const YsArray <YsShellPolygonHandle,M> &plHdArray)
{
	return TrackPolygonOneSideOfVertexSequence(shl,vtHdArray.GetN(),vtHdArray,plHdArray.GetN(),plHdArray);
}

template <const int N>
/*static*/ YsArray <YsShellVertexHandle> YsShellExt_TrackingUtil::MakeVertexSequenceFromMultiConstEdge(
    const YsShellExt &shl,const YsArray <YsShellExt::ConstEdgeHandle,N> &ceHdArray)
{
	return MakeVertexSequenceFromMultiConstEdge(shl,ceHdArray.GetN(),ceHdArray);
}


void YsShellExt_GroupEdgeConnectedPolygonByCondition(YsArray <YsArray <YsShellPolygonHandle> > &plgGrp,const YsShellExt &shl,YsShellExt::Condition *cond);


YsShell::EdgeAndPos YsShellExt_FindNearestPointOnConstEdgeStartingFromEdgePiece(const YsShellExt &shl,YsShell::Edge starEdge,const YsVec3 &refPos);


/*! Starting from vertex vtHd0, it finds the topologically-nearest boundary vertex, which is a vertex that at least one connected edge is used by only one polygon.
    If it does not find any, it returns nullptr.
*/
YsShell::VertexHandle YsShellExt_FindNearestBoundaryVertex(const YsShellExt &shl,const YsShell::VertexHandle vtHd0);

/* } */
#endif
