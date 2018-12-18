/* ////////////////////////////////////////////////////////////

File Name: ysshellext_intersectionutil.cpp
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

#include "ysshellext_intersectionutil.h"
#include "ysshellext_splitop.h"
#include <ysshellextmisc.h>

void YsShellExt_IntersectionUtil::CleanUp(void)
{
	shl=NULL;
	itscPlg.CleanUp();
	itscVtx.CleanUp();
	edgeItscArray.CleanUp();
}

YsArray <YsVec3> YsShellExt_IntersectionUtil::GetIntersectingPoint(void) const
{
	YsArray <YsVec3> itscPos;
	for(auto &edge : edgeItscArray)
	{
		itscPos.Append(edge.pos);
	}
	return itscPos;
}

YsArray <YsVec3> YsShellExt_IntersectionUtil::GetOnPlaneVertex(void) const
{
	YsArray <YsVec3> itscPos;
	for(auto vtHd : itscVtx)
	{
		itscPos.Append(shl->GetVertexPosition(vtHd));
	}
	return itscPos;
}

YsArray <YsShellExt_IntersectionUtil::EdgeIntersection> &YsShellExt_IntersectionUtil::AllEdgeIntersection(void)
{
	return edgeItscArray;
}

////////////////////////////////////////////////////////////

void YsShellExt_CutByPlaneUtil::MakeOnPlaneVertex(const YsPlane &cutPln)
{
	const YsShellExt &shl=*(this->shl);

	for(auto vtHd : shl.AllVertex())
	{
		const YsVec3 vtPos=shl.GetVertexPosition(vtHd);
		if(YSTRUE==cutPln.CheckOnPlane(vtPos))
		{
			if(0<cutPlgPrj.GetN())
			{
				const YsVec2 prjVtPos=prjMat.ToXY(vtPos);
				const YSSIDE side=YsCheckInsidePolygon2(prjVtPos,cutPlgPrj);
				if(YSINSIDE!=side && YSBOUNDARY!=side)
				{
					continue;
				}
			}
			itscVtx.AddVertex(vtHd);
		}
	}
}

void YsShellExt_CutByPlaneUtil::MakeEdgePlaneIntersection(const YsPlane &cutPln)
{
	const YsShellExt &shl=*(this->shl);

	YsShellEdgeEnumHandle edHd=NULL;
	while(YSOK==shl.MoveToNextEdge(edHd))
	{
		YsShellVertexHandle edVtHd[2];
		shl.GetEdge(edVtHd,edHd);

		const YsVec3 edVtPos[2]=
		{
			shl.GetVertexPosition(edVtHd[0]),
			shl.GetVertexPosition(edVtHd[1])
		};

		YsVec3 itsc;
		if(YSOK==cutPln.GetPenetrationHighPrecision(itsc,edVtPos[0],edVtPos[1]))
		{
			if(0<cutPlgPrj.GetN())
			{
				const YsVec2 prjItsc=prjMat.ToXY(itsc);
				const YSSIDE side=YsCheckInsidePolygon2(prjItsc,cutPlgPrj);
				if(YSINSIDE!=side && YSBOUNDARY!=side)
				{
					continue;
				}
			}

			edgeItscArray.Increment();
			edgeItscArray.Last().edVtHd[0]=edVtHd[0];
			edgeItscArray.Last().edVtHd[1]=edVtHd[1];
			edgeItscArray.Last().pos=itsc;
			edgeItscArray.Last().newVtHd=NULL;

			auto edPlHd=shl.FindPolygonFromEdgePiece(edVtHd[0],edVtHd[1]);
			for(auto plHd : edPlHd)
			{
				itscPlg.AddPolygon(plHd);
			}
		}
	}
}

void YsShellExt_CutByPlaneUtil::MakeOnPlaneEdge(void)
{
	const YsShellExt &shl=*(this->shl);

	for(auto plHd : shl.AllPolygon())
	{
		auto plVtHd=shl.GetPolygonVertex(plHd);
		for(auto idx : plVtHd.AllIndex())
		{
			const YsShellVertexHandle edVtHd[2]={plVtHd[idx],plVtHd.GetCyclic(idx+1)};
			const YsVec3 edVtPos[2]=
			{
				shl.GetVertexPosition(edVtHd[0]),
				shl.GetVertexPosition(edVtHd[1])
			};

			if(0<cutPlgPrj.GetN())
			{
				const YsVec2 prjEdVtPos[2]={prjMat.ToXY(edVtPos[0]),prjMat.ToXY(edVtPos[1])};
				const YSSIDE side[2]=
				{
					YsCheckInsidePolygon2(prjEdVtPos[0],cutPlgPrj),
					YsCheckInsidePolygon2(prjEdVtPos[1],cutPlgPrj)
				};
				if(YSINSIDE!=side[0] && YSBOUNDARY!=side[0] && YSINSIDE!=side[1] && YSBOUNDARY!=side[1])
				{
					continue;
				}
			}

			if(YSTRUE==cutPln.CheckOnPlane(edVtPos[0]) && YSTRUE==cutPln.CheckOnPlane(edVtPos[1]))
			{
				onPlaneEdge.AddEdge(edVtHd);
			}
		}
	}
}

void YsShellExt_CutByPlaneUtil::CleanUp(void)
{
	YsShellExt_IntersectionUtil::CleanUp();
	this->shl=NULL;
	onPlaneEdge.CleanUp();

	cutPlg.CleanUp();
	cutPlgPrj.CleanUp();

	polygonSplitArray.CleanUp();
	constEdgeSplitArray.CleanUp();
	faceGroupSplitArray.CleanUp();
}

const YsPlane YsShellExt_CutByPlaneUtil::GetCuttingPlane(void) const
{
	return cutPln;
}

YSRESULT YsShellExt_CutByPlaneUtil::SetCutByPlane(const YsShellExt &shl,const YsPlane &pln)
{
	CleanUp();

	this->shl=&shl;
	itscPlg.SetShell((const YsShell &)shl);
	itscVtx.SetShell((const YsShell &)shl);
	onPlaneEdge.SetShell((const YsShell &)shl);
	cutPln=pln;

	MakeOnPlaneVertex(pln);
	MakeEdgePlaneIntersection(pln);
	MakeOnPlaneEdge(); // Must come after MakeOnPlaneVertex

	return YSOK;
}

YSRESULT YsShellExt_CutByPlaneUtil::SetCutByPolygon(const YsShellExt &shl,YsShellPolygonHandle plHd)
{
	YsArray <YsShellVertexHandle,4> plVtHd;
	shl.GetPolygon(plVtHd,plHd);
	return SetCutByPolygon(shl,plVtHd);
}

YSRESULT YsShellExt_CutByPlaneUtil::SetCutByPolygon(const YsShellExt &shl,YSSIZE_T nVtx,const YsShellVertexHandle plVtHd[])
{
	CleanUp();

	this->shl=&shl;
	itscPlg.SetShell((const YsShell &)shl);
	itscVtx.SetShell((const YsShell &)shl);
	onPlaneEdge.SetShell((const YsShell &)shl);

	cutPlg.Set(nVtx,NULL);
	for(YSSIZE_T idx=0; idx<nVtx; ++idx)
	{
		shl.GetVertexPosition(cutPlg[idx],plVtHd[idx]);
	}

	YsVec3 nom;
	if(YSOK==YsFindLeastSquareFittingPlaneNormal(nom,cutPlg))
	{
		const auto cen=YsGetCenter(cutPlg);

		this->shl=&shl;
		itscPlg.SetShell((const YsShell &)shl);
		itscVtx.SetShell((const YsShell &)shl);
		onPlaneEdge.SetShell((const YsShell &)shl);
		cutPln.Set(cen,nom);

		prjMat.MakeToXY(cen,nom);
		cutPlgPrj.Set(nVtx,NULL);
		for(auto index : cutPlg.AllIndex())
		{
			cutPlgPrj[index]=prjMat.ToXY(cutPlg[index]);
		}

		MakeOnPlaneVertex(cutPln);
		MakeEdgePlaneIntersection(cutPln);
		MakeOnPlaneEdge(); // Must come after MakeOnPlaneVertex

		return YSOK;
	}

	return YSERR;
}

void YsShellExt_CutByPlaneUtil::MakeSplitInfo(void)
{
	for(auto &edgeItsc : edgeItscArray)
	{
		itscVtx.AddVertex(edgeItsc.newVtHd);
	}

	YsShell_SplitInfo splitInfo;


	for(auto plHd : shl->AllPolygon())
	{
		auto plVtHd=shl->GetPolygonVertex(plHd);
		int itscCount=0;
		for(auto vtHd : plVtHd)
		{
			if(YSTRUE==itscVtx.IsIncluded(vtHd))
			{
				++itscCount;
			}
		}
		if(2<=itscCount)
		{
			polygonSplitArray.Increment();
			splitInfo.CleanUp();
			if(YSOK!=splitInfo.SetAndSplitPolygon(polygonSplitArray.Last(),*(const YsShellExt *)shl,itscVtx,plHd))
			{
				polygonSplitArray.DeleteLast();
			}
		}
	}

	for(auto ceHd : shl->AllConstEdge())
	{
		YsArray <YsShellVertexHandle> ceVtHd;
		YSBOOL isLoop;
		shl->GetConstEdge(ceVtHd,isLoop,ceHd);
		int itscCount=0;
		for(auto vtHd : ceVtHd)
		{
			if(YSTRUE==itscVtx.IsIncluded(vtHd))
			{
				++itscCount;
			}
		}
		if(0<itscCount)
		{
			constEdgeSplitArray.Increment();
			splitInfo.CleanUp();
			if(YSOK!=splitInfo.SetAndSplitConstEdge(constEdgeSplitArray.Last(),*shl,itscVtx,ceHd))
			{
				constEdgeSplitArray.DeleteLast();
			}
		}
	}
}

YSRESULT YsShellExt_CutByPlaneUtil::MakeToDrop(YsShellPolygonStore &plgToDrop,YsShellExt::ConstEdgeStore &ceToDrop,int dropWhichSide)
{
	plgToDrop.CleanUp();
	ceToDrop.CleanUp();

	plgToDrop.SetShell(*(const YsShell *)shl);
	ceToDrop.SetShell(*shl);

	for(auto vtHd : itscVtx)
	{
		auto vtPlHdArray=shl->FindPolygonFromVertex(vtHd);
		for(auto plHd : vtPlHdArray)
		{
			YSBOOL dropSide=YSFALSE,leaveSide=YSFALSE;

			auto plVtHd=shl->GetPolygonVertex(plHd);
			for(auto vtHd : plVtHd)
			{
				if(YSTRUE!=itscVtx.IsIncluded(vtHd))
				{
					auto vtPos=shl->GetVertexPosition(vtHd);
					const int side=cutPln.GetSideOfPlane(vtPos);
					if(0<side*dropWhichSide)
					{
						dropSide=YSTRUE;
					}
					else if(0>side*dropWhichSide)
					{
						leaveSide=YSTRUE;
					}
				}
			}

			if(YSTRUE==dropSide && YSTRUE!=leaveSide)
			{
				// It is a seed polygon.
				YsArray <YsShellPolygonHandle> todo;
				todo.Append(plHd);
				while(0<todo.GetN())
				{
					auto plHd=todo.Last();
					todo.DeleteLast();

					auto plVtHd=shl->GetPolygonVertex(plHd);
					for(auto plVtIdx : plVtHd.AllIndex())
					{
						YsShellPolygonHandle neiPlHd=shl->GetNeighborPolygon(plHd,(int)plVtIdx);
						if(NULL!=neiPlHd && YSTRUE!=plgToDrop.IsIncluded(neiPlHd))
						{
							const YsShellVertexHandle edVtHd[2]=
							{
								plVtHd[plVtIdx],
								plVtHd.GetCyclic(plVtIdx+1)
							};
							const YsVec3 plVtPos[2]=
							{
								shl->GetVertexPosition(edVtHd[0]),
								shl->GetVertexPosition(edVtHd[1])
							};
							YSBOOL oneOfThemPositive=YSFALSE;
							if((YSTRUE!=itscVtx.IsIncluded(edVtHd[0]) && 0<dropWhichSide*cutPln.GetSideOfPlane(plVtPos[0])) ||
							   (YSTRUE!=itscVtx.IsIncluded(edVtHd[1]) && 0<dropWhichSide*cutPln.GetSideOfPlane(plVtPos[1])))
							{
								oneOfThemPositive=YSTRUE;
							}
							YSBOOL oneOfThemNegative=YSFALSE;
							if((YSTRUE!=itscVtx.IsIncluded(edVtHd[0]) && 0>dropWhichSide*cutPln.GetSideOfPlane(plVtPos[0])) ||
							   (YSTRUE!=itscVtx.IsIncluded(edVtHd[1]) && 0>dropWhichSide*cutPln.GetSideOfPlane(plVtPos[1])))
							{
								oneOfThemNegative=YSTRUE;
							}
							if(YSTRUE==oneOfThemPositive && YSTRUE!=oneOfThemNegative)
							{
								plgToDrop.AddPolygon(neiPlHd);
								todo.Append(neiPlHd);
							}
						}
					}
				}

				YsShellVertexStore allVtx(*(const YsShell *)shl);
				for(auto plHd : plgToDrop)
				{
					allVtx.AddVertex(shl->GetPolygonVertex(plHd));
				}

				for(auto ceHd : shl->AllConstEdge())
				{
					YsArray <YsShellVertexHandle> ceVtHd;
					YSBOOL isLoop;
					shl->GetConstEdge(ceVtHd,isLoop,ceHd);

					YSBOOL allIncluded=YSTRUE;
					YSBOOL atLeastOneOffOfItscVtx=YSFALSE;
					for(auto vtHd : ceVtHd)
					{
						if(YSTRUE!=allVtx.IsIncluded(vtHd))
						{
							allIncluded=YSFALSE;
							break;
						}
						if(YSTRUE!=itscVtx.IsIncluded(vtHd))
						{
							atLeastOneOffOfItscVtx=YSTRUE;
						}
					}

					if(YSTRUE==allIncluded && YSTRUE==atLeastOneOffOfItscVtx)
					{
						ceToDrop.AddConstEdge(ceHd);
					}
				}
			}
		}
	}

	return YSOK;
}

void YsShellExt_CutByPlaneUtil::MakeFaceGroupSplitInfo(const YsShellPolygonStore &plgToDrop)
{
	YsShellPolygonStore visited(shl->Conv());
	for(auto plHd : plgToDrop)
	{
		if(YSTRUE!=visited.IsIncluded(plHd))
		{
			auto fgHd=shl->FindFaceGroupFromPolygon(plHd);
			if(NULL!=fgHd)
			{
				faceGroupSplitArray.Increment();
				YsShell_SplitInfo splitInfo;
				splitInfo.CleanUp();
				if(YSOK!=splitInfo.SplitFaceGroup(faceGroupSplitArray.Last(),*shl,fgHd,plgToDrop))
				{
					faceGroupSplitArray.DeleteLast();
				}

				visited.AddPolygon(shl->GetFaceGroup(fgHd));
			}
		}
	}
}
