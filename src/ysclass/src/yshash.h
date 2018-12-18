/* ////////////////////////////////////////////////////////////

File Name: yshash.h
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

#ifndef YSHASH_IS_INCLUDED
#define YSHASH_IS_INCLUDED
/* { */

#include "yssort.h"
#include "ysdef.h"
#include "ysarray.h"
#include "ys2darray.h"

#include "ysudhandleiterator.h"

class YsHashElementEnumHandle
{
public:
	YSSIZE_T hashIdx;
	YSSIZE_T arrayIdx;

	bool operator==(const YsHashElementEnumHandle &from)
	{
		return (this->hashIdx==from.hashIdx && this->arrayIdx==from.arrayIdx);
	}

	bool operator!=(const YsHashElementEnumHandle &from)
	{
		return (this->hashIdx!=from.hashIdx || this->arrayIdx!=from.arrayIdx);
	}
};

// Each element must have an unique key
template <class toFind>
class YsHashTable
{
private:
	class HashElement
	{
	public:
		YSHASHKEY key;
		toFind dat;
	};


public:
	typedef YsUnidirectionalHandleIterator<YsHashTable<toFind>,YsHashElementEnumHandle,toFind> iterator;
	typedef YsUnidirectionalHandleIterator<const YsHashTable<toFind>,YsHashElementEnumHandle,const toFind> const_iterator;
	iterator begin()
	{
		iterator iter(this);
		return iter.begin();
	}
	iterator end()
	{
		iterator iter(this);
		return iter.end();
	}
	const_iterator begin() const
	{
		const_iterator iter(this);
		return iter.begin();
	}
	const_iterator end() const
	{
		const_iterator iter(this);
		return iter.end();
	}
	YsUnidirectionalHandleEnumerator<YsHashTable<toFind>,YsHashElementEnumHandle> AllHandle(void) const;

protected:
	YSBOOL enableAutoResizing;
	YSSIZE_T autoResizingMin,autoResizingMax;

	YSSIZE_T HashSize;
	YSSIZE_T nElem;
	mutable Ys2DArray <HashElement,int> table;

public:
	YsHashTable(YSSIZE_T hashSize=16);
	~YsHashTable();

	YsHashTable(const YsHashTable <toFind> &incoming);
	YsHashTable <toFind> &operator=(const YsHashTable <toFind> &incoming);
	YsHashTable <toFind> &CopyFrom(const YsHashTable <toFind> &incoming);

	YsHashTable(YsHashTable <toFind> &&incoming);
	YsHashTable <toFind> &operator=(YsHashTable <toFind> &&incoming);
	YsHashTable <toFind> &MoveFrom(YsHashTable <toFind> &incoming);

	void CleanUp(void);
	YSRESULT PrepareTable(void);
	YSRESULT PrepareTable(YSSIZE_T hashSize);
	YSRESULT PrepareTableForNElement(YSSIZE_T n);
	YSRESULT Resize(YSSIZE_T hashSize);
	YSRESULT CollectGarbage(void);

	YSRESULT Add(YSHASHKEY searchKey,toFind element);
	YSRESULT Update(YSHASHKEY searchKey,toFind element);
	YSRESULT Update(const YsHashElementEnumHandle &handle,toFind newValue);
	YSRESULT Delete(YSHASHKEY searchKey,toFind element);
	YSRESULT DeleteIfExist(YSHASHKEY searchKey);

	/*! Old naming convention.  Use Add instead. */
	YSRESULT AddElement(YSHASHKEY searchKey,toFind element);

	/*! Old naming convention.  Use Update instead. */
	YSRESULT UpdateElement(YSHASHKEY searchKey,toFind element);

	/*! Old naming convention.  Use Update instead. */
	YSRESULT UpdateElement(const YsHashElementEnumHandle &handle,toFind newValue);

	/*! Old naming convention.  Use Delete instead. */
	YSRESULT DeleteElement(YSHASHKEY searchKey,toFind element);

	/*! Old naming convention.  Use DeleteIfExist instead. */
	YSRESULT DeleteElementIfExist(YSHASHKEY searchKey);

	YSRESULT DeleteKey(YSHASHKEY key);

	YSRESULT FindElement(toFind &elem,YSHASHKEY searchKey) const;
	toFind *FindElement(YSHASHKEY searchKey);
	const toFind *FindElement(YSHASHKEY searchKey) const;
	toFind *operator[](YSHASHKEY key);
	const toFind *operator[](YSHASHKEY key) const;
	YSRESULT GetKey(YSHASHKEY &key,const YsHashElementEnumHandle &handle) const;
	YSHASHKEY GetKey(const YsHashElementEnumHandle &handle) const;
	YSRESULT GetElement(toFind &elem,const YsHashElementEnumHandle &handle) const;
	toFind &GetElement(const YsHashElementEnumHandle &handle);
	const toFind &GetElement(const YsHashElementEnumHandle &handle) const;
	YSBOOL CheckKeyExist(unsigned searchKey) const;

	/*! Returns a null handle.  You can iterate through keys by the following loop.
	    for(auto handle=hash.NullHandle(); YSOK==FindNextKey(handle); )
	    {
	        // Do something useful.
	    }
	*/
	YsHashElementEnumHandle NullHandle(void) const;

	YSBOOL IsHandleValid(const YsHashElementEnumHandle &handle) const;
	YSRESULT RewindElementEnumHandle(YsHashElementEnumHandle &handle) const;
	YSRESULT FindNextElement(YsHashElementEnumHandle &handle) const;

	/*! Returns the number of elements (equals to the number of keys) in this table. */
	YSSIZE_T GetN(void) const;

	/*! Moves the element enum handle to the next element. */
	void MoveToNext(YsHashElementEnumHandle &hd) const;

	/*! Returns the element enum handle that points to the first element in the hash table. */
	YsHashElementEnumHandle First(void) const;

	/*! Returns the null element enum handle. */
	YsHashElementEnumHandle Null(void) const;

	/*! Returns the value pointed by the element enum handle. */
	toFind &Value(const YsHashElementEnumHandle &hd);

	/*! Returns the value pointed by the element enum handle. */
	const toFind &Value(const YsHashElementEnumHandle &hd) const;

	/*! Returns the key to the element enum handle. */
	YSHASHKEY Key(const YsHashElementEnumHandle &hd) const;

	YSRESULT SelfDiagnostic(void) const;

	void EnableAutoResizing(YSSIZE_T minSize,YSSIZE_T maxSize);
	void EnableAutoResizing(void);
	void DisableAutoResizing(void);

	int GetNumUnused(void) const;

	const char *tableName;

protected:
	YSRESULT CheckAutoResizingGrow(void);
	YSRESULT CheckAutoResizingShrink(void);

private:
	HashElement *FindTableIndex(YSSIZE_T &i1,YSSIZE_T &i2,YSHASHKEY searchKey);
	const HashElement *FindTableIndex(YSSIZE_T &i1,YSSIZE_T &i2,YSHASHKEY searchKey) const;
};

////////////////////////////////////////////////////////////

/*! Support for STL-like iterator */
template <class toFind>
inline typename YsHashTable<toFind>::iterator begin(YsHashTable<toFind> &table)
{
	return table.begin();
}

template <class toFind>
inline typename YsHashTable<toFind>::iterator end(YsHashTable<toFind> &table)
{
	return table.end();
}

template <class toFind>
inline typename YsHashTable<toFind>::const_iterator begin(const YsHashTable<toFind> &table)
{
	return table.begin();
}

template <class toFind>
inline typename YsHashTable<toFind>::const_iterator end(const YsHashTable<toFind> &table)
{
	return table.end();
}

template <class toFind>
YsUnidirectionalHandleEnumerator<YsHashTable<toFind>,YsHashElementEnumHandle> YsHashTable<toFind>::AllHandle(void) const
{
	return YsUnidirectionalHandleEnumerator<YsHashTable<toFind>,YsHashElementEnumHandle>(this);
}

////////////////////////////////////////////////////////////

template <class toFind>
YsHashTable <toFind>::YsHashTable(const YsHashTable <toFind> &incoming)
{
	table=NULL;
	CopyFrom(incoming);
}

template <class toFind>
YsHashTable <toFind> &YsHashTable <toFind>::operator=(const YsHashTable <toFind> &incoming)
{
	return CopyFrom(incoming);
}

template <class toFind>
YsHashTable <toFind> &YsHashTable <toFind>::CopyFrom(const YsHashTable <toFind> &incoming)
{
	tableName=incoming.tableName;
	HashSize=incoming.HashSize;
	nElem=incoming.nElem;
	table=incoming.table;
	return *this;
}


template <class toFind>
YsHashTable <toFind>::YsHashTable(YsHashTable <toFind> &&incoming)
{
	table.CleanUp();
	MoveFrom(incoming);
}

template <class toFind>
YsHashTable <toFind> &YsHashTable <toFind>::operator=(YsHashTable <toFind> &&incoming)
{
	return MoveFrom(incoming);
}

template <class toFind>
YsHashTable <toFind> &YsHashTable <toFind>::MoveFrom(YsHashTable <toFind> &incoming)
{
	tableName=incoming.tableName;
	HashSize=incoming.HashSize;
	nElem=incoming.nElem;
	table.MoveFrom(incoming.table);

	incoming.HashSize=0;
	incoming.nElem=0;

	return *this;
}


template <class toFind>
YsHashTable <toFind> ::YsHashTable(YSSIZE_T hashSize)
{
	HashSize=hashSize;
	table.ResizeN1(hashSize);
	nElem=0;

	tableName="";

	enableAutoResizing=YSTRUE;
	autoResizingMin=0;
	autoResizingMax=0;
}

template <class toFind>
YsHashTable <toFind> ::~YsHashTable()
{
}

template <class toFind>
void YsHashTable <toFind> ::CleanUp(void)
{
	HashSize=1;
	table.CleanUp();
	table.ResizeN1(1);
	nElem=0;
}

template <class toFind>
YSRESULT YsHashTable <toFind>::PrepareTable(void)
{
	for(YSSIZE_T i=0; i<HashSize; i++)
	{
		table.ResizeN2(i,0);
	}
	nElem=0;
	return YSOK;
}

template <class toFind>
YSRESULT YsHashTable <toFind>::PrepareTable(YSSIZE_T hashSize)
{
	if(hashSize<1)
	{
		hashSize=1;
	}
	HashSize=hashSize;
	table.CleanUp();
	table.ResizeN1(hashSize);
	nElem=0;
	return YSOK;
}

template <class toFind>
YSRESULT YsHashTable <toFind>::PrepareTableForNElement(YSSIZE_T n)
{
	return PrepareTable(n/4);
}

template <class toFind>
YSRESULT YsHashTable <toFind>::Resize(YSSIZE_T hashSize)
{
	int i;
	YSBOOL saveAutoResizing;
	YsArray <HashElement,0,int> elemList;
	for(i=0; i<HashSize; i++)
	{
		elemList.Append(table.GetN2(i),table[i]);
	}
	PrepareTable(hashSize);

	saveAutoResizing=enableAutoResizing;
	enableAutoResizing=YSFALSE;
	for(i=0; i<elemList.GetN(); i++)
	{
		AddElement(elemList[i].key,elemList[i].dat);
	}
	enableAutoResizing=saveAutoResizing;

	return YSOK;
}

template <class toFind>
YSRESULT YsHashTable <toFind>::CollectGarbage(void)
{
	Resize(HashSize);
	return YSOK;
}

template <class toFind>
YSRESULT YsHashTable <toFind>::Add(YSHASHKEY searchKey,toFind element)
{
	YSHASHKEY hashId;
	YSSIZE_T i1,i2,im;
	HashElement *entry;
	HashElement neo;

	hashId=searchKey%HashSize;
	entry=table[hashId];

	neo.dat=element;
	neo.key=searchKey;

	if(table.GetN2(hashId)==0)
	{
		table.Append(hashId,neo);
	}
	else
	{
		i1=0;
		i2=table.GetN2(hashId)-1;

		if(searchKey<entry[i1].key)
		{
			table.Insert(hashId,i1,neo);
		}
		else if(entry[i2].key<searchKey)
		{
			table.Append(hashId,neo);
		}
		else
		{
			i1=0;
			i2=table.GetN2(hashId)-1;
			while(i2-i1>1)
			{
				im=(i1+i2)/2;
				if(searchKey<entry[im].key)
				{
					i2=im;
				}
				else
				{
					i1=im;
				}
			}
			table.Insert(hashId,i2,neo);
		}
	}
	nElem++;
	CheckAutoResizingGrow();
	return YSOK;
}

template <class toFind>
YSRESULT YsHashTable <toFind>::AddElement(YSHASHKEY searchKey,toFind element)
{
	return Add(searchKey,element);
}

template <class toFind>
YSRESULT YsHashTable <toFind>::Update(YSHASHKEY searchKey,toFind element)
{
	HashElement *entry;
	HashElement neo;

	YSHASHKEY hashId=searchKey%HashSize;
	entry=table[hashId];

	neo.dat=element;
	neo.key=searchKey;

	if(table.GetN2(hashId)==0)
	{
		table.Append(hashId,neo);
	}
	else
	{
		YSSIZE_T i1,i2;
		i1=0;
		i2=table.GetN2(hashId)-1;

		if(searchKey<entry[i1].key)
		{
			table.Insert(hashId,i1,neo);
		}
		else if(entry[i2].key<searchKey)
		{
			table.Append(hashId,neo);
		}
		else
		{
			while(i2-i1>1)
			{
				const YSSIZE_T im=(i1+i2)/2;
				if(searchKey<entry[im].key)
				{
					i2=im;
				}
				else
				{
					i1=im;
				}
			}

			if(searchKey==entry[i1].key)
			{
				entry[i1].dat=element;
				return YSOK;  // I must return here because the size doesn't change.
			}
			else if(searchKey==entry[i2].key)
			{
				entry[i2].dat=element;
				return YSOK;  // I must return here because the size doesn't change.
			}

			table.Insert(hashId,i2,neo);
		}
	}
	nElem++;
	CheckAutoResizingGrow();
	return YSOK;
}

template <class toFind>
YSRESULT YsHashTable <toFind>::UpdateElement(YSHASHKEY searchKey,toFind element)
{
	return Update(searchKey,element);
}

template <class toFind>
YSRESULT YsHashTable <toFind>::Update(const YsHashElementEnumHandle &handle,toFind newValue)
{
	const auto hashIdx=handle.hashIdx;
	const auto arrayIdx=handle.arrayIdx;

	if(0<=hashIdx && hashIdx<HashSize &&
	   0<=arrayIdx && arrayIdx<table.GetN2(hashIdx))
	{
		table[hashIdx][arrayIdx].dat=newValue;
		return YSOK;
	}
	return YSERR;
	
}

template <class toFind>
YSRESULT YsHashTable <toFind>::UpdateElement(const YsHashElementEnumHandle &handle,toFind newValue)
{
	return Update(handle,newValue);
}

template <class toFind>
YSRESULT YsHashTable <toFind>::Delete(YSHASHKEY searchKey,toFind)
{
	HashElement *entry;
	YSSIZE_T idx1,idx2;

	entry=FindTableIndex(idx1,idx2,searchKey);
	if(entry!=NULL)
	{
		table.Delete(idx1,idx2);
		nElem--;
		CheckAutoResizingShrink();
		return YSOK;
	}

	YsErrOut("YsHashTable::DeleteElement()\n");
	YsErrOut("  Element not found\n");
	return YSERR;
}

template <class toFind>
YSRESULT YsHashTable <toFind>::DeleteElement(YSHASHKEY searchKey,toFind abc)
{
	return Delete(searchKey,abc);
}

template <class toFind>
YSRESULT YsHashTable <toFind>::DeleteIfExist(YSHASHKEY searchKey)
{
	YSSIZE_T idx1,idx2;
	HashElement *entry=FindTableIndex(idx1,idx2,searchKey);
	if(entry!=NULL)
	{
		table.Delete(idx1,idx2);
		nElem--;
		CheckAutoResizingShrink();
		return YSOK;
	}
	return YSOK;
}

template <class toFind>
YSRESULT YsHashTable <toFind>::DeleteElementIfExist(YSHASHKEY searchKey)
{
	return DeleteIfExist(searchKey);
}

template <class toFind>
YSRESULT YsHashTable<toFind>::DeleteKey(YSHASHKEY searchKey)
{
	return DeleteElementIfExist(searchKey);
}

template <class toFind>
YSRESULT YsHashTable <toFind>::FindElement(toFind &element,YSHASHKEY searchKey) const
{
	YSSIZE_T idx1,idx2;
	const HashElement *entry=FindTableIndex(idx1,idx2,searchKey);
	if(entry!=NULL)
	{
		element=entry[idx2].dat;
		return YSOK;
	}
	return YSERR;
}

template <class toFind>
toFind *YsHashTable<toFind>::FindElement(YSHASHKEY searchKey)
{
	YSSIZE_T idx1,idx2;
	auto *entry=FindTableIndex(idx1,idx2,searchKey);
	if(entry!=NULL)
	{
		return &entry[idx2].dat;
	}
	return NULL;
}
template <class toFind>
const toFind *YsHashTable<toFind>::FindElement(YSHASHKEY searchKey) const
{
	YSSIZE_T idx1,idx2;
	auto *entry=FindTableIndex(idx1,idx2,searchKey);
	if(entry!=NULL)
	{
		return &entry[idx2].dat;
	}
	return NULL;
}

template <class toFind>
toFind *YsHashTable<toFind>::operator[](YSHASHKEY key)
{
	return FindElement(key);
}
template <class toFind>
const toFind *YsHashTable<toFind>::operator[](YSHASHKEY key) const
{
	return FindElement(key);
}


template <class toFind>
YSRESULT YsHashTable <toFind>::GetKey(YSHASHKEY &key,const YsHashElementEnumHandle &handle) const
{
	const auto hashIdx=handle.hashIdx;
	const auto arrayIdx=handle.arrayIdx;

	if(YSTRUE==table.IsInRange(hashIdx,arrayIdx))
	{
		key=table[hashIdx][arrayIdx].key;
		return YSOK;
	}
	return YSERR;
}

template <class toFind>
YSHASHKEY YsHashTable<toFind>::GetKey(const YsHashElementEnumHandle &handle) const
{
	YSHASHKEY key;
	GetKey(key,handle);
	return key;
}

template <class toFind>
YSRESULT YsHashTable <toFind>::GetElement(toFind &elem,const YsHashElementEnumHandle &handle) const
{
	const auto hashIdx=handle.hashIdx;
	const auto arrayIdx=handle.arrayIdx;

	if(YSTRUE==table.IsInRange(hashIdx,arrayIdx))
	{
		elem=table[hashIdx][arrayIdx].dat;
		return YSOK;
	}
	return YSERR;
}

template <class toFind>
toFind &YsHashTable<toFind>::GetElement(const YsHashElementEnumHandle &handle)
{
	const auto hashIdx=handle.hashIdx;
	const auto arrayIdx=handle.arrayIdx;

	if(YSTRUE==table.IsInRange(hashIdx,arrayIdx))
	{
		return table[hashIdx][arrayIdx].dat;
	}
	return table[0][0].dat;  // Good luck
}

template <class toFind>
const toFind &YsHashTable<toFind>::GetElement(const YsHashElementEnumHandle &handle) const
{
	const auto hashIdx=handle.hashIdx;
	const auto arrayIdx=handle.arrayIdx;

	if(YSTRUE==table.IsInRange(hashIdx,arrayIdx))
	{
		return table[hashIdx][arrayIdx].dat;
	}
	return table[0][0].dat;  // Good luck
}

template <class toFind>
YSBOOL YsHashTable <toFind>::CheckKeyExist(unsigned searchKey) const
{
	YSSIZE_T idx1,idx2;
	if(FindTableIndex(idx1,idx2,searchKey)!=NULL)
	{
		return YSTRUE;
	}
	else
	{
		return YSFALSE;
	}

}
template <class toFind>
YsHashElementEnumHandle YsHashTable <toFind>::NullHandle(void) const
{
	YsHashElementEnumHandle handle;
	handle.hashIdx=-1;
	handle.arrayIdx=-1;
	return handle;
}

