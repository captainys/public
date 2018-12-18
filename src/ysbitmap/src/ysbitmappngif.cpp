/* ////////////////////////////////////////////////////////////

File Name: ysbitmappngif.cpp
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

#include <stdio.h>

#ifdef WIN32
#include <windows.h>
#endif

#include "ysbitmap.h"

#include "yspng.h"  // <- For compiling YSFLIGHT code
#include "yspngenc.h"


YSRESULT YsBitmap::LoadPng(const char fn[])
{
	YsRawPngDecoder pngDec;
	if(pngDec.Decode(fn)==YSOK)
	{
		MoveFrom(pngDec);
		return YSOK;
	}
	return YSERR;
}

YSRESULT YsBitmap::LoadPng(FILE *fp)
{
	YsRawPngDecoder pngDec;
	if(pngDec.Decode(fp)==YSOK)
	{
		MoveFrom(pngDec);
		return YSOK;
	}
	return YSERR;
}

YSRESULT YsBitmap::LoadPng(size_t nByte,const unsigned char binaryData[])
{
	YsRawPngDecoder pngDec;
	YsPngBinaryMemoryStream inStream(nByte,binaryData);
	if(pngDec.Decode(inStream)==YSOK)
	{
		MoveFrom(pngDec);
		return YSOK;
	}
	return YSERR;
}

YsBitmap &YsBitmap::operator=(class YsRawPngDecoder &&pngDecoder)
{
	return MoveFrom(pngDecoder);
}

YsBitmap &YsBitmap::MoveFrom(class YsRawPngDecoder &pngDec)
{
	pngDec.Flip();
	SetDirect(pngDec.wid,pngDec.hei,pngDec.rgba);
	pngDec.autoDeleteRgbaBuffer=YSFALSE;
	return *this;
}

YSRESULT YsBitmap::SavePng(FILE *fp) const
{
	YsRawPngEncoder encoder;
	return (YSRESULT)encoder.EncodeToFile(fp,GetWidth(),GetHeight(),8,6,GetRGBABitmapPointer());
}
