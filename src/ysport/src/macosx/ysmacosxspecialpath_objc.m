/* ////////////////////////////////////////////////////////////

File Name: ysmacosxspecialpath_objc.m
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
#import <mach-o/dyld.h>

bool YsSpecialPath_GetProgramFileName(size_t lBuf,char buf[])
{
	if(nil!=[NSBundle mainBundle])
	{
	 	NSAutoreleasePool *pool=[[NSAutoreleasePool alloc] init];
		strncpy(buf,[[[NSBundle mainBundle] executablePath] UTF8String],lBuf);
		[pool release];
		buf[lBuf-1]=0;
	}
	else
	{
		_NSGetExecutablePath(buf,lBuf);
	}
	return buf;
}

bool YsSpecialPath_GetProgramBaseDir(size_t lBuf,char buf[])
{
 	NSAutoreleasePool *pool=[[NSAutoreleasePool alloc] init];
	strncpy(buf,[[[NSBundle mainBundle] resourcePath] UTF8String],lBuf); // 2015/11/30 Changed from bundlePath to resourcePath
	[pool release];
	buf[lBuf-1]=0;
	return buf;
}

bool YsSpecialPath_GetUserDir(size_t lBuf,char buf[])
{
 	NSAutoreleasePool *pool=[[NSAutoreleasePool alloc] init];
	strncpy(buf,[NSHomeDirectory() UTF8String],lBuf);
	[pool release];

	buf[lBuf-1]=0;
	return true;
}

bool YsSpecialPath_GetAllUsersDir(size_t lBuf,char buf[])
{
	return false;
}

