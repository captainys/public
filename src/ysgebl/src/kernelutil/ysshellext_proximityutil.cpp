/* ////////////////////////////////////////////////////////////

File Name: ysshellext_proximityutil.cpp
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

#include "ysshellext_proximityutil.h"

YsShellExt_EdgeVertexProximityUtil::YsShellExt_EdgeVertexProximityUtil()
{
	shlPtr=NULL;
	CleanUp();
}

void YsShellExt_EdgeVertexProximityUtil::CleanUp(void)
{
	edVtPairTree.CleanUp();
	edgeToEdVtPairHandle.CleanUp();
	vertexToEdVtPairHandle.CleanUp();
}

void YsShellExt_EdgeVertexProximityUtil::SetShell(const YsShellExt &shl)
{
	this->shlPtr=&shl;
	edgeToEdVtPairHandle.SetShell(shl.Conv());
	vertexToEdVtPairHandle.SetShell(shl.Conv());
}

YsShellExt_EdgeVertexProximityUtil::EDVTPAIR_HANDLE YsShellExt_EdgeVertexProximityUtil::AddEdgeVertexPair(const YsShellVertexHandle edVtHd[2],YsShellVertexHandle vtHd,const double dist)
{
	EdgeVertexPair evp;
	evp.edVtHd[0]=edVtHd[0];
	evp.edVtHd[1]=edVtHd[1];
	evp.vtHd=vtHd;
	evp.nSharingEdge=0;

	auto hd=edVtPairTree.Insert(dist,evp);
	edgeToEdVtPairHandle.AddAttrib(edVtHd,hd);
	vertexToEdVtPairHandle.SetAttrib(vtHd,hd);

	auto sameEdgePairHd=FindEdgeVertexPairFromEdgePiece(edVtHd);
	for(auto hd : sameEdgePairHd)
	{
		edVtPairTree[hd].nSharingEdge=(int)sameEdgePairHd.GetN();
	}

	return hd;
}

YsShellExt_EdgeVertexProximityUtil::EDVTPAIR_HANDLE YsShellExt_EdgeVertexProximityUtil::GetNearestEdgeVertexPairHandle(void) const
{
	return edVtPairTree.First();
}

void YsShellExt_EdgeVertexProximityUtil::DeleteEdgeVertexPair(EDVTPAIR_HANDLE hd)
{
	auto pair=edVtPairTree[hd];

	auto needUpdate=FindEdgeVertexPairFromEdgePiece(pair.edVtHd);
	for(auto hd : needUpdate)
	{
		--edVtPairTree[hd].nSharingEdge;
	}

	edgeToEdVtPairHandle.DeleteAttrib(pair.edVtHd,hd);
	vertexToEdVtPairHandle.DeleteAttrib(pair.vtHd);
	edVtPairTree.Delete(hd);
}

void YsShellExt_EdgeVertexProximityUtil::DeleteEdge(const YsShell::VertexHandle edVtHd[2])
{
	for(auto hd : FindEdgeVertexPairFromEdgePiece(edVtHd))
	{
		DeleteEdgeVertexPair(hd);
	}
}

YSRESULT YsShellExt_EdgeVertexProximityUtil::FindNearestEdgeFromVertexNotSharingFaceGroup(YsShellVertexHandle edVtHdMin[2],double &dMin,const YsShellExt &shl,const YsShellLattice &ltc,YsShellVertexHandle vtHd) const
{
	edVtHdMin[0]=NULL;
	edVtHdMin[1]=NULL;
	dMin=0.0;

	double searchDist=ltc.GetLatticeDimension().GetLength();
	const double blkDgn=ltc.GetBlockDimension().GetLength();

	YsArray <YsShellVertexHandle,16> edVtHd;
	YsArray <YSHASHKEY,16> ceKey;
	YSSIZE_T nTested=0;

	const YsVec3 vtPos=shl.GetVertexPosition(vtHd);

	const auto excludeFgHd=shl.FindFaceGroupFromVertex(vtHd);

	for(int blkDist=0; blkDist<ltc.GetMaxNumBlock() && blkDgn*(double)blkDist<searchDist; ++blkDist)
	{
		if(YSOK==ltc.MakeEdgePieceListByPointAndBlockDistance(edVtHd,ceKey,nTested,vtPos,blkDist) && 0<nTested)
		{
			/*
			   Probablyl, it finds itself.
			   It must ignore edges that belongs to one of the face groups incident to the vertex because I don't want collapse of a face-group.
			*/

			for(int idx=0; idx<=edVtHd.GetN()-2; idx+=2)
			{
				auto edPlHd=shl.FindFaceGroupFromEdgePiece(edVtHd[idx],edVtHd[idx+1]);
				if(YSTRUE!=excludeFgHd.HasCommonItem(edPlHd))
				{
					// Is a candidate!
					const YsVec3 edVtPos[2]=
					{
						shl.GetVertexPosition(edVtHd[idx]),
						shl.GetVertexPosition(edVtHd[idx+1]),
					};
					const double d=YsGetPointLineSegDistance3(edVtPos,vtPos);
					if(NULL==edVtHdMin[0] || d<dMin)
					{
						edVtHdMin[0]=edVtHd[idx];
						edVtHdMin[1]=edVtHd[idx+1];
						dMin=d;
						searchDist=dMin;
					}
				}
			}
		}
		else
		{
			break;
		}
	}
	if(NULL!=edVtHdMin[0])
	{
		return YSOK;
	}
	return YSERR;
}


