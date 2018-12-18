/* ////////////////////////////////////////////////////////////

File Name: ysshellext_roundutil.cpp
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

#include "ysshellext_roundutil.h"
#include "ysshellext_boundaryutil.h"
#include "ysshellext_offsetutil.h"
#include "ysshellext_trackingutil.h"

YsShellExt_RoundUtil::YsShellExt_RoundUtil() : angleThr(YsPi/60.0)
{
}

void YsShellExt_RoundUtil::CleanUp(void)
{
	cornerArray.CleanUp();
	targetPlKeyArray.CleanUp();
	targetCeKeyArray.CleanUp();
}

YSRESULT YsShellExt_RoundUtil::SetUpRoundPolygon(const YsShell &shl,YsShellPolygonHandle plHd,const YsShellVertexStore *roundVtx)
{
	YsArray <YsShellVertexHandle> plVtHd;
	shl.GetPolygon(plVtHd,plHd);
	for(YSSIZE_T idx=0; idx<plVtHd.GetN(); ++idx)
	{
		const YsShellVertexHandle fromVtHd=plVtHd[idx];
		if(NULL==roundVtx || YSTRUE==roundVtx->IsIncluded(fromVtHd))
		{
			const YsShellVertexHandle toVtHd[2]=
			{
				plVtHd.GetCyclic(idx-1),
				plVtHd.GetCyclic(idx+1)
			};
			AddRoundCorner(shl,fromVtHd,toVtHd);
		}
	}
	targetPlKeyArray.Append(shl.GetSearchKey(plHd));

	return YSOK;
}

YSRESULT YsShellExt_RoundUtil::SetUpRoundConstEdge(const YsShellExt &shl,YsShellExt::ConstEdgeHandle ceHd,const YsShellVertexStore *roundVtx)
{
	YsArray <YsShellVertexHandle> ceVtHd;
	YSBOOL isLoop;
	shl.GetConstEdge(ceVtHd,isLoop,ceHd);

	if(YSTRUE==isLoop && 3>ceVtHd.GetN())
	{
		return YSERR;
	}
	else if(YSTRUE!=isLoop && 2>ceVtHd.GetN())
	{
		return YSERR;
	}

	if(YSTRUE==isLoop)
	{
		ceVtHd.Append(ceVtHd[0]);
		ceVtHd.Append(ceVtHd[1]);
	}

	for(YSSIZE_T idx=1; idx<ceVtHd.GetN()-1; ++idx)
	{
		if(NULL==roundVtx || YSTRUE==roundVtx->IsIncluded(ceVtHd[idx]))
		{
			const YsShellVertexHandle toVtHd[2]={ceVtHd[idx-1],ceVtHd[idx+1]};
			AddRoundCorner((const YsShell &)shl,ceVtHd[idx],toVtHd);
		}
	}

	targetCeKeyArray.Append(shl.GetSearchKey(ceHd));

	return YSOK;
}

YSRESULT YsShellExt_RoundUtil::AddRoundCorner(const YsShell &,YsShellVertexHandle fromVtHd,const YsShellVertexHandle toVtHd[2])
{
	cornerArray.Increment();
	cornerArray.Last().fromVtHd=fromVtHd;
	cornerArray.Last().toVtHd[0]=toVtHd[0];
	cornerArray.Last().toVtHd[1]=toVtHd[1];
	return YSOK;
}

const double YsShellExt_RoundUtil::CalculateMaximumRadius(const YsShell &shl) const
{
	double maxRadTotal=0.0;
	for(YSSIZE_T idx=0; idx<cornerArray.GetN(); ++idx)
	{
		const RoundCorner &corner=cornerArray[idx];

		const double edgeLen[2]=
		{
			shl.GetEdgeLength(corner.fromVtHd,corner.toVtHd[0]),
			shl.GetEdgeLength(corner.fromVtHd,corner.toVtHd[1])
		};
		const double maxTanDist=YsSmaller(edgeLen[0],edgeLen[1]);
		const double maxRad=CalculateRadiusFromTangentDistance(shl,corner.fromVtHd,corner.toVtHd,maxTanDist);

		if(YsTolerance>maxRad)
		{
			return 0.0;
		}

		if(0==idx || maxRad<maxRadTotal)
		{
			maxRadTotal=maxRad;
		}
	}
	return maxRadTotal;
}

YSRESULT YsShellExt_RoundUtil::CalculateRoundingAll(const YsShell &shl,const double radius,int nDiv)
{
	if(1>nDiv)
	{
		nDiv=1;
	}

	for(auto &corner : cornerArray)
	{
		corner.subDiv[0].CleanUp();
		corner.subDiv[1].CleanUp();

		const YsVec3 vtPos[3]=
		{
			shl.GetVertexPosition(corner.fromVtHd),
			shl.GetVertexPosition(corner.toVtHd[0]),
			shl.GetVertexPosition(corner.toVtHd[1])
		};
		const double edLen[2]=
		{
			(shl.GetVertexPosition(corner.toVtHd[0])-shl.GetVertexPosition(corner.fromVtHd)).GetLength(),
			(shl.GetVertexPosition(corner.toVtHd[1])-shl.GetVertexPosition(corner.fromVtHd)).GetLength()
		};
		const YsVec3 edVec[2]=
		{
			(shl.GetVertexPosition(corner.toVtHd[0])-shl.GetVertexPosition(corner.fromVtHd))/edLen[0],
			(shl.GetVertexPosition(corner.toVtHd[1])-shl.GetVertexPosition(corner.fromVtHd))/edLen[1]
		};
		const double theata=YsPi-acos(YsBound(edVec[0]*edVec[1],-1.0,1.0));
		const double L=radius*tan(theata/2.0);

		if(angleThr>theata)
		{
			return YSERR;
		}

		YsPlane pln[3];
		if(YSOK!=pln[0].MakePlaneFromTriangle(vtPos[0],vtPos[1],vtPos[2]))
		{
			return YSERR;
		}
		pln[1].Set(vtPos[0]+edVec[0]*L,edVec[0]);
		pln[2].Set(vtPos[0]+edVec[1]*L,edVec[1]);

		// Haven't I written three-plane intersection?
		// Plane equation:
		//    (p-o)*n=0
		//    pn-on=0
		//    nx*px+ny*py+nz*pz=on
		{
			YsMatrix3x3 mat;
			YsVec3 rhs;
			for(int i=0; i<3; ++i)
			{
				rhs[i]=pln[i].GetNormal()*pln[i].GetOrigin();
				for(int j=0; j<3; ++j)
				{
					mat.Set(i+1,j+1,pln[i].GetNormal()[j]);
				}
			}

			if(YSOK!=mat.Invert())
			{
				YsPrintf("Cannot calculate the center.\n");
				return YSERR;
			}
			corner.center=mat*rhs;
		}

		corner.roundedCornerPos=YsUnitVector(vtPos[0]-corner.center);
		corner.roundedCornerPos=corner.center+corner.roundedCornerPos*radius;

		for(int edIdx=0; edIdx<2; ++edIdx)
		{
			for(int i=0; i<nDiv; ++i)
			{
				corner.subDiv[edIdx].Increment();
				corner.subDiv[edIdx].Last().vtHd=NULL;
				if(0==i)
				{
					corner.subDiv[edIdx].Last().pos=vtPos[0]+edVec[edIdx]*L;;
				}
				if(0<i)
				{
					const double t=1.0-(double)i/(double)nDiv;
					YsVec3 pos=vtPos[0]+edVec[edIdx]*L*t;

					pos-=corner.center;
					pos.Normalize();
					pos*=radius;
					pos+=corner.center;

					corner.subDiv[edIdx].Last().pos=pos;
				}
			}
		}
	}

	return YSOK;
}

YsArray <YsShellExt_RoundUtil::VertexPositionPair> YsShellExt_RoundUtil::MakeRoundedVertexSequence(const YsShell &shl,YSSIZE_T nVt,const YsShellVertexHandle vtHdArrayIn[],YSBOOL isLoop) const
{
	YsArray <YsShellVertexHandle> orgVtHdArray(nVt,vtHdArrayIn);
	YsArray <VertexPositionPair> newVtHdArray;

	for(YSSIZE_T orgVtIdx=0; orgVtIdx<orgVtHdArray.GetN(); ++orgVtIdx)
	{
		YSBOOL rounded=YSFALSE;
		for(const auto &roundCorner : cornerArray)
		{
			if(roundCorner.fromVtHd==orgVtHdArray[orgVtIdx])
			{
				int forward=0,backward=1;

				if(roundCorner.toVtHd[0]==orgVtHdArray.GetCyclic(orgVtIdx-1))
				{
					forward=1;
					backward=0;
				}
				else if(roundCorner.toVtHd[1]==orgVtHdArray.GetCyclic(orgVtIdx-1))
				{
					forward=0;
					backward=1;
				}
				else
				{
					continue;
				}

				YSBOOL skipFirst=YSFALSE;

				if(0<newVtHdArray.GetN() && newVtHdArray.Last().pos==roundCorner.subDiv[backward].Last().pos)
				{
					skipFirst=YSTRUE;
				}

				newVtHdArray.Append(roundCorner.subDiv[backward]);

				newVtHdArray.Increment();
				newVtHdArray.Last().vtHd=orgVtHdArray[orgVtIdx];
				newVtHdArray.Last().pos=roundCorner.roundedCornerPos;

				for(YSSIZE_T i=roundCorner.subDiv[forward].GetN()-1; 0<=i; --i)
				{
					if(YSTRUE==skipFirst)
					{
						skipFirst=YSFALSE;
						continue;
					}
					newVtHdArray.Append(roundCorner.subDiv[forward][i]);
				}

				rounded=YSTRUE;
			}
		}
		if(YSTRUE!=rounded)
		{
			newVtHdArray.Increment();
			newVtHdArray.Last().vtHd=orgVtHdArray[orgVtIdx];
			newVtHdArray.Last().pos=shl.GetVertexPosition(newVtHdArray.Last().vtHd);
		}
	}

	if(2<=newVtHdArray.GetN() && YSTRUE==isLoop && newVtHdArray[0].pos==newVtHdArray.Last().pos)
	{
		newVtHdArray.DeleteLast();
	}

	return newVtHdArray;
}

/*static*/ double YsShellExt_RoundUtil::CalculateRadiusFromTangentDistance(const YsShell &shl,YsShellVertexHandle fromVtHd,const YsShellVertexHandle toVtHd[2],const double dist)
{
	const double edLen[2]=
	{
		(shl.GetVertexPosition(toVtHd[0])-shl.GetVertexPosition(fromVtHd)).GetLength(),
		(shl.GetVertexPosition(toVtHd[1])-shl.GetVertexPosition(fromVtHd)).GetLength()
	};
	const YsVec3 edVec[2]=
	{
		(shl.GetVertexPosition(toVtHd[0])-shl.GetVertexPosition(fromVtHd))/edLen[0],
		(shl.GetVertexPosition(toVtHd[1])-shl.GetVertexPosition(fromVtHd))/edLen[1]
	};
	const double theata=YsPi-acos(YsBound(edVec[0]*edVec[1],-1.0,1.0));

	const double t=tan(theata/2.0);
	if(YsTolerance>t)
	{
		return 0.0;
	}

	return dist/t;
}