template <class toFind>
YSBOOL YsHashTable <toFind>::IsHandleValid(const YsHashElementEnumHandle &handle) const
{
	if(table.IsInRange(handle.hashIdx,handle.arrayIdx))
	{
		return YSTRUE;
	}
	return YSFALSE;
}

template <class toFind>
YSRESULT YsHashTable <toFind>::RewindElementEnumHandle(YsHashElementEnumHandle &handle) const
{
	// Rewind -> handle points to the first element
	for(YSSIZE_T i=0; i<HashSize; i++)
	{
		if(0<table.GetN2(i))
		{
			handle.hashIdx=i;
			handle.arrayIdx=0;
			return YSOK;
		}
	}
	handle.hashIdx=-1;
	handle.arrayIdx=-1;
	return YSERR;
}

template <class toFind>
YSRESULT YsHashTable <toFind>::FindNextElement(YsHashElementEnumHandle &handle) const
{
	if(YSTRUE!=IsHandleValid(handle))
	{
		return RewindElementEnumHandle(handle);
	}

	YSSIZE_T hashIdx=handle.hashIdx;
	YSSIZE_T arrayIdx=handle.arrayIdx;

	++arrayIdx;
	if(arrayIdx<table.GetN2(hashIdx))
	{
		handle.hashIdx=hashIdx;
		handle.arrayIdx=arrayIdx;
		return YSOK;
	}

	++hashIdx;
	arrayIdx=0;

	while(hashIdx<HashSize)
	{
		if(0<table.GetN2(hashIdx))
		{
			handle.hashIdx=hashIdx;
			handle.arrayIdx=0;
			return YSOK;
		}
		++hashIdx;
	}

	handle.hashIdx=-1;
	handle.arrayIdx=-1;

	return YSERR;
}

template <class toFind>
YSSIZE_T YsHashTable<toFind>::GetN(void) const
{
	return nElem;
}

template <class toFind>
void YsHashTable<toFind>::MoveToNext(YsHashElementEnumHandle &hd) const
{
	FindNextElement(hd);
}
template <class toFind>
YsHashElementEnumHandle YsHashTable<toFind>::First(void) const
{
	YsHashElementEnumHandle hd;
	RewindElementEnumHandle(hd);
	return hd;
}
template <class toFind>
YsHashElementEnumHandle YsHashTable<toFind>::Null(void) const
{
	return NullHandle();
}
template <class toFind>
toFind &YsHashTable<toFind>::Value(const YsHashElementEnumHandle &hd)
{
	return GetElement(hd);
}
template <class toFind>
const toFind &YsHashTable<toFind>::Value(const YsHashElementEnumHandle &hd) const
{
	return GetElement(hd);
}
template <class toFind>
YSHASHKEY YsHashTable<toFind>::Key(const YsHashElementEnumHandle &hd) const
{
	const auto hashIdx=hd.hashIdx;
	const auto arrayIdx=hd.arrayIdx;
	return table[hashIdx][arrayIdx].key;
}

template <class toFind>
typename YsHashTable<toFind>::HashElement *YsHashTable<toFind>::FindTableIndex(YSSIZE_T &idx1,YSSIZE_T &idx2,YSHASHKEY searchKey)
{
	YSHASHKEY hashId=searchKey%HashSize;
	if(0<table.GetN2(hashId))
	{
		YSSIZE_T i1=0;
		YSSIZE_T i2=table.GetN2(hashId)-1;
		auto entry=table[hashId];
		while(i2-i1>1)
		{
			YSSIZE_T im=(i1+i2)/2;
			if(searchKey<entry[im].key)
			{
				i2=im;
			}
			else
			{
				i1=im;
			}
		}

		if(entry[i1].key==searchKey)
		{
			idx1=hashId;
			idx2=i1;
			return entry;
		}
		else if(entry[i2].key==searchKey)
		{
			idx1=hashId;
			idx2=i2;
			return entry;
		}
	}

	idx1=-1;
	idx2=-1;
	return NULL;
}

template <class toFind>
const typename YsHashTable<toFind>::HashElement *YsHashTable<toFind>::FindTableIndex(YSSIZE_T &idx1,YSSIZE_T &idx2,YSHASHKEY searchKey) const
{
	YSHASHKEY hashId=searchKey%HashSize;
	if(0<table.GetN2(hashId))
	{
		YSSIZE_T i1=0;
		YSSIZE_T i2=table.GetN2(hashId)-1;
		auto entry=table[hashId];
		while(i2-i1>1)
		{
			YSSIZE_T im=(i1+i2)/2;
			if(searchKey<entry[im].key)
			{
				i2=im;
			}
			else
			{
				i1=im;
			}
		}

		if(entry[i1].key==searchKey)
		{
			idx1=hashId;
			idx2=i1;
			return entry;
		}
		else if(entry[i2].key==searchKey)
		{
			idx1=hashId;
			idx2=i2;
			return entry;
		}
	}

	idx1=-1;
	idx2=-1;
	return NULL;
}



template <class toFind>
YSRESULT YsHashTable <toFind>::SelfDiagnostic(void) const
{
	// Check all elements in the table are sorted in search key order
	// Search key and hash id is consistent

	YSSIZE_T i,j,nElemSum;
	YSBOOL err;
	YSRESULT res;
	res=YSOK;

	YsPrintf("Self Diagnostic YsHashTable\n");
	nElemSum=0;
	for(i=0; i<HashSize; i++)
	{
		if(i%20==0)
		{
			YsPrintf("%d/%d\r",(int)i,(int)HashSize);
		}

		err=YSFALSE;
		for(j=0; j<table.GetN2(i); j++)
		{
			if(j>0)
			{
				if(table[i][j-1].key>=table[i][j].key)
				{
					res=YSERR;
					err=YSTRUE;
				}
			}
			if(table[i][j].key%HashSize!=YSHASHKEY(i))
			{
				res=YSERR;
				YsPrintf("##Inconsistencies in hash table %d %d (HashSize=%d)\n",(int)i,table[i][j].key,(int)HashSize);
			}
		}
		if(err==YSTRUE)
		{
			YsPrintf("##Elements are not sorted in key order\n");
			for(j=0; j<table.GetN2(i); j++)
			{
				YsPrintf(" %d",table[i][j].key);
			}
			YsPrintf("\n");
		}
		nElemSum+=table.GetN2(i);
	}
	YsPrintf("%d/%d\n",(int)HashSize,(int)HashSize);

	if(nElemSum!=nElem)
	{
		printf("Number of elements does not match.\n");
	}

	return res;
}


template <class toFind>
void YsHashTable <toFind>::EnableAutoResizing(YSSIZE_T minSize,YSSIZE_T maxSize)
{
	enableAutoResizing=YSTRUE;
	autoResizingMin=minSize;
	autoResizingMax=maxSize;
}

template <class toFind>
void YsHashTable <toFind>::EnableAutoResizing(void)
{
	enableAutoResizing=YSTRUE;
	autoResizingMin=0;  // 0,0 -> Unrestricted auto resizing
	autoResizingMax=0;
}

template <class toFind>
void YsHashTable <toFind>::DisableAutoResizing(void)
{
	enableAutoResizing=YSFALSE;
}

template <class toFind>
int YsHashTable <toFind>::GetNumUnused(void) const
{
	// Temporarily disabled.
	return 0;

//	YSSIZE_T i,n;
//	n=0;
//	for(i=0; i<HashSize; i++)
//	{
//		n+=table[i].GetNUnused();
//	}
//	return (int)n;
}


template <class toFind>
YSRESULT YsHashTable <toFind>::CheckAutoResizingGrow(void)
{
	if(enableAutoResizing==YSTRUE)
	{
		if(autoResizingMax==0 && autoResizingMin==0)  // Unlimited mode
		{
			if(HashSize*4<nElem)
			{
				Resize(nElem);
			}
		}
		else if(HashSize<autoResizingMax && HashSize*4<nElem)
		{
			Resize(YsSmaller <YSSIZE_T> (HashSize*2,autoResizingMax));
		}
	}
	return YSOK;
}

template <class toFind>
YSRESULT YsHashTable <toFind>::CheckAutoResizingShrink(void)
{
	if(enableAutoResizing==YSTRUE)
	{
		if(autoResizingMax==0 && autoResizingMin==0)  // Unlimited mode
		{
			if(nElem<HashSize/4)
			{
				Resize(YsGreater <YSSIZE_T> (1,nElem));
			}
		}
		else if(autoResizingMin<HashSize && nElem<HashSize)
		{
			Resize(YsGreater <YSSIZE_T> (HashSize/2,autoResizingMin));
		}
	}
	return YSOK;
}


////////////////////////////////////////////////////////////

// Sub Sub Group : Group of elements that have same key

template <class toFind,const int minKeyBufSize,const int minItemBufSize>
class YsHashSameKeyGroup
{
public:
	YsArray <unsigned,minKeyBufSize> key;
	YsArray <toFind,minItemBufSize> dat;
	YsList <YsHashSameKeyGroup <toFind,minKeyBufSize,minItemBufSize> > *thisInTheList;
};


extern int YsHashSameKeyGroupCreateDeleteCounter;

// Sub Group : Group of sub groups that have same keysum

template <class toFind,const int minKeyBufSize,const int minItemBufSize>
class YsHashSameKeySumGroup
{
public:
	YsHashSameKeySumGroup();
	~YsHashSameKeySumGroup();
	YsList <YsHashSameKeyGroup <toFind,minKeyBufSize,minItemBufSize> > *elmList;
};

template <class toFind,const int minKeyBufSize,const int minItemBufSize>
YsHashSameKeySumGroup<toFind,minKeyBufSize,minItemBufSize>::YsHashSameKeySumGroup()
{
	YsHashSameKeyGroupCreateDeleteCounter++;
	elmList=NULL;
}

template <class toFind,const int minKeyBufSize,const int minItemBufSize>
YsHashSameKeySumGroup<toFind,minKeyBufSize,minItemBufSize>::~YsHashSameKeySumGroup()
{
	YsHashSameKeyGroupCreateDeleteCounter--;
	elmList->DeleteList();
}



//

template <class toFind,const int minKeyBufSize,const int minItemBufSize>
class YsMultiKeyHashElementEnumHandle
{
public:
	YsHashElementEnumHandle outerHandle;
	YsList <YsHashSameKeyGroup<toFind,minKeyBufSize,minItemBufSize> > *currentSubSubGroup;
};



// Table main body

template <class toFind,const int minKeyBufSize=1,const int minItemBufSize=1>
class YsMultiKeyHash  : protected YsHashTable <YsHashSameKeySumGroup<toFind,minKeyBufSize,minItemBufSize> *>
{
protected:
// Complying with a stupid change made in g++ 3.4
using YsHashTable <YsHashSameKeySumGroup <toFind,minKeyBufSize,minItemBufSize> *>::HashSize;
using YsHashTable <YsHashSameKeySumGroup <toFind,minKeyBufSize,minItemBufSize> *>::table;
// Complying with a stupid change made in g++ 3.4

public:
	YsMultiKeyHash(YSSIZE_T hashSize=16);
	~YsMultiKeyHash();

	YSRESULT PrepareTable(void);
	YSRESULT PrepareTable(YSSIZE_T hashSize);

	YSRESULT AddElement(int nKey,const unsigned key[],toFind element);

	YSRESULT DeleteElement(int nKey,const unsigned key[],toFind element);

	YSRESULT DeleteKey(int nKey,const unsigned key[]);

	const YsArray <toFind,minItemBufSize> *FindElement(int nKey,const unsigned key[]) const;

	YSRESULT RewindElementEnumHandle
	    (YsMultiKeyHashElementEnumHandle <toFind,minKeyBufSize,minItemBufSize> &handle) const;
	YSRESULT FindNextElement
	    (YsMultiKeyHashElementEnumHandle <toFind,minKeyBufSize,minItemBufSize> &handle) const;
	const YsArray <toFind,minItemBufSize> *GetElement
	    (YsMultiKeyHashElementEnumHandle <toFind,minKeyBufSize,minItemBufSize> &handle) const;
	const YsArray <toFind,minItemBufSize> *GetSeachKey
	    (YsMultiKeyHashElementEnumHandle <toFind,minKeyBufSize,minItemBufSize> &handle) const;

protected:
	unsigned GetKeySum(YSSIZE_T nKey,const unsigned key[]) const;
	YSBOOL CheckSameKey(YSSIZE_T nKey1,const unsigned key1[],YSSIZE_T nKey2,const unsigned key2[]) const;
	YSRESULT FindAddSameKeyGroup
	   (unsigned &keySum,
		YsArray <unsigned,minKeyBufSize> &key,
	    YsHashSameKeySumGroup <toFind,minKeyBufSize,minItemBufSize> *&sameKeySumGroup,
	    YsHashSameKeyGroup <toFind,minKeyBufSize,minItemBufSize> *&sameKeyGroup);
	YSRESULT FindSameKeyGroup
	   (unsigned &keySum,
		YsArray <unsigned,minKeyBufSize> &key,
	    YsHashSameKeySumGroup <toFind,minKeyBufSize,minItemBufSize> *&sameKeySumGroup,
	    YsHashSameKeyGroup <toFind,minKeyBufSize,minItemBufSize> *&sameKeyGroup) const;
};


template <class toFind,const int minKeyBufSize,const int minItemBufSize>
YsMultiKeyHash <toFind,minKeyBufSize,minItemBufSize>::
	YsMultiKeyHash(YSSIZE_T hashSize) : YsHashTable <YsHashSameKeySumGroup<toFind,minKeyBufSize,minItemBufSize> *>(hashSize)
{
}

template <class toFind,const int minKeyBufSize,const int minItemBufSize>
	YsMultiKeyHash <toFind,minKeyBufSize,minItemBufSize>::~YsMultiKeyHash()
{
	int i,j;
	for(i=0; i<HashSize; i++)
	{
		for(j=0; j<table.GetN2(i); j++)
		{
			delete table[i][j].dat;
		}
	}
}



template <class toFind,const int minKeyBufSize,const int minItemBufSize>
YSRESULT YsMultiKeyHash <toFind,minKeyBufSize,minItemBufSize>::PrepareTable(void)
{
	int i,j;
	for(i=0; i<HashSize; i++)
	{
		for(j=0; j<table.GetN2(i); j++)
		{
			delete table[i][j].dat;
		}
	}

	return YsHashTable<YsHashSameKeySumGroup<toFind,minKeyBufSize,minItemBufSize> *>::PrepareTable();
}



template <class toFind,const int minKeyBufSize,const int minItemBufSize>
YSRESULT YsMultiKeyHash <toFind,minKeyBufSize,minItemBufSize>::PrepareTable(YSSIZE_T hashSize)
{
	YSSIZE_T i,j;
	for(i=0; i<HashSize; i++)
	{
		for(j=0; j<table.GetN2(i); j++)
		{
			delete table[i][j].dat;
		}
	}
	return YsHashTable<YsHashSameKeySumGroup<toFind,minKeyBufSize,minItemBufSize> *>::PrepareTable(hashSize);
}



template <class toFind,const int minKeyBufSize,const int minItemBufSize>
YSRESULT YsMultiKeyHash <toFind,minKeyBufSize,minItemBufSize>::
	AddElement(int nKey,const unsigned unsortedKey[],toFind element)
{
	unsigned keySum;
	YsArray <unsigned,minKeyBufSize> key;
	YsHashSameKeySumGroup<toFind,minKeyBufSize,minItemBufSize> *sameKeySumGroup;
	YsHashSameKeyGroup<toFind,minKeyBufSize,minItemBufSize> *sameKeyGroup;

	key.Set(nKey,unsortedKey);

	if(FindAddSameKeyGroup(keySum,key,sameKeySumGroup,sameKeyGroup)==YSOK)
	{
		sameKeyGroup->dat.AppendItem(element);
		return YSOK;
	}
	return YSERR;
}



template <class toFind,const int minKeyBufSize,const int minItemBufSize>
YSRESULT YsMultiKeyHash <toFind,minKeyBufSize,minItemBufSize>::
	DeleteElement(int nKey,const unsigned unsortedKey[],toFind element)
{
	unsigned keySum;
	YsArray <unsigned,minKeyBufSize> key;
	YsHashSameKeySumGroup <toFind,minKeyBufSize,minItemBufSize> *sameKeySumGroup;
	YsHashSameKeyGroup <toFind,minKeyBufSize,minItemBufSize> *sameKeyGroup;

	key.Set(nKey,unsortedKey);  // key is sorted inside FindSameKeyGroup func

	if(FindSameKeyGroup(keySum,key,sameKeySumGroup,sameKeyGroup)==YSOK)
	{
		for(int i=0; i<sameKeyGroup->dat.GetN(); i++)
		{
			if(sameKeyGroup->dat[i]==element)
			{
				sameKeyGroup->dat.Delete(i);

				if(sameKeyGroup->dat.GetN()==0)
				{
					YsList <YsHashSameKeyGroup<toFind,minKeyBufSize,minItemBufSize> > *seek;

					//YSBOOL found;
					//found=YSFALSE;
					//for(seek=sameKeySumGroup->elmList; seek!=NULL; seek=seek->Next())
					//{
					//	if(&seek->dat==sameKeyGroup)
					//	{
					//		sameKeySumGroup->elmList=seek->DeleteFromList();
					//		found=YSTRUE;
					//		break;
					//	}
					//}
					//if(found!=YSTRUE)
					//{
					//	YsErrOut("YsMultiKeyHash::DeleteElement()\n");
					//	YsErrOut("  Internal error (1)\n");
					//}

					// Above part is replaced with >>  2000/06/05
					seek=sameKeyGroup->thisInTheList;
					if(seek->SeekTop()==sameKeySumGroup->elmList)
					{
						sameKeySumGroup->elmList=seek->DeleteFromList();
					}
					else
					{
						YsErrOut("YsMultiKeyHash::DeleteElement()\n");
						YsErrOut("  Internal error (1)\n");
					}
					// <<


					if(sameKeySumGroup->elmList==NULL)
					{
						YsHashTable
						    <YsHashSameKeySumGroup <toFind,minKeyBufSize,minItemBufSize> *>::
						        DeleteElement(keySum,sameKeySumGroup);
						delete sameKeySumGroup;
					}
				}

				return YSOK;
			}
		}
	}
	YsErrOut("YsMultiKeyHash::DeleteElement()\n");
	YsErrOut("  Tried to delete an element that is not listed.\n");
	return YSERR;
}



template <class toFind,const int minKeyBufSize,const int minItemBufSize>
YSRESULT YsMultiKeyHash <toFind,minKeyBufSize,minItemBufSize>::
	DeleteKey(int nKey,const unsigned unsortedKey[])
{
	unsigned keySum;
	YsArray <unsigned,minKeyBufSize> key;
	YsHashSameKeySumGroup<toFind,minKeyBufSize,minItemBufSize> *sameKeySumGroup;
	YsHashSameKeyGroup<toFind,minKeyBufSize,minItemBufSize> *sameKeyGroup;

	key.Set(nKey,unsortedKey);  // key is sorted inside FindSameKeyGroup func

	if(FindSameKeyGroup(keySum,key,sameKeySumGroup,sameKeyGroup)==YSOK)
	{
		YsList <YsHashSameKeyGroup<toFind,minKeyBufSize,minItemBufSize> > *seek;

		seek=sameKeyGroup->thisInTheList;
		if(seek->SeekTop()==sameKeySumGroup->elmList)
		{
			sameKeySumGroup->elmList=seek->DeleteFromList();
		}
		else
		{
			YsErrOut("YsMultiKeyHash::DeleteKey()\n");
			YsErrOut("  Internal error (1)\n");
		}

		if(sameKeySumGroup->elmList==NULL)
		{
			YsHashTable
			    <YsHashSameKeySumGroup<toFind,minKeyBufSize,minItemBufSize> *>::
			        DeleteElement(keySum,sameKeySumGroup);
			delete sameKeySumGroup;
		}

		return YSOK;
	}
	YsErrOut("YsMultiKeyHash::DeleteKey()\n");
	YsErrOut("  Tried To Delete a key that is not listed.\n");
	return YSERR;
}



