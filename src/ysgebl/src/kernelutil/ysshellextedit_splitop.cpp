/* ////////////////////////////////////////////////////////////

File Name: ysshellextedit_splitop.cpp
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

#include "ysshellextedit_splitop.h"
#include "ysshellextedit_localop.h"
#include "ysshellextedit_copyutil.h"

YSRESULT YsShellExtEdit_CutByPlane::CutByPolygon(YsShellExtEdit &shl,YSSIZE_T np,const YsVec3 p[])
{
	if(YSOK==StartByPolygon((const YsShellExt &)shl,np,p))
	{
		YsShellExtEdit::StopIncUndo stopIncUndo(shl);

		for(YSSIZE_T spIdx=0; spIdx<edgeSplit.GetN(); ++spIdx)
		{
			edgeSplit[spIdx].createdVtHd=shl.AddVertex(edgeSplit[spIdx].pos);
			YsShellExtEdit_InsertVertexOnEdge(shl,edgeSplit[spIdx].edVtHd,edgeSplit[spIdx].createdVtHd);
		}

		if(YSOK==MakeSplitInfoAfterEdgeVertexGeneration((const YsShellExt &)shl))
		{
			for(YSSIZE_T plIdx=0; plIdx<newPlg.GetN(); ++plIdx)
			{
				for(YSSIZE_T divIdx=0; divIdx<newPlg[plIdx].plVtHdArray.GetN(); ++divIdx)
				{
					if(0==divIdx)
					{
						shl.SetPolygonVertex(newPlg[plIdx].plHd,newPlg[plIdx].plVtHdArray[divIdx]);
					}
					else
					{
						YsShellPolygonHandle newPlHd=shl.AddPolygon(newPlg[plIdx].plVtHdArray[divIdx]);
						YsShellExt_CopyPolygonAttribColorNormalFaceGroup(shl,newPlHd,newPlg[plIdx].plHd);
					}
				}
			}

			for(YSSIZE_T ceIdx=0; ceIdx<newConstEdge.GetN(); ++ceIdx)
			{
				for(YSSIZE_T divIdx=0; divIdx<newConstEdge[ceIdx].ceVtHdArray.GetN(); ++divIdx)
				{
					if(0==divIdx)
					{
						shl.ModifyConstEdge(newConstEdge[ceIdx].ceHd,newConstEdge[ceIdx].ceVtHdArray[divIdx],YSFALSE);
					}
					else
					{
						auto attrib=shl.GetConstEdgeAttrib(newConstEdge[ceIdx].ceHd);
						auto newCeHd=shl.AddConstEdge(newConstEdge[ceIdx].ceVtHdArray[divIdx],YSFALSE);
						shl.SetConstEdgeAttrib(newCeHd,attrib);
					}
				}
			}

			return YSOK;
		}
	}
	return YSERR;
}

YSRESULT YsShellExtEdit_CutByPlane::CutByPolygon(YsShellExtEdit &shl,YSSIZE_T np,const YsShellVertexHandle p[])
{
	YsArray <YsVec3,16> vtPos(np,NULL);
	for(YSSIZE_T idx=0; idx<np; ++idx)
	{
		shl.GetVertexPosition(vtPos[idx],p[idx]);
	}
	return CutByPolygon(shl,vtPos);
}

