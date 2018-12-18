/* ////////////////////////////////////////////////////////////

File Name: ysshellext_facegroupgrowing.h
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

#ifndef YSSHELLEXT_FACEGROUPGROWING_IS_INCLUDED
#define YSSHELLEXT_FACEGROUPGROWING_IS_INCLUDED
/* { */


#include <ysshellext.h>
#include <ysshellextmisc.h>

// Based on:
//   David Cohon-Steiner, Pierre Alliez, and Mathieu Desbrun, "Variational Shape Apprxoimation", Proceedings of SIGGRAPH '04, pp. 905-914

// Need Proximity queue of f(fgHd,plHd) -> Distorsion metric.
// When fgHd is updated, All f(fgHd,*) need to be updated.
// Need Ni_raw(fgHd) -> Area weighted normal of polygons in fgHd.


// Memo 2015/09/04 YsShellExt_NormalBasedSegmentationBase should be inherited from YsShellExt_RegionGrowing.


class YsShellExt_NormalBasedSegmentationBase
{
public:
	enum ERROR_METRIC_TYPE
	{
		AREA_WEIGHTED_DIFFERENCE_FROM_AVERAGE_NORMAL,  // Cohen-Steiner et al. 2004
		DIFFERENCE_FROM_SEED_NORMAL,                   // Yamakawa 2015   -> Average-normal will not be updated after every growth.  Ni_raw will still be unnormalized (raw) vector sum.
	};

protected:
	YsShellExt::FaceGroupAttribTable <YsVec3> Ni_raw;
	YsShellPolygonAttribTable <YsShellExt::FaceGroupHandle> tentativePlgToFgTable;
	YsShellExt::FaceGroupAttribTable <YsArray <YsShell::PolygonHandle> > tentativeFgToPlgTable;

	void Begin(const YsShellExt &shl);
	YsVec3 Ni(const YsShellExt &shl,YsShellExt::FaceGroupHandle fgHd) const;
	void UpdateAfterAddingPolygonToFaceGroup(const YsShellExt &shl,YsShellExt::FaceGroupHandle fgHd,YsShell::PolygonHandle plHd);
	void UpdateAfterRemovingPolygonFromFaceGroup(const YsShellExt &shl,YsShellExt::FaceGroupHandle fgHd,YsShell::PolygonHandle plHd);

	const double CalculateMetric(const YsShellExt &shl,YsShellExt::FaceGroupHandle fgHd,YsShell::PolygonHandle plHd,ERROR_METRIC_TYPE metricType) const;
	virtual YsVec3 CalculateNormal(const YsShellExt &shl,YsShell::PolygonHandle plHd) const;

	YsArray <YsShell::PolygonHandle> GetFaceGroup(const YsShellExt &shl,YsShellExt::FaceGroupHandle fgHd) const;
	void AddFaceGroupPolygon(const YsShellExt &shl,YsShellExt::FaceGroupHandle fgHd,YsShell::PolygonHandle plHd);
	YsShellExt::FaceGroupHandle FindFaceGroupFromPolygon(const YsShellExt &shl,YsShell::PolygonHandle plHd) const;

};

class YsShellExt_FaceGroupGrowing : private YsShellExt_NormalBasedSegmentationBase
{
public:
	using YsShellExt_NormalBasedSegmentationBase::ERROR_METRIC_TYPE;
	using YsShellExt_NormalBasedSegmentationBase::AREA_WEIGHTED_DIFFERENCE_FROM_AVERAGE_NORMAL;
	using YsShellExt_NormalBasedSegmentationBase::DIFFERENCE_FROM_SEED_NORMAL;

	class Preference
	{
	public:
		ERROR_METRIC_TYPE errorMetricType;
		Preference()
		{
			errorMetricType=YsShellExt_NormalBasedSegmentationBase::AREA_WEIGHTED_DIFFERENCE_FROM_AVERAGE_NORMAL;
		}
	};


private:
	YsAVLTree <double,YsShellExt::PolygonAndFaceGroup> metricTree;
	YsFixedLengthHashTable <YsAVLTree <double,YsShellExt::PolygonAndFaceGroup>::NodeHandle,2> fgPlKeyToTreeNode;

public:
	void Begin(const YsShellExt &shl,const Preference &pref);
	void UpdateFaceGroupMetric(const YsShellExt &shl,YsShellExt::FaceGroupHandle fgHd,const Preference &pref);

	YsAVLTree <double,YsShellExt::PolygonAndFaceGroup>::NodeHandle AddTreeNode(const YsShellExt &shl,YsShellExt::FaceGroupHandle fgHd,YsShell::PolygonHandle plHd,const double metric);
	YsAVLTree <double,YsShellExt::PolygonAndFaceGroup>::NodeHandle FindTreeNodeHandle(const YsShellExt &shl,YsShellExt::FaceGroupHandle fgHd,YsShell::PolygonHandle plHd) const;

	YSSIZE_T GetN(void) const;

public:
	template <class SHLCLASS>
	void GrowOneStep(SHLCLASS &shl,const Preference &pref)
	{
		auto firstHd=metricTree.First();
		if(firstHd!=nullptr)
		{
			auto firstPlFg=metricTree.GetValue(firstHd);
			if(nullptr==FindFaceGroupFromPolygon(shl.Conv(),firstPlFg.plHd))
			{
				AddFaceGroupPolygon(shl.Conv(),firstPlFg.fgHd,firstPlFg.plHd);
				if(AREA_WEIGHTED_DIFFERENCE_FROM_AVERAGE_NORMAL==pref.errorMetricType)
				{
					YsShellExt_NormalBasedSegmentationBase::UpdateAfterAddingPolygonToFaceGroup(shl.Conv(),firstPlFg.fgHd,firstPlFg.plHd);
				}
				UpdateFaceGroupMetric(shl.Conv(),firstPlFg.fgHd,pref);
			}

			const YSHASHKEY fgPlKey[2]={shl.GetSearchKey(firstPlFg.fgHd),shl.GetSearchKey(firstPlFg.plHd)};
			fgPlKeyToTreeNode.Delete(2,fgPlKey);
			metricTree.Delete(firstHd);
		}
	}

	template <class SHLCLASS>
	void UpdateFaceGroup(SHLCLASS &shl)
	{
printf("%s %d\n",__FUNCTION__,__LINE__);
		for(auto fgHd : shl.AllFaceGroup())
		{
			auto plHdPtr=tentativeFgToPlgTable[fgHd];
			if(nullptr!=plHdPtr)
			{
				shl.AddFaceGroupPolygon(fgHd,*plHdPtr);
			}
		}

		/* for(auto hd : tentativeFgToPlgTable.AllHandle())
		{
			auto fgKey=tentativeFgToPlgTable.GetKey(hd);
			auto fgHd=shl.FindFaceGroup(fgKey);

			auto plHdPtr=tentativeFgToPlgTable[fgKey];
			if(nullptr!=plHdPtr)
			{
				shl.AddFaceGroupPolygon(fgHd,*plHdPtr);
			}
		} */
	}
};


/* } */
#endif
