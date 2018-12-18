/* ////////////////////////////////////////////////////////////

File Name: ysbinarytree.h
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

#ifndef YSBINARYTREE_IS_INCLUDED
#define YSBINARYTREE_IS_INCLUDED
/* { */

template <class KEYTYPE,class ASSOCTYPE>
class YsGenericBinaryTree;


template <class KEYTYPE,class ASSOCTYPE>
class YsBinaryTreeNode
{
friend class YsGenericBinaryTree <KEYTYPE,ASSOCTYPE>;

protected:
	YsBinaryTreeNode <KEYTYPE,ASSOCTYPE> *left,*right,*parent;

public:
	ASSOCTYPE dat;
	KEYTYPE key;

	inline YsBinaryTreeNode();
	inline YsBinaryTreeNode <KEYTYPE,ASSOCTYPE> *GetParent(void) const;
	inline YsBinaryTreeNode <KEYTYPE,ASSOCTYPE> *GetLeft(void) const;
	inline YsBinaryTreeNode <KEYTYPE,ASSOCTYPE> *GetRight(void) const;
	inline YSRESULT InsertSort(YsBinaryTreeNode <KEYTYPE,ASSOCTYPE> *newNode);
};


template <class KEYTYPE,class ASSOCTYPE>
inline YsBinaryTreeNode <KEYTYPE,ASSOCTYPE>::YsBinaryTreeNode()
{
	left=NULL;
	right=NULL;
	parent=NULL;
}

template <class KEYTYPE,class ASSOCTYPE>
inline YsBinaryTreeNode <KEYTYPE,ASSOCTYPE> *YsBinaryTreeNode <KEYTYPE,ASSOCTYPE>::GetParent(void) const
{
	return parent;
}
template <class KEYTYPE,class ASSOCTYPE>
inline YsBinaryTreeNode <KEYTYPE,ASSOCTYPE> *YsBinaryTreeNode <KEYTYPE,ASSOCTYPE>::GetLeft(void) const
{
	return left;
}
template <class KEYTYPE,class ASSOCTYPE>
inline YsBinaryTreeNode <KEYTYPE,ASSOCTYPE> *YsBinaryTreeNode <KEYTYPE,ASSOCTYPE>::GetRight(void) const
{
	return right;
}

template <class KEYTYPE,class ASSOCTYPE>
inline YSRESULT YsBinaryTreeNode <KEYTYPE,ASSOCTYPE>::InsertSort(YsBinaryTreeNode <KEYTYPE,ASSOCTYPE> *newNode)
{
	YsBinaryTreeNode <KEYTYPE,ASSOCTYPE> *track;
	track=this;
	for(;;)
	{
		if(newNode->key<=track->key)
		{
			if(track->left==NULL)
			{
				newNode->parent=track;
				track->left=newNode;
				return YSOK;
			}
			else
			{
				track=track->left;
			}
		}
		else
		{
			if(track->right==NULL)
			{
				newNode->parent=track;
				track->right=newNode;
				return YSOK;
			}
			else
			{
				track=track->right;
			}
		}
	}
	return YSERR;
}



template <class KEYTYPE,class ASSOCTYPE>
class YsGenericBinaryTree
{
private:
	YsBinaryTreeNode <KEYTYPE,ASSOCTYPE> *root;
	int nNode,depth;
	unsigned int balancer;

	// How balancer works:
	// When many equal numbers are given, the direction to traverse so that the tree looks somewhat balanced are:
	// nNode Trav   Binary number starting from 2
	//    0     L      10
	//    1     R      11
	//    2    LL     100
	//    3    LR     101
	//    4    RL     110
	//    5    RR     111
	//    6   LLL    1000
	//    7   LLR    1001
	//    8   LRL    1010
	//    9   LRR    1011
	//   10   RLL    1100
	//   11   RLR    1101
	//   12   RRL    1110
	//   13   RRR    1111

public:
	YsGenericBinaryTree();
	~YsGenericBinaryTree();
	void CleanUp(void);

	YSRESULT AddNode(const KEYTYPE &key);
	YSRESULT AddNode(const KEYTYPE &key,const ASSOCTYPE &dat);

	int GetNumNode(void) const;
	int GetMaxDepth(void) const;
private:
	YSRESULT AddNode(YsBinaryTreeNode <KEYTYPE,ASSOCTYPE> *newNode);

public:
	YsBinaryTreeNode <KEYTYPE,ASSOCTYPE> *FindNext(YsBinaryTreeNode <KEYTYPE,ASSOCTYPE> *) const;
	YsBinaryTreeNode <KEYTYPE,ASSOCTYPE> *FindPrev(YsBinaryTreeNode <KEYTYPE,ASSOCTYPE> *) const;

	const YsBinaryTreeNode <KEYTYPE,ASSOCTYPE> *GetRoot(void) const;
	const ASSOCTYPE *FindData(const KEYTYPE &key) const;

	virtual int Compare(const KEYTYPE &a,const KEYTYPE &b) const=0;
};

