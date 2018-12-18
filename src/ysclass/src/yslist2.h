/* ////////////////////////////////////////////////////////////

File Name: yslist2.h
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

#ifndef YSLIST__IS_INCLUDED
#define YSLIST__IS_INCLUDED
/* { */


/*
Caution:
  The function YsListContainer::Create does not necessarily calls the
  constructor of class T. */


#include <limits.h>
#include "ysmath.h"
#include "ysarray.h"

// \cond
extern int YsNPreAllocListItem;


template <class T>
class YsListContainer;

template <class T>
class YsListPreallocItem;

template <class T>
class YsListAllocator;
// \endcond

/*! A template class for a linked-list item used in YsListContainer class. 
  \tparam T Type of the item stored in the linked list. */
template <class T>
class YsListItem
{
// \cond
friend class YsListContainer <T>;
friend class YsListAllocator <T>;
friend class YsListPreallocItem <T>;

protected:
	YsListPreallocItem <T> *prealloc;
	YsListContainer <T> *belongTo;
	YsListItem <T> *next,*prev;
	int serial;
// \endcond

public:
	/*! This function returns a pointer to the YsListContainer object to which this item belongs. */
	inline YsListContainer <T> *GetContainer(void) const;
	/*! Object stored in this list item. */
	T dat;
};

template <class T>
inline YsListContainer <T> *YsListItem <T>::GetContainer(void) const
{
	return belongTo;
}

////////////////////////////////////////////////////////////

//! \cond
template <class T>
class YsListPreallocItem
{
friend class YsListContainer <T>;
friend class YsListAllocator <T>;

protected:
	inline YsListPreallocItem(int nAllocUnit,YsListAllocator <T> *owner);
	inline ~YsListPreallocItem();

	YsListAllocator <T> *belongTo;
	YsListPreallocItem <T> *next,*prev;

public:
	int nUsed,nBuffer;
	YsListItem <T> *buffer;
};

template <class T>
inline YsListPreallocItem <T>::YsListPreallocItem(int nAllocUnit,YsListAllocator <T> *owner)
{
	int i;

	buffer=new YsListItem <T> [nAllocUnit];
	if(buffer!=NULL)
	{
		buffer[0].prealloc=this;
		buffer[0].prev=NULL;
		buffer[0].next=&buffer[1];
		for(i=1; i<nAllocUnit-1; i++)
		{
			buffer[i].prealloc=this;
			buffer[i].prev=&buffer[i-1];
			buffer[i].next=&buffer[i+1];
		}
		buffer[nAllocUnit-1].prealloc=this;
		buffer[nAllocUnit-1].prev=&buffer[nAllocUnit-2];
		buffer[nAllocUnit-1].next=NULL;

		nUsed=0;

		belongTo=owner;
		next=NULL;
		prev=NULL;

		YsNPreAllocListItem++;
	}
	else
	{
		YsExceptionHandler::Exception(YsExceptionHandler::OUTOFMEMORY,"YsListPreallocItem()");
	}
}

template <class T>
inline YsListPreallocItem <T>::~YsListPreallocItem()
{
	YsNPreAllocListItem--;
	delete [] buffer;
}
//! \endcond


////////////////////////////////////////////////////////////

/*! This class is responsible for allocating items for YsListContainer class.
    To avoid excessive memory allocation and de-allocation, this class allocates
    a memory block for multiple objects at a time.

    When YsListContainer requests YsListAllocator to give one item, and if
    YsListAllocator has some unused items, a pointer to one of the unused items
    will be given to YsListContainer class.  If YsListAllocator has no unused 
    item, YsListAllocator allocates a new memory block for nUnit items, and 
    return a pointer to one of the newly allocated items to YsListContainer.

    When an item in a YsListContainer object is deleted, the item will be returned
    to the YsListAllocator class and becomes an unused item.

    Multiple YsListContainer can use to a single YsListAllocator.
    A YsListAllocator can be deleted before a YsListContainer using the 
    YsListAllocator.

    YsListAllocator and YsListContainer are not thread-safe.  To use 
    YsListAllocator and YsListContainer in a multi-threaded environment, 
    external mutex-lock must be used so that more than one thread try to
    modify YsListContainer object.

    \tparam T Type of the items stored in the linked list.
    */
template <class T>
class YsListAllocator
{
friend class YsListContainer <T>;

public:
	/*! Default constructor */
	inline YsListAllocator(int nAllocUnit=128);
	/*! Default destructor */
	inline ~YsListAllocator();

	/*! Test for its own integrity and print diagnostic messages to the console. */
	inline void SelfDiagnostic(void) const;
	/*! Set number of items that this object allocats at a time. */
	inline YSRESULT SetAllocUnit(int unit);
	/*! Returns number of items that this object allocates at a time. */
	inline int GetAllocUnit(void) const;
	/*! Frees blocks that have no used items. */
	inline YSRESULT CollectGarbage(void);
	/*! Outputs a text file for checking fragmentation. */
	inline YSRESULT SaveUsageMap(const char fn[]) const;

private:
	inline YsListItem <T> *Create(void);
	int nUnit;
	YsListPreallocItem <T> *preallocHead,*preallocTail;
	YsListItem <T> *availableHead,*availableTail;
};

template <class T>
inline YsListAllocator <T>::YsListAllocator(int nAllocUnit)
{
	availableHead=NULL;
	availableTail=NULL;
	preallocHead=NULL;
	preallocTail=NULL;
	nUnit=nAllocUnit;
}

template <class T>
inline YsListAllocator <T>::~YsListAllocator()
{
	YsListPreallocItem <T> *seek,*next;
	next=preallocHead;
	while((seek=next)!=NULL)
	{
		next=seek->next;
		if(seek->nUsed==0)
		{
			delete seek;
		}
		else
		{
			seek->next=NULL;
			seek->prev=NULL;
			seek->belongTo=NULL;  // This will be deleted when nUsed becomes zero.
		}
	}
}

