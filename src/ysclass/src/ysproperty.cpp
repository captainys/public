/* ////////////////////////////////////////////////////////////

File Name: ysproperty.cpp
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

#include "ysmath.h"
#include "ysproperty.h"

// Implementation //////////////////////////////////////////
YsColor::YsColor(double rr,double gg,double bb)
{
	col[0]=(unsigned char)(YsBound <unsigned int> (unsigned(rr*255.0),0,255));
	col[1]=(unsigned char)(YsBound <unsigned int> (unsigned(gg*255.0),0,255));
	col[2]=(unsigned char)(YsBound <unsigned int> (unsigned(bb*255.0),0,255));
	col[3]=255;
}

YsColor::YsColor(double r,double g,double b,double a)
{
	col[0]=(unsigned char)(YsBound <unsigned int> (unsigned(r*255.0),0,255));
	col[1]=(unsigned char)(YsBound <unsigned int> (unsigned(g*255.0),0,255));
	col[2]=(unsigned char)(YsBound <unsigned int> (unsigned(b*255.0),0,255));
	col[3]=(unsigned char)(YsBound <unsigned int> (unsigned(a*255.0),0,255));
}

void YsColor::Set15BitRGB(int c15bit)
{
	unsigned c;
	c=c15bit & 32767;
	col[1]=(unsigned char)(((c>>10)&31)*255/31);
	col[0]=(unsigned char)(((c>> 5)&31)*255/31);
	col[2]=(unsigned char)(((c    )&31)*255/31);
	col[3]=255;
}

void YsColor::SetIntRGB(int rr,int gg,int bb)
{
	col[0]=(unsigned char)(YsBound(rr,0,255));
	col[1]=(unsigned char)(YsBound(gg,0,255));
	col[2]=(unsigned char)(YsBound(bb,0,255));
	col[3]=255;
}

void YsColor::SetDoubleRGB(double rr,double gg,double bb)
{
	col[0]=(unsigned char)(YsBound <unsigned int> (unsigned(rr*255.0),0,255));
	col[1]=(unsigned char)(YsBound <unsigned int> (unsigned(gg*255.0),0,255));
	col[2]=(unsigned char)(YsBound <unsigned int> (unsigned(bb*255.0),0,255));
	col[3]=255;
}

void YsColor::SetFloatRGB(float rr,float gg,float bb)
{
	col[0]=(unsigned char)(YsBound <unsigned int> (unsigned(rr*255.0f),0,255));
	col[1]=(unsigned char)(YsBound <unsigned int> (unsigned(gg*255.0f),0,255));
	col[2]=(unsigned char)(YsBound <unsigned int> (unsigned(bb*255.0f),0,255));
	col[3]=255;
}

void YsColor::SetIntRGBA(int rr,int gg,int bb,int aa)
{
	col[0]=(unsigned char)(YsBound(rr,0,255));
	col[1]=(unsigned char)(YsBound(gg,0,255));
	col[2]=(unsigned char)(YsBound(bb,0,255));
	col[3]=(unsigned char)(YsBound(aa,0,255));
}

void YsColor::SetDoubleRGBA(double rr,double gg,double bb,double aa)
{
	col[0]=(unsigned char)(YsBound <unsigned int> (unsigned(rr*255.0),0,255));
	col[1]=(unsigned char)(YsBound <unsigned int> (unsigned(gg*255.0),0,255));
	col[2]=(unsigned char)(YsBound <unsigned int> (unsigned(bb*255.0),0,255));
	col[3]=(unsigned char)(YsBound <unsigned int> (unsigned(aa*255.0),0,255));
}

void YsColor::SetFloatRGBA(float rr,float gg,float bb,float aa)
{
	col[0]=(unsigned char)(YsBound <unsigned int> (unsigned(rr*255.0f),0,255));
	col[1]=(unsigned char)(YsBound <unsigned int> (unsigned(gg*255.0f),0,255));
	col[2]=(unsigned char)(YsBound <unsigned int> (unsigned(bb*255.0f),0,255));
	col[3]=(unsigned char)(YsBound <unsigned int> (unsigned(aa*255.0f),0,255));
}

void YsColor::SetRainbowColor(const double &t)
{
	if(t<0.0)
	{
		SetIntRGB(0,0,255);
	}
	else if(t<0.25)
	{
		const double inten=t/0.25;
		SetDoubleRGB(0.0,inten,1.0);
	}
	else if(t<0.5)
	{
		const double inten=(t-0.25)/0.25;
		SetDoubleRGB(0.0,1.0,1.0-inten);
	}
	else if(t<0.75)
	{
		const double inten=(t-0.5)/0.25;
		SetDoubleRGB(inten,1.0,0.0);
	}
	else if(t<1.0)
	{
		const double inten=(t-0.75)/0.25;
		SetDoubleRGB(1.0,1.0-inten,0.0);
	}
	else
	{
		SetIntRGB(255,0,0);
	}
}

void YsColor::SetDoubleHSV(const double h,const double s,const double v)
{
	SetDoubleHSVA(h,s,v,1.0);
}

void YsColor::SetDoubleHSVA(const double h,const double s,const double v,const double a)
{
	// 0.0      1/6      2/6      3/6      4/6      5/6      1.0
	// Red    ->Yellow ->Green  ->Cyan   ->Blue   ->Magenta->Red
	// (1,0,0)->(1,1,0)->(0,1,0)->(0,1,1)->(0,0,1)->(1,0,1)->(1,0,0)

	const double sixH=YsBound(h*6.0,0.0,6.0);
	double r,g,b;
	if(1.0>sixH)
	{
		const double t=sixH;
		r=1.0;
		g=t;
		b=0.0;
	}
	else if(2.0>sixH)
	{
		const double t=2.0-sixH;
		r=t;
		g=1.0;
		b=0.0;
	}
	else if(3.0>sixH)
	{
		const double t=sixH-2.0;
		r=0.0;
		g=1.0;
		b=t;
	}
	else if(4.0>sixH)
	{
		const double t=4.0-sixH;
		r=0.0;
		g=t;
		b=1.0;
	}
	else if(5.0>sixH)
	{
		const double t=sixH-4.0;
		r=t;
		g=0.0;
		b=1.0;
	}
	else
	{
		const double t=6.0-sixH;
		r=1.0;
		g=0.0;
		b=t;
	}


	r=(r*s+(1.0-s))*v;
	g=(g*s+(1.0-s))*v;
	b=(b*s+(1.0-s))*v;

	SetDoubleRGBA(r,g,b,a);
}

const double YsColor::Hd(void) const
{
	const double max=YsGreatestOf(Rd(),Gd(),Bd());
	const double min=YsSmallestOf(Rd(),Gd(),Bd());

	if(YsTolerance>max-min)
	{
		return 0.0;
	}

	const double r=(Rd()-min)/(max-min);
	const double g=(Gd()-min)/(max-min);
	const double b=(Bd()-min)/(max-min);

	double sixH=0.0;
	if(g<r && b<r) // Red=1.0 
	{
		if(b<g)
		{
			sixH=g;
		}
		else
		{
			sixH=6.0-b;
		}
	}
	else if(b<g) // Green=1.0
	{
		if(b<r)
		{
			sixH=2.0-r;
		}
		else
		{
			sixH=b+2.0;
		}
	}
	else // Blue=1.0
	{
		if(r<g)
		{
			sixH=4.0-g;
		}
		else
		{
			sixH=r+4.0;
		}
	}

	return YsBound(sixH/6.0,0.0,1.0);
}

const double YsColor::Sd(void) const
{
	const double max=YsGreatestOf(Rd(),Gd(),Bd());
	const double min=YsSmallestOf(Rd(),Gd(),Bd());
	// S=1.0 -> min==0.0
	// S=0.0 -> min==max
	if(YsTolerance>max)
	{
		return 0.0;
	}
	else
	{
		return 1.0-(min/max);
	}
}

const double YsColor::Vd(void) const
{
	return YsGreatestOf(Rd(),Gd(),Bd());
}

const int YsColor::Hi(void) const
{
	return (int)(Hd()*255.0);
}

const int YsColor::Si(void) const
{
	return (int)(Sd()*255.0);
}

const int YsColor::Vi(void) const
{
	return (int)(Vd()*255.0);
}

int YsColor::Get15BitColor(void) const
{
	int g,r,b;
	r=int(col[0]);
	g=int(col[1]);
	b=int(col[2]);

	r=YsBound((r+4)/8,0,31);
	g=YsBound((g+4)/8,0,31);
	b=YsBound((b+4)/8,0,31);

	return ((g<<10)+(r<<5)+b);
}

void YsColor::GetIntRGB(int &rr,int &gg,int &bb) const
{
	rr=int(col[0]);
	gg=int(col[1]);
	bb=int(col[2]);
}

void YsColor::GetDoubleRGB(double &rr,double &gg,double &bb) const
{
	rr=double(col[0])/255.0;
	gg=double(col[1])/255.0;
	bb=double(col[2])/255.0;
}

