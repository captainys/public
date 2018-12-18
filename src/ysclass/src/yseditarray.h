/* ////////////////////////////////////////////////////////////

File Name: yseditarray.h
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

#ifndef YSARRAYFOREDIT_IS_INCLUDED
#define YSARRAYFOREDIT_IS_INCLUDED
/* { */

#include <stdio.h>
#include "yshash.h"
#include "ysarray.h"
#include "yssegarray.h"
#include "ysbdhandleiterator.h"

template <class T,const int bitShift>
class YsEditArray;

typedef int YsEditArray_INDEXTYPE;

/* Memo:
  I really want to make YsEditArrayObjectHandle a local class of YsEditArray.
  However, for some reason, Visual C++ cannot deduce
    int operator==(void *ptr,typename const YsEditArray <T,bitShift>::ObjHandle &hd) and
    int operator!=(void *ptr,typename const YsEditArray <T,bitShift>::ObjHandle &hd).
  Forcing me to declare YsEditArrayObjectHandle outside YsEditArray.  I have also added:
    int operator==(int ptr,typename const YsEditArray <T,bitShift>::ObjHandle &hd) and
    int operator!=(int ptr,typename const YsEditArray <T,bitShift>::ObjHandle &hd),
  which cannot be found by Visual C++, either.  Why?
*/


/*!  This is an object handle for YsEditArray.
*/
template <class T,const int bitShift=8>
class YsEditArrayObjectHandle
{
friend class YsEditArray <T,bitShift>;

private:
	enum
	{
		NULLINDEX=0x7fffffff
	};

	YsEditArray_INDEXTYPE index;

public:
	YsEditArrayObjectHandle();
	YsEditArrayObjectHandle(void *ptr);
	YsEditArrayObjectHandle(const YsEditArrayObjectHandle <T,bitShift> &incoming);

	inline const YsEditArrayObjectHandle &CopyFrom(void *ptr);
	inline const YsEditArrayObjectHandle &CopyFrom(const YsEditArrayObjectHandle <T,bitShift> &incoming);
	inline bool IsEqual(void *ptr) const;
	inline bool IsNotEqual(void *ptr) const;
	inline bool IsEqual(const YsEditArrayObjectHandle <T,bitShift> &incoming) const;
	inline bool IsNotEqual(const YsEditArrayObjectHandle <T,bitShift> &incoming) const;

	inline const YsEditArrayObjectHandle &operator=(void *ptr);
	inline bool operator==(const YsEditArrayObjectHandle <T,bitShift> &incoming) const;
	inline bool operator!=(const YsEditArrayObjectHandle <T,bitShift> &incoming) const;
	inline bool operator==(void *ptr) const;
	inline bool operator!=(void *ptr) const;

	inline bool operator<(const YsEditArrayObjectHandle <T,bitShift> &incoming) const;
	inline bool operator<=(const YsEditArrayObjectHandle <T,bitShift> &incoming) const;
	inline bool operator>(const YsEditArrayObjectHandle <T,bitShift> &incoming) const;
	inline bool operator>=(const YsEditArrayObjectHandle <T,bitShift> &incoming) const;

	/*! This function is purely debugging purpose.  Don't use it in the production code.
	*/
	inline YsEditArray_INDEXTYPE GetRawIndex(void) const
	{
		return index;
	}
	/*! This function is purely debugging purpose.  Don't use it in the production code.
	*/
	void SetRawIndex(YsEditArray_INDEXTYPE i)
	{
		index=i;
	}
};

template <class T,const int bitShift>
YsEditArrayObjectHandle <T,bitShift>::YsEditArrayObjectHandle()
{
	index=NULLINDEX;
}

template <class T,const int bitShift>
YsEditArrayObjectHandle <T,bitShift>::YsEditArrayObjectHandle(void *ptr)
{
	CopyFrom(ptr);
}

template <class T,const int bitShift>
YsEditArrayObjectHandle <T,bitShift>::YsEditArrayObjectHandle(const YsEditArrayObjectHandle <T,bitShift> &incoming)
{
	CopyFrom(incoming);
}

template <class T,const int bitShift>
inline const YsEditArrayObjectHandle <T,bitShift> &YsEditArrayObjectHandle <T,bitShift>::CopyFrom(void *ptr)
{
	if(NULL==ptr)
	{
		this->index=NULLINDEX;
	}
	else
	{
		YsPrintf("%s\n",__FUNCTION__);
		YsPrintf("Only NULL pointer can be directly copied to YsEditArrayObjectHandle\n");
	}
	return *this;
}

template <class T,const int bitShift>
inline const YsEditArrayObjectHandle <T,bitShift> &YsEditArrayObjectHandle <T,bitShift>::CopyFrom(const YsEditArrayObjectHandle <T,bitShift> &incoming)
{
	this->index=incoming.index;
	return *this;
}

template <class T,const int bitShift>
inline bool YsEditArrayObjectHandle <T,bitShift>::IsEqual(void *ptr) const
{
	if(NULL==ptr)
	{
		return NULLINDEX==this->index;
	}
	YsPrintf("%s\n",__FUNCTION__);
	YsPrintf("Only NULL pointer can be directly compared with YsEditArrayObjectHandle\n");
	return 0;
}

template <class T,const int bitShift>
inline bool YsEditArrayObjectHandle <T,bitShift>::IsNotEqual(void *ptr) const
{
	if(NULL==ptr)
	{
		return NULLINDEX!=this->index;
	}
	YsPrintf("%s\n",__FUNCTION__);
	YsPrintf("Only NULL pointer can be directly compared with YsEditArrayObjectHandle\n");
	return 0;
}

template <class T,const int bitShift>
inline bool YsEditArrayObjectHandle <T,bitShift>::IsEqual(const YsEditArrayObjectHandle <T,bitShift> &incoming) const
{
	return this->index==incoming.index;
}

template <class T,const int bitShift>
inline bool YsEditArrayObjectHandle <T,bitShift>::IsNotEqual(const YsEditArrayObjectHandle <T,bitShift> &incoming) const
{
	return this->index!=incoming.index;
}

template <class T,const int bitShift>
inline const YsEditArrayObjectHandle <T,bitShift> &YsEditArrayObjectHandle <T,bitShift>::operator=(void *ptr)
{
	return CopyFrom(ptr);
}