template <class T>
inline void YsListAllocator <T>::SelfDiagnostic(void) const
{
	int nAvailable,nPrealloc,nUsed;
	YsListItem <T> *seekItem;
	YsListPreallocItem <T> *seekPrealloc;

	YsPrintf("YsListAllocator::SelfDiagnostic()\n");

	YsPrintf("  nUnit=%d\n",nUnit);
	nAvailable=0;
	for(seekItem=availableHead; seekItem!=NULL; seekItem=seekItem->next)
	{
		nAvailable++;
		if(seekItem->next!=NULL && seekItem->next->prev!=seekItem)
		{
			YsPrintf("seekItem->next->prev is not pointing seekItem\n");
		}
		if(seekItem->prev!=NULL && seekItem->prev->next!=seekItem)
		{
			YsPrintf("seekItem->prev->next is not pointing seekItem\n");
		}
	}
	YsPrintf("  %d available items.\n",nAvailable);

	nUsed=0;
	nPrealloc=0;
	for(seekPrealloc=preallocHead; seekPrealloc!=NULL; seekPrealloc=seekPrealloc->next)
	{
		nPrealloc++;
		nUsed+=seekPrealloc->nUsed;
	}
	YsPrintf("  %d preallocations.\n",nPrealloc);
	YsPrintf("  %d used items.\n",nUsed);
	YsPrintf("  %d total number of items.\n",nUsed+nAvailable);
	YsPrintf("  nPrealloc*nUnit=%d\n",nPrealloc*nUnit);
	if(nPrealloc*nUnit!=nUsed+nAvailable)
	{
		YsPrintf("  Total number of items and nPrealloc*nUnit does not match.\n");
	}
}

template <class T>
inline YsListItem <T> *YsListAllocator <T>::Create(void)
{
	if(availableHead==NULL)
	{
		YsListPreallocItem <T> *prealloc;
		prealloc=new YsListPreallocItem <T> (nUnit,this);
		if(prealloc!=NULL)
		{
			if(preallocTail!=NULL)
			{
				preallocTail->next=prealloc;
				prealloc->prev=preallocTail;
				preallocTail=prealloc;
			}
			else
			{
				preallocHead=prealloc;
				preallocTail=prealloc;
			}
			availableHead=&prealloc->buffer[0];
			availableTail=&prealloc->buffer[nUnit-1];
			prealloc->nBuffer=nUnit;
		}
		else
		{
			YsExceptionHandler::Exception(YsExceptionHandler::OUTOFMEMORY,"YsListAllocator::Create()");
			return NULL;
		}
	}

	YsListItem <T> *neo;
	neo=availableHead;
	availableHead=availableHead->next;
	if(availableHead!=NULL)
	{
		availableHead->prev=NULL;
	}
	else if(availableHead==NULL)
	{
		availableTail=NULL;
	}
	neo->prealloc->nUsed++;
	return neo;
}

template <class T>
inline YSRESULT YsListAllocator <T>::SetAllocUnit(int unit)
{
	if(unit>0)
	{
		nUnit=unit;
		return YSOK;
	}
	else
	{
		YsErrOut("YsListContainer::SetAllocUnit()\n");
		YsErrOut("  Alloc unit must be greater than 0\n");
		return YSERR;
	}
}

template <class T>
inline int YsListAllocator <T>::GetAllocUnit(void) const
{
	return nUnit;
}

template <class T>
inline YSRESULT YsListAllocator <T>::CollectGarbage(void)
{
	int c;
	YsListPreallocItem <T> *seek,*next;
	next=preallocHead;
	c=0;
	while((seek=next)!=NULL)
	{
		next=seek->next;
		if(seek->nUsed==0)
		{
			if(seek->next!=NULL)
			{
				seek->next->prev=seek->prev;
			}
			if(seek->prev!=NULL)
			{
				seek->prev->next=seek->next;
			}
			if(seek==preallocHead)
			{
				preallocHead=seek->next;
			}
			if(seek==preallocTail)
			{
				preallocTail=seek->prev;
			}

			int i;
			for(i=0; i<seek->nBuffer; i++)
			{
				if(seek->buffer[i].next!=NULL)
				{
					seek->buffer[i].next->prev=seek->buffer[i].prev;
				}
				if(seek->buffer[i].prev!=NULL)
				{
					seek->buffer[i].prev->next=seek->buffer[i].next;
				}
				if(availableHead==&seek->buffer[i])
				{
					availableHead=seek->buffer[i].next;
				}
				if(availableTail==&seek->buffer[i])
				{
					availableTail=seek->buffer[i].prev;
				}
			}

			delete seek;
		}
		c++;
	}
	return YSOK;
}

template <class T>
inline YSRESULT YsListAllocator <T>::SaveUsageMap(const char fn[]) const
{
	FILE *fp;
	fp=fopen(fn,"w");
	if(fp!=NULL)
	{
		YsListPreallocItem <T> *seeker;
		for(seeker=preallocHead; seeker!=NULL; seeker=seeker->next)
		{
			int nUsed;
			int blk,prevBlk;  // 0 to 99
			int i;
			blk=0;
			prevBlk=0;
			nUsed=0;
			for(i=0; i<seeker->nBuffer; i++)
			{
				blk=i*80/seeker->nBuffer;
				if(blk!=prevBlk)
				{
					if(nUsed<10)
					{
						fprintf(fp,"%d",nUsed);
					}
					else if(nUsed-10<='Z'-'A')
					{
						fprintf(fp,"%c",'A'+(nUsed-10));
					}
					else
					{
						fprintf(fp,"*");
					}
					nUsed=0;
				}

				if(seeker->buffer[i].belongTo!=NULL)
				{
					nUsed++;
				}

				prevBlk=blk;
			}
			fprintf(fp,"\n");
		}
		fclose(fp);
		return YSOK;
	}
	return YSERR;
}

////////////////////////////////////////////////////////////

/*! A linked list class.

    A YsListContainer needs to be associated with a YsListAllocator.  Multiple
    YsListContainer can be associated with a single YsListAllocator.  A
    reference to YsListAllocator must be given to the constructor of a
    YsListContainer.

    YsListContainer is not thread safe.  If multiple threads creates
    or deletes items in the list, the YsListContainer object may lose
    its integrity.  An external mutex-lock must be used if multiple
    threads may create or delete items in the same YsListContainer.

    Also, RewindPointer, StepPointer, and StepBackPointer function
    uses an internally stored pointer, and must not be used if multiple
    threads accesses a single YsListContainer.

    \tparam T Type of objects stored in the linked list.
 */
template <class T>
class YsListContainer
{
private:
	int nItem;

	mutable int nCached;
	mutable YsListItem <T> **cache;

	YsListItem <T> *head,*tail;
	YsListAllocator <T> *allocator;

