/* ////////////////////////////////////////////////////////////

File Name: ysmatrixtemplate.h
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

#ifndef YSMATRIXTEMPLATE_IS_INCLUDED
#define YSMATRIXTEMPLATE_IS_INCLUDED
/* { */

////////////////////////////////////////////////////////////

#include "ysdef.h"

#include <math.h>

template <const int nr>
YSRESULT YsInvertMatrixNxN(double result[],const double from[])
{
	const int nc=nr;
	double *inv,buf[nr*nc],tfm[nr*nc];
	if(result!=from)
	{
		inv=result;
	}
	else
	{
		inv=buf;
	}

	int i,r,c,cc;
	i=0;
	for(r=0; r<nr; r++)
	{
		for(c=0; c<nc; c++)
		{
			inv[i]=(r==c ? 1.0 : 0.0);
			tfm[i]=from[i];
			i++;
		}
	}

	for(c=0; c<nc; c++)
	{
		int pivotRow,pivotIdx,testIdx;
		pivotRow=c;       // Initially r==c
		pivotIdx=c*nc+c;

		// Pass 1 : Find pivot
		for(r=c+1; r<nr; r++)
		{
			testIdx=r*nc+c;
			if(YsAbs(tfm[testIdx])>YsAbs(tfm[pivotIdx]))
			{
				pivotRow=r;
				pivotIdx=testIdx;
			}
		}

		if(YsAbs(tfm[pivotIdx])<YsTolerance)
		{
			return YSERR;
		}

		// Pass 2 : Swap row
		if(pivotRow!=c)
		{
			for(cc=0; cc<c; cc++)
			{
				// Don't need to swap tfm.  They are all zero for 0<=cc<c
				// YsSwapDouble(tfm[pivotRow*nc+cc],tfm[c*nc+cc]);
				YsSwapDouble(inv[pivotRow*nc+cc],inv[c*nc+cc]);
			}
			for(cc=c; cc<nc; cc++)
			{
				YsSwapDouble(tfm[pivotRow*nc+cc],tfm[c*nc+cc]);
				YsSwapDouble(inv[pivotRow*nc+cc],inv[c*nc+cc]);
			}
		}

		// Be careful!  pivotRow is already swapped, and pivot is
		// now sitting at (c,n)

		// Pass 3 : Eliminate rest
		double coeff;
		for(r=c+1; r<nr; r++)
		{
			coeff=tfm[r*nc+c]/tfm[c*nc+c];

			for(cc=0; cc<c; cc++)
			{
				// Don't need to compute tfm.  They are all zero for 0<=cc<c
				// tfm[r*nc+cc]-=coeff*tfm[c*nc+cc];
				inv[r*nc+cc]-=coeff*inv[c*nc+cc];
			}
			for(cc=c; cc<nc; cc++)
			{
				tfm[r*nc+cc]-=coeff*tfm[c*nc+cc];
				inv[r*nc+cc]-=coeff*inv[c*nc+cc];
			}
			// tfm[r*nc+c]=0.0;
		}
	}

	// At this point, lower left half is zero.  Now go ahead and make upper right half zero.
	// And, I know all YsAbs(diagnoal elements) are greater than YsTolerance.

	for(c=1; c<nc; c++)
	{
		for(r=0; r<c; r++)
		{
			double coeff;
			coeff=tfm[r*nc+c]/tfm[c*nc+c];
			for(cc=0; cc<c; cc++)
			{
				inv[r*nc+cc]-=coeff*inv[c*nc+cc];
			}
			for(cc=c; cc<nc; cc++)
			{
				tfm[r*nc+cc]-=coeff*tfm[c*nc+cc];
				inv[r*nc+cc]-=coeff*inv[c*nc+cc];
			}
			// tfm[r*nc+c]=0.0;
		}
	}

	// Now divide inv(r,c) by tfm(r,r)
	for(r=0; r<nr; r++)
	{
		for(c=0; c<nc; c++)
		{
			inv[r*nc+c]/=tfm[r*nc+r];
		}
	}


	if(result==from)
	{
		int i;
		for(i=0; i<nr*nc; i++)
		{
			result[i]=inv[i];
		}
	}
	return YSOK;
}

