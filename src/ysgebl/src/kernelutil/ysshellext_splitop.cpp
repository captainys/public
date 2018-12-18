/* ////////////////////////////////////////////////////////////

File Name: ysshellext_splitop.cpp
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

#include "ysshellext_splitop.h"
#include "ysshellext_trackingutil.h"

YsShell_SplitInfo::YsShell_SplitInfo()
{
	CleanUp();
}

void YsShell_SplitInfo::CleanUp(void)
{
	edgeSplit.CleanUp();
	newPlg.CleanUp();
	newConstEdge.CleanUp();
}

YSRESULT YsShell_SplitInfo::SetAndSplitPolygon(PolygonSplit &plg,const YsShellExt &shl,const YsShellVertexStore &cutPoint,YsShellPolygonHandle plHd)
{
	YsArray <YsShellVertexHandle,16> plVtHd;
	YsArray <YsVec3,16> plVtPos;
	
	shl.GetVertexListOfPolygon(plVtHd,plHd);
	shl.GetVertexListOfPolygon(plVtPos,plHd);

	YsMatrix4x4 prjMat;
	if(YSOK==YsGetPolygonProjectionMatrix(prjMat,plVtPos.GetN(),plVtPos))
	{
		plg.plHd=plHd;

		plg.plVtHdArray.CleanUp();
		plg.plVtHdArray.Increment();
		plg.plVtHdArray[0].Set(plVtHd.GetN(),NULL);

		plg.prjVtPosArray.CleanUp();
		plg.prjVtPosArray.Increment();
		plg.prjVtPosArray[0].Set(plVtHd.GetN(),NULL);

		for(YSSIZE_T vtIdx=0; vtIdx<plVtHd.GetN(); ++vtIdx)
		{
			YsVec3 prj;
			prjMat.Mul(prj,plVtPos[vtIdx],1.0);
			plg.plVtHdArray[0][vtIdx]=plVtHd[vtIdx];
			plg.prjVtPosArray[0][vtIdx].Set(prj.x(),prj.y());
		}

		SplitPolygon(plg,cutPoint);
		return YSOK;
	}
	return YSERR;
}

void YsShell_SplitInfo::SplitPolygon(PolygonSplit &plg,const YsShellVertexStore &cutPoint)
{
	for(YSSIZE_T todoIdx=0; todoIdx<plg.plVtHdArray.GetN(); )
	{
		YSSIZE_T splitIdx[2]={-1,-1};
		for(YSSIZE_T i0=0; i0<plg.plVtHdArray[todoIdx].GetN(); ++i0)
		{
			if(YSTRUE==cutPoint.IsIncluded(plg.plVtHdArray[todoIdx][i0]))
			{
				for(YSSIZE_T i1=i0+1; i1<plg.plVtHdArray[todoIdx].GetN(); ++i1)
				{
					if(YSTRUE==cutPoint.IsIncluded(plg.plVtHdArray[todoIdx][i1]))
					{
						if(i0+2<=i1 && YSTRUE==YsCheckSeparatability2((int)plg.prjVtPosArray[todoIdx].GetN(),plg.prjVtPosArray[todoIdx],(int)i0,(int)i1))
						{
							splitIdx[0]=i0;
							splitIdx[1]=i1;
							goto BAILOUT;
						}
						/* I thought the following else statement was a good idea to save computation, but it was not.
						   See debug/data/20131117-split-polygon.srf
						   In this case, vertices 0,1,2, and 6 of the concave polygon are on the cutting plane (rectangle).
						   Only separatable pair is vertex[2]-vertex[6].  If I add the following else-statement block,
						   this loop will check 0-1, 1-2, and 2-6, but will never check 0-6.
						else
						{
							i0=i1-1;
							break;
						} */
					}
				}
			}
		}
	BAILOUT:
		if(0<=splitIdx[0] && 0<=splitIdx[1])
		{
			plg.plVtHdArray.Increment();
			plg.prjVtPosArray.Increment();

			if(YSOK!=YsShellExt_SplitLoop(plg.plVtHdArray[todoIdx],plg.plVtHdArray.GetEnd(),splitIdx[0],splitIdx[1]) ||
			   YSOK!=YsShellExt_SplitLoop(plg.prjVtPosArray[todoIdx],plg.prjVtPosArray.GetEnd(),splitIdx[0],splitIdx[1]))
			{
				plg.plVtHdArray.DeleteLast();
				plg.prjVtPosArray.DeleteLast();
				++todoIdx;
			}
		}
		else
		{
			++todoIdx;
		}
	}
}

