/* ////////////////////////////////////////////////////////////

File Name: ysedgeloop.h
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

#ifndef YSEDGELOOP_IS_INCLUDED
#define YSEDGELOOP_IS_INCLUDED
/* { */

#include "ysdef.h"
#include "ysarray.h"
#include "yshash.h"

template <class YsVtxId,class YsEdgeAtt>
class YsEdgeTemplate
{
public:
	YsVtxId vt[2];
	YsEdgeAtt attr;

	// For internal use.  Don't use in the application
	mutable const YsEdgeTemplate <YsVtxId,YsEdgeAtt> *prev,*next;
};

template <class YsVtxId,class YsEdgeAtt>
int operator==(const YsEdgeTemplate <YsVtxId,YsEdgeAtt> &a,const YsEdgeTemplate <YsVtxId,YsEdgeAtt> &b)
{
	return ((a.vt[0]==b.vt[0] && a.vt[1]==b.vt[1]) || (a.vt[0]==b.vt[1] && a.vt[1]==b.vt[0]));
}

template <class YsVtxId,class YsEdgeAtt>
const YsEdgeTemplate <YsVtxId,YsEdgeAtt> *YsAddEdgeTemplateList
    (const YsEdgeTemplate <YsVtxId,YsEdgeAtt> *top,const YsEdgeTemplate <YsVtxId,YsEdgeAtt> *add)
{
	if(add!=NULL)
	{
		add->next=top;
		if(top!=NULL)
		{
			top->prev=add;
		}
		return add;
	}
	else
	{
		return top;
	}
}

template <class YsVtxId,class YsEdgeAtt>
const YsEdgeTemplate <YsVtxId,YsEdgeAtt> *YsDeleteEdgeTemplateList
    (const YsEdgeTemplate <YsVtxId,YsEdgeAtt> *top,const YsEdgeTemplate <YsVtxId,YsEdgeAtt> *del)
{
	const YsEdgeTemplate <YsVtxId,YsEdgeAtt> *newTop;

	if(del==top)
	{
		newTop=del->next;
	}
	else
	{
		newTop=top;
	}

	if(del->prev!=NULL)
	{
		del->prev->next=del->next;
	}
	if(del->next!=NULL)
	{
		del->next->prev=del->prev;
	}

	return newTop;
}

