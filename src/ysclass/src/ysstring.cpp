/* ////////////////////////////////////////////////////////////

File Name: ysstring.cpp
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

#if (defined(_WIN32) || defined(_WIN64)) && !defined(_CRT_SECURE_NO_WARNINGS)
	// This disables annoying warnings VC++ gives for use of C standard library.  Shut the mouth up.
	#define _CRT_SECURE_NO_WARNINGS
#endif

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdarg.h>


#include "ysstring.h"
#include "ysexception.h"


void YsStringHead(char **line)
{
	char *ptr=(*line);

	while(*ptr != 0)
	{
		if(isprint(*ptr) && *ptr!=' ')
		{
			break;
		}
		ptr++;
	}

	(*line)=ptr;
}


void YsStringTail(char *l)
{
	unsigned int chr;
	int i,cut,len;

	len=(int)strlen(l);
	cut=0;
	for(i=0; i<len; i++)
	{
		chr=((unsigned char *)l)[i];

		if((0x81<=chr && chr<=0x9f) || 0xe0<=chr)
		{
			cut=i+2;
			i++;
			continue;
		}
		else if(isprint(chr) && chr!=' ' && chr!='\t')
		{
			cut=i+1;
			continue;
		}
	}

	for(i=cut; i<len; i++)
	{
		l[i]=0;
	}
}

void YsCapitalize(char l[])
{
	int i;
	for(i=0; l[i]!=0; i++)
	{
		if((0x81<=((unsigned char*)l)[i] && ((unsigned char*)l)[i]<=0x9f) || 0xe0<=((unsigned char*)l)[i])
		{
			i++;
			if(l[i]==0)
			{
				break;
			}
		}
		else if('a'<=l[i] && l[i]<='z')
		{
			l[i]=l[i]+'A'-'a';
		}
	}
}

void YsUncapitalize(char l[])
{
	int i;
	for(i=0; l[i]!=0; i++)
	{
		if((0x81<=((unsigned char*)l)[i] && ((unsigned char*)l)[i]<=0x9f) || 0xe0<=((unsigned char*)l)[i])
		{
			i++;
			if(l[i]==0)
			{
				break;
			}
		}
		else if('A'<=l[i] && l[i]<='Z')
		{
			l[i]=l[i]+'a'-'A';
		}
	}
}

const char *YsOptimizeDoubleString(char buf[],const double &value)
{
	int i,j;
	sprintf(buf,"%lf",value);

	for(i=0; buf[i]!=0; i++)
	{
		if(buf[i]=='.')
		{
			for(j=(int)strlen(buf)-1; j>=i && (buf[j]=='0' || buf[j]=='.') ; j--)
			{
				buf[j]=0;
			}
			break;
		}
	}
	return buf;
}

static YSBOOL CharIsOneOf(char c,const char str[])
{
	int i;
	for(i=0; str[i]!=0; i++)
	{
		if(str[i]==c)
		{
			return YSTRUE;
		}
	}
	return YSFALSE;
}

void YsArguments2(YsArray <YsString,16> &args,const char str[],const char *blank,const char *comma)
{
	int l=(int)strlen(str);
	YsArguments2(args,str,l,blank,comma);
}

void YsArguments2(YsArray <YsString,16> &args,const char vv[],YSSIZE_T l,const char *blank,const char *comma)
{
	YsString empty;
	args.Set(0,NULL);

	YSSIZE_T i=0;
	while(YSTRUE==CharIsOneOf(vv[i],blank))
	{
		i++;
	}

	YSSIZE_T head=i;
	while(i<l)
	{
		if(vv[head]=='\"')
		{
			head++;
			i++;
			while(i<l && vv[i]!='\"')
			{
				if(vv[i]&0x80)  // 1st letter of Kanji character?
				{
					i++;
				}
				else if(isprint(vv[i])==0)
				{
					break;
				}
				i++;
			}

			args.Append(empty);
			args[args.GetN()-1].Set(i-head,vv+head);

			if(vv[i]=='\"')
			{
				i++;
			}
		}
		else
		{
			while(i<l && (CharIsOneOf(vv[i],blank)!=YSTRUE && CharIsOneOf(vv[i],comma)!=YSTRUE))
			{
				if(vv[i]&0x80)  // 1st letter of Kanji character?
				{
					i++;
				}
				else if(isprint(vv[i])==0)
				{
					break;
				}
				i++;
			}

			if(head<i)  // <- This condition is added on 2005/03/03
			{
				args.Append(empty);
				args[args.GetN()-1].Set(i-head,vv+head);
			}
			else if(head==i && YSTRUE==CharIsOneOf(vv[i],comma)) // < This condition is added (I thought there was, did I accidentally delet?) on 2012/01/26
			{
				args.Increment();
				args.GetEnd().Set("");
			}
		}

		while(i<l && isprint(vv[i])==0)
		{
			i++;
		}
		while(i<l && CharIsOneOf(vv[i],blank)==YSTRUE)  // Skip blank separator
		{
			i++;
		}
		if(CharIsOneOf(vv[i],comma)==YSTRUE) // Skip one comma separator
		{
			i++;

			while(i<l && CharIsOneOf(vv[i],blank)==YSTRUE)  // Skip blank separator after a comma separator
			{
				i++;
			}

			if(i==l)
			{
				args.Append(empty);
			}
		}
		head=i;
	}
}

/***********************************************************************/

int YsString::Printf(const char *fom,...)
{
	const int trialBufSize=256;

	char buf[trialBufSize];
	va_list arg_ptr;

	va_start(arg_ptr,fom);
	const int n=vsnprintf(buf,trialBufSize,fom,arg_ptr);
	va_end(arg_ptr);

	if(n<0)
	{
		// This compiler s**ks!  It's not C99 compliant!  (I'm talking about Visual C++ 2008)
		int bufSize=trialBufSize*2;
		YsArray <char> buf;
		for(;;)
		{
			buf.Set(bufSize,NULL);

			va_start(arg_ptr,fom);
			const int n=vsnprintf(buf,bufSize,fom,arg_ptr);
			va_end(arg_ptr);

			if(0<=n && n<bufSize)
			{
				YsArray <char,16>::Set(n+1,buf);
				return n;
			}

			bufSize*=2;
		}
	}
	else if(n<trialBufSize)
	{
		YsArray <char,16>::Set(n+1,buf);
		return n;
	}
	else
	{
		// Good compiler!  It is C99 Compliant.
		char *buf=new char [n+1];
		va_start(arg_ptr,fom);
		vsnprintf(buf,n+1,fom,arg_ptr);
		va_end(arg_ptr);

		YsArray <char,16>::Set(n+1,buf);

		delete [] buf;
		return n;
	}
}

