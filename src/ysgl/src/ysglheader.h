/* ////////////////////////////////////////////////////////////

File Name: ysglheader.h
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

#ifndef YSGLHEADER_IS_INCLUDED
#define YSGLHEADER_IS_INCLUDED
/* { */

/*! \file 
	Include this header to use OpenGL or OpenGL ES functions.

	The location of OpenGL headers are total mess.  In good old days, all I needed to include were:
	
	    #include <GL/gl.h>
	    #include <GL/glu.h>
	
	Visual C++, unfortunately, required windows.h to be included before <GL/gl.h>.  

	XCode somehow put them in <OpenGL/gl.h> and <OpenGL/glu.h>

	For higher-version OpenGL, glext.h was required, but Visual C++ didn't come with it.

	OpenGL ES headers can be <GLES2/gl2.h> and <GLES2/gl2ext.h> or <OpenGLES/ES2/gl.h> and <OpenGLES/ES2/glext.h>

	Give me a break.  Why can't they just standardize?
 */

/* Force Visual C++ to type-mismatching error. */
#pragma warning( error : 4028)
#pragma warning( error : 4047)

#if defined(_MSC_VER) && 1900<=_MSC_VER
	#include "winapifamily.h"
#endif

// Macro YS_IS_UNIVERSAL_WINDOWS_APP must be defined manually for Universal Windows Platform.
// Looking for an automated way.
// 2017/04/03 (defined(WINAPI_FAMILY) && WINAPI_FAMILY==WINAPI_FAMILY_APP) seems to be true for WindowsStore app, and
//            (!defined(WINAPI_FAMILY) || WINAPI_FAMILY==WINAPI_FAMILY_DESKTOP_APP) seems to be true for Win32 app.

#if defined(_WIN32) && (!defined(WINAPI_FAMILY) || WINAPI_FAMILY==WINAPI_FAMILY_DESKTOP_APP)
	#ifndef WIN32_LEAN_AND_MEAN
		// Prevent inclusion of winsock.h
		#define WIN32_LEAN_AND_MEAN
		#include <windows.h>
		#undef WIN32_LEAN_AND_MEAN
	#else
		// Too late.  Just include it.
		#include <windows.h>
	#endif
#endif


#ifndef GL_GLEXT_PROTOTYPES
	#define GL_GLEXT_PROTOTYPES
#endif


// I didn't find a convenient macro to identify iOS.  Please add a macro __APPLE_IOS__ to the preprocessor macro option.

#if ((defined(WINAPI_FAMILY) && WINAPI_FAMILY==WINAPI_FAMILY_APP)) || defined(__ANDROID__)
	#include <GLES2/gl2.h>
	#include <GLES2/gl2ext.h>
	#define YS_GL_ES2
	typedef double GLdouble;
#elif !defined(__APPLE__)
	#include <GL/gl.h>
	#include <GL/glu.h>
	#include <GL/glext.h>
#else
    #include <TargetConditionals.h>
    #if TARGET_OS_EMBEDDED!=0 || TARGET_OS_IPHONE!=0 || TARGET_OS_IPHONE_SIMULATOR!=0
		#include <OpenGLES/ES2/gl.h>
		#include <OpenGLES/ES2/glext.h>
		typedef double GLdouble;
		#define YS_GL_ES2
	#else
		#include <OpenGL/gl.h>
		#include <OpenGL/glu.h>
		#include <OpenGL/glext.h>
	#endif
#endif

#if !defined(GL_PROGRAM_POINT_SIZE) && defined(GL_PROGRAM_POINT_SIZE_EXT)
#define GL_PROGRAM_POINT_SIZE GL_PROGRAM_POINT_SIZE_EXT
#endif

#if !defined(GL_CLAMP) && defined(GL_CLAMP_TO_EDGE)
#define GL_CLAMP GL_CLAMP_TO_EDGE
#endif


#if defined(_WIN32) && (!defined(WINAPI_FAMILY) || WINAPI_FAMILY==WINAPI_FAMILY_DESKTOP_APP)
	#include "ysglwindows.h"
	// Hey Microsoft, can you concede and add support for higher versions of OpenGL in Visual C++ ?  Thank you!  Nobody wants Direct 3D b**ls**t!
	//   |
	//   V
	// Great!  Microsoft supports OpenGL ES 2.0 at least for Universal Windows Platform!  Thank you!
#endif

/* } */
#endif