	mutable YsListItem <T> *fwdSeeker,*bckSeeker;

// STL-like iterator begin >>
public:
	class iterator
	{
	public:
		YsListContainer <T> *listContainer;
		YsListItem <T> *prev,*ptr,*next;
		int dir;
	private:
		inline void Forward(void)
		{
			if(0<=dir)
			{
				prev=ptr;
				ptr=next;
				next=listContainer->FindNext(ptr);
			}
			else
			{
				next=ptr;
				ptr=prev;
				prev=listContainer->FindPrev(ptr);
			}
		}
		inline void Backward(void)
		{
			if(0<=dir)
			{
				next=ptr;
				ptr=prev;
				prev=listContainer->FindPrev(ptr);
			}
			else
			{
				prev=ptr;
				ptr=next;
				next=listContainer->FindNext(ptr);
			}
		}
	public:
		inline iterator &operator++()
		{
			Forward();
			return *this;
		}
		inline iterator operator++(int)
		{
			iterator copy=*this;
			Forward();
			return copy;
		}
		inline iterator &operator--()
		{
			Backward();
			return *this;
		}
		inline iterator operator--(int)
		{
			iterator copy=*this;
			Backward();
			return copy;
		}
		inline bool operator==(const iterator &from)
		{
			return (listContainer==from.listContainer && ptr==from.ptr);
		}
		inline bool operator!=(const iterator &from)
		{
			return (listContainer!=from.listContainer || ptr!=from.ptr);
		}
		inline T &operator*()
		{
			return ptr->dat;
		}
	};

	class const_iterator
	{
	public:
		const YsListContainer <T> *listContainer;
		const YsListItem <T> *prev,*ptr,*next;
		int dir;
	private:
		inline void Forward(void)
		{
			if(0<=dir)
			{
				prev=ptr;
				ptr=next;
				next=listContainer->FindNext(ptr);
			}
			else
			{
				next=ptr;
				ptr=prev;
				prev=listContainer->FindPrev(ptr);
			}
		}
		inline void Backward(void)
		{
			if(0<=dir)
			{
				next=ptr;
				ptr=prev;
				prev=listContainer->FindPrev(ptr);
			}
			else
			{
				prev=ptr;
				ptr=next;
				next=listContainer->FindNext(ptr);
			}
		}
	public:
		inline const_iterator &operator++()
		{
			Forward();
			return *this;
		}
		inline const_iterator operator++(int)
		{
			iterator copy=*this;
			Forward();
			return copy;
		}
		inline const_iterator &operator--()
		{
			Backward();
			return *this;
		}
		inline const_iterator operator--(int)
		{
			iterator copy=*this;
			Backward();
			return copy;
		}
		inline bool operator==(const const_iterator &from)
		{
			return (listContainer==from.listContainer && ptr==from.ptr);
		}
		inline bool operator!=(const const_iterator &from)
		{
			return (listContainer!=from.listContainer || ptr!=from.ptr);
		}
		inline const T &operator*()
		{
			return ptr->dat;
		}
	};

	/*! Support for STL-like iterator */
	iterator begin()
	{
		iterator iter;
		iter.listContainer=this;
		iter.prev=NULL;
		iter.ptr=this->FindNext(NULL);
		iter.next=this->FindNext(iter.ptr);
		iter.dir=1;
		return iter;
	}

	/*! Support for STL-like iterator */
	iterator end()
	{
		iterator iter;
		iter.listContainer=this;
		iter.prev=NULL;
		iter.ptr=NULL;
		iter.next=NULL;
		iter.dir=1;
		return iter;
	}

	/*! Support for STL-like iterator */
	const_iterator begin() const
	{
		const_iterator iter;
		iter.listContainer=this;
		iter.prev=NULL;
		iter.ptr=this->FindNext(NULL);
		iter.next=this->FindNext(iter.ptr);
		iter.dir=1;
		return iter;
	}

	/*! Support for STL-like iterator */
	const_iterator end() const
	{
		const_iterator iter;
		iter.listContainer=this;
		iter.prev=NULL;
		iter.ptr=NULL;
		iter.next=NULL;
		iter.dir=1;
		return iter;
	}

	/*! Support for STL-like iterator */
	iterator rbegin()
	{
		iterator iter;
		iter.listContainer=this;
		iter.next=NULL;
		iter.ptr=this->FindPrev(NULL);
		iter.prev=this->FindPrev(iter.ptr);
		iter.dir=-1;
		return iter;
	}

	/*! Support for STL-like iterator */
	iterator rend()
	{
		iterator iter;
		iter.listContainer=this;
		iter.prev=NULL;
		iter.ptr=NULL;
		iter.next=NULL;
		iter.dir=-1;
		return iter;
	}

	/*! Support for STL-like iterator */
	const_iterator rbegin() const
	{
		const_iterator iter;
		iter.listContainer=this;
		iter.next=NULL;
		iter.ptr=this->FindPrev(NULL);
		iter.prev=this->FindPrev(iter.ptr);
		iter.dir=-1;
		return iter;
	}

	/*! Support for STL-like iterator */
	const_iterator rend() const
	{
		const_iterator iter;
		iter.listContainer=this;
		iter.prev=NULL;
		iter.ptr=NULL;
		iter.next=NULL;
		iter.dir=-1;
		return iter;
	}

	/*! Index enumerator enables range-based for using an index.  It can be written as:
	    YsArray <T> array;
	    for(auto idx : array.AllIndex())
	    {
	        // Do something for array[idx]
	    }
	    Useful when something needs to be done before and after an array item. */
	class PointerEnumerator
	{
	public:
		YsListContainer <T> *listContainer;

		class iterator
		{
		public:
			YsListContainer <T> *listContainer;
			YsListItem <T> *prev,*ptr,*next;
			int dir;
		private:
			inline void Forward(void)
			{
				if(0<=dir)
				{
					prev=ptr;
					ptr=next;
					next=listContainer->FindNext(ptr);
				}
				else
				{
					next=ptr;
					ptr=prev;
					prev=listContainer->FindPrev(ptr);
				}
			}
			inline void Backward(void)
			{
				if(0<=dir)
				{
					next=ptr;
					ptr=prev;
					prev=listContainer->FindPrev(ptr);
				}
				else
				{
					prev=ptr;
					ptr=next;
					next=listContainer->FindNext(ptr);
				}
			}
		public:
			inline iterator &operator++()
			{
				Forward();
				return *this;
			}
			inline iterator operator++(int)
			{
				iterator copy=*this;
				Forward();
				return copy;
			}
			inline iterator &operator--()
			{
				Backward();
				return *this;
			}
			inline iterator operator--(int)
			{
				iterator copy=*this;
				Backward();
				return copy;
			}
			inline bool operator==(const iterator &from)
			{
				return (listContainer==from.listContainer && ptr==from.ptr);
			}
			inline bool operator!=(const iterator &from)
			{
				return (listContainer!=from.listContainer || ptr!=from.ptr);
			}
			inline YsListItem <T> * operator*()
			{
				return ptr;
			}
		};

