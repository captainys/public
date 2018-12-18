/* ////////////////////////////////////////////////////////////

File Name: ysshellextedit_offsetutil.cpp
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

#include "ysshellextedit_offsetutil.h"
#include <ysshellextmisc.h>


YSRESULT YsShellExtEdit_OffsetUtil2d::ApplyByRatio(YsShellExtEdit &shl,const double ratioAgainstMaxDist)
{
	return ApplyByRatio(*this,shl,ratioAgainstMaxDist);
}

/*static*/ YSRESULT YsShellExtEdit_OffsetUtil2d::ApplyByRatio(YsShellExt_OffsetUtil2d &offsetUtil,YsShellExtEdit &shl,const double ratioAgainstMaxDist)
{
	YsShellExtEdit::StopIncUndo stopIncUndo(&shl);

	YsTwoKeyStore edgeStore;
	offsetUtil.MakeBoundaryEdgeStore(edgeStore,*(const YsShell *)&shl);

	double dist=YsInfinity;
	for(auto newVtx : offsetUtil.newVtxArray)
	{
		const double d=newVtx.maxDist*ratioAgainstMaxDist;
		dist=YsSmaller(dist,d);
	}


	for(auto &newVtx : offsetUtil.newVtxArray)
	{
		newVtx.toPos=newVtx.fromPos+newVtx.dir*dist;
		newVtx.toVtHd=shl.AddVertex(newVtx.toPos);

		for(auto &hangingPlg : newVtx.hangingPlArray)
		{
			YsArray <YsShellVertexHandle,4> plVtHd;
			shl.GetPolygon(plVtHd,hangingPlg.plHd);
			for(YSSIZE_T vtIdx=0; vtIdx<plVtHd.GetN(); ++vtIdx)
			{
				if(newVtx.fromVtHd==plVtHd[vtIdx])
				{
					if(1==hangingPlg.hairIsOneThisSideOfFromVtHd)
					{
						plVtHd.Insert(vtIdx+1,newVtx.toVtHd);
						break;
					}
					else if(-1==hangingPlg.hairIsOneThisSideOfFromVtHd)
					{
						plVtHd.Insert(vtIdx,newVtx.toVtHd);
						break;
					}
				}
			}
			shl.SetPolygonVertex(hangingPlg.plHd,plVtHd);
		}
	}

	// Const-edge re-connection must be done before polygons are modified.
	YsArray <YsShellExt_ConstEdgeVertexInsertionInfo> ceVtInsArray;
	for(auto plHd : offsetUtil.srcPlHdArray)
	{
		YsArray <YsShellVertexHandle> plVtHd;
		shl.GetPolygon(plVtHd,plHd);
		for(YSSIZE_T vtIdx=0; vtIdx<plVtHd.GetN(); ++vtIdx)
		{
			YsShellVertexHandle fromVtHd=plVtHd[vtIdx];

			YSSIZE_T newVtxIdx;
			if(YSOK==offsetUtil.vtKeyToNewVtxIndex.FindElement(newVtxIdx,shl.GetSearchKey(fromVtHd)))
			{
				// The new vertex now needs to be inserted on either plVtHd[idx]-plVtHd[idx+1] or
				// plVtHd[idx]-plVtHd[idx-1];

				YsShellVertexHandle toVtHd=NULL;
				if(YSTRUE==edgeStore.IsIncluded(shl.GetSearchKey(fromVtHd),shl.GetSearchKey(plVtHd.GetCyclic(vtIdx+1))) &&
				   YSTRUE!=edgeStore.IsIncluded(shl.GetSearchKey(fromVtHd),shl.GetSearchKey(plVtHd.GetCyclic(vtIdx-1))))
				{
					toVtHd=plVtHd.GetCyclic(vtIdx-1);
				}
				else if(YSTRUE==edgeStore.IsIncluded(shl.GetSearchKey(fromVtHd),shl.GetSearchKey(plVtHd.GetCyclic(vtIdx-1))) &&
				        YSTRUE!=edgeStore.IsIncluded(shl.GetSearchKey(fromVtHd),shl.GetSearchKey(plVtHd.GetCyclic(vtIdx+1))))
				{
					toVtHd=plVtHd.GetCyclic(vtIdx+1);
				}
				else
				{
					// Maybe the const-edge is lying on the boundary edge sequence.
				}

				if(NULL!=toVtHd)
				{
					YsArray <YsShellExt::ConstEdgeHandle> ceHdArray;
					if(YSOK==shl.FindConstEdgeFromEdgePiece(ceHdArray,fromVtHd,toVtHd))
					{
						for(auto ceHd : ceHdArray)
						{
							YsArray <YsShellVertexHandle> ceVtHd;
							YSBOOL isLoop;
							shl.GetConstEdge(ceVtHd,isLoop,ceHd);

							if(YSTRUE==isLoop)
							{
								auto vtHd0=ceVtHd[0];
								ceVtHd.Append(vtHd0);
							}

							for(YSSIZE_T vtIdx=0; vtIdx<ceVtHd.GetN()-1; ++vtIdx)
							{
								if(ceVtHd[vtIdx]==fromVtHd && ceVtHd[vtIdx+1]==toVtHd)
								{
									ceVtInsArray.Increment();
									ceVtInsArray.Last().ceHd=ceHd;
									ceVtInsArray.Last().beforeOrAfterThisVtHd=fromVtHd;
									ceVtInsArray.Last().insertThisVtHd=offsetUtil.newVtxArray[newVtxIdx].toVtHd;
									ceVtInsArray.Last().beforeOrAfter=+1;
								}
								else if(ceVtHd[vtIdx+1]==fromVtHd && ceVtHd[vtIdx]==toVtHd)
								{
									ceVtInsArray.Increment();
									ceVtInsArray.Last().ceHd=ceHd;
									ceVtInsArray.Last().beforeOrAfterThisVtHd=fromVtHd;
									ceVtInsArray.Last().insertThisVtHd=offsetUtil.newVtxArray[newVtxIdx].toVtHd;
									ceVtInsArray.Last().beforeOrAfter=-1;
								}
							}
						}
					}
				}
			}
		}
	}
	for(auto ceVtIns : ceVtInsArray)
	{
		YsArray <YsShellVertexHandle> ceVtHd;
		YSBOOL isLoop;
		shl.GetConstEdge(ceVtHd,isLoop,ceVtIns.ceHd);

		YSBOOL change=YSFALSE;
		for(YSSIZE_T idx=0; idx<ceVtHd.GetN(); ++idx)
		{
			if(ceVtIns.beforeOrAfterThisVtHd==ceVtHd[idx])
			{
				if(ceVtIns.beforeOrAfter<0)
				{
					ceVtHd.Insert(idx,ceVtIns.insertThisVtHd);
				}
				else
				{
					ceVtHd.Insert(idx+1,ceVtIns.insertThisVtHd);
				}
				change=YSTRUE;
				break;
			}
		}
		if(YSTRUE==change)
		{
			shl.ModifyConstEdge(ceVtIns.ceHd,ceVtHd,isLoop);
		}
	}


	for(auto plHd : offsetUtil.srcPlHdArray)
	{
		YSBOOL change=YSFALSE;
		YsArray <YsShellVertexHandle> plVtHd;
		shl.GetPolygon(plVtHd,plHd);
		for(YSSIZE_T vtIdx=0; vtIdx<plVtHd.GetN(); ++vtIdx)
		{
			YsShellVertexHandle &vtHd=plVtHd[vtIdx];

			YSSIZE_T newVtxIdx;
			if(YSOK==offsetUtil.vtKeyToNewVtxIndex.FindElement(newVtxIdx,shl.GetSearchKey(vtHd)))
			{
				vtHd=offsetUtil.newVtxArray[newVtxIdx].toVtHd;
				change=YSTRUE;
			}
		}
		if(YSTRUE==change)
		{
			shl.SetPolygonVertex(plHd,plVtHd);
		}
	}

	for(auto &edge : offsetUtil.boundaryEdgeArray)
	{
		const VertexOffset newVtx[2]=
		{
			offsetUtil.newVtxArray[edge.newVtxIdx[0]],
			offsetUtil.newVtxArray[edge.newVtxIdx[1]]
		};

		YsShellVertexHandle quadVtHd[4]=
		{
			newVtx[0].fromVtHd,
			newVtx[1].fromVtHd,
			newVtx[1].toVtHd,
			newVtx[0].toVtHd
		};

		YsShellPolygonHandle newPlHd=shl.AddPolygon(4,quadVtHd);

		auto plAttrib=shl.GetPolygonAttrib(edge.plHd);
		shl.SetPolygonAttrib(newPlHd,*plAttrib);
	}

	return YSOK;
}

