/* ////////////////////////////////////////////////////////////

File Name: ysshellext_localop.h
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

#ifndef YSSHELLEXT_LOCALOP_IS_INCLUDED
#define YSSHELLEXT_LOCALOP_IS_INCLUDED
/* { */

#include <ysshellext.h>
#include "ysshellext_trackingutil.h"

class YsShell_LocalOperation
{
public:
	class PolygonChange
	{
	public:
		YsShellPolygonHandle plHd;
		YsArray <YsShellVertexHandle,4> plVtHd;

		PolygonChange()
		{
		}
		PolygonChange(const PolygonChange &incoming)
		{
			plHd=incoming.plHd;
			plVtHd=incoming.plVtHd;
		}
		PolygonChange(PolygonChange &&incoming)
		{
			plHd=incoming.plHd;
			plVtHd.MoveFrom(incoming.plVtHd);
		}
		PolygonChange &operator=(const PolygonChange &incoming)
		{
			plHd=incoming.plHd;
			plVtHd=incoming.plVtHd;
			return *this;
		}
		PolygonChange &operator=(PolygonChange &&incoming)
		{
			plHd=incoming.plHd;
			plVtHd.MoveFrom(incoming.plVtHd);
			return *this;
		}
	};
	class ConstEdgeChange
	{
	public:
		YsShellExt::ConstEdgeHandle ceHd;
		YsArray <YsShellVertexHandle> ceVtHd;
		YSBOOL isLoop;

		ConstEdgeChange()
		{
		}
		ConstEdgeChange(const ConstEdgeChange &incoming)
		{
			ceHd=incoming.ceHd;
			ceVtHd=incoming.ceVtHd;
			isLoop=incoming.isLoop;
		}
		ConstEdgeChange(ConstEdgeChange &&incoming)
		{
			ceHd=incoming.ceHd;
			ceVtHd.MoveFrom(incoming.ceVtHd);
			isLoop=incoming.isLoop;
		}
		ConstEdgeChange &operator=(const ConstEdgeChange &incoming)
		{
			ceHd=incoming.ceHd;
			ceVtHd=incoming.ceVtHd;
			isLoop=incoming.isLoop;
			return *this;
		}
		ConstEdgeChange &operator=(ConstEdgeChange &&incoming)
		{
			ceHd=incoming.ceHd;
			ceVtHd.MoveFrom(incoming.ceVtHd);
			isLoop=incoming.isLoop;
			return *this;
		}
	};

	class EdgeSplit
	{
	public:
		YsShellVertexHandle edVtHd[2];
		YsVec3 pos;
		YsShellVertexHandle createdVtHd;

		inline void CleanUp(void)
		{
			edVtHd[0]=NULL;
			edVtHd[1]=NULL;
			createdVtHd=NULL;
		}
	};

	class PolygonSplit
	{
	public:
		YsShellPolygonHandle plHd;
		YsSegmentedArray <YsArray <YsShellVertexHandle,4>,4> plVtHdArray;
		YsSegmentedArray <YsArray <YsVec2,4>,4> prjVtPosArray;

		PolygonSplit()
		{
		}
		PolygonSplit(const PolygonSplit &incoming)
		{
			plHd=incoming.plHd;
			plVtHdArray=incoming.plVtHdArray;
			prjVtPosArray=incoming.prjVtPosArray;
		}
		PolygonSplit(PolygonSplit &&incoming)
		{
			plHd=incoming.plHd;
			plVtHdArray.MoveFrom(incoming.plVtHdArray);
			prjVtPosArray.MoveFrom(incoming.prjVtPosArray);
		}
		PolygonSplit &operator=(const PolygonSplit &incoming)
		{
			plHd=incoming.plHd;
			plVtHdArray=incoming.plVtHdArray;
			prjVtPosArray=incoming.prjVtPosArray;
			return *this;
		}
		PolygonSplit &operator=(PolygonSplit &&incoming)
		{
			plHd=incoming.plHd;
			plVtHdArray.MoveFrom(incoming.plVtHdArray);
			prjVtPosArray.MoveFrom(incoming.prjVtPosArray);
			return *this;
		}
	};
	class EdgeMidVertex
	{
	public:
		YsShell::Edge edge;
		YsArray <YsShellVertexHandle> midVtHd;

