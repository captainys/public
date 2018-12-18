/* ////////////////////////////////////////////////////////////

File Name: ysarray.h
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

#ifndef YSARRAY_IS_INCLUDED
#define YSARRAY_IS_INCLUDED

/*! \file */

#include <assert.h>
#include <cstddef>
#include <initializer_list>

#include "ysdef.h"
#include "ysexception.h"
#include "ysprintf.h"
#include "ysmergesort.h"
#include "yslist.h"
#include "ysmath.h"
#include "ysrange.h"
#include "ysarraymask.h"
#include "ysindexiterator.h"

/*! This function returns an item in a cyclic array. */
template <class T>
T &YsGetCyclic(YSSIZE_T n,T ary[],YSSIZE_T idx)
{
	if(0<n)
	{
		// By the way, is it guaranteed that (-3)%7 is always -3, not 4?  Common sense tells it is -3, but I'm not sure if it was a guaranteed behavior in C++.
		idx=idx%n;
		if(0>idx)
		{
			idx+=n;
		}
		return ary[idx];
	}
	else
	{
		T *ptr=NULL;
		return *ptr;
	}
}

/*! This function returns an item in a constant cyclic array. */
template <class T>
const T &YsGetCyclic(YSSIZE_T n,const T ary[],YSSIZE_T idx)
{
	if(0<n)
	{
		idx=idx%n;
		if(0>idx)
		{
			idx+=n;
		}
		return ary[idx];
	}
	else
	{
		const T *ptr=NULL;
		return *ptr;
	}
}

/*! A template class for easily making a pair of two objects.
    */
template <class T1,class T2>
class YsPair
{
public:
	/*! Member of T1 type. */
	T1 a;
	/*! Member of T2 type. */
	T2 b;
};

/*! A template class for easily making a set of three objects of different types.
    */
template <class T1,class T2,class T3>
class YsTrio
{
public:
	/*! Member of T1 type. */
	T1 a;
	/*! Member of T2 type. */
	T2 b;
	/*! Member of T3 type. */
	T3 c;
};

/*! A template class for easily making a set of four objects of different types.
    */
template <class T1,class T2,class T3,class T4>
class YsQuartet
{
public:
	/*! Member of T1 type. */
	T1 a;
	/*! Member of T2 type. */
	T2 b;
	/*! Member of T3 type. */
	T3 c;
	/*! Member of T4 type. */
	T4 d;
};



////////////////////////////////////////////////////////////

/*! A static-array class.  
*/
template <class T,const YSSIZE_T _size>
class YsStaticArray
{
private:
	T dat[_size];

public:
	/*! Operator for accessing the array. */
	inline const T &operator[](YSSIZE_T idx) const
	{
		return dat[idx];
	}

	/*! Operator for accessing the array. */
	inline T &operator[](YSSIZE_T idx)
	{
		return dat[idx];
	}

	/*! Cast operator that returns constant pointer to the array. */
	operator const T *() const
	{
		return dat;
	}

	/*! Cast operator that returns non-constant pointer to the array. */
	operator T *()
	{
		return dat;
	}

	const YSSIZE_T GetN(void) const
	{
		return _size;
	}

	const T &GetCyclic(YSSIZE_T idx) const
	{
		return YsGetCyclic(_size,dat,idx);
	}
	T &GetCyclic(YSSIZE_T idx)
	{
		return YsGetCyclic(_size,dat,idx);
	}

// STL-like iterator support begin >>
public:
	typedef YsIndexIterator <YsStaticArray<T,_size>,T> iterator;
	typedef YsIndexIterator <const YsStaticArray<T,_size>,const T> const_iterator;
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

	/*! This class enables range-based for using an index.  It can be written as:
	    YsArray <T> array;
	    for(auto idx : array.AllIndex())
	    {
	        // Do something for array[idx]
	    }
	    Useful when something needs to be done before and after an array item. */
	inline YsIndexRange AllIndex(void) const;

	/*! This class enables range-based for using an index.  It can be written as:
	    YsArray <T> array;
	    for(auto idx : array.ReverseIndex())
	    {
	        // Do something for array[idx]
	    }
	    Useful when something needs to be done before and after an array item. 
	    Variable idx starts from GetN()-1 and go all the way to 0.
	*/
	inline YsReverseIndexRange ReverseIndex(void) const;

// STL-like iterator support end <<

	/*! For better interoperability with STL */
	inline YSSIZE_T size(void) const
	{
		return GetN();
	}
	/*! For better interoperability with STL */
	inline T& at(YSSIZE_T idx)
	{
		return (*this)[idx];
	}
	/*! For better interoperability with STL */
	inline const T& at(YSSIZE_T idx) const
	{
		return (*this)[idx];
	}
};

// STL-like iterator support begin >>
template <class T,const YSSIZE_T _size>
inline typename YsStaticArray <T,_size>::iterator begin(YsStaticArray <T,_size> &array)
{
	return array.begin();
}

template <class T,const YSSIZE_T _size>
inline typename YsStaticArray <T,_size>::iterator end(YsStaticArray <T,_size> &array)
{
	return array.end();
}

template <class T,const YSSIZE_T _size>
inline typename YsStaticArray <T,_size>::const_iterator begin(const YsStaticArray <T,_size> &array)
{
	return array.begin();
}

template <class T,const YSSIZE_T _size>
inline typename YsStaticArray <T,_size>::const_iterator end(const YsStaticArray <T,_size> &array)
{
	return array.end();
}

template <class T,const YSSIZE_T _size>
inline typename YsStaticArray <T,_size>::iterator rbegin(YsStaticArray <T,_size> &array)
{
	return array.rbegin();
}

template <class T,const YSSIZE_T _size>
inline typename YsStaticArray <T,_size>::iterator rend(YsStaticArray <T,_size> &array)
{
	return array.rend();
}

template <class T,const YSSIZE_T _size>
inline typename YsStaticArray <T,_size>::const_iterator rbegin(const YsStaticArray <T,_size> &array)
{
	return array.rbegin();
}

template <class T,const YSSIZE_T _size>
inline typename YsStaticArray <T,_size>::const_iterator rend(const YsStaticArray <T,_size> &array)
{
	return array.rend();
}

template <class T,const YSSIZE_T _size>
inline YsIndexRange YsStaticArray <T,_size>::AllIndex(void) const
{
	YsIndexRange allIndex(GetN());
	return allIndex;
}

template <class T,const YSSIZE_T _size>
inline YsReverseIndexRange YsStaticArray <T,_size>::ReverseIndex(void) const
{
	YsReverseIndexRange allIndex(GetN());
	return allIndex;
}

// STL-like iterator support end <<
////////////////////////////////////////////////////////////

#include "ysarraymask.h"

////////////////////////////////////////////////////////////


/*! \deprecated This class is deprecated.  Kept only for backward compatibility.
    Use YsStaticArray instead.
    */
template <class T,const int _size>
class YsFixedLengthArray
{
public:
	/*! Array buffer */
	T dat[_size];

	/*! Operator for accessing the array. */
	inline const T &operator[](int idx) const
	{
		return dat[idx];
	}

	/*! Operator for accessing the array. */
	inline T &operator[](int idx)
	{
		return dat[idx];
	}

	/*! Cast operator that returns constant pointer to the array. */
	operator const T *() const
	{
		return dat;
	}

	/*! Cast operator that returns non-constant pointer to the array. */
	operator T *()
	{
		return dat;
	}
};

/*! \deprecated This function is deprecated.  Kept only for backward compatibility. */
template <class T,const int size>
inline int operator==(const YsFixedLengthArray <T,size> &a,const YsFixedLengthArray <T,size> &b)
{
	int i;
	for(i=0; i<size; i++)
	{
		if(a[i]!=b[i])
		{
			return 0;
		}
	}
	return 1;
}

////////////////////////////////////////////////////////////

// \cond
template <class T,const int MinLen>
class YsArrayPreAlloc
{
private:
	T pre[MinLen];
public:
	inline T *Prealloc(void)
	{
		return pre;
	}
	inline const T *Prealloc(void) const
	{
		return pre;
	}
};

template <class T>
class YsArrayPreAlloc <T,0>
{
public:
	inline T *Prealloc(void)
	{
		return NULL;
	}
	inline const T *Prealloc(void) const
	{
		return NULL;
	}
};
// \endcond


/*! A variable-length (doubling) array class.

    This class can take a template parameter MinimumLength, which is the length of the 
    statically taken interaal array.  As long as the length of the array stays within
    MinimumLength, this class will not dynamically allocate a memory block from heap.
    This internal array will not be used when the length of the array grows beyond 
    MinimuLength.

    Use of internal array substancially reduces the necessity of the dynamic memory allocation.
    For example, a typical polygonal model consists mostly of triangles and quadrilaterals.
    Therefore, most of the polygons needs only an array of length four to store indices to the
    vertices.  However, occasionally a polygon uses more than vertices.  In such conditions,
    you can set MinimumLength to 4, then in most polygons, a memory chunk will not be taken
    from heap.  And only a few polygons takes a memory chunk from heap and waste memory 
    for internal array, but the waste is small, and performance gain by avoiding dynamic 
    memory allocation is substantial.

    The functionality of YsArray class is similar to the vector class of the standard-template library.
    Major difference is the MinimumLength.

    NOTE: Currently SizeType is YSSIZE_T by default, but will be changed to size_t in the near future to fully
    adapt to 64-bit environment.  Also indexes will be changed from int to SizeType.  Not a practical
    concern until we really start using array indexes beyond 32bit boundary.

    \tparam T Type of the items stored in the array
    \tparam MinimumLength Length of the statically-allocated internal array buffer
    \tparam SizeType Type of the variable used for specifying an index
    \sa YsString

    YsArray class can be casted to YsConstArrayMask or YsArrayMask with near-zero performance penalty.

    For writing a function that takes this class, make it to receive YsConstArrayMask <T>, or
    YsArrayMask <T> so that the function can take an array regardless of the template parameter MinimumLength.
    YsArray <T,MinimumLength,SizeType> can be casted to YsArrayMask <T> regardless of MinimumLength and SizeType.
    const YsArray <T,MInimumLength,SizeType> can be casted to YsConstArrayMask <T> regardless of MinimumLength and SizeType.

    std::vector<T> and YsTightArray<T,SizeType> can be copied to YsArray <T,MinimumLength> via YsConstArrayMask<T>.  
    The copy constructors and operators from std::vector and YsTightArray have been removed on 2017/05/17 because the conversion is taken care by YsConstArrayMask<T>.

    */
