/* ////////////////////////////////////////////////////////////

File Name: ysshellext_trackingutil.cpp
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


// I have a feeling that I should have called it a grouping util.

#include <ysvisited.h>


#include "ysshellext_trackingutil.h"
#include "ysshellext_topologyutil.h"


YsShellExt_TrackingUtil::SearchCondition::SearchCondition()
{
	Initialize();
}

void YsShellExt_TrackingUtil::SearchCondition::Initialize(void)
{
	withinThesePlHd=NULL;
	excludeThesePlHd=NULL;
	onlyInThisFgHd=NULL;
	stopAtConstEdge=YSFALSE;
}

YSRESULT YsShellExt_TrackingUtil::SearchCondition::TestPolygon(const YsShellExt &shl,YsShellPolygonHandle plHd) const
{
	if(NULL!=withinThesePlHd && YSTRUE!=withinThesePlHd->IsIncluded(plHd))
	{
		return YSERR;
	}
	if(NULL!=excludeThesePlHd && YSTRUE==excludeThesePlHd->IsIncluded(plHd))
	{
		return YSERR;
	}
	if(NULL!=onlyInThisFgHd)
	{
		auto plFgHd=shl.FindFaceGroupFromPolygon(plHd);
		if(plFgHd!=onlyInThisFgHd)
		{
			return YSERR;
		}
	}
	return YSOK;
}

/* virtual */ YSRESULT YsShellExt_TrackingUtil::SearchCondition::TestEdge(const YsShellExt &shl,YsShellVertexHandle edVtHd0,YsShellVertexHandle edVtHd1) const
{
	if(YSTRUE==stopAtConstEdge && 0<shl.GetNumConstEdgeUsingEdgePiece(edVtHd0,edVtHd1))
	{
		return YSERR;
	}
	return YSOK;
}

////////////////////////////////////////////////////////////

YsShellExt_TrackingUtil::Option::Option()
{
	crossNonManifoldEdge=YSFALSE;
}

////////////////////////////////////////////////////////////


/*static*/ YsArray <YsShellVertexHandle> YsShellExt_TrackingUtil::TrackVertexConnection(
	const YsShell &shl,
	YsShellVertexHandle fromVtHd,YsShellVertexHandle toVtHd,
	const YsFixedLengthToMultiHashTable <YsShellVertexHandle,1,2> &connection)
{
	YsArray <YsShellVertexHandle> trackedPath;

	trackedPath.Append(fromVtHd);

	YsShellVertexHandle prvVtHd=fromVtHd,curVtHd=toVtHd;
	while(NULL!=curVtHd)
	{
		trackedPath.Append(curVtHd);

		if(fromVtHd==curVtHd) // Closed a loop?
		{
			break;
		}

		unsigned curVtKey[1]=
		{
			shl.GetSearchKey(curVtHd)
		};

		YsShellVertexHandle nxtVtHd=NULL;

		YSSIZE_T nConn;
		const YsShellVertexHandle *connVtHd;
		if(YSOK==connection.FindElement(nConn,connVtHd,1,curVtKey) && 2>=nConn)
		{
			if(1==nConn && connVtHd[0]!=prvVtHd)
			{
				nxtVtHd=connVtHd[0];
			}
			else if(2==nConn)
			{
				if(prvVtHd!=connVtHd[1])
				{
					nxtVtHd=connVtHd[1];
				}
				else if(prvVtHd!=connVtHd[0])
				{
					nxtVtHd=connVtHd[0];
				}
			}
		}

		prvVtHd=curVtHd;
		curVtHd=nxtVtHd;
	}

	return trackedPath;
}

