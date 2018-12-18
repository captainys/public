/* ////////////////////////////////////////////////////////////

File Name: ysunixfileio.cpp
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

#include <sys/param.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

const char *YsFileIO::Getcwd(YsString &cwd)
{
	char cwdIn[1024];
	getcwd(cwdIn,1024);
	cwd.Set(cwdIn);
	return cwd;
}

const wchar_t *YsFileIO::Getcwd(YsWString &cwd)
{
	char cwdIn[1024];
	getcwd(cwdIn,1024);
	cwd.SetUTF8String(cwdIn);
	return cwd;
}

YSRESULT YsFileIO::ChDir(const char dirName[])
{
	if(0==chdir(dirName))
	{
		return YSOK;
	}
	return YSERR;
}

YSRESULT YsFileIO::ChDir(const wchar_t dirName[])
{
	YsString utf8;
	utf8.EncodeUTF8(dirName);
	return ChDir(utf8.Txt());  // Hope the system supports UTF8 file name.
}

YSRESULT YsFileIO::MkDir(const char dirName[])
{
	if(0==mkdir(dirName,0755))
	{
		return YSOK;
	}
	return YSERR;
}

YSRESULT YsFileIO::MkDir(const wchar_t dirName[])
{
	YsString utf8;
	utf8.EncodeUTF8(dirName);
	return MkDir(utf8.Txt());  // Hope the system supports UTF8 file name.
}

FILE *YsFileIO::Fopen(const wchar_t fn[],const char mode[])
{
	YsString fnUTF8,modeUTF8;
	fnUTF8.EncodeUTF8 <wchar_t> (fn);
	return fopen(fnUTF8,mode);
}

FILE *YsFileIO::Fopen(const char fn[],const char mode[])
{
	return fopen(fn,mode);
}

YSRESULT YsFileIO::Remove(const wchar_t fn[])
{
	YsString fnUTF8;
	fnUTF8.EncodeUTF8 <wchar_t> (fn);
	if(0==remove(fnUTF8))
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

/* static */ YsWString YsFileIO::GetRealPath(const wchar_t fn[])
{
	YsWString wstr(fn);
	auto utf8=wstr.GetUTF8String();
	auto realpathUtf8=realpath(utf8,nullptr);
	if(nullptr!=realpathUtf8)
	{
		wstr.SetUTF8String(realpathUtf8);
		free(realpathUtf8);
	}
	return wstr;
}
