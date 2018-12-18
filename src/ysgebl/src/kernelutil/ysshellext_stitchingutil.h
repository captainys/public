/* ////////////////////////////////////////////////////////////

File Name: ysshellext_stitchingutil.h
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

#ifndef YSSHELLEXT_STITCHINGUTIL_IS_INCLUDED
#define YSSHELLEXT_STITCHINGUTIL_IS_INCLUDED
/* { */

#include <ysclass.h>
#include <ysshellext.h>
#include "ysshellext_mappingutil.h"

/*! \file */


/*! This function merges vertices that are located within tolerance (tol) and are on an edge used by one polygon or more than two polygons.
*/
template <class SHLCLASS>
YSRESULT YsShellExt_MergeVertexOnNonManifoldEdge(SHLCLASS &shl,const double tol)
{
	YsShellVertexStore limitedVtx(shl.Conv());
	for(auto vtHd : shl.AllVertex())
	{
		auto connVtHd=shl.GetConnectedVertex(vtHd);
		for(auto vtHd2 : connVtHd)
		{
			if(2>shl.GetNumPolygonUsingEdge(vtHd,vtHd2))
			{
				limitedVtx.AddVertex(vtHd);
				break;
			}
		}
	}
	return YsShellExt_MergeVertex(shl,tol,&limitedVtx);
}

/*! This function merges vertices that are located within tolerance (tol).
    Optionally target vertices can be specified by limitedVtx.  
    If limitedVtx is given, vertices that are not included in the vertex store will not be affected.
*/
template <class SHLCLASS>
YSRESULT YsShellExt_MergeVertex(SHLCLASS &shl,const double tol,const YsShellVertexStore *limitedVtx=NULL)
{
	YsShellExt_VertexToVertexMapping vtxMap;
	YsShellExt_MakeVertexMappingWithTolerance(vtxMap,shl.Conv(),tol,limitedVtx);

	for(auto plHd : shl.AllPolygon())
	{
		auto plVtHd=shl.GetPolygonVertex(plHd);
		YSBOOL change=YSFALSE;
		for(auto &vtHd : plVtHd)
		{
			YsArray <YsShellVertexHandle> mapped;
			vtxMap.FindMapping(mapped,shl.Conv(),vtHd);
			if(vtHd!=mapped[0])
			{
				vtHd=mapped[0];
				change=YSTRUE;
			}
		}
		if(YSTRUE==change)
		{
			for(YSSIZE_T idx=plVtHd.GetN()-1; 0<=idx; --idx)
			{
				if(plVtHd[idx]==plVtHd.GetCyclic(idx+1))
				{
					plVtHd.Delete(idx);
				}
			}
			if(3<=plVtHd.GetN())
			{
				shl.SetPolygonVertex(plHd,plVtHd);
			}
			else
			{
				shl.ForceDeletePolygon(plHd);
			}
		}
	}

	for(auto ceHd : shl.AllConstEdge())
	{
		YSBOOL isLoop;
		YsArray <YsShellVertexHandle> ceVtHd;
		shl.GetConstEdge(ceVtHd,isLoop,ceHd);

		YSBOOL change=YSFALSE;
		for(auto &vtHd : ceVtHd)
		{
			YsArray <YsShellVertexHandle> mapped;
			vtxMap.FindMapping(mapped,shl.Conv(),vtHd);
			if(vtHd!=mapped[0])
			{
				vtHd=mapped[0];
				change=YSTRUE;
			}
		}
		if(YSTRUE==change)
		{
			if(YSTRUE==isLoop)
			{
				for(YSSIZE_T idx=ceVtHd.GetN()-1; 0<=idx; --idx)
				{
					if(ceVtHd[idx]==ceVtHd.GetCyclic(idx+1))
					{
						ceVtHd.Delete(idx);
					}
				}
			}
			else
			{
				for(YSSIZE_T idx=ceVtHd.GetN()-2; 0<=idx; --idx)
				{
					if(ceVtHd[idx]==ceVtHd[idx+1])
					{
						ceVtHd.Delete(idx);
					}
				}
			}

			if(3<=ceVtHd.GetN())
			{
				shl.ModifyConstEdge(ceHd,ceVtHd,isLoop);
			}
			if(2<=ceVtHd.GetN())
			{
				shl.ModifyConstEdge(ceHd,ceVtHd,YSFALSE);
			}
			else
			{
				shl.DeleteConstEdge(ceHd);
			}
		}
	}

	return YSOK;
}


