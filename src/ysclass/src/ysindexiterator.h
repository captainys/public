/* ////////////////////////////////////////////////////////////

File Name: ysindexiterator.h
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

#ifndef YSINDEXITERATOR_H_IS_INCLUDED
#define YSINDEXITERATOR_H_IS_INCLUDED
/* { */



template <class ArrayClass,class WhatsStored>
class YsIndexIterator
{
private:
	ArrayClass *array;
	YSSIZE_T index,dir;
public:
	inline YsIndexIterator(const YsIndexIterator <ArrayClass,WhatsStored> &incoming)
	{
		this->array=incoming.array;
		this->index=incoming.index;
		this->dir=incoming.dir;
	}
	inline YsIndexIterator(ArrayClass *array)
	{
		this->array=array;
	}

	inline YsIndexIterator <ArrayClass,WhatsStored> begin(void)
	{
		index=0;
		dir=1;
		return *this;
	}
	inline YsIndexIterator <ArrayClass,WhatsStored> end(void)
	{
		index=array->GetN();
		dir=1;
		return *this;
	}
	inline YsIndexIterator <ArrayClass,WhatsStored> rbegin(void)
	{
		index=array->GetN()-1;
		dir=-1;
		return *this;
	}
	inline YsIndexIterator <ArrayClass,WhatsStored> rend(void)
	{
		index=-1;
		dir=-1;
		return *this;
	}


	inline YsIndexIterator <ArrayClass,WhatsStored> &operator++()
	{
		index+=dir;
		return *this;
	}
	inline YsIndexIterator <ArrayClass,WhatsStored> operator++(int)
	{
		auto copy=*this;
		index+=dir;
		return copy;
	}
	inline YsIndexIterator <ArrayClass,WhatsStored> &operator--()
	{
		index-=dir;
		return *this;
	}
	inline YsIndexIterator <ArrayClass,WhatsStored> operator--(int)
	{
		auto copy=*this;
		index-=dir;
		return copy;
	}
	inline bool operator==(const YsIndexIterator <ArrayClass,WhatsStored> &from)
	{
		return (array==from.array && index==from.index);
	}
	inline bool operator!=(const YsIndexIterator <ArrayClass,WhatsStored> &from)
	{
		return (array!=from.array || index!=from.index);
	}
	inline WhatsStored &operator*()
	{
		return (*array)[index];
	}
};



/* } */
#endif
