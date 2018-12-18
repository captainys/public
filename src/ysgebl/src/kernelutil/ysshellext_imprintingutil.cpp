/* ////////////////////////////////////////////////////////////

File Name: ysshellext_imprintingutil.cpp
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

#include "ysshellext_imprintingutil.h"


YsShellExt_ImprintingUtil::YsShellExt_ImprintingUtil()
{
	dstShlPtr=nullptr;
	srcShlPtr=nullptr;
	parallelImprint_BiDirectional=YSTRUE;
}

void YsShellExt_ImprintingUtil::CleanUp(void)
{
	dstShlLattice.CleanUp();
	srcToDstVtxMap.CleanUp();
	dstEdgeMidVtxTable.CleanUp();
	dstVtxConn.CleanUp();
}
void YsShellExt_ImprintingUtil::SetDestinationShell(const YsShellExt &dstShl)
{
	dstShlPtr=&dstShl;
	dstEdgeMidVtxTable.SetStorage(dstShl.Conv());
}
void YsShellExt_ImprintingUtil::SetSourceShell(const YsShellExt &srcShl)
{
	srcShlPtr=&srcShl;
	srcToDstVtxMap.SetStorage(srcShl.Conv());
	srcEdgeToDstVtxMap.SetStorage(srcShl.Conv());
}
void YsShellExt_ImprintingUtil::Begin(void)
{
	auto &dstShl=*dstShlPtr;

	CleanUp();
	auto nCell=dstShl.GetNumPolygon();
	dstShlLattice.SetDomain(dstShl.Conv(),nCell);
	dstVtxConn.SetStorage(dstShl.Conv());
}
void YsShellExt_ImprintingUtil::MakeParallelImprintingData(
	    const YsArray <YsShell::PolygonHandle> &srcPlHd,
	    const YsArray <YsShellExt::ConstEdgeHandle> &srcCeHd,
	    const YsVec3 &dir)
{
	MakeParallelImprintingInitialVertexMapping(srcPlHd,srcCeHd,dir);
	MakeSrcEdgeToDstVertexMapping(srcPlHd,srcCeHd);
}

void YsShellExt_ImprintingUtil::MakeParallelImprintingInitialVertexMapping(
	    const YsArray <YsShell::PolygonHandle> &srcPlHd,
	    const YsArray <YsShellExt::ConstEdgeHandle> &srcCeHd,
	    const YsVec3 &dir)
{
	auto &srcShl=*srcShlPtr;
	for(auto plHd : srcPlHd)
	{
		for(auto srcVtHd : srcShl.GetPolygonVertex(plHd))
		{
			AddParallelImprintingVertexMapping(srcPlHd,srcVtHd,dir);
		}
	}
	for(auto ceHd : srcCeHd)
	{
		for(auto srcVtHd : srcShl.GetConstEdgeVertex(ceHd))
		{
			AddParallelImprintingVertexMapping(srcPlHd,srcVtHd,dir);
		}
	}
}

void YsShellExt_ImprintingUtil::AddParallelImprintingVertexMapping(const YsArray <YsShell::PolygonHandle> &srcPlHd,YsShell::VertexHandle srcVtHd,const YsVec3 &dir)
{
	auto attribPtr=srcToDstVtxMap[srcVtHd];
	if(nullptr==attribPtr)
	{
		auto &srcShl=*srcShlPtr;
		auto org=srcShl.GetVertexPosition(srcVtHd);
		auto pos=FindFirstIntersection(srcPlHd,org,dir);
		pos.srcVtHd=srcVtHd;
		if(pos.elem.elemType!=YSSHELL_NOELEM)
		{
			// 
		}
		srcToDstVtxMap.SetAttrib(srcVtHd,pos);
	}
}

YsShellExt_ImprintingUtil::PosOnDstShell YsShellExt_ImprintingUtil::FindFirstIntersection(const YsArray <YsShell::PolygonHandle> &srcPlHd,const YsVec3 &org,const YsVec3 &dir) const
{
	auto &dstShl=*dstShlPtr;

	YsShell::PolygonHandle iPlHd[2]={nullptr,nullptr};
	YsVec3 itsc[2];
	YSRESULT res[2]={YSERR,YSERR};

	if(srcShlPtr!=dstShlPtr)
	{
		res[0]=dstShlLattice.FindFirstIntersectionHighPrecision(iPlHd[0],itsc[0],org, dir);
		if(YSTRUE==parallelImprint_BiDirectional)
		{
			res[1]=dstShlLattice.FindFirstIntersectionHighPrecision(iPlHd[1],itsc[1],org,-dir);
		}
	}
	else // if(srcShlPtr==dstShlPtr)
	{
		res[0]=dstShlLattice.FindFirstIntersectionHighPrecision(iPlHd[0],itsc[0],org, dir,srcPlHd.GetN(),srcPlHd);
		if(YSTRUE==parallelImprint_BiDirectional)
		{
			res[1]=dstShlLattice.FindFirstIntersectionHighPrecision(iPlHd[1],itsc[1],org,-dir,srcPlHd.GetN(),srcPlHd);
		}
	}

	YsShellExt_ImprintingUtil::PosOnDstShell posOnDstShl;
	if(nullptr!=iPlHd[0] && nullptr!=iPlHd[1])
	{
		if((itsc[0]-org).GetSquareLength()<(itsc[1]-org).GetSquareLength())
		{
			posOnDstShl.elem.elemType=YSSHELL_POLYGON;
			posOnDstShl.elem.plHd=iPlHd[0];
			posOnDstShl.pos=itsc[0];
		}
		else
		{
			posOnDstShl.elem.elemType=YSSHELL_POLYGON;
			posOnDstShl.elem.plHd=iPlHd[1];
			posOnDstShl.pos=itsc[1];
		}
	}
	else if(nullptr!=iPlHd[0])
	{
		posOnDstShl.elem.elemType=YSSHELL_POLYGON;
		posOnDstShl.elem.plHd=iPlHd[0];
		posOnDstShl.pos=itsc[0];
	}
	else if(nullptr!=iPlHd[1])
	{
		posOnDstShl.elem.elemType=YSSHELL_POLYGON;
		posOnDstShl.elem.plHd=iPlHd[1];
		posOnDstShl.pos=itsc[1];
	}
	else
	{
printf("%s %d\n",__FUNCTION__,__LINE__);
		printf(" O %s\n",org.Txt());
		printf(" V %s\n",dir.Txt());
		posOnDstShl.elem.elemType=YSSHELL_NOELEM;
	}

	return posOnDstShl;
}


void YsShellExt_ImprintingUtil::MakeSrcEdgeToDstVertexMapping(
	    const YsArray <YsShell::PolygonHandle> &srcPlHd,
	    const YsArray <YsShellExt::ConstEdgeHandle> &srcCeHd)
{
	auto &srcShl=*srcShlPtr;
	for(auto plHd : srcPlHd)
	{
		auto plVtHd=srcShl.GetPolygonVertex(plHd);
		for(auto edIdx : plVtHd.AllIndex())
		{
			YsShell::Edge srcEdge;
			srcEdge[0]=plVtHd[edIdx];
			srcEdge[1]=plVtHd.GetCyclic(edIdx+1);
			AddSrcEdgeToVertexMapping(srcEdge);
		}
	}
	for(auto ceHd : srcCeHd)
	{
		YSBOOL isLoop;
		YsArray <YsShell::VertexHandle> ceVtHd;
		srcShl.GetConstEdge(ceVtHd,isLoop,ceHd);
		if(YSTRUE!=isLoop)
		{
			for(YSSIZE_T edIdx=0; edIdx<ceVtHd.GetN()-1; ++edIdx)
			{
				YsShell::Edge srcEdge;
				srcEdge[0]=ceVtHd[edIdx];
				srcEdge[1]=ceVtHd.GetCyclic(edIdx+1);
				AddSrcEdgeToVertexMapping(srcEdge);
			}
		}
		else
		{
			for(auto edIdx : ceVtHd.AllIndex())
			{
				YsShell::Edge srcEdge;
				srcEdge[0]=ceVtHd[edIdx];
				srcEdge[1]=ceVtHd.GetCyclic(edIdx+1);
				AddSrcEdgeToVertexMapping(srcEdge);
			}
		}
	}
}

void YsShellExt_ImprintingUtil::AddSrcEdgeToVertexMapping(YsShell::Edge srcEdge)
{
	auto edgeAttrib=srcEdgeToDstVtxMap.FindAttrib(srcEdge[0],srcEdge[1]);
printf("%s %d\n",__FUNCTION__,__LINE__);
	if(nullptr==edgeAttrib)
	{
		// Cannot deal with the situation if both two ends are outside of the destination shell.
		// Or, if the edge go out of the destination shell, and come in in the middle.
		// Need to think about thes case.

		auto &srcShl=*srcShlPtr;
		auto vtAttrib0=srcToDstVtxMap[srcShl.GetSearchKey(srcEdge[0])];
		auto vtAttrib1=srcToDstVtxMap[srcShl.GetSearchKey(srcEdge[1])];
		YsShell::Elem fromElem;
		YsVec3 fromPos,toPos;
		YsShell::VertexHandle fromVtHdOnSrcShl=nullptr,toVtHdOnSrcShl=nullptr;
		decltype(vtAttrib0) fromAttrib=nullptr,toAttrib=nullptr;

		VtxOnSrcEdge itsc;
		itsc.edge=srcEdge;

		YSBOOL reverse=YSFALSE;
		if(nullptr!=vtAttrib0 && YSSHELL_NOELEM!=vtAttrib0->elem.elemType && 
		   nullptr!=vtAttrib1 && YSSHELL_NOELEM!=vtAttrib1->elem.elemType)
		{
printf("%s %d\n",__FUNCTION__,__LINE__);
			fromElem=vtAttrib0->elem;
			fromPos=vtAttrib0->pos;
			fromVtHdOnSrcShl=srcEdge[0];
			toPos=vtAttrib1->pos;
			toVtHdOnSrcShl=srcEdge[1];

			fromAttrib=vtAttrib0;
			toAttrib=vtAttrib1;
		}
		else if(nullptr!=vtAttrib0 && YSSHELL_NOELEM!=vtAttrib0->elem.elemType)
		{
printf("%s %d\n",__FUNCTION__,__LINE__);
			fromElem=vtAttrib0->elem;
			fromPos=vtAttrib0->pos;
			fromVtHdOnSrcShl=srcEdge[0];
			toPos=srcShl.GetVertexPosition(srcEdge[1]); // Should be a better way.
			toVtHdOnSrcShl=srcEdge[1];

			fromAttrib=vtAttrib0;
			toAttrib=vtAttrib1;
		}
		else if(nullptr!=vtAttrib1 && YSSHELL_NOELEM!=vtAttrib1->elem.elemType)
		{
printf("%s %d\n",__FUNCTION__,__LINE__);
			fromElem=vtAttrib1->elem;
			fromPos=vtAttrib1->pos;
			fromVtHdOnSrcShl=srcEdge[1];
			toPos=srcShl.GetVertexPosition(srcEdge[0]); // Should be a better way.
			toVtHdOnSrcShl=srcEdge[0];

			fromAttrib=vtAttrib1;
			toAttrib=vtAttrib0;

			reverse=YSTRUE;
		}
		else
		{
			return;
		}
		printf("From %.20lf %.20lf %.20lf\n",fromPos.x(),fromPos.y(),fromPos.z());
		printf("To   %.20lf %.20lf %.20lf\n",toPos.x(),toPos.y(),toPos.z());


		// 2016/02/20
		// If fromPos is on an edge, the edge can be:
		// (1) Outgoing edge of the current polygon, or
		// (2) Incoming edge of the current polygon.
		// If it is (1) outgoing edge, the first Crawl function won't change the position in the first Crawl,
		// and the crawler stops at the edge, which is added as an edge-vertex.
		// 
		// If it is (2) incoming edge, an edge-vertex won't be added when it departs the point ,either.
		// 
		// In fact, if the point is added on an edge-vertex twice, that's a problem, too.  Two vertices will
		// be created at the same location in the subsequent EmbedEdgeVertex.


		YsShellCrawler crawler;
		crawler.SetGoal(toPos);
		crawler.crawlingMode=YsShellCrawler::CRAWL_TO_GOAL;
		if(YSOK==crawler.Start(dstShlPtr->Conv(),fromPos,toPos-fromPos,fromElem))
		{
printf("%s %d\n",__FUNCTION__,__LINE__);
			while(YSOK==crawler.Crawl(dstShlPtr->Conv(),0.0))
			{
printf("%s %d\n",__FUNCTION__,__LINE__);
				auto crawlerState=crawler.GetCurrentState();
				if(toPos==crawlerState.pos)
				{
					// 2016/02/21
					//   Looks like there is a YsShellCrawler problem.  It reaches the goal, which is at 
					//   the location of the vertex, but crawlerState.edVtHd[0] is nullptr.
					//   See comment on 2016/02/21 in ysshellutil.cpp.  This should be fixed in the YsShellCrawler.
					printf("Goal! %d\n",nullptr!=crawlerState.edVtHd[0]);
				}
				if(/*crawlerState.pos!=toPos && */ nullptr!=crawlerState.edVtHd[0])
				// 2016/02/20 Condition crawlerState.pos!=toPos is commented out because toPos may be on a vertex or on an edge.
				{
					PosOnDstShell pos;
					if(crawlerState.pos==toPos)
					{
						pos.srcVtHd=toVtHdOnSrcShl;
printf("%s %d\n",__FUNCTION__,__LINE__);
					}
					else if(crawlerState.pos==fromPos)
					{
						pos.srcVtHd=fromVtHdOnSrcShl;
printf("%s %d\n",__FUNCTION__,__LINE__);
					}
					else
					{
						pos.srcVtHd=nullptr;
					}
					if(crawlerState.state==YsShellCrawler::STATE_ON_EDGE)
					{
						pos.elem.elemType=YSSHELL_EDGE;
						pos.elem.edVtHd[0]=crawlerState.edVtHd[0];
						pos.elem.edVtHd[1]=crawlerState.edVtHd[1];
						pos.pos=crawlerState.pos;
						itsc.dstVtHd.Add(pos);
printf("%s %d\n",__FUNCTION__,__LINE__);
printf("a %s ed %d-%d\n",pos.pos.Txt(),dstShlPtr->GetSearchKey(crawlerState.edVtHd[0]),dstShlPtr->GetSearchKey(crawlerState.edVtHd[1]));
					}
					else if(crawlerState.state==YsShellCrawler::STATE_ON_VERTEX)
					{
						pos.elem.elemType=YSSHELL_VERTEX;
						pos.elem.vtHd=crawlerState.edVtHd[0];
						pos.pos=crawlerState.pos;

						// 2016/02/21
						// Must prevent vertex creation in the downstream.
						// Also pos.dstVtHd can be given here.
						if(toPos==crawlerState.pos && nullptr!=toAttrib)
						{
							toAttrib->dstVtHd=crawlerState.edVtHd[0];
							pos.dstVtHd=crawlerState.edVtHd[0];
						}
						else if(fromPos==crawlerState.pos && nullptr!=fromAttrib)
						{
							fromAttrib->dstVtHd=crawlerState.edVtHd[0];
							pos.dstVtHd=crawlerState.edVtHd[0];
						}

						itsc.dstVtHd.Add(pos);

printf("%s %d\n",__FUNCTION__,__LINE__);
printf("b %s\n",pos.pos.Txt());
					}
					else
					{
printf("%s %d\n",__FUNCTION__,__LINE__);
printf("c %s\n",pos.pos.Txt());
					}
					// Duplicate vertex-bug: Was doing itsc.dstVtHd.Add(pos) here.
					// But, there is a possibility that state is STATE_IN_POLYGON.
				}

				if(crawlerState.pos==toPos || YSTRUE==crawler.reachedDeadEnd || YSTRUE==crawler.reachedGoal)
				// 2016/02/20
				//   Since crawlerState.pos!=toPos is commented out in the above if statement,
				//   New condition crawlerState.pos==toPos is added in here.  May be redundant though.
				{
printf("%s %d\n",__FUNCTION__,__LINE__);
					break;
				}
			}
		}

		if(YSTRUE==reverse)
		{
printf("%s %d\n",__FUNCTION__,__LINE__);
			itsc.dstVtHd.Invert();
		}
		srcEdgeToDstVtxMap.SetAttrib(srcEdge,(VtxOnSrcEdge &&)itsc);
	}
}

