/* ////////////////////////////////////////////////////////////

File Name: ysgeometry.h
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

#ifndef YSGEOMETRY_IS_INCLUDED
#define YSGEOMETRY_IS_INCLUDED
/* { */

/*! \file */

#include <math.h>
#include "ysdef.h"
#include "ysmath.h"
#include "ysprintf.h"
#include "ysmatrixtemplate.h"
#include "ysmatrix.h"
#include "ysarray.h"

/// \cond
#define YSMATHFAST  // This line disables Memory Leak Tracking
/// \endcond
////////////////////////////////////////////////////////////


// Declaration /////////////////////////////////////////////
/*! A 2D vector class 
    \sa operator-(const YsVec2 &a)
    \sa operator+(const YsVec2 &a,const YsVec2 &b)
    \sa operator/(const YsVec2 &a,const double &b)
    \sa operator*(const YsVec2 &a,const double &b)
    \sa operator*(const double &b,const YsVec2 &a)
    \sa operator*(const YsVec2 &a,const YsVec2 &b)
    \sa operator^(const YsVec2 &a,const YsVec2 &b)
    \sa operator==(const YsVec2 &a,const YsVec2 &b)
    \sa operator!=(const YsVec2 &a,const YsVec2 &b)
*/
class YsVec2
{
private:
	double value[2];

	bool operator==(class YsVec3 &);       // Prevent YsVec2 from being casted into const double * and ended up being compared 
	bool operator!=(class YsVec3 &);
	bool operator==(const class YsVec3 &); // Prevent YsVec2 from being casted into const double * and ended up being compared 
	bool operator!=(const class YsVec3 &);

public:
	enum
	{
		NDIMENSION=2
	};
	typedef double COMPONENT_TYPE;


	/*! Default constructor.  Does nothing. */
	inline YsVec2()
	{
	}

	/*! Constructor that takes x and y coordinates */
	inline YsVec2(const double &x,const double &y)
	{
		value[0]=x;
		value[1]=y;
	}

	/*! Constructor that takes an array of doubles. */
	inline explicit YsVec2(const double incoming[2])
	{
		value[0]=incoming[0];
		value[1]=incoming[1];
	}

	/*! Returns the origin. */
	static inline const YsVec2 Origin(void)
	{
		return YsVec2(0.0,0.0);
	}

	/*! Returns a normalized vector of v. */
	static inline YsVec2 UnitVector(const YsVec2 &v)
	{
		auto u=v;
		u.Normalize();
		return u;
	}

	// Added 2001/04/30 >>
	/*! Constructor that takes two strings.  (Strings are interpreted by atof) */
	YsVec2(const char x[],const char y[]);
	// Added 2001/04/30 <<

	// Added 2001/04/26 >>
	/*! An operator allows constant access to an individual component by an index number.
	    \param idx [In] 0->x  1->y
	*/
	inline const double &operator[](int idx) const
	{
		return value[idx];
	}

	/*! An operator allows access to an individual component by an index number.
	    \param idx [In] 0->x  1->y
	*/
	inline double &operator[](int idx)
	{
		return value[idx];
	}

	/*! A cast operator that gives constant double pointer.  With this cast operator, YsVec2 
	    object can directly be given to glVertex2dv */
	operator const double *() const
	{
		return GetValue();
	}

	/*! This function returns a formatted text of the vector.
		\param fmt [In] printf formatting string.  Memory chunk
		needed to store the text is taken from the shared
		internal memory pool.  Therefore, do NOT delete the
		returned pointer.
	*/
	const char *Txt(const char *fmt="%10.6lf %10.6lf") const;
	// Added 2001/04/26 <<

	// Added 2001/05/04 >>
	/*! Add X component */
	inline void AddX(const double &x)
	{
		value[0]+=x;
	}
	/*! Add Y component */
	inline void AddY(const double &y)
	{
		value[1]+=y;
	}
	/*! Subtract X component */
	inline void SubX(const double &x)
	{
		value[0]-=x;
	}
	/*! Subtract Y component */
	inline void SubY(const double &y)
	{
		value[1]-=y;
	}
	/*! Multipy X component */
	inline void MulX(const double &x)
	{
		value[0]*=x;
	}
	/*! Multiply Y component */
	inline void MulY(const double &y)
	{
		value[1]*=y;
	}
	/*! Divide X component */
	inline void DivX(const double &x)
	{
		value[0]/=x;
	}
	/*! Divide Y component */
	inline void DivY(const double &y)
	{
		value[1]/=y;
	}
	// Added 2001/05/04 <<


	/*! Returns X component */
	inline double x(void) const  // 2001/04/17 : Don't make it const double &
	{
		return value[0];
	}
	/*! Returns Y component */
	inline double y(void) const  // 2001/04/17 : Don't make it const double &
	{
		return value[1];
	}
	/*! Returns X component in float. */
	inline float xf(void) const  // 2001/04/17 : Don't make it const double &
	{
		return (float)value[0];
	}
	/*! Returns Y component in float. */
	inline float yf(void) const  // 2001/04/17 : Don't make it const double &
	{
		return (float)value[1];
	}

	/*! Returns X component in integer. */
	inline int xi(void) const  // 2001/04/17 : Don't make it const double &
	{
		return (int)value[0];
	}
	/*! Returns Y component in integer. */
	inline int yi(void) const  // 2001/04/17 : Don't make it const double &
	{
		return (int)value[1];
	}

	/*! Returns a constant double pointer, which can be given to a function like glVertex3dv.
	    You can use this function when you want to explicitly want a double pointer
	    from this class rather than implicitly using the cast operator.
	*/
	inline const double *GetValue(void) const
	{
		return value;
	}
	/*! Returns x and y components to the given references. */
	inline void Get(double &x,double &y) const
	{
		x=value[0];
		y=value[1];
	}
	/*! Returns x*x+y*y */
	inline double GetSquareLength(void) const
	{
		return value[0]*value[0]+value[1]*value[1];
	}
	/*! Returns sqrt(x*x+y*y) */
	inline double GetLength(void) const
	{
		return (double)sqrt(GetSquareLength());
	}

	/*! Returns greater of fabs(x) and fabs(y) */
	inline double L0Norm(void) const
	{
		return YsGreater(fabs(value[0]),fabs(value[1]));
	}

	/*! Returns an index to the axis of the longest-dimension */
	inline int GetLongestAxis(void) const
	{
		if(fabs(value[0])>fabs(value[1]))
		{
			return 0;
		}
		else
		{
			return 1;
		}
	}

	/*! Same as L0Norm */
	inline double GetLongestDimension(void) const
	{
		if(fabs(value[0])>fabs(value[1]))
		{
			return fabs(value[0]);
		}
		else
		{
			return fabs(value[1]);
		}
	}

	/*! Returns YSTRUE if the vector is normalized or YSFALSE otherwise. */
	inline YSBOOL IsNormalized(void) const;
	/*! Normalizes the vector.  If it is successful, it returns YSOK.  If the vector is of zero length, it returns YSERR. */
	inline YSRESULT Normalize(void);
	/*! Returns YSTRUE if the vector is of zero length or YSFALSE otherwise. */
	inline YSBOOL IsZeroVector(void) const
	{
		if(GetSquareLength()<YsToleranceSqr)
		{
			return YSTRUE;
		}
		else
		{
			return YSFALSE;
		}
	}

	/*! This function returns YSTRUE if the projection of this coordinate 
	    on the line that passes through the given two end points is between 
	    the two end points or is lying on one of the end points.  It returns
	    YSFALSE otherwise.
	    \param p1 [In] One of the end points
	    \param p2 [In] The other end point
	    */
	YSBOOL IsInBetween(const YsVec2 &p1,const YsVec2 &p2) const;

	/*! This function returns YSTRUE if this coordinate is inside or on the boundary of the given bounding box
	    or YSFALSE otherwise.
	    \param min [In] Minimum x, y, and z of the bounding box
	    \param max [In] Maximum x, y, and z of the bounding box
	    */
	YSBOOL IsInsideBoundingBox(const YsVec2 &min,const YsVec2 &max) const;

	/*! This function returns YSTRUE if this coordinate is inside or on the arc of a circle 
	    centered at (p1+p2)/2.0 with radius of |p2-p1|/2.0. */
	YSBOOL IsInsideDiameter(const YsVec2 &p1,const YsVec2 &p2) const;

	/*! This function returns distance from this coordinate to a line that passes through given points.
	    \param p1 [In] A point that the line passes through
	    \param p2 [In] A point that the line passes through */
	const double GetDistanceFromLine(const YsVec2 &p1,const YsVec2 &p2) const;

	/*! This function returns distance from this coordinate to a piece of line
	    \param p1 [In] A end point of the line
	    \param p2 [In] The other end point of the line */
	const double GetDistanceFromLineSegment(const YsVec2 &p1,const YsVec2 &p2) const;

	/*! This function returns the nearest point on the given line from this coordinate.
	    The line is defined by two points that the line passes through.
	    \param p1 [In] A point that the line passes through
	    \param p2 [In] A point that the line passes through */
	const YsVec2 GetNearestPointOnLine(const YsVec2 &p1,const YsVec2 &p2) const;


	/*! Sets x and y components */
	inline void Set(const double &x,const double &y)
	{
		value[0]=x;
		value[1]=y;
	}
	/*! Sets x and y components.  Strings are interpreted by atof. */
	void Set(const char x[],const char y[]);
	/*! Sets X component */
	inline void SetX(const double &x)
	{
		value[0]=x;
	}
	/*! Sets Y component */
	inline void SetY(const double &y)
	{
		value[1]=y;
	}
	/*! Sets X and Y components. */
	inline void Set(const double v[2])
	{
		value[0]=v[0];
		value[1]=v[1];
	}
	/*! Copies X and Y components of 3D vector.
	   \param vec3 [In] A const pointer to a 3D vector.  Since YsVec3 class has a cast operator to const double *, a YsVec3 object
	          can directly given to this function.
	*/
	inline void GetXY(const double vec3[])
	{
		value[0]=vec3[0];
		value[1]=vec3[1];
	}
	/*! Copies X and Z components of 3D vector.
	   \param vec3 [In] A const pointer to a 3D vector.  Since YsVec3 class has a cast operator to const double *, a YsVec3 object
	          can directly given to this function.
	*/
	inline void GetXZ(const double vec3[])
	{
		value[0]=vec3[0];
		value[1]=vec3[2];
	}
	/*! Rotates the vector counter-clockwise.
	   \param ang [In] Angle in radian
	*/
	inline void Rotate(const double &ang)
	{
		const double c=cos(ang);
		const double s=sin(ang);
		const double newVec[]={x()*c-y()*s,x()*s+y()*c};
		Set(newVec);
	}


	/*! Adds another 2D vector */
	inline const YsVec2 &operator+=(const YsVec2 &a)
	{
		value[0]+=a.value[0];
		value[1]+=a.value[1];
		return *this;
	}
	/*! Subtracts another 2D vector */
	inline const YsVec2 &operator-=(const YsVec2 &a)
	{
		value[0]-=a.value[0];
		value[1]-=a.value[1];
		return *this;
	}
	/*! Divides the vector by a scalra value. */
	inline const YsVec2 &operator/=(double a)
	{
		value[0]/=a;
		value[1]/=a;
		return *this;
	}
	/*! Multiplies the vector by a scalra value. */
	inline const YsVec2 &operator*=(double a)
	{
		value[0]*=a;
		value[1]*=a;
		return *this;
	}
};

inline YSBOOL YsVec2::IsNormalized(void) const
{
	double verify;
	verify=GetSquareLength();
	if(1.0-YsTolerance<=verify && verify<=1.0+YsTolerance)
	{
		return YSTRUE;
	}
	else
	{
		return YSFALSE;
	}
}

inline YSRESULT YsVec2::Normalize(void)
{
	const double lengthSq=GetSquareLength();
	if(lengthSq>YsTolerance*YsTolerance)  // Modified 2000/11/10
	{
		const double length=sqrt(lengthSq);
		value[0]/=length;
		value[1]/=length;
		return YSOK;
	}
	else
	{
		return YSERR;
	}
}

/*! An operator that returns the reverse vector. */
inline const YsVec2 operator-(const YsVec2 &a)
{
	const double *avalue=a;
	return YsVec2(-avalue[0],-avalue[1]);
}

/*! An operator that retuns subtraction of vector b from vector a. */
inline const YsVec2 operator-(const YsVec2 &a,const YsVec2 &b)
{
	const double *avalue=a;
	const double *bvalue=b;
	return YsVec2(avalue[0]-bvalue[0],avalue[1]-bvalue[1]);
}

/*! An operator that returns addition of vectors a and b. */
inline const YsVec2 operator+(const YsVec2 &a,const YsVec2 &b)
{
	const double *avalue=a;
	const double *bvalue=b;
	return YsVec2(avalue[0]+bvalue[0],avalue[1]+bvalue[1]);
}

/*! An operator that returns a vector divided by a scalar value. */
inline const YsVec2 operator/(const YsVec2 &a,const double &b)
{
	const double *avalue=a;
	return YsVec2(avalue[0]/b,avalue[1]/b);
}

/*! An operator that returns a vector multiplied by a scalar value. */
inline const YsVec2 operator*(const YsVec2 &a,const double &b)
{
	const double *avalue=a;
	return YsVec2(avalue[0]*b,avalue[1]*b);
}

/*! An operator that returns a vector multiplied by a scalar value. */
inline const YsVec2 operator*(const double &b,const YsVec2 &a)
{
	const double *avalue=a;
	return YsVec2(avalue[0]*b,avalue[1]*b);
}

/*! An operator that returns an inner (dot) product of vectors a and b. */
inline double operator*(const YsVec2 &a,const YsVec2 &b)
{
	const double *avalue=a;
	const double *bvalue=b;
	return avalue[0]*bvalue[0]+avalue[1]*bvalue[1];
}

/*! An operator that returns an outer (cross) product of vectors a and b. 
    Since the input parameters are 2D vector, int only returns the
    Z component of the cross product.
*/
inline double operator^(const YsVec2 &a,const YsVec2 &b)
{
	// x1 y1
	// x2 y2
	double z;
	z=a.x()*b.y()-a.y()*b.x();
	return z;
}

/*! An operator for comparing two vectors. */
inline int operator==(const YsVec2 &a,const YsVec2 &b)
{
	// Fixed 2000/11/10
	// return ((a-b).GetSquareLength()<YsTolerance);

	// Modified again 2000/11/10
	// if(YsEqual(a.x(),b.x())==YSTRUE &&
	//    YsEqual(a.y(),b.y())==YSTRUE)
	// {
	// 	return 1;
	// }
	// else
	// {
	// 	return 0;
	// }

	return ((a-b).GetSquareLength()<YsSqr(YsTolerance));  // Modified 2000/11/10
}

/*! An operator for comparing two vectors. */
inline int operator!=(const YsVec2 &a,const YsVec2 &b)
{
	// Fixed 2000/11/10
	// return ((a-b).GetSquareLength()>=YsTolerance);

	// Modified again 2000/11/10
	// if(YsEqual(a.x(),b.x())!=YSTRUE ||
	//    YsEqual(a.y(),b.y())!=YSTRUE)
	// {
	// 	return 1;
	// }
	// else
	// {
	// 	return 0;
	// }

	return ((a-b).GetSquareLength()>=YsSqr(YsTolerance));  // Modified 2000/11/10
}


