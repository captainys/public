/* ////////////////////////////////////////////////////////////

File Name: ysshellextsearch.h
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

#ifndef YSSHELLEXTSEARCH_IS_INCLUDED
#define YSSHELLEXTSEARCH_IS_INCLUDED
/* { */

#include "ysshellext.h"

// Need
//   Vertex to Const Edge
//   Polygon to Face Group
//   Face Group to Volume


class YsShellExt::SearchTable
{
friend class YsShellExt;

private:
	const YsShellExt *shl;
	YsFixedLengthToMultiHashTable <YsShellExt::ConstEdgeHandle,1,1> vtKeyToCeHd;
	YsHashTable <YsShellExt::FaceGroupHandle> plKeyToFgHd;
	YsFixedLengthToMultiHashTable <YsShellExt::VolumeHandle,1,1> fgKeyToVlHd;

	SearchTable();
	~SearchTable();

	void CleanUp(void);

	void Attach(const YsShellExt *shl);
	void Detach(void);

	void ConstEdgeAdded(YsShellExt::ConstEdgeHandle ceHd);
	void ConstEdgeVertexAdded(YsShellExt::ConstEdgeHandle ceHd,const YSSIZE_T nVt,const YsShellVertexHandle ceVtHd[]);
	void ConstEdgeAboutToBeDeleted(YsShellExt::ConstEdgeHandle ceHd);

	void FaceGroupAdded(YsShellExt::FaceGroupHandle fgHd);
	void FaceGroupAboutToBeDeleted(YsShellExt::FaceGroupHandle fgHd);

	void PolygonAddedToFaceGroup(YsShellExt::FaceGroupHandle fgHd,YsShellPolygonHandle plHd);
	void PolygonRemovedFromFaceGroup(YsShellExt::FaceGroupHandle fgHd,YsShellPolygonHandle plHd);
	void MultiplePolygonAddedToFaceGroup(YsShellExt::FaceGroupHandle fgHd,YSSIZE_T nPl,const YsShellPolygonHandle plHd[]);
	void MultiplePolygonRemovedFromFaceGroup(YsShellExt::FaceGroupHandle fgHd,YSSIZE_T nPl,YsShellPolygonHandle plHd[]);

	void VolumeAdded(YsShellExt::VolumeHandle vlHd);
	void VolumeAboutToBeDeleted(YsShellExt::VolumeHandle vlHd);

	void MultipleFaceGroupAddedToVolume(YsShellExt::VolumeHandle vlHd,YSSIZE_T nFg,const YsShellExt::FaceGroupHandle fgHd[]);

	YSRESULT FindConstEdgeFromVertex(YSSIZE_T &nCe,const YsShellExt::ConstEdgeHandle *&ceHdPtr,YsShellVertexHandle vtHd) const;
	YSRESULT FindConstEdgeFromVertex(YSSIZE_T &nCe,const YsShellExt::ConstEdgeHandle *&ceHdPtr,YSHASHKEY vtKey) const;
	template <const int N>
	YSRESULT FindConstEdgeFromVertex(YsArray <YsShellExt::ConstEdgeHandle,N> &ceHdArray,YsShellVertexHandle vtHd) const;

	YsShellExt::FaceGroupHandle FindFaceGroupFromPolygon(YsShellPolygonHandle plHd) const;

	YSRESULT FindVolumeFromFaceGroup(YSSIZE_T &nVl,const YsShellExt::VolumeHandle *&fgVlHd,YsShellExt::FaceGroupHandle fgHd) const;
};

template <const int N>
YSRESULT YsShellExt::SearchTable::FindConstEdgeFromVertex(YsArray <YsShellExt::ConstEdgeHandle,N> &ceHdArray,YsShellVertexHandle vtHd) const
{
	YSSIZE_T nCe;
	const YsShellExt::ConstEdgeHandle *ceHdPtr;

	if(YSOK==FindConstEdgeFromVertex(nCe,ceHdPtr,vtHd))
	{
		ceHdArray.Set(nCe,ceHdPtr);
		return YSOK;
	}
	else
	{
		ceHdArray.CleanUp();
		return YSERR;
	}
}

/* } */
#endif