template <class T,const int bitShift>
inline bool YsEditArrayObjectHandle <T,bitShift>::operator==(const YsEditArrayObjectHandle <T,bitShift> &incoming) const
{
	return IsEqual(incoming);
}

template <class T,const int bitShift>
inline bool YsEditArrayObjectHandle <T,bitShift>::operator!=(const YsEditArrayObjectHandle <T,bitShift> &incoming) const
{
	return IsNotEqual(incoming);
}

template <class T,const int bitShift>
inline bool YsEditArrayObjectHandle <T,bitShift>::operator==(void *ptr) const
{
	return IsEqual(ptr);
}

template <class T,const int bitShift>
inline bool YsEditArrayObjectHandle <T,bitShift>::operator!=(void *ptr) const
{
	return IsNotEqual(ptr);
}

template <class T,const int bitShift>
inline bool operator==(void *ptr,const YsEditArrayObjectHandle <T,bitShift> &obHd)
{
	return obHd.IsEqual(ptr);
}

template <class T,const int bitShift>
inline bool operator!=(void *ptr,const YsEditArrayObjectHandle <T,bitShift> &obHd)
{
	return obHd.IsNotEqual(ptr);
}

template <class T,const int bitShift>
inline bool YsEditArrayObjectHandle <T,bitShift>::operator<(const YsEditArrayObjectHandle <T,bitShift> &incoming) const
{
	return this->index<incoming.index;
}

template <class T,const int bitShift>
inline bool YsEditArrayObjectHandle <T,bitShift>::operator<=(const YsEditArrayObjectHandle <T,bitShift> &incoming) const
{
	return this->index<=incoming.index;
}

template <class T,const int bitShift>
inline bool YsEditArrayObjectHandle <T,bitShift>::operator>(const YsEditArrayObjectHandle <T,bitShift> &incoming) const
{
	return this->index>incoming.index;
}

template <class T,const int bitShift>
inline bool YsEditArrayObjectHandle <T,bitShift>::operator>=(const YsEditArrayObjectHandle <T,bitShift> &incoming) const
{
	return this->index>=incoming.index;
}

////////////////////////////////////////////////////////////

// gcc cannot match begin(YsEditArray<T,bitShift>::HandleEnumerator. 
//    I am forced to contaminate the name space by adding YsEditArrayHandleEnumerator.
//    Am I missing anything?

// Never mind.  I templatized.  The class:
//   template <class T,const int bitShift>
//   class YsEditArrayHandleEnumerator;
// is now replaced with:
//   YsBidirectionalHandleEnumerator <YsEditArray<T,bitShift>,YsEditArrayObjectHandle<T,bitShift> >.

////////////////////////////////////////////////////////////

/*!  This is a template array class for editing.  Uses YsSegmentedArray as a background data structure.
*/
template <class T,const int bitShift=8>
class YsEditArray
{
friend class HandleStore;

public:
	/*! The object in the YsEditArray can be one of ALIVE, DELETED, or FROZEN.
	*/
	enum
	{
		ALIVE=0,
		DELETED=1,
		FROZEN=2
	};

	mutable bool trapMoveToNext;

private:
	// Actually I want to add Lock/Unlock

	YSHASHKEY localSearchKeySeed;
	YSHASHKEY *searchKeySeed;
	class ArrayObject
	{
	public:
		unsigned int attrib;  // bit0-1 STATE(ALIVE,DELETED,FROZEN)  bit 2-31 search key
		mutable YsEditArray_INDEXTYPE cachedIndex;
		T dat;

		inline void SetSearchKey(unsigned int key)
		{
			attrib&=3;
			attrib|=(key<<2);
		}
		inline YSHASHKEY GetSearchKey(void) const
		{
			return (attrib>>2);
		}
		inline void SetState(unsigned int state)
		{
			attrib&=~(unsigned int)3;
			attrib|=(state&3);
		}
		inline unsigned int GetState(void) const
		{
			return (attrib&3);
		}
	};

	YsSegmentedArray <ArrayObject,bitShift> objArray;
	YsArray <YsEditArray_INDEXTYPE> deletedIndex; // <- Ready for recycling
	YsEditArray_INDEXTYPE nFrozen;

	YSBOOL searchEnabled;
	YsHashTable <YsEditArray_INDEXTYPE> searchKeyToIndex;

	mutable YsEditArray_INDEXTYPE maxCachedIndex;
	mutable YsArray <YsEditArrayObjectHandle <T,bitShift> > indexToHandleCache;

	static YsEditArrayObjectHandle <T,bitShift> MakeHandleFromRawIndex(YsEditArray_INDEXTYPE index)
	{
		YsEditArrayObjectHandle<T,bitShift> obHd;
		obHd.index=index;
		return obHd;
	}
	static YsEditArray_INDEXTYPE GetRawIndexFromHandle(YsEditArrayObjectHandle <T,bitShift> obHd)
	{
		return obHd.index;
	}

public:
	typedef YsBidirectionalHandleIterator<YsEditArray<T,bitShift>,YsEditArrayObjectHandle<T,bitShift>,T> iterator;
	typedef YsBidirectionalHandleIterator<const YsEditArray<T,bitShift>,YsEditArrayObjectHandle<T,bitShift>,const T> const_iterator;

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
	iterator rbegin()
	{
		iterator iter(this);
		return iter.rbegin();
	}
	iterator rend()
	{
		iterator iter(this);
		return iter.rend();
	}
	const_iterator rbegin() const
	{
		const_iterator iter(this);
		return iter.rbegin();
	}
	const_iterator rend() const
	{
		const_iterator iter(this);
		return iter.rend();
	}



public:
	/*! Default constructor
	*/
	YsEditArray();

	/*! Clears the array. */
	void CleanUp(void);

	/*! Clears the array.  It will free all allocated elements.
	    CleanUp function holds deleted elements for recycling, but this function completely deletes the elements.
	 */
	void ClearDeep(void);

	/*! Copy constructor.  Copies element by element. */
	YsEditArray(const YsEditArray <T,bitShift> &incoming);

	/*! Copy assignment operator.  Copies element by element. */
	YsEditArray <T,bitShift> &operator=(const YsEditArray <T,bitShift> &incoming);

	/*! Copies an array. */
	void CopyFrom(const YsEditArray <T,bitShift> &incoming);

	/*! Moves an array. */
	void MoveFrom(YsEditArray<T,bitShift> &incoming);

