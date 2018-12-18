/* ////////////////////////////////////////////////////////////

File Name: ysviewcontrol.h
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

#ifndef YSVIEWCONTROL_IS_INCLUDED
#define YSVIEWCONTROL_IS_INCLUDED
/* { */

#include "ysgeometry.h"

/*! This class stores a view point and viewing attitude. */
class YsViewPoint
{
private:
	YsVec3 viewTarget;
	YsAtt3 viewAttitude;
	double viewDistance;

	mutable YSBOOL matrixCached;
	mutable YsMatrix4x4 matrix;

public:
	/*! Default constructor. */
	YsViewPoint();

	/*! This function returns actual viewing (camera) position in pos. */
	const YsVec3 &GetViewPosition(YsVec3 &pos) const;

	/*! This function returns viewing direction in vec. */
	const YsVec3 &GetViewDirection(YsVec3 &vec) const;

	/*! This function returns actual viewing (camera) position in pos. */
	const YsVec3 GetViewPosition(void) const;

	/*! This function returns viewing direction in vec. */
	const YsVec3 GetViewDirection(void) const;

	/*! This function returns the viewing attitude. */
	const YsAtt3 &GetViewAttitude(void) const;

	/*! This function sets the viewing attitude. */
	YSRESULT SetViewAttitude(const YsAtt3 &att);

	/*! This function sets the viewing attitude by a direction. */
	YSRESULT SetViewDirection(const YsVec3 &dir,const double &bank=0.0);

	/*! This function returns the viewing target. */
	const YsVec3 &GetViewTarget(void) const;

	/*! This function sets the viewint  target. */
	YSRESULT SetViewTarget(const YsVec3 &pos);

	/*! This function sets the distance from the viewing target to the camera. */
	YSRESULT SetViewDistance(const double &viewDist);

	/*! This function returns the distance from the viewing target to the camera. */
	const double &GetViewDistance(void) const;

	/*! This function rotates the camera.  The camera orbits around the viewing target
	    by this function while maintaining the specified distance. */
	YSRESULT RotateView(double lateral,double longitudinal);

	/*! This function returns the view matrix.  From this matrix, an array of 
	    OpenGL-compatible matrix can be obtained by using GetOpenGlCompatibleMatirx
	    member function of YsMatrix4x4. If there is no model transformation, 
	    the matrix can directly be given as a ModelView matrix. */
	const YsMatrix4x4 &GetViewMatrix(void) const;
};

/*! This function retains an information for projection transformation. */
class YsProjectionTransformation
{
public:
	enum PROJMODE
	{
		PERSPECTIVE,
		ORTHOGONAL
	};

private:
	PROJMODE projectionMode;  // ORTHOGONAL or PERSPECTIVE

	double nearz,farz;
	double aspect;
	double fovy;    // For perspective
	double height;  // For orthographic

	double zoom;

	mutable YSBOOL matrixCached;
	mutable YsMatrix4x4 matrix;

public:
	/*! Default constructor. */
	YsProjectionTransformation();

	/*! This function sets the projection mode to either YsProjectionTransformation::PERSPECTIVE or 
	    YsProjectionTransformation::ORTHOGONAL. */
	void SetProjectionMode(PROJMODE mode);

	/*! This function returns the current projection mode. */
	PROJMODE GetProjectionMode(void) const;

	/*! This function sets aspect ratio (width/height) of the viewport.  */
	void SetAspectRatio(const double &aspect);

	/*! This function sets field-of-view in Y direction for perspective projection in radian. 
	    This function does NOT change the projection mode.  */
	void SetFOVY(const double &fovy);

	/*! This function returns aspect ratio set by SetAspectRatio function. */
	const double GetAspectRatio(void) const;

	/*! This function returns field-of-view in Y direction for perspective projection. */
	const double GetFOVY(void) const;

	/*! This function sets the range of orthogonal projection.
	    This function does NOT change the projection mode.  */
	void SetOrthogonalProjectionHeight(const double &height);

	/*! This function returns the range of orthogonal projection. */
	const double GetOrthogonalProjectionHeight(void) const;

	/*! This function returns the range of orthogonal projection after zoom factor is taken into account. */
	void GetOrthogonalRangeWithZoom(double &left,double &right,double &top,double &bottom) const;

	/*! This function sets near- and far- clip-plane distances. */
	void SetNearFar(const double nearz,const double farz);

	/*! This function returns near- and far- clip-plane distances. */
	void GetNearFar(double &nearz,double &farz) const;

