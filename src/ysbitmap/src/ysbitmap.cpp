/* ////////////////////////////////////////////////////////////

File Name: ysbitmap.cpp
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

#include <limits.h>


#include "ysbitmap.h"



YsBitmap::YsBitmap()
{
}

YsBitmap::~YsBitmap()
{
	CleanUp();
}

YsBitmap::YsBitmap(const YsBitmap &incoming)
{
	CopyFrom(incoming);
}

YsBitmap &YsBitmap::operator=(const YsBitmap &incoming)
{
	CopyFrom(incoming);
	return *this;
}

YsBitmap::YsBitmap(YsBitmap &&incoming)
{
	YsBitmapTemplate <unsigned char,4>::MoveFrom(incoming);
}

YsBitmap &YsBitmap::operator=(YsBitmap &&incoming)
{
	YsBitmapTemplate <unsigned char,4>::MoveFrom(incoming);
	return *this;
}

YsBitmap YsBitmap::CutOut(int x0,int y0,int wid,int hei) const
{
	YsBitmap cut;
	YsBitmapTemplate <unsigned char,4>::CutOut(cut,x0,y0,wid,hei,0);
	return cut;
}

YsBitmap YsBitmap::Rotate90R(void) const
{
	YsBitmap rot;
	rot.Create(this->GetHeight(),this->GetWidth());
	for(int y=0; y<this->GetHeight(); ++y)
	{
		for(int x=0; x<this->GetWidth(); ++x)
		{
			auto srcPixPtr=this->GetRGBAPixelPointer(x,y);
			auto dstPixPtr=rot.GetEditableRGBAPixelPointer(this->GetHeight()-1-y,x);
			dstPixPtr[0]=srcPixPtr[0];
			dstPixPtr[1]=srcPixPtr[1];
			dstPixPtr[2]=srcPixPtr[2];
			dstPixPtr[3]=srcPixPtr[3];
		}
	}
	return rot;
}

YsBitmap YsBitmap::Rotate90L(void) const
{
	YsBitmap rot;
	rot.Create(this->GetHeight(),this->GetWidth());
	for(int y=0; y<this->GetHeight(); ++y)
	{
		for(int x=0; x<this->GetWidth(); ++x)
		{
			auto srcPixPtr=this->GetRGBAPixelPointer(x,y);
			auto dstPixPtr=rot.GetEditableRGBAPixelPointer(y,this->GetWidth()-1-x);
			dstPixPtr[0]=srcPixPtr[0];
			dstPixPtr[1]=srcPixPtr[1];
			dstPixPtr[2]=srcPixPtr[2];
			dstPixPtr[3]=srcPixPtr[3];
		}
	}
	return rot;
}

YSRESULT YsBitmap::LoadBmp(const char fn[])
{
	FILE *fp=fopen(fn,"rb");
	YSRESULT res=LoadBmp(fp);
	if(NULL!=fp)
	{
		fclose(fp);
	}
	return res;
}

YSRESULT YsBitmap::LoadBmp(FILE *fp)
{
	if(fp!=NULL)
	{
		unsigned char bmfHeader[14];
		unsigned fileSize,imgOffset;

		fread(bmfHeader,1,14,fp);
		if(bmfHeader[0]=='B' && bmfHeader[1]=='M')
		{
			fileSize=GetIntelUInt(bmfHeader+2);
			imgOffset=GetIntelUInt(bmfHeader+10);
			// printf("Filename : %s\n",fn);
			// printf("Filesize : %d\n",fileSize);
			// printf("Image Offset : %d\n",imgOffset);

			unsigned char bmiHeader[40];
			fread(bmiHeader,1,40,fp);
			unsigned headSize,width,height,nPlane,bitCount,compression;
			unsigned sizeImage,resX,resY,clrUsed,clrImportant;

			headSize=GetIntelUInt(bmiHeader);
			width=GetIntelUInt(bmiHeader+4);
			height=GetIntelUInt(bmiHeader+8);
			nPlane=GetIntelUShort(bmiHeader+12);
			bitCount=GetIntelUShort(bmiHeader+14);
			compression=GetIntelUInt(bmiHeader+16);
			sizeImage=GetIntelUInt(bmiHeader+20);
			resX=GetIntelUInt(bmiHeader+24);
			resY=GetIntelUInt(bmiHeader+28);
			clrUsed=GetIntelUInt(bmiHeader+32);
			clrImportant=GetIntelUInt(bmiHeader+36);

			// printf("Width=%d\n",width);
			// printf("Height=%d\n",height);

			if(headSize==40 &&
			   (bitCount==1 || bitCount==4 || bitCount==8 || bitCount==24) &&
			   compression==0)
			{
				unsigned nColorMap;
				unsigned char *colorMap;

				if(bitCount==1 || bitCount==4 || bitCount==8)
				{
					if(clrUsed==0)
					{
						nColorMap=(1<<bitCount);
					}
					else
					{
						nColorMap=clrUsed;
					}
					colorMap=new unsigned char [nColorMap*4];
				}
				else
				{
					nColorMap=0;
					colorMap=NULL;
				}

				if(nColorMap>0)
				{
					fread(colorMap,4,nColorMap,fp);
				}

				if(PrepareBitmap(width,height)==YSOK)
				{
					unsigned bytePerLine;
					unsigned char *lineReadBuf;

					bytePerLine=(GetWidth()*bitCount)/8;
					if(1>bytePerLine)
					{
						bytePerLine=1;
					}
					bytePerLine=(bytePerLine+3)/4;
					bytePerLine=bytePerLine*4;
					// printf("%d bytes per line.\n",bytePerLine);

					lineReadBuf=new unsigned char [bytePerLine];

					for(int y=0; y<GetHeight(); y++)
					{
						auto line=LineTop(y);

						fread(lineReadBuf,1,bytePerLine,fp);
						if(bitCount==1)
						{
							for(int x=0; x<GetWidth(); x++)
							{
								unsigned offset,bit;
								offset=x/8;
								bit=7-x%8;
								if(2<=nColorMap)
								{
									if((lineReadBuf[offset]&(1<<bit))!=0)
									{
										line[x*4  ]=colorMap[6];
										line[x*4+1]=colorMap[5];
										line[x*4+2]=colorMap[4];
										line[x*4+3]=colorMap[7];
									}
									else
									{
										line[x*4  ]=colorMap[2];
										line[x*4+1]=colorMap[1];
										line[x*4+2]=colorMap[0];
										line[x*4+3]=colorMap[3];
									}
								}
								else
								{
									if((lineReadBuf[offset]&(1<<bit))!=0)
									{
										line[x*4  ]=255;
										line[x*4+1]=255;
										line[x*4+2]=255;
										line[x*4+3]=255;
									}
									else
									{
										line[x*4  ]=0;
										line[x*4+1]=0;
										line[x*4+2]=0;
										line[x*4+3]=255;
									}
								}
							}
						}
						else if(bitCount==4)
						{
							for(int x=0; x<GetWidth(); x++)
							{
								unsigned offset,bit,index;
								offset=x/2;
								bit=(1-x%2)*4;
								index=(lineReadBuf[offset]>>bit)&0x0f;
								line[x*4  ]=colorMap[index*4+2];
								line[x*4+1]=colorMap[index*4+1];
								line[x*4+2]=colorMap[index*4  ];
								line[x*4+3]=255;
							}
						}
						else if(bitCount==8)
						{
							for(int x=0; x<GetWidth(); x++)
							{
								unsigned index;
								index=lineReadBuf[x];
								line[x*4  ]=colorMap[index*4+2];
								line[x*4+1]=colorMap[index*4+1];
								line[x*4+2]=colorMap[index*4  ];
								line[x*4+3]=255;
							}
						}
						else if(bitCount==24)
						{
							for(int x=0; x<GetWidth(); x++)
							{
								line[x*4  ]=lineReadBuf[x*3+2];
								line[x*4+1]=lineReadBuf[x*3+1];
								line[x*4+2]=lineReadBuf[x*3  ];
								line[x*4+3]=255;
							}
						}
					}

					delete [] lineReadBuf;

					if(colorMap!=NULL)
					{
						delete [] colorMap;
					}

					return YSOK;
				}
				else
				{
					printf("Could not prepare a bitmap.\n");

					if(colorMap!=NULL)
					{
						delete [] colorMap;
					}

					return YSERR;
				}
			}
			else
			{
				printf("Unknown format\n");
				printf("NOTE : Compressed BMP format is not supported.\n");
				printf("  Header Size=%d\n",headSize);
				printf("  Bit Count=%d\n",bitCount);
				printf("  Compression=%d\n",compression);
				return YSERR;
			}
		}
	}

	return YSERR;
}

YSRESULT YsBitmap::PrepareBitmap(int wid,int hei)
{
	return Create(wid,hei);
}

void YsBitmap::Clear(unsigned char r,unsigned char g,unsigned char b,unsigned char a)
{
	int i,n;
	n=GetWidth()*GetHeight()*4;
	auto dat=GetEditableBitmapPointer();
	if(r==g && g==b && b==a)
	{
		memset(dat,r,n);
	}
	else
	{
		for(i=0; i<n; i+=4)
		{
			dat[i  ]=r;
			dat[i+1]=g;
			dat[i+2]=b;
			dat[i+3]=a;
		}
	}
}

YSRESULT YsBitmap::ScaleCopy(int nWid,int nHei,const YsBitmap &from)
{
	if(PrepareBitmap(nWid,nHei)==YSOK)
	{
		int oWid,oHei;
		oWid=from.GetWidth();
		oHei=from.GetHeight();

		if(oWid<nWid && oHei<nHei)
		{
			int oldX,oldY,newX1,newY1,newX2,newY2;
			for(oldY=0; oldY<oHei-1; oldY++)
			{
				newY1= oldY   *nHei/(oHei-1);
				newY2=(oldY+1)*nHei/(oHei-1);
				for(oldX=0; oldX<oWid-1; oldX++)
				{
					newX1= oldX   *nWid/(oWid-1);
					newX2=(oldX+1)*nWid/(oWid-1);

					int newX,newY;
					const unsigned char *col[2][2];
					col[0][0]=from.GetRGBAPixelPointer(oldX  ,oldY  );
					col[0][1]=from.GetRGBAPixelPointer(oldX+1,oldY  );
					col[1][0]=from.GetRGBAPixelPointer(oldX  ,oldY+1);
					col[1][1]=from.GetRGBAPixelPointer(oldX+1,oldY+1);

					for(newY=newY1; newY<newY2; newY++)
					{
						for(newX=newX1; newX<newX2; newX++)
						{
							double s,t;
							unsigned pix[4];
							s=double(newX-newX1)/double(newX2-newX1);
							t=double(newY-newY1)/double(newY2-newY1);

							pix[0]=unsigned((double(col[0][0][0])*(1.0-s)+double(col[0][1][0])*s)*(1.0-t)
							               +(double(col[1][0][0])*(1.0-s)+double(col[1][1][0])*s)*t);
							pix[1]=unsigned((double(col[0][0][1])*(1.0-s)+double(col[0][1][1])*s)*(1.0-t)
							               +(double(col[1][0][1])*(1.0-s)+double(col[1][1][1])*s)*t);
							pix[2]=unsigned((double(col[0][0][2])*(1.0-s)+double(col[0][1][2])*s)*(1.0-t)
							               +(double(col[1][0][2])*(1.0-s)+double(col[1][1][2])*s)*t);
							pix[3]=unsigned((double(col[0][0][3])*(1.0-s)+double(col[0][1][3])*s)*(1.0-t)
							               +(double(col[1][0][3])*(1.0-s)+double(col[1][1][3])*s)*t);


							/*if(pix[0]<0)
							{
								pix[0]=0;
							}
							else*/ if(pix[0]>255)
							{
								pix[0]=255;
							}

							/*if(pix[1]<0)
							{
								pix[1]=0;
							}
							else*/ if(pix[1]>255)
							{
								pix[1]=255;
							}

							/*if(pix[2]<0)
							{
								pix[2]=0;
							}
							else*/ if(pix[2]>255)
							{
								pix[2]=255;
							}

							/*if(pix[3]<0)
							{
								pix[3]=0;
							}
							else*/ if(pix[3]>255)
							{
								pix[3]=255;
							}


							LineTop(newY)[newX*4  ]=(unsigned char)pix[0];
							LineTop(newY)[newX*4+1]=(unsigned char)pix[1];
							LineTop(newY)[newX*4+2]=(unsigned char)pix[2];
							LineTop(newY)[newX*4+3]=(unsigned char)pix[3];
						}
					}
				}
			}
		}
		else if(nWid<oWid && nHei<oHei)
		{
			int newX,newY,oldX1,oldY1,oldX2,oldY2;
			for(newY=0; newY<nHei; newY++)
			{
				oldY1=oHei*(newY  )/nHei;
				oldY2=oHei*(newY+1)/nHei;
				for(newX=0; newX<nWid; newX++)
				{
					oldX1=oWid*(newX  )/nWid;
					oldX2=oWid*(newX+1)/nWid;

					int oldX,oldY;
					unsigned pix[4],n;
					const unsigned char *col;
					pix[0]=0;
					pix[1]=0;
					pix[2]=0;
					pix[3]=0;
					n=0;
					for(oldY=oldY1; oldY<oldY2; oldY++)
					{
						for(oldX=oldX1; oldX<oldX2; oldX++)
						{
							col=from.GetRGBAPixelPointer(oldX,oldY);
							pix[0]+=unsigned(col[0]);
							pix[1]+=unsigned(col[1]);
							pix[2]+=unsigned(col[2]);
							pix[3]+=unsigned(col[3]);
							n++;
						}
					}
					pix[0]/=n;
					pix[1]/=n;
					pix[2]/=n;
					pix[3]/=n;


					/*if(pix[0]<0)
					{
						pix[0]=0;
					}
					else*/ if(pix[0]>255)
					{
						pix[0]=255;
					}

					/*if(pix[1]<0)
					{
						pix[1]=0;
					}
					else*/ if(pix[1]>255)
					{
						pix[1]=255;
					}

					/*if(pix[2]<0)
					{
						pix[2]=0;
					}
					else*/ if(pix[2]>255)
					{
						pix[2]=255;
					}

					/*if(pix[3]<0)
					{
						pix[3]=0;
					}
					else*/ if(pix[3]>255)
					{
						pix[3]=255;
					}


					LineTop(newY)[newX*4  ]=(unsigned char)pix[0];
					LineTop(newY)[newX*4+1]=(unsigned char)pix[1];
					LineTop(newY)[newX*4+2]=(unsigned char)pix[2];
					LineTop(newY)[newX*4+3]=(unsigned char)pix[3];
				}
			}
		}
		else
		{
			int newX,newY,oldX,oldY;
			const unsigned char *pix;
			for(newY=0; newY<nHei; newY++)
			{
				for(newX=0; newX<nWid; newX++)
				{
					oldX=(oWid-1)*newX/(nWid-1);
					oldY=(oHei-1)*newY/(nHei-1);
					pix=from.GetRGBAPixelPointer(oldX,oldY);
					LineTop(newY)[newX*4  ]=pix[0];
					LineTop(newY)[newX*4+1]=pix[1];
					LineTop(newY)[newX*4+2]=pix[2];
					LineTop(newY)[newX*4+3]=pix[3];
				}
			}
		}
	}
	return YSOK;
}

