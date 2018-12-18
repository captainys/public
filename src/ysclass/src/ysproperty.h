/* ////////////////////////////////////////////////////////////

File Name: ysproperty.h
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

#ifndef YSPROPERTY_IS_INCLUDED
#define YSPROPERTY_IS_INCLUDED
/* { */

// Declaration /////////////////////////////////////////////
/*! A class for representing a RGBA color.
    */
class YsColor
{
public:
	/*! Default constructor.  It sets alpha component to non-transparent, but does not initialize R,G, and B components.
	    */
	inline YsColor(){col[3]=255;};

	/*! Constructor that takes RGB-component values in double precision.  Alpha component will be set to non-transparent.
	    */
	YsColor(double r,double g,double b);

	/*!  Constructor that takes RGBA-component values in double precision.
	    */
	YsColor(double r,double g,double b,double a);

	/*! Returns red component in double precision, scaling 0.0 to 1.0.
	    */
	inline double Rd(void) const
	{
		return double(col[0])/255.0;
	}

	/*! Returns green component in double precision, scaling 0.0 to 1.0.
	    */
	inline double Gd(void) const
	{
		return double(col[1])/255.0;
	}

	/*! Returns blue component in double precision, scaling 0.0 to 1.0.
	    */
	inline double Bd(void) const
	{
		return double(col[2])/255.0;
	}

	/*! Returns alpha component in double precision, scaling 0.0 to 1.0.
	    */
	inline double Ad(void) const
	{
		return double(col[3])/255.0;
	}

	/*! Returns red component in single precision, scaling 0.0 to 1.0.
	    */
	inline float Rf(void) const
	{
		return float(col[0])/255.0f;
	}
	/*! Returns green component in single precision, scaling 0.0 to 1.0.
	    */
	inline float Gf(void) const
	{
		return float(col[1])/255.0f;
	}
	/*! Returns blue component in single precision, scaling 0.0 to 1.0.
	    */
	inline float Bf(void) const
	{
		return float(col[2])/255.0f;
	}
	/*! Returns alpha component in single precision, scaling 0.0 to 1.0.
	    */
	inline float Af(void) const
	{
		return float(col[3])/255.0f;
	}

	/*! Returns red component in integer, scaling 0 to 255.
	    */
	inline int Ri(void) const
	{
		return int(col[0]);
	}
	/*! Returns green component in integer, scaling 0 to 255.
	    */
	inline int Gi(void) const
	{
		return int(col[1]);
	}
	/*! Returns blue component in integer, scaling 0 to 255.
	    */
	inline int Bi(void) const
	{
		return int(col[2]);
	}
	/*! Returns alpha component in integer, scaling 0 to 255.
	    */
	inline int Ai(void) const
	{
		return int(col[3]);
	}

	/*! Returns red component in unsigned char, scaling 0 to 255.
	    */
	inline unsigned char Ruc(void) const
	{
		return col[0];
	}
	/*! Returns green component in unsigned char, scaling 0 to 255.
	    */
	inline unsigned char Guc(void) const
	{
		return col[1];
	}
	/*! Returns blue component in unsigned char, scaling 0 to 255.
	    */
	inline unsigned char Buc(void) const
	{
		return col[2];
	}
	/*! Returns alpha component in unsigned char, scaling 0 to 255.
	    */
	inline unsigned char Auc(void) const
	{
		return col[3];
	}


	/*! Sets 15 bit color.  Blue for 0-4 bits, Red for 5-9 bits, and Green for 10-14 bits.    Alpha component will be made solid (non-transparent).
	    */
	void Set15BitRGB(int c15bit);
	/*! Sets RGB components by integer values, scaling 0 to 255.  Alpha component will be made solid (non-transparent).
	    */
	void SetIntRGB(int r,int g,int b);
	/*! Sets RGB components by double-precision values, scaling 0.0 to 1.0.    Alpha component will be made solid (non-transparent).
	    */
	void SetDoubleRGB(double r,double g,double b);
	/*! Sets RGB components by floating-point values, scaling 0 to 255.    Alpha component will be made solid (non-transparent).
	    */
	void SetFloatRGB(float r,float g,float b);
	/*! Sets RGBA components by integer values, scaling 0 to 255.
	    */
	void SetIntRGBA(int r,int g,int b,int a);
	/*! Sets RGBA components by double-precision values, scaling 0.0 to 1.0.
	    */
	void SetDoubleRGBA(double r,double g,double b,double a);
	/*! Sets RGBA components by single-precision values, scaling 0.0 to 1.0.
	    */
	void SetFloatRGBA(float r,float g,float b,float a);
	/*! Sets RGB components by gradation, blue for t=0.0, green for t=0.5, and red for t=1.0.
	    */
	void SetRainbowColor(const double &t);

	/*! Set RGB components by HSV.  Alpha component will be set to 1.0. */
	void SetDoubleHSV(const double h,const double s,const double v);

	/*! Set color by HSV and Alpha. */
	void SetDoubleHSVA(const double h,const double s,const double v,const double a);

	/*! Get Hue component. */
	const double Hd(void) const;

