/* ////////////////////////////////////////////////////////////

File Name: ysshellext_thicknessutil.cpp
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

#include <thread>

#include "ysshellext_thicknessutil.h"



YsShellExt_ThicknessEstimation::Preference::Preference()
{
	opposingAngleThr=YsPi*5.0/6.0;  // 150 degree
	excludeVertexConnectedNeighbor=YSTRUE;
	edgeSampleDhaThr=YsPi/3.0;
}


YsShellExt_ThicknessEstimation::YsShellExt_ThicknessEstimation()
{
	CleanUp();
	cond=nullptr;
}

YsShellExt_ThicknessEstimation::~YsShellExt_ThicknessEstimation()
{
	CleanUp();
}

void YsShellExt_ThicknessEstimation::CleanUp(void)
{
	thicknessSample.CleanUp();
	plgThickness.CleanUp();
	plgThicknessFilled.CleanUp();
}

void YsShellExt_ThicknessEstimation::SetCondition(const YsShellExt::Condition *cond)
{
	this->cond=cond;
}

class YsShellExt_ThicknessEstimation::MakeSampleThreadInfo
{
public:
	int threadIdx;
	YsShellExt_ThicknessEstimation *thisPtr;
	YsArray <Sample> sample;
	YsArray <YsShell::PolygonHandle> plHdArray;
	const YsShellExt *shlPtr;
	const YsShellLattice *ltcPtr;
	const Preference *prefPtr;
	const YsShellExt::Condition *cond;
};

void YsShellExt_ThicknessEstimation::MakeSample(const YsShellExt &shl,const YsShellLattice &ltc,const Preference &pref,int nThread)
{
	CleanUp();


	if(1>nThread)
	{
		nThread=1;
	}

	YsArray <MakeSampleThreadInfo> info(nThread,nullptr);

	for(auto idx : info.AllIndex())
	{
		info[idx].threadIdx=(int)idx;
		info[idx].thisPtr=this;
		info[idx].shlPtr=&shl;
		info[idx].ltcPtr=&ltc;
		info[idx].prefPtr=&pref;
		info[idx].cond=this->cond;
	}
	{
		int threadIdx=0;
		for(auto plHd : shl.AllPolygon())
		{
			info[threadIdx].plHdArray.Add(plHd);
			threadIdx=(threadIdx+1)%nThread;
		}
	}

	YsArray <std::thread> threadArray;
	for(auto &i : info)
	{
		threadArray.Add(std::thread(MakeSampleThreadCallBack,&i));
	}
	for(auto &t : threadArray)
	{
		t.join();
	}
	for(auto &i : info)
	{
		thicknessSample.Add(i.sample);
	}
}

void YsShellExt_ThicknessEstimation::AddSamplePerPolygon(YsArray <Sample> &sample,const YsShellExt &shl,YsShell::PolygonHandle fromPlHd,const YsShellLattice &ltc,const Preference &pref)
{
	const double cosOpposingAngleThreshold=cos(pref.opposingAngleThr);

	YsArray <YsVec3,4> plVtPos;
	shl.GetPolygon(plVtPos,fromPlHd);



	auto nom=shl.GetNormal(fromPlHd);
	if(YSOK!=nom.Normalize())
	{
		return;
	}



	YsArray <YsShell::PolygonHandle> excludePlHd;
	if(YSTRUE==pref.excludeVertexConnectedNeighbor)
	{
		auto plVtHd=shl.GetPolygonVertex(fromPlHd);
		for(auto vtHd : plVtHd)
		{
			auto vtPlHd=shl.FindPolygonFromVertex(vtHd);
			excludePlHd.Add(vtPlHd);
		}
		YsRemoveDuplicateInUnorderedArray(excludePlHd);
	}
	else
	{
		excludePlHd.Add(fromPlHd);
	}



	YsArray <YsVec3> samplePnt;
	if(3==plVtPos.GetN() || YSTRUE==YsCheckConvex3(plVtPos))
	{
		samplePnt.Add(YsGetCenter(plVtPos));
	}
	else
	{
		YsVec3 pos;
		if(YSOK==YsGetArbitraryInsidePointOfPolygon3(pos,plVtPos))
		{
			samplePnt.Add(pos);
		}
		else
		{
			return;
		}
	}



	for(auto fromPos : samplePnt)
	{
		YsArray <YsShell::PolygonHandle,2> srcPlHd;
		srcPlHd.push_back(fromPlHd);
		ShootRayAddSample(sample,shl,srcPlHd,ltc,pref,fromPos,nom,excludePlHd);
	}


	// Sample from edges.
	YSSIZE_T nPlVt=shl.GetPolygonNumVertex(fromPlHd);
	for(YSSIZE_T edIdx=0; edIdx<nPlVt; ++edIdx)
	{
		auto neiPlHd=shl.GetNeighborPolygon(fromPlHd,edIdx);
		if(nullptr!=neiPlHd)
		{
			const YsVec3 neiNom=shl.GetNormal(neiPlHd);
			if(nom*neiNom<cos(pref.edgeSampleDhaThr))
			{
				auto avgNom=YsVec3::UnitVector(nom+neiNom);

				auto edge=shl.GetPolygonEdge(fromPlHd,edIdx);
				auto fromPos=shl.GetCenter(edge);

				if(YSTRUE!=pref.excludeVertexConnectedNeighbor)
				{
					excludePlHd.Set(2,nullptr);
					excludePlHd[0]=fromPlHd;
					excludePlHd[1]=neiPlHd;
				}
				// If TRUE, neiPlHd is already included.

				YsArray <YsShell::PolygonHandle,2> srcPlHd;
				srcPlHd.push_back(fromPlHd);
				srcPlHd.push_back(neiPlHd);
				ShootRayAddSample(sample,shl,srcPlHd,ltc,pref,fromPos,avgNom,excludePlHd);
			}
		}
	}
}

void YsShellExt_ThicknessEstimation::ShootRayAddSample(
	    YsArray <Sample> &sample,
	    const YsShellExt &shl,
	    const YsConstArrayMask <YsShell::PolygonHandle> &fromPlHd,
	    const YsShellLattice &ltc,
	    const Preference &pref,
	    const YsVec3 &fromPos,const YsVec3 &nom,
	    const YsConstArrayMask <YsShell::PolygonHandle> &excludePlHd)
{
	YsShell::PolygonHandle iPlHd;
	YsVec3 itsc;
	auto res=ltc.FindFirstIntersection(iPlHd,itsc,fromPos,-nom,excludePlHd.GetN(),excludePlHd);

	if(YSOK==res && nullptr!=iPlHd)
	{
		if(nullptr!=cond && YSOK!=cond->TestPolygon(shl,iPlHd))
		{
			return;
		}

		auto oppNom=shl.GetNormal(iPlHd);
		if(YSOK==oppNom.Normalize() && nom*oppNom<cos(pref.opposingAngleThr))
		{
			sample.Increment();
			sample.Last().fromPlHd=fromPlHd;
			sample.Last().toPlHd=iPlHd;
			sample.Last().from=fromPos;
			sample.Last().to=itsc;
		}
	}
}


/* static */ void YsShellExt_ThicknessEstimation::MakeSampleThreadCallBack(MakeSampleThreadInfo *infoPtr)
{
	auto &shl=*(infoPtr->shlPtr);
	auto &ltc=*(infoPtr->ltcPtr);
	auto &pref=*(infoPtr->prefPtr);
	auto cond=infoPtr->cond;
	for(auto plHd : infoPtr->plHdArray)
	{
		if(nullptr!=cond && YSOK!=cond->TestPolygon(shl,plHd))
		{
			continue;
		}
		infoPtr->thisPtr->AddSamplePerPolygon(infoPtr->sample,shl,plHd,ltc,pref);
	}
}

