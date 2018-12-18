/* ////////////////////////////////////////////////////////////

File Name: fsgui3d.h
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

#ifndef FSGUI3D_IS_INCLUDED
#define FSGUI3D_IS_INCLUDED
/* { */

#include <functional>

#include <fssimplewindow.h>
#include <fsgui.h>


class FsGui3DInputCoordDialog1 : public FsGuiDialog
{
protected:
	FsGuiTextBox *xCoord,*yCoord,*zCoord;
	FsGuiButton *absoluteBtn,*relativeBtn;
	FsGuiButton *setBtn;
	FsGuiButton *xyPln,*xzPln,*yzPln;

	FsGuiButton *verticalHandleBtn;
	FsGuiButton *copyBtn,*pasteBtn;

public:
	class FsGui3DInterface *parentInterface;


	FsGui3DInputCoordDialog1(class FsGui3DInterface *parent);
	void MakeDialog(void);
	void SetCoord(const double &x,const double &y,const double &z);
	int GetBasePlane(void) const;
	void SetBasePlane(int pln);
	void DisableBasePlaneButton(void);
	void EnableBasePlaneButton(void);
	YsVec3 GetNormalDirection(void) const;
	YSBOOL ShowRelativeCoord(void) const;

	void SetVerticalHandle(YSBOOL flg);
	YSBOOL ShowVerticalHandle(void) const;

	virtual void OnButtonClick(FsGuiButton *btn);
};

class FsGui3DInputCoordDialog2 : public FsGuiDialog
{
protected:
	FsGuiTextBox *xCoord,*yCoord,*zCoord;
	FsGuiButton *absoluteBtn,*relativeBtn;
	FsGuiButton *setBtn;
	FsGuiButton *customOrientationBtn;

	FsGuiButton *copyBtn,*pasteBtn;

public:
	class FsGui3DInterface *parentInterface;


	FsGui3DInputCoordDialog2(class FsGui3DInterface *parent);
	void MakeDialog(void);
	void SetCoord(const double &x,const double &y,const double &z);
	YSBOOL UseCustomOrientation(void);
	YSBOOL ShowRelativeCoord(void) const;

	void SetUseCustomOrientation(YSBOOL sw);
	void EnableCustomOrientationButton(YSBOOL sw);

	virtual void OnButtonClick(FsGuiButton *btn);
};

class FsGui3DInputBoxDialog : public FsGuiDialog
{
protected:
	FsGuiTextBox *xCoord1,*yCoord1,*zCoord1;
	FsGuiTextBox *xCoord2,*yCoord2,*zCoord2;
	FsGuiButton *centerBtn,*cornerBtn1;
	FsGuiButton *sizeBtn,*cornerBtn2;
	FsGuiButton *setBtn,*maintainAspectBtn;

public:
	class FsGui3DInterface *parentInterface;

	FsGui3DInputBoxDialog(class FsGui3DInterface *parent);
	void MakeDialog(void);
	void SetCoord(const YsVec3 bbx[2]);
	void SetMaintainAspectRatio(YSBOOL maintainAspect);
	void EnableMaintainAspectRatioButton(void);
	void DisableMaintainAspectRatioButton(void);

	virtual void OnButtonClick(FsGuiButton *btn);
};

class FsGui3DRotationDialog : public FsGuiDialog
{
protected:
	FsGuiTextBox *axisX,*axisY,*axisZ;
	FsGuiButton *setXAxisBtn,*setYAxisBtn,*setZAxisBtn,*setAxisBtn;

	FsGuiTextBox *centerX,*centerY,*centerZ;
	FsGuiButton *setCenterBtn;

	FsGuiTextBox *angle;
	FsGuiButton *degreeBtn,*radianBtn,*setAngleBtn;
	FsGuiButton *mirrorBtn;

	FsGuiButton *deg90Btn,*deg180Btn,*deg270Btn,*deg0Btn,*plus1DegBtn,*minus1DegBtn,*plus5DegBtn,*minus5DegBtn;

public:
	class FsGui3DInterface *parentInterface;

