/* ////////////////////////////////////////////////////////////

File Name: ysshellext_deformationevaluator.cpp
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

#include <ysvisited.h>

#include "ysshellext_geomcalc.h"
#include "ysshellext_deformationevaluator.h"

YsShellExt_DeformationEvaluator::YsShellExt_DeformationEvaluator()
{
	plNomCache=NULL;
	vtFgNomCache=NULL;
	CleanUp();
}

YsShellExt_DeformationEvaluator::~YsShellExt_DeformationEvaluator()
{
	CleanUp();
}

void YsShellExt_DeformationEvaluator::CleanUp(void)
{
	DeleteNormalCache();
}

void YsShellExt_DeformationEvaluator::DeleteNormalCache(void)
{
	if(nullptr!=plNomCache)
	{
		delete plNomCache;
		plNomCache=nullptr;
	}
	if(nullptr!=vtFgNomCache)
	{
		delete vtFgNomCache;
		vtFgNomCache=nullptr;
	}
}

void YsShellExt_DeformationEvaluator::MakePolygonNormalCache(const YsShellExt &shl)
{
	if(nullptr!=plNomCache)
	{
		delete plNomCache;
		plNomCache=nullptr;
	}
	plNomCache=new YsShellPolygonAttribTable <YsVec3>(shl.Conv());
	(*plNomCache)=YsShellExt_MakePolygonNormalTable(shl.Conv());
}
void YsShellExt_DeformationEvaluator::MakeVertexFaceGroupNormalCache(const YsShellExt &shl)
{
	if(nullptr!=vtFgNomCache)
	{
		delete vtFgNomCache;
		vtFgNomCache=nullptr;
	}
	vtFgNomCache=new YsShellExt::VertexFaceGroupAttribTable <YsVec3>(shl.Conv());
	(*vtFgNomCache)=YsShellExt_MakeVertexFaceGroupNormalTable(shl.Conv());
}

const YsShellExt::VertexFaceGroupAttribTable <YsVec3> *YsShellExt_DeformationEvaluator::GetVertexFaceGroupNormalCache(void) const
{
	return vtFgNomCache;
}

YsShellExt_DeformationEvaluator::EdgeCollapseEvaluationCache YsShellExt_DeformationEvaluator::MakeCacheForEdgeCollapse(const YsShellExt &shl,YsShellVertexHandle edVtHd0,YsShellVertexHandle edVtHd1) const
{
	EdgeCollapseEvaluationCache cache;

	auto edPlHd=shl.FindPolygonFromEdgePiece(edVtHd0,edVtHd1);

	YsVisited <YsShellExt,YsShellPolygonHandle> visited;
	visited.AddMulti(shl,edPlHd);

	YsShellVertexHandle edVtHd[2]={edVtHd0,edVtHd1};
	for(auto vtHd : edVtHd)
	{
		auto vtPlHd=shl.FindPolygonFromVertex(vtHd);
		for(auto plHd : vtPlHd)
		{
			if(YSTRUE!=visited.IsIncluded(shl,plHd))
			{
				visited.Add(shl,plHd);

				cache.plNomPairArray.Increment();
				cache.plNomPairArray.Last().plHd=plHd;
				cache.plNomPairArray.Last().nom=CalculatePreDeformationNormal(shl,plHd);;
			}
		}
	}

	return cache;
}

YsVec3 YsShellExt_DeformationEvaluator::CalculatePreDeformationNormal(const YsShellExt &shl,YsShellPolygonHandle plHd) const
{
	YsVec3 nom=YsOrigin();
	const YsVec3 *cached;

	if(nullptr!=vtFgNomCache)
	{
		auto fgHd=shl.FindFaceGroupFromPolygon(plHd);
		auto plVtHd=shl.GetPolygonVertex(plHd);
		for(auto vtHd : plVtHd)
		{
			cached=vtFgNomCache->Find(shl.GetSearchKey(vtHd),shl.GetSearchKey(fgHd));
			if(NULL!=cached)
			{
				nom+=*cached;
			}
		}
	}
	if(nullptr!=plNomCache && NULL!=(cached=(*plNomCache)[shl.GetSearchKey(plHd)]))
	{
		nom+=*cached;
	}

	if(YSOK!=nom.Normalize())
	{
		YsArray <YsVec3,4> plVtPos;
		shl.GetPolygon(plVtPos,plHd);
		YsGetAverageNormalVector(nom,plVtPos);
	}
	return nom;
}

YsShellExt_DeformationEvaluator::EdgeCollapseEvaluationCache YsShellExt_DeformationEvaluator::MakeCacheForEdgeCollapse(const YsShellExt &shl,const YsShellVertexHandle edVtHd[2]) const
{
	return MakeCacheForEdgeCollapse(shl,edVtHd[0],edVtHd[1]);
}
YsShellExt_DeformationEvaluator::EdgeCollapseEvaluationCache YsShellExt_DeformationEvaluator::MakeCacheForEdgeCollapse(const YsShellExt &shl,YsShell::Edge edge) const
{
	return MakeCacheForEdgeCollapse(shl,edge[0],edge[1]);
}

YsShell::PolygonAndValue YsShellExt_DeformationEvaluator::CalculateMaxNormalChangeAfterEdgeCollapse(const YsShellExt &shl,const EdgeCollapseEvaluationCache &cache,YsShellVertexHandle edVtHd0,YsShellVertexHandle edVtHd1,const YsVec3 &newVtPos) const
{
	double minCos=1.0;
	YsShell::PolygonHandle minCosPlHd=nullptr;

	for(auto &plNom : cache.plNomPairArray)
	{
		if(plNom.nom!=YsOrigin())
		{
			YsArray <YsVec3,4> plVtPos;
			auto plVtHd=shl.GetPolygonVertex(plNom.plHd);
			for(YSSIZE_T idx=0; idx<plVtHd.GetN(); ++idx)
			{
				if(plVtHd[idx]==edVtHd0 || plVtHd[idx]==edVtHd1)
				{
					plVtPos.Add(newVtPos);
				}
				else
				{
					plVtPos.Add(shl.GetVertexPosition(plVtHd[idx]));
				}
			}
			YsVec3 newNom;
			YsGetAverageNormalVector(newNom,plVtPos);

			auto c=plNom.nom*newNom;
			if(c<minCos)
			{
				minCos=c;
				minCosPlHd=plNom.plHd;
			}
		}
	}

	YsShell::PolygonAndValue min;
	min.plHd=minCosPlHd;
	min.value=acos(YsBound(minCos,-1.0,1.0));
	return min;
}
YsShell::PolygonAndValue YsShellExt_DeformationEvaluator::CalculateMaxNormalChangeAfterEdgeCollapse(const YsShellExt &shl,const EdgeCollapseEvaluationCache &cache,const YsShellVertexHandle edVtHd[2],const YsVec3 &newVtPos) const
{
	return CalculateMaxNormalChangeAfterEdgeCollapse(shl,cache,edVtHd[0],edVtHd[1],newVtPos);
}
YsShell::PolygonAndValue YsShellExt_DeformationEvaluator::CalculateMaxNormalChangeAfterEdgeCollapse(const YsShellExt &shl,const EdgeCollapseEvaluationCache &cache,YsShell::Edge edge,const YsVec3 &newVtPos) const
{
	return CalculateMaxNormalChangeAfterEdgeCollapse(shl,cache,edge[0],edge[1],newVtPos);
}

YsShellExt_DeformationEvaluator::MinimumDeformationCollapseDirection YsShellExt_DeformationEvaluator::SelectMinimumDeformationCollpaseDirection(const YsShellExt &shl,const YsArray <YsShellVertexHandle> &candidateVtHd,YsShellVertexHandle fromVtHd) const
{
	// Tolology Destruction Check:
	//   It's a bad sign if a path B-x-A exists and x does not belong to a polygon that is deleted in the collapse, where vertex A is being collapsed into vertex B.

	auto connVtHdFrom=shl.GetConnectedVertex(fromVtHd);

	// Garland & Heckbert 97.  I cannot tell how much penalty should be added for normal-vector change.
	// YsMatrix4x4 Kp(YSFALSE);
	// YsShellExt_CalculateQuadricErrorMetric(Kp,shl.Conv(),fromVtHd);

	MinimumDeformationCollapseDirection best;
	best.vtHd=NULL;
	best.maxNormalChange.value=YsInfinity;
	best.maxNormalChange.plHd=nullptr;
	for(auto canVtHd : candidateVtHd)
	{
		// Is it collapsible without breaking the topology?
		auto plHdToDel=shl.FindPolygonFromEdgePiece(fromVtHd,canVtHd);

		auto connVtHdTo=shl.GetConnectedVertex(canVtHd);
		for(auto connA : connVtHdFrom)
		{
			for(auto connB : connVtHdTo)
			{
				if(connA==connB)  // There is a path.  The question is if this path is part of a polygon to be deleted.
				{
					auto x=connA;
					YSBOOL used=YSFALSE;
					for(auto plHd : plHdToDel)
					{
						if(YSTRUE==shl.IsPolygonUsingVertex(plHd,x))
						{
							used=YSTRUE;
							break;
						}
					}
					if(YSTRUE!=used)  // No, it's going to create an unwanted non-manifold edge.
					{
						goto NEXTCANDIDATE;
					}
				}
			}
		}

		{
			// Garland & Heckbert 97, Section 6: Quadric Error Metric itself does not prevent flipping.
			// const double err=YsShellExt_CalculateQuadricError(Kp,shl.GetVertexPosition(canVtHd));

			// Normal-change
			auto info=MakeCacheForEdgeCollapse(shl.Conv(),fromVtHd,canVtHd);
			auto err=CalculateMaxNormalChangeAfterEdgeCollapse(shl.Conv(),info,fromVtHd,canVtHd,shl.GetVertexPosition(canVtHd));

			if(NULL==best.vtHd || err.value<best.maxNormalChange.value)
			{
				best.maxNormalChange=err;
				best.vtHd=canVtHd;
			}
		}

	NEXTCANDIDATE:
		;
	}

	return best;
}



YsShell::PolygonAndValue YsShellExt_DeformationEvaluator::CalculateMaxNormalChangeAfterEdgeCollapse(const YsShellExt &shl,YsShellVertexHandle edVtHd0,YsShellVertexHandle edVtHd1,const YsVec3 &newVtPos) const
{
	double minCos=1.0;
	YsShell::PolygonHandle minCosPlHd=nullptr;

	if(nullptr!=plNomCache)
	{
		for(int edVtIdx=0; edVtIdx<2; ++edVtIdx)
		{
			auto vtHd=(0==edVtIdx ? edVtHd0 : edVtHd1);
			for(auto plHd : shl.FindPolygonFromVertex(vtHd))
			{
				auto refNomPtr=(*plNomCache)[plHd];
				if(nullptr==refNomPtr)
				{
					continue;
				}
				auto refNom=*refNomPtr;

				if(refNom!=YsOrigin())
				{
					YsArray <YsVec3,4> plVtPos;
					auto plVtHd=shl.GetPolygonVertex(plHd);
					for(auto &vtHd : plVtHd)
					{
						if(edVtHd1==vtHd)
						{
							vtHd=edVtHd0;
						}
					}
					for(auto idx=plVtHd.GetN()-1; 0<=idx; --idx)
					{
						if(plVtHd[idx]==plVtHd.GetCyclic(idx+1))
						{
							plVtHd.Delete(idx);
						}
					}
					if(3>plVtHd.GetN())
					{
						continue;
					}

					for(YSSIZE_T idx=0; idx<plVtHd.GetN(); ++idx)
					{
						if(plVtHd[idx]==edVtHd0 || plVtHd[idx]==edVtHd1)
						{
							plVtPos.Add(newVtPos);
						}
						else
						{
							plVtPos.Add(shl.GetVertexPosition(plVtHd[idx]));
						}
					}
					YsVec3 newNom;
					YsGetAverageNormalVector(newNom,plVtPos);

					auto c=refNom*newNom;
					if(c<minCos)
					{
						minCos=c;
						minCosPlHd=plHd;
					}
				}
			}
		}
	}

	YsShell::PolygonAndValue min;
	min.plHd=minCosPlHd;
	min.value=acos(YsBound(minCos,-1.0,1.0));
	return min;
}
YsShell::PolygonAndValue YsShellExt_DeformationEvaluator::CalculateMaxNormalChangeAfterEdgeCollapse(const YsShellExt &shl,const YsShellVertexHandle edVtHd[2],const YsVec3 &newVtPos) const
{
	return CalculateMaxNormalChangeAfterEdgeCollapse(shl,edVtHd[0],edVtHd[1],newVtPos);
}
YsShell::PolygonAndValue YsShellExt_DeformationEvaluator::CalculateMaxNormalChangeAfterEdgeCollapse(const YsShellExt &shl,YsShell::Edge edge,const YsVec3 &newVtPos) const
{
	return CalculateMaxNormalChangeAfterEdgeCollapse(shl,edge[0],edge[1],newVtPos);
}


double YsShellExt_DeformationEvaluator::CalculateMaxNormalChangeAfterEdgeSwapping(const YsShellExt &shl,const class YsShell_SwapInfo &swapInfo) const
{
	// Work in progress.
	return 0.0;
}
