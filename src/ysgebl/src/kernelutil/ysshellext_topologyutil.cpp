/* ////////////////////////////////////////////////////////////

File Name: ysshellext_topologyutil.cpp
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

#include "ysshellext_topologyutil.h"


/* static */ int YsShellExt_TopologyUtil::GetNumSingleUseEdgeConnectedToVertex(const YsShellExt &shl,YsShellVertexHandle vtHd)
{
	auto connVtHdArray=shl.GetConnectedVertex(vtHd);
	int n=0;
	for(auto connVtHd : connVtHdArray)
	{
		if(YSTRUE==IsSingleUseEdge(shl,vtHd,connVtHd))
		{
			++n;
		}
	}
	return n;
}
/* static */ YSBOOL YsShellExt_TopologyUtil::IsSingleUseEdge(const YsShellExt &shl,YsShellEdgeEnumHandle edHd)
{
	// To do: Should take interior faces into account.
	auto edPlHdArray=shl.FindPolygonFromEdgePiece(edHd);
	if(1==edPlHdArray.GetN())
	{
		return YSTRUE;
	}
	return YSFALSE;

}
/* static */ YSBOOL YsShellExt_TopologyUtil::IsSingleUseEdge(const YsShellExt &shl,YsShellVertexHandle edVtHd0,YsShellVertexHandle edVtHd1)
{
	// To do: Should take interior faces into account.
	auto edPlHdArray=shl.FindPolygonFromEdgePiece(edVtHd0,edVtHd1);
	if(1==edPlHdArray.GetN())
	{
		return YSTRUE;
	}
	return YSFALSE;
}
/* static */ YSBOOL YsShellExt_TopologyUtil::IsSingleUseEdge(const YsShellExt &shl,const YsShellVertexHandle edVtHd[2])
{
	return IsSingleUseEdge(shl,edVtHd[0],edVtHd[1]);
}

/* static */ YSBOOL YsShellExt_TopologyUtil::IsVertexUsedBySingleUseEdge(const YsShellExt &shl,const YsShellVertexHandle vtHd)
{
	for(auto connVtHd : shl.GetConnectedVertex(vtHd))
	{
		if(YSTRUE==IsSingleUseEdge(shl,vtHd,connVtHd))
		{
			return YSTRUE;
		}
	}
	return YSFALSE;
}


void YsShellExt_TopologyUtil::CleanUp(void)
{
	modifiedPlHdArray.CleanUp();
	modifiedCeHdArray.CleanUp();
}

YsArray <YsShell::VertexHandle,16> YsShellExt_TopologyUtil::InsertVertexOnPolygonEdge(
    const YsConstArrayMask <YsShell::VertexHandle> &plVtHd,
    YsShell::VertexHandle fromVtHd,
    YsShell::VertexHandle toVtHd,
    YSSIZE_T nVt,
    const YsShell::VertexHandle vtHdArray[]) const
{
	YsArray <YsShell::VertexHandle,16> newPlVtHd;
	for(YSSIZE_T vtIdx=0; vtIdx<plVtHd.GetN(); ++vtIdx)
	{
		newPlVtHd.Append(plVtHd[vtIdx]);
		if(fromVtHd==plVtHd[vtIdx] && toVtHd==plVtHd.GetCyclic(vtIdx+1))
		{
			newPlVtHd.Append(nVt,vtHdArray);
		}
		else if(toVtHd==plVtHd[vtIdx] && fromVtHd==plVtHd.GetCyclic(vtIdx+1))
		{
			for(auto i=nVt-1; 0<=i; --i)
			{
				newPlVtHd.Append(vtHdArray[i]);
			}
		}
	}
	return newPlVtHd;
}

YsArray <YsShell::VertexHandle,16> YsShellExt_TopologyUtil::InsertVertexOnConstEdge(
    const YsConstArrayMask <YsShell::VertexHandle> &ceVtHd,
    YSBOOL isLoop,
    YsShell::VertexHandle fromVtHd,
    YsShell::VertexHandle toVtHd,
    YSSIZE_T nVt,
    const YsShell::VertexHandle vtHdArray[]) const
{
	YsArray <YsShell::VertexHandle> newCeVtHd;

	const auto nEnd=(YSTRUE==isLoop ? ceVtHd.GetN() : ceVtHd.GetN()-1);

	for(YSSIZE_T vtIdx=0; vtIdx<nEnd; ++vtIdx)
	{
		newCeVtHd.Append(ceVtHd[vtIdx]);
		if(fromVtHd==ceVtHd[vtIdx] && toVtHd==ceVtHd.GetCyclic(vtIdx+1))
		{
			newCeVtHd.Append(nVt,vtHdArray);
		}
		else if(toVtHd==ceVtHd[vtIdx] && fromVtHd==ceVtHd.GetCyclic(vtIdx+1))
		{
			for(auto i=nVt-1; 0<=i; --i)
			{
				newCeVtHd.Append(vtHdArray[i]);
			}
		}
	}

	if(YSTRUE!=isLoop)
	{
		newCeVtHd.Append(ceVtHd.Last());
	}

	return newCeVtHd;
}

void YsShellExt_TopologyUtil::FindSplitLocation(YSSIZE_T &vtIdx0,YSSIZE_T &vtIdx1,const YsConstArrayMask <YsShell::VertexHandle> &plVtHd,YsShell::VertexHandle vtHd0,YsShell::VertexHandle vtHd1) const
{
	for(YSSIZE_T vtIdx=0; vtIdx<plVtHd.GetN(); ++vtIdx)
	{
		if(plVtHd[vtIdx]==vtHd0)
		{
			vtIdx0=vtIdx;
		}
		else if(plVtHd[vtIdx]==vtHd1)
		{
			vtIdx1=vtIdx;
		}
	}
}

