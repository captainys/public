/* ////////////////////////////////////////////////////////////

File Name: ysglcascadedshadowmap.h
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

#ifndef YSGLCASCADEDSHADOWMAP_IS_INCLUDED
#define YSGLCASCADEDSHADOWMAP_IS_INCLUDED
/* { */

#include <ysclass.h>

class YsGLCascadedShadowMap
{
public:
	/*! Orthogonal shadow frustum extends from:
	    (-dx,-dy,0)-(dx,dy,farz)
	    Z direction is taken in the viewVec.
	*/
	class OrthogonalShadowFrustum
	{
	public:
		YsVec3 viewPoint;
		YsVec3 viewVec,upVec;
		double dx,dy,farz;
	};

protected:
	YsArray <OrthogonalShadowFrustum,4> orthoShadowFrustum;

public:
	YsGLCascadedShadowMap();
	~YsGLCascadedShadowMap();
	void CleanUp(void);

	/*! Caculate 2-level light frustums of of cascaded shadow map
	*/
	void CalculateShadowFrustum(
	    const YsMatrix4x4 &viewTfm,const YsMatrix4x4 &projTfm,
	    const YsVec3 &dirToLight,
	    const double box0size,const double box1size);
};

/* } */
#endif
