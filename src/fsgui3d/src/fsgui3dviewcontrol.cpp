/* ////////////////////////////////////////////////////////////

File Name: fsgui3dviewcontrol.cpp
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

#include "fsgui3dviewcontrol.h"
#include "fsgui3dviewcontroldialog.h"
#include <fssimplewindow.h>

FsGui3DDrawingEnvironment::FsGui3DDrawingEnvironment()
{
	straightAttitude=YsZeroAtt();
	straightAttitude.SetH(-YsPi);
	SetTargetBoundingBox(YsVec3(-1.0,-1.0,-1.0),YsVec3( 1.0, 1.0, 1.0));
	Straighten();

	objBbxMin=-YsXYZ();
	objBbxMax= YsXYZ();

	zoomByViewDistanceInPerspectiveMode=YSTRUE;
}


FsGui3DDrawingEnvironment::~FsGui3DDrawingEnvironment()
{
}



YSRESULT FsGui3DDrawingEnvironment::Straighten(void)
{
	const YsVec3 cen=(objBbxMin+objBbxMax)/2.0;
	SetViewTarget(cen);
	SetViewAttitude(straightAttitude);
	SetZoom(1.0);
	return YSOK;
}

YSRESULT FsGui3DDrawingEnvironment::StraightenAttitude(void)
{
	SetViewAttitude(straightAttitude);
	return YSOK;
}

YSRESULT FsGui3DDrawingEnvironment::SetTargetBoundingBox(const YsVec3 &min,const YsVec3 &max)
{
	YsBoundingBoxMaker3 bbx;
	bbx.Begin(min);
	bbx.Add(max);
	bbx.Get(objBbxMin,objBbxMax);

	const YsVec3 objCenter=(objBbxMin+objBbxMax)/2.0;

	const double lx=objBbxMax.x()-objBbxMin.x();
	const double ly=objBbxMax.y()-objBbxMin.y();
	const double lz=objBbxMax.z()-objBbxMin.z();
	const double l=YsGreater(YsGreater(lx,ly),lz)/2.0;
	objBbxMin=objCenter-YsXYZ()*l;
	objBbxMax=objCenter+YsXYZ()*l;

	SetViewDistance((objBbxMax-objBbxMin).GetLength());
	SetOrthogonalProjectionHeight((objBbxMax.y()-objBbxMin.y())*1.732);

	double objSize=(objBbxMax-objBbxMin).GetLength()/2.0;
	if(objSize<=0.1)
	{
		objSize=0.1;
	}

	const double tanfov=0.5*1.732*objSize/GetViewDistance();
	// projPlaneDistance=(0.5*GetViewDistance()/objSize)/1.732;
	SetFOVY(atan(tanfov));

	return YSOK;
}

YSRESULT FsGui3DDrawingEnvironment::SetTargetBoundingBox(const YsVec3 minamx[2])
{
	return SetTargetBoundingBox(minamx[0],minamx[1]);
}

YSRESULT FsGui3DDrawingEnvironment::SetPerspective(YSBOOL drawInPerspectiveMode)
{
	if(YSTRUE==drawInPerspectiveMode)
	{
		SetProjectionMode(YsProjectionTransformation::PERSPECTIVE);
	}
	else
	{
		SetProjectionMode(YsProjectionTransformation::ORTHOGONAL);
	}
	return YSOK;
}

YSRESULT FsGui3DDrawingEnvironment::SetStraightAttitude(const YsAtt3 &att)
{
	straightAttitude=att;
	return YSOK;
}

const YsAtt3 &FsGui3DDrawingEnvironment::GetStraightAttitude(void) const
{
	return straightAttitude;
}

YSRESULT FsGui3DDrawingEnvironment::GetTargetBoundingBox(YsVec3 &min,YsVec3 &max)
{
	min=objBbxMin;
	max=objBbxMax;
	return YSOK;
}

YSBOOL FsGui3DDrawingEnvironment::GetPerspective(void) const
{
	return (YsProjectionTransformation::PERSPECTIVE==GetProjectionMode() ? YSTRUE : YSFALSE);
}

YSRESULT FsGui3DDrawingEnvironment::Zoom(double factor)
{
	if(YSTRUE==zoomByViewDistanceInPerspectiveMode && YSTRUE==GetPerspective())
	{
		YsVec3 bbx[2];
		GetTargetBoundingBox(bbx[0],bbx[1]);
		// const double minDist=(bbx[0]-bbx[1]).GetLength()*0.01;

		const double newDist=GetViewDistance()/factor;
		SetViewDistance(newDist);
	}
	else
	{
		double zoom=GetZoom()*factor;
		if(zoom<0.005)
		{
			zoom=0.005;
		}
		SetZoom(zoom);
	}
	return YSOK;
}

YSRESULT FsGui3DDrawingEnvironment::SaveViewpoint(int windowWidth,int windowHeight,const char fn[])
{
	FILE *fp=fopen(fn,"w");
	if(NULL!=fp)
	{
		YsTextFileOutputStream outStream(fp);
		YSRESULT res=SaveViewpoint(windowWidth,windowHeight,outStream);
		fclose(fp);
		return res;
	}
	return YSERR;
}

YSRESULT FsGui3DDrawingEnvironment::SaveViewpoint(int windowWidth,int windowHeight,YsTextOutputStream &outStream)
{
	for(auto &str : SaveViewpoint(windowWidth,windowHeight))
	{
		outStream.Printf("%s\n",str.Txt());
	}
	return YSOK;
}

YSRESULT FsGui3DDrawingEnvironment::LoadViewpoint(int &windowWidth,int &windowHeight,const char fn[])
{
	FILE *fp;
	fp=fopen(fn,"r");
	if(fp!=NULL)
	{
		YsTextFileInputStream inStream(fp);
		auto res=LoadViewpoint(windowWidth,windowHeight,inStream);
		fclose(fp);
		return res;
	}
	return YSERR;
}

YSRESULT FsGui3DDrawingEnvironment::LoadViewpoint(int &windowWidth,int &windowHeight,YsTextInputStream &inStream)
{
	YsString buf;
	double a,b,c;
	for(buf=""; 0==buf.Strlen() && YSTRUE!=inStream.EndOfFile(); inStream.Gets(buf));

	int version=0;
	if('V'==buf[0])
	{
		version=atoi(buf.Txt()+1);
	}

	if(20120831==version)
	{
		YSBOOL perspective;
		for(buf=""; 0==buf.Strlen() && YSTRUE!=inStream.EndOfFile(); inStream.Gets(buf));
		sscanf(buf,"%d",&perspective);
		SetPerspective(perspective);

		for(buf=""; 0==buf.Strlen() && YSTRUE!=inStream.EndOfFile(); inStream.Gets(buf));
		sscanf(buf,"%lf%lf%lf",&a,&b,&c);
		SetViewTarget(YsVec3(a,b,c));

		for(buf=""; 0==buf.Strlen() && YSTRUE!=inStream.EndOfFile(); inStream.Gets(buf));
		sscanf(buf,"%lf%lf%lf",&a,&b,&c);
		SetViewAttitude(YsAtt3(a,b,c));

		double viewDist;
		for(buf=""; 0==buf.Strlen() && YSTRUE!=inStream.EndOfFile(); inStream.Gets(buf));
		sscanf(buf,"%lf",&viewDist);
		SetViewDistance(viewDist);

		for(buf=""; 0==buf.Strlen() && YSTRUE!=inStream.EndOfFile(); inStream.Gets(buf));
		sscanf(buf,"%lf%lf%lf",&a,&b,&c);
		// objPosition.Set(a,b,c); no longer supported

		for(buf=""; 0==buf.Strlen() && YSTRUE!=inStream.EndOfFile(); inStream.Gets(buf));
		sscanf(buf,"%lf%lf%lf",&a,&b,&c);
		// objAttitude.Set(a,b,c); no longer supported

		double fovy;
		for(buf=""; 0==buf.Strlen() && YSTRUE!=inStream.EndOfFile(); inStream.Gets(buf));
		sscanf(buf,"%lf",&fovy);
		SetFOVY(fovy);

		double orthoHeight;
		for(buf=""; 0==buf.Strlen() && YSTRUE!=inStream.EndOfFile(); inStream.Gets(buf));
		sscanf(buf,"%lf",&orthoHeight);
		SetOrthogonalProjectionHeight(orthoHeight);

		double zoom;
		for(buf=""; 0==buf.Strlen() && YSTRUE!=inStream.EndOfFile(); inStream.Gets(buf));
		sscanf(buf,"%lf",&zoom);
		SetZoom(zoom);

		for(buf=""; 0==buf.Strlen() && YSTRUE!=inStream.EndOfFile(); inStream.Gets(buf));
		sscanf(buf,"%d%d",&windowWidth,&windowHeight);

		return YSOK;
	}
	else if(20120831>version)
	{
		YSBOOL perspective;
		for(buf=""; 0==buf.Strlen() && YSTRUE!=inStream.EndOfFile(); inStream.Gets(buf));
		sscanf(buf,"%d",&perspective);
		SetPerspective(perspective);

		for(buf=""; 0==buf.Strlen() && YSTRUE!=inStream.EndOfFile(); inStream.Gets(buf));
		sscanf(buf,"%lf%lf%lf",&a,&b,&c);
		SetViewTarget(YsVec3(a,b,c));

		for(buf=""; 0==buf.Strlen() && YSTRUE!=inStream.EndOfFile(); inStream.Gets(buf));
		sscanf(buf,"%lf%lf%lf",&a,&b,&c);
		SetViewAttitude(YsAtt3(a,b,c));

		double viewDist;
		for(buf=""; 0==buf.Strlen() && YSTRUE!=inStream.EndOfFile(); inStream.Gets(buf));
		sscanf(buf,"%lf",&viewDist);
		SetViewDistance(viewDist);

		for(buf=""; 0==buf.Strlen() && YSTRUE!=inStream.EndOfFile(); inStream.Gets(buf));
		sscanf(buf,"%lf%lf%lf",&a,&b,&c);
		// objPosition.Set(a,b,c); no longer supported

		for(buf=""; 0==buf.Strlen() && YSTRUE!=inStream.EndOfFile(); inStream.Gets(buf));
		sscanf(buf,"%lf%lf%lf",&a,&b,&c);
		// objAttitude.Set(a,b,c); no longer supported

		double fovy;
		for(buf=""; 0==buf.Strlen() && YSTRUE!=inStream.EndOfFile(); inStream.Gets(buf));
		sscanf(buf,"%lf",&fovy);
		SetFOVY(fovy);

		double ortho[4];
		for(buf=""; 0==buf.Strlen() && YSTRUE!=inStream.EndOfFile(); inStream.Gets(buf));
		sscanf(buf,"%lf%lf%lf%lf",&ortho[0],&ortho[1],&ortho[2],&ortho[3]);
		SetOrthogonalProjectionHeight(ortho[3]-ortho[1]);

		double zoom;
		for(buf=""; 0==buf.Strlen() && YSTRUE!=inStream.EndOfFile(); inStream.Gets(buf));
		sscanf(buf,"%lf",&zoom);
		SetZoom(zoom);

		for(buf=""; 0==buf.Strlen() && YSTRUE!=inStream.EndOfFile(); inStream.Gets(buf));
		sscanf(buf,"%d%d",&windowWidth,&windowHeight);

		return YSOK;
	}
	else
	{
		return YSERR;
	}
}

YsArray <YsString> FsGui3DDrawingEnvironment::SaveViewpoint(int windowWidth,int windowHeight)
{
	YsArray <YsString> strArray;

	const YSBOOL perspective=GetPerspective();
	const YsVec3 objPosition=YsOrigin();
	const YsAtt3 objAttitude=YsZeroAtt();

	strArray.Increment();
	strArray.Last().Printf("V20120831\n");
	strArray.Increment();
	strArray.Last().Printf("%d\n",perspective);
	strArray.Increment();
	strArray.Last().Printf("%lf %lf %lf\n",
	    GetViewTarget().x(),
	    GetViewTarget().y(),
	    GetViewTarget().z());
	strArray.Increment();
	strArray.Last().Printf("%lf %lf %lf\n",
	    GetViewAttitude().h(),
	    GetViewAttitude().p(),
	    GetViewAttitude().b());
	strArray.Increment();
	strArray.Last().Printf("%lf\n",GetViewDistance());
	strArray.Increment();
	strArray.Last().Printf("%lf %lf %lf\n",objPosition.x(),objPosition.y(),objPosition.z());
	strArray.Increment();
	strArray.Last().Printf("%lf %lf %lf\n",objAttitude.h(),objAttitude.p(),objAttitude.b());
	strArray.Increment();
	strArray.Last().Printf("%lf\n",GetFOVY());

	strArray.Increment();
	strArray.Last().Printf("%lf\n",GetOrthogonalProjectionHeight());
	strArray.Increment();
	strArray.Last().Printf("%lf\n",GetZoom());
	strArray.Increment();
	strArray.Last().Printf("%d %d\n",windowWidth,windowHeight);

	return strArray;
}
YSRESULT FsGui3DDrawingEnvironment::LoadViewpoint(int &windowWidth,int &windowHeight,const YsConstArrayMask <YsString> &view)
{
	YsTextMemoryInputStream inStream(view);
	return LoadViewpoint(windowWidth,windowHeight,inStream);
}

YSBOOL FsGui3DDrawingEnvironment::IsPolygonPicked(double &z,int mx,int my,int np,const YsVec3 v[]) const
{
	int viewport[4];
	YsVec2 mos;
	YsArray <YsVec2,16> v2d;
	YsVec3 prjPos;
	YSSIDE side;


	GetOpenGlCompatibleViewport(viewport);
	const YsMatrix4x4 &projMat=GetProjectionMatrix();
	const YsMatrix4x4 &viewMat=GetViewMatrix();
	const YsMatrix4x4 projViewMat=(projMat*viewMat);

	// Near clip
	{
		double nearz,farz;
		GetNearFar(nearz,farz);

		YSBOOL needClip=YSFALSE;

		YsArray <YsVec3,4> tfmPlg;
		tfmPlg.Set(np,NULL);
		for(int i=0; i<np; ++i)
		{
			tfmPlg[i]=viewMat*v[i];
			if(tfmPlg[i].z()>-nearz)   // Should I make <nearz for left-hand coord?
			{
				needClip=YSTRUE;
			}
		}
		if(YSTRUE==needClip)
		{
			YsArray <YsVec3,4> clipPlg;    // Should I make nearz,positivez=YSTRUE for left-hand coord?
			if(YSOK!=YsClipPolygonWithNearZ(clipPlg,(int)tfmPlg.GetN(),tfmPlg,-nearz,/*positive=*/YSFALSE))
			{
				return YSFALSE;
			}

			v2d.Set(clipPlg.GetN(),NULL);
			for(int i=0; i<clipPlg.GetN(); i++)
			{
				YsTransform3DCoordToScreenCoord(prjPos,clipPlg[i],viewport,projMat);
				v2d[i].Set(prjPos.x(),prjPos.y());
			}
		}
		else
		{
			v2d.Set(np,NULL);
			for(int i=0; i<np; i++)
			{
				YsTransform3DCoordToScreenCoord(prjPos,v[i],viewport,projViewMat);
				v2d[i].Set(prjPos.x(),prjPos.y());
			}
		}
	}

	int winWid,winHei;
	FsGetWindowSize(winWid,winHei);
	mos.Set(mx,winHei-1-my);

	side=YsCheckInsidePolygon2(mos,np,v2d);
	if(side==YSINSIDE || side==YSBOUNDARY)
	{
		YsVec3 org,vec,itsc;
		YsVec3 cen,nom;
		YsPlane pln;

		YsGetCenterOfPolygon3(cen,np,v);
		YsGetAverageNormalVector(nom,np,v);
		pln.Set(cen,nom);

		org.Set(mos.x(),mos.y(),-1.0);
		YsTransformScreenCoordTo3DCoord(org,org,viewport,projViewMat);
		vec.Set(mos.x(),mos.y(), 1.0);
		YsTransformScreenCoordTo3DCoord(vec,vec,viewport,projViewMat);
		vec-=org;
		vec.Normalize();

		if(pln.GetIntersection(itsc,org,vec)==YSOK)
		{
			YsVec3 prj;
			YsTransform3DCoordToScreenCoord(prj,itsc,viewport,projViewMat);
			z=prj.z();
			return YSTRUE;
		}
	}
	return YSFALSE;
}

