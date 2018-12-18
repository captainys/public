/* ////////////////////////////////////////////////////////////

File Name: ysglbuffer.cpp
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


#include "ysglbuffer.h"

YSRESULT YsGLVertexBuffer::AddArrowHeadTriangle(const YsVec3 &p0,const YsVec3 &p1,const double arrowSize)
{
	YsVec3 edVec=p1-p0;
	const double edLen=edVec.GetLength();
	if(YsTolerance<edLen)
	{
		const double arrowLen=edLen*arrowSize;

		edVec/=edLen;
		YsVec3 uVec=YsUnitVector(edVec.GetArbitraryPerpendicularVector());
		YsVec3 vVec=edVec^uVec;

		uVec*=arrowLen*0.5;
		vVec*=arrowLen*0.5;

		const YsVec3 o=p1-edVec*arrowLen;

		AddVertex(o+uVec);
		AddVertex(o-uVec);
		AddVertex(p1);

		AddVertex(o+vVec);
		AddVertex(o-vVec);
		AddVertex(p1);

		return YSOK;
	}
	return YSERR;
}

YSRESULT YsGLVertexBuffer::AddArrowHeadTriangle(const YsVec3 p[2],const double arrowSize)
{
	return AddArrowHeadTriangle(p[0],p[1],arrowSize);
}



template <class ComponentType>
void YsGLPrimitiveSequence<ComponentType>::CleanUp(void)
{
	primTypeAndNumVtx.CleanUp();
	coord.CleanUp();
	color.CleanUp();
}

