/* ////////////////////////////////////////////////////////////

File Name: yssort.h
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

#ifndef YSSORT_IS_INCLUDED
#define YSSORT_IS_INCLUDED
/* { */

#include "ysdef.h"
#include <stdio.h>
#include <stdlib.h>

/*! \file */

/*! This function sorts array items using Quick-sort algorithm based on the key values.
    This function also can take associated values that can sorted together.
    The keyType class must have math operators defined.
    This function also requires minimum and maximum of key values.  If those are not
    known at the time of sorting, there is an overloaded function that internally 
    calculates minimum and maximum.
    For sorting pointers, you can use YsQuickSortAny function insted.

    \sa YsQuickSortAny
    \tparam keyType Class of key values
    \tparam asscoType Class of associated values.  If nothing is associated, specify int.
    \param n [In] Number of items
    \param keyValue [In/Out] Array of key values
    \param associatedValue [In/Out] Array of associated values
    \param rangeMinimum [In] Minimum of the key values
    \param rangeMaximum [In] Maximum of the key values
*/
template < class keyType,class assocType >
YSRESULT YsQuickSort(YSSIZE_T n,keyType keyValue[],assocType associatedValue[],keyType rangeMinimum,keyType rangeMaximum)
{
	// int i;  // ####
	// printf("*\n");
	// printf("n=%d\n",n);
	// for(i=0; i<n; i++)
	// {
	// 	printf("%04d ",keyValue[i]);
	// 	if(i%8==7)
	// 	{
	// 		printf("\n");
	// 	}
	// }
	// printf("\n");


	if(n>1)
	{
		keyType threshold;

		threshold=(rangeMinimum+rangeMaximum)/keyType(2);

		// printf("thr=%d\n",threshold); // ####

		// smaller comes first
		keyType d0;
		YSSIZE_T i,nSmall;

		nSmall=0;
		d0=keyValue[0];

		keyType maximumAmongSmaller,minimumAmongLarger;
		maximumAmongSmaller=rangeMinimum;
		minimumAmongLarger=rangeMaximum;
		for(i=0; i<n; i++) // Actually can begin with 1
		{
			keyType var;
			var=keyValue[i];
			if(var<=threshold)
			{
				maximumAmongSmaller=YsGreater(maximumAmongSmaller,var);

				keyType a;
				a=keyValue[i];
				keyValue[i]=keyValue[nSmall];
				keyValue[nSmall]=a;

				if(associatedValue!=NULL)
				{
					assocType x;
					x=associatedValue[i];
					associatedValue[i]=associatedValue[nSmall];
					associatedValue[nSmall]=x;
				}

				nSmall++;
			}
			else
			{
				minimumAmongLarger=YsSmaller(minimumAmongLarger,var);
			}
		}

		if(nSmall!=0 && nSmall!=n)
		{
			// printf("Small=%d\n",nSmall);  // ####
			// for(i=0; i<nSmall; i++)
			// {
			// 	printf("%04d ",keyValue[i]);
			// 	if(i%8==7)
			// 	{
			// 		printf("\n");
			// 	}
			// }
			// printf("\n");
			// printf("Large=%d\n",n-nSmall);  // ####
			// for(i=0; i<n-nSmall; i++)
			// {
			// 	printf("%04d ",(keyValue+nSmall)[i]);
			// 	if(i%8==7)
			// 	{
			// 		printf("\n");
			// 	}
			// }
			// printf("\n");

			if(associatedValue!=NULL)
			{
				YsQuickSort<keyType,assocType>
				    (nSmall    ,keyValue       ,associatedValue       ,rangeMinimum,maximumAmongSmaller);
				YsQuickSort<keyType,assocType>
				    ((n-nSmall),keyValue+nSmall,associatedValue+nSmall,minimumAmongLarger,rangeMaximum);
			}
			else
			{
				YsQuickSort<keyType,assocType>
				    (nSmall    ,keyValue       ,NULL,rangeMinimum,maximumAmongSmaller);
				YsQuickSort<keyType,assocType>
				    ((n-nSmall),keyValue+nSmall,NULL,minimumAmongLarger,rangeMaximum);
			}
		}
		// >> This portion is added on 2002/06/09
		// Now nSmall==0 || nSmall==n.  It means either:
		//   all values are equal, or
		//   keyType is integer, and max-min=1
		else
		{
			YSSIZE_T i;
			keyType min;
			min=keyValue[0];
			for(i=1; i<n; i++)
			{
				min=YsSmaller(min,keyValue[i]);
			}

			nSmall=0;
			// for(nSmall=0; keyValue[nSmall]==min && nSmall<n; nSmall++) // nSmall<n is added on 2004/10/06
			for(nSmall=0; nSmall<n && keyValue[nSmall]==min; nSmall++) // nSmall<n must be checked first (2004/12/07)
			{
			}

			for(i=n-1; i>nSmall; i--)
			{
				if(keyValue[i]==min)
				{
					YsSwapSomething <keyType> (keyValue[i],keyValue[nSmall]);
					if(associatedValue!=NULL)
					{
						YsSwapSomething <assocType> (associatedValue[i],associatedValue[nSmall]);
					}
					nSmall++;
				}
			}
		}
		// << This portion is added on 2002/06/09
	}
	return YSOK;
}

