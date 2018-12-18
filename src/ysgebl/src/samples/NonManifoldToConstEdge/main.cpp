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

int main(int argc,char *argv[])
{
	if(3<=argc)
	{
		YsShellExt shl;

		YsFileIO::File ifp(argv[1],"r");
		if(nullptr!=ifp)
		{
			auto inStream=ifp.InStream();
			YsShellExtReader reader;
			reader.MergeSrf(shl,inStream);
		}
		else
		{
			fprintf(stderr,"Cannot open input file.\n");
			return 1;
		}

		shl.EnableSearch();
		for(auto ceHd : shl.AllConstEdge())
		{
			shl.DeleteConstEdge(ceHd);
		}
		YsShellEdgeEnumHandle edHd=nullptr;
		while(YSOK==shl.MoveToNextEdge(edHd))
		{
			auto edge=shl.GetEdge(edHd);
			if(2!=shl.GetNumPolygonUsingEdge(edge))
			{
				shl.AddConstEdge(2,edge,YSFALSE);
			}
		}

		for(auto plHd : shl.AllPolygon())
		{
			shl.ForceDeletePolygon(plHd);
		}

		YsFileIO::File ofp(argv[2],"w");
		if(nullptr!=ofp)
		{
			auto outStream=ofp.OutStream();
			YsShellExtWriter writer;
			writer.SaveSrf(outStream,shl);
		}
		else
		{
			fprintf(stderr,"Cannot open file for output.\n");
		}
	}
	return 0;
}