YSBOOL FsGui3DDrawingEnvironment::IsLinePicked(double &z,int mx,int my,int rad,const YsVec3 &p1,const YsVec3 &p2) const
{
	int viewport[4];
	YsVec2 mos;
	YsVec2 v2d[2],nea;
	double v2dZ[2];
	YsVec3 prjPos;

	GetOpenGlCompatibleViewport(viewport);
	const YsMatrix4x4 &projMat=GetProjectionMatrix();
	const YsMatrix4x4 &viewMat=GetViewMatrix();
	const YsMatrix4x4 projViewMat=(projMat*viewMat);


	double nearz,farz;
	GetNearFar(nearz,farz);


	YsVec3 tfm[2];
	viewMat.Mul(tfm[0],p1,1.0);
	viewMat.Mul(tfm[1],p2,1.0);
	if(tfm[0].z()>-nearz || tfm[1].z()>-nearz)
	{
		YsVec3 clp[2];
		if(YSOK!=YsClipLineWithNearZ(clp[0],clp[1],tfm[0],tfm[1],-nearz,YSFALSE))
		{
			return YSFALSE;
		}
		YsTransform3DCoordToScreenCoord(prjPos,clp[0],viewport,projMat);
		v2d[0].Set(prjPos.x(),prjPos.y());
		v2dZ[0]=prjPos.z();
		YsTransform3DCoordToScreenCoord(prjPos,clp[1],viewport,projMat);
		v2d[1].Set(prjPos.x(),prjPos.y());
		v2dZ[1]=prjPos.z();
	}
	else
	{
		YsTransform3DCoordToScreenCoord(prjPos,p1,viewport,projViewMat);
		v2d[0].Set(prjPos.x(),prjPos.y());
		v2dZ[0]=prjPos.z();
		YsTransform3DCoordToScreenCoord(prjPos,p2,viewport,projViewMat);
		v2d[1].Set(prjPos.x(),prjPos.y());
		v2dZ[1]=prjPos.z();
	}

	int winWid,winHei;
	FsGetWindowSize(winWid,winHei);
	mos.Set(mx,winHei-1-my);

	if((v2d[0]-mos).GetSquareLength()<(double)(rad*rad))
	{
		z=v2dZ[0];
		return YSTRUE;
	}
	else if((v2d[1]-mos).GetSquareLength()<(double)(rad*rad))
	{
		z=v2dZ[1];
		return YSTRUE;
	}
	else if(YsGetNearestPointOnLine2(nea,v2d[0],v2d[1],mos)==YSOK &&
	        YsCheckInBetween2(nea,v2d[0],v2d[1])==YSTRUE &&
	        (nea-mos).GetSquareLength()<(double)(rad*rad))
	{
		YsVec3 org,vec,itsc,itsc2;

		org.Set(mos.x(),mos.y(),-1.0);
		YsTransformScreenCoordTo3DCoord(org,org,viewport,projViewMat);
		vec.Set(mos.x(),mos.y(), 1.0);
		YsTransformScreenCoordTo3DCoord(vec,vec,viewport,projViewMat);
		vec-=org;
		vec.Normalize();

		if(YsGetNearestPointOfTwoLine(itsc,itsc2,p1,p2,org,org+vec)==YSOK)
		{
			YsVec3 prjPos;
			YsTransform3DCoordToScreenCoord(prjPos,itsc,viewport,projViewMat);
			z=prjPos.z();
			return YSTRUE;
		}
	}
	return YSFALSE;
}