template <class YsVtxId,class YsEdgeAtt>
YSRESULT YsAssembleLoopFromEdgeList
   (YsArray <const YsEdgeTemplate <YsVtxId,YsEdgeAtt> *> &loopEdge,
    YsArray <YsVtxId> &loopVtx,
    YSSIZE_T nEdge,const YsEdgeTemplate <YsVtxId,YsEdgeAtt> edgeList[])
{
	if(nEdge<=0)       // 2006/02/01
	{                  // 2006/02/01
		return YSERR;  // 2006/02/01
	}                  // 2006/02/01

	int nAttempt;
	YSSIZE_T i,headIdx,tailIdx;
	YsVtxId head,tail;
	YSRESULT res;
	const YsEdgeTemplate <YsVtxId,YsEdgeAtt> *unusedList,*seek;

	res=YSOK;

	unusedList=NULL;
	for(i=0; i<nEdge; i++)
	{
		edgeList[i].prev=NULL;
		edgeList[i].next=NULL;
		unusedList=YsAddEdgeTemplateList <YsVtxId,YsEdgeAtt> (unusedList,&edgeList[i]);
	}

	loopEdge.Set(nEdge,NULL);
	loopVtx.Set(nEdge,NULL);


	// Pick up first one
	loopEdge.SetItem(0,&edgeList[0]);
	loopVtx.SetItem(0,edgeList[0].vt[0]);
	loopVtx.SetItem(1,edgeList[0].vt[1]);

	headIdx=1;
	head=edgeList[0].vt[1];
	tailIdx=nEdge-1;
	tail=edgeList[0].vt[0];
	YsDeleteEdgeTemplateList <YsVtxId,YsEdgeAtt> (unusedList,&edgeList[0]);

	//Vertex
	//         headIdx        tailIdx
	// <-tail  head->                   <-tail
	//   [0]   [1]  .......   [nEdge-1]  ([0])

	//Edge
	//         headIdx         tailIdx
	//     [0]  [1]  .......   [nEdge-1]


	for(nAttempt=1; nAttempt<nEdge; nAttempt++)
	{
		for(seek=unusedList; seek!=NULL; seek=seek->next)
		{
			if(seek->vt[0]==head)
			{
				loopVtx.SetItem(headIdx+1,seek->vt[1]);
				loopEdge.SetItem(headIdx,seek);
				head=seek->vt[1];
				headIdx++;
				unusedList=YsDeleteEdgeTemplateList<YsVtxId,YsEdgeAtt>(unusedList,seek);
				goto NEXTATTEMPT;
			}
			else if(seek->vt[1]==head)
			{
				loopVtx.SetItem(headIdx+1,seek->vt[0]);
				loopEdge.SetItem(headIdx,seek);
				head=seek->vt[0];
				headIdx++;
				unusedList=YsDeleteEdgeTemplateList<YsVtxId,YsEdgeAtt>(unusedList,seek);
				goto NEXTATTEMPT;
			}
			else if(seek->vt[0]==tail)
			{
				loopVtx.SetItem(tailIdx,seek->vt[1]);
				loopEdge.SetItem(tailIdx,seek);
				tail=seek->vt[1];
				tailIdx--;
				unusedList=YsDeleteEdgeTemplateList<YsVtxId,YsEdgeAtt>(unusedList,seek);
				goto NEXTATTEMPT;
			}
			else if(seek->vt[1]==tail)
			{
				loopVtx.SetItem(tailIdx,seek->vt[0]);
				loopEdge.SetItem(tailIdx,seek);
				tail=seek->vt[0];
				tailIdx--;
				unusedList=YsDeleteEdgeTemplateList<YsVtxId,YsEdgeAtt>(unusedList,seek);
				goto NEXTATTEMPT;
			}
		}

		// YsErrOut("YsAssembleLoopFromEdgeList()\n  Cannot find connecting edge.\n");
		return YSERR;

	NEXTATTEMPT:
		;
	}

	if(head!=tail)
	{
		// YsErrOut("YsAssembleLoopFromEdgeList()\n  Head and tail mismatch.\n");
		res=YSERR;
	}


	// Say never connected from the tail
	//   headIdx begin with 1 and incremented (nEdge-1) times.
	//   then headIdx=1+(nEdge-1)
	//        tailIdx=nEdge-1
	// Say never connected to the head
	//   tailIdx begin with nEdge-1 and decremented (nEdge-1) times.
	//   then headIdx=1
	//        tailIdx=(nEdge-1)-(nEdge-1)=0
	// Theoretically, tailIdx must be always headIdx-1
	if(tailIdx!=headIdx-1)
	{
		// YsPrintf("Head index and tail index mismatch %d %d.\n",headIdx,tailIdx);
		res=YSERR;
	}


	if(unusedList!=NULL)
	{
		// YsPrintf("Some edge segments remained unconnected.\n");
		res=YSERR;
	}


	return res;
}


template <class YsVtxId,class YsEdgeAtt,int N,int M>
YSRESULT YsAssembleEdgeSequenceFromEdgeList
   (YsArray <const YsEdgeTemplate <YsVtxId,YsEdgeAtt> *,N> &lseqEdge,
    YsArray <YsVtxId,M> &lseqVtx,
    YSSIZE_T nEdge,const YsEdgeTemplate <YsVtxId,YsEdgeAtt> edgeList[],YsVtxId startVtId)
{
	int i,addPtr;
	YsVtxId currentVtId;
	const YsEdgeTemplate <YsVtxId,YsEdgeAtt> *unusedList,*seek;
	YSRESULT res;

	res=YSOK;

	unusedList=NULL;
	for(i=0; i<nEdge; i++)
	{
		edgeList[i].prev=NULL;
		edgeList[i].next=NULL;
		unusedList=YsAddEdgeTemplateList <YsVtxId,YsEdgeAtt> (unusedList,&edgeList[i]);
	}


	addPtr=0;
	lseqEdge.Set(nEdge,NULL);
	lseqVtx.Set(nEdge+1,NULL);


	lseqVtx.SetItem(addPtr,startVtId);
	currentVtId=startVtId;
	while(addPtr<nEdge)
	{
		for(seek=unusedList; seek!=NULL; seek=seek->next)
		{
			if(seek->vt[0]==currentVtId)
			{
				lseqEdge.SetItem(addPtr,seek);
				lseqVtx.SetItem(addPtr+1,seek->vt[1]);
				addPtr++;
				currentVtId=seek->vt[1];
				unusedList=YsDeleteEdgeTemplateList(unusedList,seek);
				goto NEXTEDGESEGMENT;
			}
			else if(seek->vt[1]==currentVtId)
			{
				lseqEdge.SetItem(addPtr,seek);
				lseqVtx.SetItem(addPtr+1,seek->vt[0]);
				addPtr++;
				currentVtId=seek->vt[0];
				unusedList=YsDeleteEdgeTemplateList(unusedList,seek);
				goto NEXTEDGESEGMENT;
			}
		}

		// Falling down to here means no edge segment found.
		return YSERR;

	NEXTEDGESEGMENT:
		;
	}

	if(unusedList!=NULL)
	{
		YsPrintf("Some edge segments remained unconnected.\n");
		res=YSERR;
	}

	return res;
}



