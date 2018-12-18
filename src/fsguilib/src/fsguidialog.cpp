/* ////////////////////////////////////////////////////////////

File Name: fsguidialog.cpp
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <time.h>



#include <ysclass.h>

#include <fssimplewindow.h>
#include "fsguiclipboard.h"

#include <ysbitmap.h>
#include <ysfontrenderer.h>

#include "fsgui.h"
#include "fsguibitmaputil.h"

#include "miscdlg/fsguicolordialog.h" // Needed for FsGuiColorPalette



#include <time.h>




FsGuiDialogItem::FsGuiDialogItem()
{
	Initialize(NULL);
}

void FsGuiDialogItem::GetScrollBarBugPosition(int &bugX1,int &bugX2,int barX1,int barX2,int totalLength,int showTop,int showLength) const
{
	const int sliderHei=barX2-barX1+1;

	const int sliderBugHei=sliderHei*showLength/totalLength;

	const int sliderBugPos=sliderHei*showTop/totalLength;

	bugX1=barX1+sliderBugPos;
	bugX2=YsSmaller(barX1+sliderBugPos+sliderBugHei,barX2);
}

int FsGuiDialogItem::GetShowTopFromPosition(int barX1,int barX2,int mouseX,int totalLength,int showLength) const
{
	int newShowTop=totalLength*(mouseX-barX1)/(barX2-barX1);
	newShowTop=YsBound(newShowTop,0,totalLength-showLength/2);
	return newShowTop;
}

void FsGuiDialogItem::Initialize(FsGuiDialog *owner)
{
	id=-1;
	x0=0;
	y0=0;
	wid=0;
	hei=0;
	this->owner=owner;
	enabled=YSTRUE;
	wLabel.Set(NULL);
	isTabStop=YSTRUE;
	isVisible=YSTRUE;
	newLine=YSTRUE;
	useDefaultFrame=YSTRUE;

	isText=YSTFUNKNOWN;
	prevDrawingType=FSGUI_DRAWINGTYPE_UNKNOWN;

	bmp.CleanUp();
	negativeBmp.CleanUp();
}

FSGUIITEMTYPE FsGuiDialogItem::GetItemType(void) const
{
	return itemType;
}

const YsWString &FsGuiDialogItem::GetLabel(void) const
{
	return wLabel;
}

YSBOOL FsGuiDialogItem::IsEnabled(void) const
{
	if(YSTRUE==enabled && YSTRUE==isVisible)
	{
		return YSTRUE;
	}
	return YSFALSE;
}

YSBOOL FsGuiDialogItem::IsTabStop(void) const
{
	return isTabStop;
}

YSBOOL FsGuiDialogItem::IsInRange(int mx,int my) const
{
	if(x0<=mx && mx<x0+wid && y0<=my && my<y0+hei)
	{
		return YSTRUE;
	}
	return YSFALSE;
}

YSBOOL FsGuiDialogItem::IsVisible(void) const
{
	return isVisible;
}

void FsGuiDialogItem::Enable(void)
{
	enabled=YSTRUE;
}

void FsGuiDialogItem::Disable(void)
{
	enabled=YSFALSE;
}

void FsGuiDialogItem::SetEnabled(YSBOOL enabled)
{
	this->enabled=enabled;
}

void FsGuiDialogItem::SetUseDefaultFrame(YSBOOL useDefaultFrame)
{
	this->useDefaultFrame=useDefaultFrame;
}

void FsGuiDialogItem::SetNewLineWhenCreated(YSBOOL newLine)
{
	this->newLine=newLine;
}

void FsGuiDialogItem::SetNegativeBitmap(const YsBitmap &negBmp)
{
	negativeBmp.SetBitmap(negBmp);
}

void FsGuiDialogItem::Show(void)
{
	isVisible=YSTRUE;
}

void FsGuiDialogItem::Hide(void)
{
	isVisible=YSFALSE;
}

void FsGuiDialogItem::LButtonDown(int /*mx*/,int /*my*/)
{
}

void FsGuiDialogItem::LButtonUp(int /*mx*/,int /*my*/)
{
}

void FsGuiDialogItem::SetMouseState(YSBOOL /*lb*/,YSBOOL /*mb*/,YSBOOL /*rb*/,int /*mx*/,int /*my*/)
{
}

YSBOOL FsGuiDialogItem::Highlight(int mx,int my)
{
	if(x0<=mx && mx<=x0+wid && y0<=my && my<=y0+hei)
	{
		return YSTRUE;
	}
	return YSFALSE;
}

YSRESULT FsGuiDialogItem::KeyIn(int /*key*/,YSBOOL /*shift*/,YSBOOL /*ctrl*/,YSBOOL /*alt*/)
{
	return YSERR;
}

YSRESULT FsGuiDialogItem::CharIn(int)
{
	return YSERR;
}

FsGuiStatic::FsGuiStatic()
{
	itemType=FSGUI_STATIC;
	Initialize(NULL);
}

YSBOOL FsGuiDialogItem::IsInRightSideArrow(YSBOOL &up,YSBOOL &down,int mx,int my,int /*x0*/,int y0,int x1,int y1) const
{
	int w;
	w=(y1-y0+1)/2;
	if(x1-w<=mx && mx<=x1)
	{
		if(y0<=my && my<=y0+w)
		{
			up=YSTRUE;
			down=YSFALSE;
			return YSTRUE;
		}
		if(y0+w<my && my<=y1)
		{
			up=YSFALSE;
			down=YSTRUE;
			return YSTRUE;
		}
	}
	up=YSFALSE;
	down=YSFALSE;
	return YSFALSE;
}

YsVec2i FsGuiDialogItem::GetCenter(void) const
{
	YsVec2i cen;
	cen.SetX(x0+wid/2);
	cen.SetY(y0+hei/2);
	return cen;
}

YSBOOL FsGuiDialogItem::NeedRedrawActiveItem(void) const
{
	return YSFALSE;
}

void FsGuiStatic::Initialize(FsGuiDialog *owner)
{
	FsGuiDialogItem::Initialize(owner);

	wLabel.Set(NULL);
	bmp.CleanUp();

	frame=YSTRUE;
	fill=YSTRUE;

	fgCol=defFgCol;
	bgCol=defBgCol;

	isTabStop=YSFALSE;  // Static is not a TAB stop.
}

int FsGuiDialogItem::GetIdent(void) const
{
	return id;
}

void FsGuiStatic::SetText(const char txt[])
{
	wLabel.SetUTF8String(txt);
	SetText(wLabel);
}

void FsGuiStatic::SetText(const wchar_t wLabel[])
{
	YsWString wStr;
	if(NULL!=wLabel && 0!=wLabel[0])
	{
		wStr.Set(wLabel);
		this->wLabel.Set(wLabel);
	}
	else
	{
		const wchar_t space[]={' ',0}; // Workaround to cope with 64-bit GCC bug in Mac OSX
		wStr.Set(space); // One space for display purpose.
		this->wLabel.Set(NULL); // Actual data must be cleared.
	}

	RenderOwnText();
}

void FsGuiStatic::RenderOwnText(void)
{
	int msgWid=0,msgHei=0;

	YsBitmap wholeBmp;
	if(YSOK==RenderUnicodeString(wholeBmp,wLabel,fgCol,bgCol) ||
	   YSOK==RenderAsciiString(wholeBmp,wLabel,fgCol,bgCol))
	{
		bmp.SetBitmap(wholeBmp);
		msgWid=bmp.GetWidth();
		msgHei=bmp.GetHeight();
	}
	else
	{
		bmp.MakeSmallEmptyBitmap();
		msgWid=8;
		msgHei=8;
	}

	this->wid=msgWid+defHSpaceUnit;
	this->hei=msgHei+defVSpaceUnit/2;

	if(owner!=NULL)
	{
		owner->SetNeedRedraw(YSTRUE);
	}
}

void FsGuiStatic::GetText(YsString &str) const
{
	str.EncodeUTF8 <wchar_t> (wLabel);
}

void FsGuiStatic::GetText(YsWString &wStr) const
{
	wStr.Set(wLabel);
}

YsString FsGuiStatic::GetText(void) const
{
	YsString str;
	str.EncodeUTF8 <wchar_t> (wLabel);
	return str;
}

YsWString FsGuiStatic::GetWText(void) const
{
	return wLabel;
}

void FsGuiStatic::SetFgColor(const YsColor &fgColIn)
{
	fgCol=fgColIn;
	RenderOwnText();
}


void FsGuiStatic::SetDrawFrame(YSBOOL frame)
{
	this->frame=frame;
}

void FsGuiStatic::SetFill(YSBOOL fill)
{
	this->fill=fill;
}

YSBOOL FsGuiStatic::GetDrawFrame(void) const
{
	return frame;
}

YSBOOL FsGuiStatic::GetFill(void) const
{
	return fill;
}


FsGuiButton::FsGuiButton()
{
	itemType=FSGUI_BUTTON;
	Initialize(NULL);
}

void FsGuiButton::Initialize(FsGuiDialog *owner)
{
	FsGuiDialogItem::Initialize(owner);

	btnType=FSGUI_PUSHBUTTON;
	checked=YSFALSE;
	btnGroup.Set(0,NULL);

}

void FsGuiButton::SetCheck(YSBOOL checked)
{
	this->checked=checked;
	if(owner!=NULL)
	{
		owner->SetNeedRedraw(YSTRUE);
	}
}

YSBOOL FsGuiButton::GetCheck(void) const
{
	return checked;
}

void FsGuiButton::CalculateButtonSizeFromMessageSize(int &btnWid,int &btnHei,int msgWid,int msgHei) const
{
	const int checkBoxWid=((FSGUI_PUSHBUTTON!=btnType && scheme!=MODERN) ? defHAnnotation : 0);
	btnWid=msgWid+defHSpaceUnit*3/2+checkBoxWid;
	btnHei=msgHei+defVSpaceUnit;
}

void FsGuiButton::SetText(const char label[])
{
	YsWString wStr;
	wStr.SetUTF8String(label);
	YsBitmap wholeBmp;
	if(YSOK==RenderUnicodeString(wholeBmp,wStr,defFgCol,defBgCol) ||
	   YSOK==RenderAsciiString(wholeBmp,label,defFgCol,defBgCol))
	{
		wLabel=wStr;
		bmp.SetBitmap(wholeBmp);
		CalculateButtonSizeFromMessageSize(wid,hei,bmp.GetWidth(),bmp.GetHeight());
	}
}

void FsGuiButton::SetText(const wchar_t label[])
{
	YsBitmap wholeBmp;
	if(YSOK==RenderUnicodeString(wholeBmp,label,defFgCol,defBgCol))
	{
		wLabel=label;
		bmp.SetBitmap(wholeBmp);
		CalculateButtonSizeFromMessageSize(wid,hei,bmp.GetWidth(),bmp.GetHeight());
	}
}

void FsGuiButton::Click(void)
{
	if(btnType==FSGUI_CHECKBOX)
	{
		YsFlip(checked);
		if(owner!=NULL)
		{
			owner->SetNeedRedraw(YSTRUE);
		}
	}
	else if(btnType==FSGUI_RADIOBUTTON)
	{
		int i;
		forYsArray(i,btnGroup)
		{
			btnGroup[i]->checked=YSFALSE;
		}
		checked=YSTRUE;
		if(owner!=NULL)
		{
			owner->SetNeedRedraw(YSTRUE);
		}
	}

	if(owner!=NULL)
	{
		owner->OnButtonClick(this);
	}
}

void FsGuiButton::LButtonUp(int mx,int my)
{
	mx-=x0;
	my-=y0;

	if(0<=mx && mx<=wid && 0<=my && my<=hei)
	{
		Click();
		if(owner!=NULL)
		{
			owner->SetNeedRedraw(YSTRUE);
		}
	}
}

////////////////////////////////////////////////////////////

FsGuiTextBox::FsGuiTextBox()
{
	itemType=FSGUI_TEXTBOX;
}

void FsGuiTextBox::Initialize(FsGuiDialog *owner)
{
	FsGuiDialogItem::Initialize(owner);

	lngLimit=16;
	txtType=FSGUI_STRING;
	change=YSFALSE;
	selectAllWhenGetFocus=YSTRUE;
	lastCursorState=0;

	txt.Set(NULL);
	txtBmp.CleanUp();

	cursorPos=0;
	selBegin=-1;
	showTop=0;

	cursorPixOffset=0;
	nextCursorPixOffset=0;
	selTopPixOffset=0;
	selEndPixOffset=0;

	eatEnterKey=YSTRUE;
}

void FsGuiTextBox::SetTextType(FSGUITEXTTYPE txtType)
{
	this->txtType=txtType;
}

void FsGuiTextBox::SetText(const char str[])
{
	YsWString wTxt;
	wTxt.SetUTF8String(str);
	SetText(wTxt);
}

void FsGuiTextBox::SetText(const wchar_t str[])
{
	txt.Set(str);
	cursorPos=txt.Strlen();
	selBegin=-1;
	UpdateShowTop();
	UpdatePixOffset();
	RemakeBitmap();
	if(owner!=NULL)
	{
		owner->SetNeedRedraw(YSTRUE);
	}
}

void FsGuiTextBox::SetInteger(int number)
{
	YsString str;
	str.Printf("%d",number);
	txt.SetUTF8String(str);
	cursorPos=txt.Strlen();
	selBegin=-1;
	UpdateShowTop();
	UpdatePixOffset();
	RemakeBitmap();
	if(owner!=NULL)
	{
		owner->SetNeedRedraw(YSTRUE);
	}
}

void FsGuiTextBox::SetRealNumber(const double &number,int decimal)
{
	YsString str;
	char fmt[256];

	sprintf(fmt,"%%.%dlf",decimal);

	str.Printf(fmt,number);

	SetText(str);
}

void FsGuiTextBox::SetLengthLimit(int lngLimit)
{
	this->lngLimit=lngLimit;
}

void FsGuiTextBox::SelectAll(void)
{
	selBegin=0;
	cursorPos=txt.Strlen();

	UpdateShowTop();
}

void FsGuiTextBox::UpdateShowTop(void)
{
	const int nBuffer=2;
	const int nShow=GetShowLength();
	const auto prevShowTop=showTop;

	if(txt.Strlen()<=nShow || lngLimit<=nShow)
	{
		showTop=0;
	}
	else if(cursorPos<=nBuffer)
	{
		showTop=0;
	}
	else if(cursorPos-nBuffer<showTop)
	{
		showTop=cursorPos-nBuffer;
	}
	else if(cursorPos>=showTop+nShow-2)
	{
		showTop=cursorPos-nShow+2;
	}

	if(prevShowTop!=showTop)
	{
		UpdatePixOffset();
		RemakeBitmap();
	}
}

void FsGuiTextBox::SetEatEnterKey(YSBOOL eatEnterKey)
{
	this->eatEnterKey=eatEnterKey;
}

void FsGuiTextBox::SetSelectAllWhenGetFocus(YSBOOL sw)
{
	selectAllWhenGetFocus=sw;
}

YSBOOL FsGuiTextBox::CheckEatEnterKey(void) const
{
	return eatEnterKey;
}

const YsString FsGuiTextBox::GetString(void) const
{
	YsString str;
	str.EncodeUTF8 <wchar_t> (txt);
	return str;
}

const YsWString &FsGuiTextBox::GetWString(void) const
{
	return txt;
}

void FsGuiTextBox::GetText(YsString &str) const
{
	str.EncodeUTF8 <wchar_t> (txt);
}

void FsGuiTextBox::GetText(YsWString &str) const
{
	str=txt;
}

int FsGuiTextBox::GetInteger(void) const
{
	YsString str;
	str.EncodeUTF8 <wchar_t> (txt);
	return atoi(str);
}

double FsGuiTextBox::GetRealNumber(void) const
{
	YsString str;
	str.EncodeUTF8 <wchar_t> (txt);
	return atof(str);
}

int FsGuiTextBox::GetSelectionRange(int &selTop,int &length) const
{
	if(0<=selBegin)
	{
		selTop=(int)YsSmaller(selBegin,cursorPos);
		length=(int)YsAbs(selBegin-cursorPos);

		if(0<=selTop && selTop<txt.Strlen())
		{
			if(txt.Strlen()<selTop+length)
			{
				length=(int)txt.Strlen()-selTop;
			}
			return length;
		}
	}
	selTop=0;
	length=0;
	return 0;
}

void FsGuiTextBox::GetSelectedText(YsString &str) const
{
	YsWString wStr;
	GetSelectedText(wStr);
	str.EncodeUTF8 <wchar_t> (wStr);
}

void FsGuiTextBox::GetSelectedText(YsWString &str) const
{
	int selTop,length;
	if(0<GetSelectionRange(selTop,length))
	{
		str.Set(txt.Txt()+selTop);
		str.SetLength(length);
	}
	else
	{
		str.Set(NULL);
	}
}

YSSIZE_T FsGuiTextBox::GetPreviousWordPos(YSSIZE_T from) const
{
	YSSIZE_T pos=from;
	YSSIZE_T state=0;  // 0:Seaching blank 1:Searching Word 2:Searching Word Top 3:Terminate
	while(pos>0 && 3!=state)
	{
		switch(state)
		{
		case 0:
			if(!(('0'<=txt[pos] && txt[pos]<='9') ||
			     ('a'<=txt[pos] && txt[pos]<='z') ||
			     ('A'<=txt[pos] && txt[pos]<='Z')))
			{
				state=1;
			}
			break;
		case 1:
			if(('0'<=txt[pos] && txt[pos]<='9') ||
			   ('a'<=txt[pos] && txt[pos]<='z') ||
			   ('A'<=txt[pos] && txt[pos]<='Z'))
			{
				state=2;
			}
			// Don't break fall down to case 2.
		case 2:
			if(!(('0'<=txt[pos-1] && txt[pos-1]<='9') ||
			     ('a'<=txt[pos-1] && txt[pos-1]<='z') ||
			     ('A'<=txt[pos-1] && txt[pos-1]<='Z')))
			{
				state=3;
			}
			break;
		}

		if(3!=state)
		{
			pos--;
		}
	}
	return pos;
}

YSSIZE_T FsGuiTextBox::GetNextWordPos(YSSIZE_T from) const
{
	YSSIZE_T pos=from;
	YSSIZE_T state=0;  // 0:Seaching blank 1:Searching Word 2:Searching Word Top 3:Terminate
	while(pos<txt.Strlen() && 2!=state)
	{
		switch(state)
		{
		case 0:
			if(!(('0'<=txt[pos] && txt[pos]<='9') ||
			     ('a'<=txt[pos] && txt[pos]<='z') ||
			     ('A'<=txt[pos] && txt[pos]<='Z')))
			{
				state=1;
			}
			break;
		case 1:
			if(('0'<=txt[pos] && txt[pos]<='9') ||
			   ('a'<=txt[pos] && txt[pos]<='z') ||
			   ('A'<=txt[pos] && txt[pos]<='Z'))
			{
				state=2;
			}
			break;
		}

		if(2!=state)
		{
			pos++;
		}
	}
	return pos;
}

void FsGuiTextBox::GetInputRect(int &x1,int &y1,int &x2,int &y2) const
{
	x1=x0+inputX0;
	y1=y0+inputY0;
	x2=x0+inputX0+inputWid;
	y2=y0+inputY0+inputHei;
}

void FsGuiTextBox::GetTypeRect(int &x1,int &y1,int &x2,int &y2) const
{
	x1=x0+typeX0;
	y1=y0+typeY0;
	x2=x0+typeX0+typeWid;
	y2=y0+typeY0+typeHei;
}

int FsGuiTextBox::GetShowLength(void) const
{
	int x1,y1,x2,y2;
	GetTypeRect(x1,y1,x2,y2);

	const int typeRectWid=x2-x1+1;

	int nShow=0,showWid=0;
	YsBitmap tstBmp;

	int asciiWid,asciiHei;
	GetAsciiCharacterBitmapSize(asciiWid,asciiHei);

	auto i=showTop;
	while(showWid<typeRectWid)
	{
		wchar_t tstStr[2]={'X',0};
		if(i<txt.Strlen())
		{
			tstStr[0]=txt[i];
		}

		if(RenderUnicodeString(tstBmp,tstStr,YsWhite(),YsBlack())==YSOK)
		{
			showWid+=tstBmp.GetWidth();
		}
		else
		{
			showWid+=asciiWid;
		}

		i++;
		nShow++;
	}

	return nShow;
}

YSRESULT FsGuiTextBox::KeyIn(int key,YSBOOL shift,YSBOOL ctrl,YSBOOL alt)
{
	YSBOOL worked=YSFALSE,change=YSFALSE;


	switch(key)
	{
	case FSKEY_X:
		#ifdef __APPLE__
		if(YSTRUE==alt)
		#else
		if(YSTRUE==ctrl)
		#endif
		{
			YsString str;
			GetSelectedText(str);
			FsGuiCopyStringToClipBoard(str);
			DeleteSelected();
			worked=YSTRUE;
		}
		break;
	case FSKEY_C:
		#ifdef __APPLE__
		if(YSTRUE==alt)
		#else
		if(YSTRUE==ctrl)
		#endif
		{
			YsString str;
			GetSelectedText(str);
			FsGuiCopyStringToClipBoard(str);
			selBegin=-1;
			worked=YSTRUE;
		}
		break;
	case FSKEY_V:
		#ifdef __APPLE__
		if(YSTRUE==alt)
		#else
		if(YSTRUE==ctrl)
		#endif
		{
			YsString str;
			FsGuiCopyStringFromClipBoard(str);
			DeleteSelected();

			YsWString wStr;
			wStr.SetUTF8String(str);
			txt.InsertString(cursorPos,wStr);
			if(lngLimit<txt.Strlen())
			{
				txt.SetLength(lngLimit);
			}
			worked=YSTRUE;
		}
		break;

	case FSKEY_A:
		if(YSTRUE==ctrl)
		{
			selBegin=0;
			cursorPos=txt.Strlen();
			worked=YSTRUE;
		}
		break;
	case FSKEY_BS:
		{
			if(YSOK==DeleteSelected())
			{
				worked=YSTRUE;
				change=YSTRUE;
			}
			else if(0<txt.Strlen() && txt.Strlen()<=cursorPos)
			{
				if(txt.Strlen()>0)
				{
					txt.BackSpace();
					cursorPos=txt.Strlen();
					worked=YSTRUE;
					change=YSTRUE;
				}
			}
			else if(0<cursorPos)
			{
				txt.Delete(cursorPos-1);
				cursorPos--;
				worked=YSTRUE;
				change=YSTRUE;
			}

			selBegin=-1;
		}
		break;
	case FSKEY_DEL:
		{
			if(YSOK==DeleteSelected())
			{
				worked=YSTRUE;
				change=YSTRUE;
			}
			else if(0<=cursorPos && cursorPos<txt.Strlen())
			{
				if(YSTRUE!=ctrl)
				{
					txt.Delete(cursorPos);
					worked=YSTRUE;
					change=YSTRUE;
				}
				else
				{
					txt.SetLength(cursorPos);
					worked=YSTRUE;
					change=YSTRUE;
				}
			}

			selBegin=-1;
		}
		break;
	case FSKEY_LEFT:
		if(YSTRUE==shift && 0>selBegin)
		{
			selBegin=cursorPos;
		}
		else if(YSTRUE!=shift)
		{
			selBegin=-1;
		}

		if(YSTRUE!=ctrl)
		{
			if(0<cursorPos)
			{
				cursorPos--;
				worked=YSTRUE;
			}
		}
		else
		{
			cursorPos=GetPreviousWordPos(cursorPos);
			worked=YSTRUE;
		}
		break;
	case FSKEY_RIGHT:
		if(YSTRUE==shift && 0>selBegin)
		{
			selBegin=cursorPos;
		}
		else if(YSTRUE!=shift)
		{
			selBegin=-1;
		}

		if(YSTRUE!=ctrl)
		{
			if(cursorPos<txt.Strlen())
			{
				cursorPos++;
				worked=YSTRUE;
			}
		}
		else
		{
			cursorPos=GetNextWordPos(cursorPos);
			worked=YSTRUE;
		}
		break;
	case FSKEY_HOME:
		if(YSTRUE==shift && 0>selBegin)
		{
			selBegin=cursorPos;
		}
		else if(YSTRUE!=shift)
		{
			selBegin=-1;
		}

		cursorPos=0;
		worked=YSTRUE;
		break;
	case FSKEY_END:
		if(YSTRUE==shift && 0>selBegin)
		{
			selBegin=cursorPos;
		}
		else if(YSTRUE!=shift)
		{
			selBegin=-1;
		}

		cursorPos=txt.Strlen();
		worked=YSTRUE;
		break;
	case FSKEY_ESC:
	case FSKEY_ENTER:
		if(NULL!=owner)
		{
			owner->OnTextBoxSpecialKey(this,key);
		}
		break;
	}

	UpdateShowTop();
	UpdatePixOffset();
	RemakeBitmap();

	if(YSTRUE==worked)
	{
		this->change=change;
		if(owner!=NULL)
		{
			owner->OnTextBoxChange(this);
			owner->SetNeedRedraw(YSTRUE);
		}
		return YSOK;
	}

	// This must eat everything except TAB,ESC, and SPACE, otherwise, key strokes will leak.
	if(YSTRUE!=alt &&
	   FSKEY_TAB!=key &&
	   FSKEY_ESC!=key && 
	   (FSKEY_ENTER!=key || eatEnterKey==YSTRUE))
	{
		return YSOK;
	}

	return YSERR;
}

YSRESULT FsGuiTextBox::CharIn(int c)
{
	YSRESULT res=YSERR;
	switch(txtType)
	{
	case FSGUI_STRING:
		if(isprint(c) || 256<=c)
		{
			DeleteSelected();
			if(txt.Strlen()<lngLimit)
			{
				txt.Insert(cursorPos,(wchar_t)c);
				cursorPos++;
				change=YSTRUE;
				res=YSOK;
			}
		}
		break;
	case FSGUI_ASCIISTRING:
		if(isprint(c))
		{
			DeleteSelected();
			if(txt.Strlen()<lngLimit)
			{
				txt.Insert(cursorPos,(wchar_t)c);
				cursorPos++;
				change=YSTRUE;
				res=YSOK;
			}
		}
		break;
	case FSGUI_INTEGER:
		if(('0'<=c && c<='9') || c=='-')
		{
			DeleteSelected();
			if(txt.Strlen()<lngLimit)
			{
				txt.Insert(cursorPos,(char)c);
				cursorPos++;
				change=YSTRUE;
				res=YSOK;
			}
		}
		break;
	case FSGUI_REALNUMBER:
		if(('0'<=c && c<='9') || c=='-' || c=='.')
		{
			DeleteSelected();
			if(txt.Strlen()<lngLimit)
			{
				txt.Insert(cursorPos,(char)c);
				cursorPos++;
				change=YSTRUE;
				res=YSOK;
			}
		}
		break;
	}

	UpdateShowTop();
	UpdatePixOffset();
	RemakeBitmap();

	if(YSOK==res)
	{
		if(owner!=NULL)
		{
			owner->OnTextBoxChange(this);
			owner->SetNeedRedraw(YSTRUE);
		}
	}

	return res;
}

YSBOOL FsGuiTextBox::CheckAndClearTextChange(void)
{
	YSBOOL ret;
	ret=change;
	change=YSFALSE;
	return ret;
}

YSRESULT FsGuiTextBox::DeleteSelected(void)
{
	int selTop,length;
	if(0<GetSelectionRange(selTop,length))
	{
		txt.DeleteRange(selTop,length);
		selBegin=-1;
		cursorPos=selTop;
		change=YSTRUE;

		UpdateShowTop();
		UpdatePixOffset();
		RemakeBitmap();

		return YSOK;
	}
	else
	{
		selBegin=-1;
		return YSERR;
	}
}

