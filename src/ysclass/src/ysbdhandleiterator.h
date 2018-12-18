/* ////////////////////////////////////////////////////////////

File Name: ysbdhandleiterator.h
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

#ifndef YSBDHANDLEITERATOR_IS_INCLUDED
#define YSBDHANDLEITERATOR_IS_INCLUDED
/* { */

/*
YsBidirectionalHandleIterator is for iterating over the values, not handles.  
For iterating over the handles, use YsBidirectionalForwardHandleIterator.

Storage class must have:
	MoveToNext
	MoveToPrev
	First
	Last
	Null
	Value
	const WhatsStored &Value
*/

template <class StorageClass,class HandleClass,class WhatsStored>
class YsBidirectionalHandleIterator
{
private:
	StorageClass *storage;
	HandleClass hd;
	int dir;

	inline void Forward(void)
	{
		if(0<=dir)
		{
			storage->MoveToNext(hd);
		}
		else
		{
			storage->MoveToPrev(hd);
		}
	}
	inline void Backward(void)
	{
		if(0<=dir)
		{
			storage->MoveToPrev(hd);
		}
		else
		{
			storage->MoveToNext(hd);
		}
	}
public:
	YsBidirectionalHandleIterator(const YsBidirectionalHandleIterator<StorageClass,HandleClass,WhatsStored> &from)
	{
		this->storage=from.storage;
		this->hd=from.hd;
		this->dir=from.dir;
	}
	YsBidirectionalHandleIterator(StorageClass *s)
	{
		this->storage=s;
	}

	YsBidirectionalHandleIterator<StorageClass,HandleClass,WhatsStored> begin(void)
	{
		hd=storage->First();
		dir=1;
		return *this;
	}
	YsBidirectionalHandleIterator<StorageClass,HandleClass,WhatsStored> end(void)
	{
		hd=storage->Null();
		dir=1;
		return *this;
	}
	YsBidirectionalHandleIterator<StorageClass,HandleClass,WhatsStored> rbegin(void)
	{
		hd=storage->Last();
		dir=-1;
		return *this;
	}
	YsBidirectionalHandleIterator<StorageClass,HandleClass,WhatsStored> rend(void)
	{
		hd=storage->Null();
		dir=-1;
		return *this;
	}

	inline YsBidirectionalHandleIterator<StorageClass,HandleClass,WhatsStored> &operator++()
	{
		Forward();
		return *this;
	}
	inline YsBidirectionalHandleIterator<StorageClass,HandleClass,WhatsStored> operator++(int)
	{
		auto copy=*this;
		Forward();
		return copy;
	}
	inline YsBidirectionalHandleIterator<StorageClass,HandleClass,WhatsStored> &operator--()
	{
		Backward();
		return *this;
	}
	inline YsBidirectionalHandleIterator<StorageClass,HandleClass,WhatsStored> operator--(int)
	{
		auto copy=*this;
		Backward();
		return copy;
	}
	inline bool operator==(const YsBidirectionalHandleIterator<StorageClass,HandleClass,WhatsStored> &from)
	{
		return (storage==from.storage && hd==from.hd);
	}
	inline bool operator!=(const YsBidirectionalHandleIterator<StorageClass,HandleClass,WhatsStored> &from)
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
YsBidirectionalForwardHandleIterator is for iterating over the handles, not values.
To be safe for deletion, the object retains the handle for the next object (or prev if it is backward).
Even if the object is deleted in the loop, it is able to move to the next object.
*/
template <class StorageClass,class HandleClass>
class YsBidirectionalForwardHandleIterator
{
private:
	const StorageClass *storage;
	HandleClass hd,nextHd;
	int dir;

	inline void Forward(void)
	{
		hd=nextHd;
		if(0<=dir)
		{
			storage->MoveToNext(nextHd);
		}
		else
		{
			storage->MoveToPrev(nextHd);
		}
	}
	inline void Backward(void)
	{
		hd=nextHd;
		if(0<=dir)
		{
			storage->MoveToPrev(nextHd);
		}
		else
		{
			storage->MoveToNext(nextHd);
		}
	}
public:
	YsBidirectionalForwardHandleIterator(const YsBidirectionalForwardHandleIterator<StorageClass,HandleClass> &from)
	{
		this->storage=from.storage;
		this->hd=from.hd;
		this->nextHd=from.nextHd;
		this->dir=from.dir;
	}
	YsBidirectionalForwardHandleIterator(const StorageClass *s)
	{
		this->storage=s;
	}

