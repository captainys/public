/* ////////////////////////////////////////////////////////////

File Name: ysshellext_offsetutil.h
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

#ifndef YSSHELLEXT_OFFSETUTIL_IS_INCLUDED
#define YSSHELLEXT_OFFSETUTIL_IS_INCLUDED
/* { */

#include <ysclass.h>


/*! This class helps making an offset within 2D entities such as a polygon or a group of polygon.
    An offset can start from a chain of vertices, or a loop defined by the group of polygons. */
class YsShellExt_OffsetUtil2d
{
public:
	class VertexOffset
	{
	public:
		class HangingPolygon // flagPlHd in offset.jnt
		{
		public:
			YsShellPolygonHandle plHd;
			int hairIsOneThisSideOfFromVtHd;
		};

		YsShellVertexHandle fromVtHd,toVtHd;
		YsShellPolygonHandle limitingPlHd;
		YsShellVertexHandle limitingEdVtHd[2];
		YsVec3 dir;
		YsVec3 fromPos,toPos;
		double maxDist;

		// For exception in the open-curve case.  See offset.jnt
		// Non-NULL neiFootVtHd means this vertex is at the tip of open vertex sequence.
		// footVtHd in offset.jnt is same as fromVtHd.
		YsArray <HangingPolygon> hangingPlArray;

		// YsShellExt_OffsetUtil2d may work with YsShellExt_RoundUtil3d.  cornerIdx will be used for connecting newVtxArray index and HalfRoundCorner array.
		YSSIZE_T cornerIdx;

		void Initialize(void);
	};
	class NewPolygon
	{
	public:
		YsArray <YsShellVertexHandle> plVtHd;
		YsShellPolygonHandle fromPlHd;

		void Initialize(void);
	};
	class BoundaryEdge
	{
	public:
		YSSIZE_T newVtxIdx[2];
		YsShellPolygonHandle plHd;
	};

	YsArray <YsShellPolygonHandle> srcPlHdArray;

	YsArray <VertexOffset> newVtxArray;
	YsHashTable <YSSIZE_T> vtKeyToNewVtxIndex;
	YsArray <BoundaryEdge> boundaryEdgeArray;
	YsArray <NewPolygon> newPlgArray;

	/*! Cleans up. */
	void CleanUp(void);

	/*! Set up offseting information for a set of edge-connected polygons.  
	    Returns YSOK if successful.
	    If any of the polygons is vertex-connected, it returns YSERR. */
	YSRESULT SetUpForEdgeConnectedPolygonGroup(const YsShell &shl,YSSIZE_T nPl,const YsShellPolygonHandle plHdArray[]);

	/*! Set up offseting information for a set of edge-connected polygons.  
	    Returns YSOK if successful.
	    If any of the polygons is vertex-connected, it returns YSERR. */
	template <const int N>
	YSRESULT SetUpForEdgeConnectedPolygonGroup(const YsShell &shl,const YsArray <YsShellPolygonHandle,N> &plHdArray);

private:
	YSRESULT SetUpForEdgeConnectedPolygonGroup(const YsShell &shl,const YsArray <YsArray <YsShellVertexHandle> > &boundaryInfo,YSSIZE_T nSrcPl,const YsShellPolygonHandle srcPlHd[]);

	// Note AddNewVertex sets dir member tentatively as a tangential direction.
	void AddNewVertex(const YsShell &shl,const YsShellVertexHandle vtHd[3]);

	YSRESULT AddNewVertexAtTipOfOpenVertexSequence(const YsShell &shl,const YsShellPolygonStore &srcPlHdSet,YsShellVertexHandle footVtHd,YsShellVertexHandle neiFootVtHd);

	YsShellVertexHandle FindHairVertexAtTipOfOpenVertexSequence(
	    YsShellPolygonHandle &srcPlHdOfHairEdge,
	    const YsShell &shl,const YsShellPolygonStore &srcPlHdSet,YsShellVertexHandle footVtHd,YsShellVertexHandle neiFootVtHd);

	void AddBoundaryEdgePiece(const YsShell &shl,const YsShellPolygonStore &srcPlHdSet,const YsShellVertexHandle edVtHd[2]);

	YSRESULT SetVertexLimitAndActualDirection(const YsShell &shl,const YsShellPolygonStore &srcPlHdSet,YsShellVertexHandle vtHd,const YsShellVertexStore &allContorVtx);

public:
	/*! Makes boundary-edge store. */
	void MakeBoundaryEdgeStore(YsTwoKeyStore &edgeStore,const YsShell &shl) const;

public:
	/*! Set up offsetting information for outside of a set of edge-connected polygons.
	    Returns YSOK if successful.
	    If any of the polygons is vertex-connected, it returns YSERR. */
	YSRESULT SetUpForOutsideOfEdgeConnectedPolygonGroup(const YsShell &shl,YSSIZE_T nPl,const YsShellPolygonHandle plHdArray[]);

	/*! Set up offsetting information for outside of a set of edge-connected polygons.
	    Returns YSOK if successful.
	    If any of the polygons is vertex-connected, it returns YSERR. */
	template <const int N>
	YSRESULT SetUpForOutsideOfEdgeConnectedPolygonGroup(const YsShell &shl,const YsArray <YsShellPolygonHandle,N> &plHdArray);


public:
	/*! Set up offsetting information for a set of polygons on one side of a chain of vertices. 
	    srcPlHdArray may include more polygons than specified since additional polygons may need to be offsetted. 
		If specified polygons are not incident to one of the specified vertices, it fails and returns YSERR.
		At least one polygon in the specified polygons must be sharing an edge in the vertex sequence, or it fails and returns YSERR..
		If an edge piece in the specified vertex sequence is not used by any polygons, it fails and returns YSERR.
		Returns YSOK if successful.
	    */
	YSRESULT SetUpForOneSideOfVertexSequence(
	    const class YsShellExt &shl,
	    const YsArray <YsArray <YsShellVertexHandle> > &allContour,
	    const YsArray <YsShellPolygonHandle> &seedPlHdArray);

private:
	YSRESULT SetUpForOneSideOfOpenVertexSequenceWithFullSetOfPolygon(const YsShell &shl,const YsArray <YsArray <YsShellVertexHandle> > &allContour,const YsArray <YsShellPolygonHandle> &seedPlHdArray);
};

template <const int N>
YSRESULT YsShellExt_OffsetUtil2d::SetUpForEdgeConnectedPolygonGroup(const YsShell &shl,const YsArray <YsShellPolygonHandle,N> &plHdArray)
{
	return SetUpForEdgeConnectedPolygonGroup(shl,plHdArray.GetN(),plHdArray);
}

template <const int N>
YSRESULT YsShellExt_OffsetUtil2d::SetUpForOutsideOfEdgeConnectedPolygonGroup(const YsShell &shl,const YsArray <YsShellPolygonHandle,N> &plHdArray)
{
	return SetUpForOutsideOfEdgeConnectedPolygonGroup(shl,plHdArray.GetN(),plHdArray);
}


/* } */
#endif