		iterator begin()
		{
			iterator iter;
			iter.listContainer=listContainer;
			iter.prev=NULL;
			iter.ptr=listContainer->FindNext(NULL);
			iter.next=listContainer->FindNext(iter.ptr);
			iter.dir=1;
			return iter;
		}
		iterator end()
		{
			iterator iter;
			iter.listContainer=listContainer;
			iter.prev=NULL;
			iter.ptr=NULL;
			iter.next=NULL;
			iter.dir=1;
			return iter;
		}
		iterator rbegin()
		{
			iterator iter;
			iter.listContainer=listContainer;
			iter.next=NULL;
			iter.ptr=listContainer->FindPrev(NULL);
			iter.prev=listContainer->FindPrev(iter.ptr);
			iter.dir=-1;
			return iter;
		}
		iterator rend()
		{
			iterator iter;
			iter.listContainer=listContainer;
			iter.prev=NULL;
			iter.ptr=NULL;
			iter.next=NULL;
			iter.dir=-1;
			return iter;
		}
	};
	class Const_PointerEnumerator
	{
	public:
		const YsListContainer <T> *listContainer;

		class iterator
		{
		public:
			const YsListContainer <T> *listContainer;
			YsListItem <T> *prev,*ptr,*next;
			int dir;
		private:
			inline void Forward(void)
			{
				if(0<=dir)
				{
					prev=ptr;
					ptr=next;
					next=listContainer->FindNext(ptr);
				}
				else
				{
					next=ptr;
					ptr=prev;
					prev=listContainer->FindPrev(ptr);
				}
			}
			inline void Backward(void)
			{
				if(0<=dir)
				{
					next=ptr;
					ptr=prev;
					prev=listContainer->FindPrev(ptr);
				}
				else
				{
					prev=ptr;
					ptr=next;
					next=listContainer->FindNext(ptr);
				}
			}
		public:
			inline iterator &operator++()
			{
				Forward();
				return *this;
			}
			inline iterator operator++(int)
			{
				iterator copy=*this;
				Forward();
				return copy;
			}
			inline iterator &operator--()
			{
				Backward();
				return *this;
			}
			inline iterator operator--(int)
			{
				iterator copy=*this;
				Backward();
				return copy;
			}
			inline bool operator==(const iterator &from)
			{
				return (listContainer==from.listContainer && ptr==from.ptr);
			}
			inline bool operator!=(const iterator &from)
			{
				return (listContainer!=from.listContainer || ptr!=from.ptr);
			}
			inline YsListItem <T> * operator*()
			{
				return ptr;
			}
		};

		iterator begin()
		{
			iterator iter;
			iter.listContainer=listContainer;
			iter.prev=NULL;
			iter.ptr=listContainer->FindNext(NULL);
			iter.next=listContainer->FindNext(iter.ptr);
			iter.dir=1;
			return iter;
		}
		iterator end()
		{
			iterator iter;
			iter.listContainer=listContainer;
			iter.prev=NULL;
			iter.ptr=NULL;
			iter.next=NULL;
			iter.dir=1;
			return iter;
		}
		iterator rbegin()
		{
			iterator iter;
			iter.listContainer=listContainer;
			iter.next=NULL;
			iter.ptr=listContainer->FindPrev(NULL);
			iter.prev=listContainer->FindPrev(iter.ptr);
			iter.dir=-1;
			return iter;
		}
		iterator rend()
		{
			iterator iter;
			iter.listContainer=listContainer;
			iter.prev=NULL;
			iter.ptr=NULL;
			iter.next=NULL;
			iter.dir=-1;
			return iter;
		}
	};

	inline PointerEnumerator AllPointer(void);
	inline Const_PointerEnumerator AllPointer(void) const;
// STL-like iterator end >>


public:
	/*! Default constructor */
	inline YsListContainer(YsListAllocator <T> &alloc);
	/*! Default destructor */
	inline ~YsListContainer();
	/*! This function checks for the integrity of the object and prints 
	    diagnostic message to the console. */
	inline void SelfDiagnostic(void) const;
	/*! Returns number of items stored in the list. */
	inline int GetN(void) const;
	/*! Returns number of cached items, which can be accessed by a 0-based index quickly.  
	    Also a 0-based index can be obtained quickly from a pointer to a cached item. */
	inline int GetNumCached(void) const;
	/*! Returns a 0-based index of the item from an item pointer. */
	inline int GetIdFromItem(const YsListItem <T> *ptr) const;
	/*! Returns an item pointer from a 0-based index. */
	inline YsListItem <T> *GetItemFromId(int idx) const;
	/*! Creates a new item and returns a pointer. */
	inline YsListItem <T> *Create(void);
	/*! Deletes an item. 
	    \param ptr [In] Pointer to the item to be deleted. */
	inline YSRESULT Delete(YsListItem <T> *ptr);
	/*! Deletes all items in the list. */
	inline YSRESULT CleanUp(void);
	/*! Transfers an item in this list to another list.  The destination list
	    does NOT have to be associated with the same YsListAllocator as this list. */
	inline YSRESULT Transfer(YsListItem <T> *ptr,YsListContainer <T> &dst);
	/*! Moves the item to the top of the list. 
	    \param ptr [In] Pointer to the item to be moved. */
	inline YSRESULT MoveItemToTop(YsListItem <T> *ptr);
	/*! Moves the item to the end of the list. 
	    \param ptr [In] Pointer to the item to be moved. */
	inline YSRESULT MoveItemToEnd(YsListItem <T> *ptr);
	/*! Moves the item to the next of the given item. 
	    \param ptr [In] Pointer to the item to be moved. 
	    \tparam afterThis [In] The item pointed by ptr will be moved to after this item. */
	inline YSRESULT MoveItemAfter(YsListItem <T> *ptr,YsListItem <T> *afterThis);
	/*! Moves the item to the previous of the given item. 
	    \param ptr [In] Pointer to the item to be moved.
	    \tparam afterThis [In] The item pointed by ptr will be moved to before this item. */
	inline YSRESULT MoveItemBefore(YsListItem <T> *ptr,YsListItem <T> *beforeThis);
	/*! Changes the order of the items in the list. */
	inline YSRESULT Reorder(YSSIZE_T n,YsListItem <T> *order[]);
	/*! Puts all the items in the cache for faster access by a 0-based index. */
	inline YSRESULT Encache(void) const;
	/*! Clears the cache. */
	inline YSRESULT Decache(void) const;
	/*! Returns a pointer to the next list item.  If ptr is the last item in the list,
	    this function returns NULL.  If ptr is NULL, this function returns the first 
	    item in the list or NULL if the list is empty. */
	inline YsListItem <T> *FindNext(const YsListItem <T> *ptr) const;
	/*! Returns a pointer to the previous list item.  If ptr is the first item in the list,
	    this function returns NULL.  If ptr is NULL, this function returns the last  
	    item in the list or NULL if the list is empty. */
	inline YsListItem <T> *FindPrev(const YsListItem <T> *ptr) const;
	/*! Returns the first item in the list.  Same as FindNext(NULL) */
	inline YsListItem <T> *SeekTop(void) const;
	/*! Returns the last item in the list.  Same as FindPrev(NULL) */
	inline YsListItem <T> *SeekEnd(void) const;