template <class T,const int MinimumLength=1,class SizeType=YSSIZE_T>
class YsArray : private YsArrayPreAlloc <T,MinimumLength>
{
   // CAUTION!
// Whenever add an item, do not forget to modify operator=

// Due to g++'s bug, this using-statement is required. >>
private:
	using YsArrayPreAlloc <T,MinimumLength>::Prealloc;
// Due to g++'s bug, this using-statement is required. <<

/*
  Cleaner implementation is the following:

	YsArrayPreAlloc <T,MinimumLength> pre;
	inline T *Prealloc(void)
	{
		return pre.Prealloc();
	}
	inline const T *Prealloc(void) const
	{
		return pre.Prealloc();
	}

  This will not contaminate the class hierarchy.  
  It also avoids g++'s bug that requires adding an unnecessary using statement.

  However, both Visual C++ and g++ adds sizeof(void *) bytes even when MinimumLength=0, and make no point of allowing zero-MinimumLength.
*/


protected:
	SizeType nv,nAvailable;
	T *vv;

// STL-like iterator support begin >>
public:
	typedef YsIndexIterator <YsArray<T,MinimumLength,SizeType>,T> iterator;
	typedef YsIndexIterator <const YsArray<T,MinimumLength,SizeType>,const T> const_iterator;
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


	/*! Returns a subset of this array, (*this)[top]...(*this)[top+length-1].
	    When length is -1, returns (*this)[top]...(*this)->Last().
	*/
	YsConstArrayMask <T> Subset(YSSIZE_T top,YSSIZE_T length=-1) const
	{
		if(GetN()<=top)
		{
			return YsConstArrayMask <T> (0,nullptr);
		}

		if(0>length)
		{
			return YsConstArrayMask <T> (GetN()-top,GetArray()+top);
		}
		else
		{
			if(GetN()<top+length)
			{
				length=GetN()-top;
			}
			return YsConstArrayMask <T> (length,GetArray()+top);
		}
	}

	/*! Returns a subset of this array, (*this)[top]...(*this)[top+length-1].
	    When length is -1, returns (*this)[top]...(*this)->Last().
	*/
	YsArrayMask <T> Subset(YSSIZE_T top,YSSIZE_T length=-1)
	{
		if(GetN()<=top)
		{
			return YsArrayMask <T> (0,nullptr);
		}

		if(0>length)
		{
			return YsArrayMask <T> (GetN()-top,GetEditableArray()+top);
		}
		else
		{
			if(GetN()<top+length)
			{
				length=GetN()-top;
			}
			return YsArrayMask <T> (length,GetEditableArray()+top);
		}
	}


	/*! This class enables range-based for using an index.  It can be written as:
	    YsArray <T> array;
	    for(auto idx : array.AllIndex())
	    {
	        // Do something for array[idx]
	    }
	    Useful when something needs to be done before and after an array item. */
	inline YsIndexRange AllIndex(void) const;

	/*! This class enables range-based for using an index.  It can be written as:
	    YsArray <T> array;
	    for(auto idx : array.ReverseIndexIndex())
	    {
	        // Do something for array[idx]
	    }
	    Useful when something needs to be done before and after an array item. */
	inline YsReverseIndexRange ReverseIndex(void) const;
// STL-like iterator support end <<

public:
	/*! Default constructor.  Creates an empty array. */
	YsArray();

	/*! A constructor that makes an array from a raw C++ array. 
	    \param n [In] Number of array items
	    \param v [In] Raw C++ array to be copied to this array
	    */
	YsArray(SizeType n,const T v[]);

	/*! A constructor that makes an array of the given length N.
	    When T is uncopyable, for example when T=std::thread,
	        YsArray thr(8,nullptr);
	    will give an error because it end up compiling copying part of the code.

	    I want to put N the first parameter, but I have so many code already using like:
	        YsArray xyz(100,NULL);
	    and I end up getting thousands of errors because the second parameter can match both std::nullptr_t and const T v[].

	    As a second-best option, I put std::nullptr_t as the first parameter.
	*/
	YsArray(std::nullptr_t,SizeType N);

	/*! Default copy constructor that creates an YsArray object from another YsArray object.
	    */
	YsArray(const YsArray <T,MinimumLength,SizeType> &from);

	/*! Copy constructor for copying from a YsArray with different MinimuLength or SizeType.
	    */
	YsArray(const YsConstArrayMask <T> &from);

	/*! Copy constructor from an initierlizer list. */
	YsArray(std::initializer_list <T> initList);

	/*! Default move constructor that creates an YsArray object from another YsArray object.
	    */
	YsArray(YsArray <T,MinimumLength,SizeType> &&from);

	/*! A move constructor that creates an YsArray object from another YsArray object of different MinimuLength.
	    This copy constructor is supposed to cover the case MinimumLength2==MinimumLength.  However, it seems
	    that C++ deals with the case where the input parameter has the exactly the same type, and this
	    constructo may not be called MinimumLength2==MinimumLength.  (At least that is what happened in 
	    Visual Studio 2008.  A separate default constructor is written just to be absolutely sure that
	    an appropriate constructor is called.
	    */
	template <const int MinimumLength2,class SizeType2>
	YsArray(YsArray <T,MinimumLength2,SizeType2> &&from);

	/*! This constructor copies contents of any storage class that is conformal to the range-based for. */
	template <class StorageClass>
	YsArray(const StorageClass &incoming);


	/*! Default destructor.
	    */
	~YsArray();

	/*! Default copy operator that safely copies object from the incoming YsArray.
	    */
	YsArray <T,MinimumLength,SizeType> &operator=(const YsArray <T,MinimumLength,SizeType> &from);

	/*! Default copy operator that safely copies object from the incoming storage.
	    */
	template <class StorageClass>
	YsArray <T,MinimumLength,SizeType> &operator=(const StorageClass &from);

	/*! Copy operator from an initializer_list. */
	YsArray <T,MinimumLength,SizeType> &operator=(std::initializer_list <T> initList);

	/*! A copy operator that copies from a YsArray class with different MinimumLength.
	    \tparam MinimumLength2 Internal array length of the incoming array.
	    */
	YsArray <T,MinimumLength,SizeType> &operator=(const YsConstArrayMask <T> &from);


	/*! \deprecated This function is deprecated.  Left for backward compatibility only. */
	YsArray <T,MinimumLength,SizeType> &operator=(const YsList <T> *from);


	/*! Default move operator that safely copies object from the incoming YsArray.
	    */
	YsArray <T,MinimumLength,SizeType> &operator=(YsArray <T,MinimumLength,SizeType> &&from);

	/*! A copy operator that copies from a YsArray class with different MinimumLength.
	    \tparam MinimumLength2 Internal array length of the incoming array.
	    */
	template <const int MinimumLength2,class SizeType2>
	YsArray <T,MinimumLength,SizeType> &operator=(YsArray <T,MinimumLength2,SizeType2> &&from);


	/*! Adds two arrays.
	*/
	YsArray <T,MinimumLength,SizeType> operator+(const YsConstArrayMask <T> &toAdd) const;


	/*! This function copies contents from incoming to this array when possible.
	    This function will return a reference to *this. */
	YsArray <T,MinimumLength,SizeType> &CopyFrom(const YsConstArrayMask <T> &from);

	/*! This function moves contents from incoming to this array when possible.
	    If it is not possible, it will fall back to copy. 
	    This function will return a reference to *this. */
	YsArray <T,MinimumLength,SizeType> &MoveFrom(YsArray <T,MinimumLength,SizeType> &incoming);

	/*! This function moves contents from incoming to this array when possible.
	    If it is not possible, it will fall back to copy. 
	    This function will return a reference to *this. */
	template <const int MinimumLength2,class SizeType2>
	YsArray <T,MinimumLength,SizeType> &MoveFrom(YsArray <T,MinimumLength2,SizeType2> &incoming);


	/*! An operator that allows constant access to the array items. */
	inline const T &operator[](YSSIZE_T idx) const;
	// Memo: idx must have YSSIZE_T, or parameter deduction gets confused when SizeType is a smaller integer.

	/*! An operator that allows non-constant access to the array items. */
	inline T &operator[](YSSIZE_T idx);
	// Memo: idx must have YSSIZE_T, or parameter deduction gets confused when SizeType is a smaller integer.

	/*! A cast operator that returns constant pointer to the array. */
	inline operator const T *() const;

	/*! A cast operator that returns a YsConstArrayMask.  */
	inline operator const YsConstArrayMask <T> () const;

	/*! A cast operator that returns a YsArrayMask.  */
	inline operator YsArrayMask <T> ();

	/*! A cast operator that returns non-constant pointer to the array. */
	inline operator T *();

	/*! This function returns the length of the array. */
	inline SizeType GetN(void) const;

	/*! This function adds an item to the end of the array. */
	inline YSRESULT Add(const T &dat);

	/*! This function adds an item to the end of the array. */
	inline YSRESULT Add(T &&dat);

	/*! Same as Add.  Left for backward compatibility.  Use Add instead for the new naming convention. */
	inline YSRESULT Append(const T &dat);

