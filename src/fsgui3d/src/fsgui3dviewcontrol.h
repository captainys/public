/* ////////////////////////////////////////////////////////////

File Name: fsgui3dviewcontrol.h
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

#ifndef FSGUI_DVIEWCONTROL_IS_INCLUDED
#define FSGUI_DVIEWCONTROL_IS_INCLUDED
/* { */

#include <ysclass.h>
#include <fssimplewindow.h>


/*! This class makes an expanded version of Ys3DDrawingEnvironment for 
    modeling operation. */
class FsGui3DDrawingEnvironment : public Ys3DDrawingEnvironment
{
private:
	YsVec3 objBbxMin,objBbxMax;
	YsAtt3 straightAttitude;
	YSBOOL zoomByViewDistanceInPerspectiveMode;

public:
	/*! This function updates nearZ and farZ so that it covers all target bounding box. */
	void UpdateNearFar(void);

	/*! Default constructor */
	FsGui3DDrawingEnvironment();

	/*! Default destructor */
	~FsGui3DDrawingEnvironment();

	/*! Resets view transformation. */
	YSRESULT Straighten(void);

	/*! Resets only view attitude.  Does not change the view target. */
	YSRESULT StraightenAttitude(void);

	/*! This function sets the target bounding box, which will be used for calculating nearZ and farZ. */
	YSRESULT SetTargetBoundingBox(const YsVec3 &min,const YsVec3 &max);

	/*! This function sets the target bounding box, which will be used for calculating nearZ and farZ. */
	YSRESULT SetTargetBoundingBox(const YsVec3 minamx[2]);

	/*! This function sets the projection mode to perspective if drawInPerspectiveMode==YSTRUE, or
	    orthographics otherwise. */
	YSRESULT SetPerspective(YSBOOL drawInPerspectiveMode);

	/*! This function returns YSTRUE if the projection mode is perspective, or YSFALSE otherwise. */
	YSBOOL GetPerspective(void) const;

	/*! This function sets the default attitude that is used for resetting the view transformation. */
	YSRESULT SetStraightAttitude(const YsAtt3 &att);

	/*! This function returns straight attitude. */
	const YsAtt3 &GetStraightAttitude(void) const;

	/*! This function returns the target bounding box. */
	YSRESULT GetTargetBoundingBox(YsVec3 &min,YsVec3 &max);

	/*! This function zoom in if factor is greater than 1.0, or zoom out if the factor is less than 1.0. */
	YSRESULT Zoom(double factor);

	/*! This function saves the view configuration to a file. */
	YSRESULT SaveViewpoint(int windowWidth,int windowHeight,const char fn[]);

	/*! This function saves the view configuration to a file. */
	YSRESULT SaveViewpoint(int windowWidth,int windowHeight,YsTextOutputStream &outStream);

	/*! This function reads the view configuration from a file file. */
	YSRESULT LoadViewpoint(int &windowWidth,int &windowHeight,const char fn[]);

	/*! This function reads the view configuration from an input stream. */
	YSRESULT LoadViewpoint(int &windowWidth,int &windowHeight,YsTextInputStream &inStream);

	/*! This function saves the view configuration to an array of YsString. */
	YsArray <YsString> SaveViewpoint(int windowWidth,int windowHeight);

	/*! This function reads the view configuration from an array of YsString. */
	YSRESULT LoadViewpoint(int &windowWidth,int &windowHeight,const YsConstArrayMask <YsString> &view);

	/*! This function returns YSTRUE if (mx,my) is within the polygon. 
	    (mx,my) are calculated from top left corner of the window.  */
	YSBOOL IsPolygonPicked(double &z,int mx,int my,int np,const YsVec3 v[]) const;

	/*! This function returns YSTRUE if (mx,my) hits the line p1-p2.
	    (mx,my) are calculated from top left corner of the window.  */
	YSBOOL IsLinePicked(double &z,int mx,int my,int widthInPixel,const YsVec3 &p1,const YsVec3 &p2) const;

	/*! This function returns YSTRUE if (mx,my) hits the point p.
	    (mx,my) are calculated from top left corner of the window.  */
	YSBOOL IsPointPicked(double &z,int mx,int my,int radius,const YsVec3 &p) const;

	/*! This function returns YSTRUE if the window-coordinate projected from point p is within the rectangle defined by (x0,y0)-(x1,y1). */
	YSBOOL IsPointWithinRect(double &z,int x0,int y0,int x1,int y1,const YsVec3 &p) const;

	/*! This function returns YSTRUE if all points of the given line fits within the rectangle (x0,y0)-(x1,y1). */
	YSBOOL IsLineWithinRect(double &z,int x0,int y0,int x1,int y1,const YsVec3 &p1,const YsVec3 &p2) const;

	/*! This function returns YSTRUE if all points of the given line fits within the rectangle (x0,y0)-(x1,y1). */
	YSBOOL IsLineWithinRect(double &z,int x0,int y0,int x1,int y1,const YsVec3 edVtPos[2]) const;

	/*! This function returns YSTRUE if all points of the given polygon fits within the rectangle (x0,y0)-(x1,y1). */
	YSBOOL IsPolygonWithinRect(double &z,int x0,int y0,int x1,int y1,YSSIZE_T np,const YsVec3 p[]) const;

