/* ////////////////////////////////////////////////////////////

File Name: ysshellext_imprintingutil.h
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

#ifndef YSSHELLEXT_IMPRINTINGUTIL_IS_INCLUDED
#define YSSHELLEXT_IMPRINTINGUTIL_IS_INCLUDED
/* { */

#include <ysmergesort.h>
#include <ysshellext.h>
#include "ysshellext_topologyutil.h"

#include "ysshellext_mappingutil.h"

class YsShellExt_ImprintingUtil
{
public:
	class PosOnDstShell
	{
	public:
		YsShell::Elem elem;
		YsVec3 pos;
		YsShell::VertexHandle srcVtHd,dstVtHd;
		PosOnDstShell()
		{
			srcVtHd=nullptr;
			dstVtHd=nullptr;
		}
	};

	class VtxOnSrcEdge
	{
	public:
		YsShell::Edge edge;
		YsArray <PosOnDstShell> dstVtHd;
	};

	class DstVtxChain
	{
	public:
		YSBOOL isLoop;
		YsArray <YsShell::VertexHandle> vtxChain;
	};

	// Projection can be  Parallel (with given direction.)
	//                    Nearest
	//                    

	//  (1) Make mapping from src vertices to dst vertices
	//  (2) For each source edge, create vertices
	//       - Can go out and come in from the same edge..
	//  (3) Tie vertices created in (2) with the edges.
	//       - More than one vertex may need to be inserted on an edge.
	//       - First create a table that matches a dstEdge and multipel vertices, then insert.

protected:
	YsShellLattice dstShlLattice;
	const YsShellExt *dstShlPtr,*srcShlPtr;

	YSBOOL parallelImprint_BiDirectional;
	YsShellVertexAttribTable <PosOnDstShell> srcToDstVtxMap;
	YsShellEdgeAttribTable <VtxOnSrcEdge> srcEdgeToDstVtxMap;

	YsShellEdgeAttribTable <YsArray <YsShell::VertexHandle> > dstEdgeMidVtxTable;

	YsShellVertexAttribTable <YsArray <YsShell::VertexHandle> > dstVtxConn;

public:
	YsShellExt_ImprintingUtil();
	void CleanUp(void);
	void SetDestinationShell(const YsShellExt &dstShl);
	void SetSourceShell(const YsShellExt &srcShl);

	void Begin(void);
	void MakeParallelImprintingData(
	    const YsArray <YsShell::PolygonHandle> &srcPlHd,
	    const YsArray <YsShellExt::ConstEdgeHandle> &srcCeHd,
	    const YsVec3 &dir);
protected:
	void MakeParallelImprintingInitialVertexMapping(
	    const YsArray <YsShell::PolygonHandle> &srcPlHd,
	    const YsArray <YsShellExt::ConstEdgeHandle> &srcCeHd,
	    const YsVec3 &dir);
	void AddParallelImprintingVertexMapping(const YsArray <YsShell::PolygonHandle> &srcPlHd,YsShell::VertexHandle srcVtHd,const YsVec3 &dir);
	PosOnDstShell FindFirstIntersection(const YsArray <YsShell::PolygonHandle> &srcPlHd,const YsVec3 &org,const YsVec3 &dir) const;

	void MakeSrcEdgeToDstVertexMapping(
	    const YsArray <YsShell::PolygonHandle> &srcPlHd,
	    const YsArray <YsShellExt::ConstEdgeHandle> &srcCeHd);
	void AddSrcEdgeToVertexMapping(YsShell::Edge srcEdge);

public:
	/*! This function must be called after vertices on polygons and edges on the destination shell are created.
	*/
	void MakeDstEdgeMidVertexTable(void);


	void MakeDstVertexConnection(void);

protected:
	YsArray <YsShell::VertexHandle> FindVertexChainFromSrcEdge(YsShell::VertexHandle vtHd0,YsShell::VertexHandle vtHd1) const;

public:
	YsArray <YsShell::VertexHandle> MakeDstVertexChainFromSrcChain(const YsConstArrayMask <YsShell::VertexHandle> &srcVtxChain,YSBOOL isLoop) const;

	YsArray <DstVtxChain> MakeDstVertexChain(
	    const YsArray <YsShell::PolygonHandle> &srcPlHd,
	    const YsArray <YsShellExt::ConstEdgeHandle> &srcCeHd);

public:
	template <class SHLCLASS>
	void CreateVertex(SHLCLASS &dstShl)
	{
		for(auto attribHd : srcToDstVtxMap.AllHandle())
		{
			auto &attrib=srcToDstVtxMap.GetAttrib(attribHd);
			if(YSSHELL_NOELEM!=attrib.elem.elemType && nullptr==attrib.dstVtHd)
			{
				attrib.dstVtHd=dstShl.AddVertex(attrib.pos);
printf("%s %d\n",__FUNCTION__,__LINE__);
printf("%d %s\n",attrib.elem.elemType,attrib.pos.Txt());
			}
		}
	}