	/*! Same as Add.  Left for backward compatibility.  Use Add instead for the new naming convention. */
	inline YSRESULT Append(T &&dat);

	/*! This function adds an item to the end, and returns the reference to the new item.
	    Note that the instance of the new item may be re-cycled.  The new instance may not be at the initial state. */
	inline T &New(void);

	/*! This function returns a constant reference to the first item of the array. */
	inline const T &GetTop(void) const;

	/*! This function returns a non-constant reference to the last item of the array. */
	inline T &GetEnd(void);

	/*! This function returns a constant reference to the last item of the array. */
	inline const T &GetEnd(void) const;

	/*! Alias for GetEnd. */
	inline T &Last(void);

	/*! Alias for GetEnd. */
	inline const T &Last(void) const;

	/*! This function adds multiple items given by nItm and itm to the end of the array. 
	   \param nDat [In] Number of items to be added.
	   \param dat [In] Items to be added.
	   */
	YSRESULT Add(SizeType nDat,const T dat[]);

	/*! Same as Add.  Left for backward compatibility.  Use Add instead for the new naming convention. */
	YSRESULT Append(SizeType nDat,const T dat[]);

	/*! This function adds multiple items given by ary to the end of the array. 
	   */
	YSRESULT Add(const YsConstArrayMask <T> &ary);

	/*! Same as Add.  Left for backward compatibility.  Use Add instead for the new naming convention. */
	YSRESULT Append(const YsConstArrayMask <T> &ary);


	/*! This function copies the last item of the array to the idx-th item, and shrinks
	    the length of the array by one.  When the order of the items in the array does not
	    matter, this function quickly deletes idx-th item in the array.
	    \param idx [In] Index to the item to be deleted.
	   */
	YSRESULT DeleteBySwapping(YSSIZE_T idx);  // Added 2001/04/27

	/*! This function deletes all items that is same as the incoming.
	    This function will not preserve the order. */
	YSRESULT DeleteItemBySwapping(const T &incoming);

	/*! This function deletes all items that is same as the incoming.
	    This function will preserve the order. */
	YSRESULT DeleteItem(const T &incoming);

	/*! This function deletes the last item of the array. */
	YSRESULT DeleteLast(void);  // Added 2006/03/22

	/*! This function increases the length of the array by one.  The content of the last item
	    is undetermined immediately after this function. */
	YSRESULT Increment(void);   // Added 2009/04/08

	/*! This function clears the array.  The length of the array becomes zero. This function
	    does not clear the internal buffer.  To make sure internal buffer is deleted, use ClearDeep
	    function instead. */
	YSRESULT Clear(void);       // Added 2009/04/09

	/*! Same as Clear.  Added for making it consistent with other classes. */
	YSRESULT CleanUp(void);

	/*! This function clears the array.  The length of the array becomes zero.  Internal buffer
	    will also be deleted.*/
	YSRESULT ClearDeep(void);

	/*! This function makes an array with length of one, and copies dat to the array. 
	    Contents of the array before this function is lost. */
	YSRESULT Set(const T &dat); // 2004/08/23

	/*! This function copies the incoming value to all items in the array. */
	YSRESULT SetAll(const T &incoming); // 2015/02/24

	/*! This function makes an array with length of nv, and copies dat[0] to dat[nDat-1] to the array. 
	    \param nDat [In] New length of the array.
	    \param dat [In] Items to be copied to the array.
	    */
	YSRESULT Set(SizeType nDat,const T dat[]);

	/*! This function copies a value to idx-th item in the array.
	    \param idx [In] Location of the item that receives a new value
	    \param dat [In] Incoming value 
	    */
	inline YSRESULT SetItem(SizeType idx,const T &dat);

	/*! This function creates an array with length of one, and copies the incoming item to the array. */
	YSRESULT MakeUnitLength(const T &dat);

	/*! Same as GetN() */
	inline SizeType GetNumItem(void) const;

	/*! This function returns a constant reference to the idx-th item in the array. */
	inline const T &v(SizeType idx) const;

	/*! This function returns a non-constant reference to the idx-th item in the array. */
	inline T &GetEditableItem(SizeType idx);

	/*! This function returns a constant reference to the idx-th item in the array. */
	inline const T &GetItem(SizeType idx) const;

	/*! This function returns a reference to an item assuming the array is a cyclic array. */
	inline T &GetCyclic(SizeType idx);

	/*! This function returns a const reference to an item assuming the array is a cyclic array. */
	inline const T &GetCyclic(SizeType idx) const;

	/*! This function returns an index for cyclic access. */
	inline SizeType GetCyclicIndex(SizeType idx) const;

	/*! This function returns a constant pointer to the array.  
	    It is useful when casting operator occasionally becomes ambiguous. */
	inline const T *GetArray(void) const;

	/*! This function returns a non-constant pointer to the array. 
	    It is useful when casting operator occasionally becomes ambiguous. */
	inline T *GetEditableArray(void);

	/*! This function is deprecated.  Use GetTop instead.  */
	inline T &GetEditableTopItem(void);

	/*! This function is deprecated.  Use GetTop instead.  */
	inline const T &GetTopItem(void) const;

	/*! This function is deprecated.  Use GetEnd instead.  */
	inline T &GetEditableEndItem(void);

	/*! This function is deprecated.  Use GetEnd instead.  */
	inline const T &GetEndItem(void) const;

	/*! This function checks the length of internal buffer.  */
	inline SizeType GetNAvailable(void) const;

	/*! This function checks the length of the unused portion of the internal buffer.  */
	inline SizeType GetNUnused(void) const;

	/*! This function return YSTRUE if 0<=id && id<GetN() */
	inline YSBOOL IsInRange(SizeType id) const;  // 2008/07/06

	/*! This function return YSTRUE if the array includes dat.  If the compare operator
	    is not defined for class T, the compiler will give an error. */
	inline YSBOOL IsIncluded(const T &dat) const; // 2010/01/20

	/*! If dat is included in the array, it returns an index to the dat.  Otherwise, it returns -1. */
	inline YSSIZE_T Find(const T &dat) const; // 2017/07/06

	/*! This function returns YSTRUE if the given object has a common item with this array. */
	template <class S>
	inline YSBOOL HasCommonItem(const S &from) const;

	/*! This function changes the length of the array.  If the new length is longer than
	    the current length, items beyond the current length will not be initialized. */
	YSRESULT Resize(SizeType newSize);

	/*! This function resizes the length of the array.  
	    However, the previous content of the array won't be transferred to the new array.
	    Using Resize function will give an error if T is an uncopyable class, such as std::thread.
	    ResizeNoCopy will just resizes it, but because it does not transfer the previous content,
	    it can avoid such an error.
	    Drawback is if the array is already populated, the array content is lost.
	    Therefore, this function should be only used once before the array is populated.
	*/
	YSRESULT ResizeNoCopy(SizeType newSize);

	/*! Same as Append function. */
	inline YSRESULT AppendItem(const T &dat);

	/*! This function inserts dat to the array.  dat will become idx-th item after this
	    function. */
	YSRESULT Insert(SizeType idx,const T &dat);

	/*! This function deletes idx-th item.  Latter items will be shifted (the order doesn't change). */
	YSRESULT Delete(SizeType idx);

	/*! This function numItem items starting from idxFrom-th item.  Latter items will be shifted (the order doesn't change). */
	YSRESULT Delete(SizeType idxFrom,SizeType numItem);

	/*! This function inverts the order of the array. */
	YSRESULT Invert(void);

	/*! This function swaps idx1-th and idx2-th items in the array. */
	YSRESULT Swap(SizeType idx1,SizeType idx2);

// \cond
public:
	T *DetachPointer_InternalUseOnlyDontUseItFromOutsideYsArrayClass(SizeType &nv,SizeType &nAvailable);

protected:
	inline YSRESULT Alloc(SizeType n,YSBOOL cpy);
	inline YSRESULT AllocWithoutCopy(SizeType n);
	inline YSRESULT Shrink(void);
// \endcond

public:
	/*! For better interoperability with STL */
	inline YSSIZE_T size(void) const
	{
		return GetN();
	}
	/*! For better interoperability with STL */
	inline void resize(YSSIZE_T newSize)
	{
		Resize(newSize);
	}
	/*! For better interoperability with STL */
	inline void clear(void)
	{
		CleanUp();
	}
	/*! For better interoperability with STL */
	inline void push_back(const T &value)
	{
		Add(value);
	}
	/*! For better interoperability with STL */
	inline void push_back(T &&value)
	{
		Add(value);
	}
	/*! For better interoperability with STL */
	inline void pop_back(void)
	{
		DeleteLast();
	}
	/*! For better interoperability with STL */
	inline const T &back(void) const
	{
		return Last();
	}
	/*! For better interoperability with STL */
	inline T &back(void)
	{
		return Last();
	}
	/*! For better interoperability with STL */
	inline const T &front(void) const
	{
		return vv[0];
	}
	/*! For better interoperability with STL */
	inline T &front(void)
	{
		return vv[0];
	}
	/*! For better interoperability with STL */
	inline const T *data(void) const
	{
		return GetArray();
	}
	/*! For better interoperability with STL */
	inline T *data(void)
	{
		return GetEditableArray();
	}
	/*! For better interoperability with STL */
	inline bool empty(void) const
	{
		return (0==GetN());
	}
	/*! For better interoperability with STL */
	inline void swap(YsArray <T,MinimumLength,SizeType> &from)
	{
		YsArray <T,MinimumLength,SizeType> tmp;
		tmp.MoveFrom(*this);
		this->MoveFrom(from);
		from.MoveFrom(tmp);
	}
	/*! For better interoperability with STL */
	inline T& at(YSSIZE_T idx)
	{
		return (*this)[idx];
	}
	/*! For better interoperability with STL */
	inline const T& at(YSSIZE_T idx) const
	{
		return (*this)[idx];
	}
};

