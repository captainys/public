#include <ysport.h>
#include "retromap_config.h"
#include "../filename.h"


void RetroMap_Config::Profile::Initialize(void)
{
	int ctr=0;

	profileName=L"";
	for(auto &scc : scrnShotFileDir)
	{
		scc.CleanUp();
		scc.SetFolderName(L"");
		switch(ctr)
		{
		case 0:
			scc.SetFolderName(L"~/Dropbox/Screenshots");
			scc.SetRecursive(YSFALSE);
			break;
		case 1:
			scc.SetFolderName(L"~/OneDrive/Pictures/Screenshots");
			scc.SetRecursive(YSFALSE);
			break;
		case 2:
			scc.SetFolderName(L"~/Desktop");
			scc.SetRecursive(YSFALSE);
			break;
		case 3:
			break;
		}
		++ctr;
	}
}

////////////////////////////////////////////////////////////

RetroMap_Config::RetroMap_Config()
{
	CleanUp();
}
void RetroMap_Config::CleanUp(void)
{
	currentProfileIndex=0;
	for(auto &prof : profile)
	{
		prof.Initialize();
	}
}
YsWString RetroMap_Config::GetScreenShotDirectory(YSSIZE_T idx) const
{
	if(0<=idx && idx<MAX_NUM_SCREENSHOT_DIRECTORY)
	{
		return profile[currentProfileIndex].scrnShotFileDir[idx].GetFolderName();
	}
	return YsWString();
}
void RetroMap_Config::SetScreenShotDirectory(YSSIZE_T idx,const YsWString &ws)
{
	if(0<=idx && idx<MAX_NUM_SCREENSHOT_DIRECTORY)
	{
		profile[currentProfileIndex].scrnShotFileDir[idx].MakeCache(ws);
	}
}

YSBOOL RetroMap_Config::GetRecursive(YSSIZE_T idx) const
{
	if(0<=idx && idx<MAX_NUM_SCREENSHOT_DIRECTORY)
	{
		return profile[currentProfileIndex].scrnShotFileDir[idx].GetRecursive();
	}
	return YSFALSE;
}
void RetroMap_Config::SetRecursive(YSSIZE_T idx,YSBOOL r)
{
	if(0<=idx && idx<MAX_NUM_SCREENSHOT_DIRECTORY)
	{
		profile[currentProfileIndex].scrnShotFileDir[idx].SetRecursive(r);
	}
}

const RetroMap_Config::Profile &RetroMap_Config::GetCurrentProfile(void) const
{
	return profile[currentProfileIndex];
}

const RetroMap_Config::Profile &RetroMap_Config::GetProfile(YSSIZE_T idx) const
{
	if(0<=idx && idx<MAX_NUM_SCREENSHOT_DIRECTORY)
	{
		return profile[idx];
	}
	return profile[MAX_NUM_PROFILE-1];
}

void RetroMap_Config::UpdateScreenshotCache(void)
{
	FileAndDirectoryName fileDirName;
	{
		YsFileIO::File ofp(fileDirName.GetCachingIndicatorFile(),"w");
	}

	for(auto &scc : profile[currentProfileIndex].scrnShotFileDir)
	{
		scc.UpdateCache();
	}

	YsFileIO::Remove(fileDirName.GetCachingIndicatorFile());
}

YSRESULT RetroMap_Config::Load(const YsWString &fn)
{
	YsFileIO::File fp(fn,"r");
	if(nullptr!=fp)
	{
		CleanUp();
		int readingProfile=0,nextReadingProfile=0;
		int readingScrnShotDir=0;
		YsString str;
		while(nullptr!=str.Fgets(fp))
		{
			auto args=str.Argv();
			if(0<args.GetN())
			{
				if(0==args[0].STRCMP("CURRENTPROFILE") && 2<=args.GetN())
				{
					currentProfileIndex=atoi(args[1]);
				}
				else if(0==args[0].STRCMP("BEGINPROFILE"))
				{
					readingProfile=nextReadingProfile;
					readingScrnShotDir=0;
					++nextReadingProfile;
				}
				else if(0==args[0].STRCMP("ENDPROFILE"))
				{
				}
				else if(0==args[0].STRCMP("PROFILE") && 2<=args.GetN())
				{
					if(0<=readingProfile && readingProfile<MAX_NUM_PROFILE)
					{
						profile[readingProfile].profileName.SetUTF8String(args[1]);
					}
				}
				else if(0==args[0].STRCMP("SCRNSHOTFOLDER") && 2<=args.GetN())
				{
					if(0<=readingProfile && readingProfile<MAX_NUM_PROFILE)
					{
						if(readingScrnShotDir<MAX_NUM_SCREENSHOT_DIRECTORY)
						{
							YsWString folder;
							folder.SetUTF8String(args[1]);
							profile[readingProfile].scrnShotFileDir[readingScrnShotDir].SetFolderName(folder);
							profile[readingProfile].scrnShotFileDir[readingScrnShotDir].SetRecursive(YSFALSE);
							++readingScrnShotDir;
						}
					}
				}
				else if(0==args[0].STRCMP("RSCRNSHOTFOLDER") && 2<=args.GetN())
				{
					if(0<=readingProfile && readingProfile<MAX_NUM_PROFILE)
					{
						if(readingScrnShotDir<MAX_NUM_SCREENSHOT_DIRECTORY)
						{
							YsWString folder;
							folder.SetUTF8String(args[1]);
							profile[readingProfile].scrnShotFileDir[readingScrnShotDir].SetFolderName(folder);
							profile[readingProfile].scrnShotFileDir[readingScrnShotDir].SetRecursive(YSTRUE);
							++readingScrnShotDir;
						}
					}
				}
			}
		}
		return YSOK;
	}
	return YSERR;
}
YSRESULT RetroMap_Config::Save(const YsWString &fn) const
{
	YsFileIO::File fp(fn,"w");
	if(nullptr!=fp)
	{
		auto outStream=fp.OutStream();
		outStream.Printf("CURRENTPROFILE %d\n",(int)currentProfileIndex);

		for(auto &prof : profile)
		{
			outStream.Printf("BEGINPROFILE\n");
			outStream.Printf("PROFILE \"%s\"\n",prof.profileName.GetUTF8String().Txt());
			for(auto &scc : prof.scrnShotFileDir)
			{
				auto folder=scc.GetFolderName();
				if(0<folder.Strlen())
				{
					if(YSTRUE!=scc.GetRecursive())
					{
						outStream.Printf("SCRNSHOTFOLDER \"%s\"\n",folder.GetUTF8String().Txt());
					}
					else
					{
						outStream.Printf("RSCRNSHOTFOLDER \"%s\"\n",folder.GetUTF8String().Txt());
					}
				}
			}
			outStream.Printf("ENDPROFILE\n");
		}
		return YSOK;
	}
	return YSERR;
}



YsWString RetroMap_Config::GetNewScreenshotFileName(void)
{
	auto &prof=profile[currentProfileIndex];
	for(auto &scc : prof.scrnShotFileDir)
	{
		auto fn=scc.FetchOneFile();
		if(0<fn.Strlen())
		{
			auto pth=scc.GetExpandedFolderName();
			YsWString ful;
			ful.MakeFullPathName(pth,fn);
			return ful;
		}
	}
	return YsWString();
}

YSBOOL RetroMap_Config::IsScreenshotFolderSelected(void) const
{
	auto &prof=profile[currentProfileIndex];
	for(auto &scc : prof.scrnShotFileDir)
	{
		if(0<scc.GetFolderName().Strlen())
		{
			return YSTRUE;
		}
	}
	return YSFALSE;
}