/*! This function returns one of the vectors v1 and v2, whichever longer. */
inline const YsVec2 &YsLonger(const YsVec2 &v1,const YsVec2 &v2)
{
	return (v1.GetSquareLength()>=v2.GetSquareLength() ? v1 : v2);
}


////////////////////////////////////////////////////////////

// Declaration /////////////////////////////////////////////
/*! A 3D vector class 
    \sa operator-(const YsVec3 &a,const YsVec3 &b)
    \sa operator+(const YsVec3 &a,const YsVec3 &b)
    \sa operator/(const YsVec3 &a,const double &b)
    \sa operator*(const YsVec3 &a,const double &b)
    \sa operator*(const double &b,const YsVec3 &a)
    \sa operator*(const YsVec3 &a,const YsVec3 &b)
    \sa operator^(const YsVec3 &a,const YsVec3 &b)
    \sa operator==(const YsVec3 &a,const YsVec3 &b)
    \sa operator!=(const YsVec3 &a,const YsVec3 &b)
    \sa operator*(const YsRotation &rot,const YsVec3 &vec)
    \sa YsRotation
    \sa YsMatrix3x3
    \sa YsMatrix4x4
    \sa operator*(const YsMatrix3x3 &a,const YsVec3 &b)
    \sa operator*(const YsMatrix4x4 &a,const YsVec3 &b)
*/
class YsVec3
{
private:
	double value[3];
	bool operator==(YsVec2 &);       // Prevent YsVec2 from being casted into const double * and ended up being compared 
	bool operator!=(YsVec2 &);
	bool operator==(const YsVec2 &); // Prevent YsVec2 from being casted into const double * and ended up being compared 
	bool operator!=(const YsVec2 &);

public:
	enum
	{
		NDIMENSION=3
	};
	typedef double COMPONENT_TYPE;

	/*! Default constructor.  Does nothing. */
	inline YsVec3()
	{
	}
	/*! Constructor that takes x, y, and z coordinates */
	inline YsVec3(const double &x,const double &y,const double &z)
	{
		value[0]=x;
		value[1]=y;
		value[2]=z;
	}
	/*! Constructor that takes an array of doubles. */
	inline explicit YsVec3(const double incoming[3])
	{
		value[0]=incoming[0];
		value[1]=incoming[1];
		value[2]=incoming[2];
	}
	/*! Constructor that takes three strings.  (Strings are interpreted by atof) */
	YsVec3(const char x[],const char y[],const char z[]);

	/*! Constructor that takes a YsVec2 class and z component. */
	inline YsVec3(const YsVec2 &v2,const double z)
	{
		value[0]=v2[0];
		value[1]=v2[1];
		value[2]=z;
	}

	// Added 2001/04/26 >>
	/*! An operator allows constant access to an individual component by an index number.
	    \param idx [In] 0->x  1->y   2->z
	*/
	inline const double &operator[](int idx) const
	{
		return value[idx];
	}
	/*! An operator allows access to an individual component by an index number.
	    \param idx [In] 0->x  1->y   2->z
	*/
	inline double &operator[](int idx)
	{
		return value[idx];
	}
	/*! A cast operator that gives constant double pointer.  With this cast operator, YsVec2 
	    object can directly be given to glVertex2dv */
	operator const double *() const
	{
		return GetValue();
	}
	/*! This function returns a formatted text of the vector.
		\param fmt [In] printf formatting string.  Memory chunk
		needed to store the text is taken from the shared
		internal memory pool.  Therefore, do NOT delete the
		returned pointer.
	*/
	const char *Txt(const char *fmt="%10.6lf %10.6lf %10.6lf") const;
	// Added 2001/04/26 <<

	// Added 2001/05/04 >>
	/*! Add X component */
	inline void AddX(const double &x)
	{
		value[0]+=x;
	}
	/*! Add Y component */
	inline void AddY(const double &y)
	{
		value[1]+=y;
	}
	/*! Add Z component */
	inline void AddZ(const double &z)
	{
		value[2]+=z;
	}
	/*! Subtract X component */
	inline void SubX(const double &x)
	{
		value[0]-=x;
	}
	/*! Subtract Y component */
	inline void SubY(const double &y)
	{
		value[1]-=y;
	}
	/*! Subtract Z component */
	inline void SubZ(const double &z)
	{
		value[2]-=z;
	}
	/*! Multipy X component */
	inline void MulX(const double &x)
	{
		value[0]*=x;
	}
	/*! Multipy Y component */
	inline void MulY(const double &y)
	{
		value[1]*=y;
	}
	/*! Multipy Z component */
	inline void MulZ(const double &z)
	{
		value[2]*=z;
	}
	/*! Divide X component */
	inline void DivX(const double &x)
	{
		value[0]/=x;
	}
	/*! Divide Y component */
	inline void DivY(const double &y)
	{
		value[1]/=y;
	}
	/*! Divide Z component */
	inline void DivZ(const double &z)
	{
		value[2]/=z;
	}
	// Added 2001/05/04 <<

	/*! Returns X component */
	inline double x(void) const   // 2001/04/17 : Don't make it const double &
	{
		return value[0];
	}
	/*! Returns Y component */
	inline double y(void) const  // 2001/04/17 : Don't make it const double &
	{
		return value[1];
	}
	/*! Returns Z component */
	inline double z(void) const  // 2001/04/17 : Don't make it const double &
	{
		return value[2];
	}

	/*! Returns a 2D vector by taking x- and y- components. */
	inline YsVec2 xy(void) const
	{
		return YsVec2(value[0],value[1]);
	}
	/*! Returns a 2D vector by taking x- and z- components. */
	inline YsVec2 xz(void) const
	{
		return YsVec2(value[0],value[2]);
	}
	/*! Returns a 2D vector by taking y- and z- components. */
	inline YsVec2 yz(void) const
	{
		return YsVec2(value[1],value[2]);
	}
	/*! Returns a 2D vector by taking y- and x- components. */
	inline YsVec2 yx(void) const
	{
		return YsVec2(value[1],value[0]);
	}
	/*! Returns a 2D vector by taking z- and x- components. */
	inline YsVec2 zx(void) const
	{
		return YsVec2(value[2],value[0]);
	}
	/*! Returns a 2D vector by taking z- and y- components. */
	inline YsVec2 zy(void) const
	{
		return YsVec2(value[2],value[1]);
	}

	/*! Returns X component in float. */
	inline float xf(void) const   // 2001/04/17 : Don't make it const double &
	{
		return (float)value[0];
	}
	/*! Returns Y component in float. */
	inline float yf(void) const  // 2001/04/17 : Don't make it const double &
	{
		return (float)value[1];
	}
	/*! Returns Z component in float. */
	inline float zf(void) const  // 2001/04/17 : Don't make it const double &
	{
		return (float)value[2];
	}

	/*! Returns a constant double pointer, which can be given to a function like glVertex3dv. 
	    You can use this function when you want to explicitly want a double pointer
	    from this class rather than implicitly using the cast operator.
	*/
	inline const double *GetValue(void) const
	{
		return value;
	}
	/*! Sets x, y, and z components */
	inline void Set(const double &x,const double &y,const double &z)
	{
		value[0]=x;
		value[1]=y;
		value[2]=z;
	}
	/*! Sets x, y, and z components.  Strings are interpreted by atof. */
	void Set(const char x[],const char y[],const char z[]);

	/*! Sets X component */
	inline void SetX(const double &x)
	{
		value[0]=x;
	}
	/*! Sets Y component */
	inline void SetY(const double &y)
	{
		value[1]=y;
	}
	/*! Sets Z component */
	inline void SetZ(const double &z)
	{
		value[2]=z;
	}
	/*! Sets X and Y components taken from a YsVec2 object. 
	    Z coordinate is reset to zero.
	*/
	inline void SetXY(const YsVec2 &vec2)
	{
		value[0]=vec2[0];
		value[1]=vec2[1];
		value[2]=0.0;
	}
	/*! Sets X and Z components taken from a YsVec2 object. 
	    Y coordinate is reset to zero.
	*/
	inline void SetXZ(const YsVec2 &vec2)
	{
		value[0]=vec2[0];
		value[1]=0.0;
		value[2]=vec2[1];
	}
	/*! Sets x, y, and z components */
	inline void Set(const double v[3])
	{
		value[0]=v[0];
		value[1]=v[1];
		value[2]=v[2];
	}
	/*! Returns x, y, and z components to the given references. */
	inline void Get(double &x,double &y,double &z) const
	{
		x=value[0];
		y=value[1];
		z=value[2];
	}
	/*! Returns x*x+y*y+z*z */
	inline double GetSquareLength(void) const
	{
		return value[0]*value[0]+value[1]*value[1]+value[2]*value[2];
	}
	/*! Returns (x*x+y*y) */
	inline double GetSquareLengthXY(void) const
	{
		return (double)(value[0]*value[0]+value[1]*value[1]);
	}
	/*! Returns sqrt(x*x+z*z) */
	inline double GetSquareLengthXZ(void) const
	{
		return (double)(value[0]*value[0]+value[2]*value[2]);
	}
	/*! Returns sqrt(x*x+y*y) */
	inline double GetSquareLengthYZ(void) const
	{
		return (double)(value[1]*value[1]+value[2]*value[2]);
	}

	/*! Returns sqrt(x*x+y*y+z*z) */
	inline double GetLength(void) const
	{
		return (double)sqrt(GetSquareLength());
	}
	/*! Returns sqrt(x*x+y*y) */
	inline double GetLengthXY(void) const
	{
		return (double)sqrt(value[0]*value[0]+value[1]*value[1]);
	}
	/*! Returns sqrt(x*x+z*z) */
	inline double GetLengthXZ(void) const
	{
		return (double)sqrt(value[0]*value[0]+value[2]*value[2]);
	}
	/*! Returns sqrt(x*x+y*y) */
	inline double GetLengthYZ(void) const
	{
		return (double)sqrt(value[1]*value[1]+value[2]*value[2]);
	}

	/*! Returns greatest of fabs(x), fabs(y), and fabs(z) */
	inline double L0Norm(void) const
	{
		return YsGreater(YsGreater(fabs(value[0]),fabs(value[1])),fabs(value[2]));
	}
	/*! Returns YSTRUE if the vector is normalized or YSFALSE otherwise. */
	inline YSBOOL IsNormalized(void) const;
	/*! Normalizes the vector.  If it is successful, it returns YSOK.  If the vector is of zero length, it returns YSERR. */
	inline YSRESULT Normalize(void);

	/*! Returns YSTRUE if the vector is of zero length or YSFALSE otherwise. */
	inline YSBOOL IsZeroVector(void) const
	{
		if(GetSquareLength()<YsToleranceSqr)
		{
			return YSTRUE;
		}
		else
		{
			return YSFALSE;
		}
	}

	/*! This function returns YSTRUE if the projection of this coordinate 
	    on the line that passes through the given two end points is between 
	    the two end points or is lying on one of the end points.  It returns
	    YSFALSE otherwise.
	    \param p1 [In] One of the end points
	    \param p2 [In] The other end point
	    */
	YSBOOL IsInBetween(const YsVec3 &p1,const YsVec3 &p2) const;

	/*! This function returns YSTRUE if this coordinate is inside or on the boundary of the given bounding box
	    or YSFALSE otherwise.
	    \param min [In] Minimum x, y, and z of the bounding box
	    \param max [In] Maximum x, y, and z of the bounding box
	    */
	YSBOOL IsInsideBoundingBox(const YsVec3 &min,const YsVec3 &max) const;

	/*! This function returns YSTRUE if this coordinate is inside or on the surface of a 
	    sphere centered at (p1+p2)/2.0 with radius of |p2-p1|/2.0. */
	YSBOOL IsInsideDiameter(const YsVec3 &p1,const YsVec3 &p2) const;

	/*! This function returns distance from this coordinate to a line that passes through given points.
	    \param p1 [In] A point that the line passes through
	    \param p2 [In] A point that the line passes through */
	const double GetDistanceFromLine(const YsVec3 &p1,const YsVec3 &p2) const;

	/*! This function returns distance from this coordinate to a piece of line
	    \param p1 [In] A end point of the line
	    \param p2 [In] The other end point of the line */
	const double GetDistanceFromLineSegment(const YsVec3 &p1,const YsVec3 &p2) const;

	/*! This function returns the nearest point on the given line from this coordinate.
	    The line is defined by two points that the line passes through.
	    \param p1 [In] A point that the line passes through
	    \param p2 [In] A point that the line passes through */
	const YsVec3 GetNearestPointOnLine(const YsVec3 &p1,const YsVec3 &p2) const;


	/*! Returns an index to the axis of the longest-dimension */
	inline int GetLongestAxis(void) const
	{
		if(fabs(value[0])>fabs(value[1]) && fabs(value[0])>fabs(value[2]))
		{
			return 0;
		}
		else if(fabs(value[1])>fabs(value[2]))
		{
			return 1;
		}
		else
		{
			return 2;
		}
	}

	/*! Same as L0Norm */
	inline double GetLongestDimension(void) const
	{
		if(fabs(value[0])>fabs(value[1]) && fabs(value[0])>fabs(value[2]))
		{
			return fabs(value[0]);
		}
		else if(fabs(value[1])>fabs(value[2]))
		{
			return fabs(value[1]);
		}
		else
		{
			return fabs(value[2]);
		}
	}

	/*! Calculates and returns a vector that is perpendicular to
	    this vector.  The returned vector may NOT be a unit vector. */
	YsVec3 GetArbitraryPerpendicularVector(void) const;

	/*! Rotates the vector in XY plane (X goes to the right, Y goes up)
	    counter clockwise. */
	inline void RotateXY(const double &ang)
	{
		const double c=cos(ang);
		const double s=sin(ang);
		const double xx=value[0]*c-value[1]*s;
		const double yy=value[0]*s+value[1]*c;
		value[0]=(xx);
		value[1]=(yy);
	}
	/*! Rotates the vector in XY plane (X goes to the right, Y goes up)
	    clockwise. */
	inline void RotateYX(const double &ang)
	{
		RotateXY(-ang);
	}
	/*! Rotates the vector in XZ plane (X goes to the right, Z goes up)
	    counter clockwise. */
	inline void RotateXZ(const double &ang)
	{
		const double c=cos(ang);
		const double s=sin(ang);
		const double xx=value[0]*c-value[2]*s;
		const double zz=value[0]*s+value[2]*c;
		value[0]=(xx);
		value[2]=(zz);
	}
	/*! Rotates the vector in XZ plane (X goes to the right, Z goes up)
	    clockwise. */
	inline void RotateZX(const double &ang)
	{
		RotateXZ(-ang);
	}
	/*! Rotates the vector in YZ plane (Y goes to the right, Z goes up)
	    counter clockwise. */
	inline void RotateYZ(const double &ang)
	{
		const double c=cos(ang);
		const double s=sin(ang);
		const double yy=value[1]*c-value[2]*s;
		const double zz=value[1]*s+value[2]*c;
		value[1]=(yy);
		value[2]=(zz);
	}
	/*! Rotates the vector in YZ plane (Z goes to the right, Y goes up)
	    counter clockwise. */
	inline void RotateZY(const double &ang)
	{
		RotateYZ(-ang);
	}