YSBOOL FsGui3DDrawingEnvironment::IsPointPicked(double &z,int mx,int my,int rad,const YsVec3 &p1) const
{
	int viewport[4];
	YsVec2 mos;
	YsVec2 v2d;
	YsVec3 prjPos;

	GetOpenGlCompatibleViewport(viewport);
	const YsMatrix4x4 &projMat=GetProjectionMatrix();
	const YsMatrix4x4 &viewMat=GetViewMatrix();
	const YsMatrix4x4 projViewMat=(projMat*viewMat);


	double nearz,farz;
	GetNearFar(nearz,farz);
	YsVec3 tfm;
	viewMat.Mul(tfm,p1,1.0);
	if(tfm.z()>-nearz)
	{
		return YSFALSE;
	}


	YsTransform3DCoordToScreenCoord(prjPos,p1,viewport,projViewMat);
	v2d.Set(prjPos.x(),prjPos.y());

	int winWid,winHei;
	FsGetWindowSize(winWid,winHei);
	mos.Set(mx,winHei-1-my);

	if((v2d-mos).GetSquareLength()<(double)(rad*rad))
	{
		z=prjPos.z();
		return YSTRUE;
	}
	return YSFALSE;
}

YSBOOL FsGui3DDrawingEnvironment::IsPointWithinRect(double &z,int x0,int y0,int x1,int y1,const YsVec3 &p) const
{
	int viewport[4];
	YsVec2 v2d;
	YsVec3 prjPos;

	GetOpenGlCompatibleViewport(viewport);
	const YsMatrix4x4 &projMat=GetProjectionMatrix();
	const YsMatrix4x4 &viewMat=GetViewMatrix();
	const YsMatrix4x4 projViewMat=(projMat*viewMat);


	double nearz,farz;
	GetNearFar(nearz,farz);
	YsVec3 tfm;
	viewMat.Mul(tfm,p,1.0);
	if(tfm.z()>-nearz)
	{
		return YSFALSE;
	}


	YsTransform3DCoordToScreenCoord(prjPos,p,viewport,projViewMat);
	v2d.Set(prjPos.x(),prjPos.y());

	int winWid,winHei;
	FsGetWindowSize(winWid,winHei);

	y0=winHei-1-y0;
	y1=winHei-1-y1;

	const YsVec2 minp(YsSmaller<int>(x0,x1),YsSmaller<int>(y0,y1));
	const YsVec2 maxp(YsGreater<int>(x0,x1),YsGreater<int>(y0,y1));

	if(minp.x()<=v2d.x() && v2d.x()<=maxp.x() && minp.y()<=v2d.y() && v2d.y()<=maxp.y())
	{
		z=prjPos.z();
		return YSTRUE;
	}
	return YSFALSE;
}