const YsArray <YsShellExt_ThicknessEstimation::Sample> &YsShellExt_ThicknessEstimation::GetSample(void) const
{
	return thicknessSample;
}



////////////////////////////////////////////////////////////

class YsShellExt_ThicknessEstimation::TrimmingLatticeThreadInfo
{
public:
	int threadIdx;
	YsLattice3d <YsArray <Sample *> > *ltcPtr;
	YsArray <Sample *> toWork;
	YsArray <YsPair <YsVec3i,Sample *> > ltcIdxSamplePointer;
};

class YsShellExt_ThicknessEstimation::TrimSampleThreadInfo
{
public:
	int threadIdx;
	YsShellExt_ThicknessEstimation *thisPtr;
	const YsLattice3d <YsArray <Sample *> > *ltcPtr;
	const YsShell *shlPtr;

	YsArray <Sample *> toWork,toInvalidate;
};

void YsShellExt_ThicknessEstimation::TrimSample(const YsShell &shl,int nThread)
{
	YsVec3 bbx[2];
	shl.GetBoundingBox(bbx);
	YsInflateBoundingBox(bbx[0],bbx[1],bbx[0],bbx[1],1.1);


	YsLattice3d <YsArray <Sample *> > ltc;
	if(YSOK!=ltc.Create(1+thicknessSample.GetN(),bbx[0],bbx[1]))
	{
		return;
	}

	if(1>nThread)
	{
		nThread=1;
	}



	{
		YsArray <TrimmingLatticeThreadInfo> info(nThread,nullptr);
		for(auto idx : info.AllIndex())
		{
			auto &i=info[idx];
			i.threadIdx=(int)idx;
			i.ltcPtr=&ltc;
		}
		{
			int threadIdx=0;
			for(auto &s : thicknessSample)
			{
				info[threadIdx].toWork.Add(&s);
				threadIdx=(threadIdx+1)%nThread;
			}
		}
		YsArray <std::thread> threadArray;
		for(auto &i : info)
		{
			threadArray.Add(std::thread(MakeTrimmingLatticeThreadCallBack,&i));
		}
		for(auto &t : threadArray)
		{
			t.join();
		}
		for(auto &i : info)
		{
			for(auto ltcIdx : i.ltcIdxSamplePointer)
			{
				auto blk=ltc.GetBlock(ltcIdx.a);
				if(nullptr!=blk)
				{
					blk->Add(ltcIdx.b);
				}
			}
		}
	}



	{
		YsArray <TrimSampleThreadInfo> info(nThread,nullptr);
		for(auto idx : info.AllIndex())
		{
			auto &i=info[idx];
			i.threadIdx=(int)idx;
			i.thisPtr=this;
			i.ltcPtr=&ltc;
			i.shlPtr=&shl;
		}
		{
			int threadIdx=0;
			for(auto &s : thicknessSample)
			{
				info[threadIdx].toWork.Add(&s);
				threadIdx=(threadIdx+1)%nThread;
			}
		}
		YsArray <std::thread> threadArray;
		for(auto &i : info)
		{
			threadArray.Add(std::thread(TrimSampleThreadCallBack,&i));
		}
		for(auto &t : threadArray)
		{
			t.join();
		}
		for(auto &i : info)
		{
			for(auto sPtr : i.toInvalidate)
			{
				sPtr->valid=YSFALSE;
			}
		}
	}
}

