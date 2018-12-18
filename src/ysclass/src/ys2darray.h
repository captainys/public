/* ////////////////////////////////////////////////////////////

File Name: ys2darray.h
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

#ifndef YS_DARRAY_IS_INCLUDED
#define YS_DARRAY_IS_INCLUDED
/* { */

#include "ysdef.h"

/*
A variable lengtn array can be made as small as an integer and a pointer, which takes 12 bytes.
However, because of 8-byte alignment, it will take 16 bytes anyway.

This extra 4-bytes are giving like 100+MB penalty for a large if the data set size is in the order of 20 megs.

This two-dimensional array class is for cutting down this 100+MB penalty in such situations.

	<template class T>
	class Something
	{
	public:
		int n;
		T *v;
	};

This is supposed to be 12 bytes, but takes 16 bytes.  Confirmed with sizeof(Something<int>).


*/

template <class T,class SizeType=YSSIZE_T>
class Ys2DArray
{
private:
	SizeType n1;
	SizeType *n2;
	T **v;
	SizeType nTotal;

public:
	YSBOOL verboseMode;

public:
	Ys2DArray();
	~Ys2DArray();
	Ys2DArray(YSSIZE_T n1);
	Ys2DArray(const Ys2DArray &from);
	Ys2DArray &operator=(const Ys2DArray &from);
	Ys2DArray(Ys2DArray &&from);
	Ys2DArray &operator=(Ys2DArray &&from);

	T *operator[](YSSIZE_T idx);
	const T *operator[](YSSIZE_T idx) const;

	/*! This function copies the incoming value to all items in the array. */
	YSRESULT SetAll(const T &incoming);

	YSRESULT CopyFrom(const Ys2DArray &from);
	YSRESULT MoveFrom(Ys2DArray &from);

private:
	void Initialize(void);
	SizeType NRequired(SizeType forN) const;
	SizeType NextTwoToTheNth(SizeType current) const;
	YSBOOL IsTwoToTheNth(SizeType current) const;
	YSBOOL IncrementWillRaiseMSB(SizeType current) const;
	YSBOOL DecrementWillLowerMSB(SizeType current) const;

public:
	void CleanUp(void);

	YSBOOL IsInRange(YSSIZE_T i1,YSSIZE_T i2) const;

	YSRESULT Resize(YSSIZE_T n1,YSSIZE_T n2);

	YSRESULT ResizeN1(YSSIZE_T newSize);
	YSRESULT IncrementN1(void);

	YSRESULT ResizeN2(YSSIZE_T n1Idx,YSSIZE_T newN2);
	YSRESULT IncrementN2(YSSIZE_T n1Idx);
	YSRESULT DecrementN2(YSSIZE_T n1Idx);

	YSRESULT Append(YSSIZE_T n1Idx,const T &from);
	YSRESULT Append(YSSIZE_T n1Idx,T &&from);

	YSRESULT Insert(YSSIZE_T i1,YSSIZE_T i2,const T &from);

	YSRESULT Delete(YSSIZE_T i1,YSSIZE_T i2);
	YSRESULT DeleteBySwapping(YSSIZE_T i1,YSSIZE_T i2);

	SizeType GetN1(void) const;
	SizeType GetN2(YSSIZE_T n1Idx) const;
	SizeType GetNTotal(void) const;
};

template <class T,class SizeType>
Ys2DArray <T,SizeType>::Ys2DArray()
{
	Initialize();
}

template <class T,class SizeType>
Ys2DArray <T,SizeType>::~Ys2DArray()
{
	CleanUp();
}

template <class T,class SizeType>
Ys2DArray <T,SizeType>::Ys2DArray(YSSIZE_T n1)
{
	Initialize();
	ResizeN1(n1);
}

template <class T,class SizeType>
Ys2DArray <T,SizeType>::Ys2DArray(const Ys2DArray &from)
{
	Initialize();
	CopyFrom(from);
}

template <class T,class SizeType>
Ys2DArray <T,SizeType> &Ys2DArray <T,SizeType>::operator=(const Ys2DArray &from)
{
	CopyFrom(from);
	return *this;
}

template <class T,class SizeType>
Ys2DArray <T,SizeType>::Ys2DArray(Ys2DArray &&from)
{
	Initialize();
	MoveFrom(from);
}

template <class T,class SizeType>
Ys2DArray <T,SizeType> &Ys2DArray <T,SizeType>::operator=(Ys2DArray &&from)
{
	MoveFrom(from);
	return *this;
}

template <class T,class SizeType>
T *Ys2DArray <T,SizeType>::operator[](YSSIZE_T idx)
{
	return v[idx];
}

template <class T,class SizeType>
const T *Ys2DArray <T,SizeType>::operator[](YSSIZE_T idx) const
{
	return v[idx];
}

