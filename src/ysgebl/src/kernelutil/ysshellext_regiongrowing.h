/* ////////////////////////////////////////////////////////////

File Name: ysshellext_regiongrowing.h
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

#ifndef YSSHELLEXT_REGIONGROWING_IS_INCLUDED
#define YSSHELLEXT_REGIONGROWING_IS_INCLUDED
/* { */

#include <ysshellext.h>

// I should have made this, and then create YsShellExt_FaceGroupGrowing from this class.
template <class SeedInfo>
class YsShellExt_PolygonRegionGrowing
{
public:
	class Region
	{
	public:
		SeedInfo seedInfo;
		YsArray <YsShell::PolygonHandle> plHd;
		YsShellEdgeStore regionBoundary;
	};

	typedef YsEditArrayObjectHandle <Region,8> RegionHandle;

	class RegionHandleAndPolygonHandle
	{
	public:
		RegionHandle rgHd;
		YsShell::PolygonHandle plHd;
	};

	typedef typename YsAVLTree <double,RegionHandleAndPolygonHandle>::NodeHandle TreeHandle;

public: // Will be made private when stable.
	YsEditArray <Region,8> regionArray;

	YsShellPolygonAttribTable <RegionHandle> plHdToRgHd;
	YsAVLTree <double,RegionHandleAndPolygonHandle> metricTree;
	YsFixedLengthHashTable <TreeHandle,2> rgPlKeyToTreeNode;


public:
	void Begin(const YsShellExt &shl);

	/*! This function creates a region.  However, it does not automatically initializes the seed info.
	    The caller is responsible for initializing the seed info.
	*/
	RegionHandle Create(void);

	Region *GetRegion(RegionHandle rgHd);
	const Region *GetRegion(RegionHandle rgHd) const;

	/*! This function will NOT update metric.  Call UpdateMetric after AddRegionPolygon if the metric must be updated. */
	void AddRegionPolygon(const YsShellExt &shl,RegionHandle rgHd,YsShell::PolygonHandle plHd);

	void UpdateMetric(const YsShellExt &shl,RegionHandle rgHd);
	void UpdateMetric(const YsShellExt &shl,RegionHandle rgHd,YsShell::PolygonHandle plHd);

	void GrowOneStep(const YsShellExt &shl);

	YSSIZE_T GetN(void) const;

	 typename YsEditArray <Region,8>::HandleEnumerator AllRegion(void) const;

public:
	virtual YSBOOL CanGrow(const YsShellExt &shl,RegionHandle rgHd,YsShell::PolygonHandle fromPlHd,YsShell::PolygonHandle toPlHd) const;
	virtual void UpdateRegion(const YsShellExt &shl,RegionHandle rgHd,YsShell::PolygonHandle addedPlHd);
	virtual double CalculateMetric(const YsShellExt &shl,RegionHandle rgHd,YsShell::PolygonHandle plHd) const=0;
	virtual YSBOOL Inseparable(const YsShellExt &shl,YsShell::PolygonHandle plHd0,YsShell::PolygonHandle plHd1) const;
};

template <class SeedInfo>
void YsShellExt_PolygonRegionGrowing <SeedInfo>::Begin(const YsShellExt &shl)
{
	regionArray.CleanUp();
	regionArray.EnableSearch();

	plHdToRgHd.CleanUp();
	plHdToRgHd.SetShell(shl.Conv());

	metricTree.CleanUp();

	rgPlKeyToTreeNode.CleanUpThin();
	rgPlKeyToTreeNode.SetOrderSensitivity(YSTRUE);
}