template <const int nr>
YSRESULT YsMulInverseNxN(double inv[],const double mat[],const double vec[])
{
	const int nc=nr;
	double tfm[nr*nc];
	int i,r,c,cc;

	if(inv!=vec)
	{
		for(i=0; i<nr; i++)
		{
			inv[i]=vec[i];
		}
	}

	for(i=0; i<nr*nr; i++)
	{
		tfm[i]=mat[i];
	}

	for(c=0; c<nc; c++)
	{
		int pivotRow,pivotIdx,testIdx;
		pivotRow=c;       // Initially r==c
		pivotIdx=c*nc+c;

		// Pass 1 : Find pivot
		for(r=c+1; r<nr; r++)
		{
			testIdx=r*nc+c;
			if(YsAbs(tfm[testIdx])>YsAbs(tfm[pivotIdx]))
			{
				pivotRow=r;
				pivotIdx=testIdx;
			}
		}

		if(YsAbs(tfm[pivotIdx])<YsTolerance)
		{
			return YSERR;
		}

		// Pass 2 : Swap row
		if(pivotRow!=c)
		{
			YsSwapDouble(inv[pivotRow],inv[c]);
			for(cc=c; cc<nc; cc++)
			{
				YsSwapDouble(tfm[pivotRow*nc+cc],tfm[c*nc+cc]);
			}
		}

		// Be careful!  pivotRow is already swapped, and pivot is
		// now sitting at (c,n)

		// Pass 3 : Eliminate rest
		double coeff;
		for(r=c+1; r<nr; r++)
		{
			coeff=tfm[r*nc+c]/tfm[c*nc+c];

			inv[r]-=coeff*inv[c];
			for(cc=c; cc<nc; cc++)
			{
				tfm[r*nc+cc]-=coeff*tfm[c*nc+cc];
			}
			tfm[r*nc+c]=0.0;// #### can comment out
		}
	}

	// At this point, lower left half is zero.  Now go ahead and make upper right half zero.
	// And, I know all YsAbs(diagnoal elements) are greater than YsTolerance.

	for(c=1; c<nc; c++)
	{
		for(r=0; r<c; r++)
		{
			double coeff;
			coeff=tfm[r*nc+c]/tfm[c*nc+c];
			inv[r]-=coeff*inv[c];

			for(cc=c; cc<nc; cc++)
			{
				tfm[r*nc+cc]-=coeff*tfm[c*nc+cc];
			}
			tfm[r*nc+c]=0.0;  // #### can comment out
		}
	}

	// Now divide inv(r,c) by tfm(r,r)
	for(r=0; r<nr; r++)
	{
		inv[r]/=tfm[r*nc+r];
	}

	return YSOK;
}

// The following two functions are written in ysmatrix3x3.cpp to cope with VC++'s stupidity.
YSRESULT YsInvertMatrix3x3(double result[],const double from[]);
YSRESULT YsMulInverse3x3(double inv[],const double mat[],const double vec[]);


// The following two functions are written in ysmatrix4x4.cpp to cope with VC++'s stupidity.
YSRESULT YsInvertMatrix4x4(double result[],const double from[]);
YSRESULT YsMulInverse4x4(double inv[],const double mat[],const double vec[]);


////////////////////////////////////////////////////////////

template <const int nRow,const int nColumn>
class YsMatrixTemplate
{
friend class YsMatrix3x3;
friend class YsMatrix4x4;
protected:
	double dat[nRow*nColumn];

public:
	void Create(const double d[]);
	inline const double *GetArray(void) const
	{
		return dat;
	}
	inline const double &v(int row,int column) const;
	inline int nr(void) const
	{
		return nRow;
	}
	inline int nc(void) const
	{
		return nColumn;
	}
	inline YSRESULT Set(int row,int column,const double v);
	inline YSRESULT Add(int row,int column,const double v);
	inline void SetColumn(int column,const double v[]);
	inline void SetRow(int column,const double v[]);

	void LoadIdentity(void);
	void MakeZero(void);

// Right most column and bottom most row will be random
	YSRESULT GetSubMatrix(YsMatrixTemplate <nRow,nRow> &mat,int r,int c) const;

public:
	YSRESULT GetDeterminant(double &det) const;
	YSRESULT GetDeterminantMatrix(YsMatrixTemplate <nRow,nColumn> &mat,int nr) const;
	YSRESULT GetDeterminant(double &det,int nr) const;


	YSRESULT Transpose(void);
	YSRESULT Invert(void);