template <class T,class SizeType>
YSRESULT Ys2DArray <T,SizeType>::SetAll(const T &incoming)
{
	for(YSSIZE_T i1=0; i1<n1; ++i1)
	{
		for(YSSIZE_T i2=0; i2<n2[i1]; ++i2)
		{
			v[i1][i2]=incoming;
		}
	}
	return YSOK;
}

template <class T,class SizeType>
YSRESULT Ys2DArray <T,SizeType>::CopyFrom(const Ys2DArray &from)
{
	CleanUp();

	this->n1=from.n1;
	this->n2=new SizeType[from.n1];
	this->v=new T *[from.n1];

	for(YSSIZE_T idx1=0; idx1<from.n1; ++idx1)
	{
		this->n2[idx1]=from.n2[idx1];
		this->v[idx1]=new T [from.n2[idx1]];
		for(YSSIZE_T idx2=0; idx2<from.n2[idx1]; ++idx2)
		{
			this->v[idx1][idx2]=from.v[idx1][idx2];
		}
	}

	this->nTotal=from.nTotal;

	return YSOK;
}

template <class T,class SizeType>
YSRESULT Ys2DArray <T,SizeType>::MoveFrom(Ys2DArray &from)
{
	CleanUp();
	this->n1=from.n1;
	this->n2=from.n2;
	this->nTotal=from.nTotal;
	this->v=from.v;

	from.n1=0;
	from.n2=NULL;
	from.v=NULL;
	from.nTotal=0;

	return YSOK;
}

template <class T,class SizeType>
void Ys2DArray <T,SizeType>::Initialize(void)
{
	verboseMode=YSFALSE;
	n1=0;
	n2=NULL;
	v=NULL;
	nTotal=0;
}

template <class T,class SizeType>
SizeType Ys2DArray <T,SizeType>::NRequired(SizeType forN) const
{
	// N=0 -> required 0  nextTwoToTheNth 1
	// N=1 -> required 1  nextTwoToTheNth 2
	// N=2 -> required 2  nextTwoToTheNth 4
	// N=3 -> required 4  nextTwoToTheNth 4
	// N=4 -> required 4  nextTwoToTheNth 8
	// N=5 -> required 8  nextTwoToTheNth 8
	// N=6 -> required 8  nextTwoToTheNth 8
	// N=7 -> required 8  nextTwoToTheNth 8
	// N=8 -> required 8  nextTwoToTheNth 16
	// N=9 -> required 16 nextTwoToTheNth 16

	if(0==forN)
	{
		return 0;
	}
	else
	{
		return NextTwoToTheNth(forN-1);
	}
}

template <class T,class SizeType>
SizeType Ys2DArray <T,SizeType>::NextTwoToTheNth(SizeType current) const
{
	current|=(current>>1);
	current|=(current>>2);
	current|=(current>>4);
	current|=(current>>8);
	current|=(current>>16);
	current|=(current>>31);  // Covers up to 63 bit integer.  Well, it should be big enough for a while.
	return current+1;
}

template <class T,class SizeType>
YSBOOL Ys2DArray <T,SizeType>::IsTwoToTheNth(SizeType current) const
{
	if(0==current || 1==current)
	{
		return YSTRUE;
	}
	else
	{
		if(0==(current&(current-1)))
		{
			return YSTRUE;
		}
	}
	return YSFALSE;
}

template <class T,class SizeType>
YSBOOL Ys2DArray <T,SizeType>::IncrementWillRaiseMSB(SizeType current) const
{
	if(0==((current+1)&current))
	{
		return YSTRUE;
	}
	return YSFALSE;
}

template <class T,class SizeType>
YSBOOL Ys2DArray <T,SizeType>::DecrementWillLowerMSB(SizeType current) const
{
	if(0==((current-1)&current))
	{
		return YSTRUE;
	}
	return YSFALSE;
}

template <class T,class SizeType>
YSBOOL Ys2DArray <T,SizeType>::IsInRange(YSSIZE_T i1,YSSIZE_T i2) const
{
	if(0<=i1 && i1<n1 && 0<=i2 && i2<n2[i1])
	{
		return YSTRUE;
	}
	return YSFALSE;
}

template <class T,class SizeType>
void Ys2DArray <T,SizeType>::CleanUp(void)
{
	for(YSSIZE_T idx=0; idx<n1; ++idx)
	{
		delete [] v[idx];
	}
	delete [] v;
	delete [] n2;
	Initialize();
}

template <class T,class SizeType>
YSRESULT Ys2DArray <T,SizeType>::Resize(YSSIZE_T n1,YSSIZE_T n2)
{
	if(YSOK==ResizeN1(n1))
	{
		for(YSSIZE_T i1=0; i1<n1; ++i1)
		{
			if(YSOK!=ResizeN2(i1,n2))
			{
				return YSERR;
			}
		}
		return YSOK;
	}
	return YSERR;
}