	/*! This function returns the zooming factor. */
	const double &GetZoom(void) const;

	/*! This function sets the zooming factor. */
	void SetZoom(const double &zoom);

	/*! This function returns a projection matrix.  A OpenGL comopatible matrix can be 
	    obtained by GetOpenGlCompatibleMatirx of YsMatrix4x4 class, which can directly 
	    given to OpenGL as a projection matrix. */
	const YsMatrix4x4 &GetProjectionMatrix(void) const;
};

/*! This class has a viewport, view point, projection transformation, and object bounding box.
    Is capable of calculating transformation for OpenGL drawing.
    Picking calculation can also be done in this class. */
class Ys3DDrawingEnvironment : public YsViewPoint, public YsProjectionTransformation
{
private:
	int viewport[4];
	int windowWidth,windowHeight;
	YSBOOL screenOriginIsTopLeft;

public:
	/*! Default constructor */
	Ys3DDrawingEnvironment();

	/*! This function sets the window size. */
	void SetWindowSize(int wid,int hei);

	/*! This function returns the window size. */
	void GetWindowSize(int &wid,int &hei) const;

	/*! This function sets viewport by two corner points in the screen coordinate. */
	void SetViewportByTwoCorner(int x0,int y0,int x1,int y1);

	/*! This function returns the viewport by two corner points in the screen coordinate. */
	void GetViewportByTwoCorner(int &x0,int &y0,int &x1,int &y1) const;

	/*! This function returns the OpenGL compatible viewport, that can directly be given
	    to glViewport function. */
	void GetOpenGlCompatibleViewport(int viewport[4]) const;

private:
	void UpdateAspectRatio(void);

public:
	/*! This function changes the vertical orientation of the screen coordinate system.
	    If screenOriginIsTopLeft is YSTRUE, screen coordinate (0,0) is top-left corner,
	    and positive-Y direction goes down.  Or, If screenOriginIsTopLeft is YSFALSE,
	    screen coordinate (0,0) is bottom-left corner, and positive-Y direction goes up. 
	    By default, this flat is set to YSTRUE. */
	void SetVerticalOrientation(YSBOOL screenOriginIsTopLeft);

	/*! This function transforms pos by *modelingTfm, the viewing matrix, and the projection matrix, and returns the transformed coordinate in scrn.
        i.e.,
               scrn=GetProjectionMatrix()*GetViewMatrix()*(*modelingTfm)*pos;
	    If the modeling transformation is an identity matrix, modelingTfm can be NULL.
	    */
	const YsVec3 &Convert3DCoordToScreenCoord(YsVec3 &scrn,const YsVec3 &pos,const YsMatrix4x4 *modelingTfm) const;

	/*! This function transforms pos by *modelingTfm, the viewing matrix, and the projection matrix, and the returns the transformed coordinate.
        i.e.,
               scrn=GetProjectionMatrix()*GetViewMatrix()*(*modelingTfm)*pos;
	    If the modeling transformation is an identity matrix, modelingTfm can be NULL.
	    */
	const YsVec3 Convert3DCoordToScreenCoord(const YsVec3 &pos,const YsMatrix4x4 *modelingTfm) const;

	/*! This function transforms a screen coordinate to a 3D line that goes through org and is parallel to vec. */
	void TransformScreenCoordTo3DLine(YsVec3 &org,YsVec3 &vec,double sx,double sy) const;
};


/*! This function transforms a 3D coordinate into a screen (window) coordinate with depth.  
    The projViewModelTfm must be calculated as:
    
        projViewModelTfm=Tproj*Tview*Tmodel

    This function assumes that the origin of the screen coordinate is at the lower left corner.
    */
void YsTransform3DCoordToScreenCoord(YsVec3 &scrn,const YsVec3 &from,const int viewport[4],const YsMatrix4x4 &projViewModelTfm);

/*! This function transforms a screen (window) coordinate into a 3D coordinate. 
    The projViewModelTfm must be calculated as:
    
        projViewModelTfm=Tproj*Tview*Tmodel

    This function assumes that the origin of the screen coordinate is at the lower left corner. */
void YsTransformScreenCoordTo3DCoord(YsVec3 &threeD,const YsVec3 &scrn,const int viewport[4],const YsMatrix4x4 &projViewModelTfm);

/*! This function transforms a view coordinate into screen coordinate by the given viewport. */
void YsViewportTransformation(YsVec3 &vOut,const YsVec3 &vIn,const int viewport[4]);

