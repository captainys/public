/* ////////////////////////////////////////////////////////////

File Name: filename.h
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

#ifndef FILENAME_IS_INCLUDED
#define FILENAME_IS_INCLUDED
/* { */

#include <ysclass.h>

class PolyCreFileName
{
private:
	YsWString appDataDirBaseName;

	mutable YsWString ysflightComDir;
	mutable YsWString appDataDir;
	mutable YsWString altRootDir;
	mutable YsWString optionDir,optionFile;

private:
	PolyCreFileName();
	~PolyCreFileName();
	const PolyCreFileName &operator=(const PolyCreFileName &);
public:
	static PolyCreFileName *Create(void);
	static void Delete(PolyCreFileName *ptr);

public:
	const wchar_t *GetYsflightComDir(void) const;
	const wchar_t *GetAppDataDir(void) const;
	const wchar_t *GetAlternativeRootDir(void) const;
	const wchar_t *GetOptionDir(void) const;
	const wchar_t *GetOptionFileName(void) const;
	const YsWString GetRecentFileListFileName(void) const;
	const YsWString GetLastWindowPositionFileName(void) const;
	const YsWString GetViewConfigFileName(void) const;

	void SetAppDataDirBaseName(YsWString baseName);

	static void MakeDirectoryForFile(const wchar_t ful[]);
};

/* } */
#endif