/* static */ void YsShellExt_ThicknessEstimation::MakeTrimmingLatticeThreadCallBack(TrimmingLatticeThreadInfo *infoPtr)
{
	auto &info=*infoPtr;
	auto &ltc=*info.ltcPtr;

	for(auto sPtr : info.toWork)
	{
		auto &s=*sPtr;

		YsArray <YsVec3i> itsc;
		if(YSOK==ltc.MakeIntersectingBlockList(itsc,s.from,s.to))
		{
			for(auto idx : itsc)
			{
				info.ltcIdxSamplePointer.Increment();
				info.ltcIdxSamplePointer.Last().a=idx;
				info.ltcIdxSamplePointer.Last().b=sPtr;
			}
		}
	}
}

/* static */ void YsShellExt_ThicknessEstimation::TrimSampleThreadCallBack(TrimSampleThreadInfo *infoPtr)
{
	auto &info=*infoPtr;
	auto &shl=*info.shlPtr;
	auto &ltc=*info.ltcPtr;


	for(auto s1 : info.toWork)
	{
		auto &s=*s1;
		if(YSTRUE!=s.valid)
		{
			continue;
		}

		const YsVec3 v=s.to-s.from;

		for(auto fromPlHd : s.fromPlHd)
		{
			YsArray <YsVec3,4> plVtPos;
			shl.GetPolygon(plVtPos,fromPlHd);

			YsArray <unsigned int,9> itsc;

			for(auto idx : plVtPos.AllIndex())
			{
				const YsVec3 quad[4]=
				{
					plVtPos[idx],
					plVtPos.GetCyclic(idx+1),
					plVtPos.GetCyclic(idx+1)+v,
					plVtPos[idx]+v,
				};

				YsArray <YsVec3i> itscBlk;
				if(YSOK==ltc.MakeIntersectingBlockListPolygon(itscBlk,4,quad))
				{
					YsArray <Sample *> needCheck;
					for(auto idx : itscBlk)
					{
						needCheck.Add(*ltc.GetBlock(idx));
					}

					YsPlane pln;
					if(0<needCheck.GetN() && YSOK==pln.MakePlaneFromTriangle(quad[0],quad[1],quad[2]))
					{
						YsRemoveDuplicateInUnorderedArray(needCheck);

						for(auto s2 : needCheck)
						{
							YsVec3 itsc;
							if(s2!=&s &&
							   YSTRUE==s2->valid &&
							   YSOK==pln.GetPenetration(itsc,s2->from,s2->to) &&
							   YSINSIDE==YsCheckInsidePolygon3(itsc,4,quad))
							{
								if((s.to-s.from).GetSquareLength()>(s2->to-s2->from).GetSquareLength())
								{
									info.toInvalidate.Add(&s);
									break;
								}
								else
								{
									info.toInvalidate.Add(s2);
								}
							}
						}
					}
				}
			}
		}
	}
}



