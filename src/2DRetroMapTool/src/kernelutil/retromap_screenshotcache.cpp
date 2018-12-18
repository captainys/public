#include "retromap_screenshotcache.h"
#include <ysport.h>

Retromap_ScreenShotDirectoryCache::Retromap_ScreenShotDirectoryCache()
{
	recursive=YSTRUE;
}
void Retromap_ScreenShotDirectoryCache::CleanUp(void)
{
	existingFile.CleanUp();
	recursive=YSTRUE;
}

YsWString Retromap_ScreenShotDirectoryCache::GetFolderName(void) const
{
	return folder;
}

YsWString Retromap_ScreenShotDirectoryCache::GetExpandedFolderName(void) const
{
	auto folder=this->folder;
	if('~'==folder[0])
	{
		auto right=folder.Subset(1,folder.Strlen());
		folder.MakeFullPathName(YsSpecialPath::GetUserDirW(),right);
		// printf("~ expanded: %ls\n",folder.data());
	}
	return folder;
}

void Retromap_ScreenShotDirectoryCache::SetFolderName(const YsWString &folder)
{
	this->folder=folder;
}

void Retromap_ScreenShotDirectoryCache::MakeCache(const YsWString &folder)
{
	existingFile.CleanUp();
	SetFolderName(folder);
	UpdateCache();
}
void Retromap_ScreenShotDirectoryCache::UpdateCache(void)
{
	if(0<folder.Strlen())
	{
		UpdateCache(L"");
	}
}
void Retromap_ScreenShotDirectoryCache::UpdateCache(const YsWString &subFolder)
{
	YsFileList fileList;
	auto folder=GetExpandedFolderName();

	if(0==subFolder.size())
	{
		fileList.FindFileList(folder,nullptr,nullptr);
	}
	else
	{
		YsWString fulDir;
		fulDir.MakeFullPathName(folder,subFolder);
		fileList.FindFileList(fulDir,nullptr,nullptr);
	}
	for(YSSIZE_T idx=0; idx<fileList.GetN(); ++idx)
	{
		if(YSTRUE!=fileList.IsDirectory(idx))
		{
			auto baseName=fileList.GetBaseNameW(idx);
			YsWString entry;
			if(0==subFolder.size())
			{
				entry=baseName;
			}
			else
			{
				entry.MakeFullPathName(subFolder,baseName);
			}
			if(existingFile.Null()==existingFile.FindNode(entry))
			{
				existingFile.Insert(entry,0);
			}
		}
		else if(YSTRUE==recursive)
		{
			auto baseName=fileList.GetBaseNameW(idx);
			if(0!=baseName.Strcmp(L".") && 0!=baseName.Strcmp(L".."))
			{
				YsWString subSubDir;
				subSubDir.MakeFullPathName(subFolder,baseName);
				UpdateCache(subSubDir);
			}
		}
	}
}

YsWString Retromap_ScreenShotDirectoryCache::FetchOneFile(void)
{
	if(0<folder.Strlen())
	{
		return FetchOneFile(L"");
	}
	return YsWString();
}

YsWString Retromap_ScreenShotDirectoryCache::FetchOneFile(const YsWString &subFolder)
{
	YsFileList fileList;
	auto folder=GetExpandedFolderName();

	if(0==subFolder.size())
	{
		fileList.FindFileList(folder,nullptr,nullptr);
	}
	else
	{
		YsWString fulDir;
		fulDir.MakeFullPathName(folder,subFolder);
		fileList.FindFileList(fulDir,nullptr,nullptr);
	}

	for(YSSIZE_T idx=0; idx<fileList.GetN(); ++idx)
	{
		if(YSTRUE!=fileList.IsDirectory(idx))
		{
			auto baseName=fileList.GetBaseNameW(idx);
			YsWString entry;
			if(0==subFolder.size())
			{
				entry=baseName;
			}
			else
			{
				entry.MakeFullPathName(subFolder,baseName);
			}
			if(existingFile.Null()==existingFile.FindNode(entry))
			{
				existingFile.Insert(entry,0);
				return entry;
			}
		}
		else if(YSTRUE==recursive)
		{
			auto baseName=fileList.GetBaseNameW(idx);
			if(0!=baseName.Strcmp(L".") && 0!=baseName.Strcmp(L".."))
			{
				YsWString subSubDir;
				subSubDir.MakeFullPathName(subFolder,baseName);
				auto found=FetchOneFile(subSubDir);
				if(0<found.size())
				{
					return found;
				}
			}
		}
	}
	return YsWString();
}

void Retromap_ScreenShotDirectoryCache::SetRecursive(YSBOOL r)
{
	recursive=r;
}

YSBOOL Retromap_ScreenShotDirectoryCache::GetRecursive(void) const
{
	return recursive;
}