template <class toFind,const int minKeyBufSize,const int minItemBufSize>
const YsArray <toFind,minItemBufSize> *YsMultiKeyHash <toFind,minKeyBufSize,minItemBufSize>::
	FindElement(int nKey,const unsigned unsortedKey[]) const
{
	unsigned keySum;
	YsArray <unsigned,minKeyBufSize> key;
	YsHashSameKeySumGroup<toFind,minKeyBufSize,minItemBufSize> *sameKeySumGroup;
	YsHashSameKeyGroup<toFind,minKeyBufSize,minItemBufSize> *sameKeyGroup;

	key.Set(nKey,unsortedKey);

	if(FindSameKeyGroup(keySum,key,sameKeySumGroup,sameKeyGroup)==YSOK)
	{
		return &sameKeyGroup->dat;
	}
	else
	{
		return NULL;
	}
}



template <class toFind,const int minKeyBufSize,const int minItemBufSize>
YSRESULT YsMultiKeyHash <toFind,minKeyBufSize,minItemBufSize>::
	RewindElementEnumHandle
    	(YsMultiKeyHashElementEnumHandle <toFind,minKeyBufSize,minItemBufSize> &handle) const
{
	YsHashTable <YsHashSameKeySumGroup<toFind,minKeyBufSize,minItemBufSize> *>::
	    RewindElementEnumHandle(handle.outerHandle);

	YsHashSameKeySumGroup<toFind,minKeyBufSize,minItemBufSize> *subElem;

	handle.currentSubSubGroup=NULL;
	while(handle.currentSubSubGroup==NULL)
	{
		if(YsHashTable
		       <YsHashSameKeySumGroup<toFind,minKeyBufSize,minItemBufSize> *>::
		           GetElement(subElem,handle.outerHandle)==YSOK)
		{
			if(subElem->elmList!=NULL)
			{
				handle.currentSubSubGroup=subElem->elmList;
				return YSOK;
			}
		}
		else if(YsHashTable
		       <YsHashSameKeySumGroup<toFind,minKeyBufSize,minItemBufSize> *>::
		           FindNextElement(handle.outerHandle)!=YSOK)
		{
			return YSERR;
		}
	}
	return YSERR;
}



template <class toFind,const int minKeyBufSize,const int minItemBufSize>
YSRESULT YsMultiKeyHash <toFind,minKeyBufSize,minItemBufSize>::
	FindNextElement
    	(YsMultiKeyHashElementEnumHandle <toFind,minKeyBufSize,minItemBufSize> &handle) const
{
	if(handle.currentSubSubGroup!=NULL)
	{
		handle.currentSubSubGroup=handle.currentSubSubGroup->Next();
		if(handle.currentSubSubGroup!=NULL)
		{
			return YSOK;
		}
	}

	// Falling into this point means either one of:
	//    1. Finding first element
	//    2. One SubGroup is over, and have to move to the next SubGroup
	while(handle.currentSubSubGroup==NULL)
	{
		YsHashSameKeySumGroup<toFind,minKeyBufSize,minItemBufSize> *subGroup;

		if(FindNextElement(subGroup,handle.outerHandle)==YSOK)
		{
			handle.currentSubSubGroup=subGroup->elmList;
		}
		else
		{
			return YSERR;
		}
	}
	return YSOK;
}



template <class toFind,const int minKeyBufSize,const int minItemBufSize>
const YsArray <toFind,minItemBufSize> *
    YsMultiKeyHash <toFind,minKeyBufSize,minItemBufSize>::
	    GetElement(YsMultiKeyHashElementEnumHandle <toFind,minKeyBufSize,minItemBufSize> &handle) const
{
	if(handle.currentSubSubGroup!=NULL)
	{
		return &handle.currentSubSubGroup->dat;
	}
	return NULL;
}



template <class toFind,const int minKeyBufSize,const int minItemBufSize>
const YsArray <toFind,minItemBufSize> *
    YsMultiKeyHash <toFind,minKeyBufSize,minItemBufSize>::
	    GetSeachKey
	        (YsMultiKeyHashElementEnumHandle <toFind,minKeyBufSize,minItemBufSize> &handle) const
{
	if(handle.currentSubSubGroup!=NULL)
	{
		return &handle.currentSubSubGroup->key;
	}
	return NULL;
}



template <class toFind,const int minKeyBufSize,const int minItemBufSize>
unsigned
    YsMultiKeyHash <toFind,minKeyBufSize,minItemBufSize>::
	    GetKeySum(YSSIZE_T nKey,const unsigned key[]) const
{
	int i;
	unsigned sum;
	sum=0;
	for(i=0; i<nKey; i++)
	{
		sum+=key[i];
	}
	return sum;
}



template <class toFind,const int minKeyBufSize,const int minItemBufSize>
YSBOOL
    YsMultiKeyHash <toFind,minKeyBufSize,minItemBufSize>::
	    CheckSameKey(YSSIZE_T nKey1,const unsigned key1[],YSSIZE_T nKey2,const unsigned key2[]) const
{
	if(nKey1==nKey2)
	{
		int i;
		for(i=0; i<nKey1; i++)
		{
			if(key1[i]!=key2[i])
			{
				return YSFALSE;
			}
		}
		return YSTRUE;
	}
	return YSFALSE;
}



template <class toFind,const int minKeyBufSize,const int minItemBufSize>
YSRESULT
    YsMultiKeyHash <toFind,minKeyBufSize,minItemBufSize>::
		FindAddSameKeyGroup
	       (unsigned &keySum,
			YsArray <unsigned,minKeyBufSize> &key,
	        YsHashSameKeySumGroup <toFind,minKeyBufSize,minItemBufSize> *&sameKeySumGroup,
	        YsHashSameKeyGroup <toFind,minKeyBufSize,minItemBufSize> *&sameKeyGroup)
{
	if(FindSameKeyGroup(keySum,key,sameKeySumGroup,sameKeyGroup)==YSOK)
	{
		return YSOK;
	}
	else
	{
		if(sameKeySumGroup==NULL)
		{
			sameKeySumGroup=new YsHashSameKeySumGroup<toFind,minKeyBufSize,minItemBufSize>;
			YsHashTable
		       <YsHashSameKeySumGroup<toFind,minKeyBufSize,minItemBufSize> *>::
		           AddElement(keySum,sameKeySumGroup);  // Pray for no error  :-)
		}

		YsList <YsHashSameKeyGroup<toFind,minKeyBufSize,minItemBufSize> > *neo;

		neo=new YsList <YsHashSameKeyGroup<toFind,minKeyBufSize,minItemBufSize> >;
		neo->dat.key=key;
		neo->dat.thisInTheList=neo;

		sameKeySumGroup->elmList=sameKeySumGroup->elmList->Append(neo);

		sameKeyGroup=&neo->dat;

		return YSOK;
	}
}



template <class toFind,const int minKeyBufSize,const int minItemBufSize>
YSRESULT
    YsMultiKeyHash <toFind,minKeyBufSize,minItemBufSize>::
		FindSameKeyGroup
	       (unsigned &keySum,
			YsArray <unsigned,minKeyBufSize> &key,
	        YsHashSameKeySumGroup <toFind,minKeyBufSize,minItemBufSize> *&sameKeySumGroup,
	        YsHashSameKeyGroup <toFind,minKeyBufSize,minItemBufSize> *&sameKeyGroup)
	            const
{
	sameKeySumGroup=NULL;
	sameKeyGroup=NULL;

	keySum=GetKeySum(key.GetNumItem(),key.GetArray());

	YsQuickSort <unsigned,void *> (key.GetNumItem(),key.GetEditableArray());

	if(YsHashTable
		  <YsHashSameKeySumGroup<toFind,minKeyBufSize,minItemBufSize> *>::
		      FindElement(sameKeySumGroup,keySum)==YSOK)
		          // sameKeySumGroup : Group of sub elements that have same keysum
	{
		YsList <YsHashSameKeyGroup<toFind,minKeyBufSize,minItemBufSize> > *seek;

		sameKeyGroup=NULL;
		for(seek=sameKeySumGroup->elmList; seek!=NULL; seek=seek->Next())
		{
			YSSIZE_T nKey1,nKey2;
			const unsigned *key1,*key2;

			nKey1=seek->dat.key.GetNumItem();
			key1=seek->dat.key.GetArray();
			nKey2=key.GetNumItem();
			key2=key.GetArray();

			if(CheckSameKey(nKey1,key1,nKey2,key2)==YSTRUE)
			{
				sameKeyGroup=&seek->dat;
				return YSOK;
			}
		}
	}
	return YSERR;
}



////////////////////////////////////////////////////////////

template <int nKeyLng>
class YsFixedLengthHashBase
{
protected:
	YSBOOL orderSensitive;

	inline YSRESULT OrderKey(YSHASHKEY orderedKey[],const YSHASHKEY unorderedKey[]) const;
	inline YSHASHKEY KeySum(const YSHASHKEY orderedKey[]) const;
	inline YSBOOL SameKey(const YSHASHKEY orderedKey1[],const YSHASHKEY orderedKey2[]) const;
	inline void CopyKey(YSHASHKEY to[],const YSHASHKEY from[]) const;
public:
	YsFixedLengthHashBase();
	void SetOrderSensitivity(YSBOOL sensitivity);
};


template <int nKeyLng>
YsFixedLengthHashBase<nKeyLng>::YsFixedLengthHashBase()
{
	this->orderSensitive=YSFALSE;
}

template <int nKeyLng>
void YsFixedLengthHashBase<nKeyLng>::SetOrderSensitivity(YSBOOL sensitivity)
{
	this->orderSensitive=sensitivity;
}

// 2013/12/22 Confirmed partial specialization works on VS2012 and g++ 4.6.
template <>
inline YSRESULT YsFixedLengthHashBase <1>::OrderKey(YSHASHKEY orderedKey[],const YSHASHKEY unorderedKey[]) const
{
	orderedKey[0]=unorderedKey[0];
	return YSOK;
}

template <>
inline YSRESULT YsFixedLengthHashBase <2>::OrderKey(YSHASHKEY orderedKey[],const YSHASHKEY unorderedKey[]) const
{
	if(YSTRUE==orderSensitive)
	{
		orderedKey[0]=unorderedKey[0];
		orderedKey[1]=unorderedKey[1];
		return YSOK;
	}

	if(unorderedKey[0]<unorderedKey[1])
	{
		orderedKey[0]=unorderedKey[0];
		orderedKey[1]=unorderedKey[1];
	}
	else
	{
		orderedKey[0]=unorderedKey[1];
		orderedKey[1]=unorderedKey[0];
	}
	return YSOK;
}

template <>
inline YSRESULT YsFixedLengthHashBase <3>::OrderKey(YSHASHKEY orderedKey[],const YSHASHKEY unorderedKey[]) const
{
	if(YSTRUE==orderSensitive)
	{
		orderedKey[0]=unorderedKey[0];
		orderedKey[1]=unorderedKey[1];
		orderedKey[2]=unorderedKey[2];
		return YSOK;
	}

	if(unorderedKey[0]<unorderedKey[1] && unorderedKey[0]<unorderedKey[2])
	{
		orderedKey[0]=unorderedKey[0];
		if(unorderedKey[1]<unorderedKey[2])
		{
			orderedKey[1]=unorderedKey[1];
			orderedKey[2]=unorderedKey[2];
		}
		else
		{
			orderedKey[1]=unorderedKey[2];
			orderedKey[2]=unorderedKey[1];
		}
	}
	else if(unorderedKey[1]<unorderedKey[2])
	{
		orderedKey[0]=unorderedKey[1];
		if(unorderedKey[0]<unorderedKey[2])
		{
			orderedKey[1]=unorderedKey[0];
			orderedKey[2]=unorderedKey[2];
		}
		else
		{
			orderedKey[1]=unorderedKey[2];
			orderedKey[2]=unorderedKey[0];
		}
	}
	else
	{
		orderedKey[0]=unorderedKey[2];
		if(unorderedKey[0]<unorderedKey[1])
		{
			orderedKey[1]=unorderedKey[0];
			orderedKey[2]=unorderedKey[1];
		}
		else
		{
			orderedKey[1]=unorderedKey[1];
			orderedKey[2]=unorderedKey[0];
		}
	}
	return YSOK;
}

template <int nKeyLng>
inline YSRESULT YsFixedLengthHashBase <nKeyLng>::OrderKey(YSHASHKEY orderedKey[],const YSHASHKEY unorderedKey[]) const
{
	for(int i=0; i<nKeyLng; i++)
	{
		orderedKey[i]=unorderedKey[i];
	}

	if(YSTRUE!=orderSensitive)
	{
		return YsQuickSort <YSHASHKEY,YSHASHKEY> (nKeyLng,orderedKey,NULL);
	}
	else
	{
		return YSOK;
	}
}

template <>
inline YSHASHKEY YsFixedLengthHashBase <1>::KeySum(const YSHASHKEY key[]) const
{
	return key[0];
}

template <>
inline YSHASHKEY YsFixedLengthHashBase <2>::KeySum(const YSHASHKEY key[]) const
{
	return key[0]+key[1];
}

template <>
inline YSHASHKEY YsFixedLengthHashBase <3>::KeySum(const YSHASHKEY key[]) const
{
	return key[0]+key[1]+key[2];
}

template <int nKeyLng>
inline YSHASHKEY YsFixedLengthHashBase <nKeyLng>::KeySum(const YSHASHKEY key[]) const
{
	YSHASHKEY sum=0;
	for(int i=0; i<nKeyLng; ++i)
	{
		sum+=key[i];
	}
	return sum;
}

template <>
inline YSBOOL YsFixedLengthHashBase <1>::SameKey(const YSHASHKEY orderedKey1[],const YSHASHKEY orderedKey2[]) const
{
	if(orderedKey2[0]==orderedKey1[0])
	{
		return YSTRUE;
	}
	return YSFALSE;
}

template <>
inline YSBOOL YsFixedLengthHashBase <2>::SameKey(const YSHASHKEY orderedKey1[],const YSHASHKEY orderedKey2[]) const
{
	if(orderedKey2[0]==orderedKey1[0] &&
	   orderedKey2[1]==orderedKey1[1])
	{
		return YSTRUE;
	}
	return YSFALSE;
}

template <>
inline YSBOOL YsFixedLengthHashBase <3>::SameKey(const YSHASHKEY orderedKey1[],const YSHASHKEY orderedKey2[]) const
{
	if(orderedKey2[0]==orderedKey1[0] &&
	   orderedKey2[1]==orderedKey1[1] &&
	   orderedKey2[2]==orderedKey1[2])
	{
		return YSTRUE;
	}
	return YSFALSE;
}

template <int nKeyLng>
inline YSBOOL YsFixedLengthHashBase <nKeyLng>::SameKey(const YSHASHKEY orderedKey1[],const YSHASHKEY orderedKey2[]) const
{
	for(int i=0; i<nKeyLng; ++i)
	{
		if(orderedKey1[i]!=orderedKey2[i])
		{
			return YSFALSE;
		}
	}
	return YSTRUE;
}

template <int nKeyLng>
inline void YsFixedLengthHashBase <nKeyLng>::CopyKey(YSHASHKEY to[],const YSHASHKEY from[]) const
{
	for(int i=0; i<nKeyLng; ++i)
	{
		to[i]=from[i];
	}
}

////////////////////////////////////////////////////////////

template <class toFind,int nKeyLng>
class YsFixedLengthHashTable : public YsFixedLengthHashBase <nKeyLng>
{
public:
	typedef YsHashElementEnumHandle ElemEnumHandle;

private:
	class HashElement
	{
	public:
		YSHASHKEY key[nKeyLng];
		toFind objective;
	};

	YSBOOL enableAutoResizing;
	YSSIZE_T autoResizingMin,autoResizingMax;

	YSSIZE_T nElem;
	YsArray <YsArray <HashElement,0,int> > keyList;

	// Support for STL-like iterator >>
public:
	class iterator
	{
	friend class YsFixedLengthHashTable <toFind,nKeyLng>;

	private:
		YsFixedLengthHashTable <toFind,nKeyLng> *table;
		ElemEnumHandle hd;
		YSBOOL ended;
	public:
		inline iterator &operator++()
		{
			if(YSTRUE!=ended)
			{
				if(YSOK!=table->FindNextElement(hd))
				{
					ended=YSTRUE;
				}
			}
			return *this;
		}
		inline iterator operator++(int)
		{
			iterator copy=*this;
			if(YSTRUE!=ended)
			{
				if(YSOK!=table->FindNextElement(hd))
				{
					ended=YSTRUE;
				}
			}
			return copy;
		}
		inline bool operator==(const iterator &from)
		{
			if(this->ended==YSTRUE && from.ended==YSTRUE)
			{
				return true;
			}
			else if(this->ended!=YSTRUE && from.ended!=YSTRUE &&
			        this->hashIdx==from.hashIdx && this->arrayIdx==from.arrayIdx)
			{
				return true;
			}
			return false;
		}
		inline bool operator!=(const iterator &from)
		{
			if(this->ended==YSTRUE && from.ended==YSTRUE)
			{
				return false;
			}
			else if(this->ended!=YSTRUE && from.ended!=YSTRUE &&
			        this->hd.hashIdx==from.hd.hashIdx && this->hd.arrayIdx==from.hd.arrayIdx)
			{
				return false;
			}
			return true;
		}
		inline toFind &operator*()
		{
			return table->GetElement(hd);
		}
	};
	class const_iterator
	{
	friend class YsFixedLengthHashTable <toFind,nKeyLng>;

	private:
		const YsFixedLengthHashTable <toFind,nKeyLng> *table;
		ElemEnumHandle hd;
		YSBOOL ended;
	public:
		inline const_iterator &operator++()
		{
			if(YSTRUE!=ended)
			{
				if(YSOK!=table->FindNextElement(hd))
				{
					ended=YSTRUE;
				}
			}
			return *this;
		}
		inline const_iterator operator++(int)
		{
			const_iterator copy=*this;
			if(YSTRUE!=ended)
			{
				if(YSOK!=table->FindNextElement(hd))
				{
					ended=YSTRUE;
				}
			}
			return copy;
		}
		inline bool operator==(const const_iterator &from)
		{
			if(this->ended==YSTRUE && from.ended==YSTRUE)
			{
				return true;
			}
			else if(this->ended!=YSTRUE && from.ended!=YSTRUE &&
			        this->hd.hashIdx==from.hd.hashIdx && this->hd.arrayIdx==from.hd.arrayIdx)
			{
				return true;
			}
			return false;
		}
		inline bool operator!=(const const_iterator &from)
		{
			if(this->ended==YSTRUE && from.ended==YSTRUE)
			{
				return false;
			}
			else if(this->ended!=YSTRUE && from.ended!=YSTRUE &&
			        this->hashIdx==from.hashIdx && this->arrayIdx==from.arrayIdx)
			{
				return false;
			}
			return true;
		}
		inline const toFind &operator*()
		{
			return table->GetElement(hd);
		}
	};
	iterator begin()
	{
		iterator iter;
		iter.table=this;
		iter.ended=YSFALSE;
		if(YSOK!=RewindElementEnumHandle(iter.hd))
		{
			iter.ended=YSTRUE;
		}
		return iter;
	}
	iterator end()
	{
		iterator iter;
		iter.table=this;
		iter.ended=YSTRUE;
		return iter;
	}
	const_iterator begin() const
	{
		const_iterator iter;
		iter.table=this;
		iter.ended=YSFALSE;
		if(YSOK!=RewindElementEnumHandle(iter.hd))
		{
			iter.ended=YSTRUE;
		}
		return iter;
	}
	const_iterator end() const
	{
		const_iterator iter;
		iter.table=this;
		iter.ended=YSTRUE;
		return iter;
	}
	// Support for STL-like iterator <<

public:
	inline YsFixedLengthHashTable(YSSIZE_T hSize=16);
	inline YsFixedLengthHashTable(const YsFixedLengthHashTable <toFind,nKeyLng> &incoming);
	inline YsFixedLengthHashTable(YsFixedLengthHashTable <toFind,nKeyLng> &&incoming);
	inline YsFixedLengthHashTable <toFind,nKeyLng> &operator=(const YsFixedLengthHashTable <toFind,nKeyLng> &incoming);
	inline YsFixedLengthHashTable <toFind,nKeyLng> &operator=(YsFixedLengthHashTable <toFind,nKeyLng> &&incoming);
	inline void CopyFrom(const YsFixedLengthHashTable <toFind,nKeyLng> &incoming);
	inline void MoveFrom(YsFixedLengthHashTable <toFind,nKeyLng> &incoming);

