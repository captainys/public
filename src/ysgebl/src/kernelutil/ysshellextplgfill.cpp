/* ////////////////////////////////////////////////////////////

File Name: ysshellextplgfill.cpp
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

#include "ysshellextplgfill.h"
#include <ysshellext.h>


YSRESULT YsShellExtPolygonFloodFill::FloodFillSameColor(const YsShellExt &shl,YsShellPolygonHandle seedPlHd)
{
	plHdArray.CleanUp();

	YsShellPolygonStore visited((const YsShell &)shl);

	plHdArray.Append(seedPlHd);
	visited.AddPolygon(seedPlHd);

	YsColor seedCol;
	shl.GetColorOfPolygon(seedCol,seedPlHd);

	for(YSSIZE_T idx=0; idx<plHdArray.GetN(); ++idx)
	{
		const int nPlVt=shl.GetNumVertexOfPolygon(plHdArray[idx]);
		for(int neiIdx=0; neiIdx<nPlVt; ++neiIdx)
		{
			YsShellPolygonHandle neiPlHd=shl.GetNeighborPolygon(plHdArray[idx],neiIdx);
			if(NULL!=neiPlHd && YSTRUE!=visited.IsIncluded(neiPlHd))
			{
				visited.AddPolygon(neiPlHd);

				YsColor neiCol;
				shl.GetColorOfPolygon(neiCol,neiPlHd);
				if(neiCol==seedCol)
				{
					plHdArray.Append(neiPlHd);
				}
			}
		}
	}

	return YSOK;
}

YSRESULT YsShellExtPolygonFloodFill::FloodFillBoundedByConstEdge(const class YsShellExt &shl,YsShellPolygonHandle seedPlHd)
{
	plHdArray.CleanUp();

	YsShellPolygonStore visited((const YsShell &)shl);

	plHdArray.Append(seedPlHd);
	visited.AddPolygon(seedPlHd);

	for(YSSIZE_T idx=0; idx<plHdArray.GetN(); ++idx)
	{
		const int nPlVt=shl.GetNumVertexOfPolygon(plHdArray[idx]);
		for(int neiIdx=0; neiIdx<nPlVt; ++neiIdx)
		{
			YsShellVertexHandle edVtHd[2];
			shl.GetPolygonEdgeVertex(edVtHd,plHdArray[idx],neiIdx);
			if(0==shl.GetNumConstEdgeUsingEdgePiece(edVtHd))
			{
				YsShellPolygonHandle neiPlHd=shl.GetNeighborPolygon(plHdArray[idx],neiIdx);
				if(NULL!=neiPlHd && YSTRUE!=visited.IsIncluded(neiPlHd))
				{
					visited.AddPolygon(neiPlHd);
					plHdArray.Append(neiPlHd);
				}
			}
		}
	}

	return YSOK;
}

void YsShellExtPolygonFloodFill::Get(YSSIZE_T &nPl,const YsShellPolygonHandle *&plHdPtr) const
{
	nPl=plHdArray.GetN();
	plHdPtr=plHdArray;
}

