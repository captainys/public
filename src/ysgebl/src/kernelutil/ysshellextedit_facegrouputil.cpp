/* ////////////////////////////////////////////////////////////

File Name: ysshellextedit_facegrouputil.cpp
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

#include "ysshellext_geomcalc.h"
#include "ysshellextedit_facegrouputil.h"

YSRESULT YsShellExtEdit_MergeFaceGroup(YsShellExtEdit &shl,YsShellExt::FaceGroupHandle fgHdToSurvive,YsShellExt::FaceGroupHandle fgHdToDel)
{
	auto fgPlHd=shl.GetFaceGroup(fgHdToDel);
	for(auto plHd : fgPlHd)
	{
		shl.DeleteFaceGroupPolygon(plHd);
	}
	shl.AddFaceGroupPolygon(fgHdToSurvive,fgPlHd);
	return YSOK;
}



YSRESULT YsShellExtEdit_MakeFaceGroupByConstEdge(YsShellExtEdit &shl)
{
	YsShellExtEdit::StopIncUndo incUndo(shl);

	class ConstEdgeBarrier : public YsShellExt_TrackingUtil::SearchCondition
	{
	public:
		virtual YSRESULT TestEdge(const YsShellExt &shl,YsShellVertexHandle edVtHd0,YsShellVertexHandle edVtHd1) const
		{
			if(0<shl.GetNumConstEdgeUsingEdgePiece(edVtHd0,edVtHd1))  // If the piece is used,
			{
				return YSERR;  // block it.
			}
			return YSOK;
		}
	};

	ConstEdgeBarrier barrier;

	for(auto fgHd : shl.AllFaceGroup())
	{
		shl.DeleteFaceGroup(fgHd);
	}


	YsShellPolygonStore visited(shl.Conv());
	for(auto seedPlHd : shl.AllPolygon())
	{
		if(YSTRUE!=visited.IsIncluded(seedPlHd))
		{
			auto plHdArray=YsShellExt_TrackingUtil::MakeEdgeConnectedPolygonGroup(shl.Conv(),seedPlHd,&barrier);
			shl.AddFaceGroup(plHdArray);

			for(auto plHd : plHdArray)
			{
				visited.AddPolygon(plHd);
			}
		}
	}

	return YSOK;
}



YSRESULT YsShellExtEdit_DivideHighBendingAngleFaceGroup(YsShellExtEdit &shl,const double bendingThreshold)
{
	YsArray <YsShellExt::FaceGroupHandle> todo;

	for(auto fgHd : shl.AllFaceGroup())
	{
		if(YSTRUE==YsShellExt_CheckFaceGroupBending(shl.Conv(),fgHd,bendingThreshold))
		{
			todo.Add(fgHd);
		}
	}

	while(0<todo.GetN())
	{
		auto fgHd=todo.Last();
		todo.DeleteLast();

		YsArray <double> cosDha;
		YsArray <YsShell::Edge> unconstEdge;
		YsShellEdgeStore visited(shl.Conv());

		auto fgPlHd=shl.GetFaceGroup(fgHd);
		for(auto plHd : fgPlHd)
		{
			auto plVtHd=shl.GetPolygonVertex(plHd);
			for(auto edIdx : plVtHd.AllIndex())
			{
				YsShell::Edge edge(plVtHd[edIdx],plVtHd.GetCyclic(edIdx+1));
				if(YSTRUE!=shl.IsEdgePieceConstrained(edge) && 
				   YSTRUE!=visited.IsIncluded(edge) &&
				   2==shl.GetNumPolygonUsingEdge(edge))
				{
					auto dha=YsShellExt_CalculateDihedralAngle(shl.Conv(),edge);
					cosDha.Add(cos(dha));
					unconstEdge.Add(edge);
					visited.Add(edge);
				}
			}
		}

		YsSimpleMergeSort<double,YsShell::Edge>(cosDha.GetN(),cosDha,unconstEdge);
		for(auto edge : unconstEdge)
		{
			shl.AddConstEdge(2,edge,YSFALSE);
			if(2<=shl.GetNumConstEdgeConnectedVertex(edge[0]) && 2<=shl.GetNumConstEdgeConnectedVertex(edge[1]))
			{
				class PassSameFaceGroupOnlyDontPassConstEdge : public YsShellExt::Condition
				{
				public:
					YsShellExt::FaceGroupHandle fgHd;
					virtual YSRESULT TestPolygon(const YsShellExt &shl,YsShellPolygonHandle plHd) const
					{
						if(shl.FindFaceGroupFromPolygon(plHd)==fgHd)
						{
							return YSOK;
						}
						return YSERR;
					}
					virtual YSRESULT TestEdge(const YsShellExt &shl,YsShellVertexHandle edVtHd0,YsShellVertexHandle edVtHd1) const
					{
						if(YSTRUE!=shl.IsEdgePieceConstrained(edVtHd0,edVtHd1))
						{
							return YSOK;
						}
						return YSERR;
					}
				};
				PassSameFaceGroupOnlyDontPassConstEdge cond;
				cond.fgHd=fgHd;
				auto subPlg=YsShellExt_TrackingUtil::MakeEdgeConnectedPolygonGroup(shl.Conv(),fgPlHd[0],&cond);
				if(subPlg.GetN()<fgPlHd.GetN())
				{
					shl.DeleteFaceGroupPolygonMulti(subPlg);
					auto newFgHd=shl.AddFaceGroup(subPlg);

					if(YSTRUE==YsShellExt_CheckFaceGroupBending(shl.Conv(),fgHd,bendingThreshold))
					{
						todo.Add(fgHd);
					}
					if(YSTRUE==YsShellExt_CheckFaceGroupBending(shl.Conv(),newFgHd,bendingThreshold))
					{
						todo.Add(newFgHd);
					}
					break;
				}
			}
		}
	}

	return YSOK;
}



void YsShellExtEdit_PaintFaceGroupDifferentColor(YsShellExtEdit &shl)
{
	YsShellExtEdit::StopIncUndo incUndo(shl);

	YsShellExt::FaceGroupStore painted(shl.Conv());

	YsColor fourteenColor[14];
	fourteenColor[ 0].SetIntRGB(0  ,0  ,255);
	fourteenColor[ 1].SetIntRGB(255,0  ,0  );
	fourteenColor[ 2].SetIntRGB(255,0  ,255);
	fourteenColor[ 3].SetIntRGB(0  ,255,0  );
	fourteenColor[ 4].SetIntRGB(0  ,255,255);
	fourteenColor[ 5].SetIntRGB(255,255,0  );
	fourteenColor[ 6].SetIntRGB(255,255,255);
	fourteenColor[ 7].SetIntRGB(0  ,0  ,128);
	fourteenColor[ 8].SetIntRGB(128,0  ,0  );
	fourteenColor[ 9].SetIntRGB(128,0  ,128);
	fourteenColor[10].SetIntRGB(0  ,128,0  );
	fourteenColor[11].SetIntRGB(0  ,128,128);
	fourteenColor[12].SetIntRGB(128,128,0  );
	fourteenColor[13].SetIntRGB(128,128,128);

	for(auto fgHd : shl.AllFaceGroup())
	{
		auto neiFgHdArray=shl.GetEdgeConnectedNeighborFaceGroup(fgHd);

		YsArray <YsColor> neiFgColor;
		for(auto neiFgHd : neiFgHdArray)
		{
			if(YSTRUE==painted.IsIncluded(neiFgHd))
			{
				auto neiFgPlHd=shl.GetFaceGroup(neiFgHd);
				if(0<neiFgPlHd.GetN())
				{
					neiFgColor.Append(shl.GetColor(neiFgPlHd[0]));
				}
			}
		}

		YSBOOL found=YSFALSE;
		YsColor foundColor;
		for(auto &col : fourteenColor)
		{
			if(YSTRUE!=neiFgColor.IsIncluded(col))
			{
				foundColor=col;
				found=YSTRUE;
				break;
			}
		}

		if(YSTRUE==found)
		{
			auto fgPlHd=shl.GetFaceGroup(fgHd);
			for(auto plHd : fgPlHd)
			{
				shl.SetPolygonColor(plHd,foundColor);
			}
			painted.AddFaceGroup(fgHd);
		}
		else
		{
			printf("!\n");
		}
	}
}

