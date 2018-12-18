/* ////////////////////////////////////////////////////////////

File Name: yshash_.h
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

template <class toFind>
class YsHashElement
{
public:
	toFind dat;
	unsigned key;
};

class YsHashElementEnumHandle
{
public:
	unsigned hashIdx;
	unsigned arrayIdx;
};

// Each element must have an unique key
template <class toFind>
class YsHashTable
{
public:
	YsHashTable(int hashSize);
	virtual ~YsHashTable();

	YSRESULT PrepareTable(void);
	YSRESULT AddElement(unsigned searchKey,toFind element);
	YSRESULT DeleteElement(unsigned searchKey,toFind element);
	YSRESULT FindElement(toFind &elem,unsigned searchKey) const;
	YSRESULT GetElement(toFind &elem,const YsHashElementEnumHandle &handle) const;

	YSRESULT RewindElementEnumHandle(YsHashElementEnumHandle &handle) const;
	YSRESULT FindNextElement(YsHashElementEnumHandle &handle) const;

	YSRESULT SelfDiagnostic(void) const;
protected:
	const int HashSize;
	mutable YsArray <YsHashElement <toFind> > *table;
	YsArray <YsHashElement <toFind> > *FindTableIndex(int &idx,unsigned searchKey) const;
};

template <class toFind>
YsHashTable <toFind> ::YsHashTable(int hashSize) : HashSize(hashSize)
{
	table=new YsArray <YsHashElement <toFind> >[hashSize];
}

template <class toFind>
YsHashTable <toFind> ::~YsHashTable()
{
	if(table!=NULL)
	{
		delete [] table;
	}
}

template <class toFind>
YSRESULT YsHashTable <toFind>::PrepareTable(void)
{
	int i;
	for(i=0; i<HashSize; i++)
	{
		table[i].Set(0,NULL);
	}
	return YSOK;
}

template <class toFind>
YSRESULT YsHashTable <toFind>::AddElement(unsigned searchKey,toFind element)
{
	int hashId,i1,i2,im;
	YsArray <YsHashElement <toFind> > *entry;
	YsHashElement <toFind> neo;

	hashId=searchKey%HashSize;
	entry=&table[hashId];

	neo.dat=element;
	neo.key=searchKey;

	if(entry->GetNumItem()==0)
	{
		entry->AppendItem(neo);
	}
	else
	{
		i1=0;
		i2=entry->GetNumItem()-1;

		if(searchKey<entry->GetItem(i1).key)
		{
			entry->Insert(i1,neo);
		}
		else if(entry->GetItem(i2).key<searchKey)
		{
			entry->AppendItem(neo);
		}
		else
		{
			i1=0;
			i2=entry->GetNumItem()-1;
			while(i2-i1>1)
			{
				im=(i1+i2)/2;
				if(searchKey<entry->GetItem(im).key)
				{
					i2=im;
				}
				else
				{
					i1=im;
				}
			}
			entry->Insert(i2,neo);
		}
	}
	return YSOK;
}

template <class toFind>
YSRESULT YsHashTable <toFind>::DeleteElement(unsigned searchKey,toFind element)
{
	YsArray <YsHashElement <toFind> > *entry;
	int idx;

	entry=FindTableIndex(idx,searchKey);
	if(entry!=NULL)
	{
		entry->Delete(idx);
		return YSOK;
	}

	YsPrintf::def->Printf("YsHashTable::DeleteElement()\n");
	YsPrintf::def->Printf("  Element not found\n");
	return YSERR;
}

template <class toFind>
YSRESULT YsHashTable <toFind>::FindElement(toFind &element,unsigned searchKey) const
{
	int idx;
	YsArray <YsHashElement <toFind> > *entry;

	entry=FindTableIndex(idx,searchKey);
	if(entry!=NULL)
	{
		element=entry->GetItem(idx).dat;
		return YSOK;
	}
	return YSERR;
}

template <class toFind>
YSRESULT YsHashTable <toFind>::GetElement(toFind &elem,const YsHashElementEnumHandle &handle) const
{
	int hashIdx,arrayIdx;

	hashIdx=handle.hashIdx;
	arrayIdx=handle.arrayIdx;

	if(0<=hashIdx && hashIdx<HashSize &&
	   0<=arrayIdx && arrayIdx<table[hashIdx].GetNumItem())
	{
		elem=table[hashIdx].GetItem(arrayIdx).dat;
		return YSOK;
	}
	return YSERR;
}

template <class toFind>
YSRESULT YsHashTable <toFind>::RewindElementEnumHandle(YsHashElementEnumHandle &handle) const
{
	// Rewind -> handle points to the first element
	int i;
	for(i=0; i<HashSize; i++)
	{
		if(table[i].GetNumItem()>0)
		{
			handle.hashIdx=i;
			handle.arrayIdx=0;
			return YSOK;
		}
	}
	handle.hashIdx=0;
	handle.arrayIdx=0;
	return YSERR;
}

template <class toFind>
YSRESULT YsHashTable <toFind>::FindNextElement(YsHashElementEnumHandle &handle) const
{
	int hashIdx,arrayIdx;
	hashIdx=handle.hashIdx;
	arrayIdx=handle.arrayIdx;

	arrayIdx++;
	if(arrayIdx<table[hashIdx].GetNumItem())
	{
		handle.hashIdx=hashIdx;
		handle.arrayIdx=arrayIdx;
		return YSOK;
	}

	hashIdx++;
	arrayIdx=0;

	while(hashIdx<HashSize)
	{
		if(0<table[hashIdx].GetNumItem())
		{
			handle.hashIdx=hashIdx;
			handle.arrayIdx=0;
			return YSOK;
		}
		hashIdx++;
	}

	handle.hashIdx=0;
	handle.arrayIdx=0;

	return YSERR;
}

template <class toFind>
YsArray <YsHashElement <toFind> > *YsHashTable <toFind>::FindTableIndex(int &idx,unsigned searchKey) const
{
	int hashId,i1,i2,im;
	YsArray <YsHashElement<toFind> > *entry;

	hashId=searchKey%HashSize;
	entry=&table[hashId];

	if(entry->GetNumItem()==0)
	{
		idx=0;
		return NULL;
	}
	else
	{
		i1=0;
		i2=entry->GetNumItem()-1;
		while(i2-i1>1)
		{
			im=(i1+i2)/2;
			if(searchKey<entry->GetItem(im).key)
			{
				i2=im;
			}
			else
			{
				i1=im;
			}
		}

		if(entry->GetItem(i1).key==searchKey)
		{
			idx=i1;
			return entry;
		}
		else if(entry->GetItem(i2).key==searchKey)
		{
			idx=i2;
			return entry;
		}
		else
		{
			return NULL;
		}
	}
	return NULL;
}


template <class toFind>
YSRESULT YsHashTable <toFind>::SelfDiagnostic(void) const
{
	// Check all elements in the table are sorted in search key order
	// Search key and hash id is consistent

	int i,j;
	YSBOOL err;
	YSRESULT res;
	res=YSOK;

	YsPrintf::def->Printf("Self Diagnostic YsHashTable\n");
	for(i=0; i<HashSize; i++)
	{
		if(i%20==0)
		{
			YsPrintf::def->Printf("%d/%d\r",i,HashSize);
		}

		err=YSFALSE;
		for(j=0; j<table[i].GetNumItem(); j++)
		{
			if(j>0)
			{
				if(table[i].GetItem(j-1).key>=table[i].GetItem(j).key)
				{
					res=YSERR;
					err=YSTRUE;
				}
			}
			if(table[i].GetItem(j).key%HashSize!=unsigned(i))
			{
				res=YSERR;
				YsPrintf::def->Printf("##Inconsistencies in hash table %d %d (HashSize=%d)\n",
				                      i,table[i].GetItem(j).key,HashSize);
			}
		}
		if(err==YSTRUE)
		{
			YsPrintf::def->Printf("##Elements are not sorted in key order\n");
			for(j=0; j<table[i].GetNumItem(); j++)
			{
				YsPrintf::def->Printf(" %d",table[i].GetItem(j).key);
			}
			YsPrintf::def->Printf("\n");
		}
	}
	YsPrintf::def->Printf("%d/%d\n",HashSize,HashSize);
	return res;
}


////////////////////////////////////////////////////////////

// Sub Sub Group : Group of elements that have same key

template <class toFind,
          const int unitKeyBufSize,const int minKeyBufSize,
          const int unitItemBufSize,const int minItemBufSize>
class YsHashSameKeyGroup
{
public:
	YsArray <unsigned,unitKeyBufSize,minKeyBufSize> key;
	YsArray <toFind,unitItemBufSize,minItemBufSize> dat;
	YsList <YsHashSameKeyGroup <toFind,unitKeyBufSize,minKeyBufSize,unitItemBufSize,minItemBufSize> > *thisInTheList;
};


extern int YsHashSameKeyGroupCreateDeleteCounter;

// Sub Group : Group of sub groups that have same keysum

template <class toFind,
          const int unitKeyBufSize,const int minKeyBufSize,
          const int unitItemBufSize,const int minItemBufSize>
class YsHashSameKeySumGroup
{
public:
	YsHashSameKeySumGroup();
	~YsHashSameKeySumGroup();
	YsList <YsHashSameKeyGroup <toFind,unitKeyBufSize,minKeyBufSize,unitItemBufSize,minItemBufSize> > *elmList;
};

template <class toFind,
          const int unitKeyBufSize,const int minKeyBufSize,
          const int unitItemBufSize,const int minItemBufSize>
        YsHashSameKeySumGroup<toFind,unitKeyBufSize,minKeyBufSize,unitItemBufSize,minItemBufSize>::
            YsHashSameKeySumGroup()
{
	YsHashSameKeyGroupCreateDeleteCounter++;
	elmList=NULL;
}

template <class toFind,
          const int unitKeyBufSize,const int minKeyBufSize,
          const int unitItemBufSize,const int minItemBufSize>
        YsHashSameKeySumGroup<toFind,unitKeyBufSize,minKeyBufSize,unitItemBufSize,minItemBufSize>::
            ~YsHashSameKeySumGroup()
{
	// YsPrintf("Confirmation : YsHashSameKeySumGroup is deleted.\n");
	YsHashSameKeyGroupCreateDeleteCounter--;
	elmList->DeleteList();
}



//

template <class toFind,
          const int unitKeyBufSize,const int minKeyBufSize,
          const int unitItemBufSize,const int minItemBufSize>
class YsMultiKeyHashElementEnumHandle
{
public:
	YsHashElementEnumHandle outerHandle;
	YsList <YsHashSameKeyGroup<toFind,unitKeyBufSize,minKeyBufSize,unitItemBufSize,minItemBufSize> >
	    *currentSubSubGroup;
};



// Table main body

template <class toFind,
          const int unitKeyBufSize=16,const int minKeyBufSize=1,
          const int unitItemBufSize=16,const int minItemBufSize=1>
class YsMultiKeyHash  : protected
    YsHashTable <YsHashSameKeySumGroup<toFind,unitKeyBufSize,minKeyBufSize,unitItemBufSize,minItemBufSize> *>
{
public:
	YsMultiKeyHash(int hashSize);
	virtual ~YsMultiKeyHash();

	YSRESULT PrepareTable(void);

	YSRESULT AddElement(int nKey,const unsigned key[],toFind element);

	YSRESULT DeleteElement(int nKey,const unsigned key[],toFind element);

	YSRESULT DeleteKey(int nKey,const unsigned key[]);

	const YsArray <toFind,unitItemBufSize,minItemBufSize> *FindElement(int nKey,const unsigned key[]) const;

	YSRESULT RewindElementEnumHandle
	    (YsMultiKeyHashElementEnumHandle <toFind,unitKeyBufSize,minKeyBufSize,unitItemBufSize,minItemBufSize> &handle) const;
	YSRESULT FindNextElement
	    (YsMultiKeyHashElementEnumHandle <toFind,unitKeyBufSize,minKeyBufSize,unitItemBufSize,minItemBufSize> &handle) const;
	const YsArray <toFind,unitItemBufSize,minItemBufSize> *GetElement
	    (YsMultiKeyHashElementEnumHandle <toFind,unitKeyBufSize,minKeyBufSize,unitItemBufSize,minItemBufSize> &handle) const;
	const YsArray <toFind,unitItemBufSize,minItemBufSize> *GetSeachKey
	    (YsMultiKeyHashElementEnumHandle <toFind,unitKeyBufSize,minKeyBufSize,unitItemBufSize,minItemBufSize> &handle) const;

protected:
	unsigned GetKeySum(int nKey,const unsigned key[]) const;
	YSBOOL CheckSameKey(int nKey1,const unsigned key1[],int nKey2,const unsigned key2[]) const;
	YSRESULT FindAddSameKeyGroup
	   (unsigned &keySum,
		YsArray <unsigned,unitKeyBufSize,minKeyBufSize> &key,
	    YsHashSameKeySumGroup <toFind,unitKeyBufSize,minKeyBufSize,unitItemBufSize,minItemBufSize> *&sameKeySumGroup,
	    YsHashSameKeyGroup <toFind,unitKeyBufSize,minKeyBufSize,unitItemBufSize,minItemBufSize> *&sameKeyGroup);
	YSRESULT FindSameKeyGroup
	   (unsigned &keySum,
		YsArray <unsigned,unitKeyBufSize,minKeyBufSize> &key,
	    YsHashSameKeySumGroup <toFind,unitKeyBufSize,minKeyBufSize,unitItemBufSize,minItemBufSize> *&sameKeySumGroup,
	    YsHashSameKeyGroup <toFind,unitKeyBufSize,minKeyBufSize,unitItemBufSize,minItemBufSize> *&sameKeyGroup) const;
};


template <class toFind,
          const int unitKeyBufSize,const int minKeyBufSize,
          const int unitItemBufSize,const int minItemBufSize>
		YsMultiKeyHash <toFind,unitKeyBufSize,minKeyBufSize,unitItemBufSize,minItemBufSize>::
			YsMultiKeyHash(int hashSize) : YsHashTable <YsHashSameKeySumGroup<toFind,unitKeyBufSize,minKeyBufSize,unitItemBufSize,minItemBufSize> *>(hashSize)
{
}

template <class toFind,
          const int unitKeyBufSize,const int minKeyBufSize,
          const int unitItemBufSize,const int minItemBufSize>
		YsMultiKeyHash <toFind,unitKeyBufSize,minKeyBufSize,unitItemBufSize,minItemBufSize>::
			~YsMultiKeyHash()
{
	int i,j;
	for(i=0; i<HashSize; i++)
	{
		for(j=0; j<table[i].GetNumItem(); j++)
		{
			delete table[i].GetEditableItem(j).dat;
		}
	}
}



template <class toFind,
          const int unitKeyBufSize,const int minKeyBufSize,
          const int unitItemBufSize,const int minItemBufSize>
YSRESULT
	YsMultiKeyHash <toFind,unitKeyBufSize,minKeyBufSize,unitItemBufSize,minItemBufSize>::
    	PrepareTable(void)
{
	int i,j;
	for(i=0; i<HashSize; i++)
	{
		for(j=0; j<table[i].GetNumItem(); j++)
		{
			delete table[i].GetItem(j).dat;
		}
	}

	return YsHashTable
	  <YsHashSameKeySumGroup<toFind,unitKeyBufSize,minKeyBufSize,unitItemBufSize,minItemBufSize> *>::
	    PrepareTable();
}



template <class toFind,
          const int unitKeyBufSize,const int minKeyBufSize,
          const int unitItemBufSize,const int minItemBufSize>
YSRESULT YsMultiKeyHash <toFind,unitKeyBufSize,minKeyBufSize,unitItemBufSize,minItemBufSize>
    ::AddElement(int nKey,const unsigned unsortedKey[],toFind element)
{
	unsigned keySum;
	YsArray <unsigned,unitKeyBufSize,minKeyBufSize> key;
	YsHashSameKeySumGroup<toFind,unitKeyBufSize,minKeyBufSize,unitItemBufSize,minItemBufSize> *sameKeySumGroup;
	YsHashSameKeyGroup<toFind,unitKeyBufSize,minKeyBufSize,unitItemBufSize,minItemBufSize> *sameKeyGroup;

	key.Set(nKey,unsortedKey);

	if(FindAddSameKeyGroup(keySum,key,sameKeySumGroup,sameKeyGroup)==YSOK)
	{
		sameKeyGroup->dat.AppendItem(element);
		return YSOK;
	}
	return YSERR;
}



template <class toFind,
          const int unitKeyBufSize,const int minKeyBufSize,
          const int unitItemBufSize,const int minItemBufSize>
YSRESULT
	YsMultiKeyHash <toFind,unitKeyBufSize,minKeyBufSize,unitItemBufSize,minItemBufSize>
    	::DeleteElement(int nKey,const unsigned unsortedKey[],toFind element)
{
	unsigned keySum;
	YsArray <unsigned,unitKeyBufSize,minKeyBufSize> key;
	YsHashSameKeySumGroup<toFind,unitKeyBufSize,minKeyBufSize,unitItemBufSize,minItemBufSize> *sameKeySumGroup;
	YsHashSameKeyGroup<toFind,unitKeyBufSize,minKeyBufSize,unitItemBufSize,minItemBufSize> *sameKeyGroup;

	key.Set(nKey,unsortedKey);  // key is sorted inside FindSameKeyGroup func

	if(FindSameKeyGroup(keySum,key,sameKeySumGroup,sameKeyGroup)==YSOK)
	{
		int i;
		for(i=0; i<sameKeyGroup->dat.GetNumItem(); i++)
		{
			if(sameKeyGroup->dat.GetItem(i)==element)
			{
				sameKeyGroup->dat.Delete(i);

				if(sameKeyGroup->dat.GetNumItem()==0)
				{
					YsList <YsHashSameKeyGroup<toFind,unitKeyBufSize,minKeyBufSize,unitItemBufSize,minItemBufSize> >
					    *seek;

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
					//	YsPrintf("YsMultiKeyHash::DeleteElement()\n");
					//	YsPrintf("  Internal error (1)\n");
					//}

					// Above part is replaced with >>  2000/06/05
					seek=sameKeyGroup->thisInTheList;
					if(seek->SeekTop()==sameKeySumGroup->elmList)
					{
						sameKeySumGroup->elmList=seek->DeleteFromList();
					}
					else
					{
						YsPrintf("YsMultiKeyHash::DeleteElement()\n");
						YsPrintf("  Internal error (1)\n");
					}
					// <<


					if(sameKeySumGroup->elmList==NULL)
					{
						YsHashTable
						    <YsHashSameKeySumGroup<toFind,unitKeyBufSize,minKeyBufSize,unitItemBufSize,minItemBufSize> *>::
						        DeleteElement(keySum,sameKeySumGroup);
						delete sameKeySumGroup;
					}
				}

				return YSOK;
			}
		}
	}
	YsPrintf("YsMultiKeyHash::DeleteElement()\n");
	YsPrintf("  Tried to delete an element that is not listed.\n");
	return YSERR;
}



template <class toFind,
          const int unitKeyBufSize,const int minKeyBufSize,
          const int unitItemBufSize,const int minItemBufSize>
YSRESULT
    YsMultiKeyHash <toFind,unitKeyBufSize,minKeyBufSize,unitItemBufSize,minItemBufSize>
        ::DeleteKey(int nKey,const unsigned unsortedKey[])
{
	unsigned keySum;
	YsArray <unsigned,unitKeyBufSize,minKeyBufSize> key;
	YsHashSameKeySumGroup<toFind,unitKeyBufSize,minKeyBufSize,unitItemBufSize,minItemBufSize> *sameKeySumGroup;
	YsHashSameKeyGroup<toFind,unitKeyBufSize,minKeyBufSize,unitItemBufSize,minItemBufSize> *sameKeyGroup;

	key.Set(nKey,unsortedKey);  // key is sorted inside FindSameKeyGroup func

	if(FindSameKeyGroup(keySum,key,sameKeySumGroup,sameKeyGroup)==YSOK)
	{
		YsList <YsHashSameKeyGroup<toFind,unitKeyBufSize,minKeyBufSize,unitItemBufSize,minItemBufSize> >
		    *seek;

		seek=sameKeyGroup->thisInTheList;
		if(seek->SeekTop()==sameKeySumGroup->elmList)
		{
			sameKeySumGroup->elmList=seek->DeleteFromList();
		}
		else
		{
			YsPrintf("YsMultiKeyHash::DeleteKey()\n");
			YsPrintf("  Internal error (1)\n");
		}

		if(sameKeySumGroup->elmList==NULL)
		{
			YsHashTable
			    <YsHashSameKeySumGroup<toFind,unitKeyBufSize,minKeyBufSize,unitItemBufSize,minItemBufSize> *>::
			        DeleteElement(keySum,sameKeySumGroup);
			delete sameKeySumGroup;
		}

		return YSOK;
	}
	YsPrintf("YsMultiKeyHash::DeleteKey()\n");
	YsPrintf("  Tried To Delete a key that is not listed.\n");
	return YSERR;
}



template <class toFind,
          const int unitKeyBufSize,const int minKeyBufSize,
          const int unitItemBufSize,const int minItemBufSize>
const YsArray <toFind,unitItemBufSize,minItemBufSize> *
     YsMultiKeyHash <toFind,unitKeyBufSize,minKeyBufSize,unitItemBufSize,minItemBufSize>::
         FindElement(int nKey,const unsigned unsortedKey[]) const
{
	unsigned keySum;
	YsArray <unsigned,unitKeyBufSize,minKeyBufSize> key;
	YsHashSameKeySumGroup<toFind,unitKeyBufSize,minKeyBufSize,unitItemBufSize,minItemBufSize> *sameKeySumGroup;
	YsHashSameKeyGroup<toFind,unitKeyBufSize,minKeyBufSize,unitItemBufSize,minItemBufSize> *sameKeyGroup;

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



template <class toFind,
          const int unitKeyBufSize,const int minKeyBufSize,
          const int unitItemBufSize,const int minItemBufSize>
YSRESULT
	YsMultiKeyHash <toFind,unitKeyBufSize,minKeyBufSize,unitItemBufSize,minItemBufSize>::
		RewindElementEnumHandle
	    	(YsMultiKeyHashElementEnumHandle <toFind,unitKeyBufSize,minKeyBufSize,unitItemBufSize,minItemBufSize>
	    	    &handle) const
{
	YsHashTable <YsHashSameKeySumGroup<toFind,unitKeyBufSize,minKeyBufSize,unitItemBufSize,minItemBufSize> *>::
	    RewindElementEnumHandle(handle.outerHandle);

	YsHashSameKeySumGroup<toFind,unitKeyBufSize,minKeyBufSize,unitItemBufSize,minItemBufSize> *subElem;

	handle.currentSubSubGroup=NULL;
	while(handle.currentSubSubGroup==NULL)
	{
		if(YsHashTable
		       <YsHashSameKeySumGroup<toFind,unitKeyBufSize,minKeyBufSize,unitItemBufSize,minItemBufSize> *>::
		           GetElement(subElem,handle.outerHandle)==YSOK)
		{
			if(subElem->elmList!=NULL)
			{
				handle.currentSubSubGroup=subElem->elmList;
				return YSOK;
			}
		}
		else if(YsHashTable
		       <YsHashSameKeySumGroup<toFind,unitKeyBufSize,minKeyBufSize,unitItemBufSize,minItemBufSize> *>::
		           FindNextElement(handle.outerHandle)!=YSOK)
		{
			return YSERR;
		}
	}
	return YSERR;
}



template <class toFind,
          const int unitKeyBufSize,const int minKeyBufSize,
          const int unitItemBufSize,const int minItemBufSize>
YSRESULT
	YsMultiKeyHash <toFind,unitKeyBufSize,minKeyBufSize,unitItemBufSize,minItemBufSize>::
		FindNextElement
	    	(YsMultiKeyHashElementEnumHandle <toFind,unitKeyBufSize,minKeyBufSize,unitItemBufSize,minItemBufSize>
	    	    &handle) const
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
		YsHashSameKeySumGroup<toFind,unitKeyBufSize,minKeyBufSize,unitItemBufSize,minItemBufSize> *subGroup;

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



template <class toFind,
          const int unitKeyBufSize,const int minKeyBufSize,
          const int unitItemBufSize,const int minItemBufSize>
const YsArray <toFind,unitItemBufSize,minItemBufSize> *
    YsMultiKeyHash <toFind,unitKeyBufSize,minKeyBufSize,unitItemBufSize,minItemBufSize>::
	    GetElement
	        (YsMultiKeyHashElementEnumHandle <toFind,unitKeyBufSize,minKeyBufSize,unitItemBufSize,minItemBufSize>
	             &handle) const
{
	if(handle.currentSubSubGroup!=NULL)
	{
		return &handle.currentSubSubGroup->dat;
	}
	return NULL;
}



template <class toFind,
          const int unitKeyBufSize,const int minKeyBufSize,
          const int unitItemBufSize,const int minItemBufSize>
const YsArray <toFind,unitItemBufSize,minItemBufSize> *
    YsMultiKeyHash <toFind,unitKeyBufSize,minKeyBufSize,unitItemBufSize,minItemBufSize>::
	    GetSeachKey
	        (YsMultiKeyHashElementEnumHandle <toFind,unitKeyBufSize,minKeyBufSize,unitItemBufSize,minItemBufSize>
	            &handle) const
{
	if(handle.currentSubSubGroup!=NULL)
	{
		return &handle.currentSubSubGroup->key;
	}
	return NULL;
}



template <class toFind,
          const int unitKeyBufSize,const int minKeyBufSize,
          const int unitItemBufSize,const int minItemBufSize>
unsigned
    YsMultiKeyHash <toFind,unitKeyBufSize,minKeyBufSize,unitItemBufSize,minItemBufSize>::
	    GetKeySum(int nKey,const unsigned key[]) const
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



template <class toFind,
          const int unitKeyBufSize,const int minKeyBufSize,
          const int unitItemBufSize,const int minItemBufSize>
YSBOOL
    YsMultiKeyHash <toFind,unitKeyBufSize,minKeyBufSize,unitItemBufSize,minItemBufSize>::
	    CheckSameKey(int nKey1,const unsigned key1[],int nKey2,const unsigned key2[]) const
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



template <class toFind,
          const int unitKeyBufSize,const int minKeyBufSize,
          const int unitItemBufSize,const int minItemBufSize>
YSRESULT
    YsMultiKeyHash <toFind,unitKeyBufSize,minKeyBufSize,unitItemBufSize,minItemBufSize>::
		FindAddSameKeyGroup
	       (unsigned &keySum,
			YsArray <unsigned,unitKeyBufSize,minKeyBufSize> &key,
	        YsHashSameKeySumGroup <toFind,unitKeyBufSize,minKeyBufSize,unitItemBufSize,minItemBufSize> *&sameKeySumGroup,
	        YsHashSameKeyGroup <toFind,unitKeyBufSize,minKeyBufSize,unitItemBufSize,minItemBufSize> *&sameKeyGroup)
{
	if(FindSameKeyGroup(keySum,key,sameKeySumGroup,sameKeyGroup)==YSOK)
	{
		return YSOK;
	}
	else
	{
		if(sameKeySumGroup==NULL)
		{
			sameKeySumGroup=new YsHashSameKeySumGroup<toFind,unitKeyBufSize,minKeyBufSize,unitItemBufSize,minItemBufSize>;
			YsHashTable
		       <YsHashSameKeySumGroup<toFind,unitKeyBufSize,minKeyBufSize,unitItemBufSize,minItemBufSize> *>::
		           AddElement(keySum,sameKeySumGroup);  // Pray for no error  :-)
		}

		YsList <YsHashSameKeyGroup<toFind,unitKeyBufSize,minKeyBufSize,unitItemBufSize,minItemBufSize> > *neo;

		neo=new YsList <YsHashSameKeyGroup<toFind,unitKeyBufSize,minKeyBufSize,unitItemBufSize,minItemBufSize> >;
		neo->dat.key=key;
		neo->dat.thisInTheList=neo;

		sameKeySumGroup->elmList=sameKeySumGroup->elmList->Append(neo);

		sameKeyGroup=&neo->dat;

		return YSOK;
	}
}



template <class toFind,
          const int unitKeyBufSize,const int minKeyBufSize,
          const int unitItemBufSize,const int minItemBufSize>
YSRESULT
    YsMultiKeyHash <toFind,unitKeyBufSize,minKeyBufSize,unitItemBufSize,minItemBufSize>::
		FindSameKeyGroup
	       (unsigned &keySum,
			YsArray <unsigned,unitKeyBufSize,minKeyBufSize> &key,
	        YsHashSameKeySumGroup <toFind,unitKeyBufSize,minKeyBufSize,unitItemBufSize,minItemBufSize> *&sameKeySumGroup,
	        YsHashSameKeyGroup <toFind,unitKeyBufSize,minKeyBufSize,unitItemBufSize,minItemBufSize> *&sameKeyGroup)
	            const
{
	sameKeySumGroup=NULL;
	sameKeyGroup=NULL;

	keySum=GetKeySum(key.GetNumItem(),key.GetArray());

	YsQuickSort <unsigned,void *> (key.GetNumItem(),key.GetEditableArray());

	if(YsHashTable
		  <YsHashSameKeySumGroup<toFind,unitKeyBufSize,minKeyBufSize,unitItemBufSize,minItemBufSize> *>::
		      FindElement(sameKeySumGroup,keySum)==YSOK)
		          // sameKeySumGroup : Group of sub elements that have same keysum
	{
		YsList <YsHashSameKeyGroup<toFind,unitKeyBufSize,minKeyBufSize,unitItemBufSize,minItemBufSize> > *seek;

		sameKeyGroup=NULL;
		for(seek=sameKeySumGroup->elmList; seek!=NULL; seek=seek->Next())
		{
			int nKey1,nKey2;
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

class YsFixedLengthToMultiHashElemEnumHandle
{
public:
	int idx1,idx2,objectiveIdx;
};

template <class toFind,int nKeyLng,int nDefObjLng>
class YsFixedLengthToMultiHashElement
{
public:
	unsigned key[nKeyLng];
	YsArray <toFind,nDefObjLng,nDefObjLng> objective;
};

template <class toFind,int nKeyLng,int nDefObjLng>
class YsFixedLengthToMultiHashTable
{
public:
	YsFixedLengthToMultiHashTable(int hSize)
	{
		keyList.Set(hSize,NULL);
		hashSize=hSize;
	}

	YSRESULT PrepareTable(void)
	{
		int i;
		for(i=0; i<hashSize; i++)
		{
			keyList[i].Set(0,NULL);
		}
		return YSOK;
	}

	YSRESULT AddElement(int nKey,const unsigned unorderedKey[],const toFind &objective)
	{
		unsigned orderedKey[nKeyLng],keySum;
		YsFixedLengthToMultiHashElement <toFind,nKeyLng,nDefObjLng> *entry;

		if(nKey==nKeyLng)
		{
			keySum=ComputeKeySum(unorderedKey);
			OrderKey(orderedKey,nKey,unorderedKey);
			entry=FindEntry(nKey,orderedKey,keySum);
			if(entry!=NULL)
			{
				entry->objective.Append(objective);
				return YSOK;
			}
			else
			{
				entry=CreateEntry(nKey,orderedKey);
				if(entry!=NULL)
				{
					entry->objective.Append(objective);
					return YSOK;
				}
				else
				{
					YsPrintf("YsFixedLengthToMultiHashTable::AddElement()\n");
					YsPrintf("  Out of memory warning.\n");
					return YSERR;
				}
			}
		}
		else
		{
			YsPrintf("YsFixedLengthToMultiHashTable::AddElement()\n");
			YsPrintf("  Key length does not match.\n");
			return YSERR;
		}
	}

	YSRESULT DeleteElement(int nKey,const unsigned unorderedKey[],const toFind &objective)
	{
		unsigned orderedKey[nKeyLng],keySum;

		if(nKey==nKeyLng)
		{
			int idx1,idx2;
			keySum=ComputeKeySum(unorderedKey);
			OrderKey(orderedKey,nKey,unorderedKey);
			if(FindEntryIndex(idx1,idx2,nKey,orderedKey,keySum)==YSOK)
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
						return YSOK;
					}
				}
			}
			YsPrintf("YsFixedLengthToMultiHashTable::DeleteElement()\n");
			YsPrintf("  Objective not found.\n");
			return YSERR;
		}
		else
		{
			YsPrintf("YsFixedLengthToMultiHashTable::DeleteElement()\n");
			YsPrintf("  Key length does not match.\n");
			return YSERR;
		}
	}

	YSRESULT DeleteKey(int nKey,const unsigned unorderedKey[])
	{
		unsigned keySum,orderedKey[nKeyLng];
		YsFixedLengthToMultiHashElement <toFind,nKeyLng,nDefObjLng> *entry;

		if(nKey==nKeyLng)
		{
			int idx1,idx2;
			keySum=ComputeKeySum(unorderedKey);
			OrderKey(orderedKey,nKey,unorderedKey);
			if(FindEntryIndex(idx1,idx2,nKey,orderedKey,keySum)==YSOK)
			{
				keyList[idx1].DeleteBySwapping(idx2);
				return YSOK;
			}
			return YSERR;
		}
		else
		{
			YsPrintf("YsFixedLengthToMultiHashTable::DeleteKey()\n");
			YsPrintf("  Key length does not match.\n");
			return YSERR;
		}
	}

	const YsArray <toFind,nDefObjLng,nDefObjLng> *FindElement(int nKey,const unsigned unorderedKey[]) const
	{
		const YsFixedLengthToMultiHashElement <toFind,nKeyLng,nDefObjLng> *entry;
		unsigned orderedKey[nKeyLng],keySum;

		if(nKey==nKeyLng)
		{
			keySum=ComputeKeySum(unorderedKey);
			if(keyList.GetItem(keySum%hashSize).GetN()>0)
			{
				OrderKey(orderedKey,nKey,unorderedKey);
				entry=FindEntry(nKey,orderedKey,keySum);
				if(entry!=NULL)
				{
					return &entry->objective;
				}
			}
			return NULL;
		}
		else
		{
			YsPrintf("YsFixedLengthToMultiHashTable::FindElement()\n");
			YsPrintf("  Key length does not match.\n");
			return NULL;
		}
	}

	YSRESULT RewindElemEnumHandle(YsFixedLengthToMultiHashElemEnumHandle &elHd) const
	{
		int i;
		for(i=0; i<hashSize; i++)
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

	YSRESULT FindNextElem(YsFixedLengthToMultiHashElemEnumHandle &elHd) const
	{
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

	YSRESULT FindNextKey(YsFixedLengthToMultiHashElemEnumHandle &elHd) const
	{
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

	const toFind &GetElem(YsFixedLengthToMultiHashElemEnumHandle &elHd) const
	{
		return keyList[elHd.idx1][elHd.idx2].objective[elHd.objectiveIdx];
	}

	const unsigned *GetKey(YsFixedLengthToMultiHashElemEnumHandle &elHd) const
	{
		return keyList[elHd.idx1][elHd.idx2].key;
	}

protected:
	unsigned ComputeKeySum(const unsigned key[]) const
	{
		int i;
		unsigned keySum;
		keySum=0;
		for(i=0; i<nKeyLng; i++)
		{
			keySum+=key[i];
		}
		return keySum;
	}

	YSRESULT OrderKey(unsigned orderedKey[],int nKey,const unsigned unorderedKey[]) const
	{
		int i;
		for(i=0; i<nKey; i++)
		{
			orderedKey[i]=unorderedKey[i];
		}
		return YsQuickSort <unsigned,unsigned> (nKey,orderedKey,NULL);
	}

	YSRESULT FindEntryIndex(int &idx1,int &idx2,int nKey,const unsigned orderedKey[],unsigned keySum) const
	{
		int i;

		idx1=keySum%hashSize;
		for(idx2=0; idx2<keyList[idx1].GetN(); idx2++)
		{
			for(i=nKeyLng-1; i>=0; i--)
			{
				if(keyList[idx1][idx2].key[i]!=orderedKey[i])
				{
					goto NEXTIDX2;
				}
			}
			return YSOK;
		NEXTIDX2:
			;
		}
		idx1=-1;
		idx2=-1;
		return YSERR;
	}

	const YsFixedLengthToMultiHashElement <toFind,nKeyLng,nDefObjLng> *FindEntry
	    (int nKey,const unsigned orderedKey[],unsigned keySum) const
	{
		int idx1,idx2;
		if(FindEntryIndex(idx1,idx2,nKey,orderedKey,keySum)==YSOK)
		{
			return &keyList[idx1][idx2];
		}
		return NULL;
	}

	YsFixedLengthToMultiHashElement <toFind,nKeyLng,nDefObjLng> *FindEntry
	    (int nKey,const unsigned orderedKey[],unsigned keySum)
	{
		int idx1,idx2;
		if(FindEntryIndex(idx1,idx2,nKey,orderedKey,keySum)==YSOK)
		{
			return &keyList[idx1][idx2];
		}
		return NULL;
	}

	YsFixedLengthToMultiHashElement <toFind,nKeyLng,nDefObjLng> *CreateEntry(int nKey,const unsigned orderedKey[])
	{
		unsigned keySum;
		int i;
		keySum=0;
		for(i=0; i<nKey; i++)
		{
			keySum+=orderedKey[i];
		}

		int idx1,idx2;
		idx1=keySum%hashSize;
		idx2=keyList[idx1].GetN();

		YsFixedLengthToMultiHashElement <toFind,nKeyLng,nDefObjLng> newElem;
		for(i=0; i<nKey; i++)
		{
			newElem.key[i]=orderedKey[i];
		}
		keyList[idx1].Append(newElem);
		return &keyList[idx1][idx2];
	}

	int hashSize;
	YsArray <YsArray <YsFixedLengthToMultiHashElement <toFind,nKeyLng,nDefObjLng> > > keyList;
};

////////////////////////////////////////////////////////////



/* } */
#endif