YSBOOL FsGuiTextBox::NeedRedrawActiveItem(void) const
{
	int cursorState;
	int t;
	t=FsGuiClock()/500;

	cursorState=t&1;

	if(lastCursorState!=cursorState)
	{
		lastCursorState=cursorState;
		return YSTRUE;
	}
	return YSFALSE;
}

void FsGuiTextBox::UpdatePixOffset(void)
{
	cursorPixOffset=GetPixOffset(cursorPos);
	nextCursorPixOffset=GetPixOffset(cursorPos+1);

	int selTop,length;
	GetSelectionRange(selTop,length);
	if(0<length)
	{
		selTopPixOffset=GetPixOffset(selTop);
		selEndPixOffset=GetPixOffset(selTop+length);
	}
	else
	{
		selTopPixOffset=-1;
		selEndPixOffset=-1;
	}
}

int FsGuiTextBox::GetPixOffset(YSSIZE_T pos) const
{
	YsWString tstStr;
	const YSSIZE_T relPos=pos-showTop;

	if(0>=relPos)
	{
		return 0;
	}

	tstStr.Set(relPos,txt.Txt()+showTop);
	if(txt.Strlen()<pos) // Extend by 1 letter if cursor width for the end of string is needed.
	{
		const wchar_t x[]={'X',0}; // Workaround to cope with 64-bit GCC bug in Mac OSX
		tstStr.Append(x);
	}
	else
	{
		tstStr.SetLength(relPos);
	}

	int wid,hei;
	if(YSOK==GetTightUnicodeRenderSize(wid,hei,tstStr))
	{
		return wid;
	}
	else
	{
		YsString cTstStr;
		cTstStr.EncodeUTF8 <wchar_t> (tstStr);
		if(YSOK==GetTightAsciiRenderSize(wid,hei,cTstStr))
		{
			return wid;
		}
		else
		{
			int asciiWid,asciiHei;
			GetAsciiCharacterBitmapSize(asciiWid,asciiHei);
			return asciiWid*(int)tstStr.Strlen();
		}
	}
}

void FsGuiTextBox::JustGotFocus(void)
{
	if(YSTRUE==selectAllWhenGetFocus && 0<txt.Strlen())
	{
		selBegin=0;
		cursorPos=txt.Strlen();
		UpdatePixOffset();
	}
	if(txtType==FSGUI_STRING)
	{
		FsEnableIME();
	}
	else
	{
		FsDisableIME();
	}
}

void FsGuiTextBox::RemakeBitmap(void)
{
	txtBmp.CleanUp();
	if(0<=showTop && showTop<txt.Strlen())
	{
		YsBitmap wholeBmp;

		YsWString showStr;
		showStr.Set(txt.Txt()+showTop);
		if(YSOK!=RenderUnicodeString(wholeBmp,showStr,YsWhite(),YsBlack()))
		{
			YsString cShowStr;
			cShowStr.EncodeUTF8 <wchar_t> (showStr);
			if(YSOK!=RenderAsciiString(wholeBmp,cShowStr,YsWhite(),YsBlack()))
			{
			}
		}

		txtBmp.SetBitmap(wholeBmp);
	}
}


////////////////////////////////////////////////////////////


FsGuiListBoxItem::FsGuiListBoxItem()
{
	Initialize();
}

void FsGuiListBoxItem::Initialize(void)
{
	str.Set(NULL);
	bmp.CleanUp();
	flags=0;
	intAttrib=0;
	fgCol=FsGuiObject::defListFgCol;
	bgCol=FsGuiObject::defListBgCol;
}

void FsGuiListBoxItem::Select(void)
{
	if(0==(flags&FSGUI_SELECTED))
	{
		flags|=FSGUI_SELECTED;
		RemakeBitmap();
	}
}

void FsGuiListBoxItem::Unselect(void)
{
	if(0!=(flags&FSGUI_SELECTED))
	{
		flags&=(~FSGUI_SELECTED);
		RemakeBitmap();
	}
}

void FsGuiListBoxItem::SetSelection(YSBOOL sel)
{
	if(sel==YSTRUE)
	{
		Select();
	}
	else
	{
		Unselect();
	}
}

void FsGuiListBoxItem::InvertSelection(void)
{
	if(IsSelected()==YSTRUE)
	{
		Unselect();
	}
	else
	{
		Select();
	}
}

int FsGuiListBoxItem::GetWidth(void) const
{
	return bmp.GetWidth();
}

int FsGuiListBoxItem::GetHeight(void) const
{
	return bmp.GetHeight();
}

void FsGuiListBoxItem::GetString(YsString &str) const
{
	str.EncodeUTF8 <wchar_t> (this->str);
}

void FsGuiListBoxItem::GetString(YsWString &str) const
{
	str=this->str;
}

void FsGuiListBoxItem::SetString(const char str[])
{
	this->str.SetUTF8String(str);
	RemakeBitmap();
}

void FsGuiListBoxItem::SetString(const wchar_t str[])
{
	this->str.Set(str);
	RemakeBitmap();
}

void FsGuiListBoxItem::SetFgColor(const YsColor &fgColorIn)
{
	fgCol=fgColorIn;
	RemakeBitmap();
}

void FsGuiListBoxItem::RemakeBitmap(void)
{
	YsWString wStr;
	if(YSTRUE==IsDirectory())
	{
		const wchar_t dir[]={L"*DIR*"}; // Workaround to cope with 64-bit GCC bug in Mac OSX
		wStr.Set(dir);
		wStr.Append(str);
	}
	else
	{
		wStr=str;
	}

	YsColor fgCol;
	if(YSTRUE==IsSelected())
	{
		fgCol=FsGuiObject::defListActiveFgCol;
	}
	else
	{
		fgCol=this->fgCol;
	}

	bmp.CleanUp();

	YsBitmap wholeBmp;
	if(FsGuiObject::RenderUnicodeString(wholeBmp,wStr,fgCol,YsBlack())!=YSOK)
	{
		YsString cStr;
		cStr.EncodeUTF8 <wchar_t> (wStr);
		if(FsGuiObject::RenderAsciiString(wholeBmp,cStr,fgCol,YsBlack())!=YSOK)
		{
		}
	}
	this->bmp.SetBitmap(wholeBmp);
}

const FsGuiBitmapType &FsGuiListBoxItem::GetBitmap(void) const
{
	return bmp;
}

wchar_t FsGuiListBoxItem::GetFirstLetter(void) const
{
	if(0<str.Strlen())
	{
		return str[0];
	}
	return 0;
}

YSBOOL FsGuiListBoxItem::IsSelected(void) const
{
	if(flags&FSGUI_SELECTED)
	{
		return YSTRUE;
	}
	return YSFALSE;
}

YSBOOL FsGuiListBoxItem::IsDirectory(void) const
{
	if(flags&FSGUI_DIRECTORY)
	{
		return YSTRUE;
	}
	return YSFALSE;
}

FsGuiListBox::FsGuiListBox()
{
	itemType=FSGUI_LISTBOX;
	Initialize(NULL);
}

void FsGuiListBox::Initialize(FsGuiDialog *owner)
{
	FsGuiDialogItem::Initialize(owner);

	showHeightPix=0;
	nShowWidth=8;
	showTopPix=0;
	totalHeightPix=0;
	totalWidthPix=0;

	multi=YSFALSE;
	tight=YSFALSE;
	selectionChange=YSFALSE;
	doubleClick=YSFALSE;
	captureScrollBar=YSFALSE;

	nextRollClk=0;
	lastClickClk=0;
	lastChangeClk=0;

	item.Set(0,NULL);
}

void FsGuiListBox::SetMultiSelect(YSBOOL enableMultiSelect)
{
	this->multi=enableMultiSelect;
}

YSRESULT FsGuiListBox::KeyIn(int key,YSBOOL /*shift*/,YSBOOL /*ctrl*/,YSBOOL /*alt*/)
{
	YSRESULT res=YSERR;
	const int prevShowTopPix=showTopPix;

	if(multi!=YSTRUE)
	{
		YSSIZE_T oldSel,newSel;

		oldSel=-1;
		newSel=-1;
		switch(key)
		{
		case FSKEY_DOWN:
		case FSKEY_WHEELDOWN:
			oldSel=GetSelection();
			newSel=oldSel+1;
			break;
		case FSKEY_UP:
		case FSKEY_WHEELUP:
			oldSel=GetSelection();
			newSel=oldSel-1;
			break;
		case FSKEY_PAGEDOWN:
			oldSel=GetSelection();
			newSel=YsSmaller <YSSIZE_T> (oldSel+GetAverageNShow(),item.GetN()-1);
			break;
		case FSKEY_PAGEUP:
			oldSel=GetSelection();
			newSel=YsGreater <YSSIZE_T> (oldSel-GetAverageNShow(),0);
			break;
		case FSKEY_END:
			oldSel=GetSelection();
			newSel=item.GetN()-1;
			break;
		case FSKEY_HOME:
			oldSel=GetSelection();
			newSel=0;
			break;
		}

		if(oldSel!=newSel && item.IsInRange(newSel)==YSTRUE)
		{
			res=YSOK;
			lastChangeClk=FsGuiClock();
			Select(newSel);

			AdjustShowTopFromNewSelection(newSel);

			if(owner!=NULL)
			{
				owner->OnListBoxSelChange(this,(int)oldSel);
				owner->SetNeedRedraw(YSTRUE);
			}
		}
	}

	if(prevShowTopPix!=showTopPix && owner!=NULL)
	{
		owner->SetNeedRedraw(YSTRUE);
		owner->OnListBoxScroll(this,prevShowTopPix);
		res=YSOK;
	}

	return res;
}

YSRESULT FsGuiListBox::CharIn(int key)
{
	YSRESULT res=YSERR;
	const int prevShowTopPix=showTopPix;

	if(multi!=YSTRUE)
	{
		int newSel=-1;
		int i;

		const int oldSel=GetSelection();

		if('a'<=key && key<='z')
		{
			key=key+('A'-'a');
		}

		for(i=oldSel+1; i<item.GetN(); i++)
		{
			wchar_t c;
			c=item[i].GetFirstLetter();
			if('a'<=c && c<='z')
			{
				c=c+('A'-'a');
			}
			if(c==key)
			{
				newSel=i;
				break;
			}
		}

		if(newSel<0)
		{
			for(i=0; i<oldSel; i++)
			{
				wchar_t c;
				c=item[i].GetFirstLetter();
				if('a'<=c && c<='z')
				{
					c=c+('A'-'a');
				}
				if(c==key)
				{
					newSel=i;
					break;
				}
			}
		}

		if(item.IsInRange(newSel)==YSTRUE)
		{
			res=YSOK;
			lastChangeClk=FsGuiClock();
			Select(newSel);

			AdjustShowTopFromNewSelection(newSel);

			if(owner!=NULL)
			{
				owner->OnListBoxSelChange(this,oldSel);
				owner->SetNeedRedraw(YSTRUE);
			}
		}
	}

	if(prevShowTopPix!=showTopPix && owner!=NULL)
	{
		owner->SetNeedRedraw(YSTRUE);
		owner->OnListBoxScroll(this,prevShowTopPix);
		res=YSOK;
	}

	return res;
}

void FsGuiListBox::LButtonDown(int mx,int my)
{
	int x1,y1,x2,y2;
	if(showHeightPix<totalHeightPix)
	{
		const int prevShowTopPix=showTopPix;

		GetScrollBarRect(x1,y1,x2,y2);
		if(x1<=mx && mx<=x2 && y1<=my && my<=y1+defHScrollBar)
		{
			ScrollUp();
			nextRollClk=FsGuiClock()+400;
			return;
		}
		else if(x1<=mx && mx<=x2 && y2-defHScrollBar<=my && my<=y2)
		{
			ScrollDown();
			nextRollClk=FsGuiClock()+400;
			return;
		}

		GetScrollBarSliderRect(x1,y1,x2,y2);
		if(x1<=mx && mx<=x2 && y1<=my && my<=y2)
		{
			showTopPix=totalHeightPix*(my-y1)/(y2-y1);
			if(showTopPix>totalHeightPix-showHeightPix)
			{
				showTopPix=YsGreater(0,totalHeightPix-showHeightPix);
			}
			captureScrollBar=YSTRUE;
			if(showTopPix!=prevShowTopPix && owner!=NULL)
			{
				owner->OnListBoxScroll(this,prevShowTopPix);
				owner->SetNeedRedraw(YSTRUE);
			}
			return;
		}
	}


	const int s=GetChoiceByCoord(mx,my);
	if(0<=s && s<item.GetN())
	{
		const unsigned int clk=FsGuiClock();
		unsigned int lastClkOffset=0; // For preventing multiple double-clicks with three burst of clicks
		if(multi==YSTRUE)
		{
			YsArray <YSBOOL> prevSel(item.GetN(),NULL);
			for(auto idx : item.AllIndex())
			{
				prevSel[idx]=item[idx].IsSelected();
			}

			if(YSTRUE!=FsGetKeyState(FSKEY_CTRL))
			{
				for(auto &i : item)
				{
					i.Unselect();
				}
			}
			item[s].InvertSelection();
			selectionChange=YSTRUE;
			lastChangeClk=clk;

			if(owner!=NULL)
			{
				for(auto idx : item.AllIndex())
				{
					if(prevSel[idx]!=item[idx].IsSelected())
					{
						owner->OnListBoxSelChange(this,-1);
						owner->SetNeedRedraw(YSTRUE);
						break;
					}
				}
			}
			if(item[s].IsSelected()==YSTRUE && lastClickClk<clk && clk-lastClickClk<1000)
			{
				doubleClick=YSTRUE;
				if(NULL!=owner)
				{
					owner->OnListBoxDoubleClick(this,s);
					owner->SetNeedRedraw(YSTRUE);
					lastClkOffset=10000;
				}
			}
		}
		else
		{
			if(item[s].IsSelected()==YSTRUE && lastClickClk<clk && clk-lastClickClk<1000)
			{
				doubleClick=YSTRUE;
				if(NULL!=owner)
				{
					owner->OnListBoxDoubleClick(this,s);
					owner->SetNeedRedraw(YSTRUE);
					lastClkOffset=10000;
				}
			}
			else if(item[s].IsSelected()!=YSTRUE)
			{
				int i,prevSel;
				prevSel=-1;
				forYsArray(i,item)
				{
					if(item[i].IsSelected()==YSTRUE)
					{
						prevSel=i;
					}
					item[i].Unselect();
				}
				item[s].Select();
				selectionChange=YSTRUE;

				if(owner!=NULL)
				{
					owner->OnListBoxSelChange(this,prevSel);
					owner->SetNeedRedraw(YSTRUE);
				}
			}
		}
		lastClickClk=clk-lastClkOffset;
		lastChangeClk=clk;
	}
}

void FsGuiListBox::LButtonUp(int /*mx*/,int /*my*/)
{
	captureScrollBar=YSFALSE;
}

void FsGuiListBox::SetMouseState(YSBOOL lb,YSBOOL /*mb*/,YSBOOL /*rb*/,int mx,int my)
{
	unsigned int clk;

	clk=FsGuiClock();

	if(lb!=YSTRUE)
	{
		captureScrollBar=YSFALSE;
	}

	if(lb==YSTRUE && showHeightPix<totalHeightPix)
	{
		int x1,y1,x2,y2;

		const int prevShowTopPix=showTopPix;

		GetScrollBarRect(x1,y1,x2,y2);

		if(nextRollClk<FsGuiClock())
		{
			if(x1<=mx && mx<=x2 && y1<=my && my<=y1+defHScrollBar)
			{
				ScrollUp();
				nextRollClk+=100;
			}
			if(x1<=mx && mx<=x2 && y2-defHScrollBar<=my && my<=y2)
			{
				ScrollDown();
				nextRollClk+=100;
			}
		}

		GetScrollBarSliderRect(x1,y1,x2,y2);

		if((captureScrollBar==YSTRUE || (x1<=mx && mx<=x2)) && y1<=my && my<=y2)
		{
			showTopPix=totalHeightPix*(my-y1)/(y2-y1);
			if(showTopPix>totalHeightPix-showHeightPix)
			{
				showTopPix=YsGreater(0,totalHeightPix-showHeightPix);
			}
			if(owner!=NULL)
			{
				owner->OnListBoxScroll(this,prevShowTopPix);
				owner->SetNeedRedraw(YSTRUE);
			}
		}
	}
}

void FsGuiListBox::GetChoiceRect(int &x1,int &y1,int &x2,int &y2) const
{
	if(YSTRUE!=tight)
	{
		x1=x0+defHSpaceUnit;
		y1=y0+defVSpaceUnit/2;
		if(0<msgHei)
		{
			y1+=msgHei+defVSpaceUnit/2;
		}

		x2=x0+wid-defHSpaceUnit;
		y2=y0+hei-defVSpaceUnit/2;
	}
	else
	{
		x1=x0+1;
		y1=y0+1;
		x2=x0+wid-2;
		y2=y0+hei-2;
	}
}

void FsGuiListBox::GetScrollBarRect(int &x1,int &y1,int &x2,int &y2) const
{
	GetChoiceRect(x1,y1,x2,y2);
	x1=x2-defHScrollBar+1;
}

void FsGuiListBox::GetScrollBarSliderRect(int &x1,int &y1,int &x2,int &y2) const
{
	GetScrollBarRect(x1,y1,x2,y2);

	y1+=defHScrollBar+1;
	y2-=defHScrollBar+1;
}

int FsGuiListBox::GetChoiceByCoord(int mx,int my) const
{
	int fontWid,fontHei;
	GetAsciiCharacterBitmapSize(fontWid,fontHei);

	int x1,y1,x2,y2;

	GetChoiceRect(x1,y1,x2,y2);
	if(x1<=mx && mx<=x2 && y1<=my && my<=y2)
	{
		const int yCursor=showTopPix+(my-y1);
		int y=0;
		for(int s=0; s<item.GetN(); s++)
		{
			if(y<=yCursor && yCursor<y+item[s].GetHeight())
			{
				return s;
			}
			y+=item[s].GetHeight();
		}
	}
	return -1;
}

YSBOOL FsGuiListBox::IsScrollBarVisible(void) const
{
	return (showHeightPix<totalHeightPix ? YSTRUE : YSFALSE);
}

void FsGuiListBox::ClearChoice(void)
{
	item.Set(0,NULL);
	showTopPix=0;
	totalHeightPix=1;
}

void FsGuiListBox::SetChoice(YSSIZE_T nStr,const char *const str[])
{
	SetChoiceTemplate <char> (nStr,str);
}

void FsGuiListBox::SetChoice(YSSIZE_T nStr,const wchar_t *const str[])
{
	SetChoiceTemplate <wchar_t> (nStr,str);
}

template <class CHARTYPE>
void FsGuiListBox::SetChoiceTemplate(YSSIZE_T nStr,const CHARTYPE *const str[])
{
	int i;

	item.Set(nStr,NULL);

	totalHeightPix=0;
	totalWidthPix=0;
	for(i=0; i<nStr; i++)
	{
		item[i].Initialize();
		item[i].SetString(str[i]);
		totalWidthPix=YsGreater(totalWidthPix,item[i].GetWidth());
		totalHeightPix+=item[i].GetHeight();
	}

	if(showTopPix>totalHeightPix-showHeightPix)
	{
		showTopPix=YsGreater(0,totalHeightPix-showHeightPix);
	}

	if(owner!=NULL)
	{
		owner->SetNeedRedraw(YSTRUE);
	}
}

YSRESULT FsGuiListBox::SetDirectoryFlag(int id,YSBOOL flag)
{
	if(item.IsInRange(id)==YSTRUE)
	{
		if(flag==YSTRUE)
		{
			item[id].flags|=FSGUI_DIRECTORY;
		}
		else
		{
			item[id].flags&=(~FSGUI_DIRECTORY);
		}

		const int prevItemHeight=item[id].GetHeight();
		item[id].RemakeBitmap();
		totalHeightPix=totalHeightPix-prevItemHeight+item[id].GetHeight();

		return YSOK;
	}
	return YSERR;
}

YSRESULT FsGuiListBox::SetIntAttrib(int id,int intAttrib)
{
	if(item.IsInRange(id)==YSTRUE)
	{
		item[id].intAttrib=intAttrib;
		return YSOK;
	}
	return YSERR;
}

YSRESULT FsGuiListBox::SetStringFgColor(int id,const YsColor &col)
{
	if(YSTRUE==item.IsInRange(id))
	{
		item[id].SetFgColor(col);
		return YSOK;
	}
	return YSERR;
}

void FsGuiListBox::SetChoice(YSSIZE_T nStr,const YsString str[])
{
	YsArray <const char *> strPtr;
	strPtr.Set(nStr,NULL);
	for(int i=0; i<nStr; i++)
	{
		strPtr[i]=str[i].Txt();
	}
	SetChoice(nStr,strPtr);
}

void FsGuiListBox::SetChoice(YSSIZE_T nStr,const YsWString str[])
{
	YsArray <const wchar_t *> strPtr;
	strPtr.Set(nStr,NULL);
	for(int i=0; i<nStr; i++)
	{
		strPtr[i]=str[i].Txt();
	}
	SetChoice(nStr,strPtr);
}

void FsGuiListBox::SetChoice(const YsConstArrayMask <YsString> &choiceArray)
{
	SetChoice(choiceArray.GetN(),choiceArray.GetArray());
}
void FsGuiListBox::SetChoice(const YsConstArrayMask <YsWString> &choiceArray)
{
	SetChoice(choiceArray.GetN(),choiceArray.GetArray());
}

int FsGuiListBox::AddString(const char str[],YSBOOL sel)
{
	auto id=item.GetN();

	item.Increment();
	item.GetEnd().Initialize();
	item.GetEnd().SetString(str);
	totalHeightPix+=item.GetEnd().GetHeight();

	if(sel==YSTRUE)
	{
		if(multi==YSTRUE)
		{
			item[id].Select();
		}
		else
		{
			Select(id);
		}
	}

	if(owner!=NULL)
	{
		owner->SetNeedRedraw(YSTRUE);
	}

	return (int)id;
}

int FsGuiListBox::AddString(const wchar_t str[],YSBOOL sel)
{
	auto id=item.GetN();

	item.Increment();
	item.GetEnd().Initialize();
	item.GetEnd().SetString(str);
	totalHeightPix+=item.GetEnd().GetHeight();

	if(sel==YSTRUE)
	{
		if(multi==YSTRUE)
		{
			item[id].Select();
		}
		else
		{
			Select(id);
		}
	}

	if(owner!=NULL)
	{
		owner->SetNeedRedraw(YSTRUE);
	}

	return (int)id;
}

YSRESULT FsGuiListBox::SetString(int id,const char str[])
{
	if(item.IsInRange(id)==YSTRUE)
	{
		const int prevHei=item[id].GetHeight();

		item[id].SetString(str);

		totalHeightPix=totalHeightPix-prevHei+item[id].GetHeight();

		if(owner!=NULL)
		{
			owner->SetNeedRedraw(YSTRUE);
		}
		return YSOK;
	}
	return YSERR;
}

YSRESULT FsGuiListBox::SetString(int id,const wchar_t str[])
{
	if(item.IsInRange(id)==YSTRUE)
	{
		const int prevHei=item[id].GetHeight();

		item[id].SetString(str);

		totalHeightPix=totalHeightPix-prevHei+item[id].GetHeight();

		if(owner!=NULL)
		{
			owner->SetNeedRedraw(YSTRUE);
		}
		return YSOK;
	}
	return YSERR;
}

YSRESULT FsGuiListBox::DeleteString(int id)
{
	if(item.IsInRange(id)==YSTRUE)
	{
		const int prevHei=item[id].GetHeight();

		item.Delete(id);

		totalHeightPix-=prevHei;

		if(owner!=NULL)
		{
			owner->SetNeedRedraw(YSTRUE);
		}

		return YSOK;
	}
	return YSERR;
}

void FsGuiListBox::Select(YSSIZE_T id)
{
	int i;
	if(item.IsInRange(id)==YSTRUE)
	{
		forYsArray(i,item)
		{
			item[i].Unselect();
		}
		item[id].Select();

		if(owner!=NULL)
		{
			owner->SetNeedRedraw(YSTRUE);
		}
	}
	else if(id<0)
	{
		forYsArray(i,item)
		{
			item[i].Unselect();
		}

		showTopPix=0;

		if(owner!=NULL)
		{
			owner->SetNeedRedraw(YSTRUE);
		}
	}
}

int FsGuiListBox::GetShowTop(void) const
{
	return GetCurrentShowTop();
}
	
void FsGuiListBox::SetShowTop(YSSIZE_T top)
{
	showTopPix=GetVerticalLocationAutoBound(top);

	if(showTopPix<0)
	{
		showTopPix=0;
	}

	if(showTopPix>totalHeightPix-showHeightPix)
	{
		showTopPix=YsGreater(0,totalHeightPix-showHeightPix);
	}
}

void FsGuiListBox::SelectByString(const char str[],YSBOOL selectFirstIfNoMatch)
{
	int i;
	int nSel=0;
	forYsArray(i,item)
	{
		YsString itemStr;
		item[i].GetString(itemStr);
		if(strcmp(str,itemStr)==0)
		{
			showTopPix=GetVerticalLocationAutoBound(i);
			if(showTopPix>totalHeightPix-showHeightPix)
			{
				showTopPix=YsGreater(0,totalHeightPix-showHeightPix);
			}
			item[i].Select();
			nSel++;
		}
		else
		{
			item[i].Unselect();
		}
	}

	if(nSel==0 && selectFirstIfNoMatch==YSTRUE && item.GetN()>0)
	{
		item[0].Select();
	}

	if(owner!=NULL)
	{
		owner->SetNeedRedraw(YSTRUE);
	}
}

int FsGuiListBox::GetSelection(void) const
{
	int i;
	forYsArray(i,item)
	{
		if(item[i].IsSelected()==YSTRUE)
		{
			return i;
		}
	}
	return -1;
}

YSBOOL FsGuiListBox::IsSelected(int id) const
{
	if(YSTRUE==item.IsInRange(id))
	{
		return item[id].IsSelected();
	}
	return YSFALSE;
}

int FsGuiListBox::GetNumChoice(void) const
{
	return (int)item.GetN();
}

YSRESULT FsGuiListBox::GetString(YsString &str,int id) const
{
	if(item.IsInRange(id)==YSTRUE)
	{
		item[id].GetString(str);
		return YSOK;
	}
	return YSERR;
}

YSRESULT FsGuiListBox::GetString(YsWString &str,int id) const
{
	if(item.IsInRange(id)==YSTRUE)
	{
		item[id].GetString(str);
		return YSOK;
	}
	return YSERR;
}

YSRESULT FsGuiListBox::GetSelectedString(YsString &str) const
{
	int i;
	forYsArray(i,item)
	{
		if(item[i].IsSelected()==YSTRUE)
		{
			item[i].GetString(str);
			return YSOK;
		}
	}
	str.Set("");
	return YSERR;
}

YSRESULT FsGuiListBox::GetSelectedString(YsWString &str) const
{
	int i;
	forYsArray(i,item)
	{
		if(item[i].IsSelected()==YSTRUE)
		{
			item[i].GetString(str);
			return YSOK;
		}
	}
	str.Set(NULL);
	return YSERR;
}

YsString FsGuiListBox::GetSelectedString(void) const
{
	YsString s;
	GetSelectedString(s);
	return s;
}
YsWString FsGuiListBox::GetSelectedWString(void) const
{
	YsWString s;
	GetSelectedString(s);
	return s;
}

int FsGuiListBox::GetIntAttrib(int id) const
{
	if(item.IsInRange(id)==YSTRUE)
	{
		return item[id].intAttrib;
	}
	return 0;
}

YSBOOL FsGuiListBox::IsDirectory(int id) const
{
	if(item.IsInRange(id)==YSTRUE)
	{
		if(item[id].flags&FSGUI_DIRECTORY)
		{
			return YSTRUE;
		}
	}
	return YSFALSE;
}

YSBOOL FsGuiListBox::CheckAndClearSelectionChange(void)
{
	YSBOOL ret;
	ret=selectionChange;
	selectionChange=YSFALSE;
	return ret;
}

