/* ////////////////////////////////////////////////////////////

File Name: ysviewcontrol.cpp
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

#include "yspredefined.h"
#include "ysviewcontrol.h"

YsViewPoint::YsViewPoint()
{
	viewTarget=YsOrigin();
	viewAttitude=YsZeroAtt();
	viewAttitude.SetH(YsPi);
	viewDistance=0.0;
	matrixCached=YSFALSE;
}

const YsVec3 &YsViewPoint::GetViewPosition(YsVec3 &pos) const
{
	pos=viewTarget-viewAttitude.GetForwardVector()*viewDistance;
	return pos;
}

const YsVec3 &YsViewPoint::GetViewDirection(YsVec3 &vec) const
{
	vec=viewAttitude.GetForwardVector();
	return vec;
}

const YsVec3 YsViewPoint::GetViewPosition(void) const
{
	YsVec3 pos;
	return GetViewPosition(pos);
}
const YsVec3 YsViewPoint::GetViewDirection(void) const
{
	YsVec3 dir;
	return GetViewDirection(dir);
}

const YsAtt3 &YsViewPoint::GetViewAttitude(void) const
{
	return viewAttitude;
}

YSRESULT YsViewPoint::SetViewAttitude(const YsAtt3 &att)
{
	viewAttitude=att;
	matrixCached=YSFALSE;
	return YSOK;
}

YSRESULT YsViewPoint::SetViewDirection(const YsVec3 &dir,const double &bank)
{
	viewAttitude.SetForwardVector(dir);
	viewAttitude.SetB(bank);
	matrixCached=YSFALSE;
	return YSOK;
}

const YsVec3 &YsViewPoint::GetViewTarget(void) const
{
	return viewTarget;
}

YSRESULT YsViewPoint::SetViewTarget(const YsVec3 &pos)
{
	viewTarget=pos;
	matrixCached=YSFALSE;
	return YSOK;
}

YSRESULT YsViewPoint::SetViewDistance(const double &viewDist)
{
	viewDistance=viewDist;
	matrixCached=YSFALSE;
	return YSOK;
}

const double &YsViewPoint::GetViewDistance(void) const
{
	return viewDistance;
}

YSRESULT YsViewPoint::RotateView(double lateral,double longitudinal)
{
	if(20140530<=YsAtt3::compatibilityLevel || YSOPENGL!=YsCoordSysModel)
	{
		viewAttitude.NoseUp(longitudinal);
		viewAttitude.YawLeft(lateral);
		matrixCached=YSFALSE;
	}
	else
	{
		viewAttitude.NoseUp(-longitudinal);
		viewAttitude.YawLeft(-lateral);
		matrixCached=YSFALSE;
	}
	return YSOK;
}

const YsMatrix4x4 &YsViewPoint::GetViewMatrix(void) const
{
	if(YSTRUE!=matrixCached)
	{
		matrix.LoadIdentity();
		switch(YsCoordSysModel)
		{
		case YSRIGHT_ZMINUS_YPLUS:
			matrix.Translate(YsVec3(0.0,0.0,-GetViewDistance()));
			matrix.RotateXY(-GetViewAttitude().b());
			matrix.RotateZY(-GetViewAttitude().p());
			matrix.RotateXZ(-GetViewAttitude().h());
			break;
		case YSLEFT_ZPLUS_YPLUS:
			matrix.Scale(1.0,1.0,-1.0);
			matrix.Translate(YsVec3(0.0,0.0,GetViewDistance()));
			matrix.RotateXY(-GetViewAttitude().b());
			matrix.RotateZY(-GetViewAttitude().p());
			matrix.RotateXZ(-GetViewAttitude().h());
			break;
		}
		matrix.Translate(-GetViewTarget());
		matrixCached=YSTRUE;

		// Memo: Equivalent OpenGL transformation.
		// glTranslated(0.0,0.0,-GetViewDistance());
		// glRotated(-YsRadToDeg(GetViewAttitude().b()),0.0,0.0,1.0);
		// glRotated(-YsRadToDeg(GetViewAttitude().p()),1.0,0.0,0.0);
		// glRotated(-YsRadToDeg(GetViewAttitude().h()),0.0,1.0,0.0);
		// glTranslated(-GetViewTarget().x(),-GetViewTarget().y(),-GetViewTarget().z());
	}
	return matrix;
}

////////////////////////////////////////////////////////////

YsProjectionTransformation::YsProjectionTransformation()
{
	projectionMode=PERSPECTIVE;

	nearz=1.0;
	farz=10.0;
	aspect=1.0;
	fovy=YsPi/4.0;
	height=2.0;

	zoom=1.0;

	matrixCached=YSFALSE;
}

void YsProjectionTransformation::SetProjectionMode(PROJMODE mode)
{
	if(projectionMode!=mode)
	{
		projectionMode=mode;
		matrixCached=YSFALSE;
	}
}

YsProjectionTransformation::PROJMODE YsProjectionTransformation::GetProjectionMode(void) const
{
	return projectionMode;
}

void YsProjectionTransformation::SetAspectRatio(const double &aspect)
{
	this->aspect=aspect;
	matrixCached=YSFALSE;
}

void YsProjectionTransformation::SetFOVY(const double &fovy)
{
	this->fovy=fovy;
	matrixCached=YSFALSE;
}

const double YsProjectionTransformation::GetFOVY(void) const
{
	return fovy;
}

const double YsProjectionTransformation::GetAspectRatio(void) const
{
	return aspect;
}

void YsProjectionTransformation::SetOrthogonalProjectionHeight(const double &height)
{
	this->height=height;
	matrixCached=YSFALSE;
}

const double YsProjectionTransformation::GetOrthogonalProjectionHeight(void) const
{
	return this->height;
}

void YsProjectionTransformation::GetOrthogonalRangeWithZoom(double &left,double &right,double &top,double &bottom) const
{
	const double width=height*aspect;

	left=-width/2.0;
	right=width/2.0;
	top=height/2.0;
	bottom=-height/2.0;

	const double cx=(left+right)/2.0; // which is zero for now.
	const double cy=(top+bottom)/2.0; // which is zero for now.

	left=cx+(left-cx)/zoom;
	right=cx+(right-cx)/zoom;
	top=cy+(top-cy)/zoom;
	bottom=cy+(bottom-cy)/zoom;
}

void YsProjectionTransformation::SetNearFar(const double nearz,const double farz)
{
	this->nearz=nearz;
	this->farz=farz;
	matrixCached=YSFALSE;
}

void YsProjectionTransformation::GetNearFar(double &nearz,double &farz) const
{
	nearz=this->nearz;
	farz=this->farz;
}

const double &YsProjectionTransformation::GetZoom(void) const
{
	return this->zoom;
}

void YsProjectionTransformation::SetZoom(const double &zoom)
{
	this->zoom=zoom;
	matrixCached=YSFALSE;
}

const YsMatrix4x4 &YsProjectionTransformation::GetProjectionMatrix(void) const
{
	if(YSTRUE!=matrixCached)
	{
		double mat[16];
		if(projectionMode==PERSPECTIVE)
		{
			const double f=zoom/tan(fovy);

			// Based on the formula listed in www.opengl.org
			// See also glMultMatrix reference.  In OpenGL world, matrices are stored in column-first order.
			mat[0]=f/aspect; mat[4]=0; mat[ 8]=0;                         mat[12]=0;
			mat[1]=0;        mat[5]=f; mat[ 9]=0;                         mat[13]=0;
			mat[2]=0;        mat[6]=0; mat[10]=(farz+nearz)/(nearz-farz); mat[14]=(2.0*farz*nearz)/(nearz-farz);
			mat[3]=0;        mat[7]=0; mat[11]=-1;                        mat[15]=0;
		}
		else
		{
			double l,r,t,b;
			GetOrthogonalRangeWithZoom(l,r,t,b);

			const double tx=-(r+l)/(r-l);
			const double ty=-(t+b)/(t-b);
			const double tz=-(farz+nearz)/(farz-nearz);

			// Based on the formula listed in www.opengl.org
			mat[0]=2.0/(r-l); mat[4]=0;         mat[ 8]=0;                 mat[12]=tx;
			mat[1]=0;         mat[5]=2.0/(t-b); mat[ 9]=0;                 mat[13]=ty;
			mat[2]=0;         mat[6]=0;         mat[10]=-2.0/(farz-nearz); mat[14]=tz;
			mat[3]=0;         mat[7]=0;         mat[11]=0;                 mat[15]=1;
		}
		matrix.CreateFromOpenGlCompatibleMatrix(mat);
		matrixCached=YSTRUE;
	}
	return matrix;
}


////////////////////////////////////////////////////////////


Ys3DDrawingEnvironment::Ys3DDrawingEnvironment()
{
	viewport[0]=0;
	viewport[1]=0;
	viewport[2]=640;
	viewport[3]=480;

	windowWidth=640;
	windowHeight=480;

	screenOriginIsTopLeft=YSTRUE;
}

void Ys3DDrawingEnvironment::SetWindowSize(int wid,int hei)
{
	windowWidth=wid;
	windowHeight=hei;
}

void Ys3DDrawingEnvironment::GetWindowSize(int &wid,int &hei) const
{
	wid=windowWidth;
	hei=windowHeight;
}

void Ys3DDrawingEnvironment::SetViewportByTwoCorner(int x0,int y0,int x1,int y1)
{
	if(YSTRUE==screenOriginIsTopLeft)
	{
		y0=windowHeight-y0;
		y1=windowHeight-y1;
	}

	if(x0<x1)
	{
		viewport[0]=x0;
		viewport[2]=x1-x0;
	}
	else
	{
		viewport[0]=x1;
		viewport[2]=x0-x1;
	}

	if(y0<y1)
	{
		viewport[1]=y0;
		viewport[3]=y1-y0;
	}
	else
	{
		viewport[1]=y1;
		viewport[3]=y0-y1;
	}

	UpdateAspectRatio();
}


void Ys3DDrawingEnvironment::GetViewportByTwoCorner(int &x0,int &y0,int &x1,int &y1) const
{
	x0=viewport[0];
	y0=viewport[1];

	x1=viewport[0]+viewport[2];
	y1=viewport[1]+viewport[3];

	if(YSTRUE==screenOriginIsTopLeft)
	{
		y0=windowHeight-y0;
		y1=windowHeight-y1;
	}

	if(y0>y1)
	{
		YsSwapSomething(y0,y1);
	}
}

void Ys3DDrawingEnvironment::GetOpenGlCompatibleViewport(int viewport[4]) const
{
	viewport[0]=this->viewport[0];
	viewport[1]=this->viewport[1];
	viewport[2]=this->viewport[2];
	viewport[3]=this->viewport[3];
}

void Ys3DDrawingEnvironment::UpdateAspectRatio(void)
{
	SetAspectRatio((double)viewport[2]/(double)viewport[3]);
}

void Ys3DDrawingEnvironment::SetVerticalOrientation(YSBOOL screenOriginIsTopLeft)
{
	this->screenOriginIsTopLeft=screenOriginIsTopLeft;
}

const YsVec3 &Ys3DDrawingEnvironment::Convert3DCoordToScreenCoord(YsVec3 &scrn,const YsVec3 &pos,const YsMatrix4x4 *modelingTfm) const
{
	/* int viewport[4];
	double modelMat[16],projMat[16];
	double sx,sy,sz;

	glGetIntegerv(GL_VIEWPORT,viewport);
	glGetDoublev(GL_MODELVIEW_MATRIX,modelMat);
	glGetDoublev(GL_PROJECTION_MATRIX,projMat);

	gluProject(pos.x(),pos.y(),pos.z(),modelMat,projMat,viewport,&sx,&sy,&sz);
	x=(int)sx;
	y=(int)sy;
	y=viewport[3]-y; */


	double pos4d[4]={pos.x(),pos.y(),pos.z(),1.0};
	if(NULL!=modelingTfm)
	{
		modelingTfm->Mul(pos4d,pos4d);
	}
	GetViewMatrix().Mul(pos4d,pos4d);
	GetProjectionMatrix().Mul(pos4d,pos4d);

	pos4d[0]/=pos4d[3];
	pos4d[1]/=pos4d[3];
	pos4d[2]/=pos4d[3];

	pos4d[0]=(double)viewport[0]+(double)viewport[2]*(pos4d[0]+1.0)/2.0;
	pos4d[1]=(double)viewport[1]+(double)viewport[3]*(pos4d[1]+1.0)/2.0;

	scrn.Set(pos4d[0],pos4d[1],pos4d[2]);


	if(YSTRUE==screenOriginIsTopLeft)
	{
		const double y=(double)windowHeight-scrn.y();
		scrn.SetY(y);
	}

	return scrn;
}

