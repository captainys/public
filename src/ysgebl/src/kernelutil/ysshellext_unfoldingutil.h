/* ////////////////////////////////////////////////////////////

File Name: ysshellext_unfoldingutil.h
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

#ifndef YSSHELLEXT_UNFOLDINGUTIL_IS_INCLUDED
#define YSSHELLEXT_UNFOLDINGUTIL_IS_INCLUDED
/* { */

#include <ysshellext.h>
#include "ysshellext_localop.h"

/*! After decades of CAD research, nobody finally could address broken-geometry issues.

    All the time, when I export a CAD model into a tessellated model, 
    a CAD tessellator foolishly insert triangles between faces to make it watertight.
    The result is nearly 90 degree, or even near 180 degree, dihedral angle where the surface is supposed to be smooth.

    I can at least say there should be no large dihedral angle within triangles that come from a same CAD face.

    This class attempts to semi-automatically correct errors created by those foolish tessellators.

    Such a problematic high DHA edges can be fixed by smoothing or collapsing.
*/
class YsShellExt_UnfoldingUtil
{
public:
	class FoldingInfo
	{
	public:
		double maxDha;
		int numHighDhaEdge;

		void Initialize(void);
		bool IsBetterThan(const FoldingInfo &incoming) const;
	};
	class FoldingInfoAndCollapseTarget
	{
	public:
		FoldingInfo info;
		YsShell::VertexHandle toVtHd;
		void Initialize(void);
	};
	class FoldingInfoAndNewVertexPosition
	{
	public:
		FoldingInfo info;
		YsVec3 newVtxPos;
		void Initialize(void);
	};
	class FoldingInfoAndMultiVertexPosition
	{
	public:
		FoldingInfo info;
		YsArray <YsShell::VertexAndPos,2> newVtxPos;

		inline FoldingInfoAndMultiVertexPosition(){};
		FoldingInfoAndMultiVertexPosition(const FoldingInfoAndMultiVertexPosition &incoming);
		FoldingInfoAndMultiVertexPosition &operator=(const FoldingInfoAndMultiVertexPosition &incoming);
		FoldingInfoAndMultiVertexPosition(FoldingInfoAndMultiVertexPosition &&incoming);
		FoldingInfoAndMultiVertexPosition &operator=(FoldingInfoAndMultiVertexPosition &&incoming);
		void Initialize(void);
	};


protected:
	YsShellEdgeStore problematicEdge;

public:
	void MakeInfo(const YsShellExt &shl,const double dhaThr);
	void MakeInfo(const YsShellExt &shl,YsShellExt::FaceGroupHandle fgHd,const double dhaThr);
	void MakeInfo(const YsShellExt &shl,const YsShellVertexPositionSource &vtPosSrc,YsShellExt::FaceGroupHandle fgHd,const double dhaThr);
	const YsShellEdgeStore &GetProblematicEdge(void) const;

	FoldingInfo GetFoldingInfo(const YsShellExt &shl,YsShell::VertexHandle vtHd,const double dhaThr) const;
	FoldingInfo GetFoldingInfoAfterCollapsing(const YsShellExt &shl,YsShell::VertexHandle fromVtHd,YsShell::VertexHandle toVtHd,const double dhaThr) const;
	FoldingInfo GetFoldingInfoAfterSmoothing(const YsShellExt &shl,YsShell::VertexHandle fromVtHd,const YsVec3 &newPos,const double dhaThr) const;
	FoldingInfo GetFoldingInfoBeforeMultiSmoothing(const YsShellExt &shl,YSSIZE_T nVtHd,const YsShell::VertexHandle vtHd[],const double dhaThr) const;
	FoldingInfo GetFoldingInfoAfterMultiSmoothing(const YsShellExt &shl,YsArray <YsShell::VertexAndPos> &vtSmth,const double dhaThr) const;
private:
	void GetFoldingInfo(FoldingInfo &info,const YsShellExt &shl,const YsArray <YsShell::PolygonHandle> &allPlHd,const YsShellPolygonAttribTable <YsVec3> &plNom,const double dhaThr) const;

public:
	FoldingInfoAndMultiVertexPosition GetFoldingInfoAfterMultiLaplacianSmoothing(
	    const YsShellExt &shl,YsArray <YsShell::VertexHandle> &vtHd,const double dhaThr) const;

public:
	YsArray <YsShell::VertexHandle> FindCollapseCandidate(const YsShellExt &shl,const YsShell::VertexHandle fromVtHd) const;
	FoldingInfoAndCollapseTarget FindBestCollapseCandidate(const YsShellExt &shl,const YsShell::VertexHandle fromVtHd,const YsArray <YsShell::VertexHandle> &toCandidate,const double dhaThr) const;
	FoldingInfoAndNewVertexPosition FindBestSmoothingLocation(const YsShellExt &shl,const YsShell::VertexHandle fromVtHd,const double dhaThr) const;
	FoldingInfoAndMultiVertexPosition FindBestNeighborSmoothingLocation(const YsShellExt &shl,YsShell::VertexHandle fromVtHd,int nNeighbor,const double dhaThr) const;
	FoldingInfoAndMultiVertexPosition FindBestMultiSmoothingLocation(const YsShellExt &shl,YSSIZE_T nVtHd,const YsShell::VertexHandle vtHd[],const double dhaThr) const;

