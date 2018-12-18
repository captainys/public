#include <ysclass.h>
#include <yscompilerwarning.h>
#include <ysgl.h>
#include <ysport.h>
#include <yspngenc.h>

#include "../fsgui3dapp.h"
#include "../textresource.h"
#include <fsguifiledialog.h>


void FsGui3DMainCanvas::File_Open(FsGuiPopUpMenuItem *)
{
	if(YSTRUE==world.IsModified())
	{
		auto dlg=FsGuiDialog::CreateSelfDestructiveDialog<FsGuiMessageBoxDialog>();
		dlg->Make(L"",FSGUI_DLG_OPEN_CONFIRM_DISCARD_CHANGES,FSGUI_COMMON_YES,FSGUI_COMMON_NO);
		dlg->BindCloseModalCallBack(&THISCLASS::File_Open_ConfirmDiscardChanges,this);
		AttachModalDialog(dlg);
	}
	else
	{
		File_Open_OpenFileDialog();
	}
}
void FsGui3DMainCanvas::File_Open_ConfirmDiscardChanges(FsGuiDialog *dlg,int returnCode)
{
	if(YSOK==(YSRESULT)returnCode)
	{
		File_Open_OpenFileDialog();
	}
}
void FsGui3DMainCanvas::File_Open_OpenFileDialog(void)
{
	auto defFn=world.GetFileName();
	if(0==defFn.Strlen())
	{
		defFn=L"*.retromap";
	}
	else
	{
		YsWString pth,fil;
		defFn.SeparatePathFile(pth,fil);
		defFn.MakeFullPathName(pth,L"*.retromap");
	}

	auto fdlg=FsGuiDialog::CreateSelfDestructiveDialog <FsGuiFileDialog>();
	fdlg->Initialize();
	fdlg->mode=FsGuiFileDialog::MODE_OPEN;
	fdlg->multiSelect=YSFALSE;
	fdlg->title.Set(L"Open");
	fdlg->fileExtensionArray.Append(L".retromap");
	fdlg->defaultFileName=defFn;
	fdlg->BindCloseModalCallBack(&THISCLASS::File_Open_FileSelected,this);
	AttachModalDialog(fdlg);
}
void FsGui3DMainCanvas::File_Open_FileSelected(FsGuiDialog *dlg,int returnCode)
{
	auto fdlg=dynamic_cast <FsGuiFileDialog *>(dlg);
	if(nullptr!=fdlg && (int)YSOK==returnCode && 0<fdlg->selectedFileArray.GetN())
	{
		if(YSTRUE!=world.IsModified())
		{
			File_Open_Open(fdlg->selectedFileArray[0]);
			return;
		}

		// AddRecentlyUsedFile(fn);
	}
}

void FsGui3DMainCanvas::File_Open_Open(YsWString fn)
{
	YsFileIO::File fp(fn,"r");
	if(nullptr!=fp)
	{
		world.Open(fp);
		world.SetFileName(fn);

		AddRecentlyUsedFile(fn);

		for(auto fdHd=world.GetFirstField(); nullptr!=fdHd; fdHd=world.FindNextField(fdHd))
		{
			auto fieldPtr=world.GetField(fdHd);
			auto bbx=fieldPtr->GetBoundingBox();
			drawEnv.SetViewTarget(bbx.GetCenter());
			drawEnv.SetTargetBoundingBox(bbx.Min(),bbx.Max());

			int wid,hei;
			FsGetWindowSize(wid,hei);
			fieldPtr->savedView=drawEnv.SaveViewpoint(wid,hei);
		}

		auto fdHd=world.GetFirstField();
		currentFieldKey=world.GetSearchKey(fdHd);

		auto bbx=world.GetBoundingBox(fdHd);
		drawEnv.SetViewTarget(bbx.GetCenter());
		drawEnv.SetTargetBoundingBox(bbx.Min(),bbx.Max());

		UpdateFieldSelector();
		UpdateNumBmp();
		UpdateDiffThresholdFromWorld();

		SetNeedRedraw(YSTRUE);
	}
}

void FsGui3DMainCanvas::File_Save(FsGuiPopUpMenuItem *)
{
	auto fn=world.GetFileName();
	if(0==fn.Strlen())
	{
		File_SaveAs(nullptr);
		return;
	}

	YsFileIO::File fp(fn,"w");
	if(nullptr==fp)
	{
		File_SaveAs(nullptr);
		return;
	}

	world.ConfigureTextureNoSaveFlag();
	world.Save(fp);

	auto mbox=FsGuiDialog::CreateSelfDestructiveDialog<FsGuiMessageBoxDialog>();
	mbox->Make(L"",FSGUI_DLG_SAVE_SAVED,FSGUI_COMMON_OK,nullptr);
	AttachModalDialog(mbox);
	ArrangeDialog();
}

