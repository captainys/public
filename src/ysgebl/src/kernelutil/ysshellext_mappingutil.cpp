/* ////////////////////////////////////////////////////////////

File Name: ysshellext_mappingutil.cpp
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

#include "ysshellext_mappingutil.h"

YSRESULT YsShellExt_MakeVertexMappingWithTolerance(YsShellExt_VertexToVertexMapping &vtxMap,const YsShellExt &shl,const double tol,const YsShellVertexStore *limitedVertex,const YsShellVertexStore *excludedVertex)
{
	vtxMap.CleanUp();

	for(auto vtHd : shl.AllVertex())
	{
		vtxMap.AddMapping(shl,vtHd,vtHd);
	}

	YsShell3dTree kdTree;
	kdTree.PreAllocateNode(shl.GetNumVertex());  // 2004/06/18
	kdTree.Build(shl.Conv(),limitedVertex,excludedVertex);

	auto nVtx=shl.GetNumVertex();

	YsLoopCounter ctr;
	ctr.BeginCounter(nVtx);
	for(auto vtHdA : shl.AllVertex())
	{
		if((NULL!=limitedVertex && YSTRUE!=limitedVertex->IsIncluded(vtHdA)) ||
		   (NULL!=excludedVertex && YSTRUE==excludedVertex->IsIncluded(vtHdA)))
		{
			continue;
		}

		ctr.Increment();

		YsArray <YsShellVertexHandle> mappedVtHd;
		vtxMap.FindMapping(mappedVtHd,shl,vtHdA);

		if(mappedVtHd[0]==vtHdA)
		{
			auto posA=shl.GetVertexPosition(vtHdA);

			const YsVec3 bbx[2]=
			{
				posA-YsXYZ()*tol,
				posA+YsXYZ()*tol
			};

			YsArray <YsShellVertexHandle,128> vtxList;
			if(YSOK==kdTree.MakeVertexList(vtxList,bbx[0],bbx[1]))
			{
				if(vtxList.GetN()==0)
				{
					YsErrOut("  KdTree seems to be broken.\n");
				}
				else if(vtxList.GetN()>=2)
				{
					YsShellVertexHandle topMostVtHd;
					topMostVtHd=vtxList[0];
					for(YSSIZE_T i=0; i<vtxList.GetN(); i++)
					{
						if(shl.GetSearchKey(vtxList[i])<shl.GetSearchKey(topMostVtHd))
						{
							topMostVtHd=vtxList[i];
						}
					}
					for(auto vtHdB : vtxList)
					{
						vtxMap.DeleteMapping(shl,vtHdB);
						vtxMap.AddMapping(shl,vtHdB,topMostVtHd);
					}
				}
			}
			else
			{
				YsErrOut("  KdTree Search has failed.\n");
			}
		}
	}
	ctr.EndCounter();

	for(auto vtHdA : shl.AllVertex())
	{
		YsShellVertexHandle tracker=vtHdA;

		for(;;)
		{
			YsArray <YsShellVertexHandle> mapped;

			vtxMap.FindMapping(mapped,shl,tracker);
			if(tracker==mapped[0])
			{
				break;
			}

			tracker=mapped[0];
			if(tracker==vtHdA)
			{
				YsErrOut("Internal Error!\nMapping Loop detected!\n");
				break;
			}
		}

		vtxMap.DeleteMapping(shl,vtHdA);
		vtxMap.AddMapping(shl,vtHdA,tracker);
	}

	return YSOK;
}

