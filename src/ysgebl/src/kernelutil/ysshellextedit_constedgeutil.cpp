/* ////////////////////////////////////////////////////////////

File Name: ysshellextedit_constedgeutil.cpp
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

#include "ysshellextedit_constedgeutil.h"
#include "ysshellextedit_localop.h"
#include "ysshellext_geomcalc.h"
#include "ysshellext_constedgeutil.h"


static YSRESULT YsShellExtEdit_MergeVertexOnNewConstEdgeEnd(YsShellExtEdit &shl,const YsShellVertexStore &usedVtx)
{
	for(auto vtHd : usedVtx)
	{
		auto vtCeHd=shl.FindConstEdgeFromVertex(vtHd);
		if(2==vtCeHd.GetN() && vtCeHd[0]!=vtCeHd[1])
		{
			YsShellExtEdit_MergeConstEdge(shl,vtCeHd,YSTRUE);
		}
	}
	return YSOK;
}



YSRESULT YsShellExtEdit_AddConstEdgeByHighDihedralAngleThreshold(YsShellExtEdit &shl,const double angleThr)
{
	YsShellExtEdit::StopIncUndo incUndo(shl);

	YsShellVertexStore usedVtx(shl.Conv());

	YsShellEdgeEnumHandle edHd=NULL;
	while(YSOK==shl.MoveToNextEdge(edHd))
	{
		YsShellVertexHandle edVtHd[2];
		shl.GetEdge(edVtHd,edHd);

		if(0==shl.GetNumConstEdgeUsingEdgePiece(edVtHd))
		{
			const double angle=YsShellExt_CalculateDihedralAngle(shl.Conv(),edVtHd);
			if(angleThr<=angle)
			{
				shl.AddConstEdge(2,edVtHd,YSFALSE);
				usedVtx.AddVertex(edVtHd[0]);
				usedVtx.AddVertex(edVtHd[1]);
			}
		}
	}

	YsShellExtEdit_MergeVertexOnNewConstEdgeEnd(shl,usedVtx);

	return YSOK;
}



YSRESULT YsShellExtEdit_AddConstEdgeAlongNonManifoldEdge(YsShellExtEdit &shl)
{
	YsShellExtEdit::StopIncUndo incUndo(shl);
	auto usedVtx=YsShellExt_AddConstEdgeAlongNonManifoldEdge(shl);
	YsShellExtEdit_MergeVertexOnNewConstEdgeEnd(shl,usedVtx);
	return YSOK;
}



YSRESULT YsShellExtEdit_AddConstEdgeAlongFaceGroupBoundary(YsShellExtEdit &shl)
{
	YsShellExtEdit::StopIncUndo incUndo(shl);

	YsShellVertexStore usedVtx(shl.Conv());

	YsShellEdgeEnumHandle edHd=NULL;
	while(YSOK==shl.MoveToNextEdge(edHd))
	{
		YsShellVertexHandle edVtHd[2];
		shl.GetEdge(edVtHd,edHd);

		if(0==shl.GetNumConstEdgeUsingEdgePiece(edVtHd))
		{
			auto edPlHd=shl.FindPolygonFromEdgePiece(edVtHd);
			if(2<=edPlHd.GetN())
			{
				YSBOOL multiFg=YSFALSE;
				YsShellExt::FaceGroupHandle fgHd0=shl.FindFaceGroupFromPolygon(edPlHd[0]);
				for(auto plHd : edPlHd)
				{
					if(shl.FindFaceGroupFromPolygon(plHd)!=fgHd0)
					{
						multiFg=YSTRUE;
						break;
					}
				}

				if(YSTRUE==multiFg)
				{
					shl.AddConstEdge(2,edVtHd,YSFALSE);
					usedVtx.AddVertex(edVtHd[0]);
					usedVtx.AddVertex(edVtHd[1]);
				}
			}
		}
	}

	YsShellExtEdit_MergeVertexOnNewConstEdgeEnd(shl,usedVtx);

	return YSOK;
}




YSRESULT YsShellExtEdit_ReconsiderConstEdge(YsShellExtEdit &shl)
{
	YsShellExtEdit::StopIncUndo incUndo(shl);


	// Need to remove duplicate.  I was forgetting about it.  2016/03/16
	for(auto ceHd : shl.AllConstEdge())
	{
		YsArray <YsArray <YsShell::VertexHandle> > newCeVtHd;


		YSBOOL isLoop;
		YsArray <YsShell::VertexHandle> ceVtHd,curCeVtHd;
		shl.GetConstEdge(ceVtHd,isLoop,ceHd);
		if(0==ceVtHd.GetN())
		{
			continue;
		}

		int state=0; // 0: Non-duplicate segment or initial    1: Duplicate segment
		if(YSTRUE==isLoop)
		{
			ceVtHd.Add(ceVtHd[0]);
		}
		for(int vtIdx=0; vtIdx<ceVtHd.GetN()-1; ++vtIdx)
		{
			YsShell::VertexHandle edVtHd[2]=
			{
				ceVtHd[vtIdx],ceVtHd[vtIdx+1]
			};

			if(0==state)
			{
				if(2<=shl.GetNumConstEdgeUsingEdgePiece(edVtHd))
				{
					if(2<=curCeVtHd.GetN())
					{
						newCeVtHd.Increment();
						newCeVtHd.Last().MoveFrom(curCeVtHd);
						curCeVtHd.CleanUp();
					}
					state=1;
				}
				else
				{
					if(0==curCeVtHd.GetN())
					{
						curCeVtHd.Add(edVtHd[0]);
					}
					curCeVtHd.Add(edVtHd[1]);
				}
			}
			else
			{
				if(1==shl.GetNumConstEdgeUsingEdgePiece(edVtHd))
				{
					curCeVtHd.Set(2,edVtHd);
					state=0;
				}
			}
		}
		if(2<=curCeVtHd.GetN())
		{
			newCeVtHd.Increment();
			newCeVtHd.Last().MoveFrom(curCeVtHd);
			curCeVtHd.CleanUp();
		}

		if(1==newCeVtHd.GetN() && newCeVtHd[0].GetN()==ceVtHd.GetN())
		{
			// No duplicate.  No change.
		}
		else if(0==newCeVtHd.GetN())
		{
			shl.DeleteConstEdge(ceHd);
		}
		else if(2<=newCeVtHd.GetN() || (1==newCeVtHd.GetN() && newCeVtHd[0].GetN()!=ceVtHd.GetN()))
		{
			shl.ModifyConstEdge(ceHd,newCeVtHd[0],YSFALSE); // The const-edge will not be a loop after removing a segment.
			auto attrib=shl.GetConstEdgeAttrib(ceHd);
			for(int i=1; i<newCeVtHd.GetN(); ++i)
			{
				auto newCeHd=shl.AddConstEdge(newCeVtHd[i],YSFALSE);
				shl.SetConstEdgeAttrib(newCeHd,attrib);
			}
		}
		else
		{
		}
	}



	// First merge, 
	for(auto vtHd : shl.AllVertex())
	{
		auto ceConnVtHd=shl.GetConstEdgeConnectedVertex(vtHd);
		if(2==ceConnVtHd.GetN())
		{
			auto vtCeHd=shl.FindConstEdgeFromVertex(vtHd);
			if(2==vtCeHd.GetN() && vtCeHd[0]!=vtCeHd[1])
			{
				YsShellExtEdit_MergeConstEdge(shl,vtCeHd,YSTRUE);
			}
			else if(1==vtCeHd.GetN())
			{
				YsArray <YsShellVertexHandle> ceVtHd;
				YSBOOL isLoop;
				shl.GetConstEdge(ceVtHd,isLoop,vtCeHd[0]);
				if(YSTRUE!=isLoop && 3<=ceVtHd.GetN() && ceVtHd[0]==ceVtHd.Last())
				{
					ceVtHd.DeleteLast();
					shl.ModifyConstEdge(vtCeHd[0],ceVtHd,YSTRUE);
				}
			}
		}
	}

	// and then split.
	for(auto vtHd : shl.AllVertex())
	{
		auto ceConnVtHd=shl.GetConstEdgeConnectedVertex(vtHd);
		if(2<ceConnVtHd.GetN())
		{
			auto vtCeHd=shl.FindConstEdgeFromVertex(vtHd);
			for(auto ceHd : vtCeHd)
			{
				YsShellExtEdit_SplitConstEdge(shl,ceHd,vtHd);
			}
		}
	}

	return YSOK;
}

