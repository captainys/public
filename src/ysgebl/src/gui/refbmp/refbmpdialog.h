/* ////////////////////////////////////////////////////////////

File Name: refbmpdialog.h
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

#ifndef REFBMPDIALOG_IS_INCLUDED
#define REFBMPDIALOG_IS_INCLUDED
/* { */

#include <fsgui.h>

class PolyCreRefBmpDialog : public FsGuiDialog
{
private:
	class GeblGuiEditorBase *canvas;
	class PolyCreReferenceBitmapStore *refBmpStore;

	FsGuiDropList *bmpIdxDrp;
	FsGuiButton *centerBtn,*xyBtn,*xzBtn,*yzBtn;

	FsGuiButton *moveBtn,*rotateBtn;

	FsGuiButton *loadBtn,*discardBtn;

	FsGuiButton *showBtn;

	FsGuiButton *closeBtn;

	FsGuiMessageBoxDialog *mbox;

	FsGuiSlider *transparencySlider;


	class FsGuiFileDialog *fdlg;

	YsVec3 orgCorner[4];

private:
	PolyCreRefBmpDialog();
	~PolyCreRefBmpDialog();

public:
	static PolyCreRefBmpDialog *Create(void);
	static void Delete(PolyCreRefBmpDialog *ptr);

	void Make(class GeblGuiEditorBase *canvas,class PolyCreReferenceBitmapStore *refBmpStore);

	int GetSelectedBitmapIndex(void) const;

	virtual void OnButtonClick(FsGuiButton *btn);
    virtual void OnModalDialogClosed(int dialogIdent,class FsGuiDialog *closedModalDialog,int modalDialogEndCode);
	virtual void OnDropListSelChange(FsGuiDropList *drp,int prevSel);
	virtual void OnSliderPositionChange(FsGuiSlider *slider,const double &prevPos,const double &prevValue);

	static void ThreeDInterfaceCallBack(void *contextPtr,FsGui3DInterface &itfc);
private:
	void ConfigureForBitmapIndex(int bmpIdx);
};

/* } */
#endif