template <class T,class SizeType>
YSRESULT Ys2DArray <T,SizeType>::ResizeN1(YSSIZE_T newSize)
{
	T **newV;
	SizeType *newN2;

	SizeType currentlyAvailable=NRequired(n1);
	SizeType required=NRequired((SizeType)newSize);

	if(YSTRUE==verboseMode)
	{
		printf("%s %d\n",__FUNCTION__,__LINE__);
		printf("  CurrentN1 %d  CurrentAvailable %d  Required %d\n",(int)n1,(int)currentlyAvailable,(int)required);
	}

	if(0==required)
	{
		CleanUp();
	}
	else if(currentlyAvailable!=required)
	{
		newV=new T *[required];
		newN2=new SizeType[required];

		SizeType newNTotal=0;
		for(YSSIZE_T idx=0; idx<newSize && idx<n1; ++idx)
		{
			newV[idx]=v[idx];
			newN2[idx]=n2[idx];
			newNTotal+=n2[idx];
		}
		for(YSSIZE_T idx=(n1<newSize ? n1 : newSize); idx<required; ++idx)
		{
			newV[idx]=NULL;
			newN2[idx]=0;
		}

		for(YSSIZE_T idx=newSize; idx<n1; ++idx)
		{
			if(NULL!=v[idx])
			{
				delete [] v[idx];
			}
		}

		delete [] n2;
		delete [] v;

		n1=(SizeType)newSize;
		v=newV;
		n2=newN2;
		nTotal=newNTotal;
	}
	else
	{
		// If newSize<n1, need to delete arrays beyond v[newSize].
		for(YSSIZE_T idx=newSize; idx<n1; ++idx)
		{
			nTotal-=n2[idx];
			if(NULL!=v[idx])
			{
				delete [] v[idx];
				v[idx]=NULL;
			}
		}
		n1=(SizeType)newSize;
	}
	return YSOK; // Should out-of-memory check.
}

template <class T,class SizeType>
YSRESULT Ys2DArray <T,SizeType>::IncrementN1(void)
{
	if(YSTRUE==IsTwoToTheNth(n1))
	{
		Resize(n1+1);
	}
	++n1;
	return YSOK;
}

template <class T,class SizeType>
YSRESULT Ys2DArray <T,SizeType>::ResizeN2(YSSIZE_T n1Idx,YSSIZE_T newN2)
{
	if(0<=n1Idx && n1Idx<n1)
	{
		SizeType currentlyAvailable=NRequired(n2[n1Idx]);
		SizeType required=(SizeType)NRequired((SizeType)newN2);

		if(YSTRUE==verboseMode)
		{
			printf("%s %d\n",__FUNCTION__,__LINE__);
			printf("i1 %d  n2 %d  newN2 %d  CurrentAvailable %d  Required %d\n",(int)n1Idx,(int)n2[n1Idx],(int)newN2,(int)currentlyAvailable,(int)required);
		}

		if(0==required)
		{
			if(NULL!=v[n1Idx])
			{
				delete [] v[n1Idx];
			}
			nTotal-=n2[n1Idx];
			v[n1Idx]=NULL;
			n2[n1Idx]=0;
		}
		else if(currentlyAvailable!=required)
		{
			T *newV=new T [required];

			for(YSSIZE_T idx=0; idx<n2[n1Idx] && idx<required; ++idx)
			{
				newV[idx]=v[n1Idx][idx];
			}
			if(NULL!=v[n1Idx])
			{
				delete [] v[n1Idx];
			}

			nTotal-=n2[n1Idx];
			nTotal+=(SizeType)newN2;

			v[n1Idx]=newV;
			n2[n1Idx]=(SizeType)newN2;
		}
		else
		{
			nTotal-=n2[n1Idx];
			nTotal+=(SizeType)newN2;
			n2[n1Idx]=(SizeType)newN2;
		}

		return YSOK;
	}
	else
	{
		if(YSTRUE==verboseMode)
		{
			printf("%s %d\n",__FUNCTION__,__LINE__);
			printf("  Out-of-range index access.\n");
		}
	}
	return YSERR;
}

template <class T,class SizeType>
YSRESULT Ys2DArray <T,SizeType>::IncrementN2(YSSIZE_T n1Idx)
{
	if(0<=n1Idx && n1Idx<n1)
	{
		if(YSTRUE==IsTwoToTheNth(n2[n1Idx]))
		{
			ResizeN2(n1Idx,n2[n1Idx]+1);
		}
		else
		{
			++nTotal;
			++n2[n1Idx];
		}
		return YSOK;
	}
	else
	{
		if(YSTRUE==verboseMode)
		{
			printf("%s %d\n",__FUNCTION__,__LINE__);
			printf("  Out-of-range index access.\n");
		}
	}
	return YSERR;
}