void YsShellExt_ImprintingUtil::MakeDstEdgeMidVertexTable(void)
{
	for(auto attribHd : srcEdgeToDstVtxMap.AllHandle())
	{
		auto &attrib=srcEdgeToDstVtxMap.GetAttrib(attribHd);
		for(auto &pos : attrib.dstVtHd)
		{
			if(nullptr!=pos.dstVtHd && pos.elem.elemType==YSSHELL_EDGE)
			{
				auto midVtxPtr=dstEdgeMidVtxTable.FindAttrib(pos.elem.edVtHd[0],pos.elem.edVtHd[1]);
				if(nullptr!=midVtxPtr)
				{
					midVtxPtr->Add(pos.dstVtHd);
				}
				else
				{
					YsArray <YsShell::VertexHandle> midVtxArray;
					midVtxArray.Add(pos.dstVtHd);
					dstEdgeMidVtxTable.SetAttrib(pos.elem.edVtHd,(decltype(midVtxArray) &&)midVtxArray);
				}
			}
		}
	}
}

void YsShellExt_ImprintingUtil::MakeDstVertexConnection(void)
{
	for(auto attribHd : srcEdgeToDstVtxMap.AllHandle())
	{
		auto &attrib=srcEdgeToDstVtxMap.GetAttrib(attribHd);
		for(YSSIZE_T idx=0; idx<attrib.dstVtHd.GetN()-1; ++idx)
		{
			YsShell::VertexHandle edVtHd[2]=
			{
				attrib.dstVtHd[idx  ].dstVtHd,
				attrib.dstVtHd[idx+1].dstVtHd
			};
			if(nullptr!=edVtHd[0] && nullptr!=edVtHd[1])
			{
				for(int i=0; i<2; ++i)
				{
					auto from=edVtHd[i];
					auto to=edVtHd[1-i];
					auto connVtHdPtr=dstVtxConn[from];
					if(nullptr!=connVtHdPtr)
					{
						connVtHdPtr->Add(to);
					}
					else
					{
						YsArray <YsShell::VertexHandle> connVtHd;
						connVtHd.Add(to);
						dstVtxConn.SetAttrib(from,connVtHd);
					}
				}
			}
		}
	}
}