const YsVec3 Ys3DDrawingEnvironment::Convert3DCoordToScreenCoord(const YsVec3 &pos,const YsMatrix4x4 *modelingTfm) const
{
	YsVec3 scrn;
	return Convert3DCoordToScreenCoord(scrn,pos,modelingTfm);
}

void Ys3DDrawingEnvironment::TransformScreenCoordTo3DLine(YsVec3 &org,YsVec3 &vec,double sx,double sy) const
{
	if(YSTRUE==screenOriginIsTopLeft)
	{
		sy=(double)windowHeight-sy;
	}

	const YsMatrix4x4 &viewMat=GetViewMatrix();
	const YsMatrix4x4 &projMat=GetProjectionMatrix();
	const YsMatrix4x4 projViewMat=projMat*viewMat;

	const YsVec3 p1(sx,sy,-1.0),p2(sx,sy,1.0);
	YsTransformScreenCoordTo3DCoord(org,p1,viewport,projViewMat);
	YsTransformScreenCoordTo3DCoord(vec,p2,viewport,projViewMat);

	vec-=org;
	vec.Normalize();
}



////////////////////////////////////////////////////////////

void YsTransform3DCoordToScreenCoord(YsVec3 &scrn,const YsVec3 &from,const int viewport[4],const YsMatrix4x4 &projViewModelTfm)
{
	double pos4d[4]={from.x(),from.y(),from.z(),1.0};
	projViewModelTfm.Mul(pos4d,pos4d);

	pos4d[0]/=pos4d[3];
	pos4d[1]/=pos4d[3];
	pos4d[2]/=pos4d[3];

	pos4d[0]=(double)viewport[0]+(double)viewport[2]*(pos4d[0]+1.0)/2.0;
	pos4d[1]=(double)viewport[1]+(double)viewport[3]*(pos4d[1]+1.0)/2.0;

	scrn.Set(pos4d[0],pos4d[1],pos4d[2]);
}

