/* ////////////////////////////////////////////////////////////

File Name: ysmacosxspecialpath.cpp
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

#include "../ysspecialpath.h"

extern "C" bool YsSpecialPath_GetProgramFileName(size_t lBuf,char buf[]);
extern "C" bool YsSpecialPath_GetProgramBaseDir(size_t lBuf,char buf[]);
extern "C" bool YsSpecialPath_GetUserDir(size_t lBuf,char buf[]);
extern "C" bool YsSpecialPath_GetAllUsersDir(size_t lBuf,char buf[]);

static const size_t bufSize=512;

YSRESULT YsSpecialPath::GetProgramFileName(YsString &path)
{
	char buf[bufSize];
	if(true==YsSpecialPath_GetProgramFileName(bufSize,buf))
	{
		path.Set(buf);
		return YSOK;
	}
	return YSERR;
}

YSRESULT YsSpecialPath::GetProgramFileName(YsWString &wpath)
{
	YsString path;
	if(YSOK==GetProgramFileName(path))
	{
		wpath.SetUTF8String(path);
		return YSOK;
	}
	return YSERR;
}

YSRESULT YsSpecialPath::GetProgramBaseDir(YsString &path)
{
	char buf[bufSize];
	if(true==YsSpecialPath_GetProgramBaseDir(bufSize,buf))
	{
		path.Set(buf);
		return YSOK;
	}
	return YSERR;
}

YSRESULT YsSpecialPath::GetProgramBaseDir(YsWString &wpath)
{
	YsString path;
	if(YSOK==GetProgramBaseDir(path))
	{
		wpath.SetUTF8String(path);
		return YSOK;
	}
	return YSERR;
}

YSRESULT YsSpecialPath::GetUserDir(YsString &path)
{
	char buf[bufSize];
	if(true==YsSpecialPath_GetUserDir(bufSize,buf))
	{
		path.Set(buf);
		return YSOK;
	}
	return YSERR;
}

YSRESULT YsSpecialPath::GetUserDir(YsWString &wpath)
{
	YsString path;
	if(YSOK==GetUserDir(path))
	{
		wpath.SetUTF8String(path);
		return YSOK;
	}
	return YSERR;
}

YSRESULT YsSpecialPath::GetUserDocDir(YsString &path)
{
	char buf[bufSize];
	if(true==YsSpecialPath_GetUserDir(bufSize,buf))
	{
		path.MakeFullPathName(buf,"Documents");
		return YSOK;
	}
	return YSERR;
}

YSRESULT YsSpecialPath::GetUserDocDir(YsWString &wpath)
{
	YsString path;
	if(YSOK==GetUserDocDir(path))
	{
		wpath.SetUTF8String(path);
		return YSOK;
	}
	return YSERR;
}

YSRESULT YsSpecialPath::GetUserAppDataDir(YsString &path)
{
	char buf[bufSize];
	if(true==YsSpecialPath_GetUserDir(bufSize,buf))
	{
		path.MakeFullPathName(buf,"Library/Application Support");
		return YSOK;
	}
	return YSERR;
}

YSRESULT YsSpecialPath::GetUserAppDataDir(YsWString &wpath)
{
	YsString path;
	if(YSOK==GetUserAppDataDir(path))
	{
		wpath.SetUTF8String(path);
		return YSOK;
	}
	return YSERR;
}

YSRESULT YsSpecialPath::GetAllUsersDir(YsString &path)
{
	return YSERR;
}

YSRESULT YsSpecialPath::GetAllUsersDir(YsWString &wpath)
{
	return YSERR;
}