	/*! Make this object to use a shared external search-key seed.
	*/
	void UseSharedSearchKeySeed(YSHASHKEY &seedPtr);

	/*! Returns the next search key that will be assigned to a new object to be created by the Create() function.
	*/
	YSHASHKEY GetNextSearchKey(void) const;

	/*! Enable search table.  When the search is enabled, an array object can be
	    seached by FindObject or FindFrozenObject function.
	*/
	void EnableSearch(void);

	/*! Disable search table.
	*/
	void DisableSearch(void);

	/*! Returns if search is enabled.
	*/
	YSBOOL SearchEnabled(void) const;

	/*! Returns number of objects ALIVE in the array.
	*/
	YSSIZE_T GetN(void) const;

	/*! Returns YSTRUE if the object is frozen (or tentatively deleted), or YSFALSE otherwise. */
	YSBOOL IsFrozen(YsEditArrayObjectHandle<T,bitShift> hd) const;

	/*! Create a new object and returns the handle.
	*/
	YsEditArrayObjectHandle<T,bitShift> Create(void);

	/*! Access the object. */
	T *operator[] (YsEditArrayObjectHandle<T,bitShift> hd);

	/*! Access the object. */
	const T *operator[] (YsEditArrayObjectHandle<T,bitShift> hd) const;

	/*! Access the object. */
	T &Value(YsEditArrayObjectHandle<T,bitShift> hd);

	/*! Access the object. */
	const T &Value(YsEditArrayObjectHandle<T,bitShift> hd) const;

	/*! Access the object that is tentatively deleted. */
	T *GetFrozenObject(YsEditArrayObjectHandle<T,bitShift> hd);

	/*! Access the object that is tentatively deleted. */
	const T *GetFrozenObject(YsEditArrayObjectHandle<T,bitShift> hd) const;

	/*! Access the object that can be alive or can be tentatively deleted. */
	const T *GetObjectMaybeFrozen(YsEditArrayObjectHandle<T,bitShift> hd) const;

	/*! Access the object that can be alive or can be tentatively deleted. */
	T *GetObjectMaybeFrozen(YsEditArrayObjectHandle<T,bitShift> hd);

	/*! Appends an object.  T must be safe for the copy operator. */
	YsEditArrayObjectHandle<T,bitShift> Append(const T &incoming);

	/*! Delete an object.  The state of the object pointed by the handle must be ALIVE.
	*/
	YSRESULT Delete(YsEditArrayObjectHandle<T,bitShift> hd);

	/*! Freeze an object.  Frozen object is temporarily deleted and can later be melted (or undeleted.)
	*/
	YSRESULT Freeze(YsEditArrayObjectHandle<T,bitShift> hd);

	/*! Melt (undelete) an object that was temporarily deleted by Freeze function.
	*/
	YSRESULT Melt(YsEditArrayObjectHandle<T,bitShift> hd);

	/*! Delete a frozen object.  The state of the object pointed by the handle must be FROZEN.
	*/
	YSRESULT DeleteFrozen(YsEditArrayObjectHandle<T,bitShift> hd);

	/*! Find an object from a search key.  It can find an object that is ALIVE.
	*/
	YsEditArrayObjectHandle<T,bitShift> FindObject(unsigned int key) const;

	/*! Find a frozen object from a search key.
	*/
	YsEditArrayObjectHandle<T,bitShift> FindFrozenObject(unsigned int key) const;

	/*! Find an object from a search key regardless of whether the object is ALIVE or FROZEN.
	*/
	YsEditArrayObjectHandle<T,bitShift> FindObjectMaybeFrozen(unsigned int key) const;


	/*! Find next object of obHd that is ALIVE.  The object pointed by obHd does not have to be ALIVE.
	    If obHd==NULL, this function finds the next first object ALIVE and returns the handle to the next object.
	*/
	YsEditArrayObjectHandle <T,bitShift> FindNext(const YsEditArrayObjectHandle <T,bitShift> obHd) const;

	/*! Find previous object obHd that is ALIVE.  The object pointed by obHd does not have to be ALIVE.
	    If obHd==NULL, this function finds the last object ALIVE and returns the handle to the previous object.
	*/
	YsEditArrayObjectHandle <T,bitShift> FindPrev(const YsEditArrayObjectHandle <T,bitShift> obHd) const;

	/*! Returns the first object handle.  Same as FindNext(NULL). */
	YsEditArrayObjectHandle <T,bitShift> First(void) const;

	/*! Returns the last object handle.  Same as FindPrev(NULL). */
	YsEditArrayObjectHandle <T,bitShift> Last(void) const;

	/*! Returns a NULL handle. */
	YsEditArrayObjectHandle <T,bitShift> Null(void) const;

	/*! Move obHd to the next object that is ALIVE.  The object pointed by obHd does not have to be ALIVE.
	    If obHd==NULL, this function finds the next first object ALIVE.
	    If the next of obHd exists, this function returns YSOK.
	    If the next of obHd does not exist, this function returns YSERR, and obHd will be NULL.
	*/
	YSRESULT MoveToNext(YsEditArrayObjectHandle <T,bitShift> &obHd) const;

	/*! Move obHd to the previous object that is ALIVE.  The object pointed by obHd does not have to be ALIVE.
	    If obHd==NULL, this function finds the last object ALIVE.
	    If the previous of obHd exists, this function returns YSOK, and obHd will be updated to the handle to the previous object.
	    If the previous of obHd does not exist, this function returns YSERR, and obHd will be NULL.
	*/
	YSRESULT MoveToPrev(YsEditArrayObjectHandle <T,bitShift> &obHd) const;


	/*! Move obHd to the next object regardless of the state.
	    If obHd==NULL, this function finds the next first object ALIVE.
	    If the next of obHd exists, this function returns YSOK.
	    If the next of obHd does not exist, this function returns YSERR, and obHd will be NULL.
	*/
	YSRESULT MoveToNextIncludingFrozen(YsEditArrayObjectHandle <T,bitShift> &obHd) const;


	/*! Returns a search key (hash key) of the object.  Object pointed by obHd needs to be ALIVE. */
	YSHASHKEY GetSearchKey(YsEditArrayObjectHandle <T,bitShift> &obHd) const;

	/*! Returns a search key (hash key) of the object.  Object pointed by obHd can be FROZEN. */
	YSHASHKEY GetSearchKeyMaybeFrozen(YsEditArrayObjectHandle <T,bitShift> &obHd) const;

