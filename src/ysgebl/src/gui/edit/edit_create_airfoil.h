/* ////////////////////////////////////////////////////////////

File Name: edit_create_airfoil.h
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

#ifndef EDIT_CREATE_AIRFOIL_IS_INCLUDED
#define EDIT_CREATE_AIRFOIL_IS_INCLUDED
/* { */

#include <fsgui.h>
#include <ysglbuffer.h>

class YsCreateAirfoilDialog : public FsGuiDialog
{
private:
	enum
	{
		NACA,
		FLAT,
		LENS,
		DIAMOND
	};

	GeblGuiEditorBase *canvas;

	FsGuiButton *closeBtn,*createBtn,*helpBtn;
	FsGuiTabControl *airfoilTypeTab;

	FsGuiButton *thirdPointIsNormal,*thirdPointIsTangent;

	// NACA TAB >>
	int nacaTabIdent;
	FsGuiTextBox *nacaNumberTxt;
	FsGuiTextBox *nacaNumDivisionTxt;
	FsGuiButton *nacaSharpTailBtn;
	// NACA TAB <<

	// NACA6 TAB >>
	int naca6TabIdent;
	FsGuiDropList *naca6LabelDrp;
	FsGuiDropList *naca6MeanLineDrp;
	FsGuiButton *naca6ModifiedMeanLineBtn;
	FsGuiTextBox *naca6NumDivisionTxt;
	// NACA6 TAB <<

	// Supercritical TAB >>
	int supercriticalTabIdent;
	FsGuiTextBox *supercriticalNumDivisionTxt;
	FsGuiDropList *supercriticalLabelDrp;
	FsGuiButton *superCriticalMatchTailBtn;
	// Supercritical TAB <<

	// Joukowski TAB >>
	int JoukowskiTabIdent;
	FsGuiTextBox *JoukowskiATxt;
	FsGuiTextBox *JoukowskiDTxt;
	FsGuiTextBox *JoukowskiETxt;
	FsGuiTextBox *JoukowskiNumDivisionTxt;
	// Joukowski TAB <<

	// Clark-Y >>
	int ClarkYTabIdent;
	FsGuiTextBox *ClarkYNumDivisionTxt;
	// Clark-Y <<

	// Lens >>
	int lensTabIdent;
	FsGuiTextBox *lensNumDivisionTxt;
	FsGuiTextBox *lensH[2],*lensB[2];
	// Lens <<

public:
	YsVec3 front,back,thirdPoint;
	unsigned int frontVtKey,backVtKey;
	YsGLVertexBuffer airfoilVtxBuf;

private:
	YsCreateAirfoilDialog();
	~YsCreateAirfoilDialog();

public:
	void RecalculateAirfoil(void);
	void GetAirfoil(YsArray <YsVec3> &point) const;
	void GetAirfoil(YsArray <YsVec3> &point,const class Airfoil &airfoil) const;
	void GetUV(YsVec3 &u,YsVec3 &v) const;

	static YsCreateAirfoilDialog *Create(GeblGuiEditorBase *canvas);
	static void Delete(YsCreateAirfoilDialog *toDel);

	void Make(void);

	virtual void OnButtonClick(FsGuiButton *btn);
	virtual void OnTextBoxChange(FsGuiTextBox *txt);
	virtual void OnDropListSelChange(FsGuiDropList *drp,int prevSel);
	virtual void OnSelectTab(FsGuiTabControl *tab,int newTab);
};

/* } */
#endif