	/*! Resets the internal pointer of the list item and prepare for StepPointer
	    and StepBackPointer functions.  RewindPointer, StepPointer, and 
	    StepBackPointer can iterate through the items in the list.

	    FindNext and FindPrev function also can iterate through the items in the list.
	    However, if the current item is deleted, FindNext and FindPrev cannot find the
	    next item.

	    The internal pointer is updated when an item is deleted in such a way
	    that StepPointer and StepBackPointer do not lose the current position.
	    Therefore, it is safe to delete an item inside a loop using StepPointer and
	    StepBackPointer.

	    Since one list has only one internal pointer, RewindPointer, StepPointer, and
	    StepBackPointer are not thread safe.
	     */
	inline void RewindPointer(void) const;
	/*! Move the internal pointer forward and returns the pointer.  If this function
	    is called immediately after RewindPointer, it returns the first item. */
	inline YsListItem <T> *StepPointer(void) const;
	/*! Move the internal pointer backward and returns the pointer.  If this function
	    is called immediately after RewindPointer, it returns the last item. */
	inline YsListItem <T> *StepBackPointer(void) const;

	template <int N>
	inline const YsArray <T,N> &MakeArray(YsArray <T,N> &ary) const;
	template <int N>
	inline const YsArray <T *,N> &MakePointerArray(YsArray <T *,N> &ary) const;
	template <int N>
	inline const YsArray <const T *,N> &MakeConstPointerArray(YsArray <const T *,N> &ary) const;
	template <int N>
	inline const YsArray <YsListItem <T> *,N> &MakeListItemArray(YsArray <YsListItem <T> *,N> &ary) const;
	template <int N>
	inline const YsArray <const YsListItem <T> *,N> &MakeConstListItemArray(YsArray <const YsListItem <T> *,N> &ary) const;
};


// STL-like iterator begin >>
/*! Support for STL-like iterator */
template <class T>
inline typename YsListContainer <T>::iterator begin(YsListContainer <T> &listContainer)
{
	return listContainer.begin();
}

/*! Support for STL-like iterator */
template <class T>
inline typename YsListContainer <T>::iterator end(YsListContainer <T> &listContainer)
{
	return listContainer.end();
}

/*! Support for STL-like iterator */
template <class T>
inline typename YsListContainer <T>::const_iterator begin(const YsListContainer <T> &listContainer)
{
	return listContainer.begin();
}

/*! Support for STL-like iterator */
template <class T>
inline typename YsListContainer <T>::const_iterator end(const YsListContainer <T> &listContainer)
{
	return listContainer.end();
}

/*! Support for STL-like iterator */
template <class T>
inline typename YsListContainer <T>::iterator rbegin(YsListContainer <T> &listContainer)
{
	return listContainer.rbegin();
}

/*! Support for STL-like iterator */
template <class T>
inline typename YsListContainer <T>::iterator rend(YsListContainer <T> &listContainer)
{
	return listContainer.rend();
}

/*! Support for STL-like iterator */
template <class T>
inline typename YsListContainer <T>::const_iterator rbegin(const YsListContainer <T> &listContainer)
{
	return listContainer.rbegin();
}

/*! Support for STL-like iterator */
template <class T>
inline typename YsListContainer <T>::const_iterator rend(const YsListContainer <T> &listContainer)
{
	return listContainer.rend();
}

template <class T>
inline typename YsListContainer <T>::PointerEnumerator YsListContainer <T>::AllPointer(void)
{
	YsListContainer <T>::PointerEnumerator allPointer;
	allPointer.listContainer=this;
	return allPointer;
}

template <class T>
inline typename YsListContainer <T>::Const_PointerEnumerator YsListContainer <T>::AllPointer(void) const
{
	YsListContainer <T>::Const_PointerEnumerator allPointer;
	allPointer.listContainer=this;
	return allPointer;
}

template <class T>
inline typename YsListContainer <T>::PointerEnumerator::iterator begin(typename YsListContainer <T>::PointerEnumerator allPointer)
{
	return allPointer.begin();
}

template <class T>
inline typename YsListContainer <T>::Const_PointerEnumerator::iterator begin(typename YsListContainer <T>::Const_PointerEnumerator allPointer)
{
	return allPointer.begin();
}

template <class T>
inline typename YsListContainer <T>::PointerEnumerator::iterator rbegin(typename YsListContainer <T>::PointerEnumerator allPointer)
{
	return allPointer.rbegin();
}

template <class T>
inline typename YsListContainer <T>::Const_PointerEnumerator::iterator rbegin(typename YsListContainer <T>::Const_PointerEnumerator allPointer)
{
	return allPointer.rbegin();
}

template <class T>
inline typename YsListContainer <T>::PointerEnumerator::iterator end(typename YsListContainer <T>::PointerEnumerator allPointer)
{
	return allPointer.end();
}

template <class T>
inline typename YsListContainer <T>::Const_PointerEnumerator::iterator end(typename YsListContainer <T>::Const_PointerEnumerator allPointer)
{
	return allPointer.end();
}

template <class T>
inline typename YsListContainer <T>::PointerEnumerator::iterator rend(typename YsListContainer <T>::PointerEnumerator allPointer)
{
	return allPointer.rend();
}