////////////////////////////////////////////////////////////

// See rounding.jnt


YsShellExt_RoundUtil3d::HalfRoundCorner::HalfRoundCorner()
{
	Initialize();
}

void YsShellExt_RoundUtil3d::HalfRoundCorner::Initialize(void)
{
	fromVtHd=NULL;
	toVtHd=NULL;
	roundDir=YsOrigin();
	subDiv.CleanUp();
	offsetUtil=NULL;
	offsetUtil_newVtxIdx=0;

	notReallyDistanceLimited=YSFALSE;
}

YSRESULT YsShellExt_RoundUtil3d::HalfRoundCorner::SubdivideByNumberOfSubdivision(int nDiv)
{
	const YsVec3 v1=YsUnitVector(roundedCornerPos-center);
	const YsVec3 v2=YsUnitVector(foot-center);

	const double fanAngle=acos(YsBound(v1*v2,-1.0,1.0));

	const YsVec3 axis=YsUnitVector(v2^v1);

	if(YsOrigin()==v1 || YsOrigin()==v2 || YsOrigin()==axis)
	{
		return YSERR;
	}

	const YsVec3 baseVec=foot-center;

	subDiv.CleanUp();
	for(int i=0; i<nDiv; ++i)
	{
		subDiv.Increment();

		if(0==i)
		{
			subDiv.Last().vtHd=NULL;
			subDiv.Last().pos=foot;
		}
		else
		{
			const double t=(double)i/(double)nDiv;
			const double angle=fanAngle*t;

			YsVec3 spokeVec;
			YsRotation rot(axis,angle);
			rot.RotatePositive(spokeVec,baseVec);
			subDiv.Last().vtHd=NULL;
			subDiv.Last().pos=spokeVec+center;
		}
	}

	return YSOK;
}

