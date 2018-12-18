/* ////////////////////////////////////////////////////////////

File Name: ysglmath.c
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

#include "ysglmath.h"
#include <math.h>

void YsGLMakePerspectivefv(GLfloat mat[16],const double fovy,const double aspect,const double nearz,const double farz)
{
	const GLfloat f=(GLfloat)(1.0/tan(fovy));

	// Based on the formula listed in www.opengl.org
	// See also glMultMatrix reference.  In OpenGL world, matrices are stored in column-first order.
	mat[0]=(float)(f/aspect); mat[4]=0;        mat[ 8]=0;                                  mat[12]=0;
	mat[1]=0;                 mat[5]=(float)f; mat[ 9]=0;                                  mat[13]=0;
	mat[2]=0;                 mat[6]=0;        mat[10]=(float)((farz+nearz)/(nearz-farz)); mat[14]=(float)((2.0*farz*nearz)/(nearz-farz));
	mat[3]=0;                 mat[7]=0;        mat[11]=-1;                                 mat[15]=0;
}

void YsGLMakeFrustum(GLfloat mat[16],GLfloat l,GLfloat r,GLfloat b,GLfloat t,GLfloat n,GLfloat f)
{
	mat[0]=2.0f*n/(r-l); mat[4]=0;            mat[ 8]=(r+l)/(r-l);  mat[12]=0;
	mat[1]=0;            mat[5]=2.0f*n/(t-b); mat[ 9]=(t+b)/(t-b);  mat[13]=0;
	mat[2]=0;            mat[6]=0;            mat[10]=-(f+n)/(f-n); mat[14]=-2.0f*f*n/(f-n);
	mat[3]=0;            mat[7]=0;            mat[11]=-1;           mat[15]=0;
}

void YsGLMakeOrthographicfv(
    GLfloat mat[16],
    const double left,const double right,const double bottom,const double top,const double nearz,const double farz)
{
	const double tx=-(right+left)/(right-left);
	const double ty=-(top+bottom)/(top-bottom);
	const double tz=-(farz+nearz)/(farz-nearz);

	// Based on the formula listed in www.opengl.org
	mat[0]=(float)(2.0/(right-left)); mat[4]=0;                         mat[ 8]=0;                          mat[12]=(GLfloat)tx;
	mat[1]=0;                         mat[5]=(float)(2.0/(top-bottom)); mat[ 9]=0;                          mat[13]=(GLfloat)ty;
	mat[2]=0;                         mat[6]=0;                         mat[10]=(float)(-2.0/(farz-nearz)); mat[14]=(GLfloat)tz;
	mat[3]=0;                         mat[7]=0;                         mat[11]=0;                          mat[15]=1;
}

void YsGLMakeIdentityfv(GLfloat mat[16])
{
	mat[0]=1; mat[4]=0; mat[ 8]=0; mat[12]=0;
	mat[1]=0; mat[5]=1; mat[ 9]=0; mat[13]=0;
	mat[2]=0; mat[6]=0; mat[10]=1; mat[14]=0;
	mat[3]=0; mat[7]=0; mat[11]=0; mat[15]=01;
}

void YsGLMakeScalingfv(GLfloat mat[16],const double sx,const double sy,const double sz)
{
	mat[0]=(GLfloat)sx;mat[4]=0;          mat[ 8]=0;          mat[12]=0;
	mat[1]=0;          mat[5]=(GLfloat)sy;mat[ 9]=0;          mat[13]=0;
	mat[2]=0;          mat[6]=0;          mat[10]=(GLfloat)sz;mat[14]=0;
	mat[3]=0;          mat[7]=0;          mat[11]=0;          mat[15]=01;
}

void YsGLMakeTranslationfv(GLfloat mat[16],const double tx,const double ty,const double tz)
{
	mat[0]=1; mat[4]=0; mat[ 8]=0; mat[12]=(GLfloat)tx;
	mat[1]=0; mat[5]=1; mat[ 9]=0; mat[13]=(GLfloat)ty;
	mat[2]=0; mat[6]=0; mat[10]=1; mat[14]=(GLfloat)tz;
	mat[3]=0; mat[7]=0; mat[11]=0; mat[15]=1;
}

void YsGLMakeXYRotationfv(GLfloat mat[16],const double ang)
{
	const GLfloat c=(GLfloat)cos(ang);
	const GLfloat s=(GLfloat)sin(ang);

	mat[0]= c; mat[4]=-s; mat[ 8]= 0; mat[12]=0;
	mat[1]= s; mat[5]= c; mat[ 9]= 0; mat[13]=0;
	mat[2]= 0; mat[6]= 0; mat[10]= 1; mat[14]=0;
	mat[3]= 0; mat[7]= 0; mat[11]= 0; mat[15]=1;
}

void YsGLMakeYXRotationfv(GLfloat mat[16],const double ang)
{
	YsGLMakeXYRotationfv(mat,-ang);
}

void YsGLMakeXZRotationfv(GLfloat mat[16],const double ang)
{
	const GLfloat c=(GLfloat)cos(ang);
	const GLfloat s=(GLfloat)sin(ang);

	mat[0]= c; mat[4]= 0; mat[ 8]=-s; mat[12]=0;
	mat[1]= 0; mat[5]= 1; mat[ 9]= 0; mat[13]=0;
	mat[2]= s; mat[6]= 0; mat[10]= c; mat[14]=0;
	mat[3]= 0; mat[7]= 0; mat[11]= 0; mat[15]=1;
}

void YsGLMakeZXRotationfv(GLfloat mat[16],const double ang)
{
	YsGLMakeXZRotationfv(mat,-ang);
}

void YsGLMakeYZRotationfv(GLfloat mat[16],const double ang)
{
	const GLfloat c=(GLfloat)cos(ang);
	const GLfloat s=(GLfloat)sin(ang);

	mat[0]= 1; mat[4]= 0; mat[ 8]= 0; mat[12]=0;
	mat[1]= 0; mat[5]= c; mat[ 9]=-s; mat[13]=0;
	mat[2]= 0; mat[6]= s; mat[10]= c; mat[14]=0;
	mat[3]= 0; mat[7]= 0; mat[11]= 0; mat[15]=1;
}

void YsGLMakeZYRotationfv(GLfloat mat[16],const double ang)
{
	YsGLMakeYZRotationfv(mat,-ang);
}

void YsGLCopyMatrixfv(GLfloat dst[16],const GLfloat src[16])
{
	int i;
	for(i=0; i<16; ++i)
	{
		dst[i]=src[i];
	}
}

void YsGLMultMatrixfv(GLfloat mat[16],const GLfloat a[16],const GLfloat b[16])
{
    //                          b[0]   b[4]   b[ 8]   b[12]
    //                          b[1]   b[5]   b[ 9]   b[13]
	//                          b[2]   b[6]   b[10]   b[14]
	//                          b[3]   b[7]   b[11]   b[15]
	// a[0] a[4] a[ 8] a[12]   mat[0] mat[4] mat[ 8] mat[12]
	// a[1] a[5] a[ 9] a[13]   mat[1] mat[5] mat[ 9] mat[13]
	// a[2] a[6] a[10] a[14]   mat[2] mat[6] mat[10] mat[14]
	// a[3] a[7] a[11] a[15]   mat[3] mat[7] mat[11] mat[15]

	if(mat!=a && mat!=b)
	{
		mat[ 0]=a[ 0]*b[ 0]+a[ 4]*b[ 1]+a[ 8]*b[ 2]+a[12]*b[ 3];
		mat[ 1]=a[ 1]*b[ 0]+a[ 5]*b[ 1]+a[ 9]*b[ 2]+a[13]*b[ 3];
		mat[ 2]=a[ 2]*b[ 0]+a[ 6]*b[ 1]+a[10]*b[ 2]+a[14]*b[ 3];
		mat[ 3]=a[ 3]*b[ 0]+a[ 7]*b[ 1]+a[11]*b[ 2]+a[15]*b[ 3];

		mat[ 4]=a[ 0]*b[ 4]+a[ 4]*b[ 5]+a[ 8]*b[ 6]+a[12]*b[ 7];
		mat[ 5]=a[ 1]*b[ 4]+a[ 5]*b[ 5]+a[ 9]*b[ 6]+a[13]*b[ 7];
		mat[ 6]=a[ 2]*b[ 4]+a[ 6]*b[ 5]+a[10]*b[ 6]+a[14]*b[ 7];
		mat[ 7]=a[ 3]*b[ 4]+a[ 7]*b[ 5]+a[11]*b[ 6]+a[15]*b[ 7];

		mat[ 8]=a[ 0]*b[ 8]+a[ 4]*b[ 9]+a[ 8]*b[10]+a[12]*b[11];
		mat[ 9]=a[ 1]*b[ 8]+a[ 5]*b[ 9]+a[ 9]*b[10]+a[13]*b[11];
		mat[10]=a[ 2]*b[ 8]+a[ 6]*b[ 9]+a[10]*b[10]+a[14]*b[11];
		mat[11]=a[ 3]*b[ 8]+a[ 7]*b[ 9]+a[11]*b[10]+a[15]*b[11];

		mat[12]=a[ 0]*b[12]+a[ 4]*b[13]+a[ 8]*b[14]+a[12]*b[15];
		mat[13]=a[ 1]*b[12]+a[ 5]*b[13]+a[ 9]*b[14]+a[13]*b[15];
		mat[14]=a[ 2]*b[12]+a[ 6]*b[13]+a[10]*b[14]+a[14]*b[15];
		mat[15]=a[ 3]*b[12]+a[ 7]*b[13]+a[11]*b[14]+a[15]*b[15];
	}
	else
	{
		GLfloat mm[16];
		mm[ 0]=a[ 0]*b[ 0]+a[ 4]*b[ 1]+a[ 8]*b[ 2]+a[12]*b[ 3];
		mm[ 1]=a[ 1]*b[ 0]+a[ 5]*b[ 1]+a[ 9]*b[ 2]+a[13]*b[ 3];
		mm[ 2]=a[ 2]*b[ 0]+a[ 6]*b[ 1]+a[10]*b[ 2]+a[14]*b[ 3];
		mm[ 3]=a[ 3]*b[ 0]+a[ 7]*b[ 1]+a[11]*b[ 2]+a[15]*b[ 3];

		mm[ 4]=a[ 0]*b[ 4]+a[ 4]*b[ 5]+a[ 8]*b[ 6]+a[12]*b[ 7];
		mm[ 5]=a[ 1]*b[ 4]+a[ 5]*b[ 5]+a[ 9]*b[ 6]+a[13]*b[ 7];
		mm[ 6]=a[ 2]*b[ 4]+a[ 6]*b[ 5]+a[10]*b[ 6]+a[14]*b[ 7];
		mm[ 7]=a[ 3]*b[ 4]+a[ 7]*b[ 5]+a[11]*b[ 6]+a[15]*b[ 7];

		mm[ 8]=a[ 0]*b[ 8]+a[ 4]*b[ 9]+a[ 8]*b[10]+a[12]*b[11];
		mm[ 9]=a[ 1]*b[ 8]+a[ 5]*b[ 9]+a[ 9]*b[10]+a[13]*b[11];
		mm[10]=a[ 2]*b[ 8]+a[ 6]*b[ 9]+a[10]*b[10]+a[14]*b[11];
		mm[11]=a[ 3]*b[ 8]+a[ 7]*b[ 9]+a[11]*b[10]+a[15]*b[11];

		mm[12]=a[ 0]*b[12]+a[ 4]*b[13]+a[ 8]*b[14]+a[12]*b[15];
		mm[13]=a[ 1]*b[12]+a[ 5]*b[13]+a[ 9]*b[14]+a[13]*b[15];
		mm[14]=a[ 2]*b[12]+a[ 6]*b[13]+a[10]*b[14]+a[14]*b[15];
		mm[15]=a[ 3]*b[12]+a[ 7]*b[13]+a[11]*b[14]+a[15]*b[15];

		mat[ 0]=mm[ 0];
		mat[ 1]=mm[ 1];
		mat[ 2]=mm[ 2];
		mat[ 3]=mm[ 3];
		mat[ 4]=mm[ 4];
		mat[ 5]=mm[ 5];
		mat[ 6]=mm[ 6];
		mat[ 7]=mm[ 7];
		mat[ 8]=mm[ 8];
		mat[ 9]=mm[ 9];
		mat[10]=mm[10];
		mat[11]=mm[11];
		mat[12]=mm[12];
		mat[13]=mm[13];
		mat[14]=mm[14];
		mat[15]=mm[15];
	}
}

static void YsSwapFloatC(float *a,float *b)
{
	float c=*a;
	*a=*b;
	*b=c;
}

int YsGLInvertMatrixfv(GLfloat outMat[16],const GLfloat inMat[16])
{
	const int nr=4;
	const int nc=4;
	GLfloat *inv,buf[16],tfm[16];
	int i,r,c,cc;
	if(outMat!=inMat)
	{
		inv=outMat;
	}
	else
	{
		inv=buf;
	}

	i=0;
	for(r=0; r<nr; r++)
	{
		for(c=0; c<nc; c++)
		{
			inv[i]=(r==c ? 1.0f : 0.0f);
			tfm[i]=inMat[i];
			i++;
		}
	}

	for(c=0; c<nc; c++)
	{
		float coeff;
		int pivotRow,pivotIdx,testIdx;
		pivotRow=c;       // Initially r==c
		pivotIdx=c*nc+c;

		// Pass 1 : Find pivot
		for(r=c+1; r<nr; r++)
		{
			testIdx=c*nr+r; // <-> (Row-first) r*nc+c;
			if(fabs(tfm[testIdx])>fabs(tfm[pivotIdx]))
			{
				pivotRow=r;
				pivotIdx=testIdx;
			}
		}

		if(fabs(tfm[pivotIdx])<YSGLTOL)
		{
			return 0;
		}

		// Pass 2 : Swap row
		if(pivotRow!=c)
		{
			for(cc=0; cc<c; cc++)
			{
				YsSwapFloatC(&inv[pivotRow+cc*nr],&inv[c+cc*nr]);
			}
			for(cc=c; cc<nc; cc++)
			{
				YsSwapFloatC(&tfm[pivotRow+cc*nr],&tfm[c+cc*nr]);
				YsSwapFloatC(&inv[pivotRow+cc*nr],&inv[c+cc*nr]);
			}
		}

		// Be careful!  pivotRow is already swapped, and pivot is
		// now sitting at (c,n)

		// Pass 3 : Eliminate rest
		for(r=c+1; r<nr; r++)
		{
			coeff=tfm[r+c*nr]/tfm[c+c*nr];

			for(cc=0; cc<c; cc++)
			{
				// Don't need to compute tfm.  They are all zero for 0<=cc<c
				// tfm[r*nc+cc]-=coeff*tfm[c*nc+cc];
				inv[r+cc*nr]-=coeff*inv[c+cc*nr];
			}
			for(cc=c; cc<nc; cc++)
			{
				tfm[r+cc*nr]-=coeff*tfm[c+cc*nr];
				inv[r+cc*nr]-=coeff*inv[c+cc*nr];
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
			float coeff;
			coeff=tfm[r+c*nr]/tfm[c+c*nr];
			for(cc=0; cc<c; cc++)
			{
				inv[r+cc*nr]-=coeff*inv[c+cc*nr];
			}
			for(cc=c; cc<nc; cc++)
			{
				tfm[r+cc*nr]-=coeff*tfm[c+cc*nr];
				inv[r+cc*nr]-=coeff*inv[c+cc*nr];
			}
			// tfm[r+c*nr]=0.0;
		}
	}

	// Now divide inv(r,c) by tfm(r,r)
	for(r=0; r<nr; r++)
	{
		for(c=0; c<nc; c++)
		{
			inv[r+c*nr]/=tfm[r+r*nr];
		}
	}


	if(outMat==inMat)
	{
		int i;
		for(i=0; i<nr*nc; i++)
		{
			outMat[i]=inv[i];
		}
	}

	return 1;
}

void YsGLMultMatrixTranslationfv(GLfloat mat[16],GLfloat tx,GLfloat ty,GLfloat tz)
{
    //                           1      0       0     tx
    //                           0      1       0     ty
	//                           0      0       1     tz
	//                           0      0       0     1
	// a[0] a[4] a[ 8] a[12]   mat[0] mat[4] mat[ 8] mat[12]
	// a[1] a[5] a[ 9] a[13]   mat[1] mat[5] mat[ 9] mat[13]
	// a[2] a[6] a[10] a[14]   mat[2] mat[6] mat[10] mat[14]
	// a[3] a[7] a[11] a[15]   mat[3] mat[7] mat[11] mat[15]
	mat[12]=mat[0]*tx+mat[4]*ty+mat[ 8]*tz+mat[12];
	mat[13]=mat[1]*tx+mat[5]*ty+mat[ 9]*tz+mat[13];
	mat[14]=mat[2]*tx+mat[6]*ty+mat[10]*tz+mat[14];
	mat[15]=mat[3]*tx+mat[7]*ty+mat[11]*tz+mat[15];
}

void YsGLMultMatrixRotationXYfv(GLfloat mat[16],GLfloat angle)
{
	//                            c       -s       0       0
	//                            s        c       0       0
	//                            0        0       1       0
	//                            0        0       0       1
    // a[0] a[4] a[ 8] a[12]    mat[0] mat[4] mat[ 8] mat[12]
    // a[1] a[5] a[ 9] a[13]    mat[1] mat[5] mat[ 9] mat[13]
    // a[2] a[6] a[10] a[14]    mat[2] mat[6] mat[10] mat[14]
    // a[3] a[7] a[11] a[15]    mat[3] mat[7] mat[11] mat[15]
	const GLfloat c=cosf(angle);
	const GLfloat s=sinf(angle);

	GLfloat m0,m1,m2,m3;

	m0=c*mat[0]+s*mat[4];
	m1=c*mat[1]+s*mat[5];
	m2=c*mat[2]+s*mat[6];
	m3=c*mat[3]+s*mat[7];

	mat[4]=-s*mat[0]+c*mat[4];
	mat[5]=-s*mat[1]+c*mat[5];
	mat[6]=-s*mat[2]+c*mat[6];
	mat[7]=-s*mat[3]+c*mat[7];

	mat[0]=m0;
	mat[1]=m1;
	mat[2]=m2;
	mat[3]=m3;
}

void YsGLMultMatrixRotationYXfv(GLfloat mat[16],GLfloat angle)
{
	YsGLMultMatrixRotationXYfv(mat,-angle);
}

void YsGLMultMatrixRotationXZfv(GLfloat mat[16],GLfloat angle)
{
	//                            c        0      -s       0
	//                            0        1       0       0
	//                            s        0       c       0
	//                            0        0       0       1
    // a[0] a[4] a[ 8] a[12]    mat[0] mat[4] mat[ 8] mat[12]
    // a[1] a[5] a[ 9] a[13]    mat[1] mat[5] mat[ 9] mat[13]
    // a[2] a[6] a[10] a[14]    mat[2] mat[6] mat[10] mat[14]
    // a[3] a[7] a[11] a[15]    mat[3] mat[7] mat[11] mat[15]
	const GLfloat c=cosf(angle);
	const GLfloat s=sinf(angle);

	GLfloat m0,m1,m2,m3;

	m0=c*mat[0]+s*mat[ 8];
	m1=c*mat[1]+s*mat[ 9];
	m2=c*mat[2]+s*mat[10];
	m3=c*mat[3]+s*mat[11];

	mat[ 8]=-s*mat[0]+c*mat[ 8];
	mat[ 9]=-s*mat[1]+c*mat[ 9];
	mat[10]=-s*mat[2]+c*mat[10];
	mat[11]=-s*mat[3]+c*mat[11];

	mat[0]=m0;
	mat[1]=m1;
	mat[2]=m2;
	mat[3]=m3;
}

void YsGLMultMatrixRotationZXfv(GLfloat mat[16],GLfloat angle)
{
	YsGLMultMatrixRotationXZfv(mat,-angle);
}

void YsGLMultMatrixRotationYZfv(GLfloat mat[16],GLfloat angle)
{
	//                            1        0       0       0
	//                            0        c      -s       0
	//                            0        s       c       0
	//                            0        0       0       1
    // a[0] a[4] a[ 8] a[12]    mat[0] mat[4] mat[ 8] mat[12]
    // a[1] a[5] a[ 9] a[13]    mat[1] mat[5] mat[ 9] mat[13]
    // a[2] a[6] a[10] a[14]    mat[2] mat[6] mat[10] mat[14]
    // a[3] a[7] a[11] a[15]    mat[3] mat[7] mat[11] mat[15]
	const GLfloat c=cosf(angle);
	const GLfloat s=sinf(angle);

	GLfloat m4,m5,m6,m7;

	m4=c*mat[4]+s*mat[ 8];
	m5=c*mat[5]+s*mat[ 9];
	m6=c*mat[6]+s*mat[10];
	m7=c*mat[7]+s*mat[11];

	mat[ 8]=-s*mat[4]+c*mat[ 8];
	mat[ 9]=-s*mat[5]+c*mat[ 9];
	mat[10]=-s*mat[6]+c*mat[10];
	mat[11]=-s*mat[7]+c*mat[11];

	mat[4]=m4;
	mat[5]=m5;
	mat[6]=m6;
	mat[7]=m7;
}

void YsGLMultMatrixRotationZYfv(GLfloat mat[16],GLfloat angle)
{
	YsGLMultMatrixRotationYZfv(mat,-angle);
}

void YsGLMultMatrixScalingfv(GLfloat mat[16],GLfloat sx,GLfloat sy,GLfloat sz)
{
    //                           sx     0       0     0
    //                           0      sy      0     0
	//                           0      0       sz    0
	//                           0      0       0     1
	// a[0] a[4] a[ 8] a[12]   mat[0] mat[4] mat[ 8] mat[12]
	// a[1] a[5] a[ 9] a[13]   mat[1] mat[5] mat[ 9] mat[13]
	// a[2] a[6] a[10] a[14]   mat[2] mat[6] mat[10] mat[14]
	// a[3] a[7] a[11] a[15]   mat[3] mat[7] mat[11] mat[15]
	mat[ 0]*=sx;
	mat[ 1]*=sx;
	mat[ 2]*=sx;
	mat[ 3]*=sx;
	mat[ 4]*=sy;
	mat[ 5]*=sy;
	mat[ 6]*=sy;
	mat[ 7]*=sy;
	mat[ 8]*=sz;
	mat[ 9]*=sz;
	mat[10]*=sz;
	mat[11]*=sz;
}

void YsGLMultMatrixVectorfv(GLfloat vecOut[4],const GLfloat mat[16],const GLfloat vecIn[4])
{
	if(vecIn!=vecOut)
	{
		vecOut[ 0]=mat[ 0]*vecIn[ 0]+mat[ 4]*vecIn[ 1]+mat[ 8]*vecIn[ 2]+mat[12]*vecIn[ 3];
		vecOut[ 1]=mat[ 1]*vecIn[ 0]+mat[ 5]*vecIn[ 1]+mat[ 9]*vecIn[ 2]+mat[13]*vecIn[ 3];
		vecOut[ 2]=mat[ 2]*vecIn[ 0]+mat[ 6]*vecIn[ 1]+mat[10]*vecIn[ 2]+mat[14]*vecIn[ 3];
		vecOut[ 3]=mat[ 3]*vecIn[ 0]+mat[ 7]*vecIn[ 1]+mat[11]*vecIn[ 2]+mat[15]*vecIn[ 3];
	}
	else
	{
		GLfloat o[4];
		o[ 0]=mat[ 0]*vecIn[ 0]+mat[ 4]*vecIn[ 1]+mat[ 8]*vecIn[ 2]+mat[12]*vecIn[ 3];
		o[ 1]=mat[ 1]*vecIn[ 0]+mat[ 5]*vecIn[ 1]+mat[ 9]*vecIn[ 2]+mat[13]*vecIn[ 3];
		o[ 2]=mat[ 2]*vecIn[ 0]+mat[ 6]*vecIn[ 1]+mat[10]*vecIn[ 2]+mat[14]*vecIn[ 3];
		o[ 3]=mat[ 3]*vecIn[ 0]+mat[ 7]*vecIn[ 1]+mat[11]*vecIn[ 2]+mat[15]*vecIn[ 3];
		vecOut[0]=o[0];
		vecOut[1]=o[1];
		vecOut[2]=o[2];
		vecOut[3]=o[3];
	}
}

void YsGLMultMatrixVectordv(GLdouble vecOut[4],const GLdouble mat[16],const GLdouble vecIn[4])
{
	if(vecIn!=vecOut)
	{
		vecOut[ 0]=mat[ 0]*vecIn[ 0]+mat[ 4]*vecIn[ 1]+mat[ 8]*vecIn[ 2]+mat[12]*vecIn[ 3];
		vecOut[ 1]=mat[ 1]*vecIn[ 0]+mat[ 5]*vecIn[ 1]+mat[ 9]*vecIn[ 2]+mat[13]*vecIn[ 3];
		vecOut[ 2]=mat[ 2]*vecIn[ 0]+mat[ 6]*vecIn[ 1]+mat[10]*vecIn[ 2]+mat[14]*vecIn[ 3];
		vecOut[ 3]=mat[ 3]*vecIn[ 0]+mat[ 7]*vecIn[ 1]+mat[11]*vecIn[ 2]+mat[15]*vecIn[ 3];
	}
	else
	{
		GLdouble o[4];
		o[ 0]=mat[ 0]*vecIn[ 0]+mat[ 4]*vecIn[ 1]+mat[ 8]*vecIn[ 2]+mat[12]*vecIn[ 3];
		o[ 1]=mat[ 1]*vecIn[ 0]+mat[ 5]*vecIn[ 1]+mat[ 9]*vecIn[ 2]+mat[13]*vecIn[ 3];
		o[ 2]=mat[ 2]*vecIn[ 0]+mat[ 6]*vecIn[ 1]+mat[10]*vecIn[ 2]+mat[14]*vecIn[ 3];
		o[ 3]=mat[ 3]*vecIn[ 0]+mat[ 7]*vecIn[ 1]+mat[11]*vecIn[ 2]+mat[15]*vecIn[ 3];
		vecOut[0]=o[0];
		vecOut[1]=o[1];
		vecOut[2]=o[2];
		vecOut[3]=o[3];
	}
}

void YsGLInvertZfv(GLfloat mat[16])
{
	// {1 0  0 0} {mat[0] mat[4] mat[ 8] mat[12]}   { mat[0]  mat[4]  mat[ 8]  mat[12]}
	// {0 1  0 0} {mat[1] mat[5] mat[ 9] mat[13]} = { mat[1]  mat[5]  mat[ 9]  mat[13]}
	// {0 0 -1 0} {mat[2] mat[6] mat[10] mat[14]}   {-mat[2] -mat[6] -mat[10] -mat[14]}
	// {0 0  0 1} {mat[3] mat[7] mat[11] mat[15]}   { mat[3]  mat[7]  mat[11]  mat[15]}
	mat[ 2]=-mat[ 2];
	mat[ 6]=-mat[ 6];
	mat[10]=-mat[10];
	mat[14]=-mat[14];
}

void YsGLInvertZdv(GLdouble mat[16])
{
	// {1 0  0 0} {mat[0] mat[4] mat[ 8] mat[12]}   { mat[0]  mat[4]  mat[ 8]  mat[12]}
	// {0 1  0 0} {mat[1] mat[5] mat[ 9] mat[13]} = { mat[1]  mat[5]  mat[ 9]  mat[13]}
	// {0 0 -1 0} {mat[2] mat[6] mat[10] mat[14]}   {-mat[2] -mat[6] -mat[10] -mat[14]}
	// {0 0  0 1} {mat[3] mat[7] mat[11] mat[15]}   { mat[3]  mat[7]  mat[11]  mat[15]}
	mat[ 2]=-mat[ 2];
	mat[ 6]=-mat[ 6];
	mat[10]=-mat[10];
	mat[14]=-mat[14];
}

void YsGLGetArbitraryPerpendicularVectorfv(GLfloat outVec[3],const GLfloat fromVec[3])
{
	const float x=fromVec[0],y=fromVec[1],z=fromVec[2];
	float ox,oy,oz;

	if(fabs(z)<fabs(y) && fabs(z)<fabs(z))
	{
		ox=-y;
		oy= x;
		oz= 0;
	}
	else if(fabs(y)<fabs(x))
	{
		ox=-z;
		oy= 0;
		oz= x;
	}
	else
	{
		ox= 0;
		oy=-z;
		oz= y;
	}

	{
		float l=(float)sqrt(ox*ox+oy*oy+oz*oz);

		outVec[0]=ox/l;
		outVec[1]=oy/l;
		outVec[2]=oz/l;
	}
}

void YsGLCrossProductfv(GLfloat outVec[3],const GLfloat v1[3],const GLfloat v2[3])
{
	const float newValue[3]=
	{
		v1[1]*v2[2]-v1[2]*v2[1],
		v1[2]*v2[0]-v1[0]*v2[2],
		v1[0]*v2[1]-v1[1]*v2[0]
	};
	outVec[0]=newValue[0];
	outVec[1]=newValue[1];
	outVec[2]=newValue[2];
}