/*! This function transforms a screen coordinate to a view coordinate by the given viewport. */
void YsInverseViewportTransformation(YsVec3 &vOut,const YsVec3 &vIn,const int viewport[4]);

/*! This function clips the polygon with near-plane and then transform to the screen coordinate. */
template <const int N>
void YsClipAndTransformPolygonFrom3DCoordToScreenCoord(
    YsArray <YsVec3,N> &scrn,const int np,const YsVec3 p[],
    int viewport[4],const YsMatrix4x4 &projMat,const YsMatrix4x4 &modelViewMat,const double nearz,YSBOOL positivez=YSFALSE)
{
	if(scrn.GetArray()==p)
	{
		YsArray <YsVec3,N> plgCopy(np,p);
		return YsClipAndTransformPolygonFrom3DCoordToScreenCoord(scrn,plgCopy,viewport,projMat,modelViewMat);
	}

	YSBOOL needClip=YSFALSE,atLeastVisible=YSFALSE;
	scrn.Set(np,NULL);
	for(int i=0; i<np; ++i)
	{
		modelViewMat.Mul(scrn[i],p[i],1.0);
		if((YSTRUE==positivez && scrn[i].z()<nearz) ||
		   (YSTRUE!=positivez && scrn[i].z()>nearz))
		{
			needClip=YSTRUE;
		}
		else
		{
			atLeastVisible=YSTRUE;
		}
	}

	if(YSTRUE!=atLeastVisible)
	{
		scrn.Clear();
		return;
	}
	else if(YSTRUE==needClip)
	{
		YsArray <YsVec3,N> clipped;
		if(YSTRUE==YsClipPolygonWithNearZ(clipped,scrn.GetN(),scrn,nearz,positivez))
		{
			scrn.Set(clipped.GetN(),NULL);
			for(int i=0; i<clipped.GetN(); ++i)
			{
				const double from[4]={clipped[i].x(),clipped[i].y(),clipped[i].z(),1.0};
				double prj[4];
				projMat.Mul(prj,from);
				scrn[i].Set(prj[0]/prj[3],prj[1]/prj[3],prj[2]/prj[3]);
			}
		}
		else
		{
			scrn.Clear();
		}
	}
	else
	{
		for(int i=0; i<scrn.GetN(); ++i)
		{
			const double from[4]={scrn[i].x(),scrn[i].y(),scrn[i].z(),1.0};
			double prj[4];
			projMat.Mul(prj,from);
			scrn[i].Set(prj[0]/prj[3],prj[1]/prj[3],prj[2]/prj[3]);
		}
	}
}


/*! Returns YSTRUE if points p[0]...p[np-1] is visible in the given coordinate system.
*/
YSBOOL YsIsPointVisible(
    YSSIZE_T np,const YsVec3 p[],
    const YsMatrix4x4 &viewModelTfm,
	const double &nearZ,const double &farZ,const double &tanFov,
	YSCOORDSYSMODEL coordSysModel);

/*! Returns YSTRUE if points p[0]...p[np-1] is visible in the left-hand coordinate system.
    Camera looks toward (0,0,1) in its coordinate system.
*/
YSBOOL YsIsPointVisibleLH(
    YSSIZE_T np,const YsVec3 p[],
    const YsMatrix4x4 &viewModelTfm,
	const double &nearZ,const double &farZ,const double &tanFov);

/*! Returns YSTRUE if points p[0]...p[np-1] is visible in the right-hand coordinate system.
    Camera looks toward (0,0,-1) in its coordinate system.
*/
YSBOOL YsIsPointVisibleRH(
    YSSIZE_T np,const YsVec3 p[],
    const YsMatrix4x4 &viewModelTfm,
	const double &nearZ,const double &farZ,const double &tanFov);

/*! Returns YSTRUE if points p[0]...p[np-1] is visible in the current coordinate system (YsCoordSysModel).
*/
YSBOOL YsIsPointVisible(
    YSSIZE_T np,const YsVec3 p[],
    const YsMatrix4x4 &viewModelTfm,
	const double &nearZ,const double &farZ,const double &tanFov);



/*! Returns YSTRUE if bounding box bbx[0]-bbx[1] is visible in the given coordinate system.
*/
YSBOOL YsIsBoundingBoxVisible(
    const YsVec3 bbx[2],
    const YsMatrix4x4 &viewModelTfm,
	const double &nearZ,const double &farZ,const double &tanFov,
	YSCOORDSYSMODEL coordSysModel);

