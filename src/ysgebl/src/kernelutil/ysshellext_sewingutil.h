/* ////////////////////////////////////////////////////////////

File Name: ysshellext_sewingutil.h
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

#ifndef YSSHELLEXT_SEWINGUTIL_IS_INCLUDED
#define YSSHELLEXT_SEWINGUTIL_IS_INCLUDED
/* { */

#include <ysshellext.h>
#include "ysshellext_localop.h"

/*! This class creates an information needed for sewing from one point on the shell to another. 
    The application first creates a vertex sequence and vertex-insertion info by calling MakeInfo. 
    Then iterate through all of AllVtxOnEdge() and insert vertices on the edges.
    Then call MakePolygonSplitInfo, which makes information of how the polygons must split.
    Finally, polygon-split info can be applied to finalize it.

	See ysshellextedit_sewingutil.cpp.
*/
class YsShellExt_SewingInfo : YsShell_LocalOperation
{
protected:
	class VertexPolygonPositionAndIndex
	{
	public:
		YsShellVertexHandle vtHd;
		YsShellPolygonHandle plHd;
		YsVec3 pos;
		int vtxOnEdgeIdx;
		int vtxOnPlgIdx;

		void Initialize(void)
		{
			vtHd=NULL;
			plHd=NULL;
			vtxOnEdgeIdx=-1;
			vtxOnPlgIdx=-1;
		}
		VertexPolygonPositionAndIndex()
		{
			Initialize();
		}
	};

	YsArray <YsShell_LocalOperation::EdgeSplit> vtxOnEdge;
	YsArray <YsShell::VertexPolygonAndPos> vtxOnPlg;
	YsArray <VertexPolygonPositionAndIndex> vtxSequence;

	YsArray <YsShell_LocalOperation::PolygonSplit> plgSplit;
	YsShellEdgeAttribTable <YsShell_LocalOperation::EdgeMidVertex> edgeMidVtx;

public:
	class AllVtxOnEdgeMask
	{
	friend class YsShellExt_SewingInfo;
	private:
		YsArray <YsShell_LocalOperation::EdgeSplit> *vtxOnEdge;
	public:
		inline EdgeSplit &operator[](YSSIZE_T idx)
		{
			return (*vtxOnEdge)[idx];
		}
		inline YSSIZE_T GetN(void) const
		{
			return vtxOnEdge->GetN();
		}

		typedef YsIndexIterator <AllVtxOnEdgeMask,EdgeSplit> iterator;
		inline iterator begin()
		{
			iterator iter(this);
			return iter.begin();
		}
		inline iterator end()
		{
			iterator iter(this);
			return iter.end();
		}
	};
	class AllEdgeMidVertexMask
	{
	friend class YsShellExt_SewingInfo;
	private:
		YsShellEdgeAttribTable <YsShell_LocalOperation::EdgeMidVertex> *edgeMidVtx;
	public:
		typedef YsShellEdgeAttribTable <YsShell_LocalOperation::EdgeMidVertex>::ElemEnumHandle ElemEnumHandle;

		inline void MoveToNext(ElemEnumHandle &elHd) const
		{
			return edgeMidVtx->MoveToNext(elHd);
		}
		inline ElemEnumHandle First(void) const
		{
			return edgeMidVtx->First();
		}
		inline ElemEnumHandle Null(void) const
		{
			return edgeMidVtx->Null();
		}
		inline YsShell_LocalOperation::EdgeMidVertex &Value(ElemEnumHandle elHd)
		{
			return edgeMidVtx->Value(elHd);
		}
		inline const YsShell_LocalOperation::EdgeMidVertex &Value(ElemEnumHandle elHd) const
		{
			return edgeMidVtx->Value(elHd);
		}
		typedef YsUnidirectionalHandleIterator <AllEdgeMidVertexMask,ElemEnumHandle,YsShell_LocalOperation::EdgeMidVertex> iterator;
		iterator begin(void)
		{
			YsUnidirectionalHandleIterator <AllEdgeMidVertexMask,ElemEnumHandle,YsShell_LocalOperation::EdgeMidVertex> iter(this);
			return iter.begin();
		}
		iterator end(void)
		{
			YsUnidirectionalHandleIterator <AllEdgeMidVertexMask,ElemEnumHandle,YsShell_LocalOperation::EdgeMidVertex> iter(this);
			return iter.end();
		}
	};


	class AllVtxOnPlgMask
	{
	friend class YsShellExt_SewingInfo;
	private:
		YsArray <YsShell::VertexPolygonAndPos> *vtxOnPlg;
	public:
		inline YsShell::VertexPolygonAndPos &operator[](YSSIZE_T idx)
		{
			return (*vtxOnPlg)[idx];
		}
		inline YSSIZE_T GetN(void) const
		{
			return vtxOnPlg->GetN();
		}

		typedef YsIndexIterator <AllVtxOnPlgMask,YsShell::VertexPolygonAndPos> iterator;
		inline iterator begin()
		{
			iterator iter(this);
			return iter.begin();
		}
		inline iterator end()
		{
			iterator iter(this);
			return iter.end();
		}
	};