// STL-like iterator support begin >>
template <class T,const int MinimumLength,class SizeType>
inline typename YsArray <T,MinimumLength,SizeType>::iterator begin(YsArray <T,MinimumLength,SizeType> &array)
{
	return array.begin();
}

template <class T,const int MinimumLength,class SizeType>
inline typename YsArray <T,MinimumLength,SizeType>::iterator end(YsArray <T,MinimumLength,SizeType> &array)
{
	return array.end();
}

template <class T,const int MinimumLength,class SizeType>
inline typename YsArray <T,MinimumLength,SizeType>::const_iterator begin(const YsArray <T,MinimumLength,SizeType> &array)
{
	return array.begin();
}

template <class T,const int MinimumLength,class SizeType>
inline typename YsArray <T,MinimumLength,SizeType>::const_iterator end(const YsArray <T,MinimumLength,SizeType> &array)
{
	return array.end();
}

template <class T,const int MinimumLength,class SizeType>
inline typename YsArray <T,MinimumLength,SizeType>::iterator rbegin(YsArray <T,MinimumLength,SizeType> &array)
{
	return array.rbegin();
}

template <class T,const int MinimumLength,class SizeType>
inline typename YsArray <T,MinimumLength,SizeType>::iterator rend(YsArray <T,MinimumLength,SizeType> &array)
{
	return array.rend();
}

template <class T,const int MinimumLength,class SizeType>
inline typename YsArray <T,MinimumLength,SizeType>::const_iterator rbegin(const YsArray <T,MinimumLength,SizeType> &array)
{
	return array.rbegin();
}

template <class T,const int MinimumLength,class SizeType>
inline typename YsArray <T,MinimumLength,SizeType>::const_iterator rend(const YsArray <T,MinimumLength,SizeType> &array)
{
	return array.rend();
}


template <class T,const int MinimumLength,class SizeType>
inline YsIndexRange YsArray <T,MinimumLength,SizeType>::AllIndex(void) const
{
	YsIndexRange allIndex(GetN());
	return allIndex;
}

template <class T,const int MinimumLength,class SizeType>
inline YsReverseIndexRange YsArray <T,MinimumLength,SizeType>::ReverseIndex(void) const
{
	YsReverseIndexRange allIndex(GetN());
	return allIndex;
}
// STL-like iterator support end <<


template <class T,const int MinimumLength,class SizeType>
inline const T &YsArray<T,MinimumLength,SizeType>::operator[](YSSIZE_T idx) const
{
	// return GetItem(idx);  2003/01/29 Don't check bound.
	return vv[idx];
}

template <class T,const int MinimumLength,class SizeType>
inline T &YsArray<T,MinimumLength,SizeType>::operator[](YSSIZE_T idx)
{
	return vv[idx];
	// return GetEditableItem(idx);  2003/01/29 Don't check bound.
}

template <class T,const int MinimumLength,class SizeType>
YsArray<T,MinimumLength,SizeType>::operator const T *() const
{
	return GetArray();
}

template <class T,const int MinimumLength,class SizeType>
YsArray<T,MinimumLength,SizeType>::operator T *()
{
	return GetEditableArray();
}

template <class T,const int MinimumLength,class SizeType>
inline YsArray<T,MinimumLength,SizeType>::operator const YsConstArrayMask <T> () const
{
	YsConstArrayMask <T> mask(GetN(),GetArray());
	return mask;
}

template <class T,const int MinimumLength,class SizeType>
inline YsArray<T,MinimumLength,SizeType>::operator YsArrayMask <T> ()
{
	YsArrayMask <T> mask(GetN(),GetEditableArray());
	return mask;
}

template <class T,const int MinimumLength,class SizeType>
inline SizeType YsArray<T,MinimumLength,SizeType>::GetN(void) const
{
	return GetNumItem();
}

template <class T,const int MinimumLength,class SizeType>
inline YSRESULT YsArray<T,MinimumLength,SizeType>::Add(const T &dat)
{
	return AppendItem(dat);
}

template <class T,const int MinimumLength,class SizeType>
inline YSRESULT YsArray<T,MinimumLength,SizeType>::Add(T &&dat)
{
	return Append((T &&)dat);
}

template <class T,const int MinimumLength,class SizeType>
inline YSRESULT YsArray<T,MinimumLength,SizeType>::Append(const T &itm)
{
	return AppendItem(itm);
}

template <class T,const int MinimumLength,class SizeType>
inline YSRESULT YsArray <T,MinimumLength,SizeType>::Append(T &&dat)
{
	if(&dat<vv || (vv+nv)<=&dat)
	{
		if(Alloc(nv+1,YSTRUE)==YSOK)
		{
			vv[nv-1]=(T &&)dat;
			return YSOK;
		}
	}
	else
	{
		T tmp((T &&)dat);
		if(Alloc(nv+1,YSTRUE)==YSOK)
		{
			vv[nv-1]=(T &&)tmp;
			return YSOK;
		}
	}
	return YSERR;
}

template <class T,const int MinimumLength,class SizeType>
inline T &YsArray<T,MinimumLength,SizeType>::New(void)
{
	Increment();
	return Last();
}

template <class T,const int MinimumLength,class SizeType>
inline const T &YsArray<T,MinimumLength,SizeType>::GetTop(void) const
{
	return GetTopItem();
}

template <class T,const int MinimumLength,class SizeType>
inline T &YsArray<T,MinimumLength,SizeType>::GetEnd(void)
{
	if(0<nv)
	{
		return vv[nv-1];
	}
	else
	{
		YsErrOut("YsArray::GetEnd()\n  The array is empty.\n");
		assert(0<nv);
		return vv[nv-1];
	}
}

template <class T,const int MinimumLength,class SizeType>
inline const T &YsArray<T,MinimumLength,SizeType>::GetEnd(void) const
{
	return GetEndItem();
}

template <class T,const int MinimumLength,class SizeType>
inline T &YsArray <T,MinimumLength,SizeType>::Last(void)
{
	return GetEnd();
}

template <class T,const int MinimumLength,class SizeType>
inline const T &YsArray <T,MinimumLength,SizeType>::Last(void) const
{
	return GetEnd();
}

template <class T,const int MinimumLength,class SizeType>
inline YSRESULT YsArray<T,MinimumLength,SizeType>::SetItem(SizeType idx,const T &v)
{
	if(0<=idx && idx<nv)
	{
		vv[idx]=v;
		return YSOK;
	}
	return YSERR;
}

template <class T,const int MinimumLength,class SizeType>
YSRESULT YsArray<T,MinimumLength,SizeType>::SetAll(const T &incoming)
{
	for(YSSIZE_T idx=0; idx<nv; ++idx)
	{
		vv[idx]=incoming;
	}
	return YSOK;
}

template <class T,const int MinimumLength,class SizeType>
YSRESULT YsArray <T,MinimumLength,SizeType>::MakeUnitLength(const T &dat)
{
	return Set(1,&dat);
}

template <class T,const int MinimumLength,class SizeType>
inline SizeType YsArray<T,MinimumLength,SizeType>::GetNumItem(void) const
{
	return nv;
}

template <class T,const int MinimumLength,class SizeType>
inline const T &YsArray<T,MinimumLength,SizeType>::v(SizeType i) const
{
	if(0<=i && i<nv)
	{
		return vv[i];
	}
	else
	{
		YsErrOut("YsArray::v()\n  Array Subscript Exceeded\n");
		assert(0<=i && i<nv);
		return vv[i];
	}
}

template <class T,const int MinimumLength,class SizeType>
inline T &YsArray<T,MinimumLength,SizeType>::GetEditableItem(SizeType i)
{
	if(0<=i && i<nv)
	{
		return vv[i];
	}
	YsErrOut("YsArray::GetEditableItem()\n  Array Subscript Exceeded\n");
	assert(0<=i && i<nv);
	return vv[i];
}

template <class T,const int MinimumLength,class SizeType>
inline const T &YsArray<T,MinimumLength,SizeType>::GetItem(SizeType i) const
{
	return v(i);
}

template <class T,const int MinimumLength,class SizeType>
inline T &YsArray<T,MinimumLength,SizeType>::GetCyclic(SizeType idx)
{
	return YsGetCyclic(nv,vv,idx);
}

template <class T,const int MinimumLength,class SizeType>
inline SizeType YsArray<T,MinimumLength,SizeType>::GetCyclicIndex(SizeType idx) const
{
	if(0<size())
	{
		// By the way, is it guaranteed that (-3)%7 is always -3, not 4?  Common sense tells it is -3, but I'm not sure if it was a guaranteed behavior in C++.
		idx=idx%size();
		if(0>idx)
		{
			idx+=size();
		}
		return idx;
	}
	else
	{
		return 0;
	}
}

template <class T,const int MinimumLength,class SizeType>
inline const T &YsArray<T,MinimumLength,SizeType>::GetCyclic(SizeType idx) const
{
	return YsGetCyclic(nv,vv,idx);
}

template <class T,const int MinimumLength,class SizeType>
inline const T *YsArray<T,MinimumLength,SizeType>::GetArray(void) const
{
	return vv;
}

template <class T,const int MinimumLength,class SizeType>
inline T *YsArray<T,MinimumLength,SizeType>::GetEditableArray(void)
{
	return vv;
}

template <class T,const int MinimumLength,class SizeType>
inline T &YsArray<T,MinimumLength,SizeType>::GetEditableTopItem(void)
{
	if(0<nv)
	{
		return vv[0];
	}
	else
	{
		YsErrOut("YsArray::GetEditableTopItem()\n  The array is empty.\n");
		assert(0<nv);
		return vv[0];
	}
}

template <class T,const int MinimumLength,class SizeType>
inline const T &YsArray<T,MinimumLength,SizeType>::GetTopItem(void) const
{
	if(0<nv)
	{
		return vv[0];
	}
	else
	{
		YsErrOut("YsArray::GetTopItem()\n  The array is empty.\n");
		assert(0<nv);
		return vv[0];
	}
}

