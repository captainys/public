/* ////////////////////////////////////////////////////////////

File Name: configmenu.h
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

#ifndef CONFIGMENU_IS_INCLUDED
#define CONFIGMENU_IS_INCLUDED
/* { */

#include <ysclass.h>
#include <fsgui.h>

class PolyCreConfigDialog : public FsGuiDialog
{
private:
	FsGuiDropList *coordSysDrp;
	FsGuiStatic *defDataPath;
	FsGuiButton *setDefDataPathBtn;

	FsGuiColorPalette *bgColorPlt[3];

	FsGuiButton *saveWindowPosAndSizeBtn;

	FsGuiButton *saveBinaryStlBtn;

	FsGuiButton *autoFixOrientationInDnmBtn;

	FsGuiButton *autoAdjustToleranceButton;

	FsGuiButton *okBtn,*cancelBtn,*setDefaultBtn;

	FsGuiMessageBoxDialog *mbox;
	class FsGuiFileDialog *fdlg;

	class GeblGuiEditorBase *canvas;

	const PolyCreConfigDialog &operator=(const PolyCreConfigDialog &);

public:
	PolyCreConfigDialog();
	~PolyCreConfigDialog();

	static void BeginSelectDefaultDataDirectory(FsGuiControlBase *,FsGuiDialog *,int returnValue);
	static void SelectDefaultDataDirectory_FileSelected(FsGuiControlBase *,FsGuiDialog *,int returnValue);

	void Make(class PolyCreConfig &cfg,class GeblGuiEditorBase *canvas);
	void SetFromConfig(class PolyCreConfig &cfg);
	void Retrieve(class PolyCreConfig &cfg);

	virtual void OnButtonClick(FsGuiButton *btn);
};

/* } */
#endif