template <class KEYTYPE,class ASSOCTYPE>
YsGenericBinaryTree <KEYTYPE,ASSOCTYPE>::YsGenericBinaryTree()
{
	root=NULL;
}


template <class KEYTYPE,class ASSOCTYPE>
YsGenericBinaryTree <KEYTYPE,ASSOCTYPE>::~YsGenericBinaryTree()
{
	CleanUp();
}


template <class KEYTYPE,class ASSOCTYPE>
void YsGenericBinaryTree <KEYTYPE,ASSOCTYPE>::CleanUp(void)
{
	YsArray <YsBinaryTreeNode <KEYTYPE,ASSOCTYPE> *> toDel;

	YsBinaryTreeNode <KEYTYPE,ASSOCTYPE> *seeker=NULL;
	while(NULL!=(seeker=FindNext(seeker)))
	{
		toDel.Append(seeker);
	}

	for(int i=0; i<toDel.GetN(); i++)
	{
		delete toDel[i];
	}

	root=NULL;
	nNode=0;
	balancer=2;
	depth=0;
}


template <class KEYTYPE,class ASSOCTYPE>
YSRESULT YsGenericBinaryTree <KEYTYPE,ASSOCTYPE>::AddNode(const KEYTYPE &key)
{
	YsBinaryTreeNode <KEYTYPE,ASSOCTYPE> *newNode=new YsBinaryTreeNode <KEYTYPE,ASSOCTYPE>;
	if(NULL!=newNode)
	{
		newNode->key=key;
		return AddNode(newNode);
	}
	return YSERR;
}


template <class KEYTYPE,class ASSOCTYPE>
YSRESULT YsGenericBinaryTree <KEYTYPE,ASSOCTYPE>::AddNode(const KEYTYPE &key,const ASSOCTYPE &dat)
{
	YsBinaryTreeNode <KEYTYPE,ASSOCTYPE> *newNode=new YsBinaryTreeNode <KEYTYPE,ASSOCTYPE>;
	if(NULL!=newNode)
	{
		newNode->key=key;
		newNode->dat=dat;
		return AddNode(newNode);
	}
	return YSERR;
}

template <class KEYTYPE,class ASSOCTYPE>
YSRESULT YsGenericBinaryTree <KEYTYPE,ASSOCTYPE>::AddNode(YsBinaryTreeNode <KEYTYPE,ASSOCTYPE> *newNode)
{
	unsigned int currentBalancer=balancer;

	nNode++;
	balancer++;
	if(NULL==root)
	{
		root=newNode;
		depth=1;
		return YSOK;
	}

	YsBinaryTreeNode <KEYTYPE,ASSOCTYPE> *track=root;
	int curDepth=1;
	YSBOOL done=YSFALSE;
	while(YSTRUE!=done)
	{
		curDepth++;
		const int cmp=Compare(newNode->key,track->key);
		int dir=0;

		if(cmp<0)
		{
			dir=0;
		}
		else if(cmp>0)
		{
			dir=1;
		}
		else
		{
			dir=(currentBalancer&1);
			currentBalancer>>=1;
		}

		switch(dir)
		{
		case 0:
			if(track->left==NULL)
			{
				newNode->parent=track;
				track->left=newNode;
				done=YSTRUE;
			}
			else
			{
				track=track->left;
			}
			break;
		case 1:
			if(track->right==NULL)
			{
				newNode->parent=track;
				track->right=newNode;
				done=YSTRUE;
			}
			else
			{
				track=track->right;
			}
			break;
		}
	}

	if(depth<curDepth)
	{
		depth=curDepth;
	}

	return YSOK;
}

template <class KEYTYPE,class ASSOCTYPE>
int YsGenericBinaryTree <KEYTYPE,ASSOCTYPE>::GetNumNode(void) const
{
	return nNode;
}

template <class KEYTYPE,class ASSOCTYPE>
int YsGenericBinaryTree <KEYTYPE,ASSOCTYPE>::GetMaxDepth(void) const
{
	return depth;
}

template <class KEYTYPE,class ASSOCTYPE>
YsBinaryTreeNode <KEYTYPE,ASSOCTYPE> *YsGenericBinaryTree <KEYTYPE,ASSOCTYPE>::FindNext(YsBinaryTreeNode <KEYTYPE,ASSOCTYPE> *seeker) const
{
	if(NULL==root)
	{
		return NULL;
	}
	else if(NULL==seeker)
	{
		seeker=root;
		while(NULL!=seeker->GetLeft())
		{
			seeker=seeker->GetLeft();
		}
		return seeker;
	}
	else
	{
		if(NULL!=seeker->GetRight())
		{
			seeker=seeker->GetRight();
			while(NULL!=seeker->GetLeft())
			{
				seeker=seeker->GetLeft();
			}
			return seeker;
		}
		else
		{
			YsBinaryTreeNode <KEYTYPE,ASSOCTYPE> *prvSeeker;
			for(;;)
			{
				prvSeeker=seeker;
				seeker=seeker->GetParent();
				if(NULL==seeker)
				{
					return NULL;
				}
				else if(seeker->GetLeft()==prvSeeker)  // Came up from the left.
				{
					return seeker;
				}
			}
		}
	}
}

