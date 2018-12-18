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
#include <ysshellext.h>
#include <ysshellextio.h>


void SetFaceGroupAttrib(YsShellExt &shl,YsShellExt::FaceGroupHandle fgHd,YSBOOL noBubble,YSBOOL noRemesh,YSBOOL isTemporary)
{
	auto fgProp=shl.GetFaceGroupAttrib(fgHd);
	fgProp.SetNoBubble(noBubble);
	fgProp.SetNoRemesh(noRemesh);
	fgProp.SetIsTemporary(isTemporary);
	shl.SetFaceGroupAttrib(fgHd,fgProp);
}

void SetConstEdgeAttrib(YsShellExt &shl,YsShellExt::ConstEdgeHandle ceHd,YSBOOL noBubble,YSBOOL noRemesh,YSBOOL isTemporary)
{
	auto ceProp=shl.GetConstEdgeAttrib(ceHd);
	ceProp.SetNoBubble(noBubble);
	ceProp.SetNoRemesh(noRemesh);
	ceProp.SetIsTemporary(isTemporary);
	shl.SetConstEdgeAttrib(ceHd,ceProp);
}

void CheckFaceGroupAttrib(YSRESULT &res,int &nFail,const YsShellExt &shl,YsShellExt::FaceGroupHandle fgHd,YSBOOL noBubble,YSBOOL noRemesh,YSBOOL isTemporary)
{
	auto fgProp=shl.GetFaceGroupAttrib(fgHd);
	if(fgProp.NoBubble()!=noBubble)
	{
		fprintf(stderr,"NoBubble flag not set on the face group correctly.\n");
		res=YSERR;
		++nFail;
	}
	if(fgProp.NoRemesh()!=noRemesh)
	{
		fprintf(stderr,"NoRemesh flag not set on the face group correctly.\n");
		res=YSERR;
		++nFail;
	}
	if(fgProp.IsTemporary()!=isTemporary)
	{
		fprintf(stderr,"IsTemporary flag not set on the face group correctly.\n");
		res=YSERR;
		++nFail;
	}
}
void CheckConstEdgeAttrib(YSRESULT &res,int &nFail,const YsShellExt &shl,YsShellExt::ConstEdgeHandle ceHd,YSBOOL noBubble,YSBOOL noRemesh,YSBOOL isTemporary)
{
	auto ceProp=shl.GetConstEdgeAttrib(ceHd);
	if(ceProp.NoBubble()!=noBubble)
	{
		fprintf(stderr,"NoBubble flag not set on the const edge correctly.\n");
		res=YSERR;
		++nFail;
	}
	if(ceProp.NoRemesh()!=noRemesh)
	{
		fprintf(stderr,"NoRemesh flag not set on the const edge correctly.\n");
		res=YSERR;
		++nFail;
	}
	if(ceProp.IsTemporary()!=isTemporary)
	{
		fprintf(stderr,"IsTemporary flag not set on the const edge correctly.\n");
		res=YSERR;
		++nFail;
	}
}