	/*! Cache index numbers per object so that the index can be founc quickly from a handle, and vise-versa. */
	void Encache(void) const;

	/*! Clear cache of index numbers per object so that the index can be founc quickly from a handle, and vise-versa. */
	void Decache(void) const;

	/*! Get a zero-based index from a handle. */
	YSSIZE_T GetIndexFromHandle(YsEditArrayObjectHandle <T,bitShift> obHd) const;

	/*! Get a handle from a zero-based index.  If 0>index, it returns nullptr.
	*/
	YsEditArrayObjectHandle <T,bitShift> GetObjectHandleFromIndex(YSSIZE_T index) const;

	typedef YsBidirectionalHandleEnumerator <YsEditArray<T,bitShift>,YsEditArrayObjectHandle<T,bitShift> > HandleEnumerator;

	/*! Get an all-handle object. 
	    Regular iterator is good, but not always convenient.  Rather than getting a reference to T, 
	    if often is convenient to iterate over the handles.  All-handle object will allow iterating like:
	        for(auto &hdl : array)
	        {
	            // hdl is of YsEditArrayObjectHandle <T,bitShift> &.
	        }
	   */
	HandleEnumerator AllHandle(void) const;

	/*! Override search keys.  HandleKeyTable must be a storage class of YsPair <YsEditArrayObjectHandle<T,bitShift>,YSHASHKEY>.
	    Search must be disabled by DisableSearch function before calling this function, or this function will fail and do nothing.
	    This function does not verify uniqueness of the search keys.  Caller must make sure no two YSHASHKEYs are equal.
	    If this object owns the search-key seed, it will be updated to the largest search key plus one.
	*/
	template <typename HandleKeyTable>
	YSRESULT OverrideSearchKey(const HandleKeyTable &table);


	/*! Test if the raw index is in range of the background data structure.
	*/
	YSBOOL IsRawIndexInRange(YsEditArray_INDEXTYPE index) const;

	/*! Check the state from the raw index.
	*/
	int GetStateFromRawIndex(YsEditArray_INDEXTYPE index) const;

	/*! Get search key from the raw index.
	*/
	YSHASHKEY GetSearchKeyFromRawIndex(YsEditArray_INDEXTYPE index) const;


public:
	class HandleStore : protected YsKeyStore
	{
	friend class const_iterator;

	public:
		using YsKeyStore::GetN;
		using YsKeyStore::size;

		HandleStore()
		{
		}
		HandleStore(YSSIZE_T n,const YsEditArrayObjectHandle <T,bitShift> hdArray[])
		{
			Add(n,hdArray);
		}
		HandleStore(const YsConstArrayMask <YsEditArrayObjectHandle <T,bitShift> > &hdArray)
		{
			Add(hdArray);
		}

		/*! Support for STL-like iterator. */
		class const_iterator
		{
		friend class HandleStore;

		private:
			YsKeyStore::const_iterator iter;
		public:
			inline const_iterator &operator++()
			{
				++iter;
				return *this;
			}
			inline const_iterator operator++(int)
			{
				const_iterator copy=*this;
				++iter;
				return copy;
			}
			inline bool operator==(const const_iterator &from)
			{
				return this->iter==from.iter;
			}
			inline bool operator!=(const const_iterator &from)
			{
				return this->iter!=from.iter;
			}
			inline YsEditArrayObjectHandle <T,bitShift> operator*()
			{
				return YsEditArray <T,bitShift>::MakeHandleFromRawIndex(*iter);
			}
		};

		/*! Support for STL-like iterator */
		inline const_iterator begin() const
		{
			const_iterator iter;
			iter.iter=YsKeyStore::begin();
			return iter;
		}

		/*! Support for STL-like iterator */
		inline const_iterator end() const
		{
			const_iterator iter;
			iter.iter=YsKeyStore::end();
			return iter;
		}



		/*! Returns an array that contains all polygon handles stored in this object. */
		YsArray <YsEditArrayObjectHandle <T,bitShift> > GetArray(void) const
		{
			YsArray <YsEditArrayObjectHandle <T,bitShift> > ary;
			for(auto obHd : *this)
			{
				ary.Add(obHd);
			}
			return ary;
		}

		void CleanUp(void)
		{
			YsKeyStore::CleanUp();
		}

		YSRESULT Add(YsEditArrayObjectHandle <T,bitShift> hd)
		{
			return YsKeyStore::Add((YSHASHKEY)YsEditArray<T,bitShift>::GetRawIndexFromHandle(hd));
		}
		YSRESULT Add(YSSIZE_T n,const YsEditArrayObjectHandle <T,bitShift> hd[])
		{
			for(YSSIZE_T i=0; i<n; ++i)
			{
				Add(hd[i]);
			}
			return YSOK;
		}
		YSRESULT Add(const YsConstArrayMask <YsEditArrayObjectHandle <T,bitShift> > &hdArray)
		{
			return Add(hdArray.GetN(),hdArray);
		}
		YSRESULT Delete(YsEditArrayObjectHandle <T,bitShift> hd)
		{
			return YsKeyStore::Delete((YSHASHKEY)YsEditArray <T,bitShift>::GetRawIndexFromHandle(hd));
		}

		YSBOOL IsIncluded(YsEditArrayObjectHandle <T,bitShift> hd) const
		{
			return YsKeyStore::IsIncluded((YSHASHKEY)YsEditArray <T,bitShift>::GetRawIndexFromHandle(hd));
		}
	};

	/*! To increase interoperability with STL */
	inline YSSIZE_T size(void) const
	{
		return GetN();
	}
};


template <class T,const int bitShift>
inline typename YsEditArray <T,bitShift>::iterator begin(YsEditArray <T,bitShift> &array)
{
	return array.begin();
}

template <class T,const int bitShift>
inline typename YsEditArray <T,bitShift>::iterator end(YsEditArray <T,bitShift> &array)
{
	return array.end();
}

template <class T,const int bitShift>
inline typename YsEditArray <T,bitShift>::const_iterator begin(const YsEditArray <T,bitShift> &array)
{
	return array.begin();
}

template <class T,const int bitShift>
inline typename YsEditArray <T,bitShift>::const_iterator end(const YsEditArray <T,bitShift> &array)
{
	return array.end();
}

template <class T,const int bitShift>
inline typename YsEditArray <T,bitShift>::iterator rbegin(YsEditArray <T,bitShift> &array)
{
	return array.rbegin();
}