	inline void EnableAutoResizing(void);
	inline void DisableAutoResizing(void);
	inline void CleanUpThin(void);
	inline YSRESULT PrepareTable(YSSIZE_T hashSize);
	inline YSRESULT Resize(YSSIZE_T hashSize);
	inline YSRESULT CollectGarbage(void);


	inline YSRESULT Add(int nKey,const YSHASHKEY unorderedKey[],const toFind &objective);
	inline YSRESULT Update(int nKey,const YSHASHKEY unorderedKey[],const toFind &objective);
	inline YSRESULT Delete(int nKey,const YSHASHKEY unorderedKey[]);
	inline toFind *Find(int nKey,const YSHASHKEY unorderedKey[]);
	inline const toFind *Find(int nKey,const YSHASHKEY unorderedKey[]) const;
	inline YSRESULT Find(toFind &elem,int nKey,const YSHASHKEY unorderedKey[]) const;

	inline YSSIZE_T GetHashSize(void) const;

	YSBOOL IsHandleValid(ElemEnumHandle hd) const;
	ElemEnumHandle FindHandle(int nKey,const YSHASHKEY unorderedKey[]) const;

	inline YSBOOL CheckKeyExist(int nKey,const YSHASHKEY unorderedKey[]) const;

	/*! Returns a null handle.  You can iterate through keys by the following loop.
	    for(auto handle=hash.NullHandle(); YSOK==FindNextKey(handle); )
	    {
	        // Do something useful.
	    }
	*/
	ElemEnumHandle NullHandle(void) const;
	YSRESULT RewindElementEnumHandle(ElemEnumHandle &elHd) const;
	YSRESULT FindNextElement(ElemEnumHandle &elHd) const;
	YSRESULT FindNextKey(ElemEnumHandle &elHd) const;
	toFind &GetElement(ElemEnumHandle &elHd);
	const toFind &GetElement(ElemEnumHandle &elHd) const;
	const YSHASHKEY *GetKey(ElemEnumHandle &elHd) const;

	inline YSRESULT CheckAutoResizingGrow(void);
	inline YSRESULT CheckAutoResizingShrink(void);


	// Needed for YsUnidirectionalHandleIterator and YsUnidirectionalHandleEnumerator
	inline void MoveToNext(ElemEnumHandle &elHd) const
	{
		if(YSOK!=FindNextElement(elHd))
		{
			elHd=NullHandle();
		}
	}
	inline ElemEnumHandle First(void) const
	{
		auto elHd=NullHandle();
		RewindElementEnumHandle(elHd);
		return elHd;
	}
	inline ElemEnumHandle Null(void) const
	{
		return NullHandle();
	}
	inline toFind &Value(ElemEnumHandle elHd)
	{
		return GetElement(elHd);
	}
	inline const toFind &Value(ElemEnumHandle elHd) const
	{
		return GetElement(elHd);
	}

	YsUnidirectionalHandleEnumerator <YsFixedLengthHashTable <toFind,nKeyLng>,ElemEnumHandle> AllHandle(void) const;


	/*! Old naming convention.  Use Add instead. */
	inline YSRESULT AddElement(int nKey,const YSHASHKEY unorderedKey[],const toFind &objective);
	/*! Old naming convention.  Use Update instead. */
	inline YSRESULT UpdateElement(int nKey,const YSHASHKEY unorderedKey[],const toFind &objective);
	/*! Old naming convention.  Use Delete instead. */
	inline YSRESULT DeleteElement(int nKey,const YSHASHKEY unorderedKey[]);
	/*! Old naming convention.  Use Delete instead. */
	inline YSRESULT DeleteKey(int nKey,const YSHASHKEY unorderedKey[]);
	/*! Old naming convention.  Use Find instead. */
	inline toFind *FindElement(int nKey,const YSHASHKEY unorderedKey[]);
	/*! Old naming convention.  Use Find instead. */
	inline const toFind *FindElement(int nKey,const YSHASHKEY unorderedKey[]) const;
	/*! Old naming convention.  Use Find instead. */
	inline YSRESULT FindElement(toFind &elem,int nKey,const YSHASHKEY unorderedKey[]) const;
};

////////////////////////////////////////////////////////////
// Support for STL-like iterator >>
template <class toFind,int nKeyLng>
inline typename YsFixedLengthHashTable <toFind,nKeyLng>::iterator begin(YsFixedLengthHashTable <toFind,nKeyLng> &table)
{
	return table.begin();
}

template <class toFind,int nKeyLng>
inline typename YsFixedLengthHashTable <toFind,nKeyLng>::iterator end(YsFixedLengthHashTable <toFind,nKeyLng> &table)
{
	return table.end();
}

template <class toFind,int nKeyLng>
inline typename YsFixedLengthHashTable <toFind,nKeyLng>::const_iterator begin(const YsFixedLengthHashTable <toFind,nKeyLng> &table)
{
	return table.begin();
}

template <class toFind,int nKeyLng>
inline typename YsFixedLengthHashTable <toFind,nKeyLng>::const_iterator end(const YsFixedLengthHashTable <toFind,nKeyLng> &table)
{
	return table.end();
}
// Support for STL-like iterator <<
////////////////////////////////////////////////////////////

template <class toFind,int nKeyLng>
YsFixedLengthHashTable <toFind,nKeyLng>::YsFixedLengthHashTable(YSSIZE_T hSize)
{
	enableAutoResizing=YSTRUE;
	autoResizingMin=0;
	autoResizingMax=0;

	PrepareTable(hSize);
}

template <class toFind,int nKeyLng>
inline YsFixedLengthHashTable<toFind,nKeyLng>::YsFixedLengthHashTable(const YsFixedLengthHashTable <toFind,nKeyLng> &incoming)
{
	CopyFrom(incoming);
}
template <class toFind,int nKeyLng>
inline YsFixedLengthHashTable<toFind,nKeyLng>::YsFixedLengthHashTable(YsFixedLengthHashTable <toFind,nKeyLng> &&incoming)
{
	MoveFrom(incoming);
}
template <class toFind,int nKeyLng>
inline YsFixedLengthHashTable <toFind,nKeyLng> &YsFixedLengthHashTable<toFind,nKeyLng>::operator=(const YsFixedLengthHashTable <toFind,nKeyLng> &incoming)
{
	CopyFrom(incoming);
	return *this;
}
template <class toFind,int nKeyLng>
inline YsFixedLengthHashTable <toFind,nKeyLng> &YsFixedLengthHashTable<toFind,nKeyLng>::operator=(YsFixedLengthHashTable <toFind,nKeyLng> &&incoming)
{
	MoveFrom(incoming);
	return *this;
}
template <class toFind,int nKeyLng>
inline void YsFixedLengthHashTable<toFind,nKeyLng>::CopyFrom(const YsFixedLengthHashTable <toFind,nKeyLng> &incoming)
{
	enableAutoResizing=incoming.enableAutoResizing;
	autoResizingMin=incoming.autoResizingMin;
	autoResizingMax=incoming.autoResizingMax;

	nElem=incoming.nElem;
	keyList=incoming.keyList;
}
template <class toFind,int nKeyLng>
inline void YsFixedLengthHashTable<toFind,nKeyLng>::MoveFrom(YsFixedLengthHashTable <toFind,nKeyLng> &incoming)
{
	enableAutoResizing=incoming.enableAutoResizing;
	autoResizingMin=incoming.autoResizingMin;
	autoResizingMax=incoming.autoResizingMax;

	nElem=incoming.nElem;
	keyList.MoveFrom(incoming.keyList);
}

template <class toFind,int nKeyLng>
void YsFixedLengthHashTable <toFind,nKeyLng>::EnableAutoResizing(void)
{
	enableAutoResizing=YSTRUE;
}

template <class toFind,int nKeyLng>
void YsFixedLengthHashTable <toFind,nKeyLng>::DisableAutoResizing(void)
{
	enableAutoResizing=YSFALSE;
}

template <class toFind,int nKeyLng>
void YsFixedLengthHashTable <toFind,nKeyLng>::CleanUpThin(void)
{
	for(YSSIZE_T i=0; i<keyList.GetN(); ++i)
	{
		keyList[i].Clear();
	}
	nElem=0;
}

template <class toFind,int nKeyLng>
YSRESULT YsFixedLengthHashTable <toFind,nKeyLng>::PrepareTable(YSSIZE_T hashSize)
{
	if(0>=hashSize)
	{
		hashSize=1;
	}

	keyList.Set(hashSize,NULL);
	CleanUpThin();

	return YSOK;
}

template <class toFind,int nKeyLng>
YSRESULT YsFixedLengthHashTable <toFind,nKeyLng>::Resize(YSSIZE_T hashSize)
{
	if(0>=hashSize)
	{
		hashSize=1;
	}

	YsArray <HashElement> allElem;
	for(YSSIZE_T i=0; i<keyList.GetN(); ++i)
	{
		allElem.Append(keyList[i]);
	}

	PrepareTable(hashSize);

	for(YSSIZE_T i=0; i<allElem.GetN(); ++i)
	{
		AddElement(nKeyLng,allElem[i].key,allElem[i].objective);
	}

	return YSOK;
}

template <class toFind,int nKeyLng>
YSRESULT YsFixedLengthHashTable <toFind,nKeyLng>::CollectGarbage(void)
{
	for(YSSIZE_T i=0; i<keyList.GetN(); ++i)
	{
		if(0==keyList[i].GetN())
		{
			keyList[i].ClearDeep();
		}
	}
	return YSOK;
}

template <class toFind,int nKeyLng>
YSRESULT YsFixedLengthHashTable <toFind,nKeyLng>::Add(int /*nKey*/,const YSHASHKEY unorderedKey[],const toFind &objective)
{
	YSHASHKEY hashEntry=this->KeySum(unorderedKey)%keyList.GetN();
	YSHASHKEY orderedKey[nKeyLng];

	this->OrderKey(orderedKey,unorderedKey);

	for(int i=0; i<keyList[hashEntry].GetN(); ++i)
	{
		if(YSTRUE==this->SameKey(orderedKey,keyList[hashEntry][i].key))
		{
			return YSERR;
		}
	}

	++nElem;
	keyList[hashEntry].Increment();
	this->CopyKey(keyList[hashEntry].GetEnd().key,orderedKey);
	keyList[hashEntry].GetEnd().objective=objective;

	CheckAutoResizingGrow();

	return YSOK;
}

template <class toFind,int nKeyLng>
YSRESULT YsFixedLengthHashTable <toFind,nKeyLng>::AddElement(int nKey,const YSHASHKEY unorderedKey[],const toFind &objective)
{
	return Add(nKey,unorderedKey,objective);
}

template <class toFind,int nKeyLng>
YSRESULT YsFixedLengthHashTable <toFind,nKeyLng>::Update(int /*nKey*/,const YSHASHKEY unorderedKey[],const toFind &objective)
{
	YSHASHKEY hashEntry=this->KeySum(unorderedKey)%keyList.GetN();
	YSHASHKEY orderedKey[nKeyLng];

	this->OrderKey(orderedKey,unorderedKey);

	for(YSSIZE_T i=0; i<keyList[hashEntry].GetN(); ++i)
	{
		if(YSTRUE==this->SameKey(orderedKey,keyList[hashEntry][i].key))
		{
			keyList[hashEntry][i].objective=objective;
			return YSOK;
		}
	}

	++nElem;
	keyList[hashEntry].Increment();
	this->CopyKey(keyList[hashEntry].GetEnd().key,orderedKey);
	keyList[hashEntry].GetEnd().objective=objective;

	CheckAutoResizingGrow();

	return YSOK;
}

template <class toFind,int nKeyLng>
YSRESULT YsFixedLengthHashTable <toFind,nKeyLng>::UpdateElement(int nKey,const YSHASHKEY unorderedKey[],const toFind &objective)
{
	return Update(nKey,unorderedKey,objective);
}

template <class toFind,int nKeyLng>
YSRESULT YsFixedLengthHashTable <toFind,nKeyLng>::Delete(int /*nKey*/,const YSHASHKEY unorderedKey[])
{
	YSHASHKEY hashEntry=this->KeySum(unorderedKey)%keyList.GetN();
	YSHASHKEY orderedKey[nKeyLng];

	this->OrderKey(orderedKey,unorderedKey);

	for(int i=0; i<keyList[hashEntry].GetN(); ++i)
	{
		if(YSTRUE==this->SameKey(orderedKey,keyList[hashEntry][i].key))
		{
			keyList[hashEntry].DeleteBySwapping(i);
			--nElem;

			CheckAutoResizingShrink();
			return YSOK;
		}
	}

	return YSERR;
}

template <class toFind,int nKeyLng>
YSRESULT YsFixedLengthHashTable <toFind,nKeyLng>::DeleteElement(int nKey,const YSHASHKEY unorderedKey[])
{
	return Delete(nKey,unorderedKey);
}

template <class toFind,int nKeyLng>
inline YSRESULT YsFixedLengthHashTable <toFind,nKeyLng>::DeleteKey(int nKey,const YSHASHKEY unorderedKey[])
{
	return Delete(nKey,unorderedKey);
}

template <class toFind,int nKeyLng>
YSSIZE_T YsFixedLengthHashTable <toFind,nKeyLng>::GetHashSize(void) const
{
	return keyList.GetN();
}

template <class toFind,int nKeyLng>
YSBOOL YsFixedLengthHashTable <toFind,nKeyLng>::IsHandleValid(typename YsFixedLengthHashTable <toFind,nKeyLng>::ElemEnumHandle hd) const
{
	if(YSTRUE==keyList.IsInRange((int)hd.hashIdx) && YSTRUE==keyList[hd.hashIdx].IsInRange((int)hd.arrayIdx))
	{
		return YSTRUE;
	}
	return YSFALSE;
}

template <class toFind,int nKeyLng>
typename YsFixedLengthHashTable <toFind,nKeyLng>::ElemEnumHandle YsFixedLengthHashTable <toFind,nKeyLng>::FindHandle(int nKey,const YSHASHKEY unorderedKey[]) const
{
	ElemEnumHandle hd;
	hd.hashIdx=-1;
	hd.arrayIdx=-1;
	if(nKey==nKeyLng)
	{
		YSHASHKEY orderedKey[nKeyLng];
		this->OrderKey(orderedKey,unorderedKey);

		YSSIZE_T idx1,idx2;
		if(YSOK==FindEntryIndex(idx1,idx2,nKey,orderedKey))
		{
			hd.hashIdx=idx1;
			hd.arrayIdx=idx2;
		}
	}
	return hd;
}

template <class toFind,int nKeyLng>
inline toFind *YsFixedLengthHashTable<toFind,nKeyLng>::Find(int /*nKey*/,const YSHASHKEY unorderedKey[])
{
	YSHASHKEY hashEntry=this->KeySum(unorderedKey)%keyList.GetN();
	YSHASHKEY orderedKey[nKeyLng];

	this->OrderKey(orderedKey,unorderedKey);

	for(int i=0; i<keyList[hashEntry].GetN(); ++i)
	{
		if(YSTRUE==this->SameKey(orderedKey,keyList[hashEntry][i].key))
		{
			return &keyList[hashEntry][i].objective;
		}
	}
	return NULL;
}

template <class toFind,int nKeyLng>
inline toFind *YsFixedLengthHashTable<toFind,nKeyLng>::FindElement(int nKey,const YSHASHKEY unorderedKey[])
{
	return Find(nKey,unorderedKey);
}

template <class toFind,int nKeyLng>
const toFind *YsFixedLengthHashTable <toFind,nKeyLng>::Find(int /*nKey*/,const YSHASHKEY unorderedKey[]) const
{
	YSHASHKEY hashEntry=this->KeySum(unorderedKey)%keyList.GetN();
	YSHASHKEY orderedKey[nKeyLng];

	this->OrderKey(orderedKey,unorderedKey);

	for(int i=0; i<keyList[hashEntry].GetN(); ++i)
	{
		if(YSTRUE==this->SameKey(orderedKey,keyList[hashEntry][i].key))
		{
			return &keyList[hashEntry][i].objective;
		}
	}
	return NULL;
}

template <class toFind,int nKeyLng>
const toFind *YsFixedLengthHashTable <toFind,nKeyLng>::FindElement(int nKey,const YSHASHKEY unorderedKey[]) const
{
	return Find(nKey,unorderedKey);
}

template <class toFind,int nKeyLng>
YSRESULT YsFixedLengthHashTable <toFind,nKeyLng>::Find(toFind &elem,int nKey,const YSHASHKEY unorderedKey[]) const
{
	const toFind *found=FindElement(nKey,unorderedKey);
	if(NULL!=found)
	{
		elem=*found;
		return YSOK;
	}
	return YSERR;
}

template <class toFind,int nKeyLng>
YSRESULT YsFixedLengthHashTable <toFind,nKeyLng>::FindElement(toFind &elem,int nKey,const YSHASHKEY unorderedKey[]) const
{
	return Find(elem,nKey,unorderedKey);
}

template <class toFind,int nKeyLng>
YSBOOL YsFixedLengthHashTable <toFind,nKeyLng>::CheckKeyExist(int nKey,const YSHASHKEY unorderedKey[]) const
{
	if(NULL!=FindElement(nKey,unorderedKey))
	{
		return YSTRUE;
	}
	return YSFALSE;
}

template <class toFind,int nKeyLng>
typename YsFixedLengthHashTable <toFind,nKeyLng>::ElemEnumHandle YsFixedLengthHashTable <toFind,nKeyLng>::NullHandle(void) const
{
	ElemEnumHandle handle;
	handle.hashIdx=-1;
	handle.arrayIdx=-1;
	return handle;
}

template <class toFind,int nKeyLng>
YSRESULT YsFixedLengthHashTable <toFind,nKeyLng>::RewindElementEnumHandle(ElemEnumHandle &handle) const
{
	for(YSSIZE_T i=0; i<keyList.GetN(); i++)
	{
		if(0<keyList[i].GetN())
		{
			handle.hashIdx=i;
			handle.arrayIdx=0;
			return YSOK;
		}
	}
	handle.hashIdx=-1;
	handle.arrayIdx=-1;
	return YSERR;
}

template <class toFind,int nKeyLng>
YSRESULT YsFixedLengthHashTable <toFind,nKeyLng>::FindNextElement(ElemEnumHandle &handle) const
{
	if(YSTRUE!=IsHandleValid(handle))
	{
		return RewindElementEnumHandle(handle);
	}

	YSSIZE_T hashIdx=handle.hashIdx;
	YSSIZE_T arrayIdx=handle.arrayIdx;

	++arrayIdx;
	if(arrayIdx<keyList[hashIdx].GetN())
	{
		handle.hashIdx=hashIdx;
		handle.arrayIdx=arrayIdx;
		return YSOK;
	}

	++hashIdx;
	arrayIdx=0;

	while(hashIdx<keyList.GetN())
	{
		if(0<keyList[hashIdx].GetN())
		{
			handle.hashIdx=hashIdx;
			handle.arrayIdx=0;
			return YSOK;
		}
		++hashIdx;
	}

	handle.hashIdx=-1;
	handle.arrayIdx=-1;

	return YSERR;
}

template <class toFind,int nKeyLng>
YSRESULT YsFixedLengthHashTable <toFind,nKeyLng>::FindNextKey(ElemEnumHandle &elHd) const
{
	return FindNextElement(elHd);
}

template <class toFind,int nKeyLng>
const toFind &YsFixedLengthHashTable <toFind,nKeyLng>::GetElement(ElemEnumHandle &elHd) const
{
	return keyList[elHd.hashIdx][elHd.arrayIdx].objective;
}