YSRESULT YsShellExt_RoundUtil3d::HalfRoundCorner::SubdivideByStepAngle(const double stepAngle)
{
	const YsVec3 v1=YsUnitVector(roundedCornerPos-center);
	const YsVec3 v2=YsUnitVector(foot-center);

	const double fanAngle=acos(YsBound(v1*v2,-1.0,1.0));

	return SubdivideByNumberOfSubdivision((int)(0.5+fanAngle/stepAngle));
}

////////////////////////////////////////////////////////////

void YsShellExt_RoundUtil3d::RoundEdge::Initialize(void)
{
	edVtHd[0]=NULL;
	edVtHd[1]=NULL;
}

////////////////////////////////////////////////////////////

YsShellExt_RoundUtil3d::YsShellExt_RoundUtil3d()
{
	alwaysUseOffset=YSFALSE;
}

void YsShellExt_RoundUtil3d::CleanUp(void)
{
	cornerArray.CleanUp();
	roundEdgeArray.CleanUp();
	offsetUtil[0].CleanUp();
	offsetUtil[1].CleanUp();
}

void YsShellExt_RoundUtil3d::SetAlwaysUseOffset(YSBOOL alwaysUseOffset)
{
	this->alwaysUseOffset=alwaysUseOffset;
}

YsArray <YsShellExt_RoundUtil3d::HalfRoundCorner *> YsShellExt_RoundUtil3d::MakeSortedHalfRoundCorner(const YsShell &shl)
{
	YsArray <unsigned int> vtKeyArray;
	YsArray <HalfRoundCorner *> cornerPtrArray;
	for(auto &corner : cornerArray)
	{
		vtKeyArray.Append(shl.GetSearchKey(corner.fromVtHd));
		cornerPtrArray.Append(&corner);
	}

	YsQuickSort <unsigned int,HalfRoundCorner *> (vtKeyArray.GetN(),vtKeyArray,cornerPtrArray);

	return cornerPtrArray;
}

YsArray <const YsShellExt_RoundUtil3d::HalfRoundCorner *> YsShellExt_RoundUtil3d::MakeSortedHalfRoundCorner(const YsShell &shl) const
{
	YsArray <unsigned int> vtKeyArray;
	YsArray <const HalfRoundCorner *> cornerPtrArray;
	for(auto &corner : cornerArray)
	{
		vtKeyArray.Append(shl.GetSearchKey(corner.fromVtHd));
		cornerPtrArray.Append(&corner);
	}

	YsQuickSort <unsigned int,const HalfRoundCorner *> (vtKeyArray.GetN(),vtKeyArray,cornerPtrArray);

	return cornerPtrArray;
}