/*static*/ YsArray <YsShellPolygonHandle> YsShellExt_TrackingUtil::TrackPolygonOneSideOfVertexSequence(
    const YsShellExt &shl,
    YSSIZE_T nVt,const YsShellVertexHandle vtHdArray[],
	YSSIZE_T nFlagPl,const YsShellPolygonHandle flagPlHdArray[])
{
	YsArray <YsShellPolygonHandle> empty;

	if(YSTRUE!=shl.IsSearchEnabled())
	{
		YsPrintf("%s %d\n",__FUNCTION__,__LINE__);
		YsPrintf("  This function requires a search table.\n");

		return empty;
	}

	if(2>nVt)
	{
		return empty;
	}

	YsTwoKeyStore edgePiece;
	for(YSSIZE_T idx=0; idx<nVt-1; ++idx)
	{
		edgePiece.AddKey(shl.GetSearchKey(vtHdArray[idx]),shl.GetSearchKey(vtHdArray[idx+1]));
	}

	YsShellVertexHandle tipVtHd[2]={NULL,NULL};
	if(vtHdArray[0]!=vtHdArray[nVt-1])
	{
		// Means an open vertex sequence
		tipVtHd[0]=vtHdArray[0];
		tipVtHd[1]=vtHdArray[nVt-1];
	}

	YsShellVertexStore vtxStore((const YsShell &)shl);
	for(YSSIZE_T idx=0; idx<nVt; ++idx)
	{
		vtxStore.AddVertex(vtHdArray[idx]);
	}

	YsArray <YsShellPolygonHandle> seedPlHdArray,nonSeedPlHdArray;
	for(YSSIZE_T plIdx=0; plIdx<nFlagPl; ++plIdx)
	{
		YsArray <YsShellVertexHandle,4> plVtHd;
		shl.GetPolygon(plVtHd,flagPlHdArray[plIdx]);
		for(YSSIZE_T vtIdx=0; vtIdx<plVtHd.GetN(); ++vtIdx)
		{
			if(YSTRUE==edgePiece.IsIncluded(
				shl.GetSearchKey(plVtHd[vtIdx]),
				shl.GetSearchKey(plVtHd.GetCyclic(vtIdx+1))))
			{
				seedPlHdArray.Append(flagPlHdArray[plIdx]);
				goto NEXTFLAGPLG;
			}
		}
		nonSeedPlHdArray.Append(flagPlHdArray[plIdx]);

	NEXTFLAGPLG:
		;
	}

	if(0==seedPlHdArray.GetN())
	{
printf("%s %d\n",__FUNCTION__,__LINE__);
		return empty;
	}


	YsArray <YsShellPolygonHandle> plHdArray;
	plHdArray.Append(nonSeedPlHdArray);
	plHdArray.Append(seedPlHdArray);

	YsShellPolygonStore visited((const YsShell &)shl);
	visited.AddPolygon(plHdArray);


	if(2<=nFlagPl)
	{
		// Polygons need to be edge-connected.
		YsShellExt_TrackingUtil::SearchCondition cond;
		cond.withinThesePlHd=&visited;
		YsArray <YsShellPolygonHandle> verify=YsShellExt_TrackingUtil::MakeEdgeConnectedPolygonGroup(shl,plHdArray[0],&cond);
		if(verify.GetN()!=nFlagPl)
		{
printf("%s %d\n",__FUNCTION__,__LINE__);
			return empty;
		}
	}

printf("%s %d\n",__FUNCTION__,__LINE__);
	
	for(YSSIZE_T ptr=nonSeedPlHdArray.GetN(); ptr<plHdArray.GetN(); ++ptr)
	{
		YsShellPolygonHandle plHd=plHdArray[ptr];

		YsArray <YsShellVertexHandle,4> plVtHd;
		shl.GetPolygon(plVtHd,plHd);

		for(YSSIZE_T vtIdx=0; vtIdx<plVtHd.GetN(); ++vtIdx)
		{
			const YsShellVertexHandle edVtHd[2]=
			{
				plVtHd[vtIdx],
				plVtHd.GetCyclic(vtIdx+1)
			};

			if(edVtHd[0]!=tipVtHd[0] &&
			   edVtHd[1]!=tipVtHd[0] &&
			   edVtHd[0]!=tipVtHd[1] &&
			   edVtHd[1]!=tipVtHd[1] &&
			   YSTRUE!=edgePiece.IsIncluded(shl.GetSearchKey(edVtHd[0]),shl.GetSearchKey(edVtHd[1])) &&
			   (YSTRUE==vtxStore.IsIncluded(edVtHd[0]) || YSTRUE==vtxStore.IsIncluded(edVtHd[1])))
			{
				YsShellPolygonHandle neiPlHd=shl.GetNeighborPolygon(plHd,edVtHd[0],edVtHd[1]);
				if(NULL!=neiPlHd && YSTRUE!=visited.IsIncluded(neiPlHd))
				{
					visited.AddPolygon(neiPlHd);
					plHdArray.Append(neiPlHd);
				}
			}
		}
	}

printf("%s %d\n",__FUNCTION__,__LINE__);

	return plHdArray;
}

