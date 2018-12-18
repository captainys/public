/* ////////////////////////////////////////////////////////////

File Name: fsguifiledialog_nodialog.cpp
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
#include <stddef.h>
#include <string.h>

#include <ysclass.h>
#include <ysbitmap.h>
#include <fsgui.h>
#include <fssimplewindow.h>

#include "fsguifiledialog.h"



class FsGuiFileDialog_PlatformSpecificData : public FsGuiDialog
{
public:
	FsGuiButton *okBtn;

	void Make(void);
	virtual void OnButtonClick(FsGuiButton *btn);
};

void FsGuiFileDialog_PlatformSpecificData::Make(void)
{
	FsGuiDialog::Initialize();
	AddStaticText(0,FSKEY_NULL,L"File dialog for this platform hasn't been implemented yet.",YSTRUE);
	okBtn=AddTextButton(0,FSKEY_ENTER,FSGUI_PUSHBUTTON,L"OK",YSTRUE);
	Fit();
}

/* virtual */ void FsGuiFileDialog_PlatformSpecificData::OnButtonClick(FsGuiButton *btn)
{
	CloseModalDialog((int)YSERR);
}

////////////////////////////////////////////////////////////

void FsGuiFileDialog::CloseModalCallBackFunc(class FsGuiDialog *,int returnCode)
{
	this->busy=YSFALSE;
	this->CloseModalDialog(returnCode);
}

void FsGuiFileDialog::Show(const FsGuiDialog *excluded) const
{
	FsGuiDialog::Show(excluded);

	if(YSTRUE!=busy)
	{
		busy=YSTRUE;

		FsGuiFileDialog *nonConst=(FsGuiFileDialog *)this;

		param->Make();
		param->BindCloseModalCallBack(&THISCLASS::CloseModalCallBackFunc,nonConst);

		nonConst->SetBackgroundAlpha(0.1);

		nonConst->AttachModalDialog(this->param);
	}
}

void FsGuiFileDialog::AllocatePlatformSpecificData(void)
{
	param=new FsGuiFileDialog_PlatformSpecificData;
}

void FsGuiFileDialog::DeletePlatformSpecificData(void)
{
	delete param;
}

/* virtual */ void FsGuiFileDialog::OnAttach(void)
{
}