YSRESULT YsShellExt_RoundUtil3d::CalculateRoundingDirectionAll(const YsShell &shl)
{
	YsArray <HalfRoundCorner *> cornerPtrArray=MakeSortedHalfRoundCorner(shl);

	YsArray <HalfRoundCorner *> cornerPerVertex;
	for(YSSIZE_T idx=0; idx<cornerPtrArray.GetN(); ++idx)
	{
		cornerPerVertex.Append(cornerPtrArray[idx]);
		if(cornerPtrArray.GetN()-1==idx || cornerPtrArray[idx]->fromVtHd!=cornerPtrArray[idx+1]->fromVtHd)
		{
			const YsVec3 roundDir=CalculateRoundingDirction(shl,cornerPerVertex);
			if(YsOrigin()==roundDir)
			{
				return YSERR;
			}
			for(auto ptr : cornerPerVertex)
			{
				ptr->roundDir=roundDir;
			}
			cornerPerVertex.CleanUp();
		}
	}

	return YSOK;
	
}

YsVec3 YsShellExt_RoundUtil3d::CalculateRoundingDirction(const YsShell &shl,YsShellVertexHandle fromVtHd,YSSIZE_T nTo,const YsShellVertexHandle toVtHd[]) const
{
	const YsVec3 fromPos=shl.GetVertexPosition(fromVtHd);
	YsVec3 vecSum=YsOrigin();
	for(YSSIZE_T idx=0; idx<nTo; ++idx)
	{
		const YsVec3 vec=YsUnitVector(shl.GetVertexPosition(toVtHd[idx])-fromPos);
		vecSum+=vec;
	}
	vecSum.Normalize();
	return vecSum;
}

YsVec3 YsShellExt_RoundUtil3d::CalculateRoundingDirction(const YsShell &shl,const YsArray <HalfRoundCorner *> &roundCornerPerVertex) const
{
	const YsVec3 fromPos=shl.GetVertexPosition(roundCornerPerVertex[0]->fromVtHd);
	YsVec3 vecSum=YsOrigin();
	for(auto corner : roundCornerPerVertex)
	{
		const YsVec3 vec=YsUnitVector(corner->toPos-fromPos);
		vecSum+=vec;
	}
	vecSum.Normalize();
	return vecSum;
}

// moveDir must be a unit vector.
YSRESULT YsShellExt_RoundUtil3d::CalculateCenterFromRadius(YsVec3 &center,const YsVec3 &fromPos,const YsVec3 &toPos,const YsVec3 &moveDir,const double radius) const
{
	const YsVec3 edgeVec=YsUnitVector(toPos-fromPos);

	if(YsOrigin()==moveDir || YsOrigin()==edgeVec)
	{
		return YSERR;
	}

	const double theata=acos(edgeVec*moveDir);
	const double s=sin(theata);
	if(YsTolerance>s)
	{
		return YSERR;
	}

	const double t=(radius/s)-radius;
	center=fromPos+moveDir*(radius+t);
	return YSOK;
}

const double YsShellExt_RoundUtil3d::CalculateRadiusFromLimit(const YsVec3 &fromPos,const YsVec3 &toPos,const YsVec3 &moveDir,const double L) const
{
	const YsVec3 edgeVec=YsUnitVector(toPos-fromPos);

	if(YsOrigin()==moveDir || YsOrigin()==edgeVec)
	{
		return 0.0;
	}

	const double theata=acos(edgeVec*moveDir);

	return L*tan(theata);
}

const double YsShellExt_RoundUtil3d::CalculateDisplacementFromRadius(const YsVec3 &fromPos,const YsVec3 &toPos,const YsVec3 &moveDir,const double radius) const
{
	const YsVec3 edgeVec=YsUnitVector(toPos-fromPos);

	if(YsOrigin()==moveDir || YsOrigin()==edgeVec)
	{
		return 0.0;
	}

	const double theata=acos(edgeVec*moveDir);
	const double s=sin(theata);
	if(YsTolerance>s)
	{
		return 0.0;
	}

	return (radius/s)-radius;
}

const double YsShellExt_RoundUtil3d::CalculateRadiusFromDisplacement(const YsVec3 &fromPos,const YsVec3 &toPos,const YsVec3 &moveDir,const double t) const
{
	// t=(r/sin)-r
	// t*sin=r-r*sin
	// t*sin=(1-sin)*r
	// r=t*sin/(1-sin)

	const YsVec3 edgeVec=YsUnitVector(toPos-fromPos);

	if(YsOrigin()==moveDir || YsOrigin()==edgeVec)
	{
		return 0.0;
	}

	const double theata=acos(edgeVec*moveDir);
	const double s=sin(theata);
	if(YsTolerance>1.0-s)
	{
		return 0.0;
	}

	return (t*s)/(1.0-s);
}