	FsGui3DRotationDialog(class FsGui3DInterface *parent);
	void MakeDialog(void);
	void SetAngle(const double &newAngle);
	void SetRotation(const YsVec3 &axis,const double &angle,const YsVec3 &center);
	void AllowChangeAxis(YSBOOL allowMoveChangeAxis);
	void AllowMoveCenter(YSBOOL allowMoveCenter);
	void AllowMirror(YSBOOL allowMirror);

	virtual void OnButtonClick(FsGuiButton *btn);
};




class FsGui3DInterface
{
private:
	mutable YSBOOL needRedraw;
	int viewport[4];
	YsMatrix4x4 projViewModelTfm;

	YSBOOL prevLb,prevMb,prevRb;
	int prevMx,prevMy;

	double dragGlMx0,dragGlMy0;
	double linePickingTolerance;    // In Pixel
	double pickerLength;            // In Pixel
	double markerSize;              // In Pixel

	enum DRAGTARGET
	{
		NONE,
		POINT_X,
		POINT_Y,
		POINT_Z,
		BOX_XMIN,
		BOX_XMAX,
		BOX_YMIN,
		BOX_YMAX,
		BOX_ZMIN,
		BOX_ZMAX,
		ROTATION_HANDLE,
		ROTATION_B
	};

	std::function <void()> stdChangeCallBack;
	void (*ChangeCallBack)(void *contextPtr,FsGui3DInterface &itfc);
	void *contextPtr;

public:
	enum INTERFACETYPE
	{
		IFTYPE_NONE,
		IFTYPE_BOX,
		IFTYPE_POINT,
		IFTYPE_DRAGPOINT,
		IFTYPE_SLIDER,
		IFTYPE_ROTATION
	};

	class FsGuiCanvas *canvas;

	INTERFACETYPE type;
	YSBOOL dragging;

	int ofstX,ofstY;
	double infiniteLongLine;

	// Dialogs
	FsGui3DInputCoordDialog1 *point_dlg1;
	FsGui3DInputCoordDialog2 *point_dlg2;
	FsGui3DInputBoxDialog *box_dlg;
	FsGui3DRotationDialog *rot_dlg;

	// For point
	YsVec3 point_pos,point_pos_org;
	YSBOOL point_dragDirSwitch[3];
	YsVec3 point_dragDir[3];
	DRAGTARGET point_dragging;

	// For box
	YsVec3 box_pos[2],box_pos_org[2];
	DRAGTARGET box_dragging;
	YSBOOL box_maintainAspectRatio,box_allowResize;

	// For rotation
	YsVec3 rot_axis,rot_center,rot_iVec,rot_jVec,rot_kVec;
	double rot_angle,rot_mirror;
	double rot_ref_size;
	YsVec3 rot_drag_dir;
	double rot_picked_angle,rot_drag_angle;
	DRAGTARGET rot_dragging;


	FsGui3DInterface(class FsGuiCanvas *canvas);
	~FsGui3DInterface();
	void Initialize(void);
	void Draw(void) const;

	INTERFACETYPE GetInterfaceType(void) const;

	void SetViewport(const int viewport[4]);
	void SetProjViewModelMatrix(const YsMatrix4x4 &mat);

	void SetNeedRedraw(YSBOOL needRedraw);
	YSBOOL NeedRedraw(void) const;

	void SetCallBack(void (*CallBackFunc)(void *,FsGui3DInterface &itfc),void *contextPtr);

	void ClearCallBack(void);
	void InvokeCallBack(void);

	template <class objType>
	void BindCallBack(void (objType::*func)(FsGui3DInterface *),objType *obj)
	{
		ClearCallBack();
		stdChangeCallBack=std::bind(func,obj,this);
	}


	void BeginInputBox(const YsVec3 &box1,const YsVec3 &box2,YSBOOL allowResize,YSBOOL maintainAspectRatio);
	void EnableMaintainAspectRatioButton(void);
	void DisableMaintainAspectRatioButton(void);
	void GetBox(YsVec3 bbx[2]) const;
	void GetOriginalBox(YsVec3 bbx[2]) const;
	void SetBox(const YsVec3 bbx[2]);

