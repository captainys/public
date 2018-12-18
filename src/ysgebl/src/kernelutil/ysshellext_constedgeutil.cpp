/* ////////////////////////////////////////////////////////////

File Name: ysshellext_constedgeutil.cpp
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

#include "ysshellext_constedgeutil.h"

YsShellExt::ConstEdgeAndPos YsShellExt_FindNearestPointOnConstEdgeInPolygon(
    const YsShellExt &shl,YsShellPolygonHandle plHd,const YsVec3 &from)
{
	int nPlVt;
	const YsShellVertexHandle *plVtHdPtr;
	shl.GetPolygon(nPlVt,plVtHdPtr,plHd);

	YsConstArrayMask <YsShellVertexHandle> plVtHd(nPlVt,plVtHdPtr);

	YsShellExt::ConstEdgeAndPos nearCe;
	double nearDist=0.0;
	nearCe.ceHd=NULL;
	for(auto vtIdx : plVtHd.AllIndex())
	{
		const YsShellVertexHandle edVtHd[2]=
		{
			plVtHd[vtIdx],
			plVtHd.GetCyclic(vtIdx+1)
		};
		auto edCeHd=shl.FindConstEdgeFromEdgePiece(edVtHd);
		if(0<edCeHd.GetN())
		{
			const YsVec3 edVtPos[2]=
			{
				shl.GetVertexPosition(edVtHd[0]),
				shl.GetVertexPosition(edVtHd[1])
			};

			const auto nearPos=YsGetNearestPointOnLinePiece(edVtPos,from);
			const double dist=(nearPos-from).GetSquareLength();
			if(NULL==nearCe.ceHd || dist<nearDist)
			{
				nearCe.ceHd=edCeHd[0];
				nearCe.pos=nearPos;
				nearDist=dist;
			}
		}
	}

	return nearCe;
}

YsShellExt::ConstEdgeStore YsShellExt_FindConstEdgeWithinRadiusFromVertex(
	const YsShellExt &shl,YsShell::VertexHandle fromVtHd,double radius)
{
	YsShellExt::ConstEdgeStore ce(shl.Conv());

	YsShellPolygonStore visited(shl.Conv());
	YsArray <YsShell::PolygonHandle> todo;

	todo=shl.FindPolygonFromVertex(fromVtHd);
	visited.Add(todo);

	auto c=shl.GetVertexPosition(fromVtHd);

	while(0<todo.GetN())
	{
		auto plHd=todo.Last();
		todo.DeleteLast();

		auto plVtHd=shl.GetPolygonVertex(plHd);
		for(auto edIdx : plVtHd.AllIndex())
		{
			YsShell::VertexHandle edVtHd[2]={plVtHd[edIdx],plVtHd.GetCyclic(edIdx+1)};
			auto p0=shl.GetVertexPosition(edVtHd[0]);
			auto p1=shl.GetVertexPosition(edVtHd[1]);

			YSBOOL in=YSFALSE;
			if((p0-c)*(p0-c)<=radius*radius ||
			   (p1-c)*(p1-c)<=radius*radius)
			{
				in=YSTRUE;
			}
			else
			{
				YsVec3 i0,i1;
				if(YSOK==YsComputeSphereLineIntersection(i0,i1,c,radius,p0,p1))
				{
					if(YSTRUE==YsCheckInBetween3(i0,p0,p1) ||
					   YSTRUE==YsCheckInBetween3(i1,p0,p1))
					{
						in=YSTRUE;
					}
				}
			}

			if(YSTRUE==in)
			{
				ce.Add(shl.FindConstEdgeFromEdgePiece(edVtHd));

				auto neiPlHd=shl.GetNeighborPolygon(plHd,edIdx);
				if(nullptr!=neiPlHd && YSTRUE!=visited.IsIncluded(neiPlHd))
				{
					todo.Add(neiPlHd);
					visited.Add(neiPlHd);
				}
			}
		}
	}

	return ce;
}

YsShell::EdgeAndPos YsShellExt_FindNearestPointOnConstEdge(
    const YsShellExt &shl,YsShellExt::ConstEdgeHandle ceHd,const YsVec3 &from)
{
	double minDist=YsInfinity;
	YsShell::EdgeAndPos edgeAndPos;
	edgeAndPos.edVtHd[0]=nullptr;
	edgeAndPos.edVtHd[1]=nullptr;

	YSBOOL isLoop;
	YsArray <YsShell::VertexHandle> ceVtHd;
	shl.GetConstEdge(ceVtHd,isLoop,ceHd);

	for(int idx=0; idx<ceVtHd.GetN(); ++idx)
	{
		auto vtPos=shl.GetPosition(ceVtHd[idx]);
		auto dist=(from-vtPos).GetSquareLength();
		if(nullptr==edgeAndPos.edVtHd[0] || dist<minDist)
		{
			minDist=dist;
			edgeAndPos.edVtHd[0]=ceVtHd[idx];
			edgeAndPos.edVtHd[1]=(idx<ceVtHd.GetN()-1 ? ceVtHd[idx+1] : ceVtHd[idx-1]);
			edgeAndPos.pos=vtPos;
		}
	}

	if(YSTRUE==isLoop)
	{
		ceVtHd.Add(ceVtHd[0]);
	}
	for(int idx=0; idx<ceVtHd.GetN()-1; ++idx)
	{
		const YsShell::VertexHandle edVtHd[2]={ceVtHd[idx],ceVtHd[idx+1]};
		const YsVec3 edVtPos[2]={shl.GetVertexPosition(edVtHd[0]),shl.GetVertexPosition(edVtHd[1])};

		auto nearPos=YsGetNearestPointOnLine3(edVtPos,from);
		if(edVtPos[0]!=nearPos &&
		   edVtPos[1]!=nearPos &&
		   YSTRUE==YsCheckInBetween3(nearPos,edVtPos))
		{
			auto dist=(nearPos-from).GetSquareLength();
			if(nullptr==edgeAndPos.edVtHd[0] || dist<minDist)
			{
				minDist=dist;
				edgeAndPos.edVtHd[0]=edVtHd[0];
				edgeAndPos.edVtHd[1]=edVtHd[1];
				edgeAndPos.pos=nearPos;
			}
		}
	}

	return edgeAndPos;
}

////////////////////////////////////////////////////////////

YsShellExt_ProximityConstEdgeFinder::YsShellExt_ProximityConstEdgeFinder()
{
	shlPtr=nullptr;
	stopAtConstEdge=YSTRUE;
	CleanUp();
}
void YsShellExt_ProximityConstEdgeFinder::CleanUp(void)
{
	visitedCe.CleanUp();
	foundCe.CleanUp();
	visitedPlg.CleanUp();
}
void YsShellExt_ProximityConstEdgeFinder::SetShell(const YsShellExt &shl)
{
	shlPtr=&shl;
	visitedCe.SetShell(shl.Conv());
	foundCe.SetShell(shl.Conv());
	visitedPlg.SetShell(shl.Conv());
}
void YsShellExt_ProximityConstEdgeFinder::SetStopAtConstEdge(YSBOOL sw)
{
	stopAtConstEdge=sw;
}
const YsShellExt::ConstEdgeStore &YsShellExt_ProximityConstEdgeFinder::Search(YsShell::VertexHandle fromVtHd,const double radius)
{
	CleanUp();
	return ResumeSearch(fromVtHd,radius);
}
const YsShellExt::ConstEdgeStore &YsShellExt_ProximityConstEdgeFinder::Search(const YsShell::Edge &fromEdge,const YsVec3 &startPos,const double radius)
{
	CleanUp();
	return ResumeSearch(fromEdge,startPos,radius);
}
const YsShellExt::ConstEdgeStore &YsShellExt_ProximityConstEdgeFinder::ResumeSearch(YsShell::VertexHandle fromVtHd,const double radius)
{
	TransferFoundConstEdgeToVisitedConstEdge();
	auto startPos=shlPtr->GetVertexPosition(fromVtHd);

	YsArray <YsShell::PolygonHandle> seed=shlPtr->FindPolygonFromVertex(fromVtHd);
	for(auto idx : seed.ReverseIndex())
	{
		if(YSTRUE==visitedPlg.IsIncluded(seed[idx]))
		{
			seed.DeleteBySwapping(idx);
		}
	}

	Search(seed,startPos,radius);
	return foundCe;
}
const YsShellExt::ConstEdgeStore &YsShellExt_ProximityConstEdgeFinder::ResumeSearch(const YsShell::Edge &fromEdge,const YsVec3 &startPos,const double radius)
{
	TransferFoundConstEdgeToVisitedConstEdge();

	YsArray <YsShell::PolygonHandle> seed=shlPtr->FindPolygonFromEdgePiece(fromEdge);
	for(auto idx : seed.ReverseIndex())
	{
		if(YSTRUE==visitedPlg.IsIncluded(seed[idx]))
		{
			seed.DeleteBySwapping(idx);
		}
	}

	Search(seed,startPos,radius);
	return foundCe;
}

void YsShellExt_ProximityConstEdgeFinder::Search(YsArray <YsShell::PolygonHandle> &todo,const YsVec3 &startPos,const double radius)
{
	YsShellPolygonStore visited(shlPtr->Conv());
	visited.Add(todo);

	const auto &c=startPos;

	while(0<todo.GetN())
	{
		auto plHd=todo.Last();
		todo.DeleteLast();

		auto plVtHd=shlPtr->GetPolygonVertex(plHd);
		for(auto edIdx : plVtHd.AllIndex())
		{
			YsShell::VertexHandle edVtHd[2]={plVtHd[edIdx],plVtHd.GetCyclic(edIdx+1)};
			auto p0=shlPtr->GetVertexPosition(edVtHd[0]);
			auto p1=shlPtr->GetVertexPosition(edVtHd[1]);

			YSBOOL in=YSFALSE;
			if((p0-c)*(p0-c)<=radius*radius ||
			   (p1-c)*(p1-c)<=radius*radius)
			{
				in=YSTRUE;
			}
			else
			{
				YsVec3 i0,i1;
				if(YSOK==YsComputeSphereLineIntersection(i0,i1,c,radius,p0,p1))
				{
					if(YSTRUE==YsCheckInBetween3(i0,p0,p1) ||
					   YSTRUE==YsCheckInBetween3(i1,p0,p1))
					{
						in=YSTRUE;
					}
				}
			}

			if(YSTRUE==in)
			{
				auto edCeHd=shlPtr->FindConstEdgeFromEdgePiece(edVtHd);
				for(auto ceHd : edCeHd)
				{
					if(nullptr!=ceHd && YSTRUE!=visitedCe.IsIncluded(ceHd))
					{
						foundCe.Add(ceHd);
						visitedCe.Add(ceHd);
					}
				}

				if(0<edCeHd.GetN() && YSTRUE==stopAtConstEdge)
				{
					continue;
				}

				auto neiPlHd=shlPtr->GetNeighborPolygon(plHd,edIdx);
				if(nullptr!=neiPlHd && YSTRUE!=visitedPlg.IsIncluded(neiPlHd))
				{
					todo.Add(neiPlHd);
					visitedPlg.Add(neiPlHd);
				}
			}
		}
	}
}

void YsShellExt_ProximityConstEdgeFinder::TransferFoundConstEdgeToVisitedConstEdge(void)
{
	for(auto ceHd : foundCe)
	{
		visitedCe.Add(ceHd);
	}
	foundCe.CleanUp();
}
