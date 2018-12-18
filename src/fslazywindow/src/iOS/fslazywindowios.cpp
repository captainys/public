/* ////////////////////////////////////////////////////////////

File Name: fslazywindowios.cpp
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

#include <fssimplewindow.h>
#include "fslazywindowios.h"


static decltype(FsSubSecondTimer()) prevT=0;


void FsIOSBeforeEverything(void)
{
	char progStr[]="program.exe";
	char *dummyArgs[]=
	{
		progStr
	};
	FsLazyWindowApplicationBase::GetApplication()->BeforeEverything(1,dummyArgs);
}


void FsIOSGetWindowSizeAndPreference(int *x0,int *y0,int *wid,int *hei)
{
    FsOpenWindowOption opt;
    FsLazyWindowApplicationBase::GetApplication()->GetOpenWindowOption(opt);
    *x0=opt.x0;
    *y0=opt.y0;
    *wid=opt.wid;
    *hei=opt.hei;
}


void FsIOSInitialize(void)
{
	char progStr[]="program.exe";
	char *dummyArgs[]=
	{
		progStr
	};
	FsLazyWindowApplicationBase::GetApplication()->Initialize(1,dummyArgs);
}


void FsIOSCallInterval(void)
{
	static bool busy=false;  // Prevent re-entrance
	if(true==busy)
	{
		return;
	}
	busy=true;

	FsLazyWindowApplicationBase::GetApplication()->Interval();

	auto t=FsSubSecondTimer();
	auto passed=t-prevT;

	auto sleepMS=FsLazyWindowApplicationBase::GetApplication()->GetMinimumSleepPerInterval();
	if(sleepMS>passed)
	{
		FsSleep((int)sleepMS-(int)passed);
	}

	prevT=t;
	busy=false;
}
void FsIOSDraw(void)
{
	FsLazyWindowApplicationBase::GetApplication()->Draw();
}


int FsIOSNeedRedraw(void)
{
	if(0!=FsCheckWindowExposure() ||
	   true==FsLazyWindowApplicationBase::GetApplication()->NeedRedraw())
	{
		return 1;
	}
	return 0;
}


void FsIOSBeforeTerminate(void)
{
	FsLazyWindowApplicationBase::GetApplication()->BeforeTerminate();
}

int FsIOSStayIdleUntilEvent(void)
{
	if(true==FsLazyWindowApplicationBase::GetApplication()->StayIdleUntilEvent())
	{
		return 1;
	}
	return 0;
}



