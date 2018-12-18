/* ////////////////////////////////////////////////////////////

File Name: ysshellext_astar.cpp
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

#include "ysshellext_astar.h"




// Just implementing the algorithm in Wikipedia.
YSSIZE_T YsShellExt_AstarSearch::OpenSet::GetN(void) const
{
	return minimumFScoreFinder.GetN();
}
void YsShellExt_AstarSearch::OpenSet::SetShell(const YsShell &shl)
{
	vtHdToAVLTreeNode.SetShell(shl);
}
void YsShellExt_AstarSearch::OpenSet::Update(YsShell::VertexHandle vtHd,const double fScore)
{
	auto treeNdHdPtr=vtHdToAVLTreeNode[vtHd];
	if(nullptr!=treeNdHdPtr)
	{
		minimumFScoreFinder.Delete(*treeNdHdPtr);
	}
	auto treeHd=minimumFScoreFinder.Insert(fScore,vtHd);
	vtHdToAVLTreeNode.SetAttrib(vtHd,treeHd);
}
void YsShellExt_AstarSearch::OpenSet::DeleteFirst(void)
{
	auto treeHd=minimumFScoreFinder.First();
	auto vtHd=minimumFScoreFinder.GetValue(treeHd);
	vtHdToAVLTreeNode.DeleteAttrib(vtHd);
	minimumFScoreFinder.Delete(treeHd);
}
YSBOOL YsShellExt_AstarSearch::OpenSet::IsIncluded(YsShell::VertexHandle vtHd) const
{
	if(nullptr!=vtHdToAVLTreeNode[vtHd])
	{
		return YSTRUE;
	}
	return YSFALSE;
}
YsShell::VertexHandle YsShellExt_AstarSearch::OpenSet::MinimumFScoreVtHd(void) const
{
	auto treeHd=minimumFScoreFinder.First();
	return minimumFScoreFinder.GetValue(treeHd);
}




YsArray <YsShell::VertexHandle> YsShellExt_AstarSearch::FindShortestPath(const YsShellExt &shl,YsShell::VertexHandle startVtHd,YsShell::VertexHandle goalVtHd)
{
	YsShellVertexStore closedSet(shl.Conv());
	YsShellVertexAttribTable <YsShell::VertexHandle> cameFrom(shl.Conv());

	f_score.CleanUp();
	g_score.CleanUp();
	f_score.SetShell(shl.Conv());
	g_score.SetShell(shl.Conv());

	g_score.SetAttrib(startVtHd,0.0);
	f_score.SetAttrib(startVtHd,HeuristicCostEstimate(shl,startVtHd,goalVtHd));

	OpenSet openSet;
	openSet.SetShell(shl.Conv());
	openSet.Update(startVtHd,FScore(shl.Conv(),startVtHd));

	while(0<openSet.GetN())
	{
		auto currentVtHd=openSet.MinimumFScoreVtHd();
		if(currentVtHd==goalVtHd)
		{
			YsArray <YsShell::VertexHandle> path;
			path.Add(currentVtHd);
			for(;;)
			{
				auto prevVtHdPtr=cameFrom[currentVtHd];
				if(nullptr==prevVtHdPtr)
				{
					break;
				}
				path.Add(*prevVtHdPtr);
				currentVtHd=*prevVtHdPtr;
			}
			path.Invert();
			return path;
		}

		openSet.DeleteFirst();
		closedSet.Add(currentVtHd);

		auto allNeighborVtHd=GetNeighbor(shl,currentVtHd);
		for(auto neighborVtHd : allNeighborVtHd)
		{
			if(YSTRUE==closedSet.IsIncluded(neighborVtHd))
			{
				continue;
			}
			auto tentative_g_score=GScore(shl.Conv(),currentVtHd)+GetStepDist(shl.Conv(),currentVtHd,neighborVtHd);

			if(YSTRUE!=openSet.IsIncluded(neighborVtHd) || tentative_g_score<GScore(shl.Conv(),neighborVtHd))
			{
				cameFrom.SetAttrib(neighborVtHd,currentVtHd);
				g_score.SetAttrib(neighborVtHd,tentative_g_score);

				auto fs=tentative_g_score+HeuristicCostEstimate(shl.Conv(),neighborVtHd,goalVtHd);
				f_score.SetAttrib(neighborVtHd,fs);
				openSet.Update(neighborVtHd,fs);
			}
		}
	}

	YsArray <YsShell::VertexHandle> emptyPath;
	return emptyPath;
}



/* virtual */ double YsShellExt_AstarSearch::HeuristicCostEstimate(const YsShellExt &shl,YsShell::VertexHandle fromVtHd,YsShell::VertexHandle toVtHd) const
{
	return shl.GetEdgeLength(fromVtHd,toVtHd);
}

