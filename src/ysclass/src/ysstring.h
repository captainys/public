/* ////////////////////////////////////////////////////////////

File Name: ysstring.h
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

#ifndef YSSTRING_IS_INCLUDED
#define YSSTRING_IS_INCLUDED
/* { */

/*! \file */

#include <ctype.h>
#include <string.h>
#include <stdio.h>

#include "ysdef.h"
#include "ysarray.h"

/*! Moves *ptr to the first character that is not space or tab.
    \param ptr [In/Out] String */
void YsStringHead(char **ptr);

/*! Deletes spaces, tabs, and new-line characters at the end of the string. 
    \param str [In/Out] String
    */
void YsStringTail(char *str);

/*! Turns small letters into capital letters. */
void YsCapitalize(char str[]);

/*! Turns capital letters into small letters. */
void YsUncapitalize(char str[]);

/*! Makes a string of double-precision floating point.  It drops all zeros after non-zero digit below decimal. */
const char *YsOptimizeDoubleString(char buf[],const double &value);

// For parsing CSV, blank="", comma=","
void YsArguments2(YsArray <class YsString,16> &args,const char str[],const char *blank=" \t",const char *comma=",");
void YsArguments2(YsArray <class YsString,16> &args,const char str[],YSSIZE_T l,const char *blank=" \t",const char *comma=",");

// Reference: http://en.wikipedia.org/wiki/UTF-8
/*! This function encodes UTF8 string from a wide-character (UNICODE) string.
    Wide-character type does not have to be wchar_t.  It can be any integral-type array
    that represents a sequence of Unicode characters followed by zero (terminator).

	\param lOut [In]  Length of the output buffer
	\param out  [Out] Output buffer
	\param in   [In]  Input wide-character string.
    */
template <class WCHAR_TYPE>
size_t YsWCharToUTF8(size_t lOut,char out[],WCHAR_TYPE in[])
{
	unsigned int iPtr=0,oPtr=0;
	unsigned char *uOut=(unsigned char *)out;

	while(oPtr<lOut-1 && 0!=in[iPtr])
	{
		if(in[iPtr]<=0x7f)
		{
			uOut[oPtr]=(unsigned char)in[iPtr];
			oPtr++;
		}
		else if(in[iPtr]<=0x7ff)
		{
			if(lOut-2<=oPtr)
			{
				// Buffer overrun.  Cannot put zero at the end.
				break;
			}
			unsigned int low6=in[iPtr]&0x3f;
			unsigned int high5=(in[iPtr]>>6)&0x1f;

			uOut[oPtr++]=(unsigned char)(0xc0|high5);
			uOut[oPtr++]=(unsigned char)(0x80|low6);
		}
		else if(in[iPtr]<=0xffff)
		{
			if(lOut-3<=oPtr)
			{
				// Buffer overrun.  Cannot put zero at the end.
				break;
			}

			unsigned int low6=in[iPtr]&0x3f;
			unsigned int mid6=(in[iPtr]>>6)&0x3f;
			unsigned int high4=(in[iPtr]>>12)&0x0f;

			uOut[oPtr++]=(unsigned char)(0xe0|high4);
			uOut[oPtr++]=(unsigned char)(0x80|mid6);
			uOut[oPtr++]=(unsigned char)(0x80|low6);
		}
		else // The following block will never be used unless wchar_t is greater than 16-bit.
		{
			unsigned long inChar=in[iPtr];
			unsigned int bit00to05=inChar&0x3f;
			unsigned int bit06to11=(inChar>>6)&0x3f;
			unsigned int bit12to17=(inChar>>12)&0x3f;
			unsigned int bit18to23=(inChar>>18)&0x3f;
			unsigned int bit24to29=(inChar>>24)&0x3f;
			unsigned int bit30=(inChar>>30)&0x01;

			if(in[iPtr]<=0x1fffff)
			{
				if(lOut-4<=oPtr)
				{
					// Buffer overrun.  Cannot put zero at the end.
					break;
				}
				uOut[oPtr++]=(unsigned char)(0xf0|(bit18to23&0x07));
				uOut[oPtr++]=(unsigned char)(0x80|bit12to17);
				uOut[oPtr++]=(unsigned char)(0x80|bit06to11);
				uOut[oPtr++]=(unsigned char)(0x80|bit00to05);
			}
			else if(in[iPtr]<=0x3ffffff)
			{
				if(lOut-5<=oPtr)
				{
					// Buffer overrun.  Cannot put zero at the end.
					break;
				}
				uOut[oPtr++]=(unsigned char)(0xf8|(bit24to29&0x03));
				uOut[oPtr++]=(unsigned char)(0x80|bit18to23);
				uOut[oPtr++]=(unsigned char)(0x80|bit12to17);
				uOut[oPtr++]=(unsigned char)(0x80|bit06to11);
				uOut[oPtr++]=(unsigned char)(0x80|bit00to05);
			}
			else if(in[iPtr]<=0x7fffffff)
			{
				if(lOut-6<=oPtr)
				{
					// Buffer overrun.  Cannot put zero at the end.
					break;
				}
				uOut[oPtr++]=(unsigned char)(0xfc|bit30);
				uOut[oPtr++]=(unsigned char)(0x80|bit24to29);
				uOut[oPtr++]=(unsigned char)(0x80|bit18to23);
				uOut[oPtr++]=(unsigned char)(0x80|bit12to17);
				uOut[oPtr++]=(unsigned char)(0x80|bit06to11);
				uOut[oPtr++]=(unsigned char)(0x80|bit00to05);
			}
		}
		iPtr++;
	}

	if(oPtr<lOut)
	{
		out[oPtr]=0;
		oPtr++;
	}

	return oPtr;
}

/*! This function decodes UTF8-character string into wide-character (UNICODE) string.
    Wide-character string does not have to be of wchar_t.  It can be any integral-type array.
    \tparam WCHAR_TYPE Type of the wide character.  It typically is wchar_t.
    \param lOut [In]  Length of the output buffer
    \param out  [Out] Output buffer
    \param in   [In]  Input UTF8 string
    */
template <class WCHAR_TYPE>
size_t YsUTF8ToWChar(size_t lOut,WCHAR_TYPE out[],const char in[])
{
	unsigned int iPtr=0,oPtr=0;
	unsigned char *uIn=(unsigned char *)in;

	while(oPtr<lOut-1 && 0!=uIn[iPtr])
	{
		if(0==(uIn[iPtr]&0x80))
		{
			out[oPtr]=uIn[iPtr];
			iPtr++;
		}
		else if(0xc0==(uIn[iPtr]&0xe0))
		{
			if(0==uIn[iPtr+1])
			{
				// Pre-mature termination.
				break;
			}
			unsigned int high5=uIn[iPtr]&0x1f;
			unsigned int low6=uIn[iPtr+1]&0x3f;
			out[oPtr]=(WCHAR_TYPE)((high5<<6)|low6);
			iPtr+=2;
		}
		else if(0xe0==(uIn[iPtr]&0xf0))
		{
			if(0==uIn[iPtr+1] || 0==uIn[iPtr+2])
			{
				// Pre-mature termination.
				break;
			}
			unsigned int high4=uIn[iPtr]&0x0f;
			unsigned int mid6=uIn[iPtr+1]&0x3f;
			unsigned int low6=uIn[iPtr+2]&0x3f;
			out[oPtr]=(WCHAR_TYPE)((high4<<12)|(mid6<<6)|low6);
			iPtr+=3;
		}
		else if(0xf0==(uIn[iPtr]&0xf8))
		{
			if(0==uIn[iPtr+1] || 0==uIn[iPtr+2] || 0==uIn[iPtr+3])
			{
				// Pre-mature termination.
				break;
			}
			unsigned int bit18to20=uIn[iPtr]&0x07;
			unsigned int bit12to17=uIn[iPtr+1]&0x3f;
			unsigned int bit06to11=uIn[iPtr+2]&0x3f;
			unsigned int bit00to05=uIn[iPtr+3]&0x3f;
			out[oPtr]=(WCHAR_TYPE)((bit18to20<<18)|(bit12to17<<12)|(bit06to11<<6)|bit00to05);
			iPtr+=4;
		}
		else if(0xf8==(uIn[iPtr]&0xfc))
		{
			if(0==uIn[iPtr+1] || 0==uIn[iPtr+2] || 0==uIn[iPtr+3] || 0==uIn[iPtr+4])
			{
				// Pre-mature termination.
				break;
			}
			unsigned int bit24to25=uIn[iPtr]&0x03;
			unsigned int bit18to23=uIn[iPtr+1]&0x3f;
			unsigned int bit12to17=uIn[iPtr+2]&0x3f;
			unsigned int bit06to11=uIn[iPtr+3]&0x3f;
			unsigned int bit00to05=uIn[iPtr+4]&0x3f;
			out[oPtr]=(WCHAR_TYPE)((bit24to25<<24)|(bit18to23<<18)|(bit12to17<<12)|(bit06to11<<6)|bit00to05);
			iPtr+=5;
		}
		else if(0xfc==(uIn[iPtr]&0xfe))
		{
			if(0==uIn[iPtr+1] || 0==uIn[iPtr+2] || 0==uIn[iPtr+3] || 0==uIn[iPtr+4] || 0==uIn[iPtr+5])
			{
				// Pre-mature termination.
				break;
			}
			unsigned int bit30=uIn[iPtr]&0x01;
			unsigned int bit24to29=uIn[iPtr+1]&0x3f;
			unsigned int bit18to23=uIn[iPtr+2]&0x3f;
			unsigned int bit12to17=uIn[iPtr+3]&0x3f;
			unsigned int bit06to11=uIn[iPtr+4]&0x3f;
			unsigned int bit00to05=uIn[iPtr+5]&0x3f;
			out[oPtr]=(WCHAR_TYPE)((bit30<<30)|(bit24to29<<24)|(bit18to23<<18)|(bit12to17<<12)|(bit06to11<<6)|bit00to05);
			iPtr+=6;
		}
		else
		{
			// Invalid utf8 string
			break;
		}
		oPtr++;
	}

	if(oPtr<lOut)
	{
		out[oPtr]=0;
		oPtr++;
	}

	return oPtr;
}


////////////////////////////////////////////////////////////

/*! Template class for a text string.  It is in particular used as a base class of
    \sa YsString and \sa YsWString

    This class has some common functions with std::string so that this can be used in place for std::string.
    However, std::string allows substition of '\0' through operator[], and non-const access functions.
    Since this GenericString class caches the length of the string, copying '\0' outside of the 
    modifier functions creates inconsistency between the cached length and actual length.
    Therefore, this GenericString class does not allow such modifications through operator [] and 
    non-const access functions.

	\tparam CHARTYPE An integral type that represents a character.
    */
template <class CHARTYPE>
class YsGenericString :  protected YsArray <CHARTYPE,16>
{
protected:
	// >> Workaround for g++ v4.3.2.  These lines are not supposed to be necessary.
	using YsArray <CHARTYPE,16>::vv;
	using YsArray <CHARTYPE,16>::GetN;
	using YsArray <CHARTYPE,16>::Resize;
	using YsArray <CHARTYPE,16>::MakeUnitLength;
	// << Workaround for g++ v4.3.2.  These lines are not supposed to be necessary.

	using YsArray <CHARTYPE,16>::operator+;

protected:
	enum
	{
		CRCODE=0x0d,
		LFCODE=0x0a
	};


public:
	/*! For better interoperability with STL */
	enum
	{
		npos=0x7fffffff
	};

	/*! std::string defines both const and non-const iterators.
	    This class only defines const iterator.
	    Setting zero to a mid-letter through an iterator may make the actual string length and
	    the return value of Strlen() inconsistent.
	    Modification to the string is limited through access functions.
	*/
	class const_iterator
	{
	friend YsGenericString <CHARTYPE>;
	private:
		const YsGenericString <CHARTYPE> *strPtr;
		YSSIZE_T index;
		int dir;
	public:
		inline const_iterator &operator++()
		{
			index+=dir;
			return *this;
		}
		inline const_iterator operator++(int)
		{
			auto copy=*this;
			index+=dir;
			return copy;
		}
		inline const_iterator &operator--()
		{
			index-=dir;
			return *this;
		}
		inline const_iterator operator--(int)
		{
			auto copy=*this;
			index-=dir;
			return copy;
		}
		inline bool operator==(const const_iterator &from)
		{
			return (strPtr==from.strPtr && index==from.index);
		}
		inline bool operator!=(const const_iterator &from)
		{
			return (strPtr!=from.strPtr || index!=from.index);
		}
		inline CHARTYPE operator*()
		{
			return (*strPtr)[index];
		}
	};
	inline const_iterator begin(void) const
	{
		const_iterator iter;
		iter.index=0;
		iter.dir=1;
		iter.strPtr=this;
		return iter;
	}
	inline const_iterator end(void) const
	{
		const_iterator iter;
		iter.index=this->Strlen();
		iter.dir=1;
		iter.strPtr=this;
		return iter;
	}
	inline const_iterator rbegin(void) const
	{
		const_iterator iter;
		iter.index=this->Strlen()-1;
		iter.dir=-1;
		iter.strPtr=this;
		return iter;
	}
	inline const_iterator rend(void) const
	{
		const_iterator iter;
		iter.index=-1;
		iter.dir=-1;
		iter.strPtr=this;
		return iter;
	}



public:
	/*! Can be used for identifying character type from a template class. */
	typedef CHARTYPE CHAR_T;