YSRESULT YsBitmap::ScaleCopyRaw(int wid,int hei,const YsBitmap &from)
{
	if(PrepareBitmap(wid,hei)==YSOK)
	{
		const int oWid=from.GetWidth();
		const int oHei=from.GetHeight();

		for(int y=0; y<hei; ++y)
		{
			const int oy=y*oHei/hei;
			for(int x=0; x<wid; ++x)
			{
				const int ox=x*oWid/wid;

				const unsigned char *oRGBA=from.GetRGBAPixelPointer(ox,oy);
				unsigned char *rgba=GetEditableRGBAPixelPointer(x,y);

				rgba[0]=oRGBA[0];
				rgba[1]=oRGBA[1];
				rgba[2]=oRGBA[2];
				rgba[3]=oRGBA[3];
			}
		}
	}

	return YSOK;
}

YSRESULT YsBitmap::Copy(const YsBitmap &from,int x0,int y0)
{
	for(int x=0; x<from.GetWidth(); x++)
	{
		for(int y=0; y<from.GetHeight(); y++)
		{
			if(0<=x0+x && x0+x<GetWidth() && 0<=y0+y && y0+y<GetHeight())
			{
				const unsigned char *rgba=from.GetRGBAPixelPointer(x,y);
				LineTop(y0+y)[(x0+x)*4  ]=rgba[0];
				LineTop(y0+y)[(x0+x)*4+1]=rgba[1];
				LineTop(y0+y)[(x0+x)*4+2]=rgba[2];
				LineTop(y0+y)[(x0+x)*4+3]=rgba[3];
			}
		}
	}
	return YSOK;
}

