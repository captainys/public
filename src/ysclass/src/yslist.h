/* ////////////////////////////////////////////////////////////

File Name: yslist.h
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

// NULL Check for all editing functions

#ifndef YSLIST_IS_INCLUDED
#define YSLIST_IS_INCLUDED
/* { */

#include "ysprintf.h"


extern int YsNOldStyleListItem;


// What must be updated if something is changed?
//   top,nObj,cache


template <class T> class YsList
{
public:
	YsList();
	~YsList();

	inline YsList <T> *Next(void);
	inline YsList <T> *Prev(void);
	inline YsList <T> *SeekTop(void);
	inline YsList <T> *SeekEnd(void);
	inline YsList <T> *Seek(YSSIZE_T n);
	inline const YsList <T> *Next(void) const;
	inline const YsList <T> *Prev(void) const;
	inline const YsList <T> *SeekTop(void) const;
	inline const YsList <T> *SeekEnd(void) const;
	inline const YsList <T> *Seek(YSSIZE_T n) const;
	YSBOOL IsTop(void) const;
	YSBOOL IsEnd(void) const;

	int GetNumObject(void) const;
		// {a0,a1,a2,...,aN)}
		//   return N+1;

	int GetPositionInList(void) const;
		// {a0,a1,a2,...,(this=an),...,(aN)}
		//   return n;
		// If called by NULL pointer,
		//   return -1;

	YsList <T> *Append(YsList <T> *follow);
		// {(this==a0),(a1),...,(aN)}
		//    +
		// {(follow==b0),(b1),...,(bN)}
		//    to
		// return {(a0),(a1),...,(aN),(b0),(b1),...,(bN)}

	YsList <T> *DetachFromList(void);
		// {(a0),(a1),(a2),...,(a n-1),(an=this),(a n+1),...,(aN)}
		//   to
		// return {(a0),(a1),(a2),...,(a n-1),(a n+1),...,(aN)}

	YsList <T> *DeleteFromList(void);
		// Detach from list. And the object is deleted.

	YsList <T> *Delete(int n);
		// {(this=a0),(a1),(a2),...,(a n-1),(an),(a n+1),...,(aN)}
		//    to
		// return {(a0),(a1),(a2),...,(a n-1),(a n+1),...,(aN)}

	YSRESULT DeleteList(void);
		//  Delete entire node from memory.

	YsList <T> *Insert(int n,YsList <T> *toInsert);
		// {(this=a0),(a1),...,(a n-1),(an),...,(aN)}
		// {(toInsert=b0),(b1),(b2),....,(bN)}
		//    to
		// return {(a0),(a1),...,(a n-1),(b0),...,(bN),(an),...,(aN)}
		//
		// if this==NULL
		//   return toInsert;

	YsList <T> *InsertOnTheLeft(YsList <T> *toInsert);
		// {(a0),(a1),...,(a n-1),(an=this),(a n+1),...,(aN)}
		//   to
		// {(a0),(a1),...,(a n-1),{toInsert},(an=this),(a n+1),...,(aN)}

	YsList <T> *InsertOnTheRight(YsList <T> *toInsert);
		// {(a0),(a1),...,(a n-1),(an=this),(a n+1),...,(aN)}
		//   to
		// {(a0),(a1),...,(a n-1),(an=this),{toInsert},(a n+1),...,(aN)}

	YsList <T> *Split(YsList <T> *splitPoint);
		// {(a0),(a1),...,(a n-1),(an=splitPoint),...,(aN)}
		//   to
		// return {(a0),(a1),...,(a n-1)}
		//
		// {(an=splitPoint),...,(aN)} can be used as an independent list.

	YsList <T> *Duplicate(void) const;
		// return (duplicated list)

	void Test(void) const;

	void Encache(void) const;
	void Decache(void) const;

	T dat;

protected:
	YsList <T> *prv,*nxt,*top,*btm;

	// nObj and cache is available only for top of the list
	mutable int nCache;
	mutable YsList <T> **cache;
	mutable int positionInList;

	void SetNext(YsList <T> *neoNxt);
	void SetPrev(YsList <T> *neoPrv);

	void InitializeForRecycle(void);
};