/* static */ YsArray <YsShellVertexHandle> YsShellExt_TrackingUtil::TrackEdge(const YsShellExt &shl,YsShellVertexHandle vtHd0,YsShellVertexHandle vtHd1,YsShellExt::Condition &cond)
{
	YsArray <YsShellVertexHandle> vtSeq;

	if(YSOK==cond.TestEdge(shl,vtHd0,vtHd1))
	{
		vtSeq.Append(vtHd0);
		vtSeq.Append(vtHd1);

		for(;;)
		{
			auto connVtHd=shl.GetConnectedVertex(vtSeq.Last());
			if(0==connVtHd.GetN())
			{
				break;
			}

			YsShellVertexHandle nextVtHd=NULL;
			for(auto nextVtHdCandidate : connVtHd)
			{
				if(nextVtHdCandidate!=vtSeq[vtSeq.GetN()-2] &&
				   YSOK==cond.TestEdge(shl,vtSeq.Last(),nextVtHdCandidate))
				{
					if(NULL==nextVtHd)
					{
						nextVtHd=nextVtHdCandidate;
					}
					else
					{
						nextVtHd=NULL;
						break;
					}
				}
			}

			if(NULL!=nextVtHd)
			{
				vtSeq.Append(nextVtHd);
				if(nextVtHd==vtSeq[0])
				{
					break;
				}
			}
			else
			{
				break;
			}
		}
	}

	return vtSeq;
}

/* static */ YsArray <YsShellVertexHandle> YsShellExt_TrackingUtil::TrackEdge(const YsShellExt &shl,YsShellVertexHandle vtHd0,YsShellExt::Condition &cond)
{
	auto connVtHd=shl.GetConnectedVertex(vtHd0);
	for(auto vtHd1 : connVtHd)
	{
		if(YSOK==cond.TestEdge(shl,vtHd0,vtHd1))
		{
			return TrackEdge(shl,vtHd0,vtHd1,cond);
		}
	}

	YsArray <YsShellVertexHandle> empty;
	return empty;
}

/*static*/ YsArray <YsShellPolygonHandle> YsShellExt_TrackingUtil::MakeEdgeConnectedPolygonGroup(
	    const YsShellExt &shl,YsShellPolygonHandle seedPlHd,YsShellExt::Condition *searchCondition)
{
	Option opt;
	return MakeEdgeConnectedPolygonGroup(shl,seedPlHd,opt,searchCondition);
}

/* static */ YsArray <YsShellPolygonHandle> YsShellExt_TrackingUtil::MakeEdgeConnectedPolygonGroup(
	    const YsShellExt &shl,YsShellPolygonHandle seedPlHd,const Option &opt,YsShellExt::Condition *searchCondition)
{
	YsArray <YsShellPolygonHandle> plHdArray;
	YsShellPolygonStore visited((const YsShell &)shl);

	if(YSTRUE!=shl.IsSearchEnabled())
	{
		YsPrintf("%s %d\n",__FUNCTION__,__LINE__);
		YsPrintf("  This function requires a search table.\n");
		return plHdArray;
	}

	plHdArray.Append(seedPlHd);
	visited.AddPolygon(seedPlHd);
	for(YSSIZE_T idx=0; idx<plHdArray.GetN(); ++idx)
	{
		const int nPl=shl.GetNumVertexOfPolygon(plHdArray[idx]);
		auto plVtHd=shl.GetPolygonVertex(plHdArray[idx]);
		for(int edIdx=0; edIdx<nPl; ++edIdx)
		{
			auto allNeiPlHd=shl.FindPolygonFromEdgePiece(plVtHd[edIdx],plVtHd.GetCyclic(edIdx+1));
			if(YSTRUE!=opt.crossNonManifoldEdge && 2!=allNeiPlHd.GetN())
			{
				continue;
			}

			for(auto neiPlHd : allNeiPlHd)
			{
				if(NULL!=searchCondition && YSOK!=searchCondition->TestEdge(shl,plVtHd[edIdx],plVtHd.GetCyclic(edIdx+1)))
				{
					continue;
				}

				if(NULL!=neiPlHd && YSTRUE!=visited.IsIncluded(neiPlHd))
				{
					visited.AddPolygon(neiPlHd);
					if(NULL!=searchCondition && YSOK!=searchCondition->TestPolygon(shl,neiPlHd))
					{
						continue;  // Go to next neighbor
					}
					plHdArray.Append(neiPlHd);
				}
			}
		}
	}

	return plHdArray;
}