	/*! Adds another 3D vector */
	inline const YsVec3 &operator+=(const YsVec3 &a)
	{
		value[0]+=a.value[0];
		value[1]+=a.value[1];
		value[2]+=a.value[2];
		return *this;
	}
	/*! Subtracts another 3D vector */
	inline const YsVec3 &operator-=(const YsVec3 &a)
	{
		value[0]-=a.value[0];
		value[1]-=a.value[1];
		value[2]-=a.value[2];
		return *this;
	}
	/*! Divides the vector by a scalar value. */
	inline const YsVec3 &operator/=(double a)
	{
		value[0]/=a;
		value[1]/=a;
		value[2]/=a;
		return *this;
	}
	/*! Multiplies the vector by a scalar value. */
	inline const YsVec3 &operator*=(double a)
	{
		value[0]*=a;
		value[1]*=a;
		value[2]*=a;
		return *this;
	}


public:
	/*! Returns the origin. */
	static inline YsVec3 Origin(void)
	{
		return YsVec3(0.0,0.0,0.0);
	}

	/*! Returns a normalized vector of v. */
	static inline YsVec3 UnitVector(const YsVec3 &v)
	{
		auto u=v;
		u.Normalize();
		return u;
	}
};

inline YSBOOL YsVec3::IsNormalized(void) const
{
	const double verify=GetSquareLength();
	if(YsSqr(1.0-YsTolerance)<=verify && verify<=YsSqr(1.0+YsTolerance))  // Modified 2000/11/10
	{
		return YSTRUE;
	}
	else
	{
		return YSFALSE;
	}
}

inline YSRESULT YsVec3::Normalize(void)
{
	double length;
	length=GetSquareLength();
	if(length>YsSqr(YsTolerance))
	{
		length=sqrt(length);
		value[0]/=length;
		value[1]/=length;
		value[2]/=length;
		return YSOK;
	}
	else
	{
		return YSERR;
	}
}

/*! An operator that returns the reverse vector. */
inline const YsVec3 operator-(const YsVec3 &a)
{
	return YsVec3(-a.x(),-a.y(),-a.z());
}

/*! An operator that retuns subtraction of vector b from vector a. */
inline const YsVec3 operator-(const YsVec3 &a,const YsVec3 &b)
{
	return YsVec3(a.x()-b.x(),a.y()-b.y(),a.z()-b.z());
}

/*! An operator that returns addition of vectors a and b. */
inline const YsVec3 operator+(const YsVec3 &a,const YsVec3 &b)
{
	return YsVec3(a.x()+b.x(),a.y()+b.y(),a.z()+b.z());
}

/*! An operator that returns a vector divided by a scalar value. */
inline const YsVec3 operator/(const YsVec3 &a,const double &b)
{
	return YsVec3(a.x()/b,a.y()/b,a.z()/b);
}

/*! An operator that returns a vector multiplied by a scalar value. */
inline const YsVec3 operator*(const YsVec3 &a,const double &b)
{
	return YsVec3(a.x()*b,a.y()*b,a.z()*b);
}

/*! An operator that returns a vector multiplied by a scalar value. */
inline const YsVec3 operator*(const double &b,const YsVec3 &a)
{
	return YsVec3(a.x()*b,a.y()*b,a.z()*b);
}

/*! An operator that returns an inner (dot) product of vectors a and b. */
inline double operator*(const YsVec3 &a,const YsVec3 &b)
{
	const double *u=a.GetValue(),*v=b.GetValue();
	return u[0]*v[0]+u[1]*v[1]+u[2]*v[2];  // Hoping the optimizer does something smart.
	// return a.x()*b.x()+a.y()*b.y()+a.z()*b.z();
}

/*! An operator that returns an outer (cross) product of vectors a and b. */
inline const YsVec3 operator^(const YsVec3 &a,const YsVec3 &b)
{
	// y1 z1 x1 y1
	// y2 z2 x2 y2

	// Known working set >>
	// const double x=a.y()*b.z()-a.z()*b.y();
	// const double y=a.z()*b.x()-a.x()*b.z();
	// const double z=a.x()*b.y()-a.y()*b.x();
	// return YsVec3(x,y,z);
	// Known working set <<

	const double *u=a.GetValue(),*v=b.GetValue();  // Hopeing the optimizer does something smart.
	const double newValue[3]=
	{
		u[1]*v[2]-u[2]*v[1],
		u[2]*v[0]-u[0]*v[2],
		u[0]*v[1]-u[1]*v[0]
	};
	return YsVec3(newValue[0],newValue[1],newValue[2]);
}

/*! An operator for comparing two vectors. */
inline int operator==(const YsVec3 &a,const YsVec3 &b)
{
	// Fixed 2000/11/10
	// return ((a-b).GetSquareLength()<YsTolerance);

	// Modified again 2000/11/10
	//if(YsEqual(a.x(),b.x())==YSTRUE &&
	//   YsEqual(a.y(),b.y())==YSTRUE &&
	//   YsEqual(a.z(),b.z())==YSTRUE)
	//{
	//	return 1;
	//}
	//else
	//{
	//	return 0;
	//}

	return ((a-b).GetSquareLength()<YsSqr(YsTolerance));  // Modified 2000/11/10
}

/*! An operator for comparing two vectors. */
inline int operator!=(const YsVec3 &a,const YsVec3 &b)
{
	// Fixed 2000/11/10
	// return ((a-b).GetSquareLength()>=YsTolerance);

	// Modified again 2000/11/10
	//if(YsEqual(a.x(),b.x())!=YSTRUE ||
	//   YsEqual(a.y(),b.y())!=YSTRUE ||
	//   YsEqual(a.z(),b.z())!=YSTRUE)
	//{
	//	return 1;
	//}
	//else
	//{
	//	return 0;
	//}

	return ((a-b).GetSquareLength()>=YsSqr(YsTolerance));  // Modified 2000/11/10
}


/*! This function returns one of the vectors v1 and v2, whichever longer. */
inline const YsVec3 &YsLonger(const YsVec3 &v1,const YsVec3 &v2)
{
	return (v1.GetSquareLength()>=v2.GetSquareLength() ? v1 : v2);
}


////////////////////////////////////////////////////////////
// Declaration /////////////////////////////////////////////
/*! A class that defines a rotation about an arbitrary axis. 
    Unless otherwise noted, the angle is in radian.
    \sa operator*(const YsRotation &rot,const YsVec3 &vec) */
class YsRotation
{
public:
	/*! Default constructor.  Does nothing. */
	inline YsRotation(){};

	/*! A constructor takes an axis (x,y,z) and an angle.
	    The given axis is normalized within the constructor
	    and therefore does not have to be a unit vector.
	    \param x [In] X coordinate of the axis
	    \param y [In] Y coordinate of the axis
	    \param z [In] Z coordinate of the axis
	    \param a [In] Angle in radian */
	YsRotation(double x,double y,double z,double a);

	/*! A constructor takes an axis and an angle.
	    The given axis is normalized within the constructor
	    and therefore does not have to be a unit vector.
	    \param ax [In] Axis
	    \param a [In] angle in radian */
	YsRotation(const YsVec3 &ax,double a);

	/*! A member function for setting an axis and an angle.
	    The given axis is normalized within the constructor
	    and therefore does not have to be a unit vector.
	    \param x [In] X coordinate of the axis
	    \param y [In] Y coordinate of the axis
	    \param z [In] Z coordinate of the axis
	    \param a [In] Angle in radian */
	inline void Set(double x,double y,double z,double a)
	{
		axis.Set(x,y,z);
		axis.Normalize();
		angle=a;
	}

	/*! A member function that returns the rotation axis and angle
	    in the given referendes. */
	inline void Get(double &x,double &y,double &z,double &a) const
	{
		x=axis.x();
		y=axis.y();
		z=axis.z();
		a=angle;
	}

	/*! A member function for setting an axis and an angle.
	    The given axis is normalized within the constructor
	    and therefore does not have to be a unit vector.
	    \param vec [In] Axis
	    \param a [In] Angle in radian */
	inline void Set(const YsVec3 &vec,double a)
	{
		axis=vec;
		axis.Normalize();
		angle=a;
	}

	/*! A member function for setting an axis and an angle.
	    The difference of this function from the Set function is
	    it does not normalize the given axis.  Therefore, if the
	    axis is already known to be a unit vector, this function
	    will be slightly faster than the Set function.
	    \param vec [In] Axis
	    \param a [In] angle in radian */
	inline void SetUnit(const YsVec3 &vec,double a)
	{
		axis=vec;
		angle=a;
	}

	/*! A member function that returns the rotation axis and angle
	    in the given referendes. */
	inline void Get(YsVec3 &vec,double &a) const
	{
		vec=axis;
		a=angle;
	}

	/*! Returns a constant reference to the axis. */
	inline const YsVec3 &GetAxis(void) const
	{
		return axis;
	}

	/*! Returns a constant reference to the angle. */
	inline const double &GetAngle(void) const
	{
		return angle;
	}

	/*! Sets the axis.  The given axis is normalized within this
	    function therefore does not have to be a unit vector. */
	inline void SetAxis(const YsVec3 &axs)
	{
		axis=axs;
		axis.Normalize();
	}

	/*! Sets the axis.
	    The difference from SetAxis function is this function does
	    not normalize the given axis, and the given axis needs to 
	    be a unit vector.  If the axis is already normalized, this
	    function is slightly faster than SetAxis function. */
	inline void SetUnitAxis(const YsVec3 &axs)
	{
		axis=axs;
	}

	/*! Sets the rotation angle. */
	inline void SetAngle(double a)
	{
		angle=a;
	}

	/*! Rotate the vector counter clockwise.
	    \param to [Out] Output vector
	    \param from [In] Input vector
	     */
	YSRESULT RotatePositive(YsVec3 &to,const YsVec3 &from) const;
	/*! Rotate the vector clockwise.
	    \param to [Out] Output vector
	    \param from [In] Input vector
	    */
	YSRESULT RotateNegative(YsVec3 &to,const YsVec3 &from) const;

	/*! Makes a rotation from unit vectors vecA to vecB. */
	inline void MakeAtoB(const YsVec3 &vecA,const YsVec3 &vecB)
	{
		YsVec3 axs=vecA^vecB;
		if(YSOK!=axs.Normalize()) // Means no rotation, or 180-degree rotation
		{
			if(0.0<vecA*vecB)
			{
				Set(YsVec3(1.0,0.0,0.0),0.0);
			}
			else
			{
				Set(YsVec3(1.0,0.0,0.0),YsPi);
			}
		}
		else
		{
			YsVec3 a=vecA,b=vecB;
			a.Normalize();
			b.Normalize();
			const double ang=acos(YsBound(a*b,-1.0,1.0));
			Set(axs,ang);
		}
	}

/// \cond
protected:
	double angle;
	YsVec3 axis;
/// \endcond
};

/*! An operator that returns a vector rotated counter clockwise by the given rotation. */
inline YsVec3 operator*(const YsRotation &rot,const YsVec3 &vec)
{
	YsVec3 r;
	rot.RotatePositive(r,vec);
	return r;
}

////////////////////////////////////////////////////////////
/*! A 3x3 matrix class 
    \sa operator*(const YsMatrix3x3 &a,const YsVec3 &b)
    \sa operator*(const YsMatrix3x3 &mat,const YsRotation &rot)
    \sa operator*(const YsRotation &rot,const YsMatrix3x3 &mat)
    \sa YsMatrixTemplate */
class YsMatrix3x3 : public YsMatrixTemplate <3,3>  // Body is written in ysmatrix3x3.cpp
{
public:
	/*! Returns the value at row=1 column=1 */
	inline const double &v11(void) const;
	/*! Returns the value at row=1 column=2 */
	inline const double &v12(void) const;
	/*! Returns the value at row=1 column=3 */
	inline const double &v13(void) const;
	/*! Returns the value at row=2 column=1 */
	inline const double &v21(void) const;
	/*! Returns the value at row=2 column=2 */
	inline const double &v22(void) const;
	/*! Returns the value at row=2 column=3 */
	inline const double &v23(void) const;
	/*! Returns the value at row=3 column=1 */
	inline const double &v31(void) const;
	/*! Returns the value at row=3 column=2 */
	inline const double &v32(void) const;
	/*! Returns the value at row=3 column=3 */
	inline const double &v33(void) const;

	/*! Sets the value at row=1 column=1 */
	inline void Set11(const double &v);
	/*! Sets the value at row=1 column=2 */
	inline void Set12(const double &v);
	/*! Sets the value at row=1 column=3 */
	inline void Set13(const double &v);
	/*! Sets the value at row=2 column=1 */
	inline void Set21(const double &v);
	/*! Sets the value at row=2 column=2 */
	inline void Set22(const double &v);
	/*! Sets the value at row=2 column=3 */
	inline void Set23(const double &v);
	/*! Sets the value at row=3 column=1 */
	inline void Set31(const double &v);
	/*! Sets the value at row=3 column=2 */
	inline void Set32(const double &v);
	/*! Sets the value at row=3 column=3 */
	inline void Set33(const double &v);

	/*! Default constructor.  If autoInit is YSFALSE, it does not
	    initialize the matrix, or the matrix will be initialized
	    to an identity matris. */
	inline YsMatrix3x3(YSBOOL autoInit=YSTRUE)
	{
		if(autoInit==YSTRUE)
		{
			Initialize();
		}
	}
	/*! Default copy constructor.
	    Since YsMatrix3x3 is a sub-class of YsMatrixTemplate <3,3>,
	    a reference to YsMatrix3x3 can also be given as the parameter. */
	inline YsMatrix3x3(const YsMatrixTemplate <3,3> &from)
	{
		Create(from.GetArray());
	}

	/*! An operator for multiplying two 3x3 matrices.
	    Since YsMatrix3x3 is a sub-class of YsMatrixTemplate <3,3>,
	    a reference to YsMatrix3x3 can also be given as the parameter. */
	inline const YsMatrix3x3 &operator*=(const YsMatrixTemplate <3,3> &mul)
	{
		YsMatrixTemplate <3,3> org;
		org.Create(dat);
		dat[0]=org.dat[0]*mul.dat[0]+org.dat[1]*mul.dat[3]+org.dat[2]*mul.dat[6];
		dat[1]=org.dat[0]*mul.dat[1]+org.dat[1]*mul.dat[4]+org.dat[2]*mul.dat[7];
		dat[2]=org.dat[0]*mul.dat[2]+org.dat[1]*mul.dat[5]+org.dat[2]*mul.dat[8];

		dat[3]=org.dat[3]*mul.dat[0]+org.dat[4]*mul.dat[3]+org.dat[5]*mul.dat[6];
		dat[4]=org.dat[3]*mul.dat[1]+org.dat[4]*mul.dat[4]+org.dat[5]*mul.dat[7];
		dat[5]=org.dat[3]*mul.dat[2]+org.dat[4]*mul.dat[5]+org.dat[5]*mul.dat[8];

		dat[6]=org.dat[6]*mul.dat[0]+org.dat[7]*mul.dat[3]+org.dat[8]*mul.dat[6];
		dat[7]=org.dat[6]*mul.dat[1]+org.dat[7]*mul.dat[4]+org.dat[8]*mul.dat[7];
		dat[8]=org.dat[6]*mul.dat[2]+org.dat[7]*mul.dat[5]+org.dat[8]*mul.dat[8];

		return *this;
	}

