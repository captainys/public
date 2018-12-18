/* ////////////////////////////////////////////////////////////

File Name: ysshellext_facegrouputil.cpp
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

#include "ysshellext_facegrouputil.h"

void YsShellExt_FaceGroupCandidate::CleanUp(void)
{
	grpArray.CleanUp();
}
const YsArray <YsShellExt_FaceGroupCandidate::GroupCandidate> &YsShellExt_FaceGroupCandidate::FaceGroupCandidate(void) const
{
	return grpArray;
}


class YsShellExt_FaceGroupCandidate::ForcePolygonToBeInFaceGroupTrackingCondition : public YsShellExt_TrackingUtil::SearchCondition
{
public:
	virtual YSRESULT TestPolygon(const YsShellExt &shl,YsShellPolygonHandle plHd) const;
	virtual YSRESULT TestEdge(const YsShellExt &shl,YsShellVertexHandle edVtHd0,YsShellVertexHandle edVtHd1) const;
};

/* virtual */ YSRESULT YsShellExt_FaceGroupCandidate::ForcePolygonToBeInFaceGroupTrackingCondition::TestPolygon(const YsShellExt &shl,YsShellPolygonHandle plHd) const
{
	if(NULL==shl.FindFaceGroupFromPolygon(plHd))
	{
		return YSOK;
	}
	return YSERR;
}

/* virtual */ YSRESULT YsShellExt_FaceGroupCandidate::ForcePolygonToBeInFaceGroupTrackingCondition::TestEdge(const YsShellExt &shl,YsShellVertexHandle edVtHd0,YsShellVertexHandle edVtHd1) const
{
	if(0==shl.GetNumConstEdgeUsingEdgePiece(edVtHd0,edVtHd1))
	{
		return YSOK;
	}
	return YSERR;

}

void YsShellExt_FaceGroupCandidate::ForcePolygonToBeInFaceGroup(const YsShellExt &shl)
{
	CleanUp();

	ForcePolygonToBeInFaceGroupTrackingCondition cond;
	YsShellPolygonStore visited(shl.Conv());

	for(auto plHd : shl.AllPolygon())
	{
		if(YSTRUE!=visited.IsIncluded(plHd) && YSOK==cond.TestPolygon(shl,plHd))
		{
			grpArray.Increment();
			grpArray.Last().plHdArray=YsShellExt_TrackingUtil::MakeEdgeConnectedPolygonGroup(shl,plHd,&cond);
			visited.Add(grpArray.Last().plHdArray);
		}
	}
}

////////////////////////////////////////////////////////////


YSBOOL YsShellExt_CheckFaceGroupBending(const YsShellExt &shl,const YsShellExt::FaceGroupHandle fgHd,const double angleThr)
{
	const double cosThr=cos(angleThr);

	auto fgPlHd=shl.GetFaceGroup(fgHd);
	for(auto iter0=fgPlHd.begin(); iter0!=fgPlHd.end(); ++iter0)
	{
		auto plHd0=*iter0;
		auto iter1=iter0;
		++iter1;
		auto nom0=shl.GetNormal(plHd0);
		for(; iter1!=fgPlHd.end(); ++iter1)
		{
			auto plHd1=*iter1;
			auto nom1=shl.GetNormal(plHd1);
			if(nom1*nom0<cosThr)
			{
				return YSTRUE;
			}
		}
	}
	return YSFALSE;
}



YSBOOL YsShellExt_CheckFaceGroupDisconnectedByRemovingPolygon(const YsShellExt &shl,YsShell::PolygonHandle plHd)
{
	class AroundPolygonCondition : public YsShellExt::Condition
	{
	public:
		YsShellExt::FaceGroupHandle withinThisFgHd;
		YsShell::PolygonHandle aroundThisPlHd;
		YsArray <YsShell::VertexHandle> plVtHd;
		virtual YSRESULT TestPolygon(const YsShellExt &shl,YsShellPolygonHandle plHd) const
		{
			if(aroundThisPlHd==plHd)
			{
				return YSERR;
			}
			if(withinThisFgHd!=shl.FindFaceGroupFromPolygon(plHd))
			{
				return YSERR;
			}
			int nPlVt;
			const YsShell::VertexHandle *plVtHdPtr;
			shl.GetPolygon(nPlVt,plVtHdPtr,plHd);
			const YsConstArrayMask <YsShell::VertexHandle> tstPlVtHd(nPlVt,plVtHdPtr);
			if(YSTRUE==this->plVtHd.HasCommonItem(tstPlVtHd))
			{
				return YSOK;
			}
			return YSERR;
		}
		virtual YSRESULT TestEdge(const YsShellExt &shl,YsShellVertexHandle edVtHd0,YsShellVertexHandle edVtHd1) const
		{
			if(YSTRUE!=plVtHd.IsIncluded(edVtHd0) && YSTRUE!=plVtHd.IsIncluded(edVtHd1))
			{
				return YSERR;
			}
			return YSOK;
		}
	};

	auto fgHd=shl.FindFaceGroupFromPolygon(plHd);
	if(nullptr==fgHd)
	{
		return YSFALSE;
	}

	auto plVtHd=shl.GetPolygonVertex(plHd);
	YsArray <YsShell::PolygonHandle> allPlg;
	for(auto vtHd : plVtHd)
	{
		auto vtPlHd=shl.FindPolygonFromVertex(vtHd);
		for(auto tstPlHd : vtPlHd)
		{
			if(tstPlHd!=plHd && shl.FindFaceGroupFromPolygon(tstPlHd)==fgHd)
			{
				allPlg.Add(tstPlHd);
			}
		}
	}
	YsRemoveDuplicateInUnorderedArray(allPlg);

	if(0==allPlg.GetN())
	{
		// Is it true or false?  It either removes an island or removes the face group all together...
		// I say false because it at least doesn't cause a additional division.
		return YSFALSE;
	}

	AroundPolygonCondition cond;
	cond.withinThisFgHd=fgHd;
	cond.aroundThisPlHd=plHd;
	cond.plVtHd=plVtHd;

	auto track=YsShellExt_TrackingUtil::MakeEdgeConnectedPolygonGroup(shl,allPlg[0],&cond);

	if(track.GetN()!=allPlg.GetN())
	{
		return YSTRUE;
	}
	return YSFALSE;
}

