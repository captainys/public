/* ////////////////////////////////////////////////////////////

File Name: test.cpp
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

#include <ysclass.h>

#include <stdio.h>
#include <stdlib.h>
#include <time.h>



// Based on http://arxiv.org/pdf/1410.5420v27.pdf 
// Russell A. Brown, "Building a Balanced k-d Tree in O(kn log n) Time", Journal of Computer Graphics Techniques (JCGT), vol. 4, no. 1, 50-68, 2015
static void BuildBalanced3dTreeOneStep(Ys3dTree <int> &kdTree,YsArray <YsPair <YsVec3,int> > &dat,YsStaticArray <YSSIZE_T,3> indexArray[],YSSIZE_T nIndex,int thisAxis);

void BuildBalanced3dTree(Ys3dTree <int> &kdTree,YsArray <YsPair <YsVec3,int> > &dat)
{
	YsArray <YsStaticArray <YSSIZE_T,3> > indexArray(dat.GetN(),NULL);

	{
		YsArray <YSSIZE_T> indexSorter(dat.GetN(),NULL);
		YsArray <double> componentSorter(dat.GetN(),NULL);

		for(int axis=0; axis<3; ++axis)
		{
			for(YSSIZE_T i=0; i<dat.GetN(); ++i)
			{
				indexSorter[i]=i;
				componentSorter[i]=dat[i].a[axis];
			}
			YsQuickSort(componentSorter.GetN(),componentSorter.GetEditableArray(),indexSorter.GetEditableArray());
			for(YSSIZE_T i=0; i<dat.GetN(); ++i)
			{
				indexArray[i][axis]=indexSorter[i];
			}
		}
	}

	BuildBalanced3dTreeOneStep(kdTree,dat,indexArray,indexArray.GetN(),0);
}

static void BuildBalanced3dTreeOneStep(Ys3dTree <int> &kdTree,YsArray <YsPair <YsVec3,int> > &dat,YsStaticArray <YSSIZE_T,3> indexArray[],YSSIZE_T nIndex,int thisAxis)
{
	// Comparison with the paper >>
	if(100>dat.GetN())
	{
		printf("In\n");
		YsConstArrayMask <YsStaticArray <YSSIZE_T,3> > all(nIndex,indexArray);
		for(auto &idx : all)
		{
			printf("%3d %3d %3d\n",idx[0],idx[1],idx[2]);
		}
	}
	// Comparison with the paper <<

	if(0==nIndex)
	{
		return;
	}
	else if(1==nIndex)
	{
		auto idx=indexArray[0][thisAxis];
		kdTree.AddPoint(dat[idx].a,dat[idx].b);
	}
	else
	{
		auto medIdx=nIndex/2;
		auto med=dat[indexArray[medIdx][thisAxis]];
		kdTree.AddPoint(med.a,med.b);

		auto justUsed=indexArray[medIdx][thisAxis];


		YsArray <YSBOOL> isInUpperHalf(dat.GetN(),NULL);
		for(YSSIZE_T i=0; i<nIndex; ++i)
		{
			isInUpperHalf[indexArray[i][thisAxis]]=(i<medIdx ? YSTRUE : YSFALSE);
		}


		YsArray <YSSIZE_T> tempIndex0(nIndex,NULL),tempIndex1(nIndex,NULL);

		for(int otherAxis=0; otherAxis<3; ++otherAxis)
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
				}
				// Small debugging
				if(j0!=nj0 || j1!=nj1)
				{
					printf("j0,j1 does not match nj0,nj1 %d %d %d %d  ThisAxis %d OtherAxis %d\n",j0,j1,nj0,nj1,thisAxis,otherAxis);
				}
			}
		}


		// Comparison with the paper >>
		if(100>dat.GetN())
		{
			printf("out\n");
			YsConstArrayMask <YsStaticArray <YSSIZE_T,3> > all(nIndex,indexArray);
			for(auto &idx : all)
			{
				printf("%3d %3d %3d\n",idx[0],idx[1],idx[2]);
			}
		}
		// Comparison with the paper <<


		isInUpperHalf.ClearDeep();

		BuildBalanced3dTreeOneStep(kdTree,dat,indexArray         ,medIdx         ,(thisAxis+1)%3);
		BuildBalanced3dTreeOneStep(kdTree,dat,indexArray+medIdx+1,nIndex-medIdx-1,(thisAxis+1)%3);
	}
}



void TestAsWrittenInPaper(void)
{
	const YsVec3 sample[]=
	{
		YsVec3(2.1,3.0,3.1),
		YsVec3(5  ,4.1,2.0),
		YsVec3(9.2,6.0,7.0),
		YsVec3(4  ,7.2,9.0),
		YsVec3(8  ,1.1,5.1),
		YsVec3(7  ,2.0,6.0),
		YsVec3(9.0,4.0,1.1),
		YsVec3(8.1,4.2,2.1),
		YsVec3(9.4,7.1,8.1),
		YsVec3(6  ,3.0,1.0),
		YsVec3(3  ,4.3,5.0),
		YsVec3(1  ,6.1,8.0),
		YsVec3(9.1,5.0,3.2),
		YsVec3(2  ,1.0,3.0),
		YsVec3(8.2,7.0,6.1)
	};
	Ys3dTree <int> kdTree;
	YsArray <YsPair <YsVec3,int> > record;
	for(int i=0; i<15; ++i)
	{
		record.Increment();
		record.Last().a=sample[i];
		record.Last().b=i;
	}

	YsBuildBalanced3dTree(kdTree,15,sample);
}


int main(void)
{
	TestAsWrittenInPaper();

	auto t=time(NULL);
	srand((int)t);


	YSRESULT res=YSOK;

	Ys3dTree <int> kdTree;
	YsArray <YsPair <YsVec3,int> > record;
	YsArray <YsVec3> posBuf;
	YsArray <int> idxBuf;

	const int nSample=1000;
	for(int i=0; i<nSample; ++i)
	{
		double x=(double)rand();
		double y=(double)rand();
		double z=(double)rand();

		x=(x/(double)RAND_MAX)*20.0-10.0;
		y=(y/(double)RAND_MAX)*20.0-10.0;
		z=(z/(double)RAND_MAX)*20.0-10.0;

		record.Increment();
		record.Last().a.Set(x,y,z);
		record.Last().b=i;

		posBuf.New().Set(x,y,z);
		idxBuf.Append(i);
	}

	YsBuildBalanced3dTree(kdTree,posBuf,idxBuf);

	for(int i=0; i<1000; ++i)
	{
		double x=(double)rand();
		double y=(double)rand();
		double z=(double)rand();

		x=(x/(double)RAND_MAX)*20.0-10.0;
		y=(y/(double)RAND_MAX)*20.0-10.0;
		z=(z/(double)RAND_MAX)*20.0-10.0;

		const int N=5;

		YsVec3 from(x,y,z);
		auto found=kdTree.FindNNearest(from,N);
		if(N!=found.GetN())
		{
			printf("Error!  Could not find a nearest point!\n");
			res=YSERR;
		}
		else
		{
			auto recordCopy=record;
			YsArray <double> dist(record.GetN(),NULL);

			for(int i=0; i<record.GetN(); ++i)
			{
				dist[i]=(record[i].a-from).GetLength();
			}

			YsQuickSort(dist.GetN(),dist.GetEditableArray(),recordCopy.GetEditableArray());

			printf(">");
			for(int i=0; i<N; ++i)
			{
				printf(".");
				if(recordCopy[i].a!=found[i]->GetPosition() || found[i]->GetAttribute()!=recordCopy[i].b)
				{
					printf("Error!  Correct=%s\n",recordCopy[i].a.Txt());
					printf("        Found=  %s\n",found[i]->GetPosition().Txt());
					printf("        From=   %s\n",from.Txt());
					res=YSERR;
				}
			}
			printf("\n");
		}
	}

	printf("Test done.\n");
	printf("srand=%ld\n",t);
	if(YSOK==res)
	{
		printf("OK!\n");
		return 0;
	}
	else
	{
		printf("Error!\n");
		return 1;
	}
}

