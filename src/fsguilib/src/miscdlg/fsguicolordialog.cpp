/* ////////////////////////////////////////////////////////////

File Name: fsguicolordialog.cpp
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

#include "fsguicolordialog.h"
#include "../fsguicommondrawing.h"
#include <fssimplewindow.h>

/* virtual */ void FsGuiColorDialog::CurrentColor::Draw(YSBOOL focus,YSBOOL mouseOver,const YsColor &defBgCol,const YsColor &defFgCol,const YsColor &activeBgCol,const YsColor &activeFgCol,const YsColor &frameCol) const
{
	FsGuiCommonDrawing::DrawRect(x0,y0,x0+wid,y0+hei,showColor,YSTRUE);
}

/* virtual */ void FsGuiColorDialog::ColorBand::Draw(YSBOOL focus,YSBOOL mouseOver,const YsColor &defBgCol,const YsColor &defFgCol,const YsColor &activeBgCol,const YsColor &activeFgCol,const YsColor &frameCol) const
{
	// Tentative
	FsGuiCommonDrawing::DrawRect(x0,y0,x0+wid,y0+hei,YsBlue(),YSTRUE);

	const int uStep=5,wStep=10;
	for(int u=0; u<100; u+=uStep)
	{
		for(int w=0; w<100; w+=wStep)
		{
			const int u0=u,w0=w,u1=u+uStep,w1=w+wStep;

			const int x0=this->x0+this->wid*u0/100;
			const int y0=this->y0+this->hei*w0/100;

			const int x1=this->x0+this->wid*u1/100;
			const int y1=this->y0+this->hei*w1/100;

			const double h0=(double)u0/100.0;
			const double s0=(double)w0/100.0;
			const double v0=1.0;

			const double h1=(double)u1/100.0;
			const double s1=(double)w1/100.0;
			// const double v1=1.0;

			const double plgVtx[8]=
			{
				(double)x0,(double)y0,
				(double)x1,(double)y0,
				(double)x1,(double)y1,
				(double)x0,(double)y1
			};
			YsColor plgCol[4];
			plgCol[0].SetDoubleHSV(h0,s0,v0);
			plgCol[1].SetDoubleHSV(h1,s0,v0);
			plgCol[2].SetDoubleHSV(h1,s1,v0);
			plgCol[3].SetDoubleHSV(h0,s1,v0);

			FsGuiCommonDrawing::DrawGradationPolygon(4,plgVtx,plgCol);
		}
	}
}

/* virtual */ YSRESULT FsGuiColorDialog::ColorBand::KeyIn(int key,YSBOOL shift,YSBOOL ctrl,YSBOOL alt)
{
	return YSERR;
}

/* virtual */ void FsGuiColorDialog::ColorBand::SetMouseState(YSBOOL lb,YSBOOL mb,YSBOOL rb,int mx,int my)
{
	if(YSTRUE==lb)
	{
		const int x=mx-x0;
		const int y=my-y0;

		if(0<=x && x<wid && 0<=y && y<hei)
		{
			const double h=(double)x/(double)wid;
			const double s=(double)y/(double)hei;

			FsGuiColorDialog *colDlg=(FsGuiColorDialog *)owner;
			YsColor curColor=colDlg->GetCurrentColor();
			const double v=curColor.Vd();

			curColor.SetDoubleHSV(h,s,v);
			colDlg->SetCurrentColor(curColor);

			colDlg->SetNeedRedraw(YSTRUE);
		}
	}
}

