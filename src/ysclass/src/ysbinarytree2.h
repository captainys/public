/* ////////////////////////////////////////////////////////////

File Name: ysbinarytree2.h
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

#ifndef YSBINARYTREE2_IS_INCLUDED
#define YSBINARYTREE2_IS_INCLUDED
/* { */

#include <stdlib.h>
#include <stdio.h>
#include <cstddef>

#include "ysdef.h"
#include "ysarray.h"
#include "yscomparer.h"
#include "ysbdhandleiterator.h"

// Should I go with SetNull or Nullify or both?
// What about IsNull and IsNotNull?  


// KeyComparer class must have three static member functions:
//	static bool Equal(const KeyClass &a,const KeyClass &b)
//	static bool AIsSmallerThanB(const KeyClass &a,const KeyClass &b)
//	static bool AIsGreaterThanB(const KeyClass &a,const KeyClass &b)
// YsDefaultComparer assumes operator=, operator>, and oeprator< are available for class KeyClass.


template <class KeyClass,class ValueClass,class KeyComparer=YsDefaultComparer<KeyClass> >
class YsBinaryTree2
{
typedef int indexType;

public:
	class NodeHandle
	{
	friend class YsBinaryTree2;
	private:
		indexType nodeIndex;

	public:
		inline bool operator==(NodeHandle hd) const;
		inline bool operator!=(NodeHandle hd) const;
		inline bool operator==(std::nullptr_t) const;
		inline bool operator!=(std::nullptr_t) const;
		inline void Nullify(void);
		inline bool IsNull(void) const;
		inline bool IsNotNull(void) const;

		inline indexType RawIndex(void) const  // For debugging purpose.
		{
			return nodeIndex;
		}
	};

private:
	class Node
	{
	public:
		KeyClass key;
		ValueClass value;
		NodeHandle left,right,up;
		int heightL,heightR;

		inline void Initialize(void);
		inline int Balance(void) const;
	};

	YSSIZE_T nNode;
	YsArray <Node> nodeStore;
	YsArray <YSSIZE_T> availableIndex;
	NodeHandle rootHd;


public:
	typedef YsBidirectionalHandleIterator <YsBinaryTree2<KeyClass,ValueClass,KeyComparer>,NodeHandle,ValueClass> iterator;
	typedef YsBidirectionalHandleIterator <const YsBinaryTree2<KeyClass,ValueClass,KeyComparer>,NodeHandle,const ValueClass> const_iterator;
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

	typedef YsBidirectionalHandleEnumerator<YsBinaryTree2<KeyClass,ValueClass,KeyComparer>,NodeHandle> HandleEnumerator;
	/*! This class enables range-based for using an index.  It can be written as:
	    YsArray <T> array;
	    for(auto idx : array.AllIndex())
	    {
	        // Do something for array[idx]
	    }
	    Useful when something needs to be done before and after an array item. */
	HandleEnumerator AllHandle(void) const
	{
		return HandleEnumerator(this);
	}


public:
	YsBinaryTree2();
	~YsBinaryTree2();
	void CleanUp(void);

private:
	NodeHandle CreateNode(void);
	void MakeNodeAvailable(NodeHandle ndHd);
	void UpdateUp(NodeHandle upHd,YSBOOL isLeftOfUp,NodeHandle newLeaf);
	/*
	UpdateHeightAutoStop starts re-calculating heightL and heightR of ndHd,
	and then update heights of higher nodes, until the height of a node in the middles has
	no change in the total height.
	If the heightL and heightRs of all the higher nodes are consistent with the child node's total heights,
	use this function.
	*/
	void UpdateHeightAutoStop(NodeHandle ndHd);
	void UpdateHeightSingle(Node *node);
	NodeHandle LeftMostOf(int &depth,NodeHandle ndHd) const;
	NodeHandle RightMostOf(int &depth,NodeHandle ndHd) const;

protected:
	inline Node *GetNode(NodeHandle ndHd);
	inline const Node *GetNode(NodeHandle ndHd) const;
	inline int Balance(NodeHandle ndHd) const;

public:
	inline YSSIZE_T GetN(void) const;
	inline KeyClass GetKey(NodeHandle ndHd) const;
	inline ValueClass &GetValue(NodeHandle ndHd);
	inline const ValueClass &GetValue(NodeHandle ndHd) const;
	inline ValueClass &operator[](NodeHandle ndHd);
	inline const ValueClass &operator[](NodeHandle ndHd) const;
	NodeHandle FindNode(KeyClass key) const;
	YSBOOL IsKeyIncluded(KeyClass key) const;

	inline NodeHandle Left(NodeHandle ndHd) const;
	inline NodeHandle Up(NodeHandle ndHd) const;
	inline NodeHandle Right(NodeHandle ndHd) const;
	inline YSBOOL IsLeftOfUp(NodeHandle ndHd) const;

	inline NodeHandle RootNode(void) const;
	NodeHandle First(void) const;
	NodeHandle FindNext(NodeHandle ndHd) const;

	NodeHandle Last(void) const;
	NodeHandle FindPrev(NodeHandle ndHd) const;