void YsTransformScreenCoordTo3DCoord(YsVec3 &threeD,const YsVec3 &scrn,const int viewport[4],const YsMatrix4x4 &projViewModelTfm)
{
	double pos4d[4]=
	{
		(scrn.x()-(double)viewport[0])*2.0/(double)viewport[2]-1.0,
		(scrn.y()-(double)viewport[1])*2.0/(double)viewport[3]-1.0,
		scrn.z(),
		1.0
	};

	if(YSOK!=projViewModelTfm.MulInverse(pos4d,pos4d))
	{
		pos4d[0]=(scrn.x()-(double)viewport[0])*2.0/(double)viewport[2]-1.0;
		pos4d[1]=(scrn.y()-(double)viewport[1])*2.0/(double)viewport[3]-1.0;
		pos4d[2]=scrn.z();
		pos4d[3]=1.0;

		double scale=0.0;
		for(int r=1; r<=3; ++r)
		{
			for(int c=1; c<=3; ++c)
			{
				YsMakeGreater(scale,fabs(projViewModelTfm.v(r,c)));
			}
		}
		scale=(0.0<scale ? 1.0/scale : 10000.0);

		YsMatrix4x4 tfm(YSFALSE);
		for(int r=1; r<=4; ++r)
		{
			for(int c=1; c<=4; ++c)
			{
				tfm.Set(r,c,projViewModelTfm.v(r,c)*scale);
			}
			pos4d[r-1]*=scale;
		}
		tfm.MulInverse(pos4d,pos4d);
	}


	pos4d[0]/=pos4d[3];
	pos4d[1]/=pos4d[3];
	pos4d[2]/=pos4d[3];
	threeD.Set(pos4d[0],pos4d[1],pos4d[2]);
}