YSRESULT YsShell_SplitInfo::SetAndSplitConstEdge(ConstEdgeSplit &ce,const YsShellExt &shl,const YsShellVertexStore &cutPoint,YsShellExt::ConstEdgeHandle ceHd)
{
	YSBOOL isLoop;
	YsArray <YsShellVertexHandle,16> ceVtHd;
	shl.GetConstEdge(ceVtHd,isLoop,ceHd);

	ce.ceHd=ceHd;

	if(YSTRUE==isLoop)
	{
		YSSIZE_T vtIdx0=-1;
		for(YSSIZE_T vtIdx=0; vtIdx<ceVtHd.GetN(); ++vtIdx)
		{
			if(YSTRUE==cutPoint.IsIncluded(ceVtHd[vtIdx]) &&
			  (YSTRUE!=cutPoint.IsIncluded(ceVtHd.GetCyclic(vtIdx-1)) ||
			   YSTRUE!=cutPoint.IsIncluded(ceVtHd.GetCyclic(vtIdx+1))))
			{
				vtIdx0=vtIdx;
				break;
			}
		}
		if(0<=vtIdx0)
		{
			if(0!=vtIdx0 || ceVtHd.GetN()-1==vtIdx0)
			{
				const YsArray <YsShellVertexHandle,16> swp(vtIdx0+1,ceVtHd);
				ceVtHd.Delete(0,vtIdx0);
				ceVtHd.Append(swp);
			}
		}
		else
		{
			return YSERR;
		}
	}

	const YSBOOL wasLoop=isLoop;

	ce.ceHd=ceHd;
	ce.ceVtHdArray.CleanUp();
	ce.ceVtHdArray.Increment();
	ce.ceVtHdArray[0]=ceVtHd;
	SplitConstEdge(ce,cutPoint);

	if(YSTRUE==wasLoop || 1<ce.ceVtHdArray.GetN())
	{
		return YSOK;
	}
	return YSERR;
}

void YsShell_SplitInfo::SplitConstEdge(ConstEdgeSplit &ce,const YsShellVertexStore &cutPoint)
{
	YsArray <YSBOOL,16> isCutPointRaw,isCutPoint;

	isCutPointRaw.Set(ce.ceVtHdArray[0].GetN(),NULL);
	isCutPoint.Set(ce.ceVtHdArray[0].GetN(),NULL);
	for(YSSIZE_T vtIdx=0; vtIdx<ce.ceVtHdArray[0].GetN(); ++vtIdx)
	{
		isCutPointRaw[vtIdx]=cutPoint.IsIncluded(ce.ceVtHdArray[0][vtIdx]);
		isCutPoint[vtIdx]=isCutPointRaw[vtIdx];
	}

	for(YSSIZE_T vtIdx=1; vtIdx<ce.ceVtHdArray[0].GetN()-1; ++vtIdx)
	{
		if(YSTRUE==isCutPointRaw[vtIdx-1] &&
		   YSTRUE==isCutPointRaw[vtIdx] &&
		   YSTRUE==isCutPointRaw[vtIdx+1])
		{
			isCutPoint[vtIdx]=YSFALSE;
		}
	}

	for(YSSIZE_T vtIdx=ce.ceVtHdArray[0].GetN()-2; 1<=vtIdx; --vtIdx)
	{
		if(YSTRUE==isCutPoint[vtIdx])
		{
			ce.ceVtHdArray.Increment();
			YsShellExt_SplitOpenChain(ce.ceVtHdArray[0],ce.ceVtHdArray.GetEnd(),vtIdx);
			--vtIdx;
		}
	}
}