template <class T,const int MinimumLength,class SizeType>
inline T &YsArray<T,MinimumLength,SizeType>::GetEditableEndItem(void)
{
	if(0<nv)
	{
		return vv[nv-1];
	}
	else
	{
		YsErrOut("YsArray::GetEditableEndItem()\n  The array is empty.\n");
		assert(0<nv);
		return vv[nv-1];
	}
}

template <class T,const int MinimumLength,class SizeType>
inline const T &YsArray<T,MinimumLength,SizeType>::GetEndItem(void) const
{
	if(0<nv)
	{
		return vv[nv-1];
	}
	else
	{
		YsErrOut("YsArray::GetEndItem()\n  The array is empty.\n");
		assert(0<nv);
		return vv[nv-1];
	}
}


template <class T,const int MinimumLength,class SizeType>
inline SizeType YsArray <T,MinimumLength,SizeType>::GetNAvailable(void) const
{
	return nAvailable;
}


template <class T,const int MinimumLength,class SizeType>
inline SizeType YsArray <T,MinimumLength,SizeType>::GetNUnused(void) const
{
	if(nAvailable==0)
	{
		return MinimumLength-nv;
	}
	else
	{
		return nAvailable-nv;
	}
}

template <class T,const int MinimumLength,class SizeType>
inline YSBOOL YsArray<T,MinimumLength,SizeType>::IsInRange(SizeType id) const
{
	if(0<=id && id<GetN())
	{
		return YSTRUE;
	}
	return YSFALSE;
}

template <class T,const int MinimumLength,class SizeType>
inline YSBOOL YsArray<T,MinimumLength,SizeType>::IsIncluded(const T &x) const
{
	SizeType i;
	for(i=0; i<GetN(); i++)
	{
		if(v(i)==x)
		{
			return YSTRUE;
		}
	}
	return YSFALSE;
}

template <class T,const int MinimumLength,class SizeType>
inline YSSIZE_T YsArray<T,MinimumLength,SizeType>::Find(const T &dat) const
{
	for(SizeType i=0; i<GetN(); ++i)
	{
		if(v(i)==dat)
		{
			return i;
		}
	}
	return -1;
}

template <class T,const int MinimumLength,class SizeType>
template <class S>
inline YSBOOL YsArray<T,MinimumLength,SizeType>::HasCommonItem(const S &from) const
{
	for(auto &s : from)
	{
		if(YSTRUE==IsIncluded(s))
		{
			return YSTRUE;
		}
	}
	return YSFALSE;
}

// \cond
template <class T,const int MinimumLength,class SizeType>
inline YSRESULT YsArray<T,MinimumLength,SizeType>::Alloc(SizeType n,YSBOOL cpy)
{
	if(n<=nAvailable)
	{
		nv=n;
		return YSOK;
	}
	else if(/* nAvailable<n && */ n<=MinimumLength)
	{
		if(vv==Prealloc())
		{
			// Nothing to do
		}
		else /* if(vv!=Prealloc()) */
		{
			if(cpy==YSTRUE && nv>0)
			{
				SizeType i;
				for(i=0; i<nv; i++)
				{
					Prealloc()[i]=(T &&)vv[i];
				}
			}
			if(vv!=NULL)
			{
				delete [] vv;
			}
		}
		nv=n;
		vv=Prealloc();
		nAvailable=0;
		return YSOK;
	}
	else
	{
		T *neo;
		SizeType toAlloc;

		toAlloc=YsGreater <SizeType> ((MinimumLength>0 ? MinimumLength*2 : 1),nAvailable*2);
		while(toAlloc<n)
		{
			toAlloc=toAlloc*2;
			if(toAlloc<0) // Overflow
			{
				goto INTOVERFLOW;
			}
		}
		neo=new T [toAlloc];
		if(neo!=NULL)
		{
			if(cpy==YSTRUE && nv>0)    // nv==0 -> don't have to copy
			{
				SizeType i;
				for(i=0; i<nv; i++)
				{
					neo[i]=(T &&)vv[i];
				}
			}
			if(vv!=NULL && vv!=Prealloc())
			{
				delete [] vv;
			}
			nv=n;
			vv=neo;
			nAvailable=toAlloc;
			return YSOK;
		}
		else
		{
			goto OUTOFMEM;
		}
// Will never fall into this place
	}
// Will never fall into this place

OUTOFMEM:
	YsExceptionHandler::Exception(YsExceptionHandler::OUTOFMEMORY,"YsArray::Alloc()");
	YsErrOut("YsArray::Alloc()\n");
	YsErrOut("  Out of memory!!!\n");
	return YSERR;

INTOVERFLOW:
	YsErrOut("YsArray::Alloc()\n");
	YsErrOut("  Integer Overflow!!!\n");
	return YSERR;
}

template <class T,const int MinimumLength,class SizeType>
inline YSRESULT YsArray<T,MinimumLength,SizeType>::AllocWithoutCopy(SizeType n)
{
	if(n<=nAvailable)
	{
		nv=n;
		return YSOK;
	}
	else if(/* nAvailable<n && */ n<=MinimumLength)
	{
		if(vv!=Prealloc() && vv!=NULL)
		{
			delete [] vv;
		}
		nv=n;
		vv=Prealloc();
		nAvailable=0;
		return YSOK;
	}
	else
	{
		T *neo;
		SizeType toAlloc;

		toAlloc=YsGreater <SizeType> ((MinimumLength>0 ? MinimumLength*2 : 1),nAvailable*2);
		while(toAlloc<n)
		{
			toAlloc=toAlloc*2;
			if(toAlloc<0) // Overflow
			{
				goto INTOVERFLOW;
			}
		}
		neo=new T [toAlloc];
		if(neo!=NULL)
		{
			if(vv!=NULL && vv!=Prealloc())
			{
				delete [] vv;
			}
			nv=n;
			vv=neo;
			nAvailable=toAlloc;
			return YSOK;
		}
		else
		{
			goto OUTOFMEM;
		}
// Will never fall into this place
	}
// Will never fall into this place

OUTOFMEM:
	YsExceptionHandler::Exception(YsExceptionHandler::OUTOFMEMORY,"YsArray::Alloc()");
	YsErrOut("YsArray::Alloc()\n");
	YsErrOut("  Out of memory!!!\n");
	return YSERR;

INTOVERFLOW:
	YsErrOut("YsArray::Alloc()\n");
	YsErrOut("  Integer Overflow!!!\n");
	return YSERR;
}

template <class T,const int MinimumLength,class SizeType>
inline YSRESULT YsArray <T,MinimumLength,SizeType>::Shrink(void)
{
	SizeType i;
	if(vv!=NULL && vv!=Prealloc() && (nv<nAvailable/2 || nv<=MinimumLength))
	// Condition: nv<=MinimumLength is added on 02/11/2006
	{
		if(nv<=MinimumLength)
		{
			for(i=0; i<nv; i++)
			{
				Prealloc()[i]=(T &&)vv[i];
			}
			delete [] vv;
			vv=Prealloc();
			nAvailable=0;
		}
		else
		{
			SizeType newSize;
			T *newArray;
			newSize=nAvailable/2;
			while(newSize>MinimumLength*2 && newSize>nv*2)
			{
				newSize/=2;
			}
			newArray=new T [newSize];
			if(newArray!=NULL)
			{
				for(i=0; i<nv; i++)
				{
					newArray[i]=(T &&)vv[i];
				}
				delete [] vv;
				vv=newArray;
				nAvailable=newSize;
			}
			else
			{
				YsExceptionHandler::Exception(YsExceptionHandler::OUTOFMEMORY,"YsArray::Shrink()");
				return YSERR;
			}
		}
	}
	return YSOK;
}
// \endcond

template <class T,const int MinimumLength,class SizeType>
YsArray<T,MinimumLength,SizeType>::YsArray()
{
	nv=0;
	nAvailable=0;
	vv=NULL;
}

template <class T,const int MinimumLength,class SizeType>
YsArray<T,MinimumLength,SizeType>::YsArray(SizeType nv,const T v[])
{
	this->nv=0;
	this->nAvailable=0;
	this->vv=NULL;
	Set(nv,v);
}

template <class T,const int MinimumLength,class SizeType>
YsArray<T,MinimumLength,SizeType>::YsArray(std::nullptr_t,SizeType N)
{
	this->nv=0;
	this->nAvailable=0;
	this->vv=NULL;
	AllocWithoutCopy(N);
}

template <class T,const int MinimumLength,class SizeType>
YsArray<T,MinimumLength,SizeType>::YsArray(std::initializer_list <T> initList)
{
	this->nv=0;
	this->nAvailable=0;
	this->vv=NULL;
	resize(initList.size());
	YSSIZE_T idx=0;
	for(auto &v : initList)
	{
		(*this)[idx]=v;
	}
}


template <class T,const int MinimumLength,class SizeType>
YsArray<T,MinimumLength,SizeType>::YsArray(const YsArray <T,MinimumLength,SizeType> &from)
{
	nv=0;
	nAvailable=0;
	vv=NULL;
	Set(from.GetN(),from);
}

template <class T,const int MinimumLength,class SizeType>
YsArray<T,MinimumLength,SizeType>::YsArray(const YsConstArrayMask <T> &from)
{
	nv=0;
	nAvailable=0;
	vv=NULL;
	Set((SizeType)from.GetN(),from);
}

template <class T,const int MinimumLength,class SizeType>
YsArray<T,MinimumLength,SizeType>::YsArray(YsArray <T,MinimumLength,SizeType> &&incoming)
{
	nv=0;
	nAvailable=0;
	vv=NULL;
	MoveFrom(incoming);
}

