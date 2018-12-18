/* ////////////////////////////////////////////////////////////

File Name: ysshellextedit_selectionutil.h
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

#ifndef YSSHELLEXTEDIT_SELECTIONUTIL_IS_INCLUDED
#define YSSHELLEXTEDIT_SELECTIONUTIL_IS_INCLUDED
/* { */

#include <ysshellext.h>
#include <ysshellextedit.h>

template <const int N>
YSRESULT YsShellExtEdit_GetAllVertexInSelection(YsArray <YsShellVertexHandle,N> &allSelVtHd,const YsShellExtEdit &shl)
{
	YsShellVertexStore visited((const YsShell &)shl);

	allSelVtHd.CleanUp();

	shl.GetSelectedVertex(allSelVtHd);
	for(YSSIZE_T idx=0; idx<allSelVtHd.GetN(); ++idx)
	{
		visited.AddVertex(allSelVtHd);
	}


	{
		YsArray <YsShellPolygonHandle,N> selPlHd;
		shl.GetSelectedPolygon(selPlHd);
		for(YSSIZE_T idx=0; idx<selPlHd.GetN(); ++idx)
		{
			int nPlVt;
			const YsShellVertexHandle *plVtHd;
			shl.GetVertexListOfPolygon(nPlVt,plVtHd,selPlHd[idx]);
			for(YSSIZE_T vtIdx=0; vtIdx<nPlVt; ++vtIdx)
			{
				if(YSTRUE!=visited.IsIncluded(plVtHd[vtIdx]))
				{
					visited.AddVertex(plVtHd[vtIdx]);
					allSelVtHd.Append(plVtHd[vtIdx]);
				}
			}
		}
	}


	{
		YsArray <YsShellExt::ConstEdgeHandle> selCeHd;
		shl.GetSelectedConstEdge(selCeHd);
		for(YSSIZE_T idx=0; idx<selCeHd.GetN(); ++idx)
		{
			YSSIZE_T nCeVt;
			const YsShellVertexHandle *ceVtHd;
			YSBOOL isLoop;
			shl.GetConstEdge(nCeVt,ceVtHd,isLoop,selCeHd[idx]);
			for(YSSIZE_T vtIdx=0; vtIdx<nCeVt; ++vtIdx)
			{
				if(YSTRUE!=visited.IsIncluded(ceVtHd[vtIdx]))
				{
					visited.AddVertex(ceVtHd[vtIdx]);
					allSelVtHd.Append(ceVtHd[vtIdx]);
				}
			}
		}
	}


	{
		for(auto fgHd : shl.GetSelectedFaceGroup())
		{
			for(auto fgPlHd : shl.GetFaceGroupRaw(fgHd))
			{
				int nPlVt;
				const YsShellVertexHandle *plVtHd;
				shl.GetVertexListOfPolygon(nPlVt,plVtHd,fgPlHd);
				for(YSSIZE_T vtIdx=0; vtIdx<nPlVt; ++vtIdx)
				{
					if(YSTRUE!=visited.IsIncluded(plVtHd[vtIdx]))
					{
						visited.AddVertex(plVtHd[vtIdx]);
						allSelVtHd.Append(plVtHd[vtIdx]);
					}
				}
			}
		}
	}


	return YSOK;
}

/* } */
#endif