template <class toFind,int nKeyLng>
toFind &YsFixedLengthHashTable <toFind,nKeyLng>::GetElement(ElemEnumHandle &elHd)
{
	return keyList[elHd.hashIdx][elHd.arrayIdx].objective;
}

template <class toFind,int nKeyLng>
const YSHASHKEY *YsFixedLengthHashTable <toFind,nKeyLng>::GetKey(ElemEnumHandle &elHd) const
{
	return keyList[elHd.hashIdx][elHd.arrayIdx].key;
}

template <class toFind,int nKeyLng>
YSRESULT YsFixedLengthHashTable <toFind,nKeyLng>::CheckAutoResizingGrow(void)
{
	const YSSIZE_T hashSize=GetHashSize();
	if(enableAutoResizing==YSTRUE)
	{
		if(autoResizingMax==0)
		{
			if(hashSize*4<nElem)
			{
				Resize(nElem);
			}
		}
		else if(hashSize<autoResizingMax && (hashSize<<2)<nElem)
		{
			Resize(YsSmaller <YSSIZE_T> (hashSize*2,autoResizingMax));
		}
	}
	return YSOK;
}

template <class toFind,int nKeyLng>
YSRESULT YsFixedLengthHashTable <toFind,nKeyLng>::CheckAutoResizingShrink(void)
{
	const YSSIZE_T hashSize=GetHashSize();
	if(enableAutoResizing==YSTRUE)
	{
		if(autoResizingMin==0)
		{
			if(nElem<hashSize/4)
			{
				Resize(YsGreater <YSSIZE_T> (nElem,1));
			}
		}
		else if(autoResizingMin<hashSize && nElem<hashSize)
		{
			Resize(YsGreater <YSSIZE_T> (hashSize/2,autoResizingMin));
		}
	}
	return YSOK;
}

template <class toFind,int nKeyLng>
YsUnidirectionalHandleEnumerator <YsFixedLengthHashTable <toFind,nKeyLng>,YsHashElementEnumHandle> YsFixedLengthHashTable <toFind,nKeyLng>::AllHandle(void) const
{
	return YsUnidirectionalHandleEnumerator <YsFixedLengthHashTable <toFind,nKeyLng>,ElemEnumHandle>(this);
}

////////////////////////////////////////////////////////////

class YsFixedLengthToMultiHashElemEnumHandle
{
public:
	YSSIZE_T idx1,idx2,objectiveIdx;

	inline bool operator==(const YsFixedLengthToMultiHashElemEnumHandle &from)
	{
		return (idx1==from.idx1 && idx2==from.idx2 && objectiveIdx==from.objectiveIdx);
	}
	inline bool operator!=(const YsFixedLengthToMultiHashElemEnumHandle &from)
	{
		return (idx1!=from.idx1 || idx2!=from.idx2 || objectiveIdx!=from.objectiveIdx);
	}
};

template <class toFind,int nKeyLng,int nDefObjLng,class SizeType=YSSIZE_T>
class YsFixedLengthToMultiHashTable : public YsFixedLengthHashBase <nKeyLng>
{
private:
	class HashElement
	{
	public:
		YSHASHKEY key[nKeyLng];
		YsArray <toFind,nDefObjLng,SizeType> objective;
	};

	YSBOOL enableAutoResizing;
	YSSIZE_T autoResizingMin,autoResizingMax;

	YSSIZE_T hashSize;
	YSSIZE_T nElem;
	YsArray <YsArray <HashElement,0,SizeType>,0,SizeType> keyList;

public:
	const char *tableName;

public:
	YsFixedLengthToMultiHashTable(YSSIZE_T hSize=16);

	YsFixedLengthToMultiHashTable(const YsFixedLengthToMultiHashTable <toFind,nKeyLng,nDefObjLng,SizeType> &incoming);
	YsFixedLengthToMultiHashTable <toFind,nKeyLng,nDefObjLng,SizeType> &operator=(const YsFixedLengthToMultiHashTable <toFind,nKeyLng,nDefObjLng,SizeType> &incoming);

	YsFixedLengthToMultiHashTable(YsFixedLengthToMultiHashTable <toFind,nKeyLng,nDefObjLng,SizeType> &&incoming);
	YsFixedLengthToMultiHashTable <toFind,nKeyLng,nDefObjLng,SizeType> &operator=(YsFixedLengthToMultiHashTable <toFind,nKeyLng,nDefObjLng,SizeType> &&incoming);

	void CopyFrom(const YsFixedLengthToMultiHashTable <toFind,nKeyLng,nDefObjLng,SizeType> &incoming);
	void MoveFrom(YsFixedLengthToMultiHashTable <toFind,nKeyLng,nDefObjLng,SizeType> &incoming);

	void CleanUp(void);
	void CleanUpThin(void); // Thin clean up.  Same as PrepareTable.
	YSRESULT PrepareTable(void);
	YSRESULT PrepareTable(YSSIZE_T hashSize);
	YSRESULT Resize(YSSIZE_T hashSize);
	YSRESULT CollectGarbage(void);

	template <class StorageClass>
	YSRESULT Add(int nKey,const YSHASHKEY unorderedKey[],const StorageClass &s)
	{
		YSRESULT res=YSOK;
		for(auto &x : s)
		{
			if(YSOK!=AddElement(nKey,unorderedKey,x))
			{
				res=YSERR;
			}
		}
		return res;
	}

	YSRESULT AddElement(int nKey,const YSHASHKEY unorderedKey[],const toFind &objective);
	YSRESULT DeleteElement(int nKey,const YSHASHKEY unorderedKey[],const toFind &objective);
	YSRESULT DeleteKey(int nKey,const YSHASHKEY unorderedKey[]);
	const YsArray <toFind,nDefObjLng,SizeType> *FindElement(int nKey,const YSHASHKEY unorderedKey[]) const;

	template <class SizeType2>
	YSRESULT FindElement(SizeType2 &nFind,const toFind *&find,int nKey,const YSHASHKEY unorderedKey[]) const;

	YsArray <toFind,nDefObjLng,SizeType> *FindElement(int nKey,const YSHASHKEY unorderedKey[]);
	YSRESULT FindElement(int &nFind,toFind *&find,int nKey,const YSHASHKEY unorderedKey[]);
	YSBOOL CheckKeyExist(int nKey,const YSHASHKEY unorderedKey[]) const;

	/*! Returns a null handle.  You can iterate through keys by the following loop.
	    for(auto handle=hash.NullHandle(); YSOK==FindNextKey(handle); )
	    {
	        // Do something useful.
	    }
	*/
	YsFixedLengthToMultiHashElemEnumHandle NullHandle(void) const;
	YSBOOL IsHandleValid(const YsFixedLengthToMultiHashElemEnumHandle &elHd) const;
	YSRESULT RewindElementEnumHandle(YsFixedLengthToMultiHashElemEnumHandle &elHd) const;
	YSRESULT FindNextKey(YsFixedLengthToMultiHashElemEnumHandle &elHd) const;
	YSRESULT FindNextElement(YsFixedLengthToMultiHashElemEnumHandle &elHd) const;
	toFind &GetElement(YsFixedLengthToMultiHashElemEnumHandle &elHd);
	const toFind &GetElement(YsFixedLengthToMultiHashElemEnumHandle &elHd) const;
	const YSHASHKEY *GetKey(YsFixedLengthToMultiHashElemEnumHandle &elHd) const;

protected:
	YSRESULT FindEntryIndex(YSSIZE_T &idx1,YSSIZE_T &idx2,int nKey,const YSHASHKEY orderedKey[]) const;
	const HashElement *FindEntry(int nKey,const YSHASHKEY orderedKey[]) const;
	HashElement *FindEntry(int nKey,const YSHASHKEY orderedKey[]);
	HashElement *CreateEntry(int nKey,const YSHASHKEY orderedKey[]);

public:
	/*! \deprecated This function has been deprecated.  To be consistent with YsHashTable
	    and YsFixedLengthHashTable, use RewindElementEnumHandle, which has the same functionality. */
	YSRESULT RewindElemEnumHandle(YsFixedLengthToMultiHashElemEnumHandle &elHd) const;

	/*! \deprecated This function has been deprecated.  To be consistent with YsHashTable
	    and YsFixedLengthHashTable, use FindNextElement, which has the same functionality. */
	YSRESULT FindNextElem(YsFixedLengthToMultiHashElemEnumHandle &elHd) const;

	/*! \deprecated This function has been deprecated.  To be consistent with YsHashTable
	    and YsFixedLengthHashTable, use GetElement, which has the same functionality. */
	const toFind &GetElem(YsFixedLengthToMultiHashElemEnumHandle &elHd) const;

	void EnableAutoResizing(YSSIZE_T minSize,YSSIZE_T maxSize);
	void EnableAutoResizing(void);

	int GetNumUnused(void) const;

private:
	YSRESULT CheckAutoResizingGrow(void);
	YSRESULT CheckAutoResizingShrink(void);

public:
	// Following four functions are needed for YsUnidirectionalHandleIterator
    inline void MoveToNext(YsFixedLengthToMultiHashElemEnumHandle &hd) const
    {
		if(YSOK!=FindNextElem(hd))
		{
			hd=NullHandle();
		}
	}
    inline YsFixedLengthToMultiHashElemEnumHandle First(void) const
    {
		YsFixedLengthToMultiHashElemEnumHandle hd;
		if(YSOK!=RewindElemEnumHandle(hd))
		{
			hd=NullHandle();
		}
	}
    inline YsFixedLengthToMultiHashElemEnumHandle Null(void) const
    {
		return NullHandle();
	}
    inline toFind &Value(YsFixedLengthToMultiHashElemEnumHandle hd)
    {
		return GetElement(hd);
	}
    inline const toFind &Value(YsFixedLengthToMultiHashElemEnumHandle hd) const
    {
		return GetElement(hd);
	}
public:
	typedef YsUnidirectionalHandleIterator<YsFixedLengthToMultiHashTable<toFind,nKeyLng,nDefObjLng,SizeType>,YsFixedLengthToMultiHashElemEnumHandle,toFind> iterator;
	typedef YsUnidirectionalHandleIterator<const YsFixedLengthToMultiHashTable<toFind,nKeyLng,nDefObjLng,SizeType>,YsFixedLengthToMultiHashElemEnumHandle,const toFind> const_iterator;
	iterator begin()
	{
		iterator iter(this);
		return iter.begin();
	}
	iterator end()
	{
		iterator iter(this);
		return iter.end();
	}
	const_iterator begin() const
	{
		const_iterator iter(this);
		return iter.begin();
	}
	const_iterator end() const
	{
		const_iterator iter(this);
		return iter.end();
	}
	YsUnidirectionalHandleEnumerator<YsFixedLengthToMultiHashTable<toFind,nKeyLng,nDefObjLng,SizeType>,YsFixedLengthToMultiHashElemEnumHandle> AllHandle(void) const;
};

template <class toFind,int nKeyLng,int nDefObjLng,class SizeType>
typename YsFixedLengthToMultiHashTable<toFind,nKeyLng,nDefObjLng,SizeType>::iterator begin(YsFixedLengthToMultiHashTable<toFind,nKeyLng,nDefObjLng,SizeType> &hashTab)
{
	return hashTab.begin();
}

template <class toFind,int nKeyLng,int nDefObjLng,class SizeType>
typename YsFixedLengthToMultiHashTable<toFind,nKeyLng,nDefObjLng,SizeType>::iterator end(YsFixedLengthToMultiHashTable<toFind,nKeyLng,nDefObjLng,SizeType> &hashTab)
{
	return hashTab.end();
}

template <class toFind,int nKeyLng,int nDefObjLng,class SizeType>
typename YsFixedLengthToMultiHashTable<toFind,nKeyLng,nDefObjLng,SizeType>::const_iterator begin(const YsFixedLengthToMultiHashTable<toFind,nKeyLng,nDefObjLng,SizeType> &hashTab)
{
	return hashTab.begin();
}

template <class toFind,int nKeyLng,int nDefObjLng,class SizeType>
typename YsFixedLengthToMultiHashTable<toFind,nKeyLng,nDefObjLng,SizeType>::const_iterator end(const YsFixedLengthToMultiHashTable<toFind,nKeyLng,nDefObjLng,SizeType> &hashTab)
{
	return hashTab.end();
}



template <class toFind,int nKeyLng,int nDefObjLng,class SizeType>
YsFixedLengthToMultiHashTable <toFind,nKeyLng,nDefObjLng,SizeType>::YsFixedLengthToMultiHashTable(YSSIZE_T hSize)
{
	keyList.Set((SizeType)hSize,NULL);
	hashSize=hSize;
	nElem=0;
	enableAutoResizing=YSFALSE;
	autoResizingMin=0;
	autoResizingMax=0;

	tableName="";

	EnableAutoResizing();
}

template <class toFind,int nKeyLng,int nDefObjLng,class SizeType>
YsFixedLengthToMultiHashTable<toFind,nKeyLng,nDefObjLng,SizeType>::YsFixedLengthToMultiHashTable(const YsFixedLengthToMultiHashTable <toFind,nKeyLng,nDefObjLng,SizeType> &incoming)
{
	CopyFrom(incoming);
}

template <class toFind,int nKeyLng,int nDefObjLng,class SizeType>
YsFixedLengthToMultiHashTable <toFind,nKeyLng,nDefObjLng,SizeType> &YsFixedLengthToMultiHashTable<toFind,nKeyLng,nDefObjLng,SizeType>::operator=(
    const YsFixedLengthToMultiHashTable <toFind,nKeyLng,nDefObjLng,SizeType> &incoming)
{
	CopyFrom(incoming);
	return *this;
}

template <class toFind,int nKeyLng,int nDefObjLng,class SizeType>
YsFixedLengthToMultiHashTable<toFind,nKeyLng,nDefObjLng,SizeType>::YsFixedLengthToMultiHashTable(YsFixedLengthToMultiHashTable <toFind,nKeyLng,nDefObjLng,SizeType> &&incoming)
{
	MoveFrom(incoming);
}

template <class toFind,int nKeyLng,int nDefObjLng,class SizeType>
YsFixedLengthToMultiHashTable <toFind,nKeyLng,nDefObjLng,SizeType> &YsFixedLengthToMultiHashTable<toFind,nKeyLng,nDefObjLng,SizeType>::operator=(
    YsFixedLengthToMultiHashTable <toFind,nKeyLng,nDefObjLng,SizeType> &&incoming)
{
	MoveFrom(incoming);
	return *this;
}

template <class toFind,int nKeyLng,int nDefObjLng,class SizeType>
void YsFixedLengthToMultiHashTable<toFind,nKeyLng,nDefObjLng,SizeType>::CopyFrom(const YsFixedLengthToMultiHashTable <toFind,nKeyLng,nDefObjLng,SizeType> &incoming)
{
	keyList=incoming.keyList;
	hashSize=incoming.hashSize;
	nElem=incoming.nElem;
	enableAutoResizing=incoming.enableAutoResizing;
	autoResizingMin=incoming.autoResizingMin;
	autoResizingMax=incoming.autoResizingMax;

	tableName=incoming.tableName;
}

template <class toFind,int nKeyLng,int nDefObjLng,class SizeType>
void YsFixedLengthToMultiHashTable<toFind,nKeyLng,nDefObjLng,SizeType>::MoveFrom(YsFixedLengthToMultiHashTable <toFind,nKeyLng,nDefObjLng,SizeType> &incoming)
{
	keyList.MoveFrom(incoming.keyList);
	hashSize=incoming.hashSize;
	nElem=incoming.nElem;
	enableAutoResizing=incoming.enableAutoResizing;
	autoResizingMin=incoming.autoResizingMin;
	autoResizingMax=incoming.autoResizingMax;

	tableName=incoming.tableName;
}

template <class toFind,int nKeyLng,int nDefObjLng,class SizeType>
void YsFixedLengthToMultiHashTable <toFind,nKeyLng,nDefObjLng,SizeType>::CleanUp(void)
{
	PrepareTable();
}

template <class toFind,int nKeyLng,int nDefObjLng,class SizeType>
void YsFixedLengthToMultiHashTable <toFind,nKeyLng,nDefObjLng,SizeType>::CleanUpThin(void) // Thin clean up.  Same as PrepareTable.
{
	for(YSSIZE_T i=0; i<hashSize; i++)
	{
		keyList[i].Set(0,NULL);
	}
	nElem=0;
}

template <class toFind,int nKeyLng,int nDefObjLng,class SizeType>
YSRESULT YsFixedLengthToMultiHashTable <toFind,nKeyLng,nDefObjLng,SizeType>::PrepareTable(void)
{
	for(YSSIZE_T i=0; i<hashSize; i++)
	{
		keyList[i].Set(0,NULL);
	}
	nElem=0;
	return YSOK;
}

template <class toFind,int nKeyLng,int nDefObjLng,class SizeType>
YSRESULT YsFixedLengthToMultiHashTable <toFind,nKeyLng,nDefObjLng,SizeType>::PrepareTable(YSSIZE_T hSize)
{
	YSSIZE_T i;
	keyList.Set(0,NULL);  // <- This will invoke Shrink. 2005/05/20
	keyList.Set((SizeType)hSize,NULL);
	hashSize=hSize;
	for(i=0; i<hashSize; i++)
	{
		keyList[i].Set(0,NULL);
	}
	nElem=0;
	return YSOK;
}

template <class toFind,int nKeyLng,int nDefObjLng,class SizeType>
YSRESULT YsFixedLengthToMultiHashTable <toFind,nKeyLng,nDefObjLng,SizeType>::Resize(YSSIZE_T newHashSize)
{
	YSSIZE_T i,j,idx1,keySum;
	YsArray <HashElement> elemList;
	for(i=0; i<hashSize; i++)
	{
		elemList.Append(keyList[i].GetN(),keyList[i]);
	}
	PrepareTable(newHashSize);  // <- This will reset nElem
	for(i=0; i<elemList.GetN(); i++)
	{
		keySum=0;
		for(j=0; j<nKeyLng; j++)
		{
			keySum+=elemList[i].key[j];;
		}
		idx1=keySum%hashSize;
		keyList[idx1].Append(elemList[i]);
		nElem+=elemList[i].objective.GetN();
	}
	return YSOK;
}

template <class toFind,int nKeyLng,int nDefObjLng,class SizeType>
YSRESULT YsFixedLengthToMultiHashTable <toFind,nKeyLng,nDefObjLng,SizeType>::CollectGarbage(void)
{
	Resize(hashSize);
	return YSOK;
}

template <class toFind,int nKeyLng,int nDefObjLng,class SizeType>
YSRESULT YsFixedLengthToMultiHashTable <toFind,nKeyLng,nDefObjLng,SizeType>::AddElement(
    int nKey,const YSHASHKEY unorderedKey[],const toFind &objective)
{
	YSHASHKEY orderedKey[nKeyLng];
	HashElement *entry;

	if(nKey==nKeyLng)
	{
		this->OrderKey(orderedKey,unorderedKey);
		entry=FindEntry(nKey,orderedKey);
		if(entry!=NULL)
		{
			entry->objective.Append(objective);
			nElem++;
			CheckAutoResizingGrow();
			return YSOK;
		}
		else
		{
			entry=CreateEntry(nKey,orderedKey);
			if(entry!=NULL)
			{
				entry->objective.Append(objective);
				nElem++;
				CheckAutoResizingGrow();
				return YSOK;
			}
			else
			{
				YsErrOut("YsFixedLengthToMultiHashTable::AddElement()\n");
				YsErrOut("  Out of memory warning.\n");
				return YSERR;
			}
		}
	}
	else
	{
		YsErrOut("YsFixedLengthToMultiHashTable::AddElement()\n");
		YsErrOut("  Key length does not match.\n");
		return YSERR;
	}
}