	/*! Get Saturation component. */
	const double Sd(void) const;

	/*! Get Value component. */
	const double Vd(void) const;

	/*! Get Hue component. */
	const int Hi(void) const;

	/*! Get Saturation component. */
	const int Si(void) const;

	/*! Get Value component. */
	const int Vi(void) const;

	/*! Sets red component, scaling 0 to 255. */
	inline void SetRi(int r)
	{
		col[0]=(unsigned char)r;
	}
	/*! Sets green component, scaling 0 to 255. */
	inline void SetGi(int g)
	{
		col[1]=(unsigned char)g;
	}
	/*! Sets blue component, scaling 0 to 255. */
	inline void SetBi(int b)
	{
		col[2]=(unsigned char)b;
	}
	/*! Sets alpha component, scaling 0 to 255. */
	inline void SetAi(int a)
	{
		col[3]=(unsigned char)a;
	}
	/*! Sets red component, scaling 0.0 to 1.0. */
	inline void SetRf(float r)
	{
		col[0]=(unsigned char)(YsBound <unsigned int> (unsigned(r*255.0f),0,255));
	}
	/*! Sets green component, scaling 0.0 to 1.0. */
	inline void SetGf(float g)
	{
		col[1]=(unsigned char)(YsBound <unsigned int> (unsigned(g*255.0f),0,255));
	}
	/*! Sets blue component, scaling 0.0 to 1.0. */
	inline void SetBf(float b)
	{
		col[2]=(unsigned char)(YsBound <unsigned int> (unsigned(b*255.0f),0,255));
	}
	/*! Sets alpha component, scaling 0.0 to 1.0. */
	inline void SetAf(float a)
	{
		col[3]=(unsigned char)(YsBound <unsigned int> (unsigned(a*255.0f),0,255));
	}
	/*! Sets red component, scaling 0.0 to 1.0. */
	inline void SetRd(double r)
	{
		col[0]=(unsigned char)(YsBound <unsigned int> (unsigned(r*255.0),0,255));
	}
	/*! Sets green component, scaling 0.0 to 1.0. */
	inline void SetGd(double g)
	{
		col[1]=(unsigned char)(YsBound <unsigned int> (unsigned(g*255.0),0,255));
	}
	/*! Sets blue component, scaling 0.0 to 1.0. */
	inline void SetBd(double b)
	{
		col[2]=(unsigned char)(YsBound <unsigned int> (unsigned(b*255.0),0,255));
	}
	/*! Sets alpha component, scaling 0.0 to 1.0. */
	inline void SetAd(double a)
	{
		col[3]=(unsigned char)(YsBound <unsigned int> (unsigned(a*255.0),0,255));
	}


	/*! Returns 15-bit color.
	    */
	int Get15BitColor(void) const;
	/*! Returns RGB components in integer, scaling 0 to 255.
	    */
	void GetIntRGB(int &r,int &g,int &b) const;
	/*! Returns RGB components in double-precision, scaling 0.0 to 1.0.
	    */
	void GetDoubleRGB(double &r,double &g,double &b) const;

	/*! Returns RGBA components. */
	inline void GetRGBA(int c[4]) const
	{
		c[0]=Ri();
		c[1]=Gi();
		c[2]=Bi();
		c[3]=Ai();
	}

	/*! Returns RGBA components. */
	void GetRGBA(unsigned char c[4]) const
	{
		c[0]=Ruc();
		c[1]=Guc();
		c[2]=Buc();
		c[3]=Auc();
	}

	/*! Returns RGBA components. */
	void GetRGBA(float c[4]) const
	{
		c[0]=Rf();
		c[1]=Gf();
		c[2]=Bf();
		c[3]=Af();
	}

	/*! Returns RGBA components. */
	void GetRGBA(double c[4]) const
	{
		c[0]=Rd();
		c[1]=Gd();
		c[2]=Bd();
		c[3]=Ad();
	}

	/*! Set by RRGGBB string.  It can also be RRGGBBAA. */
	void SetByString(const char str[])
	{
		int component=0;
		col[3]=255;
		for(int i=0; i<8 && 0!=str[i]; ++i)
		{
			component*=16;

			auto c=str[i];
			if('a'<=c && c<='f')
			{
				component+=(10+c-'a');
			}
			else if('A'<=c && c<='F')
			{
				component+=(10+c-'A');
			}
			else if('0'<=c && c<='9')
			{
				component+=(c-'0');
			}

			if(1==i%2)
			{
				col[i/2]=component;
			}
		}
	}

protected:
	unsigned char col[4];
};

inline int operator==(const YsColor &col1,const YsColor &col2)
{
	return (col1.Ri()==col2.Ri() && col1.Gi()==col2.Gi() && col1.Bi()==col2.Bi() && col1.Ai()==col2.Ai());
}

inline int operator!=(const YsColor &col1,const YsColor &col2)
{
	return (col1.Ri()!=col2.Ri() || col1.Gi()!=col2.Gi() || col1.Bi()!=col2.Bi() ||  col1.Ai()!=col2.Ai());
}

/* } */
#endif
