/* ////////////////////////////////////////////////////////////

File Name: workorder_facegroup.cpp
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

#include <ysshellextedit_facegrouputil.h>
#include "geblworkorder.h"
#include <ysshellextmisc.h>



YSRESULT GeblCmd_WorkOrder::RunFaceGroupWorkOrder(const YsString &workOrder,const YsArray <YsString,16> &args)
{
	if(2<=args.GetN())
	{
		if(0==args[1].STRCMP("ELEMSIZE"))
		{
			return RunFaceGroupAssignElemSize(workOrder,args);
		}
		else if(0==args[1].STRCMP("BLAYER"))
		{
			return RunFaceGroupAssignBoundaryLayerSpecification(workOrder,args);
		}
		else if(0==args[1].STRCMP("CLEARBLAYER"))
		{
			return RunFaceGroupRemoveBoundaryLayerSpecification(workOrder,args);
		}
		else if(0==args[1].STRCMP("CREATE"))
		{
			return RunFaceGroupCreate(workOrder,args);
		}
		else if(0==args[1].STRCMP("MAKENOBUBBLE"))
		{
			return RunFaceGroupMakeNoBubble(workOrder,args);
		}
		else if(0==args[1].STRCMP("EXTRACT"))
		{
			return RunFaceGroupExtract(workOrder,args);
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

YSRESULT GeblCmd_WorkOrder::RunFaceGroupAssignElemSize(const YsString &workOrder,const YsArray <YsString,16> &args)
{
	auto &shl=*slHd;
	YSSIZE_T idx=2;
	while(idx<args.size())
	{
		auto targetFgHd=cmdSupport.GetFaceGroupHandle(shl.Conv(),idx,args);
		if(args.size()<=idx)
		{
			ShowError(workOrder,"Too few arguments.");
			return YSERR;
		}
		if(0<targetFgHd.size())
		{
			for(auto fgHd : targetFgHd)
			{
				auto fgAttrib=shl.GetFaceGroupAttrib(fgHd);
				fgAttrib.SetElemSize(atof(args[idx]));
				shl.SetFaceGroupAttrib(fgHd,fgAttrib);
			}
		}
		else
		{
			ShowError(workOrder,"Specified face Group(s) does not exist.");
			return YSERR;
		}
		++idx;
	}
	return YSOK;
}

YSRESULT GeblCmd_WorkOrder::RunFaceGroupAssignBoundaryLayerSpecification(const YsString &workOrder,const YsArray <YsString,16> &args)
{
	auto &shl=*slHd;
	YSSIZE_T idx=2;
	while(idx<args.size())
	{
		auto targetFgHd=cmdSupport.GetFaceGroupHandle(shl.Conv(),idx,args);
		if(args.size()<=idx)
		{
			ShowError(workOrder,"Too few arguments.");
			return YSERR;
		}
		if(0<targetFgHd.size())
		{
			for(auto fgHd : targetFgHd)
			{
				auto fgAttrib=shl.GetFaceGroupAttrib(fgHd);
				fgAttrib.SetBoundaryLayerSpecification(atoi(args[idx]),atof(args[idx+1]),atoi(args[idx+2]),atof(args[idx+3]));
				shl.SetFaceGroupAttrib(fgHd,fgAttrib);
			}
			idx+=4;
		}
		else
		{
			ShowError(workOrder,"Specified face Group(s) does not exist.");
			return YSERR;
		}
	}
	return YSOK;
}

YSRESULT GeblCmd_WorkOrder::RunFaceGroupRemoveBoundaryLayerSpecification(const YsString &workOrder,const YsArray <YsString,16> &args)
{
	auto &shl=*slHd;
	YSSIZE_T idx=2;
	while(idx<args.size())
	{
		auto targetFgHd=cmdSupport.GetFaceGroupHandle(shl.Conv(),idx,args);
		if(0<targetFgHd.size())
		{
			for(auto fgHd : targetFgHd)
			{
				auto fgAttrib=shl.GetFaceGroupAttrib(fgHd);
				fgAttrib.SetBoundaryLayerSpecification(0,0.0,0,0.0);
				shl.SetFaceGroupAttrib(fgHd,fgAttrib);
			}
		}
		else
		{
			ShowError(workOrder,"Specified face Group(s) does not exist.");
			return YSERR;
		}
	}
	return YSOK;
}

YSRESULT GeblCmd_WorkOrder::RunFaceGroupCreate(const YsString &workOrder,const YsArray <YsString,16> &args)
{
	if(2<=args.GetN() && 0==args[2].STRCMP("FROMCE"))
	{
		YsShellExtEdit_MakeFaceGroupByConstEdge(*slHd);
		return YSOK;
	}
	YsString errorReason;
	errorReason.Printf("Unrecognized sub command parameter [%s]",args[2].Txt());
	ShowError(workOrder,errorReason);
	return YSERR;
}

YSRESULT GeblCmd_WorkOrder::RunFaceGroupMakeNoBubble(const YsString &workOrder,const YsArray <YsString,16> &args)
{
	auto &option=args[2];
	if(0==option.STRCMP("ALL"))
	{
		auto &shl=*slHd;
		for(auto fgHd : shl.AllFaceGroup())
		{
			auto fgAttrib=shl.GetFaceGroupAttrib(fgHd);
			fgAttrib.SetNoBubble(YSTRUE);
			shl.SetFaceGroupAttrib(fgHd,fgAttrib);
		}
		return YSOK;
	}

	YsString errorReason;
	errorReason.Printf("Not supported or not implemented yet. [%s]\n",option.Txt());
	ShowError(workOrder,errorReason);
	return YSERR;
}

YSRESULT GeblCmd_WorkOrder::RunFaceGroupExtract(const YsString &workOrder,const YsArray <YsString,16> &args)
{
	auto &shl=*slHd;
	YsShellExt::FaceGroupStore fgStore(shl.Conv());
	YSSIZE_T idx=2;
	while(idx<args.size())
	{
		auto fgHdArray=cmdSupport.GetFaceGroupHandle(shl.Conv(),idx,args);
		if(0<fgHdArray.GetN())
		{
			fgStore.Add(fgHdArray);
		}
		else
		{
			ShowError(workOrder,"Specified Face Group(s) does not exist.");
			return YSERR;
		}
	}
	for(auto fgHd : shl.AllFaceGroup())
	{
		if(YSTRUE!=fgStore.IsIncluded(fgHd))
		{
			shl.DeleteFaceGroup(fgHd);
		}
	}
	for(auto plHd : shl.AllPolygon())
	{
		auto fgHd=shl.FindFaceGroupFromPolygon(plHd);
		if(nullptr==fgHd)
		{
			shl.DeletePolygon(plHd);
		}
	}
	for(auto ceHd : shl.AllConstEdge())
	{
		auto ceFgHd=shl.FindFaceGroupFromConstEdge(ceHd);
		YSBOOL used=YSFALSE;
		for(auto fgHd : ceFgHd)
		{
			if(YSTRUE==fgStore.IsIncluded(fgHd))
			{
				used=YSTRUE;
				break;
			}
		}
		if(YSTRUE!=used)
		{
			shl.DeleteConstEdge(ceHd);
		}
	}
	for(auto vtHd : shl.AllVertex())
	{
		if(0==shl.FindPolygonFromVertex(vtHd).GetN() &&
		   0==shl.FindConstEdgeFromVertex(vtHd).GetN())
		{
			shl.DeleteVertex(vtHd);
		}
	}
	return YSOK;
}
