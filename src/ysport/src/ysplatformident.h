/* ////////////////////////////////////////////////////////////

File Name: ysplatformident.h
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

#ifndef YSPLATFORMIDENT_IS_INCLUDED
#define YSPLATFORMIDENT_IS_INCLUDED
/* { */

enum YS_PLATFORM_IDENT
{
	YS_PLATFORM_IDENT_UNKNOWN,
	YS_PLATFORM_IDENT_WINDOWS,
	YS_PLATFORM_IDENT_MACOSX,
	YS_PLATFORM_IDENT_IOS,
	YS_PLATFORM_IDENT_LINUX
};



#ifdef _WIN32
	#define YS_PLATFORM_IS_WINDOWS
#elif defined(__linux) || defined(linux) || defined(LINUX)
	#define YS_PLATFORM_IS_LINUX
#elif defined(__APPLE__)
    #include <TargetConditionals.h>
    #if TARGET_OS_EMBEDDED!=0 || TARGET_OS_IPHONE!=0 || TARGET_OS_IPHONE_SIMULATOR!=0
		#define YS_PLATFORM_IS_IOS
	#else
		#define YS_PLATFORM_IS_MACOSX
	#endif
#endif



inline YS_PLATFORM_IDENT YsIdentifyPlatform(void)
{
	#if   defined(YS_PLATFORM_IS_MACOSX)
		return YS_PLATFORM_IDENT_MACOSX;
	#elif defined(YS_PLATFORM_IS_IOS)
		return YS_PLATFORM_IDENT_IOS;
	#elif defined(YS_PLATFORM_IS_WINDOWS)
		return YS_PLATFORM_IDENT_WINDOWS;
	#elif defined(YS_PLATFORM_IS_LINUX)
		return YS_PLATFORM_IDENT_LINUX;
	#else
		return YS_PLATFORM_IDENT_UNKNOWN;
	#endif
}



/* } */
#endif
