/* ////////////////////////////////////////////////////////////

File Name: ysshellext_diagnoseutil.h
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

#ifndef YSSHELLEXT_DIAGNOSEUTIL_IS_INCLUDED
#define YSSHELLEXT_DIAGNOSEUTIL_IS_INCLUDED
/* { */

#include <ysshellext.h>

class YsShellExt_DiagnoseUtil
{
public:
	class NonManifoldEdge
	{
	public:
		YsShellVertexHandle edVtHd[2];
		int nUsed;
	};
	class SelfIntersection
	{
	public:
		YsShellPolygonHandle plHd;
	};
	class NonPlanarPolygon
	{
	public:
		YsShellPolygonHandle plHd;
	};

private:
	int nThread;
	const YsShellExt *shl;
	const YsShellLattice *externalLtc;

public:
	YsArray <NonManifoldEdge> nonManifoldEdgeArray;
	YsArray <SelfIntersection> selfIntersectionArray;
	YsArray <NonPlanarPolygon> nonPlanarPolygonArray;

	int debug_threadId; // For debugging.

	/*! Default constructore.  Nullify shl. */
	YsShellExt_DiagnoseUtil();

	/*! Cleans up the object. */
	void CleanUp(void);

	/*! Set number of threads. */
	void SetNumThread(int nThread);

	/*! This function sets the shell pointer to prepare for diagnostics. */
	void SetShell(const YsShellExt &shl);

	/*! This function gives an external YsShellLattice.  If the lattice is created outside, CheckSelfIntersection can skip lattice creation. */
	void SetLattice(const YsShellLattice &ltc);

	/*! Check for non-manifold edges and populates nonManifoldEdgeArray. */
	void CheckNonManifoldEdge(void);

	/*! Returns the number of non-manifold edges. */
	YSSIZE_T GetNumNonManifoldEdge(void) const;

	/*! Check for self-intersecting polygons and populates selfIntersectionArray. */
	void CheckSelfIntersection(void);
private:
	class CheckSelfIntersection_ThreadInfo;
	static void CheckSelfIntersection_ThreadEntry(CheckSelfIntersection_ThreadInfo *info);
	void CheckSelfIntersection_Thread(CheckSelfIntersection_ThreadInfo &info);

public:
	/*! Check for non-planar polygons and populates nonPlanarPolygonArray. */
	void CheckNonPlanarPolygon(void);
private:
	class CheckNonPlanrPolygon_ThreadInfo;
	void CheckNonPlanrPolygon_Thread(CheckNonPlanrPolygon_ThreadInfo &info);
};

/* } */
#endif