YSRESULT YsBitmap::Copy(const YsBitmap &from,int fromX0,int fromY0,int toX0,int toY0,int wid,int hei)
{
	for(int y=0; y<hei; ++y)
	{
		if(YSTRUE==IsYInRange(y+toY0) && YSTRUE==from.IsYInRange(fromY0+y))
		{
			for(int x=0; x<wid; ++x)
			{
				if(YSTRUE==IsXInRange(x+toX0) && YSTRUE==from.IsXInRange(fromX0+x))
				{
					const unsigned char *rgba=from.GetRGBAPixelPointer(fromX0+x,fromY0+y);
					LineTop(toY0+y)[(toX0+x)*4  ]=rgba[0];
					LineTop(toY0+y)[(toX0+x)*4+1]=rgba[1];
					LineTop(toY0+y)[(toX0+x)*4+2]=rgba[2];
					LineTop(toY0+y)[(toX0+x)*4+3]=rgba[3];
				}
			}
		}
	}
	return YSOK;
}

void YsBitmap::NegateRGB(void)
{
	auto dat=GetEditableBitmapPointer();
	for(int i=0; i<GetWidth()*GetHeight(); ++i)
	{
		dat[i*4  ]=255-dat[i*4  ];
		dat[i*4+1]=255-dat[i*4+1];
		dat[i*4+2]=255-dat[i*4+2];
	}
}