template <class T,const int bitShift>
inline typename YsEditArray <T,bitShift>::iterator rend(YsEditArray <T,bitShift> &array)
{
	return array.rend();
}

template <class T,const int bitShift>
inline typename YsEditArray <T,bitShift>::const_iterator rbegin(const YsEditArray <T,bitShift> &array)
{
	return array.rbegin();
}

template <class T,const int bitShift>
inline typename YsEditArray <T,bitShift>::const_iterator rend(const YsEditArray <T,bitShift> &array)
{
	return array.rend();
}


template <class T,const int bitShift>
inline typename YsEditArray <T,bitShift>::HandleStore::const_iterator begin(const typename YsEditArray <T,bitShift>::HandleStore &hdStore)
{
	return hdStore.begin();
}

template <class T,const int bitShift>
inline typename YsEditArray <T,bitShift>::HandleStore::const_iterator end(const typename YsEditArray <T,bitShift>::HandleStore &hdStore)
{
	return hdStore.end();
}



template <class T,const int bitShift>
YsEditArray <T,bitShift>::YsEditArray()
{
	localSearchKeySeed=0;
	searchKeySeed=&localSearchKeySeed;

	searchEnabled=YSFALSE;
	nFrozen=0;

	maxCachedIndex=-1;
	indexToHandleCache.CleanUp();

	trapMoveToNext=false;
}

template <class T,const int bitShift>
YsEditArray<T,bitShift>::YsEditArray(const YsEditArray <T,bitShift> &incoming)
{
	localSearchKeySeed=0;
	searchKeySeed=&localSearchKeySeed;

	searchEnabled=YSFALSE;
	nFrozen=0;

	maxCachedIndex=-1;
	indexToHandleCache.CleanUp();

	CopyFrom(incoming);
}
template <class T,const int bitShift>
YsEditArray <T,bitShift> &YsEditArray<T,bitShift>::operator=(const YsEditArray <T,bitShift> &incoming)
{
	this->CopyFrom(incoming);
	return *this;
}
template <class T,const int bitShift>
void YsEditArray<T,bitShift>::CopyFrom(const YsEditArray <T,bitShift> &incoming)
{
	if(&incoming!=this)
	{
		CleanUp();
		for(auto incomingHd : incoming.AllHandle())
		{
			auto newHd=Create();
			*(*this)[newHd]=*incoming[incomingHd];
		}
	}
}

template <class T,const int bitShift>
void YsEditArray <T,bitShift>::CleanUp(void)
{
	objArray.Clear();
	deletedIndex.Clear();
	nFrozen=0;
	searchKeyToIndex.PrepareTable();;

	maxCachedIndex=-1;
	indexToHandleCache.CleanUp();
}

template <class T,const int bitShift>
void YsEditArray<T,bitShift>::ClearDeep(void)
{
	objArray.ClearDeep();
	deletedIndex.ClearDeep();
	nFrozen=0;
	searchKeyToIndex.PrepareTable();;

	maxCachedIndex=-1;
	indexToHandleCache.ClearDeep();
}

template <class T,const int bitShift>
void YsEditArray<T,bitShift>::MoveFrom(YsEditArray<T,bitShift> &incoming)
{
	if(this!=&incoming)
	{
		this->localSearchKeySeed=incoming.localSearchKeySeed;
		if(incoming.searchKeySeed==&incoming.localSearchKeySeed) // Using a local seed.
		{
			this->searchKeySeed=&this->localSearchKeySeed;
		}
		else // Using a shared seed.
		{
			this->searchKeySeed=incoming.searchKeySeed;
		}

		this->objArray.MoveFrom(incoming.objArray);
		this->deletedIndex.MoveFrom(incoming.deletedIndex);
		this->nFrozen=incoming.nFrozen;

		this->searchEnabled=incoming.searchEnabled;
		this->searchKeyToIndex.MoveFrom(incoming.searchKeyToIndex);

		this->maxCachedIndex=incoming.maxCachedIndex;
		this->indexToHandleCache.MoveFrom(incoming.indexToHandleCache);
	}
}

template <class T,const int bitShift>
void YsEditArray <T,bitShift>::UseSharedSearchKeySeed(YSHASHKEY &seedPtr)
{
	searchKeySeed=&seedPtr;
}

template <class T,const int bitShift>
YSHASHKEY YsEditArray<T,bitShift>::GetNextSearchKey(void) const
{
	return *searchKeySeed;
}

template <class T,const int bitShift>
void YsEditArray <T,bitShift>::EnableSearch(void)
{
	if(YSTRUE!=searchEnabled)
	{
		searchEnabled=YSTRUE;
		searchKeyToIndex.PrepareTable(1+objArray.GetN()/4);

		for(YsEditArrayObjectHandle<T,bitShift> ptr=NULL; YSOK==MoveToNext(ptr); )
		{
			YsEditArray_INDEXTYPE index=ptr.index;
			searchKeyToIndex.AddElement(objArray[index].GetSearchKey(),index);
		}
	}
}

template <class T,const int bitShift>
void YsEditArray <T,bitShift>::DisableSearch(void)
{
	searchEnabled=YSFALSE;
	searchKeyToIndex.CleanUp();
}

template <class T,const int bitShift>
YSBOOL YsEditArray<T,bitShift>::SearchEnabled(void) const
{
	return searchEnabled;
}

template <class T,const int bitShift>
YSSIZE_T YsEditArray <T,bitShift>::GetN(void) const
{
	return objArray.GetN()-deletedIndex.GetN()-nFrozen;
}

template <class T,const int bitShift>
YSBOOL YsEditArray <T,bitShift>::IsFrozen(YsEditArrayObjectHandle<T,bitShift> obHd) const
{
	if(NULL!=obHd &&
	   YSTRUE==objArray.IsInRange(obHd.index) &&
	   FROZEN==objArray[obHd.index].GetState())
	{
		return YSTRUE;
	}
	return YSFALSE;
}