template <class T,class SizeType>
YSRESULT Ys2DArray <T,SizeType>::DecrementN2(YSSIZE_T n1Idx)
{
	if(0<=n1Idx && n1Idx<n1)
	{
		if(0==n2[n1Idx])
		{
			printf("%s %d\n",__FUNCTION__,__LINE__);
			printf("  Size is already zero.\n");
			return YSERR;
		}
		if(YSTRUE==IsTwoToTheNth(n2[n1Idx]-1))
		{
			ResizeN2(n1Idx,n2[n1Idx]-1);
		}
		else
		{
			--nTotal;
			--n2[n1Idx];
		}
		return YSOK;
	}
	else
	{
		if(YSTRUE==verboseMode)
		{
			printf("%s %d\n",__FUNCTION__,__LINE__);
			printf("  Out-of-range index access.\n");
		}
	}
	return YSERR;
}

template <class T,class SizeType>
YSRESULT Ys2DArray <T,SizeType>::Append(YSSIZE_T n1Idx,const T &from)
{
	if(0<=n1Idx && n1Idx<n1)
	{
		IncrementN2(n1Idx);
		v[n1Idx][n2[n1Idx]-1]=from;
		return YSOK;
	}
	else
	{
		if(YSTRUE==verboseMode)
		{
			printf("%s %d\n",__FUNCTION__,__LINE__);
			printf("  Out-of-range index access.\n");
		}
	}
	return YSERR;
}

template <class T,class SizeType>
YSRESULT Ys2DArray <T,SizeType>::Append(YSSIZE_T n1Idx,T &&from)
{
	if(0<=n1Idx && n1Idx<n1)
	{
		IncrementN2(n1Idx);
		v[n1Idx][n2[n1Idx]-1]=from;
		return YSOK;
	}
	else
	{
		if(YSTRUE==verboseMode)
		{
			printf("%s %d\n",__FUNCTION__,__LINE__);
			printf("  Out-of-range index access.\n");
		}
	}
	return YSERR;
}

template <class T,class SizeType>
YSRESULT Ys2DArray <T,SizeType>::Insert(YSSIZE_T i1,YSSIZE_T i2,const T &from)
{
	if(0<=i1 && i1<n1)
	{
		if(0>i2)
		{
			i2=0;
		}
		if(n2[i1]<=i2)
		{
			Append(i1,from);
		}
		else if(0<=i2 && i2<n2[i1])
		{
			IncrementN2(i1);
			for(auto idx=n2[i1]-1; i2<idx; --idx)
			{
				v[i1][idx]=v[i1][idx-1];
			}
			v[i1][i2]=from;
		}
		return YSOK;
	}
	if(YSTRUE==verboseMode)
	{
		printf("%s %d\n",__FUNCTION__,__LINE__);
		printf("  Out-of-range index access.\n");
	}
	return YSERR;
}

template <class T,class SizeType>
YSRESULT Ys2DArray <T,SizeType>::Delete(YSSIZE_T i1,YSSIZE_T i2)
{
	if(0<=i1 && i1<n1 && 0<=i2 && i2<n2[i1])
	{
		for(auto i=i2; i<n2[i1]-1; ++i)
		{
			v[i1][i]=v[i1][i+1];
		}
		DecrementN2(i1);
		return YSOK;
	}
	if(YSTRUE==verboseMode)
	{
		printf("%s %d\n",__FUNCTION__,__LINE__);
		printf("  Out-of-range index access.\n");
	}
	return YSERR;
}

template <class T,class SizeType>
YSRESULT Ys2DArray <T,SizeType>::DeleteBySwapping(YSSIZE_T i1,YSSIZE_T i2)
{
	if(0<=i1 && i1<n1 && 0<=i2 && i2<n2[i1])
	{
		v[i1][i2]=v[i1][n2[i1]-1];
		DecrementN2(i1);
		return YSOK;
	}
	if(YSTRUE==verboseMode)
	{
		printf("%s %d\n",__FUNCTION__,__LINE__);
		printf("  Out-of-range index access.\n");
	}
	return YSERR;
}

template <class T,class SizeType>
SizeType Ys2DArray <T,SizeType>::GetN1(void) const
{
	return n1;
}

template <class T,class SizeType>
SizeType Ys2DArray <T,SizeType>::GetN2(YSSIZE_T n1Idx) const
{
	if(0<=n1Idx && n1Idx<n1)
	{
		return n2[n1Idx];
	}
	else
	{
		if(YSTRUE==verboseMode)
		{
			printf("%s %d\n",__FUNCTION__,__LINE__);
			printf("  Out-of-range index access.\n");
		}
	}
	return 0;
}

template <class T,class SizeType>
SizeType Ys2DArray <T,SizeType>::GetNTotal(void) const
{
	return nTotal;
}

/* } */
#endif