void YsViewportTransformation(YsVec3 &vOut,const YsVec3 &vIn,const int viewport[4])
{
	vOut.SetX((double)viewport[0]+(double)viewport[2]*(vIn.x()+1.0)/2.0);
	vOut.SetY((double)viewport[1]+(double)viewport[3]*(vIn.y()+1.0)/2.0);
}

void YsInverseViewportTransformation(YsVec3 &vOut,const YsVec3 &vIn,const int viewport[4])
{
	vOut.SetX((vIn.x()-(double)viewport[0])*2.0/(double)viewport[2]-1.0);
	vOut.SetY((vIn.y()-(double)viewport[1])*2.0/(double)viewport[3]-1.0);
}


////////////////////////////////////////////////////////////


// tanFov : (X or Y)/Z
//       Z
//       |      /
//       |     /
//       |    /
//       |FOV/
//       |  /
//       | /
//       |/
//       +-------X
YSBOOL YsIsPointVisible(
    YSSIZE_T np,const YsVec3 p[],
    const YsMatrix4x4 &viewModelTfm,
	const double &nearZ,const double &farZ,const double &tanFov,
	YSCOORDSYSMODEL coordSysModel)
{
	YsVec3 corner;
	int i,nFar,nNear;
	int nRight,nRightRear,nLeft,nLeftRear,nTop,nTopRear,nBottom,nBottomRear;

	// Check
	//   Right threshold  x=z*tanFov   z*tanFov<x is out
	//   Left  threshold  x=-z*tanFov  x<-z*tanFov is out
	//   Top threshold    y=z*tanFov   z*tanFov<y is out
	//   Bottom Threshold y=-z*tanFov  y<-z*tanFov is out
	nFar=0;
	nNear=0;
	nRight=0;
	nRightRear=0;
	nLeft=0;
	nLeftRear=0;
	nTop=0;
	nTopRear=0;
	nBottom=0;
	nBottomRear=0;
	for(i=0; i<np; i++)
	{
		corner=viewModelTfm*p[i];

		if(YSRIGHT_ZMINUS_YPLUS==coordSysModel)
		{
			corner.MulZ(-1.0);
		}

		YSBOOL inc;
		inc=YSFALSE;
		if(corner.z()<nearZ)
		{
			nNear++;
			inc=YSTRUE;
		}
		if(corner.z()>farZ)
		{
			nFar++;
			inc=YSTRUE;
		}
		if(farZ*tanFov<corner.x())
		{
			nRight++;
			inc=YSTRUE;
		}
		if(corner.z()*tanFov<corner.x())
		{
			nRightRear++;
			inc=YSTRUE;
		}
		if(corner.x()<-farZ*tanFov)
		{
			nLeft++;
			inc=YSTRUE;
		}
		if(corner.x()<-corner.z()*tanFov)
		{
			nLeftRear++;
			inc=YSTRUE;
		}
		if(farZ*tanFov<corner.y())
		{
			nTop++;
			inc=YSTRUE;
		}
		if(corner.z()*tanFov<corner.y())
		{
			nTopRear++;
			inc=YSTRUE;
		}
		if(corner.y()<-farZ*tanFov)
		{
			nBottom++;
			inc=YSTRUE;
		}
		if(corner.y()<-corner.z()*tanFov)
		{
			nBottomRear++;
			inc=YSTRUE;
		}

		if(inc!=YSTRUE)
		{
			break;
		}
	}

	if(nFar==np || nNear==np ||
	   nRight==np || nLeft==np || nTop==np || nBottom==np ||
	   nRightRear==np || nLeftRear==np || nTopRear==np || nBottomRear==np)
	{
		return YSFALSE;
	}

	return YSTRUE;
}

