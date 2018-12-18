#include <ysclass.h>
#include <yscompilerwarning.h>
#include <ysgl.h>
#include <ysport.h>
#include <fsguifiledialog.h>

#include "../fsgui3dapp.h"
#include "../textresource.h"
#include "../config/retromap_config.h"
#include "../filename.h"


class FsGui3DMainCanvas::ConfigDialog : public FsGuiDialog
{
public:
	RetroMap_Config *configPtr;
	FsGui3DMainCanvas *canvasPtr;

	int selectingScreenshotDirIndex;
	FsGuiTextBox *folderTxt[RetroMap_Config::MAX_NUM_SCREENSHOT_DIRECTORY];
	FsGuiButton *recursiveBtn[RetroMap_Config::MAX_NUM_SCREENSHOT_DIRECTORY];
	FsGuiButton *browseBtn[RetroMap_Config::MAX_NUM_SCREENSHOT_DIRECTORY];
	FsGuiButton *okBtn,*cancelBtn;

	void Make(FsGui3DMainCanvas *canvasPtr,RetroMap_Config *configPtr);
	virtual void OnButtonClick(FsGuiButton *btn);
	void FileDialogClosed(FsGuiDialog *dlg,int returnCode);
};

void FsGui3DMainCanvas::ConfigDialog::Make(FsGui3DMainCanvas *canvasPtr,RetroMap_Config *configPtr)
{
	this->canvasPtr=canvasPtr;
	this->configPtr=configPtr;

	AddStaticText(0,FSKEY_NULL,FSGUI_DLG_CONFIG_FOLDERNAME,YSFALSE);
	for(int i=0; i<RetroMap_Config::MAX_NUM_SCREENSHOT_DIRECTORY; ++i)
	{
		folderTxt[i]=AddTextBox(0,FSKEY_NULL,FsGuiTextBox::HORIZONTAL,L"",64,YSTRUE);
		recursiveBtn[i]=AddTextButton(0,FSKEY_NULL,FSGUI_CHECKBOX,L"Sub-Folder",YSFALSE);
		browseBtn[i]=AddTextButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,FSGUI_DLG_CONFIG_BROWSE,YSFALSE);
	}
	AddStaticText(0,FSKEY_NULL,FSGUI_DLG_CONFIG_BROWSE_MESSAGE,YSTRUE);

	okBtn=AddTextButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,FSGUI_COMMON_OK,YSTRUE);
	cancelBtn=AddTextButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,FSGUI_COMMON_CANCEL,YSTRUE);
	SetArrangeType(FSDIALOG_ARRANGE_TOP_LEFT);
	Fit();

	{
		int i=0;
		auto &prof=configPtr->GetCurrentProfile();
		for(auto &scc : prof.scrnShotFileDir)
		{
			folderTxt[i]->SetText(scc.GetFolderName());
			recursiveBtn[i]->SetCheck(scc.GetRecursive());
			++i;
			if(RetroMap_Config::MAX_NUM_SCREENSHOT_DIRECTORY<=i)
			{
				break;
			}
		}
	}
}
/* virtual */ void FsGui3DMainCanvas::ConfigDialog::OnButtonClick(FsGuiButton *btn)
{
	for(int i=0; i<RetroMap_Config::MAX_NUM_SCREENSHOT_DIRECTORY; ++i)
	{
		if(btn==browseBtn[i])
		{
			selectingScreenshotDirIndex=i;
			auto defFn=canvasPtr->world.GetLastUsedImageFile();
			if(0==defFn.Strlen())
			{
				defFn=L"*.png";
			}
			else
			{
				YsWString pth,fil;
				defFn.SeparatePathFile(pth,fil);
				defFn.MakeFullPathName(pth,L"*.png");
			}

			auto fdlg=FsGuiDialog::CreateSelfDestructiveDialog <FsGuiFileDialog>();
			fdlg->Initialize();
			fdlg->mode=FsGuiFileDialog::MODE_OPEN;
			fdlg->multiSelect=YSTRUE;
			fdlg->title.Set(L"Select an Image in the Screenshot Directory");
			fdlg->fileExtensionArray.Append(L".png");
			fdlg->defaultFileName=defFn;
			fdlg->BindCloseModalCallBack(&ConfigDialog::FileDialogClosed,this);
			AttachModalDialog(fdlg);
			return;
		}
	}
	if(okBtn==btn)
	{
		for(int i=0; i<RetroMap_Config::MAX_NUM_SCREENSHOT_DIRECTORY; ++i)
		{
			// DO SET recursive flag before screenshotdirectory.
			// SetScreenshotDirectory updates the file-name cache.
			configPtr->SetRecursive(i,recursiveBtn[i]->GetCheck());
			configPtr->SetScreenShotDirectory(i,folderTxt[i]->GetWString());
		}

		// Weak defense against program-never-start problem due to "/" set as image root.
		// It doesn't save the config file unless UpdateScreenshotCache returns.

		canvasPtr->fileAndDirName.MakeDirectoryForFile(canvasPtr->fileAndDirName.GetOptionFileName());

		configPtr->UpdateScreenshotCache();
		configPtr->Save(canvasPtr->fileAndDirName.GetOptionFileName());
		CloseModalDialog(YSOK);
	}
	else if(cancelBtn==btn)
	{
		CloseModalDialog(YSERR);
	}
}
void FsGui3DMainCanvas::ConfigDialog::FileDialogClosed(FsGuiDialog *dlg,int returnCode)
{
	auto fdlg=dynamic_cast<FsGuiFileDialog *>(dlg);
	if(nullptr!=fdlg && YSOK==(YSRESULT)returnCode && 0<fdlg->selectedFileArray.GetN())
	{
		if(0<=selectingScreenshotDirIndex && selectingScreenshotDirIndex<RetroMap_Config::MAX_NUM_SCREENSHOT_DIRECTORY)
		{
			YsWString pth,fil;
			fdlg->selectedFileArray[0].SeparatePathFile(pth,fil);
			folderTxt[selectingScreenshotDirIndex]->SetText(pth);
		}
	}
}

void FsGui3DMainCanvas::Config_Config(FsGuiPopUpMenuItem *)
{
	auto dlg=FsGuiDialog::CreateSelfDestructiveDialog<ConfigDialog>();
	dlg->Make(this,&config);
	AttachModalDialog(dlg);
	ArrangeDialog();
	SetNeedRedraw(YSTRUE);
}


void FsGui3DMainCanvas::Config_DataIntegrityCheck(FsGuiPopUpMenuItem *)
{
	auto fdHd=GetCurrentField();
	if(nullptr!=fdHd)
	{
		if(YSOK==world.DataIntegrityCheck(fdHd))
		{
			auto dlg=FsGuiDialog::CreateSelfDestructiveDialog<FsGuiMessageBoxDialog>();
			dlg->Make(L"",L"No error detected.",FSGUI_COMMON_OK,nullptr);
			AttachModalDialog(dlg);
		}
		else
		{
			auto dlg=FsGuiDialog::CreateSelfDestructiveDialog<FsGuiMessageBoxDialog>();
			dlg->Make(FSGUI_COMMON_ERROR,L"There were errors.",FSGUI_COMMON_CLOSE,nullptr);
			AttachModalDialog(dlg);
		}
	}
}