void YsBitmap::NegateRGBA(void)
{
	auto dat=GetEditableBitmapPointer();
	for(int i=0; i<GetWidth()*GetHeight(); ++i)
	{
		dat[i*4  ]=255-dat[i*4  ];
		dat[i*4+1]=255-dat[i*4+1];
		dat[i*4+2]=255-dat[i*4+2];
		dat[i*4+3]=255-dat[i*4+3];
	}
}

YSRESULT YsBitmap::InsecureTransfer(const YsBitmap &from,int x0,int y0)
{
	int y,x0times4;

	x0times4=x0*4;

	for(y=0; y<(int)from.GetHeight(); y++)
	{
		unsigned char *top;
		top=LineTop(y+y0)+x0times4;

		memcpy(top,from.LineTop(y),from.GetWidth()*4);
		// int x;
		// for(x=0; x<from.GetWidth(); x++)
		// {
		// 	top[x*4  ]=from.LineTop(y)[x*4  ];
		// 	top[x*4+1]=from.LineTop(y)[x*4+1];
		// 	top[x*4+2]=from.LineTop(y)[x*4+2];
		// 	top[x*4+3]=from.LineTop(y)[x*4+3];
		// }
	}
	return YSOK;
}

YSRESULT YsBitmap::Transfer(const YsBitmap &from,int x0,int y0,unsigned flags)
{
	int x,y;
	int xStart,xEnd;
	if((flags&4)==0)
	{
		xStart=(x0<0 ? -x0 : 0);
		xEnd=(((int)GetWidth())<((int)from.GetWidth())+x0 ? ((int)GetWidth())-x0 : from.GetWidth());

		for(y=0; y<(int)from.GetHeight(); y++)
		{
			int toX,toY,fromX,fromY;
			toY=y+y0;
			if(0<=toY && toY<(int)GetHeight())
			{
				unsigned char *top;
				top=LineTop(y+y0);

				for(x=xStart; x<xEnd; x++)
				{
					toX=x+x0;

					fromX=x;
					fromY=y;

					if(flags&1)
					{
						fromX=from.GetWidth()-1-fromX;
					}
					if(flags&2)
					{
						fromY=from.GetHeight()-1-fromY;
					}

					if(flags&128)
					{
						if(from.LineTop(fromY)[fromX*4+3]<top[toX*4+3])
						{
							continue;
						}
					}

				#if INT_MAX==0x7fffffff
					*((int *)(top+toX*4))=*((int *)(from.LineTop(fromY)+fromX*4));
				#else
					top[toX*4  ]=from.LineTop(fromY)[fromX*4  ];
					top[toX*4+1]=from.LineTop(fromY)[fromX*4+1];
					top[toX*4+2]=from.LineTop(fromY)[fromX*4+2];
					top[toX*4+3]=from.LineTop(fromY)[fromX*4+3];
				#endif
				}
			}
		}
	}
	else
	{
		for(y=0; y<(int)from.GetHeight(); y++)
		{
			int toX,toY,fromX,fromY;
			int xStart,xEnd;

			// toY=xStart+y0 to xEnd+y0;
			// 0<xStart+y0
			// xEnd+y0<GetHeight()
			xStart=(y0<0 ? -y0 : 0);
			xEnd=(((int)GetHeight())<((int)from.GetWidth())+y0 ? ((int)GetHeight())-y0 : from.GetWidth());

			toX=y+x0;
			if(0<=toX && toX<(int)GetWidth())
			{
				for(x=xStart; x<xEnd; x++)
				{
					toY=x+y0;

					unsigned char *top;
					top=LineTop(toY);

					fromX=x;
					fromY=y;

					if(flags&1)
					{
						fromX=from.GetWidth()-1-fromX;
					}
					if(flags&2)
					{
						fromY=from.GetHeight()-1-fromY;
					}

					if(flags&128)
					{
						if(from.LineTop(fromY)[fromX*4+3]<top[toX*4+3])
						{
							continue;
						}
					}

				#if INT_MAX==0x7fffffff
					*((int *)(top+toX*4))=*((int *)(from.LineTop(fromY)+fromX*4));
				#else
					top[toX*4  ]=from.LineTop(fromY)[fromX*4  ];
					top[toX*4+1]=from.LineTop(fromY)[fromX*4+1];
					top[toX*4+2]=from.LineTop(fromY)[fromX*4+2];
					top[toX*4+3]=from.LineTop(fromY)[fromX*4+3];
				#endif
				}
			}
		}
	}
	return YSOK;
}