void YsShellExt_EdgeVertexProximityUtil::MoveToNextEdgeVertexPair(EDVTPAIR_HANDLE &hd) const
{
	edVtPairTree.MoveToNext(hd);
}


YsShellExt_EdgeVertexProximityUtil::EdgeVertexPair YsShellExt_EdgeVertexProximityUtil::GetEdgeVertexPair(EDVTPAIR_HANDLE hd) const
{
	return edVtPairTree[hd];
}



YsArray <YsShellExt_EdgeVertexProximityUtil::EDVTPAIR_HANDLE> YsShellExt_EdgeVertexProximityUtil::FindEdgeVertexPairFromEdgePiece(const YsShellVertexHandle edVtHd[2]) const
{
	return edgeToEdVtPairHandle.FindAttrib(edVtHd);
}

YsArray <YsShellExt_EdgeVertexProximityUtil::EDVTPAIR_HANDLE> YsShellExt_EdgeVertexProximityUtil::FindEdgeVertexPairFromEdgePiece(YsShellVertexHandle edVtHd0,YsShellVertexHandle edVtHd1) const
{
	return edgeToEdVtPairHandle.FindAttrib(edVtHd0,edVtHd1);
}

YSSIZE_T YsShellExt_EdgeVertexProximityUtil::GetEdgeIsClosestOfHowManyVertex(const YsShellVertexHandle edVtHd[2]) const
{
	return GetEdgeIsClosestOfHowManyVertex(edVtHd[0],edVtHd[1]);
}

YSSIZE_T YsShellExt_EdgeVertexProximityUtil::GetEdgeIsClosestOfHowManyVertex(YsShellVertexHandle edVtHd0,YsShellVertexHandle edVtHd1) const
{
	auto found=FindEdgeVertexPairFromEdgePiece(edVtHd0,edVtHd1);
	return found.GetN();
}

YsArray <YsShellVertexHandle> YsShellExt_EdgeVertexProximityUtil::FindNearestVertexFromEdgePiece(YsShellVertexHandle edVtHd0,YsShellVertexHandle edVtHd1) const
{
	auto foundPairHd=FindEdgeVertexPairFromEdgePiece(edVtHd0,edVtHd1);
	YsArray <YsShellVertexHandle> nearVtHd;

	if(0<foundPairHd.GetN())
	{
		YsArray <double> nearDist;
		auto &shl=*shlPtr;
		for(auto pairHd : foundPairHd)
		{
			auto pair=GetEdgeVertexPair(pairHd);
			nearVtHd.Append(pair.vtHd);
			nearDist.Append(shl.GetEdgeLength(pair.vtHd,edVtHd0));
		}

		YsQuickSort(nearDist.GetN(),nearDist.GetEditableArray(),nearVtHd.GetEditableArray());
	}

	return nearVtHd;
}

YsArray <YsShellVertexHandle> YsShellExt_EdgeVertexProximityUtil::FindNearestVertexFromEdgePiece(const YsShellVertexHandle edVtHd[2]) const
{
	return FindNearestVertexFromEdgePiece(edVtHd[0],edVtHd[1]);
}