template <class T> YsList <T>::YsList()
{
	prv=NULL;
	nxt=NULL;
	top=this;
	btm=this;

	nCache=0;
	cache=NULL;

	positionInList=-1;

	YsNOldStyleListItem++;
}

template <class T> YsList <T>::~YsList()
{
	if(cache!=NULL)
	{
		delete [] cache;
	}

	YsNOldStyleListItem--;
}

template <class T> inline YsList <T> *YsList <T>::Next(void)
{
	const YsList <T> *cThis;
	cThis=this;
	return (YsList <T> *)(cThis->Next());
}

template <class T> inline YsList <T> *YsList <T>::Prev(void)
{
	const YsList <T> *cThis;
	cThis=this;
	return (YsList <T> *)(cThis->Prev());
}

template <class T> inline YsList <T> *YsList <T>::SeekTop(void)
{
	const YsList <T> *cThis;
	cThis=this;
	return (YsList <T> *)(cThis->SeekTop());
}

template <class T> inline YsList <T> *YsList <T>::SeekEnd(void)
{
	const YsList <T> *cThis;
	cThis=this;
	return (YsList <T> *)(cThis->SeekEnd());
}

template <class T> inline YsList <T> *YsList <T>::Seek(YSSIZE_T n)
{
	const YsList <T> *cThis;
	cThis=this;
	return (YsList <T> *)(cThis->Seek(n));
}

template <class T> inline const YsList <T> *YsList <T>::Next(void) const
{
	const auto thisPtr=this;
	if(thisPtr!=NULL)
	{
		return nxt;
	}
	else
	{
		return NULL;
	}
}

template <class T> inline const YsList <T> *YsList <T>::Prev(void) const
{
	const auto thisPtr=this;
	if(thisPtr!=NULL)
	{
		return prv;
	}
	else
	{
		return NULL;
	}
}

template <class T> inline const YsList <T> * YsList <T>::SeekTop(void) const
{
	const auto thisPtr=this;
	if(thisPtr==NULL)
	{
		return NULL;
	}
	else
	{
		return top;
	}

}

template <class T> inline const YsList <T> * YsList <T>::SeekEnd(void) const
{
	const auto thisPtr=this;
	if(thisPtr==NULL)
	{
		return NULL;
	}
	else
	{
		return top->btm;
	}
}

template <class T> inline const YsList <T> * YsList <T>::Seek(YSSIZE_T n) const
{
	const auto thisPtr=this;
	if(thisPtr==NULL || n<0)
	{
		return NULL;
	}
	else if(IsTop()==YSTRUE)
	{
		if(0<=n && n<nCache && cache!=NULL)
		{
			return cache[n];
		}
		else
		{
			const YsList <T> *seeker;
			YSSIZE_T i;
			seeker=this;
			for(i=0; i<n && seeker!=NULL; i++)
			{
				seeker=seeker->Next();
			}
			return seeker;
		}
	}
	else
	{
		YsErrOut("YsList::Seek()\n  WARNING:Seek() is called for non-top member.\n");
		return top->Seek(n);
	}
}

template <class T> YSBOOL YsList <T>::IsTop(void) const
{
	const auto thisPtr=this;
	// Suppressing clang warning.  I know.  I know.  This code is very old, but some of my old source codes
	// are still using this class.  Please don't whine.
	if(thisPtr!=NULL && Prev()==NULL)
	{
		return YSTRUE;
	}
	else
	{
		return YSFALSE;
	}
}

template <class T> YSBOOL YsList <T>::IsEnd(void) const
{
	const auto thisPtr=this;
	if(thisPtr!=NULL && Next()==NULL)
	{
		return YSTRUE;
	}
	else
	{
		return YSFALSE;
	}
}

