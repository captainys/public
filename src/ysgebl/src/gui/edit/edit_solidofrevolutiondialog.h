/* ////////////////////////////////////////////////////////////

File Name: edit_solidofrevolutiondialog.h
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

#ifndef EDIT_SOLIDOFREVOLUTIONDIALOG_IS_INCLUDED
#define EDIT_SOLIDOFREVOLUTIONDIALOG_IS_INCLUDED
/* { */

#include <fsgui.h>

class PolyCreSolidOfRevolutionDialog : public FsGuiDialog
{
private:
	class GeblGuiEditorBase *canvas;
	FsGuiButton *divCountBtn,*stepAngleBtn;
	FsGuiTextBox *divCountTxt,*stepAngleTxt;
	FsGuiButton *deleteSourcePolygonBtn;
	FsGuiButton *closedBtn;
	FsGuiTextBox *fanAngleTxt;
	FsGuiTextBox *displacementTxt;
	FsGuiButton *smoothnessBtn[3];
	FsGuiButton *reverseBtn;
	FsGuiButton *okBtn,*cancelBtn;

public:
	YsArray <YsShellPolygonHandle> srcPlHdArray;
	YsArray <YsShellExt::ConstEdgeHandle> srcCeHdArray;
	YsShellVertexStore allVtHd;
	YsArray <YsArray <YsShellVertexHandle> > srcBoundaryArray;
	YsShellVertexHandle axisVtHd[2];
	const YsShellExt *shl;

	YsShellVertexHandle axisSelVtHd[2];   // For restoring selection after making a solid of revolution.
	YsShellExt::ConstEdgeHandle axisSelCeHd; // For restoring selection after making a solid of revolution.

	YsHashTable <YsArray <YsVec3> > srcVtKeyToPathPosHash;
	YsGLVertexBuffer lineBuf;

private:
	PolyCreSolidOfRevolutionDialog();
	~PolyCreSolidOfRevolutionDialog();
public:
	static PolyCreSolidOfRevolutionDialog *Create(class GeblGuiEditorBase *canvas);
	static void Delete(PolyCreSolidOfRevolutionDialog *ptr);

	YSRESULT Make(void);
	void RemakeSrcVtKeyToPathPosHash(const YsShellExt &shl);
	void RemakeDrawingBuffer(void);

	void CleanUp(const YsShellExt &shl);

	int GetDivisionCount(void) const;
	YSBOOL CloseLoop(void) const;
	YSBOOL DeleteSourcePolygon(void) const;

	virtual void OnButtonClick(FsGuiButton *btn);
	virtual void OnDropListSelChange(FsGuiDropList *drp,int);
	virtual void OnTextBoxChange(FsGuiTextBox *txt);
	virtual void OnSliderPositionChange(FsGuiSlider *slider,const double &prevPos,const double &prevValue);
};

/* } */
#endif
