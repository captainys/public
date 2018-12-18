/* ////////////////////////////////////////////////////////////

File Name: ysshellext_unfoldingutil.cpp
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

#include "ysshellext_unfoldingutil.h"
#include "ysshellext_geomcalc.h"
#include "ysshellext_localop.h"

////////////////////////////////////////////////////////////

void YsShellExt_UnfoldingUtil::FoldingInfo::Initialize(void)
{
	maxDha=0.0;
	numHighDhaEdge=0;
}

bool YsShellExt_UnfoldingUtil::FoldingInfo::IsBetterThan(const FoldingInfo &incoming) const
{
	if(this->numHighDhaEdge<incoming.numHighDhaEdge ||
	  (this->numHighDhaEdge==incoming.numHighDhaEdge && this->maxDha<incoming.maxDha))
	{
		return true;
	}
	return false;
}

void YsShellExt_UnfoldingUtil::FoldingInfoAndCollapseTarget::Initialize(void)
{
	info.Initialize();
	toVtHd=nullptr;
}
void YsShellExt_UnfoldingUtil::FoldingInfoAndNewVertexPosition::Initialize(void)
{
	info.Initialize();
	newVtxPos=YsOrigin();
}

YsShellExt_UnfoldingUtil::FoldingInfoAndMultiVertexPosition::FoldingInfoAndMultiVertexPosition(const FoldingInfoAndMultiVertexPosition &incoming)
{
	this->info=incoming.info;
	this->newVtxPos=incoming.newVtxPos;
}
YsShellExt_UnfoldingUtil::FoldingInfoAndMultiVertexPosition &YsShellExt_UnfoldingUtil::FoldingInfoAndMultiVertexPosition::operator=(const FoldingInfoAndMultiVertexPosition &incoming)
{
	this->info=incoming.info;
	this->newVtxPos=incoming.newVtxPos;
	return *this;
}
YsShellExt_UnfoldingUtil::FoldingInfoAndMultiVertexPosition::FoldingInfoAndMultiVertexPosition(FoldingInfoAndMultiVertexPosition &&incoming)
{
	this->info=incoming.info;
	this->newVtxPos.MoveFrom(incoming.newVtxPos);
}
YsShellExt_UnfoldingUtil::FoldingInfoAndMultiVertexPosition &YsShellExt_UnfoldingUtil::FoldingInfoAndMultiVertexPosition::operator=(FoldingInfoAndMultiVertexPosition &&incoming)
{
	this->info=incoming.info;
	this->newVtxPos.MoveFrom(incoming.newVtxPos);
	return *this;
}
void YsShellExt_UnfoldingUtil::FoldingInfoAndMultiVertexPosition::Initialize(void)
{
	info.Initialize();
	newVtxPos.CleanUp();
}

////////////////////////////////////////////////////////////


void YsShellExt_UnfoldingUtil::MakeInfo(const YsShellExt &shl,const double dhaThr)
{
	problematicEdge.SetShell(shl.Conv());
	problematicEdge.CleanUp();

	YsShellEdgeEnumHandle edHd=nullptr;
	while(YSOK==shl.MoveToNextEdge(edHd))
	{
		if(2==shl.GetNumPolygonUsingEdge(edHd))
		{
			auto edPlHd=shl.FindPolygonFromEdgePiece(edHd);
			auto fgHd0=shl.FindFaceGroupFromPolygon(edPlHd[0]);
			auto fgHd1=shl.FindFaceGroupFromPolygon(edPlHd[1]);
			if(fgHd0==fgHd1 && dhaThr<YsShellExt_CalculateDihedralAngle(shl,edPlHd))
			{
				problematicEdge.Add(shl.GetEdge(edHd));
			}
		}
	}
}

void YsShellExt_UnfoldingUtil::MakeInfo(const YsShellExt &shl,YsShellExt::FaceGroupHandle fgHd,const double dhaThr)
{
	MakeInfo(shl,YsShellPassThroughSource(shl.Conv()),fgHd,dhaThr);
}

void YsShellExt_UnfoldingUtil::MakeInfo(const YsShellExt &shl,const YsShellVertexPositionSource &vtPosSrc,YsShellExt::FaceGroupHandle fgHd,const double dhaThr)
{
	problematicEdge.SetShell(shl.Conv());
	problematicEdge.CleanUp();

	for(auto plHd : shl.GetFaceGroup(fgHd))
	{
		auto nPlVt=shl.GetPolygonNumVertex(plHd);
		for(decltype(nPlVt) edIdx=0; edIdx<nPlVt; ++edIdx)
		{
			auto neiPlHd=shl.GetNeighborPolygon(plHd,edIdx);
			if(nullptr!=neiPlHd && 
			   plHd<neiPlHd &&
			   dhaThr<YsShellExt_CalculateDihedralAngle(shl,vtPosSrc,plHd,neiPlHd))
			{
				problematicEdge.Add(shl.GetPolygonEdge(plHd,edIdx));
			}
		}
	}
}

const YsShellEdgeStore &YsShellExt_UnfoldingUtil::GetProblematicEdge(void) const
{
	return problematicEdge;
}

YsShellExt_UnfoldingUtil::FoldingInfo YsShellExt_UnfoldingUtil::GetFoldingInfo(const YsShellExt &shl,YsShell::VertexHandle vtHd,const double dhaThr) const
{
	FoldingInfo info;
	info.Initialize();

	auto vtPlHd=shl.FindPolygonFromVertex(vtHd);

	// Dha against surrounding polygons (i.e. polygons not using vtHd, but in the same face group.
	// For the inside polygons (i.e., polygons using vtHd), check only plHd<neiPlHd to prevent double-counting.
	for(auto plHd : vtPlHd)
	{
		const auto nPlVt=shl.GetPolygonNumVertex(plHd);
		for(YSSIZE_T edIdx=0; edIdx<nPlVt; ++edIdx)
		{
			auto neiPlHd=shl.GetNeighborPolygon(plHd,edIdx);
			if(nullptr!=neiPlHd &&
			   shl.FindFaceGroupFromPolygon(neiPlHd)==shl.FindFaceGroupFromPolygon(plHd))
			{
				if(plHd<neiPlHd || YSTRUE!=shl.IsPolygonUsingVertex(neiPlHd,vtHd))
				{
					auto dha=YsShellExt_CalculateDihedralAngle(shl.Conv(),neiPlHd,plHd);
printf("C %d<->%d %lf\n",shl.GetSearchKey(plHd),shl.GetSearchKey(neiPlHd),dha);
					YsMakeGreater(info.maxDha,dha);
					if(dhaThr<dha)
					{
						++info.numHighDhaEdge;
					}
				}
			}
		}
	}

	return info;
}

YsShellExt_UnfoldingUtil::FoldingInfo YsShellExt_UnfoldingUtil::GetFoldingInfoAfterCollapsing(const YsShellExt &shl,YsShell::VertexHandle fromVtHd,YsShell::VertexHandle toVtHd,const double dhaThr) const
{
	FoldingInfo info;
	info.Initialize();

	YsShell_CollapseInfo collInfo;
	collInfo.MakeInfo(shl.Conv(),fromVtHd,toVtHd);

	YsShellPolygonAttribTable <YsVec3> plNom(shl.Conv());
	for(auto &plChg : collInfo.newPlg)
	{
		YsArray <YsVec3,4> plVtPos;
		plVtPos.Resize(plChg.plVtHd.GetN());
		for(auto vtIdx : plChg.plVtHd.AllIndex())
		{
			plVtPos[vtIdx]=shl.GetVertexPosition(plChg.plVtHd[vtIdx]);
		}

		YsVec3 newNom;
		YsGetAverageNormalVector(newNom,plVtPos);
		plNom.SetAttrib(plChg.plHd,newNom);
	}

	// Surrounding polygons.
	for(auto &plChg : collInfo.newPlg)
	{
		auto plHd0=plChg.plHd;
		auto fgHd0=shl.FindFaceGroupFromPolygon(plChg.plHd);
		const auto &plVtHd=plChg.plVtHd;
		auto nom0=*plNom[plChg.plHd];

		for(auto edIdx : plVtHd.AllIndex())
		{
			YsShell::VertexHandle edVtHd[2]=
			{
				plVtHd[edIdx],
				plVtHd.GetCyclic(edIdx+1)
			};

			auto edPlHd=shl.FindPolygonFromEdgePiece(edVtHd);
			if(2==edPlHd.GetN() &&
			   shl.FindFaceGroupFromPolygon(edPlHd[0])==shl.FindFaceGroupFromPolygon(edPlHd[1]))
			{
				// Polygon0 is a polygon that is modified by the collapse.
				// That is, a polygon is using fromVtHd.
				// Surrounding polygons are not using fromVtHd.

				decltype(plHd0) plHd1=nullptr;
				if(YSTRUE==shl.IsPolygonUsingVertex(edPlHd[0],fromVtHd) &&
				   YSTRUE!=shl.IsPolygonUsingVertex(edPlHd[1],fromVtHd))
				{
					plHd1=edPlHd[1];
				}
				else if(YSTRUE!=shl.IsPolygonUsingVertex(edPlHd[0],fromVtHd) &&
				        YSTRUE==shl.IsPolygonUsingVertex(edPlHd[1],fromVtHd))
				{
					plHd1=edPlHd[0];
				}

				if(nullptr!=plHd1)
				{
					auto nom1=YsShell_CalculateNormal(shl.Conv(),plHd1);

					auto dha=acos(YsBound(nom0*nom1,-1.0,1.0));

printf("A %d<->%d %lf\n",shl.GetSearchKey(plHd0),shl.GetSearchKey(plHd1),dha);

					YsMakeGreater(info.maxDha,dha);
					if(dhaThr<dha)
					{
						++info.numHighDhaEdge;
					}
				}
			}
		}
	}

	// Among polygons that will be modified.
	for(auto &plChg : collInfo.newPlg)
	{
		auto nom0=*plNom[plChg.plHd];
		auto nPlVt=shl.GetPolygonNumVertex(plChg.plHd);
		for(decltype(nPlVt) edIdx=0; edIdx<nPlVt; ++edIdx)
		{
			auto neiPlHd=shl.GetNeighborPolygon(plChg.plHd,edIdx);
			if(nullptr!=neiPlHd && plChg.plHd<neiPlHd)
			{
				auto nom1Ptr=plNom[neiPlHd];
				if(nullptr!=nom1Ptr)  // Means this is a polygon to be modified.
				{
					auto nom1=*nom1Ptr;
					auto dha=acos(YsBound(nom0*nom1,-1.0,1.0));

printf("B %d<->%d %lf\n",shl.GetSearchKey(plChg.plHd),shl.GetSearchKey(neiPlHd),dha);

					YsMakeGreater(info.maxDha,dha);
					if(dhaThr<dha)
					{
						++info.numHighDhaEdge;
					}
				}
			}
		}
	}

	return info;

}

YsShellExt_UnfoldingUtil::FoldingInfo YsShellExt_UnfoldingUtil::GetFoldingInfoAfterSmoothing(const YsShellExt &shl,YsShell::VertexHandle fromVtHd,const YsVec3 &newPos,const double dhaThr) const
{
	FoldingInfo info;
	info.Initialize();

	auto vtPlHd=shl.FindPolygonFromVertex(fromVtHd);


	YsShellPolygonAttribTable <YsVec3> plNom(shl.Conv());
	for(auto plHd : vtPlHd)
	{
		auto plVtHd=shl.GetPolygonVertex(plHd);
		YsArray <YsVec3,4> plVtPos(plVtHd.GetN(),nullptr);

		for(auto vtIdx : plVtHd.AllIndex())
		{
			if(plVtHd[vtIdx]!=fromVtHd)
			{
				plVtPos[vtIdx]=shl.GetVertexPosition(plVtHd[vtIdx]);
			}
			else
			{
				plVtPos[vtIdx]=newPos;
			}
		}

		YsVec3 nom;
		YsGetAverageNormalVector(nom,plVtPos);
		plNom.SetAttrib(plHd,nom);
	}

	GetFoldingInfo(info,shl,vtPlHd,plNom,dhaThr);

	return info;
}


YsShellExt_UnfoldingUtil::FoldingInfo YsShellExt_UnfoldingUtil::GetFoldingInfoBeforeMultiSmoothing(const YsShellExt &shl,YSSIZE_T nVtHd,const YsShell::VertexHandle vtHd[],const double dhaThr) const
{
	YsArray <YsShell::VertexAndPos> vtSmth(nVtHd,nullptr);
	for(YSSIZE_T idx=0; idx<nVtHd; ++idx)
	{
		vtSmth[idx].vtHd=vtHd[idx];
		vtSmth[idx].pos=shl.GetVertexPosition(vtHd[idx]);
	}
	return GetFoldingInfoAfterMultiSmoothing(shl,vtSmth,dhaThr);
}

YsShellExt_UnfoldingUtil::FoldingInfo YsShellExt_UnfoldingUtil::GetFoldingInfoAfterMultiSmoothing(const YsShellExt &shl,YsArray <YsShell::VertexAndPos> &vtPos,const double dhaThr) const
{
	FoldingInfo info;
	info.Initialize();

	YsShellVertexAttribTable <YsVec3> tmpVtPos(shl.Conv());
	for(auto vp : vtPos)
	{
		tmpVtPos.SetAttrib(vp.vtHd,vp.pos);
	}

	YsShellPolygonAttribTable <YsVec3> plNom(shl.Conv());
	YsArray <YsShell::PolygonHandle> allVtPlHd;   // <- Will be non-overlapping polygons using at least one of the vertex to be moved.
	for(auto vp : vtPos)
	{
		for(auto plHd : shl.FindPolygonFromVertex(vp.vtHd))
		{
			if(nullptr==plNom[plHd])
			{
				allVtPlHd.Add(plHd);

				auto plVtHd=shl.GetPolygonVertex(plHd);
				YsArray <YsVec3,4> plVtPos(plVtHd.GetN(),nullptr);
				for(auto idx : plVtHd.AllIndex())
				{
					auto tmpVtPosPtr=tmpVtPos[plVtHd[idx]];
					if(nullptr!=tmpVtPosPtr)
					{
						plVtPos[idx]=*tmpVtPosPtr;
					}
					else
					{
						plVtPos[idx]=shl.GetVertexPosition(plVtHd[idx]);
					}
				}
				auto nom=YsGetAverageNormalVector(plVtPos);
				plNom.SetAttrib(plHd,nom);
			}
		}
	}

	GetFoldingInfo(info,shl,allVtPlHd,plNom,dhaThr);

	return info;
}

void YsShellExt_UnfoldingUtil::GetFoldingInfo(FoldingInfo &info,const YsShellExt &shl,const YsArray <YsShell::PolygonHandle> &allPlHd,const YsShellPolygonAttribTable <YsVec3> &plNom,const double dhaThr) const
{
	// Dha against surrounding polygons (i.e. polygons not using vtHd, but in the same face group.
	// For the inside polygons (i.e., polygons using vtHd), check only plHd<neiPlHd to prevent double-counting.
	for(auto plHd : allPlHd)
	{
		const auto nPlVt=shl.GetPolygonNumVertex(plHd);
		YsVec3 nom;
		auto nomPtr=plNom[plHd];
		if(nullptr!=nomPtr)
		{
			nom=*nomPtr;
		}
		else
		{
			nom=YsShell_CalculateNormal(shl.Conv(),plHd);
		}

		for(YSSIZE_T edIdx=0; edIdx<nPlVt; ++edIdx)
		{
			auto neiPlHd=shl.GetNeighborPolygon(plHd,edIdx);
			if(nullptr!=neiPlHd &&
			   shl.FindFaceGroupFromPolygon(neiPlHd)==shl.FindFaceGroupFromPolygon(plHd))
			{
				auto nomPtr=plNom[neiPlHd];
				if(plHd<neiPlHd ||  // Limit to one occurence per edge.
				   nullptr==nomPtr) // If it is outside the vicinity.
				{
					YsVec3 neiNom;
					if(nullptr!=nomPtr)
					{
						neiNom=*nomPtr;
					}
					else
					{
						neiNom=YsShell_CalculateNormal(shl.Conv(),neiPlHd);
					}

					auto dha=acos(YsBound(nom*neiNom,-1.0,1.0));
printf("D %d<->%d %lf\n",shl.GetSearchKey(plHd),shl.GetSearchKey(neiPlHd),dha);
					YsMakeGreater(info.maxDha,dha);
					if(dhaThr<dha)
					{
						++info.numHighDhaEdge;
					}
				}
			}
		}
	}
}

YsArray <YsShell::VertexHandle> YsShellExt_UnfoldingUtil::FindCollapseCandidate(const YsShellExt &shl,const YsShell::VertexHandle fromVtHd) const
{
	YsArray <YsShell::VertexHandle> collCan;

	if(1>=shl.GetNumFaceGroupUsingVertex(fromVtHd))
	{
		for(auto connVtHd : shl.GetConnectedVertex(fromVtHd))
		{
			if(1<shl.GetNumFaceGroupUsingVertex(connVtHd))
			{
				collCan.Add(connVtHd);
			}
		}
	}

	return collCan;
}

YsShellExt_UnfoldingUtil::FoldingInfoAndCollapseTarget YsShellExt_UnfoldingUtil::FindBestCollapseCandidate(const YsShellExt &shl,const YsShell::VertexHandle fromVtHd,const YsArray <YsShell::VertexHandle> &toCandidate,const double dhaThr) const
{
	FoldingInfoAndCollapseTarget best;
	best.Initialize();
	best.info=GetFoldingInfo(shl,fromVtHd,dhaThr);
	best.toVtHd=nullptr;

	for(auto vtHdTest : toCandidate)
	{
		auto testInfo=GetFoldingInfoAfterCollapsing(shl,fromVtHd,vtHdTest,dhaThr);
		if(testInfo.IsBetterThan(best.info))
		{
			best.info=testInfo;
			best.toVtHd=vtHdTest;
		}
	}

	return best;
}

YsShellExt_UnfoldingUtil::FoldingInfoAndNewVertexPosition YsShellExt_UnfoldingUtil::FindBestSmoothingLocation(const YsShellExt &shl,const YsShell::VertexHandle fromVtHd,const double dhaThr) const
{
	FoldingInfoAndNewVertexPosition best;
	best.Initialize();
	best.info=GetFoldingInfo(shl,fromVtHd,dhaThr);
	best.newVtxPos=shl.GetVertexPosition(fromVtHd);


	YsArray <YsVec3> candidateLocation;

	// Laplacian?
	candidateLocation.Add(shl.GetCenter(shl.GetConnectedVertex(fromVtHd)));



	// Pick the best one.
	for(auto newVtxPos : candidateLocation)
	{
		auto testInfo=GetFoldingInfoAfterSmoothing(shl,fromVtHd,newVtxPos,dhaThr);
		if(testInfo.IsBetterThan(best.info))
		{
			best.info=testInfo;
			best.newVtxPos=newVtxPos;
		}
	}

	return best;
}

//YsShellExt_UnfoldingUtil::FoldingInfoAndMultiVertexPosition YsShellExt_UnfoldingUtil::FindBestNeighborSmoothingLocation(const YsShellExt &shl,YsShell::VertexHandle fromVtHd,int nNeighbor,const double dhaThr) const
//{
//}

YsShellExt_UnfoldingUtil::FoldingInfoAndMultiVertexPosition YsShellExt_UnfoldingUtil::FindBestMultiSmoothingLocation(const YsShellExt &shl,YSSIZE_T nVtHd,const YsShell::VertexHandle vtHd[],const double dhaThr) const
{
	YsShellExt_UnfoldingUtil::FoldingInfoAndMultiVertexPosition best;
	best.Initialize();
	best.newVtxPos.Resize(nVtHd);

	YsArray <YsShell::VertexAndPos> vtSmth(nVtHd,nullptr);
	for(YSSIZE_T idx=0; idx<nVtHd; ++idx)
	{
		vtSmth[idx].vtHd=vtHd[idx];
	}

	// First try Laplacian
	for(auto &vt : vtSmth)
	{
		vt.pos=shl.GetCenter(shl.GetConnectedVertex(vt.vtHd));
		best.info=GetFoldingInfoAfterMultiSmoothing(shl,vtSmth,dhaThr);
		best.newVtxPos=vtSmth;
	}

	return best;
}