template <class T>
inline typename YsListContainer <T>::Const_PointerEnumerator::iterator rend(typename YsListContainer <T>::Const_PointerEnumerator allPointer)
{
	return allPointer.rend();
}

// STL-like iterator end >>


template <class T>
inline YsListContainer <T>::YsListContainer(YsListAllocator <T> &alloc)
{
	head=NULL;
	tail=NULL;
	fwdSeeker=NULL;
	bckSeeker=NULL;

	nItem=0;
	nCached=0;
	cache=NULL;

	allocator=&alloc;
}

template <class T>
inline YsListContainer <T>::~YsListContainer()
{
	YsListItem <T> *seeker,*next;
	seeker=head;
	while(seeker!=NULL)
	{
		next=seeker->next;
		Delete(seeker);
		seeker=next;
	}
	if(cache!=NULL)
	{
		delete [] cache;
		cache=NULL;
	}
}

template <class T>
inline void YsListContainer <T>::SelfDiagnostic(void) const
{
	int n;
	YsListItem <T> *seeker;

	YsPrintf("YsListContainer::SelfDiagnostic()\n");
	YsPrintf("  nItem=%d\n",nItem);
	YsPrintf("  nCached=%d\n",nCached);

	n=0;
	for(seeker=head; seeker!=NULL; seeker=seeker->next)
	{
		if(seeker->serial<nCached && cache[n]!=seeker)
		{
			YsPrintf("Cache Mismatch! [%d]\n",n);
		}
		if(seeker->belongTo!=this)
		{
			YsPrintf("Item [%d] does not belong to this list!\n",n);
		}
		n++;
		if(seeker->next==NULL && seeker!=tail)
		{
			YsPrintf("Tail item is incorrectly recorded!\n");
		}
	}

	YsPrintf("  Number of item counted=%d\n",n);
	if(n!=nItem)
	{
		YsPrintf("  nItem and the number of item do not match!\n");
	}
}

template <class T>
inline int YsListContainer <T>::GetN(void) const
{
	return nItem;
}

template <class T>
inline int YsListContainer <T>::GetNumCached(void) const
{
	return nCached;
}

template <class T>
inline int YsListContainer <T>::GetIdFromItem(const YsListItem <T> *ptr) const
{
	if(ptr->belongTo==this)
	{
		if(ptr->serial<nCached)
		{
			return ptr->serial;
		}
		else
		{
			int id;
			YsListItem <T> *seek;
			id=0;
			for(seek=head; seek!=ptr && seek!=NULL; seek=seek->next)
			{
				id++;
			}
			return id;
		}
	}
	else
	{
		YsErrOut("YsListContainer::GetId()\n");
		YsErrOut("  Tried to access a list item of different list.\n");
		return -1;
	}
}

template <class T>
inline YsListItem <T> *YsListContainer <T>::GetItemFromId(int id) const
{
	if(0<=id && id<nItem)
	{
		if(id<nCached)
		{
			return cache[id];
		}
		else
		{
			int n;
			YsListItem <T> *seek;
			if(nCached>0)
			{
				n=nCached;
				seek=cache[nCached-1]->next;
			}
			else
			{
				n=0;
				seek=head;
			}
			while(n<id && seek!=NULL)
			{
				n++;
				seek=seek->next;
			}
			return seek;
		}
	}
	else
	{
		YsErrOut("YsListContainer::GetItemFromId()\n");
		YsErrOut("  Index out of range.\n");
		return NULL;
	}
}

template <class T>
inline YsListItem <T> *YsListContainer <T>::Create(void)
{
	YsListItem <T> *neo;
	neo=allocator->Create();
	if(head!=NULL)
	{
		tail->next=neo;
		neo->prev=tail;
		neo->next=NULL;
		tail=neo;
	}
	else
	{
		head=neo;
		tail=neo;
		neo->prev=NULL;
		neo->next=NULL;
	}
	neo->belongTo=this;
	neo->serial=INT_MAX;
	nItem++;
	return neo;
};

template <class T>
inline YSRESULT YsListContainer <T>::Delete(YsListItem <T> *toDelete)
{
	if(toDelete==NULL)
	{
		YsErrOut("YsListContainer::Delete\n");
		YsErrOut("  NULL pointer is given.\n");
		return YSERR;
	}
	else if(toDelete->belongTo==this)
	{
		if(toDelete==fwdSeeker)
		{
			fwdSeeker=FindNext(fwdSeeker);
		}
		if(toDelete==bckSeeker)
		{
			bckSeeker=FindPrev(bckSeeker);
		}


		if(toDelete->prev!=NULL)
		{
			toDelete->prev->next=toDelete->next;
		}
		if(toDelete->next!=NULL)
		{
			toDelete->next->prev=toDelete->prev;
		}

		if(toDelete->next==NULL)
		{
			tail=toDelete->prev;
		}
		if(toDelete->prev==NULL)
		{
			head=toDelete->next;
		}

		if(toDelete->serial<nCached)  // 2002/01/04 moved from below
		{
			nCached=toDelete->serial;
		}

		toDelete->next=NULL;
		if(toDelete->prealloc->belongTo!=NULL)
		{
			toDelete->prev=toDelete->prealloc->belongTo->availableTail;
			if(toDelete->prealloc->belongTo->availableHead==NULL)
			{
				toDelete->prealloc->belongTo->availableHead=toDelete;
			}
			else
			{
				toDelete->prealloc->belongTo->availableTail->next=toDelete;
			}
			toDelete->prealloc->belongTo->availableTail=toDelete;
			toDelete->prealloc->nUsed--;
			toDelete->belongTo=NULL;
		}
		else
		{
			toDelete->prealloc->nUsed--;
			if(toDelete->prealloc->nUsed==0)
			{
				delete toDelete->prealloc;
			}
		}

		nItem--;

		// 2003/01/04  if(toDelete->serial<nCached) used to be here.
		// It's not a good location, because toDelete may be already deleted at this point.

		return YSOK;
	}
	else
	{
		YsErrOut("YsListContainer::Delete\n");
		YsErrOut("  Attempted to delete an item not belonging to the container.\n");
		return YSERR;
	}
}

