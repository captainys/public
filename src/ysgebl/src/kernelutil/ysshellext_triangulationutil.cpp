/* ////////////////////////////////////////////////////////////

File Name: ysshellext_triangulationutil.cpp
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

#include "ysshellext_triangulationutil.h"


class YsShellExt_TriangulationInfo::ThreadInfo
{
public:
	const YsShellExt *shlPtr;
	YsArray <YsShellPolygonHandle> plHdToDo;

	YsArray <YsShellExt_TriangulationInfo::Triangulation> tri;
	YsArray <YsShellPolygonHandle> failedPlHd;
};


/* static */ void YsShellExt_TriangulationInfo::ThreadEntry(ThreadInfo *infoPtr)
{
	auto &info=*infoPtr;
	auto &shl=*info.shlPtr;

	for(auto plHd : info.plHdToDo)
	{
		auto triVtHd=YsShellExt_TriangulatePolygon(shl.Conv(),plHd);
		if(0<triVtHd.GetN())
		{
			info.tri.Increment();
			info.tri.Last().plHd=plHd;
			info.tri.Last().triVtHd=triVtHd;
		}
		else
		{
			info.failedPlHd.Append(plHd);
		}
	}
}
void YsShellExt_TriangulationInfo::CleanUp(void)
{
	tri.CleanUp();
	failedPlHd.CleanUp();
}
void YsShellExt_TriangulationInfo::MakeInfo(const YsShellExt &shl,int nThread)
{
	if(1>nThread)
	{
		nThread=1;
	}

	YsArray <ThreadInfo> info(nThread,NULL);
	for(auto &i : info)
	{
		i.shlPtr=&shl;
	}


	YsArray <YsShellPolygonHandle> plHdToDo;
	YsArray <int> nVtPl;


	for(auto plHd : shl.AllPolygon())
	{
		auto n=shl.GetPolygonNumVertex(plHd);
		if(3<n)
		{
			plHdToDo.Append(plHd);
			nVtPl.Append(n);
		}
	}
	YsSimpleMergeSort<int,YsShellPolygonHandle>(nVtPl.GetN(),nVtPl,plHdToDo);
	for(YSSIZE_T idx=plHdToDo.GetN()-1; 0<=idx; --idx)
	{
		info[idx%nThread].plHdToDo.Add(plHdToDo[idx]);
	}


	YsArray <std::thread> threadArray;
	for(auto &i : info)
	{
		threadArray.Add(std::thread(ThreadEntry,&i));
	}
	for(auto &t : threadArray)
	{
		t.join();
	}

	CleanUp();
	for(auto &i : info)
	{
		if(0==tri.GetN())
		{
			tri.MoveFrom(i.tri);
		}
		else
		{
			tri.Add(i.tri);
		}
		if(0==this->failedPlHd.GetN())
		{
			failedPlHd.MoveFrom(i.failedPlHd);
		}
		else
		{
			failedPlHd.Add(i.failedPlHd);
		}
	}
}
