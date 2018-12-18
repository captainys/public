/* ////////////////////////////////////////////////////////////

File Name: ysshellext_patchutil.h
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

#ifndef YSSHELLEXT_PATCHUTIL_IS_INCLUDED
#define YSSHELLEXT_PATCHUTIL_IS_INCLUDED
/* { */

#include <ysshellext.h>

class YsShellExt_PatchBetweenTwoConstEdge
{
public:
	enum PATCHSTRATEGY
	{
		UNIFORM_PARAMETER
	};

	YsShellExt shl;
	YsHashTable <YsShellVertexHandle> patchVtKeyToSrcVtHd;
	YsHashTable <int> patchVtKeyToSeqId;

	YsArray <YsShellVertexHandle> rawVtHdSeq[2],vtHdSeq[2];
	YsArray <double> paramArray[2];
	YSBOOL isLoop[2];

	YsShellExt::ConstEdgeHandle seqCeHd[2];

	double totalLen[2];


	YsShellExt_PatchBetweenTwoConstEdge();
	~YsShellExt_PatchBetweenTwoConstEdge();

	void CleanUp(void);

	/*! Use this function to set two sequences of vertices when all vertices are already included in srcShl.
	    If some of the vertices are not yet created, use another overload of this function.
	*/
	YSRESULT SetVertexSequence(const YsShell &srcShl,
	                       YSSIZE_T nVt0,const YsShell::VertexHandle vtHdArray0[],YSBOOL isLoop0,
	                       YSSIZE_T nVt1,const YsShell::VertexHandle vtHdArray1[],YSBOOL isLoop1);

	/*! Use this function to set two sequences of vertices when some vertices are not created in srcShl yet.
	    The locations of the vertices to be created must be stored in vtPosArray.
	    When vtHdArray[][i] is non-nullptr, the vertex coordinate is taken from srcShl, and vtPosArray[][i] is ignored.
	*/
	YSRESULT SetVertexSequence(const YsShell &srcShl,
	    const YsConstArrayMask <YsShell::VertexHandle> &vtHdArray0,const YsConstArrayMask <YsVec3> &vtPosArray0,YSBOOL isLoop0,
	    const YsConstArrayMask <YsShell::VertexHandle> &vtHdArray1,const YsConstArrayMask <YsVec3> &vtPosArray1,YSBOOL isLoop1);

	/*! This function updates vertex sequence.
	*/
	YSRESULT UpdateVertexSequence(int seqIdx,const YsShell &srcShl,const YsConstArrayMask <YsShell::VertexHandle> &srcVtHdArray);


	template <const int N>
	YSRESULT SetVertexSequence(const YsShell &srcShl,
	                       const YsArray <YsShellVertexHandle,N> &vtHdArray0,YSBOOL isLoop0,
	                       const YsArray <YsShellVertexHandle,N> &vtHdArray1,YSBOOL isLoop1);

	template <const int N>
	YSRESULT SetVertexSequence(const YsShell &srcShl,
	                       const YsArray <YsShellVertexHandle,N> vtHdArray[2],const YSBOOL isLoop[2]);

	void CalculateParameter(void);
	void MakeInitialGuess(void);
	void MinimizeDihedralAngleSum(void);

	/*! Call this function to merge triangles in this patch.
	    It should be used just before the polygons in this patch are consumed.
	    Since edges of quadrilaterals cannot be swapped, MinimizeDihedralAngleSum function will not function 
	    after this function.
	*/
	void MergeTriFormQuad(void);

	/*! This function returns the vertex sequence in the original shell.
	    For the vertices that the original-shell vertes is not assigned, nullptr will be set.
	*/
	YsArray <YsShell::VertexHandle> GetVertexSequence(int seq) const;

	template <const int N>
	void GetPolygonVertexOfOriginalShell(YsArray <YsShellVertexHandle,N> &plVtHd,YsVec3 &nom,YsShellPolygonHandle patchPlHd) const;

	template <class SHLCLASS>
	void CreateMissingVertexInOriginalShell(SHLCLASS &originalShl)
	{
		for(int i=0; i<2; ++i)
		{
			for(auto vtHd : vtHdSeq[i])
			{
				YsShell::VertexHandle srcVtHd=nullptr;
				if(YSOK!=patchVtKeyToSrcVtHd.FindElement(srcVtHd,shl.GetSearchKey(vtHd)) || nullptr==srcVtHd)
				{
					srcVtHd=originalShl.AddVertex(shl.GetVertexPosition(vtHd));
					patchVtKeyToSrcVtHd.UpdateElement(shl.GetSearchKey(vtHd),srcVtHd);
				}
			}
		}
	}