	/*! Creates an isotropic scaling matrix. */
	inline void CreateScaling(const double &sx,const double &sy,const double &sz)
	{
		Set11(sx);
		Set12(0.0);
		Set13(0.0);

		Set21(0.0);
		Set22(sy);
		Set23(0.0);

		Set31(0.0);
		Set32(0.0);
		Set33(sz);
	}

	/*! An operator for multiplying a rotation defined by the Euler angle. */
	const YsMatrix3x3 &operator*=(const class YsAtt3 &att);

	/*! An operator for multiplying a rotation. */
	const YsMatrix3x3 &operator*=(const YsRotation &rot)
	{
		Rotate(rot);
		return *this;
	}

	/*! Transforms an attitude Att with the matrix. */
	inline const YsAtt3 Mul(const YsAtt3 &Att) const;

	/*! Transforms an attitude Att with the matrix and returns it in NewAtt. */
	inline const YsAtt3 Mul(YsAtt3 &NewAtt,const YsAtt3 &Att) const;

	/*! A substitution operator. 
	    Since YsMatrix3x3 is a sub-class of YsMatrixTemplate <3,3>,
	    a reference to YsMatrix3x3 can also be given as the parameter. */
	inline const YsMatrix3x3 &operator=(const YsMatrixTemplate <3,3> &from)
	{
		Create(from.GetArray());
		return *this;
	}

	/*! Makes an identity matrix. */
	inline void Initialize(void)
	{
		LoadIdentity();
	}

	/*! Returns a matrix compatible with OpenGL */
	template <class realType>
	inline void GetOpenGlCompatibleMatrix(realType mat[16]) const
	{
		mat[ 0]=(realType)dat[0];
		mat[ 1]=(realType)dat[3];
		mat[ 2]=(realType)dat[6];
		mat[ 3]=(realType)0.0;

		mat[ 4]=(realType)dat[1];
		mat[ 5]=(realType)dat[4];
		mat[ 6]=(realType)dat[7];
		mat[ 7]=(realType)0.0;

		mat[ 8]=(realType)dat[2];
		mat[ 9]=(realType)dat[5];
		mat[10]=(realType)dat[8];
		mat[11]=(realType)0.0;

		mat[12]=(realType)0.0;
		mat[13]=(realType)0.0;
		mat[14]=(realType)0.0;
		mat[15]=(realType)1.0;
	}

	// See document/mat2rot.jpg
	/*! Extracts the rotation. */
	inline YSRESULT GetRotation(double &x,double &y,double &z,double &rad) const
	{
		double a,b,c;
		double d,e,f;
		double h,i,j;

		a=dat[0];
		b=dat[1];
		c=dat[2];
		d=dat[3];
		e=dat[4];
		f=dat[5];
		h=dat[6];
		i=dat[7];
		j=dat[8];

		YsVec3 v1,v2,v3,v;
		double l1,l2,l3,l;
		v1.Set(a-1,b,c);
		v2.Set(d,e-1,f);
		v3.Set(h,i,j-1);

		l1=v1.GetSquareLength();
		l2=v2.GetSquareLength();
		l3=v3.GetSquareLength();

		if(l1>l2 && l1>l3)
		{
			v=v1;
			l=l1;
		}
		else if(l2>l3)
		{
			v=v2;
			l=l2;
		}
		else
		{
			v=v3;
			l=l3;
		}

		if(l<=YsTolerance*YsTolerance)
		{
			x=0.0;
			y=1.0;
			z=0.0;
			rad=0.0;
			return YSOK;
		}

		v.Normalize();

		YsVec3 u;
		u=v;
		Mul(u,v);
		u.Normalize();

		double inner;
		YsVec3 outer;
		inner=v*u;
		outer=v^u;
		outer.Normalize();

		rad=acos(inner);
		outer.Get(x,y,z);
		return YSOK;
	}

	/*! Extracts the rotation. */
	inline YSRESULT GetRotation(YsRotation &rot) const
	{
		double x,y,z,rad;
		if(GetRotation(x,y,z,rad)==YSOK)
		{
			rot.Set(x,y,z,rad);
		}
		return YSERR;
	}

	/*! Multiplies a counter clockwise rotation in ZY plane.
	    (Z goes to the right, Y goes up. 
	  \param a [In] Angle in radian */
	inline void RotateZY(const double &a)
	{
		double s,c;
		s=sin(a);
		c=cos(a);

		double m1,m2;
		m1=dat[1];
		m2=dat[2];
		Set12( c*m1-s*m2);
		Set13( s*m1+c*m2);
		m1=dat[4];
		m2=dat[5];
		Set22( c*m1-s*m2);
		Set23( s*m1+c*m2);
		m1=dat[7];
		m2=dat[8];
		Set32( c*m1-s*m2);
		Set33( s*m1+c*m2);
	}

	/*! Multiplies a clockwise rotation in ZY plane.
	    (Z goes to the right, Y goes up. )
	  \param a [In] Angle in radian */
	inline void RotateYZ(const double &a)
	{
		RotateZY(-a);
	}

	/*! Multiplies a counter clockwise rotation in XZ plane.
	    (X goes to the right, Z goes up. )
	  \param a [In] Angle in radian */
	void RotateXZ(const double &a)
	{
		// XZ Plane
		double s,c;
		s=sin(a);
		c=cos(a);

		double m1,m2;
		m1=dat[0];
		m2=dat[2];
		Set11( c*m1+s*m2);
		Set13(-s*m1+c*m2);
		m1=dat[3];
		m2=dat[5];
		Set21( c*m1+s*m2);
		Set23(-s*m1+c*m2);
		m1=dat[6];
		m2=dat[8];
		Set31( c*m1+s*m2);
		Set33(-s*m1+c*m2);
	}
	/*! Multiplies a clockwise rotation in XZ plane.
	    (X goes to the right, Z goes up. )
	  \param a [In] Angle in radian */
	void RotateZX(const double &a)
	{
		RotateXZ(-a);
	}

	/*! Multiplies a counter clockwise rotation in XY plane.
	    (X goes to the right, Y goes up. )
	  \param a [In] Angle in radian */
	void RotateXY(const double &a)
	{
		// XY Plane
		double s,c;
		s=sin(a);
		c=cos(a);

		double m1,m2;
		m1=dat[0];
		m2=dat[1];
		Set11( c*m1+s*m2);
		Set12(-s*m1+c*m2);
		m1=dat[3];
		m2=dat[4];
		Set21( c*m1+s*m2);
		Set22(-s*m1+c*m2);
		m1=dat[6];
		m2=dat[7];
		Set31( c*m1+s*m2);
		Set32(-s*m1+c*m2);
	}

	/*! Multiplies a clockwise rotation in XY plane.
	    (X goes to the right, Y goes up. )
	  \param a [In] Angle in radian */
	void RotateYX(const double a)
	{
		RotateXY(-a);
	}

	/*! Multiplies a counter clockwise rotation in about the
	    given axis.
	    The given axis does not have to be a unit vector.
	  \param x [In] X component of the axis
	  \param y [In] Y component of the axis
	  \param z [In] Z component of the axis
	  \param a [In] Angle in radian */
	inline void Rotate(const double &x,const double &y,const double &z,const double &a)
	{
		double l;
		l=x*x+y*y+z*z;
		if(l==0.0)
		{
			YsErrOut("YsMatrix3x3::Rotate\n  Zero Vector is specified for Rotation Axis\n");
			return;
		}

		double hdg,pch;

		if(l<=0.99F || 1.01F<=l)
		{
			l=sqrt(l);
			hdg=((x!=0.0 || z!=0.0) ? atan2(-x/l,z/l) : 0.0);
			pch=asin(y/l);
		}
		else
		{
			hdg=((x!=0.0 || z!=0.0) ? atan2(-x,z) : 0.0);
			pch=asin(y);
		}

		RotateXZ(hdg);
		RotateZY(pch);
		RotateXY(a);
		RotateZY(-pch);
		RotateXZ(-hdg);   //
	}

	/*! Multiplies the given rotation */
	void Rotate(const class YsRotation &rot);

	/*! Multiplies a rotation defined by the given Euler angle */
	void Rotate(const class YsAtt3 &att);

	/*! Multiplies a scaling transformation.
	  \param x [In] Scaling factor in X axis
	  \param y [In] Scaling factor in Y axis
	  \param z [In] Scaling factor in Z axis */
	inline void Scale(const double &x,const double &y,const double &z)
	{
		dat[0]*=x;
		dat[3]*=x;
		dat[6]*=x;

		dat[1]*=y;
		dat[4]*=y;
		dat[7]*=y;

		dat[2]*=z;
		dat[5]*=z;
		dat[8]*=z;
	}

	/*! Multiplies a scaling transformation for X coordinate. */
	inline void ScaleX(const double &sx)
	{
		dat[0]*=sx;
		dat[3]*=sx;
		dat[6]*=sx;
	}

	/*! Multiplies a scaling transformation for Y coordinate. */
	inline void ScaleY(const double &sy)
	{
		dat[1]*=sy;
		dat[4]*=sy;
		dat[7]*=sy;
	}

	/*! Multiplies a scaling transformation for Z coordinate. */
	inline void ScaleZ(const double &sz)
	{
		dat[2]*=sz;
		dat[5]*=sz;
		dat[8]*=sz;
	}

	/*! Multiplies a scaling transformation. */
	inline void Scale(const YsVec3 &sca)
	{
		dat[0]*=sca.x();
		dat[3]*=sca.x();
		dat[6]*=sca.x();

		dat[1]*=sca.y();
		dat[4]*=sca.y();
		dat[7]*=sca.y();

		dat[2]*=sca.z();
		dat[5]*=sca.z();
		dat[8]*=sca.z();
	}

	/*! Multiplies a given vector with this matrix 
	  \param dst [Out] Output vector
	  \param src [In] Input vector
	*/
	inline YSRESULT Mul(YsVec3 &dst,const YsVec3 &src) const
	{
		double x,y,z;
		x=dat[0]*src.x()+dat[1]*src.y()+dat[2]*src.z();
		y=dat[3]*src.x()+dat[4]*src.y()+dat[5]*src.z();
		z=dat[6]*src.x()+dat[7]*src.y()+dat[8]*src.z();
		dst.Set(x,y,z);
		return YSOK;
	}

	/*! Multiplise a given vector with an inverse of this matrix
	  \param dst [Out] Output vector
	  \param src [In] Input vector
	*/
	inline YSRESULT MulInverse(YsVec3 &dst,const YsVec3 &src) const
	{
		return YsMulInverse3x3(&dst[0],GetArray(),src);
	}

	/*! Multiplies a given vector with a transpose of this matrix.
	  \param dst [Out] Output vector
	  \param src [In] Input vector
	*/
	YSRESULT MulTranspose(YsVec3 &dst,const YsVec3 &src) const;  // <- Written in ysmatrix3x3.cpp

	/*! Multiplies a given vector with a transpose of this matrix.
	  \param src [In] Input vector
	*/
	YsVec3 MulTranspose(const YsVec3 &src) const;  // <- Written in ysmatrix3x3.cpp

	/*! Makes a rotation matrix that rotates a the given vector 
	    to become parallel to the Z axis.  This function is useful
	    for projecting a point to a plane. */
	YSRESULT MakeToXY(const YsVec3 &zDir);

	/*! Multiplies this matrix to 'incoming', and returns x and y components in a YsVec2. */
	inline const YsVec2 ToXY(const YsVec3 &incoming)
	{
		const double *src=incoming;
		const double x=dat[0]*src[0]+dat[1]*src[1]+dat[2]*src[2];
		const double y=dat[3]*src[0]+dat[4]*src[1]+dat[5]*src[2];
		return YsVec2(x,y);
	}

	/*! Returns a column vector.  Parameter c (column) must be 1<=c<=3 */
	inline YsVec3 GetColumnVector(const int c) const
	{
		auto i=c-1;
		return YsVec3(dat[i],dat[i+3],dat[i+6]);
	}

	/*! Sets a column vector.  Parameter c (column) must be 1<=c<=3 */
	inline void SetColumnVector(const int c,const YsVec3 &v)
	{
		auto i=c-1;
		dat[i  ]=v[0];
		dat[i+3]=v[1];
		dat[i+6]=v[2];
	}
};


inline const double &YsMatrix3x3::v11(void) const
{
	return dat[0];
}

inline const double &YsMatrix3x3::v12(void) const
{
	return dat[1];
}

inline const double &YsMatrix3x3::v13(void) const
{
	return dat[2];
}

inline const double &YsMatrix3x3::v21(void) const
{
	return dat[3];
}

inline const double &YsMatrix3x3::v22(void) const
{
	return dat[4];
}

inline const double &YsMatrix3x3::v23(void) const
{
	return dat[5];
}

inline const double &YsMatrix3x3::v31(void) const
{
	return dat[6];
}

inline const double &YsMatrix3x3::v32(void) const
{
	return dat[7];
}

inline const double &YsMatrix3x3::v33(void) const
{
	return dat[8];
}

inline void YsMatrix3x3::Set11(const double &v)
{
	dat[0]=v;
}

inline void YsMatrix3x3::Set12(const double &v)
{
	dat[1]=v;
}

inline void YsMatrix3x3::Set13(const double &v)
{
	dat[2]=v;
}

inline void YsMatrix3x3::Set21(const double &v)
{
	dat[3]=v;
}

inline void YsMatrix3x3::Set22(const double &v)
{
	dat[4]=v;
}

inline void YsMatrix3x3::Set23(const double &v)
{
	dat[5]=v;
}

inline void YsMatrix3x3::Set31(const double &v)
{
	dat[6]=v;
}

inline void YsMatrix3x3::Set32(const double &v)
{
	dat[7]=v;
}

inline void YsMatrix3x3::Set33(const double &v)
{
	dat[8]=v;
}


/*! An operator that returns a vector rotated by the 3x3 matrix. */
inline const YsVec3 operator*(const YsMatrix3x3 &a,const YsVec3 &b)
{
	YsVec3 c;
	a.Mul(c,b);
	return c;
}

/*! An operator that returns a matrix multiplied by a rotation. */
inline const YsMatrix3x3 operator*(const YsMatrix3x3 &mat,const YsRotation &rot)
{
	YsMatrix3x3 r(mat);
	r.Rotate(rot);
	return r;
}

/*! An operator that returns a matrix multiplied by a rotation. */
inline const YsMatrix3x3 operator*(const YsRotation &rot,const YsMatrix3x3 &mat)
{
	YsMatrix3x3 r;
	r.Rotate(rot);
	return r*mat;
}

////////////////////////////////////////////////////////////