void FsGui3DMainCanvas::File_SaveAs(FsGuiPopUpMenuItem *)
{
	auto defFn=world.GetFileName();
	if(0==defFn.Strlen())
	{
		auto dataDir=YsSpecialPath::GetUserDocDirW();
		defFn.MakeFullPathName(dataDir,L"newfile.retromap");
	}

	auto fdlg=FsGuiDialog::CreateSelfDestructiveDialog <FsGuiFileDialog>();
	fdlg->Initialize();
	fdlg->mode=FsGuiFileDialog::MODE_SAVE;
	fdlg->multiSelect=YSFALSE;
	fdlg->title.Set(L"Save As");
	fdlg->fileExtensionArray.Append(L".retromap");
	fdlg->defaultFileName=defFn;
	fdlg->BindCloseModalCallBack(&THISCLASS::File_SaveAs_FileSelected,this);
	AttachModalDialog(fdlg);
}
void FsGui3DMainCanvas::File_SaveAs_FileSelected(FsGuiDialog *dlg,int returnCode)
{
	auto fdlg=dynamic_cast <FsGuiFileDialog *>(dlg);
	if(nullptr!=fdlg && (int)YSOK==returnCode && 0<fdlg->selectedFileArray.GetN())
	{
		YsFileIO::File fp(fdlg->selectedFileArray[0],"rb");
		if(nullptr==fp)
		{
			File_SaveAs_Save(fdlg->selectedFileArray[0]);
		}
		else
		{
			auto mbox=FsGuiDialog::CreateSelfDestructiveDialog<FsGuiMessageBoxDialogWithPayload <YsWString> >();
			mbox->payload=fdlg->selectedFileArray[0];
			mbox->Make(FSGUI_DLG_COMMON_FILE_ALREADY_EXIST,FSGUI_DLG_COMMON_ASK_OVERWRITE,FSGUI_COMMON_YES,FSGUI_COMMON_NO);
			AttachModalDialog(mbox);
			ArrangeDialog();
			mbox->BindCloseModalCallBack(&THISCLASS::File_SaveAs_OverwriteConfirmed,this);
		}
	}
}

void FsGui3DMainCanvas::File_SaveAs_OverwriteConfirmed(FsGuiDialog *dlg,int returnCode)
{
	auto mbox=dynamic_cast <FsGuiMessageBoxDialogWithPayload <YsWString> *>(dlg);
	if(nullptr!=mbox && (int)YSOK==returnCode)
	{
		File_SaveAs_Save(mbox->payload);
	}
}

void FsGui3DMainCanvas::File_SaveAs_Save(const YsWString fn)
{
	YsFileIO::File fp(fn,"w");
	if(nullptr!=fp)
	{
		world.ConfigureTextureNoSaveFlag();
		world.Save(fp);
		world.SetFileName(fn);
		AddRecentlyUsedFile(fn);
		auto mbox=FsGuiDialog::CreateSelfDestructiveDialog<FsGuiMessageBoxDialog>();
		mbox->Make(L"",FSGUI_DLG_SAVE_SAVED,FSGUI_COMMON_OK,nullptr);
		AttachModalDialog(mbox);
		ArrangeDialog();
	}
	else
	{
		auto mbox=FsGuiDialog::CreateSelfDestructiveDialog<FsGuiMessageBoxDialog>();
		mbox->Make(FSGUI_COMMON_ERROR,FSGUI_ERROR_CANNOT_WRITE_FILE,FSGUI_COMMON_OK,nullptr);
		AttachModalDialog(mbox);
		ArrangeDialog();
	}
}

////////////////////////////////////////////////////////////

class FsGui3DMainCanvas::SaveAsOneBitmapOptionDialog : public FsGuiDialog
{
public:
	FsGuiButton *includeMarkUpBtn;
	FsGuiButton *okBtn,*cancelBtn;
	void Make(void);
	virtual void OnButtonClick(FsGuiButton *btn);
};
void FsGui3DMainCanvas::SaveAsOneBitmapOptionDialog::Make(void)
{
	includeMarkUpBtn=AddTextButton(0,FSKEY_NULL,FSGUI_CHECKBOX,FSGUI_DLG_EXPORTPNG_INCLUDE_MARKUP,YSTRUE);
	includeMarkUpBtn->SetCheck(YSTRUE);

	okBtn=AddTextButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,FSGUI_COMMON_OK,YSTRUE);
	cancelBtn=AddTextButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,FSGUI_COMMON_CANCEL,YSFALSE);
}
/* virtual */ void FsGui3DMainCanvas::SaveAsOneBitmapOptionDialog::OnButtonClick(FsGuiButton *btn)
{
	if(btn==okBtn)
	{
		CloseModalDialog(YSOK);
	}
	else if(btn==cancelBtn)
	{
		CloseModalDialog(YSERR);
	}
}
// YSBOOL File_SaveAsOneBitmapForEachField_IncludeMarkUp;

