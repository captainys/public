/* ////////////////////////////////////////////////////////////

File Name: yssystemfont.h
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

#ifndef YSSYSTEMFONT_IS_INCLUDED
#define YSSYSTEMFONT_IS_INCLUDED
/* { */

#ifndef YSRESULT_IS_DEFINED
#define YSRESULT_IS_DEFINED
typedef enum
{
	YSERR,
	YSOK
} YSRESULT;
#endif

#ifndef YSBOOL_IS_DEFINED
#define YSBOOL_IS_DEFINED
typedef enum
{
	YSFALSE,
	YSTRUE,
	YSTFUNKNOWN
} YSBOOL;
#endif

class YsSystemFontTextBitmap
{
friend class YsSystemFontCache;

private:
	unsigned int wid,hei;
	unsigned int bytePerLine;
	unsigned int bitPerPixel;
	unsigned char *dat;
public:
	inline YsSystemFontTextBitmap();
	inline ~YsSystemFontTextBitmap();

	inline YsSystemFontTextBitmap(const YsSystemFontTextBitmap &incoming);
	inline YsSystemFontTextBitmap &operator=(const YsSystemFontTextBitmap &incoming);
	inline YsSystemFontTextBitmap(YsSystemFontTextBitmap &&incoming);
	inline YsSystemFontTextBitmap &operator=(YsSystemFontTextBitmap &&incoming);

	inline void CleanUp(void);
	inline void CopyFrom(const YsSystemFontTextBitmap &incoming);
	inline void MoveFrom(YsSystemFontTextBitmap &incoming);
	inline unsigned int Width(void) const;
	inline unsigned int Height(void) const;
	inline unsigned int BytePerLine(void) const;
	inline unsigned int BitPerPixel(void) const;
	inline const unsigned char *Bitmap(void) const;

	/*! Transfers the ownership of the bitmap array.
	    This class no longer manages the bitmap array.
	    Who takes ownership is responsible for deleting the array by the delete operator.
	*/
	inline unsigned char *TransferBitmap(void);

private:
	inline void SetBitmap(unsigned int wid,unsigned int hei,unsigned int bytePerLine,unsigned int bitPerPixel,unsigned char *dat);
};

inline YsSystemFontTextBitmap::YsSystemFontTextBitmap()
{
	dat=nullptr;
	CleanUp();
}

inline YsSystemFontTextBitmap::~YsSystemFontTextBitmap()
{
	CleanUp();
}

inline YsSystemFontTextBitmap::YsSystemFontTextBitmap(const YsSystemFontTextBitmap &incoming)
{
	dat=nullptr;
	wid=0;
	hei=0;
	CopyFrom(incoming);
}
inline YsSystemFontTextBitmap &YsSystemFontTextBitmap::operator=(const YsSystemFontTextBitmap &incoming)
{
	CopyFrom(incoming);
	return *this;
}
inline YsSystemFontTextBitmap::YsSystemFontTextBitmap(YsSystemFontTextBitmap &&incoming)
{
	dat=nullptr;
	wid=0;
	hei=0;
	MoveFrom(incoming);
}
inline YsSystemFontTextBitmap &YsSystemFontTextBitmap::operator=(YsSystemFontTextBitmap &&incoming)
{
	MoveFrom(incoming);
	return *this;
}

inline void YsSystemFontTextBitmap::CleanUp(void)
{
	if(nullptr!=dat)
	{
		delete [] dat;
		dat=nullptr;
	}
	wid=0;
	hei=0;
	bytePerLine=0;
	bitPerPixel=0;
}

inline void YsSystemFontTextBitmap::CopyFrom(const YsSystemFontTextBitmap &incoming)
{
	if(this!=&incoming)
	{
		this->wid=incoming.wid;
		this->hei=incoming.hei;
		this->bytePerLine=incoming.bytePerLine;
		this->bitPerPixel=incoming.bitPerPixel;

		this->dat=new unsigned char [incoming.bytePerLine*incoming.hei];
		for(long long int i=0; i<incoming.bytePerLine*incoming.hei; ++i)
		{
			this->dat[i]=incoming.dat[i];
		}
	}
}
inline void YsSystemFontTextBitmap::MoveFrom(YsSystemFontTextBitmap &incoming)
{
	if(this!=&incoming)
	{
		this->wid=incoming.wid;
		this->hei=incoming.hei;
		this->bytePerLine=incoming.bytePerLine;
		this->bitPerPixel=incoming.bitPerPixel;
		this->dat=incoming.dat;

		incoming.wid=0;
		incoming.hei=0;
		incoming.bytePerLine=0;
		incoming.bitPerPixel=0;
		incoming.dat=nullptr;
	}
}

