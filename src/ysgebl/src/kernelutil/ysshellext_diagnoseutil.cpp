/* ////////////////////////////////////////////////////////////

File Name: ysshellext_diagnoseutil.cpp
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

#include "ysshellext_diagnoseutil.h"

#include <ysclass11.h>
#include <ystask.h>

#include <thread>

YsShellExt_DiagnoseUtil::YsShellExt_DiagnoseUtil()
{
	nThread=1;
	shl=NULL;
	externalLtc=NULL;
}

void YsShellExt_DiagnoseUtil::CleanUp(void)
{
	nonManifoldEdgeArray.CleanUp();
	selfIntersectionArray.CleanUp();
	nonPlanarPolygonArray.CleanUp();
}

void YsShellExt_DiagnoseUtil::SetNumThread(int nThread)
{
	if(1>nThread)
	{
		nThread=1;
	}
	this->nThread=nThread;
}

void YsShellExt_DiagnoseUtil::SetShell(const YsShellExt &shl)
{
	this->shl=&shl;
}

void YsShellExt_DiagnoseUtil::SetLattice(const YsShellLattice &ltc)
{
	this->externalLtc=&ltc;
}

void YsShellExt_DiagnoseUtil::CheckNonManifoldEdge(void)
{
	nonManifoldEdgeArray.CleanUp();

	const YsShellExt &shl=*(this->shl);
	YsShellEdgeEnumHandle edHd=NULL;
	while(YSOK==shl.MoveToNextEdge(edHd))
	{
		YsShellVertexHandle edVtHd[2];
		shl.GetEdge(edVtHd,edHd);

		int nPlg=shl.GetNumPolygonUsingEdge(edVtHd);
		if(2!=nPlg)
		{
			nonManifoldEdgeArray.Increment();
			nonManifoldEdgeArray.Last().edVtHd[0]=edVtHd[0];
			nonManifoldEdgeArray.Last().edVtHd[1]=edVtHd[1];
			nonManifoldEdgeArray.Last().nUsed=nPlg;
		}
	}
}

YSSIZE_T YsShellExt_DiagnoseUtil::GetNumNonManifoldEdge(void) const
{
	return nonManifoldEdgeArray.GetN();
}

////////////////////////////////////////////////////////////

class YsShellExt_DiagnoseUtil::CheckSelfIntersection_ThreadInfo : public YsTask
{
public:
	YsShellExt_DiagnoseUtil *util;
	const YsShellExt *shl;
	const YsShellLattice *ltc;
	YsArray <YsShellPolygonHandle> plHdArray;

	YsArray <YsShellPolygonHandle> itscPlHdArray;

protected:
	virtual void Run(void);
};

void YsShellExt_DiagnoseUtil::CheckSelfIntersection(void)
{
	selfIntersectionArray.CleanUp();

	const YsShellExt &shl=*(this->shl);
	YSSIZE_T nCell=1000+shl.GetNumPolygon();

printf("%s %d %d\n",__FUNCTION__,__LINE__,debug_threadId);
	const YsShellLattice *ltcPtr=NULL;
	YsShellLattice temporaryLtc;
	if(NULL!=externalLtc)
	{
		ltcPtr=externalLtc;
	}
	else
	{
		temporaryLtc.SetDomain(shl.Conv(),nCell);
		ltcPtr=&temporaryLtc;
	}
printf("%s %d %d\n",__FUNCTION__,__LINE__,debug_threadId);

	YsArray <CheckSelfIntersection_ThreadInfo> info(nThread,NULL);
	for(YSSIZE_T threadId=0; threadId<nThread; ++threadId)
	{
		info[threadId].util=this;
		info[threadId].shl=&shl;
		info[threadId].ltc=ltcPtr;
	}

printf("%s %d %d\n",__FUNCTION__,__LINE__,debug_threadId);
	{
		int threadId=0;
		for(auto plHd : shl.AllPolygon())
		{
			info[threadId].plHdArray.Append(plHd);
			threadId=(threadId+1)%nThread;
		}
	}

printf("%s %d %d\n",__FUNCTION__,__LINE__,debug_threadId);
	YsRunTaskParallelDirect(info);
	for(YSSIZE_T threadId=0; threadId<nThread; ++threadId)
	{
		for(auto plHd : info[threadId].itscPlHdArray)
		{
			selfIntersectionArray.Increment();
			selfIntersectionArray.Last().plHd=plHd;
		}
	}

printf("%s %d %d\n",__FUNCTION__,__LINE__,debug_threadId);
}

void YsShellExt_DiagnoseUtil::CheckSelfIntersection_ThreadInfo::Run(void)
{
	util->CheckSelfIntersection_Thread(*this);
}

void YsShellExt_DiagnoseUtil::CheckSelfIntersection_Thread(YsShellExt_DiagnoseUtil::CheckSelfIntersection_ThreadInfo &info)
{
	auto &ltc=*info.ltc;
	for(auto plHd : info.plHdArray)
	{
		if(YSTRUE==ltc.CheckPolygonIntersection(plHd))
		{
			info.itscPlHdArray.Append(plHd);
		}
	}
}

////////////////////////////////////////////////////////////

class YsShellExt_DiagnoseUtil::CheckNonPlanrPolygon_ThreadInfo : public YsTask
{
public:
	YsShellExt_DiagnoseUtil *util;
	YsArray <YsShellPolygonHandle> plHdArray;
	YsArray <YsShellExt_DiagnoseUtil::NonPlanarPolygon> nonPlanarPolygonArray;

protected:
	virtual void Run(void);
};

void YsShellExt_DiagnoseUtil::CheckNonPlanarPolygon(void)
{
	auto &shl=*(this->shl);

	nonPlanarPolygonArray.CleanUp();

	YsArray <CheckNonPlanrPolygon_ThreadInfo> info(nThread,NULL);
	for(YSSIZE_T threadId=0; threadId<nThread; ++threadId)
	{
		info[threadId].util=this;
	}

	{
		int threadId=0;
		for(auto plHd : shl.AllPolygon())
		{
			info[threadId].plHdArray.Append(plHd);
			threadId=(threadId+1)%nThread;
		}
	}

	YsRunTaskParallelDirect(info);

	for(YSSIZE_T threadId=0; threadId<nThread; ++threadId)
	{
		nonPlanarPolygonArray.Append(info[threadId].nonPlanarPolygonArray);
	}
}

void YsShellExt_DiagnoseUtil::CheckNonPlanrPolygon_ThreadInfo::Run(void)
{
	util->CheckNonPlanrPolygon_Thread(*this);
}

void YsShellExt_DiagnoseUtil::CheckNonPlanrPolygon_Thread(YsShellExt_DiagnoseUtil::CheckNonPlanrPolygon_ThreadInfo &info)
{
	for(auto plHd : info.plHdArray)
	{
		YsArray <YsVec3,4> plVtPos;
		shl->GetPolygon(plVtPos,plHd);
		if(3<plVtPos.GetN())
		{
			YsPlane pln;
			pln.MakeBestFitPlane(plVtPos);

			for(auto &pos : plVtPos)
			{
				if(YsTolerance<pln.GetDistance(pos))
				{
					// 2015/10/01 Missing .info added.  Why was this program working?
					info.nonPlanarPolygonArray.Increment();
					info.nonPlanarPolygonArray.Last().plHd=plHd;
					break;
				}
			}
		}
	}
}