template <class T,const int MinimumLength,class SizeType>
template <const int MinimumLength2,class SizeType2>
YsArray<T,MinimumLength,SizeType>::YsArray(YsArray <T,MinimumLength2,SizeType2> &&incoming)
{
	nv=0;
	nAvailable=0;
	vv=NULL;
	MoveFrom(incoming);
}

template <class T,const int MinimumLength,class SizeType>
template <class StorageClass>
YsArray<T,MinimumLength,SizeType>::YsArray(const StorageClass &incoming)
{
	nv=0;
	nAvailable=0;
	vv=NULL;
	for(auto iter=incoming.begin(); iter!=incoming.end(); ++iter)
	{
		Append(*iter);
	}
}

template <class T,const int MinimumLength,class SizeType>
YsArray<T,MinimumLength,SizeType>::~YsArray()
{
	if(vv!=NULL && vv!=Prealloc())
	{
		delete [] vv;
	}
}

template <class T,const int MinimumLength,class SizeType>
YsArray <T,MinimumLength,SizeType> &YsArray<T,MinimumLength,SizeType>::operator=(const YsArray <T,MinimumLength,SizeType> &from)
{
	if(this->GetArray()!=from.GetArray())
	{
		Set(from.GetN(),from.GetArray());
	}
	return *this;
}

template <class T,const int MinimumLength,class SizeType>
template <class StorageClass>
YsArray <T,MinimumLength,SizeType> &YsArray <T,MinimumLength,SizeType>::operator=(const StorageClass &from)
{
	CleanUp();
	for(auto iter=from.begin(); iter!=from.end(); ++iter)
	{
		auto &v=*iter;
		Append(v);
	}
	return *this;
}

template <class T,const int MinimumLength,class SizeType>
YsArray <T,MinimumLength,SizeType> &YsArray<T,MinimumLength,SizeType>::operator=(std::initializer_list <T> initList)
{
	resize(initList.size());
	YSSIZE_T idx=0;
	for(auto &v : initList)
	{
		(*this)[idx]=v;
	}
	return *this;
}

template <class T,const int MinimumLength,class SizeType>
YsArray <T,MinimumLength,SizeType> &YsArray<T,MinimumLength,SizeType>::operator=(const YsConstArrayMask <T> &from)
{
	if(this->GetArray()!=from.GetArray())
	{
		Set((SizeType)from.GetN(),from.GetArray());
	}
	return *this;
}


template <class T,const int MinimumLength,class SizeType>
YsArray <T,MinimumLength,SizeType> &YsArray<T,MinimumLength,SizeType>::operator=(YsArray <T,MinimumLength,SizeType> &&incoming)
{
	if(this->GetArray()!=incoming.GetArray())
	{
		MoveFrom(incoming);
	}
	return *this;
}

template <class T,const int MinimumLength,class SizeType>
template <const int MinimumLength2,class SizeType2>
YsArray <T,MinimumLength,SizeType> &YsArray<T,MinimumLength,SizeType>::operator=(YsArray <T,MinimumLength2,SizeType2> &&incoming)
{
	if(this->GetArray()!=incoming.GetArray())
	{
		MoveFrom(incoming);
	}
	return *this;
}


template <class T,const int MinimumLength,class SizeType>
YSRESULT YsArray<T,MinimumLength,SizeType>::Set(const T &v)
{
	return Set(1,&v);
}

template <class T,const int MinimumLength,class SizeType>
YSRESULT YsArray<T,MinimumLength,SizeType>::Set(SizeType NV,const T V[])
{
	if(NV==0)
	{
		if(nAvailable>MinimumLength*2)
		{
			if(vv!=NULL && vv!=Prealloc())
			{
				delete [] vv;
			}
			nv=0;
			vv=NULL;
			nAvailable=0;
		}
		else
		{
			nv=0;
		}
		return YSOK;
	}
	else
	{
		if(Alloc(NV,YSFALSE)==YSOK)
		{
			if(V!=NULL)
			{
				for(SizeType i=0; i<NV; i++)
				{
					vv[i]=V[i]; // 2014/09/26 Serious error was in here.  V[i] was casted to (T &&), although T is const.
				}
			}
			return YSOK;
		}
		else
		{
			YsErrOut("YsArray::Set()\n  Low Memory Warning!!\n");
			return YSERR;
		}
	}
}

template <class T,const int MinimumLength,class SizeType>
YsArray<T,MinimumLength,SizeType> &YsArray<T,MinimumLength,SizeType>::operator=(const YsList <T> *from)
{
	SizeType nItem;
	const YsList <T> *seeker;

	nItem=from->GetNumObject();
	if(Alloc(nItem,YSFALSE)==YSOK)
	{
		SizeType i;
		for(i=0; i<nItem; i++)
		{
			seeker=from->Seek(i);
			vv[i]=seeker->dat;
		}
	}
	else
	{
		YsErrOut("YsArray::Set()\n  Low Memory Warning!!\n");
	}
	return *this;
}

template <class T,const int MinimumLength,class SizeType>
YsArray <T,MinimumLength,SizeType> YsArray<T,MinimumLength,SizeType>::operator+(const YsConstArrayMask <T> &toAdd) const
{
	YsArray <T,MinimumLength,SizeType> sum;
	sum.resize(this->size()+toAdd.size());
	for(YSSIZE_T idx=0; idx<this->size(); ++idx)
	{
		sum[idx]=(*this)[idx];
	}
	auto base=this->size();
	for(YSSIZE_T idx=0; idx<toAdd.size(); ++idx)
	{
		sum[base+idx]=toAdd[idx];
	}
	return sum;
}

template <class T,const int MinimumLength,class SizeType>
YsArray <T,MinimumLength,SizeType> &YsArray <T,MinimumLength,SizeType>::CopyFrom(const YsConstArrayMask <T> &from)
{
	if(from.data()!=this->data())
	{
		this->Set(from.size(),from.data());
	}
	return *this;
}

template <class T,const int MinimumLength,class SizeType>
YsArray <T,MinimumLength,SizeType> &YsArray <T,MinimumLength,SizeType>::MoveFrom(YsArray <T,MinimumLength,SizeType> &incoming)
{
	if(incoming.vv!=incoming.Prealloc())
	{
		if(NULL!=this->vv && this->vv!=this->Prealloc())
		{
			delete [] this->vv;
		}
		this->vv=incoming.vv;
		this->nv=incoming.nv;
		this->nAvailable=incoming.nAvailable;

		incoming.nv=0;
		incoming.nAvailable=0;
		incoming.vv=NULL;
	}
	else
	{
		this->Set(incoming.GetN(),NULL);
		for(YSSIZE_T idx=0; idx<incoming.GetN(); ++idx)
		{
			(*this)[idx]=(T &&)(incoming[idx]);
		}
		incoming.CleanUp();
	}
	return *this;
}

 
template <class T,const int MinimumLength,class SizeType>
template <const int MinimumLength2,class SizeType2>
YsArray <T,MinimumLength,SizeType> &YsArray <T,MinimumLength,SizeType>::MoveFrom(YsArray <T,MinimumLength2,SizeType2> &incoming)
{
	T *toMove;
	SizeType2 nv,nAvailable;
	if(MinimumLength<incoming.GetN() &&
	   NULL!=(toMove=incoming.DetachPointer_InternalUseOnlyDontUseItFromOutsideYsArrayClass(nv,nAvailable)))
	{
		if(NULL!=this->vv && this->vv!=this->Prealloc())
		{
			delete [] this->vv;
		}
		this->vv=toMove;
		this->nv=(SizeType)nv;
		this->nAvailable=(SizeType)nAvailable;
	}
	else
	{
		this->Set((SizeType)incoming.GetN(),NULL);
		for(YSSIZE_T idx=0; idx<incoming.GetN(); ++idx)
		{
			(*this)[idx]=(T &&)(incoming[idx]);
		}
	}
	return *this;
}

template <class T,const int MinimumLength,class SizeType>
YSRESULT YsArray<T,MinimumLength,SizeType>::Resize(SizeType neoSize)
{
	return Alloc(neoSize,YSTRUE);
}

template <class T,const int MinimumLength,class SizeType>
YSRESULT YsArray<T,MinimumLength,SizeType>::ResizeNoCopy(SizeType newSize)
{
	return AllocWithoutCopy(newSize);
}

template <class T,const int MinimumLength,class SizeType>
YSRESULT YsArray<T,MinimumLength,SizeType>::AppendItem(const T &dat)
{
	if(&dat<vv || (vv+nv)<=&dat)
	{
		if(Alloc(nv+1,YSTRUE)==YSOK)
		{
			vv[nv-1]=dat;
			return YSOK;
		}
	}
	else // Self-copy case
	{
		SizeType srcIdx=(SizeType)((&dat)-vv);
		if(Alloc(nv+1,YSTRUE)==YSOK)
		{
			vv[nv-1]=vv[srcIdx];
			return YSOK;
		}
	}
	return YSERR;
}

template <class T,const int MinimumLength,class SizeType>
YSRESULT YsArray<T,MinimumLength,SizeType>::Add(SizeType nItm,const T itm[])
{
	if(itm<vv || (vv+nv)<=itm)
	{
		SizeType cpyTop=nv;
		if(Alloc(nv+nItm,YSTRUE)==YSOK)
		{
			for(SizeType i=0; i<nItm; i++)
			{
				vv[cpyTop+i]=itm[i];
			}
			return YSOK;
		}
	}
	else // Self copy
	{
		SizeType srcIdx=(SizeType)(itm-vv);
		SizeType cpyTop=nv;
		if(Alloc(nv+nItm,YSTRUE)==YSOK)
		{
			for(SizeType i=0; i<nItm; i++)
			{
				vv[cpyTop+i]=vv[srcIdx+i];
			}
			return YSOK;
		}
	}
	return YSERR;
}