	/*! Call this function after MakeInfo to attempt correcting problematic edges. */
	template <class SHLCLASS>
	YSRESULT CorrectFoldingWithinFaceGroup(SHLCLASS &shl,const double dhaThr)
	{
		int nFix=0;

		typename SHLCLASS::StopIncUndo undoGuard(shl);
		for(auto edge : problematicEdge)
		{
			YSBOOL corrected=YSFALSE;
			for(int i=0; i<2; ++i)
			{
				auto fromVtHd=edge[i];
				auto collCan=FindCollapseCandidate(shl.Conv(),edge[i]);
				if(0<collCan.GetN())
				{
					auto bestCollapse=FindBestCollapseCandidate(shl.Conv(),fromVtHd,collCan,dhaThr);
					auto bestSmooth=FindBestSmoothingLocation(shl.Conv(),fromVtHd,dhaThr);
					if(nullptr!=bestCollapse.toVtHd && bestSmooth.newVtxPos!=shl.GetVertexPosition(fromVtHd))
					{
						if(bestCollapse.info.IsBetterThan(bestSmooth.info))
						{
							YsShell_CollapseInfo collInfo;
							collInfo.MakeInfo(shl.Conv(),fromVtHd,bestCollapse.toVtHd);
							collInfo.Apply(shl,YSTRUE); // Preserve Vertex=YSTRUE
						}
						else
						{
							shl.SetVertexPosition(fromVtHd,bestSmooth.newVtxPos);
						}
						corrected=YSTRUE;
						++nFix;
					}
					else if(nullptr!=bestCollapse.toVtHd)
					{
						YsShell_CollapseInfo collInfo;
						collInfo.MakeInfo(shl.Conv(),fromVtHd,bestCollapse.toVtHd);
						collInfo.Apply(shl,YSTRUE); // Preserve Vertex=YSTRUE
						corrected=YSTRUE;
						++nFix;
					}
					else if(bestSmooth.newVtxPos!=shl.GetVertexPosition(fromVtHd))
					{
						shl.SetVertexPosition(fromVtHd,bestSmooth.newVtxPos);
						corrected=YSTRUE;
						++nFix;
					}
				}
			}

			if(YSTRUE!=corrected)
			{
				if(1>=shl.GetNumFaceGroupUsingVertex(edge[0]) && 1>=shl.GetNumFaceGroupUsingVertex(edge[1]))
				{
					auto baseInfo=GetFoldingInfoBeforeMultiSmoothing(shl.Conv(),2,edge,dhaThr);
					auto bestInfo=FindBestMultiSmoothingLocation(shl.Conv(),2,edge,dhaThr);
					if(bestInfo.info.IsBetterThan(baseInfo))
					{
						for(auto vtPos : bestInfo.newVtxPos)
						{
							shl.SetVertexPosition(vtPos.vtHd,vtPos.pos);
						}
						corrected=YSTRUE;
						++nFix;
					}
				}
			}
		}

		if(0<nFix)
		{
			return YSOK;
		}
		return YSERR;
	}
};


/* } */
#endif
