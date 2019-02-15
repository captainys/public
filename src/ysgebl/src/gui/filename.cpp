/* ////////////////////////////////////////////////////////////

File Name: filename.cpp
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

#include "filename.h"
#include <ysport.h>

#include <stdio.h>



PolyCreFileName::PolyCreFileName()
{
	appDataDirBaseName=L"PolygonCrest";
}

PolyCreFileName::~PolyCreFileName()
{
}

/*static*/ PolyCreFileName *PolyCreFileName::Create(void)
{
	return new PolyCreFileName;
}

/*static*/ void PolyCreFileName::Delete(PolyCreFileName *ptr)
{
	delete ptr;
}

const wchar_t *PolyCreFileName::GetYsflightComDir(void) const
{
	if(0==ysflightComDir.Strlen())
	{
		YsWString appDataRoot;
		YsSpecialPath::GetUserAppDataDir(appDataRoot);
		ysflightComDir.MakeFullPathName(appDataRoot,L"YSFLIGHT.COM");
	}
	return ysflightComDir;
}

const wchar_t *PolyCreFileName::GetAppDataDir(void) const
{
	if(0==appDataDir.Strlen())
	{
		const wchar_t *ysflightComDir=GetYsflightComDir();
		appDataDir.MakeFullPathName(ysflightComDir,appDataDirBaseName);
	}
	return appDataDir;
}

const wchar_t *PolyCreFileName::GetAlternativeRootDir(void) const
{
	if(0==altRootDir.Strlen())
	{
		return NULL;
	}
	return altRootDir;
}

const wchar_t *PolyCreFileName::GetOptionDir(void) const
{
	if(0==optionDir.Strlen())
	{
		const wchar_t *rootDir=GetAlternativeRootDir();
		if(NULL==rootDir)
		{
			altRootDir=GetAppDataDir();
		}
		optionDir.MakeFullPathName(altRootDir,L"config");
	}
	return optionDir;
}

const wchar_t *PolyCreFileName::GetOptionFileName(void) const
{
	if(0==optionFile.Strlen())
	{
		optionFile.MakeFullPathName(GetOptionDir(),L"config.txt");
	}
	printf("Option File Name: %ls\n",optionFile.Txt());
	return optionFile;
}

const YsWString PolyCreFileName::GetRecentFileListFileName(void) const
{
	const wchar_t *optDir=GetOptionDir();
	YsWString wStr;
	wStr.MakeFullPathName(optDir,L"recent.txt");
	printf("Recently-Used File Names are stored in: %ls\n",wStr.Txt());
	return wStr;
}

const YsWString PolyCreFileName::GetLastWindowPositionFileName(void) const
{
	const wchar_t *optDir=GetOptionDir();
	YsWString wStr;
	wStr.MakeFullPathName(optDir,L"windowpos.txt");
	printf("Last Window Position is stored in: %ls\n",wStr.Txt());
	return wStr;
}

const YsWString PolyCreFileName::GetViewConfigFileName(void) const
{
	const wchar_t *optDir=GetOptionDir();
	YsWString wStr;
	wStr.MakeFullPathName(optDir,L"viewoption.txt");
	printf("Last Window Position is stored in: %ls\n",wStr.Txt());
	return wStr;
}

void PolyCreFileName::SetAppDataDirBaseName(YsWString baseName)
{
	appDataDirBaseName=baseName;
}

/* static */ void PolyCreFileName::MakeDirectoryForFile(const wchar_t wfn[])
{
	YsWString ful(wfn),pth,fil;
	ful.SeparatePathFile(pth,fil);
	ful=pth;
	YsArray <YsWString> backTrack;
	for(;;)
	{
		if(ful.LastChar()=='\\' || ful.LastChar()=='/')
		{
			ful.BackSpace();
		}

		printf("%ls\n",ful.Txt());
		backTrack.Append(ful);

		YsWString pth,fil;
		ful.SeparatePathFile(pth,fil);

		if(0<pth.Strlen() && 0<fil.Strlen())
		{
			ful=pth;
		}
		else
		{
			break;
		}
	}

	for(YSSIZE_T idx=backTrack.GetN()-1; 0<=idx; --idx)
	{
		printf("MkDir %ls\n",backTrack[idx].Txt());
		YsFileIO::MkDir(backTrack[idx]);
	}
}

