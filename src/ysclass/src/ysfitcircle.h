/* ////////////////////////////////////////////////////////////

File Name: ysfitcircle.h
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

#ifndef YSFITCIRCLE_IS_INCLUDED
#define YSFITCIRCLE_IS_INCLUDED
/* { */

#include "ysgeometry.h"


/*! Finds a least-square fitting circle.
    Class T must be a storage of YsVec2.

    Based on Dale Umbach and Kerry N. Jones, 
    "A Few Methods for Fitting Circle to Data", 
    IEEE Transactions on Instrumentation and Measurement, vol.52, issue 6, pp. 1881-1885, 2003
*/
template <class T>
YSRESULT YsFindLeastSquareFittingCircle(YsVec2 &cen,double &rad,const T &bunchOfVec2)
{
	int n=0;
	double xxSum=0.0;
	double xSum=0.0;
	double yySum=0.0;
	double ySum=0.0;
	double xySum=0.0;
	double xyySum=0.0;
	double xxySum=0.0;
	double xxxSum=0.0;
	double yyySum=0.0;

	for(auto v : bunchOfVec2)
	{
		const double x=v.x();
		const double y=v.y();
		const double xx=x*x;
		const double xy=x*y;
		const double yy=y*y;
		const double xyy=x*yy;
		const double xxy=xx*y;
		const double xxx=xx*x;
		const double yyy=yy*y;

		xSum+=x;
		ySum+=y;

		xxSum+=xx;
		yySum+=yy;
		xySum+=xy;

		xxxSum+=xxx;
		yyySum+=yyy;
		xyySum+=xyy;
		xxySum+=xxy;

		++n;
	}


	double nd=(double)n;
	double A=nd*xxSum-xSum*xSum;
	double B=nd*xySum-xSum*ySum;
	double C=nd*yySum-ySum*ySum;
	double D=0.5*(nd*xyySum-xSum*yySum+nd*xxxSum-xSum*xxSum);
	double E=0.5*(nd*xxySum-ySum*xxSum+nd*yyySum-ySum*yySum);

	double denom=A*C-B*B;
	if(YsTolerance<fabs(denom))
	{
		double aM=(D*C-B*E)/denom;
		double bM=(A*E-B*D)/denom;
		cen.Set(aM,bM);

		double radSum=0.0;
		for(auto v : bunchOfVec2)
		{
			radSum+=(cen-v).GetLength();
		}
		rad=radSum/nd;

		return YSOK;
	}
	return YSERR;
}


/* } */
#endif
