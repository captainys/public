/* ////////////////////////////////////////////////////////////

File Name: fsgui3dviewcontroldialog.h
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

#ifndef FSGUI_DVIEWCONTROLDIALOG_IS_INCLUDED
#define FSGUI_DVIEWCONTROLDIALOG_IS_INCLUDED
/* { */

#include "fsgui3dviewcontrol.h"
#include <fsgui.h>


class FsGui3DViewControlDialogBase : public FsGuiDialog
{
protected:
	FsGuiCanvas *canvas;
	class FsGui3DViewControl *viewControl;
	friend class FsGui3DViewControl;

	class PosAttZoomDist
	{
	public:
		YsVec3 pos;
		YsAtt3 att;
		double zoom,dist;
	};
	PosAttZoomDist lastView;



protected:
	virtual ~FsGui3DViewControlDialogBase();

public:
	/*! Deletes a dialog object. */
	static void Delete(FsGui3DViewControlDialogBase *toDel);

	/*! This function returns a reference to the internally-stored viewing-control information. */
	FsGui3DViewControl &GetViewControl(void);

	/*! This function returns a const reference to the internally-stored viewing-control information. */
	const FsGui3DViewControl &GetViewControl(void) const;

public:
	/*! This function saves the current view point as last view so that the user can recall the view point later. */
	void SaveLastView(void);

protected:
	PosAttZoomDist RecallLastView(void);

public:
	virtual YSBOOL SingleButtonRotation(void) const=0;
	virtual YSBOOL SingleButtonScroll(void) const=0;
	virtual YSBOOL SingleButtonZoom(void) const=0;
	virtual void SetOrientationLock(YSBOOL lock)=0;
	virtual void SetTranslationLock(YSBOOL lock)=0;
	virtual void UpdateViewTarget(const YsVec3 &pos)=0;
};



class FsGui3DViewControlDialog : public FsGui3DViewControlDialogBase
{
private:
	FsGuiButton *openBtn,*closeBtn;

	FsGuiButton *rollLeftBtn,*noseDownBtn,*rollRightBtn;
	FsGuiButton *yawLeftBtn,*zeroAttBtn,*yawRightBtn;
	FsGuiButton *left90Btn,*noseUpBtn,*right90Btn;

	FsGuiButton *scrollUpBtn;
	FsGuiButton *scrollLeftBtn,*centerBtn,*scrollRightBtn;
	FsGuiButton *scrollDownBtn;

	FsGuiButton *lockOrientationBtn;
	FsGuiButton *lockTranslationBtn;

	FsGuiButton *frontViewBtn,*rearViewBtn;
	FsGuiButton *topViewBtn,*bottomViewBtn;
	FsGuiButton *leftViewBtn,*rightViewBtn;

	FsGuiButton *zoomBtn,*moozBtn;
	FsGuiButton *orthoPersBtn;

	FsGuiTextBox *lookAtCoordTxt;
	FsGuiButton *lookAtBtn,*lastViewBtn;
	FsGuiButton *lastLookAtBtn;
	FsGuiButton *lookAtCutBufBtn;

	FsGuiButton *copyBtn,*pasteBtn;

	FsGuiButton *noSingleBtnOpBtn,*rotateBtn,*translateBtn;

	FsGuiButton *saveViewBtn,*loadViewBtn;

	YSBOOL orientationLock,translationLock;


private:
	FsGui3DViewControlDialog();
	virtual ~FsGui3DViewControlDialog();

public:
	/*! Creates a dialog object. */
	static FsGui3DViewControlDialog *Create(void);

	/*! This function makes the dialog from a view controller. */
	void Make(class FsGuiCanvas *canvas,class FsGui3DViewControl *viewControl,YSBOOL minimize=YSFALSE);

	/*! Call back function for button click. */
	virtual void OnButtonClick(FsGuiButton *btn);

	/*! This function sets lock-orientation status. */
	virtual void SetOrientationLock(YSBOOL lock);

	/*! This function sets lock-translation status. */
	virtual void SetTranslationLock(YSBOOL lock);

	/*! This function returns the status of "Rotate" button for single-button operation. */
	virtual YSBOOL SingleButtonRotation(void) const;

	/*! This function returns the status of "Scroll" button for single-button operation. */
	virtual YSBOOL SingleButtonScroll(void) const;

	/*! This function returns the status of "SingleButtonZoom" button.  Not included in the standard-view control dialog. */
	virtual YSBOOL SingleButtonZoom(void) const;

private:
	void Translate(double vx,double vy);
	virtual void UpdateViewTarget(const YsVec3 &pos);
};

/* } */
#endif