	NodeHandle Insert(KeyClass key,const ValueClass &value);
	YSRESULT Delete(NodeHandle ndHd);
protected:
	YSRESULT Delete(NodeHandle &offBalanceNdHd,NodeHandle ndHd);
private:
	/*
	SimpleDetach detaches the node (and the lower structure) from the tree structure, but it is not actually deleted until 
	it is given to MakeAvailable.
	That means, the node can be re-attached to somewhere else until the node is made available for re-cycling.
	*/
	YSRESULT SimpleDetach(NodeHandle &offBalandeNode,NodeHandle ndHd);  // Succeed only if ndHd is a leaf, or has only one child.

public:
	/*! Node ndHd will become left of Right(ndHd) */
	YSRESULT RotateLeft(const NodeHandle ndHd);

	/*! Node ndHd will become right of Left(ndHd). */
	YSRESULT RotateRight(const NodeHandle ndHd);

	YSRESULT VerifyHeight(void) const;
private:
	YSRESULT VerifyHeight(NodeHandle ndHd) const;

public:
	YSRESULT VerifyNodeCount(void) const;
private:
	YSSIZE_T CountNode(NodeHandle ndHd) const;

public:
	YSRESULT VerifyOrder(void) const;

public:
	void MoveToNext(NodeHandle &ndHd) const;
	void MoveToPrev(NodeHandle &ndHd) const;
	static inline NodeHandle Null(void);
	inline ValueClass &Value(NodeHandle ndHd);
	inline const ValueClass &Value(NodeHandle ndHd) const;

public:
	/*! For better interoperability with STL. */
	inline YSSIZE_T size(void) const
	{
		return GetN();
	}
	/*! For better interoperability with STL. */
	bool empty(void) const
	{
		return 0==GetN();
	}
	/*! For better interoperability with STL. */
	void clear(void)
	{
		CleanUp();
	}
};

////////////////////////////////////////////////////////////

template <class KeyClass,class ValueClass,class KeyComparer>
typename YsBinaryTree2<KeyClass,ValueClass,KeyComparer>::iterator begin(YsBinaryTree2<KeyClass,ValueClass,KeyComparer> &tree)
{
	return tree.begin();
}
template <class KeyClass,class ValueClass,class KeyComparer>
typename YsBinaryTree2<KeyClass,ValueClass,KeyComparer>::iterator end(YsBinaryTree2<KeyClass,ValueClass,KeyComparer> &tree)
{
	return tree.end();
}
template <class KeyClass,class ValueClass,class KeyComparer>
typename YsBinaryTree2<KeyClass,ValueClass,KeyComparer>::iterator rbegin(YsBinaryTree2<KeyClass,ValueClass,KeyComparer> &tree)
{
	return tree.rbegin();
}
template <class KeyClass,class ValueClass,class KeyComparer>
typename YsBinaryTree2<KeyClass,ValueClass,KeyComparer>::iterator rend(YsBinaryTree2<KeyClass,ValueClass,KeyComparer> &tree)
{
	return tree.rend();
}

template <class KeyClass,class ValueClass,class KeyComparer>
typename YsBinaryTree2<KeyClass,ValueClass,KeyComparer>::const_iterator begin(const YsBinaryTree2<KeyClass,ValueClass,KeyComparer> &tree)
{
	return tree.begin();
}
template <class KeyClass,class ValueClass,class KeyComparer>
typename YsBinaryTree2<KeyClass,ValueClass,KeyComparer>::const_iterator end(const YsBinaryTree2<KeyClass,ValueClass,KeyComparer> &tree)
{
	return tree.end();
}
template <class KeyClass,class ValueClass,class KeyComparer>
typename YsBinaryTree2<KeyClass,ValueClass,KeyComparer>::const_iterator rbegin(const YsBinaryTree2<KeyClass,ValueClass,KeyComparer> &tree)
{
	return tree.rbegin();
}
template <class KeyClass,class ValueClass,class KeyComparer>
typename YsBinaryTree2<KeyClass,ValueClass,KeyComparer>::const_iterator rend(const YsBinaryTree2<KeyClass,ValueClass,KeyComparer> &tree)
{
	return tree.rend();
}


////////////////////////////////////////////////////////////

template <class KeyClass,class ValueClass,class KeyComparer>
inline void YsBinaryTree2<KeyClass,ValueClass,KeyComparer>::Node::Initialize(void)
{
	left.Nullify();
	right.Nullify();
	up.Nullify();
	heightR=0;
	heightL=0;
}

template <class KeyClass,class ValueClass,class KeyComparer>
inline int YsBinaryTree2<KeyClass,ValueClass,KeyComparer>::Node::Balance(void) const
{
	return heightL-heightR;
}

////////////////////////////////////////////////////////////

template <class KeyClass,class ValueClass,class KeyComparer>
bool YsBinaryTree2<KeyClass,ValueClass,KeyComparer>::NodeHandle::operator==(NodeHandle hd) const
{
	return (this->nodeIndex==hd.nodeIndex);
}
template <class KeyClass,class ValueClass,class KeyComparer>
bool YsBinaryTree2<KeyClass,ValueClass,KeyComparer>::NodeHandle::operator!=(NodeHandle hd) const
{
	return (this->nodeIndex!=hd.nodeIndex);
}

