/* ////////////////////////////////////////////////////////////

File Name: ysmacosxsystemfont_objc.m
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

#import <Cocoa/Cocoa.h>
#include <stdlib.h>

extern unsigned char *YsMacOSXSystemFont_AllocBuffer(size_t bufSize);
// Why this function?
// Buffer needs to be allocated using the new operator, or the buffer cannot
// be released by the delete operator.  Therefore, allocation needs to be
// done in C++ world.



// 2018/09/29
// Undocumented changes in macOS 10.14 destroys Objective-C variables somewhere, and cannot find a reason.
// I'm sick and tired of playing along with arbitrary changes made in macOS versions.
// It is inefficient, but I create and destroy font-related objects every time I need to render fonts.
// So be it.



typedef struct
{
	unsigned int defHeight;
} YsMacOSXSystemFontContext;

void *YsMacOSXSystemFont_CreateContext(void)
{
	YsMacOSXSystemFontContext *ctxt;
	ctxt=(YsMacOSXSystemFontContext *)malloc(sizeof(YsMacOSXSystemFontContext));
	ctxt->defHeight=20;
	return ctxt;
}
void YsMacOSXSystemFont_FreeContext(void *ctxtPtr)
{
	free(ctxtPtr);
}
int YsMacOSXSystemFont_SelectSystemFontWithHeight(void *ctxtPtr,unsigned int height)
{
	int res=0;
	YsMacOSXSystemFontContext *ctxt=(YsMacOSXSystemFontContext *)ctxtPtr;
	ctxt->defHeight=height;
	return 1;
}
int YsMacOSXSystemFont_SelectSystemFont(void *ctxtPtr)
{
	return 1;
}



typedef struct
{
#if !__has_feature(objc_arc)
	NSAutoreleasePool *pool;
#endif

	NSFontManager *fontManager;
	// Font manager is not used in the v1.0
	NSFont *font;
	NSMutableParagraphStyle *parag;
	NSMutableDictionary *attrib;

	unsigned int defHeight;
} YsMacOSXSystemFontCache;

void *YsMacOSXSystemFont_CreateCache(int defHeight)
{
	YsMacOSXSystemFontCache *cache;
	cache=(YsMacOSXSystemFontCache *)malloc(sizeof(YsMacOSXSystemFontCache));
	cache->pool=NULL;
	cache->fontManager=NULL;
	cache->font=NULL;
	cache->parag=NULL;
	cache->attrib=NULL;
	cache->defHeight=defHeight;

#if !__has_feature(objc_arc)
	cache->pool=[[NSAutoreleasePool alloc] init];
#endif

	cache->fontManager=[NSFontManager sharedFontManager];

	const float fontSize=(float)cache->defHeight;
	cache->font=[NSFont systemFontOfSize:fontSize];

	cache->parag=[[NSMutableParagraphStyle alloc] init];
	[cache->parag setMaximumLineHeight:fontSize];

	cache->attrib=[NSMutableDictionary dictionary];
	[cache->attrib setObject:cache->font forKey:NSFontAttributeName];
	[cache->attrib setObject:[NSNumber numberWithFloat:0.0] forKey:NSKernAttributeName];
	[cache->attrib setObject:cache->parag forKey:NSParagraphStyleAttributeName];
	[cache->attrib setObject:[NSColor whiteColor] forKey:NSForegroundColorAttributeName];
	[cache->attrib setObject:[NSColor blackColor] forKey:NSBackgroundColorAttributeName];

	return cache;
}

void YsMacOSXSystemFont_FreeCache(void *cachePtr)
{
	YsMacOSXSystemFontCache *cache=(YsMacOSXSystemFontCache *)cachePtr;

#if !__has_feature(objc_arc)
	[cache->pool release];
	[cache->parag release];
	cache->pool=nil;
#endif

	// Can Objective-C can really auto-release cache->font and cache->attrib?
	cache->font=nil; // Does this invoke auto release?
	cache->attrib=nil; // Does this invoke auto release?
	cache->parag=nil;

	free(cachePtr);
}

unsigned char *YsMacOSXSystemFont_RGBABitmap(
	void *ctxtPtr,
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


	YsMacOSXSystemFontContext *ctxt=(YsMacOSXSystemFontContext *)ctxtPtr;
	YsMacOSXSystemFontCache *cache=YsMacOSXSystemFont_CreateCache(ctxt->defHeight);


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
								 initWithString:rawString attributes:cache->attrib];

			NSSize strSize=[string size];
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

	unsigned char *retBuf=NULL;
	NSBitmapImageRep *bmpImg=[[NSBitmapImageRep alloc]
								 initWithBitmapDataPlanes:NULL
								 pixelsWide:*wid
								 pixelsHigh:*hei
								 bitsPerSample:8
								 samplesPerPixel:3
								 hasAlpha:NO
								 isPlanar:NO
								 colorSpaceName:NSDeviceRGBColorSpace
								 bytesPerRow:0
								 bitsPerPixel:32];
	if(NULL!=bmpImg)
	{
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

		retBuf=YsMacOSXSystemFont_AllocBuffer((*dstBytePerLine)*(*hei));
		if(NULL!=retBuf)
		{
			for(i=0; i<(*dstBytePerLine)*(*hei); i++)
			{
				retBuf[i]=0;
			}

			NSGraphicsContext *gc=[NSGraphicsContext graphicsContextWithBitmapImageRep:bmpImg];
			NSGraphicsContext *prevGc=[NSGraphicsContext currentContext];
			[NSGraphicsContext setCurrentContext:gc];

			[gc setShouldAntialias:YES];
			[[NSColor blackColor] set];

			NSRect rect;
			rect.origin.x=0;
			rect.origin.y=0;
			rect.size.width=*wid;
			rect.size.height=*hei;
			NSRectFill(rect);

			[[NSColor whiteColor] set];
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
								initWithString:rawString attributes:cache->attrib];
					
					rect.size=[string size];
					rect.origin.y-=rect.size.height;
					[string drawWithRect:rect options:NSStringDrawingUsesLineFragmentOrigin];
					
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

			unsigned char *srcDat=[bmpImg bitmapData];
			unsigned int srcBytePerLine=[bmpImg bytesPerRow];
			int y;
			for(y=0; y<*hei; y++)
			{
				int yInBmp=(0==reverse ? y : *hei-1-y);
				unsigned char *srcLineTop=srcDat+yInBmp*srcBytePerLine;
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
			[NSGraphicsContext setCurrentContext:prevGc];
		}
	#if !__has_feature(objc_arc)
		[bmpImg release];
	#endif
	}

	if(NULL==retBuf)
	{
		*wid=0;
		*hei=0;
	}

	YsMacOSXSystemFont_FreeCache(cache);

	return retBuf;
}

void YsMacOSXSystemFont_GetTightBitmapSize(void *ctxtPtr,int *wid,int *hei,const wchar_t str[])
{
	if(NULL==str)
	{
		*wid=0;
		*hei=0;
		return;
	}

	YsMacOSXSystemFontContext *ctxt=(YsMacOSXSystemFontContext *)ctxtPtr;
	YsMacOSXSystemFontCache *cache=YsMacOSXSystemFont_CreateCache(ctxt->defHeight);

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
								 initWithString:rawString attributes:cache->attrib];

			NSSize strSize=[string size];
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

	YsMacOSXSystemFont_FreeCache(cache);
}