template <class T> int YsList<T>::GetNumObject(void) const
{
	const auto thisPtr=this;
	if(thisPtr!=NULL)
	{
		if(IsTop()!=YSTRUE)
		{
			YsErrOut("YsList<T>::GetNumObject()\n  WARNING:Used for non-top node.\n");
		}

		const YsList <T> *top;
		top=SeekTop();
		if(top->nCache>0 && cache!=NULL)
		{
			return nCache;
		}
		else
		{
			int i;
			const YsList <T> *seeker;
			i=0;
			for(seeker=this; seeker!=NULL; seeker=seeker->Next())
			{
				i++;
			}
			return i;
		}
	}
	return 0;
}

template <class T> int YsList <T>::GetPositionInList(void) const
{
	// {(this=a0),(a1),(a2),...,(ptr==an),...,(aN)}
	// then return n;

	if(positionInList>=0)
	{
		return positionInList;
	}

	const auto thisPtr=this;
	if(thisPtr!=NULL)
	{
		int i;
		const YsList <T> *seeker;

		i=0;
		for(seeker=SeekTop(); seeker!=NULL; seeker=seeker->Next())
		{
			if(seeker==this)
			{
				return i;
			}
			i++;
		}
		YsErrOut("YsList <T>::GetPositionInList()\n  WARNING:Inconsistent top-node combination.\n");
	}
	return -1;
}

template <class T> YsList <T> * YsList <T>::Append(YsList <T> *follow)
{
	const auto thisPtr=this;
	// Suppressing clang warning.  I know.  I know.  This code is very old, but some of my old source codes
	// are still using this class.  SHUT UP!!!!!   ABCXYZ!@#$%!#$!!!!!!!!


	// {(this==a0),(a1),...,(aN)}
	//    +
	// {(follow==b0),(b1),...,(bN)}
	//    to
	// return {(a0),(a1),...,(aN),(b0),(b1),...,(bN)}

	if(follow==NULL && thisPtr==NULL)
	{
		return NULL;
	}
	else if(follow==NULL) // && this!=NULL
	{
		if(this->IsTop()==YSTRUE)
		{
			return this;
		}
		else
		{
			YsErrOut("YsList <T>::Append()\n  WARNING:Used for non-top node.\n");
			return SeekTop();
		}
	}
	else if(thisPtr==NULL) // && follow!=NULL
	{
		if(follow->IsTop()==YSTRUE)
		{
			return follow;
		}
		else
		{
			YsErrOut("YsList <T>::Append()\n  WARNING:Target node is not a top node.\n");
			return follow->SeekTop();
		}
	}
	else
	{
		YsList <T> *l1,*l2;
		l1=this;
		l2=follow;
		if(this->IsTop()!=YSTRUE)
		{
			YsErrOut("YsList <T>::Append()\n  WARNING:Used by a non-top node.\n");
			l1=this->SeekTop();
		}
		if(follow->IsTop()!=YSTRUE)
		{
			YsErrOut("YsList <T>::Append()\n  WARNING:Target node is not a top node.\n");
			l2=follow->SeekTop();
		}

		l1->Decache();
		l2->Decache();

		if(l1==l2)
		{
			YsErrOut("YsList <T>::Append()\n  Two lists are identical\n");
			return l1;
		}

		// l1End<->l2Top
		YsList <T> *l1End,*l2Top;
		l1End=l1->SeekEnd();
		l2Top=l2;
		l1End->SetNext(l2Top);
		l2Top->SetPrev(l1End);

		// Reset "top" of all l2 members
		YsList <T> *ptr;
		for(ptr=l2; ptr!=NULL; ptr=ptr->Next())
		{
			ptr->top=l1;
		}

		l1->btm=l2->btm;  // btm is kept only by the top node

		return l1;
	}
}

template <class T> YsList <T> * YsList <T>::DeleteFromList(void)
{
	const auto thisPtr=this;
	if(thisPtr!=NULL)
	{
		YsList <T> *ret;
		ret=DetachFromList();
		delete this;
		return ret;
	}
	return NULL;
}

