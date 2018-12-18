/* ////////////////////////////////////////////////////////////

File Name: ysreldir.cpp
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

#ifndef _CRT_SECURE_NO_WARNINGS
// This disables annoying warnings VC++ gives for use of C standard library.  Shut the mouth up.
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <stdio.h>
#include <string.h>

#include "ysdef.h"
#include "ysreldir.h"

/***********************************************************************/
static void YsAppendSlash(char *dir);
static void YsAnalyze(char *dst,char *str,char *dir);
static void YsRemoveSamePart(char **str,char **dir);
static void YsMoveUpward(char *dst,char *dir);
static void YsAntiMacintosh(char rel[]);

#define YsIsSprtr(a) ((a)=='/' || (a)=='\\' || (a)==':')

YSRESULT YsRelativePath(char *dst,char *src,char *org)
{
	char str[256],dir[256];

	strcpy(str,src);
	strcpy(dir,org);
	YsAppendSlash(dir);
	YsAnalyze(dst,str,dir);
	YsAntiMacintosh(dst);
	return YSOK;
}

static void YsAntiMacintosh(char rel[])
{
	int i;
	/* Change ':' into '/' because Macintosh doesn't know what the relative path is. */
	for(i=0; rel[i]!=0; i++)
	{
		if(rel[i]==':')
		{
			rel[i]='/';
		}
	}
}

static void YsAppendSlash(char *dir)
{
	int ln=(int)strlen(dir);
	if(!YsIsSprtr(dir[ln-1]))
	{
		dir[ln  ]='/';
		dir[ln+1]=0;
	}
}

static void YsAnalyze(char *dst,char *str,char *dir)
{
	char relPath[256];
	YsRemoveSamePart(&str,&dir);
	YsMoveUpward(relPath,dir);
	sprintf(dst,"%s%s",relPath,str);
}

static void YsRemoveSamePart(char **str,char **dir)
{
	int n;
	char *a,*b;

	a=*str;
	b=*dir;
	while(*a!=0 && *b!=0)
	{
		while(YsIsSprtr(*a) && YsIsSprtr(*b))
		{
			a++;
			b++;
		}

		n=0;
		while(a[n]!=0 && b[n]!=0 && !YsIsSprtr(a[n]) && !YsIsSprtr(b[n]))
		{
			n++;
		}
		if(n==0 || strncmp(a,b,n)!=0)
		{
			break;
		}
		a+=n;
		b+=n;
	}
	*str=a;
	*dir=b;
}

static void YsMoveUpward(char *dst,char *dir)
{
	dst[0]=0;
	while(*dir!=0)
	{
		if(YsIsSprtr(*dir))
		{
			strcpy(dst,"../");
			dst+=3;
		}
		dir++;
	}
}

////////////////////////////////////////////////////////////

#include "ysstring.h"

template <class StringClass>
static void YsAppendSlashTemplate(StringClass &dir)
{
	const int len=(int)dir.Strlen();
	if(!YsIsSprtr(dir[len-1]))
	{
		dir.Append('/');
	}
}

template <class StringClass>
static void YsRemoveSamePartTemplate(StringClass &a,StringClass &b)
{
	while(0<a.Strlen() && 0<b.Strlen())
	{
		while(YsIsSprtr(a[0]) && YsIsSprtr(b[0]))
		{
			a.Delete(0);
			b.Delete(0);
		}

		int n=0;
		while(a[n]!=0 && b[n]!=0 && !YsIsSprtr(a[n]) && !YsIsSprtr(b[n]))
		{
			n++;
		}

		if(0==n)
		{
			return;
		}

		for(int i=0; i<n; ++i)
		{
			unsigned int p=a[i];
			unsigned int q=b[i];

			#ifdef _WIN32
			if('a'<=p && p<='z')
			{
				p=p-'a'+'A';
			}
			if('a'<=q && q<='z')
			{
				q=q-'a'+'A';
			}
			#endif

			if(p!=q)
			{
				return;
			}
		}

		for(int i=0; i<n; ++i)
		{
			a.Delete(0);
			b.Delete(0);
		}
	}
}

template <class StringClass>
static void YsMoveUpwardTemplate(StringClass &dst,StringClass &dir)
{
	dst.SetLength(0);

	int i=0;
	while(dir[i]!=0)
	{
		if(YsIsSprtr(dir[i]))
		{
			dst.Append('.');
			dst.Append('.');
			dst.Append('/');
		}
		++i;
	}
}

template <class StringClass>
static YSRESULT YsAnalyzeTemplate(StringClass &relPath,StringClass &path,StringClass &fromPath)
{
	StringClass moveUp;
	YsRemoveSamePartTemplate(path,fromPath);
	YsMoveUpwardTemplate(moveUp,fromPath);
	if(0<moveUp.Strlen())
	{
		relPath.MakeFullPathName(moveUp,path);
	}
	else
	{
		relPath=path;
	}
	return YSOK;
}

template <class StringClass,class CharClass>
static YSRESULT YsMakeRelativePathTemplate(StringClass &relPath,const CharClass fullPath[],const CharClass fromPath[])
{
	StringClass str,dir;

	str.Set(fullPath);
	dir.Set(fromPath);
	YsAppendSlashTemplate(dir);
	return YsAnalyzeTemplate(relPath,str,dir);
}

YSRESULT YsMakeRelativePath(class YsString &relPath,const char fullPath[],const char fromPath[])
{
	return YsMakeRelativePathTemplate <YsString,char> (relPath,fullPath,fromPath);
}

YSRESULT YsMakeRelativePath(class YsWString &relPath,const wchar_t fullPath[],const wchar_t fromPath[])
{
	return YsMakeRelativePathTemplate <YsWString,wchar_t> (relPath,fullPath,fromPath);
}