YSBOOL YsIsPointVisibleLH(
    YSSIZE_T np,const YsVec3 p[],
    const YsMatrix4x4 &viewModelTfm,
	const double &nearZ,const double &farZ,const double &tanFov)
{
	return YsIsPointVisible(np,p,viewModelTfm,nearZ,farZ,tanFov,YSLEFT_ZPLUS_YPLUS);
}

YSBOOL YsIsPointVisibleRH(
    YSSIZE_T np,const YsVec3 p[],
    const YsMatrix4x4 &viewModelTfm,
	const double &nearZ,const double &farZ,const double &tanFov)
{
	return YsIsPointVisible(np,p,viewModelTfm,nearZ,farZ,tanFov,YSRIGHT_ZMINUS_YPLUS);
}

YSBOOL YsIsPointVisible(
    YSSIZE_T np,const YsVec3 p[],
    const YsMatrix4x4 &viewModelTfm,
	const double &nearZ,const double &farZ,const double &tanFov)
{
	return YsIsPointVisible(np,p,viewModelTfm,nearZ,farZ,tanFov,YsCoordSysModel);
}

////////////////////////////////////////////////////////////

YSBOOL YsIsBoundingBoxVisibleLH(
    const YsVec3 bbx[2],
    const YsMatrix4x4 &viewModelTfm,
	const double &nearZ,const double &farZ,const double &tanFov,
	YSCOORDSYSMODEL coordSysModel)
{
	YsVec3 corner[8];

	corner[0].Set(bbx[0].x(),bbx[0].y(),bbx[0].z());
	corner[1].Set(bbx[0].x(),bbx[0].y(),bbx[1].z());
	corner[2].Set(bbx[0].x(),bbx[1].y(),bbx[0].z());
	corner[3].Set(bbx[0].x(),bbx[1].y(),bbx[1].z());
	corner[4].Set(bbx[1].x(),bbx[0].y(),bbx[0].z());
	corner[5].Set(bbx[1].x(),bbx[0].y(),bbx[1].z());
	corner[6].Set(bbx[1].x(),bbx[1].y(),bbx[0].z());
	corner[7].Set(bbx[1].x(),bbx[1].y(),bbx[1].z());

	return YsIsPointVisible(8,corner,viewModelTfm,nearZ,farZ,tanFov,coordSysModel);
}

