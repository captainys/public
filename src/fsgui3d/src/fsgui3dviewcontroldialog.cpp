/* ////////////////////////////////////////////////////////////

File Name: fsgui3dviewcontroldialog.cpp
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

#include "fsgui3dviewcontrol.h"
#include "fsgui3dviewcontroldialog.h"
#include <fssimplewindow.h>
#include "fsgui3dicon.h"



/* virtual */ FsGui3DViewControlDialogBase::~FsGui3DViewControlDialogBase()
{
}

/* static */ void FsGui3DViewControlDialogBase::Delete(FsGui3DViewControlDialogBase *toDel)
{
	delete toDel;
}

FsGui3DViewControl &FsGui3DViewControlDialogBase::GetViewControl(void)
{
	return *viewControl;
}

const FsGui3DViewControl &FsGui3DViewControlDialogBase::GetViewControl(void) const
{
	return *viewControl;
}

void FsGui3DViewControlDialogBase::SaveLastView(void)
{
	if(NULL!=viewControl)
	{
		lastView.pos=viewControl->GetViewTarget();
		lastView.att=viewControl->GetViewAttitude();
		lastView.zoom=viewControl->GetZoom();
		lastView.dist=viewControl->GetViewDistance();
	}
}

FsGui3DViewControlDialogBase::PosAttZoomDist FsGui3DViewControlDialogBase::RecallLastView(void)
{
	if(NULL!=viewControl)
	{
		viewControl->SetViewTarget(lastView.pos);
		viewControl->SetViewAttitude(lastView.att);
		viewControl->SetZoom(lastView.zoom);
		viewControl->SetViewDistance(lastView.dist);
	}
	return lastView;
}


////////////////////////////////////////////////////////////


FsGui3DViewControlDialog::FsGui3DViewControlDialog()
{
	viewControl=NULL;
	lastView.pos=YsOrigin();
	lastView.att=YsZeroAtt();
	SetIsPermanent(YSTRUE);
	orientationLock=YSFALSE;
	translationLock=YSFALSE;
}

FsGui3DViewControlDialog::~FsGui3DViewControlDialog()
{
}

FsGui3DViewControlDialog *FsGui3DViewControlDialog::Create(void)
{
	return new FsGui3DViewControlDialog;
}