YSBOOL FsGui3DDrawingEnvironment::IsLineWithinRect(double &z,int x0,int y0,int x1,int y1,const YsVec3 &p1,const YsVec3 &p2) const
{
	const YsVec3 edVtPos[2]={p1,p2};
	return IsLineWithinRect(z,x0,y0,x1,y1,edVtPos);
}

YSBOOL FsGui3DDrawingEnvironment::IsLineWithinRect(double &z,int x0,int y0,int x1,int y1,const YsVec3 edVtPos[2]) const
{
	double minz=YsInfinity;
	for(YSSIZE_T idx=0; idx<2; ++idx)
	{
		double z;
		if(YSTRUE!=IsPointWithinRect(z,x0,y0,x1,y1,edVtPos[idx]))
		{
			return YSFALSE;
		}
		if(0==idx || z<minz)
		{
			minz=z;
		}
	}
	z=minz;
	return YSTRUE;
}

YSBOOL FsGui3DDrawingEnvironment::IsPolygonWithinRect(double &z,int x0,int y0,int x1,int y1,YSSIZE_T np,const YsVec3 p[]) const
{
	double minz=YsInfinity;
	for(YSSIZE_T idx=0; idx<np; ++idx)
	{
		double z;
		if(YSTRUE!=IsPointWithinRect(z,x0,y0,x1,y1,p[idx]))
		{
			return YSFALSE;
		}
		if(0==idx || z<minz)
		{
			minz=z;
		}
	}
	z=minz;
	return YSTRUE;
}