YSBOOL FsGuiListBox::CheckAndClearDoubleClick(void)
{
	YSBOOL ret;
	ret=doubleClick;
	doubleClick=YSFALSE;
	return ret;
}

unsigned int FsGuiListBox::GetLastChangeClock(void) const
{
	return lastChangeClk;
}

int FsGuiListBox::GetVerticalLocation(YSSIZE_T sel) const
{
	if(YSTRUE==item.IsInRange(sel))
	{
		int y=0;
		for(int i=0; i<sel; i++)
		{
			y+=item[i].GetHeight();
		}
		return y;
	}
	return 0;
}

int FsGuiListBox::GetVerticalLocationAutoBound(YSSIZE_T sel) const
{
	int y=0;
	for(int i=0; i<sel; i++)
	{
		y+=item[i].GetHeight();
	}
	return y;
}

void FsGuiListBox::AdjustShowTopFromNewSelection(YSSIZE_T newSel)
{
	if(YSTRUE==item.IsInRange(newSel))
	{
		const int yUp=GetVerticalLocation(newSel);
		const int yDown=yUp+item[newSel].GetHeight();

		if(showTopPix+showHeightPix<=yUp)
		{
			showTopPix=yDown-showHeightPix;
		}
		else if(yUp<showTopPix)
		{
			showTopPix=yUp;
		}
	}
}

int FsGuiListBox::GetCurrentShowTop(void) const
{
	int y=0;
	for(int i=0; i<item.GetN(); i++)
	{
		if(y<=showTopPix && showTopPix<y+item[i].GetHeight())
		{
			return i;
		}
		y+=item[i].GetHeight();
	}
	return 0;
}

int FsGuiListBox::GetCurrentShowBottom(void) const
{
	int y=0;
	const int showBottomPix=showTopPix+showHeightPix;
	for(int i=0; i<item.GetN(); i++)
	{
		if(y<=showBottomPix && showBottomPix<y+item[i].GetHeight())
		{
			return i;
		}
		y+=item[i].GetHeight();
	}
	return (int)item.GetN()-1;
}

void FsGuiListBox::ScrollUp(void)
{
	if(showTopPix>0)
	{
		const int prevShowTopPix=showTopPix;
		showTopPix=GetVerticalLocationAutoBound(GetCurrentShowTop()-1);
		if(owner!=NULL)
		{
			owner->OnListBoxScroll(this,prevShowTopPix);
			owner->SetNeedRedraw(YSTRUE);
		}
	}
}

void FsGuiListBox::ScrollDown(void)
{
	if(showTopPix<totalHeightPix-showHeightPix)
	{
		const int prevShowTopPix=showTopPix;
		showTopPix=GetVerticalLocationAutoBound(GetCurrentShowTop()+1);
		if(owner!=NULL)
		{
			owner->OnListBoxScroll(this,prevShowTopPix);
			owner->SetNeedRedraw(YSTRUE);
		}
	}
}

int FsGuiListBox::GetAverageNShow(void) const
{
	if(0<totalHeightPix)
	{
		return (int)(item.GetN()*showHeightPix/totalHeightPix);
	}
	return 0;
}

FsGuiDropList::FsGuiDropList()
{
	itemType=FSGUI_DROPLIST;
	Initialize(NULL);
}

void FsGuiDropList::Initialize(FsGuiDialog *owner)
{
	lbx.Initialize(owner);
	lbx.tight=YSTRUE;
	sel=-1;
	bmp.CleanUp();
	popUp=YSFALSE;
	this->owner=owner;
}

void FsGuiDropList::ClearChoice(void)
{
	lbx.ClearChoice();
	bmp.CleanUp();
	sel=-1;
}

void FsGuiDropList::SetChoice(YSSIZE_T nStr,const char *const str[])
{
	lbx.SetChoice(nStr,str);
	RemakeBitmap();
}

void FsGuiDropList::SetChoice(YSSIZE_T nStr,const YsString str[])
{
	lbx.SetChoice(nStr,str);
	RemakeBitmap();
}

void FsGuiDropList::SetChoice(YSSIZE_T nStr,const wchar_t *const str[])
{
	lbx.SetChoice(nStr,str);
	RemakeBitmap();
}
void FsGuiDropList::SetChoice(YSSIZE_T nStr,const YsWString str[])
{
	lbx.SetChoice(nStr,str);
	RemakeBitmap();
}
void FsGuiDropList::SetChoice(const YsConstArrayMask <YsString> &choiceArray)
{
	SetChoice(choiceArray.GetN(),choiceArray.GetArray());
}
void FsGuiDropList::SetChoice(const YsConstArrayMask <YsWString> &choiceArray)
{
	SetChoice(choiceArray.GetN(),choiceArray.GetArray());
}

int FsGuiDropList::AddString(const char str[],YSBOOL select)
{
	int ret=lbx.AddString(str,select);
	if(YSTRUE==select)
	{
		Select(ret);
		RemakeBitmap();
	}
	return ret;
}

int FsGuiDropList::AddString(const wchar_t str[],YSBOOL select)
{
	int ret=lbx.AddString(str,select);
	if(YSTRUE==select)
	{
		Select(ret);
		RemakeBitmap();
	}
	return ret;
}

YSRESULT FsGuiDropList::SetString(int id,const char str[])
{
	YSRESULT res=lbx.SetString(id,str);
	if(YSTRUE==lbx.IsSelected(id))
	{
		RemakeBitmap();
	}
	return res;
}

YSRESULT FsGuiDropList::SetString(int id,const wchar_t str[])
{
	YSRESULT res=lbx.SetString(id,str);
	if(YSTRUE==lbx.IsSelected(id))
	{
		RemakeBitmap();
	}
	return res;
}

YSRESULT FsGuiDropList::DeleteString(int id)
{
	const YSBOOL sel=lbx.IsSelected(id);
	YSRESULT res=lbx.DeleteString(id);
	if(YSTRUE==sel)
	{
		RemakeBitmap();
	}
	return res;
}

YSBOOL FsGuiDropList::IsInRange(int mx,int my) const
{
	if(FsGuiDialogItem::IsInRange(mx,my)==YSTRUE)
	{
		return YSTRUE;
	}

	if(popUp==YSTRUE && lbx.IsInRange(mx,my)==YSTRUE)
	{
		return YSTRUE;
	}

	return YSFALSE;
}

void FsGuiDropList::Select(int id)
{
	lbx.Select(id);
	sel=lbx.GetSelection();
	RemakeBitmap();
}

void FsGuiDropList::SelectByString(const char str[],YSBOOL selectFirstIfNoMatch)
{
	lbx.SelectByString(str,selectFirstIfNoMatch);
	sel=lbx.GetSelection();
	RemakeBitmap();
}

YSRESULT FsGuiDropList::SetIntAttrib(int id,int intAttrib)
{
	return lbx.SetIntAttrib(id,intAttrib);
}

int FsGuiDropList::GetIntAttrib(int id) const
{
	return lbx.GetIntAttrib(id);
}

int FsGuiDropList::GetNumChoice(void) const
{
	return lbx.GetNumChoice();
}


int FsGuiDropList::GetSelection(void) const
{
	return sel;
}

YSRESULT FsGuiDropList::GetSelectedString(YsString &str) const
{
	if(lbx.item.IsInRange(sel)==YSTRUE)
	{
		lbx.item[sel].GetString(str);
		return YSOK;
	}
	str.Set(NULL);
	return YSERR;
}

YSRESULT FsGuiDropList::GetSelectedString(YsWString &str) const
{
	if(YSTRUE==lbx.item.IsInRange(sel))
	{
		lbx.item[sel].GetString(str);
		return YSOK;
	}
	str.Set(NULL);
	return YSERR;
}

YsString FsGuiDropList::GetSelectedString(void) const
{
	YsString str;
	GetSelectedString(str);
	return str;
}

YsWString FsGuiDropList::GetSelectedWString(void) const
{
	YsWString str;
	GetSelectedString(str);
	return str;
}

YSRESULT FsGuiDropList::GetString(YsString &str,int idx) const
{
	if(lbx.item.IsInRange(idx)==YSTRUE)
	{
		lbx.item[idx].GetString(str);
		return YSOK;
	}
	str.Set(NULL);
	return YSERR;
}

YSRESULT FsGuiDropList::GetString(YsWString &str,int idx) const
{
	if(lbx.item.IsInRange(idx)==YSTRUE)
	{
		lbx.item[idx].GetString(str);
		return YSOK;
	}
	str.Set(NULL);
	return YSERR;
}

YSBOOL FsGuiDropList::CheckAndClearSelectionChange(void)
{
	YSBOOL ret;
	ret=selectionChange;
	selectionChange=YSFALSE;
	return ret;
}

void FsGuiDropList::LButtonDown(int mx,int my)
{
	if(popUp==YSTRUE)
	{
		int x0,y0,x1,y1;
		x0=lbx.x0;
		y0=lbx.y0;
		x1=lbx.x0+lbx.wid;
		y1=lbx.y0+lbx.hei;
		if(x0<=mx && mx<x1 && y0<=my && my<y1)
		{
			lbx.LButtonDown(mx,my);
		}
		else
		{
			owner->SetPopUp(NULL);
			popUp=YSFALSE;
			return;
		}
	}

	if(x0<=mx && mx<x0+wid && y0<=my && my<y0+hei)
	{
		if(popUp!=YSTRUE)
		{
			lbx.x0=x0;
			lbx.y0=y0+hei;
			owner->SetPopUp(this);
			popUp=YSTRUE;
		}
		else
		{
			owner->SetPopUp(NULL);
			popUp=YSFALSE;
		}
	}
}

void FsGuiDropList::LButtonUp(int mx,int my)
{
	if(popUp==YSTRUE)
	{
		int x0,y0,x1,y1;

		if(lbx.IsScrollBarVisible()==YSTRUE)
		{
			if(YSTRUE==lbx.captureScrollBar)  // If the user was scrolling.
			{
				return;
			}

			lbx.GetScrollBarRect(x0,y0,x1,y1);
			if(x0<=mx && mx<x1 && y0<=my && my<y1)
			{
				return;
			}
		}

		lbx.GetChoiceRect(x0,y0,x1,y1);
		if(x0<=mx && mx<x1 && y0<=my && my<y1)
		{
			int prevSel,sel;
			sel=lbx.GetChoiceByCoord(mx,my);
			if(lbx.item.IsInRange(sel)==YSTRUE && this->sel!=sel)
			{
				prevSel=this->sel;
				this->sel=sel;
				selectionChange=YSTRUE;
				RemakeBitmap();
				owner->OnDropListSelChange(this,prevSel);
			}

			owner->SetPopUp(NULL);
			popUp=YSFALSE;
		}
	}
}

YSRESULT FsGuiDropList::KeyIn(int key,YSBOOL shift,YSBOOL ctrl,YSBOOL alt)
{
	YSRESULT res;
	res=YSERR;

	if(key==FSKEY_ESC)
	{
		if(popUp==YSTRUE)
		{
			owner->SetPopUp(NULL);
			popUp=YSFALSE;
			res=YSOK;
		}
	}
	else if(key==FSKEY_ENTER)
	{
		if(popUp!=YSTRUE)
		{
			lbx.x0=x0;
			lbx.y0=y0+hei;
			owner->SetPopUp(this);
			popUp=YSTRUE;
			res=YSOK;
		}
		else
		{
			owner->SetPopUp(NULL);
			popUp=YSFALSE;
			res=YSOK;
		}
	}
	else
	{
		int prevSel;

		prevSel=lbx.GetSelection();
		res=lbx.KeyIn(key,shift,ctrl,alt);

		if(prevSel!=lbx.GetSelection())
		{
			sel=lbx.GetSelection();
			owner->SetNeedRedraw(YSTRUE);
			selectionChange=YSTRUE;
			RemakeBitmap();
			owner->OnDropListSelChange(this,prevSel);
			res=YSOK;
		}
	}

	return res;
}

YSRESULT FsGuiDropList::CharIn(int key)
{
	int prevSel;
	YSRESULT res;

	prevSel=lbx.GetSelection();
	res=lbx.CharIn(key);

	if(prevSel!=lbx.GetSelection())
	{
		sel=lbx.GetSelection();
		owner->SetNeedRedraw(YSTRUE);
		selectionChange=YSTRUE;
		RemakeBitmap();
		owner->OnDropListSelChange(this,prevSel);
	}

	return res;
}

void FsGuiDropList::SetMouseState(YSBOOL lb,YSBOOL mb,YSBOOL rb,int mx,int my)
{
	if(popUp==YSTRUE)
	{
		lbx.SetMouseState(lb,mb,rb,mx,my);

		if(lb==YSTRUE && lbx.captureScrollBar!=YSTRUE)
		{
			int x0,y0,x1,y1;
			lbx.GetScrollBarRect(x0,y0,x1,y1);
			if(mx<x0 || x1<mx ||  my<y0 ||  y1<my)
			{
				lbx.GetChoiceRect(x0,y0,x1,y1);
				if(x0<=mx && mx<x1 && y0<=my && my<y1)
				{
					int sel;
					sel=lbx.GetChoiceByCoord(mx,my);
					if(lbx.item.IsInRange(sel)==YSTRUE && lbx.GetSelection()!=sel)
					{
						lbx.Select(sel);
						RemakeBitmap();
						owner->SetNeedRedraw(YSTRUE);
					}
					else if(lbx.item.IsInRange(sel)!=YSTRUE && lbx.GetSelection()>=0)
					{
						lbx.Select(-1);
						RemakeBitmap();
						owner->SetNeedRedraw(YSTRUE);
					}
				}
			}
		}
	}
}

void FsGuiDropList::RemakeBitmap(void)
{
	YsWString wStr;
	YsString str;
	YsBitmap wholeBmp;
	if((YSOK==lbx.GetSelectedString(wStr) && YSOK==RenderUnicodeString(wholeBmp,wStr,defFgCol,defBgCol)) ||
	   (YSOK==lbx.GetSelectedString(str) && YSOK==RenderAsciiString(wholeBmp,str,defFgCol,defBgCol)))
	{
		bmp.SetBitmap(wholeBmp);
	}
	else
	{
		bmp.CleanUp();
	}
}

FsGuiNumberBox::FsGuiNumberBox()
{
	itemType=FSGUI_NUMBERBOX;
	Initialize(NULL);
}

void FsGuiNumberBox::Initialize(FsGuiDialog *owner)
{
	FsGuiDialogItem::Initialize(owner);
	num=0;
	min=0;
	max=0;
	step=1;
	nextRollClk=0;
	increased=YSFALSE;
	decreased=YSFALSE;

	mouseOnIncreaseButton=YSFALSE;
	mouseOnDecreaseButton=YSFALSE;

	wLabel.Set(NULL);
	labelWidth=0;
}

void FsGuiNumberBox::SetNumberAndRange(int num,int min,int max,int step)
{
	this->num=num;
	this->min=min;
	this->max=max;
	this->step=step;

	if(owner!=NULL)
	{
		owner->SetNeedRedraw(YSTRUE);
	}
}

void FsGuiNumberBox::SetNumber(int num)
{
	this->num=YsBound(num,min,max);

	if(owner!=NULL)
	{
		owner->SetNeedRedraw(YSTRUE);
	}
}

int FsGuiNumberBox::GetNumber(void) const
{
	return num;
}

void FsGuiNumberBox::GetIncreaseButtonArea(int &x0,int &y0,int &x1,int &y1) const
{
	const int arrowSize=YsGreater(msgHei,digitHei);
	x0=this->x0+this->wid-defHSpaceUnit-arrowSize*2;
	y0=this->y0+this->hei/2-arrowSize/2;
	x1=x0+arrowSize;
	y1=y0+arrowSize;
}

void FsGuiNumberBox::GetDecreaseButtonArea(int &x0,int &y0,int &x1,int &y1) const
{
	const int arrowSize=YsGreater(msgHei,digitHei);
	x0=this->x0+this->wid-defHSpaceUnit-arrowSize;
	y0=this->y0+this->hei/2-arrowSize/2;
	x1=x0+arrowSize;
	y1=y0+arrowSize;
}

YSBOOL FsGuiNumberBox::CheckAndClearNumberChange(void)
{
	if(increased==YSTRUE || decreased==YSTRUE)
	{
		increased=YSFALSE;
		decreased=YSFALSE;
		return YSTRUE;
	}
	return YSFALSE;
}

YSRESULT FsGuiNumberBox::KeyIn(int key,YSBOOL /*shift*/,YSBOOL /*ctrl*/,YSBOOL /*alt*/)
{
	int prevNum;
	prevNum=num;
	switch(key)
	{
	case FSKEY_UP:
	case FSKEY_WHEELUP:
		num=YsSmaller(num+step,max);
		break;
	case FSKEY_DOWN:
	case FSKEY_WHEELDOWN:
		num=YsGreater(num-step,min);
		break;
	}

	if(owner!=NULL && prevNum!=num)
	{
		owner->OnNumberBoxChange(this,prevNum);
		owner->SetNeedRedraw(YSTRUE);
		return YSOK;
	}
	return YSERR;
}

YSRESULT FsGuiNumberBox::CharIn(int)
{
	return YSERR;
}

void FsGuiNumberBox::LButtonDown(int mx,int my)
{
	int bx0,by0,bx1,by1;
	int prevNum;
	prevNum=num;
	GetIncreaseButtonArea(bx0,by0,bx1,by1);
	if(bx0<=mx && mx<=bx1 && by0<=my && my<=by1)
	{
		mouseOnIncreaseButton=YSTRUE;
		num+=step;
		increased=YSTRUE;
		if(num>max)
		{
			num=max;
		}
		nextRollClk=FsGuiClock()+400;
	}

	GetDecreaseButtonArea(bx0,by0,bx1,by1);
	if(bx0<=mx && mx<=bx1 && by0<=my && my<=by1)
	{
		mouseOnDecreaseButton=YSTRUE;
		num-=step;
		decreased=YSTRUE;
		if(num<min)
		{
			num=min;
		}
		nextRollClk=FsGuiClock()+400;
	}

	if(owner!=NULL && prevNum!=num)
	{
		owner->OnNumberBoxChange(this,prevNum);
		owner->SetNeedRedraw(YSTRUE);
	}
}

void FsGuiNumberBox::SetMouseState(YSBOOL lb,YSBOOL /*mb*/,YSBOOL /*rb*/,int mx,int my)
{
	mouseOnIncreaseButton=YSFALSE;
	mouseOnDecreaseButton=YSFALSE;

	int prevNum;
	prevNum=num;

	int bx0,by0,bx1,by1;
	GetIncreaseButtonArea(bx0,by0,bx1,by1);
	if(bx0<=mx && mx<=bx1 && by0<=my && my<=by1)
	{
		mouseOnIncreaseButton=YSTRUE;
		if(lb==YSTRUE && nextRollClk<FsGuiClock())
		{
			num+=step;
			increased=YSTRUE;
			if(num>max)
			{
				num=max;
			}
			nextRollClk=FsGuiClock()+100;
		}
	}

	GetDecreaseButtonArea(bx0,by0,bx1,by1);
	if(bx0<=mx && mx<=bx1 && by0<=my && my<=by1)
	{
		mouseOnDecreaseButton=YSTRUE;
		if(lb==YSTRUE && nextRollClk<FsGuiClock())
		{
			num-=step;
			decreased=YSTRUE;
			if(num<min)
			{
				num=min;
			}
			nextRollClk=FsGuiClock()+100;
		}
	}

	if(owner!=NULL && prevNum!=num)
	{
		owner->OnNumberBoxChange(this,prevNum);
		owner->SetNeedRedraw(YSTRUE);
	}
}



//	int r,g,b;
//	unsigned int nextRollClk;
//	int cursor;

FsGuiColorPalette::FsGuiColorPalette()
{
	itemType=FSGUI_COLORPALETTE;
	colDlg=NULL;
	Initialize(NULL);
}

FsGuiColorPalette::~FsGuiColorPalette()
{
	if(NULL!=colDlg)
	{
		FsGuiColorDialog::Delete(colDlg);
		colDlg=NULL;
	}
}

void FsGuiColorPalette::Initialize(FsGuiDialog *owner)
{
	int i;

	this->owner=owner;

	col[0]=255;
	col[1]=255;
	col[2]=255;

	for(i=0; i<16; i++)
	{
		int r,g,b,inten;
		b=(i&1);
		r=(i>>1)&1;
		g=(i>>2)&1;

		inten=255-(i>>3)*128;

		palette[i][0]=inten*r;
		palette[i][1]=inten*g;
		palette[i][2]=inten*b;
	}

	step=1;
	cursor=0;
	nextRollClk=0;

	arrow[0][0]=YSFALSE;
	arrow[0][1]=YSFALSE;
	arrow[1][0]=YSFALSE;
	arrow[1][1]=YSFALSE;
	arrow[2][0]=YSFALSE;
	arrow[2][1]=YSFALSE;

	hasColorButton=YSTRUE;
	hasComponentButton=YSTRUE;

	if(NULL!=colDlg)
	{
		FsGuiColorDialog::Delete(colDlg);
		colDlg=NULL;
	}
}

void FsGuiColorPalette::SetColor(const YsColor &col)
{
	this->col[0]=col.Ri();
	this->col[1]=col.Gi();
	this->col[2]=col.Bi();
}

void FsGuiColorPalette::SetPalette(int id,const YsColor &col)
{
	if(0<=id && id<16)
	{
		palette[id][0]=col.Ri();
		palette[id][1]=col.Gi();
		palette[id][2]=col.Bi();
	}
}

void FsGuiColorPalette::InvokeOnColorPaletteChange(void)
{
	if(NULL!=owner)
	{
		owner->SetNeedRedraw(YSTRUE);
		owner->OnColorPaletteChange(this);
	}
}

const YsColor FsGuiColorPalette::GetColor(void) const
{
	YsColor col;
	col.SetIntRGB(this->col[0],this->col[1],this->col[2]);
	return col;
}

const YsColor &FsGuiColorPalette::GetColor(YsColor &col) const
{
	col.SetIntRGB(this->col[0],this->col[1],this->col[2]);
	return col;
}

const YsColor &FsGuiColorPalette::GetPalette(YsColor &col,int id) const
{
	if(0<=id && id<16)
	{
		col.SetIntRGB(palette[id][0],palette[id][1],palette[id][2]);
	}
	else
	{
		col=YsWhite();
	}
	return col;
}

YSBOOL FsGuiColorPalette::HasColorButton(void) const
{
	return hasColorButton;
}

void FsGuiColorPalette::GetRedRect(int &x0,int &y0,int &x1,int &y1) const
{
	int fontWid,fontHei;
	GetAsciiCharacterBitmapSize(fontWid,fontHei);

	x0=this->x0+defHSpaceUnit;
	y0=this->y0+defVSpaceUnit/2;

	y0+=(0<bmp.GetHeight() ? bmp.GetHeight()+defVSpaceUnit/2 : 0);

	x1=x0+fontWid*5+defHAnnotation;   // "R:255"
	y1=y0+defHAnnotation*2;
}

void FsGuiColorPalette::GetGreenRect(int &x0,int &y0,int &x1,int &y1) const
{
	int fontWid,fontHei;
	GetAsciiCharacterBitmapSize(fontWid,fontHei);

	x0=this->x0+fontWid*5+defHAnnotation+defHSpaceUnit+defHSpaceUnit;
	y0=this->y0+defVSpaceUnit/2;

	y0+=(0<bmp.GetHeight() ? bmp.GetHeight()+defVSpaceUnit/2 : 0);

	x1=x0+fontWid*5+defHAnnotation;   // "R:255"
	y1=y0+defHAnnotation*2;
}

void FsGuiColorPalette::GetBlueRect(int &x0,int &y0,int &x1,int &y1) const
{
	int fontWid,fontHei;
	GetAsciiCharacterBitmapSize(fontWid,fontHei);

	x0=this->x0+(fontWid*5+defHAnnotation+defHSpaceUnit)*2+defHSpaceUnit;
	y0=this->y0+defVSpaceUnit/2;

	y0+=(0<bmp.GetHeight() ? bmp.GetHeight()+defVSpaceUnit/2 : 0);

	x1=x0+fontWid*5+defHAnnotation;   // "R:255"
	y1=y0+defHAnnotation*2;
}

void FsGuiColorPalette::GetComponentRect(int &x0,int &y0,int &x1,int &y1,int c)const 
{
	if(YSTRUE==hasComponentButton)
	{
		switch(c)
		{
		case 0:
			GetRedRect(x0,y0,x1,y1);
			break;
		case 1:
			GetGreenRect(x0,y0,x1,y1);
			break;
		case 2:
			GetBlueRect(x0,y0,x1,y1);
			break;
		}
	}
	else
	{
		x0=this->x0;
		y0=this->y0;
		x1=this->x0;
		y1=this->y0;
	}
}

void FsGuiColorPalette::GetPaletteRect(int &x0,int &y0,int &x1,int &y1)const
{
	int fontWid,fontHei;
	GetAsciiCharacterBitmapSize(fontWid,fontHei);

	x0=this->x0+defHSpaceUnit;
	y0=this->y0+defVSpaceUnit/2;

	y0+=(0<bmp.GetHeight() ? bmp.GetHeight()+defVSpaceUnit/2 : 0);
	y0+=defHAnnotation*2+defVSpaceUnit/2;

	x1=this->x0+this->wid-defHSpaceUnit;
	y1=y0+defHAnnotation*2;
}

void FsGuiColorPalette::GetColorRect(int &x0,int &y0,int &x1,int &y1) const
{
	if(YSTRUE!=hasComponentButton && 0==bmp.GetHeight())
	{
		x0=this->x0+6;
		y0=this->y0+6;
		x1=this->x0+wid-7;
		y1=this->y0+hei-7;
		return;
	}

	int fontWid,fontHei;
	GetAsciiCharacterBitmapSize(fontWid,fontHei);

	x0=this->x0+defHSpaceUnit;
	if(YSTRUE==hasComponentButton)
	{
		x0+=(fontWid*5+defHAnnotation+defHSpaceUnit)*3;
	}
	y0=this->y0+defVSpaceUnit/2;

	y0+=(0<bmp.GetHeight() ? bmp.GetHeight()+defVSpaceUnit/2 : 0);

	x1=x0+fontWid*3;
	y1=y0+defHAnnotation*2;
}

YSRESULT FsGuiColorPalette::LoadFile(const char fn[])
{
	FILE *fp=fopen(fn,"r");
	if(fp!=NULL)
	{
		YsTextFileInputStream inStream(fp);
		LoadFile(inStream);
		fclose(fp);
		return YSOK;
	}
	return YSERR;
}

YSRESULT FsGuiColorPalette::SaveFile(const char fn[]) const
{
	FILE *fp=fopen(fn,"w");
	if(NULL!=fp)
	{
		YsTextFileOutputStream outStream(fp);
		SaveFile(outStream);
		fclose(fp);
		return YSOK;
	}
	return YSERR;
}

YSRESULT FsGuiColorPalette::LoadFile(YsTextInputStream &inStream)
{
	YsString str;
	YsArray <YsString,16> args;
	while(NULL!=(inStream.Gets(str)))
	{
		if(str.Strlen()>0 && str.Arguments(args)==YSOK && args.GetN()>0)
		{
			if(args[0][0]=='S' && args.GetN()>=4)
			{
				col[0]=atoi(args[1]);
				col[1]=atoi(args[2]);
				col[2]=atoi(args[3]);
			}
			else if(args[0][0]=='C' && args.GetN()>=4)
			{
				int i=atoi((const char *)args[0]+1);
				if(0<=i && i<16)
				{
					palette[i][0]=atoi(args[1]);
					palette[i][1]=atoi(args[2]);
					palette[i][2]=atoi(args[3]);
				}
			}
		}
	}
	return YSOK;
}

