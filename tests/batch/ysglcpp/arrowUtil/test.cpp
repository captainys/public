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
#include <ysglcpp.h>

YSRESULT Test(YSBOOL headArrow,YSBOOL backArrow)
{
	YSRESULT res=YSOK;

	YsGLVertexBuffer vtxBuf;
	YsGLNormalBuffer nomBuf;
	YsGLCreateArrowForGL_TRIANGLES(vtxBuf,nomBuf,-YsXYZ(),YsXYZ(),headArrow,backArrow,0.2,0.4,0.7,12);

	if(vtxBuf.GetN()!=nomBuf.GetN())
	{
		fprintf(stderr,"Normal Buffer and Vertex Buffer sizes don't match.\n");
		res=YSERR;
	}
	if(0==vtxBuf.GetN())
	{
		fprintf(stderr,"Empty Vertex Buffer.\n");
		res=YSERR;
	}


	YsShell shl;
	for(int i=0; i<=vtxBuf.GetN()-3; i+=3)
	{
		YsShell::VertexHandle vtHd[3]=
		{
			shl.AddVertexH(YsVec3(vtxBuf[i][0],vtxBuf[i][1],vtxBuf[i][2])),
			shl.AddVertexH(YsVec3(vtxBuf[i+1][0],vtxBuf[i+1][1],vtxBuf[i+1][2])),
			shl.AddVertexH(YsVec3(vtxBuf[i+2][0],vtxBuf[i+2][1],vtxBuf[i+2][2]))
		};
		shl.AddPolygonH(3,vtHd);
	}
	int nDel;
	shl.DeleteVertexAtTheSamePositionByKdTree(nDel);

	YsShellSearchTable search;
	shl.AttachSearchTable(&search);
	YsShellEdgeEnumHandle edHd=nullptr;
	while(YSOK==search.FindNextEdge(shl,edHd))
	{
		if(2!=search.GetNumPolygonUsingEdge(shl,edHd))
		{
			fprintf(stderr,"Arrow not closed!\n");
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
	if(YSOK!=Test(YSTRUE,YSTRUE))
	{
		++nFail;
	}
	if(YSOK!=Test(YSTRUE,YSFALSE))
	{
		++nFail;
	}
	if(YSOK!=Test(YSFALSE,YSTRUE))
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
