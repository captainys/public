#ifndef YSSHELLEXT_PIPEAXISUTIL_IS_INCLUDED
#define YSSHELLEXT_PIPEAXISUTIL_IS_INCLUDED
/* { */



#include "ysshellext.h"



class YsShellExt_PipeAxisUtil
{
public:
	class Photon
	{
	public:
		YsVec3 pos,vec;
	};

	YsShellLattice ltc;
	YsArray <Photon> path;
	YsArray <YsShell::PolygonHandle> startPlg,endPlg;
	YSBOOL reachedEnd=YSFALSE;

	int nSearchDir=16;  // When the photon hits the wall, how many directions to search for the next direction?
	int nBisection=4;   // After finding rough directions, how many times bi-section search to refind the next direction?

	void CleanUp(void);
	void Begin(const YsShellExt &shl,YsConstArrayMask <YsShell::PolygonHandle> startPlg,YsConstArrayMask <YsShell::PolygonHandle> endPlg);
private:
	void CalculateInitialPosition(const YsShellExt &shl,YsConstArrayMask <YsShell::PolygonHandle> startPlg);
	YsVec3 GetContourCenter(const YsShellExt &shl,YsConstArrayMask <YsShell::PolygonHandle> plg) const;
	double EstimateRadius(const YsShellExt &shl,const YsVec3 &pos,const YsVec3 &vec) const;
	YsArray <YsVec3> ShootRadial(const YsShellExt &shl,const YsVec3 &pos,const YsVec3 &vec) const;
	YsArray <YsVec3> ShootRadialGetNormal(const YsShellExt &shl,const YsVec3 &pos,const YsVec3 &vec) const;

	bool IsBackflow(const YsShellExt &shl,const YsVec3 &currentPos,const YsVec3 &nextPos,const YsVec3 &nextItsc) const;

public:
	YSRESULT ShootOneStep(const YsShellExt &shl);
};



/* } */
#endif