/*! Returns YSTRUE if bounding box bbx[0]-bbx[1] is visible in the left-hand coordinate system.
    Camera looks toward (0,0,1) in its coordinate system.
*/
YSBOOL YsIsBoundingBoxVisibleLH(
    const YsVec3 bbx[2],
    const YsMatrix4x4 &viewModelTfm,
	const double &nearZ,const double &farZ,const double &tanFov);

/*! Returns YSTRUE if bounding box bbx[0]-bbx[1] is visible in the right-hand coordinate system.
    Camera looks toward (0,0,-1) in its coordinate system.
*/
YSBOOL YsIsBoundingBoxVisibleRH(
    const YsVec3 bbx[2],
    const YsMatrix4x4 &viewModelTfm,
	const double &nearZ,const double &farZ,const double &tanFov);

/*! Returns YSTRUE if bounding box bbx[0]-bbx[1] is visible in the current coordinate system (YsCoordSysModel).
*/
YSBOOL YsIsBoundingBoxVisible(
    const YsVec3 bbx[2],
    const YsMatrix4x4 &viewModelTfm,
	const double &nearZ,const double &farZ,const double &tanFov);


/*! Returns YSTRUE if bounding box bbx[0]-bbx[1] is visible in the current coordinate system model (YsCoordSysModel).
    ProjTfm is a OpenGL-compatible projection matrix.  Therefore, when the current coordinate system model is a left-hand coordinate,
    Z-value is negated after viewModelTfm, and then transformed by ProjTfm.
*/
YSBOOL YsIsPointVisible(
    int np,const YsVec3 pos[],
    const YsMatrix4x4 &viewModelTfm,
    const YsMatrix4x4 &ProjTfm);

/*! Returns YSTRUE if bounding box bbx[0]-bbx[1] is visible in the current coordinate system model (YsCoordSysModel).
    ProjTfm is a OpenGL-compatible projection matrix.  
*/
YSBOOL YsIsPointVisibleRH(
    int np,const YsVec3 pos[],
    const YsMatrix4x4 &viewModelTfm,
    const YsMatrix4x4 &ProjTfm);

/*! Returns YSTRUE if bounding box bbx[0]-bbx[1] is visible in the current coordinate system model (YsCoordSysModel).
    ProjTfm is a OpenGL-compatible projection matrix.  Position pos is transformed by viewModdelTfm, then its Z-value
    is negated, and then transformed by ProjTfm to check if the point is visible.
*/
YSBOOL YsIsPointVisibleLH(
    int np,const YsVec3 pos[],
    const YsMatrix4x4 &viewModelTfm,
    const YsMatrix4x4 &ProjTfm);

/*! Returns YSTRUE if bounding box bbx[0]-bbx[1] is visible in the current coordinate system model (YsCoordSysModel).
    ProjTfm is a OpenGL-compatible projection matrix.  Therefore, when the current coordinate system model is a left-hand coordinate,
    Z-value is negated after viewModelTfm, and then transformed by ProjTfm.
*/
YSBOOL YsIsBoundingBoxVisible(
    const YsVec3 bbx[2],
    const YsMatrix4x4 &viewModelTfm,
    const YsMatrix4x4 &ProjTfm);

/*! Returns YSTRUE if bounding box bbx[0]-bbx[1] is visible in the current coordinate system model (YsCoordSysModel).
    ProjTfm is a OpenGL-compatible projection matrix.
*/
YSBOOL YsIsBoundingBoxVisibleRH(
    const YsVec3 bbx[2],
    const YsMatrix4x4 &viewModelTfm,
    const YsMatrix4x4 &ProjTfm);

/*! Returns YSTRUE if bounding box bbx[0]-bbx[1] is visible in the current coordinate system model (YsCoordSysModel).
    ProjTfm is a OpenGL-compatible projection matrix.  The bounding box is transformed by viewModelTfm, then Z-values
    of the corners are negated, and then transformed by ProjTfm to check if the box is visible.
*/
YSBOOL YsIsBoundingBoxVisibleLH(
    const YsVec3 bbx[2],
    const YsMatrix4x4 &viewModelTfm,
    const YsMatrix4x4 &ProjTfm);

/*! Calculate planes of view-frustums from a OpenGL compatible matrix.
    The normal vector will point outside of the view frustum.
    First four defines left, right, top, and bottom.
    Last two defines near and far.
*/
YSRESULT YsCalcualteViewFrustumPlaneFromProjectionMatrix(YsPlane pln[6],const YsMatrix4x4 &projMat);


/* } */
#endif