	void BeginInputPoint1(const YsVec3 &pnt);
	void SetPoint1BasePlane(int pln); // 0:XY,  1:XZ,  2:YZ plane.
	void EnablePoint1BasePlaneButton(void);
	void DisablePoint1BasePlaneButton(void);
    void BeginInputPoint2(const YsVec3 &pnt,const YSBOOL dirSwitch[3]=NULL,const YsVec3 dir[3]=NULL);
	YSRESULT ChangeInputPoint1to2(YSBOOL dirSwitch[3],const YsVec3 dir[3]);
	YSRESULT ChangeInputPoint2to1(void);

    void SetInputPoint2UseCustomOrientation(YSBOOL sw);
    void EnableInputPoint2CustomOrientationButton(YSBOOL sw);
	const YsVec3 &GetPoint(void) const;
	const YsVec3 &GetOriginalPoint(void) const;

    void GetRotationMatrix(YsMatrix4x4 &mat) const;
    const double GetRotationAngle(void) const;
    const YsVec3 &GetRotationAxis(void) const;
    const YsVec3 &GetRotationCenter(void) const;
	void BeginInputRotation
	    (const YsVec3 &axis,const double &angle,const YsVec3 &center,const double &refSize,
	     YSBOOL allowChangeAxis,
	     YSBOOL allowMoveCenter,
	     YSBOOL allowMirror);
	void SetRotationAxis(const YsVec3 &axs);

	void End3DInterface(void);

	YSRESULT SetMouseState(YSBOOL lb,YSBOOL mb,YSBOOL rb,int mx,int my);
	YSRESULT SetTouchState(YSSIZE_T nTouch,const class FsVec2i touch[]);

	/*! Give key stroke to the object. */
	YSRESULT KeyIn(int fsKey,YSBOOL shift,YSBOOL ctrl,YSBOOL alt);

private:
	template <const int N>
	void MakeRotationHandle(YsArray <YsVec3,N> &handlePnt) const;

	YSRESULT OnLButtonDown(YSBOOL lb,YSBOOL mb,YSBOOL rb,int mx,int my);
	YSRESULT OnRButtonDown(YSBOOL lb,YSBOOL mb,YSBOOL rb,int mx,int my);
	YSRESULT OnLButtonUp(YSBOOL lb,YSBOOL mb,YSBOOL rb,int mx,int my);
	YSRESULT OnRButtonUp(YSBOOL lb,YSBOOL mb,YSBOOL rb,int mx,int my);
	YSRESULT OnMouseMove(YSBOOL lb,YSBOOL mb,YSBOOL rb,int mx,int my);

	YSRESULT DragBox(YSBOOL lb,YSBOOL rb,YSBOOL sft,YSBOOL ctrl,int mx,int my);
	YSRESULT DragPoint(YSBOOL lb,YSBOOL rb,YSBOOL sft,YSBOOL ctrl,int mx,int my);
	YSRESULT DragPoint2(YSBOOL lb,YSBOOL rb,YSBOOL sft,YSBOOL ctrl,int mx,int my);
	YSRESULT DragRotation(YSBOOL lb,YSBOOL rb,YSBOOL sft,YSBOOL ctrl,int mx,int my);

	void DrawLineByPixelLength(const YsVec3 &o,const YsVec3 &v,const double scrnLng) const;
	/*! Returns error if o is behind the viewer. */
	YSRESULT CalculateLineEndPointForSpecificPixelLength(
	    YsVec3 &p1,YsVec3 &p2,YsVec3 &scrnP1,YsVec3 &scrnP2,
	    const YsVec3 &o,const YsVec3 &v,const double lng) const;

	void DrawRectMarker(const YsVec3 &o,const double markerSize) const;

	YSBOOL Is3DLinePicked(const YsVec3 &o,const YsVec3 &v,const double scrnLng,int glMx,int glMy) const;
	YSBOOL Is3DLinePickedAlsoGetNearestPoint(YsVec3 &nearPos,const YsVec3 &p1,const YsVec3 &p2,int glMx,int glMy) const;
};


#include "fsgui3dutil.h"
#include "fsgui3dviewcontrol.h"
#include "fsgui3dviewcontroldialog.h"

/* } */
#endif
