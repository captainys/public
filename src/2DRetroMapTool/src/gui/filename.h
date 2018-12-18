#ifndef FILENAME_IS_INCLUDED
#define FILENAME_IS_INCLUDED
/* { */

#include <ysclass.h>
#include <ysport.h>

class FileAndDirectoryName
{
private:
	mutable YsWString ysflightComDir;
	mutable YsWString appDataDir;
	mutable YsWString altRootDir;
	mutable YsWString optionDir,optionFile;

	FileAndDirectoryName(const FileAndDirectoryName &);
	const FileAndDirectoryName &operator=(const FileAndDirectoryName &);

public:
	FileAndDirectoryName(){};

	const wchar_t *GetYsflightComDir(void) const;
	const wchar_t *GetAppDataDir(void) const;
	const wchar_t *GetAlternativeRootDir(void) const;
	const wchar_t *GetOptionDir(void) const;
	const wchar_t *GetOptionFileName(void) const;
	const YsWString GetRecentFileListFileName(void) const;
	const YsWString GetLastWindowPositionFileName(void) const;

	const YsWString GetCachingIndicatorFile(void) const;

	static void MakeDirectoryForFile(const wchar_t ful[]);
};

/* } */
#endif
