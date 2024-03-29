/* ////////////////////////////////////////////////////////////

File Name: fsguifiledialog_mac_cpp.cpp
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
#include <string.h>
#include <stddef.h>

#include <unistd.h>

#include <time.h>

#include <ysclass.h>

#include "fsguifiledialog.h"
#include "fsguifiledialog_mac.h"

#include <fssimplewindow.h>

void FsGuiFileDialog::OnAttach(void)
{
	// 2023/04/03 busy flag must be set here before FsPollDevice, or ShowDialog will re-enter and may call OnCloseModalCallBack too early.
	busy=YSTRUE;

	// 2013/08/14
	// Event queue must be cleared before the dialog.  'O' key-up event
	// may be cancelled during the file dialog.
	FsPollDevice();
	FsClearEventQueue();


	FsGuiFileDialog *nonConst=(FsGuiFileDialog *)this;

	struct FsGuiFileDialog_Mac dlgInfo;
	FsGuiFileDialog_Mac_InitDialog(&dlgInfo);

	// Setting busy here is too late.  See comment above.

	dlgInfo.in_multiSelect=(int)multiSelect;
	dlgInfo.in_mode=(mode==MODE_OPEN ? FSGUIFILEDIALOG_MAC_MODE_OPEN : FSGUIFILEDIALOG_MAC_MODE_SAVE);
	for(YSSIZE_T extIdx=fileExtensionArray.GetN()-1; 0<=extIdx; --extIdx)
	{
		// fsguifiledialog_mac_objc.m is using a LIFO singly-linked linked list, and the last extension added has the first priority.
		YsString utf8;
		utf8.EncodeUTF8 <wchar_t> (fileExtensionArray[extIdx]);
		FsGuiFileDialog_Mac_AddExtension(&dlgInfo,utf8);
	}



	YsWString wIniPath,wIniName;
	if(0<defaultFileName.Strlen())
	{
		if(YSOK==defaultFileName.SeparatePathFile(wIniPath,wIniName))
		{
		}
		else
		{
			char cwd[1024];
			wIniName=this->defaultFileName;
			wIniPath.SetUTF8String(getcwd(cwd,1023));
		}
	}
	else
	{
		char cwd[1024];
		wIniName.SetUTF8String("");
		wIniPath.SetUTF8String(getcwd(cwd,1023));
	}
	if(0==wIniPath.Strlen())
	{
		wIniPath.SetUTF8String(".");
	}

	for(YSSIZE_T idx=wIniName.Strlen()-1; 0<=idx; --idx)
	{
		if('?'==wIniName[idx] || '*'==wIniName[idx])
		{
			wIniName.Delete(idx);
		}
	}



	YsString iniPath,iniName;
	iniPath.EncodeUTF8 <wchar_t> (wIniPath);
	iniName.EncodeUTF8 <wchar_t> (wIniName);

	dlgInfo.defaultPath=iniPath;
	dlgInfo.defaultFile=iniName;

	YsString titleUTF8;
	titleUTF8.EncodeUTF8 <wchar_t> (title);
	dlgInfo.title=titleUTF8;
	
	FsGuiFileDialog_Mac_OpenDialog(&dlgInfo);

	nonConst->res=(YSRESULT)dlgInfo.out_result;

	nonConst->selectedFileArray.Set(0,NULL);
	if(YSOK==nonConst->res)
	{
		FsGuiFileDialog_Mac_StringList *ptr;
		for(ptr=dlgInfo.out_selected; NULL!=ptr; ptr=ptr->next)
		{
			nonConst->selectedFileArray.Increment();
			nonConst->selectedFileArray.GetEnd().SetUTF8String(ptr->fn);
		}
	}

	nonConst->busy=YSFALSE;
	nonConst->selectionMade=YSTRUE;

	FsGuiFileDialog_Mac_CleanUp(&dlgInfo);

	FsPollDevice();
	FsClearEventQueue();
}

void FsGuiFileDialog::Show(const FsGuiDialog *excludeFromDrawing) const
{
	if(YSTRUE!=busy)
	{
		FsGuiFileDialog *nonConst=(FsGuiFileDialog *)this;
		nonConst->CloseModalDialog(res);
	}
}

void FsGuiFileDialog::AllocatePlatformSpecificData(void)
{
	param=NULL;
}

void FsGuiFileDialog::DeletePlatformSpecificData(void)
{
}
