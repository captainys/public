/* ////////////////////////////////////////////////////////////

File Name: yssegarray.h
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

#ifndef YSSEGARRAY_IS_INCLUDED
#define YSSEGARRAY_IS_INCLUDED
/* { */

#include "ysdef.h"
#include "ysarray.h"
#include "ysrange.h"

/*! A variable length array class.
    This class stores array items as a segmented sub-arrays.  Since those sub-arrays are not
    continuous, this class cannot be casted to an ordinary C array.
    
    Since the sub-arrays will not be re-located like doubling-array, the location of the 
    array items will not change.  Therefore, it is safer to cache a pointer to an array
    item.
    */

template <class T,const unsigned int BitShift>
class YsSegmentedArray
{
// \cond
private:
	enum
	{
		SEGMENT_UNIT_SIZE=(1<<BitShift)
	};

	YSSIZE_T nv;
	YsArray <T *> segments;

protected:
	inline YSRESULT Alloc(YSSIZE_T n);
// \endcond

public:
// STL-like iterator support begin >>
public:
	typedef YsIndexIterator <YsSegmentedArray<T,BitShift>,T> iterator;
	typedef YsIndexIterator <const YsSegmentedArray<T,BitShift>,const T> const_iterator;
	inline iterator begin()
	{
		iterator iter(this);
		return iter.begin();
	}
	inline iterator end()
	{
		iterator iter(this);
		return iter.end();
	}
	inline const_iterator begin() const
	{
		const_iterator iter(this);
		return iter.begin();
	}
	inline const_iterator end() const
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

	/*! Index enumerator enables range-based for using an index.  It can be written as:
	    YsArray <T> array;
	    for(auto idx : array.AllIndex())
	    {
	        // Do something for array[idx]
	    }
	    Useful when something needs to be done before and after an array item. */
	inline YsIndexRange AllIndex(void) const;
// STL-like iterator support end <<

public:
	/*! Default constructor */
	inline YsSegmentedArray();

	/*! Copy constructor */
	inline YsSegmentedArray(const YsSegmentedArray <T,BitShift> &incoming);

	/*! Copy constructor */
	template <const unsigned int BitShift2>
	inline YsSegmentedArray(const YsSegmentedArray <T,BitShift2> &incoming);

	/*! Copy constructor */
	inline YsSegmentedArray(YSSIZE_T nv,const T v[]);

	/*! Default destructor */
	inline ~YsSegmentedArray();

	/*! This function copies v[0] to v[nv-1] to this array. */
	inline YSRESULT Set(YSSIZE_T nv,const T v[]);

	/*! This function copies incoming segmented array to this array. */
	inline YSRESULT CopyFrom(const YsSegmentedArray <T,BitShift> &incoming);

	/*! This function copies incoming segmented array to this array. */
	template <const unsigned int BitShift2>
	inline YSRESULT CopyFrom(const YsSegmentedArray <T,BitShift2> &incoming);

	/*! This function moves incoming segmented array to this array. */
	inline YSRESULT MoveFrom(YsSegmentedArray <T,BitShift> &incoming);

	/*! Copy operator */
	inline const YsSegmentedArray <T,BitShift> &operator=(const YsSegmentedArray <T,BitShift> &incoming);

	/*! Copy operator */
	template <const unsigned int BitShift2>
	inline const YsSegmentedArray <T,BitShift> &operator=(const YsSegmentedArray <T,BitShift2> &incoming);

	/*! Returns number of items stored in this array. */
	inline YSSIZE_T GetN(void) const;
	/*! Returns the total size of the array allocated for this array.  This may be bigger than GetN() */
	inline YSSIZE_T NAvailable(void) const;
	/*! This function return YSTRUE if 0<=id && id<GetN() */
	inline YSBOOL IsInRange(YSSIZE_T id) const;  // 2008/07/06
	/*! Access operator */
	inline T &operator[](YSSIZE_T idx);
	/*! Access operator */
	inline const T &operator[](YSSIZE_T idx) const;

	/*! Set number of items to zero, but does not free allocated sub-arrays. */
	void Clear(void);
	/*! Same as Clear.  Added for making it consistent with other classes. */
	void CleanUp(void);
	/*! Set number of items to zero and free all allocated sub-arrays. */
	void ClearDeep(void);

	/*! This function adds an item to the end of the array. */
	inline YSRESULT Append(const T &dat);

	/*! This function adds multiple items given by nItm and itm to the end of the array. 
	   \param nDat [In] Number of items to be added.
	   \param dat [In] Items to be added.
	   */
	inline YSRESULT Append(YSSIZE_T nDat,const T dat[]);

	/*! This function adds multiple items given by ary to the end of the array. 
	   */
	template <const int BitShift2>
	inline YSRESULT Append(const YsSegmentedArray <T,BitShift2> &incoming);

