#ifndef YSSHELLEXT_CONSTEDGESMOOTHER_IS_INCLUDED
#define YSSHELLEXT_CONSTEDGESMOOTHER_IS_INCLUDED
/* { */


#include "ysshellext.h"


class YsShellExt_ConstEdgeSmoother
{
public:
	YsShellExt::ConstEdgeHandle ceHd;
};



class YsShellExt_ConstEdgeSmoother_BandMethod : public YsShellExt_ConstEdgeSmoother
{
private:
	class Slider
	{
	public:
		YsShellExt::VertexHandle vtHd;
		YsVec3 pos;
		YsVec3 tmpPos;
	};
	YsArray <Slider> slider;
	YsShellPolygonStore constBand;

public:
	void Make(const YsShellExt &shl,YsShellExt::ConstEdgeHandle ceHd);
	const YsShellPolygonStore &GetConstBand(void) const;

	void SmoothOneStep(const YsShellExt &shl);

	YsArray <YsVec3> GetCurrentCurve(void) const;
};



/* } */
#endif
