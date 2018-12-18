/* ////////////////////////////////////////////////////////////

File Name: ysglbitmap.c
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

#include "ysgl.h"
#include <math.h>


void YsGLCopyBitmap(int w,int h,GLfloat *dst,const GLfloat *src)
{
	int i;
	for(i=0; i<w*h*4; ++i)
	{
		dst[i]=src[i];
	}
}

void YsGLCopyUCharBitmapToGLFloatBitmap(int w,int h,GLfloat *dst,const unsigned char *src)
{
	int i;
	for(i=0; i<w*h*4; ++i)
	{
		dst[i]=(GLfloat)src[i]/(GLfloat)255.0;
	}
}

void YsGLApplySphereMap(
    int w,int h,GLfloat *rgba,
    GLfloat ambientColor[],  // r,g,b
    int nLight,
	GLfloat lightEnabled[],
	GLfloat lightPos[],
	GLfloat lightColor[],
	int useSpecular,
	GLfloat specularColor[], // r,g,b
	GLfloat specularExponent)
{
	int y;
	for(y=0; y<h; ++y)
	{
		int x;
		GLfloat nom[3];
		nom[1]=(GLfloat)y/(GLfloat)h;
		nom[1]=(nom[1]*2.0f)-1.0f;

		for(x=0; x<w; ++x)
		{
			GLfloat d;
			GLfloat *pix=rgba+(y*w+x)*4;
			nom[0]=(GLfloat)x/(GLfloat)w;
			nom[0]=(nom[0]*2.0f)-1.0f;

			d=nom[0]*nom[0]+nom[1]*nom[1];
			if(1.0>d)
			{
				int lightNo;
				GLfloat accumColor[3];
				accumColor[0]=pix[0]*ambientColor[0],
				accumColor[1]=pix[1]*ambientColor[1],
				accumColor[2]=pix[2]*ambientColor[2],
				nom[2]=(GLfloat)sqrt(1.0f-d);

				for(lightNo=0; lightNo<nLight; lightNo++)
				{
					if(0.5f<lightEnabled[lightNo])
					{
						GLfloat dot=nom[0]*lightPos[lightNo*4]+nom[1]*lightPos[lightNo*4+1]+nom[2]*lightPos[lightNo*4+2];
						if(0.0f<=dot)
						{
							accumColor[0]+=dot*lightColor[lightNo*3];
							accumColor[1]+=dot*lightColor[lightNo*3+1];
							accumColor[2]+=dot*lightColor[lightNo*3+2];

							if(0!=useSpecular)
							{
								GLfloat mid[3],l,dot;
								mid[0]=lightPos[lightNo*4];
								mid[1]=lightPos[lightNo*4+2];
								mid[2]=lightPos[lightNo*4+3]+1.0f;
								l=(GLfloat)sqrt(mid[0]*mid[0]+mid[1]*mid[1]+mid[2]*mid[2]);
								mid[0]/=l;
								mid[1]/=l;
								mid[2]/=l;
								dot=mid[0]*nom[0]+mid[1]*nom[1]+mid[2]*nom[2];
								if(0.0f<dot)
								{
									GLfloat specular;
									specular=(GLfloat)pow(dot,specularExponent);

									accumColor[0]+=specular*specularColor[lightNo*3];
									accumColor[1]+=specular*specularColor[lightNo*3+1];
									accumColor[2]+=specular*specularColor[lightNo*3+2];
								}
							}
						}
					}
				}

				pix[0]=(accumColor[0]<1.0f ? accumColor[0] : 1.0f);
				pix[1]=(accumColor[1]<1.0f ? accumColor[1] : 1.0f);
				pix[2]=(accumColor[2]<1.0f ? accumColor[2] : 1.0f);
			}
			else
			{
				pix[0]=0.0f;
				pix[1]=0.0f;
				pix[2]=0.0f;
				pix[3]=0.0f;
			}
		}
	}
}