YsArray <YsShell::VertexHandle> YsShellExt_ImprintingUtil::FindVertexChainFromSrcEdge(YsShell::VertexHandle vtHd0,YsShell::VertexHandle vtHd1) const
{
	YsArray <YsShell::VertexHandle> vtHdChain;

	auto attribPtr=srcEdgeToDstVtxMap.FindAttrib(vtHd0,vtHd1);
	if(nullptr!=attribPtr)
	{
		auto dstVtx0Ptr=srcToDstVtxMap[attribPtr->edge[0]];
		auto dstVtx1Ptr=srcToDstVtxMap[attribPtr->edge[1]];
		if(nullptr!=dstVtx0Ptr && nullptr!=dstVtx1Ptr)
		{
			vtHdChain.Add(dstVtx0Ptr->dstVtHd);
			for(auto vtx : attribPtr->dstVtHd)
			{
				if(nullptr!=vtx.dstVtHd)
				{
					vtHdChain.Add(vtx.dstVtHd);
				}
			}
			vtHdChain.Add(dstVtx1Ptr->dstVtHd);

			if(attribPtr->edge[0]==vtHd1)
			{
				vtHdChain.Invert();
			}
		}
	}

	return vtHdChain;
}

YsArray <YsShell::VertexHandle> YsShellExt_ImprintingUtil::MakeDstVertexChainFromSrcChain(const YsConstArrayMask <YsShell::VertexHandle> &srcVtxChain,YSBOOL isLoop) const
{
	YsArray <YsShell::VertexHandle> dstVtxChain;

	for(YSSIZE_T idx=0; idx<srcVtxChain.GetN()-1; ++idx)
	{
		auto edgeChain=FindVertexChainFromSrcEdge(srcVtxChain[idx],srcVtxChain[idx+1]);
printf("o %d->%d, %lld\n",dstShlPtr->GetSearchKey(srcVtxChain[idx]),dstShlPtr->GetSearchKey(srcVtxChain[idx+1]),edgeChain.GetN());
		if(0==dstVtxChain.GetN() || (0<edgeChain.GetN() && edgeChain[0]==dstVtxChain.Last()))
		{
			dstVtxChain.DeleteLast();
			dstVtxChain.Add(edgeChain);
		}
	}

	if(3<=srcVtxChain.GetN() && YSTRUE==isLoop)
	{
		auto edgeChain=FindVertexChainFromSrcEdge(srcVtxChain.Last(),srcVtxChain[0]);
printf("c %d->%d, %lld\n",dstShlPtr->GetSearchKey(srcVtxChain.Last()),dstShlPtr->GetSearchKey(srcVtxChain[0]),edgeChain.GetN());
		if(0==dstVtxChain.GetN() || (0<edgeChain.GetN() && edgeChain[0]==dstVtxChain.Last()))
		{
			dstVtxChain.DeleteLast();
			dstVtxChain.Add(edgeChain);
		}
		if(2<=dstVtxChain.GetN() && dstVtxChain[0]==dstVtxChain.Last())
		{
			dstVtxChain.DeleteLast();
		}
	}

	return dstVtxChain;
}

