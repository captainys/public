/* ////////////////////////////////////////////////////////////

File Name: ysmath.h
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

#ifndef YSMATH_IS_INCLUDED
#define YSMATH_IS_INCLUDED
/* { */

/*! \file */

#include "ysdef.h"

/*! An inline template function that returns absoluve value of the parameter. */
template <class T>
inline const T YsAbs(const T &a)
{
	return (a>=0 ? a : -a);
}

#define YsEqual(a,b) (YsAbs((a)-(b))<=YsTolerance ? YSTRUE : YSFALSE)
#define YsZero(a) YsEqual((a),0.0)

void YsSetToleranceFromDimension(const double dimension,const double scale=1.0e-8);


/*! An inline template function that returns greater value of a and b. */
template <class T>
inline const T YsGreater(const T &a,const T &b)
{
	return ((a)>(b) ? (a) : (b));
}

/*! An inline template function that returns smaller value of a and b.  */
template <class T>
inline const T YsSmaller(const T &a,const T &b)
{
	return ((a)<(b) ? (a) : (b));
}

/*! Update a with the greater one of a and b. */
template <class T>
inline void YsMakeGreater(T &a,const T &b)
{
	if(b>a)
	{
		a=b;
	}
}

/*! Update a with the smaller one of a and b. */
template <class T>
inline void YsMakeSmaller(T &a,const T &b)
{
	if(b<a)
	{
		a=b;
	}
}

/*! This function returns:
       x if a<=x && x<=b,
       a if x<a, or
       b if b<x.
    Return type will be T1.
 */
template <class T>
inline const T YsBound(const T &x,const T &a,const T &b)
{
	return ((x)<(a) ? (a) : ((x)>(b) ? (b) : (x)));
}

/*! An inline template function that returns the smallest value of a, b, and c. */
template <class T>
inline const T YsSmallestOf(const T &a,const T &b,const T &c)
{
	return (((a)<(b) && (a)<(c)) ? (a) : ((b)<(c) ? (b) : (c)));
}

/*! An inline template function that returns the largest value of a, b, and c */
template <class T>
inline const T YsGreatestOf(const T &a,const T &b,const T &c)
{
	return (((a)>(b) && (a)>(c)) ? (a) : ((b)>(c) ? (b) : (c)));
}

/*! This function returns YSTRUE if edge (a,b) is same as (c,d) or (d,c), or YSFALSE otherwise.  Useful for edge comparison. */
template <class T>
inline YSBOOL YsSameEdge(const T &a,const T &b,const T &c,const T &d)
{
	return ((((a)==(c) && (b)==(d)) || ((a)==(d) && (b)==(c))) ? YSTRUE : YSFALSE);
}

/*! This function returns YSTRUE if edge (a[0],a[1]) is same as (b[0],b[1]) or (b[1],b[0]), or YSFALSE otherwise.  Useful for edge comparison. */
template <class T>
inline YSBOOL YsSameEdge(const T a[2],const T b[2])
{
	return YsSameEdge<T>(a[0],a[1],b[0],b[1]);
}

template <typename T>
inline YSBOOL YsSamePolygon(YSSIZE_T np0,const T p0[],YSSIZE_T np1,const T p1[],YSBOOL orientationSensitive=YSFALSE)
{
	if(np0!=np1)
	{
		return YSFALSE;
	}

	YSSIZE_T firstCommonVertexIdx1=-1;
	for(YSSIZE_T i1=0; i1<np1; ++i1)
	{
		if(p0[0]==p1[i1])
		{
			firstCommonVertexIdx1=i1;
			break;
		}
	}

	if(0>firstCommonVertexIdx1)
	{
		return YSFALSE;
	}

	YSBOOL diff=YSFALSE;
	for(YSSIZE_T i=1; i<np0; ++i)
	{
		if(p0[i]!=p1[(firstCommonVertexIdx1+i)%np0])
		{
			diff=YSTRUE;
			break;
		}
	}
	if(YSTRUE!=diff)
	{
		return YSTRUE;
	}
	if(YSTRUE==diff && YSTRUE==orientationSensitive)
	{
		return YSFALSE;
	}

	for(YSSIZE_T i=1; i<np0; ++i)
	{
		if(p0[np0-i]!=p1[(firstCommonVertexIdx1+i)%np0])
		{
			return YSFALSE;
		}
	}
	return YSTRUE;
}

template <class T>
inline YSRESULT YsSharedPoint(T &shared,T a0,T a1,T b0,T b1)
{
	if(a0==b0 || a0==b1)
	{
		shared=a0;
		return YSOK;
	}
	else if(a1==b0 || a1==b1)
	{
		shared=a1;
		return YSOK;
	}
	return YSERR;
}

template <class T>
inline YSRESULT YsSharedPoint(T &shared,const T a[2],const T b[2])
{
	return YsSharedPoint<T>(shared,a[0],a[1],b[0],b[1]);
}

