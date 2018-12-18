/* ////////////////////////////////////////////////////////////

File Name: editmenu.cpp
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



#include <ysgl.h>
#include <ysglslcpp.h>
#include <ysglbuffer.h>

#include <ysshellextutil.h>
#include <ysshellext_trackingutil.h>
#include <ysshellextedit_topologyutil.h>
#include <ysshellextedit_localop.h>
#include <ysshellext_offsetutil.h>
#include <ysshellextedit_sewingutil.h>
#include <ysshellext_imprintingutil.h>

#include "../ysgebl_gui_editor_base.h"
#include "../miscdlg.h"

#include "edit_create_airfoil.h"
#include "edit_sweepdialog.h"
#include "edit_mirrorimagedialog.h"
#include "edit_hemmingdialog.h"
#include "edit_rounddialog.h"
#include "edit_createprimitivedialog.h"
#include "edit_projdialog.h"
#include "edit_intersectiondialog.h"
#include "edit_insertdialog.h"
#include "edit_solidofrevolutiondialog.h"
#include "edit_offsetdialog.h"

#include "../util/utildlg.h"

#include "../refbmp/refbmpdialog.h"

#include "../dnm/dnmtreedialog.h"

#include "../textresource.h"

#include "../viewmenu/viewdialog.h"

#include "../ysgebl_gui_extension_base.h"



void GeblGuiEditorBase::Edit_Paste(FsGuiPopUpMenuItem *)
{
	if(nullptr!=Slhd())
	{
		YsShellExtEdit::StopIncUndo incUndo(Slhd());

		cutBuffer.Paste(*Slhd());

		auto selVtHd=cutBuffer.GetNonDerivedNewVertex();
		auto selPlHd=cutBuffer.GetNonDerivedNewPolygon();
		auto selCeHd=cutBuffer.GetNonDerivedNewConstEdge();
		auto selFgHd=cutBuffer.GetNonDerivedNewFaceGroup();
		auto selVlHd=cutBuffer.GetNonDerivedNewVolume();

		Slhd()->SelectVertex(selVtHd);
		Slhd()->SelectPolygon(selPlHd);
		Slhd()->SelectConstEdge(selCeHd);
		Slhd()->SelectFaceGroup(selFgHd);
		// Slhd()->SelectVolume(selVlHd);

		needRemakeDrawingBuffer=(unsigned int)NEED_REMAKE_DRAWING_ALL;
		SetNeedRedraw(YSTRUE);
	}
}

class GeblGuiPasteSpecialDialog : public FsGuiDialog
{
public:
	FsGuiDropList *mirrorImageDrp;
	FsGuiButton *okBtn,*cancelBtn;
	GeblGuiEditorBase *canvasPtr;

	void Make(GeblGuiEditorBase *canvas);
	virtual void OnButtonClick(FsGuiButton *btn);
};

void GeblGuiPasteSpecialDialog::Make(GeblGuiEditorBase *canvas)
{
	canvasPtr=canvas;

	const char * const mirrorStr[]=
	{
		"No Mirror Image",
		"YZ-plane",
		"XZ-plane",
		"XY-plane",
	};
	AddStaticText(0,FSKEY_NULL,L"Mirror Image",YSTRUE);
	mirrorImageDrp=AddDropList(0,FSKEY_NULL,"Mirror Image",4,mirrorStr,10,32,32,YSFALSE);
	mirrorImageDrp->Select(0);

	okBtn=AddTextButton(0,FSKEY_ENTER,FSGUI_PUSHBUTTON,FSGUI_COMMON_OK,YSTRUE);
	cancelBtn=AddTextButton(0,FSKEY_ESC,FSGUI_PUSHBUTTON,FSGUI_COMMON_CANCEL,YSFALSE);

	Fit();
}
/* virtual */ void GeblGuiPasteSpecialDialog::OnButtonClick(FsGuiButton *btn)
{
	if(okBtn==btn)
	{
		auto &canvas=*canvasPtr;
		if(NULL!=canvas.Slhd())
		{
			YsShellExtEdit::StopIncUndo incUndo(canvas.Slhd());

			YSBOOL invertPolygon=YSFALSE;
			YSBOOL useMatrix=YSFALSE;
			YsMatrix4x4 mirrorTfm;

			switch(mirrorImageDrp->GetSelection())
			{
			case 0:
				break;
			case 1:
				mirrorTfm.Scale(-1,1,1);
				invertPolygon=YSTRUE;
				useMatrix=YSTRUE;
				break;
			case 2:
				mirrorTfm.Scale(1,-1,1);
				invertPolygon=YSTRUE;
				useMatrix=YSTRUE;
				break;
			case 3:
				mirrorTfm.Scale(1,1,-1);
				invertPolygon=YSTRUE;
				useMatrix=YSTRUE;
				break;
			}

			YsMatrix4x4 tfm=mirrorTfm;
			canvas.cutBuffer.PasteWithTransformation(*canvas.Slhd(),invertPolygon,useMatrix,tfm);

			auto selVtHd=canvas.cutBuffer.GetNonDerivedNewVertex();
			auto selPlHd=canvas.cutBuffer.GetNonDerivedNewPolygon();
			auto selCeHd=canvas.cutBuffer.GetNonDerivedNewConstEdge();
			auto selFgHd=canvas.cutBuffer.GetNonDerivedNewFaceGroup();
			auto selVlHd=canvas.cutBuffer.GetNonDerivedNewVolume();

			canvas.Slhd()->SelectVertex(selVtHd);
			canvas.Slhd()->SelectPolygon(selPlHd);
			canvas.Slhd()->SelectConstEdge(selCeHd);
			canvas.Slhd()->SelectFaceGroup(selFgHd);
			// canvas.Slhd()->SelectVolume(selVlHd);

			canvas.needRemakeDrawingBuffer=(unsigned int)GeblGuiEditorBase::NEED_REMAKE_DRAWING_ALL;
			canvas.SetNeedRedraw(YSTRUE);
		}

		canvasPtr->RemoveDialog(this);
	}
	else if(cancelBtn==btn)
	{
		canvasPtr->RemoveDialog(this);
	}
}

