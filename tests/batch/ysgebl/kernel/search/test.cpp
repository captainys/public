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


const char *concorde_srf="concorde-clean.srf";
const char *bare_unit_cube_srf="bare_unit_cube.srf";
const char *two_volumes_srf="two_volumes.srf";


void TestSingleFile(int &errCode,int &nError,const char fn[])
{
	YsShellExt shl;
	YsShellExtReader shlReader;

	YSRESULT MergeSrf(YsShellExt &shl,YsTextInputStream &inStream);

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
		printf("%d face groups.\n",shl.GetNumFaceGroup());
		printf("%d volumes.\n",shl.GetNumVolume());

		shl.EnableSearch();
		shl.TestSearchTable();
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

