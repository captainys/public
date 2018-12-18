/* ////////////////////////////////////////////////////////////

File Name: ysmatrix.h
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

#ifndef YSMATRIX_IS_INCLUDED
#define YSMATRIX_IS_INCLUDED
/* { */

#include <stdio.h>
#include "ysmath.h"
#include "ysprintf.h"
#include "ysmatrixtemplate.h"

// Declaration /////////////////////////////////////////////
class YsMatrix
{
// CAUTION!
// Whenever add a item, do not forget to modify operator=, operator+=

public:
	YsMatrix();
	YsMatrix(const YsMatrix &from);
	~YsMatrix();

protected:
	int nRow,nColumn;
	double *dat;
	mutable YsMatrix *tmp; // To keep temoprary matrix

public:
	inline const double &v(int row,int column) const;
	inline int nr(void) const
	{
		return nRow;
	}
	inline int nc(void) const
	{
		return nColumn;
	}
	inline YSRESULT Set(int row,int column,const double &v);
	void Create(int nr,int nc);
	void Create(int nr,int nc,const double *d);
	void CreateWithoutClear(int nr,int nc);
	void LoadIdentity(void);

	YSRESULT GetSubMatrix(YsMatrix &mat,int r,int c) const;
	YSRESULT GetDeterminantMatrix(YsMatrix &mat) const;
	YSRESULT GetDeterminant(double &det) const;
	YSRESULT Transpose(void);
	YSRESULT Invert(void);

	YSRESULT SwapRow(int r1,int r2);
	YSRESULT MulRow(int r,double m);
	YSRESULT DivRow(int r,double d);
	YSRESULT Row1MinusRow2Mul(int r1,int r2,double m);

	void Print(const char *printFormat=" %10.3lf ") const;

	const YsMatrix &operator=(const YsMatrix &);
	const YsMatrix &operator+=(const YsMatrix &);

// For Internal Use
	YsMatrix *UseTemporaryMatrix(void) const;
};

inline const YsMatrix &operator*(const YsMatrix &a,const YsMatrix &b)
{
	YsMatrix *tmp;

	if(a.nc()!=b.nr())
	{
		printf("Warning : Invalid Matrix Multiplier.\n");
	}

	tmp=a.UseTemporaryMatrix();
	tmp->CreateWithoutClear(a.nr(),b.nc());

	int r,c,i;
	for(r=1; r<=a.nr(); r++)
	{
		for(c=1; c<=b.nc(); c++)
		{
			double sum;
			sum=0.0;
			for(i=1; i<=a.nc(); i++)
			{
				sum+=a.v(r,i)*b.v(i,c);
			}
			tmp->Set(r,c,sum);
		}
	}

	return *tmp;
}

inline const YsMatrix &operator+(const YsMatrix &a,const YsMatrix &b)
{
	YsMatrix *tmp;

	if(a.nc()!=b.nc() || a.nr()!=b.nr())
	{
		printf("Warning : Invalid Matrix Summation.\n");
	}

	tmp=a.UseTemporaryMatrix();
	tmp->CreateWithoutClear(a.nr(),b.nc());

	int r,c;
	for(r=1; r<=a.nr(); r++)
	{
		for(c=1; c<=b.nc(); c++)
		{
			tmp->Set(r,c,a.v(r,c)+b.v(r,c));
		}
	}

	return *tmp;
}

inline const YsMatrix &operator-(const YsMatrix &a,const YsMatrix &b)
{
	YsMatrix *tmp;

	if(a.nc()!=b.nc() || a.nr()!=b.nr())
	{
		printf("Warning : Invalid Matrix Subtraction.\n");
	}

	tmp=a.UseTemporaryMatrix();
	tmp->CreateWithoutClear(a.nr(),b.nc());

	int r,c;
	for(r=1; r<=a.nr(); r++)
	{
		for(c=1; c<=b.nc(); c++)
		{
			tmp->Set(r,c,a.v(r,c)-b.v(r,c));
		}
	}

	return *tmp;
}

inline const YsMatrix &operator+(const YsMatrix &a)
{
	return a;
}