/* virtual */ void FsGuiColorDialog::Brightness::Draw(YSBOOL focus,YSBOOL mouseOver,const YsColor &defBgCol,const YsColor &defFgCol,const YsColor &activeBgCol,const YsColor &activeFgCol,const YsColor &frameCol) const
{
	for(int t=0; t<100; t+=10)
	{
		const int y0=this->y0+hei-hei*t/100;
		const int y1=this->y0+hei-hei*(t+10)/100;

		const double v0=(double)t/100.0;
		const double v1=(double)(t+10)/100.0;

		const double rect[8]=
		{
			(double)x0,(double)y0,
			(double)x0+wid,(double)y0,
			(double)x0+wid,(double)y1,
			(double)x0,(double)y1
		};

		YsColor c0,c1;
		c0.SetDoubleRGB(v0,v0,v0);
		c1.SetDoubleRGB(v1,v1,v1);

		const YsColor col[4]=
		{
			c0,c0,c1,c1
		};

		FsGuiCommonDrawing::DrawGradationPolygon(4,rect,col);
	}
}

/* virtual */ void FsGuiColorDialog::Brightness::SetMouseState(YSBOOL lb,YSBOOL mb,YSBOOL rb,int mx,int my)
{
	if(YSTRUE==lb)
	{
		const int x=mx-x0;
		const int y=my-y0;

		if(0<=x && x<wid && 0<=y && y<hei)
		{
			const double v=1.0-(double)y/(double)hei;

			FsGuiColorDialog *colDlg=(FsGuiColorDialog *)owner;
			YsColor curColor=colDlg->GetCurrentColor();
			const double h=curColor.Hd();
			const double s=curColor.Sd();

			curColor.SetDoubleHSV(h,s,v);
			colDlg->SetCurrentColor(curColor);

			colDlg->SetNeedRedraw(YSTRUE);
		}
	}
}

FsGuiColorDialog::FsGuiColorDialog()
{
	curColorStatic=new CurrentColor(currentColor);
	colorBand=new ColorBand(currentColor);
	brightness=new Brightness(currentColor);
}

FsGuiColorDialog::~FsGuiColorDialog()
{
	delete curColorStatic;
	delete colorBand;
	delete brightness;
}

/* static */ FsGuiColorDialog *FsGuiColorDialog::Create(void)
{
	auto dlg=new FsGuiColorDialog;
	return dlg;
}

/* static */ void FsGuiColorDialog::Delete(FsGuiColorDialog *ptr)
{
	delete ptr;
}

void FsGuiColorDialog::Make(FsGuiColorPalette *linkedColorPalette)
{
	this->linkedColorPalette=linkedColorPalette;

	FsGuiDialog::Initialize();

	closeBtn=AddTextButton(0,FSKEY_ESC,FSGUI_PUSHBUTTON,L"Close",YSTRUE);

	AddCustomControl(0,FSKEY_NULL,brightness,32,240,YSTRUE);

	AddCustomControl(0,FSKEY_NULL,colorBand,320,240,YSFALSE);

	AddCustomControl(0,FSKEY_NULL,curColorStatic,64,64,YSFALSE);

	hueText=AddTextBox(0,FSKEY_NULL,FsGuiTextBox::HORIZONTAL,L"H:",4,YSTRUE);
	saturationText=AddTextBox(0,FSKEY_NULL,FsGuiTextBox::HORIZONTAL,L"S:",4,YSFALSE);
	valueText=AddTextBox(0,FSKEY_NULL,FsGuiTextBox::HORIZONTAL,L"V:",4,YSFALSE);

	redText=AddTextBox(0,FSKEY_NULL,FsGuiTextBox::HORIZONTAL,L"R:",4,YSFALSE);
	greenText=AddTextBox(0,FSKEY_NULL,FsGuiTextBox::HORIZONTAL,L"G:",4,YSFALSE);
	blueText=AddTextBox(0,FSKEY_NULL,FsGuiTextBox::HORIZONTAL,L"B:",4,YSFALSE);

	AddStaticText(0,FSKEY_NULL,L"H:",YSTRUE);
	hueSlider=AddHorizontalSlider(0,FSKEY_NULL,40,0.0,1.0,YSFALSE);
	AddStaticText(0,FSKEY_NULL,L"S:",YSTRUE);
	saturationSlider=AddHorizontalSlider(0,FSKEY_NULL,40,0.0,1.0,YSFALSE);
	AddStaticText(0,FSKEY_NULL,L"V:",YSTRUE);
	valueSlider=AddHorizontalSlider(0,FSKEY_NULL,40,0.0,1.0,YSFALSE);

	AddStaticText(0,FSKEY_NULL,L"R:",YSTRUE);
	redSlider=AddHorizontalSlider(0,FSKEY_NULL,40,0.0,1.0,YSFALSE);
	AddStaticText(0,FSKEY_NULL,L"G:",YSTRUE);
	greenSlider=AddHorizontalSlider(0,FSKEY_NULL,40,0.0,1.0,YSFALSE);
	AddStaticText(0,FSKEY_NULL,L"B:",YSTRUE);
	blueSlider=AddHorizontalSlider(0,FSKEY_NULL,40,0.0,1.0,YSFALSE);

	if(NULL!=linkedColorPalette && YSTRUE==linkedColorPalette->HasColorButton())
	{
		for(int i=0; i<16; ++i)
		{
			const YSBOOL newLine=((0==i || 8==i) ? YSTRUE : YSFALSE);

			YsString str;
			str.Printf("Set C%d",i);
			setToPaletteBtn[i]=AddTextButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,str,newLine);
		}
	}
	else
	{
		for(int i=0; i<10; ++i)
		{
			setToPaletteBtn[i]=NULL;
		}
	}

	Fit();
}