	YSRESULT SwapRow(int r1,int r2);
	YSRESULT MulRow(int r,double m);
	YSRESULT DivRow(int r,double d);
	YSRESULT Row1MinusRow2Mul(int r1,int r2,double m);  // Never make it const double &
	YSRESULT MulColumn(int c,double m);
	YSRESULT DivColumn(int c,double m);

	void Print(const char *fom=" %10.3lf ") const;

	const YsMatrixTemplate <nRow,nColumn> &operator+=(const YsMatrixTemplate <nRow,nColumn> &);
	const YsMatrixTemplate <nRow,nColumn> &operator*=(const YsMatrixTemplate <nRow,nColumn> &);

	template <const int nColumn2>
	const YsMatrixTemplate <nRow,nColumn2> operator*(const YsMatrixTemplate <nColumn,nColumn2> &vec);

public:
	YSRESULT FindEigenValueEigenVectorByJacobiMethod
	    (YsMatrixTemplate <nRow,nColumn> &value,
	     YsMatrixTemplate <nRow,nColumn> &vector,const double &tolerance) const;
protected:
	YSRESULT JacobiFindAngle
	    (double &cs,double &sn,const double &app,const double &aqq,const double &apq) const;
	YSRESULT JacobiRotate
	    (YsMatrixTemplate <nRow,nColumn> &a,YsMatrixTemplate <nRow,nColumn> &x,
	     int p,int q,const double &cs,const double &sn) const;
};

template <const int nRow,const int nColumn>
inline const double &YsMatrixTemplate <nRow,nColumn>::v(int row,int column) const
{
	if(1<=row && row<=nRow && 1<=column && column<=nColumn)
	{
		return dat[(row-1)*nColumn+(column-1)];
	}
	else
	{
		YsErrOut("YsMatrixTemplate::v()\n");
		YsErrOut("  Index out of range.  (Note : Row/Column begins with 1\n");
		return dat[0];
	}
}

template <const int nRow,const int nColumn>
inline YSRESULT YsMatrixTemplate <nRow,nColumn>::Set(int row,int column,const double a)
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

template <const int nRow,const int nColumn>
inline YSRESULT YsMatrixTemplate <nRow,nColumn>::Add(int row,int column,const double a)
{
	if(1<=row && row<=nRow && 1<=column && column<=nColumn)
	{
		dat[(row-1)*nColumn+(column-1)]+=a;
		return YSOK;
	}
	else
	{
		return YSERR;
	}
}

template <const int nRow,const int nColumn>
inline void YsMatrixTemplate <nRow,nColumn>::SetColumn(int column,const double v[])
{
	if(1<=column && column<=nColumn)
	{
		int i,ptr;
		ptr=(column-1);
		for(i=0; i<nRow; i++)
		{
			dat[ptr]=v[i];
			ptr+=nColumn;
		}
	}
}

template <const int nRow,const int nColumn>
inline void YsMatrixTemplate <nRow,nColumn>::SetRow(int row,const double v[])
{
	if(1<=row && row<=nRow)
	{
		int i;
		double *ptr;   // Removed 'const' credit to Ved.
		ptr=dat+(row-1)*nColumn;
		for(i=0; i<nColumn; i++)
		{
			*ptr=v[i];
			ptr++;
		}
	}
}

template <const int nRow,const int nColumn>
void YsMatrixTemplate <nRow,nColumn>::Create(const double d[])
{
	for(int i=0; i<nRow*nColumn; i++)
	{
		dat[i]=d[i];
	}
}

template <const int nRow,const int nColumn>
void YsMatrixTemplate <nRow,nColumn>::LoadIdentity(void)
{
	int i,j;
	for(i=1; i<=nRow; i++)
	{
		for(j=1; j<=nColumn; j++)
		{
			if(i==j)
			{
				Set(i,j,1.0);
			}
			else
			{
				Set(i,j,0.0);
			}
		}
	}
}

template <const int nRow,const int nColumn>
void YsMatrixTemplate<nRow,nColumn>::MakeZero(void)
{
	for(int i=0; i<nRow*nColumn; ++i)
	{
		dat[i]=0.0;
	}
}

