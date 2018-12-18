#ifndef YSSHELLEXT_CURVATUREUTIL_IS_INCLUDED
#define YSSHELLEXT_CURVATUREUTIL_IS_INCLUDED
/* { */



#include "ysshellext.h"

class YsShellExt_CurvatureEstimator
{
public:
	enum SCHEME
	{
		SCHEME_NONE,
		SCHEME_ANGLE_OVER_DISTANCE
	};

	class Option
	{
	public:
		SCHEME scheme;
		YSBOOL useCachedNormal;

		inline Option()
		{
			scheme=SCHEME_ANGLE_OVER_DISTANCE;
			useCachedNormal=YSTRUE;
		}
	};

	/*! Result stores success/fail (YSOK/YSERR), curvature, and curvature-radius.
	    When curvature is zero, radius will be set to YsInfinity.
	    When radius is zero, curvature will be set to YsInfinity.
	*/
	class Result
	{
	public:
		YSRESULT res;
		double curvature;
		double radius;
	};

	/*! Estimate curvature at a vertex. */
	Result EstimateAtVertex(const YsShellExt &shl,YsShell::VertexHandle vtHd,Option opt) const;
	Result EstimateAtVertex_AngleOverDistance(const YsShellExt &shl,YsShell::VertexHandle vtHd,Option opt) const;

private:
	YsVec3 GetNormal(const YsShellExt &shl,YsShell::PolygonHandle plHd,Option opt) const;
};



/* } */
#endif