template <class toFind,int nKeyLng,int nDefObjLng,class SizeType>
YSRESULT YsFixedLengthToMultiHashTable <toFind,nKeyLng,nDefObjLng,SizeType>::DeleteElement(
    int nKey,const YSHASHKEY unorderedKey[],const toFind &objective)
{
	YSHASHKEY orderedKey[nKeyLng];

	if(nKey==nKeyLng)
	{
		YSSIZE_T idx1,idx2;
		this->OrderKey(orderedKey,unorderedKey);
		if(FindEntryIndex(idx1,idx2,nKey,orderedKey)==YSOK)
		{
			int i;
			for(i=0; i<keyList[idx1][idx2].objective.GetN(); i++)
			{
				if(keyList[idx1][idx2].objective[i]==objective)
				{
					keyList[idx1][idx2].objective.DeleteBySwapping(i);
					if(keyList[idx1][idx2].objective.GetN()==0)
					{
						keyList[idx1].DeleteBySwapping(idx2);
					}
					nElem--;
					CheckAutoResizingShrink();
					return YSOK;
				}
			}
		}
		YsErrOut("YsFixedLengthToMultiHashTable::DeleteElement()\n");
		YsErrOut("  Objective not found.\n");
		return YSERR;
	}
	else
	{
		YsErrOut("YsFixedLengthToMultiHashTable::DeleteElement()\n");
		YsErrOut("  Key length does not match.\n");
		return YSERR;
	}
}

template <class toFind,int nKeyLng,int nDefObjLng,class SizeType>
YSRESULT YsFixedLengthToMultiHashTable <toFind,nKeyLng,nDefObjLng,SizeType>::DeleteKey(int nKey,const YSHASHKEY unorderedKey[])
{
	YSHASHKEY orderedKey[nKeyLng];

	if(nKey==nKeyLng)
	{
		YSSIZE_T idx1,idx2;
		this->OrderKey(orderedKey,unorderedKey);
		if(FindEntryIndex(idx1,idx2,nKey,orderedKey)==YSOK)
		{
			nElem-=keyList[idx1][idx2].objective.GetN();
			keyList[idx1].DeleteBySwapping(idx2);
			CheckAutoResizingShrink();
			return YSOK;
		}
		return YSERR;
	}
	else
	{
		YsErrOut("YsFixedLengthToMultiHashTable::DeleteKey()\n");
		YsErrOut("  Key length does not match.\n");
		return YSERR;
	}
}

template <class toFind,int nKeyLng,int nDefObjLng,class SizeType>
const YsArray <toFind,nDefObjLng,SizeType> *YsFixedLengthToMultiHashTable <toFind,nKeyLng,nDefObjLng,SizeType>::FindElement(int nKey,const YSHASHKEY unorderedKey[]) const
{
	const HashElement *entry;
	YSHASHKEY orderedKey[nKeyLng];

	if(nKey==nKeyLng)
	{
		this->OrderKey(orderedKey,unorderedKey);
		entry=FindEntry(nKey,orderedKey);
		if(entry!=NULL)
		{
			return &entry->objective;
		}
		return NULL;
	}
	else
	{
		YsErrOut("YsFixedLengthToMultiHashTable::FindElement()\n");
		YsErrOut("  Key length does not match.\n");
		return NULL;
	}
}

template <class toFind,int nKeyLng,int nDefObjLng,class SizeType>
template <class SizeType2>
YSRESULT YsFixedLengthToMultiHashTable <toFind,nKeyLng,nDefObjLng,SizeType>::FindElement(SizeType2 &nFind,const toFind *&find,int nKey,const YSHASHKEY unorderedKey[]) const
{
	const YsArray <toFind,nDefObjLng> *foundArray;
	foundArray=FindElement(nKey,unorderedKey);
	if(foundArray!=NULL && foundArray->GetN()>0)
	{
		nFind=(SizeType2)foundArray->GetN();
		find=foundArray->GetArray();
		return YSOK;
	}
	else
	{
		nFind=0;
		find=NULL;
		return YSERR;
	}
}

template <class toFind,int nKeyLng,int nDefObjLng,class SizeType>
YsArray <toFind,nDefObjLng,SizeType> *YsFixedLengthToMultiHashTable <toFind,nKeyLng,nDefObjLng,SizeType>::FindElement(int nKey,const YSHASHKEY unorderedKey[])
{
	HashElement *entry;
	YSHASHKEY orderedKey[nKeyLng];

	if(nKey==nKeyLng)
	{
		this->OrderKey(orderedKey,unorderedKey);
		entry=FindEntry(nKey,orderedKey);
		if(entry!=NULL)
		{
			return &entry->objective;
		}
		return NULL;
	}
	else
	{
		YsErrOut("YsFixedLengthToMultiHashTable::FindElement()\n");
		YsErrOut("  Key length does not match.\n");
		return NULL;
	}
}

template <class toFind,int nKeyLng,int nDefObjLng,class SizeType>
YSRESULT YsFixedLengthToMultiHashTable <toFind,nKeyLng,nDefObjLng,SizeType>::FindElement
   (int &nFind,toFind *&find,int nKey,const YSHASHKEY unorderedKey[])
{
	YsArray <toFind,nDefObjLng> *foundArray;
	foundArray=FindElement(nKey,unorderedKey);
	if(foundArray!=NULL && foundArray->GetN()>0)
	{
		nFind=(int)foundArray->GetN();
		find=foundArray->GetEditableArray();
		return YSOK;
	}
	else
	{
		nFind=0;
		find=NULL;
		return YSERR;
	}
}

template <class toFind,int nKeyLng,int nDefObjLng,class SizeType>
YSBOOL YsFixedLengthToMultiHashTable <toFind,nKeyLng,nDefObjLng,SizeType>::CheckKeyExist
   (int nKey,const YSHASHKEY unorderedKey[]) const
{
	const YsArray <toFind,nDefObjLng> *entry;
	entry=FindElement(nKey,unorderedKey);
	if(entry!=NULL && entry->GetN()>0)
	{
		return YSTRUE;
	}
	else
	{
		return YSFALSE;
	}
}

template <class toFind,int nKeyLng,int nDefObjLng,class SizeType>
YsFixedLengthToMultiHashElemEnumHandle YsFixedLengthToMultiHashTable <toFind,nKeyLng,nDefObjLng,SizeType>::NullHandle(void) const
{
	YsFixedLengthToMultiHashElemEnumHandle elHd;
	elHd.idx1=-1;
	elHd.idx2=-1;
	elHd.objectiveIdx=-1;
	return elHd;
}

template <class toFind,int nKeyLng,int nDefObjLng,class SizeType>
YSBOOL YsFixedLengthToMultiHashTable <toFind,nKeyLng,nDefObjLng,SizeType>::IsHandleValid(const YsFixedLengthToMultiHashElemEnumHandle &elHd) const
{
	if(0<=elHd.idx1 && elHd.idx1<hashSize &&
	   YSTRUE==keyList[elHd.idx1].IsInRange(elHd.idx2) &&
	   YSTRUE==keyList[elHd.idx1][elHd.idx2].objective.IsInRange(elHd.objectiveIdx))
	{
		return YSTRUE;
	}
	return YSFALSE;
}

template <class toFind,int nKeyLng,int nDefObjLng,class SizeType>
YSRESULT YsFixedLengthToMultiHashTable <toFind,nKeyLng,nDefObjLng,SizeType>::RewindElementEnumHandle
   (YsFixedLengthToMultiHashElemEnumHandle &elHd) const
{
	for(YSSIZE_T i=0; i<hashSize; i++)
	{
		for(YSSIZE_T j=0; j<keyList[i].GetN(); j++)
		{
			if(keyList[i][j].objective.GetN()>0)
			{
				elHd.idx1=i;
				elHd.idx2=j;
				elHd.objectiveIdx=0;
				return YSOK;
			}
		}
	}
	return YSERR;
}

template <class toFind,int nKeyLng,int nDefObjLng,class SizeType>
YSRESULT YsFixedLengthToMultiHashTable <toFind,nKeyLng,nDefObjLng,SizeType>::FindNextElement
   (YsFixedLengthToMultiHashElemEnumHandle &elHd) const
{
	if(YSTRUE!=IsHandleValid(elHd))
	{
		return RewindElementEnumHandle(elHd);
	}

	elHd.objectiveIdx++;
	if(elHd.objectiveIdx<keyList[elHd.idx1][elHd.idx2].objective.GetN())
	{
		return YSOK;
	}
	elHd.idx2++;
	elHd.objectiveIdx=0;
	if(elHd.idx2<keyList[elHd.idx1].GetN())
	{
		return YSOK;
	}

	int i;
	for(i=elHd.idx1+1; i<hashSize; i++)
	{
		int j;
		for(j=0; j<keyList[i].GetN(); j++)
		{
			if(keyList[i][j].objective.GetN()>0)
			{
				elHd.idx1=i;
				elHd.idx2=j;
				elHd.objectiveIdx=0;
				return YSOK;
			}
		}
	}
	return YSERR;
}

template <class toFind,int nKeyLng,int nDefObjLng,class SizeType>
YSRESULT YsFixedLengthToMultiHashTable <toFind,nKeyLng,nDefObjLng,SizeType>::FindNextKey
   (YsFixedLengthToMultiHashElemEnumHandle &elHd) const
{
	if(YSTRUE!=IsHandleValid(elHd))
	{
		return RewindElementEnumHandle(elHd);
	}

	elHd.idx2++;
	elHd.objectiveIdx=0;
	if(elHd.idx2<keyList[elHd.idx1].GetN())
	{
		return YSOK;
	}

	for(YSSIZE_T i=elHd.idx1+1; i<hashSize; i++)
	{
		for(YSSIZE_T j=0; j<keyList[i].GetN(); j++)
		{
			if(keyList[i][j].objective.GetN()>0)
			{
				elHd.idx1=i;
				elHd.idx2=j;
				elHd.objectiveIdx=0;
				return YSOK;
			}
		}
	}
	return YSERR;
}

template <class toFind,int nKeyLng,int nDefObjLng,class SizeType>
const toFind &YsFixedLengthToMultiHashTable <toFind,nKeyLng,nDefObjLng,SizeType>::GetElement(YsFixedLengthToMultiHashElemEnumHandle &elHd) const
{
	return keyList[elHd.idx1][elHd.idx2].objective[elHd.objectiveIdx];
}

template <class toFind,int nKeyLng,int nDefObjLng,class SizeType>
toFind &YsFixedLengthToMultiHashTable <toFind,nKeyLng,nDefObjLng,SizeType>::GetElement(YsFixedLengthToMultiHashElemEnumHandle &elHd)
{
	return keyList[elHd.idx1][elHd.idx2].objective[elHd.objectiveIdx];
}

template <class toFind,int nKeyLng,int nDefObjLng,class SizeType>
const YSHASHKEY *YsFixedLengthToMultiHashTable <toFind,nKeyLng,nDefObjLng,SizeType>::GetKey
   (YsFixedLengthToMultiHashElemEnumHandle &elHd) const
{
	return keyList[elHd.idx1][elHd.idx2].key;
}

template <class toFind,int nKeyLng,int nDefObjLng,class SizeType>
YSRESULT YsFixedLengthToMultiHashTable <toFind,nKeyLng,nDefObjLng,SizeType>::FindEntryIndex
   (YSSIZE_T &idx1,YSSIZE_T &idx2,int /*nKey*/,const YSHASHKEY orderedKey[]) const
{
	const YSHASHKEY keySum=this->KeySum(orderedKey);

	idx1=keySum%hashSize;
	for(idx2=0; idx2<keyList[idx1].GetN(); idx2++)
	{
		if(YSTRUE==this->SameKey(keyList[idx1][idx2].key,orderedKey))
		{
			return YSOK;
		}
	}

	idx1=-1;
	idx2=-1;
	return YSERR;
}

template <class toFind,int nKeyLng,int nDefObjLng,class SizeType>
const typename YsFixedLengthToMultiHashTable <toFind,nKeyLng,nDefObjLng,SizeType>::HashElement *
    YsFixedLengthToMultiHashTable <toFind,nKeyLng,nDefObjLng,SizeType>::FindEntry(int nKey,const YSHASHKEY orderedKey[]) const
{
	YSSIZE_T idx1,idx2;
	if(FindEntryIndex(idx1,idx2,nKey,orderedKey)==YSOK)
	{
		return &keyList[idx1][idx2];
	}
	return NULL;
}

template <class toFind,int nKeyLng,int nDefObjLng,class SizeType>
typename YsFixedLengthToMultiHashTable <toFind,nKeyLng,nDefObjLng,SizeType>::HashElement *
    YsFixedLengthToMultiHashTable <toFind,nKeyLng,nDefObjLng,SizeType>::FindEntry(int nKey,const YSHASHKEY orderedKey[])
{
	YSSIZE_T idx1,idx2;
	if(FindEntryIndex(idx1,idx2,nKey,orderedKey)==YSOK)
	{
		return &keyList[idx1][idx2];
	}
	return NULL;
}

template <class toFind,int nKeyLng,int nDefObjLng,class SizeType>
typename YsFixedLengthToMultiHashTable <toFind,nKeyLng,nDefObjLng,SizeType>::HashElement *
    YsFixedLengthToMultiHashTable <toFind,nKeyLng,nDefObjLng,SizeType>::CreateEntry(int nKey,const YSHASHKEY orderedKey[])
{
	YSHASHKEY keySum;
	int i;
	keySum=0;
	for(i=0; i<nKey; i++)
	{
		keySum+=orderedKey[i];
	}

	YSSIZE_T idx1,idx2;
	idx1=keySum%hashSize;
	idx2=keyList[idx1].GetN();

	HashElement newElem;
	for(i=0; i<nKey; i++)
	{
		newElem.key[i]=orderedKey[i];
	}
	keyList[idx1].Append(newElem);
	return &keyList[idx1][idx2];
}

template <class toFind,int nKeyLng,int nDefObjLng,class SizeType>
void YsFixedLengthToMultiHashTable <toFind,nKeyLng,nDefObjLng,SizeType>::EnableAutoResizing(YSSIZE_T minSize,YSSIZE_T maxSize)
{
	enableAutoResizing=YSTRUE;
	autoResizingMin=minSize;
	autoResizingMax=maxSize;
}

template <class toFind,int nKeyLng,int nDefObjLng,class SizeType>
void YsFixedLengthToMultiHashTable <toFind,nKeyLng,nDefObjLng,SizeType>::EnableAutoResizing(void)
{
	enableAutoResizing=YSTRUE;
	autoResizingMin=0;
	autoResizingMax=0;
}

template <class toFind,int nKeyLng,int nDefObjLng,class SizeType>
int YsFixedLengthToMultiHashTable <toFind,nKeyLng,nDefObjLng,SizeType>::GetNumUnused(void) const
{
	YSSIZE_T i,n;
	YsArray <HashElement> elemList;
	n=0;
	for(i=0; i<hashSize; i++)
	{
		n+=keyList[i].GetNUnused();
//		int j;
//		for(j=0; j<keyList[j].GetN(); j++)
//		{
//			n+=keyList[i][j].objective.GetNUnused();
//		}
	}
	return (int)n;
}

template <class toFind,int nKeyLng,int nDefObjLng,class SizeType>
YSRESULT YsFixedLengthToMultiHashTable <toFind,nKeyLng,nDefObjLng,SizeType>::CheckAutoResizingGrow(void)
{
	if(enableAutoResizing==YSTRUE)
	{
		if(autoResizingMax==0)
		{
			if(hashSize*4<nElem)
			{
				Resize(nElem);
			}
		}
		else if(hashSize<autoResizingMax && (hashSize<<2)<nElem)
		{
			Resize(YsSmaller <YSSIZE_T> (hashSize*2,autoResizingMax));
		}
	}
	return YSOK;
}

template <class toFind,int nKeyLng,int nDefObjLng,class SizeType>
YSRESULT YsFixedLengthToMultiHashTable <toFind,nKeyLng,nDefObjLng,SizeType>::CheckAutoResizingShrink(void)
{
	if(enableAutoResizing==YSTRUE)
	{
		if(autoResizingMin==0)
		{
			if(nElem<hashSize/4)
			{
				Resize(YsGreater <YSSIZE_T> (nElem,1));
			}
		}
		else if(autoResizingMin<hashSize && nElem<hashSize)
		{
			Resize(YsGreater <YSSIZE_T> (hashSize/2,autoResizingMin));
		}
	}
	return YSOK;
}

template <class toFind,int nKeyLng,int nDefObjLng,class SizeType>
YSRESULT YsFixedLengthToMultiHashTable <toFind,nKeyLng,nDefObjLng,SizeType>::RewindElemEnumHandle(YsFixedLengthToMultiHashElemEnumHandle &elHd) const
{
	return RewindElementEnumHandle(elHd);
}

template <class toFind,int nKeyLng,int nDefObjLng,class SizeType>
YSRESULT YsFixedLengthToMultiHashTable <toFind,nKeyLng,nDefObjLng,SizeType>::FindNextElem(YsFixedLengthToMultiHashElemEnumHandle &elHd) const
{
	return FindNextElement(elHd);
}

template <class toFind,int nKeyLng,int nDefObjLng,class SizeType>
const toFind &YsFixedLengthToMultiHashTable <toFind,nKeyLng,nDefObjLng,SizeType>::GetElem(YsFixedLengthToMultiHashElemEnumHandle &elHd) const
{
	return GetElement(elHd);
}

////////////////////////////////////////////////////////////

/*! StorageClass must have a member function:
      YSHASHKEY StorageClass::GetSearchKey(HandleClass hd) const;
    which returns the hash key for the handle.
*/
template <class toFind,class StorageClass,class HandleClass>
class YsHandleToAttribTable
{
private:
	const StorageClass *store;
	YsHashTable <toFind> keyToTargetHash;
public:
	YsHandleToAttribTable();
	YsHandleToAttribTable(const StorageClass &store);
	YsHandleToAttribTable(const YsHandleToAttribTable <toFind,StorageClass,HandleClass> &incoming);
	YsHandleToAttribTable(YsHandleToAttribTable <toFind,StorageClass,HandleClass> &&incoming);
	YsHandleToAttribTable <toFind,StorageClass,HandleClass> &operator=(const YsHandleToAttribTable <toFind,StorageClass,HandleClass> &incoming);
	YsHandleToAttribTable <toFind,StorageClass,HandleClass> &operator=(YsHandleToAttribTable <toFind,StorageClass,HandleClass> &&incoming);
	void CopyFrom(const YsHandleToAttribTable <toFind,StorageClass,HandleClass> &incoming);
	void MoveFrom(YsHandleToAttribTable <toFind,StorageClass,HandleClass> &incoming);

	/*! Cleans up all values.  It won't nullify the storage pointer.  Safe to call before or after SetStorage. */
	void CleanUp(void);
	void SetStorage(const StorageClass &store);
	void SetAttrib(HandleClass hd,const toFind &attrib);
	void SetAttrib(HandleClass hd,toFind &&attrib);
	void SetAttrib(YSHASHKEY key,const toFind &attrib);
	void SetAttrib(YSHASHKEY key,toFind &&attrib);

	/*! Returns the number of handles-attribute pairs. */
	YSSIZE_T GetN(void) const;

	YSRESULT DeleteAttrib(HandleClass hd);
	YSRESULT DeleteAttrib(YSHASHKEY key);

	const toFind *operator[](HandleClass hd) const;
	toFind *operator[](HandleClass hd);
	const toFind *operator[](YSHASHKEY key) const;
	toFind *operator[](YSHASHKEY key);

	YsUnidirectionalHandleEnumerator <YsHashTable <toFind>,YsHashElementEnumHandle> AllHandle(void) const;
	const YSHASHKEY GetKey(YsHashElementEnumHandle eeHd) const;
	toFind &GetAttrib(YsHashElementEnumHandle eeHd);
	const toFind &GetAttrib(YsHashElementEnumHandle eeHd) const;
};