YSRESULT FsGuiColorPalette::SaveFile(YsTextOutputStream &outStream) const
{
	outStream.Printf("S %d %d %d\n",col[0],col[1],col[2]);
	for(int i=0; i<NUM_PALETTE; i++)
	{
		outStream.Printf("C%02d %d %d %d\n",i,palette[i][0],palette[i][1],palette[i][2]);
	}
	return YSOK;
}

YSRESULT FsGuiColorPalette::KeyIn(int key,YSBOOL /*shift*/,YSBOOL /*ctrl*/,YSBOOL /*alt*/)
{
	YSRESULT res;

	res=YSERR;
	switch(key)
	{
	case FSKEY_LEFT:
		cursor=(cursor+2)%3;
		res=YSOK;
		if(owner!=NULL)
		{
			owner->SetNeedRedraw(YSTRUE);
		}
		break;
	case FSKEY_RIGHT:
		cursor=(cursor+1)%3;
		res=YSOK;
		if(owner!=NULL)
		{
			owner->SetNeedRedraw(YSTRUE);
		}
		break;
	case FSKEY_UP:
		switch(cursor)
		{
		case 0:
		case 1:
		case 2:
			col[cursor]=YsSmaller(col[cursor]+step,255);
			if(owner!=NULL)
			{
				owner->SetNeedRedraw(YSTRUE);
				owner->OnColorPaletteChange(this);
			}
			res=YSOK;
			break;
		}
		break;
	case FSKEY_DOWN:
		switch(cursor)
		{
		case 0:
		case 1:
		case 2:
			col[cursor]=YsGreater(col[cursor]-step,0);
			if(owner!=NULL)
			{
				owner->SetNeedRedraw(YSTRUE);
				owner->OnColorPaletteChange(this);
			}
			res=YSOK;
			break;
		}
		break;
	}

	return res;
}

void FsGuiColorPalette::LButtonDown(int mx,int my)
{
	int i;
	int x0,y0,x1,y1;
	YSBOOL up,down;

	if(YSTRUE==hasComponentButton)
	{
		for(i=0; i<3; i++)
		{
			GetComponentRect(x0,y0,x1,y1,i);
			if(IsInRightSideArrow(up,down,mx,my,x0,y0,x1,y1)==YSTRUE)
			{
				cursor=i;
				nextRollClk=FsGuiClock()+400;
				if(up==YSTRUE)
				{
					col[i]=YsSmaller(col[i]+step,255);
					if(owner!=NULL)
					{
						owner->SetNeedRedraw(YSTRUE);
						owner->OnColorPaletteChange(this);
					}
				}
				else if(down==YSTRUE)
				{
					col[i]=YsGreater(col[i]-step,0);
					if(owner!=NULL)
					{
						owner->SetNeedRedraw(YSTRUE);
						owner->OnColorPaletteChange(this);
					}
				}
			}
		}
	}

	if(YSTRUE==hasColorButton)
	{
		GetPaletteRect(x0,y0,x1,y1);
		if(x0<=mx && mx<x1 && y0<=my && my<y1)
		{
			if(x1!=x0 && y1!=y0)  // Previous condition guarantees this condition, but I want to make absoluately sure that I don't get division by zero error here.
			{
				int x,y,i;
				x=(mx-x0)*8/(x1-x0);
				y=(my-y0)*2/(y1-y0);
				i=y*8+x;
				if(0<=i && i<NUM_PALETTE)
				{
					col[0]=palette[i][0];
					col[1]=palette[i][1];
					col[2]=palette[i][2];
					if(owner!=NULL)
					{
						owner->SetNeedRedraw(YSTRUE);
						owner->OnColorPaletteChange(this);
					}
				}
			}
		}
	}
}

/* virtual */ void FsGuiColorPalette::LButtonUp(int mx,int my)
{
	int x0,y0,x1,y1;

	if(YSTRUE==hasComponentButton || YSTRUE==hasColorButton)
	{
		GetColorRect(x0,y0,x1,y1);
	}
	else
	{
		x0=this->x0;
		y0=this->y0;
		x1=this->x0+this->wid;
		y1=this->y0+this->hei;
	}
	if(x0<=mx && mx<x1 && y0<=my && my<y1 && NULL!=owner)
	{
		if(NULL==colDlg)
		{
			colDlg=FsGuiColorDialog::Create();
		}
		colDlg->Make(this);
		colDlg->SetCurrentColor(GetColor());
		owner->AttachModalDialog(colDlg);
		owner->SetNeedRedraw(YSTRUE);
		return;
	}
}

void FsGuiColorPalette::SetMouseState(YSBOOL lb,YSBOOL /*mb*/,YSBOOL /*rb*/,int mx,int my)
{
	if(YSTRUE==hasComponentButton)
	{
		for(int i=0; i<3; i++)
		{
			int x0,y0,x1,y1;
			GetComponentRect(x0,y0,x1,y1,i);
			if(IsInRightSideArrow(arrow[i][0],arrow[i][1],mx,my,x0,y0,x1,y1)==YSTRUE)
			{
				if(lb==YSTRUE && nextRollClk<FsGuiClock())
				{
					if(arrow[i][0]==YSTRUE)
					{
						col[i]=YsSmaller(col[i]+step,255);
						if(owner!=NULL)
						{
							owner->SetNeedRedraw(YSTRUE);
							owner->OnColorPaletteChange(this);
						}
					}
					else if(arrow[i][1]==YSTRUE)
					{
						col[i]=YsGreater(col[i]-step,0);
						if(owner!=NULL)
						{
							owner->SetNeedRedraw(YSTRUE);
							owner->OnColorPaletteChange(this);
						}
					}
					nextRollClk=FsGuiClock()+100;
				}
			}
		}
	}
}

//	double min,max;
//	double position; // 0.0 to 1.0
//	double step;
//
//	YSBOOL dragging;
//	int dragMouseOffsetX,dragMouseOffsetY;


FsGuiSlider::FsGuiSlider()
{
	itemType=FSGUI_SLIDER;
	Initialize(NULL);
}

void FsGuiSlider::Initialize(FsGuiDialog *owner)
{
	this->owner=owner;

	min=0.0;
	max=1.0;
	position=0.0;
	step=0.05;

	cursorSize=6;

	horizontal=YSTRUE;

	dragging=YSFALSE;
	dragMouseOffsetX=0;
	dragMouseOffsetY=0;
}

void FsGuiSlider::SetHorizontal(YSBOOL hor)
{
	horizontal=hor;
}

const double &FsGuiSlider::GetPosition(void) const  // 0.0 to 1.0
{
	return position;
}

const double FsGuiSlider::GetScaledValue(void) const // min to max
{
	return min*(1.0-position)+max*position;
}

void FsGuiSlider::GetSliderRect(int &x0,int &y0,int &x1,int &y1)const
{
	x0=this->x0+2;
	y0=this->y0+2;
	x1=this->x0+wid-3;
	y1=this->y0+hei-3;
}

int FsGuiSlider::GetCursorSize(void) const
{
	return cursorSize;
}

void FsGuiSlider::GetCursorMovingRange(int &min,int &max) const
{
	int sx0,sy0,sx1,sy1;

	GetSliderRect(sx0,sy0,sx1,sy1);

	if(YSTRUE==horizontal)
	{
		min=sx0+cursorSize/2;
		max=sx1-cursorSize/2;
	}
	else
	{
		max=sy0+cursorSize/2;
		min=sy1-cursorSize/2;
	}
}

void FsGuiSlider::GetCursorCenter(int &x,int &y) const
{
	int min,max,sx0,sy0,sx1,sy1;

	GetCursorMovingRange(min,max);
	GetSliderRect(sx0,sy0,sx1,sy1);
	if(YSTRUE==horizontal)
	{
		double xx;
		xx=(double)min*(1.0-position)+(double)max*position;
		x=(int)xx;
		y=(sy0+sy1)/2;
	}
	else
	{
		double yy;
		yy=(double)min*(1.0-position)+(double)max*position;
		y=(int)yy;
		x=(sx0+sx1)/2;
	}
}

void FsGuiSlider::GetCursorRect(int &x0,int &y0,int &x1,int &y1) const
{
	int sx0,sy0,sx1,sy1;
	int cx,cy;
	GetSliderRect(sx0,sy0,sx1,sy1);
	GetCursorCenter(cx,cy);
	if(YSTRUE==horizontal)
	{
		x0=cx-cursorSize/2;
		y0=sy0;
		x1=cx+cursorSize/2;
		y1=sy1;
	}
	else
	{
		x0=sx0;
		y0=cy-cursorSize/2;
		x1=sx1;
		y1=cy+cursorSize/2;
	}
}

double FsGuiSlider::CalculatePositionFromScreenCoord(int x,int y) const
{
	int min,max;
	GetCursorMovingRange(min,max);
	if(YSTRUE==horizontal)
	{
		return (double)(x-min)/(double)(max-min);
	}
	else
	{
		return (double)(y-min)/(double)(max-min);
	}
}

void FsGuiSlider::SetMinMax(const double &min,const double &max)
{
	this->min=min;
	this->max=max;
}

void FsGuiSlider::SetPosition(const double &pos)
{
	position=YsBound(pos,0.0,1.0);
}

void FsGuiSlider::SetPositionByScaledValue(const double value)
{
	SetPosition((value-min)/(max-min));
}

void FsGuiSlider::Increment(void)
{
	if(1.0>position)
	{
		double prevPos,prevValue;
		prevPos=GetPosition();
		prevValue=GetScaledValue();

		position+=step;
		if(position>1.0)
		{
			position=1.0;
		}
		if(owner!=NULL)
		{
			owner->SetNeedRedraw(YSTRUE);
			owner->OnSliderPositionChange(this,prevPos,prevValue);
		}
	}
}

void FsGuiSlider::Decrement(void)
{
	if(0.0<position)
	{
		double prevPos,prevValue;
		prevPos=GetPosition();
		prevValue=GetScaledValue();

		position-=step;
		if(position<0.0)
		{
			position=0.0;
		}
		if(owner!=NULL)
		{
			owner->SetNeedRedraw(YSTRUE);
			owner->OnSliderPositionChange(this,prevPos,prevValue);
		}
	}
}

void FsGuiSlider::SetStep(const double &step)
{
	this->step=step;
}

YSRESULT FsGuiSlider::KeyIn(int key,YSBOOL /*shift*/,YSBOOL /*ctrl*/,YSBOOL /*alt*/)
{
	switch(key)
	{
	case FSKEY_LEFT:
		if(YSTRUE==horizontal)
		{
			Decrement();
			return YSOK;
		}
		break;
	case FSKEY_RIGHT:
		if(YSTRUE==horizontal)
		{
			Increment();
			return YSOK;
		}
		break;
	case FSKEY_UP:
		if(YSTRUE!=horizontal)
		{
			Increment();
			return YSOK;
		}
		break;
	case FSKEY_DOWN:
		if(YSTRUE!=horizontal)
		{
			Decrement();
			return YSOK;
		}
		break;
	}
	return YSERR;
}

void FsGuiSlider::LButtonDown(int mx,int my)
{
	int sx0,sy0,sx1,sy1;
	int cx0,cy0,cx1,cy1;
	GetSliderRect(sx0,sy0,sx1,sy1);
	GetCursorRect(cx0,cy0,cx1,cy1);

	if(cx0<=mx && mx<=cx1 && cy0<=my && my<=cy1)
	{
		int cx,cy;
		GetCursorCenter(cx,cy);
		dragging=YSTRUE;
		dragMouseOffsetX=(cx-mx);
		dragMouseOffsetY=(cy-my);
	}
	else if(sx0<=mx && mx<=sx1 && sy0<=my && my<=sy1)
	{
		double pos,prevPos,prevValue;
		prevPos=GetPosition();
		prevValue=GetScaledValue();

		pos=CalculatePositionFromScreenCoord(mx,my);
		SetPosition(pos);
		dragging=YSTRUE;
		dragMouseOffsetX=0;
		dragMouseOffsetY=0;

		if(owner!=NULL)
		{
			owner->SetNeedRedraw(YSTRUE);
			owner->OnSliderPositionChange(this,prevPos,prevValue);
		}
	}
}

void FsGuiSlider::LButtonUp(int /*mx*/,int /*my*/)
{
	if(YSTRUE==dragging)
	{
		dragging=YSFALSE;
		if(owner!=NULL)
		{
			owner->SetNeedRedraw(YSTRUE);
		}
	}
}

void FsGuiSlider::SetMouseState(YSBOOL lb,YSBOOL /*mb*/,YSBOOL /*rb*/,int mx,int my)
{
	if(YSTRUE==dragging)
	{
		if(YSTRUE!=lb)
		{
			LButtonUp(mx,my);
		}
		else
		{
			double pos,prevPos,prevValue;
			prevPos=GetPosition();
			prevValue=GetScaledValue();

			pos=CalculatePositionFromScreenCoord(mx+dragMouseOffsetX,my+dragMouseOffsetY);
			SetPosition(pos);
			if(owner!=NULL)
			{
				owner->SetNeedRedraw(YSTRUE);
				owner->OnSliderPositionChange(this,prevPos,prevValue);
			}
		}
	}
}



////////////////////////////////////////////////////////////

void FsGuiTabControl::FsGuiTab::Initialize(void)
{
	enabled=YSTRUE;
	isVisible=YSTRUE;
	isString=YSTRUE;
	wLabel.Set(NULL);


	bmp.CleanUp();


	int fontWid,fontHei;
	GetAsciiCharacterBitmapSize(fontWid,fontHei);

	lastX=fontWid/2;
	lastY=fontWid/2;
	tabItem.Set(0,NULL);
	tabGrpBox.Set(0,NULL);
}

FsGuiTabControl::FsGuiTabControl()
{
}

void FsGuiTabControl::Initialize(FsGuiDialog *owner)
{
	FsGuiDialogItem::Initialize(owner);

	tabList.Set(0,NULL);
	currentTab=0;
	mouseOnTab=-1;
	fieldHei=0;
}


int FsGuiTabControl::AddTab(const char str[])
{
	YsWString wStr;
	wStr.SetUTF8String(str);
	return AddTab(wStr);
}

int FsGuiTabControl::AddTab(const wchar_t str[])
{
	int tabId=(int)tabList.GetN();

	tabList.Increment();
	tabList[tabId].Initialize();

	YsWString wStr;
	if(NULL!=str && 0!=str[0])
	{
		wStr.Set(str);
	}
	else
	{
		const wchar_t space[]={L" "}; // Workaround to cope with 64-bit GCC bug in Mac OSX
		wStr.Set(space);
	}

	YsBitmap wholeBmp;
	if(YSOK==RenderUnicodeString(wholeBmp,wStr,defTabClosedFgCol,defTabClosedBgCol) ||
	   YSOK==RenderAsciiString(wholeBmp,wStr,defTabClosedFgCol,defTabClosedBgCol))
	{
		tabList[tabId].bmp.SetBitmap(wholeBmp);
	}
	else
	{
		tabList[tabId].bmp.MakeSmallEmptyBitmap();
	}

	tabList[tabId].isString=YSTRUE;
	tabList[tabId].wLabel.Set(str);
	tabList[tabId].labelWid=tabList[tabId].bmp.GetWidth()+6;
	tabList[tabId].labelHei=tabList[tabId].bmp.GetHeight()+6;

	UpdateTab();

	return tabId;
}

int FsGuiTabControl::AddBmpTab(const YsBitmap &bmpIn,const wchar_t altStr[])
{
	const int tabId=(int)tabList.GetN();

	tabList.Increment();

	tabList[tabId].Initialize();

	YsWString altWStr;
	altWStr.Set(altStr);

	tabList[tabId].isString=YSFALSE;
	tabList[tabId].bmp.SetBitmap(bmpIn);
	tabList[tabId].wLabel.Set(altStr);
	tabList[tabId].labelWid=tabList[tabId].bmp.GetWidth()+6;
	tabList[tabId].labelHei=tabList[tabId].bmp.GetHeight()+6;

	UpdateTab();
	return tabId;
}

void FsGuiTabControl::EnableTab(int tabId)
{
	if(YSTRUE==tabList.IsInRange(tabId))
	{
		tabList[tabId].enabled=YSTRUE;
	}
}

void FsGuiTabControl::DisableTab(int tabId)
{
	if(YSTRUE==tabList.IsInRange(tabId))
	{
		tabList[tabId].enabled=YSFALSE;
	}
}

void FsGuiTabControl::Refresh(void)
{
	for(auto &tab : tabList)
	{
		for(auto itm : tab.tabItem)
		{
			itm->Hide();
		}
	}
	const int tabId=GetCurrentTab();
	if(YSTRUE==tabList.IsInRange(tabId))
	{
		for(auto itm : tabList[tabId].tabItem)
		{
			itm->Show();
		}
	}
}

void FsGuiTabControl::ExpandTab(void)
{
	if(0<tabList.GetN())
	{
		int totalLabelWid=0,maxLabelWid=0;
		for(int i=0; i<tabList.GetN(); ++i)
		{
			totalLabelWid+=tabList[i].labelWid;
			maxLabelWid=YsGreater(maxLabelWid,tabList[i].labelWid);
		}

		int avgLabelWid=wid/(int)tabList.GetN();
		if(maxLabelWid<avgLabelWid) // In this case, distribute evenly
		{
			for(int i=0; i<tabList.GetN(); ++i)
			{
				const int x0=wid*i/(int)tabList.GetN();
				const int x1=wid*(i+1)/(int)tabList.GetN();
				tabList[i].labelWid=x1-x0;
			}
		}
		else  // In this case, proportionaly grow the tab width.
		{
			int used=0;
			for(int i=0; i<tabList.GetN()-1; ++i)
			{
				tabList[i].labelWid=tabList[i].labelWid*wid/totalLabelWid;
				used+=tabList[i].labelWid;
			}
			tabList[tabList.GetN()-1].labelWid=wid-used;
		}
	}
}

void FsGuiTabControl::UpdateTab(void)
{
	int labelWidAll=0,labelHeiAll=0;
	int i;
	forYsArray(i,tabList)
	{
		labelWidAll+=tabList[i].labelWid;
		labelHeiAll=YsGreater(labelHeiAll,tabList[i].labelHei);
	}

	if(wid<labelWidAll)
	{
		wid=labelWidAll;
	}
	if(hei<labelHeiAll)
	{
		hei=labelHeiAll;
	}
	if(fieldHei<labelHeiAll)
	{
		fieldHei=labelHeiAll;
	}

	owner->SetNeedRedraw(YSTRUE);
}

int FsGuiTabControl::GetCurrentTab(void) const
{
	return currentTab;
}

void FsGuiTabControl::SelectCurrentTab(int tabId)
{
	if(YSTRUE==tabList.IsInRange(tabId) && currentTab!=tabId)
	{
		for(int i=0; i<tabList.GetN(); ++i)
		{
			for(int j=0; j<tabList[i].tabItem.GetN(); ++j)
			{
				if(i==tabId)
				{
					tabList[i].tabItem[j]->Show();
				}
				else
				{
					tabList[i].tabItem[j]->Hide();
				}
			}
			for(int j=0; j<tabList[i].tabGrpBox.GetN(); ++j)
			{
				if(i==tabId)
				{
					tabList[i].tabGrpBox[j]->Show();
				}
				else
				{
					tabList[i].tabGrpBox[j]->Hide();
				}
			}
		}

		if(YSTRUE==tabList.IsInRange(currentTab) && YSTRUE==tabList[currentTab].isString)
		{
			FsGuiChangeBmpNonTransparentColor(tabList[currentTab].bmp,defTabClosedFgCol);
		}
		currentTab=tabId;
		if(YSTRUE==tabList.IsInRange(currentTab) && YSTRUE==tabList[currentTab].isString)
		{
			FsGuiChangeBmpNonTransparentColor(tabList[currentTab].bmp,defFgCol);
		}

		if(NULL!=owner)
		{
			owner->SetPopUp(NULL);
			owner->SetNeedRedraw(YSTRUE);
		}
	}
}

void FsGuiTabControl::SelectFirstTab(void)
{
	for(int i=0; i<tabList.GetN(); ++i)
	{
		if(YSTRUE==tabList[i].enabled)
		{
			SelectCurrentTab(i);
			break;
		}
	}
}

void FsGuiTabControl::SelectNextTab(void)
{
	for(int i=0; i<tabList.GetN(); ++i)
	{
		int newTab=(currentTab+1)%tabList.GetN();
		if(YSTRUE==tabList[newTab].enabled)
		{
			SelectCurrentTab(newTab);
			break;
		}
	}
}

void FsGuiTabControl::SelectPrevTab(void)
{
	for(int i=0; i<tabList.GetN(); ++i)
	{
		auto newTab=(currentTab+tabList.GetN()-1)%tabList.GetN();
		if(YSTRUE==tabList[newTab].enabled)
		{
			SelectCurrentTab((int)newTab);
			break;
		}
	}
}

YSRESULT FsGuiTabControl::KeyIn(int key,YSBOOL /*shift*/,YSBOOL /*ctrl*/,YSBOOL /*alt*/)
{
	switch(key)
	{
	case FSKEY_RIGHT:
		if(0<tabList.GetN())
		{
			SelectNextTab();
			return YSOK;
		}
		break;
	case FSKEY_LEFT:
		if(0<tabList.GetN())
		{
			SelectPrevTab();
			return YSOK;
		}
		break;
	}
	return YSERR;
}

void FsGuiTabControl::LButtonDown(int mx,int my)
{
	int prevTab=currentTab;

	int i;
	int x,y;
	x=x0;
	y=y0;
	forYsArray(i,tabList)
	{
		const int labelWid=tabList[i].labelWid;
		const int labelHei=tabList[i].labelHei;
		if(x<=mx && mx<x+labelWid && y<=my && my<y+labelHei)
		{
			if(YSTRUE==tabList[i].enabled)
			{
				SelectCurrentTab(i);
				break;
			}
		}
		x+=labelWid;
	}

	if(prevTab!=currentTab && NULL!=owner)
	{
		owner->OnSelectTab(this,currentTab);
		owner->SetNeedRedraw(YSTRUE);
	}
}

void FsGuiTabControl::LButtonUp(int /*mx*/,int /*my*/)
{
}

void FsGuiTabControl::SetMouseState(YSBOOL /*lb*/,YSBOOL /*mb*/,YSBOOL /*rb*/,int mx,int my)
{
	const int prevMouseOnTab=mouseOnTab;

	int i;
	int x,y;
	x=x0;
	y=y0;
	forYsArray(i,tabList)
	{
		const int labelWid=tabList[i].labelWid;
		const int labelHei=tabList[i].labelHei;
		if(x<=mx && mx<x+labelWid && y<=my && my<y+labelHei && YSTRUE==tabList[i].enabled)
		{
			mouseOnTab=i;
			break;
		}
		x+=labelWid;
	}

	if(owner!=NULL && prevMouseOnTab!=mouseOnTab)
	{
		owner->SetNeedRedraw(YSTRUE);
	}
}



////////////////////////////////////////////////////////////

FsGuiTreeControlNode::FsGuiTreeControlNode()
{
	parent=NULL;
	child=NULL;
	Initialize();
}

FsGuiTreeControlNode::~FsGuiTreeControlNode()
{
	Initialize();
}

void FsGuiTreeControlNode::Initialize(void)
{
	if(NULL!=child)
	{
		delete child;
		child=NULL;
	}
	flags=0;
	x=0;
	y=0;
	label.Set(L"");
	bitmap.CleanUp();
	intAttrib=0;
}

void FsGuiTreeControlNode::SetSelect(YSBOOL sel)
{
	if(YSTRUE==sel)
	{
		Select();
	}
	else
	{
		Unselect();
	}
}

void FsGuiTreeControlNode::Select(void)
{
	if(0==(flags&FLAG_SELECTED))
	{
		flags|=FLAG_SELECTED;
		Render();
	}
}

void FsGuiTreeControlNode::Unselect(void)
{
	if(0!=(flags&FLAG_SELECTED))
	{
		flags&=~FLAG_SELECTED;
		Render();
	}
}

YSBOOL FsGuiTreeControlNode::IsSelected(void) const
{
	if(0!=(flags&FLAG_SELECTED))
	{
		return YSTRUE;
	}
	else
	{
		return YSFALSE;
	}
}

int FsGuiTreeControlNode::GetNChild(void) const
{
	if(NULL!=child)
	{
		return (int)child->GetN();
	}
	return 0;
}

const FsGuiTreeControlNode *FsGuiTreeControlNode::GetChild(YSSIZE_T index) const
{
	if(0<=index && index<GetNChild())
	{
		return (*child)[index];
	}
	return NULL;
}

const FsGuiTreeControlNode *FsGuiTreeControlNode::GetParent(void) const
{
	return parent;
}

YSBOOL FsGuiTreeControlNode::IsOpen(void) const
{
	if(0!=(FLAG_OPEN&flags))
	{
		return YSTRUE;
	}
	else
	{
		return YSFALSE;
	}
}

YSBOOL FsGuiTreeControlNode::IsParentOpen(void) const
{
	if(NULL!=parent)
	{
		return parent->IsOpen();
	}
	return YSFALSE;
}

void FsGuiTreeControlNode::SetIntAttrib(int attrib) const
{
	intAttrib=attrib;
}
int FsGuiTreeControlNode::GetIntAttrib(void) const
{
	return intAttrib;
}
void FsGuiTreeControlNode::GetLabel(YsWString &wStr) const
{
	wStr=label;
}

void FsGuiTreeControlNode::SetOpen(YSBOOL sw) // Only called from FsGuiDialogItem
{
	if(YSTRUE==sw)
	{
		flags|=FLAG_OPEN;
	}
	else
	{
		flags&=~FLAG_OPEN;
	}
}

void FsGuiTreeControlNode::SetLabel(const wchar_t label[]) // Only called from FsGuiTreeControl
{
	this->label.Set(label);
}

void FsGuiTreeControlNode::Render(void) // Only called from FsGuiTreeControl
{
	const YsColor &fgCol=(YSTRUE==IsSelected() ? FsGuiObject::defListActiveFgCol : FsGuiObject::defListFgCol);
	const YsColor &bgCol=(YSTRUE==IsSelected() ? FsGuiObject::defListActiveBgCol : FsGuiObject::defListBgCol);

	YsBitmap wholeBmp;
	if(YSOK==FsGuiObject::RenderUnicodeString(wholeBmp,label,fgCol,bgCol) ||
	   YSOK==FsGuiObject::RenderAsciiString(wholeBmp,label,fgCol,bgCol))
	{
		bitmap.SetBitmap(wholeBmp);
	}
	else
	{
		bitmap.MakeSmallEmptyBitmap();
	}
}

FsGuiTreeControlNode *FsGuiTreeControlNode::AddChild(FsGuiTreeControlNode *newNodePtr) // Only called from FsGuiTreeControl
{
	if(NULL==child)
	{
		child=new YsArray <FsGuiTreeControlNode *>;
	}

	YSSIZE_T n=child->GetN();
	child->Append(newNodePtr);
	(*child)[n]->parent=this;
	return (*child)[n];
}

FsGuiTreeControlNode *FsGuiTreeControlNode::GetChild(YSSIZE_T index)
{
	if(NULL!=child && YSTRUE==child->IsInRange(index))
	{
		return (*child)[index];
	}
	return NULL;
}

void FsGuiTreeControlNode::SetPosition(int x,int y) const // Only called from FsGuiTreeControl
{
	this->x=x;
	this->y=y;
}

const FsGuiBitmapType &FsGuiTreeControlNode::GetBitmap(void) const
{
	return bitmap;
}

int FsGuiTreeControlNode::LowerY(void) const
{
	return y+bitmap.GetHeight();
}

int FsGuiTreeControlNode::Width(void) const
{
	return bitmap.GetWidth();
}

int FsGuiTreeControlNode::Height(void) const
{
	return bitmap.GetHeight();
}