/*! This function returns a sequence of vertices by connecting specified constraint edges. */
/*static*/ YsArray <YsShellVertexHandle> YsShellExt_TrackingUtil::MakeVertexSequenceFromMultiConstEdge(
    const YsShellExt &shl,YSSIZE_T nCe,const YsShellExt::ConstEdgeHandle ceHdArray[])
{
	if(1==nCe)
	{
		YsArray <YsShellVertexHandle> ceVtHd;
		YSBOOL isLoop;
		shl.GetConstEdge(ceVtHd,isLoop,ceHdArray[0]);
		if(1<ceVtHd.GetN() && YSTRUE==isLoop && ceVtHd[0]!=ceVtHd.Last())
		{
			ceVtHd.Append(ceVtHd[0]);
		}
		return ceVtHd;
	}

	YsArray <YsShellVertexHandle> vtSeq;

	YsKeyStore selCeKey;
	for(YSSIZE_T idx=1; idx<nCe; ++idx)
	{
		selCeKey.AddKey(shl.GetSearchKey(ceHdArray[idx]));

		YsArray <YsShellVertexHandle> ceVtHd;
		YSBOOL isLoop;
		shl.GetConstEdge(ceVtHd,isLoop,ceHdArray[idx]);
		if(YSTRUE==isLoop || 1>ceVtHd.GetN())
		{
			vtSeq.CleanUp();
			return vtSeq;
		}
	}

	YSBOOL isLoop;
	shl.GetConstEdge(vtSeq,isLoop,ceHdArray[0]);

	if(YSTRUE==isLoop || 2>vtSeq.GetN())
	{
		vtSeq.CleanUp();
		return vtSeq;
	}

	for(YSSIZE_T nUsed=1; nUsed<nCe; ++nUsed)
	{
		YsArray <YsShellExt::ConstEdgeHandle> vtCeHd;
		shl.FindConstEdgeFromVertex(vtCeHd,vtSeq.Last());
		for(auto neiCeHd : vtCeHd)
		{
			if(YSTRUE==selCeKey.IsIncluded(shl.GetSearchKey(neiCeHd)))
			{
				YsArray <YsShellVertexHandle> neiCeVtHd;
				YSBOOL isLoop;
				shl.GetConstEdge(neiCeVtHd,isLoop,neiCeHd);
				if(neiCeVtHd[0]==vtSeq.Last())
				{
					vtSeq.DeleteLast();
					vtSeq.Append(neiCeVtHd);
					selCeKey.DeleteKey(shl.GetSearchKey(neiCeHd));
					goto NEXT;
				}
				else if(neiCeVtHd.Last()==vtSeq.Last())
				{
					neiCeVtHd.Invert();
					vtSeq.DeleteLast();
					vtSeq.Append(neiCeVtHd);
					selCeKey.DeleteKey(shl.GetSearchKey(neiCeHd));
					goto NEXT;
				}
			}
		}
		shl.FindConstEdgeFromVertex(vtCeHd,vtSeq[0]);
		for(auto neiCeHd : vtCeHd)
		{
			if(YSTRUE==selCeKey.IsIncluded(shl.GetSearchKey(neiCeHd)))
			{
				YsArray <YsShellVertexHandle> neiCeVtHd;
				YSBOOL isLoop;
				shl.GetConstEdge(neiCeVtHd,isLoop,neiCeHd);
				if(neiCeVtHd[0]==vtSeq[0])
				{
					vtSeq.Invert();
					vtSeq.DeleteLast();
					vtSeq.Append(neiCeVtHd);
					selCeKey.DeleteKey(shl.GetSearchKey(neiCeHd));
					goto NEXT;
				}
				else if(neiCeVtHd.Last()==vtSeq[0])
				{
					neiCeVtHd.Invert();
					vtSeq.Invert();
					vtSeq.DeleteLast();
					vtSeq.Append(neiCeVtHd);
					selCeKey.DeleteKey(shl.GetSearchKey(neiCeHd));
					goto NEXT;
				}
			}
		}

		// Didn't find any?
		vtSeq.CleanUp();
		return vtSeq;

	NEXT:
		;
	}
	return vtSeq;
}

