/* ////////////////////////////////////////////////////////////

File Name: miscdlg.cpp
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

#include "miscdlg.h"
#include <fsguifiledialog.h>
#include <ysport.h>
#include <fssimplewindow.h>
#include "ysgebl_gui_editor_base.h"

YsColorPaletteDialog::YsColorPaletteDialog()
{
	fdlg=new FsGuiFileDialog;
}

YsColorPaletteDialog::~YsColorPaletteDialog()
{
	delete fdlg;
}

/*static*/YsColorPaletteDialog *YsColorPaletteDialog::Create(void)
{
	YsColorPaletteDialog *dlg=new YsColorPaletteDialog;
	return dlg;
}

/*static*/void YsColorPaletteDialog::Delete(YsColorPaletteDialog *dlg)
{
	delete dlg;
}

void YsColorPaletteDialog::MakeDialog(class GeblGuiEditorBase *ownerIn)
{
	plt=AddColorPalette(0,FSKEY_NULL,"COLOR",255,255,255,4,YSTRUE,YSTRUE);
	savePlt=AddTextButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,"SAVE",YSTRUE);
	loadPlt=AddTextButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,"LOAD",YSFALSE);

	SetArrangeType(FSDIALOG_ARRANGE_BOTTOM_RIGHT);

	this->canvas=ownerIn;

	SetTransparency(YSTRUE);
	Fit();
}

void YsColorPaletteDialog::InitDialog(void)
{
}

void YsColorPaletteDialog::SetColor(const YsColor &col)
{
	plt->SetColor(col);
}

YsColor YsColorPaletteDialog::GetColor(void) const
{
	YsColor col;
	plt->GetColor(col);
	return col;
}

void YsColorPaletteDialog::OnButtonClick(FsGuiButton *btn)
{
	if(btn==savePlt)
	{
		YsString def("untitled.ypl");
		// def.Set(owner->scn.GetLastOpenedFilename());
		// if(0<def.Strlen())
		// {
		// 	def.ReplaceExtension("ypl");
		// }
		// else
		// {
		// 	def.MakeFullPathName(defDataDir,"untitled.ypl");
		// }

		fdlg->Initialize();
		fdlg->mode=FsGuiFileDialog::MODE_SAVE;
		fdlg->title.Set(L"SAVE PALETTE");
		fdlg->fileExtensionArray.Append(L".ypl");
		fdlg->defaultFileName.SetUTF8String(def);
		AttachModalDialog(fdlg);
	}
	else if(btn==loadPlt)
	{
		YsString def("untitled.ypl");
		// def.Set(owner->scn.GetLastOpenedFilename());
		// if(0<def.Strlen())
		// {
		// 	def.ReplaceExtension("ypl");
		// }
		// else
		// {
		// 	def.MakeFullPathName(scnDir,"untitled.ypl");
		// }

		fdlg->Initialize();
		fdlg->mode=FsGuiFileDialog::MODE_OPEN;
		fdlg->title.Set(L"OPEN PALETTE");
		fdlg->fileExtensionArray.Append(L".ypl");
		fdlg->defaultFileName.SetUTF8String(def);
		AttachModalDialog(fdlg);
	}
}

void YsColorPaletteDialog::OnModalDialogClosed(int,class FsGuiDialog *closedModalDialog,int modalDialogEndCode)
{
	if(closedModalDialog==fdlg && YSOK==fdlg->res && 0<fdlg->selectedFileArray.GetN())
	{
		if(fdlg->mode==FsGuiFileDialog::MODE_SAVE)
		{
			FILE *fp=YsFileIO::Fopen(fdlg->selectedFileArray[0],"rb");
			if(NULL!=fp)
			{
				fclose(fp);

				savePaletteFileName=fdlg->selectedFileArray[0];
				yesno.Initialize();
				yesno.Make(
				    L"File Already Exists.",
				    L"File Already Exists. Overwrite?",
				    L"Yes",
				    L"No",
				    (int)YSOK,
				    (int)YSERR);
				AttachModalDialog(&yesno);
			}
			else
			{
				SavePalette(fdlg->selectedFileArray[0]);
			}
		}
		else if(fdlg->mode==FsGuiFileDialog::MODE_OPEN)
		{
			LoadPalette(fdlg->selectedFileArray[0]);
			canvas->SetNeedRedraw(YSTRUE);
		}
	}
	else if(&yesno==closedModalDialog && modalDialogEndCode==(int)YSOK)
	{
		if(fdlg->mode==FsGuiFileDialog::MODE_SAVE)
		{
			SavePalette(fdlg->selectedFileArray[0]);
		}
	}
}

void YsColorPaletteDialog::SavePalette(const wchar_t fn[]) const
{
	FILE *fp=YsFileIO::Fopen(fn,"w");
	if(NULL!=fp)
	{
		YsPrintf("Save Palette %ls\n",savePaletteFileName.Txt());
		YsTextFileOutputStream outStream(fp);
		plt->SaveFile(outStream);
		fclose(fp);
		canvas->SetNeedRedraw(YSTRUE);
	}
}

void YsColorPaletteDialog::LoadPalette(const wchar_t fn[])
{
	FILE *fp=YsFileIO::Fopen(fn,"r");
	if(NULL!=fp)
	{
		YsPrintf("Load Palette %ls\n",savePaletteFileName.Txt());
		YsTextFileInputStream outStream(fp);
		plt->LoadFile(outStream);
		fclose(fp);
		canvas->SetNeedRedraw(YSTRUE);
	}
}