	/*! This function allocates the buffer good enough for the newSize.  It changes
	    NAvailable(), but does not change GetN(). */
	inline YSRESULT Resize(YSSIZE_T newSize);

	/*! This function deletes unused sub-arrays. */
	inline YSRESULT Shrink(void);

	/*! This function increases the array size by one. */
	inline YSRESULT Increment(void);

	/*! This function returns the reference to the last item in the array. */
	inline T &Last(void);

	/*! This function returns the reference to the last item in the array. */
	inline const T &Last(void) const;

	/*! This function inserts dat to the array.  dat will become idx-th item after this
	    function. */
	inline YSRESULT Insert(YSSIZE_T idx,const T &dat);

	/*! This function deletes idx-th item.  Latter items will be shifted (the order doesn't change). */
	inline YSRESULT Delete(YSSIZE_T idx);

	/*! This function copies the last item of the array to the idx-th item, and shrinks
	    the length of the array by one.  When the order of the items in the array does not
	    matter, this function quickly deletes idx-th item in the array.
	    \param idx [In] Index to the item to be deleted.
	   */
	inline YSRESULT DeleteBySwapping(YSSIZE_T idx);  // Added 2001/04/27

	/*! This function deletes the last item of the array. */
	inline YSRESULT DeleteLast(void);

	/*!  */
	inline T &GetTop(void);
	/*!  */
	inline const T& GetTop(void) const;
	/*!  */
	inline T &GetEnd(void);
	/*!  */
	inline const T& GetEnd(void) const;


	/*! To increase interoperability with STL */
	inline YSSIZE_T size(void) const
	{
		return GetN();
	}
	/*! To increase interoperability with STL */
	inline T &back(void)
	{
		return Last();
	}
	/*! To increase interoperability with STL */
	inline const T &back(void) const
	{
		return Last();
	}
};


// STL-like iterator support begin >>
template <class T,const unsigned int BitShift>
inline typename YsSegmentedArray <T,BitShift>::iterator begin(YsSegmentedArray <T,BitShift> &array)
{
	return array.begin();
}

template <class T,const unsigned int BitShift>
inline typename YsSegmentedArray <T,BitShift>::iterator end(YsSegmentedArray <T,BitShift> &array)
{
	return array.end();
}

template <class T,const unsigned int BitShift>
inline typename YsSegmentedArray <T,BitShift>::const_iterator begin(const YsSegmentedArray <T,BitShift> &array)
{
	return array.begin();
}

template <class T,const unsigned int BitShift>
inline typename YsSegmentedArray <T,BitShift>::const_iterator end(const YsSegmentedArray <T,BitShift> &array)
{
	return array.end();
}

template <class T,const unsigned int BitShift>
inline typename YsSegmentedArray <T,BitShift>::iterator rbegin(YsSegmentedArray <T,BitShift> &array)
{
	return array.rbegin();
}

template <class T,const unsigned int BitShift>
inline typename YsSegmentedArray <T,BitShift>::iterator rend(YsSegmentedArray <T,BitShift> &array)
{
	return array.rend();
}

template <class T,const unsigned int BitShift>
inline typename YsSegmentedArray <T,BitShift>::const_iterator rbegin(const YsSegmentedArray <T,BitShift> &array)
{
	return array.rbegin();
}

template <class T,const unsigned int BitShift>
inline typename YsSegmentedArray <T,BitShift>::const_iterator rend(const YsSegmentedArray <T,BitShift> &array)
{
	return array.rend();
}


template <class T,const unsigned int BitShift>
inline YsIndexRange YsSegmentedArray <T,BitShift>::AllIndex(void) const
{
	YsIndexRange allIndex(GetN());
	return allIndex;
}
// STL-like iterator support end <<


template <class T,const unsigned int BitShift>
inline YSRESULT YsSegmentedArray <T,BitShift>::Alloc(YSSIZE_T n)
{
	YSSIZE_T reqUnit=(n+SEGMENT_UNIT_SIZE-1)/SEGMENT_UNIT_SIZE;
	if(segments.GetN()<reqUnit)
	{
		YSSIZE_T n0=segments.GetN();
		if(YSOK==segments.Resize(reqUnit))
		{
			for(YSSIZE_T i=n0; i<segments.GetN(); i++)
			{
				segments[i]=new T [SEGMENT_UNIT_SIZE];
			}
			return YSOK;
		}
	}
	else if(reqUnit+1<segments.GetN())
	{
		for(YSSIZE_T i=reqUnit+1; i<segments.GetN(); i++)
		{
			delete [] segments[i];
		}
		return segments.Resize(reqUnit+1);
	}
	return YSOK;
}