YSBOOL YsIsBoundingBoxVisibleLH(
    const YsVec3 bbx[2],
    const YsMatrix4x4 &viewModelTfm,
	const double &nearZ,const double &farZ,const double &tanFov)
{
	YsVec3 corner[8];

	corner[0].Set(bbx[0].x(),bbx[0].y(),bbx[0].z());
	corner[1].Set(bbx[0].x(),bbx[0].y(),bbx[1].z());
	corner[2].Set(bbx[0].x(),bbx[1].y(),bbx[0].z());
	corner[3].Set(bbx[0].x(),bbx[1].y(),bbx[1].z());
	corner[4].Set(bbx[1].x(),bbx[0].y(),bbx[0].z());
	corner[5].Set(bbx[1].x(),bbx[0].y(),bbx[1].z());
	corner[6].Set(bbx[1].x(),bbx[1].y(),bbx[0].z());
	corner[7].Set(bbx[1].x(),bbx[1].y(),bbx[1].z());

	return YsIsPointVisibleLH(8,corner,viewModelTfm,nearZ,farZ,tanFov);
}

YSBOOL YsIsBoundingBoxVisibleRH(
    const YsVec3 bbx[2],
    const YsMatrix4x4 &viewModelTfm,
	const double &nearZ,const double &farZ,const double &tanFov)
{
	YsVec3 corner[8];

	corner[0].Set(bbx[0].x(),bbx[0].y(),bbx[0].z());
	corner[1].Set(bbx[0].x(),bbx[0].y(),bbx[1].z());
	corner[2].Set(bbx[0].x(),bbx[1].y(),bbx[0].z());
	corner[3].Set(bbx[0].x(),bbx[1].y(),bbx[1].z());
	corner[4].Set(bbx[1].x(),bbx[0].y(),bbx[0].z());
	corner[5].Set(bbx[1].x(),bbx[0].y(),bbx[1].z());
	corner[6].Set(bbx[1].x(),bbx[1].y(),bbx[0].z());
	corner[7].Set(bbx[1].x(),bbx[1].y(),bbx[1].z());

	return YsIsPointVisibleRH(8,corner,viewModelTfm,nearZ,farZ,tanFov);
}

YSBOOL YsIsBoundingBoxVisible(
    const YsVec3 bbx[2],
    const YsMatrix4x4 &viewModelTfm,
	const double &nearZ,const double &farZ,const double &tanFov)
{
	YsVec3 corner[8];

	corner[0].Set(bbx[0].x(),bbx[0].y(),bbx[0].z());
	corner[1].Set(bbx[0].x(),bbx[0].y(),bbx[1].z());
	corner[2].Set(bbx[0].x(),bbx[1].y(),bbx[0].z());
	corner[3].Set(bbx[0].x(),bbx[1].y(),bbx[1].z());
	corner[4].Set(bbx[1].x(),bbx[0].y(),bbx[0].z());
	corner[5].Set(bbx[1].x(),bbx[0].y(),bbx[1].z());
	corner[6].Set(bbx[1].x(),bbx[1].y(),bbx[0].z());
	corner[7].Set(bbx[1].x(),bbx[1].y(),bbx[1].z());

	return YsIsPointVisible(8,corner,viewModelTfm,nearZ,farZ,tanFov);
}

