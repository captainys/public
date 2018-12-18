/* ////////////////////////////////////////////////////////////

File Name: ysdict.h
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

#ifndef YSDICT_IS_INCLUDED
#define YSDICT_IS_INCLUDED
/* { */

#include "ysstring.h"
#include "yshash.h"
#include "yslist2.h"

#include <stdlib.h>

/*! A dictionary that maps a word to an attribute.
    Template parameter stringType needs to be YsString or YsWString.
    */
template <class stringType,class attribType>
class YsDictionary
{
protected:
	class KeyWord
	{
	public:
		YSSIZE_T idx;
		stringType str;
		attribType attrib;
	};
	YsFixedLengthToMultiHashTable <YsListItem <KeyWord> *,1,1> wordHash;
	YsListAllocator <KeyWord> wordAlloc;
	YsListContainer <KeyWord> wordList;

	YSBOOL caseSensitive;
public:
	/*! Default constructor. */
	YsDictionary();

	/*! Default destructor. */
	~YsDictionary();

	/*! Initializes the dictionary. */
	void Initialize(void);

	/*! Set case sensitivity. */
	void SetCaseSensitivity(YSBOOL s);

	/*! Cleans up the dictionary.  Same as Initialize. */
	void CleanUp(void);

	/*! Creates a random word that is not included in the dictionary.
	    The word is not automatically added to the dictionary.  
	    If the calling function decides to use this word, it must subsequently call AddWord function. */
	stringType MakeUniqueWord(const stringType &prefix,const stringType &postfix) const;

	/*! Returns the number of words in the dictionary. */
	YSSIZE_T GetNumWord(void) const;

	/*! Adds a new word to the dictionary.  If the word is already included in the dictionary,
	    this function returns YSERR, and the word will not be added.  Returns YSOK if successful. */
	YSRESULT AddWord(const stringType &str,const attribType &attr);

	/*! Updates the attribute assigned to the word.  If the word is not in the dictionary yet, it adds a word with the given attribute. */
	YSRESULT UpdateAttrib(const stringType &str,const attribType &attr);

	/*! Delete a word from the dictionary.  If the word is not included in the dictionary,
	    it returns YSERR.  Returns YSOK if successful. */
	YSRESULT DeleteWord(const stringType &str);

	/*! Returns YSTRUE if the word [str] is included in the dictionary.  Returns YSERR otherwise. */
	YSBOOL IsWordIncluded(const stringType &str) const;

	/*! Returns a pointer to the attribute associated with the word [str].
	    Returns nullptr if the word is not included in the dictionary. */
	const attribType *FindAttrib(const stringType &str) const;

	/*! Returns an index to of the word [str].  An index is a zero-based number that indicates
	    the order the word is added in the dictionary.  I.E. the first word added in the 
	    dictionary is associated with the index zero.  The index will not change when a word
	    in the middle is deleted. */
	YSSIZE_T FindIndex(const stringType &str) const;

private:
	const YsListItem <KeyWord> *FindKeyWord(const stringType &str) const;
	YsListItem <KeyWord> *FindKeyWord(const stringType &str);

public:
	/*! Returns a hash key for the word [str]. */
	YSHASHKEY CalculateHashKey(const stringType &str) const;
};


template <class stringType,class attribType>
YsDictionary <stringType,attribType>::YsDictionary() : wordList(wordAlloc)
{
	caseSensitive=YSTRUE;
	Initialize();
}

template <class stringType,class attribType>
YsDictionary <stringType,attribType>::~YsDictionary()
{
}

template <class stringType,class attribType>
void YsDictionary <stringType,attribType>::Initialize(void)
{
	CleanUp();
}

template <class stringType,class attribType>
void YsDictionary<stringType,attribType>::SetCaseSensitivity(YSBOOL s)
{
	caseSensitive=s;
}

template <class stringType,class attribType>
void YsDictionary <stringType,attribType>::CleanUp(void)
{
	wordHash.CleanUp();
	wordList.CleanUp();
	wordAlloc.CollectGarbage();
}

template <class stringType,class attribType>
stringType YsDictionary <stringType,attribType>::MakeUniqueWord(const stringType &prefix,const stringType &postfix) const
{
	stringType uniqueWord;
	for(;;)
	{
		uniqueWord=prefix;
		for(int i=0; i<8; ++i)
		{
			switch(rand()%3)
			{
			case 0:
				uniqueWord.Append('0'+rand()%10);
				break;
			case 1:
				uniqueWord.Append('A'+rand()%(1+'Z'-'A'));
				break;
			case 2:
				uniqueWord.Append('a'+rand()%(1+'z'-'a'));
				break;
			}
		}
		uniqueWord.Append(postfix);
		if(YSTRUE!=IsWordIncluded(uniqueWord))
		{
			break;
		}
	}
	return uniqueWord;
}

template <class stringType,class attribType>
YSSIZE_T YsDictionary <stringType,attribType>::GetNumWord(void) const
{
	return wordList.GetN();
}

template <class stringType,class attribType>
YSRESULT YsDictionary <stringType,attribType>::AddWord(const stringType &str,const attribType &attr)
{
	YSHASHKEY hashKey[1]={CalculateHashKey(str)};
	if(YSTRUE!=IsWordIncluded(str))
	{
		auto newWord=wordList.Create();
		newWord->dat.str=str;
		newWord->dat.idx=wordList.GetN()-1;
		newWord->dat.attrib=attr;

		return wordHash.AddElement(1,hashKey,newWord);
	}
	return YSERR;
}