template <class T>
inline YSBOOL YsEdgeSharingNode(T a0,T a1,T b0,T b1)
{
	T s;
	if(YSOK==YsSharedPoint(s,a0,a1,b0,b1))
	{
		return YSTRUE;
	}
	return YSFALSE;
}

template <class T>
inline YSBOOL YsEdgeSharingNode(const T a[2],const T b[2])
{
	T s;
	if(YSOK==YsSharedPoint(s,a,b))
	{
		return YSTRUE;
	}
	return YSFALSE;
}


/*! This function returns either A or B whichever is not C.  If A!=C && B!=C, this function returns A. 
    Template parameter T must be a class that operator== and operator!= is available. */
template <class T>
inline const T &YsPickOneOfAandBThatIsNotC(const T &A,const T &B,const T&C)
{
	if(A==C)
	{
		return B;
	}
	return A;
}

/*! This function returns either dat[0] or dat[1] whichever is not C.  If A!=C && B!=C, this function returns A. 
    Template parameter T must be a class that operator== and operator!= is available. */
template <class T>
inline const T &YsPickOneOfAandBThatIsNotC(const T dat[2],const T&C)
{
	if(dat[0]==C)
	{
		return dat[1];
	}
	return dat[0];
}

/*! This function returns A if condition C is true, or B otherwise. 
    Type safe way of writing (condition ? A : B). */
template <class T>
inline const T& YsIfTruePickA(int cond,const T &A,const T &B)
{
	if(cond)
	{
		return A;
	}
	return B;
}

/*! This function returns dat[0] if condition C is true, or dat[1] otherwise. 
    Type safe way of writing (cond ? A : B). */
template <class T>
inline const T& YsIfTruePickA(int cond,const T dat[2])
{
	if(cond)
	{
		return dat[0];
	}
	return dat[1];
}



#define YSBLUEIMPULSE YSLEFT_ZPLUS_YPLUS
#define YSOPENGL YSRIGHT_ZMINUS_YPLUS

const double YsPi=3.14159265358979323;
const double YsE=2.71828;



const double YsCos1deg=0.999847695156;
const double YsCos3deg=0.998629534755;
const double YsCos5deg=0.996194698092;
const double YsCos10deg=0.984807753012;
const double YsCos15deg=0.965925826289;
const double YsCos20deg=0.939692620786;
const double YsCos25deg=0.906307787037;
const double YsCos30deg=0.866025403784;
const double YsCos35deg=0.819152044289;
const double YsCos40deg=0.766044443119;
const double YsCos45deg=0.707106781187;
const double YsCos50deg=0.642787609687;
const double YsCos55deg=0.573576436351;
const double YsCos60deg=0.500000000000;
const double YsCos65deg=0.422618261741;
const double YsCos70deg=0.342020143326;
const double YsCos75deg=0.258819045103;
const double YsCos80deg=0.173648177667;
const double YsCos85deg=0.087155742748;
const double YsCos90deg=0.000000000000;
const double YsCos95deg=-0.087155742748;
const double YsCos100deg=-0.173648177667;
const double YsCos105deg=-0.258819045103;
const double YsCos110deg=-0.342020143326;
const double YsCos115deg=-0.422618261741;
const double YsCos120deg=-0.500000000000;
const double YsCos125deg=-0.573576436351;
const double YsCos130deg=-0.642787609687;
const double YsCos135deg=-0.707106781187;
const double YsCos140deg=-0.766044443119;
const double YsCos145deg=-0.819152044289;
const double YsCos150deg=-0.866025403784;
const double YsCos155deg=-0.906307787037;
const double YsCos160deg=-0.939692620786;
const double YsCos165deg=-0.965925826289;
const double YsCos170deg=-0.984807753012;
const double YsCos175deg=-0.996194698092;
const double YsCos177deg=-0.998629534755;
const double YsCos179deg=-0.999847695156;
const double YsSin1deg=0.017452406437;
const double YsSin3deg=0.052335956243;
const double YsSin5deg=0.087155742748;
const double YsSin10deg=0.173648177667;
const double YsSin15deg=0.258819045103;
const double YsSin20deg=0.342020143326;
const double YsSin25deg=0.422618261741;
const double YsSin30deg=0.500000000000;
const double YsSin35deg=0.573576436351;
const double YsSin40deg=0.642787609687;
const double YsSin45deg=0.707106781187;
const double YsSin50deg=0.766044443119;
const double YsSin55deg=0.819152044289;
const double YsSin60deg=0.866025403784;
const double YsSin65deg=0.906307787037;
const double YsSin70deg=0.939692620786;
const double YsSin75deg=0.965925826289;
const double YsSin80deg=0.984807753012;
const double YsSin85deg=0.996194698092;
const double YsSin90deg=1.000000000000;
const double YsSin95deg=0.996194698092;
const double YsSin100deg=0.984807753012;
const double YsSin105deg=0.965925826289;
const double YsSin110deg=0.939692620786;
const double YsSin115deg=0.906307787037;
const double YsSin120deg=0.866025403784;
const double YsSin125deg=0.819152044289;
const double YsSin130deg=0.766044443119;
const double YsSin135deg=0.707106781187;
const double YsSin140deg=0.642787609687;
const double YsSin145deg=0.573576436351;
const double YsSin150deg=0.500000000000;
const double YsSin155deg=0.422618261741;
const double YsSin160deg=0.342020143326;
const double YsSin165deg=0.258819045103;
const double YsSin170deg=0.173648177667;
const double YsSin175deg=0.087155742748;
const double YsSin177deg=0.052335956243;
const double YsSin179deg=0.017452406437;
const double YsTan1deg=0.017455064928;
const double YsTan3deg=0.052407779283;
const double YsTan5deg=0.087488663526;
const double YsTan10deg=0.176326980708;
const double YsTan15deg=0.267949192431;
const double YsTan20deg=0.363970234266;
const double YsTan25deg=0.466307658155;
const double YsTan30deg=0.577350269190;
const double YsTan35deg=0.700207538210;
const double YsTan40deg=0.839099631177;
const double YsTan45deg=1.000000000000;
const double YsTan50deg=1.191753592594;
const double YsTan55deg=1.428148006742;
const double YsTan60deg=1.732050807569;
const double YsTan65deg=2.144506920510;
const double YsTan70deg=2.747477419455;
const double YsTan75deg=3.732050807569;
const double YsTan80deg=5.671281819618;
const double YsTan85deg=11.430052302761;