void FsGui3DViewControlDialog::Make(class FsGuiCanvas *canvas,class FsGui3DViewControl *viewControl,YSBOOL minimize)
{
	Initialize();

	this->viewControl=viewControl;
	this->canvas=canvas;
	viewControl->AttachViewControlDialog(this);

	YsBitmap iconBmp;

	if(NULL==canvas || YSTRUE!=minimize)
	{
		if(NULL!=canvas)
		{
			closeBtn=AddTextButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,"Close",YSTRUE);
		}
		else
		{
			closeBtn=NULL;
		}
		openBtn=NULL;


		{
			iconBmp.LoadPng(sizeof(FsGui3DIcon::Shift),FsGui3DIcon::Shift);
			noSingleBtnOpBtn=AddBmpButton(0,FSKEY_NULL,FSGUI_RADIOBUTTON,iconBmp,L"-",YSFALSE);
			iconBmp.LoadPng(sizeof(FsGui3DIcon::Rotation),FsGui3DIcon::Rotation);
			rotateBtn=AddBmpButton(0,FSKEY_NULL,FSGUI_RADIOBUTTON,iconBmp,L"Rotation",YSFALSE);
			iconBmp.LoadPng(sizeof(FsGui3DIcon::Scroll),FsGui3DIcon::Scroll);
			translateBtn=AddBmpButton(0,FSKEY_NULL,FSGUI_RADIOBUTTON,iconBmp,L"Scroll",YSFALSE);
			noSingleBtnOpBtn->SetCheck(YSTRUE);

			FsGuiButton *radioButton[3]={noSingleBtnOpBtn,rotateBtn,translateBtn};

			SetRadioButtonGroup(3,radioButton);
		}


		YsArray <YsArray <FsGuiDialogItem *> > dlgItemMatrix;

		dlgItemMatrix.Set(6,NULL);
		dlgItemMatrix[0].Set(4,NULL);
		dlgItemMatrix[1].Set(4,NULL);
		dlgItemMatrix[2].Set(4,NULL);
		dlgItemMatrix[3].Set(4,NULL);
		dlgItemMatrix[4].Set(4,NULL);
		dlgItemMatrix[5].Set(4,NULL);

		iconBmp.LoadPng(sizeof(FsGui3DIcon::RollLeft),FsGui3DIcon::RollLeft);
		rollLeftBtn=AddBmpButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,iconBmp,L"<",YSTRUE);
		iconBmp.LoadPng(sizeof(FsGui3DIcon::Up),FsGui3DIcon::Up);
		noseDownBtn=AddBmpButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,iconBmp,L"^",YSFALSE);
		iconBmp.LoadPng(sizeof(FsGui3DIcon::RollRight),FsGui3DIcon::RollRight);
		rollRightBtn=AddBmpButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,iconBmp,L">",YSFALSE);
		frontViewBtn=AddTextButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,"Front",YSFALSE);

		iconBmp.LoadPng(sizeof(FsGui3DIcon::Left),FsGui3DIcon::Left);
		yawLeftBtn=AddBmpButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,iconBmp,L"<",YSTRUE);
		iconBmp.LoadPng(sizeof(FsGui3DIcon::AttitudeIndicator),FsGui3DIcon::AttitudeIndicator);
		zeroAttBtn=AddBmpButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,iconBmp,L"O",YSFALSE);
		iconBmp.LoadPng(sizeof(FsGui3DIcon::Right),FsGui3DIcon::Right);
		yawRightBtn=AddBmpButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,iconBmp,L">",YSFALSE);
		rearViewBtn=AddTextButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,"Rear",YSFALSE);

		iconBmp.LoadPng(sizeof(FsGui3DIcon::Left90),FsGui3DIcon::Left90);
		left90Btn=AddBmpButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,iconBmp,L"<",YSTRUE);
		iconBmp.LoadPng(sizeof(FsGui3DIcon::Left),FsGui3DIcon::Down);
		noseUpBtn=AddBmpButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,iconBmp,L"v",YSFALSE);
		iconBmp.LoadPng(sizeof(FsGui3DIcon::Right90),FsGui3DIcon::Right90);
		right90Btn=AddBmpButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,iconBmp,L">",YSFALSE);
		topViewBtn=AddTextButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,"Top",YSFALSE);



		iconBmp.LoadPng(sizeof(FsGui3DIcon::Up),FsGui3DIcon::Up);
		scrollUpBtn=AddBmpButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,iconBmp,L"^",YSTRUE);
		bottomViewBtn=AddTextButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,"Bottom",YSFALSE);

		iconBmp.LoadPng(sizeof(FsGui3DIcon::Left),FsGui3DIcon::Left);
		scrollLeftBtn=AddBmpButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,iconBmp,L"<",YSTRUE);
		iconBmp.LoadPng(sizeof(FsGui3DIcon::CrossHair),FsGui3DIcon::CrossHair);
		centerBtn=AddBmpButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,iconBmp,L"O",YSFALSE);
		iconBmp.LoadPng(sizeof(FsGui3DIcon::Right),FsGui3DIcon::Right);
		scrollRightBtn=AddBmpButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,iconBmp,L">",YSFALSE);
		leftViewBtn=AddTextButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,"Left",YSFALSE);

		iconBmp.LoadPng(sizeof(FsGui3DIcon::Left),FsGui3DIcon::Down);
		scrollDownBtn=AddBmpButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,iconBmp,L"v",YSTRUE);
		rightViewBtn=AddTextButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,"Right",YSFALSE);


		dlgItemMatrix[0][0]=rollLeftBtn;
		dlgItemMatrix[0][1]=noseDownBtn;
		dlgItemMatrix[0][2]=rollRightBtn;
		dlgItemMatrix[0][3]=frontViewBtn;

		dlgItemMatrix[1][0]=yawLeftBtn;
		dlgItemMatrix[1][1]=zeroAttBtn;
		dlgItemMatrix[1][2]=yawRightBtn;
		dlgItemMatrix[1][3]=rearViewBtn;

		dlgItemMatrix[2][0]=left90Btn;
		dlgItemMatrix[2][1]=noseUpBtn;
		dlgItemMatrix[2][2]=right90Btn;
		dlgItemMatrix[2][3]=topViewBtn;

		dlgItemMatrix[3][0]=NULL;
		dlgItemMatrix[3][1]=scrollUpBtn;
		dlgItemMatrix[3][2]=NULL;
		dlgItemMatrix[3][3]=bottomViewBtn;

		dlgItemMatrix[4][0]=scrollLeftBtn;
		dlgItemMatrix[4][1]=centerBtn;
		dlgItemMatrix[4][2]=scrollRightBtn;
		dlgItemMatrix[4][3]=leftViewBtn;

		dlgItemMatrix[5][0]=NULL;
		dlgItemMatrix[5][1]=scrollDownBtn;
		dlgItemMatrix[5][2]=NULL;
		dlgItemMatrix[5][3]=rightViewBtn;

		AlignLeftMiddle(dlgItemMatrix);



		iconBmp.LoadPng(sizeof(FsGui3DIcon::Lock),FsGui3DIcon::Lock);
		AddStaticBmp(0,FSKEY_NULL,iconBmp,YSTRUE);
		iconBmp.LoadPng(sizeof(FsGui3DIcon::Rotation),FsGui3DIcon::Rotation);
		lockOrientationBtn=AddBmpButton(0,FSKEY_NULL,FSGUI_CHECKBOX,iconBmp,L"Rotation",YSFALSE);
		iconBmp.LoadPng(sizeof(FsGui3DIcon::Scroll),FsGui3DIcon::Scroll);
		lockTranslationBtn=AddBmpButton(0,FSKEY_NULL,FSGUI_CHECKBOX,iconBmp,L"Scroll",YSFALSE);



		zoomBtn=AddTextButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,"Zoom",YSTRUE);
		moozBtn=AddTextButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,"Mooz",YSFALSE);

		iconBmp.LoadPng(sizeof(FsGui3DIcon::OrthoPers),FsGui3DIcon::OrthoPers);
		orthoPersBtn=AddBmpButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,iconBmp,L"Ortho/Pers",YSFALSE);



		lookAtCoordTxt=AddTextBox(0,FSKEY_NULL,"",16,YSTRUE);
		lookAtCoordTxt->SetTextType(FSGUI_ASCIISTRING);
		lookAtCoordTxt->SetLengthLimit(256);
		lookAtBtn=AddTextButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,"Look At",YSTRUE);
		lastViewBtn=AddTextButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,"Last View",YSFALSE);
		lookAtCutBufBtn=AddTextButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,"Look At Cut-Buffer",YSTRUE);



		saveViewBtn=AddTextButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,"Save View",YSTRUE);
		loadViewBtn=AddTextButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,"Load View",YSFALSE);

		SetBackgroundAlpha(0.5);

		SetOrientationLock(orientationLock);
		SetTranslationLock(translationLock);
	}
	else
	{
		closeBtn=NULL;
		openBtn=AddTextButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,"Open View-Control Dialog",YSTRUE);

		rollLeftBtn=NULL;
		noseDownBtn=NULL;
		rollRightBtn=NULL;
		frontViewBtn=NULL;

		yawLeftBtn=NULL;
		zeroAttBtn=NULL;
		yawRightBtn=NULL;
		rearViewBtn=NULL;

		left90Btn=NULL;
		noseUpBtn=NULL;
		right90Btn=NULL;
		topViewBtn=NULL;



		scrollUpBtn=NULL;
		bottomViewBtn=NULL;

		scrollLeftBtn=NULL;
		centerBtn=NULL;
		scrollRightBtn=NULL;
		leftViewBtn=NULL;

		scrollDownBtn=NULL;
		rightViewBtn=NULL;

		lockOrientationBtn=NULL;
		lockTranslationBtn=NULL;

		zoomBtn=NULL;
		moozBtn=NULL;
		orthoPersBtn=NULL;

		lookAtCoordTxt=NULL;
		lookAtBtn=NULL;
		lastViewBtn=NULL;
		lookAtCutBufBtn=NULL;

		noSingleBtnOpBtn=NULL;
		rotateBtn=NULL;
		translateBtn=NULL;

		saveViewBtn=NULL;
		loadViewBtn=NULL;

		SetBackgroundAlpha(0.0);
	}

	SetArrangeType(FSDIALOG_ARRANGE_TOP_RIGHT);
	Fit();
}



