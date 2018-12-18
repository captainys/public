/* ////////////////////////////////////////////////////////////

File Name: ysshellextedit_stitchingutil.cpp
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

#include "ysshellext_condition.h"
#include "ysshellext_trackingutil.h"
#include "ysshellextedit_stitchingutil.h"
#include "ysshellextedit_localop.h"
#include "ysshellextedit_topologyutil.h"


 #define YS_DEBUG_SPECIFIC_EDGE
// #define YS_DEBUG_SPECIFIC_VERTEX
// #define YS_DEBUG_SPECIFIC_VERTEX_STOP


#ifdef YS_DEBUG_SPECIFIC_VERTEX
static const YsVec3 watchVtPos=YsVec3(-1456.2094428700, 64.4021512874, -2.3092000235);
#endif

#ifdef YS_DEBUG_SPECIFIC_EDGE
static const YsVec3 watchEdVtPos[2]={YsVec3(-1473.708958,  59.165942,  -4.809200),YsVec3(-1475.941299,  64.107875,  -4.809200)};

#endif



YSRESULT YsShellExtEdit_FreeStitching(YsShellExtEdit &shl)
{
	YsShellExtEdit::StopIncUndo incUndo(shl);

	YsShellExtEdit_StitchingUtil stitchUtil;
	stitchUtil.PrepareForSingleUseEdge(shl.Conv());
	stitchUtil.FilterBySelfLengthAsThresholdCondition(shl.Conv());

	while(YSOK==stitchUtil.ProcessExpandedNearestMutualExclusiveEdgeVertexPair(shl.Conv()))
	{
	}


	stitchUtil.CleanUp(); // ProcessNearestMutualExclusiveEdgeVertexPair will delete all edge-vertex pairs.  Needs re-making.
	stitchUtil.PrepareForSingleUseEdge(shl.Conv());
	stitchUtil.FilterBySelfLengthAsThresholdCondition(shl.Conv());
	auto crackArray=stitchUtil.FindOneToNCrack(shl.Conv());
	printf("%d\n",(int)crackArray.GetN());

	for(auto &crack : crackArray)
	{
		stitchUtil.CloseOneToNCrack(shl,crack);
	}


	YsShellExtEdit_TopologyUtil topoUtil;
	topoUtil.SplitAllDegeneratePolygon(shl);

	return YSOK;
}



YSRESULT YsShellExtEdit_StitchingUtil::ProcessNearestMutualExclusiveEdgeVertexPair(YsShellExtEdit &shl)
{
	YsShellExtEdit_TopologyUtil topoUtil;

	// See research note 2014/12/17
	// See research note 2014/12/30 for Condition 5

	auto edVtPairHd=edVtProx.GetNearestEdgeVertexPairHandle();
	if(edVtPairHd.IsNotNull())
	{
		// Must check if no single-use edge is connected to the vertex because of the previous stitching.

		auto edVtPair=edVtProx.GetEdgeVertexPair(edVtPairHd);

		#ifdef YS_DEBUG_SPECIFIC_VERTEX
		if(shl.GetVertexPosition(edVtPair.vtHd)==watchVtPos)
		{
			YsPrintf("%s\n",shl.GetVertexPosition(edVtPair.vtHd).Txt());
			YsPrintf("%s %s\n",shl.GetVertexPosition(edVtPair.edVtHd[0]).Txt(),shl.GetVertexPosition(edVtPair.edVtHd[1]).Txt());
			YsPrintf("nSharingEdge=%d\n",edVtPair.nSharingEdge);
		}
		#endif


		if(2==YsShellExt_TopologyUtil::GetNumSingleUseEdgeConnectedToVertex(shl.Conv(),edVtPair.vtHd) &&  // Condition (3) of Mutual-Exclusive conditions
		   YSTRUE==YsShellExt_TopologyUtil::IsSingleUseEdge(shl.Conv(),edVtPair.edVtHd))  // Condition (1) of Mutual-Exclusive conditions
		{
			if(1==edVtPair.nSharingEdge)  // Condition (4) of Mutual-Exclusive conditions
			{
				// Condition 5
				const double e1e2=shl.GetEdgeLength(edVtPair.edVtHd);
				if(shl.GetEdgeLength(edVtPair.vtHd,edVtPair.edVtHd[0])<e1e2 &&
				   shl.GetEdgeLength(edVtPair.vtHd,edVtPair.edVtHd[1])<e1e2)
				{
					// Mutual
					YSBOOL reverseProximityCheckOrVertexShared[2]=
					{
						YSFALSE,YSFALSE
					};

					YsShellExt_EdgeVertexProximityUtil::EDVTPAIR_HANDLE reverseCheckPairHd[2]=
					{
						edVtProx.FindEdgeVertexPairFromVertex(edVtPair.edVtHd[0]),
						edVtProx.FindEdgeVertexPairFromVertex(edVtPair.edVtHd[1]),
					};

					for(int i=0; i<2; ++i)  // Condition (2) and (2)' of Mutual-Exclusive conditions.
					{
						if(reverseCheckPairHd[i].IsNotNull())
						{
							YsShellExt_EdgeVertexProximityUtil::EdgeVertexPair reverseCheckPair=edVtProx.GetEdgeVertexPair(reverseCheckPairHd[i]);
							if(reverseCheckPair.edVtHd[0]==edVtPair.vtHd || reverseCheckPair.edVtHd[1]==edVtPair.vtHd)  // Condition (2) of Mutual-Exclusive conditions.
							{
								#ifdef YS_DEBUG_SPECIFIC_VERTEX
								if(shl.GetVertexPosition(edVtPair.vtHd)==watchVtPos)
								{
									printf("%s %s\n",shl.GetVertexPosition(reverseCheckPair.edVtHd[0]).Txt(),shl.GetVertexPosition(reverseCheckPair.edVtHd[1]).Txt());
									printf("(2)\n");
								}
								#endif

								reverseProximityCheckOrVertexShared[i]=YSTRUE;
							}
						}
						if(YSTRUE!=reverseProximityCheckOrVertexShared[i])
						{
							if(YSTRUE==YsShellExt_TopologyUtil::IsSingleUseEdge(shl.Conv(),edVtPair.vtHd,edVtPair.edVtHd[i]) &&
							   0==edVtProx.GetEdgeIsClosestOfHowManyVertex(edVtPair.vtHd,edVtPair.edVtHd[i]))
							{
								#ifdef YS_DEBUG_SPECIFIC_VERTEX
								if(shl.GetVertexPosition(edVtPair.vtHd)==watchVtPos)
								{
									printf("(2)'\n");
								}
								#endif

								reverseProximityCheckOrVertexShared[i]=YSTRUE;
							}
						}
					}


					if(YSTRUE==reverseProximityCheckOrVertexShared[0] && YSTRUE==reverseProximityCheckOrVertexShared[1])
					{
						#ifdef YS_DEBUG_SPECIFIC_VERTEX
						if(shl.GetVertexPosition(edVtPair.vtHd)==watchVtPos)
						{
							printf("%s %d\n",__FUNCTION__,__LINE__);
						}
						#endif

						#ifdef YS_DEBUG_SPECIFIC_EDGE
						if(YSTRUE==YsSameEdge(
							   watchEdVtPos[0],
							   watchEdVtPos[1],
							   shl.GetVertexPosition(edVtPair.edVtHd[0]),
							   shl.GetVertexPosition(edVtPair.edVtHd[1])))
						{
							printf("%s %d\n",__FUNCTION__,__LINE__);
							printf("%s\n",shl.GetVertexPosition(edVtPair.vtHd).Txt());
						}
						#endif

						topoUtil.InsertVertexOnEdge(shl,edVtPair.edVtHd,1,&edVtPair.vtHd);
					}
				}
			}
		}


		#if defined(YS_DEBUG_SPECIFIC_VERTEX) && defined(YS_DEBUG_SPECIFIC_VERTEX_STOP)
		if(shl.GetVertexPosition(edVtPair.vtHd)==YsVec3(-1456.334997,  64.171291,  -0.000000) ||
		   shl.GetVertexPosition(edVtPair.vtHd)==YsVec3(-1456.2094428700, 64.4021512874, -2.3092000235))
		{
			printf("%s %d>",__FUNCTION__,__LINE__);getchar();
		}
		#endif


		edVtProx.DeleteEdgeVertexPair(edVtPairHd);
		return YSOK;
	}
	return YSERR;
}



YSRESULT YsShellExtEdit_StitchingUtil::ProcessExpandedNearestMutualExclusiveEdgeVertexPair(YsShellExtEdit &shl)
{
	YsShellExtEdit_TopologyUtil topoUtil;

	// See research note 2014/12/23
	// See research note 2014/12/30 for Condition 5

	auto edVtPairHd=edVtProx.GetNearestEdgeVertexPairHandle();
	if(edVtPairHd.IsNotNull())
	{
		auto edVtPair=edVtProx.GetEdgeVertexPair(edVtPairHd);

		#ifdef YS_DEBUG_SPECIFIC_EDGE
		if(YSTRUE==YsSameEdge(
			   watchEdVtPos[0],
			   watchEdVtPos[1],
			   shl.GetVertexPosition(edVtPair.edVtHd[0]),
			   shl.GetVertexPosition(edVtPair.edVtHd[1])))
		{
			printf("%s %d\n",__FUNCTION__,__LINE__);
			printf("Edge (%s)-(%s)\n",shl.GetVertexPosition(edVtPair.edVtHd[0]).Txt(),shl.GetVertexPosition(edVtPair.edVtHd[1]).Txt());
			printf("NPUE %d\n",shl.GetNumPolygonUsingEdge(edVtPair.edVtHd));
			printf("IsSingleUse %d\n",YsShellExt_TopologyUtil::IsSingleUseEdge(shl.Conv(),edVtPair.edVtHd));
		}
		#endif

		if(0==shl.GetNumPolygonUsingEdge(edVtPair.edVtHd) ||
		   YSTRUE!=YsShellExt_TopologyUtil::IsSingleUseEdge(shl.Conv(),edVtPair.edVtHd))  // May be fixed already.
		{
			edVtProx.DeleteEdge(edVtPair.edVtHd);
			return YSOK;
		}

		auto nearVtx=edVtProx.FindNearestVertexFromEdgePiece(edVtPair.edVtHd);  // Condition (1)&(4) getting all vertex whose closest edge is edVtHd[0]-edVtHd[1]

		#ifdef YS_DEBUG_SPECIFIC_EDGE
		if(YSTRUE==YsSameEdge(
			   watchEdVtPos[0],
			   watchEdVtPos[1],
			   shl.GetVertexPosition(edVtPair.edVtHd[0]),
			   shl.GetVertexPosition(edVtPair.edVtHd[1])))
		{
			printf("%s %d\n",__FUNCTION__,__LINE__);
			printf("  %d near vertices.\n",(int)nearVtx.GetN());
			printf("  nSharing %d\n",edVtPair.nSharingEdge);
			for(auto vtHd : nearVtx)
			{
				printf("  %s\n",shl.GetVertexPosition(vtHd).Txt());
			}
		}
		#endif

		if(0<nearVtx.GetN() &&
		   nearVtx.GetN()==edVtPair.nSharingEdge)  // One or more verteices whose closest is this edge may be processed already.  Checking for the exclusive consition again.
		{
			#ifdef YS_DEBUG_SPECIFIC_EDGE
			if(YSTRUE==YsSameEdge(
				   watchEdVtPos[0],
				   watchEdVtPos[1],
				   shl.GetVertexPosition(edVtPair.edVtHd[0]),
				   shl.GetVertexPosition(edVtPair.edVtHd[1])))
			{
				printf("%s %d\n",__FUNCTION__,__LINE__);
			}
			#endif

			// Condition (2)
			if(1<nearVtx.GetN())
			{
				for(int idx=0; idx<nearVtx.GetN()-1; ++idx)
				{
					if(YSTRUE!=YsShellExt_TopologyUtil::IsSingleUseEdge(shl.Conv(),nearVtx[idx],nearVtx[idx+1]) ||
					   0<edVtProx.GetEdgeIsClosestOfHowManyVertex(nearVtx[idx],nearVtx[idx+1]))
					{
						edVtProx.DeleteEdge(edVtPair.edVtHd);
						return YSOK;
					}
				}
			}

			// Condition (5)
			const double e1e2=shl.GetEdgeLength(edVtPair.edVtHd);
			for(auto vtHd : nearVtx)
			{
				if(shl.GetEdgeLength(vtHd,edVtPair.edVtHd[0])>=e1e2 ||
				   shl.GetEdgeLength(vtHd,edVtPair.edVtHd[1])>=e1e2)
				{
					edVtProx.DeleteEdge(edVtPair.edVtHd);
					return YSOK;
				}
			}

			#ifdef YS_DEBUG_SPECIFIC_EDGE
			if(YSTRUE==YsSameEdge(
				   watchEdVtPos[0],
				   watchEdVtPos[1],
				   shl.GetVertexPosition(edVtPair.edVtHd[0]),
				   shl.GetVertexPosition(edVtPair.edVtHd[1])))
			{
				printf("%s %d\n",__FUNCTION__,__LINE__);
			}
			#endif

			// Condition (3)
			YSBOOL reverseProximityCheckOrVertexShared[2]=
			{
				YSFALSE,YSFALSE
			};
			YsShellExt_EdgeVertexProximityUtil::EDVTPAIR_HANDLE reverseCheckPairHd[2]=
			{
				edVtProx.FindEdgeVertexPairFromVertex(edVtPair.edVtHd[0]),
				edVtProx.FindEdgeVertexPairFromVertex(edVtPair.edVtHd[1]),
			};
			const YsShellVertexHandle tipVtHd[2]=
			{
				nearVtx[0],
				nearVtx.Last()
			};


			for(int i=0; i<2; ++i)  // Condition (3) of Expanded Mutual-Exclusive conditions.
			{
				if(reverseCheckPairHd[i].IsNotNull())
				{
					YsShellExt_EdgeVertexProximityUtil::EdgeVertexPair reverseCheckPair=edVtProx.GetEdgeVertexPair(reverseCheckPairHd[i]);
					if(reverseCheckPair.edVtHd[0]==tipVtHd[i] || reverseCheckPair.edVtHd[1]==tipVtHd[i])  // Condition (2) of Mutual-Exclusive conditions.
					{
						reverseProximityCheckOrVertexShared[i]=YSTRUE;
					}
				}
				if(YSTRUE!=reverseProximityCheckOrVertexShared[i])
				{
					if(YSTRUE==YsShellExt_TopologyUtil::IsSingleUseEdge(shl.Conv(),tipVtHd[i],edVtPair.edVtHd[i]) &&
					   0==edVtProx.GetEdgeIsClosestOfHowManyVertex(tipVtHd[i],edVtPair.edVtHd[i]))
					{
						reverseProximityCheckOrVertexShared[i]=YSTRUE;
					}
				}
			}

			#ifdef YS_DEBUG_SPECIFIC_EDGE
			if(YSTRUE==YsSameEdge(
				   watchEdVtPos[0],
				   watchEdVtPos[1],
				   shl.GetVertexPosition(edVtPair.edVtHd[0]),
				   shl.GetVertexPosition(edVtPair.edVtHd[1])))
			{
				printf("%s %d\n",__FUNCTION__,__LINE__);
				printf(" %d %d\n",reverseProximityCheckOrVertexShared[0],reverseProximityCheckOrVertexShared[1]);
				printf(" Tip[0] %s\n",shl.GetVertexPosition(tipVtHd[0]).Txt());
				printf(" Tip[1] %s\n",shl.GetVertexPosition(tipVtHd[1]).Txt());
			}
			#endif


			if(YSTRUE==reverseProximityCheckOrVertexShared[0] && YSTRUE==reverseProximityCheckOrVertexShared[1])
			{
				topoUtil.InsertVertexOnEdge(shl,edVtPair.edVtHd,nearVtx.GetN(),nearVtx);
			}
		}

		edVtProx.DeleteEdge(edVtPair.edVtHd);
		return YSOK;
	}
	return YSERR;
}



YSRESULT YsShellExtEdit_StitchingUtil::CloseOneToNCrack(YsShellExtEdit &shl,Crack &crack)
{
	if(2==crack.path[0].GetN() && 3<=crack.path[1].GetN())
	{
		auto hd=edVtProx.FindEdgeVertexPairFromEdgePiece(crack.path[0]);
		if(1==hd.GetN() && hd[0].IsNotNull())
		{
			edVtProx.DeleteEdgeVertexPair(hd[0]);
		}

		for(YSSIZE_T idx=0; idx<crack.path[1].GetN()-1; ++idx)
		{
			auto hd=edVtProx.FindEdgeVertexPairFromEdgePiece(crack.path[1][idx],crack.path[1][idx+1]);
			if(1==hd.GetN() && hd[0].IsNotNull())
			{
				edVtProx.DeleteEdgeVertexPair(hd[0]);
			}
		}

		YsShellExtEdit_TopologyUtil topoUtil;
		topoUtil.InsertVertexOnEdge(shl,crack.path[0],crack.path[1].GetN(),crack.path[1]);
		return YSOK;
	}
	return YSERR;
}



////////////////////////////////////////////////////////////

YSRESULT YsShellExtEdit_LoopStitching(YsShellExtEdit &shl)
{
	YsShellExt_SingleUseEdgeCondition cond;

	YSRESULT res=YSERR;

	// First deal with a loop hanging on a vertex.
	for(auto vtHd0 : shl.AllVertex())
	{
		auto connVtHd=shl.GetConnectedVertex(vtHd0,cond);
		if(3<=connVtHd.GetN())
		{
			for(auto vtHd1 : connVtHd)
			{
				auto vtLoop=YsShellExt_TrackingUtil::TrackEdge(shl.Conv(),vtHd0,vtHd1,cond);
				if(3<=vtLoop.GetN() && vtLoop[0]==vtLoop.Last())
				{
					YsShellExtEdge_EdgeStaplingUtil stitcher;
					stitcher.SetCrackBoundary(shl.Conv(),vtLoop.GetN()-1,vtLoop);

					if(YSOK==stitcher.StapleMinimumOpenAngleCorner(shl))
					{
						res=YSOK;
					}
				}
			}
		}
	}

	for(auto vtHd0 : shl.AllVertex())
	{
		auto connVtHd=shl.GetConnectedVertex(vtHd0,cond);
		if(2==connVtHd.GetN())
		{
			auto vtLoop=YsShellExt_TrackingUtil::TrackEdge(shl.Conv(),vtHd0,connVtHd[0],cond);
			if(3<=vtLoop.GetN() && vtLoop[0]==vtLoop.Last())
			{
				YsShellExtEdge_EdgeStaplingUtil stitcher;
				stitcher.SetCrackBoundary(shl.Conv(),vtLoop.GetN()-1,vtLoop);

				if(YSOK==stitcher.StapleMinimumOpenAngleCorner(shl))
				{
					res=YSOK;
				}
			}
		}
	}

	return res;
}

YSRESULT YsShellExtEdge_EdgeStaplingUtil::StapleMinimumOpenAngleCorner(YsShellExtEdit &shl)
{
	YsShellVertexHandle cornerVtHd,connVtHd[2];
	if(YSOK==GetMinimumOpenAngleCorner(cornerVtHd,connVtHd,shl.Conv()))
	{
		// Check again.  This crack may have already been fixed.
		if(YSTRUE!=YsShellExt_TopologyUtil::IsSingleUseEdge(shl.Conv(),cornerVtHd,connVtHd[0]) ||
		   YSTRUE!=YsShellExt_TopologyUtil::IsSingleUseEdge(shl.Conv(),cornerVtHd,connVtHd[1]))
		{
			return YSERR;
		}

		// Either connVtHd[0] must be between connVtHd[1] and cornerVtHd or connVtHd[1] must be between connVtHd[0] and cornerVtHd
		YSBOOL inBetweenCheck=YSFALSE;
		for(int i=0; i<2; ++i)
		{
			const YsVec3 edVtPos[2]=
			{
				shl.GetVertexPosition(connVtHd[1-i]),
				shl.GetVertexPosition(cornerVtHd)
			};
			const YsVec3 tstPos=shl.GetVertexPosition(connVtHd[i]);
			if(YSTRUE==YsCheckInBetween3(tstPos,edVtPos))
			{
				inBetweenCheck=YSTRUE;
				break;
			}
		}
		if(YSTRUE!=inBetweenCheck)
		{
			return YSERR;
		}


		YsShellExtEdit_TopologyUtil topoUtil;

		YsShellVertexHandle edVtHd[2],toInsert;
		if(shl.GetEdgeLength(cornerVtHd,connVtHd[0])<shl.GetEdgeLength(cornerVtHd,connVtHd[1]))
		{
			edVtHd[0]=cornerVtHd;
			edVtHd[1]=connVtHd[1];
			toInsert=connVtHd[0];
		}
		else
		{
			edVtHd[0]=cornerVtHd;
			edVtHd[1]=connVtHd[0];
			toInsert=connVtHd[1];
		}
		YsShellExtEdit_InsertVertexOnEdge(shl,edVtHd,toInsert);
		return UpdateCornerAfterStapling(cornerVtHd,shl.Conv());
	}
	return YSERR;
}
