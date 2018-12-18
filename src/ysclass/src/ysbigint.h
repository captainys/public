/* ////////////////////////////////////////////////////////////

File Name: ysbigint.h
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

#ifndef YSBIGINT_IS_INCLUDED
#define YSBIGINT_IS_INCLUDED
/* { */


class YsBigInt
{
public:
	enum
	{
		INTHIGHBIT=(int)((unsigned int)((unsigned int)(~0)&(~(((unsigned int)(~0))>>1)))>>1),
		UINTHIGHBIT=((unsigned int)((unsigned int)(~0)&(~(((unsigned int)(~0))>>1))))
	};


	YsBigInt();
	YsBigInt(int x);
	YsBigInt(unsigned int x);
	int upper;
	unsigned int lower;

	inline const YsBigInt &operator+=(const YsBigInt &a);
	inline const YsBigInt &operator-=(const YsBigInt &a);
	inline const YsBigInt &operator*=(const YsBigInt &a);
	inline const YsBigInt &operator/=(const YsBigInt &a);
	inline const YsBigInt &operator>>=(int n);
	inline const YsBigInt &operator<<=(int n);

	inline const YsBigInt &Atoi(const char str[]);
	inline const char *Itoa(char str[],int nBufLen) const;
	inline const YsBigInt &Negate(void);
	inline const YsBigInt &Multiply(YsBigInt &mul,const YsBigInt &a,const YsBigInt &b);
	inline const YsBigInt &Divide(YsBigInt &div,YsBigInt &mod,const YsBigInt &denom);

protected:
	inline void InduceDivisionByZeroException(void) const;
};



inline YsBigInt::YsBigInt()
{
}

inline YsBigInt::YsBigInt(int x)
{
	upper=(x>=0 ? 0 : ~0);
	lower=(unsigned)x;
}

inline YsBigInt::YsBigInt(unsigned int x)
{
	upper=0;
	lower=x;
}

inline const YsBigInt &YsBigInt::operator+=(const YsBigInt &a)
{
	unsigned prevLower;
	prevLower=lower;
	lower+=a.lower;
	upper+=a.upper;
	if(lower<prevLower && lower<a.lower)  // Overflow?
	{
		upper++;
	}
	return *this;
}

inline const YsBigInt &YsBigInt::operator-=(const YsBigInt &a)
{
	if(a.lower>lower) // Underflow
	{
		upper--;
	}
	lower-=a.lower;
	upper-=a.upper;
	return *this;
}

inline const YsBigInt &YsBigInt::operator*=(const YsBigInt &a)
{
	YsBigInt c;
	Multiply(c,*this,a);
	*this=c;
	return *this;
}

inline const YsBigInt &YsBigInt::operator/=(const YsBigInt &a)
{
	YsBigInt div,mod;
	Divide(div,mod,a);
	*this=div;
	return *this;
}

inline const YsBigInt &YsBigInt::operator>>=(int n)
{
	if(n<0)
	{
		return (*this)<<=n;
	}
	else if(n>0)
	{
		int i;
		unsigned orBit,mask;
		orBit=0;
		mask=1;

		for(i=0; i<n; i++)
		{
			orBit>>=1;
			if(upper&mask)
			{
				orBit|=UINTHIGHBIT;
			}
			mask<<=1;
		}

		lower>>=n;
		lower|=orBit;
		upper>>=n;
	}
	return *this;
}

inline const YsBigInt &YsBigInt::operator<<=(int n)
{
	if(n<0)
	{
		return (*this)>>=n;
	}
	else if(n>0)
	{
		int i;
		unsigned orBit,mask;
		orBit=0;
		mask=UINTHIGHBIT;

		for(i=0; i<n; i++)
		{
			orBit<<=1;
			if(lower&mask)
			{
				orBit|=1;
			}
			mask>>=1;
		}

		lower<<=n;
		upper<<=n;
		upper|=orBit;
	}
	return *this;
}