// Right most column and bottom most row will be random
template <const int nRow,const int nColumn>
YSRESULT YsMatrixTemplate <nRow,nColumn>::GetSubMatrix(YsMatrixTemplate <nRow,nRow> &sub,int r,int c) const
{
	int x,y,p,q;

	q=1;
	for(y=1; y<=nRow; y++)
	{
		p=1;
		if(y!=r)
		{
			for(x=1; x<=nColumn; x++)
			{
				if(x!=c)
				{
					sub.Set(q,p,v(y,x));
					p++;
				}
			}
			q++;
		}
	}

	return YSOK;
}

/*
GetDeterminantMatrix
  Template parameters nRow and nColumn is ignored in order to
  instantiate all the template functions at compile-time.
*/
template <const int nRow,const int nColumn>
YSRESULT YsMatrixTemplate <nRow,nColumn>::GetDeterminantMatrix
    (YsMatrixTemplate <nRow,nColumn> &m,int nr) const
{
	int r,c;
	double detSub;
	YsMatrixTemplate <nRow,nColumn> sub;
	static const double modulus[2]={1.0,-1.0};

	for(r=1; r<=nr; r++)
	{
		for(c=1; c<=nr; c++)
		{
			GetSubMatrix(sub,r,c);
			sub.GetDeterminant(detSub,nr-1);
			m.Set(r,c,detSub*modulus[(r+c)&1]);
		}
	}

	m.Transpose();

	return YSOK;
}


template <const int nRow,const int nColumn>
YSRESULT YsMatrixTemplate <nRow,nColumn>::GetDeterminant(double &det,int nr) const
{
	if(nr==3)
	{
		det=v(1,1)*(v(2,2)*v(3,3)-v(2,3)*v(3,2))
		   -v(1,2)*(v(2,1)*v(3,3)-v(2,3)*v(3,1))
		   +v(1,3)*(v(2,1)*v(3,2)-v(2,2)*v(3,1));
	}
	else if(nr==2)
	{
		det=v(1,1)*v(2,2)-v(1,2)*v(2,1);
	}
	else
	{
		int i;
		double sum;
		YsMatrixTemplate <nRow,nColumn> m;
		GetDeterminantMatrix(m,nr);
		sum=0.0;
		for(i=1; i<=nr; i++)
		{
			sum+=m.v(1,i)*v(i,1);
		}
		det=sum;
	}
	return YSOK;
}

template <const int nRow,const int nColumn>
YSRESULT YsMatrixTemplate <nRow,nColumn>::GetDeterminant(double &det) const
{
	if(0==YsCompareTwoConstant <int> (nRow,nColumn))
	{
		return GetDeterminant(det,nRow);
	}
	else
	{
		YsErrOut("YsMatrixTemplate<>::GetDeterminant()\n");
		YsErrOut("  This function is only for square matrix.\n");
		return YSERR;
	}
}

template <const int nRow,const int nColumn>
YSRESULT YsMatrixTemplate <nRow,nColumn>::Transpose(void)
{
	if(0!=YsCompareTwoConstant <int> (nRow,nColumn))
	{
		YsErrOut("YsMatrixTemplate<>::Transpose()\n");
		YsErrOut("  This function is only for square matrix.\n");
		return YSERR;
	}

	int r,c;
	double a,b;
	for(r=1; r<=nRow; r++)
	{
		for(c=r+1; c<=nRow; c++)  // <- Was written as (c=r; c<nRow; c++)  2009/07/13
		{
			a=v(r,c);
			b=v(c,r);
			Set(r,c,b);
			Set(c,r,a);
		}
	}
	return YSOK;
}

template <const int nRow,const int nColumn>
YSRESULT YsMatrixTemplate <nRow,nColumn>::SwapRow(int r1,int r2)
{
	if(1<=r1 && r1<=nRow && 1<=r2 && r2<=nRow)
	{
		double a,b;
		int c;
		for(c=1; c<=nColumn; c++)
		{
			a=v(r1,c);
			b=v(r2,c);
			Set(r1,c,b);
			Set(r2,c,a);
		}
		return YSOK;
	}
	else
	{
		return YSERR;
	}
}

template <const int nRow,const int nColumn>
YSRESULT YsMatrixTemplate <nRow,nColumn>::MulRow(int r,double m)
{
	if(1<=r && r<=nRow)
	{
		int c;
		for(c=1; c<=nColumn; c++)
		{
			Set(r,c,v(r,c)*m);
		}
		return YSOK;
	}
	else
	{
		return YSERR;
	}
}

