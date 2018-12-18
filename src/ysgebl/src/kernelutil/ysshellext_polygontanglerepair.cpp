/* ////////////////////////////////////////////////////////////

File Name: ysshellext_polygontanglerepair.cpp
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

#include "ysshellext_polygontanglerepair.h"
#include "ysshellext_geomcalc.h"
#include "ysshellext_projectionutil.h"

void YsShellExt_PolygonTangleRepair::CleanUp(void)
{
	itscEdge.CleanUp();
	withinSameFaceGroupOnly=YSTRUE;
}

YSBOOL YsShellExt_PolygonTangleRepair::CheckTangledPolygon(const YsShell &shl,YsShellPolygonHandle plHd)
{
	YsShellExt_PolygonProjection prjPlg(shl,plHd);

	int nPlVt;
	const YsShellVertexHandle *plVtHd;
	shl.GetPolygon(nPlVt,plVtHd,plHd);
	return CheckTangledPolygonOnProjection(shl,nPlVt,plVtHd,prjPlg);
}

//YSBOOL YsShellExt_PolygonTangleRepair::CheckTangledPolygon(const YsShell &shl,YSSIZE_T nVt,const YsShellVertexHandle plVtHd[])
//{
//	
//	return YSFALSE;
//}

YSBOOL YsShellExt_PolygonTangleRepair::CheckTangledPolygonOnProjection(const YsShell &shl,YSSIZE_T nVt,const YsShellVertexHandle plVtHd[],class YsShellExt_PolygonProjection &prj)
{
	itscEdge.CleanUp();
	for(YSSIZE_T edIdx0=0; edIdx0<nVt; ++edIdx0)
	{
		YsShellVertexHandle edVtHd[2][2]=
		{
			{plVtHd[edIdx0],plVtHd[(edIdx0+1)%nVt]},
			{NULL,NULL}
		};
		auto &prjVtPos=prj.GetProjectedPolygon();
		YsVec2 edVtPos[2][2]=
		{
			{prjVtPos[edIdx0],prjVtPos.GetCyclic(edIdx0+1)},
			{YsVec2(0.0,0.0),YsVec2(0.0,0.0)}
		};
		if(edVtHd[0][0]!=edVtHd[0][1])
		{
			for(YSSIZE_T edIdx1=edIdx0+1; edIdx1<nVt; ++edIdx1)
			{
				edVtHd[1][0]=plVtHd[edIdx1];
				edVtHd[1][1]=plVtHd[(edIdx1+1)%nVt];
				if(YSTRUE!=YsEdgeSharingNode(edVtHd[0],edVtHd[1]))
				{
					edVtPos[1][0]=prjVtPos[edIdx1];
					edVtPos[1][1]=prjVtPos.GetCyclic(edIdx1+1);

					YsVec2 crs;
					if(YSOK==YsGetLineIntersection2(crs,edVtPos[0],edVtPos[1]) &&
					   YSTRUE==YsCheckInBetween2(crs,edVtPos[0]) &&
					   YSTRUE==YsCheckInBetween2(crs,edVtPos[1]))
					{
						YsShell::Edge edge[2];
						edge[0].edVtHd[0]=edVtHd[0][0];
						edge[0].edVtHd[1]=edVtHd[0][1];
						edge[1].edVtHd[0]=edVtHd[1][0];
						edge[1].edVtHd[1]=edVtHd[1][1];
						itscEdge.Append(2,edge);
					}
				}
			}
		}
	}

	if(0<itscEdge.GetN())
	{
		return YSTRUE;
	}
	return YSFALSE;
}
