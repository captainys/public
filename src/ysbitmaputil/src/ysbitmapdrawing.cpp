/* ////////////////////////////////////////////////////////////

File Name: ysbitmapdrawing.cpp
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

#include "ysbitmaputil.h"

YsBitmapDrawing::YsBitmapDrawing(YsBitmap &bmp)
{
	bmpPtr=&bmp;
	plgItscBuf.Set(bmp.GetHeight()*2,nullptr);
}

void YsBitmapDrawing::Clear(const unsigned char rgba[])
{
	bmpPtr->Clear(rgba[0],rgba[1],rgba[2],rgba[3]);
}

void YsBitmapDrawing::DrawLine(int x0,int y0,int x1,int y1,const LineOption &option)
{
	if(1>=option.thickness)
	{
		DrawLine(x0,y0,x1,y1,option.rgba);
	}
	else
	{
		const int t=(int)option.thickness;
		const int r=(option.thickness+1)/2;

		DrawCircle(x0,y0,r,option.rgba,YSTRUE);
		DrawCircle(x1,y1,r,option.rgba,YSTRUE);

		const int dx=x1-x0;
		const int dy=y1-y0;

		int nx=-dy;
		int ny= dx;

		const int d=(int)(sqrt((double)(nx*nx+ny*ny)));
		nx=(nx*t)/(2*d);
		ny=(ny*t)/(2*d);
		int quad[8]=
		{
			x0+nx,y0+ny,
			x0-nx,y0-ny,
			x1-nx,y1-ny,
			x1+nx,y1+ny
		};
		DrawPolygon(4,quad,option.rgba);
	}
}

void YsBitmapDrawing::DrawLine(int x0,int y0,int x1,int y1,const unsigned char rgba[])
{
	auto &bmp=*bmpPtr;

	if(0<=x0 && x0<bmp.GetWidth() && 0<=y0 && y0<bmp.GetHeight() && 
	   0<=x1 && x1<bmp.GetWidth() && 0<=y1 && y1<bmp.GetHeight())
	{
		DrawLineNoClip(x0,y0,x1,y1,rgba);
	}
	else
	{
		YsVec2 l1(x0,y0);
		YsVec2 l2(x1,y1);

		YsVec2 r1(0.0f,0.0f);
		YsVec2 r2(bmp.GetWidth()-1,bmp.GetHeight()-1);

		YsVec2 c1,c2;
		if(YSOK==YsClipLineSeg2(c1,c2,l1,l2,r1,r2))
		{
			DrawLineNoClip((int)c1.x(),(int)c1.y(),(int)c2.x(),(int)c2.y(),rgba);
		}
	}
}

void YsBitmapDrawing::DrawLineNoClip(int x0,int y0,int x1,int y1,const unsigned char rgba[])
{
	// auto &bmp=*bmpPtr;

	if(x0==x1)
	{
		if(y1>y0)
		{
			YsSwapSomething(y0,y1);
		}
		for(auto y=y0; y<y1; ++y)
		{
			SetPixel(x0,y,rgba);
		}
	}
	else if(y0==y1)
	{
		if(x1>x0)
		{
			YsSwapSomething(x0,x1);
		}
		for(auto x=x0; x<x1; ++x)
		{
			SetPixel(x,y0,rgba);
		}
	}
	else
	{
		auto dx=x1-x0;
		auto dy=y1-y0;

		auto adx=YsAbs(dx);
		auto ady=YsAbs(dy);

		// decltype(x0) du[2]={0,0};
		// decltype(y0) dv[2]={0,0};

		if(adx>ady)
		{
			auto u0=x0;
			auto v0=y0;
			auto u1=x1;
			auto v1=y1;

			if(0>dx)
			{
				YsSwapSomething(u0,u1);
				YsSwapSomething(v0,v1);
				dx=-dx;
				dy=-dy;
			}

			auto vy=dy/ady;

			int f=0;
			auto u=u0;
			auto v=v0;
			auto du=adx;
			auto dv=ady;
			for(;;)
			{
				SetPixel(u,v,rgba);

				// v=(dv/du)*u+C
				// du*v=dv*u+D
				// D=du*v-dv*u
				// f=du*v-dv*u

				if(0<f)
				{
					u++;
					f-=dv;
				}
				else
				{
					v+=vy;
					f+=du;
				}

				if(u==u1 && v==v1)
				{
					break;
				}
			}
		}
		else // if(ady>adx)
		{
			auto u0=y0;
			auto v0=x0;
			auto u1=y1;
			auto v1=x1;

			if(0>dy)
			{
				YsSwapSomething(u0,u1);
				YsSwapSomething(v0,v1);
				dy=-dy;
				dx=-dx;
			}

			auto vx=dx/adx;

			int f=0;
			auto u=u0;
			auto v=v0;
			auto du=ady;
			auto dv=adx;
			for(;;)
			{
				SetPixel(v,u,rgba);

				if(0<f)
				{
					u++;
					f-=dv;
				}
				else
				{
					v+=vx;
					f+=du;
				}

				if(u==u1 && v==v1)
				{
					break;
				}
			}
		}
	}
}

void YsBitmapDrawing::DrawPolygon(YSSIZE_T n,const int xy[],const unsigned char rgba[])
{
	int prevVy=GetPolygonLastVy(n,xy);
	if(0==prevVy)
	{
		return;
	}

	CleanUpPolygonLineIntersection();
	for(int i=0; i<n; ++i)
	{
		int j=(i+1)%n;
		AddPolygonLineIntersectionLine(xy[i*2],xy[i*2+1],xy[j*2],xy[j*2+1],prevVy);
	}
	FillPolygonLineIntersection(rgba);
}

void YsBitmapDrawing::DrawCircle(int x,int y,int r,const unsigned char rgba[],YSBOOL fill,int nDiv)
{
	if(x+r<0 || bmpPtr->GetWidth()<x-r)
	{
		return;
	}

	if(YSTRUE!=fill)
	{
		nDiv=(nDiv+3)/4;
		double cPrev=cos(0.0);
		double sPrev=sin(0.0);
		for(int i=1; i<=nDiv; ++i)
		{
			const double angle=YsPi*0.5*(double)i/(double)nDiv;
			const double c=cos(angle);
			const double s=sin(angle);

			int vxPrev=(int)(cPrev*(double)r);
			int vyPrev=(int)(sPrev*(double)r);

			int vx=(int)(c*(double)r);
			int vy=(int)(s*(double)r);

			int x0=x+vxPrev;
			int y0=y+vyPrev;
			int x1=x+vx;
			int y1=y+vy;
			DrawLine(x0,y0,x1,y1,rgba);

			x0=x-vxPrev;
			y0=y+vyPrev;
			x1=x-vx;
			y1=y+vy;
			DrawLine(x0,y0,x1,y1,rgba);

			x0=x+vxPrev;
			y0=y-vyPrev;
			x1=x+vx;
			y1=y-vy;
			DrawLine(x0,y0,x1,y1,rgba);

			x0=x-vxPrev;
			y0=y-vyPrev;
			x1=x-vx;
			y1=y-vy;
			DrawLine(x0,y0,x1,y1,rgba);

			cPrev=c;
			sPrev=s;
		}
	}
	else
	{
		CleanUpPolygonLineIntersection();

		nDiv=(nDiv+3)/4;
		double cPrev=cos(0.0);
		double sPrev=sin(0.0);
		for(int i=1; i<=nDiv; ++i)
		{
			const double angle=YsPi*0.5*(double)i/(double)nDiv;
			const double c=cos(angle);
			const double s=sin(angle);

			int vxPrev=(int)(cPrev*(double)r);
			int vyPrev=(int)(sPrev*(double)r);

			int vx=(int)(c*(double)r);
			int vy=(int)(s*(double)r);

			int x0=x+vxPrev,y0=y+vyPrev;
			int x1=x+vx,    y1=y+vy;

			int prevVy=y1-y0;   // Technically, these two AddPolygonLineIntersection calls are not connected.  It is connected to the segment for the next i.
			AddPolygonLineIntersectionLine(x0,y0,x1,y1,prevVy);

			x0=x+vx;    y0=y-vy;
			x1=x+vxPrev;y1=y-vyPrev;
			prevVy=y1-y0;
			AddPolygonLineIntersectionLine(x0,y0,x1,y1,prevVy);

			cPrev=c;
			sPrev=s;
		}

		MakePolygonLineIntersectionBufferForXSymmetric(x);
		FillPolygonLineIntersection(rgba);
	}
}


void YsBitmapDrawing::CleanUpPolygonLineIntersection(void)
{
	minPlgItscY=bmpPtr->GetHeight();
	maxPlgItscY=0;
	for(auto &v : plgItscBuf)
	{
		v=-1;
	}
}

void YsBitmapDrawing::ErasePolygonLineIntersectionForOneY(int y)
{
	if(0<=y && y<plgItscBuf.GetN()/2)
	{
		plgItscBuf[y*2  ]=-1;
		plgItscBuf[y*2+1]=-1;
	}
}

void YsBitmapDrawing::MakePolygonLineIntersectionBufferForXSymmetric(int xSymm)
{
	for(int y=minPlgItscY; y<=maxPlgItscY; ++y)
	{
		auto x0=plgItscBuf[y*2];
		auto x1=xSymm+(xSymm-x0);

		if(x0<=x1)
		{
			plgItscBuf[y*2+1]=x1;
		}
		else
		{
			plgItscBuf[y*2  ]=x1;
			plgItscBuf[y*2+1]=x0;
		}
	}
}

void YsBitmapDrawing::FillPolygonLineIntersection(const unsigned char rgba[4])
{
	for(int y=minPlgItscY; y<=maxPlgItscY; ++y)
	{
		auto x0=plgItscBuf[y*2];
		auto x1=plgItscBuf[y*2+1];

		if(x1<0 || bmpPtr->GetWidth()<x0 || (x0==0 && x1==0) || (x0==bmpPtr->GetWidth()-1 && x1==bmpPtr->GetWidth()-1))
		{
			continue;
		}

		YsMakeGreater(x0,0);
		YsMakeSmaller(x1,bmpPtr->GetWidth()-1);
		for(int x=x0; x<=x1; ++x)
		{
			SetPixel(x,y,rgba);
		}
	}
}

int YsBitmapDrawing::GetPolygonLastVy(YSSIZE_T n,const int xy[]) const
{
	for(auto i=n-1; 0<i; --i)
	{
		const auto j=(i+1)%n;
		const int y0=xy[i*2+1];
		const int y1=xy[j*2+1];

		if(y0==y1)
		{
			continue;
		}
		if((y0<0 && y1<0) || (bmpPtr->GetHeight()<=y0 && bmpPtr->GetHeight()<=y1))
		{
			continue;
		}

		return y1-y0;
	}
	return 0;
}

void YsBitmapDrawing::AddPolygonLineIntersectionLine(int x0,int y0,int x1,int y1,int &prevVy)
{
	auto &bmp=*bmpPtr;
	if((y0<0 && y1<0) || (bmp.GetHeight()<=y0 && bmp.GetHeight()<=y1))
	{
		return;
	}

	{
		// Y-Clip
		int X0=x0,Y0=y0,X1=x1,Y1=y1;
		if(Y0<0)
		{
			Y0=0;
			X0=(x1-x0)*(Y0-y0)/(y1-y0)+x0;
		}
		if(bmp.GetHeight()<=Y0)
		{
			Y0=bmp.GetHeight()-1;
			X0=(x1-x0)*(Y0-y0)/(y1-y0)+x0;
		}
		if(Y1<0)
		{
			Y1=0;
			X1=(x1-x0)*(Y1-y0)/(y1-y0)+x0;
		}
		if(bmp.GetHeight()<=Y0)
		{
			Y1=bmp.GetHeight()-1;
			X1=(x1-x0)*(Y1-y0)/(y1-y0)+x0;
		}

		x0=X0;
		y0=Y0;
		x1=X1;
		y1=Y1;
	}


	if(y0==y1)
	{
		return;
	}

	if(x0==x1)
	{
		auto dy=y1-y0;
		auto ady=YsAbs(dy);
		auto vy=dy/ady;

		if(prevVy*dy<0.0)
		{
			AddPolygonIntersection(x0,y0);
		}
		for(auto y=y0; y!=y1; y+=vy)
		{
			AddPolygonIntersection(x0,y+vy);
		}

		prevVy=vy;
	}
	else
	{
		auto dx=x1-x0;
		auto dy=y1-y0;

		auto adx=YsAbs(dx);
		auto ady=YsAbs(dy);

		auto vx=dx/adx;
		auto vy=dy/ady;

		if(prevVy*dy<0.0)
		{
			AddPolygonIntersection(x0,y0);
		}

		auto x=x0;
		decltype(x0) f=0;
		for(auto y=y0; y!=y1; y+=vy)
		{
			// D=adx*y+ady*x

			f+=adx;
			if(ady<=f)
			{
				decltype(x) mvx=f/ady;
				x+=vx*mvx;
				f-=ady*mvx;
			}

			AddPolygonIntersection(x,y+vy);
		}

		prevVy=vy;
	}
}