YSBOOL FsGui3DDrawingEnvironment::IsPointWithinPolygon(double &z,const YsShell2d &shl,const YsShell2dLattice &ltc,const YsVec3 &p) const
{
	int viewport[4];
	YsVec2 v2d;
	YsVec3 prjPos;

	GetOpenGlCompatibleViewport(viewport);
	const YsMatrix4x4 &projMat=GetProjectionMatrix();
	const YsMatrix4x4 &viewMat=GetViewMatrix();
	const YsMatrix4x4 projViewMat=(projMat*viewMat);


	double nearz,farz;
	GetNearFar(nearz,farz);
	YsVec3 tfm;
	viewMat.Mul(tfm,p,1.0);
	if(tfm.z()>-nearz)
	{
		return YSFALSE;
	}


	YsTransform3DCoordToScreenCoord(prjPos,p,viewport,projViewMat);
	v2d.Set(prjPos.x(),prjPos.y());

	int winWid,winHei;
	FsGetWindowSize(winWid,winHei);

	double y=(double)(winHei-1)-v2d.y();
	v2d.SetY(y);

	if(YSINSIDE==ltc.CheckInsidePolygon(v2d))
	{
		z=prjPos.z();
		return YSTRUE;
	}
	return YSFALSE;
}

YSBOOL FsGui3DDrawingEnvironment::IsLineWithinPolygon(double &z,const YsShell2d &shl,const YsShell2dLattice &ltc,const YsVec3 &p1,const YsVec3 &p2) const
{
	const YsVec3 edVtPos[2]={p1,p2};
	return IsLineWithinPolygon(z,shl,ltc,edVtPos);
}
YSBOOL FsGui3DDrawingEnvironment::IsLineWithinPolygon(double &z,const YsShell2d &shl,const YsShell2dLattice &ltc,const YsVec3 edVtPos[2]) const
{
	double minz=YsInfinity;
	for(YSSIZE_T idx=0; idx<2; ++idx)
	{
		double z;
		if(YSTRUE!=IsPointWithinPolygon(z,shl,ltc,edVtPos[idx]))
		{
			return YSFALSE;
		}
		if(0==idx || z<minz)
		{
			minz=z;
		}
	}
	z=minz;
	return YSTRUE;
}
YSBOOL FsGui3DDrawingEnvironment::IsPolygonWithinPolygon(double &z,const YsShell2d &shl,const YsShell2dLattice &ltc,YSSIZE_T np,const YsVec3 p[]) const
{
	double minz=YsInfinity;
	for(YSSIZE_T idx=0; idx<np; ++idx)
	{
		double z;
		if(YSTRUE!=IsPointWithinPolygon(z,shl,ltc,p[idx]))
		{
			return YSFALSE;
		}
		if(0==idx || z<minz)
		{
			minz=z;
		}
	}
	z=minz;
	return YSTRUE;
}

void FsGui3DDrawingEnvironment::UpdateNearFar(void)
{
	YsVec3 corner[8],min,max;
	YsBoundingBoxMaker3 bbx;
	YsMatrix4x4 modeling,viewing,tfm;

	corner[0].Set(objBbxMin.x(),objBbxMin.y(),objBbxMin.z());
	corner[1].Set(objBbxMin.x(),objBbxMin.y(),objBbxMax.z());
	corner[2].Set(objBbxMin.x(),objBbxMax.y(),objBbxMin.z());
	corner[3].Set(objBbxMin.x(),objBbxMax.y(),objBbxMax.z());
	corner[4].Set(objBbxMax.x(),objBbxMin.y(),objBbxMin.z());
	corner[5].Set(objBbxMax.x(),objBbxMin.y(),objBbxMax.z());
	corner[6].Set(objBbxMax.x(),objBbxMax.y(),objBbxMin.z());
	corner[7].Set(objBbxMax.x(),objBbxMax.y(),objBbxMax.z());

	viewing.Initialize();
	viewing.Translate(0.0,0.0,-GetViewDistance());
	viewing.Rotate(0.0,0.0,1.0,-GetViewAttitude().b());
	viewing.Rotate(1.0,0.0,0.0,-GetViewAttitude().p());
	viewing.Rotate(0.0,1.0,0.0,-GetViewAttitude().h());
	viewing.Translate(-GetViewTarget());

	tfm=viewing*modeling;

	for(int i=0; i<8; i++)
	{
		tfm.Mul(corner[i],corner[i],1.0);
	}
	bbx.Make(8,corner);
	bbx.Get(min,max);

	double nearZ=max.z()/1.02;
	double farZ=min.z()*1.02;

	if(nearZ>=farZ/1000.0)
	{
		nearZ=farZ/1000.0;
	}

	nearZ*=-0.2;
	farZ*=-5.0;

	SetNearFar(nearZ,farZ);
}


FsGui3DViewControl::FsGui3DViewControl()
{
	Initialize();
}

void FsGui3DViewControl::Initialize(void)
{
	needRedraw=YSFALSE;
	viewControlMode=YS_STANDARD;
	dragMode=DRAG_NOTDRAGGING;
	actuallyChangedViewByDragging=YSFALSE;

	prevLb=YSFALSE;
	prevMb=YSFALSE;
	prevRb=YSFALSE;
	prevMx=0;
	prevMy=0;

	prevTouch.CleanUp();

	orientationLock=YSFALSE;
	translationLock=YSFALSE;

	viewControlDialog=NULL;
	canvas=NULL;

	YsAtt3 att0=GetStraightAttitude();

	f1Att=att0;

	f2Att=att0;
	f2Att.NoseUp(-YsPi/2.0);

	f3Att=att0;
	f3Att.YawLeft(YsPi/2.0);

	f4Att=att0;
	f4Att.YawLeft(YsPi);

	f5Att=att0;
	f5Att.NoseUp(YsPi/2.0);

	f6Att=att0;
	f6Att.YawLeft(-YsPi/2.0);
}

