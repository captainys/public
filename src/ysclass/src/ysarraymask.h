#ifndef YSARRAYMASK_IS_INCLUDED
#define YSARRAYMASK_IS_INCLUDED
/* { */

// If conversion from YsArray to YsConstArrayMask/YsArrayMask is a cast operator of YsArray, YsArrayMask won't have to know what YsArray is.
//template <class T,const int MinimumLength,class SizeType>
//class YsArray;


#include <vector>
#include "ysdef.h"
#include "ysrange.h"
#include "ysindexiterator.h"


/*! YsArrayMask class is for dealing with an array defined by a length and a pointer, say YSSIZE_T len and T *dat.
    The array has a range dat[0]...dat[len-1], but STL-like iterator and range-based-for cannot be used for this
    kind of array.

    This class covers up such an array make STL-like iterator and range-based-for.

    It is also useful for using it as a data type for receiving an YsArray.  YsArray takes MinimumLength and SizeType
    in addition to the data type that the array stores for performance and memory-efficiency reason.  However,
    in many situations, you want to write a function that can accept a YsArray regardless of MinimumLength and SizeType.

    Without this class, only way to write such a function was to write a templated function.  However, it forces the
    function-body to be written in the header file, and the header-file size becomes excessively large.  Instead,
    you can write a function that takes YsArrayMask <T>, and write the function body in .cpp file.

    It also allows accepting std::vector class with nearly-zero performance penalty.
*/
template <class T>
class YsArrayMask
{
private:
	YSSIZE_T len;
	T *dat;

public:
	/*! Constructor to construct the object from a length and a pointer.
	*/
	YsArrayMask(YSSIZE_T len,T *dat)
	{
		this->len=len;
		this->dat=dat;
	}

// Want to make it a cast operator in YsArray
//	/*! Constructor to construct the object from an YsArray class.
//	*/
//	template <const int MinimumLength,class SizeType>
//	YsArrayMask(YsArray <T,MinimumLength,SizeType> &ary);

	/*! Constructor to construct the object from a std::vector.
	*/
	YsArrayMask(std::vector <T> &incoming)
	{
		this->len=(YSSIZE_T)incoming.size();
		this->dat=incoming.data();
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
			return YsArrayMask <T> (this->size()-top,this->data()+top);
		}
		else
		{
			if(GetN()<top+length)
			{
				length=GetN()-top;
			}
			return YsArrayMask <T> (length,this->data()+top);
		}
	}

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

	/*! Returns the number of items in the array.
	*/
	const YSSIZE_T GetN(void) const
	{
		return len;
	}

	/*! Returns the last element of the array.
	*/
	const T &Last(void) const
	{
		return dat[GetN()-1];
	}

	/*! Returns the last element of the array.
	*/
	T &Last(void)
	{
		return dat[GetN()-1];
	}

	/*! Returns the (*this)[idx], but assuming that the array is cyclic.
	    GetCyclic(-1) is same as Last().
	    GetCyclic(GetN()) is same as (*this)[0].
	*/
	T &GetCyclic(YSSIZE_T idx)
	{
		idx=idx%GetN();
		if(0>idx)
		{
			idx+=GetN();
		}
		return dat[idx];
	}

	/*! Returns the (*this)[idx], but assuming that the array is cyclic.
	    GetCyclic(-1) is same as Last().
	    GetCyclic(GetN()) is same as (*this)[0].
	*/
	const T &GetCyclic(YSSIZE_T idx) const
	{
		idx=idx%GetN();
		if(0>idx)
		{
			idx+=GetN();
		}
		return dat[idx];
	}

	/*! Returns YSTRUE if x is one of the items in this array, YSFALSE otherwise.
	*/
	YSBOOL IsIncluded(const T &x) const
	{
		for(YSSIZE_T idx=0; idx<len; ++idx)
		{
			if(dat[idx]==x)
			{
				return YSTRUE;
			}
		}
		return YSFALSE;
	}

// STL-like iterator support begin >>
public:
	typedef YsIndexIterator <YsArrayMask<T>,T> iterator;
	typedef YsIndexIterator <const YsArrayMask<T>,const T> const_iterator;
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
	    Useful when something needs to be done before and after an array item. */
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
		return (*this)[0];
	}
	/*! For better interoperability with STL */
	inline T &front(void)
	{
		return (*this)[0];
	}
	const T *data(void) const
	{
		return dat;
	}
	T *data(void)
	{
		return dat;
	}
};

// STL-like iterator support begin >>
template <class T>
inline typename YsArrayMask <T>::iterator begin(YsArrayMask <T> &array)
{
	return array.begin();
}

template <class T>
inline typename YsArrayMask <T>::iterator end(YsArrayMask <T> &array)
{
	return array.end();
}

template <class T>
inline typename YsArrayMask <T>::const_iterator begin(const YsArrayMask <T> &array)
{
	return array.begin();
}

template <class T>
inline typename YsArrayMask <T>::const_iterator end(const YsArrayMask <T> &array)
{
	return array.end();
}

template <class T>
inline typename YsArrayMask <T>::iterator rbegin(YsArrayMask <T> &array)
{
	return array.rbegin();
}

template <class T>
inline typename YsArrayMask <T>::iterator rend(YsArrayMask <T> &array)
{
	return array.rend();
}

template <class T>
inline typename YsArrayMask <T>::const_iterator rbegin(const YsArrayMask <T> &array)
{
	return array.rbegin();
}

template <class T>
inline typename YsArrayMask <T>::const_iterator rend(const YsArrayMask <T> &array)
{
	return array.rend();
}


