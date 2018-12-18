/* ////////////////////////////////////////////////////////////

File Name: fssimplefiledialog_win.cpp
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
#include <windows.h>

#include <ysclass.h>
#include <direct.h>


#include "../fssimplefiledialog.h"
#include <fssimplewindow.h>


#pragma comment(lib,"comdlg32.lib")


static void BarToZero(wchar_t str[])
{
	if(NULL!=str)
	{
		for(int i=0; 0!=str[i]; ++i)
		{
			if(str[i]=='|')
			{
				str[i]=0;
			}
		}
	}
}

static void FormatExtension(YsWString &firstext,YsArray <wchar_t> &extformat,const YsArray <YsWString> fileExtensionArray)
{
	if(0==fileExtensionArray.GetN())
	{
		firstext.Set(L"*.*");

		YsWString word(L"(*.*)|*.*||");
		extformat.Set(word.Strlen(),word);
		BarToZero(extformat);
	}
	else
	{
		extformat.Clear();
		firstext.Set(L"");

		YsWString word;
		for(int m=0; m<2; ++m)
		{
			for(int i=0; i<fileExtensionArray.GetN(); ++i)
			{
				if(0!=i)
				{
					word.Append(';');
				}

				YSSIZE_T offset=0;
				if('*'==fileExtensionArray[i][offset])
				{
					++offset;
				}
				if('.'==fileExtensionArray[i][offset])
				{
					++offset;
				}

				word.Append(L"*.");
				word.Append(((const wchar_t *)fileExtensionArray[i])+offset);
			}
			word.Append('|');
		}
		extformat.Append(word.Strlen(),word);

		for(int i=0; i<fileExtensionArray.GetN(); ++i)
		{
			word.Set(L"");

			YSSIZE_T offset=0;
			if('*'==fileExtensionArray[i][offset])
			{
				++offset;
			}
			if('.'==fileExtensionArray[i][offset])
			{
				++offset;
			}

			word.Append(L"*.");
			word.Append(((const wchar_t *)fileExtensionArray[i])+offset);
			word.Append(L"|*.");
			word.Append(((const wchar_t *)fileExtensionArray[i])+offset);
			word.Append(L"|");

			extformat.Append(word.Strlen(),word);

			if(0==i)
			{
				firstext.Append(L"*.");
				firstext.Append(((const wchar_t *)fileExtensionArray[i])+offset);
			}
		}
		extformat.Append('|');
		BarToZero(extformat);
	}
}

static void ExtractFiles(YsArray <YsWString> &fileArray,const wchar_t from[])
{
	YsWString ful(from),pth;

	fileArray.Clear();

	if(0<ful.Strlen() && 0==from[ful.Strlen()+1])
	{
		fileArray.Append(ful);
		YsWString fil;
		ful.SeparatePathFile(pth,fil);
		return;
	}
	else
	{
		pth=ful;
	}

	YSSIZE_T ptr=ful.Strlen()+1;
	while(0!=from[ptr])
	{
		const YsWString fil(from+ptr);

		fileArray.Increment();
		fileArray.GetEnd().MakeFullPathName(pth,fil);

		ptr+=fil.Strlen()+1;
	}
}

static void SearchTopLevelWindow(YsArray <HWND> &hWndAll,HWND hWnd,DWORD procId)
{
	DWORD windowProcId;
	GetWindowThreadProcessId(hWnd,&windowProcId);
	if(windowProcId==procId)
	{
		hWndAll.Append(hWnd);
		return;
	}

	HWND hWndChild=NULL;
	while(NULL!=(hWndChild=FindWindowEx(hWnd,hWndChild,NULL,NULL))!=NULL)
	{
		SearchTopLevelWindow(hWndAll,hWndChild,procId);
	}
}

static HWND GetApplicationMainWindow(void)
{
	YsArray <HWND> hWndAll;
	SearchTopLevelWindow(hWndAll,NULL,GetCurrentProcessId());

	HWND hWndLargest=NULL;
	int largestArea=0;
	for(int i=0; i<hWndAll.GetN(); ++i)
	{
		char str[256];
		GetWindowTextA(hWndAll[i],str,255);
		// printf("hWnd=%08x Title=%s\n",(int)hWndAll[i],str);
		// printf("IsVisible=%d\n",IsWindowVisible(hWndAll[i]));

		RECT rc;
		GetWindowRect(hWndAll[i],&rc);

		const int area=YsAbs((rc.right-rc.left)*(rc.bottom-rc.top));
		// printf("Area=%d square pixels. (%d x %d)\n",area,(rc.right-rc.left),(rc.bottom-rc.top));

		if(TRUE==IsWindowVisible(hWndAll[i]) && largestArea<area)
		{
			hWndLargest=hWndAll[i];
			largestArea=area;
		}
	}

	return hWndLargest;
}

static UINT_PTR CALLBACK FileDialogCallBack(HWND /*hDlg*/,UINT msg,WPARAM /*wp*/,LPARAM /*lp*/)
{
	// Attempted to relocate the file dialog so that it covers the parent window.
	// However, it didn't work for the reason unknown.
	// Quick web search came back with the following two methods, neither of which worked.
	// It relocates and resizes, but makes the vertical size of the window same as the screen height instead of the parent-window height.
	switch(msg)
	{
	case WM_NOTIFY:
		{
			/* NMHDR *nmHdr=(NMHDR *)lp;
			if(CDN_INITDONE==nmHdr->code)
			{
			} */
		}
		break;
	case WM_INITDIALOG:
		{
			/* HWND hMainWnd=GetApplicationMainWindow();
			RECT rect;

			GetWindowRect(hMainWnd,&rect);
			const int wid=rect.right-rect.left;
			const int hei=rect.bottom-rect.top;
			SetWindowPos(hDlg,HWND_TOP,rect.left,rect.top,wid,hei,SWP_NOZORDER); */
		}
		break;
	}
	return 0;  // Fall back to the default procedure.
}

