/* ////////////////////////////////////////////////////////////

File Name: miscdlg.h
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

#ifndef MISCDLG_IS_INCLUDED
#define MISCDLG_IS_INCLUDED
/* { */

#include <fsgui.h>

class YsColorPaletteDialog : public FsGuiDialog
{
private:
	YsColorPaletteDialog();
	~YsColorPaletteDialog();

public:
	FsGuiColorPalette *plt;
	FsGuiButton *savePlt,*loadPlt;

	class GeblGuiEditorBase *canvas;
	class FsGuiFileDialog *fdlg;
	FsGuiMessageBoxDialog yesno;

	static YsColorPaletteDialog *Create(void);
	static void Delete(YsColorPaletteDialog *dlg);

	void MakeDialog(class GeblGuiEditorBase *ownerIn);
	void InitDialog(void);

	void SetColor(const YsColor &col);
	YsColor GetColor(void) const;

	virtual void OnButtonClick(FsGuiButton *btn);

	YsWString savePaletteFileName;
	void SavePalette(const wchar_t fn[]) const;
	void LoadPalette(const wchar_t fn[]);
    virtual void OnModalDialogClosed(int dialogIdent,class FsGuiDialog *closedModalDialog,int modalDialogEndCode);
};

/* } */
#endif
