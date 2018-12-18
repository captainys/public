/* ////////////////////////////////////////////////////////////

File Name: ysshellextedit_roundutil.cpp
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

#include "ysshellextedit_roundutil.h"
#include "ysshellextedit_offsetutil.h"
#include "ysshellextedit_topologyutil.h"
#include "ysshellextedit_localop.h"


YSRESULT YsShellExtEdit_RoundUtil3d::Apply(YsShellExtEdit &shl,const double radius,const int nDiv)
{
	return Apply(shl,*this,radius,nDiv);
}

/*static*/ YSRESULT YsShellExtEdit_RoundUtil3d::Apply(YsShellExtEdit &shl,YsShellExt_RoundUtil3d &roundUtil,const double radius,const int nDiv)
{
	YsShellExtEdit::StopIncUndo incUndo(&shl);

	if(YSOK==roundUtil.CalculateRoundingAll((const YsShell &)shl,radius))
	{
		printf("Max Radius=%lf\n",roundUtil.CalculateMaximumRadius((const YsShell &)shl));

		// Populate offsetUtil if necessary.
		for(int side=0; side<2; ++side)
		{
			if(0<roundUtil.offsetUtil[side].newVtxArray.GetN())
			{
				YsShellExtEdit_OffsetUtil2d::ApplyByRatio(roundUtil.offsetUtil[side],shl,0.5);
				for(auto newVtx : roundUtil.offsetUtil[side].newVtxArray)
				{
					roundUtil.cornerArray[newVtx.cornerIdx].toVtHd=newVtx.toVtHd;
				}
			}
		}


		// If from[i]-to[i] equals to[j]-from[j], a virtual vertex must be placed in the middle and divided.
		YsFixedLengthToMultiHashTable <YSSIZE_T,2,1> edgeToCornerIdx;
		for(YSSIZE_T cornerIdx=0; cornerIdx<roundUtil.cornerArray.GetN(); ++cornerIdx)
		{
			YSHASHKEY edVtKey[2]=
			{
				shl.GetSearchKey(roundUtil.cornerArray[cornerIdx].fromVtHd),
				shl.GetSearchKey(roundUtil.cornerArray[cornerIdx].toVtHd)
			};
			edgeToCornerIdx.AddElement(2,edVtKey,cornerIdx);
		}
		YsFixedLengthToMultiHashElemEnumHandle elHd;
		if(YSOK==edgeToCornerIdx.RewindElemEnumHandle(elHd))
		{
			for(;;)
			{
				const YSHASHKEY *edVtKey=edgeToCornerIdx.GetKey(elHd);
				if(NULL!=edVtKey)
				{
					int nCornerIdx;
					const YSSIZE_T *cornerIdx;
					if(YSOK==edgeToCornerIdx.FindElement(nCornerIdx,cornerIdx,2,edVtKey) &&
					   2<=nCornerIdx)
					{
						const YsShellVertexHandle edVtHd[2]=
						{
							roundUtil.cornerArray[cornerIdx[0]].fromVtHd,
							roundUtil.cornerArray[cornerIdx[0]].toVtHd
						};
						const YsVec3 edVtPos[2]=
						{
							shl.GetVertexPosition(edVtHd[0]),
							shl.GetVertexPosition(edVtHd[1])
						};
						YsShellVertexHandle midVtHd=shl.AddVertex((edVtPos[0]+edVtPos[1])/2.0);
						for(YSSIZE_T foundIdx=0; foundIdx<nCornerIdx; ++foundIdx)
						{
							roundUtil.cornerArray[cornerIdx[foundIdx]].toVtHd=midVtHd;
						}
						YsArray <YsShellVertexHandle> midVtHdArray(1,&midVtHd);
						YsShellExtEdit_TopologyUtil topoUtil;
						topoUtil.InsertVertexOnEdge(shl,edVtHd[0],edVtHd[1],midVtHdArray);
					}
				}

				if(YSOK!=edgeToCornerIdx.FindNextKey(elHd))
				{
					break;
				}
			}
		}



		for(auto &corner : roundUtil.cornerArray)
		{
			if(YSOK!=corner.SubdivideByNumberOfSubdivision(nDiv))
			{
				return YSERR;
			}

			YsArray <YsShellVertexHandle> vtHdToInsert;
			for(auto &newVtx : corner.subDiv)
			{
				newVtx.vtHd=shl.AddVertex(newVtx.pos);
				vtHdToInsert.Append(newVtx.vtHd);
			}

			YsShellExtEdit_TopologyUtil topoUtil;
			topoUtil.InsertVertexOnEdge(shl,corner.toVtHd,corner.fromVtHd,vtHdToInsert);

			shl.SetVertexPosition(corner.fromVtHd,corner.roundedCornerPos);
		}



		// Split polygon per corner
		for(auto corner : roundUtil.cornerArray)
		{
			YsArray <YsShellPolygonHandle> vtPlHd;
			shl.FindPolygonFromVertex(vtPlHd,corner.fromVtHd);
			for(auto plHd : vtPlHd)
			{
				YsArray <YSSIZE_T> cornerIdxArray=roundUtil.FindHalfRoundCornerFromPolygon((const YsShell &)shl,plHd);

				for(YSSIZE_T idx=cornerIdxArray.GetN()-1; 0<=idx; --idx)
				{
					if(roundUtil.cornerArray[cornerIdxArray[idx]].fromVtHd!=corner.fromVtHd)
					{
						cornerIdxArray.DeleteBySwapping(idx);
					}
				}

				if(2==cornerIdxArray.GetN() &&
				   roundUtil.cornerArray[cornerIdxArray[0]].subDiv.GetN()==roundUtil.cornerArray[cornerIdxArray[1]].subDiv.GetN())
				{
					SplitPolygon(shl,plHd,roundUtil,roundUtil.cornerArray[cornerIdxArray[0]],roundUtil.cornerArray[cornerIdxArray[1]]);
				}
			}
		}
		// Split polygon per edge
		for(auto edge : roundUtil.roundEdgeArray)
		{
			YsArray <YsShellPolygonHandle> edPlHd;
			shl.FindPolygonFromEdgePiece(edPlHd,edge.edVtHd[0],edge.edVtHd[1]);
			for(auto plHd : edPlHd)
			{
				YsArray <YSSIZE_T> cornerIdxArray=roundUtil.FindHalfRoundCornerFromPolygon((const YsShell &)shl,plHd);

				for(YSSIZE_T idx=cornerIdxArray.GetN()-1; 0<=idx; --idx)
				{
					if(roundUtil.cornerArray[cornerIdxArray[idx]].fromVtHd!=edge.edVtHd[0] &&
					   roundUtil.cornerArray[cornerIdxArray[idx]].fromVtHd!=edge.edVtHd[1])
					{
						cornerIdxArray.DeleteBySwapping(idx);
					}
				}

				if(2==cornerIdxArray.GetN() &&
				   roundUtil.cornerArray[cornerIdxArray[0]].subDiv.GetN()==roundUtil.cornerArray[cornerIdxArray[1]].subDiv.GetN())
				{
					SplitPolygon(shl,plHd,roundUtil,roundUtil.cornerArray[cornerIdxArray[0]],roundUtil.cornerArray[cornerIdxArray[1]]);
				}
			}
		}


		// Edge collapse must be done after polygons are split since
		// FindHalfRoundCornerFromPolygon uses HalfRoundCorner::toVtHd, which is same as newVtx.toVtHd,
		// which will be deleted in edge collapse.
		for(int side=0; side<2; ++side)
		{
			if(0<roundUtil.offsetUtil[side].newVtxArray.GetN())
			{
				for(auto newVtx : roundUtil.offsetUtil[side].newVtxArray)
				{
					if(0<=newVtx.cornerIdx && 0<roundUtil.cornerArray[newVtx.cornerIdx].subDiv.GetN())
					{
						YsShellExtEdit_CollapseEdge(
							shl,
							newVtx.toVtHd,
							roundUtil.cornerArray[newVtx.cornerIdx].subDiv[0].vtHd);
					}
				}
			}
		}

		return YSOK;
	}
	return YSERR;
}

