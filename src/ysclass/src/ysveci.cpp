/* ////////////////////////////////////////////////////////////

File Name: ysveci.cpp
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

#if (defined(_WIN32) || defined(_WIN64)) && !defined(_CRT_SECURE_NO_WARNINGS)
	// This disables annoying warnings VC++ gives for use of C standard library.  Shut the mouth up.
	#define _CRT_SECURE_NO_WARNINGS
#endif

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "ysarray.h"
#include "ysveci.h"

YsVec2i::YsVec2i(const char *x,const char *y)
{
	dat[0]=atoi(x);
	dat[1]=atoi(y);
}

const char *YsVec2i::Text(char buf[],const char *fmt) const
{
	sprintf(buf,fmt,x(),y());
	return buf;
}

double YsVec2i::GetLength(void) const
{
	long long int sq=dat[0]*dat[0]+dat[1]*dat[1];
	return sqrt((double)sq);
}

////////////////////////////////////////////////////////////

YsVec3i::YsVec3i(const char *x,const char *y,const char *z)
{
	dat[0]=atoi(x);
	dat[1]=atoi(y);
	dat[2]=atoi(z);
}

const char *YsVec3i::Text(char buf[],const char *fmt) const
{
	sprintf(buf,fmt,x(),y(),z());
	return buf;
}

double YsVec3i::GetLength(void) const
{
	long long int sq=dat[0]*dat[0]+dat[1]*dat[1]+dat[2]*dat[2];
	return sqrt((double)sq);
}
