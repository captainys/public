/* ////////////////////////////////////////////////////////////

File Name: ysshellextcondition.cpp
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

#include "ysshellext.h"

/* virtual */ YSRESULT YsShellExt::PassAll::TestPolygon(const YsShellExt &,YsShellPolygonHandle) const
{
	return YSOK;
}
/* virtual */ YSRESULT YsShellExt::PassAll::TestEdge(const YsShellExt &,YsShellVertexHandle ,YsShellVertexHandle ) const
{
	return YSOK;
}

////////////////////////////////////////////////////////////

/* virtual */ YSRESULT YsShellExt::PassFaceGroup::TestPolygon(const YsShellExt &shl,YsShellPolygonHandle plHd) const
{
	if(fgHd==shl.FindFaceGroupFromPolygon(plHd))
	{
		return YSOK;
	}
	return YSERR;
}
/* virtual */ YSRESULT YsShellExt::PassFaceGroup::TestEdge(const YsShellExt &shl,YsShellVertexHandle edVtHd0,YsShellVertexHandle edVtHd1) const
{
	return YSOK;
}

////////////////////////////////////////////////////////////

/* virtual */ YSRESULT YsShellExt::PassAllPolygonDontPassConstEdge::TestPolygon(const YsShellExt &,YsShellPolygonHandle) const
{
	return YSOK;
}
/* virtual */ YSRESULT YsShellExt::PassAllPolygonDontPassConstEdge::TestEdge(const YsShellExt &shl,YsShellVertexHandle edVtHd0,YsShellVertexHandle edVtHd1) const
{
	if(YSTRUE==shl.IsEdgePieceConstrained(edVtHd0,edVtHd1))
	{
		return YSERR;
	}
	return YSOK;
}

////////////////////////////////////////////////////////////

YsShellExt::TrackingAlongEdgeStore::TrackingAlongEdgeStore(const YsShellEdgeStore &edgeStore)
{
	edgeStorePointer=&edgeStore;
}
/* virtual */ YSRESULT YsShellExt::TrackingAlongEdgeStore::TestPolygon(const YsShellExt &shl,YsShellPolygonHandle plHd) const
{
	return YSERR;
}
/* virtual */ YSRESULT YsShellExt::TrackingAlongEdgeStore::TestEdge(const YsShellExt &shl,YsShellVertexHandle edVtHd0,YsShellVertexHandle edVtHd1) const
{
	if(YSTRUE==edgeStorePointer->IsIncluded(edVtHd0,edVtHd1))
	{
		return YSOK;
	}
	return YSERR;
}

////////////////////////////////////////////////////////////

YsShellExt::TrackingAlongEdgeStoreOnSpecificConstEdge::TrackingAlongEdgeStoreOnSpecificConstEdge(const YsShellEdgeStore &edgeStore,ConstEdgeHandle ceHd) :
    TrackingAlongEdgeStore(edgeStore)
{
	this->ceHd=ceHd;
}
/* virtual */ YSRESULT YsShellExt::TrackingAlongEdgeStoreOnSpecificConstEdge::TestEdge(const YsShellExt &shl,YsShellVertexHandle edVtHd0,YsShellVertexHandle edVtHd1) const
{
	if(YSOK==TrackingAlongEdgeStore::TestEdge(shl,edVtHd0,edVtHd1))
	{
		auto edCeHd=shl.FindConstEdgeFromEdgePiece(edVtHd0,edVtHd1);
		if(YSTRUE==edCeHd.IsIncluded(this->ceHd))
		{
			return YSOK;
		}
	}
	return YSERR;
}

////////////////////////////////////////////////////////////

/* virtual */ YSRESULT YsShellExt::TrackingAlongSingleUseEdge::TestPolygon(const YsShellExt &shl,YsShellPolygonHandle plHd) const
{
	return YSERR;
}
/* virtual */ YSRESULT YsShellExt::TrackingAlongSingleUseEdge::TestEdge(const YsShellExt &shl,YsShellVertexHandle edVtHd0,YsShellVertexHandle edVtHd1) const
{
	if(1==shl.GetNumPolygonUsingEdge(edVtHd0,edVtHd1))
	{
		return YSOK;
	}
	return YSERR;
}