YSRESULT YsBitmap::VerticalScroll(int dy,unsigned char r,unsigned char g,unsigned char b,unsigned char a)
{
	if(dy<0)
	{
		int x,y;
		for(y=0; y<(int)(GetHeight()+dy); y++)
		{
			for(x=0; x<(int)GetWidth()*4; x++)
			{
				LineTop(y)[x]=LineTop(y-dy)[x];
			}
		}
		for(y=(int)(GetHeight()+dy); y<(int)GetHeight(); y++)
		{
			for(x=0; x<(int)GetWidth()*4; x+=4)
			{
				LineTop(y)[x  ]=r;
				LineTop(y)[x+1]=g;
				LineTop(y)[x+2]=b;
				LineTop(y)[x+3]=a;
			}
		}
	}
	else
	{
		int x,y;
		for(y=GetHeight()-1; y>=dy; y--)
		{
			for(x=0; x<(int)GetWidth()*4; x++)
			{
				LineTop(y)[x]=LineTop(y-dy)[x];
			}
		}
		for(y=dy-1; y>=0; y--)
		{
			for(x=0; x<(int)GetWidth()*4; x+=4)
			{
				LineTop(y)[x  ]=r;
				LineTop(y)[x+1]=g;
				LineTop(y)[x+2]=b;
				LineTop(y)[x+3]=a;
			}
		}
	}
	return YSOK;
}

