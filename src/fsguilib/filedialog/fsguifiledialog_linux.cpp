/* ////////////////////////////////////////////////////////////

File Name: fsguifiledialog_linux.cpp
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


#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>


#include <ysclass.h>
#include <ysbitmap.h>
#include <fsgui.h>
#include <fssimplewindow.h>

#include "fsguifiledialog.h"


////////////////////////////////////////////////////////////
// Getting file list >>

static YSBOOL MatchExtension(const YsString &filename,const YSSIZE_T nExt,const YsString ext[])
{
	for(YSSIZE_T extIdx=0; extIdx<nExt; ++extIdx)
	{
		if(0==strcmp(filename.Txt()+filename.Strlen()-ext[extIdx].Strlen(),ext[extIdx]))
		{
			return YSTRUE;
		}
	}
	return YSFALSE;
}

static YSRESULT FsFindFileDirList(YsArray <YsString> &filelist,YsArray <YsString> &dirlist,const char dir[],YSSIZE_T nExt,const YsWString ext[])
{
	int i;
	long hdl;
	unsigned attrib;
	YsString fn,tst,fulDir,uPrefix;
	DIR *dp;
	struct dirent *dr;
	YSBOOL matchAllExtension=YSFALSE;

	if(0==nExt)
	{
		matchAllExtension=YSTRUE;
	}

	YsArray <YsString> uExtArray;
	for(YSSIZE_T extIdx=0; extIdx<nExt; ++extIdx)
	{
		uExtArray.Increment();
		uExtArray.GetEnd().EncodeUTF8 <wchar_t> (ext[extIdx]);
		uExtArray.GetEnd().Uncapitalize();
	}

	if((dp=opendir(dir))==NULL)
	{
		return YSERR;
	}

	filelist.Set(0,NULL);

	hdl=-1;
	while((dr=readdir(dp))!=NULL)
	{
		if(dr->d_ino!=0 && dr->d_name[0]!='.')
		{
			struct stat st;

			fn.Set(dir);
			fn.Append("/");
			fn.Append(dr->d_name);

			stat(fn,&st);
			if((st.st_mode&S_IFMT)==S_IFDIR)
			{
				fn.Set(dr->d_name);
				dirlist.Append(fn);
			}
			else
			{
				fn.Set(dr->d_name);

				tst.Set(dr->d_name);
				tst.Uncapitalize();
				if(strncmp(tst,uPrefix,uPrefix.Strlen())==0 &&
				   (YSTRUE==matchAllExtension || YSTRUE==MatchExtension(tst,uExtArray.GetN(),uExtArray)))
				{
					filelist.Append(fn);
				}
			}
		}
	}

	return YSOK;
}
// Getting file list <<
////////////////////////////////////////////////////////////



////////////////////////////////////////////////////////////
// Real File Dialog >>

class FsGuiRealFileDialog : public FsGuiDialog
{
public:
	FsGuiStatic *pathTxt;
	FsGuiListBox *lbx;
	FsGuiTextBox *txt;
	FsGuiDropList *extDrp;
	FsGuiTextBox *filterTxt;
	FsGuiButton *openBtn;
	FsGuiButton *cancelBtn;

#ifdef _WIN32
	FsGuiListBox *drvLbx;
	FsGuiButton *moveDrvBtn;
#else
	FsGuiButton *homeDirBtn;
#endif

	FsGuiButton *upBtn;
	FsGuiButton *moveDirBtn;

	YSBOOL forOpenFile;
	YSBOOL multiSelect;
	YsArray <YsWString> extArray;
	YsArray <YsWString> selectedExtArray;
	YsArray <YsWString> filterArray;

	YSRESULT res;
	YsArray <YsWString> selectedFileArray;

	YsString curPath,curName;

	FsGuiRealFileDialog();
	void Initialize(void);

	YSRESULT MakeDialog(const wchar_t title[],YSBOOL forOpenFile,YSBOOL multiSelect);
	void SetExtensionArray(YSSIZE_T nExt,const YsWString extArray[]);
	YSBOOL PassFilterTest(const char fn[]) const;
	void ClearFilter(void);
	YSRESULT MakeFileList(const char iniPath[],const char iniName[]);
	YSRESULT MakeFileList(const wchar_t iniPath[],const wchar_t iniName[]);

private:
	void MoveDirectoryRelative(const char dir[]);
	void MoveDirectoryAbsolute(const char dir[]);
	YSBOOL MoveToSelectedDirectory(void);
	YSBOOL MoveToDirectory(int sel);
#ifdef _WIN32
	YSBOOL MoveToSelectedDrive(void);
#endif
	void Open(void);

private:
	void AddExtension(YsWString &fn) const;

public:
	virtual void OnButtonClick(FsGuiButton *btn);
	virtual void OnListBoxSelChange(FsGuiListBox *lbx,int prevSel);
	virtual void OnListBoxDoubleClick(FsGuiListBox *lbx,int clickedOn);
	virtual void OnDropListSelChange(FsGuiDropList *lbx,int prevSel);
	virtual void OnTextBoxChange(FsGuiTextBox *txt);
};

FsGuiRealFileDialog::FsGuiRealFileDialog()
{
	Initialize();
}

void FsGuiRealFileDialog::Initialize(void)
{
	FsGuiDialog::Initialize();

	lbx=NULL;
	txt=NULL;
	openBtn=NULL;
	cancelBtn=NULL;

	forOpenFile=YSFALSE;

	res=YSERR;
	selectedFileArray.Clear();
	selectedExtArray.Clear();
	filterArray.Clear();
}


YSRESULT FsGuiRealFileDialog::MakeDialog(const wchar_t title[],YSBOOL forOpenFile,YSBOOL multiSelect)
{
	this->multiSelect=multiSelect;

	SetTextMessage(title);

	pathTxt=AddStaticText(1,FSKEY_NULL,"",40,3,YSTRUE);
#ifndef _WIN32
	homeDirBtn=AddTextButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,"HOME",YSTRUE);
#endif
	upBtn=AddTextButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,"UP",YSFALSE);
	moveDirBtn=AddTextButton(0,FSKEY_NULL,FSGUI_PUSHBUTTON,"MOVE",YSFALSE);
	moveDirBtn->Disable();
	filterTxt=AddTextBox(0,FSKEY_NULL,"Search Filter","",48,YSTRUE);
	lbx=AddListBox(2,FSKEY_NULL,"",0,(const char **)NULL,12,48,YSTRUE);
	lbx->SetMultiSelect(multiSelect);
	if(forOpenFile!=YSTRUE)
	{
		txt=AddTextBox(3,FSKEY_NULL,"","",32,YSTRUE);
	}

	extDrp=AddEmptyDropList(0,FSKEY_NULL,"",5,32,32,YSTRUE);
	if(0==extArray.GetN())
	{
		extDrp->AddString("*.*",YSTRUE);
	}
	else
	{
		extDrp->AddString("*.*",YSFALSE);

		YsWString allExt;
		for(YSSIZE_T idx=0; idx<extArray.GetN(); ++idx)
		{
			allExt.Append(extArray[idx]);
		}
		extDrp->AddString(allExt,YSTRUE);

		for(YSSIZE_T idx=0; idx<extArray.GetN(); ++idx)
		{
			extDrp->AddString(extArray[idx],YSFALSE);
		}
	}

	openBtn=AddTextButton(4,FSKEY_ENTER,FSGUI_PUSHBUTTON,"Open",YSTRUE);
	cancelBtn=AddTextButton(5,FSKEY_ESC,FSGUI_PUSHBUTTON,"Cancel",YSFALSE);

#ifdef _WIN32
	YsArray <YsString> drvLetter;
	YsArray <const char *> drvLetterPtr;
	int i;
	drvLetter.Set('Z'-'A'+1,NULL);
	drvLetterPtr.Set('Z'-'A'+1,NULL);
	for(i='A'; i<='Z'; i++)
	{
		drvLetter[i-'A'].Append((char)i);
		drvLetter[i-'A'].Append(":");
		drvLetterPtr[i-'A']=drvLetter[i-'A'];
	}

	drvLbx=AddListBox(6,FSKEY_NULL,"Drive",drvLetter.GetN(),drvLetterPtr,4,10,YSTRUE);
	moveDrvBtn=AddTextButton(7,FSKEY_NULL,FSGUI_PUSHBUTTON,"Change Drive",YSTRUE);
#endif

	SetTransparency(YSFALSE);

	Fit();

	return YSOK;
}

void FsGuiRealFileDialog::SetExtensionArray(YSSIZE_T nExt,const YsWString ext[])
{
	this->extArray.Clear();
	for(YSSIZE_T idx=0; idx<nExt; ++idx)
	{
		this->extArray.Increment();
		this->extArray.GetEnd().Set(ext[idx]);
	}
	this->selectedExtArray=this->extArray;
}

YSBOOL FsGuiRealFileDialog::PassFilterTest(const char fn[]) const
{
	YsWString wFn;
	wFn.SetUTF8String(fn);
	for(int filterIdx=0; filterIdx<filterArray.GetN(); ++filterIdx)
	{
		if(YSTRUE!=wFn.FindWord(NULL,filterArray[filterIdx].Txt()))
		{
			return YSFALSE;
		}
	}
	return YSTRUE;
}

void FsGuiRealFileDialog::ClearFilter(void)
{
	filterTxt->SetText("");
	filterArray.Clear();
}

YSRESULT FsGuiRealFileDialog::MakeFileList(const char iniPath[],const char iniName[])
{
	YsArray <YsString> fileList,dirList;

	if(lbx!=NULL && FsFindFileDirList(fileList,dirList,iniPath,selectedExtArray.GetN(),selectedExtArray)==YSOK)
	{
		int i,id;

		lbx->ClearChoice();

		id=lbx->AddString("..",YSFALSE);
		lbx->SetDirectoryFlag(id,YSTRUE);

		YsArray <const char *> sortedDirList;
		for(int dirIdx=0; dirIdx<dirList.GetN(); ++dirIdx)
		{
			if(YSTRUE==PassFilterTest(dirList[dirIdx]))
			{
				sortedDirList.Append(dirList[dirIdx].Txt());
			}
		}

		YsQuickSortString <int> ((int)sortedDirList.GetN(),sortedDirList.GetEditableArray(),NULL);
		for(i=0; i<sortedDirList.GetN(); i++)
		{
			if(strcmp(sortedDirList[i],".")!=0)
			{
				id=lbx->AddString(sortedDirList[i],YSFALSE);
				lbx->SetDirectoryFlag(id,YSTRUE);
			}
		}


		YsArray <const char *> sortedFileList;
		for(int i=0; i<fileList.GetN(); ++i)
		{
			if(YSTRUE==PassFilterTest(fileList[i]))
			{
				sortedFileList.Append(fileList[i].Txt());
			}
		}
		YsQuickSortString <int> (sortedFileList.GetN(),sortedFileList,NULL);
		for(i=0; i<sortedFileList.GetN(); i++)
		{
			id=lbx->AddString(sortedFileList[i],YSFALSE);
			lbx->SetDirectoryFlag(id,YSFALSE);
		}

		curPath.Set(iniPath);
		curName.Set(iniName);

		if(txt!=NULL)
		{
			txt->SetText(iniName);
		}

		if(pathTxt!=NULL)
		{
			pathTxt->SetText(curPath);
		}

		return YSOK;
	}
	return YSERR;
}

YSRESULT FsGuiRealFileDialog::MakeFileList(const wchar_t wIniPath[],const wchar_t wIniName[])
{
	YsString iniPath,iniName,ext;
	iniPath.EncodeUTF8 <wchar_t> (wIniPath);
	iniName.EncodeUTF8 <wchar_t> (wIniName);
	return MakeFileList(iniPath,iniName);
}

void FsGuiRealFileDialog::MoveDirectoryRelative(const char dir[])
{
	if(strcmp(dir,"..")==0)
	{
		curPath.Set(this->curPath);
		if('/'==curPath.LastChar() || '\\'==curPath.LastChar())
		{
			curPath.BackSpace();
		}

		for(int i=curPath.Strlen(); i>=0; i--)
		{
			if(curPath[i]=='/' || curPath[i]=='\\')
			{
				curPath.Set(i,this->curPath);
				break;
			}
		}

		if(curPath.Strlen()==0)  // 2009/06/29
		{                        // 2009/06/29
			curPath.Set("/");    // 2009/06/29
		}                        // 2009/06/29

		ClearFilter();
		MakeFileList(curPath,curName);
	}
	else if(strcmp(dir,".")==0)
	{
		// Do nothing.
	}
	else
	{
		curPath.Set(this->curPath);
#ifdef _WIN32
		curPath.Append("\\");
#else
		curPath.Append("/");
#endif
		curPath.Append(dir);
		MakeFileList(curPath,curName);
	}
}

void FsGuiRealFileDialog::MoveDirectoryAbsolute(const char dir[])
{
	const YsString prevCurPath=this->curPath;
	ClearFilter();
	if(YSOK!=MakeFileList(dir,curName))
	{
		MakeFileList(prevCurPath,curName);
	}
}

YSBOOL FsGuiRealFileDialog::MoveToSelectedDirectory(void)
{
	YsString txtA;
	if(NULL!=txt)
	{
		txt->GetText(txtA);
	}

	if(txtA[0]!=0)
	{
		YsString str;
		txtA.Capitalize();
		for(int i=0; YSOK==lbx->GetString(str,i); i++)
		{
			YsString txtB(str);
			txtB.Capitalize();

			if(strcmp(txtA,txtB)==0 && lbx->IsDirectory(i)==YSTRUE)
			{
				ClearFilter();
				MoveDirectoryRelative(str);
				return YSTRUE;
			}
		}

		return YSFALSE;
	}

	return MoveToDirectory(lbx->GetSelection());
}

YSBOOL FsGuiRealFileDialog::MoveToDirectory(int sel)
{
	if(0<=sel && sel<lbx->GetNumChoice() && lbx->IsDirectory(sel)==YSTRUE)
	{
		YsString str;
		lbx->GetString(str,sel);
		ClearFilter();
		MoveDirectoryRelative(str);
		return YSTRUE;
	}
	return YSFALSE;
}

#ifdef _WIN32
YSBOOL FsGuiRealFileDialog::MoveToSelectedDrive(void)
{
	const char *drv;
	drv=drvLbx->GetSelectedString();
	if(drv!=NULL)
	{
		ClearFilter();
		MakeFileList(drv,curName);
		return YSTRUE;
	}
	return YSFALSE;
}
#endif

void FsGuiRealFileDialog::AddExtension(YsWString &fn) const
{
	auto ext=fn.GetExtension();
	for(auto regExt : extArray)
	{
		if(0==ext.STRCMP(regExt))
		{
			return;
		}
	}

	if(NULL!=extDrp && 0<extDrp->GetSelection())  // If GetSelection()==0, it is "*.*"
	{
		extDrp->GetSelectedString(ext);
		if(ext[0]=='*')
		{
			ext.Delete(0);
		}
		fn.Append(ext);
	}
}

void FsGuiRealFileDialog::Open(void)
{
	if(MoveToSelectedDirectory()!=YSTRUE)
	{
		YsWString pth;
		pth.SetUTF8String(curPath);

		YsWString txtContent;
		if(NULL!=txt)
		{
			txt->GetText(txtContent);
			AddExtension(txtContent);
		}

		if(YSTRUE!=multiSelect)
		{
			YsWString selStr;
			if(NULL!=lbx)
			{
				lbx->GetSelectedString(selStr);
				// AddExtension(selStr);  Don't add extension if it is from the list box.
			}

			if(0<txtContent.Strlen())
			{
				selectedFileArray.Set(1,NULL);
				selectedFileArray[0].MakeFullPathName(pth,txtContent);
				res=YSOK;
				CloseModalDialog(YSTRUE);
			}
			else if(0<selStr.Strlen())
			{
				selectedFileArray.Set(1,NULL);
				selectedFileArray[0].MakeFullPathName(pth,selStr);
				res=YSOK;
				CloseModalDialog(YSTRUE);
			}
		}
		else
		{
			selectedFileArray.Clear();

			YSBOOL txtContentIncluded=YSFALSE;
			for(int choiceIdx=0; choiceIdx<lbx->GetNumChoice(); ++choiceIdx)
			{
				if(YSTRUE==lbx->IsSelected(choiceIdx))
				{
					YsWString selStr;
					lbx->GetString(selStr,choiceIdx);
					// AddExtension(selStr);  Don't add extension if it is from the list box.
					selectedFileArray.Increment();
					selectedFileArray.GetEnd().MakeFullPathName(pth,selStr);
					if(0==YsWString::Strcmp(selStr,txtContent))
					{
						txtContentIncluded=YSTRUE;
					}
				}
			}

			if(YSTRUE!=txtContentIncluded && 0<txtContent.Strlen())
			{
				selectedFileArray.Increment();
				selectedFileArray.GetEnd().MakeFullPathName(pth,txtContent);
			}

			if(0<selectedFileArray.GetN())
			{
				res=YSOK;
				CloseModalDialog(YSTRUE);
			}
		}
	}
}

void FsGuiRealFileDialog::OnButtonClick(FsGuiButton *btn)
{
	if(btn==openBtn)
	{
		Open();
	}
	else if(btn==cancelBtn)
	{
		selectedFileArray.Clear();
		res=YSERR;
		CloseModalDialog(YSERR);
	}
	else if(btn==upBtn)
	{
		MoveDirectoryRelative("..");
	}
	else if(btn==moveDirBtn)
	{
		const int sel=lbx->GetSelection();
		if(lbx->IsDirectory(sel)==YSTRUE)
		{
			MoveToSelectedDirectory();
		}
	}
#ifdef _WIN32
	else if(btn==moveDrvBtn || drvLbx->CheckAndClearDoubleClick()==YSTRUE)
	{
		MoveToSelectedDrive();
	}
#else
	else if(btn==homeDirBtn)
	{
		const char *homeDir=getenv("HOME");
		if(NULL!=homeDir)
		{
			MoveDirectoryAbsolute(homeDir);
		}
	}
#endif
}

void FsGuiRealFileDialog::OnListBoxSelChange(FsGuiListBox *lbx,int prevSel)
{
	YsString selStr;
	if(NULL!=txt && YSOK==lbx->GetSelectedString(selStr))
	{
		txt->SetText(selStr);
	}
	if(lbx==this->lbx)
	{
		const int sel=lbx->GetSelection();
		moveDirBtn->SetEnabled(lbx->IsDirectory(sel));
	}
}

void FsGuiRealFileDialog::OnListBoxDoubleClick(FsGuiListBox *lbx,int clickedOn)
{
	if(YSTRUE!=multiSelect)
	{
		Open();
	}
	else
	{
		if(YSTRUE!=MoveToDirectory(clickedOn))
		{
			Open();
		}
	}
}

void FsGuiRealFileDialog::OnDropListSelChange(FsGuiDropList *lbx,int prevSel)
{
	if(lbx==extDrp)
	{
		const int sel=extDrp->GetSelection();
		if(0==sel)
		{
			selectedExtArray.Clear();
		}
		else if(1==sel)
		{
			selectedExtArray=extArray;
		}
		else
		{
			YsWString str;
			extDrp->GetSelectedString(str);
			selectedExtArray.Clear();
			selectedExtArray.Append(str);
		}

		MakeFileList(curPath,curName);
	}
}

void FsGuiRealFileDialog::OnTextBoxChange(FsGuiTextBox *txt)
{
	if(txt==filterTxt)
	{
		YsWString filter;
		filterTxt->GetText(filter);
		filter.Arguments(filterArray);
		MakeFileList(curPath,curName);
	}
}

// File Dialog <<

//////////////////////////////////////////////////////////////////////

class FsGuiFileDialog_PlatformSpecificData
{
public:
	FsGuiRealFileDialog realFdlg;
};

void FsGuiFileDialog::CloseModalCallBackFunc(class FsGuiControlBase *,class FsGuiDialog *closedModalDialog,int returnCode)
{
	FsGuiRealFileDialog *realFdlg=(FsGuiRealFileDialog *)closedModalDialog;
	FsGuiFileDialog *fdlg=(FsGuiFileDialog *)(closedModalDialog->GetParent());

	fdlg->selectionMade=YSTRUE;
	fdlg->res=realFdlg->res;
	fdlg->selectedFileArray=realFdlg->selectedFileArray;

	fdlg->CloseModalDialog(returnCode);
	fdlg->busy=YSFALSE;
}

void FsGuiFileDialog::Show(const FsGuiDialog *excluded) const
{
	FsGuiDialog::Show(excluded);

	if(YSTRUE!=busy)
	{
		FsGuiFileDialog *nonConst=(FsGuiFileDialog *)this;

		busy=YSTRUE;

		nonConst->SetBackgroundAlpha(0.1);

		const YSBOOL forOpenFile=(MODE_OPEN==this->mode ? YSTRUE : YSFALSE);

		nonConst->param->realFdlg.Initialize();
		nonConst->param->realFdlg.SetExtensionArray(this->fileExtensionArray.GetN(),this->fileExtensionArray);
		nonConst->param->realFdlg.MakeDialog(this->title,forOpenFile,this->multiSelect);

		const wchar_t *defExt=L"*";
		if(0<fileExtensionArray.GetN())
		{
			defExt=fileExtensionArray[0];
			if(defExt[0]=='*')
			{
				defExt++;
			}
			if(defExt[0]=='.')
			{
				defExt++;
			}
		}


		YsWString iniPath,iniFile;
		if(0<defaultFileName.Strlen())
		{
			defaultFileName.SeparatePathFile(iniPath,iniFile);
			if(0==iniPath.Strlen() || 
			   0==YsWString::Strcmp(iniPath,L"./") || 
			   0==YsWString::Strcmp(iniPath,L"."))
			{
				char cwd[260];
				iniPath.SetUTF8String(getcwd(cwd,259));
			}
		}
		else
		{
			char cwd[260];
			iniPath.SetUTF8String(getcwd(cwd,259));
			iniFile.Set(L"*.");
			iniFile.Append(defExt);
		}

		printf("%ls %ls\n",iniPath.Txt(),iniFile.Txt());

		if(YSOK!=param->realFdlg.MakeFileList(iniPath,iniFile))
		{
			const char *homeDir=getenv("HOME");
			YsWString wHomeDir;
			wHomeDir.SetUTF8String(homeDir);
			if(NULL==homeDir || YSOK!=param->realFdlg.MakeFileList(wHomeDir,iniFile))
			{
				param->realFdlg.MakeFileList(L"/",iniFile);
			}
		}

		nonConst->AttachModalDialog(&this->param->realFdlg);
		this->param->realFdlg.SetCloseModalCallBack(CloseModalCallBackFunc);
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
