/* ////////////////////////////////////////////////////////////

File Name: ysclassmgr.cpp
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stddef.h>

#include "ysclassmgr.h"

YsClassHierarchy::YsClassHierarchy()
{
	parent=NULL;
	brother=NULL;
	children=NULL;
	classname=NULL;

	alloc=0;
	release=0;
	accum=0;
	maxAccum=0;
}

void YsClassHierarchy::SetIsChildOf(YsClassHierarchy *parentClass)
{
	if(parentClass==NULL)
	{
		printf("Ys Class Manager :\n");
		printf("    NULL Parent Class Is Detected\n");
		printf("    In intializing class [%s]\n",classname);
	}

	parent=parentClass;

	if(parentClass->children==NULL)
	{
		parentClass->children=this;
	}
	else
	{
		YsClassHierarchy *seeker;
		for(seeker=parentClass->children; seeker!=NULL; seeker=seeker->brother)
		{
			if(seeker->brother==NULL)
			{
				seeker->brother=this;
				break;
			}
		}
	}
}

void YsClassHierarchy::Allocate(void)
{
	alloc++;
	accum++;
	if(accum>maxAccum)
	{
		maxAccum=accum;
	}
}

void YsClassHierarchy::Release(void)
{
	release++;
	accum--;
}

void YsClassHierarchy::ShowHierarchy(int indent)
{
	if(this!=NULL)
	{
		int l;

		if(indent==0)
		{
			printf("Top Down Hierarchy\n");
			printf("[CLASSNAME]                     "
			       "(   Created   Released     Remain        Max)\n");
			indent+=2;
		}

		for(int i=0; i<indent; i++)
		{
			printf(" ");
		}
		l=indent+printf("[%s]",classname);
		while(l<32)
		{
			printf(" ");
			l++;
		}
		printf("(%10d %10d %10d %10d)\n",alloc,release,accum,maxAccum);

		if(children!=NULL)
		{
			children->ShowHierarchy(indent+2);
		}
		if(brother!=NULL)
		{
			brother->ShowHierarchy(indent);
		}
	}
}

void YsClassHierarchy::ShowBackTrack(int indent)
{
	if(indent==0)
	{
		printf("BackTrack[%s]\n",classname);
		indent+=2;
	}

	for(int i=0; i<indent; i++)
	{
		printf(" ");
	}
	printf("%s\n",classname);

	if(parent!=NULL)
	{
		parent->ShowBackTrack(indent+2);
	}
}

int YsClassHierarchy::IsKindOf(char *kind)
{
	int level;
	YsClassHierarchy *seeker;
	level=1;
	for(seeker=this; seeker!=NULL; seeker=seeker->parent)
	{
		if(strcmp(kind,seeker->classname)==0)
		{
			return level;
		}
		level++;
	}
	return 0;
}

unsigned YsClassHierarchy::GetLeak(void)
{
	if(this!=NULL)
	{
		return accum;
	}
	else
	{
		return 0;
	}
}