/*! A 4x4 matrix class 
    \sa operator*(const YsMatrix4x4 &a,const YsVec3 &b)
    \sa operator*(const YsMatrix4x4 &a,const YsMatrix3x3 &b)
    \sa operator*(const YsMatrix3x3 &a,const YsMatrix4x4 &b)
    \sa operator*(const YsMatrix4x4 &mat,const YsRotation &rot)
    \sa operator*(const YsRotation &rot,const YsMatrix4x4 &mat)
    \sa YsMatrixTemplate */
class YsMatrix4x4 : public YsMatrixTemplate <4,4> // body is written in ysmatrix4x4.cpp
{
public:
	using YsMatrixTemplate <4,4>::Set;

	/*! Default constructor.  If autoInit is YSFALSE, it does not
	    initialize the matrix, or the matrix will be initialized
	    to an identity matris. */
	inline YsMatrix4x4(YSBOOL autoInit=YSTRUE)
	{
		if(autoInit==YSTRUE)
		{
			Initialize();
		}
	}

	/*! Default copy constructor.
	    Since YsMatrix3x3 is a sub-class of YsMatrixTemplate <4,4>,
	    a reference to YsMatrix4x4 can also be given as the parameter. */
	inline YsMatrix4x4(const YsMatrixTemplate <4,4> &from)
	{
		Create(from.GetArray());
	}


	/*! Returns the value at row=1 column=1 */
	inline const double &v11(void) const;
	/*! Returns the value at row=1 column=2 */
	inline const double &v12(void) const;
	/*! Returns the value at row=1 column=3 */
	inline const double &v13(void) const;
	/*! Returns the value at row=1 column=4 */
	inline const double &v14(void) const;
	/*! Returns the value at row=2 column=1 */
	inline const double &v21(void) const;
	/*! Returns the value at row=2 column=2 */
	inline const double &v22(void) const;
	/*! Returns the value at row=2 column=3 */
	inline const double &v23(void) const;
	/*! Returns the value at row=2 column=4 */
	inline const double &v24(void) const;
	/*! Returns the value at row=3 column=1 */
	inline const double &v31(void) const;
	/*! Returns the value at row=3 column=2 */
	inline const double &v32(void) const;
	/*! Returns the value at row=3 column=3 */
	inline const double &v33(void) const;
	/*! Returns the value at row=3 column=4 */
	inline const double &v34(void) const;
	/*! Returns the value at row=4 column=1 */
	inline const double &v41(void) const;
	/*! Returns the value at row=4 column=2 */
	inline const double &v42(void) const;
	/*! Returns the value at row=4 column=3 */
	inline const double &v43(void) const;
	/*! Returns the value at row=4 column=4 */
	inline const double &v44(void) const;

	/*! Sets the value at row=1 column=1 */
	inline void Set11(const double &v);
	/*! Sets the value at row=1 column=2 */
	inline void Set12(const double &v);
	/*! Sets the value at row=1 column=3 */
	inline void Set13(const double &v);
	/*! Sets the value at row=1 column=4 */
	inline void Set14(const double &v);
	/*! Sets the value at row=2 column=1 */
	inline void Set21(const double &v);
	/*! Sets the value at row=2 column=2 */
	inline void Set22(const double &v);
	/*! Sets the value at row=2 column=3 */
	inline void Set23(const double &v);
	/*! Sets the value at row=2 column=4 */
	inline void Set24(const double &v);
	/*! Sets the value at row=3 column=1 */
	inline void Set31(const double &v);
	/*! Sets the value at row=3 column=2 */
	inline void Set32(const double &v);
	/*! Sets the value at row=3 column=3 */
	inline void Set33(const double &v);
	/*! Sets the value at row=3 column=4 */
	inline void Set34(const double &v);
	/*! Sets the value at row=4 column=1 */
	inline void Set41(const double &v);
	/*! Sets the value at row=4 column=2 */
	inline void Set42(const double &v);
	/*! Sets the value at row=4 column=3 */
	inline void Set43(const double &v);
	/*! Sets the value at row=4 column=4 */
	inline void Set44(const double &v);


	/*! An operator for multiplying two 4x4 matrices.
	    Since YsMatrix4x4 is a sub-class of YsMatrixTemplate <4,4>,
	    a reference to YsMatrix4x4 can also be given as the parameter. */
	inline const YsMatrix4x4 &operator*=(const YsMatrixTemplate <4,4> &mul)
	{
		YsMatrixTemplate <4,4> org;
		org.Create(dat);
		dat[ 0]=org.dat[ 0]*mul.dat[ 0]+org.dat[ 1]*mul.dat[ 4]+org.dat[ 2]*mul.dat[ 8]+org.dat[ 3]*mul.dat[12];
		dat[ 1]=org.dat[ 0]*mul.dat[ 1]+org.dat[ 1]*mul.dat[ 5]+org.dat[ 2]*mul.dat[ 9]+org.dat[ 3]*mul.dat[13];
		dat[ 2]=org.dat[ 0]*mul.dat[ 2]+org.dat[ 1]*mul.dat[ 6]+org.dat[ 2]*mul.dat[10]+org.dat[ 3]*mul.dat[14];
		dat[ 3]=org.dat[ 0]*mul.dat[ 3]+org.dat[ 1]*mul.dat[ 7]+org.dat[ 2]*mul.dat[11]+org.dat[ 3]*mul.dat[15];

		dat[ 4]=org.dat[ 4]*mul.dat[ 0]+org.dat[ 5]*mul.dat[ 4]+org.dat[ 6]*mul.dat[ 8]+org.dat[ 7]*mul.dat[12];
		dat[ 5]=org.dat[ 4]*mul.dat[ 1]+org.dat[ 5]*mul.dat[ 5]+org.dat[ 6]*mul.dat[ 9]+org.dat[ 7]*mul.dat[13];
		dat[ 6]=org.dat[ 4]*mul.dat[ 2]+org.dat[ 5]*mul.dat[ 6]+org.dat[ 6]*mul.dat[10]+org.dat[ 7]*mul.dat[14];
		dat[ 7]=org.dat[ 4]*mul.dat[ 3]+org.dat[ 5]*mul.dat[ 7]+org.dat[ 6]*mul.dat[11]+org.dat[ 7]*mul.dat[15];

		dat[ 8]=org.dat[ 8]*mul.dat[ 0]+org.dat[ 9]*mul.dat[ 4]+org.dat[10]*mul.dat[ 8]+org.dat[11]*mul.dat[12];
		dat[ 9]=org.dat[ 8]*mul.dat[ 1]+org.dat[ 9]*mul.dat[ 5]+org.dat[10]*mul.dat[ 9]+org.dat[11]*mul.dat[13];
		dat[10]=org.dat[ 8]*mul.dat[ 2]+org.dat[ 9]*mul.dat[ 6]+org.dat[10]*mul.dat[10]+org.dat[11]*mul.dat[14];
		dat[11]=org.dat[ 8]*mul.dat[ 3]+org.dat[ 9]*mul.dat[ 7]+org.dat[10]*mul.dat[11]+org.dat[11]*mul.dat[15];

		dat[12]=org.dat[12]*mul.dat[ 0]+org.dat[13]*mul.dat[ 4]+org.dat[14]*mul.dat[ 8]+org.dat[15]*mul.dat[12];
		dat[13]=org.dat[12]*mul.dat[ 1]+org.dat[13]*mul.dat[ 5]+org.dat[14]*mul.dat[ 9]+org.dat[15]*mul.dat[13];
		dat[14]=org.dat[12]*mul.dat[ 2]+org.dat[13]*mul.dat[ 6]+org.dat[14]*mul.dat[10]+org.dat[15]*mul.dat[14];
		dat[15]=org.dat[12]*mul.dat[ 3]+org.dat[13]*mul.dat[ 7]+org.dat[14]*mul.dat[11]+org.dat[15]*mul.dat[15];

		return *this;
	}

	/*! An operator for multiplying YsMatrix3x3 into 4x4 matrices on the right.
	    Since YsMatrix4x4 is a sub-class of YsMatrixTemplate <4,4>,
	    a reference to YsMatrix4x4 can also be given as the parameter. */
	inline const YsMatrix4x4 &operator*=(const YsMatrixTemplate <3,3> &mul)
	{
		YsMatrixTemplate <4,4> org;
		org.Create(dat);
		dat[ 0]=org.dat[ 0]*mul.dat[ 0]+org.dat[ 1]*mul.dat[ 3]+org.dat[ 2]*mul.dat[ 6];
		dat[ 1]=org.dat[ 0]*mul.dat[ 1]+org.dat[ 1]*mul.dat[ 4]+org.dat[ 2]*mul.dat[ 7];
		dat[ 2]=org.dat[ 0]*mul.dat[ 2]+org.dat[ 1]*mul.dat[ 5]+org.dat[ 2]*mul.dat[ 8];
		dat[ 3]=                                                                        org.dat[ 3];

		dat[ 4]=org.dat[ 4]*mul.dat[ 0]+org.dat[ 5]*mul.dat[ 3]+org.dat[ 6]*mul.dat[ 6];
		dat[ 5]=org.dat[ 4]*mul.dat[ 1]+org.dat[ 5]*mul.dat[ 4]+org.dat[ 6]*mul.dat[ 7];
		dat[ 6]=org.dat[ 4]*mul.dat[ 2]+org.dat[ 5]*mul.dat[ 5]+org.dat[ 6]*mul.dat[ 8];
		dat[ 7]=                                                                        org.dat[ 7];

		dat[ 8]=org.dat[ 8]*mul.dat[ 0]+org.dat[ 9]*mul.dat[ 3]+org.dat[10]*mul.dat[ 6];
		dat[ 9]=org.dat[ 8]*mul.dat[ 1]+org.dat[ 9]*mul.dat[ 4]+org.dat[10]*mul.dat[ 7];
		dat[10]=org.dat[ 8]*mul.dat[ 2]+org.dat[ 9]*mul.dat[ 5]+org.dat[10]*mul.dat[ 8];
		dat[11]=                                                                        org.dat[11];

		dat[12]=org.dat[12]*mul.dat[ 0]+org.dat[13]*mul.dat[ 3]+org.dat[14]*mul.dat[ 6];
		dat[13]=org.dat[12]*mul.dat[ 1]+org.dat[13]*mul.dat[ 4]+org.dat[14]*mul.dat[ 7];
		dat[14]=org.dat[12]*mul.dat[ 2]+org.dat[13]*mul.dat[ 5]+org.dat[14]*mul.dat[ 8];
		dat[15]=                                                                        org.dat[15];

		return *this;
	}

	/*! Creates an isotropic scaling matrix. */
	inline void CreateScaling(const double &sx,const double &sy,const double &sz)
	{
		Set11(sx);
		Set12(0.0);
		Set13(0.0);
		Set14(0.0);

		Set21(0.0);
		Set22(sy);
		Set23(0.0);
		Set24(0.0);

		Set31(0.0);
		Set32(0.0);
		Set33(sz);
		Set34(0.0);

		Set41(0.0);
		Set42(0.0);
		Set43(0.0);
		Set44(1.0);
	}

	/*! Creates a translation matrix. */
	inline void CreateTranslation(const double &dx,const double &dy,const double &dz)
	{
		Set11(1.0);
		Set12(0.0);
		Set13(0.0);
		Set14(0.0);

		Set21(0.0);
		Set22(1.0);
		Set23(0.0);
		Set24(0.0);

		Set31(0.0);
		Set32(0.0);
		Set33(1.0);
		Set34(0.0);

		Set41(dx);
		Set42(dy);
		Set43(dz);
		Set44(1.0);
	}

	/*! Creates a translation matrix. */
	inline void CreateTranslation(const YsVec3 &d)
	{
		CreateTranslation(d.x(),d.y(),d.z());
	}

	/*! Default copy operator */
	inline const YsMatrix4x4 &operator=(const YsMatrixTemplate <4,4> &from)
	{
		Create(from.GetArray());
		return *this;
	}

	/*! Copy operator that copies a 3x3 matrix to a 4x4 matrix.
	    All elements on row 4 and column 4 are set to zero, except
	    the element at row=4 and column=4, which will be set to 1.0 */
	inline const YsMatrix4x4 &operator=(const YsMatrixTemplate <3,3> &from)
	{
		Set11(from.dat[0]);
		Set12(from.dat[1]);
		Set13(from.dat[2]);

		Set21(from.dat[3]);
		Set22(from.dat[4]);
		Set23(from.dat[5]);

		Set31(from.dat[6]);
		Set32(from.dat[7]);
		Set33(from.dat[8]);

		Set14(0.0);
		Set24(0.0);
		Set34(0.0);

		Set41(0.0);
		Set42(0.0);
		Set43(0.0);

		Set44(1.0);

		return *this;
	}

	/*! Copy operator that copies a general purpose matrix class to this matrix. */
	const YsMatrix4x4 &operator=(const YsMatrix &mat);

	/*! An operator that miltiplies a rotational matrix defined by the given Euler angle to this matrix. */
	const YsMatrix4x4 &operator*=(const class YsAtt3 &att);

	/*! An operator that multiplies a rotation to this matrix. */
	const YsMatrix4x4 &operator*=(const YsRotation &rot)
	{
		Rotate(rot);
		return *this;
	}

	/*! Makes an identity matrix. */
	inline void Initialize(void)
	{
		LoadIdentity();
	}

	/*! Multiplies the following transformation. 
         this->Translate(pos);
         this->RotateXZ(att.h());
         this->RotateZY(att.p());
         this->RotateXY(att.b());
	   */
	inline void Multiply(const class YsVec3 &pos,const class YsAtt3 &att);

	/*! Multiplies the following transformation. 
         this->RotateXY(-att.b());
         this->RotateZY(-att.p());
         this->RotateXZ(-att.h());
         this->Translate(-pos);
	   */
	inline void MultiplyInverse(const class YsVec3 &pos,const class YsAtt3 &att);

	/*! Multiplies a counter clockwise rotation in ZY plane.
	    (Z goes to the right and Y goes up. 
	    \param a [In] Angle in radian */
	inline void RotateZY(const double &a)
	{
		// ZY Plane
		double s,c;
		s=sin(a);
		c=cos(a);

		//  m11 m12 m13 m14     1     0     0    0
		//  m21 m22 m23 m24  x  0     c     s    0
		//  m31 m32 m33 m34     0    -s     c    0
		//  m41 m42 m43 m44     0     0     0    1
		// Only 2nd and 3rd column changes
		double m1,m2;
		m1=dat[ 1];
		m2=dat[ 2];
		Set12( c*m1-s*m2);
		Set13( s*m1+c*m2);
		m1=dat[ 5];
		m2=dat[ 6];
		Set22( c*m1-s*m2);
		Set23( s*m1+c*m2);
		m1=dat[ 9];
		m2=dat[10];
		Set32( c*m1-s*m2);
		Set33( s*m1+c*m2);
		m1=dat[13];
		m2=dat[14];
		Set42( c*m1-s*m2);
		Set43( s*m1+c*m2);
	}
	/*! Multiplies a clockwise rotation in ZY plane.
	    (Z goes to the right and Y goes up. 
	    \param a [In] Angle in radian */
	inline void RotateYZ(const double &a)
	{
		RotateZY(-a);
	}