		EdgeMidVertex()
		{
		}
		EdgeMidVertex(const EdgeMidVertex &incoming)
		{
			edge=incoming.edge;
			midVtHd=incoming.midVtHd;
		}
		EdgeMidVertex(EdgeMidVertex &&incoming)
		{
			edge=incoming.edge;
			midVtHd.MoveFrom(incoming.midVtHd);
		}
		EdgeMidVertex &operator=(const EdgeMidVertex &incoming)
		{
			edge=incoming.edge;
			midVtHd=incoming.midVtHd;
			return *this;
		}
		EdgeMidVertex &operator=(EdgeMidVertex &&incoming)
		{
			edge=incoming.edge;
			midVtHd.MoveFrom(incoming.midVtHd);
			return *this;
		}
	};
	class ConstEdgeSplit
	{
	public:
		YsShellExt::ConstEdgeHandle ceHd;
		YsSegmentedArray <YsArray <YsShellVertexHandle>,4> ceVtHdArray;

		ConstEdgeSplit()
		{
		}
		ConstEdgeSplit(const ConstEdgeSplit &incoming)
		{
			ceHd=incoming.ceHd;
			ceVtHdArray=incoming.ceVtHdArray;
		}
		ConstEdgeSplit(ConstEdgeSplit &&incoming)
		{
			ceHd=incoming.ceHd;
			ceVtHdArray.MoveFrom(incoming.ceVtHdArray);
		}
		ConstEdgeSplit &operator=(const ConstEdgeSplit &incoming)
		{
			ceHd=incoming.ceHd;
			ceVtHdArray=incoming.ceVtHdArray;
			return *this;
		}
		ConstEdgeSplit &operator=(ConstEdgeSplit &&incoming)
		{
			ceHd=incoming.ceHd;
			ceVtHdArray.MoveFrom(incoming.ceVtHdArray);
			return *this;
		}
	};
	class FaceGroupSplit
	{
	public:
		YsShellExt::FaceGroupHandle fgHd;
		YsSegmentedArray <YsArray <YsShellPolygonHandle>,4> fgPlHdArray;

		FaceGroupSplit()
		{
		}
		FaceGroupSplit(const FaceGroupSplit &incoming)
		{
			fgHd=incoming.fgHd;
			fgPlHdArray=incoming.fgPlHdArray;
		}
		FaceGroupSplit(FaceGroupSplit &&incoming)
		{
			fgHd=incoming.fgHd;
			fgPlHdArray.MoveFrom(incoming.fgPlHdArray);
		}
		FaceGroupSplit &operator=(const FaceGroupSplit &incoming)
		{
			fgHd=incoming.fgHd;
			fgPlHdArray=incoming.fgPlHdArray;
			return *this;
		}
		FaceGroupSplit &operator=(FaceGroupSplit &&incoming)
		{
			fgHd=incoming.fgHd;
			fgPlHdArray.MoveFrom(incoming.fgPlHdArray);
			return *this;
		}

		void CleanUp(void);
	};
};


class YsShell_SwapInfo : public YsShell_LocalOperation
{
public:
	YsShellVertexHandle orgDiagonal[2],newDiagonal[2],quadVtHd[4];
	YsShellVertexHandle newTriVtHd[2][3];
	YsShellPolygonHandle triPlHd[2];

	YsShell_SwapInfo();
	void CleanUp(void);

	/*!
	quadVtHd[0]      quadVtHd[3]           newTriVtHd[1][1]      newTriVtHd[1][0]
	     +--------------+                           +--------------+
	     |\             |                           |             /|newTriVtHd[0][2]
	     | \  triPlHd[1]|                           |            / |
	     |  \           |                           |           /  |
	     |   \          |                           |          /   |
	     |    \         |                           |         /    |
	     |     \        |                           |        /     |
	     |      \       |                           |       /      |
	     |       \      |                =>         |      /       |
	     |        \     |                           |     /        |
	     |         \    |                           |    /         |
	     |          \   |                           |   /          |
	     |triPlHd[0] \  |                           |  /           |
	     |            \ |                           | /            |
	     |             \|           newTriVtHd[1][2]|/             |
	     +--------------+                           +--------------+
	quadVtHd[1]      quadVtHd[2]                newTriVtHd[0][0]    newTriVtHd[0][1]
	*/

