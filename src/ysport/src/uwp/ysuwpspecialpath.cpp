/* ////////////////////////////////////////////////////////////

File Name: ysuwpspecialpath.cpp
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

using namespace Windows::Storage;
using namespace Windows::ApplicationModel;



YSRESULT YsSpecialPath::GetProgramFileName(YsString &path)
{
	YsWString wPath;
	GetProgramFileName(wPath);
	path.EncodeUTF8<wchar_t>(wPath);
	return YSOK;
}

YSRESULT YsSpecialPath::GetProgramFileName(YsWString &path)
{
	auto package=Windows::ApplicationModel::Package::Current;
	auto instDir=package->InstalledLocation->Path->Data();
	path.MakeFullPathName(instDir,L"dummy_file_name.exe");
	return YSOK;
}

YSRESULT YsSpecialPath::GetProgramBaseDir(YsString &path)
{
	YsWString wPath;
	GetProgramBaseDir(wPath);
	path.EncodeUTF8<wchar_t>(wPath);
	return YSOK;
}

YSRESULT YsSpecialPath::GetProgramBaseDir(YsWString &path)
{
	auto package=Windows::ApplicationModel::Package::Current;
	auto instDir=package->InstalledLocation->Path->Data();
	printf("%s==%ls\n",__FUNCTION__,(const wchar_t *)instDir);
	path.MakeFullPathName(instDir,L"Assets");
	return YSOK;
}

YSRESULT YsSpecialPath::GetUserDir(YsString &path)
{
	Windows::Storage::ApplicationData^ dirInfo=Windows::Storage::ApplicationData::Current;
	auto local=dirInfo->LocalFolder->Path->Data();
	printf("%s==%ls\n",__FUNCTION__,(const wchar_t *)local);
	path.EncodeUTF8<wchar_t>(local);
	return YSOK;
}

YSRESULT YsSpecialPath::GetUserDir(YsWString &path)
{
	Windows::Storage::ApplicationData^ dirInfo=Windows::Storage::ApplicationData::Current;
	auto local=dirInfo->LocalFolder->Path->Data();
	printf("%s==%ls\n",__FUNCTION__,(const wchar_t *)local);
	path.Set(local);
	return YSOK;
}

YSRESULT YsSpecialPath::GetUserDocDir(YsString &path)
{
	// There is no concept of user document dir in UWP.
	return GetUserDir(path);
}

YSRESULT YsSpecialPath::GetUserDocDir(YsWString &path)
{
	// There is no concept of user document dir in UWP.
	return GetUserDir(path);
}

YSRESULT YsSpecialPath::GetUserAppDataDir(YsString &path)
{
	Windows::Storage::ApplicationData^ dirInfo=Windows::Storage::ApplicationData::Current;
	auto local=dirInfo->RoamingFolder->Path->Data();
	printf("%s==%ls\n",__FUNCTION__,(const wchar_t *)local);
	path.EncodeUTF8<wchar_t>(local);
	return YSOK;
}

YSRESULT YsSpecialPath::GetUserAppDataDir(YsWString &wpath)
{
	Windows::Storage::ApplicationData^ dirInfo=Windows::Storage::ApplicationData::Current;
	auto local=dirInfo->RoamingFolder->Path->Data();
	printf("%s==%ls\n",__FUNCTION__,(const wchar_t *)local);
	wpath.Set(local);
	return YSOK;
}

YSRESULT YsSpecialPath::GetAllUsersDir(YsString &path)
{
	YsWString wpath;
	auto res=GetAllUsersDir(wpath);
	path.EncodeUTF8<wchar_t>(wpath);
	return res;
}

YSRESULT YsSpecialPath::GetAllUsersDir(YsWString &path)
{
	Windows::Storage::ApplicationData^ dirInfo=Windows::Storage::ApplicationData::Current;
	if(nullptr!=dirInfo->SharedLocalFolder)
	{
		auto local=dirInfo->SharedLocalFolder->Path->Data();
		printf("%s==%ls\n",__FUNCTION__,(const wchar_t *)local);
		path.Set(local);
	}
	else
	{
		printf("AllUsersDir unavailable.\n");
		path=L"";
	}
	return YSOK;
}