void FsGui3DViewControl::AttachCanvas(class FsGuiCanvas *canvas)
{
	this->canvas=canvas;
}

void FsGui3DViewControl::AttachViewControlDialog(class FsGui3DViewControlDialogBase *viewControlDialog)
{
	this->viewControlDialog=viewControlDialog;
}

void FsGui3DViewControl::SetViewControlMode(VIEWCONTROLMODE viewControlMode)
{
	this->viewControlMode=viewControlMode;
}

void FsGui3DViewControl::SetF1ViewAttitude(const YsAtt3 &att)
{
	f1Att=att;
}

void FsGui3DViewControl::SetF2ViewAttitude(const YsAtt3 &att)
{
	f2Att=att;
}

void FsGui3DViewControl::SetF3ViewAttitude(const YsAtt3 &att)
{
	f3Att=att;
}

void FsGui3DViewControl::SetF4ViewAttitude(const YsAtt3 &att)
{
	f4Att=att;
}

void FsGui3DViewControl::SetF5ViewAttitude(const YsAtt3 &att)
{
	f5Att=att;
}

void FsGui3DViewControl::SetF6ViewAttitude(const YsAtt3 &att)
{
	f6Att=att;
}

FsGui3DViewControl::VIEWCONTROLMODE FsGui3DViewControl::GetViewControlMode(void) const
{
	return viewControlMode;
}

YSRESULT FsGui3DViewControl::SetMouseState(YSBOOL lb,YSBOOL mb,YSBOOL rb,int mx,int my)
{
	YSRESULT res=YSERR;

	if(NULL!=viewControlDialog && YSTRUE==viewControlDialog->SingleButtonRotation())
	{
		if(YSTRUE==lb)
		{
			if(DRAG_ROTATION!=dragMode)
			{
				dragMode=DRAG_ROTATION;
				actuallyChangedViewByDragging=YSFALSE;
			}
			if(YSTRUE==prevLb)
			{
				Drag(mx,my,prevMx,prevMy);
				res=YSOK;  // 2014/10/17 was missing and added.
			}
		}
		else
		{
			// 2014/10/20
			// The following res=YSOK was added when OnLButtonUp was received without OnMouseMove during
			// recording a sketch stroke.  This successfully prevented firing of OnLButtonUp after
			// dragging in single-button rotation/translation.  However, it also prevented LButtonUp event
			// from being sent to dialog items.
			// Sketch stroke recorder should handle empty dragging.  And, this case must not return YSOK.

			// This must not return YSOK.  LButtonUp may need to be taken by a dialog.
			// if(dragMode!=DRAG_NOTDRAGGING)  // 2014/10/17 was missing and added.
			// {
			// 	res=YSOK;  // 2014/10/17 was missing and added.
			// }

			// 2015/12/08
			//   Unless this returns YSOK, vertices may be selected while view is rotated or translated.
			//   It should return YSOK if the view has actually be changed by dragging.
			if(YSTRUE==actuallyChangedViewByDragging)
			{
				res=YSOK;
			}
			dragMode=DRAG_NOTDRAGGING;
		}
	}
	else if(NULL!=viewControlDialog && YSTRUE==viewControlDialog->SingleButtonScroll())
	{
		if(YSTRUE==lb)
		{
			if(DRAG_SCROLL!=dragMode)
			{
				dragMode=DRAG_SCROLL;
				actuallyChangedViewByDragging=YSFALSE;
			}
			if(YSTRUE==prevLb)
			{
				Drag(mx,my,prevMx,prevMy);
				res=YSOK;  // 2014/10/17 was missing and added.
			}
		}
		else
		{
			// 2014/10/20 Commented out after all.  See comments above.
			// This must not return YSOK.  LButtonUp may need to be taken by a dialog.
			// if(dragMode!=DRAG_NOTDRAGGING)  // 2014/10/17 was missing and added.
			// {
			// 	res=YSOK;  // 2014/10/17 was missing and added.
			// }

			// 2015/12/08
			//   Unless this returns YSOK, vertices may be selected while view is rotated or translated.
			//   It should return YSOK if the view has actually be changed by dragging.
			if(YSTRUE==actuallyChangedViewByDragging)
			{
				res=YSOK;
			}
			dragMode=DRAG_NOTDRAGGING;
		}
	}
	else
	{
		switch(viewControlMode)
		{
		case YS_STANDARD:
			res=SetMouseStateYsStandard(lb,mb,rb,mx,my);
			break;
		case CATIA_COMPATIBLE:
			res=SetMouseStateCatiaCompatible(lb,mb,rb,mx,my);
			break;
		}
	}

	prevLb=lb;
	prevRb=rb;
	prevMb=mb;
	prevMx=mx;
	prevMy=my;

	return res;
}

YSRESULT FsGui3DViewControl::SetMouseStateYsStandard(YSBOOL lb,YSBOOL mb,YSBOOL rb,int mx,int my)
{
	if(0!=FsGetKeyState(FSKEY_SHIFT) && (YSTRUE==lb || YSTRUE==rb))
	{
		if(YSTRUE==lb && YSTRUE==rb)
		{
			dragMode=DRAG_ZOOM;
		}
		else if(YSTRUE==lb)
		{
			dragMode=DRAG_SCROLL;
		}
		else if(YSTRUE==rb)
		{
			dragMode=DRAG_ROTATION;
		}

		if(YSTRUE==prevLb || YSTRUE==prevRb)
		{
			Drag(mx,my,prevMx,prevMy);
		}

		return YSOK;
	}
	else
	{
		dragMode=DRAG_NOTDRAGGING;
	}
	return YSERR;
}

