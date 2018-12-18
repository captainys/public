/* ////////////////////////////////////////////////////////////

File Name: main.cpp
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

#include <stdio.h>
#include <ysport.h>
#include <ysshellext.h>
#include <ysshellextio.h>
#include <ysshellextedit_boolutil.h>

int main(int argc,char *argv[])
{
	if(4<=argc)
	{
		YsShellExtEdit shl[2];

		YsFileIO::File ifp0(argv[1],"r");
		if(nullptr!=ifp0)
		{
			auto inStream=ifp0.InStream();
			shl[0].LoadSrf(inStream);
		}
		else
		{
			fprintf(stderr,"Cannot open input file.\n");
			return 1;
		}
		YsFileIO::File ifp1(argv[2],"r");
		if(nullptr!=ifp1)
		{
			auto inStream=ifp1.InStream();
			YsShellExtReader reader;
			shl[1].LoadSrf(inStream);
		}
		else
		{
			fprintf(stderr,"Cannot open input file.\n");
			return 1;
		}


		shl[0].EnableSearch();
		shl[1].EnableSearch();
		shl[0].DisableEditLog();
		shl[1].DisableEditLog();


		YsShellExtEdit_BooleanUtil boolUtil;
		boolUtil.SetShellAndOpType(shl[0],shl[1],YSBOOLMINUS);
		boolUtil.Run();

		boolUtil.MakeVertexMapping(0);  // Also tentatively mark vertices from the other shell as 'used'.
		boolUtil.CacheUnusedPolygon(0);
		boolUtil.TransferRemainingPartOfConstEdge(0);
		boolUtil.ReconstructConstEdge(0);
		boolUtil.CombineConstEdgeOfSameOrigin(0);
		boolUtil.ConstrainBetweenInputShell(0);
		boolUtil.DeleteTemporaryVertex(0);
		boolUtil.MergePolygonFromSameOriginLeavingOneShortestEdgePerVertex(0);
		boolUtil.AddPolygonFromTheOtherShell(0);
		boolUtil.ModifyShell(0);
		boolUtil.ApplyConstEdgeChange(0);
		boolUtil.DeleteCachedUnusedPolygon(0);
		boolUtil.DeleteUsedVertexTurnedUnused(0);


		YsFileIO::File ofp(argv[3],"w");
		if(nullptr!=ofp)
		{
			auto outStream=ofp.OutStream();
			YsShellExtWriter writer;
			writer.SaveSrf(outStream,shl[0].Conv());
		}
		else
		{
			fprintf(stderr,"Cannot open file for output.\n");
		}
	}
	return 0;
}