	/*! This function returns YSTRUE if the point is inside the polygond efined in shell2d. */
	YSBOOL IsPointWithinPolygon(double &z,const YsShell2d &shl,const YsShell2dLattice &ltc,const YsVec3 &p) const;

	/*! This function returns YSTRUE if all points of the given line are inside the polygon defined in shell2d. */
	YSBOOL IsLineWithinPolygon(double &z,const YsShell2d &shl,const YsShell2dLattice &ltc,const YsVec3 &p1,const YsVec3 &p2) const;

	/*! This function returns YSTRUE if all points of the given line are inside the polygon defined in shell2d. */
	YSBOOL IsLineWithinPolygon(double &z,const YsShell2d &shl,const YsShell2dLattice &ltc,const YsVec3 edVtPos[2]) const;

	/*! This function returns YSTRUE if all points of the polygon are inside the polygon defined in shell2d. */
	YSBOOL IsPolygonWithinPolygon(double &z,const YsShell2d &shl,const YsShell2dLattice &ltc,YSSIZE_T np,const YsVec3 p[]) const;
};

/*! This class provides a standard view control. */
class FsGui3DViewControl : public FsGui3DDrawingEnvironment
{
public:
	enum VIEWCONTROLMODE
	{
		YS_STANDARD,
		CATIA_COMPATIBLE
	};

private:
	enum DRAGMODE
	{
		DRAG_NOTDRAGGING,
		DRAG_SCROLL,
		DRAG_ROTATION,
		DRAG_ZOOM
	};

	YSBOOL needRedraw;
	VIEWCONTROLMODE viewControlMode;
	DRAGMODE dragMode;
	YSBOOL actuallyChangedViewByDragging;

	YsAtt3 f1Att,f2Att,f3Att,f4Att,f5Att,f6Att;

	YSBOOL prevLb,prevMb,prevRb;
	int prevMx,prevMy;

	YsArray <YsVec2> prevTouch;

	YSBOOL orientationLock,translationLock;

	class FsGui3DViewControlDialogBase *viewControlDialog;

	class FsGuiCanvas *canvas;

public:
	/*! Default constructor. */
	FsGui3DViewControl();

	/*! Initializes the object. */
	void Initialize(void);

	/*! Attaches a canvas to the view controller */
	void AttachCanvas(class FsGuiCanvas *canvas);

	/*! Attaches a view-control dialog. */
	void AttachViewControlDialog(class FsGui3DViewControlDialogBase *viewControlDialog);

	/*! Changes the view-control mode. */
	void SetViewControlMode(VIEWCONTROLMODE viewControlMode);

	/*! Set viewing attitude for F1 key. */
	void SetF1ViewAttitude(const YsAtt3 &att);

	/*! Set viewing attitude for F2 key. */
	void SetF2ViewAttitude(const YsAtt3 &att);

	/*! Set viewing attitude for F3 key. */
	void SetF3ViewAttitude(const YsAtt3 &att);

	/*! Set viewing attitude for F4 key. */
	void SetF4ViewAttitude(const YsAtt3 &att);

	/*! Set viewing attitude for F5 key. */
	void SetF5ViewAttitude(const YsAtt3 &att);

	/*! Set viewing attitude for F6 key. */
	void SetF6ViewAttitude(const YsAtt3 &att);

	/*! Returns the current view-control mode. */
	VIEWCONTROLMODE GetViewControlMode(void) const;

	/*! Give update of mouse state to the object. */
	YSRESULT SetMouseState(YSBOOL lb,YSBOOL mb,YSBOOL rb,int mx,int my);
private:
	YSRESULT SetMouseStateYsStandard(YSBOOL lb,YSBOOL mb,YSBOOL rb,int mx,int my);
	YSRESULT SetMouseStateCatiaCompatible(YSBOOL lb,YSBOOL mb,YSBOOL rb,int mx,int my);

	void Drag(int mx,int my,int prevMx,int prevMy);
	void RotateByDragging(int mx,int my,int prevMx,int prevMy);
	void ZoomByDragging(int mx,int my,int prevMx,int prevMy);
	void TranslateByDragging(int mx,int my,int prevMx,int prevMy);
	void Scroll(double dx,double dy);

public:
	/*! Give update of touch state. */
	YSRESULT SetTouchState(YSSIZE_T nTouch,const FsVec2i touch[]);
private:
	// Re-order incoming touches based on the proximity of the previous touch
	YsArray <YsVec2> MatchTouch(YSSIZE_T nTouch,const FsVec2i touch[]);

public:
	/*! This function gives key stroke to the object. */
	YSRESULT KeyIn(int fsKey,YSBOOL shift,YSBOOL ctrl,YSBOOL alt);

	/*! This function sets orientation lock.  If the orientation is locked, the user cannot rotate the view until the lock is cleared. */
	void SetOrientationLock(YSBOOL lock);

	/*! This function sets translationlock.  If the translation is locked, the user cannot translate (scroll) the view until the lock is cleared. */
	void SetTranslationLock(YSBOOL lock);

	/*! This function sets view target.  If the view control dialog is attached, it also updates the view-target coordinate on the dialog. */
	void SetViewTarget(const YsVec3 &pos);

	/*! Set needRedraw flag.  The program must set needRedraw flag to YSFALSE when the 
	    window is drawn. */
	void SetNeedRedraw(YSBOOL flag);

	/*! Returns needRedraw flag. */
	YSBOOL NeedRedraw(void) const;
};

/* } */
#endif