	YSRESULT MakeInfo(const YsShell &shl,const YsShell::Edge edge);
	YSRESULT MakeInfo(const YsShell &shl,const YsShellVertexHandle edVtHd[2]);
	YSRESULT MakeInfo(const YsShell &shl,const YsShellVertexHandle edVtHd0,const YsShellVertexHandle edVtHd1);
	YSRESULT MakeInfo(const YsShell &shl,const YsShellPolygonHandle edPlHd[2]);
	YSRESULT MakeInfo(const YsShell &shl,const YsShellPolygonHandle edPlHd0,const YsShellPolygonHandle edPlHd1);

	/*! Call this function after MakeInfo.  It calculates cross products of the edge vectors 0 and 1, 1 and 2, 2 and 3, and 3 and 0.
        If any of the two adjacent cross products are making greater than 90 degrees, it returns YSERR.
        Otherwise it returns YSOK.
	*/
	YSRESULT ConvexityCheck(const YsShell &shl) const;


	/*! Call this function after MakeInfo.  It returns YSERR if the outside quadrilateral is self-intersecting. */
	YSRESULT SelfIntersectionCheck(const YsShell &shl) const;


	/*! Call this function after MakeInfo.  It returns YSERR if the mid point of the new diagonal is outside quadrilateral. */
	YSRESULT NewDiagonalIsInside(const YsShell &shl) const;


	/*! Call this function after MakeInfo.  It returns YSERR if the nearest point of the extensions of the two edges, orgDiagonal and newDiagonal, is outside of either or both of the two edges. 
	   If the nearest point on the new diagonal is close to one of the points of the new diagonal, it means that a very flat triangle is being swapped away and is acceptable.
	   However, if the nearest point on the old diagonal is close to one of the points of the old diagonal, it means that a very flat triangle is being created.  
	   Therefore, in this case, the function returns YSERR. 
	   
       Acceptable.                       Unacceptable
	       |                               |
	       |                               |
	       |                               |
	      -+----New---------              -+----Old---------
	       |                               |
	       |                               |
	       |                               |
	   
	    */
	YSRESULT GeometryCheck(const YsShell &shl) const;

	/*! Calculate errors from vertex-assigned normals before and after the edge swapping.
	    Error is cosine of the maximum angular deviation from the vertex normals.
	    It does not use cached normal obtained by YsShell::GetNormal function of triPlHd[i].  
	    It re-calculates the normal from the vertex coordinates. 
	    Normal vector vtxNom[i] must correspond to quadVtHd[i] and must be a unit vector. */
	void CalculateNormalError(const YsShell &shl,double errorBefore[2],double errorAfter[2],const YsVec3 vtxNom[4]) const;


	/*! Checks if the maximum angular normal error improves after the swapping.
	    This function uses CalculateNormalError function.  Therefore vtxNom[i] must correspond to quadVtHd[i]. */
	YSRESULT NormalImprovementCheck(const YsShell &shl,const YsVec3 vtxNom[4]) const;


	/*! Check if the face group becomes smoother after applying this edge-swapping.
	    It returns YSTRUE if the maximum dihedral angle between the two new triangles, and between a new triangle and neighboring triangle
	    that belong to the same face group is smaller than the maximum dihedral angle from the old triangles.  YSFALSE otherwise.
	*/
	YSBOOL FaceGroupBecomeSmoother(const YsShellExt &shl) const;
private:
	double CalculateCosineOfMaxDhaWithinSameFaceGroup(const YsShellExt &shl,const YsShell::VertexHandle tri[2][3],YsShellExt::FaceGroupHandle fgHd) const;

public:
	/*! Applies the changes. */
	template <class SHLCLASS>
	void Apply(SHLCLASS &shl)
	{
		typename SHLCLASS::StopIncUndo undoGuard(shl);
		shl.SetPolygonVertex(triPlHd[0],3,newTriVtHd[0]);
		shl.SetPolygonVertex(triPlHd[1],3,newTriVtHd[1]);
	}
};


class YsShell_MergeInfo : public YsShell_LocalOperation
{
public:
	YsShellPolygonHandle plHd[2];
	YsArray <YsShellVertexHandle> newPlVtHd;
	YSBOOL duplicateWarning; // YSTRUE -> New polygon may be using a same vertex more than once.

	YsShell_MergeInfo();
	void CleanUp(void);
	YSRESULT MakeInfo(const YsShell &shl,const YsShellPolygonHandle plHd[2]);
	YSRESULT MakeInfo(const YsShell &shl,const YsShellPolygonHandle plHd0,const YsShellPolygonHandle plHd1);