YSRESULT Test(void)
{
	int nFail=0;
	YSRESULT res=YSOK;



	YsShellExt shl;
	YsShell::VertexHandle vtHd[8]=
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
		{vtHd[0],vtHd[1],vtHd[2],vtHd[3]},
		{vtHd[4],vtHd[5],vtHd[6],vtHd[7]},
		{vtHd[0],vtHd[1],vtHd[5],vtHd[4]},
		{vtHd[1],vtHd[2],vtHd[6],vtHd[5]},
		{vtHd[2],vtHd[3],vtHd[7],vtHd[6]},
		{vtHd[3],vtHd[0],vtHd[4],vtHd[7]},
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

	YsShellExt::FaceGroupHandle fgHd[6]=
	{
		shl.AddFaceGroup(1,plHd[0]),
		shl.AddFaceGroup(1,plHd[1]),
		shl.AddFaceGroup(1,plHd[2]),
		shl.AddFaceGroup(1,plHd[3]),
		shl.AddFaceGroup(1,plHd[4]),
		shl.AddFaceGroup(1,plHd[5]),
	};

	YsShell::VertexHandle ceVtHd[12][2]=
	{
		vtHd[0],vtHd[1],
		vtHd[1],vtHd[2],
		vtHd[2],vtHd[3],
		vtHd[3],vtHd[0],
		vtHd[4],vtHd[5],
		vtHd[5],vtHd[6],
		vtHd[6],vtHd[7],
		vtHd[7],vtHd[4],
		vtHd[0],vtHd[4],
		vtHd[1],vtHd[5],
		vtHd[2],vtHd[6],
		vtHd[3],vtHd[7],
	};
	YsShellExt::ConstEdgeHandle ceHd[12]=
	{
		shl.AddConstEdge(2,ceVtHd[0],YSFALSE),
		shl.AddConstEdge(2,ceVtHd[1],YSFALSE),
		shl.AddConstEdge(2,ceVtHd[2],YSFALSE),
		shl.AddConstEdge(2,ceVtHd[3],YSFALSE),
		shl.AddConstEdge(2,ceVtHd[4],YSFALSE),
		shl.AddConstEdge(2,ceVtHd[5],YSFALSE),
		shl.AddConstEdge(2,ceVtHd[6],YSFALSE),
		shl.AddConstEdge(2,ceVtHd[7],YSFALSE),
		shl.AddConstEdge(2,ceVtHd[8],YSFALSE),
		shl.AddConstEdge(2,ceVtHd[9],YSFALSE),
		shl.AddConstEdge(2,ceVtHd[10],YSFALSE),
		shl.AddConstEdge(2,ceVtHd[11],YSFALSE),
	};


	SetFaceGroupAttrib(shl,fgHd[0],YSTRUE, YSFALSE,YSFALSE);
	SetFaceGroupAttrib(shl,fgHd[1],YSFALSE,YSTRUE, YSFALSE);
	SetFaceGroupAttrib(shl,fgHd[2],YSTRUE, YSTRUE, YSFALSE);
	SetFaceGroupAttrib(shl,fgHd[3],YSTRUE, YSFALSE,YSTRUE );
	SetFaceGroupAttrib(shl,fgHd[4],YSTRUE, YSFALSE,YSTRUE );
	SetFaceGroupAttrib(shl,fgHd[5],YSFALSE,YSTRUE, YSTRUE );

	SetConstEdgeAttrib(shl,ceHd[0],YSTRUE, YSFALSE,YSFALSE);
	SetConstEdgeAttrib(shl,ceHd[1],YSFALSE,YSTRUE, YSFALSE);
	SetConstEdgeAttrib(shl,ceHd[2],YSTRUE, YSTRUE, YSFALSE);
	SetConstEdgeAttrib(shl,ceHd[3],YSTRUE, YSFALSE,YSTRUE );
	SetConstEdgeAttrib(shl,ceHd[4],YSTRUE, YSFALSE,YSTRUE );
	SetConstEdgeAttrib(shl,ceHd[5],YSFALSE,YSTRUE, YSTRUE );
	SetConstEdgeAttrib(shl,ceHd[6],YSTRUE ,YSTRUE, YSTRUE );
	SetConstEdgeAttrib(shl,ceHd[7],YSFALSE,YSFALSE,YSFALSE);

	CheckFaceGroupAttrib(res,nFail,shl,fgHd[0],YSTRUE, YSFALSE,YSFALSE);
	CheckFaceGroupAttrib(res,nFail,shl,fgHd[1],YSFALSE,YSTRUE, YSFALSE);
	CheckFaceGroupAttrib(res,nFail,shl,fgHd[2],YSTRUE, YSTRUE, YSFALSE);
	CheckFaceGroupAttrib(res,nFail,shl,fgHd[3],YSTRUE, YSFALSE,YSTRUE );
	CheckFaceGroupAttrib(res,nFail,shl,fgHd[4],YSTRUE, YSFALSE,YSTRUE );
	CheckFaceGroupAttrib(res,nFail,shl,fgHd[5],YSFALSE,YSTRUE, YSTRUE );

	CheckConstEdgeAttrib(res,nFail,shl,ceHd[0],YSTRUE, YSFALSE,YSFALSE);
	CheckConstEdgeAttrib(res,nFail,shl,ceHd[1],YSFALSE,YSTRUE, YSFALSE);
	CheckConstEdgeAttrib(res,nFail,shl,ceHd[2],YSTRUE, YSTRUE, YSFALSE);
	CheckConstEdgeAttrib(res,nFail,shl,ceHd[3],YSTRUE, YSFALSE,YSTRUE );
	CheckConstEdgeAttrib(res,nFail,shl,ceHd[4],YSTRUE, YSFALSE,YSTRUE );
	CheckConstEdgeAttrib(res,nFail,shl,ceHd[5],YSFALSE,YSTRUE, YSTRUE );
	CheckConstEdgeAttrib(res,nFail,shl,ceHd[6],YSTRUE ,YSTRUE, YSTRUE );
	CheckConstEdgeAttrib(res,nFail,shl,ceHd[7],YSFALSE,YSFALSE,YSFALSE);


	{
		remove("verify.srf");
		YsShellExtWriter writer;
		writer.SaveSrf("verify.srf",shl);
	}

	{
		YsFileIO::File ifp("verify.srf","r");
		YsTextFileInputStream inStream=ifp.InStream();
		YsShellExt tstShl;
		YsShellExtReader shlReader;
		if(nullptr==ifp || YSOK!=shlReader.MergeSrf(tstShl,inStream))
		{
			fprintf(stderr,"Cannot read verify.srf\n");
			if(NULL==ifp)
			{
				fprintf(stderr,"Failed to fopen.\n");
			}
			return YSERR;
		}

		CheckFaceGroupAttrib(res,nFail,tstShl,fgHd[0],YSTRUE, YSFALSE,YSFALSE);
		CheckFaceGroupAttrib(res,nFail,tstShl,fgHd[1],YSFALSE,YSTRUE, YSFALSE);
		CheckFaceGroupAttrib(res,nFail,tstShl,fgHd[2],YSTRUE, YSTRUE, YSFALSE);
		CheckFaceGroupAttrib(res,nFail,tstShl,fgHd[3],YSTRUE, YSFALSE,YSTRUE );
		CheckFaceGroupAttrib(res,nFail,tstShl,fgHd[4],YSTRUE, YSFALSE,YSTRUE );
		CheckFaceGroupAttrib(res,nFail,tstShl,fgHd[5],YSFALSE,YSTRUE, YSTRUE );

		CheckConstEdgeAttrib(res,nFail,tstShl,ceHd[0],YSTRUE, YSFALSE,YSFALSE);
		CheckConstEdgeAttrib(res,nFail,tstShl,ceHd[1],YSFALSE,YSTRUE, YSFALSE);
		CheckConstEdgeAttrib(res,nFail,tstShl,ceHd[2],YSTRUE, YSTRUE, YSFALSE);
		CheckConstEdgeAttrib(res,nFail,tstShl,ceHd[3],YSTRUE, YSFALSE,YSTRUE );
		CheckConstEdgeAttrib(res,nFail,tstShl,ceHd[4],YSTRUE, YSFALSE,YSTRUE );
		CheckConstEdgeAttrib(res,nFail,tstShl,ceHd[5],YSFALSE,YSTRUE, YSTRUE );
		CheckConstEdgeAttrib(res,nFail,tstShl,ceHd[6],YSTRUE ,YSTRUE, YSTRUE );
		CheckConstEdgeAttrib(res,nFail,tstShl,ceHd[7],YSFALSE,YSFALSE,YSFALSE);
	}



	if(YSOK!=res)
	{
		fprintf(stderr,"Failed!\n");
	}
	return res;
}

int main(void)
{
	YsFileIO::ChDir(YsSpecialPath::GetProgramBaseDirW());

	int nFail=0;
	if(YSOK!=Test())
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
