/* ////////////////////////////////////////////////////////////

File Name: refbmp.cpp
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
#include "refbmp.h"
#include <ysgl.h>
#include <ysport.h>

PolyCreReferenceBitmap::PolyCreReferenceBitmap()
{
	textureNamePrepared=YSFALSE;
	Initialize();
}

PolyCreReferenceBitmap::~PolyCreReferenceBitmap()
{
	Initialize();
}

void PolyCreReferenceBitmap::Initialize(void)
{
	loaded=YSFALSE;
	fn.Set(L"");
	corner[0]=YsOrigin();
	corner[1]=YsOrigin();
	corner[2]=YsOrigin();
	corner[3]=YsOrigin();
	show=YSTRUE;
	alignCenter=YSFALSE;
	alpha=0.3;

	if(YSTRUE==textureNamePrepared)
	{
		glDeleteTextures(1,&textureName);
		textureNamePrepared=YSFALSE;
	}

	textureCached=YSFALSE;
	textureName=~(unsigned int)0;
}

YSRESULT PolyCreReferenceBitmap::Load(const char fn[])
{
	YSRESULT res=YSERR;

	if(YSFTYPE_BMP==YsIdentifyFileType(fn))
	{
		res=bmp.LoadBmp(fn);
	}
	else if(YSFTYPE_PNG==YsIdentifyFileType(fn))
	{
		res=bmp.LoadPng(fn);
	}

	if(YSOK==res)
	{
		CalculateScalingAndPadding();
		ApplyScalingAndPadding();
		MakeTransparency();

		loaded=YSTRUE;
		this->fn.SetUTF8String(fn);
	}

	return res;
}

void PolyCreReferenceBitmap::CalculateScalingAndPadding(void)
{
	int maxTextureSize;
	glGetIntegerv(GL_MAX_TEXTURE_SIZE,&maxTextureSize);


	int scaleSize=1;
	while((bmp.GetHeight()>scaleSize || bmp.GetWidth()>scaleSize) && scaleSize*2<maxTextureSize)
	{
		scaleSize*=2;
	}

	if(bmp.GetWidth()>=bmp.GetHeight())
	{
		scaleWid=scaleSize;
		scaleHei=scaleSize*bmp.GetHeight()/bmp.GetWidth();

		paddedWid=scaleSize;
		for(paddedHei=1; paddedHei<scaleHei && paddedHei*2<maxTextureSize; paddedHei*=2)
		{
		}
	}
	else
	{
		scaleWid=scaleSize*bmp.GetWidth()/bmp.GetHeight();
		scaleHei=scaleSize;

		for(paddedWid=1; paddedWid<scaleWid && paddedWid*2<maxTextureSize; paddedWid*=2)
		{
		}
		paddedHei=scaleSize;
	}
}

void PolyCreReferenceBitmap::ApplyScalingAndPadding(void)
{
	YsBitmap temp;
	temp.ScaleCopy(scaleWid,scaleHei,bmp);

	scaled.PrepareBitmap(paddedWid,paddedHei);
	for(int y=0; y<scaled.GetHeight(); y++)
	{
		for(int x=0; x<scaled.GetWidth(); x++)
		{
			scaled.SetR(x,y,255);
			scaled.SetG(x,y,255);
			scaled.SetB(x,y,255);
			scaled.SetA(x,y,0);
		}
	}

	int offsetX=0,offsetY=0;
	if(YSTRUE==alignCenter)
	{
		offsetX=(paddedWid-scaleWid)/2;
		offsetY=(paddedHei-scaleHei)/2;
	}

	scaled.Copy(temp,offsetX,offsetY);
	textureCached=YSFALSE;
}

void PolyCreReferenceBitmap::MakeTransparency(void)
{
	for(int y=0; y<scaled.GetHeight(); y++)
	{
		for(int x=0; x<scaled.GetWidth(); x++)
		{
			const unsigned char *pix;
			pix=scaled.GetRGBAPixelPointer(x,y);
			if(pix[0]>240 && pix[1]>240 && pix[2]>240)
			{
				scaled.SetA(x,y,0);
			}
			else
			{
				scaled.SetA(x,y,128);
			}
		}
	}
	textureCached=YSFALSE;
}

void PolyCreReferenceBitmap::MakeTentativeCorner(void)
{
	const double bmpAspect=(double)scaled.GetWidth()/(double)scaled.GetHeight();
	const double rectWid=5.0*bmpAspect;
	const double rectHei=5.0;

	corner[0].Set( rectWid,-rectHei,0.0);
	corner[1].Set(-rectWid,-rectHei,0.0);
	corner[2].Set(-rectWid, rectHei,0.0);
	corner[3].Set( rectWid, rectHei,0.0);
}

void PolyCreReferenceBitmap::MakeXY(void)
{
	const YsVec3 cen=(corner[0]+corner[1]+corner[2]+corner[3])/4.0;
	const double wid=(corner[1]-corner[0]).GetLength()/2.0;
	const double hei=(corner[3]-corner[0]).GetLength()/2.0;

	corner[0]=cen+YsXVec()*wid-YsYVec()*hei;
	corner[1]=cen-YsXVec()*wid-YsYVec()*hei;
	corner[2]=cen-YsXVec()*wid+YsYVec()*hei;
	corner[3]=cen+YsXVec()*wid+YsYVec()*hei;
}

void PolyCreReferenceBitmap::MakeXZ(void)
{
	const YsVec3 cen=(corner[0]+corner[1]+corner[2]+corner[3])/4.0;
	const double wid=(corner[1]-corner[0]).GetLength()/2.0;
	const double hei=(corner[3]-corner[0]).GetLength()/2.0;

	corner[0]=cen+YsXVec()*wid-YsZVec()*hei;
	corner[1]=cen-YsXVec()*wid-YsZVec()*hei;
	corner[2]=cen-YsXVec()*wid+YsZVec()*hei;
	corner[3]=cen+YsXVec()*wid+YsZVec()*hei;
}

void PolyCreReferenceBitmap::MakeYZ(void)
{
	const YsVec3 cen=(corner[0]+corner[1]+corner[2]+corner[3])/4.0;
	const double wid=(corner[1]-corner[0]).GetLength()/2.0;
	const double hei=(corner[3]-corner[0]).GetLength()/2.0;

	corner[0]=cen+YsZVec()*wid-YsYVec()*hei;
	corner[1]=cen-YsZVec()*wid-YsYVec()*hei;
	corner[2]=cen-YsZVec()*wid+YsYVec()*hei;
	corner[3]=cen+YsZVec()*wid+YsYVec()*hei;
}

void PolyCreReferenceBitmap::Draw(void) const
{
	CacheTexture();

	if(show==YSTRUE)
	{
		glEnable(GL_BLEND);
#ifdef GL_ALPHA_TEST
        glEnable(GL_ALPHA_TEST);
        glAlphaFunc(GL_GREATER,0.01F); // if Alpha==0, don't draw at all.
#endif
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		YsGLSL3DRenderer *renderer=YsGLSLSharedMonoColorBillBoard3DRenderer();

		YsGLSLUse3DRenderer(renderer);
		glBindTexture(GL_TEXTURE_2D,textureName);

		const GLfloat texCoord[8]=
		{
			-1.0f,-1.0f,
			 1.0f,-1.0f,
			 1.0f, 1.0f,
			-1.0f, 1.0f
		};
		const GLfloat vtxCoord[12]=
		{
			corner[0].xf(),corner[0].yf(),corner[0].zf(),
			corner[1].xf(),corner[1].yf(),corner[1].zf(),
			corner[2].xf(),corner[2].yf(),corner[2].zf(),
			corner[3].xf(),corner[3].yf(),corner[3].zf(),
		};
		const GLfloat vtxOffset[12]=
		{
			0.0f,0.0f,0.0f,
			0.0f,0.0f,0.0f,
			0.0f,0.0f,0.0f,
			0.0f,0.0f,0.0f,
		};

		const GLfloat color[4]=
		{
			1.0,1.0,1.0,(GLfloat)alpha
		};

		YsGLSLSet3DRendererUniformColorfv(renderer,color);
		YsGLSLDrawPrimitiveVtxVoffsetTexcoordfv(renderer,GL_TRIANGLE_FAN,4,vtxCoord,vtxOffset,texCoord);

		YsGLSLEndUse3DRenderer(renderer);
	}
}

void PolyCreReferenceBitmap::DrawHighlight(void) const
{
	YsGLSL3DRenderer *renderer=YsGLSLSharedFlat3DRenderer();
	YsGLSLUse3DRenderer(renderer);

	const GLfloat vtxCoord[12]=
	{
		corner[0].xf(),corner[0].yf(),corner[0].zf(),
		corner[1].xf(),corner[1].yf(),corner[1].zf(),
		corner[2].xf(),corner[2].yf(),corner[2].zf(),
		corner[3].xf(),corner[3].yf(),corner[3].zf(),
	};
	const GLfloat green[4]={0.0f,1.0f,0.0f,1.0f};

	glLineWidth(3);
	YsGLSLSet3DRendererUniformColorfv(renderer,green);
	YsGLSLDrawPrimitiveVtxfv(renderer,GL_LINE_LOOP,4,vtxCoord);
	glLineWidth(1);

	YsGLSLEndUse3DRenderer(renderer);
}

void PolyCreReferenceBitmap::CacheTexture(void) const
{
	if(YSTRUE!=textureNamePrepared)
	{
		glGenTextures(1,&textureName);
		textureNamePrepared=YSTRUE;
	}

	if(YSTRUE!=textureCached)
	{
		glBindTexture(GL_TEXTURE_2D,textureName);
		glPixelStorei(GL_UNPACK_ALIGNMENT,1);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
		glTexImage2D
		    (GL_TEXTURE_2D,
		     0,
		     4,
		     scaled.GetWidth(),
		     scaled.GetHeight(),
		     0,
		     GL_RGBA,
		     GL_UNSIGNED_BYTE,
		     scaled.GetRGBABitmapPointer());
		textureCached=YSTRUE;
	}
}

////////////////////////////////////////////////////////////

PolyCreReferenceBitmapStore::PolyCreReferenceBitmapStore()
{
	Initialize();
}

PolyCreReferenceBitmapStore::~PolyCreReferenceBitmapStore()
{
}

/*static*/ PolyCreReferenceBitmapStore *PolyCreReferenceBitmapStore::Create(void)
{
	return new PolyCreReferenceBitmapStore;
}

