/* ////////////////////////////////////////////////////////////

File Name: ysshellext_clusteringutil.cpp
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

#include "ysshellext_clusteringutil.h"
#include "ysshellext_diagnoseutil.h"

////////////////////////////////////////////////////////////

void YsShellExt_EdgeClusteringUtil::Cluster::CleanUp(void)
{
	edgInCluster.CleanUp();
}

////////////////////////////////////////////////////////////


YsShellExt_EdgeClusteringUtil::YsShellExt_EdgeClusteringUtil()
{
}
YsShellExt_EdgeClusteringUtil::~YsShellExt_EdgeClusteringUtil()
{
}
void YsShellExt_EdgeClusteringUtil::CleanUp(void)
{
	clusterArray.CleanUp();
}
const YsSegmentedArray <YsShellExt_EdgeClusteringUtil::Cluster,YsShellExt_EdgeClusteringUtil::SEGARRAY_BITSHIFT> &YsShellExt_EdgeClusteringUtil::AllCluster(void) const
{
	return clusterArray;
}
void YsShellExt_EdgeClusteringUtil::MakeFromEdgeStore(const YsShell &shl,const YsShellEdgeStore &edgeStore)
{
	CleanUp();

	YsShellVertexAttribTable <YsArray <YsShellVertexHandle> > connVtHdTable(shl);
	for(auto edge : edgeStore)
	{
		for(int i=0; i<2; ++i)
		{
			auto from=edge[i];
			auto to=edge[1-i];
			if(NULL==connVtHdTable[from])
			{
				YsArray <YsShellVertexHandle> t;
				connVtHdTable.SetAttrib(from,t);
			}
			connVtHdTable[from]->Append(to);
		}
	}

	YsShellEdgeStore covered(shl);
	for(auto edge : edgeStore)
	{
		if(YSTRUE!=covered.IsIncluded(edge))
		{
			covered.AddEdge(edge);

			auto &cluster=NewCluster(shl);
			cluster.edgInCluster.AddEdge(edge);

			YsArray <YsShell::Edge> todo;
			todo.Increment();
			todo.Last().edVtHd[0]=edge[0];
			todo.Last().edVtHd[1]=edge[1];
			while(0<todo.GetN())
			{
				auto edge=todo[0];
				for(int i=0; i<2; ++i)
				{
					auto allConnVtHd=*connVtHdTable[edge.edVtHd[i]];
					for(auto conn : allConnVtHd)
					{
						if(YSTRUE!=covered.IsIncluded(conn,edge.edVtHd[i]))
						{
							cluster.edgInCluster.AddEdge(conn,edge.edVtHd[i]);
							covered.AddEdge(conn,edge.edVtHd[i]);

							todo.Increment();
							todo.Last().edVtHd[0]=conn;
							todo.Last().edVtHd[1]=edge.edVtHd[i];
						}
					}
				}

				todo.DeleteBySwapping(0);
			}
		}
	}
}

YsShellExt_EdgeClusteringUtil::Cluster &YsShellExt_EdgeClusteringUtil::NewCluster(const YsShell &shl)
{
	clusterArray.Increment();
	clusterArray.Last().CleanUp();
	clusterArray.Last().edgInCluster.SetShell(shl);
	return clusterArray.Last();
}


////////////////////////////////////////////////////////////


void YsShellExt_PolygonClusteringUtil::Cluster::CleanUp(void)
{
	plgInCluster.CleanUp();
}


////////////////////////////////////////////////////////////


YsShellExt_PolygonClusteringUtil::YsShellExt_PolygonClusteringUtil()
{
}
YsShellExt_PolygonClusteringUtil::~YsShellExt_PolygonClusteringUtil()
{
}
void YsShellExt_PolygonClusteringUtil::CleanUp(void)
{
	clusterArray.CleanUp();
}
const YsSegmentedArray <YsShellExt_PolygonClusteringUtil::Cluster,YsShellExt_PolygonClusteringUtil::SEGARRAY_BITSHIFT> &YsShellExt_PolygonClusteringUtil::AllCluster(void) const
{
	return clusterArray;
}
void YsShellExt_PolygonClusteringUtil::MakeFromPolygonStore(const YsShellExt &shl,const YsShellPolygonStore &plgStore)
{
	YsShellPolygonStore visited(shl.Conv());

	for(auto plHd0 : plgStore)
	{
		if(YSTRUE!=visited.IsIncluded(plHd0))
		{
			YsArray <YsShellPolygonHandle,16> todo;
			visited.AddPolygon(plHd0);

			auto &cluster=NewCluster(shl);
			cluster.plgInCluster.AddPolygon(plHd0);
			todo.Append(plHd0);
			while(0<todo.GetN())
			{
				auto plHd=todo[0];

				const int nPlVt=shl.GetPolygonNumVertex(plHd);
				for(int i=0; i<nPlVt; ++i)
				{
					auto neiPlHd=shl.GetNeighborPolygon(plHd,i);
					if(NULL!=neiPlHd &&
					   YSTRUE!=visited.IsIncluded(neiPlHd) &&
					   YSTRUE==plgStore.IsIncluded(neiPlHd))
					{
						visited.AddPolygon(neiPlHd);
						cluster.plgInCluster.AddPolygon(neiPlHd);
						todo.Append(neiPlHd);
					}
				}

				todo.DeleteBySwapping(0);
			}
		}
	}
}

void YsShellExt_PolygonClusteringUtil::MakeSelfIntersectingPolygonCluster(const YsShellExt &shl,int nLtcCell)
{
	YsShellExt_DiagnoseUtil diagUtil;
	diagUtil.SetShell(shl.Conv());
	diagUtil.SetNumThread(8);
	diagUtil.CheckSelfIntersection();

	YsShellPolygonStore itscPlg(shl.Conv());
	for(auto selfItsc : diagUtil.selfIntersectionArray)
	{
		itscPlg.AddPolygon(selfItsc.plHd);
	}

	MakeFromPolygonStore(shl,itscPlg);
}

YsShellExt_PolygonClusteringUtil::Cluster &YsShellExt_PolygonClusteringUtil::NewCluster(const YsShellExt &shl)
{
	clusterArray.Increment();
	clusterArray.Last().CleanUp();
	clusterArray.Last().plgInCluster.SetShell(shl.Conv());
	return clusterArray.Last();
}