template <class T,const unsigned int BitShift>
inline YsSegmentedArray <T,BitShift>::YsSegmentedArray()
{
	nv=0;
}


template <class T,const unsigned int BitShift>
inline YsSegmentedArray <T,BitShift>::YsSegmentedArray(const YsSegmentedArray <T,BitShift> &incoming)
{
	CopyFrom(incoming);
}


template <class T,const unsigned int BitShift>
template <const unsigned int BitShift2>
inline YsSegmentedArray <T,BitShift>::YsSegmentedArray(const YsSegmentedArray <T,BitShift2> &incoming)
{
	CopyFrom(incoming);
}


template <class T,const unsigned int BitShift>
inline YsSegmentedArray <T,BitShift>::YsSegmentedArray(YSSIZE_T nv,const T v[])
{
	Set(nv,v);
}


template <class T,const unsigned int BitShift>
inline YsSegmentedArray <T,BitShift>::~YsSegmentedArray()
{
	ClearDeep();
}

template <class T,const unsigned int BitShift>
inline YSRESULT YsSegmentedArray <T,BitShift>::Set(YSSIZE_T nv,const T v[])
{
	if(0==nv)
	{
		Clear();
		return YSOK;
	}
	else
	{
		if(YSOK==Resize(nv))
		{
			if(NULL!=v)
			{
				for(YSSIZE_T i=0; i<nv; i++)
				{
					(*this)[i]=v[i];
				}
			}
			return YSOK;
		}
	}
	return YSERR;
}

template <class T,const unsigned int BitShift>
inline YSRESULT YsSegmentedArray <T,BitShift>::CopyFrom(const YsSegmentedArray <T,BitShift> &incoming)
{
	if(YSOK==Resize(incoming.GetN()))
	{
		for(YSSIZE_T i=0; i<incoming.GetN(); i++)
		{
			(*this)[i]=incoming[i];
		}
		return YSOK;
	}
	return YSERR;
}


template <class T,const unsigned int BitShift>
template <const unsigned int BitShift2>
inline YSRESULT YsSegmentedArray <T,BitShift>::CopyFrom(const YsSegmentedArray <T,BitShift2> &incoming)
{
	if(YSOK==Resize(incoming.GetN()))
	{
		for(YSSIZE_T i=0; i<incoming.GetN(); i++)
		{
			(*this)[i]=incoming[i];
		}
		return YSOK;
	}
	return YSERR;
}

template <class T,const unsigned int BitShift>
inline YSRESULT YsSegmentedArray <T,BitShift>::MoveFrom(YsSegmentedArray <T,BitShift> &incoming)
{
	this->nv=incoming.nv;
	this->segments.MoveFrom(incoming.segments);
	incoming.nv=0;
	incoming.segments.CleanUp();
	return YSOK;
}



template <class T,const unsigned int BitShift>
inline const YsSegmentedArray <T,BitShift> &YsSegmentedArray <T,BitShift>::operator=(const YsSegmentedArray <T,BitShift> &incoming)
{
	CopyFrom(incoming);
	return *this;
}


template <class T,const unsigned int BitShift>
template <const unsigned int BitShift2>
inline const YsSegmentedArray <T,BitShift> &YsSegmentedArray <T,BitShift>::operator=(const YsSegmentedArray <T,BitShift2> &incoming)
{
	CopyFrom(incoming);
	return *this;
}

template <class T,const unsigned int BitShift>
inline YSSIZE_T YsSegmentedArray <T,BitShift>::GetN(void) const
{
	return nv;
}


template <class T,const unsigned int BitShift>
inline YSSIZE_T YsSegmentedArray <T,BitShift>::NAvailable(void) const
{
	return SEGMENT_UNIT_SIZE*segments.GetN();
}

template <class T,const unsigned int BitShift>
inline YSBOOL YsSegmentedArray <T,BitShift>::IsInRange(YSSIZE_T id) const
{
	if(0<=id && id<GetN())
	{
		return YSTRUE;
	}
	return YSFALSE;
}

template <class T,const unsigned int BitShift>
inline T &YsSegmentedArray <T,BitShift>::operator[](YSSIZE_T idx)
{
	const YSSIZE_T seg=(idx>>BitShift);
	const YSSIZE_T offset=(idx&((1<<BitShift)-1));
	return segments[seg][offset];
}


template <class T,const unsigned int BitShift>
inline const T &YsSegmentedArray <T,BitShift>::operator[](YSSIZE_T idx) const
{
	const YSSIZE_T seg=(idx>>BitShift);
	const YSSIZE_T offset=(idx&((1<<BitShift)-1));
	return segments[seg][offset];
}

template <class T,const unsigned int BitShift>
void YsSegmentedArray <T,BitShift>::Clear(void)
{
	nv=0;
}

