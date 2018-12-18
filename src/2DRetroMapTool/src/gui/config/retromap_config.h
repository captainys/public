#ifndef RETROMAP_CONFIG_IS_INCLUDED
#define RETROMAP_CONFIG_IS_INCLUDED
/* { */

#include <ysclass.h>
#include <retromap_screenshotcache.h>


class RetroMap_Config
{
public:
	enum
	{
		MAX_NUM_PROFILE=8,
		MAX_NUM_SCREENSHOT_DIRECTORY=8
	};
	class Profile
	{
	public:
		YsWString profileName;
		Retromap_ScreenShotDirectoryCache scrnShotFileDir[MAX_NUM_SCREENSHOT_DIRECTORY];

		void Initialize(void);
	};

private:
	Profile profile[MAX_NUM_PROFILE];
	YSSIZE_T currentProfileIndex;

public:
	RetroMap_Config();
	void CleanUp(void);
	YsWString GetScreenShotDirectory(YSSIZE_T idx) const;
	void SetScreenShotDirectory(YSSIZE_T idx,const YsWString &ws);

	YSBOOL GetRecursive(YSSIZE_T idx) const;
	void SetRecursive(YSSIZE_T idx,YSBOOL r);

	const Profile &GetCurrentProfile(void) const;
	const Profile &GetProfile(YSSIZE_T idx) const;

	void UpdateScreenshotCache(void);

	YSRESULT Load(const YsWString &fn);
	YSRESULT Save(const YsWString &fn) const;

	YsWString GetNewScreenshotFileName(void);
	YSBOOL IsScreenshotFolderSelected(void) const;
};


/* } */
#endif