template <class toFind,class StorageClass,class HandleClass>
YsHandleToAttribTable<toFind,StorageClass,HandleClass>::YsHandleToAttribTable()
{
	store=NULL;
}
template <class toFind,class StorageClass,class HandleClass>
YsHandleToAttribTable<toFind,StorageClass,HandleClass>::YsHandleToAttribTable(const StorageClass &store)
{
	this->store=&store;
}
template <class toFind,class StorageClass,class HandleClass>
YsHandleToAttribTable<toFind,StorageClass,HandleClass>::YsHandleToAttribTable(const YsHandleToAttribTable <toFind,StorageClass,HandleClass> &incoming)
{
	CopyFrom(incoming);
}
template <class toFind,class StorageClass,class HandleClass>
YsHandleToAttribTable<toFind,StorageClass,HandleClass>::YsHandleToAttribTable(YsHandleToAttribTable <toFind,StorageClass,HandleClass> &&incoming)
{
	MoveFrom(incoming);
}
template <class toFind,class StorageClass,class HandleClass>
YsHandleToAttribTable <toFind,StorageClass,HandleClass> &YsHandleToAttribTable<toFind,StorageClass,HandleClass>::operator=(const YsHandleToAttribTable <toFind,StorageClass,HandleClass> &incoming)
{
	CopyFrom(incoming);
	return *this;
}
template <class toFind,class StorageClass,class HandleClass>
YsHandleToAttribTable <toFind,StorageClass,HandleClass> &YsHandleToAttribTable<toFind,StorageClass,HandleClass>::operator=(YsHandleToAttribTable <toFind,StorageClass,HandleClass> &&incoming)
{
	MoveFrom(incoming);
	return *this;
}
template <class toFind,class StorageClass,class HandleClass>
void YsHandleToAttribTable<toFind,StorageClass,HandleClass>::CopyFrom(const YsHandleToAttribTable <toFind,StorageClass,HandleClass> &incoming)
{
	this->store=incoming.store;
	this->keyToTargetHash.CopyFrom(incoming.keyToTargetHash);
}
template <class toFind,class StorageClass,class HandleClass>
void YsHandleToAttribTable<toFind,StorageClass,HandleClass>::MoveFrom(YsHandleToAttribTable <toFind,StorageClass,HandleClass> &incoming)
{
	this->store=incoming.store;
	this->keyToTargetHash.MoveFrom(incoming.keyToTargetHash);
}
template <class toFind,class StorageClass,class HandleClass>
void YsHandleToAttribTable<toFind,StorageClass,HandleClass>::CleanUp(void)
{
	keyToTargetHash.CleanUp();
}
template <class toFind,class StorageClass,class HandleClass>
void YsHandleToAttribTable<toFind,StorageClass,HandleClass>::SetStorage(const StorageClass &store)
{
	this->store=&store;
}
template <class toFind,class StorageClass,class HandleClass>
void YsHandleToAttribTable<toFind,StorageClass,HandleClass>::SetAttrib(HandleClass hd,const toFind &attrib)
{
	if(NULL==store)
	{
		printf("%s %d\n",__FUNCTION__,__LINE__);
		printf("  StorageClass pointer hasn't been set.\n");
	}
	keyToTargetHash.UpdateElement(store->GetSearchKey(hd),attrib);
}

template <class toFind,class StorageClass,class HandleClass>
YSSIZE_T YsHandleToAttribTable<toFind,StorageClass,HandleClass>::GetN(void) const
{
	return keyToTargetHash.GetN();
}

template <class toFind,class StorageClass,class HandleClass>
void YsHandleToAttribTable<toFind,StorageClass,HandleClass>::SetAttrib(HandleClass hd,toFind &&attrib)
{
	if(NULL==store)
	{
		printf("%s %d\n",__FUNCTION__,__LINE__);
		printf("  StorageClass pointer hasn't been set.\n");
	}
	keyToTargetHash.UpdateElement(store->GetSearchKey(hd),attrib);
}
template <class toFind,class StorageClass,class HandleClass>
void YsHandleToAttribTable<toFind,StorageClass,HandleClass>::SetAttrib(YSHASHKEY key,const toFind &attrib)
{
	if(NULL==store)
	{
		printf("%s %d\n",__FUNCTION__,__LINE__);
		printf("  StorageClass pointer hasn't been set.\n");
	}
	keyToTargetHash.UpdateElement(key,attrib);
}
template <class toFind,class StorageClass,class HandleClass>
void YsHandleToAttribTable<toFind,StorageClass,HandleClass>::SetAttrib(YSHASHKEY key,toFind &&attrib)
{
	if(NULL==store)
	{
		printf("%s %d\n",__FUNCTION__,__LINE__);
		printf("  StorageClass pointer hasn't been set.\n");
	}
	keyToTargetHash.UpdateElement(key,attrib);
}

template <class toFind,class StorageClass,class HandleClass>
YSRESULT YsHandleToAttribTable<toFind,StorageClass,HandleClass>::DeleteAttrib(HandleClass hd)
{
	return keyToTargetHash.DeleteKey(store->GetSearchKey(hd));
}

template <class toFind,class StorageClass,class HandleClass>
YSRESULT YsHandleToAttribTable<toFind,StorageClass,HandleClass>::DeleteAttrib(YSHASHKEY key)
{
	return keyToTargetHash.DeleteKey(key);
}

template <class toFind,class StorageClass,class HandleClass>
const toFind *YsHandleToAttribTable<toFind,StorageClass,HandleClass>::operator[](HandleClass hd) const
{
	if(NULL==store)
	{
		printf("%s %d\n",__FUNCTION__,__LINE__);
		printf("  StorageClass pointer hasn't been set.\n");
	}
	return keyToTargetHash.FindElement(store->GetSearchKey(hd));
}
template <class toFind,class StorageClass,class HandleClass>
toFind *YsHandleToAttribTable<toFind,StorageClass,HandleClass>::operator[](HandleClass hd)
{
	if(NULL==store)
	{
		printf("%s %d\n",__FUNCTION__,__LINE__);
		printf("  StorageClass pointer hasn't been set.\n");
	}
	return keyToTargetHash.FindElement(store->GetSearchKey(hd));
}

template <class toFind,class StorageClass,class HandleClass>
const toFind *YsHandleToAttribTable<toFind,StorageClass,HandleClass>::operator[](YSHASHKEY key) const
{
	return keyToTargetHash.FindElement(key);
}

template <class toFind,class StorageClass,class HandleClass>
toFind *YsHandleToAttribTable<toFind,StorageClass,HandleClass>::operator[](YSHASHKEY key)
{
	return keyToTargetHash.FindElement(key);
}

template <class toFind,class StorageClass,class HandleClass>
YsUnidirectionalHandleEnumerator <YsHashTable <toFind>,YsHashElementEnumHandle> YsHandleToAttribTable<toFind,StorageClass,HandleClass>::AllHandle(void) const
{
	return keyToTargetHash.AllHandle();
}
template <class toFind,class StorageClass,class HandleClass>
const YSHASHKEY YsHandleToAttribTable <toFind,StorageClass,HandleClass>::GetKey(YsHashElementEnumHandle eeHd) const
{
	return keyToTargetHash.Key(eeHd);
}
template <class toFind,class StorageClass,class HandleClass>
toFind &YsHandleToAttribTable<toFind,StorageClass,HandleClass>::GetAttrib(YsHashElementEnumHandle eeHd)
{
	return keyToTargetHash.GetElement(eeHd);
}
template <class toFind,class StorageClass,class HandleClass>
const toFind &YsHandleToAttribTable<toFind,StorageClass,HandleClass>::GetAttrib(YsHashElementEnumHandle eeHd) const
{
	return keyToTargetHash.GetElement(eeHd);
}


/*! StorageClass must have a member function:
      YSHASHKEY StorageClass::GetSearchKey(HandleClass hd) const;
    which returns the hash key for the handle.
*/
template <class toFind,class StorageClass,class HandleClass,const int nKey>
class YsMultiHandleToAttribTable
{
private:
	const StorageClass *store;
	YsFixedLengthHashTable <toFind,nKey> keyToAttrib;
public:
	typedef typename YsFixedLengthHashTable<toFind,nKey>::ElemEnumHandle ElemEnumHandle;

	YsMultiHandleToAttribTable();
	YsMultiHandleToAttribTable(const StorageClass &store);
	YsMultiHandleToAttribTable(const YsMultiHandleToAttribTable <toFind,StorageClass,HandleClass,nKey> &incoming);
	YsMultiHandleToAttribTable(YsMultiHandleToAttribTable <toFind,StorageClass,HandleClass,nKey> &&incoming);
	YsMultiHandleToAttribTable <toFind,StorageClass,HandleClass,nKey> &operator=(const YsMultiHandleToAttribTable <toFind,StorageClass,HandleClass,nKey> &incoming);
	YsMultiHandleToAttribTable <toFind,StorageClass,HandleClass,nKey> &operator=(YsMultiHandleToAttribTable <toFind,StorageClass,HandleClass,nKey> &&incoming);
	void CopyFrom(const YsMultiHandleToAttribTable <toFind,StorageClass,HandleClass,nKey> &incoming);
	void MoveFrom(YsMultiHandleToAttribTable <toFind,StorageClass,HandleClass,nKey> &incoming);

	void CleanUp(void);
	void SetStorage(const StorageClass &store);
	void SetAttrib(const HandleClass hd[nKey],const toFind &attrib);
	void SetAttrib(const HandleClass hd[nKey],toFind &&attrib);
	void SetAttrib(const YSHASHKEY key[nKey],const toFind &attrib);
	void SetAttrib(const YSHASHKEY key[nKey],toFind &&attrib);

	toFind *FindAttrib(const HandleClass hd[nKey]);
	const toFind *FindAttrib(const HandleClass hd[nKey]) const;
	toFind *FindAttrib(const YSHASHKEY hd[nKey]);
	const toFind *FindAttrib(const YSHASHKEY hd[nKey]) const;

	YsUnidirectionalHandleEnumerator <YsFixedLengthHashTable <toFind,nKey>,YsHashElementEnumHandle> AllHandle(void) const;
	const YSHASHKEY *GetKey(YsHashElementEnumHandle eeHd) const;
	toFind &GetAttrib(YsHashElementEnumHandle eeHd);
	const toFind &GetAttrib(YsHashElementEnumHandle eeHd) const;

	// Needed for YsUnidirectionalHandleIterator and YsUnidirectionalHandleEnumerator
	inline void MoveToNext(ElemEnumHandle &elHd) const
	{
		return keyToAttrib.MoveToNext(elHd);
	}
	inline ElemEnumHandle First(void) const
	{
		return keyToAttrib.First();
	}
	inline ElemEnumHandle Null(void) const
	{
		return keyToAttrib.Null();
	}
	inline toFind &Value(ElemEnumHandle elHd)
	{
		return keyToAttrib.Value(elHd);
	}
	inline const toFind &Value(ElemEnumHandle elHd) const
	{
		return keyToAttrib.Value(elHd);
	}
};

template <class toFind,class StorageClass,class HandleClass,const int nKey>
YsMultiHandleToAttribTable<toFind,StorageClass,HandleClass,nKey>::YsMultiHandleToAttribTable()
{
	store=NULL;
}
template <class toFind,class StorageClass,class HandleClass,const int nKey>
YsMultiHandleToAttribTable<toFind,StorageClass,HandleClass,nKey>::YsMultiHandleToAttribTable(const StorageClass &store)
{
	this->store=&store;
}
template <class toFind,class StorageClass,class HandleClass,const int nKey>
YsMultiHandleToAttribTable<toFind,StorageClass,HandleClass,nKey>::YsMultiHandleToAttribTable(const YsMultiHandleToAttribTable <toFind,StorageClass,HandleClass,nKey> &incoming)
{
	CopyFrom(incoming);
}
template <class toFind,class StorageClass,class HandleClass,const int nKey>
YsMultiHandleToAttribTable<toFind,StorageClass,HandleClass,nKey>::YsMultiHandleToAttribTable(YsMultiHandleToAttribTable <toFind,StorageClass,HandleClass,nKey> &&incoming)
{
	MoveFrom(incoming);
}
template <class toFind,class StorageClass,class HandleClass,const int nKey>
YsMultiHandleToAttribTable <toFind,StorageClass,HandleClass,nKey> &YsMultiHandleToAttribTable<toFind,StorageClass,HandleClass,nKey>::operator=(const YsMultiHandleToAttribTable <toFind,StorageClass,HandleClass,nKey> &incoming)
{
	CopyFrom(incoming);
	return *this;
}
template <class toFind,class StorageClass,class HandleClass,const int nKey>
YsMultiHandleToAttribTable <toFind,StorageClass,HandleClass,nKey> &YsMultiHandleToAttribTable<toFind,StorageClass,HandleClass,nKey>::operator=(YsMultiHandleToAttribTable <toFind,StorageClass,HandleClass,nKey> &&incoming)
{
	MoveFrom(incoming);
	return *this;
}
template <class toFind,class StorageClass,class HandleClass,const int nKey>
void YsMultiHandleToAttribTable<toFind,StorageClass,HandleClass,nKey>::CopyFrom(const YsMultiHandleToAttribTable <toFind,StorageClass,HandleClass,nKey> &incoming)
{
	store=incoming.store;
	keyToAttrib.CopyFrom(incoming.keyToAttrib);
}
template <class toFind,class StorageClass,class HandleClass,const int nKey>
void YsMultiHandleToAttribTable<toFind,StorageClass,HandleClass,nKey>::MoveFrom(YsMultiHandleToAttribTable <toFind,StorageClass,HandleClass,nKey> &incoming)
{
	store=incoming.store;
	keyToAttrib.MoveFrom(incoming.keyToAttrib);
}
template <class toFind,class StorageClass,class HandleClass,const int nKey>
void YsMultiHandleToAttribTable<toFind,StorageClass,HandleClass,nKey>::CleanUp(void)
{
	keyToAttrib.CleanUpThin();
}
template <class toFind,class StorageClass,class HandleClass,const int nKey>
void YsMultiHandleToAttribTable<toFind,StorageClass,HandleClass,nKey>::SetStorage(const StorageClass &store)
{
	this->store=&store;
}
template <class toFind,class StorageClass,class HandleClass,const int nKey>
void YsMultiHandleToAttribTable<toFind,StorageClass,HandleClass,nKey>::SetAttrib(const HandleClass hd[nKey],const toFind &attrib)
{
	if(NULL==store)
	{
		printf("%s %d\n",__FUNCTION__,__LINE__);
		printf("  StorageClass pointer not set.\n");
	}
	YSHASHKEY hashKey[nKey];
	for(int idx=0; idx<nKey; ++idx)
	{
		hashKey[idx]=store->GetSearchKey(hd[idx]);
	}
	keyToAttrib.UpdateElement(nKey,hashKey,attrib);
}
template <class toFind,class StorageClass,class HandleClass,const int nKey>
void YsMultiHandleToAttribTable<toFind,StorageClass,HandleClass,nKey>::SetAttrib(const HandleClass hd[nKey],toFind &&attrib)
{
	if(NULL==store)
	{
		printf("%s %d\n",__FUNCTION__,__LINE__);
		printf("  StorageClass pointer not set.\n");
	}
	YSHASHKEY hashKey[nKey];
	for(int idx=0; idx<nKey; ++idx)
	{
		hashKey[idx]=store->GetSearchKey(hd[idx]);
	}
	keyToAttrib.UpdateElement(nKey,hashKey,attrib);
}

template <class toFind,class StorageClass,class HandleClass,const int nKey>
void YsMultiHandleToAttribTable<toFind,StorageClass,HandleClass,nKey>::SetAttrib(const YSHASHKEY key[nKey],const toFind &attrib)
{
	if(NULL==store)
	{
		printf("%s %d\n",__FUNCTION__,__LINE__);
		printf("  StorageClass pointer not set.\n");
	}
	keyToAttrib.UpdateElement(nKey,key,attrib);
}
template <class toFind,class StorageClass,class HandleClass,const int nKey>
void YsMultiHandleToAttribTable<toFind,StorageClass,HandleClass,nKey>::SetAttrib(const YSHASHKEY key[nKey],toFind &&attrib)
{
	if(NULL==store)
	{
		printf("%s %d\n",__FUNCTION__,__LINE__);
		printf("  StorageClass pointer not set.\n");
	}
	keyToAttrib.UpdateElement(nKey,key,attrib);
}

template <class toFind,class StorageClass,class HandleClass,const int nKey>
toFind *YsMultiHandleToAttribTable<toFind,StorageClass,HandleClass,nKey>::FindAttrib(const HandleClass hd[nKey])
{
	if(NULL==store)
	{
		printf("%s %d\n",__FUNCTION__,__LINE__);
		printf("  StorageClass pointer not set.\n");
	}
	YSHASHKEY hashKey[nKey];
	for(int idx=0; idx<nKey; ++idx)
	{
		hashKey[idx]=store->GetSearchKey(hd[idx]);
	}
	return keyToAttrib.FindElement(nKey,hashKey);
}

template <class toFind,class StorageClass,class HandleClass,const int nKey>
const toFind *YsMultiHandleToAttribTable<toFind,StorageClass,HandleClass,nKey>::FindAttrib(const HandleClass hd[nKey]) const
{
	if(NULL==store)
	{
		printf("%s %d\n",__FUNCTION__,__LINE__);
		printf("  StorageClass pointer not set.\n");
	}
	YSHASHKEY hashKey[nKey];
	for(int idx=0; idx<nKey; ++idx)
	{
		hashKey[idx]=store->GetSearchKey(hd[idx]);
	}
	return keyToAttrib.FindElement(nKey,hashKey);
}

template <class toFind,class StorageClass,class HandleClass,const int nKey>
toFind *YsMultiHandleToAttribTable<toFind,StorageClass,HandleClass,nKey>::FindAttrib(const YSHASHKEY key[nKey])
{
	if(NULL==store)
	{
		printf("%s %d\n",__FUNCTION__,__LINE__);
		printf("  StorageClass pointer not set.\n");
	}
	return keyToAttrib.FindElement(nKey,key);
}

template <class toFind,class StorageClass,class HandleClass,const int nKey>
const toFind *YsMultiHandleToAttribTable<toFind,StorageClass,HandleClass,nKey>::FindAttrib(const YSHASHKEY key[nKey]) const
{
	if(NULL==store)
	{
		printf("%s %d\n",__FUNCTION__,__LINE__);
		printf("  StorageClass pointer not set.\n");
	}
	return keyToAttrib.FindElement(nKey,key);
}

template <class toFind,class StorageClass,class HandleClass,const int nKey>
YsUnidirectionalHandleEnumerator <YsFixedLengthHashTable <toFind,nKey>,YsHashElementEnumHandle> YsMultiHandleToAttribTable<toFind,StorageClass,HandleClass,nKey>::AllHandle(void) const
{
	return keyToAttrib.AllHandle();
}

template <class toFind,class StorageClass,class HandleClass,const int nKey>
const YSHASHKEY *YsMultiHandleToAttribTable<toFind,StorageClass,HandleClass,nKey>::GetKey(YsHashElementEnumHandle eeHd) const
{
	return keyToAttrib.GetKey(eeHd);
}

template <class toFind,class StorageClass,class HandleClass,const int nKey>
toFind &YsMultiHandleToAttribTable<toFind,StorageClass,HandleClass,nKey>::GetAttrib(YsHashElementEnumHandle eeHd)
{
	return keyToAttrib.GetElement(eeHd);
}

template <class toFind,class StorageClass,class HandleClass,const int nKey>
const toFind &YsMultiHandleToAttribTable<toFind,StorageClass,HandleClass,nKey>::GetAttrib(YsHashElementEnumHandle eeHd) const
{
	return keyToAttrib.GetElement(eeHd);
}

////////////////////////////////////////////////////////////

/*! StorageClass must have a member function:
      YSHASHKEY GetSearchKey(HandleClass hd) const; */
template <class toFind,class StorageClass,class HandleClass,const int nKey>
class YsMultiHandleToMultiAttribTable
{
private:
	const StorageClass *storage;
	YsFixedLengthToMultiHashTable <toFind,nKey,1> table;
public:
	YsMultiHandleToMultiAttribTable();
	YsMultiHandleToMultiAttribTable(const StorageClass &storage);
	YsMultiHandleToMultiAttribTable(const YsMultiHandleToMultiAttribTable &incoming);
	YsMultiHandleToMultiAttribTable(YsMultiHandleToMultiAttribTable &&incoming);
	YsMultiHandleToMultiAttribTable <toFind,StorageClass,HandleClass,nKey> &operator=(const YsMultiHandleToMultiAttribTable &incoming);
	YsMultiHandleToMultiAttribTable <toFind,StorageClass,HandleClass,nKey> &operator=(YsMultiHandleToMultiAttribTable &&incoming);
	void CopyFrom(const YsMultiHandleToMultiAttribTable &incoming);
	void MoveFrom(YsMultiHandleToMultiAttribTable &incoming);