// 1. Calculate max radius and then max displacement for each edge.
// 2. tmin=minimum of the maximum displacements.
// 3. Calculate radius from the tmin for each edge.
YSRESULT YsShellExt_RoundUtil3d::SetUpRoundVertex(const YsShell &shl,YSSIZE_T nVt,const YsShellVertexHandle vtHdArray[])
{
	const YsShellSearchTable *search=shl.GetSearchTable();
	if(NULL==search)
	{
		YsPrintf("%s\n",__FUNCTION__);
		YsPrintf("  This function requires a search table.\n");
		return YSERR;
	}

	CleanUp();
	for(YSSIZE_T vtIdx=0; vtIdx<nVt; ++vtIdx)
	{
		YsArray <YsShellVertexHandle> connVtHdArray;
		if(YSOK==search->GetConnectedVertexList(connVtHdArray,shl,vtHdArray[vtIdx]))
		{
			for(auto connVtHd : connVtHdArray)
			{
				cornerArray.Increment();
				cornerArray.Last().Initialize();
				cornerArray.Last().fromVtHd=vtHdArray[vtIdx];
				cornerArray.Last().toVtHd=connVtHd;
				cornerArray.Last().toPos=shl.GetVertexPosition(connVtHd);
			}
		}
	}

	if(YSOK!=CalculateRoundingDirectionAll(shl))
	{
		CleanUp();
		return YSERR;
	}

	return YSOK;
}

YSRESULT YsShellExt_RoundUtil3d::SetUpForAroundPolygonGroup(
    const YsShellExt &shl,
    YSSIZE_T nPl,const YsShellPolygonHandle plHdArray[])
{
	CleanUp();

	if(YSTRUE!=shl.IsSearchEnabled())
	{
		YsPrintf("%s\n",__FUNCTION__);
		YsPrintf("  This function requires a search table.\n");
		return YSERR;
	}

	YsShellExt_BoundaryInfo boundary;

	boundary.MakeInfo((const YsShell &)shl,nPl,plHdArray);
	if(YSOK!=boundary.CacheContour((const YsShell &)shl))
	{
		return YSERR;
	}

	YsShellPolygonStore sideAPolygon((const YsShell &)shl),sideBPolygon((const YsShell &)shl);
	sideAPolygon.AddPolygon(nPl,plHdArray);

	YsArray <YsShellPolygonHandle> sideAPolygonArray(nPl,plHdArray),sideBPolygonArray;

	YsShellEdgeStore boundaryEdge((const YsShell &)shl);
	for(YSSIZE_T contourIdx=0; contourIdx<boundary.GetNumContour(); ++contourIdx)
	{
		YsArray <YsShellVertexHandle> contourVtHd;
		boundary.GetContour(contourVtHd,contourIdx);

		if(3<=contourVtHd.GetN())
		{
			if(contourVtHd[0]!=contourVtHd.Last())
			{
				const YsShellVertexHandle vtHd0=contourVtHd[0];
				contourVtHd.Append(vtHd0);
			}

			for(YSSIZE_T vtIdx=0; vtIdx<contourVtHd.GetN()-1; ++vtIdx)
			{
				boundaryEdge.AddEdge(contourVtHd[vtIdx],contourVtHd[vtIdx+1]);

				YSSIZE_T nVtPl;
				const YsShellPolygonHandle *vtPlHd;
				shl.FindPolygonFromVertex(nVtPl,vtPlHd,contourVtHd[vtIdx]);

				for(YSSIZE_T plIdx=0; plIdx<nVtPl; ++plIdx)
				{
					if(YSTRUE!=sideAPolygon.IsIncluded(vtPlHd[plIdx]) &&
					   YSTRUE!=sideBPolygon.IsIncluded(vtPlHd[plIdx]))
					{
						sideBPolygon.AddPolygon(vtPlHd[plIdx]);
						sideBPolygonArray.Append(vtPlHd[plIdx]);
					}
				}
			}
		}
	}
	return SetUpForVertexSequenceAndPolygonArray(
	    shl,
	    boundary.GetContourAll(),
	    sideAPolygonArray,
	    sideBPolygonArray);
}

