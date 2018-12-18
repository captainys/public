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

#include <ysport.h>

#include <ysshellext.h>
#include <ysshellextio.h>

#include <stdio.h>


const char *concorde_srf=      "concorde-clean.srf";
const char *bare_unit_cube_srf="bare_unit_cube.srf";
const char *two_volumes_srf=   "two_volumes.srf";


void TestSingleFile(int &errCode,int &nError,const char fn[])
{
	YsShellExt shl;
	YsShellExtReader shlReader;

	YsFileIO::File ifp(fn,"r");
	YsTextFileInputStream inStream=ifp.InStream();
	if(nullptr==ifp || YSOK!=shlReader.MergeSrf(shl,inStream))
	{
		fprintf(stderr,"Cannot read %s\n",fn);
		if(NULL==ifp)
		{
			fprintf(stderr,"Failed to fopen.\n");
		}
		fprintf(stderr,"Error: %s\n",shlReader.GetLastErrorMessage());
		fprintf(stderr,"> %s\n",shlReader.GetLastErrorLine());
		++nError;
		errCode=-1;
	}
	else
	{
		printf("[%s]\n",fn);
		printf("%d vertices.\n",((const YsShell &)shl).GetNumVertex());
		printf("%d polygons.\n",((const YsShell &)shl).GetNumPolygon());
		printf("%d const-edges.\n",shl.GetNumConstEdge());
	}
}

void TestObjReaderWriter(int &errCode,int &nError)
{
	YsShellExt shl;
	YsShell::VertexHandle cubeVtHd[8]=
	{
		shl.AddVertex(YsVec3(0.0,0.0,0.0)),
		shl.AddVertex(YsVec3(1.0,0.0,0.0)),
		shl.AddVertex(YsVec3(1.0,1.0,0.0)),
		shl.AddVertex(YsVec3(0.0,1.0,0.0)),
		shl.AddVertex(YsVec3(0.0,0.0,1.0)),
		shl.AddVertex(YsVec3(1.0,0.0,1.0)),
		shl.AddVertex(YsVec3(1.0,1.0,1.0)),
		shl.AddVertex(YsVec3(0.0,1.0,1.0)),
	};
	YsShell::VertexHandle quadVtHd[6][4]=
	{
		{cubeVtHd[0],cubeVtHd[1],cubeVtHd[2],cubeVtHd[3]},
		{cubeVtHd[1],cubeVtHd[0],cubeVtHd[4],cubeVtHd[5]},
		{cubeVtHd[2],cubeVtHd[1],cubeVtHd[5],cubeVtHd[6]},
		{cubeVtHd[3],cubeVtHd[2],cubeVtHd[6],cubeVtHd[7]},
		{cubeVtHd[0],cubeVtHd[3],cubeVtHd[7],cubeVtHd[4]},
		{cubeVtHd[7],cubeVtHd[6],cubeVtHd[5],cubeVtHd[4]},
	};
	YsShell::PolygonHandle plHd[6][1]=
	{
		{shl.AddPolygon(4,quadVtHd[0])},
		{shl.AddPolygon(4,quadVtHd[1])},
		{shl.AddPolygon(4,quadVtHd[2])},
		{shl.AddPolygon(4,quadVtHd[3])},
		{shl.AddPolygon(4,quadVtHd[4])},
		{shl.AddPolygon(4,quadVtHd[5])},
	};
	shl.AddFaceGroup(1,plHd[0]);
	shl.AddFaceGroup(1,plHd[1]);
	shl.AddFaceGroup(1,plHd[2]);
	shl.AddFaceGroup(1,plHd[3]);
	shl.AddFaceGroup(1,plHd[4]);
	shl.AddFaceGroup(1,plHd[5]);

	YsShellExt::VertexHandle edVtHd[12][2]=
	{
		{cubeVtHd[0],cubeVtHd[1]},
		{cubeVtHd[1],cubeVtHd[2]},
		{cubeVtHd[2],cubeVtHd[3]},
		{cubeVtHd[3],cubeVtHd[0]},
		{cubeVtHd[0],cubeVtHd[4]},
		{cubeVtHd[1],cubeVtHd[5]},
		{cubeVtHd[2],cubeVtHd[6]},
		{cubeVtHd[3],cubeVtHd[7]},
		{cubeVtHd[5],cubeVtHd[4]},
		{cubeVtHd[6],cubeVtHd[5]},
		{cubeVtHd[7],cubeVtHd[6]},
		{cubeVtHd[4],cubeVtHd[7]},
	};
	shl.AddConstEdge(2,edVtHd[ 0],YSFALSE);
	shl.AddConstEdge(2,edVtHd[ 1],YSFALSE);
	shl.AddConstEdge(2,edVtHd[ 2],YSFALSE);
	shl.AddConstEdge(2,edVtHd[ 3],YSFALSE);
	shl.AddConstEdge(2,edVtHd[ 4],YSFALSE);
	shl.AddConstEdge(2,edVtHd[ 5],YSFALSE);
	shl.AddConstEdge(2,edVtHd[ 6],YSFALSE);
	shl.AddConstEdge(2,edVtHd[ 7],YSFALSE);
	shl.AddConstEdge(2,edVtHd[ 8],YSFALSE);
	shl.AddConstEdge(2,edVtHd[ 9],YSFALSE);
	shl.AddConstEdge(2,edVtHd[10],YSFALSE);
	shl.AddConstEdge(2,edVtHd[11],YSFALSE);

	{
		YsFileIO::File ofp("obj.obj","w");
		auto outStream=ofp.OutStream();
		YsShellExtObjWriter writer;
		YsShellExtObjWriter::WriteOption writeOpt;
		writer.WriteObj(outStream,shl,writeOpt);
	}

	{
		YsFileIO::File ifp("obj.obj","r");
		auto inStream=ifp.InStream();
		YsShellExtObjReader reader;
		YsShellExt testShl;
		YsShellExtObjReader::ReadOption readOpt;
		reader.ReadObj(testShl,inStream,readOpt);

		if(shl.GetNumVertex()!=   testShl.GetNumVertex() ||
		   shl.GetNumPolygon()!=  testShl.GetNumPolygon() ||
		   shl.GetNumConstEdge()!=testShl.GetNumConstEdge() ||
		   shl.GetNumFaceGroup()!=testShl.GetNumFaceGroup())
		{
			errCode=-1;
			++nError;
			fprintf(stderr,"ObjReader/ObjWriter error.\n");
			printf("Wrote %d vertices.\n",shl.GetNumVertex());
			printf("Wrote %d polygons.\n",shl.GetNumPolygon());
			printf("Wrote %d face-groups.\n",shl.GetNumFaceGroup());
			printf("Wrote %d const-edges.\n",shl.GetNumConstEdge());
			printf("Read %d vertices.\n",testShl.GetNumVertex());
			printf("Read %d polygons.\n",testShl.GetNumPolygon());
			printf("Read %d face-groups.\n",testShl.GetNumFaceGroup());
			printf("Read %d const-edges.\n",testShl.GetNumConstEdge());
		}
	}
}

int main(void)
{
	YsFileIO::ChDir(YsSpecialPath::GetProgramBaseDirW());

	int errCode=0;
	int nError=0;

	TestSingleFile(errCode,nError,bare_unit_cube_srf);
	TestSingleFile(errCode,nError,two_volumes_srf);
	TestSingleFile(errCode,nError,concorde_srf);
	TestObjReaderWriter(errCode,nError);

	if(0<nError)
	{
		printf("%d errors.\n",nError);
		return 1;
	}
	else
	{
		printf("OK.\n");
		return 0;
	}
}

