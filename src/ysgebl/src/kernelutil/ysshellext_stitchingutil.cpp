/* ////////////////////////////////////////////////////////////

File Name: ysshellext_stitchingutil.cpp
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

#include "ysshellext_stitchingutil.h"
#include "ysshellext_topologyutil.h"
#include "ysshellext_geomcalc.h"


void YsShellExt_StitchingUtil::CleanUp(void)
{
	edVtProx.CleanUp();
	ltc.CleanUp();
}



YSRESULT YsShellExt_StitchingUtil::PrepareForSingleUseEdge(const YsShellExt &shl)
{
	CleanUp();

	int nSingleUse=0;

	YsShellEdgeEnumHandle edHd=NULL;
	while(YSOK==shl.MoveToNextEdge(edHd))
	{
		if(YSTRUE==YsShellExt_TopologyUtil::IsSingleUseEdge(shl,edHd))
		{
			++nSingleUse;
		}
	}

	if(0==nSingleUse)
	{
		// No single-use edge.
		return YSERR;
	}

	YsShellVertexStore singleUseEdgeVtx(shl.Conv());

	ltc.PrepareLattice(shl.Conv(),nSingleUse);
	edHd=NULL;
	while(YSOK==shl.MoveToNextEdge(edHd))
	{
		if(YSTRUE==YsShellExt_TopologyUtil::IsSingleUseEdge(shl,edHd))
		{
			YsShellVertexHandle edVtHd[2];
			shl.GetEdge(edVtHd,edHd);
			ltc.AddEdgePiece(edVtHd,0);

			singleUseEdgeVtx.AddVertex(edVtHd[0]);
			singleUseEdgeVtx.AddVertex(edVtHd[1]);
		}
	}


	edVtProx.SetShell(shl);

	for(auto vtHd : singleUseEdgeVtx)
	{
		if(0<shl.GetNumPolygonUsingVertex(vtHd))
		{
			CalculateAndCacheVertexNearestEdgePair(shl,vtHd);
		}
	}

	return YSOK;
}

YSRESULT YsShellExt_StitchingUtil::FilterBySelfLengthAsThresholdCondition(const YsShellExt &shl)
{
	YsShellExt_EdgeVertexProximityUtil::EDVTPAIR_HANDLE hd;
	YsArray <YsShellExt_EdgeVertexProximityUtil::EDVTPAIR_HANDLE> toDel;

	hd.Nullify();
	for(;;)
	{
		edVtProx.MoveToNextEdgeVertexPair(hd);
		if(hd.IsNull())
		{
			break;
		}

		auto edVtPair=edVtProx.GetEdgeVertexPair(hd);
		const double e1e2=shl.GetEdgeLength(edVtPair.edVtHd);
		if(shl.GetEdgeLength(edVtPair.vtHd,edVtPair.edVtHd[0])>e1e2 ||
		   shl.GetEdgeLength(edVtPair.vtHd,edVtPair.edVtHd[1])>e1e2)
		{
			toDel.Append(hd);
		}
	}
	for(auto hd : toDel)
	{
		edVtProx.DeleteEdgeVertexPair(hd);
	}

	return YSOK;
}

YSRESULT YsShellExt_StitchingUtil::CalculateAndCacheVertexNearestEdgePair(const YsShellExt &shl,YsShellVertexHandle vtHd)
{
	double d;
	YsShellVertexHandle edVtHd[2];
	if(YSOK==edVtProx.FindNearestEdgeFromVertexNotSharingFaceGroup(edVtHd,d,shl,ltc,vtHd))
	{
		edVtProx.AddEdgeVertexPair(edVtHd,vtHd,d);
		return YSOK;
	}
	return YSERR;
}

YsArray <YsShellExt_StitchingUtil::Crack> YsShellExt_StitchingUtil::FindOneToNCrack(const YsShellExt &shl)
{
	// See research note 2014/12/18

	YsArray <Crack> crackArray;
	
	YsShellEdgeEnumHandle edHd=NULL;
	while(YSOK==shl.MoveToNextEdge(edHd))
	{
		if(YSTRUE==YsShellExt_TopologyUtil::IsSingleUseEdge(shl,edHd))
		{
			YsShellVertexHandle edVtHd[2];  // e1-e2
			shl.GetEdge(edVtHd,edHd);

			if(2>YsShellExt_TopologyUtil::GetNumSingleUseEdgeConnectedToVertex(shl,edVtHd[0]) || 
			   2>YsShellExt_TopologyUtil::GetNumSingleUseEdgeConnectedToVertex(shl,edVtHd[1]))
			{
				continue;
			}


			auto edVtPairHd=edVtProx.FindEdgeVertexPairFromEdgePiece(edVtHd);  // This gives v1, v2, ... ,vn

			YsShellVertexStore pathVtx(shl.Conv());
			for(auto hd : edVtPairHd)
			{
				auto edVtPair=edVtProx.GetEdgeVertexPair(hd);
				pathVtx.AddVertex(edVtPair.vtHd);
			}

			YSRESULT res=YSOK;

			YsArray <YsShellVertexHandle> path;
			path.Append(edVtHd[0]);
			while(0<pathVtx.GetN()) // Until reaching edVtHd[1], no more path, or pathVtx is empty
			{
				YsShellVertexHandle nextVtHd=NULL;
				for(auto vtHd : pathVtx)
				{
					if(YSTRUE==YsShellExt_TopologyUtil::IsSingleUseEdge(shl,path.Last(),vtHd))
					{
						nextVtHd=vtHd;
						break;
					}
				}

				if(NULL==nextVtHd)
				{
					res=YSERR;
					break;
				}

				path.Append(nextVtHd);
				pathVtx.DeleteVertex(nextVtHd);
			}
			

			if(YSOK==res && 2<=path.GetN() && YSTRUE==YsShellExt_TopologyUtil::IsSingleUseEdge(shl,path.Last(),edVtHd[1]))
			{
				path.Append(edVtHd[1]);

				for(YSSIZE_T idx=0; idx<path.GetN()-1; ++idx)
				{
					auto reverseCheckHd=edVtProx.FindEdgeVertexPairFromEdgePiece(path[idx],path[idx+1]);
					if(0<reverseCheckHd.GetN())
					{
						goto NEXTEDGE;
					}
				}

				crackArray.Increment();
				crackArray.Last().path[0].Set(2,edVtHd);
				crackArray.Last().path[1]=path;
			}
		}

	NEXTEDGE:
		;
	}

	return crackArray;
}


////////////////////////////////////////////////////////////

void YsShellExt_IsocelesStitchingUtil::CleanUp(void)
{
	toMerge.CleanUp();
}
YSRESULT YsShellExt_IsocelesStitchingUtil::MakeInfo(const YsShellExt &shl)
{
	YsArray <YsVec3> vtPosArray;
	YsArray <YsShellVertexHandle> vtHdArray;

	for(auto vtHd : shl.AllVertex())
	{
		if(0<YsShellExt_TopologyUtil::GetNumSingleUseEdgeConnectedToVertex(shl,vtHd))
		{
			vtHdArray.Add(vtHd);
			vtPosArray.Add(shl.GetVertexPosition(vtHd));
		}
	}

	if(0<vtHdArray.GetN())
	{
		Ys3dTree <YsShellVertexHandle> kdTree;
		YsBuildBalanced3dTree(kdTree,vtPosArray,vtHdArray);

		vtPosArray.ClearDeep();

		for(auto vtHd : vtHdArray)
		{
			auto nearestNeighbor=kdTree.FindNNearest(shl.GetVertexPosition(vtHd),2);
			if(2==nearestNeighbor.GetN())
			{
				YsShellVertexHandle neiVtHd=NULL;
				if(nearestNeighbor[0]->GetAttrib()==vtHd)
				{
					neiVtHd=nearestNeighbor[1]->GetAttrib();
				}
				else if(nearestNeighbor[1]->GetAttrib()==vtHd)
				{
					neiVtHd=nearestNeighbor[0]->GetAttrib();
				}
// #### ç°Ç±Ç±
			}
		}
	}

	return YSOK;
}


////////////////////////////////////////////////////////////


void YsShellExt_EdgeStaplingUtil::CleanUp(void)
{
	openAngleTree.CleanUp();
	vtKeyToTreeNode.CleanUp();
}

void YsShellExt_EdgeStaplingUtil::SetCrackBoundary(const YsShellExt &shl,YSSIZE_T nVt,const YsShellVertexHandle vtHd[])
{
	CleanUp();
	for(YSSIZE_T idx=0; idx<nVt; ++idx)
	{
		const double openAngle=YsPi*2.0-YsShellExt_CalculateTotalFaceAngle(shl,vtHd[idx]);
		auto ndHd=openAngleTree.Insert(openAngle,shl.GetSearchKey(vtHd[idx]));
		vtKeyToTreeNode.AddElement(shl.GetSearchKey(vtHd[idx]),ndHd);

		TwoVertex conn;
		conn.vtKey[0]=shl.GetSearchKey(vtHd[(idx+1)%nVt]);
		conn.vtKey[1]=shl.GetSearchKey(vtHd[(idx+nVt-1)%nVt]);
		vtxConnection.AddElement(shl.GetSearchKey(vtHd[idx]),conn);
	}
}

YSRESULT YsShellExt_EdgeStaplingUtil::GetMinimumOpenAngleCorner(YsShellVertexHandle &cornerVtHd,YsShellVertexHandle connVtHd[2],const YsShellExt &shl) const
{
	auto firstNode=openAngleTree.First();
	if(firstNode.IsNotNull())
	{
		auto openAngle=openAngleTree.GetKey(firstNode);
		auto vtKey=openAngleTree.GetValue(firstNode);

		cornerVtHd=shl.FindVertex(vtKey);
		if(NULL!=cornerVtHd)
		{
			TwoVertex conn;
			if(YSOK==vtxConnection.FindElement(conn,vtKey))
			{
				connVtHd[0]=shl.FindVertex(conn.vtKey[0]);
				connVtHd[1]=shl.FindVertex(conn.vtKey[1]);
				if(NULL!=connVtHd[0] && NULL!=connVtHd[1])
				{
					return YSOK;
				}
			}
		}
	}
	return YSERR;
}

YSRESULT YsShellExt_EdgeStaplingUtil::UpdateCornerAfterStapling(YsShellVertexHandle cornerVtHd,const YsShellExt &shl)
{
	YsAVLTree<double,YSHASHKEY>::NodeHandle nodeToDel;
	TwoVertex conn;
	YsAVLTree<double,YSHASHKEY>::NodeHandle nodeToUpdate[2];
	if(YSOK==vtKeyToTreeNode.FindElement(nodeToDel,shl.GetSearchKey(cornerVtHd)) &&
	   YSOK==vtxConnection.FindElement(conn,shl.GetSearchKey(cornerVtHd)) &&
	   YSOK==vtKeyToTreeNode.FindElement(nodeToUpdate[0],conn.vtKey[0]) &&
	   YSOK==vtKeyToTreeNode.FindElement(nodeToUpdate[1],conn.vtKey[1]))
	{
		const YsShellVertexHandle connVtHd[2]=
		{
			shl.FindVertex(conn.vtKey[0]),
			shl.FindVertex(conn.vtKey[1]),
		};
		TwoVertex reverseConnection[2];

		if(NULL!=connVtHd[0] && 
		   NULL!=connVtHd[1] &&
		   YSOK==vtxConnection.FindElement(reverseConnection[0],conn.vtKey[0]) &&
		   YSOK==vtxConnection.FindElement(reverseConnection[1],conn.vtKey[1]))
		{
			for(int i=0; i<2; ++i)
			{
				if(reverseConnection[i].vtKey[0]==shl.GetSearchKey(cornerVtHd))
				{
					reverseConnection[i].vtKey[0]=conn.vtKey[1-i];
				}
				else if(reverseConnection[i].vtKey[1]==shl.GetSearchKey(cornerVtHd))
				{
					reverseConnection[i].vtKey[1]=conn.vtKey[1-i];
				}
				else
				{
					printf("%s %d\n",__FUNCTION__,__LINE__);
					printf("  Broken connection.\n");
					return YSERR;
				}
			}

			openAngleTree.Delete(nodeToDel);

			if(reverseConnection[0].vtKey[0]==reverseConnection[0].vtKey[1] &&
			   reverseConnection[1].vtKey[0]==reverseConnection[1].vtKey[1])
			{
				// Means the crack has been closed.
				openAngleTree.Delete(nodeToUpdate[0]);
				openAngleTree.Delete(nodeToUpdate[1]);
				vtxConnection.DeleteKey(conn.vtKey[0]);
				vtxConnection.DeleteKey(conn.vtKey[1]);
				return YSOK;
			}

			vtxConnection.UpdateElement(conn.vtKey[0],reverseConnection[0]);
			vtxConnection.UpdateElement(conn.vtKey[1],reverseConnection[1]);

			const double newOpenAngle[2]=
			{
				YsPi-YsShellExt_CalculateTotalFaceAngle(shl,connVtHd[0]),
				YsPi-YsShellExt_CalculateTotalFaceAngle(shl,connVtHd[1])
			};

			openAngleTree.Delete(nodeToUpdate[0]);
			openAngleTree.Delete(nodeToUpdate[1]);

			YsAVLTree<double,YSHASHKEY>::NodeHandle newNode[2]=
			{
				openAngleTree.Insert(newOpenAngle[0],conn.vtKey[0]),
				openAngleTree.Insert(newOpenAngle[1],conn.vtKey[1])
			};

			vtKeyToTreeNode.UpdateElement(conn.vtKey[0],newNode[0]);
			vtKeyToTreeNode.UpdateElement(conn.vtKey[1],newNode[1]);

			return YSOK;
		}
	}
	return YSERR;
}