void FsGui3DMainCanvas::File_SaveAsOneBitmap(FsGuiPopUpMenuItem *)
{
	auto fdHd=GetCurrentField();
	if(nullptr!=fdHd)
	{
		auto dlgPtr=FsGuiDialog::CreateSelfDestructiveDialog<SaveAsOneBitmapOptionDialog>();
		dlgPtr->Make();
		dlgPtr->BindCloseModalCallBack(&THISCLASS::File_SaveAsOneBitmap_OptionSelected,this);
		AttachModalDialog(dlgPtr);
		ArrangeDialog();
	}
}
void FsGui3DMainCanvas::File_SaveAsOneBitmap_OptionSelected(FsGuiDialog *dlg,int returnCode)
{
	auto dlgPtr=dynamic_cast<SaveAsOneBitmapOptionDialog *>(dlg);
	if(nullptr!=dlgPtr && YSOK==(YSRESULT)returnCode)
	{
		File_SaveAsOneBitmapForEachField_IncludeMarkUp=dlgPtr->includeMarkUpBtn->GetCheck();

		auto fdHd=GetCurrentField();
		if(nullptr!=fdHd)
		{
			auto defFn=world.GetFileName();
			if(0==defFn.Strlen())
			{
				auto dataDir=YsSpecialPath::GetUserDocDirW();
				defFn.MakeFullPathName(dataDir,L"retromap.png");
			}
			else
			{
				auto fieldName=world.GetFieldName(fdHd);
				defFn.RemoveExtension();
				if(0<fieldName.Strlen())
				{
					defFn.Append(L"_");
					defFn.Append(fieldName);
				}
				defFn.ReplaceExtension(L".png");
			}

			auto fdlg=FsGuiDialog::CreateSelfDestructiveDialog <FsGuiFileDialog>();
			fdlg->Initialize();
			fdlg->mode=FsGuiFileDialog::MODE_SAVE;
			fdlg->multiSelect=YSFALSE;
			fdlg->title.Set(L"Save As One Bitmap");
			fdlg->fileExtensionArray.Append(L".png");
			fdlg->defaultFileName=defFn;
			fdlg->BindCloseModalCallBack(&THISCLASS::File_SaveAsOneBitmapFileSelected,this);
			AttachModalDialog(fdlg);
		}
	}
}
void FsGui3DMainCanvas::File_SaveAsOneBitmapFileSelected(FsGuiDialog *dlg,int returnCode)
{
	auto fdlg=dynamic_cast <FsGuiFileDialog *>(dlg);
	if(nullptr!=fdlg && (int)YSOK==returnCode && 0<fdlg->selectedFileArray.GetN())
	{
		YsFileIO::File fp(fdlg->selectedFileArray[0],"rb");
		if(nullptr==fp)
		{
			File_SaveAsOneBitmapSave(fdlg->selectedFileArray[0]);
		}
		else
		{
			auto mbox=FsGuiDialog::CreateSelfDestructiveDialog<FsGuiMessageBoxDialogWithPayload <YsWString> >();
			mbox->payload=fdlg->selectedFileArray[0];
			mbox->Make(FSGUI_DLG_COMMON_FILE_ALREADY_EXIST,FSGUI_DLG_COMMON_ASK_OVERWRITE,FSGUI_COMMON_YES,FSGUI_COMMON_NO);
			AttachModalDialog(mbox);
			ArrangeDialog();
			mbox->BindCloseModalCallBack(&THISCLASS::File_SaveAsOneBitmapOverwriteConfirmed,this);
		}
	}
}
void FsGui3DMainCanvas::File_SaveAsOneBitmapOverwriteConfirmed(FsGuiDialog *dlg,int returnCode)
{
	auto mbox=dynamic_cast <FsGuiMessageBoxDialogWithPayload <YsWString> *>(dlg);
	if(nullptr!=mbox && (int)YSOK==returnCode)
	{
		File_SaveAsOneBitmapSave(mbox->payload);
	}
}
void FsGui3DMainCanvas::File_SaveAsOneBitmapSave(const YsWString fn)
{
	YsFileIO::File fp(fn,"wb");
	if(nullptr!=fp)
	{
		auto fieldPtr=world.GetField(GetCurrentField());
		if(nullptr!=fieldPtr)
		{
			auto bmp=fieldPtr->MakeBitmap(YSTRUE,File_SaveAsOneBitmapForEachField_IncludeMarkUp);
			bmp.Invert();

			YsRawPngEncoder encoder;
			encoder.EncodeToFile(fp,bmp.GetWidth(),bmp.GetHeight(),8,6,bmp.GetRGBABitmapPointer());

			auto mbox=FsGuiDialog::CreateSelfDestructiveDialog<FsGuiMessageBoxDialog>();
			mbox->Make(L"",FSGUI_DLG_SAVE_SAVED,FSGUI_COMMON_OK,nullptr);
			AttachModalDialog(mbox);
			ArrangeDialog();
		}
	}
	else
	{
		auto mbox=FsGuiDialog::CreateSelfDestructiveDialog<FsGuiMessageBoxDialog>();
		mbox->Make(FSGUI_COMMON_ERROR,FSGUI_ERROR_CANNOT_WRITE_FILE,FSGUI_COMMON_OK,nullptr);
		AttachModalDialog(mbox);
		ArrangeDialog();
	}
}