	class AllPolygonSplitMask
	{
	friend class YsShellExt_SewingInfo;
	private:
		const YsArray <YsShell_LocalOperation::PolygonSplit> *plgSplitPtr;
	public:
		inline const PolygonSplit &operator[](YSSIZE_T idx) const
		{
			return (*plgSplitPtr)[idx];
		}
		inline YSSIZE_T GetN(void) const
		{
			return plgSplitPtr->GetN();
		}

		typedef YsIndexIterator <const AllPolygonSplitMask,const YsShell_LocalOperation::PolygonSplit> iterator;
		inline iterator begin()
		{
			iterator iter(this);
			return iter.begin();
		}
		inline iterator end()
		{
			iterator iter(this);
			return iter.end();
		}
	};

public:
	void CleanUp(void);
	YSRESULT MakeInfo(const YsShellExt &shl,YsShellVertexHandle vtHdFrom,YsShellVertexHandle vtHdTo);

	/*! Make sewing info from a vertex along the path.  Path[0] can be at the originating vertex (vtHdFrom),
	    but it doesn't have to. 
	    A point inside polygon will appear in the vertex sequence as index=-1. */
	YSRESULT MakeInfo(const YsShellExt &shl,YsShellVertexHandle vtHdFrom,YSSIZE_T nPath,const YsVec3 path[]);

	template <const int N>
	YSRESULT MakeInfo(const YsShellExt &shl,const YsArray <YsVec3,N> &path)
	{
		return MakeInf9(shl,path.GetN(),path);
	}

	/*! Make sewing info from a sequence of vertices. */
	YSRESULT MakeInfo(const YsShellExt &shl,YSSIZE_T nVt,const YsShellVertexHandle vtHd[]);

	template <const int N>
	YSRESULT MakeInfo(const YsShellExt &shl,const YsArray <YsShellVertexHandle,N> &pathVtHd)
	{
		return MakeInfo(shl,pathVtHd.GetN(),pathVtHd);
	}

	/*! Make sewing info from the starting vertex, a passing point, and the goal vertex. */
	YSRESULT MakeInfo(const YsShellExt &shl,YsShellVertexHandle fromVtHd,const YsVec3 &passPnt,YsShellVertexHandle toVtHd);


private:
	YSRESULT MakeInfoWithVertexHandle(
	    const YsShellExt &shl,YsShellVertexHandle vtHdFrom,
	    YSSIZE_T nPath,const YsVec3 pathPnt[],const YsShellVertexHandle pathVtHd[]);

	YSRESULT CrawlOneSegment(
		YsShellVertexHandle &currentVtHd,
		YsShell::Edge &currentEdge,
		YsShellPolygonHandle &currentPlHd,
		YsVec3 &currentPos,
		const YsShellExt &shl,
		const YsVec3 &goalPos,
		YsShellVertexHandle goalVtHd);

public:
	AllVtxOnEdgeMask AllVtxOnEdge(void);

	AllVtxOnPlgMask AllVtxOnPolygon(void);

	AllEdgeMidVertexMask AllEdgeMidVertex(void);

	/*! Call this function after populating vtxOnEdge. */
	void MakeEdgeMidVertexInfo(const YsShellExt &shl);

	/*! Call this function after MakeEdgeMidVertexInfo */
	void MakePolygonSplitInfo(const YsShellExt &shl);

	AllPolygonSplitMask AllPolygonSplit(void) const;

	/*! This function returns a sequence of vertices after all vtHd member of vtxOnEdge are populated, and MakePolygonSplitInfo is called. */
	YsArray <YsShellVertexHandle> GetVertexSequence(void) const;
};

inline YsShellExt_SewingInfo::AllVtxOnEdgeMask::iterator begin(YsShellExt_SewingInfo::AllVtxOnEdgeMask &dat)
{
	return dat.begin();
}
inline YsShellExt_SewingInfo::AllVtxOnEdgeMask::iterator end(YsShellExt_SewingInfo::AllVtxOnEdgeMask &dat)
{
	return dat.end();
}

inline YsShellExt_SewingInfo::AllPolygonSplitMask::iterator begin(YsShellExt_SewingInfo::AllPolygonSplitMask &dat)
{
	return dat.begin();
}
inline YsShellExt_SewingInfo::AllPolygonSplitMask::iterator end(YsShellExt_SewingInfo::AllPolygonSplitMask &dat)
{
	return dat.end();
}

inline YsShellExt_SewingInfo::AllVtxOnPlgMask::iterator begin(YsShellExt_SewingInfo::AllVtxOnPlgMask &dat)
{
	return dat.begin();
}
inline YsShellExt_SewingInfo::AllVtxOnPlgMask::iterator end(YsShellExt_SewingInfo::AllVtxOnPlgMask &dat)
{
	return dat.end();
}

inline YsShellExt_SewingInfo::AllEdgeMidVertexMask::iterator begin(YsShellExt_SewingInfo::AllEdgeMidVertexMask &dat)
{
	return dat.begin();
}
inline YsShellExt_SewingInfo::AllEdgeMidVertexMask::iterator end(YsShellExt_SewingInfo::AllEdgeMidVertexMask &dat)
{
	return dat.end();
}


/* } */
#endif
