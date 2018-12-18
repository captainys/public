/* ////////////////////////////////////////////////////////////

File Name: yskdtreeutil.h
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

#ifndef YSKDTREEUTIL_IS_INCLUDED
#define YSKDTREEUTIL_IS_INCLUDED
/* { */

#include "yskdtree.h"

// Based on Russell A. Brown, "Building a Balanced k-d Tree in O(kn log n) Time", Journal of Computer Graphics Techniques (JCGT), vol. 4, no. 1, 50-68, 2015
// http://arxiv.org/pdf/1410.5420v27.pdf

template <class kdTreeType,class VecType,const int kDim,typename ATTRIB>
static void YsBuildBalancedKdTreeMain(kdTreeType &kdTree,YSSIZE_T np,const VecType pos[],const ATTRIB attrib[]);

template <class kdTreeType,class VecType,const int kDim,typename ATTRIB>
static void YsBuildBalancedKdTreeOneStep(
	kdTreeType &kdTree,YSSIZE_T np,const VecType pos[],const ATTRIB attrib[],YsStaticArray <YSSIZE_T,kDim> indexArray[],YSSIZE_T nIndex,int thisAxis,
	YSBOOL isInUpperHalf[],YSSIZE_T tempIndex0[],YSSIZE_T tempIndex1[]);



template <typename ATTRIB>
void YsBuildBalanced3dTree(Ys3dTree <ATTRIB> &kdTree,YSSIZE_T np,const YsVec3 pos[],const ATTRIB attrib[]=NULL)
{
	return YsBuildBalancedKdTreeMain <Ys3dTree<ATTRIB>,YsVec3,3,ATTRIB>(kdTree,np,pos,attrib);
}

template <typename ATTRIB>
void YsBuildBalanced3dTree(Ys3dTree <ATTRIB> &kdTree,const YsConstArrayMask <YsVec3> posArray,const ATTRIB attribArray[])
{
	return YsBuildBalancedKdTreeMain <Ys3dTree<ATTRIB>,YsVec3,3,ATTRIB>(kdTree,posArray.GetN(),posArray,attribArray);
}

template <typename ATTRIB>
void YsBuildBalanced3dTree(Ys3dTree <ATTRIB> &kdTree,const YsConstArrayMask <YsVec3> posArray,const YsConstArrayMask <ATTRIB> attribArray)
{
	return YsBuildBalancedKdTreeMain <Ys3dTree<ATTRIB>,YsVec3,3,ATTRIB>(kdTree,posArray.GetN(),posArray,attribArray);
}

template <typename ATTRIB,const int N>
void YsBuildBalanced3dTree(Ys3dTree <ATTRIB> &kdTree,const YsConstArrayMask <YsVec3> posArray,const YsArray <ATTRIB,N> &attribArray)
{
	return YsBuildBalancedKdTreeMain <Ys3dTree<ATTRIB>,YsVec3,3,ATTRIB>(kdTree,posArray.GetN(),posArray,attribArray);
}

template <typename ATTRIB>
void YsBuildBalanced3dTree(Ys3dTree <ATTRIB> &kdTree,const YsConstArrayMask <YsVec3> posArray)
{
	return YsBuildBalancedKdTreeMain <Ys3dTree<ATTRIB>,YsVec3,3,ATTRIB>(kdTree,posArray.GetN(),posArray,NULL);
}



template <class kdTreeType,class VecType,const int kDim,typename ATTRIB>
static void YsBuildBalancedKdTreeMain(kdTreeType &kdTree,YSSIZE_T np,const VecType pos[],const ATTRIB attrib[])
{
	YsArray <YsStaticArray <YSSIZE_T,kDim> > indexArray(np,NULL);

	{
		YsArray <YSSIZE_T> indexSorter(np,NULL);
		YsArray <double> componentSorter(np,NULL);

		for(int axis=0; axis<kDim; ++axis)
		{
			for(YSSIZE_T i=0; i<np; ++i)
			{
				indexSorter[i]=i;
				componentSorter[i]=pos[i][axis];
			}
			YsQuickSort(componentSorter.GetN(),componentSorter.GetEditableArray(),indexSorter.GetEditableArray());
			for(YSSIZE_T i=0; i<np; ++i)
			{
				indexArray[i][axis]=indexSorter[i];
			}
		}
	}

	YsArray <YSBOOL> isInUpperHalf(np,NULL);
	YsArray <YSSIZE_T> tempIndex0(np,NULL),tempIndex1(np,NULL);


	YsBuildBalancedKdTreeOneStep<kdTreeType,VecType,kDim,ATTRIB>(kdTree,np,pos,attrib,indexArray,indexArray.GetN(),0,isInUpperHalf,tempIndex0,tempIndex1);
}


