/* ////////////////////////////////////////////////////////////

File Name: ysspecialpath.cpp
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

#include "ysspecialpath.h"

YSRESULT YsSpecialPath::GetProgramFileNameUTF8(YsString &path)
{
	YsWString wstr;
	if(YSOK==GetProgramFileName(wstr))
	{
		path.EncodeUTF8 <wchar_t> (wstr);
		return YSOK;
	}
	return YSERR;
}

YSRESULT YsSpecialPath::GetProgramBaseDirUTF8(YsString &path)
{
	YsWString wstr;
	if(YSOK==GetProgramBaseDir(wstr))
	{
		path.EncodeUTF8 <wchar_t> (wstr);
		return YSOK;
	}
	return YSERR;
}

YSRESULT YsSpecialPath::GetUserDirUTF8(YsString &path)
{
	YsWString wstr;
	if(YSOK==GetUserDir(wstr))
	{
		path.EncodeUTF8 <wchar_t> (wstr);
		return YSOK;
	}
	return YSERR;
}

YSRESULT YsSpecialPath::GetUserDocDirUTF8(YsString &path)
{
	YsWString wstr;
	if(YSOK==GetUserDocDir(wstr))
	{
		path.EncodeUTF8 <wchar_t> (wstr);
		return YSOK;
	}
	return YSERR;
}

YSRESULT YsSpecialPath::GetUserAppDataDirUTF8(YsString &path)
{
	YsWString wstr;
	if(YSOK==GetUserAppDataDir(wstr))
	{
		path.EncodeUTF8 <wchar_t> (wstr);
		return YSOK;
	}
	return YSERR;
}

YSRESULT YsSpecialPath::GetAllUsersDirUTF8(YsString &path)
{
	YsWString wstr;
	if(YSOK==GetAllUsersDir(wstr))
	{
		path.EncodeUTF8 <wchar_t> (wstr);
		return YSOK;
	}
	return YSERR;
}

/* static */ YsString YsSpecialPath::GetProgramFileNameUTF8(void)
{
	YsString str;
	GetProgramFileNameUTF8(str);
	return str;
}
/* static */ YsWString YsSpecialPath::GetProgramFileNameW(void)
{
	YsWString str;
	GetProgramFileName(str);
	return str;
}
/* static */ YsString YsSpecialPath::GetProgramBaseDirUTF8(void)
{
	YsString str;
	GetProgramBaseDirUTF8(str);
	return str;
}
/* static */ YsWString YsSpecialPath::GetProgramBaseDirW(void)
{
	YsWString str;
	GetProgramBaseDir(str);
	return str;
}
/* static */ YsString YsSpecialPath::GetUserDirUTF8(void)
{
	YsString str;
	GetUserDirUTF8(str);
	return str;
}
/* static */ YsWString YsSpecialPath::GetUserDirW(void)
{
	YsWString str;
	GetUserDir(str);
	return str;
}
/* static */ YsString YsSpecialPath::GetUserDocDirUTF8(void)
{
	YsString str;
	GetUserDocDirUTF8(str);
	return str;
}
/* static */ YsWString YsSpecialPath::GetUserDocDirW(void)
{
	YsWString str;
	GetUserDocDir(str);
	return str;
}
/* static */ YsString YsSpecialPath::GetUserAppDataDirUTF8(void)
{
	YsString str;
	GetUserAppDataDirUTF8(str);
	return str;
}
/* static */ YsWString YsSpecialPath::GetUserAppDataDirW(void)
{
	YsWString str;
	GetUserAppDataDir(str);
	return str;
}
/* static */ YsString YsSpecialPath::GetAllUsersDirUTF8(void)
{
	YsString str;
	GetAllUsersDirUTF8(str);
	return str;
}
/* static */ YsWString YsSpecialPath::GetAllUsersDirW(void)
{
	YsWString str;
	GetAllUsersDir(str);
	return str;
}

/* static */ YsString YsSpecialPath::ExpandUser(const YsString &fName)
{
	if('~'==fName[0])
	{
		auto base=fName.data()+1;
		if('/'==base[0] || '\\'==base[0])
		{
			++base;
		}
		YsString ful;
		ful.MakeFullPathName(GetUserDirUTF8(),base);
		return ful;
	}
	return fName;
}

/* static */ YsWString YsSpecialPath::ExpandUser(const YsWString &fName)
{
	if('~'==fName[0])
	{
		auto base=fName.data()+1;
		if('/'==base[0] || '\\'==base[0])
		{
			++base;
		}
		YsWString ful;
		ful.MakeFullPathName(GetUserDirW(),base);
		return ful;
	}
	return fName;
}