/*static*/ void PolyCreReferenceBitmapStore::Delete(PolyCreReferenceBitmapStore *ptr)
{
	delete ptr;
}

void PolyCreReferenceBitmapStore::Initialize(void)
{
	for(int i=0; i<MAX_NUM_REFBMP; ++i)
	{
		refBmp[i].Initialize();
	}
}

YSRESULT PolyCreReferenceBitmapStore::Load(const wchar_t fn[])
{
	FILE *fp=YsFileIO::Fopen(fn,"r");
	if(NULL!=fp)
	{
		Initialize();

		int version=0;

		// For Version 0 >>
		int lineNum=0;
		// For Version 0 <<
		YsArray <YsString,16> args;
		YsString str;
		while(NULL!=str.Fgets(fp))
		{
			str.Arguments(args);
			if(0<args.GetN())
			{
				if(0==strcmp(args[0],"VERSION") && 2<=args.GetN())
				{
					version=atoi(args[1]);
				}

				if(0==version)
				{
					if(0==lineNum)
					{
						// Skip
					}
					else
					{
						const int bmpIdx=(lineNum-1)/6;
						if(MAX_NUM_REFBMP>bmpIdx)
						{
							switch((lineNum-1)%6)
							{
							case 0:
								for(auto i=str.Strlen()-1; 0<=i; --i)
								{
									if(str[i]=='\"')
									{
										str.Delete(i);
									}
								}
								refBmp[bmpIdx].alignCenter=YSFALSE;
								refBmp[bmpIdx].Load(str);
								break;

							case 1:
							case 2:
							case 3:
							case 4:
								if(3<=args.GetN())
								{
									const int cornerIdx=(lineNum-1)%6-1;
									refBmp[bmpIdx].corner[cornerIdx].Set(args[0],args[1],args[2]);
								}
								break;
							case 5:
								refBmp[bmpIdx].show=(YSBOOL)atoi(args[0]);
								break;
							}
						}
					}
					++lineNum;
				}
				else if(1<=version)
				{
					if(2<=args.GetN())
					{
						const int bmpIdx=atoi(args[1]);
						if(MAX_NUM_REFBMP>bmpIdx)
						{
							if(0==strcmp(args[0],"FILE") && 3<=args.GetN())
							{
								if(YSOK==refBmp[bmpIdx].Load(args[1]))
								{
								}
								else
								{
									YsWString ful,pth,fil;
									ful.Set(fn);
									ful.SeparatePathFile(pth,fil);  // pth: RFB save path

									fil.SetUTF8String(args[2]);
									ful.MakeFullPathName(pth,fil);

									YsString fulUTF8;
									fulUTF8.EncodeUTF8 <wchar_t> (ful);

									if(YSOK==refBmp[bmpIdx].Load(fulUTF8))
									{
									}
								}
							}
							else if(0==strcmp(args[0],"CORNER") && 6<=args.GetN())
							{
								const int cornerIdx=atoi(args[2]);
								if(4>cornerIdx)
								{
									refBmp[bmpIdx].corner[cornerIdx].Set(args[3],args[4],args[5]);
								}
							}
							else if(0==strcmp(args[0],"CENTER") && 3<=args.GetN())
							{
								refBmp[bmpIdx].alignCenter=(YSBOOL)(atoi(args[2]));
							}
							else if(0==strcmp(args[0],"SHOW") && 3<=args.GetN())
							{
								refBmp[bmpIdx].show=(YSBOOL)(atoi(args[2]));
							}
							else if(0==strcmp(args[0],"ALPHA") && 3<=args.GetN())
							{
								refBmp[bmpIdx].alpha=atof(args[2]);
							}
							else if(0==strcmp(args[0],"NONE"))
							{
								refBmp[bmpIdx].loaded=YSFALSE;
							}
						}
					}
				}
			}
		}
		fclose(fp);
		return YSOK;
	}

	return YSERR;
}