/*! YsVtxId needs to be at least comparable by operator>, operator<, operator>=, operator<=, and operator==.
    Requirement for YsQuickSortAny.
*/
template <class YsVtxId,class YsEdgeAtt,int N,int M>
YSRESULT YsAssembleEdgeSequenceFromEdgeList
   (YsArray <const YsEdgeTemplate <YsVtxId,YsEdgeAtt> *,N> &lseqEdge,
    YsArray <YsVtxId,M> &lseqVtx,
    YSSIZE_T nEdge,const YsEdgeTemplate <YsVtxId,YsEdgeAtt> edgeList[])
{
	lseqEdge.Clear();
	lseqVtx.Clear();

	if(0==nEdge)
	{
		return YSERR;
	}



	YsArray <YsVtxId> allVtx;
	for(int i=0; i<nEdge; i++)
	{
		allVtx.Append(2,edgeList[i].vt);
	}
	YsQuickSortAny <YsVtxId,int> (allVtx.GetN(),allVtx,NULL);



	YSBOOL found=YSFALSE;
	YsVtxId startVtId;
	for(int i=0; i<allVtx.GetN(); i+=2)
	{
		if(allVtx.GetN()-1==i || allVtx[i]!=allVtx[i+1])
		{
			startVtId=allVtx[i];
			found=YSTRUE;
			break;
		}
	}



	if(YSTRUE==found)
	{
		return YsAssembleEdgeSequenceFromEdgeList <YsVtxId,YsEdgeAtt,N,M> (lseqEdge,lseqVtx,nEdge,edgeList,startVtId);
	}
	return YSERR;
}



////////////////////////////////////////////////////////////

template <class YsEdgeAtt>
class YsEdgeElemTemplate : public YsEdgeTemplate <unsigned,YsEdgeAtt>
{
public:
	YsList <YsEdgeElemTemplate <YsEdgeAtt> > *thisInTheList;
};

template <class YsEdgeAtt,const int minItem=1>
class YsEdgeListTemplate : protected YsMultiKeyHash <YsEdgeElemTemplate <YsEdgeAtt> *,2,minItem>
{
public:
// >> To comply with a stupid change made in g++ 3.4
using YsMultiKeyHash <YsEdgeElemTemplate <YsEdgeAtt> *,2,minItem>::FindElement;
// << To comply with a stupid change made in g++ 3.4

public:
	YsEdgeListTemplate(int hashSize);

	const YsEdgeAtt *FindEdge(unsigned vtKey1,unsigned vtKey2) const;
	YsEdgeAtt *FindEdge(unsigned vtKey1,unsigned vtKey2);
	YsEdgeAtt *AddEdge(unsigned vtKey1,unsigned vtKey2);
	YSRESULT DeleteEdge(unsigned vtKey1,unsigned vtKey2);

	YSRESULT SelfDiagnostic(void);

protected:
	YsList <YsEdgeElemTemplate <YsEdgeAtt> > *edgeList;