const double YsMeterPerNm=1852.0;
const double YsMeterPerSm=1609.0;
const double YsKmPerNm=1.852;
const double YsKmPerSm=1.609;
const double YsMeterPerInch=0.0254;
const double YsMeterPerFt=0.3048;
const double YsCmPerInch=2.54;
const double YsCmPerFt=30.48;
const double YsKgPerLb=0.453597;
const double YsKgPerPound=0.453597;

const double YsInfinity=1.0E+32;

extern double YsTolerance;
extern double YsZero;
extern YSCOORDSYSMODEL YsCoordSysModel;

#define YsToleranceSqr (YsTolerance*YsTolerance)



//extern const double YsZero;
//extern const double YsOne;




////////////////////////////////////////////////////////////

inline double YsDegToRad(const double &deg)
{
	return deg*YsPi/180.0;
}

inline double YsRadToDeg(const double &rad)
{
	return rad*180.0/YsPi;
}

template <class T>
inline const T YsSqr(const T &x)
{
	return x*x;
}
#define YsSqr(x) ((x)*(x))   // Outer () : credit to Ved
//inline double YsSqr(const double &x)
//{
//	return x*x;
//}

////////////////////////////////////////////////////////////

template <class T>
const T &YsGreatest(int n,const T v[])
{
	if(n>0 && v!=NULL)
	{
		int i,iRef;
		iRef=0;
		for(i=1; i<n; i++)
		{
			if(v[i]>v[iRef])
			{
				iRef=i;
			}
		}
		return v[iRef];
	}
	return v[0];
}

template <class T>
const T &YsSmallest(int n,const T v[])
{
	if(n>0 && v!=NULL)
	{
		int i,iRef;
		iRef=0;
		for(i=1; i<n; i++)
		{
			if(v[i]<v[iRef])
			{
				iRef=i;
			}
		}
		return v[iRef];
	}
	return v[0];
}

template <class T>
const T &YsMedian(const T &a,const T &b,const T &c)
{
	if((a<=b && b<=c) || (c<=b && b<=a))
	{
		return b;
	}
	else if((b<=a && a<=c) || (c<=a && a<=b))
	{
		return a;
	}
	else
	{
		return c;
	}
}


/*! This function calculates piecewise linear function.  The function is defined by nSample and sample.
    The function value is y=sample[n*2+1] at x=sample[n*2]. 
    sample[(n+1)*2] must be greater than sample[n*2].
    If x<sample[0] || sample[(nSample-2)*2]<x, the first or last segment will be linearly extended.  */
template <class T>
const double YsCalculatePiecewiseLinearFunction(YSSIZE_T nSample,const T sample[],const T x)
{
	// Haven't I written this function somewhere before?
	if(nSample<=0)
	{
		return 0.0;
	}
	else if(1==nSample)
	{
		return sample[1];
	}
	else
	{
		double x0,y0,x1,y1;
		if(x<=sample[0])
		{
			x0=sample[0];
			y0=sample[1];
			x1=sample[2];
			y1=sample[3];
		}
		else if(sample[nSample*2-4]<=x)
		{
			x0=sample[nSample*2-4];
			y0=sample[nSample*2-3];
			x1=sample[nSample*2-2];
			y1=sample[nSample*2-1];
		}
		else
		{
			YSSIZE_T idx;
			for(idx=0; idx<nSample-2 && sample[idx*2+2]<x; ++idx)
			{
			}
			x0=sample[idx*2];
			y0=sample[idx*2+1];
			x1=sample[idx*2+2];
			y1=sample[idx*2+3];
		}
		return y0+(x-x0)*(y1-y0)/(x1-x0);
	}
}


/* } */
#endif