void GeblGuiEditorBase::Edit_PasteSpecial(FsGuiPopUpMenuItem *)
{
	auto dlg=FsGuiDialog::CreateSelfDestructiveDialog<GeblGuiPasteSpecialDialog>();
	dlg->Make(this);
	AddDialog(dlg);
	ArrangeDialog();
}

////////////////////////////////////////////////////////////

class GeblGuiEditorBase::Edit_PasteWithTransformationDialog : public FsGuiDialog
{
public:
	GeblGuiEditorBase *canvasPtr;
	FsGuiDropList *pointIsDrp;
	FsGuiTextBox *divCountTxt;
	FsGuiTextBox *axialRotationTxt;
	FsGuiButton *rotationAxisBtn[5];
	FsGuiButton *rotationCenterBtn[4];

	FsGuiButton *okBtn,*cancelBtn;

	YsShellExtDrawingBuffer dBuf;

	void Make(GeblGuiEditorBase *canvasPtr);
	void OnButtonClick(FsGuiButton *btn);
	void OnDropListSelChange(FsGuiDropList *drp,int);
	void OnTextBoxChange(FsGuiTextBox *txt);

	void RemakeDrawingBuffer(void);
	YsMatrix4x4 GetStepTransformation(int iStep) const;
	YsVec3 GetRotationCenter(void) const;
	double GetRotationAngle(void) const;
	YsVec3 GetRotationAxis(void) const;
};

