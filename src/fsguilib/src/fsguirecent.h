/* ////////////////////////////////////////////////////////////

File Name: fsguirecent.h
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

#ifndef FSGUIRECENT_IS_INCLUDED
#define FSGUIRECENT_IS_INCLUDED
/* { */

#include <ysclass.h>
#include "fsgui.h"

class FsGuiRecentFiles
{
private:
	YsArray <YsWString> recentFileArray;

public:
	FsGuiRecentFiles();
	~FsGuiRecentFiles();

	void CleanUp(void);

	void Save(YsTextOutputStream &outStream,YSSIZE_T nRecent) const;
	void Open(YsTextInputStream &inStream);

	void AddFile(const YsWString &fn);

	void PopulateMenu(class FsGuiPopUpMenu &menu,YSSIZE_T nRecent,void (*callback)(void *,FsGuiPopUpMenu *,FsGuiPopUpMenuItem *)=NULL,void *contextPtr=NULL) const;

	template <class objType>
	void PopulateMenu(class FsGuiPopUpMenu &menu,YSSIZE_T nRecent,void (objType::*func)(FsGuiPopUpMenuItem *),objType *thisPtr) const
	{
		int fskey=FSKEY_1;
		menu.CleanUp();
		for(auto &fn : recentFileArray)
		{
			menu.AddTextItem(0,fskey,fn)->BindCallBack(func,thisPtr);
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
};

/* } */
#endif
