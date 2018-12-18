/* ////////////////////////////////////////////////////////////

File Name: ysglmaketextbitmap.c
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

#include "ysglfontdata.h"
#include <stdio.h>

int YsGlWriteStringToSingleBitBitmap(
    const char str[],
    unsigned char bmpPtr[],unsigned int bmpWid,unsigned int bmpHei,
    int bottomLeftX,int bottomLeftY,
    const unsigned char *const fontPtr[],int fontWid,int fontHei)
{
	int bmpY0,bmpX,fontY0;
	int strPtr;

	const unsigned int bytePerBmpLine=((bmpWid+31)/32)*4;   /* Assuming 32bit alignment */
	const unsigned int bytePerFontLine=((fontWid+31)/32)*4;
	const unsigned int byteWritePerFontLine=(fontWid+7)/8;

	unsigned char *bmpLineTop0;

	if(bottomLeftY<0)
	{
		fontY0=-bottomLeftY;
		bmpY0=0;
	}
	else
	{
		fontY0=0;
		bmpY0=bottomLeftY;
	}

	bmpLineTop0=bmpPtr+bytePerBmpLine*bmpY0;


	bmpX=bottomLeftX;
	for(strPtr=0; 0!=str[strPtr]; strPtr++,bmpX+=fontWid)
	{
		const unsigned char c=str[strPtr];
		unsigned char *bmpLineTop;
		const unsigned char *fontLineTop;

		int fontY=fontY0;
		int bmpY=bmpY0;

		if(bmpX<=-fontWid)
		{
			continue;
		}

		bmpLineTop=bmpLineTop0;
		fontLineTop=fontPtr[c]+bytePerFontLine*fontY0;

		if(bmpX<0)
		{
			/* Partially visible
			   Optimization impact is low in this block >> */
			unsigned int hiddenWidth=-bmpX;

			unsigned int fontByteLoc=(hiddenWidth>>3);
			unsigned int fontBitShift=(hiddenWidth&7);
			unsigned char rightMask=(255>>(8-fontBitShift));

			while(fontY<(int)fontHei && bmpY<(int)bmpHei)
			{
				int i,bmpByteLoc;
				bmpByteLoc=0;
				for(i=fontByteLoc; i<(int)byteWritePerFontLine && bmpByteLoc<(int)bytePerBmpLine; i++)
				{
					unsigned char orByte=fontLineTop[i]<<fontBitShift;

					if(0!=fontBitShift && i+1<(int)byteWritePerFontLine)
					{
						orByte|=(fontLineTop[i+1]>>(8-fontBitShift))&rightMask;
					}

					bmpLineTop[bmpByteLoc]|=orByte;

					bmpByteLoc++;
				}

				bmpLineTop+=bytePerBmpLine;
				fontLineTop+=bytePerFontLine;

				fontY++;
				bmpY++;
			}
			/* Optimization impact is low in this block << */
		}
		else /* if(0<bmpX) */
		{
			while(fontY<(int)fontHei && bmpY<(int)bmpHei)
			{
				unsigned int bitShift=(bmpX&7);
				unsigned int byteLoc=(bmpX>>3);

				if(0!=bitShift)
				{
					const unsigned char leftMask=(255<<bitShift);
					const unsigned char rightMask=(255>>(8-bitShift));
					int i;
					for(i=0; i<(int)byteWritePerFontLine && byteLoc+i<(int)bytePerBmpLine; i++)
					{
						unsigned char byteToCopy=fontLineTop[i];
						unsigned char left;

						left=(byteToCopy&leftMask)>>bitShift;
						bmpLineTop[byteLoc+i  ]|=left;

						if(byteLoc+i+1<bytePerBmpLine)
						{
							unsigned char right;
							right=(byteToCopy&rightMask)<<(8-bitShift);
							bmpLineTop[byteLoc+i+1]|=right;
						}
					}
				}
				else
				{
					int i;
					for(i=0; i<(int)byteWritePerFontLine && byteLoc+i<(int)bytePerBmpLine; i++)
					{
						bmpLineTop[byteLoc+i]|=fontLineTop[i];
					}
				}

				bmpLineTop+=bytePerBmpLine;
				fontLineTop+=bytePerFontLine;

				fontY++;
				bmpY++;
			}
		}
	}

	return 0;
}



int YsGlWriteStringToRGBA8Bitmap(
    const char str[],unsigned int c0,unsigned int c1,unsigned int c2,unsigned int c3,
    unsigned char bmpPtr[],unsigned int bmpWid,unsigned int bmpHei,
    int bottomLeftX,int bottomLeftY,
    const unsigned char *const fontPtr[],int fontWid,int fontHei)
