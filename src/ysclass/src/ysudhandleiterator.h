/* ////////////////////////////////////////////////////////////

File Name: ysudhandleiterator.h
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

#ifndef YSUDHANDLEITERATOR_IS_INCLUDED
#define YSUDHANDLEITERATOR_IS_INCLUDED
/* { */

/*
YsUnidirectionalHandleIterator is for iterating over the values, not handles.  
For iterating over the handles, use YsUnidirectionalHandleEnumerator.

Storage class must have:
    MoveToNext
    First
    Null
    Value
*/

template <class StorageClass,class HandleClass,class WhatsStored>
class YsUnidirectionalHandleIterator
{
private:
	StorageClass *storage;
	HandleClass hd;

	inline void Forward(void)
	{
		storage->MoveToNext(hd);
	}
public:
	YsUnidirectionalHandleIterator(const YsUnidirectionalHandleIterator<StorageClass,HandleClass,WhatsStored> &from)
	{
		this->storage=from.storage;
		this->hd=from.hd;
	}
	YsUnidirectionalHandleIterator(StorageClass *s)
	{
		this->storage=s;
	}

	YsUnidirectionalHandleIterator<StorageClass,HandleClass,WhatsStored> begin(void)
	{
		hd=storage->First();
		return *this;
	}
	YsUnidirectionalHandleIterator<StorageClass,HandleClass,WhatsStored> end(void)
	{
		hd=storage->Null();
		return *this;
	}

	inline YsUnidirectionalHandleIterator<StorageClass,HandleClass,WhatsStored> &operator++()
	{
		Forward();
		return *this;
	}
	inline YsUnidirectionalHandleIterator<StorageClass,HandleClass,WhatsStored> operator++(int)
	{
		auto copy=*this;
		Forward();
		return copy;
	}
	inline bool operator==(const YsUnidirectionalHandleIterator<StorageClass,HandleClass,WhatsStored> &from)
	{
		return (storage==from.storage && hd==from.hd);
	}
	inline bool operator!=(const YsUnidirectionalHandleIterator<StorageClass,HandleClass,WhatsStored> &from)
	{
		return (storage!=from.storage || hd!=from.hd);
	}
	inline WhatsStored &operator*()
	{
		return storage->Value(hd);
	}
	HandleClass Handle(void) const
	{
		return hd;
	}
};



/*
YsUnidirectionalForwardHandleIterator is for iterating over the handles, not values.
To be safe for deletion, the object retains the handle for the next object (or prev if it is backward).
Even if the object is deleted in the loop, it is able to move to the next object.
*/
template <class StorageClass,class HandleClass>
class YsUnidirectionalForwardHandleIterator
{
private:
	const StorageClass *storage;
	HandleClass hd,nextHd;
	int dir;

	inline void Forward(void)
	{
		hd=nextHd;
		storage->MoveToNext(nextHd);
	}

public:
	YsUnidirectionalForwardHandleIterator(const YsUnidirectionalForwardHandleIterator<StorageClass,HandleClass> &from)
	{
		this->storage=from.storage;
		this->hd=from.hd;
		this->nextHd=from.nextHd;
	}
	YsUnidirectionalForwardHandleIterator(const StorageClass *s)
	{
		this->storage=s;
	}

	YsUnidirectionalForwardHandleIterator<StorageClass,HandleClass> begin(void)
	{
		nextHd=storage->First();
		Forward();
		return *this;
	}
	YsUnidirectionalForwardHandleIterator<StorageClass,HandleClass> end(void)
	{
		hd=storage->Null();
		nextHd=hd;
		return *this;
	}

	inline YsUnidirectionalForwardHandleIterator<StorageClass,HandleClass> &operator++()
	{
		Forward();
		return *this;
	}
	inline YsUnidirectionalForwardHandleIterator<StorageClass,HandleClass> operator++(int)
	{
		auto copy=*this;
		Forward();
		return copy;
	}
	inline bool operator==(const YsUnidirectionalForwardHandleIterator<StorageClass,HandleClass> &from)
	{
		return (storage==from.storage && hd==from.hd);
	}
	inline bool operator!=(const YsUnidirectionalForwardHandleIterator<StorageClass,HandleClass> &from)
	{
		return (storage!=from.storage || hd!=from.hd);
	}
	inline HandleClass operator*()
	{
		return hd;
	}
};

template <class StorageClass,class HandleClass>
class YsUnidirectionalHandleEnumerator
{
private:
	const StorageClass *storage;
public:
	typedef YsUnidirectionalForwardHandleIterator <StorageClass,HandleClass> iterator;

	YsUnidirectionalHandleEnumerator(const StorageClass *storage)
	{
		this->storage=storage;
	}

	iterator begin() const
	{
		iterator iter(storage);
		return iter.begin();
	}
	iterator end() const
	{
		iterator iter(storage);
		return iter.end();
	}
};

template <class StorageClass,class HandleClass>
YsUnidirectionalForwardHandleIterator <StorageClass,HandleClass> begin(YsUnidirectionalHandleEnumerator <StorageClass,HandleClass> handleEnumerator)
{
	return handleEnumerator.begin();
}
template <class StorageClass,class HandleClass>
YsUnidirectionalForwardHandleIterator <StorageClass,HandleClass> end(YsUnidirectionalHandleEnumerator <StorageClass,HandleClass> handleEnumerator)
{
	return handleEnumerator.end();
}

/* } */
#endif