/*! This function sorts array items using Quick-sort algorithm based on the key values.
    This function also can take associated values that can sorted together.
    The keyType class must have math operators defined.
    For sorting pointers, you can use YsQuickSortAny function insted.

    \sa YsQuickSortAny
    \tparam keyType Class of key values
    \tparam asscoType Class of associated values.  If nothing is associated, specify int.
    \param n [In] Number of items
    \param keyValue [In/Out] Array of key values
    \param associatedValue [In/Out] Array of associated values
*/
template <class keyType,class assocType>
YSRESULT YsQuickSort(YSSIZE_T n,keyType keyValue[],assocType associatedValue[]=NULL)
{
	int i;
	keyType keyMin,keyMax;
	if(n>1)
	{
		keyMin=keyValue[0];
		keyMax=keyValue[0];
		for(i=1; i<n; i++)
		{
			keyMin=YsSmaller(keyMin,keyValue[i]);
			keyMax=YsGreater(keyMax,keyValue[i]);
		}
		return YsQuickSort(n,keyValue,associatedValue,keyMin,keyMax);
	}
	return YSOK;  // Don't have to sort.
}


/*! This function sorts array items using Quick-sort algorithm based on the key values.
    This function also can take associated values that can sorted together.
    The keyType class requires overloaded compare operators but does not have to have overloaded math operators.
    This function thus can be used for sorting pointers.

    \sa YsQuickSort
    \tparam keyType Class of key values
    \tparam asscoType Class of associated values.  If nothing is associated, specify int.
    \param n [In] Number of items
    \param keyValue [In/Out] Array of key values
    \param assocValue [In/Out] Array of associated values
    \param watch [In] If true, it prints debugging information.
*/
template <class keyType,class assocType>
YSRESULT YsQuickSortAny(YSSIZE_T n,keyType keyValue[],assocType assocValue[],YSBOOL watch=YSFALSE)
{
	if(n<=1)
	{
		return YSOK;
	}

	YSSIZE_T i,dividePoint;
	keyType divider,nonDivider;
	YSBOOL allEqual;
	divider=keyValue[n/2];
	nonDivider=(keyType)0;

	//if(watch==YSTRUE)
	//{
	//	YsPrintf("YsQuickSortSortAny %d\n",n);
	//}

TRYAGAIN:
	dividePoint=0;
	allEqual=YSTRUE;
	for(i=0; i<n; i++)
	{
		if(allEqual==YSTRUE && keyValue[i]!=divider)  // Remark: This check must come before swapping.
		{
			nonDivider=keyValue[i];
			allEqual=YSFALSE;
		}

		if(keyValue[i]<divider)
		{
			if(i!=dividePoint)
			{
				YsSwapSomething <keyType> (keyValue[dividePoint],keyValue[i]);
				if(assocValue!=NULL)
				{
					YsSwapSomething <assocType> (assocValue[dividePoint],assocValue[i]);
				}
			}
			dividePoint++;
		}
	}

	if(allEqual==YSTRUE)
	{
		if(watch==YSTRUE)
		{
			printf("All Equal\n");
		}
		return YSOK;
	}

	if(dividePoint==0 && nonDivider!=0)
	{
		divider=nonDivider;
		goto TRYAGAIN;
	}

	// 2010/01/18 >>
	if(0==dividePoint || n==dividePoint)
	{
		// Desperate situation.  Bring the minimum to the left, and then divide it into 1:n-1
		for(i=1; i<n; i++)
		{
			if(keyValue[i]<keyValue[0])
			{
				YsSwapSomething <keyType> (keyValue[0],keyValue[i]);
				if(assocValue!=NULL)
				{
					YsSwapSomething <assocType> (assocValue[0],assocValue[i]);
				}
			}
		}
		dividePoint=1;
	}
	// 2010/01/18 <<

	if(watch==YSTRUE)
	{
		printf("Next: %d,%d\n",(int)dividePoint,(int)(n-dividePoint));
	}

	if(assocValue!=NULL)
	{
		YsQuickSortAny <keyType,assocType> (dividePoint,keyValue,assocValue,watch);
		YsQuickSortAny <keyType,assocType> ((n-dividePoint),keyValue+dividePoint,assocValue+dividePoint,watch);
	}
	else
	{
		YsQuickSortAny <keyType,assocType> (dividePoint,keyValue,NULL,watch);
		YsQuickSortAny <keyType,assocType> ((n-dividePoint),keyValue+dividePoint,NULL,watch);
	}

	return YSOK;
}


