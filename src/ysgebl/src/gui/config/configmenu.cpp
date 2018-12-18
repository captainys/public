/* ////////////////////////////////////////////////////////////

File Name: configmenu.cpp
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

#include <ysport.h>
#include <ysgl.h>
#include <fsguifiledialog.h>

#include "../ysgebl_gui_editor_base.h"
#include "../miscdlg.h"
#include "../filename.h"
#include "configmenu.h"
#include "polycreconfig.h"

#include "../textresource.h"

PolyCreConfigDialog::PolyCreConfigDialog()
{
	mbox=new FsGuiMessageBoxDialog;
	fdlg=new FsGuiFileDialog;
}

PolyCreConfigDialog::~PolyCreConfigDialog()
{
	if(NULL!=mbox)
	{
		delete mbox;
		mbox=NULL;
	}
	if(NULL!=fdlg)
	{
		delete fdlg;
		fdlg=NULL;
	}
}

void PolyCreConfigDialog::Make(class PolyCreConfig &cfg,class GeblGuiEditorBase *canvas)
{
	this->canvas=canvas;

	FsGuiDialog::Initialize();

	SetArrangeType(FSDIALOG_ARRANGE_TOP_LEFT);

	const wchar_t * const coordSysLabel[3]=
	{
		FSGUI_DLG_CONFIG_RIGHTHANDCOORD,
		FSGUI_DLG_CONFIG_LEFTHANDCOORD,
		NULL
	};

	coordSysDrp=AddDropList(0,FSKEY_NULL,"Coordinate System",2,coordSysLabel,2,40,40,YSTRUE);

	setDefDataPathBtn=AddTextButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,L"Set",YSTRUE);
	AddStaticText(0,FSKEY_NULL,L"Default Data Path=",YSFALSE);
	defDataPath=AddStaticText(0,FSKEY_NULL,cfg.defDataDir,YSFALSE);

	bgColorPlt[0]=AddColorPalette(0,FSKEY_NULL,"",cfg.bgColor[0].Ri(),cfg.bgColor[0].Gi(),cfg.bgColor[0].Bi(),8,YSTRUE,YSTRUE);
	bgColorPlt[1]=AddColorPalette(0,FSKEY_NULL,"",cfg.bgColor[1].Ri(),cfg.bgColor[1].Gi(),cfg.bgColor[1].Bi(),8,YSTRUE,YSFALSE);
	bgColorPlt[2]=AddColorPalette(0,FSKEY_NULL,"",cfg.bgColor[2].Ri(),cfg.bgColor[2].Gi(),cfg.bgColor[2].Bi(),8,YSTRUE,YSFALSE);

	saveWindowPosAndSizeBtn=AddTextButton(0,FSKEY_NULL,FSGUI_CHECKBOX,FSGUI_DLG_CONFIG_SAVEWINDOWPOS,YSTRUE);

	autoFixOrientationInDnmBtn=AddTextButton(0,FSKEY_NULL,FSGUI_CHECKBOX,FSGUI_DLG_CONFIG_AUTOFIXDNMORIENTATION,YSTRUE);
	AddStaticText(0,FSKEY_NULL,FSGUI_DLG_CONFIG_WHYAUTOFIXDNMORIENTATION,YSTRUE);

	saveBinaryStlBtn=AddTextButton(0,FSKEY_NULL,FSGUI_CHECKBOX,FSGUI_DLG_CONFIG_SAVEBINARYSTL,YSTRUE);

	autoAdjustToleranceButton=AddTextButton(0,FSKEY_NULL,FSGUI_CHECKBOX,FSGUI_DLG_CONFIG_AUTOADJUSTTOLERANCE,YSTRUE);

	okBtn=AddTextButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,FSGUI_COMMON_OK,YSTRUE);
	cancelBtn=AddTextButton(0,FSKEY_ESC,FSGUI_PUSHBUTTON,FSGUI_COMMON_CANCEL,YSFALSE);

	setDefaultBtn=AddTextButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,FSGUI_DLG_CONFIG_SETDEFAULT,YSFALSE);


	YsString tolStr;
	tolStr.Printf("Tolerance=%.20lf",YsTolerance);
	AddStaticText(0,FSKEY_NULL,tolStr,YSTRUE);


	SetFromConfig(cfg);

	SetTransparency(YSFALSE);
	Fit();
}

void PolyCreConfigDialog::SetFromConfig(class PolyCreConfig &cfg)
{
	if(YSOPENGL==cfg.coordSys)
	{
		coordSysDrp->Select(0);
	}
	else
	{
		coordSysDrp->Select(1);
	}

	defDataPath->SetText(cfg.defDataDir);

	saveWindowPosAndSizeBtn->SetCheck(cfg.saveWindowPositionAndSize);
	autoFixOrientationInDnmBtn->SetCheck(cfg.autoFixOrientationInDnm);
	autoAdjustToleranceButton->SetCheck(cfg.autoAdjustTolerance);
	saveBinaryStlBtn->SetCheck(cfg.saveBinarySTL);

	bgColorPlt[0]->SetColor(cfg.bgColor[0]);
	bgColorPlt[1]->SetColor(cfg.bgColor[1]);
	bgColorPlt[2]->SetColor(cfg.bgColor[2]);

}

void PolyCreConfigDialog::Retrieve(class PolyCreConfig &cfg)
{
	switch(coordSysDrp->GetSelection())
	{
	case 0:
		cfg.coordSys=YSRIGHT_ZMINUS_YPLUS;
		break;
	case 1:
		cfg.coordSys=YSLEFT_ZPLUS_YPLUS;
		break;
	}

	defDataPath->GetText(cfg.defDataDir);

	cfg.saveWindowPositionAndSize=saveWindowPosAndSizeBtn->GetCheck();
	cfg.autoFixOrientationInDnm=autoFixOrientationInDnmBtn->GetCheck();
	cfg.autoAdjustTolerance=autoAdjustToleranceButton->GetCheck();
	cfg.saveBinarySTL=saveBinaryStlBtn->GetCheck();

	bgColorPlt[0]->GetColor(cfg.bgColor[0]);
	bgColorPlt[1]->GetColor(cfg.bgColor[1]);
	bgColorPlt[2]->GetColor(cfg.bgColor[2]);
}

void PolyCreConfigDialog::OnButtonClick(FsGuiButton *btn)
{
	if(okBtn==btn)
	{
		PolyCreConfig *cfg=PolyCreConfig::Create();
		Retrieve(*cfg);

		const wchar_t *ysflightComDir=canvas->fileAndDirName->GetYsflightComDir();
		YsFileIO::MkDir(ysflightComDir);

		const wchar_t *appDataDir=canvas->fileAndDirName->GetAppDataDir();
		YsFileIO::MkDir(appDataDir);

		const wchar_t *configDir=canvas->fileAndDirName->GetOptionDir();
		YsFileIO::MkDir(configDir);

		const wchar_t *configFileName=canvas->fileAndDirName->GetOptionFileName();
		cfg->Save(configFileName);

		canvas->Option_ReapplyConfig(*cfg);

		PolyCreConfig::Delete(cfg);

		CloseModalDialog(YSOK);
	}
	else if(cancelBtn==btn)
	{
		CloseModalDialog(YSERR);
	}
	else if(setDefDataPathBtn==btn)
	{
		mbox->Make(L"",L"Select a .SRF file in the default data directory.",L"Close",NULL,0,0);
		mbox->SetCloseModalCallBack(BeginSelectDefaultDataDirectory);
		AttachModalDialog(mbox);
	}
	else if(setDefaultBtn==btn)
	{
		auto *cfg=PolyCreConfig::Create();
		SetFromConfig(*cfg);
		PolyCreConfig::Delete(cfg);
	}
}

/*static*/ void PolyCreConfigDialog::BeginSelectDefaultDataDirectory(FsGuiControlBase *dlg,FsGuiDialog *,int)
{
	PolyCreConfigDialog *configDlg=(PolyCreConfigDialog *)dlg;
	GeblGuiEditorBase *canvas=configDlg->canvas;

	YsString dataDir;
	dataDir.EncodeUTF8 <wchar_t> (canvas->GetDataDir());

	YsString def;
	def.MakeFullPathName(dataDir,"*.srf");

	configDlg->fdlg->Initialize();
	configDlg->fdlg->mode=FsGuiFileDialog::MODE_OPEN;
	configDlg->fdlg->multiSelect=YSFALSE;
	configDlg->fdlg->title.Set(L"Open");
	configDlg->fdlg->fileExtensionArray.Append(L".srf");
	configDlg->fdlg->defaultFileName.SetUTF8String(def);
	configDlg->fdlg->SetCloseModalCallBack(SelectDefaultDataDirectory_FileSelected);
	configDlg->AttachModalDialog(configDlg->fdlg);
}