	/*! This function returns the new polygon vertex handles.
	    This function is available after MakeInfo.
	*/
	const YsConstArrayMask <YsShell::VertexHandle> GetNewPolygon(void) const;
};

class YsShell_CollapseInfo : public YsShell_LocalOperation
{
public:
	class Log
	{
	public:
		class Polygon
		{
		public:
			YSHASHKEY hashKey;
			YsArray <YsShell::VertexHandle> plVtHd;
		};

		YsShell::VertexHandle fromVtHd,toVtHd;
		YsArray <Polygon> deletedPlg;
		YsArray <Polygon> plgBeforeCollapse;

		Log &CopyFrom(const Log &incoming)
		{
			fromVtHd=incoming.fromVtHd;
			toVtHd=incoming.toVtHd;
			deletedPlg=incoming.deletedPlg;
			plgBeforeCollapse=incoming.plgBeforeCollapse;
			return *this;
		}
		Log &MoveFrom(Log &incoming)
		{
			fromVtHd=incoming.fromVtHd;
			toVtHd=incoming.toVtHd;
			deletedPlg.MoveFrom(incoming.deletedPlg);
			plgBeforeCollapse.MoveFrom(incoming.plgBeforeCollapse);
			return *this;
		}

		Log(){}
		Log(const Log &incoming)
		{
			CopyFrom(incoming);
		}
		Log(Log &&incoming)
		{
			MoveFrom(incoming);
		}
		Log &operator=(const Log &incoming)
		{
			return CopyFrom(incoming);
		}
		Log &operator=(Log &&incoming)
		{
			return MoveFrom(incoming);
		}
	};
	class UncollapseResult
	{
	public:
		class VertexAndPolygonAndHashKey : public YsShell::VertexAndPolygon
		{
		public:
			YSHASHKEY hashKey;
		};

		/*! Member plHd is the new polygon on the edge fromVtHd-toVtHd.
		    A new polygon is supposed to be a triangle.
		    Member vtHd is the vertex of the polygon not used by fromVtHd-toVtHd.
			Edge fromVtHd-vtHd is a recovered edge by uncollapsing.
			Edge toVtHd-vtHd is a edge that was not affected by this collapsing/uncollapsing.
		*/
		YsArray <VertexAndPolygonAndHashKey> recoveredPlHd;

		YsArray <YsShell::PolygonHandle> changedPlHd;

		UncollapseResult &CopyFrom(const UncollapseResult &incoming)
		{
			recoveredPlHd.CopyFrom(incoming.recoveredPlHd);
			changedPlHd.CopyFrom(incoming.changedPlHd);
			return *this;
		}
		UncollapseResult &MoveFrom(UncollapseResult &incoming)
		{
			recoveredPlHd.MoveFrom(incoming.recoveredPlHd);
			changedPlHd.MoveFrom(incoming.changedPlHd);
			return *this;
		}
		UncollapseResult(){}
		UncollapseResult(const UncollapseResult &incoming)
		{
			CopyFrom(incoming);
		}
		UncollapseResult(UncollapseResult &&incoming)
		{
			MoveFrom(incoming);
		}
		UncollapseResult &operator=(const UncollapseResult &incoming)
		{
			return CopyFrom(incoming);
		}
		UncollapseResult &operator=(UncollapseResult &&incoming)
		{
			return MoveFrom(incoming);
		}
	};

	YsSegmentedArray <PolygonChange,4> newPlg;
	YsSegmentedArray <ConstEdgeChange,4> newConstEdge;
	YsArray <YsShellPolygonHandle> plHdToDel;
	YsArray <YsShellExt::ConstEdgeHandle> ceHdToDel;
	YsShellVertexHandle vtHdToDel,vtHdToSurvive;

	YsShell_CollapseInfo();
	void CleanUp(void);

	/*! This function creates an information for collapsing vtHd[0] into vtHd[1]. 
	    The information includes which polygons will be deleted or modified, const edges will be modified or deleted.
	*/
	YSRESULT MakeInfo(const YsShellExt &shl,const YsShellVertexHandle vtHd[2]);

	/*! This function creates an information for collapsing vtHdToDel into vtHdToSurvive. */
	YSRESULT MakeInfo(const YsShellExt &shl,const YsShellVertexHandle vtHdToDel,const YsShellVertexHandle vtHdToSurvive);

