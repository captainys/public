#ifndef YSPARALLELMERGESORT_IS_INCLUDED
#define YSPARALLELMERGESORT_IS_INCLUDED
/* { */


#include <ysclass.h>
#include "ysthreadpool.h"


template <typename KeyClass,typename ValueClass,class KeyComparer>
YSRESULT YsParallelMergeSort(YSSIZE_T nKey,KeyClass key[],ValueClass value[],YsThreadPool &threadPool)
{
	KeyClass *keyBuf=new KeyClass [nKey];
	ValueClass *valueBuf=nullptr;
	if(NULL!=value)
	{
		valueBuf=new ValueClass [nKey];
	}

	YSRESULT res=YSOK;

	auto nThread=threadPool.size();
	if(nThread<=1 || nKey<nThread*2)
	{
		res=YsMergeSortWithBuffer<KeyClass,ValueClass,KeyComparer>(nKey,key,value,keyBuf,valueBuf);
	}
	else
	{
		std::vector <std::function <void()> > taskArray;
		std::vector <YSSIZE_T> subArrayRange;
		taskArray.resize(nThread);
		subArrayRange.push_back(0);
		for(int thrIdx=0; thrIdx<nThread; ++thrIdx)
		{
			auto i0=nKey* thrIdx   /nThread;
			auto i1=nKey*(thrIdx+1)/nThread;

			subArrayRange.push_back(i1);

			auto n=i1-i0;
			auto keyPtr=key+i0;
			auto keyBufPtr=keyBuf+i0;
			auto valuePtr=(nullptr!=value ? value+i0 : nullptr);
			auto valueBufPtr=(nullptr!=valueBuf ? valueBuf+i0 : nullptr);
			taskArray[thrIdx]=std::bind(YsMergeSortWithBuffer<KeyClass,ValueClass,KeyComparer>,n,keyPtr,valuePtr,keyBufPtr,valueBufPtr);
		}

		threadPool.Run(nThread,taskArray.data());

		// I have nThread sorted arrays that need to be merged.
		std::vector <YSSIZE_T> newSubArrayRange;
		auto fromKey=key;
		auto toKey=keyBuf;
		auto fromValue=value;
		auto toValue=valueBuf;

		while(2<subArrayRange.size())
		{
			newSubArrayRange.clear();
			newSubArrayRange.push_back(0);
			for(YSSIZE_T subArrayIdx=0; subArrayIdx<=subArrayRange.size()-2; subArrayIdx+=2)
			{
				if(subArrayIdx<=subArrayRange.size()-3)
				{
					auto i0=subArrayRange[subArrayIdx];
					auto i1=subArrayRange[subArrayIdx+1];
					auto j0=subArrayRange[subArrayIdx+1];
					auto j1=subArrayRange[subArrayIdx+2];
					auto i=i0,j=j0,to=i0;
					newSubArrayRange.push_back(j1);
					while(i<i1 || j<j1)
					{
						if(i<i1 && j<j1)
						{
							if(KeyComparer::AIsGreaterThanB(fromKey[i],fromKey[j]))
							{
								toKey[to]=fromKey[j];
								if(nullptr!=value)
								{
									toValue[to]=fromValue[j];
								}
								++j;
							}
							else
							{
								toKey[to]=fromKey[i];
								if(nullptr!=value)
								{
									toValue[to]=fromValue[i];
								}
								++i;
							}
						}
						else if(i<i1)
						{
							toKey[to]=fromKey[i];
							if(nullptr!=value)
							{
								toValue[to]=fromValue[i];
							}
							++i;
						}
						else if(j<j1)
						{
							toKey[to]=fromKey[j];
							if(nullptr!=value)
							{
								toValue[to]=fromValue[j];
							}
							++j;
						}
						to++;
					}
				}
				else
				{
					auto i0=subArrayRange[subArrayIdx];
					auto i1=subArrayRange[subArrayIdx+1];
					newSubArrayRange.push_back(i1);
					for(auto i=i0; i<i1; ++i)
					{
						toKey[i]=fromKey[i];
						if(nullptr!=value)
						{
							toValue[i]=fromValue[i];
						}
					}
				}
			}
			std::swap(fromKey,toKey);
			std::swap(fromValue,toValue);
			std::swap(subArrayRange,newSubArrayRange);
		}

		if(fromKey!=key)
		{
			for(YSSIZE_T i=0; i<nKey; ++i)
			{
				key[i]=fromKey[i];
				if(nullptr!=value)
				{
					value[i]=fromValue[i];
				}
			}
		}
	}

	delete [] keyBuf;
	delete [] valueBuf;

	return res;
}

template <typename KeyClass,class KeyComparer>
YSRESULT YsParallelMergeSort(YSSIZE_T nKey,KeyClass key[],YsThreadPool &threadPool)
{
	return YsParallelMergeSort <KeyClass,int,KeyComparer> (nKey,key,nullptr,threadPool);
}

template <typename KeyClass,typename ValueClass>
YSRESULT YsSimpleParallelMergeSort(YSSIZE_T nKey,KeyClass key[],ValueClass value[],YsThreadPool &threadPool)
{
	return YsParallelMergeSort <KeyClass,ValueClass,YsDefaultComparer <KeyClass> >(nKey,key,value,threadPool);
}

template <typename KeyClass>
YSRESULT YsSimpleParallelMergeSort(YSSIZE_T nKey,KeyClass key[],YsThreadPool &threadPool)
{
	return YsParallelMergeSort <KeyClass,YsDefaultComparer <KeyClass> >(nKey,key,threadPool);
}


/* } */
#endif