YSRESULT YsBitmap::SetR(int x,int y,unsigned char r)
{
	if(x<GetWidth() && y<GetHeight())
	{
		LineTop(y)[x*4]=r;
		return YSOK;
	}
	return YSERR;
}

YSRESULT YsBitmap::SetG(int x,int y,unsigned char g)
{
	if(x<GetWidth() && y<GetHeight())
	{
		LineTop(y)[x*4+1]=g;
		return YSOK;
	}
	return YSERR;
}

YSRESULT YsBitmap::SetB(int x,int y,unsigned char b)
{
	if(x<GetWidth() && y<GetHeight())
	{
		LineTop(y)[x*4+2]=b;
		return YSOK;
	}
	return YSERR;
}

YSRESULT YsBitmap::SetA(int x,int y,unsigned char a)
{
	if(x<GetWidth() && y<GetHeight())
	{
		LineTop(y)[x*4+3]=a;
		return YSOK;
	}
	return YSERR;
}

YSRESULT YsBitmap::SetRGBA(int x,int y,unsigned char r,unsigned char g,unsigned char b,unsigned char a)
{
	if(x<GetWidth() && y<GetHeight())
	{
		LineTop(y)[x*4  ]=r;
		LineTop(y)[x*4+1]=g;
		LineTop(y)[x*4+2]=b;
		LineTop(y)[x*4+3]=a;
		return YSOK;
	}
	return YSERR;
}

const unsigned char *YsBitmap::GetRGBABitmapPointer(void) const
{
	return GetBitmapPointer();
}

const unsigned char *YsBitmap::GetRGBAPixelPointer(int x,int y) const
{
	return &LineTop(y)[x*4];
}

unsigned char *YsBitmap::GetEditableRGBABitmapPointer(void)
{
	return GetEditableBitmapPointer();
}

unsigned char *YsBitmap::GetEditableRGBAPixelPointer(int x,int y)
{
	return &LineTop(y)[x*4];
}

unsigned YsBitmap::GetIntelUInt(const unsigned char dat[])
{
	return dat[0]+(unsigned(dat[1])<<8)+(unsigned(dat[2])<<16)+(unsigned(dat[3])<<24);
}

unsigned YsBitmap::GetIntelUShort(const unsigned char dat[])
{
	return dat[0]+(unsigned(dat[1])<<8);
}

const unsigned char *YsBitmap::GetTopOfLine(int y) const
{
	return LineTop(y);
}