void GeblGuiEditorBase::Edit_PasteWithTransformationDialog::Make(GeblGuiEditorBase *canvasPtr)
{
	this->canvasPtr=canvasPtr;

	FsGuiDialog::Initialize();

	const wchar_t *point_is_option[2]=
	{
		FSGUI_DLG_PASTE_W_TRANS_POINT_IS_FIRST,
		FSGUI_DLG_PASTE_W_TRANS_POINT_IS_LAST
	};
	pointIsDrp=AddDropList(MakeIdent("point_is"),FSKEY_NULL,L"Point Is",2,point_is_option,2,20,20,YSTRUE);
	pointIsDrp->Select(1);

	divCountTxt=AddTextBox(MakeIdent("divcount"),FSKEY_NULL,FsGuiTextBox::HORIZONTAL,FSGUI_DLG_COMMON_DIVCOUNT,L"1",8,YSTRUE);
	divCountTxt->SetTextType(FSGUI_INTEGER);

	axialRotationTxt=AddTextBox(MakeIdent("axialrotation"),FSKEY_NULL,FsGuiTextBox::HORIZONTAL,FSGUI_DLG_COMMON_ROTATE,L"0",8,YSTRUE);
	axialRotationTxt->SetTextType(FSGUI_REALNUMBER);

	rotationCenterBtn[0]=AddTextButton(MakeIdent("center_center"),FSKEY_NULL,FSGUI_RADIOBUTTON,FSGUI_COMMON_CENTER,YSTRUE);
	rotationCenterBtn[1]=AddTextButton(MakeIdent("center_bbxcenter"),FSKEY_NULL,FSGUI_RADIOBUTTON,FSGUI_COMMON_BBXCENTER,YSFALSE);
	rotationCenterBtn[2]=AddTextButton(MakeIdent("center_origin"),FSKEY_NULL,FSGUI_RADIOBUTTON,FSGUI_COMMON_ORIGIN,YSFALSE);
	rotationCenterBtn[3]=AddTextButton(MakeIdent("center_selvtx"),FSKEY_NULL,FSGUI_RADIOBUTTON,FSGUI_DLG_COMMON_SELECTEDVTX,YSFALSE);
	SetRadioButtonGroup(4,rotationCenterBtn);
	rotationCenterBtn[0]->SetCheck(YSTRUE);

	rotationAxisBtn[0]=AddTextButton(MakeIdent("cursordir"),FSKEY_NULL,FSGUI_RADIOBUTTON,FSGUI_DLG_PASTE_W_TRANS_CURSOR_DIRECTION,YSTRUE);
	rotationAxisBtn[1]=AddTextButton(MakeIdent("xaxis"),FSKEY_NULL,FSGUI_RADIOBUTTON,FSGUI_COMMON_XAXIS,YSFALSE);
	rotationAxisBtn[2]=AddTextButton(MakeIdent("yaxis"),FSKEY_NULL,FSGUI_RADIOBUTTON,FSGUI_COMMON_YAXIS,YSFALSE);
	rotationAxisBtn[3]=AddTextButton(MakeIdent("zaxis"),FSKEY_NULL,FSGUI_RADIOBUTTON,FSGUI_COMMON_ZAXIS,YSFALSE);
	rotationAxisBtn[4]=AddTextButton(MakeIdent("twovtx"),FSKEY_NULL,FSGUI_RADIOBUTTON,FSGUI_DLG_COMMON_2SELECTEDVTX,YSFALSE);
	SetRadioButtonGroup(5,rotationAxisBtn);
	rotationAxisBtn[0]->SetCheck(YSTRUE);

	okBtn=AddTextButton(MakeIdent("ok"),FSKEY_ENTER,FSGUI_PUSHBUTTON,FSGUI_COMMON_OK,YSTRUE);
	cancelBtn=AddTextButton(MakeIdent("cancel"),FSKEY_ESC,FSGUI_PUSHBUTTON,FSGUI_COMMON_CANCEL,YSFALSE);

	Fit();
}
void GeblGuiEditorBase::Edit_PasteWithTransformationDialog::OnButtonClick(FsGuiButton *btn)
{
	if(btn==okBtn)
	{
		canvasPtr->Edit_PasteWithTransformation_Paste();
	}
	else if(btn==cancelBtn)
	{
		canvasPtr->Edit_ClearUIMode();
	}
	else
	{
		RemakeDrawingBuffer();
	}
}
void GeblGuiEditorBase::Edit_PasteWithTransformationDialog::OnDropListSelChange(FsGuiDropList *drp,int)
{
	RemakeDrawingBuffer();
}
void GeblGuiEditorBase::Edit_PasteWithTransformationDialog::OnTextBoxChange(FsGuiTextBox *txt)
{
	RemakeDrawingBuffer();
}
void GeblGuiEditorBase::Edit_PasteWithTransformationDialog::RemakeDrawingBuffer(void)
{
	YsShellExt tmpShl;
	auto nStep=divCountTxt->GetInteger();
	for(int i=1; i<=nStep; ++i)
	{
		auto tfm=GetStepTransformation(i);
		canvasPtr->cutBuffer.PasteWithTransformation(tmpShl,YSFALSE,YSTRUE,tfm);
	}
	dBuf.RemakeNormalVertexBuffer(tmpShl,3,3);
	dBuf.RemakePolygonBuffer(tmpShl,0.9);
	dBuf.RemakeConstEdgeBuffer(tmpShl,0,nullptr);
}