template <class kdTreeType,class VecType,const int kDim,typename ATTRIB>
static void YsBuildBalancedKdTreeOneStep(
	kdTreeType &kdTree,YSSIZE_T np,const VecType pos[],const ATTRIB attrib[],YsStaticArray <YSSIZE_T,kDim> indexArray[],YSSIZE_T nIndex,int thisAxis,
	YSBOOL isInUpperHalf[],YSSIZE_T tempIndex0[],YSSIZE_T tempIndex1[])
{
	// Comparison with the paper >>
	// {
	// 	printf("In\n");
	// 	YsConstArrayMask <YsStaticArray <YSSIZE_T,kDim> > all(nIndex,indexArray);
	// 	for(auto &idx : all)
	// 	{
	//		for(int d=0; d<kDim; ++d)
	//		{
	//			printf("%3d ",idx[d]);
	//		}
	// 		printf("\n");
	// 	}
	// }
	// Comparison with the paper <<

	if(0==nIndex)
	{
		return;
	}
	else if(1==nIndex)
	{
		auto idx=indexArray[0][thisAxis];
		if(NULL!=attrib)
		{
			kdTree.Add(pos[idx],attrib[idx]);
		}
		else
		{
			kdTree.Add(pos[idx]);
		}
	}
	else
	{
		auto medIdx=nIndex/2;
		auto justUsed=indexArray[medIdx][thisAxis];
		if(NULL!=attrib)
		{
			kdTree.Add(pos[justUsed],attrib[justUsed]);
		}
		else
		{
			kdTree.Add(pos[justUsed]);
		}



		{
			// isInUpperHalf, tempIndex0, tempIndex1 are only used in this block.
			// Do not have to survive the recursion.
			for(YSSIZE_T i=0; i<nIndex; ++i)
			{
				isInUpperHalf[indexArray[i][thisAxis]]=(i<medIdx ? YSTRUE : YSFALSE);
			}

			for(int otherAxis=0; otherAxis<kDim; ++otherAxis)
			{
				if(thisAxis!=otherAxis)
				{
					YSSIZE_T j0=0,j1=0;
					for(YSSIZE_T i=0; i<nIndex; ++i)
					{
						auto otherIdx=indexArray[i][otherAxis];
						if(otherIdx!=justUsed)
						{
							if(YSTRUE==isInUpperHalf[otherIdx])
							{
								tempIndex0[j0]=otherIdx;
								++j0;
							}
							else
							{
								tempIndex1[j1]=otherIdx;
								++j1;
							}
						}
					}
					const auto nj0=j0,nj1=j1;
					j0=0;
					j1=0;
					for(YSSIZE_T i=0; i<nIndex; ++i)
					{
						if(i!=medIdx)
						{
							if(j0<nj0)
							{
								indexArray[i][otherAxis]=tempIndex0[j0];
								++j0;
							}
							else
							{
								indexArray[i][otherAxis]=tempIndex1[j1];
								++j1;
							}
						}
						else
						{
							indexArray[i][otherAxis]=-1;
						}
					}
					// Small debugging
					if(j0!=nj0 || j1!=nj1)
					{
						printf("j0,j1 does not match nj0,nj1 %d %d %d %d  ThisAxis %d OtherAxis %d\n",(int)j0,(int)j1,(int)nj0,(int)nj1,(int)thisAxis,(int)otherAxis);
					}
				}
			}
		}


		// Comparison with the paper >>
		// {
		// 	printf("out\n");
		// 	YsConstArrayMask <YsStaticArray <YSSIZE_T,3> > all(nIndex,indexArray);
		// 	for(auto &idx : all)
		// 	{
		//		for(int d=0; d<kDim; ++d)
		//		{
		//			printf("%3d ",idx[d]);
		//		}
		// 		printf("\n");
		// 	}
		// }
		// Comparison with the paper <<


		YsBuildBalancedKdTreeOneStep<kdTreeType,VecType,kDim,ATTRIB>(kdTree,np,pos,attrib,indexArray         ,medIdx         ,(thisAxis+1)%kDim,isInUpperHalf,tempIndex0,tempIndex1);
		YsBuildBalancedKdTreeOneStep<kdTreeType,VecType,kDim,ATTRIB>(kdTree,np,pos,attrib,indexArray+medIdx+1,nIndex-medIdx-1,(thisAxis+1)%kDim,isInUpperHalf,tempIndex0,tempIndex1);
	}
}

/* } */
#endif
