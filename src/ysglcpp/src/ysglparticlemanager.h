/* ////////////////////////////////////////////////////////////

File Name: ysglparticlemanager.h
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

#ifndef YSGLPARTICLEMANAGER_IS_INCLUDED
#define YSGLPARTICLEMANAGER_IS_INCLUDED
/* { */

#include "ysglbuffer.h"

class YsGLParticleManager
{
public:
	class Particle
	{
	public:
		YsVec3 pos;
		YsColor col;
		float depth;
		float dimension;
		float texCoord[2];
	};

	YsGLVertexBuffer triVtxBuf;
	YsGLVertexBuffer2D triTexCoordBuf;
	YsGLColorBuffer triColBuf;

	YsGLVertexBuffer pntVtxBuf;
	YsGLVertexBuffer2D pntTexCoordBuf;
	YsGLColorBuffer pntColBuf;
	YsGLPointSizeBuffer pntSizeBuf;

private:
	YsArray <Particle> particle;
	YsArray <YSSIZE_T> particleOrder;
	YsArray <double> particleDist;

public:
	YsGLParticleManager();
	~YsGLParticleManager();
	void CleanUp(void);

	YSSIZE_T GetNumParticle(void) const;

	/*! Adds a particle.  It assumes that each particle takes a constant rectangular space in the
	    texture coordinate.  That is, the pattern is (sMin,tMin)-(sMin+sWid,tMin+tWid) of the current 
	    texture.  In OpenGL 2.0 wrapper, it is specified by SetPointSpriteTextureCoordRange function.
	*/
	inline void Add(const YsVec3 &pos,YsColor col,float dim,float sMin,float tMin)
	{
		particle.Increment();
		particle.Last().pos=pos;
		particle.Last().col=col;
		particle.Last().dimension=dim;
		particle.Last().depth=0.0;
		particle.Last().texCoord[0]=sMin;
		particle.Last().texCoord[1]=tMin;
	}
	inline void Add(const YsVec3 &pos,YsColor col,float dim,YsVec2 &texMin)
	{
		Add(pos,col,dim,texMin.xf(),texMin.yf());
	}
	inline void Add(const YsVec3 &pos,YsColor col,float dim,const float texCoord[2])
	{
		Add(pos,col,dim,texCoord[0],texCoord[1]);
	}

private:
	void CalculateDotProd(YSSIZE_T i0,YSSIZE_T i1,const YsVec3 &viewPos,const YsVec3 & viewDir);

public:
	/*! Sort particles in the given view direction.  Technically, view point is not necessary,
	    but probably it is good for reducing artifacts by the numerical error.
	*/
	void Sort(const YsVec3 &viewPos,const YsVec3 & viewDir);

	/*! Sort particles in parallel using the given threadPool.
	*/
	void Sort(const YsVec3 &viewPos,const YsVec3 &viewDir,class YsThreadPool &thrPool);

	/*! Call this function after Sort to populate vertex attribute buffers for point sprite.
	    This function will populate pntVtxBuf, pntTexCoordBuf, pntColBuf, and pntSizeBuf.

	    OpenGL has a rediculously small max point-sprite size, which typically is 64 pixels.
	    Therefore particles close to the viewpoint cannot be drawn as a point-sprite.
	    The idea of minZ is for using point-sprite for far-away particles and use triangles for closer particles.
	*/
	void MakeBufferForPointSprite(double minZ);

	/*! Call this function after Sort.  This function will populate triVtxBuf,triTexCoordBuf, and triColBuf.
	    The vertex arrays in the buffers can be given to GL_TRIANGLES.
		This function assumes that a sprite takes a square in the texture coordinate (t0,s0)-(t0+spriteTexCoordRange,s0+spriteTexCoordRange).

	    OpenGL has a rediculously small max point-sprite size, which typically is 64 pixels.
	    Therefore particles close to the viewpoint cannot be drawn as a point-sprite.
	    The idea of maxZ is for using point-sprite for far-away particles and use triangles for closer particles.
	*/
	void MakeBufferForTriangle(const YsVec3 &viewDir,const float spriteTexCoordRange,double maxZ);
};


/* } */
#endif