void FsGui3DViewControlDialog::OnButtonClick(FsGuiButton *btn)
{
	if(NULL==viewControl)
	{
		return;
	}

	if(NULL!=btn && btn==openBtn && NULL!=canvas)
	{
		canvas->RemoveDialog(this);
		Make(canvas,viewControl,YSFALSE);
		canvas->AddDialog(this);
		canvas->ArrangeDialog();
	}
	else if(NULL!=btn && btn==closeBtn && NULL!=canvas)
	{
		canvas->RemoveDialog(this);
		Make(canvas,viewControl,YSTRUE);
		canvas->AddDialog(this);
		canvas->ArrangeDialog();
	}

	if(NULL!=btn && btn==rollLeftBtn)
	{
		YsAtt3 att=viewControl->GetViewAttitude();
		att.AddB(YsPi/36.0);
		viewControl->SetViewAttitude(att);
		viewControl->SetNeedRedraw(YSTRUE);
	}
	if(NULL!=btn && btn==noseDownBtn)
	{
		YsAtt3 att=viewControl->GetViewAttitude();
		att.NoseUp(-YsPi/36.0);
		viewControl->SetViewAttitude(att);
		viewControl->SetNeedRedraw(YSTRUE);
	}
	if(NULL!=btn && btn==rollRightBtn)
	{
		YsAtt3 att=viewControl->GetViewAttitude();
		att.AddB(-YsPi/36.0);
		viewControl->SetViewAttitude(att);
		viewControl->SetNeedRedraw(YSTRUE);
	}
	if(NULL!=btn && btn==yawLeftBtn)
	{
		YsAtt3 att=viewControl->GetViewAttitude();
		att.YawLeft(YsPi/36.0);
		viewControl->SetViewAttitude(att);
		viewControl->SetNeedRedraw(YSTRUE);
	}
	if(NULL!=btn && btn==zeroAttBtn)
	{
		viewControl->StraightenAttitude();
		viewControl->SetNeedRedraw(YSTRUE);
	}
	if(NULL!=btn && btn==yawRightBtn)
	{
		YsAtt3 att=viewControl->GetViewAttitude();
		att.YawLeft(-YsPi/36.0);
		viewControl->SetViewAttitude(att);
		viewControl->SetNeedRedraw(YSTRUE);
	}
	if(NULL!=btn && btn==noseUpBtn)
	{
		YsAtt3 att=viewControl->GetViewAttitude();
		att.NoseUp(YsPi/36.0);
		viewControl->SetViewAttitude(att);
		viewControl->SetNeedRedraw(YSTRUE);
	}

	if(NULL!=btn && btn==scrollUpBtn)
	{
		Translate(0.0,-0.05);
	}
	if(NULL!=btn && btn==scrollLeftBtn)
	{
		Translate(0.05,0.0);
	}
	if(NULL!=btn && btn==centerBtn)
	{
		YsVec3 min,max;
		viewControl->GetTargetBoundingBox(min,max);
		viewControl->SetViewTarget((min+max)/2.0);
	}
	if(NULL!=btn && btn==scrollRightBtn)
	{
		Translate(-0.05,0.0);
	}
	if(NULL!=btn && btn==scrollDownBtn)
	{
		Translate(0.0,0.05);
	}

	if(NULL!=btn && btn==lockOrientationBtn)
	{
		SetOrientationLock(lockOrientationBtn->GetCheck());
	}
	if(NULL!=btn && btn==lockTranslationBtn)
	{
		SetTranslationLock(lockTranslationBtn->GetCheck());
	}

	if(NULL!=btn && btn==frontViewBtn)
	{
		YsAtt3 att=viewControl->GetStraightAttitude();
		viewControl->SetViewAttitude(att);
		viewControl->SetNeedRedraw(YSTRUE);
	}
	if(NULL!=btn && btn==rearViewBtn)
	{
		YsAtt3 att=viewControl->GetStraightAttitude();
		att.YawLeft(YsPi);
		viewControl->SetViewAttitude(att);
		viewControl->SetNeedRedraw(YSTRUE);
	}
	if(NULL!=btn && btn==topViewBtn)
	{
		YsAtt3 att=viewControl->GetStraightAttitude();
		att.NoseUp(-YsPi/2.0);
		viewControl->SetViewAttitude(att);
		viewControl->SetNeedRedraw(YSTRUE);
	}
	if(NULL!=btn && btn==bottomViewBtn)
	{
		YsAtt3 att=viewControl->GetStraightAttitude();
		att.NoseUp(YsPi/2.0);
		viewControl->SetViewAttitude(att);
		viewControl->SetNeedRedraw(YSTRUE);
	}
	if(NULL!=btn && btn==leftViewBtn)
	{
		YsAtt3 att=viewControl->GetStraightAttitude();
		att.YawLeft(-YsPi/2.0);
		viewControl->SetViewAttitude(att);
		viewControl->SetNeedRedraw(YSTRUE);
	}
	if(NULL!=btn && btn==rightViewBtn)
	{
		YsAtt3 att=viewControl->GetStraightAttitude();
		att.YawLeft(YsPi/2.0);
		viewControl->SetViewAttitude(att);
		viewControl->SetNeedRedraw(YSTRUE);
	}

	if(NULL!=btn && btn==zoomBtn)
	{
		viewControl->Zoom(1.1);
	}
	if(NULL!=btn && btn==moozBtn)
	{
		viewControl->Zoom(1.0/1.1);
	}
	if(NULL!=btn && btn==orthoPersBtn)
	{
		YSBOOL pers=viewControl->GetPerspective();
		YsFlip(pers);
		viewControl->SetPerspective(pers);
	}
	if(NULL!=btn && btn==left90Btn)
	{
		YsAtt3 att=viewControl->GetViewAttitude();
		att.YawLeft(YsPi/2.0);
		viewControl->SetViewAttitude(att);
		viewControl->SetNeedRedraw(YSTRUE);
	}
	if(NULL!=btn && btn==right90Btn)
	{
		YsAtt3 att=viewControl->GetViewAttitude();
		att.YawLeft(-YsPi/2.0);
		viewControl->SetViewAttitude(att);
		viewControl->SetNeedRedraw(YSTRUE);
	}

	//FsGuiTextBox *lookAtCoordTxt

	if(NULL!=btn && btn==lookAtBtn)
	{
		SaveLastView();
		YsString str;
		lookAtCoordTxt->GetText(str);
		for(YSSIZE_T idx=0; idx<str.Strlen(); ++idx)
		{
			if('('==str[idx] || ')'==str[idx] || ','==str[idx])
			{
				str.Set(idx,' ');
			}
		}
		YsArray <YsString,4> xyz;
		str.Arguments(xyz);

		YsVec3 pos=viewControl->GetViewTarget();
		if(1<=xyz.GetN())
		{
			pos.SetX(atof(xyz[0]));
		}
		if(2<=xyz.GetN())
		{
			pos.SetY(atof(xyz[1]));
		}
		if(3<=xyz.GetN())
		{
			pos.SetZ(atof(xyz[2]));
		}

		viewControl->SetViewTarget(pos);
	}
	if(NULL!=btn && btn==lastViewBtn)
	{
		RecallLastView();
	}
	if(NULL!=btn && btn==lookAtCutBufBtn)
	{
	}

	if(NULL!=btn && btn==saveViewBtn)
	{
		auto parent=GetParent();
		if(NULL!=parent)
		{
			parent->OnSaveView(this);
		}
	}
	if(NULL!=btn && btn==loadViewBtn)
	{
		auto parent=GetParent();
		if(NULL!=parent)
		{
			parent->OnLoadView(this);
		}
	}
}