template <class KeyClass,class ValueClass,class KeyComparer>
bool YsBinaryTree2<KeyClass,ValueClass,KeyComparer>::NodeHandle::operator==(std::nullptr_t) const
{
	return this->IsNull();
}
template <class KeyClass,class ValueClass,class KeyComparer>
bool YsBinaryTree2<KeyClass,ValueClass,KeyComparer>::NodeHandle::operator!=(std::nullptr_t) const
{
	return this->IsNotNull();
}

template <class KeyClass,class ValueClass,class KeyComparer>
void YsBinaryTree2<KeyClass,ValueClass,KeyComparer>::NodeHandle::Nullify(void)
{
	nodeIndex=-1;
}

template <class KeyClass,class ValueClass,class KeyComparer>
bool YsBinaryTree2<KeyClass,ValueClass,KeyComparer>::NodeHandle::IsNull(void) const
{
	return (-1==nodeIndex);
}

template <class KeyClass,class ValueClass,class KeyComparer>
bool YsBinaryTree2<KeyClass,ValueClass,KeyComparer>::NodeHandle::IsNotNull(void) const
{
	return (-1!=nodeIndex);
}

////////////////////////////////////////////////////////////

template <class KeyClass,class ValueClass,class KeyComparer>
YsBinaryTree2<KeyClass,ValueClass,KeyComparer>::YsBinaryTree2()
{
	CleanUp();
}
template <class KeyClass,class ValueClass,class KeyComparer>
YsBinaryTree2<KeyClass,ValueClass,KeyComparer>::~YsBinaryTree2()
{
}

template <class KeyClass,class ValueClass,class KeyComparer>
void YsBinaryTree2<KeyClass,ValueClass,KeyComparer>::CleanUp(void)
{
	nNode=0;
	nodeStore.CleanUp();
	availableIndex.CleanUp();
	rootHd.Nullify();
}

template <class KeyClass,class ValueClass,class KeyComparer>
typename YsBinaryTree2<KeyClass,ValueClass,KeyComparer>::NodeHandle YsBinaryTree2<KeyClass,ValueClass,KeyComparer>::CreateNode(void)
{
	indexType idx;
	if(0<availableIndex.GetN())
	{
		idx=(indexType)availableIndex.Last();
		availableIndex.DeleteLast();
	}
	else
	{
		idx=(indexType)nodeStore.GetN();
		nodeStore.Increment();
	}
	nodeStore[idx].Initialize();
	NodeHandle ndHd;
	ndHd.nodeIndex=idx;
	return ndHd;
}

template <class KeyClass,class ValueClass,class KeyComparer>
void YsBinaryTree2<KeyClass,ValueClass,KeyComparer>::MakeNodeAvailable(NodeHandle ndHd)
{
	availableIndex.Append(ndHd.nodeIndex);
}

template <class KeyClass,class ValueClass,class KeyComparer>
void YsBinaryTree2<KeyClass,ValueClass,KeyComparer>::UpdateUp(NodeHandle upHd,YSBOOL isLeftOfUp,NodeHandle newLeafHd)
{
	auto up=GetNode(upHd);
	if(NULL==up)
	{
		rootHd=newLeafHd;
	}
	else
	{
		if(YSTRUE==isLeftOfUp)
		{
			up->left=newLeafHd;
		}
		else
		{
			up->right=newLeafHd;
		}
	}

	auto newLeaf=GetNode(newLeafHd);
	if(NULL!=newLeaf)
	{
		newLeaf->up=upHd;
	}
}

template <class KeyClass,class ValueClass,class KeyComparer>
void YsBinaryTree2<KeyClass,ValueClass,KeyComparer>::UpdateHeightAutoStop(NodeHandle ndHd)
{
	auto currentHd=ndHd;
	auto current=GetNode(ndHd);

	if(NULL==current)
	{
		return;
	}

	UpdateHeightSingle(current);

	int currentHeight=YsGreater(current->heightL,current->heightR);
	{
		auto upHd=Up(currentHd);
		auto up=GetNode(upHd);
		while(NULL!=up)
		{
			const int beforeHeight=YsGreater(up->heightL,up->heightR);
			if(up->left==currentHd)
			{
				up->heightL=1+currentHeight;
			}
			else // if(up->right==currentHd)
			{
				up->heightR=1+currentHeight;
			}
			currentHeight=YsGreater(up->heightL,up->heightR);
			if(beforeHeight==currentHeight) // Total height does not change, means the balance above this node won't change.
			{
				break;
			}
			currentHd=upHd;
			upHd=Up(currentHd);
			up=GetNode(upHd);
		}
	}
}

template <class KeyClass,class ValueClass,class KeyComparer>
void YsBinaryTree2<KeyClass,ValueClass,KeyComparer>::UpdateHeightSingle(Node *node)
{
	{
		auto left=GetNode(node->left);
		if(NULL!=left)
		{
			node->heightL=1+YsGreater(left->heightL,left->heightR);
		}
		else
		{
			node->heightL=0;
		}
	}
	{
		auto right=GetNode(node->right);
		if(NULL!=right)
		{
			node->heightR=1+YsGreater(right->heightL,right->heightR);
		}
		else
		{
			node->heightR=0;
		}
	}
}

