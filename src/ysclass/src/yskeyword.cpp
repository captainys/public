/* ////////////////////////////////////////////////////////////

File Name: yskeyword.cpp
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

#include "yskeyword.h"

YsKeyWordList::YsKeyWordList() : keyWordList(keyWordAlloc)
{
}

void YsKeyWordList::Initialize(void)
{
	keyWordList.CleanUp();
	keyWordDict.Set(0,NULL);
}

int YsKeyWordList::GetN(void) const
{
	return keyWordList.GetN();
}

YSRESULT YsKeyWordList::MakeList(const char * const str[],const YSSIZE_T attrib[])
{
	int i,n;
	YsListItem <YsKeyWord> *kw;
	YsArray <const char *> ptr;

	Initialize();

	for(i=0; str[i]!=NULL; i++)
	{
		kw=keyWordList.Create();
		kw->dat.str.Set(str[i]);
		kw->dat.id=i;
		kw->dat.attrib=(NULL!=attrib ? attrib[i] : 0);

		ptr.Append(str[i]);
		keyWordDict.Append(&kw->dat);
	}
	n=i;

	YsQuickSortString <YsKeyWord *> (n,ptr,keyWordDict);

	return YSOK;
}

YSRESULT YsKeyWordList::AddKeyWord(YSSIZE_T nStr,const char * const str[],const YSSIZE_T attrib[])
{
	int i,j,k,id;
	YsListItem <YsKeyWord> *kw;
	YsArray <YsKeyWord *> prvKwd,newKwd;
	YsArray <const char *> newKwdPtr;

	id=keyWordList.GetN();
	newKwd.Set(nStr,NULL);
	newKwdPtr.Set(nStr,NULL);
	for(i=0; i<nStr; i++)
	{
		kw=keyWordList.Create();
		kw->dat.str.Set(str[i]);
		kw->dat.id=id;
		kw->dat.attrib=(NULL!=attrib ? attrib[i] : 0);

		newKwd[i]=&kw->dat;
		newKwdPtr[i]=kw->dat.str;

		id++;
	}

	YsQuickSortString <YsKeyWord *> (newKwdPtr.GetN(),newKwdPtr,newKwd);

	prvKwd=keyWordDict;

	keyWordDict.Set(prvKwd.GetN()+newKwd.GetN(),NULL);

	i=0;
	j=0;
	k=0;
	while(i<prvKwd.GetN() || j<newKwd.GetN())
	{
		if(newKwd.GetN()<=j)
		{
			keyWordDict[k++]=prvKwd[i++];
		}
		else if(prvKwd.GetN()<=i)
		{
			keyWordDict[k++]=newKwd[j++];
		}
		else if(strcmp(prvKwd[i]->str,newKwd[j]->str)<0)
		{
			keyWordDict[k++]=prvKwd[i++];
		}
		else
		{
			keyWordDict[k++]=newKwd[j++];
		}
	}

	return YSOK;
}

YSRESULT YsKeyWordList::AddKeyWord(YSSIZE_T nStr,const YsString str[],const YSSIZE_T attrib[])
{
	YsArray <const char *> ptr;
	ptr.Set(nStr,NULL);
	for(decltype(nStr) i=0; i<nStr; i++)
	{
		ptr[i]=str[i];
	}
	return AddKeyWord(nStr,ptr.GetArray(),attrib);
}

int YsKeyWordList::GetId(const char str[]) const
{
	return FindIndex(str);
}

int YsKeyWordList::FindIndex(const char str[]) const
{
	if(keyWordDict.GetN()==0)
	{
		return -1;
	}

	YSSIZE_T idLow,idHigh;
	idLow=0;
	idHigh=keyWordDict.GetN()-1;

	while(idHigh-idLow>1)
	{
		YSSIZE_T idMid,dif;
		idMid=idLow+idHigh;
		idMid>>=1;
		dif=strcmp(str,keyWordDict[idMid]->str);
		if(dif==0)
		{
			return keyWordDict[idMid]->id;
		}
		else if(dif<0)
		{
			idHigh=idMid;
		}
		else
		{
			idLow=idMid;
		}
	}

	if(strcmp(str,keyWordDict[idLow]->str)==0)
	{
		return keyWordDict[idLow]->id;
	}
	else if(strcmp(str,keyWordDict[idHigh]->str)==0)
	{
		return keyWordDict[idHigh]->id;
	}

	return -1;
}

const char *YsKeyWordList::GetString(int id) const
{
	if(keyWordDict.IsInRange(id)==YSTRUE)
	{
		return keyWordDict[id]->str;
	}
	return NULL;
}

YSSIZE_T YsKeyWordList::GetAttribute(int idx) const
{
	if(keyWordDict.IsInRange(idx)==YSTRUE)
	{
		return keyWordDict[idx]->attrib;
	}
	return -1;
}

