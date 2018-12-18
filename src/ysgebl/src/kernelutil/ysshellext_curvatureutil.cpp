#include "ysshellext_curvatureutil.h"
#include "ysshellext_geomcalc.h"



YsShellExt_CurvatureEstimator::Result YsShellExt_CurvatureEstimator::EstimateAtVertex(
    const YsShellExt &shl,YsShell::VertexHandle vtHd,Option opt) const
{
	switch(opt.scheme)
	{
	case SCHEME_ANGLE_OVER_DISTANCE:
		return EstimateAtVertex_AngleOverDistance(shl,vtHd,opt);
	}
	Result res;
	res.res=YSERR;
	res.curvature=0.0;
	res.radius=0.0;
	return res;
}

YsShellExt_CurvatureEstimator::Result YsShellExt_CurvatureEstimator::EstimateAtVertex_AngleOverDistance(const YsShellExt &shl,YsShell::VertexHandle vtHd,Option opt) const
{
	Result res;
	res.res=YSERR;
	res.curvature=0.0;
	res.radius=0.0;

	auto vtPlHd=shl.FindPolygonFromVertex(vtHd);
	for(YSSIZE_T i0=0; i0<vtPlHd.size(); ++i0)
	{
		auto plNom0=GetNormal(shl,vtPlHd[i0],opt);
		auto plCen0=shl.GetCenter(vtPlHd[i0]);
		for(YSSIZE_T i1=i0+1; i1<vtPlHd.size(); ++i1)
		{
			auto plNom1=GetNormal(shl,vtPlHd[i1],opt);
			auto plCen1=shl.GetCenter(vtPlHd[i1]);
			auto c=plNom0*plNom1;
			c=YsBound(c,-1.0,1.0);
			auto angle=acos(c);

			auto dist=(shl.GetVertexPosition(vtHd)-plCen0).GetLength()+(shl.GetVertexPosition(vtHd)-plCen1).GetLength();

			if(YsTolerance<dist)
			{
				auto curvature=angle/dist;
				if(YSERR==res.res || res.curvature<curvature)
				{
					res.res=YSOK;
					res.curvature=curvature;
					if(YsTolerance<curvature)
					{
						res.radius=1.0/curvature;
					}
					else
					{
						res.radius=YsInfinity;
					}
				}
			}
		}
	}

	return res;
}

YsVec3 YsShellExt_CurvatureEstimator::GetNormal(const YsShellExt &shl,YsShell::PolygonHandle plHd,Option opt) const
{
	if(YSTRUE==opt.useCachedNormal)
	{
		return shl.GetNormal(plHd);
	}
	return YsShell_CalculateNormal(shl.Conv(),plHd);
}