template <class KeyClass,class ValueClass,class KeyComparer>
typename YsBinaryTree2<KeyClass,ValueClass,KeyComparer>::NodeHandle YsBinaryTree2<KeyClass,ValueClass,KeyComparer>::LeftMostOf(int &depth,NodeHandle ndHd) const
{
	depth=0;
	if(Null()!=ndHd)
	{
		auto leftHd=Left(ndHd);
		while(Null()!=leftHd)
		{
			ndHd=leftHd;
			leftHd=Left(ndHd);
			++depth;
		}
	}
	return ndHd;
}

template <class KeyClass,class ValueClass,class KeyComparer>
typename YsBinaryTree2<KeyClass,ValueClass,KeyComparer>::NodeHandle YsBinaryTree2<KeyClass,ValueClass,KeyComparer>::RightMostOf(int &depth,NodeHandle ndHd) const
{
	depth=0;
	if(Null()!=ndHd)
	{
		auto rightHd=Right(ndHd);
		while(Null()!=rightHd)
		{
			ndHd=rightHd;
			rightHd=Right(ndHd);
			++depth;
		}
	}
	return ndHd;
}

template <class KeyClass,class ValueClass,class KeyComparer>
typename YsBinaryTree2<KeyClass,ValueClass,KeyComparer>::Node *YsBinaryTree2<KeyClass,ValueClass,KeyComparer>::GetNode(NodeHandle ndHd)
{
	if(ndHd.IsNotNull())
	{
		return &nodeStore[ndHd.nodeIndex];
	}
	return NULL;
}

template <class KeyClass,class ValueClass,class KeyComparer>
const typename YsBinaryTree2<KeyClass,ValueClass,KeyComparer>::Node *YsBinaryTree2<KeyClass,ValueClass,KeyComparer>::GetNode(NodeHandle ndHd) const
{
	if(ndHd.IsNotNull())
	{
		return &nodeStore[ndHd.nodeIndex];
	}
	return NULL;
}

template <class KeyClass,class ValueClass,class KeyComparer>
int YsBinaryTree2<KeyClass,ValueClass,KeyComparer>::Balance(NodeHandle ndHd) const
{
	auto node=GetNode(ndHd);
	if(NULL!=node)
	{
		return node->Balance();
	}
	return 0;
}

template <class KeyClass,class ValueClass,class KeyComparer>
YSSIZE_T YsBinaryTree2<KeyClass,ValueClass,KeyComparer>::GetN(void) const
{
	return nNode;
}

template <class KeyClass,class ValueClass,class KeyComparer>
KeyClass YsBinaryTree2<KeyClass,ValueClass,KeyComparer>::GetKey(NodeHandle ndHd) const
{
	return nodeStore[ndHd.nodeIndex].key;  // What if node==NULL?  Just let it go.
}

template <class KeyClass,class ValueClass,class KeyComparer>
ValueClass &YsBinaryTree2<KeyClass,ValueClass,KeyComparer>::GetValue(NodeHandle ndHd)
{
	return nodeStore[ndHd.nodeIndex].value;  // What if node==NULL?  Just let it go.
}

template <class KeyClass,class ValueClass,class KeyComparer>
ValueClass &YsBinaryTree2<KeyClass,ValueClass,KeyComparer>::operator[](NodeHandle ndHd)
{
	return GetValue(ndHd);
}

template <class KeyClass,class ValueClass,class KeyComparer>
const ValueClass &YsBinaryTree2<KeyClass,ValueClass,KeyComparer>::operator[](NodeHandle ndHd) const
{
	return GetValue(ndHd);
}

template <class KeyClass,class ValueClass,class KeyComparer>
const ValueClass &YsBinaryTree2<KeyClass,ValueClass,KeyComparer>::GetValue(NodeHandle ndHd) const
{
	return nodeStore[ndHd.nodeIndex].value;  // What if node==NULL?  Just let it go.
}

template <class KeyClass,class ValueClass,class KeyComparer>
typename YsBinaryTree2<KeyClass,ValueClass,KeyComparer>::NodeHandle YsBinaryTree2<KeyClass,ValueClass,KeyComparer>::FindNode(KeyClass key) const
{
	auto ndHd=RootNode();
	while(Null()!=ndHd)
	{
		if(KeyComparer::Equal(key,GetKey(ndHd)))
		{
			return ndHd;
		}
		else if(KeyComparer::AIsSmallerThanB(key,GetKey(ndHd)))
		{
			ndHd=Left(ndHd);
		}
		else
		{
			ndHd=Right(ndHd);
		}
	}
	return ndHd;
}

template <class KeyClass,class ValueClass,class KeyComparer>
YSBOOL YsBinaryTree2<KeyClass,ValueClass,KeyComparer>::IsKeyIncluded(KeyClass key) const
{
	auto found=FindNode(key);
	if(found.IsNotNull())
	{
		return YSTRUE;
	}
	else
	{
		return YSFALSE;
	}
}

template <class KeyClass,class ValueClass,class KeyComparer>
typename YsBinaryTree2<KeyClass,ValueClass,KeyComparer>::NodeHandle YsBinaryTree2<KeyClass,ValueClass,KeyComparer>::Left(NodeHandle ndHd) const
{
	if(ndHd.IsNotNull())
	{
		auto *node=GetNode(ndHd);
		return node->left;
	}
	return Null();
}