template <class stringType,class attribType>
YSRESULT YsDictionary<stringType,attribType>::UpdateAttrib(const stringType &str,const attribType &attr)
{
	auto keyWord=FindKeyWord(str);
	if(nullptr!=keyWord)
	{
		keyWord->dat.attrib=attr;
		return YSOK;
	}
	else
	{
		return AddWord(str,attr);
	}
}

template <class stringType,class attribType>
YSRESULT YsDictionary <stringType,attribType>::DeleteWord(const stringType &str)
{
	auto wordToDel=FindKeyWord(str);
	if(nullptr!=wordToDel)
	{
		YSHASHKEY hashKey[1]={CalculateHashKey(str)};
		wordHash.DeleteElement(1,hashKey,wordToDel);
		wordList.Delete(wordToDel);
		return YSOK;
	}
	return YSERR;
}

template <class stringType,class attribType>
YSBOOL YsDictionary <stringType,attribType>::IsWordIncluded(const stringType &str) const
{
	auto found=FindKeyWord(str);
	if(nullptr!=found)
	{
		return YSTRUE;
	}
	return YSFALSE;
}

template <class stringType,class attribType>
const attribType *YsDictionary <stringType,attribType>::FindAttrib(const stringType &str) const
{
	auto found=FindKeyWord(str);
	if(nullptr!=found)
	{
		return &(found->dat.attrib);
	}
	return nullptr;
}

template <class stringType,class attribType>
YSSIZE_T YsDictionary <stringType,attribType>::FindIndex(const stringType &str) const
{
	auto found=FindKeyWord(str);
	if(nullptr!=found)
	{
		return found->dat.idx;
	}
	return -1;
}

template <class stringType,class attribType>
const YsListItem <typename YsDictionary <stringType,attribType>::KeyWord> *YsDictionary <stringType,attribType>::FindKeyWord(const stringType &str) const
{
	YSHASHKEY hashKey[1]={CalculateHashKey(str)};
	YSSIZE_T nFound;
	YsListItem <KeyWord> *const *found;
	if(YSOK==wordHash.FindElement(nFound,found,1,hashKey))
	{
		for(YSSIZE_T idx=0; idx<nFound; ++idx)
		{
			if((YSTRUE==caseSensitive && 0==stringType::Strcmp(found[idx]->dat.str,str)) ||
			   (YSTRUE!=caseSensitive && 0==stringType::STRCMP(found[idx]->dat.str,str)))
			{
				return found[idx];
			}
		}
	}
	return nullptr;
}

template <class stringType,class attribType>
YsListItem <typename YsDictionary <stringType,attribType>::KeyWord> *YsDictionary <stringType,attribType>::FindKeyWord(const stringType &str)
{
	YSHASHKEY hashKey[1]={CalculateHashKey(str)};
	YSSIZE_T nFound;
	YsListItem <KeyWord> *const *found;
	if(YSOK==wordHash.FindElement(nFound,found,1,hashKey))
	{
		for(YSSIZE_T idx=0; idx<nFound; ++idx)
		{
			if((YSTRUE==caseSensitive && found[idx]->dat.str==str) ||
			   (YSTRUE!=caseSensitive && 0==stringType::STRCMP(found[idx]->dat.str,str)))
			{
				return found[idx];
			}
		}
	}
	return nullptr;
}

template <class stringType,class attribType>
YSHASHKEY YsDictionary <stringType,attribType>::CalculateHashKey(const stringType &str) const
{
	YSHASHKEY hashKey=0;

	// The commneted-out method gives a pretty good distribution.
	// Problen: The maximum hashKey value is small.  8 letters -> 2+4+6+10+121+17+19+23=202.
	// For large number of words, I want the number to be much larger.
	// const YSHASHKEY prime[]={3,5,7,11,13,17,19,23};
	// for(YSSIZE_T idx=0; idx<str.Strlen(); ++idx)
	// {
	// 	hashKey+=(str[idx]%prime[idx%8]);
	// }

	// Kind of works ok.
	// const int nWindow=5;
	// const int base=31;
	// for(YSSIZE_T i=0; i<str.Strlen(); i+=nWindow)
	// {
	// 	YSHASHKEY add=0;
	// 	for(YSSIZE_T j=0; j<nWindow && i+j<str.Strlen(); ++j)
	// 	{
	// 		auto ij=i+j;
	// 		unsigned int chr=(unsigned int)(str[ij]);
	// 		add*=base;
	// 		add+=(chr%base);
	// 	}
	// 	hashKey+=add;
	// }

	// This prime-number trick seems to randomizes the hash key pretty well.
	const int nPrime=10;
	const YSHASHKEY prime[nPrime]={11,13,17,19,23,29,31,37,41,43};
	const int nWindow=5;
	for(YSSIZE_T i=0; i<str.Strlen(); i+=nWindow)
	{
		YSHASHKEY add=0,base=prime[nPrime/2];
		for(YSSIZE_T j=0; j<nWindow && i+j<str.Strlen(); ++j)
		{
			auto ij=i+j;
			unsigned int chr=(unsigned int)(str[ij]);
			if(YSTRUE!=caseSensitive && 'a'<=chr && chr<='z')
			{
				chr=chr+'A'-'a';
			}
			add*=base;
			add+=(chr%prime[ij%nPrime]);
		}
		hashKey+=add;
	}

	return hashKey;
}

/* } */
#endif