YsMatrix4x4 GeblGuiEditorBase::Edit_PasteWithTransformationDialog::GetStepTransformation(int iStep) const
{
	auto nStep=divCountTxt->GetInteger();
	auto toCursor=canvasPtr->threeDInterface.point_pos-canvasPtr->threeDInterface.point_pos_org;
	auto rotAxs=GetRotationAxis();
	auto rotCen=GetRotationCenter();
	auto rotAng=GetRotationAngle();

	if((0==pointIsDrp->GetSelection() && 1==iStep) ||
	   (1==pointIsDrp->GetSelection() && nStep==iStep))
	{
		YsMatrix4x4 tfm;
		tfm.Translate(toCursor);
		if(YsTolerance<fabs(rotAng))
		{
			tfm.Translate(rotCen);
			tfm.Rotate(rotAxs.x(),rotAxs.y(),rotAxs.z(),rotAng);
			tfm.Translate(-rotCen);
		}
		return tfm;
	}
	else if(1==pointIsDrp->GetSelection())
	{
		auto toStep=toCursor*(double)iStep/(double)nStep;
		auto angle=rotAng*(double)iStep/(double)nStep;
		YsMatrix4x4 tfm;
		tfm.Translate(toStep);
		if(YsTolerance<fabs(rotAng))
		{
			tfm.Translate(rotCen);
			tfm.Rotate(rotAxs.x(),rotAxs.y(),rotAxs.z(),angle);
			tfm.Translate(-rotCen);
		}
		return tfm;
	}
	else
	{
		auto toStep=toCursor*(double)iStep;
		auto angle=rotAng*(double)iStep;
		YsMatrix4x4 tfm;
		tfm.Translate(toStep);
		if(YsTolerance<fabs(rotAng))
		{
			tfm.Translate(rotCen);
			tfm.Rotate(rotAxs.x(),rotAxs.y(),rotAxs.z(),angle);
			tfm.Translate(-rotCen);
		}
		return tfm;
	}
}
YsVec3 GeblGuiEditorBase::Edit_PasteWithTransformationDialog::GetRotationCenter(void) const
{
	if(YSTRUE==rotationCenterBtn[0]->GetCheck())
	{
		return canvasPtr->cutBuffer.GetCenter();
	}
	else if(YSTRUE==rotationCenterBtn[1]->GetCheck())
	{
		YsShellExt tmpShl;
		canvasPtr->cutBuffer.Paste(tmpShl);
		return tmpShl.GetBoundingBoxCenter();
	}
	else if(YSTRUE==rotationCenterBtn[3]->GetCheck() && nullptr!=canvasPtr->slHd)
	{
		auto &shl=*(canvasPtr->slHd);
		auto selVtx=shl.GetSelectedVertex();
		if(1<=selVtx.size())
		{
			return shl.GetVertexPosition(selVtx[0]);
		}
	}
	return YsVec3::Origin();
}
double GeblGuiEditorBase::Edit_PasteWithTransformationDialog::GetRotationAngle(void) const
{
	return YsDegToRad(axialRotationTxt->GetRealNumber());
}
YsVec3 GeblGuiEditorBase::Edit_PasteWithTransformationDialog::GetRotationAxis(void) const
{
	if(YSTRUE==rotationAxisBtn[1]->GetCheck())
	{
		return YsXVec();
	}
	else if(YSTRUE==rotationAxisBtn[2]->GetCheck())
	{
		return YsYVec();
	}
	else if(YSTRUE==rotationAxisBtn[3]->GetCheck())
	{
		return YsZVec();
	}
	else if(YSTRUE==rotationAxisBtn[4]->GetCheck() && nullptr!=canvasPtr->slHd)
	{
		auto &shl=*(canvasPtr->slHd);
		auto selVtx=shl.GetSelectedVertex();
		if(2<=selVtx.size())
		{
			return shl.GetEdgeVector(selVtx[0],selVtx[1]);
		}
	}

	auto rotAxis=canvasPtr->threeDInterface.point_pos-canvasPtr->threeDInterface.point_pos_org;
	if(YSOK!=rotAxis.Normalize())
	{
		rotAxis=YsXVec();
	}
	return rotAxis;
}

