/* ////////////////////////////////////////////////////////////

File Name: yswin32fileinfo.cpp
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

#include "../ysfileinfo.h"
#include "../ysencoding.h"
#include <windows.h>


YSRESULT YsFileList::FindFileList(const char path[],const char prefix[],const char postfix[])
{
	YsWString wPath,wPrefix,wPostfix;
	if(NULL!=path)
	{
		YsSystemEncodingToUnicode(wPath,path);
	}
	if(NULL!=prefix)
	{
		YsSystemEncodingToUnicode(wPrefix,prefix);
	}
	if(NULL!=postfix)
	{
		YsSystemEncodingToUnicode(wPostfix,postfix);
	}
	return FindFileList(wPath,wPrefix,wPostfix);
}

YSRESULT YsFileList::FindFileList(const wchar_t path[],const wchar_t prefix[],const wchar_t postfix[])
{
	Initialize();


	HANDLE hFind;
	YsWString fn,fulDir,tst;


	if(NULL!=prefix)
	{
		uPrefix.Set(prefix);
	}
	else
	{
		uPrefix.Set(L"");
	}
	if(NULL!=postfix)
	{
		uPostfix.Set(postfix);
	}
	else
	{
		uPostfix.Set(L"");
	}
	uPrefix.Uncapitalize();
	uPostfix.Uncapitalize();
	this->path.Set(path);


	fulDir.Set(path);
	if(0<fulDir.Strlen() && '\\'!=fulDir.LastChar() && '/'!=fulDir.LastChar())
	{
		fulDir.Append(L"\\");
	}
	fulDir.Append(uPrefix);
	fulDir.Append(L"*");
	fulDir.Append(uPostfix);

	hFind=NULL;
	for(;;)
	{
		WIN32_FIND_DATAW fd;

		if(NULL==hFind)
		{
			hFind=FindFirstFileW(fulDir,&fd);
			if(INVALID_HANDLE_VALUE==hFind)
			{
				return YSERR;
			}
		}
		else
		{
			if(TRUE!=FindNextFileW(hFind,&fd))
			{
				break;
			}
		}

		YSBOOL isDir=YSFALSE;
		unsigned long sizeHigh=0,sizeLow=0;
		if(0!=(FILE_ATTRIBUTE_DIRECTORY&fd.dwFileAttributes))
		{
			isDir=YSTRUE;
			sizeHigh=0;
			sizeLow=0;
		}
		else
		{
			isDir=YSFALSE;
			sizeHigh=fd.nFileSizeHigh;
			sizeLow=fd.nFileSizeLow;
		}

		TestAndAddFileInfo(fd.cFileName,isDir,sizeHigh,sizeLow);
	}

	if(NULL!=hFind)
	{
		FindClose(hFind);
	}

	return YSOK;
}


