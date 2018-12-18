/* ////////////////////////////////////////////////////////////

File Name: yswin32specialpath.cpp
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

#ifdef PSAPI_VERSION
	#undef PSAPI_VERSION
#endif
#define PSAPI_VERSION 1

#include "../ysspecialpath.h"
#include <windows.h>
#include <shlobj.h>

// Memo:
//   Experiments indicate that SHGetSpecialFolderPath may not return FALSE even though the
//   folder does not exist.  To detect an error, initialize the buffer with the empty
//   string, and check if the buffer is populated.

YSRESULT YsSpecialPath::GetProgramFileName(YsString &path)
{
	char fn[MAX_PATH];
	GetModuleFileNameA(NULL,fn,MAX_PATH);
	path.Set(fn);
	return YSOK;
}

YSRESULT YsSpecialPath::GetProgramFileName(YsWString &path)
{
	wchar_t fn[MAX_PATH];
	GetModuleFileNameW(NULL,fn,MAX_PATH);
	path.Set(fn);
	return YSOK;
}

YSRESULT YsSpecialPath::GetProgramBaseDir(YsString &path)
{
	char buf[MAX_PATH];
	GetModuleFileNameA(NULL,buf,MAX_PATH);

	YsString ful(buf),fil;
	ful.SeparatePathFile(path,fil);
	return YSOK;
}

YSRESULT YsSpecialPath::GetProgramBaseDir(YsWString &path)
{
	wchar_t buf[MAX_PATH];
	GetModuleFileNameW(NULL,buf,MAX_PATH);

	YsWString ful(buf),fil;
	ful.SeparatePathFile(path,fil);
	return YSOK;
}

YSRESULT YsSpecialPath::GetUserDir(YsString &path)
{
	char buf[MAX_PATH];
	buf[0]=0;
	if(TRUE==SHGetSpecialFolderPathA(NULL,buf,CSIDL_PROFILE,TRUE) && 0!=buf[0])
	{
		path.Set(buf);
		return YSOK;
	}
	return YSERR;
}

YSRESULT YsSpecialPath::GetUserDir(YsWString &path)
{
	wchar_t buf[MAX_PATH];
	buf[0]=0;
	if(TRUE==SHGetSpecialFolderPathW(NULL,buf,CSIDL_PROFILE,TRUE) && 0!=buf[0])
	{
		path.Set(buf);
		return YSOK;
	}
	return YSERR;
}

YSRESULT YsSpecialPath::GetUserDocDir(YsString &path)
{
	char buf[MAX_PATH];
	buf[0]=0;
	if(TRUE==SHGetSpecialFolderPathA(NULL,buf,CSIDL_PERSONAL,TRUE) && 0!=buf[0])
	{
		path.Set(buf);
		return YSOK;
	}
	return YSERR;
}

YSRESULT YsSpecialPath::GetUserDocDir(YsWString &path)
{
	wchar_t buf[MAX_PATH];
	buf[0]=0;
	if(TRUE==SHGetSpecialFolderPathW(NULL,buf,CSIDL_PERSONAL,TRUE) && 0!=buf[0])
	{
		path.Set(buf);
		return YSOK;
	}
	return YSERR;
}

YSRESULT YsSpecialPath::GetUserAppDataDir(YsString &path)
{
	char buf[MAX_PATH];
	buf[0]=0;
	if(TRUE==SHGetSpecialFolderPathA(NULL,buf,CSIDL_APPDATA,TRUE) && 0!=buf[0])
	{
		path.Set(buf);
		return YSOK;
	}
	return YSERR;
}

YSRESULT YsSpecialPath::GetUserAppDataDir(YsWString &wpath)
{
	wchar_t buf[MAX_PATH];
	buf[0]=0;
	if(TRUE==SHGetSpecialFolderPathW(NULL,buf,CSIDL_APPDATA,TRUE) && 0!=buf[0])
	{
		wpath.Set(buf);
		return YSOK;
	}
	return YSERR;
}

YSRESULT YsSpecialPath::GetAllUsersDir(YsString &path)
{
	char buf[MAX_PATH];
	buf[0]=0;
	if(TRUE==SHGetSpecialFolderPathA(NULL,buf,CSIDL_COMMON_APPDATA,TRUE) && 0!=buf[0])
	{
		path.Set(buf);
		return YSOK;
	}
	return YSERR;
}

YSRESULT YsSpecialPath::GetAllUsersDir(YsWString &path)
{
	wchar_t buf[MAX_PATH];
	buf[0]=0;
	if(TRUE==SHGetSpecialFolderPathW(NULL,buf,CSIDL_COMMON_APPDATA,TRUE) && 0!=buf[0])
	{
		path.Set(buf);
		return YSOK;
	}
	return YSERR;
}

