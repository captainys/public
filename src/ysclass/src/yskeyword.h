/* ////////////////////////////////////////////////////////////

File Name: yskeyword.h
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

#ifndef YSKEYWORD_IS_INCLUDED
#define YSKEYWORD_IS_INCLUDED
/* { */

#include "ysstring.h"
#include "yslist2.h"

class YsKeyWord
{
public:
	int id;
	YSSIZE_T attrib;
	class YsString str;
};

/*! YsKeyWordList is for static set of key words.  
    For a dynamic dictionary, in which key words are frequently added and deleted, use YsDictionary defined in ysdict.h instead.
*/
class YsKeyWordList
{
protected:
	YsListAllocator <YsKeyWord> keyWordAlloc;
	YsListContainer <YsKeyWord> keyWordList;
	YsArray <YsKeyWord *> keyWordDict;
public:
	YsKeyWordList();
	void Initialize(void);

	int GetN(void) const;
	YSRESULT MakeList(const char * const str[],const YSSIZE_T attrib[]=NULL);
	YSRESULT AddKeyWord(YSSIZE_T nStr,const char * const str[],const YSSIZE_T attrib[]=NULL);
	YSRESULT AddKeyWord(YSSIZE_T nStr,const YsString str[],const YSSIZE_T attrib[]=NULL);

	/*! Deprecated.  Left for backward compatibility.  Use FindIndex instead. */
	int GetId(const char str[]) const;

	/*! Returns the index to the string.  -1 will be returned if the string is not in the dictionary. */
	int FindIndex(const char str[]) const;

	const char *GetString(int id) const;

	/*! Returns the attribute of the given index. */
	YSSIZE_T GetAttribute(int idx) const;
};

/* } */
#endif