template <class T>
inline YSRESULT YsListContainer <T>::CleanUp(void)
{
	YsListItem <T> *itm,*nxt;
	nxt=head;
	while((itm=nxt)!=NULL)
	{
		nxt=itm->next;

		if(itm->prealloc->belongTo!=NULL)
		{
			itm->prev=itm->prealloc->belongTo->availableTail;
			itm->next=NULL;
			if(itm->prealloc->belongTo->availableHead==NULL)
			{
				itm->prealloc->belongTo->availableHead=itm;
			}
			else
			{
				itm->prealloc->belongTo->availableTail->next=itm;
			}
			itm->prealloc->belongTo->availableTail=itm;
			itm->prealloc->nUsed--;
			itm->belongTo=NULL;
		}
		else
		{
			itm->prealloc->nUsed--;
			if(itm->prealloc->nUsed==0)
			{
				delete itm->prealloc;
			}
		}
	}
	nItem=0;
	nCached=0;
	head=NULL;
	tail=NULL;
	fwdSeeker=NULL;
	bckSeeker=NULL;
	if(cache!=NULL)
	{
		delete [] cache;
		cache=NULL;
	}
	return YSOK;
}

template <class T>
inline YSRESULT YsListContainer <T>::Transfer(YsListItem <T> *itm,YsListContainer <T> &dst)
{
	if(itm==NULL)
	{
		YsErrOut("YsListContainer::Transfer\n");
		YsErrOut("  NULL pointer is given.\n");
		return YSERR;
	}
	else if(itm->belongTo==this)
	{
		if(itm==fwdSeeker)
		{
			fwdSeeker=FindNext(fwdSeeker);
		}
		if(itm==bckSeeker)
		{
			bckSeeker=FindPrev(bckSeeker);
		}


		if(itm->prev!=NULL)
		{
			itm->prev->next=itm->next;
		}
		if(itm->next!=NULL)
		{
			itm->next->prev=itm->prev;
		}
		if(tail==itm)
		{
			tail=itm->prev;  // tail->next is already set to NULL by itm->prev->next=itm->next
		}
		if(head==itm)
		{
			head=itm->next;  // head->prev is already set to NULL by itm->next->prev=itm->prev
		}

		nItem--;
		if(itm->serial<nCached)
		{
			nCached=itm->serial;
		}

		if(dst.tail!=NULL)
		{
			dst.tail->next=itm;
			itm->prev=dst.tail;
			dst.tail=itm;
			// dst.nItem++; Why did I write this? Isn't it a bug? 2002/06/27
		}
		else
		{
			dst.head=itm;
			dst.tail=itm;
			itm->prev=NULL;
		}
		itm->next=NULL;
		itm->belongTo=&dst;
		dst.nItem++;

		return YSOK;
	}
	else
	{
		YsErrOut("YsListContainer::Transfer\n");
		YsErrOut("  Attempted to transfer an item not belonging to this container.\n");
		return YSERR;
	}
}

template <class T>
inline YSRESULT YsListContainer <T>::MoveItemToTop(YsListItem <T> *itm)
{
	if(itm->belongTo==this)
	{
		if(itm==head)
		{
			return YSOK;  // Already at the top
		}
		else
		{
			if(itm->prev!=NULL)
			{
				itm->prev->next=itm->next;
			}
			if(itm->next!=NULL)
			{
				itm->next->prev=itm->prev;
			}
			if(tail==itm)
			{
				tail=itm->prev;  // tail->next is already set to NULL by itm->prev->next=itm->next
			}

			nCached=0;

			itm->prev=NULL;
			itm->next=head;
			if(itm->next!=NULL)
			{
				itm->next->prev=itm;
			}
			head=itm;

			return YSOK;
		}
	}
	else
	{
		YsErrOut("YsListContainer::MoveItemToTop\n");
		YsErrOut("  Attempted to access an item that does not belong to this container.\n");
		return YSERR;
	}
}

template <class T>
inline YSRESULT YsListContainer <T>::MoveItemToEnd(YsListItem <T> *itm)
{
	if(itm->belongTo==this)
	{
		if(itm==tail)
		{
			return YSOK;  // Already at the end
		}

		if(itm->prev!=NULL)
		{
			itm->prev->next=itm->next;
		}
		if(itm->next!=NULL)
		{
			itm->next->prev=itm->prev;
		}
		if(head==itm)
		{
			head=itm->next;  // head->prev is already set to NULL by itm->next->prev=itm->prev
		}

		if(itm->serial<nCached)
		{
			nCached=itm->serial;
		}

		itm->prev=tail;
		itm->next=NULL;
		if(itm->prev!=NULL)
		{
			itm->prev->next=itm;
		}
		tail=itm;
		return YSOK;
	}
	else
	{
		YsErrOut("YsListContainer::MoveItemToEnd\n");
		YsErrOut("  Attempted to access an item that does not belong to this container.\n");
		return YSERR;
	}
}

template <class T>
inline YSRESULT YsListContainer <T>::MoveItemAfter(YsListItem <T> *itm,YsListItem <T> *afterThis)
{
	if(afterThis==NULL)
	{
		return MoveItemToTop(itm);
	}
	else
	{
		if(itm!=afterThis && itm->belongTo==this && afterThis->belongTo==this)
		{
			if(itm->prev!=NULL)
			{
				itm->prev->next=itm->next;
			}
			if(itm->next!=NULL)
			{
				itm->next->prev=itm->prev;
			}
			if(tail==itm)
			{
				tail=itm->prev;  // tail->next is already set to NULL by itm->prev->next=itm->next
			}
			if(head==itm)
			{
				head=itm->next;  // head->prev is already set to NULL by itm->next->prev=itm->prev
			}

			if(itm->serial<nCached)
			{
				nCached=itm->serial;
			}
			if(afterThis->serial<nCached)
			{
				nCached=afterThis->serial;
			}

			itm->prev=afterThis;
			itm->next=afterThis->next;
			itm->prev->next=itm;
			if(itm->next!=NULL)
			{
				itm->next->prev=itm;
			}
			else
			{
				tail=itm;
			}
			return YSOK;
		}
		else
		{
			YsErrOut("YsListContainer::MoveItemAfter\n");
			YsErrOut("  Invalid pointer.\n");
			return YSERR;
		}
	}
}

