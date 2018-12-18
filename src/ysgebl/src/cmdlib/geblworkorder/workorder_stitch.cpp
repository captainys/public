/* ////////////////////////////////////////////////////////////

File Name: workorder_stitch.cpp
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

#include "geblworkorder.h"
#include <ysshellext_diagnoseutil.h>
#include <ysshellextedit_stitchingutil.h>



YSRESULT GeblCmd_WorkOrder::RunStitchWorkOrder(const YsString &workOrder,const YsArray <YsString,16> &args)
{
	if(2<=args.GetN())
	{
		auto subCmd=args[1];
		subCmd.Capitalize();
		if(0==strcmp("FREE",subCmd))
		{
			return RunStitchFreeStitching(workOrder,args);
		}
		if(0==subCmd.STRCMP("LOOP"))
		{
			return RunStitchLoopStitching(workOrder,args);
		}

		YsString errorReason;
		errorReason.Printf("Unrecognized sub command [%s]",args[1].Txt());
		ShowError(workOrder,errorReason);
	}
	else
	{
		ShowError(workOrder,"Sub-command not given.");
	}
	return YSERR;
}

YSRESULT GeblCmd_WorkOrder::RunStitchFreeStitching(const YsString &workOrder,const YsArray <YsString,16> &args)
{
	YsShellExt_DiagnoseUtil diagno;
	diagno.SetNumThread(8);
	diagno.SetShell(slHd->Conv());

	diagno.CheckNonManifoldEdge();
	auto nNonManifold=diagno.GetNumNonManifoldEdge();

	while(0<nNonManifold)
	{
		YsShellExtEdit_FreeStitching(*slHd);

		diagno.CheckNonManifoldEdge();
		auto nNonManifoldAfter=diagno.GetNumNonManifoldEdge();
		if(nNonManifold<=nNonManifoldAfter)
		{
			break;
		}
		nNonManifold=nNonManifoldAfter;
	}
	return YSOK;
}

YSRESULT GeblCmd_WorkOrder::RunStitchLoopStitching(const YsString &workOrder,const YsArray <YsString,16> &args)
{
	YsShellExt_DiagnoseUtil diagno;
	diagno.SetNumThread(8);
	diagno.SetShell(slHd->Conv());

	diagno.CheckNonManifoldEdge();
	auto nNonManifold=diagno.GetNumNonManifoldEdge();

	while(0<nNonManifold)
	{
		YsShellExtEdit_LoopStitching(*slHd);

		diagno.CheckNonManifoldEdge();
		auto nNonManifoldAfter=diagno.GetNumNonManifoldEdge();
		if(nNonManifold<=nNonManifoldAfter)
		{
			break;
		}
		nNonManifold=nNonManifoldAfter;
	}
	return YSOK;
}
