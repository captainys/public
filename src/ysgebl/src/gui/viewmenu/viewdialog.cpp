/* ////////////////////////////////////////////////////////////

File Name: viewdialog.cpp
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

#include "../ysgebl_gui_editor_base.h"
#include "viewdialog.h"

#include <ysgl.h>
#include <fssimplewindow.h>

#include <fsguifiledialog.h>


PolyCreCrossSectionDialog::PolyCreCrossSectionDialog()
{
	canvas=NULL;
	closeBtn=NULL;
}

PolyCreCrossSectionDialog::~PolyCreCrossSectionDialog()
{
}

/*static*/ PolyCreCrossSectionDialog *PolyCreCrossSectionDialog::Create(class GeblGuiEditorBase *canvas)
{
	PolyCreCrossSectionDialog *dlg=new PolyCreCrossSectionDialog;
	dlg->canvas=canvas;
	return dlg;
}

/*static*/ void PolyCreCrossSectionDialog::Delete(PolyCreCrossSectionDialog *ptr)
{
	delete ptr;
}

YSRESULT PolyCreCrossSectionDialog::Make(GeblGuiEditorBase *canvas)
{
	this->canvas=canvas;

	if(NULL==closeBtn)
	{
		FsGuiDialog::Initialize();

		slider=AddHorizontalSlider(0,FSKEY_NULL,80,0.0,1.0,YSTRUE);

		backwardBtn=AddTextButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,L"<<",YSTRUE);
		forwardBtn=AddTextButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,L">>",YSFALSE);
		closeBtn=AddTextButton(0,FSKEY_ESC,FSGUI_PUSHBUTTON,L"Close",YSFALSE);

		SetArrangeType(FSDIALOG_ARRANGE_TOP_LEFT);
		SetBackgroundAlpha(0.3);
		Fit();
	}

	RecalculateSliderRange();

	return YSOK;
}

void PolyCreCrossSectionDialog::RecalculateSliderRange(void)
{
	auto &canvas=*(this->canvas);
	if(NULL!=canvas.Slhd())
	{
		auto &shl=*canvas.Slhd();
		auto pln=shl.GetDrawingBuffer().GetCrossSection();

		this->org=pln.GetOrigin();
		this->vec=pln.GetNormal();


		minT=0.0,maxT=0.0;
		YSBOOL first=YSTRUE;
		for(auto vtHd : shl.AllVertex())
		{
			auto vtPos=shl.GetVertexPosition(vtHd);

			const double t=(vtPos-this->org)*this->vec;

			if(YSTRUE==first)
			{
				minT=t;
				maxT=t;
				first=YSFALSE;
			}
			else
			{
				minT=YsSmaller(minT,t);
				maxT=YsGreater(maxT,t);
			}
		}
	}

	if(YsTolerance>maxT-minT)
	{
		maxT=minT+1.0;  // At least avoid zero division.
	}

	const double currentT=0.0;
	const double param=1.0-(currentT-minT)/(maxT-minT);
	slider->SetPosition(1.0-param);
	slider->SetStep(0.001);
}

void PolyCreCrossSectionDialog::RemakeDrawingBuffer(void)
{
	// As required
}

void PolyCreCrossSectionDialog::ResetCrossSectionFromSliderPosition(void)
{
	auto &canvas=*(this->canvas);
	if(NULL!=canvas.Slhd())
	{
		auto &shl=*canvas.Slhd();

		const double t=1.0-slider->GetPosition();
		const double scaledT=minT*(1.0-t)+maxT*t;

		const YsVec3 newOrigin=org+vec*scaledT;
		shl.GetDrawingBuffer().SetCrossSection(YsPlane(newOrigin,vec));
		if(YSTRUE==shl.GetDrawingBuffer().CrossSectionEnabled())
		{
			canvas.needRemakeDrawingBuffer|=GeblGuiEditorBase::NEED_REMAKE_DRAWING_ALL;
			canvas.SetNeedRedraw(YSTRUE);
		}
	}
}

void PolyCreCrossSectionDialog::OnButtonClick(FsGuiButton *btn)
{
	if(btn==closeBtn)
	{
		canvas->Edit_ClearUIMode();
	}
	if(btn==forwardBtn)
	{
		slider->Increment();
		ResetCrossSectionFromSliderPosition();
	}
	if(btn==backwardBtn)
	{
		slider->Decrement();
		ResetCrossSectionFromSliderPosition();
	}
}

void PolyCreCrossSectionDialog::OnSliderPositionChange(FsGuiSlider *slider,const double &prevPos,const double &prevValue)
{
	ResetCrossSectionFromSliderPosition();
}

/* static */ void GeblGuiEditorBase::View_CrossSection_Move(void *appPtr,FsGuiPopUpMenu *,FsGuiPopUpMenuItem *)
{
	GeblGuiEditorBase &canvas=*(GeblGuiEditorBase *)appPtr;
	if(YSOK==canvas.crossSectionDlg->Make(&canvas))
	{
		canvas.AddDialog(canvas.crossSectionDlg);
		canvas.ArrangeDialog();
	}
}

