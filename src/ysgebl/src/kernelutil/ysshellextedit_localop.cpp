/* ////////////////////////////////////////////////////////////

File Name: ysshellextedit_localop.cpp
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

#include "ysshellextedit_localop.h"
#include "ysshellext_localop.h"
#include "ysshellext_trackingutil.h"

YSRESULT YsShellExtEdit_InsertVertexOnEdge(YsShellExtEdit &shl,const YsShellVertexHandle edVtHd[2],YsShellVertexHandle vtHd)
{
	YsArray <YsShellPolygonHandle,2> plHdArray;
	if(YSOK==shl.FindPolygonFromEdgePiece(plHdArray,edVtHd))
	{
		for(YSSIZE_T plIdx=0; plIdx<plHdArray.GetN(); ++plIdx)
		{
			YsArray <YsShellVertexHandle,4> plVtHd;
			shl.GetVertexListOfPolygon(plVtHd,plHdArray[plIdx]);
			if(YSOK==YsShellExt_InsertVertexOnEdgeOfVertexArray(plVtHd,YSTRUE,edVtHd,vtHd))
			{
				shl.SetPolygonVertex(plHdArray[plIdx],plVtHd);
			}
		}
	}

	YsArray <YsShellExt::ConstEdgeHandle,2> ceHdArray;
	if(YSOK==shl.FindConstEdgeFromEdgePiece(ceHdArray,edVtHd))
	{
		for(YSSIZE_T ceIdx=0; ceIdx<ceHdArray.GetN(); ++ceIdx)
		{
			YSBOOL isLoop;
			YsArray <YsShellVertexHandle,16> ceVtHd;
			shl.GetConstEdge(ceVtHd,isLoop,ceHdArray[ceIdx]);
			if(YSOK==YsShellExt_InsertVertexOnEdgeOfVertexArray(ceVtHd,isLoop,edVtHd,vtHd))
			{
				shl.ModifyConstEdge(ceHdArray[ceIdx],ceVtHd,isLoop);
			}
		}
	}

	return YSOK;
}

YSRESULT YsShellExtEdit_CollapseEdge(YsShellExtEdit &shl,YsShellVertexHandle vtHdToDelete,YsShellVertexHandle vtHdToSurvive)
{
	YsShell_CollapseInfo info;
	if(YSOK==info.MakeInfo(shl.Conv(),vtHdToDelete,vtHdToSurvive))
	{
		const YSBOOL preserveVertes=YSFALSE;
		info.Apply<YsShellExtEdit>(shl,preserveVertes);
		return YSOK;
	}
	return YSERR;
}

YSRESULT YsShellExtEdit_TriangulatePolygon(YsShellExtEdit &shl,YsShellPolygonHandle plHd)
{
	auto triangulated=YsShellExt_TriangulatePolygon(shl.Conv(),plHd);
	if(0<triangulated.GetN())
	{
		YsShellExtEdit::StopIncUndo incUndo(shl);

		auto fgHd=shl.FindFaceGroupFromPolygon(plHd);
		for(YSSIZE_T idx=0; idx<=triangulated.GetN()-3; idx+=3)
		{
			const YsShellVertexHandle triVtHd[3]=
			{
				triangulated[idx],
				triangulated[idx+1],
				triangulated[idx+2],
			};
			if(0==idx)
			{
				shl.SetPolygonVertex(plHd,3,triVtHd);
			}
			else
			{
				auto newPlHd=shl.AddPolygon(3,triVtHd);

				auto attrib=shl.GetPolygonAttrib(plHd);
				shl.SetPolygonAttrib(newPlHd,*attrib);

				auto nom=shl.GetNormal(plHd);
				auto col=shl.GetColor(plHd);

				shl.SetPolygonNormal(newPlHd,nom);
				shl.SetPolygonColor(newPlHd,col);

				shl.AddFaceGroupPolygon(fgHd,1,&newPlHd);
			}

		}
		return YSOK;
	}
	return YSERR;
}

YsShellExt::ConstEdgeHandle YsShellExtEdit_SplitConstEdge(YsShellExtEdit &shl,YsShellExt::ConstEdgeHandle ceHd,YsShellVertexHandle vtHd)
{
	YsShellExtEdit::StopIncUndo incUndo(shl);

	YSBOOL isLoop;
	YsArray <YsShellVertexHandle,16> ceVtHd;
	shl.GetConstEdge(ceVtHd,isLoop,ceHd);
	if(YSTRUE!=isLoop)
	{
		for(YSSIZE_T idx=1; idx<ceVtHd.GetN()-1; ++idx)
		{
			if(ceVtHd[idx]==vtHd)
			{
				YsArray <YsShellVertexHandle,16> newCeVtHd;
				newCeVtHd.Set(ceVtHd.GetN()-idx,ceVtHd.GetArray()+idx);
				auto attrib=shl.GetConstEdgeAttrib(ceHd);

				auto newCeHd=shl.AddConstEdge(newCeVtHd,YSFALSE);
				shl.SetConstEdgeAttrib(newCeHd,attrib);

				shl.ModifyConstEdge(ceHd,idx+1,ceVtHd);

				return newCeHd;
			}
		}
	}
	else
	{
		for(auto idx : ceVtHd.AllIndex())
		{
			if(ceVtHd[idx]==vtHd)
			{
				for(int copyIdx=0; copyIdx<=idx; ++copyIdx)
				{
					ceVtHd.Append(ceVtHd[copyIdx]);
				}
				ceVtHd.Delete(0,idx);
				shl.ModifyConstEdge(ceHd,ceVtHd,YSFALSE);
				return ceHd;
			}
		}
	}
	return NULL;
}

void YsShellExtEdit_SplitConstEdgeAtVertex(YsShellExtEdit &shl,YsShellVertexHandle vtHd)
{
	YsShellExtEdit::StopIncUndo incUndo(shl);

	auto vtCeHd=shl.FindConstEdgeFromVertex(vtHd);
	for(auto ceHd : vtCeHd)
	{
		YsShellExtEdit_SplitConstEdge(shl,ceHd,vtHd);
	}
}

YsShellExt::ConstEdgeHandle YsShellExtEdit_MergeConstEdge(YsShellExtEdit &shl,YSSIZE_T nCe,const YsShellExt::ConstEdgeHandle ceHdArray[],YSBOOL makeLoopIfClosed)
{
	return YsShellExt_MergeConstEdge(shl,nCe,ceHdArray,makeLoopIfClosed);
}

YSRESULT YsShellExtEdit_ApplyPolygonSplit(YsShellExtEdit &shl,const YsShell_LocalOperation::PolygonSplit &plgSplit)
{
	YSBOOL first=YSTRUE;
	for(auto &newPlVtHd : plgSplit.plVtHdArray)
	{
		if(YSTRUE==first)
		{
			shl.SetPolygonVertex(plgSplit.plHd,newPlVtHd);
			first=YSFALSE;
		}
		else
		{
			auto nom=shl.GetNormal(plgSplit.plHd);
			auto col=shl.GetColor(plgSplit.plHd);
			auto attrib=shl.GetPolygonAttrib(plgSplit.plHd);
			auto fgHd=shl.FindFaceGroupFromPolygon(plgSplit.plHd);

			auto newPlHd=shl.AddPolygon(newPlVtHd);
			shl.SetPolygonNormal(newPlHd,nom);
			shl.SetPolygonColor(newPlHd,col);
			shl.SetPolygonAttrib(newPlHd,*attrib);
			if(NULL!=fgHd)
			{
				shl.AddFaceGroupPolygon(fgHd,1,&newPlHd);
			}
		}
	}
	return YSOK;
}

YSRESULT YsShellExtEdit_ApplyConstEdgeSplit(YsShellExtEdit &shl,const YsShell_LocalOperation::ConstEdgeSplit &ceSplit)
{
	YSBOOL first=YSTRUE;
	for(auto &newCeVtHd : ceSplit.ceVtHdArray)
	{
		if(YSTRUE==first)
		{
			shl.ModifyConstEdge(ceSplit.ceHd,newCeVtHd,YSFALSE);
			first=YSFALSE;
		}
		else
		{
			auto attrib=shl.GetConstEdgeAttrib(ceSplit.ceHd);
			auto newCeHd=shl.AddConstEdge(newCeVtHd,YSFALSE);
			shl.SetConstEdgeAttrib(newCeHd,attrib);
		}
	}
	return YSOK;
}

YSRESULT YsShellExtEdit_ApplyFaceGroupSplit(YsShellExtEdit &shl,const YsShell_LocalOperation::FaceGroupSplit &fgSplit)
{
	if(2<=fgSplit.fgPlHdArray.GetN())
	{
		shl.ModifyFaceGroup(fgSplit.fgHd,fgSplit.fgPlHdArray[0]);
		for(auto index : fgSplit.fgPlHdArray.AllIndex())
		{
			if(0<index)
			{
				auto &fgPlHd=fgSplit.fgPlHdArray[index];
				auto newFgHd=shl.AddFaceGroup(fgPlHd);
				auto attrib=shl.GetFaceGroupAttrib(fgSplit.fgHd);
				shl.SetFaceGroupAttrib(newFgHd,attrib);
			}
		}
		return YSOK;
	}
	return YSERR;
}