template <class T>
inline YsIndexRange YsArrayMask <T>::AllIndex(void) const
{
	YsIndexRange allIndex(GetN());
	return allIndex;
}

template <class T>
inline YsReverseIndexRange YsArrayMask <T>::ReverseIndex(void) const
{
	YsReverseIndexRange allIndex(GetN());
	return allIndex;
}
// STL-like iterator support end <<


template <class T>
YsArrayMask <T> YsMakeArrayMask(YSSIZE_T len,T *dat)
{
	YsArrayMask <T> mask(len,dat);
	return mask;
}


////////////////////////////////////////////////////////////

/*! YsConstArrayMask class is for dealing with an array defined by a length and a const pointer, say YSSIZE_T len and const T *dat.
    The array has a range dat[0]...dat[len-1], but STL-like iterator and range-based-for cannot be used for this
    kind of array.

    This class covers up such an array make STL-like iterator and range-based-for.

    It is also useful for using it as a data type for receiving an YsArray.  YsArray takes MinimumLength and SizeType
    in addition to the data type that the array stores for performance and memory-efficiency reason.  However,
    in many situations, you want to write a function that can accept a YsArray regardless of MinimumLength and SizeType.

    Without this class, only way to write such a function was to write a templated function.  However, it forces the
    function-body to be written in the header file, and the header-file size becomes excessively large.  Instead,
    you can write a function that takes YsConstArrayMask <T>, and write the function body in .cpp file.

    It also allows accepting std::vector class with nearly-zero performance penalty.
*/
template <class T>
class YsConstArrayMask
{
private:
	YSSIZE_T len;
	const T *dat;

public:
	/*! Constructor that constructs this object from a length and a pointer.
	*/
	YsConstArrayMask(YSSIZE_T len,const T *dat)
	{
		this->len=len;
		this->dat=dat;
	}

// Want to make it a cast operator in YsArray
//	/*! Constructor that constructs this object from a YsArray.
//	*/
//	template <const int N,class SizeType>
//	inline YsConstArrayMask(class YsArray <T,N,SizeType> &);

	/*! Constructor that constructs this object from a std::vector <T>.
	*/
	inline YsConstArrayMask(const std::vector <T> &incoming)
	{
		this->len=(YSSIZE_T)incoming.size();
		this->dat=incoming.data();
	}

	inline YsConstArrayMask(const YsArrayMask <T> &arrayMask)
	{
		this->len=(YSSIZE_T)arrayMask.size();
		this->dat=arrayMask.data();
	}

	/*! Operator for accessing the array. */
	inline const T &operator[](YSSIZE_T idx) const
	{
		return dat[idx];
	}

	/*! Cast operator that returns constant pointer to the array. */
	operator const T *() const
	{
		return dat;
	}

	/*! Returns the pointer to the array. */
	const T *GetArray(void) const
	{
		return dat;
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

	/*! Returns the number of items in this array.
	*/
	const YSSIZE_T GetN(void) const
	{
		return len;
	}

	/*! Returns the last element of this array.
	*/
	const T &Last(void) const
	{
		return dat[GetN()-1];
	}

	/*! Returns the last element of this array.
	*/
	const T &Last(void)
	{
		return dat[GetN()-1];
	}

	/*! Returns (*this)[idx], but assuming that the array is cyclic.
	    GetCyclic(-1) is same as Last().
	    GetCyclic(GetN()) is same as (*this)[0].
	*/
	const T &GetCyclic(YSSIZE_T idx) const
	{
		idx=idx%GetN();
		if(0>idx)
		{
			idx+=GetN();
		}
		return dat[idx];
	}

	/*! Returns YSTRUE if x is one of the items in this array, YSFALSE otherwise.
	*/
	YSBOOL IsIncluded(const T &x) const
	{
		for(YSSIZE_T idx=0; idx<len; ++idx)
		{
			if(dat[idx]==x)
			{
				return YSTRUE;
			}
		}
		return YSFALSE;
	}

// STL-like iterator support begin >>
public:
	typedef YsIndexIterator <const YsConstArrayMask<T>,const T> const_iterator;
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
	    Useful when something needs to be done before and after an array item. */
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
	/*! For better interoperability with STL */
	inline const T &back(void) const
	{
		return Last();
	}
	inline const T &front(void) const
	{
		return (*this)[0];
	}
	const T *data(void) const
	{
		return dat;
	}
};

// STL-like iterator support begin >>
template <class T>
inline typename YsConstArrayMask <T>::const_iterator begin(const YsConstArrayMask <T> &array)
{
	return array.begin();
}

template <class T>
inline typename YsConstArrayMask <T>::const_iterator end(const YsConstArrayMask <T> &array)
{
	return array.end();
}

template <class T>
inline typename YsConstArrayMask <T>::const_iterator rbegin(const YsConstArrayMask <T> &array)
{
	return array.rbegin();
}

template <class T>
inline typename YsConstArrayMask <T>::const_iterator rend(const YsConstArrayMask <T> &array)
{
	return array.rend();
}


template <class T>
inline YsIndexRange YsConstArrayMask <T>::AllIndex(void) const
{
	YsIndexRange allIndex(GetN());
	return allIndex;
}

template <class T>
inline YsReverseIndexRange YsConstArrayMask <T>::ReverseIndex(void) const
{
	YsReverseIndexRange allIndex(GetN());
	return allIndex;
}

// STL-like iterator support end <<


template <class T>
YsConstArrayMask <T> YsMakeArrayMask(YSSIZE_T len,const T *dat)
{
	YsConstArrayMask <T> mask(len,dat);
	return mask;
}


////////////////////////////////////////////////////////////
/* } */
#endif