YSRESULT FsGui3DViewControl::SetMouseStateCatiaCompatible(YSBOOL lb,YSBOOL mb,YSBOOL rb,int mx,int my)
{
	if(YSTRUE==mb)
	{
		if(YSTRUE!=prevMb)
		{
			dragMode=DRAG_SCROLL;
		}
		if(YSTRUE!=prevLb && YSTRUE==lb)
		{
			switch(dragMode)
			{
			default:
				break;
			case DRAG_SCROLL:
				dragMode=DRAG_ROTATION;
				break;
			case DRAG_ROTATION:
				dragMode=DRAG_ZOOM;
				break;
			case DRAG_ZOOM:
				dragMode=DRAG_SCROLL;
				break;
			}
		}

		if(YSTRUE==prevMb)
		{
			Drag(mx,my,prevMx,prevMy);
		}
		return YSOK;
	}
	else
	{
		dragMode=DRAG_NOTDRAGGING;
	}
	return YSERR;
}

void FsGui3DViewControl::Drag(int mx,int my,int prevMx,int prevMy)
{
	if(DRAG_NOTDRAGGING!=dragMode && (prevMx!=mx || prevMy!=my))
	{
		actuallyChangedViewByDragging=YSTRUE;
	}
	switch(dragMode)
	{
	default:
		break;
	case DRAG_ROTATION:
		RotateByDragging(mx,my,prevMx,prevMy);
		SetNeedRedraw(YSTRUE);
		break;
	case DRAG_SCROLL:
		TranslateByDragging(mx,my,prevMx,prevMy);
		SetNeedRedraw(YSTRUE);
		break;
	case DRAG_ZOOM:
		ZoomByDragging(mx,my,prevMx,prevMy);
		SetNeedRedraw(YSTRUE);
		break;
	}
}

void FsGui3DViewControl::RotateByDragging(int mx,int my,int prevMx,int prevMy)
{
	if(YSTRUE!=orientationLock)
	{
		const double dx=-double(mx-prevMx);
		const double dy=-double(my-prevMy);

		int wid,hei;
		FsGetWindowSize(wid,hei);

	    const double lateral=double(dx)*7.0/double(wid);
	    const double longitudinal=double(dy)*7.0/double(hei);
	    RotateView(lateral,longitudinal);
	}
}

void FsGui3DViewControl::ZoomByDragging(int mx,int my,int prevMx,int prevMy)
{
	// const double dx=-double(mx-prevMx);
	const double dy= double(my-prevMy);

	int wid,hei;
	FsGetWindowSize(wid,hei);

    const double zoomFactor=1.0-double(dy)*5.0/double(hei);
    if(zoomFactor>0)
    {
        Zoom(zoomFactor);
    }
}

void FsGui3DViewControl::TranslateByDragging(int mx,int my,int prevMx,int prevMy)
{
	if(YSTRUE!=translationLock)
	{
		const double dx=-double(mx-prevMx);
		const double dy= double(my-prevMy);
		Scroll(dx,dy);
	}
}

void FsGui3DViewControl::Scroll(double dx,double dy)
{
	int wid,hei;
	FsGetWindowSize(wid,hei);

	double vx,vy;
	if(GetPerspective()!=YSTRUE)
	{
		double x1,y1,x2,y2;
		GetOrthogonalRangeWithZoom(x1,y1,x2,y2);
		const double sx=fabs(x1-x2);
		const double sy=fabs(y1-y2);

		vx=dx*sx/double(wid);
		vy=dy*sy/double(hei);
	}
	else
	{
		const double tanfov=tan(GetFOVY());
		const double viewDist=GetViewDistance();
		vx=2.0*(dx*viewDist*tanfov/(double)hei)/GetZoom();
		vy=2.0*(dy*viewDist*tanfov/(double)hei)/GetZoom();
	}

	YsVec3 v(vx,vy,0.0);
	GetViewMatrix().MulInverse(v,v,0.0);
	v+=GetViewTarget();

	SetViewTarget(v);
	SetNeedRedraw(YSTRUE);
}

YSRESULT FsGui3DViewControl::SetTouchState(YSSIZE_T nTouch,const FsVec2i touch[])
{
	YSRESULT res=YSERR;
	auto newTouch=MatchTouch(nTouch,touch);

	if(2==newTouch.GetN() && 2==prevTouch.GetN())
	{
		double prevD=(prevTouch[0]-prevTouch[1]).GetLength();
		double d=(newTouch[0]-newTouch[1]).GetLength();

		if(YsTolerance<prevD)
		{
			const double scale=d/prevD;
			if(scale<0.95 || 1.05<scale)
			{
				Zoom(scale);
				res=YSOK;
			}
		}

		auto prevC=(prevTouch[0]+prevTouch[1])/2.0;
		auto c=(newTouch[0]+newTouch[1])/2.0;
		if(10.0<(prevC-c).GetLength() && YSTRUE!=translationLock)
		{
			auto d=c-prevC;
			Scroll(-d.x(),d.y());
			res=YSOK;
		}
	}
	else if(3==newTouch.GetN() && 3==prevTouch.GetN())
	{
		auto prevC=(prevTouch[0]+prevTouch[1]+prevTouch[2])/3.0;
		auto c=(newTouch[0]+newTouch[1]+newTouch[2])/3.0;
		if(10.0<(prevC-c).GetLength() && YSTRUE!=orientationLock)
		{
			auto d=prevC-c;

			int wid,hei;
			FsGetWindowSize(wid,hei);

		    const double lateral=double(d.x())*7.0/double(wid);
		    const double longitudinal=double(d.y())*7.0/double(hei);
		    RotateView(lateral,longitudinal);
		    res=YSOK;
		}

		// Bank Rotation based on the farthest-from-center point.
		int farIndex=0;
		double farDist=(prevTouch[0]-prevC).GetSquareLength();
		for(int i=1; i<3; ++i)
		{
			double dist=(prevTouch[i]-prevC).GetSquareLength();
			if(dist>farDist)
			{
				farIndex=i;
				farDist=dist;
			}
		}

		auto prevDir=YsUnitVector(prevTouch[farIndex]-prevC);
		auto newDir=YsUnitVector(newTouch[farIndex]-c);

		double angle=acos(YsBound(prevDir*newDir,-1.0,1.0));
		if(YsDegToRad(1.0)<angle && YSTRUE!=orientationLock)
		{
			if((prevDir^newDir)<0.0)
			{
				angle=-angle;
			}
			auto att=GetViewAttitude();
			att.AddB(angle);
			SetViewAttitude(att);
			res=YSOK;
		}
	}

	if(YSOK==res || prevTouch.GetN()!=newTouch.GetN())
	{
		prevTouch=newTouch;
	}
	if(YSOK==res)
	{
		SetNeedRedraw(YSTRUE);
	}
	return res;
}
YsArray <YsVec2> FsGui3DViewControl::MatchTouch(YSSIZE_T nTouch,const FsVec2i touch[])
{
	YsArray <YsVec2> newTouch;
	YsArray <int> touchIndex;

	for(int i=0; i<nTouch; ++i)
	{
		newTouch.Increment();
		newTouch.Last().Set(touch[i].x(),touch[i].y());

		int idx=0x7fffffff;
		double nearDist=YsInfinity;
		for(int j=0; j<prevTouch.GetN(); ++j)
		{
			double dist=(newTouch.Last()-prevTouch[j]).GetSquareLength();
			if(0x7fffffff==idx || dist<nearDist)
			{
				idx=j;
			}
		}

		touchIndex.Add(idx);
	}

	YsSimpleMergeSort(touchIndex.GetN(),touchIndex.GetEditableArray(),newTouch.GetEditableArray());
	return newTouch;
}