template <class T> YsList <T> * YsList <T>::DetachFromList(void)
{
	// {(a0),(a1),(a2),...,(a n-1),(an=this),(a n+1),...,(aN)}
	//   to
	// return {(a0),(a1),(a2),...,(a n-1),(a n+1),...,(aN)}
	const auto thisPtr=this;
	if(thisPtr==NULL)
	{
		return NULL;
	}
	else
	{
		if(IsTop()==YSTRUE)
		{
			YsList <T> *neoTop,*seeker;
			neoTop=Next();

			neoTop->SetPrev(NULL);
			if(neoTop!=NULL)
			{
				neoTop->btm=btm;   // btm is kept only in the top node
			}
			for(seeker=neoTop; seeker!=NULL; seeker=seeker->Next())
			{
				seeker->top=neoTop;
			}

			InitializeForRecycle();

			neoTop->Decache();

			return neoTop;
		}
		else if(IsEnd()==YSTRUE)
		{
			YsList <T> *top;
			top=SeekTop();
			if(top->cache!=NULL && top->nCache>0)
			{
				top->nCache--;
			}

			YsList <T> *neoBtm;
			neoBtm=Prev();
			neoBtm->SetNext(NULL);

			top->btm=neoBtm;  // btm is kept only on the top node

			InitializeForRecycle();

			return top;
		}
		else
		{
			YsList <T> *n1,*n2;
			n1=Prev();
			n2=Next();
			n1->SetNext(n2);
			n2->SetPrev(n1);

			YsList <T> *top;
			top=SeekTop();

			if(top->cache!=NULL && top->nCache>0 && positionInList>=0)
			{
				int i;
				top->nCache--;  // Do it before the for-loop below.
				for(i=positionInList; i<top->nCache; i++)
				{
					top->cache[i]=top->cache[i+1];
					top->cache[i]->positionInList=i;
				}
			}

			InitializeForRecycle();

			return top;
		}
	}
}


template <class T> YsList <T> * YsList <T>::Delete(int n)
{
	// {(this=a0),(a1),(a2),...,(a n-1),(an),(a n+1),...,(aN)}
	//    to
	// return {(a0),(a1),(a2),...,(a n-1),(a n+1),...,(aN)}

	const auto thisPtr=this;
	if(thisPtr==NULL)
	{
		return NULL;
	}
	else if(IsTop()!=YSTRUE)
	{
		YsErrOut("YsList <T>::Delete\n  Used by a non-top node.\n");
		return SeekTop()->Delete(n);
	}
	else
	{
		YsList <T> *target;

		Decache();

		target=Seek(n);
		if(target!=NULL)
		{
			return target->DeleteFromList();
		}
		else
		{
			return this;
		}
	}
}

template <class T> YSRESULT YsList <T>::DeleteList(void)
{
	YsList <T> *ptr,*nxt;
	for(ptr=SeekTop(); ptr!=NULL; ptr=nxt)
	{
		nxt=ptr->Next();
		delete ptr;
	}
	return YSOK;
}

template <class T> YsList <T> * YsList <T>::Insert(int n,YsList <T> *toIns)
{
	// {(this=a0),(a1),...,(a n-1),(an),...,(aN)}
	// {(toInsert=b0),(b1),(b2),....,(bN)}
	//    to
	// return {(a0),(a1),...,(a n-1),(b0),...,(bN),(an),...,(aN)}

	const auto thisPtr=this;
	if(thisPtr==NULL && toIns==NULL)
	{
		return toIns;
	}
	else if(toIns==NULL)
	{
		if(this->IsTop()!=YSTRUE)
		{
			YsErrOut("YsList <T>::Insert()\n  WARNING:Used by a non-top member.\n");
		}
		return this->SeekTop();
	}
	else if(thisPtr==NULL)
	{
		if(toIns->IsTop()!=YSTRUE)
		{
			YsErrOut("YsList <T>::Insert()\n  WARNING:Target is not a top member.\n");
		}
		return toIns->SeekTop();
	}
	else
	{
		if(IsTop()!=YSTRUE)
		{
			YsErrOut("YsList <T>::Insert\n  WARNING:Used by non-top member.\n");
			return SeekTop()->Insert(n,toIns);
		}
		if(toIns->IsTop()!=YSTRUE)
		{
			YsErrOut("YsList <T>::Insert\n  WARNING:Target is not a top member.\n");
			return Insert(n,toIns->SeekTop());
		}

		if(n<=0)
		{
			return toIns->Append(this);
		}
		else if(GetNumObject()<=n)
		{
			return this->Append(toIns);
		}
		else
		{
			YsList <T> *seeker,*n1,*n2,*i1,*i2;

			n1=Seek(n-1);
			i1=toIns->SeekTop();
			i2=toIns->SeekEnd();
			n2=Seek(n);

			n1->SetNext(i1);
			i1->SetPrev(n1);
			i2->SetNext(n2);
			n2->SetPrev(i2);

			for(seeker=i1; seeker!=n2; seeker=seeker->Next())
			{
				seeker->top=SeekTop();
				seeker->btm=SeekEnd();
			}

			toIns->Decache();
			this->Decache();
			return this;
		}
	}
}

