#include "ysshellext_constedgesmoother.h"



void YsShellExt_ConstEdgeSmoother_BandMethod::Make(const YsShellExt &shl,YsShellExt::ConstEdgeHandle ceHd)
{
	this->ceHd=ceHd;

	auto ceVtHd=shl.GetConstEdgeVertex(ceHd);
	constBand.SetShell(shl.Conv());
	constBand.CleanUp();

	for(auto vtHd : ceVtHd)
	{
		for(auto plHd : shl.FindPolygonFromVertex(vtHd))
		{
			constBand.Add(plHd);
		}
	}

	slider.resize(ceVtHd.size());
	for(auto idx : ceVtHd.AllIndex())
	{
		slider[idx].vtHd=ceVtHd[idx];
		slider[idx].pos=shl.GetVertexPosition(ceVtHd[idx]);
	}
}

const YsShellPolygonStore &YsShellExt_ConstEdgeSmoother_BandMethod::GetConstBand(void) const
{
	return constBand;
}

void YsShellExt_ConstEdgeSmoother_BandMethod::SmoothOneStep(const YsShellExt &shl)
{
	for(auto idx : slider.AllIndex())
	{
		if(0==idx || idx==slider.size()-1)
		{
			continue;
		}

		auto &s=slider[idx];
		s.tmpPos=(slider[idx-1].pos+slider[idx+1].pos+shl.GetVertexPosition(slider[idx].vtHd))/3.0;

		YsVec3 nearPos=s.tmpPos;
		double nearDist=0.0;
		bool first=true;

		for(auto connVtHd : shl.GetConnectedVertex(slider[idx].vtHd))
		{
			const YsVec3 edVtPos[2]=
			{
				shl.GetVertexPosition(slider[idx].vtHd),
				shl.GetVertexPosition(connVtHd)
			};
			auto np=YsGetNearestPointOnLinePiece(edVtPos,s.tmpPos);
			auto d=(np-s.tmpPos).GetSquareLength();

			if(true==first || d<nearDist)
			{
				first=false;
				nearDist=d;
				nearPos=np;
			}
		}

		s.tmpPos=nearPos;
	}
	for(auto &s : slider)
	{
		s.pos=s.tmpPos;
	}
}

YsArray <YsVec3> YsShellExt_ConstEdgeSmoother_BandMethod::GetCurrentCurve(void) const
{
	YsArray <YsVec3> cv;
	cv.resize(slider.size());
	for(auto idx : slider.AllIndex())
	{
		cv[idx]=slider[idx].pos;
	}
	return cv;
}