YSRESULT YsShell_SplitInfo::SplitFaceGroup(FaceGroupSplit &fgSplit,const YsShellExt &shl,YsShellExt::FaceGroupHandle fgHd,const YsShellPolygonStore &newGrouping)
{
	auto fgPlHd=shl.GetFaceGroup(fgHd);

	YsShellPolygonStore overlapping((const YsShell &)shl),nonOverlapping((const YsShell &)shl);

	for(auto plHd : fgPlHd)
	{
		if(YSTRUE!=overlapping.IsIncluded(plHd) && YSTRUE!=nonOverlapping.IsIncluded(plHd))
		{
			if(YSTRUE==newGrouping.IsIncluded(plHd))
			{
				overlapping.AddPolygon(plHd);
			}
			else
			{
				nonOverlapping.AddPolygon(plHd);
			}
		}
	}

	fgSplit.CleanUp();

	YsShellPolygonStore visited((const YsShell &)shl);

	for(auto plHd : fgPlHd)
	{
		if(YSTRUE!=visited.IsIncluded(plHd))
		{
			YsShellExt_TrackingUtil::SearchCondition cond;
			cond.onlyInThisFgHd=fgHd;
			if(YSTRUE==overlapping.IsIncluded(plHd))
			{
				cond.withinThesePlHd=&overlapping;
			}
			else
			{
				cond.withinThesePlHd=&nonOverlapping;
			}

			auto plgGrp=YsShellExt_TrackingUtil::MakeEdgeConnectedPolygonGroup(shl,plHd,&cond);
			if(0<plgGrp.GetN())
			{
				fgSplit.fgPlHdArray.Increment();
				fgSplit.fgPlHdArray.Last().MoveFrom(plgGrp);

				visited.AddPolygon(plgGrp);
			}
		}
	}

	if(1<fgSplit.fgPlHdArray.GetN())
	{
		fgSplit.fgHd=fgHd;
		return YSOK;
	}
	return YSERR;
}

////////////////////////////////////////////////////////////

void YsShell_CutByPlane::CleanUp(void)
{
	YsShell_SplitInfo::CleanUp();
	plg.CleanUp();
	prjMat.LoadIdentity();
	prjPlg.CleanUp();
	plHdToSplit.CleanUp();
	ceHdToSplit.CleanUp();
}

void YsShell_CutByPlane::SetPlane(const YsPlane &pln)
{
	this->pln=pln;
}

YSRESULT YsShell_CutByPlane::SetPolygon(YSSIZE_T np,const YsVec3 p[])
{
	plg.Set(np,p);
	if(YSOK==YsGetPolygonProjectionMatrix(prjMat,np,p))
	{
		prjPlg.Set(np,NULL);
		for(YSSIZE_T idx=0; idx<np; ++idx)
		{
			YsVec3 prj;
			prjMat.Mul(prj,p[idx],1.0);
			prjPlg[idx].Set(prj.x(),prj.y());
		}
		return YSOK;
	}
	return YSERR;
}

void YsShell_CutByPlane::FindPolygonToSplit(const YsShellExt &shl)
{
	plHdToSplit.CleanUp();

	YsArray <YsVec3,16> plVtPos;
	YsShellPolygonHandle plHd=NULL;
	while(YSOK==shl.MoveToNextPolygon(plHd))
	{
		shl.GetVertexListOfPolygon(plVtPos,plHd);

		YSBOOL positive=YSFALSE,negative=YSFALSE;
		for(YSSIZE_T idx=0; idx<plVtPos.GetN() && (YSTRUE!=positive || YSTRUE!=negative); ++idx)
		{
			const int sideOfPlane=pln.GetSideOfPlane(plVtPos[idx]);
			if(0>sideOfPlane)
			{
				negative=YSTRUE;
			}
			else if(0<sideOfPlane)
			{
				positive=YSTRUE;
			}
		}

		if(YSTRUE==positive && YSTRUE==negative)
		{
			plHdToSplit.Append(plHd);
		}
	}
}

void YsShell_CutByPlane::FindConstEdgeToSplit(const YsShellExt &shl)
{
	YSBOOL isLoop;
	YsArray <YsShellVertexHandle,16> ceVtHd;

	YsShellExt::ConstEdgeHandle ceHd=NULL;
	while(YSOK==shl.MoveToNextConstEdge(ceHd))
	{
		shl.GetConstEdge(ceVtHd,isLoop,ceHd);

		YSBOOL positive=YSFALSE,negative=YSFALSE;
		for(YSSIZE_T idx=0; idx<ceVtHd.GetN() && (YSTRUE!=positive || YSTRUE!=negative); ++idx)
		{
			YsVec3 vtPos;
			shl.GetVertexPosition(vtPos,ceVtHd[idx]);

			const int sideOfPlane=pln.GetSideOfPlane(vtPos);
			if(0>sideOfPlane)
			{
				negative=YSTRUE;
			}
			else if(0<sideOfPlane)
			{
				positive=YSTRUE;
			}
		}

		if(YSTRUE==positive && YSTRUE==negative)
		{
			ceHdToSplit.Append(ceHd);
		}
	}
}