void GeblGuiEditorBase::Edit_PasteWithTransformation(FsGuiPopUpMenuItem *)
{
	Edit_ClearUIMode();

	YsVec3 pos0=cutBuffer.GetCenter();
	this->threeDInterface.BeginInputPoint1(pos0);

	this->spaceKeyCallBack=std::bind(&THISCLASS::Edit_PasteWithTransformation_Paste,this);
	this->draw3dCallBack=std::bind(&THISCLASS::Edit_PasteWithTransformation_Draw3DCallBack,this);
	this->threeDInterface.BindCallBack(&THISCLASS::Edit_PasteWithTransformation_PointMoved,this);

	auto dlg=FsGuiDialog::CreateSelfDestructiveDialog<Edit_PasteWithTransformationDialog>();
	dlg->Make(this);
	dlg->RemakeDrawingBuffer();
	AddDialog(dlg);
}


void GeblGuiEditorBase::Edit_PasteWithTransformation_PointMoved(FsGui3DInterface *itfc)
{
	auto dlgPtr=FindTypedModelessDialog<Edit_PasteWithTransformationDialog>();
	if(nullptr!=dlgPtr)
	{
		dlgPtr->RemakeDrawingBuffer();
	}
}
void GeblGuiEditorBase::Edit_PasteWithTransformation_Draw3DCallBack(void)
{
	auto dlgPtr=FindTypedModelessDialog<Edit_PasteWithTransformationDialog>();
	if(nullptr!=dlgPtr)
	{
		YsGLSLPlain3DRenderer renderer;
		auto &drawBuf=dlgPtr->dBuf;
		renderer.SetUniformColor(YsGreen());
		renderer.DrawVtx(GL_LINES,drawBuf.normalEdgePosBuffer.GetN(),drawBuf.normalEdgePosBuffer);
	}
}
void GeblGuiEditorBase::Edit_PasteWithTransformation_Paste(void)
{
	auto dlgPtr=FindTypedModelessDialog<Edit_PasteWithTransformationDialog>();
	if(nullptr!=slHd && nullptr!=dlgPtr)
	{
		YsShellExtEdit::StopIncUndo undoGuard(*slHd);

		auto nStep=dlgPtr->divCountTxt->GetInteger();
		for(int i=1; i<=nStep; ++i)
		{
			auto tfm=dlgPtr->GetStepTransformation(i);
			cutBuffer.PasteWithTransformation(*slHd,YSFALSE,YSTRUE,tfm);
		}

		Edit_ClearUIMode();
		needRemakeDrawingBuffer=(unsigned int)GeblGuiEditorBase::NEED_REMAKE_DRAWING_ALL;
		SetNeedRedraw(YSTRUE);
	}
}