template <class T,const int bitShift>
YsEditArrayObjectHandle<T,bitShift> YsEditArray <T,bitShift>::Create(void)
{
	YsEditArray_INDEXTYPE newIdx;
	if(0<deletedIndex.GetN())
	{
		newIdx=deletedIndex.GetEnd();
		deletedIndex.DeleteLast();
	}
	else
	{
		newIdx=(YsEditArray_INDEXTYPE)objArray.GetN();
		objArray.Increment();
	}

	YsEditArrayObjectHandle<T,bitShift> obHd;
	obHd.index=newIdx;

	objArray[newIdx].SetState(ALIVE);
	objArray[newIdx].SetSearchKey(*searchKeySeed);
	objArray[newIdx].cachedIndex=0x7fffffff;

	if(YSTRUE==searchEnabled)
	{
		searchKeyToIndex.AddElement(*searchKeySeed,newIdx);
	}

	++(*searchKeySeed);

	Decache();

	return obHd;
}

template <class T,const int bitShift>
T *YsEditArray <T,bitShift>::operator[] (YsEditArrayObjectHandle<T,bitShift> hd)
{
	if(YSTRUE==objArray.IsInRange(hd.index) &&
	   ALIVE==objArray[hd.index].GetState())
	{
		return &objArray[hd.index].dat;
	}
	return NULL;
}

template <class T,const int bitShift>
const T *YsEditArray <T,bitShift>::operator[] (YsEditArrayObjectHandle<T,bitShift> hd) const
{
	if(YSTRUE==objArray.IsInRange(hd.index) &&
	   ALIVE==objArray[hd.index].GetState())
	{
		return &objArray[hd.index].dat;
	}
	return NULL;
}

template <class T,const int bitShift>
T &YsEditArray<T,bitShift>::Value(YsEditArrayObjectHandle<T,bitShift> hd)
{
	return objArray[hd.index].dat;
}

template <class T,const int bitShift>
const T &YsEditArray<T,bitShift>::Value(YsEditArrayObjectHandle<T,bitShift> hd) const
{
	return objArray[hd.index].dat;
}

template <class T,const int bitShift>
T *YsEditArray <T,bitShift>::GetFrozenObject(YsEditArrayObjectHandle<T,bitShift> hd)
{
	if(YSTRUE==objArray.IsInRange(hd.index) &&
	   FROZEN==objArray[hd.index].GetState())
	{
		return &objArray[hd.index].dat;
	}
	return NULL;
}

template <class T,const int bitShift>
const T *YsEditArray <T,bitShift>::GetFrozenObject(YsEditArrayObjectHandle<T,bitShift> hd) const
{
	if(YSTRUE==objArray.IsInRange(hd.index) &&
	   FROZEN==objArray[hd.index].GetState())
	{
		return &objArray[hd.index].dat;
	}
	return NULL;
}

template <class T,const int bitShift>
T *YsEditArray <T,bitShift>::GetObjectMaybeFrozen(YsEditArrayObjectHandle<T,bitShift> hd)
{
	if(YSTRUE==objArray.IsInRange(hd.index) &&
	   (FROZEN==objArray[hd.index].GetState() || ALIVE==objArray[hd.index].GetState()))
   {
		return &objArray[hd.index].dat;
	}
	return NULL;
}

template <class T,const int bitShift>
const T *YsEditArray <T,bitShift>::GetObjectMaybeFrozen(YsEditArrayObjectHandle<T,bitShift> hd) const
{
	if(YSTRUE==objArray.IsInRange(hd.index) &&
	   (FROZEN==objArray[hd.index].GetState() || ALIVE==objArray[hd.index].GetState()))
   {
		return &objArray[hd.index].dat;
	}
	return NULL;
}

template <class T,const int bitShift>
YsEditArrayObjectHandle<T,bitShift> YsEditArray <T,bitShift>::Append(const T &incoming)
{
	YsEditArrayObjectHandle<T,bitShift> newObjHd=Create();
	(*(*this)[newObjHd])=incoming;
	return newObjHd;
}

template <class T,const int bitShift>
YSRESULT YsEditArray <T,bitShift>::Delete(YsEditArrayObjectHandle<T,bitShift> obHd)
{
	if(NULL!=obHd && 
	   YSTRUE==objArray.IsInRange(obHd.index) &&
	   ALIVE==objArray[obHd.index].GetState())
	{
		if(YSTRUE==searchEnabled)
		{
			searchKeyToIndex.DeleteElement(objArray[obHd.index].GetSearchKey(),obHd.index);
		}
		objArray[obHd.index].SetState(DELETED);
		deletedIndex.Append(obHd.index);

		// Instead of Decache(), reduce cache.
		if(objArray[obHd.index].cachedIndex<=maxCachedIndex)
		{
			maxCachedIndex=objArray[obHd.index].cachedIndex-1;
			indexToHandleCache.Resize(objArray[obHd.index].cachedIndex);
		}

		return YSOK;
	}
	return YSERR;
}

template <class T,const int bitShift>
YSRESULT YsEditArray <T,bitShift>::Freeze(YsEditArrayObjectHandle<T,bitShift> obHd)
{
	if(NULL!=obHd &&
	   YSTRUE==objArray.IsInRange(obHd.index) &&
	   ALIVE==objArray[obHd.index].GetState())
	{
		objArray[obHd.index].SetState(FROZEN);
		++nFrozen;

		// Instead of Decache(), reduce cache.
		if(objArray[obHd.index].cachedIndex<=maxCachedIndex)
		{
			maxCachedIndex=objArray[obHd.index].cachedIndex-1;
			indexToHandleCache.Resize(objArray[obHd.index].cachedIndex);
		}

		return YSOK;
	}
	return YSERR;
}

template <class T,const int bitShift>
YSRESULT YsEditArray <T,bitShift>::Melt(YsEditArrayObjectHandle<T,bitShift> obHd)
{
	if(NULL!=obHd &&
	   YSTRUE==objArray.IsInRange(obHd.index) &&
	   FROZEN==objArray[obHd.index].GetState())
	{
		objArray[obHd.index].SetState(ALIVE);
		--nFrozen;
		Decache();
		return YSOK;
	}
	return YSERR;
}

template <class T,const int bitShift>
YSRESULT YsEditArray <T,bitShift>::DeleteFrozen(YsEditArrayObjectHandle<T,bitShift> obHd)
{
	if(NULL!=obHd &&
	   YSTRUE==objArray.IsInRange(obHd.index) &&
	   FROZEN==objArray[obHd.index].GetState())
	{
		Melt(obHd);
		Delete(obHd);
		return YSOK;
	}
	return YSERR;
}

