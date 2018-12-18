/* ////////////////////////////////////////////////////////////

File Name: yshash.cpp
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

#include <stdio.h>

#include "ysdef.h"
#include "yslist.h"
#include "ysmath.h"
#include "ysarray.h"
#include "yshash.h"

int YsHashSameKeyGroupCreateDeleteCounter=0;


YsKeyStore::YsKeyStore(YSSIZE_T nTable)
{
	YSSIZE_T i;
	if(0<nTable)
	{
		keyArray=new YsArray <YSHASHKEY> *[nTable];
	}
	else
	{
		keyArray=NULL;
	}
	for(i=0; i<nTable; i++)
	{
		keyArray[i]=NULL;
	}
	autoResizing=YSTRUE;
	autoResizeMin=0;
	autoResizeMax=0;
	nKey=0;
	nTab=nTable;
}

YsKeyStore::YsKeyStore(const YsKeyStore &incoming)
{
	CopyFrom(incoming);
}

YsKeyStore::YsKeyStore(YsKeyStore &&incoming)
{
	MoveFrom(incoming);
}

YsKeyStore::~YsKeyStore()
{
	CleanUp();
	if(NULL!=keyArray)
	{
		delete [] keyArray;
		keyArray=NULL;
	}
}

void YsKeyStore::CleanUp(void)
{
	for(int i=0; i<nTab; i++)
	{
		if(keyArray[i]!=NULL)
		{
			delete keyArray[i];
			keyArray[i]=NULL;
		}
	}
	nKey=0;
}

void YsKeyStore::CleanUpThin(void)
{
	for(int i=0; i<nTab; i++)
	{
		if(keyArray[i]!=NULL)
		{
			keyArray[i]->Clear();
		}
	}
	nKey=0;
}

void YsKeyStore::CleanUpDeep(void)
{
	if(0<nTab)
	{
		CleanUp();
		delete [] keyArray;
		keyArray=NULL;
	}
}

void YsKeyStore::CopyFrom(const YsKeyStore &incoming)
{
	if(this==&incoming)
	{
		return;
	}

	CleanUpDeep();

	autoResizing=incoming.autoResizing;
	autoResizeMin=incoming.autoResizeMin;
	autoResizeMax=incoming.autoResizeMax;
	nTab=incoming.nTab;
	nKey=incoming.nKey;

	if(0<nTab)
	{
		keyArray=new YsArray <YSHASHKEY> * [nTab];
		for(int i=0; i<nTab; ++i)
		{
			if(NULL!=incoming.keyArray[i])
			{
				keyArray[i]=new YsArray <YSHASHKEY>;
				(*keyArray[i])=*(incoming.keyArray[i]);
			}
			else
			{
				keyArray[i]=NULL;
			}
		}
	}
}

YsKeyStore &YsKeyStore::operator=(const YsKeyStore &incoming)
{
	CopyFrom(incoming);
	return *this;
}

void YsKeyStore::MoveFrom(YsKeyStore &incoming)
{
	if(this==&incoming)
	{
		return;
	}

	CleanUpDeep();

	autoResizing=incoming.autoResizing;
	autoResizeMin=incoming.autoResizeMin;
	autoResizeMax=incoming.autoResizeMax;
	nTab=incoming.nTab;
	nKey=incoming.nKey;
	keyArray=incoming.keyArray;

	incoming.nTab=0;
	incoming.nKey=0;
	incoming.keyArray=NULL;
}

YsKeyStore &YsKeyStore::operator=(YsKeyStore &&incoming)
{
	MoveFrom(incoming);
	return *this;
}

void YsKeyStore::PrepareTable(void)
{
	CleanUp();
}

void YsKeyStore::EnableAutoResizing(YSSIZE_T minSize,YSSIZE_T maxSize)
{
	autoResizing=YSTRUE;
	autoResizeMin=minSize;
	autoResizeMax=maxSize;
}

void YsKeyStore::EnableAutoResizing(void)
{
	autoResizing=YSTRUE;
	autoResizeMin=0;
	autoResizeMax=0;
}

void YsKeyStore::DisableAutoResizing(void)
{
	autoResizing=YSFALSE;
}

void YsKeyStore::Resize(YSSIZE_T newNTable)
{
	YSSIZE_T i,j,k;
	YsArray <YSHASHKEY> allKey;
	allKey.Set(nKey,NULL);
	k=0;

	for(i=0; i<nTab; i++)
	{
		if(keyArray[i]!=NULL)
		{
			for(j=0; j<keyArray[i]->GetN(); j++)
			{
				allKey[k++]=(*keyArray[i])[j];
			}
			delete keyArray[i];
			keyArray[i]=NULL;
		}
	}

	if(NULL!=keyArray)
	{
		delete [] keyArray;
	}

	keyArray=new YsArray <YSHASHKEY> * [newNTable];
	nTab=newNTable;
	for(i=0; i<nTab; i++)
	{
		keyArray[i]=NULL;
	}

	forYsArray(i,allKey)
	{
		int tabId;
		tabId=allKey[i]%nTab;
		if(keyArray[tabId]==NULL)
		{
			keyArray[tabId]=new YsArray <YSHASHKEY>;
		}
		keyArray[tabId]->Append(allKey[i]);
	}
}

YSRESULT YsKeyStore::Add(YSHASHKEY key)
{
	int i;
	YSHASHKEY tabId;

	if(0==nTab)
	{
		Resize(1);
	}

	tabId=key%nTab;
	if(keyArray[tabId]==NULL)
	{
		keyArray[tabId]=new YsArray <YSHASHKEY>;
	}
	for(i=0; i<keyArray[tabId]->GetN(); i++)
	{
		if((*keyArray[tabId])[i]==key)
		{
			return YSERR;  // Already in.
		}
	}

	keyArray[tabId]->Append(key);
	nKey++;

	CheckResizeGrow();

	return YSOK;
}

YSRESULT YsKeyStore::Delete(YSHASHKEY key)
{
	if(0==nTab)
	{
		return YSERR;
	}

	YSHASHKEY tabId=key%nTab;
	if(keyArray[tabId]==NULL)
	{
		return YSERR;  // Key doesn't exist.
	}

	for(int i=0; i<keyArray[tabId]->GetN(); i++)
	{
		if((*keyArray[tabId])[i]==key)
		{
			keyArray[tabId]->DeleteBySwapping(i);
			nKey--;
			if(keyArray[tabId]->GetN()==0)
			{
				delete keyArray[tabId];
				keyArray[tabId]=NULL;
			}

			CheckResizeShrink();

			return YSOK;
		}
	}

	return YSERR; // Key doesn't exist.
}

YSRESULT YsKeyStore::AddKey(YSHASHKEY key)
{
	return Add(key);
}
YSRESULT YsKeyStore::DeleteKey(YSHASHKEY key)
{
	return Delete(key);
}

YSBOOL YsKeyStore::IsIncluded(YSHASHKEY key) const
{
	if(0==nTab)
	{
		return YSFALSE;
	}

	YSHASHKEY tabId=key%nTab;
	if(keyArray[tabId]==NULL)
	{
		return YSFALSE;
	}

	for(int i=0; i<keyArray[tabId]->GetN(); i++)
	{
		if((*keyArray[tabId])[i]==key)
		{
			return YSTRUE;
		}
	}

	return YSFALSE;
}

void YsKeyStore::CheckResizeGrow(void)
{
	if(autoResizing==YSTRUE)
	{
		if(autoResizeMax==0)
		{
			if(nTab*4<nKey)
			{
				Resize(nKey);
			}
		}
		else if(nTab<autoResizeMax && nTab*4<nKey)
		{
			Resize(YsSmaller(nTab*2,autoResizeMax));
		}
	}
}

void YsKeyStore::CheckResizeShrink(void)
{
	if(autoResizing==YSTRUE)
	{
		if(autoResizeMin==0)
		{
			if(nKey<nTab/4)
			{
				Resize(YsGreater <YSSIZE_T> (nKey,1));
			}
		}
		else if(autoResizeMin<nTab && nKey<nTab)
		{
			Resize(YsGreater <YSSIZE_T> (nTab/2,autoResizeMin));
		}
	}
}

YSSIZE_T YsKeyStore::GetN(void) const
{
	return nKey;
}

int YsKeyStore::GetNumKey(void) const
{
	return (int)nKey;
}

int YsKeyStore::GetNumTable(void) const
{
	return (int)nTab;
}

void YsKeyStore::NullifyPointer(KeyPointer &ptr) const
{
	ptr.row=-1;
	ptr.column=-1;
}

YSRESULT YsKeyStore::FindNextKey(KeyPointer &ptr) const
{
	if(0==nKey)
	{
		return YSERR;
	}

	if(0>ptr.row)
	{
		ptr.row=0;
		ptr.column=0;
		if(NULL!=keyArray[0] && 0<keyArray[0]->GetN())
		{
			return YSOK;
		}
	}

	while(ptr.row<nTab)
	{
		ptr.column++;
		if(NULL!=keyArray[ptr.row] && ptr.column<keyArray[ptr.row]->GetN())
		{
			return YSOK;
		}
		if(NULL==keyArray[ptr.row] || keyArray[ptr.row]->GetN()<=ptr.column)
		{
			ptr.row++;
			ptr.column=-1;
		}
	}
	return YSERR;
}

YSBOOL YsKeyStore::IsPointerValid(const KeyPointer &ptr) const
{
	if(0<=ptr.row && ptr.row<nTab && NULL!=keyArray[ptr.row] && ptr.column<keyArray[ptr.row]->GetN())
	{
		return YSTRUE;
	}
	return YSFALSE;
}

YSHASHKEY YsKeyStore::GetKey(const KeyPointer &ptr) const
{
	return (*keyArray[ptr.row])[ptr.column];
}

////////////////////////////////////////////////////////////

YsTwoKeyStore::YsTwoKeyStore(int nTable)
{
	int i;
	if(0<nTable)
	{
		keyArray=new YsArray <YSHASHKEY> *[nTable];
	}
	else
	{
		keyArray=NULL;
	}
	for(i=0; i<nTable; i++)
	{
		keyArray[i]=NULL;
	}
	autoResizing=YSTRUE;
	autoResizeMin=0;
	autoResizeMax=0;
	nKey=0;
	nTab=nTable;
}

YsTwoKeyStore::YsTwoKeyStore(const YsTwoKeyStore &incoming)
{
	CopyFrom(incoming);
}

YsTwoKeyStore::YsTwoKeyStore(YsTwoKeyStore &&incoming)
{
	MoveFrom(incoming);
}

YsTwoKeyStore::~YsTwoKeyStore()
{
	CleanUp();
	if(NULL!=keyArray)
	{
		delete [] keyArray;
		keyArray=NULL;
	}
}

void YsTwoKeyStore::CleanUp(void)
{
	for(int i=0; i<nTab; i++)
	{
		if(keyArray[i]!=NULL)
		{
			delete keyArray[i];
			keyArray[i]=NULL;
		}
	}
	nKey=0;
}

void YsTwoKeyStore::CleanUpDeep(void)
{
	if(NULL!=keyArray)
	{
		CleanUp();
		delete [] keyArray;
		keyArray=NULL;
	}
}

void YsTwoKeyStore::CopyFrom(const YsTwoKeyStore &incoming)
{
	if(this==&incoming)
	{
		return;
	}

	CleanUpDeep();

	autoResizing=incoming.autoResizing;
	autoResizeMin=incoming.autoResizeMin;
	autoResizeMax=incoming.autoResizeMax;
	nTab=incoming.nTab;
	nKey=incoming.nKey;

	if(0<nTab)
	{
		keyArray=new YsArray <YSHASHKEY> * [nTab];
		for(int i=0; i<nTab; ++i)
		{
			if(NULL!=incoming.keyArray[i])
			{
				keyArray[i]=new YsArray <YSHASHKEY>;
				(*keyArray[i])=*(incoming.keyArray[i]);
			}
			else
			{
				keyArray[i]=NULL;
			}
		}
	}
}

YsTwoKeyStore &YsTwoKeyStore::operator=(const YsTwoKeyStore &incoming)
{
	if(this!=&incoming)
	{
		CopyFrom(incoming);
	}
	return *this;
}

void YsTwoKeyStore::MoveFrom(YsTwoKeyStore &incoming)
{
	if(this==&incoming)
	{
		return;
	}

	CleanUpDeep();

	autoResizing=incoming.autoResizing;
	autoResizeMin=incoming.autoResizeMin;
	autoResizeMax=incoming.autoResizeMax;
	nTab=incoming.nTab;
	nKey=incoming.nKey;
	keyArray=incoming.keyArray;

	incoming.nTab=0;
	incoming.nKey=0;
	incoming.keyArray=NULL;
}

YsTwoKeyStore &YsTwoKeyStore::operator=(YsTwoKeyStore &&incoming)
{
	MoveFrom(incoming);
	return *this;
}

void YsTwoKeyStore::PrepareTable(void)
{
	CleanUp();
}

void YsTwoKeyStore::EnableAutoResizing(YSSIZE_T minSize,YSSIZE_T maxSize)
{
	autoResizing=YSTRUE;
	autoResizeMin=minSize;
	autoResizeMax=maxSize;
}

void YsTwoKeyStore::EnableAutoResizing(void)
{
	autoResizing=YSTRUE;
	autoResizeMin=0;
	autoResizeMax=0;
}

void YsTwoKeyStore::DisableAutoResizing(void)
{
	autoResizing=YSFALSE;
}

void YsTwoKeyStore::Resize(YSSIZE_T newNTable)
{
	int i,j,k;
	YsArray <YSHASHKEY> allKey;
	allKey.Set(nKey*2,NULL);
	k=0;

	for(i=0; i<nTab; i++)
	{
		if(keyArray[i]!=NULL)
		{
			for(j=0; j<keyArray[i]->GetN(); j++)
			{
				allKey[k++]=(*keyArray[i])[j];
			}
			delete keyArray[i];
			keyArray[i]=NULL;
		}
	}

	if(NULL!=keyArray)
	{
		delete [] keyArray;
	}
	keyArray=new YsArray <YSHASHKEY> * [newNTable];
	nTab=newNTable;
	for(i=0; i<nTab; i++)
	{
		keyArray[i]=NULL;
	}

	for(i=0; i<=allKey.GetN()-2; i+=2)
	{
		int tabId;
		tabId=(allKey[i]+allKey[i+1])%nTab;
		if(keyArray[tabId]==NULL)
		{
			keyArray[tabId]=new YsArray <YSHASHKEY>;
		}
		keyArray[tabId]->Append(2,((YSHASHKEY *)allKey)+i);
	}
}

YSRESULT YsTwoKeyStore::AddKey(YSHASHKEY key1,YSHASHKEY key2)
{
	int i;
	YSHASHKEY tabId;
	YSHASHKEY key[2];

	if(key1<key2)
	{
		key[0]=key1;
		key[1]=key2;
	}
	else
	{
		key[0]=key2;
		key[1]=key1;
	}

	if(0==nTab)
	{
		Resize(1);
	}

	tabId=(key1+key2)%nTab;
	if(keyArray[tabId]==NULL)
	{
		keyArray[tabId]=new YsArray <YSHASHKEY>;
	}

	YsArray <YSHASHKEY> &entry=*keyArray[tabId];
	for(i=0; i<=entry.GetN()-2; i+=2)
	{
		if(entry[i]==key[0] && entry[i+1]==key[1])
		{
			return YSERR;  // Already in.
		}
	}

	entry.Append(2,key);
	nKey++;

	CheckResizeGrow();

	return YSOK;
}

YSRESULT YsTwoKeyStore::DeleteKey(YSHASHKEY key1,YSHASHKEY key2)
{
	if(0==nTab)
	{
		return YSERR;
	}

	int i;
	YSHASHKEY tabId;
	YSHASHKEY key[2];

	if(key1<key2)
	{
		key[0]=key1;
		key[1]=key2;
	}
	else
	{
		key[0]=key2;
		key[1]=key1;
	}

	tabId=(key1+key2)%nTab;
	if(keyArray[tabId]==NULL)
	{
		return YSERR;  // Key doesn't exist.
	}

	YsArray <YSHASHKEY> &entry=*keyArray[tabId];
	for(i=0; i<=entry.GetN()-2; i+=2)
	{
		if(entry[i]==key[0] && entry[i+1]==key[1])
		{
			entry[i  ]=entry[entry.GetN()-2];
			entry[i+1]=entry[entry.GetN()-1];
			entry.Resize(entry.GetN()-2);

			nKey--;
			if(entry.GetN()==0)
			{
				delete keyArray[tabId];
				keyArray[tabId]=NULL;
			}

			CheckResizeShrink();

			return YSOK;
		}
	}

	return YSERR; // Key doesn't exist.
}

YSBOOL YsTwoKeyStore::IsIncluded(YSHASHKEY key1,YSHASHKEY key2) const
{
	if(0==nTab)
	{
		return YSFALSE;
	}

	int i;
	YSHASHKEY tabId;
	YSHASHKEY key[2];

	if(key1<key2)
	{
		key[0]=key1;
		key[1]=key2;
	}
	else
	{
		key[0]=key2;
		key[1]=key1;
	}

	tabId=(key1+key2)%nTab;
	if(keyArray[tabId]==NULL)
	{
		return YSFALSE;
	}

	YsArray <YSHASHKEY> &entry=*keyArray[tabId];
	for(i=0; i<=entry.GetN()-2; i+=2)
	{
		if(entry[i]==key[0] && entry[i+1]==key[1])
		{
			return YSTRUE;
		}
	}

	return YSFALSE;
}

YSRESULT YsTwoKeyStore::AddKey(const YSHASHKEY key[2])
{
	return AddKey(key[0],key[1]);
}
YSRESULT YsTwoKeyStore::DeleteKey(const YSHASHKEY key[2])
{
	return DeleteKey(key[0],key[1]);
}
YSBOOL YsTwoKeyStore::IsIncluded(const YSHASHKEY key[2]) const
{
	return IsIncluded(key[0],key[1]);
}

void YsTwoKeyStore::CheckResizeGrow(void)
{
	if(autoResizing==YSTRUE)
	{
		if(autoResizeMax==0)
		{
			if(nTab*4<nKey)
			{
				Resize(nKey);
			}
		}
		else if(nTab<autoResizeMax && nTab*4<nKey)
		{
			Resize(YsSmaller(nTab*2,autoResizeMax));
		}
	}
}

void YsTwoKeyStore::CheckResizeShrink(void)
{
	if(autoResizing==YSTRUE)
	{
		if(autoResizeMin==0)
		{
			if(nKey<nTab/4)
			{
				Resize(YsGreater <YSSIZE_T> (nKey,1));
			}
		}
		else if(autoResizeMin<nTab && nKey<nTab)
		{
			Resize(YsGreater <YSSIZE_T> (nTab/2,autoResizeMin));
		}
	}
}

YSSIZE_T YsTwoKeyStore::GetN(void) const
{
	return nKey;
}

int YsTwoKeyStore::GetNumKey(void) const
{
	return (int)nKey;
}

int YsTwoKeyStore::GetNumTable(void) const
{
	return (int)nTab;
}

void YsTwoKeyStore::NullifyPointer(KeyPointer &ptr) const
{
	ptr.row=-1;
	ptr.column=-2;
}

YSRESULT YsTwoKeyStore::FindNextKey(KeyPointer &ptr) const
{
	if(0==nKey)
	{
		return YSERR;
	}

	if(0>ptr.row)
	{
		ptr.row=0;
		ptr.column=0;
		if(NULL!=keyArray[0] && 0<keyArray[0]->GetN()-1)
		{
			return YSOK;
		}
	}

	while(ptr.row<nTab)
	{
		ptr.column+=2;
		if(NULL!=keyArray[ptr.row] && ptr.column<keyArray[ptr.row]->GetN()-1)
		{
			return YSOK;
		}
		if(NULL==keyArray[ptr.row] || keyArray[ptr.row]->GetN()<=ptr.column)
		{
			ptr.row++;
			ptr.column=-2;
		}
	}
	return YSERR;
}

YSBOOL YsTwoKeyStore::IsPointerValid(const KeyPointer &ptr) const
{
	if(0<=ptr.row && ptr.row<nTab && NULL!=keyArray[ptr.row] && ptr.column<keyArray[ptr.row]->GetN()-1)
	{
		return YSTRUE;
	}
	return YSFALSE;
}

YsStaticArray <YSHASHKEY,2> YsTwoKeyStore::GetKey(const KeyPointer &ptr) const
{
	YsStaticArray <YSHASHKEY,2> key;
	key[0]=(*keyArray[ptr.row])[ptr.column];
	key[1]=(*keyArray[ptr.row])[ptr.column+1];
	return key;
}
