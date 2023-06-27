/* ////////////////////////////////////////////////////////////

File Name: yswin32fileio.cpp
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

#include "../ysfileio.h"

#include <direct.h>

const char *YsFileIO::Getcwd(YsString &cwd)
{
	char cwdIn[_MAX_PATH];
	_getcwd(cwdIn,_MAX_PATH);
	cwd.Set(cwdIn);
	return cwd;
}

const wchar_t *YsFileIO::Getcwd(YsWString &cwd)
{
	wchar_t cwdIn[_MAX_PATH];
	_wgetcwd(cwdIn,_MAX_PATH);
	cwd.Set(cwdIn);
	return cwd;
}

YSRESULT YsFileIO::ChDir(const char dirName[])
{
	if(0==_chdir(dirName))
	{
		return YSOK;
	}
	return YSERR;
}

YSRESULT YsFileIO::ChDir(const wchar_t dirName[])
{
	if(0==_wchdir(dirName))
	{
		return YSOK;
	}
	return YSERR;
}

YSRESULT YsFileIO::MkDir(const char dirName[])
{
	if(0==_mkdir(dirName))
	{
		return YSOK;
	}
	return YSERR;
}

YSRESULT YsFileIO::MkDir(const wchar_t dirName[])
{
	if(0==_wmkdir(dirName))
	{
		return YSOK;
	}
	return YSERR;
}

FILE *YsFileIO::Fopen(const wchar_t fn[],const char mode[])
{
	YsWString wmode;
	wmode.SetUTF8String(mode);
	return _wfopen(fn,wmode);
}

FILE *YsFileIO::Fopen(const char fn[],const char mode[])
{
	return fopen(fn,mode);
}

YSRESULT YsFileIO::Remove(const wchar_t fn[])
{
	if(0==_wremove(fn))
	{
		return YSOK;
	}
	return YSERR;
}

YSRESULT YsFileIO::Remove(const char fn[])
{
	if(0==remove(fn))
	{
		return YSOK;
	}
	return YSERR;
}

#include <windows.h>

/* static */ YsWString YsFileIO::GetRealPath(const wchar_t fn[])
{
	wchar_t buf[MAX_PATH];
	if(0<GetFullPathNameW(fn,MAX_PATH,buf,nullptr))
	{
		return YsWString(buf);
	}
	return YsWString(fn);
}