	void CleanUp(void);
	void SetStorage(const StorageClass &storage);

	YSRESULT AddAttrib(const HandleClass hd[nKey],const toFind &elem);
	YSRESULT AddAttrib(const YSHASHKEY hd[nKey],const toFind &elem);

	YSRESULT DeleteAttrib(const HandleClass hd[nKey],const toFind &elem);
	YSRESULT DeleteKey(const HandleClass hd[nKey]);

	YSRESULT DeleteAttrib(const YSHASHKEY key[nKey],const toFind &elem);
	YSRESULT DeleteKey(const YSHASHKEY key[nKey]);

	YSRESULT FindAttrib(int &nFind,const toFind *&find,const HandleClass hd[nKey]) const;
	YSRESULT FindAttrib(int &nFind,const toFind *&find,const YSHASHKEY key[nKey]) const;

	YsArray <toFind> FindAttrib(const HandleClass hd[nKey]) const;
	YsArray <toFind> FindAttrib(const YSHASHKEY hd[nKey]) const;
};


template <class toFind,class StorageClass,class HandleClass,const int nKey>
YsMultiHandleToMultiAttribTable<toFind,StorageClass,HandleClass,nKey>::YsMultiHandleToMultiAttribTable()
{
	this->storage=NULL;
}
template <class toFind,class StorageClass,class HandleClass,const int nKey>
YsMultiHandleToMultiAttribTable<toFind,StorageClass,HandleClass,nKey>::YsMultiHandleToMultiAttribTable(const StorageClass &storage)
{
	this->storage=&storage;
}
template <class toFind,class StorageClass,class HandleClass,const int nKey>
YsMultiHandleToMultiAttribTable<toFind,StorageClass,HandleClass,nKey>::YsMultiHandleToMultiAttribTable(const YsMultiHandleToMultiAttribTable &incoming)
{
	CopyFrom(incoming);
}
template <class toFind,class StorageClass,class HandleClass,const int nKey>
YsMultiHandleToMultiAttribTable<toFind,StorageClass,HandleClass,nKey>::YsMultiHandleToMultiAttribTable(YsMultiHandleToMultiAttribTable &&incoming)
{
	MoveFrom(incoming);
}
template <class toFind,class StorageClass,class HandleClass,const int nKey>
YsMultiHandleToMultiAttribTable <toFind,StorageClass,HandleClass,nKey> &YsMultiHandleToMultiAttribTable<toFind,StorageClass,HandleClass,nKey>::operator=(const YsMultiHandleToMultiAttribTable &incoming)
{
	CopyFrom(incoming);
	return *this;
}
template <class toFind,class StorageClass,class HandleClass,const int nKey>
YsMultiHandleToMultiAttribTable <toFind,StorageClass,HandleClass,nKey> &YsMultiHandleToMultiAttribTable<toFind,StorageClass,HandleClass,nKey>::operator=(YsMultiHandleToMultiAttribTable &&incoming)
{
	MoveFrom(incoming);
	return *this;
}
template <class toFind,class StorageClass,class HandleClass,const int nKey>
void YsMultiHandleToMultiAttribTable<toFind,StorageClass,HandleClass,nKey>::CopyFrom(const YsMultiHandleToMultiAttribTable &incoming)
{
	this->storage=incoming.storage;
	this->table.CopyFrom(incoming.table);
}
template <class toFind,class StorageClass,class HandleClass,const int nKey>
void YsMultiHandleToMultiAttribTable<toFind,StorageClass,HandleClass,nKey>::MoveFrom(YsMultiHandleToMultiAttribTable &incoming)
{
	this->storage=incoming.storage;
	this->table.MoveFrom(incoming.table);
}
template <class toFind,class StorageClass,class HandleClass,const int nKey>
void YsMultiHandleToMultiAttribTable<toFind,StorageClass,HandleClass,nKey>::CleanUp(void)
{
	table.CleanUp();
}
template <class toFind,class StorageClass,class HandleClass,const int nKey>
void YsMultiHandleToMultiAttribTable<toFind,StorageClass,HandleClass,nKey>::SetStorage(const StorageClass &storage)
{
	this->storage=&storage;
}
template <class toFind,class StorageClass,class HandleClass,const int nKey>
YSRESULT YsMultiHandleToMultiAttribTable<toFind,StorageClass,HandleClass,nKey>::AddAttrib(const HandleClass hd[nKey],const toFind &elem)
{
	if(NULL==storage)
	{
		printf("%s %d\n",__FUNCTION__,__LINE__);
		printf("  Storage pointer not set.\n");
		printf("  Probably will crash.\n");
	}

	YSHASHKEY key[nKey];
	for(int i=0; i<nKey; ++i)
	{
		key[i]=storage->GetSearchKey(hd[i]);
	}

	return table.AddElement(nKey,key,elem);
}
template <class toFind,class StorageClass,class HandleClass,const int nKey>
YSRESULT YsMultiHandleToMultiAttribTable<toFind,StorageClass,HandleClass,nKey>::AddAttrib(const YSHASHKEY key[nKey],const toFind &elem)
{
	return table.AddElement(nKey,key,elem);
}
template <class toFind,class StorageClass,class HandleClass,const int nKey>
YSRESULT YsMultiHandleToMultiAttribTable<toFind,StorageClass,HandleClass,nKey>::DeleteAttrib(const HandleClass hd[nKey],const toFind &elem)
{
	if(NULL==storage)
	{
		printf("%s %d\n",__FUNCTION__,__LINE__);
		printf("  Storage pointer not set.\n");
		printf("  Probably will crash.\n");
	}

	YSHASHKEY key[nKey];
	for(int i=0; i<nKey; ++i)
	{
		key[i]=storage->GetSearchKey(hd[i]);
	}
	return table.DeleteElement(nKey,key,elem);
}
template <class toFind,class StorageClass,class HandleClass,const int nKey>
YSRESULT YsMultiHandleToMultiAttribTable<toFind,StorageClass,HandleClass,nKey>::DeleteKey(const HandleClass hd[nKey])
{
	if(NULL==storage)
	{
		printf("%s %d\n",__FUNCTION__,__LINE__);
		printf("  Storage pointer not set.\n");
		printf("  Probably will crash.\n");
	}

	YSHASHKEY key[nKey];
	for(int i=0; i<nKey; ++i)
	{
		key[i]=storage->GetSearchKey(hd[i]);
	}

	return table.DeleteKey(nKey,key);
}
template <class toFind,class StorageClass,class HandleClass,const int nKey>
YSRESULT YsMultiHandleToMultiAttribTable<toFind,StorageClass,HandleClass,nKey>::DeleteAttrib(const YSHASHKEY key[nKey],const toFind &elem)
{
	return table.DeleteElement(nKey,key,elem);
}
template <class toFind,class StorageClass,class HandleClass,const int nKey>
YSRESULT YsMultiHandleToMultiAttribTable<toFind,StorageClass,HandleClass,nKey>::DeleteKey(const YSHASHKEY key[nKey])
{
	return table.DeleteKey(nKey,key);
}
template <class toFind,class StorageClass,class HandleClass,const int nKey>
YSRESULT YsMultiHandleToMultiAttribTable<toFind,StorageClass,HandleClass,nKey>::FindAttrib(int &nFind,const toFind *&find,const HandleClass hd[nKey]) const
{
	if(NULL==storage)
	{
		printf("%s %d\n",__FUNCTION__,__LINE__);
		printf("  Storage pointer not set.\n");
		printf("  Probably will crash.\n");
	}

	YSHASHKEY key[nKey];
	for(int i=0; i<nKey; ++i)
	{
		key[i]=storage->GetSearchKey(hd[i]);
	}

	return table.FindElement(nFind,find,nKey,key);
}
template <class toFind,class StorageClass,class HandleClass,const int nKey>
YSRESULT YsMultiHandleToMultiAttribTable<toFind,StorageClass,HandleClass,nKey>::FindAttrib(int &nFind,const toFind *&find,const YSHASHKEY key[nKey]) const
{
	return table.FindElement(nFind,find,nKey,key);
}

template <class toFind,class StorageClass,class HandleClass,const int nKey>
YsArray <toFind> YsMultiHandleToMultiAttribTable<toFind,StorageClass,HandleClass,nKey>::FindAttrib(const HandleClass hd[nKey]) const
{
	YsArray <toFind> result;

	int nFind;
	const toFind *found;
	if(YSOK==FindAttrib(nFind,found,hd))
	{
		result.Set(nFind,found);
	}
	return result;
}

template <class toFind,class StorageClass,class HandleClass,const int nKey>
YsArray <toFind> YsMultiHandleToMultiAttribTable<toFind,StorageClass,HandleClass,nKey>::FindAttrib(const YSHASHKEY key[nKey]) const
{
	YsArray <toFind> result;

	int nFind;
	const toFind *found;
	if(YSOK==FindAttrib(nFind,found,key))
	{
		result.Set(nFind,found);
	}
	return result;
}

////////////////////////////////////////////////////////////

class YsKeyStore
{
private:
	YSBOOL autoResizing;
	YSSIZE_T autoResizeMin,autoResizeMax;
	YSSIZE_T nTab,nKey;
	YsArray <YSHASHKEY> **keyArray;

public:
	class KeyPointer
	{
	public:
		YSSIZE_T row,column;
	};

	/*! Support for STL-like iterator. */
	class const_iterator
	{
	friend class YsKeyStore;

	private:
		const YsKeyStore *table;
		KeyPointer hd;
		YSBOOL ended;
	public:
		inline const_iterator &operator++();
		inline const_iterator operator++(int);
		inline bool operator==(const const_iterator &from);
		inline bool operator!=(const const_iterator &from);
		inline const YSHASHKEY operator*();
	};

	/*! Support for STL-like iterator */
	inline const_iterator begin() const;

	/*! Support for STL-like iterator */
	inline const_iterator end() const;



	YsKeyStore(YSSIZE_T nTable=16);
	YsKeyStore(const YsKeyStore &incoming);
	YsKeyStore(YsKeyStore &&incoming);
	~YsKeyStore();
	void CleanUp(void);
	void CleanUpThin(void);

	/*! This function clean up all keys and tables. */
	void CleanUpDeep(void);

	/*! This function copies incoming two-key store 'incoming' into this two-key store. */
	void CopyFrom(const YsKeyStore &incoming);

	/*! Copy operator. */
	YsKeyStore &operator=(const YsKeyStore &incoming);

	/*! This function moves incoming two-key store 'incoming' into this two-key store. */
	void MoveFrom(YsKeyStore &incoming);

	/*! Move operator. */
	YsKeyStore &operator=(YsKeyStore &&incoming);


	void PrepareTable(void);
	void EnableAutoResizing(YSSIZE_T minSize,YSSIZE_T maxSize);
	void EnableAutoResizing(void);
	void DisableAutoResizing(void);
	void Resize(YSSIZE_T nTable);

	/*! Returns the number of keys stored in this YsKeyStore. */
	YSSIZE_T GetN(void) const;

	/*! Deprecated.  Use GetN() instead. */
	int GetNumKey(void) const;

	int GetNumTable(void) const;

	/*! Adds a key. */
	YSRESULT Add(YSHASHKEY key);

	/*! Deletes a key. */
	YSRESULT Delete(YSHASHKEY key);

	/*! Returns YSTRUE if the key is included, YSFALSE if not. */
	YSBOOL IsIncluded(YSHASHKEY key) const;

	/*! Deprecated.  Use Add instead. */
	YSRESULT AddKey(YSHASHKEY key);

	/*! Deprecated.  Use Delete instead. */
	YSRESULT DeleteKey(YSHASHKEY key);

	void NullifyPointer(KeyPointer &ptr) const;
	YSRESULT FindNextKey(KeyPointer &ptr) const;
	YSBOOL IsPointerValid(const KeyPointer &ptr) const;
	YSHASHKEY GetKey(const KeyPointer &ptr) const;

protected:
	void CheckResizeGrow(void);
	void CheckResizeShrink(void);

public:
	/*! For better interoperability with STL. */
	YSSIZE_T size(void) const
	{
		return GetN();
	}
};

inline YsKeyStore::const_iterator YsKeyStore::begin() const
{
	const_iterator iter;
	iter.table=this;
	iter.ended=YSFALSE;
	NullifyPointer(iter.hd);
	if(YSOK!=FindNextKey(iter.hd))
	{
		iter.ended=YSTRUE;
	}
	return iter;
}

inline YsKeyStore::const_iterator YsKeyStore::end() const
{
	const_iterator iter;
	iter.table=this;
	iter.ended=YSTRUE;
	return iter;
}

inline YsKeyStore::const_iterator &YsKeyStore::const_iterator::operator++()
{
	if(YSTRUE!=ended)
	{
		if(YSOK!=table->FindNextKey(hd))
		{
			ended=YSTRUE;
		}
	}
	return *this;
}

inline YsKeyStore::const_iterator YsKeyStore::const_iterator::operator++(int)
{
	const_iterator copy=*this;
	if(YSTRUE!=ended)
	{
		if(YSOK!=table->FindNextKey(hd))
		{
			ended=YSTRUE;
		}
	}
	return copy;
}


inline bool YsKeyStore::const_iterator::operator==(const YsKeyStore::const_iterator &from)
{
	if(this->ended==YSTRUE && from.ended==YSTRUE)
	{
		return true;
	}
	else if(this->ended!=YSTRUE && from.ended!=YSTRUE &&
	        this->hd.row==from.hd.row && this->hd.column==from.hd.column)
	{
		return true;
	}
	return false;
}


inline bool YsKeyStore::const_iterator::operator!=(const YsKeyStore::const_iterator &from)
{
	if(this->ended==YSTRUE && from.ended==YSTRUE)
	{
		return false;
	}
	else if(this->ended!=YSTRUE && from.ended!=YSTRUE &&
	        this->hd.row==from.hd.row && this->hd.column==from.hd.column)
	{
		return false;
	}
	return true;
}


inline const YSHASHKEY YsKeyStore::const_iterator::operator*()
{
	return table->GetKey(hd);
}

inline YsKeyStore::const_iterator begin(const YsKeyStore &keystore)
{
	return keystore.begin();
}

inline YsKeyStore::const_iterator end(const YsKeyStore &keystore)
{
	return keystore.end();
}

////////////////////////////////////////////////////////////

class YsTwoKeyStore
{
protected:
	void CheckResizeGrow(void);
	void CheckResizeShrink(void);

	YSBOOL autoResizing;
	YSSIZE_T autoResizeMin,autoResizeMax;
	YSSIZE_T nTab,nKey;
	YsArray <YSHASHKEY> **keyArray;
public:
	class KeyPointer
	{
	public:
		YSSIZE_T row,column;
	};

	/*! Support for STL-like iterator. */
	class const_iterator
	{
	friend class YsTwoKeyStore;

	private:
		const YsTwoKeyStore *table;
		KeyPointer hd;
		YSBOOL ended;
	public:
		inline const_iterator &operator++();
		inline const_iterator operator++(int);
		inline bool operator==(const const_iterator &from);
		inline bool operator!=(const const_iterator &from);
		inline const YsStaticArray <YSHASHKEY,2> operator*();
	};

	/*! Support for STL-like iterator */
	inline const_iterator begin() const;

	/*! Support for STL-like iterator */
	inline const_iterator end() const;



	YsTwoKeyStore(int nTable=16);
	YsTwoKeyStore(const YsTwoKeyStore &incoming);
	YsTwoKeyStore(YsTwoKeyStore &&incoming);
	~YsTwoKeyStore();

	/*! This function clean up all keys, but keep the table size the same. */
	void CleanUp(void);

	/*! This function clean up all keys and tables. */
	void CleanUpDeep(void);

	/*! This function copies incoming two-key store 'incoming' into this two-key store. */
	void CopyFrom(const YsTwoKeyStore &incoming);

	/*! Copy operator. */
	YsTwoKeyStore &operator=(const YsTwoKeyStore &incoming);

	/*! This function moves incoming two-key store 'incoming' into this two-key store. */
	void MoveFrom(YsTwoKeyStore &incoming);

	/*! Move operator. */
	YsTwoKeyStore &operator=(YsTwoKeyStore &&incoming);


	void PrepareTable(void);
	void EnableAutoResizing(YSSIZE_T minSize,YSSIZE_T maxSize);
	void EnableAutoResizing(void);
	void DisableAutoResizing(void);
	void Resize(YSSIZE_T nTable);

	/*! Returns the number of keys stored in this YsKeyStore. */
	YSSIZE_T GetN(void) const;

	/*! Deprecated.  Use GetN() instead. */
	int GetNumKey(void) const;

	int GetNumTable(void) const;

	YSRESULT AddKey(YSHASHKEY key1,YSHASHKEY key2);
	YSRESULT DeleteKey(YSHASHKEY key1,YSHASHKEY key2);
	YSBOOL IsIncluded(YSHASHKEY key1,YSHASHKEY key2) const;

	YSRESULT AddKey(const YSHASHKEY key[2]);
	YSRESULT DeleteKey(const YSHASHKEY key[2]);
	YSBOOL IsIncluded(const YSHASHKEY key[2]) const;

	void NullifyPointer(KeyPointer &ptr) const;
	YSRESULT FindNextKey(KeyPointer &ptr) const;
	YSBOOL IsPointerValid(const KeyPointer &ptr) const;
	YsStaticArray <YSHASHKEY,2> GetKey(const KeyPointer &ptr) const;
};

inline YsTwoKeyStore::const_iterator YsTwoKeyStore::begin() const
{
	const_iterator iter;
	iter.table=this;
	iter.ended=YSFALSE;
	NullifyPointer(iter.hd);
	if(YSOK!=FindNextKey(iter.hd))
	{
		iter.ended=YSTRUE;
	}
	return iter;
}

inline YsTwoKeyStore::const_iterator YsTwoKeyStore::end() const
{
	const_iterator iter;
	iter.table=this;
	iter.ended=YSTRUE;
	return iter;
}

inline YsTwoKeyStore::const_iterator &YsTwoKeyStore::const_iterator::operator++()
{
	if(YSTRUE!=ended)
	{
		if(YSOK!=table->FindNextKey(hd))
		{
			ended=YSTRUE;
		}
	}
	return *this;
}

inline YsTwoKeyStore::const_iterator YsTwoKeyStore::const_iterator::operator++(int)
{
	const_iterator copy=*this;
	if(YSTRUE!=ended)
	{
		if(YSOK!=table->FindNextKey(hd))
		{
			ended=YSTRUE;
		}
	}
	return copy;
}


inline bool YsTwoKeyStore::const_iterator::operator==(const YsTwoKeyStore::const_iterator &from)
{
	if(this->ended==YSTRUE && from.ended==YSTRUE)
	{
		return true;
	}
	else if(this->ended!=YSTRUE && from.ended!=YSTRUE &&
	        this->hd.row==from.hd.row && this->hd.column==from.hd.column)
	{
		return true;
	}
	return false;
}


inline bool YsTwoKeyStore::const_iterator::operator!=(const YsTwoKeyStore::const_iterator &from)
{
	if(this->ended==YSTRUE && from.ended==YSTRUE)
	{
		return false;
	}
	else if(this->ended!=YSTRUE && from.ended!=YSTRUE &&
	        this->hd.row==from.hd.row && this->hd.column==from.hd.column)
	{
		return false;
	}
	return true;
}


inline const YsStaticArray <YSHASHKEY,2> YsTwoKeyStore::const_iterator::operator*()
{
	return table->GetKey(hd);
}

inline YsTwoKeyStore::const_iterator begin(const YsTwoKeyStore &keystore)
{
	return keystore.begin();
}

inline YsTwoKeyStore::const_iterator end(const YsTwoKeyStore &keystore)
{
	return keystore.end();
}



/* } */
#endif