	/*! This function returns a trailing edge of a polygon to be deleted.
	    It is available after MakeInfo.
	    plHdToDel must be one of the polygons that is sharing an edge (fromVtHd-toVtHd).
	*/
	YsShell::Edge GetTrailingEdge(const YsShellExt &shl,YsShell::PolygonHandle plHdToDel) const;
	static YsShell::Edge GetTrailingEdge(const YsShellExt &shl,YsShell::PolygonHandle plHdToDel,YsShell::VertexHandle fromVtHd,YsShell::VertexHandle toVtHd);

	/*! See research note 2015/08/11 
	    Multiple edges will vanish by collapsing an edge.  One is the edge being collapsed.
	    Other edges are the edges that are connected to the vertex to be deleted and are sharing 
	    the opposite vertex with an edge connected to the surviving vertex.
	    If the edge being collapsed is non-manifold, the number may be more than two.
	*/
	void GetVanishingEdge(YsArray <YsShell::Edge,2> &edge,const YsShellExt &shl) const;
	YsArray <YsShell::Edge,2> GetVanishingEdge(const YsShellExt &shl) const;

	YsArray <YsShell::Edge> GetEdgeToDisappear(const YsShellExt &shl) const;

	/*! Returns edges that do not exit currently and will be created by collapsing the edge.
	*/
	YsArray <YsShell::Edge> GetNewEdge(const YsShellExt &shl) const;

	/*! This function returns a log for later un-collapsing.
	    Must be called after MakeInfo before Apply.
	    For later un-collapsing, give YSTRUE to preserveVertex parameter of Apply function.

	    GetLog and Uncollapse functions give primitive un-collapsing capability.
	*/
	Log GetLog(const YsShellExt &shl) const;
	static YsArray <YsShell::PolygonHandle> GetChangedPolygonHandleFromLog(const YsShellExt &shl,const Log &log);

	/*! This function uncollapses the collapse based on the log. 
	    These functions will not restore face groups and const edges.
	*/
	template <class SHLCLASS>
	static UncollapseResult Uncollapse(SHLCLASS &shl,const Log &log)
	{
		UncollapseResult res;
		for(auto &modifiedPlg : log.plgBeforeCollapse)
		{
			auto curPlVtHd=modifiedPlg.plVtHd;
			for(auto &vtHd : curPlVtHd)
			{
				if(log.fromVtHd==vtHd)
				{
					vtHd=log.toVtHd;
				}
			}
			auto plHd=shl.FindPolygonFromVertexSequence(curPlVtHd);
			if(nullptr!=plHd)
			{
				res.changedPlHd.Add(plHd);
				shl.SetPolygonVertex(plHd,modifiedPlg.plVtHd);
			}
			else
			{
				res.changedPlHd.Add(nullptr);
			}
		}

		for(auto &deletedPlg : log.deletedPlg)
		{
			YsShell::VertexHandle unsharedVtHd=nullptr;
			for(auto vtHd : deletedPlg.plVtHd)
			{
				if(vtHd!=log.fromVtHd && vtHd!=log.toVtHd)
				{
					unsharedVtHd=vtHd;
					break;
				}
			}
			auto plHd=shl.AddPolygon(deletedPlg.plVtHd);
			res.recoveredPlHd.Increment();
			res.recoveredPlHd.Last().hashKey=deletedPlg.hashKey;
			res.recoveredPlHd.Last().plHd=plHd;
			res.recoveredPlHd.Last().vtHd=unsharedVtHd;
		}
		return res;
	}

	template <class SHLCLASS>
	void Apply(SHLCLASS &shl,YSBOOL preserveVertex)
	{
		typename SHLCLASS::StopIncUndo undoGuard(shl);

		for(YSSIZE_T idx=0; idx<this->newPlg.GetN(); ++idx)
		{
			shl.SetPolygonVertex(this->newPlg[idx].plHd,this->newPlg[idx].plVtHd);
		}
		for(YSSIZE_T idx=0; idx<this->newConstEdge.GetN(); ++idx)
		{
			shl.ModifyConstEdge(this->newConstEdge[idx].ceHd,this->newConstEdge[idx].ceVtHd,this->newConstEdge[idx].isLoop);
		}
		shl.ForceDeleteMultiPolygon(this->plHdToDel);
		shl.DeleteMultiConstEdge(this->ceHdToDel);
		if(YSTRUE!=preserveVertex)
		{
			shl.DeleteVertex(this->vtHdToDel);
		}
	}


