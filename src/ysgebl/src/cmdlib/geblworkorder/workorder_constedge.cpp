/* ////////////////////////////////////////////////////////////

File Name: workorder_constedge.cpp
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
#include <ysshellextedit_constedgeutil.h>
#include <ysshellext_geomcalc.h>



YSRESULT GeblCmd_WorkOrder::RunConstEdgeWorkOrder(const YsString &workOrder,const YsArray <YsString,16> &args)
{
	if(2<=args.GetN())
	{
		YsString subCmd=args[1];
		subCmd.Capitalize();
		if(0==args[1].STRCMP("CLEAR"))
		{
			return RunConstEdgeClear(workOrder,args);
		}
		else if(0==args[1].STRCMP("ADD"))
		{
			return RunConstEdgeAdd(workOrder,args);
		}
		else if(0==args[1].STRCMP("DELETE"))
		{
			return RunConstEdgeDelete(workOrder,args);
		}
		else if(0==args[1].STRCMP("RECONSIDER"))
		{
			return RunConstEdgeReconsider(workOrder,args);
		}
		else if(0==args[1].STRCMP("MAKENOBUBBLE"))
		{
			return RunConstEdgeMakeNoBubble(workOrder,args);
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

YSRESULT GeblCmd_WorkOrder::RunConstEdgeClear(const YsString &,const YsArray <YsString,16> &)
{
	auto &shl=*slHd;
	for(auto ceHd : shl.AllConstEdge())
	{
		shl.DeleteConstEdge(ceHd);
	}
	return YSOK;
}

YSRESULT GeblCmd_WorkOrder::RunConstEdgeAdd(const YsString &workOrder,const YsArray <YsString,16> &args)
{
	auto &option=args[2];
	if(0==option.STRCMP("FGBOUNDARY"))
	{
		return YsShellExtEdit_AddConstEdgeAlongFaceGroupBoundary(*slHd);
	}
	else if(0==option.STRCMP("DHA"))
	{
		if(4<=args.GetN())
		{
			const double angle=YsDegToRad(atof(args[3]));
			return YsShellExtEdit_AddConstEdgeByHighDihedralAngleThreshold(*slHd,angle);
		}
		else
		{
			ShowError(workOrder,"Too few arguments.");
			return YSERR;
		}
	}
	else if(0==option.STRCMP("NONMANIFOLD"))
	{
		return YsShellExtEdit_AddConstEdgeAlongNonManifoldEdge(*slHd);
	}
	else if(0==option.STRCMP("ALL"))
	{
		auto &shl=*slHd;
		YsShellEdgeEnumHandle edHd=nullptr;
		while(YSOK==shl.MoveToNextEdge(edHd))
		{
			auto edge=shl.GetEdge(edHd);
			if(YSTRUE!=shl.IsEdgePieceConstrained(edge))
			{
				shl.AddConstEdge(2,edge,YSFALSE);
			}
		}
		return YSOK;
	}

	YsString errorReason;
	errorReason.Printf("Not supported or not implemented yet. [%s]\n",option.Txt());
	ShowError(workOrder,errorReason);
	return YSERR;
}

YSRESULT GeblCmd_WorkOrder::RunConstEdgeDelete(const YsString &workOrder,const YsArray <YsString,16> &args)
{
	auto &option=args[2];
	if(0==option.STRCMP("SHORT"))
	{
		if(4<=args.size())
		{
			auto thr=atof(args[3]);
			auto &shl=*slHd;
			for(auto ceHd : shl.AllConstEdge())
			{
				if(thr>YsShellExt_CalculateConstEdgeLength(shl.Conv(),ceHd))
				{
					shl.DeleteConstEdge(ceHd);
				}
			}
			return YSOK;
		}
		else
		{
			ShowError(workOrder,"Too few arguments.");
			return YSERR;
		}
	}
	else if(0==option.STRCMP("SHORT_ISOLATED"))
	{
		if(4<=args.size())
		{
			auto thr=atof(args[3]);
			auto &shl=*slHd;
			for(auto ceHd : shl.AllConstEdge())
			{
				int nCeUseCount=0;
				for(auto vtHd : shl.GetConstEdgeVertex(ceHd))
				{
					YsMakeGreater(nCeUseCount,shl.GetNumConstEdgeUsingVertex(vtHd));
					if(2<=nCeUseCount)
					{
						break;
					}
				}
				if(1==nCeUseCount)
				{
					if(thr>YsShellExt_CalculateConstEdgeLength(shl.Conv(),ceHd))
					{
						shl.DeleteConstEdge(ceHd);
					}
				}
			}
			return YSOK;
		}
		else
		{
			ShowError(workOrder,"Too few arguments.");
			return YSERR;
		}
	}
	else if(0==option.STRCMP("DHAHIGH"))
	{
		if(4<=args.size())
		{
			auto &shl=*slHd;
			const double dhaThr=YsDegToRad(atof(args[3]));
			for(auto ceHd : shl.AllConstEdge())
			{
				int nCeUseCount=0;
				YsArray <YsShell::VertexHandle> ceVtHd=shl.GetConstEdgeVertex(ceHd);
				if(YSTRUE==shl.GetConstEdgeIsLoop(ceHd))
				{
					ceVtHd.push_back(ceVtHd[0]);
				}
				for(YSSIZE_T idx=0; idx<ceVtHd.size()-1; ++idx)
				{
					if(2==shl.GetNumPolygonUsingEdge(ceVtHd[idx],ceVtHd[idx+1]) &&
					   dhaThr<YsShellExt_CalculateDihedralAngle(shl.Conv(),ceVtHd[idx],ceVtHd[idx+1]))
					{
						shl.DeleteConstEdge(ceHd);
						break;
					}
				}
			}
			return YSOK;
		}
		else
		{
			ShowError(workOrder,"Too few arguments.");
			return YSERR;
		}
	}
	YsString errorReason;
	errorReason.Printf("Not supported or not implemented yet. [%s]\n",option.Txt());
	ShowError(workOrder,errorReason);
	return YSERR;
}

YSRESULT GeblCmd_WorkOrder::RunConstEdgeReconsider(const YsString &workOrder,const YsArray <YsString,16> &args)
{
	YsShellExtEdit_ReconsiderConstEdge(*slHd);
	return YSOK;
}

YSRESULT GeblCmd_WorkOrder::RunConstEdgeMakeNoBubble(const YsString &workOrder,const YsArray <YsString,16> &args)
{
	auto &option=args[2];
	if(0==option.STRCMP("ALL"))
	{
		auto &shl=*slHd;
		for(auto ceHd : shl.AllConstEdge())
		{
			auto ceAttrib=shl.GetConstEdgeAttrib(ceHd);
			ceAttrib.SetNoBubble(YSTRUE);
			shl.SetConstEdgeAttrib(ceHd,ceAttrib);
		}
		return YSOK;
	}
	YsString errorReason;
	errorReason.Printf("Not supported or not implemented yet. [%s]\n",option.Txt());
	ShowError(workOrder,errorReason);
	return YSERR;
}
