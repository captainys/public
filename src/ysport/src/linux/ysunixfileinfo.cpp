/* ////////////////////////////////////////////////////////////

File Name: ysunixfileinfo.cpp
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

#include <ysclass.h>
#include "../ysfileinfo.h"
#include "../ysencoding.h"

#include <stdio.h>
#include <stdlib.h>

#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>


YSRESULT YsFileList::FindFileList(const wchar_t path[],const wchar_t prefix[],const wchar_t postfix[])
{
	YsString cPath,cPrefix,cPostfix;
	YsUnicodeToSystemEncoding(cPath,path);
	YsUnicodeToSystemEncoding(cPrefix,prefix);
	YsUnicodeToSystemEncoding(cPostfix,postfix);
	return FindFileList(cPath,cPrefix,cPostfix);
}

YSRESULT YsFileList::FindFileList(const char path[],const char prefix[],const char postfix[])
{
	YsString pathForOpen(path);
	if(1<pathForOpen.Strlen() && 
       (pathForOpen.LastChar()=='/' || pathForOpen.LastChar()=='\\'))
	{
		pathForOpen.BackSpace();
	}


	DIR *dp;
	if(NULL==(dp=opendir(pathForOpen)))
	{
		return YSERR;
	}


	if(NULL!=prefix)
	{
		YsSystemEncodingToUnicode(uPrefix,prefix);
	}
	else
	{
		uPrefix.Set(L"");
	}
	if(NULL!=postfix)
	{
		YsSystemEncodingToUnicode(uPostfix,postfix);
	}
	else
	{
		uPostfix.Set(L"");
	}
	uPrefix.Uncapitalize();
	uPostfix.Uncapitalize();

	CleanUp();
	YsSystemEncodingToUnicode(this->path,path);
	
	struct dirent *de;
	while(NULL!=(de=readdir(dp)))
	{
		if(0!=de->d_ino && 0!=strcmp(de->d_name,".") && 0!=strcmp(de->d_name,".."))
		{
			YsString ful;
			ful.MakeFullPathName(path,de->d_name);

			struct stat st;
			stat(ful,&st);

			YSBOOL isDir=YSFALSE;
			unsigned long sizeHigh=0;
			unsigned long sizeLow=0;

			if(S_IFDIR==(st.st_mode&S_IFMT))
			{
				isDir=YSTRUE;
			}
			else
			{
				isDir=YSFALSE;
				unsigned long long fileSize=st.st_size;
				sizeHigh=(fileSize>>32)&0x7fffffff;
				sizeLow=(fileSize)&0xffffffff;
			}

			YsWString wName;
			YsSystemEncodingToUnicode(wName,de->d_name);
			TestAndAddFileInfo(wName,isDir,sizeHigh,sizeLow);
		}
	}
	return YSOK;
}
