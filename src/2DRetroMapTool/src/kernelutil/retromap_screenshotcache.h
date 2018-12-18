#ifndef RETROMAP_SCREENSHOTCACHE_IS_INCLUDED
#define RETROMAP_SCREENSHOTCACHE_IS_INCLUDED
/* { */

#include <ysclass.h>

class Retromap_ScreenShotDirectoryCache
{
private:
	YsWString folder;
	YSBOOL recursive;
	YsAVLTree <YsWString,int,YsStringComparer<YsWString> > existingFile;

	Retromap_ScreenShotDirectoryCache(const Retromap_ScreenShotDirectoryCache &);
	Retromap_ScreenShotDirectoryCache &operator=(const Retromap_ScreenShotDirectoryCache &);
public:
	Retromap_ScreenShotDirectoryCache();

	/*! Cleans up cached files.  It does not clear directory location.
	*/
	void CleanUp(void);

	/*! Get folder name. */
	YsWString GetFolderName(void) const;

	/*! Get expanded folder name. */
	YsWString GetExpandedFolderName(void) const;

	/*! Set folder location, but do not update until UpdateCache() is called. */
	void SetFolderName(const YsWString &folder);

	/*! Set folder location and update the cache. */
	void MakeCache(const YsWString &folder);

	/*! Update cache with the folder set by MakeCache(). */
	void UpdateCache(void);
private:
	void UpdateCache(const YsWString &subFolder);

public:
	/*! Returns a file name of a file that is not in the cache.
	    The cache is also updated so that the returned file name is included.
	*/
	YsWString FetchOneFile(void);
private:
	YsWString FetchOneFile(const YsWString &subFolder);

public:
	/*! Set recursive flag.  If this flag is on, this class will look for all the files in the sub-folders recursively in the specified folder. */
	void SetRecursive(YSBOOL r);

	/*! Returns the recursive flag. */
	YSBOOL GetRecursive(void) const;
};

/* } */
#endif
