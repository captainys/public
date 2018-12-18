/* ////////////////////////////////////////////////////////////

File Name: ysshellext_embeddingutil.h
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

#ifndef YSSHELLEXT_EMBEDDINGUTIL_IS_INCLUDED
#define YSSHELLEXT_EMBEDDINGUTIL_IS_INCLUDED
/* { */

#include <ysclass.h>

class YsShellExt_EmbeddingInfo
{
public:
	enum VV_TYPE
	{
		VV_NULL,
		VV_ON_VERTEX,
		VV_ON_EDGE,
		VV_ON_POLYGON,
		VV_CANNOT_PROJECT
	};

	class VirtualVertex
	{
	public:
		VV_TYPE vvType;
		YsVec3 fromPos,projPos;
		union
		{
			YSHASHKEY vtKey;
			YSHASHKEY plKey;
			YSHASHKEY edVtKey[2];
		};
	};

	class VirtualEdge
	{
	public:
	};

	// One polygon can be split into more than two polygons.

	// Steps:
	//   (1) Find projection
	//   (2) Add necessary on-edge virtual vertices
	//   (3) Make virtual-vertex connection, or virtual edge.

	// No, I only need to embed the boundary.
};



/* } */
#endif
