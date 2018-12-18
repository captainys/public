/* ////////////////////////////////////////////////////////////

File Name: ysinterpolation.cpp
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

#include "ysinterpolation.h"



/* This function returns intepolation weights for the parameter s,t within a unit square.
   It is derived from my note from finite-element method course that I took long time ago.

   For the values v1,v2,v3,and v4 assigned at the corners of a unit cube, the interpolated value is
   calculated as a weighted sum of wi*vi.

   Y
   |
   v2------v3
   |        |
   |        |
   |        |
   |        |
   v0------v1->X

*/
void YsGetBiLinearInterpolationWeight(double w[4],const double s,const double t)
{
	const double S=(1-s);
	const double T=(1-t);

	w[0]=S*T;
	w[1]=s*T;
	w[2]=S*t;
	w[3]=s*t;
}



/*! This function returns interpolation weights for the parameter s,t,u within a unit cube.
    It is derived from my note from finite-element method course that I took long time ago.
    I cannot tell which of 5 multiplications + 16 additions and 3 subtractions + 12 multiplications is faster.
    I have a feeling that additions is faster than multiplications, but probably it also depends on the processor.

    For the values v0,v1,...,v7 assigned to the corners of a unit cube, the interpolated value for (s,t,u) is a
    weighted sum of vi*wi.



        Y
        |
        v2--------v3
       /|         /|
      / |        / |
     /  |       /  |
    v6---------v7  |
    |   v0-----|--v1->X
    |  /       |  /
    | /        | /
    |/         |/
    v4---------v5
   /
  Z
 

 w0 (0,0,0)
 w1 (1,0,0)
 w2 (0,1,0)
 w3 (1,1,0)
 w4 (0,0,1)
 w5 (1,0,1)
 w6 (0,1,1)
 w7 (1,1,1)
*/
void YsGetTriLiniearInterpolationWeight(double w[],const double s,const double t,const double u)
{
	// 5 multiplications + 16 additions

	const double st=s*t;
	const double su=s*u;
	const double tu=t*u;
	const double stu=s*t*u;

	const double tumstu=tu-stu;
	const double stpsu=st+su;

	w[0]=1.0-s-t-u+stpsu+tumstu;  // (1-s)*(1-t)*(1-u)
	w[1]=    s    -stpsu   +stu;  // s*(1-t)*(1-u)
	w[2]=      t  -st   -tumstu;  // (1-s)*t*(1-u)
	w[3]=          st      -stu;  // s*t*(1-u)
	w[4]=       +u   -su-tumstu;  // (1-s)*(1-t)*u
	w[5]=            +su   -stu;  // s*(1-t)*u
	w[6]=                tumstu;  // (1-s)*t*u
	w[7]=                   stu;  // s*t*u

/* Is this faster?
	// 3 subtractions +12 multiplications

	const double S=(1.0-s);
	const double T=(1.0-t);
	const double U=(1.0-u);

	const double SmT=S*T;
	const double smt=s*t;
	const double smT=s*T;
	const double Smt=S*t;

	w[0]=SmT*U;
	w[1]=smT*U;
	w[2]=Smt*U;
	w[3]=smt*U;
	w[4]=SmT*u;
	w[5]=smT*u;
	w[6]=Smt*u;
	w[7]=smt*u;
*/
}



YsVec3 YsParameterize(const YsVec3 &pos,const YsVec3 minmax[2])
{
	const auto d=minmax[1]-minmax[0];
	const auto f=pos-minmax[0];
	return YsVec3(f.x()/d.x(),f.y()/d.y(),f.z()/d.z());
}

YsVec2 YsParameterize(const YsVec2 &pos,const YsVec2 minmax[2])
{
	const auto d=minmax[1]-minmax[0];
	const auto f=pos-minmax[0];
	return YsVec2(f.x()/d.x(),f.y()/d.y());
}

