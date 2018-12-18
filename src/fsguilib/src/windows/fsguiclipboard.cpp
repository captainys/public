/* ////////////////////////////////////////////////////////////

File Name: fsguiclipboard.cpp
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

#include <ysclass.h>
#include "../fsguiclipboard.h"
#include <windows.h>

YSRESULT FsGuiCopyStringToClipBoard(const YsString &str)
{
	if(TRUE==OpenClipboard(NULL))
	{
		EmptyClipboard();

		// UNICODE version
		YsWString wStr;
		wStr.SetUTF8String(str);
		HANDLE hClipBoard=GlobalAlloc(GMEM_MOVEABLE,sizeof(wchar_t)*(wStr.Strlen()+1));
		if(NULL!=hClipBoard)
		{
			wchar_t *wStrDst=(wchar_t *)GlobalLock(hClipBoard);
			if(nullptr!=wStrDst)
			{
				for(int i=0; i<=wStr.Strlen(); ++i)
				{
					wStrDst[i]=wStr[i];
				}
				if(NULL!=SetClipboardData(CF_UNICODETEXT,hClipBoard))
				{
				}
			}
		}

		// ASCII version
		//const char *newTxt=str;
		//HANDLE hClipBoard=GlobalAlloc(GMEM_MOVEABLE,strlen(newTxt)+1);
		//if(NULL!=hClipBoard)
		//{
		//	char *str=(char *)GlobalLock(hClipBoard);
		//	if(NULL!=str)
		//	{
		//		strcpy(str,newTxt);
		//		GlobalUnlock(hClipBoard);
		//		if(NULL!=SetClipboardData(CF_TEXT,hClipBoard))
		//		{
		//		}
		//	}
		//}
		CloseClipboard();
	}
	return YSOK;
}

YSRESULT FsGuiCopyStringFromClipBoard(YsString &str)
{
	// UNICODE Version
	if(TRUE==IsClipboardFormatAvailable(CF_UNICODETEXT) &&
	   TRUE==OpenClipboard(NULL))
	{
		HANDLE hClipBoard=GetClipboardData(CF_UNICODETEXT);
		if(NULL!=hClipBoard)
		{
			const wchar_t *wStrPtr=(const wchar_t *)GlobalLock(hClipBoard);
			if(NULL!=wStrPtr)
			{
				YsWString wStr(wStrPtr);
				str.EncodeUTF8<wchar_t>(wStr);
			}
			else
			{
				str="";
			}
			GlobalUnlock(hClipBoard);
		}
		CloseClipboard();
	}
	// ASCII Version
	else if(TRUE==IsClipboardFormatAvailable(CF_TEXT) &&
	   TRUE==OpenClipboard(NULL))
	{
		HANDLE hClipBoard=GetClipboardData(CF_TEXT);
		if(NULL!=hClipBoard)
		{
			const char *strPtr=(const char *)GlobalLock(hClipBoard);
			if(NULL!=strPtr)
			{
				str.Set(strPtr);
			}
			else
			{
				str.Set("");
			}
			GlobalUnlock(hClipBoard);
		}
		CloseClipboard();
	}
	return YSOK;
}
