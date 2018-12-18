/* ////////////////////////////////////////////////////////////

File Name: fsguinumberdialog.cpp
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

#include "fsguinumberdialog.h"
#include <fssimplewindow.h>

FsGuiNumberDialog::FsGuiNumberDialog()
{
	numberTxt=NULL;
	okBtn=NULL;
	cancelBtn=NULL;
}

FsGuiNumberDialog::~FsGuiNumberDialog()
{
}

/* static */ FsGuiNumberDialog *FsGuiNumberDialog::Create(void)
{
	FsGuiNumberDialog *dlg=new FsGuiNumberDialog;
	return dlg;
}

/* static */ void FsGuiNumberDialog::Delete(FsGuiNumberDialog *ptr)
{
	delete ptr;
}

void FsGuiNumberDialog::Make(const wchar_t numberLabelIn[],const wchar_t okMsgIn[],const wchar_t cancelMsgIn[])
{
	const wchar_t *okMsg=(NULL!=okMsgIn ? okMsgIn : L"OK");
	const wchar_t *cancelMsg=(NULL!=cancelMsgIn ? cancelMsgIn : L"Cancel");
	const wchar_t *numberLabel=(NULL!=numberLabelIn ? numberLabelIn : L"Number");

	if(NULL==okBtn)
	{
		okBtn=AddTextButton(0,FSKEY_ENTER,FSGUI_PUSHBUTTON,okMsg,YSTRUE);
		cancelBtn=AddTextButton(0,FSKEY_ESC,FSGUI_PUSHBUTTON,cancelMsg,YSFALSE);
		numberTxt=AddTextBox(0,FSKEY_NULL,FsGuiTextBox::HORIZONTAL,numberLabel,16,YSTRUE);
	}

	Fit();
}

void FsGuiNumberDialog::SetRealNumber(const double value,const int precision)
{
	numberTxt->SetRealNumber(value,precision);
}

void FsGuiNumberDialog::SetInteger(const int value)
{
	numberTxt->SetInteger(value);
}

const double FsGuiNumberDialog::GetRealNumber(void) const
{
	return numberTxt->GetRealNumber();
}

const int FsGuiNumberDialog::GetInteger(void) const
{
	return numberTxt->GetInteger();
}

/* virtual */ void FsGuiNumberDialog::OnButtonClick(FsGuiButton *btn)
{
	if(okBtn==btn)
	{
		CloseModalDialog(YSOK);
	}
	else if(cancelBtn==btn)
	{
		CloseModalDialog(YSERR);
	}
}
