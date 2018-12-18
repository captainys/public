/* ////////////////////////////////////////////////////////////

File Name: ysiossystemfont_objc.m
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

@import UIKit;
#include <stdlib.h>


// According to the description,
// https://developer.apple.com/library/ios/documentation/StringsTextFonts/Conceptual/CoreText_Programming/LayoutOperations/LayoutOperations.html#//apple_ref/doc/uid/TP40005533-CH12-SW8
// CoreText API should be available in both MacOSX and iOS.
//   Which seems to be a false hope....



// I don't want to re-allocate and destroy font attribute every time I render a text.
// However, ARC of Objective-C does not allow returning an object pointer.
// That's one of many reasons why Objective-C is inferior to C++.
// Objective-C is the biggest weakness of MacOSX and iOS, and should have gone much earlier.
// Looks like I need to make a static NSMutableDictionary, save font attribute in the dictionary, and return a key
// to the C++ part.  C++ part will pass the key and Objective-C part will recall it.
// Looks like covering by [NSNumber numberWithInt:keyValue] will allow using an integer as key, according to the following.
//   http://stackoverflow.com/questions/4815160/how-can-i-use-an-integer-value-as-key-to-set-value-in-nsmutabledictionary
// Will look into this issue in the future.



extern unsigned char *YsIOSSystemFont_AllocBuffer(size_t bufSize);
// Why this function?
// Buffer needs to be allocated using the new operator, or the buffer cannot
// be released by the delete operator.  Therefore, allocation needs to be
// done in C++ world.


NSMutableDictionary *YsIOS_MakeSystemFontAttribute(unsigned int height)
{
    const float fontSize=(float)height;
    UIFont *newFont=[UIFont systemFontOfSize:fontSize];
    if(NULL!=newFont)
    {
        NSMutableParagraphStyle *paragStyle=[[NSMutableParagraphStyle alloc] init];
        if(nil!=paragStyle)
        {
            [paragStyle setMaximumLineHeight:fontSize];
            
            NSMutableDictionary *attrib=[NSMutableDictionary dictionary];
            [attrib setObject:newFont forKey:NSFontAttributeName];
            [attrib setObject:[NSNumber numberWithFloat:0.0F] forKey:NSKernAttributeName];
            [attrib setObject:paragStyle forKey:NSParagraphStyleAttributeName];
            [attrib setObject:[UIColor whiteColor] forKey:NSForegroundColorAttributeName];
            [attrib setObject:[UIColor blackColor] forKey:NSBackgroundColorAttributeName];
            return attrib;
        }
    }
 
    return nil;
}

unsigned char *YsIOSSystemFont_RGBABitmap(
	int fontHeight,
	unsigned int *wid,unsigned int *hei,unsigned int *dstBytePerLine,
	const wchar_t str[],unsigned int dstBitPerPixel,
	const unsigned char fgCol[3],const unsigned char bgCol[3],
	int reverse)
{
	if(NULL==str)
	{
		*wid=0;
		*hei=0;
		*dstBytePerLine=0;
		return NULL;
	}


	wchar_t tst=0x0001;
	unsigned char *tstByte=(unsigned char *)&tst;
	NSStringEncoding encoding;
    NSMutableDictionary *attrib=YsIOS_MakeSystemFontAttribute(fontHeight);

	if(0!=tstByte[0])
	{
		encoding=NSUTF16LittleEndianStringEncoding;
	}
	else
	{
		encoding=NSUTF16BigEndianStringEncoding;
	}

	*wid=0;
	*hei=0;

	int i=0,i0=0;
	for(;;)
	{
		if(0==str[i] || '\n'==str[i])
		{
			NSString *rawString=[[NSString alloc]
									initWithBytes:str+i0 
									length:(i-i0)*sizeof(wchar_t) 
									encoding:encoding];

			NSAttributedString *string=[[NSAttributedString alloc]
								 initWithString:rawString attributes:attrib];

			CGSize strSize=[string size];
			*wid=(*wid<strSize.width ? strSize.width : *wid);
			*hei+=strSize.height;

		#if !__has_feature(objc_arc)
			[rawString release];
			[string release];
		#endif
			i0=i+1;
		}
		if(0==str[i])
		{
			break;
		}
		i++;
	}

	if(0==*wid || 0==*hei)
	{
		*dstBytePerLine=0;
		return NULL;
	}

	*wid=(*wid+3)&~3;


    switch(dstBitPerPixel)
	{
	case 1:
        *dstBytePerLine=((*wid+31)&~31)/8;
		break;
	case 16:
	case 32:
		*dstBytePerLine=(*wid)*dstBitPerPixel/8;
		break;
	}


    unsigned char *retBuf=YsIOSSystemFont_AllocBuffer((*dstBytePerLine)*(*hei));
	if(NULL!=retBuf)
	{
		for(i=0; i<(*dstBytePerLine)*(*hei); i++)
		{
			retBuf[i]=0;
		}

        CGSize bmpSize;
        bmpSize.width=*wid;
        bmpSize.height=*hei;
        UIGraphicsBeginImageContext(bmpSize);

		[[UIColor blackColor] setFill];

		CGRect rect;
		rect.origin.x=0;
		rect.origin.y=0;
		rect.size.width=*wid;
		rect.size.height=*hei;
		UIRectFill(rect);

		[[UIColor whiteColor] setFill];
		i=0;
		i0=0;
		rect.origin.y=*hei-1;
		for(;;)
		{
			if(0==str[i] || '\n'==str[i])
			{
				NSString *rawString=[[NSString alloc]
							initWithBytes:str+i0
							length:(i-i0)*sizeof(wchar_t)
							encoding:encoding];
				NSAttributedString *string=[[NSAttributedString alloc]
							initWithString:rawString attributes:attrib];

				rect.size=[string size];
				rect.origin.y-=rect.size.height;
                [string drawInRect:rect]; // What about options:NSStringDrawingUsesLineFragmentOrigin ?
				
			#if !__has_feature(objc_arc)
				[rawString release];
				[string release];
			#endif
				i0=i+1;
			}
			if(0==str[i])
			{
				break;
			}
			i++;
		}

        UIImage *image=UIGraphicsGetImageFromCurrentImageContext();
        
        CGImageRef bmpImgRef=[image CGImage];
        CFDataRef rawData=CGDataProviderCopyData(CGImageGetDataProvider(bmpImgRef));
        const unsigned char *srcDat=CFDataGetBytePtr(rawData);
		size_t srcBytePerLine=CGImageGetBytesPerRow(bmpImgRef);
		int y;
		for(y=0; y<*hei; y++)
		{
			int yInBmp=(0==reverse ? y : *hei-1-y);
			const unsigned char *srcLineTop=srcDat+yInBmp*srcBytePerLine;
			unsigned char *dstLineTop=retBuf+y*(*dstBytePerLine);

			unsigned char *dstPtr=dstLineTop;
			unsigned char dstMask=0x80;

			int x;
			for(x=0; x<*wid; x++)
			{
				const unsigned char *col;
				unsigned char alpha=srcLineTop[x*4];
				if(2<srcLineTop[x*4])
				{
					col=fgCol;
				}
				else
				{
					col=bgCol;
				}
				
				switch(dstBitPerPixel)
				{
				case 1:
					if(2<alpha)
					{
						(*dstPtr)|=dstMask;
					}
					
					if(1==dstMask)
					{
						dstMask=0x80;
						dstPtr++;
					}
					else
					{
						dstMask>>=1;
					}
					break;
				case 16:
					dstLineTop[x*2  ]=col[0];
					dstLineTop[x*2+1]=alpha;
					break;
				case 32:
					dstLineTop[x*4  ]=col[0];
					dstLineTop[x*4+1]=col[1];
					dstLineTop[x*4+2]=col[2];
					dstLineTop[x*4+3]=alpha;
					break;
				}
			}
		}
        UIGraphicsEndImageContext();
    }
	#if !__has_feature(objc_arc)
		[bmpImg release];
	#endif

	if(NULL==retBuf)
	{
		*wid=0;
		*hei=0;
	}

	return retBuf;
}

void YsIOSSystemFont_GetTightBitmapSize(int fontSize,int *wid,int *hei,const wchar_t str[])
{
	if(NULL==str)
	{
		*wid=0;
		*hei=0;
		return;
	}

    NSMutableDictionary *attrib=YsIOS_MakeSystemFontAttribute(fontSize);

	wchar_t tst=0x0001;
	unsigned char *tstByte=(unsigned char *)&tst;
	NSStringEncoding encoding;

	if(0!=tstByte[0])
	{
		encoding=NSUTF16LittleEndianStringEncoding;
	}
	else
	{
		encoding=NSUTF16BigEndianStringEncoding;
	}

	*wid=0;
	*hei=0;

	int i=0,i0=0;
	for(;;)
	{
		if(0==str[i] || '\n'==str[i])
		{
			NSString *rawString=[[NSString alloc]
									initWithBytes:str+i0 
									length:(i-i0)*sizeof(wchar_t) 
									encoding:encoding];

			NSAttributedString *string=[[NSAttributedString alloc]
								 initWithString:rawString attributes:attrib];

			CGSize strSize=[string size];
			*wid=(*wid<strSize.width ? strSize.width : *wid);
			*hei+=strSize.height;

		#if !__has_feature(objc_arc)
			[rawString release];
			[string release];
		#endif
			i0=i+1;
		}
		if(0==str[i])
		{
			break;
		}
		i++;
	}

	if(0==*wid || 0==*hei)
	{
		*wid=0;
		*hei=0;
	}
}
