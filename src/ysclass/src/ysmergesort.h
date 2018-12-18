/* ////////////////////////////////////////////////////////////

File Name: ysmergesort.h
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

#ifndef YSMERGESORT_IS_INCLUDED
#define YSMERGESORT_IS_INCLUDED
/* { */

#include <stdio.h>

#include "ysdef.h"
#include "yscomparer.h"


template <typename KeyClass,typename ValueClass,class KeyComparer>
YSRESULT YsMergeSortMain(KeyClass *&whereSorted,KeyClass key[],ValueClass value[],KeyClass keyBuf[],ValueClass valueBuf[],YSSIZE_T i0,YSSIZE_T i1)
{
	const YSSIZE_T n=i1-i0;

	if(1>=n)
	{
		whereSorted=key;
		return YSOK;
	}
	else if(2==n)
	{
		whereSorted=key;
		if(KeyComparer::AIsGreaterThanB(key[i0],key[i0+1]))
		{
			KeyClass swp=(KeyClass &&)key[i0];
			key[i0]=(KeyClass &&)key[i0+1];
			key[i0+1]=(KeyClass &&)swp;
			if(NULL!=value)
			{
				ValueClass swp=(ValueClass &&)value[i0];
				value[i0]=(ValueClass &&)value[i0+1];
				value[i0+1]=(ValueClass &&)swp;
			}
		}
		return YSOK;
	}

	YSSIZE_T mid=(i0+i1)/2;

	// Semi-double buffering:
	// YsMergeSortMain will store sorted values in either one of the buffers (key,value) or (keyBuf,valueBuf).
	// If the two sorted arrays are in the same buffer, new sorted array will be stored in the other buffer
	// and returned to the one-level up.  If two sorted arrays are in the different buffers, one needs to be
	// copied to the other.

	// YsMergeSortMain traverses through the keys and values in the depth-first order, therefore,
	// the original (key,value) is always passed to the bottom-most function-call.
	// While climbing back up, (key,value) and (keyBuf,valueBuf) are alternately used for storing
	// merge-sorted keys and values.

	KeyClass *fromKey,*toKey;
	ValueClass *fromValue,*toValue;
	{
		KeyClass *whereSortedA,*whereSortedB;
		YsMergeSortMain<KeyClass,ValueClass,KeyComparer>(whereSortedA,key,value,keyBuf,valueBuf,i0,mid);
		YsMergeSortMain<KeyClass,ValueClass,KeyComparer>(whereSortedB,key,value,keyBuf,valueBuf,mid,i1);

		if(whereSortedA==key)
		{
			toKey=keyBuf;
			toValue=valueBuf;
			fromKey=key;
			fromValue=value;
		}
		else
		{
			toKey=key;
			toValue=value;
			fromKey=keyBuf;
			fromValue=valueBuf;
		}

		if(whereSortedB!=whereSortedA)
		{
			for(auto i=mid; i<i1; ++i)
			{
				fromKey[i]=toKey[i];
			}
			if(NULL!=value)
			{
				for(auto i=mid; i<i1; ++i)
				{
					fromValue[i]=toValue[i];
				}
			}
		}
	}

	// At this point,
	//   fromKey[i0,...,mid-1] and fromKey[mid,...,i1-1] are two sorted arrays.
	//   The two arrays are merged to toKey[i0,...,i1-1].

	YSSIZE_T ia=i0,ib=mid,i=i0;
	while(ia<mid && ib<i1)
	{
		if(KeyComparer::AIsGreaterThanB(fromKey[ia],fromKey[ib]))
		{
			toKey[i]=fromKey[ib];
			if(NULL!=value)
			{
				toValue[i]=fromValue[ib];
			}
			++ib;
		}
		else
		{
			toKey[i]=fromKey[ia];
			if(NULL!=value)
			{
				toValue[i]=fromValue[ia];
			}
			++ia;
		}
		++i;
	}
	while(ia<mid)
	{
		toKey[i]=fromKey[ia];
		if(NULL!=value)
		{
			toValue[i]=fromValue[ia];
		}
		++ia;
		++i;
	}
	while(ib<i1)
	{
		toKey[i]=fromKey[ib];
		if(NULL!=value)
		{
			toValue[i]=fromValue[ib];
		}
		++ib;
		++i;
	}

	if(ia!=mid || ib!=i1 || i!=i1)
	{
		printf("Counter error! %d %d %d %d %d\n",(int)i0,(int)i1,(int)ia,(int)ib,(int)mid);
		return YSERR;
	}

	whereSorted=toKey;

	return YSOK;
}

template <typename KeyClass,typename ValueClass,class KeyComparer>
YSRESULT YsMergeSortWithBuffer(YSSIZE_T nKey,KeyClass key[],ValueClass value[],KeyClass keyBuf[],ValueClass valueBuf[])
{
	KeyClass *whereSorted;
	auto res=YsMergeSortMain <KeyClass,ValueClass,KeyComparer> (whereSorted,key,value,keyBuf,valueBuf,0,nKey);
	if(YSOK==res)
	{
		if(whereSorted!=key)
		{
			if(nullptr!=value)
			{
				for(YSSIZE_T i=0; i<nKey; ++i)
				{
					key[i]=keyBuf[i];
					value[i]=valueBuf[i];
				}
			}
			else
			{
				for(YSSIZE_T i=0; i<nKey; ++i)
				{
					key[i]=keyBuf[i];
				}
			}
		}
	}
	return res;
}

template <typename KeyClass,typename ValueClass,class KeyComparer>
YSRESULT YsMergeSort(YSSIZE_T nKey,KeyClass key[],ValueClass value[])
{
	KeyClass *keyBuf=new KeyClass [nKey];
	ValueClass *valueBuf=nullptr;
	if(NULL!=value)
	{
		valueBuf=new ValueClass [nKey];
	}

	auto res=YsMergeSortWithBuffer<KeyClass,ValueClass,KeyComparer>(nKey,key,value,keyBuf,valueBuf);

	delete [] keyBuf;
	delete [] valueBuf;

	return res;
}

template <typename KeyClass,class KeyComparer>
YSRESULT YsMergeSort(YSSIZE_T nKey,KeyClass key[])
{
	return YsMergeSort <KeyClass,int,KeyComparer> (nKey,key,NULL);
}

template <typename KeyClass,typename ValueClass>
YSRESULT YsSimpleMergeSort(YSSIZE_T nKey,KeyClass key[],ValueClass value[])
{
	return YsMergeSort <KeyClass,ValueClass,YsDefaultComparer <KeyClass> >(nKey,key,value);
}

template <typename KeyClass>
YSRESULT YsSimpleMergeSort(YSSIZE_T nKey,KeyClass key[])
{
	return YsMergeSort <KeyClass,YsDefaultComparer <KeyClass> >(nKey,key);
}



/* } */
#endif