void FsGuiColorDialog::SetCurrentColor(const YsColor &col)
{
	currentColor=col;
	UpdateRGBSliderFromCurrentColor();
	UpdateHSVSliderFromCurrentColor();
	UpdateTextBoxFromSlider();
	SetNeedRedraw(YSTRUE);
}

void FsGuiColorDialog::UpdateRGBSliderFromCurrentColor(void)
{
	auto col=GetCurrentColor();
	redSlider->SetPosition(col.Rd());
	greenSlider->SetPosition(col.Gd());
	blueSlider->SetPosition(col.Bd());
}

void FsGuiColorDialog::UpdateHSVSliderFromCurrentColor(void)
{
	auto col=GetCurrentColor();
	hueSlider->SetPosition(col.Hd());
	saturationSlider->SetPosition(col.Sd());
	valueSlider->SetPosition(col.Vd());
}

void FsGuiColorDialog::UpdateTextBoxFromSlider(void) const
{
	const int Ri=(int)(redSlider->GetPosition()*255.0);
	const int Gi=(int)(greenSlider->GetPosition()*255.0);
	const int Bi=(int)(blueSlider->GetPosition()*255.0);
	const int Hi=(int)(hueSlider->GetPosition()*255.0);
	const int Si=(int)(saturationSlider->GetPosition()*255.0);
	const int Vi=(int)(valueSlider->GetPosition()*255.0);

	redText->SetInteger(Ri);
	greenText->SetInteger(Gi);
	blueText->SetInteger(Bi);

	hueText->SetInteger(Hi);
	saturationText->SetInteger(Si);
	valueText->SetInteger(Vi);
}

YsColor FsGuiColorDialog::CaptureRGBSlider(void) const
{
	YsColor c;
	c.SetDoubleRGB(redSlider->GetPosition(),greenSlider->GetPosition(),blueSlider->GetPosition());
	return c;
}

YsColor FsGuiColorDialog::CaptureHSVSlider(void) const
{
	YsColor c;
	c.SetDoubleHSV(hueSlider->GetPosition(),saturationSlider->GetPosition(),valueSlider->GetPosition());
	return c;
}

void FsGuiColorDialog::UpdateRGBTextFromCurrentColor(void)
{
	YsColor col=currentColor;
	redText->SetInteger(col.Ri());
	greenText->SetInteger(col.Gi());
	blueText->SetInteger(col.Bi());
}