YSRESULT YsShellExt_RoundUtil3d::SetUpForVertexSequenceAndPolygonArray(
    const YsShellExt &shl,
    const YsArray <YsArray <YsShellVertexHandle> > &allBoundary,
    const YsArray <YsShellPolygonHandle> &sideAPolygonArray,
    const YsArray <YsShellPolygonHandle> &sideBPolygonArray)
{
	YsHashTable <YSSIZE_T> vtKeyToBoundaryIdx;
	YsShellEdgeStore boundaryEdge((const YsShell &)shl);
	for(auto idx : allBoundary.AllIndex())
	{
		YsArray <YsShellVertexHandle> contourVtHd=allBoundary[idx];

		for(YSSIZE_T vtIdx=0; vtIdx<contourVtHd.GetN()-1; ++vtIdx)
		{
			if(YSTRUE!=boundaryEdge.IsIncluded(contourVtHd[vtIdx],contourVtHd[vtIdx+1]))
			{
				boundaryEdge.AddEdge(contourVtHd[vtIdx],contourVtHd[vtIdx+1]);
				roundEdgeArray.Increment();
				roundEdgeArray.Last().Initialize();
				roundEdgeArray.Last().edVtHd[0]=contourVtHd[vtIdx];
				roundEdgeArray.Last().edVtHd[1]=contourVtHd[vtIdx+1];
			}
		}
		for(auto vtHd : contourVtHd)
		{
			vtKeyToBoundaryIdx.AddElement(shl.GetSearchKey(vtHd),idx);
		}
	}

	// Boundary edge hash and sideBPolygon store have been constructed.
	YsShellExt_OffsetUtil2d &sideAOffset=offsetUtil[0],&sideBOffset=offsetUtil[1];

	YsShellPolygonStore sideAPolygon((const YsShell &)shl),sideBPolygon((const YsShell &)shl);
	sideAPolygon.AddPolygon(sideAPolygonArray);
	sideBPolygon.AddPolygon(sideBPolygonArray);



	YSBOOL sideAOffsetNecessary=YSFALSE,sideBOffsetNecessary=YSFALSE;
	if(YSTRUE==alwaysUseOffset)
	{
		sideAOffsetNecessary=YSTRUE;
		sideBOffsetNecessary=YSTRUE;
	}
	else
	{
		for(auto boundary : allBoundary)
		{
			YsArray <YsShellVertexHandle> contourVtHd=boundary;

			for(auto vtHd : contourVtHd)
			{
				YsArray <YsShellVertexHandle,16> connVtHdArray;
				shl.GetConnectedVertex(connVtHdArray,vtHd);

				YSBOOL sideAEdgePresent=YSFALSE,sideBEdgePresent=YSFALSE;
				for(auto connVtHd : connVtHdArray)
				{
					if(YSTRUE==boundaryEdge.IsIncluded(vtHd,connVtHd))
					{
						continue;
					}

					if(YSTRUE==IsEdgeUsing(vtHd,connVtHd,shl.Conv(),sideAPolygon))
					{
						sideAEdgePresent=YSTRUE;
					}
					else if(YSTRUE==IsEdgeUsing(vtHd,connVtHd,shl.Conv(),sideBPolygon))
					{
						sideBEdgePresent=YSTRUE;
					}
				}
				if(YSTRUE!=sideAEdgePresent)
				{
					sideAOffsetNecessary=YSTRUE;
				}
				if(YSTRUE!=sideBEdgePresent)
				{
					sideBOffsetNecessary=YSTRUE;
				}
			}
		}
	}

	printf("%s %d\n",__FUNCTION__,__LINE__);
	printf("%d %d\n",sideAOffsetNecessary,sideBOffsetNecessary);

	if(YSTRUE==sideAOffsetNecessary)
	{
		if(YSOK!=sideAOffset.SetUpForOneSideOfVertexSequence(shl,allBoundary,sideAPolygonArray))
		{
			return YSERR;
		}
		for(YSSIZE_T idx=0; idx<sideAOffset.newVtxArray.GetN(); ++idx)
		{
			auto newVtx=sideAOffset.newVtxArray[idx];
			cornerArray.Increment();
			cornerArray.Last().Initialize();
			cornerArray.Last().fromVtHd=newVtx.fromVtHd;
			cornerArray.Last().toVtHd=NULL;
			cornerArray.Last().toPos=newVtx.fromPos+newVtx.dir*newVtx.maxDist;
			cornerArray.Last().offsetUtil=&sideAOffset;
			cornerArray.Last().offsetUtil_newVtxIdx=idx;
			cornerArray.Last().notReallyDistanceLimited=IsPolygonOnTheSameContour(shl,newVtx.limitingPlHd,vtKeyToBoundaryIdx);
		}
	}
	if(YSTRUE==sideBOffsetNecessary)
	{
		if(YSOK!=sideBOffset.SetUpForOneSideOfVertexSequence(shl,allBoundary,sideBPolygonArray))
		{
			return YSERR;
		}
		for(YSSIZE_T idx=0; idx<sideBOffset.newVtxArray.GetN(); ++idx)
		{
			auto newVtx=sideBOffset.newVtxArray[idx];
			cornerArray.Increment();
			cornerArray.Last().Initialize();
			cornerArray.Last().fromVtHd=newVtx.fromVtHd;
			cornerArray.Last().toVtHd=NULL;
			cornerArray.Last().toPos=newVtx.fromPos+newVtx.dir*newVtx.maxDist;
			cornerArray.Last().offsetUtil=&sideBOffset;
			cornerArray.Last().offsetUtil_newVtxIdx=idx;
			cornerArray.Last().notReallyDistanceLimited=IsPolygonOnTheSameContour(shl,newVtx.limitingPlHd,vtKeyToBoundaryIdx);
		}
	}

printf("%s %d\n",__FUNCTION__,__LINE__);

	for(auto boundary : allBoundary)
	{
		YsArray <YsShellVertexHandle> contourVtHd=boundary;

		if(3<=contourVtHd.GetN())
		{
			if(contourVtHd[0]==contourVtHd.Last())
			{
				contourVtHd.DeleteLast();
			}
		}

		for(auto vtHd : contourVtHd)
		{
			YsArray <YsShellVertexHandle,16> connVtHdArray;
			shl.GetConnectedVertex(connVtHdArray,vtHd);

			for(auto connVtHd : connVtHdArray)
			{
				if(YSTRUE==boundaryEdge.IsIncluded(vtHd,connVtHd))
				{
					continue;
				}
				else if((YSTRUE!=sideAOffsetNecessary && YSTRUE==IsEdgeUsing(vtHd,connVtHd,(const YsShell &)shl,sideAPolygon)) ||
				        (YSTRUE!=sideBOffsetNecessary && YSTRUE==IsEdgeUsing(vtHd,connVtHd,(const YsShell &)shl,sideBPolygon)))
				{
					cornerArray.Increment();
					cornerArray.Last().Initialize();
					cornerArray.Last().fromVtHd=vtHd;
					cornerArray.Last().toVtHd=connVtHd;
					cornerArray.Last().toPos=shl.GetVertexPosition(connVtHd);
				}
			}
		}
	}

printf("%s %d\n",__FUNCTION__,__LINE__);

	for(YSSIZE_T cornerIdx=0; cornerIdx<cornerArray.GetN(); ++cornerIdx)
	{
		if(NULL!=cornerArray[cornerIdx].offsetUtil)
		{
			cornerArray[cornerIdx].offsetUtil->newVtxArray[cornerArray[cornerIdx].offsetUtil_newVtxIdx].cornerIdx=cornerIdx;
		}
	}

printf("%s %d\n",__FUNCTION__,__LINE__);

	if(YSOK==CalculateRoundingDirectionAll((const YsShell &)shl))
	{
printf("%s %d\n",__FUNCTION__,__LINE__);
		return YSOK;
	}

printf("%s %d\n",__FUNCTION__,__LINE__);
	CleanUp();
	return YSERR;
}