	/*! Measures the number of characters of the string.  It counts the index until it finds 0==str[index]. */
	static YSSIZE_T Strlen(const CHARTYPE str[]);

	/*! Returns 0 if str1 and str2 are equal, positive number if str1 is lexically later than str2, or negative number
	   if str1 is lexically earlier than str2. */
	inline static int Strcmp(const CHARTYPE str1[],const CHARTYPE str2[]);

	/*! Returns 0 if str1 and str2 are equal, positive number if str1 is lexically later than str2, or negative number
	   if str1 is lexically earlier than str2.  
	   This function is case insensitive for the ascii alphabets. */
	inline static int STRCMP(const CHARTYPE str1[],const CHARTYPE str2[]);

	/*! Returns 0 if str1 and str2 are equal up to the given length or the length of the longer string of str1 and str2, 
	    positive number if str1 is lexically later than str2, or negative number if str1 is lexically earlier than str2. */
	static int Strncmp(const CHARTYPE str1[],const CHARTYPE str2[],YSSIZE_T n);

	/*! Returns 0 if str1 and str2 are equal up to the given length or the length of the longer string of str1 and str2, 
	    positive number if str1 is lexically later than str2, or negative number if str1 is lexically earlier than str2. */
	static int STRNCMP(const CHARTYPE str1[],const CHARTYPE str2[],YSSIZE_T n);

	/*! Copies the content of src to dst.  The caller must make sure dst has the sufficient length. */
	static CHARTYPE *Strcpy(CHARTYPE dst[],const CHARTYPE src[]);

	/*! Adds str2 to the end of str1 */
	static int Strcat(CHARTYPE str1[],const CHARTYPE str2[]);

	/*! Returns YSTRUE if c is printable, namely if c is greater than 0x20 (space).  TAB is not considered printable. */
	static YSBOOL Isprint(CHARTYPE c);

	/*! Returns YSTRUE if str includes c. */
	static YSBOOL CharIsOneOf(CHARTYPE c,const CHARTYPE str[]);

	/*! If a is same as b, return 0.  Otherwise, return a-b.  This function is case insensitive for ASCII characters. */
	static int CHARCMP(CHARTYPE a,CHARTYPE b);

	/*! Default constructor.  Creates an empty string. */
	inline YsGenericString();

	/*! Default constructor.  Creates a copy of the string. 
	    \param from [In] The source string
	    */
	inline YsGenericString(const CHARTYPE from[]);

	/*! Default constructor.  Creates a copy of the string. 
	    \param from [In] The source string
	    */
	inline YsGenericString(const YsGenericString <CHARTYPE> &from);

	/*! Cleans up. */
	inline void CleanUp(void);

	/*! Same as YsGenericString<CHARTYPE>::Strcmp(*this,compareWith). */
	inline int Strcmp(const CHARTYPE compareWith[]) const;

	/*! Same as YsGenericString<CHARTYPE>::STRCMP(*this,compareWith). */
	inline int STRCMP(const CHARTYPE compareWith[]) const;

	/*! Same as YsGenericString<CHARTYPE>::Strncmp(*this,compareWith,Strlen(compareWith)). */
	inline int Strncmp(const CHARTYPE compareWith[]) const;

	/*! Same as YsGenericString<CHARTYPE>::STRCMP(*this,compareWith,Strlen(compareWith)). */
	inline int STRNCMP(const CHARTYPE compareWith[]) const;

	/*! Same as YsGenericString<CHARTYPE>::Strncmp(*this,compareWith,len). */
	inline int Strncmp(const CHARTYPE compareWith[],YSSIZE_T len) const;

	/*! Same as YsGenericString<CHARTYPE>::STRCMP(*this,compareWith,len). */
	inline int STRNCMP(const CHARTYPE compareWith[],YSSIZE_T len) const;

	/*! Returns YSTRUE if the string starts with the given string. */
	inline YSBOOL DoesStartWith(const CHARTYPE compareWith[]) const;

	/*! Returns YSTRUE if the string starts with the given string. */
	inline YSBOOL DoesEndWith(const CHARTYPE compareWith[]) const;

	/*! Returns YSTRUE if the string starts with the given string.  DOESSTARTWITH is case insensitive.  DoesStartWith is case sensitive.  */
	inline YSBOOL DOESSTARTWITH(const CHARTYPE compareWith[]) const;

	/*! Returns YSTRUE if the string starts with the given string.  DOESENDWITH is case insensitive.  DoesEndWith is case sensitive. */
	inline YSBOOL DOESENDWITH(const CHARTYPE compareWith[]) const;

	/*! Returns the number of characters (excluding the zero terminator) of the array.
	    This function is faster than strlen in C-standard library because this YsGenericString
	    class stores the number of characters.
	    */
	inline YSSIZE_T Strlen(void) const;

	/*! Makes a copy of a string.
	    \param from [In] The source string
	    */
	inline YSRESULT Set(const CHARTYPE from[]);

	/*! Makes a copy of a string up to n characters.  If the length of the incoming
	    string is shorter than n, the string is copied up to its length.
	    \param n    [In] Maximum number of characters to be copied.
	    \param from [In] The source string
	    */
	inline YSRESULT Set(YSSIZE_T n,const CHARTYPE from[]);

	/*! Replaces one letter of the string.  This function substitutes c to the nth character (n is zero based).
	    If c is zero, it changes the length of the string. 
	    \param n [In] Location in the string to be replaced.  Must be 0<=n && n<Strlen()
	    \param c [In] New character 
	    */
	inline YSRESULT Set(YSSIZE_T n,CHARTYPE c);

	/*! Gives constant access to the characters in the string. */
	inline const CHARTYPE &operator[](YSSIZE_T idx) const;

	/*! Gives constant access to the characters in the string. */
	inline const CHARTYPE &operator[](YSSIZE_T idx);

	/*! Cast operator. */
	inline operator const CHARTYPE *() const;

	/*! Cast operator. */
	inline operator const CHARTYPE *();

	/*! Returns the constant pointer to the array that stores the string. */
	inline const CHARTYPE *Ptr(void ) const;

	/*! Returns the constant pointer to the array that stores the string. */
	inline const CHARTYPE *Txt(void ) const; // 2009/03/27 To make it look like YsVec3/YsVec2

	/*! Returns a pointer after the keyword.  If the string does not start with the keyword, it returns NULL.  
	    For example, if the string content is:
	       "InFile:Input.srf",
	    this->Skip("InFile:") will return a pointer that points to "Input.srf".
	    */
	inline const CHARTYPE *Skip(const CHARTYPE keyword[]) const;

	/*! Returns a pointer after the keyword.  If the string does not start with the keyword, it returns NULL.  
	    For example, if the string content is:
	       "InFile:Input.srf",
	    this->SKIP("infile:") will return a pointer that points to "Input.srf".

	    The difference between SKIP and Skip is that SKIP is case insensitive, and Skip is case sensitive.
	    */
	inline const CHARTYPE *SKIP(const CHARTYPE keyword[]) const;

	/*! Returns the constant pointer to the array that stores the string. */
	inline const CHARTYPE *GetArray(void) const;

	/*! Returns the last character of the string. */
	inline CHARTYPE LastChar(void) const;

	/*! Appends the given string. 
	    \param str [In] incoming string.
	    */
	inline YSRESULT Append(const CHARTYPE str[]);

	/*! Appends a character R times.
	    \param c [In] The character to be added R times.
	    */
	inline YSRESULT Append(CHARTYPE c,YSSIZE_T R=1);

	/*! Appends the given string up to n characters.
	    */
	inline YSRESULT Append(YSSIZE_T n,const CHARTYPE str[]);

	/*! Shortens the string by one character by deleting the last character. */
	inline YSRESULT BackSpace(void);

	/*! Shortens the string by one character by deleting (*this)[n]. */
	inline YSRESULT Delete(YSSIZE_T n);               // str[pos] is deleted.

	/*! Delets l characters starting from (*this)[n]. */
	inline YSRESULT DeleteRange(YSSIZE_T n,YSSIZE_T l);

	/*! Inserts R characters in the string.  Characters after (*this)[N] will be shifted
	    to the right by R characters, and (*this)[N] to (*this)[N+R-1] will become C. 
	    It does nothing if R<=0. */
	inline YSRESULT Insert(YSSIZE_T N,const CHARTYPE C,YSSIZE_T R=1);

	/*! Inserts a given string in this string.  Characters after (*this)[n] will be shifted
	    to the right by Strlen(str), and str[i] will be copied to (*this)[n+i]. */
	inline YSRESULT InsertString(YSSIZE_T n,const CHARTYPE str[]);

	/*! Deletes spaces and tabs at the begining of the string. */
	inline void DeleteHeadSpace(void);

	/*! Deletes spaces, tabs, and new-line codes at the end of the string. */
	inline void DeleteTailSpace(void);

	/*! Drop single-line comment after the given symbol. */
	inline void DropSingleLineComment(const CHARTYPE symble[],YSBOOL respectDoubleQuote=YSTRUE,YSBOOL respectSingleQuote=YSTRUE);

	/*! Pinch spaces and tabs in a sequence into a single space/tab. */
	inline void PinchConsecutiveSpace(YSBOOL respectDoubleQuote=YSTRUE,YSBOOL respectSingleQuote=YSTRUE);

	/*! Turns small letters ('a' to 'z') into capital letters ('A' to 'Z').
	    It does not capitalize full-size alphabets defined in UNICODE. */
	inline void Capitalize(void);

	/*! Turns capital letters ('A' to 'Z') into capital letters ('a' to 'z').
	    It does not uncapitalize full-size alphabets defined in UNICODE. */
	inline void Uncapitalize(void);

	/*! Changes the length of the string.  It can only shrink the string, but cannot
	    extend the string. */
	inline void SetLength(YSSIZE_T n);

	/*! This function assumes that this string is a file name and replaces the file extension.
	    \param ext [In] New extention.  It can but does not have to include '.'.  (It can be ".ext" or "ext") */
	inline void ReplaceExtension(const CHARTYPE ext[]);

	/*! This function assumes that this string is a file name and removes the file extension.*/
	inline void RemoveExtension(void);

	/*! This function assumes that this string is a file name and extracts the file extension.
	    \param ext [Out] Extention of the file name.
	    '.' will be included in the returned extension.  The ext will be like ".cpp" */
	inline void GetExtension(YsGenericString <CHARTYPE> &ext) const;

	/*! This function makes a subset of the string. */
	inline void GetSubset(YsGenericString <CHARTYPE> &sub,YSSIZE_T top,YSSIZE_T length=npos) const;

	/*! Makes a full-path file name from the directory path and file name.
	    \param path [In] Directory path
	    \param name [In] File name */
	const CHARTYPE *MakeFullPathName(const CHARTYPE path[],const CHARTYPE name[]);

	/*! Decomposes the string into multiple words. 
	    \tparam STRTYPE Type of the class that stores decomposed words.  It needs to be YsGenericString or its sub-class.
	    \param args [Out] Decomposed words
	    \param blank [In] An string of CHARTYPE that contains blank (separator) characters
	    \param comma [In] An string of CHARTYPE that contains comma (separator) characters
	    */
	template <class STRTYPE,const int N>
	YSRESULT Arguments(YsArray <STRTYPE,N> &args,const CHARTYPE *blank,const CHARTYPE *comma) const;

	/*! This function assumes that this string is a file name with directory path, and separate it into
	    directory part and file-name part.
	    \param PATH [Out] Returns directory part
	    \param NAME [Out] Returns file-name part

	    File-name part is the part of the string after the last separator, which is '/', '\\', or ':'.
	    Directory part of the string will be the rest.  Therefore, the directory part may contain the separator.
	    If this string does not include a separator, PATH will be empty, and NAME will be same as this string.
	    */
	inline YSRESULT SeparatePathFile(YsGenericString <CHARTYPE> &PATH,YsGenericString <CHARTYPE> &NAME) const;

	/*! This function simplifies the path name.  For example:
	       /a//b/c => /a/b/c
	       /a/b/./c => /a/b/c
	       /a/b/../c  => /a/c
	    Returns YSOK if the path name was simplified.  YSERR if the path name cannot be simplified any more. */
	inline YSRESULT SimplifyPath(void);
private:
	inline YSRESULT SimplifyPathOneStep(void);

public:
	/*! Makes a UNICODE string by decoding UTF-8 encoded char string.
	    \param utf8 [In] A UTF-8 encoded string 
	    */
	inline YSRESULT SetUTF8String(const char utf8[]);

	/*! Makes a UTF-8 encoded string.
	    \param utf8 [Out] Returns a UTF-8 encoded string 
	    */
	inline YSRESULT GetUTF8String(class YsString &utf8) const;

	/*! Reads a line from a file using fgets function, and decodes UNICODE string assuming that the 
	    incoming line is UTF-8 encoded. */
	inline const CHARTYPE *FgetsUTF8(FILE *fp);