template <class T>
YsList <T> *YsList <T>::InsertOnTheLeft(YsList <T> *toInsert)
{
	const auto thisPtr=this;
	if(thisPtr==NULL)
	{
		return toInsert;
	}
	else
	{
		int n;
		YsList <T> *top;
		top=SeekTop();
		n=GetPositionInList();
		return top->Insert(n,toInsert);
	}
}

template <class T>
YsList <T> *YsList <T>::InsertOnTheRight(YsList <T> *toInsert)
{
	const auto thisPtr=this;
	if(thisPtr==NULL)
	{
		return toInsert;
	}
	else
	{
		int n;
		YsList <T> *top;
		top=SeekTop();
		n=GetPositionInList();
		return top->Insert(n+1,toInsert);
	}
}

template <class T> YsList <T> *YsList <T>::Split(YsList <T> *splitPoint)
{
	const auto thisPtr=this;
	if(thisPtr==NULL)
	{
		return NULL;
	}
	else
	{
		if(IsTop()!=YSTRUE)
		{
			YsErrOut("YsList <T>::Split()\n  WARNING:Used by non-top member.\n");
			return SeekTop()->Split(splitPoint);
		}

		if(splitPoint==this)
		{
			// {a0,a1,...,aN}
			//   into
			// {} {a0,a1,...,aN}
			return NULL;
		}
		else if(splitPoint==NULL)
		{
			// {a0,a1,...,aN}
			//  into
			// {a0,a1,...,aN} {}
			return this;
		}
		else
		{
			int n;
			n=splitPoint->GetPositionInList();
			if(n<0)
			{
				YsErrOut("YsList <T>::Split()\n  Split point is not a member of the list.\n");
				return this;
			}

			YsList <T> *l1End,*l2Top,*seeker;
			l1End=splitPoint->Prev();
			l2Top=splitPoint;

			l1End->SetNext(NULL);
			l2Top->SetPrev(NULL);

			this->btm=l1End;   // btm is kept only in the top node

			for(seeker=l2Top; seeker!=NULL; seeker=seeker->Next())
			{
				seeker->top=l2Top;
			}

			Decache();

			return this;
		}
	}
}

template <class T> void YsList <T>::Test(void) const
{
	int i,n;
	const YsList <T> *seeker;

	const auto thisPtr=this;
	if(thisPtr==NULL)
	{
		return;
	}

	if(IsTop()!=YSTRUE)
	{
		YsErrOut("YsList <T>::Test\n  Used for non-top member.\n");
		SeekTop()->Test();
		return;
	}

	YsPrintf("--Self Diagnostics--\n");
	YsPrintf("YsList::Test()\n");

	if(nCache>0 && cache!=NULL)
	{
		YsPrintf(">>Encached\n");
	}
	else
	{
		YsPrintf(">>Not cached\n");
	}


	YsPrintf("--Check number of objects--\n");
	n=GetNumObject();
	i=0;
	for(seeker=this; seeker!=NULL; seeker=seeker->Next())
	{
		i++;
	}
	if(n==i)
	{
		YsPrintf(">>GetNumObj()  OK.\n");
	}
	else
	{
		YsPrintf(">>Number of object no match\n");
		YsPrintf("  GetNumObj()    :%d\n",n);
		YsPrintf("  Actual         :%d\n",i);
	}


	YsPrintf("--Check Node Pointer Matching--\n");
	i=0;
	for(seeker=this; seeker!=NULL; seeker=seeker->Next())
	{
		if(seeker==Seek(i))
		{
		}
		else
		{
			YsPrintf("[%4d] ERROR.\n",i);
		}
		i++;
	}

	YsPrintf("--Check Top/End Matching--\n");
	for(seeker=this; seeker!=NULL; seeker=seeker->Next())
	{
		if(seeker->SeekTop()==SeekTop() && seeker->SeekTop()==top &&
		   seeker->top==top)
		{
		}
		else
		{
			YsPrintf("[%4d] ERROR(top).\n",i);
		}

		if(seeker->SeekEnd()==SeekEnd() && seeker->SeekEnd()==btm)  // Non-top nodes no longer remember btm
		{
		}
		else
		{
			YsPrintf("[%4d] ERROR(top).\n",i);
		}
		i++;
	}

	YsPrintf("--Cache Check--\n");
	i=0;
	for(seeker=this; seeker!=NULL; seeker=seeker->Next())
	{
		if(seeker->IsTop()!=YSTRUE &&
		   (seeker->cache!=NULL || seeker->nCache>0))
		{
			YsPrintf("[%4d] ERROR.\n",i);
		}
		i++;
	}
}