YSBOOL YsIsPointVisible(
    int np,const YsVec3 p[],
    const YsMatrix4x4 &viewModelTfm,
    const YsMatrix4x4 &ProjTfm)
{
	switch(YsCoordSysModel)
	{
	case YSLEFT_ZPLUS_YPLUS:
		return YsIsPointVisibleLH(np,p,viewModelTfm,ProjTfm);
	case YSRIGHT_ZMINUS_YPLUS:
	default:
		return YsIsPointVisibleRH(np,p,viewModelTfm,ProjTfm);
	}
}

YSBOOL YsIsPointVisibleRH(
    int np,const YsVec3 p[],
    const YsMatrix4x4 &viewModelTfm,
    const YsMatrix4x4 &ProjTfm)
{
	auto prod=ProjTfm*viewModelTfm;
	for(int i=0; i<np; ++i)
	{
		auto v=prod*p[i];
		if(-1.0<=v.x() && v.x()<=1.0 &&
		   -1.0<=v.y() && v.y()<=1.0 &&
		   -1.0<=v.x() && v.x()<=1.0)
		{
			return YSTRUE;
		}
	}
	return YSFALSE;
}

YSBOOL YsIsPointVisibleLH(
    int np,const YsVec3 p[],
    const YsMatrix4x4 &viewModelTfm,
    const YsMatrix4x4 &ProjTfm)
{
	auto prod=ProjTfm;
	prod.ScaleZ(-1.0);
	prod*=viewModelTfm;
	for(int i=0; i<np; ++i)
	{
		auto v=prod*p[i];
		if(-1.0<=v.x() && v.x()<=1.0 &&
		   -1.0<=v.y() && v.y()<=1.0 &&
		   -1.0<=v.x() && v.x()<=1.0)
		{
			return YSTRUE;
		}
	}
	return YSFALSE;
}

YSBOOL YsIsBoundingBoxVisible(
    const YsVec3 bbx[2],
    const YsMatrix4x4 &viewModelTfm,
    const YsMatrix4x4 &ProjTfm)
{
	switch(YsCoordSysModel)
	{
	case YSLEFT_ZPLUS_YPLUS:
		return YsIsBoundingBoxVisibleLH(bbx,viewModelTfm,ProjTfm);
	case YSRIGHT_ZMINUS_YPLUS:
	default:
		return YsIsBoundingBoxVisibleRH(bbx,viewModelTfm,ProjTfm);
	}
}

// See
//   https://www.opengl.org/discussion_boards/showthread.php/150777-Bounding-Box-Visible-In-View-Frustrum
//   http://gamedevs.org/uploads/fast-extraction-viewing-frustum-planes-from-world-view-projection-matrix.pdf
// for frustum-plane extraction.

static void YsGetFrustumPlaneFromProjectionMatrix(double plnEqn[6][4],const YsMatrix4x4 &projMat)
{
	plnEqn[0][0]= projMat.v11()+projMat.v41();
	plnEqn[0][1]= projMat.v12()+projMat.v42();
	plnEqn[0][2]= projMat.v13()+projMat.v43();
	plnEqn[0][3]= projMat.v14()+projMat.v44();

	plnEqn[1][0]=-projMat.v11()+projMat.v41();
	plnEqn[1][1]=-projMat.v12()+projMat.v42();
	plnEqn[1][2]=-projMat.v13()+projMat.v43();
	plnEqn[1][3]=-projMat.v14()+projMat.v44();

	plnEqn[2][0]= projMat.v21()+projMat.v41();
	plnEqn[2][1]= projMat.v22()+projMat.v42();
	plnEqn[2][2]= projMat.v23()+projMat.v43();
	plnEqn[2][3]= projMat.v24()+projMat.v44();

	plnEqn[3][0]=-projMat.v21()+projMat.v41();
	plnEqn[3][1]=-projMat.v22()+projMat.v42();
	plnEqn[3][2]=-projMat.v23()+projMat.v43();
	plnEqn[3][3]=-projMat.v24()+projMat.v44();

	plnEqn[4][0]= projMat.v31()+projMat.v41();
	plnEqn[4][1]= projMat.v32()+projMat.v42();
	plnEqn[4][2]= projMat.v33()+projMat.v43();
	plnEqn[4][3]= projMat.v34()+projMat.v44();

	plnEqn[5][0]=-projMat.v31()+projMat.v41();
	plnEqn[5][1]=-projMat.v32()+projMat.v42();
	plnEqn[5][2]=-projMat.v33()+projMat.v43();
	plnEqn[5][3]=-projMat.v34()+projMat.v44();
}