class FsSimpleFileDialog_PlatformSpecificData
{
public:
	enum
	{
		FILENAME_BUFFER_LENGTH=65536
	};

	OPENFILENAMEW para;
	wchar_t selected[FILENAME_BUFFER_LENGTH];
	YsArray <wchar_t> extformat;
	YsWString firstext;
	YsWString initialPath;
};

void FsSimpleFileDialog::Start(void)
{
	if(YSTRUE==busy)
	{
		// Prevent re-entrance.
		return;
	}

	YSRESULT res=YSERR;

	FsSimpleFileDialog_PlatformSpecificData param;

	FsPollDevice();
	FsClearEventQueue();


	FsSimpleFileDialog *nonConst=(FsSimpleFileDialog *)this;

	busy=YSTRUE;

	int result;
	wchar_t curpath[MAX_PATH];
	HINSTANCE module;

	module=GetModuleHandle(NULL);

	FormatExtension(param.firstext,param.extformat,this->fileExtensionArray);

	OPENFILENAMEW &para=param.para;
	ZeroMemory(&para,sizeof(OPENFILENAME));

	para.lStructSize=sizeof(OPENFILENAME);
	para.hwndOwner=GetApplicationMainWindow();
	para.hInstance=module;
	para.lpstrFilter=param.extformat;
	para.lpstrCustomFilter=NULL;
	para.nFilterIndex=0;

	_wgetcwd(curpath,MAX_PATH);
	if(0<defaultFileName.Strlen())
	{
		YsWString fil;
		if(YSOK==defaultFileName.SeparatePathFile(param.initialPath,fil))
		{
			if(FsSimpleFileDialog_PlatformSpecificData::FILENAME_BUFFER_LENGTH<=fil.Strlen())
			{
				fil.SetLength(FsSimpleFileDialog_PlatformSpecificData::FILENAME_BUFFER_LENGTH-1);
			}
			YsWString::Strcpy(param.selected,fil);


			if('\\'==param.initialPath.LastChar() || '/'==param.initialPath.LastChar())
			{
				param.initialPath.Append('.');
			}
			if(0==param.initialPath.Strlen())
			{
				param.initialPath.Set(curpath);
			}
		}
		else
		{
			YsWString fil=this->defaultFileName;
			if(FsSimpleFileDialog_PlatformSpecificData::FILENAME_BUFFER_LENGTH<=fil.Strlen())
			{
				fil.SetLength(FsSimpleFileDialog_PlatformSpecificData::FILENAME_BUFFER_LENGTH-1);
			}
			YsWString::Strcpy(param.selected,fil);
			param.initialPath.Set(curpath);
		}
	}
	else
	{
		param.selected[0]=0;
		param.initialPath.Set(curpath);
	}

	for(int i=0; i<param.initialPath.Strlen(); ++i)
	{
		if(param.initialPath[i]=='/')
		{
			param.initialPath.Set(i,'\\');
		}
	}

	para.lpstrFile=param.selected;
	para.lpstrInitialDir=param.initialPath;



	para.nMaxFile=FsSimpleFileDialog_PlatformSpecificData::FILENAME_BUFFER_LENGTH;
	para.lpstrFileTitle=NULL;  // <- This will be filename part of the selected file.  What if the multi selection?
	para.lpstrTitle=this->title;
	para.Flags=OFN_ENABLEHOOK|OFN_ENABLESIZING|OFN_EXPLORER;
	if(YSTRUE==multiSelect)
	{
		para.Flags|=OFN_ALLOWMULTISELECT;
	}
	if(MODE_OPEN==mode)
	{
		para.Flags|=OFN_FILEMUSTEXIST;
	}
	if(MODE_SAVE==mode)
	{
		para.Flags|=OFN_OVERWRITEPROMPT;
		para.Flags|=OFN_HIDEREADONLY;
	}

	para.lpstrDefExt=((const wchar_t *)param.firstext)+2;

	para.lCustData=(LPARAM)this;
	para.lpfnHook=FileDialogCallBack;

	result=GetOpenFileNameW(&para);
	_wchdir(curpath);

	FsPollDevice();
	FsClearEventQueue();

	if(result!=0)
	{
		FsSimpleFileDialog *nonConst=(FsSimpleFileDialog *)this;
		if(YSTRUE==multiSelect)
		{
			ExtractFiles(nonConst->selectedFileArray,param.selected);
		}
		else
		{
			nonConst->selectedFileArray.Set(1,NULL);
			nonConst->selectedFileArray[0].Set(param.selected);
		}
		res=YSOK;
	}
	else
	{
		FsSimpleFileDialog *nonConst=(FsSimpleFileDialog *)this;
		nonConst->selectedFileArray.Clear();
		res=YSERR;
	}

	busy=YSFALSE;

	OnClose((int)res);
	if(nullptr!=closeModalCallBack)
	{
		closeModalCallBack(this,(int)res);
	}
}

void FsSimpleFileDialog::AllocatePlatformSpecificData(void)
{
}
void FsSimpleFileDialog::DeletePlatformSpecificData(void)
{
}
