/* ////////////////////////////////////////////////////////////

File Name: ystempobj.h
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

#ifndef YSTEMPOBJ_IS_INCLUDED
#define YSTEMPOBJ_IS_INCLUDED
/* { */


template <class T>
class YsTemporaryObject
{
public:
	YsTemporaryObject();
	~YsTemporaryObject();

	T *GetSingle(void);
	T *GetArray(int n);

	YSRESULT Release(void);

private:
	YSBOOL usedAsAnArray;
	T *ptr;
};

template <class T>
YsTemporaryObject <T>::YsTemporaryObject()
{
	usedAsAnArray=YSFALSE;
	ptr=NULL;
}

template <class T>
YsTemporaryObject <T>::~YsTemporaryObject()
{
	Release();
}

template <class T>
T *YsTemporaryObject <T>::GetSingle(void)
{
	if(ptr==NULL)
	{
		ptr=new T;
		if(ptr!=NULL)
		{
			usedAsAnArray=YSFALSE;
			return ptr;
		}
		else
		{
			printf("YsTemporaryObject::GetSingle()\n");
			printf("  Cannot allocate memory block.\n");
			return NULL;
		}
	}
	else
	{
		printf("YsTemporaryObject::GetSingle()\n");
		printf("  This instance is already used.\n");
		return NULL;
	}
}

template <class T>
T *YsTemporaryObject <T>::GetArray(int n)
{
	if(ptr==NULL)
	{
		ptr=new T [n];
		if(ptr!=NULL)
		{
			usedAsAnArray=YSTRUE;
			return ptr;
		}
		else
		{
			printf("YsTemporaryObject::GetArray()\n");
			printf("  Cannot allocate memory block.\n");
			return NULL;
		}
	}
	else
	{
		printf("YsTemporaryObject::GetArray()\n");
		printf("  This instance is already used.\n");
		return NULL;
	}
}

template <class T>
YSRESULT YsTemporaryObject <T>::Release(void)
{

	if(ptr!=NULL)
	{
		if(usedAsAnArray==YSTRUE)
		{
			delete [] ptr;
		}
		else
		{
			delete ptr;
		}
		ptr=NULL;
	}
	return YSOK;
}

/* } */
#endif