////////////////////////////////////////////////////////////

void FsGui3DMainCanvas::File_SaveAsOneBitmapForEachField(FsGuiPopUpMenuItem *)
{
	auto dlgPtr=FsGuiDialog::CreateSelfDestructiveDialog<SaveAsOneBitmapOptionDialog>();
	dlgPtr->Make();
	dlgPtr->BindCloseModalCallBack(&THISCLASS::File_SaveAsOneBitmapForEachField_OptionSelected,this);
	AttachModalDialog(dlgPtr);
	ArrangeDialog();
}
void FsGui3DMainCanvas::File_SaveAsOneBitmapForEachField_OptionSelected(FsGuiDialog *dlg,int returnCode)
{
	auto dlgPtr=dynamic_cast<SaveAsOneBitmapOptionDialog *>(dlg);
	if(nullptr!=dlgPtr && YSOK==(YSRESULT)returnCode)
	{
		File_SaveAsOneBitmapForEachField_IncludeMarkUp=dlgPtr->includeMarkUpBtn->GetCheck();

		auto defFn=world.GetFileName();
		if(0==defFn.Strlen())
		{
			auto dataDir=YsSpecialPath::GetUserDocDirW();
			defFn.MakeFullPathName(dataDir,L"retromap.png");
		}
		else
		{
			auto fieldName=world.GetGameTitle();
			defFn.RemoveExtension();
			defFn.ReplaceExtension(L".png");
		}

		auto fdlg=FsGuiDialog::CreateSelfDestructiveDialog <FsGuiFileDialog>();
		fdlg->Initialize();
		fdlg->mode=FsGuiFileDialog::MODE_SAVE;
		fdlg->multiSelect=YSFALSE;
		fdlg->title.Set(L"Save As PNG (All Fields)");
		fdlg->fileExtensionArray.Append(L".png");
		fdlg->defaultFileName=defFn;
		fdlg->BindCloseModalCallBack(&THISCLASS::File_SaveAsOneBitmapForEachField_FileSelected,this);
		AttachModalDialog(fdlg);
	}
}
void FsGui3DMainCanvas::File_SaveAsOneBitmapForEachField_FileSelected(FsGuiDialog *dlg,int returnCode)
{
	auto fdlg=dynamic_cast <FsGuiFileDialog *>(dlg);
	if(nullptr!=fdlg && (int)YSOK==returnCode && 0<fdlg->selectedFileArray.GetN())
	{
		auto baseFn=fdlg->selectedFileArray[0];
		pngExportInfo.CleanUp();
		pngExportInfo.MakeFIeldHandleFileNamePair(baseFn,world);
		if(YSTRUE!=pngExportInfo.CheckOverwrite())
		{
			File_SaveAsOneBitmapForEachField_Save(baseFn);
		}
		else
		{
			auto mbox=FsGuiDialog::CreateSelfDestructiveDialog<FsGuiMessageBoxDialogWithPayload <YsWString> >();
			mbox->payload=fdlg->selectedFileArray[0];
			mbox->Make(FSGUI_DLG_COMMON_FILE_ALREADY_EXIST,FSGUI_DLG_COMMON_ASK_OVERWRITE,FSGUI_COMMON_YES,FSGUI_COMMON_NO);
			AttachModalDialog(mbox);
			ArrangeDialog();
			mbox->BindCloseModalCallBack(&THISCLASS::File_SaveAsOneBitmapForEachField_OverwriteConfirmed,this);
		}

	}
}
void FsGui3DMainCanvas::File_SaveAsOneBitmapForEachField_OverwriteConfirmed(FsGuiDialog *dlg,int returnCode)
{
	auto mbox=dynamic_cast <FsGuiMessageBoxDialogWithPayload <YsWString> *>(dlg);
	if(nullptr!=mbox && (int)YSOK==returnCode)
	{
		File_SaveAsOneBitmapForEachField_Save(mbox->payload);
	}
}
void FsGui3DMainCanvas::File_SaveAsOneBitmapForEachField_Save(const YsWString fn)
{
	YSRESULT res=YSOK;
	for(auto fdHdFn : pngExportInfo.GetFieldHandleFileNamePairArray())
	{
		YsFileIO::File fp(fdHdFn.fn,"wb");
		if(nullptr!=fp)
		{
			auto fieldPtr=world.GetField(fdHdFn.fdHd);
			if(nullptr!=fieldPtr)
			{
				auto bmp=fieldPtr->MakeBitmap(YSTRUE,File_SaveAsOneBitmapForEachField_IncludeMarkUp);
				bmp.Invert();

				YsRawPngEncoder encoder;
				encoder.EncodeToFile(fp,bmp.GetWidth(),bmp.GetHeight(),8,6,bmp.GetRGBABitmapPointer());
			}
		}
		else
		{
			res=YSERR;
		}
	}

	if(YSOK==res)
	{
		auto mbox=FsGuiDialog::CreateSelfDestructiveDialog<FsGuiMessageBoxDialog>();
		mbox->Make(L"",FSGUI_DLG_SAVE_SAVED,FSGUI_COMMON_OK,nullptr);
		AttachModalDialog(mbox);
		ArrangeDialog();
	}
	else
	{
		auto mbox=FsGuiDialog::CreateSelfDestructiveDialog<FsGuiMessageBoxDialog>();
		mbox->Make(FSGUI_COMMON_ERROR,FSGUI_ERROR_CANNOT_WRITE_FILE,FSGUI_COMMON_OK,nullptr);
		AttachModalDialog(mbox);
		ArrangeDialog();
	}
}