/*! This function shuffles an array keyValue[].  If non-NULL array is given to assocType, associated
    array assocType[] will also be shuffled.  Array assocType[] must be at least as long as keyValue[].
    If there is no associated array, specify assocType=int, and assoc

	\tparam keyType Type of the array to be shuffled.
	\tparam assocType Type of the associated array to be shuffled.

	\param n Length of the array.
	\param keyValue An array to be shuffled.
	\param assocType An array associated with keyValue array.
*/
template <class keyType,class assocType>
void YsShuffle(YSSIZE_T n,keyType keyValue[],assocType assocValue[]=NULL)
{
	if(RAND_MAX>=n)  // No issue
	{
		for(YSSIZE_T i=0; i<n; ++i)
		{
			YSSIZE_T r=rand()%n;

			if(NULL!=keyValue)
			{
				keyType swp=keyValue[i];
				keyValue[i]=keyValue[r];
				keyValue[r]=swp;
			}

			if(NULL!=assocValue)
			{
				assocType swp=assocValue[i];
				assocValue[i]=assocValue[r];
				assocValue[r]=swp;
			}
		}
	}
	else
	{
		for(YSSIZE_T i=0; i<n; ++i)
		{
#if 32767==RAND_MAX || 32768==RAND_MAX
			YSSIZE_T r=rand();
			r<<=15;
			r+=rand();
			r<<=15;
			r+=rand();
			r<<=15;
			r+=rand(); // Good up to 60 bit
#elif 0x7fffffff==RAND_MAX || 0x80000000==RAND_MAX
			YSSIZE_T r=rand();
			r<<=31;
			r+=rand(); // Good up to 62 bit
#else
			YSSIZE_T r=rand();
#endif

			if(0>r)
			{
				r++;
				r=-r;
			}
			r%=n;

			if(NULL!=keyValue)
			{
				keyType swp=keyValue[i];
				keyValue[i]=keyValue[r];
				keyValue[r]=swp;
			}

			if(NULL!=assocValue)
			{
				assocType swp=assocValue[i];
				assocValue[i]=assocValue[r];
				assocValue[r]=swp;
			}
		}
	}
}


/* } */
#endif