template <class KeyClass,class ValueClass,class KeyComparer>
typename YsBinaryTree2<KeyClass,ValueClass,KeyComparer>::NodeHandle YsBinaryTree2<KeyClass,ValueClass,KeyComparer>::Up(NodeHandle ndHd) const
{
	if(ndHd.IsNotNull())
	{
		auto *node=GetNode(ndHd);
		return node->up;
	}
	return Null();
}

template <class KeyClass,class ValueClass,class KeyComparer>
typename YsBinaryTree2<KeyClass,ValueClass,KeyComparer>::NodeHandle YsBinaryTree2<KeyClass,ValueClass,KeyComparer>::Right(NodeHandle ndHd) const
{
	if(ndHd.IsNotNull())
	{
		auto *node=GetNode(ndHd);
		return node->right;
	}
	return Null();
}

template <class KeyClass,class ValueClass,class KeyComparer>
YSBOOL YsBinaryTree2<KeyClass,ValueClass,KeyComparer>::IsLeftOfUp(NodeHandle ndHd) const
{
	auto up=GetNode(Up(ndHd));
	if(NULL!=up)
	{
		if(up->left==ndHd)
		{
			return YSTRUE;
		}
		else
		{
			return YSFALSE;
		}
	}
	return YSTFUNKNOWN;
}

template <class KeyClass,class ValueClass,class KeyComparer>
typename YsBinaryTree2<KeyClass,ValueClass,KeyComparer>::NodeHandle YsBinaryTree2<KeyClass,ValueClass,KeyComparer>::RootNode(void) const
{
	return rootHd;
}
template <class KeyClass,class ValueClass,class KeyComparer>
typename YsBinaryTree2<KeyClass,ValueClass,KeyComparer>::NodeHandle YsBinaryTree2<KeyClass,ValueClass,KeyComparer>::First(void) const
{
	auto currentHd=rootHd;
	auto current=GetNode(currentHd);
	if(NULL!=current)
	{
		auto left=GetNode(current->left);
		while(NULL!=left)
		{
			currentHd=current->left;
			current=left;
			left=GetNode(left->left);
		}
	}
	return currentHd;
}
template <class KeyClass,class ValueClass,class KeyComparer>
typename YsBinaryTree2<KeyClass,ValueClass,KeyComparer>::NodeHandle YsBinaryTree2<KeyClass,ValueClass,KeyComparer>::FindNext(NodeHandle ndHd) const
{
	auto node=GetNode(ndHd);
	if(node->right.IsNotNull())
	{
		int depth;
		return LeftMostOf(depth,node->right);
	}
	else
	{
		auto currentHd=ndHd;
		auto current=node;
		while(NULL!=current)
		{
			auto upHd=current->up;
			auto up=GetNode(upHd);
			if(NULL!=up && currentHd==up->left) // Coming back from the left.
			{
				return upHd;
			}
			else // Coming back from the right
			{
				currentHd=upHd;
				current=up;
			}
		}
		return currentHd;
	}
}

template <class KeyClass,class ValueClass,class KeyComparer>
typename YsBinaryTree2<KeyClass,ValueClass,KeyComparer>::NodeHandle YsBinaryTree2<KeyClass,ValueClass,KeyComparer>::Last(void) const
{
	auto currentHd=rootHd;
	auto current=GetNode(currentHd);
	if(NULL!=current)
	{
		auto right=GetNode(current->right);
		while(NULL!=right)
		{
			currentHd=current->right;
			current=right;
			right=GetNode(right->right);
		}
	}
	return currentHd;
}

template <class KeyClass,class ValueClass,class KeyComparer>
typename YsBinaryTree2<KeyClass,ValueClass,KeyComparer>::NodeHandle YsBinaryTree2<KeyClass,ValueClass,KeyComparer>::FindPrev(NodeHandle ndHd) const
{
	auto current=GetNode(ndHd);
	if(NULL!=current && Null()!=current->left)
	{
		int depth;
		return RightMostOf(depth,current->left);
	}
	else
	{
		auto currentHd=ndHd;
		while(NULL!=current)
		{
			auto upHd=current->up;
			auto up=GetNode(upHd);
			if(NULL!=up && currentHd==up->right)
			{
				return upHd;  // Coming up from the right.
			}
			else // Coming up from the left.
			{
				currentHd=upHd;
				current=up;
			}
		}
		return currentHd;
	}
}


template <class KeyClass,class ValueClass,class KeyComparer>
typename YsBinaryTree2<KeyClass,ValueClass,KeyComparer>::NodeHandle YsBinaryTree2<KeyClass,ValueClass,KeyComparer>::Insert(KeyClass key,const ValueClass &value)
{
	auto newHd=CreateNode(); // Create first!  Array may grow.
	auto newNode=GetNode(newHd);
	newNode->key=key;
	newNode->value=value;

	if(rootHd.IsNull())
	{
		rootHd=newHd;
	}
	else
	{
		NodeHandle currentHd=rootHd;
		for(;;)
		{
			if(KeyComparer::AIsSmallerThanB(key,GetKey(currentHd)))
			{
				auto leftHd=Left(currentHd);
				if(leftHd.IsNotNull())
				{
					currentHd=leftHd;
				}
				else
				{
					auto current=GetNode(currentHd);
					current->left=newHd;
					newNode->up=currentHd;
					break;;
				}
			}
			else
			{
				auto rightHd=Right(currentHd);
				if(rightHd.IsNotNull())
				{
					currentHd=rightHd;
				}
				else
				{
					auto current=GetNode(currentHd);
					current->right=newHd;
					newNode->up=currentHd;
					break;
				}
			}
		}
	}

	// Doesn't have to start from the new node.  The balance of L and R are both zero since it is a leaf node.
	UpdateHeightAutoStop(newNode->up);

	++nNode;
	return newHd;
}


