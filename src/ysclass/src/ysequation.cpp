/* ////////////////////////////////////////////////////////////

File Name: ysequation.cpp
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

#include "ysequation.h"

#include <stdio.h>
#include "ysmatrix.h"


/*
  ax+by+c=0;
  px+qy+r=0;

  |a   b||x| |-c|
  |     || |=|  |
  |p   q||y| |-r|


  Inverted
   |ia  ib|     1  | q  -b|
   |      | = -----|      |
   |ic  id|   aq-bp|-p   a|


   |x| |ia  ib||-c|
   | |=|      ||  |
   |y| |ic  id||-r|

*/


/* ax+by+c=0 */
/* px+qy+r=0 */
YSRESULT YsSolveTwoLinearEquation
    (double *x,double *y,
     const double &a,const double &b,const double &c,
     const double &p,const double &q,const double &r)
{
	double inv[4],mat[4];
	mat[0]=a;
	mat[1]=b;
	mat[2]=p;
	mat[3]=q;
	if(YsInvertMatrix2x2(inv,mat)==YSOK)
	{
		*x=-c*inv[0]-r*inv[1];
		*y=-c*inv[2]-r*inv[3];
		return YSOK;
	}
	return YSERR;

/*
	double inv;
	double ia,ib,ic,id;

	inv=a*q-b*p;
	if(inv==0)
	{
		return YSERR;
	}

	ia= q/inv;
	ib=-b/inv;
	ic=-p/inv;
	id= a/inv;

	*x=-c*ia-r*ib;
	*y=-c*ic-r*id;
	return YSOK;
*/
}



/*
  ax+by+cz+d=0 defines a plane. (x,y,z) from 3 equations can be calculated
  as a crossing point of 3 planes.
*/
YSRESULT YsSolveThreeLinearEquation
    (double *x,double *y,double *z,
     const double &a1,const double &b1,const double &c1,const double &d1,
     const double &a2,const double &b2,const double &c2,const double &d2,
     const double &a3,const double &b3,const double &c3,const double &d3)
{
	double inv[9],mat[9];
	mat[0]=a1;
	mat[1]=b1;
	mat[2]=c1;
	mat[3]=a2;
	mat[4]=b2;
	mat[5]=c2;
	mat[6]=a3;
	mat[7]=b3;
	mat[8]=c3;
	if(YsInvertMatrixNxN <3> (inv,mat)==YSOK)
	{
		*x=-d1*inv[0]-d2*inv[1]-d3*inv[2];
		*y=-d1*inv[3]-d2*inv[4]-d3*inv[5];
		*z=-d1*inv[6]-d2*inv[7]-d3*inv[8];
		return YSOK;
	}
	return YSERR;
}


