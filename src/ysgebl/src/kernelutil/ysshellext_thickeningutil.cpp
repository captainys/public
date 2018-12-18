/* ////////////////////////////////////////////////////////////

File Name: ysshellext_thickeningutil.cpp
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

#include "ysshellext_thickeningutil.h"



YsShellExt_ThickeningInfo::PerVertexInfo &YsShellExt_ThickeningInfo::FindPerVertexInfo(const YsShellExt &shl,YsShellVertexHandle vtHd)
{
	YSSIZE_T idx;
	if(YSOK!=vtKeyToVtxInfoIndex.FindElement(idx,shl.GetSearchKey(vtHd)))
	{
		idx=vtxInfo.GetN();
		vtxInfo.Increment();
		vtKeyToVtxInfoIndex.AddElement(shl.GetSearchKey(vtHd),idx);
	}
	return vtxInfo[idx];
}

const YsShellExt_ThickeningInfo::PerVertexInfo &YsShellExt_ThickeningInfo::FindPerVertexInfo(const YsShellExt &shl,YsShellVertexHandle vtHd) const
{
	YSSIZE_T idx=0;
	vtKeyToVtxInfoIndex.FindElement(idx,shl.GetSearchKey(vtHd));
	return vtxInfo[idx];
}

YsShellExt_ThickeningInfo::YsShellExt_ThickeningInfo()
{
}

YsShellExt_ThickeningInfo::~YsShellExt_ThickeningInfo()
{
}

/* static */ YsShellExt_ThickeningInfo *YsShellExt_ThickeningInfo::Create(void)
{
	return new YsShellExt_ThickeningInfo;
}

/* static */ void YsShellExt_ThickeningInfo::Delete(YsShellExt_ThickeningInfo *ptr)
{
	delete ptr;
}

void YsShellExt_ThickeningInfo::CleanUp(void)
{
	YsShellExt_SweepInfo::CleanUp();
	vtKeyToVtxInfoIndex.CleanUp();
	vtxInfo.CleanUp();
}

void YsShellExt_ThickeningInfo::MakeInfo(
	    const YsShellExt &shl,
	    YSSIZE_T nPl,const YsShellPolygonHandle plHdArray[])
{
	CleanUp();
	YsShellExt_SweepInfo::MakeInfo(shl,nPl,plHdArray,0,NULL);

	for(auto srcVtHd : allSrcVtHd)
	{
		auto &pvi=FindPerVertexInfo(shl,srcVtHd);
		pvi.orgPos=shl.GetVertexPosition(srcVtHd);
		pvi.nom=YsOrigin();
		pvi.newPos=pvi.orgPos;
		pvi.oppVtPos=pvi.orgPos;
	}

	for(auto plHd : shl.AllPolygon())
	{
		const YsVec3 nom=shl.GetNormal(plHd);
		auto plVtHd=shl.GetPolygonVertex(plHd);
		for(auto vtHd : plVtHd)
		{
			auto &pvi=FindPerVertexInfo(shl,vtHd);
			pvi.nom+=nom;
		}
	}

	for(auto srcVtHd : allSrcVtHd)
	{
		auto &pvi=FindPerVertexInfo(shl,srcVtHd);
		pvi.nom.Normalize();
	}
}


void YsShellExt_ThickeningInfo::CalculateOppositeVertexPosition(const YsShellExt &shl,const double thickness)
{
	for(auto srcVtHd : allSrcVtHd)
	{
		auto &pvi=FindPerVertexInfo(shl,srcVtHd);
		pvi.oppVtPos=pvi.orgPos+pvi.nom*thickness;
	}
}

const YsVec3 YsShellExt_ThickeningInfo::GetOppositeVertexPosition(const YsShellExt &shl,YsShellVertexHandle srcVtHd) const
{
	auto &pvi=FindPerVertexInfo(shl,srcVtHd);
	return pvi.oppVtPos;
}

void YsShellExt_ThickeningInfo::CalculateDisplacedPosition(const YsShellExt &shl,const double minusThickness)
{
	for(auto srcVtHd : allSrcVtHd)
	{
		auto &pvi=FindPerVertexInfo(shl,srcVtHd);
		pvi.newPos=pvi.orgPos-pvi.nom*minusThickness;
	}
}
const YsVec3 YsShellExt_ThickeningInfo::GetDisplacedVertexPosition(const YsShellExt &shl,YsShellVertexHandle srcVtHd) const
{
	auto &pvi=FindPerVertexInfo(shl,srcVtHd);
	return pvi.newPos;
}