void YsShellExt_ThicknessEstimation::AssignPolygonThickness(const YsShellExt &shl)
{
	plgThickness.CleanUp();
	plgThickness.SetShell(shl.Conv());

	for(auto &s : thicknessSample)
	{
		if(YSTRUE==s.valid)
		{
			auto L=(s.from-s.to).GetLength();
			for(auto fromPlHd : s.fromPlHd)
			{
				plgThickness.SetAttrib(fromPlHd,L);
			}
		}
	}
}



void YsShellExt_ThicknessEstimation::SmoothPolygonThickness(const YsShellExt &shl,int nIter,int nThread)
{
	plgThicknessFilled.CleanUp();

	YsArray <YsShell::PolygonHandle> srcPlHd;
	for(auto plHd : shl.AllPolygon())
	{
		if(nullptr!=plgThickness[plHd])
		{
			srcPlHd.Add(plHd);
		}
		else
		{
			plgThicknessFilled.Add(plHd);
		}
	}

	while(0<srcPlHd.GetN())
	{
		auto plHd=srcPlHd.Last();
		srcPlHd.DeleteLast();

		auto t=*plgThickness[plHd];

		auto nPlVt=shl.GetPolygonNumVertex(plHd);
		for(decltype(nPlVt) edIdx=0; edIdx<nPlVt; ++edIdx)
		{
			auto neiPlHd=shl.GetNeighborPolygon(plHd,edIdx);
			if(nullptr!=neiPlHd && nullptr==plgThickness[neiPlHd])
			{
				plgThickness.SetAttrib(neiPlHd,t);
				srcPlHd.Add(neiPlHd);
			}
		}
	}

	while(0<nIter)
	{
		SmoothPolygonThicknessOneIteration(shl,nThread);
		--nIter;
	}
}



