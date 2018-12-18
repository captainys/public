/* ////////////////////////////////////////////////////////////

File Name: ysprintf.cpp
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
#include <stdarg.h>

#include "ysprintf.h"


YSBOOL YsLoopCounter::globallyHideLoopCounter=YSFALSE;


// Implementation //////////////////////////////////////////
YsPrintf::YsPrintf()
{
	enabled=YSTRUE;
}

YsPrintf::YsPrintf(const char *fmt,...)
{
	if(def!=NULL && def->enabled!=YSTRUE)
	{
		return;
	}
	if(masterSwitch!=YSTRUE)
	{
		return;
	}

	int a;
	char buf[4096];
	va_list arg_ptr;

	va_start(arg_ptr,fmt);
	a=vsprintf(buf,fmt,arg_ptr);
	va_end(arg_ptr);

	if(def!=NULL)
	{
		def->Output(buf);
	}
	else
	{
		printf("%s",buf);
	}
}

YsPrintf::YsPrintf(int,const char *fmt,...)
{
	int a;
	char buf[4096];
	va_list arg_ptr;

	va_start(arg_ptr,fmt);
	a=vsprintf(buf,fmt,arg_ptr);
	va_end(arg_ptr);

	if(def!=NULL)
	{
		def->Output(buf);
	}
	else
	{
		printf("%s",buf);
	}
}

void YsPrintf::Output(char str[])
{
	printf("%s",str);
}

int YsPrintf::Printf(const char *fmt,...)
{
	if(this!=NULL && enabled!=YSTRUE)
	{
		return 0;
	}
	if(masterSwitch!=YSTRUE)
	{
		return 0;
	}

	int a;
	char buf[4096];
	va_list arg_ptr;

	va_start(arg_ptr,fmt);
	a=vsprintf(buf,fmt,arg_ptr);
	va_end(arg_ptr);

	if(this!=NULL)
	{
		Output(buf);
	}
	else
	{
		printf("%s",buf);
	}
	return a;
}

YsPrintf *YsPrintf::def=NULL;
YSBOOL YsPrintf::masterSwitch=YSTRUE;

void YsPrintf::SetDefault(void)
{
	def=this;
}

void YsPrintf::ResetDefault(void)
{
	def=NULL;
}

YsPrintf *YsPrintf::GetDefault(void)
{
	return def;
}

////////////////////////////////////////////////////////////

static int YsClassOutputError=1;

void YsTurnOnErrorOutput(void)
{
	YsClassOutputError=1;
}

void YsTurnOffErrorOutput(void)
{
	YsClassOutputError=0;
}

int YsErrOut(const char *fmt,...)
{
	if(YsClassOutputError==1)
	{
		int a;
		char buf[4096];
		va_list arg_ptr;

		va_start(arg_ptr,fmt);
		a=vsprintf(buf,fmt,arg_ptr);
		va_end(arg_ptr);

		printf("%s",buf);

		return a;
	}
	return 0;
}


////////////////////////////////////////////////////////////

static int YsDiagnosticOutput=0;

void YsTurnOnDiagnosticOutput(void)
{
	YsDiagnosticOutput=1;
}

void YsTurnOffDiagnosticOutput(void)
{
	YsDiagnosticOutput=0;
}

int YsDiagOut(const char *fmt,...)
{
	if(YsDiagnosticOutput==1)
	{
		int a;
		char buf[4096];
		va_list arg_ptr;

		va_start(arg_ptr,fmt);
		a=vsprintf(buf,fmt,arg_ptr);
		va_end(arg_ptr);

		printf("%s",buf);

		return a;
	}
	return 0;
}

////////////////////////////////////////////////////////////

#include <time.h>

// #ifdef _WIN32
// #include <windows.h>
// HWND YsLoopCounter_OutputHWnd=NULL;
// #endif

unsigned int YsLoopCounter::defUpdateInterval=500;  // 500ms
char YsLoopCounter::defNewLine='\n';  // Mac OSX terminal doesn't update with \r. 2016/03/19

YsLoopCounter::YsLoopCounter()
{
	output=NULL;
	lastShowedTime=0;
	length=0;
	current=0;
	updateInterval=defUpdateInterval;
	output=YsPrintf::def;
}

void YsLoopCounter::Begin(YSSIZE_T l)
{
	length=l;
	lastShowedTime=time(nullptr);
	current=0;
}

void YsLoopCounter::Show(YSSIZE_T ctr)
{
	unsigned long long t=time(nullptr);
	current=ctr;

	if(YSTRUE==globallyHideLoopCounter)
	{
		return;
	}

	if(t<lastShowedTime)   // Clock Underflow
	{
		lastShowedTime=t;
		return;
	}
	else if(updateInterval<((t-lastShowedTime)*1000))
	{
// #ifdef _WIN32
// 		if(YsLoopCounter_OutputHWnd!=NULL)
// 		{
// 			char str[256];
// 			if(length!=0)
// 			{
// 				sprintf(str,"%d/%d",ctr,length);
// 			}
// 			else
// 			{
// 				sprintf(str,"%d",ctr);
// 			}
// 			SetWindowText(YsLoopCounter_OutputHWnd,str);
// 		}
// 		else
// #endif

		if(length!=0)
		{
			output->Printf("%d/%d%c",(int)ctr,(int)length,defNewLine);
		}
		else
		{
			output->Printf("%d%c",(int)ctr,defNewLine);
		}
		lastShowedTime=t;
	}
}

void YsLoopCounter::Increment(void)
{
	ShowCounter(current+1);
}

void YsLoopCounter::End(void)
{
// #ifdef _WIN32
// 		if(YsLoopCounter_OutputHWnd!=NULL)
// 		{
// 			SetWindowText(YsLoopCounter_OutputHWnd,"");
// 		}
// 		else
// #endif
	if(length!=0)
	{
		output->Printf("%d/%d\n",(int)length,(int)length);
		output->Printf("Done!\n");
	}
	else
	{
		output->Printf("Done!\n");
	}
}

void YsLoopCounter::SetOutput(YsPrintf *print)
{
	output=print;
}

YSSIZE_T YsLoopCounter::GetCount(void) const
{
	return current;
}


void YsLoopCounter::BeginCounter(YSSIZE_T length)
{
	Begin(length);
}
void YsLoopCounter::ShowCounter(YSSIZE_T ctr)
{
	Show(ctr);
}
void YsLoopCounter::EndCounter(void)
{
	End();
}