	/*! Multiplies a counter clockwise rotation in XZ plane.
	    (X goes to the right and Z goes up. 
	    \param a [In] Angle in radian */
	inline void RotateXZ(const double &a)
	{
		// XZ Plane
		double s,c;
		s=sin(a);
		c=cos(a);

		//  m11 m12 m13 m14     c     0    -s    0
		//  m21 m22 m23 m24  x  0     1     0    0
		//  m31 m32 m33 m34     s     0     c    0
		//  m41 m42 m43 m44     0     0     0    1
		// Only 1st and 3rd column changes
		double m1,m2;
		m1=dat[ 0];
		m2=dat[ 2];
		Set11( c*m1+s*m2);
		Set13(-s*m1+c*m2);
		m1=dat[ 4];
		m2=dat[ 6];
		Set21( c*m1+s*m2);
		Set23(-s*m1+c*m2);
		m1=dat[ 8];
		m2=dat[10];
		Set31( c*m1+s*m2);
		Set33(-s*m1+c*m2);
		m1=dat[12];
		m2=dat[14];
		Set41( c*m1+s*m2);
		Set43(-s*m1+c*m2);
	}
	/*! Multiplies a clockwise rotation in XZ plane.
	    (X goes to the right and Z goes up. 
	    \param a [In] Angle in radian */
	inline void RotateZX(const double &a)
	{
		RotateXZ(-a);
	}

	/*! Multiplies a counter clockwise rotation in XY plane.
	    (X goes to the right and Y goes up. 
	    \param a [In] Angle in radian */
	inline void RotateXY(const double &a)
	{
		// XY Plane
		double s,c;
		s=sin(a);
		c=cos(a);

		//  m11 m12 m13 m14     c    -s     0    0
		//  m21 m22 m23 m24  x  s     c     0    0
		//  m31 m32 m33 m34     0     0     1    0
		//  m41 m42 m43 m44     0     0     0    1
		// Only 1st and 2nd column changes
		double m1,m2;
		m1=dat[ 0];
		m2=dat[ 1];
		Set11( c*m1+s*m2);
		Set12(-s*m1+c*m2);
		m1=dat[ 4];
		m2=dat[ 5];
		Set21( c*m1+s*m2);
		Set22(-s*m1+c*m2);
		m1=dat[ 8];
		m2=dat[ 9];
		Set31( c*m1+s*m2);
		Set32(-s*m1+c*m2);
		m1=dat[12];
		m2=dat[13];
		Set41( c*m1+s*m2);
		Set42(-s*m1+c*m2);
	}
	/*! Multiplies a clockwise rotation in XY plane.
	    (X goes to the right and Y goes up. 
	    \param a [In] Angle in radian */
	inline void RotateYX(const double &a)
	{
		RotateXY(-a);
	}

	/*! Multiplies a counter clockwise rotation about the given axis.
	    \param x [In] X component of the axis
	    \param y [In] Y component of the axis
	    \param z [In] Z component of the axis
	    \param a [In] Angle in radian */
	inline void Rotate(const double &x,const double &y,const double &z,const double &a)
	{
		double l;
		l=x*x+y*y+z*z;
		if(l==0.0)
		{
			YsErrOut("YsMatrix4x4::Rotate\n  Zero Vector is specified for Rotation Axis\n");
			return;
		}

		double hdg,pch;

		if(l<=0.99F || 1.01F<=l)
		{
			l=sqrt(l);
			hdg=((x!=0.0 || z!=0.0) ? atan2(-x/l,z/l) : 0.0);
			pch=asin(y/l);
		}
		else
		{
			hdg=((x!=0.0 || z!=0.0) ? atan2(-x,z) : 0.0);
			pch=asin(y);
		}

		RotateXZ(hdg);
		RotateZY(pch);
		RotateXY(a);
		RotateZY(-pch);
		RotateXZ(-hdg);   //
	}

	/*! Multiplies a counter clockwise rotation. */
	void Rotate(const class YsRotation &rot);

	/*! Multiplies a rotation defined by the given Euler angle. */
	void Rotate(const class YsAtt3 &att);

	/*! Multiplies a translational matrix. */
	inline void Translate(const double &x,const double &y,const double &z)
	{
		//  m11 m12 m13 m14     1     0     0    x
		//  m21 m22 m23 m24  x  0     1     0    y
		//  m31 m32 m33 m34     0     0     1    z
		//  m41 m42 m43 m44     0     0     0    1
		Set14(dat[ 0]*x+dat[ 1]*y+dat[ 2]*z+dat[ 3]);
		Set24(dat[ 4]*x+dat[ 5]*y+dat[ 6]*z+dat[ 7]);
		Set34(dat[ 8]*x+dat[ 9]*y+dat[10]*z+dat[11]);
		Set44(dat[12]*x+dat[13]*y+dat[14]*z+dat[15]);
	}

	/*! Multiplies a translational matrix. */
	inline void Translate(const YsVec3 &vec)
	{
		Translate(vec.x(),vec.y(),vec.z());
	}

	/*! Multiplies a scaling matrix. */
	inline void Scale(const double &x,const double &y,const double &z)
	{
		dat[ 0]*=x;
		dat[ 4]*=x;
		dat[ 8]*=x;
		dat[12]*=x;
		dat[ 1]*=y;
		dat[ 5]*=y;
		dat[ 9]*=y;
		dat[13]*=y;
		dat[ 2]*=z;
		dat[ 6]*=z;
		dat[10]*=z;
		dat[14]*=z;
	}

	/*! Multiplies a scaling matrix. */
	inline void Scale(const YsVec3 &sca)
	{
		Scale(sca.x(),sca.y(),sca.z());
	}

	/*! Multiplies a scaling matrix for X coordinate. */
	inline void ScaleX(const double &sx)
	{
		dat[ 0]*=sx;
		dat[ 4]*=sx;
		dat[ 8]*=sx;
		dat[12]*=sx;
	}

	/*! Multiplies a scaling matrix for Y coordinate. */
	inline void ScaleY(const double &sy)
	{
		dat[ 1]*=sy;
		dat[ 5]*=sy;
		dat[ 9]*=sy;
		dat[13]*=sy;
	}

	/*! Multiplies a scaling matrix for Z coordinate. */
	inline void ScaleZ(const double &sz)
	{
		dat[ 2]*=sz;
		dat[ 6]*=sz;
		dat[10]*=sz;
		dat[14]*=sz;
	}

	/*! Transforms a 4D vector 
	    \param result [Out] Output vector
	    \param vec [In] Input vector
	    */
	inline void Mul(double result[4],const double vec[4]) const
	{
		double x,y,z,w;
		x=dat[ 0]*vec[0]+dat[ 1]*vec[1]+dat[ 2]*vec[2]+dat[ 3]*vec[3];
		y=dat[ 4]*vec[0]+dat[ 5]*vec[1]+dat[ 6]*vec[2]+dat[ 7]*vec[3];
		z=dat[ 8]*vec[0]+dat[ 9]*vec[1]+dat[10]*vec[2]+dat[11]*vec[3];
		w=dat[12]*vec[0]+dat[13]*vec[1]+dat[14]*vec[2]+dat[15]*vec[3];
		result[0]=x;
		result[1]=y;
		result[2]=z;
		result[3]=w;
	}

	/*! Transforms a vector.
	   \param result [Out] Output vector
	   \param org [In] Input vector
	   \param translate [In] 4th dimentional element of the input vector.  */
	inline void Mul(YsVec3 &result,const YsVec3 &org,const double &translate) const
	{
		double x,y,z,w;
		x=dat[ 0]*org.x()+dat[ 1]*org.y()+dat[ 2]*org.z()+dat[ 3]*translate;
		y=dat[ 4]*org.x()+dat[ 5]*org.y()+dat[ 6]*org.z()+dat[ 7]*translate;
		z=dat[ 8]*org.x()+dat[ 9]*org.y()+dat[10]*org.z()+dat[11]*translate;
		w=dat[12]*org.x()+dat[13]*org.y()+dat[14]*org.z()+dat[15]*translate;
		// 2016/02/05
		// Same error as MulInverse.  Was not dividing xyz with w.
		if(YsTolerance<fabs(w))
		{
			x/=w;
			y/=w;
			z/=w;
		}
		result.Set(x,y,z);
	}

	/*! Transforms an attitude Att with the matrix. */
	inline const YsAtt3 Mul(const YsAtt3 &Att) const;

	/*! Transforms an attitude Att with the matrix and returns it in NewAtt. */
	inline const YsAtt3 Mul(YsAtt3 &NewAtt,const YsAtt3 &Att) const;

	/*! Transforms a 4D vector by the inverse of this matrix.
	    \param result [Out] Output vector 
	    \param vec [In] Input vector
	    */
	inline YSRESULT MulInverse(double result[4],const double vec[4]) const
	{
		return YsMulInverse4x4(result,dat,vec);
	}

	/*! Transforms a vector by the inverse of this matrix.
	   \param result [Out] Output vector
	   \param org [In] Input vector
	   \param translate [In] 4th dimentional element of the input vector.  */
	inline YSRESULT MulInverse(YsVec3 &result,const YsVec3 &org,const double &translate) const
	{
		if(YsAbs(translate)<YsTolerance)
		{
			YsMatrix3x3 mat3(YSFALSE);
			mat3.Set11(dat[ 0]);
			mat3.Set12(dat[ 1]);
			mat3.Set13(dat[ 2]);
			mat3.Set21(dat[ 4]);
			mat3.Set22(dat[ 5]);
			mat3.Set23(dat[ 6]);
			mat3.Set31(dat[ 8]);
			mat3.Set32(dat[ 9]);
			mat3.Set33(dat[10]);
			return mat3.MulInverse(result,org);
		}
		else
		{
			double inv[4],src[4];
			src[0]=org.x();
			src[1]=org.y();
			src[2]=org.z();
			src[3]=translate;
			auto res=YsMulInverse4x4(inv,dat,src);
			// 2016/01/19 Bug fix.  Forgetting to divide by inv[3]
			//                      Why were the programs working with this error???
			result.Set(inv[0]/inv[3],inv[1]/inv[3],inv[2]/inv[3]);
			return res;
		}
	}

	/*! Copies the elements of the matrix to the given array. */
	inline void GetArray(double mat[16]) const
	{
		int i;
		for(i=0; i<16; i++)
		{
			mat[i]=dat[i];
		}
	}

	/*! Copies the elements of the inverse matrix to the given array. */
	inline YSRESULT GetInverseArray(double mat[16]) const
	{
		return YsInvertMatrix4x4(mat,dat);
	}

	/*! Returns a constant reference to the array of the matrix elements. */
	inline const double *GetArray(void) const
	{
		return dat;
	}

	/*! Returns a matrix that is compatible with OpenGL. */
	template <class realType>
	inline void GetOpenGlCompatibleMatrix(realType mat[16]) const
	{
		mat[ 0]=(realType)dat[ 0];
		mat[ 1]=(realType)dat[ 4];
		mat[ 2]=(realType)dat[ 8];
		mat[ 3]=(realType)dat[12];

		mat[ 4]=(realType)dat[ 1];
		mat[ 5]=(realType)dat[ 5];
		mat[ 6]=(realType)dat[ 9];
		mat[ 7]=(realType)dat[13];

		mat[ 8]=(realType)dat[ 2];
		mat[ 9]=(realType)dat[ 6];
		mat[10]=(realType)dat[10];
		mat[11]=(realType)dat[14];

		mat[12]=(realType)dat[ 3];
		mat[13]=(realType)dat[ 7];
		mat[14]=(realType)dat[11];
		mat[15]=(realType)dat[15];
	}

	/*! Creates a matrix from an array that is compatible with OpenGL matrix. */
	template <class realType>
	inline void CreateFromOpenGlCompatibleMatrix(const realType mat[16])
	{
		dat[ 0]=(double)mat[ 0];
		dat[ 4]=(double)mat[ 1];
		dat[ 8]=(double)mat[ 2];
		dat[12]=(double)mat[ 3];
                
		dat[ 1]=(double)mat[ 4];
		dat[ 5]=(double)mat[ 5];
		dat[ 9]=(double)mat[ 6];
		dat[13]=(double)mat[ 7];
                
		dat[ 2]=(double)mat[ 8];
		dat[ 6]=(double)mat[ 9];
		dat[10]=(double)mat[10];
		dat[14]=(double)mat[11];
                
		dat[ 3]=(double)mat[12];
		dat[ 7]=(double)mat[13];
		dat[11]=(double)mat[14];
		dat[15]=(double)mat[15];
	}

	// See document/mat2rot.jpg
	/*! Extracts rotation. */
	inline YSRESULT GetRotation(double &x,double &y,double &z,double &rad) const
	{
		double a,b,c;
		double d,e,f;
		double h,i,j;

		a=dat[ 0];
		b=dat[ 1];
		c=dat[ 2];
		d=dat[ 4];
		e=dat[ 5];
		f=dat[ 6];
		h=dat[ 8];
		i=dat[ 9];
		j=dat[10];

		YsVec3 v1,v2,v3,v;
		double l1,l2,l3,l;
		v1.Set(a-1,b,c);
		v2.Set(d,e-1,f);
		v3.Set(h,i,j-1);

		l1=v1.GetSquareLength();
		l2=v2.GetSquareLength();
		l3=v3.GetSquareLength();

		if(l1>l2 && l1>l3)
		{
			v=v1;
			l=l1;
		}
		else if(l2>l3)
		{
			v=v2;
			l=l2;
		}
		else
		{
			v=v3;
			l=l3;
		}

		if(l<=YsTolerance*YsTolerance)
		{
			x=0.0;
			y=1.0;
			z=0.0;
			rad=0.0;
			return YSOK;
		}

		v.Normalize();

		YsVec3 u;
		u=v;
		Mul(u,v,0.0);
		u.Normalize();

		double inner;
		YsVec3 outer;
		inner=v*u;
		outer=v^u;
		outer.Normalize();

		rad=acos(inner);
		outer.Get(x,y,z);
		return YSOK;
	}

	/*! Extracts rotation */
	inline YSRESULT GetRotation(YsRotation &rot) const
	{
		double x,y,z,rad;
		if(GetRotation(x,y,z,rad)==YSOK)
		{
			rot.Set(x,y,z,rad);
		}
		return YSERR;
	}

	/*! Extracts translation */
	inline YSRESULT GetTranslation(double &x,double &y,double &z) const
	{
		YsVec3 tmp;
		GetTranslation(tmp);
		x=tmp.x();
		y=tmp.y();
		z=tmp.z();
		return YSOK;
	}

	/*! Extracts translation */
	inline YSRESULT GetTranslation(YsVec3 &trs) const
	{
		YsVec3 tmp(0.0,0.0,0.0);
		Mul(tmp,tmp,1.0);
		trs=tmp;
		return YSOK;
	}

	/*! Makes a matrix that transforms a point on a given plane onto XY plane.
	    \param cen [In] A point on the plane
	    \param nom [In] Normal vector of the plane. */
	YSRESULT MakeToXY(const YsVec3 &cen,const YsVec3 &nom);

	/*! Multiplies this matrix to 'incoming', and returns x and y components in a YsVec2. */
	inline const YsVec2 ToXY(const YsVec3 &incoming)
	{
		double x,y;
		const double *src=incoming;
		x=dat[ 0]*src[0]+dat[ 1]*src[1]+dat[ 2]*src[2]+dat[ 3]*1.0;
		y=dat[ 4]*src[0]+dat[ 5]*src[1]+dat[ 6]*src[2]+dat[ 7]*1.0;
		return YsVec2(x,y);
	}

