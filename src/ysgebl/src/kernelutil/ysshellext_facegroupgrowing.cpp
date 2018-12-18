/* ////////////////////////////////////////////////////////////

File Name: ysshellext_facegroupgrowing.cpp
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

#include "ysshellext_geomcalc.h"
#include "ysshellext_facegroupgrowing.h"


void YsShellExt_NormalBasedSegmentationBase::Begin(const YsShellExt &shl)
{
	Ni_raw.SetShell(shl);

	tentativePlgToFgTable.CleanUp();
	tentativePlgToFgTable.SetShell(shl.Conv());

	tentativeFgToPlgTable.CleanUp();
	tentativeFgToPlgTable.SetShell(shl.Conv());


	for(auto fgHd : shl.AllFaceGroup())
	{
		auto fgPlHd=GetFaceGroup(shl,fgHd);
		YsVec3 nomSum=YsOrigin();
		for(auto plHd : fgPlHd)
		{
			nomSum+=shl.GetPolygonArea(plHd)*CalculateNormal(shl.Conv(),plHd);
		}
		Ni_raw.SetAttrib(fgHd,nomSum);
	}
}
YsVec3 YsShellExt_NormalBasedSegmentationBase::Ni(const YsShellExt &shl,YsShellExt::FaceGroupHandle fgHd) const
{
	auto Ni_rawPtr=Ni_raw[fgHd];
	if(nullptr!=Ni_rawPtr)
	{
		return YsUnitVector(*Ni_rawPtr);
	}
	return YsOrigin();
}
void YsShellExt_NormalBasedSegmentationBase::UpdateAfterAddingPolygonToFaceGroup(const YsShellExt &shl,YsShellExt::FaceGroupHandle fgHd,YsShell::PolygonHandle plHd)
{
	auto diff=YsShell_CalculateAreaWeightedPolygonNormal(shl.Conv(),plHd);
	auto Ni_rawPtr=Ni_raw[fgHd];
	if(nullptr!=Ni_rawPtr)
	{
		(*Ni_rawPtr)+=diff;
	}
	else
	{
		Ni_raw.SetAttrib(fgHd,diff);
	}
}
void YsShellExt_NormalBasedSegmentationBase::UpdateAfterRemovingPolygonFromFaceGroup(const YsShellExt &shl,YsShellExt::FaceGroupHandle fgHd,YsShell::PolygonHandle plHd)
{
	auto diff=YsShell_CalculateAreaWeightedPolygonNormal(shl.Conv(),plHd);
	auto Ni_rawPtr=Ni_raw[fgHd];
	if(nullptr!=Ni_rawPtr)
	{
		(*Ni_rawPtr)-=diff;
	}
}

const double YsShellExt_NormalBasedSegmentationBase::CalculateMetric(const YsShellExt &shl,YsShellExt::FaceGroupHandle fgHd,YsShell::PolygonHandle plHd,ERROR_METRIC_TYPE metricType) const
{
	const double Ti=(DIFFERENCE_FROM_SEED_NORMAL==metricType ? 1.0 : shl.GetPolygonArea(plHd));
	const auto *Ni_rawPtr=Ni_raw[fgHd];
	if(nullptr!=Ni_rawPtr)
	{
		auto Ni=YsUnitVector(*Ni_rawPtr);
		auto ni=shl.GetNormal(plHd);
		auto diff=(Ni-ni);
		return diff.GetSquareLength()*Ti;
	}
	else
	{
		return -4.0*Ti; // Maximum error
	}
}

/* virtual */ YsVec3 YsShellExt_NormalBasedSegmentationBase::CalculateNormal(const YsShellExt &shl,YsShell::PolygonHandle plHd) const
{
	YsArray <YsVec3,4> plVtPos;
	shl.GetPolygon(plVtPos,plHd);
	return YsGetAverageNormalVector(plVtPos);
}

YsArray <YsShell::PolygonHandle> YsShellExt_NormalBasedSegmentationBase::GetFaceGroup(const YsShellExt &shl,YsShellExt::FaceGroupHandle fgHd) const
{
	auto fgPlHd=shl.GetFaceGroup(fgHd);

	auto fgPlPtr=tentativeFgToPlgTable[fgHd];
	if(nullptr!=fgPlPtr)
	{
		fgPlHd.Add(*fgPlPtr);
	}
	return fgPlHd;
}

