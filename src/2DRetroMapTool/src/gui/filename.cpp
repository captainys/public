#include <ysclass.h>
#include <ysport.h>
#include <yscompilerwarning.h>
#include <ysgl.h>

#include "fsgui3dapp.h"
#include "textresource.h"



const wchar_t *FileAndDirectoryName::GetYsflightComDir(void) const
{
	if(0==ysflightComDir.Strlen())
	{
		YsWString appDataRoot;
		YsSpecialPath::GetUserAppDataDir(appDataRoot);
		ysflightComDir.MakeFullPathName(appDataRoot,L"YSFLIGHT.COM");
	}
	return ysflightComDir;
}

const wchar_t *FileAndDirectoryName::GetAppDataDir(void) const
{
	if(0==appDataDir.Strlen())
	{
		const wchar_t *ysflightComDir=GetYsflightComDir();
		appDataDir.MakeFullPathName(ysflightComDir,L"YSRetro2DMap");
	}
	return appDataDir;
}

const wchar_t *FileAndDirectoryName::GetAlternativeRootDir(void) const
{
	if(0==altRootDir.Strlen())
	{
		return NULL;
	}
	return altRootDir;
}

const wchar_t *FileAndDirectoryName::GetOptionDir(void) const
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

const wchar_t *FileAndDirectoryName::GetOptionFileName(void) const
{
	if(0==optionFile.Strlen())
	{
		optionFile.MakeFullPathName(GetOptionDir(),L"config.txt");
	}
	printf("Option File Name: %ls\n",optionFile.Txt());
	return optionFile;
}

const YsWString FileAndDirectoryName::GetRecentFileListFileName(void) const
{
	const wchar_t *optDir=GetOptionDir();
	YsWString wStr;
	wStr.MakeFullPathName(optDir,L"recent.txt");
	printf("Recently-Used File Names are stored in: %ls\n",wStr.Txt());
	return wStr;
}

const YsWString FileAndDirectoryName::GetLastWindowPositionFileName(void) const
{
	const wchar_t *optDir=GetOptionDir();
	YsWString wStr;
	wStr.MakeFullPathName(optDir,L"windowpos.txt");
	printf("Last Window Position is stored in: %ls\n",wStr.Txt());
	return wStr;
}

/* static */ void FileAndDirectoryName::MakeDirectoryForFile(const wchar_t wfn[])
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

const YsWString FileAndDirectoryName::GetCachingIndicatorFile(void) const
{
	const wchar_t *optDir=GetOptionDir();
	YsWString wStr;
	wStr.MakeFullPathName(optDir,L"caching_screenshots.txt");
	printf("Last Window Position is stored in: %ls\n",wStr.Txt());
	return wStr;
}