template <const int nRow,const int nColumn>
YSRESULT YsMatrixTemplate <nRow,nColumn>::DivRow(int r,double m)
{
	if(1<=r && r<=nRow)
	{
		int c;
		for(c=1; c<=nColumn; c++)
		{
			Set(r,c,v(r,c)/m);
		}
		return YSOK;
	}
	else
	{
		return YSERR;
	}
}

template <const int nRow,const int nColumn>
YSRESULT YsMatrixTemplate <nRow,nColumn>::Row1MinusRow2Mul(int r1,int r2,double m)
{
	if(1<=r1 && r1<=nRow && 1<=r2 && r2<=nRow)
	{
		int c;
		for(c=1; c<=nColumn; c++)
		{
			double a;
			a=v(r1,c)-v(r2,c)*m;
			Set(r1,c,a);
		}
		return YSOK;
	}
	else
	{
		return YSERR;
	}
}

template <const int nRow,const int nColumn>
YSRESULT YsMatrixTemplate <nRow,nColumn>::MulColumn(int c,double m)
{
	if(1<=c && c<=nColumn)
	{
		int i;
		for(i=1; i<=nRow; i++)
		{
			Set(i,c,v(i,c)*m);
		}
		return YSOK;
	}
	else
	{
		return YSERR;
	}
}

template <const int nRow,const int nColumn>
YSRESULT YsMatrixTemplate <nRow,nColumn>::DivColumn(int c,double m)
{
	if(1<=c && c<=nColumn)
	{
		int i;
		for(i=1; i<=nRow; i++)
		{
			Set(i,c,v(i,c)/m);
		}
		return YSOK;
	}
	else
	{
		return YSERR;
	}
}

template <const int nRow,const int nColumn>
YSRESULT YsMatrixTemplate <nRow,nColumn>::Invert(void)
{
// I want to simply say YsInvertMatrixNxN <nRow> (dat,dat);
// But, due to Visual C++'s bug (again),
//	return YsInvertMatrixNxN <nRow> (dat,dat);
// I cannot use this way.  What a stupid VC++6!

	YsMatrixTemplate <nRow,nColumn> sync;

	sync.LoadIdentity();

	int n,r;
	double a;
	for(n=1; n<=nColumn; n++)
	{
		// Sweep Out Column n
		for(r=n+1; r<=nRow; r++)
		{
			if(YsAbs(v(n,n))<YsAbs(v(r,n)))
			{
				SwapRow(r,n);
				sync.SwapRow(r,n);
			}
		}

		a=v(n,n);
		if(YsAbs(a)>=YsTolerance)
		{
			DivRow(n,a);
			sync.DivRow(n,a);

			for(r=1; r<=nRow; r++)
			{
				if(r!=n)
				{
					a=v(r,n);
					Row1MinusRow2Mul(r,n,a);
					sync.Row1MinusRow2Mul(r,n,a);
				}
			}
		}
		else
		{
			return YSERR;
		}
	}
	*this=sync;

	return YSOK;
}

template <const int nRow,const int nColumn>
void YsMatrixTemplate <nRow,nColumn>::Print(const char *printFormat) const
{
	for(int r=1; r<=nRow; r++)
	{
		for(int c=1; c<=nColumn; c++)
		{
			YsPrintf(printFormat,v(r,c));
		}
		YsPrintf("\n");
	}
}

template <const int nRow,const int nColumn>
const YsMatrixTemplate <nRow,nColumn> &YsMatrixTemplate <nRow,nColumn>::operator+=
    (const YsMatrixTemplate <nRow,nColumn> &a)
{
	if(this!=&a)
	{
		int r,c;
		for(r=1; r<=nr(); r++)
		{
			for(c=1; c<=nc(); c++)
			{
				Set(r,c,v(r,c)+a.v(r,c));
			}
		}
		return *this;
	}
	else
	{
		int r,c;
		for(r=1; r<=nr(); r++)
		{
			for(c=1; c<=nc(); c++)
			{
				Set(r,c,v(r,c)*2.0);
			}
		}
		return *this;
	}
}

