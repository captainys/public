/* ////////////////////////////////////////////////////////////

File Name: ysclassmgr.h
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

#ifndef YSCLASSMGR_IS_INCLUDED
#define YSCLASSMGR_IS_INCLUDED
/* { */

class YsClassHierarchy
{
public:
	YsClassHierarchy();
	YsClassHierarchy *parent,*brother,*children;
	const char *classname;

	void SetIsChildOf(YsClassHierarchy *parentClass);

	void ShowHierarchy(int indent);
	void ShowBackTrack(int indent);
	int IsKindOf(char *classname);
	unsigned alloc,release,accum,maxAccum;

	void Allocate(void);
	void Release(void);

	unsigned GetLeak(void);
};

/* } */
#endif





/* Framework for Root Class

// Declaration /////////////////////////////////////////////
class <<ROOTCLASS>>
{
public:
	static YsClassHierarchy *ClassInfo;
	static char *ClassName;
	virtual char *WhatItIs(void){return <<ROOTCLASS>>::ClassName;}
	virtual YsClassHierarchy *Hierarchy(void){return <<ROOTCLASS>>::ClassInfo;}

	int IsKindOf(char *classname){return Hierarchy()->IsKindOf(classname);}
	static unsigned GetLeak(void){return ClassInfo->GetLeak();}

	<<ROOTCLASS>>();
	virtual ~<<ROOTCLASS>>();
};



// Implementation //////////////////////////////////////////
YsClassHierarchy *<<ROOTCLASS>>::ClassInfo=NULL;
char *<<ROOTCLASS>>::ClassName="<<ROOTCLASS>>";

<<ROOTCLASS>>::<<ROOTCLASS>>()
{
	if(ClassInfo==NULL)
	{
		ClassInfo=new YsClassHierarchy;
		ClassInfo->classname=WhatItIs();
	}
	ClassInfo->Allocate();
// any initialization
}

//<<ROOTCLASS>>::<<ROOTCLASS>>(any parameter)
//{
//	if(ClassInfo==NULL)
//	{
//		ClassInfo=new YsClassHierarchy;
//		ClassInfo->classname=WhatItIs();
//	}
//	ClassInfo->Allocate();
//  any initialization
//}

<<ROOTCLASS>>::~<<ROOTCLASS>>()
{
	ClassInfo->Release();
}

*/


/* Framework for Inherited Class

// Declaration /////////////////////////////////////////////
class <<INHERITED>> : public <<BASECLASS>>
{
public:
	static YsClassHierarchy *ClassInfo;
	static char *ClassName;
	virtual char *WhatItIs(void){return <<INHERITED>>::ClassName;}
	virtual YsClassHierarchy *Hierarchy(void){return <<INHERITED>>::ClassInfo;}
	static unsigned GetLeak(void){return ClassInfo->GetLeak();}
	<<INHERITED>>();
	virtual ~<<INHERITED>>();
};



// Implementation //////////////////////////////////////////
YsClassHierarchy *<<INHERITED>>::ClassInfo=NULL;
char *<<INHERITED>>::ClassName="<<INHERITED>>";

<<INHERITED>>::<<INHERITED>>()
{
	if(ClassInfo==NULL)
	{
		ClassInfo=new YsClassHierarchy;
		ClassInfo->classname=WhatItIs();
		ClassInfo->SetIsChildOf(<<BASECLASS>>::ClassInfo);
	}
	ClassInfo->Allocate();
// any initialization
}

//<<INHERITED>>::<<INHERITED>>(any parameter)
//{
//	if(ClassInfo==NULL)
//	{
//		ClassInfo=new YsClassHierarchy;
//		ClassInfo->classname=WhatItIs();
//		ClassInfo->SetIsChildOf(<<BASECLASS>>::ClassInfo);
//	}
//	ClassInfo->Allocate();
//  any initialization
//}

<<INHERITED>>::~<<INHERITED>>()
{
	ClassInfo->Release();
}



*/
