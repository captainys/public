/* ////////////////////////////////////////////////////////////

File Name: ysspecialpath.h
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

#ifndef YSSPECIALPATH_IS_INCLUDED
#define YSSPECIALPATH_IS_INCLUDED
/* { */

#include <ysclass.h>


// Examples:
// [UserDir]
//   Windows C:\Users\soji
//   Linux   
//   MacOSX  
// [UserDocDir]
//   Windows C:\Users\soji\My Documents
//   Linux   ~/Documents
//   MacOSX  
// [UserAppDataDir]
//   Windows C:\Users\soji\Application Data
//   Linux   ~/ApplicationData (I made up)
//   MacOSX  ~/Library/Application Support
// [AllUsersDir]
//   Windows C:\Users\All Users\Application Data
//   Linux   No such thing (returns YSERR)
//   MacOSX  Does it exist?

// GetProgramBaseDir will return the directory where the executable files are stored in Windows and Linux,
// the bundle directory in MacOSX.

/*! This class returns special directory.
 */
class YsSpecialPath
{
	enum
	{
		MaxPath=512
	};

public:
	/*! Returns the full-path file name of the process.
	 */
	static YSRESULT GetProgramFileName(YsString &path);
	static YSRESULT GetProgramFileName(YsWString &wpath);
	static YSRESULT GetProgramFileNameUTF8(YsString &path);
	static YsString GetProgramFileNameUTF8(void);
	static YsWString GetProgramFileNameW(void);

	/*! Returns where the program stores the data files.
	    - In Win32 and Linux, it is the directory where the .exe file is stored.
	    - In macOS and iOS, it is the Resources directory of the bundle.
	    - In Universal Windows Platform, it is the Asset directory of the AppX package.
	 */
	static YSRESULT GetProgramBaseDir(YsString &path);
	static YSRESULT GetProgramBaseDir(YsWString &wpath);
	static YSRESULT GetProgramBaseDirUTF8(YsString &path);
	static YsString GetProgramBaseDirUTF8(void);
	static YsWString GetProgramBaseDirW(void);

	/*! Returns the current user's home directory.
	 */
	static YSRESULT GetUserDir(YsString &path);
	static YSRESULT GetUserDir(YsWString &wpath);
	static YSRESULT GetUserDirUTF8(YsString &path);
	static YsString GetUserDirUTF8(void);
	static YsWString GetUserDirW(void);

	/*! Returns the current user's document directory.
	 */
	static YSRESULT GetUserDocDir(YsString &path);
	static YSRESULT GetUserDocDir(YsWString &wpath);
	static YSRESULT GetUserDocDirUTF8(YsString &path);
	static YsString GetUserDocDirUTF8(void);
	static YsWString GetUserDocDirW(void);

	/*! Returns the application-data directory.
	 */
	static YSRESULT GetUserAppDataDir(YsString &path);
	static YSRESULT GetUserAppDataDir(YsWString &wpath);
	static YSRESULT GetUserAppDataDirUTF8(YsString &path);
	static YsString GetUserAppDataDirUTF8(void);
	static YsWString GetUserAppDataDirW(void);

	/*! Returns all-users directory.

	    There doesn't seem to be such directory in Linux and macOS.
	 */
	static YSRESULT GetAllUsersDir(YsString &path);
	static YSRESULT GetAllUsersDir(YsWString &wpath);
	static YSRESULT GetAllUsersDirUTF8(YsString &path);
	static YsString GetAllUsersDirUTF8(void);
	static YsWString GetAllUsersDirW(void);

	/*! Replace ~ with the user dir.  It only supports ~/ or ~\\.  Does not give other users' paths.
	    If the file name does not start with '~', it returns the input file name as is.
	*/
	static YsString ExpandUser(const YsString &fName);
	static YsWString ExpandUser(const YsWString &fName);
};


/* } */
#endif