void FsGui3DViewControlDialog::Translate(double dx,double dy)
{
	if(NULL!=viewControl && NULL!=lockTranslationBtn && YSTRUE!=lockTranslationBtn->GetCheck())
	{
		double vx,vy;
		if(viewControl->GetPerspective()!=YSTRUE)
		{
			double x1,y1,x2,y2;
			viewControl->GetOrthogonalRangeWithZoom(x1,y1,x2,y2);
			const double sx=fabs(x1-x2);
			const double sy=fabs(y1-y2);

			vx=dx*sx;
			vy=dy*sy;
		}
		else
		{
			const double tanfov=tan(viewControl->GetFOVY());
			const double viewDist=viewControl->GetViewDistance();
			vx=2.0*(dx*viewDist*tanfov/viewControl->GetZoom());
			vy=2.0*(dy*viewDist*tanfov/viewControl->GetZoom());
		}

		YsVec3 v(vx,vy,0.0);
		viewControl->GetViewMatrix().MulInverse(v,v,0.0);
		v+=viewControl->GetViewTarget();

		viewControl->SetViewTarget(v);
	}
}

void FsGui3DViewControlDialog::SetOrientationLock(YSBOOL lock)
{
	orientationLock=lock;
	if(NULL!=viewControl)
	{
		viewControl->SetOrientationLock(lock);
		  // viewControl->SetOrientationLock will call this function back only if lock status has changed.
		  // Therefore infinite recursion is avoided.
	}
	if(NULL!=lockOrientationBtn)
	{
		lockOrientationBtn->SetCheck(lock);
	}

	if(NULL!=rollLeftBtn)
	{
		rollLeftBtn->SetEnabled(YsReverseBool(lock));
	}
	if(NULL!=noseDownBtn)
	{
		noseDownBtn->SetEnabled(YsReverseBool(lock));
	}
	if(NULL!=rollRightBtn)
	{
		rollRightBtn->SetEnabled(YsReverseBool(lock));
	}
	if(NULL!=yawLeftBtn)
	{
		yawLeftBtn->SetEnabled(YsReverseBool(lock));
	}
	if(NULL!=zeroAttBtn)
	{
		zeroAttBtn->SetEnabled(YsReverseBool(lock));
	}
	if(NULL!=yawRightBtn)
	{
		yawRightBtn->SetEnabled(YsReverseBool(lock));
	}
	if(NULL!=noseUpBtn)
	{
		noseUpBtn->SetEnabled(YsReverseBool(lock));
	}
	if(nullptr!=left90Btn)
	{
		left90Btn->SetEnabled(YsReverseBool(lock));
	}
	if(nullptr!=right90Btn)
	{
		right90Btn->SetEnabled(YsReverseBool(lock));
	}

	if(NULL!=frontViewBtn)
	{
		frontViewBtn->SetEnabled(YsReverseBool(lock));
	}
	if(NULL!=rearViewBtn)
	{
		rearViewBtn->SetEnabled(YsReverseBool(lock));
	}
	if(NULL!=topViewBtn)
	{
		topViewBtn->SetEnabled(YsReverseBool(lock));
	}
	if(NULL!=bottomViewBtn)
	{
		bottomViewBtn->SetEnabled(YsReverseBool(lock));
	}
	if(NULL!=leftViewBtn)
	{
		leftViewBtn->SetEnabled(YsReverseBool(lock));
	}
	if(NULL!=rightViewBtn)
	{
		rightViewBtn->SetEnabled(YsReverseBool(lock));
	}
	if(nullptr!=rotateBtn)
	{
		rotateBtn->SetEnabled(YsReverseBool(lock));
	}
}