/*static*/ void PolyCreConfigDialog::SelectDefaultDataDirectory_FileSelected(FsGuiControlBase *configDlgIn,FsGuiDialog *fdlgIn,int returnValue)
{
	if(YSOK==(YSRESULT)returnValue)
	{
		PolyCreConfigDialog *configDlg=(PolyCreConfigDialog *)configDlgIn;
		FsGuiFileDialog *fdlg=(FsGuiFileDialog *)fdlgIn;

		YsWString fn=fdlg->selectedFileArray[0];
		YsWString pth,fil;
		fn.SeparatePathFile(pth,fil);

		configDlg->defDataPath->SetText(pth);
	}
}

////////////////////////////////////////////////////////////

void GeblGuiEditorBase::Option_Config(FsGuiPopUpMenuItem *)
{
	auto configDlg=FsGuiDialog::CreateSelfDestructiveDialog <PolyCreConfigDialog>();

	PolyCreConfig *cfg=PolyCreConfig::Create();

	const wchar_t *configFileName=fileAndDirName->GetOptionFileName();
	cfg->Load(configFileName);

	configDlg->Make(*cfg,this);
	AttachModalDialog(configDlg);

	SetNeedRedraw(YSTRUE);

	PolyCreConfig::Delete(cfg);
}

void GeblGuiEditorBase::Option_ReapplyConfig(class PolyCreConfig &cfg)
{
	YsCoordSysModel=cfg.coordSys;
	drawEnv.RotateView(0.0,0.0);
	drawEnv.SetZoom(drawEnv.GetZoom());
	drawEnv.SetPerspective(cfg.perspective);

	*(this->config)=cfg;

	SetNeedRedraw(YSTRUE);
}

void GeblGuiEditorBase::Option_ReapplyConfig(void)
{
	const wchar_t *configFileName=fileAndDirName->GetOptionFileName();
	config->Load(configFileName);
	Option_ReapplyConfig(*config);
}