/* In OpenGL:   c0=r  c1=g  c2=b  c3=a
   In Direct3D: c0=a  c1=r  c2=g  c3=b */
{
	int bmpY0,bmpX,fontY0;
	int strPtr;

	const unsigned int bytePerBmpLine=bmpWid*4;
	const unsigned int bytePerFontLine=((fontWid+31)/32)*4;

	unsigned char *bmpLineTop0;

	if(bottomLeftY<0)
	{
		fontY0=-bottomLeftY;
		bmpY0=0;
	}
	else
	{
		fontY0=0;
		bmpY0=bottomLeftY;
	}

	bmpLineTop0=bmpPtr+bytePerBmpLine*bmpY0;


	bmpX=bottomLeftX;
	for(strPtr=0; 0!=str[strPtr]; strPtr++,bmpX+=fontWid)
	{
		const unsigned char c=str[strPtr];
		unsigned char *bmpLineTop;
		const unsigned char *fontLineTop;

		int fontY=fontY0;
		int bmpY=bmpY0;

		int x;
		unsigned int fontX;
		unsigned int bit;

		if(bmpX<=-fontWid)
		{
			continue;
		}

		bmpLineTop=bmpLineTop0;
		fontLineTop=fontPtr[c]+bytePerFontLine*fontY0;


		while(fontY<(int)fontHei && bmpY<(int)bmpHei)
		{
			fontX=0;
			bit=fontLineTop[fontX];

			x=0;

			while(x+bmpX<0)
			{
				bit<<=1;
				if(7==(x&7))
				{
					bit=fontLineTop[++fontX];
				}
				x++;
			}

			for(x=x; x<(int)fontWid && x+bmpX<(int)bmpWid; x++)
			{
				if(bit&0x80)
				{
					unsigned int bmpXTimes4=((x+bmpX)<<2);
					bmpLineTop[bmpXTimes4  ]=(unsigned char)c0;
					bmpLineTop[bmpXTimes4+1]=(unsigned char)c1;
					bmpLineTop[bmpXTimes4+2]=(unsigned char)c2;
					bmpLineTop[bmpXTimes4+3]=(unsigned char)c3;
				}

				bit<<=1;
				if(7==(x&7))
				{
					bit=fontLineTop[++fontX];
				}
			}

			bmpLineTop+=bytePerBmpLine;
			fontLineTop+=bytePerFontLine;

			fontY++;
			bmpY++;
		}
	}

	return 0;
}



int YsGlWriteStringToRGBA8BitmapPropFont(
    const char str[],unsigned int c0,unsigned int c1,unsigned int c2,unsigned int c3,
    unsigned char bmpPtr[],unsigned int bmpWid,unsigned int bmpHei,
    int bottomLeftX,int bottomLeftY,
    const unsigned char *const fontPtr[],const int fontWid[],int fontHei)
/* In OpenGL:   c0=r  c1=g  c2=b  c3=a
   In Direct3D: c0=a  c1=r  c2=g  c3=b */
{
	int bmpY0,bmpX,fontY0;
	int strPtr;

	const unsigned int bytePerBmpLine=bmpWid*4;

	unsigned char *bmpLineTop0;

	if(bottomLeftY<0)
	{
		fontY0=-bottomLeftY;
		bmpY0=0;
	}
	else
	{
		fontY0=0;
		bmpY0=bottomLeftY;
	}

	bmpLineTop0=bmpPtr+bytePerBmpLine*bmpY0;


	bmpX=bottomLeftX;
	for(strPtr=0; 0!=str[strPtr]; strPtr++)
	{
		const unsigned char c=str[strPtr];
		unsigned char *bmpLineTop;
		const unsigned char *fontLineTop;

		const unsigned int bytePerFontLine=((fontWid[c]+31)/32)*4;

		int fontY=fontY0;
		int bmpY=bmpY0;

		int x;
		unsigned int fontX;
		unsigned int bit;

		if(bmpX<=-fontWid[c])
		{
			bmpX+=fontWid[c];
			continue;
		}

		bmpLineTop=bmpLineTop0;
		fontLineTop=fontPtr[c]+bytePerFontLine*fontY0;


		while(fontY<(int)fontHei && bmpY<(int)bmpHei)
		{
			fontX=0;
			bit=fontLineTop[fontX];

			x=0;

			while(x+bmpX<0)
			{
				bit<<=1;
				if(7==(x&7))
				{
					bit=fontLineTop[++fontX];
				}
				x++;
			}

			for(x=x; x<(int)fontWid[c] && x+bmpX<(int)bmpWid; x++)
			{
				if(bit&0x80)
				{
					unsigned int bmpXTimes4=((x+bmpX)<<2);
					bmpLineTop[bmpXTimes4  ]=(unsigned char)c0;
					bmpLineTop[bmpXTimes4+1]=(unsigned char)c1;
					bmpLineTop[bmpXTimes4+2]=(unsigned char)c2;
					bmpLineTop[bmpXTimes4+3]=(unsigned char)c3;
				}

				bit<<=1;
				if(7==(x&7))
				{
					bit=fontLineTop[++fontX];
				}
			}

			bmpLineTop+=bytePerBmpLine;
			fontLineTop+=bytePerFontLine;

			fontY++;
			bmpY++;
		}

		bmpX+=fontWid[c];
	}

	return 0;
}



