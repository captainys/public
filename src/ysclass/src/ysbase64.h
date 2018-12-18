/* ////////////////////////////////////////////////////////////

File Name: ysbase64.h
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

#ifndef YSBASE___IS_INCLUDED
#define YSBASE___IS_INCLUDED
/* { */

#include "ysarray.h"
#include "ysstring.h"

YSRESULT YsEncodeBase64(YsArray <YsString> &base64,YSSIZE_T length,const unsigned char byteData[]);

template <const int N>
YSRESULT YsEncodeBase64(YsArray <YsString> &base64,const YsArray <unsigned char,N> &byteData)
{
	return YsEncodeBase64(base64,byteData.GetN(),byteData);
}


class YsBase64Decoder
{
private:
	enum
	{
		BUFSIZE=4096,
		BITPERBYTE=8
	};

	int nBufUsed,nBitUsed;
	unsigned char bitBuf[BUFSIZE];
	unsigned char decodeTable[256];

public:
	YsBase64Decoder();
	void CleanUp(void);
private:
	void WriteBit(YsArray <unsigned char> &outBuf,unsigned int bit,int bitLength);
public:
	void DecodeString(YsArray <unsigned char> &outBuf,const char str[]);
	void DecodeString(YsArray <unsigned char> &outBuf,const YsString &str);
	void Flush(YsArray <unsigned char> &outBuf);
};


/* } */
#endif
