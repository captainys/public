/* ////////////////////////////////////////////////////////////

File Name: ysdef.h
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

#ifndef YSDEF_IS_INCLUDED
#define YSDEF_IS_INCLUDED
/* { */

/// \cond
#include <string.h>
/// \endcond

/*! \file */

#ifndef YSRESULT_IS_DEFINED
#define YSRESULT_IS_DEFINED
/*! Enum for processing result. */
typedef enum
{
	YSERR,  /*!< There were error(s). */
	YSOK    /*!< The process was successful. */
} YSRESULT;
#endif

#ifndef YSBOOL_IS_DEFINED
#define YSBOOL_IS_DEFINED
/*! Enum for boolearn. */
typedef enum
{
	YSFALSE,     /*!< False */
	YSTRUE,      /*!< True */
	YSTFUNKNOWN  /*!< Unable to tell true or false. */
} YSBOOL;

inline YSBOOL YsOr(YSBOOL a,YSBOOL b)
{
	if(YSTRUE==a || YSTRUE==b)
	{
		return YSTRUE;
	}
	return YSFALSE;
}
inline YSBOOL YsAnd(YSBOOL a,YSBOOL b)
{
	if(YSTRUE==a && YSTRUE==b)
	{
		return YSTRUE;
	}
	return YSFALSE;
}
inline YSBOOL YsXor(YSBOOL a,YSBOOL b)
{
	if(a!=b && (a==YSTRUE || b==YSTRUE))
	{
		return YSTRUE;
	}
	return YSFALSE;
}

#endif


// Tested with:
//   GCC 32bit
//   GCC 64bit
//   VC++ 64bit
//   VC++ 32bit
#if defined(__GNUC__) && __SIZEOF_POINTER__==4
	typedef int YSSIZE_T;
#elif defined(__GNUC__) && __SIZEOF_POINTER__==8
	#include <inttypes.h>
	typedef int64_t YSSIZE_T;
#elif defined(_WIN64)
	typedef __int64 YSSIZE_T;
#elif defined(_WIN32)
	typedef int YSSIZE_T;
#else
	// Could not identify bitness.  YSSIZE_T falls back to int."
	typedef int YSSIZE_T;
#endif


typedef unsigned int YSHASHKEY;
const YSHASHKEY YSNULLHASHKEY=0x7fffffff;


#if defined(_WIN32) || defined(_WIN64)
	#ifndef _CRT_SECURE_NO_WARNINGS
	// This disables annoying warnings VC++ gives for use of C standard library.  Shut the mouth up.
	#define _CRT_SECURE_NO_WARNINGS
	#endif
#endif


/*! Enum for coordinate system model */
typedef enum
{
	YSLEFT_ZPLUS_YPLUS,  /*!< Left-hand coordinate system.  Z+ is forward, Y+ is upward. */
	YSRIGHT_ZMINUS_YPLUS /*!< Right-hand coordinate system.  Z- is forward, Y+ is upward. Same as OpenGL's coordinate system (Default) */
} YSCOORDSYSMODEL;

/*! Enum for inside, outside, or on the boundary of a geometric domain */
typedef enum
{
	YSINSIDE,      /*!< Inside */
	YSOUTSIDE,     /*!< Outside */
	YSBOUNDARY,    /*!< On the boundary */
	YSUNKNOWNSIDE  /*!< Cannot tell due to an error. */
} YSSIDE;

/*! Enum for intersection cases */
typedef enum
{
	YSINTERSECT,   /*!< Clear intersection */
	YSOVERLAP,     /*!< Two entities are overlapping */
	YSTOUCH,       /*!< Two entities are touching, but not intersecting */
	YSSHAREPOINT,  /*!< Two entities are only sharing a point */
	YSAPART        /*!< Two entities are apart */
} YSINTERSECTION;

/*! Enum for boolearn operation */
typedef enum
{
	YSBOOLBLEND,    /*!< Blend two geometric models */
	YSBOOLAND,      /*!< Take intersection (AND) */
	YSBOOLOR,       /*!< Take summation (OR) */
	YSBOOLMINUS,    /*!< Take subtraction */
	YSBOOLSEPARATE, /*!< Take intersection and subtraction.  Available after YsBlendShell3 */
	YSBOOL_OPENMINUSSOLID, /*!< Open surface minus solid. */ 
	YSBOOL_OPENTIMESSOLID, /*!< Open surface AND solid. */ 
	YSBOOL_OPENPLUSSOLID, /*!< Open surface plus solid. */ 
} YSBOOLEANOPERATION;

/*! Enum for polygon-subdivision strategy */
typedef enum
{
	YSCONVEXNIZEDEFAULT,      /*!< Default strategy */
	YSCONVEXNIZENOSMALLANGLE, /*!< Avoid small angles */
	YSCONVEXNIZE90DEGREE,     /*!< Make angles as close to 90 degrees as possible */
	YSCONVEXNIZEFASTEST,      /*!< Fastest */
	YSCONVEXNIZEBALANCEDIST   /*!< Balanced distance */
} YSCONVEXNIZESTRATEGY;