template <class T,const unsigned int BitShift>
void YsSegmentedArray <T,BitShift>::CleanUp(void)
{
	Clear();
}

template <class T,const unsigned int BitShift>
void YsSegmentedArray <T,BitShift>::ClearDeep(void)
{
	for(YSSIZE_T i=0; i<segments.GetN(); i++)
	{
		T *ptr=segments[i];
		delete [] ptr;
	}
	segments.ClearDeep();
	nv=0;
}


template <class T,const unsigned int BitShift>
inline YSRESULT YsSegmentedArray <T,BitShift>::Append(const T &dat)
{
	if(YSOK==Increment())
	{
		(*this)[nv-1]=dat;
		return YSOK;
	}
	return YSERR;
}


template <class T,const unsigned int BitShift>
inline YSRESULT YsSegmentedArray <T,BitShift>::Append(YSSIZE_T nDat,const T dat[])
{
	const YSSIZE_T n0=GetN();
	Resize(GetN()+nDat);
	for(YSSIZE_T i=0; i<nDat; i++)
	{
		(*this)[n0+i]=dat[i];
	}
	return YSOK;
}


template <class T,const unsigned int BitShift>
template <const int BitShift2>
inline YSRESULT YsSegmentedArray <T,BitShift>::Append(const YsSegmentedArray <T,BitShift2> &incoming)
{
	const YSSIZE_T n0=GetN();
	Resize(GetN()+incoming.GetN());
	for(YSSIZE_T i=0; i<incoming.GetN(); i++)
	{
		(*this)[n0+i]=incoming[i];
	}
	return YSOK;
}

template <class T,const unsigned int BitShift>
inline YSRESULT YsSegmentedArray <T,BitShift>::Resize(YSSIZE_T newSize)
{
	if(YSOK==Alloc(newSize))
	{
		nv=newSize;
		return YSOK;
	}
	return YSERR;
}

template <class T,const unsigned int BitShift>
inline YSRESULT YsSegmentedArray <T,BitShift>::Shrink(void)
{
	YSSIZE_T reqUnit=(nv+SEGMENT_UNIT_SIZE-1)/SEGMENT_UNIT_SIZE;
	for(YSSIZE_T i=reqUnit; i<segments.GetN(); i++)
	{
		delete [] segments[i];
	}
	segments.Resize(reqUnit);
	return YSOK;
}


template <class T,const unsigned int BitShift>
inline YSRESULT YsSegmentedArray <T,BitShift>::Increment(void)
{
	return Resize(GetN()+1);
}

template <class T,const unsigned int BitShift>
inline T &YsSegmentedArray <T,BitShift>::Last(void)
{
	return GetEnd();
}

template <class T,const unsigned int BitShift>
inline const T &YsSegmentedArray <T,BitShift>::Last(void) const
{
	return GetEnd();
}

template <class T,const unsigned int BitShift>
YSRESULT YsSegmentedArray <T,BitShift>::Insert(YSSIZE_T idx,const T &dat)
{
	if(YSOK==Increment())
	{
		for(YSSIZE_T i=GetN()-1; idx<i; i--)
		{
			(*this)[i]=(*this)[i-1];
		}
		(*this)[idx]=dat;
		return YSOK;
	}
	return YSERR;
}

template <class T,const unsigned int BitShift>
YSRESULT YsSegmentedArray <T,BitShift>::Delete(YSSIZE_T idx)
{
	for(YSSIZE_T i=idx; idx<GetN()-1; idx++)
	{
		(*this)[i]=(*this)[i+1];
	}
	Resize(GetN()-1);
	return YSOK;
}

template <class T,const unsigned int BitShift>
YSRESULT YsSegmentedArray <T,BitShift>::DeleteBySwapping(YSSIZE_T idx)
{
	(*this)[idx]=GetEnd();
	Resize(GetN()-1);
	return YSOK;
}

template <class T,const unsigned int BitShift>
inline YSRESULT YsSegmentedArray <T,BitShift>::DeleteLast(void)
{
	Resize(GetN()-1);
	return YSOK;
}

template <class T,const unsigned int BitShift>
inline T &YsSegmentedArray <T,BitShift>::GetTop(void)
{
	return (*this)[0];
}


template <class T,const unsigned int BitShift>
inline const T& YsSegmentedArray <T,BitShift>::GetTop(void) const
{
	return (*this)[0];
}


template <class T,const unsigned int BitShift>
inline T &YsSegmentedArray <T,BitShift>::GetEnd(void)
{
	return (*this)[nv-1];
}


template <class T,const unsigned int BitShift>
inline const T& YsSegmentedArray <T,BitShift>::GetEnd(void) const
{
	return (*this)[nv-1];
}

/* } */
#endif
