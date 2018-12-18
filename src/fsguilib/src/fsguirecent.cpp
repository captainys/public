/* ////////////////////////////////////////////////////////////

File Name: fsguirecent.cpp
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

#include "fsguirecent.h"
#include "fsguipopupmenu.h"
#include <fssimplewindow.h>

FsGuiRecentFiles::FsGuiRecentFiles()
{
}

FsGuiRecentFiles::~FsGuiRecentFiles()
{
}

void FsGuiRecentFiles::CleanUp(void)
{
	recentFileArray.CleanUp();
}

void FsGuiRecentFiles::Save(YsTextOutputStream &outStream,YSSIZE_T nRecent) const
{
	for(auto &fn : recentFileArray)
	{
		YsString str;
		str.EncodeUTF8 <wchar_t> (fn);
		outStream.Printf("%s\n",str.Txt());

		--nRecent;
		if(0==nRecent)
		{
			break;
		}
	}
}

void FsGuiRecentFiles::Open(YsTextInputStream &inStream)
{
	CleanUp();

	YsString str;
	while(NULL!=inStream.Gets(str))
	{
		YsWString wStr;
		wStr.SetUTF8String(str);
		recentFileArray.Append(wStr);
	}
}

void FsGuiRecentFiles::AddFile(const YsWString &fn)
{
	for(YSSIZE_T idx=0; idx<recentFileArray.GetN(); ++idx)
	{
		if(0==YsWString::Strcmp(fn,recentFileArray[idx]))
		{
			while(0<idx)
			{
				YsWString tmp=(YsWString &&)recentFileArray[idx-1];
				recentFileArray[idx-1]=(YsWString &&)recentFileArray[idx];
				recentFileArray[idx]=(YsWString &&)tmp;
				--idx;
			}
			return;
		}
	}
	recentFileArray.Insert(0,fn);
}

void FsGuiRecentFiles::PopulateMenu(class FsGuiPopUpMenu &menu,YSSIZE_T nRecent,void (*callback)(void *,FsGuiPopUpMenu *,FsGuiPopUpMenuItem *),void *contextPtr) const
{
	int fskey=FSKEY_1;
	menu.CleanUp();
	for(auto &fn : recentFileArray)
	{
		menu.AddTextItem(0,fskey,fn,callback,contextPtr);
		if(fskey<FSKEY_9)
		{
			++fskey;
		}

		--nRecent;
		if(0>=nRecent)
		{
			break;
		}
	}
}