	/*! Returns YSTRUE if this string includes a keyword.  If firstIndex is not NULL, first position (index to the first 
	    letter of the first occurence of the keyword) will be returned to *firstIndex.

	    If this string does not include the keyword and firstIndex is not NULL, *firstIndex will be -1.

	    This function is case sensitive.  */
	inline YSBOOL FindWord(YSSIZE_T *firstIndex,const CHARTYPE keyword[]) const;

	/*! Case insensitive version of FindWord.  */
	inline YSBOOL FINDWORD(YSSIZE_T *firstIndex,const CHARTYPE keyword[]) const;

	/*! Returns YSTRUE if KEYWORD is included in the string, and either it is:
	    - delimited by a non-alphabetical and non-numeric character on both sides, or
	    - delimited by a non-alhpabetical and non-numeric character on one side and the other side by the beginning or the end of the string, or
	    - KEYWORD is this string.
	    *firstIndex will be the first appearance of the delimited word, or -1 if the word does not appear.
	 */
	inline YSBOOL FindDelimitedWord(YSSIZE_T *firstIndex,const CHARTYPE keyword[]) const;

	/*! Returns YSTRUE if all words in the searchText is included in this string. */
	inline YSBOOL MatchSearchKeyWord(const CHARTYPE searchText[]) const;

	/*! Returns YSTRUE if all words in the searchText is included in this string. */
	inline YSBOOL MatchSearchKeyWord(const YsGenericString <CHARTYPE> &searchText) const;

	/*! Converts "\n", "\a", "\r", and "\t" to a corresponding control code, and "\\" to "\". 
	    Returns YSOK if there is no error.
	    Returns YSERR if unrecognized escape is included.
	*/
	inline YSRESULT ConvertEscape(void) const;

	/*! Converts control codes to "\n", "\a", "\r", "\t".  Also "\" to "\\" and '"' to '\"'. */
	inline YSRESULT EncodeEscape(void);

	/*! Replaces fromStr with toStr.
	    Returns the number of words that has been replaced.
	    fromStr is case sensitive. */
	inline int Replace(const CHARTYPE fromStr[],const CHARTYPE toStr[]);

	/*! Replaces fromStr with toStr.
	    Returns the number of words that has been replaced.
	    fromStr is case insensitive. */
	inline int REPLACE(const CHARTYPE fromStr[],const CHARTYPE toStr[]);

	/*! Replaces N characters from FR with INCOMING string. */
	inline void Replace(YSSIZE_T FR,YSSIZE_T N,const CHARTYPE INCOMING[]);

private:
	inline int ExecReplace(
	    const CHARTYPE fromStr[],
	    const CHARTYPE toStr[],
	    int (*cmpfunc)(const CHARTYPE [],const CHARTYPE [],YSSIZE_T));

public:
	/*! For better interoperability with STL */
	inline void clear(void)
	{
		const CHARTYPE zero[1]={0};
		this->Set(zero);
	}
	/*! For better interoperability with STL */
	inline bool empty(void) const
	{
		return 0==Strlen();
	}
	/*! For better interoperability with STL */
	inline void swap(YsGenericString <CHARTYPE> &incoming)
	{
		auto tmp=*this;
		this->MoveFrom(incoming);
		incoming.MoveFrom(tmp);
	}
	/*! For better interoperability with STL */
	inline YSSIZE_T size(void) const
	{
		return Strlen();
	}
	/*! For better interoperability with STL */
	inline YSSIZE_T length(void) const
	{
		return Strlen();
	}
	/*! For better interoperability with STL */
	inline CHARTYPE front(void) const
	{
		return (*this)[0];
	}
	/*! For better interoperability with STL */
	inline CHARTYPE back(void) const
	{
		return LastChar();
	}
	/*! For better interoperability with STL */
	inline void pop_back(void)
	{
		BackSpace();
	}
	/*! For better interoperability with STL */
	inline void resize(YSSIZE_T n,CHARTYPE c=' ')
	{
		if(n<Strlen())
		{
			SetLength(n);
		}
		else
		{
			Append(c,n-Strlen());
		}
	}
	/*! For better interoperability with STL */
	inline void push_back(CHARTYPE c)
	{
		this->Append(c);
	}
	/*! For better interoperability with STL */
	inline const CHARTYPE *c_str(void) const
	{
		return Txt();
	}
	/*! For better interoperability with STL */
	inline const CHARTYPE *data(void) const
	{
		return Txt();
	}
	/*! For better interoperability with STL */
	inline int compare(const YsGenericString <CHARTYPE> &str) const
	{
		return Strcmp(str);
	}
	/*! For better interoperability with STL */
	inline int compare(const CHARTYPE str[]) const
	{
		return Strcmp(str);
	}


	/*! Rturns an integer described in this string.
	    Preciding spaces and tabs, and trailing non-numeric characters will be ignored.
	*/
	int Atoi(void) const;

	/*! Rturns an integer described in this string.
	    Preciding spaces and tabs, and trailing non-numeric characters will be ignored.
	*/
	long long int Atol(void) const;

	/*! Returns a double described in this string.
	    Preciding spaces and tabs, and trailing non-numeric, non-comma, non-period characters will be ignored.
	*/
	double Atof(void) const;

	/*! Same as Atof.
	*/
	double Atof64(void) const;

	/*! Same as Atof, but it returns float instead of double.
	*/
	float Atof32(void) const;

protected:
	template <class Float_Type>
	Float_Type AtoReal(void) const;
	template <class Int_Type>
	Int_Type GetRawNumber(long long int &ptr) const;

public:
	enum
	{
		FMT_USEPLUS=1,
		FMT_USESPACE=2,
	};
};

template <class CHARTYPE>
typename YsGenericString<CHARTYPE>::const_iterator begin(const YsGenericString<CHARTYPE> &str)
{
	return str.begin();
}
template <class CHARTYPE>
typename YsGenericString<CHARTYPE>::const_iterator end(const YsGenericString<CHARTYPE> &str)
{
	return str.end();
}
template <class CHARTYPE>
typename YsGenericString<CHARTYPE>::const_iterator rbegin(const YsGenericString<CHARTYPE> &str)
{
	return str.rbegin();
}
template <class CHARTYPE>
typename YsGenericString<CHARTYPE>::const_iterator rend(const YsGenericString<CHARTYPE> &str)
{
	return str.rend();
}


template <class CHARTYPE>
inline int YsGenericString<CHARTYPE>::Strcmp(const CHARTYPE compareWith[]) const
{
	return Strcmp(this->GetArray(),compareWith);
}

template <class CHARTYPE>
/* static */ int YsGenericString<CHARTYPE>::STRCMP(const CHARTYPE str1[],const CHARTYPE str2[])
{
	YSSIZE_T i;
	CHARTYPE c1=0,c2=0;
	for(i=0; 0!=str1[i] && 0!=str2[i]; i++)
	{
		c1=str1[i];
		c2=str2[i];
		if('a'<=c1 && c1<='z')
		{
			c1=c1-'a'+'A';
		}
		if('a'<=c2 && c2<='z')
		{
			c2=c2-'a'+'A';
		}
		if(c1!=c2)
		{
			return c1-c2;
		}
	}
	return str1[i]-str2[i];
}

template <class CHARTYPE>
inline int YsGenericString<CHARTYPE>::STRCMP(const CHARTYPE compareWith[]) const
{
	return STRCMP(this->GetArray(),compareWith);
}

template <class CHARTYPE>
inline YSBOOL YsGenericString<CHARTYPE>::DoesStartWith(const CHARTYPE compareWith[]) const
{
	const YSSIZE_T rStrlen=Strlen(compareWith);
	if(Strlen()<rStrlen)
	{
		return YSFALSE;
	}

	for(YSSIZE_T idx=0; idx<rStrlen; ++idx)
	{
		if((*this)[idx]!=compareWith[idx])
		{
			return YSFALSE;
		}
	}

	return YSTRUE;
}

template <class CHARTYPE>
inline YSBOOL YsGenericString<CHARTYPE>::DoesEndWith(const CHARTYPE compareWith[]) const
{
	const YSSIZE_T lStrlen=Strlen();
	const YSSIZE_T rStrlen=Strlen(compareWith);
	if(lStrlen<rStrlen)
	{
		return YSFALSE;
	}

	for(YSSIZE_T idx=0; idx<rStrlen; ++idx)
	{
		if((*this)[lStrlen-rStrlen+idx]!=compareWith[idx])
		{
			return YSFALSE;
		}
	}

	return YSTRUE;
}

template <class CHARTYPE>
inline YSBOOL YsGenericString<CHARTYPE>::DOESSTARTWITH(const CHARTYPE compareWith[]) const
{
	const YSSIZE_T rStrlen=Strlen(compareWith);
	if(Strlen()<rStrlen)
	{
		return YSFALSE;
	}

	for(YSSIZE_T idx=0; idx<rStrlen; ++idx)
	{
		if(0!=CHARCMP((*this)[idx],compareWith[idx]))
		{
			return YSFALSE;
		}
	}

	return YSTRUE;
}

template <class CHARTYPE>
inline YSBOOL YsGenericString<CHARTYPE>::DOESENDWITH(const CHARTYPE compareWith[]) const
{
	const YSSIZE_T lStrlen=Strlen();
	const YSSIZE_T rStrlen=Strlen(compareWith);
	if(lStrlen<rStrlen)
	{
		return YSFALSE;
	}

	for(YSSIZE_T idx=0; idx<rStrlen; ++idx)
	{
		if(0!=CHARCMP((*this)[lStrlen-rStrlen+idx],compareWith[idx]))
		{
			return YSFALSE;
		}
	}

	return YSTRUE;
}

template <class CHARTYPE>
inline YSSIZE_T YsGenericString <CHARTYPE>::Strlen(const CHARTYPE str[])
{
	YSSIZE_T i;
	for(i=0; 0!=str[i]; i+=4)
	{
		if(0==str[i+1])  // I don't know this classis loop-expansion technique does any performance improvement though.
		{
			return i+1;
		}
		else if(0==str[i+2])
		{
			return i+2;
		}
		else if(0==str[i+3])
		{
			return i+3;
		}
	}
	return i;
}

template <class CHARTYPE>
inline int YsGenericString <CHARTYPE>::Strcmp(const CHARTYPE str1[],const CHARTYPE str2[])
{
	YSSIZE_T i;
	for(i=0; 0!=str1[i] && 0!=str2[i] && str1[i]==str2[i]; i++)
	{
	}
	return (int)str1[i]-(int)str2[i];
}

template <class CHARTYPE>
inline int YsGenericString <CHARTYPE>::Strncmp(const CHARTYPE str1[],const CHARTYPE str2[],YSSIZE_T n)
{
	YSSIZE_T i;
	for(i=0; i<n && 0!=str1[i] && 0!=str2[i] && str1[i]==str2[i]; i++)
	{
	}
	return (i==n ? 0 : (int)str1[i]-(int)str2[i]);
}

template <class CHARTYPE>
inline int YsGenericString <CHARTYPE>::STRNCMP(const CHARTYPE str1[],const CHARTYPE str2[],YSSIZE_T n)
{
	YSSIZE_T i;
	CHARTYPE c1=0,c2=0;
	for(i=0; 0!=str1[i] && 0!=str2[i] && i<n; i++)
	{
		c1=str1[i];
		c2=str2[i];
		if('a'<=c1 && c1<='z')
		{
			c1=c1-'a'+'A';
		}
		if('a'<=c2 && c2<='z')
		{
			c2=c2-'a'+'A';
		}
		if(c1!=c2)
		{
			return c1-c2;
		}
	}
	return (i==n ? 0 : (int)str1[i]-(int)str2[i]);
}

template <class CHARTYPE>
inline int YsGenericString<CHARTYPE>::Strncmp(const CHARTYPE compareWith[]) const
{
	return Strncmp(this->GetArray(),compareWith,this->Strlen(compareWith));
}
template <class CHARTYPE>
inline int YsGenericString<CHARTYPE>::STRNCMP(const CHARTYPE compareWith[]) const
{
	return STRNCMP(this->GetArray(),compareWith,this->Strlen(compareWith));
}
template <class CHARTYPE>
inline int YsGenericString<CHARTYPE>::Strncmp(const CHARTYPE compareWith[],YSSIZE_T len) const
{
	return Strncmp(this->GetArray(),compareWith,len);
}
template <class CHARTYPE>
inline int YsGenericString<CHARTYPE>::STRNCMP(const CHARTYPE compareWith[],YSSIZE_T len) const
{
	return STRNCMP(this->GetArray(),compareWith,len);
}

template <class CHARTYPE>
inline CHARTYPE *YsGenericString <CHARTYPE>::Strcpy(CHARTYPE dst[],const CHARTYPE src[])
{
	int i;
	for(i=0; 0!=src[i]; i++)
	{
		dst[i]=src[i];
	}
	dst[i]=0;
	return dst;
}

template <class CHARTYPE>
int YsGenericString <CHARTYPE>::Strcat(CHARTYPE str1[],const CHARTYPE str2[])
{
	int l=0;
	for(l=0; 0!=str1[l]; ++l)
	{
	}

	if(NULL!=str2)
	{
		for(int i=0; 0!=str2[i]; ++i)
		{
			str1[l]=str2[i];
			++l;
		}
	}

	str1[l]=0;
	return l;
}

