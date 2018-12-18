/* ////////////////////////////////////////////////////////////

File Name: ysshellextedit_sewingutil.cpp
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

#include "ysshellextedit_sewingutil.h"
#include "ysshellextedit_topologyutil.h"
#include "ysshellextedit_localop.h"

// Call this function after MakeInfo
YSRESULT YsShellExtEdit_SewingInfo::Apply(YsShellExtEdit &shl)
{
	YsShellExtEdit::StopIncUndo incUndo(shl);

	for(auto &vone : AllVtxOnEdge())
	{
		if(NULL==vone.createdVtHd)
		{
			vone.createdVtHd=shl.AddVertex(vone.pos);
		}
	}
	for(auto &vonp : AllVtxOnPolygon())
	{
		if(NULL==vonp.vtHd)
		{
			vonp.vtHd=shl.AddVertex(vonp.pos);
		}
	}

	MakeEdgeMidVertexInfo(shl.Conv());
	for(auto &vone : AllEdgeMidVertex())
	{
		YsShellExtEdit_TopologyUtil util;
		util.InsertVertexOnEdge(shl,vone.edge[0],vone.edge[1],vone.midVtHd);
	}

	MakePolygonSplitInfo(shl.Conv());
	for(auto &ps : AllPolygonSplit())
	{
		YsShellExtEdit_ApplyPolygonSplit(shl,ps);
	}

	return YSOK;
}