	class TopologyChange
	{
	public:
		/*! If YSTRUE, valence of the edges will change.
		*/
		YSBOOL valenceChange;

		/*! If YSTRUE, a new over-used edge (an edge used by more than 2 polygons) will be created.
		    It is similar to valenceChange.  However, it is different because if an edge is already 
		    over-used, it doesn't count if the edge will be used by more polygons after the collapse.
		    (Like it is oke to collapse an island enclosed by non-manifold edges.
		*/
		YSBOOL newOverUsedEdge;

		/*! If YSTRUE, a single-use edge (an edge used by only one polygon) will be used by
		    2 or more polygons after the collapse.
		*/
		YSBOOL singleUseEdgeClose;

		TopologyChange()
		{
			CleanUp();
		};
		void CleanUp(void)
		{
			valenceChange=YSFALSE;
			newOverUsedEdge=YSFALSE;
			singleUseEdgeClose=YSFALSE;
		}
	};

	/*! After MakeInfo, this function checks if the topological change is tolerated based on the option.
	    (By the way, haven't I written the same function already in the YsShellExt utilities somewhere? 2016/03/28)
	*/
	TopologyChange CheckTopologyChange(const YsShellExt &shl) const;
};

/*! This class makes a list of polygon modifications needed for inserting multiple vertices on an edge. */
class YsShellExt_EdgeVertexInsertionInfo : public YsShell_LocalOperation
{
private:
	YsArray <PolygonChange> plChange;
	YsArray <ConstEdgeChange> ceChange;

public:

	/*! Makes the insertion info.  The vertices to be inserted (vtInset[0]...vtInsert[nVtInsert-1]) must be sorted in the ascending order of the distance from edVtHd[0].
	    Or, some polygon edges may go backward. */
	YSRESULT MakeInfo(const YsShellExt &shl,const YsShellVertexHandle edVtHd[2],YSSIZE_T nVtInsert,const YsShellVertexHandle vtInsert[]);

	/*! Makes the insertion info.  The vertices to be inserted (vtInset[0]...vtInsert[nVtInsert-1]) must be sorted in the ascending order of the distance from edVtHd0.
	    Or, some polygon edges may go backward. */
	YSRESULT MakeInfo(const YsShellExt &shl,YsShellVertexHandle edVtHd0,YsShellVertexHandle edVtHd1,YSSIZE_T nVtInsert,const YsShellVertexHandle vtInsert[]);

	/*! Makes the insertion info.  The vertices to be inserted (vtInset[0]...vtInsert[nVtInsert-1]) must be sorted in the ascending order of the distance from edVtHd[0].
	    Or, some polygon edges may go backward. */
	template <const int N>
	YSRESULT MakeInfo(const YsShellExt &shl,const YsShellVertexHandle edVtHd[2],const YsArray <YsShellVertexHandle,N> &vtInsert)
	{
		return MakeInfo(shl,edVtHd,vtInsert.GetN(),vtInsert);
	}

	/*! Makes the insertion info.  The vertices to be inserted (vtInset[0]...vtInsert[nVtInsert-1]) must be sorted in the ascending order of the distance from edVtHd0.
	    Or, some polygon edges may go backward. */
	template <const int N>
	YSRESULT MakeInfo(const YsShellExt &shl,YsShellVertexHandle edVtHd0,YsShellVertexHandle edVtHd1,const YsArray <YsShellVertexHandle,N> &vtInsert)
	{
		return MakeInfo(shl,edVtHd0,edVtHd1,vtInsert.GetN(),vtInsert);
	}

	/*! Returns polygon changes. */
	const YsArray <PolygonChange> &PolygonChange(void) const;

	/*! Returns const-edge changes. */
	const YsArray <ConstEdgeChange> &ConstEdgeChange(void) const;
};

/*! This function inserts a vertex on an edge of the vertex array. */
template <class VertexType,const int N>
YSRESULT YsShellExt_InsertVertexOnEdgeOfVertexArray(
    YsArray <VertexType,N> &vtHdArray,YSBOOL isLoop,const VertexType edVtHd[2],VertexType vtHd)
{
	if(edVtHd[0]==vtHd || edVtHd[1]==vtHd || 2>vtHdArray.GetN())
	{
		return YSERR;
	}

	YSRESULT found=YSERR;
	for(YSSIZE_T idx=0; idx<vtHdArray.GetN()-1; ++idx)
	{
		if(YSTRUE==YsSameEdge(vtHdArray[idx],vtHdArray[idx+1],edVtHd[0],edVtHd[1]))
		{
			vtHdArray.Insert(idx+1,vtHd);
			found=YSOK;
		}
	}

	if(YSTRUE==isLoop && YSTRUE==YsSameEdge(vtHdArray.GetTop(),vtHdArray.GetEnd(),edVtHd[0],edVtHd[1]))
	{
		vtHdArray.Append(vtHd);
		found=YSOK;
	}

	return found;
}