YSBOOL YsShellExt_RoundUtil3d::IsPolygonOnTheSameContour(const YsShellExt &shl,YsShellPolygonHandle plHd,const YsHashTable <YSSIZE_T> &vtKeyToBoundaryIdx)
{
	auto plVtHd=shl.GetPolygonVertex(plHd);
	if(0<plVtHd.GetN())
	{
		YSSIZE_T boundaryIdx0;
		if(YSOK!=vtKeyToBoundaryIdx.FindElement(boundaryIdx0,shl.GetSearchKey(plVtHd[0])))
		{
			return YSFALSE;
		}

		for(auto vtHd : plVtHd)
		{
			YSSIZE_T boundaryIdx;
			if(YSOK!=vtKeyToBoundaryIdx.FindElement(boundaryIdx,shl.GetSearchKey(vtHd)) || boundaryIdx!=boundaryIdx0)
			{
				return YSFALSE;
			}
		}

		return YSTRUE;
	}
	return YSFALSE;
}

YSRESULT YsShellExt_RoundUtil3d::SetUpForVertexSequence(
	const YsShellExt &shl,
	YSSIZE_T nVt,const YsShellVertexHandle vtHdArray[])
{
	CleanUp();

	if(2>nVt || YSTRUE!=shl.IsSearchEnabled())
	{
		return YSERR;
	}

	YSSIZE_T nEdPl;
	const YsShellPolygonHandle *edPlHd;
	if(YSOK==shl.FindPolygonFromEdgePiece(nEdPl,edPlHd,vtHdArray[0],vtHdArray[1]) && 2==nEdPl)
	{
		YsArray <YsShellPolygonHandle> sideAPolygonArray=YsShellExt_TrackingUtil::TrackPolygonOneSideOfVertexSequence(
		    shl,nVt,vtHdArray,1,edPlHd);
		YsArray <YsShellPolygonHandle> sideBPolygonArray=YsShellExt_TrackingUtil::TrackPolygonOneSideOfVertexSequence(
		    shl,nVt,vtHdArray,1,edPlHd+1);

		if(0==sideAPolygonArray.GetN() || 0==sideBPolygonArray.GetN())
		{
			return YSERR;
		}

		YsArray <YsArray <YsShellVertexHandle> > allBoundary(1,NULL);
		allBoundary[0].Set(nVt,vtHdArray);
		return YsShellExt_RoundUtil3d::SetUpForVertexSequenceAndPolygonArray(shl,allBoundary,sideAPolygonArray,sideBPolygonArray);
	}
	return YSERR;
}

const double YsShellExt_RoundUtil3d::CalculateMaximumRadius(const YsShell &shl) const
{
	if(0==cornerArray.GetN())
	{
printf("%s %d\n",__FUNCTION__,__LINE__);
		return 0.0;
	}

	double maxRadius=YsInfinity;
	for(auto corner : cornerArray)
	{
		if(YSTRUE==corner.notReallyDistanceLimited)
		{
			continue;
		}

		YsVec3 toVec=corner.toPos-shl.GetVertexPosition(corner.fromVtHd);
		const double L=toVec.GetLength();
		if(YsTolerance>L)
		{
printf("%s %d\n",__FUNCTION__,__LINE__);
			return 0.0;
		}
		toVec/=L;

		YsVec3 cenVec=corner.roundDir;
		if(YSOK!=cenVec.Normalize() || YsTolerance>cenVec*toVec)
		{
printf("%s\n",cenVec.Txt());
printf("%s\n",toVec.Txt());
printf("%lf\n",cenVec*toVec);
printf("%s %d\n",__FUNCTION__,__LINE__);
			return 0.0;
		}

		const double theata=acos(YsBound(toVec*cenVec,-1.0,1.0));

		const double maxRadiusForCorner=L*tan(theata);

		if(maxRadiusForCorner<maxRadius)
		{
			printf("Max radius limited by %s\n",shl.GetVertexPosition(corner.fromVtHd).Txt());
			maxRadius=maxRadiusForCorner;
		}
	}

	return maxRadius;
}