inline unsigned int YsSystemFontTextBitmap::Width(void) const
{
	return wid;
}

inline unsigned int YsSystemFontTextBitmap::Height(void) const
{
	return hei;
}

inline unsigned int YsSystemFontTextBitmap::BytePerLine(void) const
{
	return bytePerLine;
}

inline unsigned int YsSystemFontTextBitmap::BitPerPixel(void) const
{
	return bitPerPixel;
}

inline const unsigned char *YsSystemFontTextBitmap::Bitmap(void) const
{
	return dat;
}

inline unsigned char *YsSystemFontTextBitmap::TransferBitmap(void)
{
	unsigned char *ret=dat;
	dat=nullptr;
	CleanUp();
	return ret;
}

inline void YsSystemFontTextBitmap::SetBitmap(unsigned int wid,unsigned int hei,unsigned int bytePerLine,unsigned int bitPerPixel,unsigned char *dat)
{
	CleanUp();
	this->wid=wid;
	this->hei=hei;
	this->bytePerLine=bytePerLine;
	this->bitPerPixel=bitPerPixel;
	this->dat=dat;
}



class YsSystemFontCache
{
private:
	class InternalData;
	InternalData *internal;

public:
	YsSystemFontCache();
	~YsSystemFontCache();

	YSRESULT RequestDefaultFont(void);
	YSRESULT RequestDefaultFontWithHeight(int height);


	inline YsSystemFontTextBitmap MakeSingleBitBitmap(const wchar_t wStr[],YSBOOL reverse) const;
	inline YsSystemFontTextBitmap MakeRGBABitmap(const wchar_t wStr[],const unsigned char fgCol[3],const unsigned char bgCol[3],YSBOOL reverse) const;
	inline YsSystemFontTextBitmap MakeGrayScaleAndAlphaBitmap(const wchar_t wStr[],unsigned char fgCol,unsigned char bgCol,YSBOOL reverse) const;

	inline YSRESULT MakeRGBABitmap(YsSystemFontTextBitmap &bmp,const wchar_t wStr[],unsigned int fgCol,const unsigned int bgCol,YSBOOL reverse) const;

	YSRESULT MakeSingleBitBitmap(YsSystemFontTextBitmap &bmp,const wchar_t wStr[],YSBOOL reverse) const;
	YSRESULT MakeRGBABitmap(YsSystemFontTextBitmap &bmp,const wchar_t wStr[],const unsigned char fgCol[3],const unsigned char bgCol[3],YSBOOL reverse) const;
	YSRESULT MakeGrayScaleAndAlphaBitmap(YsSystemFontTextBitmap &bmp,const wchar_t wStr[],unsigned char fgCol,unsigned char bgCol,YSBOOL reverse) const;
	YSRESULT GetTightBitmapSize(int &wid,int &hei,const wchar_t wStr[]) const;
};

inline YsSystemFontTextBitmap YsSystemFontCache::MakeSingleBitBitmap(const wchar_t wStr[],YSBOOL reverse) const
{
	YsSystemFontTextBitmap bmp;
	MakeSingleBitBitmap(bmp,wStr,reverse);
	return bmp;
}
inline YsSystemFontTextBitmap YsSystemFontCache::MakeRGBABitmap(const wchar_t wStr[],const unsigned char fgCol[3],const unsigned char bgCol[3],YSBOOL reverse) const
{
	YsSystemFontTextBitmap bmp;
	MakeRGBABitmap(bmp,wStr,fgCol,bgCol,reverse);
	return bmp;
}
inline YsSystemFontTextBitmap YsSystemFontCache::MakeGrayScaleAndAlphaBitmap(const wchar_t wStr[],unsigned char fgCol,unsigned char bgCol,YSBOOL reverse) const
{
	YsSystemFontTextBitmap bmp;
	MakeGrayScaleAndAlphaBitmap(bmp,wStr,fgCol,bgCol,reverse);
	return bmp;
}

inline YSRESULT YsSystemFontCache::MakeRGBABitmap(YsSystemFontTextBitmap &bmp,const wchar_t wStr[],unsigned int fgColI,const unsigned int bgColI,YSBOOL reverse) const
{
	const unsigned char fgCol[3]={(unsigned char)((fgColI>>16)&255),(unsigned char)((fgColI>>8)&255),(unsigned char)(fgColI&255)};
	const unsigned char bgCol[3]={(unsigned char)((bgColI>>16)&255),(unsigned char)((bgColI>>8)&255),(unsigned char)(bgColI&255)};
	return MakeRGBABitmap(bmp,wStr,fgCol,bgCol,reverse);
}

/* } */
#endif