template <class CHARTYPE>
inline YSBOOL YsGenericString <CHARTYPE>::Isprint(CHARTYPE c)
{
	if(c<0 || 0x20<=c) // 2014/10/26 Added c<0. UTF-8 String uses bit-7
	{
		return YSTRUE;
	}
	else
	{
		return YSFALSE;
	}
}

template <class CHARTYPE>
inline YSBOOL YsGenericString <CHARTYPE>::CharIsOneOf(CHARTYPE c,const CHARTYPE str[])
{
	int i;
	for(i=0; str[i]!=0; i++)
	{
		if(str[i]==c)
		{
			return YSTRUE;
		}
	}
	return YSFALSE;
}

template <class CHARTYPE>
/* static */ int YsGenericString<CHARTYPE>::CHARCMP(CHARTYPE a,CHARTYPE b)
{
	if('a'<=a && a<='z')
	{
		a=a-'a'+'A';
	}
	if('a'<=b && b<='z')
	{
		b=b-'a'+'A';
	}
	return a-b;
}

template <class CHARTYPE>
inline YsGenericString <CHARTYPE>::YsGenericString()
{
	YsArray <CHARTYPE,16>::MakeUnitLength(0);
}

template <class CHARTYPE>
inline YsGenericString <CHARTYPE>::YsGenericString(const CHARTYPE from[])
{
	Set(from);
}

template <class CHARTYPE>
inline YsGenericString <CHARTYPE>::YsGenericString(const YsGenericString <CHARTYPE> &from)
{
	YsArray <CHARTYPE,16>::Set(from.Strlen()+1,from.GetArray());
}

template <class CHARTYPE>
inline void YsGenericString<CHARTYPE>::CleanUp(void)
{
	YsArray <CHARTYPE,16>::MakeUnitLength(0);
}

template <class CHARTYPE>
inline YSSIZE_T YsGenericString <CHARTYPE>::Strlen(void) const
{
	return YsArray <CHARTYPE,16>::GetN()-1;
}

template <class CHARTYPE>
inline YSRESULT YsGenericString <CHARTYPE>::Set(const CHARTYPE str[])
{
	if(YsArray <CHARTYPE,16>::vv==str)
	{
		return YSOK;
	}
	if(NULL!=str)
	{
		return YsArray <CHARTYPE,16>::Set(Strlen(str)+1,str);
	}
	else
	{
		CHARTYPE empty[1]={0};
		return YsArray <CHARTYPE,16>::Set(1,empty);
	}
}

template <class CHARTYPE>
inline YSRESULT YsGenericString <CHARTYPE>::Set(YSSIZE_T n,const CHARTYPE str[])
{
	if(NULL==str)
	{
		return Set(NULL);
	}

	YSSIZE_T l;
	l=(YSSIZE_T)Strlen(str);
	if(l<n)
	{
		n=l;
	}

	if(YsArray <CHARTYPE,16>::Set(n+1,str)==YSOK)
	{
		YsArray <CHARTYPE,16>::vv[n]=0;
		return YSOK;
	}
	else
	{
		return YSERR;
	}
}

template <class CHARTYPE>
inline YSRESULT YsGenericString <CHARTYPE>::Set(YSSIZE_T n,CHARTYPE c)
{
	YsArray <CHARTYPE,16>::vv[n]=c;
	if(0==c && n<GetN()-1)
	{
		Resize(n+1);
	}
	return YSOK;
}

template <class CHARTYPE>
inline const CHARTYPE &YsGenericString <CHARTYPE>::operator[](YSSIZE_T idx) const
{
	return vv[idx];
}

template <class CHARTYPE>
inline const CHARTYPE &YsGenericString <CHARTYPE>::operator[](YSSIZE_T idx)
{
	return vv[idx];
}

template <class CHARTYPE>
inline YsGenericString <CHARTYPE>::operator const CHARTYPE *() const
{
	return vv;
}

template <class CHARTYPE>
inline YsGenericString <CHARTYPE>::operator const CHARTYPE *()
{
	return vv;
}

template <class CHARTYPE>
inline const CHARTYPE *YsGenericString <CHARTYPE>::Ptr(void) const
{
	return vv;
}

template <class CHARTYPE>
inline const CHARTYPE *YsGenericString <CHARTYPE>::Txt(void) const
{
	return vv;
}

template <class CHARTYPE>
inline const CHARTYPE *YsGenericString<CHARTYPE>::Skip(const CHARTYPE keyword[]) const
{
	if(YSTRUE==this->DoesStartWith(keyword))
	{
		return Txt()+Strlen(keyword);
	}
	return NULL;
}

template <class CHARTYPE>
inline const CHARTYPE *YsGenericString<CHARTYPE>::SKIP(const CHARTYPE keyword[]) const
{
	if(YSTRUE==this->DOESSTARTWITH(keyword))
	{
		return Txt()+Strlen(keyword);
	}
	return NULL;
}

template <class CHARTYPE>
inline const CHARTYPE *YsGenericString <CHARTYPE>::GetArray(void) const
{
	return vv;
}

template <class CHARTYPE>
inline CHARTYPE YsGenericString <CHARTYPE>::LastChar(void) const
{
	if(Strlen()>0)
	{
		return vv[Strlen()-1];
	}
	return 0;
}

template <class CHARTYPE>
inline YSRESULT YsGenericString <CHARTYPE>::Append(const CHARTYPE str[])
{
	Resize(GetN()-1);
	return YsArray <CHARTYPE,16>::Append((int)Strlen(str)+1,str);
}

template <class CHARTYPE>
inline YSRESULT YsGenericString <CHARTYPE>::Append(CHARTYPE c,YSSIZE_T repeat)
{
	if(0<repeat)
	{
		auto N=Strlen();
		Resize(N+repeat+1);
		for(YSSIZE_T i=0; i<repeat; ++i)
		{
			vv[N+i]=c;
		}
		vv[N+repeat]=0;
	}
	return YSOK;
}

template <class CHARTYPE>
YSRESULT YsGenericString <CHARTYPE>::Append(YSSIZE_T n,const CHARTYPE str[])
{
	if(NULL!=str)
	{
		YSSIZE_T  curLen=Strlen();
		YSSIZE_T  toAdd=YsSmaller <YSSIZE_T> (n,this->Strlen(str));
		Resize(curLen+toAdd+1);
		for(YSSIZE_T i=0; i<toAdd; i++)
		{
			vv[curLen+i]=str[i];
		}
		vv[curLen+toAdd]=0;
	}
	return YSOK;
}

template <class CHARTYPE>
inline YSRESULT YsGenericString <CHARTYPE>::BackSpace(void)
{
	if(Strlen()>0)
	{
		YSSIZE_T l=Strlen();
		vv[l-1]=0;
		Resize(l);
		return YSOK;
	}
	return YSERR;
}

template <class CHARTYPE>
inline YSRESULT YsGenericString <CHARTYPE>::Delete(YSSIZE_T pos)               // str[pos] is deleted.
{
	if(0<=pos && pos<Strlen())
	{
		YSSIZE_T l=Strlen();

		for(YSSIZE_T i=pos; i<l; i++)
		{
			vv[i]=vv[i+1];
		}

		Resize(l);
		return YSOK;
	}
	return YSERR;
}

template <class CHARTYPE>
inline YSRESULT YsGenericString <CHARTYPE>::DeleteRange(YSSIZE_T pos,YSSIZE_T lng)
{
	if(0<=pos && pos<Strlen() && 0<lng)
	{
		if(Strlen()<=pos+lng)
		{
			SetLength(pos);
		}
		else
		{
			for(YSSIZE_T i=0; pos+i+lng<Strlen(); i++)
			{
				vv[pos+i]=vv[pos+i+lng];
			}
			SetLength(Strlen()-lng);
		}
		return YSOK;
	}
	return YSERR;
}

template <class CHARTYPE>
inline YSRESULT YsGenericString <CHARTYPE>::Insert(YSSIZE_T pos,const CHARTYPE c,YSSIZE_T repeat)  // str[pos] will be c.  Later string will be shifted.
{
	if(repeat<=0)
	{
		return YSOK;
	}
	const YSSIZE_T n=Strlen();
	if(Strlen()<=pos)
	{
		Append(c,repeat);
		return YSOK;
	}
	else if(0<=pos)
	{
		Resize(n+repeat+1);

		for(YSSIZE_T i=n+repeat; pos+repeat<=i; i--)
		{
			vv[i]=vv[i-repeat];
		}
		for(YSSIZE_T i=0; i<repeat; ++i)
		{
			vv[pos+i]=c;
		}

		return YSOK;
	}
	return YSERR;
}

template <class CHARTYPE>
inline YSRESULT YsGenericString <CHARTYPE>::InsertString(YSSIZE_T pos,const CHARTYPE str[])
{
	if(0==Strlen())
	{
		return Set(str);
	}
	else
	{
		const YSSIZE_T lng=Strlen(str);
		if(0<lng)
		{
			if(0>pos)
			{
				pos=0;
			}
			if(Strlen()<pos)
			{
				pos=Strlen();
			}

			SetLength(Strlen()+lng);

			for(YSSIZE_T i=Strlen()-1; i>=(pos+lng); i--)
			{
				vv[i]=vv[i-lng];
			}

			for(YSSIZE_T i=0; i<lng; i++)
			{
				vv[pos+i]=str[i];
			}
		}
	}
	return YSOK;
}

template <class CHARTYPE>
inline void YsGenericString <CHARTYPE>::DeleteHeadSpace(void)
{
	int i,nDel;
	nDel=0;
	for(i=0; vv[i]!=0; i++)
	{
		nDel=i;
		if(vv[i]!=' ' && vv[i]!='\t' && YSTRUE==Isprint(vv[i]))
		{
			break;
		}
	}

	if(nDel>0)
	{
		for(i=nDel; vv[i]!=0; i++)
		{
			vv[i-nDel]=vv[i];
		}
		vv[i-nDel]=0;
		Resize(GetN()-nDel);
	}
}

template <class CHARTYPE>
inline void YsGenericString <CHARTYPE>::DeleteTailSpace(void)
{
	unsigned int chr;
	YSSIZE_T i,cut,len;

	len=Strlen();
	cut=0;
	for(i=0; i<len; i++)
	{
		chr=(unsigned int)(vv[i]);
		if(chr!=' ' && chr!='\t' && YSTRUE==Isprint(chr))
		{
			cut=i+1;
		}
	}

	if(cut<len)
	{
		vv[cut]=0;
		Resize(cut+1);
	}
}

template <class CHARTYPE>
inline void YsGenericString<CHARTYPE>::DropSingleLineComment(const CHARTYPE symbol[],YSBOOL respectDoubleQuote,YSBOOL respectSingleQuote)
{
	const int NORMAL=0,IN_DOUBLEQUOTE=1,IN_SINGLEQUOTE=2;
	int state=NORMAL;

	auto symbolLen=YsGenericString<CHARTYPE>::Strlen(symbol);

	for(YSSIZE_T ptr=0; 0!=vv[ptr]; ++ptr)
	{
		switch(state)
		{
		case NORMAL:
			if(0==YsGenericString<CHARTYPE>::Strncmp(vv+ptr,symbol,symbolLen))
			{
				SetLength(ptr);
				return;
			}
			else if(YSTRUE==respectDoubleQuote && '\"'==vv[ptr])
			{
				state=IN_DOUBLEQUOTE;
			}
			else if(YSTRUE==respectSingleQuote && '\''==vv[ptr])
			{
				state=IN_SINGLEQUOTE;
			}
			break;
		case IN_DOUBLEQUOTE:
			if('\"'==vv[ptr])
			{
				state=NORMAL;
			}
			break;
		case IN_SINGLEQUOTE:
			if('\''==vv[ptr])
			{
				state=NORMAL;
			}
			break;
		}
	}
}

template <class CHARTYPE>
inline void YsGenericString<CHARTYPE>::PinchConsecutiveSpace(YSBOOL respectDoubleQuote,YSBOOL respectSingleQuote)
{
	const int NORMAL=0,IN_DOUBLEQUOTE=1,IN_SINGLEQUOTE=2;
	int state=NORMAL;

	for(YSSIZE_T ptr=0; 0!=vv[ptr]; ++ptr)
	{
		switch(state)
		{
		case NORMAL:
			if((' '==vv[ptr] || '\t'==vv[ptr]) && (' '==vv[ptr+1] || '\t'==vv[ptr+1]))
			{
				Delete(ptr);
				--ptr;
			}
			else if(YSTRUE==respectDoubleQuote && '\"'==vv[ptr])
			{
				state=IN_DOUBLEQUOTE;
			}
			else if(YSTRUE==respectSingleQuote && '\''==vv[ptr])
			{
				state=IN_SINGLEQUOTE;
			}
			break;
		case IN_DOUBLEQUOTE:
			if('\"'==vv[ptr])
			{
				state=NORMAL;
			}
			break;
		case IN_SINGLEQUOTE:
			if('\''==vv[ptr])
			{
				state=NORMAL;
			}
			break;
		}
	}
}

