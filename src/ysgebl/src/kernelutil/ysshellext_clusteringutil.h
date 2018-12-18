/* ////////////////////////////////////////////////////////////

File Name: ysshellext_clusteringutil.h
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

#ifndef YSSHELLEXT_CLUSTERINGUTIL_IS_INCLUDED
#define YSSHELLEXT_CLUSTERINGUTIL_IS_INCLUDED
/* { */

#include <ysshellext.h>


class YsShellExt_ClusteringUtil
{
public:
	enum
	{
		SEGARRAY_BITSHIFT=4
	};
};


class YsShellExt_EdgeClusteringUtil : public YsShellExt_ClusteringUtil
{
public:
	class Cluster
	{
	public:
		YsShellEdgeStore edgInCluster;
		void CleanUp(void);
	};

private:
	YsSegmentedArray <Cluster,SEGARRAY_BITSHIFT> clusterArray;

public:
	YsShellExt_EdgeClusteringUtil();
	~YsShellExt_EdgeClusteringUtil();
	void CleanUp(void);

	const YsSegmentedArray <Cluster,SEGARRAY_BITSHIFT> &AllCluster(void) const;
	void MakeFromEdgeStore(const YsShell &shl,const YsShellEdgeStore &edgeStore);

private:
	Cluster &NewCluster(const YsShell &shl);
};

class YsShellExt_PolygonClusteringUtil : public YsShellExt_ClusteringUtil
{
public:
	class Cluster
	{
	public:
		YsShellPolygonStore plgInCluster;
		void CleanUp(void);
	};

private:
	YsSegmentedArray <Cluster,SEGARRAY_BITSHIFT> clusterArray;

public:
	YsShellExt_PolygonClusteringUtil();
	~YsShellExt_PolygonClusteringUtil();
	void CleanUp(void);

	const YsSegmentedArray <Cluster,SEGARRAY_BITSHIFT> &AllCluster(void) const;

	void MakeFromPolygonStore(const YsShellExt &shl,const YsShellPolygonStore &plgStore);

	void MakeSelfIntersectingPolygonCluster(const YsShellExt &shl,int nLtcCell=0);

private:
	Cluster &NewCluster(const YsShellExt &shl);
};


/* } */
#endif