YSBOOL FsGuiTreeControlNode::IsDescendantOf(const FsGuiTreeControlNode *ofThis) const
{
	if(this==ofThis)
	{
		return YSTRUE;
	}

	for(YSSIZE_T i=0; i<ofThis->GetNChild(); ++i)
	{
		if(this==ofThis->GetChild(i))
		{
			return YSTRUE;
		}

		if(0<ofThis->GetChild(i)->GetNChild() &&
		   YSTRUE==IsDescendantOf(ofThis->GetChild(i)))
		{
			return YSTRUE;
		}
	}

	return YSFALSE;
}

////////////////////////////////////////////////////////////

FsGuiTreeControl::FsGuiTreeControl()
{
	LocalInitialize();
	itemType=FSGUI_TREECONTROL;
	dragTo=nullptr;
	cursorPos=nullptr;
}

FsGuiTreeControl::~FsGuiTreeControl()
{
	rootNode.Initialize();
}

void FsGuiTreeControl::Initialize(FsGuiDialog *owner)
{
	FsGuiDialogItem::Initialize(owner);
	LocalInitialize();
}

void FsGuiTreeControl::CleanUp(void)
{
	LocalInitialize();
}

void FsGuiTreeControl::GetVerticalScrollBarRect(int &x1,int &y1,int &x2,int &y2) const
{
	x1=this->x0+this->wid-defHScrollBar;
	y1=this->y0;
	x2=this->x0+this->wid-1;
	y2=this->y0+this->hei-1;
}

void FsGuiTreeControl::SetCaptureScrollBar(YSBOOL sw)
{
	if(YSTRUE==sw)
	{
		state|=STATE_CAPTURESCROLLBAR;
	}
	else
	{
		state&=~STATE_CAPTURESCROLLBAR;
	}
}

YSBOOL FsGuiTreeControl::CaptureScrollBar(void) const
{
	if(0!=(state&STATE_CAPTURESCROLLBAR))
	{
		return YSTRUE;
	}
	else
	{
		return YSFALSE;
	}
}

void FsGuiTreeControl::SetCaptureDragNode(YSBOOL sw,int mx,int my)
{
	if(YSTRUE==sw)
	{
		state|=STATE_CAPTUREDRAGNODE;
		mx0=mx;
		my0=my;
	}
	else
	{
		state&=~STATE_CAPTUREDRAGNODE;
	}
}

YSBOOL FsGuiTreeControl::CaptureDragNode(void) const
{
	if(0!=(state&STATE_CAPTUREDRAGNODE))
	{
		return YSTRUE;
	}
	else
	{
		return YSFALSE;
	}
}

YSBOOL FsGuiTreeControl::IsCursorWithinNode(const FsGuiTreeControlNode *node,int x,int y) const
{
	if(node->x+boxWid+defHSpaceUnit<=x && /* x<=node->x+boxWid+defHSpaceUnit+node->Width() &&*/
	   node->y<=y && y<=node->y+node->Height())
	{
		return YSTRUE;
	}
	return YSFALSE;
}

void FsGuiTreeControl::TransferNode(FsGuiTreeControlNode *thisNode,FsGuiTreeControlNode *newParent)
{
	FsGuiTreeControlNode *currentParent=thisNode->parent;
	if(NULL!=currentParent)
	{
		for(YSSIZE_T i=0; i<currentParent->child->GetN(); ++i)
		{
			if((*currentParent->child)[i]==thisNode)
			{
				currentParent->child->Delete(i);
			}
		}

		newParent->AddChild(thisNode);
		needRePosition=YSTRUE;
		owner->SetNeedRedraw(YSTRUE);
	}
}

void FsGuiTreeControl::GetVerticalScrollBarSliderRect(int &x1,int &y1,int &x2,int &y2) const
{
	GetVerticalScrollBarRect(x1,y1,x2,y2);
	y1+=defHScrollBar;
	y2-=defHScrollBar;
}

void FsGuiTreeControl::GetNodeRect(int &x1,int &y1,int &x2,int &y2,const FsGuiTreeControlNode *node) const
{
	x1=node->x+boxWid+defHSpaceUnit;
	y1=node->y;
	x2=x1+node->Width();
	y2=y1+node->Height();
}

void FsGuiTreeControl::LocalInitialize(void)
{
	rootNode.Initialize();
	needReRender=YSFALSE;
	needRePosition=YSFALSE;
	rootNode.SetOpen(YSTRUE); // Is always open.
	rootNode.SetLabel(L"<<ROOT>>");
	rootNode.Render();

	nodeAlloc.Clear();

	flags=0;
	state=0;

	xScroll=0;
	yScroll=0;

	contentWid=1;
	contentHei=1;
	boxWid=1;
	vSpace=2;

	nextRollClk=0;

	cursorPos=NULL;
}

const FsGuiTreeControlNode *FsGuiTreeControl::GetRoot(void) const
{
	return &rootNode;
}

const FsGuiTreeControlNode *FsGuiTreeControl::AddChild(const FsGuiTreeControlNode *parentNode)
{
	nodeAlloc.Increment();
	nodeAlloc.GetEnd().Initialize();

	FsGuiTreeControlNode *nonConstPtr=(FsGuiTreeControlNode *)parentNode;
	FsGuiTreeControlNode *newNode=nonConstPtr->AddChild(&nodeAlloc.GetEnd());
	if(NULL!=newNode)
	{
		if(NULL==cursorPos)
		{
			cursorPos=newNode;
		}

		if(YSTRUE==parentNode->IsOpen())
		{
			owner->SetNeedRedraw(YSTRUE);
			needReRender=YSTRUE;
		}
	}
	return newNode;
}

const FsGuiTreeControlNode *FsGuiTreeControl::AddTextChild(const FsGuiTreeControlNode *parentNode,const wchar_t str[])
{
	const FsGuiTreeControlNode *newNode=AddChild(parentNode);
	SetNodeLabel(newNode,str);
	return newNode;
}

const FsGuiTreeControlNode *FsGuiTreeControl::AddTextChild(const FsGuiTreeControlNode *parentNode,const char str[])
{
	YsWString wStr;
	wStr.SetUTF8String(str);
	return AddTextChild(parentNode,wStr);
}

void FsGuiTreeControl::SetNodeLabel(const FsGuiTreeControlNode *node,const wchar_t label[])
{
	FsGuiTreeControlNode *nonConstPtr=(FsGuiTreeControlNode *)node;
	nonConstPtr->SetLabel(label);
	if(YSTRUE==nonConstPtr->IsParentOpen())
	{
		nonConstPtr->Render();
		owner->SetNeedRedraw(YSTRUE);
		needRePosition=YSTRUE;
	}
}

void FsGuiTreeControl::SetNodeLabel(const FsGuiTreeControlNode *node,const char label[])
{
	YsWString wStr;
	wStr.SetUTF8String(label);
	SetNodeLabel(node,wStr);
}

void FsGuiTreeControl::OpenNode(const FsGuiTreeControlNode *node)
{
printf("%s %d\n",__FUNCTION__,__LINE__);
	if(YSTRUE!=node->IsOpen())
	{
		FsGuiTreeControlNode *nonConstPtr=(FsGuiTreeControlNode *)node;

		needRePosition=YSTRUE;
		nonConstPtr->SetOpen(YSTRUE);

printf("%s %d\n",__FUNCTION__,__LINE__);
		YsArray <FsGuiTreeControlNode *> todo;
		todo.Append(nonConstPtr);
		for(YSSIZE_T i=0; i<todo.GetN(); ++i)
		{
printf("%s %d\n",__FUNCTION__,__LINE__);
			todo[i]->Render();
			for(YSSIZE_T j=0; j<todo[i]->GetNChild(); ++j)
			{
				todo.Append(todo[i]->GetChild(j));
			}
		}
	}
}

void FsGuiTreeControl::CloseNode(const FsGuiTreeControlNode *node)
{
	if(node!=&rootNode && YSTRUE==node->IsOpen())  // Root node must not close
	{
		needRePosition=YSTRUE;

		FsGuiTreeControlNode *nonConstPtr=(FsGuiTreeControlNode *)node;
		nonConstPtr->SetOpen(YSFALSE);

		YsArray <FsGuiTreeControlNode *> todo;
		todo.Append(nonConstPtr);
		for(YSSIZE_T i=0; i<todo.GetN(); ++i)
		{
			if(0<i)
			{
				todo[i]->bitmap.MakeSmallEmptyBitmap();
			}
			for(YSSIZE_T j=0; j<todo[i]->GetNChild(); ++j)
			{
				todo.Append(todo[i]->GetChild(j));
			}
		}
	}
}

void FsGuiTreeControl::SetYScroll(int yScroll)
{
	if(this->yScroll!=yScroll)
	{
		this->yScroll=yScroll;
		owner->SetNeedRedraw(YSTRUE);
	}
}

const FsGuiTreeControlNode *FsGuiTreeControl::FindNodeAtWindowCoord(int x,int y) const
{
	x=x-this->x0+xScroll;
	y=y-this->y0+yScroll;


	YsArray <NodeAndIndex> stack;

	if(YSTRUE==ShowRoot() && YSTRUE==IsCursorWithinNode(&rootNode,x,y))
	{
		return &rootNode;
	}

	stack.Increment();
	stack.GetEnd().node=&rootNode;
	stack.GetEnd().childIndex=0;
	while(0<stack.GetN())
	{
		if(stack.GetEnd().node->GetNChild()<=stack.GetEnd().childIndex)
		{
			stack.DeleteLast();
		}
		else
		{
			const int childIndex=stack.GetEnd().childIndex++;
			const FsGuiTreeControlNode *child=stack.GetEnd().node->GetChild(childIndex);

			if(YSTRUE==IsCursorWithinNode(child,x,y))
			{
				return child;
			}

			if(YSTRUE==child->IsOpen() && 0<child->GetNChild())
			{
				stack.Increment();
				stack.GetEnd().node=child;
				stack.GetEnd().childIndex=0;
			}
		}
	}
	return NULL;
}

void FsGuiTreeControl::SetMultiSelect(YSBOOL sw)
{
	if(YSTRUE==sw)
	{
		flags|=FLAG_MULTISELECT;
	}
	else
	{
		flags&=~FLAG_MULTISELECT;
	}
}

YSBOOL FsGuiTreeControl::MultiSelectMode(void) const
{
	if(0!=(flags&FLAG_MULTISELECT))
	{
		return YSTRUE;
	}
	else
	{
		return YSFALSE;
	}
}

void FsGuiTreeControl::SetCanDragNode(YSBOOL sw)
{
	if(YSTRUE==sw)
	{
		flags|=FLAG_CANDRAGNODE;
	}
	else
	{
		flags&=~FLAG_CANDRAGNODE;
	}
}

YSBOOL FsGuiTreeControl::CanDragNode(void) const
{
	if(0!=(flags&FLAG_CANDRAGNODE))
	{
		return YSTRUE;
	}
	else
	{
		return YSFALSE;
	}
}

void FsGuiTreeControl::SetShowRoot(YSBOOL sw)
{
	if(ShowRoot()!=sw)
	{
		needRePosition=YSTRUE;
		owner->SetNeedRedraw(YSTRUE);
	}

	if(YSTRUE==sw)
	{
		flags|=FLAG_SHOWROOT;
	}
	else
	{
		flags&=~FLAG_SHOWROOT;
	}
}

YSBOOL FsGuiTreeControl::ShowRoot(void) const
{
	if(0!=(flags&FLAG_SHOWROOT))
	{
		return YSTRUE;
	}
	else
	{
		return YSFALSE;
	}
}

const FsGuiTreeControlNode *FsGuiTreeControl::GetShowTopNode(void) const
{
	return FindShowTopNode(&rootNode);
}

const FsGuiTreeControlNode *FsGuiTreeControl::FindShowTopNode(const FsGuiTreeControlNode *node) const
{
	for(YSSIZE_T i=0; i<node->GetNChild(); ++i)
	{
		const FsGuiTreeControlNode *child=node->GetChild(i);
		if(0<child->LowerY()-yScroll)
		{
			return child;
		}

		if(YSTRUE==child->IsOpen())
		{
			const FsGuiTreeControlNode *showTop=FindShowTopNode(child);
			if(NULL!=showTop)
			{
				return showTop;
			}
		}
	}
	return NULL;
}

const FsGuiTreeControlNode *FsGuiTreeControl::GetSecondVisibleNode(void) const
{
	const FsGuiTreeControlNode *showTopNode=GetShowTopNode();
	if(NULL!=showTopNode)
	{
		const FsGuiTreeControlNode *prevNode=NULL;
		const FsGuiTreeControlNode *secondVisibleNode=FindSecondVisibleNode(&rootNode,showTopNode,prevNode);
		return secondVisibleNode;
	}
	return NULL;
}

const FsGuiTreeControlNode *FsGuiTreeControl::FindSecondVisibleNode(const FsGuiTreeControlNode *current,const FsGuiTreeControlNode *nextOf,const FsGuiTreeControlNode *&prev) const
{
	for(YSSIZE_T i=0; i<current->GetNChild(); ++i)
	{
		const FsGuiTreeControlNode *child=current->GetChild(i);
		if(prev==nextOf)
		{
			return child;
		}
		prev=child;

		if(YSTRUE==child->IsOpen())
		{
			const FsGuiTreeControlNode *nextVisible=FindSecondVisibleNode(child,nextOf,prev);
			if(NULL!=nextVisible)
			{
				return nextVisible;
			}
		}
	}
	return NULL;
}

const FsGuiTreeControlNode *FsGuiTreeControl::GetLastHiddenNode(void) const
{
	const FsGuiTreeControlNode *showTopNode=GetShowTopNode();
	if(NULL!=showTopNode)
	{
		const FsGuiTreeControlNode *prevNode=NULL;
		const FsGuiTreeControlNode *lastHiddenNode=FindLastHiddenNode(&rootNode,showTopNode,prevNode);
		return lastHiddenNode;
	}
	return NULL;
}

const FsGuiTreeControlNode *FsGuiTreeControl::FindLastHiddenNode(const FsGuiTreeControlNode *current,const FsGuiTreeControlNode *showTop,const FsGuiTreeControlNode *&prev) const
{
	for(YSSIZE_T i=0; i<current->GetNChild(); ++i)
	{
		const FsGuiTreeControlNode *child=current->GetChild(i);
		if(showTop==child)
		{
			return prev;
		}
		prev=child;

		if(YSTRUE==child->IsOpen())
		{
			const FsGuiTreeControlNode *nextVisible=FindLastHiddenNode(child,showTop,prev);
			if(NULL!=nextVisible)
			{
				return nextVisible;
			}
		}
	}
	return NULL;
}

void FsGuiTreeControl::ScrollUp(void)
{
	const FsGuiTreeControlNode *prevNode=GetLastHiddenNode();
	if(NULL!=prevNode)
	{
		SetYScroll(prevNode->y);
		owner->SetNeedRedraw(YSTRUE);
	}
	else if(0<yScroll)
	{
		SetYScroll(0);
		owner->SetNeedRedraw(YSTRUE);
	}
}

void FsGuiTreeControl::ScrollDown(void)
{
	const FsGuiTreeControlNode *nextNode=GetSecondVisibleNode();
	if(NULL!=nextNode)
	{
		SetYScroll(nextNode->y);
		owner->SetNeedRedraw(YSTRUE);
	}
}

void FsGuiTreeControl::UnselectAll(void)
{
	rootNode.Unselect();

	YsArray <NodeAndIndex> stack;
	stack.Increment();
	stack.GetEnd().node=&rootNode;
	stack.GetEnd().childIndex=0;
	while(0<stack.GetN())
	{
		if(stack.GetEnd().node->GetNChild()<=stack.GetEnd().childIndex)
		{
			stack.DeleteLast();
		}
		else
		{
			const int childIndex=stack.GetEnd().childIndex++;
			const FsGuiTreeControlNode *child=stack.GetEnd().node->GetChild(childIndex);
			FsGuiTreeControlNode *nonConstPtr=(FsGuiTreeControlNode *)child;
			nonConstPtr->Unselect();
			if(0<child->GetNChild())
			{
				stack.Increment();
				stack.GetEnd().node=child;
				stack.GetEnd().childIndex=0;
			}
		}
	}
}

const FsGuiTreeControlNode *FsGuiTreeControl::GetSelection(void) const
{
	if(YSTRUE==rootNode.IsSelected())
	{
		return &rootNode;
	}

	YsArray <NodeAndIndex> stack;

	stack.Increment();
	stack.GetEnd().node=&rootNode;
	stack.GetEnd().childIndex=0;
	while(0<stack.GetN())
	{
		if(stack.Last().node->GetNChild()<=stack.Last().childIndex)
		{
			stack.DeleteLast();
		}
		else
		{
			const int childIndex=stack.Last().childIndex++;
			const FsGuiTreeControlNode *child=stack.Last().node->GetChild(childIndex);
			if(YSTRUE==child->IsSelected())
			{
				return child;
			}
			if(0<child->GetNChild())
			{
				stack.Increment();
				stack.GetEnd().node=child;
				stack.GetEnd().childIndex=0;
			}
		}
	}

	return NULL;
}

YSRESULT FsGuiTreeControl::KeyIn(int key ,YSBOOL /*shift*/ ,YSBOOL /*ctrl*/ ,YSBOOL /*alt*/)
{
	switch(key)
	{
	case FSKEY_SPACE:
		if(NULL!=cursorPos)
		{
			FsGuiTreeControlNode *nonConstPtr=(FsGuiTreeControlNode *)cursorPos;
			if(YSTRUE==MultiSelectMode())
			{
				const YSBOOL newSel=YsReverseBool(cursorPos->IsSelected());
				nonConstPtr->SetSelect(newSel);
				owner->SetNeedRedraw(YSTRUE);
				owner->OnTreeControlSelChange(this);
			}
			else
			{
				if(YSTRUE!=cursorPos->IsSelected())
				{
					UnselectAll();
					nonConstPtr->Select();
					owner->SetNeedRedraw(YSTRUE);
					owner->OnTreeControlSelChange(this);
				}
			}
		}
		break;
	case FSKEY_RIGHT:
		if(NULL!=cursorPos)
		{
			OpenNode(cursorPos);
			owner->SetNeedRedraw(YSTRUE);
		}
		break;
	case FSKEY_LEFT:
		if(NULL!=cursorPos)
		{
			CloseNode(cursorPos);
			owner->SetNeedRedraw(YSTRUE);
		}
		break;
	case FSKEY_DOWN:
	case FSKEY_UP:
		{
			YSBOOL moved=YSFALSE;
			YsArray <const FsGuiTreeControlNode *,16> nodeArray;
			GetAllVisibleNode(nodeArray);
			for(YSSIZE_T i=0; i<nodeArray.GetN(); ++i)
			{
				if(nodeArray[i]==cursorPos)
				{
					if(key==FSKEY_DOWN && i<nodeArray.GetN()-1)
					{
						cursorPos=nodeArray[i+1];
						moved=YSTRUE;
						break;
					}
					if(key==FSKEY_UP && 0<i)
					{
						cursorPos=nodeArray[i-1];
						moved=YSTRUE;
						break;
					}
				}
			}
			if(YSTRUE==moved)
			{
				owner->SetNeedRedraw(YSTRUE);
				if(cursorPos->y-yScroll<0)
				{
					yScroll=cursorPos->y;
					owner->OnTreeControlScroll(this);
				}
				if(cursorPos->LowerY()-yScroll>this->hei)
				{
					yScroll=cursorPos->LowerY()-this->hei;
					owner->OnTreeControlScroll(this);
				}
			}
		}
		break;
	case FSKEY_WHEELDOWN:
		ScrollDown();
		owner->OnTreeControlScroll(this);
		break;
	case FSKEY_WHEELUP:
		ScrollUp();
		owner->OnTreeControlScroll(this);
		break;
	}
	return YSERR;
}

YSRESULT FsGuiTreeControl::CharIn(int /*key*/)
{
	return YSERR;
}

void FsGuiTreeControl::LButtonDown(int mx,int my)
{
	int x1,y1,x2,y2;
	GetVerticalScrollBarSliderRect(x1,y1,x2,y2);
	if(contentHei>this->hei && x1<=mx && mx<=x2 && y1<=my && my<=y2)
	{
		int bugY1,bugY2;
		GetScrollBarBugPosition(bugY1,bugY2,y1,y2,contentHei,yScroll,this->hei);

		if(my<bugY1 || bugY2<my)
		{
			yScroll=GetShowTopFromPosition(y1,y2,my,contentHei,this->hei);
			owner->OnTreeControlScroll(this);
			owner->SetNeedRedraw(YSTRUE);

			GetScrollBarBugPosition(bugY1,bugY2,y1,y2,contentHei,yScroll,this->hei);  // Recalculate
		}

		SetCaptureScrollBar(YSTRUE);
		mx0=mx;
		my0=my;
		myOffset=bugY1-my;
	}
	else if(this->x0<=mx && mx<=this->x0+this->wid && this->y0<=my && this->y0+this->hei)
	{
		const int relX=mx-this->x0;
		const int relY=my-this->y0+yScroll;

		YsArray <const FsGuiTreeControlNode *> nodeArray;
		GetAllVisibleNode(nodeArray);
		for(YSSIZE_T i=0; i<nodeArray.GetN(); ++i)
		{
			if(0<=nodeArray[i]->GetNChild() &&
			   nodeArray[i]->y<=relY && relY<=nodeArray[i]->y+boxWid &&
			   nodeArray[i]->x<=relX && relX<=nodeArray[i]->x+boxWid)  // Clicked on a box
			{
				FsGuiTreeControlNode *nonConstPtr=(FsGuiTreeControlNode *)nodeArray[i];

				const YSBOOL setOpen=YsReverseBool(nodeArray[i]->IsOpen());
				if(YSTRUE==setOpen)
				{
					OpenNode(nonConstPtr);
				}
				else
				{
					nonConstPtr->SetOpen(setOpen);
				}
				needRePosition=YSTRUE;
				owner->SetNeedRedraw(YSTRUE);
			}
			else if(
			   nodeArray[i]->y                     <=relY && relY<=nodeArray[i]->LowerY() &&
			   nodeArray[i]->x+boxWid+defHSpaceUnit<=relX /*&& relX<=nodeArray[i]->x+boxWid+nodeArray[i]->Width()*/)
			   // 2013/01/05 Doesn't have to check right edge.  Allowing the user to click on anywhere to the right.
			{
				cursorPos=nodeArray[i];
				if(YSTRUE==MultiSelectMode())
				{
					const YSBOOL newSel=YsReverseBool(nodeArray[i]->IsSelected());
					FsGuiTreeControlNode *nonConstPtr=(FsGuiTreeControlNode *)nodeArray[i];
					nonConstPtr->SetSelect(newSel);

					if(NULL!=owner)
					{
						owner->SetNeedRedraw(YSTRUE);
						owner->OnTreeControlSelChange(this);
					}

					if(YSTRUE==newSel)
					{
						SetCaptureDragNode(YSTRUE,mx,my);
					}
				}
				else // Single-selection mode
				{
					if(YSTRUE!=nodeArray[i]->IsSelected())
					{
						UnselectAll();
						FsGuiTreeControlNode *nonConstPtr=(FsGuiTreeControlNode *)nodeArray[i];
						nonConstPtr->Select();
						if(NULL!=owner)
						{
							owner->SetNeedRedraw(YSTRUE);
							owner->OnTreeControlSelChange(this);
						}
					}
					SetCaptureDragNode(YSTRUE,mx,my);
				}
			}
		}
	}
}

void FsGuiTreeControl::LButtonUp(int mx,int my)
{
	const FsGuiTreeControlNode *dropTo=FindNodeAtWindowCoord(mx,my);
	if(YSTRUE==CanDragNode() && YSTRUE==CaptureDragNode() && NULL!=dropTo)
	{
		YsArray <const FsGuiTreeControlNode *,8> selNode;
		GetAllSelectedNode(selNode);

		YSBOOL infinite=YSFALSE;
		for(YSSIZE_T i=0; i<selNode.GetN(); ++i)
		{
			if(YSTRUE==dropTo->IsDescendantOf(selNode[i]))
			{
				infinite=YSTRUE;
				break;
			}
		}

		if(YSTRUE!=infinite)
		{
			if(NULL!=owner)
			{
				owner->OnTreeControlNodeAboutToMove(this,(int)selNode.GetN(),selNode);
			}

			for(YSSIZE_T i=0; i<selNode.GetN(); ++i)
			{
				TransferNode((FsGuiTreeControlNode *)selNode[i],(FsGuiTreeControlNode *)dropTo);
			}

			if(NULL!=owner)
			{
				owner->OnTreeControlNodeMoved(this,(int)selNode.GetN(),selNode);
			}
		}
	}

	SetCaptureScrollBar(YSFALSE);
	SetCaptureDragNode(YSFALSE,mx,my);
	dragTo=nullptr;
}

void FsGuiTreeControl::SetMouseState(YSBOOL lb,YSBOOL /*mb*/ ,YSBOOL /*rb*/ ,int mx,int my)
{
	if(YSTRUE!=lb)
	{
		SetCaptureScrollBar(YSFALSE);
		SetCaptureDragNode(YSFALSE,mx,my);
	}

	if(YSTRUE==CaptureDragNode())
	{
		auto tempDragTo=FindNodeAtWindowCoord(mx,my);
		if(nullptr!=tempDragTo)
		{
			YsArray <const FsGuiTreeControlNode *,8> selNode;
			GetAllSelectedNode(selNode);
			if(YSTRUE==selNode.IsIncluded(tempDragTo))
			{
				tempDragTo=nullptr;
			}
		}
		if(tempDragTo!=dragTo)
		{
			dragTo=tempDragTo;
			owner->SetNeedRedraw(YSTRUE);
		}

		if(my<this->y0 && nextRollClk<FsGuiClock())
		{
			ScrollUp();
			owner->OnTreeControlScroll(this);
			nextRollClk=FsGuiClock()+100;
		}
		else if(this->y0+this->hei<my && nextRollClk<FsGuiClock())
		{
			ScrollDown();
			owner->OnTreeControlScroll(this);
			nextRollClk=FsGuiClock()+100;
		}
	}

	lastMx=mx;
	lastMy=my;

	if(YSTRUE==lb)
	{
		if(nextRollClk<FsGuiClock() && contentHei>this->hei)
		{
			int x1,y1,x2,y2;
			GetVerticalScrollBarRect(x1,y1,x2,y2);

			if(x1<=mx && mx<=x2 && y1<=my && my<=y2)
			{
				if(my<=y1+defHScrollBar)
				{
					ScrollUp();
					owner->OnTreeControlScroll(this);
					owner->SetNeedRedraw(YSTRUE);
				}
				else if(my>=y2-defHScrollBar)
				{
					ScrollDown();
					owner->OnTreeControlScroll(this);
					owner->SetNeedRedraw(YSTRUE);
				}
			}

			nextRollClk=FsGuiClock()+100;
		}

		if(YSTRUE==CaptureScrollBar())
		{
			int x1,y1,x2,y2;
			GetVerticalScrollBarSliderRect(x1,y1,x2,y2);

			const int y=my+myOffset;
			const int newYScroll=GetShowTopFromPosition(y1,y2,y,contentHei,this->hei);

			if(newYScroll!=yScroll)
			{
				yScroll=newYScroll;
				owner->OnTreeControlScroll(this);
				owner->SetNeedRedraw(YSTRUE);
			}
		}
	}
}

void FsGuiTreeControl::RePosition(void) const
{
	int x=defHSpaceUnit,y=defVSpaceUnit;

	int fontWid,fontHei;
	GetAsciiCharacterBitmapSize(fontWid,fontHei);

	boxWid=fontHei;

	contentWid=1;
	contentHei=1;

	if(YSTRUE==ShowRoot())
	{
		rootNode.SetPosition(x,y);
		const FsGuiBitmapType &bitmap=rootNode.GetBitmap();
		const int yStep=YsGreater <int> (bitmap.GetHeight(),fontHei)+vSpace;
		x+=boxWid;
		y+=yStep;
	}

	RePosition(x,y,&rootNode,fontWid,fontHei);

	needRePosition=YSFALSE;
}