void YsShell_CutByPlane::MakeEdgeSplitInfo(const YsShellExt &shl)
{
	YsShellEdgeStore visited((const YsShell &)shl);

	YsArray <YsShellVertexHandle,16> plVtHd;
	for(YSSIZE_T plIdx=0; plIdx<plHdToSplit.GetN(); ++plIdx)
	{
		shl.GetVertexListOfPolygon(plVtHd,plHdToSplit[plIdx]);

		YsShellVertexHandle vtHd0=plVtHd[0];
		plVtHd.Append(vtHd0);

		for(YSSIZE_T vtIdx=0; vtIdx<plVtHd.GetN()-1; ++vtIdx)
		{
			const YsShellVertexHandle edVtHd[2]=
			{
				plVtHd[vtIdx],
				plVtHd[vtIdx+1]
			};

			MakeEdgeSplitInfo_PerEdge(shl,visited,edVtHd);
		}
	}

	YsArray <YsShellVertexHandle,16> ceVtHd;
	for(YSSIZE_T ceIdx=0; ceIdx<ceHdToSplit.GetN(); ++ceIdx)
	{
		YSBOOL isLoop;
		shl.GetConstEdge(ceVtHd,isLoop,ceHdToSplit[ceIdx]);

		if(YSTRUE==isLoop)
		{
			const YsShellVertexHandle vtHd0=ceVtHd[0];
			ceVtHd.Append(vtHd0);
		}

		for(YSSIZE_T vtIdx=0; vtIdx<ceVtHd.GetN()-1; ++vtIdx)
		{
			const YsShellVertexHandle edVtHd[2]=
			{
				ceVtHd[vtIdx],
				ceVtHd[vtIdx+1]
			};
			MakeEdgeSplitInfo_PerEdge(shl,visited,edVtHd);
		}
	}
}

void YsShell_CutByPlane::MakeEdgeSplitInfo_PerEdge(const YsShellExt &shl,YsShellEdgeStore &visited,const YsShellVertexHandle edVtHd[2])
{
	if(YSTRUE!=visited.IsIncluded(edVtHd[0],edVtHd[1]))
	{
		visited.AddEdge(edVtHd[0],edVtHd[1]);

		YsVec3 edVtPos[2];
		shl.GetVertexPosition(edVtPos[0],edVtHd[0]);
		shl.GetVertexPosition(edVtPos[1],edVtHd[1]);

		const int side[2]=
		{
			pln.GetSideOfPlane(edVtPos[0]),
			pln.GetSideOfPlane(edVtPos[1])
		};

		if(0>side[0]*side[1])
		{
			YsVec3 itsc;
			if(YSOK==pln.GetPenetrationHighPrecision(itsc,edVtPos[0],edVtPos[1]))
			{
				if(3<=prjPlg.GetN())
				{
					YsVec3 prj3d;
					prjMat.Mul(prj3d,itsc,1.0);
					const YsVec2 prj2d(prj3d.x(),prj3d.y());

					const YSSIDE side=YsCheckInsidePolygon2(prj2d,prjPlg.GetN(),prjPlg);
					if(YSINSIDE!=side && YSBOUNDARY!=side)
					{
						goto BAILOUT;
					}
				}

				edgeSplit.Increment();
				edgeSplit.GetEnd().CleanUp();
				edgeSplit.GetEnd().edVtHd[0]=edVtHd[0];
				edgeSplit.GetEnd().edVtHd[1]=edVtHd[1];
				edgeSplit.GetEnd().pos=itsc;
				edgeSplit.GetEnd().createdVtHd=NULL;
			BAILOUT:
				;
			}
		}
	}
}

YSRESULT YsShell_CutByPlane::StartByPolygon(const YsShellExt &shl,YSSIZE_T np,const YsShellVertexHandle p[])
{
	YsArray <YsVec3,16> plVtPos(np,NULL);
	for(YSSIZE_T idx=0; idx<np; ++idx)
	{
		shl.GetVertexPosition(plVtPos[idx],p[idx]);
	}
	return StartByPolygon(shl,plVtPos);
}

YSRESULT YsShell_CutByPlane::StartByPolygon(const YsShellExt &shl,YSSIZE_T np,const YsVec3 p[])
{
	YsPlane pln;
	if(YSOK==pln.MakeBestFitPlane(np,p) && YSOK==SetPolygon(np,p))
	{
		SetPlane(pln);
		FindPolygonToSplit(shl);
		FindConstEdgeToSplit(shl);
		MakeEdgeSplitInfo(shl);
		return YSOK;
	}
	return YSERR;
}