YsShellExt_EdgeVertexProximityUtil::EDVTPAIR_HANDLE YsShellExt_EdgeVertexProximityUtil::FindEdgeVertexPairFromVertex(YsShellVertexHandle vtHd) const
{
	auto hdPtr=vertexToEdVtPairHandle[vtHd];
	if(NULL!=hdPtr)
	{
		return *hdPtr;
	}
	EDVTPAIR_HANDLE hd;
	hd.Nullify();
	return hd;
}


////////////////////////////////////////////////////////////


YsShell::PolygonHandle FindNearestPolygonWithPrecisionFlag(
    double &nearDist,YsVec3 &nearPos,const YsShellExt &shl,const YsVec3 &from,YSBOOL highPrecision,const YsShellExt::Condition &cond)
{
	YsArray <YsVec3,16> plVtPos;
	YSBOOL watch=YSFALSE;

	YsShell::PolygonHandle nearPlHd=nullptr;
	YSBOOL first=YSTRUE;

	nearDist=YsInfinity*YsInfinity;

	for(auto plHd : shl.AllPolygon())
	{
		if(YSOK!=cond.TestPolygon(shl,plHd))
		{
			continue;
		}

		int nPlVt;
		const YsShellVertexHandle *plVtHd;
		YsVec3 plNom,vtPos,plCen;
		double dist;
		YsPlane pln;

		shl.GetVertexListOfPolygon(nPlVt,plVtHd,plHd);
		shl.GetNormalOfPolygon(plNom,plHd);

		plVtPos.Set(0,NULL);
		plCen=YsOrigin();
		for(int j=0; j<nPlVt; j++)
		{
			shl.GetVertexPosition(vtPos,plVtHd[j]);
			dist=(vtPos-from).GetSquareLength();
			plVtPos.Append(vtPos);
			plCen+=vtPos;
			if(first==YSTRUE || dist<nearDist)
			{
				first=YSFALSE;

				nearDist=dist;
				nearPlHd=plHd;
				nearPos=vtPos;
			}
		}

		plCen/=(double)nPlVt;
		if(YSTRUE!=highPrecision && plNom!=YsOrigin())
		{
			pln.Set(plCen,plNom);
		}
		else
		{
			pln.MakeBestFitPlane(nPlVt,plVtPos);
		}

		YSRESULT haveNearPos=(YSTRUE==highPrecision ? pln.GetNearestPointHighPrecision(vtPos,from) : pln.GetNearestPoint(vtPos,from));
		if(haveNearPos==YSOK)
		{
			dist=(vtPos-from).GetSquareLength();
			if(YSTRUE==first || dist<nearDist)
			{
				YSSIDE side;
				if(nPlVt==3)
				{
					side=YsCheckInsideTriangle3(vtPos,plVtPos);
				}
				else if(plNom!=YsOrigin())
				{
					side=YsCheckInsidePolygon3(vtPos,plVtPos.GetN(),plVtPos,plNom);
				}
				else
				{
					side=YsCheckInsidePolygon3(vtPos,plVtPos.GetN(),plVtPos);
				}

				if(side==YSINSIDE || side==YSBOUNDARY)
				{
					first=YSFALSE;

					nearPlHd=plHd;
					nearPos=vtPos;
					nearDist=dist;
				}
			}
		}

		int prevJ=nPlVt-1;
		for(int j=0; j<nPlVt; j++)
		{
			if(YsGetNearestPointOnLine3(vtPos,plVtPos[prevJ],plVtPos[j],from)==YSOK &&
			   YsCheckInBetween3(vtPos,plVtPos[prevJ],plVtPos[j])==YSTRUE)
			{
				dist=(vtPos-from).GetSquareLength();
				if(YSTRUE==first || dist<nearDist)
				{
					first=YSFALSE;

					nearPlHd=plHd;
					nearPos=vtPos;
					nearDist=dist;
				}
			}
			prevJ=j;
		}
	}

	if(nearPlHd!=nullptr)
	{
		nearDist=sqrt(nearDist);
	}

	return nearPlHd;
}

YsShell::PolygonHandle YsShellExt_FindNearestPolygon(
    double &dist,YsVec3 &nearPos,const YsShellExt &shl,const YsVec3 &from,const YsShellExt::Condition &cond)
{
	return FindNearestPolygonWithPrecisionFlag(dist,nearPos,shl,from,YSFALSE,cond);
}

