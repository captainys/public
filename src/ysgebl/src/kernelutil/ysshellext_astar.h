/* ////////////////////////////////////////////////////////////

File Name: ysshellext_astar.h
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

#ifndef YSSHELLEXT_ASTAR_IS_INCLUDED
#define YSSHELLEXT_ASTAR_IS_INCLUDED
/* { */

#include <ysshellext.h>

class YsShellExt_AstarSearch
{
private:
	YsShellVertexAttribTable <double> f_score;
	YsShellVertexAttribTable <double> g_score;

	class OpenSet
	{
	public:
		YsShellVertexAttribTable <YsAVLTree <double,YsShell::VertexHandle>::NodeHandle> vtHdToAVLTreeNode;
		YsAVLTree <double,YsShell::VertexHandle> minimumFScoreFinder;

		YSSIZE_T GetN(void) const;
		void SetShell(const YsShell &shl);
		void Update(YsShell::VertexHandle vtHd,const double fScore);
		void DeleteFirst(void);
		YSBOOL IsIncluded(YsShell::VertexHandle vtHd) const;
		YsShell::VertexHandle MinimumFScoreVtHd(void) const;
	};

public:
	YsArray <YsShell::VertexHandle> FindShortestPath(const YsShellExt &shl,YsShell::VertexHandle startVtHd,YsShell::VertexHandle goalVtHd);

	virtual double HeuristicCostEstimate(const YsShellExt &shl,YsShell::VertexHandle fromVtHd,YsShell::VertexHandle toVtHd) const;
	virtual YsArray <YsShell::VertexHandle> GetNeighbor(const YsShellExt &shl,YsShell::VertexHandle fromVtHd) const;
	virtual double GetStepDist(const YsShellExt &shl,YsShell::VertexHandle fromVtHd,YsShell::VertexHandle toVtHd) const;
	double FScore(const YsShellExt &shl,YsShell::VertexHandle vtHd) const;
	double GScore(const YsShellExt &shl,YsShell::VertexHandle vtHd) const;
};



class YsShellExt_AstarSearch_Polygon
{
private:
	YsShellPolygonAttribTable <double> f_score;
	YsShellPolygonAttribTable <double> g_score;

	class OpenSet
	{
	public:
		YsShellPolygonAttribTable <YsAVLTree <double,YsShell::PolygonHandle>::NodeHandle> plHdToAVLTreeNode;
		YsAVLTree <double,YsShell::PolygonHandle> minimumFScoreFinder;

		YSSIZE_T GetN(void) const;
		void SetShell(const YsShell &shl);
		void Update(YsShell::PolygonHandle plHd,const double fScore);
		void DeleteFirst(void);
		YSBOOL IsIncluded(YsShell::PolygonHandle plHd) const;
		YsShell::PolygonHandle MinimumFScorePlHd(void) const;
	};

public:
	YsArray <YsShell::PolygonHandle> FindShortestPath(const YsShellExt &shl,YsShell::PolygonHandle start,YsShell::PolygonHandle goal);

	virtual double HeuristicCostEstimate(const YsShellExt &shl,YsShell::PolygonHandle from,YsShell::PolygonHandle to) const;
	virtual YsArray <YsShell::PolygonHandle> GetNeighbor(const YsShellExt &shl,YsShell::PolygonHandle from) const;
	virtual double GetStepDist(const YsShellExt &shl,YsShell::PolygonHandle from,YsShell::PolygonHandle to) const;
	double FScore(const YsShellExt &shl,YsShell::PolygonHandle vtHd) const;
	double GScore(const YsShellExt &shl,YsShell::PolygonHandle vtHd) const;
};


/* } */
#endif