YsArray <YsShellExt_ImprintingUtil::DstVtxChain> YsShellExt_ImprintingUtil::MakeDstVertexChain(
	    const YsArray <YsShell::PolygonHandle> &srcPlHd,
	    const YsArray <YsShellExt::ConstEdgeHandle> &srcCeHd)
{
	auto &dstShl=*dstShlPtr;

	YsArray <DstVtxChain> vtxChain;
	for(auto plHd : srcPlHd)
	{
		auto plVtHd=dstShl.GetPolygonVertex(plHd);
		DstVtxChain chain;
		chain.vtxChain=MakeDstVertexChainFromSrcChain(plVtHd,YSTRUE);
		chain.isLoop=YSTRUE;
		vtxChain.Add(chain);
	}
	for(auto ceHd : srcCeHd)
	{
		YSBOOL isLoop;
		YsArray <YsShell::VertexHandle> ceVtHd;
		dstShl.GetConstEdge(ceVtHd,isLoop,ceHd);
		DstVtxChain chain;
		chain.vtxChain=MakeDstVertexChainFromSrcChain(ceVtHd,isLoop);
		chain.isLoop=isLoop;
		vtxChain.Add(chain);
	}

	return vtxChain;
}

YsArray <YsShell::VertexHandle,2> YsShellExt_ImprintingUtil::MakeCutterPath(const DstVtxChain &vc,YSSIZE_T fromIdx)
{
	auto &dstShl=*dstShlPtr;

	YsArray <YsShell::VertexHandle,2> cutter;

	YSSIZE_T subLoopCount=(YSTRUE==vc.isLoop ? vc.vtxChain.GetN() : vc.vtxChain.GetN()-1-fromIdx);
	YsShell::VertexHandle fromVtHd=vc.vtxChain[fromIdx],toVtHd=vc.vtxChain.GetCyclic(fromIdx+1);

	if(0!=dstShl.GetNumPolygonUsingVertex(fromVtHd) &&
	   0==dstShl.GetNumPolygonUsingEdge(fromVtHd,toVtHd))
	{
		cutter.Add(fromVtHd);
		for(decltype(fromIdx) toIdx=fromIdx+1; toIdx<fromIdx+subLoopCount; ++toIdx)
		{
			auto nextVtHd=vc.vtxChain.GetCyclic(toIdx);
			cutter.Add(nextVtHd);
			if(0!=dstShl.GetNumPolygonUsingVertex(nextVtHd))
			{
				break;
			}
		}

		if(2>cutter.GetN() || 0==dstShl.GetNumPolygonUsingVertex(cutter.Last()))
		{
			cutter.CleanUp();
		}
	}

	return cutter;
}

// Turned out it is more complex.
//YsArray <YsArray <YsShell::VertexHandle> > YsShellExt_ImprintingUtil::FindBridge(YsShell::VertexHandle fromVtHd) const
//{
//	auto &dstShl=*dstShlPtr;
//	YsArray <YsArray <YsShell::VertexHandle> > bridgeArray;
//	if(0<dstShl.GetNumPolygonUsingVertex(fromVtHd))
//	{
//		auto connVtHdPtr=dstVtxConn[fromVtHd];
//		if(nullptr!=connVtHdPtr)
//		{
//			for(auto connVtHd : *connVtHdPtr)
//			{
//				if(0==dstShl.GetNumPolygonUsingEdge(fromVtHd,connVtHd))
//				{
//					YsArray <YsShell::VertexHandle> bridge;
//					bridge.Add(fromVtHd);
//					bridge.Add(connVtHd);
//
//					auto vtHd0=fromVtHd,vtHd1=connVtHd;
//					for(;;)
//					{
//						if(0<dstShl.GetNumPolygonUsingVertex())
//						{
//							break;
//						}
//						
//					}
//				}
//			}
//		}
//	}
//}
