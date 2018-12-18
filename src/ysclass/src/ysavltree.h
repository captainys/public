/* ////////////////////////////////////////////////////////////

File Name: ysavltree.h
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

#ifndef YSAVLTREE_IS_INCLUDED
#define YSAVLTREE_IS_INCLUDED
/* { */


#include "ysbinarytree2.h"


template <class KeyClass,class ValueClass,class KeyComparer=YsDefaultComparer<KeyClass> >
class YsAVLTree : public YsBinaryTree2<KeyClass,ValueClass,KeyComparer>
{
public:
	typedef typename YsBinaryTree2<KeyClass,ValueClass,KeyComparer>::NodeHandle NodeHandle;

	NodeHandle Insert(KeyClass key,ValueClass value);
	YSRESULT Delete(NodeHandle ndHd);

protected:
	void AutoRebalance(NodeHandle ndHd);

public:
	YSRESULT VerifyBalance(void) const;
private:
	YSRESULT VerifyBalance(NodeHandle ndHd) const;
};



template <class KeyClass,class ValueClass,class KeyComparer>
typename YsBinaryTree2<KeyClass,ValueClass,KeyComparer>::NodeHandle YsAVLTree<KeyClass,ValueClass,KeyComparer>::Insert(KeyClass key,ValueClass value)
{
	auto ndHd=YsBinaryTree2<KeyClass,ValueClass,KeyComparer>::Insert(key,value);
	AutoRebalance(this->Up(ndHd));
	return ndHd;
}

template <class KeyClass,class ValueClass,class KeyComparer>
YSRESULT YsAVLTree<KeyClass,ValueClass,KeyComparer>::Delete(NodeHandle ndHd)
{
	auto offBalanceNdHd=ndHd;
	if(YSOK==YsBinaryTree2<KeyClass,ValueClass,KeyComparer>::Delete(offBalanceNdHd,ndHd))
	{
		// AutoRebalance checks one node up from currentHd. (For insertion)
		// After deletion, it needs to be checked from the left or right.
		// The question is from which side?

		auto balance=this->Balance(offBalanceNdHd);

		// Balance is L-R.  
		if(balance<=-2)
		{
			// Negative balance means Right heavy.  Means as if a node is inserted on the right.
			AutoRebalance(this->Right(offBalanceNdHd));
		}
		else if(2<=balance)
		{
			// Positive balance means Left heavy.  Means as if a node is inserted on the left.
			AutoRebalance(this->Left(offBalanceNdHd));
		}
		else
		{
			AutoRebalance(offBalanceNdHd);
		}
		return YSOK;
	}
	return YSERR;
}

template <class KeyClass,class ValueClass,class KeyComparer>
void YsAVLTree<KeyClass,ValueClass,KeyComparer>::AutoRebalance(NodeHandle currentHd)
{
	auto current=this->GetNode(currentHd);
	auto upHd=this->Up(currentHd);
	auto up=this->GetNode(upHd);
	while(NULL!=up)  // Is that it?  Is it just all I need to do?
	{
		const int upBalance=up->Balance();

		if(upBalance<-2 || 2<upBalance)
		{
			printf("Tree balance already broken.\n");
			return; // Abort re-balancing to prevent potential infinite loop.
		}

		if(2<=upBalance)
		{
			if(0>current->Balance()) // Left-Right Case
			{
				this->RotateLeft(currentHd); // Force it to be Left-Left Case
			}
			this->RotateRight(upHd); // Left-Left Case
		}
		else if(upBalance<=-2)
		{
			if(0<current->Balance()) // Right-Left Case
			{
				this->RotateRight(currentHd); // Force it to be Right-Right Case
			}
			this->RotateLeft(upHd); // Right-Right Case
		}

		currentHd=upHd; 
		current=up;
		upHd=this->Up(currentHd);
		up=this->GetNode(upHd);

		// Because upHd may have rotated.  currentHd may be going down.  Doesn't it make an infinite loop?
		// Is there way to guarantee this process terminates, even in case of something unexpected, 
		// like numerical disaster when KeyClass is double, happens?
	}
}

template <class KeyClass,class ValueClass,class KeyComparer>
YSRESULT YsAVLTree<KeyClass,ValueClass,KeyComparer>::VerifyBalance(void) const
{
	printf("Verifying Balance...\n");
	if(YSOK==VerifyBalance(YsBinaryTree2<KeyClass,ValueClass,KeyComparer>::RootNode()))
	{
		printf("OK!\n");
		return YSOK;
	}
	printf("Error!\n");
	return YSERR;
}

template <class KeyClass,class ValueClass,class KeyComparer>
YSRESULT YsAVLTree<KeyClass,ValueClass,KeyComparer>::VerifyBalance(NodeHandle ndHd) const
{
	if(ndHd.IsNotNull())
	{
		if(YSOK!=VerifyBalance(this->Left(ndHd)))
		{
			return YSERR;
		}

		int bal=this->Balance(ndHd);
		if(bal<-2 || 2<bal)
		{
			printf("Balance verification failed!\n");
			printf("  Balance=%d\n",bal);
			printf("  Key=%d\n",GetKey(ndHd));
			return YSERR;
		}

		if(YSOK!=VerifyBalance(this->Right(ndHd)))
		{
			return YSERR;
		}
	}
	return YSOK;
}

/* } */
#endif
