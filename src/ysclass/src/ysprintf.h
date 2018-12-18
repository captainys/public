/* ////////////////////////////////////////////////////////////

File Name: ysprintf.h
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

#ifndef YSPRINTF_IS_INCLUDED
#define YSPRINTF_IS_INCLUDED
/* { */

#include "ysdef.h"

#if !defined(__GNUC__) && !defined(__clang__)
#define __attribute__(x)
#endif

// Declaration /////////////////////////////////////////////
class YsPrintf
{
protected:
	virtual void Output(char str[]);
public:
	YsPrintf();
	YsPrintf(const char *fom,...) __attribute__ ((format (printf,2,3)));
	YsPrintf(int a,const char *fom,...) __attribute__ ((format (printf,3,4)));

	static YsPrintf *def;
	static YSBOOL masterSwitch;
	YSBOOL enabled;

	int Printf(const char *fom,...) __attribute__ ((format (printf,2,3)));

	void SetDefault(void);
	void ResetDefault(void);

	static YsPrintf *GetDefault(void);
};

////////////////////////////////////////////////////////////

void YsTurnOnErrorOutput(void);
void YsTurnOffErrorOutput(void);
int YsErrOut(const char *fmt,...) __attribute__ ((format (printf,1,2)));

void YsTurnOnDiagnosticOutput(void);
void YsTurnOffDiagnosticOutput(void);
int YsDiagOut(const char *fmt,...) __attribute__ ((format (printf,1,2)));

////////////////////////////////////////////////////////////

class YsLoopCounter
{
public:
	YsLoopCounter();
	void Begin(YSSIZE_T length);
	void Show(YSSIZE_T ctr);
	void Increment(void);
	void End(void);

	void SetOutput(YsPrintf *print);

	YSSIZE_T GetCount(void) const;

	static unsigned int defUpdateInterval;
	static char defNewLine;
	static YSBOOL globallyHideLoopCounter;

	/*! Old naming convention use Begin instead. */
	void BeginCounter(YSSIZE_T length);
	/*! Old naming convention use Show instead. */
	void ShowCounter(YSSIZE_T ctr);
	/*! Old naming convention use End instead. */
	void EndCounter(void);

protected:
	unsigned long long lastShowedTime;
	unsigned long long updateInterval;
	YSSIZE_T length;
	YSSIZE_T current;
	YsPrintf *output;
};

/* } */
#endif