/*
#define BI_RGB        0L
#define BI_RLE8       1L
#define BI_RLE4       2L
#define BI_BITFIELDS  3L

BITMAPFILEHEADER     14bytes
 2 WORD   "BM"
 4 DWORD  fileSize(#ofBytes/4)
 2 WORD   0
 2 WORD   0
 4 DWORD  bytes from BITMAPFILEHEADER to the bitmap (offset of bitmap)
BITMAPINFOHEADER     40bytes
 4 DWORD  sizeof(BITMAPINFOHEADER)
 4 DWORD  width
 4 DWORD  height  (Negative Number means Top-Down DIB)
 2 WORD   1
 2 WORD   bit count(24)
 4 DWORD  BI_RGB(Not compressed)
 4 DWORD  image size  0 is allowed for BI_RGB bitmap
 4 LONG   biXPelsPerMeter nearly equals to 2048
 4 LONG   biYPelsPerMeter nearly equals to 2048
 4 DWORD  biClrUsed  0 is allowed ?
 4 DWORD  biClrImportant  0 is allowed ?
bitmapdata
*/

static void PushWord(unsigned char *&ptr,short dat)
{
	ptr[0]=dat&0xff;
	ptr[1]=(dat>>8)&0xff;
	ptr+=2;
}

static void PushDWord(unsigned char *&ptr,int dat)
{
	ptr[0]=dat&0xff;
	ptr[1]=(dat>>8)&0xff;
	ptr[2]=(dat>>16)&0xff;
	ptr[3]=(dat>>24)&0xff;

	ptr+=4;
}

YSRESULT YsBitmap::SaveBmp(const char fn[]) const
{
	const unsigned int bitCount=24;
	int bytePerLine;
	bytePerLine=(GetWidth()*bitCount)/8;
	if(1>bytePerLine)
	{
		bytePerLine=1;
	}
	bytePerLine=(bytePerLine+3)/4;
	bytePerLine=bytePerLine*4;

	unsigned char bmiHeader[256],bmfHeader[256],*ptr;

	int bmiHeaderSize;
	bmiHeaderSize=40;    // Supposed to be 40 bytes.
	ptr=bmiHeader;
	PushDWord(ptr,bmiHeaderSize);    // bmiHeader.biSize=sizeof(bmiHeader);
	PushDWord(ptr,GetWidth());             // bmiHeader.biWidth=resX;
	PushDWord(ptr,GetHeight());             // bmiHeader.biHeight=resY;
	PushWord(ptr,1);                 // bmiHeader.biPlanes=1;
	PushWord(ptr,24);                // bmiHeader.biBitCount=24;
	PushDWord(ptr,0);                    // bmiHeader.biCompression=BI_RGB;
	PushDWord(ptr,0);                    // bmiHeader.biSizeImage=0;
	PushDWord(ptr,2048);                 // bmiHeader.biXPelsPerMeter=2048;
	PushDWord(ptr,2048);                 // bmiHeader.biYPelsPerMeter=2048;
	PushDWord(ptr,0);                    // bmiHeader.biClrUsed=0;
	PushDWord(ptr,0);                    // bmiHeader.biClrImportant=0;

	int bmfHeaderSize;
	bmfHeaderSize=14;
	ptr=bmfHeader;
	ptr[0]='B';
	ptr[1]='M';
	ptr+=2;
	PushDWord(ptr,bmfHeaderSize+bmiHeaderSize+bytePerLine*GetHeight());
	PushWord(ptr,0);
	PushWord(ptr,0);
	PushDWord(ptr,bmfHeaderSize+bmiHeaderSize);


	FILE *fp;
	int x,y;

	fp=fopen(fn,"wb");
	if(fp!=NULL)
	{
		unsigned char *line;
		line=new unsigned char [bytePerLine];

		fwrite(&bmfHeader,1,bmfHeaderSize,fp);
		fwrite(&bmiHeader,1,bmiHeaderSize,fp);

		for(y=0; y<GetHeight(); ++y)
		{
			for(x=0; x<(int)GetWidth(); x++)
			{
				line[x*3  ]=LineTop(y)[x*4+2];
				line[x*3+1]=LineTop(y)[x*4+1];
				line[x*3+2]=LineTop(y)[x*4  ];
			}

			for(int i=x*3; i<bytePerLine; ++i)
			{
				line[i]=0;
			}

			fwrite(line,1,bytePerLine,fp);
		}


		fclose(fp);

		delete [] line;

		return YSOK;
	}
	return YSERR;
}

