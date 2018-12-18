/* ////////////////////////////////////////////////////////////

File Name: ysfilename.h
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

#ifndef YSFILENAME_IS_INCLUDED
#define YSFILENAME_IS_INCLUDED


#include "ysdef.h"
#include <stdio.h>


enum YSFTYPE
{
	YSFTYPE_UNKNOWN,
	YSFTYPE_SRF,
	YSFTYPE_SRM,
	YSFTYPE_DNM,
	YSFTYPE_FLD,
	YSFTYPE_STP,
	YSFTYPE_YFS,
	YSFTYPE_MSH,
	YSFTYPE_MS3,
	YSFTYPE_STL,
	YSFTYPE_BMP,
	YSFTYPE_PNG,
	YSFTYPE_S2D,
	YSFTYPE_PAR,
	YSFTYPE_NAS,
	YSFTYPE_ABAQUSINP,
	YSFTYPE_STARCDINP,
	YSFTYPE_HGC,
	YSFTYPE_HBS,
	YSFTYPE_SHL,
	YSFTYPE_TSHL,
	YSFTYPE_OBJ,
	YSFTYPE_OFF
};



YSFTYPE YsIdentifyFileType(const char fn[]);
YSFTYPE YsIdentifyFileType(const wchar_t fn[]);
void YsPutExt(char *fname,const char *ext);
unsigned long YsFileSize(const char *fname);
YSBOOL YsFileExist(const char *fn);
YSRESULT YsSeparatePathFile(char *pth,char *fil,const char *org);
void YsDeleteExtension(char def[]);
void YsReplaceExtension(char fn[],const char ext[]);


const char *YsGetExtensionFromFileType(YSFTYPE fType);
const wchar_t *YsGetWExtensionFromFileType(YSFTYPE fType);
YSFTYPE YsGetFileTypeFromExtension(const char ext[]);
YSFTYPE YsGetFileTypeFromExtension(const wchar_t ext[]);


class YsBufferedFileReader
{
protected:
	FILE *fp;
	YSSIZE_T bufSize,nBufFilled,nBufUsed;
	unsigned char *buf;
	unsigned filePtrHWord,filePtrLWord;
public:
	YsBufferedFileReader(YSSIZE_T bufSize=4*1024*1024);
	~YsBufferedFileReader();
	YSRESULT SetBufferSize(YSSIZE_T bufSize);
	YSRESULT Open(const char fn[]);
	YSSIZE_T Read(void *buf,YSSIZE_T readSize);
	YSRESULT Close(void);

	YSSIZE_T GetFilePtrKB(void) const;
};

class YsBufferedFileWriter
{
protected:
	FILE *fp;
	YSSIZE_T bufSize,nBufFilled;
	unsigned char *buf;
public:
	YsBufferedFileWriter(YSSIZE_T bufSize=4*1024*1024);
	~YsBufferedFileWriter();
	YSRESULT SetBufferSize(YSSIZE_T bufSize);
	YSRESULT Open(const char fn[]);
	YSRESULT Write(void *buf,YSSIZE_T writeSize);
	YSRESULT Close(void);
};

#endif
