/* ////////////////////////////////////////////////////////////

File Name: test.cpp
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

#include <ysclass.h>
#include <ysport.h>
#include <ysspecialpath.h>
#include <ysshellextio.h>
#include <ysshellext_patchutil.h>
#include <ysshellext_geomcalc.h>

YSRESULT TestRotationalSweep(void)
{
	YSRESULT res=YSOK;


	YsWString fn;
	fn.MakeFullPathName(YsSpecialPath::GetProgramBaseDirW(),L"rotational_sweep.srf");


	YsShellExt shl;
	YsFileIO::File fp(fn,"r");
	auto inStream=fp.InStream();
	if(nullptr==fp || YSOK!=shl.LoadSrf(inStream))
	{
		fprintf(stderr,"Cannot open rotational_sweep.srf\n");
		return YSERR;
	}

	YsShellPolygonStore orgPlHd(shl.Conv());
	for(auto plHd : shl.AllPolygon())
	{
		orgPlHd.Add(plHd);
	}

	shl.Encache();

	auto plHd5=shl.GetPolygonHandleFromId(5);
	auto plHd4=shl.GetPolygonHandleFromId(4);
	auto ceHd7=shl.GetConstEdgeHandleFromIndex(7);

	auto plHd1=shl.GetPolygonHandleFromId(1);
	auto plHd2=shl.GetPolygonHandleFromId(2);
	auto plHd0=shl.GetPolygonHandleFromId(0);

	auto ceHd0=shl.GetConstEdgeHandleFromIndex(0);
	auto ceHd2=shl.GetConstEdgeHandleFromIndex(2);
	auto ceHd4=shl.GetConstEdgeHandleFromIndex(4);

	auto plHd3=shl.GetPolygonHandleFromId(3);
	auto ceHd5=shl.GetConstEdgeHandleFromIndex(5);
	auto ceHd6=shl.GetConstEdgeHandleFromIndex(6);

	// PlId 5->PlId 4, CeId=7
	if(nullptr==plHd5 || nullptr==plHd4 || nullptr==ceHd7)
	{
		fprintf(stderr,"Missing polygon or const edge.\n");
		return YSERR;
	}
	{
		YsShellExt_MultiPatchSequence multiPatch;
		YsArray <YsShell::VertexHandle> plVtHd0=shl.GetPolygonVertex(plHd5);
		YsArray <YsShell::VertexHandle> plVtHd1=shl.GetPolygonVertex(plHd4);
		auto path=shl.GetConstEdgeVertex(ceHd7);
		if(YSOK!=YsExtractNonSharedPart<YsShell::VertexHandle>(plVtHd0,plVtHd1,plVtHd0,YSTRUE,plVtHd1,YSTRUE))
		{
			fprintf(stderr,"Failed to extract common edge(s) of the polygons\n");
			return YSERR;
		}
		multiPatch.SetUpFromFirstLastCrossSectionAndPathBetween(shl,plVtHd0,plVtHd1,path);
		multiPatch.CreateMissingVertexInOriginalShell(shl);
		multiPatch.TransferPatchToOriginalShell(shl,YsBlue());
	}
	{
		YsShellExt_MultiPatchSequence multiPatch;
		YsArray <YsShell::PolygonHandle> plHd;
		plHd.push_back(plHd1);
		plHd.push_back(plHd2);
		plHd.push_back(plHd0);
		multiPatch.SetUpFromPolygon(shl,plHd);	
		multiPatch.CreateMissingVertexInOriginalShell(shl);
		multiPatch.TransferPatchToOriginalShell(shl,YsBlue());
	}
	{
		YsShellExt_MultiPatchSequence multiPatch;
		YsArray <YsShell::VertexHandle> ceVtHd0=shl.GetConstEdgeVertex(ceHd0);
		YsArray <YsShell::VertexHandle> ceVtHd1=shl.GetConstEdgeVertex(ceHd2);
		auto path=shl.GetConstEdgeVertex(ceHd4);
		if(YSOK!=YsExtractNonSharedPart<YsShell::VertexHandle>(ceVtHd0,ceVtHd1,ceVtHd0,YSFALSE,ceVtHd1,YSFALSE))
		{
			fprintf(stderr,"Failed to extract common edge(s) of the polygons\n");
			return YSERR;
		}
		multiPatch.SetUpFromFirstLastCrossSectionAndPathBetween(shl,ceVtHd0,ceVtHd1,path);
		multiPatch.CreateMissingVertexInOriginalShell(shl);
		multiPatch.TransferPatchToOriginalShell(shl,YsBlue());
	}
	{
		YsShellExt_MultiPatchSequence multiPatch;
		YsArray <YsShell::VertexHandle> plVtHd0=shl.GetPolygonVertex(plHd3);
		YsArray <YsShell::VertexHandle> ceVtHd1=shl.GetConstEdgeVertex(ceHd5);
		auto path=shl.GetConstEdgeVertex(ceHd6);
		if(YSOK!=YsExtractNonSharedPart<YsShell::VertexHandle>(plVtHd0,ceVtHd1,plVtHd0,YSTRUE,ceVtHd1,YSFALSE))
		{
			fprintf(stderr,"Failed to extract common edge(s) of the polygons\n");
			return YSERR;
		}
		multiPatch.SetUpFromFirstLastCrossSectionAndPathBetween(shl,plVtHd0,ceVtHd1,path);
		multiPatch.CreateMissingVertexInOriginalShell(shl);
		multiPatch.TransferPatchToOriginalShell(shl,YsBlue());
	}

	YsShellExtWriter writer;
	writer.SaveSrf("result_rotational_sweep.srf",shl);

	if(148!=shl.GetNumVertex() || 202!=shl.GetNumPolygon())
	{
		fprintf(stderr,"Vertex count/Polygon count is not as predicted.\n");
		res=YSERR;
	}
	if(YSTRUE!=YsEqual(996.302880,shl.ComputeTotalArea()))
	{
		fprintf(stderr,"Total area is not as predicted.\n");
		res=YSERR;
	}
	for(auto plHd : shl.AllPolygon())
	{
		auto nom=shl.GetNormal(plHd);
		if(YSTRUE!=orgPlHd.IsIncluded(plHd) && nom==YsVec3::Origin())
		{
			fprintf(stderr,"New polygon does not have an assigned normal.\n");
			res=YSERR;
		}
	}

	if(YSOK!=res)
	{
		fprintf(stderr,"Failed!\n");
	}
	return res;
}

int main(void)
{
	int nFail=0;
	if(YSOK!=TestRotationalSweep())
	{
		++nFail;
	}

	printf("%d failed.\n",nFail);
	if(0<nFail)
	{
		return 1;
	}
	return 0;
}
