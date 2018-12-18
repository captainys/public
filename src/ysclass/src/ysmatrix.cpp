/* ////////////////////////////////////////////////////////////

File Name: ysmatrix.cpp
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
#include <math.h>

#include "ysmatrix.h"

// 02/14/1999
//   YsMatrix::~YsMatrix
//     delete dat; -> delete [] dat;
//
// 02/14/2000  (What's the occasion!)
//   YsMatrix::CreateWithoutClear
//     delete dat; -> delete [] dat;







// Implementation //////////////////////////////////////////
YsMatrix::YsMatrix()
{
	nRow=0;
	nColumn=0;
	dat=NULL;
	tmp=NULL;
}

YsMatrix::YsMatrix(const YsMatrix &from)
{
	*this=from;
}

YsMatrix::~YsMatrix()
{
	if(tmp!=NULL)
	{
		delete tmp;
	}
	if(dat!=NULL)
	{
		delete [] dat;
		dat=NULL;
	}
}

void YsMatrix::CreateWithoutClear(int nr,int nc)
{
	if(dat==NULL || nRow!=nr || nColumn!=nc)
	{
		if(dat!=NULL)
		{
			delete [] dat;
		}
		nRow=nr;
		nColumn=nc;
		dat=new double[nc*nr];
	}
}

void YsMatrix::Create(int nr,int nc)
{
	CreateWithoutClear(nr,nc);
	for(int i=0; i<nc*nr; i++)
	{
		dat[i]=0.0;
	}
}

void YsMatrix::Create(int nr,int nc,const double *d)
{
	CreateWithoutClear(nr,nc);
	for(int i=0; i<nr*nc; i++)
	{
		dat[i]=d[i];
	}
}

void YsMatrix::LoadIdentity(void)
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

YSRESULT YsMatrix::GetSubMatrix(YsMatrix &sub,int r,int c) const
{
	int x,y,p,q;

	sub.CreateWithoutClear(nRow-1,nColumn-1);
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

YSRESULT YsMatrix::GetDeterminantMatrix(YsMatrix &m) const
{
	int r,c;
	double detSub;
	YsMatrix sub;
	static const double modulus[2]={1.0,-1.0};

	m.CreateWithoutClear(nRow,nColumn);
	for(r=1; r<=nRow; r++)
	{
		for(c=1; c<=nColumn; c++)
		{
			GetSubMatrix(sub,r,c);
			sub.GetDeterminant(detSub);
			m.Set(r,c,detSub*modulus[(r+c)&1]);
		}
	}
	m.Transpose();

	return YSOK;
}

YSRESULT YsMatrix::GetDeterminant(double &det) const
{
	if(nColumn!=nRow)
	{
		return YSERR;
	}

	if(nRow==2)
	{
		det=v(1,1)*v(2,2)-v(1,2)*v(2,1);
	}
	else
	{
		int i;
		double sum;
		YsMatrix m;
		GetDeterminantMatrix(m);
		sum=0.0;
		for(i=1; i<=nRow; i++)
		{
			sum+=m.v(1,i)*v(i,1);
		}
		det=sum;
	}
	return YSOK;
}

YSRESULT YsMatrix::Transpose(void)
{
	if(nRow!=nColumn)
	{
		YsErrOut("YsMatrix::Transpose() doesn't suppose non-square matrix currently.\n");
		return YSERR;
	}
	else
	{
		int r,c;
		double a,b;
		for(r=1; r<=nRow; r++)
		{
			for(c=r; c<=nColumn; c++)
			{
				a=v(r,c);
				b=v(c,r);
				Set(r,c,b);
				Set(c,r,a);
			}
		}
		return YSOK;
	}
}

//YSRESULT YsMatrix::Invert(void)
//{
//	if(nColumn!=nRow)
//	{
//		return YSERR;
//	}
//
//	int i;
//	double det;
//	YsMatrix m;
//	GetDeterminantMatrix(m);
//	det=0.0;
//	for(i=1; i<=nRow; i++)
//	{
//		det+=m.v(1,i)*v(i,1);
//	}
//
//	if(det!=0.0)
//	{
//		int r,c;
//		for(r=1; r<=nRow; r++)
//		{
//			for(c=1; c<=nColumn; c++)
//			{
//				Set(r,c,m.v(r,c)/det);
//			}
//		}
//		return YSOK;
//	}
//	else
//	{
//		return YSERR;
//	}
//}

YSRESULT YsMatrix::SwapRow(int r1,int r2)
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

YSRESULT YsMatrix::MulRow(int r,double m)
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

YSRESULT YsMatrix::DivRow(int r,double m)
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

YSRESULT YsMatrix::Row1MinusRow2Mul(int r1,int r2,double m)
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

YSRESULT YsMatrix::Invert(void)
{
	YsMatrix sync;

	sync.Create(nRow,nColumn);
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

void YsMatrix::Print(const char *printFormat) const
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

const YsMatrix &YsMatrix::operator+=(const YsMatrix &a)
{
	if(this!=&a)
	{
		if(nr()==a.nr() && nc()==a.nc())
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
			YsErrOut("YsMatirx::operator+=\n  Number of rows and/or columns doesn't match.\n");
			return *this;
		}
	}
	else
	{
		int r,c;
		for(r=1; r<=nr(); r++)
		{
			for(c=1; c<=nc(); c++)
			{
				Set(r,c,v(r,c)*2);
			}
		}
		return *this;
	}
}

const YsMatrix &YsMatrix::operator=(const YsMatrix &a)
{
	if(this!=&a)
	{
		Create(a.nRow,a.nColumn,a.dat);
	}
	return *this;
}


YsMatrix *YsMatrix::UseTemporaryMatrix(void) const
{
	if(tmp!=NULL)
	{
		return tmp;  // Direct Recycle. Good for environment?
	}
	else
	{
		tmp=new YsMatrix;
		if(tmp==NULL)
		{
			YsErrOut("YsMatrix::UseTemporaryMatrix(void)\n");
			YsErrOut("Low memory warning!!!\n");
		}
		return tmp;
	}
}

//
//int main(void)
//{
//	double mat[5*5]=
//	{
//		 0, 0, 3, 0, 8,
//		 1, 0, 1, 0, 0,
//		 0, 1, 0, 1,-1,
//		 0,-8, 0,-8,-1,
//		 0, 0, 0, 0, 1
//	};
//	double cst[5]=
//	{
//		0,0,0,0,2600
//	};
//	YsMatrix m1,m2,c,a;
//
//	m1.Create(5,5,mat);
//	m2=m1;
//	m2.InvertBySweepingOut();
//	c.Create(5,1,cst);
//	a=m2*c;
//	a.Print();
//
//	return 0;
//}



////////////////////////////////////////////////////////////

YSRESULT YsLUDecomposition(YsMatrix &l,YsMatrix &u,const YsMatrix &from)
{
	int r,c;

	if(from.nr()==from.nc())
	{
		u=from;
		l.Create(from.nr(),from.nc());

		for(c=1; c<=u.nc(); c++)
		{
			l.Set(c,c,1.0);
		}

		for(c=1; c<u.nc(); c++)  // (not c<=u.nc() because right bottom end element doesn't have to be eliminated)
		{
			if(YsAbs(u.v(c,c))>YsTolerance)
			{
				for(r=c+1; r<=u.nr(); r++)
				{
					double coffee;
					coffee=u.v(r,c)/u.v(c,c);
					l.Set(r,c,coffee);
					u.Row1MinusRow2Mul(r,c,coffee);
				}
			}
			else
			{
				YsErrOut("YsLUDecomposition()\n");
				YsErrOut("  Cannot find the decomposition.\n");
				YsErrOut("  Original Matrix's (%d,%d)=0.0\n",c,c);
				return YSERR;
			}
		}
		return YSOK;
	}
	else
	{
		YsErrOut("YsLUDecomposition()\n");
		YsErrOut("  The original matrix is not a square matrix.\n");
		return YSERR;
	}
}

// Note : If from is non-symmetric or non-positive-definite,
//        the result will be wrong. This should be checked by an application.
YSRESULT YsCholeskyDecomposition(YsMatrix &l,const YsMatrix &from)
{
	YsMatrix L,U,D;

	if(YsLUDecomposition(L,U,from)==YSOK)
	{
		int r;

		D.Create(from.nr(),from.nc());
		for(r=1; r<=from.nr(); r++)
		{
			D.Set(r,r,sqrt(U.v(r,r)));
		}

		l=L*D;

		return YSOK;
	}
	else
	{
		YsErrOut("YsCholeskyDecomposition()\n");
		YsErrOut("  Cannot find LUDecomposition.\n");
		return YSERR;
	}
}



////////////////////////////////////////////////////////////


// Jacobi Method
//    Reference
//    Terrence J. Akai, Applied Numerical Methods for Engineers, John Wiley & Sons, Inc, 1993
//    pp. 282-285

//
// Diagonal Elements of YsMatrix &value will contain eigen values
// Columns of YsMatrix &vector will contain eigen vectors of a corresponding eigen value of the same column.

static YSRESULT YsJacobiFindAngle(double &cs,double &sn,const double &app,const double &aqq,const double &apq);
static YSRESULT YsJacobiRotate(YsMatrix &a,YsMatrix &x,int p,int q,const double &cs,const double &sn);

YSRESULT YsFindEigenValueEigenVectorByJacobiMethod
    (YsMatrix &value,YsMatrix &vector,const YsMatrix &from,const double &tolerance)
{
	int p,q,n;
	YSBOOL done;
	double cs,sn;

	if(from.nr()==from.nc())
	{
		n=from.nr();

		value=from;
		vector.Create(value.nr(),value.nc());
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
						YsJacobiFindAngle(cs,sn,value.v(p,p),value.v(q,q),value.v(p,q));
						YsJacobiRotate(value,vector,p,q,cs,sn);
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

static YSRESULT YsJacobiFindAngle(double &cs,double &sn,const double &app,const double &aqq,const double &apq)
{
	double alpha,delta;

	delta=app-aqq;
	alpha=sqrt(delta*delta+4*apq*apq);
	cs=sqrt((alpha+delta)/(2*alpha));
	sn=apq/(cs*alpha);
	return YSOK;
}

static YSRESULT YsJacobiRotate(YsMatrix &a,YsMatrix &x,int p,int q,const double &cs,const double &sn)
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

YSRESULT YsInvertMatrix2x2(double result[4],const double from[4])
{
	double *inv,buf[4],tfm[4];
	const int i11=0,i12=1,i21=2,i22=3;

	if(result!=from)
	{
		inv=result;
	}
	else
	{
		inv=buf;
	}

	tfm[i11]=from[i11];
	tfm[i12]=from[i12];
	tfm[i21]=from[i21];
	tfm[i22]=from[i22];

	inv[i11]=1.0;
	inv[i12]=0.0;
	inv[i21]=0.0;
	inv[i22]=1.0;

	if(YsAbs(from[i11])<YsAbs(from[i21]))
	{
		YsSwapDouble(tfm[i11],tfm[i21]);
		YsSwapDouble(tfm[i12],tfm[i22]);
		YsSwapDouble(inv[i11],inv[i21]);
		YsSwapDouble(inv[i12],inv[i22]);
	}

	if(YsAbs(tfm[i11])<YsTolerance)
	{
		return YSERR;
	}

	double c;

	c=tfm[i21]/tfm[i11];
	tfm[i21] =0.0;
	tfm[i22]-=tfm[i12]*c;
	inv[i21]-=inv[i11]*c;
	inv[i22]-=inv[i12]*c;

	c=tfm[i11];
	// tfm[i11]=1.0;        // (1,1) is alreadydone.
	tfm[i12]/=c;
	inv[i11]/=c;
	inv[i12]/=c;

	if(YsAbs(tfm[i22])<YsTolerance)
	{
		return YSERR;
	}

	c=tfm[i12]/tfm[i22];
	// tfm[i11]-=tfm[i21]*c;   (Theoretically, but don't need)
	// tfm[i12]-=tfm[i22]*c;   (Theoretically, but don't need)
	inv[i11]-=inv[i21]*c;
	inv[i12]-=inv[i22]*c;

	c=tfm[i22];
	inv[i21]/=c;
	inv[i22]/=c;

	if(inv!=result)
	{
		result[0]=inv[0];
		result[1]=inv[1];
		result[2]=inv[2];
		result[3]=inv[3];
	}

	return YSOK;
}


