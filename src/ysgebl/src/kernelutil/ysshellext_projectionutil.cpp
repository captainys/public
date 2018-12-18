/* ////////////////////////////////////////////////////////////

File Name: ysshellext_projectionutil.cpp
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

#include "ysshellext_projectionutil.h"



YsShellExt_PolygonProjection::YsShellExt_PolygonProjection()
{
	Initialize();
}
YsShellExt_PolygonProjection::YsShellExt_PolygonProjection(const YsShell &shl,YsShellPolygonHandle plHd,NORMAL_CALCULATION_STRATEGY strategy)
{
	normalCalculationStrategy=strategy;
	Project(shl,plHd);
}

void YsShellExt_PolygonProjection::Initialize(void)
{
	normalCalculationStrategy=TRY_OWN_NORMAL_THEN_RECALCULATE_THEN_MAX_DIMENSION;
}
YSRESULT YsShellExt_PolygonProjection::Project(const YsShell &shl,YsShellPolygonHandle plHd)
{
	switch(normalCalculationStrategy)
	{
	case TRY_OWN_NORMAL_THEN_RECALCULATE_THEN_MAX_DIMENSION:
		if(YSOK==ProjectByOwnNormal(shl,plHd) ||
		   YSOK==ProjectByNormalRecalculation(shl,plHd) ||
		   YSOK==ProjectByMaxDimension(shl,plHd))
		{
			return YSOK;
		}
		break;
	case OWN_NORMAL:
		return ProjectByOwnNormal(shl,plHd);
	case RECALCULATE:
		return ProjectByNormalRecalculation(shl,plHd);
	case MAX_DIMENSION:
		return ProjectByMaxDimension(shl,plHd);
	default:
		break;
	}
	return YSERR;
}
YSRESULT YsShellExt_PolygonProjection::Project(YSSIZE_T np,const YsVec3 p[],const YsVec3 &nom)
{
	switch(normalCalculationStrategy)
	{
	case TRY_OWN_NORMAL_THEN_RECALCULATE_THEN_MAX_DIMENSION:
		if(YSOK==ProjectByGivenNormal(np,p,nom) ||
		   YSOK==ProjectByNormalRecalculation(np,p) ||
		   YSOK==ProjectByMaxDimension(np,p))
		{
			return YSOK;
		}
		break;
	case OWN_NORMAL:
		return ProjectByGivenNormal(np,p,nom);
	case RECALCULATE:
		return ProjectByNormalRecalculation(np,p);
	case MAX_DIMENSION:
		return ProjectByMaxDimension(np,p);
	default:
		break;
	}
	return YSERR;
}

YSRESULT YsShellExt_PolygonProjection::ProjectByOwnNormal(const YsShell &shl,YsShellPolygonHandle plHd)
{
	auto nom=shl.GetNormal(plHd);
	if(YsOrigin()!=nom)
	{
		prjMat.MakeToXY(nom);
		ProjectUsingPrjMat(shl,plHd);
		return YSOK;
	}
	return YSERR;
}
YSRESULT YsShellExt_PolygonProjection::ProjectByNormalRecalculation(const YsShell &shl,YsShellPolygonHandle plHd)
{
	YsArray <YsVec3,4> plVtPos;
	shl.GetPolygon(plVtPos,plHd);
	return ProjectByNormalRecalculation(plVtPos);
}

YSRESULT YsShellExt_PolygonProjection::ProjectByNormalRecalculation(YSSIZE_T np,const YsVec3 p[])
{
	YsVec3 nom;
	if(YSOK==YsGetAverageNormalVector(nom,np,p))
	{
		prjMat.MakeToXY(nom);
		ProjectUsingPrjMat(np,p);
		return YSOK;
	}
	return YSERR;
}

YSRESULT YsShellExt_PolygonProjection::ProjectByMaxDimension(const YsShell &shl,YsShellPolygonHandle plHd)
{
	YsArray <YsVec3,4> plVtPos;
	shl.GetPolygon(plVtPos,plHd);
	return ProjectByMaxDimension(plVtPos);
}

YSRESULT YsShellExt_PolygonProjection::ProjectByMaxDimension(YSSIZE_T np,const YsVec3 p[])
{
	YsBoundingBoxMaker3 mkBbx;
	mkBbx.Make(np,p);
	auto dim=mkBbx.GetDimension();

	YsVec3 nom;
	if(dim.x()<dim.y() && dim.x()<dim.z())
	{
		nom=YsXVec();
	}
	else if(dim.y()<dim.z())
	{
		nom=YsYVec();
	}
	else
	{
		nom=YsZVec();
	}

	prjMat.MakeToXY(nom);
	ProjectUsingPrjMat(np,p);
	return YSOK;
}

YSRESULT YsShellExt_PolygonProjection::ProjectByGivenNormal(YSSIZE_T np,const YsVec3 p[],const YsVec3 &nom)
{
	if(YsOrigin()!=nom)
	{
		prjMat.MakeToXY(nom);
		ProjectUsingPrjMat(np,p);
		return YSOK;
	}
	return YSERR;
}


void YsShellExt_PolygonProjection::ProjectUsingPrjMat(const YsShell &shl,YsShellPolygonHandle plHd)
{
	YsArray <YsVec3,4> plVtPos;
	shl.GetPolygon(plVtPos,plHd);
	prjVtPos.Set(plVtPos.GetN(),NULL);
	for(YSSIZE_T idx=0; idx<plVtPos.GetN(); ++idx)
	{
		prjVtPos[idx]=(prjMat*plVtPos[idx]).xy();
	}
}

void YsShellExt_PolygonProjection::ProjectUsingPrjMat(YSSIZE_T nVt,const YsVec3 vtPos[])
{
	prjVtPos.Set(nVt,NULL);
	for(YSSIZE_T idx=0; idx<nVt; ++idx)
	{
		prjVtPos[idx]=(prjMat*vtPos[idx]).xy();
	}
}

const YsArray <YsVec2,4> &YsShellExt_PolygonProjection::GetProjectedPolygon(void) const
{
	return prjVtPos;
}
const YsMatrix3x3 &YsShellExt_PolygonProjection::GetProjectionMatrix(void) const
{
	return prjMat;
}