void FsGui3DViewControlDialog::SetTranslationLock(YSBOOL lock)
{
	translationLock=lock;
	if(NULL!=viewControl)
	{
		// viewControl->SetTranslationLock will call this function back only if lock status has changed.
		// Therefore infinite recursion is avoided.
		viewControl->SetTranslationLock(lock);
	}
	if(NULL!=lockTranslationBtn)
	{
		lockTranslationBtn->SetCheck(lock);
	}

	if(NULL!=scrollUpBtn)
	{
		scrollUpBtn->SetEnabled(YsReverseBool(lock));
	}
	if(NULL!=scrollLeftBtn)
	{
		scrollLeftBtn->SetEnabled(YsReverseBool(lock));
	}
	if(NULL!=centerBtn)
	{
		centerBtn->SetEnabled(YsReverseBool(lock));
	}
	if(NULL!=scrollRightBtn)
	{
		scrollRightBtn->SetEnabled(YsReverseBool(lock));
	}
	if(NULL!=scrollDownBtn)
	{
		scrollDownBtn->SetEnabled(YsReverseBool(lock));
	}
	if(nullptr!=translateBtn)
	{
		translateBtn->SetEnabled(YsReverseBool(lock));
	}
}

void FsGui3DViewControlDialog::UpdateViewTarget(const YsVec3 &pos)
{
	if(NULL!=lookAtCoordTxt)
	{
		YsString str;
		str.Printf("%.3lf, %.3lf, %.3lf",pos.x(),pos.y(),pos.z());
		lookAtCoordTxt->SetText(str);
	}
}

YSBOOL FsGui3DViewControlDialog::SingleButtonRotation(void) const
{
	if(NULL!=rotateBtn)
	{
		return rotateBtn->GetCheck();
	}
	return YSFALSE;
}

YSBOOL FsGui3DViewControlDialog::SingleButtonScroll(void) const
{
	if(NULL!=translateBtn)
	{
		return translateBtn->GetCheck();
	}
	return YSFALSE;
}

/* virtual */ YSBOOL FsGui3DViewControlDialog::SingleButtonZoom(void) const
{
	return YSFALSE;
}