/* virtual */ YsArray <YsShell::VertexHandle> YsShellExt_AstarSearch::GetNeighbor(const YsShellExt &shl,YsShell::VertexHandle fromVtHd) const
{
	return shl.GetConnectedVertex(fromVtHd);
}

/* virtual */ double YsShellExt_AstarSearch::GetStepDist(const YsShellExt &shl,YsShell::VertexHandle fromVtHd,YsShell::VertexHandle toVtHd) const
{
	return shl.GetEdgeLength(fromVtHd,toVtHd);
}

double YsShellExt_AstarSearch::FScore(const YsShellExt &shl,YsShell::VertexHandle vtHd) const
{
	const double *scorePtr=f_score[shl.GetSearchKey(vtHd)];
	if(nullptr!=scorePtr)
	{
		return *scorePtr;
	}
	return YsInfinity;
}

double YsShellExt_AstarSearch::GScore(const YsShellExt &shl,YsShell::VertexHandle vtHd) const
{
	const double *scorePtr=g_score[shl.GetSearchKey(vtHd)];
	if(nullptr!=scorePtr)
	{
		return *scorePtr;
	}
	return YsInfinity;
}



////////////////////////////////////////////////////////////



YSSIZE_T YsShellExt_AstarSearch_Polygon::OpenSet::GetN(void) const
{
	return minimumFScoreFinder.GetN();
}
void YsShellExt_AstarSearch_Polygon::OpenSet::SetShell(const YsShell &shl)
{
	plHdToAVLTreeNode.SetShell(shl);
}
void YsShellExt_AstarSearch_Polygon::OpenSet::Update(YsShell::PolygonHandle plHd,const double fScore)
{
	auto treeNdHdPtr=plHdToAVLTreeNode[plHd];
	if(nullptr!=treeNdHdPtr)
	{
		minimumFScoreFinder.Delete(*treeNdHdPtr);
	}
	auto treeHd=minimumFScoreFinder.Insert(fScore,plHd);
	plHdToAVLTreeNode.SetAttrib(plHd,treeHd);
}
void YsShellExt_AstarSearch_Polygon::OpenSet::DeleteFirst(void)
{
	auto treeHd=minimumFScoreFinder.First();
	auto plHd=minimumFScoreFinder.GetValue(treeHd);
	plHdToAVLTreeNode.DeleteAttrib(plHd);
	minimumFScoreFinder.Delete(treeHd);
}
YSBOOL YsShellExt_AstarSearch_Polygon::OpenSet::IsIncluded(YsShell::PolygonHandle plHd) const
{
	if(nullptr!=plHdToAVLTreeNode[plHd])
	{
		return YSTRUE;
	}
	return YSFALSE;
}
YsShell::PolygonHandle YsShellExt_AstarSearch_Polygon::OpenSet::MinimumFScorePlHd(void) const
{
	auto treeHd=minimumFScoreFinder.First();
	return minimumFScoreFinder.GetValue(treeHd);
}