	YsEdgeElemTemplate <YsEdgeAtt> *CreateEdge(unsigned vtKey1,unsigned vtKey2);
	const YsEdgeElemTemplate <YsEdgeAtt> *LookUpEdge(unsigned vtKey1,unsigned vtKey2) const;
	YsEdgeElemTemplate <YsEdgeAtt> *LookUpEdge(unsigned vtKey1,unsigned vtKey2);
	YSRESULT DeleteEdge(YsEdgeElemTemplate <YsEdgeAtt> *edgeToDel);
};

template <class YsEdgeAtt,const int minItem>
const YsEdgeAtt *YsEdgeListTemplate <YsEdgeAtt,minItem>::FindEdge(unsigned vtKey1,unsigned vtKey2) const
{
	const YsEdgeElemTemplate <YsEdgeAtt> *edgeElem;
	unsigned key[2];

	key[0]=vtKey1;
	key[1]=vtKey2;

	edgeElem=LookUpEdge(vtKey1,vtKey2);
	if(edgeElem!=NULL)
	{
		return &edgeElem->attr;
	}
	else
	{
		return NULL;
	}
}


template <class YsEdgeAtt,const int minItem>
YsEdgeAtt *YsEdgeListTemplate <YsEdgeAtt,minItem>::FindEdge(unsigned vtKey1,unsigned vtKey2)
{
	const YsEdgeListTemplate <YsEdgeAtt,minItem> *constThis;
	constThis=this;
	return (YsEdgeAtt *)(constThis->FindEdge(vtKey1,vtKey2));
}


template <class YsEdgeAtt,const int minItem>
YsEdgeAtt *YsEdgeListTemplate <YsEdgeAtt,minItem>::AddEdge(unsigned vtKey1,unsigned vtKey2)
{
	if(FindEdge(vtKey1,vtKey2)==NULL)
	{
		YsEdgeElemTemplate <YsEdgeAtt> *edgeElem;
		edgeElem=CreateEdge(vtKey1,vtKey2);
		if(edgeElem!=NULL)
		{
			return &edgeElem->attr;
		}
		return NULL;
	}
	else
	{
		YsPrintf("YsEdgeListTemplate::AddEdge()\n");
		YsPrintf("  Attempted to doubly add an edge\n");
		return NULL;
	}
}


template <class YsEdgeAtt,const int minItem>
YSRESULT YsEdgeListTemplate <YsEdgeAtt,minItem>::DeleteEdge(unsigned vtKey1,unsigned vtKey2)
{
	YsEdgeElemTemplate <YsEdgeAtt> *edgeElem;
	edgeElem=LookUpEdge(vtKey1,vtKey2);
	if(edgeElem!=NULL)
	{
		return DeleteEdge(edgeElem);
	}
	return YSERR;
}


template <class YsEdgeAtt,const int minItem>
YSRESULT YsEdgeListTemplate <YsEdgeAtt,minItem>::SelfDiagnostic(void)
{
	YsList <YsEdgeElemTemplate <YsEdgeAtt> > *edgeSeek;
	YSRESULT res;


	YsPrintf("<<YsEdgeListTemplate::SelfDiagnostic()>>\n");
	res=YSOK;

	// From list to search table
	YsPrintf("Checking EdgeList->SearchTable\n");
	for(edgeSeek=edgeList; edgeSeek!=NULL; edgeSeek=edgeSeek->Next())
	{
		YsEdgeAtt *att;
		att=FindEdge(edgeSeek->dat.vt[0],edgeSeek->dat.vt[1]);
		if(att!=NULL)
		{
			if(att!=&edgeSeek->dat.attr)
			{
				YsPrintf("Conflict between hash table and edge list.\n");
				res=YSERR;
			}
		}
		else
		{
			YsPrintf("An edge that is supposed to exist cannot be found.\n");
			res=YSERR;
		}
	}

	return res;
}

template <class YsEdgeAtt,const int minItem>
YsEdgeListTemplate <YsEdgeAtt,minItem>::YsEdgeListTemplate(int hashSize) :
    YsMultiKeyHash <YsEdgeElemTemplate <YsEdgeAtt> *,2,minItem> (hashSize)
{
	edgeList=NULL;
}