////////////////////////////////////////////////////////////

void FsGui3DMainCanvas::RefreshRecentlyUsedFileList(void)
{
	YsWString recentFn=fileAndDirName.GetRecentFileListFileName();
	FILE *fp=YsFileIO::Fopen(recentFn,"r");
	if(NULL!=fp)
	{
		YsTextFileInputStream inStream(fp);
		recent.Open(inStream);
		fclose(fp);

		recent.PopulateMenu(*fileRecent,16,&THISCLASS::File_Recent,this);
	}
}

void FsGui3DMainCanvas::AddRecentlyUsedFile(const wchar_t wfn[])
{
	recent.AddFile(wfn);
	recent.PopulateMenu(*fileRecent,16,&THISCLASS::File_Recent,this);

	YsWString recentFn=fileAndDirName.GetRecentFileListFileName();

	fileAndDirName.MakeDirectoryForFile(recentFn);

	FILE *fp=YsFileIO::Fopen(recentFn,"w");
	if(NULL!=fp)
	{
		YsTextFileOutputStream outStream(fp);
		recent.Save(outStream,16);
		fclose(fp);
	}
}

void FsGui3DMainCanvas::File_Recent(FsGuiPopUpMenuItem *itm)
{
	const wchar_t *wfn=itm->GetString();
	if(YSTRUE!=world.IsModified())
	{
		File_Open_Open(wfn);
		return;
	}

	auto dlg=FsGuiDialog::CreateSelfDestructiveDialog<FsGuiMessageBoxDialogWithPayload <YsWString> >();
	dlg->Make(L"",FSGUI_DLG_OPEN_CONFIRM_DISCARD_CHANGES,FSGUI_COMMON_YES,FSGUI_COMMON_NO);
	dlg->BindCloseModalCallBack(&THISCLASS::File_Recent_ConfirmOpen,this);
	dlg->payload=wfn;
	AttachModalDialog(dlg);
}

void FsGui3DMainCanvas::File_Recent_ConfirmOpen(FsGuiDialog *dlg,int returnCode)
{
	auto msgBox=dynamic_cast<FsGuiMessageBoxDialogWithPayload <YsWString> *>(dlg);
	if(nullptr!=msgBox && YSOK==(YSRESULT)returnCode)
	{
		File_Open_Open(msgBox->payload);
	}
}