YSRESULT YsShell_CutByPlane::MakeSplitInfoAfterEdgeVertexGeneration(const YsShellExt &shl)
{
	newPlg.CleanUp();
	newConstEdge.CleanUp();

	YsShellVertexStore cutPoint((const YsShell &)shl);
	for(YSSIZE_T idx=0; idx<edgeSplit.GetN(); ++idx)
	{
		cutPoint.AddVertex(edgeSplit[idx].createdVtHd);
	}

	YsArray <YsVec3,16> plVtPos;
	YsArray <YsShellVertexHandle,16> plVtHd;
	for(YSSIZE_T plIdx=0; plIdx<plHdToSplit.GetN(); ++plIdx)
	{
		shl.GetVertexListOfPolygon(plVtPos,plHdToSplit[plIdx]);
		shl.GetVertexListOfPolygon(plVtHd,plHdToSplit[plIdx]);

		for(YSSIZE_T vtIdx=0; vtIdx<plVtHd.GetN(); ++vtIdx)
		{
			if(YSTRUE!=cutPoint.IsIncluded(plVtHd[vtIdx]) && YSTRUE==pln.CheckOnPlane(plVtPos[vtIdx]))
			{
				if(3<=prjPlg.GetN())
				{
					YsVec3 prj3d;
					prjMat.Mul(prj3d,plVtPos[vtIdx],1.0);
					const YsVec2 prj2d(prj3d.x(),prj3d.y());
					const YSSIDE side=YsCheckInsidePolygon2(prj2d,prjPlg.GetN(),prjPlg);
					if(YSINSIDE!=side && YSBOUNDARY!=side)
					{
						continue;
					}
				}
				cutPoint.AddVertex(plVtHd[vtIdx]);
			}
		}
	}

	for(YSSIZE_T ceIdx=0; ceIdx<ceHdToSplit.GetN(); ++ceIdx)
	{
		YSBOOL isLoop;
		shl.GetConstEdge(plVtHd,isLoop,ceHdToSplit[ceIdx]);

		for(YSSIZE_T vtIdx=0; vtIdx<plVtHd.GetN(); ++vtIdx)
		{
			YsVec3 vtPos;
			shl.GetVertexPosition(vtPos,plVtHd[vtIdx]);
			if(YSTRUE!=cutPoint.IsIncluded(plVtHd[vtIdx]) && YSTRUE==pln.CheckOnPlane(vtPos))
			{
				if(3<=prjPlg.GetN())
				{
					YsVec3 prj3d;
					prjMat.Mul(prj3d,vtPos,1.0);
					const YsVec2 prj2d(prj3d.x(),prj3d.y());
					const YSSIDE side=YsCheckInsidePolygon2(prj2d,prjPlg.GetN(),prjPlg);
					if(YSINSIDE!=side && YSBOUNDARY!=side)
					{
						continue;
					}
				}
				cutPoint.AddVertex(plVtHd[vtIdx]);
			}
		}
	}



	YsArray <YsVec2,16> prjPlVtPos;
	YsArray <YSBOOL,16> isCutPoint;
	for(YSSIZE_T plIdx=0; plIdx<plHdToSplit.GetN(); ++plIdx)
	{
		shl.GetVertexListOfPolygon(plVtHd,plHdToSplit[plIdx]);

		int nCutPoint=0;
		isCutPoint.Set(plVtHd.GetN(),NULL);
		for(YSSIZE_T vtIdx=0; vtIdx<plVtHd.GetN(); ++vtIdx)
		{
			isCutPoint[vtIdx]=cutPoint.IsIncluded(plVtHd[vtIdx]);
			if(YSTRUE==isCutPoint[vtIdx])
			{
				++nCutPoint;
			}
		}

		if(2<=nCutPoint)
		{
			newPlg.Increment();
			if(YSOK!=SetAndSplitPolygon(newPlg.GetEnd(),shl,cutPoint,plHdToSplit[plIdx]) ||
			   1==newPlg.GetEnd().plVtHdArray.GetN())
			{
				newPlg.DeleteLast();
			}
		}
	}

	for(YSSIZE_T ceIdx=0; ceIdx<ceHdToSplit.GetN(); ++ceIdx)
	{
		newConstEdge.Increment();
		if(YSOK!=SetAndSplitConstEdge(newConstEdge.GetEnd(),shl,cutPoint,ceHdToSplit[ceIdx]))
		{
			newConstEdge.DeleteLast();
		}
	}

	return YSOK;
}