void FsGuiTreeControl::RePosition(int &x,int &y,const FsGuiTreeControlNode *node,int fontWid,int fontHei) const
{
	for(YSSIZE_T i=0; i<node->GetNChild(); ++i)
	{
		const FsGuiTreeControlNode *child=node->GetChild(i);
		child->SetPosition(x,y);

		const FsGuiBitmapType &bitmap=child->GetBitmap();
		const int yStep=YsGreater <int> (bitmap.GetHeight(),fontHei)+vSpace;

		y+=yStep;

		if(YSTRUE==child->IsOpen() && 0<child->GetNChild())
		{
			x+=boxWid;
			RePosition(x,y,child,fontWid,fontHei);
			x-=boxWid;
		}

		contentHei=YsGreater <int> (contentHei,y);
		contentWid=YsGreater <int> (contentWid,x+fontHei+defHSpaceUnit+bitmap.GetWidth());
	}
}

////////////////////////////////////////////////////////////



FsGuiGroupBox::FsGuiGroupBox()
{
	Initialize();
}

void FsGuiGroupBox::Initialize(void)
{
	n=0;
	x0=0;
	y0=0;
	x1=0;
	y1=0;
	visible=YSTRUE;
}

void FsGuiGroupBox::Show(void)
{
	visible=YSTRUE;
}

void FsGuiGroupBox::Hide(void)
{
	visible=YSFALSE;
}

YSBOOL FsGuiGroupBox::IsVisible(void) const
{
	return visible;
}

void FsGuiGroupBox::AddGuiItem(const FsGuiDialogItem *itm)
{
	if(itm!=NULL)
	{
		int x0,y0,x1,y1;

		x0=itm->x0-2;
		y0=itm->y0-2;
		x1=itm->x0+itm->wid+1;
		y1=itm->y0+itm->hei+1;

		if(n==0)
		{
			this->x0=x0;
			this->y0=y0;
			this->x1=x1;
			this->y1=y1;
		}
		else
		{
			this->x0=YsSmaller(this->x0,x0);
			this->y0=YsSmaller(this->y0,y0);
			this->x1=YsGreater(this->x1,x1);
			this->y1=YsGreater(this->y1,y1);
		}

		n++;
	}
}



FsGuiDialog::FsGuiDialog() : 
    btnList(btnAlloc),txtList(txtAlloc),lstList(lstAlloc),drpList(drpAlloc),sttList(sttAlloc),nbxList(nbxAlloc),cplList(cplAlloc),
    grpBoxList(grpBoxAlloc), sldList(sldAlloc),tabList(tabAlloc),parentDlg(NULL),foregroundDlg(NULL)
{
	Initialize();
	autoNewColumn=YSTRUE;
	isSelfDestructiveDialog=YSFALSE;
	isPermanent=YSFALSE;
}

/* virtual */ FsGuiDialog::~FsGuiDialog()
{
}

void FsGuiDialog::Initialize()
{
	ClearStringToIdentTable();

	arrangeType=FSDIALOG_ARRANGE_TOP_LEFT; // FSDIALOG_NOARRANGE;

	int fontWid,fontHei;
	GetAsciiCharacterBitmapSize(fontWid,fontHei);

	RemoveAllItem();

	txtMessage.Set(NULL);
	bmpMessage.CleanUp();
	msgWid=0;
	msgHei=0;

	dlgX0=0;
	dlgY0=0;
	dlgWid=65535;
	dlgHei=65535;
	itemX0=fontWid;
	itemY0=fontHei;

	enabled=YSTRUE;

	dialogBgCol=defDialogBgCol;
	tabBgCol=defTabBgCol;

	bgCol=defBgCol;
	fgCol=defFgCol;

	activeBgCol=defActiveBgCol;
	activeFgCol=defActiveFgCol;

	frameCol=defFrameCol;

	pMx=0;
	pMy=0;
	pLb=YSFALSE;
	pMb=YSFALSE;
	pRb=YSFALSE;

	endModal=YSFALSE;
	needRedraw=YSTRUE;
	result=YSOK;

	foregroundDlg=NULL;
	parentDlg=NULL;

	modalDialogIdent=0;

	closeModalCallBackFunc=NULL;
	stdCloseModalCallBack=nullptr;
}

void FsGuiDialog::RemoveAllItem(void)
{
	int fontWid,fontHei;
	GetAsciiCharacterBitmapSize(fontWid,fontHei);

	btnList.CleanUp();
	txtList.CleanUp();
	lstList.CleanUp();
	sttList.CleanUp();
	nbxList.CleanUp();
	cplList.CleanUp();
	sldList.CleanUp();
	treeAlloc.Clear();
	tabList.CleanUp();

	grpBoxList.CleanUp();

	itemList.Set(0,NULL);

	lastX=fontWid/2;
	lastY=fontHei;

	makingTab=NULL;
	makingTabId=-1;

	focus=NULL;
	mouseOver=NULL;
	clicked=NULL;
	popUp=NULL;
	primaryTab=NULL;
}

void FsGuiDialog::SetCloseModalCallBack(void (*closeModalCallBackFunc)(FsGuiControlBase *,FsGuiDialog *,int))
{
	this->closeModalCallBackFunc=closeModalCallBackFunc;
}

void FsGuiDialog::UnbindCloseModalCallBack(void)
{
	stdCloseModalCallBack=nullptr;
}

void FsGuiDialog::SetIsPermanent(YSBOOL flg)
{
	isPermanent=flg;
}
YSBOOL FsGuiDialog::IsPermanent(void) const
{
	return isPermanent;
}

void FsGuiDialog::SetTextMessage(const char msg[])
{
	YsWString wStr;
	wStr.SetUTF8String(msg);
	SetTextMessage(wStr);
}

void FsGuiDialog::SetTextMessage(const wchar_t msg[])
{
	msgWid=0;
	msgHei=0;

	txtMessage.Set(NULL);
	bmpMessage.CleanUp();

	YsBitmap wholeBmp;
	if(NULL!=msg && 0!=msg[0] && 
	   (YSOK==RenderUnicodeString(wholeBmp,msg,defFgCol,defBgCol) ||
	    YSOK==RenderAsciiString(wholeBmp,msg,defFgCol,defBgCol)))
	{
		bmpMessage.SetBitmap(wholeBmp);
		txtMessage.Set(msg);
		msgWid=bmpMessage.GetWidth();
		msgHei=bmpMessage.GetHeight();

		itemY0=dlgY0+msgHei+2*defVSpaceUnit;
		lastY=itemY0;
	}
}

void FsGuiDialog::SetTopLeftCorner(int x,int y)
{
	int fontWid,fontHei;
	GetAsciiCharacterBitmapSize(fontWid,fontHei);

	dlgX0=x;
	dlgY0=y;

	if(itemX0<x+fontWid)
	{
		itemX0=x+fontWid;
	}
	if(itemY0<y+fontHei)
	{
		itemY0=y+fontHei;
	}

	if(lastX<itemX0)
	{
		lastX=itemX0;
	}
	if(lastY<itemY0)
	{
		lastY=itemY0;
	}
}

void FsGuiDialog::SetSize(int w,int h)
{
	dlgWid=w;
	dlgHei=h;
}

void FsGuiDialog::SetTransparency(YSBOOL trsp)
{
	if(YSTRUE==trsp)
	{
		dialogBgCol.SetAi(0);
	}
	else
	{
		dialogBgCol.SetAi(255);
	}
}

void FsGuiDialog::SetBackgroundAlpha(const double alpha)
{
	dialogBgCol.SetAd(alpha);
}

void FsGuiDialog::SetBackgroundColor(const YsColor &col)
{
	dialogBgCol=col;
}

void FsGuiDialog::AlignRowMiddle(const YsArray <FsGuiDialogItem *> &dlgItemRow)
{
	YSBOOL first=YSTRUE;
	int rowY0=0,rowY1=0;
	for(int column=0; column<dlgItemRow.GetN(); ++column)
	{
		if(NULL!=dlgItemRow[column])
		{
			if(YSTRUE==first)
			{
				rowY0=dlgItemRow[column]->y0;
				rowY1=dlgItemRow[column]->y0+dlgItemRow[column]->hei;
				first=YSFALSE;
			}
			else
			{
				rowY0=YsSmaller(rowY0,dlgItemRow[column]->y0);
				rowY1=YsGreater(rowY1,dlgItemRow[column]->y0+dlgItemRow[column]->hei);
			}
		}
	}
	if(YSTRUE!=first)
	{
		const int midY=(rowY0+rowY1)/2;
		for(int column=0; column<dlgItemRow.GetN(); ++column)
		{
			if(NULL!=dlgItemRow[column])
			{
				dlgItemRow[column]->y0=midY-dlgItemRow[column]->hei/2;
			}
		}
	}
}

void FsGuiDialog::AlignColumnRight(const YsArray <FsGuiDialogItem *> &dlgItemColumn)
{
	int xRight=0;
	for(auto itm : dlgItemColumn)
	{
		YsMakeGreater(xRight,itm->x0+itm->wid);
	}
	for(auto itm : dlgItemColumn)
	{
		itm->x0=xRight-itm->wid;
	}
}

void FsGuiDialog::AlignLeftMiddle(const YsArray <YsArray <FsGuiDialogItem *> > &dlgItemMatrix)
{
	// Row
	for(int row=0; row<dlgItemMatrix.GetN(); ++row)
	{
		AlignRowMiddle(dlgItemMatrix[row]);
	}

	// Column
	YSSIZE_T nColumn=0;
	for(int row=0; row<dlgItemMatrix.GetN(); ++row)
	{
		nColumn=YsGreater <YSSIZE_T> (nColumn,dlgItemMatrix[row].GetN());
	}

	if(0<nColumn)
	{
		YsArray <int> columnWid(nColumn,0);
		for(int column=0; column<nColumn; ++column)
		{
			columnWid[column]=0;
		}

		YSBOOL first=YSFALSE;
		int x0=0;
		for(int row=0; row<dlgItemMatrix.GetN(); ++row)
		{
			for(int column=0; column<nColumn; ++column)
			{
				if(YSTRUE==dlgItemMatrix[row].IsInRange(column) && NULL!=dlgItemMatrix[row][column])
				{
					if(YSTRUE!=first)
					{
						x0=dlgItemMatrix[row][column]->x0;
						first=YSTRUE;
					}
					else
					{
						x0=YsSmaller(x0,dlgItemMatrix[row][column]->x0);
					}
					columnWid[column]=YsGreater <int> (columnWid[column],dlgItemMatrix[row][column]->wid);
				}
			}
		}
		for(int row=0; row<dlgItemMatrix.GetN(); ++row)
		{
			int x=x0;
			for(int column=0; column<dlgItemMatrix[row].GetN(); ++column)
			{
				if(NULL!=dlgItemMatrix[row][column])
				{
					dlgItemMatrix[row][column]->x0=x;
					dlgItemMatrix[row][column]->wid=columnWid[column];
				}
				x+=columnWid[column]+defHSpaceUnit;

				if(NULL!=makingTab)
				{
					const auto item=dlgItemMatrix[row][column];
					const int x1=item->x0+item->wid-1;

					const int tabCtrlWid=x1-makingTab->x0+2;
					if(makingTab->wid<tabCtrlWid)
					{
						makingTab->wid=tabCtrlWid;
					}
				}
			}
		}
	}
}

void FsGuiDialog::Fit(void)
{
	int i,w,h;

	if(0<msgWid && 0<msgHei)
	{
		w=msgWid+defHSpaceUnit*2;
		h=msgHei+defVSpaceUnit*2;
	}
	else
	{
		w=defHSpaceUnit*2;
		h=defVSpaceUnit;
	}

	forYsArray(i,itemList)
	{
		int x2,y2;
		x2=itemList[i]->x0+itemList[i]->wid-dlgX0;
		y2=itemList[i]->y0+itemList[i]->hei-dlgY0;

		w=YsGreater(w,x2+defHSpaceUnit);
		h=YsGreater(h,y2+defVSpaceUnit);
	}

	SetSize(w,h);
}

void FsGuiDialog::Move(int newX0,int newY0)
{
	int dx,dy;
	dx=newX0-dlgX0;
	dy=newY0-dlgY0;

	int i;
	forYsArray(i,itemList)
	{
		itemList[i]->x0+=dx;
		itemList[i]->y0+=dy;
	}

	YsListItem <FsGuiGroupBox> *grpBox;
	grpBox=NULL;
	while(NULL!=(grpBox=grpBoxList.FindNext(grpBox)))
	{
		grpBox->dat.x0+=dx;
		grpBox->dat.y0+=dy;
		grpBox->dat.x1+=dx;
		grpBox->dat.y1+=dy;
	}

	itemX0+=dx;
	itemY0+=dy;

	dlgX0=newX0;
	dlgY0=newY0;
}

void FsGuiDialog::StepToNextColumn(void)
{
	auto nextX=lastX;
	for(auto i=itemList.GetN()-1; i>=0 && lastX<=itemList[i]->x0; i--)
	{
		nextX=YsGreater(nextX,itemList[i]->x0+itemList[i]->wid);
	}

	int fontWid,fontHei;
	GetAsciiCharacterBitmapSize(fontWid,fontHei);

	lastX=nextX+fontWid;
	lastY=itemY0;
}

void FsGuiDialog::SetFocus(FsGuiDialogItem *focus)
{
	FSGUIITEMTYPE prevFocusType=FSGUI_STATIC;
	if(nullptr!=this->focus)
	{
		prevFocusType=this->focus->GetItemType();
	}

	this->focus=focus;
	if(NULL!=focus)
	{
		switch(focus->GetItemType())
		{
		default:
			if(prevFocusType==FSGUI_TEXTBOX)
			{
				FsDisableIME();
			}
			break;
		case FSGUI_TEXTBOX:
			{
				auto txt=(FsGuiTextBox *)focus;
				txt->JustGotFocus();
			}
			break;
		}
	}
	needRedraw=YSTRUE;
}

FsGuiDialogItem *FsGuiDialog::GetFocus(void) const
{
	return focus;
}

int FsGuiDialog::GetLeftX(void) const
{
	return dlgX0;
}

int FsGuiDialog::GetTopY(void) const
{
	return dlgY0;
}

int FsGuiDialog::GetWidth(void) const
{
	return dlgWid;
}

int FsGuiDialog::GetHeight(void) const
{
	return dlgHei;
}

YSBOOL FsGuiDialog::GetTransparency(void) const
{
	if(2>dialogBgCol.Ai())
	{
		return YSTRUE;
	}
	return YSFALSE;
}

FsGuiDialogItem *FsGuiDialog::FindItem(int mx,int my)
{
	int i;
	forYsArray(i,itemList)
	{
		if(itemList[i]->x0<=mx && mx<itemList[i]->x0+itemList[i]->wid &&
		   itemList[i]->y0<=my && my<itemList[i]->y0+itemList[i]->hei)
		{
			return itemList[i];
		}
	}
	return NULL;
}

FsGuiDialogItem *FsGuiDialog::FindVisibleItem(int mx,int my)
{
	int i;
	forYsArray(i,itemList)
	{
		if(YSTRUE==itemList[i]->IsVisible() &&
		   itemList[i]->x0<=mx && mx<itemList[i]->x0+itemList[i]->wid &&
		   itemList[i]->y0<=my && my<itemList[i]->y0+itemList[i]->hei)
		{
			return itemList[i];
		}
	}
	return NULL;
}

FsGuiDialog *FsGuiDialog::GetActiveModalDialog(void) const
{
	auto dlg=GetModalDialog();
	if(NULL!=dlg)
	{
		while(NULL!=dlg->GetModalDialog())
		{
			dlg=dlg->GetModalDialog();
		}
		return dlg;
	}
	return NULL;
}

YSBOOL FsGuiDialog::IsMouseOnDialog(int mx,int my) const
{
	if(YSTRUE!=GetTransparency() &&
	   (GetLeftX()<=mx && mx<=GetLeftX()+GetWidth() &&
	    GetTopY()<=my  && my<=GetTopY() +GetHeight()))
	{
		return YSTRUE;
	}

	if(NULL!=popUp && YSTRUE==popUp->IsInRange(mx,my))
	{
		return YSTRUE;
	}

	if(YSTRUE==GetTransparency())
	{
		for(int i=0; i<itemList.GetN(); ++i)
		{
			if(YSTRUE==itemList[i]->IsInRange(mx,my))
			{
				return YSTRUE;
			}
		}
	}

	return YSFALSE;
}

YSRESULT FsGuiDialog::SetMouseState(YSBOOL lb,YSBOOL mb,YSBOOL rb,int mx,int my)
{
	auto activeModal=GetActiveModalDialog();
	if(NULL!=activeModal)
	{
		return activeModal->SetMouseState(lb,mb,rb,mx,my);
	}

	if(enabled==YSTRUE)
	{
		int i;
		FsGuiDialogItem *prevMouseOver,*prevFocus;

		prevMouseOver=mouseOver;
		prevFocus=focus;

		mouseOver=FindVisibleItem(mx,my);

		if(NULL!=mouseOver && YSTRUE!=mouseOver->IsEnabled())
		{
			mouseOver=NULL;
		}

		if(pLb!=YSTRUE && lb==YSTRUE && 0==prevTouch.GetN()) // Don't fire LButtonDown if a touch is on.
		{
			if(popUp!=NULL)
			{
				popUp->LButtonDown(mx,my);
			}
			else if(mouseOver!=NULL)
			{
				mouseOver->LButtonDown(mx,my);
			}
		}

		if(pLb==YSTRUE && lb!=YSTRUE && 0==prevTouch.GetN()) // Don't fire LButtonDown if a touch is on.
		{
			if(popUp!=NULL)
			{
				// (*) 2017/07/30 FsGuiFileDialog hack may fire up the file dialog before LButtonUp returns.
				//     as a result, IME sub-window may still be active, and file dialog won't become visible
				//     forever, and the program gets into an infinite loop.  To prevent such an infinite loop,
				//     IME must be disabled, when the focus is about to change.
				if(popUp!=this->focus)
				{
					FsDisableIME();
				}
				popUp->LButtonUp(mx,my);
				SetFocus(popUp);
				clicked=popUp;
			}
			else if(mouseOver!=NULL)
			{
				// (*) 2017/07/30 FsGuiFileDialog hack may fire up the file dialog before LButtonUp returns.
				//     as a result, IME sub-window may still be active, and file dialog won't become visible
				//     forever, and the program gets into an infinite loop.  To prevent such an infinite loop,
				//     IME must be disabled, when the focus is about to change.
				if(mouseOver!=this->focus)
				{
					FsDisableIME();
				}
				mouseOver->LButtonUp(mx,my);
				SetFocus(mouseOver);
				clicked=mouseOver;
			}
			else
			{
				clicked=NULL;
			}
		}

		// 2012/10/27 Memo: SetMouseState must come after LButtonDown and LButtonUp

		if(popUp!=NULL)
		{
			popUp->SetMouseState(lb,mb,rb,mx,my);
		}
		else
		{
			forYsArray(i,itemList)
			{
				if(YSTRUE==itemList[i]->IsEnabled())
				{
					itemList[i]->SetMouseState(lb,mb,rb,mx,my);
				}
			}
		}

		if(prevMouseOver!=mouseOver || prevFocus!=focus)
		{
			needRedraw=YSTRUE;
		}


		if(mx!=pMx || my!=pMy)
		{
			OnMouseMove(lb,mb,rb,mx,my,pLb,pMb,pRb,pMx,pMy,mouseOver);
		}
	}
	else
	{
		mouseOver=NULL;
	}

	pLb=lb;
	pMb=mb;
	pRb=rb;
	pMx=mx;
	pMy=my;

	if(mouseOver!=NULL)
	{
		return YSOK;
	}

	if(popUp!=NULL && popUp->IsInRange(mx,my)==YSTRUE)
	{
		return YSOK;
	}

	return YSERR;
}

YSRESULT FsGuiDialog::SetTouchState(YSSIZE_T nTouch,const class FsVec2i touch[])
{
	auto activeModal=GetActiveModalDialog();
	if(NULL!=activeModal)
	{
		return activeModal->SetTouchState(nTouch,touch);
	}

	if(enabled==YSTRUE)
	{
		YSRESULT res=YSERR;

		auto newTouch=ConvertTouch(nTouch,touch);
		auto trans=MatchTouch(newTouch,prevTouch);

		if(YSTRUE!=pLb)  // Don't fire LButtonUp or LButtonDown if previous left button is on.
		{
			for(auto touch : trans.newTouch)
			{
				auto touchOver=FindVisibleItem(touch.xi(),touch.yi());
				if(nullptr!=touchOver)
				{
					touchOver->LButtonDown(touch.xi(),touch.yi());
				}
			}
			for(auto touch : trans.endedTouch)
			{
				auto touchOver=FindVisibleItem(touch.xi(),touch.yi());
				if(nullptr!=touchOver)
				{
					touchOver->LButtonUp(touch.xi(),touch.yi());
				}
			}
		}

		prevTouch=newTouch;
		return res;
	}
	return YSERR;
}

void FsGuiDialog::ForgetPreviousMouseButtonState(void)
{
	pLb=YSFALSE;
	pMb=YSFALSE;
	pRb=YSFALSE;
}

void FsGuiDialog::SetPopUp(FsGuiDialogItem *popUpItem)
{
	if(popUp!=popUpItem)
	{
		popUp=popUpItem;
		needRedraw=YSTRUE;
	}
}

FsGuiDialogItem *FsGuiDialog::LButtonDown(int mx,int my)
{
	auto activeModal=GetActiveModalDialog();
	if(NULL!=activeModal)
	{
		return activeModal->LButtonDown(mx,my);
	}

	FsGuiDialogItem *itm;
	itm=FindVisibleItem(mx,my);
	if(NULL!=itm && YSTRUE==itm->IsEnabled())
	{
		itm->LButtonDown(mx,my);
		return itm;
	}
	return NULL;
}

FsGuiDialogItem *FsGuiDialog::LButtonUp(int mx,int my)
{
	auto activeModal=GetActiveModalDialog();
	if(NULL!=activeModal)
	{
		return activeModal->LButtonUp(mx,my);
	}

	FsGuiDialogItem *itm;
	itm=FindVisibleItem(mx,my);
	if(NULL!=itm && YSTRUE==itm->IsEnabled())
	{
		itm->LButtonUp(mx,my);

		SetFocus(itm);
		clicked=itm;
		return itm;
	}
	SetFocus(NULL);
	clicked=NULL;
	return NULL;
}

YSRESULT FsGuiDialog::KeyIn(int key,YSBOOL shift,YSBOOL ctrl,YSBOOL alt)
{
	auto activeModal=GetActiveModalDialog();
	if(NULL!=activeModal)
	{
		return activeModal->KeyIn(key,shift,ctrl,alt);
	}

	YSRESULT res;
	res=YSERR;

	OnKeyDown(key,shift,ctrl,alt);

	if(enabled==YSTRUE && key!=FSKEY_NULL)
	{
		int i;

		if(key==FSKEY_SPACE && focus!=NULL && YSTRUE==focus->IsVisible())
		{
			ClickEquivalent(focus);
			clicked=focus;
		}

		if(FSKEY_TAB==key && YSTRUE==ctrl && NULL!=primaryTab)
		{
			if(YSTRUE!=shift)
			{
				primaryTab->SelectNextTab();
			}
			else
			{
				primaryTab->SelectPrevTab();
			}
		}
		else if(FSKEY_TAB==key && itemList.GetN()>0)
		{
			FsGuiDialogItem *prevFocus;
			int current;

			prevFocus=focus;
			current=0;
			forYsArray(i,itemList)
			{
				if(focus==itemList[i])
				{
					current=i;
					break;
				}
			}


			if(shift==YSTRUE)
			{
				for(i=1; i<itemList.GetN(); i++)
				{
					if(YSTRUE==itemList[(current+itemList.GetN()-i)%itemList.GetN()]->IsVisible() &&
					   YSTRUE==itemList[(current+itemList.GetN()-i)%itemList.GetN()]->IsTabStop() &&
					   YSTRUE==itemList[(current+itemList.GetN()-i)%itemList.GetN()]->IsEnabled())
					{
						SetFocus(itemList[(current+itemList.GetN()-i)%itemList.GetN()]);
						SetPopUp(NULL);
						res=YSOK;
						needRedraw=YSTRUE;
						break;
					}
				}
			}
			else
			{
				for(i=1; i<itemList.GetN(); i++)
				{
					if(YSTRUE==itemList[(current+i)%itemList.GetN()]->IsVisible() &&
					   YSTRUE==itemList[(current+i)%itemList.GetN()]->IsTabStop() &&
					   YSTRUE==itemList[(current+i)%itemList.GetN()]->IsEnabled())
					{
						SetFocus(itemList[(current+i)%itemList.GetN()]);
						SetPopUp(NULL);
						res=YSOK;
						needRedraw=YSTRUE;
						break;
					}
				}
			}
		}

		if(focus!=NULL && YSTRUE==focus->IsVisible() && focus->KeyIn(key,shift,ctrl,alt)==YSOK)
		{
			res=YSOK;
		}
		else
		{
			forYsArray(i,itemList)
			{
				if(itemList[i]->fsKey==key && YSTRUE==itemList[i]->IsVisible())
				{
					clicked=itemList[i];
					ClickEquivalent(itemList[i]);
					res=YSOK;
					break;
				}
			}
		}
	}

	return res;
}

YSRESULT FsGuiDialog::CharIn(int c)
{
	auto activeModal=GetActiveModalDialog();
	if(NULL!=activeModal)
	{
		return activeModal->CharIn(c);
	}

	if(c!=0 && enabled==YSTRUE)
	{
		if(focus!=NULL && YSTRUE==focus->IsVisible() && focus->CharIn(c)==YSOK)
		{
			return YSOK;
		}
	}
	return YSERR;
}

/* virtual */ YSRESULT FsGuiDialog::VirtualClick(const YsString &ident)
{
	auto identNumber=FindIdent(ident);
	for(auto itemPtr : itemList)
	{
		if(identNumber==itemPtr->GetIdent())
		{
			ProcessClick(itemPtr);
			return YSOK;
		}
	}
	return YSERR;
}

/* virtual */ YSRESULT FsGuiDialog::VirtualCheck(const YsString &ident)
{
	auto identNumber=FindIdent(ident);
	for(auto itemPtr : itemList)
	{
		if(identNumber==itemPtr->GetIdent())
		{
			auto btnPtr=dynamic_cast <FsGuiButton *>(itemPtr);
			if(nullptr!=btnPtr && YSTRUE!=btnPtr->GetCheck())
			{
				ProcessClick(itemPtr);
				return YSOK;
			}
		}
	}
	return YSERR;
}

/* virtual */ YSRESULT FsGuiDialog::VirtualUncheck(const YsString &ident)
{
	auto identNumber=FindIdent(ident);
	for(auto itemPtr : itemList)
	{
		if(identNumber==itemPtr->GetIdent())
		{
			auto btnPtr=dynamic_cast <FsGuiButton *>(itemPtr);
			if(nullptr!=btnPtr && YSTRUE==btnPtr->GetCheck())
			{
				ProcessClick(itemPtr);
				return YSOK;
			}
		}
	}
	return YSERR;
}

