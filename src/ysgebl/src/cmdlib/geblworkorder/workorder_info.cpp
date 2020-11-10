/* ////////////////////////////////////////////////////////////

File Name: workorder_fileio.cpp
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
#include "../geblfileio/fileio.h"
#include <ysport.h>
#include <ysshellextio.h>
#include <ysshellext_curvatureutil.h>
#include <ysshellext_geomcalc.h>

YSRESULT GeblCmd_WorkOrder::RunInfoWorkOrder(const YsString &workOrder,const YsArray <YsString,16> &args)
{
	if(1<=args.size())
	{
		if(0==args[1].STRCMP("VOLUME"))
		{
			return RunInfoWorkOrder_ComputeVolume(workOrder,args);
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
YSRESULT GeblCmd_WorkOrder::RunInfoWorkOrder_ComputeVolume(const YsString &workOrder,const YsArray <YsString,16> &args)
{
	if(3<=args.GetN())
	{
		auto &shl=*slHd;
		auto volume=shl.ComputeVolume();
		FILE *fp=fopen(args[2],"w");
		if(nullptr!=fp)
		{
			fprintf(fp,"%.20f\n",volume);
			fclose(fp);
			return YSOK;
		}
		else
		{
			ShowError(workOrder,"Cannot open file.");
			return YSERR;
		}
	}
	else
	{
		ShowError(workOrder,"Too few arguments.");
		return YSERR;
	}
}
