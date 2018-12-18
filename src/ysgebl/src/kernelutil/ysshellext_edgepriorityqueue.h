/* ////////////////////////////////////////////////////////////

File Name: ysshellext_edgepriorityqueue.h
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

#ifndef YSSHELLEXT_EDGEPRIORITYQUEUE_IS_INCLUDED
#define YSSHELLEXT_EDGEPRIORITYQUEUE_IS_INCLUDED
/* { */

#include <ysshellext.h>



class YsShellExt_EdgePriorityQueue
{
public:
	typedef YsAVLTree <double,YsShell::Edge>::NodeHandle EdgeHandle;

protected:
	YsAVLTree <double,YsShell::Edge> shortEdgeTree;
	YsFixedLengthHashTable <EdgeHandle,2> edVtKeyToAVLTreeHandle;

public:
	void CleanUp(void);

	/*! If set to YSTRUE, edges A->B and B->A are considered different. 
	    Otherwise, edges A->B and B->A are considered equal. */
	void SetOrderDependency(YSBOOL orderDependency);

	/*! Adds an edge.  If the edge is already in the priority queue, it does nothing. 
	    CAUTION!  It won't re-calculate the cost if the edge is already in the priority queue.
	*/
	void Add(const YsShellExt &shl,YsShell::Edge edge);

	/*! Deletes an edge.  If the edge is not in the priority queue, it does nothing.
	*/
	void Delete(const YsShellExt &shl,YsShell::Edge edge);

	YSSIZE_T GetN(void) const;

	EdgeHandle GetMinimumCostEdge(void) const;
	YsShell::Edge GetEdge(EdgeHandle edHd) const;
	double GetCost(EdgeHandle edHd) const;

	virtual double CalculateCost(const YsShellExt &shl,YsShell::Edge edge) const=0;
};



class YsShellExt_ShortEdgePriorityQueue : public YsShellExt_EdgePriorityQueue
{
public:
	void CleanUp(void);
	void Begin(const YsShellExt &shl);

	EdgeHandle GetShortestEdge(void) const;
	double GetLength(EdgeHandle edHd) const;

	virtual double CalculateCost(const YsShellExt &shl,YsShell::Edge edge) const;
};


/* } */
#endif
