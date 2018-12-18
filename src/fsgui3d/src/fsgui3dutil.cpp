/* ////////////////////////////////////////////////////////////

File Name: fsgui3dutil.cpp
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

#include "fsgui3dutil.h"



YSRESULT FsGui3DGetArrowHeadTriangle(
    YsVec3 plg[3],YsVec3 p1,YsVec3 p2,
    const int viewport[4],const YsMatrix4x4 &projMat,const YsMatrix4x4 &modelViewMat,const double nearz,
    const double lengthInPixel,const double widthInPixel)
{
	plg[0]=p2;

	modelViewMat.Mul(p1,p1,1.0);
	modelViewMat.Mul(p2,p2,1.0);

	if(p1.z()>nearz || p2.z()>nearz)
	{
		if(YSOK!=YsClipLineWithNearZ(p1,p2,p1,p2,nearz))
		{
			return YSERR;
		}
	}

	double h[4]={p1.x(),p1.y(),p1.z(),1.0};
	projMat.Mul(h,h);
	p1.Set(h[0]/h[3],h[1]/h[3],h[2]/h[3]);

	h[0]=p2.x();
	h[1]=p2.y();
	h[2]=p2.z();
	h[3]=1.0;
	projMat.Mul(h,h);
	p2.Set(h[0]/h[3],h[1]/h[3],h[2]/h[3]);

	YsViewportTransformation(p1,p1,viewport);
	YsViewportTransformation(p2,p2,viewport);

	YsVec3 u(p2.x()-p1.x(),p2.y()-p1.y(),0.0);
	if(YSOK==u.Normalize())
	{
		YsVec3 v(-u.y(),u.x(),0.0);

		plg[1]=p2-u*lengthInPixel+v*widthInPixel/2.0;
		plg[2]=p2-u*lengthInPixel-v*widthInPixel/2.0;

		YsTransformScreenCoordTo3DCoord(plg[1],plg[1],viewport,projMat*modelViewMat);
		YsTransformScreenCoordTo3DCoord(plg[2],plg[2],viewport,projMat*modelViewMat);

		return YSOK;
	}
	return YSERR;
}


YSRESULT FsGui3DGetPixelVector(
    YsVec3 &xVec,YsVec3 &yVec,const YsVec3 &p,
    const int viewport[4],const YsMatrix4x4 &projMat,const YsMatrix4x4 &modelViewMat,const double nearz)
{
	double h[4]={p.x(),p.y(),p.z(),1.0};

	modelViewMat.Mul(h,h);
	projMat.Mul(h,h);


	YsVec3 scrn(h[0]/h[3],h[1]/h[3],h[2]/h[3]);
	if(scrn.z()<nearz)
	{
		return YSERR;
	}

	YsViewportTransformation(scrn,scrn,viewport);

	xVec=scrn;
	xVec.AddX(1.0);

	yVec=scrn;
	yVec.AddY(1.0);

	YsTransformScreenCoordTo3DCoord(xVec,xVec,viewport,projMat*modelViewMat);
	YsTransformScreenCoordTo3DCoord(yVec,yVec,viewport,projMat*modelViewMat);

	xVec-=p;
	yVec-=p;

	return YSOK;
}

YSRESULT FsGui3DGetStarPoint(
    YsVec3 plg[5],const YsVec3 &p,
    const int viewport[4],const YsMatrix4x4 &projMat,const YsMatrix4x4 &modelViewMat,const double nearz,
    const double iconSize)
{
	YsVec3 xVec,yVec;
	if(YSOK==FsGui3DGetPixelVector(xVec,yVec,p,viewport,projMat,modelViewMat,nearz))
	{
		xVec*=iconSize/2.0;
		yVec*=iconSize/2.0;

		plg[0]=p+yVec;
		plg[1]=p+xVec*YsSin145deg+yVec*YsCos145deg;
		plg[2]=p-xVec*YsSin70deg +yVec*YsCos70deg;
		plg[3]=p+xVec*YsSin70deg +yVec*YsCos70deg;
		plg[4]=p-xVec*YsSin145deg+yVec*YsCos145deg;

		return YSOK;
	}
	return YSERR;
}

YSRESULT FsGui3DGetRectPoint(
    YsVec3 plg[4],const YsVec3 &p,
    const int viewport[4],const YsMatrix4x4 &projMat,const YsMatrix4x4 &modelViewMat,const double nearz,
    const double iconSize)
{
	YsVec3 xVec,yVec;
	if(YSOK==FsGui3DGetPixelVector(xVec,yVec,p,viewport,projMat,modelViewMat,nearz))
	{
		xVec*=iconSize/2.0;
		yVec*=iconSize/2.0;

		plg[0]=p+xVec+yVec;
		plg[1]=p-xVec+yVec;
		plg[2]=p-xVec-yVec;
		plg[3]=p+xVec-yVec;

		return YSOK;
	}
	return YSERR;
}

YSRESULT FsGui3DGetCirclePoint(
    YsVec3 plg[],int nDiv,const YsVec3 &p,
    const int viewport[4],const YsMatrix4x4 &projMat,const YsMatrix4x4 &modelViewMat,const double nearz,
    const double iconSize)
{
	YsVec3 xVec,yVec;
	if(YSOK==FsGui3DGetPixelVector(xVec,yVec,p,viewport,projMat,modelViewMat,nearz))
	{
		xVec*=iconSize/2.0;
		yVec*=iconSize/2.0;

		for(int i=0; i<nDiv; ++i)
		{
			const double angle=YsPi*2.0*(double)i/(double)nDiv;
			plg[i]=p+xVec*cos(angle)+yVec*sin(angle);
		}

		return YSOK;
	}
	return YSERR;
}