template <class T> void YsList <T>::Encache(void) const
{
	const auto thisPtr=this;
	if(thisPtr!=NULL)
	{
		Decache();

		int i,n;
		const YsList <T> *seeker;
		n=0;
		for(seeker=this; seeker!=NULL; seeker=seeker->Next())
		{
			n++;
		}

		cache=new YsList <T> *[n];
		if(cache!=NULL)
		{
			seeker=this;
			for(i=0; i<n; i++)
			{
				seeker->positionInList=i;
				cache[i]=(YsList *)seeker;
				seeker=seeker->Next();
			}
			nCache=n;
		}
	}
}

template <class T> void YsList <T>::Decache(void) const
{
	const auto thisPtr=this;
	if(thisPtr!=NULL)
	{
		if(IsTop()!=YSTRUE)
		{
			YsErrOut("YsList::Decache()\n  Decache() is used to non-top member.\n");
		}

		if(cache!=NULL)
		{
			const YsList <T> *seeker;
			for(seeker=this; seeker!=NULL; seeker=seeker->Next())
			{
				seeker->positionInList=-1;
			}

			delete [] cache;
		}
		nCache=0;
		cache=NULL;
	}
}

template <class T> inline void YsList <T>::SetNext(YsList <T> *neoNext)
{
	const auto thisPtr=this;
	// Suppressing clang warning.  I know.  I know.  This code is very old, but some of my old source codes
	// are still using this class.  SHUT UP!!!!!   DO NOT NEVER EVER WHINE FOR THIS LEGACY CODD!!!!!!!!!
	if(thisPtr!=NULL)
	{
		nxt=neoNext;
	}
}

template <class T> inline void YsList <T>::SetPrev(YsList <T> *neoPrev)
{
	const auto thisPtr=this;
	// Suppressing clang warning.  I know.  I know.  This code is very old, but some of my old source codes
	// are still using this class.  PLEEEEASE!!!!  Don't whine.
	if(thisPtr!=NULL)
	{
		prv=neoPrev;
	}
}

template <class T> inline void YsList <T>::InitializeForRecycle(void)
{
	prv=NULL;
	nxt=NULL;
	top=this;
	btm=this;

	nCache=0;
	if(cache!=NULL)
	{
		delete [] cache;
	}
	cache=NULL;

	positionInList=-1;
}

template <class T> YsList<T> *YsList <T>::Duplicate(void) const
{
	const YsList <T> *ptr;
	YsList <T> *neo,*dup;
	dup=NULL;
	for(ptr=this; ptr!=NULL; ptr=ptr->Next())
	{
		neo=new YsList <T>;
		if(neo!=NULL)
		{
			neo->dat=ptr->dat;
			dup=dup->Append(neo);
		}
		else
		{
			YsErrOut("YsList::Duplicate\n  LowMemoryWarning\n");
			break;
		}
	}
	return dup;
}

/* } */
#endif