void YsShellExt_NormalBasedSegmentationBase::AddFaceGroupPolygon(const YsShellExt &shl,YsShellExt::FaceGroupHandle fgHd,YsShell::PolygonHandle plHd)
{
	auto fgPlPtr=tentativeFgToPlgTable[fgHd];
	if(nullptr!=fgPlPtr)
	{
		fgPlPtr->Add(plHd);
	}
	else
	{
		YsArray <YsShell::PolygonHandle> fgPlHd;
		fgPlHd.Add(plHd);
		tentativeFgToPlgTable.SetAttrib(fgHd,fgPlHd);
	}

	tentativePlgToFgTable.SetAttrib(plHd,fgHd);
}

YsShellExt::FaceGroupHandle YsShellExt_NormalBasedSegmentationBase::FindFaceGroupFromPolygon(const YsShellExt &shl,YsShell::PolygonHandle plHd) const
{
	auto fgHd=shl.FindFaceGroupFromPolygon(plHd);
	if(nullptr!=fgHd)
	{
		return fgHd;
	}
	auto fgHdPtr=tentativePlgToFgTable[plHd];
	if(nullptr!=fgHdPtr)
	{
		return *fgHdPtr;
	}
	return nullptr;
}

////////////////////////////////////////////////////////////

void YsShellExt_FaceGroupGrowing::Begin(const YsShellExt &shl,const Preference &pref)
{
printf("%s %d\n",__FUNCTION__,__LINE__);
	YsShellExt_NormalBasedSegmentationBase::Begin(shl);

printf("%s %d\n",__FUNCTION__,__LINE__);
	fgPlKeyToTreeNode.SetOrderSensitivity(YSTRUE);
	for(auto fgHd : shl.AllFaceGroup())
	{
		UpdateFaceGroupMetric(shl,fgHd,pref);
	}
printf("%s %d\n",__FUNCTION__,__LINE__);
}

void YsShellExt_FaceGroupGrowing::UpdateFaceGroupMetric(const YsShellExt &shl,YsShellExt::FaceGroupHandle fgHd,const Preference &pref)
{
	auto fgPlHd=GetFaceGroup(shl,fgHd);
	for(auto plHd : fgPlHd)
	{
		auto nPlVt=shl.GetPolygonNumVertex(plHd);
		for(int i=0; i<nPlVt; ++i)
		{
			auto neiPlHd=shl.GetNeighborPolygon(plHd,i);
			if(nullptr!=neiPlHd && nullptr==FindFaceGroupFromPolygon(shl,neiPlHd))
			{
				auto metric=CalculateMetric(shl,fgHd,neiPlHd,pref.errorMetricType);

				YsShellExt::PolygonAndFaceGroup plFg;
				plFg.plHd=neiPlHd;
				plFg.fgHd=fgHd;

				auto currentHd=FindTreeNodeHandle(shl,fgHd,neiPlHd);
				if(currentHd!=nullptr)
				{
					metricTree.Delete(currentHd);
				}

				AddTreeNode(shl,fgHd,neiPlHd,metric);
			}
		}
	}
}

YsAVLTree <double,YsShellExt::PolygonAndFaceGroup>::NodeHandle YsShellExt_FaceGroupGrowing::AddTreeNode(const YsShellExt &shl,YsShellExt::FaceGroupHandle fgHd,YsShell::PolygonHandle plHd,const double metric)
{
	YsShellExt::PolygonAndFaceGroup fgPl;
	fgPl.fgHd=fgHd;
	fgPl.plHd=plHd;
	auto hd=metricTree.Insert(metric,fgPl);

	const YSHASHKEY fgPlKey[2]={shl.GetSearchKey(fgHd),shl.GetSearchKey(plHd)};
	fgPlKeyToTreeNode.Update(2,fgPlKey,hd);

	return hd;
}
YsAVLTree <double,YsShellExt::PolygonAndFaceGroup>::NodeHandle YsShellExt_FaceGroupGrowing::FindTreeNodeHandle(const YsShellExt &shl,YsShellExt::FaceGroupHandle fgHd,YsShell::PolygonHandle plHd) const
{
	const YSHASHKEY fgPlKey[2]={shl.GetSearchKey(fgHd),shl.GetSearchKey(plHd)};
	auto nodeHdPtr=fgPlKeyToTreeNode.Find(2,fgPlKey);
	if(nullptr!=nodeHdPtr)
	{
		return *nodeHdPtr;
	}
	return metricTree.Null();
}

YSSIZE_T YsShellExt_FaceGroupGrowing::GetN(void) const
{
	return metricTree.GetN();
}