////////////////////////////////////////////////////////////

void YsShellExt_GroupEdgeConnectedPolygonByCondition(YsArray <YsArray <YsShellPolygonHandle> > &plgGrp,const YsShellExt &shl,YsShellExt::Condition *cond)
{
	YsShellPolygonStore visited(shl.Conv());

	plgGrp.CleanUp();

	for(auto plHd : shl.AllPolygon())
	{
		if(YSTRUE!=visited.IsIncluded(plHd) && YSOK==cond->TestPolygon(shl,plHd))
		{
			auto grp=YsShellExt_TrackingUtil::MakeEdgeConnectedPolygonGroup(shl,plHd,cond);

			for(auto p : grp)
			{
				visited.AddPolygon(p);
			}

			plgGrp.Increment();
			plgGrp.Last().MoveFrom(grp);
		}
	}
}

////////////////////////////////////////////////////////////

/* static */ YsArray <YsShellPolygonHandle> YsShellExt_TrackingUtil::MakePolygonFanAroundVertex(
	    YsShellVertexHandle &lastVtHd,const YsShellExt &shl,YsShellVertexHandle vtHd0,YsShellVertexHandle vtHd1,YsShellPolygonHandle plHd0,YsShellExt::Condition &cond)
{
	YsArray <YsShellPolygonHandle> fanPlHd;
	fanPlHd.Append(plHd0);

	auto curPlHd=plHd0;
	auto curVtHd=vtHd1;

	for(;;)  // As long as it can go
	{
		YsShellPolygonHandle nextPlHd=NULL;
		YsShellVertexHandle nextVtHd=NULL;

		auto plVtHd=shl.GetPolygonVertex(curPlHd);
		for(YSSIZE_T idx=0; idx<plVtHd.GetN(); ++idx)
		{
			if(plVtHd[idx]==vtHd0)
			{
				if(NULL!=nextVtHd)
				{
					nextVtHd=NULL;
					break;
				}
				else
				{
					if(plVtHd.GetCyclic(idx-1)==curVtHd && plVtHd.GetCyclic(idx+1)!=curVtHd)
					{
						nextVtHd=plVtHd.GetCyclic(idx+1);
					}
					else if(plVtHd.GetCyclic(idx-1)!=curVtHd && plVtHd.GetCyclic(idx+1)==curVtHd)
					{
						nextVtHd=plVtHd.GetCyclic(idx-1);
					}
					else
					{
						nextVtHd=NULL;
						break;
					}
				}
			}
		}

		if(NULL==nextVtHd)
		{
			break;
		}

		lastVtHd=nextVtHd;

		if(YSOK!=cond.TestEdge(shl,vtHd0,nextVtHd))
		{
			break;
		}

		nextPlHd=shl.GetNeighborPolygon(curPlHd,vtHd0,nextVtHd);
		if(NULL==nextPlHd || YSOK!=cond.TestPolygon(shl,nextPlHd) || YSTRUE==fanPlHd.IsIncluded(nextPlHd))
		{
			break;
		}

		fanPlHd.Append(nextPlHd);

		curPlHd=nextPlHd;
		curVtHd=nextVtHd;
	}

	return fanPlHd;
}

