/* ////////////////////////////////////////////////////////////

File Name: edit_createprimitivedialog.h
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

#ifndef EDIT_CREATEPRIMITIVEDIALOG_IS_INCLUDED
#define EDIT_CREATEPRIMITIVEDIALOG_IS_INCLUDED
/* { */

#include <fsgui.h>
#include <ysglbuffer.h>

// implemented in edit_createmenu.cpp
class PolyCreCreatePrimitiveDialog : public FsGuiDialog
{
private:
	GeblGuiEditorBase *canvas;
	FsGuiDropList *primTypeDrp;
	FsGuiTextBox *numDivTxt;
	FsGuiButton *orientationBtn;

	FsGuiButton *okBtn,*cancelBtn;

	PolyCreCreatePrimitiveDialog(){};
	~PolyCreCreatePrimitiveDialog(){};

public:
	YsVec3 box[2];
	int shapeType,orientation,division;
	YsShellExt tmpShl;
	YsGLVertexBuffer vtxBuf;

public:
	static PolyCreCreatePrimitiveDialog *Create(GeblGuiEditorBase *canvas);
	static void Delete(PolyCreCreatePrimitiveDialog *ptr);

	void Make(void);

	void PrepareShape(void);
private:
	void GetTransformation(YsVec3 &cen,YsVec3 &uVec,YsVec3 &vVec,YsVec3 &wVec) const;
	void PrepareBox(void);
	void PrepareCylinder(void);
	void PrepareEllipsoid(void);
	void PrepareCone(void);

public:
	virtual void OnButtonClick(FsGuiButton *btn);
	virtual void OnTextBoxChange(FsGuiTextBox *txt);
	virtual void OnDropListSelChange(FsGuiDropList *drp,int);
};


class PolyCreParallelogramDialog : public FsGuiDialog
{
private:
	class GeblGuiEditorBase *canvas;
	FsGuiButton *okBtn,*cancelBtn,*changePointSchemeBtn;

	PolyCreParallelogramDialog();
	~PolyCreParallelogramDialog();
public:
	static PolyCreParallelogramDialog *Create(class GeblGuiEditorBase *canvas);
	static void Delete(PolyCreParallelogramDialog *ptr);

	YSRESULT Make(void);
	void RemakeDrawingBuffer(void);

	virtual void OnButtonClick(FsGuiButton *btn);
	virtual void OnDropListSelChange(FsGuiDropList *drp,int);
	virtual void OnTextBoxChange(FsGuiTextBox *txt);
	virtual void OnSliderPositionChange(FsGuiSlider *slider,const double &prevPos,const double &prevValue);

	YsGLVertexBuffer lineBuf; // ,triBuf; as required
	YsArray <YsShellVertexHandle> topRow,bottomRow;
	YsArray <YsShellPolygonHandle> plgChain;
	FsGuiButton *closeLoopBtn,*closeLoopFloorCeilingBtn,*cancelLastBtn;
};


class PolyCreEquilateralDialog : public FsGuiDialog
{
private:
	class GeblGuiEditorBase *canvas;
	FsGuiDropList *shapeTypeDrp;
	FsGuiDropList *takeCursorPosAsDrp;
	FsGuiDropList *whichHalfDrp;
	FsGuiDropList *firstVertexIsDrp;
	FsGuiTextBox *numDivTxt;
	FsGuiButton *include2ndVertexBtn;
	FsGuiButton *includeCursorPosBtn;
	FsGuiButton *okBtn,*cancelBtn;
	FsGuiButton *polygonOrConstEdgeBtn[2];

public:
	YsGLVertexBuffer circleBuf,lineBuf;

	class PositionKeyPair
	{
	public:
		YSHASHKEY vtKey;
		YsVec3 vtPos;
	};
	int nSrcVt;
	PositionKeyPair srcVtCache[3];

private:
	PolyCreEquilateralDialog();
	~PolyCreEquilateralDialog();
public:
	static PolyCreEquilateralDialog *Create(class GeblGuiEditorBase *canvas);
	static void Delete(PolyCreEquilateralDialog *ptr);

	YSRESULT Make(void);
	void RemakeDrawingBuffer(void);

	YSRESULT GetCircumCircle(YsVec3 &cen,double &rad,YsVec3 &uVec,YsVec3 &vVec) const;
	YSRESULT GetCircumEllipse(YsVec3 &cen,double &uRad,double &vRad,YsVec3 &uVec,YsVec3 &vVec) const;
	YsArray <YsPair <YsVec3,YsShellVertexHandle> > GetPolygon(void) const;
	YSBOOL IsLoop(void) const;
	YSBOOL ShouldMakePolygon(void) const;
	YSBOOL ShouldMakeConstEdge(void) const;

	virtual void OnButtonClick(FsGuiButton *btn);
	virtual void OnDropListSelChange(FsGuiDropList *drp,int);
	virtual void OnTextBoxChange(FsGuiTextBox *txt);
};


/* } */
#endif