YSBOOL YsIsBoundingBoxVisibleRH(
    const YsVec3 bbx[2],
    const YsMatrix4x4 &viewModelTfm,
    const YsMatrix4x4 &ProjTfm)
{
	auto prod=viewModelTfm;

	double plnEqn[6][4];
	YsGetFrustumPlaneFromProjectionMatrix(plnEqn,ProjTfm);

	YSBOOL allOut[6]={YSTRUE,YSTRUE,YSTRUE,YSTRUE,YSTRUE,YSTRUE};
	for(int i=0; i<8; ++i)
	{
		YsVec3 pos(bbx[i&1].x(),bbx[(i/2)&1].y(),bbx[(i/4)&1].z());
		pos=prod*pos;
		for(int j=0; j<6; ++j)
		{
			if(plnEqn[j][0]*pos.x()+plnEqn[j][1]*pos.y()+plnEqn[j][2]*pos.z()+plnEqn[j][3]>=0.0)
			{
				allOut[j]=YSFALSE;
			}
		}
	}

	if(YSTRUE==allOut[0] || YSTRUE==allOut[1] || YSTRUE==allOut[2] || YSTRUE==allOut[3] || YSTRUE==allOut[4] || YSTRUE==allOut[5])
	{
		return YSFALSE;
	}
	return YSTRUE;
}

YSBOOL YsIsBoundingBoxVisibleLH(
    const YsVec3 bbx[2],
    const YsMatrix4x4 &viewModelTfm,
    const YsMatrix4x4 &ProjTfm)
{
	YsMatrix4x4 prod;
	prod.ScaleZ(-1.0);
	prod*=viewModelTfm;

	double plnEqn[6][4];
	YsGetFrustumPlaneFromProjectionMatrix(plnEqn,ProjTfm);

	YSBOOL allOut[6]={YSTRUE,YSTRUE,YSTRUE,YSTRUE,YSTRUE,YSTRUE};
	for(int i=0; i<8; ++i)
	{
		YsVec3 pos(bbx[i&1].x(),bbx[(i/2)&1].y(),bbx[(i/4)&1].z());
		pos=prod*pos;
		for(int j=0; j<6; ++j)
		{
			if(plnEqn[j][0]*pos.x()+plnEqn[j][1]*pos.y()+plnEqn[j][2]*pos.z()+plnEqn[j][3]>=0.0)
			{
				allOut[j]=YSFALSE;
			}
		}
	}

	if(YSTRUE==allOut[0] || YSTRUE==allOut[1] || YSTRUE==allOut[2] || YSTRUE==allOut[3] || YSTRUE==allOut[4] || YSTRUE==allOut[5])
	{
		return YSFALSE;
	}
	return YSTRUE;
}

YSRESULT YsCalcualteViewFrustumPlaneFromProjectionMatrix(YsPlane pln[6],const YsMatrix4x4 &projMat)
{
	double plnEqn[6][4];
	YsGetFrustumPlaneFromProjectionMatrix(plnEqn,projMat);

	for(int i=0; i<6; ++i)
	{
		// ax+by+cz+d=0
		auto a=plnEqn[i][0];
		auto b=plnEqn[i][1];
		auto c=plnEqn[i][2];
		auto d=plnEqn[i][3];

		auto l=sqrt(a*a+b*b+c*c);
		a/=l;
		b/=l;
		c/=l;
		d/=l;

		YsVec3 nom(-a,-b,-c);
		YsVec3 org=nom*d;
		if(YSLEFT_ZPLUS_YPLUS==YsCoordSysModel)
		{
			org.MulZ(-1.0);
			nom.MulZ(-1.0);
		}

		pln[i].Set(org,nom);
	}

	return YSOK;
}