template <class T,const int MinimumLength,class SizeType>
YSRESULT YsArray<T,MinimumLength,SizeType>::Append(SizeType nItm,const T itm[])
{
	return Add(nItm,itm);
}

template <class T,const int MinimumLength,class SizeType>
YSRESULT YsArray<T,MinimumLength,SizeType>::Add(const YsConstArrayMask <T> &ary)
{
	return Add((SizeType)ary.GetN(),ary.GetArray());
}

template <class T,const int MinimumLength,class SizeType>
YSRESULT YsArray<T,MinimumLength,SizeType>::Append(const YsConstArrayMask <T> &ary)
{
	return Add((SizeType)ary.GetN(),ary.GetArray());
}

template <class T,const int MinimumLength,class SizeType>
YSRESULT YsArray<T,MinimumLength,SizeType>::Delete(SizeType id)
{
	if(0<=id && id<nv)
	{
		SizeType i;
		for(i=id; i<nv-1; i++)
		{
			vv[i]=(T &&)vv[i+1];
		}
		nv--;
		Shrink();
		return YSOK;
	}
	return YSERR;
}

template <class T,const int MinimumLength,class SizeType>
YSRESULT YsArray <T,MinimumLength,SizeType>::Delete(SizeType idFrom,SizeType numItem)
{
	if(GetN()<=idFrom+numItem)
	{
		Resize(idFrom);
	}
	else
	{
		SizeType i;
		for(i=idFrom; i+numItem<GetN(); i++)
		{
			vv[i]=vv[i+numItem];
		}
		Resize(GetN()-numItem);
	}
	return YSOK;
}

template <class T,const int MinimumLength,class SizeType>
YSRESULT YsArray <T,MinimumLength,SizeType>::Invert(void)
{
	SizeType i;
	for(i=0; i<nv/2; i++)
	{
		T a;
		a=vv[i];
		vv[i]=vv[nv-1-i];
		vv[nv-1-i]=a;
	}
	return YSOK;
}

template <class T,const int MinimumLength,class SizeType>
YSRESULT YsArray <T,MinimumLength,SizeType>::Swap(SizeType a,SizeType b)
{
	T swp;
	swp=vv[a];
	vv[a]=vv[b];
	vv[b]=swp;
	return YSOK;
}

template <class T,const int MinimumLength,class SizeType>
T *YsArray <T,MinimumLength,SizeType>::DetachPointer_InternalUseOnlyDontUseItFromOutsideYsArrayClass(SizeType &nv,SizeType &nAvailable)
{
	if(vv!=Prealloc())
	{
		T *retPtr=vv;
		nv=this->nv;
		nAvailable=this->nAvailable;

		this->nv=0;
		this->nAvailable=0;
		this->vv=NULL;
		return retPtr;
	}
	return NULL;
}

template <class T,const int MinimumLength,class SizeType>
YSRESULT YsArray<T,MinimumLength,SizeType>::DeleteBySwapping(YSSIZE_T id)
{
	if(0<=id && id<nv)
	{
		vv[id]=(T &&)vv[nv-1];
		--nv;
		Shrink();
		return YSOK;
	}
	return YSERR;
}

template <class T,const int MinimumLength,class SizeType>
YSRESULT YsArray<T,MinimumLength,SizeType>::DeleteItemBySwapping(const T &incoming)
{
	auto newSize=GetN();
	for(auto idx=GetN()-1; 0<=idx; --idx)
	{
		if(vv[idx]==incoming)
		{
			--newSize;
			if(idx!=newSize)
			{
				vv[idx]=(T &&)vv[newSize];
			}
		}
	}
	if(GetN()!=newSize)
	{
		Resize(newSize);
		return YSOK;
	}
	return YSERR;
}

template <class T,const int MinimumLength,class SizeType>
YSRESULT YsArray<T,MinimumLength,SizeType>::DeleteItem(const T &incoming)
{
	auto newSize=GetN();
	for(auto idx=GetN()-1; 0<=idx; --idx)
	{
		if(vv[idx]==incoming)
		{
			--newSize;
			for(auto i=idx; i<newSize; ++i)
			{
				vv[i]=(T &&)vv[i+1];
			}
		}
	}
	if(GetN()!=newSize)
	{
		Resize(newSize);
		return YSOK;
	}
	return YSERR;
}

template <class T,const int MinimumLength,class SizeType>
YSRESULT YsArray<T,MinimumLength,SizeType>::DeleteLast(void)
{
	if(nv>0)
	{
		--nv;
		Shrink();
		return YSOK;
	}
	return YSERR;
}

template <class T,const int MinimumLength,class SizeType>
YSRESULT YsArray<T,MinimumLength,SizeType>::Increment(void)
{
	return Alloc(GetN()+1,YSTRUE);
}

template <class T,const int MinimumLength,class SizeType>
YSRESULT YsArray<T,MinimumLength,SizeType>::Clear(void)
{
	return Set(0,NULL);
}

template <class T,const int MinimumLength,class SizeType>
YSRESULT YsArray<T,MinimumLength,SizeType>::CleanUp(void)
{
	return Set(0,NULL);
}

template <class T,const int MinimumLength,class SizeType>
YSRESULT YsArray<T,MinimumLength,SizeType>::ClearDeep(void)
{
	if(NULL!=vv && Prealloc()!=vv)
	{
		delete [] vv;
	}
	nv=0;
	nAvailable=0;
	vv=NULL;
	return YSOK;
}

template <class T,const int MinimumLength,class SizeType>
YSRESULT YsArray<T,MinimumLength,SizeType>::Insert(SizeType id,const T &dat)
{
	if(0<=id && id<=nv && Alloc(nv+1,YSTRUE)==YSOK)
	{
		SizeType i;
		for(i=nv-1; i>id; i--)
		{
			vv[i]=(T &&)vv[i-1];
		}
		vv[id]=dat;
		return YSOK;
	}
	return YSERR;
}

/*! Compare operator that compares two YsArray objects.  Compare operator (operator==) must be
    defined for class T, or the compiler will give an error. */
template <class T,const int MinimumLength,class SizeType>
inline int operator==(const YsArray <T,MinimumLength,SizeType> &a,const YsArray <T,MinimumLength,SizeType> &b)
{
	if(a.GetN()==b.GetN())
	{
		int i;
		for(i=0; i<a.GetN(); i++)
		{
			if(a[i]!=b[i])
			{
				return 0;
			}
		}
		return 1;
	}
	else
	{
		return 0;
	}
}

// Want to make it a cast operator for YsArray
//template <class T>
//template <const int MinimumLength,class SizeType>
//YsArrayMask<T>::YsArrayMask(YsArray <T,MinimumLength,SizeType> &ary)
//{
//	this->len=ary.GetN();
//	this->dat=ary.GetEditableArray();
//}

// Want to make it a cast operator for YsArray
//template <class T>
//template <const int N,class SizeType>
//YsConstArrayMask<T>::YsConstArrayMask(class YsArray <T,N,SizeType> &incoming)
//{
//	this->len=incoming.GetN();
//	this->dat=incoming.GetArray();
//}

////////////////////////////////////////////////////////////

/*! This class can be a base class for an ordered array.
    This class maintains the order of the items according to the return value
    of Compare function.
    By overriding Compare function, this class can sort items by different
    criteria.
   */
template <class T,const int MinimumLength=1,class SizeType=int>
class YsOrderedArrayBase : protected YsArray <T,MinimumLength,SizeType>
{
// >> Complying with a stupid change made in g++ 3.4
protected:
using YsArray <T,MinimumLength,SizeType>::Set;
// << Complying with a stupid change made in g++ 3.4

public:
	/*! This function clears the array. */
	YSRESULT Clear(void);

	/*! Same as Clear.  Added for making it consistent with other classes. */
	YSRESULT CleanUp(void);

	/*! This function adds an item. */
	YSRESULT AppendItem(const T &dat);

	/*! This function deletes an item. */
	YSRESULT DeleteItem(const T &dat);

	using YsArray <T,MinimumLength,SizeType>::operator[];
	using YsArray <T,MinimumLength,SizeType>::operator const T *;
	using YsArray <T,MinimumLength,SizeType>::GetN;
	using YsArray <T,MinimumLength,SizeType>::GetTop;
	using YsArray <T,MinimumLength,SizeType>::GetEnd;
	using YsArray <T,MinimumLength,SizeType>::GetNumItem;
	using YsArray <T,MinimumLength,SizeType>::GetItem;
	using YsArray <T,MinimumLength,SizeType>::GetArray;
	using YsArray <T,MinimumLength,SizeType>::Delete;

protected:
	/*! This function returns -1 if A should be before B, 0 if the order priority
	    of A and B are equal, and 1 if A should be after B. */
	virtual int Compare(const T &A,const T &B);  // Return -1,0 or +1
};

template <class T,const int MinimumLength,class SizeType>
inline YSRESULT YsOrderedArrayBase <T,MinimumLength,SizeType>::Clear(void)
{
	Set(0,NULL);
	return YSOK;
}

template <class T,const int MinimumLength,class SizeType>
inline YSRESULT YsOrderedArrayBase <T,MinimumLength,SizeType>::CleanUp(void)
{
	Set(0,NULL);
	return YSOK;
}

template <class T,const int MinimumLength,class SizeType>
inline YSRESULT YsOrderedArrayBase <T,MinimumLength,SizeType>::AppendItem(const T &dat)
{
	SizeType i1,i2,im,n;
	n=GetNumItem();
	if(n==0 || Compare(dat,GetItem(n-1))>=0)
	{
		return YsArray <T,MinimumLength,SizeType>::AppendItem(dat);
	}
	else if(Compare(dat,GetItem(0))<=0)
	{
		return YsArray <T,MinimumLength,SizeType>::Insert(0,dat);
	}
	else
	{
		i1=0;
		i2=n-1;
		while(i2-i1>1)
		{
			im=(i1+i2)/2;
			if(Compare(dat,GetItem(im))<0)
			{
				i2=im;
			}
			else if(Compare(dat,GetItem(im))==0)
			{
				i1=im;
				i2=im+1;
			}
			else if(Compare(dat,GetItem(im))>0)
			{
				i1=im;
			}
		}
		return YsArray <T,MinimumLength,SizeType>::Insert(i2,dat);
	}
}