YSRESULT YsBitmap::SaveBlackAndWhiteBmp(const char fn[],const int thr,YSBOOL invert) const
{
	unsigned char bmiHeader[256],bmfHeader[256],*ptr;

	const unsigned int bitCount=1;
	int bytePerLine=(GetWidth()*bitCount)/8;
	if(1>bytePerLine)
	{
		bytePerLine=1;
	}
	bytePerLine=(bytePerLine+3)/4;
	bytePerLine=bytePerLine*4;

	int bmiHeaderSize;
	bmiHeaderSize=40;    // Supposed to be 40 bytes.
	ptr=bmiHeader;
	PushDWord(ptr,bmiHeaderSize);    // bmiHeader.biSize=sizeof(bmiHeader);
	PushDWord(ptr,GetWidth());               // bmiHeader.biWidth=resX;
	PushDWord(ptr,GetHeight());               // bmiHeader.biHeight=resY;
	PushWord(ptr,1);                 // bmiHeader.biPlanes=1;
	PushWord(ptr,1);                 // bmiHeader.biBitCount=1;
	PushDWord(ptr,0);                    // bmiHeader.biCompression=BI_RGB;
	PushDWord(ptr,0);                    // bmiHeader.biSizeImage=0;
	PushDWord(ptr,2048);                 // bmiHeader.biXPelsPerMeter=2048;
	PushDWord(ptr,2048);                 // bmiHeader.biYPelsPerMeter=2048;
	PushDWord(ptr,0);                    // bmiHeader.biClrUsed=0;
	PushDWord(ptr,0);                    // bmiHeader.biClrImportant=0;

	int bmfHeaderSize;
	bmfHeaderSize=14;
	ptr=bmfHeader;
	ptr[0]='B';
	ptr[1]='M';
	ptr+=2;
	PushDWord(ptr,bmfHeaderSize+bmiHeaderSize+GetHeight()*bytePerLine);
	PushWord(ptr,0);
	PushWord(ptr,0);
	PushDWord(ptr,bmfHeaderSize+bmiHeaderSize);


	FILE *fp=fopen(fn,"wb");
	if(fp!=NULL)
	{
		unsigned char *line=new unsigned char [bytePerLine];

		fwrite(&bmfHeader,1,bmfHeaderSize,fp);
		fwrite(&bmiHeader,1,bmiHeaderSize,fp);

		const unsigned char colorMap[8]={0,0,0,0,255,255,255,255};
		fwrite(colorMap,1,8,fp);

		for(int y=0; y<GetHeight(); ++y)
		{
			for(int i=0; i<bytePerLine; ++i)
			{
				line[i]=0;
			}

			unsigned char bitShift=128,byteShift=0;
			for(int x=0; x<(int)GetWidth(); x++)
			{
				if(thr<=LineTop(y)[x*4+2] || thr<=LineTop(y)[x*4+1] || thr<=LineTop(y)[x*4  ])
				{
					line[byteShift]|=bitShift;
				}
				if(7==(x&7))
				{
					bitShift=128;
					++byteShift;
				}
				else
				{
					bitShift>>=1;
				}
			}

			if(YSTRUE==invert)
			{
				for(int i=0; i<bytePerLine; ++i)
				{
					line[i]=~line[0];
				}
			}

			fwrite(line,1,bytePerLine,fp);
		}


		fclose(fp);

		delete [] line;

		return YSOK;
	}
	return YSERR;
}

////////////////////////////////////////////////////////////

YSBOOL YsBitmap::operator==(const YsBitmap &bitmapB) const
{
	if(this->GetWidth()!=bitmapB.GetWidth() || this->GetHeight()!=bitmapB.GetHeight())
	{
		return YSFALSE;
	}
	auto numPix=GetWidth()*GetHeight();
	auto thisRgba=GetBitmapPointer();
	auto testRgba=bitmapB.GetBitmapPointer();
	for(int i=0; i<numPix*4; ++i)
	{
		if(thisRgba[i]!=testRgba[i])
		{
			return YSFALSE;
		}
	}
	return YSTRUE;
}

YSBOOL YsBitmap::operator!=(const YsBitmap &bitmapB) const
{
	if(this->GetWidth()!=bitmapB.GetWidth() || this->GetHeight()!=bitmapB.GetHeight())
	{
		return YSTRUE;
	}
	auto numPix=GetWidth()*GetHeight();
	auto thisRgba=GetBitmapPointer();
	auto testRgba=bitmapB.GetBitmapPointer();
	for(int i=0; i<numPix*4; ++i)
	{
		if(thisRgba[i]!=testRgba[i])
		{
			return YSTRUE;
		}
	}
	return YSFALSE;
}