////////////////////////////////////////////////////////////



#include "ysshellext_proximityutil.h"

class YsShellExt_StitchingUtil
{
public:
	class Crack
	{
	public:
		YsArray <YsShellVertexHandle> path[2];
	};

protected:
	YsShellExt_EdgeVertexProximityUtil edVtProx;
	YsShellLattice ltc;

public:
	void CleanUp(void);
	YSRESULT PrepareForSingleUseEdge(const YsShellExt &shl);

	/*! Drops all vertex-edge pairs that one of v-e1 or v-e2 distance is longer than e1-e2. 
	    See research note 2014/12/30 also (SLT condition). */
	YSRESULT FilterBySelfLengthAsThresholdCondition(const YsShellExt &shl);

	YSRESULT CalculateAndCacheVertexNearestEdgePair(const YsShellExt &shl,YsShellVertexHandle vtHd);

	YsArray <Crack> FindOneToNCrack(const YsShellExt &shl);
};



////////////////////////////////////////////////////////////

/*! See research note 2015/06/16 */
class YsShellExt_IsocelesStitchingUtil
{
protected:
	YsArray <YsStaticArray <YsShellVertexHandle,2> > toMerge;
public:
	void CleanUp(void);
	YSRESULT MakeInfo(const YsShellExt &shl);
};

////////////////////////////////////////////////////////////

/*! See research note 2015/02/09 for the algorithm of EdgeStapling. */
class YsShellExt_EdgeStaplingUtil
{
private:
	YsAVLTree <double,YSHASHKEY> openAngleTree;
	YsHashTable <YsAVLTree <double,YSHASHKEY>::NodeHandle> vtKeyToTreeNode;

	class TwoVertex
	{
	public:
		YSHASHKEY vtKey[2];
	};

	YsHashTable <TwoVertex> vtxConnection;

public:
	void CleanUp(void);

	/*! All the edges of the input loop (vtHd[0]...vtHd[nVt-1]) must be a single-use edge.
	    This function assumes this single-edge condition when calculating open angle. */
	void SetCrackBoundary(const YsShellExt &shl,YSSIZE_T nVt,const YsShellVertexHandle vtHd[]);

	template <const int N>
	void SetCrackBoundary(const YsShellExt &shl,const YsArray <YsShellVertexHandle,N> &vtHdArray);

	/*! Returns an information of the minimum open-angle corner. 
	    Either connVtHd[0] may be stitched onto edge cornerVtHd-connVtHd[1], or
	    connVtHd[1] can be stitched onto edge cornerVtHd-connVtHd[0]
	    */
	YSRESULT GetMinimumOpenAngleCorner(YsShellVertexHandle &cornerVtHd,YsShellVertexHandle connVtHd[2],const YsShellExt &shl) const;

	/*! This function must be called after the minimum open-angle corner has been stapled. */
	YSRESULT UpdateCornerAfterStapling(YsShellVertexHandle cornerVtHd,const YsShellExt &shl);
};

template <const int N>
void YsShellExt_EdgeStaplingUtil::SetCrackBoundary(const YsShellExt &shl,const YsArray <YsShellVertexHandle,N> &vtHdArray)
{
	SetCrackBoundary(shl,vtHdArray.GetN(),vtHdArray);
}

/* } */
#endif