/*! This function splits a loop into two loops at index idx1 and idx2.  
    The original (input) loop is loopVtHd0.  
    After split, the loop becomes two loops and will be stored in loopVtHd0 and loopVtHd1. 

	If no error, this function returns YSOK.

    Absolute value of idx0-idx1 must be at least 2, or this function returns YSERR.

    ****!  I'm sure I have written an exactly same function before.  I cannot just find it.
    */
template <class VertexType,const int N>
YSRESULT YsShellExt_SplitLoop(YsArray <VertexType,N> &loopVtHd0,YsArray <VertexType,N> &loopVtHd1,YSSIZE_T idx0,YSSIZE_T idx1)
{
	if(YSTRUE==loopVtHd0.IsInRange(idx0) && YSTRUE==loopVtHd0.IsInRange(idx1))
	{
		if(idx1<idx0)
		{
			YsSwapSomething (idx1,idx0);
		}

		if(2>idx1-idx0 || 2>loopVtHd0.GetN()-(idx1-idx0))
		{
			return YSERR;
		}

		loopVtHd1.Set(idx1-idx0+1,loopVtHd0.GetArray()+idx0);

		const YSSIZE_T toDel=idx1-idx0-1;
		loopVtHd0.Delete(idx0+1,toDel);

		return YSOK;
	}
	return YSERR;
}


/*! This function splits an open chain, chain0, at index of idx0 into two separate chains. 

	The first of the two will be returned in chain0, and the rest in chain1.
    */
template <class VertexType,const int N>
YSRESULT YsShellExt_SplitOpenChain(YsArray <VertexType,N> &chain0,YsArray <VertexType,N> &chain1,YSSIZE_T idx0)
{
	if(1<=idx0 && idx0<chain0.GetN()-1)
	{
		chain1.Set(chain0.GetN()-idx0,chain0.GetArray()+idx0);
		chain0.Resize(idx0+1);
		return YSOK;
	}
	return YSERR;
}

template <class VertexType,const int N>
YSRESULT YsShellExt_SplitLoopByHandle(YsArray <VertexType,N> &loopVtHd0,YsArray <VertexType,N> &loopVtHd1,VertexType vtx0,VertexType vtx1)
{
	YSSIZE_T idx0=-1,idx1=-1;
	for(auto idx : loopVtHd0.AllIndex())
	{
		if(loopVtHd0[idx]==vtx0)
		{
			idx0=idx;
		}
		if(loopVtHd0[idx]==vtx1)
		{
			idx1=idx;
		}
	}

	if(0<=idx0 && 0<=idx1)
	{
		return YsShellExt_SplitLoop(loopVtHd0,loopVtHd1,idx0,idx1);
	}
	return YSERR;
}

