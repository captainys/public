/* ////////////////////////////////////////////////////////////

File Name: ysmacosxfileinfo.cpp
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

#include <unistd.h>
#include <sys/stat.h>

extern "C" int YsMacOSXGetFileList(void *objPointer,const char dir[],int errCode,int okCode);

YSRESULT YsFileList::FindFileList(const char path[],const char prefix[],const char postfix[])
{
	YsWString wPath,wPrefix,wPostfix;
	YsSystemEncodingToUnicode(wPath,path);
	YsSystemEncodingToUnicode(wPrefix,prefix);
	YsSystemEncodingToUnicode(wPostfix,postfix);
	return FindFileList(wPath,wPrefix,wPostfix);
}

YSRESULT YsFileList::FindFileList(const wchar_t path[],const wchar_t prefix[],const wchar_t postfix[])
{
	this->path.Set(path);
	if(0<this->path.Strlen() && ('/'==this->path.LastChar() || '\\'==this->path.LastChar()))
	{
		this->path.BackSpace();
	}

	if(NULL!=prefix)
	{
		this->uPrefix.Set(prefix);
	}
	else
	{
		this->uPrefix.Set(NULL);  // Changed from L"" to NULL.  Workaround for GCC 64-bit bug
	}
	if(NULL!=postfix)
	{
		this->uPostfix.Set(postfix);
	}
	else
	{
		this->uPostfix.Set(NULL); // Changed from L"" to NULL.  Workaround for GCC 64-bit bug
	}
	this->uPrefix.Uncapitalize();
	this->uPostfix.Uncapitalize();

	Initialize();
	YsString utf8;
	utf8.EncodeUTF8 <wchar_t> (path);
	if((int)YSOK==YsMacOSXGetFileList(this,utf8,(int)YSERR,(int)YSOK))
	{
		return YSOK;
	}
	else
	{
		return YSERR;
	}
}

extern "C" void YsMacOSXGetFileListCallBack(void *objPointer,const char fn[],int isDirectory,unsigned long long fileSize)
{
	YsFileList *fileList=(YsFileList *)objPointer;
	unsigned long sizeHigh=(fileSize>>32)&0x7fffffff;
	unsigned long sizeLow=(fileSize&0xffffffff);
	YSBOOL isDir=(isDirectory==0 ? YSFALSE : YSTRUE);

	YsWString wFn;
	wFn.SetUTF8String(fn);
	fileList->TestAndAddFileInfo(wFn,isDir,sizeHigh,sizeLow);
}