/* virtual */ YSRESULT FsGuiDialog::VirtualSelect(const YsString &ident,const YsString &str)
{
	auto identNumber=FindIdent(ident);
printf("%s %d\n",__FUNCTION__,__LINE__);
	for(auto itemPtr : itemList)
	{
		if(identNumber==itemPtr->GetIdent())
		{
			{
printf("%s %d\n",__FUNCTION__,__LINE__);
				auto listBoxPtr=dynamic_cast <FsGuiListBox *> (itemPtr);
				if(nullptr!=listBoxPtr)
				{
printf("%s %d\n",__FUNCTION__,__LINE__);
					auto prevSel=listBoxPtr->GetSelection();
					listBoxPtr->SelectByString(str);
					if(prevSel!=listBoxPtr->GetSelection())
					{
						OnListBoxSelChange(listBoxPtr,prevSel);
						SetNeedRedraw(YSTRUE);
					}
					return YSOK;
				}
			}
			{
printf("%s %d\n",__FUNCTION__,__LINE__);
				auto dropListPtr=dynamic_cast <FsGuiDropList *> (itemPtr);
				if(nullptr!=dropListPtr)
				{
printf("%s %d\n",__FUNCTION__,__LINE__);
					auto prevSel=dropListPtr->GetSelection();
					dropListPtr->SelectByString(str);
					if(prevSel!=dropListPtr->GetSelection())
					{
						OnDropListSelChange(dropListPtr,prevSel);
						SetNeedRedraw(YSTRUE);
					}
					return YSOK;
				}
			}
		}
	}
printf("%s %d\n",__FUNCTION__,__LINE__);
	return YSERR;
}

/* virtual */ YSRESULT FsGuiDialog::VirtualSelect(const YsString &ident,int n)
{
	auto identNumber=FindIdent(ident);
	for(auto itemPtr : itemList)
	{
		if(identNumber==itemPtr->GetIdent())
		{
			{
				auto listBoxPtr=dynamic_cast <FsGuiListBox *> (itemPtr);
				if(nullptr!=listBoxPtr)
				{
					auto prevSel=listBoxPtr->GetSelection();
					listBoxPtr->Select(n);
					if(prevSel!=listBoxPtr->GetSelection())
					{
						OnListBoxSelChange(listBoxPtr,prevSel);
						SetNeedRedraw(YSTRUE);
					}
					return YSOK;
				}
			}
			{
				auto dropListPtr=dynamic_cast <FsGuiDropList *> (itemPtr);
				if(nullptr!=dropListPtr)
				{
					auto prevSel=dropListPtr->GetSelection();
					dropListPtr->Select(n);
					if(prevSel!=dropListPtr->GetSelection())
					{
						OnDropListSelChange(dropListPtr,prevSel);
						SetNeedRedraw(YSTRUE);
					}
					return YSOK;
				}
			}
		}
	}
	return YSERR;
}

void FsGuiDialog::ProcessClick(FsGuiDialogItem *item)
{
	ClickEquivalent(item);
	clicked=item;
	SetFocus(item);
}

FsGuiDialogItem *FsGuiDialog::GetClickedItem(void)
{
	FsGuiDialogItem *itm;
	itm=clicked;
	clicked=NULL;
	return itm;
}

void FsGuiDialog::SetNeedRedraw(YSBOOL needRedraw)
{
	this->needRedraw=needRedraw;
}

YSBOOL FsGuiDialog::NeedRedraw(void) const
{
	if(NULL!=GetModalDialog() && YSTRUE==GetModalDialog()->NeedRedraw())
	{
		return YSTRUE;
	}

	if(needRedraw==YSTRUE)
	{
		return YSTRUE;
	}

	if(focus!=NULL && focus->NeedRedrawActiveItem()==YSTRUE)
	{
		return YSTRUE;
	}

	return YSFALSE;
}

void FsGuiDialog::StartModalDialog(void)
{
	this->endModal=YSFALSE;
}

YSRESULT FsGuiDialog::CloseModalDialog(int returnCode)
{
	YSRESULT res=YSERR;
	FsDisableIME();

	// For legacy support >>
	this->endModal=YSTRUE;
	this->result=(YSRESULT)returnCode;
	// For legacy support <<

	FsGuiControlBase *parent=GetParent();
	if(nullptr!=parent)
	{
		parent->DetachModalDialog();
		// 2012/10/14 Memo
		//   This dialog needs to be detached before OnModalDialogClosed is called.
		//   In OnModalDialogClosed, the call back function may attach a new dialog,
		//   which may be a modified version of the same dialog.  (Eg. Asking Yes/No
		//   in a sequence)
		//   Therefore, if the modal dialog is detached after OnModalDialogClosed,
		//   New modal dialog assigned in OnModalDialogClosed may be wiped.  That's
		//   why DetachModalDialog must come before OnModalDialogClosed.
	}

	// 2015/08/09
	//   OnModalDialogClose may clear all dialog's own close-modal call back so that next use of the same dialog
	//   will not accidentally fire previously assigned call back.
	//   For those purposes, stdCloseModalCallBack and closeModalCallBackFunc must be called before OnModalDialogClosed.
	if(true==(bool)stdCloseModalCallBack)
	{
		stdCloseModalCallBack(returnCode);
	}

	if(NULL!=closeModalCallBackFunc)
	{
		(*closeModalCallBackFunc)(parent,this,returnCode);
	}

	if(NULL!=parent)
	{
		parent->OnModalDialogClosed(modalDialogIdent,this,returnCode);
		res=YSOK;
	}

	return res;
}

void FsGuiDialog::HideAll(void)
{
	for(auto itm : itemList)
	{
		itm->Hide();
	}
	SetNeedRedraw(YSTRUE);
}
void FsGuiDialog::ShowAll(void)
{
	for(auto itm : itemList)
	{
		itm->Show();
	}
	for(auto itm : itemList)
	{
		auto tab=dynamic_cast <FsGuiTabControl *>(itm);
		if(NULL!=tab)
		{
			tab->Refresh();
		}
	}
	SetNeedRedraw(YSTRUE);
}

YSBOOL FsGuiDialog::GetEndModal(void) const
{
	return endModal;
}

YSRESULT FsGuiDialog::GetResult(void) const
{
	return result;
}

YSBOOL FsGuiDialog::MayEatThisKeyStroke(int fskey,YSBOOL ,YSBOOL ,YSBOOL alt) const
{
	if(NULL!=focus)
	{
		switch(focus->GetItemType())
		{
		default:
			break;
		case FSGUI_TEXTBOX:
			if(FSKEY_NULL!=fskey && YSTRUE!=alt)
			{
				return YSTRUE;
			}
			break;
		case FSGUI_LISTBOX:
		case FSGUI_DROPLIST:
			if(FSKEY_0<=fskey && fskey<=FSKEY_9)
			{
				return YSTRUE;
			}
			if(FSKEY_A<=fskey && fskey<=FSKEY_Z)
			{
				return YSTRUE;
			}
			if(FSKEY_UP==fskey || FSKEY_DOWN==fskey)
			{
				return YSTRUE;
			}
			break;
		case FSGUI_NUMBERBOX:
			if(FSKEY_UP==fskey || FSKEY_DOWN==fskey)
			{
				return YSTRUE;
			}
			break;
		}
	}
	return YSFALSE;
}

/* virtual */ void FsGuiDialog::OnAttach(void)
{
}

/* virtual */ void FsGuiDialog::OnDetach(void)
{
}

void FsGuiDialog::OnKeyDown(int /*fsKey*/ ,YSBOOL /*shift*/ ,YSBOOL /*ctrl*/ ,YSBOOL /*alt*/)
{
}

void FsGuiDialog::OnButtonClick(FsGuiButton * /*btn*/)
{
}

void FsGuiDialog::OnListBoxSelChange(FsGuiListBox * /*lbx*/,int /*prevSel*/)
{
}

void FsGuiDialog::OnListBoxScroll(FsGuiListBox * /*lbx*/,int /*prevShowTop*/)
{
}

void FsGuiDialog::OnListBoxDoubleClick(FsGuiListBox * /*lbx*/,int /*clickedOn*/)
{
}

void FsGuiDialog::OnDropListSelChange(FsGuiDropList * /*drp*/,int /*prevSel*/)
{
}

void FsGuiDialog::OnNumberBoxChange(FsGuiNumberBox * /*nbx*/,int /*prevNum*/)
{
}

void FsGuiDialog::OnTextBoxChange(FsGuiTextBox * /*txt*/)
{
}

void FsGuiDialog::OnTextBoxSpecialKey(FsGuiTextBox * /*txt*/,int /*fskey*/)
{
}

void FsGuiDialog::OnMouseMove(
    YSBOOL /*lb*/,YSBOOL /*mb*/,YSBOOL /*rb*/,int /*mx*/,int /*my*/,
    YSBOOL /*plb*/,YSBOOL /*pmb*/,YSBOOL /*prb*/,int /*pmx*/,int /*pmy*/,
    FsGuiDialogItem * /*mouseOver*/)
{
}

void FsGuiDialog::OnSliderPositionChange(FsGuiSlider * /*slider*/,const double & /*prevPos*/,const double & /*prevValue*/)
{
}

void FsGuiDialog::OnSelectTab(FsGuiTabControl * /*tab*/,int /*newTab*/)
{
}

void FsGuiDialog::OnTreeControlSelChange(FsGuiTreeControl *)
{
}

void FsGuiDialog::OnTreeControlScroll(FsGuiTreeControl *)
{
}

void FsGuiDialog::OnTreeControlNodeAboutToMove(FsGuiTreeControl *,int ,const FsGuiTreeControlNode * const [])
{
}

void FsGuiDialog::OnTreeControlNodeMoved(FsGuiTreeControl *,int ,const FsGuiTreeControlNode * const [])
{
}

void FsGuiDialog::OnColorPaletteChange(FsGuiColorPalette *)
{
}

void FsGuiDialog::SetRadioButtonGroup(YSSIZE_T nBtn,FsGuiButton *btn[])
{
	for(int i=0; i<nBtn; i++)
	{
		btn[i]->btnGroup.Set(nBtn,btn);
	}
}
void FsGuiDialog::SetRadioButtonGroup(YsArrayMask <FsGuiButton *> btn)
{
	SetRadioButtonGroup(btn.GetN(),btn);
}

void FsGuiDialog::InsertVerticalSpace(int hei)
{
	int fontWid,fontHei;
	GetAsciiCharacterBitmapSize(fontWid,fontHei);

	if(autoNewColumn==YSTRUE && lastY+hei>dlgY0+dlgHei-fontHei)
	{
		StepToNextColumn();
	}

	lastY+=hei;
}

FsGuiStatic *FsGuiDialog::AddStaticText(int id,int fsKey,const char label[],int nWidth,int nLine,YSBOOL newLine)
{
	YsListItem <FsGuiStatic> *neo=sttList.Create();
	neo->dat.Initialize(this);

	YsWString wLabel;
	wLabel.SetUTF8String(label);

	int fontWid=8,fontHei=8;
	YsBitmap wholeBmp;
	if(YSOK==RenderUnicodeString(wholeBmp,wLabel,defFgCol,defBgCol))
	{
		neo->dat.bmp.SetBitmap(wholeBmp);
		GetUnicodeCharacterBitmapSize(fontWid,fontHei);
	}
	else if(YSOK==RenderAsciiString(wholeBmp,label,defFgCol,defBgCol))
	{
		neo->dat.bmp.SetBitmap(wholeBmp);
		GetAsciiCharacterBitmapSize(fontWid,fontHei);
	}
	else
	{
		neo->dat.bmp.MakeSmallEmptyBitmap(); // At least crash must be prevented.
	}

	const int msgWid=YsGreater(nWidth*fontWid,(int)neo->dat.bmp.GetWidth());
	const int msgHei=YsGreater(nLine*fontHei,(int)neo->dat.bmp.GetHeight());

	neo->dat.wLabel=wLabel;
	SetUpStatic(&neo->dat,msgWid,msgHei,id,fsKey,newLine);

	return &neo->dat;
}

FsGuiStatic *FsGuiDialog::AddStaticText(int id,int fsKey,const char label[],YSBOOL newLine)
{
	return AddStaticText(id,fsKey,label,0,0,newLine);
}

FsGuiStatic *FsGuiDialog::AddStaticText(int id,int fsKey,const wchar_t wLabel[],int nWidth,int nLine,YSBOOL newLine)
{
	YsListItem <FsGuiStatic> *neo=sttList.Create();
	neo->dat.Initialize(this);

	YsWString wStr;
	if(NULL!=wLabel && 0!=wLabel[0])
	{
		wStr.Set(wLabel);
	}
	else
	{
		const wchar_t space[]={L" "}; // Workaround to cope with 64-bit GCC bug in Mac OSX
		wStr.Set(space);
	}

	int msgWid=0,msgHei=0,fontWid=8,fontHei=8;
	YsBitmap wholeBmp;
	if(YSOK==RenderUnicodeString(wholeBmp,wLabel,defFgCol,defBgCol))
	{
		neo->dat.bmp.SetBitmap(wholeBmp);
		GetUnicodeCharacterBitmapSize(fontWid,fontHei);
	}
	else if(YSOK==RenderAsciiString(wholeBmp,wLabel,defFgCol,defBgCol))
	{
		neo->dat.bmp.SetBitmap(wholeBmp);
		GetAsciiCharacterBitmapSize(fontWid,fontHei);
	}
	else
	{
		neo->dat.bmp.MakeSmallEmptyBitmap(); // At least crash must be prevented.
	}

	msgWid=neo->dat.bmp.GetWidth();
	msgHei=neo->dat.bmp.GetHeight();

	msgWid=YsGreater(nWidth*fontWid,msgWid);
	msgHei=YsGreater(nLine*fontHei,msgHei);

	neo->dat.wLabel=wLabel;
	SetUpStatic(&neo->dat,msgWid,msgHei,id,fsKey,newLine);

	return &neo->dat;
}

FsGuiStatic *FsGuiDialog::AddStaticText(int id,int fsKey,const wchar_t wLabel[],YSBOOL newLine)
{
	return AddStaticText(id,fsKey,wLabel,0,0,newLine);
}

FsGuiStatic *FsGuiDialog::AddStaticBmp(int id,int fsKey,const YsBitmap &bmpIn,YSBOOL newLine)
{
	YsListItem <FsGuiStatic> *neo=sttList.Create();
	neo->dat.Initialize(this);

	neo->dat.bmp.SetBitmap(bmpIn);
	SetUpStatic(&neo->dat,bmpIn.GetWidth(),bmpIn.GetHeight(),id,fsKey,newLine);

	return &neo->dat;
}

void FsGuiDialog::SetUpStatic(FsGuiStatic *item,int msgWid,int msgHei,int id,int fsKey,YSBOOL newLine)
{
	const int itemWid=msgWid+defHSpaceUnit;
	const int itemHei=msgHei+defVSpaceUnit/2;

	PlaceNewItem(item,itemWid,itemHei,newLine);

	item->SetDrawFrame(YSFALSE);
	item->SetFill(YSFALSE);
	item->SetNewLineWhenCreated(newLine);

	item->id=id;
	item->fsKey=fsKey;

	itemList.Append(item);
}




FsGuiButton *FsGuiDialog::AddTextButton(int id,int fsKey,FSGUIBUTTONTYPE btnType,const char label[],YSBOOL newLine)
{
	YsListItem <FsGuiButton> *neo=btnList.Create();
	neo->dat.Initialize(this);

	int msgWid,msgHei;
	if(NULL!=label && 0!=label[0])
	{
		YsWString wStr;
		wStr.SetUTF8String(label);
		YsBitmap wholeBmp;
		if(YSOK==RenderUnicodeString(wholeBmp,wStr,defFgCol,defBgCol) ||
		   YSOK==RenderAsciiString(wholeBmp,label,defFgCol,defBgCol))
		{
			neo->dat.bmp.SetBitmap(wholeBmp);
		}
		else
		{
			neo->dat.bmp.MakeSmallEmptyBitmap(); // At least crash must be prevented.
		}
	}

	msgWid=neo->dat.bmp.GetWidth();
	msgHei=neo->dat.bmp.GetHeight();
	neo->dat.isText=YSTRUE;
	YsWString wStr;
	wStr.SetUTF8String(label);
	SetUpButton(&neo->dat,msgWid,msgHei,btnType,id,fsKey,wStr,newLine);

	return &neo->dat;
}

FsGuiButton *FsGuiDialog::AddTextButton(int id,int fsKey,FSGUIBUTTONTYPE btnType,const wchar_t wLabel[],YSBOOL newLine)
{
	YsListItem <FsGuiButton> *neo=btnList.Create();
	neo->dat.Initialize(this);

	YsWString wStr;
	if(NULL!=wLabel && 0!=wLabel[0])
	{
		wStr.Set(wLabel);
	}
	else
	{
		const wchar_t space[]={L" "}; // Workaround to cope with 64-bit GCC bug in Mac OSX
		wStr.Set(space);
	}

	int msgWid,msgHei;
	YsBitmap wholeBmp;
	if(YSOK==RenderUnicodeString(wholeBmp,wStr,defFgCol,defBgCol) ||
	   YSOK==RenderAsciiString(wholeBmp,wStr,defFgCol,defBgCol))
	{
		neo->dat.bmp.SetBitmap(wholeBmp);
	}
	else
	{
		neo->dat.bmp.MakeSmallEmptyBitmap(); // At least crash must be prevented.
	}

	msgWid=neo->dat.bmp.GetWidth();
	msgHei=neo->dat.bmp.GetHeight();
	neo->dat.isText=YSTRUE;
	SetUpButton(&neo->dat,msgWid,msgHei,btnType,id,fsKey,wStr,newLine);

	return &neo->dat;
}

FsGuiButton *FsGuiDialog::AddBmpButton(int id,int fsKey,FSGUIBUTTONTYPE btnType,const YsBitmap &bmpIn,const wchar_t altLabel[],YSBOOL newLine)
{
	YsListItem <FsGuiButton> *neo=btnList.Create();
	neo->dat.Initialize(this);

	const int msgWid=bmpIn.GetWidth();
	const int msgHei=bmpIn.GetHeight();
	neo->dat.bmp.SetBitmap(bmpIn);
	neo->dat.isText=YSFALSE;
	SetUpButton(&neo->dat,msgWid,msgHei,btnType,id,fsKey,altLabel,newLine);
	return &neo->dat;

}

void FsGuiDialog::SetUpButton(FsGuiButton *item,int msgWid,int msgHei,FSGUIBUTTONTYPE btnType,int id,int fsKey,const wchar_t label[],YSBOOL newLine)
{
	item->btnType=btnType;

	int itemWid,itemHei;
	item->CalculateButtonSizeFromMessageSize(itemWid,itemHei,msgWid,msgHei);

	PlaceNewItem(item,itemWid,itemHei,newLine);
	SetIdKeyLabel(item,id,fsKey,label);

	itemList.Append(item);
}



FsGuiTextBox *FsGuiDialog::AddTextBox(int id,int fsKey,const char label[],int nShow,YSBOOL newLine)
{
	return AddTextBox(id,fsKey,FsGuiTextBox::VERTICAL,label,nShow,newLine);
}

FsGuiTextBox *FsGuiDialog::AddTextBox(int id,int fsKey,const wchar_t label[],int nShow,YSBOOL newLine)
{
	return AddTextBox(id,fsKey,FsGuiTextBox::VERTICAL,label,nShow,newLine);
}

FsGuiTextBox *FsGuiDialog::AddBmpTextBox(int id,int fsKey,const YsBitmap &bmpIn,const char label[],int nShow,YSBOOL newLine)
{
	return AddBmpTextBox(id,fsKey,FsGuiTextBox::VERTICAL,bmpIn,label,nShow,newLine);
}

FsGuiTextBox *FsGuiDialog::AddTextBox(int id,int fsKey,const char label[],const char defTxt[],int nShow,YSBOOL newLine)
{
	return AddTextBox(id,fsKey,FsGuiTextBox::VERTICAL,label,defTxt,nShow,newLine);
}

FsGuiTextBox *FsGuiDialog::AddTextBox(int id,int fsKey,const wchar_t label[],const char defTxt[],int nShow,YSBOOL newLine)
{
	return AddTextBox(id,fsKey,FsGuiTextBox::VERTICAL,label,defTxt,nShow,newLine);
}

FsGuiTextBox *FsGuiDialog::AddBmpTextBox(int id,int fsKey,const YsBitmap &bmpIn,const char label[],const char defTxt[],int nShow,YSBOOL newLine)
{
	return AddBmpTextBox(id,fsKey,FsGuiTextBox::VERTICAL,bmpIn,label,defTxt,nShow,newLine);
}

FsGuiTextBox *FsGuiDialog::AddTextBox(int id,int fsKey,const char label[],const wchar_t defTxt[],int nShow,YSBOOL newLine)
{
	return AddTextBox(id,fsKey,FsGuiTextBox::VERTICAL,label,defTxt,nShow,newLine);
}

FsGuiTextBox *FsGuiDialog::AddTextBox(int id,int fsKey,const wchar_t label[],const wchar_t defTxt[],int nShow,YSBOOL newLine)
{
	return AddTextBox(id,fsKey,FsGuiTextBox::VERTICAL,label,defTxt,nShow,newLine);
}

FsGuiTextBox *FsGuiDialog::AddBmpTextBox(int id,int fsKey,const YsBitmap &bmpIn,const char label[],const wchar_t defTxt[],int nShow,YSBOOL newLine)
{
	return AddBmpTextBox(id,fsKey,FsGuiTextBox::VERTICAL,bmpIn,label,defTxt,nShow,newLine);
}




FsGuiTextBox *FsGuiDialog::AddTextBox(int id,int fsKey,FsGuiTextBox::LAYOUT layout,const char label[],int nShow,YSBOOL newLine)
{
	return AddTextBox(id,fsKey,layout,label,"",nShow,newLine);
}

FsGuiTextBox *FsGuiDialog::AddTextBox(int id,int fsKey,FsGuiTextBox::LAYOUT layout,const wchar_t label[],int nShow,YSBOOL newLine)
{
	return AddTextBox(id,fsKey,layout,label,"",nShow,newLine);
}

FsGuiTextBox *FsGuiDialog::AddBmpTextBox(int id,int fsKey,FsGuiTextBox::LAYOUT layout,const YsBitmap &bmpIn,const char label[],int nShow,YSBOOL newLine)
{
	return AddBmpTextBox(id,fsKey,layout,bmpIn,label,"",nShow,newLine);
}

FsGuiTextBox *FsGuiDialog::AddTextBox(int id,int fsKey,FsGuiTextBox::LAYOUT layout,const char label[],const char defTxt[],int nShow,YSBOOL newLine)
{
	YsWString wStr;
	wStr.SetUTF8String(defTxt);
	return AddTextBox(id,fsKey,layout,label,wStr,nShow,newLine);
}

FsGuiTextBox *FsGuiDialog::AddTextBox(int id,int fsKey,FsGuiTextBox::LAYOUT layout,const wchar_t label[],const char defTxt[],int nShow,YSBOOL newLine)
{
	YsWString wStr;
	wStr.SetUTF8String(defTxt);
	return AddTextBox(id,fsKey,layout,label,wStr,nShow,newLine);
}

FsGuiTextBox *FsGuiDialog::AddBmpTextBox(int id,int fsKey,FsGuiTextBox::LAYOUT layout,const YsBitmap &bmpIn,const char label[],const char defTxt[],int nShow,YSBOOL newLine)
{
	YsWString wStr;
	wStr.SetUTF8String(defTxt);
	return AddBmpTextBox(id,fsKey,layout,bmpIn,label,wStr,nShow,newLine);
}

FsGuiTextBox *FsGuiDialog::AddTextBox(int id,int fsKey,FsGuiTextBox::LAYOUT layout,const char label[],const wchar_t defTxt[],int nShow,YSBOOL newLine)
{
	YsWString wStr;
	wStr.SetUTF8String(label);
	return AddTextBox(id,fsKey,layout,wStr,defTxt,nShow,newLine);
}

FsGuiTextBox *FsGuiDialog::AddTextBox(int id,int fsKey,FsGuiTextBox::LAYOUT layout,const wchar_t wLabel[],const wchar_t defTxt[],int nShow,YSBOOL newLine)
{
	YsListItem <FsGuiTextBox> *neo=txtList.Create();
	neo->dat.Initialize(this);

	int msgWid=0,msgHei=0;
	YsBitmap wholeBmp;
	if(NULL!=wLabel && 0!=wLabel[0] && 
	   (YSOK==RenderUnicodeString(wholeBmp,wLabel,defFgCol,defBgCol) ||
	    YSOK==RenderAsciiString(wholeBmp,wLabel,defFgCol,defBgCol)))
	{
		msgWid=wholeBmp.GetWidth();
		msgHei=wholeBmp.GetHeight();
		neo->dat.bmp.SetBitmap(wholeBmp);
	}

	SetUpTextBox(&neo->dat,msgWid,msgHei,defTxt,nShow,layout,newLine);
	SetIdKeyLabel(&neo->dat,id,fsKey,wLabel);

	return &neo->dat;
}

FsGuiTextBox *FsGuiDialog::AddBmpTextBox(int id,int fsKey,FsGuiTextBox::LAYOUT layout,const YsBitmap &bmpIn,const char altLabel[],const wchar_t defTxt[],int nShow,YSBOOL newLine)
{
	YsListItem <FsGuiTextBox> *neo=txtList.Create();
	neo->dat.Initialize(this);

	const int msgWid=bmpIn.GetWidth();
	const int msgHei=bmpIn.GetHeight();

	neo->dat.bmp.SetBitmap(bmpIn);
	SetUpTextBox(&neo->dat,msgWid,msgHei,defTxt,nShow,layout,newLine);
	SetIdKeyLabel(&neo->dat,id,fsKey,altLabel);

	return &neo->dat;
}

void FsGuiDialog::SetUpTextBox(FsGuiTextBox *item,int msgWid,int msgHei,const wchar_t defTxt[],int nShow,FsGuiTextBox::LAYOUT layout,YSBOOL newLine)
{
	int fontWid,fontHei;
	if(YSOK!=GetUnicodeCharacterBitmapSize(fontWid,fontHei))
	{
		GetAsciiCharacterBitmapSize(fontWid,fontHei);
	}

	item->typeWid=fontWid*nShow;
	item->typeHei=fontHei;

	item->inputWid=item->typeWid+fontWid*4/10;
	item->inputHei=item->typeHei+fontHei*4/10;

	item->msgX0=defHSpaceUnit/2;
	item->msgY0=defVSpaceUnit/2;
	item->msgWid=msgWid;
	item->msgHei=msgHei;

	int itemWid=0,itemHei=0;

	if(0>=msgWid || 0>=msgHei) // Layout doesn't matter.
	{
		item->inputX0=defHSpaceUnit/2;
		item->inputY0=defVSpaceUnit/4;

		itemWid=YsGreater(item->inputWid,item->msgWid)+defHSpaceUnit;
		itemHei=defVSpaceUnit/2+item->inputHei;
	}
	else if(FsGuiTextBox::VERTICAL==layout)
	{
		item->inputX0=defHSpaceUnit/2;
		item->inputY0=defVSpaceUnit+item->msgHei;

		itemWid=YsGreater(item->inputWid,item->msgWid)+defHSpaceUnit;
		itemHei=defVSpaceUnit*3/2+item->msgHei+item->inputHei;
	}
	else // HORIZONTAL
	{
		item->inputX0=item->msgX0+msgWid+defHSpaceUnit/2;
		item->inputY0=defVSpaceUnit/4;

		itemWid=item->msgWid+item->inputWid+defHSpaceUnit*3/2;
		itemHei=defVSpaceUnit/2+YsGreater(item->msgHei,item->inputHei);
	}

	item->typeX0=item->inputX0+fontWid*2/10;
	item->typeY0=item->inputY0+fontHei*2/10;

	PlaceNewItem(item,itemWid,itemHei,newLine);

	itemList.Append(item);

	item->txt.Set(defTxt);
	item->lngLimit=nShow;
	item->nShow=nShow;

	item->UpdateShowTop();
	item->UpdatePixOffset();
	item->RemakeBitmap();
}



FsGuiListBox *FsGuiDialog::AddEmptyListBox(
   int id,int fsKey,const char label[],int nShow,int nWidth,YSBOOL newLine)
{
	return AddListBoxTemplate <wchar_t> (id,fsKey,label,0,NULL,nShow,nWidth,newLine);
}