template <const int nRow,const int nColumn>
const YsMatrixTemplate <nRow,nColumn> &YsMatrixTemplate <nRow,nColumn>::operator*=
    (const YsMatrixTemplate <nRow,nColumn> &a)
{
	if(nRow==nColumn)
	{
		int i,j,k;
		YsMatrixTemplate <nRow,nColumn> tmp;
		tmp=*this;
		for(i=1; i<=nRow; i++)
		{
			for(j=1; j<=nColumn; j++)
			{
				double v;
				v=0.0;
				for(k=1; k<=nColumn; k++)
				{
					v+=tmp.v(i,k)*a.v(k,j);
				}
				Set(i,j,v);
			}
		}
		return *this;
	}
	else
	{
		YsErrOut("YsMatrixTemplate <>::operator*=()\n");
		YsErrOut("  This function is limited to square matrices,\n");
		YsErrOut("  Due to apparent bug of Visual C++ 6.0\n");
		return *this;
	}
}

//       yy
//       yy
//       yy
//       yy
// xxxx
// xxxx
// xxxx
// 
// nRow1=3
// nColumn1=4
// 
// nRow2=4=nColumn1
// nColumn2=2
// 
// nRowOut=3=nRow1
// nColumnOut=2=nColumn2
template <const int nRow1,const int nColumn1>
template <const int nColumn2>
const YsMatrixTemplate <nRow1,nColumn2> YsMatrixTemplate<nRow1,nColumn1>::operator*(const YsMatrixTemplate <nColumn1,nColumn2> &vec)
{
	YsMatrixTemplate <nRow1,nColumn2> rhs;
	rhs.MakeZero();

	// const int nRow2=nColumn1;
	const int nRowOut=nRow1;
	const int nColumnOut=nColumn2;
	for(int r=1; r<=nRowOut; ++r)
	{
		for(int c=1; c<=nColumnOut; ++c)
		{
			double sum=0.0;
			for(int s=1; s<=nColumn1; ++s)
			{
				sum+=this->v(r,s)*vec.v(s,c);
			}
			rhs.Set(r,c,sum);
		}
	}
	return rhs;
}

////////////////////////////////////////////////////////////


// Jacobi Method
//    Reference
//    Terrence J. Akai, Applied Numerical Methods for Engineers, John Wiley & Sons, Inc, 1993
//    pp. 282-285

//
// Diagonal Elements of YsMatrixTemplate &value will contain eigen values
// Columns of YsMatrixTemplate &vector will contain eigen vectors of a corresponding eigen value of the same column.

template <const int nRow,const int nColumn>
YSRESULT YsMatrixTemplate <nRow,nColumn>::FindEigenValueEigenVectorByJacobiMethod
    (YsMatrixTemplate <nRow,nColumn> &value,
     YsMatrixTemplate <nRow,nColumn> &vector,const double &tolerance) const
{
	int p,q,n;
	YSBOOL done;
	double cs,sn;

	if(nr()==nc())
	{
		n=nr();

		value=(*this);
		vector.LoadIdentity();

		done=YSFALSE;
		while(done!=YSTRUE)
		{
			done=YSTRUE;
			for(p=1; p<=n-1; p++)
			{
				for(q=p+1; q<=n; q++)
				{
					if(YsAbs(value.v(p,q))>tolerance)
					{
						done=YSFALSE;
						JacobiFindAngle(cs,sn,value.v(p,p),value.v(q,q),value.v(p,q));
						JacobiRotate(value,vector,p,q,cs,sn);
					}
				}
			}
		}
		return YSOK;
	}
	else
	{
		YsErrOut("YsFindEigenValueEigenVectorByJacobiMethod()\n");
		YsErrOut("  The matrix must be square, symmetric.\n");
		return YSERR;
	}
}

template <const int nRow,const int nColumn>
YSRESULT YsMatrixTemplate <nRow,nColumn>::JacobiFindAngle
    (double &cs,double &sn,const double &app,const double &aqq,const double &apq) const
{
	double alpha,delta;

	delta=app-aqq;
	alpha=sqrt(delta*delta+4*apq*apq);
	cs=sqrt((alpha+delta)/(2*alpha));
	sn=apq/(cs*alpha);
	return YSOK;
}

