/* ////////////////////////////////////////////////////////////

File Name: ystextstream.cpp
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

#if (defined(_WIN32) || defined(_WIN64)) && !defined(_CRT_SECURE_NO_WARNINGS)
	// This disables annoying warnings VC++ gives for use of C standard library.  Shut the mouth up.
	#define _CRT_SECURE_NO_WARNINGS
#endif

#include <stdio.h>
#include <stdarg.h>

#include "yssegarray.h"
#include "ystextstream.h"
#include "ysstring.h"


YsTextInputStream::YsTextInputStream()
{
	endOfFile=YSFALSE;
}

////////////////////////////////////////////////////////////

YSBOOL YsTextInputStream::EndOfFile(void) const
{
	return endOfFile;
}
YsString YsTextInputStream::Gets(void)
{
	YsString str;
	Gets(str);
	return str;
}
YsString YsTextInputStream::Fgets(void)
{
	YsString str;
	Gets(str);
	return str;
}

////////////////////////////////////////////////////////////

YsTextFileInputStream::YsTextFileInputStream(FILE *fp)
{
	this->fp=fp;
}

YsTextFileInputStream::YsTextFileInputStream(const YsTextFileInputStream &incoming)
{
	this->fp=incoming.fp;
}
YsTextFileInputStream &YsTextFileInputStream::operator=(const YsTextFileInputStream &incoming)
{
	this->fp=incoming.fp;
	return *this;
}

const char *YsTextFileInputStream::Gets(YsString &str)
{
	if(nullptr==str.Fgets(fp))
	{
		endOfFile=YSTRUE;
		return nullptr;
	}
	return str;
}



YsTextFileOutputStream::YsTextFileOutputStream(FILE *fp)
{
	this->fp=fp;
}

YsTextFileOutputStream::YsTextFileOutputStream(const YsTextFileOutputStream &incoming)
{
	this->fp=incoming.fp;
}
YsTextFileOutputStream &YsTextFileOutputStream::operator=(const YsTextFileOutputStream &incoming)
{
	this->fp=incoming.fp;
	return *this;
}

YSRESULT YsTextFileOutputStream::Puts(const char str[])
{
	if(EOF!=fputs(str,fp))
	{
		return YSOK;
	}
	return YSERR;
}

YSRESULT YsTextFileOutputStream::Puts(const YsString &str)
{
	if(EOF!=fputs(str,fp))
	{
		return YSOK;
	}
	return YSERR;
}

YSRESULT YsTextFileOutputStream::Printf(const char fmt[],...)
{
	va_list arg_ptr;
	va_start(arg_ptr,fmt);
	vfprintf(fp,fmt,arg_ptr);
	va_end(arg_ptr);
	return YSOK;

}



YsTextMemoryOutputStream::YsTextMemoryOutputStream()
{
	curLineIdx=0;
}

void YsTextMemoryOutputStream::CleanUp(void)
{
	YsSegmentedArray <YsString,8>::CleanUp();
	curLineIdx=0;
}

YsString &YsTextMemoryOutputStream::GetCurrentLine(void)
{
	if(YSTRUE==IsInRange(curLineIdx))
	{
		return (*this)[curLineIdx];
	}
	else
	{
		return NewLine();
	}
}

YsString &YsTextMemoryOutputStream::NewLine(void)
{
	curLineIdx=this->GetN();
	this->Increment();
	return this->Last();
}

YSRESULT YsTextMemoryOutputStream::Puts(const char str[])
{
	int i,head=0,count=0;
	for(i=0; 0!=str[i]; i++)
	{
		if('\n'==str[i])
		{
			auto &ptr=GetCurrentLine();
			ptr.Append(count,str+head);
			NewLine();
			count=0;
			head=i+1;
		}
		else
		{
			count++;
		}
	}

	if(0<count)
	{
		auto &ptr=GetCurrentLine();
		ptr.Append(count,str+head);
	}

	return YSOK;
}

YSRESULT YsTextMemoryOutputStream::Puts(const YsString &str)
{
	return Puts(str.Txt());
}

YSRESULT YsTextMemoryOutputStream::Printf(const char fmt[],...)
{
	const int iniBufSize=256;
	YsArray <char,256> buf;
	buf.Set(iniBufSize,NULL);
	buf[0]=0;

	for(;;)
	{
		const size_t bufSize=buf.GetN();
		va_list arg_ptr;

		va_start(arg_ptr,fmt);
		const int n=vsnprintf(buf,bufSize-1,fmt,arg_ptr);
		va_end(arg_ptr);

		if(n<0)
		{
			// This compiler s**ks!  It's not C99 compliant!  (I'm talking about Visual C++ 2008)
			buf.Resize(bufSize*2);
		}
		else if(n<(int)bufSize)
		{
			break;
		}
		else
		{
			// Good compiler!  It is C99 Compliant.
			buf.Resize(n+8);
		}
	}

	return Puts(buf);
}

const YSSIZE_T YsTextMemoryOutputStream::GetNumLine(void) const
{
	return this->GetN();
}

////////////////////////////////////////////////////////////


YsTextMemoryInputStream::YsTextMemoryInputStream(const char *const inStr[])
{
	ptr=0;
	this->inStr=inStr;
	mode=MODE_CSTRING_ARRAY;
}

YsTextMemoryInputStream::YsTextMemoryInputStream(const YsConstArrayMask <YsString> &inStr)
{
	ptr=0;
	this->nYsStr=inStr.GetN();
	this->inYsStr=inStr;
	mode=MODE_YSSTRING_ARRAY;
}

/* virtual */ const char *YsTextMemoryInputStream::Gets(YsString &str)
{
	switch(mode)
	{
	default:
		break;
	case MODE_CSTRING_ARRAY:
		if(NULL!=inStr[ptr])
		{
			str=inStr[ptr];
			++ptr;
			return str;
		}
		break;
	case MODE_YSSTRING_ARRAY:
		if(ptr<nYsStr)
		{
			str=inYsStr[ptr];
			++ptr;
			return str;
		}
	}
	str="";
	endOfFile=YSTRUE;
	return nullptr;
}
