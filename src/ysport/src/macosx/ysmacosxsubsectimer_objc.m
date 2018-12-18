/* ////////////////////////////////////////////////////////////

File Name: ysmacosxsubsectimer_objc.m
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

double YsSubSecTimer_GetSecond(void)
{
 	NSAutoreleasePool *pool=[[NSAutoreleasePool alloc] init];

	NSTimeInterval curTime=[NSDate timeIntervalSinceReferenceDate]; // Reference Date is Jan 1 2001
	const double curTimeD=(double)curTime;

	[pool release];

	return curTimeD;
}

void YsSubSecTimer_SleepMilliSecond(unsigned int ms)
{
	if(ms>0)
	{
		double sec;
		sec=(double)ms/1000.0;
		[NSThread sleepForTimeInterval:sec];
	}
}

void YsSubSecTimer_SleepSecond(const double sec)
{
	if(0.0<sec)
	{
		[NSThread sleepForTimeInterval:sec];
	}
}

