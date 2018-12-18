/* ////////////////////////////////////////////////////////////

File Name: fsguidialogutil.cpp
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
#include <fssimplewindow.h>
#include "fsguidialogutil.h"

////////////////////////////////////////////////////////////

void FsGuiMessageBoxDialog::Make(
    const wchar_t title[],const wchar_t msg[],const wchar_t yesBtn[],const wchar_t noBtn[],
    int returnCodeYes,int returnCodeNo,YSBOOL okBtnBottom)
{
	Make(title,msg,yesBtn,noBtn,NULL,returnCodeYes,returnCodeNo,okBtnBottom);
}

void FsGuiMessageBoxDialog::Make(
	    const wchar_t title[],const wchar_t msg[],const wchar_t yesBtn[],const wchar_t noBtn[],const wchar_t copyBtnTxt[],
	    int returnCodeYes,int returnCodeNo,YSBOOL okBtnBottom)
{
	Initialize();

	this->returnCodeYes=returnCodeYes;
	this->returnCodeNo=returnCodeNo;

	SetTransparency(YSFALSE);

	SetTopLeftCorner(16,16);
	SetTextMessage(title);

	if(YSTRUE!=okBtnBottom)
	{
		this->yesBtn=AddTextButton(MakeIdent("ok"),FSKEY_Y,FSGUI_PUSHBUTTON,yesBtn,YSTRUE);
		if(NULL!=noBtn)
		{
			this->noBtn=AddTextButton(MakeIdent("cancel"),FSKEY_N,FSGUI_PUSHBUTTON,noBtn,YSFALSE);
		}
		else
		{
			this->noBtn=NULL;
		}
		if(NULL!=copyBtnTxt)
		{
			this->copyBtn=AddTextButton(0,FSKEY_C,FSGUI_PUSHBUTTON,copyBtnTxt,YSFALSE);
		}
		else
		{
			this->copyBtn=NULL;
		}
	}

	label=AddStaticText(0,FSKEY_NULL,msg,YSTRUE);
	label->SetFill(YSFALSE);
	label->SetDrawFrame(YSFALSE);

	if(YSTRUE==okBtnBottom)
	{
		this->yesBtn=AddTextButton(MakeIdent("ok"),FSKEY_Y,FSGUI_PUSHBUTTON,yesBtn,YSTRUE);
		if(NULL!=noBtn)
		{
			this->noBtn=AddTextButton(MakeIdent("cancel"),FSKEY_N,FSGUI_PUSHBUTTON,noBtn,YSFALSE);
		}
		else
		{
			this->noBtn=NULL;
		}
		if(NULL!=copyBtnTxt)
		{
			this->copyBtn=AddTextButton(0,FSKEY_C,FSGUI_PUSHBUTTON,copyBtnTxt,YSFALSE);
		}
		else
		{
			this->copyBtn=NULL;
		}
	}


	Fit();
}

void FsGuiMessageBoxDialog::OnKeyDown(int fsKey,YSBOOL ,YSBOOL ,YSBOOL )
{




	if(NULL==noBtn)
	{
		if(FSKEY_ENTER==fsKey || FSKEY_ESC==fsKey || FSKEY_SPACE==fsKey)
		{
			CloseModalDialog(returnCodeYes);
		}
	}
	else
	{
		if(FSKEY_ESC==fsKey)  // Cannot tell the intention of SPACE or ENTER
		{
			CloseModalDialog(returnCodeNo);
		}
	}
}

void FsGuiMessageBoxDialog::OnButtonClick(FsGuiButton *btn)
{
	if(btn==yesBtn)
	{
		CloseModalDialog(returnCodeYes);
	}
	else if(NULL!=noBtn && btn==noBtn)
	{
		CloseModalDialog(returnCodeNo);
	}
	else if(NULL!=btn && NULL!=copyBtn)
	{
		YsString str=label->GetText();
		FsGuiCopyStringToClipBoard(str);
	}
}

////////////////////////////////////////////////////////////

template <const int N>
void FsGuiInputNumberDialogTemplate <N>::OnButtonClickTemplate(FsGuiButton *btn)
{
	if(btn==okBtn)
	{
		CloseModalDialog(returnCodeYes);
	}
	if(btn==cancelBtn)
	{
		CloseModalDialog(returnCodeNo);
	}
}



void FsGuiInputNumberDialog::Make(
    const double defNumber,const int belowDecimal,
    const wchar_t title[],const wchar_t msg[],const wchar_t label[],const wchar_t yesBtn[],const wchar_t noBtn[],
    int returnCodeYes,int returnCodeNo)
{
	Initialize();

	SetTextMessage(title);
	AddStaticText(0,FSKEY_NULL,msg,YSTRUE);
	AddStaticText(0,FSKEY_NULL,label,YSTRUE);
	numTxt[0]=AddTextBox(0,FSKEY_NULL,"","",24,YSFALSE);
	numTxt[0]->SetRealNumber(defNumber,belowDecimal);

	okBtn=AddTextButton(0,FSKEY_ENTER,FSGUI_PUSHBUTTON,yesBtn,YSTRUE);
	cancelBtn=AddTextButton(0,FSKEY_ESC,FSGUI_PUSHBUTTON,noBtn,YSFALSE);

	this->returnCodeYes=returnCodeYes;
	this->returnCodeNo=returnCodeNo;

	SetFocus(numTxt[0]);

	SetTransparency(YSFALSE);
	Fit();
}

const double FsGuiInputNumberDialog::GetNumber(void) const
{
	return numTxt[0]->GetRealNumber();
}

void FsGuiInputNumberDialog::OnButtonClick(FsGuiButton *btn)
{
	OnButtonClickTemplate(btn);
}



void FsGuiInputTwoNumberDialog::Make(
    const double dflt[2],const int belowDecimal,
    const wchar_t msg[],const wchar_t title[],const wchar_t label1[],const wchar_t label2[],
    const wchar_t yesBtn[],const wchar_t noBtn[],int returnCodeYes,int returnCodeNo)
{
	Initialize();

	SetTextMessage(title);
	AddStaticText(0,FSKEY_NULL,msg,YSTRUE);

	wchar_t const *label[]={label1,label2};

	for(int i=0; i<2; i++)
	{
		AddStaticText(0,FSKEY_NULL,label[i],YSTRUE);
		numTxt[i]=AddTextBox(0,FSKEY_NULL,"","",24,YSFALSE);
		numTxt[i]->SetRealNumber(dflt[i],belowDecimal);
	}

	okBtn=AddTextButton(0,FSKEY_ENTER,FSGUI_PUSHBUTTON,yesBtn,YSTRUE);
	cancelBtn=AddTextButton(0,FSKEY_ESC,FSGUI_PUSHBUTTON,noBtn,YSFALSE);

	this->returnCodeYes=returnCodeYes;
	this->returnCodeNo=returnCodeNo;

	SetFocus(numTxt[0]);

	SetTransparency(YSFALSE);

	Fit();

}

const double FsGuiInputTwoNumberDialog::GetNumber(int idx) const
{
	if(0==idx || 1==idx)
	{
		return numTxt[idx]->GetRealNumber();
	}
	return 0.0;
}

void FsGuiInputTwoNumberDialog::OnButtonClick(FsGuiButton *btn)
{
	OnButtonClickTemplate(btn);
}