/* static */ YsArray <YsShellPolygonHandle> YsShellExt_TrackingUtil::MakePolygonFanAroundVertex(
	    const YsShellExt &shl,YsShellVertexHandle vtHd0,YsShellPolygonHandle plHd0,YsShellExt::Condition &cond)
{
	int nPlVt;
	const YsShellVertexHandle *plVtHdPtr;
	shl.GetPolygon(nPlVt,plVtHdPtr,plHd0);

	YsConstArrayMask <YsShellVertexHandle> plVtHd(nPlVt,plVtHdPtr);

	YsArray <YsShellPolygonHandle> plHdArray;
	// 2015/08/27 Debug.  plHd0 shouldn't be added to plHdArray here.  It may end up appearing in the middle.
	// plHdArray.Append(plHd0);  <- Was an error.

	for(int vtIdx=0; vtIdx<nPlVt; ++vtIdx)
	{
		if(plVtHd[vtIdx]==vtHd0)
		{
			YsShellVertexHandle lastVtHd;
			auto fanPlHd=MakePolygonFanAroundVertex(lastVtHd,shl,vtHd0,plVtHd.GetCyclic(vtIdx-1),plHd0,cond);
			if(lastVtHd!=plVtHd.GetCyclic(vtIdx-1))
			// If lastVtHd==plVtHd.GetCyclic(vtIdx-1), it closed a loop.
			{
				fanPlHd.Invert();
				plHdArray.Append(fanPlHd.GetN()-1,fanPlHd.GetArray());

				fanPlHd=MakePolygonFanAroundVertex(lastVtHd,shl,vtHd0,plVtHd.GetCyclic(vtIdx+1),plHd0,cond);
				plHdArray.Append(fanPlHd);
			}
			else
			{
				plHdArray=fanPlHd;
			}
		}
	}

	return plHdArray;
}


/* static */ YsArray <YsArray <YsShell::PolygonHandle> > YsShellExt_TrackingUtil::MakePolygonFanAroundVertex(const YsShellExt &shl,YsShellVertexHandle vtHd0,YsShellExt::Condition &cond)
{
	YsVisited <const YsShell,YsShell::PolygonHandle> visitedPlg;

	YsArray <YsArray <YsShell::PolygonHandle> > fan;

	auto vtPlHd=shl.FindPolygonFromVertex(vtHd0);
	for(auto plHd0 : vtPlHd)
	{
		if(YSTRUE!=visitedPlg.IsIncluded(shl.Conv(),plHd0) && YSOK==cond.TestPolygon(shl.Conv(),plHd0))
		{
			auto f=MakePolygonFanAroundVertex(shl,vtHd0,plHd0,cond);
			visitedPlg.AddMulti(shl.Conv(),f);
			fan.Increment();
			fan.Last().MoveFrom(f);
		}
	}

	return fan;
}


// Almost same code as MakePolygonFanAroundVertex.  Is there any way to unify them without using macros?
/* static */ YsArray <YsShellVertexHandle> YsShellExt_TrackingUtil::MakeVertexFanAroundVertex(
	    YsShellPolygonHandle &lastPlHd,
	    const YsShellExt &shl,YsShell::VertexHandle vtHd0,YsShell::VertexHandle vtHd1,YsShellPolygonHandle plHd0,YsShellExt::Condition &cond)
{
	YsArray <YsShell::VertexHandle> fanVtHd;
	fanVtHd.Append(vtHd1);

	auto curPlHd=plHd0;
	auto curVtHd=vtHd1;

	for(;;)  // As long as it can go
	{
		YsShellPolygonHandle nextPlHd=NULL;
		YsShellVertexHandle nextVtHd=NULL;

		auto plVtHd=shl.GetPolygonVertex(curPlHd);
		for(YSSIZE_T idx=0; idx<plVtHd.GetN(); ++idx)
		{
			if(plVtHd[idx]==vtHd0)
			{
				if(NULL!=nextVtHd)
				{
					nextVtHd=NULL;
					break;
				}
				else
				{
					if(plVtHd.GetCyclic(idx-1)==curVtHd && plVtHd.GetCyclic(idx+1)!=curVtHd)
					{
						nextVtHd=plVtHd.GetCyclic(idx+1);
					}
					else if(plVtHd.GetCyclic(idx-1)!=curVtHd && plVtHd.GetCyclic(idx+1)==curVtHd)
					{
						nextVtHd=plVtHd.GetCyclic(idx-1);
					}
					else
					{
						nextVtHd=NULL;
						break;
					}
				}
			}
		}

		if(NULL==nextVtHd)
		{
			break;
		}

		lastPlHd=nextPlHd;

		if(YSOK!=cond.TestEdge(shl,vtHd0,nextVtHd))
		{
			break;
		}

		nextPlHd=shl.GetNeighborPolygon(curPlHd,vtHd0,nextVtHd);
		if(NULL==nextPlHd || YSOK!=cond.TestPolygon(shl,nextPlHd) || YSTRUE==fanVtHd.IsIncluded(nextVtHd))
		{
			break;
		}

		fanVtHd.Append(nextVtHd);

		curPlHd=nextPlHd;
		curVtHd=nextVtHd;
	}

	return fanVtHd;
}