/*
           upHd
             |
            ndHd
           /    \
       left     right
       /  \     /    \

Either right can be connected to the right-most of left, or left can be connected to the left-most of right

Option 1.
           upHd
             |
            ndHd
           /    \
       left     right
       /  \     /    \
          ...
            \
           RMOL  <- Right Most of Left Node
           /
       LORMOL    <- Left of Right Most of Left Node

    Detach RMOL and replace ndHd with RMOL.  Recalculate balance when RMOL is temporarily detached.

           upHd
             |
            RMOL
           /    \
       left     right
       /  \     /    \
          ...
            \
          LORMOL

    Balances need update at and above RMOL.  Since balance is re-calculated when RMOL is temporarily detached, nodes between LORMOL and RMOL are already updated.



Option 2.
           upHd
             |
            ndHd
           /    \
       left     right
       /  \     /    \
              ...
              /
            LMOR  <- Left Most of Right Node
              \
              ROLMOR    <- Right of Left Most of Right Node

    Detach LMOR and replace ndHd with LMOR.  Recalculate balance whem LMOR is temporarily detached.

           upHd
             |
            LMOR
           /    \
       left     right
       /  \     /    \
              ...
              /
            ROLMOR    <- Right of Left Most of Right Node

    Balances need update at and above LMOR.  Since balance is re-calculated when LMOR is temporarily detached, nodes between ROLMOR and LMOR are already updated.

*/

template <class KeyClass,class ValueClass,class KeyComparer>
YSRESULT YsBinaryTree2<KeyClass,ValueClass,KeyComparer>::Delete(NodeHandle ndHd)
{
	NodeHandle offBalanceNdHd;
	return Delete(offBalanceNdHd,ndHd);
}

template <class KeyClass,class ValueClass,class KeyComparer>
YSRESULT YsBinaryTree2<KeyClass,ValueClass,KeyComparer>::Delete(NodeHandle &offBalanceNdHd,NodeHandle ndHd)
{
	// printf("Deleting %d\n",GetKey(ndHd)); // Warning: This works only when KeyClass==int
	// auto deletingKey=GetKey(ndHd); // For debugging.

	if(YSOK==SimpleDetach(offBalanceNdHd,ndHd))
	{
		MakeNodeAvailable(ndHd);
		--nNode;
		return YSOK;
	}

	// At this point, node[ndHd]->Left and ->Right are both Non Null.  Otherwise, SimpleDetach must succeed.
	auto upHd=Up(ndHd);
	auto node=GetNode(ndHd);
	const YSBOOL isLeftOfUp=IsLeftOfUp(ndHd);

	// Go option 1.
	auto leftHd=node->left;
	// auto left=GetNode(leftHd);

	int depthRightMostOfLeft=0;
	auto rightMostOfLeftHd=RightMostOf(depthRightMostOfLeft,leftHd);

	if(YSOK!=SimpleDetach(offBalanceNdHd,rightMostOfLeftHd))
	{
		printf("This is not supposed to fail!  Something went wrong!\n");
		exit(1);
	}
	if(offBalanceNdHd==ndHd)
	{
		// If rightMostOfLeftHd is a direct child of ndHd, where to start re-balancing must be
		// rightMostOfLeftHd after it is put in place of ndHd.
		offBalanceNdHd=rightMostOfLeftHd;
	}

	// At this point, height must be consistent since SimpleDetach updates heights once.
	// Therefore, only above rightMostOfLeft (after taking place of ndHd) needs height update.

	// Be careful.  node->left or ->right may become Null() if rightMostOfLeft is directly under ndHd.

	auto rightMostOfLeft=GetNode(rightMostOfLeftHd);

	rightMostOfLeft->left=node->left;
	auto leftOfRightMostOfLeft=GetNode(rightMostOfLeft->left);
	if(NULL!=leftOfRightMostOfLeft)
	{
		leftOfRightMostOfLeft->up=rightMostOfLeftHd;
	}

	rightMostOfLeft->right=node->right;
	auto rightOfRightMostOfLeft=GetNode(rightMostOfLeft->right);
	if(NULL!=rightOfRightMostOfLeft)
	{
		rightOfRightMostOfLeft->up=rightMostOfLeftHd;
	}

	UpdateUp(upHd,isLeftOfUp,rightMostOfLeftHd);
	UpdateHeightAutoStop(rightMostOfLeftHd);

	MakeNodeAvailable(ndHd);
	--nNode;
	return YSOK;
}

