/* ////////////////////////////////////////////////////////////

File Name: fsguicolordialog.h
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

#ifndef FSGUICOLORDLG_IS_INCLUDED
#define FSGUICOLORDLG_IS_INCLUDED
/* { */

#include "../fsguidialog.h"

class FsGuiColorDialog : public FsGuiDialog
{
public:
	FsGuiColorPalette *linkedColorPalette;
	YsColor currentColor;

	class CurrentColor : public FsGuiDialogItem
	{
	private:
		CurrentColor &operator=(const CurrentColor &);
	public:
		YsColor &showColor;

		CurrentColor(YsColor &currentColor) : showColor(currentColor)
		{
		}
		virtual void Draw(YSBOOL focus,YSBOOL mouseOver,const YsColor &defBgCol,const YsColor &defFgCol,const YsColor &activeBgCol,const YsColor &activeFgCol,const YsColor &frameCol) const;
	};
	class ColorBand : public FsGuiDialogItem
	{
	private:
		ColorBand &operator=(const ColorBand &);
	public:
		double s,v;
		YsColor &showColor;

		ColorBand(YsColor &currentColor) : showColor(currentColor)
		{
		}
		virtual void Draw(YSBOOL focus,YSBOOL mouseOver,const YsColor &defBgCol,const YsColor &defFgCol,const YsColor &activeBgCol,const YsColor &activeFgCol,const YsColor &frameCol) const;
		virtual YSRESULT KeyIn(int key,YSBOOL shift,YSBOOL ctrl,YSBOOL alt);
		virtual void SetMouseState(YSBOOL lb,YSBOOL mb,YSBOOL rb,int mx,int my);
	};
	class Brightness : public FsGuiDialogItem
	{
	private:
		Brightness &operator=(const Brightness &);
	public:
		YsColor &showColor;

		Brightness(YsColor &currentColor) : showColor(currentColor)
		{
		}

		virtual void Draw(YSBOOL focus,YSBOOL mouseOver,const YsColor &defBgCol,const YsColor &defFgCol,const YsColor &activeBgCol,const YsColor &activeFgCol,const YsColor &frameCol) const;
		virtual void SetMouseState(YSBOOL lb,YSBOOL mb,YSBOOL rb,int mx,int my);
	};

private:
	FsGuiButton *closeBtn;
	CurrentColor *curColorStatic; // <- showColor is the selected color.
	ColorBand *colorBand;
	Brightness *brightness;

	FsGuiTextBox *redText,*greenText,*blueText;
	FsGuiTextBox *hueText,*saturationText,*valueText;

	FsGuiSlider *hueSlider;
	FsGuiSlider *saturationSlider;
	FsGuiSlider *valueSlider;

	FsGuiSlider *redSlider;
	FsGuiSlider *greenSlider;
	FsGuiSlider *blueSlider;

	FsGuiButton *setToPaletteBtn[16];

	FsGuiColorDialog();
	~FsGuiColorDialog();

public:
	static FsGuiColorDialog *Create(void);
	static void Delete(FsGuiColorDialog *ptr);

	void Make(FsGuiColorPalette *linkedColorPalette);
	void SetCurrentColor(const YsColor &col);
private:
	void UpdateRGBSliderFromCurrentColor(void);
	void UpdateHSVSliderFromCurrentColor(void);
	void UpdateTextBoxFromSlider(void) const;
	YsColor CaptureRGBSlider(void) const;
	YsColor CaptureHSVSlider(void) const;

	void UpdateRGBTextFromCurrentColor(void);
	void UpdateHSVTextFromCurrentColor(void);
	void UpdateSliderFromText(void);
	YsColor CaptureRGBText(void) const;
	YsColor CaptureHSVText(void) const;

public:
	const YsColor GetCurrentColor(void) const;

	virtual void OnButtonClick(FsGuiButton *btn);
	virtual void OnSliderPositionChange(FsGuiSlider *slider,const double &prevPos,const double &prevValue);
	virtual void OnTextBoxChange(FsGuiTextBox *txt);
};

/* } */
#endif
