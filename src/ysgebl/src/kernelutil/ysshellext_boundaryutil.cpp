/* ////////////////////////////////////////////////////////////

File Name: ysshellext_boundaryutil.cpp
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

#include "ysshellext_boundaryutil.h"
#include "ysshellext_trackingutil.h"

YsShellExt_BoundaryInfo::YsShellExt_BoundaryInfo()
{
	CleanUp();
}

YsShellExt_BoundaryInfo::~YsShellExt_BoundaryInfo()
{
	CleanUp();
}

void YsShellExt_BoundaryInfo::CleanUp(void)
{
	srcEdVtHd.CleanUp();
	edgeToPlgHash.CleanUp();
	edgeDirectionHash.CleanUpThin();
	boundaryEdgeHash.CleanUp();
	nonDuplicateVtHdArray.CleanUp();
	contourCache.CleanUp();
}

void YsShellExt_BoundaryInfo::MakeInfo(
    const YsShell &shl,
    YSSIZE_T nPl,const YsShellPolygonHandle plHdPtr[])
{
	auto plHdArray=YsMakeArrayMask(nPl,plHdPtr);
	MakeInfo(shl,plHdArray);
}

void YsShellExt_BoundaryInfo::MakeVertexConnectionTable(
		YsFixedLengthToMultiHashTable <YsShellVertexHandle,1,2> &vtKeyToConnVtHd,
		const YsShell &shl) const
{
	vtKeyToConnVtHd.PrepareTable();
	vtKeyToConnVtHd.EnableAutoResizing();

	for(YSSIZE_T idx=0; idx<=srcEdVtHd.GetN()-2; idx+=2)
	{
		const unsigned vtKey[2]=
		{
			shl.GetSearchKey(srcEdVtHd[idx]),
			shl.GetSearchKey(srcEdVtHd[idx+1])
		};

		vtKeyToConnVtHd.AddElement(1,vtKey+0,srcEdVtHd[idx+1]);
		vtKeyToConnVtHd.AddElement(1,vtKey+1,srcEdVtHd[idx+0]);
	}
}

void YsShellExt_BoundaryInfo::CacheNonDuplicateVtHdArray(void)
{
	nonDuplicateVtHdArray=srcEdVtHd;
	YsRemoveDuplicateInUnorderedArray(nonDuplicateVtHdArray);
}

YsVec3 YsShellExt_BoundaryInfo::GetCenter(const YsShellExt &shl) const
{
	YsVec3 cen=YsOrigin();
	if(0<nonDuplicateVtHdArray.GetN())
	{
		for(YSSIZE_T vtIdx=0; vtIdx<nonDuplicateVtHdArray.GetN(); ++vtIdx)
		{
			YsVec3 pos;
			shl.GetVertexPosition(pos,nonDuplicateVtHdArray[vtIdx]);
			cen+=pos;
		}
		cen/=(double)nonDuplicateVtHdArray.GetN();
	}
	return cen;
}

YsVec3 YsShellExt_BoundaryInfo::GetBoundingBoxCenter(const YsShellExt &shl) const
{
	if(0<nonDuplicateVtHdArray.GetN())
	{
		YsBoundingBoxMaker <YsVec3> bbx;
		for(YSSIZE_T vtIdx=0; vtIdx<nonDuplicateVtHdArray.GetN(); ++vtIdx)
		{
			bbx.Add(shl.GetVertexPosition(nonDuplicateVtHdArray[vtIdx]));
		}
		return bbx.GetCenter();
	}
	return YsVec3::Origin();
}

const YsArray <YsShellVertexHandle> &YsShellExt_BoundaryInfo::GetVertexAll(void) const
{
	return nonDuplicateVtHdArray;
}

YSRESULT YsShellExt_BoundaryInfo::CacheContour(const YsShell &shl)
{
	YsFixedLengthToMultiHashTable <YsShellVertexHandle,1,2> connection;

	MakeVertexConnectionTable(connection,shl);
	CacheNonDuplicateVtHdArray();

	contourCache.CleanUp();

	// First, start from the vertices with 3 or more connections.
	YsArray <YsShellVertexHandle> moreThanTwo,twoConnection;
	for(auto vtHd : nonDuplicateVtHdArray)
	{
		YSSIZE_T nConnVtHd;
		const YsShellVertexHandle *connVtHd;
		const unsigned int vtKey[1]={shl.GetSearchKey(vtHd)};
		if(YSOK==connection.FindElement(nConnVtHd,connVtHd,1,vtKey) && 3<=nConnVtHd)
		{
			moreThanTwo.Append(vtHd);
		}
		else if(2==nConnVtHd)
		{
			twoConnection.Append(vtHd);
		}
		else if(1==nConnVtHd)
		{
			contourCache.CleanUp();
			return YSERR;
		}
	}

	YsTwoKeyStore usedEdgePiece;
	for(auto vtHd : moreThanTwo)
	{
		YSSIZE_T nConnVtHd;
		const YsShellVertexHandle *connVtHd;
		const unsigned int vtKey[1]={shl.GetSearchKey(vtHd)};
		if(YSOK==connection.FindElement(nConnVtHd,connVtHd,1,vtKey))
		{
			for(int i=0; i<nConnVtHd; ++i)
			{
				if(YSOK!=TrackAndAddLoop(usedEdgePiece,shl,vtHd,connVtHd[i],connection))
				{
					contourCache.CleanUp();
					return YSERR;
				}
			}
		}
	}

	for(auto vtHd : twoConnection)
	{
		YSSIZE_T nConnVtHd;
		const YsShellVertexHandle *connVtHd;
		const unsigned int vtKey[1]={shl.GetSearchKey(vtHd)};
		if(YSOK==connection.FindElement(nConnVtHd,connVtHd,1,vtKey))
		{
			for(int i=0; i<nConnVtHd; ++i)
			{
				if(YSOK!=TrackAndAddLoop(usedEdgePiece,shl,vtHd,connVtHd[i],connection))
				{
					contourCache.CleanUp();
					return YSERR;
				}
			}
		}
	}

	for(YSSIZE_T idx=0; idx<=srcEdVtHd.GetN()-2; idx+=2)
	{
		const unsigned vtKey[2]=
		{
			shl.GetSearchKey(srcEdVtHd[idx]),
			shl.GetSearchKey(srcEdVtHd[idx+1])
		};
		if(YSTRUE!=usedEdgePiece.IsIncluded(vtKey[0],vtKey[1]))
		{
			contourCache.CleanUp();
			return YSERR;
		}
	}

	return YSOK;
}

void YsShellExt_BoundaryInfo::ReorientContour(const YsShell &shl)
{
	for(auto &contour : contourCache)
	{
		if(2<=contour.vtHdArray.GetN())
		{
			const YsShell::VertexHandle edVtHd[2]=
			{
				contour.vtHdArray[0],
				contour.vtHdArray[1],
			};
			const YSHASHKEY edVtKey[2]=
			{
				shl.GetSearchKey(contour.vtHdArray[0]),
				shl.GetSearchKey(contour.vtHdArray[1]),
			};
			auto plHdPtr=edgeToPlgHash.FindElement(2,edVtKey);
			if(1==plHdPtr->GetN())
			{
				auto plVtHd=shl.GetPolygonVertex((*plHdPtr)[0]);
				for(int i=0; i<plVtHd.GetN(); ++i)
				{
					if(plVtHd[i]==edVtHd[0] && plVtHd.GetCyclic(i+1)==edVtHd[1])
					{
						contour.vtHdArray.Invert();
						break;
					}
					else if(plVtHd[i]==edVtHd[1] && plVtHd.GetCyclic(i+1)==edVtHd[0])
					{
						break;
					}
				}
			}
		}
	}
}

YSRESULT YsShellExt_BoundaryInfo::TrackAndAddLoop(
    YsTwoKeyStore &usedEdgePiece,
    const YsShell &shl,YsShellVertexHandle fromVtHd,YsShellVertexHandle toVtHd,
    const YsFixedLengthToMultiHashTable <YsShellVertexHandle,1,2> &connection)
{
	if(YSTRUE==usedEdgePiece.IsIncluded(shl.GetSearchKey(fromVtHd),shl.GetSearchKey(toVtHd)))
	{
		return YSOK;
	}

	YsArray <YsShellVertexHandle> path=YsShellExt_TrackingUtil::TrackVertexConnection(shl,fromVtHd,toVtHd,connection);
	if(2<path.GetN() && path[0]==path.Last())
	{
		YSHASHKEY edVtKey[2]={shl.GetSearchKey(path[0]),shl.GetSearchKey(path[1])};
		YsShell::Edge *edgePtr=edgeDirectionHash.FindElement(2,edVtKey);
		if(nullptr!=edgePtr &&  (*edgePtr)[0]==path[1])
		{
			path.Invert();
		}

		contourCache.Increment();
		contourCache.Last().Initialize();
		contourCache.Last().vtHdArray=path;

		for(YSSIZE_T vtIdx=0; vtIdx<path.GetN()-1; ++vtIdx)
		{
			usedEdgePiece.AddKey(shl.GetSearchKey(path[vtIdx]),shl.GetSearchKey(path[vtIdx+1]));
		}
		return YSOK;
	}
	else
	{
		return YSERR;
	}
}

YSSIZE_T YsShellExt_BoundaryInfo::GetNumContour(void) const
{
	return contourCache.GetN();
}

YSRESULT YsShellExt_BoundaryInfo::GetContour(YSSIZE_T &nVt,const YsShellVertexHandle *&vtHdArray,YSSIZE_T idx) const
{
	if(YSTRUE==contourCache.IsInRange(idx))
	{
		nVt=contourCache[idx].vtHdArray.GetN();
		vtHdArray=contourCache[idx].vtHdArray;
		return YSOK;
	}
	nVt=0;
	vtHdArray=NULL;
	return YSERR;
}

const YsConstArrayMask <YsShell::VertexHandle> YsShellExt_BoundaryInfo::GetContour(YSSIZE_T idx) const
{
	if(YSTRUE==contourCache.IsInRange(idx))
	{
		auto nVt=contourCache[idx].vtHdArray.GetN();
		auto vtHdArray=contourCache[idx].vtHdArray;
		return YsConstArrayMask <YsShell::VertexHandle> (nVt,vtHdArray);
	}
	return YsConstArrayMask <YsShell::VertexHandle> (0,nullptr);
}

YsArray <YsArray <YsShellVertexHandle> > YsShellExt_BoundaryInfo::GetContourAll(void) const
{
	YsArray <YsArray <YsShellVertexHandle> > allContour(GetNumContour(),NULL);
	for(YSSIZE_T idx=0; idx<GetNumContour(); ++idx)
	{
		GetContour(allContour[idx],idx);
	}
	return allContour;
}

YSBOOL YsShellExt_BoundaryInfo::IsBoundaryEdge(const YsShell &shl,const YsShell::VertexHandle edVtHd[2]) const
{
	return IsBoundaryEdge(shl,edVtHd[0],edVtHd[1]);
}

YSBOOL YsShellExt_BoundaryInfo::IsBoundaryEdge(const YsShell &shl,YsShell::VertexHandle edVtHd0,YsShell::VertexHandle edVtHd1) const
{
	return boundaryEdgeHash.IsIncluded(edVtHd0,edVtHd1);
}

void YsShellExt_BoundaryInfo::GetEdgeUsageMinMax(int &min,int &max,const YsShellExt &shl) const
{
	if(2<=srcEdVtHd.GetN())
	{
		min=shl.GetNumPolygonUsingEdge(srcEdVtHd[0],srcEdVtHd[1]);
		max=min;
		for(YSSIZE_T edIdx=2; edIdx<=srcEdVtHd.GetN()-2; edIdx+=2)
		{
			const int n=shl.GetNumPolygonUsingEdge(srcEdVtHd[edIdx],srcEdVtHd[edIdx+1]);
			min=YsSmaller(min,n);
			max=YsGreater(max,n);
		}
	}
	else
	{
		min=0;
		max=0;
	}
}

YSSIZE_T YsShellExt_BoundaryInfo::GetNumEdgePiece(void) const
{
	return srcEdVtHd.GetN()/2;
}

YSRESULT YsShellExt_BoundaryInfo::GetEdge(YsShellVertexHandle edVtHd[2],YSSIZE_T edIdx) const
{
	if(0<=edIdx && edIdx*2+1<srcEdVtHd.GetN())
	{
		edVtHd[0]=srcEdVtHd[edIdx*2];
		edVtHd[1]=srcEdVtHd[edIdx*2+1];
		return YSOK;
	}
	return YSERR;
}