template <class T>
inline YSRESULT YsListContainer <T>::MoveItemBefore(YsListItem <T> *itm,YsListItem <T> *beforeThis)
{
	if(beforeThis==NULL)
	{
		return MoveItemToEnd(itm);
	}
	else
	{
		if(itm!=beforeThis && itm->belongTo==this && beforeThis->belongTo==this)
		{
			if(itm->prev!=NULL)
			{
				itm->prev->next=itm->next;
			}
			if(itm->next!=NULL)
			{
				itm->next->prev=itm->prev;
			}
			if(tail==itm)
			{
				tail=itm->prev;  // tail->next is already set to NULL by itm->prev->next=itm->next
			}
			if(head==itm)
			{
				head=itm->next;  // head->prev is already set to NULL by itm->next->prev=itm->prev
			}

			if(itm->serial<nCached)
			{
				nCached=itm->serial;
			}
			if(beforeThis->serial<nCached)
			{
				nCached=beforeThis->serial-1;
				if(nCached<0)
				{
					nCached=0;
				}
			}

			itm->prev=beforeThis->prev;
			itm->next=beforeThis;
			itm->next->prev=itm;
			if(itm->prev!=NULL)
			{
				itm->prev->next=itm;
			}
			else
			{
				head=itm;
			}

			return YSOK;
		}
		else
		{
			YsErrOut("YsListContainer::MoveItemBefore\n");
			YsErrOut("  Invalid pointer.\n");
			return YSERR;
		}
	}
}

template <class T>
inline YSRESULT YsListContainer <T>::Reorder(YSSIZE_T n,YsListItem <T> *order[])
{
	YSSIZE_T i;
	for(i=0; i<n; i++)
	{
		if(order[i]->belongTo!=this)
		{
			YsErrOut("YsListContainer::Reorder()\n");
			YsErrOut("  Attempted to access an item not belonging to this container.\n");
			return YSERR;
		}
	}

	for(i=n-1; i>=0; i--)
	{
		if(order[i]!=head)
		{
			if(order[i]->prev!=NULL)
			{
				order[i]->prev->next=order[i]->next;
			}
			if(order[i]->next!=NULL)
			{
				order[i]->next->prev=order[i]->prev;
			}
			// if(head==order[i])
			// {
			// 	head=order[i]->next;
			// }
			if(tail==order[i])
			{
				tail=order[i]->prev;
			}

			order[i]->prev=NULL;
			order[i]->next=head;
			head->prev=order[i];
			head=order[i];
		}
	}

	nCached=0;
	if(cache!=NULL)
	{
		delete [] cache;
		cache=NULL;
	}

	return YSOK;
}

template <class T>
inline YSRESULT YsListContainer <T>::Encache(void) const
{
	if(nCached<nItem)
	{
		int i;
		YsListItem <T> *seeker;

		if(cache!=NULL)
		{
			delete [] cache;
			cache=NULL;
		}

		cache=new YsListItem <T> * [nItem];
		if(cache!=NULL)
		{
			i=0;
			for(seeker=head; seeker!=NULL; seeker=seeker->next)
			{
				seeker->serial=i;
				cache[i]=seeker;
				i++;
			}

			nCached=nItem;
		}
		else
		{
			YsExceptionHandler::Exception(YsExceptionHandler::OUTOFMEMORY,"YsListAllocator::Encache()");
		}
	}
	return YSOK;
}

template <class T>
inline YSRESULT YsListContainer <T>::Decache(void) const
{
	nCached=0;
	if(cache!=NULL)
	{
		delete [] cache;
		cache=NULL;
	}
	return YSOK;
}

template <class T>
inline YsListItem <T> *YsListContainer <T>::FindNext(const YsListItem <T> *ptr) const
{
	if(ptr!=NULL)
	{
		return ptr->next;
	}
	else
	{
		return head;
	}
}

template <class T>
inline YsListItem <T> *YsListContainer <T>::FindPrev(const YsListItem <T> *ptr) const
{
	if(ptr!=NULL)
	{
		return ptr->prev;
	}
	else
	{
		return tail;
	}
}

template <class T>
inline YsListItem <T> *YsListContainer <T>::SeekTop(void) const
{
	return head;
}

template <class T>
inline YsListItem <T> *YsListContainer <T>::SeekEnd(void) const
{
	return tail;
}

template <class T>
inline void YsListContainer <T>::RewindPointer(void) const
{
	fwdSeeker=FindNext(NULL);
	bckSeeker=FindPrev(NULL);
}

template <class T>
inline YsListItem <T> *YsListContainer <T>::StepPointer(void) const
{
	YsListItem <T> *ptr;
	ptr=fwdSeeker;
	fwdSeeker=FindNext(fwdSeeker);
	return ptr;
}

template <class T>
inline YsListItem <T> *YsListContainer <T>::StepBackPointer(void) const
{
	YsListItem <T> *ptr;
	ptr=bckSeeker;
	bckSeeker=FindPrev(bckSeeker);
	return ptr;
}

template <class T>
template <int N>
inline const YsArray <T,N> &YsListContainer <T>::MakeArray(YsArray <T,N> &ary) const
{
	ary.Set(GetN(),NULL);

	int i=0;
	YsListItem <T> *ptr=NULL;
	while(NULL!=(ptr=FindNext(ptr)))
	{
		ary[i++]=ptr->dat;
	}

	return ary;
}

template <class T>
template <int N>
inline const YsArray <T *,N> &YsListContainer <T>::MakePointerArray(YsArray <T *,N> &ary) const
{
	ary.Set(GetN(),NULL);

	int i=0;
	YsListItem <T> *ptr=NULL;
	while(NULL!=(ptr=FindNext(ptr)))
	{
		ary[i++]=&ptr->dat;
	}

	return ary;
}


template <class T>
template <int N>
inline const YsArray <const T *,N> &YsListContainer <T>::MakeConstPointerArray(YsArray <const T *,N> &ary) const
{
	ary.Set(GetN(),NULL);

	int i=0;
	YsListItem <T> *ptr=NULL;
	while(NULL!=(ptr=FindNext(ptr)))
	{
		ary[i++]=&ptr->dat;
	}

	return ary;
}


template <class T>
template <int N>
inline const YsArray <YsListItem <T> *,N> &YsListContainer <T>::MakeListItemArray(YsArray <YsListItem <T> *,N> &ary) const
{
	ary.Set(GetN(),NULL);

	int i=0;
	YsListItem <T> *ptr=NULL;
	while(NULL!=(ptr=FindNext(ptr)))
	{
		ary[i++]=ptr;
	}

	return ary;
}

template <class T>
template <int N>
inline const YsArray <const YsListItem <T> *,N> &YsListContainer <T>::MakeConstListItemArray(YsArray <const YsListItem <T> *,N> &ary) const
{
	ary.Set(GetN(),NULL);

	int i=0;
	YsListItem <T> *ptr=NULL;
	while(NULL!=(ptr=FindNext(ptr)))
	{
		ary[i++]=ptr;
	}

	return ary;
}


/* } */
#endif