template <class CHARTYPE>
inline void YsGenericString <CHARTYPE>::Capitalize(void)
{
	int i;
	for(i=0; vv[i]!=0; i++)
	{
		if('a'<=vv[i] && vv[i]<='z')
		{
			vv[i]=vv[i]+'A'-'a';
		}
	}
}

template <class CHARTYPE>
inline void YsGenericString <CHARTYPE>::Uncapitalize(void)
{
	int i;
	for(i=0; vv[i]!=0; i++)
	{
		if('A'<=vv[i] && vv[i]<='Z')
		{
			vv[i]=vv[i]+'a'-'A';
		}
	}
}

template <class CHARTYPE>
inline void YsGenericString <CHARTYPE>::SetLength(YSSIZE_T n)
{
	if(0<=n)
	{
		YSSIZE_T k=Strlen();

		Resize(n+1);
		for(YSSIZE_T i=k; i<n; i++)
		{
			vv[i]=' ';
		}
		vv[n]=0;
	}
}

template <class CHARTYPE>
inline void YsGenericString <CHARTYPE>::ReplaceExtension(const CHARTYPE ext[])
{
	YSSIZE_T i,insPoint;
	YSSIZE_T extLen;
	const CHARTYPE *extWithoutDot;

	if(ext==NULL || ext[0]==0)
	{
		RemoveExtension();
		return;
	}

	if(ext[0]=='.')
	{
		extLen=(int)Strlen(ext)-1;
		extWithoutDot=ext+1;
	}
	else
	{
		extLen=(int)Strlen(ext);
		extWithoutDot=ext;
	}

	YSSIZE_T l=Strlen();
	for(i=l-1; i>=0; i--)
	{
		if(vv[i]=='.' || vv[i]=='/' || vv[i]=='\\')
		{
			break;
		}
	}

	if(i<0 || vv[i]!='.') // Means there's no dot in the filename.
	{
		insPoint=Strlen();
		YsArray <CHARTYPE,16>::Resize(Strlen()+extLen+2);
	}
	else
	{
		insPoint=i;
		YsArray <CHARTYPE,16>::Resize(insPoint+extLen+2);
	}

	vv[insPoint]='.';
	Strcpy(vv+insPoint+1,extWithoutDot);
}

template <class CHARTYPE>
inline void YsGenericString <CHARTYPE>::RemoveExtension(void)
{
	for(YSSIZE_T i=Strlen()-1; i>=0; i--)
	{
		if(vv[i]=='.')
		{
			vv[i]=0;
			YsArray <CHARTYPE,16>::Resize(i+1);
			break;
		}
		else if(vv[i]=='/' || vv[i]=='\\' || vv[i]==':')
		{
			break;
		}
	}
}

template <class CHARTYPE>
inline void YsGenericString <CHARTYPE>::GetExtension(YsGenericString <CHARTYPE> &ext) const
{
	for(YSSIZE_T i=Strlen()-1; i>=0; i--)
	{
		if('.'==vv[i])
		{
			ext.Set(vv+i);
			return;
		}
	}
	ext.SetLength(0);
}

template <class CHARTYPE>
inline void YsGenericString <CHARTYPE>::GetSubset(YsGenericString <CHARTYPE> &sub,YSSIZE_T top,YSSIZE_T length) const
{
	YsMakeSmaller<YSSIZE_T>(length,Strlen()-top);
	if(0>=length)
	{
		sub.SetLength(0);
	}
	else
	{
		sub.Set(length,YsArray <CHARTYPE,16>::GetArray()+top);
	}
}

template <class CHARTYPE>
inline const CHARTYPE *YsGenericString <CHARTYPE>::MakeFullPathName(const CHARTYPE path[],const CHARTYPE name[])
{
	Set(path);
	if(LastChar()=='/' || LastChar()=='\\')
	{
		if('/'!=name[0] && '\\'!=name[0])
		{
			Append(name);
		}
		else
		{
			Append(name+1);
		}
	}
	else
	{
		if('/'!=name[0] && '\\'!=name[0])
		{
			const CHARTYPE add[2]={'/',0};
			Append(add);
		}
		Append(name);
	}
	return Txt();
}

template <class CHARTYPE>
template <class STRTYPE,const int N>
YSRESULT YsGenericString <CHARTYPE>::Arguments(YsArray <STRTYPE,N> &args,const CHARTYPE *blank,const CHARTYPE *comma) const
{
	args.Set(0,NULL);

	const YSSIZE_T l=Strlen();

	YSSIZE_T i=0;
	while(YSTRUE==CharIsOneOf(vv[i],blank))
	{
		i++;
	}

	YSSIZE_T head=i;
	while(i<l)
	{
		if(vv[head]=='\"')
		{
			head++;
			i++;
			while(i<l && vv[i]!='\"')
			{
				if(YSTRUE!=Isprint(vv[i]))
				{
					break;
				}
				i++;
			}

			args.Increment();
			args.GetEnd().Set(i-head,vv+head);

			if(vv[i]=='\"')
			{
				i++;
			}
		}
		else
		{
			while(i<l && (CharIsOneOf(vv[i],blank)!=YSTRUE && CharIsOneOf(vv[i],comma)!=YSTRUE))
			{
				if(YSTRUE!=Isprint(vv[i]))
				{
					break;
				}
				i++;
			}

			if(head<i)  // <- This condition is added on 2005/03/03
			{
				args.Increment();
				args.GetEnd().Set(i-head,vv+head);
			}
			else if(head==i && CharIsOneOf(vv[i],comma)==YSTRUE) // < This condition is added (I thought there was, did I accidentally delete?) on 2012/01/26
			{
				const CHARTYPE empty[1]={0};
				args.Increment();
				args.GetEnd().Set(empty);
			}
		}

		while(i<l && YSTRUE!=Isprint(vv[i]))
		{
			i++;
		}
		while(i<l && CharIsOneOf(vv[i],blank)==YSTRUE)  // Skip blank separator
		{
			i++;
		}
		if(CharIsOneOf(vv[i],comma)==YSTRUE) // Skip one comma separator
		{
			i++;

			while(i<l && CharIsOneOf(vv[i],blank)==YSTRUE)  // Skip blank separator after a comma separator
			{
				i++;
			}

			if(i==l)
			{
				args.Increment();
				args.GetEnd().MakeUnitLength(0);
			}
		}
		head=i;
	}

	return YSOK;
}

template <class CHARTYPE>
inline YSRESULT YsGenericString <CHARTYPE>::SeparatePathFile(YsGenericString <CHARTYPE> &pth,YsGenericString <CHARTYPE> &fil) const
{
	int cutPtr,seekPtr;

	cutPtr=0;

	/* Skip Drive Name */
	if(vv[0]!=0 && vv[1]==':')
	{
		cutPtr+=2;
	}

	/* scan until nul Stopper */
	for(seekPtr=cutPtr; vv[seekPtr]!=0; seekPtr++)
	{
		if(vv[seekPtr]=='\\' || vv[seekPtr]=='/' || vv[seekPtr]==':')
		{
			cutPtr=seekPtr+1;
		}
	}

	/* cutPtr points *tmp  or after ':' or after last '\\' */
	fil.Set(vv+cutPtr);

	pth.Resize(cutPtr+1);
	for(YSSIZE_T i=0; i<cutPtr; i++)
	{
		pth.vv[i]=vv[i];
	}
	pth.vv[cutPtr]=0;

	return YSOK;
}

template <class CHARTYPE>
inline YSRESULT YsGenericString <CHARTYPE>::SimplifyPath(void)
{
	YSRESULT res=YSERR;
	while(YSOK==SimplifyPathOneStep())
	{
		res=YSOK;
	}
	return res;
}

template <class CHARTYPE>
inline YSRESULT YsGenericString <CHARTYPE>::SimplifyPathOneStep(void)
{
	YSRESULT worked=YSERR;
	YsGenericString <CHARTYPE> edit=*this;

	// Also need to change // \\ \/ or \/ into single separator.
	for(auto idx=edit.Strlen()-1; 0<idx; --idx)
	{
		if(('/'==edit[idx] || '\\'==edit[idx]) && ('/'==edit[idx-1] || '\\'==edit[idx-1]))
		{
			edit.Delete((int)idx);
			worked=YSOK;
		}
	}

	// Delete something/.. or something\..
	// Do not delete ../.. or ..\..
	for(YSSIZE_T idx=0; idx<edit.Strlen(); ++idx)
	{
		if(0<idx && 
		   (edit[idx]=='/' || edit[idx]=='\\') && 
		   edit[idx+1]=='.' && 
		   edit[idx+2]=='.' &&
		   (edit[idx+3]=='/' || edit[idx+3]=='\\' || edit[idx+3]==0))
		{
			YSSIZE_T prevSeparatorPos=-1;
			for(auto search=idx-1; 0<=search; --search)
			{
				if(edit[search]=='/' || edit[search]=='\\')
				{
					prevSeparatorPos=search;
					break;
				}
			}
			// If not found, assume that edit[-1] is a separator.

			if(prevSeparatorPos==idx-3 && edit[prevSeparatorPos+1]=='.' && edit[prevSeparatorPos+2]=='.')
			{
				// It is ../.. or ..\.. situation.  Do nothing.
				continue;
			}

			// Otherwise, delete all the way from prevSeparatorPos+1 to idx+3
			if(0!=edit[idx+3])
			{
				edit.Delete((int)idx+3);
			}
			for(auto delPtr=idx+2; prevSeparatorPos<delPtr; --delPtr)
			{
				edit.Delete((int)delPtr);
				worked=YSOK;
			}

			idx=prevSeparatorPos+1;
		}
	}

	// Change /./ into /
	for(int idx=0; idx<edit.Strlen()-2; ++idx)
	{
		if(('/'==edit[idx] || '\\'==edit[idx]) &&
		   '.'==edit[idx+1] &&
		   ('/'==edit[idx+2] || '\\'==edit[idx+2]))
		{
			edit.Delete(idx);
			edit.Delete(idx);
			idx-=2;
			worked=YSOK;
		}
	}

	// Delete .\ or ./ upfront
	while(edit[0]=='.' && (edit[1]=='/' || edit[1]=='\\'))
	{
		edit.Delete(0);
		edit.Delete(0);
		worked=YSOK;
	}

	// Delete last \. or \.
	while(edit.LastChar()=='.' && (edit[edit.Strlen()-2]=='/' || edit[edit.Strlen()-2]=='\\'))
	{
		edit.BackSpace();
		edit.BackSpace();
		worked=YSOK;
	}


	if(YSOK==worked)
	{
		*this=edit;
	}
	return worked;
}

template <class CHARTYPE>
inline YSRESULT YsGenericString <CHARTYPE>::SetUTF8String(const char in[])
{
	if(NULL!=in)
	{
		YSSIZE_T l=strlen(in);
		Resize(l+1);

		YSSIZE_T nWChar=YsUTF8ToWChar <CHARTYPE> (l+1,vv,in);  // nWChar includes zero terminator.
		Resize(nWChar);
	}
	else
	{
		CHARTYPE empty[1]={0};
		Set(empty);
	}

	return YSOK;
}

template <class CHARTYPE>
inline const CHARTYPE *YsGenericString <CHARTYPE>::FgetsUTF8(FILE *fp)
{
	const YSSIZE_T bufSize=256;

	char buf[bufSize];
	if(NULL==fgets(buf,bufSize-1,fp))
	{
		return NULL;
	}

	size_t l=strlen(buf);
	if('\n'==buf[l-1])
	{
		buf[l-1]=0;
		SetUTF8String(buf);
		return vv;
	}

	YsArray <char,256> utf8;
	utf8.Set(l,buf);
	while(fgets(buf,bufSize-1,fp)!=NULL)
	{
		size_t l=strlen(buf);
		utf8.Append(l,buf);
		if(buf[l-1]=='\n')
		{
			utf8[utf8.GetN()-1]=0;
			SetUTF8String(utf8);
			return vv;
		}
	}

	if(utf8.GetN()==0)
	{
		MakeUnitLength(0);
		return NULL;
	}

	utf8.Append(0);
	SetUTF8String(utf8);
	return vv;
}

template <class CHARTYPE>
inline YSBOOL YsGenericString <CHARTYPE>::FindWord(YSSIZE_T *firstIndex,const CHARTYPE keyword[]) const
{
	YSSIZE_T keywordL=Strlen(keyword);
	for(YSSIZE_T i=0; i<=Strlen()-keywordL; i++)
	{
		for(YSSIZE_T j=0; 0!=keyword[j]; j++)
		{
			if(vv[i+j]!=keyword[j])
			{
				goto NEXTI;
			}
		}
		if(NULL!=firstIndex)
		{
			*firstIndex=i;
		}
		return YSTRUE;
	NEXTI:
		;
	}

	if(NULL!=firstIndex)
	{
		*firstIndex=-1;
	}
	return YSFALSE;
}