YSRESULT PolyCreReferenceBitmapStore::Save(const wchar_t fn[]) const
{
	FILE *fp=YsFileIO::Fopen(fn,"w");
	if(NULL!=fp)
	{
		YsWString ful,pth,fil;
		ful.Set(fn);
		ful.SeparatePathFile(pth,fil);  // pth: RFB save path

		fprintf(fp,"VERSION 1\n");
		for(int idx=0; idx<MAX_NUM_REFBMP; ++idx)
		{
			if(YSTRUE==refBmp[idx].loaded)
			{
				for(int cornerIdx=0; cornerIdx<4; ++cornerIdx)
				{
					fprintf(fp,"CORNER %d %d %s\n",idx,cornerIdx,refBmp[idx].corner[cornerIdx].Txt());
				}

				fprintf(fp,"CENTER %d %d\n",idx,(int)refBmp[idx].alignCenter);
				fprintf(fp,"SHOW %d %d\n",idx,(int)refBmp[idx].show);
				fprintf(fp,"ALPHA %d %.3lf\n",idx,refBmp[idx].alpha);
				YsWString relPath;
				YsMakeRelativePath(relPath,refBmp[idx].fn,pth);


				// 2013/10/23 Write FILE after other settings.  This way the bitmap will be 
				//            processed according to the above settings.
				YsString utf8;
				utf8.EncodeUTF8 <wchar_t> (relPath);
				fprintf(fp,"FILE %d \"%s\"\n",idx,utf8.Txt());
			}
			else
			{
				fprintf(fp,"NONE %d\n",idx);
			}
		}
		fclose(fp);

		return YSOK;
	}
	return YSERR;
}

void PolyCreReferenceBitmapStore::Draw(void) const
{
	for(int i=0; i<MAX_NUM_REFBMP; ++i)
	{
		if(YSTRUE==refBmp[i].loaded && YSTRUE==refBmp[i].show)
		{
			refBmp[i].Draw();
		}
	}
}

void PolyCreReferenceBitmapStore::DrawHighlight(int bmpIdx) const
{
	if(0<=bmpIdx && bmpIdx<MAX_NUM_REFBMP && YSTRUE==refBmp[bmpIdx].loaded)
	{
		refBmp[bmpIdx].DrawHighlight();
	}
}
