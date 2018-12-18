/* ////////////////////////////////////////////////////////////

File Name: ysshellext_projectionutil.h
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

#ifndef YSSHELLEXT_PROJECTIONUTIL_IS_INCLUDED
#define YSSHELLEXT_PROJECTIONUTIL_IS_INCLUDED
/* { */

#include <ysshellext.h>

class YsShellExt_PolygonProjection
{
public:
	enum NORMAL_CALCULATION_STRATEGY
	{
		TRY_OWN_NORMAL_THEN_RECALCULATE_THEN_MAX_DIMENSION,
		OWN_NORMAL,
		RECALCULATE,
		MAX_DIMENSION
	};

	NORMAL_CALCULATION_STRATEGY normalCalculationStrategy;
protected:
	YsMatrix3x3 prjMat;
	YsArray <YsVec2,4> prjVtPos;

public:
	YsShellExt_PolygonProjection();
	YsShellExt_PolygonProjection(const YsShell &shl,YsShellPolygonHandle plHd,NORMAL_CALCULATION_STRATEGY strategy=TRY_OWN_NORMAL_THEN_RECALCULATE_THEN_MAX_DIMENSION);

	void Initialize(void);

	/*! Project the polygon based on the member variable normalCalculationStrategy. */
	YSRESULT Project(const YsShell &shl,YsShellPolygonHandle plHd);

	/*! Project the polygon based on the member variable normalCalculationStrategy. 
	    ProjectByGivenNormal is used in place for ProjectByOwnNormal.
	*/
	YSRESULT Project(YSSIZE_T np,const YsVec3 p[],const YsVec3 &nom=YsOrigin());
	template <const int N>
	YSRESULT Project(const YsArray <YsVec3,N> &p,const YsVec3 &nom=YsOrigin())
	{
		return Project(p.GetN(),p,nom);
	}


	/*! Project the polybon by its own assigned normal vector. */
	YSRESULT ProjectByOwnNormal(const YsShell &shl,YsShellPolygonHandle plHd);

	/*! Project the polybon by re-calculationg normal vector from vertex coordinates. */
	YSRESULT ProjectByNormalRecalculation(const YsShell &shl,YsShellPolygonHandle plHd);
	YSRESULT ProjectByNormalRecalculation(YSSIZE_T np,const YsVec3 p[]);
	template <const int N>
	YSRESULT ProjectByNormalRecalculation(const YsArray <YsVec3,N> &p)
	{
		return ProjectByNormalRecalculation(p.GetN(),p);
	}

	/*! Project the polybon by taking normal as the minimum dinemsion of X-, Y-, Z- direction. */
	YSRESULT ProjectByMaxDimension(const YsShell &shl,YsShellPolygonHandle plHd);
	YSRESULT ProjectByMaxDimension(YSSIZE_T np,const YsVec3 p[]);
	template <const int N>
	YSRESULT ProjectByMaxDimension(const YsArray <YsVec3,N> &p)
	{
		return ProjectByMaxDimension(p.GetN(),(const YsVec3 *)p);
	}

	/*! Project the polygon by the given normal. */
	YSRESULT ProjectByGivenNormal(YSSIZE_T np,const YsVec3 p[],const YsVec3 &nom);

private:
	void ProjectUsingPrjMat(const YsShell &shl,YsShellPolygonHandle plHd);
	void ProjectUsingPrjMat(YSSIZE_T nVt,const YsVec3 vtPos[]);
	template <const int N>
	void ProjectUsingPrjMat(const YsArray <YsVec3,N> &plVtPos)
	{
		ProjectUsingPrjMat(plVtPos.GetN(),plVtPos);
	}

public:
	const YsArray <YsVec2,4> &GetProjectedPolygon(void) const;
	const YsMatrix3x3 &GetProjectionMatrix(void) const;
};

/* } */
#endif
