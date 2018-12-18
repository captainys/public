#include <ysport.h>

#include "fsguiapp.h"
#include "fsguifiledialog.h"





void FsGuiMainCanvas::File_Open(FsGuiPopUpMenuItem *)
{
	auto fdlg=FsGuiDialog::CreateSelfDestructiveDialog <FsGuiFileDialog> ();

	YsWString lastPath,lastFile;
	lastUsedFileName.SeparatePathFile(lastPath,lastFile);

	if(0==lastPath.Strlen())
	{
		lastPath=YsSpecialPath::GetUserDocDirW();
	}

	YsWString def;
	def.MakeFullPathName(lastPath,L"*.*");

	fdlg->Initialize();
	fdlg->mode=FsGuiFileDialog::MODE_OPEN;
	fdlg->multiSelect=YSFALSE;
	fdlg->title.Set(L"Open");
	fdlg->fileExtensionArray.Append(L".txt");
	fdlg->defaultFileName=def;
	fdlg->BindCloseModalCallBack(&THISCLASS::File_Open_FileSelected,this);
	AttachModalDialog(fdlg);

}

void FsGuiMainCanvas::File_Save(FsGuiPopUpMenuItem *)
{
	if(0!=lastUsedFileName.size())
	{
		Save(lastUsedFileName,YSTRUE);
	}
}

void FsGuiMainCanvas::File_Open_FileSelected(FsGuiDialog *dlg,int returnCode)
{
	auto fdlg=dynamic_cast <FsGuiFileDialog *> (dlg);
	if(nullptr!=fdlg && (int)YSOK==returnCode && 1==fdlg->selectedFileArray.size())
	{
		YsWString fName=fdlg->selectedFileArray[0];
		printf("Selected %ls\n",fName.c_str());
		this->Open(fName,YSTRUE);
	}
}



void FsGuiMainCanvas::File_SaveAs(FsGuiPopUpMenuItem *)
{
	auto fdlg=FsGuiDialog::CreateSelfDestructiveDialog <FsGuiFileDialog> ();

	YsWString lastPath,lastFile;
	lastUsedFileName.SeparatePathFile(lastPath,lastFile);

	if(0==lastPath.Strlen())
	{
		lastPath=YsSpecialPath::GetUserDocDirW();
	}
	if(0==lastFile.Strlen())
	{
		lastFile=L"newfile.txt";
	}

	YsWString def;
	def.MakeFullPathName(lastPath,lastFile);

	fdlg->Initialize();
	fdlg->mode=FsGuiFileDialog::MODE_SAVE;
	fdlg->multiSelect=YSFALSE;
	fdlg->title.Set(L"Save");
	fdlg->fileExtensionArray.Append(L".txt");
	fdlg->defaultFileName=def;
	fdlg->BindCloseModalCallBack(&THISCLASS::File_SaveAs_FileSelected,this);
	AttachModalDialog(fdlg);
}
void FsGuiMainCanvas::File_SaveAs_FileSelected(FsGuiDialog *dlg,int returnCode)
{
	auto fdlg=dynamic_cast <FsGuiFileDialog *> (dlg);
	if(nullptr!=fdlg && (int)YSOK==returnCode && 1==fdlg->selectedFileArray.size())
	{
		YsWString fName=fdlg->selectedFileArray[0];
		printf("Selected %ls\n",fName.c_str());

		YsFileIO::File fp(fName,"rb");
		if(nullptr!=fp)
		{
			auto msgDialog=FsGuiDialog::CreateSelfDestructiveDialog<FsGuiMessageBoxDialogWithPayload <YsWString> >();
			msgDialog->payload=fName;

			YsWString msg;
			msg.Append(fName);
			msg.Append('\n');
			msg.Append(L"File Exists. Overwrite?");
			msgDialog->Make(L"Confirm Overwrite?",msg,L"Yes, Overwrite.",L"No, don't overwrite.");
			AttachModalDialog(msgDialog);
			msgDialog->BindCloseModalCallBack(&THISCLASS::File_SaveAs_OverwriteConfirmed,this);
		}
		else
		{
			this->Save(fName,YSTRUE);
		}
	}
}
void FsGuiMainCanvas::File_SaveAs_OverwriteConfirmed(FsGuiDialog *dlg,int returnCode)
{
	auto msgDialog=dynamic_cast <FsGuiMessageBoxDialogWithPayload <YsWString> *>(dlg);
	if(nullptr!=msgDialog && (int)YSOK==returnCode)
	{
		this->Save(msgDialog->payload,YSTRUE);
	}
}


void FsGuiMainCanvas::File_Recent(FsGuiPopUpMenuItem *itm)
{
	const wchar_t *fName=itm->GetString();
	Open(fName,YSTRUE);
}


void FsGuiMainCanvas::RefreshRecentlyUsedFileList(void)
{
	YsWString recentFn=GetRecentFileListFileName();
	YsFileIO::File fp(recentFn,"r");
	if(nullptr!=fp)
	{
		YsTextFileInputStream inStream(fp);
		recent.Open(inStream);
		recent.PopulateMenu(*fileRecent,16,&FsGuiMainCanvas::File_Recent,this);
	}
}

void FsGuiMainCanvas::AddRecentlyUsedFile(YsWString fName)
{
	recent.AddFile(fName);
	recent.PopulateMenu(*fileRecent,16,&FsGuiMainCanvas::File_Recent,this);

	auto recentFName=GetRecentFileListFileName();

	YsFileIO::File fp(recentFName,"w");
	if(nullptr!=fp)
	{
		auto outStream=fp.OutStream();
		recent.Save(outStream,16);
	}
}

YsWString FsGuiMainCanvas::GetRecentFileListFileName(void) const
{
	auto userDir=YsSpecialPath::GetUserDocDirW();
	YsWString fName;
	fName.MakeFullPathName(userDir,L"template_recent.txt");
	return fName;
}
