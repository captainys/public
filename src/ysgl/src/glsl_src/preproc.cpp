/* ////////////////////////////////////////////////////////////

File Name: preproc.cpp
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

#include <ysclass.h>

const char *const noExpand[]=
{
	"YS_GLSL_LOWP",
	"YS_GLSL_MIDP",
	"YS_GLSL_HIGHP",
	NULL
};

static YSBOOL IsNoExpand(const char str[])
{
	for(int i=0; NULL!=noExpand[i]; ++i)
	{
		if(0==strncmp(noExpand[i],str,strlen(noExpand[i])))
		{
			return YSTRUE;
		}
	}
	return YSFALSE;
}

static YSRESULT Expand(FILE *ofp,const YsString &indent,const YsString &fn)
{
	YSRESULT res=YSOK;

	FILE *ifp=fopen(fn,"r");
	if(NULL==ifp)
	{
		return YSERR;
	}

	YsString str;
	while(NULL!=str.Fgets(ifp))
	{
		str.DeleteTailSpace();
		fprintf(ofp,"%s%s\n",indent.Txt(),str.Txt());
	}
	fprintf(ofp,"\n");

	fclose(ifp);

	return res;
}

static YSRESULT GetIncludeFileName(YsString &fn,const char *ptr)
{
	if(0==strncmp(ptr,"#include",8))
	{
		ptr+=8;
	}

	while('\"'!=*ptr && 0!=*ptr)
	{
		++ptr;
	}
	if('\"'!=*ptr)
	{
		return YSERR;
	}

	fn.Set("");
	++ptr;

	while('\"'!=*ptr && 0!=*ptr)
	{
		fn.Append(*ptr);
		++ptr;
	}
	if('\"'!=*ptr)
	{
		return YSERR;
	}

	return YSOK;
}

int main(int ac,char *av[])
{
	int errCode=0;

	if(3==ac)
	{
		FILE *ifp=fopen(av[1],"r");
		FILE *hfp=fopen("../ysgldef.h","r");
		FILE *ofp=fopen(av[2],"w");
		if(NULL!=ifp && NULL!=hfp && NULL!=ofp)
		{
			YsString firstLine;

			// MacOSX OpenGL driver's bug is now taken care in ysglslutil.c
			// if(NULL!=firstLine.Fgets(ifp) && 0==strncmp(firstLine,"#version",8))
			// {
			// 	// Version must be before other statements.
			// 	fprintf(ofp,"%s\n",firstLine.Txt());
			// }

			YsString str;
			while(NULL!=str.Fgets(hfp) && 0==errCode)
			{
				str.DeleteTailSpace();

				if(0==strncmp(str,"#define YSGLSL_",15))
				{
					fprintf(ofp,"%s\n",str.Txt());
				}
			}
			fprintf(ofp,"\n");


			// if(0!=strncmp(firstLine,"#version",8))
			// {
			// 	fprintf(ofp,"%s\n",firstLine.Txt());
			// }

			while(NULL!=str.Fgets(ifp) && 0==errCode)
			{
				str.DeleteTailSpace();

				YSBOOL expanded=YSFALSE;
				YsString indent;
				for(int idx=0; idx<str.Strlen(); ++idx)
				{
					if(0==strncmp(str.Txt()+idx,"#include",8))
					{
						YsString fn;
						if(YSOK!=GetIncludeFileName(fn,str.Txt()+idx))
						{
							printf("Cannot recognize the include file name.\n");
							printf(">> %s\n",str.Txt());
							errCode=1;
							break;
						}
						
						YSRESULT res=Expand(ofp,indent,fn);
						if(YSOK==res)
						{
							expanded=YSTRUE;
						}
						else
						{
							printf("Cannot expand %s\n",str.Txt()+idx);
							errCode=1;
						}
						break;
					}
					else
					{
						indent.Append(str[idx]);
					}
				}
				if(YSTRUE!=expanded)
				{
					fprintf(ofp,"%s\n",str.Txt());
				}
			}
		}
		else
		{
			printf("Cannot open %s or %s or ../ysdef.h\n",av[1],av[2]);
			errCode=1;
		}

		if(NULL!=ifp)
		{
			fclose(ifp);
		}
		if(NULL!=hfp)
		{
			fclose(hfp);
		}
		if(NULL!=ofp)
		{
			fclose(ofp);
		}
	}

	return errCode;
}