template <class KEYTYPE,class ASSOCTYPE>
YsBinaryTreeNode <KEYTYPE,ASSOCTYPE> *YsGenericBinaryTree <KEYTYPE,ASSOCTYPE>::FindPrev(YsBinaryTreeNode <KEYTYPE,ASSOCTYPE> *seeker) const
{
	if(NULL==root)
	{
		return NULL;
	}
	else if(NULL==seeker)
	{
		seeker=root;
		while(NULL!=seeker->GetRight())
		{
			seeker=seeker->GetRight();
		}
		return seeker;
	}
	else
	{
		if(NULL!=seeker->GetLeft())
		{
			seeker=seeker->GetLeft();
			while(NULL!=seeker->GetRight())
			{
				seeker=seeker->GetRight();
			}
			return seeker;
		}
		else
		{
			YsBinaryTreeNode <KEYTYPE,ASSOCTYPE> *prvSeeker;
			for(;;)
			{
				prvSeeker=seeker;
				seeker=seeker->GetParent();
				if(NULL==seeker)
				{
					return NULL;
				}
				else if(seeker->GetRight()==prvSeeker)  // Came up from the right.
				{
					return seeker;
				}
			}
		}
	}
}

template <class KEYTYPE,class ASSOCTYPE>
const ASSOCTYPE *YsGenericBinaryTree <KEYTYPE,ASSOCTYPE>::FindData(const KEYTYPE &key) const
{
	YsBinaryTreeNode <KEYTYPE,ASSOCTYPE> *node=root;
	while(NULL!=node)
	{
		int c=Compare(key,node->key);
		if(0==c)
		{
			return &node->dat;
		}
		else if(0<c)
		{
			node=node->GetRight();
		}
		else if(0>c)
		{
			node=node->GetLeft();
		}
	}
	return NULL;
}

template <class KEYTYPE,class ASSOCTYPE>
const YsBinaryTreeNode <KEYTYPE,ASSOCTYPE> *YsGenericBinaryTree <KEYTYPE,ASSOCTYPE>::GetRoot(void) const
{
	return root;
}

////////////////////////////////////////////////////////////

template <class KEYTYPE,class ASSOCTYPE>
class YsBasicBinaryTree : public YsGenericBinaryTree <KEYTYPE,ASSOCTYPE> // Usable if compare operators are defined for KEYTYPE.
{
public:
	virtual int Compare(const KEYTYPE &a,const KEYTYPE &b) const;
};

template <class KEYTYPE,class ASSOCTYPE>
int YsBasicBinaryTree <KEYTYPE,ASSOCTYPE>::Compare(const KEYTYPE &a,const KEYTYPE &b) const
{
	const KEYTYPE dif=a-b;
	if(0>dif)
	{
		return -1;
	}
	else if(0<dif)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

////////////////////////////////////////////////////////////

template <class KEYTYPE,class ASSOCTYPE>
class YsFloatBinaryTree : public YsGenericBinaryTree <KEYTYPE,ASSOCTYPE> // KEYTYPE must be float or double.
{
private:
	double tol;
public:
	YsFloatBinaryTree(const KEYTYPE tol=YsTolerance);
	void SetTolerance(const KEYTYPE tol);
	virtual int Compare(const KEYTYPE &a,const KEYTYPE &b) const;
};

template <class KEYTYPE,class ASSOCTYPE>
YsFloatBinaryTree <KEYTYPE,ASSOCTYPE>::YsFloatBinaryTree(const KEYTYPE tol)
{
	this->tol=tol;
}

template <class KEYTYPE,class ASSOCTYPE>
void YsFloatBinaryTree <KEYTYPE,ASSOCTYPE>::SetTolerance(const KEYTYPE tol)
{
	this->tol=tol;
}

template <class KEYTYPE,class ASSOCTYPE>
int YsFloatBinaryTree <KEYTYPE,ASSOCTYPE>::Compare(const KEYTYPE &a,const KEYTYPE &b) const
{
	if(tol>a-b)
	{
		return -1;
	}
	else if(tol<a-b)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

////////////////////////////////////////////////////////////

template <class KEYTYPE,class ASSOCTYPE>
class YsStringBinaryTree : public YsGenericBinaryTree <KEYTYPE,ASSOCTYPE> // KEYTYPE needs to be an array in C-string format.
{
public:
	virtual int Compare(const KEYTYPE &a,const KEYTYPE &b) const;
};

template <class KEYTYPE,class ASSOCTYPE>
int YsStringBinaryTree <KEYTYPE,ASSOCTYPE>::Compare(const KEYTYPE &a,const KEYTYPE &b) const
{
	int i;
	for(i=0; 0!=a[i] || 0!=b[i]; i++)
	{
		if(a[i]!=b[i])
		{
			return a[i]-b[i];
		}
	}
	return 0;
}

/* } */
#endif
