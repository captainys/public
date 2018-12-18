/* ////////////////////////////////////////////////////////////

File Name: ysshellext_deformationevaluator.h
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

#ifndef YSSHELLEXT_DEFORMATIONEVALUATOR_IS_INCLUDED
#define YSSHELLEXT_DEFORMATIONEVALUATOR_IS_INCLUDED
/* { */


/*! WARNING!
  This class is very pre-mature.  Might substantially change in the next version.
  This class is very pre-mature.  Might substantially change in the next version.
  This class is very pre-mature.  Might substantially change in the next version.
  This class is very pre-mature.  Might substantially change in the next version.
  This class is very pre-mature.  Might substantially change in the next version.
*/


#include <ysshellext.h>
#include <ysshellextmisc.h>

class YsShellExt_DeformationEvaluator
{
public:
	class MinimumDeformationCollapseDirection
	{
	public:
		YsShell::VertexHandle vtHd;
		YsShell::PolygonAndValue maxNormalChange;

		MinimumDeformationCollapseDirection()
		{
			vtHd=nullptr;
			maxNormalChange.plHd=nullptr;
			maxNormalChange.value=YsInfinity;
		}
	};


private:
	class PolygonNormalPair
	{
	public:
		YsShellPolygonHandle plHd;
		YsVec3 nom;
	};
	class EdgeCollapseEvaluationCache
	{
	public:
		YsArray <PolygonNormalPair,8> plNomPairArray;
		YsShell::Edge edgeToBeCollapsed;

		EdgeCollapseEvaluationCache()
		{
		}
		EdgeCollapseEvaluationCache(const EdgeCollapseEvaluationCache &from)
		{
			this->plNomPairArray=from.plNomPairArray;
			this->edgeToBeCollapsed=from.edgeToBeCollapsed;
		}
		EdgeCollapseEvaluationCache(EdgeCollapseEvaluationCache &&from)
		{
			this->plNomPairArray.MoveFrom(from.plNomPairArray);
			this->edgeToBeCollapsed=from.edgeToBeCollapsed;
		}
		EdgeCollapseEvaluationCache &operator=(const EdgeCollapseEvaluationCache &from)
		{
			this->plNomPairArray=from.plNomPairArray;
			this->edgeToBeCollapsed=from.edgeToBeCollapsed;
			return *this;
		}
		EdgeCollapseEvaluationCache &operator=(EdgeCollapseEvaluationCache &&from)
		{
			this->plNomPairArray.MoveFrom(from.plNomPairArray);
			this->edgeToBeCollapsed=from.edgeToBeCollapsed;
			return *this;
		}
	};

	YsShellPolygonAttribTable <YsVec3> *plNomCache;
	YsShellExt::VertexFaceGroupAttribTable <YsVec3> *vtFgNomCache;

public:
	YsShellExt_DeformationEvaluator();
	~YsShellExt_DeformationEvaluator();

	/*! This function cleans up everything including cached polygon-normals and vertex facegroup-normals. */
	void CleanUp(void);

	void DeleteNormalCache(void);

	void MakePolygonNormalCache(const YsShellExt &shl);
	void MakeVertexFaceGroupNormalCache(const YsShellExt &shl);

	const YsShellExt::VertexFaceGroupAttribTable <YsVec3> *GetVertexFaceGroupNormalCache(void) const;

	/*! Cache polygon-normal pairs that will be affected by collapsing the edge, excluding the polygons that will disappear. */
	EdgeCollapseEvaluationCache MakeCacheForEdgeCollapse(const YsShellExt &shl,YsShellVertexHandle edVtHd0,YsShellVertexHandle edVtHd1) const;
	EdgeCollapseEvaluationCache MakeCacheForEdgeCollapse(const YsShellExt &shl,const YsShellVertexHandle edVtHd[2]) const;
	EdgeCollapseEvaluationCache MakeCacheForEdgeCollapse(const YsShellExt &shl,YsShell::Edge edge) const;

private:
	YsVec3 CalculatePreDeformationNormal(const YsShellExt &shl,YsShellPolygonHandle plHd) const;

public:
	/*! Returns the maximum normal change after collapsing the edge.  Polygon-normal pairs must be cached by MakeCacheForEdgeCollapse before using these functions. */
	YsShell::PolygonAndValue CalculateMaxNormalChangeAfterEdgeCollapse(const YsShellExt &shl,const EdgeCollapseEvaluationCache &cache,YsShellVertexHandle edVtHd0,YsShellVertexHandle edVtHd1,const YsVec3 &newVtPos) const;
	YsShell::PolygonAndValue CalculateMaxNormalChangeAfterEdgeCollapse(const YsShellExt &shl,const EdgeCollapseEvaluationCache &cache,const YsShellVertexHandle edVtHd[2],const YsVec3 &newVtPos) const;
	YsShell::PolygonAndValue CalculateMaxNormalChangeAfterEdgeCollapse(const YsShellExt &shl,const EdgeCollapseEvaluationCache &cache,YsShell::Edge edge,const YsVec3 &newVtPos) const;


	/*! Returns the maximum normal change after collapsing the edge based on the polygon normal cached by MakePolygonNormalCache.
	    By calling MakePolygonNormalCache before a sequence of edge-collapsing, this function will compare the normal change 
	    from the beginning of the sequence.

	    If this function is called before calling MakePolygonNormalCache, plHd member of the return value will be nullptr.
	*/
	YsShell::PolygonAndValue CalculateMaxNormalChangeAfterEdgeCollapse(const YsShellExt &shl,YsShellVertexHandle edVtHd0,YsShellVertexHandle edVtHd1,const YsVec3 &newVtPos) const;
	YsShell::PolygonAndValue CalculateMaxNormalChangeAfterEdgeCollapse(const YsShellExt &shl,const YsShellVertexHandle edVtHd[2],const YsVec3 &newVtPos) const;
	YsShell::PolygonAndValue CalculateMaxNormalChangeAfterEdgeCollapse(const YsShellExt &shl,YsShell::Edge edge,const YsVec3 &newVtPos) const;


	/*! This function identifies minimum-deformation collapse direction from vertex fromVtHd among vertices candidateVtHd. 
	*/
	MinimumDeformationCollapseDirection SelectMinimumDeformationCollpaseDirection(const YsShellExt &shl,const YsArray <YsShellVertexHandle> &candidateVtHd,YsShellVertexHandle fromVtHd) const;


public:
	/*! This function returns maximum normal change if swapInfo is applied. */
	double CalculateMaxNormalChangeAfterEdgeSwapping(const YsShellExt &shl,const class YsShell_SwapInfo &swapInfo) const;
};



/* } */
#endif
