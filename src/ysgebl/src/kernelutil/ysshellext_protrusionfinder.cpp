/* ////////////////////////////////////////////////////////////

File Name: ysshellext_protrusionfinder.cpp
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

#include <ysshellextmisc.h>

#include "ysshellext_protrusionfinder.h"
#include "ysshellext_boundaryutil.h"
#include "ysshellext_geomcalc.h"
#include "ysshellext_trackingutil.h"
#include "ysshellext_localop.h"

void YsShellExt_ProtrusionFinder::Protrusion::Boundary::CleanUp(void)
{
	vtHd.CleanUp();
	constraintIndex.CleanUp();

	midVertex=nullptr;
	tri.CleanUp();
	newCeVtHd.CleanUp();

	canRemoveSurroundingConstEdge=YSFALSE;
}

////////////////////////////////////////////////////////////

void YsShellExt_ProtrusionFinder::Protrusion::CleanUp(void)
{
	plKey.CleanUp();
	path.CleanUp();
	boundary.CleanUp();
}

void YsShellExt_ProtrusionFinder::Protrusion::MakeBoundaryConstraint(const YsShellExt &shl)
{
	YsShellPolygonStore plToDel(shl.Conv());
	plToDel.Add(PlHd(shl));

	for(auto &b : boundary)
	{
		MakeBoundaryConstraint(b.constraintIndex,shl,b.vtHd,plToDel);
	}
}

void YsShellExt_ProtrusionFinder::Protrusion::MakeBoundaryConstraint(YsArray <YSSIZE_T> &boundaryConstIndex,const YsShellExt &shl,const YsConstArrayMask <YsShell::VertexHandle> &boundary,const YsShellPolygonStore &plToDel)
{
	boundaryConstIndex.CleanUp();

	for(YSSIZE_T vtIdx=0; vtIdx<boundary.GetN(); ++vtIdx)
	{
		YsArray <YsShellExt::FaceGroupHandle,8> vtFgHd;
		for(auto plHd : shl.FindPolygonFromVertex(boundary[vtIdx]))
		{
			if(YSTRUE!=plToDel.IsIncluded(plHd))
			{
				auto fgHd=shl.FindFaceGroupFromPolygon(plHd);
				if(YSTRUE!=vtFgHd.IsIncluded(fgHd))
				{
					vtFgHd.Add(fgHd);
					if(1<vtFgHd.GetN())
					{
						boundaryConstIndex.Add(vtIdx);
						goto NEXTVTIDX;
					}
				}
			}
		}

		for(auto connVtHd : shl.GetConnectedVertex(boundary[vtIdx]))
		{
			auto edCeHd=shl.FindConstEdgeFromEdgePiece(boundary[vtIdx],connVtHd);
			if(0<edCeHd.GetN())
			{
				for(auto edPlHd : shl.FindPolygonFromEdgePiece(boundary[vtIdx],connVtHd))
				{
					if(YSTRUE==plToDel.IsIncluded(edPlHd))
					{
						goto NEXTVTIDX;
					}
				}
				boundaryConstIndex.Add(vtIdx);
				goto NEXTVTIDX;
			}
		}
	NEXTVTIDX:
		;
	}
}

YSRESULT YsShellExt_ProtrusionFinder::Protrusion::TessellateBoundary(const YsShellExt &shl)
{
	YSRESULT status=YSOK;
	for(auto &b : boundary)
	{
		auto tri=TessellateBoundary(b,shl);
		if(0==tri.GetN())
		{
			status=YSERR;
			break;
		}
		b.tri.MoveFrom(tri);
	}

	if(YSOK!=status)
	{
		for(auto &b : boundary)
		{
			b.tri.CleanUp();
		}
	}

	return status;
}

YsArray <YsShell::Triangle> YsShellExt_ProtrusionFinder::Protrusion::TessellateBoundary(Boundary &b,const YsShellExt &shl)
{
	YsArray <YsShell::Triangle> tri;

	if(nullptr!=b.midVertex && 2<=b.constraintIndex.GetN())
	{
		YsArray <YsArray <YsShell::VertexHandle> > subBoundary;

		subBoundary.Increment();
		subBoundary.Last().Add(b.midVertex);
		for(auto i=b.constraintIndex.Last(); i<b.vtHd.GetN(); ++i)
		{
			subBoundary.Last().Add(b.vtHd[i]);
		}
		for(YSSIZE_T i=0; i<=b.constraintIndex[0]; ++i)
		{
			subBoundary.Last().Add(b.vtHd[i]);
		}

		for(int subIdx=0; subIdx<b.constraintIndex.GetN()-1; ++subIdx)
		{
			subBoundary.Increment();
			subBoundary.Last().Add(b.midVertex);
			for(YSSIZE_T i=b.constraintIndex[subIdx]; i<=b.constraintIndex[subIdx+1]; ++i)
			{
				subBoundary.Last().Add(b.vtHd[i]);
			}
		}

		YSRESULT status=YSOK;
		for(auto &sb : subBoundary)
		{
			auto subTri=YsShellExt_TriangulatePolygon(shl.Conv(),sb);
			if(0==subTri.GetN())
			{
				status=YSERR;
				break;
			}
			for(int i=0; i<subTri.GetN(); i+=3)
			{
				tri.Increment();
				tri.Last().Set(subTri[i],subTri[i+1],subTri[i+2]);
			}
		}

		if(YSOK==status)
		{
			if(2==b.constraintIndex.GetN())
			{
				b.newCeVtHd.Set(1,nullptr);
				b.newCeVtHd[0].Resize(3);
				b.newCeVtHd[0][0]=b.vtHd[b.constraintIndex[0]];
				b.newCeVtHd[0][1]=b.midVertex;
				b.newCeVtHd[0][2]=b.vtHd[b.constraintIndex[1]];
			}
			else if(3<=b.constraintIndex.GetN())
			{
				b.newCeVtHd.Set(b.constraintIndex.GetN(),nullptr);
				for(auto idx : b.constraintIndex.AllIndex())
				{
					b.newCeVtHd[idx].Resize(2);
					b.newCeVtHd[idx][0]=b.midVertex;
					b.newCeVtHd[idx][1]=b.vtHd[b.constraintIndex[idx]];
				}
			}
			else
			{
				b.newCeVtHd.CleanUp();
			}
			b.canRemoveSurroundingConstEdge=YSTRUE;
			return tri;
		}

		tri.CleanUp();
	}

	// Last resort: Plain tessellation.  Forget about the mid vertices.
	//              Or, if there is no mid vertex, it is just fine.
	if(0==tri.GetN())
	{
		auto triVtHd=YsShellExt_TriangulatePolygon(shl.Conv(),b.vtHd);

		tri.Resize(triVtHd.GetN()/3);
		for(int t=0; t<tri.GetN(); ++t)
		{
			tri[t].Set(triVtHd[t*3],triVtHd[t*3+1],triVtHd[t*3+2]);
		}

		if(b.constraintIndex.GetN()<=1)
		{
			b.canRemoveSurroundingConstEdge=YSTRUE;
		}
		else
		{
			b.canRemoveSurroundingConstEdge=YSFALSE;
		}
	}
	return tri;
}

YsArray <YsShell::PolygonHandle> YsShellExt_ProtrusionFinder::Protrusion::PlHd(const YsShellExt &shl) const
{
	YsArray <YsShell::PolygonHandle> plHdArray;
	for(auto k : plKey)
	{
		auto plHd=shl.FindPolygon(k);
		if(nullptr!=plHd)
		{
			plHdArray.Add(plHd);
		}
	}
	return plHdArray;
}

////////////////////////////////////////////////////////////

YsShellExt_ProtrusionFinder::SearchCondition::SearchCondition()
{
	fromPolygon=YSTRUE;
	fromFaceGroup=YSFALSE;

	taperAngleThr=YsPi/18.0;  // 10 degree
	diameter=1.0;
	perFaceGroup=YSFALSE;
	nSample=10000;
	nMinimumSamplePerFaceGroup=6;
}

////////////////////////////////////////////////////////////

YsShellExt_ProtrusionFinder::YsShellExt_ProtrusionFinder()
{
}
YsShellExt_ProtrusionFinder::~YsShellExt_ProtrusionFinder()
{
	CleanUp();
}
void YsShellExt_ProtrusionFinder::CleanUp(void)
{
	protrusion.CleanUp();
}

void YsShellExt_ProtrusionFinder::Begin(const YsShellExt &shl)
{
	partOfProtrusion.CleanUp();
	partOfProtrusion.SetShell(shl.Conv());

	chunkPlKey.CleanUp();
	plHdToChunkIdx.CleanUp();
	plHdToChunkIdx.SetShell(shl.Conv());

	for(auto plHd : shl.AllPolygon())
	{
		if(nullptr==plHdToChunkIdx[plHd])
		{
			YsShellExt_TrackingUtil::Option opt;
			opt.crossNonManifoldEdge=YSTRUE;
			auto chunk=YsShellExt_TrackingUtil::MakeEdgeConnectedPolygonGroup(shl.Conv(),plHd,opt,nullptr);

			auto nextIdx=chunkPlKey.GetN();
			for(auto plHd : chunk)
			{
				plHdToChunkIdx.SetAttrib(plHd,nextIdx);
			}

			chunkPlKey.Increment();
			chunkPlKey.Last().CleanUp();
			for(auto plHd : chunk)
			{
				chunkPlKey.Last().Add(shl.GetSearchKey(plHd));
			}
		}
	}
}

void YsShellExt_ProtrusionFinder::Find(const YsShellExt &shl,SearchCondition cond)
{
	CleanUp();
	if(YSTRUE==cond.fromPolygon)
	{
		printf("Searching holes/protrusions from each polygon.\n");
		FindFromPolygon(shl,cond);
	}
	if(YSTRUE==cond.fromFaceGroup)
	{
		printf("Searching holes/protrusions from each face group.\n");
		FindFromFaceGroup(shl,cond);
	}
}

void YsShellExt_ProtrusionFinder::FindFromPolygon(const YsShellExt &shl,SearchCondition cond)
{
	auto plSample=MakeSamplePolygon(shl,cond);

	YsLoopCounter ctr;
	ctr.BeginCounter(plSample.GetN());

	for(auto plHd : plSample)
	{
		ctr.Increment();

		if(YSTRUE!=partOfProtrusion.IsIncluded(plHd))
		{
			Protrusion prt;
			if(YSOK==FindFromPolygon(prt,shl,plHd,cond))
			{
				OptimizeAxis(prt,shl,cond);
				if(YSTRUE==cond.perFaceGroup)
				{
					AugmentByFaceGroup(prt,shl);
					if(YSOK!=RecheckDiameter(prt,shl,cond))
					{
						continue;
					}
				}

				if(YSOK!=CheckTaper(prt,shl,cond))
				{
					continue;
				}

				CacheBoundary(prt,shl);
				if(2==prt.boundary.GetN())
				{
					partOfProtrusion.Add(prt.PlHd(shl));
					CacheVolume(prt,shl);

					protrusion.Increment();
					protrusion.Last()=(Protrusion &&)prt;
				}
			}
		}
	}

	ctr.EndCounter();
}

void YsShellExt_ProtrusionFinder::FindFromFaceGroup(const YsShellExt &shl,SearchCondition cond)
{
	YsLoopCounter ctr;
	ctr.BeginCounter(shl.GetNumFaceGroup());

	for(auto fgHd : shl.AllFaceGroup())
	{
		Protrusion prt;
		prt.seedPlHd=nullptr;
		auto fgPlHd=shl.GetFaceGroup(fgHd);

		if(0<fgPlHd.GetN())
		{
			YsVec3 cen=YsVec3::Origin();
			YsArray <YsVec3> nom;
			for(auto plHd : fgPlHd)
			{
				nom.Add(shl.GetNormal(plHd));
				cen+=shl.GetCenter(plHd);
			}

			YSBOOL alreadyCovered=YSTRUE;
			for(auto plHd : fgPlHd)
			{
				if(YSTRUE!=partOfProtrusion.IsIncluded(plHd))
				{
					alreadyCovered=YSFALSE;
					break;
				}
			}
			if(YSTRUE!=alreadyCovered)
			{
				prt.plKey.CleanUp();
				for(auto plHd : fgPlHd)
				{
					prt.plKey.Add(shl.GetSearchKey(plHd));
				}

				cen/=(double)fgPlHd.GetN();
				if(YSOK==YsFindLeastSquareFittingPlaneNormal(prt.axis,nom))
				{
					prt.cen=cen;
					if(YSOK==RecheckDiameter(prt,shl,cond) &&
					   YSOK==CheckTaper(prt,shl,cond) &&
					   YSOK==CheckAngularCoverageAroundAxis(prt,shl))
					{
						CacheBoundary(prt,shl);
						if(2<=prt.boundary.GetN())
						{
							CacheVolume(prt,shl);
							partOfProtrusion.Add(prt.PlHd(shl));

							protrusion.Increment();
							protrusion.Last()=(Protrusion &&)prt;
						}
					}
				}
			}
		}
	}
}

YsShellPolygonStore YsShellExt_ProtrusionFinder::MakeSamplePolygon(const YsShellExt &shl,SearchCondition cond) const
{
	YsShellPolygonStore plSample(shl.Conv());

	{
		YSSIZE_T ctr=0;
		for(auto plHd : shl.AllPolygon())
		{
			if(0>=ctr)
			{
				plSample.Add(plHd);
				ctr+=shl.GetNumPolygon();
			}
			ctr-=cond.nSample;
		}
	}

	for(auto fgHd : shl.AllFaceGroup())
	{
		auto fgPlHd=shl.GetFaceGroup(fgHd);

		int nIncluded=0;
		for(auto idx : fgPlHd.ReverseIndex())
		{
			if(YSTRUE==plSample.IsIncluded(fgPlHd[idx]))
			{
				fgPlHd.DeleteBySwapping(idx);
				++nIncluded;
			}
		}

		YSSIZE_T toAdd=cond.nMinimumSamplePerFaceGroup-nIncluded;
		if(0<toAdd)
		{
			YSSIZE_T ctr=0;
			for(auto plHd : fgPlHd)
			{
				if(0>=ctr)
				{
					plSample.Add(plHd);
					ctr+=fgPlHd.GetN();
				}
				ctr-=toAdd;
			}
		}
	}

	return plSample;
}

YSRESULT YsShellExt_ProtrusionFinder::FindFromPolygon(Protrusion &prt,const YsShellExt &shl,YsShell::PolygonHandle plHd,SearchCondition cond)
{
	auto nom=shl.GetNormal(plHd);

	YsArray <YsVec3,4> plVtPos;
	shl.GetPolygon(plVtPos,plHd);
	for(int edIdx=0; edIdx<plVtPos.GetN(); ++edIdx)
	{
		YsVec3 dir=plVtPos.GetCyclic(edIdx+1)-plVtPos[edIdx];
		YsVec3 axis=nom^dir;
		if(YSOK==dir.Normalize() && YSOK==axis.Normalize())
		{
			if(YSOK==FindFromPolygonOneDirection(prt,shl,plHd,cond,axis,dir))
			{
				return YSOK;
			}
		}
	}

	auto u=YsUnitVector(nom.GetArbitraryPerpendicularVector());
	auto v=nom^u;

	for(int angle=0; angle<4; ++angle)
	{
		const double radian=YsPi*2.0*(double)angle/8.0;
		auto dir=u*cos(radian)+v*sin(radian);
		auto axis=nom^dir;
		if(YSOK==FindFromPolygonOneDirection(prt,shl,plHd,cond,axis,dir))
		{
			return YSOK;
		}
	}

	return YSERR;
}

YSRESULT YsShellExt_ProtrusionFinder::FindFromPolygonOneDirection(Protrusion &prt,const YsShellExt &shl,YsShell::PolygonHandle plHd0,SearchCondition cond,const YsVec3 &axis,const YsVec3 &dir)
{
	auto cen=shl.GetCenter(plHd0);
	auto nom=shl.GetNormal(plHd0);

	YsShellCrawler crawler;

	YsArray <YsShell::PolygonHandle> pathPlHd;
	YsArray <YsVec3> pathPos;

	crawler.SetMode(YsShellCrawler::CRAWL_ON_PLANE);
	crawler.SetConstraintPlane(YsPlane(cen,axis));

	if(YSOK!=crawler.Start(shl.Conv(),cen,dir,plHd0))
	{
		return YSERR;
	}

	YsShellPolygonStore visited(shl.Conv());
	visited.Add(plHd0);

	YsVec3 sum=YsVec3::Origin();
	while(YSOK==crawler.Crawl(shl.Conv(),0.0))
	{
		auto state=crawler.GetCurrentState();

		pathPlHd.Add(state.plHd);
		pathPos.Add(state.pos);

		sum+=state.pos;

		if(state.plHd==plHd0)
		{
			break;
		}

		// 2016/05/01 The crawler can wonder into an infinite loop from a vertex.
		//            Visited polygon must be kept track and prevent it.
		if(YSTRUE==visited.IsIncluded(state.plHd))
		{
			return YSERR;
		}
		visited.Add(state.plHd);

		if((state.pos-cen).GetLength()>cond.diameter)
		{
			return YSERR;
		}
		if(YSTRUE==crawler.reachedDeadEnd)
		{
			return YSERR;
		}
	}

	if(3<=pathPos.GetN())
	{
		prt.cen=sum/(double)pathPos.GetN();
		prt.seedPlHd=plHd0;
		prt.plKey.CleanUp();
		for(auto plHd : pathPlHd)
		{
			prt.plKey.Add(shl.GetSearchKey(plHd));
		}
		prt.path.MoveFrom(pathPos);
		prt.axis=axis;
		return YSOK;
	}

	return YSERR;
}

void YsShellExt_ProtrusionFinder::IdentifyConstVertexOnBoundary(const YsShellExt &shl)
{
	for(auto &p : protrusion)
	{
		p.MakeBoundaryConstraint(shl);
	}
}

const YsConstArrayMask <YsShellExt_ProtrusionFinder::Protrusion> YsShellExt_ProtrusionFinder::GetProtrusion(void) const
{
	YsConstArrayMask <Protrusion> mask(protrusion.GetN(),protrusion);
	return mask;
}

YSRESULT YsShellExt_ProtrusionFinder::OptimizeAxis(Protrusion &prt,const YsShellExt &shl,SearchCondition cond)
{
	YSRESULT res=YSERR;
	auto curErr=CalculateAxisNormalError(prt,shl);

	auto plNom0=shl.GetNormal(prt.seedPlHd);

	for(;;)
	{
		auto allPlHd=prt.PlHd(shl);
		YsArray <YsVec3> plNom(allPlHd.GetN(),nullptr);
		for(auto idx : allPlHd.AllIndex())
		{
			plNom[idx]=shl.GetNormal(allPlHd[idx]);
		}
		YsVec3 newAxis;
		YsFindLeastSquareFittingPlaneNormal(newAxis,plNom);

		auto newDir=plNom0^newAxis;

		Protrusion newPrt;
		if(YSOK!=newDir.Normalize() ||
		   YSOK!=FindFromPolygonOneDirection(newPrt,shl,prt.seedPlHd,cond,newAxis,newDir))
		{
			break;
		}

		auto newErr=CalculateAxisNormalError(newPrt,shl);
		if(newErr>=curErr)
		{
			break;
		}

		curErr=newErr;
		prt=(Protrusion &&)newPrt;
	}
	return res;
}

double YsShellExt_ProtrusionFinder::CalculateAxisNormalError(const Protrusion &prt,const YsShellExt &shl) const
{
	// Axis & Normal must be perpendicular -> axis*nom should be close to zero.
	double maxDot=0.0;

	for(auto plHd : prt.PlHd(shl))
	{
		auto nom=shl.GetNormal(plHd);
		YsMakeGreater(maxDot,fabs(nom*prt.axis));
	}

	return (YsPi/2.0)-acos(maxDot);
}

void YsShellExt_ProtrusionFinder::AugmentByFaceGroup(Protrusion &prt,const YsShellExt &shl) const
{
	YsShellExt::FaceGroupStore allFgHd(shl);
	for(auto plHd : prt.PlHd(shl))
	{
		auto fgHd=shl.FindFaceGroupFromPolygon(plHd);
		if(nullptr!=fgHd)
		{
			allFgHd.Add(fgHd);
		}
	}

	prt.plKey.CleanUp();
	for(auto fgHd : allFgHd)
	{
		for(auto plHd : shl.GetFaceGroup(fgHd))
		{
			prt.plKey.Add(shl.GetSearchKey(plHd));
		}
	}
}

void YsShellExt_ProtrusionFinder::CacheBoundary(Protrusion &prt,const YsShellExt &shl) const
{
	YsShellExt_BoundaryInfo bi;
	bi.MakeInfo(shl.Conv(),prt.PlHd(shl));
	bi.CacheContour(shl.Conv());
	bi.ReorientContour(shl.Conv());

	auto boundary=bi.GetContourAll();
	prt.boundary.Resize(boundary.GetN());
	for(auto idx : prt.boundary.AllIndex())
	{
		prt.boundary[idx].CleanUp();
		prt.boundary[idx].vtHd.MoveFrom(boundary[idx]);
	}
}

YSRESULT YsShellExt_ProtrusionFinder::CheckTaper(const Protrusion &prt,const YsShellExt &shl,SearchCondition cond) const
{
	for(auto plHd : prt.PlHd(shl))
	{
		auto nom=YsShell_CalculateNormal(shl.Conv(),plHd);
		auto angleOff=YsPi/2.0-acos(YsSmaller(fabs(nom*prt.axis),1.0));
		if(cond.taperAngleThr<angleOff)
		{
			return YSERR;
		}
	}
	return YSOK;
}

YSRESULT YsShellExt_ProtrusionFinder::CheckAngularCoverageAroundAxis(const Protrusion &prt,const YsShellExt &shl) const
{
	bool covered[360];
	for(auto &c : covered)
	{
		c=false;
	}

	const YsVec3 xVec=YsUnitVector(prt.axis.GetArbitraryPerpendicularVector());

	YsShellPolygonStore inProtrusion(shl.Conv());
	inProtrusion.Add(prt.PlHd(shl));

	for(auto plHd : prt.PlHd(shl))
	{
		auto nom=shl.GetNormal(plHd);
		double a0=acos(YsBound(xVec*nom,-1.0,1.0));
		if(prt.axis*(xVec^nom)<0.0)
		{
			a0=YsPi*2.0-a0;
		}

		for(int edIdx=0; edIdx<shl.GetPolygonNumVertex(plHd); ++edIdx)
		{
			auto neiPlHd=shl.GetNeighborPolygon(plHd,edIdx);
			if(nullptr!=neiPlHd && YSTRUE==inProtrusion.IsIncluded(neiPlHd))
			{
				auto neiNom=shl.GetNormal(neiPlHd);

				double da=acos(YsBound(nom*neiNom,-1.0,1.0));

				if(0.0<=prt.axis*(nom^neiNom))
				{
					// Covers positive side of a0
					for(int i=(int)YsRadToDeg(a0); i<=(int)YsRadToDeg(a0+da); ++i)
					{
						covered[i%360]=true;
					}
				}
				else
				{
					// Covers negative side of a0
					for(int i=(int)YsRadToDeg(a0-da); i<=(int)YsRadToDeg(a0); ++i)
					{
						covered[(i+360)%360]=true;
					}
				}
			}
		}
	}

	for(auto c : covered)
	{
		if(true!=c)
		{
			return YSERR;
		}
	}
	return YSOK;
}

YSRESULT YsShellExt_ProtrusionFinder::RecheckDiameter(const Protrusion &prt,const YsShellExt &shl,SearchCondition cond) const
{
	for(auto plHd : prt.PlHd(shl))
	{
		YsArray <YsVec3,4> plVtPos;
		shl.GetPolygon(plVtPos,plHd);

		for(auto p : plVtPos)
		{
			auto d=YsGetPointLineDistance3(prt.cen,prt.cen+prt.axis,p);
			if(cond.diameter/2<d)
			{
				return YSERR;
			}
		}
	}
	return YSOK;
}

void YsShellExt_ProtrusionFinder::CacheVolume(Protrusion &prt,const YsShellExt &shl) const
{
	double v=0.0;

	for(auto plHd : prt.PlHd(shl))
	{
		YsArray <YsVec3,4> plVtPos;
		shl.GetPolygon(plVtPos,plHd);

		for(int i=2; i<plVtPos.GetN(); ++i)
		{
			auto v1=plVtPos[i-1]-plVtPos[0];
			auto v2=plVtPos[i  ]-plVtPos[0];
			v+=plVtPos[0]*(v1^v2);
		}
	}
	for(auto &c : prt.boundary)
	{
		auto v0=shl.GetVertexPosition(c.vtHd[0]);
		for(int i=2; i<c.vtHd.GetN(); ++i)
		{
			auto v1=shl.GetVertexPosition(c.vtHd[i-1])-v0;
			auto v2=shl.GetVertexPosition(c.vtHd[i  ])-v0;
			v+=v0*(v1^v2);
		}
	}

	prt.volume=v/6.0;
	printf("%lf\n",prt.volume);
}


void YsShellExt_ProtrusionFinder::CalculateAftermath(const YsShellExt &shl)
{
	YsShellPolygonStore visited(shl.Conv());

	aftermath.CleanUp();

	int chunkIdx=0;
	for(auto &chunk : chunkPlKey)
	{
		Aftermath am;
		am.chunkIdx=chunkIdx;
		for(auto plKey : chunk)
		{
			auto plHd=shl.FindPolygon(plKey);
			if(nullptr!=plHd && YSTRUE!=visited.IsIncluded(plHd))
			{
				YsShellExt_TrackingUtil::Option opt;
				opt.crossNonManifoldEdge=YSTRUE;
				auto subChunk=YsShellExt_TrackingUtil::MakeEdgeConnectedPolygonGroup(shl.Conv(),plHd,opt,nullptr);

				double area=0.0;
				for(auto plHd : subChunk)
				{
					visited.Add(plHd);
					area+=shl.GetPolygonArea(plHd);
				}
				am.subChunk.Increment();
				am.subChunk.Last().plHd.MoveFrom(subChunk);
				am.subChunk.Last().area=area;
			}
		}

		am.largestSubChunkIdx=0;
		for(int idx=1; idx<am.subChunk.GetN(); ++idx)
		{
			if(am.subChunk[am.largestSubChunkIdx].area<am.subChunk[idx].area)
			{
				am.largestSubChunkIdx=idx;
			}
		}

		++chunkIdx;

		aftermath.Add((Aftermath &&)am);
	}
}

const YsConstArrayMask <YsShellExt_ProtrusionFinder::Aftermath> YsShellExt_ProtrusionFinder::GetAftermath(void) const
{
	return aftermath;
}

class YsShellExt_ProtrusionFinder::UngroupedPolygonEnclosedByConstEdge : public YsShellExt::Condition
{
public:
	virtual YSRESULT TestPolygon(const YsShellExt &shl,YsShellPolygonHandle plHd) const
	{
		if(nullptr==shl.FindFaceGroupFromPolygon(plHd))
		{
			return YSOK;
		}
		return YSERR;
	}
	virtual YSRESULT TestEdge(const YsShellExt &shl,YsShellVertexHandle edVtHd0,YsShellVertexHandle edVtHd1) const
	{
		if(0==shl.GetNumConstEdgeUsingEdgePiece(edVtHd0,edVtHd1))
		{
			return YSOK;
		}
		return YSERR;
	}
};

YsArray <YsShell::PolygonHandle> YsShellExt_ProtrusionFinder::FindUngroupedPolygonSeparatedByConstEdge(const YsShellExt &shl,YsShell::PolygonHandle seedPlHd) const
{
	UngroupedPolygonEnclosedByConstEdge cond;
	return YsShellExt_TrackingUtil::MakeEdgeConnectedPolygonGroup(shl.Conv(),seedPlHd,&cond);
}

