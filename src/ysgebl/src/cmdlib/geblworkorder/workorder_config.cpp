/* ////////////////////////////////////////////////////////////

File Name: workorder_config.cpp
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




YSRESULT GeblCmd_WorkOrder::RunConfigWorkOrder(const YsString &workOrder,const YsArray <YsString,16> &args)
{
	if(2<=args.GetN())
	{
		if(0==args[1].STRCMP("TOLERANCE"))
		{
			return RunConfigTolerance(workOrder,args);
		}
		else if(0==args[1].STRCMP("COLORTOLERANCE"))
		{
			return RunConfigColorTolerance(workOrder,args);
		}
		else if(0==args[1].STRCMP("NUMTHREAD"))
		{
			return RunConfigNumThread(workOrder,args);
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

YSRESULT GeblCmd_WorkOrder::RunConfigTolerance(const YsString &workOrder,const YsArray <YsString,16> &args)
{
	if(3<=args.GetN())
	{
		YsString param=args[2];
		param.Capitalize();
		if(0==param.Strcmp("AUTO"))
		{
			auto &shl=*(slHd);
			YsVec3 bbx[2];
			shl.GetBoundingBox(bbx);
			YsSetToleranceFromDimension((bbx[1]-bbx[0]).GetLength());
		}
		else
		{
			YsTolerance=atof(param);
		}
		return YSOK;
	}
	return YSERR;
}

YSRESULT GeblCmd_WorkOrder::RunConfigColorTolerance(const YsString &workOrder,const YsArray <YsString,16> &args)
{
	if(3<=args.GetN())
	{
		cmdSupport.SetColorTolerance(atoi(args[2]));
		return YSOK;
	}
	return YSERR;
}

YSRESULT GeblCmd_WorkOrder::RunConfigNumThread(const YsString &workOrder,const YsArray <YsString,16> &args)
{
	if(3<=args.GetN())
	{
		cmdSupport.SetNumThread(atoi(args[2]));
		return YSOK;
	}
	return YSERR;
}