template <const int nRow,const int nColumn>
YSRESULT YsMatrixTemplate <nRow,nColumn>::JacobiRotate
    (YsMatrixTemplate <nRow,nColumn> &a,YsMatrixTemplate <nRow,nColumn> &x,
     int p,int q,const double &cs,const double &sn) const
{
	int k,n;
	double t1,t2;
	n=a.nr();
	for(k=1; k<=n; k++)
	{
		t1=a.v(p,k);
		a.Set(p,k, cs*t1+sn*a.v(q,k));
		a.Set(q,k,-sn*t1+cs*a.v(q,k));
	}
	for(k=1; k<=n; k++)
	{
		t1=a.v(k,p);
		t2=x.v(k,p);

		a.Set(k,p, cs*t1+sn*a.v(k,q));
		a.Set(k,q,-sn*t1+cs*a.v(k,q));
		x.Set(k,p, cs*t2+sn*x.v(k,q));
		x.Set(k,q,-sn*t2+cs*x.v(k,q));
	}
	return YSOK;
}


////////////////////////////////////////////////////////////

template <const int nRow>
inline YSRESULT YsBalanceEquation(YsMatrixTemplate <nRow,nRow> &mat,YsMatrixTemplate <nRow,1> &rhs)
{
	double avg,rowAvg;
	int r,c,n;
	avg=0.0;
	for(r=1; r<=nRow; r++)
	{
		for(c=1; c<=nRow; c++)
		{
			avg+=YsAbs(mat.v(r,c));
		}
	}
	n=nRow*nRow;
	avg/=(double)n;

	// ax+by+cz+d=0  ->  kax+kby+kcz+kd=0

	for(r=1; r<=nRow; r++)
	{
		rowAvg=0.0;
		for(c=1; c<=nRow; c++)
		{
			rowAvg+=YsAbs(mat.v(r,c));
		}
		rowAvg/=(double)nRow;
		if(fabs(rowAvg)<YsTolerance)  // 2009/03/25
		{
			return YSERR;
		}

		for(c=1; c<=nRow; c++)
		{
			mat.Set(r,c,mat.v(r,c)*avg/rowAvg);
		}
		rhs.Set(r,1,rhs.v(r,1)*avg/rowAvg);
	}
	return YSOK;
}

////////////////////////////////////////////////////////////

template <const int nRow,const int nColumn>
inline const YsMatrixTemplate <nRow,nColumn> operator+
     (const YsMatrixTemplate <nRow,nColumn> &a,const YsMatrixTemplate <nRow,nColumn> &b)
{
	YsMatrixTemplate <nRow,nColumn> tmp;

	int r,c;
	for(r=1; r<=a.nr(); r++)
	{
		for(c=1; c<=b.nc(); c++)
		{
			tmp.Set(r,c,a.v(r,c)+b.v(r,c));
		}
	}

	return tmp;
}

template <const int nRow,const int nColumn>
inline const YsMatrixTemplate <nRow,nColumn> operator-
    (const YsMatrixTemplate <nRow,nColumn> &a,const YsMatrixTemplate <nRow,nColumn> &b)
{
	YsMatrixTemplate <nRow,nColumn> tmp;

	int r,c;
	for(r=1; r<=a.nr(); r++)
	{
		for(c=1; c<=b.nc(); c++)
		{
			tmp.Set(r,c,a.v(r,c)-b.v(r,c));
		}
	}
	return tmp;
}

template <const int nRow,const int nColumn>
inline const YsMatrixTemplate <nRow,nColumn> &operator+
    (const YsMatrixTemplate <nRow,nColumn> &a)
{
	return a;
}

template <const int nRow,const int nColumn>
inline const YsMatrixTemplate <nRow,nColumn> operator-
    (const YsMatrixTemplate <nRow,nColumn> &a)
{
	YsMatrixTemplate <nRow,nColumn> tmp;
	int r,c;
	for(r=1; r<=a.nr(); r++)
	{
		for(c=1; c<=a.nc(); c++)
		{
			tmp->Set(r,c,-a.v(r,c));
		}
	}
	return tmp;
}

template <const int nRow>
inline const YsMatrixTemplate <nRow,1> operator^
    (const YsMatrixTemplate <nRow,1> &a,const YsMatrixTemplate <nRow,1> &b)
{
	YsMatrixTemplate <nRow,1> tmp;

	if(a.nr()!=b.nr() || a.nc()!=1 || b.nc()!=1)
	{
		YsErrOut("Outer product is invalid.\n");
	}

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
	tmp.Set(1,1,x);
	tmp.Set(2,1,y);
	tmp.Set(3,1,z);

	if(a.nr()==4)
	{
		tmp.Set(4,1,a.v(4,1));
	}

	return tmp;
}

