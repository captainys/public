/* ////////////////////////////////////////////////////////////

File Name: ysshellext_geomcalc.cpp
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

#include <ysshellext.h>
#include <ysshellextmisc.h>

#include "ysshellext_geomcalc.h"
#include "ysshellext_projectionutil.h"



const double YsShellExt_CalculateDihedralAngle(const YsShellExt &shl,const YsShellPolygonHandle plHd[2])
{
	return YsShellExt_CalculateDihedralAngle(shl,plHd[0],plHd[1]);
}

const double YsShellExt_CalculateDihedralAngle(const YsShellExt &shl,YsShellPolygonHandle plHd0,YsShellPolygonHandle plHd1)
{
	YsArray <YsVec3,4> plVtPos[2];
	shl.GetPolygon(plVtPos[0],plHd0);
	shl.GetPolygon(plVtPos[1],plHd1);

	YsVec3 nom[2];
	if(YSOK==YsGetAverageNormalVector(nom[0],plVtPos[0]) && YSOK==YsGetAverageNormalVector(nom[1],plVtPos[1]))
	{
		return acos(YsBound(nom[0]*nom[1],-1.0,1.0));
	}
	return 0.0;
}



const double YsShellExt_CalculateDihedralAngle(const YsShellExt &shl,const YsShellVertexHandle edVtHd[2])
{
	return YsShellExt_CalculateDihedralAngle(shl,edVtHd[0],edVtHd[1]);
}

const double YsShellExt_CalculateDihedralAngle(const class YsShellExt &shl,const YsShellVertexHandle edVtHd0,const YsShellVertexHandle edVtHd1)
{
	YsArray <YsShellPolygonHandle,2> edPlHd;
	if(YSOK==shl.FindPolygonFromEdgePiece(edPlHd,edVtHd0,edVtHd1) && 2==edPlHd.GetN())
	{
		return YsShellExt_CalculateDihedralAngle(shl,edPlHd);
	}
	return 0.0;
}

const double YsShellExt_CalculateDihedralAngle(const class YsShellExt &shl,const YsShellPolygonHandle plHd,int nThEdgeZeroBased)
{
	YsArray <YsShellVertexHandle,4> plVtHd;
	shl.GetPolygon(plVtHd,plHd);
	return YsShellExt_CalculateDihedralAngle(shl,plVtHd[nThEdgeZeroBased],plVtHd.GetCyclic(nThEdgeZeroBased+1));
}



const double YsShellExt_CalculateTotalDihedralAngleAroundEdge(const class YsShellExt &shl,const YsShellVertexHandle edVtHd[2])
{
	return YsShellExt_CalculateTotalDihedralAngleAroundEdge(shl,edVtHd[0],edVtHd[1]);
}

const double YsShellExt_CalculateTotalDihedralAngleAroundEdge(const class YsShellExt &shl,const YsShellVertexHandle edVtHd0,const YsShellVertexHandle edVtHd1)
{
	YsArray <YsShellPolygonHandle,2> basePlHdPair;
	if(YSOK==shl.FindPolygonFromEdgePiece(basePlHdPair,edVtHd0,edVtHd1) && 2==basePlHdPair.GetN())
	{
		double total=YsShellExt_CalculateDihedralAngle(shl,basePlHdPair);
		for(auto plHd : basePlHdPair)
		{
			YsArray <YsShellVertexHandle,4> plVtHd;
			shl.GetPolygon(plVtHd,plHd);
			for(auto vtIdx : plVtHd.AllIndex())
			{
				if(YSTRUE!=YsSameEdge(plVtHd[vtIdx],plVtHd.GetCyclic(vtIdx+1),edVtHd0,edVtHd1))
				{
					total+=YsShellExt_CalculateDihedralAngle(shl,plVtHd[vtIdx],plVtHd.GetCyclic(vtIdx+1));
				}
			}
		}
		return total;
	}
	return 0.0;
}


const double YsShellExt_CalculateDihedralAngle(const class YsShellExt &shl,const YsShellVertexPositionSource &vtPosSrc,const YsShellPolygonHandle plHd[2])
{
	return YsShellExt_CalculateDihedralAngle(shl,vtPosSrc,plHd[0],plHd[1]);
}

const double YsShellExt_CalculateDihedralAngle(const class YsShellExt &shl,const YsShellVertexPositionSource &vtPosSrc,YsShellPolygonHandle plHd0,YsShellPolygonHandle plHd1)
{
	YsArray <YsVec3,4> plVtPos[2];
	for(int i=0; i<2; ++i)
	{
		auto plHd=(0==i ? plHd0 : plHd1);
		int nPlVt=shl.GetPolygonNumVertex(plHd);
		const YsShell::VertexHandle *plVtHd;

		shl.GetPolygon(nPlVt,plVtHd,plHd);
		plVtPos[i].Set(nPlVt,nullptr);
		for(int j=0; j<nPlVt; ++j)
		{
			plVtPos[i][j]=vtPosSrc.GetVertexPosition(plVtHd[j]);
		}
	}

	YsVec3 nom[2];
	if(YSOK==YsGetAverageNormalVector(nom[0],plVtPos[0]) && YSOK==YsGetAverageNormalVector(nom[1],plVtPos[1]))
	{
		return acos(YsBound(nom[0]*nom[1],-1.0,1.0));
	}
	return 0.0;
}


const double YsShellExt_CalculateDihedralAngle(const class YsShellExt &shl,const YsShellVertexPositionSource &vtPosSrc,const YsShellVertexHandle edVtHd[2])
{
	return YsShellExt_CalculateDihedralAngle(shl,vtPosSrc,edVtHd[0],edVtHd[1]);
}

const double YsShellExt_CalculateDihedralAngle(const class YsShellExt &shl,const YsShellVertexPositionSource &vtPosSrc,const YsShellVertexHandle edVtHd0,const YsShellVertexHandle edVtHd1)
{
	YsArray <YsShellPolygonHandle,2> edPlHd;
	if(YSOK==shl.FindPolygonFromEdgePiece(edPlHd,edVtHd0,edVtHd1) && 2==edPlHd.GetN())
	{
		return YsShellExt_CalculateDihedralAngle(shl,vtPosSrc,edPlHd);
	}
	return 0.0;
}

const double YsShellExt_CalculateDihedralAngle(const class YsShellExt &shl,const YsShellVertexPositionSource &vtPosSrc,const YsShellPolygonHandle plHd,int nThEdgeZeroBased)
{
	YsArray <YsShellVertexHandle,4> plVtHd;
	shl.GetPolygon(plVtHd,plHd);
	return YsShellExt_CalculateDihedralAngle(shl,vtPosSrc,plVtHd[nThEdgeZeroBased],plVtHd.GetCyclic(nThEdgeZeroBased+1));
}



const YsVec3 YsShell_CalculatePolygonAverageNormal(const YsShell &shl,YSSIZE_T nPl,const YsShellPolygonHandle plHdArray[])
{
	YsArray <YsVec3,16> allVtPos;
	for(YSSIZE_T plIdx=0; plIdx<nPl; ++plIdx)
	{
		YsArray <YsShellVertexHandle,4> plVtHd;
		shl.GetPolygon(plVtHd,plHdArray[plIdx]);
		for(auto vtHd : plVtHd)
		{
			allVtPos.Append(shl.GetVertexPosition(vtHd));
		}
	}
	YsVec3 nom;
	if(YSOK==YsFindLeastSquareFittingPlaneNormal(nom,allVtPos))
	{
		return nom;
	}
	return YsOrigin();
}

const YsVec3 YsShell_CalculateAreaWeightedPolygonNormal(const YsShell &shl,YsShell::PolygonHandle plHd)
{
	YsArray <YsVec3,4> plVtPos;
	shl.GetPolygon(plVtPos,plHd);
	return shl.GetPolygonArea(plHd)*YsGetAverageNormalVector(plVtPos);
}

const YsVec3 YsShell_CalculateAreaWeightedSumOfPolygonNormal(const YsShell &shl,YSSIZE_T nPl,const YsShell::PolygonHandle plHdArray[])
{
	YsVec3 nomSum=YsOrigin();
	for(YSSIZE_T idx=0; idx<nPl; ++idx)
	{
		nomSum+=YsShell_CalculateAreaWeightedPolygonNormal(shl,plHdArray[idx]);
	}
	return nomSum;
}

const YsVec3 YsShell_CalculateNormal(const YsShell &shl,YsShell::PolygonHandle plHd)
{
	YsArray <YsVec3,4> plVtPos;
	shl.GetPolygon(plVtPos,plHd);
	return YsGetAverageNormalVector(plVtPos);
}


////////////////////////////////////////////////////////////

YsShellExt_NormalCalculator::YsShellExt_NormalCalculator()
{
	weight=NO_WEIGHT;
	limitFgHd=NULL;
}

YsVec3 YsShellExt_NormalCalculator::CalculateVertexNormal(const YsShellExt &shl,YsShellVertexHandle vtHd) const
{
	YsVec3 nomSum=YsOrigin();

	YSSIZE_T nVtPl;
	const YsShellPolygonHandle *vtPlHd;
	if(YSOK==shl.FindPolygonFromVertex(nVtPl,vtPlHd,vtHd))
	{
		for(auto idx=nVtPl-1; 0<=idx; --idx)
		{
			if(NULL!=limitFgHd && shl.FindFaceGroupFromPolygon(vtPlHd[idx])!=limitFgHd)
			{
				continue;
			}

			double weight=1.0;
			nomSum+=weight*shl.GetNormal(vtPlHd[idx]);
		}
		nomSum.Normalize();
	}

	return nomSum;
}

YsVec3 YsShellExt_NormalCalculator::CalculateEdgeNormal(const YsShellExt &shl,const YsShellVertexHandle edVtHd[2]) const
{
	YsVec3 nomSum=YsOrigin();

	YSSIZE_T nEdPl;
	const YsShellPolygonHandle *edPlHd;
	if(YSOK==shl.FindPolygonFromEdgePiece(nEdPl,edPlHd,edVtHd))
	{
		for(auto idx=nEdPl-1; 0<=idx; --idx)
		{
			if(NULL!=limitFgHd && shl.FindFaceGroupFromPolygon(edPlHd[idx])!=limitFgHd)
			{
				continue;
			}

			double weight=1.0;
			nomSum+=weight*shl.GetNormal(edPlHd[idx]);
		}
		nomSum.Normalize();
	}

	return nomSum;
}

void YsShellExt_CalculateQuadricErrorMetric(YsMatrix4x4 &Kp,const YsShellExt &shl,YsShellVertexHandle vtHd)
{
	auto vtPlHd=shl.FindPolygonFromVertex(vtHd);

	Kp.MakeZero();

	const YsVec3 o=shl.GetVertexPosition(vtHd);

	for(auto plHd : vtPlHd)
	{
		auto plVtHd=shl.GetPolygonVertex(plHd);
		YsVec3 n=YsOrigin();

		if(3==plVtHd.GetN())
		{
			const YsVec3 plVtPos[3]=
			{
				shl.GetVertexPosition(plVtHd[0]),
				shl.GetVertexPosition(plVtHd[1]),
				shl.GetVertexPosition(plVtHd[2])
			};
			const YsVec3 edVec[2]=
			{
				plVtPos[1]-plVtPos[0],
				plVtPos[2]-plVtPos[0]
			};
			n=YsUnitVector(edVec[0]^edVec[1]);
		}
		else
		{
			for(int idx=0; idx<plVtHd.GetN(); ++idx)
			{
				if(vtHd==plVtHd[idx])
				{
					const YsVec3 plVtPos[3]=
					{
						shl.GetVertexPosition(plVtHd.GetCyclic(idx-1)),
						shl.GetVertexPosition(plVtHd[idx]),
						shl.GetVertexPosition(plVtHd.GetCyclic(idx+1))
					};
					const YsVec3 edVec[2]=
					{
						plVtPos[1]-plVtPos[0],
						plVtPos[2]-plVtPos[0]
					};
					n=YsUnitVector(edVec[0]^edVec[1]);
					break;
				}
			}
		}

		// (p-o)*n=0 -> p*n-o*n=0  -> nx*x+ny*y+nz*z-o*n=0
		const double a=n.x(),b=n.y(),c=n.z(),d=-o*n;
		const double ab=a*b;
		const double ac=a*c;
		const double ad=a*d;
		const double bc=b*c;
		const double bd=b*d;
		const double cd=c*d;

		YsMatrix4x4 m;
		m.Set(1,1,a*a);
		m.Set(1,2,ab);
		m.Set(1,3,ac);
		m.Set(1,4,ad);

		m.Set(2,1,ab);
		m.Set(2,2,b*b);
		m.Set(2,3,bc);
		m.Set(2,4,bd);

		m.Set(3,1,ac);
		m.Set(3,2,bc);
		m.Set(3,3,c*c);
		m.Set(3,4,cd);

		m.Set(4,1,ad);
		m.Set(4,2,bd);
		m.Set(4,3,cd);
		m.Set(4,4,d*d);

		Kp+=m;
	}
}


double YsShellExt_CalculateQuadricError(const YsMatrix4x4 &Kp,const YsVec3 &pos)
{
	//
	//               v11    v12    v13    v14
	//               v21    v22    v23    v24
	//               v31    v32    v33    v34
	//               v41    v42    v43    v44
	//  [x y z 1]

	const double posTimesKp[4]=
	{
		pos.x()*Kp.v(1,1)+pos.y()*Kp.v(2,1)+pos.z()*Kp.v(3,1)+Kp.v(4,1),
		pos.x()*Kp.v(1,2)+pos.y()*Kp.v(2,2)+pos.z()*Kp.v(3,2)+Kp.v(4,2),
		pos.x()*Kp.v(1,3)+pos.y()*Kp.v(2,3)+pos.z()*Kp.v(3,3)+Kp.v(4,3),
		pos.x()*Kp.v(1,4)+pos.y()*Kp.v(2,4)+pos.z()*Kp.v(3,4)+Kp.v(4,4),
	};

	return pos[0]*posTimesKp[0]+pos[1]*posTimesKp[1]+pos[2]*posTimesKp[2]+posTimesKp[3];
}

double YsShellExt_CalculateTotalFaceAngle(const YsShellExt &shl,YsShellVertexHandle vtHd)
{
	auto vtPlHd=shl.FindPolygonFromVertex(vtHd);

	double totalAngle=0.0;
	for(auto plHd : vtPlHd)
	{
		totalAngle+=YsShellExt_CalculatePolygonFaceAngle(shl,plHd,vtHd);
	}

	return totalAngle;
}

double YsShellExt_CalculatePolygonFaceAngle(const YsShellExt &shl,YsShellPolygonHandle plHd,YsShellVertexHandle vtHd)
{
	return YsShellExt_CalculatePolygonFaceAngle(shl,shl.GetPolygonVertex(plHd),vtHd);
}

double YsShellExt_CalculatePolygonFaceAngle(const YsShellExt &shl,YSSIZE_T nVtIn,const YsShellVertexHandle plVtHdIn[],YsShellVertexHandle vtHd)
{
	YSSIZE_T vtIdx=-1;
	YsArray <YsVec3,4> plVtPos;
	plVtPos.Resize(nVtIn);
	for(YSSIZE_T idx=0; idx<nVtIn; ++idx)
	{
		plVtPos[idx]=shl.GetVertexPosition(plVtHdIn[idx]);
		if(plVtHdIn[idx]==vtHd)
		{
			vtIdx=idx;
		}
	}

	YsShellExt_PolygonProjection prj;
	prj.Project(plVtPos);
	auto prjVtPos=prj.GetProjectedPolygon();

	if(0<=vtIdx)
	{
		return YsGetPolygonFaceAngle(prjVtPos,vtIdx);
	}
	return 0.0;
}

YSBOOL YsShellExt_IsFaceGroupPlanar(const YsShellExt &shl,YsShellExt::FaceGroupHandle fgHd,const double cosineTolerance)
{
	auto fgPlHd=shl.GetFaceGroup(fgHd);

	if(0<fgPlHd.GetN())
	{
		const YsVec3 refNom=shl.GetNormal(fgPlHd[0]);
		for(auto plHd : fgPlHd)
		{
			const double c=refNom*shl.GetNormal(plHd);
			if(c<cosineTolerance)
			{
				return YSFALSE;
			}
		}
		return YSTRUE;
	}
	return YSFALSE;
}

void YsShellExt_CalculateAspectRatio(double &length,double &height,YSSIZE_T np,const YsVec3 p[])
{
	if(3<=np)
	{
		auto longestIdx=YsFindLongestEdgeIndexOfPolygon(np,p);

		const YsVec3 &edVtPos0=p[longestIdx];
		const YsVec3 &edVtPos1=YsGetCyclic(np,p,longestIdx+1);

		length=(edVtPos1-edVtPos0).GetLength();


		height=0;
		for(YSSIZE_T idx=0; idx<np; ++idx)
		{
			if(idx!=longestIdx && idx!=longestIdx+1) // This will miss idx==np-1 and longestIdx=0, but can save bunch of integer divisions.
			{
				const double h=YsGetPointLineDistance3(edVtPos0,edVtPos1,p[idx]);
				YsMakeGreater(height,h);
			}
		}
	}
	else
	{
		length=0.0;
		height=0.0;
	}
}

void YsShellExt_CalculateAspectRatio(double &length,double &height,const YsShell &shl,YSSIZE_T nv,const YsShellVertexHandle v[])
{
	YsArray <YsVec3,4> plg(nv,NULL);
	for(YSSIZE_T idx=0; idx<nv; ++idx)
	{
		plg[idx]=shl.GetVertexPosition(v[idx]);
	}
	YsShellExt_CalculateAspectRatio(length,height,nv,plg);
}

void YsShellExt_CalculateAspectRatio(double &length,double &height,const YsShell &shl,YsShellPolygonHandle plHd)
{
	int nPlVt;
	const YsShellVertexHandle *plVtHd;
	shl.GetPolygon(nPlVt,plVtHd,plHd);
	YsShellExt_CalculateAspectRatio(length,height,shl,nPlVt,plVtHd);
}

YsArray <YsShell::EdgeAndPos> YsShellExt_GetConstEdgeAndPlaneIntersection(const YsShellExt &shl,YsShellExt::ConstEdgeHandle ceHd,const YsPlane &pln)
{
	YSBOOL isLoop;
	YsArray <YsShellVertexHandle> ceVtHd;
	shl.GetConstEdge(ceVtHd,isLoop,ceHd);

	if(YSTRUE==isLoop)
	{
		ceVtHd.Append(ceVtHd[0]);
	}

	YsArray <YsShell::EdgeAndPos> itsc;

	for(YSSIZE_T idx=0; idx<ceVtHd.GetN()-1; ++idx)
	{
		const YsShellVertexHandle edVtHd[2]={ceVtHd[idx],ceVtHd[idx+1]};
		const YsVec3 edVtPos[2]={shl.GetVertexPosition(ceVtHd[idx]),shl.GetVertexPosition(ceVtHd[idx+1])};
		YsVec3 itscPos;

		if(YSTRUE==pln.CheckOnPlane(edVtPos[0]))
		{
			itsc.Increment();
			itsc.Last().edVtHd[0]=edVtHd[0];
			itsc.Last().edVtHd[1]=edVtHd[1];
			itsc.Last().pos=edVtPos[0];
		}
		else if(YSTRUE==pln.CheckOnPlane(edVtPos[1]))
		{
			// Will be taken in the next iteration.
		}
		else if(YSOK==pln.GetPenetration(itscPos,edVtPos[0],edVtPos[1]))
		{
			itsc.Increment();
			itsc.Last().edVtHd[0]=edVtHd[0];
			itsc.Last().edVtHd[1]=edVtHd[1];
			itsc.Last().pos=itscPos;
		}
	}

	return itsc;
}


double YsShellExt_CalculateConstEdgeLength(const YsShellExt &shl,YsShellExt::ConstEdgeHandle ceHd)
{
	YSSIZE_T nCeVt;
	const YsShellVertexHandle *ceVtHd;
	YSBOOL isLoop;
	shl.GetConstEdge(nCeVt,ceVtHd,isLoop,ceHd);
	double len=0.0;
	for(YSSIZE_T idx=0; idx<nCeVt-1; ++idx)
	{
		len+=shl.GetEdgeLength(ceVtHd[idx],ceVtHd[idx+1]);
	}
	if(YSTRUE==isLoop && 0<nCeVt)
	{
		len+=shl.GetEdgeLength(ceVtHd[0],ceVtHd[nCeVt-1]);
	}
	return len;
}

double YsShellExt_CalculateMaxNormalDeviationAcrossConstEdge(const YsShellExt &shl,YsShellExt::ConstEdgeHandle ceHd)
{
	YsShellPolygonHandle plHd[2];
	return YsShellExt_CalculateMaxNormalDeviationAcrossConstEdge(plHd,shl,ceHd);
}

double YsShellExt_CalculateMaxNormalDeviationAcrossConstEdge(YsShellPolygonHandle plHd[2],const YsShellExt &shl,YsShellExt::ConstEdgeHandle ceHd)
{
	plHd[0]=NULL;
	plHd[1]=NULL;

	auto ceFgHd=shl.FindFaceGroupFromConstEdge(ceHd);
	YsArray <YsShellPolygonHandle> allPlHd;
	for(auto fgHd : ceFgHd)
	{
		YsArray <YsShellPolygonHandle> fgPlHd;
		shl.GetFaceGroup(fgPlHd,fgHd);
		allPlHd.Append(fgPlHd);
	}

	double minCos=1.0;
	for(auto iter0=allPlHd.begin(); iter0!=allPlHd.end(); ++iter0)
	{
		const YsVec3 nom0=shl.GetNormal(*iter0);
		auto iter1=iter0;
		++iter1;
		for(iter1=iter1; iter1!=allPlHd.end(); ++iter1)
		{
			const YsVec3 nom1=shl.GetNormal(*iter1);
			auto dot=nom0*nom1;
			if(dot<minCos)
			{
				minCos=dot;
				plHd[0]=*iter0;
				plHd[1]=*iter1;
			}
		}
	}

	return acos(YsBound(minCos,-1.0,1.0));
}

void YsShellExt_CalculateNormalConeAcrossConstEdge(YsVec3 &axis,double &aperture,const YsShellExt &shl,YsShellExt::ConstEdgeHandle ceHd)
{
	auto ceFgHd=shl.FindFaceGroupFromConstEdge(ceHd);

	axis=YsOrigin();
	for(auto fgHd : ceFgHd)
	{
		YsArray <YsShellPolygonHandle> fgPlHd;
		shl.GetFaceGroup(fgPlHd,fgHd);
		for(auto plHd : fgPlHd)
		{
			axis+=shl.GetNormal(plHd);
		}
	}

	axis.Normalize();
	double minCos=1.0;
	for(auto fgHd : ceFgHd)
	{
		YsArray <YsShellPolygonHandle> fgPlHd;
		shl.GetFaceGroup(fgPlHd,fgHd);
		for(auto plHd : fgPlHd)
		{
			YsMakeSmaller(minCos,axis*shl.GetNormal(plHd));
		}
	}
	aperture=2.0*acos(YsGreater(-1.0,minCos));
}

YsVec3 YsShellExt_CalculateInboundConstEdgeVectorAtTip(const YsShellExt &shl,YsShellExt::ConstEdgeHandle ceHd,int tip)
{
	YSBOOL isLoop;
	YSSIZE_T nCeVt;
	const YsShellVertexHandle *ceVtHd;
	shl.GetConstEdge(nCeVt,ceVtHd,isLoop,ceHd);

	if(2<=nCeVt)
	{
		if(0==tip)
		{
			return shl.GetEdgeVector(ceVtHd[0],ceVtHd[1]);
		}
		else
		{
			if(YSTRUE!=isLoop)
			{
				return shl.GetEdgeVector(ceVtHd[nCeVt-1],ceVtHd[nCeVt-2]);
			}
			else
			{
				return shl.GetEdgeVector(ceVtHd[0],ceVtHd[nCeVt-1]);
			}
		}
	}

	return YsOrigin();
}

YsVec3 YsShellExt_CalculateInboundConstEdgeVectorAtTip(const YsShellExt &shl,YsShellExt::ConstEdgeHandle ceHd,YsShellVertexHandle tipVtHd)
{
	YSBOOL isLoop;
	YSSIZE_T nCeVt;
	const YsShellVertexHandle *ceVtHd;
	shl.GetConstEdge(nCeVt,ceVtHd,isLoop,ceHd);

	if(2<=nCeVt)
	{
		if(tipVtHd==ceVtHd[0])
		{
			return YsShellExt_CalculateInboundConstEdgeVectorAtTip(shl,ceHd,0);
		}
		else if(tipVtHd==ceVtHd[nCeVt-1])
		{
			return YsShellExt_CalculateInboundConstEdgeVectorAtTip(shl,ceHd,1);
		}
	}

	return YsOrigin();
}



YsShellExt::VertexFaceGroupAttribTable <YsVec3> YsShellExt_MakeVertexFaceGroupNormalTable(const YsShellExt &shl)
{
	YsShellExt::VertexFaceGroupAttribTable <YsVec3> vtFgNomTable(shl);
	for(auto vtHd : shl.AllVertex())
	{
		// Maybe the following block is useful for the other purposes? >>
		auto vtPlHd=shl.FindPolygonFromVertex(vtHd);

		YsArray <YsShellExt::PolygonAndFaceGroup> plFgArray;
		YsArray <YSHASHKEY> fgKeyArray;

		for(auto plHd : vtPlHd)
		{
			auto fgHd=shl.FindFaceGroupFromPolygon(plHd);
			if(NULL!=fgHd)
			{
				plFgArray.Increment();
				plFgArray.Last().plHd=plHd;
				plFgArray.Last().fgHd=fgHd;
				fgKeyArray.Add(shl.GetSearchKey(fgHd));
			}
		}

		YsSimpleMergeSort(fgKeyArray.GetN(),fgKeyArray.GetEditableArray(),plFgArray.GetEditableArray());
		// Maybe the above block is useful for the other purposes? <<

		YsVec3 nomSum=YsOrigin();
		for(YSSIZE_T idx=0; idx<plFgArray.GetN(); ++idx)
		{
			YsVec3 nom;
			YsArray <YsVec3,4> plVtPos;
			shl.GetPolygon(plVtPos,plFgArray[idx].plHd);
			YsGetAverageNormalVector(nom,plVtPos);
			nomSum+=nom;
			if(idx==plFgArray.GetN()-1 || plFgArray[idx].fgHd!=plFgArray[idx+1].fgHd)
			{
				nomSum.Normalize();
				vtFgNomTable.Update(vtHd,plFgArray[idx].fgHd,nomSum);
			}
		}
	}

	return vtFgNomTable;
}

YsShellPolygonAttribTable <YsVec3> YsShellExt_MakePolygonNormalTable(const YsShell &shl)
{
	YsShellPolygonAttribTable <YsVec3> table(shl);
	for(auto plHd : shl.AllPolygon())
	{
		YsArray <YsVec3,4> plVtPos;
		shl.GetPolygon(plVtPos,plHd);

		YsVec3 nom;
		YsGetAverageNormalVector(nom,plVtPos);

		table.SetAttrib(plHd,nom);
	}

	return table;
}


const double YsShellExt_CalculateMaxDihedralAngleInFaceGroup(const class YsShellExt &shl,YsShellExt::FaceGroupHandle fgHd)
{
	double maxDha=0.0;

	auto fgPlHd=shl.GetFaceGroup(fgHd);
	for(auto plHd0 : fgPlHd)
	{
		int nEdge=shl.GetPolygonNumVertex(plHd0);
		for(int i=0; i<nEdge; ++i)
		{
			auto plHd1=shl.GetNeighborPolygon(plHd0,i);
			if(nullptr!=plHd1 && shl.FindFaceGroupFromPolygon(plHd1)==fgHd)
			{
				YsMakeGreater(maxDha,YsShellExt_CalculateDihedralAngle(shl.Conv(),plHd0,plHd1));
			}
		}
	}

	return maxDha;
}

const double YsShellExt_CalculateMaxDihedralAngleInFaceGroup(const class YsShellExt &shl,const YsShellVertexPositionSource &vtPosSrc,YsShellExt::FaceGroupHandle fgHd)
{
	double maxDha=0.0;

	auto fgPlHd=shl.GetFaceGroup(fgHd);
	for(auto plHd0 : fgPlHd)
	{
		int nEdge=shl.GetPolygonNumVertex(plHd0);
		for(int i=0; i<nEdge; ++i)
		{
			auto plHd1=shl.GetNeighborPolygon(plHd0,i);
			if(nullptr!=plHd1 && shl.FindFaceGroupFromPolygon(plHd1)==fgHd)
			{
				YsMakeGreater(maxDha,YsShellExt_CalculateDihedralAngle(shl.Conv(),vtPosSrc,plHd0,plHd1));
			}
		}
	}

	return maxDha;
}


YSRESULT YsShell_CalculateRawRadiusRatio(double &rr,const YsVec3 triVtPos[3])
{
	double inRad,circumRad;
	YsVec3 inCen,circumCen;
	if(YSOK==YsGetCircumSphereOfTriangle(circumCen,circumRad,triVtPos[0],triVtPos[1],triVtPos[2]) &&
	   YSOK==YsGetInscribedSphereOfTriangle(inCen,inRad,triVtPos[0],triVtPos[1],triVtPos[2]) &&
	   YsTolerance<inRad)
	{
		rr=circumRad/inRad;
		return YSOK;
	}
	rr=0.0;
	return YSERR;

}

YSRESULT YsShell_CalculateRawRadiusRatio(double &rr,const YsShell &shl,YsShell::PolygonHandle plHd)
{
	YsArray <YsVec3,4> plVtPos;
	shl.GetPolygon(plVtPos,plHd);
	if(3==plVtPos.GetN())
	{
		return YsShell_CalculateRawRadiusRatio(rr,plVtPos);
	}
	rr=0.0;
	return YSERR;
}

YSRESULT YsShell_CalculateRawRadiusRatio(double &rr,const YsShell &shl,const YsShell::VertexHandle triVtHd[3])
{
	const YsVec3 triVtPos[3]=
	{
		shl.GetVertexPosition(triVtHd[0]),
		shl.GetVertexPosition(triVtHd[1]),
		shl.GetVertexPosition(triVtHd[2]),
	};
	return YsShell_CalculateRawRadiusRatio(rr,triVtPos);
}