YSRESULT FsGui3DViewControl::KeyIn(int fsKey,YSBOOL shift,YSBOOL ctrl,YSBOOL alt)
{
	switch(fsKey)
	{
	case FSKEY_F1:
		SetViewAttitude(f1Att);
		SetNeedRedraw(YSTRUE);
		return YSOK;
	case FSKEY_F2:
		SetViewAttitude(f2Att);
		SetNeedRedraw(YSTRUE);
		return YSOK;
	case FSKEY_F3:
		SetViewAttitude(f3Att);
		SetNeedRedraw(YSTRUE);
		return YSOK;
	case FSKEY_F4:
		SetViewAttitude(f4Att);
		SetNeedRedraw(YSTRUE);
		return YSOK;
	case FSKEY_F5:
		SetViewAttitude(f5Att);
		SetNeedRedraw(YSTRUE);
		return YSOK;
	case FSKEY_F6:
		SetViewAttitude(f6Att);
		SetNeedRedraw(YSTRUE);
		return YSOK;
	case FSKEY_WHEELUP:
		Zoom(1.05);
		SetNeedRedraw(YSTRUE);
		return YSOK;
	case FSKEY_WHEELDOWN:
		Zoom(1.0/1.05);
		SetNeedRedraw(YSTRUE);
		return YSOK;
	case FSKEY_F7:
		Zoom(1.1);
		SetNeedRedraw(YSTRUE);
		return YSOK;
	case FSKEY_F8:
		Zoom(1.0/1.1);
		SetNeedRedraw(YSTRUE);
		return YSOK;
	case FSKEY_LEFT:
		if(YSTRUE==alt || (YSTRUE==shift && YSTRUE==ctrl))
		{
			int wid,hei;
			FsGetWindowSize(wid,hei);
			auto d=YsSmaller(wid,hei);
			Scroll(-d/10,0);
			return YSOK;
		}
		break;
	case FSKEY_RIGHT:
		if(YSTRUE==alt || (YSTRUE==shift && YSTRUE==ctrl))
		{
			int wid,hei;
			FsGetWindowSize(wid,hei);
			auto d=YsSmaller(wid,hei);
			Scroll(d/10,0);
			return YSOK;
		}
		break;
	case FSKEY_UP:
		if(YSTRUE==alt || (YSTRUE==shift && YSTRUE==ctrl))
		{
			int wid,hei;
			FsGetWindowSize(wid,hei);
			auto d=YsSmaller(wid,hei);
			Scroll(0,d/10);
			return YSOK;
		}
		break;
	case FSKEY_DOWN:
		if(YSTRUE==alt || (YSTRUE==shift && YSTRUE==ctrl))
		{
			int wid,hei;
			FsGetWindowSize(wid,hei);
			auto d=YsSmaller(wid,hei);
			Scroll(0,-d/10);
			return YSOK;
		}
		break;
	}
	return YSERR;
}

void FsGui3DViewControl::SetOrientationLock(YSBOOL lock)
{
	if(lock!=orientationLock)  // This check is needed to avoid infinite call backs between this and viewControlDialog.
	{
		orientationLock=lock;
		if(NULL!=viewControlDialog)
		{
			viewControlDialog->SetOrientationLock(lock);
		}
	}
}

void FsGui3DViewControl::SetTranslationLock(YSBOOL lock)
{
	if(lock!=translationLock)  // This check is needed to avoid infinite call backs between this and viewControlDialog.
	{
		translationLock=lock;
		if(NULL!=viewControlDialog)
		{
			viewControlDialog->SetTranslationLock(lock);
		}
	}
}

void FsGui3DViewControl::SetViewTarget(const YsVec3 &pos)
{
	FsGui3DDrawingEnvironment::SetViewTarget(pos);
	if(NULL!=viewControlDialog)
	{
		viewControlDialog->UpdateViewTarget(pos);
	}
}

void FsGui3DViewControl::SetNeedRedraw(YSBOOL flag)
{
	needRedraw=flag;
	if(NULL!=canvas)
	{
		canvas->SetNeedRedraw(flag);
	}
}

YSBOOL FsGui3DViewControl::NeedRedraw(void) const
{
	return needRedraw;
}
