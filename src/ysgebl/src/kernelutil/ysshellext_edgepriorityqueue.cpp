/* ////////////////////////////////////////////////////////////

File Name: ysshellext_edgepriorityqueue.cpp
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

#include "ysshellext_edgepriorityqueue.h"


void YsShellExt_EdgePriorityQueue::CleanUp(void)
{
	shortEdgeTree.CleanUp();
	edVtKeyToAVLTreeHandle.CleanUpThin();
}

void YsShellExt_EdgePriorityQueue::SetOrderDependency(YSBOOL orderDependency)
{
	edVtKeyToAVLTreeHandle.SetOrderSensitivity(orderDependency);
}

void YsShellExt_EdgePriorityQueue::Add(const YsShellExt &shl,YsShell::Edge edge)
{
	const YSHASHKEY edVtKey[2]={shl.GetSearchKey(edge[0]),shl.GetSearchKey(edge[1])};

	if(nullptr!=edVtKeyToAVLTreeHandle.Find(2,edVtKey))
	{
		return;
	}

	const double cost=CalculateCost(shl,edge);
	auto treeHd=shortEdgeTree.Insert(cost,edge);

	edVtKeyToAVLTreeHandle.Update(2,edVtKey,treeHd);
}

void YsShellExt_EdgePriorityQueue::Delete(const YsShellExt &shl,YsShell::Edge edge)
{
	const YSHASHKEY edVtKey[2]={shl.GetSearchKey(edge[0]),shl.GetSearchKey(edge[1])};

	auto treeHdPtr=edVtKeyToAVLTreeHandle.Find(2,edVtKey);
	if(nullptr!=treeHdPtr)
	{
		auto treeHd=*treeHdPtr;
		auto res=shortEdgeTree.Delete(treeHd);  // Do it before DeleteKey to be safe.
		edVtKeyToAVLTreeHandle.DeleteKey(2,edVtKey);
	}
}

YSSIZE_T YsShellExt_EdgePriorityQueue::GetN(void) const
{
	return shortEdgeTree.GetN();
}

YsShellExt_EdgePriorityQueue::EdgeHandle YsShellExt_EdgePriorityQueue::GetMinimumCostEdge(void) const
{
	auto edHd=shortEdgeTree.First();
	return edHd;
}

YsShell::Edge YsShellExt_EdgePriorityQueue::GetEdge(EdgeHandle edHd) const
{
	return shortEdgeTree.GetValue(edHd);
}

double YsShellExt_EdgePriorityQueue::GetCost(EdgeHandle edHd) const
{
	return shortEdgeTree.GetKey(edHd);
}

////////////////////////////////////////////////////////////

void YsShellExt_ShortEdgePriorityQueue::CleanUp(void)
{
	YsShellExt_EdgePriorityQueue::CleanUp();
}
void YsShellExt_ShortEdgePriorityQueue::Begin(const YsShellExt &shl)
{
	CleanUp();

	YsShellEdgeEnumHandle edHd=nullptr;
	while(YSOK==shl.MoveToNextEdge(edHd))
	{
		auto edge=shl.GetEdge(edHd);
		Add(shl.Conv(),shl.GetEdge(edHd));
	}
}

YsShellExt_ShortEdgePriorityQueue::EdgeHandle YsShellExt_ShortEdgePriorityQueue::GetShortestEdge(void) const
{
	return GetMinimumCostEdge();
}

double YsShellExt_ShortEdgePriorityQueue::GetLength(EdgeHandle edHd) const
{
	return GetCost(edHd);
}

/* virtual */ double YsShellExt_ShortEdgePriorityQueue::CalculateCost(const YsShellExt &shl,YsShell::Edge edge) const
{
	return shl.GetEdgeLength(edge);
}