template <class CHARTYPE>
inline YSBOOL YsGenericString<CHARTYPE>::FINDWORD(YSSIZE_T *firstIndex,const CHARTYPE keyword[]) const
{
	YSSIZE_T keywordL=Strlen(keyword);
	for(YSSIZE_T i=0; i<=Strlen()-keywordL; i++)
	{
		for(YSSIZE_T j=0; 0!=keyword[j]; j++)
		{
			auto a=(('a'<=vv[i+j] && vv[i+j]<='z') ? (vv[i+j]+'A'-'a') : vv[i+j]);
			auto b=(('a'<=keyword[j] && keyword[j]<='z') ? (keyword[j]+'A'-'a') : keyword[j]);
			if(a!=b)
			{
				goto NEXTI;
			}
		}
		if(NULL!=firstIndex)
		{
			*firstIndex=i;
		}
		return YSTRUE;
	NEXTI:
		;
	}

	if(NULL!=firstIndex)
	{
		*firstIndex=-1;
	}
	return YSFALSE;
}

template <class CHARTYPE>
inline YSBOOL YsGenericString<CHARTYPE>::FindDelimitedWord(YSSIZE_T *firstIndex,const CHARTYPE keyword[]) const
{
	YSSIZE_T keywordL=Strlen(keyword);
	int state=0;  // 0: Non-alphabet, non-numeric character   1:Alphabetical or numeric character

	for(YSSIZE_T i=0; i<=Strlen()-keywordL; ++i)
	{
		if(0==state)
		{
			for(YSSIZE_T j=0; j<keywordL; ++j)
			{
				if(vv[i+j]!=keyword[j])
				{
					goto NEXTI;
				}
			}
			if(('a'<=vv[i+keywordL] && vv[i+keywordL]<='z') ||
			   ('A'<=vv[i+keywordL] && vv[i+keywordL]<='Z') ||
			   ('0'<=vv[i+keywordL] && vv[i+keywordL]<='9'))
			{
				goto NEXTI;
			}

			if(nullptr!=firstIndex)
			{
				*firstIndex=i;
			}
			return YSTRUE;

		NEXTI:
			if(('a'<=vv[i] && vv[i]<='z') ||
			   ('A'<=vv[i] && vv[i]<='Z') ||
			   ('0'<=vv[i] && vv[i]<='9'))
			{
				state=1;
			}
		}
		else
		{
			if((vv[i]<'a' || 'z'<vv[i]) &&
			   (vv[i]<'A' || 'Z'<vv[i]) &&
			   (vv[i]<'0' || '9'<vv[i]))
			{
				state=0;
			}
		}
	}

	if(nullptr!=firstIndex)
	{
		*firstIndex=-1;
	}
	return YSFALSE;
}

template <class CHARTYPE>
inline YSBOOL YsGenericString <CHARTYPE>::MatchSearchKeyWord(const CHARTYPE searchText[]) const
{
	YsGenericString <CHARTYPE> txt(searchText);
	return MatchSearchKeyWord(txt);
}

template <class CHARTYPE>
inline YSBOOL YsGenericString <CHARTYPE>::MatchSearchKeyWord(const YsGenericString <CHARTYPE> &searchText) const
{
	YsArray <YsGenericString <CHARTYPE> > args;
	const CHARTYPE blank[3]={' ','\t',0};
	const CHARTYPE comma[3]={',',';',0};

	searchText.Arguments(args,blank,comma);

	for(YSSIZE_T argIdx=0; argIdx<args.GetN(); ++argIdx)
	{
		if(YSTRUE!=FindWord(NULL,args[argIdx]))
		{
			return YSFALSE;
		}
	}
	return YSTRUE;
}

template <class CHARTYPE>
inline YSRESULT YsGenericString<CHARTYPE>::ConvertEscape(void) const
{
	YSRESULT res=YSOK;
	auto len=Strlen();
	for(auto i=len-2; 0<=i; --i)
	{
		if('\\'==vv[i])
		{
			if('n'==vv[i+1])
			{
				vv[i]='\n';
			}
			else if('a'==vv[i+1])
			{
				vv[i]='\a';
			}
			else if('r'==vv[i+1])
			{
				vv[i]='\r';
			}
			else if('t'==vv[i+1])
			{
				vv[i]='\t';
			}
			else if('\\'==vv[i+1])
			{
				// vv[i]='\\';
			}
			else
			{
				res=YSERR;
			}
			for(auto j=i+1; j<len; ++j)
			{
				vv[j]=vv[j+1];
			}
		}
	}
	return res;
}

template <class CHARTYPE>
inline YSRESULT YsGenericString<CHARTYPE>::EncodeEscape(void)
{
	YsGenericString<CHARTYPE> encoded;
	for(auto c : *this)
	{
		if('\n'==c)
		{
			encoded.push_back('\\');
			encoded.push_back('n');
		}
		else if('\a'==c)
		{
			encoded.push_back('\\');
			encoded.push_back('a');
		}
		else if('\t'==c)
		{
			encoded.push_back('\\');
			encoded.push_back('t');
		}
		else if('\r'==c)
		{
			encoded.push_back('\\');
			encoded.push_back('r');
		}
		else if('\"'==c)
		{
			encoded.push_back('\\');
			encoded.push_back('\"');
		}
		else
		{
			encoded.push_back(c);
		}
	}
	this->swap(encoded);
	return YSOK;
}

template <class CHARTYPE>
inline int YsGenericString<CHARTYPE>::Replace(const CHARTYPE fromStr[],const CHARTYPE toStr[])
{
	return ExecReplace(fromStr,toStr,&YsGenericString<CHARTYPE>::Strncmp);
}
template <class CHARTYPE>
inline int YsGenericString<CHARTYPE>::REPLACE(const CHARTYPE fromStr[],const CHARTYPE toStr[])
{
	return ExecReplace(fromStr,toStr,&YsGenericString<CHARTYPE>::STRNCMP);
}
template <class CHARTYPE>
inline int YsGenericString<CHARTYPE>::ExecReplace(
	    const CHARTYPE fromStr[],
	    const CHARTYPE toStr[],
	    int (*cmpfunc)(const CHARTYPE [],const CHARTYPE [],YSSIZE_T))
{
	if(nullptr==fromStr || 0==fromStr[0])
	{
		return 0;
	}

	const CHARTYPE nullStr[]={0};
	if(nullptr==toStr)
	{
		toStr=nullStr;
	}

	auto nFrom=Strlen(fromStr);
	auto nTo=Strlen(toStr);

	int nRepl=0;
	for(YSSIZE_T idx=0; idx<=Strlen()-nFrom; )
	{
		if(0==cmpfunc(vv+idx,fromStr,nFrom))
		{
			if(nFrom<nTo)
			{
				Insert(idx,' ',nTo-nFrom);
			}
			else // The new word is shorter.
			{
				DeleteRange(idx,nFrom-nTo);
			}
			for(YSSIZE_T j=0; j<nTo; ++j)
			{
				vv[idx+j]=toStr[j];
			}
			idx+=nTo;
			++nRepl;
		}
		else
		{
			++idx;
		}
	}

	return nRepl;
}

template <class CHARTYPE>
inline void YsGenericString<CHARTYPE>::Replace(YSSIZE_T FR,YSSIZE_T N,const CHARTYPE INCOMING[])
{
	auto INCOMINGLEN=Strlen(INCOMING);
	if(this->Strlen()<FR)
	{
		FR=Strlen();
	}
	if(this->Strlen()<FR+N)
	{
		N=Strlen()-FR;
	}

	if(N<INCOMINGLEN)
	{
		Insert(FR,' ',INCOMINGLEN-N);
	}
	else
	{
		DeleteRange(FR,N-INCOMINGLEN);
	}
	for(YSSIZE_T j=0; j<INCOMINGLEN; ++j)
	{
		vv[FR+j]=INCOMING[j];
	}
}

template <class CHARTYPE>
int YsGenericString<CHARTYPE>::Atoi(void) const
{
	return (int)Atol();
}

template <class CHARTYPE>
long long int YsGenericString<CHARTYPE>::Atol(void) const
{
	long long int value=0;
	int sign=1;
	long long int ptr=0;
	while(ptr<Strlen() && (vv[ptr]==' ' || vv[ptr]=='\t'))
	{
		++ptr;
	}
	switch(vv[ptr])
	{
	case '+':
		++ptr;
		break;
	case '-':
		sign=-1;
		++ptr;
		break;
	default:
		break;
	}
	value=GetRawNumber<long long int>(ptr);
	if(1!=sign)
	{
		value=-value;
	}
	return value;
}

template <class CHARTYPE>
double YsGenericString<CHARTYPE>::Atof(void) const
{
	return AtoReal<double>();
}
template <class CHARTYPE>
double YsGenericString<CHARTYPE>::Atof64(void) const
{
	return AtoReal<double>();
}
template <class CHARTYPE>
float YsGenericString<CHARTYPE>::Atof32(void) const
{
	return AtoReal<float>();
}

template <class CHARTYPE>
template <class Float_Type>
Float_Type YsGenericString<CHARTYPE>::AtoReal(void) const
{
	long long int aboveDecimal=0,add=1;
	int sign=1;
	long long int ptr=0;
	while(ptr<Strlen() && (vv[ptr]==' ' || vv[ptr]=='\t'))
	{
		++ptr;
	}
	switch(vv[ptr])
	{
	case '+':
		++ptr;
		break;
	case '-':
		sign=-1;
		++ptr;
		break;
	default:
		break;
	}

	aboveDecimal=GetRawNumber<long long int>(ptr);

	long long belowDecimal=0;
	int belowDecimalDenom=1;
	add=1;
	if(ptr<Strlen() && '.'==vv[ptr])
	{
		++ptr;
		while(ptr<Strlen())
		{
			if('0'<=vv[ptr] && vv[ptr]<='9')
			{
				long long int n=vv[ptr]-'0';
				belowDecimal*=10;
				belowDecimal+=n;
				belowDecimalDenom*=10;
			}
			else if(','!=vv[ptr] || '.'==vv[ptr])
			{
				break;
			}
			++ptr;
		}
	}

	double base=(double)aboveDecimal+((double)belowDecimal/(double)belowDecimalDenom);
	if(sign<0)
	{
		base=-base;
	}

	if(ptr<Strlen() && ('e'==vv[ptr] || 'E'==vv[ptr]))
	{
		int expoSign=1;
		++ptr;

		switch(vv[ptr])
		{
		case '+':
			++ptr;
			break;
		case '-':
			expoSign=-1;
			++ptr;
			break;
		default:
			break;
		}

		int expo=GetRawNumber<int>(ptr);
		double scaled;
		if(expo<20)
		{
			long long int scale=1;
			for(int i=0; i<expo; ++i)
			{
				scale*=10;
			}
			scaled=(Float_Type)scale;
		}
		else
		{
			scaled=(Float_Type)1;
			for(int i=0; i<expo; ++i)
			{
				scaled*=(Float_Type)10;
			}
		}
		if(0<expoSign)
		{
			return (Float_Type)(base*scaled);
		}
		else
		{
			return (Float_Type)(base/scaled);
		}
	}
	else
	{
		return (Float_Type)base;
	}
}

template <class CHARTYPE>
template <class Int_Type>
Int_Type YsGenericString<CHARTYPE>::GetRawNumber(long long int &ptr) const
{
	Int_Type sum=0;
	while(ptr<Strlen())
	{
		if('0'<=vv[ptr] && vv[ptr]<='9')
		{
			Int_Type n=vv[ptr]-'0';
			sum*=10;
			sum+=n;
		}
		else if(','!=vv[ptr] || '.'==vv[ptr])
		{
			break;
		}
		++ptr;
	}
	return sum;
}





////////////////////////////////////////////////////////////





/*! Class for storing and manipulating C string. */
class YsString : public YsGenericString <char>
{
public:
	using YsGenericString <char>::GetExtension;
	using YsGenericString <char>::npos;

	/*! Default constructor.  Constructs an empty string. */
	inline YsString();

	/*! Creates a copy of the incoming string. 
	    \param from [In] Incoming (source) string
	    */
	inline YsString(const char from[]);

	/*! Creates a copy of the incoming string. 
	    \param from [In] Incoming (source) string
	    */
	inline YsString(const YsString &from);

	/*! Copy operator. */
	inline YsString &operator=(const YsString &from);

	/*! Move constructor. */
	inline YsString(YsString &&from);

	/*! Move operator. */
	inline YsString &operator=(YsString &&str);

	/*! Extracts the file extension. 
	    '.' will be included in the returned extension.  The ext will be like ".cpp" */
	inline YsString GetExtension(void) const;

	/*! Extracts a subset of the string.  If top is beyond the length of this string, the returned string will be empty.
	    If top+length is beyond the length of this string, the returned length will be truncated. */
	inline YsString Subset(YSSIZE_T top,YSSIZE_T length=npos) const;

	/*! Extracts a subset of the string and drops the head and tail spaces.  
	    If top is beyond the length of this string, the returned string will be empty.
	    If top+length is beyond the length of this string, the returned length will be truncated. */
	inline YsString CleanSubset(YSSIZE_T top,YSSIZE_T length=npos) const;

	/*! Moves "incoming" into this string. */
	inline YsString &MoveFrom(YsString &incoming);

	/*! Decompose the string into words separated by the blank and comma characters.
	    \param args [Out] Decomposed words
	    \param blank [In] Blank (separator) characters
	    \param comma [In] Comma (separator) characters
	    */
	template <const int N>
	inline YSRESULT Arguments(YsArray <YsString,N> &args,const char *blank=" \t",const char *comma=",") const;