void FsGuiColorDialog::UpdateHSVTextFromCurrentColor(void)
{
	YsColor col=currentColor;
	hueText->SetInteger(col.Hi());
	saturationText->SetInteger(col.Si());
	valueText->SetInteger(col.Vi());
}

void FsGuiColorDialog::UpdateSliderFromText(void)
{
	const double Rd=YsBound(atof(redText->GetString())/255.0,0.0,1.0);
	const double Gd=YsBound(atof(greenText->GetString())/255.0,0.0,1.0);
	const double Bd=YsBound(atof(blueText->GetString())/255.0,0.0,1.0);
	const double Hd=YsBound(atof(hueText->GetString())/255.0,0.0,1.0);
	const double Sd=YsBound(atof(saturationText->GetString())/255.0,0.0,1.0);
	const double Vd=YsBound(atof(valueText->GetString())/255.0,0.0,1.0);

	redSlider->SetPosition(Rd);
	greenSlider->SetPosition(Gd);
	blueSlider->SetPosition(Bd);

	hueSlider->SetPosition(Hd);
	saturationSlider->SetPosition(Sd);
	valueSlider->SetPosition(Vd);
}

YsColor FsGuiColorDialog::CaptureRGBText(void) const
{
	const double Rd=YsBound(atof(redText->GetString())/255.0,0.0,1.0);
	const double Gd=YsBound(atof(greenText->GetString())/255.0,0.0,1.0);
	const double Bd=YsBound(atof(blueText->GetString())/255.0,0.0,1.0);
	YsColor c;
	c.SetDoubleRGB(Rd,Gd,Bd);
	return c;
}

YsColor FsGuiColorDialog::CaptureHSVText(void) const
{
	const double Hd=YsBound(atof(hueText->GetString())/255.0,0.0,1.0);
	const double Sd=YsBound(atof(saturationText->GetString())/255.0,0.0,1.0);
	const double Vd=YsBound(atof(valueText->GetString())/255.0,0.0,1.0);
	YsColor c;
	c.SetDoubleHSV(Hd,Sd,Vd);
	return c;
}

const YsColor FsGuiColorDialog::GetCurrentColor(void) const
{
	return currentColor;
}

/* virtual */ void FsGuiColorDialog::OnButtonClick(FsGuiButton *btn)
{
	if(closeBtn==btn)
	{
		if(NULL!=linkedColorPalette)
		{
			linkedColorPalette->SetColor(currentColor);
			linkedColorPalette->InvokeOnColorPaletteChange();
		}
		CloseModalDialog(YSOK);
		return;
	}
	if(NULL!=linkedColorPalette)
	{
		for(int i=0; i<16; ++i)
		{
			if(btn==setToPaletteBtn[i])
			{
				linkedColorPalette->SetPalette(i,currentColor);
			}
		}
	}
}

/* virtual */ void FsGuiColorDialog::OnTextBoxChange(FsGuiTextBox *txt)
{
	if(txt==redText || txt==greenText || txt==blueText)
	{
		currentColor=CaptureRGBText();
		UpdateHSVTextFromCurrentColor();
		UpdateSliderFromText();
	}
	else if(txt==hueText || txt==saturationText || txt==valueText)
	{
		currentColor=CaptureHSVText();
		UpdateRGBTextFromCurrentColor();
		UpdateSliderFromText();
	}
}

/* virtual */ void FsGuiColorDialog::OnSliderPositionChange(FsGuiSlider *slider,const double &prevPos,const double &prevValue)
{
	if(slider==redSlider || slider==greenSlider || slider==blueSlider)
	{
		currentColor=CaptureRGBSlider();
		UpdateHSVSliderFromCurrentColor();
		UpdateTextBoxFromSlider();
	}
	else if(slider==hueSlider || slider==saturationSlider || slider==valueSlider)
	{
		currentColor=CaptureHSVSlider();
		UpdateRGBSliderFromCurrentColor();
		UpdateTextBoxFromSlider();
	}
}
