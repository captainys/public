/* ////////////////////////////////////////////////////////////

File Name: ysargs.cpp
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
#include <string.h>
#include <ctype.h>

#include "ysdef.h"
#include "ysargs.h"

/***********************************************************************/
static char *YsStrtok(char **src);

YSRESULT YsArguments(int *ac,char *av[],int mxac,char *src)
{
	char *arg;
	YSSIZE_T len;

	while(*src==' ' || *src=='\t')
	{
		src++;
	}

	len=strlen(src)-1;
	while(len>=0 && isprint(src[len])==0)
	{
		src[len]=0;
		len--;
	}

	*ac=0;
	while((arg=YsStrtok(&src))!=NULL && *ac<mxac)
	{
		av[*ac]=arg;
		(*ac)++;
	}

	if(*ac<mxac)  // 2007/06/22 Added this check.
	{
		av[*ac]=NULL;
	}
	return YSOK;
}

static char *YsStrtok(char **src)
{
	char *r;
	switch(**src)
	{
	case 0:
		return NULL;
	case '\"':
		(*src)++;
		r=(*src);
		while((**src)!='\"' && (**src)!=0)
		{
			(*src)++;
		}
		break;
	default:
		r=(*src);
		while((**src)!=' ' && (**src)!='\t' && (**src)!=0)
		{
			(*src)++;
		}
		break;
	}
	if((**src)!=0)
	{
		(**src)=0;
		(*src)++;
	}
	while((**src)!=0 && ((**src)==' ' || (**src)=='\t'))
	{
		(*src)++;
	}
	return r;
}


/***********************************************************************/
YSRESULT YsCommandNumber(int *cmd,const char in[],const char *const lst[])
{
	int i;
	for(i=0; lst[i]!=NULL; i++)
	{
		YSSIZE_T l=strlen(lst[i]);
		if(strncmp(in,lst[i],l)==0)
		{
			*cmd=i;
			return YSOK;
		}
	}
	*cmd=0;
	return YSERR;
}