YsArray <YsShell::PolygonHandle> YsShellExt_AstarSearch_Polygon::FindShortestPath(const YsShellExt &shl,YsShell::PolygonHandle startPlHd,YsShell::PolygonHandle goalPlHd)
{
	YsShellPolygonStore closedSet(shl.Conv());
	YsShellPolygonAttribTable <YsShell::PolygonHandle> cameFrom(shl.Conv());

	f_score.CleanUp();
	g_score.CleanUp();
	f_score.SetShell(shl.Conv());
	g_score.SetShell(shl.Conv());

	g_score.SetAttrib(startPlHd,0.0);
	f_score.SetAttrib(startPlHd,HeuristicCostEstimate(shl,startPlHd,goalPlHd));

	OpenSet openSet;
	openSet.SetShell(shl.Conv());
	openSet.Update(startPlHd,FScore(shl.Conv(),startPlHd));

	while(0<openSet.GetN())
	{
		auto currentPlHd=openSet.MinimumFScorePlHd();
		if(currentPlHd==goalPlHd)
		{
			YsArray <YsShell::PolygonHandle> path;
			path.Add(currentPlHd);
			for(;;)
			{
				auto prevPlHdPtr=cameFrom[currentPlHd];
				if(nullptr==prevPlHdPtr)
				{
					break;
				}
				path.Add(*prevPlHdPtr);
				currentPlHd=*prevPlHdPtr;
			}
			path.Invert();
			return path;
		}

		openSet.DeleteFirst();
		closedSet.Add(currentPlHd);

		auto allNeighborPlHd=GetNeighbor(shl,currentPlHd);
		for(auto neighborPlHd : allNeighborPlHd)
		{
			if(YSTRUE==closedSet.IsIncluded(neighborPlHd))
			{
				continue;
			}
			auto tentative_g_score=GScore(shl.Conv(),currentPlHd)+GetStepDist(shl.Conv(),currentPlHd,neighborPlHd);

			if(YSTRUE!=openSet.IsIncluded(neighborPlHd) || tentative_g_score<GScore(shl.Conv(),neighborPlHd))
			{
				cameFrom.SetAttrib(neighborPlHd,currentPlHd);
				g_score.SetAttrib(neighborPlHd,tentative_g_score);

				auto fs=tentative_g_score+HeuristicCostEstimate(shl.Conv(),neighborPlHd,goalPlHd);
				f_score.SetAttrib(neighborPlHd,fs);
				openSet.Update(neighborPlHd,fs);
			}
		}
	}

	YsArray <YsShell::PolygonHandle> emptyPath;
	return emptyPath;
}



/* virtual */ double YsShellExt_AstarSearch_Polygon::HeuristicCostEstimate(const YsShellExt &shl,YsShell::PolygonHandle fromPlHd,YsShell::PolygonHandle toPlHd) const
{
	auto cen0=shl.GetCenter(fromPlHd);
	auto cen1=shl.GetCenter(toPlHd);
	return (cen1-cen0).GetLength();
}

/* virtual */ YsArray <YsShell::PolygonHandle> YsShellExt_AstarSearch_Polygon::GetNeighbor(const YsShellExt &shl,YsShell::PolygonHandle fromPlHd) const
{
	YsArray <YsShell::PolygonHandle> neiPlHd;

	auto nPlVt=shl.GetPolygonNumVertex(fromPlHd);
	for(decltype(nPlVt) i=0; i<nPlVt; ++i)
	{
		neiPlHd.Add(shl.GetNeighborPolygon(fromPlHd,i));
	}

	return neiPlHd;
}

/* virtual */ double YsShellExt_AstarSearch_Polygon::GetStepDist(const YsShellExt &shl,YsShell::PolygonHandle fromPlHd,YsShell::PolygonHandle toPlHd) const
{
	auto cen0=shl.GetCenter(fromPlHd);
	auto cen1=shl.GetCenter(toPlHd);
	return (cen1-cen0).GetLength();
}

double YsShellExt_AstarSearch_Polygon::FScore(const YsShellExt &shl,YsShell::PolygonHandle plHd) const
{
	const double *scorePtr=f_score[shl.GetSearchKey(plHd)];
	if(nullptr!=scorePtr)
	{
		return *scorePtr;
	}
	return YsInfinity;
}

double YsShellExt_AstarSearch_Polygon::GScore(const YsShellExt &shl,YsShell::PolygonHandle plHd) const
{
	const double *scorePtr=g_score[shl.GetSearchKey(plHd)];
	if(nullptr!=scorePtr)
	{
		return *scorePtr;
	}
	return YsInfinity;
}
