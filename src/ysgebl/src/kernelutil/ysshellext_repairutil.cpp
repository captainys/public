#include "ysshellext_repairutil.h"



void YsShellExt_IdenticalPolygonRemover::MakeDuplicatePolygonList(const YsShellExt &shl,YSBOOL takeReverse)
{
	toDel.CleanUp();
	toDel.SetShell(shl.Conv());

	for(auto plHd : shl.AllPolygon())
	{
		auto plVtHd=shl.GetPolygonVertex(plHd);
		for(auto plHdCmp : shl.FindPolygonFromVertex(plVtHd[0]))
		{
			if(YSTRUE!=toDel.IsIncluded(plHdCmp) &&
			   shl.GetSearchKey(plHd)<shl.GetSearchKey(plHdCmp))
			{
				auto plVtHdCmp=shl.GetPolygonVertex(plHdCmp);
				if(plVtHd.size()==plVtHdCmp.size() &&
				   YSTRUE==YsCheckSamePolygon<YsShell::VertexHandle>(plVtHd.size(),plVtHd.data(),plVtHdCmp.data(),takeReverse))
				{
					toDel.Add(plHdCmp);
				}
			}
		}
	}
}

YSBOOL YsShellExt_FlatTriangleKiller::IsFlatTriangle(const YsShellExt &shl,YsShell::PolygonHandle plHd) const
{
	auto plVtPos=shl.GetPolygonVertexPosition(plHd);
	switch(classifier)
	{
	case CANNOT_COMPUTE_NORMAL:
		{
			YsVec3 nom;
			if(YSOK!=YsGetAverageNormalVector(nom,plVtPos))
			{
				return YSTRUE;
			}
		}
		break;
	case NARROWER_THAN_TOLERANCE:
		if(0<plVtPos.size())
		{
			YsVec3 longest[2]={plVtPos[0],plVtPos[0]};
			double lMax=0;
			for(YSSIZE_T idx=0; idx<plVtPos.size(); ++idx)
			{
				double l=(plVtPos.GetCyclic(idx+1)-plVtPos[idx]).GetSquareLength();
				if(lMax<l)
				{
					lMax=l;
					longest[0]=plVtPos[idx];
					longest[1]=plVtPos.GetCyclic(idx+1);
				}
			}
			if(sqrt(lMax)<YsTolerance)
			{
				return YSTRUE;
			}
			else
			{
				for(auto p : plVtPos)
				{
					double d=YsGetPointLineDistance3(longest[0],longest[1],p);
					if(YsTolerance<d)
					{
						return YSFALSE;
					}
				}
				return YSTRUE;
			}
		}
		break;
	}
	return YSFALSE;
}
YSBOOL YsShellExt_FlatTriangleKiller::IsShortestEdgeSafeToCollapse(YsShell_CollapseInfo &info,const YsShellExt &shl,YsShell::PolygonHandle plHd) const
{
	auto plVtHd=shl.GetPolygonVertex(plHd);
	if(3==plVtHd.size())
	{
		int shortestIndex=0;
		double shortestEdgeLength=shl.GetEdgeLength(plVtHd[1],plVtHd[0]);
		for(int i=1; i<3; ++i)
		{
			double l=shl.GetEdgeLength(plVtHd.GetCyclic(i+1),plVtHd[i]);
			if(shortestEdgeLength>l)
			{
				shortestIndex=i;
				shortestEdgeLength=l;
			}
		}

		if(shortestEdgeLength<YsTolerance)
		{
			if(YSOK==info.MakeInfo(shl.Conv(),plVtHd[shortestIndex],plVtHd.GetCyclic(shortestIndex+1)))
			{
				auto topoChg=info.CheckTopologyChange(shl.Conv());
				if(YSTRUE!=topoChg.newOverUsedEdge)
				{
					return YSTRUE;
				}
			}
		}
	}

	return YSFALSE;
}
YSBOOL YsShellExt_FlatTriangleKiller::IsLongestEdgeSafeToSwap(YsShell_SwapInfo &info,const YsShellExt &shl,YsShell::PolygonHandle plHd) const
{
	auto plVtHd=shl.GetPolygonVertex(plHd);
	if(3==plVtHd.size())
	{
		int longestIndex=0;
		double longestEdgeLength=shl.GetEdgeLength(plVtHd[1],plVtHd[0]);
		for(int i=1; i<3; ++i)
		{
			double l=shl.GetEdgeLength(plVtHd.GetCyclic(i+1),plVtHd[i]);
			if(longestEdgeLength<l)
			{
				longestIndex=i;
				longestEdgeLength=l;
			}
		}

		YsShellExt::Edge edge(plVtHd[longestIndex],plVtHd.GetCyclic(longestIndex+1));
		if(YSOK==info.MakeInfo(shl.Conv(),edge))
		{
			YsShell::PolygonHandle neiPlHd=nullptr;
			if(plHd==info.triPlHd[0])
			{
				neiPlHd=info.triPlHd[1];
			}
			else if(plHd==info.triPlHd[1])
			{
				neiPlHd=info.triPlHd[0];
			}
			else
			{
				return YSFALSE;
			}

			if(YSTRUE!=IsFlatTriangle(shl,neiPlHd))
			{
				return YSTRUE;
			}
		}
	}
	return YSFALSE;
}
YSBOOL YsShellExt_FlatTriangleKiller::IsLongestEdgeSafeToMerge(YsShell::Edge &edge,const YsShellExt &shl,YsShell::PolygonHandle plHd) const
{
	// Working on it
	return YSFALSE;
}
