/* ////////////////////////////////////////////////////////////

File Name: ysshellext_thickeningutil.h
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

#ifndef YSSHELLEXT_THICKENINGUTIL_IS_INCLUDED
#define YSSHELLEXT_THICKENINGUTIL_IS_INCLUDED
/* { */

#include "ysshellext_sweeputil.h"

class YsShellExt_ThickeningInfo : public YsShellExt_SweepInfo
{
private:
	using YsShellExt_SweepInfo::MakeInfo;

	// Vertex mapping taken care by YsShellExt_VertexToVertexMapping, base class of YsShellExt_SweepInfo.

	class PerVertexInfo
	{
	public:
		YsVec3 nom,orgPos;
		YsVec3 newPos,oppVtPos;
	};

	YsHashTable <YSSIZE_T> vtKeyToVtxInfoIndex;
	YsSegmentedArray <PerVertexInfo,4> vtxInfo;
	PerVertexInfo &FindPerVertexInfo(const YsShellExt &shl,YsShellVertexHandle vtHd);
	const PerVertexInfo &FindPerVertexInfo(const YsShellExt &shl,YsShellVertexHandle vtHd) const;

	YsShellExt_ThickeningInfo();
	~YsShellExt_ThickeningInfo();
public:
	static YsShellExt_ThickeningInfo *Create(void);
	static void Delete(YsShellExt_ThickeningInfo *);
	void CleanUp(void);

	/*! This function registers source polygons for thickening. */
	void MakeInfo(
	    const YsShellExt &shl,
	    YSSIZE_T nPl,const YsShellPolygonHandle plHdArray[]);

	template <const int N>
	void MakeInfo(const YsShellExt &shl,const YsArray <YsShellPolygonHandle,N> &plHdArray);

	/*! This function calculates the location of the opposite vertex of each source vertex.
	    Location of the opposite vertex can be accessed by GetOppositeVertexPosition. */
	void CalculateOppositeVertexPosition(const YsShellExt &shl,const double plusThickenss);

	/*! This function returns the location where the opposite vertex must be placed, calculated by 
	    CalculateOppositeVertexPosition. */
	const YsVec3 GetOppositeVertexPosition(const YsShellExt &shl,YsShellVertexHandle srcVtHd) const;

	/*! This function calculates where the source vertices must be displaced.  The locations can be
	    accessed by GetDisplacedVertexPosition. */
	void CalculateDisplacedPosition(const YsShellExt &shl,const double minusThickness);

	/*! This function returns the displaced vertex location calculated by CalculateDisplacedPosition. */
	const YsVec3 GetDisplacedVertexPosition(const YsShellExt &shl,YsShellVertexHandle srcVtHd) const;
};

template <const int N>
void YsShellExt_ThickeningInfo::MakeInfo(const YsShellExt &shl,const YsArray <YsShellPolygonHandle,N> &plHdArray)
{
	return MakeInfo(shl,plHdArray.GetN(),plHdArray);
}

/* } */
#endif
