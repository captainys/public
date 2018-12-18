/* ////////////////////////////////////////////////////////////

File Name: ysshellextedit_boolutil.h
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

#ifndef YSSHELLEXTEDIT_BOOLUTIL_IS_INCLUDED
#define YSSHELLEXTEDIT_BOOLUTIL_IS_INCLUDED
/* { */

#include <ysclass.h>
#include <ysshellextedit.h>

class YsShellExtEdit_BooleanUtil
{
private:
	class VertexOrigin
	{
	public:
		YsShellVertexHandle vtHd[2];
		// vtHd[0]=vtHd in shlA
		// vtHd[1]=vtHd in shlB
		VertexOrigin();
		void Initialize(void);
	};
	class PolygonOrigin
	{
	public:
		YsShellPolygonHandle plHd[2];
		// plHd[0]=plHd in shlA
		// plHd[1]=plHd in shlB
		PolygonOrigin();
		void Initialize(void);
		int GetCode(void) const; // freeAttribute2

		bool operator==(const PolygonOrigin &from) const;
		bool operator!=(const PolygonOrigin &from) const;
	};
	class ConstEdgeOrigin
	{
	public:
		YsShellExt::ConstEdgeHandle ceHd[2];
		// ceHd[0]=ceHd in shlA
		// ceHd[1]=ceHd in shlB
		ConstEdgeOrigin();
		void Initialize(void);
	};

/*
	For separate operation, I want to set up all necessary modifications and stop short of actually applying changes.
	Do it for both shells, and then apply to both.
*/

	YsShellExtEdit *shlA,*shlB;
	YSBOOLEANOPERATION boolOpType;
	YsShellExt mixed[2];  // For separate operation, keep two mixed shell.

	YsHashTable <VertexOrigin> vtKeyInMixToOrigin[2];
	YsHashTable <PolygonOrigin> plKeyInMixToOrigin[2];
	YsHashTable <ConstEdgeOrigin> ceKeyInMixToOrigin[2];
	YsShellVertexStore usedVtxCache[2];

	YsHashTable <YsShellVertexHandle> vtKeyInMixToTargetVtHd[2];
	YsHashTable <YsShellExt::FaceGroupHandle> fgKeyInTheOtherShellToFgHdInTarget[2];

	YsArray <YSHASHKEY> unusedPlKey[2];

public:
	YsShellExtEdit_BooleanUtil();
	void CleanUp(void);
	void SetShellAndOpType(YsShellExtEdit &shlA,YsShellExtEdit &shlB,YSBOOLEANOPERATION boolOpType);
	YSRESULT Run(void);
private:
	void TransferBareMixToShellExt(int shlId,YsShell &bareShl);

public:
	void MakeVertexMapping(int shlId);  // Also tentatively mark vertices from the other shell as 'used'.
private:
	void CacheEdgeBetweenShell(YsFixedLengthToMultiHashTable <YsShellVertexHandle,1,2> &edgeBetweenShl,int shlId);
public:
	void CacheUnusedPolygon(int shlId);
	void TransferRemainingPartOfConstEdge(int shlId);
	void ReconstructConstEdge(int shlId);
	void CombineConstEdgeOfSameOrigin(int shlId);
	void ConstrainBetweenInputShell(int shlId);
	void DeleteTemporaryVertex(int shlId);
private:
	YsArray <PolygonOrigin> PlOriginOfMultiPolygon(int shlId,const YsArray <YsShellPolygonHandle> &plHdArray);

public:
	void MergePolygonFromSameOriginLeavingOneShortestEdgePerVertex(int shlId);
private:
	void MergePolygonFromSameOriginLeavingOneShortestEdgePerVertex(int shlId,YsShellVertexHandle vtHd,PolygonOrigin &origin);
	void MergePolygonFromSameOriginLeavingOneShortestEdgePerVertex(int shlId,YsShellVertexHandle vtHd,const YsArray <YsShellVertexHandle> &sortedConnVtHdArray);

public:
	void ModifyShell(int shlId);
	void AddPolygonFromTheOtherShell(int shlId);
	void ApplyConstEdgeChange(int shlId);
	void DeleteCachedUnusedPolygon(int shlId);
	void DeleteUsedVertexTurnedUnused(int shlId);
};

/* } */
#endif
