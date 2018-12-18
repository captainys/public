/* ////////////////////////////////////////////////////////////

File Name: ysvisited.h
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

#ifndef YSVISITED_IS_INCLUDED
#define YSVISITED_IS_INCLUDED
/* { */

#include "ysdef.h"
#include "ysavltree.h"

/*!
KeyIssuer class must have a member function:
	KeyType GetSearchKey(HandleType hd) const;
which gives a unique search key for the handle.
*/
template <typename KeyIssuer,typename HandleType,const int ArrayUseThreshold=16,typename KeyType=YSHASHKEY>
class YsVisited
{
private:
	YSSIZE_T n;
	YsArray <KeyType,ArrayUseThreshold> ary;
	YsAVLTree <KeyType,int> tree;

	void ArrayToTree(void)
	{
		tree.CleanUp();
		for(auto key : ary)
		{
			tree.Insert(key,0);
		}
		ary.CleanUp();
	}
	void TreeToArray(void)
	{
		ary.CleanUp();
		for(auto hd : tree.AllHandle())
		{
			ary.Add(tree.GetKey(hd));
		}
		tree.CleanUp();
	}

public:
	YsVisited()
	{
		CleanUp();
	}
	void CleanUp(void)
	{
		n=0;
		ary.CleanUp();
		tree.CleanUp();
	}

	YSSIZE_T GetN(void) const
	{
		return n;
	}

	void Add(const KeyIssuer &issuer,HandleType hd)
	{
		if(YSTRUE!=IsIncluded(issuer,hd))
		{
			KeyType key=issuer.GetSearchKey(hd);
			if(n<ArrayUseThreshold)
			{
				ary.Add(key);
			}
			else if(n==ArrayUseThreshold)
			{
				ArrayToTree();
				tree.Insert(key,0);
			}
			else
			{
				tree.Insert(key,0);
			}
			++n;
		}
	}

	template <typename StorageType>
	void AddMulti(const KeyIssuer &issuer,const StorageType &hdStore)
	{
		for(auto hd : hdStore)
		{
			Add(issuer,hd);
		}
	}

	YSBOOL IsIncluded(const KeyIssuer &issuer,HandleType hd) const
	{
		KeyType key=issuer.GetSearchKey(hd);
		if(n<=ArrayUseThreshold)
		{
			return ary.IsIncluded(key);
		}
		else
		{
			return tree.IsKeyIncluded(key);
		}
	}
};


/* } */
#endif