template <const int nRow,const int nColumn>
inline const YsMatrixTemplate <nRow,nColumn> operator*
    (const YsMatrixTemplate <nRow,nColumn> &a,const double &b)
{
	YsMatrixTemplate <nRow,nColumn> tmp;

	int r,c;
	for(r=1; r<=a.nr(); r++)
	{
		for(c=1; c<=a.nc(); c++)
		{
			tmp.Set(r,c,a.v(r,c)*b);   // '->' to '.'  credit to Ved.
		}
	}
	return tmp;
}

template <const int nRow,const int nColumn>
inline const YsMatrixTemplate <nRow,nColumn> operator*
    (const double &b,const YsMatrixTemplate <nRow,nColumn> &a)
{
	return a*b;
}

template <const int nRow,const int nColumn>
inline const YsMatrixTemplate <nRow,nColumn> operator/
    (const YsMatrixTemplate <nRow,nColumn> &a,const double &b)
{
	YsMatrixTemplate <nRow,nColumn> tmp;

	int r,c;
	for(r=1; r<=a.nr(); r++)
	{
		for(c=1; c<=a.nc(); c++)
		{
			tmp.Set(r,c,a.v(r,c)/b);
		}
	}

	return tmp;
}


template <const int nRowA,const int nColumnA,const int nColumnB> // nRowB==nColumnA
inline const YsMatrixTemplate <nRowA,nColumnB> &YsMulMatrix
    (YsMatrixTemplate <nRowA,nColumnB> &result,
     const YsMatrixTemplate <nRowA,nColumnA> &a,const YsMatrixTemplate <nColumnA,nColumnB> &b)
{
	YsMatrixTemplate <nRowA,nColumnB> *tmp,buf;

	if(((void *)&result)!=((void *)&a) &&
	   ((void *)&result)!=((void *)&b))
	{
		tmp=&result;
	}
	else
	{
		tmp=&buf;
	}

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

	if(((void *)&result)!=((void *)tmp))
	{
		result=(*tmp);
	}

	return result;
}


template <const int nRowA,const int nColumnA,const int nColumnB> // nRowA==nRowB
inline const YsMatrixTemplate <nRowA,nColumnB> &YsMulMatrixTranspose
    (YsMatrixTemplate <nColumnA,nColumnB> &result,
     const YsMatrixTemplate <nRowA,nColumnA> &a,const YsMatrixTemplate <nRowA,nColumnB> &b)
{
	YsMatrixTemplate <nColumnA,nColumnB> *tmp,buf;

	if(((void *)&result)!=((void *)&a) &&
	   ((void *)&result)!=((void *)&b))
	{
		tmp=&result;
	}
	else
	{
		tmp=&buf;
	}

	int aCol,bCol,row;
	for(aCol=1; aCol<=a.nc(); aCol++)
	{
		for(bCol=1; bCol<=b.nc(); bCol++)
		{
			double sum;
			sum=0.0;
			for(row=1; row<=a.nr(); row++)
			{
				sum+=a.v(row,aCol)*b.v(row,bCol);
			}
			tmp->Set(aCol,bCol,sum);
		}
	}

	if(((void *)&result)!=((void *)tmp))
	{
		result=(*tmp);
	}

	return result;
}


inline YsMatrixTemplate <4,4> operator*
    (const YsMatrixTemplate <4,4> &a,const YsMatrixTemplate <4,4> &b)
{
	YsMatrixTemplate <4,4> tmp;
	YsMulMatrix <4,4,4> (tmp,a,b);
	return tmp;
}

inline YsMatrixTemplate <3,3> operator*
    (const YsMatrixTemplate <3,3> &a,const YsMatrixTemplate <3,3> &b)
{
	YsMatrixTemplate <3,3> tmp;
	YsMulMatrix <3,3,3> (tmp,a,b);
	return tmp;
}

inline YsMatrixTemplate <4,1> operator*
    (const YsMatrixTemplate <4,4> &a,const YsMatrixTemplate <4,1> &b)
{
	YsMatrixTemplate <4,1> tmp;
	YsMulMatrix <4,4,1> (tmp,a,b);
	return tmp;
}

inline YsMatrixTemplate <3,1> operator*
    (const YsMatrixTemplate <3,3> &a,const YsMatrixTemplate <3,1> &b)
{
	YsMatrixTemplate <3,1> tmp;
	YsMulMatrix <3,3,1> (tmp,a,b);
	return tmp;
}






/* } */
#endif
