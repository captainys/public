/* ////////////////////////////////////////////////////////////

File Name: fsguiclipboard_mac.m
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

#include <Cocoa/Cocoa.h>

int FsGuiCopyStringToClipBoardC(const char cstr[])
{
	NSAutoreleasePool *pool=[[NSAutoreleasePool alloc] init];

	NSPasteboard *pasteboard=[NSPasteboard generalPasteboard];

	[pasteboard clearContents];


	NSString *str=[[NSString alloc] initWithUTF8String:cstr];

	NSMutableArray *toCopy=[[NSMutableArray alloc] init];
	[toCopy addObject:str];

	BOOL res=[pasteboard writeObjects:toCopy];

	[str release];

	[pool release];

	return 1;
}



int FsGuiGetStringLengthInClipBoardC(void)
{
	int n=0;

	NSAutoreleasePool *pool=[[NSAutoreleasePool alloc] init];

	NSPasteboard *pasteboard=[NSPasteboard generalPasteboard];

	NSArray *classArray=[[NSMutableArray alloc] initWithObjects:[NSString class],nil];
	NSDictionary *options=[NSDictionary dictionary];

	NSArray *clipboardContent=[pasteboard readObjectsForClasses:classArray options:options];

	if(nil!=clipboardContent)
	{
		if(0<[clipboardContent count])
		{
			NSString *str=[clipboardContent objectAtIndex:0];
			if(NULL!=str)
			{
				n=strlen([str UTF8String]);
			}
		}
	}

	[classArray release];

	[pool release];

	return n;
}

int FsGuiCopyStringFromClipBoardC(char cstr[])
{
	NSAutoreleasePool *pool=[[NSAutoreleasePool alloc] init];

	NSPasteboard *pasteboard=[NSPasteboard generalPasteboard];

	NSArray *classArray=[[NSMutableArray alloc] initWithObjects:[NSString class],nil];
	NSDictionary *options=[NSDictionary dictionary];

	NSArray *clipboardContent=[pasteboard readObjectsForClasses:classArray options:options];

	if(nil!=clipboardContent)
	{
		if(0<[clipboardContent count])
		{
			NSString *str=[clipboardContent objectAtIndex:0];
			if(NULL!=str)
			{
				strcpy(cstr,[str UTF8String]);
			}
		}
	}

	[classArray release];

	[pool release];

	return 1;
}
