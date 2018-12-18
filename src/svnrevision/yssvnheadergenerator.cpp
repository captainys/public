/* ////////////////////////////////////////////////////////////

File Name: yssvnheadergenerator.cpp
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

int GetCurrentVersion(void)
{
	FILE *ifp=fopen("yssvnrevision.h","r");
	if(NULL!=ifp)
	{
		char hdrStr[256];
		while(NULL!=fgets(hdrStr,255,ifp))
		{
			hdrStr[255]=0;  // To be absolutely safe.

			for(int i=0; hdrStr[i]!=0; ++i)
			{
				if(0==strncmp("YsSvnRevisionString",hdrStr+i,19))
				{
					for(int j=i+19; hdrStr[j]!=0; ++j)
					{
						if('\"'==hdrStr[j])
						{
							fclose(ifp);
							return atoi(hdrStr+j+1);
						}
					}
				}
			}
		}
		fclose(ifp);
	}
	return -1;
}

int main(void)
{
	int curVer=GetCurrentVersion();
	printf("Current version in the header=%d\n",curVer);

	FILE *fp=fopen("revision.txt","r");
	if(NULL!=fp)
	{
		char revStr[256];
		if(NULL!=fgets(revStr,255,fp))
		{
			int err=0;

			for(int i=0; 0!=revStr[i]; i++)
			{
				if((revStr[i]<'0' || '9'<revStr[i]) &&
				   (revStr[i]<'a' || 'z'<revStr[i]) &&
				   (revStr[i]<'A' || 'Z'<revStr[i]))
				{
					revStr[i]=0;
					break;
				}
			}
			fclose(fp);

			printf("Revision String=[%s]\n",revStr);
			if(curVer!=atoi(revStr))
			{
				FILE *ofp=fopen("yssvnrevision.h","w");
				if(NULL!=ofp)
				{
					fprintf(ofp,"#ifndef YSSVNREVISION_H_IS_INCLUDED\n");
					fprintf(ofp,"#define YSSVNREVISION_H_IS_INCLUDED\n");
					fprintf(ofp,"const char *YsSvnRevisionString=\"%s\";\n",revStr);
					fprintf(ofp,"#endif\n");

					fclose(ofp);
				}
				else
				{
					err=-1;
				}

				return err;
			}
			else
			{
				printf("Doesn't have to update.\n");
				return 0;
			}
		}
	}

	if(NULL!=fp)
	{
		fclose(fp);
	}

	return -1;
}