FsGuiListBox *FsGuiDialog::AddEmptyListBox(
   int id,int fsKey,const wchar_t wLabel[],int nShow,int nWidth,YSBOOL newLine)
{
	return AddListBoxTemplate <wchar_t> (id,fsKey,wLabel,0,NULL,nShow,nWidth,newLine);
}

FsGuiListBox *FsGuiDialog::AddEmptyBmpListBox(
   int id,int fsKey,const YsBitmap &bmpIn,const wchar_t altLabel[],int nShow,int nWidth,YSBOOL newLine)
{
	return AddBmpListBoxTemplate <wchar_t> (id,fsKey,bmpIn,altLabel,0,NULL,nShow,nWidth,newLine);
}

FsGuiListBox *FsGuiDialog::AddListBox(
   int id,int fsKey,const char label[],YSSIZE_T nChoice,const char *const choice[],int nShow,int nWidth,YSBOOL newLine)
{
	return AddListBoxTemplate <char> (id,fsKey,label,nChoice,choice,nShow,nWidth,newLine);
}

FsGuiListBox *FsGuiDialog::AddListBox(
   int id,int fsKey,const wchar_t wLabel[],YSSIZE_T nChoice,const char *const choice[],int nShow,int nWidth,YSBOOL newLine)
{
	return AddListBoxTemplate <char> (id,fsKey,wLabel,nChoice,choice,nShow,nWidth,newLine);
}

FsGuiListBox *FsGuiDialog::AddBmpListBox(
   int id,int fsKey,const YsBitmap &bmpIn,const wchar_t altLabel[],YSSIZE_T nChoice,const char *const choice[],int nShow,int nWidth,YSBOOL newLine)
{
	return AddBmpListBoxTemplate <char> (id,fsKey,bmpIn,altLabel,nChoice,choice,nShow,nWidth,newLine);
}

FsGuiListBox *FsGuiDialog::AddListBox(
   int id,int fsKey,const char label[],YSSIZE_T nChoice,const wchar_t *const choice[],int nShow,int nWidth,YSBOOL newLine)
{
	return AddListBoxTemplate <wchar_t> (id,fsKey,label,nChoice,choice,nShow,nWidth,newLine);
}

FsGuiListBox *FsGuiDialog::AddListBox(
   int id,int fsKey,const wchar_t wLabel[],YSSIZE_T nChoice,const wchar_t *const choice[],int nShow,int nWidth,YSBOOL newLine)
{
	return AddListBoxTemplate <wchar_t> (id,fsKey,wLabel,nChoice,choice,nShow,nWidth,newLine);
}

FsGuiListBox *FsGuiDialog::AddBmpListBox(
   int id,int fsKey,const YsBitmap &bmpIn,const wchar_t altLabel[],YSSIZE_T nChoice,const wchar_t *const choice[],int nShow,int nWidth,YSBOOL newLine)
{
	return AddBmpListBoxTemplate <wchar_t> (id,fsKey,bmpIn,altLabel,nChoice,choice,nShow,nWidth,newLine);
}


template <class CHARTYPE>
FsGuiListBox *FsGuiDialog::AddListBoxTemplate(
   int id,int fsKey,const char label[],YSSIZE_T nChoice,const CHARTYPE *const choice[],int nShow,int nWidth,YSBOOL newLine)
{
	YsWString wStr;
	wStr.SetUTF8String(label);
	return AddListBoxTemplate(id,fsKey,wStr,nChoice,choice,nShow,nWidth,newLine);
}

template <class CHARTYPE>
FsGuiListBox *FsGuiDialog::AddListBoxTemplate(
   int id,int fsKey,const wchar_t wLabel[],YSSIZE_T nChoice,const CHARTYPE *const choice[],int nShow,int nWidth,YSBOOL newLine)
{
	YsListItem <FsGuiListBox> *neo=lstList.Create();
	neo->dat.Initialize(this);

	int msgWid=0,msgHei=0;

	YsBitmap wholeBmp;
	if(NULL!=wLabel && 0!=wLabel[0] && 
	   (YSOK==RenderUnicodeString(wholeBmp,wLabel,defFgCol,defBgCol) ||
	    YSOK==RenderAsciiString(wholeBmp,wLabel,defFgCol,defBgCol)))
	{
		neo->dat.bmp.SetBitmap(wholeBmp);
		msgWid=wholeBmp.GetWidth();
		msgHei=wholeBmp.GetHeight();
	}

	SetUpListBox(&neo->dat,msgWid,msgHei,nChoice,choice,nShow,nWidth,newLine);
	SetIdKeyLabel(&neo->dat,id,fsKey,wLabel);

	return &neo->dat;
}

template <class CHARTYPE>
FsGuiListBox *FsGuiDialog::AddBmpListBoxTemplate(
   int id,int fsKey,const YsBitmap &bmpIn,const wchar_t altLabel[],YSSIZE_T nChoice,const CHARTYPE *const choice[],int nShow,int nWidth,YSBOOL newLine)
{
	YsListItem <FsGuiListBox> *neo=lstList.Create();
	neo->dat.Initialize(this);

	neo->dat.bmp.SetBitmap(bmpIn);
	const int msgWid=bmpIn.GetWidth();
	const int msgHei=bmpIn.GetHeight();

	SetUpListBox(&neo->dat,msgWid,msgHei,nChoice,choice,nShow,nWidth,newLine);
	SetIdKeyLabel(&neo->dat,id,fsKey,altLabel);

	return &neo->dat;
}

template <class CHARTYPE>
void FsGuiDialog::SetUpListBox(FsGuiListBox *item,int msgWid,int msgHei,YSSIZE_T nChoice,const CHARTYPE *const choice[],int nShow,int nWidth,YSBOOL newLine)
{
	item->msgWid=msgWid;
	item->msgHei=msgHei;

	int fontWid,fontHei;
	GetUnicodeCharacterBitmapSize(fontWid,fontHei);
	if(0==fontHei)
	{
		GetAsciiCharacterBitmapSize(fontWid,fontHei);
	}

	const int selWid=fontWid*nWidth+defHScrollBar;
	const int selHei=fontHei*nShow;

	const int itemWid=YsGreater(selWid,msgWid)+defHSpaceUnit*2;

	int itemHei;
	if(0<msgHei)
	{
		itemHei=selHei+defVSpaceUnit*3/2+msgHei;
	}
	else
	{
		itemHei=selHei+defVSpaceUnit;
	}

	PlaceNewItem(item,itemWid,itemHei,newLine);

	item->showHeightPix=nShow*fontHei;
	item->nShowWidth=nWidth;
	item->showTopPix=0;
	item->SetChoice(nChoice,choice);

	itemList.Append(item);
}

FsGuiNumberBox *FsGuiDialog::AddNumberBox(
   int id,int fsKey,const char label[],int labelWidth,int num,int min,int max,int step,YSBOOL newLine)
{
	YsWString wStr;
	wStr.SetUTF8String(label);
	return AddNumberBox(id,fsKey,wStr,labelWidth,num,min,max,step,newLine);
}

FsGuiNumberBox *FsGuiDialog::AddNumberBox(
   int id,int fsKey,const wchar_t wLabel[],int labelWidth,int num,int min,int max,int step,YSBOOL newLine)
{
	YsListItem <FsGuiNumberBox> *neo=nbxList.Create();
	neo->dat.Initialize(this);

	int msgWid=0,msgHei=0,fontWid=0,fontHei=0;
	if(NULL!=wLabel && 0!=wLabel[0])
	{
		YsBitmap wholeBmp;
		if(YSOK==RenderUnicodeString(wholeBmp,wLabel,defFgCol,defBgCol))
		{
			neo->dat.bmp.SetBitmap(wholeBmp);
			GetUnicodeCharacterBitmapSize(fontWid,fontHei);
		}
		else if(YSOK==RenderAsciiString(wholeBmp,wLabel,defFgCol,defBgCol))
		{
			neo->dat.bmp.SetBitmap(wholeBmp);
			GetAsciiCharacterBitmapSize(fontWid,fontHei);
		}
	}

	msgWid=YsGreater <int> (neo->dat.bmp.GetWidth(),fontWid*labelWidth);
	msgHei=YsGreater <int> (neo->dat.bmp.GetHeight(),fontHei);

	SetUpNumberBox(&neo->dat,msgWid,msgHei,labelWidth,num,min,max,step,newLine);
	SetIdKeyLabel(&neo->dat,id,fsKey,wLabel);

	return &neo->dat;
}

FsGuiNumberBox *FsGuiDialog::AddBmpNumberBox(
   int id,int fsKey,const YsBitmap &bmpIn,const char altLabel[],int labelWidth,int num,int min,int max,int step,YSBOOL newLine)
{
	YsListItem <FsGuiNumberBox> *neo=nbxList.Create();
	neo->dat.Initialize(this);

	int fontWid=0,fontHei=0;
	if(YSOK!=GetUnicodeCharacterBitmapSize(fontWid,fontHei))
	{
		GetAsciiCharacterBitmapSize(fontWid,fontHei);
	}

	const int msgWid=YsGreater <int> (bmpIn.GetWidth(),fontWid*labelWidth);
	const int msgHei=YsGreater <int> (bmpIn.GetHeight(),fontHei);

	neo->dat.bmp.SetBitmap(bmpIn);

	SetUpNumberBox(&neo->dat,msgWid,msgHei,labelWidth,num,min,max,step,newLine);
	SetIdKeyLabel(&neo->dat,id,fsKey,altLabel);

	return &neo->dat;
}

void FsGuiDialog::SetUpNumberBox(FsGuiNumberBox *item,int msgWid,int msgHei,int labelWidth,int num,int min,int max,int step,YSBOOL newLine)
{
	item->msgWid=msgWid;
	item->msgHei=msgHei;

	item->digitWid=0;
	item->digitHei=0;

	YsString digitTest;
	digitTest.Printf("%d",min);
	for(int i=0; i<2; i++)
	{
		for(int j=0; j<10; j++)
		{
			for(int k=0; k<digitTest.Strlen(); k++)
			{
				if('0'<=digitTest[k] && digitTest[k]<='9')
				{
					digitTest.Set(k,'0'+(char)j);
				}
			}
			int txtWid,txtHei;
			GetAsciiStringBitmapSize(txtWid,txtHei,digitTest);
			item->digitWid=YsGreater(item->digitWid,txtWid);
			item->digitHei=YsGreater(item->digitHei,txtHei);
		}
		digitTest.Printf("%d",max);
	}

	const int arrowSize=YsGreater(item->msgHei,item->digitHei);

	int itemWid;
	if(0<msgWid && 0<msgHei)
	{
		itemWid=defHSpaceUnit*3+msgWid+item->digitWid+arrowSize*2;
	}
	else
	{
		itemWid=defHSpaceUnit*5/2+item->digitWid+arrowSize*2;
	}
	const int itemHei=arrowSize+defVSpaceUnit;

	PlaceNewItem(item,itemWid,itemHei,newLine);

	item->num=num;
	item->min=min;
	item->max=max;
	item->step=step;
	item->labelWidth=labelWidth;

	itemList.Append(item);
}

FsGuiDropList *FsGuiDialog::AddEmptyDropList(
   int id,int fsKey,const char label[],int nShow,int nWidth,int nLstWidth,YSBOOL newLine)
{
	YsWString wLabel;
	wLabel.SetUTF8String(label);
	return AddDropListTemplate <char> (id,fsKey,wLabel,0,NULL,nShow,nWidth,nLstWidth,newLine);
}

FsGuiDropList *FsGuiDialog::AddDropList(
   int id,int fsKey,const char label[],YSSIZE_T nChoice,const char *const choice[],int nShow,int nWidth,int nLstWidth,YSBOOL newLine)
{
	YsWString wLabel;
	wLabel.SetUTF8String(label);
	return AddDropListTemplate <char> (id,fsKey,wLabel,nChoice,choice,nShow,nWidth,nLstWidth,newLine);
}

FsGuiDropList *FsGuiDialog::AddDropList(
   int id,int fsKey,const char label[],YSSIZE_T nChoice,const wchar_t *const choice[],int nShow,int nWidth,int nLstWidth,YSBOOL newLine)
{
	YsWString wLabel;
	wLabel.SetUTF8String(label);
	return AddDropListTemplate <wchar_t> (id,fsKey,wLabel,nChoice,choice,nShow,nWidth,nLstWidth,newLine);
}

FsGuiDropList *FsGuiDialog::AddDropList(
	   int id,int fsKey,const wchar_t label[],YSSIZE_T nChoice,const wchar_t *const choice[],int nShow,int nWidth,int nLstWidth,YSBOOL newLine)
{
	return AddDropListTemplate <wchar_t> (id,fsKey,label,nChoice,choice,nShow,nWidth,nLstWidth,newLine);
}

template <class CHARTYPE>
FsGuiDropList *FsGuiDialog::AddDropListTemplate(
   int id,int fsKey,const YsWString &label,YSSIZE_T nChoice,const CHARTYPE *const choice[],int nShow,int nWidth,int nLstWidth,YSBOOL newLine)
{
	YsListItem <FsGuiDropList> *neo;
	int x0,y0;
	int drpWid,drpHei,lstWid,lstHei;

	int fontWid,fontHei;
	GetUnicodeCharacterBitmapSize(fontWid,fontHei);
	if(0==fontHei)
	{
		GetAsciiCharacterBitmapSize(fontWid,fontHei);
	}

	drpWid=(nWidth+2)*fontWid;
	drpHei=fontHei+8;

	lstWid=nLstWidth*fontWid;
	lstHei=fontHei*(nShow+1);



	neo=drpList.Create();
	neo->dat.Initialize(this);

	PlaceNewItem(&neo->dat,drpWid,drpHei,newLine);

	x0=neo->dat.x0;
	y0=neo->dat.y0;

	neo->dat.wLabel=label;
	neo->dat.id=id;
	neo->dat.fsKey=fsKey;

	neo->dat.nWidth=nWidth;


	neo->dat.lbx.x0=x0;
	neo->dat.lbx.y0=y0+drpHei;

	neo->dat.lbx.wid=lstWid;
	neo->dat.lbx.hei=lstHei;

	neo->dat.lbx.wLabel=label;
	neo->dat.lbx.id=id;
	neo->dat.lbx.fsKey=fsKey;

	neo->dat.lbx.showHeightPix=nShow*fontHei;
	neo->dat.lbx.nShowWidth=nLstWidth;
	neo->dat.lbx.showTopPix=0;
	neo->dat.lbx.SetChoice(nChoice,choice);


	itemList.Append(&neo->dat);

	return &neo->dat;
}

FsGuiColorPalette *FsGuiDialog::AddColorPalette(int id,int fsKey,const char label[],int r,int g,int b,int step,YSBOOL colorBtn,YSBOOL newLine)
{
	int cplWid,cplHei;
	YsListItem <FsGuiColorPalette> *neo=cplList.Create();
	neo->dat.Initialize(this);
	neo->dat.hasComponentButton=YSTRUE;
	neo->dat.hasColorButton=colorBtn;


	if(NULL!=label && 0!=label[0])
	{
		YsWString wLabel;
		wLabel.SetUTF8String(label);
		YsBitmap wholeBmp;
		if(YSOK==RenderUnicodeString(wholeBmp,wLabel,defFgCol,defBgCol) ||
		   YSOK==RenderAsciiString(wholeBmp,label,defFgCol,defBgCol))
		{
			neo->dat.bmp.SetBitmap(wholeBmp);
		}
		else
		{
			neo->dat.bmp.CleanUp();
		}
	}

	int fontWid,fontHei;
	GetAsciiCharacterBitmapSize(fontWid,fontHei);

	const int totalItemWid=(fontWid*5+defHAnnotation+defHSpaceUnit)*3+fontWid*5;
	cplWid=defHSpaceUnit*2+YsGreater((int)neo->dat.bmp.GetWidth(),totalItemWid);


	const int numberBoxHei=YsGreater(fontHei,defHAnnotation*2);
	if(0<neo->dat.bmp.GetHeight())
	{
		cplHei=defVSpaceUnit+neo->dat.bmp.GetHeight()+defVSpaceUnit+numberBoxHei;
	}
	else
	{
		cplHei=defVSpaceUnit+defVSpaceUnit/2+numberBoxHei;
	}

	if(YSTRUE==colorBtn)
	{
		cplHei+=defHAnnotation*2;
	}


	PlaceNewItem(&neo->dat,cplWid,cplHei,newLine);

	itemList.Append(&neo->dat);

	neo->dat.wLabel.SetUTF8String(label);
	neo->dat.id=id;
	neo->dat.fsKey=fsKey;

	neo->dat.col[0]=r;
	neo->dat.col[1]=g;
	neo->dat.col[2]=b;
	neo->dat.step=step;

	return &neo->dat;
}

FsGuiColorPalette *FsGuiDialog::AddCompactColorPalette(int id,int fsKey,const char label[],int r,int g,int b,YSBOOL newLine)
{
	int cplWid,cplHei;
	YsListItem <FsGuiColorPalette> *neo=cplList.Create();
	neo->dat.Initialize(this);

	neo->dat.hasComponentButton=YSFALSE;
	neo->dat.hasColorButton=YSFALSE;


	if(NULL!=label && 0!=label[0])
	{
		YsWString wLabel;
		wLabel.SetUTF8String(label);
		YsBitmap wholeBmp;
		if(YSOK==RenderUnicodeString(wholeBmp,wLabel,defFgCol,defBgCol) ||
		   YSOK==RenderAsciiString(wholeBmp,label,defFgCol,defBgCol))
		{
			neo->dat.bmp.SetBitmap(wholeBmp);
		}
		else
		{
			neo->dat.bmp.CleanUp();
		}
	}

	int fontWid,fontHei;
	GetAsciiCharacterBitmapSize(fontWid,fontHei);

	const int totalItemWid=fontWid*5;
	cplWid=defHSpaceUnit*2+YsGreater((int)neo->dat.bmp.GetWidth(),totalItemWid);


	if(0<neo->dat.bmp.GetHeight())
	{
		cplHei=defVSpaceUnit+neo->dat.bmp.GetHeight()+defVSpaceUnit+fontHei;
	}
	else
	{
		cplHei=defVSpaceUnit+defVSpaceUnit/2+fontHei;
	}

	PlaceNewItem(&neo->dat,cplWid,cplHei,newLine);

	itemList.Append(&neo->dat);

	neo->dat.wLabel.SetUTF8String(label);
	neo->dat.id=id;
	neo->dat.fsKey=fsKey;

	neo->dat.col[0]=r;
	neo->dat.col[1]=g;
	neo->dat.col[2]=b;
	neo->dat.step=0;

	return &neo->dat;
}

FsGuiSlider *FsGuiDialog::AddHorizontalSlider(int /*id*/,int /*fsKey*/,int nWidth,const double &min,const double &max,YSBOOL newLine)
{
	int fontWid,fontHei;
	GetAsciiCharacterBitmapSize(fontWid,fontHei);

	int sldWid,sldHei;
	sldWid=nWidth*fontWid+6;
	sldHei=fontHei+6;


	YsListItem <FsGuiSlider> *neo;
	neo=sldList.Create();
	neo->dat.Initialize(this);

	PlaceNewItem(&neo->dat,sldWid,sldHei,newLine);

	itemList.Append(&neo->dat);

	neo->dat.SetMinMax(min,max);

	return &neo->dat;

}

FsGuiSlider *FsGuiDialog::AddVerticalSlider(int /*id*/,int /*fsKey*/,int nHeight,const double &min,const double &max,YSBOOL newLine)
{
	int fontWid,fontHei;
	GetAsciiCharacterBitmapSize(fontWid,fontHei);

	int sldWid,sldHei;
	sldWid=fontWid+8;
	sldHei=fontHei*nHeight+6;


	YsListItem <FsGuiSlider> *neo;
	neo=sldList.Create();
	neo->dat.Initialize(this);
	neo->dat.SetHorizontal(YSFALSE);

	PlaceNewItem(&neo->dat,sldWid,sldHei,newLine);

	itemList.Append(&neo->dat);

	neo->dat.SetMinMax(min,max);

	return &neo->dat;

}

FsGuiTreeControl *FsGuiDialog::AddTreeControl(int id,int fsKey,int nShow,int nWidth,YSBOOL newLine)
{
	int fontWid,fontHei;
	GetAsciiCharacterBitmapSize(fontWid,fontHei);

	const int wid=fontWid*nWidth;
	const int hei=fontHei*nShow;

	treeAlloc.Increment();
	FsGuiTreeControl *newItem=&treeAlloc.GetEnd();

	newItem->Initialize(this);
	PlaceNewItem(newItem,wid,hei,newLine);
	SetIdKeyLabel(newItem,id,fsKey,L"");
	itemList.Append(newItem);
	
	return newItem;
}

void FsGuiDialog::AddCustomControl(int id,int fsKey,FsGuiDialogItem *item,int widInPixel,int heiInPixel,YSBOOL newLine)
{
	item->Initialize(this);
	item->itemType=FSGUI_CUSTOM;
	item->id=id;
	item->fsKey=id;
	item->x0=0; // Tentative
	item->y0=0; // Tentative
	item->wid=widInPixel;
	item->hei=heiInPixel;

	PlaceNewItem(item,widInPixel,heiInPixel,newLine);
	SetIdKeyLabel(item,id,fsKey,"");

	itemList.Append(item);
}

FsGuiTabControl *FsGuiDialog::AddTabControl(int /*id*/,int /*fsKey*/,YSBOOL newLine)
{
	int fontWid,fontHei;
	GetAsciiCharacterBitmapSize(fontWid,fontHei);

	YsListItem <FsGuiTabControl> *neo;
	neo=tabList.Create();
	neo->dat.Initialize(this);

	PlaceNewItem(&neo->dat,fontWid,fontHei,newLine);

	itemList.Append(&neo->dat);

	if(NULL==primaryTab)
	{
		primaryTab=&neo->dat;
	}

	return &neo->dat;
}

void FsGuiDialog::ExpandTab(FsGuiTabControl *tabCtrl)
{
	tabCtrl->ExpandTab();
}

int FsGuiDialog::AddTab(FsGuiTabControl *tabCtrl,const char str[])
{
	YsWString wStr;
	wStr.SetUTF8String(str);
	return AddTab(tabCtrl,wStr);
}

int FsGuiDialog::AddTab(FsGuiTabControl *tabCtrl,const wchar_t str[])
{
	if(NULL!=tabCtrl)
	{
		const int prevHei=tabCtrl->hei;
		const int tabId=tabCtrl->AddTab(str);
		const int newHei=tabCtrl->hei;

		if(prevHei<newHei)
		{
			int diff=newHei-prevHei;
			lastY+=diff;
		}

		return tabId;
	}
	return -1;
}

int FsGuiDialog::AddBmpTab(FsGuiTabControl *tabCtrl,const YsBitmap &bmpIn,const wchar_t str[])
{
	if(NULL!=tabCtrl)
	{
		const int prevHei=tabCtrl->hei;
		const int tabId=tabCtrl->AddBmpTab(bmpIn,str);
		const int newHei=tabCtrl->hei;

		if(prevHei<newHei)
		{
			int diff=newHei-prevHei;
			lastY+=diff;
		}

		return tabId;
	}
	return -1;
}

void FsGuiDialog::BeginAddTabItem(FsGuiTabControl *tabCtrl,int tabId)
{
	if(NULL==makingTab)
	{
		makingTab=tabCtrl;
		makingTabId=tabId;
	}
}

void FsGuiDialog::EndAddTabItem(void)
{
	if(NULL!=makingTab)
	{
		const int y=makingTab->y0+makingTab->fieldHei+2;

		makingTab->SelectCurrentTab(makingTabId);

		if(lastY<y)
		{
			lastY=y;
		}
	}

	makingTab=NULL;
	makingTabId=-1;
}



FsGuiGroupBox *FsGuiDialog::AddGroupBox(void)
{
	YsListItem <FsGuiGroupBox> *neo;
	neo=grpBoxList.Create();
	neo->dat.Initialize();

	if(NULL!=makingTab && YSTRUE==makingTab->tabList.IsInRange(makingTabId))
	{
		makingTab->tabList[makingTabId].tabGrpBox.Append(&neo->dat);
	}

	return &neo->dat;
}

void FsGuiDialog::ClickEquivalent(FsGuiDialogItem *item)
{
	switch(item->itemType)
	{
	default:
		{
			auto cen=item->GetCenter();
			item->LButtonDown(cen.x(),cen.y());
			item->LButtonUp(cen.x(),cen.y());
		}
		break;
	case FSGUI_BUTTON:
		{
			auto btn=dynamic_cast <FsGuiButton *>(item);
			btn->Click();
		}
		break;
	}
}

void FsGuiDialog::PlaceNewItem(FsGuiDialogItem *item,int itemWid,int itemHei,YSBOOL newLine)
{
	int fontWid,fontHei;
	GetAsciiCharacterBitmapSize(fontWid,fontHei);

	if(NULL==makingTab || YSTRUE!=makingTab->tabList.IsInRange(makingTabId))
	{
		if(autoNewColumn==YSTRUE && newLine==YSTRUE)
		{
			if(lastY+itemHei>dlgY0+dlgHei-fontHei)
			{
				StepToNextColumn();
			}
		}

		if(newLine!=YSTRUE && itemList.GetN()>0)
		{
			item->x0=itemList.GetEnd()->x0+itemList.GetEnd()->wid+fontWid;
			item->y0=itemList.GetEnd()->y0;
			lastY=YsGreater(lastY,item->y0+itemHei+2);
		}
		else
		{
			item->x0=lastX;
			item->y0=lastY;
			lastY+=itemHei+2;
		}
	}
	else
	{
		const int tabId=makingTabId;
		int lastX=makingTab->x0+makingTab->tabList[tabId].lastX;
		int lastY=makingTab->y0+makingTab->tabList[tabId].labelHei+makingTab->tabList[tabId].lastY;

		if(newLine!=YSTRUE && itemList.GetN()>0)
		{
			item->x0=itemList.GetEnd()->x0+itemList.GetEnd()->wid+fontWid;
			item->y0=itemList.GetEnd()->y0;
			lastY=YsGreater(lastY,item->y0+itemHei+2);
		}
		else
		{
			item->x0=lastX;
			item->y0=lastY;
			lastY+=itemHei+2;
		}

		makingTab->tabList[tabId].lastY=lastY-(makingTab->y0+makingTab->tabList[tabId].labelHei);
		makingTab->tabList[tabId].tabItem.Append(item);

		int x1=item->x0+itemWid-1;
		int y1=item->y0+itemHei-1;

		int tabCtrlWid=x1-makingTab->x0+2;
		int tabCtrlHei=y1-makingTab->y0+4;
		if(makingTab->wid<tabCtrlWid)
		{
			makingTab->wid=tabCtrlWid;
		}
		if(makingTab->fieldHei<tabCtrlHei)
		{
			makingTab->fieldHei=tabCtrlHei;
		}
	}

	item->SetNewLineWhenCreated(newLine);
	item->wid=itemWid;
	item->hei=itemHei;
}

void FsGuiDialog::SetIdKeyLabel(FsGuiDialogItem *item,int id,int fsKey,const char label[])
{
	item->id=id;
	item->fsKey=fsKey;
	if(NULL!=label)
	{
		item->wLabel.SetUTF8String(label);
	}
	else
	{
		item->wLabel.Set(NULL);
	}
}

void FsGuiDialog::SetIdKeyLabel(FsGuiDialogItem *item,int id,int fsKey,const wchar_t wLabel[])
{
	item->id=id;
	item->fsKey=fsKey;
	if(NULL!=wLabel)
	{
		item->wLabel.Set(wLabel);
	}
	else
	{
		item->wLabel.Set(NULL);
	}
}

////////////////////////////////////////////////////////////

void FsGuiDialog::SetArrangeType(FSDIALOGARRANGETYPE arrangeType)
{
	this->arrangeType=arrangeType;
}

FSDIALOGARRANGETYPE FsGuiDialog::GetArrangeType(void) const
{
	return arrangeType;
}