	template <class SHLCLASS>
	void CreateVertexOnEdge(SHLCLASS &dstShl)
	{
		for(auto attribHd : srcEdgeToDstVtxMap.AllHandle())
		{
			auto &attrib=srcEdgeToDstVtxMap.GetAttrib(attribHd);
			for(auto &pos : attrib.dstVtHd)
			{
				if(pos.elem.elemType==YSSHELL_VERTEX)
				{
					pos.dstVtHd=pos.elem.vtHd;
				}
				else if(pos.elem.elemType!=YSSHELL_NOELEM)
				{
					if(nullptr!=pos.srcVtHd)
					{
						auto attribPtr=srcToDstVtxMap[pos.srcVtHd];
						if(nullptr!=attribPtr)
						{
							pos.dstVtHd=attribPtr->dstVtHd;
						}
					}
					if(nullptr==pos.dstVtHd)
					{
						pos.dstVtHd=dstShl.AddVertex(pos.pos);
printf("%s %d\n",__FUNCTION__,__LINE__);
printf("%s\n",pos.pos.Txt());
					}
				}
			}
		}
	}

	template <class SHLCLASS>
	void EmbedEdgeVertex(SHLCLASS &dstShl)
	{
		for(auto attribHd : dstEdgeMidVtxTable.AllHandle())
		{
			auto &midVtx=dstEdgeMidVtxTable.GetAttrib(attribHd);
			auto edVtKey=dstEdgeMidVtxTable.GetKey(attribHd);

			YsShell::VertexHandle dstEdVtHd[2]=
			{
				dstShl.FindVertex(edVtKey[0]),
				dstShl.FindVertex(edVtKey[1])
			};

			if(nullptr!=dstEdVtHd[0] && nullptr!=dstEdVtHd[1] && 0<midVtx.GetN())
			{
				YsArray <double> dist;
				for(auto vtHd : midVtx)
				{
					dist.Add(dstShl.GetEdgeLength(vtHd,dstEdVtHd[0]));
				}
				YsSimpleMergeSort<double,YsShell::VertexHandle>(dist.GetN(),dist,midVtx);

				// It may include duplicate vertices.
				for(auto idx=midVtx.GetN()-1; 0<idx; --idx)
				{
					if(midVtx[idx]==midVtx[idx-1])
					{
						midVtx.Delete(idx);
					}
				}

printf("NMV %lld ED %d-%d\n",midVtx.GetN(),dstShl.GetSearchKey(dstEdVtHd[0]),dstShl.GetSearchKey(dstEdVtHd[1]));

				YsShellExt_TopologyUtil topoUtil;
				topoUtil.InsertVertexOnEdge(dstShl,dstEdVtHd,midVtx);
			}
		}
	}

protected:
	template <class SHLCLASS>
	YSRESULT TrySplit(SHLCLASS &dstShl,const YsConstArrayMask <YsShell::VertexHandle> &cutter)
	{
		YSRESULT res=YSERR;
		for(auto vtHd : cutter)
		{
			if(nullptr==vtHd)
			{
				return res;
			}
		}

		YsVec3 tst;
		if(2==cutter.GetN())
		{
			tst=dstShl.GetCenter(cutter[0],cutter[1]);
		}
		else
		{
			tst=dstShl.GetVertexPosition(cutter[cutter.GetN()/2]);
		}

		auto plHdSet0=dstShl.FindPolygonFromVertex(cutter[0]);
		auto plHdSet1=dstShl.FindPolygonFromVertex(cutter.Last());

		auto plHdCandidate=YsCommonArrayElement<YsShell::PolygonHandle>(plHdSet0,plHdSet1);
		for(auto plHd : plHdCandidate)
		{
			YsArray <YsVec3,4> plVtPos;
			dstShl.GetPolygon(plVtPos,plHd);
			if(YSINSIDE==YsCheckInsidePolygon3(tst,plVtPos))
			{
				YsShellExt_TopologyUtil topoUtil;
				if(2==cutter.GetN())
				{
					res=topoUtil.SplitPolygon(dstShl,plHd,cutter[0],cutter.Last());
				}
				else
				{
					res=topoUtil.SplitPolygonWithMidNode(dstShl,plHd,cutter);
				}
				break;
			}
		}

		return res;
	}

	template <class SHLCLASS>
	void CutDstPolygon(SHLCLASS &dstShl,const DstVtxChain &vc)
	{
		YSSIZE_T loopCount=(YSTRUE==vc.isLoop ? vc.vtxChain.GetN() : vc.vtxChain.GetN()-1);
		for(decltype(loopCount) idx=0; idx<=loopCount; ++idx)
		{
			auto cutter=MakeCutterPath(vc,idx);
			if(2<=cutter.GetN() && 0!=dstShl.GetNumPolygonUsingVertex(cutter.Last()))
			{
				if(YSOK==TrySplit(dstShl,cutter))
				{
					dstShl.AddConstEdge(cutter.GetN(),cutter,YSFALSE);
				}
			}
		}
	}

	YsArray <YsShell::VertexHandle,2> MakeCutterPath(const DstVtxChain &vc,YSSIZE_T fromIdx);

public:
	template <class SHLCLASS>
	void CutDstPolygon(SHLCLASS &dstShl,const YsConstArrayMask <DstVtxChain> &vtxChain)
	{
		for(auto vc : vtxChain)
		{
			CutDstPolygon(dstShl,vc);
		}
	}
};



/* } */
#endif