template <class YsEdgeAtt,const int minItem>
YsEdgeElemTemplate <YsEdgeAtt> *YsEdgeListTemplate <YsEdgeAtt,minItem>::CreateEdge(unsigned vtKey1,unsigned vtKey2)
{
	YsList <YsEdgeElemTemplate <YsEdgeAtt> > *newElem;
	if(LookUpEdge(vtKey1,vtKey2)==NULL)
	{
		newElem=new YsList <YsEdgeElemTemplate <YsEdgeAtt> >;
		if(newElem!=NULL)
		{
			newElem->dat.thisInTheList=newElem;
			newElem->dat.vt[0]=vtKey1;
			newElem->dat.vt[1]=vtKey2;

			unsigned key[2];
			key[0]=vtKey1;
			key[1]=vtKey2;
			if(AddElement(2,key,&newElem->dat)==YSOK)
			{
				edgeList=edgeList->Append(newElem);
				return &newElem->dat;
			}
			else
			{
				delete newElem;
				YsPrintf("YsEdgeListTemplate::CreateEdge()\n");
				YsPrintf("  Failed to add an element to hash table.\n");
				return NULL;
			}
		}
		return NULL;
	}
	else
	{
		YsPrintf("YsEdgeListTemplate::CreateEdge()\n");
		YsPrintf("  Edge (%d %d) already exists.\n",vtKey1,vtKey2);
		return NULL;
	}
}

template <class YsEdgeAtt,const int minItem>
const YsEdgeElemTemplate <YsEdgeAtt> *YsEdgeListTemplate <YsEdgeAtt,minItem>::LookUpEdge(unsigned vtKey1,unsigned vtKey2) const
{
	const YsArray <YsEdgeElemTemplate <YsEdgeAtt> *,minItem> *elmList;
	unsigned key[2];

	key[0]=vtKey1;
	key[1]=vtKey2;
	elmList=FindElement(2,key);
	if(elmList!=NULL && elmList->GetNumItem()==1)
	{
		return elmList->GetItem(0);
	}
	else if(elmList!=NULL && elmList->GetNumItem()>1)
	{
		YsPrintf("YsEdgeListTemplate::LookUpEdge()\n");
		YsPrintf("  Edge list hash table is broken.\n");
		return NULL;
	}
	else
	{
		return NULL;
	}
}

template <class YsEdgeAtt,const int minItem>
YsEdgeElemTemplate <YsEdgeAtt> *YsEdgeListTemplate <YsEdgeAtt,minItem>::LookUpEdge(unsigned vtKey1,unsigned vtKey2)
{
	const YsEdgeListTemplate <YsEdgeAtt,minItem> *constThis;
	constThis=this;
	return (YsEdgeElemTemplate <YsEdgeAtt> *)constThis->LookUpEdge(vtKey1,vtKey2);
}

template <class YsEdgeAtt,const int minItem>
YSRESULT YsEdgeListTemplate <YsEdgeAtt,minItem>::DeleteEdge(YsEdgeElemTemplate <YsEdgeAtt> *edgeToDel)
{
	if(DeleteElement(2,edgeToDel->vt,edgeToDel)==YSOK)
	{
		YsList <YsEdgeElemTemplate <YsEdgeAtt> > *toDel;
		toDel=edgeToDel->thisInTheList;
		edgeList=toDel->DeleteFromList();
		return YSOK;
	}
	YsPrintf("YsEdgeListTemplate::DeleteEdge()\n");
	YsPrintf("  Failed to delete an element from hash table.\n");
	return YSERR;
}




////////////////////////////////////////////////////////////



/*! This function splits a degenerate polygon, which uses a same vertex more than once, into multiple non-degenerate polygons.
    It returns YSOK if the polygon was split (i.e., the input polygon was degenerate), or YSERR if nothing is done.
    In case the polygon shrunk to nothing, it returns YSOK, and newPlg will be empty. */