template <class T,const int bitShift>
YsEditArrayObjectHandle<T,bitShift> YsEditArray <T,bitShift>::FindObject(unsigned int key) const
{
	YsEditArray_INDEXTYPE index;
	if(YSOK==searchKeyToIndex.FindElement(index,key) &&
	   YSTRUE==objArray.IsInRange(index) &&
	   ALIVE==objArray[index].GetState())
	{
		return MakeHandleFromRawIndex(index);
	}
	return NULL;
}

template <class T,const int bitShift>
YsEditArrayObjectHandle<T,bitShift> YsEditArray <T,bitShift>::FindFrozenObject(unsigned int key) const
{
	YsEditArray_INDEXTYPE index;
	if(YSOK==searchKeyToIndex.FindElement(index,key) &&
	   YSTRUE==objArray.IsInRange(index) &&
	   FROZEN==objArray[index].GetState())
	{
		YsEditArrayObjectHandle<T,bitShift> obHd;
		obHd.index=index;
		return obHd;
	}
	return NULL;
}

template <class T,const int bitShift>
YsEditArrayObjectHandle<T,bitShift> YsEditArray<T,bitShift>::FindObjectMaybeFrozen(unsigned int key) const
{
	YsEditArray_INDEXTYPE index;
	if(YSOK==searchKeyToIndex.FindElement(index,key) &&
	   YSTRUE==objArray.IsInRange(index))
	{
		YsEditArrayObjectHandle<T,bitShift> obHd;
		obHd.index=index;
		return obHd;
	}
	return NULL;
}

template <class T,const int bitShift>
YsEditArrayObjectHandle <T,bitShift> YsEditArray <T,bitShift>::FindNext(const YsEditArrayObjectHandle <T,bitShift> obHd) const
{
	YsEditArrayObjectHandle <T,bitShift> nextObHd=obHd;
	if(YSOK==MoveToNext(nextObHd))
	{
		return nextObHd;
	}
	return NULL;
}

template <class T,const int bitShift>
YsEditArrayObjectHandle <T,bitShift> YsEditArray <T,bitShift>::FindPrev(const YsEditArrayObjectHandle <T,bitShift> obHd) const
{
	YsEditArrayObjectHandle <T,bitShift> prevObHd=obHd;
	if(YSOK==MoveToPrev(prevObHd))
	{
		return prevObHd;
	}
	return NULL;
}

template <class T,const int bitShift>
YsEditArrayObjectHandle <T,bitShift> YsEditArray<T,bitShift>::First(void) const
{
	YsEditArrayObjectHandle <T,bitShift> obHd=NULL;
	MoveToNext(obHd);
	return obHd;
}

template <class T,const int bitShift>
YsEditArrayObjectHandle <T,bitShift> YsEditArray<T,bitShift>::Last(void) const
{
	YsEditArrayObjectHandle <T,bitShift> obHd=NULL;
	MoveToPrev(obHd);
	return obHd;
}

template <class T,const int bitShift>
YsEditArrayObjectHandle <T,bitShift> YsEditArray<T,bitShift>::Null(void) const
{
	return NULL;
}

template <class T,const int bitShift>
YSRESULT YsEditArray <T,bitShift>::MoveToNext(YsEditArrayObjectHandle <T,bitShift> &obHd) const
{
	if(true==trapMoveToNext)
	{
		printf("Moving Next from RawIndex=%d\n",obHd.GetRawIndex());
	}

	YsEditArray_INDEXTYPE i0;
	if(NULL==obHd || YSTRUE!=objArray.IsInRange(obHd.index))
	{
		i0=0;
	}
	else
	{
		i0=obHd.index+1;
	}
	for(YsEditArray_INDEXTYPE i=i0; i<objArray.GetN(); ++i)
	{
		if(ALIVE==objArray[i].GetState())
		{
			obHd.index=i;
			return YSOK;
		}
	}
	obHd=NULL;
	return YSERR;
}

template <class T,const int bitShift>
YSRESULT YsEditArray <T,bitShift>::MoveToNextIncludingFrozen(YsEditArrayObjectHandle <T,bitShift> &obHd) const
{
	YsEditArray_INDEXTYPE i0;
	if(NULL==obHd || YSTRUE!=objArray.IsInRange(obHd.index))
	{
		i0=0;
	}
	else
	{
		i0=obHd.index+1;
	}
	if(YSTRUE==objArray.IsInRange(i0))
	{
		obHd.index=i0;
		return YSOK;
	}
	obHd=NULL;
	return YSERR;
}

template <class T,const int bitShift>
YSRESULT YsEditArray <T,bitShift>::MoveToPrev(YsEditArrayObjectHandle <T,bitShift> &obHd) const
{
	YsEditArray_INDEXTYPE i0;
	if(NULL==obHd || YSTRUE!=objArray.IsInRange(obHd.index))
	{
		i0=(YsEditArray_INDEXTYPE)objArray.GetN()-1;
	}
	else
	{
		i0=obHd.index-1;
	}
	for(YsEditArray_INDEXTYPE i=i0; 0<=i; --i)
	{
		if(ALIVE==objArray[i].GetState())
		{
			obHd.index=i;
			return YSOK;
		}
	}
	obHd=NULL;
	return YSERR;
}

template <class T,const int bitShift>
YSHASHKEY YsEditArray <T,bitShift>::GetSearchKey(YsEditArrayObjectHandle <T,bitShift> &obHd) const
{
	if(NULL!=obHd && 
	   YSTRUE==objArray.IsInRange(obHd.index) &&
	   ALIVE==objArray[obHd.index].GetState())
	{
		return objArray[obHd.index].GetSearchKey();
	}
	return YSNULLHASHKEY;
}

template <class T,const int bitShift>
YSHASHKEY YsEditArray <T,bitShift>::GetSearchKeyMaybeFrozen(YsEditArrayObjectHandle <T,bitShift> &obHd) const
{
	if(NULL!=obHd && 
	   YSTRUE==objArray.IsInRange(obHd.index) &&
	   (ALIVE==objArray[obHd.index].GetState() || FROZEN==objArray[obHd.index].GetState()))
	{
		return objArray[obHd.index].GetSearchKey();
	}
	return YSNULLHASHKEY;
}

template <class T,const int bitShift>
void YsEditArray <T,bitShift>::Encache(void) const
{
	indexToHandleCache.CleanUp();
	maxCachedIndex=0;

	YsEditArray_INDEXTYPE index=0;

	YsEditArrayObjectHandle <T,bitShift> obHd=NULL;
	while(YSOK==MoveToNext(obHd))
	{
		objArray[obHd.index].cachedIndex=index;
		indexToHandleCache.Append(obHd);
		++maxCachedIndex;
		++index;
	}
}