	/*! Decompose the string into words separated by the blank and comma characters.
	    \param blank [In] Blank (separator) characters
	    \param comma [In] Comma (separator) characters
	    */
	inline YsArray <YsString> Argv(const char *blank=" \t",const char *comma=",") const;

	/*! This function assumes that this string is a file name with directory path, and separate it into
	    directory part and file-name part.
	    \param path [Out] Returns directory part
	    \param name [Out] Returns file-name part
	    */
	inline YSRESULT SeparatePathFile(YsString &path,YsString &name) const;

	/*! Deletes spaces, tabs, and new-line characters at the end of the string. */
	inline void DeleteTailSpace(void);

	/*! Turns small letters ('a' to 'z') into capital letters ('A' to 'Z'). */
	inline void Capitalize(void);

	/*! Turns capital letters ('A' to 'Z') into small letters ('a' to 'z'). */
	inline void Uncapitalize(void);

	/*! Encodes incoming zero-terminated UNICODE string to UTF8 string.
	    \param wStr [In] Incoming UNICODE string
	    */
	template <class WCHARTYPE>
	inline void EncodeUTF8(const WCHARTYPE wStr[]);

	/*! Read a line from a file. */
	inline const char *Fgets(FILE *fp);

	/*! Makes a printf-formatted string.  It re-sizes itself according to
	    the output and is a good replacement for sprintf. */
	int Printf(const char *fom,...);

	/*! For better interoperability with STL */
	inline YsString substr(YSSIZE_T pos=0,YSSIZE_T len=npos)
	{
		YsString str;
		this->GetSubset(str,pos,len);
		return str;
	}
	/*! For better interoperability with STL */
	inline YsString &operator+=(const YsGenericString<char> &incoming)
	{
		this->Append(incoming);
		return *this;
	}
	/*! For better interoperability with STL */
	inline YsString &operator+=(const char incoming[])
	{
		this->Append(incoming);
		return *this;
	}
	/*! For better interoperability with STL */
	inline YsString operator+(const YsString &incoming) const
	{
		auto copy=*this;
		copy.Append(incoming);
		return copy;
	}
	/*! For better interoperability with STL */
	inline YsString operator+(const char incoming[]) const
	{
		auto copy=*this;
		copy.Append(incoming);
		return copy;
	}
	/*! For better interoperability with STL */
	inline YsString &replace(YSSIZE_T pos,YSSIZE_T len,const YsString &str)
	{
		if(this==&str)
		{
			auto copy=str;
			return replace(pos,len,copy);
		}
		else
		{
			Replace(pos,len,str);
			return *this;
		}
	}
	/*! For better interoperability with STL */
	inline YsString &replace(YSSIZE_T pos,YSSIZE_T len,const char str[])
	{
		if(data()==str)
		{
			YsString copy(str);
			return replace(pos,len,copy);
		}
		else
		{
			Replace(pos,len,str);
			return *this;
		}
	}
};

template <class CHARTYPE>
inline YSRESULT YsGenericString <CHARTYPE>::GetUTF8String(class YsString &outStr) const
{
	outStr.EncodeUTF8 <CHARTYPE> (vv);
	return YSOK;
}

inline YsString::YsString()
{
}

inline YsString::YsString(const char from[])  // Isn't it automatic?
{
	Set(from);
}

inline YsString::YsString(const YsString &from)
{
	YsArray <char,16>::Set(from.Strlen()+1,from.GetArray());
}

inline YsString &YsString::operator=(const YsString &from)
{
	YsGenericString <char>::Set(from);
	return *this;
}

inline YsString YsString::GetExtension(void) const
{
	YsString ext;
	YsGenericString <char>::GetExtension(ext);
	return ext;
}

inline YsString YsString::Subset(YSSIZE_T top,YSSIZE_T length) const
{
	YsString sub;
	YsGenericString <char>::GetSubset(sub,top,length);
	return sub;
}

inline YsString YsString::CleanSubset(YSSIZE_T top,YSSIZE_T length) const
{
	auto sub=Subset(top,length);
	sub.DeleteTailSpace();
	sub.DeleteHeadSpace();
	return sub;
}

inline YsString::YsString(YsString &&from)
{
	MoveFrom(from);
}

inline YsString &YsString::operator=(YsString &&str)
{
	MoveFrom(str);
	return *this;
}

inline YsString &YsString::MoveFrom(YsString &incoming)
{
	YsGenericString <char>::MoveFrom(incoming); // Is supposed to fall back to YsArray <char,16>::MoveFrom
	return *this;
}

template <const int N>
inline YSRESULT YsString::Arguments(YsArray <YsString,N> &args,const char *blank,const char *comma) const
{
	YsGenericString <char>::Arguments <YsString,N> (args,blank,comma);
	return YSOK;
}

inline YsArray <YsString> YsString::Argv(const char *blank,const char *comma) const
{
	YsArray <YsString> argv;
	Arguments(argv,blank,comma);
	return argv;
}

inline YSRESULT YsString::SeparatePathFile(YsString &pth,YsString &fil) const
{
	int cutPtr,seekPtr;

	cutPtr=0;

	/* Skip Drive Name */
	if(vv[0]!=0 && vv[1]==':')
	{
		cutPtr+=2;
	}

	/* scan until nul Stopper */
	for(seekPtr=cutPtr; vv[seekPtr]!=0; seekPtr++)
	{
		if((vv[seekPtr]&0x80)!=0 && vv[seekPtr+1]!=0)  // 2 byte char
		{
			seekPtr++;
		}
		else if(vv[seekPtr]=='\\' || vv[seekPtr]=='/' || vv[seekPtr]==':')
		{
			cutPtr=seekPtr+1;
		}
	}

	/* cutPtr points *tmp  or after ':' or after last '\\' */
	fil.Set(vv+cutPtr);

	int i;
	pth.Resize(cutPtr+1);
	for(i=0; i<cutPtr; i++)
	{
		pth.vv[i]=vv[i];
	}
	pth.vv[cutPtr]=0;

	return YSOK;
}

inline void YsString::DeleteTailSpace(void)
{
	unsigned int chr;
	YSSIZE_T i,cut,len;

	len=Strlen();
	cut=0;
	for(i=0; i<len; i++)
	{
		chr=((unsigned char *)vv)[i];

		if((0x81<=chr && chr<=0x9f) || 0xe0<=chr)
		{
			cut=i+2;
			i++;
		}
		else if(isprint(chr) && chr!=' ' && chr!='\t')
		{
			cut=i+1;
		}
	}

	if(cut<len)
	{
		vv[cut]=0;
		Resize(cut+1);
	}
}

inline void YsString::Capitalize(void)
{
	for(YSSIZE_T i=0; vv[i]!=0; i++)
	{
		if((0x81<=((unsigned char*)vv)[i] && ((unsigned char*)vv)[i]<=0x9f) || 0xe0<=((unsigned char*)vv)[i])
		{
			i++;
			if(vv[i]==0)
			{
				break;
			}
		}
		else if('a'<=vv[i] && vv[i]<='z')
		{
			vv[i]=vv[i]+'A'-'a';
		}
	}
}

inline void YsString::Uncapitalize(void)
{
	for(YSSIZE_T i=0; vv[i]!=0; i++)
	{
		if((0x81<=((unsigned char*)vv)[i] && ((unsigned char*)vv)[i]<=0x9f) || 0xe0<=((unsigned char*)vv)[i])
		{
			i++;
			if(vv[i]==0)
			{
				break;
			}
		}
		else if('A'<=vv[i] && vv[i]<='Z')
		{
			vv[i]=vv[i]+'a'-'A';
		}
	}
}

template <class WCHARTYPE>
inline void YsString::EncodeUTF8(const WCHARTYPE wStr[])
{
	if(NULL!=wStr)
	{
		YSSIZE_T l=0;
		for(l=0; wStr[l]!=0; l++)
		{
		}

		size_t lOut=l*6+1;  // Worst case six-byte encoding times N wchars plus terminator.
		YsArray <char,16>::Resize(lOut);

		size_t nByte=YsWCharToUTF8(lOut,vv,wStr); // nByte includes zero terminator.
		YsArray <char,16>::Resize(nByte);
	}
	else
	{
		Set("");
	}
}

inline bool operator==(const YsString &str1,const YsString &str2)
{
	return (strcmp(str1,str2)==0);
}

inline bool operator!=(const YsString &str1,const YsString &str2)
{
	return (strcmp(str1,str2)!=0);
}

inline bool operator<(const YsString &str1,const YsString &str2)
{
	// str1<str2   <=>  str1-str2<0
	return (strcmp(str1,str2)<0);
}

inline bool operator>(const YsString &str1,const YsString &str2)
{
	// str1>str2   <=>  str1-str2>0
	return (strcmp(str1,str2)>0);
}

inline bool operator<=(const YsString &str1,const YsString &str2)
{
	return (strcmp(str1,str2)<=0);
}

inline bool operator>=(const YsString &str1,const YsString &str2)
{
	return (strcmp(str1,str2)>=0);
}

inline const char *YsString::Fgets(FILE *fp)
{
	char buf[256];
	YsArray <char,16>::Set(0,NULL);
	while(fgets(buf,255,fp)!=NULL)
	{
		YSBOOL eol=YSFALSE;

		int len=(int)strlen(buf);
		while(0<len && (0x0d==buf[len-1] || 0x0a==buf[len-1]))
		{
			eol=YSTRUE;
			buf[len-1]=0;
			--len;
		}

		if(YSTRUE==eol)
		{
			YsArray <char,16>::Append(len+1,buf);
			break;
		}
		else
		{
			YsArray <char,16>::Append(len,buf);
		}
	}

	if(GetN()==0)
	{
		Set("");
		return NULL;
	}

	if(vv[GetN()-1]!=0)  // In case /n is not at the end of the file 2006/10/26
	{
		YsArray <char,16>::Append(0);
	}

	return vv;
}

////////////////////////////////////////////////////////////

/*! Class for storing and manipulating wide-character (UNICODE) string. */
class YsWString : public YsGenericString <wchar_t>
{
public:
	using YsGenericString <wchar_t>::GetExtension;
	using YsGenericString <wchar_t>::npos;

	/*! Default constructor.  Creates an empty string. */
	inline YsWString();

	/*! Creates a copy of the incoming string.
	    \param from [In] Incoming string.
	    */
	inline YsWString(const wchar_t from[]);

	/*! Creates a copy of the incoming string.
	    \param from [In] Incoming string.
	    */
	inline YsWString(const YsWString &from);

	/*! Copy operator. */
	inline YsWString &operator=(const YsWString &from);

	/*! Move constructor.
	    Question is if the move constructor of YsArray <wchar_t> is automatically used if I do not make a specific move constructor here.  I need more study. 
	    Experiment with VS2012 implies the move constructor/operator of the base class will not be automatically used for a sub-class, even though the sub-class has no specific data members.  So, probably I need this function. */
	inline YsWString(YsWString &&from);

	/*! Move operator.  Same question as move constructor remains. */
	inline YsWString &operator=(YsWString &&from);

	/*! Moves a string from incoming string. */
	inline YsWString &MoveFrom(YsWString &from);

	/*! Decompose the string into words separated by the blank and comma characters.
	    \param args [Out] Decomposed words
	    \param blank [In] Blank (separator) characters
	    \param comma [In] Comma (separator) characters
	    */
	template <const int N>
	inline YSRESULT Arguments(YsArray <YsWString,N> &args,const wchar_t *blank=L" \t",const wchar_t *comma=L",") const;

	/*! Decompose the string into words separated by the blank and comma characters.
	    \param blank [In] Blank (separator) characters
	    \param comma [In] Comma (separator) characters
	    */
	inline YsArray <YsWString> Argv(const wchar_t *blank=L" \t",const wchar_t *comma=L",") const;

	/*! Extracts the file-extension part of the file name. 
	    '.' will be included in the returned extension.  The ext will be like ".cpp" */
	inline YsWString GetExtension(void) const;

	/*! Extracts a subset of the string.  If top is beyond the length of this string, the returned string will be empty.
	    If top+length is beyond the length of this string, the returned length will be truncated. */
	inline YsWString Subset(YSSIZE_T top,YSSIZE_T length=npos) const;

	/*! Extracts a subset of the string and removes head and tail spaces.  
	    If top is beyond the length of this string, the returned string will be empty.
	    If top+length is beyond the length of this string, the returned length will be truncated. */
	inline YsWString CleanSubset(YSSIZE_T top,YSSIZE_T length=npos) const;

	/*! Returns UTF8 string. */
	inline YsString GetUTF8String(void) const;