template <class SeedInfo>
typename YsShellExt_PolygonRegionGrowing <SeedInfo>::RegionHandle YsShellExt_PolygonRegionGrowing <SeedInfo>::Create(void)
{
	auto hd=regionArray.Create();
	regionArray[hd]->plHd.CleanUp();
	regionArray[hd]->regionBoundary.CleanUp();
	return hd;
}
template <class SeedInfo>
typename YsShellExt_PolygonRegionGrowing<SeedInfo>::Region *YsShellExt_PolygonRegionGrowing<SeedInfo>::GetRegion(RegionHandle rgHd)
{
	return regionArray[rgHd];
}
template <class SeedInfo>
const typename YsShellExt_PolygonRegionGrowing<SeedInfo>::Region *YsShellExt_PolygonRegionGrowing<SeedInfo>::GetRegion(RegionHandle rgHd) const
{
	return regionArray[rgHd];
}
template <class SeedInfo>
void YsShellExt_PolygonRegionGrowing <SeedInfo>::AddRegionPolygon(const YsShellExt &shl,RegionHandle rgHd,YsShell::PolygonHandle plHd)
{
	auto rg=regionArray[rgHd];
	rg->plHd.Add(plHd);

	rg->regionBoundary.SetShell(shl.Conv());
	auto plVtHd=shl.GetPolygonVertex(plHd);
	for(auto edIdx : plVtHd.AllIndex())
	{
		const YsShell::VertexHandle edVtHd[2]=
		{
			plVtHd[edIdx],
			plVtHd.GetCyclic(edIdx+1)
		};
		if(rg->regionBoundary.IsIncluded(edVtHd))
		{
			rg->regionBoundary.Delete(edVtHd);
		}
		else
		{
			rg->regionBoundary.Add(edVtHd);
		}
	}

	plHdToRgHd.SetAttrib(plHd,rgHd);
	UpdateRegion(shl,rgHd,plHd);
}
template <class SeedInfo>
void YsShellExt_PolygonRegionGrowing <SeedInfo>::UpdateMetric(const YsShellExt &shl,RegionHandle rgHd)
{
	auto rgPtr=regionArray[rgHd];

	for(auto edge : rgPtr->regionBoundary)
	{
		auto edPlHd=shl.FindPolygonFromEdgePiece(edge);
		if(2==edPlHd.GetN())
		{
			auto rgHdPtr0=plHdToRgHd[edPlHd[0]];
			auto rgHdPtr1=plHdToRgHd[edPlHd[1]];
			const RegionHandle rgHd0=(nullptr!=rgHdPtr0 ? *rgHdPtr0 : nullptr);
			const RegionHandle rgHd1=(nullptr!=rgHdPtr1 ? *rgHdPtr1 : nullptr);
			if(rgHd0==rgHd && nullptr==rgHd1 && YSTRUE==CanGrow(shl,rgHd,edPlHd[0],edPlHd[1]))
			{
				UpdateMetric(shl,rgHd,edPlHd[1]);
			}
			else if(nullptr==rgHd0 && rgHd1==rgHd && YSTRUE==CanGrow(shl,rgHd,edPlHd[1],edPlHd[0]))
			{
				UpdateMetric(shl,rgHd,edPlHd[0]);
			}
		}
	}

	// Exhaustive method that works, but slow.
	// for(auto plHd : rgPtr->plHd)
	// {
	// 	const int nPlVt=shl.GetPolygonNumVertex(plHd);
	// 	for(int edIdx=0; edIdx<nPlVt; ++edIdx)
	// 	{
	// 		auto neiPlHd=shl.GetNeighborPolygon(plHd,edIdx);
	// 		if(nullptr!=neiPlHd && nullptr==plHdToRgHd[neiPlHd] && YSTRUE==CanGrow(shl,rgHd,plHd,neiPlHd))
	// 		{
	// 			UpdateMetric(shl,rgHd,neiPlHd);
	// 		}
	// 	}
	// }
}
template <class SeedInfo>
void YsShellExt_PolygonRegionGrowing <SeedInfo>::UpdateMetric(const YsShellExt &shl,RegionHandle rgHd,YsShell::PolygonHandle plHd)
{
	const YSHASHKEY rgPlKey[2]={regionArray.GetSearchKey(rgHd),shl.GetSearchKey(plHd)};
	auto hdPtr=rgPlKeyToTreeNode.Find(2,rgPlKey);
	if(nullptr!=hdPtr)
	{
		metricTree.Delete(*hdPtr);
		rgPlKeyToTreeNode.DeleteKey(2,rgPlKey);
	}

	const double metric=CalculateMetric(shl,rgHd,plHd);
	RegionHandleAndPolygonHandle rgPl;
	rgPl.rgHd=rgHd;
	rgPl.plHd=plHd;

	auto treeHd=metricTree.Insert(metric,rgPl);
	rgPlKeyToTreeNode.Add(2,rgPlKey,treeHd);
}

template <class SeedInfo>
void YsShellExt_PolygonRegionGrowing <SeedInfo>::GrowOneStep(const YsShellExt &shl)
{
	auto treeHd=metricTree.First();
	if(treeHd.IsNotNull())
	{
		auto treeNode=metricTree.GetValue(treeHd);
		auto rgHd=treeNode.rgHd;
		auto plHd=treeNode.plHd;

		const YSHASHKEY rgPlKey[2]={regionArray.GetSearchKey(rgHd),shl.GetSearchKey(plHd)};
		metricTree.Delete(treeHd);
		rgPlKeyToTreeNode.Delete(2,rgPlKey);

		if(nullptr==plHdToRgHd[plHd])
		{
			AddRegionPolygon(shl,rgHd,plHd);
			UpdateMetric(shl,rgHd);
		}

		auto nPlVt=shl.GetPolygonNumVertex(plHd);
		for(int idx=0; idx<nPlVt; ++idx)
		{
			auto neiPlHd=shl.GetNeighborPolygon(plHd,idx);
			if(nullptr!=neiPlHd &&
			   YSTRUE==Inseparable(shl,plHd,neiPlHd) &&
			   nullptr==plHdToRgHd[neiPlHd])
			{
				AddRegionPolygon(shl,rgHd,neiPlHd);
				UpdateMetric(shl,rgHd);
			}
		}
	}
}

template <class SeedInfo>
YSSIZE_T YsShellExt_PolygonRegionGrowing<SeedInfo>::GetN(void) const
{
	return metricTree.GetN();
}

template <class SeedInfo>
typename YsEditArray <typename YsShellExt_PolygonRegionGrowing<SeedInfo>::Region,8>::HandleEnumerator YsShellExt_PolygonRegionGrowing<SeedInfo>::AllRegion(void) const
{
	return regionArray.AllHandle();
}

template <class SeedInfo>
/* virtual */ YSBOOL YsShellExt_PolygonRegionGrowing <SeedInfo>::CanGrow(const YsShellExt &,RegionHandle ,YsShell::PolygonHandle ,YsShell::PolygonHandle ) const
{
	return YSTRUE;
}

template <class SeedInfo>
/* virtual */ void YsShellExt_PolygonRegionGrowing <SeedInfo>::UpdateRegion(const YsShellExt &shl,RegionHandle,YsShell::PolygonHandle)
{
}

template <class SeedInfo>
/* virtual */ YSBOOL YsShellExt_PolygonRegionGrowing<SeedInfo>::Inseparable(const YsShellExt &shl,YsShell::PolygonHandle plHd0,YsShell::PolygonHandle plHd1) const
{
	return YSFALSE;
}

////////////////////////////////////////////////////////////

class YsShellExt_RegionGrowingFromFixedNormal : public YsShellExt_PolygonRegionGrowing <YsVec3>
{
public:
	virtual double CalculateMetric(const YsShellExt &shl,RegionHandle rgHd,YsShell::PolygonHandle plHd) const;
};

/* } */
#endif