/*static*/ void YsShellExtEdit_RoundUtil3d::SplitPolygon(
    YsShellExtEdit &shl,YsShellPolygonHandle plHd,
    YsShellExt_RoundUtil3d &,YsShellExt_RoundUtil3d::HalfRoundCorner &corner0,YsShellExt_RoundUtil3d::HalfRoundCorner &corner1)
{
	if(corner0.subDiv.GetN()==corner1.subDiv.GetN())
	{
		YsShellPolygonHandle plHdToSplit=plHd;
		YsShellExtEdit_TopologyUtil topoUtil;
		for(int i=0; i<corner0.subDiv.GetN(); ++i)
		{
			if(YSOK==topoUtil.SplitPolygon(shl,plHdToSplit,corner0.subDiv[i].vtHd,corner1.subDiv[i].vtHd))
			{
				YsArray <YsShellVertexHandle> plVtHd;
				shl.GetPolygon(plVtHd,topoUtil.modifiedPlHdArray[0]);
				if(YSTRUE==plVtHd.IsIncluded(corner0.fromVtHd))
				{
					plHdToSplit=topoUtil.modifiedPlHdArray[0];
				}
				else
				{
					plHdToSplit=topoUtil.modifiedPlHdArray[1];
				}
			}
			else
			{
				break;
			}
		}
	}
}