template <class VertexType,const int N>
YSRESULT YsShellExt_SplitLoopByHandleAndMidNode(
    YsArray <VertexType,N> &loopVtHd0,YsArray <VertexType,N> &loopVtHd1,YSSIZE_T nDivider,const VertexType dividerPtr[])
{
	YsConstArrayMask <VertexType> divider(nDivider,dividerPtr);

	if(2>divider.GetN())
	{
		return YSERR;
	}
	if(2==divider.GetN())
	{
		return YsShellExt_SplitLoopByHandle(loopVtHd0,loopVtHd1,divider[0],divider[1]);
	}

	loopVtHd1.CleanUp();
	YSSIZE_T idx0=-1,idx1=-1;
	for(auto idx : loopVtHd0.AllIndex())
	{
		if(loopVtHd0[idx]==divider[0])
		{
			idx0=idx;
		}
		if(loopVtHd0[idx]==divider.Last())
		{
			idx1=idx;
		}
	}

	if(0<=idx0 && 0<=idx1 && idx0!=idx1)
	{
		YsArray <VertexType,N> loopOriginal;
		loopOriginal.MoveFrom(loopVtHd0);

		YSBOOL reverse=YSFALSE;
		if(idx0>idx1)
		{
			reverse=YSTRUE;
			YsSwapSomething(idx0,idx1);
		}

		loopVtHd0.CleanUp();
		for(YSSIZE_T idx=idx1; idx<loopOriginal.GetN(); ++idx)
		{
			loopVtHd0.Append(loopOriginal[idx]);
		}
		for(YSSIZE_T idx=0; idx<=idx0; ++idx)
		{
			loopVtHd0.Append(loopOriginal[idx]);
		}

		for(YSSIZE_T idx=idx0; idx<=idx1; ++idx)
		{
			loopVtHd1.Append(loopOriginal[idx]);
		}

		if(YSTRUE!=reverse)
		{
			for(YSSIZE_T idx=1; idx<divider.GetN()-1; ++idx)
			{
				loopVtHd0.Append(divider[idx]);
				loopVtHd1.Append(divider[divider.GetN()-1-idx]);
			}
		}
		else
		{
			for(YSSIZE_T idx=1; idx<divider.GetN()-1; ++idx)
			{
				loopVtHd0.Append(divider[divider.GetN()-1-idx]);
				loopVtHd1.Append(divider[idx]);
			}
		}
		return YSOK;
	}

	return YSERR;
}

template <class VertexType,const int N>
YSRESULT YsShellExt_SplitLoopByHandleAndMidNode(
    YsArray <VertexType,N> &loopVtHd0,YsArray <VertexType,N> &loopVtHd1,const YsConstArrayMask <VertexType> &divider)
{
	return YsShellExt_SplitLoopByHandleAndMidNode <VertexType,N> (loopVtHd0,loopVtHd1,divider.GetN(),divider);
}

template <class VertexType,const int N,const int M>
YSRESULT YsShellExt_SplitLoopByHandleAndMidNode(
    YsArray <VertexType,N> &loopVtHd0,YsArray <VertexType,N> &loopVtHd1,const YsArray <VertexType,M> &divider)
{
	return YsShellExt_SplitLoopByHandleAndMidNode <VertexType,N> (loopVtHd0,loopVtHd1,divider.GetN(),divider);
}

template <class VertexType,const int N>
YSRESULT YsShellExt_SplitLoopByHandleAndMidNode(
    YsArray <VertexType,N> &loopVtHd0,YsArray <VertexType,N> &loopVtHd1,YsArray <VertexType,N> &divider)
{
	YsConstArrayMask <VertexType> dividerMask(divider.GetN(),divider);
	return YsShellExt_SplitLoopByHandleAndMidNode <VertexType,N,N> (loopVtHd0,loopVtHd1,divider.GetN(),divider);
}


YsArray <YsShellVertexHandle> YsShellExt_TriangulatePolygon(const YsShell &shl,YsShellPolygonHandle plHd);

/*! Triangulates the polygon defined by plVtHd and returns a sequence of triangles.
    Elements ([n*3], [n*3+1], [n*3+2]) defines the nth polygon (zero based).
*/
YsArray <YsShellVertexHandle> YsShellExt_TriangulatePolygon(const YsShell &shl,const YsConstArrayMask <YsShell::VertexHandle> &plVtHd);


template <class SHLCLASS>
YsShellExt::ConstEdgeHandle YsShellExt_MergeConstEdge(SHLCLASS &shl,YSSIZE_T nCe,const YsShellExt::ConstEdgeHandle ceHdArray[],YSBOOL makeLoopIfClosed)
{
	auto vtHdChain=YsShellExt_TrackingUtil::MakeVertexSequenceFromMultiConstEdge(shl.Conv(),nCe,ceHdArray);
	if(2>vtHdChain.GetN())
	{
		return NULL;
	}

	typename SHLCLASS::StopIncUndo incUndo(shl);
	YSBOOL isLoop=YSFALSE;
	if(YSTRUE==makeLoopIfClosed && 4<=vtHdChain.GetN() && vtHdChain[0]==vtHdChain.Last())
	{
		isLoop=YSTRUE;
		vtHdChain.DeleteLast();
	}

	shl.ModifyConstEdge(ceHdArray[0],vtHdChain,isLoop);
	for(YSSIZE_T idx=1; idx<nCe; ++idx)
	{
		if(ceHdArray[idx]!=ceHdArray[0])
		{
			shl.DeleteConstEdge(ceHdArray[idx]);
		}
	}

	return ceHdArray[0];
}



/* } */
#endif