YSRESULT YsShellExt_RoundUtil3d::CalculateRoundingAll(const YsShell &shl,const double radius)
{
	YsArray <unsigned int> vtKeyArray;
	YsArray <HalfRoundCorner *> cornerPtrArray;
	for(auto &corner : cornerArray)
	{
		vtKeyArray.Append(shl.GetSearchKey(corner.fromVtHd));
		cornerPtrArray.Append(&corner);
	}

	YsQuickSort <unsigned int,HalfRoundCorner *> (vtKeyArray.GetN(),vtKeyArray,cornerPtrArray);

	YsArray <HalfRoundCorner *> cornerPerVertex;
	for(YSSIZE_T idx=0; idx<cornerPtrArray.GetN(); ++idx)
	{
		cornerPerVertex.Append(cornerPtrArray[idx]);
		if(cornerPtrArray.GetN()-1==idx || vtKeyArray[idx]!=vtKeyArray[idx+1])
		{
			if(YSOK!=CalculateRoundingPerVertex(shl,cornerPerVertex,radius))
			{
				return YSERR;
			}
			cornerPerVertex.CleanUp();
		}
	}

	return YSOK;
}

YSRESULT YsShellExt_RoundUtil3d::SubdivideByNumberOfSubdivision(int nDiv)
{
	for(auto &corner : cornerArray)
	{
		if(YSOK!=corner.SubdivideByNumberOfSubdivision(nDiv))
		{
			return YSERR;
		}
	}
	return YSOK;
}

YSRESULT YsShellExt_RoundUtil3d::SubdivideByStepAngle(const double stepAngle)
{
	for(auto &corner : cornerArray)
	{
		if(YSOK!=corner.SubdivideByStepAngle(stepAngle))
		{
			return YSERR;
		}
	}
	return YSOK;
}

// 1. Calculate max radius and then max displacement for each edge.
// 2. tmin=minimum of the maximum displacements.
// 3. Calculate radius from the tmin for each edge.
YSRESULT YsShellExt_RoundUtil3d::CalculateRoundingPerVertex(const YsShell &shl,const YsArray <YsShellExt_RoundUtil3d::HalfRoundCorner *> &roundCornerPerVertex,const double radius) const
{
	const YsVec3 m=CalculateRoundingDirction(shl,roundCornerPerVertex);
	const YsVec3 fromPos=shl.GetVertexPosition(roundCornerPerVertex[0]->fromVtHd);

	double tMin=-1.0;
	for(auto corner : roundCornerPerVertex)
	{
		const YsVec3 toPos=corner->toPos;

		const double L=(toPos-fromPos).GetLength();
		const double r=(YSTRUE==corner->notReallyDistanceLimited ? radius : YsSmaller(radius,CalculateRadiusFromLimit(fromPos,toPos,m,L)));

		const double t=CalculateDisplacementFromRadius(fromPos,toPos,m,r);

		if(-0.1>tMin || t<tMin)
		{
			tMin=t;
		}
	}

	if(YsTolerance>tMin)
	{
		return YSERR;
	}

	const YsVec3 roundedCornerPos=fromPos+m*tMin;

	for(auto corner : roundCornerPerVertex)
	{
		const YsVec3 toPos=corner->toPos;

		const double r=CalculateRadiusFromDisplacement(fromPos,toPos,m,tMin);
		if(YsTolerance>r)
		{
			return YSERR;
		}

		YsVec3 center;
		if(YSOK!=CalculateCenterFromRadius(center,fromPos,toPos,m,r))
		{
			return YSERR;
		}


		corner->roundedCornerPos=roundedCornerPos;
		YsGetNearestPointOnLine3(corner->foot,fromPos,toPos,center);
		corner->radius=r;
		corner->center=center;
	}

	return YSOK;
}

YSBOOL YsShellExt_RoundUtil3d::IsEdgeUsing(const YsShellVertexHandle edVtHd0,const YsShellVertexHandle edVtHd1,const YsShell &shl,const YsShellPolygonStore &polygonStore) const
{
	const YsShellSearchTable *search=shl.GetSearchTable();

	int nEdPl;
	const YsShellPolygonHandle *edPlHd;
	if(YSOK==search->FindPolygonListByEdge(nEdPl,edPlHd,shl,edVtHd0,edVtHd1))
	{
		for(int plIdx=0; plIdx<nEdPl; ++plIdx)
		{
			if(YSTRUE==polygonStore.IsIncluded(edPlHd[plIdx]))
			{
				return YSTRUE;
			}
		}
	}
	return YSFALSE;
}

YsArray <YSSIZE_T> YsShellExt_RoundUtil3d::FindHalfRoundCornerFromPolygon(const YsShell &shl,YsShellPolygonHandle plHd) const
{
	YsArray <YsShellVertexHandle,4> plVtHd;
	shl.GetPolygon(plVtHd,plHd);

	YsArray <YSSIZE_T> idxArray;
	for(YSSIZE_T i=0; i<plVtHd.GetN(); ++i)
	{
		for(YSSIZE_T j=0; j<cornerArray.GetN(); ++j)
		{
			if(plVtHd[i]==cornerArray[j].fromVtHd && YSTRUE==plVtHd.IsIncluded(cornerArray[j].toVtHd))
			{
				idxArray.Append(j);
			}
		}
	}

	return idxArray;
}