template <class T,const int bitShift>
void YsEditArray <T,bitShift>::Decache(void) const
{
	maxCachedIndex=-1;
	indexToHandleCache.CleanUp();
}

template <class T,const int bitShift>
YSSIZE_T YsEditArray <T,bitShift>::GetIndexFromHandle(YsEditArrayObjectHandle <T,bitShift> obHd) const
{
	if(NULL!=obHd && 
	   YSTRUE==objArray.IsInRange(obHd.index) &&
	   objArray[obHd.index].cachedIndex<maxCachedIndex)
	{
		return objArray[obHd.index].cachedIndex;
	}
	return -1;
}

template <class T,const int bitShift>
YsEditArrayObjectHandle <T,bitShift> YsEditArray <T,bitShift>::GetObjectHandleFromIndex(YSSIZE_T index) const
{
	if(YSTRUE==indexToHandleCache.IsInRange(index))
	{
		return indexToHandleCache[index];
	}
	if(0>index || size()<=index)
	{
		return nullptr;
	}

	YsEditArrayObjectHandle <T,bitShift> obHd=nullptr;
	decltype(index) ctr=0;
	if(0<indexToHandleCache.GetN())
	{
		obHd=indexToHandleCache.Last();
		ctr=indexToHandleCache.GetN();
	}

	while(ctr<=index)
	{
		MoveToNext(obHd);
		++ctr;
		if(nullptr==obHd)
		{
			break;
		}
	}
	return obHd;
}

template <class T,const int bitShift>
template <typename HandleKeyTable>
YSRESULT YsEditArray <T,bitShift>::OverrideSearchKey(const HandleKeyTable &table)
{
	auto searchEnabled=SearchEnabled();
	if(YSTRUE==searchEnabled)
	{
		DisableSearch();
	}

	YSHASHKEY maxKey=0;
	for(auto &handleKeyPair : table)
	{
		auto obHd=handleKeyPair.a;
		auto key=handleKeyPair.b;
		if(NULL!=obHd && YSTRUE==objArray.IsInRange(obHd.index))
		{
			objArray[obHd.index].SetSearchKey(key);
			YsMakeGreater(maxKey,key);
		}
	}

	YsMakeGreater(localSearchKeySeed,maxKey+1);

	if(YSTRUE==searchEnabled)
	{
		EnableSearch();
	}

	return YSOK;
}

template <class T,const int bitShift>
YSBOOL YsEditArray<T,bitShift>::IsRawIndexInRange(YsEditArray_INDEXTYPE index) const
{
	return objArray.IsInRange(index);
}
template <class T,const int bitShift>
int YsEditArray<T,bitShift>::GetStateFromRawIndex(YsEditArray_INDEXTYPE index) const
{
	return objArray[index].GetState();
}
template <class T,const int bitShift>
YSHASHKEY YsEditArray<T,bitShift>::GetSearchKeyFromRawIndex(YsEditArray_INDEXTYPE index) const
{
	return objArray[index].GetSearchKey();
}

////////////////////////////////////////////////////////////

// This class is supposed to be YsEditArray <T,bitShift>::HandleEnumerator.
// However, gcc cannot match begin(YsEditArray <T,bitShift>::HandleEnumerator &).
// It actually find the right function as a candidate, but for some wrong reason, it thinks
// the function does not match.  Am I missing something? 
// gcc 4.6.3
template <class T,const int bitShift>
class YsEditArrayHandleEnumerator
{
public:
	const YsEditArray <T,bitShift> *array;

	class iterator : public YsEditArray <T,bitShift>::const_iterator
	{
	public:
		iterator(const YsEditArray <T,bitShift> *ptr) : YsEditArray <T,bitShift>::const_iterator(ptr)
		{
		}

		inline YsEditArrayObjectHandle <T,bitShift> &operator*()
		{
			// I don't think any ambiguity of the member variable index.
			// However, gcc cannot find it unless I add
			//   'YsEditArray <T,bitShift>::const_iterator::'
			// in front of index.
			// gcc 4.6.3
			return YsEditArray <T,bitShift>::const_iterator::Handle();
		}
	};

	/*! Support for STL-like iterator */
	iterator begin() const
	{
		iterator iter(array);
		iter.begin();
		return iter;
	}

	/*! Support for STL-like iterator */
	iterator end() const
	{
		iterator iter(array);
		iter.end();
		return iter;
	}

	/*! Returns an array that contains all handles. */
	YsArray <YsEditArrayObjectHandle <T,bitShift> > Array(void) const
	{
		YsArray <YsEditArrayObjectHandle <T,bitShift> > hdArray(array->GetN(),NULL);
		YsEditArray_INDEXTYPE idx=0;
		for(auto hd=begin(); hd!=end(); ++hd)
		{
			hdArray[idx]=*hd;
			++idx;
		}
		return hdArray;
	}
};

template <class T,const int bitShift>
typename YsEditArrayHandleEnumerator <T,bitShift>::iterator begin(const YsEditArrayHandleEnumerator <T,bitShift> &allHandle)
{
	return allHandle.begin();
}

template <class T,const int bitShift>
typename YsEditArrayHandleEnumerator <T,bitShift>::iterator end(const YsEditArrayHandleEnumerator <T,bitShift> &allHandle)
{
	return allHandle.end();
}

template <class T,const int bitShift>
typename YsEditArrayHandleEnumerator <T,bitShift>::iterator rbegin(const YsEditArrayHandleEnumerator <T,bitShift> &allHandle)
{
	return allHandle.rbegin();
}

template <class T,const int bitShift>
typename YsEditArrayHandleEnumerator <T,bitShift>::iterator rend(const YsEditArrayHandleEnumerator <T,bitShift> &allHandle)
{
	return allHandle.rend();
}



////////////////////////////////////////////////////////////

template <class T,const int bitShift>
typename YsEditArray <T,bitShift>::HandleEnumerator YsEditArray <T,bitShift>::AllHandle(void) const
{
	YsBidirectionalHandleEnumerator <YsEditArray<T,bitShift>,YsEditArrayObjectHandle<T,bitShift> > allHandle(this);
	return allHandle;
}


/* } */
#endif