template <class T,const int MinimumLength,class SizeType>
inline YSRESULT YsOrderedArrayBase <T,MinimumLength,SizeType>::DeleteItem(const T &dat)
{
	SizeType n,i1,i2,im;

	n=GetNumItem();
	if(n>0 && Compare(GetItem(0),dat)<=0 && Compare(dat,GetItem(n-1))<=0)
	{
		i1=0;
		i2=n-1;
		while(i2-i1>1)
		{
			im=(i1+i2)/2;
			if(Compare(dat,GetItem(im))<0)
			{
				i2=im;
			}
			else if(Compare(GetItem(im),dat)==0)
			{
				i1=im;
				i2=im+1;
			}
			else if(Compare(dat,GetItem(im))>0)
			{
				i1=im;
			}
		}
		if(Compare(GetItem(i1),dat)==0)
		{
			return YsArray <T,MinimumLength,SizeType>::Delete(i1);
		}
		else if(Compare(GetItem(i2),dat)==0)
		{
			return YsArray <T,MinimumLength,SizeType>::Delete(i2);
		}
	}
	return YSERR;
}

template <class T,const int MinimumLength,class SizeType>
inline int YsOrderedArrayBase <T,MinimumLength,SizeType>::Compare(const T &a,const T &b)
{
	YsErrOut("YsOrderedArrayBase <T,MinimumLength,SizeType>::Compate(const T &a,const T &b)\n");
	YsErrOut("  This function must not be called directly!\n");
	return 0;
}


////////////////////////////////////////////////////////////

/*! An ordered array class.  The items are stored in the array in the ascending order
    based on the compare operator (operator> and operator<). */
template <class T,const int MinimumLength=1,class SizeType=int>
class YsOrderedArray : public YsOrderedArrayBase <T,MinimumLength,SizeType>
{
protected:
// \cond
	virtual int Compare(const T &a,const T &b); // Return -1,0 or +1
// \endcond
};

// \cond
template <class T,const int MinimumLength,class SizeType>
inline int YsOrderedArray <T,MinimumLength,SizeType>::Compare(const T &a,const T &b)
{
	// Basically, return a-b)
	if(a<b)
	{
		return -1;
	}
	else if(a>b)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}
// \endcond

////////////////////////////////////////////////////////////

/*! This function deletes items that is equal to dat from YsArray object.
    Compare operator (operator==) must be defined for class T, or the compiler
    wil give an error. */
template <class T,const int MinimumLength,class SizeType>
inline YSRESULT YsDeleteArrayItem(int &nDel,YsArray <T,MinimumLength,SizeType> &ary, const T &dat)
{
	SizeType i;
	nDel=0;
	for(i=ary.GetN()-1; i>=0; i--)
	{
		if(ary[i]==dat)
		{
			ary.Delete(i);
			nDel++;
		}
	}
	return YSOK;
}

////////////////////////////////////////////////////////////

/*! This function returns YSTRUE if at least one of v[0], v[1], ..., v[n-1] is equal to x,
    or YSFALSE otherwise.  The position of the first item that is equal to x will be returned
    to idx.
    Compare operator (operator==) must be defined for class T, or the compiler
    wil give an error. */
template <class T>
inline YSBOOL YsIsIncluded(YSSIZE_T &idx,YSSIZE_T n,const T v[],const T &x)
{
	YSSIZE_T i;
	for(i=0; i<n; i++)
	{
		if(x==v[i])
		{
			idx=i;
			return YSTRUE;
		}
	}
	return YSFALSE;
}

/*! This function returns YSTRUE if at least one of v[0], v[1], ..., v[n-1] is equal to x,
    or YSFALSE otherwise.
    Compare operator (operator==) must be defined for class T, or the compiler
    wil give an error. */
template <class T>
inline YSBOOL YsIsIncluded(YSSIZE_T n,const T v[],const T &x)
{
	YSSIZE_T idx;
	return YsIsIncluded(idx,n,v,x);
}

/*! This function returns YSTRUE if ary includes at least one item equals to dat,
    or YSFALSE otherwise.  The position of the first item that is equal to dat will be returned
    to idx.
    Compare operator (operator==) must be defined for class T, or the compiler
    wil give an error. */
template <class T,const int MinimumLength,class SizeType>
inline YSBOOL YsIsIncluded(SizeType &idx,YsArray <T,MinimumLength,SizeType> &ary, const T &dat)
{
	return YsIsIncluded(idx,ary.GetN(),ary.GetArray(),dat);
}

/*! This function returns YSTRUE if ary includes at least one item equals to dat,
    or YSFALSE otherwise.
    Compare operator (operator==) must be defined for class T, or the compiler
    wil give an error. */
template <class T,const int MinimumLength,class SizeType>
inline YSBOOL YsIsIncluded(YsArray <T,MinimumLength,SizeType> &ary, const T &dat)
{
	return YsIsIncluded(ary.GetN(),ary.GetArray(),dat);
}

/*! This function returns YSTRUE if ary includes at least one item equals to dat,
    or YSFALSE otherwise.
    This function assumes items in the given array is sorted, and performs binary
    search to find a matching item.
    Compare operator (operator==) must be defined for class T, or the compiler
    wil give an error. */
template <class T>
inline YSBOOL YsIsIncludedInSortedArray(YSSIZE_T n,const T V[],const T &x)
{
	YSSIZE_T a,b,m;
	a=0;
	b=n-1;
	while(1<b-a)
	{
		m=(a+b)/2;
		if(V[m]==x)
		{
			return YSTRUE;
		}
		else if(x<V[m])
		{
			b=m;
		}
		else
		{
			a=m;
		}
	}

	if(a==b)
	{
		if(V[a]==x)
		{
			return YSTRUE;
		}
	}
	else if(V[a]==x || V[b]==x)  // Note: Equal operator may cost considerably.  So, take a==b case separately.
	{
		return YSTRUE;
	}

	return YSFALSE;
}

/*! This function copies YsArray object src to dst. */
template <class T,const int ML1,const int ML2,class SizeType1,class SizeType2>
inline YSRESULT YsCopyArray(YsArray <T,ML1,SizeType1> &dst,const YsArray <T,ML2,SizeType2> &src)
{
	return dst.Set(src.GetN(),src.GetArray());
}

/*! This function returns YSTRUE if contents of array a1 are equal to contents of a2, or YSFALSE otherwise. */
template <class SizeType,class T>
inline YSBOOL YsCheckSameArray(SizeType n,const T a1[],const T a2[])
{
	for(SizeType i=0; i<n; i++)
	{
		if(a1[i]!=a2[i])
		{
			return YSFALSE;
		}
	}
	return YSTRUE;
}

/*! This function returns YSTRUE if contents of array a1 are equal to contents of a2, or YSFALSE otherwise. 
    Kept for backward comopability, but the naming was unclear if it returns YSTRUE when two arrays are same or different. 
    So, use YsCheckSameArray function instead. */
template <class T>
inline YSBOOL YsCompareArray(YSSIZE_T n,const T a1[],const T a2[])
{
	return YsCheckSameArray <YSSIZE_T,T> (n,a1,a2);
}

/*! This function checks if any item in ary1[0] to ary1[n1-1] is included in ary2[0] to ary2[n1-1]
    and returns YSTRUE if it finds any. */
template <class T>
inline YSBOOL YsCheckOverlapInArray(YSSIZE_T n1,const T ary1[],YSSIZE_T n2,const T ary2[])
{
	for(YSSIZE_T i=0; i<n1; ++i)
	{
		for(YSSIZE_T j=0; j<n2; ++j)
		{
			if(ary1[i]==ary2[j])
			{
				return YSTRUE;
			}
		}
	}
	return YSFALSE;
}

////////////////////////////////////////////////////////////

/*! This function removes duplicated items included in ary.  This function first performs merge sort
    to sort items, and then collapses subsequent equal items.  Expected computational complexity is
    order of N.  */
template <class T,const int MinimumLength,class SizeType>
inline YSSIZE_T YsRemoveDuplicateInUnorderedArray(YsArray <T,MinimumLength,SizeType> &ary)
{
	YSSIZE_T i,n;
	YsSimpleMergeSort <T> (ary.GetN(),ary);

	n=0;
	for(i=ary.GetN()-1; i>0; i--)
	{
		if(ary[i]==ary[i-1])
		{
			ary.DeleteBySwapping(i);
			n++;
		}
	}
	return n;
}

/*! A macro that makes a for-loop that iterates through array items (for convenience) */
#define forYsArray(i,ary) for((i)=0; (i)<(ary).GetN(); (i)++)

/*! A macro that makes a for-loop that iterates through array items (for convenience).  It runs in the reverse order. */
#define forYsArrayRev(i,ary) for((i)=(ary).GetN()-1; (i)>=0; (i)--)

/*! A macro for copying a YsArray to another. */
#define YsAryCpy(dst,src) (dst).Set((src).GetN(),(src))

/*! A macro for concatinating a YsArray to another. */
#define YsAryCat(dst,src) (dst).Append((src).GetN(),(src))


template <class T>
YsArray <T> YsCommonArrayElement(const YsConstArrayMask <T> &ary1,const YsConstArrayMask <T> &ary2)
{
	YsArray <T> common;
	for(auto &a : ary1)
	{
		for(auto &b : ary2)
		{
			if(a==b)
			{
				common.Add(a);
				break;
			}
		}
	}
	return common;
}


#endif