template <class KeyClass,class ValueClass,class KeyComparer>
YSRESULT YsBinaryTree2<KeyClass,ValueClass,KeyComparer>::SimpleDetach(NodeHandle &offBalanceNdHd,NodeHandle ndHd)
{
	auto upHd=Up(ndHd);
	auto node=GetNode(ndHd);
	const YSBOOL isLeftOfUp=IsLeftOfUp(ndHd);

	auto rightHd=node->right;
	auto leftHd=node->left;

	auto right=GetNode(rightHd);
	auto left=GetNode(leftHd);

	if(NULL==left && NULL==right)
	{
		offBalanceNdHd=upHd;
		UpdateUp(upHd,isLeftOfUp,Null());
		UpdateHeightAutoStop(upHd);
		return YSOK;
	}
	else if(NULL==left)
	{
		offBalanceNdHd=upHd;
		UpdateUp(upHd,isLeftOfUp,rightHd);
		UpdateHeightAutoStop(upHd);
		return YSOK;
	}
	else if(NULL==right)
	{
		offBalanceNdHd=upHd;
		UpdateUp(upHd,isLeftOfUp,leftHd);
		UpdateHeightAutoStop(upHd);
		return YSOK;
	}
	return YSERR;
}


/*
Before rotation
         (From the up, or null.)
           |
         ndHd
         /   \
    noChange  right
              /    \
          detach   noChange

  [detach] is in between ndHd and right.


After rotation
         (From the up, or null.)
           |
         right
         /    \
       ndHd   noChange
       /   \
  noChange  detach
*/


template <class KeyClass,class ValueClass,class KeyComparer>
YSRESULT YsBinaryTree2<KeyClass,ValueClass,KeyComparer>::RotateLeft(const NodeHandle ndHd)
{
	auto node=GetNode(ndHd);
	auto upHd=node->up;
	auto up=GetNode(upHd);
	auto rightHd=node->right;
	auto right=GetNode(rightHd);

	if(NULL==right)
	{
		// Cannot rotate!
		return YSERR;
	}

	auto detachHd=right->left;  // This detached chunk is in between ndHd and right.
	auto detach=GetNode(detachHd);

	node->right=detachHd;
	if(NULL!=detach)
	{
		detach->up=ndHd;
	}


	right->left=ndHd;
	node->up=rightHd;


	if(NULL==up)
	{
		rootHd=rightHd;
		right->up.Nullify();
	}
	else
	{
		if(up->left==ndHd)
		{
			up->left=rightHd;
			right->up=upHd;
		}
		else
		{
			up->right=rightHd;
			right->up=upHd;
		}
	}

	UpdateHeightSingle(node);
	UpdateHeightAutoStop(rightHd);

	return YSOK;
}

/*
Before rotation
           (From the up, or null.)
               |
             ndHd
             /   \
         left     noChange
        /    \
noChange    detach

  [detach] is in between left and ndHd.


After rotation
           (From the up, or null.)
               |
             left
            /     \
    noChange      ndHd
                 /    \
            detach    noChange

*/
template <class KeyClass,class ValueClass,class KeyComparer>
YSRESULT YsBinaryTree2<KeyClass,ValueClass,KeyComparer>::RotateRight(const NodeHandle ndHd)
{
	auto node=GetNode(ndHd);
	auto upHd=node->up;
	auto up=GetNode(upHd);
	auto leftHd=node->left;
	auto left=GetNode(leftHd);

	if(NULL==left)
	{
		// Cannot rotate!
		return YSERR;
	}

	auto detachHd=left->right;  // This detached chunk is in between ndHd and right.
	auto detach=GetNode(detachHd);


	node->left=detachHd;
	if(NULL!=detach)
	{
		detach->up=ndHd;
	}


	left->right=ndHd;
	node->up=leftHd;


	if(NULL==up)
	{
		rootHd=leftHd;
		left->up.Nullify();
	}
	else
	{
		if(up->left==ndHd)
		{
			up->left=leftHd;
			left->up=upHd;
		}
		else
		{
			up->right=leftHd;
			left->up=upHd;
		}
	}

	UpdateHeightSingle(node);
	UpdateHeightAutoStop(leftHd);

	return YSOK;
}

template <class KeyClass,class ValueClass,class KeyComparer>
YSRESULT YsBinaryTree2<KeyClass,ValueClass,KeyComparer>::VerifyHeight(void) const
{
	printf("Testing height.\n");
	if(YSOK==VerifyHeight(rootHd))
	{
		printf("OK!\n");
		return YSOK;
	}
	return YSERR;
}

template <class KeyClass,class ValueClass,class KeyComparer>
YSRESULT YsBinaryTree2<KeyClass,ValueClass,KeyComparer>::VerifyHeight(NodeHandle ndHd) const
{
	if(Null()!=ndHd)
	{
		if(YSOK!=VerifyHeight(Left(ndHd)))
		{
			return YSERR;
		}

		auto node=GetNode(ndHd);
		auto left=GetNode(node->left);
		const int correctHeightL=(NULL!=left ? 1+YsGreater(left->heightL,left->heightR) : 0);
		if(correctHeightL!=node->heightL)
		{
			printf("Error: key=%d",node->key);
			printf("  HeightL must be %d. (HeightL=%d)\n",correctHeightL,node->heightL);
			return YSERR;
		}

		auto right=GetNode(node->right);
		const int correctHeightR=(NULL!=right ? 1+YsGreater(right->heightL,right->heightR) : 0);
		if(correctHeightR!=node->heightR)
		{
			printf("Error: key=%d",node->key);
			printf("  HeightR must be %d. (HeightR=%d)\n",correctHeightR,node->heightR);
			return YSERR;
		}

		if(YSOK!=VerifyHeight(Right(ndHd)))
		{
			return YSERR;
		}
	}
	return YSOK;
}

