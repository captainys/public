/* ////////////////////////////////////////////////////////////

File Name: ysshellext_offsetutil.cpp
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

#include "ysshellext_offsetutil.h"
#include "ysshellext_trackingutil.h"

#include "ysshellext_boundaryutil.h"

void YsShellExt_OffsetUtil2d::VertexOffset::Initialize(void)
{
	fromVtHd=NULL;
	toVtHd=NULL;
	limitingPlHd=NULL;
	maxDist=0.0;
	cornerIdx=-1;

	hangingPlArray.CleanUp();;
}

////////////////////////////////////////////////////////////

void YsShellExt_OffsetUtil2d::CleanUp(void)
{
	srcPlHdArray.CleanUp();

	newVtxArray.CleanUp();
	vtKeyToNewVtxIndex.CleanUp();
	boundaryEdgeArray.CleanUp();
	newPlgArray.CleanUp();
}

YSRESULT YsShellExt_OffsetUtil2d::SetUpForEdgeConnectedPolygonGroup(const YsShell &shl,YSSIZE_T nPl,const YsShellPolygonHandle plHdArray[])
{
	YsShellExt_BoundaryInfo boundary;
	boundary.MakeInfo(shl,nPl,plHdArray);
	if(YSOK!=boundary.CacheContour(shl) || 0==boundary.GetNumContour())
	{
		return YSERR;
	}

	YsArray <YsArray <YsShellVertexHandle> > allContour=boundary.GetContourAll();
	return SetUpForEdgeConnectedPolygonGroup(shl,allContour,nPl,plHdArray);
}

YSRESULT YsShellExt_OffsetUtil2d::SetUpForEdgeConnectedPolygonGroup(const YsShell &shl,const YsArray <YsArray <YsShellVertexHandle> > &boundaryInfo,YSSIZE_T nSrcPl,const YsShellPolygonHandle srcPlHd[])
{
	srcPlHdArray.Set(nSrcPl,srcPlHd);

	const YsShellSearchTable *search=shl.GetSearchTable();
	if(NULL==search)
	{
		YsPrintf("%s\n",__FUNCTION__);
		YsPrintf("  This function requires a search table.\n");
		return YSERR;
	}

	for(auto contour : boundaryInfo)
	{
		YsArray <YsShellVertexHandle> boundaryContour=contour;

		YsShellVertexStore contourVtxStore(shl);
		for(auto vtHd : contour)
		{
			contourVtxStore.AddVertex(vtHd);
		}

		if(0<boundaryContour.GetN() && boundaryContour[0]==boundaryContour.Last())
		{
			printf("%s %d\n",__FUNCTION__,__LINE__);
			printf("  Closed loop.\n");
			boundaryContour.DeleteLast();
		}

		if(3<=boundaryContour.GetN())
		{
			YsShellPolygonStore plHdSet(shl);
			plHdSet.AddPolygon(nSrcPl,srcPlHd);

			for(YSSIZE_T vtIdx=0; vtIdx<boundaryContour.GetN(); ++vtIdx)
			{
				const YsShellVertexHandle vtHd[3]=
				{
					boundaryContour.GetCyclic(vtIdx-1),
					boundaryContour[vtIdx],
					boundaryContour.GetCyclic(vtIdx+1)
				};
				AddNewVertex(shl,vtHd);
			}

			for(YSSIZE_T vtIdx=0; vtIdx<boundaryContour.GetN(); ++vtIdx)
			{
				// Note: Different for closed- and open- vertex sequence.
				const YsShellVertexHandle edVtHd[2]=
				{
					boundaryContour[vtIdx],
					boundaryContour.GetCyclic(vtIdx+1)
				};
				AddBoundaryEdgePiece(shl,plHdSet,edVtHd);
			}

			for(auto vtHd : boundaryContour)
			{
				if(YSOK!=SetVertexLimitAndActualDirection(shl,plHdSet,vtHd,contourVtxStore))
				{
					return YSERR;
				}
			}
		}
	}

	return YSOK;
}

void YsShellExt_OffsetUtil2d::AddNewVertex(const YsShell &shl,const YsShellVertexHandle vtHd[3])
{
	const YsVec3 vtPos[3]=
	{
		shl.GetVertexPosition(vtHd[0]),
		shl.GetVertexPosition(vtHd[1]),
		shl.GetVertexPosition(vtHd[2])
	};

	const YsVec3 dir[2]=
	{
		YsUnitVector(vtPos[1]-vtPos[0]),
		YsUnitVector(vtPos[2]-vtPos[1])
	};

	const YSSIZE_T newVtxIndex=newVtxArray.GetN();
	newVtxArray.Increment();
	newVtxArray.Last().Initialize();
	newVtxArray.Last().fromVtHd=vtHd[1];
	newVtxArray.Last().fromPos=shl.GetVertexPosition(vtHd[1]);
	newVtxArray.Last().dir=YsUnitVector(dir[0]+dir[1]); // Tentative.
	vtKeyToNewVtxIndex.AddElement(shl.GetSearchKey(vtHd[1]),newVtxIndex);
}

YSRESULT YsShellExt_OffsetUtil2d::AddNewVertexAtTipOfOpenVertexSequence(const YsShell &shl,const YsShellPolygonStore &srcPlHdSet,YsShellVertexHandle footVtHd,YsShellVertexHandle neiFootVtHd)
{
	YsShellPolygonHandle limitingPlHd;
	YsShellVertexHandle hairVtHd=FindHairVertexAtTipOfOpenVertexSequence(limitingPlHd,shl,srcPlHdSet,footVtHd,neiFootVtHd);
	if(NULL!=hairVtHd)
	{
		const YSSIZE_T newVtxIndex=newVtxArray.GetN();
		newVtxArray.Increment();
		newVtxArray.Last().Initialize();
		newVtxArray.Last().fromVtHd=footVtHd;
		newVtxArray.Last().fromPos=shl.GetVertexPosition(footVtHd);
		newVtxArray.Last().maxDist=(shl.GetVertexPosition(hairVtHd)-shl.GetVertexPosition(footVtHd)).GetLength();
		newVtxArray.Last().dir=YsUnitVector(shl.GetVertexPosition(hairVtHd)-shl.GetVertexPosition(footVtHd));
		newVtxArray.Last().limitingPlHd=limitingPlHd;
		newVtxArray.Last().limitingEdVtHd[0]=hairVtHd;
		newVtxArray.Last().limitingEdVtHd[1]=hairVtHd;

		const YsShellSearchTable *search=shl.GetSearchTable();

		int nEndPl;
		const YsShellPolygonHandle *endPlHd;
		if(YSOK==search->FindPolygonListByEdge(nEndPl,endPlHd,shl,footVtHd,hairVtHd))
		{
			for(YSSIZE_T plIdx=0; plIdx<nEndPl; ++plIdx)
			{
				if(YSTRUE==srcPlHdSet.IsIncluded(endPlHd[plIdx]))
				{
					continue;
				}
				YsArray <YsShellVertexHandle,4> plVtHd;
				shl.GetPolygon(plVtHd,endPlHd[plIdx]);
				for(YSSIZE_T vtIdx=0; vtIdx<plVtHd.GetN(); ++vtIdx)
				{
					if(plVtHd[vtIdx]==footVtHd)
					{
						if(hairVtHd==plVtHd.GetCyclic(vtIdx+1))
						{
							newVtxArray.Last().hangingPlArray.Increment();
							newVtxArray.Last().hangingPlArray.Last().plHd=endPlHd[plIdx];
							newVtxArray.Last().hangingPlArray.Last().hairIsOneThisSideOfFromVtHd=1;
						}
						else if(hairVtHd==plVtHd.GetCyclic(vtIdx-1))
						{
							newVtxArray.Last().hangingPlArray.Increment();
							newVtxArray.Last().hangingPlArray.Last().plHd=endPlHd[plIdx];
							newVtxArray.Last().hangingPlArray.Last().hairIsOneThisSideOfFromVtHd=-1;
						}
					}
				}
			}
		}

		vtKeyToNewVtxIndex.AddElement(shl.GetSearchKey(footVtHd),newVtxIndex);

		return YSOK;
	}
	return YSERR;
}

YsShellVertexHandle YsShellExt_OffsetUtil2d::FindHairVertexAtTipOfOpenVertexSequence(
    YsShellPolygonHandle &srcPlHdOfHairEdge,
    const YsShell &shl,const YsShellPolygonStore &srcPlHdSet,YsShellVertexHandle footVtHd,YsShellVertexHandle neiFootVtHd)
{
	const YsShellSearchTable *search=shl.GetSearchTable();

	YsArray <YsShellVertexHandle,16> connVtHdArray;
	search->GetConnectedVertexList(connVtHdArray,shl,footVtHd);

	int nCandidate=0;
	YsShellVertexHandle hairVtHd=NULL;
	for(auto connVtHd : connVtHdArray)
	{
		int nEdPl;
		const YsShellPolygonHandle *edPlHd;
		if(connVtHd!=neiFootVtHd && YSOK==search->FindPolygonListByEdge(nEdPl,edPlHd,shl,footVtHd,connVtHd))
		{
			int nSrcPl=0,nNonSrcPl=0;
			YsShellPolygonHandle fromThisPlHd=NULL;
			for(YSSIZE_T plIdx=0; plIdx<nEdPl; ++plIdx)
			{
				if(YSTRUE==srcPlHdSet.IsIncluded(edPlHd[plIdx]))
				{
					++nSrcPl;
					fromThisPlHd=edPlHd[plIdx];
				}
				else
				{
					++nNonSrcPl;
				}
			}
			if(1==nSrcPl)
			{
				++nCandidate;
				hairVtHd=connVtHd;
				srcPlHdOfHairEdge=fromThisPlHd;
			}
		}
	}

	if(1==nCandidate)
	{
printf("%s %d\n",__FUNCTION__,__LINE__);
		return hairVtHd;
	}

printf("%s %d\n",__FUNCTION__,__LINE__);
	return NULL;
}

void YsShellExt_OffsetUtil2d::AddBoundaryEdgePiece(
    const YsShell &shl,const YsShellPolygonStore &srcPlHdSet,const YsShellVertexHandle edVtHd[2])
{
	const YsShellSearchTable *search=shl.GetSearchTable();

	int nPl;
	const YsShellPolygonHandle *edPlHd;

	YSSIZE_T newVtxIdx[2];

	if(YSOK==search->FindPolygonListByEdge(nPl,edPlHd,shl,edVtHd[0],edVtHd[1]) &&
	   YSOK==vtKeyToNewVtxIndex.FindElement(newVtxIdx[0],shl.GetSearchKey(edVtHd[0])) &&
	   YSOK==vtKeyToNewVtxIndex.FindElement(newVtxIdx[1],shl.GetSearchKey(edVtHd[1])))
	{
		for(YSSIZE_T plIdx=0; plIdx<nPl; ++plIdx)
		{
			if(YSTRUE==srcPlHdSet.IsIncluded(edPlHd[plIdx]))
			{
				boundaryEdgeArray.Increment();
				boundaryEdgeArray.Last().newVtxIdx[0]=newVtxIdx[0];
				boundaryEdgeArray.Last().newVtxIdx[1]=newVtxIdx[1];
				boundaryEdgeArray.Last().plHd=edPlHd[plIdx];
			}
		}
	}
}

YSRESULT YsShellExt_OffsetUtil2d::SetVertexLimitAndActualDirection(const YsShell &shl,const YsShellPolygonStore &srcPlHdSet,YsShellVertexHandle vtHd,const YsShellVertexStore &contourVtxStore)
{
	const YsShellSearchTable *search=shl.GetSearchTable();

	YSSIZE_T newVtxIndex;
	vtKeyToNewVtxIndex.FindElement(newVtxIndex,shl.GetSearchKey(vtHd));

	// Memo: newVtxArray[newVtxIndex].dir is tentatively a tangential direction until actual direction is calculated here.
	const YsPlane cutPln(shl.GetVertexPosition(vtHd),newVtxArray[newVtxIndex].dir);

	int nPl;
	const YsShellPolygonHandle *vtPlHd;
	search->FindPolygonListByVertex(nPl,vtPlHd,shl,vtHd);

	YsVec3 actualDir=YsOrigin();

	const YsArray <YsShellPolygonHandle> vtPlHdArray(nPl,vtPlHd);
	for(auto plHd : vtPlHdArray)
	{
		if(YSTRUE!=srcPlHdSet.IsIncluded(plHd))
		{
			continue;
		}

		YsArray <YsShellVertexHandle,4> plVtHd;
		YsArray <YsVec3,4> plVtPos;
		shl.GetPolygon(plVtHd,plHd);
		shl.GetPolygon(plVtPos,plHd);

		YSSIZE_T srcVtIdx;
		YsIsIncluded(srcVtIdx,plVtHd,vtHd);

		const YsVec3 plNom=YsGetAverageNormalVector(plVtPos);
		YsMatrix4x4 toXY;
		toXY.MakeToXY(plVtPos[0],plNom);

		YsArray <YsVec2,4> prjPlVtPos(plVtPos.GetN(),nullptr);
		for(YSSIZE_T idx : plVtPos.AllIndex())
		{
			prjPlVtPos[idx]=(toXY*plVtPos[idx]).xy();
		}

		for(YSSIZE_T vtIdx=0; vtIdx<plVtPos.GetN(); ++vtIdx)
		{
			const YsShellVertexHandle edVtHd[2]=
			{
				plVtHd[vtIdx],
				plVtHd.GetCyclic(vtIdx+1)
			};

			const YsVec3 edVtPos[2]=
			{
				plVtPos[vtIdx],
				plVtPos.GetCyclic(vtIdx+1)
			};

			const int side[2]=
			{
				cutPln.GetSideOfPlane(edVtPos[0]),
				cutPln.GetSideOfPlane(edVtPos[1]),
			};

			if(plVtHd[vtIdx]!=vtHd && 0==side[0])
			{
				const YsVec3 dVec=plVtPos[vtIdx]-shl.GetVertexPosition(vtHd);
				const double d=dVec.GetLength();

				YsVec3 prjVec;
				toXY.Mul(prjVec,dVec,0.0);

				if((srcVtIdx==(vtIdx+1)%plVtPos.GetN() || vtIdx==(srcVtIdx+1)%plVtPos.GetN() || YSTRUE==YsVectorPointingInside(prjPlVtPos.GetN(),prjPlVtPos,srcVtIdx,prjVec.xy())) &&
				  (NULL==newVtxArray[newVtxIndex].limitingPlHd || d<newVtxArray[newVtxIndex].maxDist))
				{
					newVtxArray[newVtxIndex].limitingPlHd=plHd;
					newVtxArray[newVtxIndex].limitingEdVtHd[0]=plVtHd[vtIdx];
					newVtxArray[newVtxIndex].limitingEdVtHd[1]=plVtHd[vtIdx];
					newVtxArray[newVtxIndex].maxDist=d;
					actualDir=dVec/d;
				}
			}
			else if(plVtHd[vtIdx]!=vtHd && 
			        plVtHd.GetCyclic(vtIdx+1)!=vtHd &&
			        side[0]*side[1]<0)
			{
				YsVec3 itsc;
				if(YSOK==cutPln.GetIntersection(itsc,edVtPos[0],edVtPos[1]-edVtPos[0]))
				{
					const YsVec3 dVec=itsc-shl.GetVertexPosition(vtHd);
					const double d=dVec.GetLength();
					const YsVec3 unitDVec=dVec/d;

					YsVec3 prjVec;
					toXY.Mul(prjVec,dVec,0.0);

					if(YSTRUE==YsVectorPointingInside(prjPlVtPos.GetN(),prjPlVtPos,srcVtIdx,prjVec.xy()) &&
					  (NULL==newVtxArray[newVtxIndex].limitingPlHd || d<newVtxArray[newVtxIndex].maxDist))
					{
						newVtxArray[newVtxIndex].limitingPlHd=plHd;
						newVtxArray[newVtxIndex].limitingEdVtHd[0]=plVtHd[vtIdx];
						newVtxArray[newVtxIndex].limitingEdVtHd[1]=plVtHd.GetCyclic(vtIdx+1);
						// See offset2d.jnt memo of 2014/10/17
						if(YSTRUE==contourVtxStore.IsIncluded(edVtHd[0]) || YSTRUE==contourVtxStore.IsIncluded(edVtHd[1]))
						{
							YsVec3 limitter;
							if(YSTRUE==contourVtxStore.IsIncluded(edVtHd[0]))
							{
								limitter=shl.GetVertexPosition(edVtHd[1]);
							}
							else
							{
								limitter=shl.GetVertexPosition(edVtHd[0]);
							}
							newVtxArray[newVtxIndex].maxDist=(limitter-shl.GetVertexPosition(vtHd))*unitDVec;
						}
						else
						{
							newVtxArray[newVtxIndex].maxDist=d;
						}
						actualDir=dVec/d;
					}
				}
			}
		}
	}

	if(NULL==newVtxArray[newVtxIndex].limitingPlHd || YsOrigin()==actualDir)
	{
		printf("%s %d\n",__FUNCTION__,__LINE__);
		printf(" VtKey=%d\n",shl.GetSearchKey(vtHd));
		return YSERR;
	}

	newVtxArray[newVtxIndex].dir=actualDir;

	return YSOK;
}

YSRESULT YsShellExt_OffsetUtil2d::SetUpForOutsideOfEdgeConnectedPolygonGroup(const YsShell &shl,YSSIZE_T nPl,const YsShellPolygonHandle plHdArray[])
{
	YsShellExt_BoundaryInfo boundary;
	boundary.MakeInfo(shl,nPl,plHdArray);
	if(YSOK!=boundary.CacheContour(shl) || 0==boundary.GetNumContour())
	{
		return YSERR;
	}

	const YsShellSearchTable *search=shl.GetSearchTable();
	if(NULL==search)
	{
		YsPrintf("%s\n",__FUNCTION__);
		YsPrintf("  This function requires a search table.\n");
		return YSERR;
	}

	YsShellPolygonStore visitedPlHd(shl);
	visitedPlHd.AddPolygon(nPl,plHdArray);

	YsArray <YsShellPolygonHandle> reverseSidePlHd;
	for(YSSIZE_T contourIdx=0; contourIdx<boundary.GetNumContour(); ++contourIdx)
	{
		YsArray <YsShellVertexHandle> boundaryContour;
		boundary.GetContour(boundaryContour,contourIdx);

		if(3<=boundaryContour.GetN())
		{
			for(auto vtHd : boundaryContour)
			{
				int nVtPl;
				const YsShellPolygonHandle *vtPlHd;
				if(YSOK==search->FindPolygonListByVertex(nVtPl,vtPlHd,shl,vtHd))
				{
					for(YSSIZE_T plIdx=0; plIdx<nVtPl; ++plIdx)
					{
						if(YSTRUE!=visitedPlHd.IsIncluded(vtPlHd[plIdx]))
						{
							reverseSidePlHd.Append(vtPlHd[plIdx]);
							visitedPlHd.AddPolygon(vtPlHd[plIdx]);
						}
					}
				}
			}
		}
	}


	YsArray <YsArray <YsShellVertexHandle> > allContour=boundary.GetContourAll();
	return SetUpForEdgeConnectedPolygonGroup(shl,allContour,reverseSidePlHd.GetN(),reverseSidePlHd);
}

YSRESULT YsShellExt_OffsetUtil2d::SetUpForOneSideOfVertexSequence(
    const YsShellExt &shl,
    const YsArray <YsArray <YsShellVertexHandle> > &allContour,
    const YsArray <YsShellPolygonHandle> &seedPlHdArray)
{
	CleanUp();
	if(0<seedPlHdArray.GetN())
	{
		YsArray <YsShellPolygonHandle> allPlHdArray;
		for(auto contour : allContour)
		{
			YsArray <YsShellPolygonHandle> plHdArray=YsShellExt_TrackingUtil::TrackPolygonOneSideOfVertexSequence(shl,contour.GetN(),contour,seedPlHdArray.GetN(),seedPlHdArray);
			allPlHdArray.Append(plHdArray);

			// The following line crashes.  Why?
			// allPlHdArray.Append(YsShellExt_TrackingUtil::TrackPolygonOneSideOfVertexSequence(shl,contour.GetN(),contour,seedPlHdArray.GetN(),seedPlHdArray));
		}

		srcPlHdArray=allPlHdArray;

		if(0<seedPlHdArray.GetN())
		{
			return SetUpForOneSideOfOpenVertexSequenceWithFullSetOfPolygon(shl.Conv(),allContour,allPlHdArray);
		}
	}
	return YSERR;
}

YSRESULT YsShellExt_OffsetUtil2d::SetUpForOneSideOfOpenVertexSequenceWithFullSetOfPolygon(
    const YsShell &shl,
    const YsArray <YsArray <YsShellVertexHandle> > &allContour,
    const YsArray <YsShellPolygonHandle> &seedPlHdArray)
{
	const YsShellSearchTable *search=shl.GetSearchTable();
	if(NULL==search)
	{
		YsPrintf("%s\n",__FUNCTION__);
		YsPrintf("  This function requires a search table.\n");
		return YSERR;
	}

	YsShellPolygonStore plHdSet(shl);
	plHdSet.AddPolygon(seedPlHdArray);

	for(auto contour : allContour)
	{
		YSBOOL isLoop=YSFALSE;
		if(contour[0]==contour.Last())
		{
			isLoop=YSTRUE;
			contour.DeleteLast();
			if(2>=contour.GetN())
			{
				return YSERR;
			}

		}

		YsShellVertexStore contourVtxStore(shl);
		for(auto vtHd : contour)
		{
			contourVtxStore.AddVertex(vtHd);
		}


		if(2<=contour.GetN())
		{
			YsShellVertexHandle tipVtHd[2]={NULL,NULL};

			// Separate for open and closed vertex sequence.
			if(YSTRUE==isLoop)
			{
				for(YSSIZE_T vtIdx=0; vtIdx<contour.GetN(); ++vtIdx)
				{
					const YsShellVertexHandle vtHd[3]=
					{
						contour.GetCyclic(vtIdx-1),
						contour[vtIdx],
						contour.GetCyclic(vtIdx+1)
					};
					AddNewVertex(shl,vtHd);
				}
			}
			else
			{
				// Exception for the first and the last vertices.  See offset.jnt

				tipVtHd[0]=contour[0];
				tipVtHd[1]=contour.Last();

				if(YSOK!=AddNewVertexAtTipOfOpenVertexSequence(shl,plHdSet,contour[0],contour[1]))
				{
					return YSERR;
				}

				for(YSSIZE_T vtIdx=1; vtIdx<contour.GetN()-1; ++vtIdx)
				{
					const YsShellVertexHandle vtHd[3]=
					{
						contour[vtIdx-1],
						contour[vtIdx],
						contour[vtIdx+1]
					};
					AddNewVertex(shl,vtHd);
				}

				if(YSOK!=AddNewVertexAtTipOfOpenVertexSequence(shl,plHdSet,contour.GetCyclic(-1),contour.GetCyclic(-2)))
				{
					return YSERR;
				}
			}

			// Must be done after AddNewVertex calls.
			const YSSIZE_T lastIdx=(YSTRUE==isLoop ? contour.GetN()-1 : contour.GetN()-2);
			for(YSSIZE_T vtIdx=0; vtIdx<=lastIdx; ++vtIdx)
			{
				const YsShellVertexHandle edVtHd[2]=
				{
					contour[vtIdx],
					contour.GetCyclic(vtIdx+1)
				};
				AddBoundaryEdgePiece(shl,plHdSet,edVtHd);
			}

			// Common for open and closed vertex sequence after duplicate vertex at the top and the end have been removed and tipVtHd[] are set.
			for(auto vtHd : contour)
			{
				if(vtHd!=tipVtHd[0] && vtHd!=tipVtHd[1])
				{
					if(YSOK!=SetVertexLimitAndActualDirection(shl,plHdSet,vtHd,contourVtxStore))
					{
						return YSERR;
					}
				}
			}
		}
	}
	return YSOK;
}

void YsShellExt_OffsetUtil2d::MakeBoundaryEdgeStore(YsTwoKeyStore &edgeStore,const YsShell &shl) const
{
	for(auto edge : boundaryEdgeArray)
	{
		const YsShellVertexHandle edVtHd[2]=
		{
			newVtxArray[edge.newVtxIdx[0]].fromVtHd,
			newVtxArray[edge.newVtxIdx[1]].fromVtHd
		};
		edgeStore.AddKey(shl.GetSearchKey(edVtHd[0]),shl.GetSearchKey(edVtHd[1]));
	}
}

