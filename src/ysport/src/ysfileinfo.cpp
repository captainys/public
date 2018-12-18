/* ////////////////////////////////////////////////////////////

File Name: ysfileinfo.cpp
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

#include "ysfileinfo.h"
#include "ysencoding.h"

YSBOOL YsFileList::MatchPrefix(const YsWString &tst,const YsWString &uPrefix)
{
	const wchar_t asterisk[]={L"*"}; // Workaround for GCC MacOSX specific 64-bit bug
	const wchar_t question[]={L"?"}; // Workaround for GCC MacOSX specific 64-bit bug
	if(0==uPrefix.Strlen() ||
	   0==YsWString::Strcmp(uPrefix,asterisk) ||
	   0==YsWString::Strcmp(uPrefix,question) ||
	   0==YsWString::Strncmp(tst,uPrefix,uPrefix.Strlen()))
	{
		return YSTRUE;
	}
	else
	{
		return YSFALSE;
	}
}

YSBOOL YsFileList::MatchPostfix(const YsWString &tst,const YsWString &uPostfix)
{
	const wchar_t asterisk[]={L"*"}; // Workaround for GCC MacOSX specific 64-bit bug
	const wchar_t question[]={L"?"}; // Workaround for GCC MacOSX specific 64-bit bug
	if(0==uPostfix.Strlen() ||
	   0==YsWString::Strcmp(uPostfix,asterisk) ||
	   0==YsWString::Strcmp(uPostfix,question) ||
	   (tst.Strlen()>=uPostfix.Strlen() && 0==YsWString::Strcmp(tst.GetArray()+tst.Strlen()-uPostfix.Strlen(),uPostfix)))
	{
		return YSTRUE;
	}
	else
	{
		return YSFALSE;
	}
}

YsFileList::YsFileList()
{
}

YsFileList::~YsFileList()
{
}

void YsFileList::Initialize(void)
{
	fileInfo.Clear();
}

void YsFileList::CleanUp(void)
{
	Initialize();
}

YSRESULT YsFileList::TestAndAddFileInfo(const wchar_t fn[],YSBOOL isDir,unsigned long szHigh,unsigned long szLow)
{
	YsWString tst(fn);

	tst.Uncapitalize();
	if(YSTRUE==isDir ||
	   (YSTRUE==MatchPrefix(tst,uPrefix) &&
	    YSTRUE==MatchPostfix(tst,uPostfix)))
	{
		fileInfo.Increment();
		fileInfo.GetEnd().fn.Set(fn);
		YsUnicodeToSystemEncoding(fileInfo.GetEnd().sysFn,fn);
		fileInfo.GetEnd().isDirectory=isDir;
		fileInfo.GetEnd().sizeHigh=szHigh;
		fileInfo.GetEnd().sizeLow=szLow;
	}

	return YSOK;
}

YSSIZE_T YsFileList::GetN(void) const
{
	return fileInfo.GetN();
}

YSBOOL YsFileList::IsDirectory(YSSIZE_T fileNum) const
{
	if(YSTRUE==fileInfo.IsInRange(fileNum))
	{
		return fileInfo[fileNum].isDirectory;
	}
	return YSFALSE;
}

const char *YsFileList::GetSystemEncodingName(YSSIZE_T fileNum) const
{
	if(YSTRUE==fileInfo.IsInRange(fileNum))
	{
		return fileInfo[fileNum].sysFn;
	}
	else
	{
		return NULL;
	}
}

const char *YsFileList::GetSystemEncodingNameWithPath(YsString &nameWithPath,YSSIZE_T fileNum) const
{
	if(YSTRUE==fileInfo.IsInRange(fileNum))
	{
		YsWString ful;
		ful.MakeFullPathName(path,fileInfo[fileNum].fn);
		YsUnicodeToSystemEncoding(nameWithPath,ful);
		return nameWithPath;
	}
	else
	{
		nameWithPath.Set("");
		return NULL;
	}
}

const wchar_t *YsFileList::GetFileName(YSSIZE_T fileNum) const
{
	if(YSTRUE==fileInfo.IsInRange(fileNum))
	{
		return fileInfo[fileNum].fn;
	}
	else
	{
		return NULL;
	}
}

const wchar_t *YsFileList::GetFileName(YsWString &nameWithPath,YSSIZE_T fileNum) const
{
	if(YSTRUE==fileInfo.IsInRange(fileNum))
	{
		nameWithPath.MakeFullPathName(path,fileInfo[fileNum].fn);
		return nameWithPath;
	}
	else
	{
		nameWithPath.Set(NULL);
		return NULL;
	}
}

YsWString YsFileList::GetFileNameW(YSSIZE_T fileNum) const
{
	if(YSTRUE==fileInfo.IsInRange(fileNum))
	{
		YsWString ful;
		ful.MakeFullPathName(path,fileInfo[fileNum].fn);
		return ful;
	}
	else
	{
		return YsWString();
	}
}

YsWString YsFileList::GetBaseNameW(YSSIZE_T fileNum) const
{
	if(YSTRUE==fileInfo.IsInRange(fileNum))
	{
		return fileInfo[fileNum].fn;
	}
	else
	{
		return YsWString();
	}
}

unsigned long int YsFileList::GetSizeLow(YSSIZE_T fileNum) const
{
	if(YSTRUE==fileInfo.IsInRange(fileNum))
	{
		return fileInfo[fileNum].sizeLow;
	}
	return 0;
}

unsigned long int YsFileList::GetSizeHigh(YSSIZE_T fileNum) const
{
	if(YSTRUE==fileInfo.IsInRange(fileNum))
	{
		return fileInfo[fileNum].sizeHigh;
	}
	return 0;
}

YSSIZE_T YsFileList::GetSize(YSSIZE_T fileNum) const
{
	YSSIZE_T highBit=GetSizeHigh(fileNum);
	YSSIZE_T lowBit=GetSizeLow(fileNum);
	return highBit*0x100000000+lowBit;
}