	template <class SHLCLASS>
	void TransferPatchToOriginalShell(SHLCLASS &originalShl,const YsColor col) const
	{
		for(auto patchPlHd : shl.AllPolygon())
		{
			YsArray <YsShellVertexHandle,4> plVtHd;
			YsVec3 nom;
			GetPolygonVertexOfOriginalShell(plVtHd,nom,patchPlHd);
			if(3<=plVtHd.size())
			{
				auto newPlHd=originalShl.AddPolygon(plVtHd);
				originalShl.SetPolygonNormal(newPlHd,nom);
				originalShl.SetPolygonColor(newPlHd,col);
			}
		}
	}
};

template <const int N>
YSRESULT YsShellExt_PatchBetweenTwoConstEdge::SetVertexSequence(const YsShell &srcShl,
                       const YsArray <YsShellVertexHandle,N> &vtHdArray0,YSBOOL isLoop0,
                       const YsArray <YsShellVertexHandle,N> &vtHdArray1,YSBOOL isLoop1)
{
	return SetVertexSequence(srcShl,vtHdArray0.GetN(),vtHdArray0,isLoop0,vtHdArray1.GetN(),vtHdArray1,isLoop1);
}

template <const int N>
YSRESULT YsShellExt_PatchBetweenTwoConstEdge::SetVertexSequence(const YsShell &srcShl,
                       const YsArray <YsShellVertexHandle,N> vtHdArray[2],const YSBOOL isLoop[2])
{
	return SetVertexSequence(srcShl,vtHdArray[0],isLoop[0],vtHdArray[1],isLoop[1]);
}

template <const int N>
void YsShellExt_PatchBetweenTwoConstEdge::GetPolygonVertexOfOriginalShell(YsArray <YsShellVertexHandle,N> &plVtHd,YsVec3 &nom,YsShellPolygonHandle patchPlHd) const
{
	nom=shl.GetNormal(patchPlHd);
	shl.GetPolygon(plVtHd,patchPlHd);
	for(auto &vtHd : plVtHd)
	{
		patchVtKeyToSrcVtHd.FindElement(vtHd,shl.GetSearchKey(vtHd));
	}
	for(auto idx=plVtHd.size()-1; 0<=idx; --idx)
	{
		if(plVtHd[idx]==plVtHd.GetCyclic(idx+1))
		{
			plVtHd.Delete(idx);
		}
	}
}



class YsShellExt_MultiPatchSequence
{
private:
	YsArray <YsShellExt_PatchBetweenTwoConstEdge> patchUtil;
public:
	/*! Removes patches.
	*/
	void CleanUp(void);

	/*! Returns a read-only sequence of patches.
	*/
	const YsConstArrayMask<YsShellExt_PatchBetweenTwoConstEdge> GetPatchSequence(void) const;

	/*! Set up from multiple const-edges.  If less than two const edges are given, it fails and returns YSERR.
	*/
	YSRESULT SetUpFromConstEdge(const YsShellExt &srcShl,const YsConstArrayMask<YsShellExt::ConstEdgeHandle> &inCeHd);

	/*! Set up from multiple polygons.  If less than two polygons are given, it fails and returns YSERR */
	YSRESULT SetUpFromPolygon(const YsShellExt &srcShl,const YsConstArrayMask<YsShell::PolygonHandle> &inPlHd);

	/*! Set up from first and last cross-sections and a path connecting them.
	*/
	YSRESULT SetUpFromFirstLastCrossSectionAndPathBetween(
	    const YsShellExt &srcShl,
	    const YsConstArrayMask <YsShell::VertexHandle> &seg0,
	    const YsConstArrayMask <YsShell::VertexHandle> &seg1,
	    const YsConstArrayMask <YsShell::VertexHandle> &path);

private:
	void CalculateNormal(void);

public:
	/*! Inverts current candidate polygons. 
	*/
	void Invert(void);

	/*! Merge triangles and form quads.
	*/
	void MergeTriFormQuad(void);

	/*! Creates vertices that are not yet created in the original shell.
	*/
	template <class SHLCLASS>
	void CreateMissingVertexInOriginalShell(SHLCLASS &originalShl)
	{
		YsArray <YsShell::VertexHandle> prevSeq;
		for(auto &patch : patchUtil)
		{
			patch.UpdateVertexSequence(0,originalShl.Conv(),prevSeq);
			patch.CreateMissingVertexInOriginalShell(originalShl);
			prevSeq=patch.GetVertexSequence(1);
		}
	}

	template <class SHLCLASS>
	void TransferPatchToOriginalShell(SHLCLASS &originalShl,const YsColor col) const
	{
		for(auto &patch : patchUtil)
		{
			patch.TransferPatchToOriginalShell(originalShl,col);
		}
	}
};


/* } */
#endif
