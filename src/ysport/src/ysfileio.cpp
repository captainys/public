/* ////////////////////////////////////////////////////////////

File Name: ysfileio.cpp
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

#include "ysfileio.h"

/* static */ YsWString YsFileIO::Getcwd(void)
{
	YsWString cwd;
	Getcwd(cwd);
	return cwd;
}

/* static */ YSBOOL YsFileIO::CheckFileExist(const wchar_t fn[])
{
	auto fp=Fopen(fn,"rb");
	if(NULL!=fp)
	{
		fclose(fp);
		return YSTRUE;
	}
	return YSFALSE;
}
/* static */ YSBOOL YsFileIO::CheckFileExist(const char fn[])
{
	auto fp=Fopen(fn,"rb");
	if(NULL!=fp)
	{
		fclose(fp);
		return YSTRUE;
	}
	return YSFALSE;
}


////////////////////////////////////////////////////////////

YsFileIO::File::File()
{
	fp=NULL;
}

YsFileIO::File::File(const wchar_t fn[],const char mode[])
{
	fp=YsFileIO::Fopen(fn,mode);
	if(nullptr!=fp)
	{
		this->mode=mode;
	}
}

YsFileIO::File::File(const char fn[],const char mode[])
{
	fp=YsFileIO::Fopen(fn,mode);
	if(nullptr!=fp)
	{
		this->mode=mode;
	}
}

void YsFileIO::File::MoveFrom(File &incoming)
{
	if(this!=&incoming)
	{
		this->fp=incoming.fp;
		this->mode=incoming.mode;
		incoming.fp=nullptr;
	}
}

YsFileIO::File::File(File &&incoming)
{
	MoveFrom(incoming);
}
YsFileIO::File &YsFileIO::File::operator=(File &&incoming)
{
	MoveFrom(incoming);
	return *this;
}

YsFileIO::File::~File()
{
	Fclose();
}

YsFileIO::File *YsFileIO::File::Fopen(const wchar_t fn[],const char mode[])
{
	if(nullptr==fp)
	{
		fp=YsFileIO::Fopen(fn,mode);
		if(nullptr!=fp)
		{
			this->mode=mode;
			return this;
		}
	}
	return nullptr;
}
YsFileIO::File *YsFileIO::File::Fopen(const char fn[],const char mode[])
{
	if(nullptr==fp)
	{
		fp=YsFileIO::Fopen(fn,mode);
		if(nullptr!=fp)
		{
			this->mode=mode;
			return this;
		}
	}
	return nullptr;
}

void YsFileIO::File::Fclose(void)
{
	if(NULL!=fp)
	{
		fclose(fp);
		fp=NULL;
		mode="";
	}
}

FILE *YsFileIO::File::Fp(void) const
{
	return fp;
}


YsArray <unsigned char> YsFileIO::File::Fread(void)
{
	YsArray <unsigned char> dat;
	if(nullptr!=fp && mode[0]=='r')
	{
		fseek(fp,0,SEEK_END);
		YSSIZE_T sz=ftell(fp);
		fseek(fp,0,SEEK_SET);

		dat.resize(sz);
		fread(dat.data(),1,sz,fp);
	}
	return dat;
}
YSRESULT YsFileIO::File::Fwrite(YsConstArrayMask <unsigned char> byteArray)
{
	if(nullptr!=fp && mode[0]=='w')
	{
		fwrite(byteArray.data(),1,byteArray.size(),fp);
		return YSOK;
	}
	return YSERR;
}

YsArray <YsString> YsFileIO::File::ReadText(void)
{
	YsArray <YsString> dat;
	if(nullptr!=fp && mode[0]=='r')
	{
		YsString str;
		while(nullptr!=str.Fgets(fp))
		{
			dat.push_back((YsString &&)str);
		}
	}
	return dat;
}

YSRESULT YsFileIO::File::WriteText(const YsConstArrayMask <YsString> txt)
{
	if(nullptr!=fp && mode[0]=='w')
	{
		for(auto &s : txt)
		{
			fprintf(fp,"%s\n",s.c_str());
		}
		return YSOK;
	}
	return YSERR;
}