inline YsBigInt operator+(const YsBigInt &a,const YsBigInt &b)
{
	YsBigInt x;
	x.lower=a.lower+b.lower;
	x.upper=a.upper+b.upper;
	if(x.lower<a.lower && x.lower<b.lower) // Overflow?
	{
		x.upper++;
	}
	return x;
}

inline const YsBigInt &operator+(const YsBigInt &a)
{
	return a;
}

inline YsBigInt operator-(const YsBigInt &a,const YsBigInt &b)
{
	YsBigInt x;
	x.lower=a.lower-b.lower;
	x.upper=a.upper-b.upper;
	if(x.lower>a.lower && x.lower>b.lower) // Underflow?
	{
		x.upper--;
	}
	return x;
}

inline YsBigInt operator*(const YsBigInt &a,const YsBigInt &b)
{
	YsBigInt c;
	c=a;
	c*=b;
	return c;
}

inline YsBigInt operator/(const YsBigInt &a,const YsBigInt &b)
{
	YsBigInt c;
	c=a;
	c/=b;
	return c;
}

inline YsBigInt operator-(const YsBigInt &a)
{
	YsBigInt x;
	unsigned *lowbit;
	x=a;
	lowbit=(unsigned *)&x.lower;
	(*lowbit)=~(*lowbit);
	x.upper=~x.upper;
	x.lower++;
	if(x.lower==0)
	{
		x.upper++;
	}
	return x;
}

inline YsBigInt operator<<(const YsBigInt &a,int n)
{
	YsBigInt x;
	x=a;
	x<<=n;
	return x;
}

inline YsBigInt operator>>(const YsBigInt &a,int n)
{
	YsBigInt x;
	x=a;
	x>>=n;
	return x;
}



inline int operator==(const YsBigInt &a,const YsBigInt &b)
{
	return (a.lower==b.lower && a.upper==b.upper);
}

inline int operator!=(const YsBigInt &a,const YsBigInt &b)
{
	return (a.lower!=b.lower || a.upper!=b.upper);
}

inline int operator>(const YsBigInt &a,const YsBigInt &b)
{
	if(a.upper!=b.upper)
	{
		return a.upper>b.upper;
	}
	else
	{
		return a.lower>b.lower;
	}
}

inline int operator<(const YsBigInt &a,const YsBigInt &b)
{
	if(a.upper!=b.upper)
	{
		return a.upper<b.upper;
	}
	else
	{
		return a.lower<b.lower;
	}
}

inline int operator<=(const YsBigInt &a,const YsBigInt &b)
{
	if(a.upper==b.upper)
	{
		return a.lower<=b.lower;
	}
	else
	{
		return a.upper<b.upper;
	}
}

inline int operator>=(const YsBigInt &a,const YsBigInt &b)
{
	if(a.upper==b.upper)
	{
		return a.lower>=b.lower;
	}
	else
	{
		return a.upper>b.upper;
	}
}

inline YsBigInt operator&(const YsBigInt &a,const YsBigInt &b)
{
	YsBigInt c;
	c.upper=(a.upper&b.upper);
	c.lower=(a.lower&b.lower);
	return c;
}

inline YsBigInt operator|(const YsBigInt &a,const YsBigInt &b)
{
	YsBigInt c;
	c.upper=(a.upper|b.upper);
	c.lower=(a.lower|b.lower);
	return c;
}

inline YsBigInt operator^(const YsBigInt &a,const YsBigInt &b)
{
	YsBigInt c;
	c.upper=(a.upper^b.upper);
	c.lower=(a.lower^b.lower);
	return c;
}