class YsShellExt_ThicknessEstimation::SmoothPolygonThreadInfo
{
public:
	int threadIdx;
	YsShellExt_ThicknessEstimation *thisPtr;
	const YsShellExt *shlPtr;

	YsArray <YsShell::PolygonHandle> toWork;
	YsArray <YsPair <YsShell::PolygonHandle,double> > newThickness;
};

void YsShellExt_ThicknessEstimation::SmoothPolygonThicknessOneIteration(const YsShellExt &shl,int nThread)
{
	YsMakeGreater(nThread,1);

	YsArray <SmoothPolygonThreadInfo> info(nThread,nullptr);
	for(auto idx : info.AllIndex())
	{
		auto &i=info[idx];
		i.threadIdx=(int)idx;
		i.thisPtr=this;
		i.shlPtr=&shl;
	}
	{
		int threadIdx=0;
		for(auto plHd : plgThicknessFilled)
		{
			info[threadIdx].toWork.Add(plHd);
			threadIdx=(threadIdx+1)%nThread;
		}
	}
	YsArray <std::thread> threadArray;
	for(auto &i : info)
	{
		threadArray.Add(std::thread(SmoothPolygonThreadCallBack,&i));
	}
	for(auto &t : threadArray)
	{
		t.join();
	}
	for(auto &i : info)
	{
		for(auto nt : i.newThickness)
		{
			plgThickness.SetAttrib(nt.a,nt.b);
		}
	}
}

/* static */ void YsShellExt_ThicknessEstimation::SmoothPolygonThreadCallBack(SmoothPolygonThreadInfo *infoPtr)
{
	auto &info=*infoPtr;
	auto &shl=*info.shlPtr;

	for(auto plHd : info.toWork)
	{
		auto nPlVt=shl.GetPolygonNumVertex(plHd);
		decltype(nPlVt) nSample=0;
		double sample=0.0;
		for(decltype(nPlVt) edIdx=0; edIdx<nPlVt; ++edIdx)
		{
			auto neiPlHd=shl.GetNeighborPolygon(plHd,edIdx);
			if(nullptr!=neiPlHd)
			{
				auto t=info.thisPtr->plgThickness[neiPlHd];
				if(nullptr!=t)
				{
					sample+=*t;
					++nSample;
				}
			}
		}
		if(0<nSample)
		{
			info.newThickness.Increment();
			info.newThickness.Last().a=plHd;
			info.newThickness.Last().b=sample/(double)nSample;
		}
	}
}




YSBOOL YsShellExt_ThicknessEstimation::IsThicknessAssignedPolygon(YsShell::PolygonHandle plHd) const
{
	if(nullptr!=plgThickness[plHd])
	{
		return YSTRUE;
	}
	return YSFALSE;
}

YSBOOL YsShellExt_ThicknessEstimation::IsThicknessAssignedPolygon(const YsShell &shl,YsShell::PolygonHandle plHd) const
{
	if(nullptr!=plgThickness[shl.GetSearchKey(plHd)])
	{
		return YSTRUE;
	}
	return YSFALSE;
}

double YsShellExt_ThicknessEstimation::GetPolygonThickness(YsShell::PolygonHandle plHd) const
{
	auto tPtr=plgThickness[plHd];
	if(nullptr!=tPtr)
	{
		return *tPtr;
	}
	return 0.0;
}

double YsShellExt_ThicknessEstimation::GetPolygonThickness(const YsShell &shl,YsShell::PolygonHandle plHd) const
{
	auto tPtr=plgThickness[shl.GetSearchKey(plHd)];
	if(nullptr!=tPtr)
	{
		return *tPtr;
	}
	return 0.0;
}
