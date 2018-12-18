/* ////////////////////////////////////////////////////////////

File Name: ysshellext_smoothingutil.cpp
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

#include "ysshellext_smoothingutil.h"
#include "ysshellext_duplicateutil.h"


YsShellExt_FaceGroupSmoothingUtil::YsShellExt_FaceGroupSmoothingUtil()
{
	canPassFunc=nullptr;
	watchMode=YSFALSE;
}

void YsShellExt_FaceGroupSmoothingUtil::SetCanPassFunc(YsShellCrawler::CanPassFunction *func)
{
	canPassFunc=func;
}

YSRESULT YsShellExt_FaceGroupSmoothingUtil::Begin(const YsShellExt &shl,YsShellExt::FaceGroupHandle fgHd)
{
	originalShl=&shl;
	cloneShl.CleanUp();

	YsShellExt_DuplicateUtil dup;
	dup.SetShell(shl);

	YsShellExt::FaceGroupHandle allFgHd[]=
	{
		fgHd
	};
	dup.AddFaceGroup(1,allFgHd);

	cloneShl.DisableSearch();
	dup.MakeClone(cloneShl);
	cloneShl.EnableSearch();

	crawlState.CleanUp();
	crawlState.SetShell(shl.Conv());

	return YSOK;
}

YSRESULT YsShellExt_FaceGroupSmoothingUtil::Begin(const YsShellExt &shl)
{
	originalShl=&shl;
	cloneShl.CleanUp();

	YsShellExt_DuplicateUtil dup;
	dup.SetShell(shl);

	dup.AddVertex(shl.AllVertex());
	dup.AddPolygon(shl.AllPolygon());
	dup.AddConstEdge(shl.AllConstEdge());
	dup.AddFaceGroup(shl.AllFaceGroup());

	cloneShl.DisableSearch();
	dup.MakeClone(cloneShl);
	cloneShl.EnableSearch();

	crawlState.CleanUp();
	crawlState.SetShell(shl.Conv());

	return YSOK;
}

YsShell::VertexHandle YsShellExt_FaceGroupSmoothingUtil::OriginalVtHdToCloneVtHd(YsShell::VertexHandle orgVtHd) const
{
	return cloneShl.FindVertex(originalShl->GetSearchKey(orgVtHd));
}

YsVec3 YsShellExt_FaceGroupSmoothingUtil::GetVertexPosition(YsShell::VertexHandle orgVtHd) const
{
	if(YSTRUE==IsIncluded(orgVtHd))
	{
		return cloneShl.GetVertexPosition(OriginalVtHdToCloneVtHd(orgVtHd));
	}
	else
	{
		return originalShl->GetVertexPosition(orgVtHd);
	}
}

YsShellExt_FaceGroupSmoothingUtil::VertexState YsShellExt_FaceGroupSmoothingUtil::SaveVertexState(YsShell::VertexHandle vtHd) const
{
	VertexState sta;
	auto statePtr=crawlState[vtHd];
	if(nullptr!=statePtr)
	{
		sta.hasCrawlingState=YSTRUE;
		sta.pos=GetVertexPosition(vtHd);
		sta.sta=*statePtr;
	}
	else
	{
		sta.hasCrawlingState=YSFALSE;
		sta.pos=GetVertexPosition(vtHd);
	}

	return sta;
}

void YsShellExt_FaceGroupSmoothingUtil::RestoreVertexState(YsShell::VertexHandle vtHd,const VertexState &sta)
{
	if(YSTRUE==IsIncluded(vtHd))
	{
		cloneShl.SetVertexPosition(OriginalVtHdToCloneVtHd(vtHd),sta.pos);
	}
	if(YSTRUE==sta.hasCrawlingState)
	{
		crawlState.SetAttrib(vtHd,sta.sta);
	}
	else
	{
		if(nullptr!=crawlState[vtHd])
		{
			crawlState.DeleteAttrib(vtHd);
		}
	}
}

YSRESULT YsShellExt_FaceGroupSmoothingUtil::SetVertexPosition(YsShell::VertexHandle orgVtHd,const YsVec3 &Pos)
{
	auto cloneVtHd=OriginalVtHdToCloneVtHd(orgVtHd);
	if(nullptr!=cloneVtHd)
	{
		YSBOOL watch=this->watchMode;

		YsShellCrawler crawler;

		crawler.SetMode(YsShellCrawler::CRAWL_TO_GOAL);
		crawler.SetGoal(Pos);
		crawler.SetCanPassFunc(canPassFunc);

		auto crawlerStatePtr=crawlState[orgVtHd];
		YSRESULT res=YSERR;
		if(nullptr==crawlerStatePtr)
		{
			if(YSTRUE==this->watchMode)
			{
				printf("%s %d\n",__FUNCTION__,__LINE__);
			}

			auto dir=YsUnitVector(Pos-cloneShl.GetVertexPosition(cloneVtHd));

			YSBOOL foundOne=YSFALSE;
			int foundLevel=0; // 0: Not found  1: Found but low priority  2: Found
			auto closest=originalShl->GetVertexPosition(orgVtHd);
			auto closeDist=(closest-Pos).GetSquareLength();
			decltype(crawler.GetCurrentState()) closeState;

			// 2016/08/17
			//   There can be multiple paths starting from a vertex, especially a vertex on the high-dihedral-angle corner.
			//   All possible paths must be explored (, at least for the first iteration).
			for(auto plHdAndIndex : crawler.PossiblePolygonToGoIn(originalShl->Conv(),originalShl->GetVertexPosition(orgVtHd),dir,orgVtHd))
			{
				if(YSOK==crawler.Start(originalShl->Conv(),originalShl->GetVertexPosition(orgVtHd),dir,plHdAndIndex.plHd))
				{
					crawler.Crawl(originalShl->Conv(),YsInfinity,watch);
					auto state=crawler.GetCurrentState();

					auto d=(state.pos-Pos).GetSquareLength();

					int thisLevel=2;
					if(nullptr!=canPassFunc && YSTRUE==canPassFunc->IsLowPriorityPolygon(originalShl->Conv(),plHdAndIndex.plHd))
					{
						thisLevel=1;
					}

					if(YSTRUE!=foundOne || foundLevel<thisLevel || (foundLevel==thisLevel && d<closeDist))
					{
						foundOne=YSTRUE;
						foundLevel=thisLevel;
						closeState=state;
						closeDist=d;
					}
				}
			}

			if(YSTRUE==foundOne)
			{
				cloneShl.SetVertexPosition(cloneVtHd,closeState.pos);
				crawlState.SetAttrib(orgVtHd,closeState);
			}
		}
		else
		{
			if(YSTRUE==this->watchMode)
			{
				printf("%s %d\n",__FUNCTION__,__LINE__);
			}

			auto start=*crawlerStatePtr;
			start.dir=YsUnitVector(Pos-start.pos);
			if(YSOK==crawler.Restart(originalShl->Conv(),start,this->watchMode))
			{
				crawler.Crawl(originalShl->Conv(),YsInfinity,watch);
				if(YSTRUE==this->watchMode)
				{
					printf("%s %d\n",__FUNCTION__,__LINE__);
					if(YSTRUE==crawler.reachedDeadEnd)
					{
						printf("Dead end.\n");
					}
				}

				auto state=crawler.GetCurrentState();
				cloneShl.SetVertexPosition(cloneVtHd,state.pos);

				crawlState.SetAttrib(orgVtHd,state);
			}
		}

		return YSOK;
	}
	return YSERR;
}

YSRESULT YsShellExt_FaceGroupSmoothingUtil::ForceSetVertexPosition(YsShell::VertexHandle VtHd,const YsVec3 &Pos)
{
	auto cloneVtHd=OriginalVtHdToCloneVtHd(VtHd);
	if(nullptr!=cloneVtHd)
	{
		SetVertexPosition(VtHd,Pos);
		cloneShl.SetVertexPosition(cloneVtHd,Pos);
		return YSOK;
	}
	return YSERR;
}

void YsShellExt_FaceGroupSmoothingUtil::SetWatchMode(YSBOOL watch)
{
	watchMode=watch;
}

YSBOOL YsShellExt_FaceGroupSmoothingUtil::IsIncluded(YsShell::VertexHandle VtHd) const
{
	auto vtKey=originalShl->GetSearchKey(VtHd);
	if(nullptr!=cloneShl.FindVertex(vtKey))
	{
		return YSTRUE;
	}
	return YSFALSE;
}

YsArray <YsShell::VertexHandle> YsShellExt_FaceGroupSmoothingUtil::AllVertex(void) const
{
	YsArray <YsShell::VertexHandle> vtHd;
	for(auto cloneVtHd : cloneShl.AllVertex())
	{
		auto vtKey=cloneShl.GetSearchKey(cloneVtHd);
		auto orgVtHd=originalShl->FindVertex(vtKey);
		if(nullptr!=orgVtHd)
		{
			vtHd.Add(orgVtHd);
		}
	}
	return vtHd;
}


////////////////////////////////////////////////////////////



YsResultAndVec3 YsShellExt_LaplacianSmoothing(const YsShellExt &Shl,const YsShellVertexPositionSource &VtPosSrc,YsShell::VertexHandle vtHd)
{
	auto sum=YsOrigin();

	int n=0;
	for(auto connVtHd : Shl.GetConnectedVertex(vtHd))
	{
		sum+=VtPosSrc.GetVertexPosition(connVtHd);
		++n;
	}

	YsResultAndVec3	res;
	if(0<n)
	{
		res.pos=sum/(double)n;
		res.res=YSOK; 
		return res;
	}
	res.pos=YsOrigin();
	res.res=YSERR;
	return res;
}

