/* ////////////////////////////////////////////////////////////

File Name: ysshellext_sewingutil.cpp
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

#include "ysshellext_sewingutil.h"



void YsShellExt_SewingInfo::CleanUp(void)
{
	vtxOnEdge.CleanUp();
	vtxOnPlg.CleanUp();
	vtxSequence.CleanUp();
	plgSplit.CleanUp();
	edgeMidVtx.CleanUp();
}

YSRESULT YsShellExt_SewingInfo::MakeInfo(const YsShellExt &shl,YsShellVertexHandle vtHdFrom,YsShellVertexHandle vtHdTo)
{
	const YsVec3 pathPnt[1]={shl.GetVertexPosition(vtHdTo)};
	const YsShellVertexHandle pathVtHd[1]={vtHdTo};
	return MakeInfoWithVertexHandle(shl,vtHdFrom,1,pathPnt,pathVtHd);
}

YSRESULT YsShellExt_SewingInfo::MakeInfo(const YsShellExt &shl,YSSIZE_T nVt,const YsShellVertexHandle vtHd[])
{
	if(0<nVt)
	{
		YsArray <YsVec3> pathPnt(nVt,NULL);
		for(YSSIZE_T idx=0; idx<nVt; ++idx)
		{
			pathPnt[idx]=shl.GetVertexPosition(vtHd[idx]);
		}
		return MakeInfoWithVertexHandle(shl,vtHd[0],nVt,pathPnt,vtHd);
	}
	return YSERR;
}

YSRESULT YsShellExt_SewingInfo::MakeInfo(const YsShellExt &shl,YsShellVertexHandle fromVtHd,const YsVec3 &passPnt,YsShellVertexHandle toVtHd)
{
	YsVec3 p[2]={passPnt,shl.GetVertexPosition(toVtHd)};
	YsShellVertexHandle v[2]={NULL,toVtHd};
	return MakeInfoWithVertexHandle(shl,fromVtHd,2,p,v);
}

YSRESULT YsShellExt_SewingInfo::MakeInfoWithVertexHandle(
	    const YsShellExt &shl,YsShellVertexHandle vtHdFrom,
	    YSSIZE_T nPathIn,const YsVec3 pathPntIn[],const YsShellVertexHandle pathVtHdIn[])
{
	// if(NULL!=goalVtHd), goalVtHd must be at path[nPath-1].

	if(0>=nPathIn)
	{
		return YSERR;
	}
	if(0<nPathIn && pathPntIn[0]==shl.GetVertexPosition(vtHdFrom))
	{
		return MakeInfoWithVertexHandle(shl,vtHdFrom,nPathIn-1,pathPntIn+1,pathVtHdIn+1);
	}


	YsConstArrayMask <YsVec3> pathPnt(nPathIn,pathPntIn);
	YsConstArrayMask <YsShellVertexHandle> pathVtHd(nPathIn,pathVtHdIn);

	CleanUp();

	YsShellVertexHandle currentVtHd=vtHdFrom;
	YsShell::Edge currentEdge(NULL,NULL);
	YsShellPolygonHandle currentPlHd=NULL;
	YsVec3 currentPos=shl.GetVertexPosition(vtHdFrom);

	for(YSSIZE_T idx=0; idx<pathPnt.GetN(); ++idx)
	{
		if(YSOK!=CrawlOneSegment(
		    currentVtHd,currentEdge,currentPlHd,currentPos,shl,pathPnt[idx],pathVtHd[idx]))
		{
			return YSERR;
		}
	}

	return YSOK;
}

YSRESULT YsShellExt_SewingInfo::CrawlOneSegment(
		YsShellVertexHandle &currentVtHd,
		YsShell::Edge &currentEdge,
		YsShellPolygonHandle &currentPlHd,
		YsVec3 &currentPos,
		const YsShellExt &shl,
		const YsVec3 &goalPos,
		YsShellVertexHandle goalVtHd)
{
	auto startDir=YsUnitVector(goalPos-currentPos);

	YsShellCrawler crawler;
	crawler.crawlingMode=YsShellCrawler::CRAWL_TO_GOAL;
	crawler.goal=goalPos;

	YSRESULT started=YSERR;
	if(NULL!=currentVtHd)
	{
		started=crawler.Start(shl.Conv(),currentPos,startDir,currentVtHd);
	}
	else if(NULL!=currentEdge[0] && NULL!=currentEdge[1])
	{
		started=crawler.Start(shl.Conv(),currentPos,startDir,currentEdge);
	}
	else if(NULL!=currentPlHd)
	{
		started=crawler.Start(shl.Conv(),currentPos,startDir,currentPlHd);
	}

	if(YSOK==started)
	{
		if(0==vtxSequence.GetN())
		{
			vtxSequence.Increment();
			vtxSequence.Last().Initialize();
			vtxSequence.Last().vtHd=currentVtHd;
			vtxSequence.Last().plHd=crawler.currentPlHd;
			vtxSequence.Last().pos=crawler.currentPos;
		}
		else
		{
			vtxSequence.Last().plHd=crawler.currentPlHd;
		}

// Debugging sphere.srf 371 276 375
// [Selected Vertices]
// Vertex Id:     370  Internal Hash Key:     371  Position: -3.467600  -0.689748   3.535534
// Vertex Id:     275  Internal Hash Key:     276  Position: -4.530637  -1.876651   0.975452
// Vertex Id:     374  Internal Hash Key:     375  Position: -1.964237  -2.939689   3.535534


		YSBOOL watch=YSFALSE;
		while(YSOK==crawler.Crawl(shl.Conv(),0.0,watch))
		{
			if((crawler.currentState==YsShellCrawler::STATE_ON_VERTEX || crawler.currentEdVtHd[0]==crawler.currentEdVtHd[1]) &&
			   crawler.currentEdVtHd[0]==vtxSequence.Last().vtHd)
			{
				vtxSequence.Last().plHd=crawler.currentPlHd;
			}
			else
			{
				switch(crawler.currentState)
				{
				case YsShellCrawler::STATE_IN_POLYGON:
					if(crawler.currentPos==goalPos || 
					   ((NULL==goalVtHd || 0==shl.GetNumPolygonUsingVertex(goalVtHd)) && YSTRUE==crawler.reachedDeadEnd)) // Can happen in the end.
					{
						vtxSequence.Increment();
						vtxSequence.Last().Initialize();
						vtxSequence.Last().plHd=crawler.currentPlHd;
						vtxSequence.Last().vtHd=goalVtHd;
						vtxSequence.Last().pos=crawler.currentPos;
						if(NULL==goalVtHd)
						{
							vtxSequence.Last().vtxOnPlgIdx=(int)vtxOnPlg.GetN();
							vtxOnPlg.Increment();
							vtxOnPlg.Last().plHd=crawler.currentPlHd;
							vtxOnPlg.Last().vtHd=goalVtHd;
							vtxOnPlg.Last().pos=goalPos;
						}
					}
					break;
				case YsShellCrawler::STATE_ON_VERTEX:
					vtxSequence.Increment();
					vtxSequence.Last().Initialize();
					vtxSequence.Last().plHd=crawler.currentPlHd;
					vtxSequence.Last().vtHd=crawler.currentEdVtHd[0];
					vtxSequence.Last().pos=crawler.currentPos;
					break;
				case YsShellCrawler::STATE_ON_EDGE:
					if(crawler.currentEdVtHd[0]==crawler.currentEdVtHd[1])
					{
						vtxSequence.Increment();
						vtxSequence.Last().Initialize();
						vtxSequence.Last().plHd=crawler.currentPlHd;
						vtxSequence.Last().vtHd=crawler.currentEdVtHd[0];
						vtxSequence.Last().pos=crawler.currentPos;
					}
					else
					{
						YsShellVertexHandle onThisVtHd=(crawler.currentPos==goalPos ? goalVtHd : NULL);

						vtxSequence.Increment();
						vtxSequence.Last().Initialize();
						vtxSequence.Last().plHd=crawler.currentPlHd;
						vtxSequence.Last().vtHd=onThisVtHd;
						vtxSequence.Last().vtxOnEdgeIdx=(int)vtxOnEdge.GetN();
						vtxOnEdge.Increment();
						vtxOnEdge.Last().edVtHd[0]=crawler.currentEdVtHd[0];
						vtxOnEdge.Last().edVtHd[1]=crawler.currentEdVtHd[1];
						vtxOnEdge.Last().createdVtHd=onThisVtHd;
						vtxOnEdge.Last().pos=crawler.currentPos;
					}
					break;
				}
			}

			if(YSTRUE==crawler.reachedDeadEnd || goalPos==crawler.currentPos)
			{
				break;
			}
		}


		currentPos=crawler.currentPos;
		switch(crawler.currentState)
		{
		case YsShellCrawler::STATE_IN_POLYGON:
			currentVtHd=NULL;
			currentEdge.edVtHd[0]=NULL;
			currentEdge.edVtHd[1]=NULL;
			currentPlHd=crawler.currentPlHd;
			break;
		case YsShellCrawler::STATE_ON_VERTEX:
			currentVtHd=crawler.currentEdVtHd[0];
			currentEdge.edVtHd[0]=NULL;
			currentEdge.edVtHd[1]=NULL;
			currentPlHd=NULL;
			break;
		case YsShellCrawler::STATE_ON_EDGE:
			if(crawler.currentEdVtHd[0]!=crawler.currentEdVtHd[1])
			{ // Minor glitch that needs to be fixed.
				currentVtHd=NULL;
				currentEdge.edVtHd[0]=crawler.currentEdVtHd[0];
				currentEdge.edVtHd[1]=crawler.currentEdVtHd[1];
				currentPlHd=NULL;
			}
			else
			{
				currentVtHd=crawler.currentEdVtHd[0];
				currentEdge.edVtHd[0]=NULL;
				currentEdge.edVtHd[1]=NULL;
				currentPlHd=NULL;
			}
			break;
		default:
			printf("%s %d\n",__FUNCTION__,__LINE__);
			printf("%d\n",crawler.currentState);
			return YSERR;
		}



		if(crawler.currentPos==goalPos)
		{
			if(NULL==vtxSequence.Last().vtHd)
			{
				if(0<=vtxSequence.Last().vtxOnEdgeIdx)
				{
					printf("%d %d\n",shl.GetSearchKey(vtxOnEdge[vtxSequence.Last().vtxOnEdgeIdx].edVtHd[0]),shl.GetSearchKey(vtxOnEdge[vtxSequence.Last().vtxOnEdgeIdx].edVtHd[1]));
					printf("Landed between %s %s\n",
					   shl.GetVertexPosition(vtxOnEdge[vtxSequence.Last().vtxOnEdgeIdx].edVtHd[0]).Txt(),
					   shl.GetVertexPosition(vtxOnEdge[vtxSequence.Last().vtxOnEdgeIdx].edVtHd[1]).Txt());
				}
				else
				{
					printf("Nowhere to go.\n");
				}
			}
			if(goalVtHd!=vtxSequence.Last().vtHd)
			{
				printf("Could not reach the goal vertex.\n");
				return YSERR;
			}

			return YSOK;
		}

		// If crawler didn't reach the goal, it might mean the goal is slightly off the polygon.
		// It should be allowed because some polygons may be slightly off the plane.
		if((NULL==goalVtHd || 0==shl.GetNumPolygonUsingVertex(goalVtHd)) && YSTRUE==crawler.reachedDeadEnd)
		{
			// vtxSequence is supposed to be incremented in [case YsShellExt::STATE_IN_POLYGON]
			return YSOK;
		}
	}
	return YSERR;
}

YsShellExt_SewingInfo::AllVtxOnEdgeMask YsShellExt_SewingInfo::AllVtxOnEdge(void)
{
	AllVtxOnEdgeMask mask;
	mask.vtxOnEdge=&vtxOnEdge;
	return mask;
}

YsShellExt_SewingInfo::AllVtxOnPlgMask YsShellExt_SewingInfo::AllVtxOnPolygon(void)
{
	AllVtxOnPlgMask mask;
	mask.vtxOnPlg=&vtxOnPlg;
	return mask;
}

YsShellExt_SewingInfo::AllEdgeMidVertexMask YsShellExt_SewingInfo::AllEdgeMidVertex(void)
{
	AllEdgeMidVertexMask mask;
	mask.edgeMidVtx=&edgeMidVtx;
	return mask;
}

void YsShellExt_SewingInfo::MakePolygonSplitInfo(const YsShellExt &shl)
{
	// A concave polygon may be cut twice by one crawling.
	YsShellPolygonAttribTable <YsArray <YsArray <YsShellVertexHandle,2> > > plHdToDivider(shl.Conv());

	for(auto &v : vtxSequence)
	{
		if(NULL==v.vtHd)
		{
			if(0<=v.vtxOnEdgeIdx)
			{
				v.vtHd=vtxOnEdge[v.vtxOnEdgeIdx].createdVtHd;
			}
			else if(0<=v.vtxOnPlgIdx)
			{
				v.vtHd=vtxOnPlg[v.vtxOnPlgIdx].vtHd;
			}
		}
	}

	{
		YsShellPolygonHandle currentPlHd=NULL;
		YsArray <YsShellVertexHandle,2> currentDivider;
		for(YSSIZE_T idx=0; idx<vtxSequence.GetN(); ++idx)
		{
			if(0==currentDivider.GetN())
			{
				currentPlHd=vtxSequence[idx].plHd;
				currentDivider.Append(vtxSequence[idx].vtHd);
			}
			else
			{
				currentDivider.Append(vtxSequence[idx].vtHd);

				if(idx==vtxSequence.GetN()-1 || vtxSequence[idx].plHd!=currentPlHd)
				{
					if(2<=currentDivider.GetN())
					{
						YsArray <YsArray <YsShellVertexHandle,2> > *pldv=plHdToDivider[currentPlHd];
						if(NULL==pldv)
						{
							YsArray <YsArray <YsShellVertexHandle,2> > newDividerArray(1,NULL);
							newDividerArray[0].MoveFrom(currentDivider);
							plHdToDivider.SetAttrib(currentPlHd,newDividerArray);
						}
						else
						{
							pldv->Append(currentDivider);
						}
					}

					currentDivider.CleanUp();
					currentDivider.Append(vtxSequence[idx].vtHd);
				}

				currentPlHd=vtxSequence[idx].plHd;
			}
		}
	}

	for(auto hashHd : plHdToDivider.AllHandle())
	{
		auto plKey=plHdToDivider.GetKey(hashHd);
		auto plHd=shl.FindPolygon(plKey);
		auto &divider=*plHdToDivider[plHd];

		YsSegmentedArray <YsArray <YsShellVertexHandle,4>,4> plVtHd(1,NULL);
		plVtHd[0]=shl.GetPolygonVertex(plHd);

		for(auto d : divider)
		{
			YsArray <YsShellVertexHandle,4> fragment;
			for(auto &pl : plVtHd)
			{
				if(YSOK==YsShellExt_SplitLoopByHandleAndMidNode(pl,fragment,d))
				{
					plVtHd.Append(fragment);
					break;
				}
			}
		}

		plgSplit.Increment();
		plgSplit.Last().plHd=plHd;
		plgSplit.Last().plVtHdArray=plVtHd;
	}
}

void YsShellExt_SewingInfo::MakeEdgeMidVertexInfo(const YsShellExt &shl)
{
	edgeMidVtx.CleanUp();
	edgeMidVtx.SetShell(shl.Conv());
	for(auto &vone : vtxOnEdge)
	{
		YsShell_LocalOperation::EdgeMidVertex *midVtx=edgeMidVtx.FindAttrib(vone.edVtHd[0],vone.edVtHd[1]);
		if(NULL==midVtx)
		{
			YsShell_LocalOperation::EdgeMidVertex newEdgeMidVtx;
			newEdgeMidVtx.edge[0]=vone.edVtHd[0];
			newEdgeMidVtx.edge[1]=vone.edVtHd[1];
			newEdgeMidVtx.midVtHd.MakeUnitLength(vone.createdVtHd);
			edgeMidVtx.SetAttrib(vone.edVtHd,newEdgeMidVtx);
		}
		else
		{
			midVtx->midVtHd.Append(vone.createdVtHd);
		}
	}

	for(auto hd : edgeMidVtx.AllHandle())
	{
		auto &edgVtxInfo=edgeMidVtx.GetAttrib(hd);
		if(1<edgVtxInfo.midVtHd.GetN())
		{
			YsArray <double> dist(edgVtxInfo.midVtHd.GetN(),NULL);
			for(YSSIZE_T idx=0; idx<edgVtxInfo.midVtHd.GetN(); ++idx)
			{
				dist[idx]=shl.GetEdgeLength(edgVtxInfo.edge[0],edgVtxInfo.midVtHd[idx]);
			}
			YsQuickSort(dist.GetN(),dist.GetEditableArray(),edgVtxInfo.midVtHd.GetEditableArray());
		}
	}
}

YsShellExt_SewingInfo::AllPolygonSplitMask YsShellExt_SewingInfo::AllPolygonSplit(void) const
{
	AllPolygonSplitMask mask;
	mask.plgSplitPtr=&plgSplit;
	return mask;
}

YsArray <YsShellVertexHandle> YsShellExt_SewingInfo::GetVertexSequence(void) const
{
	YsArray <YsShellVertexHandle> vtHdSeq(vtxSequence.GetN(),NULL);
	for(YSSIZE_T idx=0; idx<vtxSequence.GetN(); ++idx)
	{
		auto &v=vtxSequence[idx];
		if(NULL!=v.vtHd)
		{
			vtHdSeq[idx]=v.vtHd;
		}
		else if(0<=v.vtxOnEdgeIdx)
		{
			vtHdSeq[idx]=vtxOnEdge[v.vtxOnEdgeIdx].createdVtHd;
		}
		else if(0<=v.vtxOnPlgIdx)
		{
			vtHdSeq[idx]=vtxOnPlg[v.vtxOnPlgIdx].vtHd;
		}
		else
		{
			vtHdSeq[idx]=NULL;
		}
	}
	return vtHdSeq;
}