template <class KeyClass,class ValueClass,class KeyComparer>
YSRESULT YsBinaryTree2<KeyClass,ValueClass,KeyComparer>::VerifyNodeCount(void) const
{
	printf("Verifying Node Count...\n");

	auto ndHd=First();
	YSSIZE_T ctr=0;
	while(Null()!=ndHd)
	{
		ndHd=FindNext(ndHd);
		++ctr;
	}
	if(ctr!=nNode)
	{
		printf("%s %d\n",__FUNCTION__,__LINE__);
		printf("  Node count did not match! (nNode=%d Counted=%d)\n",(int)nNode,(int)ctr);
		return YSERR;
	}


	ndHd=Last();
	ctr=0;
	while(Null()!=ndHd)
	{
		ndHd=FindPrev(ndHd);
		++ctr;
	}
	if(ctr!=nNode)
	{
		printf("%s %d\n",__FUNCTION__,__LINE__);
		printf("  Node count (reverse) did not match! (nNode=%d Counted=%d)\n",(int)nNode,(int)ctr);
		return YSERR;
	}


	ctr=CountNode(RootNode());
	if(ctr!=nNode)
	{
		printf("%s %d\n",__FUNCTION__,__LINE__);
		printf("  Node count did not match! (nNode=%d Counted=%d)\n",(int)nNode,(int)ctr);
		return YSERR;
	}

	printf("OK!\n");

	return YSOK;
}

template <class KeyClass,class ValueClass,class KeyComparer>
YSSIZE_T YsBinaryTree2<KeyClass,ValueClass,KeyComparer>::CountNode(NodeHandle ndHd) const
{
	if(Null()!=ndHd)
	{
		return 1+CountNode(Left(ndHd))+CountNode(Right(ndHd));
	}
	return 0;
}

template <class KeyClass,class ValueClass,class KeyComparer>
YSRESULT YsBinaryTree2<KeyClass,ValueClass,KeyComparer>::VerifyOrder(void) const
{
	printf("Verifying Order.\n");

	auto ndHd=First();
	auto prevNdHd=Null();
	while(Null()!=ndHd)
	{
		if(Null()!=prevNdHd)
		{
			if(KeyComparer::AIsGreaterThanB(GetKey(prevNdHd),GetKey(ndHd)))
			{
				printf("%s %d\n",__FUNCTION__,__LINE__);
				printf("  Order check failed!\n");
				printf("  %d %d\n",GetKey(prevNdHd),GetKey(ndHd));
				return YSERR;
			}
		}
		prevNdHd=ndHd;
		ndHd=FindNext(ndHd);
	}

	ndHd=Last();
	prevNdHd=Null();
	while(Null()!=ndHd)
	{
		if(Null()!=prevNdHd)
		{
			if(KeyComparer::AIsSmallerThanB(GetKey(prevNdHd),GetKey(ndHd)))
			{
				printf("%s %d\n",__FUNCTION__,__LINE__);
				printf("  Order check (Reverse) failed!\n");
				printf("  %d %d\n",GetKey(prevNdHd),GetKey(ndHd));
				return YSERR;
			}
		}
		prevNdHd=ndHd;
		ndHd=FindPrev(ndHd);
	}

	printf("OK!\n");

	return YSOK;
}

template <class KeyClass,class ValueClass,class KeyComparer>
void YsBinaryTree2<KeyClass,ValueClass,KeyComparer>::MoveToNext(NodeHandle &ndHd) const
{
	if(Null()!=ndHd)
	{
		ndHd=FindNext(ndHd);
	}
	else
	{
		ndHd=First();
	}
}
template <class KeyClass,class ValueClass,class KeyComparer>
void YsBinaryTree2<KeyClass,ValueClass,KeyComparer>::MoveToPrev(NodeHandle &ndHd) const
{
	if(Null()!=ndHd)
	{
		ndHd=FindPrev(ndHd);
	}
	else
	{
		ndHd=Last();
	}
}
template <class KeyClass,class ValueClass,class KeyComparer>
/* static */ typename YsBinaryTree2<KeyClass,ValueClass,KeyComparer>::NodeHandle YsBinaryTree2<KeyClass,ValueClass,KeyComparer>::Null(void)
{
	NodeHandle ndHd;
	ndHd.Nullify();
	return ndHd;
}
template <class KeyClass,class ValueClass,class KeyComparer>
ValueClass &YsBinaryTree2<KeyClass,ValueClass,KeyComparer>::Value(NodeHandle ndHd)
{
	return nodeStore[ndHd.nodeIndex].value;  // What if node==NULL?  Just let it go.
}
template <class KeyClass,class ValueClass,class KeyComparer>
const ValueClass &YsBinaryTree2<KeyClass,ValueClass,KeyComparer>::Value(NodeHandle ndHd) const
{
	return nodeStore[ndHd.nodeIndex].value;  // What if node==NULL?  Just let it go.
}

/* } */
#endif