/*! Enum for flip direction. */
typedef enum
{
	YSFLIPCCW,      /*!< Counter clockwise */
	YSFLIPCW,       /*!< Clockwise */
	YSFLIPUNKNOWN   /*!< Unknown orientation */
} YSFLIPDIRECTION;

/*! Enum for End-Of-Line type. */
enum YSEOLTYPE
{
	YSEOLTYPE_UNKNOWN,
	YSEOLTYPE_DEFAULT,
	YSEOLTYPE_CR,
	YSEOLTYPE_LF,
	YSEOLTYPE_CRLF
};

////////////////////////////////////////////////////////////

/*! This function returns "TRUE", "FALSE", or "UNKNOWNBOOL" according to the input parameter
    \param boo [In] Input boolearn value.
*/
inline const char *YsBoolToStr(YSBOOL boo)
{
	switch(boo)
	{
	case YSTRUE:
		return "TRUE";
		break;
	case YSFALSE:
		return "FALSE";
	default:
	case YSTFUNKNOWN:
		return "UNKNOWNBOOL";
	}
}

/*! This function returns YSTRUE, YSFALSE, or YSTFUNKNOWN according to the input string.
    It is a reverse of YsBoolToStr.
    \param str [In] A string.  "TRUE", "FALSE", or "UNKNOWNBOOL".
*/
inline YSBOOL YsStrToBool(const char str[])
{
	if(0==strcmp(str,"TRUE") || 0==strcmp(str,"true") || 0==strcmp(str,"True"))
	{
		return YSTRUE;
	}
	else if(0==strcmp(str,"FALSE") || 0==strcmp(str,"false") || 0==strcmp(str,"False"))
	{
		return YSFALSE;
	}
	return YSTFUNKNOWN;
}

/*! This function returns "OK", "ERR", or "UNKNOWNRESULT" according to the input parameter.
    \param res [In] Input result
*/
inline const char *YsResultToStr(YSRESULT res)
{
	switch(res)
	{
	case YSOK:
		return "OK";
	case YSERR:
		return "ERR";
	default:
		return "UNKNOWNRESULT";
	}
}

/*! This function returns YSOK or YSERR according to the input string.
    It is a reverse of YsResultToStr.
    \param str [In] Input string.  "OK" or "ERR"
*/
inline YSRESULT YsStrToResult(const char str[])
{
	if(0==strcmp(str,"OK") || 0==strcmp(str,"ok"))
	{
		return YSOK;
	}
	else
	{
		return YSERR;
	}
}

/*! This function returns "INSIDE", "OUTSIDE", "BOUNDARY", or "UNKNOWNSIDE" according
    to the input parameter.
    \param side [In] Input side
*/
inline const char *YsSideToStr(YSSIDE side)
{
	switch(side)
	{
	case YSINSIDE:
		return "INSIDE";
	case YSOUTSIDE:
		return "OUTSIDE";
	case YSBOUNDARY:
		return "BOUNDARY";
	default:
		return "UNKNOWNSIDE";
	}
}

inline const char *YsIntersectionToStr(YSINTERSECTION itsc)
{
	switch(itsc)
	{
	case YSINTERSECT:
		return "INTERSECT";
	case YSOVERLAP:
		return "OVERLAP";
	case YSTOUCH:
		return "TOUCH";
	case YSSHAREPOINT:
		return "SHAREPOINT";
	case YSAPART:
		return "APART";
	}
	return "UNKNOWN";
}

////////////////////////////////////////////////////////////

/*! This function inverts a boolean value.
    \param boo [In/Out] A boolean to be inverted.
*/
inline void YsFlip(YSBOOL &boo)
{
	boo=(boo==YSTRUE ? YSFALSE : YSTRUE);
}

/*! This function returns YSTRUE if the input parameter is YSFALSE, or YSFALSE otherwise.
*/
inline YSBOOL YsReverseBool(YSBOOL boo)
{
	return (YSFALSE==boo ? YSTRUE : YSFALSE);
}

/*! This function swaps two objects of any data type that has a safe copy operator defined.
*/
template <class T>
inline void YsSwapSomething(T &a,T &b)
{
	T c=(T &&)a;
	a=(T &&)b;
	b=(T &&)c;
}

/// \cond
inline void YsSwapDouble(double &a,double &b)
{
	YsSwapSomething <double> (a,b);
}

inline void YsSwapInt(int &a,int &b)
{
	YsSwapSomething <int> (a,b);
}

inline void YsSwapUnsignedInt(unsigned &a,unsigned &b)
{
	YsSwapSomething <unsigned> (a,b);
}

inline void YsSwapBool(YSBOOL &a,YSBOOL &b)
{
	YsSwapSomething <YSBOOL> (a,b);
}

template <class T>
inline int YsCompareTwoConstant(const T a,const T b)
{
	return a-b;
}
/// \endcond

/* } */
#endif
