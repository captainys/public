/* ////////////////////////////////////////////////////////////

File Name: ysmacosxfileinfo_objc.m
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

extern void YsMacOSXGetFileListCallBack(void *objPointer,const char fn[],int isDirectory,unsigned long long fileSize);

int YsMacOSXGetFileList(void *objPointer,const char dir[],int errCode,int okCode)
{
	NSAutoreleasePool *pool=[[NSAutoreleasePool alloc] init];
	NSFileManager *fileManager=[[NSFileManager alloc] init];
	NSString *atPath=[[NSString alloc] initWithUTF8String:dir];
	NSArray *fileList=[fileManager contentsOfDirectoryAtPath:atPath error:nil];

	int ret=okCode;

	if(nil!=fileList)
	{
		int i;
		for(i=0; i<[fileList count]; i++)
		{
			NSString *str=[fileList objectAtIndex:i];

			NSString *fulPath=[atPath stringByAppendingPathComponent:str];
			BOOL isDir;
			[fileManager fileExistsAtPath:fulPath isDirectory:&isDir];

			NSDictionary *attrib=[fileManager attributesOfItemAtPath:fulPath error:nil];
			unsigned long long fileSize=(TRUE==isDir ? 0 : [attrib fileSize]);

			YsMacOSXGetFileListCallBack(objPointer,[str UTF8String],(TRUE==isDir ? 1 : 0),fileSize);
		}
	}
	else
	{
		ret=errCode;
	}

	[atPath release];
	[fileManager release];
	[pool release];

	return ret;
}