	/*! Makes a matrix from the given array. */
	void Set(const double from[16])
	{
		dat[ 0]=from[ 0];
		dat[ 1]=from[ 1];
		dat[ 2]=from[ 2];
		dat[ 3]=from[ 3];
		dat[ 4]=from[ 4];
		dat[ 5]=from[ 5];
		dat[ 6]=from[ 6];
		dat[ 7]=from[ 7];
		dat[ 8]=from[ 8];
		dat[ 9]=from[ 9];
		dat[10]=from[10];
		dat[11]=from[11];
		dat[12]=from[12];
		dat[13]=from[13];
		dat[14]=from[14];
		dat[15]=from[15];
	}

	/*! Returns a column vector of row 1 to 3.  The fourth row is ignored.  Parameter c (column) must be 1<=c<=4. */
	inline YsVec3 GetColumnVector(const int c) const
	{
		auto i=c-1;
		return YsVec3(dat[i],dat[i+4],dat[i+8]);
	}


	/*! Returns a column vector and the fourth-row value is set in w.  Parameter c (column) must be 1<=c<=4. */
	inline YsVec3 GetColumnVector(double &w,const int c) const
	{
		auto i=c-1;
		w=dat[i+12];
		return YsVec3(dat[i],dat[i+4],dat[i+8]);
	}


	/*! Sets a column vector.  The fourth row will not be updated.  Parameter c (column) must be 1<=c<=4 */
	inline void SetColumnVector(const int c,const YsVec3 &v)
	{
		auto i=c-1;
		dat[i  ]=v[0];
		dat[i+4]=v[1];
		dat[i+8]=v[2];
	}

	/*! Sets a column vector.  The fourth row value will be set to w.  Parameter c (column) must be 1<=c<=4 */
	inline void SetColumnVector(const int c,const YsVec3 &v,const double w)
	{
		auto i=c-1;
		dat[i   ]=v[0];
		dat[i+ 4]=v[1];
		dat[i+ 8]=v[2];
		dat[i+12]=w;
	}
};

inline const double &YsMatrix4x4::v11(void) const
{
	return dat[0];
}

inline const double &YsMatrix4x4::v12(void) const
{
	return dat[1];
}

inline const double &YsMatrix4x4::v13(void) const
{
	return dat[2];
}

inline const double &YsMatrix4x4::v14(void) const
{
	return dat[3];
}

inline const double &YsMatrix4x4::v21(void) const
{
	return dat[4];
}

inline const double &YsMatrix4x4::v22(void) const
{
	return dat[5];
}

inline const double &YsMatrix4x4::v23(void) const
{
	return dat[6];
}

inline const double &YsMatrix4x4::v24(void) const
{
	return dat[7];
}

inline const double &YsMatrix4x4::v31(void) const
{
	return dat[8];
}

inline const double &YsMatrix4x4::v32(void) const
{
	return dat[9];
}

inline const double &YsMatrix4x4::v33(void) const
{
	return dat[10];
}

inline const double &YsMatrix4x4::v34(void) const
{
	return dat[11];
}

inline const double &YsMatrix4x4::v41(void) const
{
	return dat[12];
}

inline const double &YsMatrix4x4::v42(void) const
{
	return dat[13];
}

inline const double &YsMatrix4x4::v43(void) const
{
	return dat[14];
}

inline const double &YsMatrix4x4::v44(void) const
{
	return dat[15];
}

inline void YsMatrix4x4::Set11(const double &v)
{
	dat[0]=v;
}

inline void YsMatrix4x4::Set12(const double &v)
{
	dat[1]=v;
}

inline void YsMatrix4x4::Set13(const double &v)
{
	dat[2]=v;
}

inline void YsMatrix4x4::Set14(const double &v)
{
	dat[3]=v;
}

inline void YsMatrix4x4::Set21(const double &v)
{
	dat[4]=v;
}

inline void YsMatrix4x4::Set22(const double &v)
{
	dat[5]=v;
}

inline void YsMatrix4x4::Set23(const double &v)
{
	dat[6]=v;
}

inline void YsMatrix4x4::Set24(const double &v)
{
	dat[7]=v;
}

inline void YsMatrix4x4::Set31(const double &v)
{
	dat[8]=v;
}

inline void YsMatrix4x4::Set32(const double &v)
{
	dat[9]=v;
}

inline void YsMatrix4x4::Set33(const double &v)
{
	dat[10]=v;
}

inline void YsMatrix4x4::Set34(const double &v)
{
	dat[11]=v;
}

inline void YsMatrix4x4::Set41(const double &v)
{
	dat[12]=v;
}

inline void YsMatrix4x4::Set42(const double &v)
{
	dat[13]=v;
}

inline void YsMatrix4x4::Set43(const double &v)
{
	dat[14]=v;
}

inline void YsMatrix4x4::Set44(const double &v)
{
	dat[15]=v;
}


/*! An operator that multiplies a vector by a 4x4 matrix.
    The 4th dimentional component of the vector is assumed to be 1.0 */
inline const YsVec3 operator*(const YsMatrix4x4 &a,const YsVec3 &b)
{
	YsVec3 r;
	a.Mul(r,b,1.0);
	return r;
}

/*! An operator that multiplies a 4x4 matrix and a 3x3 matrix and returns
    a 4x4 matrix.  4th row and 4th column elements of the 3x3 matrix is all 
    assumed to be zero, except the element at row=4 and column=4, which is
    assumed to be one. */
inline const YsMatrix4x4 operator*(const YsMatrix4x4 &a,const YsMatrix3x3 &b)
{
	YsMatrix4x4 mat;
	mat.Set11(a.v11()*b.v11()+a.v12()*b.v21()+a.v13()*b.v31());
	mat.Set12(a.v11()*b.v12()+a.v12()*b.v22()+a.v13()*b.v32());
	mat.Set13(a.v11()*b.v13()+a.v12()*b.v23()+a.v13()*b.v33());

	mat.Set21(a.v21()*b.v11()+a.v22()*b.v21()+a.v23()*b.v31());
	mat.Set22(a.v21()*b.v12()+a.v22()*b.v22()+a.v23()*b.v32());
	mat.Set23(a.v21()*b.v13()+a.v22()*b.v23()+a.v23()*b.v33());

	mat.Set31(a.v31()*b.v11()+a.v32()*b.v21()+a.v33()*b.v31());
	mat.Set32(a.v31()*b.v12()+a.v32()*b.v22()+a.v33()*b.v32());
	mat.Set33(a.v31()*b.v13()+a.v32()*b.v23()+a.v33()*b.v33());

	mat.Set41(a.v41());
	mat.Set42(a.v42());
	mat.Set43(a.v43());

	mat.Set14(a.v14());
	mat.Set24(a.v24());
	mat.Set34(a.v34());

	mat.Set44(a.v44());

	return mat;
}

/*! An operator that multiplies a 3x3 matrix and a 4x4 matrix and returns
    a 4x4 matrix.  4th row and 4th column elements of the 3x3 matrix is all 
    assumed to be zero, except the element at row=4 and column=4, which is
    assumed to be one. */
inline const YsMatrix4x4 operator*(const YsMatrix3x3 &a,const YsMatrix4x4 &b)
{
	YsMatrix4x4 mat;
	mat.Set11(a.v11()*b.v11()+a.v12()*b.v21()+a.v13()*b.v31());
	mat.Set12(a.v11()*b.v12()+a.v12()*b.v22()+a.v13()*b.v32());
	mat.Set13(a.v11()*b.v13()+a.v12()*b.v23()+a.v13()*b.v33());

	mat.Set21(a.v21()*b.v11()+a.v22()*b.v21()+a.v23()*b.v31());
	mat.Set22(a.v21()*b.v12()+a.v22()*b.v22()+a.v23()*b.v32());
	mat.Set23(a.v21()*b.v13()+a.v22()*b.v23()+a.v23()*b.v33());

	mat.Set31(a.v31()*b.v11()+a.v32()*b.v21()+a.v33()*b.v31());
	mat.Set32(a.v31()*b.v12()+a.v32()*b.v22()+a.v33()*b.v32());
	mat.Set33(a.v31()*b.v13()+a.v32()*b.v23()+a.v33()*b.v33());

	mat.Set41(b.v41());
	mat.Set42(b.v42());
	mat.Set43(b.v43());

	mat.Set14(b.v14());
	mat.Set24(b.v24());
	mat.Set34(b.v34());

	mat.Set44(b.v44());

	return mat;
}

/*! An operator that multiplies two 4x4 matrices */
inline const YsMatrix4x4 operator*(const YsMatrix4x4 &mat,const YsRotation &rot)
{
	YsMatrix4x4 r(mat);
	r.Rotate(rot);
	return r;
}

/*! An operator that multiplies a rotational matrix and a 4x4 matrix and returns a 4x4 matricss */
inline const YsMatrix4x4 operator*(const YsRotation &rot,const YsMatrix4x4 &mat)
{
	YsMatrix4x4 r;
	r.Rotate(rot);
	return r*mat;
}

////////////////////////////////////////////////////////////
/*! An Euler-angle class.  It represents a rotation by three angles, heading, pitch, and bank.
    Heading angle defines a rotation in XZ plane.  Pitch angle defines a elevation angle from XZ plane,
    or the rotation about the object's own X axis, and ranges from -PI/2 to +PI/2.  Bank angle
    defines a rotation about the object's own Z axis.

    The orientation of the heading rotation depends on the global variable YsCoordSysModel.  
    When the coordinate system model is a right-hand coordinate system (such as OpenGL's coordinate
    system) the heading rotation is a clockwise rotation in XZ plane (X goes to the right and Z
    goes up).  Or, if the coordinate system model is a left-hand coordinate sytem (such as Direct3D's
    coordinate system) the heading rotation is a counter-clockwise rotation in XZ plane.

    Since positive-Z direction flips between typical left-hand and right-hand coordinate systems,
    from the viewer, the heading rotation appears to be the same regardless of the coordinante system
    model.

    Bank angle is often also called as roll angle.  However, in aviation terminology, roll is
    often used as a rotational velocity and bank is an angle from the horizon.  Therefore, here
    I use bank angle instead of roll angle.

    \sa operator*(const YsAtt3 &att,const YsVec3 &vec)
    \sa operator*(const YsMatrix3x3 &mat,const YsAtt3 &att)
    \sa operator*(const YsAtt3 &att,const YsMatrix3x3 &mat)
    \sa operator*(const YsMatrix4x4 &mat,const YsAtt3 &att)
    \sa operator*(const YsAtt3 &att,const YsMatrix4x4 &mat)
    \sa operator==(const YsAtt3 &a,const YsAtt3 &b)
    \sa operator!=(const YsAtt3 &a,const YsAtt3 &b)
    \sa YsCoordSysModel
*/
class YsAtt3
{
public:
	/*! Set compatibilityLevel<20140530 to use older definition of YsAtt3 rotation.
	    Since 20140530 version, YsAtt3 defines the same rotation regardless of the coord sys model.
	*/
	static int compatibilityLevel; 

	/*! Default constructor.  Does nothing. */
	inline YsAtt3(){}
	/*! A constructor that takes heading, pitch, and bank angles.
	    \param h [In] Heading angle in radian
	    \param p [In] Pitch angle in radian
	    \param b [In] Bank angle in radian */
	YsAtt3(const double &h,const double &p,const double &b);

	/*! Returns heading angle */
	inline double h(void) const
	{
		return hdg;
	}
	/*! Returns pitch angle */
	inline double p(void) const
	{
		return pch;
	}
	/*! Returns bank angle */
	inline double b(void) const
	{
		return bnk;
	}
	/*! Sets all three angles */
	void Set(const double &h,const double &p,const double &b);
	/*! Sets heading angle */
	void SetH(const double &h);
	/*! Sets pitch angle */
	void SetP(const double &p);
	/*! Sets bank angle */
	void SetB(const double &b);
	/*! Sets heading angle */
	void AddH(const double &h);
	/*! Sets pitch angle */
	void AddP(const double &p);
	/*! Sets bank angle */
	void AddB(const double &b);
	/*! Returns a forward vector */
	YsVec3 GetForwardVector(void) const;
	/*! Returns a upward vector */
	YsVec3 GetUpVector(void) const;
	/*! Calculates Euler angle based on the forward vector and upward vector. */
	YSRESULT SetTwoVector(const YsVec3 &fwd,const YsVec3 &up);
	/*! Calculates heading and pitch angles based on the forward vector.  Bank angle will be set to zero. */
	YSRESULT SetForwardVector(YsVec3 vec);
	/*! Calculates bank angle based on the given upward vector.  Heading and pitch angles will not change. */
	YSRESULT SetUpVector(YsVec3 up);

	/*! Rotates a vector.
	    \param to [Out] Output vector 
	    \param from [In] Input vector
	    */
	inline void Mul(YsVec3 &to,const YsVec3 &from) const
	{
		to=from;
		if(20140530<=compatibilityLevel || YSOPENGL!=YsCoordSysModel)
		{
			to.RotateXY(bnk);
			to.RotateYZ(-pch);  // RotateZY(pch);
			to.RotateXZ(hdg);
		}
		else
		{
			to.RotateXY(bnk);
			to.RotateYZ(pch);  // RotateZY(-pch);
			to.RotateXZ(-hdg);
		}
	}

	/*! Inversely rotates a vector.
	    \param to [Out] Output vector 
	    \param from [In] Input vector
	    */
	inline void MulInverse(YsVec3 &to,const YsVec3 &from) const
	{
		to=from;
		if(20140530<=compatibilityLevel || YSOPENGL!=YsCoordSysModel)
		{
			to.RotateXZ(-hdg);
			to.RotateYZ(pch);  // RotateZY(-pch);
			to.RotateXY(-bnk);
		}
		else
		{
			to.RotateXZ(hdg);
			to.RotateYZ(-pch);  // RotateZY(pch);
			to.RotateXY(-bnk);
		}
	}

	/*! Transforms an attitude Att with the matrix. */
	inline const YsAtt3 Mul(const YsAtt3 &Att) const;

	/*! Transforms an attitude Att with the matrix and returns it in NewAtt. */
	inline const YsAtt3 Mul(YsAtt3 &NewAtt,const YsAtt3 &Att) const;

	/*! Rotates about the object's own X axis */
	void NoseUp(const double &d);
	/*! Rotates about the object's own Y axis counter clockwise */
	void YawLeft(const double &d);

	/*! Converts the Euler angle to a 4x4 matrix */
	inline const YsMatrix4x4 &GetMatrix4x4(YsMatrix4x4 &mat) const
	{
		mat.Initialize();
		if(20140530<=compatibilityLevel || YSOPENGL!=YsCoordSysModel)
		{
			mat.RotateXZ(hdg);
			mat.RotateZY(pch);
			mat.RotateXY(bnk);
		}
		else
		{
			mat.RotateXZ(-hdg);
			mat.RotateZY(-pch);
			mat.RotateXY(bnk);
		}
		return mat;
	}

