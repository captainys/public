/* ////////////////////////////////////////////////////////////

File Name: ysairfoil_lens.cpp
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

#include <ysclass.h>
#include "ysairfoil.h"



void AirfoilLens::Generate(int numDivision,const double c,const double h1,const double b1,const double h2,const double b2)
{
	const double hh[2]={h1,h2};
	const double bb[2]={b1,b2};

	YsArray <YsVec3> curve[2];

	double s=1.0;
	for(int i=0; i<2; ++i)
	{
		const double h=hh[i];
		const double r=((c/2.0)*(c/2.0)+h*h)/(2.0*h);

		const double fanAngle=2.0*atan2(c/2.0,r-h);

		YsVec2 o(c/2.0,h-r);

		// Bias
		// y=A*x*x+Bx+C
		//   Passes through (0,0),(0.5,0.5-b),(1.0,1.0)
		//     (0,0)=> C=0
		//     (0.5,0.5-b) => 0.5-b=0.25*A+0.5B  => B=1-2b-0.5A
		//     (1,1)=> 1=A+B
		//               => 1=A+1-2b-0.5A => 0.5A=2b  => A=4b
		//               => B=1-4b
		const double b=bb[i];
		const double A=4.0*b;
		const double B=1.0-4.0*b;

		curve[i].Append(YsOrigin());
		for(int j=1; j<numDivision-1; ++j)
		{
			const double t=(double)j/(double)(numDivision-1);
			const double rotAngle=fanAngle*t;
			YsVec2 v(-c/2.0,r-h);
			v.Rotate(-rotAngle);
			v+=o;

			const double Tin=v.x()/c;
			const double Tout=A*Tin*Tin+B*Tin;

			curve[i].Append(YsVec3(c*Tout,s*v.y(),0.0));
		}
		curve[i].Append(YsVec3(c,0.0,0.0));
		s=-s;
	}


	frontIdx=0;

	point.CleanUp();
	for(int i=0; i<curve[0].GetN(); ++i)
	{
		point.Append(YsVec2(curve[0][i].x(),curve[0][i].y()));
	}

	backIdx=numDivision-1;

	for(auto i=curve[1].GetN()-1; 1<=i; --i)
	{
		point.Append(YsVec2(curve[1][i].x(),curve[1][i].y()));
	}
}

