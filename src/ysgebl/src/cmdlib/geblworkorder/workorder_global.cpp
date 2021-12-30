/* ////////////////////////////////////////////////////////////

File Name: workorder_global.cpp
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

#include <ysshellextedit.h>

#include "geblworkorder.h"

YSRESULT GeblCmd_WorkOrder::RunGlobalWorkOrder(const YsString &workOrder,const YsArray <YsString,16> &args)
{
	if(2<=args.GetN())
	{
		auto subCmd=args[1];
		subCmd.Capitalize();
		if(0==subCmd.STRCMP("DELETE_UNUSED_VERTEX"))
		{
			return RunGlobalWorkOrder_DeleteUnusedVertex(workOrder,args);
		}
		if(0==subCmd.STRCMP("SCALE"))
		{
			return RunGlobalWorkOrder_Scaling(workOrder,args);
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

YSRESULT GeblCmd_WorkOrder::RunGlobalWorkOrder_DeleteUnusedVertex(const YsString &workOrder,const YsArray <YsString,16> &args)
{
	auto &shl=*slHd;
	YsArray <YsShell::VertexHandle> toDel;
	for(auto vtHd : shl.AllVertex())
	{
		if(0==shl.GetNumPolygonUsingVertex(vtHd) &&
		   0==shl.GetNumConstEdgeUsingVertex(vtHd))
		{
			toDel.Add(vtHd);
		}
	}
	shl.DeleteMultiVertex(toDel);
	return YSOK;
}

YSRESULT GeblCmd_WorkOrder::RunGlobalWorkOrder_Scaling(const YsString &workOrder,const YsArray <YsString,16> &args)
{
	double sx=1.0,sy=1.0,sz=1.0;
	if(5<=args.size())
	{
		sx=args[2].Atof();
		sy=args[3].Atof();
		sz=args[4].Atof();
	}
	else if(3<=args.size())
	{
		sx=args[2].Atof();
		sy=args[2].Atof();
		sz=args[2].Atof();
	}
	else
	{
		ShowError(workOrder,"Too few arguments.");
		return YSERR;
	}

	auto &shl=*slHd;
	YsArray <YsShell::VertexHandle> vtHd;
	YsArray <YsVec3> newPos;
	for(auto hd : shl.AllVertex())
	{
		auto pos=shl.GetVertexPosition(hd);
		pos.MulX(sx);
		pos.MulY(sy);
		pos.MulZ(sz);
		vtHd.push_back(hd);
		newPos.push_back(pos);
	}
	shl.SetMultiVertexPosition(vtHd,newPos);
	return YSOK;
}