template <class HandleClass,const int N,const int M>
YSRESULT YsSplitDegeneratePolygon(YsArray <YsArray <HandleClass,N>,M> &newPlg,YSSIZE_T nVtIn,const HandleClass vtHdIn[])
{
	YSBOOL degenerate=YSFALSE;
	newPlg.CleanUp();

	YsArray <HandleClass,16> vtHd;
	for(int i=0; i<nVtIn; ++i)
	{
		if(vtHdIn[i]!=YsGetCyclic(nVtIn,vtHdIn,i-1))
		{
			vtHd.Append(vtHdIn[i]);
		}
		else
		{
			degenerate=YSTRUE;
		}
	}

	for(int i=0; i<vtHd.GetN(); ++i)
	{
		for(int j=i+1; j<vtHd.GetN(); ++j)
		{
			if(vtHd[i]==vtHd[j])
			{
				degenerate=YSTRUE;
			}
		}
	}

	if(YSTRUE!=degenerate)
	{
		return YSERR;
	}



	while(3<=vtHd.GetN())
	{
		// Trim  A-B-C-B-D  -> A-B-D
		for(;;)
		{
			YSBOOL trimmed=YSFALSE;
			for(int i=0; i<vtHd.GetN(); ++i)
			{
				if(vtHd[i]==vtHd.GetCyclic(i+1))
				{
					vtHd.Delete(i);
					trimmed=YSTRUE;
					break;
				}
				if(vtHd[i]==vtHd.GetCyclic(i+2))
				{
					const int toDel[2]={i,(i+1)%(int)vtHd.GetN()};
					vtHd.Delete(YsGreater(toDel[0],toDel[1]));
					vtHd.Delete(YsSmaller(toDel[0],toDel[1]));
					trimmed=YSTRUE;
					break;
				}
			}
			if(YSTRUE!=trimmed)
			{
				break;
			}
		}
		if(3>vtHd.GetN())
		{
			break;
		}


		YSBOOL degenerate=YSFALSE;
		// Mark multi-use
		YsArray <YSBOOL> multiUse(vtHd.GetN(),NULL);
		for(int i=0; i<vtHd.GetN(); ++i)
		{
			multiUse[i]=YSFALSE;
		}
		for(int i=0; i<vtHd.GetN(); ++i)
		{
			for(int j=i+1; j<vtHd.GetN(); ++j)
			{
				if(vtHd[i]==vtHd[j])
				{
					multiUse[i]=YSTRUE;
					multiUse[j]=YSTRUE;
					degenerate=YSTRUE;
				}
			}
		}
		if(YSTRUE!=degenerate)
		{
			newPlg.Increment();
			newPlg.Last().MoveFrom(vtHd);
			break;
		}


		// Cut out a loop
		YSBOOL chopped=YSFALSE;

		YsArray <HandleClass,4> plg;
		for(int i0=0; i0<vtHd.GetN(); ++i0)
		{
			if(YSTRUE==multiUse[i0])
			{
				for(int i=1; i<vtHd.GetN(); ++i)
				{
					if(vtHd[i0]==vtHd.GetCyclic(i0+i))  // Closed a loop without passing through another multi-use vertex
					{
						for(int k=0; k<i; ++k)
						{
							plg.Append(vtHd.GetCyclic(i0+k));
						}

						//  0 1 2 3 4 5 6 7 8 9
						//  A-B-C-D-E-F-G-D-H-I   => i0=3, i0+i=7 Delete 7,6,5,4

						//  0 1 2 3 4 5 6 7 8 9   ??? makes another loop.
						//  A-B-C-D-?-?-?-D-H-I   => i0=7, (i0+i)%n=3 Delete 9,8,3,2,1,0

						const int i1=(i0+i)%vtHd.GetN();
						if(i0<i1)
						{
							vtHd.Delete(i0+1,i1-i0);
						}
						else
						{
							vtHd.Resize(i0);  // Deletes i0+1, i0+2, ... last
							vtHd.Delete(0,i1);
						}

						break;
					}
					else if(YSTRUE==multiUse.GetCyclic(i0+i))
					{
						break;
					}
				}
			}
			if(0<plg.GetN())
			{
				newPlg.Increment();
				newPlg.Last().MoveFrom(plg);
				chopped=YSTRUE;
				break;
			}
		}

		if(YSTRUE!=chopped)
		{
			return YSERR;
		}
	}

	return YSOK;
}

template <class HandleClass,const int N,const int M,class ArrayClass>
YSRESULT YsSplitDegeneratePolygon(YsArray <YsArray <HandleClass,N>,M> &newPlg,const ArrayClass &vtHdIn)
{
	return YsSplitDegeneratePolygon<HandleClass,N,M>(newPlg,vtHdIn.GetN(),vtHdIn);
}

/* } */
#endif