/* static */ YsArray <YsShell::VertexHandle> YsShellExt_TrackingUtil::GetNNeighbor(const YsShellExt &shl,YsShell::VertexHandle fromVtHd,int nNeighbor)
{
	YsArray <YsShell::VertexHandle> f;
	f.Add(fromVtHd);
	return GetNNeighbor(shl,f,nNeighbor);
}

/* static */ YsArray <YsShell::VertexHandle> YsShellExt_TrackingUtil::GetNNeighbor(const YsShellExt &shl,const YsArray <YsShell::VertexHandle> &fromVtHd,int nNeighbor)
{
	YsArray <YsShell::VertexHandle> neiVtHd=fromVtHd;
	YsArray <int> nRing(fromVtHd.GetN(),nullptr);
	YsVisited <YsShellExt,YsShell::VertexHandle> visited;

	for(auto &n : nRing)
	{
		n=0;
	}
	for(auto vtHd : fromVtHd)
	{
		visited.Add(shl,vtHd);
	}

	for(YSSIZE_T idx=0; idx<neiVtHd.GetN(); ++idx)
	{
		auto n=nRing[idx];
		if(n<nNeighbor)
		{
			for(auto connVtHd : shl.GetConnectedVertex(neiVtHd[idx]))
			{
				if(YSTRUE!=visited.IsIncluded(shl,connVtHd))
				{
					visited.Add(shl,connVtHd);
					neiVtHd.Add(connVtHd);
					nRing.Add(n+1);
				}
			}
		}
		else
		{
			break;
		}
	}

	return neiVtHd;
}

YsArray <YsShell::PolygonHandle>  YsShellExt_TrackingUtil::FloodFill(const YsShellExt &shl,YsShell::PolygonHandle plHd0,YsShellExt::Condition &cond)
{
	YsArray <YsShell::PolygonHandle> plHd;
	if(YSOK==cond.TestPolygon(shl,plHd0))
	{
		YsShellPolygonStore visited(shl.Conv());
		plHd.push_back(plHd0);
		visited.Add(plHd0);
		for(YSSIZE_T idx=0; idx<plHd.size(); ++idx)
		{
			auto plVtHd=shl.GetPolygonVertex(plHd[idx]);
			for(YSSIZE_T edIdx=0; edIdx<plVtHd.size(); ++edIdx)
			{
				if(YSOK==cond.TestEdge(shl,plVtHd[edIdx],plVtHd.GetCyclic(edIdx+1)))
				{
					auto neiPlHd=shl.GetNeighborPolygon(plHd[idx],edIdx);
					if(nullptr!=neiPlHd &&
					   YSTRUE!=visited.IsIncluded(neiPlHd) &&
					   YSOK==cond.TestPolygon(shl,neiPlHd))
					{
						visited.Add(neiPlHd);
						plHd.push_back(neiPlHd);
					}
				}
			}
		}
	}
	return plHd;
}