	YsBidirectionalForwardHandleIterator<StorageClass,HandleClass> begin(void)
	{
		nextHd=storage->First();
		dir=1;
		Forward();
		return *this;
	}
	YsBidirectionalForwardHandleIterator<StorageClass,HandleClass> end(void)
	{
		hd=storage->Null();
		nextHd=hd;
		dir=1;
		return *this;
	}
	YsBidirectionalForwardHandleIterator<StorageClass,HandleClass> rbegin(void)
	{
		hd=storage->Last();
		dir=-1;
		Forward();
		return *this;
	}
	YsBidirectionalForwardHandleIterator<StorageClass,HandleClass> rend(void)
	{
		hd=storage->Null();
		nextHd=hd;
		dir=-1;
		return *this;
	}

	inline YsBidirectionalForwardHandleIterator<StorageClass,HandleClass> &operator++()
	{
		Forward();
		return *this;
	}
	inline YsBidirectionalForwardHandleIterator<StorageClass,HandleClass> operator++(int)
	{
		auto copy=*this;
		Forward();
		return copy;
	}
	inline YsBidirectionalForwardHandleIterator<StorageClass,HandleClass> &operator--()
	{
		Backward();
		return *this;
	}
	inline YsBidirectionalForwardHandleIterator<StorageClass,HandleClass> operator--(int)
	{
		auto copy=*this;
		Backward();
		return copy;
	}
	inline bool operator==(const YsBidirectionalForwardHandleIterator<StorageClass,HandleClass> &from)
	{
		return (storage==from.storage && hd==from.hd);
	}
	inline bool operator!=(const YsBidirectionalForwardHandleIterator<StorageClass,HandleClass> &from)
	{
		return (storage!=from.storage || hd!=from.hd);
	}
	inline HandleClass operator*()
	{
		return hd;
	}
};

template <class StorageClass,class HandleClass>
class YsBidirectionalHandleEnumerator
{
private:
	const StorageClass *storage;
public:
	typedef YsBidirectionalForwardHandleIterator <StorageClass,HandleClass> iterator;

	YsBidirectionalHandleEnumerator(const StorageClass *storage)
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

	iterator rbegin() const
	{
		iterator iter(storage);
		return iter.rbegin();
	}
	iterator rend() const
	{
		iterator iter(storage);
		return iter.rend();
	}

	YsArray <HandleClass> Array(void) const
	{
		YsArray <HandleClass> ary;
		for(auto iter=this->begin(); iter!=this->end(); ++iter)
		{
			ary.push_back(*iter);
		}
		return ary;
	}
};

template <class StorageClass,class HandleClass>
YsBidirectionalForwardHandleIterator <StorageClass,HandleClass> begin(YsBidirectionalHandleEnumerator <StorageClass,HandleClass> handleEnumerator)
{
	return handleEnumerator.begin();
}
template <class StorageClass,class HandleClass>
YsBidirectionalForwardHandleIterator <StorageClass,HandleClass> end(YsBidirectionalHandleEnumerator <StorageClass,HandleClass> handleEnumerator)
{
	return handleEnumerator.end();
}
template <class StorageClass,class HandleClass>
YsBidirectionalForwardHandleIterator <StorageClass,HandleClass> rbegin(YsBidirectionalHandleEnumerator <StorageClass,HandleClass> handleEnumerator)
{
	return handleEnumerator.rbegin();
}
template <class StorageClass,class HandleClass>
YsBidirectionalForwardHandleIterator <StorageClass,HandleClass> rend(YsBidirectionalHandleEnumerator <StorageClass,HandleClass> handleEnumerator)
{
	return handleEnumerator.rend();
}

/* } */
#endif