inline const YsBigInt &YsBigInt::Atoi(const char str[])
{
	int i;
	int sgn;

	sgn=1;
	i=0;
	upper=0;
	lower=0;

	for(i=0; str[i]=='-' || str[i]=='+'; i++)
	{
		if(str[i]=='-')
		{
			sgn=-1;
		}
	}

	if('0'<=str[i] && str[i]<='9')
	{
		lower=(str[i]-'0');
		i++;

		while('0'<=str[i] && str[i]<='9' && i<10)
		{
			lower*=10;
			lower+=(str[i]-'0');
			i++;
		}

		while('0'<=str[i] && str[i]<='9')
		{
			(*this)*=10;
			(*this)+=(str[i]-'0');
			i++;
		}
	}

	if(sgn<0)
	{
		(*this)=-(*this);
	}

	return *this;
}

inline const char *YsBigInt::Itoa(char str[],int nBufLen) const
{
	if(nBufLen>0)
	{
		if(upper==0 && lower==0)
		{
			str[0]='0';
			str[1]=0;
			return str;
		}

		int i,i0,n,sgn;
		YsBigInt a,div,mod;

		i=0;
		i0=0;
		sgn=1;

		a=*this;
		if(a.upper<0)
		{
			a=-a;
			sgn=-1;
			str[i]='-';
			i++;
		}

		i0=i;
		n=0;
		while(i<nBufLen-1 && a>0)
		{
			a.Divide(div,mod,10);
			str[i++]='0'+mod.lower;
			a=div;
			n++;
		}
		str[i]=0;

		for(i=0; i<n/2; i++)
		{
			char swp;
			swp=str[i+i0];
			str[i0+i]=str[i0+n-1-i];
			str[i0+n-1-i]=swp;
		}
	}
	return str;
}

inline const YsBigInt &YsBigInt::Negate(void)
{
	upper=~upper;
	lower=~lower;
	lower++;
	if(lower==0)
	{
		upper++;
	}
	return *this;
}

inline const YsBigInt &YsBigInt::Multiply(YsBigInt &mul,const YsBigInt &_a,const YsBigInt &_b)
{
	YsBigInt a,b,tst;
	int sgn;

	sgn=1;

	a=_a;
	if(a.upper<0)
	{
		a.Negate();
		sgn=-sgn;
	}

	b=_b;
	if(b.upper<0)
	{
		b.Negate();
		sgn=-sgn;
	}

	mul=0;
	tst=1;
	while(tst!=YsBigInt(0))
	{
		if((tst&a)!=0)
		{
			mul+=b;
		}
		tst<<=1;
		b<<=1;
	}

	return mul;
}

inline const YsBigInt &YsBigInt::Divide(YsBigInt &div,YsBigInt &mod,const YsBigInt &_denom)
{
	int sgn;
	YsBigInt denom,numer;

	numer=*this;
	sgn=1;
	if(numer.upper<0)
	{
		sgn=-1;
		numer.Negate();
	}

	denom=_denom;
	if(_denom.upper<0)
	{
		sgn=-sgn;
		denom.Negate();
	}

	if(denom==0)
	{
		InduceDivisionByZeroException();
	}

	if(numer<denom)
	{
		div=0;
		mod=*this;
	}
	else
	{
		YsBigInt highBit,divBit;

		highBit.upper=INTHIGHBIT;
		highBit.lower=0;
		while((highBit&numer)==0)
		{
			highBit>>=1;
			if(highBit==0)
			{
				InduceDivisionByZeroException();
			}
		}

		divBit=1;
		div=0;
		mod=0;
		while((denom&highBit)==0)
		{
			denom<<=1;
			divBit<<=1;
		}

		while(1)
		{
			if(denom<=numer)
			{
				div+=divBit;
				numer-=denom;
			}

			divBit>>=1;
			if(divBit==0)
			{
				break;
			}
			denom>>=1;
		}
		mod=numer;

		if(sgn<0)
		{
			div.Negate();
			mod.Negate();
		}
	}
	return div;
}

inline void YsBigInt::InduceDivisionByZeroException(void) const
{
	int a,b;
	a=10;
	b=0;
	a/=b;
}


/* } */
#endif