	/*! For better interoperability with STL */
	inline YsWString substr(YSSIZE_T pos=0,YSSIZE_T len=npos)
	{
		YsWString str;
		this->GetSubset(str,pos,len);
		return str;
	}
	/*! For better interoperability with STL */
	inline YsWString &operator+=(const YsGenericString<wchar_t> &incoming)
	{
		this->Append(incoming);
		return *this;
	}
	/*! For better interoperability with STL */
	inline YsWString &operator+=(const wchar_t incoming[])
	{
		this->Append(incoming);
		return *this;
	}
	/*! For better interoperability with STL */
	inline YsWString operator+(const YsWString &incoming) const
	{
		auto copy=*this;
		copy.Append(incoming);
		return copy;
	}
	/*! For better interoperability with STL */
	inline YsWString operator+(const wchar_t incoming[]) const
	{
		auto copy=*this;
		copy.Append(incoming);
		return copy;
	}
	/*! For better interoperability with STL */
	inline YsWString &replace(YSSIZE_T pos,YSSIZE_T len,const YsWString &str)
	{
		if(this==&str)
		{
			auto copy=str;
			return replace(pos,len,copy);
		}
		else
		{
			Replace(pos,len,str);
			return *this;
		}
	}
	/*! For better interoperability with STL */
	inline YsWString &replace(YSSIZE_T pos,YSSIZE_T len,const wchar_t str[])
	{
		if(data()==str)
		{
			YsWString copy(str);
			return replace(pos,len,copy);
		}
		else
		{
			Replace(pos,len,str);
			return *this;
		}
	}
};

inline YsWString::YsWString()
{
}

inline YsWString::YsWString(const wchar_t from[])
{
	Set(from);
}

inline YsWString::YsWString(const YsWString &from)
{
	Set(from);
}

inline YsWString &YsWString::operator=(const YsWString &from)
{
	YsGenericString <wchar_t>::Set(from);
	return *this;
}

inline YsWString::YsWString(YsWString &&from)
{
	MoveFrom(from);
}

inline YsWString &YsWString::operator=(YsWString &&from)
{
	MoveFrom(from);
	return *this;
}

inline YsWString &YsWString::MoveFrom(YsWString &from)
{
	YsGenericString <wchar_t>::MoveFrom(from); // Is supposed to fall back to YsArray <wchar_t,16>::MoveFrom
	return *this;
}

template <const int N>
inline YSRESULT YsWString::Arguments(YsArray <YsWString,N> &args,const wchar_t *blank,const wchar_t *comma) const
{
	return YsGenericString <wchar_t>::Arguments <YsWString,N> (args,blank,comma);
}

inline YsArray <YsWString> YsWString::Argv(const wchar_t *blank,const wchar_t *comma) const
{
	YsArray <YsWString> argv;
	Arguments(argv,blank,comma);
	return argv;
}

inline YsWString YsWString::GetExtension(void) const
{
	YsWString ext;
	YsGenericString <wchar_t>::GetExtension(ext);
	return ext;
}

inline YsWString YsWString::Subset(YSSIZE_T top,YSSIZE_T length) const
{
	YsWString sub;
	YsGenericString <wchar_t>::GetSubset(sub,top,length);
	return sub;
}

inline YsWString YsWString::CleanSubset(YSSIZE_T top,YSSIZE_T length) const
{
	auto sub=Subset(top,length);
	sub.DeleteTailSpace();
	sub.DeleteHeadSpace();
	return sub;
}

inline YsString YsWString::GetUTF8String(void) const
{
	YsString utf8;
	utf8.EncodeUTF8<wchar_t>(*this);
	return utf8;
}

inline bool operator==(const YsWString &str1,const YsWString &str2)
{
	return (YsWString::Strcmp(str1,str2)==0);
}

inline bool operator!=(const YsWString &str1,const YsWString &str2)
{
	return (YsWString::Strcmp(str1,str2)!=0);
}

inline bool operator<(const YsWString &str1,const YsWString &str2)
{
	// str1<str2   <=>  str1-str2<0
	return (YsWString::Strcmp(str1,str2)<0);
}

inline bool operator>(const YsWString &str1,const YsWString &str2)
{
	// str1>str2   <=>  str1-str2>0
	return (YsWString::Strcmp(str1,str2)>0);
}

inline bool operator<=(const YsWString &str1,const YsWString &str2)
{
	return (YsWString::Strcmp(str1,str2)<=0);
}

inline bool operator>=(const YsWString &str1,const YsWString &str2)
{
	return (YsWString::Strcmp(str1,str2)>=0);
}

////////////////////////////////////////////////////////////

template <class strType,class assocType>
YSRESULT YsQuickSortString(YSSIZE_T n,strType const *keyValue[],assocType assocValue[],YSBOOL watch=YSFALSE)
{
	if(n<=1)
	{
		return YSOK;
	}

	if(watch==YSTRUE)
	{
		YsPrintf("YsQuickSortString (YsString *) %d\n",n);
	}

	YSSIZE_T i,dividePoint;
	int dif;
	const strType *divider,*nonDivider;
	YSBOOL allEqual;
	divider=keyValue[n/2];  // Can use random number.
	nonDivider=NULL;

TRYAGAIN:
	dividePoint=0;
	allEqual=YSTRUE;
	for(i=0; i<n; i++)
	{
		dif=strType::Strcmp(*keyValue[i],*divider);
		if(dif<0)
		{
			if(i!=dividePoint)
			{
				YsSwapSomething <const strType *> (keyValue[dividePoint],keyValue[i]);
				if(assocValue!=NULL)
				{
					YsSwapSomething <assocType> (assocValue[dividePoint],assocValue[i]);
				}
			}
			dividePoint++;
		}
		if(dif!=0)
		{
			nonDivider=keyValue[i];
			allEqual=YSFALSE;
		}
	}

	if(allEqual==YSTRUE)
	{
		return YSOK;
	}

	if(dividePoint==0 && nonDivider!=NULL)
	{
		divider=nonDivider;
		goto TRYAGAIN;
	}

	if(assocValue!=NULL)
	{
		YsQuickSortString <strType,assocType> (dividePoint,keyValue,assocValue);
		YsQuickSortString <strType,assocType> ((n-dividePoint),keyValue+dividePoint,assocValue+dividePoint);
	}
	else
	{
		YsQuickSortString <strType,assocType> (dividePoint,keyValue,NULL);
		YsQuickSortString <strType,assocType> ((n-dividePoint),keyValue+dividePoint,NULL);
	}

	return YSOK;
}

template <class strType,class assocType>
YSRESULT YsQuickSortString(YSSIZE_T n,strType keyValue[],assocType assocValue[],YSBOOL watch=YSFALSE)
{
	if(n<=1)
	{
		return YSOK;
	}

	if(watch==YSTRUE)
	{
		YsPrintf("YsQuickSortString (YsString *) %d\n",(int)n);
	}

	YSSIZE_T i,dividePoint;
	int dif;

	/* Bug: 2014/11/01 YsSwapSomething uses move operator (&&).  Therefore, YsString or YsString's internal pointers
	     are swapped, but the pointer to YsString or YsWString stays the same.
	     It means that even if divider stays constant, the content of divider changes after YsSwapSomething. 

	     But, I don't want to make a copy here.

       Solution: When divider is one of what's swapped, also swap divider pointer.
                 Do the same thing for nonDivider.
         */
	YSBOOL allEqual;
	strType *divider=&keyValue[n/2];  // Can use random number.
	strType *nonDivider=NULL;

TRYAGAIN:
	dividePoint=0;
	allEqual=YSTRUE;
	for(i=0; i<n; i++)
	{
		dif=strType::Strcmp(keyValue[i],divider->Txt());
		if(dif<0)
		{
			if(i!=dividePoint)
			{
				// See above bug.  When one of keyValue[i] or keyValue[dividePoint] is divider, divider also needs to be swapped.
				// Same thing for nonDivider.
				if(&keyValue[i]==divider)
				{
					divider=&keyValue[dividePoint];
				}
				else if(&keyValue[dividePoint]==divider)
				{
					divider=&keyValue[i];
				}
				if(&keyValue[i]==nonDivider)
				{
					nonDivider=&keyValue[dividePoint];
				}
				else if(&keyValue[dividePoint]==nonDivider)
				{
					nonDivider=&keyValue[i];
				}

				YsSwapSomething <strType> (keyValue[dividePoint],keyValue[i]);
				if(assocValue!=NULL)
				{
					YsSwapSomething <assocType> (assocValue[dividePoint],assocValue[i]);
				}
			}
			dividePoint++;
		}
		if(dif!=0)
		{
			nonDivider=&keyValue[i];
			allEqual=YSFALSE;
		}
	}

	if(allEqual==YSTRUE)
	{
		return YSOK;
	}

	if(dividePoint==0 && nonDivider!=NULL)
	{
		divider=nonDivider;
		goto TRYAGAIN;
	}

	if(assocValue!=NULL)
	{
		YsQuickSortString <strType,assocType> (dividePoint,keyValue,assocValue);
		YsQuickSortString <strType,assocType> ((n-dividePoint),keyValue+dividePoint,assocValue+dividePoint);
	}
	else
	{
		YsQuickSortString <strType,assocType> (dividePoint,keyValue,NULL);
		YsQuickSortString <strType,assocType> ((n-dividePoint),keyValue+dividePoint,NULL);
	}

	return YSOK;
}

template <class assocType>
YSRESULT YsQuickSortString(YSSIZE_T n,YsString const *keyValue[],assocType assocValue[],YSBOOL watch=YSFALSE)
{
	if(n<=1)
	{
		return YSOK;
	}

	if(watch==YSTRUE)
	{
		YsPrintf("YsQuickSortString (YsString *) %d\n",n);
	}

	YSSIZE_T i,dividePoint;
	int dif;
	const YsString *divider,*nonDivider;
	YSBOOL allEqual;
	divider=keyValue[n/2];  // Can use random number.
	nonDivider=NULL;

TRYAGAIN:
	dividePoint=0;
	allEqual=YSTRUE;
	for(i=0; i<n; i++)
	{
		dif=YsString::Strcmp(*keyValue[i],*divider);
		if(dif<0)
		{
			if(i!=dividePoint)
			{
				YsSwapSomething <const YsString *> (keyValue[dividePoint],keyValue[i]);
				if(assocValue!=NULL)
				{
					YsSwapSomething <assocType> (assocValue[dividePoint],assocValue[i]);
				}
			}
			dividePoint++;
		}
		if(dif!=0)
		{
			nonDivider=keyValue[i];
			allEqual=YSFALSE;
		}
	}

	if(allEqual==YSTRUE)
	{
		return YSOK;
	}

	if(dividePoint==0 && nonDivider!=NULL)
	{
		divider=nonDivider;
		goto TRYAGAIN;
	}

	if(assocValue!=NULL)
	{
		YsQuickSortString <assocType> (dividePoint,keyValue,assocValue);
		YsQuickSortString <assocType> ((n-dividePoint),keyValue+dividePoint,assocValue+dividePoint);
	}
	else
	{
		YsQuickSortString <assocType> (dividePoint,keyValue,NULL);
		YsQuickSortString <assocType> ((n-dividePoint),keyValue+dividePoint,NULL);
	}

	return YSOK;
}

template <class assocType>
YSRESULT YsQuickSortString(YSSIZE_T n,char const *keyValue[],assocType assocValue[],YSBOOL watch=YSFALSE)
{
	if(n<=1)
	{
		return YSOK;
	}

	YSSIZE_T i,dividePoint;
	int dif;
	const char *divider,*nonDivider;
	YSBOOL allEqual;
	divider=keyValue[n/2];  // Can use random number.
	nonDivider=NULL;

	if(watch==YSTRUE)
	{
		YsPrintf("YsQuickSortString (const char *) %d\n",(int)n);
	}

TRYAGAIN:
	dividePoint=0;
	allEqual=YSTRUE;
	for(i=0; i<n; i++)
	{
		dif=strcmp(keyValue[i],divider);
		if(dif<0)
		{
			if(i!=dividePoint)
			{
				YsSwapSomething <const char *> (keyValue[dividePoint],keyValue[i]);
				if(assocValue!=NULL)
				{
					YsSwapSomething <assocType> (assocValue[dividePoint],assocValue[i]);
				}
			}
			dividePoint++;
		}
		if(dif!=0)
		{
			nonDivider=keyValue[i];
			allEqual=YSFALSE;
		}
	}

	if(allEqual==YSTRUE)
	{
		return YSOK;
	}

	if(dividePoint==0 && nonDivider!=NULL)
	{
		if(watch==YSTRUE)
		{
			YsPrintf("YsQuickSortString: Trying again...\n");
		}
		divider=nonDivider;
		goto TRYAGAIN;
	}

	if(assocValue!=NULL)
	{
		YsQuickSortString <assocType> (dividePoint,keyValue,assocValue,watch);
		YsQuickSortString <assocType> ((n-dividePoint),keyValue+dividePoint,assocValue+dividePoint,watch);
	}
	else
	{
		YsQuickSortString <assocType> (dividePoint,keyValue,NULL,watch);
		YsQuickSortString <assocType> ((n-dividePoint),keyValue+dividePoint,NULL,watch);
	}

	return YSOK;
}


////////////////////////////////////////////////////////////


template <class Key>
class YsStringComparer
{
public:
	inline static bool Equal(const Key &a,const Key &b)
	{
		return (0==a.Strcmp(b));
	}

	inline static bool AIsSmallerThanB(const Key &a,const Key &b)
	{
		return a.Strcmp(b)>0;
	}

	inline static bool AIsGreaterThanB(const Key &a,const Key &b)
	{
		return a.Strcmp(b)<0;
	}
};


/* } */
#endif