	/*! Converts the Euler angle to a 3x3 matrix */
	inline const YsMatrix3x3 &GetMatrix3x3(YsMatrix3x3 &mat) const
	{
		mat.Initialize();
		if(20140530<=compatibilityLevel || YSOPENGL!=YsCoordSysModel)
		{
			mat.RotateXZ(hdg);
			mat.RotateZY(pch);
			mat.RotateXY(bnk);
		}
		else
		{
			mat.RotateXZ(-hdg);
			mat.RotateZY(-pch);
			mat.RotateXY(bnk);
		}
		return mat;
	}

/// \cond
protected:
	double hdg,pch,bnk;
/// \endcond
};

/*! Rotates a vector by the rotation defined by the Euler angle. */
inline YsVec3 operator*(const YsAtt3 &att,const YsVec3 &vec)
{
	YsVec3 r;
	att.Mul(r,vec);
	return r;
}

/*! Multiplies a 3x3 matrix and the rotation defined by the Euler angle and returns a 3x3 matrix. */
inline YsMatrix3x3 operator*(const YsMatrix3x3 &mat,const YsAtt3 &att)
{
	YsMatrix3x3 r(YSFALSE);
	r=mat;
	r.Rotate(att);
	return r;
}

/*! Multiplies the rotation defined by the Euler angle and a 3x3 matrix and returns a 3x3 matrix. */
inline YsMatrix3x3 operator*(const YsAtt3 &att,const YsMatrix3x3 &mat)
{
	YsMatrix3x3 r(YSFALSE);
	att.GetMatrix3x3(r);
	r=r*mat;
	return r;
}

/*! Multiplies a 4x4 matrix and the rotation defined by the Euler angle and returns a 3x3 matrix. */
inline YsMatrix4x4 operator*(const YsMatrix4x4 &mat,const YsAtt3 &att)
{
	YsMatrix4x4 r(YSFALSE);
	r=mat;
	r.Rotate(att);
	return r;
}

/*! Multiplies the rotation defined by the Euler angle and a 4x4 matrix and returns a 3x3 matrix. */
inline YsMatrix4x4 operator*(const YsAtt3 &att,const YsMatrix4x4 &mat)
{
	YsMatrix4x4 r(YSFALSE);
	att.GetMatrix4x4(r);
	r=r*mat;
	return r;
}

/*! Compares two Euler angles. */
inline int operator==(const YsAtt3 &a,const YsAtt3 &b)
{
	return (YsEqual(a.h(),b.h())==YSTRUE && YsEqual(a.p(),b.p())==YSTRUE && YsEqual(a.b(),b.b())==YSTRUE);
}

/*! Compares two Euler angles. */
inline int operator!=(const YsAtt3 &a,const YsAtt3 &b)
{
	return (YsEqual(a.h(),b.h())!=YSTRUE || YsEqual(a.p(),b.p())!=YSTRUE || YsEqual(a.b(),b.b())!=YSTRUE);
}

inline void YsMatrix4x4::Multiply(const class YsVec3 &pos,const class YsAtt3 &att)
{
	 Translate(pos);
	 RotateXZ(att.h());
	 RotateZY(att.p());
	 RotateXY(att.b());
}

inline void YsMatrix4x4::MultiplyInverse(const class YsVec3 &pos,const class YsAtt3 &att)
{
	 RotateXY(-att.b());
	 RotateZY(-att.p());
	 RotateXZ(-att.h());
	 Translate(-pos);
}


inline const YsAtt3 YsMatrix3x3::Mul(const YsAtt3 &Att) const
{
	auto ev=Att.GetForwardVector();
	auto uv=Att.GetUpVector();

	Mul(ev,ev);
	Mul(uv,uv);

	YsAtt3 newAtt;
	newAtt.SetTwoVector(ev,uv);

	return newAtt;
}

inline const YsAtt3 YsMatrix3x3::Mul(YsAtt3 &NewAtt,const YsAtt3 &Att) const
{
	NewAtt=this->Mul(Att);
	return NewAtt;
}

inline const YsAtt3 YsMatrix4x4::Mul(const YsAtt3 &Att) const
{
	auto ev=Att.GetForwardVector();
	auto uv=Att.GetUpVector();

	Mul(ev,ev,0.0);
	Mul(uv,uv,0.0);

	YsAtt3 newAtt;
	newAtt.SetTwoVector(ev,uv);

	return newAtt;
}

inline const YsAtt3 YsMatrix4x4::Mul(YsAtt3 &NewAtt,const YsAtt3 &Att) const
{
	NewAtt=this->Mul(Att);
	return NewAtt;
}

////////////////////////////////////////////////////////////
// Declaration /////////////////////////////////////////////
/*! A plane class. */
class YsPlane
{
public:
	/*! Default constructor.  Does nothing. */
	inline YsPlane(){}
	/*! Constructor that creates a plane from a point on the plane and a normal vector.
	    The incoming normal vector is normalized within this function.  Therefore, the
	    vector does not have to be a unit vector.
	    \param o [In] A point on the plane
	    \param n [In] A normal vector */
	YsPlane(const YsVec3 &o,const YsVec3 &n);

	/*! returns XY-plane. */
	static YsPlane XYPlane(void);

	/*! returns XZ-plane. */
	static YsPlane XZPlane(void);

	/*! returns YZ-plane. */
	static YsPlane YZPlane(void);

	/*! Creates a plane.
	    The incoming normal vector is normalized within this function.  Therefore, the
	    vector does not have to be a unit vector.
	    \param o [In] A point on the plane
	    \param n [In] A normal vector */
	inline void Set(const YsVec3 &o,const YsVec3 &n)
	{
		org=o;
		nom=n;
		if(nom.IsNormalized()!=YSTRUE)
		{
			nom.Normalize();
		}
	}

	/*! Changes a point on the plane.  It does not change the normal. */
	inline void SetOrigin(const YsVec3 &o)
	{
		org=o;
	}
	/*! Changes a normal of the plane.  It does not change the normal. */
	inline void SetNormal(const YsVec3 &n)
	{
		nom=n;
		if(nom.IsNormalized()!=YSTRUE)
		{
			nom.Normalize();
		}
	}

	/*! Returns a point on the plane. */
	inline void GetOrigin(YsVec3 &o) const
	{
		o=org;
	}
	/*! Returns a normal of the plane. */
	inline void GetNormal(YsVec3 &n) const
	{
		n=nom;
	}

	/*! Returns a point on the plane. */
	inline const YsVec3 &GetOrigin(void) const
	{
		return org;
	}
	/*! Returns a normal of the plane. */
	inline const YsVec3 &GetNormal(void) const
	{
		return nom;
	}

	/*! Returns YSTRUE if \a pos is on the plane (withitn tolerance), or YSFALSE otherwise. */
	YSBOOL CheckOnPlane(const YsVec3 &pos) const;
	/*! If a given infinite line intersects with the plane, it returns YSTRUE and the intersecting point is returned to crs.  It returns YSFALSE otherwise.
	    \param crs [Out] Intersecting point
	    \param o A [In] point on the line
	    \param v A [In] vector that defines the direction of the line */
	YSRESULT GetIntersection(YsVec3 &crs,const YsVec3 &o,const YsVec3 &v) const;

	/*! If a given infinite line intersects with the plane, it returns YSOK and the intersecting point is returned to crs.  It returns YSERR otherwise.
	    This function iteratively improves the precision of the intersecting point.
	    \param crs [Out] Intersecting point
	    \param o [In] A point on the line
	    \param v [In] A vector that defines the direction of the line */
	YSRESULT GetIntersectionHighPrecision(YsVec3 &crs,const YsVec3 &o,const YsVec3 &v) const;

	/*! If a given finite-length line intersects with the plane, it returns YSOK and the intersecting point is returns to crs.  It returns YSERR otherwise.
	    \param crs [Out] Intersecting point
	    \param p1 [In] An end point of the finite-length line
	    \param p2 [In] The other end point of the finite-length line */
	YSRESULT GetPenetration(YsVec3 &crs,const YsVec3 &p1,const YsVec3 &p2) const;

	/*! If a given finite-length line intersects with the plane, it returns YSOK and the intersecting point is returns to crs.  It returns YSERR otherwise.
	    This function iteratively improves the precision of the intersecting point.
	    \param crs [Out] Intersecting point
	    \param p1 [In] An end point of the finite-length line
	    \param p2 [In] The other end point of the finite-length line */
	YSRESULT GetPenetrationHighPrecision(YsVec3 &crs,const YsVec3 &p1,const YsVec3 &p2) const;

	/*! Returns a nearest point on the plane from the given reference point.
	    \param np [Out] Nearest point on the plane
	    \param ref [In] A reference point. */
	YSRESULT GetNearestPoint(YsVec3 &np,const YsVec3 &ref) const;

	/*! Returns a nearest point on the plane from the given reference point.
	    \param ref [In] A reference point. */
	YsVec3 GetNearestPoint(const YsVec3 &ref) const;

	/*! Returns a nearest point on the plane from the given reference point.
	    This function iteratively improves the precision of the intersecting point.
	    \param np [Out] Nearest point on the plane
	    \param ref [In] A reference point. */
	YSRESULT GetNearestPointHighPrecision(YsVec3 &np,const YsVec3 &ref) const;

	/*! Makes a plane that fits to the given set of points using the least-square method.
	    \param np [In] Number of points
	    \param p [In] Array of points. */
	YSRESULT MakeBestFitPlane(YSSIZE_T np,const YsVec3 p[]);

	/*! Makes a plane that fits to the given set of points 'p'. 
	    std::vector <YsVec3> or YsArray <YsVec3,N> can be given as p.
	*/
	YSRESULT MakeBestFitPlane(const YsConstArrayMask <YsVec3> &p)
	{
		return MakeBestFitPlane(p.GetN(),p);
	}

	/*! Makes a plane on which given three points lies. */
	YSRESULT MakePlaneFromTriangle(const YsVec3 &p1,const YsVec3 &p2,const YsVec3 &p3);

	/*! Returns zero if the reference point is within tolerance from the plane, or a positive number if
	    (ref-o)*n is positive, or a negative number if (ref-o)*n is negative. */
	int GetSideOfPlane(const YsVec3 &ref) const;

	/*! Returns the distance from the reference point to the plane. */
	double GetDistance(const YsVec3 &ref) const;
/// \cond
protected:
	YsVec3 org,nom;
/// \endcond
};

////////////////////////////////////////////////////////////

template <class VecType,class DistType=double>
class YsEquiDistant
{
private:
	VecType cen;
	DistType rad;
public:
	inline YsEquiDistant(){}
	inline YsEquiDistant(const VecType &cen,const DistType rad)
	{
		Set(cen,rad);
	}
	inline void Set(const VecType &cen,const DistType rad)
	{
		this->cen=cen;
		this->rad=rad;
	}
	inline const VecType &Center(void) const
	{
		return cen;
	}
	inline DistType Radius(void) const
	{
		return rad;
	}
	inline YSBOOL IsInside(const VecType &ref) const
	{
		return ((ref-cen).GetSquareLength()<rad*rad ? YSTRUE : YSFALSE);
	}
	inline YSBOOL Overlap(const YsEquiDistant <VecType,DistType> &from) const
	{
		DistType radSum=rad+from.rad;
		return ((cen-from.cen).GetSquareLength()<radSum*radSum ? YSTRUE : YSFALSE);
	}

	inline void SetCenter(const VecType &cen)
	{
		this->cen=cen;
	}
	inline void SetRadius(const DistType rad)
	{
		this->rad=rad;
	}
};

typedef YsEquiDistant <YsVec2,double> YsCircle;
typedef YsEquiDistant <YsVec3,double> YsSphere;

////////////////////////////////////////////////////////////

// I'm working on it.  My current question is is I should cache unit line vector or not.
// Probably yes.
template <class VectorClass>
class YsLineTemplate
{
private:
	VectorClass p[2],v;
public:
	/*! Make a line from two passing points.
	    The cached vector will be UnitVector(p2-p1).
	*/
	void MakeFromTwoPoint(const VectorClass &p1,const VectorClass &p2);

	/*! Make a line from a passing point and a vector parallel to the line.
	    The first end point will be p1, and the second end point will be p1+v.
	*/
	void MakeFromPointAndVector(const VectorClass &p1,const VectorClass &v);

	/*! Returns one of the end points.  Index must be 0 or 1.
	    The function is not checking out-of-bound Index.
	    The calling function is responsible for not giving Index less than 0 or greater than 1.
	*/
	inline VectorClass operator[](int Index) const;

	/*! Returns the first end point.
	*/
	inline VectorClass Origin(void) const;

	/*! Returns a unit vector that is parallel to the line.
	*/
	inline VectorClass Vector(void) const;
};

template <class VectorClass>
void YsLineTemplate<VectorClass>::MakeFromTwoPoint(const VectorClass &p1,const VectorClass &p2)
{
	this->p[0]=p1;
	this->p[1]=p2;
	this->v=p2-p1;
	this->v.Normalize();
}

template <class VectorClass>
void YsLineTemplate<VectorClass>::MakeFromPointAndVector(const VectorClass &p1,const VectorClass &v)
{
	p[0]=p1;
	p[1]=p1+v;
	this->v=v;
	this->v.Normalize();
}

template <class VectorClass>
inline VectorClass YsLineTemplate<VectorClass>::operator[](int idx) const
{
	return p[idx];
}

template <class VectorClass>
inline VectorClass YsLineTemplate<VectorClass>::Origin(void) const
{
	return p[0];
}

template <class VectorClass>
inline VectorClass YsLineTemplate<VectorClass>::Vector(void) const
{
	return v;
}

////////////////////////////////////////////////////////////

class YsLine2 : public YsLineTemplate <YsVec2>
{
	inline static YsLine2 FromPointAndVector(const YsVec2 &o,const YsVec2 &v);
};

YsLine2 YsLine2::FromPointAndVector(const YsVec2 &o,const YsVec2 &v)
{
	YsLine2 l;
	l.MakeFromPointAndVector(o,v);
	return l;
}

class YsLine3 : public YsLineTemplate <YsVec3>
{
public:
	inline static YsLine3 FromPointAndVector(const YsVec3 &o,const YsVec3 &v);
};

YsLine3 YsLine3::FromPointAndVector(const YsVec3 &o,const YsVec3 &v)
{
	YsLine3 l;
	l.MakeFromPointAndVector(o,v);
	return l;
}

////////////////////////////////////////////////////////////

/*! Returns a normalized vector of the incoming vector.
    */
inline YsVec3 YsUnitVector(const YsVec3 &incoming)
{
	const double l=incoming.GetLength();
	if(YsTolerance<=l)
	{
		return incoming/l;
	}
	return incoming;
}

inline YsVec2 YsUnitVector(const YsVec2 &incoming)
{
	const double l=incoming.GetLength();
	if(YsTolerance<=l)
	{
		return incoming/l;
	}
	return incoming;
}

////////////////////////////////////////////////////////////

class YsResultAndVec2
{
public:
	YSRESULT res;
	YsVec2 pos;
};

class YsResultAndVec3
{
public:
	YSRESULT res;
	YsVec3 pos;
};

////////////////////////////////////////////////////////////

/* } */
#endif
