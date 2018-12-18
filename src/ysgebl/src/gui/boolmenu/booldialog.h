/* ////////////////////////////////////////////////////////////

File Name: booldialog.h
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

#ifndef BOOLDIALOG_IS_INCLUDED
#define BOOLDIALOG_IS_INCLUDED
/* { */

#include <fsgui.h>

class PolyCreBoolDialog : public FsGuiDialog
{
private:
	class GeblGuiEditorBase *canvas;

	FsGuiButton *cancelBtn;
public:
	FsGuiButton *diagnoseBtn;
	FsGuiButton *recalcNormalBtn;
	FsGuiButton *triangulateNonPlanarBtn;

private:
	PolyCreBoolDialog();
	~PolyCreBoolDialog();
public:
	static PolyCreBoolDialog *Create(class GeblGuiEditorBase *canvas);
	static void Delete(PolyCreBoolDialog *ptr);

	YSRESULT Make(void);
	void RemakeDrawingBuffer(void);

	virtual void OnButtonClick(FsGuiButton *btn);
	virtual void OnDropListSelChange(FsGuiDropList *drp,int);
	virtual void OnTextBoxChange(FsGuiTextBox *txt);
	virtual void OnSliderPositionChange(FsGuiSlider *slider,const double &prevPos,const double &prevValue);

	YsShellExtEdit *slHdB; // Piggy back for retaining a shell while displaying other dialogs.
};

/* } */
#endif