inline const YsMatrix &operator-(const YsMatrix &a)
{
	YsMatrix *tmp;

	tmp=a.UseTemporaryMatrix();

	tmp->CreateWithoutClear(a.nr(),a.nc());

	int r,c;
	for(r=1; r<=a.nr(); r++)
	{
		for(c=1; c<=a.nc(); c++)
		{
			tmp->Set(r,c,-a.v(r,c));
		}
	}

	return *tmp;
}

inline const YsMatrix &operator^(const YsMatrix &a,const YsMatrix &b)
{
	YsMatrix *tmp;

	if((a.nr()!=3 && a.nr()!=4) || (b.nr()!=3 && b.nr()!=4))
	{
		printf("Outer product is only valid for 3x1 or 4x1 matrix.\n");
	}

	if(a.nr()!=b.nr() || a.nc()!=1 || b.nc()!=1)
	{
		printf("Outer product is invalid.\n");
	}

	tmp=a.UseTemporaryMatrix();
	tmp->CreateWithoutClear(a.nr(),1);

	double x1,y1,z1,x2,y2,z2;
	x1=a.v(1,1);
	y1=a.v(2,1);
	z1=a.v(3,1);
	x2=b.v(1,1);
	y2=b.v(2,1);
	z2=b.v(3,1);

	double x,y,z;
	x=y1*z1-y2*z2;
	y=z1*x1-z2*x2;
	z=x1*y1-x2*y2;
	tmp->Set(1,1,x);
	tmp->Set(2,1,y);
	tmp->Set(3,1,z);

	if(a.nr()==4)
	{
		tmp->Set(4,1,a.v(4,1));
	}

	return *tmp;
}

inline const YsMatrix &operator*(const YsMatrix &a,double b)
{
	YsMatrix *tmp;

	tmp=a.UseTemporaryMatrix();
	tmp->CreateWithoutClear(a.nr(),a.nc());

	int r,c;
	for(r=1; r<=a.nr(); r++)
	{
		for(c=1; c<=a.nc(); c++)
		{
			tmp->Set(r,c,a.v(r,c)*b);
		}
	}

	return *tmp;
}

inline const YsMatrix &operator*(double b,const YsMatrix &a)
{
	return a*b;
}

inline const YsMatrix &operator/(const YsMatrix &a,double b)
{
	YsMatrix *tmp;

	tmp=a.UseTemporaryMatrix();
	tmp->CreateWithoutClear(a.nr(),a.nc());

	int r,c;
	for(r=1; r<=a.nr(); r++)
	{
		for(c=1; c<=a.nc(); c++)
		{
			tmp->Set(r,c,a.v(r,c)/b);
		}
	}

	return *tmp;
}


inline const double &YsMatrix::v(int row,int column) const
{
	if(1<=row && row<=nRow && 1<=column && column<=nColumn)
	{
		return dat[(row-1)*nColumn+(column-1)];
	}
	else
	{
		YsErrOut("YsMatrix::v()\n");
		YsErrOut("  Index out of range.  (Note : Row/Column begins with 1\n");
		return dat[0];
	}
}

inline YSRESULT YsMatrix::Set(int row,int column,const double &a)
{
	if(1<=row && row<=nRow && 1<=column && column<=nColumn)
	{
		dat[(row-1)*nColumn+(column-1)]=a;
		return YSOK;
	}
	else
	{
		return YSERR;
	}
}

////////////////////////////////////////////////////////////

YSRESULT YsLUDecomposition(YsMatrix &l,YsMatrix &u,const YsMatrix &from);
YSRESULT YsCholeskyDecomposition(YsMatrix &l,const YsMatrix &from);

YSRESULT YsFindEigenValueEigenVectorByJacobiMethod
    (YsMatrix &value,YsMatrix &vector,const YsMatrix &from,const double &tolerance=YsTolerance);

YSRESULT YsInvertMatrix2x2(double result[4],const double from[4]);


/*
template <const int nr>
YSRESULT YsInvertMatrixNxN(double result[],const double from[])
    This function is now moved to ysmatrixtemplate.h
*/


/* } */
#endif

