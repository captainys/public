/* ////////////////////////////////////////////////////////////

File Name: ysfileinfo.h
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

#ifndef YSFILEINFO_IS_INCLUDED
#define YSFILEINFO_IS_INCLUDED
/* { */

#include <ysclass.h>

/*! A class that retrieves files in a directory.
    */
class YsFileList
{
private:
	class FileInfo
	{
	public:
		YSBOOL isDirectory;
		YsWString fn;
		YsString sysFn;  // System-recognized string.  Eg. In Japanese Windows, needs to be in Shift-JIS typically.
		unsigned long int sizeHigh,sizeLow;
	};

	YsWString path,uPrefix,uPostfix;
	YsArray <FileInfo> fileInfo;

	YSBOOL MatchPrefix(const YsWString &tst,const YsWString &uPrefix);
	YSBOOL MatchPostfix(const YsWString &tst,const YsWString &uPostfix);

public:
	/*!
	   This function needs to be private.  However, for MacOSX, it needs to be called from a C function,
	   which is a callback from an Objective-C function.  Therefore, I left with no other option but keeping
	   this function public.  Don't use it from elsewhere. */
	YSRESULT TestAndAddFileInfo(const wchar_t fn[],YSBOOL isDir,unsigned long szHigh,unsigned long szLow);

public:
	/*! Default constructor */
	YsFileList();

	/*! Default destructor */
	~YsFileList();

	/*! Cleans up the object, same as CleanUp() */
	void Initialize(void);

	/*! Cleans up the object, same as Initialize() */
	void CleanUp(void);

	/*! Reads files in the given path that matches given prefix and postfix.
	    Prefix and postfix are case insensitive.
	    Prefix and postfix can be NULL when unnecessary.
	    Encoding of path, prefix, and postfix must be system-recognized encoding, that is compatible with fopen etc.
	   */
	YSRESULT FindFileList(const char path[],const char prefix[],const char postfix[]);

	/*! Reads files in the given path that matches given prefix and postfix. 
	    Prefix and postfix are case insensitive.
	    Prefix and postfix can be NULL when unnecessary.
	    */
	YSRESULT FindFileList(const wchar_t path[],const wchar_t prefix[],const wchar_t postfix[]);

	/*! Returns the number of files found by FindFileList function. 
	    */
	YSSIZE_T GetN(void) const;

	/*! Returns YSTRUE if fileNum-th (zero based) file is a directory.
	    */
	YSBOOL IsDirectory(YSSIZE_T fileNum) const;

	/*! Returns a pointer to a C-String, which is in system encoding. 
	    */
	const char *GetSystemEncodingName(YSSIZE_T fileNum) const;

	/*! Returns a full-path filename in C-String, which is in system encoding. 
	    */
	const char *GetSystemEncodingNameWithPath(YsString &nameWithPath,YSSIZE_T fileNum) const;

	/*! Returns a pointer to a Unicode file name string */
	const wchar_t *GetFileName(YSSIZE_T fileNum) const;

	/*! Returns a full-path Unicode file name string */
	const wchar_t *GetFileName(YsWString &nameWithPath,YSSIZE_T fileNum) const;

	/*! Returns a full-path Unicode file name string. */
	YsWString GetFileNameW(YSSIZE_T fileNum) const;

	/*! Returns a Unicode base name part of the file name. */
	YsWString GetBaseNameW(YSSIZE_T fileNum) const;

	/*! Returns the file size. */
	YSSIZE_T GetSize(YSSIZE_T fileNum) const;

	/*! Returns lower bits of the file size of fileNum'th file found by FindFileList function. */
	unsigned long int GetSizeLow(YSSIZE_T fileNum) const;

	/*! Returns higher bits of the file size of fileNum'th file found by FindFileList function. */
	unsigned long int GetSizeHigh(YSSIZE_T fileNum) const;
};

/* } */
#endif