YsShell::EdgeAndPos YsShellExt_FindNearestPointOnConstEdgeStartingFromEdgePiece(const YsShellExt &shl,YsShell::Edge startEdge,const YsVec3 &refPos)
{
	YsShell::EdgeAndPos reached;
	reached.edVtHd[0]=NULL;
	reached.edVtHd[1]=NULL;

	auto edCeHd=shl.FindConstEdgeFromEdgePiece(startEdge);
	if(0<edCeHd.GetN())
	{
		YsShell::Edge curEdge=startEdge,lastEdge;
		lastEdge[0]=NULL;
		lastEdge[1]=NULL;

		for(;;)
		{
			const YsVec3 edVtPos[2]={shl.GetVertexPosition(curEdge[0]),shl.GetVertexPosition(curEdge[1])};
			YsVec3 nearPos;

			if(YSOK==YsGetNearestPointOnLine3(nearPos,edVtPos[0],edVtPos[1],refPos))
			{
				if(edVtPos[0]==nearPos)
				{
					reached.edVtHd[0]=curEdge[0];
					reached.edVtHd[1]=curEdge[0];
					reached.pos=edVtPos[0];
					break;
				}
				else if(edVtPos[1]==nearPos)
				{
					reached.edVtHd[0]=curEdge[1];
					reached.edVtHd[1]=curEdge[1];
					reached.pos=edVtPos[1];
					break;
				}
				else if(YSTRUE==YsCheckInBetween3(nearPos,edVtPos))
				{
					reached.edVtHd[0]=curEdge[0];
					reached.edVtHd[1]=curEdge[1];
					reached.pos=nearPos;
					break;
				}
				else
				{
					int closer=-1;
					if((nearPos-edVtPos[0]).GetSquareLength()<(nearPos-edVtPos[1]).GetSquareLength())
					{
						closer=0;
					}
					else
					{
						closer=1;
					}

					YsShell::Edge nextEdge;
					nextEdge[0]=NULL;
					nextEdge[1]=NULL;

					auto tipVtHd=curEdge[closer];
					auto connVtHd=shl.GetConnectedVertex(tipVtHd);
					for(auto nextVtHd : connVtHd)
					{
						if(nextVtHd!=curEdge[1-closer])
						{
							auto nextEdCeHd=shl.FindConstEdgeFromEdgePiece(tipVtHd,nextVtHd);
							if(YSTRUE==edCeHd.HasCommonItem(nextEdCeHd))
							{
								if(nextEdge[0]!=NULL) // Means it is the second possible edge.
								{
									nextEdge[0]=NULL;
									nextEdge[1]=NULL;
									break;
								}
								else
								{
									nextEdge[0]=tipVtHd;
									nextEdge[1]=nextVtHd;
								}
							}
						}
					}

					if(NULL==nextEdge[0])
					{
						reached[0]=tipVtHd;
						reached[1]=tipVtHd;
						reached.pos=edVtPos[closer];
						break;
					}
					else
					{
						if(lastEdge==nextEdge)
						{
							reached[0]=tipVtHd;
							reached[1]=tipVtHd;
							reached.pos=edVtPos[closer];
							break;
						}
						lastEdge=curEdge;
						curEdge=nextEdge;
					}
				}
			}
			else
			{
				return reached;
			}
		}
	}

	return reached;
}

YsShell::VertexHandle YsShellExt_FindNearestBoundaryVertex(const YsShellExt &shl,const YsShell::VertexHandle vtHd0)
{
	YsShellVertexStore visited(shl.Conv());
	visited.Add(vtHd0);
	YsArray <YsShell::VertexHandle> todo;
	todo.Add(vtHd0);
	for(YSSIZE_T idx=0; idx<todo.GetN(); ++idx)
	{
		if(YSTRUE==YsShellExt_TopologyUtil::IsVertexUsedBySingleUseEdge(shl,todo[idx]))
		{
			return todo[idx];
		}
		for(auto connVtHd : shl.GetConnectedVertex(todo[idx]))
		{
			if(YSTRUE!=visited.IsIncluded(connVtHd))
			{
				visited.Add(connVtHd);
				todo.Add(connVtHd);
			}
		}
	}
	return nullptr;
}

