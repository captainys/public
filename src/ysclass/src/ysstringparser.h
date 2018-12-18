/* ////////////////////////////////////////////////////////////

File Name: ysstringparser.h
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

#ifndef YSSTRINGPARSER_IS_INCLUDED
#define YSSTRINGPARSER_IS_INCLUDED
/* { */

#include "ysstring.h"

template <class STRTYPE>
class YsStringParser
{
private:
	YsArray <STRTYPE> argv;
	YsArray <STRTYPE> separator;
	YsArray <STRTYPE> comma;
	YsArray <STRTYPE> takeAsWord;
	YsArray <STRTYPE> quote;
	YsArray <STRTYPE> inlineComment;
	YsArray <STRTYPE> escape;

	enum STATE
	{
		IN_BLANK,
		IN_QUOTE,
		IN_WORD,
	};

	static YSSIZE_T Match(const typename STRTYPE::CHAR_T strIn[],const STRTYPE &pattern);
	static YSSIZE_T Match(const typename STRTYPE::CHAR_T strIn[],const YsArrayMask <STRTYPE> &patternArray);

public:
	YsStringParser();
	YSRESULT Parse(const typename STRTYPE::CHAR_T strIn[]);
	const YsArray <STRTYPE> &Argv(void) const;

	void SetTakeAsWord(const YsConstArrayMask <STRTYPE> &takeAsWord);
	void SetInlineComment(const YsConstArrayMask <STRTYPE> &inlineComment);
};

template <class STRTYPE>
YSSIZE_T YsStringParser<STRTYPE>::Match(const typename STRTYPE::CHAR_T strIn[],const STRTYPE &pattern)
{
	int i;
	for(i=0; 0!=strIn[i] && i<pattern.Strlen(); ++i)
	{
		if(strIn[i]!=pattern[i])
		{
			break;
		}
	}
	if(i==pattern.Strlen())
	{
		return pattern.Strlen();
	}
	return 0;
}

template <class STRTYPE>
YSSIZE_T YsStringParser<STRTYPE>::Match(const typename STRTYPE::CHAR_T strIn[],const YsArrayMask <STRTYPE> &patternArray)
{
	for(auto &ptn : patternArray)
	{
		auto match=Match(strIn,ptn);
		if(0<match)
		{
			return match;
		}
	}
	return 0;
}

template <>
inline YsStringParser<YsString>::YsStringParser()
{
	separator.Add(" ");
	separator.Add("\t");

	comma.Add(",");
	comma.Add(";");

	takeAsWord.Add("{");
	takeAsWord.Add("}");
	takeAsWord.Add("(");
	takeAsWord.Add(")");
	takeAsWord.Add("[");
	takeAsWord.Add("]");

	quote.Add("\"");
	quote.Add("'");

	escape.Add("\\");
}

template <>
inline YsStringParser<YsWString>::YsStringParser()
{
	separator.Add(L" ");
	separator.Add(L"\t");

	comma.Add(L",");
	comma.Add(L";");

	takeAsWord.Add(L"{");
	takeAsWord.Add(L"}");
	takeAsWord.Add(L"(");
	takeAsWord.Add(L")");
	takeAsWord.Add(L"[");
	takeAsWord.Add(L"]");

	quote.Add(L"\"");
	quote.Add(L"'");

	escape.Add(L"\\");
}


template <class STRTYPE>
YSRESULT YsStringParser<STRTYPE>::Parse(const typename STRTYPE::CHAR_T strIn[])
{
	YSSIZE_T ptr=0;
	STATE state=IN_BLANK;
	STRTYPE closingQuote;
	YSBOOL escapeNext=YSFALSE;

	argv.CleanUp();
	while(strIn[ptr]!=0)
	{
		if(IN_BLANK==state)
		{
			YSSIZE_T sz;
			if(0<(sz=Match(strIn+ptr,separator)))
			{
				ptr+=sz;
			}
			else if(0<(sz=Match(strIn+ptr,comma)))
			{
				argv.Increment();
				argv.Last().CleanUp();
				state=IN_WORD;
				ptr+=sz;
			}
			else if(0<(sz=Match(strIn+ptr,takeAsWord)))
			{
				argv.Increment();
				argv.Last().Set(sz,strIn+ptr);
				ptr+=sz;
			}
			else if(0<(sz=Match(strIn+ptr,quote)))
			{
				argv.Increment();
				argv.Last().CleanUp();
				closingQuote.Set(sz,strIn+ptr);
				state=IN_QUOTE;
				ptr+=sz;
			}
			else if(0<(sz=Match(strIn+ptr,escape)))
			{
				escapeNext=YSTRUE;
				argv.Increment();
				argv.Last().CleanUp();
				state=IN_WORD;
				ptr+=sz;
			}
			else if(0<(sz=Match(strIn+ptr,inlineComment)))
			{
				return YSOK;
			}
			else
			{
				argv.Increment();
				argv.Last().CleanUp();
				argv.Last().Append(strIn[ptr]);
				state=IN_WORD;
				++ptr;
			}
		}
		else if(IN_WORD==state)
		{
			YSSIZE_T sz;
			if(YSTRUE==escapeNext)
			{
				argv.Last().Append(strIn[ptr]);
				++ptr;
				escapeNext=YSFALSE;
			}
			else if(0<(sz=Match(strIn+ptr,separator)))
			{
				state=IN_BLANK;
				ptr+=sz;
			}
			else if(0<(sz=Match(strIn+ptr,comma)))
			{
				argv.Increment();
				argv.Last().CleanUp();
				state=IN_WORD;
				ptr+=sz;
			}
			else if(0<(sz=Match(strIn+ptr,takeAsWord)))
			{
				argv.Increment();
				argv.Last().Set(sz,strIn+ptr);
				ptr+=sz;
				state=IN_BLANK;
			}
			else if(0<(sz=Match(strIn+ptr,quote)))
			{
				argv.Increment();
				argv.Last().CleanUp();
				closingQuote.Set(sz,strIn+ptr);
				state=IN_QUOTE;
			}
			else if(0<(sz=Match(strIn+ptr,escape)))
			{
				escapeNext=YSTRUE;
				ptr+=sz;
			}
			else if(0<(sz=Match(strIn+ptr,inlineComment)))
			{
				return YSOK;
			}
			else
			{
				argv.Last().Append(strIn[ptr]);
				++ptr;
			}
		}
		else if(IN_QUOTE==state)
		{
			YSSIZE_T sz;
			if(YSTRUE==escapeNext)
			{
				argv.Last().Append(strIn[ptr]);
				++ptr;
				escapeNext=YSFALSE;
			}
			else if(0<(sz=Match(strIn+ptr,closingQuote)))
			{
				state=IN_BLANK;
				ptr+=sz;
			}
			else if(0<(sz=Match(strIn+ptr,escape)))
			{
				escapeNext=YSTRUE;
				ptr+=sz;
			}
			else
			{
				argv.Last().Append(strIn[ptr]);
				++ptr;
			}
		}
	}

	return YSOK;
}

template <class STRTYPE>
const YsArray <STRTYPE> &YsStringParser<STRTYPE>::Argv(void) const
{
	return argv;
}

template <class STRTYPE>
void YsStringParser<STRTYPE>::SetTakeAsWord(const YsConstArrayMask <STRTYPE> &takeAsWord)
{
	this->takeAsWord.Set(takeAsWord.GetN(),takeAsWord);
}

template <class STRTYPE>
void YsStringParser<STRTYPE>::SetInlineComment(const YsConstArrayMask <STRTYPE> &inlineComment)
{
	this->inlineComment=inlineComment;
}

/* } */
#endif
